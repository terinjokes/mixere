// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
		00      21nov03 initial version
		01		26jan04	add normalization functions

		wrap the Audiere audio DLL
 
*/

#ifndef CAUDIO_INCLUDED
#define	CAUDIO_INCLUDED

#include "audiere.h"
using namespace audiere;

class CAudio
{
public:
	class CLoop {
	public:
		CLoop();
		CLoop(int Start, int End, int Count = -1);
		int		Start() const;
		int		End() const;
		int		Count() const;
		bool	operator==(const CLoop& Loop) const;
		bool	operator!=(const CLoop& Loop) const;
    
	private:
		int		m_Start;
		int		m_End;
		int		m_Count;
		friend	CAudio;
	};
	CAudio();
	~CAudio();

// Operations
	bool	Open(LPCSTR Path);
	void	Close();
	void	GetFormat(int& Channels, int& SampleRate, int& Format) const;
	void	Play();
	void	Stop();
	void	StopDead();
	void	Reset();
	void	RemoveLoop();

// Attributes
	bool	IsLoaded() const;
	bool	IsPlaying() const;
	void	SetRepeat(bool Enable);
	bool	GetRepeat() const;
	void	SetVolume(float Volume);
	float	GetVolume() const;
	void	SetPan(float Pan);
	float	GetPan() const;
	void	SetPitch(float Pitch);
	float	GetPitch() const;
	bool	IsSeekable() const;
	int		GetLength() const;
	void	SetPosition(int Frame);
	int		GetPosition() const;
	bool	IsLoopable() const;
	bool	IsLooped() const;
	bool	GetLoop(CLoop& Loop) const;
	CLoop	GetLoop() const;
	void	SetLoop(const CLoop& Loop);

// Conversions
	float	NormToVol(float Norm) const;
	float	VolToNorm(float Vol) const;
	float	NormToPan(float Norm) const;
	float	PanToNorm(float Pan) const;
	float	NormToPitch(float Norm) const;
	float	PitchToNorm(float Pitch) const;
	float	NormToFrame(float Norm) const;
	float	FrameToNorm(float Frame) const;
	float	FramesToSecs(int Frames) const;
	int		SecsToFrames(float Secs) const;
	static	double	LogToLinear(double x);	// for normal to pitch
	static	double	LinearToLog(double x);	// for pitch to normal

// Device handling
	static	void	GetDeviceList(CStringArray& List);
	static	LPCSTR	GetDevice();
	static	bool	SetDevice(LPCSTR DeviceName);
	static	bool	IsDeviceInUse();

private:
	OutputStreamPtr	m_Sound;
	LoopPointSourcePtr	m_Loop;
	static	AudioDevicePtr	m_Device;
	static	int	m_OpenSounds;
	int		m_Channels;
	int		m_SampleRate;
	int		m_Format;
	NOCOPIES(CAudio);
};

inline CAudio::CLoop::CLoop() :
	m_Start(0), m_End(0), m_Count(0)
{
}

inline CAudio::CLoop::CLoop(int Start, int End, int Count) :
	m_Start(Start), m_End(End), m_Count(Count)
{
}

inline int CAudio::CLoop::Start() const
{
	return(m_Start);
}

inline int CAudio::CLoop::End() const
{
	return(m_End);
}

inline int CAudio::CLoop::Count() const
{
	return(m_Count);
}

inline bool CAudio::CLoop::operator==(const CLoop& Loop) const
{
	return(Loop.m_Start == m_Start && Loop.m_End == m_End 
		&& Loop.m_Count == m_Count);
}

inline bool CAudio::CLoop::operator!=(const CLoop& Loop) const
{
	return(!(Loop == *this));
}

inline bool CAudio::IsLoaded() const
{
	return(m_Sound != 0);
}

inline float CAudio::NormToVol(float Norm) const
{
	return(Norm);
}

inline float CAudio::VolToNorm(float Volume) const
{
	return(Volume);
}

inline float CAudio::NormToPan(float Norm) const
{
	return(Norm * 2 - 1);
}

inline float CAudio::PanToNorm(float Pan) const
{
	return((Pan + 1) / 2);
}

inline float CAudio::NormToPitch(float Norm) const
{
	return(LogToLinear(Norm * 2 - 1));
}

inline float CAudio::PitchToNorm(float Pitch) const
{
	return((LinearToLog(Pitch) + 1) / 2);
}

inline float CAudio::NormToFrame(float Norm) const
{
	return(Norm * (GetLength() - 1));
}

inline float CAudio::FrameToNorm(float Frame) const
{
	return(Frame / max((float(GetLength()) - 1), 1));
}

#endif
