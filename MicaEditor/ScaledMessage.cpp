#include "pch.h"
#include "ScaledMessage.h"
#include "ScaledMessage.g.cpp"

using namespace winrt::Windows::Foundation;

namespace winrt::MicaEditor::implementation
{
	ScaledMessage::ScaledMessage(bool scaleWParam, bool scaleLParam)
	{
		_scaleWParam = scaleWParam;
		_scaleLParam = scaleLParam;
	}

	bool ScaledMessage::ScaleWParam()
	{
		return _scaleWParam;
	}

	void ScaledMessage::ScaleWParam(bool value)
	{
		_scaleWParam = value;
	}

	bool ScaledMessage::ScaleLParam()
	{
		return _scaleLParam;
	}

	void ScaledMessage::ScaleLParam(bool value)
	{
		_scaleLParam = value;
	}

	uint64_t ScaledMessage::WParam()
	{
		return _wParam;
	}

	void ScaledMessage::WParam(uint64_t value)
	{
		_wParam = value;
	}

	int64_t ScaledMessage::LParam()
	{
		return _lParam;
	}

	void ScaledMessage::LParam(int64_t value)
	{
		_lParam = value;
	}
}
