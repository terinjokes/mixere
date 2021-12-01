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
		02		11may04	add choose font dialog
		03		12may04	make font setting persistent
		04		14may04	check GetSafeHandle before GetLogFont or debug asserts
		05		18nov04	add choose color dialog
		06		19nov04	on first run, add default selected color to custom
		07		04jan05	split options dialog into property pages

		appearance options property page
		
*/

// OptsAppearDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "OptsAppearDlg.h"
#include "Channel.h"
#include "Persist.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptsAppearDlg property page

IMPLEMENT_DYNCREATE(COptsAppearDlg, CPropertyPage)

LPCSTR COptsAppearDlg::TITLE_FONT	= "TitleFont";
LPCSTR COptsAppearDlg::COLOR_PREFIX	= "Color";
LPCSTR COptsAppearDlg::CUSTOM_COLOR	= "CustomColor";

const LPCSTR COptsAppearDlg::m_ColorName[COLORS] = {
	"Selected",
	"Active"
};

COptsAppearDlg::COptsAppearDlg() : CPropertyPage(COptsAppearDlg::IDD)
{
	//{{AFX_DATA_INIT(COptsAppearDlg)
	//}}AFX_DATA_INIT
	CPersist::GetFont(REG_SETTINGS, TITLE_FONT, &m_Font);
	m_ColorIdx = 0;
	m_ColorWasChanged = FALSE;
	GetColorState();
}

COptsAppearDlg::~COptsAppearDlg()
{
	CPersist::WriteFont(REG_SETTINGS, TITLE_FONT, &m_Font);
	WriteColorState();
}

void COptsAppearDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptsAppearDlg)
	DDX_Control(pDX, IDC_FONT_SAMPLE, m_FontSample);
	DDX_Control(pDX, IDC_COLOR_SAMPLE, m_ColorSample);
	DDX_Control(pDX, IDC_COLOR_LIST, m_ColorList);
	//}}AFX_DATA_MAP
}

void COptsAppearDlg::GetColorState()
{
	m_ColorWasChanged = FALSE;
	memset(m_Color, 0, COLOR_SIZE);	// init color list to black
	int i;
	for (i = 0; i < MAX_CUSTOM_COLORS; i++)	// init custom colors to white
		m_CustomColor[i] = RGB(255, 255, 255);
	GetChannelColors();	// copy channel's default color scheme into m_Color
	m_CustomColor[0] = m_Color[SELECTED];	// default selected color is custom
	for (i = 0; i < COLORS; i++) {		// retrieve color list from registry
		m_Color[i] = CPersist::GetInt(REG_SETTINGS, 
			CString(COLOR_PREFIX) + m_ColorName[i], m_Color[i]);
	}
	DWORD	ccs = CUST_CLR_SIZE;	// retrieve custom colors from registry
	CPersist::GetBinary(REG_SETTINGS, CUSTOM_COLOR, m_CustomColor, &ccs);
	SetChannelColors();	// update channel color scheme from m_Color
}

void COptsAppearDlg::WriteColorState()
{
	for (int i = 0; i < COLORS; i++) {
		CPersist::WriteInt(REG_SETTINGS, 
			CString(COLOR_PREFIX) + m_ColorName[i], m_Color[i]);
	}
	CPersist::WriteBinary(REG_SETTINGS, CUSTOM_COLOR, m_CustomColor, CUST_CLR_SIZE);
}

void COptsAppearDlg::GetChannelColors()
{
	CChannel::COLOR_SCHEME	Scheme;
	CChannel::GetColorScheme(Scheme);
	m_Color[SELECTED] = Scheme.Selected;
	m_Color[ACTIVE] = Scheme.Active;
}

void COptsAppearDlg::SetChannelColors()
{
	CChannel::COLOR_SCHEME	Scheme;
	Scheme.Selected = m_Color[SELECTED];
	Scheme.Active = m_Color[ACTIVE];
	CChannel::SetColorScheme(Scheme);
}

void COptsAppearDlg::UpdateColors(bool Save)
{
	if (Save) {
		m_ColorIdx = m_ColorList.GetCurSel();
		m_ColorWasChanged = memcmp(m_Color, m_TempColor, COLOR_SIZE) != 0;
		memcpy(m_Color, m_TempColor, COLOR_SIZE);
		memcpy(m_CustomColor, m_TempCustomColor, CUST_CLR_SIZE);
		SetChannelColors();	// update channel color scheme from m_Color
	} else {
		m_ColorList.SetCurSel(m_ColorIdx);
		m_ColorWasChanged = FALSE;
		memcpy(m_TempColor, m_Color, COLOR_SIZE);
		memcpy(m_TempCustomColor, m_CustomColor, CUST_CLR_SIZE);
	}
}

void COptsAppearDlg::CopyFont(CFont& dst, CFont& src)
{
	dst.Detach();	// detach before create or debug version asserts
	LOGFONT	lf;
	if (src.GetSafeHandle() && src.GetLogFont(&lf))
		dst.CreateFontIndirect(&lf);
}

void COptsAppearDlg::UpdateFontSample()
{
	CFont	*Font = &m_TempFont, tmp;
	if (!Font->GetSafeHandle()) {
		tmp.CreateStockObject(DEFAULT_GUI_FONT);
		Font = &tmp;
	}
	LOGFONT	lf;
	Font->GetLogFont(&lf);
	m_FontSample.SetFont(Font);
	CString	s;
	s.Format("%d pt %s", HeightToPointSize(lf.lfHeight), lf.lfFaceName);
	m_FontSample.SetWindowText(s);
}

int COptsAppearDlg::HeightToPointSize(int Height)
{
	CDC	*pDC = GetDC();
	int	Points = 0;
	if (pDC != NULL) {
		Points = int(-Height * 72.0 / pDC->GetDeviceCaps(LOGPIXELSY) + 0.5);
		ReleaseDC(pDC);
	}
	return(Points);
}

BEGIN_MESSAGE_MAP(COptsAppearDlg, CPropertyPage)
	//{{AFX_MSG_MAP(COptsAppearDlg)
	ON_BN_CLICKED(IDC_CHOOSE_FONT, OnChooseFont)
	ON_BN_CLICKED(IDC_DEFAULT_FONT, OnDefaultFont)
	ON_BN_CLICKED(IDC_CHOOSE_COLOR, OnChooseColor)
	ON_WM_CTLCOLOR()
	ON_CBN_SELCHANGE(IDC_COLOR_LIST, OnSelchangeColorList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptsAppearDlg message handlers

BOOL COptsAppearDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	CopyFont(m_TempFont, m_Font);	// copy actual font to temp font
	UpdateFontSample();

	for (int i = 0; i < COLORS; i++)	// add color names to combo box
		m_ColorList.AddString(m_ColorName[i]);
	UpdateColors(FALSE);	// copy actual colors to temp colors

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptsAppearDlg::OnOK() 
{
	CopyFont(m_Font, m_TempFont);	// copy temp font to actual font

	UpdateColors(TRUE);	// copy temp colors to actual colors

	CPropertyPage::OnOK();
}

void COptsAppearDlg::OnChooseFont() 
{
	LOGFONT	lf;
	CFontDialog	dlg(m_TempFont.GetSafeHandle() 
		&& m_TempFont.GetLogFont(&lf) ? &lf : NULL);
	if (dlg.DoModal() == IDOK) {
		dlg.GetCurrentFont(&lf);
		m_TempFont.Detach();	// detach before create or debug version asserts
		m_TempFont.CreateFontIndirect(&lf);
		UpdateFontSample();
	}
}

void COptsAppearDlg::OnDefaultFont() 
{
	m_TempFont.Detach();
	UpdateFontSample();
}

void COptsAppearDlg::OnChooseColor() 
{
	int	ci = m_ColorList.GetCurSel();
	if (ci < 0 || ci >= COLORS)
		return;
	CColorDialog	dlg(m_TempColor[ci]);
	memcpy(dlg.m_cc.lpCustColors, m_TempCustomColor, CUST_CLR_SIZE);
	if (dlg.DoModal() == IDOK) {
		m_TempColor[ci] = dlg.GetColor();
		memcpy(m_TempCustomColor, dlg.m_cc.lpCustColors, CUST_CLR_SIZE);
		m_ColorSample.Invalidate();
	}
}

HBRUSH COptsAppearDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);

	static	CBrush	b;
	if (pWnd == &m_ColorSample) {
		b.Detach();
		b.CreateSolidBrush(m_TempColor[m_ColorList.GetCurSel()]);
		return(b);
	}

	return hbr;
}

void COptsAppearDlg::OnSelchangeColorList() 
{
	m_ColorSample.Invalidate();
}
