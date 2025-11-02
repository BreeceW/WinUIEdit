#include "pch.h"
#include "EditorBaseControl.h"
#include "AutocompletionControl.h"
#include "AutocompletionWrapper.h"

namespace WinUIEditor
{
	AutocompletionWrapper::AutocompletionWrapper(winrt::DUXC::Control const &control) : Wrapper(control)
	{
	}

	void AutocompletionWrapper::Show(bool visible)
	{
		if (const auto popup{ _popup.get() })
		{
			popup.Visibility(visible ? winrt::DUX::Visibility::Visible : winrt::DUX::Visibility::Collapsed);
		}
	}

	void AutocompletionWrapper::Destroy()
	{
		if (const auto popup{ _popup.get() })
		{
			popup.IsOpen(false);
		}
	}

	void AutocompletionWrapper::SetPositionRelative(Scintilla::Internal::PRectangle rc, Wrapper const &wrapper)
	{
		// Todo: Make sure to position such that the tooltip does not hang off the monitor
		// Transform to root rather than other window because that is how XAML Popup offset will be set
		const auto dpiScale{ LogicalDpi() / 96.f };
		float x = rc.left / dpiScale;
		float y = rc.top / dpiScale;
		float width = rc.Width() / dpiScale;
		float height = rc.Height() / dpiScale;
		if (!wrapper.TransformToRoot(x, y))
		{
			return;
		}

		if (const auto control{ _control.get().try_as<winrt::WinUIEditor::implementation::AutocompletionControl>() })
		{
			control->Width(width);
			control->Height(height);
		}
		if (const auto popup{ _popup.get() })
		{
			popup.HorizontalOffset(x);
			popup.VerticalOffset(y);
		}
	}

	std::shared_ptr<Wrapper> AutocompletionWrapper::Create(winrt::DUX::XamlRoot const &xamlRoot, float logicalDpi, winrt::DUX::ElementTheme theme)
	{
#ifndef WINUI3
		static bool hasUac8{ winrt::Windows::Foundation::Metadata::ApiInformation::IsApiContractPresent(L"Windows.Foundation.UniversalApiContract", 8) }; // Todo: Make static
#endif

		const auto autocompletion{ winrt::make_self<winrt::WinUIEditor::implementation::AutocompletionControl>() };
		autocompletion->MaxWidth(std::numeric_limits<double>::infinity());
		autocompletion->Padding(winrt::DUX::Thickness{ 0, 0, 0, 0 });
		autocompletion->Wrapper()->LogicalDpi(logicalDpi);
		autocompletion->IsTabStop(false);
		autocompletion->HorizontalAlignment(winrt::DUX::HorizontalAlignment::Center);
		autocompletion->VerticalAlignment(winrt::DUX::VerticalAlignment::Center);
		winrt::DUXC::Primitives::Popup popup{};
		autocompletion->Wrapper()->_popup = popup;
#ifndef WINUI3
		if (hasUac8)
		{
#endif
			// Per WinUI 3 source code, the shadow is added to the tooltip ContentPresenter for animation reasons
			// We add it to the popup level instead because it is easier and we are not animating tooltips
			// Setting translation on the tooltip level causes rendering to break when the popup is in some positions
			popup.Shadow(winrt::DUX::Media::ThemeShadow{});
			popup.Translation(winrt::Windows::Foundation::Numerics::float3{ 0, 0, 16 });
			// Needed for AppWindow
			popup.ShouldConstrainToRootBounds(false);
			popup.XamlRoot(xamlRoot);
#ifndef WINUI3
		}
#endif
		// Todo: Live theme changes
		popup.RequestedTheme(theme);
		popup.IsOpen(true);
		popup.Visibility(winrt::DUX::Visibility::Collapsed);
		popup.Child(*autocompletion);
		return autocompletion->Wrapper();
	}

	void AutocompletionWrapper::Append(winrt::hstring const &text)
	{
		if (const auto control{ _control.get().try_as<winrt::WinUIEditor::implementation::AutocompletionControl>() })
		{
			const auto list{ control->ItemsSource().as<winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring>>() };
			list.Append(text);
		}
	}

	void AutocompletionWrapper::Clear()
	{
		if (const auto control{ _control.get().try_as<winrt::WinUIEditor::implementation::AutocompletionControl>() })
		{
			const auto list{ control->ItemsSource().as<winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring>>() };
			list.Clear();
		}
	}

	int AutocompletionWrapper::SelectedIndex()
	{
		const auto control{ _control.get().try_as<winrt::WinUIEditor::implementation::AutocompletionControl>() };
		return control ? control->SelectedIndex() : -1;
	}

	void AutocompletionWrapper::SelectedIndex(int value)
	{
		if (const auto control{ _control.get().try_as<winrt::WinUIEditor::implementation::AutocompletionControl>() })
		{
			control->SelectedIndex(value);
		}
	}
}
