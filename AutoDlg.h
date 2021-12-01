// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      21nov03 initial version

        automation dialog
 
*/

#if !defined(AFX_AUTODLG_H__E186F30D_7965_4630_B148_5F48C8C02425__INCLUDED_)
#define AFX_AUTODLG_H__E186F30D_7965_4630_B148_5F48C8C02425__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AutoDlg.h : header file
//

#include "DialogEx.h"
#include "IconButton.h"
#include "ClickSliderCtrl.h"
#include "AutoInfo.h"
#include "ChildToolTip.h"
#include "TimeEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoDlg dialog

class CAutoDlg : public CDialogEx
{
public:
// Construction
	CAutoDlg(UINT nIDTemplate = 0, CWnd* pParent = NULL);
	virtual ~CAutoDlg();

// Constants
	enum TRANSPORT {	// transport states
		STOP	= CAutoInfo::STOP,
		PAUSE	= CAutoInfo::PAUSE,
		PLAY	= CAutoInfo::PLAY,
		UNSET	= CAutoInfo::UNSET,
		TRANSPORT_STATES	= CAutoInfo::TRANSPORT_STATES
	};

// Attributes
	void	GetInfo(CAutoInfo& Info);
	void	SetInfo(const CAutoInfo& Info);
	CAutoInfo	*GetInfo();
	void	SetInfoPtr(CAutoInfo *Info);
	static	void	SetTimerPeriod(int Period);
	static	int		GetTimerPeriod();

// Operations
	static	bool	TimerHook(CAutoInfo& Info, CAutoDlg *AutoDlg, float Scale);
	void	SetTransport(int Transport);
	bool	TogglePlay();
	void	SetParent(CWnd *Parent);
	void	UpdateUI();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CAutoDlg)
	enum { IDD = IDD_AUTO_DLG };
	CIconButton	m_Minutes;
	CClickSliderCtrl	m_TimeSlider;
	CIconButton	m_Play;
	CIconButton	m_Stop;
	CIconButton	m_Pause;
	CIconButton	m_Loop;
	CTimeEdit	m_TimeEdit;
	CComboBox	m_Waveform;
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CAutoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnDeltaposAutoTimeSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPause();
	afx_msg void OnPlay();
	afx_msg void OnStop();
	afx_msg void OnMinutes();
	afx_msg void OnLoop();
	afx_msg void OnToggleAutoDlg();
	afx_msg void OnTogglePlay();
	afx_msg void OnToggleLoop();
	afx_msg void OnToggleMinutes();
	afx_msg void OnSelchangeAutoWaveform();
	//}}AFX_MSG
	virtual	afx_msg BOOL OnNcActivate(BOOL bActive);
	DECLARE_MESSAGE_MAP()

// Overridables
	virtual	void OnTransport(int Transport);
	virtual	void OnGo();
	virtual	void OnModify();
	virtual	void OnTimeChange();
	virtual void OnNewPos();

// Constants
	static const double	m_Tolerance;	// for floating comparisons
	static const int	m_WaveNameID[CAutoInfo::WAVEFORMS];

// Member data
	static	int	m_TimerPeriod;
	CAutoInfo	*m_ip;
	CAutoInfo	m_DefaultInfo;

// Overrides
	void	SaveUndoState(CUndoState& State);
	void	RestoreUndoState(const CUndoState& State);
	CUndoState::UNDO_KEY	GetUndoKey();
	void	GetUndoTitle(const CUndoState& State, CString& Title);

// Helpers
	void	UpdateTime();
	static	int		DblCmp(double a, double b);
	NOCOPIES(CAutoDlg);
};

inline CAutoInfo *CAutoDlg::GetInfo()
{
	return(m_ip);
}

inline void CAutoDlg::GetInfo(CAutoInfo& Info)
{
	Info = *m_ip;
}

inline int CAutoDlg::GetTimerPeriod()
{
	return(m_TimerPeriod);
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTODLG_H__E186F30D_7965_4630_B148_5F48C8C02425__INCLUDED_)
