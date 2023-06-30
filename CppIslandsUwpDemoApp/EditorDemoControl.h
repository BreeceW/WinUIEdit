#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "EditorDemoControl.g.h"

namespace winrt::CppIslandsUwpDemoApp::implementation
{
    struct EditorDemoControl : EditorDemoControlT<EditorDemoControl>
    {
        EditorDemoControl();

        void CodeEditorControl_Loaded(Windows::Foundation::IInspectable const &sender, Windows::UI::Xaml::RoutedEventArgs const &e);
    };
}

namespace winrt::CppIslandsUwpDemoApp::factory_implementation
{
    struct EditorDemoControl : EditorDemoControlT<EditorDemoControl, implementation::EditorDemoControl>
    {
    };
}
