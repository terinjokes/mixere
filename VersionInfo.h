// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.

#ifndef CVERSIONINFO_INCLUDED
#define CVERSIONINFO_INCLUDED

class CVersionInfo {
public:
	static std::tuple<VS_FIXEDFILEINFO, bool>	GetFileInfo(const std::string& path = "");
	static std::tuple<VS_FIXEDFILEINFO, bool>	GetModuleInfo(const std::string& module_name);
};

#endif
