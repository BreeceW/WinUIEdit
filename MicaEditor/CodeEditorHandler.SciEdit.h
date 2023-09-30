// This file includes code derived from SciTEBase.h and StyleWriter.h
// Copyright 1998-2011 by Neil Hodgson <neilh@scintilla.org>
// The scintilla\License.txt file describes the conditions under which this software may be distributed.

#pragma once

namespace MicaEditor::Internal
{
	enum class IndentationStatus
	{
		none, // no effect on indentation
		blockStart,	// indentation block begin such as "{" or VB "function"
		blockEnd, // indentation end indicator such as "}" or VB "end"
		keyWordStart, // Keywords that cause indentation
	};

	class StyleAndWords
	{
		int styleNumber = 0;
		std::set<std::string> words;
	public:
		StyleAndWords() noexcept;
		explicit StyleAndWords(const std::string &definition);
		StyleAndWords(int style, const std::set<std::string> &words);
		[[nodiscard]] bool IsEmpty() const noexcept;
		[[nodiscard]] bool IsSingleChar() const noexcept;
		[[nodiscard]] bool IsCharacter(char ch) const noexcept;
		[[nodiscard]] int Style() const noexcept;
		[[nodiscard]] bool Includes(const std::string &value) const;
	};

	// Read only access to a document, its styles and other data
	class TextReader
	{
	protected:
		static constexpr Scintilla::Position extremePosition = INTPTR_MAX;
		/** @a bufferSize is a trade off between time taken to copy the characters
		 * and retrieval overhead.
		 * @a slopSize positions the buffer before the desired position
		 * in case there is some backtracking. */
		static constexpr Scintilla::Position bufferSize = 4000;
		static constexpr Scintilla::Position slopSize = bufferSize / 8;
		char buf[bufferSize + 1];
		Scintilla::Position startPos;
		Scintilla::Position endPos;
		int codePage;

		Scintilla::ScintillaCall &sc;
		Scintilla::Position lenDoc;

		void Fill(Scintilla::Position position);
	public:
		explicit TextReader(Scintilla::ScintillaCall &sc_) noexcept;
		// Deleted so TextReader objects can not be copied.
		TextReader(const TextReader &source) = delete;
		TextReader &operator=(const TextReader &) = delete;
		char operator[](Scintilla::Position position)
		{
			if (position < startPos || position >= endPos)
			{
				Fill(position);
			}
			return buf[position - startPos];
		}
		/** Safe version of operator[], returning a defined value for invalid position. */
		char SafeGetCharAt(Scintilla::Position position, char chDefault = ' ')
		{
			if (position < startPos || position >= endPos)
			{
				Fill(position);
				if (position < startPos || position >= endPos)
				{
					// Position is outside range of document
					return chDefault;
				}
			}
			return buf[position - startPos];
		}
		void SetCodePage(int codePage_) noexcept
		{
			codePage = codePage_;
		}
		bool Match(Scintilla::Position pos, const char *s);
		int StyleAt(Scintilla::Position position);
		Scintilla::Line GetLine(Scintilla::Position position);
		Scintilla::Position LineStart(Scintilla::Line line);
		Scintilla::FoldLevel LevelAt(Scintilla::Line line);
		Scintilla::Position Length();
		int GetLineState(Scintilla::Line line);
	};
}
