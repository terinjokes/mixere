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

#include "stdafx.h"
#include "ChildTooltip.h"

CChildToolTip::CChildToolTip()
{
	m_ToolTip = NULL;
}

CChildToolTip::~CChildToolTip()
{
	delete m_ToolTip;
}

BOOL CChildToolTip::EnableToolTips(CWnd *pWnd, BOOL bEnable, BOOL UseDlgIDs)
{
	if (bEnable) {
		if (m_ToolTip != NULL)
			return(TRUE);		// already enabled, nothing to do
		m_ToolTip = new CToolTipCtrl;
		if (!m_ToolTip->Create(pWnd))
			return(FALSE);
		// iterate over our child controls, adding them to tooltip
		CWnd	*p = pWnd->GetWindow(GW_CHILD);
		while (p != NULL) {
			if (UseDlgIDs) {
				int	nID = p->GetDlgCtrlID();
				if (nID != -1) {
       				if (!m_ToolTip->AddTool(p, nID))
						return(FALSE);
				}
			} else {
       			if (!m_ToolTip->AddTool(p))
					return(FALSE);
			}	
			p = p->GetWindow(GW_HWNDNEXT);
		}
	} else {
		if (m_ToolTip == NULL)
			return(TRUE);		// already disabled, nothing to do
		delete m_ToolTip;
		m_ToolTip = NULL;
	}
	return(TRUE);
}
