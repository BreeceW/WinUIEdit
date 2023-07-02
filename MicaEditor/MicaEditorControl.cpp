#include "pch.h"
#include "MicaEditorControl.h"
#if __has_include("MicaEditorControl.g.cpp")
#include "MicaEditorControl.g.cpp"
#endif
#include "EditorWrapper.h"
#include "Helpers.h"

using namespace ::MicaEditor;
using namespace winrt;
using namespace DUX;
using namespace DUX::Controls;
using namespace DUX::Controls::Primitives;
using namespace DUX::Input;
using namespace DUX::Media;
using namespace DUX::Media::Imaging;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::System;
using namespace Windows::Graphics::Display;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::DataTransfer;

namespace winrt::MicaEditor::implementation
{
	// Todo: Something about this control is keeping the WinUI 3 versions from closing.
	// Note that making this into a blank control fixes the issue, so it is definitely something here.
	MicaEditorControl::MicaEditorControl()
	{
		DefaultStyleKey(winrt::box_value(L"MicaEditor.MicaEditorControl"));

		_wrapper = std::make_shared<Wrapper>();

		_loadedRevoker = Loaded(auto_revoke, { this, &MicaEditorControl::OnLoaded });
		_unloadedRevoker = Unloaded(auto_revoke, { this, &MicaEditorControl::OnUnloaded });

#ifndef WINUI3
		if (!_hasXamlRoot)
		{
			_displayInformation = DisplayInformation::GetForCurrentView();
		}
#endif

		_scintilla = make_self<Scintilla::Internal::ScintillaWinUI>();

		_editorWrapper = make<implementation::Editor>(get_strong());

		_scintilla->SetWndProcTag(*this);
		_scintilla->SetWndProc(&MicaEditorControl::WndProc);

#ifndef WINUI3
		if (_hasFcu)
		{
#endif
			// Registering the CharacterReceived event causes undesirable behavior with TSF in CoreWindow (normal UWP)
			// but is required to get text in classic windows (XAML Islands and WinUI 3)
			// Todo: Find more ideal way to do this
			// Tried using _tfThreadManager->GetActiveFlags but TF_TMF_IMMERSIVEMODE flag was not accurate
			if (IsClassicWindow())
			{
				CharacterReceived({ this, &MicaEditorControl::MicaEditorControl_CharacterReceived });
			}
#ifndef WINUI3
		}
#endif
	}

	MicaEditorControl::~MicaEditorControl()
	{
		_scintilla->Finalize();
	}

	MicaEditor::Editor MicaEditorControl::Editor()
	{
		return _editorWrapper;
	}

	void MicaEditorControl::OnLoaded(IInspectable const &sender, DUX::RoutedEventArgs const &args)
	{
		// Following pattern from https://github.com/microsoft/microsoft-ui-xaml/blob/a7183df20367bc0e2b8c825430597a5c1e6871b6/dev/WebView2/WebView2.cpp#L1556

#ifndef WINUI3
		_isLoaded = true;
#endif

		if (!IsLoadedCompat())
		{
			return;
		}

#ifndef WINUI3
		if (_hasXamlRoot)
		{
#endif
			UpdateDpi(XamlRoot().RasterizationScale());
			_xamlRootChangedRevoker = XamlRoot().Changed(auto_revoke, { this, &MicaEditorControl::XamlRoot_Changed });
#ifndef WINUI3
		}
		else
		{
			UpdateDpi(_displayInformation.RawPixelsPerViewPixel());
			_dpiChangedRevoker = _displayInformation.DpiChanged(auto_revoke, { this, &MicaEditorControl::DisplayInformation_DpiChanged });
		}
#endif
	}

	void MicaEditorControl::OnUnloaded(IInspectable const &sender, DUX::RoutedEventArgs const &args)
	{
#ifndef WINUI3
		_isLoaded = false;
#endif

		if (IsLoadedCompat())
		{
			return;
		}

#ifndef WINUI3
		if (_hasXamlRoot)
		{
#endif
			_xamlRootChangedRevoker.revoke();
#ifndef WINUI3
		}
		else
		{
			_dpiChangedRevoker.revoke();
		}
#endif

		_scintilla->StopTimers();
	}

	bool MicaEditorControl::IsLoadedCompat()
	{
#ifndef WINUI3
		if (_hasIsLoaded)
		{
#endif
			return IsLoaded();
#ifndef WINUI3
		}
		else
		{
			return _isLoaded;
		}
#endif
	}

#ifndef WINUI3
	void MicaEditorControl::DisplayInformation_DpiChanged(DisplayInformation const &sender, IInspectable const &args)
	{
		UpdateDpi(sender.RawPixelsPerViewPixel());
	}
#endif

	void MicaEditorControl::XamlRoot_Changed(DUX::XamlRoot const &sender, XamlRootChangedEventArgs const &args)
	{
		UpdateDpi(sender.RasterizationScale());
	}

	void MicaEditorControl::UpdateDpi(float dpiScale)
	{
		if (_dpiScale != dpiScale)
		{
			_wrapper->LogicalDpi(dpiScale * 96);
			_dpiScale = dpiScale;
			_scintilla->DpiChanged();
		}
	}

	void MicaEditorControl::AddContextMenuItems(MenuFlyout const &menu)
	{
		menu.Opening([&](const auto &, const auto &)
			{
				_isContextMenuOpen = true;
			});
		menu.Closing([&](const auto &, const auto &)
			{
				_isContextMenuOpen = false;
			});

		const auto writable{ !static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetReadOnly, 0, 0)) };
		const auto selection{ !static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetSelectionEmpty, 0, 0)) };

		const MenuFlyoutItem undoItem{};
		undoItem.Text(L"Undo"); // Todo: Localize
		undoItem.Icon(SymbolIcon{ Symbol::Undo });
		undoItem.Tag(box_value(ScintillaMessage::Undo));
		undoItem.IsEnabled(_scintilla->WndProc(Scintilla::Message::CanUndo, 0, 0));
		undoItem.Click({ this, &MicaEditorControl::ContextMenuItem_Click }); // Todo: Revoke event handler?
		menu.Items().Append(undoItem);

		const MenuFlyoutItem redoItem{};
		redoItem.Text(L"Redo");
		redoItem.Icon(SymbolIcon{ Symbol::Redo });
		redoItem.Tag(box_value(ScintillaMessage::Redo));
		redoItem.IsEnabled(_scintilla->WndProc(Scintilla::Message::CanRedo, 0, 0));
		redoItem.Click({ this, &MicaEditorControl::ContextMenuItem_Click });
		menu.Items().Append(redoItem);

		menu.Items().Append(MenuFlyoutSeparator{});

		const MenuFlyoutItem cutItem{};
		cutItem.Text(L"Cut");
		cutItem.Icon(SymbolIcon{ Symbol::Cut });
		cutItem.Tag(box_value(ScintillaMessage::Cut));
		cutItem.IsEnabled(writable && selection);
		cutItem.Click({ this, &MicaEditorControl::ContextMenuItem_Click });
		menu.Items().Append(cutItem);

		const MenuFlyoutItem copyItem{};
		copyItem.Text(L"Copy");
		copyItem.Icon(SymbolIcon{ Symbol::Copy });
		copyItem.Tag(box_value(ScintillaMessage::Copy));
		copyItem.IsEnabled(selection);
		copyItem.Click({ this, &MicaEditorControl::ContextMenuItem_Click });
		menu.Items().Append(copyItem);

		const MenuFlyoutItem pasteItem{};
		pasteItem.Text(L"Paste");
		pasteItem.Icon(SymbolIcon{ Symbol::Paste });
		pasteItem.Tag(box_value(ScintillaMessage::Paste));
		pasteItem.IsEnabled(_scintilla->WndProc(Scintilla::Message::CanPaste, 0, 0));
		pasteItem.Click({ this, &MicaEditorControl::ContextMenuItem_Click });
		menu.Items().Append(pasteItem);

		const MenuFlyoutItem deleteItem{};
		deleteItem.Text(L"Delete");
		deleteItem.Icon(SymbolIcon{ Symbol::Delete });
		deleteItem.Tag(box_value(ScintillaMessage::Clear));
		deleteItem.IsEnabled(writable && selection);
		deleteItem.Click({ this, &MicaEditorControl::ContextMenuItem_Click });
		menu.Items().Append(deleteItem);

		menu.Items().Append(MenuFlyoutSeparator{});

		const MenuFlyoutItem selectAllItem{};
		selectAllItem.Text(L"Select all");
		selectAllItem.Icon(SymbolIcon{ Symbol::SelectAll });
		selectAllItem.Tag(box_value(ScintillaMessage::SelectAll));
		selectAllItem.Click({ this, &MicaEditorControl::ContextMenuItem_Click });
		menu.Items().Append(selectAllItem);
	}

	bool MicaEditorControl::ShowContextMenu(UIElement const &targetElement, Point const &point)
	{
		if (_scintilla->ShouldShowContextMenu(Point{ point.X * _dpiScale, point.Y * _dpiScale }))
		{
			const MenuFlyout menu{};
			AddContextMenuItems(menu);
			menu.ShowAt(targetElement, point);
			return true;
		}
		return false;
	}

	bool MicaEditorControl::ShowContextMenuAtCurrentPosition()
	{
		if (auto imageTarget{ GetTemplateChild(L"ImageTarget").try_as<UIElement>() }) // Todo: Store this
		{
			const auto pos{ _scintilla->WndProc(Scintilla::Message::GetCurrentPos, 0, 0) };
			return ShowContextMenu(imageTarget, Point{
				_scintilla->WndProc(Scintilla::Message::PointXFromPosition, 0, pos) / _dpiScale,
				(_scintilla->WndProc(Scintilla::Message::PointYFromPosition, 0, pos) + _scintilla->WndProc(Scintilla::Message::TextHeight, 0, 0)) / _dpiScale
				});
		}
		return false;
	}

	Scintilla::sptr_t MicaEditorControl::PublicWndProc(Scintilla::Message iMessage, Scintilla::uptr_t wParam, Scintilla::sptr_t lParam)
	{
		return _scintilla->WndProc(iMessage, wParam, lParam);
	}

	uint64_t MicaEditorControl::Scintilla(ScintillaMessage const &message, uint64_t wParam, uint64_t lParam)
	{
		return PublicWndProc(static_cast<Scintilla::Message>(message), wParam, lParam);
	}

	void MicaEditorControl::StyleSetBackTransparent(int style, Scintilla::Internal::ColourRGBA color)
	{
		_scintilla->StyleSetBackTransparent(style, color);
	}

	void MicaEditorControl::OnApplyTemplate()
	{
		__super::OnApplyTemplate();

#ifndef WINUI3
		if (_hasXamlRoot)
		{
#endif
			UpdateDpi(XamlRoot().RasterizationScale());
#ifndef WINUI3
		}
		else
		{
			UpdateDpi(_displayInformation.RawPixelsPerViewPixel());
		}
#endif

		VirtualSurfaceImageSource virtualSurfaceImageSource{ 0, 0 };


		_vsisNative = virtualSurfaceImageSource.as<::IVirtualSurfaceImageSourceNative>();

		_wrapper->VsisNative(_vsisNative);
		_scintilla->RegisterGraphics(_wrapper);
		_vsisNative->RegisterForUpdatesNeeded(_scintilla.as<::IVirtualSurfaceUpdatesCallbackNative>().get());

		// The SurfaceImageSource object's underlying
		// ISurfaceImageSourceNativeWithD2D object will contain the completed bitmap.

		auto horizontalScrollBar{ GetTemplateChild(L"HorizontalScrollBar").try_as<ScrollBar>() };
		auto verticalScrollBar{ GetTemplateChild(L"VerticalScrollBar").try_as<ScrollBar>() };
		if (horizontalScrollBar)
		{
			horizontalScrollBar.Scroll({ this, &MicaEditorControl::HorizontalScrollBar_Scroll });
		}
		if (verticalScrollBar)
		{
			verticalScrollBar.Scroll({ this, &MicaEditorControl::VerticalScrollBar_Scroll });
		}
		_wrapper->SetScrollBars(horizontalScrollBar, verticalScrollBar);

		if (auto imageTarget{ GetTemplateChild(L"ImageTarget").try_as<Border>() })
		{
			// Todo: do these need auto revokers
			// Todo: is this safe to have in OnApplyTemplate?
			imageTarget.SizeChanged({ this, &MicaEditorControl::ImageTarget_SizeChanged });
			imageTarget.PointerWheelChanged({ this, &MicaEditorControl::ImageTarget_PointerWheelChanged });
			imageTarget.DragEnter({ this, &MicaEditorControl::ImageTarget_DragEnter });
			imageTarget.DragOver({ this, &MicaEditorControl::ImageTarget_DragOver });
			imageTarget.DragLeave({ this, &MicaEditorControl::ImageTarget_DragLeave });
			imageTarget.Drop({ this, &MicaEditorControl::ImageTarget_Drop });

			_wrapper->SetMouseCaptureElement(imageTarget);
			imageTarget.DragStarting({ this, &MicaEditorControl::ImageTarget_DragStarting });

			imageTarget.ContextRequested({ this, &MicaEditorControl::ImageTarget_ContextRequested });

			ImageBrush brush{};
			brush.ImageSource(virtualSurfaceImageSource);
			imageTarget.Background(brush);
		}

#ifndef WINUI3
		// Todo: Evaluate if this is an appropriate place to add this event (and other code in this method)
		_suspendingRevoker = Application::Current().Suspending(auto_revoke, { this, &MicaEditorControl::Application_Suspending });
#endif
	}

	// Todo: Focus bug: deactive window, click on control, press ctrl+a quickly. result: selection disappears

	void MicaEditorControl::OnGotFocus(RoutedEventArgs const &e)
	{
		__super::OnGotFocus(e);

		_isFocused = true;

		_scintilla->FocusChanged(true);
	}

	void MicaEditorControl::OnLostFocus(RoutedEventArgs const &e)
	{
		__super::OnLostFocus(e);

		_isFocused = false;

		if (!_isContextMenuOpen)
		{
			_scintilla->FocusChanged(false);
		}
	}

	// Todo: Combine pointer events and use PointerPointProperties.PointerUpdateKind to determine which button and state
	void MicaEditorControl::OnPointerPressed(PointerRoutedEventArgs const &e)
	{
		__super::OnPointerPressed(e);

		Focus(FocusState::Programmatic);

		if (auto imageTarget{ GetTemplateChild(L"ImageTarget").try_as<UIElement>() }) // Todo: Store this
		{
			auto point{ e.GetCurrentPoint(imageTarget) };
			auto scaled{ point.Position() };
			switch (point.Properties().PointerUpdateKind())
			{
			case DUI::PointerUpdateKind::LeftButtonPressed:
				_scintilla->PointerPressed(Point{ scaled.X * _dpiScale, scaled.Y * _dpiScale }, point.Timestamp() / 1000ul, e.KeyModifiers());
				break;
			case DUI::PointerUpdateKind::RightButtonPressed:
				_scintilla->RightPointerPressed(Point{ scaled.X * _dpiScale, scaled.Y * _dpiScale }, point.Timestamp() / 1000ul, e.KeyModifiers());
				break;
			}
			// Todo: make sure the loss of precision is not going to realistically cause a problem
		}
	}

	void MicaEditorControl::OnPointerMoved(PointerRoutedEventArgs const &e)
	{
		__super::OnPointerMoved(e);

		if (auto imageTarget{ GetTemplateChild(L"ImageTarget").try_as<UIElement>() }) // Todo: Store this
		{
			auto point{ e.GetCurrentPoint(imageTarget) };
			auto scaled{ point.Position() };
			auto x{ scaled.X * _dpiScale };
			auto y{ scaled.Y * _dpiScale };

			_scintilla->PointerMoved(Point{ x, y }, point.Timestamp() / 1000ul, e.KeyModifiers(), point);

			//auto cursor{ _scintilla->ContextCursor(Scintilla::Internal::Point{ x,y }) };
			//switch (cursor)
			//{
			//case Scintilla::Internal::Window::SetCursor invalid:
			//case text:
			//case arrow:
			//case up:
			//case wait:
			//case horizontal:
			//case vertical:
			//case reverseArrow:
			//case hand:
			//	break;
			//}
		}
	}

	void MicaEditorControl::OnPointerReleased(PointerRoutedEventArgs const &e)
	{
		__super::OnPointerReleased(e);

		if (auto imageTarget{ GetTemplateChild(L"ImageTarget").try_as<UIElement>() }) // Todo: Store this
		{
			auto point{ e.GetCurrentPoint(imageTarget) };
			auto scaled{ point.Position() };
			switch (point.Properties().PointerUpdateKind())
			{
			case DUI::PointerUpdateKind::LeftButtonReleased:
				_scintilla->PointerReleased(Point{ scaled.X * _dpiScale, scaled.Y * _dpiScale }, point.Timestamp() / 1000ul, e.KeyModifiers());
				break;
			}
		}

		e.Handled(true); // Prevents control from losing focus on pointer released
		// See https://stackoverflow.com/questions/59392044/uwp-control-to-keep-focus-after-mouse-click
		// Alternate approach: call Focus in OnFocusLost
	}

#ifndef WINUI3
	void MicaEditorControl::OnPointerCaptureLost(PointerRoutedEventArgs const &e)
	{
		if (!_isPointerOver)
		{
			// Todo: if you, e.g. hover over a HyperlinkButton when this is called, you will get an arrow instead of the hand you want
			winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread().PointerCursor(Windows::UI::Core::CoreCursor{ Windows::UI::Core::CoreCursorType::Arrow, 0 });
		}
	}

	void MicaEditorControl::OnPointerEntered(PointerRoutedEventArgs const &e)
	{
		_isPointerOver = true;
	}

	void MicaEditorControl::OnPointerExited(PointerRoutedEventArgs const &e)
	{
		_isPointerOver = false;
		if (!_wrapper->HaveMouseCapture())
		{
			winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread().PointerCursor(Windows::UI::Core::CoreCursor{ Windows::UI::Core::CoreCursorType::Arrow, 0 });
		}
	}
#endif

	void MicaEditorControl::OnKeyDown(KeyRoutedEventArgs const &e)
	{
		__super::OnKeyDown(e);

		const auto modifiers{ GetKeyModifiersForCurrentThread() };

		auto handled = true;
		_scintilla->KeyDown(e.Key(), modifiers, e.KeyStatus().IsExtendedKey, &handled); // Todo: Or use VirtualKey?

		if (!handled
			&& e.Key() == VirtualKey::F10 && (modifiers & VirtualKeyModifiers::Shift) == VirtualKeyModifiers::Shift
			&& ShowContextMenuAtCurrentPosition())
		{
			handled = true;
		}

		e.Handled(handled);
	}

	void MicaEditorControl::OnKeyUp(KeyRoutedEventArgs const &e)
	{
		if (e.Key() == VirtualKey::Application)
		{
			e.Handled(ShowContextMenuAtCurrentPosition());
		}
	}

	void MicaEditorControl::MicaEditorControl_CharacterReceived(DUX::UIElement const &sender, CharacterReceivedRoutedEventArgs const &e)
	{
		if (_isFocused)
		{
			_scintilla->CharacterReceived(e.Character());
			e.Handled(true);
		}
	}

	void MicaEditorControl::ImageTarget_SizeChanged(IInspectable const &sender, SizeChangedEventArgs const &args)
	{
		if (_vsisNative)
		{
			auto width{ ConvertFromDipToPixelUnit(args.NewSize().Width, _dpiScale) };
			auto height{ ConvertFromDipToPixelUnit(args.NewSize().Height, _dpiScale) };
			_wrapper->Width(width);
			_wrapper->Height(height);
			_vsisNative->Resize(width, height);
			_scintilla->SizeChanged();
		}
	}

	void MicaEditorControl::ImageTarget_PointerWheelChanged(IInspectable const &sender, PointerRoutedEventArgs const &e)
	{
		auto properties{ e.GetCurrentPoint(sender.as<UIElement>()).Properties() };
		_scintilla->PointerWheelChanged(properties.MouseWheelDelta(), properties.IsHorizontalMouseWheel(), e.KeyModifiers());
	}

	void MicaEditorControl::ImageTarget_DragEnter(IInspectable const &sender, DragEventArgs const &e)
	{
		DataPackageOperation op;
		_scintilla->DragEnter(e.DataView(), e.AllowedOperations(), e.Modifiers(), op);
		e.AcceptedOperation(op);
		e.DragUIOverride().IsContentVisible(false);
		e.DragUIOverride().IsCaptionVisible(false);
	}

	void MicaEditorControl::ImageTarget_DragOver(IInspectable const &sender, DragEventArgs const &e)
	{
		auto point{ e.GetPosition(sender.as<UIElement>()) };
		point.X *= _dpiScale;
		point.Y *= _dpiScale;
		DataPackageOperation op;
		_scintilla->DragOver(point, e.AllowedOperations(), e.Modifiers(), op);
		e.AcceptedOperation(op);
	}

	void MicaEditorControl::ImageTarget_DragLeave(IInspectable const &sender, DragEventArgs const &e)
	{
		_scintilla->DragLeave();
	}

	void MicaEditorControl::ImageTarget_Drop(IInspectable const &sender, DragEventArgs const &e)
	{
		auto point{ e.GetPosition(sender.as<UIElement>()) };
		point.X *= _dpiScale;
		point.Y *= _dpiScale;
		DataPackageOperation op;
		_scintilla->Drop(point, e.DataView(), e.AllowedOperations(), e.Modifiers(), op);
		e.AcceptedOperation(op);
	}

	void MicaEditorControl::ImageTarget_DragStarting(UIElement const &sender, DragStartingEventArgs const &e)
	{
		e.AllowedOperations(DataPackageOperation::Copy | DataPackageOperation::Move);
		e.Data().SetText(winrt::to_hstring(_scintilla->GetDragData()));
		e.DragUI().SetContentFromDataPackage();
	}

	void MicaEditorControl::ImageTarget_ContextRequested(UIElement const &sender, ContextRequestedEventArgs const &e)
	{
		Point point;
		if (e.TryGetPosition(sender, point))
		{
			e.Handled(ShowContextMenu(sender, point));
		}
		else if (const auto frameworkElement{ sender.try_as<FrameworkElement>() })
		{
			e.Handled(ShowContextMenuAtCurrentPosition());
		}
	}

	void MicaEditorControl::ContextMenuItem_Click(Windows::Foundation::IInspectable const &sender, DUX::RoutedEventArgs const &e)
	{
		_scintilla->WndProc(static_cast<Scintilla::Message>(unbox_value<ScintillaMessage>(sender.as<FrameworkElement>().Tag())), 0, 0);
	}

	void MicaEditorControl::HorizontalScrollBar_Scroll(IInspectable const &sender, ScrollEventArgs const &e)
	{
		_scintilla->HorizontalScroll(static_cast<Scintilla::Internal::ScrollEventType>(e.ScrollEventType()), static_cast<int>(e.NewValue()));
	}

	void MicaEditorControl::VerticalScrollBar_Scroll(IInspectable const &sender, ScrollEventArgs const &e)
	{
		_scintilla->Scroll(static_cast<Scintilla::Internal::ScrollEventType>(e.ScrollEventType()), static_cast<int>(e.NewValue()));
	}

	LRESULT MicaEditorControl::WndProc(IInspectable const &tag, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (msg == WM_NOTIFY)
		{
			const auto data{ reinterpret_cast<Scintilla::NotificationData *>(lParam) };
			const auto sender{ tag.as<MicaEditorControl>() };
			sender->_editorWrapper.as<implementation::Editor>()->ProcessEvent(data);
		}

		return 0;
	}

#ifndef WINUI3
	void MicaEditorControl::Application_Suspending(IInspectable const &sender, SuspendingEventArgs const &args)
	{
		// Required or crashes on resume
		// https://learn.microsoft.com/en-us/windows/uwp/gaming/directx-and-xaml-interop
		// https://learn.microsoft.com/en-us/windows/win32/api/dxgi1_3/nf-dxgi1_3-idxgidevice3-trim
		_scintilla->TrimGraphics();
	}
#endif
}
