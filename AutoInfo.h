// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      12dec03 initial version
        01      26sep04	HasSelection test was reversed

		automation information package; atomic data only
 
*/

#ifndef CAUTOINFO_INCLUDED
#define	CAUTOINFO_INCLUDED

class CAutoInfo {
public:
	enum TRANSPORT {
		STOP,
		PAUSE,
		PLAY,
		UNSET,
		TRANSPORT_STATES
	};
	enum WAVEFORM {
		TRIANGLE,
		RAMP_UP,
		RAMP_DOWN,
		SQUARE,
		RANDOM,
		WAVEFORMS
	};
	double	m_Pos;			// current position, from 0 to 1
	float	m_StartPos;		// initial position, from 0 to 1, or -1 if no selection
	float	m_EndPos;		// final position, from 0 to 1, or -1 if no selection
	int		m_Time;			// automation time in milliseconds
	BYTE	m_TimeUnit;		// 0 == seconds, 1 == minutes
	BYTE	m_Reverse;		// 0 == forward, 1 == reverse
	BYTE	m_Transport;	// 0 == stop, 1 == pause, 2 == play, 3 == unset
	BYTE	m_Loop;			// non-zero to loop automation
	BYTE	m_Waveform;		// see waveform enum above
	CAutoInfo();
	float	GetStart() const;
	float	GetEnd() const;
	float	GetLength() const;
	bool	AtStart() const;
	bool	AtEnd() const;
	bool	HasSelection() const;
	bool	WithinSelection() const;
	bool	IsPlaying() const;
	void	Serialize(CArchive& ar, int Version);
	void	SetCourse();
	void	ClearSel();
};

inline float CAutoInfo::GetStart() const
{
	return(m_StartPos >= 0 ? m_StartPos : 0);
}

inline float CAutoInfo::GetEnd() const
{
	return(m_EndPos >= 0 ? m_EndPos : 1);
}

inline float CAutoInfo::GetLength() const
{
	return(GetEnd() - GetStart());
}

inline bool CAutoInfo::AtStart() const
{
	return(m_Pos <= GetStart());
}

inline bool CAutoInfo::AtEnd() const
{
	return(m_Pos >= GetEnd());
}

inline bool CAutoInfo::HasSelection() const
{
	return(!(m_EndPos == -1 && m_StartPos == -1));
}

inline bool CAutoInfo::WithinSelection() const
{
	return(m_Pos >= m_StartPos && m_Pos <= m_EndPos);
}

inline bool CAutoInfo::IsPlaying() const
{
	return(m_Transport == PLAY);
}

#endif
