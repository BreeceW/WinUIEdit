#pragma once

#include "MainPage.g.h"

namespace winrt::CppDemoUwp::implementation
{
	struct MainPage : MainPageT<MainPage>
	{
		MainPage();
		void Editor_Loaded(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		Windows::Foundation::IAsyncAction OpenMenuItem_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		Windows::Foundation::IAsyncAction SaveMenuItem_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		Windows::Foundation::IAsyncAction SaveAsMenuItem_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		Windows::Foundation::IAsyncAction NewWindowMenuItem_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		void NewMenuItem_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		void ZoomInMenuItem_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		void ZoomOutMenuItem_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		void ZoomRestoreMenuItem_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		void StatusBarMenuItem_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		void WordWrapMenuItem_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		void TimeDateMenuItem_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		void CommandMenuItem_Click(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const &e);

	private:
		void Editor_UpdateUI(MicaEditor::Editor const &sender, MicaEditor::UpdateUIEventArgs const &args);
		void Editor_ZoomChanged(MicaEditor::Editor const &sender, MicaEditor::ZoomChangedEventArgs const &args);
		void Editor_SavePointReached(MicaEditor::Editor const &sender, MicaEditor::SavePointReachedEventArgs const &args);
		void Editor_SavePointLeft(MicaEditor::Editor const &sender, MicaEditor::SavePointLeftEventArgs const &args);
		MicaEditor::Editor::UpdateUI_revoker _updateUIRevoker{};
		MicaEditor::Editor::ZoomChanged_revoker _zoomChangedRevoker{};
		MicaEditor::Editor::SavePointReached_revoker _savePointReachedRevoker{};
		MicaEditor::Editor::SavePointLeft_revoker _savePointLeftRevoker{};
		Windows::UI::Core::CoreWindow::Activated_revoker _activatedRevoker{};
		void OnActivated(Windows::UI::Core::CoreWindow const &sender, Windows::UI::Core::WindowActivatedEventArgs const &e);
		void Activated(bool active);
		bool _hasFcu{ Windows::Foundation::Metadata::ApiInformation::IsApiContractPresent(L"Windows.Foundation.UniversalApiContract", 5) }; // Todo: Make static
		Windows::Storage::StorageFile _activeFile{ nullptr };
		Windows::Foundation::IAsyncAction OpenAsync(Windows::Storage::StorageFile file);
		Windows::Foundation::IAsyncAction SaveAsync(Windows::Storage::StorageFile file);
		Windows::Foundation::IAsyncAction SaveAsAsync();
		void SetTitle(bool modified);
		void FocusEditor();
	};
}

namespace winrt::CppDemoUwp::factory_implementation
{
	struct MainPage : MainPageT<MainPage, implementation::MainPage>
	{
	};
}
