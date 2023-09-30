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
using namespace Windows::System;
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
		_call = _editor->Call();
		SetScintilla(_call);
		SetLexilla(CreateLexer);
		Initialize();
		_editor->DpiChanged({ this, &CodeEditorControl::Editor_DpiChanged });
		_editor->ScintillaNotification({ this, &CodeEditorControl::Editor_ScintillaNotification });

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
			presenter.Content(_editor.as<MicaEditor::MicaEditorControl>());
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

	void CodeEditorControl::OnActualThemeChanged(IInspectable const &sender, IInspectable const &e)
	{
		UpdateColors(XamlToCodeEditorTheme(ActualTheme()));
	}

	void CodeEditorControl::Editor_DpiChanged(IInspectable const &sender, double value)
	{
		UpdateDpi(value);
	}

	void CodeEditorControl::Editor_ScintillaNotification(IInspectable const &sender, uint64_t value)
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

	void CodeEditorControl::DefaultColorsChanged(CodeEditorTheme theme)
	{
		_defaultColorsChangedEvent(*this, CodeEditorToXamlTheme(theme));
	}

	void CodeEditorControl::SyntaxHighlightingApplied(CodeEditorTheme theme)
	{
		_syntaxHighlightingAppliedEvent(*this, CodeEditorToXamlTheme(theme));
	}
}
