// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      11dec03 initial version

		general options property page
		
*/

#if !defined(AFX_OPTSGENERALDLG_H__20611F5D_A71B_4C58_A0C8_711C0F175600__INCLUDED_)
#define AFX_OPTSGENERALDLG_H__20611F5D_A71B_4C58_A0C8_711C0F175600__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptsGeneralDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptsGeneralDlg dialog

class COptsGeneralDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(COptsGeneralDlg)

// Construction
public:
	COptsGeneralDlg();
	~COptsGeneralDlg();

// Attributes
	bool	ShowToolTips() const;
	LPCSTR	GetAudioDevice() const;
	int		GetUndoLevels() const;

// Dialog Data
	//{{AFX_DATA(COptsGeneralDlg)
	enum { IDD = IDD_OPTS_GENERAL_DLG };
	CEdit	m_UndoLevelsEdit;
	CButton	m_UnlimitedUndo;
	CButton	m_ShowToolTipsBtn;
	CComboBox	m_AudioDevList;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COptsGeneralDlg)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Generated message map functions
	//{{AFX_MSG(COptsGeneralDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnUnlimitedUndo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Constants
	enum {
		DEFAULT_UNDO_LEVELS = 100
	};
	static	LPCSTR	AUDIO_DEVICE;
	static	LPCSTR	SHOW_TOOL_TIPS;
	static	LPCSTR	DEV_DEFAULT;
	static	LPCSTR	UNDO_LEVELS;

// Member data
	bool	m_ShowToolTips;
	CString	m_CurAudioDev;
	int		m_UndoLevels;

// Helpers
	void	SetAudioDevice();
	void	PopulateAudioDevList();
	void	UpdateUndoLevels(bool Save);

	NOCOPIES(COptsGeneralDlg);
};

inline bool COptsGeneralDlg::ShowToolTips() const
{
	return(m_ShowToolTips);
}

inline LPCSTR COptsGeneralDlg::GetAudioDevice() const
{
	return(m_CurAudioDev);
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTSGENERALDLG_H__20611F5D_A71B_4C58_A0C8_711C0F175600__INCLUDED_)
