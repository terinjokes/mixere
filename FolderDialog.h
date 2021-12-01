// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      28jan02 initial version
 
        Browses for a folder.
 
*/

#ifndef CFOLDERDIALOG_INCLUDED
#define CFOLDERDIALOG_INCLUDED

class CFolderDialog {
public:
	bool	BrowseFolder(
		LPCSTR	Title,			// Title to display in dialog box.
		CString&	Folder,		// Receives the selected folder's path.
		LPCSTR	Root = NULL,	// Optional root path to browse from.
		UINT	Flags = 0		// See SHBrowseForFolder for more info.
	);
};

#endif
