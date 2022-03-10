#include "pch.h"
#include "Helpers.h"

namespace MicaEditor
{
	// Derived from https://github.com/microsoft/Windows-universal-samples/blob/main/Samples/ComplexInk/cpp/Common/DeviceResources.h
	int Helpers::ConvertFromDipToPixelUnit(float val, float dpiAdjustmentRatio, bool rounded)
	{
		float pixelVal = val * dpiAdjustmentRatio;
		return static_cast<int>(rounded ? floorf(pixelVal + 0.5f) : pixelVal); // Todo: Test if this is ever necessary
	}
}
