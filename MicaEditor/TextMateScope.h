// Ported from
// https://github.com/microsoft/vscode/blob/main/extensions/theme-defaults/themes/light_plus.json
// https://github.com/microsoft/vscode/blob/main/extensions/theme-defaults/themes/light_vs.json
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

enum class Scope
{
	MetaEmbedded, // meta.embedded
	SourceGroovyEmbedded, // source.groovy.embedded
	String__MetaImageInlineMarkdown, // string meta.image.inline.markdown
	VariableLegacyBuiltinPython, // variable.legacy.builtin.python
	MetaDiffHeader, // meta.diff.header
	Comment, // comment
	ConstantLanguage, // constant.language
	ConstantNumeric, // constant.numeric
	VariableOtherEnummember, // variable.other.enummember
	KeywordOperatorPlusExponent, // keyword.operator.plus.exponent
	KeywordOperatorMinusExponent, // keyword.operator.minus.exponent
	ConstantRegexp, // constant.regexp
	EntityNameTag, // entity.name.tag
	EntityNameSelector, // entity.name.selector
	EntityOtherAttribute_Name, // entity.other.attribute-name
	EntityOtherAttribute_NameClassCss, // entity.other.attribute-name.class.css
	EntityOtherAttribute_NameClassMixinCss, // entity.other.attribute-name.class.mixin.css
	EntityOtherAttribute_NameIdCss, // entity.other.attribute-name.id.css
	EntityOtherAttribute_NameParent_SelectorCss, // entity.other.attribute-name.parent-selector.css
	EntityOtherAttribute_NamePseudo_ClassCss, // entity.other.attribute-name.pseudo-class.css
	EntityOtherAttribute_NamePseudo_ElementCss, // entity.other.attribute-name.pseudo-element.css
	SourceCssLess__EntityOtherAttribute_NameId, // source.css.less entity.other.attribute-name.id
	EntityOtherAttribute_NameScss, // entity.other.attribute-name.scss
	Invalid, // invalid
	MarkupBold, // markup.bold
	MarkupHeading, // markup.heading
	MarkupInserted, // markup.inserted
	MarkupDeleted, // markup.deleted
	MarkupChanged, // markup.changed
	PunctuationDefinitionQuoteBeginMarkdown, // punctuation.definition.quote.begin.markdown
	PunctuationDefinitionListBeginMarkdown, // punctuation.definition.list.begin.markdown
	MarkupInlineRaw, // markup.inline.raw
	PunctuationDefinitionTag, // punctuation.definition.tag
	MetaPreprocessor, // meta.preprocessor
	EntityNameFunctionPreprocessor, // entity.name.function.preprocessor
	MetaPreprocessorString, // meta.preprocessor.string
	MetaPreprocessorNumeric, // meta.preprocessor.numeric
	MetaStructureDictionaryKeyPython, // meta.structure.dictionary.key.python
	Storage, // storage
	StorageType, // storage.type
	StorageModifier, // storage.modifier
	KeywordOperatorNoexcept, // keyword.operator.noexcept
	String, // string
	MetaEmbeddedAssembly, // meta.embedded.assembly
	StringCommentBufferedBlockPug, // string.comment.buffered.block.pug
	StringQuotedPug, // string.quoted.pug
	StringInterpolatedPug, // string.interpolated.pug
	StringUnquotedPlainInYaml, // string.unquoted.plain.in.yaml
	StringUnquotedPlainOutYaml, // string.unquoted.plain.out.yaml
	StringUnquotedBlockYaml, // string.unquoted.block.yaml
	StringQuotedSingleYaml, // string.quoted.single.yaml
	StringQuotedDoubleXml, // string.quoted.double.xml
	StringQuotedSingleXml, // string.quoted.single.xml
	StringUnquotedCdataXml, // string.unquoted.cdata.xml
	StringQuotedDoubleHtml, // string.quoted.double.html
	StringQuotedSingleHtml, // string.quoted.single.html
	StringUnquotedHtml, // string.unquoted.html
	StringQuotedSingleHandlebars, // string.quoted.single.handlebars
	StringQuotedDoubleHandlebars, // string.quoted.double.handlebars
	StringRegexp, // string.regexp
	PunctuationDefinitionTemplate_ExpressionBegin, // punctuation.definition.template-expression.begin
	PunctuationDefinitionTemplate_ExpressionEnd, // punctuation.definition.template-expression.end
	PunctuationSectionEmbedded, // punctuation.section.embedded
	MetaTemplateExpression, // meta.template.expression
	SupportConstantProperty_Value, // support.constant.property-value
	SupportConstantFont_Name, // support.constant.font-name
	SupportConstantMedia_Type, // support.constant.media-type
	SupportConstantMedia, // support.constant.media
	ConstantOtherColorRgb_Value, // constant.other.color.rgb-value
	ConstantOtherRgb_Value, // constant.other.rgb-value
	SupportConstantColor, // support.constant.color
	SupportTypeVendoredProperty_Name, // support.type.vendored.property-name
	SupportTypeProperty_Name, // support.type.property-name
	VariableCss, // variable.css
	VariableScss, // variable.scss
	VariableOtherLess, // variable.other.less
	SourceCoffeeEmbedded, // source.coffee.embedded
	SupportTypeProperty_NameJson, // support.type.property-name.json
	Keyword, // keyword
	KeywordControl, // keyword.control
	KeywordOperator, // keyword.operator
	KeywordOperatorNew, // keyword.operator.new
	KeywordOperatorExpression, // keyword.operator.expression
	KeywordOperatorCast, // keyword.operator.cast
	KeywordOperatorSizeof, // keyword.operator.sizeof
	KeywordOperatorAlignof, // keyword.operator.alignof
	KeywordOperatorTypeid, // keyword.operator.typeid
	KeywordOperatorAlignas, // keyword.operator.alignas
	KeywordOperatorInstanceof, // keyword.operator.instanceof
	KeywordOperatorLogicalPython, // keyword.operator.logical.python
	KeywordOperatorWordlike, // keyword.operator.wordlike
	KeywordOtherUnit, // keyword.other.unit
	PunctuationSectionEmbeddedBeginPhp, // punctuation.section.embedded.begin.php
	PunctuationSectionEmbeddedEndPhp, // punctuation.section.embedded.end.php
	SupportFunctionGit_Rebase, // support.function.git-rebase
	ConstantShaGit_Rebase, // constant.sha.git-rebase
	StorageModifierImportJava, // storage.modifier.import.java
	VariableLanguageWildcardJava, // variable.language.wildcard.java
	StorageModifierPackageJava, // storage.modifier.package.java
	VariableLanguage, // variable.language
	EntityNameFunction, // entity.name.function
	SupportFunction, // support.function
	SupportConstantHandlebars, // support.constant.handlebars
	SourcePowershell__VariableOtherMember, // source.powershell variable.other.member
	EntityNameOperatorCustom_Literal, // entity.name.operator.custom-literal
	SupportClass, // support.class
	SupportType, // support.type
	EntityNameType, // entity.name.type
	EntityNameNamespace, // entity.name.namespace
	EntityOtherAttribute, // entity.other.attribute
	EntityNameScope_Resolution, // entity.name.scope-resolution
	EntityNameClass, // entity.name.class
	StorageTypeNumericGo, // storage.type.numeric.go
	StorageTypeByteGo, // storage.type.byte.go
	StorageTypeBooleanGo, // storage.type.boolean.go
	StorageTypeStringGo, // storage.type.string.go
	StorageTypeUintptrGo, // storage.type.uintptr.go
	StorageTypeErrorGo, // storage.type.error.go
	StorageTypeRuneGo, // storage.type.rune.go
	StorageTypeCs, // storage.type.cs
	StorageTypeGenericCs, // storage.type.generic.cs
	StorageTypeModifierCs, // storage.type.modifier.cs
	StorageTypeVariableCs, // storage.type.variable.cs
	StorageTypeAnnotationJava, // storage.type.annotation.java
	StorageTypeGenericJava, // storage.type.generic.java
	StorageTypeJava, // storage.type.java
	StorageTypeObjectArrayJava, // storage.type.object.array.java
	StorageTypePrimitiveArrayJava, // storage.type.primitive.array.java
	StorageTypePrimitiveJava, // storage.type.primitive.java
	StorageTypeTokenJava, // storage.type.token.java
	StorageTypeGroovy, // storage.type.groovy
	StorageTypeAnnotationGroovy, // storage.type.annotation.groovy
	StorageTypeParametersGroovy, // storage.type.parameters.groovy
	StorageTypeGenericGroovy, // storage.type.generic.groovy
	StorageTypeObjectArrayGroovy, // storage.type.object.array.groovy
	StorageTypePrimitiveArrayGroovy, // storage.type.primitive.array.groovy
	StorageTypePrimitiveGroovy, // storage.type.primitive.groovy
	MetaTypeCastExpr, // meta.type.cast.expr
	MetaTypeNewExpr, // meta.type.new.expr
	SupportConstantMath, // support.constant.math
	SupportConstantDom, // support.constant.dom
	SupportConstantJson, // support.constant.json
	EntityOtherInherited_Class, // entity.other.inherited-class
	SourceCpp__KeywordOperatorNew, // source.cpp keyword.operator.new
	SourceCpp__KeywordOperatorDelete, // source.cpp keyword.operator.delete
	KeywordOtherUsing, // keyword.other.using
	KeywordOtherDirectiveUsing, // keyword.other.directive.using
	KeywordOtherOperator, // keyword.other.operator
	EntityNameOperator, // entity.name.operator
	Variable, // variable
	MetaDefinitionVariableName, // meta.definition.variable.name
	SupportVariable, // support.variable
	EntityNameVariable, // entity.name.variable
	ConstantOtherPlaceholder, // constant.other.placeholder
	VariableOtherConstant, // variable.other.constant
	MetaObject_LiteralKey, // meta.object-literal.key
	PunctuationDefinitionGroupRegexp, // punctuation.definition.group.regexp
	PunctuationDefinitionGroupAssertionRegexp, // punctuation.definition.group.assertion.regexp
	PunctuationDefinitionCharacter_ClassRegexp, // punctuation.definition.character-class.regexp
	PunctuationCharacterSetBeginRegexp, // punctuation.character.set.begin.regexp
	PunctuationCharacterSetEndRegexp, // punctuation.character.set.end.regexp
	KeywordOperatorNegationRegexp, // keyword.operator.negation.regexp
	SupportOtherParenthesisRegexp, // support.other.parenthesis.regexp
	ConstantCharacterCharacter_ClassRegexp, // constant.character.character-class.regexp
	ConstantOtherCharacter_ClassSetRegexp, // constant.other.character-class.set.regexp
	ConstantOtherCharacter_ClassRegexp, // constant.other.character-class.regexp
	ConstantCharacterSetRegexp, // constant.character.set.regexp
	KeywordOperatorQuantifierRegexp, // keyword.operator.quantifier.regexp
	KeywordOperatorOrRegexp, // keyword.operator.or.regexp
	KeywordControlAnchorRegexp, // keyword.control.anchor.regexp
	ConstantCharacter, // constant.character
	ConstantOtherOption, // constant.other.option
	ConstantCharacterEscape, // constant.character.escape
	EntityNameLabel, // entity.name.label
	Header, // header
	EntityNameTagCss, // entity.name.tag.css
	StringTag, // string.tag
	StringValue, // string.value
	KeywordOperatorDelete, // keyword.operator.delete
};
