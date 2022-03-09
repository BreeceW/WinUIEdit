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

	class ScintillaWinUI : public ScintillaBase
	{
	public:
		ScintillaWinUI() = default;

	private:
		// Deleted so ScintillaWinUI objects can not be copied.
		ScintillaWinUI(const ScintillaWinUI &) = delete;
		ScintillaWinUI(ScintillaWinUI &&) = delete;
		ScintillaWinUI &operator=(const ScintillaWinUI &) = delete;
		ScintillaWinUI &operator=(ScintillaWinUI &&) = delete;

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
	};
}

#endif
