#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"
#include "App.h"

using namespace winrt;
using namespace Windows::System;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::UI::Core;
using namespace Windows::UI::Core::Preview;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
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

		_closeRequestedRevoker = SystemNavigationManagerPreview::GetForCurrentView().CloseRequested(auto_revoke, { this, &MainPage::OnCloseRequested });

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

	IAsyncAction MainPage::OnCloseRequested(IInspectable sender, SystemNavigationCloseRequestedPreviewEventArgs e)
	{
		if (Editor().Editor().Modify())
		{
			const auto def{ e.GetDeferral() };

			switch (co_await PromptSaveAsync())
			{
			case ContentDialogResult::None:
				e.Handled(true);
				break;

			case ContentDialogResult::Primary:
				if (!co_await SaveAsync())
				{
					e.Handled(true);
				}
				break;
			}

			def.Complete();
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

	void MainPage::Editor_Loaded(IInspectable const &sender, RoutedEventArgs const &e)
	{
		FocusEditor();
	}

	/*
	 * The Editor.Scintilla method is a temporary API that exposes Scintilla's window message-based API to WinRT.
	 * The plan is to expose Scintilla's API in a manner more suitable for WinRT (properties, methods, and events instead of messages).
	 * It may be beneficial to leave the window message API available to allow porting win32 code to UWP/WinUI more easily.
	 */

	IAsyncAction MainPage::OpenMenuItem_Click(IInspectable const &sender, RoutedEventArgs const &e)
	{
		if (Editor().Editor().Modify())
		{
			switch (co_await PromptSaveAsync())
			{
			case ContentDialogResult::None:
				FocusEditor();
				co_return;

			case ContentDialogResult::Primary:
				if (!co_await SaveAsync())
				{
					FocusEditor();
					co_return;
				}
				break;
			}
		}

		FocusEditor();

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
		FocusEditor();

		co_await SaveAsync();
	}

	IAsyncAction MainPage::SaveAsMenuItem_Click(IInspectable const &sender, RoutedEventArgs const &e)
	{
		FocusEditor();

		co_await SaveAsAsync();
	}

	IAsyncAction MainPage::NewWindowMenuItem_Click(IInspectable const &sender, RoutedEventArgs const &e)
	{
		FocusEditor();

		co_await Application::Current().as<App>()->NewWindowAsync();
	}

	IAsyncAction MainPage::NewMenuItem_Click(IInspectable const &sender, RoutedEventArgs const &e)
	{
		if (Editor().Editor().Modify())
		{
			switch (co_await PromptSaveAsync())
			{
			case ContentDialogResult::None:
				FocusEditor();
				co_return;

			case ContentDialogResult::Primary:
				if (!co_await SaveAsync())
				{
					FocusEditor();
					co_return;
				}
				break;
			}
		}

		FocusEditor();

		_activeFile = nullptr;
		Editor().Editor().ClearAll();
		Editor().Editor().EmptyUndoBuffer();
		SetTitle(false);
	}

	IAsyncAction MainPage::ExitMenuItem_Click(IInspectable const &sender, RoutedEventArgs const &e)
	{
		if (Editor().Editor().Modify())
		{
			switch (co_await PromptSaveAsync())
			{
			case ContentDialogResult::None:
				FocusEditor();
				co_return;

			case ContentDialogResult::Primary:
				if (!co_await SaveAsync())
				{
					FocusEditor();
					co_return;
				}
				break;
			}
		}

		if (!co_await ApplicationView::GetForCurrentView().TryConsolidateAsync())
		{
			if (CoreApplication::Views().Size() > 1)
			{
				Window::Current().Close();
			}
			else
			{
				Application::Current().Exit();
			}
		}
	}

	void MainPage::ZoomInMenuItem_Click(IInspectable const &sender, RoutedEventArgs const &e)
	{
		FocusEditor();

		Editor().Editor().ZoomIn();
	}

	void MainPage::ZoomOutMenuItem_Click(IInspectable const &sender, RoutedEventArgs const &e)
	{
		FocusEditor();

		Editor().Editor().ZoomOut();
	}

	void MainPage::ZoomRestoreMenuItem_Click(IInspectable const &sender, RoutedEventArgs const &e)
	{
		FocusEditor();

		Editor().Editor().Zoom(0);
	}

	void MainPage::StatusBarMenuItem_Click(IInspectable const &sender, RoutedEventArgs const &e)
	{
		FocusEditor();

		StatusBar().Visibility(sender.as<ToggleMenuFlyoutItem>().IsChecked() ? Visibility::Visible : Visibility::Collapsed);
	}

	void MainPage::WordWrapMenuItem_Click(IInspectable const &sender, RoutedEventArgs const &e)
	{
		FocusEditor();

		Editor().Editor().WrapMode(sender.as<ToggleMenuFlyoutItem>().IsChecked() ? Wrap::Word : Wrap::None);
	}

	void MainPage::TimeDateMenuItem_Click(IInspectable const &sender, RoutedEventArgs const &e)
	{
		FocusEditor();

		// Using GetTimeFormatEx instead of Windows.Globalization.DateTimeFormatting because the latter adds left to right marks
		SYSTEMTIME time;
		GetLocalTime(&time);
		WCHAR timeBuffer[MAX_PATH];
		GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, TIME_NOSECONDS, &time, nullptr, timeBuffer, MAX_PATH);
		WCHAR dateBuffer[MAX_PATH];
		GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, DATE_SHORTDATE, &time, nullptr, dateBuffer, MAX_PATH, nullptr);
		const auto formatted{ std::format(L"{} {}", timeBuffer, dateBuffer) };
		Editor().Editor().AddText(formatted.size(), formatted);
	}

	void MainPage::CommandMenuItem_Click(IInspectable const &sender, RoutedEventArgs const &e)
	{
		FocusEditor();

		Editor().Scintilla(unbox_value<ScintillaMessage>(sender.as<FrameworkElement>().Tag()), 0, 0);
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
		_activeFile = file;
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
		SetTitle(false);
	}

	IAsyncOperation<bool> MainPage::SaveAsync()
	{
		if (_activeFile)
		{
			co_await SaveAsync(_activeFile);
			co_return true;
		}
		else
		{
			co_return co_await SaveAsAsync();
		}
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

	IAsyncOperation<bool> MainPage::SaveAsAsync()
	{
		const FileSavePicker savePicker{};
		savePicker.FileTypeChoices().Insert(L"Text documents", single_threaded_vector<hstring>({ L".txt" }));
		savePicker.FileTypeChoices().Insert(L"All files", single_threaded_vector<hstring>({ L"." }));
		const auto file{ co_await savePicker.PickSaveFileAsync() };
		if (file)
		{
			_activeFile = file;
			SetTitle(true);
			co_await SaveAsync(file);
			co_return true;
		}
		else
		{
			co_return false;
		}
	}

	void MainPage::SetTitle(bool modified)
	{
		const auto title{ (modified ? L"*" : L"") + (_activeFile ? _activeFile.Name() : L"Untitled") };
		TitleText().Text(title + L" - Mica Editor");
		ApplicationView::GetForCurrentView().Title(title);
	}

	void MainPage::Editor_UpdateUI(MicaEditor::Editor const &sender, UpdateUIEventArgs const &args)
	{
		if (static_cast<int>(static_cast<Update>(args.Updated()) & (Update::Content | Update::Selection)))
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

	void MainPage::FocusEditor()
	{
		Editor().Focus(FocusState::Programmatic);
	}

	template <typename T>
	T GetFirstOfTypeAndName(DependencyObject const &parent, hstring const &name)
	{
		const auto count{ VisualTreeHelper::GetChildrenCount(parent) };
		for (auto i = 0; i < count; i++)
		{
			const auto child{ VisualTreeHelper::GetChild(parent, i) };
			const auto c{ child.try_as<T>() };
			if (c && c.Name() == name)
			{
				return c;
			}
			else if (const auto sub{ GetFirstOfTypeAndName<T>(child, name) })
			{
				return sub;
			}
		}

		return nullptr;
	}

	IAsyncOperation<ContentDialogResult> MainPage::PromptSaveAsync()
	{
		const ContentDialog dialog{};
		dialog.Style(Application::Current().Resources().Lookup(box_value(L"DefaultContentDialogStyle")).as<Windows::UI::Xaml::Style>()); // Fixes opening animation
		dialog.Title(box_value(L"Mica Editor"));
		dialog.Content(box_value(std::format(L"Do you want to save changes to {}?", _activeFile ? _activeFile.Name() : L"Untitled")));
		dialog.DefaultButton(ContentDialogButton::Primary);
		dialog.PrimaryButtonText(L"Save");
		dialog.SecondaryButtonText(L"Don\u2019t save");
		dialog.CloseButtonText(L"Cancel");
		if (_hasFcu)
		{
			dialog.PreviewKeyDown([](IInspectable const &sender, KeyRoutedEventArgs const &e)
				{
					switch (e.Key())
					{
					case VirtualKey::S:
						sender.as<FrameworkElement>().Tag(box_value(ContentDialogResult::Primary));
						sender.as<ContentDialog>().Hide();
						break;

					case VirtualKey::N:
						sender.as<FrameworkElement>().Tag(box_value(ContentDialogResult::Secondary));
						sender.as<ContentDialog>().Hide();
						break;
					}
				});
		}
		dialog.Opened([](ContentDialog const &sender, ContentDialogOpenedEventArgs const &e)
			{
				GetFirstOfTypeAndName<Button>(sender, L"PrimaryButton").AccessKey(L"S");
				GetFirstOfTypeAndName<Button>(sender, L"SecondaryButton").AccessKey(L"N");
			});
		const auto result{ co_await dialog.ShowAsync() };
		co_return unbox_value_or<ContentDialogResult>(dialog.Tag(), result);
	}
}
