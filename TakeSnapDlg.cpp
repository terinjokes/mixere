// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      16jan05	initial version
		02		05feb05	add hot key

		set properties of new mixer snapshot
 
*/

// TakeSnapDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "TakeSnapDlg.h"
#include "Snapshot.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTakeSnapDlg dialog


CTakeSnapDlg::CTakeSnapDlg(const CSnapshot& Snapshot, CWnd* pParent /*=NULL*/)
	: m_Snapshot(Snapshot), CDialog(CTakeSnapDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTakeSnapDlg)
	//}}AFX_DATA_INIT
	m_HotKey = 0;		// no hot key
	m_OwnerPos = -1;	// no owner
}


void CTakeSnapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTakeSnapDlg)
	DDX_Control(pDX, IDC_SNAP_HOTKEY, m_HotKeyCtrl);
	DDX_Control(pDX, IDC_SNAP_NAME_EDIT, m_SnapNameEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTakeSnapDlg, CDialog)
	//{{AFX_MSG_MAP(CTakeSnapDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTakeSnapDlg message handlers

BOOL CTakeSnapDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_SnapNameEdit.SetWindowText(m_Name);
	m_SnapNameEdit.SetLimitText(CSnapshot::MAX_TITLE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTakeSnapDlg::OnOK() 
{
	m_SnapNameEdit.GetWindowText(m_Name);
	if (m_HotKeyCtrl.GetHotKey()) {	// if hot key was entered
		DWORD	Key = m_HotKeyCtrl.GetHotKey();
		if (m_Snapshot.ValidateHotKey(Key, -1, m_Name, m_OwnerPos)) {
			m_HotKey = Key;
			CDialog::OnOK();
		} else {	// don't end dialog
			m_HotKeyCtrl.SetHotKey(0, 0);	// reset control
			m_HotKeyCtrl.SetFocus();		// and try again
		}
	} else
		CDialog::OnOK();
}
