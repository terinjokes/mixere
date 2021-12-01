// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      21nov03 initial version

		form list view
 
*/

// FormListView.h : interface of the CFormListView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_FORMLISTVIEW_H__21B166D3_F631_4A9B_9142_C0857ED3C3BF__INCLUDED_)
#define AFX_FORMLISTVIEW_H__21B166D3_F631_4A9B_9142_C0857ED3C3BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Undoable.h"
#include "ColumnResizer.h"
#include "FormListItemDlg.h"
#include "FormListInfo.h"
#include "FormListHeader.h"
#include "UndoManager.h"
#include "Clipboard.h"

class CFormListView : public CFormView, public CUndoable
{
// Construction
protected: // create from serialization only
	CFormListView();
	DECLARE_DYNCREATE(CFormListView)

public:
// Initialization
	void	InitView(const CColumnResizer::COLDEF *ColumnInfo, int Columns,
					 int InfoItemSize, LPCSTR ClipboardFormat);

// Attributes
	CFormListItemDlg	*GetItem(int Pos);
	int		GetItemCount() const;
	int		GetFullItemCount() const;
	int		SetItemCount(int Count);
	int		SetItemCountUndoable(int Count);
	int		GetCurPos() const;
	void	SetCurPos(int Pos);
	int		GetSelCount() const;
	int		GetSingleSel() const;
	void	SetSelect(int Pos, bool State);
	bool	IsValidPos(int Pos) const;
	bool	IsEmpty(int Pos) const;
	bool	IsCurEmpty() const;
	bool	IsSelected(int Pos) const;
	bool	IsSelOrCur(int Pos) const;
	bool	IsSelOrCurAndFull(int Pos) const;
	bool	ClipboardHasData() const;
	int		GetColumnCount() const;
	const	CColumnResizer::COLDEF	*GetColumnInfo();
	int		GetColumnWidth(int Col) const;
	bool	SetColumnWidth(int Col, int Width);
	CUndoManager	*GetUndoManager();

// Operations
	void	Copy();
	void	Cut();
	void	Paste();
	void	Insert();
	void	Delete();
	void	SelectAll();
	void	Deselect();
	void	SortItems(int SortKey, bool Descending);
	bool	ResizeColumn(int Col, int Width);
	void	ResizeHeader();
	void	BeginDrag(CFormListItemDlg *Item, const CRect& Bounds);
	void	EnsureVisible(int Pos);

// Item handlers
	void	OnSelect(CFormListItemDlg *Item, int Flags);
	void	OnActivate(CFormListItemDlg *Item);

public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFormListView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType = adjustBorder);
	//}}AFX_VIRTUAL

public:
	virtual ~CFormListView();

// Implementation
protected:
	//{{AFX_DATA(CFormListView)
	enum{ IDD = IDD_FORMLIST_FORM };
	//}}AFX_DATA

// Generated message map functions
protected:
	//{{AFX_MSG(CFormListView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditPaste();
	afx_msg void OnEditInsert();
	afx_msg void OnEditDelete();
	afx_msg void OnEditSelectAll();
	afx_msg void OnEditDeselect();
	afx_msg void OnEditUndo();
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditInsert(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditDelete(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditDeselect(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
// Types
	class CMyColumnResizer : public CColumnResizer {
	public:
		CMyColumnResizer(CFormListView *View);
		int		GetRowCount();
		WNDLIST	GetWndList(int Row);
		int		GetHScroll();

	private:
		CFormListView	*m_View;
	};
	class CMyUndoManager : public CUndoManager {
	public:
		CMyUndoManager(CFormListView *View);
		void	OnModify(bool Modified);

	private:
		CFormListView	*m_View;
	};

// Constants
	enum {	// undo codes: must match m_UndoTitleID
		UCODE_SELECT,
		UCODE_SORT,
		UCODE_CUT,
		UCODE_PASTE,
		UCODE_INSERT,
		UCODE_DELETE,
		UCODE_RAISE_ITEM_COUNT,
		UCODE_LOWER_ITEM_COUNT,
		UCODE_DRAG,
		UCODE_COLUMN_WIDTH,
		UNDO_CODES
	};
	enum {	// undo flags: combine using bitwise OR
		UFLAG_INS		= 0x01,		// insert state items
		UFLAG_DEL		= 0x02,		// delete state items
		UFLAG_HADSEL	= 0x04,		// restore state selection
		UFLAG_EMPTY		= 0x08,		// insert empty items
		UFLAG_DIR		= UFLAG_INS | UFLAG_DEL	// direction mask
	};
	static	const	int		m_UndoTitleID[UNDO_CODES];
	static	const	int		SCROLL_DELAY;

// Member data
	static	CFormListView *m_SortThis;
	CArray<CFormListItemDlg*, CFormListItemDlg*>	m_Item;
	CMyUndoManager		m_UndoManager;
	CMyColumnResizer	m_Resizer;
	CFormListHeader		m_Hdr;
	CClipboard	m_Clipboard;
	GUID		m_ClipboardOwner;
	bool	m_ViewCreated;
	int		m_ItemInfoSize;
	const	CColumnResizer::COLDEF	*m_ColDef;
	int		m_Columns;
	int		m_SortKey;
	int		m_SortDir;
	int		m_CurPos;
	int		m_Selections;
	int		m_BlockBegin;
	int		m_BlockEnd;
	int		m_SelCury;
	int		m_SelOrgy;
	int		m_ScrollDelta;
	int		m_ScrollTimer;
	CRect	m_DragRect;
	int		m_DragPos;
	bool	m_Dragging;
	// Undo
	int		m_NewItemCount;
	int		m_PrevSortKey;
	int		m_PrevSortDir;
	bool	m_Modified;
	LPVOID	m_PasteInfo;
	DWORD	m_PasteLen;

// Helpers
	bool	Create();
	CFormListItemDlg *CreateItem(int ItemIdx);
	void	DestroyItem(int ItemIdx);
	void	MoveItem(CFormListItemDlg *Item, int ItemIdx);
	void	MoveItems();
	void	ResizeToFit();
	void	IndexItems();
	void	PopulateHeader();
	int		PointToItemIdx(CPoint Point);
	void	ToggleSelect(int Pos);
	void	SaveSelection();
	void	ClearSelection();
	int		Insert(int Pos, const void *Info, int Count, bool Move = TRUE);
	void	Delete(int Pos, void *Info, int Count, bool Move = TRUE);
	void	Delete(bool Cut);
	void	DragSelect(const CPoint& Cursor);
	void	AutoScroll(const CPoint& Cursor);
	void	ContinueDrag(const CPoint& Cursor);
	void	EndDrag(const CPoint *Cursor);
	void	DropItems(int Pos);
	// Sort
	template<class T> int SortCmp(const T& a, const T& b)
		{ return(a == b ? 0 : (a < b ? -m_SortDir : m_SortDir)); }
	static	int		SortCompare(const void *elem1, const void *elem2);
	void	Sort(int& a, int& b);
	void	MoveSortedItems();
	// Undo
	void	RestoreUndoItems(const CUndoState& State);
	void	*AllocUndoItemInfo(CUndoState& State, int Items) const;
	CFormListInfo	*GetUndoItemInfo(const CUndoState& State, int Item) const;
	CFormListInfo	*GetItemInfo(const void *Info, int Item) const;
	template<class T> void AllocUndoArray(T*& uap, CUndoState& State, int Items)
	{
		uap = new T[Items];
		State.SetDataPtr(uap, Items * sizeof(T));
		UValItems(State) = Items;
	}
	template<class T> void GetUndoArray(T*& uap, const CUndoState& State)
	{
		uap = (T *)State.GetData();
	}

// Overridables
	virtual	void	OnCreateView();
	virtual	CFormListItemDlg	*OnNewItem();
	virtual	int		MemberSortCompare(const void *elem1, const void *elem2);
	virtual	void	OnPaste(void *Info, int Count, bool Local);

// Undo overrides
	void	SaveUndoState(CUndoState& State);
	void	RestoreUndoState(const CUndoState& State);
	CUndoable	*FindUndoable(CUndoState::UNDO_KEY Key);
	void	GetUndoTitle(const CUndoState& State, CString& Title);

// Aliases for undo value members
	CUNDOSTATE_VAL(	UValItems,		WORD,	x.w.lo)	// item count
	CUNDOSTATE_VAL(	UValCurPos,		WORD,	x.w.hi)	// current position
	CUNDOSTATE_VAL(	UValFlags,		WORD,	y.w.lo)	// undo flags
	CUNDOSTATE_VAL(	UValSortKey,	BYTE,	y.c.al)	// sort key index
	CUNDOSTATE_VAL(	UValSortDesc,	BYTE,	y.c.ah)	// sort descending flag

	NOCOPIES(CFormListView);
};

inline CFormListItemDlg *CFormListView::GetItem(int Pos)
{
	return(m_Item[Pos]);
}

inline int CFormListView::GetItemCount() const
{
	return(m_Item.GetSize());
}

inline int CFormListView::GetCurPos() const
{
	return(m_CurPos);
}

inline int CFormListView::GetSelCount() const
{
	return(m_Selections);
}

inline bool CFormListView::IsValidPos(int Pos) const
{
	return(Pos >= 0 && Pos < GetItemCount());
}

inline int CFormListView::SortCompare(const void *elem1, const void *elem2)
{
	return(m_SortThis->MemberSortCompare(elem1, elem2));
}

inline int CFormListView::GetColumnCount() const
{
	return(m_Columns);
}

inline const CColumnResizer::COLDEF *CFormListView::GetColumnInfo()
{
	return(m_ColDef);
}

inline CUndoManager	*CFormListView::GetUndoManager()
{
	return(&m_UndoManager);
}

inline CFormListInfo *CFormListView::GetItemInfo(const void *Info, int Item) const
{
	return((CFormListInfo *)((BYTE *)Info + Item * m_ItemInfoSize));
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORMLISTVIEW_H__21B166D3_F631_4A9B_9142_C0857ED3C3BF__INCLUDED_)
