#include "CodeEditorHandler.h"
#include "SciLexer.h"
#include "TextMateScope.h"
#include "DarkPlus.h"
#include "LightPlus.h"

using namespace Scintilla;

namespace WinUIEditor
{
	static constexpr auto WINUI_SCE_HJ_KEYWORD2{ 54 }; // Todo: Use constant in LexHTML

	void CodeEditorHandler::SetLexer()
	{
		if (_highlightingLanguage == L"cpp")
		{
			const auto lexer{ _createLexer("cpp") };
			lexer->PropertySet("fold", "1");
			_call->SetILexer(lexer);
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
			_call->SetKeyWords(2,
				"a addindex addtogroup anchor arg attention"
				"author b brief bug c class code date def defgroup deprecated dontinclude"
				"e em endcode endhtmlonly endif endlatexonly endlink endverbatim enum example exception"
				"f$ f[ f] file fn hideinitializer htmlinclude htmlonly"
				"if image include ingroup internal invariant interface latexonly li line link"
				"mainpage name namespace nosubgrouping note overload"
				"p page par param param[in] param[out]"
				"post pre ref relates remarks return retval"
				"sa section see showinitializer since skip skipline struct subsection"
				"test throw throws todo typedef union until"
				"var verbatim verbinclude version warning weakgroup $ @ \\ & < > # { }");
			SetLanguageIndentMode(
				SCE_C_WORD2, { "case", "default", "do", "else", "for", "if", "while", },
				SCE_C_OPERATOR, { ";", },
				SCE_C_OPERATOR, { "{", },
				SCE_C_OPERATOR, { "}", });
		}
		else if (_highlightingLanguage == L"csharp")
		{
			const auto lexer{ _createLexer("cpp") };
			lexer->PropertySet("fold", "1");
			_call->SetILexer(lexer);
			// This list of keywords from SciTe (cpp.properties) and https://learn.microsoft.com/en-us/dotnet/csharp/language-reference/keywords/
			_call->SetKeyWords(0,
				"abstract as ascending async await base bool by byte char checked "
				"class const decimal default delegate descending double enum "
				"equals event explicit extern false fixed float from get group "
				"implicit in int interface internal into is join lock let long nameof namespace new nint null "
				"object on operator orderby out override params partial private protected public "
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
		else if (_highlightingLanguage == L"javascript")
		{
			const auto lexer{ _createLexer("cpp") };
			lexer->PropertySet("fold", "1");
			_call->SetILexer(lexer);
			SetJavaScriptDefaults(0, 1, SCE_C_WORD2, SCE_C_OPERATOR);
		}
		else if (_highlightingLanguage == L"json")
		{
			const auto lexer{ _createLexer("json") };
			lexer->PropertySet("fold", "1");
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
		else if (_highlightingLanguage == L"xml" || _highlightingLanguage == L"html")
		{
			const auto lexer{ _createLexer(_highlightingLanguage == L"xml" ? "xml" : "hypertext") };
			lexer->PropertySet("fold", "1");
			lexer->PropertySet("fold.html", "1");
			lexer->PropertySet("winuiedit.style.tag.brackets.as.tag.end", "1");
			_call->SetILexer(lexer);
			if (_highlightingLanguage == L"html")
			{
				_call->SetKeyWords(5, "ELEMENT DOCTYPE doctype ATTLIST ENTITY NOTATION");

				SetJavaScriptDefaults(1, 6, WINUI_SCE_HJ_KEYWORD2, SCE_HJ_SYMBOLS);
			}
			else
			{
				SetLanguageIndentMode(0, { }, 0, { }, 0, { }, 0, { });
			}
		}
		else if (_highlightingLanguage == L"yaml")
		{
			const auto lexer{ _createLexer("yaml") };
			lexer->PropertySet("fold", "1");
			_call->SetILexer(lexer);
			_call->SetKeyWords(0, "true True TRUE false False FALSE yes Yes YES no No NO null Null NULL");
			SetLanguageIndentMode(0, { }, 0, { }, 0, { }, 0, { });
		}
		else if (_highlightingLanguage == L"plaintext")
		{
			_call->SetILexer(_createLexer("null"));
			SetLanguageIndentMode(0, { }, 0, { }, 0, { }, 0, { });
		}
		else
		{
			_call->SetILexer(nullptr);
			SetLanguageIndentMode(0, { }, 0, { }, 0, { }, 0, { });
		}
		if (_highlightingLanguage == L"cpp" || _highlightingLanguage == L"csharp" || _highlightingLanguage == L"javascript")
		{
			_call->SetKeyWords(5, "todo toDo Todo ToDo TODO fixme fixMe Fixme FixMe FIXME");
		}
	}

	void CodeEditorHandler::SetJavaScriptDefaults(int wordList1, int wordList2, int indentKeywordStyle, int symbolStyle)
	{
		// This list of keywords from MDN https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Lexical_grammar#keywords
		// Note additional words like undefined
		_call->SetKeyWords(wordList1,
			"class const debugger delete export extends false function in instanceof "
			"new null super this true typeof var void let static enum implements "
			"interface private protected public arguments async get of set Infinity NaN undefined");
		_call->SetKeyWords(wordList2,
			"break case catch continue default do else finally for if import return "
			"switch throw try while with yield await package as from");
		SetLanguageIndentMode(
			indentKeywordStyle, { "case", "default", "do", "else", "for", "if", "while", },
			symbolStyle, { ";", },
			symbolStyle, { "{", "[" },
			symbolStyle, { "}", "]" });
	}

	void CodeEditorHandler::UpdateLanguageStyles()
	{
		if (_highlightingLanguage == L"cpp" || _highlightingLanguage == L"csharp" || _highlightingLanguage == L"javascript")
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
				if (_highlightingLanguage != L"cpp")
				{
					StyleSetFore(SCE_C_IDENTIFIER, DarkPlus2(Scope::Variable));
				}
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
				if (_highlightingLanguage != L"cpp")
				{
					StyleSetFore(SCE_C_IDENTIFIER, LightPlus2(Scope::Variable));
				}
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
		else if (_highlightingLanguage == L"xml" || _highlightingLanguage == L"html")
		{
			const auto isHtml{ _highlightingLanguage == L"html" };
			switch (_theme)
			{
			case CodeEditorTheme::Dark:
				StyleSetFore(static_cast<int>(StylesCommon::Default), DarkPlusEditorForeground);
				StyleClearCustom();

				StyleSetFore(static_cast<int>(StylesCommon::BraceLight), DarkPlusEditorForeground);

				StyleSetFore(SCE_H_TAG, DarkPlus2(Scope::EntityNameTag));
				StyleSetFore(SCE_H_TAGUNKNOWN, DarkPlus2(Scope::EntityNameTag));
				StyleSetFore(SCE_H_ATTRIBUTE, DarkPlus2(Scope::EntityOtherAttribute_Name));
				StyleSetFore(SCE_H_ATTRIBUTEUNKNOWN, DarkPlus2(Scope::EntityOtherAttribute_Name));
				StyleSetFore(SCE_H_NUMBER, DarkPlus2(Scope::ConstantNumeric));
				StyleSetFore(SCE_H_DOUBLESTRING, DarkPlus2(Scope::String));
				StyleSetFore(SCE_H_SINGLESTRING, DarkPlus2(Scope::String));
				// ...
				StyleSetFore(SCE_H_COMMENT, DarkPlus2(Scope::Comment));
				StyleSetFore(SCE_H_ENTITY, DarkPlus2(Scope::ConstantCharacter));
				StyleSetFore(SCE_H_TAGEND, DarkPlus2(Scope::PunctuationDefinitionTag));
				StyleSetFore(SCE_H_XMLSTART, DarkPlus2(Scope::PunctuationDefinitionTag));
				StyleSetFore(SCE_H_XMLEND, DarkPlus2(Scope::PunctuationDefinitionTag));
				// ...
				StyleSetFore(SCE_H_CDATA, DarkPlus2(Scope::String));
				StyleSetFore(SCE_H_SGML_DEFAULT, DarkPlus2(Scope::PunctuationDefinitionTag));
				StyleSetFore(SCE_H_SGML_COMMAND, DarkPlus2(Scope::EntityNameTag));
				StyleSetFore(SCE_H_SGML_1ST_PARAM, DarkPlus2(Scope::EntityOtherAttribute_Name));
				StyleSetFore(SCE_H_SGML_DOUBLESTRING, DarkPlus2(Scope::String));
				// ...
				StyleSetFore(SCE_H_SGML_COMMENT, DarkPlus2(Scope::Comment));
				// ...
				if (isHtml)
				{
					StyleSetFore(SCE_H_VALUE, DarkPlus2(Scope::String));

					StyleSetFore(SCE_HJ_COMMENT, DarkPlus2(Scope::Comment));
					StyleSetFore(SCE_HJ_COMMENTLINE, DarkPlus2(Scope::Comment));
					StyleSetFore(SCE_HJ_COMMENTDOC, DarkPlus2(Scope::Comment));
					StyleSetFore(SCE_HJ_NUMBER, DarkPlus2(Scope::ConstantNumeric));
					StyleSetFore(SCE_HJ_WORD, DarkPlus2(Scope::Variable));
					StyleSetFore(SCE_HJ_KEYWORD, DarkPlus2(Scope::Keyword));
					StyleSetFore(WINUI_SCE_HJ_KEYWORD2, DarkPlus2(Scope::KeywordControl));
					StyleSetFore(SCE_HJ_DOUBLESTRING, DarkPlus2(Scope::String));
					StyleSetFore(SCE_HJ_SINGLESTRING, DarkPlus2(Scope::String));
					StyleSetFore(SCE_HJ_SYMBOLS, DarkPlus2(Scope::KeywordOperator));
					StyleSetFore(SCE_HJ_STRINGEOL, DarkPlus2(Scope::String));
					StyleSetFore(SCE_HJ_REGEX, DarkPlus2(Scope::StringRegexp));
				}
				break;

			case CodeEditorTheme::Light:
				StyleSetFore(static_cast<int>(StylesCommon::Default), LightPlusEditorForeground);
				StyleClearCustom();

				StyleSetFore(static_cast<int>(StylesCommon::BraceLight), LightPlusEditorForeground);

				StyleSetFore(SCE_H_TAG, LightPlus2(Scope::EntityNameTag));
				StyleSetFore(SCE_H_TAGUNKNOWN, LightPlus2(Scope::EntityNameTag));
				StyleSetFore(SCE_H_ATTRIBUTE, LightPlus2(Scope::EntityOtherAttribute_Name));
				StyleSetFore(SCE_H_ATTRIBUTEUNKNOWN, LightPlus2(Scope::EntityOtherAttribute_Name));
				StyleSetFore(SCE_H_NUMBER, LightPlus2(Scope::ConstantNumeric));
				StyleSetFore(SCE_H_DOUBLESTRING, isHtml ? LightPlus2(Scope::StringQuotedDoubleHtml) : LightPlus2(Scope::StringQuotedDoubleXml));
				StyleSetFore(SCE_H_SINGLESTRING, isHtml ? LightPlus2(Scope::StringQuotedSingleHtml) : LightPlus2(Scope::StringQuotedSingleXml));
				// ...
				StyleSetFore(SCE_H_COMMENT, LightPlus2(Scope::Comment));
				StyleSetFore(SCE_H_ENTITY, LightPlus2(Scope::ConstantCharacter));
				StyleSetFore(SCE_H_TAGEND, LightPlus2(Scope::PunctuationDefinitionTag));
				StyleSetFore(SCE_H_XMLSTART, LightPlus2(Scope::PunctuationDefinitionTag));
				StyleSetFore(SCE_H_XMLEND, LightPlus2(Scope::PunctuationDefinitionTag));
				// ...
				StyleSetFore(SCE_H_CDATA, isHtml ? LightPlus2(Scope::String) : LightPlus2(Scope::StringUnquotedCdataXml));
				StyleSetFore(SCE_H_SGML_DEFAULT, LightPlus2(Scope::PunctuationDefinitionTag));
				StyleSetFore(SCE_H_SGML_COMMAND, LightPlus2(Scope::EntityNameTag));
				StyleSetFore(SCE_H_SGML_1ST_PARAM, LightPlus2(Scope::EntityOtherAttribute_Name));
				StyleSetFore(SCE_H_SGML_DOUBLESTRING, LightPlus2(Scope::StringQuotedDoubleHtml));
				// ...
				StyleSetFore(SCE_H_SGML_COMMENT, LightPlus2(Scope::Comment));
				// ...
				if (isHtml)
				{
					StyleSetFore(SCE_H_VALUE, LightPlus2(Scope::StringUnquotedHtml));

					StyleSetFore(SCE_HJ_COMMENT, LightPlus2(Scope::Comment));
					StyleSetFore(SCE_HJ_COMMENTLINE, LightPlus2(Scope::Comment));
					StyleSetFore(SCE_HJ_COMMENTDOC, LightPlus2(Scope::Comment));
					StyleSetFore(SCE_HJ_NUMBER, LightPlus2(Scope::ConstantNumeric));
					StyleSetFore(SCE_HJ_WORD, LightPlus2(Scope::Variable));
					StyleSetFore(SCE_HJ_KEYWORD, LightPlus2(Scope::Keyword));
					StyleSetFore(WINUI_SCE_HJ_KEYWORD2, LightPlus2(Scope::KeywordControl));
					StyleSetFore(SCE_HJ_DOUBLESTRING, LightPlus2(Scope::String));
					StyleSetFore(SCE_HJ_SINGLESTRING, LightPlus2(Scope::String));
					StyleSetFore(SCE_HJ_SYMBOLS, LightPlus2(Scope::KeywordOperator));
					StyleSetFore(SCE_HJ_STRINGEOL, LightPlus2(Scope::String));
					StyleSetFore(SCE_HJ_REGEX, LightPlus2(Scope::StringRegexp));
				}
				break;
			}
		}
		else if (_highlightingLanguage == L"yaml")
		{
			switch (_theme)
			{
			case CodeEditorTheme::Dark:
				StyleSetFore(static_cast<int>(StylesCommon::Default), DarkPlusEditorForeground);
				StyleClearCustom();

				StyleSetFore(SCE_YAML_DEFAULT, DarkPlus2(Scope::String));
				StyleSetFore(SCE_YAML_COMMENT, DarkPlus2(Scope::Comment));
				StyleSetFore(SCE_YAML_IDENTIFIER, DarkPlus2(Scope::EntityNameTag));
				StyleSetFore(SCE_YAML_KEYWORD, DarkPlus2(Scope::ConstantLanguage));
				StyleSetFore(SCE_YAML_NUMBER, DarkPlus2(Scope::ConstantNumeric));
				StyleSetFore(SCE_YAML_REFERENCE, DarkPlus2(Scope::Variable));
				StyleSetFore(SCE_YAML_DOCUMENT, DarkPlusEditorForeground);
				StyleSetFore(SCE_YAML_TEXT, DarkPlus2(Scope::String));
				StyleSetFore(SCE_YAML_ERROR, DarkPlus2(Scope::Invalid));
				StyleSetFore(SCE_YAML_OPERATOR, DarkPlusEditorForeground);
				break;

			case CodeEditorTheme::Light:
				StyleSetFore(static_cast<int>(StylesCommon::Default), LightPlusEditorForeground);
				StyleClearCustom();

				// Scopes are slightly different to improve legibility of properties
				// Todo: Unify scopes to be correct once a real theming system is made
				StyleSetFore(SCE_YAML_DEFAULT, LightPlus2(Scope::String));
				StyleSetFore(SCE_YAML_COMMENT, LightPlus2(Scope::Comment));
				StyleSetFore(SCE_YAML_IDENTIFIER, LightPlus2(Scope::SupportConstantProperty_Value));
				StyleSetFore(SCE_YAML_KEYWORD, LightPlus2(Scope::ConstantLanguage));
				StyleSetFore(SCE_YAML_NUMBER, LightPlus2(Scope::ConstantNumeric));
				StyleSetFore(SCE_YAML_REFERENCE, LightPlus2(Scope::Variable));
				StyleSetFore(SCE_YAML_DOCUMENT, LightPlusEditorForeground);
				StyleSetFore(SCE_YAML_TEXT, LightPlus2(Scope::String));
				StyleSetFore(SCE_YAML_ERROR, LightPlus2(Scope::Invalid));
				StyleSetFore(SCE_YAML_OPERATOR, LightPlusEditorForeground);
				break;
			}
		}
		else
		{
			StyleClearCustom();
		}
	}
}
