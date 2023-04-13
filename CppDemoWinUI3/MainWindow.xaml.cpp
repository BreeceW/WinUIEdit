#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

using namespace winrt;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Media;

namespace winrt::CppDemoWinUI3::implementation
{
	MainWindow::MainWindow()
	{
		InitializeComponent();

		Title(L"Demo WinUI 3 (C++)");

		SystemBackdrop(MicaBackdrop{});
	}
}
