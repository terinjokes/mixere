// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      21nov03 initial version
		01		17jan04	ignore space so it can be used as an accelerator key
		02		22feb04	add undo

        pushbutton with up/down icons
 
*/

// IconButton.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "IconButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIconButton

CIconButton::CIconButton()
{
	m_ResUp = 0;
	m_ResDown = 0;
	m_PrevCheck = -1;
}

CIconButton::~CIconButton()
{
}

void CIconButton::SetIcons(int ResUp, int ResDown)
{
	m_ResUp = ResUp;
	m_ResDown = ResDown;
}

void CIconButton::SaveUndoState(CUndoState& State)
{
	State.m_Val.x.i = GetCheck();
}

void CIconButton::RestoreUndoState(const CUndoState& State)
{
	SetCheck(State.m_Val.x.i);
	GetParent()->SendMessage(WM_COMMAND, 
		MAKELONG(GetDlgCtrlID(), BN_CLICKED), (long)m_hWnd);
}

BEGIN_MESSAGE_MAP(CIconButton, CButton)
	//{{AFX_MSG_MAP(CIconButton)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIconButton message handlers


HBRUSH CIconButton::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	int	check = GetCheck();
	if (check != m_PrevCheck) {
		SetIcon(AfxGetApp()->LoadIcon(check ? m_ResDown : m_ResUp));
		m_PrevCheck = check;
	}
	return NULL;
}

void CIconButton::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch (nChar) {
	case VK_SPACE:	// space is too useful an accelerator to waste on a button
		return;
	}
	CButton::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CIconButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	NotifyUndoableEdit(0);
	CButton::OnLButtonDown(nFlags, point);
}
