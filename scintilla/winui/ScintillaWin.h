// Scintilla source code edit control
/** @file ScintillaWin.h
 ** Define functions from ScintillaWin.cxx that can be called from ScintillaDLL.cxx.
 **/
 // Copyright 1998-2018 by Neil Hodgson <neilh@scintilla.org>
 // The License.txt file describes the conditions under which this software may be distributed.

#ifndef SCINTILLAWIN_H
#define SCINTILLAWIN_H

namespace Scintilla::Internal {
	class ScintillaWin;

	class ScintillaWinUI :
		public ScintillaBase,
		public ::winrt::implements<ScintillaWinUI, ::IVirtualSurfaceUpdatesCallbackNative>
	{
	public:
		ScintillaWinUI();
		void RegisterGraphics(winrt::com_ptr<::ISurfaceImageSourceNativeWithD2D> const &sisNativeWithD2D,
			winrt::com_ptr<::IVirtualSurfaceImageSourceNative> const &vsisNative,
			winrt::com_ptr<::ID2D1DeviceContext> const &d2dDeviceContext,
			std::shared_ptr<MicaEditor::Wrapper> const &wrapper);
		void DpiChanged();
		void SizeChanged();
		void FocusChanged(bool focused);
		void PointerPressed(winrt::Windows::Foundation::Point const &point, uint64_t timestamp, winrt::Windows::System::VirtualKeyModifiers const &modifiers);

	private:
		// Deleted so ScintillaWinUI objects can not be copied.
		ScintillaWinUI(const ScintillaWinUI &) = delete;
		ScintillaWinUI(ScintillaWinUI &&) = delete;
		ScintillaWinUI &operator=(const ScintillaWinUI &) = delete;
		ScintillaWinUI &operator=(ScintillaWinUI &&) = delete;

		winrt::com_ptr<::ISurfaceImageSourceNativeWithD2D> _sisNativeWithD2D{ nullptr };
		winrt::com_ptr<::IVirtualSurfaceImageSourceNative> _vsisNative{ nullptr };
		winrt::com_ptr<::ID2D1DeviceContext> _d2dDeviceContext{ nullptr };
		std::shared_ptr<::MicaEditor::Wrapper> _wrapper{ nullptr };

		// Timer implementation
		// Todo: Planning to not use XAML APIs on the Scintilla side, so replace DispatcherTimer with something that works outside of XAML
		// Candidates include DispatcherQueueTimer (not available on 1703) or ThreadPoolTimer, which should work in UWP and WinUI 3
		// TickReason: caret, scroll, widen, dwell, platform (unused?)
		winrt::DUX::DispatcherTimer _caretTimer{};
		winrt::DUX::DispatcherTimer _scrollTimer{};
		winrt::DUX::DispatcherTimer _widenTimer{};
		winrt::DUX::DispatcherTimer _dwellTimer{};
		void OnCaretTimerTick(winrt::Windows::Foundation::IInspectable const &sender, winrt::Windows::Foundation::IInspectable const &args);
		void OnScrollTimerTick(winrt::Windows::Foundation::IInspectable const &sender, winrt::Windows::Foundation::IInspectable const &args);
		void OnWidenTimerTick(winrt::Windows::Foundation::IInspectable const &sender, winrt::Windows::Foundation::IInspectable const &args);
		void OnDwellTimerTick(winrt::Windows::Foundation::IInspectable const &sender, winrt::Windows::Foundation::IInspectable const &args);
		winrt::DUX::DispatcherTimer::Tick_revoker _caretTickRevoker{};
		winrt::DUX::DispatcherTimer::Tick_revoker _scrollTickRevoker{};
		winrt::DUX::DispatcherTimer::Tick_revoker _widenTickRevoker{};
		winrt::DUX::DispatcherTimer::Tick_revoker _dwellTickRevoker{};
		winrt::DUX::DispatcherTimer GetTimerForReason(TickReason reason);

		virtual bool FineTickerRunning(TickReason reason) override;
		virtual void FineTickerStart(TickReason reason, int millis, int tolerance) override;
		virtual void FineTickerCancel(TickReason reason) override;
		virtual void SetVerticalScrollPos() override;
		virtual void SetHorizontalScrollPos() override;
		virtual bool ModifyScrollBars(Sci::Line nMax, Sci::Line nPage) override;
		virtual void Copy() override;
		virtual void Paste() override;
		virtual void ClaimSelection() override;
		virtual void NotifyChange() override;
		virtual void NotifyParent(Scintilla::NotificationData scn) override;
		virtual void CopyToClipboard(const SelectionText &selectedText) override;
		virtual void SetMouseCapture(bool on) override;
		virtual bool HaveMouseCapture() override;
		virtual std::string UTF8FromEncoded(std::string_view encoded) const override;
		virtual std::string EncodedFromUTF8(std::string_view utf8) const override;
		virtual Scintilla::sptr_t DefWndProc(Scintilla::Message iMessage, Scintilla::uptr_t wParam, Scintilla::sptr_t lParam) override;
		virtual void CreateCallTipWindow(PRectangle rc) override;
		virtual void AddToPopUp(const char *label, int cmd = 0, bool enabled = true) override;
		IFACEMETHOD(UpdatesNeeded)();
		void DrawBit(RECT const &drawingBounds);
	};
}

#endif
