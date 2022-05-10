#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::CppDemoWinUI3::implementation
{
	MainWindow::MainWindow()
	{
		InitializeComponent();

		Title(L"Demo WinUI 3 (C++)");

		Editor().Window(*this);

		/*SizeChanged([&](auto &&, auto const &args)
			{
				Title(winrt::to_hstring(args.Size().Width) + L"x" + winrt::to_hstring(args.Size().Height));
			});*/
	}
}
