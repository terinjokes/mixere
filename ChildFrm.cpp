// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      19jan04	initial version
		01		20jan04	pass size msg to hdr ctrl, else status bar trashes view
		02		09feb04	save and restore window placement in registry
		03		11feb04	OnSize must call CWnd instead of base class
		04		13feb04	header control is a member of mixer now
		05		25sep04	add dialog bars
		06		25sep04	move view's OnDestroy zero volume here
		07		01oct04	RecalcLayout must resize header after calling base class
		08		08nov04	close during solo crashes; OnDestroy must end solo too

		child frame
 
*/

// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "Resource.h"

#include "ChildFrm.h"
#include "MixereDoc.h"
#include "MixereView.h"
#include "Persist.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
}

CChildFrame::~CChildFrame()
{
}

int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	EnableDocking(CBRS_ALIGN_ANY);

	return 0;
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// override default window class styles CS_HREDRAW and CS_VREDRAW
	// otherwise resizing frame redraws entire view, causing flicker
	cs.lpszClass = AfxRegisterWndClass(	// create our own window class
		CS_DBLCLKS,						// request double-clicks
		NULL,							// no cursor (use default)
		NULL,							// no background brush
		AfxGetApp()->LoadIcon(IDR_MAINFRAME));	// app icon
    ASSERT(cs.lpszClass);

	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers

void CChildFrame::ActivateFrame(int nCmdShow) 
{
	if (GetMDIFrame()->MDIGetActive())
		CMDIChildWnd::ActivateFrame(nCmdShow); 
	else {
		int	RegShow = CPersist::GetWndShow(REG_SETTINGS, "Doc");
		CMDIChildWnd::ActivateFrame(RegShow == SW_SHOWMAXIMIZED ? RegShow : nCmdShow);
	}
}

void CChildFrame::OnDestroy() 
{
	CMixereView	*vp = (CMixereView *)GetActiveView();
	if (vp != NULL) {
		vp->SetVolume(0);	// so audio stops all at once
		vp->EndSolo();	// otherwise app crashes if view is destroyed while
						// channels are soloed: OnSolo calls UpdateVolume,
						// which tries to read mixer volume from volume bar,
						// but control bars are destroyed before channels
	}
	// save maximize setting in registry
	CPersist::SaveWnd(REG_SETTINGS, this, "Doc");
	CMDIChildWnd::OnDestroy();
}

void CChildFrame::RecalcLayout(BOOL bNotify) 
{
	CMDIChildWnd::RecalcLayout(bNotify);	// must call base class first
	CMixereView	*vp = (CMixereView *)GetActiveView();
	if (vp != NULL)
		vp->ResizeHeader();	// then resize header control
}
