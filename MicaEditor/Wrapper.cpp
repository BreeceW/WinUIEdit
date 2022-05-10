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
}
