// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      31jan04	initial version
		01		22feb04	add undo
		02		05mar04	escape cancels undoable edit now
		03		25jan05	restore undo state must notify parent of change

        static control with child edit box
 
*/

// StaticEdit.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "StaticEdit.h"
#include "PopupEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStaticEdit

CStaticEdit::CStaticEdit()
{
	m_MaxText = 0;	// no limit
}

bool CStaticEdit::IsEdit()
{
	return(CPopupEdit::Exists());
}

void CStaticEdit::SetLimitText(UINT nMax)
{
	m_MaxText = nMax;
}

void CStaticEdit::Edit()
{
	NotifyUndoableEdit(0);
	// If the user right-clicks on a scroll bar, the edit control destroys 
	// itself immediately, but we don't regain control until the mouse button
	// is released.  This can cause the display to temporarily revert to the 
	// unmodified value.  To avoid this, the edit control modifies our window 
	// text directly, instead of modifying a buffer that we display.
	CPopupEdit	pe(this);
	pe.SetLimitText(m_MaxText);
	// if edit ends normally and text was changed, notify parent
	if (pe.DoModal() == IDOK && pe.GetModify()) {
		NotifyParent(EN_CHANGE);
	} else {
		if (pe.Exists())	// make sure our instance still exists; see below
			CancelUndoableEdit(0);
	// DO NOT access non-static members if CPopupEdit::Exists returns FALSE.
	// If our parent window is closed while we're inside CPopupEdit::DoModal,
	// our instance will be destroyed BEFORE we regain control; incredible, but
	// true.  DoModal runs the windows message loop while it waits, so whatever 
	// message handler destroyed our parent and its children (including us) was
	// called from DoModal and MUST finish executing before DoModal can return.
	}
}

void CStaticEdit::NotifyParent(int Notification)
{
	GetParent()->PostMessage(WM_COMMAND, 
		MAKELONG(GetDlgCtrlID(), Notification), (LONG)m_hWnd);
}

void CStaticEdit::SaveUndoState(CUndoState& State)
{
	CString	s;
	GetWindowText(s);
	State.SetData(s.GetBuffer(0), s.GetLength() + 1);
}

void CStaticEdit::RestoreUndoState(const CUndoState& State)
{
	CString	s = (char *)State.GetData();
	SetWindowText(s);
	NotifyParent(EN_CHANGE);
}

void CStaticEdit::GetUndoTitle(const CUndoState& State, CString& Title)
{
	// base class behavior of returning window text is not helpful
	Title = LDS(STATIC_EDIT_TEXT);
}

BEGIN_MESSAGE_MAP(CStaticEdit, CStatic)
	//{{AFX_MSG_MAP(CStaticEdit)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStaticEdit message handlers

void CStaticEdit::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	Edit();
}

void CStaticEdit::OnDestroy() 
{
	// closing app while we're stuck in a modal loop causes crash
	CPopupEdit::Abort();	// abort modal loop and destroy edit instance
	CStatic::OnDestroy();
}
