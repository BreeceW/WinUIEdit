#include "pch.h"
#include "EditorDemoControl.h"
#if __has_include("EditorDemoControl.g.cpp")
#include "EditorDemoControl.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::CppIslandsUwpDemoApp::implementation
{
    EditorDemoControl::EditorDemoControl()
    {
        InitializeComponent();
    }
}
