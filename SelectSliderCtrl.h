// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      21nov03 initial version

		slider with dynamic range selection 
 
*/

#if !defined(AFX_SELECTSLIDERCTRL_H__CE81CB93_5F8C_40E8_9373_234F67ACC924__INCLUDED_)
#define AFX_SELECTSLIDERCTRL_H__CE81CB93_5F8C_40E8_9373_234F67ACC924__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectSliderCtrl.h : header file
//

#include "ClickSliderCtrl.h"

#define	SB_SLIDER_SELECTION	1000	// non-standard scroll bar constant

/////////////////////////////////////////////////////////////////////////////
// CSelectSliderCtrl window

class CSelectSliderCtrl : public CClickSliderCtrl
{
public:
// Construction
	CSelectSliderCtrl();
	virtual ~CSelectSliderCtrl();

// Constants
	enum {	// selection change notification types
		SCN_NONE,
		SCN_BOTH,
		SCN_START,
		SCN_END,
		SCN_MOVE
	};

// Attributes
	bool	HasSelection() const;
	bool	IsSelecting() const;
	void	GetSelection(int& nMin, int& nMax) const;
	void	SetSelection(int nMin, int nMax);
	void	GetNormSelection(float& fMin, float& fMax) const;
	void	SetNormSelection(float fMin, float fMax);
	double	GetNormPos() const;
	void	SetNormPos(double Pos);
	void	GetRange(int& nMin, int& nMax) const;
	int		GetRangeMin() const;
	int		GetRangeMax() const;
	void	SetRange(int nMin, int nMax);
	void	SetNormDefault(float Pos);

// Operations
	void	ClearSel(BOOL bRedraw = FALSE);
	void	CreateTicks(int Count);
	void	Redraw();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectSliderCtrl)
	//}}AFX_VIRTUAL

// Implementation
protected:
// Generated message map functions
	//{{AFX_MSG(CSelectSliderCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSysColorChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Constants
	enum {
		UNDO_POSITION,
		UNDO_SELECTION
	};

// Member data
	bool	m_HasSelection;
	bool	m_Selecting;
	bool	m_Dragging;
	int		m_DownPos;
	int		m_StartPos;
	int		m_EndPos;
	bool	m_Moving;
	int		m_SelectLen;
	int		m_RangeMin;
	int		m_RangeMax;
	int		m_NumTics;
	CRect	m_ThumbRect;

	static	CBrush	m_BrChannel;
	static	CBrush	m_BrDisabled;
	static	CBrush	m_BrSelection;
	static	bool	m_IsColorSet;

// Overrides
	void	SaveUndoState(CUndoState& State);
	void	RestoreUndoState(const CUndoState& State);

// Helpers
	void	UpdateSelection(int SelType);
	void	NotifySelection(int SelType);
	double	SliderToNorm(int SliderPos) const;
	int		NormToSlider(double NormPos) const;
	int		Round(double x) const;
	void	MoveSelection(CPoint point);
	static	bool	SetColors();
	NOCOPIES(CSelectSliderCtrl);
};

inline bool CSelectSliderCtrl::HasSelection() const
{
	return(m_HasSelection);
}

inline bool CSelectSliderCtrl::IsSelecting() const
{
	return(m_Selecting);
}

inline void CSelectSliderCtrl::GetRange(int& nMin, int& nMax) const
{
	nMin = m_RangeMin;
	nMax = m_RangeMax;
}

inline int CSelectSliderCtrl::GetRangeMin() const
{
	return(m_RangeMin);
}

inline int CSelectSliderCtrl::GetRangeMax() const
{
	return(m_RangeMax);
}

inline void CSelectSliderCtrl::GetSelection(int& nMin, int& nMax) const
{
	nMin = m_StartPos;
	nMax = m_EndPos;
}

inline int CSelectSliderCtrl::Round(double x) const
{
	return(int(x < 0 ? x - 0.5 : x + 0.5));
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTSLIDERCTRL_H__CE81CB93_5F8C_40E8_9373_234F67ACC924__INCLUDED_)
