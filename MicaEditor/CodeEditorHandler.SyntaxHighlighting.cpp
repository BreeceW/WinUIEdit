#include "CodeEditorHandler.h"
#include "SciLexer.h"

using namespace Scintilla;

namespace MicaEditor
{
	void CodeEditorHandler::SetLexer()
	{
		if (_highlightingLanguage == L"cpp")
		{
			_call->SetILexer(_createLexer("cpp"));
			// This list of keywords from SciTe (cpp.properties)
			_call->SetKeyWords(0,
				"alignas alignof and and_eq asm audit auto axiom bitand bitor bool "
				"char char8_t char16_t char32_t class compl concept "
				"const consteval constexpr const_cast "
				"decltype default delete double dynamic_cast enum explicit export extern false final float "
				"friend import inline int long module mutable namespace new noexcept not not_eq nullptr "
				"operator or or_eq override private protected public "
				"register reinterpret_cast requires "
				"short signed sizeof static static_assert static_cast struct "
				"template this thread_local true typedef typeid typename union unsigned using "
				"virtual void volatile wchar_t xor xor_eq");
			_call->SetKeyWords(1,
				"break case catch co_await co_return co_yield continue do else for goto if return switch throw try while");
			SetLanguageIndentMode(
				SCE_C_WORD2, { "case", "default", "do", "else", "for", "if", "while", },
				SCE_C_OPERATOR, { ";", },
				SCE_C_OPERATOR, { "{", },
				SCE_C_OPERATOR, { "}", });
		}
		else if (_highlightingLanguage == L"csharp")
		{
			_call->SetILexer(_createLexer("cpp"));
			// This list of keywords from SciTe (cpp.properties) and https://learn.microsoft.com/en-us/dotnet/csharp/language-reference/keywords/
			_call->SetKeyWords(0,
				"abstract as ascending async await base bool by byte char checked "
				"class const decimal default delegate descending double enum "
				"equals event explicit extern false fixed float from get group "
				"implicit in int interface internal into is join lock let long nameof namespace new nint null "
				"object on operator orderby out override params private protected public "
				"readonly ref sbyte sealed select set short sizeof stackalloc static "
				"string struct this true typeof uint ulong"
				"unchecked unsafe ushort var virtual void volatile where");
			_call->SetKeyWords(1,
				"break case catch continue do else finally for foreach goto if return switch throw try using while yield");
			// XML document comments from https://learn.microsoft.com/en-us/dotnet/csharp/language-reference/language-specification/documentation-comments#d31-general
			_call->SetKeyWords(2,
				"c code example exception include list para param paramref permission remarks returns see seealso summary typeparam typeparamref value");
			SetLanguageIndentMode(
				SCE_C_WORD2, { "case", "default", "do", "else", "for", "foreach", "if", "using", "while", },
				SCE_C_OPERATOR, { ";", },
				SCE_C_OPERATOR, { "{", },
				SCE_C_OPERATOR, { "}", });
		}
		else if (_highlightingLanguage == L"js")
		{
			_call->SetILexer(_createLexer("cpp"));
			// This list of keywords from MDN https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Lexical_grammar#keywords
			// Note additional words like undefined
			_call->SetKeyWords(0,
				"class const debugger delete export extends false function in instanceof "
				"new null super this true typeof var void let static enum implements "
				"interface private protected public arguments async get of set Infinity NaN undefined");
			_call->SetKeyWords(1,
				"break case catch continue default do else finally for if import return "
				"switch throw try while with yield await package as from");
			_call->SetKeyWords(2,
				"flow");
			SetLanguageIndentMode(
				SCE_C_WORD2, { "case", "default", "do", "else", "for", "if", "while", },
				SCE_C_OPERATOR, { ";", },
				SCE_C_OPERATOR, { "{", "[" },
				SCE_C_OPERATOR, { "}", "]" });
		}
		else if (_highlightingLanguage == L"json")
		{
			const auto lexer{ _createLexer("json") };
			lexer->PropertySet("lexer.json.allow.comments", "1");
			lexer->PropertySet("lexer.json.escape.sequence", "1");
			_call->SetILexer(lexer);
			// This list of keywords from SciTe (json.properties)
			_call->SetKeyWords(0, "false true null");
			_call->SetKeyWords(1,
				"@id @context @type @value @language @container @list @set @reverse @index @base @vocab @graph");
			SetLanguageIndentMode(
				0, { },
				0, { },
				SCE_JSON_OPERATOR, { "{", "[", },
				SCE_JSON_OPERATOR, { "}", "]", });
		}
		else
		{
			_call->SetILexer(nullptr);
			SetLanguageIndentMode(0, { }, 0, { }, 0, { }, 0, { });
		}
	}

	void CodeEditorHandler::UpdateLanguageStyles()
	{
		if (_highlightingLanguage == L"cpp" || _highlightingLanguage == L"csharp" || _highlightingLanguage == L"js")
		{
			switch (_theme)
			{
			case CodeEditorTheme::Dark:
				StyleSetFore(static_cast<int>(StylesCommon::Default), IntRGBA(0xD4, 0xD4, 0xD4));
				StyleClearCustom();

				StyleSetFore(static_cast<int>(StylesCommon::BraceLight), IntRGBA(0xD4, 0xD4, 0xD4));

				//StyleSetFore(SCE_C_IDENTIFIER, IntRGBA(0x9C, 0xDC, 0xFE));
				StyleSetFore(SCE_C_ESCAPESEQUENCE, IntRGBA(0xD7, 0xBA, 0x7D));
				StyleSetFore(SCE_C_NUMBER, IntRGBA(0xB5, 0xCE, 0xA8));
				StyleSetFore(SCE_C_WORD, IntRGBA(0x56, 0x9C, 0xD6));
				StyleSetFore(SCE_C_WORD2, IntRGBA(0xC5, 0x86, 0xC0));
				StyleSetFore(SCE_C_STRING, IntRGBA(0xCE, 0x91, 0x78));
				StyleSetFore(SCE_C_OPERATOR, IntRGBA(0xD4, 0xD4, 0xD4));
				StyleSetFore(SCE_C_PREPROCESSOR, IntRGBA(0x9B, 0x9B, 0x9B));
				StyleSetFore(SCE_C_COMMENT, IntRGBA(0x6A, 0x99, 0x55));
				StyleSetFore(SCE_C_COMMENTLINE, IntRGBA(0x6A, 0x99, 0x55));
				StyleSetFore(SCE_C_COMMENTLINEDOC, IntRGBA(0x6A, 0x99, 0x55));
				StyleSetFore(SCE_C_COMMENTDOC, IntRGBA(0x6A, 0x99, 0x55));
				StyleSetFore(SCE_C_COMMENTDOCKEYWORD, IntRGBA(0x56, 0x9C, 0xD6));
				StyleSetFore(SCE_C_COMMENTDOCKEYWORDERROR, IntRGBA(0x6A, 0x99, 0x55));
				break;

			case CodeEditorTheme::Light:
				StyleSetFore(static_cast<int>(StylesCommon::Default), IntRGBA(0, 0, 0));
				StyleClearCustom();

				StyleSetFore(static_cast<int>(StylesCommon::BraceLight), IntRGBA(0x00, 0x00, 0x00));

				//StyleSetFore(SCE_C_IDENTIFIER, IntRGBA(0x00, 0x10, 0x80));
				StyleSetFore(SCE_C_ESCAPESEQUENCE, IntRGBA(0xEE, 0x00, 0x00));
				StyleSetFore(SCE_C_NUMBER, IntRGBA(0x09, 0x86, 0x58));
				StyleSetFore(SCE_C_WORD, IntRGBA(0x00, 0x00, 0xFF));
				StyleSetFore(SCE_C_WORD2, IntRGBA(0xaf, 0x00, 0xdb));
				StyleSetFore(SCE_C_STRING, IntRGBA(0xa3, 0x15, 0x15));
				StyleSetFore(SCE_C_OPERATOR, IntRGBA(0x00, 0x00, 0x00));
				StyleSetFore(SCE_C_PREPROCESSOR, IntRGBA(0x80, 0x80, 0x80));
				StyleSetFore(SCE_C_COMMENT, IntRGBA(0x00, 0x80, 0x00));
				StyleSetFore(SCE_C_COMMENTLINE, IntRGBA(0x00, 0x80, 0x00));
				StyleSetFore(SCE_C_COMMENTLINEDOC, IntRGBA(0x00, 0x80, 0x00));
				StyleSetFore(SCE_C_COMMENTDOC, IntRGBA(0x00, 0x80, 0x00));
				StyleSetFore(SCE_C_COMMENTDOCKEYWORD, IntRGBA(0x80, 0x00, 0x00));
				StyleSetFore(SCE_C_COMMENTDOCKEYWORDERROR, IntRGBA(0x00, 0x80, 0x00));
				break;
			}
		}
		else if (_highlightingLanguage == L"json")
		{
			switch (_theme)
			{
			case CodeEditorTheme::Dark:
				StyleSetFore(static_cast<int>(StylesCommon::Default), IntRGBA(0xD4, 0xD4, 0xD4));
				StyleClearCustom();

				StyleSetFore(static_cast<int>(StylesCommon::BraceLight), IntRGBA(0xD4, 0xD4, 0xD4));

				StyleSetFore(SCE_JSON_PROPERTYNAME, IntRGBA(0x9C, 0xDC, 0xFE));
				StyleSetFore(SCE_JSON_ESCAPESEQUENCE, IntRGBA(0xD7, 0xBA, 0x7D));
				StyleSetFore(SCE_JSON_NUMBER, IntRGBA(0xB5, 0xCE, 0xA8));
				StyleSetFore(SCE_JSON_KEYWORD, IntRGBA(0x56, 0x9C, 0xD6));
				StyleSetFore(SCE_JSON_LDKEYWORD, IntRGBA(0xC5, 0x86, 0xC0));
				StyleSetFore(SCE_JSON_STRING, IntRGBA(0xCE, 0x91, 0x78));
				StyleSetFore(SCE_JSON_STRINGEOL, IntRGBA(0xCE, 0x91, 0x78));
				StyleSetFore(SCE_JSON_OPERATOR, IntRGBA(0xD4, 0xD4, 0xD4));
				StyleSetFore(SCE_JSON_ERROR, IntRGBA(0xcd, 0x31, 0x31));
				StyleSetFore(SCE_JSON_BLOCKCOMMENT, IntRGBA(0x6A, 0x99, 0x55));
				StyleSetFore(SCE_JSON_LINECOMMENT, IntRGBA(0x6A, 0x99, 0x55));
				break;

			case CodeEditorTheme::Light:
				StyleSetFore(static_cast<int>(StylesCommon::Default), IntRGBA(0, 0, 0));
				StyleClearCustom();

				StyleSetFore(static_cast<int>(StylesCommon::BraceLight), IntRGBA(0x00, 0x00, 0x00));

				StyleSetFore(SCE_JSON_PROPERTYNAME, IntRGBA(0x04, 0x51, 0xa5));
				StyleSetFore(SCE_JSON_ESCAPESEQUENCE, IntRGBA(0xEE, 0x00, 0x00));
				StyleSetFore(SCE_JSON_NUMBER, IntRGBA(0x09, 0x86, 0x58));
				StyleSetFore(SCE_JSON_KEYWORD, IntRGBA(0x00, 0x00, 0xFF));
				StyleSetFore(SCE_JSON_LDKEYWORD, IntRGBA(0xaf, 0x00, 0xdb));
				StyleSetFore(SCE_JSON_STRING, IntRGBA(0xa3, 0x15, 0x15));
				StyleSetFore(SCE_JSON_STRINGEOL, IntRGBA(0xa3, 0x15, 0x15));
				StyleSetFore(SCE_JSON_OPERATOR, IntRGBA(0x00, 0x00, 0x00));
				StyleSetFore(SCE_JSON_ERROR, IntRGBA(0xcd, 0x31, 0x31));
				StyleSetFore(SCE_JSON_BLOCKCOMMENT, IntRGBA(0x00, 0x80, 0x00));
				StyleSetFore(SCE_JSON_LINECOMMENT, IntRGBA(0x00, 0x80, 0x00));
				break;
			}
		}
		else
		{
			StyleClearCustom();
		}
	}
}
