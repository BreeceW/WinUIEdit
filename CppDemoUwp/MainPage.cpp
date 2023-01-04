#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"
#include "App.h"

using namespace winrt;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Streams;

namespace winrt::CppDemoUwp::implementation
{
	MainPage::MainPage()
	{
		InitializeComponent();

		auto coreWindow{ CoreWindow::GetForCurrentThread() };
		_activatedRevoker = coreWindow.Activated(winrt::auto_revoke, { this, &MainPage::OnActivated });
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
			_activeFile = file;
			const auto buffer{ co_await FileIO::ReadBufferAsync(file) };
			Editor().Scintilla(/*SCI_SETTEXT*/ 2181, 0, reinterpret_cast<uint64_t>(buffer.data()));
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

	IAsyncAction MainPage::SaveAsAsync()
	{
		const FileSavePicker savePicker{};
		savePicker.FileTypeChoices().Insert(L"Text documents", single_threaded_vector<hstring>({ L".txt" }));
		savePicker.FileTypeChoices().Insert(L"All files", single_threaded_vector<hstring>({ L"." }));
		const auto file{ co_await savePicker.PickSaveFileAsync() };
		if (file)
		{
			_activeFile = file;
			co_await SaveAsync(file);
		}
	}

	IAsyncAction MainPage::SaveAsync(StorageFile file)
	{
		const auto length{ Editor().Scintilla(/*SCI_GETLENGTH*/ 2006, 0, 0) };
		const Buffer buffer{ static_cast<uint32_t>(length + 1) };
		buffer.Length(length);
		Editor().Scintilla(/*SCI_GETTEXT*/ 2182, length, reinterpret_cast<uint64_t>(buffer.data()));
		Editor().Scintilla(/*SCI_SETSAVEPOINT*/ 2014, 0, 0);
		co_await FileIO::WriteBufferAsync(file, buffer);
	}
}
