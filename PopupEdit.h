// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      31jan04	initial version

        popup edit control
 
*/

#if !defined(AFX_POPUPEDIT_H__D540148F_907B_4CC5_B189_2F75F1269E75__INCLUDED_)
#define AFX_POPUPEDIT_H__D540148F_907B_4CC5_B189_2F75F1269E75__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// PopupEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPopupEdit window

class CPopupEdit : public CEdit
{
public:
// Construction
	CPopupEdit(CWnd *Parent, const CRect *Rect = NULL);
	virtual ~CPopupEdit();

// Attributes
	static	bool	Exists();
	bool	GetModify() const;
	void	SetLimitText(UINT nMax);

// Operations
	int		DoModal();
	static	void	Abort();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPopupEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:
// Generated message map functions
	//{{AFX_MSG(CPopupEdit)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

// Data members
	static	HHOOK	m_MouseHook;
	static	CPopupEdit	*m_This;
	CWnd	*m_Parent;
	const	CRect	*m_Rect;
	bool	m_Modify;
	UINT	m_MaxText;

// Helpers
	void	EndEdit(int retc);

// Friends
	static	LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam);

	NOCOPIES(CPopupEdit);
};

inline bool CPopupEdit::GetModify() const
{
	return(m_Modify);
}

inline bool CPopupEdit::Exists()
{
	return(m_This != NULL);
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POPUPEDIT_H__D540148F_907B_4CC5_B189_2F75F1269E75__INCLUDED_)
