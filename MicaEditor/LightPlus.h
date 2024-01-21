// Ported from
// https://github.com/microsoft/vscode/blob/main/extensions/theme-defaults/themes/light_plus.json
// https://github.com/microsoft/vscode/blob/main/extensions/theme-defaults/themes/light_vs.json
// under the following license

/*
	MIT License

	Copyright (c) 2015 - present Microsoft Corporation

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#pragma once

namespace WinUIEditor
{
	constexpr int LightPlusEditorForeground{ IntRGBA(0x00, 0x00, 0x00) };

	constexpr int LightPlus(Scope scope)
	{
		switch (scope)
		{
		case Scope::MetaEmbedded: return IntRGBA(0x00, 0x00, 0x00);
		case Scope::SourceGroovyEmbedded: return IntRGBA(0x00, 0x00, 0x00);
		case Scope::String__MetaImageInlineMarkdown: return IntRGBA(0x00, 0x00, 0x00);
		case Scope::VariableLegacyBuiltinPython: return IntRGBA(0x00, 0x00, 0x00);
		case Scope::MetaDiffHeader: return IntRGBA(0x00, 0x00, 0x80);
		case Scope::Comment: return IntRGBA(0x00, 0x80, 0x00);
		case Scope::ConstantLanguage: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::ConstantNumeric: return IntRGBA(0x09, 0x86, 0x58);
		case Scope::VariableOtherEnummember: return IntRGBA(0x00, 0x70, 0xC1);
		case Scope::KeywordOperatorPlusExponent: return IntRGBA(0x09, 0x86, 0x58);
		case Scope::KeywordOperatorMinusExponent: return IntRGBA(0x09, 0x86, 0x58);
		case Scope::ConstantRegexp: return IntRGBA(0x81, 0x1F, 0x3F);
		case Scope::EntityNameTag: return IntRGBA(0x80, 0x00, 0x00);
		case Scope::EntityNameSelector: return IntRGBA(0x80, 0x00, 0x00);
		case Scope::EntityOtherAttribute_Name: return IntRGBA(0xE5, 0x00, 0x00);
		case Scope::EntityOtherAttribute_NameClassCss: return IntRGBA(0x80, 0x00, 0x00);
		case Scope::EntityOtherAttribute_NameClassMixinCss: return IntRGBA(0x80, 0x00, 0x00);
		case Scope::EntityOtherAttribute_NameIdCss: return IntRGBA(0x80, 0x00, 0x00);
		case Scope::EntityOtherAttribute_NameParent_SelectorCss: return IntRGBA(0x80, 0x00, 0x00);
		case Scope::EntityOtherAttribute_NamePseudo_ClassCss: return IntRGBA(0x80, 0x00, 0x00);
		case Scope::EntityOtherAttribute_NamePseudo_ElementCss: return IntRGBA(0x80, 0x00, 0x00);
		case Scope::SourceCssLess__EntityOtherAttribute_NameId: return IntRGBA(0x80, 0x00, 0x00);
		case Scope::EntityOtherAttribute_NameScss: return IntRGBA(0x80, 0x00, 0x00);
		case Scope::Invalid: return IntRGBA(0xCD, 0x31, 0x31);
		case Scope::MarkupBold: return IntRGBA(0x00, 0x00, 0x80);
		case Scope::MarkupHeading: return IntRGBA(0x80, 0x00, 0x00);
		case Scope::MarkupInserted: return IntRGBA(0x09, 0x86, 0x58);
		case Scope::MarkupDeleted: return IntRGBA(0xA3, 0x15, 0x15);
		case Scope::MarkupChanged: return IntRGBA(0x04, 0x51, 0xA5);
		case Scope::PunctuationDefinitionQuoteBeginMarkdown: return IntRGBA(0x04, 0x51, 0xA5);
		case Scope::PunctuationDefinitionListBeginMarkdown: return IntRGBA(0x04, 0x51, 0xA5);
		case Scope::MarkupInlineRaw: return IntRGBA(0x80, 0x00, 0x00);
		case Scope::PunctuationDefinitionTag: return IntRGBA(0x80, 0x00, 0x00);
		case Scope::MetaPreprocessor: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::EntityNameFunctionPreprocessor: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::MetaPreprocessorString: return IntRGBA(0xA3, 0x15, 0x15);
		case Scope::MetaPreprocessorNumeric: return IntRGBA(0x09, 0x86, 0x58);
		case Scope::MetaStructureDictionaryKeyPython: return IntRGBA(0x04, 0x51, 0xA5);
		case Scope::Storage: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::StorageType: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::StorageModifier: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::KeywordOperatorNoexcept: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::String: return IntRGBA(0xA3, 0x15, 0x15);
		case Scope::MetaEmbeddedAssembly: return IntRGBA(0xA3, 0x15, 0x15);
		case Scope::StringCommentBufferedBlockPug: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::StringQuotedPug: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::StringInterpolatedPug: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::StringUnquotedPlainInYaml: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::StringUnquotedPlainOutYaml: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::StringUnquotedBlockYaml: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::StringQuotedSingleYaml: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::StringQuotedDoubleXml: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::StringQuotedSingleXml: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::StringUnquotedCdataXml: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::StringQuotedDoubleHtml: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::StringQuotedSingleHtml: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::StringUnquotedHtml: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::StringQuotedSingleHandlebars: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::StringQuotedDoubleHandlebars: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::StringRegexp: return IntRGBA(0x81, 0x1F, 0x3F);
		case Scope::PunctuationDefinitionTemplate_ExpressionBegin: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::PunctuationDefinitionTemplate_ExpressionEnd: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::PunctuationSectionEmbedded: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::MetaTemplateExpression: return IntRGBA(0x00, 0x00, 0x00);
		case Scope::SupportConstantProperty_Value: return IntRGBA(0x04, 0x51, 0xA5);
		case Scope::SupportConstantFont_Name: return IntRGBA(0x04, 0x51, 0xA5);
		case Scope::SupportConstantMedia_Type: return IntRGBA(0x04, 0x51, 0xA5);
		case Scope::SupportConstantMedia: return IntRGBA(0x04, 0x51, 0xA5);
		case Scope::ConstantOtherColorRgb_Value: return IntRGBA(0x04, 0x51, 0xA5);
		case Scope::ConstantOtherRgb_Value: return IntRGBA(0x04, 0x51, 0xA5);
		case Scope::SupportConstantColor: return IntRGBA(0x04, 0x51, 0xA5);
		case Scope::SupportTypeVendoredProperty_Name: return IntRGBA(0xE5, 0x00, 0x00);
		case Scope::SupportTypeProperty_Name: return IntRGBA(0xE5, 0x00, 0x00);
		case Scope::VariableCss: return IntRGBA(0xE5, 0x00, 0x00);
		case Scope::VariableScss: return IntRGBA(0xE5, 0x00, 0x00);
		case Scope::VariableOtherLess: return IntRGBA(0xE5, 0x00, 0x00);
		case Scope::SourceCoffeeEmbedded: return IntRGBA(0xE5, 0x00, 0x00);
		case Scope::SupportTypeProperty_NameJson: return IntRGBA(0x04, 0x51, 0xA5);
		case Scope::Keyword: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::KeywordControl: return IntRGBA(0xAF, 0x00, 0xDB);
		case Scope::KeywordOperator: return IntRGBA(0x00, 0x00, 0x00);
		case Scope::KeywordOperatorNew: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::KeywordOperatorExpression: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::KeywordOperatorCast: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::KeywordOperatorSizeof: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::KeywordOperatorAlignof: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::KeywordOperatorTypeid: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::KeywordOperatorAlignas: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::KeywordOperatorInstanceof: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::KeywordOperatorLogicalPython: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::KeywordOperatorWordlike: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::KeywordOtherUnit: return IntRGBA(0x09, 0x86, 0x58);
		case Scope::PunctuationSectionEmbeddedBeginPhp: return IntRGBA(0x80, 0x00, 0x00);
		case Scope::PunctuationSectionEmbeddedEndPhp: return IntRGBA(0x80, 0x00, 0x00);
		case Scope::SupportFunctionGit_Rebase: return IntRGBA(0x04, 0x51, 0xA5);
		case Scope::ConstantShaGit_Rebase: return IntRGBA(0x09, 0x86, 0x58);
		case Scope::StorageModifierImportJava: return IntRGBA(0x00, 0x00, 0x00);
		case Scope::VariableLanguageWildcardJava: return IntRGBA(0x00, 0x00, 0x00);
		case Scope::StorageModifierPackageJava: return IntRGBA(0x00, 0x00, 0x00);
		case Scope::VariableLanguage: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::EntityNameFunction: return IntRGBA(0x79, 0x5E, 0x26);
		case Scope::SupportFunction: return IntRGBA(0x79, 0x5E, 0x26);
		case Scope::SupportConstantHandlebars: return IntRGBA(0x79, 0x5E, 0x26);
		case Scope::SourcePowershell__VariableOtherMember: return IntRGBA(0x79, 0x5E, 0x26);
		case Scope::EntityNameOperatorCustom_Literal: return IntRGBA(0x79, 0x5E, 0x26);
		case Scope::SupportClass: return IntRGBA(0x26, 0x7F, 0x99);
		case Scope::SupportType: return IntRGBA(0x26, 0x7F, 0x99);
		case Scope::EntityNameType: return IntRGBA(0x26, 0x7F, 0x99);
		case Scope::EntityNameNamespace: return IntRGBA(0x26, 0x7F, 0x99);
		case Scope::EntityOtherAttribute: return IntRGBA(0x26, 0x7F, 0x99);
		case Scope::EntityNameScope_Resolution: return IntRGBA(0x26, 0x7F, 0x99);
		case Scope::EntityNameClass: return IntRGBA(0x26, 0x7F, 0x99);
		case Scope::StorageTypeNumericGo: return IntRGBA(0x26, 0x7F, 0x99);
		case Scope::StorageTypeByteGo: return IntRGBA(0x26, 0x7F, 0x99);
		case Scope::StorageTypeBooleanGo: return IntRGBA(0x26, 0x7F, 0x99);
		case Scope::StorageTypeStringGo: return IntRGBA(0x26, 0x7F, 0x99);
		case Scope::StorageTypeUintptrGo: return IntRGBA(0x26, 0x7F, 0x99);
		case Scope::StorageTypeErrorGo: return IntRGBA(0x26, 0x7F, 0x99);
		case Scope::StorageTypeRuneGo: return IntRGBA(0x26, 0x7F, 0x99);
		case Scope::StorageTypeCs: return IntRGBA(0x26, 0x7F, 0x99);
		case Scope::StorageTypeGenericCs: return IntRGBA(0x26, 0x7F, 0x99);
		case Scope::StorageTypeModifierCs: return IntRGBA(0x26, 0x7F, 0x99);
		case Scope::StorageTypeVariableCs: return IntRGBA(0x26, 0x7F, 0x99);
		case Scope::StorageTypeAnnotationJava: return IntRGBA(0x26, 0x7F, 0x99);
		case Scope::StorageTypeGenericJava: return IntRGBA(0x26, 0x7F, 0x99);
		case Scope::StorageTypeJava: return IntRGBA(0x26, 0x7F, 0x99);
		case Scope::StorageTypeObjectArrayJava: return IntRGBA(0x26, 0x7F, 0x99);
		case Scope::StorageTypePrimitiveArrayJava: return IntRGBA(0x26, 0x7F, 0x99);
		case Scope::StorageTypePrimitiveJava: return IntRGBA(0x26, 0x7F, 0x99);
		case Scope::StorageTypeTokenJava: return IntRGBA(0x26, 0x7F, 0x99);
		case Scope::StorageTypeGroovy: return IntRGBA(0x26, 0x7F, 0x99);
		case Scope::StorageTypeAnnotationGroovy: return IntRGBA(0x26, 0x7F, 0x99);
		case Scope::StorageTypeParametersGroovy: return IntRGBA(0x26, 0x7F, 0x99);
		case Scope::StorageTypeGenericGroovy: return IntRGBA(0x26, 0x7F, 0x99);
		case Scope::StorageTypeObjectArrayGroovy: return IntRGBA(0x26, 0x7F, 0x99);
		case Scope::StorageTypePrimitiveArrayGroovy: return IntRGBA(0x26, 0x7F, 0x99);
		case Scope::StorageTypePrimitiveGroovy: return IntRGBA(0x26, 0x7F, 0x99);
		case Scope::MetaTypeCastExpr: return IntRGBA(0x26, 0x7F, 0x99);
		case Scope::MetaTypeNewExpr: return IntRGBA(0x26, 0x7F, 0x99);
		case Scope::SupportConstantMath: return IntRGBA(0x26, 0x7F, 0x99);
		case Scope::SupportConstantDom: return IntRGBA(0x26, 0x7F, 0x99);
		case Scope::SupportConstantJson: return IntRGBA(0x26, 0x7F, 0x99);
		case Scope::EntityOtherInherited_Class: return IntRGBA(0x26, 0x7F, 0x99);
		case Scope::SourceCpp__KeywordOperatorNew: return IntRGBA(0xAF, 0x00, 0xDB);
		case Scope::SourceCpp__KeywordOperatorDelete: return IntRGBA(0xAF, 0x00, 0xDB);
		case Scope::KeywordOtherUsing: return IntRGBA(0xAF, 0x00, 0xDB);
		case Scope::KeywordOtherDirectiveUsing: return IntRGBA(0xAF, 0x00, 0xDB);
		case Scope::KeywordOtherOperator: return IntRGBA(0xAF, 0x00, 0xDB);
		case Scope::EntityNameOperator: return IntRGBA(0xAF, 0x00, 0xDB);
		case Scope::Variable: return IntRGBA(0x00, 0x10, 0x80);
		case Scope::MetaDefinitionVariableName: return IntRGBA(0x00, 0x10, 0x80);
		case Scope::SupportVariable: return IntRGBA(0x00, 0x10, 0x80);
		case Scope::EntityNameVariable: return IntRGBA(0x00, 0x10, 0x80);
		case Scope::ConstantOtherPlaceholder: return IntRGBA(0x00, 0x10, 0x80);
		case Scope::VariableOtherConstant: return IntRGBA(0x00, 0x70, 0xC1);
		case Scope::MetaObject_LiteralKey: return IntRGBA(0x00, 0x10, 0x80);
		case Scope::PunctuationDefinitionGroupRegexp: return IntRGBA(0xD1, 0x69, 0x69);
		case Scope::PunctuationDefinitionGroupAssertionRegexp: return IntRGBA(0xD1, 0x69, 0x69);
		case Scope::PunctuationDefinitionCharacter_ClassRegexp: return IntRGBA(0xD1, 0x69, 0x69);
		case Scope::PunctuationCharacterSetBeginRegexp: return IntRGBA(0xD1, 0x69, 0x69);
		case Scope::PunctuationCharacterSetEndRegexp: return IntRGBA(0xD1, 0x69, 0x69);
		case Scope::KeywordOperatorNegationRegexp: return IntRGBA(0xD1, 0x69, 0x69);
		case Scope::SupportOtherParenthesisRegexp: return IntRGBA(0xD1, 0x69, 0x69);
		case Scope::ConstantCharacterCharacter_ClassRegexp: return IntRGBA(0x81, 0x1F, 0x3F);
		case Scope::ConstantOtherCharacter_ClassSetRegexp: return IntRGBA(0x81, 0x1F, 0x3F);
		case Scope::ConstantOtherCharacter_ClassRegexp: return IntRGBA(0x81, 0x1F, 0x3F);
		case Scope::ConstantCharacterSetRegexp: return IntRGBA(0x81, 0x1F, 0x3F);
		case Scope::KeywordOperatorQuantifierRegexp: return IntRGBA(0x00, 0x00, 0x00);
		case Scope::KeywordOperatorOrRegexp: return IntRGBA(0xEE, 0x00, 0x00);
		case Scope::KeywordControlAnchorRegexp: return IntRGBA(0xEE, 0x00, 0x00);
		case Scope::ConstantCharacter: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::ConstantOtherOption: return IntRGBA(0x00, 0x00, 0xFF);
		case Scope::ConstantCharacterEscape: return IntRGBA(0xEE, 0x00, 0x00);
		case Scope::EntityNameLabel: return IntRGBA(0x00, 0x00, 0x00);
		}
	}

	constexpr int LightPlus2(Scope scope)
	{
		switch (scope)
		{
		case Scope::MetaPreprocessor: return IntRGBA(0x80, 0x80, 0x80);
		default: return LightPlus(scope);
		}
	}
}
