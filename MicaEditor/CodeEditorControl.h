#pragma once

#include "CodeEditorControl.g.h"

#include "MicaEditorControl.h"
#include "CodeEditorControl.SciEdit.h"

namespace winrt::MicaEditor::implementation
{
	struct CodeEditorControl : CodeEditorControlT<CodeEditorControl>
	{
		CodeEditorControl();
		~CodeEditorControl()
		{
			to_hstring(L"");
		}
		uint64_t Scintilla(ScintillaMessage const &message, uint64_t wParam, uint64_t lParam);
		MicaEditor::Editor Editor();
		void OnApplyTemplate();
		void OnKeyDown(DUX::Input::KeyRoutedEventArgs const &e);

		hstring HighlightingLanguage();
		void HighlightingLanguage(hstring const &value);

		event_token DefaultColorsChanged(Windows::Foundation::EventHandler<DUX::ElementTheme> const &handler);
		void DefaultColorsChanged(event_token const &token) noexcept;

		event_token SyntaxHighlightingApplied(Windows::Foundation::EventHandler<DUX::ElementTheme> const &handler);
		void SyntaxHighlightingApplied(event_token const &token) noexcept;

	private:
		com_ptr<MicaEditorControl> _editor{ nullptr };
		constexpr Scintilla::ScintillaCall &Call() noexcept { return _editor->Call(); }

		hstring _highlightingLanguage;
		event<Windows::Foundation::EventHandler<DUX::ElementTheme>> _defaultColorsChangedEvent;
		event<Windows::Foundation::EventHandler<DUX::ElementTheme>> _syntaxHighlightingAppliedEvent;
		DUD::DispatcherQueue _dispatcherQueue{ nullptr };
		DUX::ElementTheme _theme{ DUX::ElementTheme::Default };
#ifndef WINUI3
		bool _hasFcu{ Windows::Foundation::Metadata::ApiInformation::IsApiContractPresent(L"Windows.Foundation.UniversalApiContract", 5) }; // Todo: Make static
		bool _has1803{ Windows::Foundation::Metadata::ApiInformation::IsApiContractPresent(L"Windows.Foundation.UniversalApiContract", 6) }; // Todo: Make static
		bool _hasIsLoaded{ Windows::Foundation::Metadata::ApiInformation::IsPropertyPresent(L"Windows.UI.Xaml.FrameworkElement", L"IsLoaded") }; // Todo: Make static
		bool _isLoaded{ false };
#endif
		double _dpiScale;
		int8_t _hasEmptySelection{ -1 };
		void OnLoaded(Windows::Foundation::IInspectable const &sender, DUX::RoutedEventArgs const &args);
		void OnUnloaded(Windows::Foundation::IInspectable const &sender, DUX::RoutedEventArgs const &args);
		bool IsLoadedCompat();
		void OnGettingFocus(Windows::Foundation::IInspectable const &sender, DUX::Input::GettingFocusEventArgs const &e);
		DUX::FrameworkElement::ActualThemeChanged_revoker _actualThemeChangedRevoker{};
		void OnActualThemeChanged(Windows::Foundation::IInspectable const &sender, Windows::Foundation::IInspectable const &e);
		void Editor_DpiChanged(Windows::Foundation::IInspectable const &sender, double value);
		void Editor_ScintillaNotification(Windows::Foundation::IInspectable const &sender, uint64_t value);
		void UpdateColors(DUX::ElementTheme theme);
		void UpdateStyles();
		void UpdateCaretLineBackColors(bool colorsUpdated = false);
		void UpdateBraceMatch();
		void AutoIndent(char ch);
		void UpdateZoom();
		void AddKeyboardShortcuts();
		void ChangeDefaults();
#ifndef WINUI3
		DUX::ElementTheme LegacyActualTheme();
#endif
		void ChangeAllOccurrences();
		std::string GetMainSelectedText(bool expandCaretToWord, Scintilla::sptr_t &start, Scintilla::sptr_t &end);
		void SetLexer();
		void UpdateLanguageStyles();
		void SetLanguageIndentMode(
			int indentKeywordStyle, const std::set<std::string> &indentKeywords,
			int lineEndStyle, const std::set<std::string> &lineEndWords,
			int blockStartStyle, const std::set<std::string> &blockStartWords,
			int blockEndStyle, const std::set<std::string> &blockEndWords
		);

		static constexpr bool _sciIndentOpening{ false };
		static constexpr bool _sciIndentClosing{ false };
		static constexpr int _sciStatementLookback{ 20 };
		StyleAndWords _sciStatementIndent;
		StyleAndWords _sciStatementEnd;
		StyleAndWords _sciBlockStart;
		StyleAndWords _sciBlockEnd;
		bool SciFindMatchingBracePosition(Scintilla::Position &braceAtCaret, Scintilla::Position &braceOpposite, bool sloppy);
		void SciBraceMatch();
		Scintilla::Line SciGetCurrentLineNumber();
		int SciIndentOfBlock(Scintilla::Line line);
		int SciGetLineIndentation(Scintilla::Line line);
		Scintilla::Position SciGetLineIndentPosition(Scintilla::Line line);
		void SciSetLineIndentation(Scintilla::Line line, int indent);
		Scintilla::Span SciGetSelection();
		std::vector<std::string> SciGetLinePartsInStyle(Scintilla::Line line, const StyleAndWords &saw);
		void SciSetSelection(Scintilla::Position anchor, Scintilla::Position currentPos);
		bool SciRangeIsAllWhitespace(Scintilla::Position start, Scintilla::Position end);
		IndentationStatus SciGetIndentState(Scintilla::Line line);
		void SciAutomaticIndentation(char ch);
	};
}

namespace winrt::MicaEditor::factory_implementation
{
	struct CodeEditorControl : CodeEditorControlT<CodeEditorControl, implementation::CodeEditorControl>
	{
	};
}
