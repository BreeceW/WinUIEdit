#pragma once

namespace Scintilla::Internal
{
	class ScintillaWinUI;
}

namespace WinUIEditor
{
	class Wrapper
	{
	public:
		winrt::com_ptr<::IVirtualSurfaceImageSourceNative> VsisNative();
		void VsisNative(winrt::com_ptr<::IVirtualSurfaceImageSourceNative> const &value);
		winrt::com_ptr<::ISurfaceImageSourceNativeWithD2D> SisNativeWithD2D();
		winrt::com_ptr<::ID2D1DeviceContext> D2dDeviceContext();
		void TrimDxgiDevice();
		float LogicalDpi();
		void LogicalDpi(float value);
		int Width();
		void Width(int value);
		int Height();
		void Height(int value);

		void CreateGraphicsDevices();

		std::shared_ptr<Wrapper> CreateCallTipWindow(Scintilla::Internal::PRectangle rc, winrt::com_ptr<Scintilla::Internal::ScintillaWinUI> const &scintilla);
		virtual void SetPositionRelative(Scintilla::Internal::PRectangle rc, Wrapper const &wrapper) = 0;

		virtual void Show(bool visible) = 0;
		virtual void Destroy() = 0;
		void AdjustCallTipPadding(Scintilla::Internal::CallTip &callTip);

		bool TransformToRoot(float &x, float &y) const;

		// Font used for arrows in folding column markers. Stored here to allow retrieval from SurfaceD2D
		// Do not access outside of GetChevronFontFromSurface
		std::shared_ptr<Scintilla::Internal::Font> chevronFont{ nullptr };
		Scintilla::Internal::XYPOSITION chevronFontSize{ -1.0 };

	protected:
		Wrapper(winrt::DUXC::Control const& control);
		winrt::weak_ref<winrt::DUXC::Control> _control{ nullptr };

	private:
		winrt::com_ptr<IVirtualSurfaceImageSourceNative> _vsisNative{ nullptr };
		winrt::com_ptr<::ISurfaceImageSourceNativeWithD2D> _sisNativeWithD2D{ nullptr };
		winrt::com_ptr<::ID2D1DeviceContext> _d2dDeviceContext{ nullptr };
		winrt::com_ptr<::IDXGIDevice3> _dxgiDevice{ nullptr };
		float _logicalDpi{ 96.f };
		int _width{ 0 };
		int _height{ 0 };
#ifndef WINUI3
		bool _hasUac8{ winrt::Windows::Foundation::Metadata::ApiInformation::IsApiContractPresent(L"Windows.Foundation.UniversalApiContract", 8) }; // Todo: Make static
		bool _hasFcu{ winrt::Windows::Foundation::Metadata::ApiInformation::IsApiContractPresent(L"Windows.Foundation.UniversalApiContract", 5) }; // Todo: Make static
#endif
	};
}
