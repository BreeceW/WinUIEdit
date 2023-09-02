#pragma once

#include "MainWindow.g.h"

namespace winrt::CppDemoWinUI3::implementation
{
	struct MainWindow : MainWindowT<MainWindow>
	{
		MainWindow();

		void CodeEditorControl_Loaded(Windows::Foundation::IInspectable const &sender, Microsoft::UI::Xaml::RoutedEventArgs const &e);
	};
}

namespace winrt::CppDemoWinUI3::factory_implementation
{
	struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
	{
	};
}
