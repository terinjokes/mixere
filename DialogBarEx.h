// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      28sep04	initial version

		dialog bar base class
 
*/

#if !defined(AFX_DIALOGBAREX_H__9D725823_F782_4C58_BBB8_01B240B648CA__INCLUDED_)
#define AFX_DIALOGBAREX_H__9D725823_F782_4C58_BBB8_01B240B648CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogBarEx.h : header file
//

#include "Undoable.h"
#include "CtrlResize.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogBarEx dialog

class CDialogBarEx : public CDialogBar, public CUndoable
{
public:
// Construction
	CDialogBarEx(UINT nIDAccel);
	virtual ~CDialogBarEx();
	BOOL	Create(CWnd *pParentWnd, UINT nIDTemplate, UINT nStyle, UINT nID);
	BOOL	Create(CFrameWnd *pFrameWnd, UINT nIDTemplate, UINT nStyle,
				   DWORD dwDockStyle, UINT nBarIndex, CControlBar *LeftOf);

// Attributes
	int		GetLeftMargin() const;
	void	SetBarCaption(LPCSTR Text);
	void	SetWindowText(LPCTSTR lpszString);

// Operations
	void	DockBar(CFrameWnd *Frame, CControlBar *LeftOf);
	virtual	BOOL	EnableToolTips(BOOL bEnable);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogBarEx)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual CSize CalcDynamicLayout(int nLength, DWORD dwMode);
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CDialogBarEx)
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CDialogBarEx)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	afx_msg LONG OnInitDialog(UINT wParam, LONG lParam);
	afx_msg BOOL OnToolTip(UINT id, NMHDR* pTTTStruct, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

// Constants
	static const int	CAP_LMARGIN;
	static const RECT	NO_CAP_RECT;

// Member data
	CSize	m_DockedSize;
	CSize	m_FloatingSize;
	BOOL	m_bChangeDockedSize;
	HACCEL	m_Accel;
	CStatic	m_Caption;
	CRect	m_CapRect;
	CCtrlResize	m_Resize;
	int		m_PrevFloating;
	bool	m_GotMRUWidth;

	NOCOPIES(CDialogBarEx);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGBAREX_H__9D725823_F782_4C58_BBB8_01B240B648CA__INCLUDED_)
