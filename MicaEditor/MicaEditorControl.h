#pragma once

#include "MicaEditorControl.g.h"

namespace winrt::MicaEditor::implementation
{
	struct MicaEditorControl : MicaEditorControlT<MicaEditorControl>
	{
		MicaEditorControl();

		static DUX::DependencyProperty TextProperty() { return s_textProperty; }
		hstring Text();
		void Text(hstring const &value);

		void OnApplyTemplate(); // Should these have override?
		void OnGotFocus(DUX::RoutedEventArgs const &e);
		void OnLostFocus(DUX::RoutedEventArgs const &e);
		void OnPointerPressed(DUX::Input::PointerRoutedEventArgs const &e);
		void OnPointerMoved(DUX::Input::PointerRoutedEventArgs const &e);
		void OnPointerReleased(DUX::Input::PointerRoutedEventArgs const &e);
#ifndef WINUI3
		void OnPointerCaptureLost(DUX::Input::PointerRoutedEventArgs const &e);
		void OnPointerEntered(DUX::Input::PointerRoutedEventArgs const &e);
		void OnPointerExited(DUX::Input::PointerRoutedEventArgs const &e);
#endif
		void OnKeyDown(DUX::Input::KeyRoutedEventArgs const &e);
		void OnCharacterReceived(DUX::Input::CharacterReceivedRoutedEventArgs const &e);

		uint64_t Scintilla(int32_t message, uint64_t wParam, uint64_t lParam);

	private:
		static void OnTextPropertyChanged(IInspectable const &sender, DUX::DependencyPropertyChangedEventArgs const &args);
		inline static DUX::DependencyProperty s_textProperty{ DUX::DependencyProperty::Register(L"Text", xaml_typename<hstring>(), xaml_typename<MicaEditor::MicaEditorControl>(), DUX::PropertyMetadata{box_value(L""), &OnTextPropertyChanged}) };

#ifndef WINUI3
		bool _hasFcu{ Windows::Foundation::Metadata::ApiInformation::IsApiContractPresent(L"Windows.Foundation.UniversalApiContract", 5) }; // Todo: Make static
		bool _isPointerOver{ false };
#endif
		com_ptr<::Scintilla::Internal::ScintillaWinUI> _scintilla{ nullptr };
		float _dpiScale = 1;
		float _logicalDpi = 96;
		Windows::Graphics::Display::DisplayInformation::DpiChanged_revoker _dpiChangedRevoker{};
		void OnDpiChanged(Windows::Graphics::Display::DisplayInformation const &sender, Windows::Foundation::IInspectable const &args);
		void OnActualThemeChanged(DUX::FrameworkElement const &sender, Windows::Foundation::IInspectable const &args);
#ifndef WINUI3
		Windows::UI::ViewManagement::UISettings::ColorValuesChanged_revoker _colorValuesChangedRevoker{};
		void OnColorValuesChanged(Windows::UI::ViewManagement::UISettings const &uiSettings, Windows::Foundation::IInspectable const &args);
		bool UseDarkColors();
#endif
		void ImageTarget_SizeChanged(Windows::Foundation::IInspectable const &sender, DUX::SizeChangedEventArgs const &args);
		void OnUnloaded(Windows::Foundation::IInspectable const &sender, DUX::RoutedEventArgs const &args);
		void UpdateDisplayInformation(float dpiScale, float logicalDpi);
		void UpdateSizes();
		void UpdateColors(bool useDarkTheme);
		winrt::com_ptr<::IVirtualSurfaceImageSourceNative> _vsisNative;
		std::shared_ptr<::MicaEditor::Wrapper> _wrapper{ nullptr };
		static LRESULT WndProc(Windows::Foundation::IInspectable const &, UINT msg, WPARAM wParam, LPARAM lParam);
		Windows::UI::ViewManagement::UISettings _uiSettings{};
	};
}

namespace winrt::MicaEditor::factory_implementation
{
	struct MicaEditorControl : MicaEditorControlT<MicaEditorControl, implementation::MicaEditorControl>
	{
	};
}
