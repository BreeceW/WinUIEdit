#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

using namespace winrt;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Media;

namespace winrt::CppDemoWinUI3::implementation
{
	MainWindow::MainWindow()
	{
		InitializeComponent();

		Title(L"Demo WinUI 3 (C++)");

		SystemBackdrop(MicaBackdrop{});

		Editor().Editor().CallTipSetPosition(true);
		Editor().Editor().MouseDwellTime(500);
		Editor().Editor().DwellStart([](winrt::WinUIEditor::Editor const &sender, winrt::WinUIEditor::DwellStartEventArgs const &args)
			{
				const auto pos{ args.Position() };
				if (pos == -1)
				{
					return;
				}
				const auto start{ sender.WordStartPosition(pos, true) };
				const auto end{ sender.WordEndPosition(pos, true) };
				if (end - start <= 0)
				{
					return;
				}
				if (sender.CallTipActive())
				{
					const auto callTipPos{ sender.CallTipPosStart() };
					if (callTipPos >= start && callTipPos <= end)
					{
						return;
					}
				}
				using Position = intptr_t;
				struct CharacterRangeFull
				{
					Position cpMin;
					Position cpMax;
				};
				struct TextRangeFull
				{
					CharacterRangeFull chrg;
					char *lpstrText;
				};
				std::string s(end - start, '\0');
				TextRangeFull tr{ { start, end }, &s[0] };
				sender.GetTextRangeFull(reinterpret_cast<uint64_t>(&tr));
				sender.CallTipShow(pos, L"Hovering " + to_hstring(s));
				sender.CallTipSetPosStart(pos);
				sender.CallTipSetHlt(9, 9 + end - start);
				sender.CallTipSetForeHlt(sender.StyleGetFore(sender.GetStyleAt(start)));
			});
	}

	void MainWindow::CodeEditorControl_Loaded(IInspectable const &sender, RoutedEventArgs const &e)
	{
		const auto target{ Editor().try_as<IControlProtected>().GetTemplateChild(L"EditorContainer").as<ContentPresenter>().Content().try_as<IControlProtected>().GetTemplateChild(L"ImageTarget").as<FrameworkElement>() };
		target.PointerMoved([&](IInspectable const &sender, winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const &e)
			{
				const auto editor{ Editor().Editor() };

				if (editor.CallTipActive())
				{
					const auto s{ sender.as<FrameworkElement>() };
					auto point{ e.GetCurrentPoint(s) };
					auto scaled{ point.Position() };
					const auto dpiScale{ s.XamlRoot().RasterizationScale() };
					auto x{ scaled.X * dpiScale };
					auto y{ scaled.Y * dpiScale };
					const auto pos{ editor.PositionFromPoint(x, y) };

					const auto callTipPos{ editor.CallTipPosStart() };
					if (pos == -1 || (pos < editor.WordStartPosition(callTipPos, true) || pos > editor.WordEndPosition(callTipPos, true)))
					{
						editor.CallTipCancel();
					}
				}
			});

		// Needs to set focus explicitly due to WinUI 3 regression https://github.com/microsoft/microsoft-ui-xaml/issues/8816 
		sender.as<Control>().Focus(FocusState::Programmatic);
	}
}
