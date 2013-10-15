// CoreServer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#ifdef IDC_STATIC
#undef IDC_STATIC
#endif
#include "resource.h"
#include "dpcoresrvr.h"
#include "dpcachesrvr.h"
#include "dploginsrvr.h"
#include "msghdr.h"
#include "MyTrace.h"
#include "..\_UnhandledException\ExceptionHandler.h"
#include "xutil.h"
#include "dpdatabaseclient.h"
extern	CDPDatabaseClient g_dpDatabaseClient;

#include "lang.h"

#include "guild.h"
extern	CGuildMng	g_GuildMng;
#include "guildwar.h"
extern	CGuildWarMng	g_GuildWarMng;

extern	CDPCoreSrvr		g_dpCoreSrvr;
extern	CDPCacheSrvr	g_DPCacheSrvr;
extern	CDPLoginSrvr	g_dpLoginSrvr;
extern	CMyTrace	g_MyTrace;

#include "party.h"
extern	CPartyMng	g_PartyMng;

#include "Environment.h"

#ifndef __ENVIRONMENT_EFFECT
extern	CEnvironment	g_Environment;
#endif // __ENVIRONMENT_EFFECT

#include "player.h"
extern	CPlayerMng	g_PlayerMng;

#ifdef __EVENT0913
extern	BOOL	s_bEvent0913;
#endif	// __EVENT0913
#ifdef __EVENT1206
extern	BOOL	s_bEvent1206;
#endif	// __EVENT1206

extern	BOOL	s_bTest;

extern	DWORD	s_dwEvent;
#include "flyffevent.h"

/////////////////////////////////////////////////////////////
#define			MAX_LOADSTRING	100
const WORD		TIMER_ID_DEFAULT = 100;
const WORD		TIMER_ID_WANTED  = 200;
const WORD		TIMER_ID_EVENT0913	= 300;
const WORD		TIMER_ID_EVENT1206	= 400;

const	WORD	TIMER_ID_EVENT	= 500;

#ifdef __INTERNALSERVER
	const UINT g_uWantedElapse = SEC( 30 );
#else
	const UINT g_uWantedElapse = MIN( 10 );
#endif //__INTERNALSERVER

/////////////////////////////////////////////////////////////
HINSTANCE	hInst;
HWND		hMainWnd;
TCHAR		szTitle[MAX_LOADSTRING];
TCHAR		szWindowClass[MAX_LOADSTRING];
DWORD		g_dwSys;
DWORD		g_dwId;
CHAR		szCertifierAddr[16];
CHAR		szDataBaseAddr[16];


ATOM				MyRegisterClass(HINSTANCE hInstance);
LRESULT	CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL				InitInstance(HINSTANCE, int);
void				ExitInstance( void );
BOOL				Script( LPCSTR lpszFileName );
void				OnTimer( WORD wTimerID );



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

/////////////////////////////////////////////////////////////
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

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CORESERVER, szWindowClass, MAX_LOADSTRING);
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
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_CORESERVER);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
//	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.hbrBackground	= (HBRUSH)COLOR_MENUTEXT;
	wcex.lpszMenuName	= (LPCSTR)IDC_CORESERVER;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

	hMainWnd	= hWnd	=
		CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

	int x = 0, y = 416;
	SetWindowPos( hWnd, NULL, x, y, 400, 416, SWP_SHOWWINDOW );

	g_GameTimer.Compute();
	SetTimer( hWnd, TIMER_ID_DEFAULT, 1000, NULL );

#if !defined(__INTERNALSERVER)
	SetTimer( hWnd, TIMER_ID_WANTED, g_uWantedElapse, NULL );
#endif

#ifdef __EVENT0913
	SetTimer( hWnd, TIMER_ID_EVENT0913, 1000, NULL );
#endif	// __EVENT0913
#ifdef __EVENT1206
	SetTimer( hWnd, TIMER_ID_EVENT1206, 1000 * 60, NULL );
#endif	// __EVENT1206

	SetTimer( hWnd, TIMER_ID_EVENT, 1000 * 60, NULL );

	g_MyTrace.Initialize( hWnd, "SquireD", RGB( 0xff, 0xff, 0xff ), RGB( 0x00, 0x00, 0x00 ) );

	CTime time	= CTime::GetCurrentTime();
	g_MyTrace.Add( 0, FALSE, "%s", time.Format( "%Y/%m/%d %H:%M:%S" ) );
	g_MyTrace.AddLine( '\0' );

#ifndef _DEBUG
//	InitEH();
#endif	// _DEBUG

	g_TickCount.Start();

	if( !Script( "CoreServer.ini" ) )
	{
		return FALSE;
	}
	
	if( InitializeNetLib() == FALSE )
		return FALSE;

	if( !g_dpCoreSrvr.StartServer( PN_CORESRVR + 0 ) )
		return FALSE;

	if( !g_DPCacheSrvr.StartServer( PN_CORESRVR + 1 ) )
		return FALSE;

	if( !g_dpLoginSrvr.StartServer( PN_CORESRVR + 2 ) )
		return FALSE;

	if( !g_dpDatabaseClient.ConnectToServer( szDataBaseAddr, PN_DBSRVR_2, FALSE ) )
	{
		Error( "Not TransServer Connect" );
		return FALSE;
	}

	if( !g_PartyMng.CreateWorkers() )
	{
		assert( 0 );
	}

#ifdef __ENVIRONMENT_EFFECT
	if( CEnvironment::GetInstance()->CreateWorkers() == FALSE )
	{
		assert( 0 );
	}
#else // __ENVIRONMENT_EFFECT
	if( !g_Environment.CreateWorkers() )
	{
		assert( 0 );
	}
#endif // __ENVIRONMENT_EFFECT

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

void ExitInstance( void )
{
	g_dpCoreSrvr.DeleteDPObject();
	g_DPCacheSrvr.DeleteDPObject();
	g_dpLoginSrvr.DeleteDPObject();
	g_dpDatabaseClient.DeleteDPObject();

	g_PartyMng.Clear();

	g_PlayerMng.Free();

	SAFE_DELETE( CParty::m_pPool );
	SAFE_DELETE( CPlayer::m_pPool );
	g_GuildMng.m_AddRemoveLock.Enter( theLineFile );
	g_GuildWarMng.Clear();
	g_GuildMng.m_AddRemoveLock.Leave( theLineFile );

	g_GuildMng.Clear();
	SAFE_DELETE( CGuildMember::sm_pPool );
	SAFE_DELETE( CGuild::sm_pPool );

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
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_TIMER:
			{
				OnTimer( wParam ); // wTimerID = wParam 
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
			KillTimer( hMainWnd, TIMER_ID_DEFAULT );
#if !defined(__INTERNALSERVER)
			KillTimer( hMainWnd, TIMER_ID_WANTED );
#endif
#ifdef __EVENT0913
			KillTimer( hMainWnd, TIMER_ID_EVENT0913 );
#endif	// __EVENT0913
#ifdef __EVENT1206
			KillTimer( hMainWnd, TIMER_ID_EVENT1206 );
#endif	// __EVENT1206
			KillTimer( hMainWnd, TIMER_ID_EVENT );
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

void OnTimer( WORD wTimerID )
{
	if( wTimerID == TIMER_ID_DEFAULT )
	{
		g_GameTimer.Compute();

	#ifdef __EVE_NEWYEAR
		static BOOL s_bCountdown	= FALSE;
		CTime tmCur		= CTime::GetCurrentTime();
		if( ::GetLanguage() == LANG_GER || ::GetLanguage() == LANG_FRE )
		{
			if( !s_bCountdown && tmCur > CTime( 2009, 12, 31, 22, 58, 59 ) && tmCur < CTime( 2009, 12, 31, 23, 0, 0 ) )
			{
				s_bCountdown	= TRUE;
				g_DPCacheSrvr.SendHdr( PACKETTYPE_NEWYEAR, DPID_ALLPLAYERS, DPID_ALLPLAYERS );
			}
			if( tmCur > CTime( 2009, 12, 31, 23, 0, 0 ) )
				s_bCountdown	= FALSE;
		}
		else
		{
			if( !s_bCountdown && tmCur > CTime( 2009, 12, 31, 23, 58, 59 ) && tmCur < CTime( 2010, 1, 1, 0, 0, 0 ) )
			{
				s_bCountdown	= TRUE;
				g_DPCacheSrvr.SendHdr( PACKETTYPE_NEWYEAR, DPID_ALLPLAYERS, DPID_ALLPLAYERS );
			}
			if( tmCur > CTime( 2010, 1, 1, 0, 0, 0 ) )
				s_bCountdown	= FALSE;
		}
	#endif	// __EVE_NEWYEAR

		return;
	}

#if !defined(__INTERNALSERVER)
	if( wTimerID == TIMER_ID_WANTED )	// 현상금 처리 
	{
		g_dpCoreSrvr.SendCWWantedList();	// 현상금 리스트를 보낸다.
		return;
	}
#endif // __INTERNALSERVER

#ifdef __EVENT0913
	if( wTimerID == TIMER_ID_EVENT0913 )
	{
		CTime	tmCurr	= CTime::GetCurrentTime();
		BOOL bEventTime		= ( tmCurr.GetHour() >= 17 && tmCurr.GetHour() < 18 ) || ( tmCurr.GetHour() >= 22 && tmCurr.GetHour() < 23 );
		if( bEventTime && !s_bEvent0913 )	// 이벤트 시간 - 이벤트 설정되어 있지 않으면,
		{
			g_dpCoreSrvr.SendEvent0913( s_bEvent0913 = TRUE );
		}
		else if( !bEventTime && s_bEvent0913 )	// 이벤트 시간이 아닌데 이벤트 설정되어 있으면,
		{
			g_dpCoreSrvr.SendEvent0913( s_bEvent0913 = FALSE );
		}
	}
#endif	// __EVENT0913
#ifdef __EVENT1206
	if( wTimerID == TIMER_ID_EVENT1206 )
	{
		CTime	tmCurr	= CTime::GetCurrentTime();
		BOOL bEventTime;
		if( s_bTest )
		{
			bEventTime	= ( tmCurr.GetHour() % 2 == 0 );	// even
		}
		else
		{
			bEventTime	= ( tmCurr >= CTime( 2005, 12, 10, 19, 0, 0 ) && tmCurr < CTime( 2005, 12, 10, 22, 0, 0 ) )
						|| ( tmCurr >= CTime( 2005, 12, 11, 19, 0, 0 ) && tmCurr < CTime( 2005, 12, 11, 22, 0, 0 ) );
		}
		if( bEventTime && !s_bEvent1206 )	// 이벤트 시간 - 이벤트 설정되어 있지 않으면,
		{
			g_dpCoreSrvr.SendEvent1206( s_bEvent1206 = TRUE );
		}
		else if( !bEventTime && s_bEvent1206 )	// 이벤트 시간이 아닌데 이벤트 설정되어 있으면,
		{
			g_dpCoreSrvr.SendEvent1206( s_bEvent1206 = FALSE );
		}
	}
#endif	// __EVENT1206

	if( wTimerID == TIMER_ID_EVENT )
	{
		BOOL	f	= FALSE;
		CTime	tmCurr	= CTime::GetCurrentTime();

		BOOL bXmas;
		if( s_bTest )
		{
			bXmas	= tmCurr < CTime( 2005, 12, 26, 10, 0, 0 );
		}
		else
		{
			bXmas	= ( tmCurr >= CTime( 2005, 12, 23, 0, 0, 0 ) && tmCurr < CTime( 2005, 12, 27, 9, 0, 0 ) );
		}

		BOOL bNewYear;
		if( s_bTest )
		{
			bNewYear	= tmCurr < CTime( 2006, 1, 31, 10, 0, 0 );
		}
		else
		{
			bNewYear	= ( tmCurr >= CTime( 2006, 1, 26, 0, 0, 0 ) && tmCurr < CTime( 2006, 1, 31, 10, 0, 0 ) );
		}

		BOOL bValentinesDay;
		if( s_bTest )
		{
			bValentinesDay	= tmCurr < CTime( 2006, 2, 15, 13, 0, 0 );
		}
		else
		{
			bValentinesDay	= ( tmCurr >= CTime( 2006, 2, 14, 0, 0, 0 ) && tmCurr < CTime( 2006, 2, 15, 13, 0, 0 ) );
		}
			
		if( bXmas && !( s_dwEvent & EVE_FLAG1219 ) )
		{
			s_dwEvent	|= EVE_FLAG1219;
			f	= TRUE;
		}
		else if( !bXmas && ( s_dwEvent & EVE_FLAG1219 ) )
		{
			s_dwEvent	&= ~EVE_FLAG1219;
			f	= TRUE;
		}

		if( bNewYear && !( s_dwEvent & EVE_FLAG0127 ) )
		{
			s_dwEvent	|= EVE_FLAG0127;
			f	= TRUE;
		}
		else if( !bNewYear && ( s_dwEvent & EVE_FLAG0127 ) )
		{
			s_dwEvent	&= ~EVE_FLAG0127;
			f	= TRUE;
		}
		
		if( bValentinesDay && !( s_dwEvent & EVE_FLAG0214 ) )
		{
			s_dwEvent	|= EVE_FLAG0214;
			f	= TRUE;
		}
		else if( !bValentinesDay && ( s_dwEvent & EVE_FLAG0214 ) )
		{
			s_dwEvent	&= ~EVE_FLAG0214;
			f	= TRUE;
		}

		if( f )
		{
			g_dpCoreSrvr.SendEvent( s_dwEvent );
		}
	}
}

BOOL Script( LPCSTR lpszFileName )
{
	CScanner s;
	CServerDesc* pServer;
	POINT topLeft;
	SIZE	size;

	if( s.Load( lpszFileName ) )
	{
		g_MyTrace.AddLine( '-' );

		s.GetToken();

		while( s.tok != FINISHED )
		{
			if( s.Token == "Sys" )
			{
				g_dwSys	= s.GetNumber();
			}
			else if( s.Token ==  "dwId" )
			{
				g_dwId = s.GetNumber();
			}
			else if( s.Token == "Database" )
			{
				s.GetToken();
				strcpy( szDataBaseAddr, s.Token );
			}
			else if( s.Token == "Certifier" )
			{
				s.GetToken();
				strcpy( szCertifierAddr, s.Token );
			}
			else if( s.Token == "TEST" )
			{
				s_bTest	= TRUE;			
			}
			else if( s.Token == "PartyExpRate" )
			{
				s_fPartyExpRate		= s.GetFloat();
			}
			else
			{
				pServer		= new CServerDesc;
				u_long uKey	= (u_long)_ttoi( s.Token );
				pServer->SetKey( uKey );

				if( s.GetToken() == DELIMITER )
				{
					while( s.GetToken() != DELIMITER )
					{
						CJurisdiction* pJurisdiction	= new CJurisdiction;
						pJurisdiction->m_dwWorldID	= (DWORD)_ttoi( s.Token );
						topLeft.x	= s.GetNumber();	topLeft.y	= s.GetNumber();
						size.cx		= s.GetNumber();	size.cy		= s.GetNumber();
						pJurisdiction->m_rect.SetRect( topLeft.x, topLeft.y, topLeft.x + size.cx, topLeft.y + size.cy );
						pJurisdiction->m_wLeft	= s.GetNumber();	pJurisdiction->m_wRight		= s.GetNumber();
						pServer->m_lspJurisdiction.push_back( pJurisdiction );
					}
				}
		#ifdef __STL_0402
				bool bResult	= g_dpCoreSrvr.m_apSleepServer.insert( CServerDescArray::value_type( pServer->GetKey(), pServer ) ).second;
				ASSERT( bResult );
		#else	// __STL_0402
				g_dpCoreSrvr.m_apSleepServer.SetAt( pServer->GetKey(), pServer );
		#endif	// __STL_0402
				g_MyTrace.Add( pServer->GetKey(), TRUE, "%04d", pServer->GetKey() );
			}
			s.GetToken();
		}
		g_MyTrace.AddLine( '-' );

		return TRUE;
	}

	Error( "Can't open file, %s.\n", lpszFileName );
	return FALSE;
}