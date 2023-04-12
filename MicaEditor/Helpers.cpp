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

	typedef LONG(*AppPolicyGetWindowingModelPtr)(HANDLE processToken, AppPolicyWindowingModel *policy);
	static AppPolicyGetWindowingModelPtr s_appPolicyGetWindowingModel;

	bool Helpers::IsClassicWindow()
	{
		// To avoid issues with AppPolicyGetWindowingModel and Store certification, this method cannot be called normally
		// See https://github.com/microsoft/react-native-windows/pull/5369 (though OneCoreUAP_apiset.lib did pass either)

		if (!s_appPolicyGetWindowingModel)
		{
			s_appPolicyGetWindowingModel =
				reinterpret_cast<AppPolicyGetWindowingModelPtr>(GetProcAddress(GetModuleHandleW(L"Api-ms-win-appmodel-runtime-l1-1-2.dll"), "AppPolicyGetWindowingModel"));
		}
		AppPolicyWindowingModel model;
		return !(s_appPolicyGetWindowingModel && SUCCEEDED(s_appPolicyGetWindowingModel(GetCurrentThreadEffectiveToken(), &model)))
			|| model != AppPolicyWindowingModel_Universal;
	}
}
