// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      27jan04	initial version
		01		29jan04	handle accelerator's auto toggle loop msg
		02		01feb04	fix arrows and page up/down
		03		01feb04	set ctrl-click default same as auto slider's
		04		03feb04	don't set auto transport for auto trigger channels
		05		15feb04	add more customized behavior for auto trigger
		06		17feb04	auto pos accelerator wasn't toggling dialog
		07		10mar04	if single channel, use its title, not its file name
		08		11mar04	playing trigger now forces automation looping
		09		28sep04	must call UpdateUI instead of UpdateSlider
		10		29sep04	for fine adjust, use CAutoSliderCtrl with dlg disabled
		11		14dec04	zero slider time to disable quick start
		12		22dec04	add edit boxes for position and selection start/end
		13		24dec04	hook timer to update UI when transport states change
		14		31dec04	make waveform variable 
		15		03jan05	disable waveform combo box for auto triggers
		16		09jan05	add go
		17		03feb05	add IsModified
		18		24feb05	for auto-trigger, play forces audio looping on
		19		25feb05	add option for auto-trigger turns off audio looping
		20		27feb05	OnNewPos must use m_Info.m_Pos instead of GetNormPos

		multi-channel automation dialog
 
*/

// MultiAutoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "MultiAutoDlg.h"
#include "MixereView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMultiAutoDlg dialog

HACCEL CMultiAutoDlg::m_Accel;

const float	CMultiAutoDlg::m_PosSpinAmt = .001f;
const int	CMultiAutoDlg::m_PosPrecision = 3;

CMultiAutoDlg::CMultiAutoDlg(CMixereView& Mixer, int AutoIdx, CWnd* pParent)
	: m_Mixer(Mixer), CAutoDlg(CMultiAutoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMultiAutoDlg)
	//}}AFX_DATA_INIT
	if (m_Accel == NULL)
		m_Accel = LOADACCEL(IDR_MULTI_AUTO_DLG);
	ASSERT(AutoIdx >= 0 && AutoIdx < CChanInfo::AUTOS);
	m_AutoIdx = AutoIdx;
	m_Slider.SetDefaultPos(GetSlider(0)->GetDefaultPos());
	m_HandlerWrite = TRUE;
	m_FirstChan = -1;
	m_IsModified = FALSE;
	// m_Slider uses CAutoSliderCtrl only for fine adjust, so disable auto dialog
	m_Slider.SetAutoStyle(m_Slider.GetAutoStyle() & ~CAutoSliderCtrl::AUTO_DIALOG);
}


void CMultiAutoDlg::DoDataExchange(CDataExchange* pDX)
{
	CAutoDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMultiAutoDlg)
	DDX_Control(pDX, IDC_AUTO_POS_EDIT, m_PosEdit);
	DDX_Control(pDX, IDC_AUTO_START_EDIT, m_StartEdit);
	DDX_Control(pDX, IDC_AUTO_END_EDIT, m_EndEdit);
	DDX_Control(pDX, IDC_AUTO_STAGGER, m_Stagger);
	DDX_Control(pDX, IDC_AUTO_SLIDER, m_Slider);
	//}}AFX_DATA_MAP
}

int CMultiAutoDlg::GetState(CAutoInfo& Info, float& Val, int& FirstChan) const
{
	FirstChan = -1;
	CAutoInfo	PrvInfo;
	float	PrvVal;
	int	Mask = 0;
	for (int i = 0; i < m_Mixer.GetItemCount(); i++) {
		if (m_Mixer.IsSelOrCurAndFull(i)) {
			GetSlider(i)->GetInfo(Info);
			Val = GetSlider(i)->GetNormPos();
			if (FirstChan < 0) {
				FirstChan = i;
				PrvInfo = Info;
				PrvVal = Val;
				Mask = -1;
			} else {
				if (Val != PrvVal)
					Mask &= ~BM_POSITION;
				if (Info.m_StartPos != PrvInfo.m_StartPos 
					|| Info.m_EndPos != PrvInfo.m_EndPos)
					Mask &= ~BM_SELECTION;
				if (Info.m_Time != PrvInfo.m_Time)
					Mask &= ~BM_TIME;
				if (Info.m_Transport != PrvInfo.m_Transport)
					Mask &= ~BM_TRANSPORT;
				if (Info.m_Loop != PrvInfo.m_Loop)
					Mask &= ~BM_LOOP;
				if (Info.m_Waveform != PrvInfo.m_Waveform)
					Mask &= ~BM_WAVEFORM;
			}
		}
	}
	return(Mask);
}

void CMultiAutoDlg::SetPosition(float Val)
{
	for (int i = 0; i < m_Mixer.GetItemCount(); i++) {
		if (m_Mixer.IsSelOrCurAndFull(i)) {
			switch (m_AutoIdx) {
			case CChanInfo::AUTO_POS:
				// if slider is a playing trigger, just resync it
				if (GetSlider(i)->IsPlaying()) 
					((CAutoTriggerCtrl *)GetSlider(i))->Trigger();
				else
					GetSlider(i)->SetNormPos(Val);
				break;
			default:
				GetSlider(i)->SetNormPos(Val);
				GetSlider(i)->SetReverse(FALSE);
			}
		}
	}
	UpdatePosEdit();
}

void CMultiAutoDlg::SetSelection(float fMin, float fMax)
{
	for (int i = 0; i < m_Mixer.GetItemCount(); i++) {
		if (m_Mixer.IsSelOrCurAndFull(i))
			GetSlider(i)->SetNormSelection(fMin, fMax);
	}
	UpdateSelectionEdits();
}

void CMultiAutoDlg::SetTime(int Time)
{
	for (int i = 0; i < m_Mixer.GetItemCount(); i++) {
		if (m_Mixer.IsSelOrCurAndFull(i))
			GetSlider(i)->SetTime(Time);
	}
}

void CMultiAutoDlg::SetTransport(int Transport)
{
	for (int i = 0; i < m_Mixer.GetItemCount(); i++) {
		if (m_Mixer.IsSelOrCurAndFull(i)) {
			GetSlider(i)->SetTransport(Transport);
			switch (m_AutoIdx) {
			case CChanInfo::AUTO_VOL:
				// if starting a volume automation, and channel isn't 
				// auto triggering, and audio is paused, play audio
				if (Transport == PLAY && !GetChannel(i)->IsTriggering()
				&& GetChannel(i)->GetTransport() == CChannel::PAUSE)
					GetChannel(i)->SetTransport(CChannel::PLAY);
				break;
			case CChanInfo::AUTO_POS:
				// if auto-trigger play turns off audio looping as a side effect
				if (CChannel::GetOptions() == CChannel::AUTO_TRIG_LOOP_OFF) {
					if (Transport == PLAY)	// if starting a trigger automation
						GetChannel(i)->SetLoop(FALSE);	// disable audio looping
				}
				break;
			}
		}
	}
}

void CMultiAutoDlg::SetLoop(bool Enable)
{
	for (int i = 0; i < m_Mixer.GetItemCount(); i++) {
		if (m_Mixer.IsSelOrCurAndFull(i))
			GetSlider(i)->SetLoop(Enable);
	}
	OnModify();
}

void CMultiAutoDlg::SetWaveform(int Waveform)
{
	for (int i = 0; i < m_Mixer.GetItemCount(); i++) {
		if (m_Mixer.IsSelOrCurAndFull(i))
			GetSlider(i)->SetWaveform(Waveform);
	}
	OnModify();
}

void CMultiAutoDlg::Stagger()
{
	int	SelCount = m_Mixer.GetSelCount();
	if (!SelCount)
		return;
	int	j = 0;
	SelCount = max(SelCount - 1, 1);
	// if we're editing position, must bump divisor by one; otherwise last 
	// channel gets same position as first one, because of loop wraparound
	if (m_AutoIdx == CChanInfo::AUTO_POS)
		SelCount++;
	for (int i = 0; i < m_Mixer.GetItemCount(); i++) {
		if (m_Mixer.IsSelOrCurAndFull(i)) {
			SetPosWithinSel(i, float(j) / SelCount);
			j++;
		}
	}
	OnModify();
}

void CMultiAutoDlg::OnTimeChange()
{
	SetTime(m_ip->m_Time);
}

void CMultiAutoDlg::OnTransport(int Transport)
{
	m_Slider.SetTransport(Transport);
	if (m_HandlerWrite)
		SetTransport(Transport);
}

void CMultiAutoDlg::OnModify()
{
	m_IsModified = TRUE;
}

CChannel *CMultiAutoDlg::GetChannel(int ChanIdx) const
{
	return(m_Mixer.GetChan(ChanIdx));
}

CAutoSliderCtrl	*CMultiAutoDlg::GetSlider(int ChanIdx) const
{
	return(m_Mixer.GetChan(ChanIdx)->GetAuto(m_AutoIdx));
}

void CMultiAutoDlg::SetPosWithinSel(int ChanIdx, float Pos)
{
	float	SelMin, SelMax;
	CAutoSliderCtrl	*Slider = GetSlider(ChanIdx);
	// if slider is a playing trigger, ignore selection because it affects
	// audio loop, not trigger loop; trigger loop's range is always 0..1
	if (m_AutoIdx == CChanInfo::AUTO_POS && Slider->IsPlaying())
		((CAutoTriggerCtrl *)Slider)->StartFromPos(Pos);
	else {
		// if selection, translate position so it fits within selection
		if (Slider->HasSelection()) {
			Slider->GetNormSelection(SelMin, SelMax);
			Slider->SetNormPos(SelMin + (SelMax - SelMin) * Pos);
			Slider->SetReverse(FALSE);
		} else
			Slider->SetNormPos(Pos);
	}
}

void CMultiAutoDlg::UpdateSelectionEdits()
{
	if (m_FirstChan < 0)
		return;
	CChannel	*Chan = m_Mixer.GetChan(m_FirstChan);
	CAutoSliderCtrl	*Slider = GetSlider(m_FirstChan);
	if (Slider->HasSelection()) {
		float	fMin, fMax;
		Slider->GetNormSelection(fMin, fMax);
		m_StartEdit.SetVal(Chan->Denormalize(m_AutoIdx, fMin));
		m_EndEdit.SetVal(Chan->Denormalize(m_AutoIdx, fMax));
	} else {
		m_StartEdit.SetBlank();
		m_EndEdit.SetBlank();
	}
}

void CMultiAutoDlg::UpdatePosEdit()
{
	if (m_FirstChan < 0)
		return;
	CChannel	*Chan = m_Mixer.GetChan(m_FirstChan);
	CAutoSliderCtrl	*Slider = GetSlider(m_FirstChan);
	m_PosEdit.SetVal(Chan->Denormalize(m_AutoIdx, Slider->GetNormPos()));
}

void CMultiAutoDlg::UpdateCtrls()
{
	// for each data element, if it's consistent across entire selection, set
	// its dialog control accordingly; otherwise set control to indeterminate
	float	Val;
	CAutoInfo	Info;
	int	Mask = GetState(Info, Val, m_FirstChan);
	m_HandlerWrite = FALSE;	// prevent handlers from updating channels
	if (Mask & BM_POSITION) {
		m_Slider.SetNormPos(Val);
		UpdatePosEdit();
	} else
		m_PosEdit.SetBlank();
	if (Mask & BM_SELECTION) {
		m_Slider.SetNormSelection(Info.m_StartPos, Info.m_EndPos);
		UpdateSelectionEdits();
	} else {
		m_StartEdit.SetBlank();
		m_EndEdit.SetBlank();
	}
	if (Mask & BM_TIME) {
		m_ip->m_Time = Info.m_Time;
		m_ip->m_TimeUnit = CTimeEdit::GetAppropriateUnit(Info.m_Time);
		UpdateUI();
	} else {
		m_ip->m_Time = 0;
		UpdateUI();
		m_TimeEdit.SetBlank();
	}
	if (Mask & BM_TRANSPORT)
		CAutoDlg::SetTransport(Info.m_Transport);
	else
		CAutoDlg::SetTransport(UNSET);
	if (Mask & BM_LOOP) {
		m_ip->m_Loop = Info.m_Loop;
		m_Loop.SetCheck(Info.m_Loop);
	} else
		m_Loop.SetCheck(2);	// set loop button to indeterminate
	if (Mask & BM_WAVEFORM) {
		m_ip->m_Waveform = Info.m_Waveform;
		m_Waveform.SetCurSel(Info.m_Waveform);
	} else
		m_Waveform.SetCurSel(-1);
	m_HandlerWrite = TRUE;	// restore normal handler behavior
}

float CMultiAutoDlg::Normalize(float Pos)
{
	int	ChanIdx = max(m_FirstChan, 0);
	return(CLAMP(m_Mixer.GetChan(ChanIdx)->Normalize(m_AutoIdx, Pos), 0, 1));
}

float CMultiAutoDlg::Denormalize(float Pos)
{
	int	ChanIdx = max(m_FirstChan, 0);
	return(m_Mixer.GetChan(ChanIdx)->Denormalize(m_AutoIdx, CLAMP(Pos, 0, 1)));
}

void CMultiAutoDlg::TimerHook(float Scale)
{
	int	Transport = UNSET;
	bool	First = TRUE;
	for (int i = 0; i < m_Mixer.GetItemCount(); i++) {
		if (m_Mixer.IsSelOrCurAndFull(i)) {
			if (GetSlider(i)->GetTransport() != Transport) {
				if (First) {
					Transport = GetSlider(i)->GetTransport();
					First = FALSE;
				} else	// sliders don't all have the same transport state
					Transport = UNSET;	// make UI appear indeterminate
			}
		}
	}
	if (Transport != m_ip->m_Transport) {
		m_HandlerWrite = FALSE;	// prevent OnTransport from updating channels
		CAutoDlg::SetTransport(Transport);
		m_HandlerWrite = TRUE;
	}
}

void CMultiAutoDlg::CMyAutoSliderCtrl::OnNewPos()
{
	CMultiAutoDlg	*cp = (CMultiAutoDlg *)GetCookie();
	// while we're playing, GetNormPos returns m_WavePos, but because we're not
	// really an autoslider and don't get a timer hook, our m_WavePos is always
	// zero, so we must use m_Info.m_Pos directly instead of calling GetNormPos
	if (cp->m_HandlerWrite)
		cp->SetPosition(m_Info.m_Pos);
	cp->OnModify();
}

void CMultiAutoDlg::CMyAutoSliderCtrl::OnSelection()
{
	CMultiAutoDlg	*cp = (CMultiAutoDlg *)GetCookie();
	float	SelMin, SelMax;
	GetNormSelection(SelMin, SelMax);
	if (cp->m_HandlerWrite)
		cp->SetSelection(SelMin, SelMax);
	cp->OnModify();
}

BEGIN_MESSAGE_MAP(CMultiAutoDlg, CAutoDlg)
	//{{AFX_MSG_MAP(CMultiAutoDlg)
	ON_BN_CLICKED(IDC_AUTO_LOOP, OnLoop)
	ON_COMMAND(ID_AUTO_VOLUME, OnAutoVolume)
	ON_COMMAND(ID_AUTO_PAN, OnAutoPan)
	ON_COMMAND(ID_AUTO_PITCH, OnAutoPitch)
	ON_COMMAND(ID_AUTO_POS, OnAutoPos)
	ON_BN_CLICKED(IDC_AUTO_STAGGER, OnAutoStagger)
	ON_COMMAND(ID_AUTO_TOGGLE_LOOP, OnToggleLoop)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_NOTIFY(UDN_DELTAPOS, IDC_AUTO_POS_SPIN, OnDeltaposAutoPosSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_AUTO_START_SPIN, OnDeltaposAutoStartSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_AUTO_END_SPIN, OnDeltaposAutoEndSpin)
	ON_CBN_SELCHANGE(IDC_AUTO_WAVEFORM, OnSelchangeAutoWaveform)
	ON_COMMAND(ID_GO, OnGo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMultiAutoDlg message handlers

BOOL CMultiAutoDlg::OnInitDialog() 
{
	CAutoDlg::OnInitDialog();	// initialize base dialog first

	m_Slider.SetRange(0, CAutoSliderDlg::SLIDER_RANGE);
	m_Slider.CreateTicks(CAutoSliderDlg::SLIDER_TICKS);
	m_Slider.SetTime(0);	// disable quick start
	m_Stagger.SetIcon(AfxGetApp()->LoadIcon(IDI_STAGGER));
	CString	AutoName;
	CChannel::GetAutoName(m_AutoIdx, AutoName);
	SetWindowText(AutoName + " - " + 
		(m_Mixer.GetSelCount() ? LDS(SELECTED_CHANS) : 
		GetChannel(m_Mixer.GetCurPos())->GetTitle()));
	if (m_Mixer.HasToolTips())
		EnableToolTips(TRUE);
	m_Slider.SetCookie(this);
	if (m_AutoIdx == CChanInfo::AUTO_POS) {	// if editing auto trigger, waveform has
		m_Waveform.EnableWindow(FALSE);		// no effect, so disable waveform combo box
		m_Loop.EnableWindow(FALSE);			// disable loop button too
	}
	WPARAM	Font = (WPARAM)GetStockObject(SYSTEM_FONT);
	m_PosEdit.SendMessage(WM_SETFONT, Font);
	m_PosEdit.SetPrecision(m_PosPrecision);
	m_StartEdit.SendMessage(WM_SETFONT, Font);
	m_StartEdit.SetPrecision(m_PosPrecision);
	m_EndEdit.SendMessage(WM_SETFONT, Font);
	m_EndEdit.SetPrecision(m_PosPrecision);
	UpdateCtrls();
	m_IsModified = FALSE;	// init sets modified, so reset it; do this LAST

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMultiAutoDlg::OnLoop() 
{
	m_ip->m_Loop ^= 1;
	m_Loop.SetCheck(m_ip->m_Loop);
	SetLoop(m_ip->m_Loop != 0);
}

BOOL CMultiAutoDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST) {
		if (TranslateAccelerator(m_hWnd, m_Accel, pMsg))
			return(TRUE);
	}
	return CAutoDlg::PreTranslateMessage(pMsg);
}

void CMultiAutoDlg::OnAutoVolume() 
{
	if (m_AutoIdx == CChanInfo::AUTO_VOL)
		OnOK();
}

void CMultiAutoDlg::OnAutoPan() 
{
	if (m_AutoIdx == CChanInfo::AUTO_PAN)
		OnOK();
}

void CMultiAutoDlg::OnAutoPitch() 
{
	if (m_AutoIdx == CChanInfo::AUTO_PITCH)
		OnOK();
}

void CMultiAutoDlg::OnAutoPos() 
{
	if (m_AutoIdx == CChanInfo::AUTO_POS)
		OnOK();
}

void CMultiAutoDlg::OnAutoStagger() 
{
	Stagger();
}

void CMultiAutoDlg::OnToggleLoop() 
{
	OnLoop();
}

void CMultiAutoDlg::OnEditUndo() 
{
	// disables undo accelerator during modal edit
}

void CMultiAutoDlg::OnEditRedo() 
{
	// disables redo accelerator during modal edit
}

void CMultiAutoDlg::OnDeltaposAutoPosSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	m_PosEdit.AddSpin(-pNMUpDown->iDelta * m_PosSpinAmt);
	*pResult = 0;
}

void CMultiAutoDlg::OnDeltaposAutoStartSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	m_StartEdit.AddSpin(-pNMUpDown->iDelta * m_PosSpinAmt);
	*pResult = 0;
}

void CMultiAutoDlg::OnDeltaposAutoEndSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	m_EndEdit.AddSpin(-pNMUpDown->iDelta * m_PosSpinAmt);
	*pResult = 0;
}

BOOL CMultiAutoDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	NMHDR	*nmh = (NMHDR *)lParam;
	HANDLE	hFrom = nmh->hwndFrom;
	if (hFrom == m_PosEdit.m_hWnd) {
		float	Pos = Normalize(m_PosEdit.GetVal());
		m_Slider.SetNormPos(Pos);
		SetPosition(Pos);
		return(TRUE);
	} else if (hFrom == m_StartEdit.m_hWnd || hFrom == m_EndEdit.m_hWnd) {
		float	fMin = Normalize(m_StartEdit.GetVal());
		float	fMax = Normalize(m_EndEdit.GetVal());
		// if values are non-blank and in correct order, update selection
		if (!m_StartEdit.IsBlank() && !m_EndEdit.IsBlank() && fMin < fMax) {
			m_Slider.SetNormSelection(fMin, fMax);
			SetSelection(fMin, fMax);
		} else {	// clamp notifying value and refresh its edit box
			if (hFrom == m_StartEdit.m_hWnd)
				m_StartEdit.SetVal(Denormalize(fMin));
			else
				m_EndEdit.SetVal(Denormalize(fMax));
		}
		return(TRUE);
	}
	return CAutoDlg::OnNotify(wParam, lParam, pResult);
}

void CMultiAutoDlg::OnSelchangeAutoWaveform() 
{
	SetWaveform(m_Waveform.GetCurSel());
}

void CMultiAutoDlg::OnGo() 
{
	m_Mixer.Go();
}
