// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      21nov03 initial version
		01		02dec03	right-click must always notify
		02		07dec03	during drag, notify for every mouse move
		03		26dec03	add show selection
		04		29dec03	show selection: notify parent, save and restore Z-pows
		05		08jan04	remove show/hide selection, implement custom draw
		06		26jan04	move normalization here
		07		29jan04	change get/set pos to use double
		08		17feb04	add move selection
		09		19feb04	when moving selection, center it around cursor
		10		22feb04	add undo
		11		19apr04	optimize custom draw; shadow range and number of tics
		12		24jun04	add SetNormDefault
		13		27jun04	fatten short selections so they're always visible
		14		27jun04	add selection change notification types
		15		26sep04	if new selection via right-click, notify both points
		16		29sep04	restore undo state notifies parent now
		17		06nov04	get disabled and selection brush colors from system
		18		27dec04	test SetNormSelection's args for -1 instead of equality

		slider with dynamic range selection 
 
*/

// SelectSliderCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "SelectSliderCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectSliderCtrl

static const DRAG_TOLERANCE = 4;	// in logical coordinates

CBrush	CSelectSliderCtrl::m_BrChannel;
CBrush	CSelectSliderCtrl::m_BrDisabled;
CBrush	CSelectSliderCtrl::m_BrSelection;

// since our brushes are static, we can init them during startup
bool	CSelectSliderCtrl::m_IsColorSet = SetColors();

CSelectSliderCtrl::CSelectSliderCtrl()
{
	m_Dragging = m_Selecting = m_HasSelection = FALSE;
	m_DownPos = m_StartPos = m_EndPos = -1;
	m_Moving = FALSE;
	m_SelectLen = 0;
	m_RangeMin = 0;
	m_RangeMax = 100;
	m_NumTics = 0;
}

CSelectSliderCtrl::~CSelectSliderCtrl()
{
}

void CSelectSliderCtrl::SetRange(int nMin, int nMax)
{
	CClickSliderCtrl::SetRange(m_RangeMin = nMin, m_RangeMax = nMax);
}

void CSelectSliderCtrl::SetNormDefault(float Pos)
{
	CClickSliderCtrl::SetDefaultPos(NormToSlider(Pos));
}

void CSelectSliderCtrl::SetSelection(int nMin, int nMax)
{
	if (nMin == -1 && nMax == -1)
		ClearSel(TRUE);
	else {
		m_StartPos = nMin;
		m_EndPos = nMax;
		m_Selecting = FALSE;
		m_HasSelection = TRUE;
		UpdateSelection(SCN_NONE);	// don't notify
	}
}

void CSelectSliderCtrl::ClearSel(BOOL bRedraw)
{
	m_Selecting = m_HasSelection = FALSE;
	m_StartPos = m_EndPos = -1;
	CClickSliderCtrl::ClearSel(bRedraw);
}

void CSelectSliderCtrl::CreateTicks(int Count)
{
	m_NumTics = Count;
}

void CSelectSliderCtrl::Redraw()
{
	// force update of background color
	ModifyStyle(WS_VISIBLE, 0);
	ModifyStyle(0, WS_VISIBLE);
}

void CSelectSliderCtrl::UpdateSelection(int SelType)
{
	CClickSliderCtrl::SetSelection(m_StartPos, m_EndPos);
	CRect	r;
	GetChannelRect(r);
	InvalidateRect(r);	// only need to redraw channel area
	if (SelType != SCN_NONE)
		NotifySelection(SelType);
}

void CSelectSliderCtrl::NotifySelection(int SelType)
{
	GetParent()->PostMessage(WM_HSCROLL, 
		MAKELONG(SB_SLIDER_SELECTION, SelType), (LONG)m_hWnd);
}

void CSelectSliderCtrl::GetNormSelection(float& fMin, float& fMax) const
{
	if (HasSelection()) {
		fMin = float(SliderToNorm(m_StartPos));
		fMax = float(SliderToNorm(m_EndPos));
	} else
		fMin = fMax = -1;
}

void CSelectSliderCtrl::SetNormSelection(float fMin, float fMax)
{
	if (fMin == -1 && fMax == -1)
		ClearSel(TRUE);
	else
		SetSelection(NormToSlider(fMin), NormToSlider(fMax));
}

double CSelectSliderCtrl::GetNormPos() const
{
	return(SliderToNorm(GetPos()));
}

void CSelectSliderCtrl::SetNormPos(double Pos)
{
	SetPos(NormToSlider(Pos));
}

double CSelectSliderCtrl::SliderToNorm(int SliderPos) const
{
	int	RangeMin, RangeMax;
	GetRange(RangeMin, RangeMax);
	return((SliderPos - RangeMin) / double(RangeMax - RangeMin));
}

int CSelectSliderCtrl::NormToSlider(double NormPos) const
{
	int	RangeMin, RangeMax;
	GetRange(RangeMin, RangeMax);
	return(Round(NormPos * (RangeMax - RangeMin) + RangeMin));
}

void CSelectSliderCtrl::MoveSelection(CPoint point)
{
	int	pos = PointToPos(point);
	// clamp position so selection can't be clipped
	int	halfsel = m_SelectLen / 2;
	pos = max(halfsel, min(pos, GetRangeMax() - halfsel));
	m_StartPos = pos - halfsel;
	m_EndPos = pos + halfsel;
	UpdateSelection(SCN_MOVE);
}

bool CSelectSliderCtrl::SetColors()
{
	m_BrChannel.CreateSolidBrush(RGB(220, 220, 220));
	m_BrDisabled.CreateSolidBrush(GetSysColor(COLOR_3DFACE));	// array lookup
	m_BrSelection.CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT));
	return(TRUE);	// return value so we can run during startup
}

void CSelectSliderCtrl::SaveUndoState(CUndoState& State)
{
	switch (State.GetCode()) {
	case UNDO_POSITION:
		State.m_Val.x.i = GetPos();
		PostPos();	// notify parent
		break;
	case UNDO_SELECTION:
		int	MinSel, MaxSel;
		GetSelection(MinSel, MaxSel);
		State.m_Val.x.i = MinSel;
		State.m_Val.y.i = MaxSel;
		NotifySelection(SCN_BOTH);	// notify parent
		break;
	default:
		NODEFAULTCASE;
	}
}

void CSelectSliderCtrl::RestoreUndoState(const CUndoState& State)
{
	switch (State.GetCode()) {
	case UNDO_POSITION:
		SetPos(State.m_Val.x.i);
		break;
	case UNDO_SELECTION:
		SetSelection(State.m_Val.x.i, State.m_Val.y.i);
		break;
	default:
		NODEFAULTCASE;
	}
}

BEGIN_MESSAGE_MAP(CSelectSliderCtrl, CClickSliderCtrl)
	//{{AFX_MSG_MAP(CSelectSliderCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomdraw)
	ON_WM_SYSCOLORCHANGE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectSliderCtrl message handlers

void CSelectSliderCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CClickSliderCtrl::OnLButtonDown(nFlags, point);
}

void CSelectSliderCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	SetFocus();
	if (nFlags & MK_CONTROL) {
		if (m_HasSelection) {
			NotifyUndoableEdit(UNDO_SELECTION);
			// enter moving mode; mouse slides entire selection
			m_Moving = TRUE;
			m_SelectLen = m_EndPos - m_StartPos;
			// set cursor to horizontal two-headed arrow
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
			SetCapture();	// track mouse even if it leaves our window
			MoveSelection(point);
		}
	} else {
		NotifyUndoableEdit(UNDO_SELECTION);
		// if outside slider channel, just clear selection
		CRect	r;
		GetChannelRect(r);
		if (!r.PtInRect(point))
			ClearSel(TRUE);
		else {
			// otherwise save cursor position and enter selecting mode
			m_DownPos = PointToPos(point);
			m_Selecting = TRUE;
			m_Dragging = FALSE;
			SetCapture();	// track mouse even if it leaves our window
		}
		NotifySelection(SCN_NONE);
	}
	CClickSliderCtrl::OnRButtonDown(nFlags, point);
}

void CSelectSliderCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	if (m_Selecting) {
		// if we never entered dragging mode, extend/trim current selection
		int	SelType = SCN_BOTH;
		if (!m_Dragging) {
			int pos = PointToPos(point);
			// if no current selection, select from current position
			if (!m_HasSelection) {
				m_StartPos = GetPos();
				m_EndPos = GetPos();
			}
			// if we're left of the middle of the current selection,
			// extend/trim start point, otherwise extend/trim end point
			int	midpoint = (m_EndPos + m_StartPos) / 2;
			if (pos < midpoint) {
				m_StartPos = pos;
				SelType = SCN_START;	// notify start point only
			} else {
				m_EndPos = pos;
				SelType = SCN_END;		// notify end point only
			}
			if (!m_HasSelection)		// new selection, notify both points
			   SelType = SCN_BOTH;
			SetSelection(m_StartPos, m_EndPos);
		}
		// exit selecting mode, redraw control, and notify parent
		m_Selecting = FALSE;
		m_HasSelection = TRUE;
		m_Dragging = FALSE;
		UpdateSelection(SelType);
		ReleaseCapture();
	} else {
		if (m_Moving) {
			m_Moving = FALSE;
			ReleaseCapture();
		}
	}
}

void CSelectSliderCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_Selecting) {
		int pos = PointToPos(point);
		if (!m_Dragging) {
			// if mouse has moved too far away from 
			// button down position, enter drag mode
			if (abs(pos - m_DownPos) >= DRAG_TOLERANCE) {
				m_Dragging = TRUE;
				m_HasSelection = TRUE;	// drag in progress is a selection
			}
		}
		// if dragging mode, track mouse with selection bar
		if (m_Dragging) {
			m_StartPos = m_DownPos;
			m_EndPos = pos;
			if (m_StartPos > m_EndPos) {	// sort endpoints
				int	i = m_StartPos;
				m_StartPos = m_EndPos;
				m_EndPos = i;
			}
			UpdateSelection(SCN_BOTH);
		}
	} else {
		// if moving mode, mouse slides entire selection
		if (m_Moving)
			MoveSelection(point);
	}
	CClickSliderCtrl::OnMouseMove(nFlags, point);
}

void CSelectSliderCtrl::OnCustomdraw(NMHDR *pNMHDR, LRESULT *pResult) 
{
	NMCUSTOMDRAW	*pNMCD = (NMCUSTOMDRAW *)pNMHDR;
	switch (pNMCD->dwDrawStage) {
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		GetThumbRect(m_ThumbRect);	// so derived handler needn't send message
		break;
	case CDDS_ITEMPREPAINT:
		if (pNMCD->dwItemSpec == TBCD_CHANNEL) {
			// draw channel background
			CBrush	*pBr;
			if (IsWindowEnabled())
				pBr = &m_BrChannel;
			else
				pBr = &m_BrDisabled;
			FillRect(pNMCD->hdc, &pNMCD->rc, *pBr);
			CRect	cr(&pNMCD->rc);	// channel rect
			cr.DeflateRect(m_ThumbRect.Width() / 2, 3);	// max selection rect
			// draw selection range
			if (HasSelection()) {
				// draw selection range
				int SelMin, SelMax;
				GetSelection(SelMin, SelMax);
				int RangeMin, RangeMax;
				GetRange(RangeMin, RangeMax);
				float	scale = cr.Width() / float(RangeMax - RangeMin);
				CRect	sr;		// actual selection rect
				sr.left =  int(cr.left + (SelMin - RangeMin) * scale + 0.5);
				sr.right = int(cr.left + (SelMax - RangeMin) * scale + 0.5);
				// if selection is too small, fatten it
				if (sr.right == sr.left) {	
					if (sr.left < cr.CenterPoint().x)	// fatten towards middle
						sr.right++;
					else
						sr.left--;
				}
				sr.top = cr.top;
				sr.bottom = cr.bottom;
				FillRect(pNMCD->hdc, &sr, m_BrSelection);
			}
			// draw channel edge
			DrawEdge(pNMCD->hdc, &pNMCD->rc, EDGE_SUNKEN, BF_RECT);
			// draw tics along bottom edge of window
			CRect	wr;
			GetClientRect(wr);
			float	dx = float(cr.Width() - 1);	// tic delta x
			if (m_NumTics > 1)		// avoid divide by zero
				dx /= (m_NumTics - 1);
			for (int i = 0; i < m_NumTics; i++) {
				if (dx > 0) {	// avoid drawing backwards
					int x = int(cr.left + dx * i + 0.5);
					if (x >= 0) {
						MoveToEx(pNMCD->hdc, x, wr.bottom, NULL);
						LineTo(pNMCD->hdc, x, wr.bottom - 4);
					}
				}
			}
			// skip default channel drawing
			*pResult = CDRF_SKIPDEFAULT;
		}
		break;
	}
}

void CSelectSliderCtrl::OnSysColorChange() 
{
	CClickSliderCtrl::OnSysColorChange();
	SetColors();
}
