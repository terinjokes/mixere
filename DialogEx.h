// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      19apr04	initial version

		customized dialog base class
 
*/

#if !defined(AFX_DIALOGEX_H__4FAACFA4_71A0_4230_BE68_654E89AE406E__INCLUDED_)
#define AFX_DIALOGEX_H__4FAACFA4_71A0_4230_BE68_654E89AE406E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogEx.h : header file
//

#include "Undoable.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogEx dialog

class CDialogEx : public CDialog, public CUndoable
{
public:
// Construction
	CDialogEx(UINT nIDTemplate, UINT nIDAccel, CWnd* pParent = NULL);
	virtual ~CDialogEx();

// Operations
	void	SetParent(CWnd *Parent);
	void	CenterWindowEx(const CPoint& Offset, bool Repaint = FALSE);
	virtual	BOOL	EnableToolTips(BOOL bEnable);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogEx)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog data
	//{{AFX_DATA(CDialogEx)
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CDialogEx)
	//}}AFX_MSG
	afx_msg BOOL OnToolTip(UINT id, NMHDR* pTTTStruct, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

// Overridables

// Member data
	HACCEL	m_Accel;
	CWnd	*m_Parent;

	NOCOPIES(CDialogEx);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGEX_H__4FAACFA4_71A0_4230_BE68_654E89AE406E__INCLUDED_)
