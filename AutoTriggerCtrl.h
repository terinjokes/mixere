// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      02feb04 initial version

		auto slider adapted for triggering
 
*/

#if !defined(AFX_AUTOTRIGGERCTRL_H__4AB9A141_DDD5_4B02_90B4_C9570EC90629__INCLUDED_)
#define AFX_AUTOTRIGGERCTRL_H__4AB9A141_DDD5_4B02_90B4_C9570EC90629__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AutoTriggerCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAutoTriggerCtrl window

#include "AutoSliderCtrl.h"

class CAutoTriggerCtrl : public CAutoSliderCtrl
{
public:
// Construction
	CAutoTriggerCtrl();
	virtual	~CAutoTriggerCtrl();

// Constants

// Attributes
	void	SetInfo(const CAutoInfo& Info);
	void	GetInfo(CAutoInfo& Info) const;
	void	SetNormPosNoEcho(double Pos);
	double	GetTriggerPos() const;
	void	SetTriggerPos(double Pos);
	void	StartFromPos(double Pos);

// Operations
	bool	TimerHook(float Scale = 1);
	void	Trigger();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoTriggerCtrl)
	//}}AFX_VIRTUAL

// Implementation
protected:
// Generated message map functions
	//{{AFX_MSG(CAutoTriggerCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Member data
	double	m_TriggerPos;

// Overrides
	void	OnTransport(int Transport);

// Overridables
	virtual	void	OnTrigger();

// Overrides

	NOCOPIES(CAutoTriggerCtrl);
};

inline double CAutoTriggerCtrl::GetTriggerPos() const
{
	return(m_TriggerPos);
}

inline void CAutoTriggerCtrl::SetTriggerPos(double Pos)
{
	m_TriggerPos = Pos;
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOTRIGGERCTRL_H__4AB9A141_DDD5_4B02_90B4_C9570EC90629__INCLUDED_)
