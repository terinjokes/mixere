// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      04feb05 initial version

		manage a list of hot keys
 
*/

#include "stdafx.h"
#include "HotKeyList.h"
#include "AccelTable.h"

CHotKeyList::CHotKeyList()
{
}

bool CHotKeyList::LoadAccelRes(int ResID)
{
	HACCEL	Accel;
	Accel = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(ResID));
	if (Accel == NULL)
		return(FALSE);
	LoadAccelHandle(Accel);
	// accelerator tables loaded from resources are freed automatically
	return(TRUE);
}

void CHotKeyList::LoadAccelHandle(HACCEL Accel)
{
	int	Entries = CopyAcceleratorTable(Accel, NULL, 0);
	if (Entries) {
		m_Accel.SetSize(Entries);
		CopyAcceleratorTable(Accel, m_Accel.GetData(), Entries);
	}
}

void CHotKeyList::LoadAccelTable(const CAccelTable& Table)
{
	m_Accel.SetSize(Table.GetEntries());
	Table.GetTable(m_Accel.GetData(), m_Accel.GetSize());
}

void CHotKeyList::CreateAccelTable(CAccelTable& Table) const
{
	Table.LoadFromTable(m_Accel.GetData(), m_Accel.GetSize());
}

DWORD CHotKeyList::AccelToKey(const ACCEL& Accel, WORD& Cmd)
{
	WORD	flags = 0;
	if (Accel.fVirt & FALT)
		flags |= HOTKEYF_ALT;
	if (Accel.fVirt & FCONTROL)
		flags |= HOTKEYF_CONTROL;  
	if (Accel.fVirt & FSHIFT)
		flags |= HOTKEYF_SHIFT;
	if (CAccelTable::IsExtended(Accel.key))
		flags |= HOTKEYF_EXT;
	Cmd = Accel.cmd;
	return(MAKELONG(Accel.key, flags));
}

void CHotKeyList::KeyToAccel(DWORD Key, WORD Cmd, ACCEL& Accel)
{
	WORD	flags = HIWORD(Key);
	Accel.fVirt = FVIRTKEY;	// assume hot keys are virtual
	if (flags & HOTKEYF_ALT)
		Accel.fVirt |= FALT;
	if (flags & HOTKEYF_CONTROL)
		Accel.fVirt |= FCONTROL;  
	if (flags & HOTKEYF_SHIFT)
		Accel.fVirt |= FSHIFT;
	Accel.key = LOWORD(Key);
	Accel.cmd = Cmd;
}

void CHotKeyList::GetKeyName(int Idx, CString& KeyName) const
{
	CAccelTable::GetKeyName(m_Accel[Idx], KeyName);
}

void CHotKeyList::GetCmdName(int Idx, CString& CmdName) const
{
	CAccelTable::GetCmdName(m_Accel[Idx], CmdName);
}

void CHotKeyList::GetCmdHelp(int Idx, CString& CmdHelp) const
{
	CAccelTable::GetCmdHelp(m_Accel[Idx], CmdHelp);
}

void CHotKeyList::KeyToName(DWORD Key, CString& KeyName)
{
	ACCEL	Accel;
	KeyToAccel(Key, 0, Accel);
	CAccelTable::GetKeyName(Accel, KeyName);
}

void CHotKeyList::CmdToName(WORD Cmd, CString& CmdName)
{
	ACCEL	Accel;
	Accel.cmd = Cmd;
	CAccelTable::GetCmdName(Accel, CmdName);
}

void CHotKeyList::RemoveAll()
{
	m_Accel.RemoveAll();
}

int CHotKeyList::AddKey(DWORD Key, WORD Cmd)
{
	ACCEL	Accel;
	KeyToAccel(Key, Cmd, Accel);
	return(m_Accel.Add(Accel));
}

void CHotKeyList::InsertKey(int Idx, DWORD Key, WORD Cmd)
{
	ACCEL	Accel;
	KeyToAccel(Key, Cmd, Accel);
	m_Accel.InsertAt(Idx, Accel);
}

void CHotKeyList::DeleteKey(int Idx)
{
	m_Accel.RemoveAt(Idx);
}

int CHotKeyList::FindKey(DWORD Key) const
{
	WORD	Cmd;
	DWORD	ListKey;
	for (int i = 0; i < m_Accel.GetSize(); i++) {
		ListKey = AccelToKey(m_Accel[i], Cmd);
		if (ListKey == Key)
			return(i);
	}
	return(-1);
}
