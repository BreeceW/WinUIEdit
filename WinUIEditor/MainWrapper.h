#pragma once

#include "Wrapper.h"

namespace WinUIEditor
{
	class MainWrapper : public Wrapper
	{
	public:
		MainWrapper(winrt::DUXC::Control const &control);

		// Todo: Make abstract
		void SetMouseCapture(bool on);
		bool HaveMouseCapture();

		// Todo: Move to separate XAML class
		void SetMouseCaptureElement(winrt::DUX::UIElement const &element);
		void SetCursor(winrt::DCUR cursor);

		void SetScrollBars(winrt::DUX::Controls::Primitives::ScrollBar const &horizontalScrollBar, winrt::DUX::Controls::Primitives::ScrollBar const &verticalScrollBar);
		bool HasScrollBars();
		void HorizontalScrollBarValue(double value);
		void VerticalScrollBarValue(double value);
		void HorizontalScrollBarMinimum(double value);
		void VerticalScrollBarMinimum(double value);
		void HorizontalScrollBarMaximum(double value);
		void VerticalScrollBarMaximum(double value);
		void HorizontalScrollBarViewportSize(double value);
		void VerticalScrollBarViewportSize(double value);
		double HorizontalScrollBarValue();
		double VerticalScrollBarValue();
		double HorizontalScrollBarMinimum();
		double VerticalScrollBarMinimum();
		double HorizontalScrollBarMaximum();
		double VerticalScrollBarMaximum();
		double HorizontalScrollBarViewportSize();
		double VerticalScrollBarViewportSize();

		void SetContainer(winrt::DUX::FrameworkElement const& container);
		bool ResizeContainersVertical(double value);
		bool GetUseVerticalScrollBar();
		void SetUseVerticalScrollBar(bool value);

		winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::ApplicationModel::DataTransfer::DataPackageOperation> StartDragAsync(winrt::DUI::PointerPoint const &pointerPoint);

		void Show(bool visible) override;
		void Destroy() override;
		void SetPositionRelative(Scintilla::Internal::PRectangle rc, Wrapper const &wrapper) override;

	private:
		winrt::DUX::Input::Pointer _lastPointer{ nullptr };
		winrt::DUX::UIElement _mouseCaptureElement{ nullptr };
		winrt::DUX::Controls::Primitives::ScrollBar _horizontalScrollBar{ nullptr };
		winrt::DUX::Controls::Primitives::ScrollBar _verticalScrollBar{ nullptr };
		bool _captured{ false };
		bool _useVerticalScrollBar{ false };
		winrt::DUX::FrameworkElement _container{ nullptr };
	};
}
