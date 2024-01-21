#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"
#include "App.h"
#include "Helpers.h"

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
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Metadata;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Streams;
using namespace Microsoft::UI::Xaml::Controls;
using namespace WinUIEditor;

namespace winrt::CppDemoUwp::implementation
{
	void MainPage::InitializeComponent()
	{
		MainPageT::InitializeComponent();

		_closeRequestedRevoker = SystemNavigationManagerPreview::GetForCurrentView().CloseRequested(auto_revoke, { this, &MainPage::OnCloseRequested });

		SetTitle(false);

		_updateUIRevoker = Editor().Editor().UpdateUI(auto_revoke, { this, &MainPage::Editor_UpdateUI });
		_zoomChangedRevoker = Editor().Editor().ZoomChanged(auto_revoke, { this, &MainPage::Editor_ZoomChanged });
		_savePointReachedRevoker = Editor().Editor().SavePointReached(auto_revoke, { this, &MainPage::Editor_SavePointReached });
		_savePointLeftRevoker = Editor().Editor().SavePointLeft(auto_revoke, { this, &MainPage::Editor_SavePointLeft });

		_activatedRevoker = CoreWindow::GetForCurrentThread().Activated(auto_revoke, { this, &MainPage::OnActivated });

		if (!ApiInformation::IsApiContractPresent(L"Windows.Foundation.UniversalApiContract", 14, 0))
		{
			Editor().Editor().StyleSetFont(static_cast<int32_t>(StylesCommon::Default), L"Consolas");
		}
	}

	fire_and_forget MainPage::OnCloseRequested(IInspectable sender, SystemNavigationCloseRequestedPreviewEventArgs e)
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

	void MainPage::SettingsButton_Click(IInspectable const &sender, RoutedEventArgs const &e)
	{
		Application::Current().as<App>()->GoToSettingsPage();
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

	fire_and_forget MainPage::NewMenuItem_Click(IInspectable const &sender, RoutedEventArgs const &e)
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

	fire_and_forget MainPage::ExitMenuItem_Click(IInspectable const &sender, RoutedEventArgs const &e)
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

		co_await CloseCurrentWindowAsync();
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

	void MainPage::DisableBidirectionalMenuItem_Click(IInspectable const &sender, RoutedEventArgs const &e)
	{
		FocusEditor();

		Editor().Editor().Bidirectional(Bidirectional::Disabled);
	}

	void MainPage::LtrBidirectionalMenuItem_Click(IInspectable const &sender, RoutedEventArgs const &e)
	{
		FocusEditor();

		Editor().Editor().Bidirectional(Bidirectional::L2r);
	}

	void MainPage::RtlBidirectionalMenuItem_Click(IInspectable const &sender, RoutedEventArgs const &e)
	{
		FocusEditor();

		Editor().Editor().Bidirectional(Bidirectional::R2l);
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
		const auto formatted{ format(L"{} {}", timeBuffer, dateBuffer) };
		Editor().Editor().ReplaceSel(formatted);
		Editor().Editor().ScrollCaret();
	}

	void MainPage::CommandMenuItem_Click(IInspectable const &sender, RoutedEventArgs const &e)
	{
		FocusEditor();

		Editor().SendMessage(unbox_value<ScintillaMessage>(sender.as<FrameworkElement>().Tag()), 0, 0);
	}

	void MainPage::HighlightingLanguageButton_Click(IInspectable const &sender, RoutedEventArgs const &e)
	{
		FlyoutBase::ShowAttachedFlyout(sender.as<FrameworkElement>());
	}

	void MainPage::HighlightingLanguageItem_Click(IInspectable const &sender, RoutedEventArgs const &e)
	{
		FocusEditor();

		const auto item{ sender.as<MenuFlyoutItem>() };
		Editor().HighlightingLanguage(unbox_value<hstring>(item.Tag()));
		HighlightingLanguageButton().Content(box_value(item.Text()));
	}

	void MainPage::OnNavigatedTo(NavigationEventArgs const &e)
	{
		if (const auto file{ e.Parameter().try_as<StorageFile>() })
		{
			OpenAsync(file);
		}
	}

	class ILoader
	{
	public:
		virtual int __stdcall Release() = 0;
		// Returns a status code from SC_STATUS_*
		virtual int __stdcall AddData(const char *data, ptrdiff_t length) = 0;
		virtual void *__stdcall ConvertToDocument() = 0;
	};

	IAsyncAction MainPage::OpenAsync(StorageFile file)
	{
		// Todo: this needs to be aborted if the window is closed while trying to open a document
		Editor().Editor().ReadOnly(true);
		_activeFile = file;
		const auto stream{ co_await file.OpenReadAsync() };
		auto options{ DocumentOption::Default };
		const auto size{ stream.Size() };
		const auto large{ size > 100000000 };
		if (large)
		{
			options = DocumentOption::TextLarge | DocumentOption::StylesNone;
			FileLoadingBar().Value(0);
			FileLoadingBar().Maximum(size);
			RCIndicator().Visibility(Visibility::Collapsed);
			FileLoadingBar().Visibility(Visibility::Visible);
		}
		const auto loader{ reinterpret_cast<ILoader *>(Editor().Editor().CreateLoader(size, options)) };
		constexpr size_t blockSize{ 128 * 1024 };
		const Buffer string{ blockSize };
		auto i{ 0 };
		uint64_t progress{ 0 };
		do
		{
			co_await stream.ReadAsync(string, string.Capacity(), InputStreamOptions::None);
			loader->AddData(reinterpret_cast<char *>(string.data()), string.Length());
			if (large)
			{
				progress += string.Length();
				// Updating the progress bar every block is too slow
				if (i++ == 0) FileLoadingBar().Value(progress + string.Length());
				else if (i == 128) i = 0;
			}
		} while (string.Length() > 0);
		if (large)
		{
			FileLoadingBar().Value(progress);
		}
		stream.Close();
		Editor().Editor().DocPointer(reinterpret_cast<uint64_t>(loader->ConvertToDocument()));
		const auto zoom{ Editor().Editor().Zoom() };
		Editor().Editor().Zoom(zoom + 1); // Zoom in and out to force recalculation of number width
		Editor().Editor().Zoom(zoom); // Todo: Remove hack
		Editor().Editor().AllocateLineCharacterIndex(LineCharacterIndexType::Utf16); // Todo: Run on background thread
		Editor().Editor().UndoCollection(true);
		if (large)
		{
			FileLoadingBar().Visibility(Visibility::Collapsed);
			RCIndicator().Visibility(Visibility::Visible);
		}
		SetTitle(false);
		Editor().Editor().ReadOnly(false);
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

	void MainPage::Editor_UpdateUI(WinUIEditor::Editor const &sender, UpdateUIEventArgs const &args)
	{
		if (static_cast<int>(static_cast<Update>(args.Updated()) & (Update::Content | Update::Selection)))
		{
			const auto pos{ sender.CurrentPos() };
			const auto ln{ sender.LineFromPosition(pos) };
			const auto col{ sender.GetColumn(pos) };
			RCIndicator().Text(format(L"Ln {}, Col {}", ln + 1, col + 1));
		}
	}

	void MainPage::Editor_ZoomChanged(WinUIEditor::Editor const &sender, ZoomChangedEventArgs const &args)
	{
		const auto size{ sender.StyleGetSizeFractional(static_cast<int32_t>(StylesCommon::Default)) };
		ZoomStatus().Text(to_hstring(static_cast<uint16_t>(std::round((size + sender.Zoom() * 100) * 100.0 / size))) + L"%");
	}

	void MainPage::Editor_SavePointReached(WinUIEditor::Editor const &sender, SavePointReachedEventArgs const &args)
	{
		SetTitle(false);
	}

	void MainPage::Editor_SavePointLeft(WinUIEditor::Editor const &sender, SavePointLeftEventArgs const &args)
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
		dialog.RequestedTheme(Application::Current().as<App>()->Theme());
		dialog.Style(Application::Current().Resources().Lookup(box_value(L"DefaultContentDialogStyle")).as<Windows::UI::Xaml::Style>()); // Fixes opening animation
		dialog.Title(box_value(L"Mica Editor"));
		dialog.Content(box_value(format(L"Do you want to save changes to {}?", _activeFile ? _activeFile.Name() : L"Untitled")));
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
				if (const auto primaryButton{ GetFirstOfTypeAndName<Button>(sender, L"PrimaryButton") })
				{
					primaryButton.AccessKey(L"S");
				}
				if (const auto secondaryButton{ GetFirstOfTypeAndName<Button>(sender, L"SecondaryButton") })
				{
					secondaryButton.AccessKey(L"N");
				}
			});
		const auto result{ co_await dialog.ShowAsync() };
		co_return unbox_value_or<ContentDialogResult>(dialog.Tag(), result);
	}
}
