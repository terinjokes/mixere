// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      21nov03 initial version
		01		12dec03	use GetChannelRect to detect click outside channel
		02		16jan04	un-reverse arrow up/down, page up/page down, wheel
		03		25jan04	restore default must set focus
		04		22feb04	add undo
		05		12mar04	add undo for left, right, home, end keys
		06		12mar04	coalesce undo for wheel and all shortcut keys
		07		19apr04	if click was within thumb, don't jump to position
		08		29sep04	restore undo state notifies parent now

		slider with jump to position and default
 
*/

// ClickSliderCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "ClickSliderCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClickSliderCtrl

CClickSliderCtrl::CClickSliderCtrl()
{
	m_DefPos = 0;
}

CClickSliderCtrl::~CClickSliderCtrl()
{
}

int CClickSliderCtrl::PointToPos(CPoint point)
{
	int	rmin, rmax, rwid;
	GetRange(rmin, rmax);
	rwid = rmax - rmin;
	CRect	cr, tr;
	GetChannelRect(cr);
	GetThumbRect(tr);
	int pos = rmin + rwid * (point.x - cr.left - tr.Width() / 2)
		/ (cr.Width() - tr.Width());
	return(min(max(pos, rmin), rmax));	// clamp to range
}

void CClickSliderCtrl::PostPos()
{
	GetParent()->PostMessage(WM_HSCROLL,
		MAKELONG(SB_THUMBTRACK, GetPos()), long(this->m_hWnd));
}

void CClickSliderCtrl::PostNotification(int Code)
{
	GetParent()->PostMessage(WM_HSCROLL, Code, long(this->m_hWnd));
}

void CClickSliderCtrl::SaveUndoState(CUndoState& State)
{
	State.m_Val.x.i = GetPos();
}

void CClickSliderCtrl::RestoreUndoState(const CUndoState& State)
{
	SetPos(State.m_Val.x.i);
	PostPos();	// notify parent
}

BEGIN_MESSAGE_MAP(CClickSliderCtrl, CSliderCtrl)
	//{{AFX_MSG_MAP(CClickSliderCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClickSliderCtrl message handlers

void CClickSliderCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	NotifyUndoableEdit(0);
	if (nFlags & MKU_DEFAULT_POS) {	// restore default position
		SetPos(m_DefPos);
		PostPos();	// post HScroll to parent window
		SetFocus();	// must do this because we're not calling base class
		return;
	}
	CRect	tr;
	GetThumbRect(tr);
	if (!tr.PtInRect(point))	// if click was within thumb, don't jump
		SetPos(PointToPos(point));
	CSliderCtrl::OnLButtonDown(nFlags, point);
	// if click was outside channel, MFC doesn't post HScroll
	CRect	cr;
	GetChannelRect(cr);
	if (!cr.PtInRect(point))
		PostPos();
}

void CClickSliderCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// un-reverse arrow up/down, page up/page down
	switch (nChar) {
	case VK_UP:
		NotifyUndoableEdit(0, UE_COALESCE);
		SetPos(GetPos() + GetLineSize());
		PostNotification(TB_LINEUP);
		return;
	case VK_DOWN:
		NotifyUndoableEdit(0, UE_COALESCE);
		SetPos(GetPos() - GetLineSize());
		PostNotification(TB_LINEDOWN);
		return;
	case VK_LEFT:
	case VK_RIGHT:
	case VK_HOME:
	case VK_END:
		NotifyUndoableEdit(0, UE_COALESCE);
		break;
	case VK_PRIOR:
		NotifyUndoableEdit(0, UE_COALESCE);
		SetPos(GetPos() + GetPageSize());
		PostNotification(TB_PAGEUP);
		return;
	case VK_NEXT:
		NotifyUndoableEdit(0, UE_COALESCE);
		SetPos(GetPos() - GetPageSize());
		PostNotification(TB_PAGEDOWN);
		return;
	}
	CSliderCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CClickSliderCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// un-reverse wheel
	if (zDelta > 0)
		OnKeyDown(VK_UP, 0, 0);
	else
		OnKeyDown(VK_DOWN, 0, 0);
	return(TRUE);
}
