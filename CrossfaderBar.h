// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      28jan04 initial version

		crossfader dialog
 
*/

#if !defined(AFX_CROSSFADERBAR_H__E998E1E2_1E6A_4D7F_BEDE_7AE0EC55CE86__INCLUDED_)
#define AFX_CROSSFADERBAR_H__E998E1E2_1E6A_4D7F_BEDE_7AE0EC55CE86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CrossfaderBar.h : header file
//

#include "AutoSliderDlgBar.h"

/////////////////////////////////////////////////////////////////////////////
// CCrossfaderBar dialog

class CCrossfaderBar : public CAutoSliderDlgBar
{
public:
// Construction
	CCrossfaderBar();

// Constants
	enum FADER_TYPE {
		FADER_LINEAR,
		FADER_DIPLESS
	};

// Attributes
	float	GetPos() const;
	void	SetPos(float Pos);
	float	GetVolume(PVOID Source) const;
	int		GetFaderType() const;
	void	SetFaderType(int FaderType);

// Operations
	void	AddSource(PVOID Source);
	void	RemoveSource(PVOID Source);
	void	UpdateSourceName(PVOID Source);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCrossfaderBar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Nested classes
	class CMyComboBox : public CComboBox {
	public:
		int		FindItemDataPtr(PVOID pData) const;
		int		UpdateString(int nIndex, LPCTSTR lpszString);
	};

// Dialog Data
	//{{AFX_DATA(CCrossfaderBar)
	enum { IDD = IDD_CROSSFADER_BAR };
	CIconButton	m_FaderTypeButton;
	CMyComboBox	m_SrcA;
	CMyComboBox	m_SrcB;
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CCrossfaderBar)
	afx_msg void OnSelchangeSourceA();
	afx_msg void OnSelchangeSourceB();
	afx_msg void OnFaderType();
	//}}AFX_MSG
	afx_msg LRESULT OnInitDialog(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

// Constants
	enum {
		SRC_A,
		SRC_B,
		SOURCES
	};

// Member data
	CMyComboBox	*m_Src[SOURCES];
	PVOID	m_Sel[SOURCES];
	int		m_FaderType;

// Overridables
	virtual	LPCSTR	GetSourceName(PVOID Source);

// Helpers
	void	SetSource(int Idx);
	float	ApplyCurve(float Pos) const;
	NOCOPIES(CCrossfaderBar);
};

inline float CCrossfaderBar::GetPos() const
{
	return(float(m_Slider.GetNormPos()));
}

inline void CCrossfaderBar::SetPos(float Pos)
{
	m_Slider.SetNormPos(Pos);
}

inline float CCrossfaderBar::ApplyCurve(float Pos) const
{
	return(m_FaderType == FADER_LINEAR ? Pos : (Pos < .5 ? Pos * 2 : 1));
}

inline int CCrossfaderBar::GetFaderType() const
{
	return(m_FaderType);
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CROSSFADERBAR_H__E998E1E2_1E6A_4D7F_BEDE_7AE0EC55CE86__INCLUDED_)
