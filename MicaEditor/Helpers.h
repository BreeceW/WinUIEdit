#pragma once

namespace MicaEditor
{
	// Todo: This does not need to be a class
	class Helpers
	{
	public:
		static int ConvertFromDipToPixelUnit(float val, float dpiAdjustmentRatio, bool rounded = true);
		static bool IsClassicWindow();
		static winrt::Windows::System::VirtualKeyModifiers GetKeyModifiersForCurrentThread();
	};
}
