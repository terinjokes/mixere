// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      04dec03 initial version

		abstract base class to manage column resizing
 
*/

#ifndef CCOLUMNRESIZER
#define CCOLUMNRESIZER

class CColumnResizer {
public:
	// constant data about columns
	typedef struct {
		int		NameID;
		int		SortDesc;
		int		DefaultWidth;
		int		MinWidth;
		int		Members;
	} COLDEF;
	typedef	CWnd *const *WNDLIST;
	CColumnResizer();
	virtual	~CColumnResizer();
	void	Create(const COLDEF *ColDef, int Columns);
	bool	SetColumnWidth(int Col, int Width);
	int		GetColumnWidth(int Col) const;
	void	UpdateRow(WNDLIST WndList);

protected:
	// variable data about columns
	typedef	struct {
		int		x;
		int		Width;
		int		WndIdx;
	} COLVAR;
	const	COLDEF	*m_ColDef;
	int		m_Columns;
	COLVAR	*m_ColVar;
	COLVAR	m_Parent;
	int		m_Delta;
	WNDLIST	m_WndList;

	void	MoveRow(int StartCol);
	void	MoveCol(int Col);
	void	MoveWnd(int WndIdx, int x, int Width);
	void	MakeColumnArray();
	virtual	int		GetRowCount() = 0;
	virtual	WNDLIST	GetWndList(int Row) = 0;
	virtual	int		GetHScroll() = 0;
	NOCOPIES(CColumnResizer);
};

#endif
