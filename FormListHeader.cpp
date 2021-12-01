// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      09jan04	initial version
		01		11jan04	add reset
		02		18jan04	override create function
		03		19jan04	created by child frame now
		04		13feb04	created by mixer now
		05		17feb04	set window pointers in create, not ctor
		06		25sep04	get vertical position from view rect
		07		07nov04	implement minimum width
		08		15dec04	divider double-click now sets default width
		09		14jan05	OnItemClick now allows sort on leftmost column
		10		07feb05	OnItemChanging now only resizes if width has changed

        specialize header control for FormListView
 
*/

// FormListHeader.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "FormListHeader.h"
#include "FormListView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFormListHeader

const int CFormListHeader::HEADER_HEIGHT = 18;

CFormListHeader::CFormListHeader(CFormListView *View) :
	m_View(View)
{
	m_Frm = NULL;
	m_HaveSel = FALSE;
	m_SortCol = -1;
	m_SortDesc = FALSE;
	m_bmUp = (HBITMAP)LoadImage(AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDB_SORTU), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
	m_bmDown = (HBITMAP)LoadImage(AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDB_SORTD), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
}

CFormListHeader::~CFormListHeader()
{
	DeleteObject(m_bmUp);
	DeleteObject(m_bmDown);
}

BOOL CFormListHeader::Create()
{
	m_Frm = m_View->GetParent();
	CRect	r;
	m_Frm->GetClientRect(r);
	r.bottom = HEADER_HEIGHT;
	BOOL retc = CHeaderCtrl::Create(
		WS_CHILD | WS_VISIBLE | HDS_BUTTONS | HDS_FULLDRAG, r, m_Frm, 0);
	if (retc) {
		// tell the control to use non-bold text
		SendMessage(WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT));
	}
	return(retc);
}

void CFormListHeader::DrawSortArrow(int Col, int Desc)
{
	if (Col < 0)
		return;
	HDITEM	hdi;
	// mask tells GetItem which HDITEM members to retrieve; width
	// is mandatory, because SetItem reads it, regardless of mask
	hdi.mask = HDI_FORMAT | HDI_BITMAP | HDI_WIDTH;
	GetItem(Col, &hdi);
	if (Desc < 0) {	// erase
		hdi.mask = HDI_FORMAT;
		hdi.fmt &= ~HDF_BITMAP;
	} else {
		hdi.mask = HDI_FORMAT | HDI_BITMAP;
		hdi.fmt |= HDF_BITMAP | HDF_BITMAP_ON_RIGHT; 
		if (Desc)
			hdi.hbm = m_bmDown;
		else
			hdi.hbm = m_bmUp;
	}
	SetItem(Col, &hdi);
}

void CFormListHeader::Reset()
{
	DrawSortArrow(m_SortCol, -1);	// erase previous sort column's arrow
	m_SortCol = -1;
}

void CFormListHeader::SetSort(int Col, bool Desc)
{
	DrawSortArrow(m_SortCol, -1);	// erase previous sort column's arrow
	m_SortCol = Col;
	m_SortDesc = Desc;
	if (Col != -1)
		DrawSortArrow(Col, Desc);
}

void CFormListHeader::OnViewSize()
{
	CRect	fr, vr;
	m_Frm->GetClientRect(fr);
	m_View->GetWindowRect(vr);
	m_Frm->ScreenToClient(vr);
	fr.left -= m_View->GetScrollPosition().x;
	fr.bottom = vr.top;
	fr.top = vr.top - HEADER_HEIGHT;
	MoveWindow(fr);
}

int CFormListHeader::GetWidth(int Col) const
{
	HDITEM	hdi;
	hdi.mask = HDI_WIDTH;
	GetItem(Col, &hdi);
	return(hdi.cxy);
}

void CFormListHeader::SetWidth(int Col, int Width)
{
	HDITEM	hdi;
	hdi.mask = HDI_WIDTH;
	hdi.cxy = Width;
	SetItem(Col, &hdi);
}

BEGIN_MESSAGE_MAP(CFormListHeader, CHeaderCtrl)
	//{{AFX_MSG_MAP(CFormListHeader)
	ON_NOTIFY_REFLECT(HDN_ITEMCHANGING, OnItemChanging)
	ON_NOTIFY_REFLECT(HDN_TRACK, OnTrack)
	ON_NOTIFY_REFLECT(HDN_BEGINTRACK, OnBeginTrack)
	ON_NOTIFY_REFLECT(HDN_ENDTRACK, OnEndTrack)
	ON_NOTIFY_REFLECT(HDN_ITEMCLICK, OnItemClick)
	ON_NOTIFY_REFLECT(HDN_DIVIDERDBLCLICK, OnDividerDblClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFormListHeader message handlers

void CFormListHeader::OnItemChanging(NMHDR *pNotify, LRESULT *pResult)
{
	NMHEADER	*nmh = (NMHEADER *)pNotify;
	int	Col = nmh->iItem;
	int	Width = nmh->pitem->cxy;
	if (Width != m_View->GetColumnWidth(Col)) {	// if width has changed
		if (m_View->ResizeColumn(Col, Width)) {		// if resize succeeds
			// prevent headers from getting out of sync with columns when 
			// divider is dragged left while horz scroll bar is at far right
			OnViewSize();
		} else {
			*pResult = TRUE;	// resize failed, so prevent changes
			SetWidth(Col, m_View->GetColumnWidth(Col));
		}
	}
}

void CFormListHeader::OnTrack(NMHDR *pNotify, LRESULT *pResult)
{
	NMHEADER	*nmh = (NMHEADER *)pNotify;
	CRect	br, vr;
	GetItemRect(nmh->iItem, br);	// header item bounding rect
	m_View->GetClientRect(vr);
	m_View->MapWindowPoints(m_Frm, vr);	// convert viewport to our space
	int	x = br.left + nmh->pitem->cxy - m_View->GetScrollPosition().x;
	CRect	sel(x, vr.top, x + 1, vr.bottom + 1);
	CDC	*pDC = m_Frm->GetDC();
	ASSERT(pDC != NULL);
	if (m_HaveSel)	// un-invert previous selection if any
		pDC->InvertRect(m_PrevSel);
	pDC->InvertRect(sel);
	ReleaseDC(pDC);
	m_PrevSel = sel;	// save selection
	m_HaveSel = TRUE;
}

void CFormListHeader::OnBeginTrack(NMHDR *pNotify, LRESULT *pResult)
{
	m_HaveSel = FALSE;
}

void CFormListHeader::OnEndTrack(NMHDR *pNotify, LRESULT *pResult)
{
	if (m_HaveSel) {
		CDC	*pDC = m_Frm->GetDC();
		ASSERT(pDC != NULL);
		pDC->InvertRect(m_PrevSel);
		ReleaseDC(pDC);
	}
}

void CFormListHeader::OnItemClick(NMHDR *pNotify, LRESULT *pResult)
{
	NMHEADER	*nmh = (NMHEADER *)pNotify;
	int	Col = nmh->iItem;
	if (Col != m_SortCol) {		// if sort column has changed
		DrawSortArrow(m_SortCol, -1);	// erase previous sort column's arrow
		m_SortDesc = m_View->GetColumnInfo()[Col].SortDesc != 0;
	} else
		m_SortDesc ^= 1;	// same column, reverse sort
	m_SortCol = Col;
	DrawSortArrow(Col, m_SortDesc);	// draw arrow in current column
	if (Col >= 0)
		m_View->SortItems(Col, m_SortDesc);
}

void CFormListHeader::OnViewHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	OnViewSize();
}

void CFormListHeader::OnDividerDblClick(NMHDR *pNotify, LRESULT *pResult)
{
	NMHEADER	*nmh = (NMHEADER *)pNotify;
	int	Col = nmh->iItem;
	SetWidth(Col, m_View->GetColumnInfo()[Col].DefaultWidth);
}
