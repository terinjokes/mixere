// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      31jan04	initial version

        static control with child edit box
 
*/

#if !defined(AFX_STATICEDIT_H__BB79FD04_2A38_4BD3_AFED_2C490C7C1D23__INCLUDED_)
#define AFX_STATICEDIT_H__BB79FD04_2A38_4BD3_AFED_2C490C7C1D23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StaticEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStaticEdit window

#include "Undoable.h"

class CStaticEdit : public CStatic, public CUndoable
{
public:
// Construction
	CStaticEdit();

// Attributes
	bool	IsEdit();
	void	SetLimitText(UINT nMax);

// Operations
	void	Edit();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStaticEdit)
	//}}AFX_VIRTUAL

// Implementation
protected:
// Generated message map functions
	//{{AFX_MSG(CStaticEdit)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	//}}AFX_MSG

// Overrides
	void	SaveUndoState(CUndoState& State);
	void	RestoreUndoState(const CUndoState& State);
	void	GetUndoTitle(const CUndoState& State, CString& Title);

// Member data
	UINT	m_MaxText;

// Helpers
	void	NotifyParent(int Notification);

	DECLARE_MESSAGE_MAP()

	NOCOPIES(CStaticEdit);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATICEDIT_H__BB79FD04_2A38_4BD3_AFED_2C490C7C1D23__INCLUDED_)
