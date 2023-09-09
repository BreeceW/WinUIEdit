#include "pch.h"
#include "CodeEditorControl.h"

using namespace winrt;
using namespace DUX;
using namespace Scintilla;
using namespace Scintilla::Internal;

namespace winrt::MicaEditor::implementation
{
	void CodeEditorControl::SetLexer()
	{
		if (_highlightingLanguage == L"cpp")
		{
			_editor->PublicWndProc(Scintilla::Message::SetILexer, 0, reinterpret_cast<Scintilla::uptr_t>(CreateLexer("cpp")));
			// This list of keywords from SciTe (cpp.properties)
			_editor->PublicWndProc(Scintilla::Message::SetKeyWords, 0, reinterpret_cast<Scintilla::uptr_t>(
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
			_editor->PublicWndProc(Scintilla::Message::SetKeyWords, 1, reinterpret_cast<Scintilla::uptr_t>(
				"break case catch co_await co_return co_yield continue do else for goto if return switch throw try while"));
		}
		else if (_highlightingLanguage == L"csharp")
		{
			_editor->PublicWndProc(Scintilla::Message::SetILexer, 0, reinterpret_cast<Scintilla::uptr_t>(CreateLexer("cpp")));
			// This list of keywords from SciTe (cpp.properties) and https://learn.microsoft.com/en-us/dotnet/csharp/language-reference/keywords/
			_editor->PublicWndProc(Scintilla::Message::SetKeyWords, 0, reinterpret_cast<Scintilla::uptr_t>(
				"abstract as ascending async await base bool by byte char checked "
				"class const decimal default delegate descending double enum "
				"equals event explicit extern false fixed float from get group "
				"implicit in int interface internal into is join lock let long nameof namespace new nint null "
				"object on operator orderby out override params private protected public "
				"readonly ref sbyte sealed select set short sizeof stackalloc static "
				"string struct this true typeof uint ulong"
				"unchecked unsafe ushort var virtual void volatile where "));
			_editor->PublicWndProc(Scintilla::Message::SetKeyWords, 1, reinterpret_cast<Scintilla::uptr_t>(
				"break case catch continue do else finally for foreach goto if return switch throw try using while yield "));
		}
		else if (_highlightingLanguage == L"json")
		{
			const auto lexer{ CreateLexer("json") };
			lexer->PropertySet("lexer.json.allow.comments", "1");
			lexer->PropertySet("lexer.json.escape.sequence", "1");
			_editor->PublicWndProc(Scintilla::Message::SetILexer, 0, reinterpret_cast<Scintilla::uptr_t>(lexer));
			// This list of keywords from SciTe (json.properties)
			_editor->PublicWndProc(Scintilla::Message::SetKeyWords, 0, reinterpret_cast<Scintilla::uptr_t>("false true null"));
			_editor->PublicWndProc(Scintilla::Message::SetKeyWords, 1, reinterpret_cast<Scintilla::uptr_t>(
				"@id @context @type @value @language @container @list @set @reverse @index @base @vocab @graph"));
		}
		else
		{
			_editor->PublicWndProc(Scintilla::Message::SetILexer, 0, 0);
		}
	}

	void CodeEditorControl::UpdateLanguageStyles()
	{
		if (_highlightingLanguage == L"cpp" || _highlightingLanguage == L"csharp")
		{
			switch (_theme)
			{
			case ElementTheme::Dark:
				_editor->StyleSetBackTransparent(STYLE_DEFAULT, ColourRGBA{});
				_editor->StyleSetForeTransparent(STYLE_DEFAULT, ColourRGBA{ 0xD4, 0xD4, 0xD4 });
				_editor->PublicWndProc(Message::StyleClearAll, 0, 0);

				//_editor->StyleSetForeTransparent(SCE_C_IDENTIFIER, ColourRGBA{ 0x9C, 0xDC, 0xFE });
				_editor->StyleSetForeTransparent(SCE_C_ESCAPESEQUENCE, ColourRGBA{ 0xD7, 0xBA, 0x7D });
				_editor->StyleSetForeTransparent(SCE_C_NUMBER, ColourRGBA{ 0xB5, 0xCE, 0xA8 });
				_editor->StyleSetForeTransparent(SCE_C_WORD, ColourRGBA{ 0x56, 0x9C, 0xD6 });
				_editor->StyleSetForeTransparent(SCE_C_WORD2, ColourRGBA{ 0xC5, 0x86, 0xC0 });
				_editor->StyleSetForeTransparent(SCE_C_STRING, ColourRGBA{ 0xCE, 0x91, 0x78 });
				_editor->StyleSetForeTransparent(SCE_C_OPERATOR, ColourRGBA{ 0xD4, 0xD4, 0xD4 });
				_editor->StyleSetForeTransparent(SCE_C_PREPROCESSOR, ColourRGBA{ 0x9B, 0x9B, 0x9B });
				_editor->StyleSetForeTransparent(SCE_C_COMMENT, ColourRGBA{ 0x6A, 0x99, 0x55 });
				_editor->StyleSetForeTransparent(SCE_C_COMMENTLINE, ColourRGBA{ 0x6A, 0x99, 0x55 });
				break;

			case ElementTheme::Light:
				_editor->StyleSetBackTransparent(STYLE_DEFAULT, ColourRGBA{});
				_editor->StyleSetForeTransparent(STYLE_DEFAULT, ColourRGBA{ 0, 0, 0 });
				_editor->PublicWndProc(Message::StyleClearAll, 0, 0);

				//_editor->StyleSetForeTransparent(SCE_C_IDENTIFIER, ColourRGBA{ 0x00, 0x10, 0x80 });
				_editor->StyleSetForeTransparent(SCE_C_ESCAPESEQUENCE, ColourRGBA{ 0xEE, 0x00, 0x00 });
				_editor->StyleSetForeTransparent(SCE_C_NUMBER, ColourRGBA{ 0x09, 0x86, 0x58 });
				_editor->StyleSetForeTransparent(SCE_C_WORD, ColourRGBA{ 0x00, 0x00, 0xFF });
				_editor->StyleSetForeTransparent(SCE_C_WORD2, ColourRGBA{ 0xaf, 0x00, 0xdb });
				_editor->StyleSetForeTransparent(SCE_C_STRING, ColourRGBA{ 0xa3, 0x15, 0x15 });
				_editor->StyleSetForeTransparent(SCE_C_OPERATOR, ColourRGBA{ 0x00, 0x00, 0x00 });
				_editor->StyleSetForeTransparent(SCE_C_PREPROCESSOR, ColourRGBA{ 0x80, 0x80, 0x80 });
				_editor->StyleSetForeTransparent(SCE_C_COMMENT, ColourRGBA{ 0x00, 0x80, 0x00 });
				_editor->StyleSetForeTransparent(SCE_C_COMMENTLINE, ColourRGBA{ 0x00, 0x80, 0x00 });
				break;
			}
		}
		else if (_highlightingLanguage == L"json")
		{
			switch (_theme)
			{
			case ElementTheme::Dark:
				_editor->StyleSetBackTransparent(STYLE_DEFAULT, ColourRGBA{});
				_editor->StyleSetForeTransparent(STYLE_DEFAULT, ColourRGBA{ 0xD4, 0xD4, 0xD4 });
				_editor->PublicWndProc(Message::StyleClearAll, 0, 0);

				_editor->StyleSetForeTransparent(SCE_JSON_PROPERTYNAME, ColourRGBA{ 0x9C, 0xDC, 0xFE });
				_editor->StyleSetForeTransparent(SCE_JSON_ESCAPESEQUENCE, ColourRGBA{ 0xD7, 0xBA, 0x7D });
				_editor->StyleSetForeTransparent(SCE_JSON_NUMBER, ColourRGBA{ 0xB5, 0xCE, 0xA8 });
				_editor->StyleSetForeTransparent(SCE_JSON_KEYWORD, ColourRGBA{ 0x56, 0x9C, 0xD6 });
				_editor->StyleSetForeTransparent(SCE_JSON_LDKEYWORD, ColourRGBA{ 0xC5, 0x86, 0xC0 });
				_editor->StyleSetForeTransparent(SCE_JSON_STRING, ColourRGBA{ 0xCE, 0x91, 0x78 });
				_editor->StyleSetForeTransparent(SCE_JSON_STRINGEOL, ColourRGBA{ 0xCE, 0x91, 0x78 });
				_editor->StyleSetForeTransparent(SCE_JSON_OPERATOR, ColourRGBA{ 0xD4, 0xD4, 0xD4 });
				_editor->StyleSetForeTransparent(SCE_JSON_ERROR, ColourRGBA{ 0xcd, 0x31, 0x31 });
				_editor->StyleSetForeTransparent(SCE_JSON_BLOCKCOMMENT, ColourRGBA{ 0x6A, 0x99, 0x55 });
				_editor->StyleSetForeTransparent(SCE_JSON_LINECOMMENT, ColourRGBA{ 0x6A, 0x99, 0x55 });
				break;

			case ElementTheme::Light:
				_editor->StyleSetBackTransparent(STYLE_DEFAULT, ColourRGBA{});
				_editor->StyleSetForeTransparent(STYLE_DEFAULT, ColourRGBA{ 0, 0, 0 });
				_editor->PublicWndProc(Message::StyleClearAll, 0, 0);

				_editor->StyleSetForeTransparent(SCE_JSON_PROPERTYNAME, ColourRGBA{ 0x04, 0x51, 0xa5 });
				_editor->StyleSetForeTransparent(SCE_JSON_ESCAPESEQUENCE, ColourRGBA{ 0xEE, 0x00, 0x00 });
				_editor->StyleSetForeTransparent(SCE_JSON_NUMBER, ColourRGBA{ 0x09, 0x86, 0x58 });
				_editor->StyleSetForeTransparent(SCE_JSON_KEYWORD, ColourRGBA{ 0x00, 0x00, 0xFF });
				_editor->StyleSetForeTransparent(SCE_JSON_LDKEYWORD, ColourRGBA{ 0xaf, 0x00, 0xdb });
				_editor->StyleSetForeTransparent(SCE_JSON_STRING, ColourRGBA{ 0xa3, 0x15, 0x15 });
				_editor->StyleSetForeTransparent(SCE_JSON_STRINGEOL, ColourRGBA{ 0xa3, 0x15, 0x15 });
				_editor->StyleSetForeTransparent(SCE_JSON_OPERATOR, ColourRGBA{ 0x00, 0x00, 0x00 });
				_editor->StyleSetForeTransparent(SCE_JSON_ERROR, ColourRGBA{ 0xcd, 0x31, 0x31 });
				_editor->StyleSetForeTransparent(SCE_JSON_BLOCKCOMMENT, ColourRGBA{ 0x00, 0x80, 0x00 });
				_editor->StyleSetForeTransparent(SCE_JSON_LINECOMMENT, ColourRGBA{ 0x00, 0x80, 0x00 });
				break;
			}
		}
		else
		{
			switch (_theme)
			{
			case ElementTheme::Dark:
				_editor->StyleSetBackTransparent(STYLE_DEFAULT, ColourRGBA{});
				_editor->StyleSetForeTransparent(STYLE_DEFAULT, ColourRGBA{ 255, 255, 255 });
				_editor->PublicWndProc(Message::StyleClearAll, 0, 0);
				break;

			case ElementTheme::Light:
				_editor->StyleSetBackTransparent(STYLE_DEFAULT, ColourRGBA{});
				_editor->StyleSetForeTransparent(STYLE_DEFAULT, ColourRGBA{ 0, 0, 0 });
				_editor->PublicWndProc(Message::StyleClearAll, 0, 0);
				break;
			}
		}
	}
}
