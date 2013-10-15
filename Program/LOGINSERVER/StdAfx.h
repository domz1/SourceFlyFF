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

#define __MASSIVE

#include <afxwin.h>

#include <mmsystem.h>

#pragma warning(disable:4786)
#include <map>
#include <list>
#include <vector>
using	namespace	std;


#include "VersionCommon.h"

#include "memtrace.h"

#include "DefineCommon.h"
#include "CmnHdr.h"

#include <afxdisp.h>        // MFC Automation classes

#include "DXUtil.h"
#include "Data.h"
#include "file.h"
#include "Scanner.h"
#include "Timer.h"
#include "d3dfont.h"
#include "vutil.h"
//#include "targa.h"

#include "debug.h"
#include "xutil.h"
//#include "exceptionhandler.h"

#include "tickcount.h"

extern	CTickCount	g_TickCount;
extern	char	g_lpCoreAddr[16];
extern	char	g_lpDBAddr[16];
extern  char	g_szMSG_VER[];
extern  BOOL	g_bNProtectAuth;
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
