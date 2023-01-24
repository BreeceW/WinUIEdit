#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"
#include "App.h"

using namespace winrt;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Streams;
using namespace MicaEditor;

namespace winrt::CppDemoUwp::implementation
{
	MainPage::MainPage()
	{
		InitializeComponent();

		SetTitle(false);

		_updateUIRevoker = Editor().Editor().UpdateUI(auto_revoke, { this, &MainPage::Editor_UpdateUI });
		_zoomChangedRevoker = Editor().Editor().ZoomChanged(auto_revoke, { this, &MainPage::Editor_ZoomChanged });
		_savePointReachedRevoker = Editor().Editor().SavePointReached(auto_revoke, { this, &MainPage::Editor_SavePointReached });
		_savePointLeftRevoker = Editor().Editor().SavePointLeft(auto_revoke, { this, &MainPage::Editor_SavePointLeft });

		auto coreWindow{ CoreWindow::GetForCurrentThread() };
		_activatedRevoker = coreWindow.Activated(auto_revoke, { this, &MainPage::OnActivated });
		if (_hasFcu)
		{
			Activated(coreWindow.ActivationMode() != CoreWindowActivationMode::Deactivated);
		}
	}

	void MainPage::OnActivated(CoreWindow const &sender, WindowActivatedEventArgs const &e)
	{
		Activated(e.WindowActivationState() != CoreWindowActivationState::Deactivated);
	}

	void MainPage::Activated(bool active)
	{
		TitleText().Opacity(active ? 1 : 0.4);
	}

	/*
	 * The Editor.Scintilla method is a temporary API that exposes Scintilla's window message-based API to WinRT.
	 * The plan is to expose Scintilla's API in a manner more suitable for WinRT (properties, methods, and events instead of messages).
	 * It may be beneficial to leave the window message API available to allow porting win32 code to UWP/WinUI more easily.
	 */

	IAsyncAction MainPage::OpenMenuItem_Click(IInspectable const &sender, RoutedEventArgs const &e)
	{
		const FileOpenPicker picker{};
		picker.FileTypeFilter().Append(L".txt");
		picker.FileTypeFilter().Append(L"*");

		const auto file{ co_await picker.PickSingleFileAsync() };
		if (file)
		{
			co_await OpenAsync(file);
		}
	}

	IAsyncAction MainPage::SaveMenuItem_Click(IInspectable const &sender, RoutedEventArgs const &e)
	{
		if (_activeFile)
		{
			co_await SaveAsync(_activeFile);
		}
		else
		{
			co_await SaveAsAsync();
		}
	}

	IAsyncAction MainPage::SaveAsMenuItem_Click(IInspectable const &sender, RoutedEventArgs const &e)
	{
		co_await SaveAsAsync();
	}

	IAsyncAction MainPage::NewWindowMenuItem_Click(IInspectable const &sender, RoutedEventArgs const &e)
	{
		co_await Application::Current().as<App>()->NewWindowAsync();
	}

	void MainPage::ZoomInMenuItem_Click(IInspectable const &sender, RoutedEventArgs const &e)
	{
		Editor().Editor().ZoomIn();
	}

	void MainPage::ZoomOutMenuItem_Click(IInspectable const &sender, RoutedEventArgs const &e)
	{
		Editor().Editor().ZoomOut();
	}

	void MainPage::ZoomRestoreMenuItem_Click(IInspectable const &sender, RoutedEventArgs const &e)
	{
		Editor().Editor().Zoom(0);
	}

	void MainPage::StatusBarMenuItem_Click(IInspectable const &sender, RoutedEventArgs const &e)
	{
		StatusBar().Visibility(sender.as<ToggleMenuFlyoutItem>().IsChecked() ? Visibility::Visible : Visibility::Collapsed);
	}

	void MainPage::WordWrapMenuItem_Click(IInspectable const &sender, RoutedEventArgs const &e)
	{
		Editor().Editor().WrapMode(sender.as<ToggleMenuFlyoutItem>().IsChecked() ? Wrap::Word : Wrap::None);
	}

	void MainPage::OnNavigatedTo(NavigationEventArgs const &e)
	{
		if (const auto file{ e.Parameter().try_as<StorageFile>() })
		{
			OpenAsync(file);
		}
	}

	IAsyncAction MainPage::OpenAsync(StorageFile file)
	{
		SetOpenFile(file);
		const auto stream{ co_await file.OpenReadAsync() };
		if (stream.Size() >= (std::numeric_limits<uint32_t>::max)())
		{
			co_return;
		}
		const Buffer string{ static_cast<uint32_t>(stream.Size() + 1) };
		co_await stream.ReadAsync(string, stream.Size(), InputStreamOptions::None);
		string.data()[stream.Size()] = '\0';
		stream.Close();
		Editor().Editor().Allocate(string.Length() + 1000);
		Editor().Editor().SetTextFromBuffer(string);
		Editor().Editor().EmptyUndoBuffer();
	}

	IAsyncAction MainPage::SaveAsync(StorageFile file)
	{
		const auto length{ Editor().Editor().Length() };
		const Buffer buffer{ static_cast<uint32_t>(length + 1) };
		buffer.Length(length);
		Editor().Editor().GetTextWriteBuffer(length, buffer);
		co_await FileIO::WriteBufferAsync(file, buffer);
		Editor().Editor().SetSavePoint();
	}

	IAsyncAction MainPage::SaveAsAsync()
	{
		const FileSavePicker savePicker{};
		savePicker.FileTypeChoices().Insert(L"Text documents", single_threaded_vector<hstring>({ L".txt" }));
		savePicker.FileTypeChoices().Insert(L"All files", single_threaded_vector<hstring>({ L"." }));
		const auto file{ co_await savePicker.PickSaveFileAsync() };
		if (file)
		{
			SetOpenFile(file);
			co_await SaveAsync(file);
		}
	}

	void MainPage::SetOpenFile(StorageFile const &file)
	{
		_activeFile = file;
		SetTitle(false);
	}

	void MainPage::SetTitle(bool modified)
	{
		const auto title{ (modified ? L"*" : L"") + (_activeFile ? _activeFile.Name() : L"Untitled") };
		TitleText().Text(title + L" - Mica Editor");
		ApplicationView::GetForCurrentView().Title(title);
	}

	void MainPage::Editor_UpdateUI(MicaEditor::Editor const &sender, UpdateUIEventArgs const &args)
	{
		if (static_cast<Update>(args.Updated()) == Update::Selection)
		{
			const auto pos{ sender.CurrentPos() };
			const auto ln{ sender.LineFromPosition(pos) };
			const auto col{ sender.GetColumn(pos) };
			RCIndicator().Text(std::format(L"Ln {}, Col {}", ln + 1, col + 1));
		}
	}

	void MainPage::Editor_ZoomChanged(MicaEditor::Editor const &sender, ZoomChangedEventArgs const &args)
	{
		const auto size{ sender.StyleGetSizeFractional(static_cast<int32_t>(StylesCommon::Default)) };
		ZoomStatus().Text(to_hstring(static_cast<uint16_t>(std::round((size + sender.Zoom() * 100) * 100.0 / size))) + L"%");
	}

	void MainPage::Editor_SavePointReached(MicaEditor::Editor const &sender, SavePointReachedEventArgs const &args)
	{
		SetTitle(false);
	}

	void MainPage::Editor_SavePointLeft(MicaEditor::Editor const &sender, SavePointLeftEventArgs const &args)
	{
		SetTitle(true);
	}
}
