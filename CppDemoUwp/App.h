#pragma once
#include "App.xaml.g.h"

namespace winrt::CppDemoUwp::implementation
{
    struct App : AppT<App>
    {
        App();

        void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs const&);
        void OnNavigationFailed(IInspectable const&, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs const&);
        Windows::Foundation::IAsyncAction NewWindowAsync();

    private:
        Windows::UI::ViewManagement::UISettings::ColorValuesChanged_revoker _colorValuesChangedRevoker{};
        void OnColorValuesChanged(Windows::UI::ViewManagement::UISettings const &uiSettings, Windows::Foundation::IInspectable const &args);
        Windows::UI::ViewManagement::UISettings _uiSettings{};
        static void UpdateTitleBar(Windows::UI::Xaml::ApplicationTheme theme);
    };
}
