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

#if !defined(AFX_VOLUMEBAR_H__9D725823_F782_4C58_BBB8_01B240B648CA__INCLUDED_)
#define AFX_VOLUMEBAR_H__9D725823_F782_4C58_BBB8_01B240B648CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VolumeBar.h : header file
//

#include "AutoSliderDlgBar.h"

/////////////////////////////////////////////////////////////////////////////
// CVolumeBar dialog

class CVolumeBar : public CAutoSliderDlgBar
{
public:
// Construction
	CVolumeBar();

// Attributes
	float	GetVolume() const;
	void	SetVolume(float Volume);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVolumeBar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CVolumeBar)
	enum { IDD = IDD_VOLUME_BAR };
	//}}AFX_DATA

	// Generated message map functions
	//{{AFX_MSG(CVolumeBar)
	//}}AFX_MSG
	afx_msg LONG OnInitDialog(UINT wParam, LONG lParam);
	DECLARE_MESSAGE_MAP()

	NOCOPIES(CVolumeBar);
};

inline float CVolumeBar::GetVolume() const
{
	return(GetNormPos());
}

inline void CVolumeBar::SetVolume(float Volume)
{
	SetNormPos(Volume);
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VOLUMEBAR_H__9D725823_F782_4C58_BBB8_01B240B648CA__INCLUDED_)
