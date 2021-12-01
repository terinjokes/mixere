// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        visualcpp.net
 
		revision history:
		rev		date	comments
        00      07may03	ck: replaced leading underscores with m_ convention
		01		02oct04	ck: add optional origin shift to initial parent rect
 
		resize a windows control

*/

#include "stdafx.h"
#include "CtrlResize.h"

CCtrlResize::CControlInfo::CControlInfo(int controlID, int bindtype, CRect& rectInitial, CWnd* pControlWnd)
{
	m_controlID = controlID;
	m_bindtype = bindtype;
	m_rectInitial = rectInitial;
	m_pControlWnd = pControlWnd;
};

int CCtrlResize::AddControl(int controlID, int bindtype, CRect &rectInitial)
{
	m_aCtrls.Add(new CControlInfo(controlID, bindtype, rectInitial));
	return 0;
}

int CCtrlResize::FixControls()
{
	if (!m_pWnd)
		return 1;
	m_pWnd->GetClientRect(&m_rectInitialParent);
	m_pWnd->ScreenToClient(&m_rectInitialParent);
	
	for (int i = 0; i < m_aCtrls.GetSize(); i++) {
		m_aCtrls.GetAt(i)->m_pControlWnd = m_pWnd->GetDlgItem(m_aCtrls.GetAt(i)->m_controlID);
		if (m_aCtrls.GetAt(i)->m_pControlWnd) {
			m_aCtrls.GetAt(i)->m_pControlWnd->GetWindowRect(&m_aCtrls.GetAt(i)->m_rectInitial);
			m_pWnd->ScreenToClient(&m_aCtrls.GetAt(i)->m_rectInitial);
		}
	}
	return 0;
}

CCtrlResize::~CCtrlResize()
{
	for (int i = 0; i < m_aCtrls.GetSize(); i++)
		delete m_aCtrls.GetAt(i);
}

CCtrlResize::CCtrlResize() 
	: m_pWnd(NULL), m_originShift(CSize(0, 0))	// ck: added origin shift
{
}

void CCtrlResize::SetParentWnd(CWnd *pWnd)
{
	m_pWnd = pWnd;
}

void CCtrlResize::OnSize()
{
	if (!m_pWnd || !m_pWnd->IsWindowVisible())
		return;

	CRect rr, rectWnd;
	m_pWnd->GetClientRect(&rectWnd);
	
	for (int i = 0; i < m_aCtrls.GetSize(); i++) {
		rr = m_aCtrls.GetAt(i)->m_rectInitial;
		rr.OffsetRect(m_originShift);	// ck: shift controls by this amount
		if (m_aCtrls.GetAt(i)->m_bindtype & BIND_RIGHT) 
			rr.right = rectWnd.right -(m_rectInitialParent.Width() - m_aCtrls.GetAt(i)->m_rectInitial.right);
		if (m_aCtrls.GetAt(i)->m_bindtype & BIND_BOTTOM) 
			rr.bottom = rectWnd.bottom -(m_rectInitialParent.Height() - m_aCtrls.GetAt(i)->m_rectInitial.bottom);
		if (m_aCtrls.GetAt(i)->m_bindtype & BIND_TOP)
			;
		else
			rr.top = rr.bottom - m_aCtrls.GetAt(i)->m_rectInitial.Height();
		if (m_aCtrls.GetAt(i)->m_bindtype & BIND_LEFT)
			;
		else
			rr.left = rr.right - m_aCtrls.GetAt(i)->m_rectInitial.Width();
		
		m_aCtrls.GetAt(i)->m_pControlWnd->MoveWindow(&rr);
		m_aCtrls.GetAt(i)->m_pControlWnd->Invalidate(FALSE);
	}
}

void CCtrlResize::SetOriginShift(SIZE shift)	// ck: added
{
	m_originShift = shift;
}
