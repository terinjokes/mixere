// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      26dec03 initial version
		01		29dec03	add notification codes
		02		02jan04	automation dlg handles selection now
		03		08jan04	implement custom draw
		04		11jan04	clear selection in right button down instead of up
		05		16jan04	add OnKeyDown and OnMouseWheel
		06		21jan04	disable thumb positioning during automation
		07		23jan04	position auto dlg relative to slider ctrl
		08		24jan04	add modify notification
		09		25jan04	don't handle key down, use accelerator table
		10		26jan04	move normalization into base class
		11		27jan04	add get and set reverse
		12		29jan04	override get/set pos
		13		01feb04	allow thumb positioning during play
		14		02feb04	OnSelection and OnTransport are virtual now
		15		15feb04	HScroll calls OnSelection now
		16		19feb04	don't set course in OnSelection anymore
		17		22feb04	add undo
		18		25feb04	set parent again after ShowWindow
		19		10mar04	add scale to timer hook
		20		22mar04	in OnSelection, set start/end even if no selection
		21		16apr04	must use SetInfoPtr now
		22		19apr04	in custom draw, get thumb rect from base class
		23		19apr04	add animation delay to reduce overhead
		24		20apr04	add dialog save/restore undo state
		25		24jun04	add fine adjust
		26		28jun04	move selection now clamps without changing length
		27		26sep04	fix move selection clamping in HScroll
		28		26sep04	AdjSel was notifying selection instead of position
		29		26sep04	remove OnNewPos/OnSelection from RestoreUndoState
		30		29sep04	add EnableAutoDlg to allow fine adjust w/o auto dlg
		31		05oct04	no more child dialogs; disable SetAutoDlgParent
		32		12nov04	SetInfo and similar accessors must update auto dlg's UI
		33		14dec04	add quick start
		34		29dec04	OnLButtonDown must end select slider's modal state
		35		29dec04	add support for Ctrl+Home and Ctrl+End to AdjSel
		36		31dec04	make waveform variable
		37		03jan05	allow derived sliders to disable waveform combo box
		38		04jan05	in OnTransport, use AtStart/AtEnd for play from pause
		39		07jan05	quick start is now alt+left-click
		40		09jan05	add go
		41		24feb05	allow derived sliders to disable loop button

		slider with automation support
 
*/

// AutoSliderCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "AutoSliderCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAutoSliderCtrl

const	CPen	CAutoSliderCtrl::m_PenDisabled(PS_SOLID, 0, RGB(128, 128, 128));
const	CBrush	CAutoSliderCtrl::m_BrStop(RGB(255, 0, 0));		// red
const	CBrush	CAutoSliderCtrl::m_BrPause(RGB(255, 255, 0));	// yellow
const	CBrush	CAutoSliderCtrl::m_BrPlay(RGB(0, 255, 0));		// green

// map transport states to thumb indicator colors
const	CBrush	*CAutoSliderCtrl::m_BrTransport[TRANSPORT_STATES] = {
	&m_BrStop,
	&m_BrPause,
	&m_BrPlay,
	&m_BrDisabled
};

// map transport states to notification codes
const	int		CAutoSliderCtrl::m_TransportNotification[TRANSPORT_STATES] = {
	AUTOSLIDER_STOP,
	AUTOSLIDER_PAUSE,
	AUTOSLIDER_PLAY,
	AUTOSLIDER_UNSET
};

const	int		CAutoSliderCtrl::m_FineAdjustPrecision = 16;

HACCEL CAutoSliderCtrl::m_Accel;

int CAutoSliderCtrl::m_AnimDelay;
int CAutoSliderCtrl::m_AnimCount;
int CAutoSliderCtrl::m_AnimBase;

int	CAutoSliderCtrl::m_Options = DEFAULT_OPTIONS;

CAutoSliderCtrl::CAutoSliderCtrl()
{
	if (m_Accel == NULL)
		m_Accel = LOADACCEL(IDR_AUTO_SLIDER);
	m_AutoDlg = NULL;
	m_PrevTransport = 0;
	m_AutoDlgParent = NULL;
	m_HasToolTips = FALSE;
	m_Cookie = NULL;
	m_AutoStyle = AUTO_DIALOG | AUTO_DLG_WAVEFORM | AUTO_DLG_LOOP;
	m_WavePos = 0;
}

CAutoSliderCtrl::~CAutoSliderCtrl()
{
	delete m_AutoDlg;
}

void CAutoSliderCtrl::GetInfo(CAutoInfo& Info) const
{
	Info = m_Info;
}

void CAutoSliderCtrl::SetInfo(const CAutoInfo& Info)
{
	m_Info = Info;
	CSelectSliderCtrl::SetNormPos(Info.m_Pos);
	CSelectSliderCtrl::SetNormSelection(Info.m_StartPos, Info.m_EndPos);
	UpdateAutoDlgUI();
}

void CAutoSliderCtrl::SetInfoNotify(const CAutoInfo& Info)
{
	SetInfo(Info);
	OnSelection();
	OnNewPos();
}

void CAutoSliderCtrl::SetTime(int Time)
{
	m_Info.m_Time = Time;
	m_Info.m_TimeUnit = CTimeEdit::GetAppropriateUnit(Time);
	UpdateAutoDlgUI();
}

void CAutoSliderCtrl::SetSelection(int nMin, int nMax)
{
	CSelectSliderCtrl::SetSelection(nMin, nMax);
	m_Info.m_StartPos = float(SliderToNorm(nMin));
	m_Info.m_EndPos = float(SliderToNorm(nMax));
	OnSelection();
}

void CAutoSliderCtrl::SetNormSelection(float fMin, float fMax)
{
	CSelectSliderCtrl::SetNormSelection(fMin, fMax);
	m_Info.m_StartPos = fMin;
	m_Info.m_EndPos = fMax;
	OnSelection();
}

void CAutoSliderCtrl::SetPos(int Pos)
{
	CSelectSliderCtrl::SetPos(Pos);
	m_Info.m_Pos = SliderToNorm(Pos);
	OnNewPos();
}

void CAutoSliderCtrl::SetNormPos(double Pos)
{
	CSelectSliderCtrl::SetNormPos(Pos);
	m_Info.m_Pos = Pos;
	OnNewPos();
}

void CAutoSliderCtrl::SetReverse(bool Reverse)
{
	m_Info.m_Reverse = Reverse;
}

void CAutoSliderCtrl::SetWaveform(int Waveform)
{
	m_Info.m_Waveform = Waveform;
	UpdateAutoDlgUI();
}

void CAutoSliderCtrl::SetTransport(int Transport)
{
	OnTransport(Transport);
	m_Info.m_Transport = Transport;
	Redraw();	// force update of transport indicator
	UpdateAutoDlgUI();
}

void CAutoSliderCtrl::SetLoop(bool Enable)
{
	m_Info.m_Loop = Enable;
	UpdateAutoDlgUI();
}

void CAutoSliderCtrl::Go()
{
	if (m_Info.m_Transport == PAUSE) {
		SetTransport(PLAY);
		ShowAutoDlg(FALSE, FALSE);
	}
}

void CAutoSliderCtrl::ShowAutoDlg(bool Enable, bool Undoable)
{
	if (Enable && (m_AutoStyle & AUTO_DIALOG)) {
		if (!IsAutoDlgVisible()) {
			if (Undoable && !IsUndoInProgress())
				NotifyUndoableEdit(UNDO_SHOW_DLG, UE_INSIGNIFICANT);
			if (m_AutoDlg == NULL) {
				m_AutoDlg = new CMyAutoDlg(this);
				m_AutoDlg->Create(IDD_AUTO_DLG);
				Notify(AUTOSLIDER_CREATE);
				if (m_AutoDlgParent != NULL)
					m_AutoDlg->SetParent(m_AutoDlgParent);
				m_AutoDlg->SetOwner(this);	// so we can find undo manager
				if (m_HasToolTips)
					m_AutoDlg->EnableToolTips(TRUE);
			}
			m_AutoDlg->SetInfoPtr(&m_Info);
			// position auto dialog relative to slider
			CRect	sr, dr;		// slider rect, dialog rect
			GetWindowRect(sr);
			m_AutoDlg->GetWindowRect(dr);
			CPoint	pos;
			// if auto dialog has an adopted parent
			if (m_AutoDlgParent != NULL) {
				m_AutoDlgParent->ScreenToClient(sr);	// convert to parent coords
				// move dialog's top left corner to slider's bottom left corner
				pos = CPoint(sr.left, sr.bottom);
				CRect	pr;		// parent rect
				m_AutoDlgParent->GetClientRect(pr);
				// if dialog clips parent window's bottom edge
				if (sr.bottom + dr.Height() > pr.bottom) {
					pos.y -= dr.Height() + sr.Height();		// move it above slider
					if (pos.y < pr.top)		// if dialog now clips parent top edge
						pos.y = sr.top;		// move it over slider
				}
			} else
				pos = CPoint(sr.left, sr.bottom);	// assume dialog is top-level
			m_AutoDlg->MoveWindow(pos.x, pos.y, dr.Width(), dr.Height());
			m_AutoDlg->ShowWindow(SW_SHOWNORMAL);	// make dialog visible
			// must set parent again; otherwise if RestoreState calls us during
			// redo, and parent was deleted and recreated, dialog becomes modal
			if (m_AutoDlgParent != NULL)
				m_AutoDlg->SetParent(m_AutoDlgParent);
		}
	} else {
		if (m_AutoDlg != NULL) {
			Notify(AUTOSLIDER_CLOSE);
			if (Undoable && !IsUndoInProgress())
				NotifyUndoableEdit(UNDO_SHOW_DLG, UE_INSIGNIFICANT);
			delete m_AutoDlg;
			m_AutoDlg = NULL;
		}
	}
}

bool CAutoSliderCtrl::TimerHook(float Scale)
{
	bool	retc = CAutoDlg::TimerHook(m_Info, m_AutoDlg, Scale);
	if (retc) {	// if selection was valid
		switch (m_Info.m_Waveform) {
		case CAutoInfo::TRIANGLE:	// most likely case so test for it first
			m_WavePos = m_Info.m_Pos;
			break;
		case CAutoInfo::SQUARE:
			m_WavePos = m_Info.m_Reverse ? m_Info.GetEnd() : m_Info.GetStart();
			break;
		case CAutoInfo::RANDOM:
			if (m_Info.AtStart() || m_Info.AtEnd())
				m_WavePos = m_Info.GetStart() + m_Info.GetLength() * rand() / RAND_MAX;
			break;
		default:
			m_WavePos = m_Info.m_Pos;
		}
		// SetPos paints the slider, causing major overhead; animation delay 
		// trades smoothness for performance by periodically skipping SetPos
		if (IsAnimated())	// if non-zero animation delay, maybe skip this tic
			CSelectSliderCtrl::SetNormPos(m_WavePos);	// set slider position
		OnNewPos();
	} else
		m_WavePos = m_Info.m_Pos;
	if (m_Info.m_Transport != m_PrevTransport) {
		if (GetAnimDelay())	// if we were skipping tics, must set final position
			CSelectSliderCtrl::SetNormPos(m_Info.m_Pos);
		if (m_Info.m_Transport != PLAY)
			ShowAutoDlg(FALSE, FALSE);	// close without undo notification
		Redraw();	// force update of transport indicator
		m_PrevTransport = m_Info.m_Transport;
	}
	return(retc);
}

void CAutoSliderCtrl::Notify(int NotificationCode)
{
	CWnd	*Parent = GetParent();
	if (Parent != NULL) {
		NMHDR	nmh;
		nmh.hwndFrom = m_hWnd;
		nmh.idFrom = GetDlgCtrlID();
		nmh.code = NotificationCode;
		Parent->SendMessage(WM_NOTIFY, nmh.idFrom, long(&nmh));
	}
}

void CAutoSliderCtrl::ToggleAutoDlg()
{
	ShowAutoDlg(!IsAutoDlgVisible());
}

void CAutoSliderCtrl::SetAutoDlgParent(CWnd *Parent)
{
//	m_AutoDlgParent = Parent;	// disable child dialogs
}

void CAutoSliderCtrl::SetAutoDlgTitle(LPCSTR Title)
{
	if (m_AutoDlg != NULL)
		m_AutoDlg->SetWindowText(Title);
}

void CAutoSliderCtrl::UpdateAutoDlgUI()
{
	if (m_AutoDlg != NULL)
		m_AutoDlg->UpdateUI();
}

void CAutoSliderCtrl::OnSelection()
{
}

void CAutoSliderCtrl::OnTransport(int Transport)
{
	if (Transport == PLAY) {
		if (m_Info.m_Transport == STOP || m_Info.AtStart() || m_Info.AtEnd())
			m_Info.SetCourse();	// clamp initial position and set direction
	}
}

void CAutoSliderCtrl::OnNewPos()
{
}

BOOL CAutoSliderCtrl::EnableToolTips(BOOL bEnable)
{
	m_HasToolTips = bEnable;
	if (m_AutoDlg != NULL)
		m_AutoDlg->EnableToolTips(bEnable);
	return(bEnable);
}

void CAutoSliderCtrl::AdjSel(int Func)
{
	if (!HasSelection())	// control key down but no selection
		return;
	NotifyUndoableEdit(UNDO_SELECTION, TRUE);	// coalesce
	if (Func & AF_GOTO) {
		float	Len = m_Info.m_EndPos - m_Info.m_StartPos;
		if (Func & AF_START) {
			m_Info.m_StartPos = 0;
			m_Info.m_EndPos = Len;
		} else {
			m_Info.m_StartPos = 1 - Len;
			m_Info.m_EndPos = 1;
		}
	} else {
		float	Delta = 1.0f / (m_RangeMax - m_RangeMin);
		if (IsKeyDown(VK_SHIFT))
			Delta /= m_FineAdjustPrecision;
		float	MinLength = Delta;
		if (Func & AF_DEC)
			Delta = -Delta;
		switch (Func & AF_BOTH) {
		case AF_START:
			m_Info.m_StartPos = CLAMP(min(m_Info.m_StartPos + Delta, 
				m_Info.m_EndPos - MinLength), 0, 1);
			break;
		case AF_END:
			m_Info.m_EndPos = CLAMP(max(m_Info.m_StartPos + MinLength, 
				m_Info.m_EndPos + Delta), 0, 1);
			break;
		case AF_BOTH:
			if (Delta > 0)
				Delta = min(m_Info.m_EndPos + Delta, 1) - m_Info.m_EndPos;
			else
				Delta = max(m_Info.m_StartPos + Delta, 0) - m_Info.m_StartPos;
			m_Info.m_StartPos += Delta;
			m_Info.m_EndPos += Delta;
			break;
		}
	}
	SetNormSelection(m_Info.m_StartPos, m_Info.m_EndPos);
}

void CAutoSliderCtrl::AdjPos(int Func)
{
	NotifyUndoableEdit(UNDO_POSITION, TRUE);	// coalesce
	float	Delta = 1.0f / (m_RangeMax - m_RangeMin);
	if (IsKeyDown(VK_SHIFT))
		Delta /= m_FineAdjustPrecision;
	if (Func & AF_DEC)
		Delta = -Delta;
	m_Info.m_Pos = CLAMP(float(m_Info.m_Pos) + Delta, 0, 1);
	SetNormPos(m_Info.m_Pos);
}

void CAutoSliderCtrl::SaveUndoState(CUndoState& State)
{
	switch (State.GetCode()) {
	case UNDO_POSITION:
		State.m_Val.x.f = float(m_Info.m_Pos);
		break;
	case UNDO_SELECTION:
		State.m_Val.x.f = m_Info.m_StartPos;
		State.m_Val.y.f = m_Info.m_EndPos;
		break;
	case UNDO_SHOW_DLG:
		State.m_Val.x.b = IsAutoDlgVisible();
		break;
	case UNDO_TRANSPORT:
		State.m_Val.x.i = m_Info.m_Transport;
		break;
	case UNDO_QUICK_START:
		{
			QUICK_START_UNDO_STATE	us;
			us.Pos = float(m_Info.m_Pos);
			us.Start = m_Info.m_StartPos;
			us.End = m_Info.m_EndPos;
			us.Transport = m_Info.m_Transport;
			us.Reverse = m_Info.m_Reverse;
			us.ShowDlg = m_AutoDlg != NULL;
			State.SetData(&us, sizeof(QUICK_START_UNDO_STATE));
			OnDlgSaveUndoState(State);
		}
		break;
	default:
		NODEFAULTCASE;
	}
}

void CAutoSliderCtrl::RestoreUndoState(const CUndoState& State)
{
	switch (State.GetCode()) {
	case UNDO_POSITION:
		SetNormPos(State.m_Val.x.f);
		break;
	case UNDO_SELECTION:
		SetNormSelection(State.m_Val.x.f, State.m_Val.y.f);
		break;
	case UNDO_SHOW_DLG:
		ShowAutoDlg(State.m_Val.x.b);
		break;
	case UNDO_TRANSPORT:
		SetTransport(State.m_Val.x.i);
		break;
	case UNDO_QUICK_START:
		{
			QUICK_START_UNDO_STATE	*usp = 
				(QUICK_START_UNDO_STATE *)State.GetData();
			SetNormPos(usp->Pos);
			SetNormSelection(usp->Start, usp->End);
			SetTransport(usp->Transport);
			ShowAutoDlg(usp->ShowDlg, FALSE);	// no undo notification
			m_Info.m_Reverse = usp->Reverse;
			OnDlgRestoreUndoState(State);
		}
		break;
	default:
		NODEFAULTCASE;
	}
	Notify(AUTOSLIDER_ACTIVATE);
}

CUndoable *CAutoSliderCtrl::FindUndoable(CUndoState::UNDO_KEY Key)
{
	switch (Key) {
	case IDD_AUTO_DLG:
		// in case dialog got closed by timer hook, force it to exist
		if (!IsAutoDlgVisible())
			ShowAutoDlg(TRUE, FALSE);	// open without undo notification
		return(m_AutoDlg);
	default:
		return(NULL);
	}
}

void CAutoSliderCtrl::GetUndoTitle(const CUndoState& State, CString& Title)
{
	CString	Text;
	GetWindowText(Text);
	switch (State.GetCode()) {
	case UNDO_POSITION:
		Title = Text;
		break;
	case UNDO_SELECTION:
		Title.Format(LDS(AUTO_SELECTION), Text);
		break;
	case UNDO_SHOW_DLG:
		Title = LDS(AUTO_DIALOG);
		break;
	case UNDO_TRANSPORT:
		Title = LDS(AUTO_TRANSPORT);
		break;
	case UNDO_QUICK_START:
		Title = LDS(AUTO_QUICK_START);
		break;
	default:
		Title.Empty();
	}
}

void CAutoSliderCtrl::OnDlgSaveUndoState(CUndoState& State)
{
}

void CAutoSliderCtrl::OnDlgRestoreUndoState(const CUndoState& State)
{
}

/////////////////////////////////////////////////////////////////////////////
// CAutoSliderCtrl::CMyAutoDlg overrides

CAutoSliderCtrl::CMyAutoDlg::CMyAutoDlg(CAutoSliderCtrl *Slider, CWnd *pParent) :
	m_Slider(Slider), CAutoDlg(CAutoDlg::IDD, pParent)
{
}

BOOL CAutoSliderCtrl::CMyAutoDlg::OnInitDialog()
{
	BOOL	retc = CAutoDlg::OnInitDialog();
	m_Waveform.EnableWindow(m_Slider->m_AutoStyle & AUTO_DLG_WAVEFORM);
	m_Loop.EnableWindow(m_Slider->m_AutoStyle & AUTO_DLG_LOOP);
	return(retc);
}

void CAutoSliderCtrl::CMyAutoDlg::OnTransport(int Transport)
{
	ASSERT(Transport >= 0 && Transport < TRANSPORT_STATES);
	m_Slider->Notify(m_TransportNotification[Transport]);
	m_Slider->OnTransport(Transport);
	if (Transport == PLAY && !IsUndoInProgress())
		PostMessage(WM_CLOSE);	// close automation dialog
	m_Slider->Redraw();	// force update of transport indicator
}

void CAutoSliderCtrl::CMyAutoDlg::OnGo()
{
	m_Slider->Notify(AUTOSLIDER_GO);
}

void CAutoSliderCtrl::CMyAutoDlg::OnOK()
{
	m_Slider->ShowAutoDlg(FALSE);
}

void CAutoSliderCtrl::CMyAutoDlg::OnCancel()
{
	m_Slider->ShowAutoDlg(FALSE);
}

BOOL CAutoSliderCtrl::CMyAutoDlg::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message) {
	case WM_CLOSE:
	case WM_NCMBUTTONDOWN:
	case WM_MBUTTONDOWN:
		m_Slider->ShowAutoDlg(FALSE);
		return(TRUE);
	}
	return(CAutoDlg::PreTranslateMessage(pMsg));
}

BOOL CAutoSliderCtrl::CMyAutoDlg::OnNcActivate(BOOL bActive)
{
	if (bActive)
		m_Slider->Notify(AUTOSLIDER_ACTIVATE);
	return(CAutoDlg::OnNcActivate(bActive));
}

void CAutoSliderCtrl::CMyAutoDlg::OnModify()
{
	m_Slider->Notify(AUTOSLIDER_MODIFY);
}

void CAutoSliderCtrl::CMyAutoDlg::OnNewPos()
{
	m_Slider->SetNormPos(m_ip->m_Pos);
	m_Slider->OnNewPos();
}

void CAutoSliderCtrl::CMyAutoDlg::SaveUndoState(CUndoState& State)
{
	m_Slider->OnDlgSaveUndoState(State);
	CAutoDlg::SaveUndoState(State);
}

void CAutoSliderCtrl::CMyAutoDlg::RestoreUndoState(const CUndoState& State)
{
	m_Slider->OnDlgRestoreUndoState(State);
	CAutoDlg::RestoreUndoState(State);
}

BEGIN_MESSAGE_MAP(CAutoSliderCtrl, CSelectSliderCtrl)
	//{{AFX_MSG_MAP(CAutoSliderCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_MBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomdraw)
	ON_COMMAND(ID_TOGGLE_AUTO_DLG, OnToggleAutoDlg)
	ON_WM_HSCROLL_REFLECT()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoSliderCtrl message handlers

void CAutoSliderCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CSelectSliderCtrl::OnRButtonUp(nFlags, point);	// end modal state
	if (nFlags & MKU_SHOW_DLG) {
		ToggleAutoDlg();
	} else {
		// only allow quick start behavior if automation time is non-zero
		if (IsKeyDown(VK_MENU) && m_Info.m_Time) {
			// if click was outside thumb, do quick start
			if (!m_ThumbRect.PtInRect(point)) {	
				SetFocus();	// because we don't call base class
				int	Start = GetPos();
				int	End = PointToPos(point);
				if (Start != End) {
					if (Start > End) {	// sort selection endpoints
						int	tmp = Start;
						Start = End;
						End = tmp;
					}
					NotifyUndoableEdit(UNDO_QUICK_START);
					SetTransport(STOP);		// stop first so direction gets set
					SetSelection(Start, End);
					UpdateSelection(SCN_BOTH);
					SetTransport(PLAY);
					Notify(AUTOSLIDER_PLAY);
					ShowAutoDlg(FALSE, FALSE);	// close, no undo notification
				}
			} else
				CSelectSliderCtrl::OnLButtonDown(nFlags, point);
		} else
			CSelectSliderCtrl::OnLButtonDown(nFlags, point);
	}
}

void CAutoSliderCtrl::OnMButtonDown(UINT nFlags, CPoint point) 
{
	CSelectSliderCtrl::OnRButtonUp(nFlags, point);	// end modal state
	if (m_Options & MCLICK_SHOWS_DLG) {
		// call base class first or dialog won't get focus
		CSliderCtrl::OnMButtonDown(nFlags, point);
		ToggleAutoDlg();
	}
}

void CAutoSliderCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CSelectSliderCtrl::OnRButtonDown(nFlags, point);
	if (!HasSelection())
		m_Info.ClearSel();
}

void CAutoSliderCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	CSelectSliderCtrl::OnRButtonUp(nFlags, point);
}

void CAutoSliderCtrl::OnCustomdraw(NMHDR *pNMHDR, LRESULT *pResult) 
{
	NMCUSTOMDRAW	*pNMCD = (NMCUSTOMDRAW *)pNMHDR;
	switch (pNMCD->dwDrawStage) {
	case CDDS_ITEMPREPAINT:
		if (pNMCD->dwItemSpec == TBCD_THUMB)
			*pResult = CDRF_NOTIFYPOSTPAINT;
		break;
	case CDDS_ITEMPOSTPAINT:
		if (pNMCD->dwItemSpec == TBCD_THUMB) {
			// draw transport indicator on trackbar's thumb
			static	const	IndRad = 3;		// indicator radius, in logical coordinates
			static	const	CPoint	IndPos(0, -1);	// indicator offset from thumb center
			CRect	ir;		// indicator rect
			ir = CRect(m_ThumbRect.CenterPoint(), m_ThumbRect.CenterPoint());
			ir.OffsetRect(IndPos);
			ir.InflateRect(IndRad, IndRad);
			const	CBrush	*pBr;
			if (IsWindowEnabled())
				pBr = m_BrTransport[GetTransport()];
			else {
				pBr = &m_BrChannel;
				SelectObject(pNMCD->hdc, m_PenDisabled);
			}
			SelectObject(pNMCD->hdc, *pBr);
			RoundRect(pNMCD->hdc, ir.left, ir.top, ir.right, ir.bottom, IndRad, IndRad);
		}
		break;
	}
	CSelectSliderCtrl::OnCustomdraw(pNMHDR, pResult);
}

BOOL CAutoSliderCtrl::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST) {
		if (TranslateAccelerator(m_hWnd, m_Accel, pMsg))
			return(TRUE);
	}
	return CSliderCtrl::PreTranslateMessage(pMsg);
}

void CAutoSliderCtrl::OnToggleAutoDlg() 
{
	ToggleAutoDlg();
}

void CAutoSliderCtrl::HScroll(UINT nSBCode, UINT nPos)
{
	// make only; eliminate button up and key up
	switch (nSBCode) {
	case SB_ENDSCROLL:
	case SB_THUMBPOSITION:
		break;
	case SB_SLIDER_SELECTION:
		float	Start, End, Len;
		CSelectSliderCtrl::GetNormSelection(Start, End);
		switch (nPos) {	// selection type
		case SCN_BOTH:
			m_Info.m_StartPos = Start;
			m_Info.m_EndPos = End;
			break;
		case SCN_START:
			m_Info.m_StartPos = Start;
			break;
		case SCN_END:
			m_Info.m_EndPos = End;
			break;
		case SCN_MOVE:
			Len = m_Info.m_EndPos - m_Info.m_StartPos;
			if (Start + Len > 1)
				Start = 1 - Len;	// clamp without changing length
			m_Info.m_StartPos = Start;
			m_Info.m_EndPos = Start + Len;
			break;
		}
		OnSelection();
		break;
	default:
		m_Info.m_Pos = CSelectSliderCtrl::GetNormPos();
		OnNewPos();
	}	
}

void CAutoSliderCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (IsKeyDown(VK_CONTROL)) {
		switch (nChar) {
		case VK_UP:
			AdjSel(AF_END | AF_INC);
			break;
		case VK_DOWN:
			AdjSel(AF_END | AF_DEC);
			break;
		case VK_LEFT:
			AdjSel(AF_START | AF_DEC);
			break;
		case VK_RIGHT:
			AdjSel(AF_START | AF_INC);
			break;
		case VK_PRIOR:	// page up
			AdjSel(AF_BOTH | AF_INC);
			break;
		case VK_NEXT:	// page down
			AdjSel(AF_BOTH | AF_DEC);
			break;
		case VK_HOME:
			AdjSel(AF_GOTO | AF_START);
			break;
		case VK_END:
			AdjSel(AF_GOTO | AF_END);
			break;
		default:
			CSelectSliderCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
		}
	} else {
		switch (nChar) {
		case VK_UP:
		case VK_RIGHT:
			AdjPos(AF_INC);
			break;
		case VK_LEFT:
		case VK_DOWN:
			AdjPos(AF_DEC);
			break;
		default:
			CSelectSliderCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
		}
	}
}
