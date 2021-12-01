// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      19jan04	initial version

		manage multiple mixers
 
*/

#ifndef CMULTIMIX_INCLUDED
#define CMULTIMIX_INCLUDED

#include <afxtempl.h>
#include "OptionsDlg.h"
#include "VolumeBar.h"
#include "CrossfaderBar.h"

class CMainFrame;
class CMixereView;

class CMultiMix
{
public:
// Construction
	CMultiMix(CMainFrame *Frame);
	~CMultiMix();

// Constants
	enum {	// dialog bars: must match ctor's init of m_DlgBar
		DLGBAR_MASTERVOL,
		DLGBAR_CROSSFADER,
		DLGBARS
	};

// Attributes
	int		GetMixerCount() const;
	CMixereView	*GetMixer(int Index);
	CMixereView	*GetActiveMixer();
	float	GetMasterVolume() const;
	float	GetMasterVolume(CMixereView *Mixer);
	void	SetMasterVolume(float Volume);
	bool	IsDlgBarVisible(int BarIdx) const;
	bool	HasToolTips() const;

// Operations
	void	Create();
	void	Destroy();
	void	AddMixer(CMixereView *Mixer);
	bool	RemoveMixer(CMixereView *Mixer);
	bool	UpdateMixerName(CMixereView *Mixer);
	void	TimerHook();
	void	ShowOptionsDlg();
	void	ShowDlgBar(int BarIdx, bool Enable);
	void	ToggleDlgBar(int BarIdx);
	void	UpdateVolume();
	BOOL	EnableToolTips(BOOL bEnable);

private:
// Nested classes
	class CMasterVolBar : public CVolumeBar {
	public:
		CMasterVolBar(CMultiMix *MultiMix);
		void	OnPosChange();

	private:
		CMultiMix	*m_MultiMix;
	};
	class CMyCrossfaderBar : public CCrossfaderBar {
	public:
		CMyCrossfaderBar(CMultiMix *MultiMix);
		LPCSTR	GetSourceName(PVOID Source);
		void	OnPosChange();

	private:
		CMultiMix	*m_MultiMix;
	};

// Constants
	static	const int	m_TimerPeriod;
	static	LPCSTR	MASTER_VOL_AUTO;
	static	LPCSTR	CROSSFADER_TYPE;
	static	LPCSTR	CROSSFADER_AUTO;
	static	const int	m_DlgBarResID[DLGBARS];

// Member data
	CArray<CMixereView *, CMixereView *>	m_Mixer;
	CMainFrame	*m_Frm;
	int		m_Timer;
	COptionsDlg	m_OptionsDlg;
	CMasterVolBar	m_MasterVolBar;
	CMyCrossfaderBar	m_CrossfaderBar;
	CDialogBarEx	*m_DlgBar[DLGBARS];

// Helpers
	int		FindMixer(CMixereView *Mixer) const;
	void	ApplyOptions(CMixereView *Mixer);
	bool	GetAutoBar(LPCTSTR Entry, CAutoSliderDlgBar& Bar);
	void	WriteAutoBar(LPCTSTR Entry, const CAutoSliderDlgBar& Bar);

	NOCOPIES(CMultiMix);
};

inline int CMultiMix::GetMixerCount() const
{
	return(m_Mixer.GetSize());
}

inline float CMultiMix::GetMasterVolume() const
{
	return(m_MasterVolBar.GetVolume());
}

inline bool CMultiMix::HasToolTips() const
{
	return(m_OptionsDlg.ShowToolTips());
}

#endif
