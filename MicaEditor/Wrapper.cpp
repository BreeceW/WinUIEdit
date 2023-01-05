#include "pch.h"
#include "Wrapper.h"

namespace MicaEditor
{
	winrt::com_ptr<::IVirtualSurfaceImageSourceNative> Wrapper::VsisNative()
	{
		return _vsisNative;
	}

	void Wrapper::VsisNative(winrt::com_ptr<::IVirtualSurfaceImageSourceNative> const &vsisNative)
	{
		_vsisNative = vsisNative;
	}

	float Wrapper::DpiScale()
	{
		return _dpiScale;
	}

	void Wrapper::DpiScale(float value)
	{
		_dpiScale = value;
	}

	float Wrapper::LogicalDpi()
	{
		return _logicalDpi;
	}

	void Wrapper::LogicalDpi(float value)
	{
		_logicalDpi = value;
	}

	int Wrapper::Width()
	{
		return _width;
	}

	void Wrapper::Width(int value)
	{
		_width = value;
	}

	int Wrapper::Height()
	{
		return _height;
	}

	void Wrapper::Height(int value)
	{
		_height = value;
	}

	void Wrapper::SetMouseCapture(bool on)
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

	bool Wrapper::HaveMouseCapture()
	{
		return _captured;
	}

	void Wrapper::SetMouseCaptureElement(winrt::DUX::UIElement const &element)
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

	void Wrapper::SetCursor(winrt::DCUR cursor)
	{
#ifdef WINUI3
		if (_mouseCaptureElement)
		{
			// Todo: This is protected but works anyway
			_mouseCaptureElement.ProtectedCursor(winrt::Microsoft::UI::Input::InputSystemCursor::Create(cursor));
		}
#else
		winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread().PointerCursor(winrt::Windows::UI::Core::CoreCursor{ cursor, 0 });
#endif
	}

	void Wrapper::SetScrollBars(winrt::DUX::Controls::Primitives::ScrollBar const &horizontalScrollBar, winrt::DUX::Controls::Primitives::ScrollBar const &verticalScrollBar)
	{
		_horizontalScrollBar = horizontalScrollBar;
		_verticalScrollBar = verticalScrollBar;
	}

	bool Wrapper::HasScrollBars()
	{
		return _horizontalScrollBar && _verticalScrollBar;
	}

	void Wrapper::HorizontalScrollBarValue(double value)
	{
		_horizontalScrollBar.Value(value);
	}

	void Wrapper::VerticalScrollBarValue(double value)
	{
		_verticalScrollBar.Value(value);
	}

	void Wrapper::HorizontalScrollBarMinimum(double value)
	{
		_horizontalScrollBar.Minimum(value);
	}

	void Wrapper::VerticalScrollBarMinimum(double value)
	{
		_verticalScrollBar.Minimum(value);
	}

	void Wrapper::HorizontalScrollBarMaximum(double value)
	{
		_horizontalScrollBar.Maximum(value);
	}

	void Wrapper::VerticalScrollBarMaximum(double value)
	{
		_verticalScrollBar.Maximum(value);
	}

	void Wrapper::HorizontalScrollBarViewportSize(double value)
	{
		_horizontalScrollBar.ViewportSize(value);
	}

	void Wrapper::VerticalScrollBarViewportSize(double value)
	{
		_verticalScrollBar.ViewportSize(value);
	}

	double Wrapper::HorizontalScrollBarValue()
	{
		return _horizontalScrollBar.Value();
	}

	double Wrapper::VerticalScrollBarValue()
	{
		return _verticalScrollBar.Value();
	}

	double Wrapper::HorizontalScrollBarMinimum()
	{
		return _horizontalScrollBar.Minimum();
	}

	double Wrapper::VerticalScrollBarMinimum()
	{
		return _verticalScrollBar.Minimum();
	}

	double Wrapper::HorizontalScrollBarMaximum()
	{
		return _horizontalScrollBar.Maximum();
	}

	double Wrapper::VerticalScrollBarMaximum()
	{
		return _verticalScrollBar.Maximum();
	}

	double Wrapper::HorizontalScrollBarViewportSize()
	{
		return _horizontalScrollBar.ViewportSize();
	}

	double Wrapper::VerticalScrollBarViewportSize()
	{
		return _verticalScrollBar.ViewportSize();
	}
}
