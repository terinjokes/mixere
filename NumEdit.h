// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      28sep04	initial version

        numeric edit control
 
*/

#if !defined(AFX_NUMEDIT_H__2E70D361_CC38_42FC_868E_27A60970D10B__INCLUDED_)
#define AFX_NUMEDIT_H__2E70D361_CC38_42FC_868E_27A60970D10B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NumEdit.h : header file
//

#include "Undoable.h"

/////////////////////////////////////////////////////////////////////////////
// CNumEdit window

class CNumEdit : public CEdit, public CUndoable
{
// Construction
public:
	CNumEdit();

// Attributes
public:
	void	SetVal(float Val);
	float	GetVal() const;
	void	SetPrecision(int Precision);
	void	SetUndoTitle(LPCSTR Title);
	void	SetBlank();
	bool	IsBlank() const;

// Operations
public:
	void	AddSpin(float Delta);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNumEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNumEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CNumEdit)
	afx_msg void OnKillfocus();
	afx_msg void OnSetfocus();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Member data
	float	m_Val;
	int		m_Precision;
	CString	m_UndoTitle;

// Overrides
	void	SaveUndoState(CUndoState& State);
	void	RestoreUndoState(const CUndoState& State);
	void	GetUndoTitle(const CUndoState& State, CString& Title);

// Helpers
	virtual	void	StrToVal(LPCSTR Str);
	virtual	void	ValToStr(CString& Str);
	void	SetText();
	void	GetText();
	void	NotifyParent();

	NOCOPIES(CNumEdit);
};

inline float CNumEdit::GetVal() const
{
	return(m_Val);
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NUMEDIT_H__2E70D361_CC38_42FC_868E_27A60970D10B__INCLUDED_)
