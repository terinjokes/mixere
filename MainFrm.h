// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00		21nov03 initial version

        main frame window
 
*/

// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__27BBE5B0_A6CF_481B_9DA5_AB6F282EFF08__INCLUDED_)
#define AFX_MAINFRM_H__27BBE5B0_A6CF_481B_9DA5_AB6F282EFF08__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ColorStatusBar.h"

class CMultiMix;

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Constants
	enum {	// status bar panes; must match indicators array
		SBP_HINT,
		SBP_MUTE,
		SBP_SOLO
	};

// Attributes
public:
	CMultiMix	*GetMultiMix();
	CToolBar	*GetToolBar();
	CColorStatusBar	*GetStatusBar();

// Operations
public:
	static	BOOL	VerifyDockState(const CDockState& State, CFrameWnd *Frm);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL DestroyWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CColorStatusBar	m_wndStatusBar;
	CToolBar	m_wndToolBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnToolsMastervol();
	afx_msg void OnUpdateToolsMastervol(CCmdUI* pCmdUI);
	afx_msg void OnToolsOptions();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnUpdateIndicatorMute(CCmdUI* pCmdUI);
	afx_msg void OnUpdateIndicatorSolo(CCmdUI* pCmdUI);
	afx_msg void OnCrossfader();
	afx_msg void OnUpdateCrossfader(CCmdUI* pCmdUI);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Data members
	CMultiMix	*m_MultiMix;
	bool	m_FirstShow;
	DWORD	m_HelpCookie;	// for HTMLHelp API

// Helpers
	BOOL	VerifyBarState(LPCTSTR lpszProfileName);

	NOCOPIES(CMainFrame);
};

inline CMultiMix *CMainFrame::GetMultiMix()
{
	return(m_MultiMix);
}

inline CToolBar *CMainFrame::GetToolBar()
{
	return(&m_wndToolBar);
}

inline CColorStatusBar *CMainFrame::GetStatusBar()
{
	return(&m_wndStatusBar);
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__27BBE5B0_A6CF_481B_9DA5_AB6F282EFF08__INCLUDED_)
