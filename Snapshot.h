// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      16jan05	initial version

		save and restore mixer states
 
*/

#ifndef CSNAPSHOT_INCLUDED
#define CSNAPSHOT_INCLUDED

#include <afxtempl.h>
#include "ChanInfo.h"
#include "UndoState.h"
#include "AccelTable.h"
#include "HotKeyList.h"

class CMixereView;
class CChannel;
class CEditSnapsDlg;

class CSnapshot {
public:
// Construction
	CSnapshot();
	~CSnapshot();

// Constants
	enum {	// restore types; must match m_RestoreTypeID array
		RT_EXACT,	// restore audio and automation positions in all cases
		RT_SMOOTH,	// avoid skips in audio and automations where possible
		RT_TYPES
	};
	enum {	// sort types; must match m_SortTypeID array
		SORT_TITLE,		// sort by title
		SORT_CREATED,	// sort by creation time; equivalent to sort by ID
		SORT_TYPES
	};
	enum {
		START_UCODE = 2000,	// so we don't overlap with mixer undo codes
		// start of undo codes; must match m_UndoTitleID array
		UCODE_TAKE,
		UCODE_RESTORE,
		UCODE_RENAME,
		UCODE_DELETE,
		UCODE_OPTIONS,
		UCODE_HOTKEY,
		// end of undo codes
		END_UCODE,
		UNDO_CODE_OFFSET = START_UCODE + 1,
		UNDO_CODES = END_UCODE - START_UCODE - 1
	};
	enum {
		MAX_TITLE = 64,		// maximum length of a snapshot title
		HOTKEY_CMD_BASE = WM_APP + 0x1000	// hot key message offset
	};

// Attributes
	CMixereView	*GetMixer() const;
	void	SetMixer(CMixereView *Mixer);
	int		GetCount() const;
	LPCSTR	GetTitle(int SnapIdx) const;
	void	SetTitle(int SnapIdx, LPCSTR Title);
	DWORD	GetID(int SnapIdx) const;
	DWORD	GetHotKey(int SnapIdx) const;
	bool	SetHotKey(int SnapIdx, DWORD Key);
	int		GetActive() const;
	void	SetActive(int SnapIdx);
	int		GetRestoreType() const;
	void	SetRestoreType(int RestoreType);
	int		GetSortType() const;
	void	SetSortType(int SortType);
	static	void	GetRestoreTypeName(int RestoreType, CString& Name);
	static	void	GetSortTypeName(int SortType, CString& Name);
	int		GetChanCount(int SnapIdx) const;
	int		CalcInfoSize(int Pos) const;
	int		GetInfoSize(LPCVOID SnapInfo) const;
	void	GetInfo(int Pos, LPVOID SnapInfo) const;
	void	SetInfo(int Pos, LPCVOID SnapInfo);
	HACCEL	GetAccel() const;

// Operations
	bool	Take();
	bool	Restore(int SnapIdx);
	void	Delete(int SnapIdx);
	bool	ShowChannels(int SnapIdx);
	void	EditList();
	void	UpdateList();
	int		FindByID(DWORD SnapID) const;
	void	Serialize(CArchive& ar, int Version, const CChanInfo *Chan, int Chans);
	bool	ValidateHotKey(DWORD Key, int Pos, LPCSTR Title, int& OwnerPos) const;

// Undo
	void	SetUndoParam(int Param);
	void	SaveUndoState(CUndoState& State);
	void	RestoreUndoState(const CUndoState& State);
	void	GetUndoTitle(const CUndoState& State, CString& Title);

private:
// Nested classes
	typedef struct CHAN_SNAP {	// channel snapshot: CChanInfo w/o path and title
		union {
			DWORD	m_ID;		// channel ID: overlaps m_Info
			BYTE	m_Info[sizeof(CChanInfo) - offsetof(CChanInfo, m_ID)];
		};
	};
	class CSnap {	// mixer snapshot
	public:
		DWORD	m_ID;		// snapshot's unique ID
		DWORD	m_Key;		// snapshot's shortcut key
		CString	m_Title;	// snapshot's title
		CArray<CHAN_SNAP, CHAN_SNAP&>	m_ChanSnap;	// array of channel snaps

		CSnap();	// CArray requires default ctor
		CSnap(const CSnap& Snap);
		CSnap&	operator=(const CSnap& Snap);
		void	Copy(const CSnap& Snap);
		int		GetChanCount() const;
		int		CalcInfoSize() const;
		void	GetInfo(LPVOID Info) const;
		void	SetInfo(LPCVOID Info);
	};
	typedef struct SNAP_INFO {	// variable-length struct
		int		m_Size;		// size of struct including variable portion
		DWORD	m_ID;		// snapshot's unique ID
		DWORD	m_Key;		// snapshot's shortcut key
		char	m_Title[MAX_TITLE + 1];	// snapshot's title, null-terminated
		int		m_Chans;	// number of channel snaps
		CHAN_SNAP	m_ChanSnap[1];	// variable portion: array of channel snaps
	};

// Constants
	static	const	int		m_RestoreTypeID[RT_TYPES];	// restore type string IDs
	static	const	int		m_SortTypeID[SORT_TYPES];	// sort type string IDs
	static	const	int		m_UndoTitleID[UNDO_CODES];	// undo title string IDs

// Member data
	static	CSnapshot	*m_This;	// passes our this to SortCompare
	CArray<CSnap, CSnap&>	m_Snap;	// array of mixer snapshots
	CMixereView	*m_Mixer;	// if non-NULL, pointer to attached mixer
	DWORD	m_SnapIDs;		// number of IDs we've generated so far
	DWORD	m_ActiveID;		// ID of most recently restored snapshot
	int		m_ActiveIdx;	// index of most recently restored snapshot
	BYTE	m_RestoreType;	// see restore type enum above
	BYTE	m_SortType;		// see sort type enum above
	CDWordArray	m_SortMap;	// array of snapshot indices for unsorting
	CEditSnapsDlg	*m_EditDlg;	// if non-NULL, pointer to edit dialog
	static	CHotKeyList	m_MainKeys;	// main frame's shortcut keys
	CHotKeyList	m_SnapKeys;	// our shortcut keys
	CAccelTable	m_Accel;	// our accelerator table
	int		m_UndoParam;	// additional data for undo notification

// Helpers
	int		Unsort(int SnapIdx) const;
	void	Restore(CChannel& Chan, const CChanInfo& Info);
	static	int		SortCompare(const void *elem1, const void *elem2);
	static	CHAN_SNAP&	GetChanSnap(CChanInfo& Info);
	void	GetSnapInfo(int Pos, CUndoState& State);

// Aliases for undo value members
	CUNDOSTATE_VAL(	UValActiveID,	WORD,	x.w.lo)	// active ID
	CUNDOSTATE_VAL(	UValPos,		WORD,	x.w.hi)	// list position
	CUNDOSTATE_VAL(	UValChans,		WORD,	y.w.lo)	// channel count
	CUNDOSTATE_VAL(	UValInsert,		WORD,	y.w.lo)	// insert flag
	CUNDOSTATE_VAL(	UValOptions,	WORD,	y.w.lo)	// options mask
	CUNDOSTATE_VAL(	UValHotKey,		UINT,	y.u)	// hot key

	NOCOPIES(CSnapshot);
};

inline CMixereView *CSnapshot::GetMixer() const
{
	return(m_Mixer);
}

inline void CSnapshot::SetMixer(CMixereView *Mixer)
{
	m_Mixer = Mixer;
}

inline int CSnapshot::GetCount() const
{
	return(m_Snap.GetSize());
}

inline int CSnapshot::Unsort(int SnapIdx) const
{
	ASSERT(SnapIdx >= 0 && SnapIdx < GetCount());
	return(m_SortMap[SnapIdx]);	// remove sort indirection
}

inline LPCSTR CSnapshot::GetTitle(int SnapIdx) const
{
	int	pos = Unsort(SnapIdx);	// remove sort indirection
	return(m_Snap.GetData()[pos].m_Title);	// GetData avoids copy
}

inline DWORD CSnapshot::GetID(int SnapIdx) const
{
	int	pos = Unsort(SnapIdx);	// remove sort indirection
	return(m_Snap.GetData()[pos].m_ID);	// GetData avoids copy
}

inline DWORD CSnapshot::GetHotKey(int SnapIdx) const
{
	int	pos = Unsort(SnapIdx);	// remove sort indirection
	return(m_Snap.GetData()[pos].m_Key);	// GetData avoids copy
}

inline int CSnapshot::GetActive() const
{
	return(m_ActiveIdx);
}

inline int CSnapshot::GetRestoreType() const
{
	return(m_RestoreType);
}

inline int CSnapshot::GetSortType() const
{
	return(m_SortType);
}

inline void CSnapshot::GetRestoreTypeName(int RestoreType, CString& Name)
{
	ASSERT(RestoreType >= 0 && RestoreType < RT_TYPES);
	Name.LoadString(m_RestoreTypeID[RestoreType]);
}

inline void CSnapshot::GetSortTypeName(int SortType, CString& Name)
{
	ASSERT(SortType >= 0 && SortType < SORT_TYPES);
	Name.LoadString(m_SortTypeID[SortType]);
}

inline CSnapshot::CHAN_SNAP& CSnapshot::GetChanSnap(CChanInfo& Info)
{
	return(*(CHAN_SNAP *)&Info.m_ID);
}

inline HACCEL CSnapshot::GetAccel() const
{
	return(m_Accel);
}

inline void CSnapshot::SetUndoParam(int Param)
{
	m_UndoParam = Param;
}

/////////////////////////////////////////////////////////////////////////////
// CSnapshot::CSnap inlines

inline CSnapshot::CSnap::CSnap()
{
}

inline CSnapshot::CSnap::CSnap(const CSnap& Snap)
{
	Copy(Snap);
}

inline CSnapshot::CSnap& CSnapshot::CSnap::operator=(const CSnap& Snap)
{
	Copy(Snap);
	return(*this);
}

inline int CSnapshot::CSnap::GetChanCount() const
{
	return(m_ChanSnap.GetSize());
}

inline int CSnapshot::CSnap::CalcInfoSize() const
{
	return(offsetof(SNAP_INFO, m_ChanSnap) + sizeof(CHAN_SNAP) * GetChanCount());
}

#endif 
