#include "pch.h"

#include "App.h"
#include "MainPage.h"

using namespace winrt;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;
using namespace Windows::UI;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::ViewManagement;
using namespace CppDemoUwp;
using namespace CppDemoUwp::implementation;

App::App()
{
	InitializeComponent();

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

void App::OnLaunched(LaunchActivatedEventArgs const &e)
{
	CoreApplication::GetCurrentView().TitleBar().ExtendViewIntoTitleBar(true);
	ApplicationView::GetForCurrentView().SetPreferredMinSize(Size{ 308, 188 });
	UpdateTitleBar(RequestedTheme());

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

		if (e.PrelaunchActivated() == false)
		{
			if (rootFrame.Content() == nullptr)
			{
				// When the navigation stack isn't restored navigate to the first page,
				// configuring the new page by passing required information as a navigation
				// parameter
				rootFrame.Navigate(xaml_typename<CppDemoUwp::MainPage>());
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
				rootFrame.Navigate(xaml_typename<CppDemoUwp::MainPage>());
			}
			// Ensure the current window is active
			Window::Current().Activate();
		}
	}
}

void App::OnNavigationFailed(IInspectable const &, NavigationFailedEventArgs const &e)
{
	throw hresult_error(E_FAIL, hstring(L"Failed to load Page ") + e.SourcePageType().Name);
}

IAsyncAction App::NewWindowAsync()
{
	const auto newView{ CoreApplication::CreateNewView() };
	int newViewId = 0;
	co_await newView.Dispatcher().RunAsync(CoreDispatcherPriority::Normal, [&]()
		{
			CoreApplication::GetCurrentView().TitleBar().ExtendViewIntoTitleBar(true);
			const auto view{ ApplicationView::GetForCurrentView() };
			view.SetPreferredMinSize(Size{ 308, 188 });
			UpdateTitleBar(RequestedTheme());

			const Frame frame{};
			frame.NavigationFailed({ this, &App::OnNavigationFailed });
			frame.Navigate(xaml_typename<CppDemoUwp::MainPage>());
			Window::Current().Content(frame);
			Window::Current().Activate();

			newViewId = view.Id();
		});
	co_await ApplicationViewSwitcher::TryShowAsStandaloneAsync(newViewId);
}

void App::OnColorValuesChanged(UISettings const &uiSettings, IInspectable const &args)
{
	// Todo: Not sure if there is a way to detect if the RequestedTheme changes in Creators Update. Unimportant
	for (const auto &view : CoreApplication::Views())
	{
		view.Dispatcher().RunAsync(CoreDispatcherPriority::Normal, [&]()
			{
				UpdateTitleBar(RequestedTheme());
			});
	}
}

void App::UpdateTitleBar(ApplicationTheme theme)
{
	auto titleBar{ ApplicationView::GetForCurrentView().TitleBar() };
	titleBar.ButtonBackgroundColor(Color{ 0, 255, 255, 255, });
	titleBar.ButtonInactiveBackgroundColor(Color{ 0, 255, 255, 255, });
	if (theme == ApplicationTheme::Dark)
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
