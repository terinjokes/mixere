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

#include "stdafx.h"
#include "ChanAudio.h"
#include "Channel.h"

CChanAudio::CChanAudio()
{
	m_Channel = NULL;
	m_Volume = 0;
}

CChanAudio::~CChanAudio()
{
}

void CChanAudio::SetChannel(CChannel *Channel)
{
	m_Channel = Channel;
}

void CChanAudio::SetVolume(float Volume)
{
	m_Volume = Volume;
	UpdateVolume();
}

void CChanAudio::UpdateVolume()
{
	CAudio::SetVolume(m_Volume * m_Channel->OnUpdateVolume());
}
