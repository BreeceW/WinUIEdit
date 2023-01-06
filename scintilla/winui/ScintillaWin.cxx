// Scintilla source code edit control
/** @file ScintillaWin.cxx
 ** Windows specific subclass of ScintillaBase.
 **/
 // Copyright 1998-2003 by Neil Hodgson <neilh@scintilla.org>
 // The License.txt file describes the conditions under which this software may be distributed.

#include "pch.h"

#include <cstddef>
#include <cstdlib>
#include <cstdint>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <climits>

#include <stdexcept>
#include <new>
#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <set>
#include <optional>
#include <algorithm>
#include <memory>
#include <chrono>
#include <mutex>

// Want to use std::min and std::max so don't want Windows.h version of min and max
#if !defined(NOMINMAX)
#define NOMINMAX
#endif
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#undef WINVER
#define WINVER 0x0A00
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#include <commctrl.h>
#include <richedit.h>
#include <windowsx.h>
//#include <zmouse.h>
// WinUI Todo
#include <ole2.h>

#if !defined(DISABLE_D2D)
#define USE_D2D 1
#endif

#if defined(USE_D2D)
#include <d2d1.h>
#include <dwrite.h>
#endif

#include "ScintillaTypes.h"
#include "ScintillaMessages.h"
#include "ScintillaStructures.h"
#include "ILoader.h"
#include "ILexer.h"

#include "Debugging.h"
#include "Geometry.h"
#include "Platform.h"

#include "CharacterCategoryMap.h"
#include "Position.h"
#include "UniqueString.h"
#include "SplitVector.h"
#include "Partitioning.h"
#include "RunStyles.h"
#include "ContractionState.h"
#include "CellBuffer.h"
#include "CallTip.h"
#include "KeyMap.h"
#include "Indicator.h"
#include "LineMarker.h"
#include "Style.h"
#include "ViewStyle.h"
#include "CharClassify.h"
#include "Decoration.h"
#include "CaseFolder.h"
#include "Document.h"
#include "CaseConvert.h"
#include "UniConversion.h"
#include "Selection.h"
#include "PositionCache.h"
#include "EditModel.h"
#include "MarginView.h"
#include "EditView.h"
#include "Editor.h"
#include "ElapsedPeriod.h"

#include "AutoComplete.h"
#include "ScintillaBase.h"

#include "WinTypes.h"
#include "PlatWin.h"
#include "ScintillaWin.h"

void WINAPIV DebugOut(const wchar_t *fmt, ...) {
	wchar_t s[2000];
	va_list args;
	va_start(args, fmt);
	vswprintf(s, 2000, fmt, args);
	va_end(args);
	OutputDebugStringW(s);
}

namespace Scintilla::Internal {

	UINT CodePageFromCharSet(CharacterSet characterSet, UINT documentCodePage) noexcept {
		if (documentCodePage == CpUtf8) {
			return CpUtf8;
		}
		switch (characterSet) {
		case CharacterSet::Ansi: return 1252;
		case CharacterSet::Default: return documentCodePage ? documentCodePage : 1252;
		case CharacterSet::Baltic: return 1257;
		case CharacterSet::ChineseBig5: return 950;
		case CharacterSet::EastEurope: return 1250;
		case CharacterSet::GB2312: return 936;
		case CharacterSet::Greek: return 1253;
		case CharacterSet::Hangul: return 949;
		case CharacterSet::Mac: return 10000;
		case CharacterSet::Oem: return 437;
		case CharacterSet::Russian: return 1251;
		case CharacterSet::ShiftJis: return 932;
		case CharacterSet::Turkish: return 1254;
		case CharacterSet::Johab: return 1361;
		case CharacterSet::Hebrew: return 1255;
		case CharacterSet::Arabic: return 1256;
		case CharacterSet::Vietnamese: return 1258;
		case CharacterSet::Thai: return 874;
		case CharacterSet::Iso8859_15: return 28605;
			// Not supported
		case CharacterSet::Cyrillic: return documentCodePage;
		case CharacterSet::Symbol: return documentCodePage;
		default: break;
		}
		return documentCodePage;
	}

	int InputCodePage() noexcept {
		/*HKL inputLocale = ::GetKeyboardLayout(0);
		const LANGID inputLang = LOWORD(inputLocale);
		char sCodePage[10];
		const int res = ::GetLocaleInfoA(MAKELCID(inputLang, SORT_DEFAULT),
		  LOCALE_IDEFAULTANSICODEPAGE, sCodePage, sizeof(sCodePage));
		if (!res)
			return 0;
		return atoi(sCodePage);*/
		return 0;
		// WinUI Todo
	}

	/** Map the key codes to their equivalent Keys:: form. */
	Keys KeyTranslate(uptr_t keyIn) noexcept {
		switch (keyIn) {
		case VK_DOWN:		return Keys::Down;
		case VK_UP:		return Keys::Up;
		case VK_LEFT:		return Keys::Left;
		case VK_RIGHT:		return Keys::Right;
		case VK_HOME:		return Keys::Home;
		case VK_END:		return Keys::End;
		case VK_PRIOR:		return Keys::Prior;
		case VK_NEXT:		return Keys::Next;
		case VK_DELETE:	return Keys::Delete;
		case VK_INSERT:		return Keys::Insert;
		case VK_ESCAPE:	return Keys::Escape;
		case VK_BACK:		return Keys::Back;
		case VK_TAB:		return Keys::Tab;
		case VK_RETURN:	return Keys::Return;
		case VK_ADD:		return Keys::Add;
		case VK_SUBTRACT:	return Keys::Subtract;
		case VK_DIVIDE:		return Keys::Divide;
		case VK_LWIN:		return Keys::Win;
		case VK_RWIN:		return Keys::RWin;
		case VK_APPS:		return Keys::Menu;
		case VK_OEM_2:		return static_cast<Keys>('/');
		case VK_OEM_3:		return static_cast<Keys>('`');
		case VK_OEM_4:		return static_cast<Keys>('[');
		case VK_OEM_5:		return static_cast<Keys>('\\');
		case VK_OEM_6:		return static_cast<Keys>(']');
		default:			return static_cast<Keys>(keyIn);
		}
	}

	bool BoundsContains(PRectangle rcBounds, HRGN hRgnBounds, PRectangle rcCheck) noexcept {
		/*bool contains = true;
		if (!rcCheck.Empty()) {
			if (!rcBounds.Contains(rcCheck)) {
				contains = false;
			} else if (hRgnBounds) {
				// In bounding rectangle so check more accurately using region
				const RECT rcw = RectFromPRectangle(rcCheck);
				HRGN hRgnCheck = ::CreateRectRgnIndirect(&rcw);
				if (hRgnCheck) {
					HRGN hRgnDifference = ::CreateRectRgn(0, 0, 0, 0);
					if (hRgnDifference) {
						const int combination = ::CombineRgn(hRgnDifference, hRgnCheck, hRgnBounds, RGN_DIFF);
						if (combination != NULLREGION) {
							contains = false;
						}
						::DeleteRgn(hRgnDifference);
					}
					::DeleteRgn(hRgnCheck);
				}
			}
		}
		return contains;*/
		return false;
		// WinUI Todo
	}

	// Simplify calling WideCharToMultiByte and MultiByteToWideChar by providing default parameters and using string view.

	int MultiByteFromWideChar(UINT codePage, std::wstring_view wsv, LPSTR lpMultiByteStr, ptrdiff_t cbMultiByte) noexcept {
		return ::WideCharToMultiByte(codePage, 0, wsv.data(), static_cast<int>(wsv.length()), lpMultiByteStr, static_cast<int>(cbMultiByte), nullptr, nullptr);
	}

	int MultiByteLenFromWideChar(UINT codePage, std::wstring_view wsv) noexcept {
		return MultiByteFromWideChar(codePage, wsv, nullptr, 0);
	}

	int WideCharFromMultiByte(UINT codePage, std::string_view sv, LPWSTR lpWideCharStr, ptrdiff_t cchWideChar) noexcept {
		return ::MultiByteToWideChar(codePage, 0, sv.data(), static_cast<int>(sv.length()), lpWideCharStr, static_cast<int>(cchWideChar));
	}

	int WideCharLenFromMultiByte(UINT codePage, std::string_view sv) noexcept {
		return WideCharFromMultiByte(codePage, sv, nullptr, 0);
	}

	std::string StringEncode(std::wstring_view wsv, int codePage) {
		const int cchMulti = wsv.length() ? MultiByteLenFromWideChar(codePage, wsv) : 0;
		std::string sMulti(cchMulti, 0);
		if (cchMulti) {
			MultiByteFromWideChar(codePage, wsv, sMulti.data(), cchMulti);
		}
		return sMulti;
	}

	std::wstring StringDecode(std::string_view sv, int codePage) {
		const int cchWide = sv.length() ? WideCharLenFromMultiByte(codePage, sv) : 0;
		std::wstring sWide(cchWide, 0);
		if (cchWide) {
			WideCharFromMultiByte(codePage, sv, sWide.data(), cchWide);
		}
		return sWide;
	}

	std::wstring StringMapCase(std::wstring_view wsv, DWORD mapFlags) {
		/*const int charsConverted = ::LCMapStringW(LOCALE_SYSTEM_DEFAULT, mapFlags,
			wsv.data(), static_cast<int>(wsv.length()), nullptr, 0);
		std::wstring wsConverted(charsConverted, 0);
		if (charsConverted) {
			::LCMapStringW(LOCALE_SYSTEM_DEFAULT, mapFlags,
				wsv.data(), static_cast<int>(wsv.length()), wsConverted.data(), charsConverted);
		}
		return wsConverted;*/
		return std::wstring{ wsv }; // WinUI Todo
	}

	constexpr uint8_t NotifyMessageId = 1;
	class NotifyMessage
		: public IMessage
	{
	private:
		uptr_t _wParam;
		NotificationData _notificationData;
		bool _notifyTsf;

	public:
		NotifyMessage::NotifyMessage(uptr_t wParam, NotificationData notificationData, bool notifyTsf)
		{
			_wParam = wParam;
			_notificationData = notificationData;
			_notifyTsf = notifyTsf;
		}

		uint8_t NotifyMessage::Type() const override
		{
			return NotifyMessageId;
		}

		uptr_t NotifyMessage::WParam() const
		{
			return _wParam;
		}

		bool NotifyMessage::NotifyTsf() const
		{
			return _notifyTsf;
		}

		NotificationData const &NotifyMessage::NotificationData() const
		{
			return _notificationData;
		}
	};

	constexpr uint8_t KeyMessageId = 2;
	class KeyMessage
		: public IMessage
	{
	public:
		enum class KeyEventType
		{
			KeyDown,
		};

	private:
		KeyEventType _keyEvent;
		winrt::Windows::System::VirtualKey _key;
		winrt::Windows::System::VirtualKeyModifiers _modifiers;

	public:
		KeyMessage::KeyMessage(KeyEventType keyEvent, winrt::Windows::System::VirtualKey key, winrt::Windows::System::VirtualKeyModifiers modifiers)
		{
			_keyEvent = keyEvent;
			_key = key;
			_modifiers = modifiers;
		}

		uint8_t KeyMessage::Type() const override
		{
			return KeyMessageId;
		}

		KeyEventType KeyMessage::KeyEvent() const
		{
			return _keyEvent;
		}

		winrt::Windows::System::VirtualKey Key() const
		{
			return _key;
		}

		winrt::Windows::System::VirtualKeyModifiers Modifiers() const
		{
			return _modifiers;
		}
	};

	constexpr uint8_t PointerMessageId = 3;
	class PointerMessage
		: public IMessage
	{
	public:
		enum class PointerEventType
		{
			PointerPressed,
			RightPointerPressed,
			PointerMoved,
			PointerReleased,
		};

	private:
		PointerEventType _pointerEvent;
		winrt::Windows::Foundation::Point _point;
		uint64_t _timestamp;
		winrt::Windows::System::VirtualKeyModifiers _modifiers;

	public:
		PointerMessage::PointerMessage(PointerEventType pointerEvent, winrt::Windows::Foundation::Point point, uint64_t timestamp, winrt::Windows::System::VirtualKeyModifiers modifiers)
		{
			_pointerEvent = pointerEvent;
			_point = point;
			_timestamp = timestamp;
			_modifiers = modifiers;
		}

		uint8_t PointerMessage::Type() const override
		{
			return PointerMessageId;
		}

		PointerEventType PointerMessage::PointerEvent() const
		{
			return _pointerEvent;
		}

		winrt::Windows::Foundation::Point const &Point()
		{
			return _point;
		}

		uint64_t Timestamp()
		{
			return _timestamp;
		}

		winrt::Windows::System::VirtualKeyModifiers Modifiers()
		{
			return _modifiers;
		}
	};

	constexpr uint8_t CharacterRecievedMessageId = 4;
	class CharacterRecievedMessage
		: public IMessage
	{
	private:
		char16_t _character;

	public:
		CharacterRecievedMessage(char16_t character)
		{
			_character = character;
		}

		uint8_t CharacterRecievedMessage::Type() const override
		{
			return CharacterRecievedMessageId;
		}

		char16_t Character() const
		{
			return _character;
		}
	};

	ScintillaWinUI::ScintillaWinUI()
	{
		// Todo: Note that Windows Terminal repo may be able to help find a way to use TSF instead of WM_CHAR.
		// Islands eats the text entry too
		// This probably won't work, since it seems mandatory for win32 (https://stackoverflow.com/a/41452204/16152265)
		// But maybe CoreWindow handles it somehow
		// I THINK Windows Terminal is using KeyDown, but I do not know how it is translating
		// In WT Islands, TextUpdating is never called for keyboard, but it is in UWP.
		// The KeyDown is also triggered, but I do not know which is actually doing the input (does it matter?)
		// It does not seem like WT ever uses WM_CHAR for input (maybe)
		// This suggests that the best course of action is to figure out how to translate the KeyDown into input, and possibly use that input to call the TSF function
		// Though it may be better to use the KeyDown on both UWP and win32 (can't figure out which WT UWP uses to actually input)

		MULTI_QI mq = { &__uuidof(ITfThreadMgr2), nullptr, 0 };
		_tsfCore = FAILED(CoCreateInstanceFromApp(CLSID_TF_ThreadMgr, nullptr, CLSCTX_INPROC_SERVER, nullptr, 1, &mq)); // Todo NOTE: On HoloLens 2, this succeeds, even though it doesn't seem to work
		if (_tsfCore)
		{
			//winrt::Windows::UI::Popups::MessageDialog{ L"TSF Core", L"Input method" }.ShowAsync();
			auto manager{ winrt::Windows::UI::Text::Core::CoreTextServicesManager::GetForCurrentView() };
			_editContext = manager.CreateEditContext();
			_editContext.InputScope(winrt::Windows::UI::Text::Core::CoreTextInputScope::Text); // Todo: make sure this is needed. Update: Unset value is 0, Default
			_textRequestedRevoker = _editContext.TextRequested(winrt::auto_revoke, { this, &ScintillaWinUI::OnTextRequested });
			_selectionRequestedRevoker = _editContext.SelectionRequested(winrt::auto_revoke, { this, &ScintillaWinUI::OnSelectionRequested });
			_focusRemovedRevoker = _editContext.FocusRemoved(winrt::auto_revoke, { this, &ScintillaWinUI::OnFocusRemoved });
			_textUpdatingRevoker = _editContext.TextUpdating(winrt::auto_revoke, { this, &ScintillaWinUI::OnTextUpdating });
			_selectionUpdatingRevoker = _editContext.SelectionUpdating(winrt::auto_revoke, { this, &ScintillaWinUI::OnSelectionUpdating });
			_formatUpdatingRevoker = _editContext.FormatUpdating(winrt::auto_revoke, { this, &ScintillaWinUI::OnFormatUpdating });
			_layoutRequestedRevoker = _editContext.LayoutRequested(winrt::auto_revoke, { this, &ScintillaWinUI::OnLayoutRequested });
			_compositionStartedRevoker = _editContext.CompositionStarted(winrt::auto_revoke, { this, &ScintillaWinUI::OnCompositionStarted });
			_compositionCompletedRevoker = _editContext.CompositionCompleted(winrt::auto_revoke, { this, &ScintillaWinUI::OnCompositionCompleted });
			// Todo: needs to close touch keyboard on backspace etc.
		}
		else
		{
			//winrt::Windows::UI::Popups::MessageDialog{ L"TSF Classic", L"Input method" }.ShowAsync();
			winrt::check_hresult(mq.hr);
			winrt::copy_from_abi(_tfThreadManager, mq.pItf);
			winrt::check_hresult(_tfThreadManager->Activate(&_tfClientId));
			winrt::check_hresult(_tfThreadManager->CreateDocumentMgr(_tfDocumentManager.put()));
			winrt::com_ptr<IUnknown> t{ nullptr };
			this->QueryInterface(__uuidof(IUnknown), t.put_void()); // Todo: See if there is a better way to get this
			winrt::check_hresult(_tfDocumentManager->CreateContext(_tfClientId, 0, t.get(), _tfContext.put(), &_tfEditCookie));
			winrt::check_hresult(_tfDocumentManager->Push(_tfContext.get()));
			// Todo: Some of this might have to be manually deinitialized. Namely, the Push part (Pop?)
		}

		_caretTickRevoker = _caretTimer.Tick(winrt::auto_revoke, { this, &ScintillaWinUI::OnCaretTimerTick });
		_scrollTickRevoker = _scrollTimer.Tick(winrt::auto_revoke, { this, &ScintillaWinUI::OnScrollTimerTick });
		_widenTickRevoker = _widenTimer.Tick(winrt::auto_revoke, { this, &ScintillaWinUI::OnWidenTimerTick });
		_dwellTickRevoker = _dwellTimer.Tick(winrt::auto_revoke, { this, &ScintillaWinUI::OnDwellTimerTick });

		technology = Scintilla::Technology::DirectWrite; // Todo: This should be the default anyway
		if (!LoadD2D())
		{
			winrt::throw_hresult(E_UNEXPECTED); // Todo: Better exception
		}
	}

	Scintilla::KeyMod ScintillaWinUI::WindowsModifiers(winrt::Windows::System::VirtualKeyModifiers modifiers)
	{
		return ModifierFlags(
			static_cast<int>(modifiers) & static_cast<int>(winrt::Windows::System::VirtualKeyModifiers::Shift),
			static_cast<int>(modifiers) & static_cast<int>(winrt::Windows::System::VirtualKeyModifiers::Control),
			static_cast<int>(modifiers) & static_cast<int>(winrt::Windows::System::VirtualKeyModifiers::Menu), // Menu is alt key
			false, // No meta key
			static_cast<int>(modifiers) & static_cast<int>(winrt::Windows::System::VirtualKeyModifiers::Windows)); // Super key is actually Windows logo key
	}

	void ScintillaWinUI::AddWString(std::wstring_view wsv, CharacterSource charSource) {
		if (wsv.empty())
		{
			return;
		}

		const int codePage = CodePageOfDocument();
		for (size_t i = 0; i < wsv.size(); ) {
			const size_t ucWidth = UTF16CharLength(wsv[i]);
			const std::string docChar = StringEncode(wsv.substr(i, ucWidth), codePage);

			InsertCharacter(docChar, charSource);
			i += ucWidth;
		}
	}

	std::string ScintillaWinUI::EncodeWString(std::wstring_view wsv) {
		if (IsUnicodeMode()) {
			const size_t len = UTF8Length(wsv);
			std::string putf(len, 0);
			UTF8FromUTF16(wsv, putf.data(), len);
			return putf;
		}
		else {
			// Not in Unicode mode so convert from Unicode to current Scintilla code page
			return StringEncode(wsv, CodePageOfDocument());
		}
	}

	void ScintillaWinUI::ProcessCharacterRecievedMessage(char16_t character)
	{
		// Todo: Make sure this updates TSF caret position correctly

		if (((character >= 128) || !iscntrl(static_cast<int>(character))) || !lastKeyDownConsumed) {
			wchar_t wcs[3] = { static_cast<wchar_t>(character), 0 };
			unsigned int wclen = 1;
			if (IS_HIGH_SURROGATE(wcs[0])) { // There is a WinRT API UnicodeCharacters.IsHighSurrogate for this, but it is probably lower performance for no gain
				// If this is a high surrogate character, we need a second one
				lastHighSurrogateChar = wcs[0];
				return;
			}
			else if (IS_LOW_SURROGATE(wcs[0])) {
				wcs[1] = wcs[0];
				wcs[0] = lastHighSurrogateChar;
				lastHighSurrogateChar = 0;
				wclen = 2;
			}

			TS_TEXTCHANGE chg;
			chg.acpStart = DocPositionToAcp(SelectionStart().Position());
			chg.acpOldEnd = DocPositionToAcp(SelectionEnd().Position());
			_shouldNotifyTsf = false;
			AddWString(std::wstring_view(wcs, wclen), CharacterSource::DirectInput);
			chg.acpNewEnd = chg.acpOldEnd + wclen;
			_shouldNotifyTsf = true;
			if (_tsfCore)
			{
				_editContext.NotifyTextChanged(winrt::Windows::UI::Text::Core::CoreTextRange{ chg.acpStart, chg.acpNewEnd }, chg.acpNewEnd - chg.acpStart, winrt::Windows::UI::Text::Core::CoreTextRange{ chg.acpNewEnd, chg.acpNewEnd });
				_editContext.NotifySelectionChanged(winrt::Windows::UI::Text::Core::CoreTextRange{ static_cast<int32_t>(DocPositionToAcp(SelectionStart().Position())), static_cast<int32_t>(DocPositionToAcp(SelectionEnd().Position())) });
		}
			else if (_tfTextStoreACPSink)
			{
				_tfTextStoreACPSink->OnTextChange(0, &chg);
				_tfTextStoreACPSink->OnSelectionChange();
	}
		}
	}

	// Todo: wParam might not make sense here
	void ScintillaWinUI::ProcessNotifyMessage(uptr_t wParam, NotificationData const &notificationData, bool notifyTsf)
	{
		SendMessage(WM_NOTIFY, GetCtrlID(), reinterpret_cast<LPARAM>(&notificationData));

		if (!notifyTsf)
		{
			return;
		}

		if (notificationData.nmhdr.code == Scintilla::Notification::Modified)
		{
			bool fNotify(false);
			static TS_TEXTCHANGE chg;
			// Lots of notifications get routed here.  We're only interested in insert/delete

			// insert operations have before/after notifications, so we save up info
			if (FlagSet(notificationData.modificationType, Scintilla::ModificationFlags::BeforeInsert))
			{
				chg.acpStart = notificationData.position;
			}
			if (FlagSet(notificationData.modificationType, Scintilla::ModificationFlags::InsertText))
			{
				fNotify = true;
				chg.acpOldEnd = notificationData.position;
				chg.acpNewEnd = notificationData.position + notificationData.length;
			}
			if (FlagSet(notificationData.modificationType, Scintilla::ModificationFlags::BeforeDelete))
			{
				chg.acpStart = notificationData.position;
			}
			if (FlagSet(notificationData.modificationType, Scintilla::ModificationFlags::DeleteText))
			{
				fNotify = true;
				chg.acpOldEnd = notificationData.position + notificationData.length;
				chg.acpNewEnd = notificationData.position;
			}
			if (fNotify)
			{
				if (_tsfCore)
				{
					_editContext.NotifyTextChanged(winrt::Windows::UI::Text::Core::CoreTextRange{ chg.acpStart, chg.acpNewEnd }, chg.acpNewEnd - chg.acpStart, winrt::Windows::UI::Text::Core::CoreTextRange{ chg.acpNewEnd, chg.acpNewEnd });
				}
				else if (_tfTextStoreACPSink)
				{
					DebugOut(L"OnTextChange\n");
					_tfTextStoreACPSink->OnTextChange(0, &chg);
				}
			}
		}
		else if (FlagSet(notificationData.nmhdr.code, Scintilla::Notification::UpdateUI))
		{
			if (_tsfCore)
			{
				_editContext.NotifyLayoutChanged();
			}
			else if (_tfTextStoreACPSink)
			{
				_tfTextStoreACPSink->OnLayoutChange(TS_LC_CHANGE, 0);
			}
		}
	}

	void ScintillaWinUI::ProcessMessage(std::unique_ptr<IMessage> const &message)
	{
		switch (message->Type())
		{
		case CharacterRecievedMessageId:
		{
			const auto characterRecievedMessage{ static_cast<CharacterRecievedMessage *>(message.get()) };
			ProcessCharacterRecievedMessage(characterRecievedMessage->Character());
		}
		break;

		case KeyMessageId:
		{
			const auto keyMessage{ static_cast<KeyMessage *>(message.get())};
			switch (keyMessage->KeyEvent())
			{
			case KeyMessage::KeyEventType::KeyDown:
				ProcessKeyDownMessage(keyMessage->Key(), keyMessage->Modifiers());
				break;
			}
		}
		break;

		case NotifyMessageId:
		{
			const auto notifyMessage{ static_cast<NotifyMessage *>(message.get()) };
			ProcessNotifyMessage(notifyMessage->WParam(), notifyMessage->NotificationData(), notifyMessage->NotifyTsf());
		}
		break;
		}
	}

	void ScintillaWinUI::ProcessKeyDownMessage(winrt::Windows::System::VirtualKey key, winrt::Windows::System::VirtualKeyModifiers modifiers)
	{
		lastKeyDownConsumed = false;
		/*const bool altDown = KeyboardIsKeyDown(VK_MENU);
		if (altDown && KeyboardIsNumericKeypadFunction(wParam, lParam)) {
			// Don't interpret these as they may be characters entered by number.
			return;
		}*/
		// Todo: Number pad key
		// Not sure if need KeyStatus.IsMenuKeyDown	(alt)
		/*const int ret = */KeyDownWithModifiers(
			KeyTranslate(static_cast<uptr_t>(key)),
			WindowsModifiers(modifiers),
			&lastKeyDownConsumed);
		// On Win32, the newline is inserted by CharacterRecieved. This works fine
		// On UWP, the tab key event works because TSF seems to automatically check for the cursor position after the key is pressed regardless of what we do
		// UPDATE: in order to prevent XAML from moving focus on tab, we mark the tab key as handled, which prevents TSF from getting it, so now we do the same for tab
		// It does not do this for delete or backspace, but I don't think it would need to
		// It does not do this for enter, so this is an ugly special case. Space is handled by SetText anyway
		// Todo: There might be some other whitespace keys that cause a similar problem!
		// Todo: Win32 gets \r, UWP gets \r\n
		if ((key == winrt::Windows::System::VirtualKey::Enter || key == winrt::Windows::System::VirtualKey::Tab)
			&& (modifiers == winrt::Windows::System::VirtualKeyModifiers::None || modifiers == winrt::Windows::System::VirtualKeyModifiers::Shift))
		{
			if (_tsfCore)
			{
				_editContext.NotifySelectionChanged(winrt::Windows::UI::Text::Core::CoreTextRange{ static_cast<int32_t>(DocPositionToAcp(SelectionStart().Position())), static_cast<int32_t>(DocPositionToAcp(SelectionEnd().Position())) });
			}
			else
			{
				if (_tfTextStoreACPSink && _lock == NONE)
				{
					_tfTextStoreACPSink->OnSelectionChange();
				}
			}
		}
		// Todo: not sure if we need return value
	}

	void ScintillaWinUI::ImeEndComposition()
	{
		// Called when IME Window closed
		// clear IME composition state.
		view.imeCaretBlockOverride = false;
		pdoc->TentativeUndo();
		ShowCaretAtCurrentPosition();
	}

	void ScintillaWinUI::CharacterReceived(char16_t character)
	{
		if (_lock != NONE)
		{
			msgq.push(std::make_unique<CharacterRecievedMessage>(character));
		}
		else
		{
			ProcessCharacterRecievedMessage(character);
		}
	}

	// Todo: conflated with the incorrectly spelled version
	void ScintillaWinUI::Finalize()
	{
		ScintillaBase::Finalise();
		_caretTimer.Stop();
		_scrollTimer.Stop();
		_widenTimer.Stop();
		_dwellTimer.Stop();
		SetIdle(false);
		//DropRenderTarget(); // WinUI Todo
		/*::RevokeDragDrop(MainHWND());
		if (SUCCEEDED(hrOle)) {
			::OleUninitialize();
		}*/
		// WinUI Todo
	}

	// TSF WinRT

	void ScintillaWinUI::OnTextRequested(winrt::Windows::UI::Text::Core::CoreTextEditContext const &sender, winrt::Windows::UI::Text::Core::CoreTextTextRequestedEventArgs const &args)
	{
		// Todo: Probably don't return whole document if acpEnd is -1. Callers will ask for more if they need it.

		auto acpStart{ args.Request().Range().StartCaretPosition };
		auto acpEnd{ args.Request().Range().EndCaretPosition };
		// acpEnd can be -1, initialize it properly.
		if (acpEnd == -1)
		{
			acpEnd = pdoc->Length();
		}

		if ((acpStart < 0) || (acpStart > acpEnd) || (acpEnd > pdoc->Length()))
		{
			return;
		}

		auto length{ acpEnd - acpStart };
		auto size{ length + 1 };
		auto buff{ new wchar_t[size] };
		GetText(size, reinterpret_cast<sptr_t>(buff));
		args.Request().Text(buff);
		delete[] buff;
		// Todo: incomplete
	}

	void ScintillaWinUI::OnSelectionRequested(winrt::Windows::UI::Text::Core::CoreTextEditContext const &sender, winrt::Windows::UI::Text::Core::CoreTextSelectionRequestedEventArgs const &args)
	{
		DebugOut(L"OnSelectionRequested, Start: %d, End: %d\n", DocPositionToAcp(SelectionStart().Position()), DocPositionToAcp(SelectionEnd().Position()));
		args.Request().Selection(winrt::Windows::UI::Text::Core::CoreTextRange{ static_cast<int32_t>(DocPositionToAcp(SelectionStart().Position())), static_cast<int32_t>(DocPositionToAcp(SelectionEnd().Position())) });
	}

	void ScintillaWinUI::OnFocusRemoved(winrt::Windows::UI::Text::Core::CoreTextEditContext const &sender, winrt::Windows::Foundation::IInspectable const &args)
	{
		SetFocusState(false);
	}

	void ScintillaWinUI::OnTextUpdating(winrt::Windows::UI::Text::Core::CoreTextEditContext const &sender, winrt::Windows::UI::Text::Core::CoreTextTextUpdatingEventArgs const &args)
	{
		DebugOut(L"OnTextUpdating\n");

		if (pdoc->IsReadOnly())
		{
			args.Result(winrt::Windows::UI::Text::Core::CoreTextTextUpdatingResult::Failed);
		}

		auto docStart{ AcpToDocPosition(args.Range().StartCaretPosition) };
		auto docEnd{ AcpToDocPosition(args.Range().EndCaretPosition) };

		// set text by deleting existing chars and inserting new text
		auto startPos{ pdoc->MovePositionOutsideChar(docStart, -1, true) };
		auto endPos{ pdoc->MovePositionOutsideChar(docEnd, 1, true) };
		auto len{ endPos - startPos };
		auto utf16len{ pdoc->CountUTF16(startPos, endPos) };
		pdoc->BeginUndoAction();
		pdoc->DeleteChars(startPos, len);
		int cchText{ -1 };
		char *szText{ nullptr };
		if (IsUnicodeMode())
		{
			cchText = UTF8Length(args.Text());
			szText = new char[cchText + 1];
			UTF8FromUTF16(args.Text(), szText, cchText);
		}
		else
		{
			UINT cpDest = CodePageFromCharSet(vs.styles[STYLE_DEFAULT].characterSet, pdoc->dbcsCodePage);
			cchText = ::WideCharToMultiByte(cpDest, 0, args.Text().c_str(), args.Text().size(),
				NULL, 0, NULL, NULL);
			szText = new char[cchText + 1];
			if (szText) {
				::WideCharToMultiByte(cpDest, 0, args.Text().c_str(), args.Text().size(),
					szText, cchText, NULL, NULL);
				szText[cchText] = '\0';
			}
		}
		pdoc->InsertString(startPos, szText, cchText);

		EnsureCaretVisible();
		ShowCaretAtCurrentPosition(); // Todo: Reevaluate how text at the current position is inserted (this is copied out of InsertCharacter)
		if (cchText == 1)
		{
			NotifyChar(szText[0], Scintilla::CharacterSource::DirectInput); // Todo: This is temporary
		}

		delete[] szText;
		/*if (pChange)
		{
			auto newAcpStart{ DocPositionToAcp(startPos) };
			pChange->acpStart = newAcpStart;
			pChange->acpOldEnd = newAcpStart + utf16len;
			pChange->acpNewEnd = newAcpStart + cch;
			DebugOut(L"SetText, Start: %d, Old End: %d, New End: %d\n", pChange->acpStart, pChange->acpOldEnd, pChange->acpNewEnd);
		}*/
		auto newSelection{ args.NewSelection() };
		// Todo: Do not duplicate
		__super::SetSelection(AcpToDocPosition(newSelection.StartCaretPosition), AcpToDocPosition(newSelection.EndCaretPosition));
		pdoc->EndUndoAction();
		NotifyChange();
		Redraw();
	}

	void ScintillaWinUI::OnSelectionUpdating(winrt::Windows::UI::Text::Core::CoreTextEditContext const &sender, winrt::Windows::UI::Text::Core::CoreTextSelectionUpdatingEventArgs const &args)
	{
		auto selection{ args.Selection() };

		// Todo: Not sure how to set the style

		__super::SetSelection(AcpToDocPosition(selection.StartCaretPosition), AcpToDocPosition(selection.EndCaretPosition));

		/*switch (pSelection->style.ase)
		{
		case TsActiveSelEnd::TS_AE_START:
			__super::SetSelection(AcpToDocPosition(pSelection->acpStart), AcpToDocPosition(pSelection->acpEnd));
			break;
		case TsActiveSelEnd::TS_AE_NONE:
			DropCaret();
			[[fallthrough]];
		case TsActiveSelEnd::TS_AE_END:
			__super::SetSelection(AcpToDocPosition(pSelection->acpEnd), AcpToDocPosition(pSelection->acpStart));
			break;
		}
		DebugOut(L"SetSelection, ACP: %d, Sci: %d; ACP: %d, Sci: %d; Mode: %d\n", pSelection->acpStart, AcpToDocPosition(pSelection->acpStart), pSelection->acpEnd, AcpToDocPosition(pSelection->acpEnd), pSelection->style.ase);
		return S_OK;*/
	}

	void ScintillaWinUI::OnFormatUpdating(winrt::Windows::UI::Text::Core::CoreTextEditContext const &sender, winrt::Windows::UI::Text::Core::CoreTextFormatUpdatingEventArgs const &args)
	{

	}

	void ScintillaWinUI::OnLayoutRequested(winrt::Windows::UI::Text::Core::CoreTextEditContext const &sender, winrt::Windows::UI::Text::Core::CoreTextLayoutRequestedEventArgs const &args)
	{

	}

	void ScintillaWinUI::OnCompositionStarted(winrt::Windows::UI::Text::Core::CoreTextEditContext const &sender, winrt::Windows::UI::Text::Core::CoreTextCompositionStartedEventArgs const &args)
	{

	}

	void ScintillaWinUI::OnCompositionCompleted(winrt::Windows::UI::Text::Core::CoreTextEditContext const &sender, winrt::Windows::UI::Text::Core::CoreTextCompositionCompletedEventArgs const &args)
	{

	}

	void ScintillaWinUI::OnCaretTimerTick(winrt::Windows::Foundation::IInspectable const &sender, winrt::Windows::Foundation::IInspectable const &args)
	{
		TickFor(TickReason::caret);
	}

	void ScintillaWinUI::OnScrollTimerTick(winrt::Windows::Foundation::IInspectable const &sender, winrt::Windows::Foundation::IInspectable const &args)
	{
		TickFor(TickReason::scroll);
	}

	void ScintillaWinUI::OnWidenTimerTick(winrt::Windows::Foundation::IInspectable const &sender, winrt::Windows::Foundation::IInspectable const &args)
	{
		TickFor(TickReason::widen);
	}

	void ScintillaWinUI::OnDwellTimerTick(winrt::Windows::Foundation::IInspectable const &sender, winrt::Windows::Foundation::IInspectable const &args)
	{
		TickFor(TickReason::dwell);
	}

	// TSF COM

	// See http://web.archive.org/web/20091003084412/http://msdn.microsoft.com/en-us/magazine/cc187238.aspx
	// https://github.com/chromium/chromium/blob/main/ui/base/ime/win/tsf_text_store.cc

	IFACEMETHODIMP ScintillaWinUI::AdviseSink(REFIID riid, IUnknown *punk, DWORD dwMask)
	{
		HRESULT	hr;
		winrt::com_ptr<IUnknown> punkID;

		// Determine if the sink interface exists.
		// Get the pointer to the IUnknown interface and check if the IUnknown
		// pointer is the same as a pointer to an existing sink.
		// If the sink exists, update the existing sink with the 
		// dwMask parameters passed to this method.	
		hr = punk->QueryInterface(IID_IUnknown, (LPVOID *)&punkID);

		if (FAILED(hr))
		{
			return E_INVALIDARG;
		}

		if (!!_sinkUnk && punkID == _sinkUnk)
		{
			_textStoreSinkMask = dwMask;
			return S_OK;
		}
		if (!!_tfTextStoreACPSink) // if we already have a sink, we can't add another one.
		{
			return CONNECT_E_ADVISELIMIT;
		}

		// If the sink does not exist, do the following:
		// 1. Install a new sink.
		// 2. Keep the pointer to the IUnknown interface to uniquely
		//	  identify this advise sink.
		// 3. Set the dwMask parameter of this new sink to the dwMask 
		//    parameters passed to this method.

		if (IsEqualIID(riid, IID_ITextStoreACPSink))
		{
			punk->QueryInterface(IID_ITextStoreACPSink, _tfTextStoreACPSink.put_void());
			_sinkUnk = punkID;
			_textStoreSinkMask = dwMask;

			hr = S_OK;
		}
		else
		{
			hr = E_INVALIDARG; // we only support IID_ITextStoreACPSink
		}
		return hr;
	}

	IFACEMETHODIMP ScintillaWinUI::UnadviseSink(IUnknown *)
	{
		return E_NOTIMPL;
	}

	IFACEMETHODIMP ScintillaWinUI::RequestLock(DWORD dwLockFlags, HRESULT *phrSession)
	{
		if (phrSession == NULL)
		{
			return E_INVALIDARG;
		}
		if (_tfTextStoreACPSink == nullptr)
		{
			return E_UNEXPECTED;
		}
		/*if (!MainHWND())
		{
			return E_FAIL; // WinUI Todo
		}*/

		HRESULT hr = S_OK;
		*phrSession = E_FAIL;

		if (_lock != NONE)
		{
			// The document is locked, therefore since lock re-entrancy is not allowed
			// we reject synchronous lock requests and postpone asynchronous ones.
			if (dwLockFlags & TS_LF_SYNC)
			{
				// TS_E_SYNCHRONOUS means "the document can not be locked synchronously"
				*phrSession = TS_E_SYNCHRONOUS;
			}
			else if (_lockAsync == NONE &&
				(_lock == READONLY) &&
				(dwLockFlags & TS_LF_READWRITE) == TS_LF_READWRITE)
			{
				// While in a synch read-only lock, we allow one asynch read-write request.
				// It'll be postponed until the read-only session is over.
				*phrSession = TS_S_ASYNC;
				_lockAsync = READWRITE;
			}
			else
			{
				*phrSession = E_FAIL;
				hr = E_FAIL;
			}
		}
		else
		{
			if (dwLockFlags & (TS_LF_READWRITE & ~TS_LF_READ))
			{
				ImeEndComposition();
			}

			// Lock it and give it to the manager.
			_lock = static_cast<LockTypes>(dwLockFlags & TS_LF_READWRITE);

			*phrSession = _tfTextStoreACPSink->OnLockGranted(_lock);

			while (_lockAsync && _tfTextStoreACPSink != NULL)
			{
				// Grant the pending async read/write lock if there's one.
				_lock = _lockAsync;
				_lockAsync = NONE;
				_tfTextStoreACPSink->OnLockGranted(_lock);
			}
			_lock = NONE;
		}

		while (!msgq.empty())
		{
			OutputDebugStringW(L"Processing Queued Message\n");
			ProcessMessage(msgq.front());
			msgq.pop();
		}
		while (!notifyq.empty())
		{
			OutputDebugStringW(L"Processing Queued Notification\n");
			ProcessMessage(notifyq.front());
			notifyq.pop();
		}

		return hr;
	}

	IFACEMETHODIMP ScintillaWinUI::GetStatus(TS_STATUS *pdcs)
	{
		if (!pdcs)
		{
			return E_INVALIDARG;
		}
		pdcs->dwStaticFlags = 0; // Scintilla doesn't support regions, disjoint selections, etc.
		pdcs->dwDynamicFlags = pdoc->IsReadOnly() ? TS_SD_READONLY : 0; // Todo: TS_SS_TKBAUTOCORRECTENABLE, TS_SS_TKBPREDICTIONENABLE?
		return S_OK;
	}

	IFACEMETHODIMP ScintillaWinUI::QueryInsert(LONG acpTestStart, LONG acpTestEnd, ULONG cch, LONG *pacpResultStart, LONG *pacpResultEnd)
	{
		if (pacpResultStart == 0 || pacpResultEnd == 0)
		{
			return E_INVALIDARG;
		}
		if (_lock == NONE)
			return TS_E_NOLOCK;

		if ((acpTestStart < 0) || (acpTestStart > acpTestEnd) || (acpTestEnd > pdoc->Length()))
		{
			return E_INVALIDARG;
		}

		/*if (MainHWND()) // WinUI Todo
		{*/
		// Set the start point to the given start point.
		*pacpResultStart = pdoc->MovePositionOutsideChar(acpTestStart, -1, true);
		// Set the end point to the given end point.
		*pacpResultEnd = pdoc->MovePositionOutsideChar(acpTestEnd, 1, true);
		/*}
		else
		{
			// No window - don't insert.
			*pacpResultStart = -1;
			*pacpResultEnd = -1;
		}*/

		return S_OK;
	}

	IFACEMETHODIMP ScintillaWinUI::GetSelection(ULONG ulIndex, ULONG ulCount, TS_SELECTION_ACP *pSelection, ULONG *pcFetched)
	{
		if (pSelection == 0 || pcFetched == 0)
		{
			return E_INVALIDARG;
		}
		if (_lock == NONE)
		{
			return TS_E_NOLOCK;
		}
		// we only have one selection, so if you ask for more than one, fail.
		if (ulIndex != 0 && ulIndex != TF_DEFAULT_SELECTION)
		{
			return E_INVALIDARG;
		}
		*pcFetched = 1;
		if (ulCount == 0)
		{
			return S_OK;
		}
		auto &range{ sel.RangeMain() };
		if (range.caret < range.anchor)
		{
			pSelection->acpStart = DocPositionToAcp(range.caret.Position());
			pSelection->acpEnd = DocPositionToAcp(range.anchor.Position());
			pSelection->style.ase = caret.active ? TS_AE_START : TS_AE_NONE;
		}
		else
		{
			pSelection->acpStart = DocPositionToAcp(range.anchor.Position());
			pSelection->acpEnd = DocPositionToAcp(range.caret.Position());
			pSelection->style.ase = caret.active ? TS_AE_END : TS_AE_NONE;
		}
		pSelection->style.fInterimChar = 0;
		DebugOut(L"GetSelection, ACP: %d, Sci: %d; ACP: %d, Sci: %d, Mode: %d\n", DocPositionToAcp(SelectionStart().Position()), SelectionStart().Position(), DocPositionToAcp(SelectionEnd().Position()), SelectionEnd().Position(), pSelection->style.ase);
		return S_OK;
	}

	IFACEMETHODIMP ScintillaWinUI::SetSelection(ULONG ulCount, const TS_SELECTION_ACP *pSelection)
	{
		// UTF-16 idea
		if (pSelection == 0)
		{
			return E_INVALIDARG;
		}
		if (_lock != READWRITE)
		{
			return TS_E_NOLOCK;
		}
		// we only have one selection, so if you ask for more than one, fail.
		if (ulCount != 1)
		{
			return E_INVALIDARG;
		}

		// Comments somewhat out of date
		// Not sure about combos (e.g. multi-tone emojis)
		// DebugOut below is wrong
		// I think this only seems to not work because the other methods are not changed yet
		// Todo: Make work for selection end

		// The version with NextPosition does not work with 4 byte UTF-16 chars!
		// The CountUTF16 one does, but is insanely slow (think 10 minutes to insert in a file of this size)

		switch (pSelection->style.ase)
		{
		case TsActiveSelEnd::TS_AE_START:
			__super::SetSelection(AcpToDocPosition(pSelection->acpStart), AcpToDocPosition(pSelection->acpEnd));
			break;
		case TsActiveSelEnd::TS_AE_NONE:
			//DropCaret();
			//[[fallthrough]];
		case TsActiveSelEnd::TS_AE_END:
			__super::SetSelection(AcpToDocPosition(pSelection->acpEnd), AcpToDocPosition(pSelection->acpStart));
			break;
		}
		DebugOut(L"SetSelection, ACP: %d, Sci: %d; ACP: %d, Sci: %d; Mode: %d\n", pSelection->acpStart, AcpToDocPosition(pSelection->acpStart), pSelection->acpEnd, AcpToDocPosition(pSelection->acpEnd), pSelection->style.ase);
		return S_OK;
	}

	IFACEMETHODIMP ScintillaWinUI::GetText(LONG acpStart, LONG acpEnd, WCHAR *pchPlain, ULONG cchPlainReq, ULONG *pcchPlainRet, TS_RUNINFO *prgRunInfo, ULONG cRunInfoReq, ULONG *pcRunInfoRet, LONG *pacpNext)
	{
		// Todo: Probably don't return whole document if acpEnd is -1. Callers will ask for more if they need it.
		// Todo: Remove UTF-8 hack

		DebugOut(L"GetText: %d_%d\n", acpStart, acpEnd);

		// Todo: Suspecting this method might be some kind of bottleneck. Look at ScintillaWin::GetText for inspiration

		if (_lock == NONE)
			return TS_E_NOLOCK;
		// acpEnd can be -1, initialize it properly.
		if (acpEnd == -1)
			acpEnd = pdoc->Length();

		if ((acpStart < 0) || (acpStart > acpEnd) || (acpEnd > pdoc->Length()))
			return TF_E_INVALIDPOS;

		// validate plain text buffers.
		bool fDoPlainText = cchPlainReq > 0;
		if (fDoPlainText && (pchPlain == NULL || pcchPlainRet == NULL))
			return E_INVALIDARG;

		// validate text run buffers.
		bool fDoTextRun = cRunInfoReq > 0;
		if ((prgRunInfo == NULL || pcRunInfoRet == NULL))
			return E_INVALIDARG;

		// Validate window
		/*if (!MainHWND())
			return E_FAIL;*/ // WinUI Todo

			// limit request to size of internal buffers
#define MAX_RUN 128
		if (cchPlainReq > MAX_RUN)
			cchPlainReq = MAX_RUN;

		*pacpNext = acpStart;
		*pcchPlainRet = 0;
		*pcRunInfoRet = 0;

		if (fDoPlainText || fDoTextRun)
		{
			//int mask = pdoc->stylingBitsMask; // Todo: check
			prgRunInfo->type = TS_RT_PLAIN;
			prgRunInfo->uCount = 0;
			ULONG cchFetched(0);
			ULONG cRunsFetched(1);
			// this gets a bit weird.  Text data in the document is stored as DBCS/UTF8 (8-bit chars); we need to convert the text into
			// unicode, without changing the character positions.  I cheat here by converting the lead byte as plain text and changing the
			// trail bytes to hidden spaces.
			int startPos = pdoc->MovePositionOutsideChar(acpStart, -1, true);
			int endPos = pdoc->MovePositionOutsideChar(acpEnd, 1, true);
			bool fProtectionActive = vs.ProtectionActive();

			// Scintilla supports folds (to hide un-necessary text).  If we're requesting text that's either invisible
			// or unchangable, mark it hidden.
			// TODO:  Consider marking unchangable text as visible & failing any SetText operations that cover unchangeable text.
			if (fProtectionActive && vs.styles[pdoc->StyleAt(startPos)/* & mask*/].IsProtected()) // Todo: I don't think mask is needed since 2014
				prgRunInfo->type = TS_RT_HIDDEN;

			if (startPos < endPos)
			{
				char text[MAX_RUN];		// Enough to handle an entire run of text
				// go until we run out of space in the output buffers or we run out of text
				while (cchPlainReq > 0 && cRunInfoReq > 0 && startPos < endPos)
				{
					DWORD charSize = pdoc->LenChar(startPos);

					if (charSize > cchPlainReq)     // don't put a char in if it won't fit.
					{
						break;
					}

					// Bypass conversion for single-byte characters (the common case)
					if (charSize == 1)
					{
						*pchPlain++ = pdoc->CharAt(startPos);
						--cchPlainReq;
					}
					else
					{
						for (DWORD i = 0; i < charSize; ++i)
						{
							text[i] = pdoc->CharAt(startPos + i);
						}
						text[charSize] = '\0';
						::MultiByteToWideChar(IsUnicodeMode() ? CP_UTF8 : pdoc->dbcsCodePage, 0, text, charSize, pchPlain, cchPlainReq);
						pchPlain++;
						--cchPlainReq;
						for (DWORD i = 0; i < charSize - 1 && cchPlainReq > 0; ++i, --cchPlainReq)	// copy out trail bytes as spaces
						{
							*pchPlain++ = L' ';
						}
					}
					cchFetched += charSize;

					TsRunType runtype = TS_RT_PLAIN;
					if (fProtectionActive && vs.styles[pdoc->StyleAt(startPos)/* & mask*/].IsProtected()) // Todo: I don't think mask is needed since 2014
						runtype = TS_RT_HIDDEN;
					// update runs
					if (prgRunInfo->type == runtype)
					{
						prgRunInfo->uCount++;
					}
					else		// prev char had trail bytes.  Add an entry for this char
					{
						prgRunInfo++;
						cRunsFetched++;
						cRunInfoReq--;
						prgRunInfo->type = runtype;
						prgRunInfo->uCount = 1;
					}
					if (charSize > 1)		// tag trail bytes as hidden
					{
						if (runtype == TS_RT_PLAIN) // add a new run
						{
							prgRunInfo++;
							cRunsFetched++;
							cRunInfoReq--;
							prgRunInfo->type = TS_RT_HIDDEN;
							prgRunInfo->uCount = charSize - 1;
						}
						else
						{
							prgRunInfo->uCount += charSize - 1;
						}
					}
					startPos += charSize;
				}
				*pacpNext = startPos;
				*pcchPlainRet = cchFetched;
				*pcRunInfoRet = cRunsFetched;
			}
		}
		return S_OK;
	}

	IFACEMETHODIMP ScintillaWinUI::SetText(DWORD dwFlags, LONG acpStart, LONG acpEnd, const WCHAR *pchText, ULONG cch, TS_TEXTCHANGE *pChange)
	{
		// Todo: Problem with inserting characters like Right Single Quotation Mark
		// Todo: Problems with Cyrillic Small Letter Short I (cursor in wrong spot + same problem as above)
		// Todo: Should not make an undo for individual letters

		if (pchText == 0)
		{
			return E_POINTER;
		}
		if (_lock != READWRITE)
		{
			return TS_E_NOLOCK;
		}
		if (pdoc->IsReadOnly())
		{
			return TS_E_READONLY;
		}

		auto docStart{ AcpToDocPosition(acpStart) };
		auto docEnd{ AcpToDocPosition(acpEnd) };

		// set text by deleting existing chars and inserting new text
		auto startPos{ pdoc->MovePositionOutsideChar(docStart, -1, true) };
		auto  endPos{ pdoc->MovePositionOutsideChar(docEnd, 1, true) };
		auto  len{ endPos - startPos };
		auto utf16len{ pdoc->CountUTF16(startPos, endPos) };
		pdoc->BeginUndoAction();
		pdoc->DeleteChars(startPos, len);
		int cchText{ -1 };
		char *szText{ nullptr };
		if (IsUnicodeMode())
		{
			cchText = UTF8Length({ pchText, cch });
			szText = new char[cchText + 1];
			UTF8FromUTF16({ pchText, cch }, szText, cchText);
		}
		else
		{
			UINT cpDest = CodePageFromCharSet(vs.styles[STYLE_DEFAULT].characterSet, pdoc->dbcsCodePage);
			cchText = ::WideCharToMultiByte(cpDest, 0, pchText, cch,
				NULL, 0, NULL, NULL);
			szText = new char[cchText + 1];
			if (szText) {
				::WideCharToMultiByte(cpDest, 0, pchText, cch,
					szText, cchText, NULL, NULL);
				szText[cchText] = '\0';
			}
		}
		pdoc->InsertString(startPos, szText, cchText);

		EnsureCaretVisible();
		ShowCaretAtCurrentPosition(); // Todo: Reevaluate how text at the current position is inserted (this is copied out of InsertCharacter)
		if (cchText == 1)
		{
			NotifyChar(szText[0], Scintilla::CharacterSource::DirectInput); // Todo: This is temporary
		}

		delete[] szText;
		if (pChange)
		{
			auto newAcpStart{ DocPositionToAcp(startPos) };
			pChange->acpStart = newAcpStart;
			pChange->acpOldEnd = newAcpStart + utf16len;
			pChange->acpNewEnd = newAcpStart + cch;
			DebugOut(L"SetText, Start: %d, Old End: %d, New End: %d\n", pChange->acpStart, pChange->acpOldEnd, pChange->acpNewEnd);
		}
		pdoc->EndUndoAction();
		NotifyChange();
		Redraw();
		return S_OK;
	}

	IFACEMETHODIMP ScintillaWinUI::GetFormattedText(LONG, LONG, IDataObject **)
	{
		return E_NOTIMPL; // Not implemented
	}

	IFACEMETHODIMP ScintillaWinUI::GetEmbedded(LONG, const GUID &, const IID &, IUnknown **)
	{
		return E_NOTIMPL; // Not implemented
	}

	IFACEMETHODIMP ScintillaWinUI::QueryInsertEmbedded(const GUID *pguidService, const FORMATETC *pFormatEtc, BOOL *pfInsertable)
	{
		if (!pfInsertable)
		{
			return E_INVALIDARG;
		}

		// We don't support embedded objects
		*pfInsertable = FALSE;
		return S_OK;
	}

	IFACEMETHODIMP ScintillaWinUI::InsertEmbedded(DWORD dwFlags, LONG acpStart, LONG acpEnd, IDataObject *pDataObject, TS_TEXTCHANGE *pChange)
	{
		if (!pDataObject)
		{
			return E_INVALIDARG;
		}
		// We don't support embedded objects
		return TS_E_FORMAT;
	}

	IFACEMETHODIMP ScintillaWinUI::InsertTextAtSelection(DWORD dwFlags, const WCHAR *pchText, ULONG cch, LONG *pacpStart, LONG *pacpEnd, TS_TEXTCHANGE *pChange)
	{
		if (pchText == 0 || pChange == 0)
			return E_POINTER;
		if ((dwFlags & TS_IAS_QUERYONLY) || !(dwFlags & TS_IAS_NOQUERY))
		{
			if (!pacpStart || !pacpEnd)
				return E_POINTER;
		}
		if (_lock != READWRITE)
			return TS_E_NOLOCK;
		if (pdoc->IsReadOnly())
			return TS_E_READONLY;

		TS_SELECTION_ACP acpSelection;
		ULONG cFetched(0);
		HRESULT hr = S_OK;

		hr = GetSelection(TS_DEFAULT_SELECTION, 1, &acpSelection, &cFetched);
		if (hr != S_OK)
			return hr;

		LONG acpResultStart;
		LONG acpResultEnd;
		hr = QueryInsert(acpSelection.acpStart, acpSelection.acpEnd, cch,
			&acpResultStart, &acpResultEnd);

		if (hr != S_OK)
			return hr;

		if (dwFlags & TS_IAS_QUERYONLY)
		{
			// Query only, return data
			*pacpStart = acpResultStart;
			*pacpEnd = acpResultEnd;
			return S_OK;
		}
		hr = SetText(0, acpResultStart, acpResultEnd, pchText, cch, pChange);

		if (hr != S_OK)
			return hr;

		if (!(dwFlags & TS_IAS_NOQUERY) && pChange)
		{
			*pacpStart = pChange->acpStart;
			*pacpEnd = pChange->acpNewEnd;
			SetEmptySelection(pChange->acpNewEnd);
		}
		return hr;
	}

	IFACEMETHODIMP ScintillaWinUI::InsertEmbeddedAtSelection(DWORD, IDataObject *, LONG *, LONG *, TS_TEXTCHANGE *)
	{
		return E_NOTIMPL; // Not implemented
	}

	IFACEMETHODIMP ScintillaWinUI::RequestSupportedAttrs(DWORD, ULONG, const TS_ATTRID *)
	{
		return E_NOTIMPL; // Not implemented
	}

	IFACEMETHODIMP ScintillaWinUI::RequestAttrsAtPosition(LONG, ULONG, const TS_ATTRID *, DWORD)
	{
		return E_NOTIMPL; // Not implemented
	}

	IFACEMETHODIMP ScintillaWinUI::RequestAttrsTransitioningAtPosition(LONG, ULONG, const TS_ATTRID *, DWORD)
	{
		return E_NOTIMPL; // Not implemented
	}

	IFACEMETHODIMP ScintillaWinUI::FindNextAttrTransition(LONG, LONG, ULONG, const TS_ATTRID *, DWORD, LONG *, BOOL *, LONG *)
	{
		return E_NOTIMPL; // Not implemented
	}

	IFACEMETHODIMP ScintillaWinUI::RetrieveRequestedAttrs(ULONG, TS_ATTRVAL *, ULONG *)
	{
		return E_NOTIMPL; // Not implemented
	}

	IFACEMETHODIMP ScintillaWinUI::GetEndACP(LONG *pacp)
	{
		if (!pacp)
		{
			return E_POINTER;
		}
		if (_lock == NONE)
		{
			return TS_E_NOLOCK;
		}
		*pacp = pdoc->Length();
		return S_OK;
	}

	IFACEMETHODIMP ScintillaWinUI::GetActiveView(TsViewCookie *pvcView)
	{
		if (_lock == NONE)
		{
			return TS_E_NOLOCK;
		}
		if (!pvcView)
		{
			return E_INVALIDARG;
		}
		else
		{
			*pvcView = 0; // only one view
		}
		return S_OK;
	}

	IFACEMETHODIMP ScintillaWinUI::GetACPFromPoint(TsViewCookie vcView, const POINT *ptScreen, DWORD dwFlags, LONG *pacp)
	{
		return E_NOTIMPL; // Todo: Not sure if we really need this, or even can do this from UWP. Chromium (metro and current) do not implement
		/*if (vcView != 0 || ptScreen == 0 || pacp == 0)
		{
			return E_INVALIDARG;
		}
		if (dwLock == NONE)
			return TS_E_NOLOCK;
		// Map screen point into window-relative point
		POINT ptWnd = *ptScreen;
		if (!::MapWindowPoints(NULL, MainHWND(), &ptWnd, 1))
		{
			return HRESULT_FROM_WIN32(::GetLastError());
		}
		Point pt(ptWnd.x, ptWnd.y);
		int posExclude = PositionFromLocationClose(pt);
		HRESULT hr = S_OK;
		if (posExclude == INVALID_POSITION)
		{
			hr = TF_E_INVALIDPOINT;
			if (dwFlags == GXFPF_NEAREST)
			{
				*pacp = pdoc->MovePositionOutsideChar(PositionFromLocation(pt), 0, false);
				return S_OK;
			}
		}
		if (SUCCEEDED(hr) && dwFlags == GXFPF_ROUND_NEAREST)
		{
			// TODO:  may need to bump the character pos up one if it's actually closer to the next char...
		}
		*pacp = posExclude;
		return hr;*/
	}

	IFACEMETHODIMP ScintillaWinUI::GetTextExt(TsViewCookie vcView, LONG acpStart, LONG acpEnd, RECT *prc, BOOL *pfClipped)
	{
		if (vcView != 0 || prc == 0 || pfClipped == 0)
		{
			return E_INVALIDARG;
		}
		if (_lock == NONE)
			return TS_E_NOLOCK;
		// TODO:  What about offscreen positions?  I really don't think this works here.
		PRectangle pr(RectangleFromRange(Range{ pdoc->MovePositionOutsideChar(acpStart, -1, false), pdoc->MovePositionOutsideChar(acpEnd, 1, false) }, 0));
		*prc = *(reinterpret_cast<RECT *>(&pr));
		// and change it into screen coordinates.
		/*if (!::MapWindowPoints(MainHWND(), NULL, (LPPOINT)prc, 2))
		{
			return HRESULT_FROM_WIN32(::GetLastError());
		}*/ // WinUI Todo
		*pfClipped = 0;
		return S_OK;
	}

	IFACEMETHODIMP ScintillaWinUI::GetScreenExt(TsViewCookie vcView, RECT *prc)
	{
		if (vcView != 0 || prc == 0)
		{
			return E_INVALIDARG;
		}
		if (_lock == NONE)
		{
			return TS_E_NOLOCK;
		}
		// Get the client rectangle
		//::SetRectEmpty(prc); // WinUI Todo
		PRectangle prClient(GetClientRectangle());
		PRectangle prText(GetTextRectangle());
		//::IntersectRect(prc, reinterpret_cast<RECT *>(&prClient), reinterpret_cast<RECT *>(&prText)); // WinUI Todo
		// and change it into screen coordinates.
		/*if (!::MapWindowPoints(MainHWND(), NULL, (LPPOINT)prc, 2))
		{
			return HRESULT_FROM_WIN32(::GetLastError());
		}*/ // WinUI Todo
		return S_OK;
	}

	IFACEMETHODIMP ScintillaWinUI::OnEndComposition(ITfCompositionView *pComposition)
	{
		DebugOut(L"OnEndComposition");
		ImeEndComposition(); // Todo: Maybe
		return S_OK;
	}

	IFACEMETHODIMP ScintillaWinUI::OnStartComposition(ITfCompositionView *pComposition, BOOL *pfOk)
	{
		DebugOut(L"OnStartComposition");
		view.imeCaretBlockOverride = true;
		return S_OK;
	}

	IFACEMETHODIMP ScintillaWinUI::OnUpdateComposition(ITfCompositionView *pComposition, ITfRange *pRangeNew)
	{
		DebugOut(L"OnUpdateComposition");
		return S_OK;
	}

	Sci::Position ScintillaWinUI::AcpToDocPosition(Sci::Position acp)
	{
		/*Sci::Position i{0};
		Sci::Position p{ 0 };
		Sci::Position utf16{ 0 };

		while (utf16 != acp)
		{
			utf16 = pdoc->CountUTF16(0, i++);
		}
		if (i != 0)
		{
			p = i - 1;
		}
		return p;*/

		LONG c{ 0 };
		Sci::Position p{ 0 };

		while (c != acp)
		{
			Sci::Position lp{ p };
			p = pdoc->NextPosition(p, 1);
			if (p - lp > 3)
			{
				c++;
			}
			c++; // Todo: Make sure there is an escape so this does not act as an infinite loop
		}

		return p;
	}

	Sci::Position ScintillaWinUI::DocPositionToAcp(Sci::Position docPosition)
	{
		// A little worried that CountUTF16 calling MovePositionOutsideChar might make position translation inconsistent
		// Also, we have calls that pass -1 and start and 1 and end to MovePositionOutsideChar, but this uses 1 and start and -1 and end
		// Todo: So double check elsewhere if it makes the most sense to move in that direction
		// Why not use pdoc->GetRelativePositionUTF16? Because it fails with multi-byte UTF-16 characters
		return pdoc->CountUTF16(0, docPosition);
	}

	winrt::DUX::DispatcherTimer ScintillaWinUI::GetTimerForReason(TickReason reason)
	{
		switch (reason)
		{
		case TickReason::caret:
			return _caretTimer;
		case TickReason::scroll:
			return _scrollTimer;
		case TickReason::widen:
			return _widenTimer;
		case TickReason::dwell:
			return _dwellTimer;
		default:
			return nullptr; // Todo: throw exception
		}
	}

	bool ScintillaWinUI::FineTickerRunning(TickReason reason)
	{
		return GetTimerForReason(reason).IsEnabled();
	}

	void ScintillaWinUI::FineTickerStart(TickReason reason, int millis, int tolerance)
	{
		auto timer{ GetTimerForReason(reason) };
		timer.Interval(winrt::Windows::Foundation::TimeSpan{ std::chrono::milliseconds{ millis } });
		timer.Start();
	}

	void ScintillaWinUI::FineTickerCancel(TickReason reason)
	{
		GetTimerForReason(reason).Stop();
	}

	void ScintillaWinUI::ScrollText(Sci::Line /* linesToMove */) {
		//Platform::DebugPrintf("ScintillaWin::ScrollText %d\n", linesToMove);
		//::ScrollWindow(MainHWND(), 0,
		//	vs.lineHeight * linesToMove, 0, 0);
		//::UpdateWindow(MainHWND());
		Redraw();
		UpdateSystemCaret();
	}

	void ScintillaWinUI::SetVerticalScrollPos() {
		ChangeScrollPos(SB_VERT, topLine);
	}

	void ScintillaWinUI::SetHorizontalScrollPos() {
		ChangeScrollPos(SB_HORZ, xOffset);
	}

	void ScintillaWinUI::HorizontalScrollToClamped(int xPos) {
		const HorizontalScrollRange range = GetHorizontalScrollRange();
		HorizontalScrollTo(std::clamp(xPos, 0, range.documentWidth - range.pageWidth + 1));
	}

	HorizontalScrollRange ScintillaWinUI::GetHorizontalScrollRange() const {
		const PRectangle rcText = GetTextRectangle();
		int pageWidth = static_cast<int>(rcText.Width());
		const int horizEndPreferred = std::max({ scrollWidth, pageWidth - 1, 0 });
		if (!horizontalScrollBarVisible || Wrapping())
			pageWidth = horizEndPreferred + 1;
		return { pageWidth, horizEndPreferred };
	}

	bool ScintillaWinUI::ModifyScrollBars(Sci::Line nMax, Sci::Line nPage) {
		/*if (!IsVisible()) {
			return false;
		}*/

		bool modified = false;
		const Sci::Line vertEndPreferred = nMax;
		if (!verticalScrollBarVisible)
			nPage = vertEndPreferred + 1;
		if (ChangeScrollRange(SB_VERT, 0, static_cast<int>(vertEndPreferred), static_cast<unsigned int>(nPage))) {
			modified = true;
		}
		const HorizontalScrollRange range = GetHorizontalScrollRange();
		if (ChangeScrollRange(SB_HORZ, 0, range.documentWidth, range.pageWidth)) {
			modified = true;
			if (scrollWidth < range.pageWidth) {
				HorizontalScrollTo(0);
			}
		}

		return modified;
	}

	bool ScintillaWinUI::ChangeScrollRange(int nBar, int nMin, int nMax, UINT nPage) noexcept {
		if (!_wrapper || !_wrapper->HasScrollBars())
		{
			return false;
		}

		const auto max{ std::max(0, nMax - static_cast<int>(nPage) + 1) };

		// ViewportSize must be set first or this causes an issue where the bars are the wrong length after a window resize

		if (nBar == SB_HORZ)
		{
			if (static_cast<int>(_wrapper->HorizontalScrollBarMinimum()) != nMin || static_cast<int>(_wrapper->HorizontalScrollBarMaximum()) != max || static_cast<UINT>(_wrapper->HorizontalScrollBarViewportSize()) != nPage)
			{
				_wrapper->HorizontalScrollBarViewportSize(nPage);
				_wrapper->HorizontalScrollBarMinimum(nMin);
				_wrapper->HorizontalScrollBarMaximum(max);
				return true;
			}
		}
		else
		{
			if (static_cast<int>(_wrapper->VerticalScrollBarMinimum()) != nMin || static_cast<int>(_wrapper->VerticalScrollBarMaximum()) != max || static_cast<UINT>(_wrapper->VerticalScrollBarViewportSize()) != nPage)
			{
				_wrapper->VerticalScrollBarViewportSize(nPage);
				_wrapper->VerticalScrollBarMinimum(nMin);
				_wrapper->VerticalScrollBarMaximum(max);
				return true;
			}
		}
		return false;
	}

	// Change the scroll position but avoid repaint if changing to same value
	void ScintillaWinUI::ChangeScrollPos(int barType, Sci::Position pos) {
		/*if (!IsVisible()) {
			return;
		}*/

		const auto value{ static_cast<int>(barType == SB_HORZ ? _wrapper->HorizontalScrollBarValue() : _wrapper->VerticalScrollBarValue()) };
		if (value != pos) {
			DwellEnd(true);
			if (barType == SB_HORZ)
			{
				_wrapper->HorizontalScrollBarValue(pos);
			}
			else
			{
				_wrapper->VerticalScrollBarValue(pos);
			}
		}
	}

	void ScintillaWinUI::Copy()
	{
		if (!sel.Empty())
		{
			SelectionText selectedText;
			CopySelectionRange(&selectedText);
			CopyToClipboard(selectedText);
		}
	}

	void ScintillaWinUI::Paste()
	{
		PasteAsync();
	}

	winrt::fire_and_forget ScintillaWinUI::PasteAsync()
	{
		// Todo: Editor needs to restore selection if undo is pressed (not limited to paste)

		// WinUI Todo: needs to recognize other paste shapes, formats, etc.

		winrt::hstring paste{};
		try
		{
			auto dpv{ winrt::Windows::ApplicationModel::DataTransfer::Clipboard::GetContent() };
			if (dpv.Contains(winrt::Windows::ApplicationModel::DataTransfer::StandardDataFormats::Text()))
			{
				paste = co_await dpv.GetTextAsync();
			}
		}
		catch (winrt::hresult_error const &)
		{
			co_return;
		}

		UndoGroup ug(pdoc);
		ClearSelection(multiPasteMode == MultiPaste::Each);

		auto str{ EncodeWString(paste) };
		InsertPasteShape(str.data(), str.length(), PasteShape::stream);

		Redraw();
	}

	void ScintillaWinUI::ClaimSelection()
	{
		// Windows does not have a primary selection
		if (_tsfCore)
		{
			_editContext.NotifySelectionChanged(winrt::Windows::UI::Text::Core::CoreTextRange{ static_cast<int32_t>(DocPositionToAcp(SelectionStart().Position())), static_cast<int32_t>(DocPositionToAcp(SelectionEnd().Position())) });
		}
		else
		{
			//TSF:  The selection has changed - notify the sink, if there's no lock held.
			if (_tfTextStoreACPSink && _lock == NONE)
			{
				DebugOut(L"OnSelectionChange\n");
				_tfTextStoreACPSink->OnSelectionChange();
			}
		}
	}

	void ScintillaWinUI::NotifyChange()
	{
		SendMessage(WM_COMMAND,
			MAKEWPARAM(GetCtrlID(), FocusChange::Change),
			reinterpret_cast<LPARAM>(nullptr));
	}

	void ScintillaWinUI::NotifyFocus(bool focus)
	{
		// Todo: notifyq
		if (commandEvents)
		{
			SendMessage(WM_COMMAND,
				MAKEWPARAM(GetCtrlID(), focus ? FocusChange::Setfocus : FocusChange::Killfocus),
				reinterpret_cast<LPARAM>(nullptr));
		}
		if (_tsfCore)
		{
			if (focus) // Todo: should this be inside if (commandEvents)?
			{
				_editContext.NotifyFocusEnter();
			}
			else
			{
				_editContext.NotifyFocusLeave();
			}
		}
		else
		{
			winrt::check_hresult(_tfThreadManager->SetFocus(focus ? _tfDocumentManager.get() : nullptr));
		}
		Editor::NotifyFocus(focus);
	}

	void ScintillaWinUI::NotifyParent(Scintilla::NotificationData scn)
	{
		if (_lock != NONE)
		{
			notifyq.push(std::make_unique<NotifyMessage>(static_cast<uptr_t>(GetCtrlID()), scn, false));
		}
		else
		{
			ProcessNotifyMessage(static_cast<uptr_t>(GetCtrlID()), scn, _shouldNotifyTsf);
		}
		// Todo: Probably do not need GetCtrlID
	}

	void ScintillaWinUI::CopyToClipboard(const SelectionText &selectedText)
	{
		// WinUI Todo: This could be a lot more complicated
		// Also, you could copy the win32 version if the winrt version can't do some of those things
		winrt::Windows::ApplicationModel::DataTransfer::DataPackage dp{};
		// Todo: RequestedOperation = DataPackageOperation::Move (cut)/Copy
		dp.SetText(winrt::to_hstring(selectedText.Data()));
		try
		{
			winrt::Windows::ApplicationModel::DataTransfer::Clipboard::SetContent(dp);
			winrt::Windows::ApplicationModel::DataTransfer::Clipboard::Flush();
		}
		catch (winrt::hresult_error const &ex)
		{
			// Todo: Retry
		}
	}

	void ScintillaWinUI::SetMouseCapture(bool on)
	{
		if (_wrapper)
		{
			_wrapper->SetMouseCapture(on);
		}
	}

	bool ScintillaWinUI::HaveMouseCapture()
	{
		return _wrapper
			? _wrapper->HaveMouseCapture()
			: false;
	}

	std::string ScintillaWinUI::UTF8FromEncoded(std::string_view encoded) const
	{
		return std::string();
	}

	std::string ScintillaWinUI::EncodedFromUTF8(std::string_view utf8) const
	{
		return std::string();
	}

	Scintilla::sptr_t ScintillaWinUI::DefWndProc(Scintilla::Message iMessage, Scintilla::uptr_t wParam, Scintilla::sptr_t lParam)
	{
		return 0;
	}

	void ScintillaWinUI::CreateCallTipWindow(PRectangle rc)
	{
	}

	void ScintillaWinUI::AddToPopUp(const char *label, int cmd, bool enabled)
	{
	}

	void ScintillaWinUI::RegisterGraphics(winrt::com_ptr<::ISurfaceImageSourceNativeWithD2D> const &sisNativeWithD2D,
		winrt::com_ptr<::IVirtualSurfaceImageSourceNative> const &vsisNative,
		winrt::com_ptr<::ID2D1DeviceContext> const &d2dDeviceContext,
		std::shared_ptr<MicaEditor::Wrapper> const &wrapper)
	{
		_sisNativeWithD2D = sisNativeWithD2D;
		_vsisNative = vsisNative;
		_d2dDeviceContext = d2dDeviceContext;

		_wrapper = wrapper;
		wMain = _wrapper.get();
	}

	void ScintillaWinUI::DpiChanged()
	{
		// Todo: need to re-evaluate how DPI is handled. might need to set DPI property on pBitmapRenderTarget in PlatWin.cxx
		InvalidateStyleRedraw();
	}

	void ScintillaWinUI::SizeChanged()
	{
		ChangeSize();
	}

	void ScintillaWinUI::FocusChanged(bool focused)
	{
		SetFocusState(focused);
	}

	void ScintillaWinUI::PointerPressed(winrt::Windows::Foundation::Point const &point, uint64_t timestamp, winrt::Windows::System::VirtualKeyModifiers modifiers)
	{
		if (_lock != NONE)
		{
			msgq.push(std::make_unique<PointerMessage>(PointerMessage::PointerEventType::PointerPressed, point, timestamp, modifiers));
		}
		else
		{
			ProcessPointerPressedMessage(point, timestamp, modifiers);
		}
	}

	void ScintillaWinUI::RightPointerPressed(winrt::Windows::Foundation::Point const &point, uint64_t timestamp, winrt::Windows::System::VirtualKeyModifiers modifiers)
	{
		if (_lock != NONE)
		{
			msgq.push(std::make_unique<PointerMessage>(PointerMessage::PointerEventType::RightPointerPressed, point, timestamp, modifiers));
		}
		else
		{
			ProcessRightPointerPressedMessage(point, timestamp, modifiers);
		}
	}

	void ScintillaWinUI::PointerMoved(winrt::Windows::Foundation::Point const &point, uint64_t timestamp, winrt::Windows::System::VirtualKeyModifiers modifiers)
	{
		//SetTrackMouseLeaveEvent(true); // WinUI Todo: fix this
		// Todo: might want to check if actually moved
		if (_lock != NONE)
		{
			msgq.push(std::make_unique<PointerMessage>(PointerMessage::PointerEventType::PointerMoved, point, timestamp, modifiers));
		}
		else
		{
			ProcessPointerMovedMessage(point, timestamp, modifiers);
		}
	}

	void ScintillaWinUI::PointerReleased(winrt::Windows::Foundation::Point const &point, uint64_t timestamp, winrt::Windows::System::VirtualKeyModifiers modifiers)
	{
		if (_lock != NONE)
		{
			msgq.push(std::make_unique<PointerMessage>(PointerMessage::PointerEventType::PointerReleased, point, timestamp, modifiers));
		}
		else
		{
			ProcessPointerReleasedMessage(point, timestamp, modifiers);
		}
	}

	void ScintillaWinUI::ProcessPointerPressedMessage(winrt::Windows::Foundation::Point const &point, uint64_t timestamp, winrt::Windows::System::VirtualKeyModifiers modifiers)
	{
		ButtonDownWithModifiers(Point{ point.X, point.Y }, timestamp, WindowsModifiers(modifiers));
	}

	void ScintillaWinUI::ProcessRightPointerPressedMessage(winrt::Windows::Foundation::Point const &point, uint64_t timestamp, winrt::Windows::System::VirtualKeyModifiers modifiers)
	{
		RightButtonDownWithModifiers(Point{ point.X, point.Y }, timestamp, WindowsModifiers(modifiers));
	}

	void ScintillaWinUI::ProcessPointerMovedMessage(winrt::Windows::Foundation::Point const &point, uint64_t timestamp, winrt::Windows::System::VirtualKeyModifiers modifiers)
	{
		//SetTrackMouseLeaveEvent(true); // WinUI Todo: fix this
		// Todo: might want to check if actually moved
		ButtonMoveWithModifiers(Point{ point.X, point.Y }, timestamp, WindowsModifiers(modifiers));
	}

	void ScintillaWinUI::ProcessPointerReleasedMessage(winrt::Windows::Foundation::Point const &point, uint64_t timestamp, winrt::Windows::System::VirtualKeyModifiers modifiers)
	{
		ButtonUpWithModifiers(Point{ point.X, point.Y }, timestamp, WindowsModifiers(modifiers));
	}

	void ScintillaWinUI::PointerWheelChanged(int delta, bool horizontal, winrt::Windows::System::VirtualKeyModifiers modifiers)
	{
		/*if (!mouseWheelCaptures) {
			// if the mouse wheel is not captured, test if the mouse
			// pointer is over the editor window and if not, don't
			// handle the message but pass it on.
			RECT rc;
			GetWindowRect(MainHWND(), &rc);
			const POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			if (!PtInRect(&rc, pt))
				return ::DefWindowProc(MainHWND(), iMessage, wParam, lParam);
		}
		// if autocomplete list active then send mousewheel message to it
		if (ac.Active()) {
			HWND hWnd = HwndFromWindow(*(ac.lb));
			::SendMessage(hWnd, iMessage, wParam, lParam);
			break;
		}*/

		// Treat Shift+WM_MOUSEWHEEL as horizontal scrolling, not data-zoom.
		if (horizontal || (static_cast<int>(modifiers) & static_cast<int>(winrt::Windows::System::VirtualKeyModifiers::Shift))) {
			if (vs.wrap.state != Wrap::None || charsPerScroll == 0) {
				return;
			}

			MouseWheelDelta &wheelDelta = horizontal ? horizontalWheelDelta : verticalWheelDelta;
			if (wheelDelta.Accumulate(delta)) {
				const int charsToScroll = charsPerScroll * wheelDelta.Actions() * (horizontal ? -1 : 1); // WinUI Todo: Make sure horizontal scrolling direction is correct
				const int widthToScroll = static_cast<int>(std::lround(charsToScroll * vs.aveCharWidth));

				// signChanged and uniformWheel prevent bumping at the edges on touchpads
				auto uniformWheel{ std::abs(_lastHorizontalScrollDelta) == std::abs(delta) };
				auto signChanged{ _lastHorizontalScrollDelta <= 0 && delta > 0 || _lastHorizontalScrollDelta >= 0 && delta < 0 };
				_lastHorizontalScrollDelta = delta;

				if (!uniformWheel && signChanged)
				{
					return;
				}

				HorizontalScrollToClamped(xOffset + widthToScroll);
			}
			return;
		}

		// Either SCROLL vertically or ZOOM. We handle the wheel steppings calculation
		if (linesPerScroll != 0 && verticalWheelDelta.Accumulate(delta)) {
			Sci::Line linesToScroll = linesPerScroll;
			if (linesPerScroll == WHEEL_PAGESCROLL)
				linesToScroll = LinesOnScreen() - 1;
			if (linesToScroll == 0) {
				linesToScroll = 1;
			}
			linesToScroll *= verticalWheelDelta.Actions();

			if (static_cast<int>(modifiers) & static_cast<int>(winrt::Windows::System::VirtualKeyModifiers::Control)) {
				// Zoom! We play with the font sizes in the styles.
				// Number of steps/line is ignored, we just care if sizing up or down
				if (linesToScroll < 0) {
					KeyCommand(Message::ZoomIn);
				}
				else {
					KeyCommand(Message::ZoomOut);
				}
			}
			else {
				// Scroll

				// signChanged and uniformWheel prevent bumping at the edges on touchpads
				auto uniformWheel{ std::abs(_lastVerticalScrollDelta) == std::abs(delta) };
				auto signChanged{ _lastVerticalScrollDelta <= 0 && delta > 0 || _lastVerticalScrollDelta >= 0 && delta < 0 };
				_lastVerticalScrollDelta = delta;
				if (!uniformWheel && signChanged)
				{
					return;
				}

				ScrollTo(topLine + linesToScroll);
			}
		}
	}

	void ScintillaWinUI::HorizontalScroll(ScrollEventType event, int value) {
		int xPos = xOffset;
		const PRectangle rcText = GetTextRectangle();
		const int pageWidth = static_cast<int>(rcText.Width() * 2 / 3);
		switch (event) {
		case ScrollEventType::SmallDecrement:
			xPos -= 20;
			break;
		case ScrollEventType::SmallIncrement:	// May move past the logical end
			xPos += 20;
			break;
		case ScrollEventType::LargeDecrement:
			xPos -= pageWidth;
			break;
		case ScrollEventType::LargeIncrement:
			xPos += pageWidth;
			break;
		case ScrollEventType::First:
			xPos = 0;
			break;
		case ScrollEventType::Last:
			xPos = scrollWidth;
			break;
		case ScrollEventType::ThumbPosition:
		case ScrollEventType::ThumbTrack: {
			xPos = value;
		}
										break;
		}
		HorizontalScrollToClamped(xPos);
	}

	void ScintillaWinUI::Scroll(ScrollEventType event, int value) {
		//DWORD dwStart = timeGetTime();
		//Platform::DebugPrintf("Scroll %x %d\n", wParam, lParam);

		//Platform::DebugPrintf("ScrollInfo %d mask=%x min=%d max=%d page=%d pos=%d track=%d\n", b,sci.fMask,
		//sci.nMin, sci.nMax, sci.nPage, sci.nPos, sci.nTrackPos);
		Sci::Line topLineNew = topLine;
		switch (event) {
		case ScrollEventType::SmallDecrement:
			topLineNew -= 1;
			break;
		case ScrollEventType::SmallIncrement:
			topLineNew += 1;
			break;
		case ScrollEventType::LargeDecrement:
			topLineNew -= LinesToScroll(); break;
		case ScrollEventType::LargeIncrement: topLineNew += LinesToScroll(); break;
		case ScrollEventType::First: topLineNew = 0; break;
		case ScrollEventType::Last: topLineNew = MaxScrollPos(); break;
		case ScrollEventType::ThumbPosition: topLineNew = value; break;
		case ScrollEventType::ThumbTrack: topLineNew = value; break;
		}
		ScrollTo(topLineNew);
	}

	void ScintillaWinUI::KeyDown(winrt::Windows::System::VirtualKey key, winrt::Windows::System::VirtualKeyModifiers modifiers)
	{
		if (_lock != NONE)
		{
			msgq.push(std::make_unique<KeyMessage>(KeyMessage::KeyEventType::KeyDown, key, modifiers));
		}
		else
		{
			ProcessKeyDownMessage(key, modifiers);
		}
	}

	IFACEMETHODIMP ScintillaWinUI::UpdatesNeeded()
	{
		ULONG drawingBoundsCount = 0;
		HRESULT hr = _vsisNative->GetUpdateRectCount(&drawingBoundsCount);
		if (FAILED(hr))
			return hr;

		std::unique_ptr<RECT[]> drawingBounds{ std::make_unique<RECT[]>(drawingBoundsCount) };
		hr = _vsisNative->GetUpdateRects(drawingBounds.get(), drawingBoundsCount);
		if (FAILED(hr))
			return hr;

		// Todo: Make sure paintState is being used correctly in this method
		paintState = PaintState::painting;

		// This code doesn't try to coalesce multiple drawing bounds into one. Although that
		// extra process will reduce the number of draw calls, it requires the virtual surface
		// image source to manage non-uniform tile size, which requires it to make extra copy
		// operations to the compositor. By using the drawing bounds it directly returns, which are
		// of non-overlapping uniform tile size, the compositor is able to use these tiles directly,
		// which can greatly reduce the amount of memory needed by the virtual surface image source.
		// It will result in more draw calls though, but Direct2D will be able to accommodate that
		// without significant impact on presentation frame rate.
		for (ULONG i = 0; i < drawingBoundsCount; i++)
		{
			DrawBit(drawingBounds[i]);
		}

		paintState = PaintState::notPainting;

		return hr;
	}

	void ScintillaWinUI::DrawBit(RECT const &drawingBounds)
	{
		winrt::com_ptr<IDXGISurface> surface;
		POINT surfaceOffset = { 0 };

		//Provide a pointer to IDXGISurface object to ISurfaceImageSourceNative::BeginDraw, and
		//draw into that surface using DirectX. Only the area specified for update in the
		//updateRect parameter is drawn.
		//
		//This method returns the point (x,y) offset of the updated target rectangle in the offset
		//parameter. You use this offset to determine where to draw into inside the IDXGISurface.
		HRESULT beginDrawHR = _sisNativeWithD2D->BeginDraw(drawingBounds, __uuidof(::IDXGISurface), surface.put_void(), &surfaceOffset);
		if (beginDrawHR == DXGI_ERROR_DEVICE_REMOVED || beginDrawHR == DXGI_ERROR_DEVICE_RESET) {
			// device changed
		}
		else {
			winrt::com_ptr<ID2D1Bitmap1> bitmap;
			HRESULT hrBitMap = _d2dDeviceContext->CreateBitmapFromDxgiSurface(
				surface.get(), nullptr, bitmap.put());
			if (FAILED(hrBitMap)) {
				winrt::throw_hresult(hrBitMap);
			}
			_d2dDeviceContext->BeginDraw();
			_d2dDeviceContext->SetTarget(bitmap.get());
			_d2dDeviceContext->SetTransform(D2D1::IdentityMatrix());

			// Translate the drawing to the designated place on the surface.
			D2D1::Matrix3x2F transform =
				D2D1::Matrix3x2F::Scale(1.0f, 1.0f) *
				D2D1::Matrix3x2F::Translation(
					static_cast<float>(surfaceOffset.x - drawingBounds.left),
					static_cast<float>(surfaceOffset.y - drawingBounds.top)
				);

			// Constrain the drawing only to the designated portion of the surface
			_d2dDeviceContext->PushAxisAlignedClip(
				D2D1::RectF(
					static_cast<float>(surfaceOffset.x),
					static_cast<float>(surfaceOffset.y),
					static_cast<float>(surfaceOffset.x + (drawingBounds.right - drawingBounds.left)),
					static_cast<float>(surfaceOffset.y + (drawingBounds.bottom - drawingBounds.top))
				),
				D2D1_ANTIALIAS_MODE_ALIASED
			);

			_d2dDeviceContext->SetTransform(transform);

			auto surf{ Scintilla::Internal::Surface::Allocate(technology) };
			surf->Init(_d2dDeviceContext.get(), nullptr);
			//surf->SetUnicodeMode(true); // Todo: Figure out what to make this
			//surf->SetDBCSMode(0);  // Todo: Figure out what to make this
			surf->SetMode(SurfaceMode{ 65001, false }); // Todo: Ensure these values are good
			rcPaint = Scintilla::Internal::PRectangle(drawingBounds.left, drawingBounds.top, drawingBounds.right, drawingBounds.bottom);
			paintingAllText = true;
			Paint(surf.get(), rcPaint);

			surf->Release();

			_d2dDeviceContext->EndDraw();
		}

		_sisNativeWithD2D->EndDraw();
	}

	UINT ScintillaWinUI::CodePageOfDocument() const noexcept {
		return CodePageFromCharSet(vs.styles[StyleDefault].characterSet, pdoc->dbcsCodePage);
	}

	sptr_t ScintillaWinUI::GetTextLength() {
		if (pdoc->dbcsCodePage == 0 || pdoc->dbcsCodePage == CpUtf8) {
			return pdoc->CountUTF16(0, pdoc->Length());
		}
		else {
			// Count the number of UTF-16 code units line by line
			const UINT cpSrc = CodePageOfDocument();
			const Sci::Line lines = pdoc->LinesTotal();
			Sci::Position codeUnits = 0;
			std::string lineBytes;
			for (Sci::Line line = 0; line < lines; line++) {
				const Sci::Position start = pdoc->LineStart(line);
				const Sci::Position width = pdoc->LineStart(line + 1) - start;
				lineBytes.resize(width);
				pdoc->GetCharRange(lineBytes.data(), start, width);
				codeUnits += WideCharLenFromMultiByte(cpSrc, lineBytes);
			}
			return codeUnits;
		}
	}

	sptr_t ScintillaWinUI::GetText(uptr_t bufferSize, sptr_t buffer) {
		if (buffer == 0) {
			return GetTextLength();
		}
		if (bufferSize == 0) {
			return 0;
		}
		wchar_t *ptr = static_cast<wchar_t *>(PtrFromSPtr(buffer));
		if (pdoc->Length() == 0) {
			*ptr = L'\0';
			return 0;
		}
		const Sci::Position lengthWanted = bufferSize - 1;
		if (IsUnicodeMode()) {
			Sci::Position sizeRequestedRange = pdoc->GetRelativePositionUTF16(0, lengthWanted);
			if (sizeRequestedRange < 0) {
				// Requested more text than there is in the document.
				sizeRequestedRange = pdoc->Length();
			}
			std::string docBytes(sizeRequestedRange, '\0');
			pdoc->GetCharRange(&docBytes[0], 0, sizeRequestedRange);
			const size_t uLen = UTF16FromUTF8(docBytes, ptr, lengthWanted);
			ptr[uLen] = L'\0';
			return uLen;
		}
		else {
			// Not Unicode mode
			// Convert to Unicode using the current Scintilla code page
			// Retrieve as UTF-16 line by line
			const UINT cpSrc = CodePageOfDocument();
			const Sci::Line lines = pdoc->LinesTotal();
			Sci::Position codeUnits = 0;
			std::string lineBytes;
			std::wstring lineAsUTF16;
			for (Sci::Line line = 0; line < lines && codeUnits < lengthWanted; line++) {
				const Sci::Position start = pdoc->LineStart(line);
				const Sci::Position width = pdoc->LineStart(line + 1) - start;
				lineBytes.resize(width);
				pdoc->GetCharRange(lineBytes.data(), start, width);
				const Sci::Position codeUnitsLine = WideCharLenFromMultiByte(cpSrc, lineBytes);
				lineAsUTF16.resize(codeUnitsLine);
				const Sci::Position lengthLeft = lengthWanted - codeUnits;
				WideCharFromMultiByte(cpSrc, lineBytes, lineAsUTF16.data(), lineAsUTF16.length());
				const Sci::Position lengthToCopy = std::min(lengthLeft, codeUnitsLine);
				lineAsUTF16.copy(ptr + codeUnits, lengthToCopy);
				codeUnits += lengthToCopy;
			}
			ptr[codeUnits] = L'\0';
			return codeUnits;
		}
	}

	bool ScintillaWinUI::SetText(std::wstring_view const &text)
	{
		// Todo: What about locks?

		if (text.size() == 0)
		{
			return 0;
		}
		UndoGroup ug(pdoc);
		pdoc->DeleteChars(0, pdoc->Length());
		SetEmptySelection(0);
		auto str{ EncodeWString(text) };
		pdoc->InsertString(0, str.c_str(), str.size());
		return 1;
	}

	void ScintillaWinUI::SetWndProc(std::function<LRESULT(winrt::Windows::Foundation::IInspectable const &, UINT, WPARAM, LPARAM)> wndProc)
	{
		_wndProc = wndProc;
	}

	void ScintillaWinUI::SetWndProcTag(winrt::Windows::Foundation::IInspectable const &tag)
	{
		_wndProcTag = tag;
	}

	LRESULT ScintillaWinUI::SendMessage(UINT msg, WPARAM wParam, LPARAM lParam)
	{
		return _wndProc ? _wndProc(_wndProcTag, msg, wParam, lParam) : 0;
	}
}
