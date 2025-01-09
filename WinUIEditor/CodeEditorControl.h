#pragma once

#include "CodeEditorControl.g.h"

#include "CodeEditorHandler.h"
#include "EditorBaseControl.h"

namespace winrt::WinUIEditor::implementation
{
	struct CodeEditorControl : CodeEditorControlT<CodeEditorControl>, ::WinUIEditor::CodeEditorHandler
	{
		CodeEditorControl();
		int64_t SendMessage(ScintillaMessage const &message, uint64_t wParam, int64_t lParam);
		WinUIEditor::Editor Editor();
		void OnApplyTemplate();
		void OnKeyDown(DUX::Input::KeyRoutedEventArgs const &e);

		hstring HighlightingLanguage();
		void HighlightingLanguage(hstring const &value);

		event_token NotifyMessageReceived(Windows::Foundation::EventHandler<int64_t> const &handler);
		void NotifyMessageReceived(event_token const &token) noexcept;

		event_token DefaultColorsChanged(Windows::Foundation::EventHandler<DUX::ElementTheme> const &handler);
		void DefaultColorsChanged(event_token const &token) noexcept;

		event_token SyntaxHighlightingApplied(Windows::Foundation::EventHandler<DUX::ElementTheme> const &handler);
		void SyntaxHighlightingApplied(event_token const &token) noexcept;

		void ApplyDefaultsToDocument();
		void ResetLexer();

	private:
		com_ptr<EditorBaseControl> _editor{ nullptr };
		std::shared_ptr<Scintilla::ScintillaCall> _call{ nullptr };
		event<Windows::Foundation::EventHandler<int64_t>> _notifyMessageReceived;

		event<Windows::Foundation::EventHandler<DUX::ElementTheme>> _defaultColorsChangedEvent;
		event<Windows::Foundation::EventHandler<DUX::ElementTheme>> _syntaxHighlightingAppliedEvent;
		DUD::DispatcherQueue _dispatcherQueue{ nullptr };
#ifndef WINUI3
		bool _hasFcu{ Windows::Foundation::Metadata::ApiInformation::IsApiContractPresent(L"Windows.Foundation.UniversalApiContract", 5) }; // Todo: Make static
		bool _has1803{ Windows::Foundation::Metadata::ApiInformation::IsApiContractPresent(L"Windows.Foundation.UniversalApiContract", 6) }; // Todo: Make static
		bool _hasIsLoaded{ Windows::Foundation::Metadata::ApiInformation::IsPropertyPresent(L"Windows.UI.Xaml.FrameworkElement", L"IsLoaded") }; // Todo: Make static
		bool _isLoaded{ false };
#endif
		void OnLoaded(Windows::Foundation::IInspectable const &sender, DUX::RoutedEventArgs const &args);
		void OnUnloaded(Windows::Foundation::IInspectable const &sender, DUX::RoutedEventArgs const &args);
		bool IsLoadedCompat();
		void OnGettingFocus(Windows::Foundation::IInspectable const &sender, DUX::Input::GettingFocusEventArgs const &e);
		DUX::FrameworkElement::ActualThemeChanged_revoker _actualThemeChangedRevoker{};
		void OnActualThemeChanged(Windows::Foundation::IInspectable const &sender, Windows::Foundation::IInspectable const &e);
		void Editor_DpiChanged(Windows::Foundation::IInspectable const &sender, double value);
		void Editor_NotifyMessageReceived(Windows::Foundation::IInspectable const &sender, int64_t value);
#ifndef WINUI3
		DUX::ElementTheme LegacyActualTheme();
#endif
		void StyleSetFore(int style, Scintilla::ColourAlpha color) override;
		void StyleSetBack(int style, Scintilla::ColourAlpha color) override;
		void InvalidateStyleRedraw() override;
		void StyleClearCustom() override;
		void SetFoldMarginColor(bool useSetting, Scintilla::ColourAlpha back) override;
		void SetFoldMarginHiColor(bool useSetting, Scintilla::ColourAlpha fore) override;
		void SetCallTipBackgroundColor(Scintilla::ColourAlpha color) override;
		void SetCallTipHoverColor(Scintilla::ColourAlpha color) override;

		void DefaultColorsChanged(::WinUIEditor::CodeEditorTheme theme) override;
		void SyntaxHighlightingApplied(::WinUIEditor::CodeEditorTheme theme) override;
	};
}

namespace winrt::WinUIEditor::factory_implementation
{
	struct CodeEditorControl : CodeEditorControlT<CodeEditorControl, implementation::CodeEditorControl>
	{
	};
}
