// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      20jan04	initial version
		01		14nov04	override OnUpdateCmdUI so panes draw after SetDockState
		02		13jan05	replace OnUpdateCmdUI override with EnablePane function

		color status bar panes and text
 
*/

// ColorStatusBar.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "ColorStatusBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorStatusBar

CColorStatusBar::CColorStatusBar()
{
	m_Indicators = 0;
}

CColorStatusBar::~CColorStatusBar()
{
}

void CColorStatusBar::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	int	item = lpDrawItemStruct->itemID;
	// attach to a CDC object
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);
	// get pane rectangle
	CRect	r(&lpDrawItemStruct->rcItem);
	if (!(GetPaneStyle(item) & SBPS_DISABLED)) {
		// if pane color specified, fill rect with color
		if (m_PaneColor[item] != -1) {
			CBrush	b(m_PaneColor[item]);
			dc.FillRect(r, &b);
		}
		// if text color specified, set it
		if (m_TextColor[item] != -1)
			dc.SetTextColor(m_TextColor[item]);
		// calculate text coordinates and output text
		dc.SetBkMode(TRANSPARENT);
		dc.SetTextAlign(TA_CENTER);
		dc.TextOut(r.left + r.Width() / 2, r.top, GetPaneText(item));
	}
	// detach from the CDC object, otherwise the hDC will be
	// destroyed when the CDC object goes out of scope
	dc.Detach();
}

BOOL CColorStatusBar::SetIndicators(const UINT* lpIDArray, int nIDCount)
{
	UINT	nID, nStyle;
	int		nWidth;
	if (!CStatusBar::SetIndicators(lpIDArray, nIDCount))
		return(FALSE);
	m_PaneColor.SetSize(nIDCount);
	m_TextColor.SetSize(nIDCount);
	for (int i = 1; i < nIDCount; i++) {
		GetPaneInfo(i, nID, nStyle, nWidth);
		SetPaneInfo(i, nID, nStyle | SBPS_OWNERDRAW, nWidth);
		m_PaneColor[i] = -1;
		m_TextColor[i] = -1;
	}
	m_Indicators = nIDCount;
	return(TRUE);
}

void CColorStatusBar::EnablePane(int nIndex)
{
	// When a document opens, if a non-maximized child frame restores docked
	// control bars via SetDockState, owner-drawn status bar panes don't get 
	// updated when they should (in release mode only), because GetPaneStyle 
	// says the pane is disabled even though CCmdUI::Enable was passed TRUE.
	// If the pane should be enabled, call this function to force its style.
	SetPaneStyle(nIndex, GetPaneStyle(nIndex) & ~SBPS_DISABLED);
}

void CColorStatusBar::SetPaneColor(int nIndex, COLORREF Color)
{
	ASSERT(nIndex >= 0 && nIndex < m_Indicators);
	m_PaneColor[nIndex] = Color;
}

void CColorStatusBar::SetTextColor(int nIndex, COLORREF Color)
{
	ASSERT(nIndex >= 0 && nIndex < m_Indicators);
	m_TextColor[nIndex] = Color;
}

void CColorStatusBar::SetPaneWidth(int nIndex, int nWidth)
{
	ASSERT(nIndex >= 0 && nIndex < m_Indicators);
	UINT	nID, nStyle;
	int		nPrevWidth;
	GetPaneInfo(nIndex, nID, nStyle, nPrevWidth);
	SetPaneInfo(nIndex, nID, nStyle, nWidth);
}

BEGIN_MESSAGE_MAP(CColorStatusBar, CStatusBar)
	//{{AFX_MSG_MAP(CColorStatusBar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorStatusBar message handlers
