// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      19apr04	initial version
		01		05oct04	no more child dialogs; remove OnNcActivate
		02		07oct04	override EnableToolTips to make it virtual

		customized dialog base class
 
*/

// DialogEx.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "DialogEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogEx dialog

CDialogEx::CDialogEx(UINT nIDTemplate, UINT nIDAccel, CWnd* pParent /*=NULL*/)
	: CDialog(nIDTemplate, pParent)
{
	//{{AFX_DATA_INIT(CDialogEx)
	//}}AFX_DATA_INIT
	m_Parent = NULL;
	m_Accel = LOADACCEL(nIDAccel);
}

CDialogEx::~CDialogEx()
{
	DestroyWindow();
}

void CDialogEx::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogEx)
	//}}AFX_DATA_MAP
}

void CDialogEx::SetParent(CWnd *Parent)
{
	m_Parent = Parent;
	CDialog::SetParent(Parent);
}

void CDialogEx::CenterWindowEx(const CPoint& Offset, bool Repaint)
{
	if (m_Parent == NULL)
		return;
	// center dialog's top left corner within parent window
	CRect	vr, dr;
	m_Parent->GetClientRect(vr);
	GetWindowRect(dr);
	MoveWindow(vr.Width() / 2 + Offset.x, vr.Height() / 2 + Offset.y, 
		dr.Width(), dr.Height(), Repaint);
}

BOOL CDialogEx::EnableToolTips(BOOL bEnable)
{
	return(CDialog::EnableToolTips(bEnable));
}

BEGIN_MESSAGE_MAP(CDialogEx, CDialog)
	//{{AFX_MSG_MAP(CDialogEx)
	ON_WM_HSCROLL()
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTip)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogEx message handlers

BOOL CDialogEx::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST) {
		if (TranslateAccelerator(m_hWnd, m_Accel, pMsg))
			return(TRUE);
	}
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CDialogEx::OnToolTip(UINT id, NMHDR* pTTTStruct, LRESULT* pResult)
{
	LPNMTTDISPINFO	ttp = LPNMTTDISPINFO(pTTTStruct);
	UINT nID = ttp->hdr.idFrom;
	if (ttp->uFlags & TTF_IDISHWND)		// idFrom can be HWND or ID
		nID = ::GetDlgCtrlID((HWND)nID);
	CString	s;
	s.LoadString(nID);
	strcpy(ttp->szText, s);
	return(TRUE);
}
