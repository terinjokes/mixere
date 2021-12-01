// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      19jan04	initial version
		01		20jan04	add get active mixer
		02		28jan04	add crossfader
		03		31jan04	forgot to delete crossfader dialog
		04		22feb04	in remove mixer, don't assert mixer not found
		05		07mar04	add undo limit
		06		07may04	crossfader overridable name is now OnPosChange
		07		11may04	add set title font
		08		02oct04	convert master volume and crossfader to bars
		09		03oct04	create hides bars in case bar state can't be restored
		10		07oct04	make master volume and crossfader type persistent
		11		18nov04	add color scheme
		12		04jan05	make master and X-fader automation states persistent
		13		20jan05	add UpdateMixerName

		manage multiple mixers
 
*/

#include "stdafx.h"
#include "Resource.h"
#include "MainFrm.h"
#include "MixereDoc.h"
#include "MixereView.h"
#include "MultiMix.h"
#include "UndoManager.h"
#include "Persist.h"

const int CMultiMix::m_TimerPeriod = 50;	// in milliseconds

LPCSTR CMultiMix::MASTER_VOL_AUTO	= "MasterVolAuto";
LPCSTR CMultiMix::CROSSFADER_TYPE	= "CrossfaderType";
LPCSTR CMultiMix::CROSSFADER_AUTO	= "CrossfaderAuto";

const int CMultiMix::m_DlgBarResID[DLGBARS] = {
	IDD_VOLUME_BAR,
	IDD_CROSSFADER_BAR
};

CMultiMix::CMultiMix(CMainFrame *Frame) :
#pragma warning(disable : 4355)	// 'this' used in base member initializer list
	// these ctors don't access 'this', they only copy it to a member var
	m_MasterVolBar(this),
	m_CrossfaderBar(this),
#pragma warning(default : 4355)
	m_OptionsDlg(LDS(OPTIONS))
{
	m_Frm = Frame;
	m_Timer = 0;
	m_DlgBar[DLGBAR_MASTERVOL] = &m_MasterVolBar;
	m_DlgBar[DLGBAR_CROSSFADER] = &m_CrossfaderBar;
}

CMultiMix::~CMultiMix()
{
}

void CMultiMix::Create()
{
	// initialize timer
	CAutoDlg::SetTimerPeriod(m_TimerPeriod);
	m_Timer = m_Frm->SetTimer(1, m_TimerPeriod, NULL);
	// create control bars and dock them in a row
	int	DockStyle = CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM;	// horizontal only
	CControlBar	*LeftOf = m_Frm->GetToolBar();
	int i;
	for (i = 0; i < DLGBARS; i++) {
		m_DlgBar[i]->Create(m_Frm, m_DlgBarResID[i], 0, DockStyle, i, LeftOf);
		LeftOf = m_DlgBar[i];
	}
	// now hide docked bars in case frame can't restore dock state
	for (i = 0; i < DLGBARS; i++)
		m_Frm->ShowControlBar(m_DlgBar[i], FALSE, 0);
	// initialize dialog bar data
	m_MasterVolBar.SetWindowText(LDS(MASTER_VOLUME));
	m_MasterVolBar.SetBarCaption(LDS(MASTER));
	if (!GetAutoBar(MASTER_VOL_AUTO, m_MasterVolBar))
		m_MasterVolBar.SetVolume(1);	// default master volume
	if (!GetAutoBar(CROSSFADER_AUTO, m_CrossfaderBar))
		m_CrossfaderBar.SetPos(.5);		// default crossfader position
	m_CrossfaderBar.SetFaderType(CPersist::GetInt(REG_SETTINGS, CROSSFADER_TYPE, 0));
	// applying our tool tip state here is useless, because when we return, the
	// main frame restores the bar state, which magically enables tool tips for
	// all the bars; the main frame has to apply our tool tip state for us, but
	EnableToolTips(m_OptionsDlg.ShowToolTips());   // do it anyway just in case
}

void CMultiMix::Destroy()
{
	m_Frm->KillTimer(m_Timer);
	WriteAutoBar(MASTER_VOL_AUTO, m_MasterVolBar);
	WriteAutoBar(CROSSFADER_AUTO, m_CrossfaderBar);
	CPersist::WriteInt(REG_SETTINGS, CROSSFADER_TYPE, m_CrossfaderBar.GetFaderType());
}

void CMultiMix::AddMixer(CMixereView *Mixer)
{
	m_Mixer.Add(Mixer);
	Mixer->UpdateVolume();
	ApplyOptions(Mixer);
	m_CrossfaderBar.AddSource(Mixer);
}

int CMultiMix::FindMixer(CMixereView *Mixer) const
{
	for (int i = 0; i < GetMixerCount(); i++) {
		if (m_Mixer[i] == Mixer)
			return(i);
	}
	return(-1);
}

bool CMultiMix::RemoveMixer(CMixereView *Mixer)
{
	int	pos = FindMixer(Mixer);
	if (pos < 0)
		return(FALSE);
	m_Mixer.RemoveAt(pos);
	m_CrossfaderBar.RemoveSource(Mixer);
	return(TRUE);
}

bool CMultiMix::UpdateMixerName(CMixereView *Mixer)
{
	int	pos = FindMixer(Mixer);
	if (pos < 0)
		return(FALSE);
	m_CrossfaderBar.UpdateSourceName(Mixer);
	return(TRUE);
}

CMixereView *CMultiMix::GetMixer(int Index)
{
	ASSERT(Index >= 0 && Index < GetMixerCount());
	return(m_Mixer[Index]);
}

CMixereView *CMultiMix::GetActiveMixer()
{
	// Get the active MDI child window.
	CMDIChildWnd *pChild = m_Frm->MDIGetActive();
	if (pChild == NULL)
		return(NULL);
	// Get the active view attached to the active MDI child window.
	CMixereView	*pView = (CMixereView *)pChild->GetActiveView();
	return(pView);
}

void CMultiMix::TimerHook()
{
	for (int i = 0; i < GetMixerCount(); i++)
		m_Mixer[i]->TimerHook();
	m_MasterVolBar.TimerHook();
	m_CrossfaderBar.TimerHook();
}

void CMultiMix::ShowOptionsDlg()
{
	if (m_OptionsDlg.DoModal() == IDOK) {
		for (int i = 0; i < GetMixerCount(); i++)
			ApplyOptions(m_Mixer[i]);
		EnableToolTips(m_OptionsDlg.ShowToolTips());
	}
}

void CMultiMix::ApplyOptions(CMixereView *Mixer)
{
	Mixer->EnableToolTips(m_OptionsDlg.ShowToolTips());
	Mixer->GetUndoManager()->SetLevels(m_OptionsDlg.GetUndoLevels());
	Mixer->SetTitleFont(&m_OptionsDlg.GetFont());
	if (m_OptionsDlg.ColorWasChanged())	// don't repaint channels needlessly
		Mixer->ApplyColorScheme();
}

BOOL CMultiMix::EnableToolTips(BOOL bEnable)
{
	m_Frm->GetToolBar()->EnableToolTips(bEnable);
	for (int i = 0; i < DLGBARS; i++)
		m_DlgBar[i]->EnableToolTips(bEnable);
	return(bEnable);
}

void CMultiMix::SetMasterVolume(float Volume)
{
	m_MasterVolBar.SetVolume(Volume);
	UpdateVolume();
}

float CMultiMix::GetMasterVolume(CMixereView *Mixer)
{
	return(m_MasterVolBar.GetVolume() * m_CrossfaderBar.GetVolume(Mixer));
}

void CMultiMix::UpdateVolume()
{
	for (int i = 0; i < GetMixerCount(); i++)
		m_Mixer[i]->UpdateVolume();
}

bool CMultiMix::IsDlgBarVisible(int BarIdx) const
{
	ASSERT(BarIdx >= 0 && BarIdx < DLGBARS);
	return(m_DlgBar[BarIdx]->IsWindowVisible() != 0);
}

void CMultiMix::ShowDlgBar(int BarIdx, bool Enable)
{
	ASSERT(BarIdx >= 0 && BarIdx < DLGBARS);
	m_Frm->ShowControlBar(m_DlgBar[BarIdx], Enable, 0);
}

void CMultiMix::ToggleDlgBar(int BarIdx)
{
	ShowDlgBar(BarIdx, !IsDlgBarVisible(BarIdx));
}

bool CMultiMix::GetAutoBar(LPCTSTR Entry, CAutoSliderDlgBar& Bar)
{
	CAutoInfo	Info;
	DWORD	Size = sizeof(CAutoInfo);
	if (CPersist::GetBinary(REG_SETTINGS, Entry, &Info, &Size)) {
		Bar.SetInfo(Info);
		return(TRUE);
	}
	return(FALSE);
}

void CMultiMix::WriteAutoBar(LPCTSTR Entry, const CAutoSliderDlgBar& Bar)
{
	CAutoInfo	Info;
	Bar.GetInfo(Info);
	CPersist::WriteBinary(REG_SETTINGS, Entry, &Info, sizeof(CAutoInfo));
}

CMultiMix::CMasterVolBar::CMasterVolBar(CMultiMix *MultiMix)
	: m_MultiMix(MultiMix)
{
}

void CMultiMix::CMasterVolBar::OnPosChange()
{
	m_MultiMix->UpdateVolume();
}

CMultiMix::CMyCrossfaderBar::CMyCrossfaderBar(CMultiMix *MultiMix)
	: m_MultiMix(MultiMix)
{
}

LPCSTR CMultiMix::CMyCrossfaderBar::GetSourceName(PVOID Source)
{
	return(((CMixereView *)Source)->GetDocument()->GetTitle());
}

void CMultiMix::CMyCrossfaderBar::OnPosChange()
{
	m_MultiMix->UpdateVolume();
}

