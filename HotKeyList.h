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

#ifndef HOTKEYLIST_INCLUDED
#define HOTKEYLIST_INCLUDED

#include <afxtempl.h>

class CAccelTable;

class CHotKeyList {
public:
// Construction
	CHotKeyList();

// Initialization
	bool	LoadAccelRes(int ResID);
	void	LoadAccelHandle(HACCEL Accel);
	void	LoadAccelTable(const CAccelTable& Table);

// Attributes
	int		GetCount() const;
	const	ACCEL&	GetAccel(int Idx) const;
	DWORD	GetKey(int Idx) const;
	WORD	GetCmd(int Idx) const;
	void	SetKey(int Idx, DWORD Key, WORD Cmd);
	void	GetKeyName(int Idx, CString& KeyName) const;
	void	GetCmdName(int Idx, CString& CmdName) const;
	void	GetCmdHelp(int Idx, CString& CmdHelp) const;

// Operations
	void	CreateAccelTable(CAccelTable& Table) const;
	void	RemoveAll();
	int		AddKey(DWORD Key, WORD Cmd);
	void	InsertKey(int Idx, DWORD Key, WORD Cmd);
	void	DeleteKey(int Idx);
	int		FindKey(DWORD Key) const;

// Conversions
	static	DWORD	AccelToKey(const ACCEL& Accel, WORD& Cmd);
	static	void	KeyToAccel(DWORD Key, WORD Cmd, ACCEL& Accel);
	static	void	KeyToName(DWORD Key, CString& KeyName);
	static	void	CmdToName(WORD Cmd, CString& CmdName);

protected:
// Member data
	CArray<ACCEL, ACCEL&>	m_Accel;

// Helpers
	CString	GetKeyNameText(LONG lParam) const;

	NOCOPIES(CHotKeyList);
};

inline int CHotKeyList::GetCount() const
{
	return(m_Accel.GetSize());
}

inline const ACCEL& CHotKeyList::GetAccel(int Idx) const
{
	return(m_Accel.GetData()[Idx]);
}

inline DWORD CHotKeyList::GetKey(int Idx) const
{
	WORD	Cmd;
	return(AccelToKey(m_Accel[Idx], Cmd));
}

inline WORD CHotKeyList::GetCmd(int Idx) const
{
	return(m_Accel[Idx].cmd);
}

inline void CHotKeyList::SetKey(int Idx, DWORD Key, WORD Cmd)
{
	KeyToAccel(Key, Cmd, m_Accel[Idx]);
}

#endif
