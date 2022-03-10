#pragma once

namespace MicaEditor
{
	class Wrapper
	{
	public:
		winrt::com_ptr<::IVirtualSurfaceImageSourceNative> VsisNative();
		void VsisNative(winrt::com_ptr<::IVirtualSurfaceImageSourceNative> const &vsisNative);
		float DpiScale();
		void DpiScale(float value);
		float LogicalDpi();
		void LogicalDpi(float value);
		int Width();
		void Width(int value);
		int Height();
		void Height(int value);

	private:
		winrt::com_ptr<IVirtualSurfaceImageSourceNative> _vsisNative{ nullptr };
		float _dpiScale;
		float _logicalDpi;
		int _width;
		int _height;
	};
}
