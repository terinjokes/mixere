// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      28sep4	initial version

		tempo dialog bar
 
*/

#if !defined(AFX_TEMPOBAR_H__9D725823_F782_4C58_BBB8_01B240B648CA__INCLUDED_)
#define AFX_TEMPOBAR_H__9D725823_F782_4C58_BBB8_01B240B648CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TempoBar.h : header file
//

#include "AutoSliderDlgBar.h"
#include "NumEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CTempoBar dialog

class CTempoBar : public CAutoSliderDlgBar
{
public:
// Construction
	CTempoBar();

// Attributes
	float	GetBpm() const;
	void	SetBpm(float Bpm);
	float	GetNormTempo() const;
	void	SetNormTempo(float Tempo);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTempoBar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CTempoBar)
	enum { IDD = IDD_TEMPO_BAR };
	CSpinButtonCtrl	m_BpmSpin;
	CNumEdit	m_BpmEdit;
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CTempoBar)
	afx_msg void OnDeltaposBpmSpin(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg LONG OnInitDialog(UINT wParam, LONG lParam);
	DECLARE_MESSAGE_MAP()

// Constants
	static const float	m_NominalBpm;

// Overrides
	void	OnPosChange();

	NOCOPIES(CTempoBar);
};

inline float CTempoBar::GetBpm() const
{
	return(GetNormTempo() * m_NominalBpm);
}

inline void CTempoBar::SetBpm(float Bpm)
{
	SetNormTempo(Bpm / m_NominalBpm);
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEMPOBAR_H__9D725823_F782_4C58_BBB8_01B240B648CA__INCLUDED_)
