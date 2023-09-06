#include "pch.h"
#include "CodeEditorControl.h"
#if __has_include("CodeEditorControl.g.cpp")
#include "CodeEditorControl.g.cpp"
#endif
#include "Helpers.h"

using namespace ::MicaEditor;
using namespace winrt;
using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace DUD;
using namespace DUX;
using namespace DUX::Controls;
using namespace DUX::Input;

using namespace winrt;

namespace winrt::MicaEditor::implementation
{
	CodeEditorControl::CodeEditorControl()
	{
		DefaultStyleKey(winrt::box_value(L"MicaEditor.CodeEditorControl"));

		_editor = make_self<MicaEditorControl>();
		AddKeyboardShortcuts();
		ChangeDefaults();
		_editor->DpiChanged({ this, &CodeEditorControl::Editor_DpiChanged });
		_editor->ScintillaNotification({ this, &CodeEditorControl::Editor_ScintillaNotification });

		Loaded({ this, &CodeEditorControl::OnLoaded });
		Unloaded({ this, &CodeEditorControl::OnUnloaded });
		GettingFocus({ this, &CodeEditorControl::OnGettingFocus });

#ifndef WINUI3
		if (_hasFcu)
		{
#endif
			_dispatcherQueue = DUD::DispatcherQueue::GetForCurrentThread();
#ifndef WINUI3
		}
#endif
	}

	void CodeEditorControl::OnApplyTemplate()
	{
		__super::OnApplyTemplate();

#ifndef WINUI3
		if (_hasFcu)
		{
#endif
			UpdateColors(ActualTheme());
#ifndef WINUI3
		}
		else
		{
			UpdateColors(LegacyActualTheme());
		}
#endif

		if (const auto presenter{ GetTemplateChild(L"EditorContainer").try_as<ContentPresenter>() })
		{
			presenter.Content(_editor.as<MicaEditor::MicaEditorControl>());
		}
	}

	void CodeEditorControl::OnKeyDown(KeyRoutedEventArgs const &e)
	{
		__super::OnKeyDown(e);

		const auto modifiers{ GetKeyModifiersForCurrentThread() }; // Todo: Can we avoid calling this every time?

		if ((modifiers & VirtualKeyModifiers::Control) == VirtualKeyModifiers::Control)
		{
			switch (e.Key())
			{
			case VirtualKey::F2: // Todo: make customizable
			{
				if (_editor->PublicWndProc(Scintilla::Message::GetFocus, 0, 0))
				{
					ChangeAllOccurrences();
					e.Handled(true);
				}
			}
			break;
			}
		}
	}

	void CodeEditorControl::OnLoaded(IInspectable const &sender, DUX::RoutedEventArgs const &args)
	{
#ifndef WINUI3
		_isLoaded = true;
#endif

		if (!IsLoadedCompat())
		{
			return;
		}

#ifndef WINUI3
		if (_hasFcu)
		{
#endif
			UpdateColors(ActualTheme());
			_actualThemeChangedRevoker = ActualThemeChanged(auto_revoke, { this, &CodeEditorControl::OnActualThemeChanged });
#ifndef WINUI3
		}
		else
		{
			UpdateColors(LegacyActualTheme());
			// Todo: Add fallback for ActualThemeChanged
		}
#endif
	}

	void CodeEditorControl::OnUnloaded(IInspectable const &sender, DUX::RoutedEventArgs const &args)
	{
#ifndef WINUI3
		_isLoaded = false;
#endif

		if (IsLoadedCompat())
		{
			return;
		}

#ifndef WINUI3
		if (_hasFcu)
		{
#endif
			_actualThemeChangedRevoker.revoke();
#ifndef WINUI3
		}
		else
		{
			// Todo: Add fallback
		}
#endif
	}

	bool CodeEditorControl::IsLoadedCompat()
	{
#ifndef WINUI3
		if (_hasIsLoaded)
		{
#endif
			return IsLoaded();
#ifndef WINUI3
		}
		else
		{
			return _isLoaded;
		}
#endif
	}

	void CodeEditorControl::OnGettingFocus(IInspectable const &sender, GettingFocusEventArgs const &e)
	{
		if (e.NewFocusedElement() == *this && (
#ifndef WINUI3
			!_has1803 ||
#endif
			!e.TrySetNewFocusedElement(*_editor)))
		{
			const auto focusState{ e.FocusState() };
#ifndef WINUI3
			if (_hasFcu)
			{
#endif
				_dispatcherQueue.TryEnqueue([this, focusState]()
					{
						_editor->Focus(focusState);
					});
#ifndef WINUI3
			}
			else
			{
#endif
				Dispatcher().RunAsync(CoreDispatcherPriority::Normal, [this, focusState]()
					{
						_editor->Focus(focusState);
					});
#ifndef WINUI3
			}
#endif
			e.Handled(true);
		}
	}

	uint64_t CodeEditorControl::Scintilla(ScintillaMessage const &message, uint64_t wParam, uint64_t lParam)
	{
		return _editor->PublicWndProc(static_cast<Scintilla::Message>(message), wParam, lParam);
	}

	MicaEditor::Editor CodeEditorControl::Editor()
	{
		return _editor->Editor();
	}

	void CodeEditorControl::OnActualThemeChanged(IInspectable const &sender, IInspectable const &e)
	{
		UpdateColors(ActualTheme());
	}

	void CodeEditorControl::Editor_DpiChanged(IInspectable const &sender, double value)
	{
		_dpiScale = value;
		_editor->PublicWndProc(Scintilla::Message::SetXCaretPolicy, CARET_SLOP | CARET_STRICT | CARET_EVEN, ConvertFromDipToPixelUnit(24, value));
		UpdateZoom();
	}

	void CodeEditorControl::Editor_ScintillaNotification(IInspectable const &sender, uint64_t value)
	{
		const auto data{ reinterpret_cast<Scintilla::NotificationData *>(value) };

		if (data->nmhdr.code == Scintilla::Notification::Zoom
			|| (data->nmhdr.code == Scintilla::Notification::Modified && data->linesAdded))
		{
			UpdateZoom();
		}

		if ((data->nmhdr.code == Scintilla::Notification::UpdateUI && Scintilla::FlagSet(data->updated, Scintilla::Update::Selection))
			|| (data->nmhdr.code == Scintilla::Notification::Modified && Scintilla::FlagSet(data->modificationType, Scintilla::ModificationFlags::InsertText | Scintilla::ModificationFlags::DeleteText)))
		{
			UpdateCaretLineBackColors();
		}
	}

	void CodeEditorControl::UpdateColors(DUX::ElementTheme theme)
	{
		// Todo: Support high contrast mode

		if (_theme != theme)
		{
			_theme = theme;

			switch (theme)
			{
			case ElementTheme::Dark:
				_editor->PublicWndProc(Scintilla::Message::StyleSetFore, 0, Scintilla::Internal::ColourRGBA{ 255, 255, 255 }.AsInteger());
				_editor->PublicWndProc(Scintilla::Message::StyleSetFore, STYLE_LINENUMBER, RGB(0x85, 0x85, 0x85));
				_editor->PublicWndProc(Scintilla::Message::SetCaretFore, RGB(0xAE, 0xAF, 0xAD), 0);
				_editor->PublicWndProc(Scintilla::Message::SetElementColour, SC_ELEMENT_SELECTION_BACK, Scintilla::Internal::ColourRGBA{ 0x26, 0x4F, 0x78 }.AsInteger());
				_editor->PublicWndProc(Scintilla::Message::SetElementColour, SC_ELEMENT_SELECTION_ADDITIONAL_BACK, Scintilla::Internal::ColourRGBA{ 0x26, 0x4F, 0x78 }.AsInteger());
				_editor->PublicWndProc(Scintilla::Message::SetElementColour, SC_ELEMENT_SELECTION_INACTIVE_BACK, Scintilla::Internal::ColourRGBA{ 0x3A, 0x3D, 0x41 }.AsInteger());
				break;

			case ElementTheme::Light:
				_editor->PublicWndProc(Scintilla::Message::StyleSetFore, 0, Scintilla::Internal::ColourRGBA{ 0, 0, 0 }.AsInteger());
				_editor->PublicWndProc(Scintilla::Message::StyleSetFore, STYLE_LINENUMBER, RGB(0x23, 0x78, 0x93));
				_editor->PublicWndProc(Scintilla::Message::SetCaretFore, RGB(0x00, 0x00, 0x00), 0);
				_editor->PublicWndProc(Scintilla::Message::SetElementColour, SC_ELEMENT_SELECTION_BACK, Scintilla::Internal::ColourRGBA{ 0xAD, 0xD6, 0xFF }.AsInteger());
				_editor->PublicWndProc(Scintilla::Message::SetElementColour, SC_ELEMENT_SELECTION_ADDITIONAL_BACK, Scintilla::Internal::ColourRGBA{ 0xAD, 0xD6, 0xFF }.AsInteger());
				_editor->PublicWndProc(Scintilla::Message::SetElementColour, SC_ELEMENT_SELECTION_INACTIVE_BACK, Scintilla::Internal::ColourRGBA{ 0xE5, 0xEB, 0xF1 }.AsInteger());
				break;
			}

			UpdateCaretLineBackColors(true);
		}
	}

	void CodeEditorControl::UpdateCaretLineBackColors(bool colorsUpdated)
	{
		const auto hasEmptySelection = _editor->PublicWndProc(Scintilla::Message::GetSelectionEmpty, 0, 0);

		if ((_hasEmptySelection != hasEmptySelection) || (hasEmptySelection && colorsUpdated))
		{
			_hasEmptySelection = hasEmptySelection;

			if (hasEmptySelection)
			{
				switch (_theme)
				{
				case ElementTheme::Dark:
					_editor->PublicWndProc(Scintilla::Message::SetElementColour, SC_ELEMENT_CARET_LINE_BACK, Scintilla::Internal::ColourRGBA{ 0xFF, 0xFF, 0xFF, 16 }.AsInteger());
					break;

				case ElementTheme::Light:
					_editor->PublicWndProc(Scintilla::Message::SetElementColour, SC_ELEMENT_CARET_LINE_BACK, Scintilla::Internal::ColourRGBA{ 0x00, 0x00, 0x00, 12 }.AsInteger());
					break;
				}
			}
			else
			{
				_editor->PublicWndProc(Scintilla::Message::ResetElementColour, SC_ELEMENT_CARET_LINE_BACK, 0);
			}
		}
	}

	void CodeEditorControl::UpdateZoom()
	{
		const auto size{ _editor->PublicWndProc(Scintilla::Message::StyleGetSizeFractional, static_cast<Scintilla::uptr_t>(Scintilla::StylesCommon::Default), 0) };
		const auto zoom{ static_cast<int>(_editor->PublicWndProc(Scintilla::Message::GetZoom, 0, 0)) };
		const auto factor{ static_cast<float>((size + zoom * 100)) / size };
		// Todo: Width 11 hard coded for a digit. Use below for real value:
		// _editor->PublicWndProc(Scintilla::Message::TextWidth, static_cast<Scintilla::uptr_t>(Scintilla::StylesCommon::LineNumber), reinterpret_cast<Scintilla::sptr_t>("9"))
		const auto line{ _editor->PublicWndProc(Scintilla::Message::GetLineCount, 0, 0) };
		const auto width{ 12 + 11 * std::max(3, static_cast<int>(std::floor(std::log10(line) + 1))) };
		_editor->PublicWndProc(Scintilla::Message::SetMarginWidthN, 0, ConvertFromDipToPixelUnit(std::floorf(factor * width + 0.5f), _dpiScale));
		_editor->PublicWndProc(Scintilla::Message::SetMarginLeft, 0, ConvertFromDipToPixelUnit(std::floorf(factor * 23 + 0.5f), _dpiScale));
		// Todo: Set caret width to be at least the UISettings system caret width
		const auto caretWidth{ std::max(1.0f, std::floorf(factor * 2 * _dpiScale)) };
		_editor->PublicWndProc(Scintilla::Message::SetCaretWidth, caretWidth, 0); // Todo: Needs to stop blinking after timeout and respect blink rate
		_editor->PublicWndProc(Scintilla::Message::SetCaretLineFrame, caretWidth, 0);
		_editor->PublicWndProc(Scintilla::Message::SetExtraDescent, std::floorf(factor * 1.8 * _dpiScale), 0);
	}

	void CodeEditorControl::AddKeyboardShortcuts()
	{
		_editor->PublicWndProc(Scintilla::Message::AssignCmdKey, 187 + (SCMOD_CTRL << 16), SCI_ZOOMIN); // Ctrl+Plus
		_editor->PublicWndProc(Scintilla::Message::AssignCmdKey, 189 + (SCMOD_CTRL << 16), SCI_ZOOMOUT); // Ctrl+Minus
		_editor->PublicWndProc(Scintilla::Message::AssignCmdKey, 48 + (SCMOD_CTRL << 16), SCI_SETZOOM); // Ctrl+0
	}

	void CodeEditorControl::ChangeDefaults()
	{
		_editor->PublicWndProc(Scintilla::Message::SetMultipleSelection, true, 0);
		_editor->PublicWndProc(Scintilla::Message::SetScrollWidth, 2000, 0);
		_editor->PublicWndProc(Scintilla::Message::SetScrollWidthTracking, true, 0);
		_editor->PublicWndProc(Scintilla::Message::SetYCaretPolicy, CARET_SLOP | CARET_STRICT | CARET_EVEN, 1);
		_editor->PublicWndProc(Scintilla::Message::SetVisiblePolicy, VISIBLE_SLOP, 0);
		_editor->PublicWndProc(Scintilla::Message::SetHScrollBar, true, 0);
		_editor->PublicWndProc(Scintilla::Message::SetEndAtLastLine, false, 0);
		_editor->PublicWndProc(Scintilla::Message::SetTabWidth, 4, 0);
		_editor->PublicWndProc(Scintilla::Message::SetMarginWidthN, 1, 0);
		_editor->PublicWndProc(Scintilla::Message::StyleSetFont, STYLE_DEFAULT, reinterpret_cast<Scintilla::sptr_t>("Cascadia Code"));
		_editor->PublicWndProc(Scintilla::Message::StyleSetSizeFractional, STYLE_DEFAULT, 11 * SC_FONT_SIZE_MULTIPLIER);
		_editor->PublicWndProc(Scintilla::Message::SetAdditionalSelectionTyping, 1, 0);
		_editor->PublicWndProc(Scintilla::Message::SetMultiPaste, SC_MULTIPASTE_EACH, 0);
		_editor->PublicWndProc(Scintilla::Message::SetLayoutThreads, 16, 0); // Todo: Determine performance impact
		_editor->PublicWndProc(Scintilla::Message::SetCaretLineVisibleAlways, true, 0);
		_editor->PublicWndProc(Scintilla::Message::SetCaretLineLayer, SC_LAYER_UNDER_TEXT, 0);
		_editor->PublicWndProc(Scintilla::Message::SetCaretLineHighlightSubLine, true, 0);

		_editor->StyleSetBackTransparent(0, Scintilla::Internal::ColourRGBA{});
		_editor->StyleSetBackTransparent(STYLE_DEFAULT, Scintilla::Internal::ColourRGBA{});
		_editor->StyleSetBackTransparent(STYLE_LINENUMBER, Scintilla::Internal::ColourRGBA{});
	}

#ifndef WINUI3
	DUX::ElementTheme CodeEditorControl::LegacyActualTheme()
	{
		// Todo: Fully implement
		const auto requestedTheme{ RequestedTheme() };
		if (requestedTheme == ElementTheme::Default)
		{
			return Application::Current().RequestedTheme() == ApplicationTheme::Dark
				? ElementTheme::Dark
				: ElementTheme::Light;
		}
		else
		{
			return requestedTheme;
		}
	}
#endif

	// Todo: This code needs to integrate with find and replace
	// Todo: This code seems to struggle with rectangular selections and/or multiple carets already existing and/or multiple selections already existing
	void CodeEditorControl::ChangeAllOccurrences()
	{
		Scintilla::sptr_t start;
		Scintilla::sptr_t end;
		const auto s{ GetMainSelectedText(true, start, end) };
		if (s.length() == 0)
		{
			return;
		}
		const auto length{ end - start };

		// Todo: Why are we setting these? Are these the best methods to use here if so?
		_editor->PublicWndProc(Scintilla::Message::SetSelectionStart, start, 0);
		_editor->PublicWndProc(Scintilla::Message::SetSelectionEnd, end, 0);

		_editor->PublicWndProc(Scintilla::Message::TargetWholeDocument, 0, 0);
		_editor->PublicWndProc(Scintilla::Message::SetSearchFlags, static_cast<Scintilla::uptr_t>(Scintilla::FindOption::MatchCase), 0);

		const auto mainSelection{ _editor->PublicWndProc(Scintilla::Message::GetMainSelection, 0, 0) };
		const auto bodyLength{ _editor->PublicWndProc(Scintilla::Message::GetLength, 0, 0) };

		while (true)
		{
			const auto match{ _editor->PublicWndProc(Scintilla::Message::SearchInTarget, length, reinterpret_cast<sptr_t>(&s[0])) };

			if (match == -1)
			{
				break;
			}

			const auto targetEnd{ _editor->PublicWndProc(Scintilla::Message::GetTargetEnd, 0, 0) };

			if (match != start)
			{
				// Todo: Add maximum number of carets and notify user if exceeded (VS Code allows 10,000)
				// Todo: This method calls a lot of redraws in a loop. You might need to use the lower level API to avoid that
				_editor->PublicWndProc(Scintilla::Message::AddSelection, match + length, match);
			}

			_editor->PublicWndProc(Scintilla::Message::SetTargetStart, targetEnd, 0);
			_editor->PublicWndProc(Scintilla::Message::SetTargetEnd, bodyLength, 0);
		}

		_editor->PublicWndProc(Scintilla::Message::SetMainSelection, mainSelection, 0);
	}

	std::string CodeEditorControl::GetMainSelectedText(bool expandCaretToWord, Scintilla::sptr_t &start, Scintilla::sptr_t &end)
	{
		// Todo: This code may be problematic for rectangular selections
		start = _editor->PublicWndProc(Scintilla::Message::GetSelectionStart, 0, 0);
		end = _editor->PublicWndProc(Scintilla::Message::GetSelectionEnd, 0, 0);
		if (expandCaretToWord && start == end)
		{
			start = _editor->PublicWndProc(Scintilla::Message::WordStartPosition, start, true);
			end = _editor->PublicWndProc(Scintilla::Message::WordEndPosition, start, true);

			if (start == end)
			{
				return "";
			}
		}

		const auto length{ end - start };
		std::string s(length, '\0');
		const Sci_TextRangeFull range
		{
			{ start, end, },
			&s[0],
		};
		_editor->PublicWndProc(Scintilla::Message::GetTextRangeFull, 0, reinterpret_cast<sptr_t>(&range));

		return s;
	}
}
