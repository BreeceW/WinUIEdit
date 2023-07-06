#include "pch.h"
#include "CodeEditorControl.h"
#if __has_include("CodeEditorControl.g.cpp")
#include "CodeEditorControl.g.cpp"
#endif
#include "Helpers.h"

using namespace ::MicaEditor;
using namespace winrt;
using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace DUD;
using namespace DUX;
using namespace DUX::Controls;
using namespace DUX::Input;

using namespace winrt;

namespace winrt::MicaEditor::implementation
{
	CodeEditorControl::CodeEditorControl()
	{
		DefaultStyleKey(winrt::box_value(L"MicaEditor.CodeEditorControl"));

		_editor = make_self<MicaEditorControl>();
		AddKeyboardShortcuts();
		ChangeDefaults();
		_editor->DpiChanged({ this, &CodeEditorControl::Editor_DpiChanged });
		_editor->ScintillaNotification({ this, &CodeEditorControl::Editor_ScintillaNotification });

		_gettingFocusRevoker = GettingFocus(auto_revoke, { this, &CodeEditorControl::OnGettingFocus });

#ifndef WINUI3
		if (_hasFcu)
		{
#endif
			_dispatcherQueue = DUD::DispatcherQueue::GetForCurrentThread();
#ifndef WINUI3
		}
#endif
	}

	void CodeEditorControl::OnApplyTemplate()
	{
		__super::OnApplyTemplate();

		if (const auto presenter{ GetTemplateChild(L"EditorContainer").try_as<ContentPresenter>() })
		{
			presenter.Content(_editor.as<MicaEditor::MicaEditorControl>());
		}
	}

	void CodeEditorControl::OnGettingFocus(IInspectable const &sender, GettingFocusEventArgs const &e)
	{
		if (e.NewFocusedElement() == *this && (
#ifndef WINUI3
			!_has1803 ||
#endif
			!e.TrySetNewFocusedElement(*_editor)))
		{
			const auto focusState{ e.FocusState() };
#ifndef WINUI3
			if (_hasFcu)
			{
#endif
				_dispatcherQueue.TryEnqueue([this, focusState]()
					{
						_editor->Focus(focusState);
					});
#ifndef WINUI3
			}
			else
			{
#endif
				Dispatcher().RunAsync(CoreDispatcherPriority::Normal, [this, focusState]()
					{
						_editor->Focus(focusState);
					});
#ifndef WINUI3
			}
#endif
			e.Handled(true);
		}
	}

	uint64_t CodeEditorControl::Scintilla(ScintillaMessage const &message, uint64_t wParam, uint64_t lParam)
	{
		return _editor->PublicWndProc(static_cast<Scintilla::Message>(message), wParam, lParam);
	}

	MicaEditor::Editor CodeEditorControl::Editor()
	{
		return _editor->Editor();
	}

	void CodeEditorControl::Editor_DpiChanged(IInspectable const &sender, double value)
	{
		_dpiScale = value;
		_editor->PublicWndProc(Scintilla::Message::SetXCaretPolicy, CARET_SLOP | CARET_STRICT | CARET_EVEN, ConvertFromDipToPixelUnit(24, value));
		// Todo: Replace placeholder 1 for caret width from UISettings
		_editor->PublicWndProc(Scintilla::Message::SetCaretWidth, ConvertFromDipToPixelUnit(1, value), 0); // Todo: Needs to stop blinking after timeout and respect blink rate
		UpdateZoom();
	}

	void CodeEditorControl::Editor_ScintillaNotification(IInspectable const &sender, uint64_t value)
	{
		const auto data{ reinterpret_cast<Scintilla::NotificationData *>(value) };
		if (data->nmhdr.code == Scintilla::Notification::Zoom
			|| (data->nmhdr.code == Scintilla::Notification::Modified && data->linesAdded))
		{
			UpdateZoom();
		}
	}

	void CodeEditorControl::UpdateZoom()
	{
		const auto size{ _editor->PublicWndProc(Scintilla::Message::StyleGetSizeFractional, static_cast<Scintilla::uptr_t>(Scintilla::StylesCommon::Default), 0) };
		const auto zoom{ static_cast<int>(_editor->PublicWndProc(Scintilla::Message::GetZoom, 0, 0)) };
		const auto factor{ static_cast<float>((size + zoom * 100)) / size };
		// Todo: Width 11 hard coded for a digit. Use below for real value:
		// _editor->PublicWndProc(Scintilla::Message::TextWidth, static_cast<Scintilla::uptr_t>(Scintilla::StylesCommon::LineNumber), reinterpret_cast<Scintilla::sptr_t>("9"))
		const auto line{ _editor->PublicWndProc(Scintilla::Message::GetLineCount, 0, 0) };
		const auto width{ 12 + 11 * std::max(3, static_cast<int>(std::floor(std::log10(line) + 1))) };
		_editor->PublicWndProc(Scintilla::Message::SetMarginWidthN, 0, ConvertFromDipToPixelUnit(std::floorf(factor * width + 0.5f), _dpiScale));
		_editor->PublicWndProc(Scintilla::Message::SetMarginLeft, 0, ConvertFromDipToPixelUnit(std::floorf(factor * 23 + 0.5f), _dpiScale));
	}

	void CodeEditorControl::AddKeyboardShortcuts()
	{
		_editor->PublicWndProc(Scintilla::Message::AssignCmdKey, 187 + (SCMOD_CTRL << 16), SCI_ZOOMIN); // Ctrl+Plus
		_editor->PublicWndProc(Scintilla::Message::AssignCmdKey, 189 + (SCMOD_CTRL << 16), SCI_ZOOMOUT); // Ctrl+Minus
		_editor->PublicWndProc(Scintilla::Message::AssignCmdKey, 48 + (SCMOD_CTRL << 16), SCI_SETZOOM); // Ctrl+0
	}

	void CodeEditorControl::ChangeDefaults()
	{
		_editor->PublicWndProc(Scintilla::Message::SetMultipleSelection, true, 0);
		_editor->PublicWndProc(Scintilla::Message::SetScrollWidth, 2000, 0);
		_editor->PublicWndProc(Scintilla::Message::SetScrollWidthTracking, true, 0);
		_editor->PublicWndProc(Scintilla::Message::SetYCaretPolicy, CARET_SLOP | CARET_STRICT | CARET_EVEN, 1);
		_editor->PublicWndProc(Scintilla::Message::SetVisiblePolicy, VISIBLE_SLOP, 0);
		_editor->PublicWndProc(Scintilla::Message::SetHScrollBar, true, 0);
		_editor->PublicWndProc(Scintilla::Message::SetEndAtLastLine, false, 0);
		_editor->PublicWndProc(Scintilla::Message::SetTabWidth, 4, 0);
		_editor->PublicWndProc(Scintilla::Message::SetMarginWidthN, 1, 0);
		_editor->PublicWndProc(Scintilla::Message::StyleSetFont, STYLE_DEFAULT, reinterpret_cast<Scintilla::sptr_t>("Cascadia Code"));
		_editor->PublicWndProc(Scintilla::Message::StyleSetSizeFractional, STYLE_DEFAULT, 11 * SC_FONT_SIZE_MULTIPLIER);
		_editor->PublicWndProc(Scintilla::Message::SetAdditionalSelectionTyping, 1, 0);
		_editor->PublicWndProc(Scintilla::Message::SetMultiPaste, SC_MULTIPASTE_EACH, 0);
		_editor->PublicWndProc(Scintilla::Message::SetLayoutThreads, 16, 0); // Todo: Determine performance impact
	}
}
