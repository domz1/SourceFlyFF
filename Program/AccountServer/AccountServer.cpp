// AccountServer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#ifdef IDC_STATIC
#undef IDC_STATIC
#endif
#include "resource.h"
#include "dpsrvr.h"
#include "dpdbsrvr.h"
#include "mytrace.h"
#include "account.h"
#include "BillingMgr.h"
#include "dpadbill.h"
#include "..\_UnhandledException\exceptionhandler.h"
#include "..\Resource\Lang.h"

/*
#ifdef __GIFTBOX0213
#include "dpwldsrvr.h"
#endif	// __GIFTBOX0213
*/

#include "dbmanager.h"
extern	CDbManager	g_DbManager;

extern	CDPSrvr		g_dpSrvr;
extern	CDPDBSrvr	g_dpDbSrvr;
extern	CMyTrace	g_MyTrace;
extern	CAccountMng	g_AccountMng;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE		hInst;								// current instance
HWND			hMainWnd;
TCHAR			szTitle[MAX_LOADSTRING];			// The title bar text
TCHAR			szWindowClass[MAX_LOADSTRING];		// The title bar text

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
#ifdef __SECURITY_0628
void	LoadResAuth( LPCTSTR lpszFileName );
#endif	// __SECURITY_0628

LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
void	ExitInstance( void );
BOOL	Script( LPCTSTR lpszFileName );
BOOL	CALLBACK	MaxConnDlgProc( HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam );
BOOL	CALLBACK	ToggleDlgProc( HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam );
BOOL	CALLBACK	SetTimeDlgProc( HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam );



// //////////////////////////////////////////////////////////////////////////
// //	BEGINTEST100111	FFL_DUMP
#include "../__ffl_dump/include/ffl_dump_unexception_filter.h"

#ifdef	_DEBUG
#pragma comment( lib, "../__lib/win32/msvc71/ffl_dumpd.lib" )
#else
#pragma comment( lib, "../__lib/win32/msvc71/ffl_dumpr.lib" )
#endif
// //	ENDTEST100111	FFL_DUMP
// //////////////////////////////////////////////////////////////////////////



void InitLanguageFromResource( HINSTANCE hInstance )
{
	char szLang[16], szSubLang[16];
	LoadString(hInstance, IDS_LANG, szLang, 16);	
	LoadString(hInstance, IDS_SUBLANG, szSubLang, 16);	
	
	char szCodePage[16];
	LoadString(hInstance, IDS_CODEPAGE, szCodePage, 16);	
	g_codePage = atoi( szCodePage );

	::SetLanguageInfo( atoi( szLang ), atoi( szSubLang ) );
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	//////////////////////////////////////////////////////////////////////////
	//	BEGINTEST100111	FFL_DUMP
 	ffl_dump_unexception_filter::install( ffl_dump_level_heavy );
	//	ENDTEST100111	FFL_DUMP
	//////////////////////////////////////////////////////////////////////////

	InitLanguageFromResource( hInstance );

	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_ACCOUNTSERVER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance (hInstance, nCmdShow)) 
	{
		ExitInstance();
		return FALSE;
	}

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	ExitInstance();

	return msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_ACCOUNTSERVER);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_ACCOUNTSERVER;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
#ifndef _DEBUG
//   InitEH();
#endif	// _DEBUG

#ifdef __BILLING0712
	if( ::CreateBillingMgr() == FALSE )		// CreateWindow전에 호출되어야 한다.
		return FALSE;
#endif

	HWND hWnd;
	hInst = hInstance; // Store instance handle in our global variable
	hMainWnd = hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);	
	if( !hWnd )
		return FALSE;
	
	HMENU hMenu		= GetMenu( hWnd );
	CheckMenuItem( hMenu, IDM_OPT_INTERNAL, MF_CHECKED );
	EnableMenuItem( hMenu, IDM_OPT_INTERNAL, MF_DISABLED | MF_GRAYED );
	g_dpSrvr.m_bCheckAddr	= true;

	int x = 400, y = 416;
	SetWindowPos( hWnd, NULL, x, y, 400, 416, SWP_SHOWWINDOW );

	LOAD_WS2_32_DLL;

	::srand( timeGetTime() );

	// Script함수의 호출순서가 중요하다. (menu -> script -> createdbworker)
	if( Script( "AccountServer.ini" ) == FALSE )	
		return FALSE;

#ifdef __SECURITY_0628
	LoadResAuth( "Flyff.b" );
#endif	// __SECURITY_0628

//	if( CQuery::EnableConnectionPooling() )
//		OutputDebugString( "EnableConnectionPooling\n" );

	g_DbManager.CreateDbWorkers();
	
	if( g_dpSrvr.LoadAddrPmttd( "pmttd.ini" ) == false )
	{
		TRACE("LoadAddrPmttd false\n");
	}	
	
	if( g_dpSrvr.LoadIPCut( "IPCut.ini" ) == FALSE )
	{
		TRACE( "LoadIPCut FALSE\n" );
	}
	
	g_MyTrace.Initialize( hWnd, "SquireD", RGB( 0x00, 0x00, 0x00 ), RGB( 0xff, 0xff, 0xff ) );
	CTime time	= CTime::GetCurrentTime();
	g_MyTrace.Add( 0, FALSE, "%s", time.Format( "%Y/%m/%d %H:%M:%S" ) );
	g_MyTrace.AddLine( '\0' );
	
	if( FALSE == g_dpSrvr.StartServer( PN_ACCOUNTSRVR_0 )
		|| FALSE == g_dpDbSrvr.StartServer( PN_ACCOUNTSRVR_1 )
		|| FALSE == CDPAdbill::GetInstance()->StartServer( PN_ADBILL )
		/*
#ifdef __GIFTBOX0213
		|| FALSE == CDPWldSrvr::GetInstance()->StartServer( PN_ACCOUNTSRVR_2 )
#endif	// __GIFTBOX0213
		*/
		 )
	{
		AfxMessageBox( "Unable to start server" );
		return FALSE;
	}

#ifdef __BILLING0712
	if( GetBillingMgr()->Init( hWnd ) == false )
		return FALSE;
#endif

	SetTimer( hWnd, IDT_SENDPLAYERCOUNT, 1000 * 60, NULL );
	SetTimer( hWnd, IDT_TIME_CHECKADDR, 1000 * 30, NULL );
	if( ::GetLanguage() == LANG_THA )
		SetTimer( hWnd, IDT_PREVENT_EXCESS, 1000 * 60, NULL );

/*
#ifdef __S0114_RELOADPRO
	SetTimer( hWnd, IDT_RELOAD_PROJECT, 1000 * 30, NULL );
#endif // __S0114_RELOADPRO
*/
	SetTimer( hWnd, IDT_BUYING_INFO, 500, NULL );

	if( FALSE == g_DbManager.AllOff() )
		return FALSE;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);	
	return TRUE;
}

#ifdef __SECURITY_0628
void	LoadResAuth( LPCTSTR lpszFileName )
{
	CScanner s;
	if( s.Load( lpszFileName ) )
	{
		s.GetToken();
		lstrcpy( g_dpSrvr.m_szResVer, s.Token );
	}
}
#endif	// __SECURITY_0628

BOOL Script( LPCTSTR lpszFileName )
{
#ifdef __INTERNALSERVER	
	strcpy( g_DbManager.m_szLoginPWD, "#^#^account" );
	strcpy( g_DbManager.m_szLogPWD, "#^#^log" );
#endif

/*
AddTail( -1, 1, "TEST", "192.168.0.103" );
	AddTail( 1, 1, "1", "" );
*/

	CScanner s;
	BOOL bSkipTracking = FALSE;

	if( s.Load( lpszFileName ) )
	{
		s.GetToken();
		while( s.tok != FINISHED )
		{
			if( s.Token == ";" )
			{
				s.GetToken();
				continue;
			}
			else if( s.Token == "TEST" )
			{
				HMENU hMenu	= GetMenu( hMainWnd );
				CheckMenuItem( hMenu, IDM_OPT_EXTERNAL, MF_CHECKED );
				EnableMenuItem( hMenu, IDM_OPT_EXTERNAL, MF_DISABLED | MF_GRAYED );
				CheckMenuItem( hMenu, IDM_OPT_INTERNAL, MF_UNCHECKED );
				EnableMenuItem( hMenu, IDM_OPT_INTERNAL, MF_ENABLED );
				g_dpSrvr.m_bCheckAddr	= false;
				char lpString[MAX_LOADSTRING];
#ifdef _DEBUG
				sprintf( lpString, "%s - debug", szTitle );
#else	// _DEBUG
				sprintf( lpString, "%s - release", szTitle );
#endif	// _DEBUG
				SetWindowText( hMainWnd, lpString );
			}
			else if( s.Token == "AddTail" )
			{
				LPSERVER_DESC pServer
					= g_dpSrvr.m_aServerset + g_dpSrvr.m_dwSizeofServerset++;
				s.GetToken();	// (
				pServer->dwParent	= s.GetNumber();
				s.GetToken();	// ,
				pServer->dwID	= s.GetNumber();
				s.GetToken();	// ,
				s.GetToken();
				strcpy( pServer->lpName, s.Token );
				s.GetToken();	// ,
				s.GetToken();
				strcpy( pServer->lpAddr, s.Token );
				s.GetToken();	// ,
				pServer->b18	= (BOOL)s.GetNumber();
				s.GetToken();	// ,
				pServer->lEnable	= (long)s.GetNumber();
#ifdef __SERVERLIST0911
				pServer->lEnable	= 0L;
#endif	// __SERVERLIST0911
				s.GetToken();	// ,
				pServer->lMax	= (long)s.GetNumber();
				s.GetToken();	// )

//				if( pServer->dwParent != NULL_ID )
				{
					u_long uId	= pServer->dwParent * 100 + pServer->dwID;
					g_dpSrvr.m_2ServersetPtr.insert( map<u_long, LPSERVER_DESC>::value_type( uId, pServer ) );
				}
			}
			else if( s.Token == "MAX" )
			{
				g_dpSrvr.m_nMaxConn = s.GetNumber();
			}
			else if( s.Token == "BillingIP" )
			{
				s.GetToken();
				GetBillingMgr()->SetConfig( BID_IP, (DWORD)(LPCTSTR)s.Token );
			}
			else if( s.Token == "BillingPort" )
			{
				GetBillingMgr()->SetConfig( BID_PORT, s.GetNumber() );
			}
			else if( s.Token == "BillingFreePass" )
			{
				GetBillingMgr()->SetConfig( BID_FREEPASS, s.GetNumber() );
			}
			else if( s.Token == "BillingPWD" )
			{
				// 디비패스워드는 암호화 되어 있다. 암호 시키는 프로그램은 외부실행파일이다.
				s.GetToken();
				char* szPWD = GetBillingPWD();
				::GetPWDFromToken( s.Token, szPWD ); // from query.cpp
			}
			else if( s.Token == "MSG_VER" )
			{
				s.GetToken();
				lstrcpy( g_dpSrvr.m_szVer, s.Token );
			}
			else if( s.Token == "SKIP_TRACKING" )		// login logout로그를 남기지 않는다.
			{
				bSkipTracking = TRUE;
				g_DbManager.SetTracking( FALSE );
			}
			else if( s.Token == "DSN_NAME_LOGIN" )
			{
				s.GetToken();
				strcpy( DSN_NAME_LOGIN, s.Token );
				if( bSkipTracking == FALSE )
					g_DbManager.SetTracking( TRUE );
			}
			else if( s.Token == "DB_ADMIN_ID_LOGIN" )
			{
				s.GetToken();
				strcpy( DB_ADMIN_ID_LOGIN, s.Token );
			}
			else if( s.Token == "DSN_NAME_LOG" )
			{
				s.GetToken();
				strcpy( DSN_NAME_LOG, s.Token );
			}
			else if( s.Token == "DB_ADMIN_ID_LOG" )
			{
				s.GetToken();
				strcpy( DB_ADMIN_ID_LOG, s.Token );
			}
			else if( s.Token == "DSN_NAME_BILLING" )
			{
				s.GetToken();
				strcpy( DSN_NAME_BILLING, s.Token );
			}
			else if( s.Token == "DB_ADMIN_ID_BILLING" )
			{
				s.GetToken();
				strcpy( DB_ADMIN_ID_BILLING, s.Token );
			}
			else if( s.Token == "NOT_RELOADPRO" )
			{
				g_dpSrvr.m_bReloadPro = FALSE;
			}
			else if( s.Token == "NOLOG" )
			{
				g_DbManager.SetLogging( FALSE );	// 상용화 아이템 로그를 남기지 않는다.
			}
			else if( s.Token == "DB_PWD_LOGIN" )
			{
				s.GetToken();
				::GetPWDFromToken( s.Token, g_DbManager.m_szLoginPWD ); 
				TRACE("%s\n", g_DbManager.m_szLoginPWD ); 
			}
			else if( s.Token == "DB_PWD_LOG" )
			{
				s.GetToken();
				::GetPWDFromToken( s.Token, g_DbManager.m_szLogPWD ); 
				TRACE("%s\n", g_DbManager.m_szLogPWD ); 
			}
#ifdef __LOG_PLAYERCOUNT_CHANNEL
			else if( s.Token == "AddChannel" )
			{
				s.GetToken();
				g_dpDbSrvr.m_vecstrChannelAccount.push_back( s.Token );
			}
#endif // __LOG_PLAYERCOUNT_CHANNEL

			s.GetToken();
		}
		return TRUE;
	}

	Error( "Can't open file %s\n", lpszFileName );
	return FALSE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

#ifdef __BILLING0712
	// 빌링에 관련된 윈도우 메세지가 처리되게 한다.
	if( GetBillingMgr()->PreTranslateMessage( hWnd, message, wParam, lParam ) )
		return 0;
#endif

	switch (message) 
	{
		case WM_TIMER:
			{
				switch( wParam )
				{
				case IDT_SENDPLAYERCOUNT:
					g_dpDbSrvr.SendPlayerCount();
					break;
				case IDT_PREVENT_EXCESS:
					g_AccountMng.PreventExcess();	
					break;
				case IDT_TIME_CHECKADDR:
					{
						if( g_dpSrvr.m_bCheckAddr && g_AccountMng.IsTimeCheckAddr() )
						{
							HMENU hMenu		= GetMenu( hWnd );
							CheckMenuItem( hMenu, IDM_OPT_EXTERNAL, MF_CHECKED );
							EnableMenuItem( hMenu, IDM_OPT_EXTERNAL, MF_DISABLED | MF_GRAYED );
							CheckMenuItem( hMenu, IDM_OPT_INTERNAL, MF_UNCHECKED );
							EnableMenuItem( hMenu, IDM_OPT_INTERNAL, MF_ENABLED );
							g_dpSrvr.m_bCheckAddr	= false;
						}
					}
					break;
/*
#ifdef __S0114_RELOADPRO
				case IDT_RELOAD_PROJECT:
					{
						if( g_dpSrvr.m_bReloadPro )
							g_DbManager.Load_ReloadAccount();
					}
					break;
#endif // __S0114_RELOADPRO
*/
				case IDT_BUYING_INFO:
					CBuyingInfoMng::GetInstance()->Process();
					break;
				}
			}
			break;

		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				case IDM_OPT_INTERNAL:
					{
						HMENU hMenu		= GetMenu( hWnd );
						CheckMenuItem( hMenu, IDM_OPT_INTERNAL, MF_CHECKED );
						EnableMenuItem( hMenu, IDM_OPT_INTERNAL, MF_DISABLED | MF_GRAYED );
						CheckMenuItem( hMenu, IDM_OPT_EXTERNAL, MF_UNCHECKED );
						EnableMenuItem( hMenu, IDM_OPT_EXTERNAL, MF_ENABLED );
						g_dpSrvr.m_bCheckAddr	= true;
						break;
					}
				case IDM_OPT_EXTERNAL:
					{
						HMENU hMenu		= GetMenu( hWnd );
						CheckMenuItem( hMenu, IDM_OPT_EXTERNAL, MF_CHECKED );
						EnableMenuItem( hMenu, IDM_OPT_EXTERNAL, MF_DISABLED | MF_GRAYED );
						CheckMenuItem( hMenu, IDM_OPT_INTERNAL, MF_UNCHECKED );
						EnableMenuItem( hMenu, IDM_OPT_INTERNAL, MF_ENABLED );
						g_dpSrvr.m_bCheckAddr	= false;
						break;
					}
				case IDM_OPT_MAX_CONN:
					{
						DialogBox( hInst, MAKEINTRESOURCE( IDD_DIALOG_MAX_CONN ), hWnd, MaxConnDlgProc );
						break;
					}
				case IDM_OPT_TOGGLE:
					{
						DialogBox( hInst, MAKEINTRESOURCE( IDD_DIALOG_TOGGLE ), hWnd, ToggleDlgProc );
						break;
					}
				case IDM_OPT_SETTIME:
					{
						DialogBox( hInst, MAKEINTRESOURCE( IDD_DIALOG_TIME ), hWnd, SetTimeDlgProc );
						break;
					}
				case IDM_OPT_INTERNAL_IP:
					{
						g_dpSrvr.LoadAddrPmttd( "pmttd.ini" );
						g_dpSrvr.LoadIPCut( "IPCut.ini" );
						break;
					}
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;

		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			g_MyTrace.Paint( hdc );
			EndPaint(hWnd, &ps);
			break;

		case WM_DESTROY:
			KillTimer( hMainWnd, IDT_SENDPLAYERCOUNT );
			if( ::GetLanguage() == LANG_THA )
				KillTimer( hMainWnd, IDT_PREVENT_EXCESS );
/*
#ifdef __S0114_RELOADPRO
			KillTimer( hMainWnd, IDT_RELOAD_PROJECT );
#endif // __S0114_RELOADPRO
*/
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

void ExitInstance( void )
{
#ifdef __BILLING0712
	GetBillingMgr()->Release();
#endif
	g_dpSrvr.DeleteDPObject();
	g_dpDbSrvr.DeleteDPObject();
	CDPAdbill::GetInstance()->DeleteDPObject();
	/*
#ifdef __GIFTBOX0213
	CDPWldSrvr::GetInstance()->DeleteDPObject();
#endif	// __GIFTBOX0213
	*/

	g_AccountMng.Clear();
	SAFE_DELETE( CAccount::m_pPool );
	
	g_DbManager.CloseDbWorkers();
	
//	UninitializeNetLib();
	SAFE_DELETE( CAr::m_pHeapMng );
	SAFE_DELETE( CBuffer::m_pPool );
	SAFE_DELETE( CBuffer2::m_pPool2 );	
	SAFE_DELETE( CBuffer3::m_pPool2 );	
	SAFE_DELETE( CBuffer::m_pHeapMng );

	UNLOAD_WS2_32_DLL;
}

BOOL CALLBACK ToggleDlgProc( HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam )
{
	switch( iMessage )
	{
		case WM_INITDIALOG:
			{
				SetDlgItemText( hDlg, IDC_EDIT1, "Parent" );
				SetDlgItemText( hDlg, IDC_EDIT2, "ID" );
				break;
			}
		case WM_COMMAND:
			switch( wParam )
			{
				case IDOK:
					{
						char lpString[MAX_PATH]	= { 0, };
						DWORD dwParent	= 0;
						DWORD dwID	= 0;
						GetDlgItemText( hDlg, IDC_EDIT3, lpString, MAX_PATH );
						dwParent	= atoi( lpString );
						GetDlgItemText( hDlg, IDC_EDIT4, lpString, MAX_PATH );
						dwID	= atoi( lpString );
						
						g_dpSrvr.EnableServer( dwParent, dwID, 1L );

						EndDialog( hDlg, 0 );
						return TRUE;
					}
				case IDCANCEL:
					{
						EndDialog( hDlg, 0 );
						return TRUE;
					}
				case IDC_BUTTON_DISABLE:
					{
						char lpString[MAX_PATH]	= { 0, };
						DWORD dwParent	= 0;
						DWORD dwID	= 0;
						GetDlgItemText( hDlg, IDC_EDIT3, lpString, MAX_PATH );
						dwParent	= atoi( lpString );
						GetDlgItemText( hDlg, IDC_EDIT4, lpString, MAX_PATH );
						dwID	= atoi( lpString );

						g_dpSrvr.EnableServer( dwParent, dwID, 0L );
						
						EndDialog( hDlg, 0 );
						return TRUE;
					}
			}
			break;
	}
	return FALSE;
}

BOOL CALLBACK MaxConnDlgProc( HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam )
{
	switch( iMessage )
	{
		case WM_INITDIALOG:
			{
				char lpString[16];
				itoa( (int)g_dpSrvr.m_nMaxConn, lpString, 10 );
				SetDlgItemText( hDlg, IDC_EDIT_MAX_CONN, lpString );
				break;
			}
		case WM_COMMAND:
			switch( wParam )
			{
				case IDOK:
					{
						char lpString[16];
						GetDlgItemText( hDlg, IDC_EDIT_MAX_CONN, lpString, 16 );
						int nMaxConn	= atoi( lpString );
						if( nMaxConn > -1 )
							g_dpSrvr.m_nMaxConn		= nMaxConn;
						EndDialog( hDlg, 0 );
						return TRUE;
					}
				case IDCANCEL:
					{
						EndDialog( hDlg, 0 );
						return TRUE;
					}
			}
			break;
	}
	return FALSE;
}

void PrintTimeDlg( HWND hDlg )
{
	char lpString[16];
	sprintf( lpString, "%d", g_AccountMng.m_nYear );
	SetDlgItemText( hDlg, IDC_EDIT_YEAR, lpString );
	sprintf( lpString, "%d", g_AccountMng.m_nMonth );
	SetDlgItemText( hDlg, IDC_EDIT_MONTH, lpString );
	sprintf( lpString, "%d", g_AccountMng.m_nDay );
	SetDlgItemText( hDlg, IDC_EDIT_DAY, lpString );
	sprintf( lpString, "%d", g_AccountMng.m_nHour );
	SetDlgItemText( hDlg, IDC_EDIT_HOUR, lpString );
	sprintf( lpString, "%d", g_AccountMng.m_nMinute );
	SetDlgItemText( hDlg, IDC_EDIT_MINUTE, lpString );
}

BOOL SetTimeDlg( HWND hDlg )
{
	int nYear, nMonth, nDay, nHour, nMinute;
	char lpString[16];
	GetDlgItemText( hDlg, IDC_EDIT_YEAR, lpString, 16 );
	nYear = atoi( lpString );
	GetDlgItemText( hDlg, IDC_EDIT_MONTH, lpString, 16 );
	nMonth = atoi( lpString );
	if( nMonth <= 0 || 12 < nMonth )
	{
		AfxMessageBox( "Error Retry \"Month : 1 ~ 12\"" );
		return FALSE;
	}
	GetDlgItemText( hDlg, IDC_EDIT_DAY, lpString, 16 );
	nDay = atoi( lpString );
	if( nDay <= 0 || 31 < nDay )
	{
		AfxMessageBox( "Error Retry \"Day : 1 ~ 31\"" );
		return FALSE;
	}
	GetDlgItemText( hDlg, IDC_EDIT_HOUR, lpString, 16 );
	nHour = atoi( lpString );
	if( nHour < 0 || 23 < nHour )
	{
		AfxMessageBox( "Error Retry \"Hour : 0 ~ 23\"" );
		return FALSE;
	}
	GetDlgItemText( hDlg, IDC_EDIT_MINUTE, lpString, 16 );
	nMinute = atoi( lpString );
	if( nMinute < 0 || 59 < nMinute )
	{
		AfxMessageBox( "Error Retry \"Minute : 0 ~ 59\"" );
		return FALSE;
	}
	g_AccountMng.m_nYear	= nYear;
	g_AccountMng.m_nMonth	= nMonth;
	g_AccountMng.m_nDay		= nDay;
	g_AccountMng.m_nHour	= nHour;
	g_AccountMng.m_nMinute	= nMinute;
	return TRUE;	
}

BOOL CALLBACK SetTimeDlgProc( HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam )
{
	switch( iMessage )
	{
	case WM_INITDIALOG:
		{
			PrintTimeDlg( hDlg );
			break;
		}
	case WM_COMMAND:
		switch( wParam )
		{
		case IDOK:
			{
				if( SetTimeDlg( hDlg ) == FALSE )
				{
					return TRUE;
				}
				EndDialog( hDlg, 0 );
				return TRUE;
			}
		case IDCANCEL:
			{
				EndDialog( hDlg, 0 );
				return TRUE;
			}
		case IDCLEAR:
			{
				CTime tTime = CTime::GetCurrentTime();
				g_AccountMng.m_nYear = tTime.GetYear() - 1;
				g_AccountMng.m_nMonth = tTime.GetMonth();
				g_AccountMng.m_nDay = tTime.GetDay();
				g_AccountMng.m_nHour = tTime.GetHour();
				g_AccountMng.m_nMinute = tTime.GetMinute();
				PrintTimeDlg( hDlg );
				return TRUE;
			}
		}
		break;
	}
	return FALSE;
}
