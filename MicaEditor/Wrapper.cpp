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
}
