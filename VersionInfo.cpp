// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      19jan04	initial version

        retrieve version information
 
*/

#include "stdafx.h"
#include "VersionInfo.h"

bool CVersionInfo::GetFileInfo(VS_FIXEDFILEINFO& Info, LPCSTR Path)
{
	ZeroMemory(&Info, sizeof(VS_FIXEDFILEINFO));
	char	module[MAX_PATH];
	if (Path != NULL)
		strncpy(module, Path, MAX_PATH);
	else
		GetModuleFileName(AfxGetInstanceHandle(), module, MAX_PATH);
	DWORD	handle;
	DWORD	size = GetFileVersionInfoSize(module, &handle);
	if (size) {
		CString	buffer;
		char	*pData = buffer.GetBuffer(size);
		if (GetFileVersionInfo(module, handle, size, pData)) {
			UINT	uInfoSize = 0;
			PVOID	pInfo;
			if (VerQueryValue(pData, "\\", &pInfo, &uInfoSize)) {
				Info = *((VS_FIXEDFILEINFO *)pInfo);
				return(TRUE);
			}
		}
	}
	return(FALSE);
}

bool CVersionInfo::GetModuleInfo(VS_FIXEDFILEINFO& Info, LPCSTR ModuleName)
{
	char	Path[MAX_PATH];
	HMODULE	Hand = GetModuleHandle(ModuleName);
	if (Hand != NULL && GetModuleFileName(Hand, Path, MAX_PATH))
		return(GetFileInfo(Info, Path));
	return(FALSE);
}
