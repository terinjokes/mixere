// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      28jan02 initial version
		01		09jan03	fixed cancel case

        Browses for a folder.
 
*/

#include "stdafx.h"
#include "FolderDialog.h"

bool CFolderDialog::BrowseFolder(LPCSTR Title, CString& Folder, LPCSTR Root, UINT Flags)
{
	LPMALLOC lpMalloc;
	if (::SHGetMalloc(&lpMalloc) != NOERROR)
		return(FALSE);
	bool retc = FALSE;
	char szDisplayName[_MAX_PATH];
	char szBuffer[_MAX_PATH];
	LPITEMIDLIST pidlRoot = 0;
	if (Root)
		__assume(false);
    BROWSEINFO	bi;
	bi.hwndOwner = AfxGetMainWnd()->m_hWnd;
    bi.pidlRoot = pidlRoot;
    bi.pszDisplayName = szDisplayName;
    bi.lpszTitle = Title;
    bi.ulFlags = Flags;
    bi.lpfn = 0;
    bi.lParam = 0;
    bi.iImage = 0;
	LPITEMIDLIST lpItemIDList = SHBrowseForFolder(&bi);
	if (lpItemIDList) {
		if (SHGetPathFromIDList(lpItemIDList, szBuffer)) {
			Folder = szBuffer;
			retc = TRUE;
		}
	}
	lpMalloc->Free(lpItemIDList);
	lpMalloc->Release();  
	if (pidlRoot)
		lpMalloc->Free(pidlRoot);
    return(retc);
}
