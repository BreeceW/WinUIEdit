#include "CodeEditorHandler.h"
#include <cmath>
#include "ScintillaMessages.h"

using namespace Scintilla;

namespace WinUIEditor
{
	void CodeEditorHandler::SetScintilla(std::shared_ptr<ScintillaCall> call)
	{
		_call = call;
	}

	void CodeEditorHandler::SetLexilla(Lexilla::CreateLexerFn createLexer)
	{
		_createLexer = createLexer;
	}

	void CodeEditorHandler::Initialize()
	{
		AddKeyboardShortcuts();
		ChangeDefaults();
		ChangeDocumentDefaults();
	}

	void CodeEditorHandler::ChangeDocumentDefaults()
	{
		_call->SetUseTabs(false);
		_call->SetTabWidth(4);
		_call->SetIndent(4); // Brace matching and autoindent relies on this
	}

	void CodeEditorHandler::UpdateColors(CodeEditorTheme theme)
	{
		// Todo: Support high contrast mode

		constexpr auto folderForeDark{ IntRGBA(0, 0, 0, 0) };
		constexpr auto folderBackDark{ IntRGBA(0xFF, 0xFF, 0xFF, 148) };
		constexpr auto folderBackHighlightDark{ folderBackDark };
		constexpr auto folderForeLight{ IntRGBA(0xFF, 0xFF, 0xFF, 0) };
		constexpr auto folderBackLight{ IntRGBA(0, 0, 0, 108) };
		constexpr auto folderBackHighlightLight{ folderBackLight };

		if (_theme != theme)
		{
			_theme = theme;

			switch (theme)
			{
			case CodeEditorTheme::Dark:
				_call->SetElementColour(Element::Caret, IntRGBA(0xAE, 0xAF, 0xAD));
				_call->SetElementColour(Element::SelectionBack, IntRGBA(0x26, 0x4F, 0x78));
				_call->SetElementColour(Element::SelectionAdditionalBack, IntRGBA(0x26, 0x4F, 0x78));
				_call->SetElementColour(Element::SelectionInactiveBack, IntRGBA(0x3A, 0x3D, 0x41));
				_call->SetElementColour(Element::HiddenLine, IntRGBA(0xFF, 0xFF, 0xFF, 48));

				MarkerSetColors(Scintilla::MarkerOutline::FolderOpen, folderForeDark, folderBackDark, folderBackHighlightDark);
				MarkerSetColors(Scintilla::MarkerOutline::Folder, folderForeDark, folderBackDark, folderBackHighlightDark);
				MarkerSetColors(Scintilla::MarkerOutline::FolderSub, folderForeDark, folderBackDark, folderBackHighlightDark);
				MarkerSetColors(Scintilla::MarkerOutline::FolderTail, folderForeDark, folderBackDark, folderBackHighlightDark);
				MarkerSetColors(Scintilla::MarkerOutline::FolderEnd, folderForeDark, folderBackDark, folderBackHighlightDark);
				MarkerSetColors(Scintilla::MarkerOutline::FolderOpenMid, folderForeDark, folderBackDark, folderBackHighlightDark);
				MarkerSetColors(Scintilla::MarkerOutline::FolderMidTail, folderForeDark, folderBackDark, folderBackHighlightDark);

				_call->MarkerSetForeTranslucent(static_cast<int>(Scintilla::MarkerOutline::HistoryRevertedToOrigin), IntRGBA(0x35, 0x95, 0xDE));
				_call->MarkerSetBackTranslucent(static_cast<int>(Scintilla::MarkerOutline::HistoryRevertedToOrigin), IntRGBA(0x35, 0x95, 0xDE, 0x00));
				_call->MarkerSetForeTranslucent(static_cast<int>(Scintilla::MarkerOutline::HistorySaved), IntRGBA(0x55, 0xB1, 0x55));
				_call->MarkerSetBackTranslucent(static_cast<int>(Scintilla::MarkerOutline::HistorySaved), IntRGBA(0x55, 0xB1, 0x55));
				_call->MarkerSetForeTranslucent(static_cast<int>(Scintilla::MarkerOutline::HistoryModified), IntRGBA(0xD0, 0xB1, 0x32));
				_call->MarkerSetBackTranslucent(static_cast<int>(Scintilla::MarkerOutline::HistoryModified), IntRGBA(0x27, 0x27, 0x27, 0x00));
				_call->MarkerSetForeTranslucent(static_cast<int>(Scintilla::MarkerOutline::HistoryRevertedToModified), IntRGBA(0x93, 0xB1, 0x44));
				_call->MarkerSetBackTranslucent(static_cast<int>(Scintilla::MarkerOutline::HistoryRevertedToModified), IntRGBA(0x93, 0xB1, 0x44, 0x00));

				SetCallTipBackgroundColor(IntRGBA(0x00, 0x00, 0x00, 0x00));
				_call->CallTipSetFore(IntRGBA(0xFF, 0xFF, 0xFF));
				_call->CallTipSetForeHlt(IntRGBA(0x56, 0x9C, 0xD6));
				SetCallTipHoverColor(IntRGBA(0xFF, 0xFF, 0xFF, 0xA6));
				break;

			case CodeEditorTheme::Light:
				_call->SetElementColour(Element::Caret, IntRGBA(0x00, 0x00, 0x00));
				_call->SetElementColour(Element::SelectionBack, IntRGBA(0xAD, 0xD6, 0xFF));
				_call->SetElementColour(Element::SelectionAdditionalBack, IntRGBA(0xAD, 0xD6, 0xFF));
				_call->SetElementColour(Element::SelectionInactiveBack, IntRGBA(0xE5, 0xEB, 0xF1));
				_call->SetElementColour(Element::HiddenLine, IntRGBA(0x00, 0x00, 0x00, 64));

				MarkerSetColors(Scintilla::MarkerOutline::FolderOpen, folderForeLight, folderBackLight, folderBackHighlightLight);
				MarkerSetColors(Scintilla::MarkerOutline::Folder, folderForeLight, folderBackLight, folderBackHighlightLight);
				MarkerSetColors(Scintilla::MarkerOutline::FolderSub, folderForeLight, folderBackLight, folderBackHighlightLight);
				MarkerSetColors(Scintilla::MarkerOutline::FolderTail, folderForeLight, folderBackLight, folderBackHighlightLight);
				MarkerSetColors(Scintilla::MarkerOutline::FolderEnd, folderForeLight, folderBackLight, folderBackHighlightLight);
				MarkerSetColors(Scintilla::MarkerOutline::FolderOpenMid, folderForeLight, folderBackLight, folderBackHighlightLight);
				MarkerSetColors(Scintilla::MarkerOutline::FolderMidTail, folderForeLight, folderBackLight, folderBackHighlightLight);

				_call->MarkerSetForeTranslucent(static_cast<int>(Scintilla::MarkerOutline::HistoryRevertedToOrigin), IntRGBA(0x00, 0x78, 0xD4));
				_call->MarkerSetBackTranslucent(static_cast<int>(Scintilla::MarkerOutline::HistoryRevertedToOrigin), IntRGBA(0x00, 0x78, 0xD4, 0x00));
				_call->MarkerSetForeTranslucent(static_cast<int>(Scintilla::MarkerOutline::HistorySaved), IntRGBA(0x10, 0x7C, 0x10));
				_call->MarkerSetBackTranslucent(static_cast<int>(Scintilla::MarkerOutline::HistorySaved), IntRGBA(0x10, 0x7C, 0x10));
				_call->MarkerSetForeTranslucent(static_cast<int>(Scintilla::MarkerOutline::HistoryModified), IntRGBA(0xAE, 0x8C, 0x00));
				_call->MarkerSetBackTranslucent(static_cast<int>(Scintilla::MarkerOutline::HistoryModified), IntRGBA(0xF5, 0xF5, 0xF5, 0x00));
				_call->MarkerSetForeTranslucent(static_cast<int>(Scintilla::MarkerOutline::HistoryRevertedToModified), IntRGBA(0x5F, 0x84, 0x08));
				_call->MarkerSetBackTranslucent(static_cast<int>(Scintilla::MarkerOutline::HistoryRevertedToModified), IntRGBA(0x5F, 0x84, 0x08, 0x00));

				SetCallTipBackgroundColor(IntRGBA(0xFF, 0xFF, 0xFF, 0x00));
				_call->CallTipSetFore(IntRGBA(0x00, 0x00, 0x00));
				_call->CallTipSetForeHlt(IntRGBA(0x00, 0x00, 0xFF));
				SetCallTipHoverColor(IntRGBA(0x00, 0x00, 0x00, 0x88));
				break;
			}

			UpdateCaretLineBackColors(true);

			UpdateStyles();
		}
	}

	// Duplicated from Helpers.cpp to make portable
	// Derived from https://github.com/microsoft/Windows-universal-samples/blob/main/Samples/ComplexInk/cpp/Common/DeviceResources.h
	static constexpr int ConvertFromDipToPixelUnit(float val, float dpiAdjustmentRatio, bool rounded = true)
	{
		float pixelVal = val * dpiAdjustmentRatio;
		return static_cast<int>(rounded ? floorf(pixelVal + 0.5f) : pixelVal); // Todo: Test if this is ever necessary
	}

	void CodeEditorHandler::UpdateDpi(double value)
	{
		_dpiScale = value;
		_call->SetXCaretPolicy(CaretPolicy::Slop | CaretPolicy::Strict | CaretPolicy::Even, ConvertFromDipToPixelUnit(24, value));
		UpdateZoom();
	}

	void CodeEditorHandler::StyleSetFore(int style, ColourAlpha color)
	{
		// Overridable to allow users to internally set transparent color
		_call->StyleSetFore(style, color);
	}

	void CodeEditorHandler::StyleSetBack(int style, ColourAlpha color)
	{
		// Overridable to allow users to internally set transparent color
		_call->StyleSetBack(style, color);
	}

	void CodeEditorHandler::InvalidateStyleRedraw()
	{
		// Overridable to allow users to invalidate after setting all colors
		// Not needed if using default StyleSetFore/Back
	}

	void CodeEditorHandler::StyleClearCustom()
	{
		// Less performant way to copy styles
		// Overridable when there is access to Scintilla internals
		for (size_t i = 0; i < 256; i++)
		{
			if (i < static_cast<int>(StylesCommon::Default) || i > static_cast<int>(StylesCommon::LastPredefined))
			{
				_call->StyleSetFont(i, _call->StyleGetFont(static_cast<int>(StylesCommon::Default)).c_str());
				_call->StyleSetSizeFractional(i, _call->StyleGetSizeFractional(static_cast<int>(StylesCommon::Default)));
				_call->StyleSetBold(i, _call->StyleGetBold(static_cast<int>(StylesCommon::Default)));
				_call->StyleSetWeight(i, _call->StyleGetWeight(static_cast<int>(StylesCommon::Default)));
				_call->StyleSetItalic(i, _call->StyleGetItalic(static_cast<int>(StylesCommon::Default)));
				_call->StyleSetUnderline(i, _call->StyleGetUnderline(static_cast<int>(StylesCommon::Default)));
				_call->StyleSetFore(i, _call->StyleGetFore(static_cast<int>(StylesCommon::Default)));
				_call->StyleSetBack(i, _call->StyleGetBack(static_cast<int>(StylesCommon::Default)));
				_call->StyleSetEOLFilled(i, _call->StyleGetEOLFilled(static_cast<int>(StylesCommon::Default)));
				_call->StyleSetCharacterSet(i, _call->StyleGetCharacterSet(static_cast<int>(StylesCommon::Default)));
				_call->StyleSetCase(i, _call->StyleGetCase(static_cast<int>(StylesCommon::Default)));
				_call->StyleSetVisible(i, _call->StyleGetVisible(static_cast<int>(StylesCommon::Default)));
				_call->StyleSetChangeable(i, _call->StyleGetChangeable(static_cast<int>(StylesCommon::Default)));
				_call->StyleSetHotSpot(i, _call->StyleGetHotSpot(static_cast<int>(StylesCommon::Default)));
				_call->StyleSetCheckMonospaced(i, _call->StyleGetCheckMonospaced(static_cast<int>(StylesCommon::Default)));
				_call->StyleSetInvisibleRepresentation(i, _call->StyleGetInvisibleRepresentation(static_cast<int>(StylesCommon::Default)).c_str());
			}
		}
	}

	void CodeEditorHandler::SetFoldMarginColor(bool useSetting, ColourAlpha back)
	{
		// Implement for transparent folding margin. Not implemented so default is preserved
	}

	void CodeEditorHandler::SetFoldMarginHiColor(bool useSetting, ColourAlpha fore)
	{
		// Implement for transparent folding margin. Not implemented so default is preserved
	}

	void CodeEditorHandler::SetCallTipBackgroundColor(Scintilla::ColourAlpha color)
	{
		// Overridable to allow users to internally set transparent color
		_call->CallTipSetBack(color);
	}

	void CodeEditorHandler::SetCallTipHoverColor(Scintilla::ColourAlpha color)
	{
		// Implement if platform supports setting a color when call tip arrows are hovered
	}

	void CodeEditorHandler::DefaultColorsChanged(CodeEditorTheme theme)
	{
		// Default event handler
	}

	void CodeEditorHandler::SyntaxHighlightingApplied(CodeEditorTheme theme)
	{
		// Default event handler
	}

	void CodeEditorHandler::UpdateStyles()
	{
		constexpr auto bgDark{ IntRGBA(0x27, 0x27, 0x27, 0x00) };
		constexpr auto bgLight{ IntRGBA(0xF5, 0xF5, 0xF5, 0x00) };

		switch (_theme)
		{
		case CodeEditorTheme::Dark:
			StyleSetFore(static_cast<int>(StylesCommon::LineNumber), IntRGBA(0x85, 0x85, 0x85));
			StyleSetBack(static_cast<int>(StylesCommon::LineNumber), bgDark);

			StyleSetFore(static_cast<int>(StylesCommon::Default), IntRGBA(0xFF, 0xFF, 0xFF));
			StyleSetBack(static_cast<int>(StylesCommon::Default), bgDark);

			StyleSetFore(static_cast<int>(StylesCommon::BraceLight), IntRGBA(0xFF, 0xFF, 0xFF));
			StyleSetBack(static_cast<int>(StylesCommon::BraceLight), IntRGBA(0x11, 0x3D, 0x6F));
			StyleSetFore(static_cast<int>(StylesCommon::BraceBad), IntRGBA(0xcd, 0x31, 0x31));
			StyleSetBack(static_cast<int>(StylesCommon::BraceBad), bgDark);

			StyleSetFore(static_cast<int>(StylesCommon::IndentGuide), IntRGBA(0xFF, 0xFF, 0xFF, 48));
			StyleSetBack(static_cast<int>(StylesCommon::IndentGuide), bgDark);

			StyleSetFore(static_cast<int>(StylesCommon::ControlChar), IntRGBA(0xFF, 0xFF, 0xFF));
			StyleSetBack(static_cast<int>(StylesCommon::ControlChar), IntRGBA(0x96, 0x00, 0x00));

			StyleSetFore(static_cast<int>(StylesCommon::FoldDisplayText), IntRGBA(0xB8, 0xC2, 0xCC));
			StyleSetBack(static_cast<int>(StylesCommon::FoldDisplayText), IntRGBA(0x26, 0x33, 0x3F));
			break;

		case CodeEditorTheme::Light:
			StyleSetFore(static_cast<int>(StylesCommon::LineNumber), IntRGBA(0x23, 0x78, 0x93));
			StyleSetBack(static_cast<int>(StylesCommon::LineNumber), bgLight);

			StyleSetFore(static_cast<int>(StylesCommon::Default), IntRGBA(0x00, 0x00, 0x00));
			StyleSetBack(static_cast<int>(StylesCommon::Default), bgLight);

			StyleSetFore(static_cast<int>(StylesCommon::BraceLight), IntRGBA(0x00, 0x00, 0x00));
			StyleSetBack(static_cast<int>(StylesCommon::BraceLight), IntRGBA(0xE2, 0xE6, 0xD6));
			StyleSetFore(static_cast<int>(StylesCommon::BraceBad), IntRGBA(0xcd, 0x31, 0x31));
			StyleSetBack(static_cast<int>(StylesCommon::BraceBad), bgLight);

			StyleSetFore(static_cast<int>(StylesCommon::IndentGuide), IntRGBA(0x00, 0x00, 0x00, 64));
			StyleSetBack(static_cast<int>(StylesCommon::IndentGuide), bgLight);

			StyleSetFore(static_cast<int>(StylesCommon::ControlChar), IntRGBA(0xFF, 0xFF, 0xFF));
			StyleSetBack(static_cast<int>(StylesCommon::ControlChar), IntRGBA(0x96, 0x00, 0x00));

			StyleSetFore(static_cast<int>(StylesCommon::FoldDisplayText), IntRGBA(0x73, 0x79, 0x80));
			StyleSetBack(static_cast<int>(StylesCommon::FoldDisplayText), IntRGBA(0xDC, 0xEA, 0xF5));
			break;
		}

		DefaultColorsChanged(_theme);

		UpdateLanguageStyles();

		SyntaxHighlightingApplied(_theme);

		InvalidateStyleRedraw();
	}

	void CodeEditorHandler::MarkerSetColors(Scintilla::MarkerOutline marker, Scintilla::ColourAlpha fore, Scintilla::ColourAlpha back, Scintilla::ColourAlpha backHighlight)
	{
		const auto markerNumber{ static_cast<int>(marker) };
		_call->MarkerSetForeTranslucent(markerNumber, fore);
		_call->MarkerSetBackTranslucent(markerNumber, back);
		_call->MarkerSetBackSelectedTranslucent(markerNumber, backHighlight);
	}

	void CodeEditorHandler::UpdateCaretLineBackColors(bool colorsUpdated)
	{
		const auto hasEmptySelection = _call->SelectionEmpty();

		if ((_hasEmptySelection != hasEmptySelection) || (hasEmptySelection && colorsUpdated))
		{
			_hasEmptySelection = hasEmptySelection;

			if (hasEmptySelection)
			{
				switch (_theme)
				{
				case CodeEditorTheme::Dark:
					_call->SetElementColour(Element::CaretLineBack, IntRGBA(0xFF, 0xFF, 0xFF, 16));
					break;

				case CodeEditorTheme::Light:
					_call->SetElementColour(Element::CaretLineBack, IntRGBA(0x00, 0x00, 0x00, 12));
					break;
				}
			}
			else
			{
				_call->ResetElementColour(Element::CaretLineBack);
			}
		}
	}

	void CodeEditorHandler::UpdateBraceMatch()
	{
		SciBraceMatch();
	}

	void CodeEditorHandler::AutoIndent(char ch)
	{
		SciAutomaticIndentation(ch);
	}

	void CodeEditorHandler::SetLanguageIndentMode(
		int indentKeywordStyle, const std::set<std::string> &indentKeywords,
		int lineEndStyle, const std::set<std::string> &lineEndWords,
		int blockStartStyle, const std::set<std::string> &blockStartWords,
		int blockEndStyle, const std::set<std::string> &blockEndWords)
	{
		_sciStatementIndent = { indentKeywordStyle, { indentKeywords, } }; // Todo: customize. also note using WORD2 for these
		_sciStatementEnd = { lineEndStyle, { lineEndWords, } };
		_sciBlockStart = { blockStartStyle, { blockStartWords, } };
		_sciBlockEnd = { blockEndStyle, { blockEndWords, } };
	}

	void CodeEditorHandler::UpdateZoom()
	{
		const auto size{ _call->StyleGetSizeFractional(static_cast<int>(StylesCommon::Default)) };
		const auto zoom{ static_cast<int>(_call->Zoom()) };
		const auto factor{ static_cast<float>((size + zoom * 100)) / size };
		// Todo: Width 11 hard coded for a digit. Use below for real value:
		// _call->TextWidth, static_cast<uptr_t>(StylesCommon::LineNumber), reinterpret_cast<sptr_t>("9"))
		const auto line{ _call->LineCount() };
		const auto width{ 12 + 11 * std::max(3, static_cast<int>(std::floor(std::log10(line) + 1))) };
		_call->SetMarginWidthN(0, ConvertFromDipToPixelUnit(std::floorf(factor * width + 0.5f), _dpiScale));
		_call->SetMarginWidthN(1, ConvertFromDipToPixelUnit(std::floorf(factor * 12 + 0.5f), _dpiScale));
		_call->SetMarginWidthN(2, ConvertFromDipToPixelUnit(std::floorf(factor * 10 + 0.5f), _dpiScale));
		const auto foldMarkerStroke{ ConvertFromDipToPixelUnit(factor * 100.0f, _dpiScale, false) };
		_call->MarkerSetStrokeWidth(static_cast<int>(Scintilla::MarkerOutline::FolderOpen), foldMarkerStroke);
		_call->MarkerSetStrokeWidth(static_cast<int>(Scintilla::MarkerOutline::Folder), foldMarkerStroke);
		_call->MarkerSetStrokeWidth(static_cast<int>(Scintilla::MarkerOutline::FolderSub), foldMarkerStroke);
		_call->MarkerSetStrokeWidth(static_cast<int>(Scintilla::MarkerOutline::FolderTail), foldMarkerStroke);
		_call->MarkerSetStrokeWidth(static_cast<int>(Scintilla::MarkerOutline::FolderEnd), foldMarkerStroke);
		_call->MarkerSetStrokeWidth(static_cast<int>(Scintilla::MarkerOutline::FolderOpenMid), foldMarkerStroke);
		_call->MarkerSetStrokeWidth(static_cast<int>(Scintilla::MarkerOutline::FolderMidTail), foldMarkerStroke);
		const auto historyMarkerStroke{ foldMarkerStroke };
		_call->MarkerSetStrokeWidth(static_cast<int>(Scintilla::MarkerOutline::HistoryRevertedToOrigin), historyMarkerStroke);
		// HistorySaved is left at default width (1) because rendering artifacts occur with the solid color + box outline
		//_call->MarkerSetStrokeWidth(static_cast<int>(Scintilla::MarkerOutline::HistorySaved), historyMarkerStroke);
		_call->MarkerSetStrokeWidth(static_cast<int>(Scintilla::MarkerOutline::HistoryModified), historyMarkerStroke);
		_call->MarkerSetStrokeWidth(static_cast<int>(Scintilla::MarkerOutline::HistoryRevertedToModified), historyMarkerStroke);
		_call->SetMarginLeft(ConvertFromDipToPixelUnit(std::floorf(factor * 1 + 0.5f), _dpiScale));
		// Todo: Set caret width to be at least the UISettings system caret width
		const auto caretWidth{ std::max(1.0f, std::floorf(factor * 2 * _dpiScale)) };
		_call->SetCaretWidth(caretWidth); // Todo: Needs to stop blinking after timeout and respect blink rate
		_call->SetCaretLineFrame(caretWidth);
		_call->SetExtraDescent(std::floorf(factor * 1.8 * _dpiScale));
	}

	void CodeEditorHandler::AddKeyboardShortcuts()
	{
		_call->AssignCmdKey(187 + (static_cast<int>(KeyMod::Ctrl) << 16), static_cast<int>(Message::ZoomIn)); // Ctrl+Plus
		_call->AssignCmdKey(189 + (static_cast<int>(KeyMod::Ctrl) << 16), static_cast<int>(Message::ZoomOut)); // Ctrl+Minus
		_call->AssignCmdKey(48 + (static_cast<int>(KeyMod::Ctrl) << 16), static_cast<int>(Message::SetZoom)); // Ctrl+0
	}

	void CodeEditorHandler::ChangeDefaults()
	{
		_call->SetMultipleSelection(true);
		_call->SetScrollWidth(2000);
		_call->SetScrollWidthTracking(true);
		_call->SetYCaretPolicy(CaretPolicy::Slop | CaretPolicy::Strict | CaretPolicy::Even, 1);
		_call->SetVisiblePolicy(VisiblePolicy::Slop, 0);
		_call->SetHScrollBar(true);
		_call->SetEndAtLastLine(false);
		_call->StyleSetFont(static_cast<int>(StylesCommon::Default), "Cascadia Code"); // Todo: Use font available on Windows 10
		_call->StyleSetSizeFractional(static_cast<int>(StylesCommon::Default), 11 * FontSizeMultiplier);
		_call->SetAdditionalSelectionTyping(true);
		_call->SetMultiPaste(MultiPaste::Each);
		_call->SetLayoutThreads(16); // Todo: Determine performance impact
		_call->SetCaretLineVisibleAlways(true);
		_call->SetCaretLineLayer(Layer::UnderText);
		_call->SetCaretLineHighlightSubLine(true);
		_call->SetIndentationGuides(IndentView::LookBoth);
		_call->SetMarginMaskN(2, Scintilla::MaskFolders);
		_call->SetMarginSensitiveN(2, true);
		SetFoldMarginColor(true, IntRGBA(0, 0, 0, 0));
		SetFoldMarginHiColor(true, IntRGBA(0, 0, 0, 0));

		constexpr auto useCustomChevron{ true }; // Todo: make overridable
		constexpr auto chevronMinus{ static_cast<MarkerSymbol>(1989) };
		constexpr auto chevronPlus{ static_cast<MarkerSymbol>(1990) };
		_call->MarkerDefine(static_cast<int>(MarkerOutline::FolderOpen), useCustomChevron ? chevronMinus : MarkerSymbol::BoxMinus);
		_call->MarkerDefine(static_cast<int>(MarkerOutline::Folder), useCustomChevron ? chevronPlus : MarkerSymbol::BoxPlus);
		_call->MarkerDefine(static_cast<int>(MarkerOutline::FolderSub), MarkerSymbol::VLine);
		_call->MarkerDefine(static_cast<int>(MarkerOutline::FolderTail), MarkerSymbol::LCorner);
		_call->MarkerDefine(static_cast<int>(MarkerOutline::FolderEnd), useCustomChevron ? chevronPlus : MarkerSymbol::BoxPlusConnected);
		_call->MarkerDefine(static_cast<int>(MarkerOutline::FolderOpenMid), useCustomChevron ? chevronMinus : MarkerSymbol::BoxMinusConnected);
		_call->MarkerDefine(static_cast<int>(MarkerOutline::FolderMidTail), MarkerSymbol::TCorner);
		_call->MarkerEnableHighlight(false);

		_call->SetDefaultFoldDisplayText(u8"\u00a0\u22ef\u00a0"); // ... nbsp + centered vertically + nbsp
		_call->FoldDisplayTextSetStyle(Scintilla::FoldDisplayTextStyle::Standard);

		_call->SetAutomaticFold(static_cast<Scintilla::AutomaticFold>(static_cast<int>(Scintilla::AutomaticFold::Show) | static_cast<int>(Scintilla::AutomaticFold::Click) | static_cast<int>(Scintilla::AutomaticFold::Change)));
	}

	// Todo: This code needs to integrate with find and replace
	// Todo: This code seems to struggle with rectangular selections and/or multiple carets already existing and/or multiple selections already existing
	void CodeEditorHandler::ChangeAllOccurrences()
	{
		sptr_t start;
		sptr_t end;
		const auto s{ GetMainSelectedText(true, start, end) };
		if (s.length() == 0)
		{
			return;
		}
		const auto length{ end - start };

		// Drop additional selections and ensure caret is at end of selection
		_call->SetSelection(end, start);

		_call->TargetWholeDocument();
		_call->SetSearchFlags(FindOption::MatchCase);

		const auto mainSelection{ _call->MainSelection() };
		const auto bodyLength{ _call->Length() };

		while (true)
		{
			const auto match{ _call->SearchInTarget(length, &s[0]) };

			if (match == -1)
			{
				break;
			}

			const auto targetEnd{ _call->TargetEnd() };

			if (match != start)
			{
				// Todo: Add maximum number of carets and notify user if exceeded (VS Code allows 10,000)
				// Todo: This method calls a lot of redraws in a loop. You might need to use the lower level API to avoid that
				_call->AddSelection(match + length, match);
			}

			_call->SetTargetStart(targetEnd);
			_call->SetTargetEnd(bodyLength);
		}

		_call->SetMainSelection(mainSelection);
	}

	std::wstring_view CodeEditorHandler::HighlightingLanguage()
	{
		return _highlightingLanguage;
	}

	void CodeEditorHandler::HighlightingLanguage(std::wstring_view value)
	{
		if (_highlightingLanguage == value)
		{
			return;
		}

		_highlightingLanguage = value;

		_call->ClearDocumentStyle();

		SetLexer();

		UpdateStyles();

		_call->ColouriseAll();
	}

	void CodeEditorHandler::ProcessNotification(NotificationData *data)
	{
		if (data->nmhdr.code == Notification::Zoom
			|| (data->nmhdr.code == Notification::Modified && data->linesAdded))
		{
			UpdateZoom();
		}

		if (data->nmhdr.code == Notification::UpdateUI && FlagSet(data->updated, Update::Content | Update::Selection))
		{
			UpdateCaretLineBackColors();
			if (_call->Focus())
			{
				UpdateBraceMatch();
			}
		}

		if (data->nmhdr.code == Notification::FocusOut)
		{
			_call->BraceBadLight(-1);
		}
		else if (data->nmhdr.code == Notification::FocusIn)
		{
			UpdateBraceMatch();
		}

		if (data->nmhdr.code == Notification::CharAdded && data->ch <= 0xFF)
		{
			const auto sel{ _call->SelectionSpan() };

			if (sel.start == sel.end && sel.start > 0
				&& !_call->CallTipActive()
				&& !_call->AutoCActive())
			{
				AutoIndent(data->ch);
			}
		}

		if (data->nmhdr.code == Notification::DoubleClick && _call->FoldDisplayTextGetStyle() != FoldDisplayTextStyle::Hidden)
		{
			const auto lineEndPosition{ _call->LineEndPosition(data->line) };
			if ((data->position == lineEndPosition || data->position == InvalidPosition)
				&& !_call->FoldExpanded(data->line))
			{
				const auto ctrl{ FlagSet(data->modifiers, KeyMod::Ctrl) };
				const auto shift{ FlagSet(data->modifiers, KeyMod::Shift) };
				const auto levelClick{ _call->FoldLevel(data->line) };
				if (LevelIsHeader(levelClick))
				{
					if (shift)
					{
						// Ensure all children visible
						_call->ExpandChildren(data->line, levelClick);
					}
					else if (ctrl)
					{
						_call->FoldChildren(data->line, FoldAction::Toggle);
					}
					else
					{
						// Toggle this line
						_call->FoldLine(data->line, FoldAction::Toggle);
					}

					// Remove selection from double click
					_call->SetEmptySelection(lineEndPosition);
				}
			}
		}
	}

	std::string CodeEditorHandler::GetMainSelectedText(bool expandCaretToWord, sptr_t &start, sptr_t &end)
	{
		// Todo: This code may be problematic for rectangular selections
		start = _call->SelectionStart();
		end = _call->SelectionEnd();
		if (expandCaretToWord && start == end)
		{
			start = _call->WordStartPosition(start, true);
			end = _call->WordEndPosition(start, true);

			if (start == end)
			{
				return "";
			}
		}

		const auto length{ end - start };
		std::string s(length, '\0');
		TextRangeFull range
		{
			{ start, end, },
			&s[0],
		};
		_call->GetTextRangeFull(&range);

		return s;
	}
}
