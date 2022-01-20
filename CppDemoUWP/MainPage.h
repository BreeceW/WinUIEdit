#pragma once

#include "MainPage.g.h"

namespace winrt::CppDemoUWP::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage();
    };
}

namespace winrt::CppDemoUWP::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
