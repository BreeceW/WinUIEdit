#pragma once

namespace MicaEditor
{
	class Helpers
	{
	public:
		static int ConvertFromDipToPixelUnit(float val, float dpiAdjustmentRatio, bool rounded = true);
	};
}
