// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      12dec03 initial version
		01		11jan04	add HasSelection, ClearSel
		02		17jan04	increase default time to five seconds
		03		29jan04	add WithinSelection
		04		22mar04	in SetCourse, handle no selection
		05		31dec04	make waveform variable 

		automation information package; atomic data only
 
*/

#include "stdafx.h"
#include "AutoInfo.h"

CAutoInfo::CAutoInfo() :
	m_Pos(0),
	m_StartPos(-1),
	m_EndPos(-1),
	m_Time(5000),
	m_TimeUnit(0),
	m_Reverse(0),
	m_Transport(STOP),
	m_Loop(0),
	m_Waveform(TRIANGLE)
{
}

void CAutoInfo::Serialize(CArchive& ar, int Version)
{
	if (ar.IsStoring()) {
		ar
			<< m_Pos
			<< m_StartPos
			<< m_EndPos
			<< m_Time
			<< m_TimeUnit
			<< m_Reverse
			<< m_Transport
			<< m_Loop
			<< m_Waveform
			;
	} else {
		ar
			>> m_Pos
			>> m_StartPos
			>> m_EndPos
			>> m_Time
			>> m_TimeUnit
			>> m_Reverse
			>> m_Transport
			>> m_Loop
			>> m_Waveform
			;
	}
}

void CAutoInfo::SetCourse()
{
	// if no selection, use full range
	float	Start = GetStart();
	float	End = GetEnd();
	switch (m_Waveform) {
	case RAMP_UP:
		m_Pos = Start;
		m_Reverse = 0;
		break;
	case RAMP_DOWN:
		m_Pos = End;
		m_Reverse = 1;
		break;
	default:
		// clamp position within limits
		if (m_Pos < Start)
			m_Pos = Start;
		else if (m_Pos > End)
			m_Pos = End;
		// head for whichever limit is further away
		m_Reverse = (End - m_Pos < m_Pos - Start);
	}
}

void CAutoInfo::ClearSel()
{
	m_EndPos = m_StartPos = -1;
}
