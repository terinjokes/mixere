// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        visualcpp.net
 
		revision history:
		rev		date	comments
        00      07may03	ck: replaced leading underscores with m_ convention
		01		02oct04	ck: add optional origin shift to initial parent rect
 
		resize a windows control

*/

// CtrlsResize.h: interface for the CCtrlsResize class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CTRLRESIZE_INCLUDED
#define CTRLRESIZE_INCLUDED

#include <afxtempl.h>

const int BIND_TOP =	0x01;
const int BIND_LEFT =	0x02;
const int BIND_RIGHT =	0x04;
const int BIND_BOTTOM =	0x08;
const int BIND_ALL =	0x0F;
const int BIND_UNKNOWN = 0x00;

class CCtrlResize {
public:
	void OnSize();
	void SetParentWnd(CWnd *pWnd);
	int FixControls();
	int AddControl(int _controlID, int _bindtype, CRect& _rectInitial = CRect(NULL));
	void SetOriginShift(SIZE shift);	// ck: added

	class CControlInfo {
	public:
		CControlInfo() : m_controlID(0), m_bindtype(BIND_UNKNOWN), m_rectInitial(NULL) {};
		CControlInfo(int controlID, int bindtype, CRect& rectInitial, CWnd* pControlWnd = NULL);
		virtual ~CControlInfo() {};

		int m_controlID;
		int m_bindtype;
		CRect m_rectInitial;
		CWnd* m_pControlWnd;
	};
	CCtrlResize();
	~CCtrlResize();

private:
	CWnd* m_pWnd;
	CArray <CControlInfo*, CControlInfo*> m_aCtrls;
	CRect m_rectInitialParent;
	CSize m_originShift;	// ck: added
};

#endif
