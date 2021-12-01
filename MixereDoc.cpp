// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      21nov03 initial version
		01		18jan04	doc finally owns mixer info
		02		19jan04	convert to MDI
		03		08feb04	when doc is opened, close unmodified initial new doc
		04		11feb04	make close a public member function
		05		08jan05	store must reset undo manager's modified flag
		06		20jan05	handle save: if doc title will change, notify view

		mixer document
 
*/

// MixereDoc.cpp : implementation of the CMixereDoc class
//

#include "stdafx.h"
#include "Resource.h"

#include "MixereDoc.h"
#include "MixereView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMixereDoc

CMixereDoc	*CMixereDoc::m_InitialNewDoc;
DWORD	CMixereDoc::m_DocsCreated;

IMPLEMENT_DYNCREATE(CMixereDoc, CDocument)

BEGIN_MESSAGE_MAP(CMixereDoc, CDocument)
	//{{AFX_MSG_MAP(CMixereDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMixereDoc construction/destruction

CMixereDoc::CMixereDoc()
{
	m_DocsCreated++;
}

CMixereDoc::~CMixereDoc()
{
	m_InitialNewDoc = NULL;
}

BOOL CMixereDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	if (m_DocsCreated == 1)
		m_InitialNewDoc = this;

	m_MixerInfo.Reset();

	return TRUE;
}

CMixereView	*CMixereDoc::GetView() const
{
	POSITION	pos = GetFirstViewPosition();
	return(pos != NULL ? (CMixereView *)GetNextView(pos) : NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CMixereDoc operations

void CMixereDoc::Close()
{
	POSITION pos = GetFirstViewPosition();
	while (pos != NULL) {	// for each view of this document
		CView	*pView = GetNextView(pos);
		CFrameWnd	*pFrame = pView->GetParentFrame();	// find its parent frame
		if (pFrame != NULL)
			pFrame->PostMessage(WM_CLOSE);	// and close it
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMixereDoc serialization

void CMixereDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring()) {
		CMixereView	*vp = GetView();
		if (vp == NULL)		// should never happen
			AfxThrowArchiveException(CArchiveException::generic);
		vp->GetInfo(m_MixerInfo);
		vp->GetUndoManager()->ResetModifiedFlag();
	}
	m_MixerInfo.Serialize(ar);
}

/////////////////////////////////////////////////////////////////////////////
// CMixereDoc diagnostics

#ifdef _DEBUG
void CMixereDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMixereDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMixereDoc commands

BOOL CMixereDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	// if initial new doc is unmodified, close it
	if (m_InitialNewDoc != NULL && !m_InitialNewDoc->IsModified())
		m_InitialNewDoc->Close();

	return TRUE;
}

BOOL CMixereDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnSaveDocument(lpszPathName))
		return FALSE;

	// if document title will change, notify view so it can update windows that
	// depend on document title; use post instead of send, otherwise view grabs
	// the title before the document has a chance to modify it
	if (GetPathName() != lpszPathName) {	// if title will change
		CMixereView	*vp = GetView();
		if (vp != NULL)							
			vp->PostMessage(CMixereView::WM_SETDOCTITLE);	// post notification
	}
	return TRUE;
}
