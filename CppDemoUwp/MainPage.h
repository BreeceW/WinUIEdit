#pragma once

#include "MainPage.g.h"

namespace winrt::CppDemoUwp::implementation
{
	struct MainPage : MainPageT<MainPage>
	{
		MainPage();
		Windows::Foundation::IAsyncAction OpenMenuItem_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		Windows::Foundation::IAsyncAction SaveMenuItem_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		Windows::Foundation::IAsyncAction SaveAsMenuItem_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		Windows::Foundation::IAsyncAction NewWindowMenuItem_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		void ZoomInMenuItem_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		void ZoomOutMenuItem_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		void ZoomRestoreMenuItem_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const &e);
		void Editor_UpdateUI(MicaEditor::Editor const &sender, MicaEditor::UpdateUIEventArgs const &args);
		void Editor_ZoomChanged(MicaEditor::Editor const &sender, MicaEditor::ZoomChangedEventArgs const &args);

	private:
		MicaEditor::Editor::UpdateUI_revoker _updateUIRevoker{};
		MicaEditor::Editor::ZoomChanged_revoker _zoomChangedRevoker{};
		Windows::UI::Core::CoreWindow::Activated_revoker _activatedRevoker{};
		void OnActivated(Windows::UI::Core::CoreWindow const &sender, Windows::UI::Core::WindowActivatedEventArgs const &e);
		void Activated(bool active);
		bool _hasFcu{ Windows::Foundation::Metadata::ApiInformation::IsApiContractPresent(L"Windows.Foundation.UniversalApiContract", 5) }; // Todo: Make static
		Windows::Storage::StorageFile _activeFile{ nullptr };
		Windows::Foundation::IAsyncAction OpenAsync(Windows::Storage::StorageFile file);
		Windows::Foundation::IAsyncAction SaveAsync(Windows::Storage::StorageFile file);
		Windows::Foundation::IAsyncAction SaveAsAsync();
		void SetOpenFile(Windows::Storage::StorageFile const &file);
	};
}

namespace winrt::CppDemoUwp::factory_implementation
{
	struct MainPage : MainPageT<MainPage, implementation::MainPage>
	{
	};
}
