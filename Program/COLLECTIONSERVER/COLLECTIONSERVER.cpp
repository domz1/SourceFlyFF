// COLLECTIONSERVER.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#ifdef IDC_STATIC
#undef IDC_STATIC
#endif
#include "resource.h"
#include "DPCollectionSrvr.h"
#include "DbManager.h"
#include "mytrace.h"
#include "..\_unhandledexception\exceptionhandler.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];								// The title bar text

extern	CMyTrace			g_MyTrace;
char	lpConnection[16];

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
void				ExitInstance( void );
BOOL				Script( LPCTSTR lpszFileName );
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);


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
 	// TODO: Place code here.
	InitLanguageFromResource( hInstance );

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_COLLECTIONSERVER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		ExitInstance();
		return FALSE;
	}

	// Main message loop:
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	ExitInstance();
	return msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_COLLECTIONSERVER);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_COLLECTIONSERVER;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   int x = 200, y = 416;
   SetWindowPos( hWnd, NULL, x, y, 400, 416, SWP_SHOWWINDOW );
   
   g_MyTrace.Initialize( hWnd, "SquireD", RGB( 0x00, 0x00, 0x00 ), RGB( 0xff, 0xff, 0xff ) );
   
   CTime time	= CTime::GetCurrentTime();
   g_MyTrace.Add( 0, FALSE, "%s", time.Format( "%Y/%m/%d %H:%M:%S" ) );
   g_MyTrace.AddLine( '\0' );

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   
#ifndef _DEBUG
   InitEH();
#endif	// _DEBUG

   if( Script( "Collection.ini" ) == FALSE )
	   return FALSE;
   
   if( DbManager->CreateDbWorkers() == FALSE )
   {
	   AfxMessageBox( "DbManager->CreateDbWorkers odbc connect timeout." ); 
	   return FALSE;
   }
   
   if( InitializeNetLib() == FALSE )
	   return FALSE;

#ifdef __CRC
   if( DPCollectionSrvr->StartServer( PN_COLLECTION, FALSE, CSock::crcRead ) == FALSE )
#else	// __CRC
   if( DPCollectionSrvr->StartServer( PN_COLLECTION) == FALSE )
#endif	// __CRC
	   return FALSE;

   return TRUE;
}

void ExitInstance( void )
{
	DPCollectionSrvr->DeleteDPObject();

	UninitializeNetLib();
}

BOOL Script( LPCTSTR lpszFileName )
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
			if( s.Token == "DSN_NAME_COLLECTION" )
			{
				s.GetToken();
				strcpy( DbManager->m_szDSN_NAME_COLLECTION, s.Token );
			}
			else if( s.Token == "DB_ADMIN_ID_COLLECTION" )
			{
				s.GetToken();
				strcpy( DbManager->m_szDB_ADMIN_ID_COLLECTION, s.Token );
			}
			else if( s.Token == "DB_PWD_COLLECTION" )
			{
				s.GetToken();
				::GetPWDFromToken( s.Token, DbManager->m_szDB_ADMIN_PASS_COLLECTION ); 
			}
			s.GetToken();
		}
		return TRUE;
	}

	Error( "Can't open file, %s.\n", lpszFileName );
	return FALSE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);

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
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			g_MyTrace.Paint( hdc );
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}
