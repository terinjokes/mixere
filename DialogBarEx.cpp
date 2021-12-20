// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      28sep04	initial version
		01		02oct04	use resizer origin shift to make room for caption
		02		02oct04	OnSize must invalidate bar on dock state change
		03		03oct04	add overload of create that docks too
		04		06oct04	make bar width persistent
		05		07oct04	override EnableToolTips to make it virtual
		06		20jan05	window ID isn't res ID; must be in correct range too
		07		20jan05	override SetWindowText so it works when we're floating

		dialog bar base class
 
*/

// DialogBarEx.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "DialogBarEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogBarEx dialog

const int CDialogBarEx::CAP_LMARGIN = 12;	// space between bar left edge and caption
const RECT CDialogBarEx::NO_CAP_RECT = {0, 0, CAP_LMARGIN / 2, 0};	// room for gripper

CDialogBarEx::CDialogBarEx(UINT nIDAccel)
{
	//{{AFX_DATA_INIT(CDialogBarEx)
	//}}AFX_DATA_INIT
	m_DockedSize = m_FloatingSize = 0;
	m_bChangeDockedSize = FALSE;
	m_Accel = LOADACCEL(nIDAccel);
	m_CapRect = NO_CAP_RECT;
	m_PrevFloating = -1;	// so first OnSize will invalidate regardless
	m_GotMRUWidth = FALSE;	// true if we've loaded CControlBar's MRU width
}

CDialogBarEx::~CDialogBarEx()
{
}

BOOL CDialogBarEx::Create(CWnd *pParentWnd, UINT nIDTemplate, UINT nStyle, UINT nID)
{
	if (!CDialogBar::Create(pParentWnd, nIDTemplate, nStyle, nID))
		return FALSE;

	m_bChangeDockedSize = FALSE;
	m_FloatingSize = m_DockedSize = m_sizeDefault;

	return TRUE;
}

BOOL CDialogBarEx::Create(CFrameWnd *pFrameWnd, UINT nIDTemplate, UINT nStyle,
						  DWORD dwDockStyle, UINT nBarIndex, CControlBar *LeftOf)
{
	if (!nStyle) {	// if style is zero, use default style
		nStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER |
			CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC;
	}
	// convert bar index to a control bar window ID; see MFC TN031
	UINT	nIDWnd = AFX_IDW_CONTROLBAR_FIRST + 32 + nBarIndex;
	if (!Create(pFrameWnd, nIDTemplate, nStyle, nIDWnd))
		return(FALSE);
	EnableDocking(dwDockStyle);
	DockBar(pFrameWnd, LeftOf);
	return(TRUE);
}

CSize CDialogBarEx::CalcDynamicLayout(int nLength, DWORD dwMode)
{
	CSize	size;
	if (dwMode & (LM_MRUWIDTH | LM_STRETCH)) {
		// if CControlBar has a valid MRU width and we haven't copied it yet
		if (m_nMRUWidth != 32767 && !m_GotMRUWidth) {
			m_FloatingSize.cx = m_nMRUWidth;	// copy width from CControlBar
			m_GotMRUWidth = TRUE;				// from now on we maintain width
		}
		size = m_FloatingSize;
	} else if (dwMode & (LM_VERTDOCK | LM_HORZDOCK)) {
		size = m_FloatingSize + CSize(m_CapRect.right, 0);	// make room for caption
	} else {
		if (dwMode & LM_LENGTHY) {
			size = CSize(
				m_FloatingSize.cx,
				m_FloatingSize.cy);	// don't allow vertical resizing
		} else {
			size = CSize(
				m_FloatingSize.cx = max(nLength, m_DockedSize.cx),
				m_FloatingSize.cy);
		}
		m_nMRUWidth = m_FloatingSize.cx;	// so SaveBarState captures our width
	}
	PostMessage(WM_SIZE, 0, MAKELONG(size.cx, size.cy));
	return(size);
}

void CDialogBarEx::DockBar(CFrameWnd *Frame, CControlBar *LeftOf)
{
	if (LeftOf == NULL) {
		Frame->DockControlBar(this);
		return;
	}
	CRect rect;
	DWORD dw;
	UINT n;
	// Get MFC to adjust the dimensions of all docked ToolBars
	// so that GetWindowRect will be accurate.
	Frame->RecalcLayout(TRUE);
	LeftOf->GetWindowRect(&rect);
	rect.OffsetRect(1, 0);
	dw = LeftOf->GetBarStyle();
	n = 0;
	n = (dw & CBRS_ALIGN_TOP)				? AFX_IDW_DOCKBAR_TOP : n;
	n = ((dw & CBRS_ALIGN_BOTTOM) && !n)	? AFX_IDW_DOCKBAR_BOTTOM : n;
	n = ((dw & CBRS_ALIGN_LEFT) && !n)		? AFX_IDW_DOCKBAR_LEFT : n;
	n = ((dw & CBRS_ALIGN_RIGHT) && !n)		? AFX_IDW_DOCKBAR_RIGHT : n;
	// When we take the default parameters on rect, DockControlBar will dock
	// each Toolbar on a separate line. By calculating a rectangle, we
	// are simulating a Toolbar being dragged to that location and docked.
	Frame->DockControlBar(this, n, &rect);
}

void CDialogBarEx::SetBarCaption(LPCSTR Text)
{
	m_CapRect = NO_CAP_RECT;	// assume failure
	if (Text != NULL) {
		CDC	*pDC = GetDC();
		ASSERT(pDC != NULL);
		pDC->SelectObject(GetFont());	// use our font to calculate caption text size
		if (pDC->DrawText(Text, m_CapRect, DT_CALCRECT | DT_NOPREFIX)) {
			CRect	cr;
			GetClientRect(cr);
			int	y = (cr.Height() - m_CapRect.Height()) / 2;
			m_CapRect = CRect(CAP_LMARGIN, y, CAP_LMARGIN + m_CapRect.Width(), 
				y + m_CapRect.Height());
		}
		ReleaseDC(pDC);
	}
	m_Caption.MoveWindow(m_CapRect, TRUE);
	m_Caption.SetWindowText(Text);
	if (!IsFloating())
		GetDockingFrame()->RecalcLayout();		// force bar to be resized
}

void CDialogBarEx::SetWindowText(LPCTSTR lpszString)
{
	CDialogBar::SetWindowText(lpszString);
	// SetWindowText doesn't work while a control bar is floating; it's because
	// in this case, the caption is owned by the parent frame (CMiniDockFrame)
	if (IsFloating()) {
		CFrameWnd	*Frame = GetParentFrame();
		if (Frame != NULL)		// call SetWindowText on the parent frame too
			Frame->SetWindowText(lpszString);
	}
}

int CDialogBarEx::GetLeftMargin() const
{
	return(IsFloating() ? 0 : m_CapRect.right);		// caption only when docked
}

BOOL CDialogBarEx::EnableToolTips(BOOL bEnable)
{
	return(CDialogBar::EnableToolTips(bEnable));
}

void CDialogBarEx::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogBarEx)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDialogBarEx, CDialogBar)
	//{{AFX_MSG_MAP(CDialogBarEx)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_INITDIALOG, OnInitDialog)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTip)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogBarEx message handlers

LRESULT CDialogBarEx::OnInitDialog(WPARAM wParam, LPARAM lParam)
{
	BOOL bRet = HandleInitDialog(wParam, lParam);

	if (!UpdateData(FALSE))
	   TRACE0("Warning: UpdateData failed during dialog init.\n");

	// create caption window
	m_Caption.Create("", WS_CHILD | SS_NOPREFIX, CRect(0, 0, 0, 0), this);
	m_Caption.SetFont(GetFont());	// give it our font
	m_Resize.SetParentWnd(this);

	return TRUE;
}

BOOL CDialogBarEx::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST) {
		if (TranslateAccelerator(m_hWnd, m_Accel, pMsg))
			return(TRUE);
	}
	return CDialogBar::PreTranslateMessage(pMsg);
}

BOOL CDialogBarEx::OnToolTip(UINT id, NMHDR* pTTTStruct, LRESULT* pResult)
{
	LPNMTTDISPINFO	ttp = LPNMTTDISPINFO(pTTTStruct);
	UINT nID = ttp->hdr.idFrom;
	if (ttp->uFlags & TTF_IDISHWND)		// idFrom can be HWND or ID
		nID = ::GetDlgCtrlID((HWND)nID);
	CString	s;
	s.LoadString(nID);
	strcpy(ttp->szText, s);
	return(TRUE);
}

void CDialogBarEx::OnSize(UINT nType, int cx, int cy) 
{
	CDialogBar::OnSize(nType, cx, cy);
	if (IsWindowVisible()) {
		m_Caption.ShowWindow(IsFloating() ? SW_HIDE : SW_SHOW);
		// trick resizer into making room for caption
		m_Resize.SetOriginShift(CSize(GetLeftMargin(), 0));
		m_Resize.OnSize();
		// If dock state changed, we must invalidate bar; otherwise if app is
		// run via shell open, and document is initially maximized, and there's
		// a docked bar visible, bar's controls may not paint correctly.  Only
		// invalidate on dock state change, to avoid needless resizing flicker.
		if (m_PrevFloating != IsFloating()) {	// if dock state changed
			Invalidate();
			m_PrevFloating = IsFloating();
		}
	}
}
