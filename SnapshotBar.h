// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      16jan05	initial version

		snapshot dialog bar
 
*/

#if !defined(AFX_SNAPSHOTBAR_H__9D725823_F782_4C58_BBB8_01B240B648CA__INCLUDED_)
#define AFX_SNAPSHOTBAR_H__9D725823_F782_4C58_BBB8_01B240B648CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// SnapshotBar.h : header file
//

#include "DialogBarEx.h"

class CMixereView;

/////////////////////////////////////////////////////////////////////////////
// SnapshotBar dialog

class CSnapshotBar : public CDialogBarEx
{
public:
// Construction
	CSnapshotBar(CMixereView *Mixer);

// Attributes

// Operations
	void	UpdateList();
	void	Select(int SnapIdx);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSnapshotBar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CSnapshotBar)
	enum { IDD = IDD_SNAPSHOT_BAR };
	CComboBox	m_List;
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CSnapshotBar)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchangeSnapshotCombo();
	//}}AFX_MSG
	afx_msg LONG OnInitDialog(UINT wParam, LONG lParam);
	DECLARE_MESSAGE_MAP()

// Member data
	CMixereView	*m_Mixer;

// Overridables

// Overrides
	void	SaveUndoState(CUndoState& State);
	void	RestoreUndoState(const CUndoState& State);
	void	GetUndoTitle(const CUndoState& State, CString& Title);

// Helpers

	NOCOPIES(CSnapshotBar);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SNAPSHOTBAR_H__9D725823_F782_4C58_BBB8_01B240B648CA__INCLUDED_)
