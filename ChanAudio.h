// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      05dec03 initial version

		customize audio class for channel
 
*/

#ifndef CCHANAUDIO_INCLUDED
#define	CCHANAUDIO_INCLUDED

#include "Audio.h"

class CChannel;

class CChanAudio : public CAudio
{
public:
	CChanAudio();
	~CChanAudio();
	void	SetChannel(CChannel *Channel);
	void	SetVolume(float Volume);
	float	GetVolume() const;
	void	UpdateVolume();

private:
	CChannel	*m_Channel;
	float	m_Volume;
	NOCOPIES(CChanAudio);
};

inline float CChanAudio::GetVolume() const
{
	return(m_Volume);
}

#endif
