// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      22feb04	initial version
		01		07mar04	add limit
		02		08mar04	in SwapState, set busy before finding undoable
		03		08mar04	in NotifyEdit, ctor isn't called when array shrinks
		04		12mar04	if coalescing, remove states above current position
		05		29sep04	cancel edit must update titles

        undoable edit interface
 
*/

#include "stdafx.h"
#include "Resource.h"
#include "UndoManager.h"

#if UNDO_NATTER
#define	UNDO_DUMP_STATE(Tag, Title, Pos) DumpState(Tag, Title, Pos);
#else
#define	UNDO_DUMP_STATE(Tag, Title, Pos)
#endif

bool CUndoManager::m_Busy;

CUndoManager::CUndoManager(CUndoable *Root) :
	m_Root(Root)
{
	m_Pos = 0;
	m_Levels = INT_MAX;
	m_Edits = 0;
}

CUndoManager::~CUndoManager()
{
}

void CUndoManager::Undo()
{
	if (IsUndoable()) {
		UNDO_DUMP_STATE("Undo", m_UndoTitle, m_Pos - 1);
		SwapState(--m_Pos);
	}
}

void CUndoManager::Redo()
{
	if (IsRedoable()) {
		UNDO_DUMP_STATE("Redo", m_RedoTitle, m_Pos);
		SwapState(m_Pos++);
	}
}

void CUndoManager::SwapState(int Pos)
{
	ASSERT(Pos >= 0 && Pos < m_List.GetSize());
	CUndoState	PrevState = m_List[Pos];
	m_Busy = TRUE;
	CUndoable	*uap = FindUndoable(m_List[Pos].m_Path);
	if (uap != NULL) {
		uap->SaveUndoState(m_List[Pos]);
		uap->RestoreUndoState(PrevState);
	} 
	m_Busy = FALSE;
	UpdateTitles();
#if UNDO_NATTER
	if (uap == NULL)
		AfxMessageBox(LDS(UNDO_CANT_FIND_INSTANCE));
#endif
}

void CUndoManager::NotifyEdit(CUndoable *Adr, WORD CtrlID, WORD Code, UINT Flags)
{
	if (Adr == NULL)
		return;
	if (!m_Busy) {
		CUndoState::UNDO_PATH	Path;
		Adr->FindUndoManager(&Path);	// build access path to control
		// if coalesce requested and notifier's key matches top of stack
		if ((Flags & CUndoable::UE_COALESCE) && m_Pos 
		&& m_List[m_Pos - 1].IsMatch(Path, CtrlID, Code)) {
			if (m_List.GetSize() > m_Pos)	// don't resize array needlessly
				m_List.SetSize(m_Pos);	// remove states above current position
			return;
		}
		if (m_Levels <= 0)
			return;
		if (m_Pos >= m_Levels) {
			m_List.RemoveAt(0);
			m_Pos--;
		}
		if (!(Flags & CUndoable::UE_INSIGNIFICANT)) {
			if (!m_Edits++)			// if first modification
				OnModify(TRUE);		// call derived handler
		}
		m_List.SetSize(m_Pos + 1);	// array shrinks if we've undone
		CUndoState	*usp = &m_List[m_Pos];
		usp->Empty();	// ctor isn't called when array shrinks
		usp->m_CtrlID = CtrlID;
		usp->m_Code = Code;
		usp->m_Path = Path;
		Adr->SaveUndoState(*usp);
		Adr->GetUndoTitle(*usp, m_UndoTitle);
		m_RedoTitle.Empty();
		UNDO_DUMP_STATE("Notify", m_UndoTitle, m_Pos);
		m_Pos++;
	} else {
#if UNDO_NATTER
		AfxMessageBox(LDS(UNDO_IGNORING_NOTIFY));
#endif
	}
}

void CUndoManager::CancelEdit(CUndoable *Adr, WORD CtrlID, WORD Code)
{
	if (Adr == NULL)
		return;
	CUndoState::UNDO_PATH	Path;
	Adr->FindUndoManager(&Path);
#if UNDO_NATTER
	printf("CancelEdit CtrlID=%d Code=%d Path=%s\n", CtrlID, Code, 
		CUndoState::PathToString(Path));
#endif
	for (int i = m_Pos - 1; i >= 0; i--) {
		if (m_List[i].IsMatch(Path, CtrlID, Code))
			break;
	}
	if (i >= 0) {
		m_List.RemoveAt(i);
		m_Pos--;
		if (!--m_Edits)			// if last modification
			OnModify(FALSE);	// call derived handler
		UpdateTitles();
	}
#if UNDO_NATTER
	if (i < 0)
		AfxMessageBox(LDS(UNDO_CANT_CANCEL_EDIT));
#endif
}

void CUndoManager::DiscardAllEdits()
{
#if UNDO_NATTER
	printf("DiscardAllEdits\n");
#endif
	m_List.SetSize(0);
	m_Pos = 0;
	m_Edits = 0;
	OnModify(FALSE);	// call derived handler
	UpdateTitles();
}

bool CUndoManager::IsUndoable() const
{
	return(m_Pos > 0);
}

bool CUndoManager::IsRedoable() const
{
	return(m_Pos < m_List.GetSize());
}

CUndoable *CUndoManager::FindUndoable(const CUndoState::UNDO_PATH& Path) const
{
	CUndoable	*uap = m_Root;
	for (int i = 0; i < CUndoState::MAX_UNDO_KEYS; i++) {
		if (!Path.Key[i])	// zero reserved for unused keys
			break;
		uap = uap->FindUndoable(Path.Key[i]);
		if (uap == NULL)
			break;
	}
	return(uap);
}

void CUndoManager::DumpState(LPCSTR Tag, LPCSTR Title, int Pos) const
{
	printf("%s '%s' Pos=%d %s Len=%d Data=0x%x\n", Tag, Title, Pos, 
		m_List[Pos].DumpState(), m_List[Pos].GetDataLen(), m_List[Pos].GetData());
}

void CUndoManager::UpdateTitles()
{
	if (IsUndoable())
		GetTitle(m_Pos - 1, m_UndoTitle);
	else
		m_UndoTitle.Empty();
	if (IsRedoable())
		GetTitle(m_Pos, m_RedoTitle);
	else
		m_RedoTitle.Empty();
}

void CUndoManager::GetTitle(int Pos, CString& Title) const
{
	CUndoable	*uap = FindUndoable(m_List[Pos].m_Path);
	if (uap != NULL)
		uap->GetUndoTitle(m_List[Pos], Title);
	else
		Title.Empty();
}

void CUndoManager::SetLevels(int Levels)
{
	if (Levels < 0)
		Levels = INT_MAX;
	if (Levels < m_List.GetSize()) {
		int	Excess = m_List.GetSize() - Levels;
		m_List.RemoveAt(0, Excess);
		m_Pos = max(m_Pos - Excess, 0);
	}
	m_Levels = Levels;
}

void CUndoManager::OnModify(bool Modified)
{
}
