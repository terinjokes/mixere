// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      18jan04	initial version
		01		22jan04	save volume and its automation
		02		24jan04	use auto pos as volume
		03		31jan04	bump version for channel title
		04		02feb04	bump version for auto trigger
		05		08feb04	if missing file isn't found, reset channel
		06		08feb04	save channel defaults, bump version
		07		10feb04	save column widths, bump version
		08		13feb04	reject doc with version number higher than ours
		09		10mar04	add tempo
		10		16apr04	add mute/solo time
		11		28sep04	add dock state
		12		23dec04	consolidate file versions
		13		31dec04	bump version for auto waveform
		14		10jan05	add channel ID, bump version
		15		16jan05	add snapshots, bump version
		16		02feb05	pass channel array to snapshot
		17		03feb05	bump version for snapshot hot key

		mixer information package
 
*/

#include "stdafx.h"
#include "Resource.h"
#include "MixerInfo.h"
#include "Channel.h"
#include "MissingFilesDlg.h"

const int	CMixerInfo::FILE_SIG = *((int *)"MIXR");
const int	CMixerInfo::FILE_VERSION = 15;
const int	CMixerInfo::DEFAULT_CHANNEL_COUNT = 64;

CMixerInfo::CMixerInfo()
{
	m_Version = 0;
	m_ChanIDs = 0;
	m_AutoVol.m_Pos = 1;	// default volume
	m_AutoTempo.m_Pos = .5;	// default tempo
	m_MSFade.m_Time = 0;	// default fade time
}

void CMixerInfo::Reset()
{
	int	count = DEFAULT_CHANNEL_COUNT;
	m_Chan.SetSize(count);
	for (int i = 0; i < count; i++)
		m_Chan[i].Reset();
}

void CMixerInfo::Serialize(CArchive& ar)
{
	m_Version = FILE_VERSION;
	if (ar.IsStoring()) {
		ar << FILE_SIG;
		ar << m_Version;
		ar << m_ChanIDs;
		ar << GetChanCount();
		for (int i = 0; i < GetChanCount(); i++)
			m_Chan[i].Serialize(ar, m_Version);
	} else {
		// read header and check signature
		int	Count, FileSig;
		ar >> FileSig;
		ar >> m_Version;
		if (FileSig != FILE_SIG || m_Version > FILE_VERSION)
			AfxThrowArchiveException(CArchiveException::badIndex);
		ar >> m_ChanIDs;
		ar >> Count;
		// read channels into our array
		m_Chan.SetSize(Count);
		for (int i = 0; i < Count; i++)
			m_Chan[i].Serialize(ar, m_Version);
		if (!VerifyAudioLinks())
			AfxThrowArchiveException(CArchiveException::none);
	}
	m_AutoVol.Serialize(ar, m_Version);
	m_ChanDefaults.Serialize(ar, m_Version);
	m_ColumnWidth.Serialize(ar);
	m_AutoTempo.Serialize(ar, m_Version);
	m_MSFade.Serialize(ar, m_Version);
	m_DockState.Serialize(ar);
	m_Snapshot.Serialize(ar, m_Version, m_Chan.GetData(), m_Chan.GetSize());
}

bool CMixerInfo::VerifyAudioLinks()
{
	int	Count = m_Chan.GetSize();
	CStringArray	Path;
	Path.SetSize(Count);
	for (int i = 0; i < Count; i++)
		Path[i] = m_Chan[i].m_Path;
	CMissingFilesDlg	mfd(Path, CChannel::GetFileFilter());
	switch (mfd.Check()) {
	case IDOK:
	case IDIGNORE:
		break;
	default:
		return(FALSE);
	}
	// store the corrected links
	for (i = 0; i < Count; i++) {
		if (!m_Chan[i].IsEmpty()) {	// if channel not empty
			if (Path[i].IsEmpty())		// and file not found
				m_Chan[i].Reset();			// reset channel
			else
				strncpy(m_Chan[i].m_Path, Path[i], MAX_PATH);
		}
	}
	return(TRUE);
}
