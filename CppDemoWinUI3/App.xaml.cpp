#include "pch.h"

#include "App.xaml.h"
#include "MainWindow.xaml.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Content;
using namespace Microsoft::UI::Dispatching;
using namespace Microsoft::UI::Windowing;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Hosting;
using namespace Microsoft::UI::Xaml::Media;
using namespace Microsoft::UI::Xaml::Navigation;
using namespace WinUIEditor;
using namespace CppDemoWinUI3;
using namespace CppDemoWinUI3::implementation;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

/// <summary>
/// Initializes the singleton application object.  This is the first line of authored code
/// executed, and as such is the logical equivalent of main() or WinMain().
/// </summary>
App::App()
{
    InitializeComponent();

#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
    UnhandledException([this](IInspectable const&, UnhandledExceptionEventArgs const& e)
    {
        if (IsDebuggerPresent())
        {
            auto errorMessage = e.Message();
            __debugbreak();
        }
    });
#endif
}

/// <summary>
/// Invoked when the application is launched normally by the end user.  Other entry points
/// will be used such as when the application is launched to open a specific file.
/// </summary>
/// <param name="e">Details about the launch request and process.</param>
void App::OnLaunched(LaunchActivatedEventArgs const&)
{
    constexpr auto useIslands = false;

    if constexpr (!useIslands)
    {
        window = make<MainWindow>();
        window.Activate();
    }
    else
    {
        // This path exists to test the scenario where islands are used without a window
        // Does not work currently. Seems to be issue with WinUI 3. Workaround not yet discovered
        //window = Window{}; // Works with MUX.Window loaded
        const auto appWindow{ AppWindow::Create() };
        appWindow.Title(L"Demo WinUI 3 Islands (C++)");
        const DesktopWindowXamlSource dwxs{};
        dwxs.Initialize(appWindow.Id());
        dwxs.SiteBridge().ResizePolicy(ContentSizePolicy::ResizeContentToParentWindow);
        dwxs.SiteBridge().Show();
        Border b{};
        b.Child(CodeEditorControl{});
        dwxs.Content(b);
        dwxs.SystemBackdrop(MicaBackdrop{});
        appWindow.Show();
    }
}
