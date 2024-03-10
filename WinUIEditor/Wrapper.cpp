#include "pch.h"
#include "Wrapper.h"

namespace WinUIEditor
{
	winrt::com_ptr<::IVirtualSurfaceImageSourceNative> Wrapper::VsisNative()
	{
		return _vsisNative;
	}

	void Wrapper::VsisNative(winrt::com_ptr<::IVirtualSurfaceImageSourceNative> const &value)
	{
		_vsisNative = value;
		_sisNativeWithD2D = _vsisNative.as<::ISurfaceImageSourceNativeWithD2D>();
	}

	winrt::com_ptr<::ISurfaceImageSourceNativeWithD2D> Wrapper::SisNativeWithD2D()
	{
		return _sisNativeWithD2D;
	}

	winrt::com_ptr<::ID2D1DeviceContext> Wrapper::D2dDeviceContext()
	{
		return _d2dDeviceContext;
	}

	void Wrapper::TrimDxgiDevice()
	{
		if (_dxgiDevice)
		{
			_dxgiDevice->Trim();
		}
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
			_mouseCaptureElement.try_as<winrt::Microsoft::UI::Xaml::IUIElementProtected>().ProtectedCursor(winrt::Microsoft::UI::Input::InputSystemCursor::Create(cursor));
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

	winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::ApplicationModel::DataTransfer::DataPackageOperation> Wrapper::StartDragAsync(winrt::DUI::PointerPoint const &pointerPoint)
	{
		return _mouseCaptureElement.StartDragAsync(pointerPoint);
	}

	void Wrapper::CreateGraphicsDevices()
	{
		// Todo: Is there any cleanup that needs to be done when the control is deleted or if the device gets re-created?

		uint32_t creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1,
		};

		// Create the Direct3D device
		winrt::com_ptr<ID3D11Device> d3dDevice;
		D3D_FEATURE_LEVEL supportedFeatureLevel;
		winrt::check_hresult(D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			0,
			creationFlags,
			featureLevels,
			ARRAYSIZE(featureLevels),
			D3D11_SDK_VERSION,
			d3dDevice.put(),
			&supportedFeatureLevel,
			nullptr));

		// Get the Direct3D device.
		_dxgiDevice = d3dDevice.as<IDXGIDevice3>();

		// Create the Direct2D device and a corresponding context
		winrt::com_ptr<ID2D1Device> d2dDevice;
		D2D1CreateDevice(_dxgiDevice.get(), nullptr, d2dDevice.put());

		winrt::check_hresult(
			d2dDevice->CreateDeviceContext(
				D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
				_d2dDeviceContext.put()));

		// Associate the Direct2D device with the SurfaceImageSource
		SisNativeWithD2D()->SetDevice(d2dDevice.get());
	}
}
