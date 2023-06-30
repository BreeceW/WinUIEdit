#include "pch.h"
#include "EditorDemoControl.h"
#if __has_include("EditorDemoControl.g.cpp")
#include "EditorDemoControl.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

namespace winrt::CppIslandsUwpDemoApp::implementation
{
	EditorDemoControl::EditorDemoControl()
	{
		InitializeComponent();
	}

	void EditorDemoControl::CodeEditorControl_Loaded(IInspectable const &sender, RoutedEventArgs const &e)
	{
		sender.as<Control>().Focus(FocusState::Programmatic);
	}
}
