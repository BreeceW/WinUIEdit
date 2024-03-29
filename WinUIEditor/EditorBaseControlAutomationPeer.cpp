﻿#include "pch.h"
#include "EditorBaseControlAutomationPeer.h"
#include "EditorBaseControlAutomationPeer.g.cpp"
#include "TextRangeProvider.h"
#include "Helpers.h"
#include "EditorBaseControl.h"

using namespace ::WinUIEditor;
using namespace winrt;
using namespace Windows::Foundation;
using namespace DUX::Automation;
using namespace DUX::Automation::Peers;
using namespace DUX::Automation::Provider;
using namespace DUX::Automation::Text;

namespace winrt::WinUIEditor::implementation
{
	EditorBaseControlAutomationPeer::EditorBaseControlAutomationPeer(WinUIEditor::EditorBaseControl const &owner) : base_type(owner)
	{
		_updateUIRevoker = owner.Editor().UpdateUI(auto_revoke, { this, &EditorBaseControlAutomationPeer::Editor_UpdateUI });
	}

	void EditorBaseControlAutomationPeer::Editor_UpdateUI(Editor const &sender, UpdateUIEventArgs const &args)
	{
		// https://github.com/microsoft/terminal/blob/main/src/cascadia/TerminalControl/TermControlAutomationPeer.cpp#L133
		// Todo: Maybe we should raise selection changed on Update::Content also
		if (static_cast<int>(static_cast<Update>(args.Updated()) & Update::Selection))
		{
			this->RaiseAutomationEvent(AutomationEvents::TextPatternOnTextSelectionChanged);
		}
	}

	hstring EditorBaseControlAutomationPeer::GetLocalizedControlTypeCore()
	{
		// Todo: Localize
		// Todo: Make sure this is a good name
		return L"code editor";
	}

	IInspectable EditorBaseControlAutomationPeer::GetPatternCore(PatternInterface const &patternInterface)
	{
		// Todo: Should we forward scroll elements? https://learn.microsoft.com/en-us/windows/apps/design/accessibility/custom-automation-peers#forwarding-patterns-from-sub-elements
		switch (patternInterface)
		{
		case PatternInterface::TextEdit:
		case PatternInterface::Text:
		case PatternInterface::Text2:
		case PatternInterface::Value:
			return *this;
		default:
			return __super::GetPatternCore(patternInterface);
		}
	}

	hstring EditorBaseControlAutomationPeer::GetClassNameCore()
	{
		return hstring{ name_of<WinUIEditor::EditorBaseControl>() };
	}

	AutomationControlType EditorBaseControlAutomationPeer::GetAutomationControlTypeCore()
	{
		return AutomationControlType::Edit;
	}

	ITextRangeProvider EditorBaseControlAutomationPeer::DocumentRange()
	{
		// Todo: Look at how WinUI 2 handles GetImpl in automation peers
		const auto editor{ Owner().as<WinUIEditor::EditorBaseControl>().Editor() };
		return make<TextRangeProvider>(ProviderFromPeer(*this), editor, 0, editor.Length(), GetBoundingRectangle());
	}

	SupportedTextSelection EditorBaseControlAutomationPeer::SupportedTextSelection()
	{
		const auto editor{ Owner().as<WinUIEditor::EditorBaseControl>().Editor() };
		return editor.MultipleSelection() ? SupportedTextSelection::Multiple : SupportedTextSelection::Single;
	}

	com_array<ITextRangeProvider> EditorBaseControlAutomationPeer::GetSelection()
	{
		const auto editor{ Owner().as<WinUIEditor::EditorBaseControl>().Editor() };
		const auto n{ static_cast<uint32_t>(editor.Selections()) };
		com_array<ITextRangeProvider> arr(n, nullptr);
		const auto topLeft{ GetBoundingRectangle() };
		for (uint32_t i{ 0 }; i < n; i++)
		{
			const auto start{ editor.GetSelectionNStart(i) };
			const auto end{ editor.GetSelectionNEnd(i) };
			arr[i] = make<TextRangeProvider>(ProviderFromPeer(*this), editor, start, end, topLeft);
		}
		return arr; // Todo: Trying to retrieve the value of a selection n >= 1 crashes Accessibility Insights
	}

	com_array<ITextRangeProvider> EditorBaseControlAutomationPeer::GetVisibleRanges()
	{
		// Visual Studio implements this line by line
		// Todo: Consider folding and line visible messages
		// Todo: Not sure if this is the best method for top line
		const auto editor{ Owner().as<WinUIEditor::EditorBaseControl>().Editor() };

		const auto firstVisibleLine{ editor.FirstVisibleLine() };
		const auto maxLine{ editor.LineCount() - 1 };
		// Todo: LinesOnScreen has no concept of wrapping, so you could end up with many more lines than truly visibly
		const auto lastVisibleLine{ std::min(maxLine, firstVisibleLine + editor.LinesOnScreen()) };

		const auto count{ lastVisibleLine - firstVisibleLine + 1 };

		com_array<ITextRangeProvider> arr(count, nullptr);
		const auto topLeft{ GetBoundingRectangle() };
		for (int64_t i{ 0 }; i < count; i++)
		{
			const auto line{ firstVisibleLine + i };
			const auto start{ editor.PositionFromLine(line) };
			const auto end{ editor.GetLineEndPosition(line) };
			arr[i] = make<TextRangeProvider>(ProviderFromPeer(*this), editor, start, end, topLeft);
		}
		return arr;
	}

	ITextRangeProvider EditorBaseControlAutomationPeer::RangeFromChild(IRawElementProviderSimple const &childElement)
	{
		// Todo: Does this need to be implemented?
		return nullptr;
	}

	ITextRangeProvider EditorBaseControlAutomationPeer::RangeFromPoint(Point const &screenLocation)
	{
		const auto rect{ GetBoundingRectangle() };
		const Point point{
			screenLocation.X - rect.X,
			screenLocation.Y - rect.Y };

		const auto editor{ Owner().as<WinUIEditor::EditorBaseControl>().Editor() };
		const auto pos{ editor.PositionFromPoint(point.X, point.Y) };
		const auto line{ editor.LineFromPosition(pos) };
		const auto start{ editor.PositionFromLine(line) };
		const auto end{ editor.GetLineEndPosition(line) };

		return make<TextRangeProvider>(ProviderFromPeer(*this), editor, start, end, rect);
	}

	ITextRangeProvider EditorBaseControlAutomationPeer::GetActiveComposition()
	{
		// Todo: implement
		throw hresult_not_implemented{};
	}

	ITextRangeProvider EditorBaseControlAutomationPeer::GetConversionTarget()
	{
		// Todo: implement
		throw hresult_not_implemented{};
	}

	ITextRangeProvider EditorBaseControlAutomationPeer::RangeFromAnnotation(IRawElementProviderSimple const &annotationElement)
	{
		// Todo: implement
		throw hresult_not_implemented{};
	}

	ITextRangeProvider EditorBaseControlAutomationPeer::GetCaretRange(bool &isActive)
	{
		const auto editor{ Owner().as<WinUIEditor::EditorBaseControl>().Editor() };

		isActive = editor.Focus();

		const auto pos{ editor.GetSelectionNCaret(editor.MainSelection()) };

		return make<TextRangeProvider>(ProviderFromPeer(*this), editor, pos, pos, GetBoundingRectangle());
	}

	bool EditorBaseControlAutomationPeer::IsReadOnly()
	{
		const auto editor{ Owner().as<WinUIEditor::EditorBaseControl>().Editor() };

		return editor.ReadOnly();
	}

	hstring EditorBaseControlAutomationPeer::Value()
	{
		const auto editor{ Owner().as<WinUIEditor::EditorBaseControl>().Editor() };

		return editor.GetText(editor.Length()); // Todo: this could potentially be gigabytes of data, so provide a maximum safeguard
	}

	void EditorBaseControlAutomationPeer::SetValue(hstring const &value)
	{
		const auto editor{ Owner().as<WinUIEditor::EditorBaseControl>().Editor() };

		editor.SetText(value);
	}
}
