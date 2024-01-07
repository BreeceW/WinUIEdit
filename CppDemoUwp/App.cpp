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
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::ViewManagement;
using namespace Windows::Storage;
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

	_colorValuesChangedRevoker = _uiSettings.ColorValuesChanged(auto_revoke, { this, &App::OnColorValuesChanged });
}

void App::OnLaunched(LaunchActivatedEventArgs const &e)
{
	if (!e.PrelaunchActivated())
	{
		NewWindow(nullptr);
	}
}

void App::OnFileActivated(FileActivatedEventArgs const &e)
{
	for (const auto &file : e.Files())
	{
		NewWindow(file);
	}
}

void App::OnNavigationFailed(IInspectable const &, NavigationFailedEventArgs const &e)
{
	throw hresult_error(E_FAIL, hstring(L"Failed to load Page ") + e.SourcePageType().Name);
}

int32_t App::NewWindow(IStorageItem const &file)
{
	const auto window{ Window::Current() };

	Frame frame{ window.Content().try_as<Frame>() };
	if (!frame)
	{
		frame = Frame{};
		frame.NavigationFailed({ this, &App::OnNavigationFailed });
	}
	const auto theme{ Theme() };
	frame.RequestedTheme(theme);
	if (!frame.Content())
	{
		frame.Navigate(xaml_typename<CppDemoUwp::MainPage>(), file);
	}
	window.Content(frame);

	CoreApplication::GetCurrentView().TitleBar().ExtendViewIntoTitleBar(true);
	const auto &view{ ApplicationView::GetForCurrentView() };
	view.SetPreferredMinSize(Size{ 308, 188 });
	UpdateTitleBar(CurrentTheme(theme));

	window.Activate();

	SystemNavigationManager::GetForCurrentView().BackRequested({ this, &App::OnSystemBackRequested });

	return view.Id();
}

void App::OnSystemBackRequested(IInspectable const &sender, BackRequestedEventArgs const &e)
{
	if (GoBack())
	{
		e.Handled(true);
	}
}

IAsyncAction App::NewWindowAsync()
{
	const auto newView{ CoreApplication::CreateNewView() };
	int newViewId = 0;
	co_await newView.Dispatcher().RunAsync(CoreDispatcherPriority::Normal, [&]()
		{
			newViewId = NewWindow(nullptr);
		});
	co_await ApplicationViewSwitcher::TryShowAsStandaloneAsync(newViewId);
}

void App::GoToSettingsPage()
{
	const auto frame{ Window::Current().Content().as<Frame>() };

	frame.Navigate(xaml_typename<CppDemoUwp::SettingsPage>());

	UpdateTitleBarBackButton(frame);
}

bool App::GoBack()
{
	auto ret{ false };

	const auto frame{ Window::Current().Content().as<Frame>() };

	if (frame.CanGoBack())
	{
		frame.GoBack();
		ret = true;
	}

	UpdateTitleBarBackButton(frame);

	return ret;
}

ElementTheme App::Theme()
{
	return unbox_value_or<ElementTheme>(ApplicationData::Current().LocalSettings().Values().TryLookup(L"Theme"), ElementTheme::Default);
}

void App::Theme(ElementTheme theme)
{
	ApplicationData::Current().LocalSettings().Values().Insert(L"Theme", box_value(static_cast<int32_t>(theme)));
	ReloadTheme(theme);
}

void App::UpdateTitleBarBackButton(Frame const &frame)
{
	SystemNavigationManager::GetForCurrentView().AppViewBackButtonVisibility(frame.CanGoBack()
		? AppViewBackButtonVisibility::Visible
		: AppViewBackButtonVisibility::Disabled);
}

void App::ReloadTheme(ElementTheme theme)
{
	const auto currentTheme{ CurrentTheme(theme) };
	for (const auto &view : CoreApplication::Views())
	{
		view.Dispatcher().RunAsync(CoreDispatcherPriority::Normal, [&, theme, currentTheme]()
			{
				const auto window{ Window::Current() };
				window.Content().as<FrameworkElement>().RequestedTheme(theme);
				for (const auto &popup : VisualTreeHelper::GetOpenPopups(window))
				{
					if (const auto dialog{ popup.Child().try_as<ContentDialog>() })
					{
						dialog.RequestedTheme(theme);
					}
				}
				UpdateTitleBar(currentTheme);
			});
	}
}

bool IsColorLight(Color const &color)
{
	return (5 * color.G) + (2 * color.R) + color.B > 8 * 128;
}

ApplicationTheme App::CurrentTheme(ElementTheme theme)
{
	switch (theme)
	{
	case ElementTheme::Default:
		return IsColorLight(_uiSettings.GetColorValue(UIColorType::Foreground)) ? ApplicationTheme::Dark : ApplicationTheme::Light;
	case ElementTheme::Dark:
		return ApplicationTheme::Dark;
	default:
		return ApplicationTheme::Light;
	}
}

void App::OnColorValuesChanged(UISettings const &uiSettings, IInspectable const &args)
{
	const auto theme{ Theme() };
	if (theme == ElementTheme::Default)
	{
		ReloadTheme(theme);
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
