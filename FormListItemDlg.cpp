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

// FormListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "FormListItemDlg.h"
#include "FormListView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFormListItemDlg dialog

CFormListItemDlg::CFormListItemDlg(UINT nIDTemplate, CFormListView& View)
	: CDialog(nIDTemplate, NULL), m_View(View)
{
	//{{AFX_DATA_INIT(CFormListItemDlg)
	//}}AFX_DATA_INIT
	m_Index = 0;
	m_Selected = FALSE;
	m_Active = FALSE;
	m_Empty = TRUE;
	m_WasSel = FALSE;
}

CFormListItemDlg::~CFormListItemDlg()
{
	DestroyWindow();
}

void CFormListItemDlg::SetIndex(int Index)
{
	m_Index = Index;
}

void CFormListItemDlg::GetInfo(void *Info) const
{
	((CFormListInfo *)Info)->m_Index = m_Index;
}

void CFormListItemDlg::SetInfo(const void *Info)
{
	m_Index = ((CFormListInfo *)Info)->m_Index;
}

void CFormListItemDlg::SelectItem(bool Enable)
{
	m_Selected = Enable;
}

void CFormListItemDlg::ActivateItem(bool Enable)
{
	m_Active = Enable;
}

bool CFormListItemDlg::OpenItem(const void *Info)
{
	return(FALSE);
}

void CFormListItemDlg::CloseItem()
{
}

void CFormListItemDlg::OnSelectItem(UINT nFlags)
{
	m_View.OnSelect(this, nFlags);
}

void CFormListItemDlg::OnActivateItem()
{
	m_View.OnActivate(this);
}

void CFormListItemDlg::SaveUndoState(CUndoState& State)
{
}

void CFormListItemDlg::RestoreUndoState(const CUndoState& State)
{
}

CUndoState::UNDO_KEY CFormListItemDlg::GetUndoKey()
{
	return(m_Index + 1);	// zero is reserved for unused keys
}

/////////////////////////////////////////////////////////////////////////////
// CFormListItemDlg data exchange and message map

void CFormListItemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFormListItemDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFormListItemDlg, CDialog)
	//{{AFX_MSG_MAP(CFormListItemDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFormListItemDlg message handlers

BOOL CFormListItemDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST) {
		switch (pMsg->wParam) {
		case VK_TAB:
			break;
		default:
			return(FALSE);		// pass keyboard messages to parent window
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}
