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

#ifndef CUNDOABLE_INCLUDED
#define CUNDOABLE_INCLUDED

#include "UndoState.h"

class CUndoManager;

class CUndoable {
public:
// Constants
	enum UE_FLAGS {
		UE_COALESCE			= 0x01,
		UE_INSIGNIFICANT	= 0x02
	};

// Construction
	CUndoable();
	virtual ~CUndoable();

// Attributes
	CUndoManager	*GetUndoManager() const;
	void	SetUndoManager(CUndoManager *Mgr);
	CUndoable	*GetUndoHandler() const;
	void	SetUndoHandler(CUndoable *Handler);
	bool	IsUndoInProgress() const;

// Operations
	void	NotifyUndoableEdit(WORD Code, UINT Flags = 0);
	void	CancelUndoableEdit(WORD Code);
	void	ClearUndoHistory();
	CUndoManager	*FindUndoManager(CUndoState::UNDO_PATH *Path = NULL);

// Overridables
	virtual	void	SaveUndoState(CUndoState& State) = 0;
	virtual	void	RestoreUndoState(const CUndoState& State) = 0;
	virtual	CUndoState::UNDO_KEY	GetUndoKey();
	virtual	CUndoable	*FindUndoable(CUndoState::UNDO_KEY Key);
	virtual	void	GetUndoTitle(const CUndoState& State, CString& Title);

// Conversions
	static	CUndoable	*WndToUndo(CWnd *Wnd);
	static	CWnd	*UndoToWnd(CUndoable *Undoable);

private:
// Member data
	CUndoable	*m_UndoHandler;
	CUndoManager	*m_UndoManager;

	NOCOPIES(CUndoable);
};

inline CUndoManager *CUndoable::GetUndoManager() const
{
	return(m_UndoManager);
}

inline void CUndoable::SetUndoManager(CUndoManager *Mgr)
{
	m_UndoManager = Mgr;
}

inline CUndoable* CUndoable::GetUndoHandler() const
{
	return(m_UndoHandler);
}

inline void CUndoable::SetUndoHandler(CUndoable *Handler)
{
	m_UndoHandler = Handler;
}

inline CUndoable *CUndoable::WndToUndo(CWnd *Wnd)
{
	// NOTE: this only works if RTTI is enabled
	return(dynamic_cast<CUndoable *>(Wnd));
}

inline CWnd *CUndoable::UndoToWnd(CUndoable *Undoable)
{
	// NOTE: this only works if RTTI is enabled
	return(dynamic_cast<CWnd *>(Undoable));
}

#endif
