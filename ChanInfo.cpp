// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      21nov03 initial version
		01		05dec03	add optional version number to serialize
		02		26dec03	add automation
		03		17jan04	add mute/solo
		04		31jan04	add title
		05		02feb04	add auto trigger
		06		08feb04	add reset
		07		23feb04	add index for clipboard undo; not archived
		08		23dec04	consolidate file versions
		09		10jan05	add ID
		10		24feb05	for auto-trigger, force automation looping on

		channel information package; atomic data only
 
*/

#include "stdafx.h"
#include "ChanInfo.h"

CChanInfo::CChanInfo() :
	m_ID(0),
	m_Transport(STOP),
	m_Loop(0),
	m_Mute(0),
	m_Solo(0),
	m_Volume(0),
	m_Pan(0),
	m_Pitch(1),
	m_Pos(0),
	m_Start(0),
	m_End(0),
	m_Count(0)
{
	m_Path[0] = 0;
	m_Path[MAX_PATH] = 0;	// in case of truncation
	m_Title[0] = 0;
	m_Title[MAX_TITLE] = 0;	// in case of truncation
	m_Auto[AUTO_PAN].m_Pos = .5;	// normalized m_Pan
	m_Auto[AUTO_PITCH].m_Pos = .5;	// normalized m_Pitch
	m_Auto[AUTO_POS].m_Loop = TRUE;	// auto-trigger is always looped
}

void CChanInfo::Reset()
{
	static const CChanInfo Default;
	*this = Default;
}

void CChanInfo::Serialize(CArchive& ar, int Version)
{
	if (ar.IsStoring()) {
		ar << CString(m_Path);
		if (!IsEmpty()) {
			ar
				<< CString(m_Title)
				<< m_ID
				<< m_Transport
				<< m_Loop
				<< m_Mute
				<< m_Solo
				<< m_Volume
				<< m_Pan
				<< m_Pitch
				<< m_Pos
				<< m_Start
				<< m_End
				<< m_Count
				;
		}
	} else {
		CString	Path;
		ar >> Path;
		strncpy(m_Path, Path, MAX_PATH);
		if (!IsEmpty()) {
			CString	Title;
			ar
				>> Title
				>> m_ID
				>> m_Transport
				>> m_Loop
				>> m_Mute
				>> m_Solo
				>> m_Volume
				>> m_Pan
				>> m_Pitch
				>> m_Pos
				>> m_Start
				>> m_End
				>> m_Count
				;
			strncpy(m_Title, Title, MAX_TITLE);
		}
	}
	if (!IsEmpty()) {
		for (int i = 0; i < AUTOS; i++)
			m_Auto[i].Serialize(ar, Version);
	}
}
