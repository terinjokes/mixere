// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      16jan05	initial version

		set properties of new mixer snapshot
 
*/

#if !defined(AFX_TAKESNAPDLG_H__01DC4E20_4EE6_42B2_9621_5ADF7BDF98B0__INCLUDED_)
#define AFX_TAKESNAPDLG_H__01DC4E20_4EE6_42B2_9621_5ADF7BDF98B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TakeSnapDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTakeSnapDlg dialog

#include "HotKeyCtrlEx.h"

class CSnapshot;

class CTakeSnapDlg : public CDialog
{
public:
// Construction
	CTakeSnapDlg(const CSnapshot& Snapshot, CWnd* pParent = NULL);

// Attributes
	LPCSTR	GetName() const;
	void	SetName(LPCSTR Name);
	DWORD	GetHotKey(int& OwnerPos) const;
	void	SetHotKey(DWORD Key);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTakeSnapDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Generated message map functions
	//{{AFX_MSG(CTakeSnapDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dialog Data
	//{{AFX_DATA(CTakeSnapDlg)
	enum { IDD = IDD_TAKE_SNAP_DLG };
	CHotKeyCtrlEx	m_HotKeyCtrl;
	CEdit	m_SnapNameEdit;
	//}}AFX_DATA

// Member data
	const	CSnapshot&	m_Snapshot;
	CString	m_Name;
	DWORD	m_HotKey;
	int		m_OwnerPos;

	NOCOPIES(CTakeSnapDlg)
};

inline LPCSTR CTakeSnapDlg::GetName() const
{
	return(m_Name);
}

inline void CTakeSnapDlg::SetName(LPCSTR Name)
{
	m_Name = Name;
}

inline DWORD CTakeSnapDlg::GetHotKey(int& OwnerPos) const
{
	OwnerPos = m_OwnerPos;
	return(m_HotKey);
}

inline void CTakeSnapDlg::SetHotKey(DWORD Key)
{
	m_HotKey = Key;
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TAKESNAPDLG_H__01DC4E20_4EE6_42B2_9621_5ADF7BDF98B0__INCLUDED_)
