#include "stdafx.h"
#include <stdio.h>

#ifndef __MINIMIZE_USE_OPTION
	#include "scanner.h"
	#include "lang.h"
#endif

#include "HwOption.h"

#if __VER >= 9 // __CSC_VER9_RESOLUTION
#include "D3D9.h"
#pragma comment	(lib, "d3d9.lib")
#endif //__CSC_VER9_RESOLUTION


COption		g_Option;


#ifdef __MINIMIZE_USE_OPTION
	// chat style ( copy from \\_Interface\\WndManager.h )
	#define CHATSTY_GENERAL   0x0001
	#define CHATSTY_WHISPER   0x0002
	#define CHATSTY_SHOUT     0x0004
	#define CHATSTY_PARTY     0x0008
	#define CHATSTY_GUILD     0x0010

	#define CHATSTY_HELP          0x0060
	#define CHATSTY_HELP_CLIENT   0x0020
	#define CHATSTY_HELP_CHAT     0x0040
	#define CHATSTY_SYSTEM        0x0180
	#define CHATSTY_SYSTEM_CLIENT 0x0080
	#define CHATSTY_SYSTEM_CHAT   0x0100
	#define CHATSTY_GAME          0x0600
	#define CHATSTY_GAME_CLIENT   0x0200
	#define CHATSTY_GAME_CHAT     0x0400

	#define FINISHED	true
	#define Error		printf
	static const char g_seps[] = " ,\t";

	class CScannerEx
	{
	public:
		CScannerEx();
		~CScannerEx();

		BOOL	Load_FileIO( LPCTSTR szFilename );
		void	GetToken();
		void	GetTokenEx();
		int		GetNumber();
		float	GetFloat();

	public:
		CString	Token;
		bool	tok;

	private:
		char	m_szBuffer[256];
		char*	m_szToken;
		FILE*   m_pFILE;

	};

	CScannerEx::CScannerEx()
	{
		Token.Empty();
		tok = false;
		m_pFILE = NULL;
		m_szToken = NULL;
	}

	CScannerEx::~CScannerEx()
	{
		if( m_pFILE )
			fclose( m_pFILE );
	}

	BOOL CScannerEx::Load_FileIO( LPCTSTR szFilename )
	{
		FILE* fp = fopen( szFilename, "rt" );
		if( fp == NULL )
			return FALSE;
		else
		{
			m_pFILE = fp;
			return TRUE;
		}
	}
	void CScannerEx::GetToken()
	{
		if( fgets( m_szBuffer, sizeof( m_szBuffer ), m_pFILE ) )
		{
			m_szToken = strtok( m_szBuffer, g_seps );
			Token = m_szToken; 
		}
		else
		{
			tok = FINISHED;
			Token = "";
		}
	}

	void CScannerEx::GetTokenEx()
	{
		m_szToken = strtok( NULL, g_seps );
		if( m_szToken[0] == '"' )
		{
			Token = "";
			for( int i=1; m_szToken[i] != '"'; ++i )
				Token += m_szToken[i];
		}
		else
		{
			Token = m_szToken;
		}
	}
	
	int	 CScannerEx::GetNumber()
	{
		m_szToken = strtok( NULL, g_seps );
		if( m_szToken )
			return atoi( m_szToken );
		else
			return 0;
	}

	float CScannerEx::GetFloat()
	{
		m_szToken = strtok( NULL, g_seps );
		if( m_szToken )
			return static_cast< float >( atof( m_szToken ) );
		else
			return 0.0f;
	}
	int GetLanguage()
	{
		return __CURRENT_LANG;	// patchclient의 versioncommon.h에 정의됨 
	}
#else

	#define CScannerEx	CScanner
	#define GetTokenEx	GetToken

#endif

///////////////////////////////////////////////////////////////////
//  COption
///////////////////////////////////////////////////////////////////

COption::COption()
{
	Init();
}

COption::~COption()
{
}

void COption::Init()
{
	memset( m_szFileName, 0, sizeof(m_szFileName) );
	m_nWeatherEffect = TRUE;
	m_bOperator = FALSE;
	m_nResWidth = 1024;
	m_nResHeight = 768;
	m_bStartFullScreen = TRUE;		
	m_nTextureQuality = 1;
	m_nViewArea = 1;
	m_nObjectDistant = 1;
	m_nObjectDetail = 1;
	m_nShadow = 2;
	m_nWindowAlpha = 255; 
	m_nWindowEffect = 0;
	m_nSlangWord    = 0;
	m_nChatCommand  = 0;
	m_nInstantHelp  = 1;
	m_nInterface	= 1;	// 디폴트를 신버전으로 해놓자.
	m_bRollEffect = 0;
	m_bTip = 1;
	
	m_bSaveAccount = 0;
	m_szAccount[ 0 ] = 0;
	m_bNotice = TRUE;
	m_bTip = TRUE;
	m_tNoticeTime = 0;
	m_bTrade = TRUE;
	m_bSay = TRUE;
	m_bParty = TRUE;
	m_bShout = TRUE;
	m_bMessengerJoin = TRUE;
	m_bMessenger = TRUE;
#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
	m_bBattleBGM = FALSE;
#endif // __IMPROVE_SYSTEM_VER15
#ifdef __GAME_GRADE_SYSTEM
	m_bGameGradeRendering = FALSE;
#endif // __GAME_GRADE_SYSTEM
	m_nToggleScreen = 0;
	m_nSer = m_nMSer = 0;
	m_nBloom = 0;

	m_nMonName = 1;
	m_nToolTipTexture = 0;
	m_nToolTipText = 0;
	
	m_bVoice        = TRUE;
	m_fEffectVolume = 1.0f;
	m_fBGMVolume    = 0.5f;
	m_nGuide1        = 0;
	m_nGuide2        = 0;
	m_nGuide3        = 0;
#if __VER >= 12 // __MOD_TUTORIAL
	m_nTutorialLv = 0;
#endif	
#if __VER >= 12 // __UPDATE_OPT
	m_bViewMask = TRUE;
#endif
	m_bIsShowReSkillMessage = TRUE;
	m_pGuide = NULL;

	m_dwChatFilter[0] = CHATSTY_GENERAL | CHATSTY_WHISPER | CHATSTY_SHOUT | CHATSTY_PARTY | CHATSTY_GUILD;
	m_dwChatFilter[1] = CHATSTY_GENERAL | CHATSTY_WHISPER ;
	m_dwChatFilter[2] = CHATSTY_GENERAL | CHATSTY_SHOUT ;
	m_dwChatFilter[3] = CHATSTY_GENERAL | CHATSTY_PARTY ;
	m_dwChatFilter[4] = CHATSTY_GENERAL | CHATSTY_GUILD;
	m_dwChatFilterEtc = CHATSTY_HELP_CHAT | CHATSTY_SYSTEM_CHAT | CHATSTY_GAME_CLIENT; 

	m_IPAddress[0] = NULL;
#ifdef __SFX_OPT
	m_nSfxLevel = 5;	
#endif

#ifdef __YENV
	m_bExitChange = FALSE;
	m_bSpecBump = TRUE;
#endif //__YENV

	m_bDamageRender = FALSE;
	
	m_bVisibleBuffTimeRender = TRUE;
	
	m_MouseSpeed = 1;
	m_bSFXRenderOff = FALSE;	

	m_bCameraLock = TRUE;			// 디폴트로 켜져있게 내가 고쳤슴데이~ -xuzhu-
	m_bAutoAttack = TRUE;
	m_nPlayerName		= 1;		    // 자신 이름 On / Off
	m_nOtherPlayerName	= 1;		    // 다른 플레이어 이름 On / Off

#if __VER >= 8 //__Y_GAMMA_CONTROL_8
	m_nOverBright = 10;
	m_fGamma = 1.0f;
	m_fContrast = 1.0f;
#endif //__Y_GAMMA_CONTROL_8

#if __VER >= 8 //__CSC_VER8_3
	m_BuffStatusMode = 0;
#endif //__CSC_VER8_3
	
#if __VER >= 11 // __ADD_ZOOMOPT
	m_bZoomLimit = TRUE;
#endif

}

#if defined( _DEBUG) && !defined(__MINIMIZE_USE_OPTION)

int COption::LoadToolTip( LPCTSTR szFileName )
{
	_tcscpy( m_szFileName, szFileName);
	CScannerEx scan;
	if(scan.Load_FileIO( szFileName ) == FALSE )
		return 0;
	
	m_nToolTipText = 2;
	int nR = 0;
	int nG = 0;
	int nB = 0;

	do
	{
		nR = 0;
		nG = 0;
		nB = 0;
		scan.GetToken();
		if( scan.Token == _T( "아이템이름0" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwName0 = D3DCOLOR_ARGB( 255, nR, nG, nB );
		}
		else if( scan.Token == _T( "아이템이름1" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwName1 = D3DCOLOR_ARGB( 255, nR, nG, nB );
		}
		else if( scan.Token == _T( "아이템이름2" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwName2 = D3DCOLOR_ARGB( 255, nR, nG, nB );
		}
		else if( scan.Token == _T( "아이템이름3" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwName3 = D3DCOLOR_ARGB( 255, nR, nG, nB );
		}
		else if( scan.Token == _T( "아이템이름4" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwName4 = D3DCOLOR_ARGB( 255, nR, nG, nB );
		}
		else if( scan.Token == _T( "아이템이름5" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwName5 = D3DCOLOR_ARGB( 255, nR, nG, nB );
		}
		else if( scan.Token == _T( "일반" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwGeneral = D3DCOLOR_ARGB( 255, nR, nG, nB );		// 일반	
		}
		else if( scan.Token == _T( "플러스옵션" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwPlusOption = D3DCOLOR_ARGB( 255, nR, nG, nB );	// 플러스 옵션
		}
		else if( scan.Token == _T( "피어싱" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwPiercing = D3DCOLOR_ARGB( 255, nR, nG, nB );	// 피어싱
		}
		else if( scan.Token == _T( "속성일반" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwResist = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 속성 일반
		}
		else if( scan.Token == _T( "속성불" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwResistFire = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 속성 불
		}
		else if( scan.Token == _T( "속성물" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwResistWater = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 속성 물
		}
		else if( scan.Token == _T( "속성땅" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwResistEarth = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 속성 땅
		}
		else if( scan.Token == _T( "속성전기" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwResistElectricity = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 속성 전기
		}
		else if( scan.Token == _T( "속성바람" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwResistWind = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 속성 바람
		}
		else if( scan.Token == _T( "속성상용화무기" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwResistSM = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 속성 상용화 무기
		}
		else if( scan.Token == _T( "속성상용화방어구" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwResistSM1 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 속성 상용화 방어구
		}
		else if( scan.Token == _T( "시간" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwTime = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 시간 및 효과
		}
		else if( scan.Token == _T( "효능0" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwEffective0 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 아이템 효능
		}
		else if( scan.Token == _T( "효능1" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwEffective1 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 아이템 효능1
		}
		else if( scan.Token == _T( "효능2" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwEffective2 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 아이템 효능2
		}
		else if( scan.Token == _T( "효능3" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwEffective3 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 아이템 효능3
		}
		else if( scan.Token == _T( "랜덤옵션" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwRandomOption = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 랜덤 옵션
		}
		else if( scan.Token == _T( "인첸트옵션" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwEnchantOption = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 인첸트 옵션
		}
		else if( scan.Token == _T( "세트이름" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwSetName = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 세트 이름
		}
		else if( scan.Token == _T( "세트목록비착용" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwSetItem0 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 세트 목록(비착용시)
		}
		else if( scan.Token == _T( "세트목록착용" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwSetItem1 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 세트 목록(착용시)
		}
		else if( scan.Token == _T( "세트효과" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwSetEffect = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 세트 효과
		}
		else if( scan.Token == _T( "가격" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwGold = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 가격
		}
		else if( scan.Token == _T( "설명" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwCommand = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 설명
		}
		else if( scan.Token == _T( "사용못함" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwNotUse = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 사용못함	
		}
#if __VER >= 9 // __CSC_VER9_1
		else if( scan.Token == _T( "무기추가옵션1" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwAddedOpt1 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 무기추가옵션1				
		}
		else if( scan.Token == _T( "무기추가옵션2" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwAddedOpt2 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 무기추가옵션2
		}
		else if( scan.Token == _T( "무기추가옵션3" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwAddedOpt3 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 무기추가옵션3
		}
		else if( scan.Token == _T( "무기추가옵션4" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwAddedOpt4 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 무기추가옵션4
		}
		else if( scan.Token == _T( "무기추가옵션5" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwAddedOpt5 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 무기추가옵션5
		}
		else if( scan.Token == _T( "무기추가옵션6" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwAddedOpt6 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 무기추가옵션6
		}
		else if( scan.Token == _T( "무기추가옵션7" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwAddedOpt7 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 무기추가옵션7
		}
		else if( scan.Token == _T( "무기추가옵션8" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwAddedOpt8 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 무기추가옵션8
		}
		else if( scan.Token == _T( "무기추가옵션9" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwAddedOpt9 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 무기추가옵션9
		}
#endif //__CSC_VER9_1
#if __VER >= 11 // __SYS_IDENTIFY
		else if( scan.Token == _T( "아이템각성" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwAwakening	= D3DCOLOR_ARGB( 255, nR, nG, nB );
		}
		else if( scan.Token == _T( "여신의 축복" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwBlessing	= D3DCOLOR_ARGB( 255, nR, nG, nB );
		}
		else if( scan.Token == _T( "여신의 축복 경고" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwBlessingWarning	= D3DCOLOR_ARGB( 255, nR, nG, nB );
		}
#endif	// __SYS_IDENTIFY
	} while( scan.tok != FINISHED );

	return 1;
}
#endif // _DEBUG

#if __VER >= 9 // __CSC_VER9_RESOLUTION
void COption::CheckResolution()
{
	IDirect3D9* pD3D9;
	BOOL bResolution = FALSE;
	
	pD3D9 = Direct3DCreate9( D3D_SDK_VERSION );
	//UINT numAdapters = pD3D9->GetAdapterCount();
	
	D3DFORMAT allowedAdapterFormat = D3DFMT_X8R8G8B8;
	UINT numAdapterModes = pD3D9->GetAdapterModeCount( 0, allowedAdapterFormat );
	for (UINT mode = 0; mode < numAdapterModes; mode++)
	{
		D3DDISPLAYMODE displayMode;
		pD3D9->EnumAdapterModes( 0, allowedAdapterFormat, mode, &displayMode );
		if( displayMode.Width == m_nResWidth && displayMode.Height == m_nResHeight )
			bResolution = TRUE;
	}
	
	if(!bResolution && m_bStartFullScreen) //미지원 해상도의 경우 800x600으로 고정
	{
		Error( "We Can't Support this Resolution!! - Width : %d, Height : %d", m_nResWidth, m_nResHeight );
		m_nResWidth = 800;
		m_nResHeight = 600;
	}
	else //그래픽카드가 지원하는 해상도 중 사용가능한 해상도 선별
	{
		switch(m_nResWidth)
		{
		case 800: //800x600
			if(m_nResHeight != 600)
			{
				Error( "We Can't Support this Resolution!! - Width : %d, Height : %d", m_nResWidth, m_nResHeight );
				m_nResHeight = 600;
			}
			break;
		case 1024: //1024x768
			if(m_nResHeight != 768)
			{
				Error( "We Can't Support this Resolution!! - Width : %d, Height : %d", m_nResWidth, m_nResHeight );
				m_nResHeight = 768;
			}
			break;
		case 1280: //1280x1024 or 1280x720 (WIDE) or 1280x768 (WIDE) or 1280x800 (WIDE)
			if(m_nResHeight != 1024 && m_nResHeight != 720 && m_nResHeight != 768 && m_nResHeight != 800)
			{
				Error( "We Can't Support this Resolution!! - Width : %d, Height : %d", m_nResWidth, m_nResHeight );
				m_nResWidth = 800;
				m_nResHeight = 600;				
			}
			break;
		case 1600: //1600x1200
			if(m_nResHeight != 1200)
			{
				Error( "We Can't Support this Resolution!! - Width : %d, Height : %d", m_nResWidth, m_nResHeight );
				m_nResHeight = 1200;
			}
			break;
		case 1360: //1360x768 (WIDE)
			if(m_nResHeight != 768)
			{
				Error( "We Can't Support this Resolution!! - Width : %d, Height : %d", m_nResWidth, m_nResHeight );
				m_nResHeight = 768;
			}
			break;
		case 1400: //1400x1050
			if(m_nResHeight != 1050)
			{
				Error( "We Can't Support this Resolution!! - Width : %d, Height : %d", m_nResWidth, m_nResHeight );
				m_nResHeight = 1050;
			}
			break;
		case 1440: //1440x900 (WIDE)
			if(m_nResHeight != 900)
			{
				Error( "We Can't Support this Resolution!! - Width : %d, Height : %d", m_nResWidth, m_nResHeight );
				m_nResHeight = 900;
			}
			break;
		case 1680: //1680x1050 (WIDE)
			if(m_nResHeight != 1050)
			{
				Error( "We Can't Support this Resolution!! - Width : %d, Height : %d", m_nResWidth, m_nResHeight );
				m_nResHeight = 1050;
			}
			break;
		default:
			Error( "We Can't Support this Resolution!! - Width : %d, Height : %d", m_nResWidth, m_nResHeight );
			m_nResWidth = 800;
			m_nResHeight = 600;
			break;
		}
	}
	//SAFE_RELEASE( pD3D9 );
	pD3D9->Release();
}
#endif//__CSC_VER9_RESOLUTION

int COption::Load( LPCTSTR szFileName )
{
	_tcscpy( m_szFileName, szFileName);

	CScannerEx scan;
	if(scan.Load_FileIO( szFileName ) == FALSE )
		return 0;

	do {
		scan.GetToken();
		if( scan.Token == _T( "resolution" ) )
		{
			m_nResWidth = scan.GetNumber();
			m_nResHeight = scan.GetNumber();

#if __VER < 9 // __CSC_VER9_RESOLUTION
			if( m_nResWidth != 800 && m_nResWidth != 1024 && m_nResWidth != 1280 )
			{
				Error( "지원하지않는 해상도 발견!! - Width : %d", m_nResWidth );
				m_nResWidth = 800;
			}

			if( m_nResHeight != 600 && m_nResHeight != 768 && m_nResHeight != 1024 )
			{
				Error( "지원하지않는 해상도 발견!! - Height : %d", m_nResHeight );
				m_nResHeight = 600;
			}
#endif //__CSC_VER9_RESOLUTION	
		}
		else
		if( scan.Token == _T( "fullscreen" ) )
			m_bStartFullScreen = scan.GetNumber();
		else
		if( scan.Token == _T( "texture" ) )
			m_nTextureQuality = scan.GetNumber();
		else
		if( scan.Token == _T( "view" ) )
		{
			m_nViewArea = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "distant" ) )
		{
			m_nObjectDistant = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "shadow" ) )
		{
			m_nShadow = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "bloom" ) )
		{
			m_nBloom = scan.GetNumber();
			m_nBloom	= 0;	// 2008/04/25
		}
		else
		if( scan.Token == _T( "detail" ) )
			m_nObjectDetail = scan.GetNumber();
		else
		if( scan.Token == _T( "hotkey" ) ) 
		{
		}
		else
		if( scan.Token == _T( "WindowAlpha" ) ) 
		{
			m_nWindowAlpha = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "SlangWord" ) ) 
		{
			m_nSlangWord = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "ChatCommand" ) ) 
		{
			m_nChatCommand = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "InstantHelp" ) ) 
		{
			m_nInstantHelp = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "Interface" ) ) 
		{
			m_nInterface = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "RollEffect" ) ) 
		{
			m_bRollEffect = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "account" ) ) 
		{
			scan.GetTokenEx();
			strcpy( m_szAccount, scan.Token );
		}
		else
		if( scan.Token == _T( "saveAccount" ) ) 
		{
			m_bSaveAccount = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "notice" ) ) 
		{
			m_bNotice = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "noticeTime" ) ) 
		{
			m_tNoticeTime = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "tip" ) ) 
		{
			m_bTip = scan.GetNumber();
		}				
		else
		if( scan.Token == _T( "Trade") )
		{
			m_bTrade = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "Say") )
		{
			m_bSay = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "Party") )
		{
			m_bParty = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "sunkist" ) )
		{
			m_bOperator = TRUE;
			m_nToggleScreen = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "Shout" ) )
		{
			m_bShout = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "MessengerJoin" ) )
		{
			m_bMessengerJoin = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "Messenger") )
		{
			m_bMessenger = scan.GetNumber();
		}
		else
#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
		if( scan.Token == _T( "BattleBGM") )
		{
			m_bBattleBGM = scan.GetNumber();
		}
		else
#endif // __IMPROVE_SYSTEM_VER15
#ifdef __GAME_GRADE_SYSTEM
		if( scan.Token == _T( "GameGrade") )
		{
			m_bGameGradeRendering = scan.GetNumber();
		}
		else
#endif // __GAME_GRADE_SYSTEM
		if( scan.Token == _T( "Server" ) )
		{
			m_nSer = scan.GetNumber();
			m_nMSer = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "Sound" ) )
		{
			m_bVoice        = scan.GetNumber();
			m_fEffectVolume = scan.GetFloat();
			m_fBGMVolume    = scan.GetFloat();
		}
		else
		if( scan.Token == _T( "Guide" ) )
		{
			m_nGuide1        = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "Guide2" ) )
		{
			m_nGuide2        = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "Guide3" ) )
		{
			m_nGuide3        = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "SkillShowMessage" ) )
		{
			m_bIsShowReSkillMessage = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "ChatFilter0" ) )
		{
			m_dwChatFilter[0] = scan.GetNumber();
			m_dwChatFilter[0] &= ~( CHATSTY_HELP | CHATSTY_SYSTEM | CHATSTY_GAME );
		}
		else
		if( scan.Token == _T( "ChatFilter1" ) )
		{
			m_dwChatFilter[1] = scan.GetNumber();
			m_dwChatFilter[1] &= ~( CHATSTY_HELP | CHATSTY_SYSTEM | CHATSTY_GAME );
		}
		else
		if( scan.Token == _T( "ChatFilter2" ) )
		{
			m_dwChatFilter[2] = scan.GetNumber();
			m_dwChatFilter[2] &= ~( CHATSTY_HELP | CHATSTY_SYSTEM | CHATSTY_GAME );
		}
		else
		if( scan.Token == _T( "ChatFilter3" ) )
		{
			m_dwChatFilter[3] = scan.GetNumber();
			m_dwChatFilter[3] &= ~( CHATSTY_HELP | CHATSTY_SYSTEM | CHATSTY_GAME );
		}
		else
		if( scan.Token == _T( "ChatFilter4" ) )
		{
			m_dwChatFilter[4] = scan.GetNumber();
			m_dwChatFilter[4] &= ~( CHATSTY_HELP | CHATSTY_SYSTEM | CHATSTY_GAME );
		}
		else
		if( scan.Token == _T( "ChatFilterEtc" ) )
		{
#if __VER >= 8 //__Y_CHAT_SYSTEM_8
			m_dwChatFilterEtc = CHATSTY_HELP_CHAT | CHATSTY_SYSTEM_CHAT | CHATSTY_GAME_CLIENT;
#else //__Y_CHAT_SYSTEM_8
			m_dwChatFilterEtc = scan.GetNumber();
#endif //__Y_CHAT_SYSTEM_8
		}
		/*else
        if( scan.Token == _T( "ip" ) )
        {
            scan.GetTokenEx();
            strcpy( m_IPAddress, scan.Token );
        } */
#ifdef __YENV
		else
		if( scan.Token == _T( "SpecBumpEffect" ) )
		{
			m_bSpecBump = scan.GetNumber();
		}
#endif //__YENV
		else
		if( scan.Token == _T( "DamageRender" ) )
		{
			m_bDamageRender = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "BuffTimeRender" ) )
		{
			m_bVisibleBuffTimeRender = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "MouseSpeed" ) )
		{
			m_MouseSpeed = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "SFXRenderOff" ) )
		{
			m_bSFXRenderOff = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "CameraLock" ) )
		{
			m_bCameraLock = scan.GetNumber();
		}	
		else
		if( scan.Token == _T( "AutoAttack" ) )
		{
			m_bAutoAttack = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "PRenderName" ) )
		{
			m_nPlayerName = scan.GetNumber();
		}	
		else
		if( scan.Token == _T( "OPRenderName" ) )
		{
			m_nOtherPlayerName = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "NRenderName" ) )
		{
			m_nMonName = scan.GetNumber();
		}
#if __VER >= 8 //__Y_GAMMA_CONTROL_8
		else
		if( scan.Token == _T( "BRIGHT" ) )
		{
			m_nOverBright = scan.GetNumber();
		}	
		else
		if( scan.Token == _T( "GAMMA" ) )
		{
			m_fGamma = scan.GetFloat();
		}
		else
		if( scan.Token == _T( "CONTRAST" ) )
		{
			m_fContrast = scan.GetFloat();
		}
#endif //__Y_GAMMA_CONTROL_8

#if __VER >= 8 //__CSC_VER8_3
		else if(scan.Token == _T( "BUFFSTATUSMODE" ))
		{
			m_BuffStatusMode = scan.GetNumber();
		}
#endif //__CSC_VER8_3
		
#if __VER >= 11 // __ADD_ZOOMOPT
	else if(scan.Token == _T( "ZOOMLIMIT" ))
	{
		m_bZoomLimit = scan.GetNumber();
	}
#endif
#ifdef __SFX_OPT
	else if(scan.Token == _T( "SFXLEVEL" ))
	{
		m_nSfxLevel = scan.GetNumber();
	}
#endif
	else if(scan.Token == _T( "WEATHEREFFECT" ))
	{
		m_nWeatherEffect = scan.GetNumber();
	}
#if __VER >= 12 // __UPDATE_OPT
	else if(scan.Token == _T( "VIEWMASK" ))
	{
		m_bViewMask = scan.GetNumber();
	}
#endif




	} while( scan.tok != FINISHED );

#if __VER >= 9 // __CSC_VER9_RESOLUTION
	CheckResolution();
#endif //__CSC_VER9_RESOLUTION

#if __VER >= 12 // __UPDATE_OPT
	m_nChatCommand				= 0;
	m_nSlangWord				= 0;
	m_nInterface				= 1;
	m_bRollEffect				= FALSE;
	m_MouseSpeed				= 1;
	m_bVisibleBuffTimeRender	= TRUE;
	m_bShout					= TRUE;
	m_bMessengerJoin   			= TRUE;
#endif

	return 1;
}

int	COption::Save( LPCTSTR szFileName )
{
	FILE *fp = fopen( szFileName, _T( "wt" ) );
	if(fp == NULL) 
		return 0;

	_ftprintf(fp, _T( "// neuz browser ini file\n\n" ) );
	_ftprintf(fp, _T( "// option\n" ) ,0);
	_ftprintf(fp, _T( "resolution %d %d\n" ) ,m_nResWidth, m_nResHeight );
	_ftprintf(fp, _T( "fullscreen %d\n" ) ,m_bStartFullScreen );
	_ftprintf(fp, _T( "texture %d\n" ) ,m_nTextureQuality );
	_ftprintf(fp, _T( "view %d\n" ) ,m_nViewArea );
	_ftprintf(fp, _T( "detail %d\n" ) ,m_nObjectDetail );
	_ftprintf(fp, _T( "distant %d\n" ) ,m_nObjectDistant );
	_ftprintf(fp, _T( "shadow %d\n" ) ,m_nShadow );
	_ftprintf(fp, _T( "bloom %d\n" ) ,m_nBloom );
	_ftprintf(fp, _T( "WindowAlpha %d\n" ) ,m_nWindowAlpha );
	_ftprintf(fp, _T( "SlangWord %d\n" )   ,m_nSlangWord );
	_ftprintf(fp, _T( "ChatCommand %d\n" ) ,m_nChatCommand );
	_ftprintf(fp, _T( "InstantHelp %d\n" ) ,m_nInstantHelp );
	_ftprintf(fp, _T( "Interface %d\n" ) ,m_nInterface );
	_ftprintf(fp, _T( "RollEffect %d\n" ) ,m_bRollEffect );
	_ftprintf(fp, _T( "saveAccount %d\n" ), m_bSaveAccount );

	if( m_bSaveAccount )
		_ftprintf(fp, _T( "account \"%s\"\n" ), m_szAccount );
	else
		_ftprintf(fp, _T( "account \"%s\"\n" ), "" );

	_ftprintf(fp, _T( "notice %d\n" ), m_bNotice );
	_ftprintf(fp, _T( "noticeTime %d\n" ), m_tNoticeTime );
	_ftprintf(fp, _T( "tip %d\n" ), m_bTip );
	_ftprintf(fp, _T( "Trade %d\n" ), m_bTrade );
	_ftprintf(fp, _T( "Say %d\n" ), m_bSay );
	_ftprintf(fp, _T( "Party %d\n" ), m_bParty );
	_ftprintf(fp, _T( "Shout %d\n" ), m_bShout );
	_ftprintf(fp, _T( "MessengerJoin %d\n" ), m_bMessengerJoin );
	_ftprintf(fp, _T( "Messenger %d\n" ), m_bMessenger );
#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
	_ftprintf(fp, _T( "BattleBGM %d\n" ), m_bBattleBGM );
#endif // __IMPROVE_SYSTEM_VER15
#ifdef __GAME_GRADE_SYSTEM
	_ftprintf(fp, _T( "GameGrade %d\n" ), m_bGameGradeRendering );
#endif // __GAME_GRADE_SYSTEM
	_ftprintf(fp, _T( "Server %d %d\n" ), m_nSer, m_nMSer );

	if( m_nToggleScreen )
		_ftprintf(fp, _T( "sunkist %d\n" ), m_nToggleScreen );

	_ftprintf(fp, _T( "Sound %d %.1f %.1f\n" ), m_bVoice, m_fEffectVolume, m_fBGMVolume );
	_ftprintf(fp, _T( "Guide %d\n" ), m_nGuide1 );
	_ftprintf(fp, _T( "Guide2 %d\n" ), m_nGuide2 );
	_ftprintf(fp, _T( "Guide3 %d\n" ), m_nGuide3 );
	_ftprintf(fp, _T( "SkillShowMessage %d\n" ), m_bIsShowReSkillMessage );
	_ftprintf(fp, _T( "ChatFilter0 %d\n" ), m_dwChatFilter[0] );
	_ftprintf(fp, _T( "ChatFilter1 %d\n" ), m_dwChatFilter[1] );
	_ftprintf(fp, _T( "ChatFilter2 %d\n" ), m_dwChatFilter[2] );
	_ftprintf(fp, _T( "ChatFilter3 %d\n" ), m_dwChatFilter[3] );
	_ftprintf(fp, _T( "ChatFilter4 %d\n" ), m_dwChatFilter[4] );
	_ftprintf(fp, _T( "ChatFilterEtc %d\n" ), m_dwChatFilterEtc );

#ifdef __YENV
	if( m_bExitChange )
		_ftprintf(fp, _T( "SpecBumpEffect %d\n" ), !m_bSpecBump );
	else
		_ftprintf(fp, _T( "SpecBumpEffect %d\n" ), m_bSpecBump );	
#endif //__YENV

	_ftprintf(fp, _T( "ip \"%s\"\n" ), m_IPAddress);
	_ftprintf(fp, _T( "DamageRender %d\n" ), m_bDamageRender );
	_ftprintf(fp, _T( "BuffTimeRender %d\n" ), m_bVisibleBuffTimeRender );
	_ftprintf(fp, _T( "MouseSpeed %d\n" ), m_MouseSpeed );
	_ftprintf(fp, _T( "SFXRenderOff %d\n" ), m_bSFXRenderOff );
	_ftprintf(fp, _T( "CameraLock %d\n" ), m_bCameraLock );
	_ftprintf(fp, _T( "AutoAttack %d\n" ), m_bAutoAttack );
	_ftprintf(fp, _T( "PRenderName %d\n" ), m_nPlayerName );
	_ftprintf(fp, _T( "OPRenderName %d\n" ), m_nOtherPlayerName );
	_ftprintf(fp, _T( "NRenderName %d\n" ), m_nMonName );

#if __VER >= 8 //__Y_GAMMA_CONTROL_8
	_ftprintf(fp, _T( "BRIGHT %d\n" ),   m_nOverBright );
	_ftprintf(fp, _T( "GAMMA %f\n" ),    m_fGamma );
	_ftprintf(fp, _T( "CONTRAST %f\n" ), m_fContrast );
#endif //__Y_GAMMA_CONTROL_8
			
#if __VER >= 8 //__CSC_VER8_3
	_ftprintf(fp, _T( "BUFFSTATUSMODE %d\n" ), m_BuffStatusMode );
#endif //__CSC_VER8_3
#if __VER >= 11 // __ADD_ZOOMOPT
	_ftprintf(fp, _T( "ZOOMLIMIT %d\n" ), m_bZoomLimit );
#endif
#ifdef __SFX_OPT
	_ftprintf(fp, _T( "SFXLEVEL %d\n" ), m_nSfxLevel );
#endif
	_ftprintf(fp, _T( "WEATHEREFFECT %d\n" ), m_nWeatherEffect );
#if __VER >= 12 // __UPDATE_OPT
	_ftprintf(fp, _T( "VIEWMASK %d\n" ), m_bViewMask );
#endif


	fclose(fp);
	return 1;
}











#ifdef __OPTION_EX
int COption::Defalt( LPCTSTR szFileName )
{
	CResFile file; 
	_tcscpy( m_szFileName, szFileName);

	  CScannerEx scan;

	if(file.Open( szFileName, "rb" ) == FALSE )
			Error( "%s failed to load", szFileName );
	   return 0;

	if(scan.Load_FileIO( szFileName ) == FALSE )
	   return 0;

	do {
		scan.GetToken();

		if( scan.Token == _T( "view" ) )
		{
			m_nViewArea = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "distant" ) )
		{
			m_nObjectDistant = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "detail" ) )
			m_nObjectDetail = scan.GetNumber();
		else
		if( scan.Token == _T( "shadow" ) )
		{
			m_nShadow = scan.GetNumber();
		}
		else
#ifdef __SFX_OPT
	    if(scan.Token == _T( "SFXLEVEL" ))
	    {
			m_nSfxLevel = scan.GetNumber();
	    }
#endif
		if( scan.Token == _T( "SFXRenderOff" ) )
		{
			m_bSFXRenderOff = scan.GetNumber();
		}
	    else if(scan.Token == _T( "ZOOMLIMIT" ))
	    {
			m_bZoomLimit = scan.GetNumber();
	    }
		if( scan.Token == _T( "DamageRender" ) )
		{
			m_bDamageRender = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "PRenderName" ) )
		{
			m_nPlayerName = scan.GetNumber();
		}	
		else
		if( scan.Token == _T( "OPRenderName" ) )
		{
			m_nOtherPlayerName = scan.GetNumber();
		}
	    else 
		if(scan.Token == _T( "WEATHEREFFECT" ))
	    {
			m_nWeatherEffect = scan.GetNumber();
	    }
	    else
		if(scan.Token == _T( "VIEWMASK" ))
	    {
			m_bViewMask = scan.GetNumber();
	    }
		if( scan.Token == _T( "CameraLock" ) )
		{
			m_bCameraLock = scan.GetNumber();
		}	
		else
		if( scan.Token == _T( "Trade") )
		{
			m_bTrade = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "Say") )
		{
			m_bSay = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "Messenger") )
		{
			m_bMessenger = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "Party") )
		{
			m_bParty = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "AutoAttack" ) )
		{
			m_bAutoAttack = scan.GetNumber();
		}
		else

		if( scan.Token == _T( "MessengerJoin" ) )
		{
			m_bMessengerJoin = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "InstantHelp" ) ) 
		{
			m_nInstantHelp = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "Guide" ) )
		{
			m_nGuide1        = scan.GetNumber();
		}
		else
#ifdef __GAME_GRADE_SYSTEM
		if( scan.Token == _T( "GameGrade") )
		{
			m_bGameGradeRendering = scan.GetNumber();
		}
		else
#endif // __GAME_GRADE_SYSTEM
		if( scan.Token == _T( "WindowAlpha" ) ) 
		{
			m_nWindowAlpha = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "Sound" ) )
		{
			m_bVoice        = scan.GetNumber();
			m_fEffectVolume = scan.GetFloat();
			m_fBGMVolume    = scan.GetFloat();
		}
		else
		if( scan.Token == _T( "BattleBGM") )
		{
			m_bBattleBGM = scan.GetNumber();
		}

	} while( scan.tok != FINISHED );

	CheckResolution();


	m_nChatCommand				= 0;
	m_nSlangWord				= 0;
	m_nInterface				= 1;
	m_bRollEffect				= FALSE;
	m_MouseSpeed				= 1;
	m_bVisibleBuffTimeRender	= TRUE;
	m_bShout					= TRUE;
	m_bMessengerJoin   			= TRUE;

	return 1;
}

#endif//__OPTION_EX








