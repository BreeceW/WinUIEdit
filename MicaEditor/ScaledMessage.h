#pragma once
#include "ScaledMessage.g.h"

namespace winrt::MicaEditor::implementation
{
	struct ScaledMessage : ScaledMessageT<ScaledMessage>
	{
		ScaledMessage(bool scaleWParam, bool scaleLParam);

		bool ScaleWParam();
		void ScaleWParam(bool value);
		bool ScaleLParam();
		void ScaleLParam(bool value);

		uint64_t WParam();
		void WParam(uint64_t value);
		int64_t LParam();
		void LParam(int64_t value);

	private:
		bool _scaleWParam;
		bool _scaleLParam;
		uint64_t _wParam{ 0 };
		int64_t _lParam{ 0 };
	};
}
namespace winrt::MicaEditor::factory_implementation
{
	struct ScaledMessage : ScaledMessageT<ScaledMessage, implementation::ScaledMessage>
	{
	};
}
