// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      21nov03 initial version

		base class for derived form list dialog's info
 
*/

#ifndef CFORMLISTINFO_INCLUDED
#define CFORMLISTINFO_INCLUDED

class CFormListInfo {
public:
	CFormListInfo();
	int		m_Index;	// item's zero-based offset within view
};

#endif
