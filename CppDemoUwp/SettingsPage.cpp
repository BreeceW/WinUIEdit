#include "pch.h"
#include "SettingsPage.h"
#include "SettingsPage.g.cpp"
#include "App.h"

using namespace winrt;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Controls;

namespace winrt::CppDemoUwp::implementation
{
	SettingsPage::SettingsPage()
	{
		InitializeComponent();

		_activatedRevoker = CoreWindow::GetForCurrentThread().Activated(auto_revoke, { this, &SettingsPage::OnActivated });

		const auto titleBar{ CoreApplication::GetCurrentView().TitleBar() };
		_titleBarLayoutMetricsChangedRevoker = titleBar.LayoutMetricsChanged(auto_revoke, { this, &SettingsPage::OnTitleBarChanged });
		_isVisibleChangedRevoker = titleBar.IsVisibleChanged(auto_revoke, { this, &SettingsPage::OnTitleBarChanged });
	}

	void SettingsPage::ThemeSwitcher_Loaded(IInspectable const &sender, RoutedEventArgs const &e)
	{
		const auto themeSwitcher{ sender.as<RadioButtons>() };
		_loadingThemeSetting = true;
		switch (Application::Current().as<App>()->Theme())
		{
		case ElementTheme::Default:
			themeSwitcher.SelectedIndex(2);
			break;
		case ElementTheme::Light:
			themeSwitcher.SelectedIndex(0);
			break;
		case ElementTheme::Dark:
			themeSwitcher.SelectedIndex(1);
			break;
		}
	}

	void SettingsPage::ThemeSwitcher_SelectionChanged(IInspectable const &sender, SelectionChangedEventArgs const &e)
	{
		if (_loadingThemeSetting)
		{
			_loadingThemeSetting = false;
			return;
		}

		const auto themeSwitcher{ sender.as<RadioButtons>() };
		switch (themeSwitcher.SelectedIndex())
		{
		case 2:
			Application::Current().as<App>()->Theme(ElementTheme::Default);
			break;
		case 0:
			Application::Current().as<App>()->Theme(ElementTheme::Light);
			break;
		case 1:
			Application::Current().as<App>()->Theme(ElementTheme::Dark);
			break;
		}
	}

	void SettingsPage::OnActivated(CoreWindow const &sender, WindowActivatedEventArgs const &e)
	{
		Activated(e.WindowActivationState() != CoreWindowActivationState::Deactivated);
	}

	void SettingsPage::Activated(bool active)
	{
		TitleText().Opacity(active ? 1 : 0.4);
	}

	void SettingsPage::OnTitleBarChanged(CoreApplicationViewTitleBar const &sender, IInspectable const &e)
	{
		TitleBar().Margin(Thickness{ sender.IsVisible() ? (std::max)(sender.SystemOverlayLeftInset() - 12, 0.0) : 0, 0, 0, 0 });
	}
}
