#include "pch.h"
#include "MainWrapper.h"

namespace WinUIEditor
{
	MainWrapper::MainWrapper(winrt::DUXC::Control const &control) : Wrapper(control)
	{
	}

	void MainWrapper::SetMouseCapture(bool on)
	{
		if (!_mouseCaptureElement)
		{
			return;
		}

		if (on)
		{
			if (_lastPointer) // Todo: Check if works
			{
				_captured = _mouseCaptureElement.CapturePointer(_lastPointer);
			}
		}
		else
		{
			_mouseCaptureElement.ReleasePointerCaptures(); // Todo: Or just one?
			_captured = false;
		}
	}

	bool MainWrapper::HaveMouseCapture()
	{
		return _captured;
	}

	void MainWrapper::SetMouseCaptureElement(winrt::DUX::UIElement const &element)
	{
		_mouseCaptureElement = element;
		// Todo: Do we need to revoke?
		// Todo: Do we need a strong/weak reference?
		_mouseCaptureElement.PointerPressed([&](winrt::Windows::Foundation::IInspectable const &sender, winrt::DUX::Input::PointerRoutedEventArgs const &args)
			{
				_lastPointer = args.Pointer();
			});
		_mouseCaptureElement.PointerCaptureLost([&](winrt::Windows::Foundation::IInspectable const &sender, winrt::DUX::Input::PointerRoutedEventArgs const &args)
			{
				_captured = false;
			});
	}

	void MainWrapper::SetCursor(winrt::DCUR cursor)
	{
#ifdef WINUI3
		if (_mouseCaptureElement)
		{
			_mouseCaptureElement.try_as<winrt::Microsoft::UI::Xaml::IUIElementProtected>().ProtectedCursor(winrt::Microsoft::UI::Input::InputSystemCursor::Create(cursor));
		}
#else
		winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread().PointerCursor(winrt::Windows::UI::Core::CoreCursor{ cursor, 0 });
#endif
	}

	void MainWrapper::SetScrollBars(winrt::DUX::Controls::Primitives::ScrollBar const &horizontalScrollBar, winrt::DUX::Controls::Primitives::ScrollBar const &verticalScrollBar)
	{
		_horizontalScrollBar = horizontalScrollBar;
		_verticalScrollBar = verticalScrollBar;
		SetUseVerticalScrollBar(_useVerticalScrollBar); // hides the vertical scrollbar if needed
	}

	bool MainWrapper::HasScrollBars()
	{
		return _horizontalScrollBar && _verticalScrollBar;
	}

	void MainWrapper::HorizontalScrollBarValue(double value)
	{
		_horizontalScrollBar.Value(value);
	}

	void MainWrapper::VerticalScrollBarValue(double value)
	{
		_verticalScrollBar.Value(value);
	}

	void MainWrapper::HorizontalScrollBarMinimum(double value)
	{
		_horizontalScrollBar.Minimum(value);
	}

	void MainWrapper::VerticalScrollBarMinimum(double value)
	{
		_verticalScrollBar.Minimum(value);
	}

	void MainWrapper::HorizontalScrollBarMaximum(double value)
	{
		_horizontalScrollBar.Maximum(value);
	}

	void MainWrapper::VerticalScrollBarMaximum(double value)
	{
		_verticalScrollBar.Maximum(value);
	}

	void MainWrapper::HorizontalScrollBarViewportSize(double value)
	{
		_horizontalScrollBar.ViewportSize(value);
	}

	void MainWrapper::VerticalScrollBarViewportSize(double value)
	{
		_verticalScrollBar.ViewportSize(value);
	}

	double MainWrapper::HorizontalScrollBarValue()
	{
		return _horizontalScrollBar.Value();
	}

	double MainWrapper::VerticalScrollBarValue()
	{
		return _verticalScrollBar.Value();
	}

	double MainWrapper::HorizontalScrollBarMinimum()
	{
		return _horizontalScrollBar.Minimum();
	}

	double MainWrapper::VerticalScrollBarMinimum()
	{
		return _verticalScrollBar.Minimum();
	}

	double MainWrapper::HorizontalScrollBarMaximum()
	{
		return _horizontalScrollBar.Maximum();
	}

	double MainWrapper::VerticalScrollBarMaximum()
	{
		return _verticalScrollBar.Maximum();
	}

	double MainWrapper::HorizontalScrollBarViewportSize()
	{
		return _horizontalScrollBar.ViewportSize();
	}

	double MainWrapper::VerticalScrollBarViewportSize()
	{
		return _verticalScrollBar.ViewportSize();
	}

	winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::ApplicationModel::DataTransfer::DataPackageOperation> MainWrapper::StartDragAsync(winrt::DUI::PointerPoint const &pointerPoint)
	{
		return _mouseCaptureElement.StartDragAsync(pointerPoint);
	}

	void MainWrapper::Show(bool visible)
	{
	}

	void MainWrapper::Destroy()
	{
	}

	void MainWrapper::SetPositionRelative(Scintilla::Internal::PRectangle rc, Wrapper const &wrapper)
	{
	}

	void MainWrapper::SetContainer(winrt::DUX::FrameworkElement const& container)
	{
		_container = container;
	}

	/* ResizeContainersVertical
	   return true when resized
	*/

	bool MainWrapper::ResizeContainersVertical(double value)
	{
		if (_useVerticalScrollBar || _container == NULL) return false;
		
		auto height = _container.ActualHeight();
		if (std::isnan(height)) return true;

		if (value != floorf(height)) _container.Height(value  / LogicalDpi() * 96.0f);
		
		return true;
	}

	bool MainWrapper::GetUseVerticalScrollBar()
	{
		return _useVerticalScrollBar;
	}

	void MainWrapper::SetUseVerticalScrollBar(bool value)
	{
		_useVerticalScrollBar = value;
		if (_verticalScrollBar != NULL) _verticalScrollBar.Visibility((value) ? winrt::DUX::Visibility::Visible : winrt::DUX::Visibility::Collapsed);
	}
}
