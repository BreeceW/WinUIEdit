#pragma once

#include <string>
#include <memory>
#include <set>
#include <vector>
#include "ScintillaTypes.h"
#include "ScintillaStructures.h"
#include "ScintillaCall.h"
#include "ILexer.h"
#include "Lexilla.h"
#include "CodeEditorHandler.SciEdit.h"

namespace MicaEditor
{
	constexpr int IntRGBA(unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha = 0xff) noexcept
	{
		return red | (green << 8) | (blue << 16) | (alpha << 24);
	}

	enum class CodeEditorTheme
	{
		Unset,
		Light,
		Dark,
	};

	struct CodeEditorHandler
	{
		void SetScintilla(std::shared_ptr<Scintilla::ScintillaCall> call);
		void SetLexilla(Lexilla::CreateLexerFn createLexer);
		void Initialize();
		void UpdateColors(CodeEditorTheme theme);
		void UpdateDpi(double value);
		void ChangeAllOccurrences();
		void SetHighlightingLanguage(std::wstring_view language);
		void ProcessNotification(Scintilla::NotificationData *data);

	protected:
		virtual void StyleSetFore(int style, Scintilla::ColourAlpha color);
		virtual void StyleSetBack(int style, Scintilla::ColourAlpha color);
		virtual void InvalidateStyleRedraw();
		virtual void StyleClearCustom();

		virtual void DefaultColorsChanged(CodeEditorTheme theme);
		virtual void SyntaxHighlightingApplied(CodeEditorTheme theme);

	private:
		std::shared_ptr<Scintilla::ScintillaCall> _call{ nullptr };
		Lexilla::CreateLexerFn _createLexer{ nullptr };
		CodeEditorTheme _theme{ CodeEditorTheme::Unset };
		double _dpiScale;
		int8_t _hasEmptySelection{ -1 };
		std::wstring _highlightingLanguage;

		void UpdateStyles();
		void UpdateCaretLineBackColors(bool colorsUpdated = false);
		void UpdateBraceMatch();
		void AutoIndent(char ch);
		void UpdateZoom();
		void AddKeyboardShortcuts();
		void ChangeDefaults();
		std::string GetMainSelectedText(bool expandCaretToWord, Scintilla::sptr_t &start, Scintilla::sptr_t &end);
		void SetLanguageIndentMode(
			int indentKeywordStyle, const std::set<std::string> &indentKeywords,
			int lineEndStyle, const std::set<std::string> &lineEndWords,
			int blockStartStyle, const std::set<std::string> &blockStartWords,
			int blockEndStyle, const std::set<std::string> &blockEndWords
		);

		void SetLexer();
		void UpdateLanguageStyles();

		static constexpr bool _sciIndentOpening{ false };
		static constexpr bool _sciIndentClosing{ false };
		static constexpr int _sciStatementLookback{ 20 };
		Internal::StyleAndWords _sciStatementIndent;
		Internal::StyleAndWords _sciStatementEnd;
		Internal::StyleAndWords _sciBlockStart;
		Internal::StyleAndWords _sciBlockEnd;
		bool SciFindMatchingBracePosition(Scintilla::Position &braceAtCaret, Scintilla::Position &braceOpposite, bool sloppy);
		void SciBraceMatch();
		Scintilla::Line SciGetCurrentLineNumber();
		int SciIndentOfBlock(Scintilla::Line line);
		int SciGetLineIndentation(Scintilla::Line line);
		Scintilla::Position SciGetLineIndentPosition(Scintilla::Line line);
		void SciSetLineIndentation(Scintilla::Line line, int indent);
		Scintilla::Span SciGetSelection();
		std::vector<std::string> SciGetLinePartsInStyle(Scintilla::Line line, const Internal::StyleAndWords &saw);
		void SciSetSelection(Scintilla::Position anchor, Scintilla::Position currentPos);
		bool SciRangeIsAllWhitespace(Scintilla::Position start, Scintilla::Position end);
		Internal::IndentationStatus SciGetIndentState(Scintilla::Line line);
		void SciAutomaticIndentation(char ch);
	};
}
