// CacheServer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#ifdef IDC_STATIC
#undef IDC_STATIC
#endif
#include "resource.h"
#include "DPCoreClient.h"
#include "DPClient.h"
#include "DPCacheSrvr.h"
#include "MyTrace.h"
#include "player.h"
#include "..\_UnhandledException\exceptionhandler.h"
#include "..\Resource\Lang.h"

#ifdef __PL_0917
#include "packetlog.h"
#endif	// __PL_0917

#ifdef __CRASH_0404
#include "crashstatus.h"
#endif	// __CRASH_0404

#define MAX_LOADSTRING 100

extern	CDPCoreClient	g_DPCoreClient;
extern	CDPClientArray	g_DPClientArray;
extern	CDPCacheSrvr	g_DPCacheSrvr;
extern	CMyTrace		g_MyTrace;

HINSTANCE				hInst;
TCHAR					szTitle[MAX_LOADSTRING];
TCHAR					szWindowClass[MAX_LOADSTRING];
char					g_szCoreAddr[16];
HWND					hMainWnd;
u_short					g_uPort = PN_CACHESRVR;

ATOM				MyRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL				InitInstance(HINSTANCE, int);
void				ExitInstance( void );
BOOL				Script( LPCSTR lpszFileName );



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
	LoadString(hInstance, IDC_CACHESERVER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

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
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_CACHESERVER);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_CACHESERVER;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	hMainWnd	= hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	  CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
		return FALSE;

	SetTimer( hMainWnd, 3, SEC( 60 ), NULL );	// 10(s)

	int x = 480, y = 606;
	SetWindowPos( hWnd, NULL, x, y, 400, 416, SWP_SHOWWINDOW );

	g_MyTrace.Initialize( hWnd, "SquireD", RGB( 0x00, 0x00, 0x00 ), RGB( 0xff, 0xff, 0xff ) );

	CTime time	= CTime::GetCurrentTime();
	g_MyTrace.Add( 0, FALSE, "%s", time.Format( "%Y/%m/%d %H:%M:%S" ) );
	g_MyTrace.AddLine( '\0' );

#ifndef _DEBUG
//   InitEH();
#endif	// _DEBUG

	if( !Script( "CacheServer.ini" ) )
		return FALSE;

	if( InitializeNetLib() == FALSE )
		return FALSE;

	if( !g_DPCoreClient.ConnectToServer( g_szCoreAddr, PN_CORESRVR + 1 ) )
		return FALSE;
	
#ifdef __CRC
	if( !g_DPCacheSrvr.StartServer( g_uPort, FALSE, CSock::crcRead ) )
#else	// __CRC
	if( !g_DPCacheSrvr.StartServer( g_uPort ) )
#endif	// __CRC
		return FALSE;


	g_MyTrace.AddLine( '-' );
	char lpszIpAddr[16];
	g_DPCoreClient.GetPlayerAddr( DPID_SERVERPLAYER, lpszIpAddr );

	g_MyTrace.Add( CMyTrace::Key( "main" ), FALSE, "%s", lpszIpAddr );
	g_MyTrace.AddLine( '-' );

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

void ExitInstance( void )
{
	g_DPCoreClient.DeleteDPObject();
	g_DPClientArray.Free();
	g_DPCacheSrvr.DeleteDPObject();

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
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
#ifdef __PL_0917
		case WM_LBUTTONDOWN:
			CPacketLog::Instance()->Print();
			break;
		case WM_RBUTTONDOWN:
			CPacketLog::Instance()->Reset();
			break;
#endif	// __PL_0917
		case WM_TIMER:
			{
				switch( wParam )
				{
					case 3:
#ifdef __CRASH_0404
						CCrashStatus::GetInstance()->SetMainThreadTask( 1 );
#endif	// __CRASH_0404
						CPlayerMng::Instance()->DestroyGarbage();
#ifdef __CRASH_0404
						CCrashStatus::GetInstance()->SetMainThreadTask( 0 );
#endif	// __CRASH_0404

						break;
				}
				break;
			}
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
//			RECT rt;
//			GetClientRect(hWnd, &rt);
			g_MyTrace.Paint( hdc );
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			KillTimer( hMainWnd, 3 );
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
			if( s.Token == "Core" )
			{
				s.GetToken();
				strcpy( g_szCoreAddr, s.Token );
			}
			else if( s.Token == "Port" )
			{
				g_uPort = s.GetNumber();		
			}
			s.GetToken();
		}
		return TRUE;
	}
	Error( "Can't open file, %s.\n", lpszFileName );
	return FALSE;
};