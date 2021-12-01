// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      04dec03 initial version
		01		10dec03	get current widths from derived object
		02		05jan04	don't apply margin correction to parent window
		03		07nov04	if control's width changes, must invalidate it
		04		07nov04	implement minimum width

		abstract base class to manage column resizing
 
*/

#include "stdafx.h"
#include "ColumnResizer.h"

CColumnResizer::CColumnResizer()
{
	m_ColDef = NULL;
	m_Columns = 0;
	m_ColVar = NULL;
	m_Delta = 0;
	m_WndList = NULL;
}

CColumnResizer::~CColumnResizer()
{
	delete [] m_ColVar;
}

void CColumnResizer::Create(const COLDEF *ColDef, int Columns)
{
	m_Columns = Columns;
	m_ColDef = ColDef;
	m_ColVar = new COLVAR[Columns];
	for (int i = 0; i < Columns; i++)
		m_ColVar[i].Width = ColDef[i].DefaultWidth;
	MakeColumnArray();
}

bool CColumnResizer::SetColumnWidth(int Col, int Width)
{
	ASSERT(Col >= 0 && Col < m_Columns);
	bool	retc = TRUE;
	if (Width < m_ColDef[Col].MinWidth) {	// if below min width
		Width = m_ColDef[Col].MinWidth;		// clamp width to min
		retc = FALSE;	// tells header control to abort tracking
	}
	m_Delta = Width - m_ColVar[Col].Width;
	m_ColVar[Col].Width = Width;
	MakeColumnArray();
	m_WndList = NULL;	// operate on all rows
	MoveRow(Col);
	return(retc);
}

int CColumnResizer::GetColumnWidth(int Col) const
{
	ASSERT(Col >= 0 && Col < m_Columns);
	return(m_ColVar[Col].Width);
}

void CColumnResizer::UpdateRow(WNDLIST WndList)
{
	m_WndList = WndList;	// operate on this row only
	MoveRow(0);		// start from first column
}

void CColumnResizer::MakeColumnArray()
{
	int	x = 0;
	int	WndIdx = 0;
	for (int i = 0; i < m_Columns; i++) {
		m_ColVar[i].x = x;
		m_ColVar[i].WndIdx = WndIdx;
		x += m_ColVar[i].Width;
		WndIdx += max(m_ColDef[i].Members, 1);
	}
	m_Parent.x = 0;
	m_Parent.Width = x;
	m_Parent.WndIdx = WndIdx;
}

void CColumnResizer::MoveRow(int StartCol)
{
	ASSERT(StartCol >= 0 && StartCol < m_Columns);
	if (m_Delta > 0) {	// if we're widening
		MoveCol(-1);	// widen parent window
		// move columns right, in right-to-left order
		for (int i = m_Columns - 1; i >= StartCol; i--)
			MoveCol(i);
	} else {			// if we're narrowing
		// move columns left, in left-to-right order
		for (int i = StartCol; i < m_Columns; i++)
			MoveCol(i);
		MoveCol(-1);	// narrow parent window
	}
}

void CColumnResizer::MoveCol(int Col)
{
	int	Members = m_ColDef[Col].Members;
	if (Col < 0)	// if parent window
		MoveWnd(m_Parent.WndIdx, m_Parent.x, m_Parent.Width);
	else {
		ASSERT(Col < m_Columns);
		COLVAR	*cp = &m_ColVar[Col];
		if (Members > 1) {	// if column group
			int	Width = cp->Width / Members;
			if (m_Delta > 0) {	// MoveWnd right, in right-to-left order
				for (int i = Members - 1; i >= 0; i--)
					MoveWnd(cp->WndIdx + i, cp->x + Width * i, Width);
			} else {			// MoveWnd left, in left-to-right order
				for (int i = 0; i < Members; i++)
					MoveWnd(cp->WndIdx + i, cp->x + Width * i, Width);
			}
		} else	// ordinary column
			MoveWnd(cp->WndIdx, cp->x, cp->Width);
	}
}

void CColumnResizer::MoveWnd(int WndIdx, int x, int Width)
{
	static const UINT LMARGIN = -2;
	static const UINT RMARGIN = 4;
	ASSERT(WndIdx >= 0 && WndIdx <= m_Parent.WndIdx);
	if (WndIdx == m_Parent.WndIdx)
		x -= GetHScroll();
	else {
		x += LMARGIN;
		Width = Width - (LMARGIN + RMARGIN);
	}
	int Rows = m_WndList == NULL ? GetRowCount() : 1;
	for (int i = 0; i < Rows; i++) {
		CRect	r;
		CWnd	*wp = m_WndList == NULL ? 
			GetWndList(i)[WndIdx] : m_WndList[WndIdx];
		wp->GetWindowRect(r);
		wp->GetParent()->ScreenToClient(r);
		int	PrevWidth = r.Width();
		r.left = x;
		r.right = x + Width;
		wp->MoveWindow(r);
		// if control width changed, must invalidate, otherwise certain control
		// types paint incorrectly, e.g. static control with horiz. centered text
		if (r.Width() != PrevWidth && WndIdx != m_Parent.WndIdx)
			wp->Invalidate(FALSE);
	}
}
