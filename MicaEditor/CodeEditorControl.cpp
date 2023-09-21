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
using namespace Windows::System;
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
				if (Call().Focus())
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
		Call().SetXCaretPolicy(Scintilla::CaretPolicy::Slop | Scintilla::CaretPolicy::Strict | Scintilla::CaretPolicy::Even, ConvertFromDipToPixelUnit(24, value));
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

		if (data->nmhdr.code == Scintilla::Notification::UpdateUI && Scintilla::FlagSet(data->updated, Scintilla::Update::Content | Scintilla::Update::Selection))
		{
			UpdateCaretLineBackColors();
			if (Call().Focus())
			{
				UpdateBraceMatch();
			}
		}

		if (data->nmhdr.code == Scintilla::Notification::FocusOut)
		{
			Call().BraceBadLight(-1);
		}
		else if (data->nmhdr.code == Scintilla::Notification::FocusIn)
		{
			UpdateBraceMatch();
		}

		if (data->nmhdr.code == Scintilla::Notification::CharAdded && data->ch <= 0xFF)
		{
			const auto sel{ Call().SelectionSpan() };

			if (sel.start == sel.end && sel.start > 0
				&& !Call().CallTipActive()
				&& !Call().AutoCActive())
			{
				AutoIndent(data->ch);
			}
		}
	}

	hstring CodeEditorControl::HighlightingLanguage()
	{
		return _highlightingLanguage;
	}

	void CodeEditorControl::HighlightingLanguage(hstring const &value)
	{
		if (_highlightingLanguage == value)
		{
			return;
		}

		_highlightingLanguage = value;

		Call().ClearDocumentStyle();

		SetLexer();

		UpdateStyles();
	}

	event_token CodeEditorControl::DefaultColorsChanged(EventHandler<ElementTheme> const &handler)
	{
		return _defaultColorsChangedEvent.add(handler);
	}

	void CodeEditorControl::DefaultColorsChanged(event_token const &token) noexcept
	{
		_defaultColorsChangedEvent.remove(token);
	}

	event_token CodeEditorControl::SyntaxHighlightingApplied(EventHandler<ElementTheme> const &handler)
	{
		return _syntaxHighlightingAppliedEvent.add(handler);
	}

	void CodeEditorControl::SyntaxHighlightingApplied(event_token const &token) noexcept
	{
		_syntaxHighlightingAppliedEvent.remove(token);
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
				Call().SetElementColour(Scintilla::Element::Caret, Scintilla::Internal::ColourRGBA{ 0xAE, 0xAF, 0xAD }.AsInteger());
				Call().SetElementColour(Scintilla::Element::SelectionBack, Scintilla::Internal::ColourRGBA{ 0x26, 0x4F, 0x78 }.AsInteger());
				Call().SetElementColour(Scintilla::Element::SelectionAdditionalText, Scintilla::Internal::ColourRGBA{ 0x26, 0x4F, 0x78 }.AsInteger());
				Call().SetElementColour(Scintilla::Element::SelectionInactiveBack, Scintilla::Internal::ColourRGBA{ 0x3A, 0x3D, 0x41 }.AsInteger());
				break;

			case ElementTheme::Light:
				Call().SetElementColour(Scintilla::Element::Caret, Scintilla::Internal::ColourRGBA{ 0x00, 0x00, 0x00 }.AsInteger());
				Call().SetElementColour(Scintilla::Element::SelectionBack, Scintilla::Internal::ColourRGBA{ 0xAD, 0xD6, 0xFF }.AsInteger());
				Call().SetElementColour(Scintilla::Element::SelectionAdditionalBack, Scintilla::Internal::ColourRGBA{ 0xAD, 0xD6, 0xFF }.AsInteger());
				Call().SetElementColour(Scintilla::Element::SelectionInactiveBack, Scintilla::Internal::ColourRGBA{ 0xE5, 0xEB, 0xF1 }.AsInteger());
				break;
			}

			UpdateCaretLineBackColors(true);

			UpdateStyles();
		}
	}

	void CodeEditorControl::UpdateStyles()
	{
		switch (_theme)
		{
		case ElementTheme::Dark:
			_editor->StyleSetForeTransparent(STYLE_LINENUMBER, Scintilla::Internal::ColourRGBA{ 0x85, 0x85, 0x85 });
			_editor->StyleSetBackTransparent(STYLE_LINENUMBER, Scintilla::Internal::ColourRGBA{});

			_editor->StyleSetForeTransparent(STYLE_DEFAULT, Scintilla::Internal::ColourRGBA{ 0xFF, 0xFF, 0xFF });
			_editor->StyleSetBackTransparent(STYLE_DEFAULT, Scintilla::Internal::ColourRGBA{});

			_editor->StyleSetForeTransparent(STYLE_BRACELIGHT, Scintilla::Internal::ColourRGBA{ 0xFF, 0xFF, 0xFF });
			_editor->StyleSetBackTransparent(STYLE_BRACELIGHT, Scintilla::Internal::ColourRGBA{ 0x11, 0x3D, 0x6F });
			_editor->StyleSetForeTransparent(STYLE_BRACEBAD, Scintilla::Internal::ColourRGBA{ 0xcd, 0x31, 0x31 });
			_editor->StyleSetBackTransparent(STYLE_BRACEBAD, Scintilla::Internal::ColourRGBA{});

			_editor->StyleSetForeTransparent(STYLE_INDENTGUIDE, Scintilla::Internal::ColourRGBA{ 0xFF, 0xFF, 0xFF, 48 });
			_editor->StyleSetBackTransparent(STYLE_INDENTGUIDE, Scintilla::Internal::ColourRGBA{});
			break;

		case ElementTheme::Light:
			_editor->StyleSetBackTransparent(STYLE_LINENUMBER, Scintilla::Internal::ColourRGBA{});
			_editor->StyleSetForeTransparent(STYLE_LINENUMBER, Scintilla::Internal::ColourRGBA{ 0x23, 0x78, 0x93 });

			_editor->StyleSetForeTransparent(STYLE_DEFAULT, Scintilla::Internal::ColourRGBA{ 0x00, 0x00, 0x00 });
			_editor->StyleSetBackTransparent(STYLE_DEFAULT, Scintilla::Internal::ColourRGBA{});

			_editor->StyleSetForeTransparent(STYLE_BRACELIGHT, Scintilla::Internal::ColourRGBA{ 0x00, 0x00, 0x00 });
			_editor->StyleSetBackTransparent(STYLE_BRACELIGHT, Scintilla::Internal::ColourRGBA{ 0xE2, 0xE6, 0xD6 });
			_editor->StyleSetForeTransparent(STYLE_BRACEBAD, Scintilla::Internal::ColourRGBA{ 0xcd, 0x31, 0x31 });
			_editor->StyleSetBackTransparent(STYLE_BRACEBAD, Scintilla::Internal::ColourRGBA{});

			_editor->StyleSetForeTransparent(STYLE_INDENTGUIDE, Scintilla::Internal::ColourRGBA{ 0x00, 0x00, 0x00, 64 });
			_editor->StyleSetBackTransparent(STYLE_INDENTGUIDE, Scintilla::Internal::ColourRGBA{});
			break;
		}

		_defaultColorsChangedEvent(*this, _theme);

		UpdateLanguageStyles();

		_syntaxHighlightingAppliedEvent(*this, _theme);

		_editor->InvalidateStyleRedraw();
	}

	void CodeEditorControl::UpdateCaretLineBackColors(bool colorsUpdated)
	{
		const auto hasEmptySelection = Call().SelectionEmpty();

		if ((_hasEmptySelection != hasEmptySelection) || (hasEmptySelection && colorsUpdated))
		{
			_hasEmptySelection = hasEmptySelection;

			if (hasEmptySelection)
			{
				switch (_theme)
				{
				case ElementTheme::Dark:
					Call().SetElementColour(Scintilla::Element::CaretLineBack, Scintilla::Internal::ColourRGBA{ 0xFF, 0xFF, 0xFF, 16 }.AsInteger());
					break;

				case ElementTheme::Light:
					Call().SetElementColour(Scintilla::Element::CaretLineBack, Scintilla::Internal::ColourRGBA{ 0x00, 0x00, 0x00, 12 }.AsInteger());
					break;
				}
			}
			else
			{
				Call().ResetElementColour(Scintilla::Element::CaretLineBack);
			}
		}
	}

	void CodeEditorControl::UpdateBraceMatch()
	{
		SciBraceMatch();
	}

	void CodeEditorControl::AutoIndent(char ch)
	{
		SciAutomaticIndentation(ch);
	}

	void CodeEditorControl::SetLanguageIndentMode(
		int indentKeywordStyle, const std::set<std::string> &indentKeywords,
		int lineEndStyle, const std::set<std::string> &lineEndWords,
		int blockStartStyle, const std::set<std::string> &blockStartWords,
		int blockEndStyle, const std::set<std::string> &blockEndWords)
	{
		_sciStatementIndent = { indentKeywordStyle, { indentKeywords, } }; // Todo: customize. also note using WORD2 for these
		_sciStatementEnd = { lineEndStyle, { lineEndWords, } };
		_sciBlockStart = { blockStartStyle, { blockStartWords, } };
		_sciBlockEnd = { blockEndStyle, { blockEndWords, } };
	}

	void CodeEditorControl::UpdateZoom()
	{
		const auto size{ Call().StyleGetSizeFractional(STYLE_DEFAULT) };
		const auto zoom{ static_cast<int>(Call().Zoom()) };
		const auto factor{ static_cast<float>((size + zoom * 100)) / size };
		// Todo: Width 11 hard coded for a digit. Use below for real value:
		// Call().TextWidth, static_cast<Scintilla::uptr_t>(Scintilla::StylesCommon::LineNumber), reinterpret_cast<Scintilla::sptr_t>("9"))
		const auto line{ Call().LineCount() };
		const auto width{ 12 + 11 * std::max(3, static_cast<int>(std::floor(std::log10(line) + 1))) };
		Call().SetMarginWidthN(0, ConvertFromDipToPixelUnit(std::floorf(factor * width + 0.5f), _dpiScale));
		Call().SetMarginLeft(ConvertFromDipToPixelUnit(std::floorf(factor * 23 + 0.5f), _dpiScale));
		// Todo: Set caret width to be at least the UISettings system caret width
		const auto caretWidth{ std::max(1.0f, std::floorf(factor * 2 * _dpiScale)) };
		Call().SetCaretWidth(caretWidth); // Todo: Needs to stop blinking after timeout and respect blink rate
		Call().SetCaretLineFrame(caretWidth);
		Call().SetExtraDescent(std::floorf(factor * 1.8 * _dpiScale));
	}

	void CodeEditorControl::AddKeyboardShortcuts()
	{
		Call().AssignCmdKey(187 + (SCMOD_CTRL << 16), SCI_ZOOMIN); // Ctrl+Plus
		Call().AssignCmdKey(189 + (SCMOD_CTRL << 16), SCI_ZOOMOUT); // Ctrl+Minus
		Call().AssignCmdKey(48 + (SCMOD_CTRL << 16), SCI_SETZOOM); // Ctrl+0
	}

	void CodeEditorControl::ChangeDefaults()
	{
		Call().SetMultipleSelection(true);
		Call().SetScrollWidth(2000);
		Call().SetScrollWidthTracking(true);
		Call().SetYCaretPolicy(Scintilla::CaretPolicy::Slop | Scintilla::CaretPolicy::Strict | Scintilla::CaretPolicy::Even, 1);
		Call().SetVisiblePolicy(Scintilla::VisiblePolicy::Slop, 0);
		Call().SetHScrollBar(true);
		Call().SetEndAtLastLine(false);
		Call().SetTabWidth(4);
		Call().SetIndent(4); // Brace matching and autoindent relies on this
		Call().SetMarginWidthN(1, 0);
		Call().StyleSetFont(STYLE_DEFAULT, "Cascadia Code");
		Call().StyleSetSizeFractional(STYLE_DEFAULT, 11 * SC_FONT_SIZE_MULTIPLIER);
		Call().SetAdditionalSelectionTyping(true);
		Call().SetMultiPaste(Scintilla::MultiPaste::Each);
		Call().SetLayoutThreads(16); // Todo: Determine performance impact
		Call().SetCaretLineVisibleAlways(true);
		Call().SetCaretLineLayer(Scintilla::Layer::UnderText);
		Call().SetCaretLineHighlightSubLine(true);
		Call().SetIndentationGuides(Scintilla::IndentView::LookBoth);
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
		Call().SetSelectionStart(start);
		Call().SetSelectionEnd(end);

		Call().TargetWholeDocument();
		Call().SetSearchFlags(Scintilla::FindOption::MatchCase);

		const auto mainSelection{ Call().MainSelection() };
		const auto bodyLength{ Call().Length() };

		while (true)
		{
			const auto match{ Call().SearchInTarget(length, &s[0]) };

			if (match == -1)
			{
				break;
			}

			const auto targetEnd{ Call().TargetEnd() };

			if (match != start)
			{
				// Todo: Add maximum number of carets and notify user if exceeded (VS Code allows 10,000)
				// Todo: This method calls a lot of redraws in a loop. You might need to use the lower level API to avoid that
				Call().AddSelection(match + length, match);
			}

			Call().SetTargetStart(targetEnd);
			Call().SetTargetEnd(bodyLength);
		}

		Call().SetMainSelection(mainSelection);
	}

	std::string CodeEditorControl::GetMainSelectedText(bool expandCaretToWord, Scintilla::sptr_t &start, Scintilla::sptr_t &end)
	{
		// Todo: This code may be problematic for rectangular selections
		start = Call().SelectionStart();
		end = Call().SelectionEnd();
		if (expandCaretToWord && start == end)
		{
			start = Call().WordStartPosition(start, true);
			end = Call().WordEndPosition(start, true);

			if (start == end)
			{
				return "";
			}
		}

		const auto length{ end - start };
		std::string s(length, '\0');
		Scintilla::TextRangeFull range
		{
			{ start, end, },
			&s[0],
		};
		Call().GetTextRangeFull(&range);

		return s;
	}
}
