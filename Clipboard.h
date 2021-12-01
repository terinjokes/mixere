// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      21nov03 initial version

		generic read/write interface for windows clipboard
 
*/

#ifndef CCLIPBOARD_INCLUDED
#define CCLIPBOARD_INCLUDED

class CClipboard {
public:
	CClipboard(HWND hWnd, LPCSTR Format);
	~CClipboard();
	void	SetFormat(LPCSTR Format);
	void	SetOwner(LPGUID Owner);
	bool	Write(const LPVOID Data, DWORD Length) const;
	LPVOID	Read(DWORD& Length, LPGUID Owner = NULL) const;
	bool	HasData() const;

private:
	static	const DWORD	HDRSIZE;
	HWND	m_hWnd;
	int		m_Format;
	GUID	m_Owner;
	NOCOPIES(CClipboard);
};

#endif 
