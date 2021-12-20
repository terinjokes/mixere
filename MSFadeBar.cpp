// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      28sep04	initial version
		01		07oct04	undo slider was updating edit ctrl but not slider

		mute/solo fade time dialog bar
 
*/

// MSFadeBar.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "MSFadeBar.h"
#include "AutoSliderDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMSFadeBar dialog

CMSFadeBar::CMSFadeBar()
	: CDialogBarEx(NULL)
{
	//{{AFX_DATA_INIT(CMSFadeBar)
	//}}AFX_DATA_INIT
}

void CMSFadeBar::GetInfo(CAutoInfo& Info) const
{
	m_TimeEdit.GetInfo(Info);
}

void CMSFadeBar::SetInfo(const CAutoInfo& Info)
{
	m_TimeEdit.SetInfo(Info);
	m_Slider.SetPos(m_TimeEdit.GetPos());
}

void CMSFadeBar::SetTime(int Time)
{
	m_TimeEdit.SetTime(Time);
	m_Slider.SetPos(m_TimeEdit.GetPos());
}

void CMSFadeBar::SaveUndoState(CUndoState& State)
{
	switch (State.GetCtrlID()) {
	case IDC_MS_FADE_SLIDER:		// slider position isn't precise enough
		State.m_Val.x.i = m_TimeEdit.GetTime();
		break;
	case IDC_MS_FADE_MINUTES:		// minutes changes time as side effect
		WndToUndo(&m_Minutes)->SaveUndoState(State);
		State.m_Val.y.i = m_TimeEdit.GetTime();
		break;
	}
}

void CMSFadeBar::RestoreUndoState(const CUndoState& State)
{
	switch (State.GetCtrlID()) {
	case IDC_MS_FADE_SLIDER:
		m_TimeEdit.SetTime(State.m_Val.x.i);
		m_Slider.SetPos(m_TimeEdit.GetPos());	// update slider too
		break;
	case IDC_MS_FADE_MINUTES:
		WndToUndo(&m_Minutes)->RestoreUndoState(State);
		m_TimeEdit.SetTime(State.m_Val.y.i);
		break;
	}
}

void CMSFadeBar::GetUndoTitle(const CUndoState& State, CString& Title)
{
	CUndoable	*uap = FindUndoable(State.GetCtrlID());
	if (uap != NULL)
		uap->GetUndoTitle(State, Title);
}

void CMSFadeBar::UpdateTime()
{
	OnTimeChange();
}

void CMSFadeBar::OnTimeChange()
{
}

void CMSFadeBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBarEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMSFadeBar)
	DDX_Control(pDX, IDC_MS_FADE_SPIN, m_TimeSpin);
	DDX_Control(pDX, IDC_MS_FADE_EDIT, m_TimeEdit);
	DDX_Control(pDX, IDC_MS_FADE_SLIDER, m_Slider);
	DDX_Control(pDX, IDC_MS_FADE_MINUTES, m_Minutes);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMSFadeBar, CDialogBarEx)
	//{{AFX_MSG_MAP(CMSFadeBar)
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_NOTIFY(UDN_DELTAPOS, IDC_MS_FADE_SPIN, OnDeltaposTimeSpin)
	ON_BN_CLICKED(IDC_MS_FADE_MINUTES, OnMinutes)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_INITDIALOG, OnInitDialog)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMSFadeBar message handlers

LRESULT CMSFadeBar::OnInitDialog(WPARAM wParam, LPARAM lParam)
{
	CDialogBarEx::OnInitDialog(wParam, lParam);
	SetWindowText(LDS(MS_FADE_BAR));
	SetBarCaption(LDS(MS_FADE_CAPTION));
	m_Slider.SetRange(0, CAutoSliderDlg::SLIDER_RANGE);
	m_Slider.CreateTicks(CAutoSliderDlg::SLIDER_TICKS);
	// give edit control a fatter font
	m_TimeEdit.SendMessage(WM_SETFONT, (WPARAM)GetStockObject(SYSTEM_FONT));
	m_TimeEdit.SetUndoTitle(LDS(MS_FADE_EDIT));
	m_Slider.SetRange(0, 60);
	m_Minutes.SetIcons(IDI_MINUTESU, IDI_MINUTESD);
	// add controls to resizing object
	m_Resize.AddControl(IDC_MS_FADE_SLIDER, BIND_LEFT | BIND_RIGHT);
	m_Resize.AddControl(IDC_MS_FADE_MINUTES, BIND_RIGHT);
	m_Resize.AddControl(IDC_MS_FADE_EDIT, BIND_RIGHT);
	m_Resize.AddControl(IDC_MS_FADE_SPIN, BIND_RIGHT);
	m_Resize.FixControls();
	// these controls send undo notifications, but we handle save/restore
	m_Slider.SetUndoHandler(this);
	m_Minutes.SetUndoHandler(this);

	return TRUE;
}

void CMSFadeBar::OnSize(UINT nType, int cx, int cy) 
{
	CDialogBarEx::OnSize(nType, cx, cy);
	m_Resize.OnSize();
}

void CMSFadeBar::OnMinutes() 
{
	m_TimeEdit.SetUnit(m_Minutes.GetCheck());
	UpdateTime();
}

void CMSFadeBar::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CDialogBarEx::OnHScroll(nSBCode, nPos, pScrollBar);
	m_TimeEdit.SetPos(m_Slider.GetPos());
	UpdateTime();
}

BOOL CMSFadeBar::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	NMHDR	*nmh = (NMHDR *)lParam;
	if (nmh->hwndFrom == m_TimeEdit.m_hWnd) {
		m_Slider.SetPos(m_TimeEdit.GetPos());
		m_Minutes.SetCheck(m_TimeEdit.GetUnit());
		UpdateTime();
		return(TRUE);
	}
	return CDialogBarEx::OnNotify(wParam, lParam, pResult);
}

void CMSFadeBar::OnDeltaposTimeSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	m_TimeEdit.AddSpin(-pNMUpDown->iDelta);
	*pResult = 0;
}
