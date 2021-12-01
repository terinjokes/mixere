// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      21nov03 initial version

		file properties dialog
		
*/

#if !defined(AFX_PROPERTIESDLG_H__E699B1C2_0EF2_4723_B367_A10B71D9F476__INCLUDED_)
#define AFX_PROPERTIESDLG_H__E699B1C2_0EF2_4723_B367_A10B71D9F476__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertiesDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropertiesDlg dialog

class CPropertiesDlg : public CDialog
{
// Construction
public:
	CPropertiesDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPropertiesDlg)
	enum { IDD = IDD_PROPERTIES_DLG };
	int		m_ChannelCount;
	//}}AFX_DATA

// Overrides
protected:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertiesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Generated message map functions
	//{{AFX_MSG(CPropertiesDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	NOCOPIES(CPropertiesDlg);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTIESDLG_H__E699B1C2_0EF2_4723_B367_A10B71D9F476__INCLUDED_)
