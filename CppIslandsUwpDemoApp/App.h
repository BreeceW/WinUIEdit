#pragma once

#include "App.g.h"
#include "App.base.h"

namespace winrt::CppIslandsUwpDemoApp::implementation
{
    class App : public AppT2<App>
    {
    public:
        App();
        ~App();
    };
}
namespace winrt::CppIslandsUwpDemoApp::factory_implementation
{
    class App : public AppT<App, implementation::App>
    {
    };
}
