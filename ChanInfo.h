// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      21nov03 initial version
		01		12dec03	add automation
		02		02feb04	add auto trigger

		channel information package; atomic data only
 
*/

#ifndef CCHANINFO_INCLUDED
#define	CCHANINFO_INCLUDED

#include "AutoInfo.h"
#include "FormListInfo.h"

class CChanInfo : public CFormListInfo {
public:
	enum AUTO_TYPE {
		AUTO_VOL,
		AUTO_PAN,
		AUTO_PITCH,
		AUTO_POS,
		AUTOS
	};
	enum TRANSPORT {
		STOP,
		PAUSE,
		PLAY,
		TRANSPORT_STATES
	};
	enum {
		MAX_TITLE = 64
	};
	char	m_Path[MAX_PATH + 1];	// path to audio file; may be empty
	char	m_Title[MAX_TITLE + 1];	// channel title; may be empty
	DWORD	m_ID;			// if non-zero, unique identifier
	BYTE	m_Transport;	// 0 == stop, 1 == pause, 2 == play
	BYTE	m_Loop;			// non-zero if looping sample
	BYTE	m_Mute;			// non-zero if muted
	BYTE	m_Solo;			// non-zero if soloed
	float	m_Volume;		// 0 == silence, 1 == maximum volume
	float	m_Pan;			// -1 == left, 0 == center, 1 == right
	float	m_Pitch;		// 0.5 == octave down, 1 == no shift, 2 == octave up
	int		m_Pos;			// current position in frames
	int		m_Start;		// selection start position in frames
	int		m_End;			// selection end position in frames
	int		m_Count;		// 0 == no selection, -1 == has selection
	CAutoInfo	m_Auto[AUTOS];	// automation states

	CChanInfo();
	void	Reset();
	void	Serialize(CArchive& ar, int Version);
	bool	IsEmpty() const;
	bool	IsPlaying() const;
};

inline bool CChanInfo::IsEmpty() const
{
	return(!m_Path[0]);
}

inline bool CChanInfo::IsPlaying() const
{
	return(m_Transport == PLAY);
}

#endif
