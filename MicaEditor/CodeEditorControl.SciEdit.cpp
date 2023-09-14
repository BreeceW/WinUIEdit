// This file includes code derived from SciTEBase.cxx
// Copyright 1998-2011 by Neil Hodgson <neilh@scintilla.org>
// The scintilla\License.txt file describes the conditions under which this software may be distributed.

// Notable changes:
// Modifications to StyleAndWords:
// - Added constructor that is not string-based
// - Changed functions (IsSingleChar, IsCharacter, Includes) to support multiple characters for indent blocks

#include "pch.h"
#include "CodeEditorControl.h"

using namespace winrt;
using namespace DUX;
using namespace Scintilla;
using namespace Scintilla::Internal;

namespace winrt::MicaEditor::implementation
{
	static constexpr bool IsBrace(char ch) noexcept
	{
		return ch == '[' || ch == ']' || ch == '(' || ch == ')' || ch == '{' || ch == '}';
	}

	static constexpr bool IsAlphabetic(int ch) noexcept
	{
		return ((ch >= 'A') && (ch <= 'Z')) || ((ch >= 'a') && (ch <= 'z'));
	}

	static int IntegerFromString(const std::string &val, int defaultValue)
	{
		try
		{
			if (val.length())
			{
				return std::stoi(val);
			}
		}
		catch (std::logic_error &)
		{
			// Ignore bad values, either non-numeric or out of range numeric
		}
		return defaultValue;
	}

	static std::set<std::string> SetFromString(std::string_view text, char separator)
	{
		std::set<std::string> result;

		while (!text.empty())
		{
			const size_t after = text.find_first_of(separator);
			const std::string_view symbol(text.substr(0, after));
			if (!symbol.empty())
			{
				result.emplace(symbol);
			}
			if (after == std::string_view::npos)
			{
				break;
			}
			text.remove_prefix(after + 1);
		}
		return result;
	}


	StyleAndWords::StyleAndWords() noexcept
	{
	};

	// Set of words separated by spaces. First is style number, rest are symbols.
	// <styleNumber> [symbol]*
	StyleAndWords::StyleAndWords(const std::string &definition)
	{
		styleNumber = IntegerFromString(definition, 0);

		std::string_view symbols(definition);

		// Remove initial style number
		const size_t endNumber = symbols.find_first_of(' ');
		if (endNumber == std::string::npos)
		{
			return;
		}
		symbols.remove_prefix(endNumber + 1);

		words = SetFromString(symbols, ' ');
	}

	StyleAndWords::StyleAndWords(int style, const std::set<std::string> &words)
	{
		styleNumber = style;
		this->words = words;
	}

	bool StyleAndWords::IsEmpty() const noexcept
	{
		return words.empty();
	}

	// WinUI modified
	bool StyleAndWords::IsSingleChar() const noexcept
	{
		//if (words.size() != 1)
		if (words.size() == 0)
		{
			return false;
		}
		const std::string &first = *words.begin();
		return first.length() == 1;
	}

	// WinUI modified
	bool StyleAndWords::IsCharacter(char ch) const noexcept
	{
		//if (words.size() != 1)
		if (words.size() == 0)
		{
			return false;
		}
		for (std::string word : words)
		{
			if (word.size() == 1 && ch == word[0])
			{
				return true;
			}
		}
		return false;
		//const std::string &first = *words.begin();
		//return (first.length() == 1) && (ch == first[0]);
	}

	int StyleAndWords::Style() const noexcept
	{
		return styleNumber;
	}

	// WinUI modified
	bool StyleAndWords::Includes(const std::string &value) const
	{
		if (words.empty())
		{
			return false;
		}
		const std::string &first = *words.begin();
		if (first.empty())
		{
			return false;
		}
		if (IsAlphabetic(first[0]))
		{
			return words.count(value) != 0;
		}
		// Set of individual characters. Only one character allowed for now
		//const char ch = first[0];
		//return value.find(ch) != std::string::npos;
		for (std::string ch : words)
		{
			if (ch.size() == 1 && value.find(ch[0]) != std::string::npos)
			{
				return true;
			}
		}
		return false;
	}

	TextReader::TextReader(ScintillaCall &sc_) noexcept :
		startPos(extremePosition),
		endPos(0),
		codePage(0),
		sc(sc_),
		lenDoc(-1)
	{
		buf[0] = 0;
	}

	bool TextReader::InternalIsLeadByte(char ch) const
	{
		if (Scintilla::CpUtf8 == codePage)
			// For lexing, all characters >= 0x80 are treated the
			// same so none is considered a lead byte.
			return false;
		else
			return ::IsDBCSLeadByteEx(codePage, ch) != 0;
	}

	void TextReader::Fill(Position position)
	{
		if (lenDoc == -1)
			lenDoc = sc.Length();
		startPos = position - slopSize;
		if (startPos + bufferSize > lenDoc)
			startPos = lenDoc - bufferSize;
		if (startPos < 0)
			startPos = 0;
		endPos = startPos + bufferSize;
		if (endPos > lenDoc)
			endPos = lenDoc;
		sc.SetTarget(Span(startPos, endPos));
		sc.TargetText(buf);
	}

	bool TextReader::Match(Position pos, const char *s)
	{
		for (int i = 0; *s; i++)
		{
			if (*s != SafeGetCharAt(pos + i))
				return false;
			s++;
		}
		return true;
	}

	int TextReader::StyleAt(Position position)
	{
		return sc.UnsignedStyleAt(position);
	}

	Line TextReader::GetLine(Position position)
	{
		return sc.LineFromPosition(position);
	}

	Position TextReader::LineStart(Line line)
	{
		return sc.LineStart(line);
	}

	Scintilla::FoldLevel TextReader::LevelAt(Line line)
	{
		return sc.FoldLevel(line);
	}

	Position TextReader::Length()
	{
		if (lenDoc == -1)
			lenDoc = sc.Length();
		return lenDoc;
	}

	int TextReader::GetLineState(Line line)
	{
		return sc.LineState(line);
	}

	/**
	 * Find if there is a brace next to the caret, checking before caret first, then
	 * after caret. If brace found also find its matching brace.
	 * @return @c true if inside a bracket pair.
	 */
	bool CodeEditorControl::SciFindMatchingBracePosition(Scintilla::Position &braceAtCaret, Scintilla::Position &braceOpposite, bool sloppy)
	{
		// Config
		const auto bracesStyle{ 0 };
		const int lexLanguage{ Call().Lexer() };

		bool isInside = false;

		const int mainSel = Call().MainSelection();
		if (Call().SelectionNCaretVirtualSpace(mainSel) > 0)
			return false;

		const int bracesStyleCheck = bracesStyle;
		Scintilla::Position caretPos = Call().CurrentPos();
		braceAtCaret = -1;
		braceOpposite = -1;
		char charBefore = '\0';
		int styleBefore = 0;
		const Scintilla::Position lengthDoc = Call().Length();
		TextReader acc(Call());
		if ((lengthDoc > 0) && (caretPos > 0))
		{
			// Check to ensure not matching brace that is part of a multibyte character
			if (Call().PositionBefore(caretPos) == (caretPos - 1))
			{
				charBefore = acc[caretPos - 1];
				styleBefore = acc.StyleAt(caretPos - 1);
			}
		}
		// Priority goes to character before caret
		if (charBefore && IsBrace(charBefore) &&
			((styleBefore == bracesStyleCheck) || (!bracesStyle)))
		{
			braceAtCaret = caretPos - 1;
		}
		bool colonMode = false;
		if ((lexLanguage == SCLEX_PYTHON) &&
			(':' == charBefore) && (SCE_P_OPERATOR == styleBefore))
		{
			braceAtCaret = caretPos - 1;
			colonMode = true;
		}
		bool isAfter = true;
		if (lengthDoc > 0 && sloppy && (braceAtCaret < 0) && (caretPos < lengthDoc))
		{
			// No brace found so check other side
			// Check to ensure not matching brace that is part of a multibyte character
			if (Call().PositionAfter(caretPos) == (caretPos + 1))
			{
				const char charAfter = acc[caretPos];
				const int styleAfter = acc.StyleAt(caretPos);
				if (charAfter && IsBrace(charAfter) && ((styleAfter == bracesStyleCheck) || (!bracesStyle)))
				{
					braceAtCaret = caretPos;
					isAfter = false;
				}
				if ((lexLanguage == SCLEX_PYTHON) &&
					(':' == charAfter) && (SCE_P_OPERATOR == styleAfter))
				{
					braceAtCaret = caretPos;
					colonMode = true;
				}
			}
		}
		if (braceAtCaret >= 0)
		{
			if (colonMode)
			{
				const Scintilla::Line lineStart = Call().LineFromPosition(braceAtCaret);
				const Scintilla::Line lineMaxSubord = Call().LastChild(lineStart, static_cast<Scintilla::FoldLevel>(-1));
				braceOpposite = Call().LineEnd(lineMaxSubord);
			}
			else
			{
				braceOpposite = Call().BraceMatch(braceAtCaret, 0);
			}
			if (braceOpposite > braceAtCaret)
			{
				isInside = isAfter;
			}
			else
			{
				isInside = !isAfter;
			}
		}
		return isInside;
	}

	void CodeEditorControl::SciBraceMatch()
	{
		// Config
		const auto bracesCheck{ true };
		const auto bracesSloppy{ true };

		if (!bracesCheck)
			return;
		Scintilla::Position braceAtCaret = -1;
		Scintilla::Position braceOpposite = -1;
		SciFindMatchingBracePosition(braceAtCaret, braceOpposite, bracesSloppy);
		if ((braceAtCaret != -1) && (braceOpposite == -1))
		{
			//Call().BraceBadLight(braceAtCaret);
			Call().BraceBadLight(-1);
			Call().SetHighlightGuide(0);
		}
		else
		{
			char chBrace = 0;
			if (braceAtCaret >= 0)
				chBrace = Call().CharacterAt(braceAtCaret);
			Call().BraceHighlight(braceAtCaret, braceOpposite);
			Scintilla::Position columnAtCaret = Call().Column(braceAtCaret);
			Scintilla::Position columnOpposite = Call().Column(braceOpposite);
			if (chBrace == ':')
			{
				const Scintilla::Line lineStart = Call().LineFromPosition(braceAtCaret);
				const Scintilla::Position indentPos = Call().LineIndentPosition(lineStart);
				const Scintilla::Position indentPosNext = Call().LineIndentPosition(lineStart + 1);
				columnAtCaret = Call().Column(indentPos);
				const Scintilla::Position columnAtCaretNext = Call().Column(indentPosNext);
				const int indentSize = Call().Indent();
				if (columnAtCaretNext - indentSize > 1)
					columnAtCaret = columnAtCaretNext - indentSize;
				if (columnOpposite == 0)	// If the final line of the structure is empty
					columnOpposite = columnAtCaret;
			}
			else
			{
				if (Call().LineFromPosition(braceAtCaret) == Call().LineFromPosition(braceOpposite))
				{
					// Avoid attempting to draw a highlight guide
					columnAtCaret = 0;
					columnOpposite = 0;
				}
			}

			// Todo: has rendering issues
			//if (props.GetInt("highlight.indentation.guides"))
				//Call().SetHighlightGuide(std::min(columnAtCaret, columnOpposite));
		}
	}

	Line CodeEditorControl::SciGetCurrentLineNumber()
	{
		return Call().LineFromPosition(
			Call().CurrentPos());
	}

	int CodeEditorControl::SciGetLineIndentation(Line line)
	{
		return Call().LineIndentation(line);
	}

	Position CodeEditorControl::SciGetLineIndentPosition(Line line)
	{
		return Call().LineIndentPosition(line);
	}

	std::vector<std::string> CodeEditorControl::SciGetLinePartsInStyle(Line line, const StyleAndWords &saw)
	{
		std::vector<std::string> sv;
		TextReader acc(Call());
		std::string s;
		const bool separateCharacters = saw.IsSingleChar();
		const Position thisLineStart = Call().LineStart(line);
		const Position nextLineStart = Call().LineStart(line + 1);
		for (Position pos = thisLineStart; pos < nextLineStart; pos++)
		{
			if (acc.StyleAt(pos) == saw.Style())
			{
				if (separateCharacters)
				{
					// Add one character at a time, even if there is an adjacent character in the same style
					if (s.length() > 0)
					{
						sv.push_back(s);
					}
					s = "";
				}
				s += acc[pos];
			}
			else if (s.length() > 0)
			{
				sv.push_back(s);
				s = "";
			}
		}
		if (s.length() > 0)
		{
			sv.push_back(s);
		}
		return sv;
	}

	IndentationStatus CodeEditorControl::SciGetIndentState(Line line)
	{
		// C like language indentation defined by braces and keywords
		IndentationStatus indentState = IndentationStatus::none;
		const std::vector<std::string> controlIndents = SciGetLinePartsInStyle(line, _sciStatementIndent);
		for (const std::string &sIndent : controlIndents)
		{
			if (_sciStatementIndent.Includes(sIndent))
				indentState = IndentationStatus::keyWordStart;
		}
		const std::vector<std::string> controlEnds = SciGetLinePartsInStyle(line, _sciStatementEnd);
		for (const std::string &sEnd : controlEnds)
		{
			if (_sciStatementEnd.Includes(sEnd))
				indentState = IndentationStatus::none;
		}
		// Braces override keywords
		const std::vector<std::string> controlBlocks = SciGetLinePartsInStyle(line, _sciBlockEnd);
		for (const std::string &sBlock : controlBlocks)
		{
			if (_sciBlockEnd.Includes(sBlock))
				indentState = IndentationStatus::blockEnd;
			if (_sciBlockStart.Includes(sBlock))
				indentState = IndentationStatus::blockStart;
		}
		return indentState;
	}

	int CodeEditorControl::SciIndentOfBlock(Line line)
	{
		if (line < 0)
			return 0;
		const int indentSize = Call().Indent();
		int indentBlock = SciGetLineIndentation(line);
		Line backLine = line;
		IndentationStatus indentState = IndentationStatus::none;
		if (_sciStatementIndent.IsEmpty() && _sciBlockStart.IsEmpty() && _sciBlockEnd.IsEmpty())
			indentState = IndentationStatus::blockStart;	// Don't bother searching backwards

		Line lineLimit = line - _sciStatementLookback;
		if (lineLimit < 0)
			lineLimit = 0;
		while ((backLine >= lineLimit) && (indentState == IndentationStatus::none))
		{
			indentState = SciGetIndentState(backLine);
			if (indentState != IndentationStatus::none)
			{
				indentBlock = SciGetLineIndentation(backLine);
				if (indentState == IndentationStatus::blockStart)
				{
					if (!_sciIndentOpening)
						indentBlock += indentSize;
				}
				if (indentState == IndentationStatus::blockEnd)
				{
					if (_sciIndentClosing)
						indentBlock -= indentSize;
					if (indentBlock < 0)
						indentBlock = 0;
				}
				if ((indentState == IndentationStatus::keyWordStart) && (backLine == line))
					indentBlock += indentSize;
			}
			backLine--;
		}
		return indentBlock;
	}

	Span CodeEditorControl::SciGetSelection()
	{
		return Call().SelectionSpan();
	}

	void CodeEditorControl::SciSetSelection(Position anchor, Position currentPos)
	{
		Call().SetSel(anchor, currentPos);
	}

	void CodeEditorControl::SciSetLineIndentation(Line line, int indent)
	{
		if (indent < 0)
			return;
		const Span rangeStart = SciGetSelection();
		Span range = rangeStart;
		const Position posBefore = SciGetLineIndentPosition(line);
		Call().SetLineIndentation(line, indent);
		const Position posAfter = SciGetLineIndentPosition(line);
		const Position posDifference = posAfter - posBefore;
		if (posAfter > posBefore)
		{
			// Move selection on
			if (range.start >= posBefore)
			{
				range.start += posDifference;
			}
			if (range.end >= posBefore)
			{
				range.end += posDifference;
			}
		}
		else if (posAfter < posBefore)
		{
			// Move selection back
			if (range.start >= posAfter)
			{
				if (range.start >= posBefore)
					range.start += posDifference;
				else
					range.start = posAfter;
			}
			if (range.end >= posAfter)
			{
				if (range.end >= posBefore)
					range.end += posDifference;
				else
					range.end = posAfter;
			}
		}
		if (!(rangeStart == range))
		{
			SciSetSelection(range.start, range.end);
		}
	}

	bool CodeEditorControl::SciRangeIsAllWhitespace(Position start, Position end)
	{
		TextReader acc(Call());
		for (Position i = start; i < end; i++)
		{
			if ((acc[i] != ' ') && (acc[i] != '\t'))
				return false;
		}
		return true;
	}

	void CodeEditorControl::SciAutomaticIndentation(char ch)
	{
		const Span range = Call().SelectionSpan();
		const Position selStart = range.start;
		const Line curLine = SciGetCurrentLineNumber();
		const Position thisLineStart = Call().LineStart(curLine);
		const int indentSize = Call().Indent();
		int indentBlock = SciIndentOfBlock(curLine - 1);

		if ((Call().Lexer() == SCLEX_PYTHON) &&
			/*(props.GetInt("indent.python.colon") == 1)*/ true)
		{
			const Scintilla::EndOfLine eolMode = Call().EOLMode();
			const int eolChar = (eolMode == Scintilla::EndOfLine::Cr ? '\r' : '\n');
			const int eolChars = (eolMode == Scintilla::EndOfLine::CrLf ? 2 : 1);
			const Position prevLineStart = Call().LineStart(curLine - 1);
			const Position prevIndentPos = SciGetLineIndentPosition(curLine - 1);
			const int indentExisting = SciGetLineIndentation(curLine);

			if (ch == eolChar)
			{
				// Find last noncomment, nonwhitespace character on previous line
				char character = '\0';
				int style = 0;
				for (Position p = selStart - eolChars - 1; p > prevLineStart; p--)
				{
					style = Call().UnsignedStyleAt(p);
					if (style != SCE_P_DEFAULT && style != SCE_P_COMMENTLINE &&
						style != SCE_P_COMMENTBLOCK)
					{
						character = Call().CharacterAt(p);
						break;
					}
				}
				indentBlock = SciGetLineIndentation(curLine - 1);
				if (style == SCE_P_OPERATOR && character == ':')
				{
					SciSetLineIndentation(curLine, indentBlock + indentSize);
				}
				else if (selStart == prevIndentPos + eolChars)
				{
					// Preserve the indentation of preexisting text beyond the caret
					SciSetLineIndentation(curLine, indentBlock + indentExisting);
				}
				else
				{
					SciSetLineIndentation(curLine, indentBlock);
				}
			}
			return;
		}

		if (_sciBlockEnd.IsCharacter(ch))
		{	// Dedent maybe
			if (!_sciIndentClosing)
			{
				if (SciRangeIsAllWhitespace(thisLineStart, selStart - 1))
				{
					SciSetLineIndentation(curLine, indentBlock - indentSize);
				}
			}
		}
		else if (!_sciBlockEnd.IsSingleChar() && (ch == ' '))
		{	// Dedent maybe
			if (!_sciIndentClosing && (SciGetIndentState(curLine) == IndentationStatus::blockEnd)) {}
		}
		else if (_sciBlockStart.IsCharacter(ch))
		{
			// Dedent maybe if first on line and previous line was starting keyword
			if (!_sciIndentOpening && (SciGetIndentState(curLine - 1) == IndentationStatus::keyWordStart))
			{
				if (SciRangeIsAllWhitespace(thisLineStart, selStart - 1))
				{
					SciSetLineIndentation(curLine, indentBlock - indentSize);
				}
			}
		}
		else if ((ch == '\r' || ch == '\n') && (selStart == thisLineStart))
		{
			if (!_sciIndentClosing && !_sciBlockEnd.IsSingleChar())
			{	// Dedent previous line maybe
				const std::vector<std::string> controlWords = SciGetLinePartsInStyle(curLine - 1, _sciBlockEnd);
				if (!controlWords.empty())
				{
					if (_sciBlockEnd.Includes(controlWords[0]))
					{
						// Check if first keyword on line is an ender
						SciSetLineIndentation(curLine - 1, SciIndentOfBlock(curLine - 2) - indentSize);
						// Recalculate as may have changed previous line
						indentBlock = SciIndentOfBlock(curLine - 1);
					}
				}
			}
			SciSetLineIndentation(curLine, indentBlock);
		}
	}
}
