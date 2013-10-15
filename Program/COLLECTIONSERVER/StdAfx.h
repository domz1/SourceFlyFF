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
#define	WINVER	0x0500

#define		__COLLECTION
#define		__SERVER				// 클라이언트 전용코드를 빌드하지 않기 위한 define
#define		__CRC
#define		__SO1014				// 소켓 예외 처리( 캐쉬, 인증, 로그인 )
#define		__PROTOCOL0910
#define		__PROTOCOL1021
#define		__VERIFYNETLIB
#define		__DOS1101
#define		__STL_0402				// stl

#if (_MSC_VER > 1200)
#define		__VS2003		// 컴파일러변경.net
#endif

// Windows Header Files:
//#include <windows.h>
#include <afxwin.h>
#include <mmsystem.h>

// C RunTime Header Files
//#include <stdlib.h>
//#include <malloc.h>
//#include <memory.h>
//#include <tchar.h>

// Local Header Files
#pragma warning(disable:4786)
#include <list>
#include <map>
#include <vector>
using namespace std;

#include "memtrace.h"

#include "DefineCommon.h"
#include "CmnHdr.h"
#include "DXUtil.h"
#include "data.h"
#include "Query.h"
#include "debug.h"
#include "file.h"
#include "scanner.h"
#include "d3dfont.h"
#include "timer.h"
#include "xutil.h"
#include "vutil.h"

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
