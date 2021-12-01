// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      28sep04 initial version

		automation slider dialog
 
*/

#if !defined(AFX_AUTOSLIDERDLGBAR_H__4FAACFA4_71A0_4230_BE68_654E89AE406E__INCLUDED_)
#define AFX_AUTOSLIDERDLGBAR_H__4FAACFA4_71A0_4230_BE68_654E89AE406E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AutoSliderDlgBar.h : header file
//

#include "DialogBarEx.h"
#include "AutoSliderCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoSliderDlgBar dialog

class CAutoSliderDlgBar : public CDialogBarEx
{
public:
// Construction
	CAutoSliderDlgBar(UINT nIDAccel);
	virtual ~CAutoSliderDlgBar();

// Attributes
	void	GetInfo(CAutoInfo& Info) const;
	void	SetInfo(const CAutoInfo& Info);
	float	GetNormPos() const;
	void	SetNormPos(float Pos);

// Operations
	void	TimerHook();
	BOOL	EnableToolTips(BOOL bEnable);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoSliderDlgBar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
// Nested classes
	class CMyAutoSliderCtrl	: public CAutoSliderCtrl {
	public:
		void	OnNewPos();
	};
// Dialog data
	//{{AFX_DATA(CAutoSliderDlgBar)
	CMyAutoSliderCtrl	m_Slider;
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CAutoSliderDlgBar)
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	afx_msg LONG OnInitDialog(UINT wParam, LONG lParam);
	afx_msg	BOOL OnToolTip(UINT id, NMHDR* pTTTStruct, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

// Overrides
	void	SaveUndoState(CUndoState& State);
	void	RestoreUndoState(const CUndoState& State);
	void	GetUndoTitle(const CUndoState& State, CString& Title);

// Overridables
	virtual	void	OnPosChange();

// Friends
	friend	CMyAutoSliderCtrl;

	NOCOPIES(CAutoSliderDlgBar);
};

inline float CAutoSliderDlgBar::GetNormPos() const
{
	return(float(m_Slider.GetNormPos()));
}

inline void CAutoSliderDlgBar::SetNormPos(float Pos)
{
	m_Slider.SetNormPos(Pos);
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOSLIDERDLGBAR_H__4FAACFA4_71A0_4230_BE68_654E89AE406E__INCLUDED_)
