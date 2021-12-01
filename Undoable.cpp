// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      22feb04	initial version

        undoable edit interface
 
*/

#include "stdafx.h"
#include "Resource.h"
#include "Undoable.h"
#include "UndoManager.h"

CUndoable::CUndoable()
{
	m_UndoManager = NULL;
	m_UndoHandler = this;
}

CUndoable::~CUndoable()
{
}

void CUndoable::NotifyUndoableEdit(WORD Code, UINT Flags)
{
	CUndoManager	*Mgr = FindUndoManager();
	if (Mgr != NULL)
		Mgr->NotifyEdit(m_UndoHandler, GetUndoKey(), Code, Flags);
}

void CUndoable::CancelUndoableEdit(WORD Code)
{
	CUndoManager	*Mgr = FindUndoManager();
	if (Mgr != NULL)
		Mgr->CancelEdit(m_UndoHandler, GetUndoKey(), Code);
}

void CUndoable::ClearUndoHistory()
{
	CUndoManager	*Mgr = FindUndoManager();
	if (Mgr != NULL)
		Mgr->DiscardAllEdits();
}

CUndoState::UNDO_KEY CUndoable::GetUndoKey()
{
	return(UndoToWnd(this)->GetDlgCtrlID());
}

CUndoable *CUndoable::FindUndoable(CUndoState::UNDO_KEY Key)
{
	return(WndToUndo(UndoToWnd(this)->GetDlgItem(Key)));
}

void CUndoable::GetUndoTitle(const CUndoState& State, CString& Title)
{
	UndoToWnd(this)->GetWindowText(Title);
}

bool CUndoable::IsUndoInProgress() const
{
	return(CUndoManager::IsUndoInProgress());
}

CUndoManager *CUndoable::FindUndoManager(CUndoState::UNDO_PATH *Path)
{
	CWnd	*wp = UndoToWnd(this);
	CUndoManager	*Mgr = NULL;
	CUndoState::UNDO_PATH	TmpPath;
	int		KeyIdx = 0;
	// undo manager is at root of undo tree; as we search for manager, 
	// build access path that manager can use to locate our instance
	while (Mgr == NULL) {	// loop until we find root
		CUndoable	*uap = WndToUndo(wp);
		if (uap == NULL)	// end of window chain, manager not found
			break;
		Mgr = uap->GetUndoManager();
		if (Mgr == NULL) {	// don't add key for root, no need
			if (KeyIdx < CUndoState::MAX_UNDO_KEYS)
				TmpPath.Key[KeyIdx++] = uap->GetUndoKey();
			else {
				AfxMessageBox(LDS(UNDO_TREE_TOO_DEEP));
				break;
			}
		}
		// undo parent is window owner, not window parent, so that undo 
		// hierarchy is independent of child window clipping hierarchy
		wp = wp->GetOwner();
	}
	if (Path != NULL) {
		// we traversed tree from leaf to root, generating path backwards;
		// reverse path, so most significant key is furthest from root
		int i;
		for (i = 0; i < KeyIdx; i++)
			Path->Key[i] = TmpPath.Key[KeyIdx - i - 1];
		for (; i < CUndoState::MAX_UNDO_KEYS; i++)	// zero unused keys
			Path->Key[i] = 0;
	}
	return(Mgr);
}
