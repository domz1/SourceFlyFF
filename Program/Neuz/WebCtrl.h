// WebCtrl.h: interface for the CWebCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WEBCTRL_H__9B4BB033_6A45_4F9C_BAE0_76F7BF476070__INCLUDED_)
#define AFX_WEBCTRL_H__9B4BB033_6A45_4F9C_BAE0_76F7BF476070__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

 

//#include <ocidl.h>
#include <shlobj.h>
#include <EXDISPID.H>
#include <ATLBASE.H>
//#include <ATLCONV.H>

#define WC_WEBCTRLA "WebCtrl32"
#define WC_WEBCTRLW "WebCtrl32"

#ifdef UNICODE
#define WC_WEBCTRL WC_WEBCTRLW
#else
#define WC_WEBCTRL WC_WEBCTRLA
#endif

#define WEB_CTRL_SHOW	(WM_USER + 1)
#define WEB_CTRL_HIDE	(WM_USER + 2)

#define WEB_CTRL_ADDRESS	(WM_USER + 3)
#define WEB_CTRL_POST   	(WM_USER + 4)
#define WEB_CTRL_HEADER 	(WM_USER + 5)
#define WEB_CTRL_NAVIGATE 	(WM_USER + 6)
#ifdef __NEW_WEB_BOX
#define WEB_CTRL_GO_BACK 		( WM_USER + 20 )
#define WEB_CTRL_GO_FORWARD		( WM_USER + 21 )
#define WEB_CTRL_STOP	 		( WM_USER + 22 )
#define WEB_CTRL_REFRESH		( WM_USER + 23 )
#endif // __NEW_WEB_BOX

#define WEB_STR_LEN         512

#define WM_CHECKWEBFINISH  0xaa

class CWebContainer;
class CWebCtrl  
{
public:

	CWebCtrl( HWND );

private:

	
	friend LRESULT CALLBACK WebCtrlProc( HWND, UINT, WPARAM, LPARAM );

	LRESULT WebCtrlProc( UINT, WPARAM, LPARAM );


	HWND m_hWnd;

	int  m_cxScroll;
	int  m_cyScroll;

	CWebContainer *m_pContainer ; 
	VARIANT  m_Va;

public:
	IWebBrowser2 *m_pweb ; 

//	virtual ~CWebCtrl();
public:
//	void Navigate( LPTSTR );
	void Navigate( char* szURL, char* szPostData = NULL , char* szHeaders = NULL) ; 


};

bool WINAPI InitWebCtrl();
bool WINAPI UninitWebCtrl();

static char g_address[WEB_STR_LEN] ; 
static char g_postdata[WEB_STR_LEN] ; 
static char g_header[WEB_STR_LEN] ; 


//HWND WINAPI CreateWebControl(LPCTSTR pszText, DWORD dwStyles, int x, int y, int cx,int cy, HWND hWndParent, UINT uId );

#endif // !defined(AFX_WEBCTRL_H__9B4BB033_6A45_4F9C_BAE0_76F7BF476070__INCLUDED_)
