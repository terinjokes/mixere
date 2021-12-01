// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      16jan05	initial version
		01		01feb05	add undo
		02		02feb05	in Serialize, prune snaps during load
		03		03feb05	reserve space for hot key
		04		05feb05	implement hot keys

		save and restore mixer states
 
*/

#include "stdafx.h"
#include "Resource.h"
#include "Snapshot.h"
#include "MixereDoc.h"
#include "MixereView.h"
#include "EditSnapsDlg.h"
#include "TakeSnapDlg.h"

const int CSnapshot::m_RestoreTypeID[RT_TYPES] = {	// must match restore type enum
	IDS_SNAP_RT_EXACT,
	IDS_SNAP_RT_SMOOTH
};

const int CSnapshot::m_SortTypeID[SORT_TYPES] = {	// must match sort type enum
	IDS_SNAP_SORT_TITLE,
	IDS_SNAP_SORT_CREATED
};

const int CSnapshot::m_UndoTitleID[UNDO_CODES] = {	// must match undo code enum
	IDS_SSU_TAKE,
	IDS_SSU_RESTORE,
	IDS_SSU_RENAME,
	IDS_SSU_DELETE,
	IDS_SSU_OPTIONS,
	IDS_SSU_HOTKEY
};

CSnapshot *CSnapshot::m_This;

CHotKeyList	CSnapshot::m_MainKeys;

CSnapshot::CSnapshot()
{
	m_Mixer = NULL;
	m_SnapIDs = 0;
	m_ActiveID = 0;
	m_ActiveIdx = 0;
	m_RestoreType = RT_EXACT;
	m_SortType = SORT_TITLE;
	m_EditDlg = NULL;
	m_UndoParam = 0;
	if (!m_MainKeys.GetCount())	// first time only, load main accelerators
		m_MainKeys.LoadAccelRes(IDR_MAINFRAME);
}

CSnapshot::~CSnapshot()
{
}

void CSnapshot::SetTitle(int SnapIdx, LPCSTR Title)
{
	int	pos = Unsort(SnapIdx);	// remove sort indirection
	m_Snap[pos].m_Title = Title;
	UpdateList();
}

void CSnapshot::SetActive(int SnapIdx)
{
	m_ActiveIdx = SnapIdx;
}

bool CSnapshot::SetHotKey(int SnapIdx, DWORD Key)
{
	int	pos = Unsort(SnapIdx);	// remove sort indirection
	int	OwnerPos;
	if (!ValidateHotKey(Key, pos, m_Snap[pos].m_Title, OwnerPos))
		return(FALSE);
	if (OwnerPos >= 0)
		m_Snap[OwnerPos].m_Key = 0;
	m_Snap[pos].m_Key = Key;
	UpdateList();
	return(TRUE);
}

void CSnapshot::SetRestoreType(int RestoreType)
{
	m_RestoreType = RestoreType;
}

void CSnapshot::SetSortType(int SortType)
{
	m_SortType = SortType;
	UpdateList();
}

int CSnapshot::GetChanCount(int SnapIdx) const
{
	int	pos = Unsort(SnapIdx);	// remove sort indirection
	return(m_Snap.GetData()[pos].GetChanCount());	// GetData avoids copy
}

int CSnapshot::CalcInfoSize(int Pos) const
{
	return(m_Snap.GetData()[Pos].CalcInfoSize());	// GetData avoids copy
}

int CSnapshot::GetInfoSize(LPCVOID SnapInfo) const
{
	return(((SNAP_INFO *)SnapInfo)->m_Size);
}

void CSnapshot::GetInfo(int Pos, LPVOID SnapInfo) const
{
	m_Snap.GetData()[Pos].GetInfo(SnapInfo);	// GetData avoids copy
}
	
void CSnapshot::SetInfo(int Pos, LPCVOID SnapInfo)
{
	m_Snap.GetData()[Pos].SetInfo(SnapInfo);	// GetData avoids copy
}

bool CSnapshot::Take()
{
	if (m_Mixer == NULL)
		return(FALSE);
	int	Chans = m_Mixer->GetFullItemCount();	// empty channels not included
	int	Snaps = m_Snap.GetSize();
	m_Snap.SetSize(Snaps + 1);		// add space for new element
	int	NewID = ++m_SnapIDs;		// create new ID
	CSnap	*sp = &m_Snap[Snaps];
	sp->m_ID = NewID;
	sp->m_Title.Format(LDS(SNAP_DEFAULT_TITLE), NewID);
	sp->m_ChanSnap.SetSize(Chans);
	CChanInfo	ci;
	int	j = 0;
	// take a snapshot of each non-empty channel
	for (int i = 0; i < m_Mixer->GetItemCount(); i++) {
		if (!m_Mixer->IsEmpty(i)) {	// skip empty channels
			m_Mixer->GetChan(i)->GetInfo(&ci);
			sp->m_ChanSnap[j++] = GetChanSnap(ci);
		}
	}
	// prompt for snapshot title
	CTakeSnapDlg	dlg(*this);
	dlg.SetName(sp->m_Title);
	if (dlg.DoModal() != IDOK) {
		m_SnapIDs--;	// didn't use the new ID, so return it
		m_Snap.RemoveAt(Snaps);	// delete the canceled snapshot
		return(FALSE);
	}
	int	OwnerPos;
	sp->m_Key = dlg.GetHotKey(OwnerPos);
	if (OwnerPos >= 0)			// if shortcut key was reassigned
		m_Snap[OwnerPos].m_Key = 0;	// remove it from previous owner
	sp->m_Title = dlg.GetName();
	m_ActiveID = NewID;
	UpdateList();
	m_Mixer->NotifyUndoableEdit(UCODE_TAKE);
	return(TRUE);
}

void CSnapshot::Restore(CChannel& Chan, const CChanInfo& Info)
{
	// first restore the sliders
	for (int i = 0; i < CChannel::AUTOS; i++) {
		CAutoInfo	ai = Info.m_Auto[i];
		CAutoSliderCtrl	*Slider = Chan.GetAuto(i);
		if (Slider->IsPlaying() && ai.IsPlaying() && m_RestoreType == RT_SMOOTH) {
			// for smooth restore, if the slider is currently playing, and was
			// playing in the snapshot, don't update its position or direction,
			// and don't let it send a notification message to the channel
			ai.m_Pos = Slider->GetNormPos();
			ai.m_Reverse = Slider->GetReverse();
			Slider->SetInfo(ai);	// without notification
		} else {	// unconditional update
			if (i != CChannel::AUTO_POS) {
				// for all sliders except position, send a notification, so the
				// channel updates the corresponding audio attribute from m_Pos
				Slider->SetInfoNotify(ai);	// with notification
			} else {
				// position slider is a special case; it overloads m_Pos to be
				// the trigger loop position instead of the audio position, so
				// we want to restore its trigger position without moving the 
				// slider, and let the audio restore set the slider position
				double	TriggerPos = ai.m_Pos;		// save trigger position
				ai.m_Pos = Slider->GetNormPos();	// keep slider from moving
				((CAutoTriggerCtrl *)Slider)->SetInfo(ai);	// downcast
				((CAutoTriggerCtrl *)Slider)->SetTriggerPos(TriggerPos);
			}
		}
	}
	// then restore the audio state
	CAudio::CLoop	InfoSel(Info.m_Start, Info.m_End, Info.m_Count);
	if (Chan.IsPlaying() && Info.IsPlaying() && m_RestoreType == RT_SMOOTH) {
		// for smooth restore, if the audio is currently playing, and was playing
		// in the snapshot, try to avoid making the audio skip; don't update its
		// position, and only update its loop and selection attributes if their
		// current values differ from the snapshot's values
		if (Chan.GetLoop() != (Info.m_Loop != 0))	// if loop differs
			Chan.SetLoop(Info.m_Loop != 0);				// update it
		CAudio::CLoop	ChanSel;
		Chan.GetSelection(ChanSel);
		if (ChanSel != InfoSel)		// if selection differs
			Chan.SetSelection(InfoSel);	// update it
	} else {	// unconditional update
		Chan.SetLoop(Info.m_Loop != 0);
		Chan.SetSelection(InfoSel);
		Chan.SetTransportAndPos(Info.m_Transport, Info.m_Pos);
	}
	Chan.SetMute(Info.m_Mute != 0);
	Chan.SetSolo(Info.m_Solo != 0);
}

bool CSnapshot::Restore(int SnapIdx)
{
	if (m_Mixer == NULL)
		return(FALSE);
	m_Mixer->NotifyUndoableEdit(UCODE_RESTORE, CUndoable::UE_COALESCE);
	int	pos = Unsort(SnapIdx);	// remove sort indirection
	int	Chans = m_Snap[pos].GetChanCount();
	CByteArray	WasRestored;
	WasRestored.SetSize(m_Mixer->GetItemCount());
	CChanInfo	ci;
	// for each channel snapshot, try to find the corresponding channel
	int i;
	for (i = 0; i < Chans; i++) {
		GetChanSnap(ci) = m_Snap[pos].m_ChanSnap[i];
		int	ChanIdx = m_Mixer->FindChanByID(ci.m_ID);
		if (ChanIdx >= 0) {							// if we found the channel
			Restore(*m_Mixer->GetChan(ChanIdx), ci);	// restore it
			WasRestored[ChanIdx] = TRUE;				// mark it restored
		}
	}
	// mute any channels that weren't restored
	for (i = 0; i < m_Mixer->GetItemCount(); i++) {
		if (!m_Mixer->IsEmpty(i) && !WasRestored[i])
			m_Mixer->GetChan(i)->SetMute(TRUE);
	}
	m_ActiveID = m_Snap[pos].m_ID;
	m_ActiveIdx = SnapIdx;
	m_Mixer->OnRestoreSnapshot(SnapIdx);	// mixer expects sort indirection
	return(TRUE);
}

void CSnapshot::Delete(int SnapIdx)
{
	int	pos = Unsort(SnapIdx);	// remove sort indirection
	m_Snap.RemoveAt(pos);
	UpdateList();
}

bool CSnapshot::ShowChannels(int SnapIdx)
{
	if (m_Mixer == NULL)
		return(FALSE);
	int	pos = Unsort(SnapIdx);	// remove sort indirection
	m_Mixer->Deselect();
	int	Chans = m_Snap[pos].GetChanCount();
	CChanInfo	ci;
	for (int i = 0; i < Chans; i++) {
		GetChanSnap(ci) = m_Snap[pos].m_ChanSnap[i];
		int	ChanIdx = m_Mixer->FindChanByID(ci.m_ID);
		if (ChanIdx >= 0)
			m_Mixer->SetSelect(ChanIdx, TRUE);
	}
	return(TRUE);
}

void CSnapshot::EditList()
{
	CEditSnapsDlg	dlg(*this);
	m_EditDlg = &dlg;	// so undo handler can get current selection
	dlg.DoModal();
	m_EditDlg = NULL;
}

int CSnapshot::FindByID(DWORD SnapID) const
{
	for (int i = 0; i < GetCount(); i++) {
		if (GetID(i) == SnapID) 
			return(i);	// returned index has sort indirection
	}
	return(-1);	// ID not found
}

int CSnapshot::SortCompare(const void *elem1, const void *elem2)
{
	// only name sort is supported; ID sort is a no-op, see below
	CSnap	*p1 = &m_This->m_Snap[*((int *)elem1)];
	CSnap	*p2 = &m_This->m_Snap[*((int *)elem2)];
	return(stricmp(p1->m_Title, p2->m_Title));	// case-insensitive
}

void CSnapshot::UpdateList()
{
	// generate table of snapshot indices for sort indirection
	m_SortMap.SetSize(GetCount());
	int i;
	for (i = 0; i < GetCount(); i++)
		m_SortMap[i] = i;
	// sort the table; sorting by creation time is the same as sorting by ID,
	// and that's a no-op because snapshots are physically stored in ID order
	if (m_SortType != SORT_CREATED) {
		m_This = this;	// give SortCompare our this
		qsort(m_SortMap.GetData(), GetCount(), sizeof(DWORD), SortCompare);
	}
	// active snapshot may have moved in sorted coordinates; find it again
	m_ActiveIdx = max(FindByID(m_ActiveID), 0);
	// generate shortcut key list
	m_SnapKeys.RemoveAll();
	for (i = 0; i < GetCount(); i++) {
		int	Key = m_Snap[m_SortMap[i]].m_Key;	// apply sort indirection
		if (Key)
			m_SnapKeys.AddKey(Key, HOTKEY_CMD_BASE + i);
	}
	// generate accelerator table from shortcut key list
	m_SnapKeys.CreateAccelTable(m_Accel);
	// update mixer's UI
	if (m_Mixer != NULL)
		m_Mixer->OnUpdateSnapshotList();
}

void CSnapshot::Serialize(CArchive& ar, int Version, const CChanInfo *Chan, int Chans)
{
	int	Count;
	if (ar.IsStoring()) {
		Count = m_Snap.GetSize();
		ar << Count;
		for (int i = 0; i < Count; i++) {
			ar << m_Snap[i].m_ID;
			ar << m_Snap[i].m_Key;
			ar << m_Snap[i].m_Title;
			m_Snap[i].m_ChanSnap.Serialize(ar);
		}
		ar << m_SnapIDs;
		ar << m_ActiveID;
		ar << m_RestoreType;
		ar << m_SortType;
	} else {
		ar >> Count;
		m_Snap.SetSize(Count);
		for (int i = 0; i < m_Snap.GetSize(); i++) {
				ar >> m_Snap[i].m_ID;
				ar >> m_Snap[i].m_Key;
				ar >> m_Snap[i].m_Title;
				m_Snap[i].m_ChanSnap.Serialize(ar);
				// for each channel snap, verify that a channel with that ID
				// exists in the channel array; if not, delete channel snap
				CSnap	*sp = &m_Snap[i];
				int	j = 0;
				while (j < sp->m_ChanSnap.GetSize()) {
					DWORD	ChanID = sp->m_ChanSnap[j].m_ID;
					int k;
					for (k = 0; k < Chans; k++) {	// for each channel
						if (Chan[k].m_ID == ChanID)			// if IDs match
							break;								// snap is OK
					}
					if (k < Chans)	// if snap is OK
						j++;			// next snap
					else
						sp->m_ChanSnap.RemoveAt(j);	// dead wood
				}
		}
		ar >> m_SnapIDs;
		ar >> m_ActiveID;
		ar >> m_RestoreType;
		ar >> m_SortType;
		UpdateList();	// sets m_SortMap and m_ActiveIdx
	}
}

bool CSnapshot::ValidateHotKey(DWORD Key, int Pos, LPCSTR Title, int& OwnerPos) const
{
	OwnerPos = -1;	// assume key is unassigned
	int	KeyIdx;
	CString	msg, KeyName, CmdName;
	CHotKeyList::KeyToName(Key, KeyName);
	// if key is a main accelerator, display error message
	if ((KeyIdx = m_MainKeys.FindKey(Key)) >= 0) {
		m_MainKeys.GetCmdName(KeyIdx, CmdName);
		msg.FormatMessage(IDS_SSK_RSVD_CMD, KeyName, CmdName);
		AfxMessageBox(msg);
		return(FALSE);
	}
	// if key is an alphabetic menu key, display error message
	WORD	VKey = LOWORD(Key);
	WORD	ModFlags = HIWORD(Key);
	if ((ModFlags & HOTKEYF_ALT) && isalpha(VKey)) {
		msg.FormatMessage(IDS_SSK_RSVD_MENU, KeyName);
		AfxMessageBox(msg);
		return(FALSE);
	}
	// if key is a system key, display error message
	bool	SysKey;
	switch (VKey) {
	case VK_CAPITAL:
	case VK_NUMLOCK:
	case VK_SCROLL:
		SysKey = TRUE;
		break;
	default:
		SysKey = (VKey >= VK_SPACE && VKey < '0');
	}
	if (SysKey) {
		msg.FormatMessage(IDS_SSK_RSVD_SYSTEM, KeyName);
		AfxMessageBox(msg);
		return(FALSE);
	}
	// if key is assigned to another snapshot, display reassign prompt
	if ((KeyIdx = m_SnapKeys.FindKey(Key)) >= 0) {
		// get the index of the snapshot this key is assigned to
		int	OwnerSnapIdx = m_SnapKeys.GetCmd(KeyIdx) - HOTKEY_CMD_BASE;
		OwnerPos = Unsort(OwnerSnapIdx);	// remove sort indirection
		if (OwnerPos != Pos) {	// if key isn't assigned to us, prompt
			msg.FormatMessage(IDS_SSK_ASSIGNED, KeyName,
				m_Snap[OwnerPos].m_Title, Title);
			if (AfxMessageBox(msg, MB_YESNO | MB_DEFBUTTON2) != IDYES)
				return(FALSE);
		}
	}
	return(TRUE);
}

void CSnapshot::GetSnapInfo(int Pos, CUndoState& State)
{
	int	InfoSize = m_Snap[Pos].CalcInfoSize();
	BYTE	*ip = new BYTE[InfoSize];
	m_Snap[Pos].GetInfo(ip);
	State.SetData(ip, InfoSize);
	delete [] ip;
	UValPos(State) = Pos;
}

void CSnapshot::SaveUndoState(CUndoState& State)
{
	UValActiveID(State) = m_ActiveID;
	switch (State.GetCode()) {
	case UCODE_RESTORE:
		{
			// restore snapshot doesn't affect empty channels, so we can save
			// memory by skipping them; allocate space for full channels only
			int	Chans = m_Mixer->GetFullItemCount();
			CHAN_SNAP	*uap = new CHAN_SNAP[Chans];
			State.SetDataPtr(uap, Chans * sizeof(CHAN_SNAP));
			UValChans(State) = Chans;
			CChanInfo	ci;
			for (int i = 0; i < m_Mixer->GetItemCount(); i++) {
				if (!m_Mixer->IsEmpty(i)) {	// skip empty channels
					m_Mixer->GetChan(i)->GetInfo(&ci);
					ci.m_ID = i;	// overload ID to store channel index
					*uap++ = GetChanSnap(ci);
				}
			}
		}
		break;
	case UCODE_TAKE:
		{
			if (State.IsEmpty()) {	// if first time
				int	pos = GetCount() - 1;	// new snap is at end of list
				GetSnapInfo(pos, State);
				UValInsert(State) = 0;	// first restore will delete
			} else
				UValInsert(State) ^= 1;	// otherwise just flip direction
		}
		break;
	case UCODE_DELETE:
		{
			if (State.IsEmpty()) {	// if first time
				int SnapIdx = m_UndoParam;	// get current selection
				int	pos = Unsort(SnapIdx);	// remove sort indirection
				GetSnapInfo(pos, State);
				UValInsert(State) = 1;	// first restore will insert
			} else
				UValInsert(State) ^= 1;	// otherwise just flip direction
		}
		break;
	case UCODE_RENAME:
		{
			if (State.IsEmpty()) {	// if first time
				int SnapIdx = m_UndoParam;	// get current selection
				int	pos = Unsort(SnapIdx);	// remove sort indirection
				UValPos(State) = pos;
			}
			LPCSTR	Title = m_Snap[UValPos(State)].m_Title;
			State.SetData(Title, strlen(Title) + 1);
		}
		break;
	case UCODE_OPTIONS:
		UValOptions(State) = MAKEWORD(m_RestoreType, m_SortType);
		break;
	case UCODE_HOTKEY:
		{
			if (!UValHotKey(State)) {	// if first time
				int SnapIdx = m_UndoParam;	// get current selection
				int	pos = Unsort(SnapIdx);	// remove sort indirection
				UValPos(State) = pos;
			}
			// increment key by one, reserving zero for first time flag
			UValHotKey(State) = m_Snap[UValPos(State)].m_Key + 1;
		}
		break;
	default:
		NODEFAULTCASE;
	}
}

void CSnapshot::RestoreUndoState(const CUndoState& State)
{
	switch (State.GetCode()) {
	case UCODE_RESTORE:
		{
			CHAN_SNAP	*uap = (CHAN_SNAP *)State.GetData();
			CChanInfo	ci;
			int	Chans = UValChans(State);
			for (int i = 0; i < Chans; i++) {
				GetChanSnap(ci) = *uap++;
				// ID is overloaded; it contains channel's index within m_Mixer
				CChannel	*Chan = m_Mixer->GetChan(ci.m_ID);
				// snapshot doesn't include path and title; copy from channel
				strcpy(ci.m_Path, Chan->GetFilePath());
				strcpy(ci.m_Title, Chan->GetTitle());
				ci.m_ID = Chan->GetID();	// ID is now back to being ID again
				Chan->SetInfo(&ci);
			}
		}
		break;
	case UCODE_TAKE:
	case UCODE_DELETE:
		{
			if (UValInsert(State)) {
				CSnap	ts;	// temp object; can't avoid copy here
				ts.SetInfo(State.GetData());
				m_Snap.InsertAt(UValPos(State), ts);
			} else
				m_Snap.RemoveAt(UValPos(State));
		}
		break;
	case UCODE_RENAME:
		m_Snap[UValPos(State)].m_Title = (LPCSTR)State.GetData();
		break;
	case UCODE_OPTIONS:
		m_RestoreType = LOBYTE(UValOptions(State));
		m_SortType = HIBYTE(UValOptions(State));
		break;
	case UCODE_HOTKEY:
		m_Snap[UValPos(State)].m_Key = UValHotKey(State) - 1;
		break;
	default:
		NODEFAULTCASE;
	}
	m_ActiveID = UValActiveID(State);
	UpdateList();
}

void CSnapshot::GetUndoTitle(const CUndoState& State, CString& Title)
{
	int	Code = State.GetCode() - UNDO_CODE_OFFSET;
	ASSERT(Code >= 0 && Code < UNDO_CODES);
	Title.LoadString(m_UndoTitleID[Code]);
}

void CSnapshot::CSnap::Copy(const CSnap& Snap)
{
	m_ID = Snap.m_ID;
	m_Key = Snap.m_Key;
	m_Title = Snap.m_Title;
	m_ChanSnap.Copy(Snap.m_ChanSnap);
}

void CSnapshot::CSnap::GetInfo(LPVOID Info) const
{
	SNAP_INFO	*ip = (SNAP_INFO *)Info;
	ip->m_ID = m_ID;
	ip->m_Key = m_Key;
	ip->m_Size = CalcInfoSize();
	strncpy(ip->m_Title, m_Title, MAX_TITLE);
	ip->m_Title[MAX_TITLE] = 0;	// in case of truncation
	ip->m_Chans = GetChanCount();
	memcpy(ip->m_ChanSnap, m_ChanSnap.GetData(), 
		GetChanCount() * sizeof(CHAN_SNAP));
}

void CSnapshot::CSnap::SetInfo(LPCVOID Info)
{
	SNAP_INFO	*ip = (SNAP_INFO *)Info;
	m_ID = ip->m_ID;
	m_Key = ip->m_Key;
	m_Title = ip->m_Title;
	m_ChanSnap.SetSize(ip->m_Chans);
	memcpy(m_ChanSnap.GetData(), ip->m_ChanSnap,
		GetChanCount() * sizeof(CHAN_SNAP));
}
