// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      28sep04	initial version

        numeric edit control
 
*/

// NumEdit.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "NumEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNumEdit

CNumEdit::CNumEdit()
{
	m_Val = 0;
	m_Precision = 2;
}

CNumEdit::~CNumEdit()
{
}

void CNumEdit::SetPrecision(int Precision)
{
	m_Precision = Precision;
}

void CNumEdit::SetText()
{
	CString	s;
	ValToStr(s);
	SetWindowText(s);
}

void CNumEdit::GetText()
{
	CString	s;
	GetWindowText(s);
	StrToVal(s);
}

void CNumEdit::SetVal(float Val)
{
	m_Val = Val;
	SetText();
}

void CNumEdit::SetBlank()
{
	m_Val = 0;
	SetWindowText("");
}

bool CNumEdit::IsBlank() const
{
	CString	s;
	GetWindowText(s);
	return(s.IsEmpty() != 0);
}

void CNumEdit::AddSpin(float Delta)
{
	GetText();	// freshen value before incrementing it
	m_Val += Delta;
	SetText();
	NotifyParent();
	SetModify();	// otherwise kill focus cancels edit
}

void CNumEdit::StrToVal(LPCSTR Str)
{
	sscanf(Str, "%f", &m_Val);
}

void CNumEdit::ValToStr(CString& Str)
{
	Str.Format("%.*f", m_Precision, m_Val);
}

void CNumEdit::NotifyParent()
{
	CWnd	*Parent = GetParent();
	if (Parent != NULL) {
		NMHDR	nmh;
		nmh.hwndFrom = m_hWnd;
		nmh.idFrom = GetDlgCtrlID();
		nmh.code = 0;
		Parent->SendMessage(WM_NOTIFY, nmh.idFrom, long(&nmh));
	}
}

void CNumEdit::SaveUndoState(CUndoState& State)
{
	State.m_Val.x.f = m_Val;
}

void CNumEdit::RestoreUndoState(const CUndoState& State)
{
	m_Val = State.m_Val.x.f;
	SetText();
	GetParent()->SetFocus();	// keeps accelerators working
	NotifyParent();
}

void CNumEdit::GetUndoTitle(const CUndoState& State, CString& Title)
{
	Title = m_UndoTitle;
}

void CNumEdit::SetUndoTitle(LPCSTR Title)
{
	m_UndoTitle = Title;
}

BEGIN_MESSAGE_MAP(CNumEdit, CEdit)
	//{{AFX_MSG_MAP(CNumEdit)
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
	ON_CONTROL_REFLECT(EN_SETFOCUS, OnSetfocus)
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNumEdit message handlers

void CNumEdit::OnSetfocus() 
{
	NotifyUndoableEdit(0);
}

void CNumEdit::OnKillfocus() 
{
	if (!GetModify()) {
		if (!IsUndoInProgress())
			CancelUndoableEdit(0);
	} else {
		GetText();
		SetText();
		NotifyParent();
	}
}

void CNumEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch (nChar) {
	case '.':
	case '-':
	case VK_BACK:
		break;
	default:
		if (!(isdigit(nChar) || iscntrl(nChar)))
			return;
	}
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

BOOL CNumEdit::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		GetParent()->SetFocus();	// return ends edit
	return CEdit::PreTranslateMessage(pMsg);
}
