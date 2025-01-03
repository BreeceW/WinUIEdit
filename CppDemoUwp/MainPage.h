#pragma once

#include "MainPage.g.h"

namespace winrt::CppDemoUwp::implementation
{
	struct MainPage : MainPageT<MainPage>
	{
		void InitializeComponent();

		void Editor_Loaded(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		void SettingsButton_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		Windows::Foundation::IAsyncAction OpenMenuItem_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		Windows::Foundation::IAsyncAction SaveMenuItem_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		Windows::Foundation::IAsyncAction SaveAsMenuItem_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		Windows::Foundation::IAsyncAction NewWindowMenuItem_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		fire_and_forget NewMenuItem_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		fire_and_forget ExitMenuItem_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		void ZoomInMenuItem_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		void ZoomOutMenuItem_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		void ZoomRestoreMenuItem_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		void StatusBarMenuItem_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		void WordWrapMenuItem_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		void DisableBidirectionalMenuItem_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		void LtrBidirectionalMenuItem_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		void RtlBidirectionalMenuItem_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		void TimeDateMenuItem_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		void CommandMenuItem_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		void HighlightingLanguageButton_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		void HighlightingLanguageItem_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		Windows::Foundation::IAsyncAction TestAppWindowMenuItem_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const &e);

	private:
		void Editor_UpdateUI(WinUIEditor::Editor const &sender, WinUIEditor::UpdateUIEventArgs const &args);
		void Editor_ZoomChanged(WinUIEditor::Editor const &sender, WinUIEditor::ZoomChangedEventArgs const &args);
		void Editor_SavePointReached(WinUIEditor::Editor const &sender, WinUIEditor::SavePointReachedEventArgs const &args);
		void Editor_SavePointLeft(WinUIEditor::Editor const &sender, WinUIEditor::SavePointLeftEventArgs const &args);
		WinUIEditor::Editor::UpdateUI_revoker _updateUIRevoker{};
		WinUIEditor::Editor::ZoomChanged_revoker _zoomChangedRevoker{};
		WinUIEditor::Editor::SavePointReached_revoker _savePointReachedRevoker{};
		WinUIEditor::Editor::SavePointLeft_revoker _savePointLeftRevoker{};
		Windows::UI::Core::Preview::SystemNavigationManagerPreview::CloseRequested_revoker _closeRequestedRevoker{};
		fire_and_forget OnCloseRequested(Windows::Foundation::IInspectable sender, Windows::UI::Core::Preview::SystemNavigationCloseRequestedPreviewEventArgs e);
		void OnActivated(Windows::UI::Core::CoreWindow const &sender, Windows::UI::Core::WindowActivatedEventArgs const &e);
		Windows::UI::Core::CoreWindow::Activated_revoker _activatedRevoker{};
		void Activated(bool active);
		bool _hasFcu{ Windows::Foundation::Metadata::ApiInformation::IsApiContractPresent(L"Windows.Foundation.UniversalApiContract", 5) }; // Todo: Make static
		Windows::Storage::StorageFile _activeFile{ nullptr };
		Windows::Foundation::IAsyncAction OpenAsync(Windows::Storage::StorageFile file);
		Windows::Foundation::IAsyncOperation<bool> SaveAsync();
		Windows::Foundation::IAsyncAction SaveAsync(Windows::Storage::StorageFile file);
		Windows::Foundation::IAsyncOperation<bool> SaveAsAsync();
		void SetTitle(bool modified);
		void FocusEditor();
		Windows::Foundation::IAsyncOperation<Windows::UI::Xaml::Controls::ContentDialogResult> PromptSaveAsync();
	};
}

namespace winrt::CppDemoUwp::factory_implementation
{
	struct MainPage : MainPageT<MainPage, implementation::MainPage>
	{
	};
}
