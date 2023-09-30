#pragma once

#include "CodeEditorControl.g.h"

#include "CodeEditorHandler.h"
#include "MicaEditorControl.h"

namespace winrt::MicaEditor::implementation
{
	struct CodeEditorControl : CodeEditorControlT<CodeEditorControl>, ::MicaEditor::CodeEditorHandler
	{
		CodeEditorControl();
		uint64_t Scintilla(ScintillaMessage const &message, uint64_t wParam, uint64_t lParam);
		MicaEditor::Editor Editor();
		void OnApplyTemplate();
		void OnKeyDown(DUX::Input::KeyRoutedEventArgs const &e);

		hstring HighlightingLanguage();
		void HighlightingLanguage(hstring const &value);

		event_token DefaultColorsChanged(Windows::Foundation::EventHandler<DUX::ElementTheme> const &handler);
		void DefaultColorsChanged(event_token const &token) noexcept;

		event_token SyntaxHighlightingApplied(Windows::Foundation::EventHandler<DUX::ElementTheme> const &handler);
		void SyntaxHighlightingApplied(event_token const &token) noexcept;

	private:
		com_ptr<MicaEditorControl> _editor{ nullptr };
		std::shared_ptr<Scintilla::ScintillaCall> _call{ nullptr };

		hstring _highlightingLanguage;
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
		void Editor_ScintillaNotification(Windows::Foundation::IInspectable const &sender, uint64_t value);
#ifndef WINUI3
		DUX::ElementTheme LegacyActualTheme();
#endif
		void StyleSetFore(int style, Scintilla::ColourAlpha color) override;
		void StyleSetBack(int style, Scintilla::ColourAlpha color) override;
		void InvalidateStyleRedraw() override;
		void StyleClearCustom() override;

		void DefaultColorsChanged(::MicaEditor::CodeEditorTheme theme) override;
		void SyntaxHighlightingApplied(::MicaEditor::CodeEditorTheme theme) override;
	};
}

namespace winrt::MicaEditor::factory_implementation
{
	struct CodeEditorControl : CodeEditorControlT<CodeEditorControl, implementation::CodeEditorControl>
	{
	};
}
