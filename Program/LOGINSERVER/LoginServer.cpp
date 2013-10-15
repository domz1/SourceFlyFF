// LoginServer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#ifdef IDC_STATIC
#undef IDC_STATIC
#endif
#include "resource.h"
#include "dploginsrvr.h"
#include "dpdatabaseclient.h"
#include "dpcoreclient.h"
#include "mytrace.h"
#include "user.h"
#include "..\_UnhandledException\ExceptionHandler.h"
#include "ggsrv26.h"			// nprotect관련 _Network폴더에 


extern	CDPLoginSrvr		g_dpLoginSrvr;
extern	CDPDatabaseClient	g_dpDBClient;
extern	CDPCoreClient		g_dpCoreClient;
extern	CMyTrace			g_MyTrace;
extern	CUserMng			g_UserMng;


#define MAX_LOADSTRING 100

HINSTANCE	hInst;	// current instance
TCHAR		szTitle[MAX_LOADSTRING];	// The title bar text
TCHAR		szWindowClass[MAX_LOADSTRING];	// The title bar text
HWND		hMainWnd;


ATOM	MyRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL	InitInstance(HINSTANCE, int);
void	ExitInstance( void );
BOOL	Script( LPCSTR lpszFileName );

//////////
#define NPROTECT_UPDATE_TIMER	1234

GGAUTHS_API void NpLog(int mode, char* msg)
{
	if(mode & (NPLOG_DEBUG | NPLOG_ERROR)) //Select log mode. 
		Error( msg );
};

GGAUTHS_API void GGAuthUpdateCallback(PGG_UPREPORT report)
{
	Error("GGAuth version update [%s] : [%ld] -> [%ld] \n", 
		report->nType==1?"GameGuard Ver":"Protocol Num", 
		report->dwBefore, report->dwNext); 
};
//////////




//////////////////////////////////////////////////////////////////////////
//	BEGINTEST100111	FFL_DUMP
#include "../__ffl_dump/include/ffl_dump_unexception_filter.h"

#ifdef	_DEBUG
#pragma comment( lib, "../__lib/win32/msvc71/ffl_dumpd.lib" )
#else
#pragma comment( lib, "../__lib/win32/msvc71/ffl_dumpr.lib" )
#endif
//	ENDTEST100111	FFL_DUMP
//////////////////////////////////////////////////////////////////////////




void InitLanguageFromResource( HINSTANCE hInstance )
{
	char szLang[16], szSubLang[16];
	LoadString(hInstance, IDS_LANG, szLang, 16);	
	LoadString(hInstance, IDS_SUBLANG, szSubLang, 16);	
	
	char szCodePage[16];
	LoadString(hInstance, IDS_CODEPAGE, szCodePage, 16);	
	g_codePage = atoi( szCodePage );

	::SetLanguageInfo( atoi( szLang ), atoi( szSubLang ) );

#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
	char sz2ndPassWord[2];
	LoadString( hInstance, IDS_2ND_PASSWORD, sz2ndPassWord, 2 );
	::SetUse2ndPassWord( static_cast<BOOL>( atoi( sz2ndPassWord ) ) );
#endif // __2ND_PASSWORD_SYSTEM
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
// 	//////////////////////////////////////////////////////////////////////////



 	InitLanguageFromResource( hInstance );

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_LOGINSERVER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
		return FALSE;

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
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_LOGINSERVER);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_LOGINSERVER;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	if( !Script( "LoginServer.ini" ) )
		return FALSE;

	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	hMainWnd	= hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if( !hWnd )
	{
		return FALSE;
	}

	SetTimer( hWnd, 0, 60 * 1000, NULL );

	int x = 880, y = 606;
	SetWindowPos( hWnd, NULL, x, y, 400, 416, SWP_SHOWWINDOW );

	g_MyTrace.Initialize( hWnd, "SquireD", RGB( 0x00, 0x00, 0x00 ), RGB( 0xff, 0xff, 0xff ) );

	CTime time	= CTime::GetCurrentTime();
	g_MyTrace.Add( 0, FALSE, "%s", time.Format( "%Y/%m/%d %H:%M:%S" ) );
	g_MyTrace.AddLine( '\0' );

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

#ifndef _DEBUG
//   InitEH();
#endif	// _DEBUG

	if( InitializeNetLib() == FALSE )
		return FALSE;
	if( !g_dpDBClient.ConnectToServer( g_lpDBAddr, PN_DBSRVR_0 ) )
	{
		return FALSE;
	}

	if( !g_dpCoreClient.ConnectToServer( g_lpCoreAddr, PN_CORESRVR + 2 ) )
	{
		return FALSE;
	}
	g_dpCoreClient.QueryTickCount();	// block,

#ifdef __CRC
	if( !g_dpLoginSrvr.StartServer( PN_LOGINSRVR, FALSE, CSock::crcRead ) )
#else	// __CRC
	if( !g_dpLoginSrvr.StartServer( PN_LOGINSRVR ) )
#endif	// __CRC
		return FALSE;

	if( g_bNProtectAuth )
	{
//		DWORD dw = InitGameguardAuth( NULL, 5, true, 0x3 );
//		DWORD dw = InitGameguardAuth( NULL, 1000, true, 0 );
		DWORD dw = InitGameguardAuth( NULL, 1000, true, 0x03 );
		if( dw != ERROR_SUCCESS  )
		{
			char s[128];
			sprintf( s, "InitGameguardAuth//%d - check ggauth*.dll", dw );
			MessageBox( hWnd, s, "", MB_OK );
			return FALSE;
		}
		SetTimer( hWnd, 1, SEC( 1 ), NULL );
		SetTimer( hWnd, NPROTECT_UPDATE_TIMER, MIN( 5 ), NULL );	//5분 주기 권장
	}

	return TRUE;
}

void ExitInstance( void )
{
	if( g_bNProtectAuth )
		CleanupGameguardAuth();

	g_dpLoginSrvr.DeleteDPObject();
	g_dpCoreClient.DeleteDPObject();
	g_dpDBClient.DeleteDPObject();
	g_UserMng.Free();
	SAFE_DELETE( CUser::m_pPool );

	UninitializeNetLib();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
				   break;
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				case IDM_DO_UPDATE_GAMEGUARD_AUTH_DLL:
					{
						AddAuthProtocol( "./ggauth01.dll" );
						break;
					}
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_TIMER:
			{
				WORD wTimerID	= wParam;
				switch( wTimerID )
				{
					case 0:
						g_UserMng.DestroyAbnormalPlayer();
						break;
					case 1:
						if( g_bNProtectAuth )
							g_UserMng.CheckAll();
						break;
					case NPROTECT_UPDATE_TIMER:
						GGAuthUpdateTimer();
						break;
				}
				break;
			}
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			g_MyTrace.Paint( hdc );
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			KillTimer( hMainWnd, 0 );
			if( g_bNProtectAuth )
				KillTimer( hMainWnd, 1 );
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

BOOL Script( LPCSTR lpszFileName )
{
	CScanner s;

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
			else if( s.Token == "DB" )
			{
				s.GetToken();
				strcpy( g_lpDBAddr, s.Token );
			}
			else if( s.Token == "Core" )
			{
				s.GetToken();
				strcpy( g_lpCoreAddr, s.Token );
			}
			else if( s.Token == "AddCache" )
			{
				s.GetToken();	// (
				s.GetToken();	// ip
				g_dpLoginSrvr.AddCahce( s.Token );
				s.GetToken();	// )
			}
			else if( s.Token == "MSG_VER" )
			{
				s.GetToken();
				strcpy( g_szMSG_VER, s.Token );
			}
			else if( s.Token == "NPROTECT" )
			{
//				s.GetToken();
				g_bNProtectAuth = TRUE;
			}
			s.GetToken();
		}

		if( g_szMSG_VER[0] == 0 )
			strcpy( g_szMSG_VER, "20040706" );
		return TRUE;
	}

	Error( "Can't open file, %s.\n", lpszFileName );
	return FALSE;
};