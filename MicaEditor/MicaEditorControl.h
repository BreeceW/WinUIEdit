﻿#pragma once

#include "MicaEditorControl.g.h"

#include "ScintillaWin.h"
#include "Wrapper.h"

namespace winrt::MicaEditor::implementation
{
	struct MicaEditorControl : MicaEditorControlT<MicaEditorControl>
	{
		MicaEditorControl();
		~MicaEditorControl();

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

		Scintilla::sptr_t PublicWndProc(Scintilla::Message iMessage, Scintilla::uptr_t wParam, Scintilla::sptr_t lParam);
		uint64_t Scintilla(ScintillaMessage const &message, uint64_t wParam, uint64_t lParam);

		void StyleSetBackTransparent(int style, Scintilla::Internal::ColourRGBA color);

		event_token DpiChanged(Windows::Foundation::EventHandler<double> const &handler);
		void DpiChanged(event_token const &token) noexcept;

		event_token ScintillaNotification(Windows::Foundation::EventHandler<uint64_t> const &handler);
		void ScintillaNotification(event_token const &token) noexcept;

	private:
#ifndef WINUI3
		bool _hasFcu{ Windows::Foundation::Metadata::ApiInformation::IsApiContractPresent(L"Windows.Foundation.UniversalApiContract", 5) }; // Todo: Make static
		bool _hasXamlRoot{ Windows::Foundation::Metadata::ApiInformation::IsTypePresent(L"Windows.UI.Xaml.XamlRoot") }; // Todo: Make static
		bool _hasIsLoaded{ Windows::Foundation::Metadata::ApiInformation::IsPropertyPresent(L"Windows.UI.Xaml.FrameworkElement", L"IsLoaded") }; // Todo: Make static
		bool _isPointerOver{ false };
		Windows::Graphics::Display::DisplayInformation _displayInformation{ nullptr };
		bool _isLoaded{ false };
#endif
		bool _isFocused{ false };
		bool _isContextMenuOpen{ false };
		MicaEditor::Editor _editorWrapper{ nullptr };
		com_ptr<::Scintilla::Internal::ScintillaWinUI> _scintilla{ nullptr };
		float _dpiScale;
		event<Windows::Foundation::EventHandler<double>> _dpiChangedEvent;
		event<Windows::Foundation::EventHandler<uint64_t>> _scintillaNotificationEvent;
		DUXC::Image::SizeChanged_revoker _imageTargetSizeChangedRevoker{};
		void ImageTarget_SizeChanged(Windows::Foundation::IInspectable const &sender, DUX::SizeChangedEventArgs const &args);
		DUXC::Image::PointerWheelChanged_revoker _imageTargetPointerWheelChangedRevoker{};
		void ImageTarget_PointerWheelChanged(Windows::Foundation::IInspectable const &sender, DUX::Input::PointerRoutedEventArgs const &e);
		DUXC::Image::DragEnter_revoker _imageTargetDragEnterRevoker{};
		void ImageTarget_DragEnter(Windows::Foundation::IInspectable const &sender, DUX::DragEventArgs const &e);
		DUXC::Image::DragOver_revoker _imageTargetDragOverRevoker{};
		void ImageTarget_DragOver(Windows::Foundation::IInspectable const &sender, DUX::DragEventArgs const &e);
		DUXC::Image::DragLeave_revoker _imageTargetDragLeaveRevoker{};
		void ImageTarget_DragLeave(Windows::Foundation::IInspectable const &sender, DUX::DragEventArgs const &e);
		DUXC::Image::Drop_revoker _imageTargetDropRevoker{};
		void ImageTarget_Drop(Windows::Foundation::IInspectable const &sender, DUX::DragEventArgs const &e);
		DUXC::Image::DragStarting_revoker _imageTargetDragStartingRevoker{};
		void ImageTarget_DragStarting(DUX::UIElement const &sender, DUX::DragStartingEventArgs const &e);
		DUXC::Image::ContextRequested_revoker _imageTargetContextRequestedRevoker{};
		void ImageTarget_ContextRequested(DUX::UIElement const &sender, DUX::Input::ContextRequestedEventArgs const &e);
		void ContextMenuItem_Click(Windows::Foundation::IInspectable const &sender, DUX::RoutedEventArgs const &e);
		DUXC::Primitives::ScrollBar::Scroll_revoker _horizontalScrollBarScrollRevoker{};
		void HorizontalScrollBar_Scroll(Windows::Foundation::IInspectable const &sender, DUX::Controls::Primitives::ScrollEventArgs const &e);
		DUXC::Primitives::ScrollBar::Scroll_revoker _verticalScrollBarScrollRevoker{};
		void VerticalScrollBar_Scroll(Windows::Foundation::IInspectable const &sender, DUX::Controls::Primitives::ScrollEventArgs const &e);
		void MicaEditorControl_CharacterReceived(DUX::UIElement const &sender, DUX::Input::CharacterReceivedRoutedEventArgs const &args);
		void OnLoaded(Windows::Foundation::IInspectable const &sender, DUX::RoutedEventArgs const &args);
		void OnUnloaded(Windows::Foundation::IInspectable const &sender, DUX::RoutedEventArgs const &args);
		bool IsLoadedCompat();
		DUX::XamlRoot::Changed_revoker _xamlRootChangedRevoker{};
		void XamlRoot_Changed(DUX::XamlRoot const &sender, DUX::XamlRootChangedEventArgs const &args);
		void UpdateDpi(float dpiScale);
		void AddContextMenuItems(DUX::Controls::MenuFlyout const &menu);
		bool ShowContextMenu(DUX::UIElement const &targetElement, Windows::Foundation::Point const &point);
		bool ShowContextMenuAtCurrentPosition();
		winrt::com_ptr<::IVirtualSurfaceImageSourceNative> _vsisNative;
		std::shared_ptr<::MicaEditor::Wrapper> _wrapper{ nullptr };
		static LRESULT WndProc(Windows::Foundation::IInspectable const &, UINT msg, WPARAM wParam, LPARAM lParam);
#ifndef WINUI3
		Windows::Graphics::Display::DisplayInformation::DpiChanged_revoker _dpiChangedRevoker{};
		void DisplayInformation_DpiChanged(Windows::Graphics::Display::DisplayInformation const &sender, Windows::Foundation::IInspectable const &args);
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
