/**
 * @file textclipsview.cpp
 * @brief View to display text clips.
 * @author Simon Steele
 * @note Copyright (c) 2002-2003 Simon Steele <s.steele@pnotepad.org>
 *
 * Programmers Notepad 2 : The license file (license.[txt|html]) describes 
 * the conditions under which this source may be modified / distributed.
 */

#include "stdafx.h"
#include "resource.h"
#include "textclips.h"
#include "textclipsview.h"
#include "childfrm.h"

CClipsDocker::CClipsDocker()
{
	m_pTheClips = new TextClips::TextClipsManager();
}

CClipsDocker::~CClipsDocker()
{
	delete m_pTheClips;
}

LRESULT CClipsDocker::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_TEXTCLIPS), 
			IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	SetIcon(hIconSmall, FALSE);

	RECT rc;
	TEXTMETRIC tm;

	GetClientRect(&rc);

	HDC hdc = GetDC();
	HFONT hFont = ((HFONT)GetStockObject( DEFAULT_GUI_FONT ));
	HFONT hOldFont = static_cast<HFONT>( ::SelectObject(hdc, hFont) );
	
	GetTextMetrics(hdc, &tm);
	//::SelectObject(hdc, hOldFont);
	int fontHeight = tm.tmHeight;
	m_comboHeight = MulDiv(12, fontHeight, 8); // 12 dialog units - default height.
	
	CRect rcCombo(rc);
	rcCombo.bottom = rcCombo.top + (m_comboHeight * 8); // what value here?
	rc.top += m_comboHeight;

	m_view.Create(m_hWnd, rc, _T("ClipsList"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | LVS_REPORT | LVS_NOCOLUMNHEADER, 0, IDC_CLIPSLIST);
	m_view.ShowWindow(SW_SHOW);

	m_view.InsertColumn(0, _T("Name"), LVCFMT_LEFT, rc.right - rc.left, 0);

	m_combo.Create(m_hWnd, rcCombo, _T("ClipsCombo"), WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_TABSTOP | CBS_DROPDOWNLIST);
	m_combo.SetFont( static_cast<HFONT> (GetStockObject( DEFAULT_GUI_FONT )) );
	
	for(TextClips::LIST_CLIPSETS::const_iterator i = m_pTheClips->GetClipSets().begin();
		i != m_pTheClips->GetClipSets().end();
		++i)
	{
		m_combo.InsertString(m_combo.GetCount(), (*i)->GetName());
	}

	m_combo.InsertString(0, "test 1");
	m_combo.InsertString(0, "test 2");
	m_combo.InsertString(0, "test 3");
	m_combo.InsertString(0, "test 4");

	const TextClips::LIST_CLIPS& clips = (*m_pTheClips->GetClipSets().begin())->GetClips();
	
	int index;
	for(TextClips::LIST_CLIPS::const_iterator i = clips.begin(); i != clips.end(); ++i)
	{
		index = m_view.InsertItem(m_view.GetItemCount(), (*i)->Name.c_str());
		m_view.SetItemData(index, reinterpret_cast<DWORD_PTR>( (*i) ));
	}

	return 0;
}

LRESULT CClipsDocker::OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	if(wParam != SIZE_MINIMIZED )
	{
		RECT rc;
		GetClientRect(&rc);
		CRect rcCombo(rc);
		rcCombo.bottom = rcCombo.top + m_comboHeight;
		rc.top += m_comboHeight;

		m_view.SetWindowPos(NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top ,SWP_NOZORDER | SWP_NOACTIVATE);
		m_combo.SetWindowPos(NULL, rcCombo.left, rcCombo.top, rcCombo.right - rcCombo.left, rcCombo.bottom - rcCombo.top, SWP_NOZORDER | SWP_NOACTIVATE);
	}

	bHandled = FALSE;

	return 0;
}

/**
 * For some reason the WM_CTLCOLORLISTBOX message from the combo is not getting
 * a satisfactory result somewhere resulting in a black background. This fixes that.
 */
LRESULT CClipsDocker::OnCtlColor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	return (LRESULT)::GetSysColorBrush( COLOR_WINDOW );
}

LRESULT CClipsDocker::OnHide(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	Hide();

	return 0;
}

/**
 * Insert a selected clip into the current text
 */
LRESULT CClipsDocker::OnClipSelected(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	TextClips::Clip* clip = reinterpret_cast<TextClips::Clip*>( m_view.GetItemData(((LPNMITEMACTIVATE)pnmh)->iItem));
	
	CChildFrame* pChild = CChildFrame::FromHandle( GetCurrentEditor() );
	
	if(pChild)
	{
		CTextView* pS = pChild->GetTextView();
		if(!pS)
			return 0;

		tstring clipstr = clip->Text;
		
		size_t offset = clipstr.find(_T('|'));
		if(offset != clipstr.npos)
			clipstr.erase(offset, 1);
		else
			offset = 0;
		
		int curPos = pS->GetCurrentPos();
	
		pS->InsertText(curPos, clipstr.c_str());
		pS->SetCurrentPos(curPos + offset);
		pS->SetSel(curPos + offset, curPos + offset);
		::SetFocus(pS->m_hWnd);
	}

	return 0;
}

void CClipsDocker::AddClip(TextClips::Clip* tc)
{
	int iIndex = m_view.InsertItem(m_view.GetItemCount(), tc->Name.c_str());
	m_view.SetItemData(iIndex, reinterpret_cast<DWORD_PTR>(tc));
}