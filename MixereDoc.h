// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      21nov03 initial version

		mixer document
 
*/

// MixereDoc.h : interface of the CMixereDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MIXEREDOC_H__48FCB384_4785_4313_92DE_EFE803A4FDEA__INCLUDED_)
#define AFX_MIXEREDOC_H__48FCB384_4785_4313_92DE_EFE803A4FDEA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MixerInfo.h"

class CMixereDoc : public CDocument
{
protected: // create from serialization only
	CMixereDoc();
	DECLARE_DYNCREATE(CMixereDoc)

// Member data
public:
	CMixerInfo	m_MixerInfo;

// Attributes
public:
	CMixereView	*GetView() const;

// Operations
public:
	void	Close();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMixereDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMixereDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMixereDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	static	CMixereDoc	*m_InitialNewDoc;
	static	DWORD	m_DocsCreated;

	NOCOPIES(CMixereDoc);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MIXEREDOC_H__48FCB384_4785_4313_92DE_EFE803A4FDEA__INCLUDED_)
