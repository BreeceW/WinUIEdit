#include "pch.h"
#include "MicaEditorControl.h"
#if __has_include("MicaEditorControl.g.cpp")
#include "MicaEditorControl.g.cpp"
#endif

#include <Scintilla.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;

namespace winrt::MicaEditor::implementation
{
	MicaEditorControl::MicaEditorControl()
	{
		DefaultStyleKey(winrt::box_value(L"MicaEditor.MicaEditorControl"));

		Scintilla_RegisterClasses(GetModuleHandleW(nullptr));
	}

	int32_t MicaEditorControl::MyProperty()
	{
		return _myProperty;
	}

	void MicaEditorControl::MyProperty(int32_t value)
	{
		_myProperty = value;
		UpdateBox();
	}

	void MicaEditorControl::OnApplyTemplate()
	{
		UpdateBox();
	}

	void MicaEditorControl::UpdateBox()
	{
		if (auto box = GetTemplateChild(L"DemoBox").try_as<TextBox>())
		{
			box.FontSize(static_cast<int32_t>(_myProperty));
		}
	}
}
