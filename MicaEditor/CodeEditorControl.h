#pragma once

#include "CodeEditorControl.g.h"

#include "MicaEditorControl.h"

namespace winrt::MicaEditor::implementation
{
	struct CodeEditorControl : CodeEditorControlT<CodeEditorControl>
	{
		CodeEditorControl();
		uint64_t Scintilla(ScintillaMessage const &message, uint64_t wParam, uint64_t lParam);
		MicaEditor::Editor Editor();
		void OnApplyTemplate();
		void OnGettingFocus(Windows::Foundation::IInspectable const &sender, DUX::Input::GettingFocusEventArgs const &e);

	private:
		com_ptr<MicaEditorControl> _editor{ nullptr };
		DUX::Controls::Control::GettingFocus_revoker _gettingFocusRevoker{};
		DUD::DispatcherQueue _dispatcherQueue{ nullptr };
#ifndef WINUI3
		bool _hasFcu{ Windows::Foundation::Metadata::ApiInformation::IsApiContractPresent(L"Windows.Foundation.UniversalApiContract", 5) }; // Todo: Make static
		bool _has1803{ Windows::Foundation::Metadata::ApiInformation::IsApiContractPresent(L"Windows.Foundation.UniversalApiContract", 6) }; // Todo: Make static
#endif
	};
}

namespace winrt::MicaEditor::factory_implementation
{
	struct CodeEditorControl : CodeEditorControlT<CodeEditorControl, implementation::CodeEditorControl>
	{
	};
}
