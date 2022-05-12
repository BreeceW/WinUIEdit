#include "pch.h"

#include "App.h"
#include "MainPage.h"

using namespace winrt;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace CppDemoUwp;
using namespace CppDemoUwp::implementation;

/// <summary>
/// Initializes the singleton application object.  This is the first line of authored code
/// executed, and as such is the logical equivalent of main() or WinMain().
/// </summary>
App::App()
{
	InitializeComponent();
	Suspending({ this, &App::OnSuspending });

#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
	UnhandledException([this](IInspectable const &, UnhandledExceptionEventArgs const &e)
		{
			if (IsDebuggerPresent())
			{
				auto errorMessage = e.Message();
				__debugbreak();
			}
		});
#endif

	_colorValuesChangedRevoker = _uiSettings.ColorValuesChanged(winrt::auto_revoke, { this, &App::OnColorValuesChanged });
}

/// <summary>
/// Invoked when the application is launched normally by the end user.  Other entry points
/// will be used such as when the application is launched to open a specific file.
/// </summary>
/// <param name="e">Details about the launch request and process.</param>
void App::OnLaunched(LaunchActivatedEventArgs const &e)
{
	Frame rootFrame{ nullptr };
	auto content = Window::Current().Content();
	if (content)
	{
		rootFrame = content.try_as<Frame>();
	}

	// Do not repeat app initialization when the Window already has content,
	// just ensure that the window is active
	if (rootFrame == nullptr)
	{
		// Create a Frame to act as the navigation context and associate it with
		// a SuspensionManager key
		rootFrame = Frame();

		rootFrame.NavigationFailed({ this, &App::OnNavigationFailed });

		if (e.PreviousExecutionState() == ApplicationExecutionState::Terminated)
		{
			// Restore the saved session state only when appropriate, scheduling the
			// final launch steps after the restore is complete
		}

		if (e.PrelaunchActivated() == false)
		{
			if (rootFrame.Content() == nullptr)
			{
				// When the navigation stack isn't restored navigate to the first page,
				// configuring the new page by passing required information as a navigation
				// parameter
				rootFrame.Navigate(xaml_typename<CppDemoUwp::MainPage>(), box_value(e.Arguments()));
			}
			// Place the frame in the current Window
			Window::Current().Content(rootFrame);
			// Ensure the current window is active
			Window::Current().Activate();
		}
	}
	else
	{
		if (e.PrelaunchActivated() == false)
		{
			if (rootFrame.Content() == nullptr)
			{
				// When the navigation stack isn't restored navigate to the first page,
				// configuring the new page by passing required information as a navigation
				// parameter
				rootFrame.Navigate(xaml_typename<CppDemoUwp::MainPage>(), box_value(e.Arguments()));
			}
			// Ensure the current window is active
			Window::Current().Activate();
		}
	}

	Windows::ApplicationModel::Core::CoreApplication::GetCurrentView().TitleBar().ExtendViewIntoTitleBar(true);
	UpdateTitleBar();
}

/// <summary>
/// Invoked when application execution is being suspended.  Application state is saved
/// without knowing whether the application will be terminated or resumed with the contents
/// of memory still intact.
/// </summary>
/// <param name="sender">The source of the suspend request.</param>
/// <param name="e">Details about the suspend request.</param>
void App::OnSuspending([[maybe_unused]] IInspectable const &sender, [[maybe_unused]] SuspendingEventArgs const &e)
{
	// Save application state and stop any background activity
}

/// <summary>
/// Invoked when Navigation to a certain page fails
/// </summary>
/// <param name="sender">The Frame which failed navigation</param>
/// <param name="e">Details about the navigation failure</param>
void App::OnNavigationFailed(IInspectable const &, NavigationFailedEventArgs const &e)
{
	throw hresult_error(E_FAIL, hstring(L"Failed to load Page ") + e.SourcePageType().Name);
}

void App::OnColorValuesChanged(Windows::UI::ViewManagement::UISettings const &uiSettings, IInspectable const &args)
{
	// Todo: Not sure if there is a way to detect if the RequestedTheme changes in Creators Update. Unimportant
	Windows::ApplicationModel::Core::CoreApplication::MainView().Dispatcher().RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, [&]()
		{
			UpdateTitleBar();
		});
}

void App::UpdateTitleBar()
{
	auto titleBar{ Windows::UI::ViewManagement::ApplicationView::GetForCurrentView().TitleBar() };
	titleBar.ButtonBackgroundColor(Color{ 0, 255, 255, 255, });
	titleBar.ButtonInactiveBackgroundColor(Color{ 0, 255, 255, 255, });
	if (RequestedTheme() == ApplicationTheme::Dark)
	{
		titleBar.ButtonForegroundColor(Color{ 255, 255, 255, 255, });
		titleBar.ButtonHoverForegroundColor(Color{ 255, 255, 255, 255, });
		titleBar.ButtonHoverBackgroundColor(Color{ 24, 255, 255, 255, });
		titleBar.ButtonPressedForegroundColor(Color{ 255, 255, 255, 255, });
		titleBar.ButtonPressedBackgroundColor(Color{ 47, 255, 255, 255, });
		titleBar.ButtonInactiveForegroundColor(Color{ 255, 118, 118, 118, });
	}
	else
	{
		titleBar.ButtonForegroundColor({ Color { 255, 0, 0, 0, } });
		titleBar.ButtonHoverForegroundColor({ Color { 255, 0, 0, 0, } });
		titleBar.ButtonHoverBackgroundColor({ Color { 25, 0, 0, 0, } });
		titleBar.ButtonPressedForegroundColor({ Color { 255, 0, 0, 0, } });
		titleBar.ButtonPressedBackgroundColor({ Color { 51, 0, 0, 0, } });
		titleBar.ButtonInactiveForegroundColor({ Color { 255, 153, 153, 153, } });
	}
}
