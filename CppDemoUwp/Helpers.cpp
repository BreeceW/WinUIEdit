#include "pch.h"
#include "Helpers.h"

using namespace winrt;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;

namespace winrt::CppDemoUwp::implementation
{
	IAsyncAction CloseCurrentWindowAsync()
	{
		if (co_await ApplicationView::GetForCurrentView().TryConsolidateAsync())
		{
			ApplicationView::GetForCurrentView().Consolidated([](ApplicationView const &sender, ApplicationViewConsolidatedEventArgs const &args)
				{
					if (!CoreApplication::GetCurrentView().IsMain())
					{
						Window::Current().Close();
					}
					else
					{
						Window::Current().Content(nullptr);
						// Todo: Does view leak?
					}
				});
		}
		else
		{
			if (CoreApplication::Views().Size() > 1)
			{
				Window::Current().Close();
			}
			else
			{
				Application::Current().Exit();
			}
		}
	}
}
