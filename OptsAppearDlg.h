// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      11dec03 initial version

		appearance options property page
		
*/

#if !defined(AFX_OPTSAPPEARDLG_H__0A571D4B_7E0E_4171_AD3E_B058B0826699__INCLUDED_)
#define AFX_OPTSAPPEARDLG_H__0A571D4B_7E0E_4171_AD3E_B058B0826699__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptsAppearDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptsAppearDlg dialog

class COptsAppearDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(COptsAppearDlg)

public:
// Construction
	COptsAppearDlg();
	~COptsAppearDlg();

// Attributes
	CFont&	GetFont();
	bool	ColorWasChanged() const;

// Dialog Data
	//{{AFX_DATA(COptsAppearDlg)
	enum { IDD = IDD_OPTS_APPEAR_DLG };
	CStatic	m_FontSample;
	CStatic	m_ColorSample;
	CComboBox	m_ColorList;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COptsAppearDlg)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COptsAppearDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChooseFont();
	afx_msg void OnDefaultFont();
	afx_msg void OnChooseColor();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSelchangeColorList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Constants
	enum COLOR {
		SELECTED,
		ACTIVE,
		COLORS
	};
	enum {
		MAX_CUSTOM_COLORS = 16,
		COLOR_SIZE = COLORS * sizeof(COLORREF),
		CUST_CLR_SIZE = MAX_CUSTOM_COLORS * sizeof(COLORREF)
	};
	static	LPCSTR	TITLE_FONT;
	static	LPCSTR	COLOR_PREFIX;
	static	LPCSTR	CUSTOM_COLOR;
	static	const LPCSTR	m_ColorName[COLORS];

// Member data
	CFont	m_Font;
	CFont	m_TempFont;
	int		m_ColorIdx;
	bool	m_ColorWasChanged;
	COLORREF	m_Color[COLORS];
	COLORREF	m_TempColor[COLORS];
	COLORREF	m_CustomColor[MAX_CUSTOM_COLORS];
	COLORREF	m_TempCustomColor[MAX_CUSTOM_COLORS];

// Helpers
	static	void	CopyFont(CFont& dst, CFont& src);
	void	GetColorState();
	void	WriteColorState();
	void	SetChannelColors();
	void	GetChannelColors();
	void	UpdateColors(bool Save);
	void	UpdateFontSample();
	int		HeightToPointSize(int Height);

	NOCOPIES(COptsAppearDlg);
};

inline CFont& COptsAppearDlg::GetFont()
{
	return(m_Font);
}

inline bool COptsAppearDlg::ColorWasChanged() const
{
	return(m_ColorWasChanged);
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTSAPPEARDLG_H__0A571D4B_7E0E_4171_AD3E_B058B0826699__INCLUDED_)
