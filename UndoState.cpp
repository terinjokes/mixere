// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      22feb04	initial version

        undo state container
 
*/

#include "stdafx.h"
#include "UndoState.h"

CUndoState::CUndoState()
{
}

CUndoState::~CUndoState()
{
}

CUndoState::CUndoState(const CUndoState& State) :
	CSmartBuf(State)	// call base class copy constructor
{
	Copy(State);
}

CUndoState& CUndoState::operator=(const CUndoState& State)
{
	CSmartBuf::operator=(State);	// call base class assignment operator
    Copy(State);
	return(*this);
}

void CUndoState::Copy(const CUndoState& State)
{
	m_Val = State.m_Val;
	m_CtrlID = State.m_CtrlID;
	m_Code = State.m_Code;
	m_Path = State.m_Path;
}

CString CUndoState::PathToString(const UNDO_PATH& Path)
{
	CString	s, t;
	for (int i = 0; i < MAX_UNDO_KEYS; i++) {
		if (i)
			s += " ";
		t.Format("%d", Path.Key[i]);
		s += t;
	}
	return(s);
}

CString CUndoState::DumpState() const
{
	CString	s;
	s.Format("CtrlID=%d Code=%d a=0x%x b=0x%x Path=%s", 
		m_CtrlID, m_Code, m_Val.x.i, m_Val.y.i, PathToString(m_Path));
	return(s);
}
