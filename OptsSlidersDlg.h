// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      11dec03 initial version

		sliders options property page
		
*/

#if !defined(AFX_OPTSSLIDERSDLG_H__3018796B_6317_4818_BA72_9EE2F0782017__INCLUDED_)
#define AFX_OPTSSLIDERSDLG_H__3018796B_6317_4818_BA72_9EE2F0782017__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptsSlidersDlg.h : header file
//

#include "ClickSliderCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// COptsSlidersDlg dialog

class COptsSlidersDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(COptsSlidersDlg)

// Construction
public:
	COptsSlidersDlg();
	~COptsSlidersDlg();

// Dialog Data
	//{{AFX_DATA(COptsSlidersDlg)
	enum { IDD = IDD_OPTS_SLIDERS_DLG };
	CStatic	m_AnimSpeedSliderMin;
	CStatic	m_AnimSpeedSliderMax;
	CClickSliderCtrl	m_AnimSpeedSlider;
	CButton	m_AutoVolUnpause;
	CButton	m_AutoMClick;
	CButton	m_AutoTrigLoopOff;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COptsSlidersDlg)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Generated message map functions
	//{{AFX_MSG(COptsSlidersDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Constants
	enum {
		MAX_ANIM_SPEED = 10
	};
	static	LPCSTR	ANIM_SPEED;
	static	LPCSTR	AUTO_OPTIONS;
	static	LPCSTR	CHAN_OPTIONS;

// Member data
	int		m_AnimSpeed;
	int		m_AutoOptions;
	int		m_ChanOptions;

	NOCOPIES(COptsSlidersDlg);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTSSLIDERSDLG_H__3018796B_6317_4818_BA72_9EE2F0782017__INCLUDED_)
