// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      21nov03 initial version
		01		09dec03	if no full drag, track divider with vert line in list
		02		09jan04	add header sort; move header handling to its own object
		03		18jan04	move header creation details into header object
		04		19jan04	convert to MDI
		05		19jan04	add on drop files and drag accept
		06		20jan04	add solo indicator to status bar
		07		09feb04	save and restore window placement in registry
		08		10mar04	deactivate takes focus away from modeless child dialogs
		09		19apr04	add reset animation count in timer hook
        10		25sep04	set window pos in OnShowWindow to avoid double paint
		11		26sep04	if an edit control has focus, disable main accelerators
		12		03oct04	create toolbar visible so LeftOf works without dock state
		13		03oct04	after restoring bar state, must reapply our tip state
		14		05oct04	no more child dialogs; remove OnActivateApp
		15		04jan05	verify bar state before restoring it
		16		07jan05	in alt menu mode, middle-click comes here; redirect it
		17		21jan05	add support for HTML help
		18		27jan05	initialize/uninitialize HTML help to avoid crash on exit
		19		05feb05	add snapshot hot keys
		20		26jun05	add retry for help; append help file name to app path

        main frame window
 
*/

// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Resource.h"

#include "MainFrm.h"
#include "MixereDoc.h"
#include "MixereView.h"
#include "MultiMix.h"
#include "Persist.h"
#include "AutoSliderCtrl.h"
#include "afxpriv.h"	// needed for VerifyDockState
#include "htmlhelp.h"	// needed for HTML Help API
#include "shlwapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLS_MASTERVOL, OnToolsMastervol)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_MASTERVOL, OnUpdateToolsMastervol)
	ON_COMMAND(ID_TOOLS_OPTIONS, OnToolsOptions)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_DROPFILES()
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_MUTE, OnUpdateIndicatorMute)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_SOLO, OnUpdateIndicatorSolo)
	ON_COMMAND(ID_CROSSFADER, OnCrossfader)
	ON_UPDATE_COMMAND_UI(ID_CROSSFADER, OnUpdateCrossfader)
	ON_WM_SHOWWINDOW()
	ON_WM_MBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_MUTE,
	ID_INDICATOR_SOLO
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_MultiMix = new CMultiMix(this);
	m_FirstShow = TRUE;
	m_HelpCookie = 0;
}

CMainFrame::~CMainFrame()
{
	delete m_MultiMix;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// create toolbar
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
	| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC)
	|| !m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	// create status bar
	if (!m_wndStatusBar.Create(this, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM)
	|| !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	// create custom status bar panes
	m_wndStatusBar.SetPaneWidth(SBP_MUTE, 50);
	m_wndStatusBar.SetPaneColor(SBP_MUTE, RGB(255, 192, 0));
	m_wndStatusBar.SetPaneWidth(SBP_SOLO, 50);
	m_wndStatusBar.SetPaneColor(SBP_SOLO, RGB(0, 255, 0));
	// make toolbar dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	// create mixer manager and other main frame control bars
	m_MultiMix->Create();
	// restore our bar state
	if (VerifyBarState(REG_SETTINGS))
		LoadBarState(REG_SETTINGS);
	// restoring bar state enables bar tool tips; reapply our tip state
	m_MultiMix->EnableToolTips(m_MultiMix->HasToolTips());

	DragAcceptFiles(TRUE);	// so we can drop files over empty frame

	return 0;
}

BOOL CMainFrame::VerifyBarState(LPCTSTR lpszProfileName)
{
	CDockState	state;
	state.LoadState(lpszProfileName);
	return(VerifyDockState(state, this));
}

BOOL CMainFrame::VerifyDockState(const CDockState& state, CFrameWnd *Frm)
{
	// thanks to Cristi Posea at codeproject.com
	for (int i = 0; i < state.m_arrBarInfo.GetSize(); i++) {
		CControlBarInfo* pInfo = (CControlBarInfo*)state.m_arrBarInfo[i];
		ASSERT(pInfo != NULL);
		int nDockedCount = pInfo->m_arrBarID.GetSize();
		if (nDockedCount > 0) {
			// dockbar
			for (int j = 0; j < nDockedCount; j++)
			{
				UINT	nID = (UINT) pInfo->m_arrBarID[j];
				if (nID == 0)
					continue; // row separator
				if (nID > 0xFFFF)
					nID &= 0xFFFF; // placeholder - get the ID
				if (Frm->GetControlBar(nID) == NULL)
					return FALSE;
			}
		}
		if (!pInfo->m_bFloating) // floating dockbars can be created later
			if (Frm->GetControlBar(pInfo->m_nBarID) == NULL)
				return FALSE; // invalid bar ID
	}
    return TRUE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
 	if (!CMDIFrameWnd::PreCreateWindow(cs))
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnToolsOptions()
{
	m_MultiMix->ShowOptionsDlg();
}

void CMainFrame::OnToolsMastervol()
{
	m_MultiMix->ToggleDlgBar(CMultiMix::DLGBAR_MASTERVOL);
}

void CMainFrame::OnUpdateToolsMastervol(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_MultiMix->IsDlgBarVisible(CMultiMix::DLGBAR_MASTERVOL));
}

void CMainFrame::OnTimer(UINT nIDEvent)
{
	m_MultiMix->TimerHook();
	CAutoSliderCtrl::ResetAnimCount();
	CFrameWnd::OnTimer(nIDEvent);
}

void CMainFrame::OnDestroy() 
{
	m_MultiMix->Destroy();
	// save window placement in registry
	CPersist::SaveWnd(REG_SETTINGS, this, "App");
	// destroy the frame and all its children
	CFrameWnd::OnDestroy();
}

void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	char	Path[MAX_PATH];
	UINT	Files = DragQueryFile(hDropInfo, -1, 0, 0);
	for (UINT i = 0; i < Files; i++) {
		DragQueryFile(hDropInfo, i, Path, MAX_PATH);
		AfxGetApp()->OpenDocumentFile(Path);
	}
}

void CMainFrame::OnUpdateIndicatorMute(CCmdUI* pCmdUI) 
{
	CMixereView *Mixer = m_MultiMix->GetActiveMixer();
	pCmdUI->Enable(Mixer != NULL ? Mixer->GetMuteCount() : 0);
}

void CMainFrame::OnUpdateIndicatorSolo(CCmdUI* pCmdUI) 
{
	CMixereView *Mixer = m_MultiMix->GetActiveMixer();
	pCmdUI->Enable(Mixer != NULL ? Mixer->GetSoloCount() : 0);
}

void CMainFrame::OnCrossfader() 
{
	m_MultiMix->ToggleDlgBar(CMultiMix::DLGBAR_CROSSFADER);
}

void CMainFrame::OnUpdateCrossfader(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_MultiMix->IsDlgBarVisible(CMultiMix::DLGBAR_CROSSFADER));
}

void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CMDIFrameWnd::OnShowWindow(bShow, nStatus);
	if (m_FirstShow && !IsWindowVisible()) {
		m_FirstShow = FALSE;
		CPersist::LoadWnd(REG_SETTINGS, this, "App", CPersist::NO_MINIMIZE);
	}
}

BOOL CMainFrame::DestroyWindow() 
{
	// save bar states in registry
	SaveBarState(REG_SETTINGS);
	// if HTML help was initialized, uninitialize it
	if (m_HelpCookie) {
		::HtmlHelp(NULL, NULL, HH_CLOSE_ALL, 0);
		::HtmlHelp(NULL, NULL, HH_UNINITIALIZE, m_HelpCookie);
	}
	return CMDIFrameWnd::DestroyWindow();
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST) {
		// if active mixer has snapshot hot keys, try translating them
		CMixereView *Mixer = m_MultiMix->GetActiveMixer();
		if (Mixer != NULL) {
			HACCEL	Accel = Mixer->GetSnapshotAccel();
			if (Accel != NULL) {
				if (TranslateAccelerator(Mixer->m_hWnd, Accel, pMsg))
					return(TRUE);	// message was a snapshot hot key
			}
		}
		// if an edit control has focus, disable main accelerators;
		// otherwise edit controls in control bars behave unexpectedly
		CWnd	*fw = GetFocus();
		if (fw != NULL && fw->IsKindOf(RUNTIME_CLASS(CEdit)))
			return(FALSE);	// dispatch to next handler
	}
	return CMDIFrameWnd::PreTranslateMessage(pMsg);
}

void CMainFrame::OnMButtonDown(UINT nFlags, CPoint point) 
{
	POINT	cp;
	GetCursorPos(&cp);
	CWnd	*w = WindowFromPoint(cp);
	if (w != NULL)	// redirect middle-click to window under cursor
		w->SendMessage(WM_MBUTTONDOWN, nFlags, MAKELONG(point.x, point.y));
	CMDIFrameWnd::OnMButtonDown(nFlags, point);
}

void CMainFrame::WinHelp(DWORD dwData, UINT nCmd) 
{
	static const LPCSTR	HELP_FILE_NAME = "mixere.chm";
	// if HTML help hasn't been initialized yet, initialize it
	if (!m_HelpCookie)
		::HtmlHelp(NULL, NULL, HH_INITIALIZE, (DWORD)&m_HelpCookie);
	HWND retc = ::HtmlHelp(m_hWnd, HELP_FILE_NAME, HH_DISPLAY_TOC, 0);
	if (!retc) {	// not found, try appending help file name to app path
		CString	HelpPath = GetCommandLine();
		HelpPath.TrimLeft();	// trim leading whitespace just in case
		if (HelpPath[0] == '"')		// if first char is a quote
			HelpPath = HelpPath.Mid(1).SpanExcluding("\"");	// span to next quote
		else
			HelpPath = HelpPath.SpanExcluding(" \t");	// span to next whitespace
		char	*p = HelpPath.GetBuffer(0);
		PathRemoveFileSpec(p);
		HelpPath.ReleaseBuffer(-1);	// HelpPath now contains app path
		HelpPath += "\\";
		HelpPath += HELP_FILE_NAME;
		retc = ::HtmlHelp(m_hWnd, HelpPath, HH_DISPLAY_TOC, 0);	// try again
		if (!retc) {	// not found, give up
			CString	s;
			AfxFormatString1(s, IDS_HELP_FILE_MISSING, HELP_FILE_NAME);
			AfxMessageBox(s);
		}
	}
}
