// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      28sep04	initial version

		volume dialog bar
 
*/

// VolumeBar.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "VolumeBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVolumeBar dialog


CVolumeBar::CVolumeBar() :
	CAutoSliderDlgBar(NULL)
{
	//{{AFX_DATA_INIT(CVolumeBar)
	//}}AFX_DATA_INIT
}

void CVolumeBar::DoDataExchange(CDataExchange* pDX)
{
	CAutoSliderDlgBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVolumeBar)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CVolumeBar, CAutoSliderDlgBar)
	//{{AFX_MSG_MAP(CVolumeBar)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_INITDIALOG, OnInitDialog)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVolumeBar message handlers

LONG CVolumeBar::OnInitDialog(UINT wParam, LONG lParam)
{
	CAutoSliderDlgBar::OnInitDialog(wParam, lParam);
	SetWindowText(LDS(VOLUME_BAR));
	SetBarCaption(LDS(VOLUME_BAR));

	return TRUE;
}
