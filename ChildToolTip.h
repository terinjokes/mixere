// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      27jan04	initial version

		implement tooltips for children of a window
 
*/

#ifndef CCHILDTOOLTIP_INCLUDED
#define CCHILDTOOLTIP_INCLUDED

class CChildToolTip
{
public:
	CChildToolTip();
	~CChildToolTip();
	BOOL	EnableToolTips(CWnd *pWnd, BOOL bEnable, BOOL UseDlgIDs = TRUE);
	void	RelayEvent(LPMSG lpMsg);
	CToolTipCtrl *operator->() const;
	operator bool() const;

private:
	CToolTipCtrl	*m_ToolTip;

	NOCOPIES(CChildToolTip);
};

inline CToolTipCtrl *CChildToolTip::operator->() const
{
	return(m_ToolTip);
}

inline CChildToolTip::operator bool() const
{
	return(m_ToolTip != NULL);
}

#endif
