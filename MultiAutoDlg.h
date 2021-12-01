// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      27jan04	initial version

		multi-channel automation dialog
 
*/

#if !defined(AFX_MULTIAUTODLG_H__2D00BA51_4D78_4BAB_8D87_9590FCA565D7__INCLUDED_)
#define AFX_MULTIAUTODLG_H__2D00BA51_4D78_4BAB_8D87_9590FCA565D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MultiAutoDlg.h : header file
//

#include "AutoDlg.h"
#include "AutoSliderCtrl.h"

class CChannel;
class CMixereView;
class CAutoSliderCtrl;

/////////////////////////////////////////////////////////////////////////////
// CMultiAutoDlg dialog

class CMultiAutoDlg : public CAutoDlg
{
public:
// Construction
	CMultiAutoDlg(CMixereView& Mixer, int AutoIdx, CWnd* pParent = NULL);

// Attributes
	bool	IsModified() const;

// Operations
	void	TimerHook(float Scale);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMultiAutoDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
// Nested classes
	class CMyAutoSliderCtrl : public CAutoSliderCtrl {
		void	OnNewPos();
		void	OnSelection();
	};
// Dialog Data
	//{{AFX_DATA(CMultiAutoDlg)
	enum { IDD = IDD_MULTI_AUTO_DLG };
	CNumEdit	m_PosEdit;
	CNumEdit	m_StartEdit;
	CNumEdit	m_EndEdit;
	CButton	m_Stagger;
	CMyAutoSliderCtrl	m_Slider;
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CMultiAutoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnLoop();
	afx_msg void OnAutoVolume();
	afx_msg void OnAutoPan();
	afx_msg void OnAutoPitch();
	afx_msg void OnAutoPos();
	afx_msg void OnAutoStagger();
	afx_msg void OnToggleLoop();
	afx_msg void OnEditUndo();
	afx_msg void OnEditRedo();
	afx_msg void OnDeltaposAutoPosSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposAutoStartSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposAutoEndSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeAutoWaveform();
	afx_msg void OnGo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Constants
	enum {
		BM_POSITION		= 0x01,
		BM_SELECTION	= 0x02,
		BM_TIME			= 0x04,
		BM_TRANSPORT	= 0x08,
		BM_LOOP			= 0x10,
		BM_WAVEFORM		= 0x20
	};
	static	const float	m_PosSpinAmt;
	static	const int m_PosPrecision;

// Member data
	static	HACCEL	m_Accel;
	CMixereView&	m_Mixer;
	int		m_AutoIdx;
	bool	m_HandlerWrite;
	int		m_FirstChan;
	bool	m_IsModified;

// Overrides
	void	OnTimeChange();
	void	OnTransport(int Transport);
	void	OnModify();

// Helpers
	int		GetState(CAutoInfo& Info, float& Val, int& FirstChan) const;
	void	SetPosition(float Val);
	void	SetSelection(float fMin, float fMax);
	void	SetTime(int Time);
	void	SetTransport(int Transport);
	void	SetLoop(bool Enable);
	void	SetWaveform(int Waveform);
	CAutoSliderCtrl	*GetSlider(int ChanIdx) const;
	CChannel	*GetChannel(int ChanIdx) const;
	void	SetPosWithinSel(int ChanIdx, float Pos);
	void	Stagger();
	void	UpdateCtrls();
	void	UpdatePosEdit();
	void	UpdateSelectionEdits();
	float	Normalize(float Pos);
	float	Denormalize(float Pos);

// Friends
	friend	CMyAutoSliderCtrl;
	NOCOPIES(CMultiAutoDlg);
};

inline bool CMultiAutoDlg::IsModified() const
{
	return(m_IsModified);
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MULTIAUTODLG_H__2D00BA51_4D78_4BAB_8D87_9590FCA565D7__INCLUDED_)
