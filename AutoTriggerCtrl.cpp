// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      02feb04 initial version
		01		15feb04	add resync
		02		16feb04	set auto pos replaces resync
		03		17feb04	if not triggering, don't resync
		04		10mar04	add scale to timer hook
		05		12mar04	shift-left click should open/close auto dlg
		06		24jun04	base class uses selection, override in timer hook
		07		07nov04	undo auto pos doesn't work; revert to base class undo
		08		14dec04	disable base class quick start behavior
		09		29dec04	OnLButtonDown must end select slider's modal state
		10		03jan05	waveform not supported, so disable waveform combo box
		11		07jan05	auto pos is now a member; m_Pos is always slider pos
		12		24feb05	looping is always on, so disable loop button

		auto slider adapted for triggering
 
*/

// AutoTriggerCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "AutoTriggerCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAutoTriggerCtrl

CAutoTriggerCtrl::CAutoTriggerCtrl()
{
	m_Info.m_StartPos = 0;
	m_Info.m_EndPos = 1;
	m_Info.m_Loop = TRUE;
	m_AutoStyle &= ~AUTO_DLG_WAVEFORM;	// disable auto dialog's waveform combo box
	m_AutoStyle &= ~AUTO_DLG_LOOP;		// disable auto dialog's loop button
	m_TriggerPos = 0;
}

CAutoTriggerCtrl::~CAutoTriggerCtrl()
{
}

void CAutoTriggerCtrl::GetInfo(CAutoInfo& Info) const
{
	CAutoSliderCtrl::GetInfo(Info);
	Info.m_Pos = m_TriggerPos;
}

void CAutoTriggerCtrl::SetInfo(const CAutoInfo& Info)
{
	CAutoSliderCtrl::SetInfo(Info);
	m_TriggerPos = Info.m_Pos;
}

bool CAutoTriggerCtrl::TimerHook(float Scale)
{
	// save and restore auto info's pos and selection
	double	Pos = m_Info.m_Pos;
	float	Start = m_Info.m_StartPos;
	float	End = m_Info.m_EndPos;
	m_Info.m_StartPos = 0;
	m_Info.m_EndPos = 1;
	m_Info.m_Pos = m_TriggerPos;
	bool	retc = CAutoDlg::TimerHook(m_Info, m_AutoDlg, Scale);
	m_TriggerPos = m_Info.m_Pos;
	if (m_TriggerPos <= 0 || m_TriggerPos >= 1)
		OnTrigger();
	m_Info.m_Pos = Pos;
	m_Info.m_StartPos = Start;
	m_Info.m_EndPos = End;
	m_WavePos = Pos;
	return(retc);
}

void CAutoTriggerCtrl::OnTransport(int Transport)
{
	if (Transport == PLAY && m_Info.m_Transport != PLAY) {
		m_Info.m_Loop = TRUE;
		Trigger();
	}
}

void CAutoTriggerCtrl::SetNormPosNoEcho(double Pos)
{
	// identical to base class SetNormPos, except don't call OnNewPos
	CSelectSliderCtrl::SetNormPos(Pos);
	m_Info.m_Pos = Pos;
}

void CAutoTriggerCtrl::StartFromPos(double Pos)
{
	m_TriggerPos = Pos;
	m_Info.m_Reverse = 0;
	if (Pos <= 0 || Pos >= 1)
		OnTrigger();
}

void CAutoTriggerCtrl::Trigger()
{
	m_TriggerPos = 0;
	m_Info.m_Reverse = 0;
	OnTrigger();
}

void CAutoTriggerCtrl::OnTrigger()
{
}

BEGIN_MESSAGE_MAP(CAutoTriggerCtrl, CAutoSliderCtrl)
	//{{AFX_MSG_MAP(CAutoTriggerCtrl)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoTriggerCtrl message handlers

void CAutoTriggerCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CSelectSliderCtrl::OnRButtonUp(nFlags, point);	// end modal state
	if (nFlags & MKU_SHOW_DLG)	// base class opens/closes auto dlg
		CAutoSliderCtrl::OnLButtonDown(nFlags, point);
	else {
		if (IsPlaying()) {
			NotifyUndoableEdit(UNDO_POSITION);
			SetFocus();		// because we don't call base class
			Trigger();
		} else	// call base of base to avoid quick start
			CSelectSliderCtrl::OnLButtonDown(nFlags, point);
	}
}
