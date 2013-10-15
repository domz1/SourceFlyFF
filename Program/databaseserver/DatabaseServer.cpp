// DatabaseServer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#ifdef IDC_STATIC
#undef IDC_STATIC
#endif
#include "resource.h"
#include "dploginsrvr.h"
#include "dptrans.h"
#include "dpaccountclient.h"
#include "MyTrace.h"
#include "..\_Network\Objects\Obj.h"
#include "..\_UnhandledException\exceptionhandler.h"
#include "xutil.h"
#include "guild.h"
extern	CGuildMng	g_GuildMng;

#ifdef __PROVIDE
#include "defineitem.h"

#if __VER >= 19
#include "defineItemGrade.h"
#include "defineItemType.h"
#endif//__VER >= 19

#endif	// __PROVIDE

#include "post.h"

#include <mmsystem.h>

#if __VER >= 12 // __TAX
#include "dbcontroller.h"
#endif	// __TAX

extern	CDPLoginSrvr	g_dpLoginSrvr;
extern	CDPAccountClient	g_dpAccountClient;
extern	CMyTrace	g_MyTrace;
extern	CProject	prj;

#include "DPCoreSrvr.h"
extern	CDPCoreSrvr	g_dpCoreSrvr;

#include "spevent.h"

#if __VER >= 12 // __LORD
#include "tlord.h"
#endif	// __LORD

#define MAX_LOADSTRING 100

extern	APP_INFO	g_appInfo;

#include "dpaccountclient.h"
extern	CDPAccountClient	g_dpAccountClient;
char	lpConnection[16];

BOOL	m_bBankToItemSendTbl	= FALSE;
BOOL	m_bInventoryToItemSendTbl	= FALSE;
BOOL	s_bItemRemove	= FALSE;
BOOL	s_bRemoveInvalidItem	= FALSE;

#include "DbController.h"

#if __VER >= 12 // __TAX
#include "Tax.h"
#endif // __TAX

#ifdef __QUIZ
#include "QuizDBCtrl.h"
#endif // __QUIZ

#if __VER >= 15 // __GUILD_HOUSE
#include "GuildHouseDBCtrl.h"
#endif //__GUILD_HOUSE

#if __VER >= 15 // __CAMPUS
#include "CampusDBCtrl.h"
#endif // __CAMPUS

TCHAR szTitle[MAX_LOADSTRING];	// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];	// The title bar text

// Foward declarations of functions included in this code module:
ATOM	MyRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL	InitInstance(HINSTANCE, int);
void	ExitInstance( void );
BOOL	Script( LPCSTR lpszFileName );
BOOL	LoadLastItem( LPCSTR lpszFileName );
BOOL	LoadRemoveItem( LPCSTR lpszFileName );



#define	TID_MEM_RELEASE		0
#define TID_RELOAD_PROJECT  1
#define	TID_POSTPROC	2
#define	TID_QUERY_REMOVE_GUILD_BANK_TBL	3
#define	TIMER_ID_EVENT_GENERIC	4

#if defined(__INTERNALSERVER)
	static char g_szINI[] = "databaseserver.ini";
#else
	static char g_szINI[] = "..\\script\\databaseserver.ini";
#endif



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
	//////////////////////////////////////////////////////////////////////////


 	InitLanguageFromResource( hInstance );

	LoadString( hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING );
	LoadString( hInstance, IDC_DATABASESERVER, szWindowClass, MAX_LOADSTRING );
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
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_DATABASESERVER);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
//	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.hbrBackground	= (HBRUSH)COLOR_MENUTEXT;
	wcex.lpszMenuName	= (LPCSTR)IDC_DATABASESERVER;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	hMainWnd	= hWnd	= CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	  return FALSE;

	// Item 시리얼 번호 생성 랜덤값 초기화
	xSRand( timeGetTime()  );	// 속도를 요하는 랜덤은 xRandom()을 쓰도록...

#ifndef _DEBUG
//   InitEH();
#endif	// _DEBUG

	if( InitializeNetLib() == FALSE )
		return FALSE;

	if( Script( g_szINI ) == FALSE )
	{
		char szMsg[256];
		sprintf( szMsg, "FAIL read ini - %s", g_szINI );
		AfxMessageBox( szMsg, MB_OK );
		return FALSE;
	}
	
	if( CQuery::EnableConnectionPooling() )
		OutputDebugString( "EnableConnectionPooling\n" );
	g_DbManager.CreateDbWorkers();

/*
#ifdef __S0114_RELOADPRO
	SetTimer( hWnd, TID_RELOAD_PROJECT, SEC( 30 ), NULL );
#endif // __S0114_RELOADPRO
*/
#ifdef __INTERNALSERVER
	SetTimer(  hWnd, TID_QUERY_REMOVE_GUILD_BANK_TBL, MIN( 1 ), NULL );
#else	// __INTERNALSERVER
	SetTimer(  hWnd, TID_QUERY_REMOVE_GUILD_BANK_TBL, MIN( 10 ), NULL );
#endif	// __INTERNALSERVER

#ifdef __VERIFY_PLAYER
	g_DbManager.VerifyPlayer();
	return FALSE;
#endif	// __VERIFY_PLAYER
#ifdef __PROVIDE
	return TRUE;
#endif	// __PROVIDE
//______________________________________________________________________
#ifdef __CONV_SKILL
	g_DbManager.ConvSkill();
	if( MB_OK != AfxMessageBox( "Complete Skill Convert", MB_OK ) )
		return FALSE;
	return FALSE;
#endif	// __CONV_SKILL
#ifdef __CONV_SKILL_11_MONTH_JOB1
	AfxMessageBox( "Start Skill Convert 11_Month Point", MB_OK );
	g_DbManager.ConvSkill_11_MONTH_POINT();
	if( MB_OK != AfxMessageBox( "Complete Skill Convert 11_Month Point", MB_OK ) )
		return FALSE;
	return FALSE;	
#endif // __CONV_SKILL_11_MONTH_JOB1
#ifdef __CONV_SKILL_11_MONTH_JOB2
	AfxMessageBox( "Start Skill Convert 11_Month", MB_OK );
	g_DbManager.ConvSkill_11_MONTH();
	if( MB_OK != AfxMessageBox( "Complete Skill Convert 11_Month", MB_OK ) )
		return FALSE;
	return FALSE;	
#endif // __CONV_SKILL_11_MONTH_JOB2
#ifdef __CONV_SERIAL_NUMBER
	g_DbManager.SN();
	return FALSE;
#endif	// __CONV_SERIAL_NUMBER
#ifdef __CONV_SKILL_STOP
	g_DbManager.ConvSkillStop();
	if( MB_OK != AfxMessageBox( "완료 컨버트 스킬 스톱", MB_OK ) )
		return FALSE;
#endif // __CONV_SKILL_STOP
	
#ifdef __S0707_ITEM_CONV
	if( g_DbManager.ConvItem() )	// // 아이템 프로퍼티가 바뀌었을시 사용
	{
		if( MB_OK != AfxMessageBox( "완료 컨버트 Item", MB_OK ) )
			return FALSE;
	}
	else
	{
		if( MB_OK != AfxMessageBox( "실패 컨버트 Item", MB_OK ) )
			return FALSE;
	}
#endif // __S0707_ITEM_CONV
		
#ifdef __ITEM_REMOVE_LIST
	if( prj.IsConvMode( CONVER_ITEM_START ) )
	{
		if( prj.IsConvMode( REMOVE_ITEM_ALL ) )
		{
			if( LoadRemoveItem( "LoadRemoveItemList.ini" ) == FALSE )
			{
				AfxMessageBox( "LoadRemoveItemList.ini FAILE", MB_OK );
				return FALSE;
			}			
		}
		if( g_DbManager.ConvItemStart() )
		{
			AfxMessageBox( "ConvItem Success!", MB_OK );
		}
		return FALSE;
	}
#endif // __ITEM_REMOVE_LIST
	

#ifdef __RECOVER0816
	if( g_DbManager.RecoverCharacter() )
	{
		AfxMessageBox( "3" );
		return FALSE;
	}
	else
	{
		AfxMessageBox( "4" );
	}
#endif	// __RECOVER0816
//______________________________________________________________________


#if __VER >= 11 // __SYS_PLAYER_DATA
	if( !g_DbManager.LoadPlayerData() )
		return FALSE;
#else	// __SYS_PLAYER_DATA
	if( !g_DbManager.OpenPlayerID() )
	{
		ASSERT( 0 );
	}
#endif	// __SYS_PLAYER_DATA

	if( !g_DbManager.GetPartyName() )
	{
		ASSERT( 0 );
	}
#ifndef __RT_1025	
	if( !g_DbManager.SetMessenger() )
	{
		ASSERT( 0 );
	}
#endif	// __RT_1025

	if( !g_DbManager.LoadPost() )
	{
		ASSERT( 0 );
	}
	SetTimer( hWnd, TID_POSTPROC, MIN( 1 ), NULL );

	if( !CEventGeneric::GetInstance()->LoadScript( "propEvent.inc" ) )
	{
//		ASSERT( 0 );
		AfxMessageBox( "propevent.inc" );
	}
	SetTimer( hWnd, TIMER_ID_EVENT_GENERIC, MIN( 1 ), NULL );

	if( !g_DbManager.OpenGuildCombat() )
	{
		ASSERT( 0 );
	}

#if __VER >= 12 // __LORD
	// 군주 시스템을 구성하는 협력 객체들을 생성한다
	CTLord::Instance()->CreateColleagues();
#endif	// __LORD
#if __VER >= 12 // __TAX
	CDbControllerTimer::GetInstance()->Create();
#endif	// __TAX
#ifdef __QUIZ
	// load QuizEvent Id
	if( !CQuizDBMng::GetInstance()->InitQuizEventId() )
	{
		ASSERT( 0 );
	}
	//CQuizDBMng::GetInstance()->PostRequest( QUERY_QUIZ_INIT );
#endif // __QUIZ

#if __VER >= 15 // __GUILD_HOUSE
	GuildHouseDBMng->PostRequest( GUILDHOUSE_LOAD );
#endif // __GUILD_HOUSE
#if __VER >= 15 // __CAMPUS
	CCampusHelper::GetInstance()->PostRequest( CAMPUS_LOAD );
#endif // __CAMPUS

	if( !g_dpLoginSrvr.StartServer( PN_DBSRVR_0 ) )
	{
		return FALSE;
	}
	
	if( !CDPTrans::GetInstance()->StartServer( PN_DBSRVR_1 ) )
	{
		return FALSE;
	}

	if( !g_dpCoreSrvr.StartServer( PN_DBSRVR_2 ) )
	{
		return FALSE;
	}

	if( FALSE == g_dpAccountClient.ConnectToServer( lpConnection, PN_ACCOUNTSRVR_1 ) )
		return FALSE;
	g_dpAccountClient.SendAddIdofServer( g_appInfo.dwSys );

	NotifyProcessStart( g_appInfo.dwId );	

	int x = 0, y = 0;
	SetWindowPos( hWnd, NULL, x, y, 400, 416, SWP_SHOWWINDOW );

	g_MyTrace.Initialize( hWnd, "SquireD", RGB( 0xff, 0xff, 0xff ), RGB( 0x00, 0x00, 0x00 ) );

	CTime time	= CTime::GetCurrentTime();
	g_MyTrace.Add( 0, FALSE, "%s", time.Format( "%Y/%m/%d %H:%M:%S" ) );
	g_MyTrace.AddLine( '\0' );

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

void ExitInstance( void )
{
	g_dpLoginSrvr.DeleteDPObject();
	CDPTrans::GetInstance()->DeleteDPObject();
	g_dpAccountClient.DeleteDPObject();
	g_dpCoreSrvr.DeleteDPObject();
	g_DbManager.Clear();
	g_GuildMng.Clear();
#ifndef __VM_0820
#ifndef __MEM_TRACE
	SAFE_DELETE( CGuildMember::sm_pPool );
	SAFE_DELETE( CGuild::sm_pPool );
	SAFE_DELETE( CMover::m_pPool );
#endif	// __MEM_TRACE
#endif	// __VM_0820

#if __VER >= 12 // __TAX
	CDbControllerTimer::GetInstance()->Destroy();
#endif	// __TAX
#if __VER >= 12 // __LORD
	// 군주 시스템 협력 객체들을 제거한다
	CTLord::Instance()->DestroyColleagues();
#endif	// __LORD

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
				case IDM_TEST:
					{
						CAr ar;
						int nNum = 100;
						ar << nNum;
						int nBufSize;
						LPBYTE lpBuf	= ar.GetBuffer( &nBufSize ); 
						CDPTrans::GetInstance()->OnSaveConcurrentUserNumber( ar, DPID_UNKNOWN, DPID_UNKNOWN, DPID_UNKNOWN, lpBuf, nBufSize );
						break;
					}
#if __VER >= 9 // __EVENTLUA
				case IDM_EVENTLUA_APPLYNOW:
					{
						prj.m_EventLua.m_Access.Enter();
						prj.m_EventLua.LoadScript();
						CDPTrans::GetInstance()->SendEventLuaChanged();
						if( prj.m_EventLua.CheckEventState() )
							CDPTrans::GetInstance()->SendEventLuaState( prj.m_EventLua.m_mapState, FALSE );
						prj.m_EventLua.m_Access.Leave();
					}
					break;
#endif // __EVENTLUA
#if __VER >= 14 // __PCBANG
				case IDM_PCBANG_ON:
					{
						HMENU hMenu = GetMenu( hWnd );
						CheckMenuItem( hMenu, IDM_PCBANG_ON, MF_CHECKED );
						CheckMenuItem( hMenu, IDM_PCBANG_OFF, MF_UNCHECKED );
						CDPTrans::GetInstance()->m_bPCBangApply = TRUE;
						CDPTrans::GetInstance()->SendPCBangSetApply( DPID_ALLPLAYERS );
					}
					break;
				case IDM_PCBANG_OFF:
					{
						HMENU hMenu = GetMenu( hWnd );
						CheckMenuItem( hMenu, IDM_PCBANG_OFF, MF_CHECKED );
						CheckMenuItem( hMenu, IDM_PCBANG_ON, MF_UNCHECKED );
						CDPTrans::GetInstance()->m_bPCBangApply = FALSE;
						CDPTrans::GetInstance()->SendPCBangSetApply( DPID_ALLPLAYERS );
					}
					break;
#endif // __PCBANG
#ifdef __QUIZ
				case IDM_QUIZEVENT_APPLYNOW:
					{
						if( !CQuiz::GetInstance()->IsRun() )
						{
							CQuiz::GetInstance()->LoadScript();
							CDPTrans::GetInstance()->SendQuizEventChanged();
						}
					}
					break;
#endif // __QUIZ
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_TIMER:
			{
				WORD wTimerID	= wParam;
				switch( wTimerID )
				{
/*
#ifdef __S0114_RELOADPRO
					case TID_RELOAD_PROJECT:
						g_dpAccountClient.ReloadProject();
						break;
#endif // __S0114_RELOADPRO
*/
					case TID_POSTPROC:
						CPost::GetInstance()->Process();
						break;
					case TID_QUERY_REMOVE_GUILD_BANK_TBL:
						g_DbManager.QueryRemoveGuildBankTbl();
						break;
					case TIMER_ID_EVENT_GENERIC:
						{
							if( CEventGeneric::GetInstance()->Run() )
							CDPTrans::GetInstance()->SendEventFlag( CEventGeneric::GetInstance()->GetFlag() ); 
						#if __VER >= 9 // __EVENTLUA
							prj.m_EventLua.m_Access.Enter();
						#ifdef __AUTO_NOTICE
							if( prj.m_EventLua.IsNoticeTime() )
								g_dpCoreSrvr.SendEventLuaNotice();
						#endif // __AUTO_NOTICE
							if( prj.m_EventLua.CheckEventState() )
								CDPTrans::GetInstance()->SendEventLuaState( prj.m_EventLua.m_mapState, TRUE );
							prj.m_EventLua.m_Access.Leave();
						#endif // __EVENTLUA

						#if __VER >= 11 // __GUILD_COMBAT_1TO1
							if( prj.m_GuildCombat1to1.CheckOpenTIme() == 1
								&& prj.m_GuildCombat1to1.m_nState == CGuildCombat1to1Mng::GC1TO1_CLOSE )
								CDPTrans::GetInstance()->SendGC1to1Open();
						#endif // __GUILD_COMBAT_1TO1

						#ifdef __GETMAILREALTIME
							LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
							lpDbOverlappedPlus->nQueryMode	= QM_GETMAIL_REALTIME;
							PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
							//g_DbManager.QueryGetMailRealTime();
						#endif // __GETMAILREALTIME
						}
						break;
						
					default:
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
/*
#ifdef __S0114_RELOADPRO
			KillTimer( hWnd, TID_RELOAD_PROJECT );
#endif // __S0114_RELOADPRO
*/
			KillTimer( hWnd, TID_POSTPROC );
			KillTimer( hWnd, TID_QUERY_REMOVE_GUILD_BANK_TBL );
			KillTimer( hWnd, TIMER_ID_EVENT_GENERIC );
			PostQuitMessage(0);
			break;
#if __VER >= 9 // __EVENTLUA
		case WM_RBUTTONDOWN:
			break;
#endif // __EVENTLUA
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}


BOOL Script( LPCSTR lpszFileName )
{
	CDbManager& db = CDbManager::GetInstance();
#ifdef __INTERNALSERVER
	strcpy( db.DB_ADMIN_PASS_LOG, "#^#^log" );	
	strcpy( db.DB_ADMIN_PASS_CHARACTER01, "#^#^character");	
	strcpy( db.DB_ADMIN_PASS_BACKSYSTEM, "!@!@backend" );	
	strcpy( db.DB_ADMIN_PASS_ITEMUPDATE, "#^#^item" ); 
#endif
	BOOL	bToolRemoveQuest	= FALSE;
	BOOL	bToolItemId		= FALSE;
	BOOL	bToolRestorePet	= FALSE;
	CScanner s;

	if( s.Load( lpszFileName ) )
	{
		s.GetToken();
		while( s.tok != FINISHED )
		{
			if( s.Token == "ResourceFolder" )
			{
				s.GetToken();
				if( !SetCurrentDirectory( s.Token ) )
				{
					g_MyTrace.Add( CMyTrace::Key( "status" ), FALSE, "Folder not found, %s", s.Token );
					return FALSE;
				}
			}
			else if( s.Token == "Sys" )
			{
				g_appInfo.dwSys		= s.GetNumber();
			}
			else if( s.Token ==  "dwId" )
			{
				g_appInfo.dwId		= s.GetNumber();
			}
			else if( s.Token == "Account" )
			{
				s.GetToken();
				strcpy( lpConnection, s.Token );
			}
			else if( s.Token == "DSN_NAME_CHARACTER" )
			{
				s.GetToken();
				strcpy( DSN_NAME_CHARACTER01, s.Token );
			}
			else if( s.Token == "DB_ADMIN_ID_CHARACTER" )
			{
				s.GetToken();
				strcpy( DB_ADMIN_ID_CHARACTER01, s.Token );
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
#ifdef __S1108_BACK_END_SYSTEM
			else if( s.Token == "DSN_NAME_BACKSYSTEM" )
			{
				s.GetToken();
				strcpy( DSN_NAME_BACKSYSTEM, s.Token );
			}
			else if( s.Token == "DB_ADMIN_ID_BACKSYSTEM" )
			{
				s.GetToken();
				strcpy( DB_ADMIN_ID_BACKSYSTEM, s.Token );
			}
			else if( s.Token == "BACKENDSYSTEM" )
			{
				prj.m_bBackEndSystem = TRUE;
			}
#endif // __S1108_BACK_END_SYSTEM
			else if( s.Token == "DSN_NAME_ITEMUPDATE" )
			{
				s.GetToken();
				strcpy( DSN_NAME_ITEMUPDATE, s.Token );
			}
			else if( s.Token == "DB_ADMIN_ID_ITEMUPDATE" )
			{
				s.GetToken();
				strcpy( DB_ADMIN_ID_ITEMUPDATE, s.Token );
			}
			else if( s.Token == "ITEMUPDATE" )
			{
				prj.m_bItemUpdate = TRUE;
			}
#ifdef __ITEM_REMOVE_LIST
			else if( s.Token == "ConvStartItem" )
			{
				prj.SetConvMode( CONVER_ITEM_START );
			}
			else if( s.Token == "SAVE_TEXT" )
			{
				prj.SetConvMode( SAVE_TEXT );
			}
			else if( s.Token == "REMOVE_ITEM_ALL" )
			{
				prj.SetConvMode( REMOVE_ITEM_ALL );
			}
			else if( s.Token == "REMOVE_ITEM_ALL_PAY" )
			{
				prj.SetConvMode( REMOVE_ITEM_ALL_PAY );
			}
			else if( s.Token == "PIERCING_CONFIRM" )
			{
				prj.SetConvMode( PIERCING_CONFIRM );
			}
#endif // __ITEM_REMOVE_LIST
			else if( s.Token == "BankToItemSendTbl" )
			{
				m_bBankToItemSendTbl	= TRUE;
			}
			else if( s.Token == "InventoryToItemSendTbl" )
			{
				m_bInventoryToItemSendTbl	= TRUE;
			}
			else if( s.Token == "__ITEM_REMOVE0203" )
			{
				s_bItemRemove	= TRUE;
			}
			else if( s.Token == "REMOVE_INVALID_ITEM" )
			{
				s_bRemoveInvalidItem	= TRUE;
			}
			else if( s.Token == "DB_PWD_LOG" )
			{
				s.GetToken();
				::GetPWDFromToken( s.Token, db.DB_ADMIN_PASS_LOG ); 
				TRACE("%s\n", db.DB_ADMIN_PASS_LOG ); 
			}
			else if( s.Token == "DB_PWD_CHARACTER" )
			{
				s.GetToken();
				::GetPWDFromToken( s.Token, db.DB_ADMIN_PASS_CHARACTER01 ); 
				TRACE("%s\n", db.DB_ADMIN_PASS_CHARACTER01 ); 
			}
			else if( s.Token == "DB_PWD_BACKEND" )
			{
				s.GetToken();
				::GetPWDFromToken( s.Token, db.DB_ADMIN_PASS_BACKSYSTEM ); 
				TRACE("%s\n", db.DB_ADMIN_PASS_BACKSYSTEM ); 
			}
			else if( s.Token == "DB_PWD_ITEMUPDATE" )
			{
				s.GetToken();
				::GetPWDFromToken( s.Token, db.DB_ADMIN_PASS_ITEMUPDATE ); 
				TRACE("%s\n", db.DB_ADMIN_PASS_ITEMUPDATE ); 
			}
			else if( s.Token == "ITEM_ID" )
			{
				bToolItemId	= TRUE;
				OutputDebugString( "ITEM_ID" );
			}
			else if( s.Token == "RESTORE_PET" )
			{
				bToolRestorePet		= TRUE;
				OutputDebugString( "RESTORE_PET" );
			}
			else if( s.Token == "REMOVE_QUEST" )
			{
				bToolRemoveQuest	= TRUE;
				OutputDebugString( "REMOVE_QUEST" );
			}
			s.GetToken();
		}


		if( !prj.OpenProject( "masquerade.prj" ) ) {
			g_MyTrace.Add( CMyTrace::Key( "status" ), FALSE, "Fail to open project" );
			return FALSE;
		}
		else {
			if( m_bBankToItemSendTbl )
			{
				g_DbManager.BankToItemSendTbl( "BankToItemSendTbl.txt" );
				return FALSE;
			}
			if( m_bInventoryToItemSendTbl )
			{
				g_DbManager.InventoryToItemSendTbl( "InventoryToItemSendTbl.txt" );
				return FALSE;
			}
			if( s_bItemRemove )
			{
				g_DbManager.ItemRemove0203( "ItemRemove.txt" );
				return FALSE;
			}
			if( s_bRemoveInvalidItem )
			{
				g_DbManager.RemoveInvalidItem();
				return FALSE;
			}
			if( bToolItemId )
			{
				g_DbManager.Conv( "conv.txt" );
				return FALSE;
			}
			if( bToolRestorePet )
			{
				g_DbManager.RestorePet( "restorepet.txt" );
				OutputDebugString( "end - restorepet.txt" );
				return FALSE;
			}
			if( bToolRemoveQuest )
			{
				g_DbManager.RemoveQuest();
				return FALSE;
			}

			g_MyTrace.Add( CMyTrace::Key( "status" ), FALSE, "Ready" );
			return TRUE;
		}
	}

	g_MyTrace.Add( CMyTrace::Key( "status" ), FALSE, "Fail to open file, %s", lpszFileName );
	return FALSE;
};

#ifdef __ITEM_REMOVE_LIST
BOOL LoadRemoveItem( LPCSTR lpszFileName )
{
	CScanner s;
	
	if( s.Load( lpszFileName ) )
	{
		s.GetToken();
		while( s.tok != FINISHED )
		{
			string strbuf = s.Token;
			g_DbManager.m_RemoveItem_List.insert( strbuf );
			s.GetToken();
		}
		return TRUE;
	}
	return FALSE;
}
#endif // __ITEM_REMOVE_LIST
