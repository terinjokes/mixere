// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      31jan04	initial version
		01		05mar04	abort marks instance deleted now
		02		04jul04	use parent's font instead of default GUI font
		03		14jan05	add optional rect argument to ctor
		04		29jan05	ctor must initialize m_MaxText

        popup edit control
 
*/

// PopupEdit.cpp : implementation file
//

#include "stdafx.h"
#include "PopupEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPopupEdit

HHOOK CPopupEdit::m_MouseHook;
CPopupEdit *CPopupEdit::m_This;

CPopupEdit::CPopupEdit(CWnd *Parent, const CRect *Rect)
	: m_Parent(Parent), m_Rect(Rect)
{
	ASSERT(m_This == NULL);	// only one instance at a time
	m_This = this;
	m_Modify = FALSE;
	m_MaxText = 0;	// no limit
}

CPopupEdit::~CPopupEdit()
{
	m_This = NULL;
}

void CPopupEdit::SetLimitText(UINT nMax)
{
	m_MaxText = nMax;
}

void CPopupEdit::Abort()
{
	if (m_This != NULL) {
		m_This->EndModalLoop(IDABORT);
		m_This = NULL;	// mark instance deleted; we no longer exist
	}
}

void CPopupEdit::EndEdit(int retc)
{
	ShowWindow(SW_HIDE);
	if (retc == IDOK) {
		// set parent text even if we didn't modify it, to force repaint
		CString	Text;
		GetWindowText(Text);
		m_Parent->SetWindowText(Text);
		m_Modify = CEdit::GetModify() != 0;
	}
    EndModalLoop(retc);
}

LRESULT CALLBACK CPopupEdit::MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION) {	// ignore peek
		// if any kind of mouse button down event
		if (wParam == WM_RBUTTONDOWN || wParam == WM_LBUTTONDOWN
		|| wParam == WM_NCLBUTTONDOWN || wParam == WM_NCRBUTTONDOWN) {
			MOUSEHOOKSTRUCT	*mh = (MOUSEHOOKSTRUCT *)lParam;
			CRect	r;
			m_This->GetWindowRect(r);
			if (!r.PtInRect(mh->pt))	// if cursor is outside edit box
				m_This->EndEdit(IDOK);
		}
	}
	return(CallNextHookEx(m_MouseHook, nCode, wParam, lParam));
}

int CPopupEdit::DoModal()
{
	int	retc = IDABORT;
	m_MouseHook = SetWindowsHookEx(WH_MOUSE, MouseProc, 
		AfxGetApp()->m_hInstance, AfxGetApp()->m_nThreadID);
	if (m_MouseHook != NULL) {
		CString	Text;
		m_Parent->GetWindowText(Text);
		CRect	r;
		if (m_Rect != NULL)	// if ctor was passed a rect, use it
			r = *m_Rect;
		else	// otherwise use parent's client rect
			m_Parent->GetClientRect(r);
		// create the edit control
		if (CEdit::CreateEx(0, "edit", Text, 
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, r, m_Parent, 0)) {
			// set font to parent's font
			SetFont(m_Parent->GetFont());
			SetFocus();
			SetSel(0, -1);	// select entire text
			CEdit::SetLimitText(m_MaxText);
			retc = RunModalLoop();
			DestroyWindow();
		}
		UnhookWindowsHookEx(m_MouseHook);
	}
    return(retc);
}

BEGIN_MESSAGE_MAP(CPopupEdit, CEdit)
	//{{AFX_MSG_MAP(CPopupEdit)
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPopupEdit message handlers

void CPopupEdit::OnKillFocus(CWnd* pNewWnd) 
{
	EndEdit(IDCANCEL);
}

BOOL CPopupEdit::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST) {
		switch (pMsg->wParam) {
		case VK_ESCAPE:
			EndEdit(IDCANCEL);
			return(TRUE);
		case VK_RETURN:
		case VK_TAB:
			EndEdit(IDOK);
			return(TRUE);
		}
	}
	return CEdit::PreTranslateMessage(pMsg);
}
