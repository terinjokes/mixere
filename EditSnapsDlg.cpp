// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      16jan05	initial version
		01		01feb05	add undo
		02		05feb05	add hot key
		03		07feb05	add F2 shortcut for rename

        manage mixer snapshots
 
*/

// SnapshotsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "EditSnapsDlg.h"
#include "Snapshot.h"
#include "PopupEdit.h"
#include "MixereView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditSnapsDlg dialog


CEditSnapsDlg::CEditSnapsDlg(CSnapshot& Snapshot, CWnd* pParent)
	: m_Snapshot(Snapshot), CDialog(CEditSnapsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditSnapsDlg)
	m_RestoreType = 0;
	//}}AFX_DATA_INIT
}


void CEditSnapsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditSnapsDlg)
	DDX_Control(pDX, IDC_SNAP_HOTKEY, m_HotKeyCtrl);
	DDX_Control(pDX, IDC_SNAP_SORT, m_Sort);
	DDX_Control(pDX, IDC_SNAP_LIST, m_List);
	DDX_Radio(pDX, IDC_SNAP_EXACT, m_RestoreType);
	//}}AFX_DATA_MAP
}

void CEditSnapsDlg::UpdateListBox()
{
	m_List.ResetContent();
	for (int i = 0; i < m_Snapshot.GetCount(); i++)
		m_List.AddString(m_Snapshot.GetTitle(i));
	m_List.SetCurSel(m_Snapshot.GetActive());
}

void CEditSnapsDlg::NotifyUndo(WORD Code, int Param, UINT Flags)
{
	m_Snapshot.SetUndoParam(Param);
	m_Snapshot.GetMixer()->NotifyUndoableEdit(Code, Flags);
}

void CEditSnapsDlg::CancelUndo(WORD Code)
{
	m_Snapshot.GetMixer()->CancelUndoableEdit(Code);
}

BEGIN_MESSAGE_MAP(CEditSnapsDlg, CDialog)
	//{{AFX_MSG_MAP(CEditSnapsDlg)
	ON_BN_CLICKED(IDC_SNAP_RESTORE, OnSnapRestore)
	ON_BN_CLICKED(IDC_SNAP_DELETE, OnSnapDelete)
	ON_BN_CLICKED(IDC_SNAP_RENAME, OnSnapRename)
	ON_LBN_DBLCLK(IDC_SNAP_LIST, OnDblclkSnapList)
	ON_BN_CLICKED(IDC_SNAP_EXACT, OnRestoreType)
	ON_CBN_SELCHANGE(IDC_SNAP_SORT, OnSelchangeSnapSort)
	ON_LBN_SELCHANGE(IDC_SNAP_LIST, OnSelchangeSnapList)
	ON_BN_CLICKED(IDC_SNAP_SMOOTH, OnRestoreType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditSnapsDlg message handlers

BOOL CEditSnapsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_RestoreType = m_Snapshot.GetRestoreType();
	UpdateData(FALSE);
	CString	s;
	for (int i = 0; i < CSnapshot::SORT_TYPES; i++) {
		m_Snapshot.GetSortTypeName(i, s);
		m_Sort.AddString(s);
	}
	m_Sort.SetCurSel(m_Snapshot.GetSortType());
	UpdateListBox();
	OnSelchangeSnapList();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEditSnapsDlg::OnSnapRestore() 
{
	int	pos = m_List.GetCurSel();
	if (pos != LB_ERR)
		m_Snapshot.Restore(pos);
}

void CEditSnapsDlg::OnSnapDelete() 
{
	int	pos = m_List.GetCurSel();
	if (pos != LB_ERR) {
		NotifyUndo(CSnapshot::UCODE_DELETE, pos);
		m_Snapshot.Delete(pos);
		UpdateListBox();
		pos = min(pos, m_Snapshot.GetCount() - 1);
		m_List.SetCurSel(pos);	// restore selection
	}
}

void CEditSnapsDlg::OnSnapRename()
{
	int	pos = m_List.GetCurSel();
	if (pos != LB_ERR) {
		NotifyUndo(CSnapshot::UCODE_RENAME, pos);
		CRect	r;
		m_List.GetItemRect(pos, r);
		CString	s;
		m_List.GetText(pos, s);
		m_List.SetWindowText(s);	// pass text to popup edit
		CPopupEdit	pe(&m_List, &r);
		pe.SetLimitText(CSnapshot::MAX_TITLE);
		if (pe.DoModal() != IDOK || !pe.GetModify())
			CancelUndo(CSnapshot::UCODE_RENAME);
		m_List.GetWindowText(s);	// retrieve text from popup edit
		DWORD	CurSelID = m_Snapshot.GetID(pos);	// save selection's ID
		m_Snapshot.SetTitle(pos, s);	// selection may move within list
		UpdateListBox();
		pos = m_Snapshot.FindByID(CurSelID);	// find selection by ID
		m_List.SetCurSel(pos);	// restore selection
	}
}

void CEditSnapsDlg::OnDblclkSnapList() 
{
	OnSnapRename();
}

void CEditSnapsDlg::OnRestoreType() 
{
	UpdateData(TRUE);
	NotifyUndo(CSnapshot::UCODE_OPTIONS);
	m_Snapshot.SetRestoreType(m_RestoreType);
}

void CEditSnapsDlg::OnSelchangeSnapSort() 
{
	NotifyUndo(CSnapshot::UCODE_OPTIONS);
	m_Snapshot.SetSortType(m_Sort.GetCurSel());
	UpdateListBox();
}

void CEditSnapsDlg::OnSelchangeSnapList() 
{
	int	pos = m_List.GetCurSel();
	if (pos != LB_ERR) {
		DWORD	Key = m_Snapshot.GetHotKey(pos);
		m_HotKeyCtrl.SetHotKey(LOWORD(Key), HIWORD(Key));
	}
}

BOOL CEditSnapsDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	NMHDR	*nmh = (NMHDR *)lParam;
	if (nmh->hwndFrom == m_HotKeyCtrl.m_hWnd) {
		switch (nmh->code) {
		case HKN_KILLFOCUS:
			int	pos = m_List.GetCurSel();
			if (pos != LB_ERR) {
				NotifyUndo(CSnapshot::UCODE_HOTKEY, pos);
				DWORD	HotKey = m_HotKeyCtrl.GetHotKey();
				if (!m_Snapshot.SetHotKey(pos, HotKey)) {
					m_HotKeyCtrl.SetFocus();
					OnSelchangeSnapList();
					CancelUndo(CSnapshot::UCODE_HOTKEY);
				}
			}
			break;
		}
	}
	return CDialog::OnNotify(wParam, lParam, pResult);
}

BOOL CEditSnapsDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_F2
	&& pMsg->hwnd == m_List.m_hWnd)	// if F2 pressed in snap list, rename
		OnSnapRename();
	return CDialog::PreTranslateMessage(pMsg);
}
