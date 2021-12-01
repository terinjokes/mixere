// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      21nov03 initial version
		01		25dec03	allow timerhook to update dialog's tranport buttons
		02		29dec03	add clamp time, close handler
		03		02jan04	move selection handling into timer hook
		04		08jan04	add middle mouse button handler
		05		16jan04	play closes dialog now
		06		19jan04	add SetTimerPeriod
		07		21jan04	add accelerator keys
		08		23jan04	move accelerator keys to derived PreTranslateMessage
		09		24jan04	add OnModify
		10		25jan04	don't handle key down, use accelerator table
		11		26jan04	add dialog template argument to ctor for subclassing
		12		27jan04	allow enter key to close dialog
		13		03feb04	add floating-point compare with tolerance
		14		17feb04	disable accelerators while edit control has focus
		15		25feb04	add undo
		16		10mar04	add scale to timer hook
		17		22mar04	allow automation without selection; use full range
		18		15apr04	add CenterWindowEx
		19		16apr04	add UpdateUI and SetInfo
		20		19apr04	allow derived classes to load their own accelerators
		21		28sep04	time edit moves to its own class
		22		08nov04	add virtual OnNcActivate handler, was in base class
		23		19nov04	make sure scroll message is from us, not derived class
		24		31dec04	make waveform variable 
		25		09jan05	add go

		automation dialog
 
*/

// AutoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "AutoDlg.h"
#include "AutoInfo.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAutoDlg dialog

int CAutoDlg::m_TimerPeriod;	// in milliseconds

const int CAutoDlg::m_WaveNameID[CAutoInfo::WAVEFORMS] = {
	IDS_AUTO_WAVE_TRIANGLE,
	IDS_AUTO_WAVE_RAMP_UP,
	IDS_AUTO_WAVE_RAMP_DOWN,
	IDS_AUTO_WAVE_SQUARE,
	IDS_AUTO_WAVE_RANDOM
};

CAutoDlg::CAutoDlg(UINT nIDTemplate, CWnd* pParent)
	: CDialogEx(nIDTemplate ? nIDTemplate : CAutoDlg::IDD, IDR_AUTO_DLG, pParent)
{
	//{{AFX_DATA_INIT(CAutoDlg)
	//}}AFX_DATA_INIT
	m_ip = &m_DefaultInfo;
}

CAutoDlg::~CAutoDlg()
{
	DestroyWindow();
}

inline int CAutoDlg::DblCmp(double a, double b)
{
	static const double Tolerance = 1e-12;	// double precision is 15 digits
	if (fabs(a - b) < Tolerance)
		return(0);
	return(a < b ? -1 : 1);
}

bool CAutoDlg::TimerHook(CAutoInfo& Info, CAutoDlg *AutoDlg, float Scale)
{
	if (Info.m_Transport != PLAY)
		return(FALSE);
	float	Start = Info.GetStart();
	float	End = Info.GetEnd();
	double	Len = End - Start;
	if (Len <= 0)
		return(FALSE);
	double	delta = m_TimerPeriod / double(max(Info.m_Time, 1)) * Scale * Len;
	if (Info.m_Reverse)
		delta = -delta;
	Info.m_Pos += delta;
	bool	AtStart = DblCmp(Info.m_Pos, Start) <= 0;
	bool	AtEnd = DblCmp(Info.m_Pos, End) >= 0;
	if (AtStart || AtEnd) {
		switch (Info.m_Waveform) {
		case CAutoInfo::RAMP_UP:
			if (Info.m_Loop) {
				Info.m_Pos = Start;
				Info.m_Reverse = 0;
			}
			break;
		case CAutoInfo::RAMP_DOWN:
			if (Info.m_Loop) {
				Info.m_Pos = End;
				Info.m_Reverse = 1;
			}
			break;
		default:
			if (AtStart) {
				Info.m_Pos = Start;
				Info.m_Reverse = 0;
			} else {
				Info.m_Pos = End;
				Info.m_Reverse = 1;
			}
		}
		if (!Info.m_Loop) {
			Info.m_Transport = STOP;
			// if dialog is visible, update its transport buttons
			if (AutoDlg != NULL && AutoDlg->IsWindowVisible())
				AutoDlg->UpdateUI();
		}
	}
	return(TRUE);
}

void CAutoDlg::SetTimerPeriod(int Period)
{
	m_TimerPeriod = Period;
}

void CAutoDlg::SetTransport(int Transport)
{
	m_Play.SetCheck(Transport == PLAY);
	m_Pause.SetCheck(Transport == PAUSE);
	m_Stop.SetCheck(Transport == STOP);
	if (Transport != m_ip->m_Transport)
		OnTransport(Transport);
	m_ip->m_Transport = Transport;
}

bool CAutoDlg::TogglePlay()
{
	SetTransport(m_ip->m_Transport == PLAY ? PAUSE : PLAY);
	return(m_ip->m_Transport == PLAY);
}

void CAutoDlg::UpdateUI()
{
	m_TimeEdit.SetInfo(*m_ip);
	m_TimeSlider.SetPos(m_TimeEdit.GetPos());
	m_Minutes.SetCheck(m_ip->m_TimeUnit);
	m_Loop.SetCheck(m_ip->m_Loop);
	SetTransport(m_ip->m_Transport);
	m_Waveform.SetCurSel(m_ip->m_Waveform);
}

void CAutoDlg::UpdateTime()
{
	m_ip->m_Time = m_TimeEdit.GetTime();
	m_ip->m_TimeUnit = m_TimeEdit.GetUnit();
	OnTimeChange();
	OnModify();
}

void CAutoDlg::SetInfoPtr(CAutoInfo *Info)
{
	m_ip = Info;
	UpdateUI();
}

void CAutoDlg::SetInfo(const CAutoInfo& Info)
{
	*m_ip = Info;
	UpdateUI();
}

void CAutoDlg::SetParent(CWnd *Parent)
{
	m_Parent = Parent;
	CDialogEx::SetParent(Parent);
}

void CAutoDlg::SaveUndoState(CUndoState& State)
{
	// If the undo notification came from this instance, GetCtrlID returns
	// IDD_AUTO_DLG and GetCode returns the control ID; otherwise we assume it
	// came from one of our child controls with custom undo handling, in which 
	// case GetCtrlID returns the ID and GetCode is reserved for the control.
	switch (State.GetCode()) {
	case IDC_AUTO_PLAY:
	case IDC_AUTO_PAUSE:
	case IDC_AUTO_STOP:
	case ID_AUTO_TOGGLE_PLAY:		// menu command
		State.m_Val.x.i = m_ip->m_Transport;
		State.m_Val.y.f = (float)m_ip->m_Pos;
		break;
	case ID_AUTO_TOGGLE_LOOP:		// menu command
		WndToUndo(&m_Loop)->SaveUndoState(State);
		break;
	case ID_AUTO_TOGGLE_MINUTES:	// menu command
		WndToUndo(&m_Minutes)->SaveUndoState(State);
		break;
	case IDC_AUTO_WAVEFORM:
		State.m_Val.x.i = m_ip->m_Waveform;
		break;
	default:	// assume notification came from child control
		switch (State.GetCtrlID()) {
		case IDC_AUTO_TIME_POS:		// slider position isn't precise enough
			State.m_Val.x.i = m_ip->m_Time;
			break;
		case IDC_AUTO_MINUTES:		// minutes changes time as side effect
			WndToUndo(&m_Minutes)->SaveUndoState(State);
			State.m_Val.y.i = m_ip->m_Time;
			break;
		default:
			NODEFAULTCASE;
		}
	}
}

void CAutoDlg::RestoreUndoState(const CUndoState& State)
{
	switch (State.GetCode()) {
	case IDC_AUTO_PLAY:
	case IDC_AUTO_PAUSE:
	case IDC_AUTO_STOP:
	case ID_AUTO_TOGGLE_PLAY:		// menu command
		SetTransport(State.m_Val.x.i);
		m_ip->m_Pos = State.m_Val.y.f;
		OnNewPos();
		break;
	case ID_AUTO_TOGGLE_LOOP:		// menu command
		WndToUndo(&m_Loop)->RestoreUndoState(State);
		break;
	case ID_AUTO_TOGGLE_MINUTES:	// menu command
		WndToUndo(&m_Minutes)->RestoreUndoState(State);
		UpdateUI();
		break;
	case IDC_AUTO_WAVEFORM:
		m_ip->m_Waveform = State.m_Val.x.i;
		m_Waveform.SetCurSel(State.m_Val.x.i);
		break;
	default:	// assume notification came from child control
		switch (State.GetCtrlID()) {
		case IDC_AUTO_TIME_POS:
			m_ip->m_Time = State.m_Val.x.i;
			UpdateUI();
			break;
		case IDC_AUTO_MINUTES:
			WndToUndo(&m_Minutes)->RestoreUndoState(State);
			m_ip->m_Time = State.m_Val.y.i;
			UpdateUI();
			break;
		default:
			NODEFAULTCASE;
		}
	}
}

void CAutoDlg::GetUndoTitle(const CUndoState& State, CString& Title)
{
	switch (State.GetCode()) {	// check for menu commands
	case ID_AUTO_TOGGLE_PLAY:
		m_Play.GetWindowText(Title);
		break;
	case ID_AUTO_TOGGLE_LOOP:
		m_Loop.GetWindowText(Title);
		break;
	case ID_AUTO_TOGGLE_MINUTES:
		m_Minutes.GetWindowText(Title);
		break;
	case IDC_AUTO_WAVEFORM:
		Title = LDS(AUTO_WAVEFORM);
		break;
	default:
		if (State.GetCtrlID() == IDD) {	// notification came from us
			CWnd	*p = GetDlgItem(State.GetCode());
			if (p != NULL)
				p->GetWindowText(Title);
			else
				Title.Empty();
		} else {	// notification came from child control
			CUndoable	*uap = FindUndoable(State.GetCtrlID());
			if (uap != NULL)
				uap->GetUndoTitle(State, Title);
		}
	}
}

CUndoState::UNDO_KEY CAutoDlg::GetUndoKey()
{
	return(IDD_AUTO_DLG);
}

/////////////////////////////////////////////////////////////////////////////
// CAutoDlg overridables

void CAutoDlg::OnTransport(int Transport)
{
}

void CAutoDlg::OnGo()
{
}

void CAutoDlg::OnModify()
{
}

void CAutoDlg::OnTimeChange()
{
}

void CAutoDlg::OnNewPos()
{
}

void CAutoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutoDlg)
	DDX_Control(pDX, IDC_AUTO_MINUTES, m_Minutes);
	DDX_Control(pDX, IDC_AUTO_TIME_POS, m_TimeSlider);
	DDX_Control(pDX, IDC_AUTO_PLAY, m_Play);
	DDX_Control(pDX, IDC_AUTO_STOP, m_Stop);
	DDX_Control(pDX, IDC_AUTO_PAUSE, m_Pause);
	DDX_Control(pDX, IDC_AUTO_LOOP, m_Loop);
	DDX_Control(pDX, IDC_AUTO_TIME_EDIT, m_TimeEdit);
	DDX_Control(pDX, IDC_AUTO_WAVEFORM, m_Waveform);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAutoDlg, CDialogEx)
	//{{AFX_MSG_MAP(CAutoDlg)
	ON_WM_HSCROLL()
	ON_NOTIFY(UDN_DELTAPOS, IDC_AUTO_TIME_SPIN, OnDeltaposAutoTimeSpin)
	ON_BN_CLICKED(IDC_AUTO_PAUSE, OnPause)
	ON_BN_CLICKED(IDC_AUTO_PLAY, OnPlay)
	ON_BN_CLICKED(IDC_AUTO_STOP, OnStop)
	ON_BN_CLICKED(IDC_AUTO_MINUTES, OnMinutes)
	ON_BN_CLICKED(IDC_AUTO_LOOP, OnLoop)
	ON_COMMAND(ID_TOGGLE_AUTO_DLG, OnToggleAutoDlg)
	ON_COMMAND(ID_AUTO_TOGGLE_PLAY, OnTogglePlay)
	ON_COMMAND(ID_AUTO_TOGGLE_LOOP, OnToggleLoop)
	ON_COMMAND(ID_AUTO_TOGGLE_MINUTES, OnToggleMinutes)
	ON_WM_NCACTIVATE()
	ON_COMMAND(ID_AUTO_PLAY, OnPlay)
	ON_COMMAND(ID_AUTO_PAUSE, OnPause)
	ON_COMMAND(ID_AUTO_STOP, OnStop)
	ON_CBN_SELCHANGE(IDC_AUTO_WAVEFORM, OnSelchangeAutoWaveform)
	//}}AFX_MSG_MAP
	ON_WM_NCACTIVATE()	// declared virtual
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoDlg message handlers

BOOL CAutoDlg::OnInitDialog() 
{
	CDialogEx::OnInitDialog();
	// load the button icons	
	m_Play.SetIcons(IDI_PLAYU, IDI_PLAYD);
	m_Pause.SetIcons(IDI_PAUSEU, IDI_PAUSED);
	m_Stop.SetIcons(IDI_STOPU, IDI_STOPD);
	m_Loop.SetIcons(IDI_LOOPU, IDI_LOOPD);
	m_Minutes.SetIcons(IDI_MINUTESU, IDI_MINUTESD);
	// give edit control a fatter font
	m_TimeEdit.SendMessage(WM_SETFONT, (WPARAM)GetStockObject(SYSTEM_FONT));
	m_TimeEdit.SetUndoTitle(LDS(AUTO_TIME_EDIT));
	// set slider range and value
	m_TimeSlider.SetRange(0, 60);
	// init controls from parent's info
	SetInfoPtr(&m_DefaultInfo);
	// create additional slider tics
	int i;
	for (i = 0; i <= 12; i++)
		m_TimeSlider.SetTic(i * 5);
	// prevent transport controls from sending undo notifications, because 
	// they're grouped; we'll send notifications from our clicked handlers
	m_Play.SetUndoHandler(NULL);	
	m_Pause.SetUndoHandler(NULL);
	m_Stop.SetUndoHandler(NULL);
	// these controls send undo notifications, but we handle save/restore
	m_TimeSlider.SetUndoHandler(this);
	m_Minutes.SetUndoHandler(this);
	for (i = 0; i < CAutoInfo::WAVEFORMS; i++) {
		CString	s;
		s.LoadString(m_WaveNameID[i]);
		m_Waveform.AddString(s);
	}
	return(TRUE);
}

void CAutoDlg::OnPlay() 
{
	if (m_ip->m_Transport != PLAY)	// prevent duplicate notifications
		NotifyUndoableEdit(IDC_AUTO_PLAY);
	SetTransport(PLAY);
	OnModify();
}

void CAutoDlg::OnPause() 
{
	if (m_ip->m_Transport != PAUSE)	// prevent duplicate notifications
		NotifyUndoableEdit(IDC_AUTO_PAUSE);
	SetTransport(PAUSE);
	OnModify();
}

void CAutoDlg::OnStop() 
{
	if (m_ip->m_Transport != STOP)	// prevent duplicate notifications
		NotifyUndoableEdit(IDC_AUTO_STOP);
	SetTransport(STOP);
	OnModify();
}

void CAutoDlg::OnLoop() 
{
	m_ip->m_Loop = m_Loop.GetCheck();
	OnModify();
}

void CAutoDlg::OnMinutes() 
{
	m_TimeEdit.SetUnit(m_Minutes.GetCheck());	// update time edit
	UpdateTime();
}

void CAutoDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
	CWnd	*wp = pScrollBar;	// pScrollBar could be a derived class control;
	if (wp == &m_TimeSlider) {	// make sure message came from our time slider
		m_TimeEdit.SetPos(m_TimeSlider.GetPos());	// update time edit
		UpdateTime();
	}
}

BOOL CAutoDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	NMHDR	*nmh = (NMHDR *)lParam;
	if (nmh->hwndFrom == m_TimeEdit.m_hWnd) {	// if time edit notification
		m_TimeSlider.SetPos(m_TimeEdit.GetPos());	// update slider
		m_Minutes.SetCheck(m_TimeEdit.GetUnit());	// update minutes button
		UpdateTime();
		return(TRUE);
	}
	return CDialogEx::OnNotify(wParam, lParam, pResult);
}

void CAutoDlg::OnDeltaposAutoTimeSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	m_TimeEdit.AddSpin(-pNMUpDown->iDelta);
	*pResult = 0;
}

BOOL CAutoDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST) {
		// if edit control has focus, don't translate accelerators
		CWnd	*fw = GetFocus();
		if (fw != NULL && fw->IsKindOf(RUNTIME_CLASS(CEdit)))
			// call CDialog directly to implement standard dialog keys;
			// can't call CDialogEx because it translates accelerators
			return(CDialog::PreTranslateMessage(pMsg));
		if (TranslateAccelerator(m_hWnd, m_Accel, pMsg))
			return(TRUE);
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CAutoDlg::OnToggleAutoDlg() 
{
	OnCancel();
}

void CAutoDlg::OnTogglePlay() 
{
	NotifyUndoableEdit(ID_AUTO_TOGGLE_PLAY);
	TogglePlay();
}

void CAutoDlg::OnToggleLoop() 
{
	NotifyUndoableEdit(ID_AUTO_TOGGLE_LOOP);
	m_Loop.SetCheck(!m_Loop.GetCheck());
	OnLoop();
}

void CAutoDlg::OnToggleMinutes() 
{
	NotifyUndoableEdit(ID_AUTO_TOGGLE_MINUTES);
	m_Minutes.SetCheck(!m_Minutes.GetCheck());
	OnMinutes();
}

void CAutoDlg::OnSelchangeAutoWaveform() 
{
	NotifyUndoableEdit(IDC_AUTO_WAVEFORM, UE_COALESCE);
	m_ip->m_Waveform = m_Waveform.GetCurSel();
	OnModify();
}

BOOL CAutoDlg::OnNcActivate(BOOL bActive)
{
	return(CDialog::OnNcActivate(bActive));
}
