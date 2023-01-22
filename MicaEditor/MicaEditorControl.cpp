#include "pch.h"
#include "MicaEditorControl.h"
#if __has_include("MicaEditorControl.g.cpp")
#include "MicaEditorControl.g.cpp"
#endif
#include "EditorWrapper.h"

using namespace ::MicaEditor;
using namespace winrt;
using namespace DUX;
using namespace DUX::Controls;
using namespace DUX::Controls::Primitives;
using namespace DUX::Input;
using namespace DUX::Media;
using namespace DUX::Media::Imaging;
using namespace Windows::Foundation;
using namespace Windows::System;
using namespace Windows::Graphics::Display;
using namespace Windows::ApplicationModel;

namespace winrt::MicaEditor::implementation
{
	// Todo: Something about this control is keeping the WinUI 3 versions from closing.
	// Note that making this into a blank control fixes the issue, so it is definitely something here.
	MicaEditorControl::MicaEditorControl()
	{
		DefaultStyleKey(winrt::box_value(L"MicaEditor.MicaEditorControl"));

		_wrapper = std::make_shared<Wrapper>();

		Unloaded({ this, &MicaEditorControl::OnUnloaded });

#ifndef WINUI3
		auto displayInformation{ DisplayInformation::GetForCurrentView() };
		_dpiChangedRevoker = displayInformation.DpiChanged(winrt::auto_revoke, { this, &MicaEditorControl::OnDpiChanged });
		UpdateDisplayInformation(displayInformation.RawPixelsPerViewPixel(), displayInformation.LogicalDpi());
#endif

		_scintilla = make_self<::Scintilla::Internal::ScintillaWinUI>();

		_editorWrapper = winrt::make<implementation::Editor>(_scintilla);

		_scintilla->SetWndProcTag(*this);
		_scintilla->SetWndProc(&MicaEditorControl::WndProc);

		// Scintilla has built-in shortcuts for zoom in and out, but they only work with the numpad plus/minus
		_scintilla->WndProc(Scintilla::Message::AssignCmdKey, 187 + (SCMOD_CTRL << 16), SCI_ZOOMIN); // Ctrl+Plus
		_scintilla->WndProc(Scintilla::Message::AssignCmdKey, 189 + (SCMOD_CTRL << 16), SCI_ZOOMOUT); // Ctrl+Minus
		_scintilla->WndProc(Scintilla::Message::AssignCmdKey, 48 + (SCMOD_CTRL << 16), SCI_SETZOOM); // Ctrl+0

		_scintilla->WndProc(Scintilla::Message::InsertText, 0, reinterpret_cast<Scintilla::uptr_t>(
			"// n! where n is greater than or equal to 0\r\n"
			"int Factorial(int n)\r\n"
			"{\r\n"
			"    if (n <= 1)\r\n"
			"    {\r\n"
			"        return 1;\r\n"
			"    }\r\n"
			"    else\r\n"
			"    {\r\n"
			"        return n * Factorial(n - 1);\r\n"
			"    }\r\n"
			"}\r\n"));
		/*for (int i = 1; i <= 10000000; i++)
		{
			char buf[50];
			sprintf(buf, "Test line %d\r\n", i);
			_scintilla->WndProc(Scintilla::Message::AppendText, strlen(buf), reinterpret_cast<Scintilla::uptr_t>(buf));
		}*/
		_scintilla->WndProc(Scintilla::Message::SetMultipleSelection, true, 0);
		_scintilla->WndProc(Scintilla::Message::SetScrollWidth, 2000 * _dpiScale, 0); // Todo: Update on scale (careful not to override measured)
		_scintilla->WndProc(Scintilla::Message::SetScrollWidthTracking, true, 0);
		_scintilla->WndProc(Scintilla::Message::SetXCaretPolicy, CARET_SLOP, 20 * _dpiScale); // Todo: Update on scale (should this be scaled?)
		_scintilla->WndProc(Scintilla::Message::SetYCaretPolicy, CARET_SLOP | CARET_STRICT | CARET_EVEN, 1);
		_scintilla->WndProc(Scintilla::Message::SetVisiblePolicy, VISIBLE_SLOP, 0);
		_scintilla->WndProc(Scintilla::Message::SetHScrollBar, true, 0);
		_scintilla->WndProc(Scintilla::Message::SetEndAtLastLine, false, 0);
		_scintilla->WndProc(Scintilla::Message::SetTabWidth, 4, 0);
		//_scintilla->WndProc(Scintilla::Message::SetWrapMode, SC_WRAP_WHITESPACE, 0);
		_scintilla->WndProc(Scintilla::Message::SetMarginTypeN, 1, SC_MARGIN_NUMBER);

		_scintilla->WndProc(Scintilla::Message::SetILexer, 0, reinterpret_cast<Scintilla::uptr_t>(CreateLexer("cpp")));
		// This list of keywords from SciTe (cpp.properties)
		_scintilla->WndProc(Scintilla::Message::SetKeyWords, 0, reinterpret_cast<Scintilla::uptr_t>(
			"alignas alignof and and_eq asm audit auto axiom bitand bitor bool "
			"char char8_t char16_t char32_t class compl concept "
			"const consteval constexpr const_cast "
			"decltype default delete double dynamic_cast enum explicit export extern false final float "
			"friend import inline int long module mutable namespace new noexcept not not_eq nullptr "
			"operator or or_eq override private protected public "
			"register reinterpret_cast requires "
			"short signed sizeof static static_assert static_cast struct "
			"template this thread_local true typedef typeid typename union unsigned using "
			"virtual void volatile wchar_t xor xor_eq"));
		_scintilla->WndProc(Scintilla::Message::SetKeyWords, 1, reinterpret_cast<Scintilla::uptr_t>(
			"break case catch co_await co_return co_yield continue do else for goto if return switch throw try while "));

		// Use the new ActualTheme property on Fall Creators Update and above. On WinUI 3, this is always present, so the check is not needed
#ifndef WINUI3
		if (_hasFcu)
		{
#endif
			ActualThemeChanged({ this, &MicaEditorControl::OnActualThemeChanged });
			UpdateColors(ActualTheme() == ElementTheme::Dark);
#ifndef WINUI3
		}
		else
		{
			_colorValuesChangedRevoker = _uiSettings.ColorValuesChanged(winrt::auto_revoke, { this, &MicaEditorControl::OnColorValuesChanged });
			UpdateColors(UseDarkColors());
		}
#endif
	}

	hstring MicaEditorControl::Text()
	{
		return GetValue(s_textProperty).as<hstring>();
	}

	void MicaEditorControl::Text(hstring const &value)
	{
		SetValue(s_textProperty, box_value(value));
	}

	MicaEditor::Editor MicaEditorControl::Editor()
	{
		return _editorWrapper;
	}

	void MicaEditorControl::OnUnloaded(IInspectable const &sender, DUX::RoutedEventArgs const &args)
	{
		_scintilla->Finalize();
		// Todo: Finish Finalize method and figure out what else needs to get cleaned up
	}

	void MicaEditorControl::OnDpiChanged(DisplayInformation const &sender, IInspectable const &args)
	{
		UpdateDisplayInformation(sender.RawPixelsPerViewPixel(), sender.LogicalDpi());
		_scintilla->DpiChanged();
	}

	void MicaEditorControl::OnActualThemeChanged(FrameworkElement const &sender, IInspectable const &args)
	{
		UpdateColors(ActualTheme() == ElementTheme::Dark);
	}

#ifndef WINUI3
	void MicaEditorControl::OnColorValuesChanged(Windows::UI::ViewManagement::UISettings const &uiSettings, IInspectable const &args)
	{
		// Todo: Not sure if there is a way to detect if the RequestedTheme changes in Creators Update. Unimportant
		Dispatcher().RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, [&]()
			{
				UpdateColors(UseDarkColors());
			});
	}

	bool MicaEditorControl::UseDarkColors()
	{
		auto theme{ RequestedTheme() };
		return theme == ElementTheme::Default ? Application::Current().RequestedTheme() == ApplicationTheme::Dark : theme == ElementTheme::Dark;
	}
#endif

	void MicaEditorControl::UpdateDisplayInformation(float dpiScale, float logicalDpi)
	{
		_dpiScale = dpiScale;
		_logicalDpi = logicalDpi;
		UpdateSizes();
	}

	void MicaEditorControl::UpdateColors(bool useDarkTheme)
	{
		// This is just a random color that our backend will treat as transparent
		// The Scintilla API doesn't let us set alpha on back color, so this is easier
		// Users of this control are meant to control the background through XAML
		// This only affects rectangles. Text can use this color
		// Todo: Come up with a better solution
		constexpr sptr_t transparencyColor{ -7791875 };
		// These colors mostly adapted from https://github.com/microsoft/vscode/blob/main/extensions/theme-defaults/themes/light_plus.json
		if (useDarkTheme)
		{
			for (uptr_t i{ SCE_C_DEFAULT }; i < SCE_C_ESCAPESEQUENCE; i++) // Todo: see if there is better way
			{
				_scintilla->WndProc(Scintilla::Message::StyleSetFore, i, RGB(0xD4, 0xD4, 0xD4));
				_scintilla->WndProc(Scintilla::Message::StyleSetBack, i, transparencyColor);

				_scintilla->WndProc(Scintilla::Message::StyleSetFore, i + 64, RGB(167, 167, 167)); // Inactive states
				_scintilla->WndProc(Scintilla::Message::StyleSetBack, i + 64, transparencyColor);
			}

			//_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_IDENTIFIER, RGB(0x9C, 0xDC, 0xFE));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_NUMBER, RGB(0xB5, 0xCE, 0xA8));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_WORD, RGB(0x56, 0x9C, 0xD6));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_WORD2, RGB(0xC5, 0x86, 0xC0));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_STRING, RGB(0xCE, 0x91, 0x78));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_OPERATOR, RGB(0xD4, 0xD4, 0xD4));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_PREPROCESSOR, RGB(0x9B, 0x9B, 0x9B));
			//_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_ERROR, RGB(0xcd, 0x31, 0x31));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_COMMENT, RGB(0x6A, 0x99, 0x55));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_COMMENTLINE, RGB(0x6A, 0x99, 0x55));

			_scintilla->WndProc(Scintilla::Message::SetSelBack, true, RGB(0x26, 0x4F, 0x78));
			_scintilla->WndProc(Scintilla::Message::StyleSetBack, STYLE_DEFAULT, transparencyColor);
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, STYLE_LINENUMBER, RGB(0x85, 0x85, 0x85));
			_scintilla->WndProc(Scintilla::Message::StyleSetBack, STYLE_LINENUMBER, transparencyColor);
			_scintilla->WndProc(Scintilla::Message::SetCaretFore, RGB(0xAE, 0xAF, 0xAD), 0);
		}
		else
		{
			for (uptr_t i{ SCE_C_DEFAULT }; i < SCE_C_ESCAPESEQUENCE; i++) // Todo: see if there is better way
			{
				_scintilla->WndProc(Scintilla::Message::StyleSetFore, i, RGB(0x00, 0x00, 0x00));
				_scintilla->WndProc(Scintilla::Message::StyleSetBack, i, transparencyColor);

				_scintilla->WndProc(Scintilla::Message::StyleSetFore, i + 64, RGB(185, 185, 185)); // Inactive states
				_scintilla->WndProc(Scintilla::Message::StyleSetBack, i + 64, transparencyColor);
			}

			//_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_IDENTIFIER, RGB(0x00, 0x10, 0x80));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_NUMBER, RGB(0x09, 0x86, 0x58));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_WORD, RGB(0x00, 0x00, 0xFF));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_WORD2, RGB(0xaf, 0x00, 0xdb));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_STRING, RGB(0xa3, 0x15, 0x15));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_OPERATOR, RGB(0x00, 0x00, 0x00));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_PREPROCESSOR, RGB(0x80, 0x80, 0x80));
			//_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_ERROR, RGB(0xcd, 0x31, 0x31));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_COMMENT, RGB(0x00, 0x80, 0x00));
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, SCE_C_COMMENTLINE, RGB(0x00, 0x80, 0x00));

			_scintilla->WndProc(Scintilla::Message::SetSelBack, true, RGB(0xAD, 0xD6, 0xFF));
			_scintilla->WndProc(Scintilla::Message::StyleSetBack, STYLE_DEFAULT, transparencyColor);
			for (uptr_t i{ SCE_B_DEFAULT }; i < SCE_B_DOCKEYWORD; i++) // Todo: see if there is better way
			{
				_scintilla->WndProc(Scintilla::Message::StyleSetBack, i, transparencyColor);
			}
			_scintilla->WndProc(Scintilla::Message::StyleSetFore, STYLE_LINENUMBER, RGB(0x23, 0x78, 0x93));
			_scintilla->WndProc(Scintilla::Message::StyleSetBack, STYLE_LINENUMBER, transparencyColor);
			_scintilla->WndProc(Scintilla::Message::SetCaretFore, RGB(0x00, 0x00, 0x00), 0);
		}
	}

	void MicaEditorControl::UpdateSizes()
	{
		_wrapper->DpiScale(_dpiScale);
		_wrapper->LogicalDpi(_logicalDpi);
		if (_scintilla)
		{
			_scintilla->WndProc(Scintilla::Message::SetMarginWidthN, 1, Helpers::ConvertFromDipToPixelUnit(45, _dpiScale));
			_scintilla->WndProc(Scintilla::Message::SetMarginLeft, 0, Helpers::ConvertFromDipToPixelUnit(23, _dpiScale));
			_scintilla->WndProc(Scintilla::Message::SetCaretWidth, Helpers::ConvertFromDipToPixelUnit(_uiSettings.CaretWidth(), _dpiScale), 0); // Todo: Needs to stop blinking after timeout and respect blink rate
		}
	}

	uint64_t MicaEditorControl::Scintilla(ScintillaMessage const &message, uint64_t wParam, uint64_t lParam)
	{
		return _scintilla->WndProc(static_cast<Scintilla::Message>(message), wParam, lParam);
	}

	void MicaEditorControl::OnTextPropertyChanged(IInspectable const &sender, DUX::DependencyPropertyChangedEventArgs const &args)
	{
		auto scintilla{ sender.as<MicaEditorControl>()->_scintilla };
		if (scintilla)
		{
			auto str{ args.NewValue().as<hstring>() };
			scintilla->SetText(str);
		}
	}

	void MicaEditorControl::OnApplyTemplate()
	{
#ifdef WINUI3
		// Temporary until it is known how to respond to DPI changes with WASDK
		// Todo: Can the UWP version of this code go down here also?
		auto scale{ XamlRoot().RasterizationScale() };
		UpdateDisplayInformation(scale, scale * 96);
#endif
		UpdateSizes();

		uint32_t creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1
		};

		// Create the Direct3D device.
		winrt::com_ptr<::ID3D11Device> d3dDevice;
		D3D_FEATURE_LEVEL supportedFeatureLevel;
		winrt::check_hresult(::D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			0,
			creationFlags,
			featureLevels,
			ARRAYSIZE(featureLevels),
			D3D11_SDK_VERSION,
			d3dDevice.put(),
			&supportedFeatureLevel,
			nullptr)
		);

		// Get the Direct3D device.
		_dxgiDevice = d3dDevice.as<::IDXGIDevice3>();

		// Create the Direct2D device and a corresponding context.
		winrt::com_ptr<::ID2D1Device> d2dDevice;
		::D2D1CreateDevice(_dxgiDevice.get(), nullptr, d2dDevice.put());

		winrt::com_ptr<::ID2D1DeviceContext> d2dDeviceContext;
		winrt::check_hresult(
			d2dDevice->CreateDeviceContext(
				D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
				d2dDeviceContext.put()
			)
		);

		auto width{ Helpers::ConvertFromDipToPixelUnit(ActualWidth(), _dpiScale) };
		auto height{ Helpers::ConvertFromDipToPixelUnit(ActualHeight(), _dpiScale) };
		_wrapper->Width(width); // Todo: Is this zero at this point?
		_wrapper->Height(height);
		VirtualSurfaceImageSource virtualSurfaceImageSource{ width, height };

		winrt::com_ptr<::ISurfaceImageSourceNativeWithD2D> sisNativeWithD2D{
			virtualSurfaceImageSource.as<::ISurfaceImageSourceNativeWithD2D>() };

		// Associate the Direct2D device with the SurfaceImageSource.
		sisNativeWithD2D->SetDevice(d2dDevice.get());

		// To enable multi-threaded access (optional)
		winrt::com_ptr<::ID3D11Multithread> d3dMultiThread{
			d3dDevice.as<::ID3D11Multithread>() };
		d3dMultiThread->SetMultithreadProtected(true);

		_vsisNative = virtualSurfaceImageSource.as<::IVirtualSurfaceImageSourceNative>();

		_wrapper->VsisNative(_vsisNative);
		_scintilla->RegisterGraphics(sisNativeWithD2D, _vsisNative, d2dDeviceContext, _wrapper);
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

			_wrapper->SetMouseCaptureElement(imageTarget);

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

		_scintilla->FocusChanged(true);
	}

	void MicaEditorControl::OnLostFocus(RoutedEventArgs const &e)
	{
		__super::OnLostFocus(e);

		_scintilla->FocusChanged(false);
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

			_scintilla->PointerMoved(Point{ x, y }, point.Timestamp() / 1000ul, e.KeyModifiers());

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
		if (!e.GetCurrentPoint(nullptr).Properties().IsLeftButtonPressed())
		{
			winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread().PointerCursor(Windows::UI::Core::CoreCursor{ Windows::UI::Core::CoreCursorType::Arrow, 0 });
		}
	}
#endif

	void MicaEditorControl::OnKeyDown(KeyRoutedEventArgs const &e)
	{
		__super::OnKeyDown(e);

		auto modifiers{ VirtualKeyModifiers::None };
#ifdef WINUI3
		// Todo: Do we need to check Locked?
		// Todo: Left vs right keys?
		//  does not help us
		if ((Microsoft::UI::Input::InputKeyboardSource::GetKeyStateForCurrentThread(VirtualKey::Shift) & Windows::UI::Core::CoreVirtualKeyStates::Down) == Windows::UI::Core::CoreVirtualKeyStates::Down)
		{
			modifiers |= VirtualKeyModifiers::Shift;
		}
		if ((Microsoft::UI::Input::InputKeyboardSource::GetKeyStateForCurrentThread(VirtualKey::Control) & Windows::UI::Core::CoreVirtualKeyStates::Down) == Windows::UI::Core::CoreVirtualKeyStates::Down)
		{
			modifiers |= VirtualKeyModifiers::Control;
		}
		if ((Microsoft::UI::Input::InputKeyboardSource::GetKeyStateForCurrentThread(VirtualKey::Menu) & Windows::UI::Core::CoreVirtualKeyStates::Down) == Windows::UI::Core::CoreVirtualKeyStates::Down)
		{
			modifiers |= VirtualKeyModifiers::Menu; // Todo: KeyStatus.IsMenuKeyDown?
		}
		if ((Microsoft::UI::Input::InputKeyboardSource::GetKeyStateForCurrentThread(VirtualKey::LeftWindows) & Windows::UI::Core::CoreVirtualKeyStates::Down) == Windows::UI::Core::CoreVirtualKeyStates::Down
			|| (Microsoft::UI::Input::InputKeyboardSource::GetKeyStateForCurrentThread(VirtualKey::RightWindows) & Windows::UI::Core::CoreVirtualKeyStates::Down) == Windows::UI::Core::CoreVirtualKeyStates::Down)
		{
			modifiers |= VirtualKeyModifiers::Windows;
		}
#else
		auto window{ Windows::UI::Core::CoreWindow::GetForCurrentThread() }; // Todo: is it worth it to store this?
		// Todo: Do we need to check Locked?
		// Todo: Left vs right keys?
		if ((window.GetKeyState(VirtualKey::Shift) & Windows::UI::Core::CoreVirtualKeyStates::Down) == Windows::UI::Core::CoreVirtualKeyStates::Down)
		{
			modifiers |= VirtualKeyModifiers::Shift;
		}
		if ((window.GetKeyState(VirtualKey::Control) & Windows::UI::Core::CoreVirtualKeyStates::Down) == Windows::UI::Core::CoreVirtualKeyStates::Down)
		{
			modifiers |= VirtualKeyModifiers::Control;
		}
		if ((window.GetKeyState(VirtualKey::Menu) & Windows::UI::Core::CoreVirtualKeyStates::Down) == Windows::UI::Core::CoreVirtualKeyStates::Down)
		{
			modifiers |= VirtualKeyModifiers::Menu; // Todo: KeyStatus.IsMenuKeyDown?
		}
		if ((window.GetKeyState(VirtualKey::LeftWindows) & Windows::UI::Core::CoreVirtualKeyStates::Down) == Windows::UI::Core::CoreVirtualKeyStates::Down
			|| (window.GetKeyState(VirtualKey::RightWindows) & Windows::UI::Core::CoreVirtualKeyStates::Down) == Windows::UI::Core::CoreVirtualKeyStates::Down)
		{
			modifiers |= VirtualKeyModifiers::Windows;
		}
#endif

		if ((e.Key() == VirtualKey::Tab || e.Key() == VirtualKey::Enter) && (modifiers == winrt::Windows::System::VirtualKeyModifiers::None || modifiers == winrt::Windows::System::VirtualKeyModifiers::Shift))
		{
			e.Handled(true);
			// Handle tab to prevent tab navigation
			// Todo: do we want to do this if document is read-only?
			// Handle enter due to system XAML bug in older Windows versions that fired enter key specifically twice
		}

		_scintilla->KeyDown(e.Key(), modifiers); // Todo: Or use VirtualKey?
	}

	void MicaEditorControl::OnCharacterReceived(CharacterReceivedRoutedEventArgs const &e)
	{
		// Todo: This is 1709. What happens on 1703? Answer: Nothing, which is fine, since it is UWP-only anyway
		// 1703 might be fine with TSF only. If not, CoreWindow has a CharacterReceived and you can probably just check focus
		// Todo: Queue these?
		// It seems like this doesn't fire on UWP
		_scintilla->CharacterReceived(e.Character());
	}

	void MicaEditorControl::ImageTarget_SizeChanged(IInspectable const &sender, SizeChangedEventArgs const &args)
	{
		if (_vsisNative)
		{
			auto width{ Helpers::ConvertFromDipToPixelUnit(args.NewSize().Width, _dpiScale) };
			auto height{ Helpers::ConvertFromDipToPixelUnit(args.NewSize().Height, _dpiScale) };
			_wrapper->Width(width);
			_wrapper->Height(height);
			_vsisNative->Resize(width, height);
			_scintilla->SizeChanged();
		}
	}

	void MicaEditorControl::ImageTarget_PointerWheelChanged(Windows::Foundation::IInspectable const &sender, DUX::Input::PointerRoutedEventArgs const &e)
	{
		auto properties{ e.GetCurrentPoint(sender.as<UIElement>()).Properties() };
		_scintilla->PointerWheelChanged(properties.MouseWheelDelta(), properties.IsHorizontalMouseWheel(), e.KeyModifiers());
	}

	void MicaEditorControl::HorizontalScrollBar_Scroll(IInspectable const &sender, ScrollEventArgs const &e)
	{
		_scintilla->HorizontalScroll(static_cast<Scintilla::Internal::ScrollEventType>(e.ScrollEventType()), static_cast<int>(e.NewValue()));
	}

	void MicaEditorControl::VerticalScrollBar_Scroll(IInspectable const &sender, ScrollEventArgs const &e)
	{
		_scintilla->Scroll(static_cast<Scintilla::Internal::ScrollEventType>(e.ScrollEventType()), static_cast<int>(e.NewValue()));
	}

	LRESULT MicaEditorControl::WndProc(Windows::Foundation::IInspectable const &tag, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (msg == WM_NOTIFY)
		{
			const auto data{ reinterpret_cast<Scintilla::NotificationData *>(lParam) };
			const auto sender{ tag.as<MicaEditorControl>() };
			get_self<implementation::Editor>(sender->_editorWrapper)->ProcessEvent(data);
			}

		return 0;
	}

#ifndef WINUI3
	void MicaEditorControl::Application_Suspending(IInspectable const &sender, SuspendingEventArgs const &args)
	{
		// Required or crashes on resume
		// https://learn.microsoft.com/en-us/windows/uwp/gaming/directx-and-xaml-interop
		// https://learn.microsoft.com/en-us/windows/win32/api/dxgi1_3/nf-dxgi1_3-idxgidevice3-trim
		_dxgiDevice->Trim();
	}
#endif
}
