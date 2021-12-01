// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      21nov03 initial version
		01		26dec03	add automation
		02		17jan04	add mute/solo

		channel control
 
*/

#if !defined(AFX_CHANNEL_H__E3CCBDCD_5996_4CFF_8B0E_24DC9F7A1E2C__INCLUDED_)
#define AFX_CHANNEL_H__E3CCBDCD_5996_4CFF_8B0E_24DC9F7A1E2C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Channel.h : header file
//

#include "Resource.h"
#include "FormListItemDlg.h"
#include "ChanInfo.h"
#include "AutoSliderCtrl.h"
#include "AutoSliderDlg.h"
#include "IconButton.h"
#include "ChanAudio.h"
#include "ChildToolTip.h"
#include "StaticEdit.h"
#include "AutoTriggerCtrl.h"

class CMixereView;

/////////////////////////////////////////////////////////////////////////////
// CChannel dialog

class CChannel : public CFormListItemDlg
{
public:
// Construction
	CChannel(CMixereView& Mixer);
	virtual ~CChannel();

// Types
	typedef struct {
		COLORREF	Active;
		COLORREF	Selected;
	} COLOR_SCHEME;

// Constants
	enum TRANSPORT {	// transport states
		STOP	= CChanInfo::STOP,
		PAUSE	= CChanInfo::PAUSE,
		PLAY	= CChanInfo::PLAY,
		TRANSPORT_STATES	= CChanInfo::TRANSPORT_STATES
	};
	enum AUTO_TYPE {	// auto slider types
		AUTO_VOL		= CChanInfo::AUTO_VOL,
		AUTO_PAN		= CChanInfo::AUTO_PAN,
		AUTO_PITCH		= CChanInfo::AUTO_PITCH,
		AUTO_POS		= CChanInfo::AUTO_POS,
		AUTOS			= CChanInfo::AUTOS
	};
	enum OPTION {		// same for all channels
		AUTO_VOL_UNPAUSE	= 0x01,	// automate volume unpauses audio
		AUTO_TRIG_LOOP_OFF	= 0x02,	// auto-trigger turns off audio looping
		DEFAULT_OPTIONS		= AUTO_TRIG_LOOP_OFF
	};
	enum {
		UNDO_KEY_OFFSET = 0x8000	// offset index above control ID range
	};

// Attributes
	LPCSTR	GetFilePath() const;
	LPCSTR	GetFileName() const;
	LPCSTR	GetTitle() const;
	void	SetTitle(LPCSTR Title);
	DWORD	GetID() const;
	int		GetTransport() const;
	void	SetTransport(int Transport);
	void	SetTransportAndPos(int Transport, int Frame);
	bool	IsPlaying() const;
	bool	TogglePlay();
	bool	IsRewound() const;
	void	Rewind();
	bool	GetLoop() const;
	void	SetLoop(bool Enable);
	bool	GetMute() const;
	void	SetMute(bool Enable);
	bool	GetSolo() const;
	void	SetSolo(bool Enable);
	float	GetVolume() const;
	void	SetVolume(float Volume);
	float	GetPan() const;
	void	SetPan(float Pan);
	float	GetPitch() const;
	void	SetPitch(float Pitch);
	int		GetPosition() const;
	void	SetPosition(int Frame);
	float	GetNormPosition() const;
	void	GetSelection(CAudio::CLoop& Selection);
	void	SetSelection(const CAudio::CLoop& Selection);
	bool	IsTriggering() const;
	static	LPCSTR	GetFileFilter();
	static	void	GetExtensionList(CStringArray& ExtList);
	CAutoSliderCtrl	*GetAuto(int AutoIdx);
	static	void	GetAutoName(int AutoIdx, CString& Name);
	int		GetMSFadeTime() const;
	void	SetMSFadeTime(int Time);
	bool	GetMSFadeEnable() const;
	void	SetMSFadeEnable(bool Enable);
	static	int		GetOptions();
	static	void	SetOptions(int Options);
	CFont	*GetTitleFont() const;
	void	SetTitleFont(CFont *Font);
	static	void	GetColorScheme(COLOR_SCHEME& Scheme);
	static	void	SetColorScheme(const COLOR_SCHEME& Scheme);

// Operations
	void	Go();
	void	Reset();
	void	TimerHook(float Scale);
	bool	LoadAudio(LPCSTR Path);
	void	CloseAudio();
	void	UpdateVolume();
	BOOL	EnableToolTips(BOOL bEnable);
	void	EditName();
	void	GetAutoTip(char *DestBuf, int AutoIdx) const;
	void	ApplyMuteSolo(bool Fade);
	float	Normalize(int AutoIdx, float Val) const;
	float	Denormalize(int AutoIdx, float Val) const;
	void	ApplyColorScheme();

// Item overrides
	void	SetIndex(int Index);
	void	GetInfo(void *Info) const;
	void	SetInfo(const void *Info);
	bool	OpenItem(const void *Info);
	void	SelectItem(bool Enable);
	void	ActivateItem(bool Enable);

// Handlers
	float	OnUpdateVolume();

protected:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChannel)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
// Types
	typedef struct {
		float	(CChannel::*Normalize)(float Val) const;
		float	(CChannel::*Denormalize)(float Val) const;
	} COORD_FUNC;
	typedef struct {
		int		Position;
		BYTE	Transport;
		BYTE	Loop;
	} TRANSPORT_UNDO_STATE;

// Nested classes
	class CAutoVolumeCtrl	: public CAutoSliderCtrl {
		void	OnNewPos();
		void	OnDlgSaveUndoState(CUndoState& State);
		void	OnDlgRestoreUndoState(const CUndoState& State);
	};
	class CAutoPanCtrl		: public CAutoSliderCtrl {
		void	OnNewPos();
	};
	class CAutoPitchCtrl	: public CAutoSliderCtrl {
		void	OnNewPos();
	};
	class CAutoPosCtrl		: public CAutoTriggerCtrl {
		void	OnNewPos();
		void	OnSelection();
		void	OnTrigger();
		void	OnDlgSaveUndoState(CUndoState& State);
		void	OnDlgRestoreUndoState(const CUndoState& State);
	};

// Dialog data
	//{{AFX_DATA(CChannel)
	enum { IDD = IDD_CHANNEL };
	CStatic	m_Number;
	CStaticEdit	m_Name;
	CIconButton	m_Play;
	CIconButton	m_Pause;
	CIconButton	m_Stop;
	CIconButton	m_Loop;
	CIconButton	m_Mute;
	CIconButton	m_Solo;
	CAutoVolumeCtrl	m_Volume;
	CAutoPitchCtrl	m_Pitch;
	CAutoPanCtrl	m_Pan;
	CAutoPosCtrl	m_Pos;
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CChannel)
	virtual BOOL OnInitDialog();
	afx_msg void OnName();
	afx_msg void OnPlay();
	afx_msg void OnPause();
	afx_msg void OnStop();
	afx_msg void OnLoop();
	afx_msg void OnMute();
	afx_msg void OnSolo();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnNameChange();
	//}}AFX_MSG

// Message map functions
	afx_msg BOOL OnToolTip(UINT id, NMHDR* pTTTStruct, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

// Constants
	enum {
		SLIDER_RANGE	= CAutoSliderDlg::SLIDER_RANGE,
		SLIDER_TICKS	= CAutoSliderDlg::SLIDER_TICKS,
		CONTROLS		= 12,	// must match m_WndList init in ctor
	};
	static	const	UINT	m_AutoNameID[AUTOS];
	static	const	COORD_FUNC	m_CoordFunc[AUTOS];
	static	const	LPCSTR	m_TipFmtf;
	static	const	LPCSTR	m_TipFmtf2;
	static	const	LPCSTR	m_TipFmti;
	static	const	LPCSTR	m_FileFilter;

// Member data
	static	CChanInfo	m_DefaultInfo;
	static	HACCEL	m_Accel;
	static	COLOR_SCHEME	m_Scheme;
	static	CBrush	m_SelectionBrush;
	static	CStringArray	m_ExtList;
	static	int		m_ExtCount;
	static	int		m_Options;
	CMixereView&	m_Mixer;
	CAutoSliderCtrl	*m_Auto[AUTOS];
	CWnd	*m_WndList[CONTROLS + 1];	// one extra for parent
	CChanAudio	m_Audio;
	CString	m_Path;
	CString	m_Title;
	DWORD	m_ID;
	int		m_Transport;
	CRect	m_InitRect;
	bool	m_Muted;
	bool	m_Soloed;
	CChildToolTip	m_ToolTip;
	CAutoInfo	m_MSFader;
	int		m_MSFadeTime;
	bool	m_MSFadeEnable;

// Helpers
	void	ClampAudioPos();
	void	EnableCtrls(bool Enable);
	void	UpdateSliderSelection();
	void	UpdateSolo(bool Enable);
	void	UpdateMute(bool Enable);
	static	int		ParseFileFilter(LPCSTR Filter, CStringArray& ExtList);
	void	OnMute(bool Enable);
	void	OnSolo(bool Enable);
	bool	GetMuteSolo() const;
	void	SaveTransportUndoState(CUndoState& State) const;
	void	RestoreTransportUndoState(const CUndoState& State);

// Conversions
	float	NormVolume(float Val) const;
	float	DenormVolume(float Val) const;
	float	NormPan(float Val) const;
	float	DenormPan(float Val) const;
	float	NormPitch(float Val) const;
	float	DenormPitch(float Val) const;
	float	NormPos(float Val) const;
	float	DenormPos(float Val) const;

// Undo overrides
	void	SaveUndoState(CUndoState& State);
	void	RestoreUndoState(const CUndoState& State);
	CUndoState::UNDO_KEY	GetUndoKey();
	CUndoable	*FindUndoable(CUndoState::UNDO_KEY Key);
	void	GetUndoTitle(const CUndoState& State, CString& Title);

// Item overrides
	WNDLIST	GetWndList();
	void	CloseItem();

// Friends
	friend	CAutoVolumeCtrl;
	friend	CAutoPanCtrl;
	friend	CAutoPitchCtrl;
	friend	CAutoPosCtrl;
	NOCOPIES(CChannel);
};

inline LPCSTR CChannel::GetFilePath() const
{
	return(m_Path);
}

inline LPCSTR CChannel::GetTitle() const
{
	return(m_Title);
}

inline DWORD CChannel::GetID() const
{
	return(m_ID);
}

inline int CChannel::GetTransport() const
{
	return(m_Transport);
}

inline bool CChannel::IsPlaying() const
{
	return(m_Transport == PLAY);
}

inline bool CChannel::GetMute() const
{
	return(m_Muted);
}

inline bool CChannel::GetSolo() const
{
	return(m_Soloed);
}

inline float CChannel::GetNormPosition() const
{
	return(NormPos(GetPosition()));
}

inline LPCSTR CChannel::GetFileFilter()
{
	return(m_FileFilter);
}

inline void CChannel::GetExtensionList(CStringArray& ExtList)
{
	ExtList.Copy(m_ExtList);
}

inline CAutoSliderCtrl *CChannel::GetAuto(int AutoIdx)
{
	ASSERT(AutoIdx >= 0 && AutoIdx < AUTOS);
	return(m_Auto[AutoIdx]);
}

inline bool CChannel::IsTriggering() const
{
	return(m_Pos.IsPlaying());
}

inline int CChannel::GetMSFadeTime() const
{
	return(m_MSFadeTime);
}

inline void CChannel::SetMSFadeTime(int Time)
{
	m_MSFadeTime = Time;
}

inline bool CChannel::GetMSFadeEnable() const
{
	return(m_MSFadeEnable);
}

inline void CChannel::SetMSFadeEnable(bool Enable)
{
	m_MSFadeEnable = Enable;
}

inline int CChannel::GetOptions()
{
	return(m_Options);
}

inline void CChannel::SetOptions(int Options)
{
	m_Options = Options;
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANNEL_H__E3CCBDCD_5996_4CFF_8B0E_24DC9F7A1E2C__INCLUDED_)
