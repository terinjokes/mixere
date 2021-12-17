// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.

// Mixere.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Mixere.h"

#include <tuple>

#include "MainFrm.h"
#include "ChildFrm.h"
#include "MixereDoc.h"
#include "MixereView.h"
#include "VersionInfo.h"
#include "Win32Console.h"
#include "Hyperlink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

LPCSTR	CMixere::HOME_PAGE_URL = "http://mixere.sourceforge.net";

// minimum audiere DLL version; note that initialization is little endian
static const ULARGE_INTEGER	MIN_AUDIERE_VER = {MAKELONG(0, 4), MAKELONG(9, 1)};

/////////////////////////////////////////////////////////////////////////////
// CMixere

BEGIN_MESSAGE_MAP(CMixere, CWinApp)
	//{{AFX_MSG_MAP(CMixere)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_HOME_PAGE, OnHomePage)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	ON_COMMAND(ID_HELP_INDEX, OnHelpIndex)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMixere construction

CMixere::CMixere()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMixere object

CMixere theApp;

/////////////////////////////////////////////////////////////////////////////
// CMixere initialization

BOOL CMixere::InitInstance()
{
	VS_FIXEDFILEINFO DLLInfo;
	bool success;

	std::tie(DLLInfo, success) = CVersionInfo::GetModuleInfo("audiere");
	if (success) {
		if (DLLInfo.dwFileVersionMS < MIN_AUDIERE_VER.HighPart
		|| DLLInfo.dwFileVersionLS < MIN_AUDIERE_VER.LowPart) {
			CString	msg;
			msg.Format("Wrong version of audiere.dll, must be at least %d.%d.%d.%d",
				HIWORD(MIN_AUDIERE_VER.HighPart), LOWORD(MIN_AUDIERE_VER.HighPart),
				HIWORD(MIN_AUDIERE_VER.LowPart), LOWORD(MIN_AUDIERE_VER.LowPart));
			AfxMessageBox(msg);
		}
	}

	AfxEnableControlContainer();

#if SHOW_CONSOLE
	Win32Console::Create();
#endif

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	// Change the registry key under which our settings are stored.
	SetRegistryKey(_T("Anal Software"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_MIXERETYPE,
		RUNTIME_CLASS(CMixereDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CMixereView));
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// enable file manager drag/drop and DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes();

   	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

// By default, CWinApp::OnIdle is called after WM_TIMER messages.  This isn't
// normally a problem, but if the application uses a short timer, OnIdle will
// be called frequently, seriously degrading performance.  Performance can be
// improved by overriding IsIdleMessage to return FALSE for WM_TIMER messages,
// which prevents them from triggering OnIdle.  This technique can be applied
// to any idle-triggering message that repeats frequently, e.g. WM_MOUSEMOVE.
//
BOOL CMixere::IsIdleMessage(MSG* pMsg)
{
	if (CWinApp::IsIdleMessage(pMsg)) {
		switch (pMsg->message) {	// don't call OnIdle after these messages
		case WM_TIMER:
		case WM_MOUSEMOVE:
		case WM_NCMOUSEMOVE:
		case WM_KEYDOWN:	// so keyboard auto-repeat doesn't hog CPU
		case WM_HSCROLL:	// Page Up/Down auto-repeats this message too
			return(FALSE);
		default:
			return(TRUE);
		}
	} else
		return(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CHyperlink	m_AboutUrl;
	CStatic	m_AboutText;
	CEdit	m_License;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_ABOUT_URL, m_AboutUrl);
	DDX_Control(pDX, IDC_ABOUT_TEXT, m_AboutText);
	DDX_Control(pDX, IDC_LICENSE, m_License);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CMixere::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CMixere::OnHomePage() 
{
	CHyperlink::GotoUrl(HOME_PAGE_URL);
}

/////////////////////////////////////////////////////////////////////////////
// CMixere message handlers


BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	VS_FIXEDFILEINFO AppInfo, DLLInfo;
	bool success;

	std::tie(AppInfo, success) = CVersionInfo::GetFileInfo();
	std::tie(DLLInfo, success) = CVersionInfo::GetModuleInfo("audiere");
	CString	s;
	s.Format(IDS_APP_ABOUT_TEXT,
		HIWORD(AppInfo.dwFileVersionMS), LOWORD(AppInfo.dwFileVersionMS),
		HIWORD(AppInfo.dwFileVersionLS), LOWORD(AppInfo.dwFileVersionLS),
		HIWORD(DLLInfo.dwFileVersionMS), LOWORD(DLLInfo.dwFileVersionMS),
		HIWORD(DLLInfo.dwFileVersionLS), LOWORD(DLLInfo.dwFileVersionLS));
	m_AboutText.SetWindowText(s);
	m_AboutUrl.SetUrl(CMixere::HOME_PAGE_URL);
	m_License.SetWindowText(LDS(APP_ABOUT_LICENSE));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
