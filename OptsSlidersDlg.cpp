// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      11dec03 initial version
		01		09feb04	save and restore state in registry
		02		19apr04	add animation speed
		03		22dec04	add checkbox for automation slider quick start
		04		04jan05	split options dialog into property pages
		05		25feb05	add option for auto-trigger turns off audio looping

		sliders options property page
		
*/

// OptsSlidersDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "OptsSlidersDlg.h"
#include "Persist.h"
#include "Channel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptsSlidersDlg property page

IMPLEMENT_DYNCREATE(COptsSlidersDlg, CPropertyPage)

LPCSTR COptsSlidersDlg::ANIM_SPEED		= "AnimSpeed";
LPCSTR COptsSlidersDlg::AUTO_OPTIONS	= "AutoOptions";
LPCSTR COptsSlidersDlg::CHAN_OPTIONS	= "ChanOptions";

COptsSlidersDlg::COptsSlidersDlg() : CPropertyPage(COptsSlidersDlg::IDD)
{
	//{{AFX_DATA_INIT(COptsSlidersDlg)
	//}}AFX_DATA_INIT
	m_AnimSpeed = CPersist::GetInt(REG_SETTINGS, ANIM_SPEED, MAX_ANIM_SPEED);
	CAutoSliderCtrl::SetAnimDelay(MAX_ANIM_SPEED - m_AnimSpeed);
	m_AutoOptions = CPersist::GetInt(REG_SETTINGS, AUTO_OPTIONS,
		CAutoSliderCtrl::DEFAULT_OPTIONS);
	CAutoSliderCtrl::SetOptions(m_AutoOptions);
	m_ChanOptions = CPersist::GetInt(REG_SETTINGS, CHAN_OPTIONS,
		CChannel::DEFAULT_OPTIONS);
	CChannel::SetOptions(m_ChanOptions);
}

COptsSlidersDlg::~COptsSlidersDlg()
{
	CPersist::WriteInt(REG_SETTINGS, ANIM_SPEED, m_AnimSpeed);
	CPersist::WriteInt(REG_SETTINGS, AUTO_OPTIONS, m_AutoOptions);
	CPersist::WriteInt(REG_SETTINGS, CHAN_OPTIONS, m_ChanOptions);
}

void COptsSlidersDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptsSlidersDlg)
	DDX_Control(pDX, IDC_ANIM_SPEED_MIN, m_AnimSpeedSliderMin);
	DDX_Control(pDX, IDC_ANIM_SPEED_MAX, m_AnimSpeedSliderMax);
	DDX_Control(pDX, IDC_ANIM_SPEED, m_AnimSpeedSlider);
	DDX_Control(pDX, IDC_AUTO_VOL_UNPAUSE, m_AutoVolUnpause);
	DDX_Control(pDX, IDC_AUTO_MIDDLE_CLICK, m_AutoMClick);
	DDX_Control(pDX, IDC_AUTO_TRIG_LOOP_OFF, m_AutoTrigLoopOff);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptsSlidersDlg, CPropertyPage)
	//{{AFX_MSG_MAP(COptsSlidersDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptsSlidersDlg message handlers

BOOL COptsSlidersDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_AnimSpeedSlider.SetRange(1, MAX_ANIM_SPEED);
	m_AnimSpeedSlider.SetBuddy(&m_AnimSpeedSliderMin, TRUE);
	m_AnimSpeedSlider.SetBuddy(&m_AnimSpeedSliderMax, FALSE);
	m_AnimSpeedSlider.SetDefaultPos(MAX_ANIM_SPEED);
	m_AnimSpeedSlider.SetPos(m_AnimSpeed);

	m_AutoMClick.SetCheck(m_AutoOptions & CAutoSliderCtrl::MCLICK_SHOWS_DLG);

	m_AutoVolUnpause.SetCheck(m_ChanOptions & CChannel::AUTO_VOL_UNPAUSE);
	m_AutoTrigLoopOff.SetCheck(m_ChanOptions & CChannel::AUTO_TRIG_LOOP_OFF);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptsSlidersDlg::OnOK() 
{
	m_AnimSpeed	= m_AnimSpeedSlider.GetPos();
	CAutoSliderCtrl::SetAnimDelay(MAX_ANIM_SPEED - m_AnimSpeed);
	
	m_AutoOptions = 
		(m_AutoMClick.GetCheck() ? CAutoSliderCtrl::MCLICK_SHOWS_DLG: 0);
	CAutoSliderCtrl::SetOptions(m_AutoOptions);

	m_ChanOptions = 
		(m_AutoVolUnpause.GetCheck() ? CChannel::AUTO_VOL_UNPAUSE : 0) |
		(m_AutoTrigLoopOff.GetCheck() ? CChannel::AUTO_TRIG_LOOP_OFF : 0);
	CChannel::SetOptions(m_ChanOptions);

	CPropertyPage::OnOK();
}
