// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      21nov03 initial version
		01		02dec03	partial fix for loop position problem
		02		11dec03	add get device list and get/set device
		03		11dec03	don't allow set device while sounds are open
		04		08jan04	add frames/seconds conversions
		05		20jan04	SetPosition clamps position to length now

		wrap the Audiere audio DLL
 
*/

#include "stdafx.h"
#include "Audio.h"
#include "audiere.h"
#include "math.h"

using namespace std;
using namespace audiere;

AudioDevicePtr	CAudio::m_Device;
int CAudio::m_OpenSounds;

#define	SOUND_CHECK { if (!m_Sound) return; }
#define	SOUND_CHECK_RET0 { if (!m_Sound) return(0); }

CAudio::CAudio()
{
	m_Channels = m_SampleRate = m_Format = 0;
}

CAudio::~CAudio()
{
	Close();
}

bool CAudio::Open(LPCSTR Path)
{
	if (!m_Device) {
		// open the default audio device
		m_Device = OpenDevice("");
		if (!m_Device)
			return(FALSE);
	}
	OutputStreamPtr	sound;
	SampleSourcePtr	sample;
	// try to create loop point sample source from file
	LoopPointSourcePtr	loop = CreateLoopPointSource(Path);
	if (loop) {
		// create sound from loop source; request streaming
		sample = loop.get();
		sound = OpenSound(m_Device, sample, TRUE);
	} else {
		// try to create ordinary sample source from file
		sample = OpenSampleSource(Path);
		if (!sample)
			return(FALSE);
		// create sound from sample source; request streaming
		sound = OpenSound(m_Device, sample, TRUE);
	}
	if (!sound)
		return(FALSE);
	Stop();
	if (!m_Sound)
		m_OpenSounds++;
	m_Sound = sound;
	m_Loop = loop;
	SampleFormat	sfmt;
	sample->getFormat(m_Channels, m_SampleRate, sfmt);
	m_Format = sfmt;
	return(TRUE);
}

void CAudio::Close()
{
	if (m_Sound)
		m_OpenSounds--;
	m_Loop = NULL;
	m_Sound = NULL;
}

void CAudio::GetFormat(int& Channels, int& SampleRate, int& Format) const
{
	Channels = m_Channels;
	SampleRate = m_SampleRate;
	Format = m_Format;
}

void CAudio::Play()
{
	SOUND_CHECK;
	m_Sound->play();
}

void CAudio::Stop()
{
	SOUND_CHECK;
	if (!m_Sound->isPlaying())
		return;
	// mute volume before stop or next play will glitch
	int	frame = GetPosition();	// works with any source
	float	vol = m_Sound->getVolume();
	m_Sound->setVolume(0);
	Sleep(50);		// give stream a chance to flush
	m_Sound->stop();
	m_Sound->setVolume(vol);
	m_Sound->setPosition(frame);
}

void CAudio::StopDead()
{
	SOUND_CHECK;
	m_Sound->stop();
}

bool CAudio::IsPlaying() const
{
	SOUND_CHECK_RET0;
	return(m_Sound->isPlaying());
}

void CAudio::Reset()
{
	SOUND_CHECK;
	m_Sound->reset();
}

void CAudio::SetRepeat(bool Enable)
{
	SOUND_CHECK;
	// setRepeat resets position, so save and restore it
	int	pos = GetPosition();
	m_Sound->setRepeat(Enable);
	SetPosition(pos);
}

bool CAudio::GetRepeat() const
{
	SOUND_CHECK_RET0;
	return(m_Sound->getRepeat());
}

void CAudio::SetVolume(float Volume)
{
	SOUND_CHECK;
	m_Sound->setVolume(Volume);
}

float CAudio::GetVolume() const
{
	SOUND_CHECK_RET0;
	return(m_Sound->getVolume());
}

void CAudio::SetPan(float Pan)
{
	SOUND_CHECK;
	m_Sound->setPan(Pan);
}

float CAudio::GetPan() const
{
	SOUND_CHECK_RET0;
	return(m_Sound->getPan());
}

void CAudio::SetPitch(float Pitch)
{
	SOUND_CHECK;
	m_Sound->setPitchShift(Pitch);
}

float CAudio::GetPitch() const
{
	SOUND_CHECK_RET0;
	return(m_Sound->getPitchShift());
}

bool CAudio::IsSeekable() const
{
	SOUND_CHECK_RET0;
	return(m_Sound->isSeekable());
}

int	CAudio::GetLength() const
{
	SOUND_CHECK_RET0;
	return(m_Sound->getLength());
}

void CAudio::SetPosition(int Frame)
{
	SOUND_CHECK;
	// clamp position, otherwise initial play may not repeat
	m_Sound->setPosition(min(Frame, GetLength()));
}

int	CAudio::GetPosition() const
{
	SOUND_CHECK_RET0;
	if (!m_Sound->isSeekable())
		return(0);
	int pos = m_Sound->getPosition();	// get sound source position
	if (m_Loop && m_Loop->getLoopPointCount()) {	// if we're looping
		// loop source position is 1 second in the future relative to sound 
		// source position; we can partially fix it by rotating loop source 
		// position backwards 1 second, but we'll still get bogus positions
		// when entering a loop for the first time
		int	Start = 0, End = 0, Count;
		m_Loop->getLoopPoint(0, End, Start, Count);
		pos = m_Loop->getPosition();	// get loop source position
		// WithinLoop doesn't quite work because we're 1 sec in the future
		bool	WithinLoop = pos >= Start && pos <= End;
		// rotate loop source position backwards 1 second
		int	len = WithinLoop ? max(End - Start, 1) : m_Sound->getLength();
		int	origin = WithinLoop ? Start : 0;
		pos -= origin;			// zero-based coords
		pos -= m_SampleRate;	// shift backwards 1 sec
		pos %= len;				// modulus pos by length
		if (pos < 0)			// shift caused negative?
			pos += len;			// y, offset by length
		pos += origin;			// normal coords again
	}
	return(pos);
}

bool CAudio::IsLoopable() const
{
	return(m_Loop);
}

bool CAudio::IsLooped() const
{
	return(m_Loop && m_Loop->getLoopPointCount());
}

bool CAudio::GetLoop(CLoop& Loop) const
{
	if (!m_Loop || !m_Loop->getLoopPointCount()) {
		Loop = CLoop();	// always set params
		return(FALSE);
	}
	// start and end are swapped in audiere
	m_Loop->getLoopPoint(0, Loop.m_End, Loop.m_Start, Loop.m_Count);
	return(TRUE);
}

CAudio::CLoop CAudio::GetLoop() const
{
	CLoop	Loop;
	GetLoop(Loop);
	return(Loop);
}

void CAudio::SetLoop(const CLoop& Loop)
{
	if (!m_Loop)
		return;
	RemoveLoop();
	// start and end are swapped in audiere
	m_Loop->addLoopPoint(Loop.m_End, Loop.m_Start, Loop.m_Count);
}

void CAudio::RemoveLoop()
{
	if (!m_Loop)
		return;
	if (m_Loop->getLoopPointCount())
		m_Loop->removeLoopPoint(0);
}

double CAudio::LogToLinear(double x)
{
	return(pow(2, x));
}

double CAudio::LinearToLog(double x)
{
	return(log(x) / log(2));
}

float CAudio::FramesToSecs(int Frames) const
{
	return(float(Frames) / m_SampleRate);
}

int CAudio::SecsToFrames(float Secs) const
{
	return(int(Secs * m_SampleRate + 0.5));
}

void CAudio::GetDeviceList(CStringArray& List)
{
	vector<AudioDeviceDesc>	s;
	GetSupportedAudioDevices(s);
	List.RemoveAll();
	for (int i = 0; i < s.size(); i++)
		List.Add(s[i].name.c_str());
}

bool CAudio::SetDevice(LPCSTR DeviceName)
{
	if (m_OpenSounds)
		return(FALSE);
	m_Device = OpenDevice(DeviceName);
	return(m_Device);
}

LPCSTR CAudio::GetDevice()
{
	if (!m_Device)
		return(NULL);
	return(m_Device->getName());
}

bool CAudio::IsDeviceInUse()
{
	return(m_OpenSounds > 0);
}
