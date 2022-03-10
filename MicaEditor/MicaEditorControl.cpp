#include "pch.h"
#include "MicaEditorControl.h"
#if __has_include("MicaEditorControl.g.cpp")
#include "MicaEditorControl.g.cpp"
#endif

using namespace winrt;
using namespace DUX;
using namespace DUX::Controls;
using namespace DUX::Media::Imaging;

namespace winrt::MicaEditor::implementation
{
	MicaEditorControl::MicaEditorControl()
	{
		DefaultStyleKey(winrt::box_value(L"MicaEditor.MicaEditorControl"));

		_scintilla = make_self<::Scintilla::Internal::ScintillaWinUI>();
	}

	int32_t MicaEditorControl::MyProperty()
	{
		return _myProperty;
	}

	void MicaEditorControl::MyProperty(int32_t value)
	{
		_myProperty = value;
	}

	void MicaEditorControl::OnApplyTemplate()
	{
		_scintilla->WndProc(Scintilla::Message::InsertText, 0, reinterpret_cast<Scintilla::uptr_t>("Insert text"));
		_scintilla->WndProc(Scintilla::Message::SetWrapMode, SC_WRAP_WHITESPACE, 0);
		_scintilla->WndProc(Scintilla::Message::SetMarginTypeN, 1, SC_MARGIN_NUMBER);

		uint32_t creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1
		};

		// Create the Direct3D device.
		winrt::com_ptr<::ID3D11Device> d3dDevice;
		D3D_FEATURE_LEVEL supportedFeatureLevel;
		winrt::check_hresult(::D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			0,
			creationFlags,
			featureLevels,
			ARRAYSIZE(featureLevels),
			D3D11_SDK_VERSION,
			d3dDevice.put(),
			&supportedFeatureLevel,
			nullptr)
		);

		// Get the Direct3D device.
		winrt::com_ptr<::IDXGIDevice> dxgiDevice{
			d3dDevice.as<::IDXGIDevice>() };

		// Create the Direct2D device and a corresponding context.
		winrt::com_ptr<::ID2D1Device> d2dDevice;
		::D2D1CreateDevice(dxgiDevice.get(), nullptr, d2dDevice.put());

		winrt::com_ptr<::ID2D1DeviceContext> d2dDeviceContext;
		winrt::check_hresult(
			d2dDevice->CreateDeviceContext(
				D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
				d2dDeviceContext.put()
			)
		);

		VirtualSurfaceImageSource virtualSurfaceImageSource(375, 375);

		winrt::com_ptr<::ISurfaceImageSourceNativeWithD2D> sisNativeWithD2D{
			virtualSurfaceImageSource.as<::ISurfaceImageSourceNativeWithD2D>() };

		// Associate the Direct2D device with the SurfaceImageSource.
		sisNativeWithD2D->SetDevice(d2dDevice.get());

		// To enable multi-threaded access (optional)
		winrt::com_ptr<::ID3D11Multithread> d3dMultiThread{
			d3dDevice.as<::ID3D11Multithread>() };
		d3dMultiThread->SetMultithreadProtected(true);

		winrt::com_ptr<::IVirtualSurfaceImageSourceNative> vsisNative{
			virtualSurfaceImageSource.as<::IVirtualSurfaceImageSourceNative>() };

		_scintilla->RegisterGraphics(sisNativeWithD2D, vsisNative, d2dDeviceContext);
		vsisNative->RegisterForUpdatesNeeded(_scintilla.as<::IVirtualSurfaceUpdatesCallbackNative>().get());

		// The SurfaceImageSource object's underlying 
		// ISurfaceImageSourceNativeWithD2D object will contain the completed bitmap.

		if (auto imageTarget{ GetTemplateChild(L"ImageTarget").try_as<Image>() })
		{
			imageTarget.Source(virtualSurfaceImageSource);
			imageTarget.Tapped({ this, &MicaEditorControl::Image_Tapped });
		}
	}

	void MicaEditorControl::Image_Tapped(Windows::Foundation::IInspectable const &sender, DUX::Input::TappedRoutedEventArgs const &args)
	{
		_scintilla->WndProc(Scintilla::Message::InsertText, 0, reinterpret_cast<Scintilla::uptr_t>("Insert text "));
	}
}
