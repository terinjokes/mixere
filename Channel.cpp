// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda

		revision history:
		rev		date	comments
        00      21nov03 initial version
		01		30nov03	clip child on chan & cform, no redraw in chan OnSize
		02		30nov03 allow OnSize when invisible
		03		30nov03	only number should select, not all bg
		04		30nov03 select must invalidate number
		05		02dec03	starting pos drag select clears current loop now
		06		05dec03	add master volume
		07		05dec03	add column resizing
		08		08dec03	during loop editing, don't let position escape
		09		11dec03	don't let a disabled control keep the focus
		10		26dec03	add automation
		11		28dec03	add tooltips
		12		31dec03	add current position
		13		08jan04	tooltips: show pitch in semis, position in secs
		14		16jan04	add file filter and parse function
		15		17jan04	add mute/solo
		16		20jan04	move drop files handler to mixer channel
		17		21jan04	name is a static control now; don't change its color
		18		25jan04	don't center tooltip, let it track mouse for now
		19		26jan04	add GetAutoSlider accessor
		20		27jan04	missing PitchToSlider in Redraw
		21		31jan04	add edit name
		22		01feb04	auto dlg must display title not file name
		23		02feb04 add auto trigger
		24		07feb04	load audio must reset title
		25		15feb04	OnSelection calls UpdateSliderSelection now
		26		22feb04	add undo
		27		23feb04	GetInfo saves index now
		28		25feb04 restore focus and activation too
		29		08mar04	in FindUndoable, only focus during undo/redo
		30		09mar04	in PreTranslateMessage, pass keys to parent window
		31		10mar04	add scale to timer hook
		32		12mar04	add edit name, fix tab key
		33		24mar04	don't bail out of SetInfo if open audio fails
		34		16apr04	add mute/solo fader
		35		19apr04	add animation delay for position
		36		20apr04	auto volume and pos now save/restore transport state
		37		08may04	add denorm so auto slider datatips can show selection
		38		11may04	add set title font
		39		14may04	in SetInfo, if we're in solo mode, zero fader position
		40		14may04 ApplyMuteSolo gets enable from caller now
		41		24jun04	use only normalized coordinates with auto sliders
		42		05oct04	no more child dialogs; pass all keyboard msgs to parent
		43		08oct04	in ApplyMuteSolo, if not fading, stop fader
		44		08nov04	don't post message in OnParentNotify, just activate item
		45		12nov04	move key redirection into base class
		46		15nov04	add drag detection
		47		18nov04	add color scheme
		48		14dec04	add save/restore transport state for volume quick start
		49		15dec04	auto volume play forces audio play only if audio paused
		50		22dec04	add Ogg Vorbis, FLAC, and MOD audio files to filter
		51		29dec04	audio pos escaping loop: remove overlap test, add fudge
		52		06jan05	OnStop's dup test must account for rewind side effect
		53		06jan05	add SetTransportAndPos for SetInfo and RestoreUndoState
		54		07jan05	don't upcast auto trigger for get/set info; non-virtual
		55		09jan05	add go
		56		10jan05	add ID
		57		17jan05	in SetInfo, don't set position slider from audio loop
		58		21jan05	in Rewind, if selection starts at zero, use Reset
		59		24feb05	change auto-trigger to use stop instead of pause
		60		25feb05	add option for auto-trigger turns off audio looping

		channel control

*/

// Channel.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "Channel.h"
#include "Shlwapi.h"
#include "math.h"
#include "AutoDlg.h"
#include "MixereDoc.h"
#include "MixereView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DYNAMIC_TOOLTIPS	// uncomment for tooltips that track real-time data

/////////////////////////////////////////////////////////////////////////////
// CChannel dialog

const	UINT	CChannel::m_AutoNameID[AUTOS] = {
	IDS_CH_VOLUME,
	IDS_CH_PAN,
	IDS_CH_PITCH,
	IDS_CH_TRIGGER
};

const CChannel::COORD_FUNC CChannel::m_CoordFunc[AUTOS] = {
	{&NormVolume,	&DenormVolume},
	{&NormPan,		&DenormPan},
	{&NormPitch,	&DenormPitch},
	{&NormPos,		&DenormPos}
};

CChanInfo CChannel::m_DefaultInfo;

const LPCSTR CChannel::m_FileFilter = 
	"Sound Files (*.wav, *.mp3, *.aif, *.aiff, *.aifc, "
	"*.ogg, *.flac, *.mod, *.s3m, *.xm, *.it)|"
	"*.wav; *.mp3; *.aif; *.aiff; *.aifc; "
	"*.ogg; *.flac; *.mod; *.s3m; *.xm; *.it|"
	"WAV Files (*.wav)|*.wav|"
	"MP3 Files (*.mp3)|*.mp3|"
	"AIFF Files (*.aif, *.aiff, *.aifc)|*.aif; *.aiff; *.aifc|"
	"Ogg Vorbis Files (*.ogg)|*.ogg|"
	"FLAC Files (*.flac)|*.flac|"
	"MOD Files (*.mod, *.s3m, *.xm, *.it)|*.mod; *.s3m; *.xm; *.it|"
	"All Files (*.*)|*.*||";

CStringArray CChannel::m_ExtList;	// sound file extensions from file filter
int	CChannel::m_ExtCount = ParseFileFilter(m_FileFilter, m_ExtList);

const LPCSTR CChannel::m_TipFmtf	= ": %.3f";
const LPCSTR CChannel::m_TipFmtf2	= ": %.3f, %.3f";
const LPCSTR CChannel::m_TipFmti	= ": %d";

HACCEL CChannel::m_Accel;

int CChannel::m_Options = DEFAULT_OPTIONS;

CChannel::COLOR_SCHEME CChannel::m_Scheme = {
	RGB(255, 0, 0),		// active = red
	RGB(0, 220, 220)	// selected = medium cyan
};
CBrush	CChannel::m_SelectionBrush(m_Scheme.Selected);

/////////////////////////////////////////////////////////////////////////////
// CChannel construction/destruction

CChannel::CChannel(CMixereView& Mixer)
	: m_Mixer(Mixer), CFormListItemDlg(CChannel::IDD, Mixer)
{
	//{{AFX_DATA_INIT(CChannel)
	//}}AFX_DATA_INIT
	m_Audio.SetChannel(this);
	m_ID = 0;
	m_Transport = STOP;
	m_Muted = FALSE;
	m_Soloed = FALSE;
	// initialize mute/solo fader
	m_MSFader.m_StartPos = 0;
	m_MSFader.m_EndPos = 1;
	m_MSFader.m_Pos = 1;
	m_MSFader.m_Time = 0;
	m_MSFadeTime = m_Mixer.GetMSFadeTime();
	m_MSFadeEnable = TRUE;
	// list our automation sliders, in left-to-right order
	m_Auto[0] = &m_Volume;
	m_Auto[1] = &m_Pan;
	m_Auto[2] = &m_Pitch;
	m_Auto[3] = &m_Pos;
	// list our child controls, in left-to-right order
	m_WndList[0]	= &m_Number;
	m_WndList[1]	= &m_Name;
	m_WndList[2]	= &m_Play;
	m_WndList[3]	= &m_Pause;
	m_WndList[4]	= &m_Stop;
	m_WndList[5]	= &m_Loop;
	m_WndList[6]	= &m_Mute;
	m_WndList[7]	= &m_Solo;
	m_WndList[8]	= &m_Volume;
	m_WndList[9]	= &m_Pan;
	m_WndList[10]	= &m_Pitch;
	m_WndList[11]	= &m_Pos;
	m_WndList[12]	= this;	// one extra for parent window
	// load accelerators if they're not already loaded
	if (m_Accel == NULL)
		m_Accel = LOADACCEL(IDR_MAINFRAME);
}

CChannel::~CChannel()
{
	UpdateSolo(FALSE);
	DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
// CChannel transport

void CChannel::SetTransport(int Transport)
{
	m_Play.SetCheck(Transport == PLAY);
	m_Pause.SetCheck(Transport == PAUSE);
	m_Stop.SetCheck(Transport == STOP);
	switch (Transport) {
	case PLAY:
		if (m_Pos.HasSelection())
			ClampAudioPos();	// keep position within selection
		m_Audio.Play();
		break;
	case PAUSE:
		m_Audio.Stop();
		break;
	case STOP:
		m_Audio.Stop();
		Rewind();
		break;
	}
	m_Transport = Transport;
}

void CChannel::SetTransportAndPos(int Transport, int Frame)
{
	m_Play.SetCheck(Transport == PLAY);
	m_Pause.SetCheck(Transport == PAUSE);
	m_Stop.SetCheck(Transport == STOP);
	switch (Transport) {
	case PLAY:
		SetPosition(Frame);	// set position before playing
		if (m_Transport != PLAY)
			m_Audio.Play();
		break;
	case PAUSE:
	case STOP:
		m_Audio.Stop();	// stop before setting position
		SetPosition(Frame);
		break;
	}
	m_Transport = Transport;
}

bool CChannel::TogglePlay()
{
	SetTransport(m_Transport == PLAY ? PAUSE : PLAY);
	return(m_Transport == PLAY);
}

void CChannel::Go()
{
	if (m_Transport == PAUSE)
		SetTransport(PLAY);
	for (int i = 0; i < AUTOS; i++)
		m_Auto[i]->Go();
}

void CChannel::Rewind()
{
	// if we have a selection, reset to start of selection
	if (m_Audio.IsLooped()) {
		int	Start = m_Audio.GetLoop().Start();
		// after a SetPosition, the starting position of the audio varies, by a
		// small but seemingly random amount; this is especially annoying when
		// the selection starts at zero, and it can be easily avoided in that
		// case by using Reset instead of SetPosition
		if (Start)
			SetPosition(Start);
		else {	// start position is zero, so use reset to avoid variation
			m_Audio.Reset();
			m_Pos.SetPos(0);
		}
	} else {	// reset to start of audio file
		m_Audio.Reset();
		m_Pos.SetPos(0);
	}
}

bool CChannel::IsRewound() const
{
	int	Pos = m_Audio.GetPosition();
	if (m_Audio.IsLooped())
		return(Pos == m_Audio.GetLoop().Start());
	return(!Pos);
}

void CChannel::ClampAudioPos()
{
	CAudio::CLoop	Loop;
	if (m_Audio.GetLoop(Loop)) {
		int pos = m_Audio.GetPosition();
		if (pos < Loop.Start() || pos > Loop.End())	// if outside of loop
			m_Audio.SetPosition(Loop.Start());		// clamp audio position
	}
}

/////////////////////////////////////////////////////////////////////////////
// CChannel attributes

bool CChannel::GetLoop() const
{
	return(m_Audio.GetRepeat());
}

void CChannel::SetLoop(bool Enable)
{
	m_Audio.SetRepeat(Enable);
	m_Loop.SetCheck(Enable);
}

void CChannel::SetMute(bool Enable)
{
	m_Mute.SetCheck(Enable);
	UpdateMute(Enable);
}

void CChannel::SetSolo(bool Enable)
{
	m_Solo.SetCheck(Enable);
	UpdateSolo(Enable);
}

float CChannel::GetVolume() const
{
	return(m_Audio.GetVolume());
}

void CChannel::SetVolume(float Volume)
{
	m_Audio.SetVolume(Volume);
	m_Volume.SetNormPos(m_Audio.VolToNorm(Volume));
}

float CChannel::GetPan() const
{
	return(m_Audio.GetPan());
}

void CChannel::SetPan(float Pan)
{
	m_Audio.SetPan(Pan);
	m_Pan.SetNormPos(m_Audio.PanToNorm(Pan));
}

float CChannel::GetPitch() const
{
	return(m_Audio.GetPitch());
}

void CChannel::SetPitch(float Pitch)
{
	m_Audio.SetPitch(Pitch);
	m_Pitch.SetNormPos(m_Audio.PitchToNorm(Pitch));
}

int CChannel::GetPosition() const
{
	return(m_Audio.GetPosition());
}

void CChannel::SetPosition(int Frame)
{
	m_Audio.SetPosition(Frame);
	m_Pos.SetNormPos(m_Audio.FrameToNorm(Frame));
}

void CChannel::GetSelection(CAudio::CLoop& Selection)
{
	m_Audio.GetLoop(Selection);
}

void CChannel::SetSelection(const CAudio::CLoop& Selection)
{
	if (Selection.Count())
		m_Audio.SetLoop(Selection);
	else
		m_Audio.RemoveLoop();
}

void CChannel::GetAutoName(int AutoIdx, CString& Name)
{
	ASSERT(AutoIdx >= 0 && AutoIdx < AUTOS);
	Name = CString(LPCSTR(m_AutoNameID[AutoIdx]));
}

/////////////////////////////////////////////////////////////////////////////
// CChannel volume

void CChannel::UpdateMute(bool Enable)
{
	if (Enable != m_Muted) {
		m_Muted = Enable;
		OnMute(Enable);
		UpdateVolume();
	}
}

void CChannel::UpdateSolo(bool Enable)
{
	if (Enable != m_Soloed) {
		m_Soloed = Enable;
		OnSolo(Enable);
		UpdateVolume();
	}
}

void CChannel::UpdateVolume()
{
	m_Audio.UpdateVolume();
}

void CChannel::OnSolo(bool Enable)
{
	m_Mixer.OnSolo(this, Enable);
	ApplyMuteSolo(m_MSFadeEnable);
}

void CChannel::OnMute(bool Enable)
{
	m_Mixer.OnMute(this, Enable);
	ApplyMuteSolo(m_MSFadeEnable);
}

void CChannel::ApplyMuteSolo(bool Fade)
{
	bool	Level = GetMuteSolo();
	if (Fade && m_MSFadeTime) {			// if fade enabled and non-zero time
		m_MSFader.m_Reverse = !Level;				// set fader direction
		m_MSFader.m_Time = m_MSFadeTime;				// set fader time
		m_MSFader.m_Transport = CAutoInfo::PLAY;	// start fading to level
	} else {
		m_MSFader.m_Transport = CAutoInfo::STOP;	// stop fading
		m_MSFader.m_Pos = Level;					// jump to level
	}
}

bool CChannel::GetMuteSolo() const
{
	if (m_Mixer.GetSoloCount()) {
		if (!m_Soloed)
			return(0);
	} else {
		if (m_Muted)
			return(0);
	}
	return(1);
}

float CChannel::OnUpdateVolume()
{
	double	MuteVol;
	if (m_MSFader.IsPlaying())
		MuteVol = m_MSFader.m_Pos;
	else
		MuteVol = GetMuteSolo();
	return(MuteVol * m_Mixer.GetVolume() * m_Mixer.GetMasterVolume());
}

/////////////////////////////////////////////////////////////////////////////
// CChannel timer

void CChannel::TimerHook(float Scale)
{
	bool	Stop = FALSE;
	if (m_Audio.IsPlaying()) {
		// set slider position from audio position, but without calling OnNewPos
		if (CAutoSliderCtrl::IsAnimated())	// maybe skip this tic
			m_Pos.SetNormPosNoEcho(m_Audio.FrameToNorm(m_Audio.GetPosition()));
		// if loop is a one-shot, and position exceeds end, stop
		if (m_Audio.IsLooped() && !m_Audio.GetRepeat()
		&& m_Audio.GetPosition() > m_Audio.GetLoop().End())
			Stop = TRUE;
	} else {
		// if audio stopped playing by itself, update transport controls
		if (m_Transport == PLAY)
			Stop = TRUE;
	}
	if (Stop)
		SetTransport(STOP);
	for (int i = 0; i < AUTOS - 1; i++) {	// exclude auto trigger to avoid upcast
		if (m_Auto[i]->IsPlaying())
			m_Auto[i]->TimerHook(Scale);
	}
	if (m_Pos.IsPlaying())
		m_Pos.TimerHook(Scale);	// override of non-virtual function
	if (m_MSFader.IsPlaying()) {
		if (CAutoDlg::TimerHook(m_MSFader, NULL, Scale)) {	// mute/solo fader
			UpdateVolume();
		}
	}
#ifdef DYNAMIC_TOOLTIPS
	// if tooltip is visible, and cursor is within our window,
	// and within our parent's client area, update tooltip
	if (m_ToolTip != NULL && m_ToolTip->IsWindowVisible()) {
		CPoint	pt;
		GetCursorPos(&pt);	// in screen coordinates
		CRect	r, pr;
		GetWindowRect(r);
		GetParent()->GetClientRect(&pr);
		GetParent()->ClientToScreen(&pr);
		if (r.PtInRect(pt) && pr.PtInRect(pt))
			m_ToolTip->Update();
	}
#endif
}

/////////////////////////////////////////////////////////////////////////////
// CChannel serialization

void CChannel::GetInfo(void *Info) const
{
	CChanInfo *cip = (CChanInfo *)Info;
	cip->m_Index = m_Index;	// not set by SetInfo
	strncpy(cip->m_Path, m_Path, MAX_PATH);
	strncpy(cip->m_Title, m_Title, CChanInfo::MAX_TITLE);
	cip->m_ID = m_ID;
	cip->m_Transport = m_Transport;
	cip->m_Loop = m_Audio.GetRepeat();
	cip->m_Volume = m_Audio.GetVolume();
	cip->m_Pan = m_Audio.GetPan();
	cip->m_Pitch = m_Audio.GetPitch();
	cip->m_Pos = m_Audio.GetPosition();
	CAudio::CLoop	Loop;
	m_Audio.GetLoop(Loop);
	cip->m_Start = Loop.Start();
	cip->m_End = Loop.End();
	cip->m_Count = Loop.Count();
	for (int i = 0; i < AUTOS - 1; i++)	// exclude auto trigger to avoid upcast
		m_Auto[i]->GetInfo(cip->m_Auto[i]);
	m_Pos.GetInfo(cip->m_Auto[AUTO_POS]); // override of non-virtual function
	cip->m_Mute = m_Muted;
	cip->m_Solo = m_Soloed;
}

void CChannel::SetInfo(const void *Info)
{
	const CChanInfo *cip = (CChanInfo *)Info;
	// set audio state
	if (m_Audio.IsLoaded()) {
		m_Audio.SetRepeat(cip->m_Loop != 0);
		m_Audio.SetVolume(cip->m_Volume);
		m_Audio.SetPan(cip->m_Pan);
		m_Audio.SetPitch(cip->m_Pitch);
		if (cip->m_Count) {
			CAudio::CLoop	Loop(cip->m_Start, cip->m_End, cip->m_Count);
			m_Audio.SetLoop(Loop);
		} else
			m_Audio.RemoveLoop();
	}
	// set our state
	m_Path = cip->m_Path;
	m_Title = cip->m_Title[0] ? cip->m_Title : GetFileName();
	m_ID = cip->m_ID;
	for (int i = 0; i < AUTOS - 1; i++)	// exclude auto trigger to avoid upcast
		m_Auto[i]->SetInfo(cip->m_Auto[i]);
	m_Pos.SetInfo(cip->m_Auto[AUTO_POS]);	// override of non-virtual function
	m_MSFadeEnable = FALSE;		// temporarily disable mute/solo fading
	if (m_Mixer.GetSoloCount())		// if mixer in solo mode, zero fader
		m_MSFader.m_Pos = 0;
	UpdateMute(cip->m_Mute != 0);
	UpdateSolo(cip->m_Solo != 0);
	m_MSFadeEnable = TRUE;		// re-enable mute/solo fading
	m_Name.SetWindowText(m_Title);
	m_Loop.SetCheck(cip->m_Loop);
	m_Mute.SetCheck(m_Muted);
	m_Solo.SetCheck(m_Soloed);
	SetTransportAndPos(cip->m_Transport, cip->m_Pos);
	EnableCtrls(m_Path.GetLength() != 0);
}

void CChannel::Reset()
{
	CChanInfo	ci;
	OpenItem(&ci);
	SetInfo(&ci);
}

/////////////////////////////////////////////////////////////////////////////
// CChannel drawing

void CChannel::EnableCtrls(bool Enable)
{
	// enable all our child controls except number and position
	for (int i = 0; i < CONTROLS; i++) {
		CWnd	*p = m_WndList[i]; 
		if (p != &m_Number && p != &m_Pos)
			p->EnableWindow(Enable);
	}
	m_Pos.EnableWindow(Enable && m_Audio.IsSeekable());
}

void CChannel::SetIndex(int Index)
{
	CString	s;
	s.Format("%d", Index + 1);
	m_Number.SetWindowText(s);
	m_Index = Index;	// instead of calling base class
}

void CChannel::SelectItem(bool Enable)
{
	if (Enable != m_Selected) {
		Invalidate();
		m_Number.Invalidate();
		m_Volume.Redraw();
		m_Pan.Redraw();
		m_Pitch.Redraw();
		m_Pos.Redraw();
	}
	m_Selected = Enable;	// instead of calling base class
}

void CChannel::ActivateItem(bool Enable)
{
	if (Enable != m_Active) {
		m_Number.Invalidate();
	}
	m_Active = Enable;	// instead of calling base class
}

void CChannel::UpdateSliderSelection()
{
	if (m_Pos.HasSelection()) {
		float	selmin, selmax;
		m_Pos.GetNormSelection(selmin, selmax);
		// force selection length to be at least one slider increment
		static const float	MIN_LOOP_LENGTH = 1.0 / SLIDER_RANGE;
		if (selmax - selmin < MIN_LOOP_LENGTH)
			selmax += MIN_LOOP_LENGTH;
		CAudio::CLoop	NewLoop(m_Audio.NormToFrame(selmin),
			m_Audio.NormToFrame(selmax));
		bool	WasLooped = m_Audio.IsLooped();
		m_Audio.SetLoop(NewLoop);
		// Don't let the audio position escape from the loop.  Note that if
		// audio is playing, no previous loop exists, and a loop is created
		// using a single click to the left of the slider thumb, the loop's
		// end point can be slightly ahead of the audio position.  Since we
		// only clamp the position when it's OUTSIDE the loop, the position
		// could escape in this case.  It's an audio bug, so we work around
		// it by fudging the endpoint by a slider increment in the compare.
		if (m_Transport == PLAY) {
			int pos = m_Audio.GetPosition();
			int	Fudge = WasLooped ? 0 : m_Audio.NormToFrame(MIN_LOOP_LENGTH);
			if (pos < NewLoop.Start() || pos > NewLoop.End() - Fudge)
				m_Audio.SetPosition(NewLoop.Start());	// clamp audio position
		}
	} else {
		if (m_Audio.IsLooped())
			m_Audio.RemoveLoop();
	}
}

void CChannel::GetColorScheme(COLOR_SCHEME& Scheme)
{
	Scheme = m_Scheme;
}

void CChannel::SetColorScheme(const COLOR_SCHEME& Scheme)
{
	m_Scheme = Scheme;
	if (m_SelectionBrush.GetSafeHandle() != NULL)	// if brush already exists
		m_SelectionBrush.DeleteObject();	// must delete before re-creating
	m_SelectionBrush.CreateSolidBrush(Scheme.Selected);
}

void CChannel::ApplyColorScheme()
{
	if (m_Selected) {
		SelectItem(FALSE);	// force a repaint
		SelectItem(TRUE);
	} else if (m_Active) {
		ActivateItem(FALSE);	// force a repaint
		ActivateItem(TRUE);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CChannel file support

bool CChannel::LoadAudio(LPCSTR Path)
{
	CChanInfo	ci(m_Mixer.GetChanDefaults());	// obtain defaults
	strncpy(ci.m_Path, Path, MAX_PATH);
	ci.m_Title[0] = 0;	// must reset title too
	ci.m_ID = m_Mixer.CreateChanID();
	if (!OpenItem(&ci))
		return(FALSE);
	SetInfo(&ci);
	return(TRUE);
}

void CChannel::CloseAudio()
{
	m_Audio.Close();
}

LPCSTR CChannel::GetFileName() const
{
	return(PathFindFileName(m_Path));
}

void CChannel::SetTitle(LPCSTR Title)
{
	m_Name.SetWindowText(Title);
	OnNameChange();
}

int CChannel::ParseFileFilter(LPCSTR Filter, CStringArray& ExtList)
{
	ExtList.RemoveAll();
	CString	s(Filter);
	char	*p = s.GetBuffer(0);
	if (p = strchr(p, '(')) {
		if (p = strtok(p, "()")) {
			p = strtok(p + 1, "*, )");
			while (p != NULL) {
				ExtList.Add(p);
				p = strtok(NULL, "*, )");
			}
		}
	}
	return(ExtList.GetSize());
}

/////////////////////////////////////////////////////////////////////////////
// CChannel conversions

float CChannel::NormVolume(float Pos) const
{
	return(m_Audio.VolToNorm(Pos));
}

float CChannel::DenormVolume(float Pos) const
{
	return(m_Audio.NormToVol(Pos));
}

float CChannel::NormPan(float Pos) const
{
	return(m_Audio.PanToNorm(Pos));
}

float CChannel::DenormPan(float Pos) const
{
	return(m_Audio.NormToPan(Pos));
}

float CChannel::NormPitch(float Pos) const
{
	return(m_Audio.PitchToNorm(m_Audio.LogToLinear(Pos / 12)));
}

float CChannel::DenormPitch(float Pos) const
{
	return(m_Audio.LinearToLog(m_Audio.NormToPitch(Pos)) * 12);
}

float CChannel::NormPos(float Pos) const
{
	return(m_Audio.FrameToNorm(m_Audio.SecsToFrames(Pos)));
}

float CChannel::DenormPos(float Pos) const
{
	return(m_Audio.FramesToSecs(m_Audio.NormToFrame(Pos)));
}

/////////////////////////////////////////////////////////////////////////////
// CChannel slider overrides 

void CChannel::CAutoVolumeCtrl::OnNewPos()
{
	CChannel	*cp = (CChannel *)GetCookie();
	cp->m_Audio.SetVolume(cp->m_Audio.NormToVol(GetNormPos()));
}

void CChannel::CAutoVolumeCtrl::OnDlgSaveUndoState(CUndoState& State)
{
	CChannel	*cp = (CChannel *)GetCookie();
	switch (State.GetCode()) {
	case UNDO_QUICK_START:	// auto slider uses SetData, but not m_Val
		State.m_Val.x.i = cp->m_Transport;
		State.m_Val.y.i = cp->m_Audio.GetPosition();
		break;
	default:
		cp->SaveTransportUndoState(State);
	}
}

void CChannel::CAutoVolumeCtrl::OnDlgRestoreUndoState(const CUndoState& State)
{
	CChannel	*cp = (CChannel *)GetCookie();
	switch (State.GetCode()) {
	case UNDO_QUICK_START:
		cp->SetTransportAndPos(State.m_Val.x.i, State.m_Val.y.i);
		break;
	default:
		cp->RestoreTransportUndoState(State);
	}
}

void CChannel::CAutoPanCtrl::OnNewPos()
{
	CChannel	*cp = (CChannel *)GetCookie();
	cp->m_Audio.SetPan(cp->m_Audio.NormToPan(GetNormPos()));
}

void CChannel::CAutoPitchCtrl::OnNewPos()
{
	CChannel	*cp = (CChannel *)GetCookie();
	cp->m_Audio.SetPitch(cp->m_Audio.NormToPitch(GetNormPos()));
}

void CChannel::CAutoPosCtrl::OnNewPos()
{
	if (!IsPlaying()) {		// if triggering, leave audio alone
		CChannel	*cp = (CChannel *)GetCookie();
		cp->m_Audio.SetPosition(cp->m_Audio.NormToFrame(GetNormPos()));
	}
}

void CChannel::CAutoPosCtrl::OnSelection()
{
	CChannel	*cp = (CChannel *)GetCookie();
	cp->UpdateSliderSelection();
	CAutoTriggerCtrl::OnSelection();	// must set trigger range too
}

void CChannel::CAutoPosCtrl::OnTrigger()
{
	CChannel	*cp = (CChannel *)GetCookie();
	cp->Rewind();
	cp->SetTransport(PLAY);
}

void CChannel::CAutoPosCtrl::OnDlgSaveUndoState(CUndoState& State)
{
	((CChannel *)GetCookie())->SaveTransportUndoState(State);
}

void CChannel::CAutoPosCtrl::OnDlgRestoreUndoState(const CUndoState& State)
{
	((CChannel *)GetCookie())->RestoreTransportUndoState(State);
}

/////////////////////////////////////////////////////////////////////////////
// CChannel undo/redo

void CChannel::SaveTransportUndoState(CUndoState& State) const
{
	switch (State.GetCode()) {
	case IDC_AUTO_PLAY:
	case ID_AUTO_TOGGLE_PLAY:
		{
			TRANSPORT_UNDO_STATE	us;
			us.Loop = m_Audio.GetRepeat();
			us.Transport = m_Transport;
			us.Position = m_Audio.GetPosition();
			State.SetData(&us, sizeof(TRANSPORT_UNDO_STATE));
		}
		break;
	}
}

void CChannel::RestoreTransportUndoState(const CUndoState& State)
{
	switch (State.GetCode()) {
	case IDC_AUTO_PLAY:
	case ID_AUTO_TOGGLE_PLAY:
		{
			TRANSPORT_UNDO_STATE	*usp;
			usp = (TRANSPORT_UNDO_STATE *)State.GetData();
			SetLoop(usp->Loop != 0);	// order matters here
			SetTransportAndPos(usp->Transport, usp->Position);
		}
		break;
	}
}

void CChannel::SaveUndoState(CUndoState& State)
{
	switch (State.GetCode()) {
	case IDC_CH_PLAY:
	case IDC_CH_PAUSE:
	case IDC_CH_STOP:
		State.m_Val.x.i = m_Transport;
		State.m_Val.y.i = m_Audio.GetPosition();
		break;
	default:
		NODEFAULTCASE;
	}
}

void CChannel::RestoreUndoState(const CUndoState& State)
{
	switch (State.GetCode()) {
	case IDC_CH_PLAY:
	case IDC_CH_PAUSE:
	case IDC_CH_STOP:
		SetTransportAndPos(State.m_Val.x.i, State.m_Val.y.i);
		GetDlgItem(State.GetCode())->SetFocus();
		break;
	default:
		NODEFAULTCASE;
	}
	OnActivateItem();
}

CUndoState::UNDO_KEY CChannel::GetUndoKey()
{
	return(m_Index + UNDO_KEY_OFFSET);	// offset index above control ID range
}

CUndoable *CChannel::FindUndoable(CUndoState::UNDO_KEY Key)
{
	// we override only to set focus and activate; base class finds control
	CUndoable	*uap = CUndoable::FindUndoable(Key);
	if (IsUndoInProgress()) {	// only focus during undo/redo
		UndoToWnd(uap)->SetFocus();
		OnActivateItem();
	}
	return(uap);
}

void CChannel::GetUndoTitle(const CUndoState& State, CString& Title)
{
	CWnd	*p = GetDlgItem(State.GetCode());
	if (p != NULL)
		WndToUndo(p)->GetUndoTitle(State, Title);
	else
		Title.Empty();
}

/////////////////////////////////////////////////////////////////////////////
// CChannel misc functions

BOOL CChannel::EnableToolTips(BOOL bEnable)
{
	for (int i = 0; i < AUTOS; i++)
		m_Auto[i]->EnableToolTips(bEnable);
	m_ToolTip.EnableToolTips(this, bEnable, FALSE);	// don't use dlg IDs
	return(bEnable);
}

void CChannel::GetAutoTip(char *DestBuf, int AutoIdx) const
{
	ASSERT(AutoIdx >= 0 && AutoIdx < AUTOS);
	const CAutoSliderCtrl	*Auto = m_Auto[AutoIdx];
	// if selection in progress or control key pressed, show selection range
	if (Auto->IsSelecting() || CAutoSliderCtrl::IsKeyDown(VK_CONTROL)) {
		if (Auto->HasSelection()) {
			float	selmin, selmax;
			Auto->GetNormSelection(selmin, selmax);
			sprintf(DestBuf, LDS(SELECTION) + m_TipFmtf2,
				Denormalize(AutoIdx, selmin), Denormalize(AutoIdx, selmax));
		} else
			strcpy(DestBuf, LDS(SELECTION) + ": 0");	// no selection
	} else {	// show position
		CString	Name;
		Auto->GetWindowText(Name);
		switch (AutoIdx) {
		case AUTO_POS:	// slider pos is trigger, use audio pos instead
			sprintf(DestBuf, Name + m_TipFmtf,
				m_Audio.FramesToSecs(m_Audio.GetPosition()));
			break;
		default:	// show slider pos
			sprintf(DestBuf, Name + m_TipFmtf,
				Denormalize(AutoIdx, Auto->GetNormPos()));
		}
	}
}

float CChannel::Normalize(int AutoIdx, float Val) const
{
	ASSERT(AutoIdx >= 0 && AutoIdx < AUTOS);
	return((this->*m_CoordFunc[AutoIdx].Normalize)(Val));
}

float CChannel::Denormalize(int AutoIdx, float Val) const
{
	ASSERT(AutoIdx >= 0 && AutoIdx < AUTOS);
	return((this->*m_CoordFunc[AutoIdx].Denormalize)(Val));
}

CFont *CChannel::GetTitleFont() const
{
	return(m_Name.GetFont());
}

void CChannel::SetTitleFont(CFont *Font)
{
	if (Font != NULL && Font->GetSafeHandle())
		m_Name.SetFont(Font, TRUE);
	else
		m_Name.SendMessage(WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), 1);
}

CChannel::WNDLIST CChannel::GetWndList()
{
	return(m_WndList);
}

bool CChannel::OpenItem(const void *Info)
{
	const CChanInfo *cip = (CChanInfo *)Info;
	m_Empty = cip->IsEmpty();
	if (m_Empty) {
		m_Audio.Close();
		return(TRUE);
	}
	return(m_Audio.Open(cip->m_Path));
}

void CChannel::CloseItem()
{
	m_Audio.Close();
}

/////////////////////////////////////////////////////////////////////////////
// CChannel data exchange and message map

void CChannel::DoDataExchange(CDataExchange* pDX)
{
	CFormListItemDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChannel)
	DDX_Control(pDX, IDC_CH_NUMBER, m_Number);
	DDX_Control(pDX, IDC_CH_NAME, m_Name);
	DDX_Control(pDX, IDC_CH_PLAY, m_Play);
	DDX_Control(pDX, IDC_CH_PAUSE, m_Pause);
	DDX_Control(pDX, IDC_CH_STOP, m_Stop);
	DDX_Control(pDX, IDC_CH_LOOP, m_Loop);
	DDX_Control(pDX, IDC_CH_SOLO, m_Solo);
	DDX_Control(pDX, IDC_CH_MUTE, m_Mute);
	DDX_Control(pDX, IDC_CH_VOL, m_Volume);
	DDX_Control(pDX, IDC_CH_PITCH, m_Pitch);
	DDX_Control(pDX, IDC_CH_PAN, m_Pan);
	DDX_Control(pDX, IDC_CH_POS, m_Pos);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CChannel, CFormListItemDlg)
	//{{AFX_MSG_MAP(CChannel)
	ON_BN_CLICKED(IDC_CH_NAME, OnName)
	ON_BN_CLICKED(IDC_CH_PLAY, OnPlay)
	ON_BN_CLICKED(IDC_CH_PAUSE, OnPause)
	ON_BN_CLICKED(IDC_CH_STOP, OnStop)
	ON_BN_CLICKED(IDC_CH_LOOP, OnLoop)
	ON_BN_CLICKED(IDC_CH_MUTE, OnMute)
	ON_BN_CLICKED(IDC_CH_SOLO, OnSolo)
	ON_WM_HSCROLL()
	ON_WM_CTLCOLOR()
	ON_WM_PARENTNOTIFY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_CONTEXTMENU()
	ON_WM_DROPFILES()
	ON_EN_CHANGE(IDC_CH_NAME, OnNameChange)
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTip)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChannel message handlers

BOOL CChannel::OnInitDialog()
{
	CFormListItemDlg::OnInitDialog();
	// save initial window coordinates
	GetWindowRect(m_InitRect);
	GetParent()->ScreenToClient(m_InitRect);
	// set text limit for title editing
	m_Name.SetLimitText(CChanInfo::MAX_TITLE);
	// load pairs of button icons
	m_Play.SetIcons(IDI_PLAYU, IDI_PLAYD);
	m_Pause.SetIcons(IDI_PAUSEU, IDI_PAUSED);
	m_Stop.SetIcons(IDI_STOPU, IDI_STOPD);
	m_Loop.SetIcons(IDI_LOOPU, IDI_LOOPD);
	m_Mute.SetIcons(IDI_MUTEU, IDI_MUTED);
	m_Solo.SetIcons(IDI_SOLOU, IDI_SOLOD);
	// init automation sliders
	int i;
	for (i = 0; i < AUTOS; i++) {
		m_Auto[i]->SetRange(0, SLIDER_RANGE);
		m_Auto[i]->CreateTicks(SLIDER_TICKS);
		m_Auto[i]->SetAutoDlgParent(&m_View);
		m_Auto[i]->SetCookie(this);
		m_Auto[i]->SetNormDefault(m_DefaultInfo.m_Auto[i].m_Pos);
	}
	// reset controls
	Reset();
	// request notification from all our child controls
	for (i = 0; i < CONTROLS; i++) {
		m_WndList[i]->ModifyStyleEx(WS_EX_NOPARENTNOTIFY, 0);
	}
	// prevent transport controls from sending undo notifications, because 
	// they're grouped; we'll send notifications from our clicked handlers
	m_Play.SetUndoHandler(NULL);
	m_Pause.SetUndoHandler(NULL);
	m_Stop.SetUndoHandler(NULL);
	return TRUE;
}

void CChannel::OnCancel()
{
}

void CChannel::OnOK()
{
}

void CChannel::OnName() 
{
	m_Name.SetFocus();
	CRect	r;
	m_Name.GetWindowRect(r);
	ScreenToClient(r);
	m_View.BeginDrag(this, r);
}

void CChannel::OnPlay()
{
	if (m_Transport != PLAY)	// prevent duplicate notifications
		NotifyUndoableEdit(IDC_CH_PLAY);
	SetTransport(PLAY);
}

void CChannel::OnPause()
{
	if (m_Transport != PAUSE)	// prevent duplicate notifications
		NotifyUndoableEdit(IDC_CH_PAUSE);
	SetTransport(PAUSE);
}

void CChannel::OnStop()
{
	if (m_Transport != STOP || !IsRewound())	// prevent duplicate notifications
		NotifyUndoableEdit(IDC_CH_STOP);		
	SetTransport(STOP);	// stop rewinds audio as a side effect
}

void CChannel::OnLoop()
{
	m_Audio.SetRepeat(m_Loop.GetCheck() != 0);
}

void CChannel::OnMute() 
{
	UpdateMute(m_Mute.GetCheck() != 0);
}

void CChannel::OnSolo() 
{
	UpdateSolo(m_Solo.GetCheck() != 0);
}

void CChannel::OnDropFiles(HDROP hDropInfo)
{
	m_Mixer.OnDropFiles(this, hDropInfo);
}

void CChannel::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CFormListItemDlg::OnHScroll(nSBCode, nPos, pScrollBar);
#ifdef DYNAMIC_TOOLTIPS
	if (m_ToolTip != NULL)
		m_ToolTip->Update();
#endif
}

BOOL CChannel::PreTranslateMessage(MSG* pMsg)
{
	if (m_ToolTip != NULL)
		m_ToolTip->RelayEvent(pMsg);
	return CFormListItemDlg::PreTranslateMessage(pMsg);
}

void CChannel::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	m_Mixer.OnContextMenu(this, pWnd, point);
}

HBRUSH CChannel::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH	hbr;
	// note that trackbars share same control type as static controls
	// name static control has a border, so we don't want to color it
	if (m_Selected && nCtlColor != CTLCOLOR_BTN && pWnd != &m_Name) {
		pDC->SetBkColor(m_Scheme.Selected);
		hbr = m_SelectionBrush;
	} else
		hbr = CFormListItemDlg::OnCtlColor(pDC, pWnd, nCtlColor);
	if (pWnd == &m_Number) {
		if (m_Active)
			pDC->SetTextColor(m_Scheme.Active);
	}
	return(hbr);
}

BOOL CChannel::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	NMHDR	*nmh = (NMHDR *)lParam;
	int	i;
	switch (nmh->code) {
	case AUTOSLIDER_ACTIVATE:
		OnActivateItem();
		return(TRUE);
	case AUTOSLIDER_CREATE:
		// automation dialog was created; set dialog's title
		for (i = 0; i < AUTOS; i++) {
			if (nmh->hwndFrom == m_Auto[i]->m_hWnd) {
				CString	Title;
				Title.Format(LDS(AUTO) + " %s - %s", 
					CString(LPCSTR(m_AutoNameID[i])), GetTitle());
				m_Auto[i]->SetAutoDlgTitle(Title);
				break;
			}
		}
		return(TRUE);
	case AUTOSLIDER_PLAY:
		// if notifier is volume slider 
		if (nmh->hwndFrom == m_Volume.m_hWnd) {
			// if playing volume automation unpauses audio as a side effect
			if (m_Options & AUTO_VOL_UNPAUSE) {
				// if we're in pause, and not auto triggered, play audio
				if (m_Transport == PAUSE && !m_Pos.IsPlaying())
					SetTransport(PLAY);
			}
		// if notifier is position slider 
		} else if (nmh->hwndFrom == m_Pos.m_hWnd) {
			// if auto-trigger play turns off audio looping as a side effect
			if (m_Options & AUTO_TRIG_LOOP_OFF)
				SetLoop(FALSE);	// turn off audio looping
		}
		return(TRUE);
	case AUTOSLIDER_GO:
		m_Mixer.Go();
		return(TRUE);
	}
	return CFormListItemDlg::OnNotify(wParam, lParam, pResult);
}

void CChannel::OnParentNotify(UINT message, LPARAM lParam)
{
	// intercept mouse button events from our child controls
	switch (message) {
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnActivateItem();
		break;
	}
}

BOOL CChannel::OnToolTip(UINT id, NMHDR* pTTTStruct, LRESULT* pResult)
{
	LPNMTTDISPINFO	ttp = LPNMTTDISPINFO(pTTTStruct);
	UINT nID = ttp->hdr.idFrom;
	if (ttp->uFlags & TTF_IDISHWND)		// idFrom can be HWND or ID
		nID = ::GetDlgCtrlID((HWND)nID);
	switch (nID) {
	case IDC_CH_NAME:	strcpy(ttp->szText, GetFileName());		break;
	case IDC_CH_PLAY:	strcpy(ttp->szText, LDS(CH_PLAY));		break;
	case IDC_CH_PAUSE:	strcpy(ttp->szText, LDS(CH_PAUSE));		break;
	case IDC_CH_STOP:	strcpy(ttp->szText, LDS(CH_STOP));		break;
	case IDC_CH_LOOP:	strcpy(ttp->szText, LDS(CH_LOOP));		break;
	case IDC_CH_MUTE:
		if (m_MSFader.IsPlaying())
			sprintf(ttp->szText, LDS(FADER) + m_TipFmtf, m_MSFader.m_Pos);
		else
			sprintf(ttp->szText, LDS(CH_MUTE) + m_TipFmti, m_Muted);
		break;
	case IDC_CH_SOLO:	
		if (m_MSFader.IsPlaying())
			sprintf(ttp->szText, LDS(FADER) + m_TipFmtf, m_MSFader.m_Pos);
		else
			sprintf(ttp->szText, LDS(CH_SOLO) + m_TipFmti, m_Soloed);
		break;
	case IDC_CH_VOL:	GetAutoTip(ttp->szText, AUTO_VOL);		break;
	case IDC_CH_PAN:	GetAutoTip(ttp->szText, AUTO_PAN);		break;
	case IDC_CH_PITCH:	GetAutoTip(ttp->szText, AUTO_PITCH);	break;
	case IDC_CH_POS:	GetAutoTip(ttp->szText, AUTO_POS);		break;
	}
	return(TRUE);
}

void CChannel::OnLButtonDown(UINT nFlags, CPoint point) 
{
	OnActivateItem();
	CRect	r;
	m_Name.GetWindowRect(r);
	ScreenToClient(r);
	if (point.x < r.left)	// if cursor is left of name, select
		OnSelectItem(nFlags);
	else if (point.x < r.right)	// else if cursor is within name, detect drag
		m_View.BeginDrag(this, r);
	CFormListItemDlg::OnLButtonDown(nFlags, point);
}


void CChannel::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CRect	r;
	m_Name.GetWindowRect(r);
	ScreenToClient(r);
	if (point.x < r.left)	// if cursor is left of name, select
		OnSelectItem(nFlags);
	CFormListItemDlg::OnLButtonDblClk(nFlags, point);
}

void CChannel::OnRButtonDown(UINT nFlags, CPoint point) 
{
	OnActivateItem();
	CFormListItemDlg::OnRButtonDown(nFlags, point);
}

void CChannel::OnNameChange()
{
	m_Name.GetWindowText(m_Title);
	// if title is empty, revert to file name
	if (m_Title.IsEmpty()) {
		m_Title = GetFileName();
		m_Name.SetWindowText(m_Title);
	}
}

void CChannel::EditName() 
{
	if (m_Title.IsEmpty())
		return;
	m_Name.Edit();
}
