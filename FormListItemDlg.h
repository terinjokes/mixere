// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      21nov03 initial version

		abstract form list item
 
*/

#if !defined(AFX_FORMLISTDLG_H__E3CCBDCD_5996_4CFF_8B0E_24DC9F7A1E2C__INCLUDED_)
#define AFX_FORMLISTDLG_H__E3CCBDCD_5996_4CFF_8B0E_24DC9F7A1E2C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FormListDlg.h : header file
//

#include "Undoable.h"

class CFormListView;

class CFormListItemDlg : public CDialog, public CUndoable
{
public:
// Types
	typedef	CWnd *const *WNDLIST;

// Construction
	CFormListItemDlg(UINT nIDTemplate, CFormListView& View);
	virtual ~CFormListItemDlg();

// Attributes
	int		GetIndex() const;
	bool	IsSelected() const;
	bool	IsActive() const;
	bool	IsEmpty() const;
	bool	GetWasSel() const;
	void	SetWasSel(bool Enable);

// Overridables
	virtual	void	SetIndex(int Index);
	virtual	void	GetInfo(void *Info) const;
	virtual	void	SetInfo(const void *Info);
	virtual	void	SelectItem(bool Enable);
	virtual	void	ActivateItem(bool Enable);
	virtual	bool	OpenItem(const void *Info);
	virtual	void	CloseItem();
	virtual	WNDLIST	GetWndList() = 0;	// pure virtual

// Overrides
protected:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFormListItemDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog data
	//{{AFX_DATA(CFormListItemDlg)
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CFormListItemDlg)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

// Member data
	CFormListView&	m_View;
	int		m_Index;
	bool	m_Selected;
	bool	m_Active;
	bool	m_Empty;
	bool	m_WasSel;

// Handlers
	void	OnSelectItem(UINT nFlags);
	void	OnActivateItem();

// Undo overrides
	void	SaveUndoState(CUndoState& State);
	void	RestoreUndoState(const CUndoState& State);
	CUndoState::UNDO_KEY	GetUndoKey();

	NOCOPIES(CFormListItemDlg);
};

inline int CFormListItemDlg::GetIndex() const
{
	return(m_Index);
}

inline bool CFormListItemDlg::IsSelected() const
{
	return(m_Selected);
}

inline bool CFormListItemDlg::IsActive() const
{
	return(m_Active);
}

inline bool CFormListItemDlg::IsEmpty() const
{
	return(m_Empty);
}

inline bool CFormListItemDlg::GetWasSel() const
{
	return(m_WasSel);
}

inline void CFormListItemDlg::SetWasSel(bool Enable)
{
	m_WasSel = Enable;
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORMLISTDLG_H__E3CCBDCD_5996_4CFF_8B0E_24DC9F7A1E2C__INCLUDED_)
