// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      09jan04	initial version
		01		25sep04	make header height public

        specialize header control for FormListView
 
*/

#if !defined(AFX_FORMLISTHEADER_H__7D9A9758_00B7_4AB0_BB98_A39386C2BA3F__INCLUDED_)
#define AFX_FORMLISTHEADER_H__7D9A9758_00B7_4AB0_BB98_A39386C2BA3F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FormListHeader.h : header file
//

class CFormListView;

/////////////////////////////////////////////////////////////////////////////
// CFormListHeader window

class CFormListHeader : public CHeaderCtrl
{
public:
// Construction
	CFormListHeader(CFormListView *View);
	virtual ~CFormListHeader();

// Constants
	static const int HEADER_HEIGHT;

// Attributes
	int		GetSortCol() const;
	bool	GetSortDesc() const;
	void	SetSort(int Col, bool Desc);
	int		GetWidth(int Col) const;
	void	SetWidth(int Col, int Width);

// Operations
	BOOL	Create();
	void	OnViewSize();
	void	OnViewHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void	Reset();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFormListHeader)
	//}}AFX_VIRTUAL

// Implementation
protected:
// Generated message map functions
	//{{AFX_MSG(CFormListHeader)
	afx_msg void OnItemChanging(NMHDR *pNotify, LRESULT *pResult);
	afx_msg void OnTrack(NMHDR *pNotify, LRESULT *pResult);
	afx_msg void OnBeginTrack(NMHDR *pNotify, LRESULT *pResult);
	afx_msg void OnEndTrack(NMHDR *pNotify, LRESULT *pResult);
	afx_msg void OnItemClick(NMHDR *pNotify, LRESULT *pResult);
	afx_msg void OnDividerDblClick(NMHDR *pNotify, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Member data
	CFormListView	*m_View;
	CWnd	*m_Frm;
	CRect	m_PrevSel;
	bool	m_HaveSel;
	int		m_SortCol;
	bool	m_SortDesc;
	HBITMAP	m_bmUp;
	HBITMAP	m_bmDown;

// Helpers
	void	DrawSortArrow(int Col, int Desc);
	NOCOPIES(CFormListHeader);
};

inline int CFormListHeader::GetSortCol() const
{
	return(m_SortCol);
}

inline bool CFormListHeader::GetSortDesc() const
{
	return(m_SortDesc);
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORMLISTHEADER_H__7D9A9758_00B7_4AB0_BB98_A39386C2BA3F__INCLUDED_)
