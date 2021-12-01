// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      21nov03 initial version
		01		30nov03	add console
		02		22jan04	suppress debug warning caused by audiere STL code
		03		26jan04	add no copies macro
		04		25sep04	remove console

		standard includes
 
*/

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__7501BA92_EC5A_4DC8_9FE2_4F88B611FDA7__INCLUDED_)
#define AFX_STDAFX_H__7501BA92_EC5A_4DC8_9FE2_4F88B611FDA7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

// suppress Audiere debug warning message
#pragma warning(disable : 4786)	// identifier was truncated in debug info 

#include "Audiere.h"		// speeds up compilation; Audiere uses STL

// create a CString from a string resource
#define LDS(x) CString((LPCSTR)IDS_##x)

// prevent object from being copied or assigned
#define NOCOPIES(object) \
private: \
	object(const object&); \
	object& operator=(const object&);

// load accelerator table from resource
#define LOADACCEL(x) LoadAccelerators(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(x))

// registry section for settings
#define REG_SETTINGS	"Settings"

// these flags should be ZERO in a released version
#define SHOW_CONSOLE	0	// create Win32 console
#define UNDO_NATTER		0	// instrument undo manager

// clamp a value to a range
#define CLAMP(x, lo, hi) (min(max((x), (lo)), (hi)))

// trap bogus default case in switch statement
#define NODEFAULTCASE	ASSERT(0)

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__7501BA92_EC5A_4DC8_9FE2_4F88B611FDA7__INCLUDED_)
