// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda

		revision history:
		rev		date	comments
        00      21nov03 initial version
		01		04dec03	use CStringArray to pass paths to link check 
		02		05dec03	add master volume
		03		11dec03	add options dialog
		04		26dec03 bump file version to 2 for automation
		05		11jan04	ResetMixer and Serialize must reset header ctrl in SDI
		06		17jan04	add mute/solo
		07		18jan04	move serialize to mixer info object
		08		19jan04	convert to MDI
		09		20jan04	add get extension and drop files
		10		22jan04	move editing stuff here
		11		26jan04	add automation editor
		12		26jan04	add GetChan accessor
		13		27jan04	add HasToolTips accessor
		14		31jan04	sort on title instead of file name
		15		08feb04	include channel defaults in mixer info
		16		10feb04	include column widths in mixer info
		17		11feb04	mixer set info can fail now
		18		11feb04	add recovery from create channel failure
		19		11feb04	make channel visible with ShowWindow, not ModifyStyle
		20		13feb04	header control is a member of mixer now
		21		17feb04	create volume dialog in Create, not ctor
		22		22feb04	add undo
		23		22feb04	in OnDestroy, check created flag before setting volume
		24		10mar04	add tempo
		25		12mar04	add edit name
		26		24mar04	display error message if SetInfo can't open audio files
		27		26mar04	add keep solo
		28		16apr04	add mute/solo time dialog
		29		19apr04	add CreateDlg
		30		19apr04	OnMSFade was testing tempo dialog's IsVisible
		31		10may04	mute/solo time dialog wasn't getting enable tooltips
		32		11may04	add set title font
		33		14may04 in OnSolo, soloed channel now controls soft muting enable
		34		24jun04	auto slider's SetInfo sets position and selection now 
		35		25sep04	can't zero volume in OnDestroy, bars already destroyed
		36		28sep04	add dock state to set/get info
		37		01oct04	combine with mixer object
		38		01oct04 horizontal docking only, else header control loses sync
		39		02oct04 lost implementation of get master volume, put it back
		40		03oct04	after restoring bar state, must reapply our tip state
		41		18nov04	add color scheme
		42		19nov04	allow set defaults on empty channel
		43		24dec04	add multi auto dlg to timer hook
		44		29dec04	use SetInfoNotify in undo auto edit so audio gets updated
		45		03jan05	append file version to clipboard format ID string
		46		04jan05	undo play from pause wasn't restoring audio positions
		47		04jan05	verify bar state before restoring it
		48		06jan05	restore transport undo state with SetTransportAndPos
		49		09jan05	add go
		50		10jan05	add channel ID
		51		13jan05	SetInfo must enable status bar mute/solo indicators
		52		14jan05	number column header now sorts by channel ID
		53		16jan05	add snapshots
		54		20jan05	add OnSetDocTitle to handle document title changes
		55		31jan05	FindUndoable must check for control bar window IDs
		56		01feb05	add snapshot undo codes to GetUndoTitle
		57		03feb05	in EditAuto, if not modified, cancel edit
		58		05feb05	add snapshot hot keys
		59		07feb05	add find dialog
		60		24feb05	change find implementation to allow FindNext
		
		mixer view

*/

// MixereView.cpp : implementation of the CMixereView class
//

#include "stdafx.h"
#include "Resource.h"

#include "ChildFrm.h"
#include "MixereDoc.h"
#include "MixereView.h"

#include "PropertiesDlg.h"
#include "Channel.h"	// for transport enum
#include "MainFrm.h"
#include "MultiMix.h"
#include "MultiAutoDlg.h"
#include "Shlwapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMixereView

const CColumnResizer::COLDEF CMixereView::m_ColumnInfo[COLUMNS] = {
//	name ID				desc	defwid	minwid	members
	{IDS_CH_NUMBER,		FALSE,	25,		25},			// number
	{IDS_CH_NAME,		FALSE,	165,	0},				// name
	{IDS_CH_TRANSPORT,	TRUE,	100,	0,		4},		// play, pause, stop, loop
	{IDS_CH_MUTE_SOLO,	TRUE,	50,		0,		2},		// mute, solo
	{IDS_CH_VOLUME,		TRUE,	100,	0},				// volume
	{IDS_CH_PAN,		TRUE,	100,	0},				// pan
	{IDS_CH_PITCH,		TRUE,	100,	0},				// pitch
	{IDS_CH_POSITION,	TRUE,	100,	0}				// position
};

const int CMixereView::m_UndoTitleID[UNDO_CODES] = {	// must match undo code enum
	IDS_MU_TRANSPORT,
	IDS_MU_GO,
	IDS_MU_LOOP,
	IDS_MU_MUTE,
	IDS_MU_SOLO,
	IDS_MU_END_SOLO,
	IDS_MU_KEEP_SOLO,
	IDS_MU_LOAD_AUDIO,
	IDS_MU_INSERT_AUDIO,
	IDS_MU_SHOW_VOLUME,
	IDS_MU_SHOW_SNAPSHOT,
	IDS_MU_SHOW_MS_FADE,
	IDS_MU_SHOW_TEMPO,
	IDS_MU_SET_DEFAULTS,
	IDS_MU_EDIT_AUTO
};

// must match dialog bar enum; order determines initial docking order
const CMixereView::DLGBAR_INFO CMixereView::m_DlgBarInfo[DLGBARS] = {
	{IDD_VOLUME_BAR,	UCODE_SHOW_VOLUME},
	{IDD_SNAPSHOT_BAR,	UCODE_SHOW_SNAPSHOT},
	{IDD_MS_FADE_BAR,	UCODE_SHOW_MS_FADE},
	{IDD_TEMPO_BAR,		UCODE_SHOW_TEMPO}
};

const UINT CMixereView::WM_FINDREPLACE = RegisterWindowMessage(FINDMSGSTRING);

IMPLEMENT_DYNCREATE(CMixereView, CFormListView)

/////////////////////////////////////////////////////////////////////////////
// CMixereView construction/destruction

CMixereView::CMixereView() :
#pragma warning(disable : 4355)	// 'this' used in base member initializer list
	// these ctors don't access 'this', they only copy it to a member var
	m_VolumeBar(this),
	m_TempoBar(this),
	m_MSFadeBar(this),
	m_SnapshotBar(this)
#pragma warning(default : 4355)
{
	//{{AFX_DATA_INIT(CMixereView)
	//}}AFX_DATA_INIT
	CString	ClipboardID;
	ClipboardID.Format("Mixere%d\n", CMixerInfo::FILE_VERSION);
	InitView(m_ColumnInfo, COLUMNS, sizeof(CChanInfo), ClipboardID);
	m_DlgBar[DLGBAR_VOLUME] = &m_VolumeBar;
	m_DlgBar[DLGBAR_SNAPSHOT] = &m_SnapshotBar;
	m_DlgBar[DLGBAR_MS_FADE] = &m_MSFadeBar;
	m_DlgBar[DLGBAR_TEMPO] = &m_TempoBar;
	m_ChildFrm = NULL;
	m_MultiMix = ((CMainFrame *)AfxGetMainWnd())->GetMultiMix();
	m_MultiAutoDlg = NULL;
	m_Snapshot = NULL;
	m_HasToolTips = FALSE;
	m_MuteCount = 0;
	m_SoloCount = 0;
	m_Tempo = 1;
	m_ChanIDs = 0;
	m_UndoIdx = 0;
	m_PrevPos = 0;
	m_FindDlg = NULL;
	m_NewFind = FALSE;
	m_FindMatchCase = FALSE;
	m_FindSearchDown = FALSE;
}

CMixereView::~CMixereView()
{
	m_MultiMix->RemoveMixer(this);
	if (m_Snapshot != NULL)	// in case document was invalid
		m_Snapshot->SetMixer(NULL);
	if (m_FindDlg != NULL)
		m_FindDlg->SendMessage(WM_CLOSE);
}

void CMixereView::OnCreateView()
{
	m_ChildFrm = GetParentFrame();	// must do this first
	// create dialog bars and dock them in a row
	CreateDlgBars();
	// register with mixer manager
	m_MultiMix->AddMixer(this);
	// copy the document's data into the mixer
	bool	retc = SetInfo(GetDocument()->m_MixerInfo);
	if (!retc) {	// item creation failed; probably one too many docs open
		SetItemCount(0);	// free up resources, else message box could fail
		AfxMessageBox(LDS(CANT_CREATE_VIEW));
		GetDocument()->Close();
	}
	DragAcceptFiles(TRUE);	// so we can drop files over view background
}

void CMixereView::CreateDlgBars()
{
	// create dialog bars and dock them in a row
	int	DockStyle = CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM;	// horizontal only
	CControlBar	*LeftOf = NULL;
	for (int i = 0; i < DLGBARS; i++) {
		CDialogBarEx	*Bar = m_DlgBar[i];
		Bar->Create(m_ChildFrm, m_DlgBarInfo[i].ResID, 0, DockStyle, i, LeftOf);
		LeftOf = Bar;
		Bar->GetWindowText(m_DlgBarTitle[i]);	// save initial bar title
		Bar->SetOwner(this);		// link to undo tree
	}
	UpdateDlgBarTitles();
	// now hide docked bars in case frame can't restore dock state
	for (i = 0; i < DLGBARS; i++)
		m_ChildFrm->ShowControlBar(m_DlgBar[i], FALSE, 0);
}

void CMixereView::UpdateDlgBarTitles()
{
	for (int i = 0; i < DLGBARS; i++)
		m_DlgBar[i]->SetWindowText(	// append document title to initial title
			m_DlgBarTitle[i] + " - " + GetDocument()->GetTitle());
}

/////////////////////////////////////////////////////////////////////////////
// CMixereView serialization

void CMixereView::GetInfo(CMixerInfo& Info) const
{
	int	cols = COLUMNS;
	Info.m_ColumnWidth.SetSize(cols);
	for (int i = 0; i < cols; i++)
		Info.m_ColumnWidth[i] = GetColumnWidth(i);
	int	chans = GetItemCount();
	Info.m_Chan.SetSize(chans);
	for (i = 0; i < chans; i++)
		GetChan(i)->GetInfo(&Info.m_Chan[i]);
	m_VolumeBar.GetInfo(Info.m_AutoVol);
	m_TempoBar.GetInfo(Info.m_AutoTempo);
	m_MSFadeBar.GetInfo(Info.m_MSFade);
	m_ChildFrm->GetDockState(Info.m_DockState);
	Info.m_ChanDefaults = m_ChanDefaults;
	Info.m_ChanIDs = m_ChanIDs;
}

bool CMixereView::SetInfo(const CMixerInfo& Info)
{
	CWaitCursor	wc;
	m_MSFadeBar.SetInfo(Info.m_MSFade);	// before creating channels
	// set the column widths
	int	cols = Info.m_ColumnWidth.GetSize();
	for (int i = 0; i < cols; i++)
		SetColumnWidth(i, Info.m_ColumnWidth[i]);
	// set the channel count
	int	chans = Info.m_Chan.GetSize();
	if (SetItemCount(chans) < chans)	// if create failed, bail out
		return(FALSE);
	// set the mixer controls
	CStringArray	ErrPath;
	for (i = 0; i < chans; i++) {
		if (!GetChan(i)->OpenItem(&Info.m_Chan[i]))		// if can't load audio file
			AddStringUnique(ErrPath, Info.m_Chan[i].m_Path);	// add its path to list
	}
	// if audio files couldn't be loaded, display error message
	if (ErrPath.GetSize())
		MsgBoxStrList(LDS(CANT_LOAD_AUDIO), ErrPath);
	m_VolumeBar.SetInfo(Info.m_AutoVol);
	m_TempoBar.SetInfo(Info.m_AutoTempo);
	m_ChanDefaults = Info.m_ChanDefaults;
	m_ChanIDs = Info.m_ChanIDs;
	// redraw channels all at once, looks and sounds better
	for (i = 0; i < chans; i++)
		GetChan(i)->SetInfo(&Info.m_Chan[i]);
	ScrollToPosition(CPoint(0, 0));
	ClearSelection();
	SetCurPos(0);
	ClearUndoHistory();
	// restore our bar state
	if (CMainFrame::VerifyDockState(Info.m_DockState, m_ChildFrm))
		m_ChildFrm->SetDockState(Info.m_DockState);
	// restoring bar state enables bar tool tips; reapply our tip state
	for (i = 0; i < DLGBARS; i++)
		m_DlgBar[i]->EnableToolTips(m_HasToolTips);
	// attach to snapshot object and update our snapshot bar
	m_Snapshot = const_cast<CSnapshot *>(&Info.m_Snapshot);
	m_Snapshot->SetMixer(this);
	m_Snapshot->UpdateList();
	// enable status bar panes as needed; see note in CColorStatusBar::Reset
	CColorStatusBar	*csb = ((CMainFrame *)AfxGetMainWnd())->GetStatusBar();
	if (GetMuteCount())
		csb->EnablePane(CMainFrame::SBP_MUTE);
	if (GetSoloCount())
		csb->EnablePane(CMainFrame::SBP_SOLO);
	return(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// CMixereView channel sorting

#define CMIXER_SORT(x) { retc = SortCmp(p1->x, p2->x); }

#define CMIXER_SORT2(a, b) { CMIXER_SORT(a); if (!retc) CMIXER_SORT(b); }

int CMixereView::MemberSortCompare(const void *elem1, const void *elem2)
{
	const CChannel *p1 = *((CChannel **)elem1);
	const CChannel *p2 = *((CChannel **)elem2);
	// empty channels sort last
	if (p1->IsEmpty())
		return(p2->IsEmpty() ? 0 : 1);
	if (p2->IsEmpty())
		return(p1->IsEmpty() ? 0 : -1);
	int	retc;
	switch (m_SortKey) {
	case COL_NUMBER:
		CMIXER_SORT(GetID());
		break;
	case COL_NAME:
		retc = stricmp(p1->GetTitle(), p2->GetTitle()) * m_SortDir;
		break;
	case COL_TRANSPORT:
		CMIXER_SORT(GetTransport());
		if (!retc)
			retc = stricmp(p1->GetTitle(), p2->GetTitle());
		break;
	case COL_MUTE:
		CMIXER_SORT2(GetMute(), GetSolo());
		break;
	case COL_VOLUME:
		CMIXER_SORT(GetVolume());
		break;
	case COL_PAN:
		CMIXER_SORT(GetPan());
		break;
	case COL_PITCH:
		CMIXER_SORT(GetPitch());
		break;
	case COL_POS:
		CMIXER_SORT(GetNormPosition());
		break;
	default:
		ASSERT(FALSE);	// unknown sort key
		return(0);
	}
	return(retc);
}

/////////////////////////////////////////////////////////////////////////////
// CMixereView channel operations

void CMixereView::SetTransport(int Transport)
{
	NotifyUndoableEdit(UCODE_TRANSPORT);
	for (int i = 0; i < GetItemCount(); i++) {
		if (IsSelOrCurAndFull(i))
			GetChan(i)->SetTransport(Transport);
	}
}

void CMixereView::TogglePlay()
{
	NotifyUndoableEdit(UCODE_TRANSPORT);
	for (int i = 0; i < GetItemCount(); i++) {
		if (IsSelOrCurAndFull(i))
			GetChan(i)->TogglePlay();
	}
}

void CMixereView::Go()
{
	NotifyUndoableEdit(UCODE_GO);
	for (int i = 0; i < GetItemCount(); i++) {
		if (IsSelOrCurAndFull(i))
			GetChan(i)->Go();
	}
}

void CMixereView::SetLoop(int State)
{
	NotifyUndoableEdit(UCODE_LOOP);
	for (int i = 0; i < GetItemCount(); i++) {
		if (IsSelOrCurAndFull(i))
			GetChan(i)->SetLoop(State < 0 ? !GetChan(i)->GetLoop() : State != 0);
	}
}

void CMixereView::SetMute(int State)
{
	NotifyUndoableEdit(UCODE_MUTE);
	for (int i = 0; i < GetItemCount(); i++) {
		if (IsSelOrCurAndFull(i))
			GetChan(i)->SetMute(State < 0 ? !GetChan(i)->GetMute() : State != 0);
	}
}

void CMixereView::SetSolo(int State)
{
	NotifyUndoableEdit(UCODE_SOLO);
	for (int i = 0; i < GetItemCount(); i++) {
		if (IsSelOrCurAndFull(i))
			GetChan(i)->SetSolo(State < 0 ? !GetChan(i)->GetSolo() : State != 0);
	}
}

void CMixereView::EndSolo()
{
	NotifyUndoableEdit(UCODE_END_SOLO);
	for (int i = 0; i < GetItemCount(); i++)
		GetChan(i)->SetSolo(FALSE);
}

void CMixereView::KeepSolo()
{
	NotifyUndoableEdit(UCODE_KEEP_SOLO);
	for (int i = 0; i < GetItemCount(); i++) {
		if (!IsEmpty(i)) {
			GetChan(i)->SetMute(!GetChan(i)->GetSolo());
			GetChan(i)->SetSolo(FALSE);
		}
	}
}

void CMixereView::OnMute(CChannel *Chan, bool Enable)
{
	m_MuteCount += Enable ? 1 : -1;
}

void CMixereView::OnSolo(CChannel *Chan, bool Enable)
{
	int	Prev = m_SoloCount;
	m_SoloCount += Enable ? 1 : -1;
	if (!Prev || !m_SoloCount) {
		for (int i = 0; i < GetItemCount(); i++) {
			// check for null pointer, edit could be in progress
			if (GetChan(i) != NULL && !GetChan(i)->IsEmpty()) {
				// solo channel determines whether we're soft muting
				GetChan(i)->ApplyMuteSolo(Chan->GetMSFadeEnable());
				GetChan(i)->UpdateVolume();
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMixereView audio files

bool CMixereView::LoadAudio(int Pos, LPCSTR Path)
{
	// undo notification must be deferred until after audio is loaded, so that
	// the undo handler can save the loaded channel's info; loading audio also
	// modifies the current position, so we must save it before loading audio
	m_PrevPos = m_CurPos;	// save current position for undo handler
	bool	retc = FALSE;	// assume failure
	int		UndoCode;
	if (Pos < GetItemCount() && IsEmpty(Pos)) {
		if (retc = GetChan(Pos)->LoadAudio(Path))
			UndoCode = UCODE_LOAD_AUDIO;
		else
			GetChan(Pos)->Reset();
	} else {
		if (Insert(Pos, NULL, 1)) {
			if (retc = GetChan(Pos)->LoadAudio(Path))
				UndoCode = UCODE_INSERT_AUDIO;
			else
				Delete(Pos, NULL, 1);
		}
	}
	if (retc) {	// if load succeeded
		SetCurPos(Pos);	// undo handler assumes current position
		NotifyUndoableEdit(UndoCode);
		SetCurPos(min(Pos + 1, GetItemCount() - 1));	// bump pos if possible
	}
	return(retc);
}

void CMixereView::LoadAudio()
{
	CFileDialog	fd(TRUE, ".wav", NULL,
		OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		CChannel::GetFileFilter());
	// prepare OPENFILENAME struct for multiple select
	CString	Buffer;
	const	BUFSIZE = 0x7fff;
	LPTSTR	FileBuf = Buffer.GetBufferSetLength(BUFSIZE);
	ZeroMemory(FileBuf, BUFSIZE);
	fd.m_ofn.lpstrFile = FileBuf;
	fd.m_ofn.nMaxFile = BUFSIZE;
	fd.m_ofn.nFileOffset = 0;
	CString	Title(LDS(LOAD_AUDIO));
	fd.m_ofn.lpstrTitle = Title;
	// display the dialog
	int	retc = fd.DoModal();
	int	Pos = m_CurPos;
	if (retc == IDOK) {
		// iterate through the results
		CStringArray	ErrPath;
		POSITION	FilePos;
        FilePos = fd.GetStartPosition();
        while (FilePos != NULL) {
			CString	Path = fd.GetNextPathName(FilePos);
			if (!LoadAudio(Pos++, Path))
				AddStringUnique(ErrPath, Path);
		}
		// if audio files couldn't be opened, display error message
		if (ErrPath.GetSize())
			MsgBoxStrList(LDS(CANT_LOAD_AUDIO), ErrPath);
	} else {
		if (CommDlgExtendedError())
			AfxMessageBox(LDS(FILE_DIALOG_ERROR));
	}
}

bool CMixereView::GetExtension(CString& Ext)
{
	return(GetDocument()->GetDocTemplate()->GetDocString(
		Ext, CDocTemplate::filterExt) != 0);
}

void CMixereView::OnDropFiles(CChannel *Chan, HDROP hDropInfo)
{
	char	Path[MAX_PATH];
	UINT	Files = DragQueryFile(hDropInfo, -1, 0, 0);
	DragQueryFile(hDropInfo, 0, Path, MAX_PATH);
	CString	MixerExt;
	GetExtension(MixerExt);
	CStringArray	ChanExt;
	CChannel::GetExtensionList(ChanExt);
	int	Pos = Chan != NULL ? Chan->GetIndex() : GetItemCount();
	// assume any non-mixer files are audio files
	CStringArray	LoadErr;
	CStringArray	FormatErr;
	for (UINT i = 0; i < Files; i++) {
		DragQueryFile(hDropInfo, i, Path, MAX_PATH);
		if (stricmp(PathFindExtension(Path), MixerExt)) {
			for (int j = 0; j < ChanExt.GetSize(); j++) {
				if (!stricmp(PathFindExtension(Path), ChanExt[j]))
					break;
			}
			if (j < ChanExt.GetSize()) {
				if (!LoadAudio(Pos++, Path))
					AddStringUnique(LoadErr, Path);
			} else
				AddStringUnique(FormatErr, Path);
		}
	}
	// if audio files couldn't be loaded, display error message
	if (LoadErr.GetSize())
		MsgBoxStrList(LDS(CANT_LOAD_AUDIO), LoadErr);
	// if audio files had invalid formats, display error message
	if (FormatErr.GetSize())
		MsgBoxStrList(LDS(BAD_FILE_FORMAT), FormatErr);
	// now open mixer files if any
	for (i = 0; i < Files; i++) {
		DragQueryFile(hDropInfo, i, Path, MAX_PATH);
		if (!stricmp(PathFindExtension(Path), MixerExt))
			AfxGetApp()->OpenDocumentFile(Path);
	}
}

void CMixereView::DropFiles(HDROP hDropInfo)
{
	OnDropFiles(NULL, hDropInfo);
}

/////////////////////////////////////////////////////////////////////////////
// CMixereView volume control

void CMixereView::SetVolume(float Volume)
{
	if (IsWindow(m_VolumeBar.m_hWnd))
		m_VolumeBar.SetVolume(Volume);
	UpdateVolume();
}

void CMixereView::UpdateVolume()
{
	for (int i = 0; i < GetItemCount(); i++)
		GetChan(i)->UpdateVolume();
}

float CMixereView::GetMasterVolume()
{
	return(m_MultiMix->GetMasterVolume(this));
}

/////////////////////////////////////////////////////////////////////////////
// CMixereView misc functions

void CMixereView::TimerHook() 
{
	for (int i = 0; i < GetItemCount(); i++) {
		if (!GetChan(i)->IsEmpty())
			GetChan(i)->TimerHook(m_Tempo);
	}
	m_VolumeBar.TimerHook();
	m_TempoBar.TimerHook();
	if (m_MultiAutoDlg != NULL)
		m_MultiAutoDlg->TimerHook(m_Tempo);
}

BOOL CMixereView::EnableToolTips(BOOL bEnable)
{
	for (int i = 0; i < GetItemCount(); i++)
		GetChan(i)->EnableToolTips(bEnable);
	for (i = 0; i < DLGBARS; i++)
		m_DlgBar[i]->EnableToolTips(bEnable);
	m_HasToolTips = bEnable;
	return(bEnable);
}

void CMixereView::OnContextMenu(CChannel *Chan, CWnd* pWnd, CPoint point)
{
	CMenu	Menu, *SubMenu;
	Menu.LoadMenu(IDR_POPUP);
	SubMenu = Menu.GetSubMenu(0);
	SubMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, AfxGetMainWnd());
}

void CMixereView::SetDefaults()
{
	NotifyUndoableEdit(UCODE_SET_DEFAULTS);
	GetChan(m_CurPos)->GetInfo(&m_ChanDefaults);
}

void CMixereView::ResetDefaults()
{
	NotifyUndoableEdit(UCODE_SET_DEFAULTS);
	m_ChanDefaults.Reset();
}

void CMixereView::EditName()
{
	GetChan(m_CurPos)->EditName();
}

void CMixereView::AddStringUnique(CStringArray& StrArr, LPCSTR Str)
{
	for (int i = 0; i < StrArr.GetSize(); i++) {
		if (StrArr[i] == Str)
			return;
	}
	StrArr.Add(Str);
}

void CMixereView::MsgBoxStrList(LPCSTR Msg, const CStringArray& StrArr)
{
	static	const	MAX_ERR_LINES = 10;
	CString	s = Msg;
	for (int i = 0; i < StrArr.GetSize() && i < MAX_ERR_LINES; i++) {
		s += "\n";
		s += StrArr[i];
	}
	if (i < StrArr.GetSize())
		s += "\n...";
	AfxMessageBox(s);
}

void CMixereView::SetTitleFont(CFont *Font)
{
	m_TitleFont = Font;
	for (int i = 0; i < GetItemCount(); i++)
		GetChan(i)->SetTitleFont(Font);
}

void CMixereView::ApplyColorScheme()
{
	for (int i = 0; i < GetItemCount(); i++)
		GetChan(i)->ApplyColorScheme();
}

void CMixereView::OnPaste(void *Info, int Count, bool Local) 
{
	CChanInfo	*cip = (CChanInfo *)Info;
	if (Local) {	// if paste channels came from this instance
		for (int i = 0; i < Count; i++) {	// for each paste channel
			if (cip[i].m_ID) {					// if not empty
				for (int j = 0; j < GetItemCount(); j++) {	// look up its ID
					if (cip[i].m_ID == GetChan(j)->GetID()) {	// if duplicate ID
						cip[i].m_ID = CreateChanID();			// give it a new ID
						break;
					}
				}
			}
		}
	} else {	// foreign paste channels get new IDs regardless
		for (int i = 0; i < Count; i++) {	// for each paste channel
			if (cip[i].m_ID)					// if not empty
				cip[i].m_ID = CreateChanID();		// give it a new ID
		}
	}
}

int CMixereView::FindChanByID(DWORD ChanID) const
{
	for (int i = 0; i < GetItemCount(); i++) {
		if (GetChan(i)->GetID() == ChanID)
			return(i);
	}
	return(-1);
}

void CMixereView::OnUpdateSnapshotList()
{
	m_SnapshotBar.UpdateList();
}

void CMixereView::OnRestoreSnapshot(int SnapIdx)
{
	m_SnapshotBar.Select(SnapIdx);
}

bool CMixereView::FindNext()
{
	if (!GetItemCount())
		return(FALSE);
	bool	Found = FALSE;
	int	i = m_CurPos;
	while (!Found) {
		if (!m_NewFind) {	// first time, don't update position
			if (!m_FindSearchDown) {
				if (--i < 0)
					i = GetItemCount() - 1;
			} else {
				if (++i >= GetItemCount())
					i = 0;
			}
		}
		CString	Title = GetChan(i)->GetTitle();
		if (!m_FindMatchCase)	// if case-insensitive match
			Title.MakeLower();		// make title lower case
		int	pos = Title.Find(m_FindStr);
		if (pos >= 0) {		// if title contains find string
			SetCurPos(i);
			Deselect();
			SetSelect(i, TRUE);
			EnsureVisible(i);
			Found = TRUE;
		}
		if (!m_NewFind && i == m_CurPos)	// if we're back where we started
			break;								// we're done
		m_NewFind = FALSE;
	}
	if (!Found) {
		CString	msg;
		msg.FormatMessage(IDS_CANT_FIND_STRING, m_FindStr);
		AfxMessageBox(msg);
	}
	return(Found);
}

/////////////////////////////////////////////////////////////////////////////
// CMixereView undo/redo

void CMixereView::SaveUndoState(CUndoState& State)
{
	if (State.GetCode() < START_UCODE) {	// if base class undo code
		CFormListView::SaveUndoState(State);	// defer to base class
		return;
	}
	if (State.GetCode() > END_UCODE) {		// if snapshot undo code
		m_Snapshot->SaveUndoState(State);		// defer to snapshot class
		return;
	}
	int	Chans = GetItemCount();
	UValCurPos(State) = m_CurPos;	// do first so handlers can override
	switch (State.GetCode()) {
	case UCODE_TRANSPORT:
		{
			TRANSPORT_UNDO_STATE	*uap;
			if (State.IsEmpty()) {	// if first time
				AllocUndoArray(uap, State, max(m_Selections, 1));
				SetUndoChanIdxs(uap);	// set each element's channel index
			} else
				GetUndoArray(uap, State);
			for (int i = 0; i < UValItems(State); i++, uap++) {
				CChannel	*Chan = GetChan(uap->m_ChanIdx);
				uap->m_Transport = Chan->GetTransport();
				uap->m_Pos = Chan->GetPosition();
			}
		}
		break;
	case UCODE_GO:
		{
			GO_UNDO_STATE	*uap;
			if (State.IsEmpty()) {	// if first time
				AllocUndoArray(uap, State, max(m_Selections, 1));
				SetUndoChanIdxs(uap);	// set each element's channel index
			} else
				GetUndoArray(uap, State);
			for (int i = 0; i < UValItems(State); i++, uap++) {
				CChannel	*Chan = GetChan(uap->m_ChanIdx);
				uap->m_Transport = Chan->GetTransport();
				uap->m_Pos = Chan->GetPosition();
				for (int j = 0; j < CChanInfo::AUTOS; j++) {
					uap->m_Auto[j].m_Transport = Chan->GetAuto(j)->GetTransport();
					uap->m_Auto[j].m_Pos = Chan->GetAuto(j)->GetNormPos();
				}
			}
		}
		break;
	case UCODE_LOOP:
		{
			bool	*uap;
			AllocUndoArray(uap, State, Chans);
			for (int i = 0; i < Chans; i++)
				uap[i] = GetChan(i)->GetLoop();
		}
		break;
	case UCODE_MUTE:
		{
			bool	*uap;
			AllocUndoArray(uap, State, Chans);
			for (int i = 0; i < Chans; i++)
				uap[i] = GetChan(i)->GetMute();
		}
		break;
	case UCODE_SOLO:
	case UCODE_END_SOLO:
		{
			bool	*uap;
			AllocUndoArray(uap, State, Chans);
			for (int i = 0; i < Chans; i++)
				uap[i] = GetChan(i)->GetSolo();
		}
		break;
	case UCODE_KEEP_SOLO:
		{
			BYTE	*uap;
			AllocUndoArray(uap, State, Chans);
			for (int i = 0; i < Chans; i++)
				uap[i] = GetChan(i)->GetMute() + (GetChan(i)->GetSolo() << 1);
		}
		break;
	case UCODE_LOAD_AUDIO:
	case UCODE_INSERT_AUDIO:
		if (!UValFlags(State)) {
			// override current position with pre-insert position
			UValCurPos(State) = m_PrevPos;
			CChanInfo	*uap;
			AllocUndoArray(uap, State, 1);
			GetChan(m_CurPos)->GetInfo(uap);
			UValFlags(State) = UFLAG_DEL;	// first restore will delete
		} else
			UValFlags(State) ^= UFLAG_DIR;	// reverse direction
		break;
	case UCODE_SHOW_VOLUME:
	case UCODE_SHOW_SNAPSHOT:
	case UCODE_SHOW_MS_FADE:
	case UCODE_SHOW_TEMPO:
		if (!UValBarNum(State))	// if first time (zero bar number)
			// assume m_UndoIdx contains notifier's dialog bar index
			UValBarNum(State) = m_UndoIdx + 1;	// zero is reserved for first time
		UValShowBar(State) = IsDlgBarVisible(UValBarNum(State) - 1);
		break;
	case UCODE_SET_DEFAULTS:
		{
			CChanInfo	*uap;
			AllocUndoArray(uap, State, 1);
			*uap = m_ChanDefaults;
		}
		break;
	case UCODE_EDIT_AUTO:
		{
			CEditAutoUndoState	*uap;
			if (State.IsEmpty()) {	// if first time
				AllocUndoArray(uap, State, max(m_Selections, 1));
				SetUndoChanIdxs(uap);	// set each element's channel index
				// assume m_UndoIdx contains notifier's auto slider index
				UValAutoIdx(State) = m_UndoIdx;
			} else
				GetUndoArray(uap, State);
			for (int i = 0; i < UValItems(State); i++, uap++) {
				CChannel	*Chan = GetChan(uap->m_ChanIdx);
				CAutoSliderCtrl	*Auto = Chan->GetAuto(UValAutoIdx(State));
				Auto->GetInfo(*uap);
				uap->m_ChanTransport = Chan->GetTransport();
				uap->m_ChanLoop = Chan->GetLoop();
				uap->m_ChanPos = Chan->GetPosition();
			}
		}
		break;
	default:
		NODEFAULTCASE;		
	}
}

void CMixereView::RestoreUndoState(const CUndoState& State)
{
	if (State.GetCode() < START_UCODE) {	// if base class undo code
		CFormListView::RestoreUndoState(State);	// defer to base class
		return;
	}
	if (State.GetCode() > END_UCODE) {		// if snapshot undo code
		m_Snapshot->RestoreUndoState(State);	// defer to snapshot class
		return;
	}
	CWaitCursor	wc;
	switch (State.GetCode()) {
	case UCODE_TRANSPORT:
		{
			TRANSPORT_UNDO_STATE	*uap;
			GetUndoArray(uap, State);
			for (int i = 0; i < UValItems(State); i++, uap++) {
				CChannel	*Chan = GetChan(uap->m_ChanIdx);
				Chan->SetTransportAndPos(uap->m_Transport, uap->m_Pos);
			}
		}
		break;
	case UCODE_GO:
		{
			GO_UNDO_STATE	*uap;
			GetUndoArray(uap, State);
			for (int i = 0; i < UValItems(State); i++, uap++) {
				CChannel	*Chan = GetChan(uap->m_ChanIdx);
				for (int j = 0; j < CChanInfo::AUTOS; j++) {
					Chan->GetAuto(j)->SetTransport(uap->m_Auto[j].m_Transport);
					Chan->GetAuto(j)->SetNormPos(uap->m_Auto[j].m_Pos);
				}
				Chan->SetTransportAndPos(uap->m_Transport, uap->m_Pos);
			}
		}
		break;
	case UCODE_LOOP:
		{
			bool	*uap;
			GetUndoArray(uap, State);
			for (int i = 0; i < UValItems(State); i++)
				GetChan(i)->SetLoop(uap[i]);
		}
		break;
	case UCODE_MUTE:
		{
			bool	*uap;
			GetUndoArray(uap, State);
			for (int i = 0; i < UValItems(State); i++)
				GetChan(i)->SetMute(uap[i]);
		}
		break;
	case UCODE_SOLO:
	case UCODE_END_SOLO:
		{
			bool	*uap;
			GetUndoArray(uap, State);
			for (int i = 0; i < UValItems(State); i++)
				GetChan(i)->SetSolo(uap[i]);
		}
		break;
	case UCODE_KEEP_SOLO:
		{
			BYTE	*uap;
			GetUndoArray(uap, State);
			for (int i = 0; i < UValItems(State); i++) {
				GetChan(i)->SetMute((uap[i] & 0x1) != 0);
				GetChan(i)->SetSolo((uap[i] & 0x2) != 0);
			}
		}
		break;
	case UCODE_LOAD_AUDIO:
		{
			CChanInfo	*uap;
			GetUndoArray(uap, State);
			if ((UValFlags(State) & UFLAG_DIR) == UFLAG_INS) {
				GetChan(uap->m_Index)->OpenItem(uap);
				GetChan(uap->m_Index)->SetInfo(uap);
			} else
				GetChan(uap->m_Index)->Reset();
		}
		break;
	case UCODE_INSERT_AUDIO:
		RestoreUndoItems(State);
		break;
	case UCODE_SHOW_VOLUME:
	case UCODE_SHOW_SNAPSHOT:
	case UCODE_SHOW_MS_FADE:
	case UCODE_SHOW_TEMPO:
		ShowDlgBar(UValBarNum(State) - 1, UValShowBar(State) != 0);
		break;
	case UCODE_SET_DEFAULTS:
		{
			CChanInfo	*uap;
			GetUndoArray(uap, State);
			m_ChanDefaults = *uap;
		}
		break;
	case UCODE_EDIT_AUTO:
		{
			CEditAutoUndoState	*uap;
			GetUndoArray(uap, State);
			for (int i = 0; i < UValItems(State); i++, uap++) {
				CChannel	*Chan = GetChan(uap->m_ChanIdx);
				CAutoSliderCtrl	*Auto = Chan->GetAuto(UValAutoIdx(State));
				Auto->SetInfoNotify(*uap);	// calls OnNewPos and OnSelection
				Chan->SetLoop(uap->m_ChanLoop != 0);
				// if channel transport changed, restore it and audio position
				if (uap->m_ChanTransport != Chan->GetTransport())
					Chan->SetTransportAndPos(uap->m_ChanTransport, uap->m_ChanPos);
			}
		}
		break;
	default:
		NODEFAULTCASE;		
	}
	SetCurPos(UValCurPos(State));
}

CUndoable *CMixereView::FindUndoable(CUndoState::UNDO_KEY Key)
{
	// if key < 0x8000, it's a normal child window ID
	// if key >= 0xE800, it's a control bar child window ID
	// otherwise, assume it's a channel index offset by 0x8000
	if (Key < CChannel::UNDO_KEY_OFFSET)
		return(NULL);	// we don't own any normal child windows
	if (Key >= AFX_IDW_CONTROLBAR_FIRST)
		return(WndToUndo(m_ChildFrm->GetControlBar(Key)));	// return control bar
	return(GetChan(Key - CChannel::UNDO_KEY_OFFSET));	// return channel
}

void CMixereView::GetUndoTitle(const CUndoState& State, CString& Title)
{
	int	Code = State.GetCode();
	if (Code < UNDO_CODE_OFFSET)	// if it's a base class undo code
		CFormListView::GetUndoTitle(State, Title);	// defer to base class
	else {
		Code -= UNDO_CODE_OFFSET;	// subtract off base class range
		if (Code < UNDO_CODES) {	// if the undo code is within our range
			switch (Code) {
			case UCODE_EDIT_AUTO:		// auto edit dialog has custom title
				CChannel::GetAutoName(UValAutoIdx(State), Title);
				break;
			default:	// use code as index into our table of undo strings
				Title.LoadString(m_UndoTitleID[Code]);	
			}
		} else	// assume it's a snapshot undo code
			m_Snapshot->GetUndoTitle(State, Title);	// defer to snapshot class
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMixereView dialogs

void CMixereView::EditAuto(int AutoIdx)
{
	if (!GetItemCount())
		return;
	m_UndoIdx = AutoIdx;	// pass auto slider index to SaveUndoState
	NotifyUndoableEdit(UCODE_EDIT_AUTO);
	CMultiAutoDlg	dlg(*this, AutoIdx);
	m_MultiAutoDlg = &dlg;	// tells our timer hook to call dialog's hook
	dlg.DoModal();
	m_MultiAutoDlg = NULL;
	if (!dlg.IsModified())
		CancelUndoableEdit(UCODE_EDIT_AUTO);
}

bool CMixereView::IsDlgBarVisible(int BarIdx) const
{
	ASSERT(BarIdx >= 0 && BarIdx < DLGBARS);
	return(m_DlgBar[BarIdx]->IsWindowVisible() != 0);
}

void CMixereView::ShowDlgBar(int BarIdx, bool Enable)
{
	ASSERT(BarIdx >= 0 && BarIdx < DLGBARS);
	m_UndoIdx = BarIdx;		// pass dialog bar index to SaveUndoState
	if (!IsUndoInProgress())
		NotifyUndoableEdit(m_DlgBarInfo[BarIdx].UndoCode, UE_INSIGNIFICANT);
	m_ChildFrm->ShowControlBar(m_DlgBar[BarIdx], Enable, 0);
}

void CMixereView::ToggleDlgBar(int BarIdx)
{
	ShowDlgBar(BarIdx, !IsDlgBarVisible(BarIdx));
}

/////////////////////////////////////////////////////////////////////////////
// CMixereView::CMixerVolumeBar

CMixereView::CMixerVolumeBar::CMixerVolumeBar(CMixereView *Mixer) :
	m_Mixer(Mixer)
{
}

void CMixereView::CMixerVolumeBar::OnPosChange()
{
	CVolumeBar::OnPosChange();
	m_Mixer->UpdateVolume();
}

/////////////////////////////////////////////////////////////////////////////
// CMixereView::CMixerTempoBar

CMixereView::CMixerTempoBar::CMixerTempoBar(CMixereView *Mixer) :
	m_Mixer(Mixer)
{
}

void CMixereView::CMixerTempoBar::OnPosChange()
{
	CTempoBar::OnPosChange();
	m_Mixer->m_Tempo = GetNormTempo();
}

/////////////////////////////////////////////////////////////////////////////
// CMixereView::CMixerMSFadeBar

CMixereView::CMixerMSFadeBar::CMixerMSFadeBar(CMixereView *Mixer) :
	m_Mixer(Mixer)
{
}

void CMixereView::CMixerMSFadeBar::OnTimeChange()
{
	for (int i = 0; i < m_Mixer->GetItemCount(); i++)
		m_Mixer->GetChan(i)->SetMSFadeTime(GetTime());
}

/////////////////////////////////////////////////////////////////////////////
// CMixereView overrides

CFormListItemDlg *CMixereView::OnNewItem()
{
	// create channel as a modeless dialog
	CChannel	*Chan = new CChannel(*this);
	if (!Chan->Create(IDD_CHANNEL)) {
		delete Chan;
		return(NULL);
	}
	Chan->SetTitleFont(m_TitleFont);	// set channel's title font
	if (m_HasToolTips)					// optionally enable its tooltips
		Chan->EnableToolTips(TRUE);
	return(Chan);
}

void CMixereView::DoDataExchange(CDataExchange* pDX)
{
	CFormListView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMixereView)
	//}}AFX_DATA_MAP
}

BOOL CMixereView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CFormListView::PreCreateWindow(cs);
}

void CMixereView::OnInitialUpdate()
{
	CFormListView::OnInitialUpdate();
}

/////////////////////////////////////////////////////////////////////////////
// CMixereView diagnostics

#ifdef _DEBUG
void CMixereView::AssertValid() const
{
	CFormListView::AssertValid();
}

void CMixereView::Dump(CDumpContext& dc) const
{
	CFormListView::Dump(dc);
}

CMixereDoc* CMixereView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMixereDoc)));
	return (CMixereDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMixereView message map

BEGIN_MESSAGE_MAP(CMixereView, CFormListView)
	//{{AFX_MSG_MAP(CMixereView)
	ON_WM_DROPFILES()
	ON_COMMAND(ID_FILE_PROPERTIES, OnFileProperties)
	ON_COMMAND(ID_CONTEXT_COPY, OnContextCopy)
	ON_COMMAND(ID_CONTEXT_CUT, OnContextCut)
	ON_COMMAND(ID_CONTEXT_INSERT, OnContextInsert)
	ON_COMMAND(ID_CONTEXT_DELETE, OnContextDelete)
	ON_COMMAND(ID_TOGGLE_PLAY, OnTogglePlay)
	ON_COMMAND(ID_PLAY, OnPlay)
	ON_COMMAND(ID_PAUSE, OnPause)
	ON_COMMAND(ID_STOP, OnStop)
	ON_COMMAND(ID_TOGGLE_LOOP, OnToggleLoop)
	ON_COMMAND(ID_TOGGLE_MUTE, OnToggleMute)
	ON_COMMAND(ID_TOGGLE_SOLO, OnToggleSolo)
	ON_COMMAND(ID_END_SOLO, OnEndSolo)
	ON_COMMAND(ID_KEEP_SOLO, OnKeepSolo)
	ON_COMMAND(ID_LOAD_AUDIO, OnLoadAudio)
	ON_COMMAND(ID_EDIT_NAME, OnEditName)
	ON_COMMAND(ID_AUTO_VOLUME, OnAutoVolume)
	ON_COMMAND(ID_AUTO_PAN, OnAutoPan)
	ON_COMMAND(ID_AUTO_PITCH, OnAutoPitch)
	ON_COMMAND(ID_AUTO_POS, OnAutoPos)
	ON_COMMAND(ID_SET_DEFAULTS, OnSetDefaults)
	ON_UPDATE_COMMAND_UI(ID_TOGGLE_PLAY, OnUpdatePlay)
	ON_UPDATE_COMMAND_UI(ID_END_SOLO, OnUpdateEndSolo)
	ON_UPDATE_COMMAND_UI(ID_SET_DEFAULTS, OnUpdateSetDefaults)
	ON_COMMAND(ID_MIXER_VOLUME, OnMixerVolume)
	ON_UPDATE_COMMAND_UI(ID_MIXER_VOLUME, OnUpdateMixerVolume)
	ON_COMMAND(ID_MIXER_TEMPO, OnMixerTempo)
	ON_UPDATE_COMMAND_UI(ID_MIXER_TEMPO, OnUpdateMixerTempo)
	ON_COMMAND(ID_MS_FADE, OnMSFade)
	ON_UPDATE_COMMAND_UI(ID_MS_FADE, OnUpdateMSFade)
	ON_COMMAND(ID_GO, OnGo)
	ON_COMMAND(ID_TAKE_SNAPSHOT, OnTakeSnapshot)
	ON_COMMAND(ID_EDIT_SNAPSHOTS, OnEditSnapshots)
	ON_COMMAND(ID_SNAPSHOTS, OnSnapshots)
	ON_UPDATE_COMMAND_UI(ID_SNAPSHOTS, OnUpdateSnapshots)
	ON_COMMAND(ID_EDIT_FIND, OnEditFind)
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(ID_PLAY, OnUpdatePlay)
	ON_UPDATE_COMMAND_UI(ID_PAUSE, OnUpdatePlay)
	ON_UPDATE_COMMAND_UI(ID_STOP, OnUpdatePlay)
	ON_UPDATE_COMMAND_UI(ID_GO, OnUpdatePlay)
	ON_UPDATE_COMMAND_UI(ID_TOGGLE_LOOP, OnUpdatePlay)
	ON_UPDATE_COMMAND_UI(ID_TOGGLE_MUTE, OnUpdatePlay)
	ON_UPDATE_COMMAND_UI(ID_TOGGLE_SOLO, OnUpdatePlay)
	ON_UPDATE_COMMAND_UI(ID_KEEP_SOLO, OnUpdateEndSolo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_NAME, OnUpdatePlay)
	ON_UPDATE_COMMAND_UI(ID_AUTO_PAN, OnUpdatePlay)
	ON_UPDATE_COMMAND_UI(ID_AUTO_PITCH, OnUpdatePlay)
	ON_UPDATE_COMMAND_UI(ID_AUTO_VOLUME, OnUpdatePlay)
	ON_UPDATE_COMMAND_UI(ID_AUTO_POS, OnUpdatePlay)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SETDOCTITLE, OnSetDocTitle)
	ON_REGISTERED_MESSAGE(WM_FINDREPLACE, OnFindReplace)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMixereView message handlers

void CMixereView::OnDropFiles(HDROP hDropInfo)
{
	DropFiles(hDropInfo);
}

void CMixereView::OnFileProperties() 
{
	CPropertiesDlg		dlg;
	dlg.m_ChannelCount = GetItemCount();
	if (dlg.DoModal() == IDOK) {
		SetItemCountUndoable(dlg.m_ChannelCount);
	}
}

void CMixereView::OnContextCopy() 
{
	Copy();
}

void CMixereView::OnContextCut() 
{
	Cut();
}

void CMixereView::OnContextInsert() 
{
	Insert();
}

void CMixereView::OnContextDelete() 
{
	Delete();
}

void CMixereView::OnTogglePlay() 
{
	TogglePlay();
}

void CMixereView::OnPlay() 
{
	SetTransport(CChannel::PLAY);
}

void CMixereView::OnPause() 
{
	SetTransport(CChannel::PAUSE);
}

void CMixereView::OnStop() 
{
	SetTransport(CChannel::STOP);
}

void CMixereView::OnGo() 
{
	Go();
}

void CMixereView::OnToggleLoop() 
{
	SetLoop(-1);
}

void CMixereView::OnToggleMute() 
{
	SetMute(-1);
}

void CMixereView::OnToggleSolo() 
{
	SetSolo(-1);
}

void CMixereView::OnEndSolo() 
{
	EndSolo();
}

void CMixereView::OnKeepSolo() 
{
	KeepSolo();
}

void CMixereView::OnLoadAudio() 
{
	LoadAudio();
}

void CMixereView::OnEditName() 
{
	EditName();
}

void CMixereView::OnAutoVolume() 
{
	EditAuto(CChanInfo::AUTO_VOL);
}

void CMixereView::OnAutoPan() 
{
	EditAuto(CChanInfo::AUTO_PAN);
}

void CMixereView::OnAutoPitch() 
{
	EditAuto(CChanInfo::AUTO_PITCH);
}

void CMixereView::OnAutoPos() 
{
	EditAuto(CChanInfo::AUTO_POS);
}

void CMixereView::OnSetDefaults() 
{
	SetDefaults();
}

void CMixereView::OnUpdatePlay(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetSelCount() || !IsCurEmpty());
}

void CMixereView::OnUpdateEndSolo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetSoloCount());
}

void CMixereView::OnUpdateSetDefaults(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetItemCount());
}

void CMixereView::OnMixerVolume() 
{
	ToggleDlgBar(DLGBAR_VOLUME);
}

void CMixereView::OnUpdateMixerVolume(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(IsDlgBarVisible(DLGBAR_VOLUME));
}

void CMixereView::OnMixerTempo()
{
	ToggleDlgBar(DLGBAR_TEMPO);
}

void CMixereView::OnUpdateMixerTempo(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(IsDlgBarVisible(DLGBAR_TEMPO));
}

void CMixereView::OnMSFade() 
{
	ToggleDlgBar(DLGBAR_MS_FADE);
}

void CMixereView::OnUpdateMSFade(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(IsDlgBarVisible(DLGBAR_MS_FADE));
}

void CMixereView::OnTakeSnapshot() 
{
	m_Snapshot->Take();
}

void CMixereView::OnEditSnapshots() 
{
	m_Snapshot->EditList();
}

void CMixereView::OnSnapshots() 
{
	ToggleDlgBar(DLGBAR_SNAPSHOT);
}

void CMixereView::OnUpdateSnapshots(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(IsDlgBarVisible(DLGBAR_SNAPSHOT));
}

LRESULT	CMixereView::OnSetDocTitle(UINT wParam, LONG lParam)
{
	m_MultiMix->UpdateMixerName(this);	// update main frame bars
	UpdateDlgBarTitles();
	return(0);
}

BOOL CMixereView::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if (LOWORD(wParam) >= CSnapshot::HOTKEY_CMD_BASE)
		m_Snapshot->Restore(LOWORD(wParam) - CSnapshot::HOTKEY_CMD_BASE);
	return CFormListView::OnCommand(wParam, lParam);
}

void CMixereView::OnEditFind() 
{
	if (m_FindDlg != NULL)
		return;
	m_NewFind = TRUE;
	m_FindDlg = new CFindReplaceDialog;
	m_FindDlg->Create(TRUE, m_FindStr, NULL,
		FR_DOWN | FR_HIDEWHOLEWORD, this);
}

LONG CMixereView::OnFindReplace(WPARAM wParam, LPARAM lParam)
{
	CFindReplaceDialog *frd = CFindReplaceDialog::GetNotifier(lParam);
	if (frd->IsTerminating()) {
		m_FindStr = frd->GetFindString();
		m_FindDlg = NULL;
	} else if (frd->FindNext() || frd->ReplaceCurrent()) {
		CString	FindStr = frd->GetFindString();
		if (!frd->MatchCase())	// if case-insensitive match
			FindStr.MakeLower();	// make find string lower case
		if (FindStr != m_FindStr) {		// if find string changed
			m_FindStr = FindStr;			// update member var
			m_NewFind = TRUE;				// reset new find flag
		}
		m_FindMatchCase = frd->MatchCase() != 0;
		m_FindSearchDown = frd->SearchDown() != 0;
		FindNext();
	}
	return(0);
}
