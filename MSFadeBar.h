// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      28sep04	initial version

		mute/solo fade time dialog bar
 
*/

#if !defined(AFX_MSFADEBAR_H__9D725823_F782_4C58_BBB8_01B240B648CA__INCLUDED_)
#define AFX_MSFADEBAR_H__9D725823_F782_4C58_BBB8_01B240B648CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// MSFadeBar.h : header file
//

#include "DialogBarEx.h"
#include "AutoSliderCtrl.h"
#include "IconButton.h"
#include "TimeEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CMSFadeBar dialog

class CMSFadeBar : public CDialogBarEx
{
public:
// Construction
	CMSFadeBar();

// Attributes
	void	GetInfo(CAutoInfo& Info) const;
	void	SetInfo(const CAutoInfo& Info);
	int		GetTime() const;
	void	SetTime(int Time);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMSFadeBar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CMSFadeBar)
	enum { IDD = IDD_MS_FADE_BAR };
	CSpinButtonCtrl	m_TimeSpin;
	CTimeEdit	m_TimeEdit;
	CSelectSliderCtrl	m_Slider;
	CIconButton	m_Minutes;
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CMSFadeBar)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnDeltaposTimeSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMinutes();
	//}}AFX_MSG
	afx_msg LONG OnInitDialog(UINT wParam, LONG lParam);
	DECLARE_MESSAGE_MAP()

// Overridables
	virtual	void	OnTimeChange();

// Overrides
	void	SaveUndoState(CUndoState& State);
	void	RestoreUndoState(const CUndoState& State);
	void	GetUndoTitle(const CUndoState& State, CString& Title);

// Helpers
	void	UpdateTime();

	NOCOPIES(CMSFadeBar);
};

inline int CMSFadeBar::GetTime() const
{
	return(m_TimeEdit.GetTime());
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MSFADEBAR_H__9D725823_F782_4C58_BBB8_01B240B648CA__INCLUDED_)
