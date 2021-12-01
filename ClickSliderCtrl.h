// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      21nov03 initial version

		slider with jump to position and default
 
*/

#if !defined(AFX_CLICKSLIDERCTRL_H__CE81CB93_5F8C_40E8_9373_234F67ACC924__INCLUDED_)
#define AFX_CLICKSLIDERCTRL_H__CE81CB93_5F8C_40E8_9373_234F67ACC924__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ClickSliderCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CClickSliderCtrl window

#include "Undoable.h"

class CClickSliderCtrl : public CSliderCtrl, public CUndoable
{
public:
// Construction
	CClickSliderCtrl();
	virtual ~CClickSliderCtrl();

// Attributes
	int		GetDefaultPos();
	void	SetDefaultPos(int Pos);

// Operations

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClickSliderCtrl)
	//}}AFX_VIRTUAL

// Implementation
protected:
// Generated message map functions
	//{{AFX_MSG(CClickSliderCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Constants
	enum {
		MKU_DEFAULT_POS = MK_SHIFT
	};

// Member data
	int		m_DefPos;

// Overrides
	void	SaveUndoState(CUndoState& State);
	void	RestoreUndoState(const CUndoState& State);

// Helpers
	int		PointToPos(CPoint point);
	void	PostPos();
	void	PostNotification(int Code);
	NOCOPIES(CClickSliderCtrl); 
};

inline int CClickSliderCtrl::GetDefaultPos()
{
	return(m_DefPos);
}

inline void CClickSliderCtrl::SetDefaultPos(int Pos)
{
	m_DefPos = Pos;
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLICKSLIDERCTRL_H__CE81CB93_5F8C_40E8_9373_234F67ACC924__INCLUDED_)
