// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      09feb04	initial version

		make states persistent using registry
 
*/

#ifndef CPERSIST_INCLUDED
#define	CPERSIST_INCLUDED

class CPersist {
public:
	enum OPTION {
		NO_MINIMIZE = 0x01,
		NO_MAXIMIZE = 0x02
	};
	static	UINT	GetInt(LPCTSTR Section, LPCTSTR Entry, int Default);
	static	BOOL	WriteInt(LPCTSTR Section, LPCTSTR Entry, int Value);
	static	CString	GetString(LPCTSTR Section, LPCTSTR Entry, LPCTSTR Default = NULL);
	static	BOOL	WriteString(LPCTSTR Section, LPCTSTR Entry, LPCTSTR Value);
	static	void	SaveWnd(LPCTSTR Section, const CWnd *Wnd, LPCSTR Name);
	static	int		LoadWnd(LPCTSTR Section, CWnd *Wnd, LPCSTR Name, int Options = 0);
	static	int		GetWndShow(LPCTSTR Section, LPCSTR Name);
	static	BOOL	GetBinary(LPCTSTR Section, LPCTSTR Entry, LPVOID Buffer, LPDWORD Size);
	static	BOOL	WriteBinary(LPCTSTR Section, LPCTSTR Entry, LPCVOID Buffer, DWORD Size);
	static	BOOL	GetFont(LPCTSTR Section, LPCTSTR Entry, CFont *Font);
	static	BOOL	WriteFont(LPCTSTR Section, LPCTSTR Entry, CFont *Font);
	static	float	GetFloat(LPCTSTR Section, LPCTSTR Entry, float Default);
	static	BOOL	WriteFloat(LPCTSTR Section, LPCTSTR Entry, float Value);

private:
	static	LPCSTR WND_PLACE;
};

inline UINT	CPersist::GetInt(LPCTSTR Section, LPCTSTR Entry, int Default)
{
	return(AfxGetApp()->GetProfileInt(Section, Entry, Default));
}

inline BOOL	CPersist::WriteInt(LPCTSTR Section, LPCTSTR Entry, int Value)
{
	return(AfxGetApp()->WriteProfileInt(Section, Entry, Value));
}

inline CString CPersist::GetString(LPCTSTR Section, LPCTSTR Entry, LPCTSTR Default)
{
	return(AfxGetApp()->GetProfileString(Section, Entry, Default));
}

inline BOOL	CPersist::WriteString(LPCTSTR Section, LPCTSTR Entry, LPCTSTR Value)
{
	return(AfxGetApp()->WriteProfileString(Section, Entry, Value));
}

#endif