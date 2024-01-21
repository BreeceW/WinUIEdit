#pragma once

namespace WinUIEditor
{
	class Wrapper
	{
	public:
		winrt::com_ptr<::IVirtualSurfaceImageSourceNative> VsisNative();
		void VsisNative(winrt::com_ptr<::IVirtualSurfaceImageSourceNative> const &vsisNative);
		float LogicalDpi();
		void LogicalDpi(float value);
		int Width();
		void Width(int value);
		int Height();
		void Height(int value);

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

		winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::ApplicationModel::DataTransfer::DataPackageOperation> StartDragAsync(winrt::DUI::PointerPoint const &pointerPoint);

	private:
		winrt::DUX::Input::Pointer _lastPointer{ nullptr };
		winrt::DUX::UIElement _mouseCaptureElement{ nullptr };
		winrt::DUX::Controls::Primitives::ScrollBar _horizontalScrollBar{ nullptr };
		winrt::DUX::Controls::Primitives::ScrollBar _verticalScrollBar{ nullptr };
		bool _captured;
		winrt::com_ptr<IVirtualSurfaceImageSourceNative> _vsisNative{ nullptr };
		float _logicalDpi;
		int _width;
		int _height;
	};
}
