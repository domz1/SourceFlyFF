// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define	_WIN32_WINNT	0x0500
#define	WINVER		0x0500

#define __DBSERVER

#include <afx.h>
#include <afxwin.h>
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPO

#include "VersionCommon.h"

#include "memtrace.h"

#include "DefineCommon.h"
#include "CmnHdr.h"

#include <map>
#include <set>
#include <string>
#include <vector>
using namespace std;

#include <assert.h>


#include "vutil.h"
#include "DXUtil.h"
#include <d3dx9math.h>
#include <mmsystem.h>
#include "xutil.h"
#include "path.h"
#include "Define.h"
//#include "DefineObj.h"

#include "defineAttribute.h"

#include "DefineNeuz.h"
#include "DefineJob.h"
#include "DefineWorld.h"
#include "resdata.h"
#include "Data.h"
#include "file.h"
#include "authorization.h"
#include "Scanner.h"
#include "ScriptDialog.h"
#include "Project.h"
#include "Timer.h"
#include "Debug.h"
#include "d3dfont.h"
//#include "exceptionhandler.h"
#include "Query.h"
#include "DbManager.h"
#include "defineitemkind.h"

#if __VER >= 19
#include "defineItemGrade.h"
#include "defineItemType.h"
#endif//__VER >= 19

#include "buyinginfo.h"
#include "DefineConMode.h"

#include "memtrace.h"
#ifdef __MEM_TRACE
#ifndef _DEBUG
#define	new	MT_NEW
#endif	// _DEBUG
#endif	// __MEM_TRACE

extern	HWND	hMainWnd;
extern	HINSTANCE	hInst;

typedef	struct	AppInfo
{
	DWORD	dwSys;
	DWORD	dwId;		// server id
}	APP_INFO, *LPAPP_INFO;

extern	char	DSN_NAME_CHARACTER01[260];
extern	char	DB_ADMIN_ID_CHARACTER01[260];
extern	char	DSN_NAME_LOG[260];
extern	char	DB_ADMIN_ID_LOG[260];

#ifdef __S1108_BACK_END_SYSTEM
extern	char	DSN_NAME_BACKSYSTEM[260];
extern	char	DB_ADMIN_ID_BACKSYSTEM[260];
#endif // __S1108_BACK_END_SYSTEM

extern	char	DSN_NAME_ITEMUPDATE[260];
extern	char	DB_ADMIN_ID_ITEMUPDATE[260];


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)