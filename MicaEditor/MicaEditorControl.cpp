#include "pch.h"
#include "MicaEditorControl.h"
#if __has_include("MicaEditorControl.g.cpp")
#include "MicaEditorControl.g.cpp"
#endif

using namespace ::MicaEditor;
using namespace winrt;
using namespace DUX;
using namespace DUX::Controls;
using namespace DUX::Input;
using namespace DUX::Media::Imaging;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;

namespace winrt::MicaEditor::implementation
{
	MicaEditorControl::MicaEditorControl()
	{
		DefaultStyleKey(winrt::box_value(L"MicaEditor.MicaEditorControl"));

		_wrapper = std::make_shared<Wrapper>();

		SizeChanged({ this, &MicaEditorControl::OnSizeChanged });

#ifndef WINUI3
		auto displayInformation{ DisplayInformation::GetForCurrentView() };
		_dpiChangedRevoker = displayInformation.DpiChanged(winrt::auto_revoke, { this, &MicaEditorControl::OnDpiChanged });
		UpdateDisplayInformation(displayInformation);
#endif

		_scintilla = make_self<::Scintilla::Internal::ScintillaWinUI>();

		_scintilla->WndProc(Scintilla::Message::InsertText, 0, reinterpret_cast<Scintilla::uptr_t>(
			"' n! where n is greater than or equal to 0\r\n"
			"Function Factorial(n As Integer) As Integer\r\n"
			"    If n <= 1 Then\r\n"
			"        Factorial = 1\r\n"
			"    Else\r\n"
			"        Factorial = n * Factorial(n - 1)\r\n"
			"    End If\r\n"
			"End Function"));
		_scintilla->WndProc(Scintilla::Message::SetWrapMode, SC_WRAP_WHITESPACE, 0);
		_scintilla->WndProc(Scintilla::Message::SetMarginTypeN, 1, SC_MARGIN_NUMBER);
		_scintilla->WndProc(Scintilla::Message::StyleSetFore, STYLE_LINENUMBER, RGB(0x23, 0x78, 0x93));
		_scintilla->WndProc(Scintilla::Message::SetILexer, 0, reinterpret_cast<Scintilla::uptr_t>(CreateLexer("vbscript")));
		// This list of keywords from SciTe (vb.properties)
		_scintilla->WndProc(Scintilla::Message::SetKeyWords, 0, reinterpret_cast<Scintilla::uptr_t>(
			"addressof alias and as attribute base begin binary boolean byref byte byval call case cdbl "
			"cint clng compare const csng cstr currency date decimal declare defbool defbyte defcur "
			"defdate defdbl defdec defint deflng defobj defsng defstr defvar dim do double each else "
			"elseif empty end enum eqv erase error event exit explicit false for friend function get "
			"global gosub goto if imp implements in input integer is len let lib like load lock long "
			"loop lset me mid midb mod new next not nothing null object on option optional or paramarray "
			"preserve print private property public raiseevent randomize redim rem resume return rset "
			"seek select set single static step stop string sub text then time to true type typeof "
			"unload until variant wend while with withevents xor"));
		// These colors mostly adapted from https://github.com/microsoft/vscode/blob/main/extensions/theme-defaults/themes/light_plus.json
		_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_B_NUMBER, RGB(0x09, 0x86, 0x58));
		_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_B_KEYWORD, RGB(0x00, 0x00, 0xFF));
		_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_B_KEYWORD2, RGB(0xaf, 0x00, 0xdb));
		_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_B_STRING, RGB(0xa3, 0x15, 0x15));
		_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_B_STRING, RGB(0xa3, 0x15, 0x15));
		_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_B_OPERATOR, RGB(0x00, 0x00, 0x00));
		_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_B_PREPROCESSOR, RGB(0x80, 0x80, 0x80));
		_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_B_ERROR, RGB(0xcd, 0x31, 0x31));
		_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_B_COMMENT, RGB(0x00, 0x80, 0x00));
	}

	void MicaEditorControl::OnSizeChanged(const IInspectable &sender, const SizeChangedEventArgs &args)
	{
		if (_vsisNative)
		{
			auto width{ Helpers::ConvertFromDipToPixelUnit(args.NewSize().Width, _dpiScale) };
			auto height{ Helpers::ConvertFromDipToPixelUnit(args.NewSize().Height, _dpiScale) };
			_wrapper->Width(width);
			_wrapper->Height(height);
			_vsisNative->Resize(width, height);
			_scintilla->SizeChanged();
		}
	}

	void MicaEditorControl::OnDpiChanged(DisplayInformation const &sender, IInspectable const &args)
	{
		UpdateDisplayInformation(sender);
		_scintilla->DpiChanged();
	}

	void MicaEditorControl::UpdateDisplayInformation(DisplayInformation const &displayInformation)
	{
		_dpiScale = displayInformation.RawPixelsPerViewPixel();
		_wrapper->DpiScale(_dpiScale); // Todo: May remain unused
		_wrapper->LogicalDpi(displayInformation.LogicalDpi());
		if (_scintilla)
		{
			_scintilla->WndProc(Scintilla::Message::SetMarginWidthN, 1, Helpers::ConvertFromDipToPixelUnit(24, _dpiScale));
			_scintilla->WndProc(Scintilla::Message::SetMarginLeft, 0, Helpers::ConvertFromDipToPixelUnit(8, _dpiScale));
		}
	}

	int32_t MicaEditorControl::MyProperty()
	{
		return _myProperty;
	}

	void MicaEditorControl::MyProperty(int32_t value)
	{
		_myProperty = value;
	}

	uint64_t MicaEditorControl::Scintilla(int32_t message, uint64_t wParam, uint64_t lParam)
	{
		return _scintilla->WndProc(static_cast<Scintilla::Message>(message), wParam, lParam);
	}

	void MicaEditorControl::OnApplyTemplate()
	{
#ifdef WINUI3
		// Temporary until it is known how to respond to DPI changes with WASDK
		_dpiScale = XamlRoot().RasterizationScale();
		_wrapper->DpiScale(_dpiScale);
		_wrapper->LogicalDpi(96 * _dpiScale);
#endif

		_scintilla->WndProc(Scintilla::Message::SetMarginWidthN, 1, Helpers::ConvertFromDipToPixelUnit(24, _dpiScale));
		_scintilla->WndProc(Scintilla::Message::SetMarginLeft, 0, Helpers::ConvertFromDipToPixelUnit(8, _dpiScale));

		uint32_t creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1
		};

		// Create the Direct3D device.
		winrt::com_ptr<::ID3D11Device> d3dDevice;
		D3D_FEATURE_LEVEL supportedFeatureLevel;
		winrt::check_hresult(::D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			0,
			creationFlags,
			featureLevels,
			ARRAYSIZE(featureLevels),
			D3D11_SDK_VERSION,
			d3dDevice.put(),
			&supportedFeatureLevel,
			nullptr)
		);

		// Get the Direct3D device.
		winrt::com_ptr<::IDXGIDevice> dxgiDevice{
			d3dDevice.as<::IDXGIDevice>() };

		// Create the Direct2D device and a corresponding context.
		winrt::com_ptr<::ID2D1Device> d2dDevice;
		::D2D1CreateDevice(dxgiDevice.get(), nullptr, d2dDevice.put());

		winrt::com_ptr<::ID2D1DeviceContext> d2dDeviceContext;
		winrt::check_hresult(
			d2dDevice->CreateDeviceContext(
				D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
				d2dDeviceContext.put()
			)
		);

		auto width{ Helpers::ConvertFromDipToPixelUnit(ActualWidth(), _dpiScale) };
		auto height{ Helpers::ConvertFromDipToPixelUnit(ActualHeight(), _dpiScale) };
		_wrapper->Width(width);
		_wrapper->Height(height);
		VirtualSurfaceImageSource virtualSurfaceImageSource{ width, height };

		winrt::com_ptr<::ISurfaceImageSourceNativeWithD2D> sisNativeWithD2D{
			virtualSurfaceImageSource.as<::ISurfaceImageSourceNativeWithD2D>() };

		// Associate the Direct2D device with the SurfaceImageSource.
		sisNativeWithD2D->SetDevice(d2dDevice.get());

		// To enable multi-threaded access (optional)
		winrt::com_ptr<::ID3D11Multithread> d3dMultiThread{
			d3dDevice.as<::ID3D11Multithread>() };
		d3dMultiThread->SetMultithreadProtected(true);

		_vsisNative = virtualSurfaceImageSource.as<::IVirtualSurfaceImageSourceNative>();

		_wrapper->VsisNative(_vsisNative);
		_scintilla->RegisterGraphics(sisNativeWithD2D, _vsisNative, d2dDeviceContext, _wrapper);
		_vsisNative->RegisterForUpdatesNeeded(_scintilla.as<::IVirtualSurfaceUpdatesCallbackNative>().get());

		// The SurfaceImageSource object's underlying 
		// ISurfaceImageSourceNativeWithD2D object will contain the completed bitmap.

		if (auto imageTarget{ GetTemplateChild(L"ImageTarget").try_as<Image>() })
		{
			imageTarget.Source(virtualSurfaceImageSource);
			//imageTarget.Tapped({ this, &MicaEditorControl::Image_Tapped });
		}
	}

	void MicaEditorControl::OnGotFocus(RoutedEventArgs const &e)
	{
		__super::OnGotFocus(e);

		_scintilla->FocusChanged(true);
	}

	void MicaEditorControl::OnLostFocus(RoutedEventArgs const &e)
	{
		__super::OnLostFocus(e);

		//_scintilla->FocusChanged(false);
		// Todo: Focus does not seem to be gained when clicking on the editor
	}

	void MicaEditorControl::OnPointerPressed(PointerRoutedEventArgs const &e)
	{
		__super::OnPointerPressed(e);

		if (auto imageTarget{ GetTemplateChild(L"ImageTarget").try_as<UIElement>() }) // Todo: Store this
		{
			auto point{ e.GetCurrentPoint(imageTarget) };
			auto scaled{ point.Position() };
			_scintilla->PointerPressed(Point{ scaled.X * _dpiScale, scaled.Y * _dpiScale }, point.Timestamp(), e.KeyModifiers());
		}
	}

	void MicaEditorControl::Image_Tapped(IInspectable const &sender, TappedRoutedEventArgs const &args)
	{
		_scintilla->WndProc(Scintilla::Message::AppendText, 15, reinterpret_cast<Scintilla::uptr_t>("\r\n' Insert text"));
	}
}
