#include "pch.h"
#include "MicaEditorControl.h"
#if __has_include("MicaEditorControl.g.cpp")
#include "MicaEditorControl.g.cpp"
#endif

using namespace winrt;
using namespace DUX;
using namespace DUXC;

namespace winrt::MicaEditor::implementation
{
	MicaEditorControl::MicaEditorControl()
	{
		DefaultStyleKey(winrt::box_value(L"MicaEditor.MicaEditorControl"));

		//Scintilla_RegisterClasses(GetModuleHandleW(nullptr));
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
		_scintilla.WndProc(Scintilla::Message::InsertText, 0, reinterpret_cast<Scintilla::uptr_t>("Test set text"));
		char buf[400];
		_scintilla.WndProc(Scintilla::Message::GetText, 400, reinterpret_cast<Scintilla::uptr_t>(buf));
		if (auto box = GetTemplateChild(L"DemoBox").try_as<TextBox>())
		{
			box.Text(to_hstring(buf));
		}

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
