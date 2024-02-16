#include "pch.h"
#include "CodeEditorControl.h"
#if __has_include("CodeEditorControl.g.cpp")
#include "CodeEditorControl.g.cpp"
#endif
#include "Helpers.h"

using namespace ::WinUIEditor;
using namespace winrt;
using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Windows::System;
using namespace DUD;
using namespace DUX;
using namespace DUX::Controls;
using namespace DUX::Input;

using namespace winrt;

namespace winrt::WinUIEditor::implementation
{
	CodeEditorControl::CodeEditorControl()
	{
		DefaultStyleKey(winrt::box_value(L"WinUIEditor.CodeEditorControl"));

		_editor = make_self<EditorBaseControl>();
		_call = _editor->Call();
		SetScintilla(_call);
		SetLexilla(CreateLexer);
		Initialize();
		_editor->DpiChanged({ this, &CodeEditorControl::Editor_DpiChanged });
		_editor->InternalNotifyMessageReceived.add({ this, &CodeEditorControl::Editor_NotifyMessageReceived });

		Loaded({ this, &CodeEditorControl::OnLoaded });
		Unloaded({ this, &CodeEditorControl::OnUnloaded });
		GettingFocus({ this, &CodeEditorControl::OnGettingFocus });

#ifndef WINUI3
		if (_hasFcu)
		{
#endif
			_dispatcherQueue = DUD::DispatcherQueue::GetForCurrentThread();
#ifndef WINUI3
		}
#endif
	}

	static constexpr CodeEditorTheme XamlToCodeEditorTheme(ElementTheme theme)
	{
		return static_cast<CodeEditorTheme>(theme);
	}

	static constexpr ElementTheme CodeEditorToXamlTheme(CodeEditorTheme theme)
	{
		return static_cast<ElementTheme>(theme);
	}

	void CodeEditorControl::OnApplyTemplate()
	{
		__super::OnApplyTemplate();

#ifndef WINUI3
		if (_hasFcu)
		{
#endif
			UpdateColors(XamlToCodeEditorTheme(ActualTheme()));
#ifndef WINUI3
		}
		else
		{
			UpdateColors(XamlToCodeEditorTheme(LegacyActualTheme()));
		}
#endif

		if (const auto presenter{ GetTemplateChild(L"EditorContainer").try_as<ContentPresenter>() })
		{
			presenter.Content(_editor.as<WinUIEditor::EditorBaseControl>());
		}
	}

	void CodeEditorControl::OnKeyDown(KeyRoutedEventArgs const &e)
	{
		__super::OnKeyDown(e);

		const auto modifiers{ GetKeyModifiersForCurrentThread() }; // Todo: Can we avoid calling this every time?

		if ((modifiers & VirtualKeyModifiers::Control) == VirtualKeyModifiers::Control)
		{
			switch (e.Key())
			{
			case VirtualKey::F2: // Todo: make customizable
			{
				if (_call->Focus())
				{
					ChangeAllOccurrences();
					e.Handled(true);
				}
			}
			break;
			}
		}
	}

	void CodeEditorControl::OnLoaded(IInspectable const &sender, DUX::RoutedEventArgs const &args)
	{
#ifndef WINUI3
		_isLoaded = true;
#endif

		if (!IsLoadedCompat())
		{
			return;
		}

#ifndef WINUI3
		if (_hasFcu)
		{
#endif
			UpdateColors(XamlToCodeEditorTheme(ActualTheme()));
			_actualThemeChangedRevoker = ActualThemeChanged(auto_revoke, { this, &CodeEditorControl::OnActualThemeChanged });
#ifndef WINUI3
		}
		else
		{
			UpdateColors(XamlToCodeEditorTheme(LegacyActualTheme()));
			// Todo: Add fallback for ActualThemeChanged
		}
#endif
	}

	void CodeEditorControl::OnUnloaded(IInspectable const &sender, DUX::RoutedEventArgs const &args)
	{
#ifndef WINUI3
		_isLoaded = false;
#endif

		if (IsLoadedCompat())
		{
			return;
		}

#ifndef WINUI3
		if (_hasFcu)
		{
#endif
			_actualThemeChangedRevoker.revoke();
#ifndef WINUI3
		}
		else
		{
			// Todo: Add fallback
		}
#endif
	}

	bool CodeEditorControl::IsLoadedCompat()
	{
#ifndef WINUI3
		if (_hasIsLoaded)
		{
#endif
			return IsLoaded();
#ifndef WINUI3
		}
		else
		{
			return _isLoaded;
		}
#endif
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
				Dispatcher().RunAsync(CoreDispatcherPriority::Normal, [this, focusState]()
					{
						_editor->Focus(focusState);
					});
			}
#endif
			e.Handled(true);
		}
	}

	int64_t CodeEditorControl::SendMessage(ScintillaMessage const &message, uint64_t wParam, int64_t lParam)
	{
		return _editor->PublicWndProc(static_cast<Scintilla::Message>(message), wParam, lParam);
	}

	WinUIEditor::Editor CodeEditorControl::Editor()
	{
		return _editor->Editor();
	}

	void CodeEditorControl::OnActualThemeChanged(IInspectable const &sender, IInspectable const &e)
	{
		UpdateColors(XamlToCodeEditorTheme(ActualTheme()));
	}

	void CodeEditorControl::Editor_DpiChanged(IInspectable const &sender, double value)
	{
		UpdateDpi(value);
	}

	void CodeEditorControl::Editor_NotifyMessageReceived(IInspectable const &sender, int64_t value)
	{
		ProcessNotification(reinterpret_cast<Scintilla::NotificationData *>(value));
	}

	hstring CodeEditorControl::HighlightingLanguage()
	{
		return _highlightingLanguage;
	}

	void CodeEditorControl::HighlightingLanguage(hstring const &value)
	{
		SetHighlightingLanguage(value);
	}

	event_token CodeEditorControl::DefaultColorsChanged(EventHandler<ElementTheme> const &handler)
	{
		return _defaultColorsChangedEvent.add(handler);
	}

	void CodeEditorControl::DefaultColorsChanged(event_token const &token) noexcept
	{
		_defaultColorsChangedEvent.remove(token);
	}

	event_token CodeEditorControl::SyntaxHighlightingApplied(EventHandler<ElementTheme> const &handler)
	{
		return _syntaxHighlightingAppliedEvent.add(handler);
	}

	void CodeEditorControl::SyntaxHighlightingApplied(event_token const &token) noexcept
	{
		_syntaxHighlightingAppliedEvent.remove(token);
	}

#ifndef WINUI3
	DUX::ElementTheme CodeEditorControl::LegacyActualTheme()
	{
		// Todo: Fully implement
		const auto requestedTheme{ RequestedTheme() };
		if (requestedTheme == ElementTheme::Default)
		{
			return Application::Current().RequestedTheme() == ApplicationTheme::Dark
				? ElementTheme::Dark
				: ElementTheme::Light;
		}
		else
		{
			return requestedTheme;
		}
	}
#endif

	void CodeEditorControl::StyleSetFore(int style, Scintilla::ColourAlpha color)
	{
		_editor->StyleSetForeTransparent(style, Scintilla::Internal::ColourRGBA{ color });
	}

	void CodeEditorControl::StyleSetBack(int style, Scintilla::ColourAlpha color)
	{
		_editor->StyleSetBackTransparent(style, Scintilla::Internal::ColourRGBA{ color });
	}

	void CodeEditorControl::InvalidateStyleRedraw()
	{
		_editor->InvalidateStyleRedraw();
	}

	void CodeEditorControl::StyleClearCustom()
	{
		_editor->StyleClearCustom();
	}

	void CodeEditorControl::SetFoldMarginColor(bool useSetting, Scintilla::ColourAlpha back)
	{
		_editor->SetFoldMarginColorTransparent(useSetting, Scintilla::Internal::ColourRGBA{ back });
	}

	void CodeEditorControl::SetFoldMarginHiColor(bool useSetting, Scintilla::ColourAlpha fore)
	{
		_editor->SetFoldMarginHiColorTransparent(useSetting, Scintilla::Internal::ColourRGBA{ fore });
	}

	void CodeEditorControl::DefaultColorsChanged(CodeEditorTheme theme)
	{
		_defaultColorsChangedEvent(*this, CodeEditorToXamlTheme(theme));
	}

	void CodeEditorControl::SyntaxHighlightingApplied(CodeEditorTheme theme)
	{
		_syntaxHighlightingAppliedEvent(*this, CodeEditorToXamlTheme(theme));
	}

	event_token CodeEditorControl::NotifyMessageReceived(EventHandler<int64_t> const &handler)
	{
		return _editor->NotifyMessageReceived(handler);
	}

	void CodeEditorControl::NotifyMessageReceived(event_token const &token) noexcept
	{
		_editor->NotifyMessageReceived(token);
	}
}
