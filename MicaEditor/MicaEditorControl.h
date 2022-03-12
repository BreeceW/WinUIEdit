#pragma once

#include "MicaEditorControl.g.h"

namespace winrt::MicaEditor::implementation
{
	struct MicaEditorControl : MicaEditorControlT<MicaEditorControl>
	{
		MicaEditorControl();

		int32_t MyProperty();
		void MyProperty(int32_t value);

		void OnApplyTemplate();
		void OnGotFocus(DUX::RoutedEventArgs const &e);
		void OnLostFocus(DUX::RoutedEventArgs const &e);
		void OnPointerPressed(DUX::Input::PointerRoutedEventArgs const &e);

		uint64_t Scintilla(int32_t message, uint64_t wParam, uint64_t lParam);

	private:
		int32_t _myProperty = 0;
		com_ptr<::Scintilla::Internal::ScintillaWinUI> _scintilla{ nullptr };
		void Image_Tapped(Windows::Foundation::IInspectable const &sender, DUX::Input::TappedRoutedEventArgs const &args);
		float _dpiScale = 1;
		Windows::Graphics::Display::DisplayInformation::DpiChanged_revoker _dpiChangedRevoker{};
		void OnDpiChanged(Windows::Graphics::Display::DisplayInformation const &sender, Windows::Foundation::IInspectable const &args);
		void MicaEditorControl::OnSizeChanged(const Windows::Foundation::IInspectable &sender, const DUX::SizeChangedEventArgs &args);
		void UpdateDisplayInformation(Windows::Graphics::Display::DisplayInformation const &displayInformation);
		winrt::com_ptr<::IVirtualSurfaceImageSourceNative> _vsisNative;
		std::shared_ptr<::MicaEditor::Wrapper> _wrapper{ nullptr };
	};
}

namespace winrt::MicaEditor::factory_implementation
{
	struct MicaEditorControl : MicaEditorControlT<MicaEditorControl, implementation::MicaEditorControl>
	{
	};
}
