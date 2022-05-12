#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

using namespace winrt;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Xaml;

namespace winrt::CppDemoWinUI3::implementation
{
	MainWindow::MainWindow()
	{
		InitializeComponent();

		Title(L"Demo WinUI 3 (C++)");

		TrySetMicaBackdrop();
	}

	bool MainWindow::TrySetMicaBackdrop()
	{
		if (Microsoft::UI::Composition::SystemBackdrops::MicaController::IsSupported())
		{
			if (Windows::System::DispatcherQueue::GetForCurrentThread() == nullptr && _dispatcherQueueController == nullptr)
			{
				DispatcherQueueOptions options;
				options.dwSize = sizeof DispatcherQueueOptions;
				options.threadType = DQTYPE_THREAD_CURRENT;
				options.apartmentType = DQTAT_COM_STA;

				CreateDispatcherQueueController(options, reinterpret_cast<PDISPATCHERQUEUECONTROLLER *>(put_abi(_dispatcherQueueController)));
			}

			_configurationSource = Microsoft::UI::Composition::SystemBackdrops::SystemBackdropConfiguration{};
			_activatedRevoker = Activated(auto_revoke, { this, &MainWindow::OnActivated });
			Closed({ this, &MainWindow::OnClosed });

			_configurationSource.IsInputActive(true);
			switch (Content().as<FrameworkElement>().ActualTheme())
			{
			case ElementTheme::Dark: _configurationSource.Theme(Microsoft::UI::Composition::SystemBackdrops::SystemBackdropTheme::Dark); break;
			case ElementTheme::Light: _configurationSource.Theme(Microsoft::UI::Composition::SystemBackdrops::SystemBackdropTheme::Light); break;
			case ElementTheme::Default: _configurationSource.Theme(Microsoft::UI::Composition::SystemBackdrops::SystemBackdropTheme::Default); break;
			}

			_micaController = Microsoft::UI::Composition::SystemBackdrops::MicaController{};

			_micaController.AddSystemBackdropTarget(this->try_as<Microsoft::UI::Composition::ICompositionSupportsSystemBackdrop>());
			_micaController.SetSystemBackdropConfiguration(_configurationSource);
			return true;
		}

		return false;
	}

	void MainWindow::OnActivated(Windows::Foundation::IInspectable const &sender, WindowActivatedEventArgs const &args)
	{
		_configurationSource.IsInputActive(args.WindowActivationState() != WindowActivationState::Deactivated);
	}

	void MainWindow::OnClosed(Windows::Foundation::IInspectable const &sender, WindowEventArgs const &args)
	{
		if (_micaController != nullptr)
		{
			_micaController.Close();
			_micaController = nullptr;
		}
		_activatedRevoker.revoke();
		_configurationSource = nullptr;
	}
}
