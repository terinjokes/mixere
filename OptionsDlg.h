// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      11dec03 initial version

		options property sheet
		
*/

#if !defined(AFX_OPTIONSDLG_H__4F2B5B65_6A0A_43FF_B5BA_6EF9A0F72A3D__INCLUDED_)
#define AFX_OPTIONSDLG_H__4F2B5B65_6A0A_43FF_B5BA_6EF9A0F72A3D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg

#include "OptsAppearDlg.h"
#include "OptsGeneralDlg.h"
#include "OptsSlidersDlg.h"

class COptionsDlg : public CPropertySheet
{
	DECLARE_DYNAMIC(COptionsDlg)

// Construction
public:
	COptionsDlg(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	COptionsDlg(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~COptionsDlg();

// Attributes
public:
	bool	ShowToolTips() const;
	LPCSTR	GetAudioDevice() const;
	int		GetUndoLevels() const;
	CFont&	GetFont();
	bool	ColorWasChanged() const;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsDlg)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:
// Generated message map functions
	//{{AFX_MSG(COptionsDlg)
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Member data
	COptsGeneralDlg	m_General;
	COptsSlidersDlg	m_Sliders;
	COptsAppearDlg	m_Appear;
	int		m_CurPage;

// Helpers
	void	Construct();

	NOCOPIES(COptionsDlg);
};

inline bool COptionsDlg::ShowToolTips() const
{
	return(m_General.ShowToolTips());
}

inline LPCSTR COptionsDlg::GetAudioDevice() const
{
	return(m_General.GetAudioDevice());
}

inline int COptionsDlg::GetUndoLevels() const
{
	return(m_General.GetUndoLevels());
}

inline CFont& COptionsDlg::GetFont()
{
	return(m_Appear.GetFont());
}

inline bool COptionsDlg::ColorWasChanged() const
{
	return(m_Appear.ColorWasChanged());
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSDLG_H__4F2B5B65_6A0A_43FF_B5BA_6EF9A0F72A3D__INCLUDED_)
