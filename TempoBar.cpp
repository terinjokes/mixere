// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      28sep4	initial version

		tempo dialog bar
 
*/

// TempoBar.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "TempoBar.h"
#include "Audio.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTempoBar dialog

const float CTempoBar::m_NominalBpm = 120;

CTempoBar::CTempoBar() :
	CAutoSliderDlgBar(NULL)
{
	//{{AFX_DATA_INIT(CTempoBar)
	//}}AFX_DATA_INIT
}

float CTempoBar::GetNormTempo() const
{
	return(CAudio::LogToLinear(GetNormPos() * 2 - 1));
}

void CTempoBar::SetNormTempo(float Tempo)
{
	SetNormPos((CAudio::LinearToLog(Tempo) + 1) / 2);
}

/////////////////////////////////////////////////////////////////////////////
// CTempoBar overrides

void CTempoBar::OnPosChange()
{
	m_BpmEdit.SetVal(GetBpm());
}

void CTempoBar::DoDataExchange(CDataExchange* pDX)
{
	CAutoSliderDlgBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTempoBar)
	DDX_Control(pDX, IDC_BPM_SPIN, m_BpmSpin);
	DDX_Control(pDX, IDC_BPM_EDIT, m_BpmEdit);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTempoBar, CAutoSliderDlgBar)
	//{{AFX_MSG_MAP(CTempoBar)
	ON_NOTIFY(UDN_DELTAPOS, IDC_BPM_SPIN, OnDeltaposBpmSpin)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_INITDIALOG, OnInitDialog)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTempoBar message handlers

LONG CTempoBar::OnInitDialog(UINT wParam, LONG lParam)
{
	CAutoSliderDlgBar::OnInitDialog(wParam, lParam);
	SetWindowText(LDS(TEMPO_BAR));
	SetBarCaption(LDS(TEMPO_BAR));
	m_Slider.SetDefaultPos(m_Slider.GetRangeMax() / 2);
	// give edit control a fatter font
	m_BpmEdit.SendMessage(WM_SETFONT, (WPARAM)GetStockObject(SYSTEM_FONT));
	m_BpmEdit.SetUndoTitle(LDS(EDIT_TEMPO));
	m_BpmEdit.SetVal(GetBpm());
	// add additional controls to base class resizing object
	m_Resize.AddControl(IDC_BPM_EDIT, BIND_RIGHT);
	m_Resize.AddControl(IDC_BPM_SPIN, BIND_RIGHT);
	m_Resize.FixControls();

	return TRUE;
}

void CTempoBar::OnDeltaposBpmSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	m_BpmEdit.AddSpin(.01 * -pNMUpDown->iDelta);
	*pResult = 0;
}

BOOL CTempoBar::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	NMHDR	*nmh = (NMHDR *)lParam;
	if (nmh->hwndFrom == m_BpmEdit.m_hWnd) {
		SetBpm(m_BpmEdit.GetVal());
		return(TRUE);
	}
	return CAutoSliderDlgBar::OnNotify(wParam, lParam, pResult);
}
