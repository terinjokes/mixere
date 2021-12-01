// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      21nov03 initial version
		01		25sep04	fix comment typos

        mixer application
 
*/

// Mixere.h : main header file for the MIXERE application
//

#if !defined(AFX_MIXERE_H__C3217868_A2D9_42D7_9391_AA364468DE6A__INCLUDED_)
#define AFX_MIXERE_H__C3217868_A2D9_42D7_9391_AA364468DE6A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CMixere:
// See Mixere.cpp for the implementation of this class
//

class CMixere : public CWinApp
{
public:
// Construction
	CMixere();

// Constants
	static	LPCSTR	HOME_PAGE_URL;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMixere)
	public:
	virtual BOOL InitInstance();
	virtual BOOL IsIdleMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
private:
// Generated message map functions
	//{{AFX_MSG(CMixere)
	afx_msg void OnAppAbout();
	afx_msg void OnHomePage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	NOCOPIES(CMixere);
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MIXERE_H__C3217868_A2D9_42D7_9391_AA364468DE6A__INCLUDED_)
