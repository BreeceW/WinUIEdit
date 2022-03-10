#include "pch.h"
#include "MicaEditorControl.h"
#if __has_include("MicaEditorControl.g.cpp")
#include "MicaEditorControl.g.cpp"
#endif

using namespace ::MicaEditor;
using namespace winrt;
using namespace DUX;
using namespace DUX::Controls;
using namespace DUX::Input;
using namespace DUX::Media::Imaging;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;

namespace winrt::MicaEditor::implementation
{
	MicaEditorControl::MicaEditorControl()
	{
		DefaultStyleKey(winrt::box_value(L"MicaEditor.MicaEditorControl"));

		_wrapper = std::make_shared<Wrapper>();

		SizeChanged({ this, &MicaEditorControl::OnSizeChanged });

#ifndef WINUI3
		auto displayInformation{ DisplayInformation::GetForCurrentView() };
		_dpiChangedRevoker = displayInformation.DpiChanged(winrt::auto_revoke, { this, &MicaEditorControl::OnDpiChanged });
		UpdateDisplayInformation(displayInformation);
#endif

		_scintilla = make_self<::Scintilla::Internal::ScintillaWinUI>();
	}

	void MicaEditorControl::OnSizeChanged(const IInspectable &sender, const SizeChangedEventArgs &args)
	{
		if (_vsisNative)
		{
			auto width{ Helpers::ConvertFromDipToPixelUnit(args.NewSize().Width, _dpiScale) };
			auto height{ Helpers::ConvertFromDipToPixelUnit(args.NewSize().Height, _dpiScale) };
			_wrapper->Width(width);
			_wrapper->Height(height);
			_vsisNative->Resize(width, height);
			_scintilla->SizeChanged();
		}
	}

	void MicaEditorControl::OnDpiChanged(DisplayInformation const &sender, IInspectable const &args)
	{
		UpdateDisplayInformation(sender);
		_scintilla->DpiChanged();
	}

	void MicaEditorControl::UpdateDisplayInformation(DisplayInformation const &displayInformation)
	{
		_dpiScale = displayInformation.RawPixelsPerViewPixel();
		_wrapper->DpiScale(_dpiScale); // Todo: May remain unused
		_wrapper->LogicalDpi(displayInformation.LogicalDpi());
		if (_scintilla)
		{
			_scintilla->WndProc(Scintilla::Message::SetMarginWidthN, 1, Helpers::ConvertFromDipToPixelUnit(24, _dpiScale));
			_scintilla->WndProc(Scintilla::Message::SetMarginLeft, 0, Helpers::ConvertFromDipToPixelUnit(8, _dpiScale));
		}
	}

	int32_t MicaEditorControl::MyProperty()
	{
		return _myProperty;
	}

	void MicaEditorControl::MyProperty(int32_t value)
	{
		_myProperty = value;
	}

	uint64_t MicaEditorControl::Scintilla(int32_t message, uint64_t wParam, uint64_t lParam)
	{
		return _scintilla->WndProc(static_cast<Scintilla::Message>(message), wParam, lParam);
	}

	void MicaEditorControl::OnApplyTemplate()
	{
#ifdef WINUI3
		// Temporary until it is known how to respond to DPI changes with WASDK
		_dpiScale = XamlRoot().RasterizationScale();
		_wrapper->DpiScale(_dpiScale);
		_wrapper->LogicalDpi(96 * _dpiScale);
#endif

		_scintilla->WndProc(Scintilla::Message::InsertText, 0, reinterpret_cast<Scintilla::uptr_t>("Insert text"));
		_scintilla->WndProc(Scintilla::Message::SetWrapMode, SC_WRAP_WHITESPACE, 0);
		_scintilla->WndProc(Scintilla::Message::SetMarginTypeN, 1, SC_MARGIN_NUMBER);
		_scintilla->WndProc(Scintilla::Message::SetMarginWidthN, 1, Helpers::ConvertFromDipToPixelUnit(24, _dpiScale));
		_scintilla->WndProc(Scintilla::Message::SetMarginLeft, 0, Helpers::ConvertFromDipToPixelUnit(8, _dpiScale));

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

		auto width{ Helpers::ConvertFromDipToPixelUnit(ActualWidth(), _dpiScale) };
		auto height{ Helpers::ConvertFromDipToPixelUnit(ActualHeight(), _dpiScale) };
		_wrapper->Width(width);
		_wrapper->Height(height);
		VirtualSurfaceImageSource virtualSurfaceImageSource{ width, height };

		winrt::com_ptr<::ISurfaceImageSourceNativeWithD2D> sisNativeWithD2D{
			virtualSurfaceImageSource.as<::ISurfaceImageSourceNativeWithD2D>() };

		// Associate the Direct2D device with the SurfaceImageSource.
		sisNativeWithD2D->SetDevice(d2dDevice.get());

		// To enable multi-threaded access (optional)
		winrt::com_ptr<::ID3D11Multithread> d3dMultiThread{
			d3dDevice.as<::ID3D11Multithread>() };
		d3dMultiThread->SetMultithreadProtected(true);

		_vsisNative = virtualSurfaceImageSource.as<::IVirtualSurfaceImageSourceNative>();

		_wrapper->VsisNative(_vsisNative);
		_scintilla->RegisterGraphics(sisNativeWithD2D, _vsisNative, d2dDeviceContext, _wrapper);
		_vsisNative->RegisterForUpdatesNeeded(_scintilla.as<::IVirtualSurfaceUpdatesCallbackNative>().get());

		// The SurfaceImageSource object's underlying 
		// ISurfaceImageSourceNativeWithD2D object will contain the completed bitmap.

		if (auto imageTarget{ GetTemplateChild(L"ImageTarget").try_as<Image>() })
		{
			imageTarget.Source(virtualSurfaceImageSource);
			imageTarget.Tapped({ this, &MicaEditorControl::Image_Tapped });
		}
	}

	void MicaEditorControl::OnGotFocus(RoutedEventArgs const &args)
	{
		__super::OnGotFocus(args);

		//_scintilla->FocusChanged(true);
	}

	void MicaEditorControl::OnLostFocus(RoutedEventArgs const &args)
	{
		__super::OnLostFocus(args);

		//_scintilla->FocusChanged(false);
	}

	void MicaEditorControl::Image_Tapped(Windows::Foundation::IInspectable const &sender, TappedRoutedEventArgs const &args)
	{
		_scintilla->WndProc(Scintilla::Message::AppendText, 13, reinterpret_cast<Scintilla::uptr_t>("\r\nInsert text"));
	}
}
