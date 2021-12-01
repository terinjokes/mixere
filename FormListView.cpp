// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda

		revision history:
		rev		date	comments
        00      21nov03 initial version
		01		30nov03	CreateItem must maximize pos slider
		02		01dec03	MoveItems must call ResizeToFit
		03		02dec03	bring back MaximizeItems
		04		10dec03	don't call ResizeParentToFit
		05		10dec03	add default column widths
		06		11dec03	in Reset, close item first to avoid slow painting
		07		30dec03	add sort items, add set transport
		08		31dec03	add current position
		09		05jan04	sort was moving items needlessly
		10		08jan04	add sort items
		11		19jan04	convert to MDI
		12		23jan04	create item was trashing siblings, must set Z-order
		13		25jan04 make item visible, then set parent, then set Z-order
		14		25jan04	deselect before inserting, otherwise selection scrolls
		15		31jan04	OnActivate now calls SetCurPos
		16		31jan04	create item: set Z-pos, make it visible, then move
		17		11feb04	make item visible with ShowWindow, not ModifyStyle
		18		13feb04	header control is a member of view now
		19		13feb04	improve modularity of clipboard support
		20		17feb04	go back to creating view in OnInitialUpdate
		21		22feb04	add undo
		22		07mar04	undo sort must save indices, not pointers
		23		08mar04	undo sort must restore sort key and direction
		24		09mar04	set wait cursor in RestoreUndoState
		25		25sep04	override CalcWindowRect to make room for header ctrl
		26		25sep04	header's OnViewSize doesn't take args anymore
		27		15nov04	add item dragging
		28		15dec04	in SetItemCount, if doc was empty, set current pos
		29		30dec04	in CreateItem, if OnNewItem fails, return failure
		30		10jan05	add OnPaste handler
		31		02feb05	add undo column width
		32		07feb05	add EnsureVisible; call it from SortItems

		form list view

*/

// FormListView.cpp : implementation of the CFormListView class
//

#include "stdafx.h"
#include "Resource.h"

#include "ChildFrm.h"
#include "FormListView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFormListView

const int CFormListView::m_UndoTitleID[UNDO_CODES] = {	// must match undo code enum
	IDS_FLU_SELECT,
	IDS_FLU_SORT,
	IDS_FLU_CUT,
	IDS_FLU_PASTE,
	IDS_FLU_INSERT,
	IDS_FLU_DELETE,
	IDS_FLU_RAISE_ITEM_COUNT,
	IDS_FLU_LOWER_ITEM_COUNT,
	IDS_FLU_DRAG,
	IDS_FLU_COLUMN_WIDTH
};

const int CFormListView::SCROLL_DELAY = 50;	// milliseconds

CFormListView	*CFormListView::m_SortThis;

IMPLEMENT_DYNCREATE(CFormListView, CFormView)

/////////////////////////////////////////////////////////////////////////////
// CFormListView construction/destruction

CFormListView::CFormListView() : 
	CFormView(CFormListView::IDD),
	m_Clipboard(m_hWnd, NULL),		// don't register a format
#pragma warning(disable : 4355)	// 'this' used in base member initializer list
	// these ctors don't access 'this', they only copy it to a member var
	m_UndoManager(this),
	m_Resizer(this),
	m_Hdr(this)
#pragma warning(default : 4355)
{
	//{{AFX_DATA_INIT(CFormListView)
	//}}AFX_DATA_INIT
	m_ViewCreated = FALSE;
	m_ItemInfoSize = sizeof(CFormListInfo);
	m_ColDef = NULL;
	m_Columns = 0;
	m_SortKey = -1;
	m_SortDir = 0;
	m_CurPos = 0;
	m_Selections = 0;
	m_BlockBegin = 0;
	m_BlockEnd = 0;
	m_SelCury = -1;
	m_SelOrgy = -1;
	m_ScrollDelta = 0;
	m_ScrollTimer = 0;
	m_DragPos = -1;
	m_Dragging = FALSE;
	m_NewItemCount = 0;
	m_PrevSortKey = 0;
	m_PrevSortDir = 0;
	m_Modified = FALSE;
	m_PasteInfo = NULL;
	m_PasteLen = 0;
	SetUndoManager(&m_UndoManager);
	CoCreateGuid(&m_ClipboardOwner);
	m_Clipboard.SetOwner(&m_ClipboardOwner);
}

CFormListView::~CFormListView()
{
	SetItemCount(0);
}

/////////////////////////////////////////////////////////////////////////////
// CFormListView initialization

void CFormListView::InitView(const CColumnResizer::COLDEF *ColumnInfo, int Columns,
							 int InfoItemSize, LPCSTR ClipboardFormat)
{
	m_ColDef = ColumnInfo;
	m_Columns = Columns;
	m_ItemInfoSize = InfoItemSize;
	m_Clipboard.SetFormat(ClipboardFormat);
}

bool CFormListView::Create()
{
	m_Resizer.Create(m_ColDef, m_Columns);	// pass resizer our column info
	m_Hdr.Create();	// create header control at top of our frame
	PopulateHeader();
	OnCreateView();	// create derived class
	m_ViewCreated = TRUE;
	return(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// CFormListView item array

int CFormListView::GetFullItemCount() const
{
	int	FullItems = 0;
	for (int i = 0; i < GetItemCount(); i++) {
		if (!IsEmpty(i))
			FullItems++;
	}
	return(FullItems);
}

int CFormListView::SetItemCount(int Count)
{
	int	Prev = GetItemCount();
	if (Count == Prev)
		return(Prev);
	CWaitCursor	wc;
	if (Count > Prev) {
		m_Item.SetSize(Count);
		for (int i = Prev; i < Count; i++) {
			m_Item[i] = CreateItem(i);
			// if create failed, stop creating items
			if (m_Item[i] == NULL) {
				Count = i;	// number of items actually created
				m_Item.SetSize(Count);	// resize the array
				break;
			}
		}
	} else {
		// close items before deleting them to avoid slow painting
		int i;
		for (i = Count; i < Prev; i++)
			m_Item[i]->CloseItem();
		// now items can be deleted quickly
		for (i = Count; i < Prev; i++)
			DestroyItem(i);
		m_Item.SetSize(Count);
	}
	// current position could be invalid
	if (m_CurPos >= Count || !Prev)
		SetCurPos(0);
	// calculate the minimum size required to fit all items
	ResizeToFit();	// otherwise TFU
	return(Count);	// number of items actually created
}

int CFormListView::SetItemCountUndoable(int Count)
{
	int	Prev = GetItemCount();
	if (Count == Prev)	// nothing to do
		return(Prev);
	if (Count < Prev) {	// deleting items, undo handler must save them
		CWaitCursor	wc;
		m_NewItemCount = Count;	// pass new item count to undo handler
		NotifyUndoableEdit(UCODE_LOWER_ITEM_COUNT);
		// close items before deleting them to avoid slow painting
		for (int i = Count; i < Prev; i++)
			m_Item[i]->CloseItem();
		Delete(Count, NULL, Prev - Count);
	} else {	// adding empty items, undo handler only saves item count
		NotifyUndoableEdit(UCODE_RAISE_ITEM_COUNT);
		Count = SetItemCount(Count);
	}
	return(Count);
}

CFormListItemDlg *CFormListView::CreateItem(int ItemIdx)
{
	CFormListItemDlg *Item = OnNewItem();
	if (Item == NULL)	// if item creation failed, bail out
		return(NULL);
	// make item a child of view, so it scrolls with view
	Item->SetParent(this);
	// set its index
	Item->SetIndex(ItemIdx);
	// set its column widths
	m_Resizer.UpdateRow(Item->GetWndList());
	// move it to bottom of Z-order, to avoid trashing siblings
	Item->SetWindowPos(&CWnd::wndBottom, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	// make it visible
	Item->ShowWindow(SW_NORMAL);
	// move it into place; must be visible, or results are unpredictable
	MoveItem(Item, ItemIdx);
	return(Item);
}

void CFormListView::DestroyItem(int ItemIdx)
{
	CFormListItemDlg	*Item = m_Item[ItemIdx];
	// mark as deleted first, in case destructor iterates over items
	m_Item[ItemIdx] = NULL;
	delete Item;
}

void CFormListView::MoveItem(CFormListItemDlg *Item, int ItemIdx)
{
	CRect	cr;
	Item->GetClientRect(cr);
	cr.OffsetRect(-GetScrollPosition());
	cr.OffsetRect(0, ItemIdx * cr.Height());
	Item->MoveWindow(cr);
}

void CFormListView::ResizeToFit()
{
	CRect	cr, BestFit;
	int	Items = GetItemCount();
	if (Items) {
		m_Item[Items - 1]->GetWindowRect(cr);
		ScreenToClient(cr);
		GetClientRect(BestFit);
		BestFit.BottomRight() = cr.BottomRight() + GetScrollPosition();
	} else
		BestFit.SetRectEmpty();
	SetScrollSizes(MM_TEXT, BestFit.Size());
}

void CFormListView::MoveItems()
{
	for (int i = 0; i < GetItemCount(); i++)
		MoveItem(m_Item[i], i);
	ResizeToFit();
}

void CFormListView::IndexItems()
{
	for (int i = 0; i < GetItemCount(); i++)
		m_Item[i]->SetIndex(i);
}

/////////////////////////////////////////////////////////////////////////////
// CFormListView item sorting

void CFormListView::SortItems(int SortKey, bool Descending)
{
	Deselect();
	m_PrevSortKey = m_SortKey;	// save current sort parameters for undo
	m_PrevSortDir = m_SortDir;
	m_SortKey = SortKey;
	m_SortDir = Descending ? -1 : 1;
	CWaitCursor	wc;
	m_SortThis = this;	// pass sort function our this
	qsort(m_Item.GetData(), GetItemCount(), sizeof(void *), SortCompare);
	NotifyUndoableEdit(UCODE_SORT);
	MoveSortedItems();
	EnsureVisible(m_CurPos);
}

void CFormListView::MoveSortedItems()
{
	// item pointers are in order now, but their windows haven't been moved;
	// move windows in a chain pattern so that no window ever gets overwritten
	CByteArray	Moved;
	Moved.SetSize(GetItemCount());
	int i;
	for (i = 0; i < GetItemCount(); i++) {
		// if window hasn't been moved yet, and needs to move
		if (!Moved[i] && m_Item[i]->GetIndex() != i) {
			// first window in the chain; move it out of harm's way
			MoveItem(m_Item[i], -1);
			Moved[i] = TRUE;
			int	Next = i;
			do {
				Next = m_Item[Next]->GetIndex();	// find next window in chain
				MoveItem(m_Item[Next], Next);	// move it into place
				Moved[Next] = TRUE;
			} while (Next != i);	// until we're back where we started
		}
	}
	// re-index the items and fix current position
	for (i = 0; i < GetItemCount(); i++) {
		m_Item[i]->SetIndex(i);
		if (m_Item[i]->IsActive())
			m_CurPos = i;
	}
}

void CFormListView::EnsureVisible(int Pos)
{
	CRect	vr, ir;
	GetClientRect(vr);	// get view's rect
	m_Item[Pos]->GetWindowRect(ir);	// get item's rect
	ScreenToClient(ir);	//  in view coordinates
	if (ir.top < 0 || ir.bottom > vr.bottom) {	// if item isn't entirely visible
		CPoint	sp = GetScrollPosition();
		sp.y += ir.top;
		ScrollToPosition(sp);	// scroll so that it's at the top of the view
	}
}

/////////////////////////////////////////////////////////////////////////////
// CFormListView position/selection

void CFormListView::SetCurPos(int Pos)
{
	if (m_CurPos < GetItemCount())
		m_Item[m_CurPos]->ActivateItem(FALSE);
	if (Pos < GetItemCount())
		m_Item[Pos]->ActivateItem(TRUE);
	m_CurPos = Pos;
}

void CFormListView::OnActivate(CFormListItemDlg *Item)
{
	int	Pos = Item->GetIndex();
	if (Pos == m_CurPos)		// item already active, nothing to do
		return;
	SetCurPos(Pos);
}

bool CFormListView::IsEmpty(int Pos) const
{
	if (!IsValidPos(Pos))
		return(TRUE);
	return(m_Item[Pos]->IsEmpty());
}

bool CFormListView::IsCurEmpty() const
{
	if (!IsValidPos(m_CurPos))
		return(TRUE);
	return(m_Item[m_CurPos]->IsEmpty());
}

bool CFormListView::IsSelected(int Pos) const
{
	if (!IsValidPos(Pos))
		return(FALSE);
	return(m_Item[Pos]->IsSelected());
}

bool CFormListView::IsSelOrCur(int Pos) const
{
	if (!IsValidPos(Pos))
		return(FALSE);
	return(m_Selections ? m_Item[Pos]->IsSelected() : (Pos == m_CurPos));
}

bool CFormListView::IsSelOrCurAndFull(int Pos) const
{
	return(IsSelOrCur(Pos) && !IsEmpty(Pos));
}

int CFormListView::GetSingleSel() const
{
	if (!m_Selections)
		return(m_CurPos);
	if (m_Selections == 1) {
		for (int i = 0; i < GetItemCount(); i++) {
			if (IsSelected(i))
				return(i);
		}
	}
	return(-1);
}

void CFormListView::SetSelect(int Pos, bool State)
{
	if (!IsUndoInProgress())
		NotifyUndoableEdit(UCODE_SELECT, UE_COALESCE | UE_INSIGNIFICANT);
	if (m_Item[Pos]->IsSelected() != State)
		m_Selections += State ? 1 : -1;
	m_Item[Pos]->SelectItem(State);
}

void CFormListView::ToggleSelect(int Pos)
{
	SetSelect(Pos, !m_Item[Pos]->IsSelected());
}

void CFormListView::SelectAll()
{
	for (int i = 0; i < GetItemCount(); i++)
		SetSelect(i, TRUE);
}

void CFormListView::Deselect()
{
	if (!m_Selections)
		return;	// nothing to do
	if (!IsUndoInProgress())
		NotifyUndoableEdit(UCODE_SELECT, UE_COALESCE | UE_INSIGNIFICANT);
	ClearSelection();	// doesn't send undo notifications
}

void CFormListView::ClearSelection()
{
	for (int i = 0; i < GetItemCount(); i++)
		m_Item[i]->SelectItem(FALSE);
	m_Selections = 0;
}

void CFormListView::SaveSelection()
{
	for (int i = 0; i < GetItemCount(); i++)
		m_Item[i]->SetWasSel(m_Item[i]->IsSelected());
	if (!m_Selections && IsValidPos(m_CurPos))
		m_Item[m_CurPos]->SetWasSel(TRUE);
}

int CFormListView::PointToItemIdx(CPoint Point)
{
	if (!GetItemCount())
		return(-1);
	CRect	wr, cr;
	GetClientRect(wr);
	m_Item[0]->GetWindowRect(cr);
	ScreenToClient(cr);
	Point += GetScrollPosition();
	return(Point.y / cr.Height());
}

inline void CFormListView::Sort(int& a, int& b)
{
	if (a > b) {
		int	t = a;
		a = b;
		b = t;
	}
}

void CFormListView::OnSelect(CFormListItemDlg *Item, int Flags)
{
	if (Flags & MK_SHIFT && m_Selections) {
		// sort previous block boundaries
		int	PrevBeg = m_BlockBegin;
		int	PrevEnd = m_BlockEnd;
		Sort(PrevBeg, PrevEnd);
		// sort new block boundaries
		int	NewBeg = m_BlockBegin;
		int	NewEnd = Item->GetIndex();
		Sort(NewBeg, NewEnd);
		// unselect any portion of previous block that's outside new block
		int i;
		for (i = PrevBeg; i <= PrevEnd; i++) {
			if (i < NewBeg || i > NewEnd)
				SetSelect(i, FALSE);
		}
		// select new block
		for (i = NewBeg; i <= NewEnd; i++)
			SetSelect(i, TRUE);
		// store new end of block position
		m_BlockEnd = Item->GetIndex();
	} else {
		int	Pos = Item->GetIndex();
		SetSelect(Pos, !Item->IsSelected());
		m_BlockBegin = m_BlockEnd = Pos;
		// capture mouse for drag select
		SetCapture();	// released by OnLButtonUp
		m_SelCury = m_SelOrgy = Pos;
	}
}

void CFormListView::DragSelect(const CPoint& Cursor)
{
	if (m_SelOrgy < 0)
		return;
	AutoScroll(Cursor);	// if cursor is outside of view, scroll view
	// extend selection to include item cursor is over
	int	y = min(max(PointToItemIdx(Cursor), 0), GetItemCount() - 1);
	if (y < m_SelOrgy)
		y--;
	while (y > m_SelCury) {
		m_SelCury++;
		if (m_SelCury != m_SelOrgy)
			ToggleSelect(m_SelCury);
	}
	while (y < m_SelCury) {
		if (m_SelCury != m_SelOrgy)
			ToggleSelect(m_SelCury);
		m_SelCury--;
	}
}

void CFormListView::AutoScroll(const CPoint& Cursor)
{
	CRect	r;
	GetClientRect(r);
	CSize	vs = GetTotalSize();
	// if cursor is outside of view, enable auto-scroll
	if (vs.cy > r.Height()) {	// if view is scrollable
		if (Cursor.y < r.top)
			m_ScrollDelta = Cursor.y - r.top;
		else if (Cursor.y > r.bottom)
			m_ScrollDelta = Cursor.y - r.bottom;
		else
			m_ScrollDelta = 0;
	} else
		m_ScrollDelta = 0;
	if (m_ScrollDelta && !m_ScrollTimer)
		m_ScrollTimer = SetTimer(1, SCROLL_DELAY, NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CFormListView item dragging

void CFormListView::BeginDrag(CFormListItemDlg *Item, const CRect& Bounds)
{
	m_DragPos = Item->GetIndex();
	m_DragRect = Bounds;
	Item->MapWindowPoints(this, m_DragRect);
	SetCapture();
}

void CFormListView::ContinueDrag(const CPoint& Cursor)
{
	if (m_DragPos < 0)
		return;
	if (!m_Dragging) {
		// if cursor moved outside drag detection bounds, start dragging
		if (Cursor.y < m_DragRect.top || Cursor.y > m_DragRect.bottom)
			m_Dragging = TRUE;
	}
	if (m_Dragging) {
		CRect	r;
		GetClientRect(r);
		int	ResID;
		if (r.PtInRect(Cursor))	// if within view, show drag cursor
			ResID = GetSelCount() ? IDC_DRAG_MULTI : IDC_DRAG_SINGLE;
		else
			ResID = AFX_IDC_NODROPCRSR;	// show no drop cursor
		SetCursor(AfxGetApp()->LoadCursor(ResID));
		AutoScroll(Cursor);	// if cursor is outside of view, scroll view
	}
}

void CFormListView::EndDrag(const CPoint *Cursor)
{
	if (m_Dragging) {		// if dragging items
		m_Dragging = FALSE;
		ReleaseCapture();		// release mouse
		if (Cursor != NULL) {	// if drag wasn't aborted
			CRect	r;
			GetClientRect(r);
			if (r.PtInRect(*Cursor)) {	// if cursor still within view
				int	DropPos = PointToItemIdx(*Cursor);
				if (DropPos != m_DragPos)	// if moved from starting position
					DropItems(DropPos);
			}
		}
	}
	m_DragPos = -1;		// reset drag detect state
}

void CFormListView::DropItems(int Pos)
{
	Pos = min(max(Pos, 0), GetItemCount());	// clamp insert position
	int	sels = max(GetSelCount(), 1);
	CFormListItemDlg	**ip = new CFormListItemDlg *[sels];
	if (GetSelCount()) {	// if selection, drop selected items
		int	j = 0;
		for (int i = 0; i < GetItemCount(); i++) {
			if (IsSelected(i)) {
				ip[j++] = m_Item[i];	// copy item pointer to list
				m_Item[i] = NULL;		// mark element for deletion
			}
		}
	} else {	// drop current item
		ip[0] = m_Item[m_CurPos];	// copy item pointer to list
		m_Item[m_CurPos] = NULL;	// mark element for deletion
	}
	int i;
	for (i = 0; i < sels; i++)	// insert copied pointers
		m_Item.InsertAt(Pos + i, ip[i]);
	i = 0;
	while (i < GetItemCount()) {	// remove deleted elements
		if (m_Item[i] == NULL)
			m_Item.RemoveAt(i);
		else
			i++;
	}
	m_PrevSortKey = m_SortKey;	// save current sort parameters for undo
	m_PrevSortDir = m_SortDir;
	m_SortKey = -1;	// reset sort parameters
	m_SortDir = 0;
	m_Hdr.SetSort(m_SortKey, m_SortDir != 0);
	NotifyUndoableEdit(UCODE_DRAG);
	MoveSortedItems();
}

/////////////////////////////////////////////////////////////////////////////
// CFormListView clipboard

int CFormListView::Insert(int Pos, const void *Info, int Count, bool Move)
{
	ASSERT(Pos >= 0 && Pos <= GetItemCount());	// pos can equal item count
	CFormListItemDlg	*CurItem;
	if (IsValidPos(m_CurPos))
		CurItem = m_Item[m_CurPos];	// save current position
	else
		CurItem = NULL;
	// insert items and create them
	m_Item.InsertAt(Pos, NULL, Count);
	int i;
	for (i = 0; i < Count; i++) {
		m_Item[Pos + i] = CreateItem(Pos + i);
		// if create failed, abort entire insert
		if (m_Item[Pos + i] == NULL) {
			// must clean up before displaying message, because AfxMessageBox 
			// allows timer hook to run, and null item pointer will crash it
			for (int j = 0; j < i; j++)
				DestroyItem(Pos + j);
			m_Item.RemoveAt(Pos, Count);
			Count = 0;
			AfxMessageBox(LDS(FL_CANT_INSERT_ITEM));
			break;
		}
		if (Info != NULL)
			m_Item[Pos + i]->OpenItem(GetItemInfo(Info, i));
	}
	// set item info
	if (Info != NULL) {
		for (i = 0; i < Count; i++)
			m_Item[Pos + i]->SetInfo(GetItemInfo(Info, i));
	}
	// clean up
	if (Move)
		MoveItems();
	IndexItems();
	SetCurPos(CurItem != NULL ? CurItem->GetIndex() : 0);	// fix current position
	return(Count);	// number of items actually inserted
}

void CFormListView::Delete(int Pos, void *Info, int Count, bool Move)
{
	ASSERT(IsValidPos(Pos));
	int	EndPos = Pos + Count;
	ASSERT(Count >= 0 && EndPos <= GetItemCount());
	// if current position will be deleted, try to find a safe position
	int	SafePos = m_CurPos;
	if (m_CurPos >= Pos && m_CurPos < EndPos) {
		if (EndPos < GetItemCount())
			SafePos = EndPos;	// after deletion
		else {
			if (Pos)
				SafePos = Pos - 1;	// before deletion
			else
				SafePos = -1;	// no safe position exists
		}
	}
	CFormListItemDlg	*SafeItem = NULL;	// pointer to safe item
	if (SafePos >= 0)
		SafeItem = m_Item[SafePos];
	// destroy the items and remove them
	for (int i = 0; i < Count; i++) {
		// if item info was requested, retrieve it
		if (Info != NULL)
			m_Item[Pos + i]->GetInfo(GetItemInfo(Info, i));
		DestroyItem(Pos + i);
	}
	m_Item.RemoveAt(Pos, Count);
	// clean up
	if (Move)
		MoveItems();
	IndexItems();
	SetCurPos(SafeItem != NULL ? SafeItem->GetIndex() : 0);	// fix current position
}

void CFormListView::Delete(bool Cut)
{
	CWaitCursor	wc;
	if (!GetItemCount())
		return;		// nothing to do
	NotifyUndoableEdit(Cut ? UCODE_CUT : UCODE_DELETE);
	BYTE	*cip;
	int	Items = max(m_Selections, 1);	// either selections, or current item
	if (Cut)
		cip = new BYTE[Items * m_ItemInfoSize];	// allocate memory for items
	// close items before deleting them to avoid slow painting
	int	j = 0;
	for (int i = 0; i < GetItemCount(); i++) {
		if (IsSelOrCur(i)) {
			if (Cut)
				m_Item[i]->GetInfo(GetItemInfo(cip, j++));	// save info before closing
			m_Item[i]->CloseItem();
		}
	}
	// save and clear selection
	SaveSelection();
	ClearSelection();
	// delete selected items
	int Pos = 0;
	while (Pos < GetItemCount()) {
		int	i = Pos;	// assume a block of contiguous selected items
		while (i < GetItemCount() && m_Item[i]->GetWasSel())
			i++;
		int	Len = i - Pos;	// length of block
		if (Len > 0)
			Delete(Pos, NULL, Len, FALSE);	// don't move items yet
		else
			Pos++;
	}
	// if cutting, write deleted items to clipboard
	if (Cut) {
		m_Clipboard.Write(cip, Items * m_ItemInfoSize);
		delete [] cip;	// free item memory
	}
	MoveItems();	// move items all at once, looks better
}

void CFormListView::Copy()
{
	CWaitCursor	wc;
	if (!GetItemCount())
		return;		// nothing to do
	BYTE	*cip;
	int	Items = max(m_Selections, 1);	// either selections, or current item
	cip = new BYTE[Items * m_ItemInfoSize];	// allocate memory for items
	// save selected items
	int	j = 0;
	for (int i = 0; i < GetItemCount(); i++) {
		if (IsSelOrCur(i))
			m_Item[i]->GetInfo(GetItemInfo(cip, j++));
	}
	// write saved items to clipboard
	m_Clipboard.Write(cip, Items * m_ItemInfoSize);
	delete [] cip;	// free item memory
	ClearSelection();
}

void CFormListView::Cut()
{
	Delete(TRUE);	// copy deleted items to clipboard
}

void CFormListView::Paste()
{
	GUID	Owner;
	m_PasteInfo = m_Clipboard.Read(m_PasteLen, &Owner);	// read paste items
	if (m_PasteInfo == NULL)
		return;
	int	Pos = GetSingleSel();
	if (Pos < 0)
		return;
	CWaitCursor	wc;
	ClearSelection();
	SetCurPos(Pos);	// undo assumes paste at current position
	int	Count = m_PasteLen / m_ItemInfoSize;	// get item count from length
	// determine if paste items came from this instance
	bool	Local = !memcmp(&Owner, &m_ClipboardOwner, sizeof(GUID));
	OnPaste(m_PasteInfo, Count, Local);
	NotifyUndoableEdit(UCODE_PASTE);
	Insert(Pos, m_PasteInfo, Count);	// and insert them
	delete [] m_PasteInfo;	// free paste items
}

void CFormListView::Insert()
{
	if (!GetItemCount())
		return;		// nothing to do
	NotifyUndoableEdit(UCODE_INSERT);
	SaveSelection();
	ClearSelection();
	int Pos = 0;
	while (Pos < GetItemCount()) {
		int	i = Pos;	// assume a block of contiguous selected items
		while (i < GetItemCount() && m_Item[i]->GetWasSel())
			i++;
		int	Len = i - Pos;	// length of block
		if (Len > 0) {
			Insert(Pos, NULL, Len);
			Pos += Len * 2;	// skip block and inserted items
		} else
			Pos++;
	}
}

void CFormListView::Delete()
{
	Delete(FALSE);	// don't copy deleted items to clipboard
}

bool CFormListView::ClipboardHasData() const
{
	return(m_Clipboard.HasData());
}

/////////////////////////////////////////////////////////////////////////////
// CFormListView header control

bool CFormListView::ResizeColumn(int Col, int Width)
{
	ASSERT(Col >= 0 && Col < m_Columns);
	if (!IsUndoInProgress())
		NotifyUndoableEdit(UCODE_COLUMN_WIDTH, UE_COALESCE);
	bool	retc = m_Resizer.SetColumnWidth(Col, Width);
	if (retc)
		ResizeToFit();
	return(retc);
}

void CFormListView::PopulateHeader()
{
	if (m_ColDef != NULL) {
		HDITEM	hdi;
		hdi.mask = HDI_TEXT | HDI_WIDTH | HDI_FORMAT;
		hdi.fmt = HDF_STRING | HDF_CENTER;
		CString	Name;
		for (int i = 0; i < m_Columns; i++) {
			Name.LoadString(m_ColDef[i].NameID);
			hdi.pszText = Name.GetBuffer(0);
			hdi.cchTextMax = Name.GetLength();
			hdi.cxy = m_ColDef[i].DefaultWidth;
			m_Hdr.InsertItem(i, &hdi);
		}
	}
}

int CFormListView::GetColumnWidth(int Col) const
{
	ASSERT(Col >= 0 && Col < m_Columns);
	return(m_Resizer.GetColumnWidth(Col));
}

bool CFormListView::SetColumnWidth(int Col, int Width)
{
	ASSERT(Col >= 0 && Col < m_Columns);
	m_Hdr.SetWidth(Col, Width);
	return(TRUE);
}

void CFormListView::ResizeHeader()
{
	m_Hdr.OnViewSize();
}

/////////////////////////////////////////////////////////////////////////////
// CFormListView undo/redo

void *CFormListView::AllocUndoItemInfo(CUndoState& State, int Items) const
{
	BYTE	*uap = new BYTE[Items * m_ItemInfoSize];
	State.SetDataPtr(uap, Items * m_ItemInfoSize);
	UValItems(State) = Items;
	return(uap);
}

CFormListInfo *CFormListView::GetUndoItemInfo(const CUndoState& State, int Item) const
{
	return(GetItemInfo(State.GetData(), Item));
}

void CFormListView::SaveUndoState(CUndoState& State)
{
	int	Items = GetItemCount();
	UValCurPos(State) = m_CurPos;	// do first so handlers can override
	switch (State.GetCode()) {
	case UCODE_SELECT:
		{
			bool	*uap;
			AllocUndoArray(uap, State, Items);
			for (int i = 0; i < Items; i++)
				uap[i] = m_Item[i]->IsSelected();
		}
		break;
	case UCODE_SORT:
	case UCODE_DRAG:
		{
			if (State.IsEmpty()) {	// if first time
				// copy sorted indices
				WORD	*uap;
				AllocUndoArray(uap, State, Items);
				for (int i = 0; i < Items; i++)
					uap[i] = m_Item[i]->GetIndex();
				// save previous sort parameters
				UValSortKey(State) = m_PrevSortKey;
				UValSortDesc(State) = m_PrevSortDir < 0;
			} else {
				// invert sortedness of indices
				WORD	*Temp = new WORD[Items];
				WORD	*uap;
				GetUndoArray(uap, State);
				memcpy(Temp, uap, Items * sizeof(WORD));
				for (int i = 0; i < Items; i++)
					uap[Temp[i]] = i;
				delete [] Temp;
				// save current sort parameters
				UValSortKey(State) = m_Hdr.GetSortCol();
				UValSortDesc(State) = m_Hdr.GetSortDesc();
			}
		}
		break;
	case UCODE_CUT:
	case UCODE_DELETE:
		if (!UValFlags(State)) {
			void	*uap = AllocUndoItemInfo(State, max(m_Selections, 1));
			int	j = 0;
			for (int i = 0; i < Items; i++) {
				if (IsSelOrCur(i))
					m_Item[i]->GetInfo(GetItemInfo(uap, j++));
			}
			UValFlags(State) = UFLAG_INS;	// first restore will insert
			if (m_Selections)
				UValFlags(State) |= UFLAG_HADSEL;
		} else
			UValFlags(State) ^= UFLAG_DIR;	// reverse direction
		break;
	case UCODE_PASTE:
		if (!UValFlags(State)) {
			int	Count = m_PasteLen / m_ItemInfoSize;
			void	*uap = AllocUndoItemInfo(State, Count);
			memcpy(uap, m_PasteInfo, m_PasteLen);	// copy paste items
			// assign new item indices, starting from insert position
			for (int i = 0; i < Count; i++)
				GetItemInfo(uap, i)->m_Index = m_CurPos + i;
			UValFlags(State) = UFLAG_DEL;	// first restore will delete
		} else
			UValFlags(State) ^= UFLAG_DIR;	// reverse direction
		break;
	case UCODE_INSERT:
		if (!UValFlags(State)) {
			void	*uap = AllocUndoItemInfo(State, max(m_Selections, 1));
			int	j = 0, Len = 0, Offset = 0;
			for (int i = 0; i < Items; i++) {
				if (IsSelOrCur(i)) {
					// index must compensate for previous insertions
					GetItemInfo(uap, j++)->m_Index = i + Offset;
					Len++;			// bump block length
				} else {		// end of block
					Offset += Len;	// increase compensation by block length
					Len = 0;		// reset block length
				}
			}
			// specify empty flag; tells restore to insert empty items
			UValFlags(State) = UFLAG_EMPTY | UFLAG_DEL;	// first restore will delete
			if (m_Selections)
				UValFlags(State) |= UFLAG_HADSEL;
		} else
			UValFlags(State) ^= UFLAG_DIR;	// reverse direction
		break;
	case UCODE_RAISE_ITEM_COUNT:
		UValItems(State) = Items;
		break;
	case UCODE_LOWER_ITEM_COUNT:
		if (!UValFlags(State)) {
			int	DeleteCount = Items - m_NewItemCount;
			void	*uap = AllocUndoItemInfo(State, DeleteCount);
			for (int i = 0; i < DeleteCount; i++)
				m_Item[m_NewItemCount + i]->GetInfo(GetItemInfo(uap, i));
			UValFlags(State) = UFLAG_INS;	// first restore will insert
		} else
			UValFlags(State) ^= UFLAG_DIR;	// reverse direction
		break;
	case UCODE_COLUMN_WIDTH:
		{
			int	*uap;
			AllocUndoArray(uap, State, m_Columns);
			for (int i = 0; i < m_Columns; i++)
				uap[i] = m_Resizer.GetColumnWidth(i);
		}
		break;
	default:
		NODEFAULTCASE;
	}
}

void CFormListView::RestoreUndoItems(const CUndoState& State)
{
	if ((UValFlags(State) & UFLAG_DIR) == UFLAG_INS) {
		bool	IsEmpty = (UValFlags(State) & UFLAG_EMPTY) != 0;
		ClearSelection();
		for (int i = 0; i < UValItems(State); i++) {
			CFormListInfo	*uap = GetUndoItemInfo(State, i);
			Insert(uap->m_Index, IsEmpty ? NULL : uap, 1, FALSE);
		}
		MoveItems();
	} else {
		ClearSelection();
		for (int i = UValItems(State) - 1; i >= 0; i--) {
			CFormListInfo	*uap = GetUndoItemInfo(State, i);
			Delete(uap->m_Index, NULL, 1, FALSE);
		}
		MoveItems();
	}
}

void CFormListView::RestoreUndoState(const CUndoState& State)
{
	CWaitCursor	wc;
	int	i;
	switch (State.GetCode()) {
	case UCODE_SELECT:
		{
			bool	*uap;
			GetUndoArray(uap, State);
			for (i = 0; i < UValItems(State); i++)
				SetSelect(i, uap[i]);
		}
		break;
	case UCODE_SORT:
	case UCODE_DRAG:
		{
			// restore sort key and direction
			m_SortKey = UValSortKey(State);
			m_SortDir = UValSortDesc(State) ? -1 : 1;
			// restore header sort state
			m_Hdr.SetSort(UValSortKey(State), UValSortDesc(State) != 0);
			// reorder item pointers according to saved indices
			WORD	*uap;
			GetUndoArray(uap, State);
			CFormListItemDlg	**Item = new CFormListItemDlg *[UValItems(State)];
			for (int i = 0; i < UValItems(State); i++)
				Item[uap[i]] = m_Item[i];
			for (i = 0; i < UValItems(State); i++)
				m_Item[i] = Item[i];
			delete [] Item;
			MoveSortedItems();
		}
		break;
	case UCODE_CUT:
	case UCODE_DELETE:
		RestoreUndoItems(State);
		// if we inserted deletion, restore selection
		if ((UValFlags(State) & UFLAG_DIR) == UFLAG_INS
		&& (UValFlags(State) & UFLAG_HADSEL)) {
			for (int i = 0; i < UValItems(State); i++)
				SetSelect(GetUndoItemInfo(State, i)->m_Index, TRUE);
		}
		break;
	case UCODE_PASTE:
		RestoreUndoItems(State);
		break;
	case UCODE_INSERT:
		RestoreUndoItems(State);
		// if we deleted insertion, restore selection
		if ((UValFlags(State) & UFLAG_DIR) == UFLAG_DEL
		&& (UValFlags(State) & UFLAG_HADSEL)) {
			int	Prev = 0, Len = 0, Offset = 0;
			for (i = 0; i < UValItems(State); i++) {
				CFormListInfo	*uap = GetUndoItemInfo(State, i);
				if (uap->m_Index != Prev) {	// if start of new block
					Offset = Len;				// update compensation
					Prev = uap->m_Index;		// reset block detector
				}
				SetSelect(uap->m_Index - Offset, TRUE);
				Len++;
				Prev++;
			}
		}
		break;
	case UCODE_RAISE_ITEM_COUNT:
		SetItemCount(UValItems(State));
		break;
	case UCODE_LOWER_ITEM_COUNT:
		RestoreUndoItems(State);
		break;
	case UCODE_COLUMN_WIDTH:
		{
			int	*uap;
			GetUndoArray(uap, State);
			for (i = 0; i < UValItems(State); i++)
				SetColumnWidth(i, uap[i]);
		}
		break;
	default:
		NODEFAULTCASE;
	}
	SetCurPos(UValCurPos(State));
}

CUndoable *CFormListView::FindUndoable(CUndoState::UNDO_KEY Key)
{
	return(WndToUndo(GetItem(Key - 1)));	// zero is reserved for unused keys
}

void CFormListView::GetUndoTitle(const CUndoState& State, CString& Title)
{
	int	Code = State.GetCode();
	ASSERT(Code >= 0 && Code < UNDO_CODES);
	Title.LoadString(m_UndoTitleID[Code]);
}

CFormListView::CMyUndoManager::CMyUndoManager(CFormListView *View)
	: CUndoManager(View), m_View(View)
{
}

void CFormListView::CMyUndoManager::OnModify(bool Modified)
{
	CDocument	*pDoc = m_View->GetDocument();
	if (pDoc != NULL)
		pDoc->SetModifiedFlag(Modified);
}

/////////////////////////////////////////////////////////////////////////////
// CFormListView overridables

void CFormListView::OnCreateView()
{
}

CFormListItemDlg *CFormListView::OnNewItem()
{
	return(NULL);	// CFormListItemDlg is abstract, can't create it
}

int CFormListView::MemberSortCompare(const void *elem1, const void *elem2)
{
	return(0);
}

void CFormListView::OnPaste(void *Info, int Count, bool Local)
{
}

/////////////////////////////////////////////////////////////////////////////
// CFormListView overrides

void CFormListView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFormListView)
	//}}AFX_DATA_MAP
}

BOOL CFormListView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CFormView::PreCreateWindow(cs);
}

void CFormListView::OnInitialUpdate()
{
	// create view here, not in OnCreate, because otherwise if replace files
	// dialog happens during serialization, view gets drawn in pieces, which
	// looks sloppy, and in some cases maximize state doesn't get restored
	Create();
	CFormView::OnInitialUpdate();
}

void CFormListView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
}

void CFormListView::CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType) 
{
	CFormView::CalcWindowRect(lpClientRect, nAdjustType);
	lpClientRect->top += CFormListHeader::HEADER_HEIGHT;	// make room for header
}

/////////////////////////////////////////////////////////////////////////////
// CFormListView column resizer overrides

CFormListView::CMyColumnResizer::CMyColumnResizer(CFormListView *View)
	: m_View(View)
{
}

int CFormListView::CMyColumnResizer::GetRowCount()
{
	return(m_View->GetItemCount());
}

CColumnResizer::WNDLIST CFormListView::CMyColumnResizer::GetWndList(int Row)
{
	return(m_View->GetItem(Row)->GetWndList());
}

int CFormListView::CMyColumnResizer::GetHScroll()
{
	return(m_View->GetScrollPosition().x);
}

/////////////////////////////////////////////////////////////////////////////
// CFormListView message map

BEGIN_MESSAGE_MAP(CFormListView, CFormView)
	//{{AFX_MSG_MAP(CFormListView)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_TIMER()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_INSERT, OnEditInsert)
	ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_COMMAND(ID_EDIT_DESELECT, OnEditDeselect)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_INSERT, OnUpdateEditInsert)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateEditDelete)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DESELECT, OnUpdateEditDeselect)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFormListView message handlers

int CFormListView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// must set mapping mode before first size message is received; else
	// debug version asserts when header control calls GetScrollPosition
	SetScrollSizes(MM_TEXT, CSize(0, 0));

	return 0;
}

void CFormListView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	Deselect();	// left-click on view background clears selection
	CFormView::OnLButtonDown(nFlags, point);
}

void CFormListView::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();	// release mouse
	m_SelOrgy = -1;		// reset drag select state
	m_ScrollDelta = 0;	// disable auto-scroll
	if (m_ScrollTimer) {	// kill auto-scroll timer
		KillTimer(m_ScrollTimer);
		m_ScrollTimer = 0;
	}
	if (m_DragPos >= 0)
		EndDrag(&point);
	CFormView::OnLButtonUp(nFlags, point);
}

void CFormListView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_DragPos >= 0)
		ContinueDrag(point);
	if (m_SelOrgy >= 0 && (nFlags & MK_LBUTTON))
		DragSelect(point);
	CFormView::OnMouseMove(nFlags, point);
}

void CFormListView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	if (m_ViewCreated)		// needed because of delayed view creation
		m_Hdr.OnViewSize();
}

void CFormListView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CFormView::OnHScroll(nSBCode, nPos, pScrollBar);
	m_Hdr.OnViewHScroll(nSBCode, nPos, pScrollBar);
}

void CFormListView::OnTimer(UINT nIDEvent) 
{
	if (m_ScrollDelta) {	// if auto-scrolling view
		ScrollToPosition(CPoint(GetScrollPosition() 
			+ CPoint(0, m_ScrollDelta)));
	}
	CFormView::OnTimer(nIDEvent);
}

void CFormListView::OnEditCopy()
{
	Copy();
}

void CFormListView::OnEditCut()
{
	Cut();
}

void CFormListView::OnEditPaste()
{
	Paste();
}

void CFormListView::OnEditInsert()
{
	Insert();
}

void CFormListView::OnEditDelete()
{
	Delete();
}

void CFormListView::OnEditSelectAll() 
{
	SelectAll();
}

void CFormListView::OnEditDeselect()
{
	Deselect();
}

void CFormListView::OnEditUndo() 
{
	m_UndoManager.Undo();
}

void CFormListView::OnEditRedo() 
{
	m_UndoManager.Redo();
}

void CFormListView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetItemCount());
}

void CFormListView::OnUpdateEditCut(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetSelCount());
}

void CFormListView::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(ClipboardHasData() && GetSingleSel() >= 0);
}

void CFormListView::OnUpdateEditInsert(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetSelCount());
}

void CFormListView::OnUpdateEditDelete(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetSelCount());
}

void CFormListView::OnUpdateEditSelectAll(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetItemCount());
}

void CFormListView::OnUpdateEditDeselect(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetSelCount());
}

void CFormListView::OnUpdateEditUndo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_UndoManager.IsUndoable());
	CString	Text;
	Text.Format(LDS(FL_EDIT_UNDO_FMT), m_UndoManager.GetUndoTitle());
	pCmdUI->SetText(Text);
}

void CFormListView::OnUpdateEditRedo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_UndoManager.IsRedoable());
	CString	Text;
	Text.Format(LDS(FL_EDIT_REDO_FMT), m_UndoManager.GetRedoTitle());
	pCmdUI->SetText(Text);
}

BOOL CFormListView::PreTranslateMessage(MSG* pMsg) 
{
	// if escape was pressed while dragging items, abort drag
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE && m_DragPos >= 0) {
		EndDrag(NULL);	// null means don't drop
		return TRUE;	// eat the key
	}
	return CFormView::PreTranslateMessage(pMsg);
}
