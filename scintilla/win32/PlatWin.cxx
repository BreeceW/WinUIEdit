// Scintilla source code edit control
/** @file PlatWin.cxx
 ** Implementation of platform facilities on Windows.
 **/
// Copyright 1998-2003 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <cmath>
#include <climits>

#include <string_view>
#include <vector>
#include <map>
#include <optional>
#include <algorithm>
#include <iterator>
#include <memory>
#include <mutex>

// Want to use std::min and std::max so don't want Windows.h version of min and max
#if !defined(NOMINMAX)
#define NOMINMAX
#endif
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#undef WINVER
#define WINVER 0x0500
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#include <commctrl.h>
#include <richedit.h>
#include <windowsx.h>

#if !defined(DISABLE_D2D)
#define USE_D2D 1
#endif

#if defined(USE_D2D)
#include <d2d1.h>
#include <dwrite.h>
#endif

#include "ScintillaTypes.h"

#include "Debugging.h"
#include "Geometry.h"
#include "Platform.h"
#include "XPM.h"
#include "UniConversion.h"
#include "DBCS.h"

#include "WinTypes.h"
#include "PlatWin.h"

#ifndef SPI_GETFONTSMOOTHINGCONTRAST
#define SPI_GETFONTSMOOTHINGCONTRAST	0x200C
#endif

#ifndef LOAD_LIBRARY_SEARCH_SYSTEM32
#define LOAD_LIBRARY_SEARCH_SYSTEM32 0x00000800
#endif

// __uuidof is a Microsoft extension but makes COM code neater, so disable warning
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wlanguage-extension-token"
#endif

using namespace Scintilla;

namespace Scintilla::Internal {

UINT CodePageFromCharSet(CharacterSet characterSet, UINT documentCodePage) noexcept;

#if defined(USE_D2D)
IDWriteFactory *pIDWriteFactory = nullptr;
ID2D1Factory *pD2DFactory = nullptr;
IDWriteRenderingParams *defaultRenderingParams = nullptr;
IDWriteRenderingParams *customClearTypeRenderingParams = nullptr;
D2D1_DRAW_TEXT_OPTIONS d2dDrawTextOptions = D2D1_DRAW_TEXT_OPTIONS_NONE;

static HMODULE hDLLD2D {};
static HMODULE hDLLDWrite {};

void LoadD2DOnce() noexcept {
	DWORD loadLibraryFlags = 0;
	HMODULE kernel32 = ::GetModuleHandleW(L"kernel32.dll");
	if (kernel32) {
		if (::GetProcAddress(kernel32, "SetDefaultDllDirectories")) {
			// Availability of SetDefaultDllDirectories implies Windows 8+ or
			// that KB2533623 has been installed so LoadLibraryEx can be called
			// with LOAD_LIBRARY_SEARCH_SYSTEM32.
			loadLibraryFlags = LOAD_LIBRARY_SEARCH_SYSTEM32;
		}
	}

	typedef HRESULT (WINAPI *D2D1CFSig)(D2D1_FACTORY_TYPE factoryType, REFIID riid,
		CONST D2D1_FACTORY_OPTIONS *pFactoryOptions, IUnknown **factory);
	typedef HRESULT (WINAPI *DWriteCFSig)(DWRITE_FACTORY_TYPE factoryType, REFIID iid,
		IUnknown **factory);

	hDLLD2D = ::LoadLibraryEx(TEXT("D2D1.DLL"), 0, loadLibraryFlags);
	D2D1CFSig fnD2DCF = DLLFunction<D2D1CFSig>(hDLLD2D, "D2D1CreateFactory");
	if (fnD2DCF) {
		// A single threaded factory as Scintilla always draw on the GUI thread
		fnD2DCF(D2D1_FACTORY_TYPE_SINGLE_THREADED,
			__uuidof(ID2D1Factory),
			nullptr,
			reinterpret_cast<IUnknown**>(&pD2DFactory));
	}
	hDLLDWrite = ::LoadLibraryEx(TEXT("DWRITE.DLL"), 0, loadLibraryFlags);
	DWriteCFSig fnDWCF = DLLFunction<DWriteCFSig>(hDLLDWrite, "DWriteCreateFactory");
	if (fnDWCF) {
		const GUID IID_IDWriteFactory2 = // 0439fc60-ca44-4994-8dee-3a9af7b732ec
		{ 0x0439fc60, 0xca44, 0x4994, { 0x8d, 0xee, 0x3a, 0x9a, 0xf7, 0xb7, 0x32, 0xec } };

		const HRESULT hr = fnDWCF(DWRITE_FACTORY_TYPE_SHARED,
			IID_IDWriteFactory2,
			reinterpret_cast<IUnknown**>(&pIDWriteFactory));
		if (SUCCEEDED(hr)) {
			// D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT
			d2dDrawTextOptions = static_cast<D2D1_DRAW_TEXT_OPTIONS>(0x00000004);
		} else {
			fnDWCF(DWRITE_FACTORY_TYPE_SHARED,
				__uuidof(IDWriteFactory),
				reinterpret_cast<IUnknown**>(&pIDWriteFactory));
		}
	}

	if (pIDWriteFactory) {
		const HRESULT hr = pIDWriteFactory->CreateRenderingParams(&defaultRenderingParams);
		if (SUCCEEDED(hr)) {
			unsigned int clearTypeContrast = 0;
			if (::SystemParametersInfo(SPI_GETFONTSMOOTHINGCONTRAST, 0, &clearTypeContrast, 0)) {

				FLOAT gamma;
				if (clearTypeContrast >= 1000 && clearTypeContrast <= 2200)
					gamma = static_cast<FLOAT>(clearTypeContrast) / 1000.0f;
				else
					gamma = defaultRenderingParams->GetGamma();

				pIDWriteFactory->CreateCustomRenderingParams(gamma, defaultRenderingParams->GetEnhancedContrast(), defaultRenderingParams->GetClearTypeLevel(),
					defaultRenderingParams->GetPixelGeometry(), defaultRenderingParams->GetRenderingMode(), &customClearTypeRenderingParams);
			}
		}
	}
}

bool LoadD2D() {
	static std::once_flag once;
	std::call_once(once, LoadD2DOnce);
	return pIDWriteFactory && pD2DFactory;
}

#endif

#ifndef CLEARTYPE_QUALITY
#define CLEARTYPE_QUALITY 5
#endif

void *PointerFromWindow(HWND hWnd) noexcept {
	return reinterpret_cast<void *>(::GetWindowLongPtr(hWnd, 0));
}

void SetWindowPointer(HWND hWnd, void *ptr) noexcept {
	::SetWindowLongPtr(hWnd, 0, reinterpret_cast<LONG_PTR>(ptr));
}

namespace {

// system DPI, same for all monitor.
UINT uSystemDPI = USER_DEFAULT_SCREEN_DPI;

using GetDpiForWindowSig = UINT(WINAPI *)(HWND hwnd);
GetDpiForWindowSig fnGetDpiForWindow = nullptr;

HMODULE hDLLShcore {};
using GetDpiForMonitorSig = HRESULT (WINAPI *)(HMONITOR hmonitor, /*MONITOR_DPI_TYPE*/int dpiType, UINT *dpiX, UINT *dpiY);
GetDpiForMonitorSig fnGetDpiForMonitor = nullptr;

using GetSystemMetricsForDpiSig = int(WINAPI *)(int nIndex, UINT dpi);
GetSystemMetricsForDpiSig fnGetSystemMetricsForDpi = nullptr;

using AdjustWindowRectExForDpiSig = BOOL(WINAPI *)(LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle, UINT dpi);
AdjustWindowRectExForDpiSig fnAdjustWindowRectExForDpi = nullptr;

void LoadDpiForWindow() noexcept {
	HMODULE user32 = ::GetModuleHandleW(L"user32.dll");
	fnGetDpiForWindow = DLLFunction<GetDpiForWindowSig>(user32, "GetDpiForWindow");
	fnGetSystemMetricsForDpi = DLLFunction<GetSystemMetricsForDpiSig>(user32, "GetSystemMetricsForDpi");
	fnAdjustWindowRectExForDpi = DLLFunction<AdjustWindowRectExForDpiSig>(user32, "AdjustWindowRectExForDpi");

	using GetDpiForSystemSig = UINT(WINAPI *)(void);
	GetDpiForSystemSig fnGetDpiForSystem = DLLFunction<GetDpiForSystemSig>(user32, "GetDpiForSystem");
	if (fnGetDpiForSystem) {
		uSystemDPI = fnGetDpiForSystem();
	} else {
		HDC hdcMeasure = ::CreateCompatibleDC({});
		uSystemDPI = ::GetDeviceCaps(hdcMeasure, LOGPIXELSY);
		::DeleteDC(hdcMeasure);
	}

	if (!fnGetDpiForWindow) {
		hDLLShcore = ::LoadLibraryExW(L"shcore.dll", {}, LOAD_LIBRARY_SEARCH_SYSTEM32);
		if (hDLLShcore) {
			fnGetDpiForMonitor = DLLFunction<GetDpiForMonitorSig>(hDLLShcore, "GetDpiForMonitor");
		}
	}
}

HINSTANCE hinstPlatformRes {};

const Supports SupportsGDI[] = {
	Supports::PixelModification,
};

constexpr BYTE Win32MapFontQuality(FontQuality extraFontFlag) noexcept {
	switch (extraFontFlag & FontQuality::QualityMask) {

		case FontQuality::QualityNonAntialiased:
			return NONANTIALIASED_QUALITY;

		case FontQuality::QualityAntialiased:
			return ANTIALIASED_QUALITY;

		case FontQuality::QualityLcdOptimized:
			return CLEARTYPE_QUALITY;

		default:
			return DEFAULT_QUALITY;
	}
}

#if defined(USE_D2D)
constexpr D2D1_TEXT_ANTIALIAS_MODE DWriteMapFontQuality(FontQuality extraFontFlag) noexcept {
	switch (extraFontFlag & FontQuality::QualityMask) {

		case FontQuality::QualityNonAntialiased:
			return D2D1_TEXT_ANTIALIAS_MODE_ALIASED;

		case FontQuality::QualityAntialiased:
			return D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE;

		case FontQuality::QualityLcdOptimized:
			return D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE;

		default:
			return D2D1_TEXT_ANTIALIAS_MODE_DEFAULT;
	}
}
#endif

// Both GDI and DirectWrite can produce a HFONT for use in list boxes
struct FontWin : public Font {
	virtual HFONT HFont() const noexcept = 0;
};

void SetLogFont(LOGFONTW &lf, const char *faceName, CharacterSet characterSet, XYPOSITION size, FontWeight weight, bool italic, FontQuality extraFontFlag) {
	lf = LOGFONTW();
	// The negative is to allow for leading
	lf.lfHeight = -(std::abs(std::lround(size)));
	lf.lfWeight = static_cast<LONG>(weight);
	lf.lfItalic = italic ? 1 : 0;
	lf.lfCharSet = static_cast<BYTE>(characterSet);
	lf.lfQuality = Win32MapFontQuality(extraFontFlag);
	UTF16FromUTF8(faceName, lf.lfFaceName, LF_FACESIZE);
}

struct FontGDI : public FontWin {
	HFONT hfont = {};
	FontGDI(const FontParameters &fp) {
		LOGFONTW lf;
		SetLogFont(lf, fp.faceName, fp.characterSet, fp.size, fp.weight, fp.italic, fp.extraFontFlag);
		hfont = ::CreateFontIndirectW(&lf);
	}
	// Deleted so FontGDI objects can not be copied.
	FontGDI(const FontGDI &) = delete;
	FontGDI(FontGDI &&) = delete;
	FontGDI &operator=(const FontGDI &) = delete;
	FontGDI &operator=(FontGDI &&) = delete;
	~FontGDI() noexcept override {
		if (hfont)
			::DeleteObject(hfont);
	}
	HFONT HFont() const noexcept override {
		// Duplicating hfont
		LOGFONTW lf = {};
		if (0 == ::GetObjectW(hfont, sizeof(lf), &lf)) {
			return {};
		}
		return ::CreateFontIndirectW(&lf);
	}
};

#if defined(USE_D2D)
struct FontDirectWrite : public FontWin {
	IDWriteTextFormat *pTextFormat = nullptr;
	FontQuality extraFontFlag = FontQuality::QualityDefault;
	CharacterSet characterSet = CharacterSet::Ansi;
	FLOAT yAscent = 2.0f;
	FLOAT yDescent = 1.0f;
	FLOAT yInternalLeading = 0.0f;

	FontDirectWrite(const FontParameters &fp) :
		extraFontFlag(fp.extraFontFlag),
		characterSet(fp.characterSet) {
		const std::wstring wsFace = WStringFromUTF8(fp.faceName);
		const std::wstring wsLocale = WStringFromUTF8(fp.localeName);
		const FLOAT fHeight = static_cast<FLOAT>(fp.size);
		const DWRITE_FONT_STYLE style = fp.italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL;
		HRESULT hr = pIDWriteFactory->CreateTextFormat(wsFace.c_str(), nullptr,
			static_cast<DWRITE_FONT_WEIGHT>(fp.weight),
			style,
			DWRITE_FONT_STRETCH_NORMAL, fHeight, wsLocale.c_str(), &pTextFormat);
		if (hr == E_INVALIDARG) {
			// Possibly a bad locale name like "/" so try "en-us".
			hr = pIDWriteFactory->CreateTextFormat(wsFace.c_str(), nullptr,
				static_cast<DWRITE_FONT_WEIGHT>(fp.weight),
				style,
				DWRITE_FONT_STRETCH_NORMAL, fHeight, L"en-us", &pTextFormat);
		}
		if (SUCCEEDED(hr)) {
			pTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

			IDWriteTextLayout *pTextLayout = nullptr;
			hr = pIDWriteFactory->CreateTextLayout(L"X", 1, pTextFormat,
					100.0f, 100.0f, &pTextLayout);
			if (SUCCEEDED(hr) && pTextLayout) {
				constexpr int maxLines = 2;
				DWRITE_LINE_METRICS lineMetrics[maxLines]{};
				UINT32 lineCount = 0;
				hr = pTextLayout->GetLineMetrics(lineMetrics, maxLines, &lineCount);
				if (SUCCEEDED(hr)) {
					yAscent = lineMetrics[0].baseline;
					yDescent = lineMetrics[0].height - lineMetrics[0].baseline;

					FLOAT emHeight;
					hr = pTextLayout->GetFontSize(0, &emHeight);
					if (SUCCEEDED(hr)) {
						yInternalLeading = lineMetrics[0].height - emHeight;
					}
				}
				ReleaseUnknown(pTextLayout);
				pTextFormat->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_UNIFORM, lineMetrics[0].height, lineMetrics[0].baseline);
			}
		}
	}
	// Deleted so FontDirectWrite objects can not be copied.
	FontDirectWrite(const FontDirectWrite &) = delete;
	FontDirectWrite(FontDirectWrite &&) = delete;
	FontDirectWrite &operator=(const FontDirectWrite &) = delete;
	FontDirectWrite &operator=(FontDirectWrite &&) = delete;
	~FontDirectWrite() noexcept override {
		ReleaseUnknown(pTextFormat);
	}
	HFONT HFont() const noexcept override {
		LOGFONTW lf = {};
		const HRESULT hr = pTextFormat->GetFontFamilyName(lf.lfFaceName, LF_FACESIZE);
		if (!SUCCEEDED(hr)) {
			return {};
		}
		lf.lfWeight = pTextFormat->GetFontWeight();
		lf.lfItalic = pTextFormat->GetFontStyle() == DWRITE_FONT_STYLE_ITALIC;
		lf.lfHeight = -static_cast<int>(pTextFormat->GetFontSize());
		return ::CreateFontIndirectW(&lf);
	}
};
#endif

}

std::shared_ptr<Font> Font::Allocate(const FontParameters &fp) {
#if defined(USE_D2D)
	return std::make_shared<FontDirectWrite>(fp);
#endif
}

// Buffer to hold strings and string position arrays without always allocating on heap.
// May sometimes have string too long to allocate on stack. So use a fixed stack-allocated buffer
// when less than safe size otherwise allocate on heap and free automatically.
template<typename T, int lengthStandard>
class VarBuffer {
	T bufferStandard[lengthStandard];
public:
	T *buffer;
	explicit VarBuffer(size_t length) : buffer(nullptr) {
		if (length > lengthStandard) {
			buffer = new T[length];
		} else {
			buffer = bufferStandard;
		}
	}
	// Deleted so VarBuffer objects can not be copied.
	VarBuffer(const VarBuffer &) = delete;
	VarBuffer(VarBuffer &&) = delete;
	VarBuffer &operator=(const VarBuffer &) = delete;
	VarBuffer &operator=(VarBuffer &&) = delete;

	~VarBuffer() noexcept {
		if (buffer != bufferStandard) {
			delete []buffer;
			buffer = nullptr;
		}
	}
};

constexpr int stackBufferLength = 400;
class TextWide : public VarBuffer<wchar_t, stackBufferLength> {
public:
	int tlen;	// Using int instead of size_t as most Win32 APIs take int.
	TextWide(std::string_view text, int codePage) :
		VarBuffer<wchar_t, stackBufferLength>(text.length()) {
		if (codePage == CpUtf8) {
			tlen = static_cast<int>(UTF16FromUTF8(text, buffer, text.length()));
		} else {
			// Support Asian string display in 9x English
			tlen = ::MultiByteToWideChar(codePage, 0, text.data(), static_cast<int>(text.length()),
				buffer, static_cast<int>(text.length()));
		}
	}
};
typedef VarBuffer<XYPOSITION, stackBufferLength> TextPositions;

UINT DpiForWindow(WindowID wid) noexcept {
	if (fnGetDpiForWindow) {
		return fnGetDpiForWindow(HwndFromWindowID(wid));
	}
	if (fnGetDpiForMonitor) {
		HMONITOR hMonitor = ::MonitorFromWindow(HwndFromWindowID(wid), MONITOR_DEFAULTTONEAREST);
		UINT dpiX = 0;
		UINT dpiY = 0;
		if (fnGetDpiForMonitor(hMonitor, 0 /*MDT_EFFECTIVE_DPI*/, &dpiX, &dpiY) == S_OK) {
			return dpiY;
		}
	}
	return uSystemDPI;
}

int SystemMetricsForDpi(int nIndex, UINT dpi) noexcept {
	if (fnGetSystemMetricsForDpi) {
		return fnGetSystemMetricsForDpi(nIndex, dpi);
	}

	int value = ::GetSystemMetrics(nIndex);
	value = (dpi == uSystemDPI) ? value : ::MulDiv(value, dpi, uSystemDPI);
	return value;
}

#if defined(USE_D2D)

namespace {

constexpr D2D1_RECT_F RectangleFromPRectangle(PRectangle rc) noexcept {
	return {
		static_cast<FLOAT>(rc.left),
		static_cast<FLOAT>(rc.top),
		static_cast<FLOAT>(rc.right),
		static_cast<FLOAT>(rc.bottom)
	};
}

constexpr D2D1_POINT_2F DPointFromPoint(Point point) noexcept {
	return { static_cast<FLOAT>(point.x), static_cast<FLOAT>(point.y) };
}

const Supports SupportsD2D[] = {
	Supports::LineDrawsFinal,
	Supports::FractionalStrokeWidth,
	Supports::TranslucentStroke,
	Supports::PixelModification,
};

constexpr D2D_COLOR_F ColorFromColourAlpha(ColourRGBA colour) noexcept {
	return D2D_COLOR_F{
		colour.GetRedComponent(),
		colour.GetGreenComponent(),
		colour.GetBlueComponent(),
		colour.GetAlphaComponent()
	};
}

constexpr D2D1_RECT_F RectangleInset(D2D1_RECT_F rect, FLOAT inset) noexcept {
	return D2D1_RECT_F{
		rect.left + inset,
		rect.top + inset,
		rect.right - inset,
		rect.bottom - inset };
}

}

class BlobInline;

class SurfaceD2D : public Surface {
	SurfaceMode mode;

	int codePageText = 0;

	ID2D1RenderTarget *pRenderTarget = nullptr;
	ID2D1BitmapRenderTarget *pBitmapRenderTarget = nullptr;
	bool ownRenderTarget = false;
	int clipsActive = 0;

	IDWriteTextFormat *pTextFormat = nullptr;
	FLOAT yAscent = 2;
	FLOAT yDescent = 1;
	FLOAT yInternalLeading = 0;

	ID2D1SolidColorBrush *pBrush = nullptr;

	int logPixelsY = USER_DEFAULT_SCREEN_DPI;

	void Clear() noexcept;
	void SetFont(const Font *font_);
	HRESULT GetBitmap(ID2D1Bitmap **ppBitmap);

public:
	SurfaceD2D() noexcept;
	SurfaceD2D(ID2D1RenderTarget *pRenderTargetCompatible, int width, int height, SurfaceMode mode_, int logPixelsY_) noexcept;
	// Deleted so SurfaceD2D objects can not be copied.
	SurfaceD2D(const SurfaceD2D &) = delete;
	SurfaceD2D(SurfaceD2D &&) = delete;
	SurfaceD2D &operator=(const SurfaceD2D &) = delete;
	SurfaceD2D &operator=(SurfaceD2D &&) = delete;
	~SurfaceD2D() noexcept override;

	void SetScale(WindowID wid) noexcept;
	void Init(WindowID wid) override;
	void Init(SurfaceID sid, WindowID wid) override;
	std::unique_ptr<Surface> AllocatePixMap(int width, int height) override;

	void SetMode(SurfaceMode mode_) override;

	void Release() noexcept override;
	int SupportsFeature(Supports feature) noexcept override;
	bool Initialised() override;

	void D2DPenColourAlpha(ColourRGBA fore) noexcept;
	int LogPixelsY() override;
	int PixelDivisions() override;
	int DeviceHeightFont(int points) override;
	void LineDraw(Point start, Point end, Stroke stroke) override;
	ID2D1PathGeometry *Geometry(const Point *pts, size_t npts, D2D1_FIGURE_BEGIN figureBegin) noexcept;
	void PolyLine(const Point *pts, size_t npts, Stroke stroke) override;
	void Polygon(const Point *pts, size_t npts, FillStroke fillStroke) override;
	void RectangleDraw(PRectangle rc, FillStroke fillStroke) override;
	void RectangleFrame(PRectangle rc, Stroke stroke) override;
	void FillRectangle(PRectangle rc, Fill fill) override;
	void FillRectangleAligned(PRectangle rc, Fill fill) override;
	void FillRectangle(PRectangle rc, Surface &surfacePattern) override;
	void RoundedRectangle(PRectangle rc, FillStroke fillStroke) override;
	void AlphaRectangle(PRectangle rc, XYPOSITION cornerSize, FillStroke fillStroke) override;
	void GradientRectangle(PRectangle rc, const std::vector<ColourStop> &stops, GradientOptions options) override;
	void DrawRGBAImage(PRectangle rc, int width, int height, const unsigned char *pixelsImage) override;
	void Ellipse(PRectangle rc, FillStroke fillStroke) override;
	void Stadium(PRectangle rc, FillStroke fillStroke, Ends ends) override;
	void Copy(PRectangle rc, Point from, Surface &surfaceSource) override;

	std::unique_ptr<IScreenLineLayout> Layout(const IScreenLine *screenLine) override;

	void DrawTextCommon(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text, int codePageOverride, UINT fuOptions);

	void DrawTextNoClip(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text, ColourRGBA fore, ColourRGBA back) override;
	void DrawTextClipped(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text, ColourRGBA fore, ColourRGBA back) override;
	void DrawTextTransparent(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text, ColourRGBA fore) override;
	void MeasureWidths(const Font *font_, std::string_view text, XYPOSITION *positions) override;
	XYPOSITION WidthText(const Font *font_, std::string_view text) override;

	void DrawTextNoClipUTF8(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text, ColourRGBA fore, ColourRGBA back) override;
	void DrawTextClippedUTF8(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text, ColourRGBA fore, ColourRGBA back) override;
	void DrawTextTransparentUTF8(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text, ColourRGBA fore) override;
	void MeasureWidthsUTF8(const Font *font_, std::string_view text, XYPOSITION *positions) override;
	XYPOSITION WidthTextUTF8(const Font *font_, std::string_view text) override;

	XYPOSITION Ascent(const Font *font_) override;
	XYPOSITION Descent(const Font *font_) override;
	XYPOSITION InternalLeading(const Font *font_) override;
	XYPOSITION Height(const Font *font_) override;
	XYPOSITION AverageCharWidth(const Font *font_) override;

	void SetClip(PRectangle rc) override;
	void PopClip() override;
	void FlushCachedState() override;
	void FlushDrawing() override;
};

SurfaceD2D::SurfaceD2D() noexcept {
}

SurfaceD2D::SurfaceD2D(ID2D1RenderTarget *pRenderTargetCompatible, int width, int height, SurfaceMode mode_, int logPixelsY_) noexcept {
	const D2D1_SIZE_F desiredSize = D2D1::SizeF(static_cast<float>(width), static_cast<float>(height));
	D2D1_PIXEL_FORMAT desiredFormat;
#ifdef __MINGW32__
	desiredFormat.format = DXGI_FORMAT_UNKNOWN;
#else
	desiredFormat = pRenderTargetCompatible->GetPixelFormat();
#endif
	desiredFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
	const HRESULT hr = pRenderTargetCompatible->CreateCompatibleRenderTarget(
		&desiredSize, nullptr, &desiredFormat, D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS_NONE, &pBitmapRenderTarget);
	if (SUCCEEDED(hr)) {
		pRenderTarget = pBitmapRenderTarget;
		pRenderTarget->BeginDraw();
		ownRenderTarget = true;
	}
	mode = mode_;
	logPixelsY = logPixelsY_;
}

SurfaceD2D::~SurfaceD2D() noexcept {
	Clear();
}

void SurfaceD2D::Clear() noexcept {
	ReleaseUnknown(pBrush);
	if (pRenderTarget) {
		while (clipsActive) {
			pRenderTarget->PopAxisAlignedClip();
			clipsActive--;
		}
		if (ownRenderTarget) {
			pRenderTarget->EndDraw();
			ReleaseUnknown(pRenderTarget);
			ownRenderTarget = false;
		}
		pRenderTarget = nullptr;
	}
	pBitmapRenderTarget = nullptr;
}

void SurfaceD2D::Release() noexcept {
	Clear();
}

void SurfaceD2D::SetScale(WindowID wid) noexcept {
	logPixelsY = DpiForWindow(wid);
}

int SurfaceD2D::SupportsFeature(Supports feature) noexcept {
	for (const Supports f : SupportsD2D) {
		if (f == feature)
			return 1;
	}
	return 0;
}

bool SurfaceD2D::Initialised() {
	return pRenderTarget != nullptr;
}

void SurfaceD2D::Init(WindowID wid) {
	Release();
	SetScale(wid);
}

void SurfaceD2D::Init(SurfaceID sid, WindowID wid) {
	Release();
	SetScale(wid);
	pRenderTarget = static_cast<ID2D1RenderTarget *>(sid);
}

std::unique_ptr<Surface> SurfaceD2D::AllocatePixMap(int width, int height) {
	return std::make_unique<SurfaceD2D>(pRenderTarget, width, height, mode, logPixelsY);
}

void SurfaceD2D::SetMode(SurfaceMode mode_) {
	mode = mode_;
}

HRESULT SurfaceD2D::GetBitmap(ID2D1Bitmap **ppBitmap) {
	PLATFORM_ASSERT(pBitmapRenderTarget);
	return pBitmapRenderTarget->GetBitmap(ppBitmap);
}

void SurfaceD2D::D2DPenColourAlpha(ColourRGBA fore) noexcept {
	if (pRenderTarget) {
		const D2D_COLOR_F col = ColorFromColourAlpha(fore);
		if (pBrush) {
			pBrush->SetColor(col);
		} else {
			const HRESULT hr = pRenderTarget->CreateSolidColorBrush(col, &pBrush);
			if (!SUCCEEDED(hr)) {
				ReleaseUnknown(pBrush);
			}
		}
	}
}

void SurfaceD2D::SetFont(const Font *font_) {
	const FontDirectWrite *pfm = dynamic_cast<const FontDirectWrite *>(font_);
	PLATFORM_ASSERT(pfm);
	if (!pfm) {
		throw std::runtime_error("SurfaceD2D::SetFont: wrong Font type.");
	}
	pTextFormat = pfm->pTextFormat;
	yAscent = pfm->yAscent;
	yDescent = pfm->yDescent;
	yInternalLeading = pfm->yInternalLeading;
	codePageText = mode.codePage;
	if (!(mode.codePage == CpUtf8) && (pfm->characterSet != CharacterSet::Ansi)) {
		codePageText = CodePageFromCharSet(pfm->characterSet, mode.codePage);
	}
	if (pRenderTarget) {
		D2D1_TEXT_ANTIALIAS_MODE aaMode;
		aaMode = DWriteMapFontQuality(pfm->extraFontFlag);

		if (aaMode == D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE && customClearTypeRenderingParams)
			pRenderTarget->SetTextRenderingParams(customClearTypeRenderingParams);
		else if (defaultRenderingParams)
			pRenderTarget->SetTextRenderingParams(defaultRenderingParams);

		pRenderTarget->SetTextAntialiasMode(aaMode);
	}
}

int SurfaceD2D::LogPixelsY() {
	return logPixelsY;
}

int SurfaceD2D::PixelDivisions() {
	// Win32 uses device pixels.
	return 1;
}

int SurfaceD2D::DeviceHeightFont(int points) {
	return ::MulDiv(points, LogPixelsY(), 72);
}

void SurfaceD2D::LineDraw(Point start, Point end, Stroke stroke) {
	D2DPenColourAlpha(stroke.colour);

	D2D1_STROKE_STYLE_PROPERTIES strokeProps {};
	strokeProps.startCap = D2D1_CAP_STYLE_SQUARE;
	strokeProps.endCap = D2D1_CAP_STYLE_SQUARE;
	strokeProps.dashCap = D2D1_CAP_STYLE_FLAT;
	strokeProps.lineJoin = D2D1_LINE_JOIN_MITER;
	strokeProps.miterLimit = 4.0f;
	strokeProps.dashStyle = D2D1_DASH_STYLE_SOLID;
	strokeProps.dashOffset = 0;

	// get the stroke style to apply
	ID2D1StrokeStyle *pStrokeStyle = nullptr;
	const HRESULT hr = pD2DFactory->CreateStrokeStyle(
		strokeProps, nullptr, 0, &pStrokeStyle);
	if (SUCCEEDED(hr)) {
		pRenderTarget->DrawLine(
			DPointFromPoint(start),
			DPointFromPoint(end), pBrush, stroke.WidthF(), pStrokeStyle);
	}

	ReleaseUnknown(pStrokeStyle);
}

ID2D1PathGeometry *SurfaceD2D::Geometry(const Point *pts, size_t npts, D2D1_FIGURE_BEGIN figureBegin) noexcept {
	ID2D1PathGeometry *geometry = nullptr;
	HRESULT hr = pD2DFactory->CreatePathGeometry(&geometry);
	if (SUCCEEDED(hr) && geometry) {
		ID2D1GeometrySink *sink = nullptr;
		hr = geometry->Open(&sink);
		if (SUCCEEDED(hr) && sink) {
			sink->BeginFigure(DPointFromPoint(pts[0]), figureBegin);
			for (size_t i = 1; i < npts; i++) {
				sink->AddLine(DPointFromPoint(pts[i]));
			}
			sink->EndFigure((figureBegin == D2D1_FIGURE_BEGIN_FILLED) ?
				D2D1_FIGURE_END_CLOSED : D2D1_FIGURE_END_OPEN);
			sink->Close();
			ReleaseUnknown(sink);
		}
	}
	return geometry;
}

void SurfaceD2D::PolyLine(const Point *pts, size_t npts, Stroke stroke) {
	PLATFORM_ASSERT(pRenderTarget && (npts > 1));
	if (!pRenderTarget || (npts <= 1)) {
		return;
	}

	ID2D1PathGeometry *geometry = Geometry(pts, npts, D2D1_FIGURE_BEGIN_HOLLOW);
	PLATFORM_ASSERT(geometry);
	if (!geometry) {
		return;
	}

	D2DPenColourAlpha(stroke.colour);
	D2D1_STROKE_STYLE_PROPERTIES strokeProps {};
	strokeProps.startCap = D2D1_CAP_STYLE_ROUND;
	strokeProps.endCap = D2D1_CAP_STYLE_ROUND;
	strokeProps.dashCap = D2D1_CAP_STYLE_FLAT;
	strokeProps.lineJoin = D2D1_LINE_JOIN_MITER;
	strokeProps.miterLimit = 4.0f;
	strokeProps.dashStyle = D2D1_DASH_STYLE_SOLID;
	strokeProps.dashOffset = 0;

	// get the stroke style to apply
	ID2D1StrokeStyle *pStrokeStyle = nullptr;
	const HRESULT hr = pD2DFactory->CreateStrokeStyle(
		strokeProps, nullptr, 0, &pStrokeStyle);
	if (SUCCEEDED(hr)) {
		pRenderTarget->DrawGeometry(geometry, pBrush, stroke.WidthF(), pStrokeStyle);
	}
	ReleaseUnknown(pStrokeStyle);
	ReleaseUnknown(geometry);
}

void SurfaceD2D::Polygon(const Point *pts, size_t npts, FillStroke fillStroke) {
	PLATFORM_ASSERT(pRenderTarget && (npts > 2));
	if (pRenderTarget) {
		ID2D1PathGeometry *geometry = Geometry(pts, npts, D2D1_FIGURE_BEGIN_FILLED);
		PLATFORM_ASSERT(geometry);
		if (geometry) {
			D2DPenColourAlpha(fillStroke.fill.colour);
			pRenderTarget->FillGeometry(geometry, pBrush);
			D2DPenColourAlpha(fillStroke.stroke.colour);
			pRenderTarget->DrawGeometry(geometry, pBrush, fillStroke.stroke.WidthF());
			ReleaseUnknown(geometry);
		}
	}
}

void SurfaceD2D::RectangleDraw(PRectangle rc, FillStroke fillStroke) {
	if (!pRenderTarget)
		return;
	const D2D1_RECT_F rect = RectangleFromPRectangle(rc);
	const D2D1_RECT_F rectFill = RectangleInset(rect, fillStroke.stroke.WidthF());
	const float halfStroke = fillStroke.stroke.WidthF() / 2.0f;
	const D2D1_RECT_F rectOutline = RectangleInset(rect, halfStroke);

	D2DPenColourAlpha(fillStroke.fill.colour);
	pRenderTarget->FillRectangle(&rectFill, pBrush);
	D2DPenColourAlpha(fillStroke.stroke.colour);
	pRenderTarget->DrawRectangle(&rectOutline, pBrush, fillStroke.stroke.WidthF());
}

void SurfaceD2D::RectangleFrame(PRectangle rc, Stroke stroke) {
	if (pRenderTarget) {
		const XYPOSITION halfStroke = stroke.width / 2.0f;
		const D2D1_RECT_F rectangle1 = RectangleFromPRectangle(rc.Inset(halfStroke));
		D2DPenColourAlpha(stroke.colour);
		pRenderTarget->DrawRectangle(&rectangle1, pBrush, stroke.WidthF());
	}
}

void SurfaceD2D::FillRectangle(PRectangle rc, Fill fill) {
	if (pRenderTarget) {
		D2DPenColourAlpha(fill.colour);
		const D2D1_RECT_F rectangle = RectangleFromPRectangle(rc);
		pRenderTarget->FillRectangle(&rectangle, pBrush);
	}
}

void SurfaceD2D::FillRectangleAligned(PRectangle rc, Fill fill) {
	FillRectangle(PixelAlign(rc, 1), fill);
}

void SurfaceD2D::FillRectangle(PRectangle rc, Surface &surfacePattern) {
	SurfaceD2D *psurfOther = dynamic_cast<SurfaceD2D *>(&surfacePattern);
	PLATFORM_ASSERT(psurfOther);
	if (!psurfOther) {
		throw std::runtime_error("SurfaceD2D::FillRectangle: wrong Surface type.");
	}
	ID2D1Bitmap *pBitmap = nullptr;
	HRESULT hr = psurfOther->GetBitmap(&pBitmap);
	if (SUCCEEDED(hr) && pBitmap) {
		ID2D1BitmapBrush *pBitmapBrush = nullptr;
		const D2D1_BITMAP_BRUSH_PROPERTIES brushProperties =
	        D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_WRAP, D2D1_EXTEND_MODE_WRAP,
			D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
		// Create the bitmap brush.
		hr = pRenderTarget->CreateBitmapBrush(pBitmap, brushProperties, &pBitmapBrush);
		ReleaseUnknown(pBitmap);
		if (SUCCEEDED(hr) && pBitmapBrush) {
			pRenderTarget->FillRectangle(
				RectangleFromPRectangle(rc),
				pBitmapBrush);
			ReleaseUnknown(pBitmapBrush);
		}
	}
}

void SurfaceD2D::RoundedRectangle(PRectangle rc, FillStroke fillStroke) {
	if (pRenderTarget) {
		D2D1_ROUNDED_RECT roundedRectFill = {
			RectangleFromPRectangle(rc.Inset(1.0)),
			4, 4};
		D2DPenColourAlpha(fillStroke.fill.colour);
		pRenderTarget->FillRoundedRectangle(roundedRectFill, pBrush);

		D2D1_ROUNDED_RECT roundedRect = {
			RectangleFromPRectangle(rc.Inset(0.5)),
			4, 4};
		D2DPenColourAlpha(fillStroke.stroke.colour);
		pRenderTarget->DrawRoundedRectangle(roundedRect, pBrush, fillStroke.stroke.WidthF());
	}
}

void SurfaceD2D::AlphaRectangle(PRectangle rc, XYPOSITION cornerSize, FillStroke fillStroke) {
	const D2D1_RECT_F rect = RectangleFromPRectangle(rc);
	const D2D1_RECT_F rectFill = RectangleInset(rect, fillStroke.stroke.WidthF());
	const float halfStroke = fillStroke.stroke.WidthF() / 2.0f;
	const D2D1_RECT_F rectOutline = RectangleInset(rect, halfStroke);
	if (pRenderTarget) {
		if (cornerSize == 0) {
			// When corner size is zero, draw square rectangle to prevent blurry pixels at corners
			D2DPenColourAlpha(fillStroke.fill.colour);
			pRenderTarget->FillRectangle(rectFill, pBrush);

			D2DPenColourAlpha(fillStroke.stroke.colour);
			pRenderTarget->DrawRectangle(rectOutline, pBrush, fillStroke.stroke.WidthF());
		} else {
			const float cornerSizeF = static_cast<float>(cornerSize);
			D2D1_ROUNDED_RECT roundedRectFill = {
				rectFill, cornerSizeF - 1.0f, cornerSizeF - 1.0f };
			D2DPenColourAlpha(fillStroke.fill.colour);
			pRenderTarget->FillRoundedRectangle(roundedRectFill, pBrush);

			D2D1_ROUNDED_RECT roundedRect = {
				rectOutline, cornerSizeF, cornerSizeF};
			D2DPenColourAlpha(fillStroke.stroke.colour);
			pRenderTarget->DrawRoundedRectangle(roundedRect, pBrush, fillStroke.stroke.WidthF());
		}
	}
}

void SurfaceD2D::GradientRectangle(PRectangle rc, const std::vector<ColourStop> &stops, GradientOptions options) {
	if (pRenderTarget) {
		D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES lgbp {
			DPointFromPoint(Point(rc.left, rc.top)), {}
		};
		switch (options) {
		case GradientOptions::leftToRight:
			lgbp.endPoint = DPointFromPoint(Point(rc.right, rc.top));
			break;
		case GradientOptions::topToBottom:
		default:
			lgbp.endPoint = DPointFromPoint(Point(rc.left, rc.bottom));
			break;
		}

		std::vector<D2D1_GRADIENT_STOP> gradientStops;
		for (const ColourStop &stop : stops) {
			gradientStops.push_back({ static_cast<FLOAT>(stop.position), ColorFromColourAlpha(stop.colour) });
		}
		ID2D1GradientStopCollection *pGradientStops = nullptr;
		HRESULT hr = pRenderTarget->CreateGradientStopCollection(
			gradientStops.data(), static_cast<UINT32>(gradientStops.size()), &pGradientStops);
		if (FAILED(hr) || !pGradientStops) {
			return;
		}
		ID2D1LinearGradientBrush *pBrushLinear = nullptr;
		hr = pRenderTarget->CreateLinearGradientBrush(
			lgbp, pGradientStops, &pBrushLinear);
		if (SUCCEEDED(hr) && pBrushLinear) {
			const D2D1_RECT_F rectangle = RectangleFromPRectangle(PRectangle(
				std::round(rc.left), rc.top, std::round(rc.right), rc.bottom));
			pRenderTarget->FillRectangle(&rectangle, pBrushLinear);
			ReleaseUnknown(pBrushLinear);
		}
		ReleaseUnknown(pGradientStops);
	}
}

void SurfaceD2D::DrawRGBAImage(PRectangle rc, int width, int height, const unsigned char *pixelsImage) {
	if (pRenderTarget) {
		if (rc.Width() > width)
			rc.left += std::floor((rc.Width() - width) / 2);
		rc.right = rc.left + width;
		if (rc.Height() > height)
			rc.top += std::floor((rc.Height() - height) / 2);
		rc.bottom = rc.top + height;

		std::vector<unsigned char> image(RGBAImage::bytesPerPixel * height * width);
		RGBAImage::BGRAFromRGBA(image.data(), pixelsImage, static_cast<ptrdiff_t>(height) * width);

		ID2D1Bitmap *bitmap = nullptr;
		const D2D1_SIZE_U size = D2D1::SizeU(width, height);
		D2D1_BITMAP_PROPERTIES props = {{DXGI_FORMAT_B8G8R8A8_UNORM,
		    D2D1_ALPHA_MODE_PREMULTIPLIED}, 72.0, 72.0};
		const HRESULT hr = pRenderTarget->CreateBitmap(size, image.data(),
                  width * 4, &props, &bitmap);
		if (SUCCEEDED(hr)) {
			const D2D1_RECT_F rcDestination = RectangleFromPRectangle(rc);
			pRenderTarget->DrawBitmap(bitmap, rcDestination);
			ReleaseUnknown(bitmap);
		}
	}
}

void SurfaceD2D::Ellipse(PRectangle rc, FillStroke fillStroke) {
	if (!pRenderTarget)
		return;
	const D2D1_POINT_2F centre = DPointFromPoint(rc.Centre());

	const FLOAT radiusFill = static_cast<FLOAT>(rc.Width() / 2.0f - fillStroke.stroke.width);
	const D2D1_ELLIPSE ellipseFill = { centre, radiusFill, radiusFill };

	D2DPenColourAlpha(fillStroke.fill.colour);
	pRenderTarget->FillEllipse(ellipseFill, pBrush);

	const FLOAT radiusOutline = static_cast<FLOAT>(rc.Width() / 2.0f - fillStroke.stroke.width / 2.0f);
	const D2D1_ELLIPSE ellipseOutline = { centre, radiusOutline, radiusOutline };

	D2DPenColourAlpha(fillStroke.stroke.colour);
	pRenderTarget->DrawEllipse(ellipseOutline, pBrush, fillStroke.stroke.WidthF());
}

void SurfaceD2D::Stadium(PRectangle rc, FillStroke fillStroke, Ends ends) {
	if (!pRenderTarget)
		return;
	if (rc.Width() < rc.Height()) {
		// Can't draw nice ends so just draw a rectangle
		RectangleDraw(rc, fillStroke);
		return;
	}
	const FLOAT radius = static_cast<FLOAT>(rc.Height() / 2.0);
	const FLOAT radiusFill = radius - fillStroke.stroke.WidthF();
	const FLOAT halfStroke = fillStroke.stroke.WidthF() / 2.0f;
	if (ends == Surface::Ends::semiCircles) {
		const D2D1_RECT_F rect = RectangleFromPRectangle(rc);
		D2D1_ROUNDED_RECT roundedRectFill = { RectangleInset(rect, fillStroke.stroke.WidthF()),
			radiusFill, radiusFill };
		D2DPenColourAlpha(fillStroke.fill.colour);
		pRenderTarget->FillRoundedRectangle(roundedRectFill, pBrush);

		D2D1_ROUNDED_RECT roundedRect = { RectangleInset(rect, halfStroke),
			radius, radius };
		D2DPenColourAlpha(fillStroke.stroke.colour);
		pRenderTarget->DrawRoundedRectangle(roundedRect, pBrush, fillStroke.stroke.WidthF());
	} else {
		const Ends leftSide = static_cast<Ends>(static_cast<int>(ends) & 0xf);
		const Ends rightSide = static_cast<Ends>(static_cast<int>(ends) & 0xf0);
		PRectangle rcInner = rc;
		rcInner.left += radius;
		rcInner.right -= radius;
		ID2D1PathGeometry *pathGeometry = nullptr;
		const HRESULT hrGeometry = pD2DFactory->CreatePathGeometry(&pathGeometry);
		if (FAILED(hrGeometry) || !pathGeometry)
			return;
		ID2D1GeometrySink *pSink = nullptr;
		const HRESULT hrSink = pathGeometry->Open(&pSink);
		if (SUCCEEDED(hrSink) && pSink) {
			switch (leftSide) {
				case Ends::leftFlat:
					pSink->BeginFigure(DPointFromPoint(Point(rc.left + halfStroke, rc.top + halfStroke)), D2D1_FIGURE_BEGIN_FILLED);
					pSink->AddLine(DPointFromPoint(Point(rc.left + halfStroke, rc.bottom - halfStroke)));
					break;
				case Ends::leftAngle:
					pSink->BeginFigure(DPointFromPoint(Point(rcInner.left + halfStroke, rc.top + halfStroke)), D2D1_FIGURE_BEGIN_FILLED);
					pSink->AddLine(DPointFromPoint(Point(rc.left + halfStroke, rc.Centre().y)));
					pSink->AddLine(DPointFromPoint(Point(rcInner.left + halfStroke, rc.bottom - halfStroke)));
					break;
				case Ends::semiCircles:
				default: {
						pSink->BeginFigure(DPointFromPoint(Point(rcInner.left + halfStroke, rc.top + halfStroke)), D2D1_FIGURE_BEGIN_FILLED);
						D2D1_ARC_SEGMENT segment{};
						segment.point = DPointFromPoint(Point(rcInner.left + halfStroke, rc.bottom - halfStroke));
						segment.size = D2D1::SizeF(radiusFill, radiusFill);
						segment.rotationAngle = 0.0f;
						segment.sweepDirection = D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE;
						segment.arcSize = D2D1_ARC_SIZE_SMALL;
						pSink->AddArc(segment);
					}
					break;
			}

			switch (rightSide) {
			case Ends::rightFlat:
				pSink->AddLine(DPointFromPoint(Point(rc.right - halfStroke, rc.bottom - halfStroke)));
				pSink->AddLine(DPointFromPoint(Point(rc.right - halfStroke, rc.top + halfStroke)));
				break;
			case Ends::rightAngle:
				pSink->AddLine(DPointFromPoint(Point(rcInner.right - halfStroke, rc.bottom - halfStroke)));
				pSink->AddLine(DPointFromPoint(Point(rc.right - halfStroke, rc.Centre().y)));
				pSink->AddLine(DPointFromPoint(Point(rcInner.right - halfStroke, rc.top + halfStroke)));
				break;
			case Ends::semiCircles:
			default: {
					pSink->AddLine(DPointFromPoint(Point(rcInner.right - halfStroke, rc.bottom - halfStroke)));
					D2D1_ARC_SEGMENT segment{};
					segment.point = DPointFromPoint(Point(rcInner.right - halfStroke, rc.top + halfStroke));
					segment.size = D2D1::SizeF(radiusFill, radiusFill);
					segment.rotationAngle = 0.0f;
					segment.sweepDirection = D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE;
					segment.arcSize = D2D1_ARC_SIZE_SMALL;
					pSink->AddArc(segment);
				}
				break;
			}

			pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

			pSink->Close();
		}
		ReleaseUnknown(pSink);
		D2DPenColourAlpha(fillStroke.fill.colour);
		pRenderTarget->FillGeometry(pathGeometry, pBrush);
		D2DPenColourAlpha(fillStroke.stroke.colour);
		pRenderTarget->DrawGeometry(pathGeometry, pBrush, fillStroke.stroke.WidthF());
		ReleaseUnknown(pathGeometry);
	}
}

void SurfaceD2D::Copy(PRectangle rc, Point from, Surface &surfaceSource) {
	SurfaceD2D &surfOther = dynamic_cast<SurfaceD2D &>(surfaceSource);
	ID2D1Bitmap *pBitmap = nullptr;
	const HRESULT hr = surfOther.GetBitmap(&pBitmap);
	if (SUCCEEDED(hr) && pBitmap) {
		const D2D1_RECT_F rcDestination = RectangleFromPRectangle(rc);
		const D2D1_RECT_F rcSource = RectangleFromPRectangle(PRectangle(
			from.x, from.y, from.x + rc.Width(), from.y + rc.Height()));
		pRenderTarget->DrawBitmap(pBitmap, rcDestination, 1.0f,
			D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, rcSource);
		ReleaseUnknown(pBitmap);
	}
}

class BlobInline : public IDWriteInlineObject {
	XYPOSITION width;

	// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, PVOID *ppv) override;
	STDMETHODIMP_(ULONG)AddRef() override;
	STDMETHODIMP_(ULONG)Release() override;

	// IDWriteInlineObject
	COM_DECLSPEC_NOTHROW STDMETHODIMP Draw(
		void *clientDrawingContext,
		IDWriteTextRenderer *renderer,
		FLOAT originX,
		FLOAT originY,
		BOOL isSideways,
		BOOL isRightToLeft,
		IUnknown *clientDrawingEffect
		) override;
	COM_DECLSPEC_NOTHROW STDMETHODIMP GetMetrics(DWRITE_INLINE_OBJECT_METRICS *metrics) override;
	COM_DECLSPEC_NOTHROW STDMETHODIMP GetOverhangMetrics(DWRITE_OVERHANG_METRICS *overhangs) override;
	COM_DECLSPEC_NOTHROW STDMETHODIMP GetBreakConditions(
		DWRITE_BREAK_CONDITION *breakConditionBefore,
		DWRITE_BREAK_CONDITION *breakConditionAfter) override;
public:
	BlobInline(XYPOSITION width_=0.0f) noexcept : width(width_) {
	}
	// Defaulted so BlobInline objects can be copied.
	BlobInline(const BlobInline &) = default;
	BlobInline(BlobInline &&) = default;
	BlobInline &operator=(const BlobInline &) = default;
	BlobInline &operator=(BlobInline &&) = default;
	virtual ~BlobInline() noexcept = default;
};

/// Implement IUnknown
STDMETHODIMP BlobInline::QueryInterface(REFIID riid, PVOID *ppv) {
	if (!ppv)
		return E_POINTER;
	// Never called so not checked.
	*ppv = nullptr;
	if (riid == IID_IUnknown)
		*ppv = static_cast<IUnknown *>(this);
	if (riid == __uuidof(IDWriteInlineObject))
		*ppv = static_cast<IDWriteInlineObject *>(this);
	if (!*ppv)
		return E_NOINTERFACE;
	return S_OK;
}

STDMETHODIMP_(ULONG) BlobInline::AddRef() {
	// Lifetime tied to Platform methods so ignore any reference operations.
	return 1;
}

STDMETHODIMP_(ULONG) BlobInline::Release() {
	// Lifetime tied to Platform methods so ignore any reference operations.
	return 1;
}

/// Implement IDWriteInlineObject
COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE BlobInline::Draw(
	void*,
	IDWriteTextRenderer*,
	FLOAT,
	FLOAT,
	BOOL,
	BOOL,
	IUnknown*) {
	// Since not performing drawing, not necessary to implement
	// Could be implemented by calling back into platform-independent code.
	// This would allow more of the drawing to be mediated through DirectWrite.
	return S_OK;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE BlobInline::GetMetrics(
	DWRITE_INLINE_OBJECT_METRICS *metrics
) {
	if (!metrics)
		return E_POINTER;
	metrics->width = static_cast<FLOAT>(width);
	metrics->height = 2;
	metrics->baseline = 1;
	metrics->supportsSideways = FALSE;
	return S_OK;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE BlobInline::GetOverhangMetrics(
	DWRITE_OVERHANG_METRICS *overhangs
) {
	if (!overhangs)
		return E_POINTER;
	overhangs->left = 0;
	overhangs->top = 0;
	overhangs->right = 0;
	overhangs->bottom = 0;
	return S_OK;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE BlobInline::GetBreakConditions(
	DWRITE_BREAK_CONDITION *breakConditionBefore,
	DWRITE_BREAK_CONDITION *breakConditionAfter
) {
	if (!breakConditionBefore || !breakConditionAfter)
		return E_POINTER;
	// Since not performing 2D layout, not necessary to implement
	*breakConditionBefore = DWRITE_BREAK_CONDITION_NEUTRAL;
	*breakConditionAfter = DWRITE_BREAK_CONDITION_NEUTRAL;
	return S_OK;
}

class ScreenLineLayout : public IScreenLineLayout {
	IDWriteTextLayout *textLayout = nullptr;
	std::string text;
	std::wstring buffer;
	std::vector<BlobInline> blobs;
	static void FillTextLayoutFormats(const IScreenLine *screenLine, IDWriteTextLayout *textLayout, std::vector<BlobInline> &blobs);
	static std::wstring ReplaceRepresentation(std::string_view text);
	static size_t GetPositionInLayout(std::string_view text, size_t position);
public:
	ScreenLineLayout(const IScreenLine *screenLine);
	// Deleted so ScreenLineLayout objects can not be copied
	ScreenLineLayout(const ScreenLineLayout &) = delete;
	ScreenLineLayout(ScreenLineLayout &&) = delete;
	ScreenLineLayout &operator=(const ScreenLineLayout &) = delete;
	ScreenLineLayout &operator=(ScreenLineLayout &&) = delete;
	~ScreenLineLayout() noexcept override;
	size_t PositionFromX(XYPOSITION xDistance, bool charPosition) override;
	XYPOSITION XFromPosition(size_t caretPosition) override;
	std::vector<Interval> FindRangeIntervals(size_t start, size_t end) override;
};

// Each char can have its own style, so we fill the textLayout with the textFormat of each char

void ScreenLineLayout::FillTextLayoutFormats(const IScreenLine *screenLine, IDWriteTextLayout *textLayout, std::vector<BlobInline> &blobs) {
	// Reserve enough entries up front so they are not moved and the pointers handed
	// to textLayout remain valid.
	const ptrdiff_t numRepresentations = screenLine->RepresentationCount();
	std::string_view text = screenLine->Text();
	const ptrdiff_t numTabs = std::count(std::begin(text), std::end(text), '\t');
	blobs.reserve(numRepresentations + numTabs);

	UINT32 layoutPosition = 0;

	for (size_t bytePosition = 0; bytePosition < screenLine->Length();) {
		const unsigned char uch = screenLine->Text()[bytePosition];
		const unsigned int byteCount = UTF8BytesOfLead[uch];
		const UINT32 codeUnits = UTF16LengthFromUTF8ByteCount(byteCount);
		const DWRITE_TEXT_RANGE textRange = { layoutPosition, codeUnits };

		XYPOSITION representationWidth = screenLine->RepresentationWidth(bytePosition);
		if ((representationWidth == 0.0f) && (screenLine->Text()[bytePosition] == '\t')) {
			D2D1_POINT_2F realPt {};
			DWRITE_HIT_TEST_METRICS realCaretMetrics {};
			textLayout->HitTestTextPosition(
				layoutPosition,
				false, // trailing if false, else leading edge
				&realPt.x,
				&realPt.y,
				&realCaretMetrics
			);

			const XYPOSITION nextTab = screenLine->TabPositionAfter(realPt.x);
			representationWidth = nextTab - realPt.x;
		}
		if (representationWidth > 0.0f) {
			blobs.push_back(BlobInline(representationWidth));
			textLayout->SetInlineObject(&blobs.back(), textRange);
		};

		const FontDirectWrite *pfm =
			dynamic_cast<const FontDirectWrite *>(screenLine->FontOfPosition(bytePosition));
		if (!pfm) {
			throw std::runtime_error("FillTextLayoutFormats: wrong Font type.");
		}

		const unsigned int fontFamilyNameSize = pfm->pTextFormat->GetFontFamilyNameLength();
		std::wstring fontFamilyName(fontFamilyNameSize, 0);
		const HRESULT hrFamily = pfm->pTextFormat->GetFontFamilyName(fontFamilyName.data(), fontFamilyNameSize + 1);
		if (SUCCEEDED(hrFamily)) {
			textLayout->SetFontFamilyName(fontFamilyName.c_str(), textRange);
		}

		textLayout->SetFontSize(pfm->pTextFormat->GetFontSize(), textRange);
		textLayout->SetFontWeight(pfm->pTextFormat->GetFontWeight(), textRange);
		textLayout->SetFontStyle(pfm->pTextFormat->GetFontStyle(), textRange);

		const unsigned int localeNameSize = pfm->pTextFormat->GetLocaleNameLength();
		std::wstring localeName(localeNameSize, 0);
		const HRESULT hrLocale = pfm->pTextFormat->GetLocaleName(localeName.data(), localeNameSize + 1);
		if (SUCCEEDED(hrLocale)) {
			textLayout->SetLocaleName(localeName.c_str(), textRange);
		}

		textLayout->SetFontStretch(pfm->pTextFormat->GetFontStretch(), textRange);

		IDWriteFontCollection *fontCollection = nullptr;
		if (SUCCEEDED(pfm->pTextFormat->GetFontCollection(&fontCollection))) {
			textLayout->SetFontCollection(fontCollection, textRange);
		}

		bytePosition += byteCount;
		layoutPosition += codeUnits;
	}

}

/* Convert to a wide character string and replace tabs with X to stop DirectWrite tab expansion */

std::wstring ScreenLineLayout::ReplaceRepresentation(std::string_view text) {
	const TextWide wideText(text, CpUtf8);
	std::wstring ws(wideText.buffer, wideText.tlen);
	std::replace(ws.begin(), ws.end(), L'\t', L'X');
	return ws;
}

// Finds the position in the wide character version of the text.

size_t ScreenLineLayout::GetPositionInLayout(std::string_view text, size_t position) {
	const std::string_view textUptoPosition = text.substr(0, position);
	return UTF16Length(textUptoPosition);
}

ScreenLineLayout::ScreenLineLayout(const IScreenLine *screenLine) {
	// If the text is empty, then no need to go through this function
	if (!screenLine || !screenLine->Length())
		return;

	text = screenLine->Text();

	// Get textFormat
	const FontDirectWrite *pfm = dynamic_cast<const FontDirectWrite *>(screenLine->FontOfPosition(0));

	if (!pIDWriteFactory || !pfm || !pfm->pTextFormat) {
		return;
	}

	// Convert the string to wstring and replace the original control characters with their representative chars.
	buffer = ReplaceRepresentation(screenLine->Text());

	// Create a text layout
	const HRESULT hrCreate = pIDWriteFactory->CreateTextLayout(
		buffer.c_str(),
		static_cast<UINT32>(buffer.length()),
		pfm->pTextFormat,
		static_cast<FLOAT>(screenLine->Width()),
		static_cast<FLOAT>(screenLine->Height()),
		&textLayout);
	if (!SUCCEEDED(hrCreate)) {
		return;
	}

	// Fill the textLayout chars with their own formats
	FillTextLayoutFormats(screenLine, textLayout, blobs);
}

ScreenLineLayout::~ScreenLineLayout() noexcept {
	ReleaseUnknown(textLayout);
}

// Get the position from the provided x

size_t ScreenLineLayout::PositionFromX(XYPOSITION xDistance, bool charPosition) {
	if (!textLayout) {
		return 0;
	}

	// Returns the text position corresponding to the mouse x,y.
	// If hitting the trailing side of a cluster, return the
	// leading edge of the following text position.

	BOOL isTrailingHit = FALSE;
	BOOL isInside = FALSE;
	DWRITE_HIT_TEST_METRICS caretMetrics {};

	textLayout->HitTestPoint(
		static_cast<FLOAT>(xDistance),
		0.0f,
		&isTrailingHit,
		&isInside,
		&caretMetrics
	);

	DWRITE_HIT_TEST_METRICS hitTestMetrics {};
	if (isTrailingHit) {
		FLOAT caretX = 0.0f;
		FLOAT caretY = 0.0f;

		// Uses hit-testing to align the current caret position to a whole cluster,
		// rather than residing in the middle of a base character + diacritic,
		// surrogate pair, or character + UVS.

		// Align the caret to the nearest whole cluster.
		textLayout->HitTestTextPosition(
			caretMetrics.textPosition,
			false,
			&caretX,
			&caretY,
			&hitTestMetrics
		);
	}

	size_t pos;
	if (charPosition) {
		pos = isTrailingHit ? hitTestMetrics.textPosition : caretMetrics.textPosition;
	} else {
		pos = isTrailingHit ? static_cast<size_t>(hitTestMetrics.textPosition) + hitTestMetrics.length : caretMetrics.textPosition;
	}

	// Get the character position in original string
	return UTF8PositionFromUTF16Position(text, pos);
}

// Finds the point of the caret position

XYPOSITION ScreenLineLayout::XFromPosition(size_t caretPosition) {
	if (!textLayout) {
		return 0.0;
	}
	// Convert byte positions to wchar_t positions
	const size_t position = GetPositionInLayout(text, caretPosition);

	// Translate text character offset to point x,y.
	DWRITE_HIT_TEST_METRICS caretMetrics {};
	D2D1_POINT_2F pt {};

	textLayout->HitTestTextPosition(
		static_cast<UINT32>(position),
		false, // trailing if false, else leading edge
		&pt.x,
		&pt.y,
		&caretMetrics
	);

	return pt.x;
}

// Find the selection range rectangles

std::vector<Interval> ScreenLineLayout::FindRangeIntervals(size_t start, size_t end) {
	std::vector<Interval> ret;

	if (!textLayout || (start == end)) {
		return ret;
	}

	// Convert byte positions to wchar_t positions
	const size_t startPos = GetPositionInLayout(text, start);
	const size_t endPos = GetPositionInLayout(text, end);

	// Find selection range length
	const size_t rangeLength = (endPos > startPos) ? (endPos - startPos) : (startPos - endPos);

	// Determine actual number of hit-test ranges
	UINT32 actualHitTestCount = 0;

	// First try with 2 elements and if more needed, allocate.
	std::vector<DWRITE_HIT_TEST_METRICS> hitTestMetrics(2);
	textLayout->HitTestTextRange(
		static_cast<UINT32>(startPos),
		static_cast<UINT32>(rangeLength),
		0, // x
		0, // y
		hitTestMetrics.data(),
		static_cast<UINT32>(hitTestMetrics.size()),
		&actualHitTestCount
	);

	if (actualHitTestCount == 0) {
		return ret;
	}

	if (hitTestMetrics.size() < actualHitTestCount) {
		// Allocate enough room to return all hit-test metrics.
		hitTestMetrics.resize(actualHitTestCount);
		textLayout->HitTestTextRange(
			static_cast<UINT32>(startPos),
			static_cast<UINT32>(rangeLength),
			0, // x
			0, // y
			hitTestMetrics.data(),
			static_cast<UINT32>(hitTestMetrics.size()),
			&actualHitTestCount
		);
	}

	// Get the selection ranges behind the text.

	for (size_t i = 0; i < actualHitTestCount; ++i) {
		// Store selection rectangle
		const DWRITE_HIT_TEST_METRICS &htm = hitTestMetrics[i];
		const Interval selectionInterval { htm.left, htm.left + htm.width };
		ret.push_back(selectionInterval);
	}

	return ret;
}

std::unique_ptr<IScreenLineLayout> SurfaceD2D::Layout(const IScreenLine *screenLine) {
	return std::make_unique<ScreenLineLayout>(screenLine);
}

void SurfaceD2D::DrawTextCommon(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text, int codePageOverride, UINT fuOptions) {
	SetFont(font_);

	// Use Unicode calls
	const int codePageDraw = codePageOverride ? codePageOverride : codePageText;
	const TextWide tbuf(text, codePageDraw);
	if (pRenderTarget && pTextFormat && pBrush) {
		if (fuOptions & ETO_CLIPPED) {
			const D2D1_RECT_F rcClip = RectangleFromPRectangle(rc);
			pRenderTarget->PushAxisAlignedClip(rcClip, D2D1_ANTIALIAS_MODE_ALIASED);
		}

		// Explicitly creating a text layout appears a little faster
		IDWriteTextLayout *pTextLayout = nullptr;
		const HRESULT hr = pIDWriteFactory->CreateTextLayout(
				tbuf.buffer,
				tbuf.tlen,
				pTextFormat,
				static_cast<FLOAT>(rc.Width()),
				static_cast<FLOAT>(rc.Height()),
				&pTextLayout);
		if (SUCCEEDED(hr)) {
			const D2D1_POINT_2F origin = DPointFromPoint(Point(rc.left, ybase-yAscent));
			pRenderTarget->DrawTextLayout(origin, pTextLayout, pBrush, d2dDrawTextOptions);
			ReleaseUnknown(pTextLayout);
		}

		if (fuOptions & ETO_CLIPPED) {
			pRenderTarget->PopAxisAlignedClip();
		}
	}
}

void SurfaceD2D::DrawTextNoClip(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text,
	ColourRGBA fore, ColourRGBA back) {
	if (pRenderTarget) {
		FillRectangleAligned(rc, back);
		D2DPenColourAlpha(fore);
		DrawTextCommon(rc, font_, ybase, text, 0, ETO_OPAQUE);
	}
}

void SurfaceD2D::DrawTextClipped(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text,
	ColourRGBA fore, ColourRGBA back) {
	if (pRenderTarget) {
		FillRectangleAligned(rc, back);
		D2DPenColourAlpha(fore);
		DrawTextCommon(rc, font_, ybase, text, 0, ETO_OPAQUE | ETO_CLIPPED);
	}
}

void SurfaceD2D::DrawTextTransparent(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text,
	ColourRGBA fore) {
	// Avoid drawing spaces in transparent mode
	for (const char ch : text) {
		if (ch != ' ') {
			if (pRenderTarget) {
				D2DPenColourAlpha(fore);
				DrawTextCommon(rc, font_, ybase, text, 0, 0);
			}
			return;
		}
	}
}

void SurfaceD2D::MeasureWidths(const Font *font_, std::string_view text, XYPOSITION *positions) {
	SetFont(font_);
	if (!pIDWriteFactory || !pTextFormat) {
		// SetFont failed or no access to DirectWrite so give up.
		return;
	}
	const TextWide tbuf(text, codePageText);
	TextPositions poses(tbuf.tlen);
	// Initialize poses for safety.
	std::fill(poses.buffer, poses.buffer + tbuf.tlen, 0.0f);
	// Create a layout
	IDWriteTextLayout *pTextLayout = nullptr;
	const HRESULT hrCreate = pIDWriteFactory->CreateTextLayout(tbuf.buffer, tbuf.tlen, pTextFormat, 10000.0, 1000.0, &pTextLayout);
	if (!SUCCEEDED(hrCreate) || !pTextLayout) {
		return;
	}
	constexpr int clusters = stackBufferLength;
	DWRITE_CLUSTER_METRICS clusterMetrics[clusters];
	UINT32 count = 0;
	const HRESULT hrGetCluster = pTextLayout->GetClusterMetrics(clusterMetrics, clusters, &count);
	ReleaseUnknown(pTextLayout);
	if (!SUCCEEDED(hrGetCluster)) {
		return;
	}
	// A cluster may be more than one WCHAR, such as for "ffi" which is a ligature in the Candara font
	XYPOSITION position = 0.0;
	int ti=0;
	for (unsigned int ci=0; ci<count; ci++) {
		for (unsigned int inCluster=0; inCluster<clusterMetrics[ci].length; inCluster++) {
			poses.buffer[ti++] = position + clusterMetrics[ci].width * (inCluster + 1) / clusterMetrics[ci].length;
		}
		position += clusterMetrics[ci].width;
	}
	PLATFORM_ASSERT(ti == tbuf.tlen);
	if (mode.codePage == CpUtf8) {
		// Map the widths given for UTF-16 characters back onto the UTF-8 input string
		size_t i = 0;
		for (int ui = 0; ui < tbuf.tlen; ui++) {
			const unsigned char uch = text[i];
			const unsigned int byteCount = UTF8BytesOfLead[uch];
			if (byteCount == 4) {	// Non-BMP
				ui++;
			}
			for (unsigned int bytePos=0; (bytePos<byteCount) && (i<text.length()) && (ui<tbuf.tlen); bytePos++) {
				positions[i++] = poses.buffer[ui];
			}
		}
		const XYPOSITION lastPos = (i > 0) ? positions[i - 1] : 0.0;
		while (i<text.length()) {
			positions[i++] = lastPos;
		}
	} else if (!IsDBCSCodePage(codePageText)) {

		// One char per position
		PLATFORM_ASSERT(text.length() == static_cast<size_t>(tbuf.tlen));
		for (int kk=0; kk<tbuf.tlen; kk++) {
			positions[kk] = poses.buffer[kk];
		}

	} else {

		// May be one or two bytes per position
		int ui = 0;
		for (size_t i=0; i<text.length() && ui<tbuf.tlen;) {
			positions[i] = poses.buffer[ui];
			if (DBCSIsLeadByte(codePageText, text[i])) {
				positions[i+1] = poses.buffer[ui];
				i += 2;
			} else {
				i++;
			}

			ui++;
		}
	}
}

XYPOSITION SurfaceD2D::WidthText(const Font *font_, std::string_view text) {
	FLOAT width = 1.0;
	SetFont(font_);
	const TextWide tbuf(text, codePageText);
	if (pIDWriteFactory && pTextFormat) {
		// Create a layout
		IDWriteTextLayout *pTextLayout = nullptr;
		const HRESULT hr = pIDWriteFactory->CreateTextLayout(tbuf.buffer, tbuf.tlen, pTextFormat, 1000.0, 1000.0, &pTextLayout);
		if (SUCCEEDED(hr) && pTextLayout) {
			DWRITE_TEXT_METRICS textMetrics;
			if (SUCCEEDED(pTextLayout->GetMetrics(&textMetrics)))
				width = textMetrics.widthIncludingTrailingWhitespace;
			ReleaseUnknown(pTextLayout);
		}
	}
	return width;
}

void SurfaceD2D::DrawTextNoClipUTF8(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text,
	ColourRGBA fore, ColourRGBA back) {
	if (pRenderTarget) {
		FillRectangleAligned(rc, back);
		D2DPenColourAlpha(fore);
		DrawTextCommon(rc, font_, ybase, text, CpUtf8, ETO_OPAQUE);
	}
}

void SurfaceD2D::DrawTextClippedUTF8(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text,
	ColourRGBA fore, ColourRGBA back) {
	if (pRenderTarget) {
		FillRectangleAligned(rc, back);
		D2DPenColourAlpha(fore);
		DrawTextCommon(rc, font_, ybase, text, CpUtf8, ETO_OPAQUE | ETO_CLIPPED);
	}
}

void SurfaceD2D::DrawTextTransparentUTF8(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text,
	ColourRGBA fore) {
	// Avoid drawing spaces in transparent mode
	for (const char ch : text) {
		if (ch != ' ') {
			if (pRenderTarget) {
				D2DPenColourAlpha(fore);
				DrawTextCommon(rc, font_, ybase, text, CpUtf8, 0);
			}
			return;
		}
	}
}

void SurfaceD2D::MeasureWidthsUTF8(const Font *font_, std::string_view text, XYPOSITION *positions) {
	SetFont(font_);
	if (!pIDWriteFactory || !pTextFormat) {
		// SetFont failed or no access to DirectWrite so give up.
		return;
	}
	const TextWide tbuf(text, CpUtf8);
	TextPositions poses(tbuf.tlen);
	// Initialize poses for safety.
	std::fill(poses.buffer, poses.buffer + tbuf.tlen, 0.0f);
	// Create a layout
	IDWriteTextLayout *pTextLayout = nullptr;
	const HRESULT hrCreate = pIDWriteFactory->CreateTextLayout(tbuf.buffer, tbuf.tlen, pTextFormat, 10000.0, 1000.0, &pTextLayout);
	if (!SUCCEEDED(hrCreate) || !pTextLayout) {
		return;
	}
	constexpr int clusters = stackBufferLength;
	DWRITE_CLUSTER_METRICS clusterMetrics[clusters];
	UINT32 count = 0;
	const HRESULT hrGetCluster = pTextLayout->GetClusterMetrics(clusterMetrics, clusters, &count);
	ReleaseUnknown(pTextLayout);
	if (!SUCCEEDED(hrGetCluster)) {
		return;
	}
	// A cluster may be more than one WCHAR, such as for "ffi" which is a ligature in the Candara font
	XYPOSITION position = 0.0;
	int ti = 0;
	for (unsigned int ci = 0; ci < count; ci++) {
		for (unsigned int inCluster = 0; inCluster < clusterMetrics[ci].length; inCluster++) {
			poses.buffer[ti++] = position + clusterMetrics[ci].width * (inCluster + 1) / clusterMetrics[ci].length;
		}
		position += clusterMetrics[ci].width;
	}
	PLATFORM_ASSERT(ti == tbuf.tlen);
	// Map the widths given for UTF-16 characters back onto the UTF-8 input string
	size_t i = 0;
	for (int ui = 0; ui < tbuf.tlen; ui++) {
		const unsigned char uch = text[i];
		const unsigned int byteCount = UTF8BytesOfLead[uch];
		if (byteCount == 4) {	// Non-BMP
			ui++;
			PLATFORM_ASSERT(ui < ti);
		}
		for (unsigned int bytePos=0; (bytePos<byteCount) && (i<text.length()); bytePos++) {
			positions[i++] = poses.buffer[ui];
		}
	}
	const XYPOSITION lastPos = (i > 0) ? positions[i - 1] : 0.0;
	while (i < text.length()) {
		positions[i++] = lastPos;
	}
}

XYPOSITION SurfaceD2D::WidthTextUTF8(const Font * font_, std::string_view text) {
	FLOAT width = 1.0;
	SetFont(font_);
	const TextWide tbuf(text, CpUtf8);
	if (pIDWriteFactory && pTextFormat) {
		// Create a layout
		IDWriteTextLayout *pTextLayout = nullptr;
		const HRESULT hr = pIDWriteFactory->CreateTextLayout(tbuf.buffer, tbuf.tlen, pTextFormat, 1000.0, 1000.0, &pTextLayout);
		if (SUCCEEDED(hr)) {
			DWRITE_TEXT_METRICS textMetrics;
			if (SUCCEEDED(pTextLayout->GetMetrics(&textMetrics)))
				width = textMetrics.widthIncludingTrailingWhitespace;
			ReleaseUnknown(pTextLayout);
		}
	}
	return width;
}

XYPOSITION SurfaceD2D::Ascent(const Font *font_) {
	SetFont(font_);
	return std::ceil(yAscent);
}

XYPOSITION SurfaceD2D::Descent(const Font *font_) {
	SetFont(font_);
	return std::ceil(yDescent);
}

XYPOSITION SurfaceD2D::InternalLeading(const Font *font_) {
	SetFont(font_);
	return std::floor(yInternalLeading);
}

XYPOSITION SurfaceD2D::Height(const Font *font_) {
	return Ascent(font_) + Descent(font_);
}

XYPOSITION SurfaceD2D::AverageCharWidth(const Font *font_) {
	FLOAT width = 1.0;
	SetFont(font_);
	if (pIDWriteFactory && pTextFormat) {
		// Create a layout
		IDWriteTextLayout *pTextLayout = nullptr;
		static const WCHAR wszAllAlpha[] = L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
		const size_t lenAllAlpha = wcslen(wszAllAlpha);
		const HRESULT hr = pIDWriteFactory->CreateTextLayout(wszAllAlpha, static_cast<UINT32>(lenAllAlpha),
			pTextFormat, 1000.0, 1000.0, &pTextLayout);
		if (SUCCEEDED(hr) && pTextLayout) {
			DWRITE_TEXT_METRICS textMetrics;
			if (SUCCEEDED(pTextLayout->GetMetrics(&textMetrics)))
				width = textMetrics.width / lenAllAlpha;
			ReleaseUnknown(pTextLayout);
		}
	}
	return width;
}

void SurfaceD2D::SetClip(PRectangle rc) {
	if (pRenderTarget) {
		const D2D1_RECT_F rcClip = RectangleFromPRectangle(rc);
		pRenderTarget->PushAxisAlignedClip(rcClip, D2D1_ANTIALIAS_MODE_ALIASED);
		clipsActive++;
	}
}

void SurfaceD2D::PopClip() {
	if (pRenderTarget) {
		PLATFORM_ASSERT(clipsActive > 0);
		pRenderTarget->PopAxisAlignedClip();
		clipsActive--;
	}
}

void SurfaceD2D::FlushCachedState() {
}

void SurfaceD2D::FlushDrawing() {
	if (pRenderTarget) {
		pRenderTarget->Flush();
	}
}

#endif

std::unique_ptr<Surface> Surface::Allocate(Technology technology) {
#if defined(USE_D2D)
	return std::make_unique<SurfaceD2D>();
#endif
}

Window::~Window() noexcept {
}

void Window::Destroy() noexcept {
	if (wid)
		::DestroyWindow(HwndFromWindowID(wid));
	wid = nullptr;
}

PRectangle Window::GetPosition() const {
	RECT rc;
	::GetWindowRect(HwndFromWindowID(wid), &rc);
	return PRectangle::FromInts(rc.left, rc.top, rc.right, rc.bottom);
}

void Window::SetPosition(PRectangle rc) {
	::SetWindowPos(HwndFromWindowID(wid),
		0, static_cast<int>(rc.left), static_cast<int>(rc.top),
		static_cast<int>(rc.Width()), static_cast<int>(rc.Height()), SWP_NOZORDER | SWP_NOACTIVATE);
}

namespace {

RECT RectFromMonitor(HMONITOR hMonitor) noexcept {
	MONITORINFO mi = {};
	mi.cbSize = sizeof(mi);
	if (GetMonitorInfo(hMonitor, &mi)) {
		return mi.rcWork;
	}
	RECT rc = {0, 0, 0, 0};
	if (::SystemParametersInfoA(SPI_GETWORKAREA, 0, &rc, 0) == 0) {
		rc.left = 0;
		rc.top = 0;
		rc.right = 0;
		rc.bottom = 0;
	}
	return rc;
}

}

void Window::SetPositionRelative(PRectangle rc, const Window *relativeTo) {
	const DWORD style = GetWindowStyle(HwndFromWindowID(wid));
	if (style & WS_POPUP) {
		POINT ptOther = {0, 0};
		::ClientToScreen(HwndFromWindow(*relativeTo), &ptOther);
		rc.Move(static_cast<XYPOSITION>(ptOther.x), static_cast<XYPOSITION>(ptOther.y));

		const RECT rcMonitor = RectFromPRectangle(rc);

		HMONITOR hMonitor = MonitorFromRect(&rcMonitor, MONITOR_DEFAULTTONEAREST);
		// If hMonitor is NULL, that's just the main screen anyways.
		const RECT rcWork = RectFromMonitor(hMonitor);

		if (rcWork.left < rcWork.right) {
			// Now clamp our desired rectangle to fit inside the work area
			// This way, the menu will fit wholly on one screen. An improvement even
			// if you don't have a second monitor on the left... Menu's appears half on
			// one screen and half on the other are just U.G.L.Y.!
			if (rc.right > rcWork.right)
				rc.Move(rcWork.right - rc.right, 0);
			if (rc.bottom > rcWork.bottom)
				rc.Move(0, rcWork.bottom - rc.bottom);
			if (rc.left < rcWork.left)
				rc.Move(rcWork.left - rc.left, 0);
			if (rc.top < rcWork.top)
				rc.Move(0, rcWork.top - rc.top);
		}
	}
	SetPosition(rc);
}

PRectangle Window::GetClientPosition() const {
	RECT rc={0,0,0,0};
	if (wid)
		::GetClientRect(HwndFromWindowID(wid), &rc);
	return PRectangle::FromInts(rc.left, rc.top, rc.right, rc.bottom);
}

void Window::Show(bool show) {
	if (show)
		::ShowWindow(HwndFromWindowID(wid), SW_SHOWNOACTIVATE);
	else
		::ShowWindow(HwndFromWindowID(wid), SW_HIDE);
}

void Window::InvalidateAll() {
	::InvalidateRect(HwndFromWindowID(wid), nullptr, FALSE);
}

void Window::InvalidateRectangle(PRectangle rc) {
	const RECT rcw = RectFromPRectangle(rc);
	::InvalidateRect(HwndFromWindowID(wid), &rcw, FALSE);
}

namespace {

void FlipBitmap(HBITMAP bitmap, int width, int height) noexcept {
	HDC hdc = ::CreateCompatibleDC({});
	if (hdc) {
		HBITMAP prevBmp = SelectBitmap(hdc, bitmap);
		::StretchBlt(hdc, width - 1, 0, -width, height, hdc, 0, 0, width, height, SRCCOPY);
		SelectBitmap(hdc, prevBmp);
		::DeleteDC(hdc);
	}
}

}

HCURSOR LoadReverseArrowCursor(UINT dpi) noexcept {
	HCURSOR reverseArrowCursor {};

	bool created = false;
	HCURSOR cursor = ::LoadCursor({}, IDC_ARROW);

	if (dpi != uSystemDPI) {
		const int width = SystemMetricsForDpi(SM_CXCURSOR, dpi);
		const int height = SystemMetricsForDpi(SM_CYCURSOR, dpi);
		HCURSOR copy = static_cast<HCURSOR>(::CopyImage(cursor, IMAGE_CURSOR, width, height, LR_COPYFROMRESOURCE | LR_COPYRETURNORG));
		if (copy) {
			created = copy != cursor;
			cursor = copy;
		}
	}

	ICONINFO info;
	if (::GetIconInfo(cursor, &info)) {
		BITMAP bmp {};
		if (::GetObject(info.hbmMask, sizeof(bmp), &bmp)) {
			FlipBitmap(info.hbmMask, bmp.bmWidth, bmp.bmHeight);
			if (info.hbmColor)
				FlipBitmap(info.hbmColor, bmp.bmWidth, bmp.bmHeight);
			info.xHotspot = bmp.bmWidth - 1 - info.xHotspot;

			reverseArrowCursor = ::CreateIconIndirect(&info);
		}

		::DeleteObject(info.hbmMask);
		if (info.hbmColor)
			::DeleteObject(info.hbmColor);
	}

	if (created) {
		::DestroyCursor(cursor);
	}
	return reverseArrowCursor;
}

void Window::SetCursor(Cursor curs) {
	switch (curs) {
	case Cursor::text:
		::SetCursor(::LoadCursor(NULL,IDC_IBEAM));
		break;
	case Cursor::up:
		::SetCursor(::LoadCursor(NULL,IDC_UPARROW));
		break;
	case Cursor::wait:
		::SetCursor(::LoadCursor(NULL,IDC_WAIT));
		break;
	case Cursor::horizontal:
		::SetCursor(::LoadCursor(NULL,IDC_SIZEWE));
		break;
	case Cursor::vertical:
		::SetCursor(::LoadCursor(NULL,IDC_SIZENS));
		break;
	case Cursor::hand:
		::SetCursor(::LoadCursor(NULL,IDC_HAND));
		break;
	case Cursor::reverseArrow:
	case Cursor::arrow:
	case Cursor::invalid:	// Should not occur, but just in case.
		::SetCursor(::LoadCursor(NULL,IDC_ARROW));
		break;
	}
}

/* Returns rectangle of monitor pt is on, both rect and pt are in Window's
   coordinates */
PRectangle Window::GetMonitorRect(Point pt) {
	const PRectangle rcPosition = GetPosition();
	POINT ptDesktop = {static_cast<LONG>(pt.x + rcPosition.left),
		static_cast<LONG>(pt.y + rcPosition.top)};
	HMONITOR hMonitor = MonitorFromPoint(ptDesktop, MONITOR_DEFAULTTONEAREST);

	const RECT rcWork = RectFromMonitor(hMonitor);
	if (rcWork.left < rcWork.right) {
		PRectangle rcMonitor(
			rcWork.left - rcPosition.left,
			rcWork.top - rcPosition.top,
			rcWork.right - rcPosition.left,
			rcWork.bottom - rcPosition.top);
		return rcMonitor;
	} else {
		return PRectangle();
	}
}

struct ListItemData {
	const char *text;
	int pixId;
};

class LineToItem {
	std::vector<char> words;

	std::vector<ListItemData> data;

public:
	void Clear() noexcept {
		words.clear();
		data.clear();
	}

	ListItemData Get(size_t index) const noexcept {
		if (index < data.size()) {
			return data[index];
		} else {
			ListItemData missing = {"", -1};
			return missing;
		}
	}
	int Count() const noexcept {
		return static_cast<int>(data.size());
	}

	void AllocItem(const char *text, int pixId) {
		ListItemData lid = { text, pixId };
		data.push_back(lid);
	}

	char *SetWords(const char *s) {
		words = std::vector<char>(s, s+strlen(s)+1);
		return &words[0];
	}
};

const TCHAR ListBoxX_ClassName[] = TEXT("ListBoxX");

ListBox::ListBox() noexcept {
}

ListBox::~ListBox() noexcept {
}

Menu::Menu() noexcept : mid{} {
}

void Menu::CreatePopUp() {
	Destroy();
	mid = ::CreatePopupMenu();
}

void Menu::Destroy() noexcept {
	if (mid)
		::DestroyMenu(static_cast<HMENU>(mid));
	mid = 0;
}

void Menu::Show(Point pt, const Window &w) {
	::TrackPopupMenu(static_cast<HMENU>(mid),
		TPM_RIGHTBUTTON, static_cast<int>(pt.x - 4), static_cast<int>(pt.y), 0,
		HwndFromWindow(w), nullptr);
	Destroy();
}

ColourRGBA Platform::Chrome() {
	return ColourRGBA::FromRGB(static_cast<int>(::GetSysColor(COLOR_3DFACE)));
}

ColourRGBA Platform::ChromeHighlight() {
	return ColourRGBA::FromRGB(static_cast<int>(::GetSysColor(COLOR_3DHIGHLIGHT)));
}

const char *Platform::DefaultFont() {
	return "Verdana";
}

int Platform::DefaultFontSize() {
	return 8;
}

unsigned int Platform::DoubleClickTime() {
	return ::GetDoubleClickTime();
}

void Platform::DebugDisplay(const char *s) noexcept {
	::OutputDebugStringA(s);
}

//#define TRACE

#ifdef TRACE
void Platform::DebugPrintf(const char *format, ...) noexcept {
	char buffer[2000];
	va_list pArguments;
	va_start(pArguments, format);
	vsprintf(buffer,format,pArguments);
	va_end(pArguments);
	Platform::DebugDisplay(buffer);
}
#else
void Platform::DebugPrintf(const char *, ...) noexcept {
}
#endif

static bool assertionPopUps = true;

bool Platform::ShowAssertionPopUps(bool assertionPopUps_) noexcept {
	const bool ret = assertionPopUps;
	assertionPopUps = assertionPopUps_;
	return ret;
}

void Platform::Assert(const char *c, const char *file, int line) noexcept {
	char buffer[2000] {};
	sprintf(buffer, "Assertion [%s] failed at %s %d%s", c, file, line, assertionPopUps ? "" : "\r\n");
	if (assertionPopUps) {
		const int idButton = ::MessageBoxA(0, buffer, "Assertion failure",
			MB_ABORTRETRYIGNORE|MB_ICONHAND|MB_SETFOREGROUND|MB_TASKMODAL);
		if (idButton == IDRETRY) {
			::DebugBreak();
		} else if (idButton == IDIGNORE) {
			// all OK
		} else {
			abort();
		}
	} else {
		Platform::DebugDisplay(buffer);
		::DebugBreak();
		abort();
	}
}

void Platform_Initialise(void *hInstance) noexcept {
	hinstPlatformRes = static_cast<HINSTANCE>(hInstance);
	LoadDpiForWindow();
	ListBoxX_Register();
}

void Platform_Finalise(bool fromDllMain) noexcept {
#if defined(USE_D2D)
	if (!fromDllMain) {
		ReleaseUnknown(defaultRenderingParams);
		ReleaseUnknown(customClearTypeRenderingParams);
		ReleaseUnknown(pIDWriteFactory);
		ReleaseUnknown(pD2DFactory);
		if (hDLLDWrite) {
			FreeLibrary(hDLLDWrite);
			hDLLDWrite = {};
		}
		if (hDLLD2D) {
			FreeLibrary(hDLLD2D);
			hDLLD2D = {};
		}
	}
#endif
	if (!fromDllMain && hDLLShcore) {
		FreeLibrary(hDLLShcore);
		hDLLShcore = {};
	}
	ListBoxX_Unregister();
}

}
