// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      05dec03 initial version
		01		30dec03	convert to modeless, add automation
		02		08jan04	add ticks to slider
		03		18jan04	parent is now frame instead of view
		04		20jan04	convert to a base class
		05		24jan04	use auto pos as volume
		06		27jan04	add accelerator
		07		28jan04	set focus in OnShowWindow and when auto dialog closes
		08		01feb04	fix arrows and page up/down
		09		29feb04	add undo
        10      10mar04 was volume dlg, generalize for use as base class
		11		26sep04	override OnNewPos instead of handling HScroll

		automation slider dialog
 
*/

// AutoSliderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "AutoSliderDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAutoSliderDlg dialog

CAutoSliderDlg::CAutoSliderDlg(UINT nIDTemplate, UINT nIDAccel, CWnd* pParent /*=NULL*/)
	: CDialogEx(nIDTemplate, nIDAccel, pParent)
{
	//{{AFX_DATA_INIT(CAutoSliderDlg)
	//}}AFX_DATA_INIT
}

CAutoSliderDlg::~CAutoSliderDlg()
{
	DestroyWindow();
}

void CAutoSliderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutoSliderDlg)
	DDX_Control(pDX, IDC_AUTO_SLIDER, m_Slider);
	//}}AFX_DATA_MAP
}

void CAutoSliderDlg::TimerHook()
{
	if (m_Slider.IsPlaying())
		m_Slider.TimerHook();
}

void CAutoSliderDlg::GetInfo(CAutoInfo& Info)
{
	m_Slider.GetInfo(Info);
}

void CAutoSliderDlg::SetInfo(const CAutoInfo& Info)
{
	m_Slider.SetInfo(Info);
	SetNormPos(float(Info.m_Pos));
}

void CAutoSliderDlg::SetParent(CWnd *Parent)
{
	m_Slider.SetAutoDlgParent(Parent);
	CDialogEx::SetParent(Parent);
}

BOOL CAutoSliderDlg::EnableToolTips(BOOL bEnable)
{
	m_Slider.EnableToolTips(bEnable);
	return(CDialogEx::EnableToolTips(bEnable));
}

void CAutoSliderDlg::SaveUndoState(CUndoState& State)
{
	WndToUndo(&m_Slider)->SaveUndoState(State);
}

void CAutoSliderDlg::RestoreUndoState(const CUndoState& State)
{
	WndToUndo(&m_Slider)->RestoreUndoState(State);
}

void CAutoSliderDlg::GetUndoTitle(const CUndoState& State, CString& Title)
{
	WndToUndo(&m_Slider)->GetUndoTitle(State, Title);
}

void CAutoSliderDlg::CMyAutoSliderCtrl::OnNewPos()
{
	CAutoSliderDlg	*asp = (CAutoSliderDlg *)GetCookie();
	asp->OnPosChange();
}

/////////////////////////////////////////////////////////////////////////////
// CAutoSliderDlg overridables

void CAutoSliderDlg::OnPosChange()
{
}

BEGIN_MESSAGE_MAP(CAutoSliderDlg, CDialogEx)
	//{{AFX_MSG_MAP(CAutoSliderDlg)
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTip)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoSliderDlg message handlers

BOOL CAutoSliderDlg::OnInitDialog() 
{
	CDialogEx::OnInitDialog();

	m_Slider.SetRange(0, SLIDER_RANGE);
	m_Slider.CreateTicks(SLIDER_TICKS);
	m_Slider.SetUndoHandler(this);	// override default handling
	m_Slider.SetCookie(this);	// override slider's OnNewPos

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAutoSliderDlg::OnClose() 
{
	m_Slider.ShowAutoDlg(FALSE);
	CDialogEx::OnClose();
}

void CAutoSliderDlg::OnCancel()
{
	m_Slider.ShowAutoDlg(FALSE);
	CDialogEx::OnCancel();
}

BOOL CAutoSliderDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
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
	return CDialogEx::OnNotify(wParam, lParam, pResult);
}

void CAutoSliderDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	if (bShow)
		m_Slider.SetFocus();
}

BOOL CAutoSliderDlg::OnToolTip(UINT id, NMHDR* pTTTStruct, LRESULT* pResult)
{
	LPNMTTDISPINFO	ttp = LPNMTTDISPINFO(pTTTStruct);
	UINT nID = ttp->hdr.idFrom;
	if (ttp->uFlags & TTF_IDISHWND)		// idFrom can be HWND or ID
		nID = ::GetDlgCtrlID((HWND)nID);
	switch (nID) {
	case IDC_AUTO_SLIDER:
		{
			CString	s;
			GetWindowText(s);
			strcpy(ttp->szText, s);
		}
		return(TRUE);
	default:
		return(CDialogEx::OnToolTip(id, pTTTStruct, pResult));
	}
	return(FALSE);
}
