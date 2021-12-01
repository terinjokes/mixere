// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      18jan04	initial version

		mixer information package
 
*/

#ifndef CMIXERINFO_INCLUDED
#define	CMIXERINFO_INCLUDED

#include <afxtempl.h>
#include <afxadv.h>
#include "ChanInfo.h"
#include "Snapshot.h"

class CMixerInfo {
public:
	static const int FILE_SIG;		// file signature
	static const int FILE_VERSION;	// file format version number
	static const int DEFAULT_CHANNEL_COUNT;
	int		m_Version;
	DWORD	m_ChanIDs;
	CArray<CChanInfo, CChanInfo&>	m_Chan;
	CAutoInfo	m_AutoVol;
	CAutoInfo	m_AutoTempo;
	CChanInfo	m_ChanDefaults;
	CDWordArray	m_ColumnWidth;
	CAutoInfo	m_MSFade;
	CDockState	m_DockState;
	CSnapshot	m_Snapshot;

	CMixerInfo();
	void	Reset();
	void	Serialize(CArchive& ar);
	int		GetChanCount() const;
	bool	VerifyAudioLinks();
	NOCOPIES(CMixerInfo);
};

inline int CMixerInfo::GetChanCount() const
{
	return(m_Chan.GetSize());
}

#endif
