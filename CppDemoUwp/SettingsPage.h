#pragma once

#include "SettingsPage.g.h"

namespace winrt::CppDemoUwp::implementation
{
	struct SettingsPage : SettingsPageT<SettingsPage>
	{
		SettingsPage();

		void ThemeSwitcher_Loaded(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
		void ThemeSwitcher_SelectionChanged(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs const &e);

	private:
		void OnActivated(Windows::UI::Core::CoreWindow const &sender, Windows::UI::Core::WindowActivatedEventArgs const &e);
		Windows::UI::Core::CoreWindow::Activated_revoker _activatedRevoker{};
		void Activated(bool active);
		void OnTitleBarChanged(Windows::ApplicationModel::Core::CoreApplicationViewTitleBar const &sender, Windows::Foundation::IInspectable const &e);
		Windows::ApplicationModel::Core::CoreApplicationViewTitleBar::LayoutMetricsChanged_revoker _titleBarLayoutMetricsChangedRevoker{};
		Windows::ApplicationModel::Core::CoreApplicationViewTitleBar::IsVisibleChanged_revoker _isVisibleChangedRevoker{};
		bool _loadingThemeSetting{ false };
	};
}

namespace winrt::CppDemoUwp::factory_implementation
{
	struct SettingsPage : SettingsPageT<SettingsPage, implementation::SettingsPage>
	{
	};
}
