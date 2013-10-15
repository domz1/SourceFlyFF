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
#define _WIN32_WINNT 0x0500
#define	WINVER	0x0500

#define __ACCOUNT

//#define IDC_STATIC              (-1)     // all static controls

// Windows Header Files:
#include <afxwin.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <mmsystem.h>
//#include <winsock2.h>
#include <dplay.h>
#include <d3dx9tex.h>

#pragma warning(disable:4786)
#include <map>
#include <set>
#include <list>
#include <vector>
using	namespace	std;

//#include "compile.h"
#include "VersionCommon.h"

#include "memtrace.h"

#include "DefineCommon.h"
#include "cmnhdr.h"

#include <afxdisp.h>        // MFC Automation classes

#include "dxutil.h"
#include "vutil.h"
#include "data.h"
#include "file.h"
#include "d3dfont.h"
#include "scanner.h"
#include "timer.h"
#include "debug.h"
#include "xutil.h"
//#include "targa.h"
//#include "exceptionhandler.h"
#include "buyinginfo.h"

const UINT	IDT_SENDPLAYERCOUNT	= 0;
const UINT	IDT_RECONNECT		= 1;
const UINT  IDT_KEEPALIVE		= 2;
const UINT  IDT_KICKOUT			= 3;
const UINT  IDT_PREVENT_EXCESS	= 4;
const UINT	IDT_BUYING_INFO		= 5;
const UINT  IDT_RELOAD_PROJECT	= 6;
const UINT  IDT_TIME_CHECKADDR	= 7;


const UINT		WM_CONNENT_BILLING = WM_USER + 100;

extern char		DSN_NAME_LOG[];
extern char		DB_ADMIN_ID_LOG[];

extern char		DSN_NAME_LOGIN[];
extern char		DB_ADMIN_ID_LOGIN[];

extern char		DSN_NAME_BILLING[];
extern char		DB_ADMIN_ID_BILLING[];

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
