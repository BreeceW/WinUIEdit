#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"

using namespace winrt;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::Foundation;

namespace winrt::CppDemoUwp::implementation
{
	MainPage::MainPage()
	{
		InitializeComponent();

		auto coreWindow{ CoreWindow::GetForCurrentThread() };
		_activatedRevoker = coreWindow.Activated(winrt::auto_revoke, { this, &MainPage::OnActivated });
		if (_hasFcu)
		{
			Activated(coreWindow.ActivationMode() != CoreWindowActivationMode::Deactivated);
		}
	}

	void MainPage::OnActivated(CoreWindow const &sender, WindowActivatedEventArgs const &e)
	{
		Activated(e.WindowActivationState() != CoreWindowActivationState::Deactivated);
	}
	void MainPage::Activated(bool active)
	{
		TitleText().Opacity(active ? 1 : 0.4);
	}
}
