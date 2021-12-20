// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      21nov03 initial version

		mixer view
 
*/

// MixereView.h : interface of the CMixereView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MIXEREVIEW_H__21B166D3_F631_4A9B_9142_C0857ED3C3BF__INCLUDED_)
#define AFX_MIXEREVIEW_H__21B166D3_F631_4A9B_9142_C0857ED3C3BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxdlgs.h>	// for find/replace dialog
#include "FormListView.h"
#include "Channel.h"	// for transport enum
#include "MixerInfo.h"
#include "VolumeBar.h"
#include "TempoBar.h"
#include "MSFadeBar.h"
#include "SnapshotBar.h"

class CMixereDoc;
class CMixerInfo;
class CMultiMix;
class CMultiAutoDlg;
class CSnapshot;

class CMixereView : public CFormListView
{
// Construction
protected: // create from serialization only
	CMixereView();
	DECLARE_DYNCREATE(CMixereView)

public:
// Constants
	enum {	// columns: must match column name IDs
		COL_NUMBER,
		COL_NAME,
		COL_TRANSPORT,
		COL_MUTE,
		COL_VOLUME,
		COL_PAN,
		COL_PITCH,
		COL_POS,
		COLUMNS
	};
	enum {	// dialog bars: must match m_DlgBarInfo, and ctor's m_DlgBar init
		DLGBAR_VOLUME,
		DLGBAR_SNAPSHOT,
		DLGBAR_MS_FADE,
		DLGBAR_TEMPO,
		DLGBARS
	};
	enum {	// custom windows messages
		WM_SETDOCTITLE = WM_APP		// document title change notification
	};

// Attributes
	CMixereDoc	*GetDocument();
	CChannel	*GetChan(int Idx) const;
	void	GetInfo(CMixerInfo& Info) const;
	bool	SetInfo(const CMixerInfo& Info);
	int		GetMuteCount() const;
	int		GetSoloCount() const;
	float	GetVolume() const;
	void	SetVolume(float Volume);
	bool	IsDlgBarVisible(int BarIdx) const;
	BOOL	HasToolTips() const;
	void	SetTitleFont(CFont *Font);
	void	ApplyColorScheme();
	CSnapshot	*GetSnapshot() const;
	HACCEL	GetSnapshotAccel() const;

// Operations
	bool	LoadAudio(int Pos, LPCSTR Path);
	void	LoadAudio();
	void	DropFiles(HDROP hDropInfo);
	void	SetTransport(int Transport);
	void	TogglePlay();
	void	Go();
	void	SetLoop(int State);
	void	SetMute(int State);
	void	SetSolo(int State);
	void	EndSolo();
	void	KeepSolo();
	void	SetDefaults();
	void	ResetDefaults();
	BOOL	EnableToolTips(BOOL bEnable);
	void	ShowDlgBar(int BarIdx, bool Enable);
	void	ToggleDlgBar(int BarIdx);
	void	UpdateVolume();
	void	EditAuto(int AutoIdx);
	void	EditName();
	void	TimerHook();
	DWORD	CreateChanID();
	int		FindChanByID(DWORD ChanID) const;

// Channel handlers
	void	OnMute(CChannel *Chan, bool Enable);
	void	OnSolo(CChannel *Chan, bool Enable);
	void	OnDropFiles(CChannel *Chan, HDROP hDropInfo);
	void	OnContextMenu(CChannel *Item, CWnd* pWnd, CPoint point);
	const	CChanInfo& GetChanDefaults() const;
	int		GetMSFadeTime() const;

// Snapshot handlers
	void	OnUpdateSnapshotList();
	void	OnRestoreSnapshot(int SnapIdx);

// Overridables
	virtual	float	GetMasterVolume();

public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMixereView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

public:
	virtual ~CMixereView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Implementation
protected:
// Generated message map functions
	//{{AFX_MSG(CMixereView)
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnFileProperties();
	afx_msg void OnContextCopy();
	afx_msg void OnContextCut();
	afx_msg void OnContextInsert();
	afx_msg void OnContextDelete();
	afx_msg void OnTogglePlay();
	afx_msg void OnPlay();
	afx_msg void OnPause();
	afx_msg void OnStop();
	afx_msg void OnToggleLoop();
	afx_msg void OnToggleMute();
	afx_msg void OnToggleSolo();
	afx_msg void OnEndSolo();
	afx_msg void OnKeepSolo();
	afx_msg void OnLoadAudio();
	afx_msg void OnEditName();
	afx_msg void OnAutoVolume();
	afx_msg void OnAutoPan();
	afx_msg void OnAutoPitch();
	afx_msg void OnAutoPos();
	afx_msg void OnSetDefaults();
	afx_msg void OnUpdatePlay(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEndSolo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSetDefaults(CCmdUI* pCmdUI);
	afx_msg void OnMixerVolume();
	afx_msg void OnUpdateMixerVolume(CCmdUI* pCmdUI);
	afx_msg void OnMixerTempo();
	afx_msg void OnUpdateMixerTempo(CCmdUI* pCmdUI);
	afx_msg void OnMSFade();
	afx_msg void OnUpdateMSFade(CCmdUI* pCmdUI);
	afx_msg void OnGo();
	afx_msg void OnTakeSnapshot();
	afx_msg void OnEditSnapshots();
	afx_msg void OnSnapshots();
	afx_msg void OnUpdateSnapshots(CCmdUI* pCmdUI);
	afx_msg void OnEditFind();
	//}}AFX_MSG
	afx_msg LRESULT	OnSetDocTitle(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnFindReplace(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

protected:
// Nested classes
	class CMixerVolumeBar : public CVolumeBar {
	public:
		CMixerVolumeBar(CMixereView *Mixer);
		void	OnPosChange();

	private:
		CMixereView	*m_Mixer;
	};
	class CMixerTempoBar : public CTempoBar {
	public:
		CMixerTempoBar(CMixereView *Mixer);
		void	OnPosChange();

	private:
		CMixereView	*m_Mixer;
	};
	class CMixerMSFadeBar : public CMSFadeBar {
	public:
		CMixerMSFadeBar(CMixereView *Mixer);
		void	OnTimeChange();

	private:
		CMixereView	*m_Mixer;
	};
	class CEditAutoUndoState : public CAutoInfo {
	public:
		WORD	m_ChanIdx;
		BYTE	m_ChanTransport;
		BYTE	m_ChanLoop;
		int		m_ChanPos;
	};

// Types
#pragma pack(push, 1)	// 1-byte packing alignment
	typedef struct {
		int		m_ChanIdx;
		int		m_Pos;
		BYTE	m_Transport;
	} TRANSPORT_UNDO_STATE;
	typedef struct : TRANSPORT_UNDO_STATE {
		struct {
			double	m_Pos;
			BYTE	m_Transport;
		} m_Auto[CChanInfo::AUTOS];
	} GO_UNDO_STATE;
#pragma pack(pop)		// restore default packing
	typedef struct {
		int		ResID;
		int		UndoCode;
	} DLGBAR_INFO;

// Constants
	enum {
		START_UCODE = 1000,	// so we don't overlap base class undo codes
		// start of undo codes: must match m_UndoTitleID array
		UCODE_TRANSPORT,
		UCODE_GO,
		UCODE_LOOP,
		UCODE_MUTE,
		UCODE_SOLO,
		UCODE_END_SOLO,
		UCODE_KEEP_SOLO,
		UCODE_LOAD_AUDIO,
		UCODE_INSERT_AUDIO,
		UCODE_SHOW_VOLUME,
		UCODE_SHOW_SNAPSHOT,
		UCODE_SHOW_MS_FADE,
		UCODE_SHOW_TEMPO,
		UCODE_SET_DEFAULTS,
		UCODE_EDIT_AUTO,
		// end of undo codes
		END_UCODE,
		UNDO_CODE_OFFSET = START_UCODE + 1,
		UNDO_CODES = END_UCODE - START_UCODE - 1
	};
	static	const	CColumnResizer::COLDEF	m_ColumnInfo[COLUMNS];
	static	const	int		m_UndoTitleID[UNDO_CODES];
	static	const	DLGBAR_INFO	m_DlgBarInfo[DLGBARS];
	static	const	UINT	WM_FINDREPLACE;

// Member data
	// Components
	CMixerVolumeBar	m_VolumeBar;
	CMixerTempoBar	m_TempoBar;
	CMixerMSFadeBar	m_MSFadeBar;
	CSnapshotBar	m_SnapshotBar;
	CChanInfo	m_ChanDefaults;
	CFont	*m_TitleFont;
	CDialogBarEx	*m_DlgBar[DLGBARS];
	CString			m_DlgBarTitle[DLGBARS];
	CFrameWnd	*m_ChildFrm;
	CMultiMix	*m_MultiMix;
	CMultiAutoDlg	*m_MultiAutoDlg;
	CSnapshot	*m_Snapshot;
	// Scalars
	BOOL	m_HasToolTips;
	int		m_MuteCount;
	int		m_SoloCount;
	float	m_Tempo;
	DWORD	m_ChanIDs;
	// Undo
	int		m_UndoIdx;
	int		m_PrevPos;
	// Find
	CFindReplaceDialog	*m_FindDlg;
	CString	m_FindStr;
	bool	m_NewFind;
	bool	m_FindMatchCase;
	bool	m_FindSearchDown;

// Helpers
	bool	GetExtension(CString& Ext);
	static	void	AddStringUnique(CStringArray& StrArr, LPCSTR Str);
	static	void	MsgBoxStrList(LPCSTR Msg, const CStringArray& StrArr);
	void	CreateDlgBars();
	void	UpdateDlgBarTitles();
	bool	FindNext();

// Templates
	template<class T> void SetUndoChanIdxs(T *uap)
	{
		int	j = 0;
		for (int i = 0; i < GetItemCount(); i++) {
			if (IsSelOrCur(i))
				uap[j++].m_ChanIdx = i;
		}
	}

// Friends
	friend	CMixerVolumeBar;
	friend	CMixerTempoBar;
	friend	CMixerMSFadeBar;

// Overrides
	void	OnCreateView();
	CFormListItemDlg	*OnNewItem();
	int		MemberSortCompare(const void *elem1, const void *elem2);
	void	OnPaste(void *Info, int Count, bool Local);
	void	SaveUndoState(CUndoState& State);
	void	RestoreUndoState(const CUndoState& State);
	CUndoable	*FindUndoable(CUndoState::UNDO_KEY Key);
	void	GetUndoTitle(const CUndoState& State, CString& Title);

// Aliases for undo value members
	CUNDOSTATE_VAL(	UValBarNum,		BYTE,	y.c.al)	// dialog bar number
	CUNDOSTATE_VAL(	UValShowBar,	BYTE,	y.c.ah)	// show dialog bar
	CUNDOSTATE_VAL(	UValAutoIdx,	WORD,	y.w.lo)	// automation index

	NOCOPIES(CMixereView);
};

#ifndef _DEBUG  // debug version in CMixereView.cpp
inline CMixereDoc* CMixereView::GetDocument()
   { return (CMixereDoc*)m_pDocument; }
#endif

inline CChannel *CMixereView::GetChan(int Idx) const
{
	return((CChannel *)m_Item[Idx]);
}

inline float CMixereView::GetVolume() const
{
	return(m_VolumeBar.GetVolume());
}

inline int CMixereView::GetMuteCount() const
{
	return(m_MuteCount);
}

inline int CMixereView::GetSoloCount() const
{
	return(m_SoloCount);
}

inline BOOL CMixereView::HasToolTips() const
{
	return(m_HasToolTips);
}

inline const CChanInfo& CMixereView::GetChanDefaults() const
{
	return(m_ChanDefaults);
}

inline int CMixereView::GetMSFadeTime() const
{
	return(m_MSFadeBar.GetTime());
}

inline DWORD CMixereView::CreateChanID()
{
	return(++m_ChanIDs);
}

inline CSnapshot *CMixereView::GetSnapshot() const
{
	return(m_Snapshot);
}

inline HACCEL CMixereView::GetSnapshotAccel() const
{
	return(m_Snapshot->GetAccel());
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MIXEREVIEW_H__21B166D3_F631_4A9B_9142_C0857ED3C3BF__INCLUDED_)
