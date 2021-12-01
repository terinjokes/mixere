// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      16jan05	initial version

        edit mixer snapshots
 
*/

#if !defined(AFX_EDITSNAPSDLG_H__889E47CA_6DDC_4728_9C02_F55FDC17B559__INCLUDED_)
#define AFX_EDITSNAPSDLG_H__889E47CA_6DDC_4728_9C02_F55FDC17B559__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SnapshotsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditSnapsDlg dialog

#include "HotKeyCtrlEx.h"

class CSnapshot;

class CEditSnapsDlg : public CDialog
{
public:
// Construction
	CEditSnapsDlg(CSnapshot& Snapshot, CWnd* pParent = NULL);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditSnapsDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
// Generated message map functions
	//{{AFX_MSG(CEditSnapsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSnapRestore();
	afx_msg void OnSnapDelete();
	afx_msg void OnSnapRename();
	afx_msg void OnDblclkSnapList();
	afx_msg void OnRestoreType();
	afx_msg void OnSelchangeSnapSort();
	afx_msg void OnSelchangeSnapList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dialog Data
	//{{AFX_DATA(CEditSnapsDlg)
	enum { IDD = IDD_EDIT_SNAPS_DLG };
	CHotKeyCtrlEx	m_HotKeyCtrl;
	CComboBox	m_Sort;
	CListBox	m_List;
	int		m_RestoreType;
	//}}AFX_DATA

// Member data
	CSnapshot&	m_Snapshot;

// Helpers
	void	UpdateListBox();
	void	NotifyUndo(WORD Code, int Param = 0, UINT Flags = 0);
	void	CancelUndo(WORD Code);

	NOCOPIES(CEditSnapsDlg);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITSNAPSDLG_H__889E47CA_6DDC_4728_9C02_F55FDC17B559__INCLUDED_)
