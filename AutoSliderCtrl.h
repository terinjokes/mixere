// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      26dec03 initial version

		slider with automation support
 
*/

#if !defined(AFX_AUTOSLIDERCTRL_H__03E1B7AD_4969_4091_B6E1_CB470C63B395__INCLUDED_)
#define AFX_AUTOSLIDERCTRL_H__03E1B7AD_4969_4091_B6E1_CB470C63B395__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AutoSliderCtrl.h : header file
//

#include "SelectSliderCtrl.h"
#include "AutoDlg.h"

// non-standard notification codes
#define	AUTOSLIDER_STOP		1000	// sent when stop is pressed
#define	AUTOSLIDER_PAUSE	1001	// sent when pause is pressed
#define	AUTOSLIDER_PLAY		1002	// sent when play is pressed
#define	AUTOSLIDER_UNSET	1003	// sent when transport is unset
#define	AUTOSLIDER_CREATE	1004	// sent when dialog is created
#define	AUTOSLIDER_CLOSE	1005	// sent when dialog is closed
#define	AUTOSLIDER_ACTIVATE	1006	// sent when dialog is activated
#define	AUTOSLIDER_MODIFY	1007	// sent when dialog is modified
#define	AUTOSLIDER_GO		1008	// sent when go is signaled

/////////////////////////////////////////////////////////////////////////////
// CAutoSliderCtrl window

class CAutoSliderCtrl : public CSelectSliderCtrl
{
public:
// Construction
	CAutoSliderCtrl();
	virtual ~CAutoSliderCtrl();

// Constants
	enum TRANSPORT {	// transport states
		STOP	= CAutoInfo::STOP,
		PAUSE	= CAutoInfo::PAUSE,
		PLAY	= CAutoInfo::PLAY,
		UNSET	= CAutoInfo::UNSET,
		TRANSPORT_STATES	= CAutoInfo::TRANSPORT_STATES
	};
	enum OPTION {		// same for all auto-sliders
		MCLICK_SHOWS_DLG	= 0x01,	// middle-click shows auto dialog
		DEFAULT_OPTIONS		= MCLICK_SHOWS_DLG
	};
	enum AUTO_STYLE {	// auto-slider styles
		AUTO_DIALOG			= 0x01,	// enable automation dialog
		AUTO_DLG_WAVEFORM	= 0x02,	// enable auto dialog's waveform combo box
		AUTO_DLG_LOOP		= 0x04	// enable auto dialog's loop button
	};

// Attributes
	void	GetInfo(CAutoInfo& Info) const;
	void	SetInfo(const CAutoInfo& Info);
	void	SetInfoNotify(const CAutoInfo& Info);
	int		GetTime() const;
	void	SetTime(int Time);
	int		GetTransport() const;
	void	SetTransport(int Transport);
	bool	IsPlaying() const;
	bool	GetLoop() const;
	void	SetLoop(bool Enable);
	void	SetSelection(int nMin, int nMax);
	void	GetSelection(int& nMin, int& nMax) const;
 	void	SetNormSelection(float fMin, float fMax);
	void	GetNormSelection(float& fMin, float& fMax) const;
 	int		GetPos() const;
 	void	SetPos(int Pos);
	double	GetNormPos() const;
	void	SetNormPos(double Pos);
	bool	GetReverse() const;
	void	SetReverse(bool Reverse);
	int		GetWaveform() const;
	void	SetWaveform(int Waveform);
 	bool	IsAutoDlgVisible() const;
	bool	IsSelectionActive() const;
	PVOID	GetCookie() const;
	void	SetCookie(PVOID Cookie);
	int		GetAutoStyle() const;
	void	SetAutoStyle(int Style);
	static	void	SetAnimDelay(int Delay);
	static	int		GetAnimDelay();
	static	bool	IsAnimated();
	static	void	SetOptions(int Options);
	static	int		GetOptions();

// Operations
	void	Go();
	void	ShowAutoDlg(bool Enable, bool Undoable = TRUE);
	void	ToggleAutoDlg();
	bool	TimerHook(float Scale = 1);
	void	SetAutoDlgParent(CWnd *Parent);
	void	SetAutoDlgTitle(LPCSTR Title);
	BOOL	EnableToolTips(BOOL bEnable);
	static	void	ResetAnimCount();
	static	bool	IsKeyDown(int VKey);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoSliderCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:
// Generated message map functions
	//{{AFX_MSG(CAutoSliderCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnToggleAutoDlg();
	afx_msg void HScroll(UINT nSBCode, UINT nPos);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Nested classes
	class CMyAutoDlg : public CAutoDlg {
	public:
		CMyAutoDlg(CAutoSliderCtrl *Slider, CWnd *pParent = NULL);

	private:
		BOOL	OnInitDialog();
		void	OnOK();
		void	OnCancel();
		BOOL	PreTranslateMessage(MSG* pMsg);
		void	OnTransport(int Transport);
		void	OnGo();
		BOOL	OnNcActivate(BOOL bActive);
		void	OnModify();
		void	OnNewPos();
		void	SaveUndoState(CUndoState& State);
		void	RestoreUndoState(const CUndoState& State);
		CAutoSliderCtrl	*m_Slider;
	};

// Types
	typedef struct {
		float	Pos;
		float	Start;
		float	End;
		BYTE	Transport;
		BYTE	Reverse;
		bool	ShowDlg;
	} QUICK_START_UNDO_STATE;

// Constants
	enum {
		MKU_SHOW_DLG	= MK_CONTROL
	};
	enum {
		UNDO_SHOW_DLG = 0x8000,	// avoid base undo codes and windows control IDs
		UNDO_TRANSPORT,
		UNDO_QUICK_START
	};
	enum {	// adjust function masks
		AF_INC			= 0,	// increment selection point(s)
		AF_DEC			= 0x01,	// decrement selection point(s)
		AF_GOTO			= 0x02,	// move selection to start/end
		AF_START		= 0x04,	// selection start point
		AF_END			= 0x08,	// selection end point
		AF_BOTH			= AF_START | AF_END	// entire selection
	};
	static const CPen	m_PenDisabled;
	static const CBrush	m_BrStop;
	static const CBrush	m_BrPause;
	static const CBrush	m_BrPlay;
	static const CBrush	*m_BrTransport[TRANSPORT_STATES];
	static const int	m_TransportNotification[TRANSPORT_STATES];
	static const int	m_FineAdjustPrecision;

// Member data
	static	HACCEL	m_Accel;
	static	int		m_AnimDelay;
	static	int		m_AnimCount;
	static	int		m_AnimBase;
	static	int		m_Options;
	CAutoInfo	m_Info;
	CMyAutoDlg	*m_AutoDlg;
	int		m_PrevTransport;
	CWnd	*m_AutoDlgParent;
	BOOL	m_HasToolTips;
	PVOID	m_Cookie;
	int		m_AutoStyle;
	double	m_WavePos;

// Helpers
	void	Notify(int NotificationCode);
	void	AdjSel(int Func);
	void	AdjPos(int Func);
	void	UpdateAutoDlgUI();

// Overridables
	virtual	void	OnSelection();
	virtual	void	OnTransport(int Transport);
	virtual	void	OnNewPos();
	virtual	void	OnDlgSaveUndoState(CUndoState& State);
	virtual	void	OnDlgRestoreUndoState(const CUndoState& State);

// Overrides
	void	SaveUndoState(CUndoState& State);
	void	RestoreUndoState(const CUndoState& State);
	CUndoable	*FindUndoable(CUndoState::UNDO_KEY Key);
	void	GetUndoTitle(const CUndoState& State, CString& Title);

// Friends
	friend	CMyAutoDlg;
	NOCOPIES(CAutoSliderCtrl);
};

inline int CAutoSliderCtrl::GetTime() const
{
	return(m_Info.m_Time);
}

inline int CAutoSliderCtrl::GetTransport() const
{
	return(m_Info.m_Transport);
}

inline bool CAutoSliderCtrl::IsPlaying() const
{
	return(m_Info.m_Transport == PLAY);
}

inline bool CAutoSliderCtrl::GetLoop() const
{
	return(m_Info.m_Loop != 0);
}

inline void CAutoSliderCtrl::GetSelection(int& nMin, int& nMax) const
{
	nMin = NormToSlider(m_Info.m_StartPos);
	nMax = NormToSlider(m_Info.m_EndPos);
}

inline int CAutoSliderCtrl::GetPos() const
{
	return(NormToSlider(m_Info.m_Pos));
}

inline double CAutoSliderCtrl::GetNormPos() const
{
	return(m_Info.IsPlaying() ? m_WavePos : m_Info.m_Pos);
}

inline bool CAutoSliderCtrl::GetReverse() const
{
	return(m_Info.m_Reverse != 0);
}

inline int CAutoSliderCtrl::GetWaveform() const
{
	return(m_Info.m_Waveform);
}

inline bool CAutoSliderCtrl::IsAutoDlgVisible() const
{
	return(m_AutoDlg != NULL && m_AutoDlg->IsWindowVisible());
}

inline bool CAutoSliderCtrl::IsSelectionActive() const
{
	return(m_Info.m_Transport != STOP);
}

inline PVOID CAutoSliderCtrl::GetCookie() const
{
	return(m_Cookie);
}

inline void CAutoSliderCtrl::SetCookie(PVOID Cookie)
{
	m_Cookie = Cookie;
}

inline void CAutoSliderCtrl::SetAnimDelay(int Delay)
{
	m_AnimDelay = Delay;
}

inline int CAutoSliderCtrl::GetAnimDelay()
{
	return(m_AnimDelay);
}

inline bool CAutoSliderCtrl::IsAnimated()
{
	return(m_AnimDelay ? !(m_AnimCount++ % (m_AnimDelay + 1)) : TRUE);
}

inline void CAutoSliderCtrl::ResetAnimCount()
{
	m_AnimCount = m_AnimBase++;
}

inline void CAutoSliderCtrl::GetNormSelection(float& fMin, float& fMax) const
{
	fMin = m_Info.m_StartPos;
	fMax = m_Info.m_EndPos;
}

inline bool CAutoSliderCtrl::IsKeyDown(int VKey)
{
 	return((GetAsyncKeyState(VKey) & 0x80000000) != 0);
}

inline int CAutoSliderCtrl::GetAutoStyle() const
{
	return(m_AutoStyle);
}

inline void CAutoSliderCtrl::SetAutoStyle(int Style)
{
	m_AutoStyle = Style;
}

inline void CAutoSliderCtrl::SetOptions(int Options)
{
	m_Options = Options;
}

inline int CAutoSliderCtrl::GetOptions()
{
	return(m_Options);
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOSLIDERCTRL_H__03E1B7AD_4969_4091_B6E1_CB470C63B395__INCLUDED_)
