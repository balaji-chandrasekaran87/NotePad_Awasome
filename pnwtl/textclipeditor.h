/**
 * @file textclipeditor.h
 * @brief Text Clip Editing Dialog for Programmer's Notepad 2
 * @author Simon Steele
 * @note Copyright (c) 2007 Simon Steele <s.steele@pnotepad.org>
 *
 * Programmer's Notepad 2 : The license file (license.[txt|html]) describes 
 * the conditions under which this source may be modified / distributed.
 */

#ifndef textclipeditor_h__included
#define textclipeditor_h__included

/**
 * Editor window for text clips
 */
class CTextClipEditor : public CDialogImpl<CTextClipEditor>
{
public:
	CTextClipEditor(tstring shortcut, tstring text)
	{
		m_shortcut = shortcut;
		m_text = text;
	}

	enum {IDD = IDD_TEXTCLIPEDITOR };

	BEGIN_MSG_MAP(CTextClipEditor)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CRect rcScintilla;
		::GetWindowRect(GetDlgItem(IDC_PLACEHOLDER), rcScintilla);
		ScreenToClient(rcScintilla);
		m_scintilla.Create(m_hWnd, rcScintilla, "EditClipText", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_TABSTOP, WS_EX_STATICEDGE);
		::SetWindowPos(m_scintilla, GetDlgItem(IDC_PLACEHOLDER), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		m_scintilla.SetWrapMode(SC_WRAP_WORD);
		m_scintilla.AssignCmdKey(SCK_HOME, SCI_HOMEDISPLAY);
		m_scintilla.AssignCmdKey(SCK_END, SCI_LINEENDDISPLAY);
		m_scintilla.SetMarginWidthN(1, 0);
		
		// Stop scintilla from capturing the escape and tab keys...
		m_scintilla.ClearCmdKey(SCK_ESCAPE);
		m_scintilla.ClearCmdKey(SCK_TAB);

		m_scintilla.SetText(m_text.c_str());
	
		GetDlgItem(IDC_SHORTCUT_EDIT).SetWindowText(m_shortcut.c_str());

		return 0;
	}

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if (wID == IDOK)
		{
			CWindowText wt(GetDlgItem(IDC_SHORTCUT_EDIT).m_hWnd);
			if ((LPCTSTR)wt != NULL)
			{
				m_shortcut = (LPCTSTR)wt;
			}

			int len = m_scintilla.GetTextLength();
			char* buffer = new char[len+1];
			m_scintilla.GetText(len+1, buffer);
			buffer[len] = 0;
			m_text = buffer;
			delete [] buffer;
		}

		EndDialog(wID);
		return 0;
	}

	LPCTSTR GetShortcut() const
	{
		return m_shortcut.c_str();
	}

	LPCTSTR GetText() const
	{
		return m_text.c_str();
	}

private:
	CScintillaDialogWnd m_scintilla;
	tstring m_shortcut;
	tstring m_text;
};

#endif // #ifndef textclipeditor_h__included