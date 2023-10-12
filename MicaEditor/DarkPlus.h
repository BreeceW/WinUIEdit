// Ported from
// https://github.com/microsoft/vscode/blob/main/extensions/theme-defaults/themes/dark_plus.json
// https://github.com/microsoft/vscode/blob/main/extensions/theme-defaults/themes/dark_vs.json
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

namespace MicaEditor
{
	constexpr int DarkPlusEditorForeground{ IntRGBA(0xD4, 0xD4, 0xD4) };

	constexpr int DarkPlus(Scope scope)
	{
		switch (scope)
		{
		case Scope::MetaEmbedded: return IntRGBA(0xD4, 0xD4, 0xD4);
		case Scope::SourceGroovyEmbedded: return IntRGBA(0xD4, 0xD4, 0xD4);
		case Scope::String__MetaImageInlineMarkdown: return IntRGBA(0xD4, 0xD4, 0xD4);
		case Scope::VariableLegacyBuiltinPython: return IntRGBA(0xD4, 0xD4, 0xD4);
		case Scope::Header: return IntRGBA(0x00, 0x00, 0x80);
		case Scope::Comment: return IntRGBA(0x6A, 0x99, 0x55);
		case Scope::ConstantLanguage: return IntRGBA(0x56, 0x9C, 0xD6);
		case Scope::ConstantNumeric: return IntRGBA(0xB5, 0xCE, 0xA8);
		case Scope::VariableOtherEnummember: return IntRGBA(0x4F, 0xC1, 0xFF);
		case Scope::KeywordOperatorPlusExponent: return IntRGBA(0xB5, 0xCE, 0xA8);
		case Scope::KeywordOperatorMinusExponent: return IntRGBA(0xB5, 0xCE, 0xA8);
		case Scope::ConstantRegexp: return IntRGBA(0x64, 0x66, 0x95);
		case Scope::EntityNameTag: return IntRGBA(0x56, 0x9C, 0xD6);
		case Scope::EntityNameTagCss: return IntRGBA(0xD7, 0xBA, 0x7D);
		case Scope::EntityOtherAttribute_Name: return IntRGBA(0x9C, 0xDC, 0xFE);
		case Scope::EntityOtherAttribute_NameClassCss: return IntRGBA(0xD7, 0xBA, 0x7D);
		case Scope::EntityOtherAttribute_NameClassMixinCss: return IntRGBA(0xD7, 0xBA, 0x7D);
		case Scope::EntityOtherAttribute_NameIdCss: return IntRGBA(0xD7, 0xBA, 0x7D);
		case Scope::EntityOtherAttribute_NameParent_SelectorCss: return IntRGBA(0xD7, 0xBA, 0x7D);
		case Scope::EntityOtherAttribute_NamePseudo_ClassCss: return IntRGBA(0xD7, 0xBA, 0x7D);
		case Scope::EntityOtherAttribute_NamePseudo_ElementCss: return IntRGBA(0xD7, 0xBA, 0x7D);
		case Scope::SourceCssLess__EntityOtherAttribute_NameId: return IntRGBA(0xD7, 0xBA, 0x7D);
		case Scope::EntityOtherAttribute_NameScss: return IntRGBA(0xD7, 0xBA, 0x7D);
		case Scope::Invalid: return IntRGBA(0xF4, 0x47, 0x47);
		case Scope::MarkupBold: return IntRGBA(0x56, 0x9C, 0xD6);
		case Scope::MarkupHeading: return IntRGBA(0x56, 0x9C, 0xD6);
		case Scope::MarkupInserted: return IntRGBA(0xB5, 0xCE, 0xA8);
		case Scope::MarkupDeleted: return IntRGBA(0xCE, 0x91, 0x78);
		case Scope::MarkupChanged: return IntRGBA(0x56, 0x9C, 0xD6);
		case Scope::PunctuationDefinitionQuoteBeginMarkdown: return IntRGBA(0x6A, 0x99, 0x55);
		case Scope::PunctuationDefinitionListBeginMarkdown: return IntRGBA(0x67, 0x96, 0xE6);
		case Scope::MarkupInlineRaw: return IntRGBA(0xCE, 0x91, 0x78);
		case Scope::PunctuationDefinitionTag: return IntRGBA(0x80, 0x80, 0x80);
		case Scope::MetaPreprocessor: return IntRGBA(0x56, 0x9C, 0xD6);
		case Scope::EntityNameFunctionPreprocessor: return IntRGBA(0x56, 0x9C, 0xD6);
		case Scope::MetaPreprocessorString: return IntRGBA(0xCE, 0x91, 0x78);
		case Scope::MetaPreprocessorNumeric: return IntRGBA(0xB5, 0xCE, 0xA8);
		case Scope::MetaStructureDictionaryKeyPython: return IntRGBA(0x9C, 0xDC, 0xFE);
		case Scope::MetaDiffHeader: return IntRGBA(0x56, 0x9C, 0xD6);
		case Scope::Storage: return IntRGBA(0x56, 0x9C, 0xD6);
		case Scope::StorageType: return IntRGBA(0x56, 0x9C, 0xD6);
		case Scope::StorageModifier: return IntRGBA(0x56, 0x9C, 0xD6);
		case Scope::KeywordOperatorNoexcept: return IntRGBA(0x56, 0x9C, 0xD6);
		case Scope::String: return IntRGBA(0xCE, 0x91, 0x78);
		case Scope::MetaEmbeddedAssembly: return IntRGBA(0xCE, 0x91, 0x78);
		case Scope::StringTag: return IntRGBA(0xCE, 0x91, 0x78);
		case Scope::StringValue: return IntRGBA(0xCE, 0x91, 0x78);
		case Scope::StringRegexp: return IntRGBA(0xD1, 0x69, 0x69);
		case Scope::PunctuationDefinitionTemplate_ExpressionBegin: return IntRGBA(0x56, 0x9C, 0xD6);
		case Scope::PunctuationDefinitionTemplate_ExpressionEnd: return IntRGBA(0x56, 0x9C, 0xD6);
		case Scope::PunctuationSectionEmbedded: return IntRGBA(0x56, 0x9C, 0xD6);
		case Scope::MetaTemplateExpression: return IntRGBA(0xD4, 0xD4, 0xD4);
		case Scope::SupportTypeVendoredProperty_Name: return IntRGBA(0x9C, 0xDC, 0xFE);
		case Scope::SupportTypeProperty_Name: return IntRGBA(0x9C, 0xDC, 0xFE);
		case Scope::VariableCss: return IntRGBA(0x9C, 0xDC, 0xFE);
		case Scope::VariableScss: return IntRGBA(0x9C, 0xDC, 0xFE);
		case Scope::VariableOtherLess: return IntRGBA(0x9C, 0xDC, 0xFE);
		case Scope::SourceCoffeeEmbedded: return IntRGBA(0x9C, 0xDC, 0xFE);
		case Scope::Keyword: return IntRGBA(0x56, 0x9C, 0xD6);
		case Scope::KeywordControl: return IntRGBA(0xC5, 0x86, 0xC0);
		case Scope::KeywordOperator: return IntRGBA(0xD4, 0xD4, 0xD4);
		case Scope::KeywordOperatorNew: return IntRGBA(0x56, 0x9C, 0xD6);
		case Scope::KeywordOperatorExpression: return IntRGBA(0x56, 0x9C, 0xD6);
		case Scope::KeywordOperatorCast: return IntRGBA(0x56, 0x9C, 0xD6);
		case Scope::KeywordOperatorSizeof: return IntRGBA(0x56, 0x9C, 0xD6);
		case Scope::KeywordOperatorAlignof: return IntRGBA(0x56, 0x9C, 0xD6);
		case Scope::KeywordOperatorTypeid: return IntRGBA(0x56, 0x9C, 0xD6);
		case Scope::KeywordOperatorAlignas: return IntRGBA(0x56, 0x9C, 0xD6);
		case Scope::KeywordOperatorInstanceof: return IntRGBA(0x56, 0x9C, 0xD6);
		case Scope::KeywordOperatorLogicalPython: return IntRGBA(0x56, 0x9C, 0xD6);
		case Scope::KeywordOperatorWordlike: return IntRGBA(0x56, 0x9C, 0xD6);
		case Scope::KeywordOtherUnit: return IntRGBA(0xB5, 0xCE, 0xA8);
		case Scope::PunctuationSectionEmbeddedBeginPhp: return IntRGBA(0x56, 0x9C, 0xD6);
		case Scope::PunctuationSectionEmbeddedEndPhp: return IntRGBA(0x56, 0x9C, 0xD6);
		case Scope::SupportFunctionGit_Rebase: return IntRGBA(0x9C, 0xDC, 0xFE);
		case Scope::ConstantShaGit_Rebase: return IntRGBA(0xB5, 0xCE, 0xA8);
		case Scope::StorageModifierImportJava: return IntRGBA(0xD4, 0xD4, 0xD4);
		case Scope::VariableLanguageWildcardJava: return IntRGBA(0xD4, 0xD4, 0xD4);
		case Scope::StorageModifierPackageJava: return IntRGBA(0xD4, 0xD4, 0xD4);
		case Scope::VariableLanguage: return IntRGBA(0x56, 0x9C, 0xD6);
		case Scope::EntityNameFunction: return IntRGBA(0xDC, 0xDC, 0xAA);
		case Scope::SupportFunction: return IntRGBA(0xDC, 0xDC, 0xAA);
		case Scope::SupportConstantHandlebars: return IntRGBA(0xDC, 0xDC, 0xAA);
		case Scope::SourcePowershell__VariableOtherMember: return IntRGBA(0xDC, 0xDC, 0xAA);
		case Scope::EntityNameOperatorCustom_Literal: return IntRGBA(0xDC, 0xDC, 0xAA);
		case Scope::SupportClass: return IntRGBA(0x4E, 0xC9, 0xB0);
		case Scope::SupportType: return IntRGBA(0x4E, 0xC9, 0xB0);
		case Scope::EntityNameType: return IntRGBA(0x4E, 0xC9, 0xB0);
		case Scope::EntityNameNamespace: return IntRGBA(0x4E, 0xC9, 0xB0);
		case Scope::EntityOtherAttribute: return IntRGBA(0x4E, 0xC9, 0xB0);
		case Scope::EntityNameScope_Resolution: return IntRGBA(0x4E, 0xC9, 0xB0);
		case Scope::EntityNameClass: return IntRGBA(0x4E, 0xC9, 0xB0);
		case Scope::StorageTypeNumericGo: return IntRGBA(0x4E, 0xC9, 0xB0);
		case Scope::StorageTypeByteGo: return IntRGBA(0x4E, 0xC9, 0xB0);
		case Scope::StorageTypeBooleanGo: return IntRGBA(0x4E, 0xC9, 0xB0);
		case Scope::StorageTypeStringGo: return IntRGBA(0x4E, 0xC9, 0xB0);
		case Scope::StorageTypeUintptrGo: return IntRGBA(0x4E, 0xC9, 0xB0);
		case Scope::StorageTypeErrorGo: return IntRGBA(0x4E, 0xC9, 0xB0);
		case Scope::StorageTypeRuneGo: return IntRGBA(0x4E, 0xC9, 0xB0);
		case Scope::StorageTypeCs: return IntRGBA(0x4E, 0xC9, 0xB0);
		case Scope::StorageTypeGenericCs: return IntRGBA(0x4E, 0xC9, 0xB0);
		case Scope::StorageTypeModifierCs: return IntRGBA(0x4E, 0xC9, 0xB0);
		case Scope::StorageTypeVariableCs: return IntRGBA(0x4E, 0xC9, 0xB0);
		case Scope::StorageTypeAnnotationJava: return IntRGBA(0x4E, 0xC9, 0xB0);
		case Scope::StorageTypeGenericJava: return IntRGBA(0x4E, 0xC9, 0xB0);
		case Scope::StorageTypeJava: return IntRGBA(0x4E, 0xC9, 0xB0);
		case Scope::StorageTypeObjectArrayJava: return IntRGBA(0x4E, 0xC9, 0xB0);
		case Scope::StorageTypePrimitiveArrayJava: return IntRGBA(0x4E, 0xC9, 0xB0);
		case Scope::StorageTypePrimitiveJava: return IntRGBA(0x4E, 0xC9, 0xB0);
		case Scope::StorageTypeTokenJava: return IntRGBA(0x4E, 0xC9, 0xB0);
		case Scope::StorageTypeGroovy: return IntRGBA(0x4E, 0xC9, 0xB0);
		case Scope::StorageTypeAnnotationGroovy: return IntRGBA(0x4E, 0xC9, 0xB0);
		case Scope::StorageTypeParametersGroovy: return IntRGBA(0x4E, 0xC9, 0xB0);
		case Scope::StorageTypeGenericGroovy: return IntRGBA(0x4E, 0xC9, 0xB0);
		case Scope::StorageTypeObjectArrayGroovy: return IntRGBA(0x4E, 0xC9, 0xB0);
		case Scope::StorageTypePrimitiveArrayGroovy: return IntRGBA(0x4E, 0xC9, 0xB0);
		case Scope::StorageTypePrimitiveGroovy: return IntRGBA(0x4E, 0xC9, 0xB0);
		case Scope::MetaTypeCastExpr: return IntRGBA(0x4E, 0xC9, 0xB0);
		case Scope::MetaTypeNewExpr: return IntRGBA(0x4E, 0xC9, 0xB0);
		case Scope::SupportConstantMath: return IntRGBA(0x4E, 0xC9, 0xB0);
		case Scope::SupportConstantDom: return IntRGBA(0x4E, 0xC9, 0xB0);
		case Scope::SupportConstantJson: return IntRGBA(0x4E, 0xC9, 0xB0);
		case Scope::EntityOtherInherited_Class: return IntRGBA(0x4E, 0xC9, 0xB0);
		case Scope::SourceCpp__KeywordOperatorNew: return IntRGBA(0xC5, 0x86, 0xC0);
		case Scope::KeywordOperatorDelete: return IntRGBA(0xC5, 0x86, 0xC0);
		case Scope::KeywordOtherUsing: return IntRGBA(0xC5, 0x86, 0xC0);
		case Scope::KeywordOtherDirectiveUsing: return IntRGBA(0xC5, 0x86, 0xC0);
		case Scope::KeywordOtherOperator: return IntRGBA(0xC5, 0x86, 0xC0);
		case Scope::EntityNameOperator: return IntRGBA(0xC5, 0x86, 0xC0);
		case Scope::Variable: return IntRGBA(0x9C, 0xDC, 0xFE);
		case Scope::MetaDefinitionVariableName: return IntRGBA(0x9C, 0xDC, 0xFE);
		case Scope::SupportVariable: return IntRGBA(0x9C, 0xDC, 0xFE);
		case Scope::EntityNameVariable: return IntRGBA(0x9C, 0xDC, 0xFE);
		case Scope::ConstantOtherPlaceholder: return IntRGBA(0x9C, 0xDC, 0xFE);
		case Scope::VariableOtherConstant: return IntRGBA(0x4F, 0xC1, 0xFF);
		case Scope::MetaObject_LiteralKey: return IntRGBA(0x9C, 0xDC, 0xFE);
		case Scope::SupportConstantProperty_Value: return IntRGBA(0xCE, 0x91, 0x78);
		case Scope::SupportConstantFont_Name: return IntRGBA(0xCE, 0x91, 0x78);
		case Scope::SupportConstantMedia_Type: return IntRGBA(0xCE, 0x91, 0x78);
		case Scope::SupportConstantMedia: return IntRGBA(0xCE, 0x91, 0x78);
		case Scope::ConstantOtherColorRgb_Value: return IntRGBA(0xCE, 0x91, 0x78);
		case Scope::ConstantOtherRgb_Value: return IntRGBA(0xCE, 0x91, 0x78);
		case Scope::SupportConstantColor: return IntRGBA(0xCE, 0x91, 0x78);
		case Scope::PunctuationDefinitionGroupRegexp: return IntRGBA(0xCE, 0x91, 0x78);
		case Scope::PunctuationDefinitionGroupAssertionRegexp: return IntRGBA(0xCE, 0x91, 0x78);
		case Scope::PunctuationDefinitionCharacter_ClassRegexp: return IntRGBA(0xCE, 0x91, 0x78);
		case Scope::PunctuationCharacterSetBeginRegexp: return IntRGBA(0xCE, 0x91, 0x78);
		case Scope::PunctuationCharacterSetEndRegexp: return IntRGBA(0xCE, 0x91, 0x78);
		case Scope::KeywordOperatorNegationRegexp: return IntRGBA(0xCE, 0x91, 0x78);
		case Scope::SupportOtherParenthesisRegexp: return IntRGBA(0xCE, 0x91, 0x78);
		case Scope::ConstantCharacterCharacter_ClassRegexp: return IntRGBA(0xD1, 0x69, 0x69);
		case Scope::ConstantOtherCharacter_ClassSetRegexp: return IntRGBA(0xD1, 0x69, 0x69);
		case Scope::ConstantOtherCharacter_ClassRegexp: return IntRGBA(0xD1, 0x69, 0x69);
		case Scope::ConstantCharacterSetRegexp: return IntRGBA(0xD1, 0x69, 0x69);
		case Scope::KeywordOperatorOrRegexp: return IntRGBA(0xDC, 0xDC, 0xAA);
		case Scope::KeywordControlAnchorRegexp: return IntRGBA(0xDC, 0xDC, 0xAA);
		case Scope::KeywordOperatorQuantifierRegexp: return IntRGBA(0xD7, 0xBA, 0x7D);
		case Scope::ConstantCharacter: return IntRGBA(0x56, 0x9C, 0xD6);
		case Scope::ConstantOtherOption: return IntRGBA(0x56, 0x9C, 0xD6);
		case Scope::ConstantCharacterEscape: return IntRGBA(0xD7, 0xBA, 0x7D);
		case Scope::EntityNameLabel: return IntRGBA(0xC8, 0xC8, 0xC8);
		}
	}

	constexpr int DarkPlus2(Scope scope)
	{
		switch (scope)
		{
		case Scope::MetaPreprocessor: return IntRGBA(0x9B, 0x9B, 0x9B);
		case Scope::SupportTypeProperty_NameJson: return DarkPlus(Scope::SupportTypeProperty_Name);
		default: return DarkPlus(scope);
		}
	}
}
