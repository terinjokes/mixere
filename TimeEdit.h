// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      28sep04	initial version

        time edit control
 
*/

#if !defined(AFX_TIMEEDIT_H__2E70D361_CC38_42FC_868E_27A60970D10B__INCLUDED_)
#define AFX_TIMEEDIT_H__2E70D361_CC38_42FC_868E_27A60970D10B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TimeEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTimeEdit window

#include "NumEdit.h"
#include "Undoable.h"

class CAutoInfo;

class CTimeEdit : public CNumEdit
{
// Construction
public:
	CTimeEdit();

// Constants
	enum {		// time units
		UNIT_SECONDS,
		UNIT_MINUTES,
		UNITS
	};

// Attributes
public:
	void	SetTime(int Time);
	int		GetTime() const;
	void	SetPos(int Pos);
	int		GetPos() const;
	void	SetUnit(int Unit);
	int		GetUnit() const;
	void	GetInfo(CAutoInfo& Info) const;
	void	SetInfo(const CAutoInfo& Info);

// Operations
public:
	void	AddSpin(int Delta);
	static	int		GetAppropriateUnit(int Time);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimeEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTimeEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTimeEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillfocus();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Constants
	enum {
		PLACES = 4		// number of places in a time
	};	
	static const int m_Numer[PLACES];	// time place numerators

// Member data
	int		m_Time;
	int		m_Unit;

// Overrides
	void	StrToVal(LPCSTR Str);
	void	ValToStr(CString& Str);
	void	SaveUndoState(CUndoState& State);
	void	RestoreUndoState(const CUndoState& State);

// Helpers
	int		StrToTime(LPCSTR Str) const;
	void	TimeToStr(int Time, CString& Str) const;
	int		ClampTime(int Time) const;

	NOCOPIES(CTimeEdit);
};

inline int CTimeEdit::GetTime() const
{
	return(m_Time);
}

inline int CTimeEdit::GetUnit() const
{
	return(m_Unit);
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIMEEDIT_H__2E70D361_CC38_42FC_868E_27A60970D10B__INCLUDED_)
