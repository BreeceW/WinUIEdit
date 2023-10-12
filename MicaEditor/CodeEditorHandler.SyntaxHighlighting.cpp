#include "CodeEditorHandler.h"
#include "SciLexer.h"
#include "TextMateScope.h"
#include "DarkPlus.h"
#include "LightPlus.h"

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
		if (_highlightingLanguage == L"cpp" || _highlightingLanguage == L"csharp" || _highlightingLanguage == L"js" || _highlightingLanguage == L"json")
		{
			_call->SetKeyWords(5, "todo toDo Todo ToDo TODO fixme fixMe Fixme FixMe FIXME");
		}
	}

	void CodeEditorHandler::UpdateLanguageStyles()
	{
		if (_highlightingLanguage == L"cpp" || _highlightingLanguage == L"csharp" || _highlightingLanguage == L"js")
		{
			switch (_theme)
			{
			case CodeEditorTheme::Dark:
				StyleSetFore(static_cast<int>(StylesCommon::Default), DarkPlusEditorForeground);
				StyleClearCustom();

				StyleSetFore(static_cast<int>(StylesCommon::BraceLight), DarkPlusEditorForeground);

				StyleSetFore(SCE_C_COMMENT, DarkPlus2(Scope::Comment));
				StyleSetFore(SCE_C_COMMENTLINE, DarkPlus2(Scope::Comment));
				StyleSetFore(SCE_C_COMMENTDOC, DarkPlus2(Scope::Comment));
				StyleSetFore(SCE_C_NUMBER, DarkPlus2(Scope::ConstantNumeric));
				StyleSetFore(SCE_C_WORD, DarkPlus2(Scope::Keyword));
				StyleSetFore(SCE_C_STRING, DarkPlus2(Scope::String));
				StyleSetFore(SCE_C_CHARACTER, DarkPlus2(Scope::String));
				StyleSetFore(SCE_C_UUID, DarkPlus2(Scope::ConstantNumeric));
				StyleSetFore(SCE_C_PREPROCESSOR, DarkPlus2(Scope::MetaPreprocessor));
				StyleSetFore(SCE_C_OPERATOR, DarkPlus2(Scope::KeywordOperator));
				//StyleSetFore(SCE_C_IDENTIFIER, DarkPlus2(Scope::));
				StyleSetFore(SCE_C_STRINGEOL, DarkPlus2(Scope::String));
				StyleSetFore(SCE_C_VERBATIM, DarkPlus2(Scope::String));
				StyleSetFore(SCE_C_REGEX, DarkPlus2(Scope::StringRegexp));
				StyleSetFore(SCE_C_COMMENTLINEDOC, DarkPlus2(Scope::Comment));
				StyleSetFore(SCE_C_WORD2, DarkPlus2(Scope::KeywordControl));
				StyleSetFore(SCE_C_COMMENTDOCKEYWORD, DarkPlus2(Scope::Keyword));
				StyleSetFore(SCE_C_COMMENTDOCKEYWORDERROR, DarkPlus2(Scope::Comment));
				StyleSetFore(SCE_C_GLOBALCLASS, DarkPlus2(Scope::EntityNameType));
				StyleSetFore(SCE_C_STRINGRAW, DarkPlus2(Scope::String));
				StyleSetFore(SCE_C_TRIPLEVERBATIM, DarkPlus2(Scope::String));
				StyleSetFore(SCE_C_HASHQUOTEDSTRING, DarkPlus2(Scope::String));
				StyleSetFore(SCE_C_PREPROCESSORCOMMENT, DarkPlus2(Scope::Comment));
				StyleSetFore(SCE_C_PREPROCESSORCOMMENTDOC, DarkPlus2(Scope::Comment));
				StyleSetFore(SCE_C_USERLITERAL, DarkPlus2(Scope::KeywordOtherUnit));
				StyleSetFore(SCE_C_TASKMARKER, DarkPlus2(Scope::Comment));
				_call->StyleSetItalic(SCE_C_TASKMARKER, true);
				_call->StyleSetBold(SCE_C_TASKMARKER, true);
				StyleSetFore(SCE_C_ESCAPESEQUENCE, DarkPlus2(Scope::ConstantCharacterEscape));
				break;

			case CodeEditorTheme::Light:
				StyleSetFore(static_cast<int>(StylesCommon::Default), LightPlusEditorForeground);
				StyleClearCustom();

				StyleSetFore(static_cast<int>(StylesCommon::BraceLight), LightPlusEditorForeground);

				StyleSetFore(SCE_C_COMMENT, LightPlus2(Scope::Comment));
				StyleSetFore(SCE_C_COMMENTLINE, LightPlus2(Scope::Comment));
				StyleSetFore(SCE_C_COMMENTDOC, LightPlus2(Scope::Comment));
				StyleSetFore(SCE_C_NUMBER, LightPlus2(Scope::ConstantNumeric));
				StyleSetFore(SCE_C_WORD, LightPlus2(Scope::Keyword));
				StyleSetFore(SCE_C_STRING, LightPlus2(Scope::String));
				StyleSetFore(SCE_C_CHARACTER, LightPlus2(Scope::String));
				StyleSetFore(SCE_C_UUID, LightPlus2(Scope::ConstantNumeric));
				StyleSetFore(SCE_C_PREPROCESSOR, LightPlus2(Scope::MetaPreprocessor));
				StyleSetFore(SCE_C_OPERATOR, LightPlus2(Scope::KeywordOperator));
				//StyleSetFore(SCE_C_IDENTIFIER, LightPlus2(Scope::));
				StyleSetFore(SCE_C_STRINGEOL, LightPlus2(Scope::String));
				StyleSetFore(SCE_C_VERBATIM, LightPlus2(Scope::String));
				StyleSetFore(SCE_C_REGEX, LightPlus2(Scope::StringRegexp));
				StyleSetFore(SCE_C_COMMENTLINEDOC, LightPlus2(Scope::Comment));
				StyleSetFore(SCE_C_WORD2, LightPlus2(Scope::KeywordControl));
				StyleSetFore(SCE_C_COMMENTDOCKEYWORD, LightPlus2(Scope::Keyword));
				StyleSetFore(SCE_C_COMMENTDOCKEYWORDERROR, LightPlus2(Scope::Comment));
				StyleSetFore(SCE_C_GLOBALCLASS, LightPlus2(Scope::EntityNameType));
				StyleSetFore(SCE_C_STRINGRAW, LightPlus2(Scope::String));
				StyleSetFore(SCE_C_TRIPLEVERBATIM, LightPlus2(Scope::String));
				StyleSetFore(SCE_C_HASHQUOTEDSTRING, LightPlus2(Scope::String));
				StyleSetFore(SCE_C_PREPROCESSORCOMMENT, LightPlus2(Scope::Comment));
				StyleSetFore(SCE_C_PREPROCESSORCOMMENTDOC, LightPlus2(Scope::Comment));
				StyleSetFore(SCE_C_USERLITERAL, LightPlus2(Scope::KeywordOtherUnit));
				StyleSetFore(SCE_C_TASKMARKER, LightPlus2(Scope::Comment));
				_call->StyleSetItalic(SCE_C_TASKMARKER, true);
				_call->StyleSetBold(SCE_C_TASKMARKER, true);
				StyleSetFore(SCE_C_ESCAPESEQUENCE, LightPlus2(Scope::ConstantCharacterEscape));
				break;
			}
		}
		else if (_highlightingLanguage == L"json")
		{
			switch (_theme)
			{
			case CodeEditorTheme::Dark:
				StyleSetFore(static_cast<int>(StylesCommon::Default), DarkPlusEditorForeground);
				StyleClearCustom();

				StyleSetFore(static_cast<int>(StylesCommon::BraceLight), DarkPlusEditorForeground);

				StyleSetFore(SCE_JSON_NUMBER, DarkPlus2(Scope::ConstantNumeric));
				StyleSetFore(SCE_JSON_STRING, DarkPlus2(Scope::String));
				StyleSetFore(SCE_JSON_STRINGEOL, DarkPlus2(Scope::String));
				StyleSetFore(SCE_JSON_PROPERTYNAME, DarkPlus2(Scope::SupportTypeProperty_NameJson));
				StyleSetFore(SCE_JSON_ESCAPESEQUENCE, DarkPlus2(Scope::ConstantCharacterEscape));
				StyleSetFore(SCE_JSON_LINECOMMENT, DarkPlus2(Scope::Comment));
				StyleSetFore(SCE_JSON_BLOCKCOMMENT, DarkPlus2(Scope::Comment));
				StyleSetFore(SCE_JSON_OPERATOR, DarkPlus2(Scope::KeywordOperator));
				StyleSetFore(SCE_JSON_URI, DarkPlus2(Scope::String));
				_call->StyleSetUnderline(SCE_JSON_URI, true);
				StyleSetFore(SCE_JSON_COMPACTIRI, DarkPlus2(Scope::String));
				StyleSetFore(SCE_JSON_KEYWORD, DarkPlus2(Scope::ConstantLanguage));
				StyleSetFore(SCE_JSON_LDKEYWORD, DarkPlus2(Scope::KeywordControl));
				StyleSetFore(SCE_JSON_ERROR, DarkPlus2(Scope::Invalid));
				break;

			case CodeEditorTheme::Light:
				StyleSetFore(static_cast<int>(StylesCommon::Default), LightPlusEditorForeground);
				StyleClearCustom();

				StyleSetFore(static_cast<int>(StylesCommon::BraceLight), LightPlusEditorForeground);

				StyleSetFore(SCE_JSON_NUMBER, LightPlus2(Scope::ConstantNumeric));
				StyleSetFore(SCE_JSON_STRING, LightPlus2(Scope::String));
				StyleSetFore(SCE_JSON_STRINGEOL, LightPlus2(Scope::String));
				StyleSetFore(SCE_JSON_PROPERTYNAME, LightPlus2(Scope::SupportTypeProperty_NameJson));
				StyleSetFore(SCE_JSON_ESCAPESEQUENCE, LightPlus2(Scope::ConstantCharacterEscape));
				StyleSetFore(SCE_JSON_LINECOMMENT, LightPlus2(Scope::Comment));
				StyleSetFore(SCE_JSON_BLOCKCOMMENT, LightPlus2(Scope::Comment));
				StyleSetFore(SCE_JSON_OPERATOR, LightPlus2(Scope::KeywordOperator));
				StyleSetFore(SCE_JSON_URI, LightPlus2(Scope::String));
				_call->StyleSetUnderline(SCE_JSON_URI, true);
				StyleSetFore(SCE_JSON_COMPACTIRI, LightPlus2(Scope::String));
				StyleSetFore(SCE_JSON_KEYWORD, LightPlus2(Scope::ConstantLanguage));
				StyleSetFore(SCE_JSON_LDKEYWORD, LightPlus2(Scope::KeywordControl));
				StyleSetFore(SCE_JSON_ERROR, LightPlus2(Scope::Invalid));
				break;
			}
		}
		else
		{
			StyleClearCustom();
		}
	}
}
