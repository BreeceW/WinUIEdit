#include "pch.h"
#include "Wrapper.h"
#include "EditorBaseControl.h"
#include "CallTipControl.h"
#include "Helpers.h"

namespace WinUIEditor
{
	Wrapper::Wrapper(winrt::DUXC::Control const &control)
	{
		_control = control;
	}

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
		if (FAILED(D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			0,
			creationFlags,
			featureLevels,
			ARRAYSIZE(featureLevels),
			D3D11_SDK_VERSION,
			d3dDevice.put(),
			&supportedFeatureLevel,
			nullptr)))
		{
			winrt::check_hresult(D3D11CreateDevice(
				nullptr,
				D3D_DRIVER_TYPE_WARP,
				0,
				creationFlags,
				featureLevels,
				ARRAYSIZE(featureLevels),
				D3D11_SDK_VERSION,
				d3dDevice.put(),
				&supportedFeatureLevel,
				nullptr));
		}

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

	std::shared_ptr<Wrapper> Wrapper::CreateCallTipWindow(Scintilla::Internal::PRectangle rc, winrt::com_ptr<Scintilla::Internal::ScintillaWinUI> const &scintilla)
	{
		auto theme{ winrt::DUX::ElementTheme::Default };
		winrt::DUX::XamlRoot xamlRoot{ nullptr };

		if (const auto control{ _control.get() })
		{
#ifndef WINUI3
			if (_hasUac8)
			{
#endif
				// Needed for AppWindow
				// It should be fine if XamlRoot is not set on older versions pre-AppWindow and islands
				xamlRoot = control.XamlRoot();
#ifndef WINUI3
			}
			if (_hasFcu)
			{
#endif
				// Todo: Live theme changes
				theme = control.ActualTheme(); // Todo: path without ActualTheme
#ifndef WINUI3
			}
#endif
		}

		return CallTipWrapper::Create(scintilla, xamlRoot, _logicalDpi, theme);
	}

	void Wrapper::AdjustCallTipPadding(Scintilla::Internal::CallTip &callTip)
	{
		callTip.borderHeight = ::WinUIEditor::ConvertFromDipToPixelUnit(8, _logicalDpi / 96.f);
		callTip.insetX = ::WinUIEditor::ConvertFromDipToPixelUnit(10, _logicalDpi / 96.f);
	}

	bool Wrapper::TransformToRoot(float &x, float &y) const
	{
		if (const auto control{ _control.get() })
		{
			// TransformToVisual(nullptr) is relative to root
			const auto relative{ control.TransformToVisual(nullptr).TransformPoint({ x, y }) };
			x = relative.X;
			y = relative.Y;
			return true;
		}

		return false;
	}
}
