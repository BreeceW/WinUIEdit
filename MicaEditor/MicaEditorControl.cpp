#include "pch.h"
#include "MicaEditorControl.h"
#if __has_include("MicaEditorControl.g.cpp")
#include "MicaEditorControl.g.cpp"
#endif
#include "EditorWrapper.h"
#include "ScaledMessage.h"

//#define JSON

using namespace ::MicaEditor;
using namespace winrt;
using namespace DUX;
using namespace DUX::Controls;
using namespace DUX::Controls::Primitives;
using namespace DUX::Input;
using namespace DUX::Media;
using namespace DUX::Media::Imaging;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::System;
using namespace Windows::Graphics::Display;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::DataTransfer;

namespace winrt::MicaEditor::implementation
{
	// Todo: Something about this control is keeping the WinUI 3 versions from closing.
	// Note that making this into a blank control fixes the issue, so it is definitely something here.
	MicaEditorControl::MicaEditorControl()
	{
		DefaultStyleKey(winrt::box_value(L"MicaEditor.MicaEditorControl"));

		_wrapper = std::make_shared<Wrapper>();

		_loadedRevoker = Loaded(auto_revoke, { this, &MicaEditorControl::OnLoaded });
		_unloadedRevoker = Unloaded(auto_revoke, { this, &MicaEditorControl::OnUnloaded });

#ifndef WINUI3
		auto displayInformation{ DisplayInformation::GetForCurrentView() };
		_dpiChangedRevoker = displayInformation.DpiChanged(auto_revoke, { this, &MicaEditorControl::OnDpiChanged });
		UpdateDisplayInformation(displayInformation.RawPixelsPerViewPixel(), displayInformation.LogicalDpi());
#endif

		_scintilla = make_self<Scintilla::Internal::ScintillaWinUI>();

		_editorWrapper = make<implementation::Editor>(get_strong());

		_scintilla->SetWndProcTag(*this);
		_scintilla->SetWndProc(&MicaEditorControl::WndProc);

		// Scintilla has built-in shortcuts for zoom in and out, but they only work with the numpad plus/minus
		_scintilla->WndProc(Scintilla::Message::AssignCmdKey, 187 + (SCMOD_CTRL << 16), SCI_ZOOMIN); // Ctrl+Plus
		_scintilla->WndProc(Scintilla::Message::AssignCmdKey, 189 + (SCMOD_CTRL << 16), SCI_ZOOMOUT); // Ctrl+Minus
		_scintilla->WndProc(Scintilla::Message::AssignCmdKey, 48 + (SCMOD_CTRL << 16), SCI_SETZOOM); // Ctrl+0

		_scintilla->WndProc(Scintilla::Message::SetMultipleSelection, true, 0);
		_scintilla->WndProc(Scintilla::Message::SetScrollWidth, 2000 * _dpiScale, 0); // Not updating on DPI change because this value can change
		_scintilla->WndProc(Scintilla::Message::SetScrollWidthTracking, true, 0);
		_scintilla->WndProc(Scintilla::Message::SetYCaretPolicy, CARET_SLOP | CARET_STRICT | CARET_EVEN, 1);
		_scintilla->WndProc(Scintilla::Message::SetVisiblePolicy, VISIBLE_SLOP, 0);
		_scintilla->WndProc(Scintilla::Message::SetHScrollBar, true, 0);
		_scintilla->WndProc(Scintilla::Message::SetEndAtLastLine, false, 0);
		_scintilla->WndProc(Scintilla::Message::SetTabWidth, 4, 0);
		_scintilla->WndProc(Scintilla::Message::SetMarginTypeN, 1, SC_MARGIN_NUMBER);
		_scintilla->WndProc(Scintilla::Message::StyleSetFont, STYLE_DEFAULT, reinterpret_cast<Scintilla::sptr_t>("Cascadia Code"));
		_scintilla->WndProc(Scintilla::Message::StyleSetSizeFractional, STYLE_DEFAULT, 11 * SC_FONT_SIZE_MULTIPLIER);

		_scaleMessages = single_threaded_map<ScintillaMessage, MicaEditor::ScaledMessage>();
		_scaleMessages.Insert(ScintillaMessage::SetXCaretPolicy, MicaEditor::ScaledMessage{ false, true });
		_scaleMessages.Insert(ScintillaMessage::SetMarginWidthN, MicaEditor::ScaledMessage{ false, true });
		_scaleMessages.Insert(ScintillaMessage::SetMarginLeft, MicaEditor::ScaledMessage{ false, true });
		_scaleMessages.Insert(ScintillaMessage::SetMarginRight, MicaEditor::ScaledMessage{ false, true });
		_scaleMessages.Insert(ScintillaMessage::SetCaretWidth, MicaEditor::ScaledMessage{ true, false });
		PublicWndProc(Scintilla::Message::SetXCaretPolicy, CARET_SLOP, 20);
		PublicWndProc(Scintilla::Message::SetMarginWidthN, 1, 45);
		PublicWndProc(Scintilla::Message::SetMarginLeft, 0, 23);
		PublicWndProc(Scintilla::Message::SetMarginRight, 0, 12);
		PublicWndProc(Scintilla::Message::SetCaretWidth, _uiSettings.CaretWidth(), 0); // Todo: Needs to stop blinking after timeout and respect blink rate*/

#ifdef JSON
		const auto lexer{ CreateLexer("json") };
		lexer->PropertySet("lexer.json.allow.comments", "1");
		lexer->PropertySet("lexer.json.escape.sequence", "1");
		_scintilla->WndProc(Scintilla::Message::SetILexer, 0, reinterpret_cast<Scintilla::uptr_t>(lexer));
#else
		_scintilla->WndProc(Scintilla::Message::SetILexer, 0, reinterpret_cast<Scintilla::uptr_t>(CreateLexer("cpp")));
		// This list of keywords from SciTe (cpp.properties)
		_scintilla->WndProc(Scintilla::Message::SetKeyWords, 0, reinterpret_cast<Scintilla::uptr_t>(
			"alignas alignof and and_eq asm audit auto axiom bitand bitor bool "
			"char char8_t char16_t char32_t class compl concept "
			"const consteval constexpr const_cast "
			"decltype default delete double dynamic_cast enum explicit export extern false final float "
			"friend import inline int long module mutable namespace new noexcept not not_eq nullptr "
			"operator or or_eq override private protected public "
			"register reinterpret_cast requires "
			"short signed sizeof static static_assert static_cast struct "
			"template this thread_local true typedef typeid typename union unsigned using "
			"virtual void volatile wchar_t xor xor_eq"));
		_scintilla->WndProc(Scintilla::Message::SetKeyWords, 1, reinterpret_cast<Scintilla::uptr_t>(
			"break case catch co_await co_return co_yield continue do else for goto if return switch throw try while "));
#endif

		// Use the new ActualTheme property on Fall Creators Update and above. On WinUI 3, this is always present, so the check is not needed
		// ActualThemeChanged has to be registered and unregistered in Loaded/Unloaded or it will not work after unloading
#ifndef WINUI3
		if (_hasFcu)
		{
#endif
			// Registering the CharacterReceived event causes undesirable behavior with TSF in CoreWindow (normal UWP)
			// but is required to get text in classic windows (XAML Islands and WinUI 3)
			// Todo: Find more ideal way to do this
			// Tried using _tfThreadManager->GetActiveFlags but TF_TMF_IMMERSIVEMODE flag was not accurate
			if (Helpers::IsClassicWindow())
			{
				CharacterReceived({ this, &MicaEditorControl::MicaEditorControl_CharacterReceived });
			}

			UpdateColors(ActualTheme() == ElementTheme::Dark);
#ifndef WINUI3
		}
		else
		{
			// This does not work if RequestedTheme is changed on a parent control in Windows 10, version 1703
			_colorValuesChangedRevoker = _uiSettings.ColorValuesChanged(auto_revoke, { this, &MicaEditorControl::OnColorValuesChanged });
			UpdateColors(UseDarkColors());
		}
#endif
	}

	MicaEditorControl::~MicaEditorControl()
	{
		_scintilla->Finalize();
	}

	hstring MicaEditorControl::Text()
	{
		return GetValue(s_textProperty).as<hstring>();
	}

	void MicaEditorControl::Text(hstring const &value)
	{
		SetValue(s_textProperty, box_value(value));
	}

	MicaEditor::Editor MicaEditorControl::Editor()
	{
		return _editorWrapper;
	}

	void MicaEditorControl::OnLoaded(IInspectable const &sender, DUX::RoutedEventArgs const &args)
	{
#ifndef WINUI3
		if (_hasFcu)
		{
#endif
			// Registering and unregistering in Loaded/Unloaded fixes an issue
			// where ActualThemeChanged stopped firing after navigating to a different page
			_actualThemeChangedRevoker = ActualThemeChanged(auto_revoke, { this, &MicaEditorControl::OnActualThemeChanged });
#ifndef WINUI3
		}
#endif
	}

	void MicaEditorControl::OnUnloaded(IInspectable const &sender, DUX::RoutedEventArgs const &args)
	{
		_scintilla->StopTimers();

#ifndef WINUI3
		if (_hasFcu)
		{
#endif
			_actualThemeChangedRevoker.revoke();
#ifndef WINUI3
		}
#endif
	}

	void MicaEditorControl::OnDpiChanged(DisplayInformation const &sender, IInspectable const &args)
	{
		UpdateDisplayInformation(sender.RawPixelsPerViewPixel(), sender.LogicalDpi());
		_scintilla->DpiChanged();
	}

	void MicaEditorControl::OnActualThemeChanged(FrameworkElement const &sender, IInspectable const &args)
	{
		UpdateColors(ActualTheme() == ElementTheme::Dark);
	}

#ifndef WINUI3
	void MicaEditorControl::OnColorValuesChanged(Windows::UI::ViewManagement::UISettings const &uiSettings, IInspectable const &args)
	{
		// Todo: Not sure if there is a way to detect if the RequestedTheme changes in Creators Update. Unimportant
		Dispatcher().RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, [&]()
			{
				UpdateColors(UseDarkColors());
			});
	}

	bool MicaEditorControl::UseDarkColors()
	{
		auto theme{ RequestedTheme() };
		return theme == ElementTheme::Default ? Application::Current().RequestedTheme() == ApplicationTheme::Dark : theme == ElementTheme::Dark;
	}
#endif

	void MicaEditorControl::UpdateDisplayInformation(float dpiScale, float logicalDpi)
	{
		_dpiScale = dpiScale;
		_logicalDpi = logicalDpi;
		UpdateSizes();
	}

	void MicaEditorControl::UpdateColors(bool useDarkTheme)
	{
		if (_useDarkTheme.has_value() && useDarkTheme == *_useDarkTheme)
		{
			return;
		}
		_useDarkTheme = useDarkTheme;

		// This is just a random color that our backend will treat as transparent
		// The Scintilla API doesn't let us set alpha on back color, so this is easier
		// Users of this control are meant to control the background through XAML
		// This only affects rectangles. Text can use this color
		// Todo: Come up with a better solution
		constexpr sptr_t transparencyColor{ -7791875 };
		// These colors mostly adapted from https://github.com/microsoft/vscode/blob/main/extensions/theme-defaults/themes/light_plus.json
		if (useDarkTheme)
		{
#ifdef JSON
			for (uptr_t i{ SCE_JSON_DEFAULT }; i <= SCE_JSON_ERROR; i++)
#else
			for (uptr_t i{ SCE_C_DEFAULT }; i <= SCE_C_ESCAPESEQUENCE; i++) // Todo: see if there is better way
#endif
			{
				_scintilla->WndProc(Scintilla::Message::StyleSetFore, i, RGB(0xD4, 0xD4, 0xD4));
				_scintilla->WndProc(Scintilla::Message::StyleSetBack, i, transparencyColor);

				_scintilla->WndProc(Scintilla::Message::StyleSetFore, i + 64, RGB(167, 167, 167)); // Inactive states
				_scintilla->WndProc(Scintilla::Message::StyleSetBack, i + 64, transparencyColor);
			}

#ifdef JSON
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_JSON_PROPERTYNAME, RGB(0x9C, 0xDC, 0xFE));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_JSON_ESCAPESEQUENCE, RGB(0xD7, 0xBA, 0x7D));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_JSON_NUMBER, RGB(0xB5, 0xCE, 0xA8));
			//_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_WORD, RGB(0x56, 0x9C, 0xD6));
			//_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_WORD2, RGB(0xC5, 0x86, 0xC0));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_JSON_STRING, RGB(0xCE, 0x91, 0x78));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_JSON_STRINGEOL, RGB(0xCE, 0x91, 0x78));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_JSON_OPERATOR, RGB(0xD4, 0xD4, 0xD4));
			//_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_PREPROCESSOR, RGB(0x9B, 0x9B, 0x9B));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_JSON_ERROR, RGB(0xcd, 0x31, 0x31));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_JSON_BLOCKCOMMENT, RGB(0x6A, 0x99, 0x55));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_JSON_LINECOMMENT, RGB(0x6A, 0x99, 0x55));
#else
			//_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_IDENTIFIER, RGB(0x9C, 0xDC, 0xFE));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_ESCAPESEQUENCE, RGB(0xD7, 0xBA, 0x7D));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_NUMBER, RGB(0xB5, 0xCE, 0xA8));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_WORD, RGB(0x56, 0x9C, 0xD6));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_WORD2, RGB(0xC5, 0x86, 0xC0));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_STRING, RGB(0xCE, 0x91, 0x78));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_OPERATOR, RGB(0xD4, 0xD4, 0xD4));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_PREPROCESSOR, RGB(0x9B, 0x9B, 0x9B));
			//_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_ERROR, RGB(0xcd, 0x31, 0x31));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_COMMENT, RGB(0x6A, 0x99, 0x55));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_COMMENTLINE, RGB(0x6A, 0x99, 0x55));
#endif

			_scintilla->WndProc(Scintilla::Message::SetSelBack, true, RGB(0x26, 0x4F, 0x78));
			_scintilla->WndProc(Scintilla::Message::StyleSetBack, STYLE_DEFAULT, transparencyColor);
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, STYLE_LINENUMBER, RGB(0x85, 0x85, 0x85));
			_scintilla->WndProc(Scintilla::Message::StyleSetBack, STYLE_LINENUMBER, transparencyColor);
			_scintilla->WndProc(Scintilla::Message::SetCaretFore, RGB(0xAE, 0xAF, 0xAD), 0);
		}
		else
		{
#ifdef JSON
			for (uptr_t i{ SCE_JSON_DEFAULT }; i <= SCE_JSON_ERROR; i++)
#else
			for (uptr_t i{ SCE_C_DEFAULT }; i <= SCE_C_ESCAPESEQUENCE; i++) // Todo: see if there is better way
#endif
			{
				_scintilla->WndProc(Scintilla::Message::StyleSetFore, i, RGB(0x00, 0x00, 0x00));
				_scintilla->WndProc(Scintilla::Message::StyleSetBack, i, transparencyColor);

				_scintilla->WndProc(Scintilla::Message::StyleSetFore, i + 64, RGB(185, 185, 185)); // Inactive states
				_scintilla->WndProc(Scintilla::Message::StyleSetBack, i + 64, transparencyColor);
			}

#ifdef JSON
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_JSON_PROPERTYNAME, RGB(0x00, 0x10, 0x80));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_JSON_ESCAPESEQUENCE, RGB(0xEE, 0x00, 0x00));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_JSON_NUMBER, RGB(0x09, 0x86, 0x58));
			//_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_WORD, RGB(0x00, 0x00, 0xFF));
			//_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_WORD2, RGB(0xaf, 0x00, 0xdb));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_JSON_STRING, RGB(0xa3, 0x15, 0x15));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_JSON_STRINGEOL, RGB(0xa3, 0x15, 0x15));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_JSON_OPERATOR, RGB(0x00, 0x00, 0x00));
			//_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_PREPROCESSOR, RGB(0x80, 0x80, 0x80));
			//_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_ERROR, RGB(0xcd, 0x31, 0x31));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_JSON_BLOCKCOMMENT, RGB(0x00, 0x80, 0x00));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_JSON_LINECOMMENT, RGB(0x00, 0x80, 0x00));
#else
			//_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_IDENTIFIER, RGB(0x00, 0x10, 0x80));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_ESCAPESEQUENCE, RGB(0xEE, 0x00, 0x00));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_NUMBER, RGB(0x09, 0x86, 0x58));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_WORD, RGB(0x00, 0x00, 0xFF));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_WORD2, RGB(0xaf, 0x00, 0xdb));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_STRING, RGB(0xa3, 0x15, 0x15));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_OPERATOR, RGB(0x00, 0x00, 0x00));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_PREPROCESSOR, RGB(0x80, 0x80, 0x80));
			//_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_ERROR, RGB(0xcd, 0x31, 0x31));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_COMMENT, RGB(0x00, 0x80, 0x00));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_COMMENTLINE, RGB(0x00, 0x80, 0x00));
#endif

			_scintilla->WndProc(Scintilla::Message::SetSelBack, true, RGB(0xAD, 0xD6, 0xFF));
			_scintilla->WndProc(Scintilla::Message::StyleSetBack, STYLE_DEFAULT, transparencyColor);
			for (uptr_t i{ SCE_B_DEFAULT }; i < SCE_B_DOCKEYWORD; i++) // Todo: see if there is better way
			{
				_scintilla->WndProc(Scintilla::Message::StyleSetBack, i, transparencyColor);
			}
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, STYLE_LINENUMBER, RGB(0x23, 0x78, 0x93));
			_scintilla->WndProc(Scintilla::Message::StyleSetBack, STYLE_LINENUMBER, transparencyColor);
			_scintilla->WndProc(Scintilla::Message::SetCaretFore, RGB(0x00, 0x00, 0x00), 0);
		}
	}

	void MicaEditorControl::AddContextMenuItems(MenuFlyout const &menu)
	{
		menu.Opening([&](const auto &, const auto &)
			{
				_isContextMenuOpen = true;
			});
		menu.Closing([&](const auto &, const auto &)
			{
				_isContextMenuOpen = false;
			});

		const auto writable{ !static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetReadOnly, 0, 0)) };
		const auto selection{ !static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetSelectionEmpty, 0, 0)) };

		const MenuFlyoutItem undoItem{};
		undoItem.Text(L"Undo"); // Todo: Localize
		undoItem.Icon(SymbolIcon{ Symbol::Undo });
		undoItem.Tag(box_value(ScintillaMessage::Undo));
		undoItem.IsEnabled(_scintilla->WndProc(Scintilla::Message::CanUndo, 0, 0));
		undoItem.Click({ this, &MicaEditorControl::ContextMenuItem_Click }); // Todo: Revoke event handler?
		menu.Items().Append(undoItem);

		const MenuFlyoutItem redoItem{};
		redoItem.Text(L"Redo");
		redoItem.Icon(SymbolIcon{ Symbol::Redo });
		redoItem.Tag(box_value(ScintillaMessage::Redo));
		redoItem.IsEnabled(_scintilla->WndProc(Scintilla::Message::CanRedo, 0, 0));
		redoItem.Click({ this, &MicaEditorControl::ContextMenuItem_Click });
		menu.Items().Append(redoItem);

		menu.Items().Append(MenuFlyoutSeparator{});

		const MenuFlyoutItem cutItem{};
		cutItem.Text(L"Cut");
		cutItem.Icon(SymbolIcon{ Symbol::Cut });
		cutItem.Tag(box_value(ScintillaMessage::Cut));
		cutItem.IsEnabled(writable && selection);
		cutItem.Click({ this, &MicaEditorControl::ContextMenuItem_Click });
		menu.Items().Append(cutItem);

		const MenuFlyoutItem copyItem{};
		copyItem.Text(L"Copy");
		copyItem.Icon(SymbolIcon{ Symbol::Copy });
		copyItem.Tag(box_value(ScintillaMessage::Copy));
		copyItem.IsEnabled(selection);
		copyItem.Click({ this, &MicaEditorControl::ContextMenuItem_Click });
		menu.Items().Append(copyItem);

		const MenuFlyoutItem pasteItem{};
		pasteItem.Text(L"Paste");
		pasteItem.Icon(SymbolIcon{ Symbol::Paste });
		pasteItem.Tag(box_value(ScintillaMessage::Paste));
		pasteItem.IsEnabled(_scintilla->WndProc(Scintilla::Message::CanPaste, 0, 0));
		pasteItem.Click({ this, &MicaEditorControl::ContextMenuItem_Click });
		menu.Items().Append(pasteItem);

		const MenuFlyoutItem deleteItem{};
		deleteItem.Text(L"Delete");
		deleteItem.Icon(SymbolIcon{ Symbol::Delete });
		deleteItem.Tag(box_value(ScintillaMessage::Clear));
		deleteItem.IsEnabled(writable && selection);
		deleteItem.Click({ this, &MicaEditorControl::ContextMenuItem_Click });
		menu.Items().Append(deleteItem);

		menu.Items().Append(MenuFlyoutSeparator{});

		const MenuFlyoutItem selectAllItem{};
		selectAllItem.Text(L"Select all");
		selectAllItem.Icon(SymbolIcon{ Symbol::SelectAll });
		selectAllItem.Tag(box_value(ScintillaMessage::SelectAll));
		selectAllItem.Click({ this, &MicaEditorControl::ContextMenuItem_Click });
		menu.Items().Append(selectAllItem);
	}

	void MicaEditorControl::ShowContextMenuAtCurrentPosition()
	{
		if (auto imageTarget{ GetTemplateChild(L"ImageTarget").try_as<UIElement>() }) // Todo: Store this
		{
			const MenuFlyout menu{};
			AddContextMenuItems(menu);
			const auto pos{ _scintilla->WndProc(Scintilla::Message::GetCurrentPos, 0, 0) };
			menu.ShowAt(imageTarget, Point{
				_scintilla->WndProc(Scintilla::Message::PointXFromPosition, 0, pos) / _dpiScale,
				(_scintilla->WndProc(Scintilla::Message::PointYFromPosition, 0, pos) + _scintilla->WndProc(Scintilla::Message::TextHeight, 0, 0)) / _dpiScale
				});
		}
	}

	void MicaEditorControl::UpdateSizes()
	{
		_wrapper->DpiScale(_dpiScale);
		_wrapper->LogicalDpi(_logicalDpi);
		if (_scintilla)
		{
			ApplyScaleSettings();
		}
	}

	uint64_t MicaEditorControl::ScaleWParam(uint64_t wParam)
	{
		return static_cast<uint64_t>(floor(wParam * _dpiScale + 0.5f));
	}

	int64_t MicaEditorControl::ScaleLParam(int64_t lParam)
	{
		return static_cast<int64_t>(floor(lParam * _dpiScale + 0.5f));
	}

	void MicaEditorControl::ApplyScaleSettings()
	{
		for (const auto &scaledMessage : _scaleMessages)
		{
			const auto scaledMessageImpl{ scaledMessage.Value().as<ScaledMessage>() };
			_scintilla->WndProc(
				static_cast<Scintilla::Message>(scaledMessage.Key()),
				scaledMessageImpl->ScaleWParam() ? ScaleWParam(scaledMessageImpl->WParam()) : scaledMessageImpl->WParam(),
				scaledMessageImpl->ScaleLParam() ? ScaleLParam(scaledMessageImpl->LParam()) : scaledMessageImpl->LParam());
		}
	}

	Scintilla::sptr_t MicaEditorControl::PublicWndProc(Scintilla::Message iMessage, Scintilla::uptr_t wParam, Scintilla::sptr_t lParam)
	{
		if (_scaleMessages.HasKey(static_cast<ScintillaMessage>(iMessage)))
		{
			const auto scaledMessageImpl{ _scaleMessages.Lookup(static_cast<ScintillaMessage>(iMessage)).as<ScaledMessage>() };
			scaledMessageImpl->WParam(wParam);
			scaledMessageImpl->LParam(lParam);
			if (scaledMessageImpl->ScaleWParam())
			{
				wParam = ScaleWParam(wParam);
			}
			if (scaledMessageImpl->ScaleLParam())
			{
				lParam = ScaleLParam(lParam);
			}
		}
		return _scintilla->WndProc(iMessage, wParam, lParam);
	}

	uint64_t MicaEditorControl::Scintilla(ScintillaMessage const &message, uint64_t wParam, uint64_t lParam)
	{
		return PublicWndProc(static_cast<Scintilla::Message>(message), wParam, lParam);
	}

	IMap<ScintillaMessage, MicaEditor::ScaledMessage> MicaEditorControl::ScaleMessages()
	{
		return _scaleMessages;
	}

	void MicaEditorControl::OnTextPropertyChanged(IInspectable const &sender, DUX::DependencyPropertyChangedEventArgs const &args)
	{
		auto scintilla{ sender.as<MicaEditorControl>()->_scintilla };
		if (scintilla)
		{
			auto str{ args.NewValue().as<hstring>() };
			scintilla->SetText(str);
		}
	}

	void MicaEditorControl::OnApplyTemplate()
	{
#ifdef WINUI3
		// Temporary until it is known how to respond to DPI changes with WASDK (answer: XamlRoot.Changed)
		// Todo: Can the UWP version of this code go down here also?
		auto scale{ XamlRoot().RasterizationScale() };
		UpdateDisplayInformation(scale, scale * 96);
#endif
		UpdateSizes();

		auto width{ Helpers::ConvertFromDipToPixelUnit(ActualWidth(), _dpiScale) };
		auto height{ Helpers::ConvertFromDipToPixelUnit(ActualHeight(), _dpiScale) };
		_wrapper->Width(width); // Todo: Is this zero at this point?
		_wrapper->Height(height);
		VirtualSurfaceImageSource virtualSurfaceImageSource{ width, height };


		_vsisNative = virtualSurfaceImageSource.as<::IVirtualSurfaceImageSourceNative>();

		_wrapper->VsisNative(_vsisNative);
		_scintilla->RegisterGraphics(_wrapper);
		_vsisNative->RegisterForUpdatesNeeded(_scintilla.as<::IVirtualSurfaceUpdatesCallbackNative>().get());

		// The SurfaceImageSource object's underlying
		// ISurfaceImageSourceNativeWithD2D object will contain the completed bitmap.

		auto horizontalScrollBar{ GetTemplateChild(L"HorizontalScrollBar").try_as<ScrollBar>() };
		auto verticalScrollBar{ GetTemplateChild(L"VerticalScrollBar").try_as<ScrollBar>() };
		if (horizontalScrollBar)
		{
			horizontalScrollBar.Scroll({ this, &MicaEditorControl::HorizontalScrollBar_Scroll });
		}
		if (verticalScrollBar)
		{
			verticalScrollBar.Scroll({ this, &MicaEditorControl::VerticalScrollBar_Scroll });
		}
		_wrapper->SetScrollBars(horizontalScrollBar, verticalScrollBar);

		if (auto imageTarget{ GetTemplateChild(L"ImageTarget").try_as<Border>() })
		{
			// Todo: do these need auto revokers
			// Todo: is this safe to have in OnApplyTemplate?
			imageTarget.SizeChanged({ this, &MicaEditorControl::ImageTarget_SizeChanged });
			imageTarget.PointerWheelChanged({ this, &MicaEditorControl::ImageTarget_PointerWheelChanged });
			imageTarget.DragEnter({ this, &MicaEditorControl::ImageTarget_DragEnter });
			imageTarget.DragOver({ this, &MicaEditorControl::ImageTarget_DragOver });
			imageTarget.DragLeave({ this, &MicaEditorControl::ImageTarget_DragLeave });
			imageTarget.Drop({ this, &MicaEditorControl::ImageTarget_Drop });

			_wrapper->SetMouseCaptureElement(imageTarget);
			imageTarget.DragStarting({ this, &MicaEditorControl::ImageTarget_DragStarting });

			imageTarget.ContextRequested({ this, &MicaEditorControl::ImageTarget_ContextRequested });

			ImageBrush brush{};
			brush.ImageSource(virtualSurfaceImageSource);
			imageTarget.Background(brush);
		}

#ifndef WINUI3
		// Todo: Evaluate if this is an appropriate place to add this event (and other code in this method)
		_suspendingRevoker = Application::Current().Suspending(auto_revoke, { this, &MicaEditorControl::Application_Suspending });
#endif
	}

	// Todo: Focus bug: deactive window, click on control, press ctrl+a quickly. result: selection disappears

	void MicaEditorControl::OnGotFocus(RoutedEventArgs const &e)
	{
		__super::OnGotFocus(e);

		_isFocused = true;

		_scintilla->FocusChanged(true);
	}

	void MicaEditorControl::OnLostFocus(RoutedEventArgs const &e)
	{
		__super::OnLostFocus(e);

		_isFocused = false;

		if (!_isContextMenuOpen)
		{
			_scintilla->FocusChanged(false);
		}
	}

	// Todo: Combine pointer events and use PointerPointProperties.PointerUpdateKind to determine which button and state
	void MicaEditorControl::OnPointerPressed(PointerRoutedEventArgs const &e)
	{
		__super::OnPointerPressed(e);

		Focus(FocusState::Programmatic);

		if (auto imageTarget{ GetTemplateChild(L"ImageTarget").try_as<UIElement>() }) // Todo: Store this
		{
			auto point{ e.GetCurrentPoint(imageTarget) };
			auto scaled{ point.Position() };
			switch (point.Properties().PointerUpdateKind())
			{
			case DUI::PointerUpdateKind::LeftButtonPressed:
				_scintilla->PointerPressed(Point{ scaled.X * _dpiScale, scaled.Y * _dpiScale }, point.Timestamp() / 1000ul, e.KeyModifiers());
				break;
			case DUI::PointerUpdateKind::RightButtonPressed:
				_scintilla->RightPointerPressed(Point{ scaled.X * _dpiScale, scaled.Y * _dpiScale }, point.Timestamp() / 1000ul, e.KeyModifiers());
				break;
			}
			// Todo: make sure the loss of precision is not going to realistically cause a problem
		}
	}

	void MicaEditorControl::OnPointerMoved(PointerRoutedEventArgs const &e)
	{
		__super::OnPointerMoved(e);

		if (auto imageTarget{ GetTemplateChild(L"ImageTarget").try_as<UIElement>() }) // Todo: Store this
		{
			auto point{ e.GetCurrentPoint(imageTarget) };
			auto scaled{ point.Position() };
			auto x{ scaled.X * _dpiScale };
			auto y{ scaled.Y * _dpiScale };

			_scintilla->PointerMoved(Point{ x, y }, point.Timestamp() / 1000ul, e.KeyModifiers(), point);

			//auto cursor{ _scintilla->ContextCursor(Scintilla::Internal::Point{ x,y }) };
			//switch (cursor)
			//{
			//case Scintilla::Internal::Window::SetCursor invalid:
			//case text:
			//case arrow:
			//case up:
			//case wait:
			//case horizontal:
			//case vertical:
			//case reverseArrow:
			//case hand:
			//	break;
			//}
		}
	}

	void MicaEditorControl::OnPointerReleased(PointerRoutedEventArgs const &e)
	{
		__super::OnPointerReleased(e);

		if (auto imageTarget{ GetTemplateChild(L"ImageTarget").try_as<UIElement>() }) // Todo: Store this
		{
			auto point{ e.GetCurrentPoint(imageTarget) };
			auto scaled{ point.Position() };
			switch (point.Properties().PointerUpdateKind())
			{
			case DUI::PointerUpdateKind::LeftButtonReleased:
				_scintilla->PointerReleased(Point{ scaled.X * _dpiScale, scaled.Y * _dpiScale }, point.Timestamp() / 1000ul, e.KeyModifiers());
				break;
			}
		}

		e.Handled(true); // Prevents control from losing focus on pointer released
		// See https://stackoverflow.com/questions/59392044/uwp-control-to-keep-focus-after-mouse-click
		// Alternate approach: call Focus in OnFocusLost
	}

#ifndef WINUI3
	void MicaEditorControl::OnPointerCaptureLost(PointerRoutedEventArgs const &e)
	{
		if (!_isPointerOver)
		{
			// Todo: if you, e.g. hover over a HyperlinkButton when this is called, you will get an arrow instead of the hand you want
			winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread().PointerCursor(Windows::UI::Core::CoreCursor{ Windows::UI::Core::CoreCursorType::Arrow, 0 });
		}
	}

	void MicaEditorControl::OnPointerEntered(PointerRoutedEventArgs const &e)
	{
		_isPointerOver = true;
	}

	void MicaEditorControl::OnPointerExited(PointerRoutedEventArgs const &e)
	{
		_isPointerOver = false;
		if (!e.GetCurrentPoint(nullptr).Properties().IsLeftButtonPressed())
		{
			winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread().PointerCursor(Windows::UI::Core::CoreCursor{ Windows::UI::Core::CoreCursorType::Arrow, 0 });
		}
	}
#endif

	void MicaEditorControl::OnKeyDown(KeyRoutedEventArgs const &e)
	{
		__super::OnKeyDown(e);

		auto modifiers{ VirtualKeyModifiers::None };
#ifdef WINUI3
		// Todo: Do we need to check Locked?
		// Todo: Left vs right keys?
		//  does not help us
		if ((Microsoft::UI::Input::InputKeyboardSource::GetKeyStateForCurrentThread(VirtualKey::Shift) & Windows::UI::Core::CoreVirtualKeyStates::Down) == Windows::UI::Core::CoreVirtualKeyStates::Down)
		{
			modifiers |= VirtualKeyModifiers::Shift;
		}
		if ((Microsoft::UI::Input::InputKeyboardSource::GetKeyStateForCurrentThread(VirtualKey::Control) & Windows::UI::Core::CoreVirtualKeyStates::Down) == Windows::UI::Core::CoreVirtualKeyStates::Down)
		{
			modifiers |= VirtualKeyModifiers::Control;
		}
		if ((Microsoft::UI::Input::InputKeyboardSource::GetKeyStateForCurrentThread(VirtualKey::Menu) & Windows::UI::Core::CoreVirtualKeyStates::Down) == Windows::UI::Core::CoreVirtualKeyStates::Down)
		{
			modifiers |= VirtualKeyModifiers::Menu; // Todo: KeyStatus.IsMenuKeyDown?
		}
		if ((Microsoft::UI::Input::InputKeyboardSource::GetKeyStateForCurrentThread(VirtualKey::LeftWindows) & Windows::UI::Core::CoreVirtualKeyStates::Down) == Windows::UI::Core::CoreVirtualKeyStates::Down
			|| (Microsoft::UI::Input::InputKeyboardSource::GetKeyStateForCurrentThread(VirtualKey::RightWindows) & Windows::UI::Core::CoreVirtualKeyStates::Down) == Windows::UI::Core::CoreVirtualKeyStates::Down)
		{
			modifiers |= VirtualKeyModifiers::Windows;
		}
#else
		auto window{ Windows::UI::Core::CoreWindow::GetForCurrentThread() }; // Todo: is it worth it to store this?
		// Todo: Do we need to check Locked?
		// Todo: Left vs right keys?
		if ((window.GetKeyState(VirtualKey::Shift) & Windows::UI::Core::CoreVirtualKeyStates::Down) == Windows::UI::Core::CoreVirtualKeyStates::Down)
		{
			modifiers |= VirtualKeyModifiers::Shift;
		}
		if ((window.GetKeyState(VirtualKey::Control) & Windows::UI::Core::CoreVirtualKeyStates::Down) == Windows::UI::Core::CoreVirtualKeyStates::Down)
		{
			modifiers |= VirtualKeyModifiers::Control;
		}
		if ((window.GetKeyState(VirtualKey::Menu) & Windows::UI::Core::CoreVirtualKeyStates::Down) == Windows::UI::Core::CoreVirtualKeyStates::Down)
		{
			modifiers |= VirtualKeyModifiers::Menu; // Todo: KeyStatus.IsMenuKeyDown?
		}
		if ((window.GetKeyState(VirtualKey::LeftWindows) & Windows::UI::Core::CoreVirtualKeyStates::Down) == Windows::UI::Core::CoreVirtualKeyStates::Down
			|| (window.GetKeyState(VirtualKey::RightWindows) & Windows::UI::Core::CoreVirtualKeyStates::Down) == Windows::UI::Core::CoreVirtualKeyStates::Down)
		{
			modifiers |= VirtualKeyModifiers::Windows;
		}
#endif

		bool handled = true;
		_scintilla->KeyDown(e.Key(), modifiers, e.KeyStatus().IsExtendedKey, &handled); // Todo: Or use VirtualKey?
		e.Handled(handled);

		if (e.Key() == VirtualKey::F10 && (modifiers & VirtualKeyModifiers::Shift) == VirtualKeyModifiers::Shift)
		{
			ShowContextMenuAtCurrentPosition();
		}
	}

	void MicaEditorControl::OnKeyUp(KeyRoutedEventArgs const &e)
	{
		if (e.Key() == VirtualKey::Application)
		{
			ShowContextMenuAtCurrentPosition();
		}
	}

	void MicaEditorControl::MicaEditorControl_CharacterReceived(DUX::UIElement const &sender, CharacterReceivedRoutedEventArgs const &e)
	{
		if (_isFocused)
		{
			_scintilla->CharacterReceived(e.Character());
			e.Handled(true);
		}
	}

	void MicaEditorControl::ImageTarget_SizeChanged(IInspectable const &sender, SizeChangedEventArgs const &args)
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

	void MicaEditorControl::ImageTarget_PointerWheelChanged(IInspectable const &sender, PointerRoutedEventArgs const &e)
	{
		auto properties{ e.GetCurrentPoint(sender.as<UIElement>()).Properties() };
		_scintilla->PointerWheelChanged(properties.MouseWheelDelta(), properties.IsHorizontalMouseWheel(), e.KeyModifiers());
	}

	void MicaEditorControl::ImageTarget_DragEnter(IInspectable const &sender, DragEventArgs const &e)
	{
		DataPackageOperation op;
		_scintilla->DragEnter(e.DataView(), e.AllowedOperations(), e.Modifiers(), op);
		e.AcceptedOperation(op);
		e.DragUIOverride().IsContentVisible(false);
		e.DragUIOverride().IsCaptionVisible(false);
	}

	void MicaEditorControl::ImageTarget_DragOver(IInspectable const &sender, DragEventArgs const &e)
	{
		auto point{ e.GetPosition(sender.as<UIElement>()) };
		point.X *= _dpiScale;
		point.Y *= _dpiScale;
		DataPackageOperation op;
		_scintilla->DragOver(point, e.AllowedOperations(), e.Modifiers(), op);
		e.AcceptedOperation(op);
	}

	void MicaEditorControl::ImageTarget_DragLeave(IInspectable const &sender, DragEventArgs const &e)
	{
		_scintilla->DragLeave();
	}

	void MicaEditorControl::ImageTarget_Drop(IInspectable const &sender, DragEventArgs const &e)
	{
		auto point{ e.GetPosition(sender.as<UIElement>()) };
		point.X *= _dpiScale;
		point.Y *= _dpiScale;
		DataPackageOperation op;
		_scintilla->Drop(point, e.DataView(), e.AllowedOperations(), e.Modifiers(), op);
		e.AcceptedOperation(op);
	}

	void MicaEditorControl::ImageTarget_DragStarting(UIElement const &sender, DragStartingEventArgs const &e)
	{
		e.AllowedOperations(DataPackageOperation::Copy | DataPackageOperation::Move);
		e.Data().SetText(winrt::to_hstring(_scintilla->GetDragData()));
		e.DragUI().SetContentFromDataPackage();
	}

	void MicaEditorControl::ImageTarget_ContextRequested(UIElement const &sender, ContextRequestedEventArgs const &e)
	{
		const MenuFlyout menu{};
		AddContextMenuItems(menu);

		Point point;
		if (e.TryGetPosition(sender, point))
		{
			menu.ShowAt(sender, point);
		}
		else
		{
			menu.ShowAt(sender.as<FrameworkElement>());
		}
	}

	void MicaEditorControl::ContextMenuItem_Click(Windows::Foundation::IInspectable const &sender, DUX::RoutedEventArgs const &e)
	{
		_scintilla->WndProc(static_cast<Scintilla::Message>(unbox_value<ScintillaMessage>(sender.as<FrameworkElement>().Tag())), 0, 0);
	}

	void MicaEditorControl::HorizontalScrollBar_Scroll(IInspectable const &sender, ScrollEventArgs const &e)
	{
		_scintilla->HorizontalScroll(static_cast<Scintilla::Internal::ScrollEventType>(e.ScrollEventType()), static_cast<int>(e.NewValue()));
	}

	void MicaEditorControl::VerticalScrollBar_Scroll(IInspectable const &sender, ScrollEventArgs const &e)
	{
		_scintilla->Scroll(static_cast<Scintilla::Internal::ScrollEventType>(e.ScrollEventType()), static_cast<int>(e.NewValue()));
	}

	LRESULT MicaEditorControl::WndProc(IInspectable const &tag, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (msg == WM_NOTIFY)
		{
			const auto data{ reinterpret_cast<Scintilla::NotificationData *>(lParam) };
			const auto sender{ tag.as<MicaEditorControl>() };
			sender->_editorWrapper.as<implementation::Editor>()->ProcessEvent(data);
		}

		return 0;
	}

#ifndef WINUI3
	void MicaEditorControl::Application_Suspending(IInspectable const &sender, SuspendingEventArgs const &args)
	{
		// Required or crashes on resume
		// https://learn.microsoft.com/en-us/windows/uwp/gaming/directx-and-xaml-interop
		// https://learn.microsoft.com/en-us/windows/win32/api/dxgi1_3/nf-dxgi1_3-idxgidevice3-trim
		_scintilla->TrimGraphics();
	}
#endif
}
