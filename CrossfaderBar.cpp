// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      28jan04 initial version
		01		01feb04	fix arrows and page up/down
		02		07may04	derive from auto slider dialog
		03		07may04	prevent same source on both sides
		04		02oct04	convert to control bar
		05		20jan05	add UpdateSourceName and UpdateString

		crossfader dialog
 
*/

// CrossfaderBar.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "CrossfaderBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCrossfaderBar dialog

CCrossfaderBar::CCrossfaderBar()
	: CAutoSliderDlgBar(NULL)
{
	//{{AFX_DATA_INIT(CCrossfaderBar)
	//}}AFX_DATA_INIT
	m_Src[SRC_A] = &m_SrcA;
	m_Src[SRC_B] = &m_SrcB;
	m_Sel[SRC_A] = 0;
	m_Sel[SRC_B] = 0;
	m_FaderType = FADER_LINEAR;
}

void CCrossfaderBar::AddSource(PVOID Source)
{
	ASSERT(Source != NULL);
	for (int i = 0; i < SOURCES; i++) {
		int	pos = m_Src[i]->AddString(GetSourceName(Source));
		m_Src[i]->SetItemDataPtr(pos, Source);
	}
}

void CCrossfaderBar::RemoveSource(PVOID Source)
{
	for (int i = 0; i < SOURCES; i++) {
		int	SourceIdx = m_Src[i]->FindItemDataPtr(Source);
		ASSERT(SourceIdx >= 0);
		if (m_Src[i]->GetCurSel() == SourceIdx) {	// if removing current selection
			int	NoInputIdx = m_Src[i]->FindItemDataPtr(0);	// find no input
			ASSERT(NoInputIdx >= 0);
			m_Src[i]->SetCurSel(NoInputIdx);				// select no input
			m_Sel[i] = NULL;
		}
		m_Src[i]->DeleteString(SourceIdx);
	}
}

void CCrossfaderBar::UpdateSourceName(PVOID Source)
{
	for (int i = 0; i < SOURCES; i++) {
		int	SourceIdx = m_Src[i]->FindItemDataPtr(Source);
		ASSERT(SourceIdx >= 0);
		m_Src[i]->UpdateString(SourceIdx, GetSourceName(Source));
	}
}

float CCrossfaderBar::GetVolume(PVOID Source) const
{
	if (Source == m_Sel[SRC_A])
		return(ApplyCurve(1 - GetPos()));
	if (Source == m_Sel[SRC_B])
		return(ApplyCurve(GetPos()));
	return(1);
}

void CCrossfaderBar::SetSource(int Idx)
{
	ASSERT(Idx >= 0 && Idx < SOURCES);
	m_Sel[Idx] = m_Src[Idx]->GetItemDataPtr(m_Src[Idx]->GetCurSel());
	if (m_Sel[SRC_A] == m_Sel[SRC_B]) {	// if same source on both sides
		int	Other = Idx ^ 1;				// disconnect other side
		m_Src[Other]->SetCurSel(m_Src[Other]->FindItemDataPtr(0));
		m_Sel[Other] = NULL;
	}
}

LPCSTR CCrossfaderBar::GetSourceName(PVOID Source)
{
	return("");
}

void CCrossfaderBar::SetFaderType(int FaderType)
{
	m_FaderType = FaderType;
	m_FaderTypeButton.SetCheck(m_FaderType);
}

void CCrossfaderBar::DoDataExchange(CDataExchange* pDX)
{
	CAutoSliderDlgBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCrossfaderBar)
	DDX_Control(pDX, IDC_FADER_TYPE, m_FaderTypeButton);
	DDX_Control(pDX, IDC_SOURCE_A, m_SrcA);
	DDX_Control(pDX, IDC_SOURCE_B, m_SrcB);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCrossfaderBar, CAutoSliderDlgBar)
	//{{AFX_MSG_MAP(CCrossfaderBar)
	ON_CBN_SELCHANGE(IDC_SOURCE_A, OnSelchangeSourceA)
	ON_CBN_SELCHANGE(IDC_SOURCE_B, OnSelchangeSourceB)
	ON_BN_CLICKED(IDC_FADER_TYPE, OnFaderType)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_INITDIALOG, OnInitDialog)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCrossfaderBar message handlers

LRESULT CCrossfaderBar::OnInitDialog(WPARAM wParam, LPARAM lParam)
{
	CAutoSliderDlgBar::OnInitDialog(wParam, lParam);

	SetWindowText(LDS(CROSSFADER));
	SetBarCaption(LDS(CROSSFADER_CAPTION));
	m_Slider.SetDefaultPos(m_Slider.GetRangeMax() / 2);
	m_SrcA.SetCurSel(m_SrcA.AddString(LDS(NO_INPUT)));
	m_SrcB.SetCurSel(m_SrcB.AddString(LDS(NO_INPUT)));
	m_FaderTypeButton.SetIcons(IDI_CFLINEAR, IDI_CFDIPLESS);
	// add additional controls to base class resizing object
	m_Resize.AddControl(IDC_SOURCE_A, BIND_LEFT);
	m_Resize.AddControl(IDC_SOURCE_B, BIND_RIGHT);
	m_Resize.AddControl(IDC_FADER_TYPE, BIND_RIGHT);
	m_Resize.FixControls();


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCrossfaderBar::OnSelchangeSourceA() 
{
	SetSource(SRC_A);
	OnPosChange();
}

void CCrossfaderBar::OnSelchangeSourceB() 
{
	SetSource(SRC_B);
	OnPosChange();
}

void CCrossfaderBar::OnFaderType() 
{
	m_FaderType ^= 1;
	OnPosChange();
}

int CCrossfaderBar::CMyComboBox::FindItemDataPtr(PVOID pData) const
{
	for (int i = 0; i < GetCount(); i++) {
		if (GetItemDataPtr(i) == pData)
			return(i);
	}
	return(-1);
}

int CCrossfaderBar::CMyComboBox::UpdateString(int nIndex, LPCTSTR lpszString)
{
	PVOID	pCurSel = GetItemDataPtr(GetCurSel());
	PVOID	pData = GetItemDataPtr(nIndex);
	if (DeleteString(nIndex) == CB_ERR)
		return(CB_ERR);
	int	pos = AddString(lpszString);
	SetItemDataPtr(pos, pData);
	SetCurSel(FindItemDataPtr(pCurSel));
	return(pos);
}
