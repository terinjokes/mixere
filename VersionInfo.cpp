// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.

#include "stdafx.h"
#include "VersionInfo.h"

#include <tuple>

std::tuple<VS_FIXEDFILEINFO, bool> CVersionInfo::GetFileInfo(const std::string& path)
{
	VS_FIXEDFILEINFO info;
	std::string mod_path = path;
	if (path.empty())
	{
		char module[MAX_PATH];
		GetModuleFileName(AfxGetInstanceHandle(), module, MAX_PATH);
		mod_path = module;
	}

	DWORD	handle;
	const DWORD	size = GetFileVersionInfoSize(mod_path.c_str(), &handle);

	if (!size)
	{
		return std::make_tuple(info, false);
	}

	std::vector<char> buffer(size);
	if (GetFileVersionInfo(mod_path.c_str(), handle, size, buffer.data()))
	{
		UINT info_size = 0;
		PVOID ffi;
		if (VerQueryValue(buffer.data(), "\\", &ffi, &info_size))
		{
			info = *static_cast<VS_FIXEDFILEINFO*>(ffi);
			return std::make_tuple(info, true);
		}
	}

	return std::make_tuple(info, false);
}

std::tuple<VS_FIXEDFILEINFO, bool> CVersionInfo::GetModuleInfo(const std::string& module_name)
{
	VS_FIXEDFILEINFO info;
	char	mod[MAX_PATH];
	const HMODULE	handle = GetModuleHandle(module_name.c_str());

	if (handle == nullptr)
	{
		return std::make_tuple(info, false);
	}

	if (!GetModuleFileName(handle, mod, MAX_PATH))
	{
		return std::make_tuple(info, false);
	}

	return GetFileInfo(mod);
}
