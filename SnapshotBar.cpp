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

// SnapshotBar.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "SnapshotBar.h"
#include "MixereDoc.h"
#include "MixereView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSnapshotBar dialog

CSnapshotBar::CSnapshotBar(CMixereView *Mixer)
	: CDialogBarEx(NULL), m_Mixer(Mixer)
{
	//{{AFX_DATA_INIT(CSnapshotBar)
	//}}AFX_DATA_INIT
}

void CSnapshotBar::UpdateList()
{
	CSnapshot	*Snapshot = m_Mixer->GetSnapshot();
	if (Snapshot == NULL)
		return;
	m_List.ResetContent();
	for (int i = 0; i < Snapshot->GetCount(); i++)
		m_List.AddString(Snapshot->GetTitle(i));
	m_List.SetCurSel(Snapshot->GetActive());
	m_List.EnableWindow(Snapshot->GetCount());
}

void CSnapshotBar::Select(int SnapIdx)
{
	m_List.SetCurSel(SnapIdx);
}

void CSnapshotBar::SaveUndoState(CUndoState& State)
{
}

void CSnapshotBar::RestoreUndoState(const CUndoState& State)
{
}

void CSnapshotBar::GetUndoTitle(const CUndoState& State, CString& Title)
{
}

void CSnapshotBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBarEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSnapshotBar)
	DDX_Control(pDX, IDC_SNAPSHOT_COMBO, m_List);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSnapshotBar, CDialogBarEx)
	//{{AFX_MSG_MAP(CSnapshotBar)
	ON_WM_SIZE()
	ON_CBN_SELCHANGE(IDC_SNAPSHOT_COMBO, OnSelchangeSnapshotCombo)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_INITDIALOG, OnInitDialog)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSnapshotBar message handlers

LONG CSnapshotBar::OnInitDialog(UINT wParam, LONG lParam)
{
	CDialogBarEx::OnInitDialog(wParam, lParam);
	SetWindowText(LDS(SNAPSHOT_BAR));
	SetBarCaption(LDS(SNAPSHOT_CAPTION));
	// add controls to resizing object
	m_Resize.AddControl(IDC_SNAPSHOT_COMBO, BIND_LEFT | BIND_RIGHT);
	m_Resize.FixControls();

	return TRUE;
}

void CSnapshotBar::OnSize(UINT nType, int cx, int cy) 
{
	CDialogBarEx::OnSize(nType, cx, cy);
	m_Resize.OnSize();
}

void CSnapshotBar::OnSelchangeSnapshotCombo() 
{
	int	pos = m_List.GetCurSel();
	if (pos != CB_ERR) {
		CSnapshot	*Snap = m_Mixer->GetSnapshot();
		if (Snap != NULL)
			Snap->Restore(pos);
	}
}
