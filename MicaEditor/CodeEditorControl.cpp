#include "pch.h"
#include "CodeEditorControl.h"
#if __has_include("CodeEditorControl.g.cpp")
#include "CodeEditorControl.g.cpp"
#endif
#include "MicaEditorControl.h"
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
}
