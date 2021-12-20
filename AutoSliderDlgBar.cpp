// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      28sep04	initial version

		automation slider dialog
 
*/

// AutoSliderDlgBar.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "AutoSliderDlgBar.h"
#include "AutoSliderDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAutoSliderDlgBar dialog

CAutoSliderDlgBar::CAutoSliderDlgBar(UINT nIDAccel)
	: CDialogBarEx(nIDAccel)
{
	//{{AFX_DATA_INIT(CAutoSliderDlgBar)
	//}}AFX_DATA_INIT
}

CAutoSliderDlgBar::~CAutoSliderDlgBar()
{
	DestroyWindow();
}

void CAutoSliderDlgBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBarEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutoSliderDlgBar)
	DDX_Control(pDX, IDC_AUTO_SLIDER, m_Slider);
	//}}AFX_DATA_MAP
}

void CAutoSliderDlgBar::TimerHook()
{
	if (m_Slider.IsPlaying())
		m_Slider.TimerHook();
}

void CAutoSliderDlgBar::GetInfo(CAutoInfo& Info) const
{
	m_Slider.GetInfo(Info);
}

void CAutoSliderDlgBar::SetInfo(const CAutoInfo& Info)
{
	m_Slider.SetInfo(Info);
	SetNormPos(float(Info.m_Pos));
}

BOOL CAutoSliderDlgBar::EnableToolTips(BOOL bEnable)
{
	m_Slider.EnableToolTips(bEnable);
	return(CDialogBarEx::EnableToolTips(bEnable));
}

void CAutoSliderDlgBar::SaveUndoState(CUndoState& State)
{
	WndToUndo(&m_Slider)->SaveUndoState(State);
}

void CAutoSliderDlgBar::RestoreUndoState(const CUndoState& State)
{
	WndToUndo(&m_Slider)->RestoreUndoState(State);
}

void CAutoSliderDlgBar::GetUndoTitle(const CUndoState& State, CString& Title)
{
	WndToUndo(&m_Slider)->GetUndoTitle(State, Title);
}

void CAutoSliderDlgBar::CMyAutoSliderCtrl::OnNewPos()
{
	CAutoSliderDlgBar	*asp = (CAutoSliderDlgBar *)GetCookie();
	asp->OnPosChange();
}

/////////////////////////////////////////////////////////////////////////////
// CAutoSliderDlgBar overridables

void CAutoSliderDlgBar::OnPosChange()
{
}

BEGIN_MESSAGE_MAP(CAutoSliderDlgBar, CDialogBarEx)
	//{{AFX_MSG_MAP(CAutoSliderDlgBar)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_INITDIALOG, OnInitDialog)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTip)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoSliderDlgBar message handlers

LRESULT CAutoSliderDlgBar::OnInitDialog(WPARAM wParam, LPARAM lParam) 
{
	CDialogBarEx::OnInitDialog(wParam, lParam);

	m_Slider.SetRange(0, CAutoSliderDlg::SLIDER_RANGE);
	m_Slider.CreateTicks(CAutoSliderDlg::SLIDER_TICKS);
	m_Slider.SetUndoHandler(this);	// override default handling
	m_Slider.SetCookie(this);	// override slider's OnNewPos
	// add controls to resizing object
	m_Resize.AddControl(IDC_AUTO_SLIDER, BIND_LEFT | BIND_RIGHT);
	m_Resize.FixControls();

	return TRUE;
}

void CAutoSliderDlgBar::OnClose() 
{
	m_Slider.ShowAutoDlg(FALSE);
	CDialogBarEx::OnClose();
}

BOOL CAutoSliderDlgBar::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	NMHDR	*nmh = (NMHDR *)lParam;
	switch (nmh->code) {
	case AUTOSLIDER_CREATE:
		{
			CString	Text;
			GetWindowText(Text);
			m_Slider.SetAutoDlgTitle(LDS(AUTO) + " " + Text);
		}
		break;
	case AUTOSLIDER_CLOSE:
		m_Slider.SetFocus();
		return(TRUE);
	}
	return CDialogBarEx::OnNotify(wParam, lParam, pResult);
}

void CAutoSliderDlgBar::OnSize(UINT nType, int cx, int cy) 
{
	CDialogBarEx::OnSize(nType, cx, cy);
	m_Resize.OnSize();
}

BOOL CAutoSliderDlgBar::OnToolTip(UINT id, NMHDR* pTTTStruct, LRESULT* pResult)
{
	LPNMTTDISPINFO	ttp = LPNMTTDISPINFO(pTTTStruct);
	UINT nID = ttp->hdr.idFrom;
	if (ttp->uFlags & TTF_IDISHWND)		// idFrom can be HWND or ID
		nID = ::GetDlgCtrlID((HWND)nID);
	switch (nID) {
	case IDC_AUTO_SLIDER:	// auto slider's tooltip is our window text
		{
			CString	s;
			GetWindowText(s);
			strcpy(ttp->szText, s);
		}
		return(TRUE);
	default:
		return(CDialogBarEx::OnToolTip(id, pTTTStruct, pResult));
	}
	return(FALSE);
}
