#pragma once

#include "MainWindow.g.h"

namespace winrt::CppDemoWinUI3::implementation
{
	struct MainWindow : MainWindowT<MainWindow>
	{
		MainWindow();

	private:
		bool TrySetMicaBackdrop();
		void OnActivated(Windows::Foundation::IInspectable const &sender, Microsoft::UI::Xaml::WindowActivatedEventArgs const &args);
		void OnClosed(Windows::Foundation::IInspectable const &sender, Microsoft::UI::Xaml::WindowEventArgs const &args);
		Windows::System::DispatcherQueueController _dispatcherQueueController{ nullptr };
		Microsoft::UI::Composition::SystemBackdrops::MicaController _micaController{ nullptr };
		Microsoft::UI::Composition::SystemBackdrops::SystemBackdropConfiguration _configurationSource{ nullptr };
		Microsoft::UI::Xaml::Window::Activated_revoker _activatedRevoker{};
	};
}

namespace winrt::CppDemoWinUI3::factory_implementation
{
	struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
	{
	};
}
