// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      11dec03 initial version
		01		04jan05	convert from dialog to property sheet

		options property sheet
		
*/

// OptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "mixere.h"
#include "OptionsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg

IMPLEMENT_DYNAMIC(COptionsDlg, CPropertySheet)

COptionsDlg::COptionsDlg(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	Construct();
}

COptionsDlg::COptionsDlg(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	Construct();
}

COptionsDlg::~COptionsDlg()
{
}

void COptionsDlg::	Construct()
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
	AddPage(&m_General);
	AddPage(&m_Sliders);
	AddPage(&m_Appear);
	m_CurPage = 0;
}

BEGIN_MESSAGE_MAP(COptionsDlg, CPropertySheet)
	//{{AFX_MSG_MAP(COptionsDlg)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg message handlers

BOOL COptionsDlg::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	
	SetActivePage(m_CurPage);
	
	return bResult;
}

void COptionsDlg::OnDestroy() 
{
	m_CurPage = GetActiveIndex();

	CPropertySheet::OnDestroy();
}
