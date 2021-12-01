// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      28sep04	initial version

        time edit control
 
*/

// TimeEdit.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "TimeEdit.h"
#include "AutoInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTimeEdit

// numerators for milliseconds, 1/100ths, seconds, minutes
const int CTimeEdit::m_Numer[PLACES] = {10, 1000, 60000, 3600000};

CTimeEdit::CTimeEdit()
{
	m_Time = 0;
	m_Unit = 0;
}

CTimeEdit::~CTimeEdit()
{
}

void CTimeEdit::SetTime(int Time)
{
	m_Time = Time;
	SetText();
}

int CTimeEdit::GetPos() const
{
	return(m_Time / m_Numer[m_Unit + 1]);
}

void CTimeEdit::SetPos(int Pos)
{
	m_Time = Pos * m_Numer[m_Unit + 1];
	SetText();
}

void CTimeEdit::SetUnit(int Unit)
{
	int	Pos = GetPos();
	m_Unit = Unit;
	SetPos(Pos);
}

void CTimeEdit::GetInfo(CAutoInfo& Info) const
{
	Info.m_Time = m_Time;
	Info.m_TimeUnit = m_Unit;
}

void CTimeEdit::SetInfo(const CAutoInfo& Info)
{
	m_Time = Info.m_Time;
	m_Unit = Info.m_TimeUnit;
	SetText();
}

void CTimeEdit::AddSpin(int Delta)
{
	GetText();	// freshen value before incrementing it
	m_Time += Delta * m_Numer[m_Unit];
	m_Time = ClampTime(m_Time);
	SetText();
	NotifyParent();
	SetModify();	// otherwise kill focus cancels edit
}

void CTimeEdit::StrToVal(LPCSTR Str)
{
	m_Time = ClampTime(StrToTime(Str));
}

void CTimeEdit::ValToStr(CString& Str)
{
	TimeToStr(m_Time, Str);
}

int CTimeEdit::StrToTime(LPCSTR Str) const
{
	int pl[PLACES];
	// allow spaces as separators
	int	fields = sscanf(Str, "%d%*[: ]%d%*[: ]%d%*[. ]%d",
		&pl[0], &pl[1], &pl[2], &pl[3]);
	int	time = 0;
	for (int i = 0; i < PLACES; i++) {
		if (fields > i)
			time += pl[i] * m_Numer[PLACES - 1 - i];
	}
	return(time);
}

void CTimeEdit::TimeToStr(int Time, CString& Str) const
{
	int	pl[PLACES];
	div_t	qr;
	qr.rem = Time;
	for (int i = 0; i < PLACES; i++) {
		qr = div(qr.rem, m_Numer[PLACES - 1 - i]);
		pl[i] = qr.quot;
	}
	Str.Format("%d:%02d:%02d.%02d", pl[0], pl[1], pl[2], pl[3]);
}

int CTimeEdit::ClampTime(int Time) const
{
	// avoid wraparound, clamp time to 100 hours
	return(max(min(Time, m_Numer[PLACES - 1] * 100), 0));
}

int CTimeEdit::GetAppropriateUnit(int Time)
{
	int i;
	for (i = 0; i < UNITS; i++) {
		if (Time / m_Numer[i + 2] == 0)
			break;
	}
	return(min(i, UNITS - 1));
}

void CTimeEdit::SaveUndoState(CUndoState& State)
{
	State.m_Val.x.i = m_Time;
	State.m_Val.y.i = m_Unit;
}

void CTimeEdit::RestoreUndoState(const CUndoState& State)
{
	m_Time = State.m_Val.x.i;
	m_Unit = State.m_Val.y.i;
	CNumEdit::RestoreUndoState(State);
}

BEGIN_MESSAGE_MAP(CTimeEdit, CNumEdit)
	//{{AFX_MSG_MAP(CTimeEdit)
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTimeEdit message handlers

void CTimeEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch (nChar) {
	case ':':
	case '.':
	case ' ':
	case VK_BACK:
		break;
	default:
		if (!(isdigit(nChar) || iscntrl(nChar)))
			return;
	}
	// don't want CNumEdit to re-validate, so call CEdit directly
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

void CTimeEdit::OnKillfocus()
{
	if (GetModify()) {
		GetText();
		m_Unit = GetAppropriateUnit(m_Time);
	}
	CNumEdit::OnKillfocus();
}
