// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      04feb05 initial version

		wrapper for accelerator table
 
*/

#ifndef ACCELTABLE_INCLUDED
#define ACCELTABLE_INCLUDED

class CAccelTable {
public:
// Construction
	CAccelTable();
	~CAccelTable();

// Initialization
	bool	LoadFromRes(int ResID);
	void	LoadFromHandle(HACCEL Accel);
	void	LoadFromTable(const ACCEL *Table, int Entries);

// Attributes
	operator bool() const;
	operator HACCEL() const;
	int		GetEntries() const;
	int		GetTable(ACCEL *Table, int Entries) const;
	static	bool	IsExtended(DWORD Key);
	static	void	GetKeyName(const ACCEL& Accel, CString& KeyName);
	static	void	GetCmdName(const ACCEL& Accel, CString& CmdName);
	static	void	GetCmdHelp(const ACCEL& Accel, CString& CmdHelp);

protected:
// Member data
	HACCEL	m_Accel;	// if non-NULL, handle to an accelerator table
	bool	m_Destroy;	// true if we're responsible for destroying table

// Helpers
	void	Cleanup();
	static	CString	GetKeyNameText(LONG lParam);

	NOCOPIES(CAccelTable);
};

inline CAccelTable::operator bool() const
{
	return(m_Accel != NULL);
}

inline CAccelTable::operator HACCEL() const
{
	return(m_Accel);
}

#endif
