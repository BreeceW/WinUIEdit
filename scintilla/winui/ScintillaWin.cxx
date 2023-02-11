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
		int _utf16Length;

	public:
		NotifyMessage::NotifyMessage(uptr_t wParam, NotificationData notificationData, bool notifyTsf, int utf16Length)
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

		int NotifyMessage::Utf16Length() const
		{
			return _utf16Length;
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
		bool _isExtendedKey;

	public:
		KeyMessage::KeyMessage(KeyEventType keyEvent, winrt::Windows::System::VirtualKey key, winrt::Windows::System::VirtualKeyModifiers modifiers, bool const isExtendedKey)
		{
			_keyEvent = keyEvent;
			_key = key;
			_modifiers = modifiers;
			_isExtendedKey = isExtendedKey;
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

		bool IsExtendedKey()
		{
			return _isExtendedKey;
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
		winrt::DUI::PointerPoint _pointerPoint{ nullptr };

	public:
		PointerMessage::PointerMessage(PointerEventType pointerEvent, winrt::Windows::Foundation::Point point, uint64_t timestamp, winrt::Windows::System::VirtualKeyModifiers modifiers, winrt::DUI::PointerPoint const &pointerPoint = nullptr)
		{
			_pointerEvent = pointerEvent;
			_point = point;
			_timestamp = timestamp;
			_modifiers = modifiers;
			_pointerPoint = pointerPoint;
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

		winrt::DUI::PointerPoint PointerPoint()
		{
			return _pointerPoint;
		}
	};

	constexpr uint8_t CharacterReceivedMessageId = 4;
	class CharacterReceivedMessage
		: public IMessage
	{
	private:
		char16_t _character;

	public:
		CharacterReceivedMessage(char16_t character)
		{
			_character = character;
		}

		uint8_t CharacterReceivedMessage::Type() const override
		{
			return CharacterReceivedMessageId;
		}

		char16_t Character() const
		{
			return _character;
		}
	};

	ScintillaWinUI::ScintillaWinUI()
	{
		pdoc->AllocateLineCharacterIndex(Scintilla::LineCharacterIndexType::Utf16);

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
			winrt::check_hresult(mq.hr);
			winrt::copy_from_abi(_tfThreadManager, mq.pItf);
			winrt::check_hresult(_tfThreadManager->Activate(&_tfClientId));
			winrt::check_hresult(_tfThreadManager->CreateDocumentMgr(_tfDocumentManager.put()));
			winrt::check_hresult(_tfDocumentManager->CreateContext(_tfClientId, 0, static_cast<ITextStoreACP2 *>(this), _tfContext.put(), &_tfEditCookie));
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

	void ScintillaWinUI::ProcessCharacterReceivedMessage(char16_t character)
	{
		// Todo: Make sure this updates TSF caret position correctly

		// Win32 code is:
		// ((wParam >= 128) || !iscntrl(static_cast<int>(wParam))) || !lastKeyDownConsumed
		// We are blocking control characters because, unlike in Win32, keyboard accelerators
		// do not override them
		if (character >= 128 || (!lastKeyDownConsumed && !iscntrl(static_cast<int>(character)))) {
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
				//_editContext.NotifySelectionChanged(winrt::Windows::UI::Text::Core::CoreTextRange{ static_cast<int32_t>(DocPositionToAcp(SelectionStart().Position())), static_cast<int32_t>(DocPositionToAcp(SelectionEnd().Position())) });
			}
			else if (_tfTextStoreACPSink)
			{
				_tfTextStoreACPSink->OnTextChange(0, &chg);
				// Todo: Figure out if adding OnSelectionChange here was ever needed
				//_tfTextStoreACPSink->OnSelectionChange();
			}
		}
	}

	// Todo: wParam might not make sense here
	void ScintillaWinUI::ProcessNotifyMessage(uptr_t wParam, NotificationData const &notificationData, bool notifyTsf, int utf16Length)
	{
		SendMessage(WM_NOTIFY, GetCtrlID(), reinterpret_cast<LPARAM>(&notificationData));

		if (!notifyTsf || !(_tsfCore || _tfTextStoreACPSink))
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
				chg.acpStart = DocPositionToAcp(notificationData.position);
			}
			if (FlagSet(notificationData.modificationType, Scintilla::ModificationFlags::InsertText))
			{
				fNotify = true;
				chg.acpOldEnd = DocPositionToAcp(notificationData.position);
				chg.acpNewEnd = DocPositionToAcp(notificationData.position) + utf16Length;
			}
			if (FlagSet(notificationData.modificationType, Scintilla::ModificationFlags::BeforeDelete))
			{
				chg.acpStart = DocPositionToAcp(notificationData.position);
			}
			if (FlagSet(notificationData.modificationType, Scintilla::ModificationFlags::DeleteText))
			{
				fNotify = true;
				chg.acpOldEnd = DocPositionToAcp(notificationData.position) + utf16Length;
				chg.acpNewEnd = DocPositionToAcp(notificationData.position);
			}
			if (fNotify)
			{
				if (_tsfCore)
				{
					_editContext.NotifyTextChanged(winrt::Windows::UI::Text::Core::CoreTextRange{ chg.acpStart, chg.acpNewEnd }, chg.acpOldEnd > chg.acpNewEnd ? 0 : utf16Length, winrt::Windows::UI::Text::Core::CoreTextRange{ chg.acpNewEnd, chg.acpNewEnd });
				}
				else if (_tfTextStoreACPSink)
				{
					DebugOut(L"OnTextChange, Start: %d, Old End: %d, New End: %d\n", chg.acpStart, chg.acpOldEnd, chg.acpNewEnd);
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
		case NotifyMessageId:
		{
			const auto notifyMessage{ static_cast<NotifyMessage *>(message.get()) };
			ProcessNotifyMessage(notifyMessage->WParam(), notifyMessage->NotificationData(), notifyMessage->NotifyTsf(), notifyMessage->Utf16Length());
		}
		break;

		case KeyMessageId:
		{
			const auto keyMessage{ static_cast<KeyMessage *>(message.get()) };
			switch (keyMessage->KeyEvent())
			{
			case KeyMessage::KeyEventType::KeyDown:
				ProcessKeyDownMessage(keyMessage->Key(), keyMessage->Modifiers(), keyMessage->IsExtendedKey(), nullptr);
				break;
			}
		}
		break;

		case PointerMessageId:
		{
			const auto pointerMessage{ static_cast<PointerMessage *>(message.get()) };
			switch (pointerMessage->PointerEvent())
			{
			case PointerMessage::PointerEventType::PointerPressed:
				ProcessPointerPressedMessage(pointerMessage->Point(), pointerMessage->Timestamp(), pointerMessage->Modifiers());
				break;
			case PointerMessage::PointerEventType::RightPointerPressed:
				ProcessRightPointerPressedMessage(pointerMessage->Point(), pointerMessage->Timestamp(), pointerMessage->Modifiers());
				break;
			case PointerMessage::PointerEventType::PointerMoved:
				ProcessPointerMovedMessage(pointerMessage->Point(), pointerMessage->Timestamp(), pointerMessage->Modifiers(), pointerMessage->PointerPoint());
				break;
			case PointerMessage::PointerEventType::PointerReleased:
				ProcessPointerReleasedMessage(pointerMessage->Point(), pointerMessage->Timestamp(), pointerMessage->Modifiers());
				break;
			}
		}
		break;

		case CharacterReceivedMessageId:
		{
			const auto characterReceivedMessage{ static_cast<CharacterReceivedMessage *>(message.get()) };
			ProcessCharacterReceivedMessage(characterReceivedMessage->Character());
		}
		break;
		}
	}

	constexpr bool KeyboardIsNumericKeypadFunction(winrt::Windows::System::VirtualKey key, bool const isExtendedKey)
	{
		if (isExtendedKey)
		{
			// Not from the numeric keypad
			return false;
		}

		switch (key)
		{
		case winrt::Windows::System::VirtualKey::Insert: // 0
		case winrt::Windows::System::VirtualKey::End: // 1
		case winrt::Windows::System::VirtualKey::Down: // 2
		case winrt::Windows::System::VirtualKey::PageDown: // 3
		case winrt::Windows::System::VirtualKey::Left: // 4
		case winrt::Windows::System::VirtualKey::Clear: // 5
		case winrt::Windows::System::VirtualKey::Right: // 6
		case winrt::Windows::System::VirtualKey::Home: // 7
		case winrt::Windows::System::VirtualKey::Up: // 8
		case winrt::Windows::System::VirtualKey::PageUp: // 9
			return true;
		default:
			return false;
		}
	}

	void ScintillaWinUI::ProcessKeyDownMessage(winrt::Windows::System::VirtualKey key, winrt::Windows::System::VirtualKeyModifiers modifiers, bool const isExtendedKey, bool *handled)
	{
		lastKeyDownConsumed = false;
		if (handled
			&& (static_cast<int>(modifiers) & static_cast<int>(winrt::Windows::System::VirtualKeyModifiers::Menu))
			&& KeyboardIsNumericKeypadFunction(key, isExtendedKey))
		{
			// Don't interpret these as they may be characters entered by number.
			*handled = false;
			return;
		}
		const int ret = KeyDownWithModifiers(
			KeyTranslate(static_cast<uptr_t>(key)),
			WindowsModifiers(modifiers),
			&lastKeyDownConsumed);

		// On Win32, the newline is inserted by CharacterReceived. This works fine
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

		if (handled && !ret && !lastKeyDownConsumed)
		{
			*handled = false;
		}
	}

	void ScintillaWinUI::ImeEndComposition()
	{
		// Called when IME Window closed
		// clear IME composition state.
		view.imeCaretBlockOverride = false;
		pdoc->TentativeUndo(); // Todo: IME undo support needs work
		ShowCaretAtCurrentPosition();
	}

	void ScintillaWinUI::CharacterReceived(char16_t character)
	{
		if (_lock != NONE)
		{
			msgq.push(std::make_unique<CharacterReceivedMessage>(character));
		}
		else
		{
			ProcessCharacterReceivedMessage(character);
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
		if (!TsfCoreLock(READONLY))
		{
			return;
		}

		WCHAR text[128];
		ULONG retText;
		TS_RUNINFO runs[33];
		ULONG retRuns;
		LONG next{ args.Request().Range().StartCaretPosition };
		LONG end{ std::min(args.Request().Range().EndCaretPosition, static_cast<int32_t>(DocPositionToAcp(pdoc->Length()))) };
		std::wstringstream str{};
		while (next < end)
		{
			if (FAILED(GetText(next, end, text, 128, &retText, runs, 33, &retRuns, &next)))
			{
				TsfCoreUnlock();

				return;
			}
			str << std::wstring_view{ text, retText };
		}
		args.Request().Text(str.str());

		TsfCoreUnlock();
	}

	void ScintillaWinUI::OnSelectionRequested(winrt::Windows::UI::Text::Core::CoreTextEditContext const &sender, winrt::Windows::UI::Text::Core::CoreTextSelectionRequestedEventArgs const &args)
	{
		if (!TsfCoreLock(READONLY))
		{
			return;
		}

		TS_SELECTION_ACP sel;
		ULONG sels;
		if (SUCCEEDED(GetSelection(TF_DEFAULT_SELECTION, 1, &sel, &sels)) && sels == 1)
		{
			args.Request().Selection(winrt::Windows::UI::Text::Core::CoreTextRange{ sel.acpStart, sel.acpEnd });
		}

		TsfCoreUnlock();
	}

	void ScintillaWinUI::OnFocusRemoved(winrt::Windows::UI::Text::Core::CoreTextEditContext const &sender, winrt::Windows::Foundation::IInspectable const &args)
	{
		SetFocusState(false);
	}

	void ScintillaWinUI::OnTextUpdating(winrt::Windows::UI::Text::Core::CoreTextEditContext const &sender, winrt::Windows::UI::Text::Core::CoreTextTextUpdatingEventArgs const &args)
	{
		if (!TsfCoreLock(READWRITE))
		{
			return;
		}

		TS_TEXTCHANGE chg;
		const auto hr{ SetText(0, args.Range().StartCaretPosition, args.Range().EndCaretPosition, args.Text().c_str(), args.Text().size(), &chg) };
		__super::SetSelection(AcpToDocPosition(args.NewSelection().StartCaretPosition), AcpToDocPosition(args.NewSelection().EndCaretPosition));
		args.Result(SUCCEEDED(hr)
			? winrt::Windows::UI::Text::Core::CoreTextTextUpdatingResult::Succeeded
			: winrt::Windows::UI::Text::Core::CoreTextTextUpdatingResult::Failed);

		TsfCoreUnlock();
	}

	void ScintillaWinUI::OnSelectionUpdating(winrt::Windows::UI::Text::Core::CoreTextEditContext const &sender, winrt::Windows::UI::Text::Core::CoreTextSelectionUpdatingEventArgs const &args)
	{
		if (!TsfCoreLock(READWRITE))
		{
			return;
		}

		TS_SELECTION_ACP sel;
		sel.acpStart = args.Selection().StartCaretPosition;
		sel.acpEnd = args.Selection().EndCaretPosition;
		sel.style.ase = TS_AE_END; // Todo: Figure out how to set this
		// Docs say "StartCaretPosition must always be less than or equal to the EndCaretPosition"
		// so it does not work like Scintilla, where they can be flipped
		sel.style.fInterimChar = 0; // Todo: Figure out how to set this
		const auto hr{ SetSelection(1, &sel) };
		args.Result(SUCCEEDED(hr)
			? winrt::Windows::UI::Text::Core::CoreTextSelectionUpdatingResult::Succeeded
			: winrt::Windows::UI::Text::Core::CoreTextSelectionUpdatingResult::Failed);

		TsfCoreUnlock();
	}

	void ScintillaWinUI::OnFormatUpdating(winrt::Windows::UI::Text::Core::CoreTextEditContext const &sender, winrt::Windows::UI::Text::Core::CoreTextFormatUpdatingEventArgs const &args)
	{
	}

	void ScintillaWinUI::OnLayoutRequested(winrt::Windows::UI::Text::Core::CoreTextEditContext const &sender, winrt::Windows::UI::Text::Core::CoreTextLayoutRequestedEventArgs const &args)
	{
		if (!TsfCoreLock(READONLY))
		{
			return;
		}

		RECT textRect;
		BOOL clipped;
		// Normally get cookie by GetActiveView, but it is hardcoded to 0 anyway
		if (SUCCEEDED(GetTextExt(0, args.Request().Range().StartCaretPosition, args.Request().Range().EndCaretPosition, &textRect, &clipped)))
		{
			// LayoutBoundsVisualPixels on Windows 1809+ should be useful but causes a crash
			args.Request().LayoutBounds().TextBounds(winrt::Windows::Foundation::Rect{ static_cast<float>(textRect.left), static_cast<float>(textRect.top), static_cast<float>(textRect.right - textRect.left), static_cast<float>(textRect.bottom - textRect.top) });
		}

		RECT screenRect;
		if (SUCCEEDED(GetScreenExt(0, &screenRect)))
		{
			args.Request().LayoutBounds().ControlBounds(winrt::Windows::Foundation::Rect{ static_cast<float>(screenRect.left), static_cast<float>(screenRect.top), static_cast<float>(screenRect.right - screenRect.left), static_cast<float>(screenRect.bottom - screenRect.top) });
		}

		// Todo: Make a helper method for converting RECT to Windows.Foundation.Rect

		TsfCoreUnlock();
	}

	void ScintillaWinUI::OnCompositionStarted(winrt::Windows::UI::Text::Core::CoreTextEditContext const &sender, winrt::Windows::UI::Text::Core::CoreTextCompositionStartedEventArgs const &args)
	{
		OnStartComposition(nullptr, nullptr);
	}

	void ScintillaWinUI::OnCompositionCompleted(winrt::Windows::UI::Text::Core::CoreTextEditContext const &sender, winrt::Windows::UI::Text::Core::CoreTextCompositionCompletedEventArgs const &args)
	{
		OnEndComposition(nullptr);
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

	bool ScintillaWinUI::TsfCoreLock(DWORD lock)
	{
		if (_lock == NONE)
		{
			_lock = static_cast<LockTypes>(lock);
			return true;
		}
		return false;
	}

	void ScintillaWinUI::TsfCoreUnlock()
	{
		ProcessQueues();

		_lock = NONE;
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

			ProcessQueues();

			_lock = NONE;
		}

		return hr;
	}

	void ScintillaWinUI::ProcessQueues()
	{
		while (!msgq.empty())
		{
			OutputDebugStringW(L"Processing Queued Message\n");
			ProcessMessage(msgq.front());
			msgq.pop();
		}
		_fromNotifyQueue = true;
		while (!notifyq.empty())
		{
			OutputDebugStringW(L"Processing Queued Notification\n");
			ProcessMessage(notifyq.front());
			notifyq.pop();
		}
		_fromNotifyQueue = false;
		while (!freeq.empty())
		{
			delete[] freeq.front();
			freeq.pop();
		}
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
		// Todo: Convert ACP to doc pos
		*pacpResultStart = pdoc->MovePositionOutsideChar(acpTestStart, 1, true);
		// Set the end point to the given end point.
		*pacpResultEnd = pdoc->MovePositionOutsideChar(acpTestEnd, -1, true);
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
		DebugOut(L"GetText: %d_%d\n", acpStart, acpEnd);

		if (_lock == NONE)
		{
			return TS_E_NOLOCK;
		}

		const auto lastAcp{ DocPositionToAcp(pdoc->Length()) };

		// acpEnd can be -1, initialize it properly.
		if (acpEnd == -1)
		{
			acpEnd = lastAcp;
		}

		if ((acpStart < 0) || (acpStart > acpEnd) || (acpEnd > lastAcp))
		{
			return TF_E_INVALIDPOS;
		}

		// Validate plain text buffers
		bool fDoPlainText = cchPlainReq > 0;
		if (fDoPlainText && (pchPlain == NULL || pcchPlainRet == NULL))
		{
			return E_INVALIDARG;
		}

		// Validate text run buffers
		bool fDoTextRun = cRunInfoReq > 0;
		if ((prgRunInfo == NULL || pcRunInfoRet == NULL))
		{
			return E_INVALIDARG;
		}

		// Validate window
		// WinUI Todo
		//if (!MainHWND())
			//return E_FAIL;

			// Limit request to size of internal buffers
#define MAX_RUN 128
		if (cchPlainReq > MAX_RUN)
		{
			cchPlainReq = MAX_RUN;
		}

		*pacpNext = acpStart;
		*pcchPlainRet = 0;
		*pcRunInfoRet = 0;

		if (fDoPlainText || fDoTextRun)
		{
			const UINT codePage{ static_cast<UINT>(IsUnicodeMode() ? CP_UTF8 : pdoc->dbcsCodePage) };

			prgRunInfo->type = TS_RT_PLAIN;
			prgRunInfo->uCount = 0;
			ULONG cchFetched(0);
			ULONG cRunsFetched(1);
			bool acpStartWithinSurrogatePair;
			const auto startPos{ AcpToDocPosition(acpStart, &acpStartWithinSurrogatePair) };
			bool acpEndWithinSurrogatePair;
			const auto endPos{ AcpToDocPosition(acpEnd, &acpEndWithinSurrogatePair) };
			bool fProtectionActive = vs.ProtectionActive();

			auto iPos{ startPos };

			if (iPos < endPos)
			{
				// IMEs can request an ACP that is within a surrogate pair, so special handling is needed
				// to insert half the character (two bytes of four) at the ends of the requested string
				if (acpStartWithinSurrogatePair && cchPlainReq > 0 && iPos - 3 > 0 && pdoc->LenChar(iPos - 4) == 4)
				{
					char narrowChar[4];
					for (auto i{ 0 }; i < 4; i++)
					{
						narrowChar[i] = pdoc->CharAt(iPos - 4 + i);
					}

					wchar_t wideChar[2];
					const auto wideCharSize{ MultiByteToWideChar(codePage, 0, narrowChar, 4, wideChar, 2) };
					if (wideCharSize == 2)
					{
						pchPlain[0] = wideChar[1];

						pchPlain++;
						cchPlainReq--;
						cchFetched++;

						const auto runType{ vs.styles[pdoc->StyleAt(iPos - 4)].IsProtected() ? TsRunType::TS_RT_HIDDEN : TsRunType::TS_RT_PLAIN };
						if (prgRunInfo->type == runType || cRunInfoReq == 0)
						{
							prgRunInfo->uCount++;
						}
						else
						{
							prgRunInfo++;
							cRunsFetched++;
							cRunInfoReq--;
							prgRunInfo->type = runType;
							prgRunInfo->uCount = 1;
						}
					}
				}

				// Scintilla supports folds (to hide unnecessary text). If we're requesting text that's either invisible
				// or unchangable, mark it hidden.
				// Todo: Consider marking unchangable text as visible & failing any SetText operations that cover unchangeable text
				if (fProtectionActive && vs.styles[pdoc->StyleAt(iPos)].IsProtected())
				{
					prgRunInfo->type = TS_RT_HIDDEN;
				}

				char text[MAX_RUN];	// Enough to handle an entire run of text
				// Go until we run out of space in the output buffers or we run out of text
				while (cchPlainReq > 0 && cRunInfoReq > 0 && iPos < endPos && (!acpEndWithinSurrogatePair || iPos < endPos - 4))
				{
					const auto multiByteCharSize{ pdoc->LenChar(iPos) };

					for (auto i{ 0 }; i < multiByteCharSize; i++)
					{
						text[i] = pdoc->CharAt(iPos + i);
					}
					const auto wideCharSize{ MultiByteToWideChar(codePage, 0, text, multiByteCharSize, pchPlain, cchPlainReq) }; // Todo: What are all the overloads of this method in this class?
					if (wideCharSize == 0 && GetLastError() == ERROR_INSUFFICIENT_BUFFER)
					{
						break;
					}
					pchPlain += wideCharSize;
					cchPlainReq -= wideCharSize;
					cchFetched += wideCharSize;

					auto runType{ TsRunType::TS_RT_PLAIN };
					if (fProtectionActive && vs.styles[pdoc->StyleAt(iPos)].IsProtected())
					{
						runType = TsRunType::TS_RT_HIDDEN;
					}
					// Update runs
					if (prgRunInfo->type == runType)
					{
						prgRunInfo->uCount += wideCharSize;
					}
					else
					{
						prgRunInfo++;
						cRunsFetched++;
						cRunInfoReq--;
						prgRunInfo->type = runType;
						prgRunInfo->uCount = wideCharSize;
					}
					iPos += multiByteCharSize;
				}

				if (acpEndWithinSurrogatePair && cchPlainReq > 0 && iPos + 3 < endPos && pdoc->LenChar(iPos) == 4)
				{
					char narrowChar[4];
					for (auto i{ 0 }; i < 4; i++)
					{
						narrowChar[i] = pdoc->CharAt(iPos + i);
					}

					wchar_t wideChar[2];
					const auto wideCharSize{ MultiByteToWideChar(codePage, 0, narrowChar, 4, wideChar, 2) };
					if (wideCharSize == 2)
					{
						pchPlain[0] = wideChar[0];

						pchPlain++;
						cchPlainReq--;
						cchFetched++;

						const auto runType{ vs.styles[pdoc->StyleAt(iPos)].IsProtected() ? TsRunType::TS_RT_HIDDEN : TsRunType::TS_RT_PLAIN };
						if (prgRunInfo->type == runType || cRunInfoReq == 0)
						{
							prgRunInfo->uCount++;
						}
						else
						{
							prgRunInfo++;
							cRunsFetched++;
							cRunInfoReq--;
							prgRunInfo->type = runType;
							prgRunInfo->uCount = 1;
						}
					}
				}

				*pacpNext = cchFetched + acpStart;
				*pcchPlainRet = cchFetched;
				*pcRunInfoRet = cRunsFetched;
			}
			else if (fProtectionActive && vs.styles[pdoc->StyleAt(iPos)].IsProtected())
			{
				prgRunInfo->type = TS_RT_HIDDEN;
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

		const auto startPos{ AcpToDocPosition(acpStart) };
		const auto endPos{ AcpToDocPosition(acpEnd) };

		const auto utf16Len{ pdoc->CountUTF16(startPos, endPos) };
		if (cch == 0)
		{
			pdoc->DeleteChars(startPos, endPos - startPos);
		}
		else
		{
			auto cchText{ -1 };
			char *szText{ nullptr };

			if (IsUnicodeMode())
			{
				cchText = UTF8Length({ pchText, cch });
				szText = new char[cchText + 1];
				if (!szText)
				{
					return E_OUTOFMEMORY;
				}
				UTF8FromUTF16({ pchText, cch }, szText, cchText);
			}
			else
			{
				const auto cpDest{ CodePageFromCharSet(vs.styles[STYLE_DEFAULT].characterSet, pdoc->dbcsCodePage) };
				cchText = WideCharToMultiByte(cpDest, 0, pchText, cch, nullptr, 0, nullptr, nullptr);
				szText = new char[cchText + 1];
				if (!szText)
				{
					return E_OUTOFMEMORY;
				}
				WideCharToMultiByte(cpDest, 0, pchText, cch, szText, cchText, nullptr, nullptr);
			}
			szText[cchText] = '\0';

			__super::SetSelection(startPos, endPos); // Todo: Fix multi-caret support
			try
			{
				InsertCharacter(szText, Scintilla::CharacterSource::DirectInput);
			}
			catch (std::runtime_error const &)
			{
				// Todo: this catches in error in trying to convert some longer strings into UTF-32
				// This is not ideal and InsertCharacter should probably be replaced with a custom method based off it
			}
			freeq.push(szText);
		}

		if (pChange)
		{
			auto newAcpStart{ DocPositionToAcp(startPos) };
			pChange->acpStart = newAcpStart;
			pChange->acpOldEnd = newAcpStart + utf16Len;
			pChange->acpNewEnd = newAcpStart + cch;
			DebugOut(L"SetText, Start: %d, Old End: %d, New End: %d\n", pChange->acpStart, pChange->acpOldEnd, pChange->acpNewEnd);
		}

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
		{
			return TS_E_NOLOCK;
		}

		// Todo: Original note: What about offscreen positions? I really don't think this works here.
		// Todo: Translate to screen. DPI?
		const auto pr{ RectangleFromRange(Range{ AcpToDocPosition(acpStart), AcpToDocPosition(acpEnd) }, 0) };
		prc->left = pr.left;
		prc->top = pr.top;
		prc->right = pr.right;
		prc->bottom = pr.bottom;

		*pfClipped = 0; // Todo: Not sure what this needs to be

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

		// Todo: Translate to screen. DPI?
		const auto pr{ GetTextRectangle() };
		prc->left = pr.left;
		prc->top = pr.top;
		prc->right = pr.right;
		prc->bottom = pr.bottom;

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

	Sci::Position ScintillaWinUI::AcpToDocPosition(Sci::Position acp, bool *acpWithinSurrogatePair)
	{
		const Sci::Line line{ pdoc->LineFromPositionIndex(acp, Scintilla::LineCharacterIndexType::Utf16) };
		Sci::Position c{ pdoc->IndexLineStart(line, Scintilla::LineCharacterIndexType::Utf16) };
		Sci::Position p{ pdoc->LineStart(line) };

		while (c < acp)
		{
			Sci::Position lp{ p };
			p = pdoc->NextPosition(p, 1);
			if (p - lp > 3)
			{
				c++;
			}
			c++;
		}

		if (acpWithinSurrogatePair)
		{
			*acpWithinSurrogatePair = c != acp;
		}

		return p;
	}

	Sci::Position ScintillaWinUI::DocPositionToAcp(Sci::Position docPosition)
	{
		const auto line{ pdoc->LineFromPosition(docPosition) };
		return pdoc->CountUTF16(pdoc->LineStart(line), docPosition) + pdoc->IndexLineStart(pdoc->LineFromPosition(docPosition), Scintilla::LineCharacterIndexType::Utf16);
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
		else if (_tfThreadManager)
		{
			winrt::check_hresult(_tfThreadManager->SetFocus(focus ? _tfDocumentManager.get() : nullptr));
		}
		Editor::NotifyFocus(focus);
	}

	void ScintillaWinUI::NotifyParent(Scintilla::NotificationData scn)
	{
		if (_lock != NONE)
		{
			// Todo: notification queue might defeat the point of "before" notifications
			notifyq.push(std::make_unique<NotifyMessage>(static_cast<uptr_t>(GetCtrlID()), scn, _fromNotifyQueue, CalculateNotifyMessageUtf16Length(scn.nmhdr.code, scn.modificationType, _shouldNotifyTsf, scn.text, scn.length)));
		}
		else
		{
			ProcessNotifyMessage(static_cast<uptr_t>(GetCtrlID()), scn, _shouldNotifyTsf, CalculateNotifyMessageUtf16Length(scn.nmhdr.code, scn.modificationType, _shouldNotifyTsf, scn.text, scn.length));
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

	void ScintillaWinUI::CreateGraphicsDevices()
	{
		// Todo: Is there any cleanup that needs to be done when the control is deleted or if the device gets re-created?

		uint32_t creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1,
		};

		// Create the Direct3D device
		winrt::com_ptr<ID3D11Device> d3dDevice;
		D3D_FEATURE_LEVEL supportedFeatureLevel;
		winrt::check_hresult(D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			0,
			creationFlags,
			featureLevels,
			ARRAYSIZE(featureLevels),
			D3D11_SDK_VERSION,
			d3dDevice.put(),
			&supportedFeatureLevel,
			nullptr));

		// Get the Direct3D device.
		_dxgiDevice = d3dDevice.as<IDXGIDevice3>();

		// Create the Direct2D device and a corresponding context
		winrt::com_ptr<ID2D1Device> d2dDevice;
		D2D1CreateDevice(_dxgiDevice.get(), nullptr, d2dDevice.put());

		winrt::check_hresult(
			d2dDevice->CreateDeviceContext(
				D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
				_d2dDeviceContext.put()));

		// Todo: Call _d2dDeviceContext->SetDpi and fix how DPI is handled throughout control https://learn.microsoft.com/en-us/windows/win32/direct2d/direct2d-and-high-dpi

		// Associate the Direct2D device with the SurfaceImageSource
		_sisNativeWithD2D->SetDevice(d2dDevice.get());
	}

	void ScintillaWinUI::RegisterGraphics(std::shared_ptr<MicaEditor::Wrapper> const &wrapper)
	{
		_wrapper = wrapper;
		wMain = _wrapper.get(); // Todo: Make sure this makes sense

		_vsisNative = wrapper->VsisNative();
		_sisNativeWithD2D = _vsisNative.as<ISurfaceImageSourceNativeWithD2D>();

		CreateGraphicsDevices();
	}

	void ScintillaWinUI::TrimGraphics()
	{
		// Todo: Should ClearResources get called too? https://github.com/microsoft/Win2D/blob/master/winrt/lib/drawing/CanvasDevice.cpp#L1040
		_dxgiDevice->Trim();
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

	void ScintillaWinUI::PointerMoved(winrt::Windows::Foundation::Point const &point, uint64_t timestamp, winrt::Windows::System::VirtualKeyModifiers modifiers, winrt::DUI::PointerPoint const &pointerPoint)
	{
		//SetTrackMouseLeaveEvent(true); // WinUI Todo: fix this
		// Todo: might want to check if actually moved
		if (_lock != NONE)
		{
			msgq.push(std::make_unique<PointerMessage>(PointerMessage::PointerEventType::PointerMoved, point, timestamp, modifiers, pointerPoint));
		}
		else
		{
			ProcessPointerMovedMessage(point, timestamp, modifiers, pointerPoint);
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
		const Point pt{ point.X, point.Y };
		if (!PointInSelection(pt))
		{
			CancelModes();
			SetEmptySelection(PositionFromLocation(pt));
		}

		RightButtonDownWithModifiers(pt, timestamp, WindowsModifiers(modifiers));
	}

	void ScintillaWinUI::ProcessPointerMovedMessage(winrt::Windows::Foundation::Point const &point, uint64_t timestamp, winrt::Windows::System::VirtualKeyModifiers modifiers, winrt::DUI::PointerPoint const &pointerPoint)
	{
		//SetTrackMouseLeaveEvent(true); // WinUI Todo: fix this
		// Todo: might want to check if actually moved
		if (inDragDrop == DragDrop::initial)
		{
			_dragPointer = pointerPoint;
		}
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

	void ScintillaWinUI::KeyDown(winrt::Windows::System::VirtualKey key, winrt::Windows::System::VirtualKeyModifiers modifiers, bool const isExtendedKey, bool *handled)
	{
		if (_lock != NONE)
		{
			msgq.push(std::make_unique<KeyMessage>(KeyMessage::KeyEventType::KeyDown, key, modifiers, isExtendedKey));
		}
		else
		{
			ProcessKeyDownMessage(key, modifiers, isExtendedKey, handled);
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
		if (beginDrawHR == DXGI_ERROR_DEVICE_REMOVED || beginDrawHR == DXGI_ERROR_DEVICE_RESET || beginDrawHR == E_SURFACE_CONTENTS_LOST)
		{
			// For surface lost:
			// Handles what seems to be a XAML-specific issue https://github.com/Microsoft/Win2D/blob/master/winrt/lib/xaml/CanvasControl.cpp#L199
			// Always reproducible by disabling GPU in Device Manager
			// Sometimes reproducible by launching with debugger attached and switching virtual desktops (wait a few seconds before switching)
			// Sometimes reproducible by minimize and restoring repeatedly
			// See https://github.com/Microsoft/Win2D/issues/584
			// https://learn.microsoft.com/en-us/windows/uwp/gaming/directx-and-xaml-interop
			// That says you do not need to re-create the devices when E_SURFACE_CONTENTS_LOST but in testing it seems necessary
			// https://learn.microsoft.com/en-us/windows/uwp/gaming/handling-device-lost-scenarios

			CreateGraphicsDevices();
			InvalidateStyleRedraw(); // just Redraw() does not work
		}
		else
		{
			winrt::com_ptr<ID2D1Bitmap1> bitmap;
			// surface can be null if E_FAIL (got while debugging)
			// Todo: handle general fail case for above
			HRESULT hrBitMap = _d2dDeviceContext->CreateBitmapFromDxgiSurface(
				surface.get(), nullptr, bitmap.put());
			if (FAILED(hrBitMap))
			{
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

	winrt::Windows::ApplicationModel::DataTransfer::DataPackageOperation ScintillaWinUI::EffectFromState(winrt::Windows::ApplicationModel::DataTransfer::DataPackageOperation const &allowedOperations, winrt::Windows::ApplicationModel::DataTransfer::DragDrop::DragDropModifiers const &grfKeyState) const noexcept {
		// Control is for copy and alt is for move
		winrt::Windows::ApplicationModel::DataTransfer::DataPackageOperation dwEffect{ static_cast<int>(allowedOperations) & static_cast<int>(winrt::Windows::ApplicationModel::DataTransfer::DataPackageOperation::Move)
			? winrt::Windows::ApplicationModel::DataTransfer::DataPackageOperation::Move
			: winrt::Windows::ApplicationModel::DataTransfer::DataPackageOperation::Copy };
		if (static_cast<int>(grfKeyState) & static_cast<int>(winrt::Windows::ApplicationModel::DataTransfer::DragDrop::DragDropModifiers::Alt)
			&& static_cast<int>(allowedOperations) & static_cast<int>(winrt::Windows::ApplicationModel::DataTransfer::DataPackageOperation::Move))
		{
			dwEffect = winrt::Windows::ApplicationModel::DataTransfer::DataPackageOperation::Move;
		}
		if (static_cast<int>(grfKeyState) & static_cast<int>(winrt::Windows::ApplicationModel::DataTransfer::DragDrop::DragDropModifiers::Control)
			&& static_cast<int>(allowedOperations) & static_cast<int>(winrt::Windows::ApplicationModel::DataTransfer::DataPackageOperation::Copy))
		{
			dwEffect = winrt::Windows::ApplicationModel::DataTransfer::DataPackageOperation::Copy;
		}
		return dwEffect;
	}

	void ScintillaWinUI::DragEnter(winrt::Windows::ApplicationModel::DataTransfer::DataPackageView const &dataView, winrt::Windows::ApplicationModel::DataTransfer::DataPackageOperation const &allowedOperations, winrt::Windows::ApplicationModel::DataTransfer::DragDrop::DragDropModifiers const &modifiers, winrt::Windows::ApplicationModel::DataTransfer::DataPackageOperation &operation)
	{
		hasOKText = dataView.Contains(winrt::Windows::ApplicationModel::DataTransfer::StandardDataFormats::Text());
		if (hasOKText)
		{
			operation = EffectFromState(allowedOperations, modifiers);
		}
	}

	void ScintillaWinUI::DragOver(winrt::Windows::Foundation::Point const &point, winrt::Windows::ApplicationModel::DataTransfer::DataPackageOperation const &allowedOperations, winrt::Windows::ApplicationModel::DataTransfer::DragDrop::DragDropModifiers const &modifiers, winrt::Windows::ApplicationModel::DataTransfer::DataPackageOperation &operation)
	{
		try
		{
			if (!hasOKText || pdoc->IsReadOnly())
			{
				operation = winrt::Windows::ApplicationModel::DataTransfer::DataPackageOperation::None;
				return;
			}

			operation = EffectFromState(allowedOperations, modifiers);

			// Update the cursor.
			SetDragPosition(SPositionFromLocation(Point{ point.X, point.Y }, false, false, UserVirtualSpace()));
		}
		catch (...)
		{
			errorStatus = Status::Failure;
		}
	}

	void ScintillaWinUI::DragLeave()
	{
		try
		{
			SetDragPosition(SelectionPosition(Sci::invalidPosition));
		}
		catch (...)
		{
			errorStatus = Status::Failure;
		}
	}

	void ScintillaWinUI::Drop(winrt::Windows::Foundation::Point const &point, winrt::Windows::ApplicationModel::DataTransfer::DataPackageView const &dataView, winrt::Windows::ApplicationModel::DataTransfer::DataPackageOperation const &allowedOperations, winrt::Windows::ApplicationModel::DataTransfer::DragDrop::DragDropModifiers const &modifiers, winrt::Windows::ApplicationModel::DataTransfer::DataPackageOperation &operation)
	{
		try
		{
			operation = EffectFromState(allowedOperations, modifiers);

			SetDragPosition(SelectionPosition(Sci::invalidPosition));

			DoDropAsync(point, dataView, operation);
		}
		catch (...)
		{
			errorStatus = Status::Failure;
		}
	}

	winrt::fire_and_forget ScintillaWinUI::DoDropAsync(winrt::Windows::Foundation::Point const point, winrt::Windows::ApplicationModel::DataTransfer::DataPackageView const dataView, winrt::Windows::ApplicationModel::DataTransfer::DataPackageOperation const operation)
	{
		try
		{
			std::string putf{ winrt::to_string(co_await dataView.GetTextAsync()) }; // Todo: Fix (check content type and async)

			if (putf.empty())
			{
				co_return;
			}

			bool isRectangular{ false }; // WinUI Todo: implement

			const SelectionPosition movePos = SPositionFromLocation(Point{ point.X, point.Y }, false, false, UserVirtualSpace());

			DropAt(movePos, putf.c_str(), putf.size(), operation == winrt::Windows::ApplicationModel::DataTransfer::DataPackageOperation::Move, isRectangular);
		}
		catch (...)
		{
			errorStatus = Status::Failure;
		}
	}

	// WinUI Todo: consider overriding DragThreshold

	void ScintillaWinUI::StartDrag()
	{
		DoDragAsync();
	}

	winrt::fire_and_forget ScintillaWinUI::DoDragAsync()
	{
		if (!_wrapper || !_dragPointer)
		{
			co_return;
		}
		inDragDrop = DragDrop::dragging;
		DWORD dwEffect = 0;
		dropWentOutside = true;
		//Platform::DebugPrintf("About to DoDragDrop %x %x\n", pDataObject, pDropSource);
		try
		{
			const auto operation{ co_await _wrapper->StartDragAsync(_dragPointer) };
			//Platform::DebugPrintf("DoDragDrop = %x\n", hr);
			if (operation == winrt::Windows::ApplicationModel::DataTransfer::DataPackageOperation::Move && dropWentOutside)
			{
				// Remove dragged out text
				ClearSelection();
			}
		}
		catch (winrt::hresult_error const &)
		{
		}
		inDragDrop = DragDrop::none;
		_dragPointer = nullptr;
		SetDragPosition(SelectionPosition(Sci::invalidPosition));
	}

	int ScintillaWinUI::CalculateNotifyMessageUtf16Length(Scintilla::Notification const &code, Scintilla::ModificationFlags const &modFlags, bool notifyTsf, const char *text, Scintilla::Position mbLength)
	{
		return notifyTsf
			&& code == Scintilla::Notification::Modified
			&& FlagSet(modFlags, Scintilla::ModificationFlags::InsertText | Scintilla::ModificationFlags::DeleteText)
			? WideCharLenFromMultiByte(IsUnicodeMode() ? CP_UTF8 : pdoc->dbcsCodePage, std::string_view{ text, static_cast<size_t>(mbLength) })
			: 0;
	}

	std::string_view ScintillaWinUI::GetDragData()
	{
		return { drag.Data(), drag.LengthWithTerminator() };
	}

	LRESULT ScintillaWinUI::SendMessage(UINT msg, WPARAM wParam, LPARAM lParam)
	{
		return _wndProc ? _wndProc(_wndProcTag, msg, wParam, lParam) : 0;
	}

	sptr_t ScintillaWinUI::WndProc(Message iMessage, uptr_t wParam, sptr_t lParam)
	{
		// Todo: Consider implementing these
		/*
		case Message::GetDirectFunction:
		case Message::GetDirectStatusFunction:
		case Message::GetDirectPointer:
		case Message::GrabFocus:
		case Message::SetTechnology:
		case Message::SetBidirectional:
		case Message::TargetAsUTF8:
		case Message::EncodedFromUTF8:
		*/

		switch (iMessage)
		{
		case Scintilla::Message::SetBidirectional:
		{
			if (static_cast<Bidirectional>(wParam) <= Bidirectional::R2L)
			{
				bidirectional = static_cast<Bidirectional>(wParam);
			}
			// Invalidate all cached information including layout.
			InvalidateStyleRedraw();
		}
		return 0;

		default:
			return __super::WndProc(iMessage, wParam, lParam);
		}
	}
}
