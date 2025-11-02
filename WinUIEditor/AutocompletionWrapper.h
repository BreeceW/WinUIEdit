#pragma once

#include "Wrapper.h"

namespace WinUIEditor
{
	class AutocompletionWrapper : public Wrapper
	{
	public:
		AutocompletionWrapper(winrt::DUXC::Control const &control);

		void Show(bool visible) override;
		void Destroy() override;
		void SetPositionRelative(Scintilla::Internal::PRectangle rc, Wrapper const &wrapper) override;
		static std::shared_ptr<Wrapper> Create(winrt::DUX::XamlRoot const &xamlRoot, float logicalDpi, winrt::DUX::ElementTheme theme);

		void Append(winrt::hstring const &text);
		void Clear();
		int SelectedIndex();
		void SelectedIndex(int value);

	private:
		winrt::weak_ref<winrt::DUXC::Primitives::Popup> _popup{ nullptr };
	};
}
