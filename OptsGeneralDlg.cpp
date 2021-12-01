// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      11dec03 initial version
		01		30dec03	add tooltips checkbox
		02		09feb04	save and restore state in registry
		03		07mar04	add undo limit
		04		04jan05	split options dialog into property pages

		general options property page
		
*/

// OptsEditingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "OptsGeneralDlg.h"
#include "Persist.h"
#include "Audio.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptsGeneralDlg property page

IMPLEMENT_DYNCREATE(COptsGeneralDlg, CPropertyPage)

LPCSTR COptsGeneralDlg::AUDIO_DEVICE	= "AudioDevice";
LPCSTR COptsGeneralDlg::SHOW_TOOL_TIPS	= "ShowToolTips";
LPCSTR COptsGeneralDlg::DEV_DEFAULT		= "default";
LPCSTR COptsGeneralDlg::UNDO_LEVELS		= "UndoLevels";

COptsGeneralDlg::COptsGeneralDlg() : CPropertyPage(COptsGeneralDlg::IDD)
{
	//{{AFX_DATA_INIT(COptsGeneralDlg)
	//}}AFX_DATA_INIT
	m_ShowToolTips = CPersist::GetInt(REG_SETTINGS, SHOW_TOOL_TIPS, 1) != 0;
	m_CurAudioDev = CPersist::GetString(REG_SETTINGS, AUDIO_DEVICE);
	m_UndoLevels = CPersist::GetInt(REG_SETTINGS, UNDO_LEVELS, -1);
}

COptsGeneralDlg::~COptsGeneralDlg()
{
	CPersist::WriteInt(REG_SETTINGS, SHOW_TOOL_TIPS, m_ShowToolTips);
	CPersist::WriteString(REG_SETTINGS, AUDIO_DEVICE, m_CurAudioDev);
	CPersist::WriteInt(REG_SETTINGS, UNDO_LEVELS, m_UndoLevels);
}

void COptsGeneralDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptsGeneralDlg)
	DDX_Control(pDX, IDC_UNDO_LEVELS, m_UndoLevelsEdit);
	DDX_Control(pDX, IDC_UNDO_UNLIMITED, m_UnlimitedUndo);
	DDX_Control(pDX, IDC_DISPLAY_TOOLTIPS, m_ShowToolTipsBtn);
	DDX_Control(pDX, IDC_AUDIO_DEVICE, m_AudioDevList);
	//}}AFX_DATA_MAP
}

void COptsGeneralDlg::PopulateAudioDevList()
{
	CStringArray	DevList;
	CAudio::GetDeviceList(DevList);	// get device list from audio
	CString	CurDev = m_CurAudioDev;
	if (CurDev.IsEmpty())
		CurDev = DEV_DEFAULT;
	m_AudioDevList.AddString(DEV_DEFAULT);
	int	DevIdx = 0;
	for (int i = 0; i < DevList.GetSize(); i++) {	// populate list
		m_AudioDevList.AddString(DevList[i]);
		if (DevList[i] == CurDev)
			DevIdx = i + 1;
	}
	m_AudioDevList.SetCurSel(DevIdx);	// set current selection
}

void COptsGeneralDlg::SetAudioDevice()
{
	CString	NewDev;
	m_AudioDevList.GetWindowText(NewDev);
	if (NewDev == DEV_DEFAULT)
		NewDev.Empty();
	if (NewDev != m_CurAudioDev) {
		if (CAudio::SetDevice(NewDev))
			m_CurAudioDev = NewDev;
		else
			AfxMessageBox(LDS(CANT_SET_AUDIO_DEV));
	}
}

int COptsGeneralDlg::GetUndoLevels() const
{
	return(m_UndoLevels);
}

void COptsGeneralDlg::UpdateUndoLevels(bool Save)
{
	if (Save) {
		if (m_UnlimitedUndo.GetCheck())
			m_UndoLevels = -1;
		else {
			CString	s;
			m_UndoLevelsEdit.GetWindowText(s);
			m_UndoLevels = atoi(s);
		}
	} else {
		if (m_UnlimitedUndo.GetCheck()) {
			m_UndoLevelsEdit.SetWindowText(NULL);
			m_UndoLevelsEdit.EnableWindow(FALSE);
		} else {
			CString	s;
			s.Format("%d", m_UndoLevels < 0 ? DEFAULT_UNDO_LEVELS : m_UndoLevels);
			m_UndoLevelsEdit.SetWindowText(s);
			m_UndoLevelsEdit.EnableWindow(TRUE);
		}
	}
}

BEGIN_MESSAGE_MAP(COptsGeneralDlg, CPropertyPage)
	//{{AFX_MSG_MAP(COptsGeneralDlg)
	ON_BN_CLICKED(IDC_UNDO_UNLIMITED, OnUnlimitedUndo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptsGeneralDlg message handlers

BOOL COptsGeneralDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	PopulateAudioDevList();
	m_AudioDevList.EnableWindow(!CAudio::IsDeviceInUse());

	m_ShowToolTipsBtn.SetCheck(m_ShowToolTips);

	m_UnlimitedUndo.SetCheck(m_UndoLevels < 0);
	UpdateUndoLevels(FALSE);	// initialize controls

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptsGeneralDlg::OnOK() 
{
	SetAudioDevice();
	
	m_ShowToolTips = m_ShowToolTipsBtn.GetCheck() != 0;

	UpdateUndoLevels(TRUE);	// retrieve controls
	
	CPropertyPage::OnOK();
}

void COptsGeneralDlg::OnUnlimitedUndo() 
{
	UpdateUndoLevels(FALSE);
}
