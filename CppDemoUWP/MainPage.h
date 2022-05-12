#pragma once

#include "MainPage.g.h"

namespace winrt::CppDemoUwp::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage();

    private:
        Windows::UI::Core::CoreWindow::Activated_revoker _activatedRevoker{};
        void OnActivated(Windows::UI::Core::CoreWindow const &sender, Windows::UI::Core::WindowActivatedEventArgs const &e);
        void Activated(bool active);
        bool _hasFcu{ Windows::Foundation::Metadata::ApiInformation::IsApiContractPresent(L"Windows.Foundation.UniversalApiContract", 5) }; // Todo: Make static
    };
}

namespace winrt::CppDemoUwp::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
