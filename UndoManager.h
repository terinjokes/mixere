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

#ifndef CUNDOMANAGER_INCLUDED
#define CUNDOMANAGER_INCLUDED

#include <afxtempl.h>
#include "Undoable.h"

class CUndoManager {
public:
// Construction
	CUndoManager(CUndoable *Root);
	virtual	~CUndoManager();

// Attributes
	bool	IsUndoable() const;
	bool	IsRedoable() const;
	static	bool	IsUndoInProgress();
	bool	IsModified() const;
	void	ResetModifiedFlag();
	LPCSTR	GetUndoTitle() const;
	LPCSTR	GetRedoTitle() const;
	int		GetLevels() const;
	void	SetLevels(int Levels);

// Operations
	void	Undo();
	void	Redo();
	void	NotifyEdit(CUndoable *Adr, WORD CtrlID, WORD Code, UINT Flags = 0);
	void	CancelEdit(CUndoable *Adr, WORD CtrlID, WORD Code);
	void	DiscardAllEdits();

protected:
// Overridables
	virtual	void	OnModify(bool Modified);

private:
// Member data
	CUndoable	*m_Root;
	CArray<CUndoState, const CUndoState&>	m_List;
	int		m_Pos;
	int		m_Levels;
	int		m_Edits;
	static	bool	m_Busy;
	CString	m_UndoTitle;
	CString	m_RedoTitle;

// Helpers
	void	SwapState(int Pos);
	CUndoable	*FindUndoable(const CUndoState::UNDO_PATH& Path) const;
	void	DumpState(LPCSTR Tag, LPCSTR Title, int Pos) const;
	void	GetTitle(int Pos, CString& Title) const;
	void	UpdateTitles();
	NOCOPIES(CUndoManager);
};

inline bool CUndoManager::IsUndoInProgress()
{
	return(m_Busy);
}

inline bool CUndoManager::IsModified() const
{
	return(m_Edits > 0);
}

inline void CUndoManager::ResetModifiedFlag()
{
	m_Edits = 0;
}

inline int CUndoManager::GetLevels() const
{
	return(m_Levels);
}

inline LPCSTR CUndoManager::GetUndoTitle() const
{
	return(m_UndoTitle);
}

inline LPCSTR CUndoManager::GetRedoTitle() const
{
	return(m_RedoTitle);
}

#endif
