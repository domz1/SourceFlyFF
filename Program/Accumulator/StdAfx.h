// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__736CA8C6_3203_45C9_B0E8_1E7F6096E4EC__INCLUDED_)
#define AFX_STDAFX_H__736CA8C6_3203_45C9_B0E8_1E7F6096E4EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#define	_WIN32_WINNT	0x0500
#define	WINVER	0x0500
//#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <mmsystem.h>
#include "cmnhdr.h"
#include "dxutil.h"
#include "..\_common\debug.h"

#include <list>
using	namespace	std;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__736CA8C6_3203_45C9_B0E8_1E7F6096E4EC__INCLUDED_)
