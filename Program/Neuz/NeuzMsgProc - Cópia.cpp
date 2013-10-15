#include "stdafx.h"
#include "resource.h"
#include "defineText.h"
#include "AppDefine.h"
#include "Webbox.h"
#include "dpmng.h"
#include "WorldMap.h"
#include "msghdr.h"

#ifdef __CSC_VER9_4
#include "OpenLoadWnd.h"
#endif //__CSC_VER9_4

#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
#include "xUtil.h"
#endif // __2ND_PASSWORD_SYSTEM

HANDLE	g_hMutex = NULL;
static char		g_szProtocolVersion[32];			// MSG_VER
static HHOOK	g_hHook;

extern	LPTSTR WEB_ADDRESS_DEFAULT;
extern	LPTSTR WEB_POSTDATA;
#ifdef __LINK_PORTAL
BOOL	g_bBuddyFlag;
#endif //__LINK_PORTAL

#ifdef __BS_CHECKLEAK
#include <crtdbg.h>
#define CRTDBG_MAP_ALLOC

#ifdef _DEBUG
#define new new( _NORMAL_BLOCK, __FILE__, __LINE__ )
#endif
#endif

#ifdef __CERTIFIER_COLLECTING_SYSTEM
#ifdef __CLIENT
void EncryptedValidCertifierIPSetup( void );
#endif // __CLIENT
#endif // __CERTIFIER_COLLECTING_SYSTEM

/*
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
*/

LPCTSTR GetProtocolVersion()
{
	return g_szProtocolVersion;
}

LRESULT CALLBACK LowLevelKeyboardProc (INT nCode, WPARAM wParam, LPARAM lParam)
{
    // By returning a non-zero value from the hook procedure, the
    // message does not get passed to the target window
    KBDLLHOOKSTRUCT *pkbhs = (KBDLLHOOKSTRUCT *) lParam;
    BOOL bControlKeyDown = 0;
	
    switch (nCode)
    {
	case HC_ACTION:
        {
            // Check to see if the CTRL key is pressed
            bControlKeyDown = GetAsyncKeyState (VK_CONTROL) >> ((sizeof(SHORT) * 8) - 1);
            
            // Disable CTRL+ESC
            if (pkbhs->vkCode == VK_ESCAPE && bControlKeyDown)
                return 1;
		#ifndef _DEBUG
			#ifndef __XALTTAB            
			// Disable ALT+TAB
			//if (pkbhs->vkCode == VK_TAB && pkbhs->flags & LLKHF_ALTDOWN)
				//return 1;
			#endif
		#endif	
            // Disable ALT+ESC
            if (pkbhs->vkCode == VK_ESCAPE && pkbhs->flags & LLKHF_ALTDOWN)
                return 1;
			if( pkbhs->vkCode == VK_LWIN || pkbhs->vkCode == VK_RWIN )
				return 1;
			
            break;
        }
		
	default:
		break;
    }
    return CallNextHookEx (g_hHook, nCode, wParam, lParam);
}

void InitLanguageFromResource( HINSTANCE hInstance )
{
	char szLang[16], szSubLang[16];

	LoadString(hInstance, IDS_LANG, szLang, 16);	
	LoadString(hInstance, IDS_SUBLANG, szSubLang, 16);	
	::SetLanguageInfo( atoi( szLang ), atoi( szSubLang ) );
#ifdef __LANG_1013
	char szCodePage[16]	= { 0,};
	LoadString( hInstance, IDS_CODEPAGE, szCodePage, 16 );
	g_codePage	= atoi( szCodePage );
#endif	// __LANG_1013
}

void InitGlobalVars( HINSTANCE hInstance )
{
	InitLanguageFromResource( hInstance );
	SetCodePage( ::GetLanguage() );

	char szEnryptPWD[4];
	char szFakeFullmode[4];
	char szCachePort[16];
	char szSecond[16];
	char szCount[16];
	char szUsing2ndPassword[4];

	LoadString(hInstance, IDS_SERVER_IP, g_Neuz.m_lpCertifierAddr, 16);
	LoadString(hInstance, IDS_ENCRYPTPWD, szEnryptPWD, 4);	
	LoadString(hInstance, IDS_FAKEFULLMODE, szFakeFullmode, 4);	
	LoadString(hInstance, IDS_MSGVER, g_szProtocolVersion, 32);
	LoadString(hInstance, IDS_CACHE_PORT, szCachePort, 16);
	LoadString(hInstance, IDS_SHOUTLIMIT_SECOND, szSecond, 16);
	LoadString(hInstance, IDS_SHOUTLIMIT_COUNT, szCount, 16);
	LoadString(hInstance, IDS_2ND_PASSWORD, szUsing2ndPassword, 4);

	g_Neuz.m_dwShoutLimitSecond = atoi( szSecond );
	g_Neuz.m_nShoutLimitCount = atoi( szCount );

	g_Neuz.m_uCachePort = atoi( szCachePort );
	g_Neuz.m_bFakeFullMode = ( atoi(szFakeFullmode) == 1 );
	g_Neuz.m_bEncryptPWD = ( atoi(szEnryptPWD) == 1 );
#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
	::SetUse2ndPassWord( atoi( szUsing2ndPassword ) == 1 );
#endif // __2ND_PASSWORD_SYSTEM
#ifdef __GPAUTH
	g_Neuz.m_bGPotatoAuth	= ::GetLanguage() == LANG_GER || ::GetLanguage() == LANG_FRE;
	if( g_Neuz.m_bGPotatoAuth )
		g_Neuz.m_bEncryptPWD	= FALSE;
#endif	// __GPAUTH
	if( g_Neuz.m_bFakeFullMode )
		g_Neuz.m_bStartFullscreen = FALSE;	

#if defined(__INTERNALSERVER ) || defined(__TESTSERVER)
	strcpy( g_szProtocolVersion, "20040706" );
#endif

	if( ::GetLanguage() == LANG_KOR )
	{
	#if defined(__TESTSERVER)
		strcpy( g_Neuz.m_lpCertifierAddr, "119.207.77.54" );
	#elif defined(__MAINSERVER)
		strcpy( g_Neuz.m_lpCertifierAddr, "119.207.77.44" );
	#endif
	}
	
	InitWebGlobalVar();					// WEB_ADDRESS_DEFAULT, WEB_POSTDATA설정 

	if( ::GetLanguage() == LANG_FRE )
	{
		g_Neuz.Key.chLeft	= 'Q';
		g_Neuz.Key.chUp	= 'Z';
		g_Neuz.Key.chTrace	= 'F';
		g_Neuz.Key.chQuest	= 'L';
		g_Neuz.Key.chWalk	= 'W';
	}
	else
	{
		g_Neuz.Key.chLeft	= 'A';
		g_Neuz.Key.chUp	= 'W';
		g_Neuz.Key.chTrace	= 'Z';
		g_Neuz.Key.chQuest	= 'Q';
		g_Neuz.Key.chWalk	= 'X';
	}
}

void DisableTaskSwitching( HINSTANCE hInstance )
{
#ifndef _DEBUG	
	OSVERSIONINFO versionInformation;
	versionInformation.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if( GetVersionEx( &versionInformation ) )
	{
		if( versionInformation.dwPlatformId == VER_PLATFORM_WIN32_NT )		// NT계열이면 키보드를 후킹하고
		{
			g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)::LowLevelKeyboardProc, hInstance, 0);
		} 
		else
		{
			// Disables task switching
			UINT nPreviousState;
			::SystemParametersInfo(SPI_SCREENSAVERRUNNING, 1, &nPreviousState, 0);		// 95, 98, Me면 API를 쓴다.
		}
	}
#endif	
}

void GetIPFromFile( TCHAR* szArg1, TCHAR* szArg2 )
{
#ifdef _DEBUG
	if( strcmpi( szArg1, "secom" ) == 0 )
	{	
		CScanner scanner;
		if( scanner.Load( "ServerIPList.txt" ) )
		{
			CStringArray strArray;
			scanner.GetToken();
			while( scanner.tok != FINISHED )
			{
				strArray.Add( scanner.token );
				scanner.GetToken();
			} 
			CString strIP = strArray.GetAt( atoi( szArg2 ) );
			_tcscpy( g_Neuz.m_lpCertifierAddr, strIP );
		}
	}
#endif
}

BOOL ParseCmdLine( LPCTSTR lpCmdLine )
{
	TCHAR szArg1[64] = {0, }; 
	TCHAR szArg2[64] = {0, }; 
	TCHAR szArg3[64] = {0, }; 
	TCHAR szArg4[64] = {0, }; 
#ifdef __TWN_LOGIN0816
	TCHAR szArg5[64] = {0, }; 
#endif	// __TWN_LOGIN0816

#ifdef __TWN_LOGIN0816
	_stscanf( lpCmdLine, "%s %s %s %s %s", szArg1, szArg2, szArg3, szArg4, szArg5 );
#else	// __TWN_LOGIN0816
	_stscanf( lpCmdLine, "%s %s %s %s", szArg1, szArg2, szArg3, szArg4 );	
#endif	// __TWN_LOGIN0816
/*
#if !defined(__INTERNALSERVER)
#if __CURRENT_LANG == LANG_KOR
#ifdef __LINK_PORTAL
	AfxMessageBox(lpCmdLine);

	if( strcmpi( szArg1, "localhost" ) )
	{
		AfxMessageBox(szArg1);
		AfxMessageBox(szArg2);

		CString strCmdLine;
		strCmdLine.Format("%s", szArg2);

		int nLength = strCmdLine.GetLength();
		if(nLength > 4)
		{
			CString strCmpWord = strCmdLine.Mid(nLength-4, nLength);

			if(strCmpWord == "__bu")
			{
				AfxMessageBox("Stop Shop!");
				//임시로 구매사이트 중단
				g_bBuddyFlag = FALSE;
			}
		}
	}
#else //__LINK_PORTAL
	if( strcmpi( szArg1, "reversion" ) )
		return FALSE;
#endif //__LINK_PORTAL
#else //LANG_KOR
	if( strcmpi( szArg1, "sunkist" ) )
		return FALSE;
#endif //LANG_KOR
#endif //__INTERNALSERVER
*/
#ifndef __MAINSERVER
	g_Neuz.m_bEncryptPWD	= TRUE;
#else	// __INTERNALSERVER
	if( strcmpi( szArg1, "sunkist" ) )
		return FALSE;
	if( ::GetLanguage() == LANG_KOR )
	{
		if( strcmpi( szArg2, "localhost" ) )
			g_Neuz.m_bEncryptPWD	= TRUE;
		else
			g_Neuz.m_bEncryptPWD	= FALSE;
	}
#endif	// __MAINSERVER

#ifdef __THROUGHPORTAL0810
	char	szPortal[LANG_MAX][3][100]	=
		{
			"localhost",	"",	"",		// LANG_KOR
			"",	"",	"",		// LANG_USA
			"hangame.co.jp",	"",	"",		// LANG_JAP
			"",	"",	"",		// LANG_CHI
			"flyffonline.ini3.co.th",	"",	"",		// LANG_THA
			"www.omg.com.tw/fff/", "", "",		// LANG_TWN
			"",	"",	"",		// LANG_GER
			"http://www.zoomby.net/",	"",	"",		// LANG_SPA
			"",	"",	"",		// LANG_FRE
			"",	"",	"",		// LANG_HK
			"",	"",	"",		// LANG_POR
			"",	"",	"",		// LANG_VTN
			"", "", "",		// LANG_RUS
		};
	if( lstrlen( szPortal[GetLanguage()][GetSubLanguage()] ) > 0 && lstrcmpi( szArg2, szPortal[GetLanguage()][GetSubLanguage()] ) == 0 )
	{
		g_Neuz.m_bThroughPortal		= TRUE;

		//	mulcom	BEGIN100323	일본 인증관련
#ifdef __JAPAN_AUTH
		if( ::GetLanguage() == LANG_JAP  && lstrcmpi( szArg2, "hangame.co.jp" ) != 0 )
		{
			g_Neuz.m_bEncryptPWD = FALSE;
		}
#endif // __JAPAN_AUTH
		//	mulcom	END100323	일본 인증관련

#ifdef __TWN_LOGIN0816
		g_Neuz.SetAccountInfo( szArg3, szArg4, szArg5 );
#else	// __TWN_LOGIN0816
		g_Neuz.SetAccountInfo( szArg3, szArg4 );
#endif	// __TWN_LOGIN0816

	}
//	if( GetLanguage() == LANG_TWN && !g_Neuz.m_bThroughPortal )
//		return 0;
#else	// __THROUGHPORTAL0810
	if( ::GetLanguage() == LANG_JAP && lstrcmpi( szArg2, "hangame.co.jp" ) == 0 )
	{
		g_Neuz.m_bHanGame = TRUE;
		g_Neuz.SetAccountInfo( szArg3, szArg4 ); // account, password
	}
#endif	// __THROUGHPORTAL0810
	
	GetIPFromFile( szArg1, szArg2 );

	return TRUE;
}

void EnableFloatException()
{
	int cw = _controlfp( 0, 0 );
	cw &= ~(EM_OVERFLOW|EM_UNDERFLOW|EM_ZERODIVIDE|EM_DENORMAL);
	_controlfp( cw, MCW_EM );
}

BOOL InitApp()
{
	// Get Os Version
	OSVERSIONINFO versionInfo;
	versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	BOOL bGetOsVr = GetVersionEx( &versionInfo );

	// Get VGA Identifier
	LPDIRECT3D9 pD3D;
	D3DADAPTER_IDENTIFIER9 Identifier; 
	pD3D = Direct3DCreate9( D3D_SDK_VERSION );
	HRESULT hres = pD3D->GetAdapterIdentifier( D3DADAPTER_DEFAULT, 0, &Identifier ); 
	
//	CString temp;
//	temp.Format("Os Ver : %d, VGA Vendor Id : %d", versionInfo.dwMajorVersion, Identifier.VendorId);
//	AfxMessageBox(temp, MB_OK);

	//ATI계열 카드의 Vista드라이버에서 Floating Point Exception이 발생하여 Vista & ATI가 아닐 경우만 Enable함
	if( bGetOsVr && hres == S_OK && versionInfo.dwMajorVersion != 6 && Identifier.VendorId != 4098 )
		EnableFloatException();

	SAFE_RELEASE(pD3D);

	// Set up the highest resolution timer we can manage
	TIMECAPS tc; 
	UINT uPeriod = (TIMERR_NOERROR == timeGetDevCaps(&tc, sizeof(tc))) ? tc.wPeriodMin : 1;  
	timeBeginPeriod( uPeriod ); 

	xSRand( timeGetTime() );	// 속도를 요하는 랜덤은 xRandom()을 쓰도록...
	srand( timeGetTime() );		// c내장 rand를 쓰고 싶다면 random()을 쓰도록...
 
	InitUPS();					// 애니메이션 타이머 초기화

	if( InitializeNetLib() == FALSE )
		return FALSE;

	LPCTSTR szCPU = GetCPUInfo();


	//////////////////////////////////////////////////////////////////////////
	//	BEGINTEST100111	FFL_DUMP
	//ffl_dump_unexception_filter::install( ffl_dump_level_heavy );
	//	ENDTEST100111	FFL_DUMP
	//////////////////////////////////////////////////////////////////////////

	#ifndef _DEBUG
	{
		InitEH();
		ADDERRORMSG_NOHEAD( szCPU );
	}
	#endif

	g_Neuz.LoadOption();		// Neuz.ini를 로딩 	
	if( lstrlen( g_Option.m_IPAddress ) > 2 )
		lstrcpy( g_Neuz.m_lpCertifierAddr, g_Option.m_IPAddress );

	TestNetLib( g_Neuz.m_lpCertifierAddr, PN_CERTIFIER );

//	g_Neuz.BeginLoadThread();	// prj.OpenProject()
	return TRUE;
}

void SkipPatch()
{
	g_hMutex = OpenMutex( MUTEX_ALL_ACCESS, TRUE, "neuz" );
	if( g_hMutex == NULL )
	{
		g_hMutex = CreateMutex( NULL, TRUE, "neuz" );
		if( g_hMutex )
			ReleaseMutex( g_hMutex );
	}
}

int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
#ifdef __LINK_PORTAL
	g_bBuddyFlag = TRUE;
#endif //__LINK_PORTAL

#ifdef _DEBUG
	afxTraceEnabled = TRUE;
#endif

#ifdef __BS_CHECKLEAK
	// 메모리 릭 탐지 및 alloc 라인 찾기 ( 릭이 발생하면 _CrtSetBreakAlloc( )에 인덱스를 넣어서 alloc을 시도한 라인에 브레이크가 걸림 )
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	//_CrtSetBreakAlloc( 912154 );
#endif 

	
#ifdef __CSC_VER9_4
#ifdef __CLIENT
	//Make Openning Load Window
	g_OpLoadWnd.Create(hInstance);
#endif //__CLIENT	
#endif //__CSC_VER9_4


	SkipPatch();
	InitGlobalVars( hInstance );
	DisableTaskSwitching( hInstance );	

	if( ParseCmdLine( lpCmdLine ) == FALSE )
		return 0;

	InitApp();

	if( lstrcmp( g_Neuz.m_lpCertifierAddr, "125.5.127.16" ) == 0 )
		WEB_ADDRESS_DEFAULT = "http://levelupgames.dyndns.org/GameWeb/Charge/Flyff/ItemShopMain.asp";

	if( lstrcmp( g_Neuz.m_lpCertifierAddr, "64.127.103.254" ) == 0 )
		WEB_ADDRESS_DEFAULT = "about:blank";

	// 미국 오픈 테스트 서버
	//	mulcom	BEGIN100422
	if( lstrcmp( g_Neuz.m_lpCertifierAddr, "204.2.134.200" ) == 0 )
		WEB_ADDRESS_DEFAULT = "http://test.billing.gpotato.com/Flyff/ItemShopGame/login.asp";
	//	mulcom	END100422

	// fr
	if( lstrcmp( g_Neuz.m_lpCertifierAddr, "84.203.243.116" ) == 0 )
		WEB_ADDRESS_DEFAULT	= "http://billing.gpotato.eu:8088/Charge/FlyffItemList.asp";

	// th
	if( lstrcmp( g_Neuz.m_lpCertifierAddr, "203.195.98.27" ) == 0 )
		WEB_ADDRESS_DEFAULT	= "http://billtest.ini3.co.th/Charge/Flyff/ItemShopMain.asp";

	//vtn
	if( lstrcmp( g_Neuz.m_lpCertifierAddr, "222.255.38.120" ) == 0 )
		WEB_ADDRESS_DEFAULT	= "http://testshop.flyff.vn/Default.aspx";

	//rus
	//	mulcom	BEGIN100209	러시아 테스트 서버 URL 변경
	if( lstrcmp( g_Neuz.m_lpCertifierAddr, "91.212.60.104" ) == 0 )
		WEB_ADDRESS_DEFAULT	= "http://test.flyffgame.ru/client/mall/";
	//	mulcom	END100209	러시아 테스트 서버 URL 변경

	//칠레 테섭
	//	mulcom	BEGIN100318	칠레 테스트 서버 아이템샵 URL 변경
	if( lstrcmp( g_Neuz.m_lpCertifierAddr, "204.2.134.13" ) == 0 )
		WEB_ADDRESS_DEFAULT	= "http://test.billing.es.gpotato.com/charge/FlyffItemList.asp";
	//	mulcom	END100318	칠레 테스트 서버 아이템샵 URL 변경


#ifdef __CERTIFIER_COLLECTING_SYSTEM
#ifdef __CLIENT
	EncryptedValidCertifierIPSetup();
#endif // __CLIENT
#endif // __CERTIFIER_COLLECTING_SYSTEM

	if( FAILED( g_Neuz.Create( hInstance ) ) )
		return 0;
	
#ifdef __CSC_VER9_4
#ifdef __CLIENT
	//Openning Wnd죽이고 로딩완료된 본 클라이언트를 보여주자.
	if(g_OpLoadWnd.GetSafeHwnd() != NULL)
		DestroyWindow(g_OpLoadWnd.GetSafeHwnd());	
	
	::ShowWindow(g_Neuz.GetSafeHwnd(), SW_SHOW);
#endif //__CLIENT	
#endif //__CSC_VER9_4

	return g_Neuz.Run();
}

#ifdef __CERTIFIER_COLLECTING_SYSTEM
#ifdef __CLIENT
void EncryptedValidCertifierIPSetup( void )
{
	// localhost (TwiLight)
	g_vecEncryptedValidCertifierIP.push_back( _T( "f528764d624db129b32c21fbca0cb8d6" ) ); // 127.0.0.1

	// kor
	g_vecEncryptedValidCertifierIP.push_back( _T( "87450e835d4f48e30ca7911550fc3def" ) ); // 119.207.77.44
	g_vecEncryptedValidCertifierIP.push_back( _T( "45cc88ee12277a8f93f1a4132c9a2fef" ) ); // 119.207.77.54

	// eng
	g_vecEncryptedValidCertifierIP.push_back( _T( "c1b955e6867668f0346ab6804963d84f" ) ); // 204.2.134.207
	g_vecEncryptedValidCertifierIP.push_back( _T( "b6b5098f4d14dd8b77bc515f031c057c" ) ); // 38.104.134.26
	g_vecEncryptedValidCertifierIP.push_back( _T( "e46174e308a0b7b066085fdb1026736b" ) ); // 204.2.134.200
	g_vecEncryptedValidCertifierIP.push_back( _T( "314b5f6f1d7be350a1e550a047539c24" ) ); // 38.99.100.190

	// php
	g_vecEncryptedValidCertifierIP.push_back( _T( "24ab6abba5a77773d30372aa97d8c277" ) ); // 125.5.127.17
	g_vecEncryptedValidCertifierIP.push_back( _T( "21604a1351d421e8dd4e081f3297ad93" ) ); // 125.5.127.16

	// jap
	g_vecEncryptedValidCertifierIP.push_back( _T( "771168ff1a3469936100e614b3819f4e" ) ); // 210.148.99.31
	g_vecEncryptedValidCertifierIP.push_back( _T( "eaa5c6b31311462387e165bdfa985285" ) ); // 210.148.99.30

	// tha
	g_vecEncryptedValidCertifierIP.push_back( _T( "25f1000c18048f445f12e171993d25c7" ) ); // 203.195.98.81
	g_vecEncryptedValidCertifierIP.push_back( _T( "cbb11ac1baddb7b87ccdadac67e8f0d9" ) ); // 203.195.98.27

	// twn
	g_vecEncryptedValidCertifierIP.push_back( _T( "cfb8faec43d8cdcb5904b01a0147628e" ) ); // 210.242.235.17
	g_vecEncryptedValidCertifierIP.push_back( _T( "4a7c98c8cccf47d77960e2e9637ab356" ) ); // 210.242.174.100
	g_vecEncryptedValidCertifierIP.push_back( _T( "4953d4f7cf6a2fec2ba60a5df0ac9586" ) ); // 60.199.180.13

	// ger
	g_vecEncryptedValidCertifierIP.push_back( _T( "a05132d7ccb5eb4f72aa550dfbc76bcc" ) ); // 195.59.138.10
	g_vecEncryptedValidCertifierIP.push_back( _T( "8a800a3ed741f0cb5bec5ef03004201f" ) ); // 84.203.140.29
	g_vecEncryptedValidCertifierIP.push_back( _T( "a00c29dcf8e07b9b763d26e8bdf26f11" ) ); // 84.203.140.30

	// chl
	g_vecEncryptedValidCertifierIP.push_back( _T( "507e2cdcaee1603ebc1482bde44743ef" ) ); // 204.2.134.20
	g_vecEncryptedValidCertifierIP.push_back( _T( "028abb4c46626f63172b8f2df376c032" ) ); // 10.10.65.19

	// fre
	g_vecEncryptedValidCertifierIP.push_back( _T( "34bb79d1c8fff2e8e002490b7c8ac13d" ) ); // 195.59.138.5
	g_vecEncryptedValidCertifierIP.push_back( _T( "7792c7d2e53778e43b537f60382d0fad" ) ); // 84.203.140.31
	g_vecEncryptedValidCertifierIP.push_back( _T( "af02b17d7d900de779e408eb85eb39e1" ) ); // 84.203.140.34

	// hk
	g_vecEncryptedValidCertifierIP.push_back( _T( "89a7c12c3908e0659289110128c668dd" ) ); // 203.174.49.238
	g_vecEncryptedValidCertifierIP.push_back( _T( "b43fe601c455abec2af4a97d50eae107" ) ); // 202.64.64.172

	// por
	g_vecEncryptedValidCertifierIP.push_back( _T( "143443684d7d1f847ed0344b881caa45" ) ); // 204.2.134.191
	g_vecEncryptedValidCertifierIP.push_back( _T( "6fc636213ddbeb419488c17e6f10e807" ) ); // 204.2.134.18

	// vtn
	g_vecEncryptedValidCertifierIP.push_back( _T( "525aab63bd814ea8ed175d1273a3d25c" ) ); // 203.89.140.12
	g_vecEncryptedValidCertifierIP.push_back( _T( "91ffed15f9329ad509d9045175984dc4" ) ); // 123.30.178.88

	// rus
	g_vecEncryptedValidCertifierIP.push_back( _T( "d9e0fcfeb2dfd78abf6de93b3919d776" ) ); // 88.212.221.67
	g_vecEncryptedValidCertifierIP.push_back( _T( "ad1329540cd9d50b9161a02868fc88bc" ) ); // 91.212.60.104

	// ind
	g_vecEncryptedValidCertifierIP.push_back( _T( "6ea9b07ff40a1b97ac7df5115cb91009" ) ); // 220.226.181.109
}
#endif // __CLIENT
#endif // __CERTIFIER_COLLECTING_SYSTEM