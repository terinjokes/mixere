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

#ifndef CUNDOSTATE_INCLUDED
#define CUNDOSTATE_INCLUDED

#pragma pack(push, 1)	// 1-byte packing alignment

#include "SmartBuf.h"

class CUndoManager;

// creates an accessor for a member of an undo state's value
#define CUNDOSTATE_VAL(alias, type, member) \
	inline type& alias(const CUndoState& State) const \
		{ return(const_cast<type&>(State.m_Val.member)); }

class CUndoState : public CSmartBuf {
public:
// Types
	enum {
		MAX_UNDO_KEYS = 4
	};
	typedef	WORD UNDO_KEY;
	class UNDO_PATH {
	public:
		UNDO_KEY	Key[MAX_UNDO_KEYS];
		bool	operator==(const UNDO_PATH& p) const;
		bool	operator!=(const UNDO_PATH& p) const;
	};
	typedef union {
		int		i;
		UINT	u;
		float	f;
		bool	b;
		struct {
			WORD	lo;
			WORD	hi;
		} w;
		struct {
			BYTE	al;
			BYTE	ah;
			BYTE	bl;
			BYTE	bh;
		} c;
	} ELEM;
	typedef struct {
		ELEM	x;
		ELEM	y;
	} VALUE;

// Member data
	VALUE	m_Val;

// Attributes
	WORD	GetCtrlID() const;
	WORD	GetCode() const;

// Construction
	CUndoState();
	CUndoState(const CUndoState& State);
	CUndoState& operator=(const CUndoState& State);
	~CUndoState();

// Operations
	static	CString PathToString(const UNDO_PATH& Path);
	CString	DumpState() const;
	void	Empty();
	bool	IsMatch(const UNDO_PATH& Path, WORD CtrlID, WORD Code) const;

private:
// Member data
	WORD	m_CtrlID;	// ID of notifying control
	WORD	m_Code;		// edit function code
	UNDO_PATH	m_Path;	// access path to handler

// Helpers
	void	Copy(const CUndoState& State);

	friend CUndoManager;
};

#pragma pack(pop)	// restore default packing

inline WORD CUndoState::GetCtrlID() const
{
	return(m_CtrlID);
}

inline WORD CUndoState::GetCode() const
{
	return(m_Code);
}

inline bool CUndoState::UNDO_PATH::operator==(const UNDO_PATH& p) const
{
	return(memcmp(this, &p, sizeof(UNDO_PATH)) == 0);
}

inline bool CUndoState::UNDO_PATH::operator!=(const UNDO_PATH& p) const
{
	return(memcmp(this, &p, sizeof(UNDO_PATH)) != 0);
}

inline bool CUndoState::IsMatch(const UNDO_PATH& Path, WORD CtrlID, WORD Code) const
{
	return(Path == m_Path && CtrlID == m_CtrlID && Code == m_Code);
}

inline void CUndoState::Empty()
{
	CSmartBuf::Empty();
	memset(&m_Val, 0, sizeof(VALUE));
}

#endif
