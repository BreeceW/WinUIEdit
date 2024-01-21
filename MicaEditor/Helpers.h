#pragma once

namespace WinUIEditor
{
	int ConvertFromDipToPixelUnit(float val, float dpiAdjustmentRatio, bool rounded = true);
	bool IsClassicWindow();
	winrt::Windows::System::VirtualKeyModifiers GetKeyModifiersForCurrentThread();
}
