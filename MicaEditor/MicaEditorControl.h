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
		MicaEditor::Editor Editor();

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
		void OnKeyUp(DUX::Input::KeyRoutedEventArgs const &e);

		uint64_t Scintilla(ScintillaMessage const &message, uint64_t wParam, uint64_t lParam);

	private:
		static void OnTextPropertyChanged(IInspectable const &sender, DUX::DependencyPropertyChangedEventArgs const &args);
		inline static DUX::DependencyProperty s_textProperty{ DUX::DependencyProperty::Register(L"Text", xaml_typename<hstring>(), xaml_typename<MicaEditor::MicaEditorControl>(), DUX::PropertyMetadata{box_value(L""), &OnTextPropertyChanged}) };

#ifndef WINUI3
		bool _hasFcu{ Windows::Foundation::Metadata::ApiInformation::IsApiContractPresent(L"Windows.Foundation.UniversalApiContract", 5) }; // Todo: Make static
		bool _isPointerOver{ false };
#endif
		bool _isFocused{ false };
		bool _isContextMenuOpen{ false };
		MicaEditor::Editor _editorWrapper{ nullptr };
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
		void ImageTarget_PointerWheelChanged(Windows::Foundation::IInspectable const &sender, DUX::Input::PointerRoutedEventArgs const &e);
		void ImageTarget_DragEnter(Windows::Foundation::IInspectable const &sender, DUX::DragEventArgs const &e);
		void ImageTarget_DragOver(Windows::Foundation::IInspectable const &sender, DUX::DragEventArgs const &e);
		void ImageTarget_DragLeave(Windows::Foundation::IInspectable const &sender, DUX::DragEventArgs const &e);
		void ImageTarget_Drop(Windows::Foundation::IInspectable const &sender, DUX::DragEventArgs const &e);
		void ImageTarget_DragStarting(DUX::UIElement const &sender, DUX::DragStartingEventArgs const &e);
		void ImageTarget_ContextRequested(DUX::UIElement const &sender, DUX::Input::ContextRequestedEventArgs const &e);
		void ContextMenuItem_Click(Windows::Foundation::IInspectable const &sender, DUX::RoutedEventArgs const &e);
		void HorizontalScrollBar_Scroll(Windows::Foundation::IInspectable const &sender, DUX::Controls::Primitives::ScrollEventArgs const &e);
		void VerticalScrollBar_Scroll(Windows::Foundation::IInspectable const &sender, DUX::Controls::Primitives::ScrollEventArgs const &e);
		void MicaEditorControl_CharacterReceived(DUX::UIElement const &sender, DUX::Input::CharacterReceivedRoutedEventArgs const &args);
		void OnUnloaded(Windows::Foundation::IInspectable const &sender, DUX::RoutedEventArgs const &args);
		void UpdateDisplayInformation(float dpiScale, float logicalDpi);
		void UpdateSizes();
		void UpdateColors(bool useDarkTheme);
		void AddContextMenuItems(DUX::Controls::MenuFlyout const &menu);
		void ShowContextMenuAtCurrentPosition();
		winrt::com_ptr<::IVirtualSurfaceImageSourceNative> _vsisNative;
		std::shared_ptr<::MicaEditor::Wrapper> _wrapper{ nullptr };
		static LRESULT WndProc(Windows::Foundation::IInspectable const &, UINT msg, WPARAM wParam, LPARAM lParam);
		Windows::UI::ViewManagement::UISettings _uiSettings{};
#ifndef WINUI3
		Windows::UI::Xaml::Application::Suspending_revoker _suspendingRevoker{};
		void Application_Suspending(Windows::Foundation::IInspectable const &sender, Windows::ApplicationModel::SuspendingEventArgs const &args);
#endif
	};
}

namespace winrt::MicaEditor::factory_implementation
{
	struct MicaEditorControl : MicaEditorControlT<MicaEditorControl, implementation::MicaEditorControl>
	{
	};
}
