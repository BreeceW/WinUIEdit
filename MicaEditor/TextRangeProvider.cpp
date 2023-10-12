#include "pch.h"
#include "TextRangeProvider.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace DUX;
using namespace DUX::Automation;
using namespace DUX::Automation::Peers;
using namespace DUX::Automation::Provider;
using namespace DUX::Automation::Text;

constexpr hresult XAML_E_NOT_SUPPORTED{ static_cast<int32_t>(0x80131515) };

namespace winrt::MicaEditor::implementation
{
	TextRangeProvider::TextRangeProvider(IRawElementProviderSimple const &peer, Editor const &editor, int64_t rangeStart, int64_t rangeEnd, Rect const &bounds)
		: _peer{ peer }, _editor{ editor }, _rangeStart{ rangeStart }, _rangeEnd{ rangeEnd }, _bounds{ bounds }
	{
	}

	ITextRangeProvider TextRangeProvider::Clone()
	{
		return make<TextRangeProvider>(_peer, _editor.get(), _rangeStart, _rangeEnd, _bounds);
	}

	bool TextRangeProvider::Compare(ITextRangeProvider const &textRangeProvider)
	{
		const auto target{ textRangeProvider.as<TextRangeProvider>() };
		return _rangeStart == target->_rangeStart && _rangeEnd == target->_rangeEnd;
	}

	int32_t TextRangeProvider::CompareEndpoints(TextPatternRangeEndpoint const &endpoint, ITextRangeProvider const &textRangeProvider, TextPatternRangeEndpoint const &targetEndpoint)
	{
		const auto target{ textRangeProvider.as<TextRangeProvider>() };
		const auto thisEnd{ endpoint == TextPatternRangeEndpoint::Start ? _rangeStart : _rangeEnd };
		const auto targetEnd{ targetEndpoint == TextPatternRangeEndpoint::Start ? target->_rangeStart : target->_rangeEnd };
		return thisEnd - targetEnd;
	}

	void TextRangeProvider::ExpandToEnclosingUnit(TextUnit const &unit)
	{
		const auto editor{ _editor.get() };
		if (!editor)
		{
			throw hresult_error{ E_POINTER };
		}

		if (unit == TextUnit::Document)
		{
			_rangeStart = 0;
			_rangeEnd = editor.Length();
		}
		else if (unit == TextUnit::Line || unit == TextUnit::Paragraph)
		{
			const auto line{ editor.LineFromPosition(_rangeStart) };
			_rangeStart = editor.PositionFromLine(line);
			_rangeEnd = editor.GetLineEndPosition(line);
		}
		else if (unit == TextUnit::Page)
		{
			const auto firstVisibleLine{ editor.FirstVisibleLine() };
			const auto maxLine{ editor.LineCount() - 1 };
			const auto lastVisibleLine{ std::min(maxLine, firstVisibleLine + editor.LinesOnScreen()) };

			_rangeStart = editor.PositionFromLine(firstVisibleLine);
			_rangeEnd = editor.GetLineEndPosition(lastVisibleLine);
		}
		else if (unit == TextUnit::Word)
		{
			_rangeStart = editor.WordStartPosition(_rangeStart, false);
			_rangeEnd = editor.WordEndPosition(_rangeStart, false);
		}
		else if (unit == TextUnit::Character)
		{
			_rangeEnd = std::min(_rangeStart + 1, editor.Length()); // Todo: Multibyte chars
		}
	}

	ITextRangeProvider TextRangeProvider::FindAttribute(int32_t attributeId, IInspectable const &value, bool backward)
	{
		throw hresult_not_implemented{};
	}

	ITextRangeProvider TextRangeProvider::FindText(hstring const &text, bool backward, bool ignoreCase)
	{
		throw hresult_not_implemented{};
	}

	IInspectable TextRangeProvider::GetAttributeValue(int32_t attributeId)
	{
		const auto editor{ _editor.get() };
		if (!editor)
		{
			throw hresult_error{ E_POINTER };
		}

		switch (static_cast<AutomationTextAttributesEnum>(attributeId))
		{
		case AutomationTextAttributesEnum::IsReadOnlyAttribute:
			return box_value(editor.ReadOnly());
		case AutomationTextAttributesEnum::CaretPositionAttribute:
		{
			const auto sel{ editor.MainSelection() };
			const auto caret{ editor.GetSelectionNCaret(sel) };
			if (caret < _rangeStart || caret > _rangeEnd)
			{
				// Unknown if caret is outside this text range
				// Todo: Not sure if this is the correct behavior
				return box_value(AutomationCaretPosition::Unknown);
			}
			const auto line{ editor.LineFromPosition(caret) };
			if (caret == editor.PositionFromLine(line))
			{
				return box_value(AutomationCaretPosition::BeginningOfLine);
			}
			else if (caret == editor.GetLineEndPosition(line))
			{
				return box_value(AutomationCaretPosition::EndOfLine);
			}
			else
			{
				return box_value(AutomationCaretPosition::Unknown);
			}
		}
		default:
			// Must throw instead of returning nullptr
			// https://github.com/microsoft/terminal/blob/main/src/cascadia/TerminalControl/XamlUiaTextRange.cpp#L141
			throw_hresult(XAML_E_NOT_SUPPORTED);
		}
	}

	void TextRangeProvider::GetBoundingRectangles(com_array<double> &returnValue)
	{
		// Structure: https://github.com/microsoft/terminal/blob/main/src/types/UiaTextRangeBase.cpp#L836

		const auto editor{ _editor.get() };
		if (!editor)
		{
			throw hresult_error{ E_POINTER };
		}

		// Todo: see MicaEditorControlAutomationPeer::GetVisibleRanges for caveats
		// Todo: Does not account for horizontal scrolling
		// Todo: Does not account for ranges that do not start and end at line starts/ends

		const auto firstVisibleLine{ editor.FirstVisibleLine() };
		const auto maxLine{ editor.LineCount() - 1 };
		const auto lastVisibleLine{ std::min(maxLine, firstVisibleLine + editor.LinesOnScreen()) };

		const auto startLine{ editor.LineFromPosition(_rangeStart) };
		const auto endLine{ editor.LineFromPosition(_rangeEnd) };

		const auto top{ std::max(startLine, firstVisibleLine) };
		const auto end{ std::min(endLine, lastVisibleLine) };
		const auto count{ end - top + 1 };

		if (count < 1)
		{
			returnValue = {};
			return;
		}

		auto margins{ editor.MarginLeft() };
		const auto nMargins{ editor.Margins() };
		for (auto i{ 0 }; i < nMargins; i++)
		{
			margins += editor.GetMarginWidthN(i);
		}
		const auto maxTextWidth{ static_cast<int>(_bounds.Width) - editor.MarginRight() };

		returnValue = com_array<double>(count * 4);
		// Todo: This code pretends that TextHeight gets the correct height including sublines (wrapping). It does not
		for (int64_t i = 0, j = 0; i < count; i++, j += 4)
		{
			const auto line{ top + i };
			const auto pos{ editor.PositionFromLine(line) };
			const auto xStart{ editor.PointXFromPosition(line == startLine ? _rangeStart : pos) };
			const auto xEnd{ editor.PointXFromPosition((line == endLine ? _rangeEnd : editor.GetLineEndPosition(line))) };
			const auto textStart{ std::max(margins, xStart) };
			returnValue[j + 0] = _bounds.X + textStart; // Left
			returnValue[j + 1] = _bounds.Y + editor.PointYFromPosition(pos); // Top
			returnValue[j + 2] = std::min(maxTextWidth, xEnd) - textStart; // Width
			returnValue[j + 3] = editor.TextHeight(line); // Height

			// Todo: Reconsider how off screen text is handled
			if (xEnd < margins)
			{
				returnValue[j + 0] = _bounds.X + margins;
				returnValue[j + 2] = 0;
			}
			if (xStart > maxTextWidth)
			{
				returnValue[j + 0] = _bounds.X + maxTextWidth;
				returnValue[j + 2] = 0;
			}
		}
	}

	IRawElementProviderSimple TextRangeProvider::GetEnclosingElement()
	{
		return _peer;
	}

	hstring TextRangeProvider::GetText(int32_t maxLength)
	{
		const auto editor{ _editor.get() };
		if (!editor)
		{
			throw hresult_error{ E_POINTER };
		}

		const auto length{ std::min(editor.Length() - _rangeStart, maxLength == -1 ? _rangeEnd - _rangeStart : std::min(_rangeEnd - _rangeStart, static_cast<int64_t>(maxLength))) };
		std::string s(length, '\0');
		Scintilla::TextRangeFull range
		{
			{ _rangeStart, _rangeStart + length, },
			&s[0],
		};
		editor.GetTextRangeFull(reinterpret_cast<uint64_t>(&range));
		return to_hstring(s);
	}

	int32_t TextRangeProvider::Move(TextUnit const &unit, int32_t count)
	{
		// Todo: Bounds checking and other units etc. and make sure handling this right
		if (unit == TextUnit::Character)
		{
			_rangeStart += count;
			_rangeEnd += count;
			return count;
		}

		return 0;
	}

	int32_t TextRangeProvider::MoveEndpointByUnit(TextPatternRangeEndpoint const &endpoint, TextUnit const &unit, int32_t count)
	{
		// Todo: Bounds checking and other units etc.
		if (endpoint == TextPatternRangeEndpoint::Start && unit == TextUnit::Character)
		{
			_rangeStart += count;
			return count;
		}
		else if (endpoint == TextPatternRangeEndpoint::End && unit == TextUnit::Character)
		{
			_rangeEnd += count;
			return count;
		}

		return 0;
	}

	void TextRangeProvider::MoveEndpointByRange(TextPatternRangeEndpoint const &endpoint, ITextRangeProvider const &textRangeProvider, TextPatternRangeEndpoint const &targetEndpoint)
	{
		const auto target{ textRangeProvider.as<TextRangeProvider>() };
		const auto src{ targetEndpoint == TextPatternRangeEndpoint::Start ? target->_rangeStart : target->_rangeEnd };
		if (endpoint == TextPatternRangeEndpoint::Start)
		{
			_rangeStart = src;
			if (_rangeStart > _rangeEnd)
			{
				_rangeEnd = _rangeStart;
			}
		}
		else
		{
			_rangeEnd = src;
			if (_rangeStart > _rangeEnd)
			{
				_rangeStart = _rangeEnd;
			}
		}
	}

	void TextRangeProvider::Select()
	{
		const auto editor{ _editor.get() };
		if (!editor)
		{
			throw hresult_error{ E_POINTER };
		}

		editor.SetSelection(_rangeEnd, _rangeStart);
	}

	void TextRangeProvider::AddToSelection()
	{
		const auto editor{ _editor.get() };
		if (!editor)
		{
			throw hresult_error{ E_POINTER };
		}

		editor.AddSelection(_rangeEnd, _rangeStart);
	}

	void TextRangeProvider::RemoveFromSelection()
	{
		const auto editor{ _editor.get() };
		if (!editor)
		{
			throw hresult_error{ E_POINTER };
		}

		const auto count{ editor.Selections() };
		for (auto i{ 0 }; i < count; i++)
		{
			if (_rangeStart == editor.GetSelectionNStart(i)
				&& _rangeEnd == editor.GetSelectionNEnd(i))
			{
				editor.DropSelectionN(i);
				return;
			}
		}
	}

	void TextRangeProvider::ScrollIntoView(bool alignToTop)
	{
		const auto editor{ _editor.get() };
		if (!editor)
		{
			throw hresult_error{ E_POINTER };
		}

		// Todo: You would want primary to be the caret and secondary to be anchor rather than start and end
		// Todo: see if we can honor alignToTop
		editor.ScrollRange(_rangeEnd, _rangeStart);
	}

	com_array<IRawElementProviderSimple> TextRangeProvider::GetChildren()
	{
		return {};
	}
}
