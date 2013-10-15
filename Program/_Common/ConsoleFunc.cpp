
#include "StdAfx.h"

#ifdef __BS_CONSOLE

#include "Console.h"
#include "defineObj.h"
#include "dpclient.h"
extern	CDPClient	g_DPlay;

//TEST!!!!!
#include "WndGuildHouse.h"
#include "WndHousing.h"
#include "ResData.h"
#include "WndGuildTabPower.h"
#include "AppDefine.h"
#include "defineText.h"
#include "dpCertified.h"

void _help_commend(  std::vector< string >& arg )
{ 
	gConsole()->AddString( "***************** Command list ************************" );
	gConsole()->ShowAllCommand( );	
	gConsole()->AddString( "***************** Command list end ********************" );
}

void _show_mypos( std::vector< string >& arg )
{
	if( !g_pPlayer )
	{
		gConsole()->AddString( "g_pPlayer is NULL" );
		return;
	}
			
	D3DXVECTOR3 kMyPos = g_pPlayer->GetPos( );
	char temp[ 64 ];
	sprintf( temp, "MYPOS:%f %f %f", kMyPos.x, kMyPos.y, kMyPos.z );
	gConsole()->AddString( temp );
}

void _show_fps( std::vector< string >& arg )
{
	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( 2002 );
	if( !pWndWorld )
		return;

	pWndWorld->m_bRenderFPS = !pWndWorld->m_bRenderFPS;

	gConsole()->Log( "=>now showing fps is %d", pWndWorld->m_bRenderFPS );
	
}

void _set_speed( std::vector< string >& arg )
{
	if( arg.size() < 2 )
		return;

	float fSpeedMul = 1.0f;
	const char* str = arg[ 1 ].c_str( );
	sscanf( str, "%f", &fSpeedMul );

	gConsole()->_fSpeedMul = fSpeedMul;

	gConsole()->Log( "=>now speed is ( org * %3.0f )", fSpeedMul );
}

void _show_mpu( std::vector< string >& arg )
{
	gConsole()->Log( "=>MPU : %d", g_MPU );

}

void _volum( std::vector< string >& arg )
{
	if( arg.size() < 2 )
		return;

	int iVol = 0;
	const char* str = arg[ 1 ].c_str( );
	sscanf( str, "%d", &iVol );
	float fVol = (float)iVol / 255.0f;
	SetVolume( fVol );
}

void _show_music_info( std::vector< string >& arg )
{
#if ( __USE_FMOD_MUSIC == ML_MILES )
	gConsole()->Log( "FMOD Music, Current Volum is : %f", GetVolume( ) );
#else
	gConsole()->Log( "Miles Music or ..., Current Volum is :%f", GetVolum( ) );
#endif
}

void _play_music( std::vector< string >& arg )
{
	if( arg.size() < 2 )
		return;

	int musicIndex = 0;
	sscanf( arg[ 1 ].c_str(), "%d", &musicIndex );
	PlayBGM( musicIndex );
}

void _set_weather( std::vector< string >& arg )
{
	if( arg.size() < 2 )
		return;

	int weather = 0;
	sscanf( arg[ 1 ].c_str( ), "%d", &weather );
	CWorld::m_skyBox.SetWeather( weather, TRUE );
}

void _change_model( std::vector< string >& arg )
{
	if( arg.size() < 2 )
		return;
	
	int modelIndex = 0;
	sscanf( arg[ 1 ].c_str(), "%d", &modelIndex );

	if( g_pPlayer ) 
	{
		g_pPlayer->SetIndex( D3DDEVICE , modelIndex );
	}
}

void _sfx_pos( std::vector< string >& arg )
{
	if( arg.size() < 2 )
		return;

	int index = 0;
	int nRepeat = 5;
	sscanf( arg[1].c_str(), "%d", &index );
	
	if( g_pPlayer )
	{
		D3DXVECTOR3 kPos = g_pPlayer->GetPos( );
		D3DXMATRIX kMat = g_pPlayer->GetMatrixRotation( );

		D3DXVECTOR3 kDir;
		kDir.x = kMat._31;
		kDir.y = kMat._32;
		kDir.z = kMat._33;

		kPos = kPos - kDir * 2.0f;

		CSfx *pSfx = NULL;
		for( int i = 0; i < nRepeat; ++i )
		{
			
// 			CMover* pPet =  prj.GetMover( g_pPlayer->GetEatPetId( ) );
// 			if( pPet )
// 			{
// 				index = pPet->m_dwMoverSfxId;
// 				pSfx = CreateSfx( D3DDEVICE, index, kPos );
// 				pSfx->SetScale( D3DXVECTOR3( 2.0f, 2.0f, 2.0f  ) );
// 			}
			
		}
	}
}

void _sfx_link( std::vector< string >& arg )
{
	if( arg.size() < 2 )
		return;

	int index = 0;
	sscanf( arg[1].c_str(), "%d", &index );

	if( g_pPlayer )
	{
		CSfx *pSfx = CreateSfx( D3DDEVICE, index, g_pPlayer->GetPos(), g_pPlayer->GetId() );  // 시전동작이기때문에 무조건 자기에게 나타난다.
		if( pSfx )
		{
			pSfx->SetPartLink( 0 );		// 오른손에 링크.
			pSfx->SetPartLink( 1 );
		}

	}
}

void _world_height( std::vector< string >& arg )
{
	if( arg.size() < 2 )
		return;

	float fnewMax = 0.0f;
	sscanf( arg[1].c_str(), "%f", &fnewMax );

	g_WorldMng.Get()->m_fMaxHeight = fnewMax;

	gConsole()->Log( "==>OK.. Now the world height is %f(m)", fnewMax );
}

void _test_func( std::vector< string >& arg )
{
	BOOL bAuthority = FALSE;
	CGuild* pGuild = g_pPlayer->GetGuild( );
	if( pGuild )
		bAuthority = pGuild->IsAuthority( g_pPlayer->m_idPlayer, PF_GUILDHOUSE_UPKEEP );

	if( !bAuthority )		//유지비 권한이 없으면 
	{
		g_WndMng.PutString( GETTEXT( TID_GAME_GUILDHOUSE_TAX_LEVEL ) );
		return;
	}

	if( !g_WndMng.m_pWndUpkeep )
	{
		g_WndMng.m_pWndUpkeep = new CWndGHUpkeep;
		g_WndMng.m_pWndUpkeep->Initialize(&g_WndMng, APP_CONFIRM_BUY_);
		
	}
}

void _random_moving( std::vector< string >& arg )
{
	g_Console._bRandomMoving = !g_Console._bRandomMoving;

	g_Console.Log( "now Random Moving is %d", g_Console._bRandomMoving );
}

void _ambient( std::vector< string >& arg )
{
	if( arg.size() < 2 )
		return;

	DWORD all = 0;
	sscanf( arg[1].c_str(), "%d", &all );
	
	if( g_pPlayer )
	{
		CWorld* pWorld = g_pPlayer->GetWorld( );
		if( pWorld )
		{
			pWorld->m_dwAmbient = 0;
			pWorld->m_dwAmbient |= (all << 16 ) ;
			pWorld->m_dwAmbient |= (all << 8 ) ;
			pWorld->m_dwAmbient |= all;
			
		}
	}
}

void _diffuse( std::vector< string >& arg )
{
	if( arg.size() < 2 )
		return;

	DWORD all = 0;
	sscanf( arg[1].c_str(), "%d", &all );
	
	if( g_pPlayer )
	{
		CWorld* pWorld = g_pPlayer->GetWorld( );
		if( pWorld )
		{
			pWorld->m_dwDiffuse = 0;
			pWorld->m_dwDiffuse |= (all << 16 ) ;
			pWorld->m_dwDiffuse |= (all << 8 ) ;
			pWorld->m_dwDiffuse |= all;
			
		}
	}
}

//void _change_reflect_texture1( std::vector< string >& arg )
//{
//	if( arg.size() < 2 )
//		return;
//
//	if( g_pReflectMap )
//		SAFE_RELEASE( g_pReflectMap );
//	
//	if( FAILED( LoadTextureFromRes( g_Neuz.m_pd3dDevice, MakePath( DIR_MODELTEX, arg[1].c_str() ), &g_pReflectMap ) ) )
//		AfxMessageBox( "error! cannot find reflect1 texture file" );
//
//	g_Console.Log( "changed reflection texture1" );
//}
//
//void _change_reflect_texture2( std::vector< string >& arg )
//{
//	if( arg.size() < 2 )
//		return;
//
//	if( g_pReflectMap2 )
//		SAFE_RELEASE( g_pReflectMap2 );
//	
//	if( FAILED( LoadTextureFromRes( g_Neuz.m_pd3dDevice, MakePath( DIR_MODELTEX, arg[1].c_str() ), &g_pReflectMap2 ) ) )
//		AfxMessageBox( "error! cannot find reflect1 texture file" );
//
//	g_Console.Log( "changed reflection texture2" );
//}

void _fog_start( std::vector< string >& arg )
{
	if( arg.size() < 2 )
		return;

	float fVal = 0.0f;
	sscanf( arg[1].c_str(), "%f", &fVal );

	if( g_pPlayer )
	{
		CWorld* pWorld = g_pPlayer->GetWorld( );
		if( pWorld )
		{
			pWorld->m_fFogStartValue = fVal;
		}
	}
}


// nomal func here
void RandomMoving( DWORD dt )
{
	static DWORD dwDelta = 0;
	dwDelta += dt;
	if( dwDelta > 5000 && g_pPlayer )
	{
		D3DXVECTOR3 kDir;
		float fAngle = float( rand() % 361 );
		float fSpeed = float( rand() % 30 );
		AngleToVectorXZ( &kDir, fAngle, fSpeed );

		D3DXVECTOR3 kPos = g_pPlayer->GetPos( );
		kPos += kDir;
		g_pPlayer->SetDestPos( kPos );

		static unsigned int index = 0;
		g_Console.Log( "(%d) : DestPos( %f, %f )", index, kPos.x, kPos.z );
		++index;

		if( index > 0xfffffff0 )
			index = 0;

		dwDelta = 0;
	}
}

extern	CDPCertified	g_dpCertified;
void _auto_login( )
{
	CScanner s;
	if( TRUE == s.Load( "autologin.ini" ) )
	{
		while( s.tok != FINISHED )
 		{
			s.GetToken();

			if( "Account" == s.Token )
			{
				s.GetToken();
				g_Console._strAccount = s.Token;
			}
			else if( "Password" == s.Token )
			{
				s.GetToken();
				g_Console._StrPassword = s.Token;
			}
			else if( "n2ndPassword" == s.Token )
			{
				g_Console._nPasswordReal = s.GetNumber();
			}
			else if( "nServer" == s.Token )
			{
				g_Console._nServer = s.GetNumber();
			}
			else if( "nCharacter" == s.Token )
			{
				g_Console._nCharacter = s.GetNumber();
			}
 		}
 	}
}

void _update_auto_login( const DWORD dwDelta )
{
	static BYTE bySEQ = 0;

	if( bySEQ > 2 )
		return;

	if( 0 == bySEQ )
	{
 		CWndLogin* pWndLogin	= (CWndLogin*)g_WndMng.GetWndBase( APP_LOGIN );
 		if( pWndLogin )
		{
#ifdef __CON_AUTO_LOGIN
			pWndLogin->SetAccountAndPassword( g_Console._strAccount, g_Console._StrPassword );
#endif
			pWndLogin->OnChildNotify( 0, WIDC_OK, 0 );

			bySEQ = 1;
			g_WndMng.PutString( "<WARNING!!!>자동로그인이 진행중입니다, 월드진입까지 키보드 만지지 마세요 ", NULL, 0xffff0000 );
		}
	}
	else if( 1 == bySEQ )
	{
		CWndSelectServer* pWndSelectServer = (CWndSelectServer*)g_WndMng.GetWndBase( APP_SELECT_SERVER );
		if( pWndSelectServer )
		{
			CWndListBox* pWndList	= (CWndListBox*)pWndSelectServer->GetDlgItem( WIDC_CONTROL0 );
			pWndList->SetCurSel( g_Console._nServer - 1 );

			pWndSelectServer->OnChildNotify( 0, WIDC_NEXT, 0 );
			bySEQ = 2;

			CString strTemp;
			strTemp.Format( "<WARNING!!!>자동로그인 SERVER INDEX: %d ", g_Console._nServer );
			g_WndMng.PutString( strTemp, NULL, 0xffff0000 );
		}
	}
	else if( 2 == bySEQ )
	{
		CWndSelectChar* pWnd	= (CWndSelectChar *)g_WndMng.GetWndBase( APP_SELECT_CHAR );
		if( pWnd )
		{
			pWnd->m_nSelectCharacter = g_Console._nCharacter - 1;

			pWnd->OnChildNotify( 0, WIDC_ACCEPT, 0 );
			bySEQ = 3;

			CString strTemp;
			strTemp.Format( "<WARNING!!!>자동로그인 CHARACTER INDEX: %d ", g_Console._nCharacter );
			g_WndMng.PutString( strTemp, NULL, 0xffff0000 );

			g_WndMng.PutString( "GOOD LUCK", NULL, 0xff00ffff );
		}
	}
	
}


void ConsoleRegister( )
{	
	// 여기서 명령 문자열 과 함수를 등록합니다.
	CONSOLE_REGISTER( string("help"), CON_DATA::VT_FUNCTION, _help_commend, string("도움말") );	
	CONSOLE_REGISTER( string("?"), CON_DATA::VT_FUNCTION, _help_commend, string("도움말") );
	CONSOLE_REGISTER( string("pos"), CON_DATA::VT_FUNCTION, _show_mypos, string("내좌표 출력") );
	CONSOLE_REGISTER( string("fps"), CON_DATA::VT_FUNCTION, _show_fps, string("toggle show fps") );	
	CONSOLE_REGISTER( string("speed"), CON_DATA::VT_FUNCTION, _set_speed, string("스피드 배율") );	
	CONSOLE_REGISTER( string("ㄴ"), CON_DATA::VT_FUNCTION, _set_speed, string("스피드 배율") );	
	CONSOLE_REGISTER( string("mpu"), CON_DATA::VT_FUNCTION, _show_mpu, string("print MPU") );	
	CONSOLE_REGISTER( string("vol"), CON_DATA::VT_FUNCTION, _volum, string("music volum ( 0 - 255 )") );	
	CONSOLE_REGISTER( string("music_info"), CON_DATA::VT_FUNCTION, _show_music_info, string("music info") );	
	CONSOLE_REGISTER( string("music"), CON_DATA::VT_FUNCTION, _play_music, string("play music (number)") );
	
	CONSOLE_REGISTER( string("weather"), CON_DATA::VT_FUNCTION, _set_weather, string("change weather( 2rain, 3snow)") );
	CONSOLE_REGISTER( string("change_model"), CON_DATA::VT_FUNCTION, _change_model, string( "모델바꿈" ) );
	CONSOLE_REGISTER( string("sfx_pos"), CON_DATA::VT_FUNCTION, _sfx_pos, string( "주인공 앞에 play sfx" ) );
	CONSOLE_REGISTER( string("sfx_link"), CON_DATA::VT_FUNCTION, _sfx_link, string( "쥬인공 link sfx" ) );
	CONSOLE_REGISTER( string("world_height"), CON_DATA::VT_FUNCTION, _world_height, string( "월드 높이 설정" ) );
	CONSOLE_REGISTER( string("wh"), CON_DATA::VT_FUNCTION, _world_height, string( "월드 높이 설정" ) );
	CONSOLE_REGISTER( string("test" ), CON_DATA::VT_FUNCTION, _test_func, string( "test" ) );
	CONSOLE_REGISTER( string("auto_login" ), CON_DATA::VT_FUNCTION, _auto_login, string( "auto login" ) );
	CONSOLE_REGISTER( string("random_moving"), CON_DATA::VT_FUNCTION, _random_moving, string("무작위 이동") );

	CONSOLE_REGISTER( string("ambient"), CON_DATA::VT_FUNCTION, _ambient, string("ambient value") );
	CONSOLE_REGISTER( string("diffuse"), CON_DATA::VT_FUNCTION, _diffuse, string("diffuse value") );

	//CONSOLE_REGISTER( string("crm1"), CON_DATA::VT_FUNCTION, _change_reflect_texture1, string("change reflect1") );
	//CONSOLE_REGISTER( string("crm2"), CON_DATA::VT_FUNCTION, _change_reflect_texture2, string("change reflect2") );
	
	CONSOLE_REGISTER( string("fog_start"), CON_DATA::VT_FUNCTION, _fog_start, string("fog start") );
}

void _auto_start( )
{
	
#ifdef _DEBUG
	g_Console.AddString( "★ Excute auto-command ★" );

	vector< string > arg;

//	arg.clear();
	arg.push_back( "speed" );
	arg.push_back( "2" );
	_set_speed( arg );

// 	arg.clear( );
// 	arg.push_back( "test" );
// 	_test_func( arg );

//	g_Console.Show( );
#endif

#ifdef __CON_AUTO_LOGIN
	_auto_login( );
#endif
}

int gnFail = 0;
int gnReq = 0;

int debugIndex = -1;

void _update_console( )
{

	if( GetAsyncKeyState( VK_CONTROL ) & 0x80000 )
	{
		if( g_bKeyTable[ 'T' ] )
		{
			//test mail
			CMover* pObj = (CMover*)g_WorldMng.Get()->GetObjFocus( );
			if( pObj )
			{
				char* lpszTitle = "letter";
				CString title;
				char* lpszText = "난 알아요 이밤이 흐르고 흐르면 모든것이 떠나야만 한다는 이사실을 그 이유를 나는 알수가 알수가 있어욤 taeji boys ye";
				  

				for( int i = 0; i < 90; ++i )
				{
					title.Format( "%d 번째 편지", i );
					g_DPlay.SendQueryPostMail( 1, 1, (char*)pObj->GetName(), 0,title.GetBuffer(0),lpszText );
				}
			}
			
			//g_Console._bTestToggle = !g_Console._bTestToggle;
			//g_Console.Log( "Toggle func - testing is %d", g_Console._bTestToggle );

			//if( !g_WndMng.m_pWndGHBid )
			//{
			//	g_WndMng.m_pWndGHBid = new CWndGuildHouseBid;
			//	g_WndMng.m_pWndGHBid->Initialize( );
		
			//}else
			//	SAFE_DELETE( g_WndMng.m_pWndGHBid );
		}
		else if( g_bKeyTable[ 'Y' ] )
		{
			CMailBox* pMailBox	= CMailBox::GetInstance();
			int nMax = pMailBox->size();

			CMailBox& mailbox = *pMailBox;
			
			for( int i = 0; i < nMax; ++i )
			{
				CMail* pMail = mailbox[i];
				if( pMail )
				{
					g_DPlay.SendQueryRemoveMail( pMail->m_nMail );						
				}
			}
		}
	}

 	static DWORD sDelta = 0;
 	static DWORD sOld = timeGetTime( );
// 
 	DWORD dwCur = timeGetTime( );
	DWORD dwDt = dwCur - sOld;
 	sDelta +=  dwDt;
 	sOld = dwCur;

#ifdef __CON_AUTO_LOGIN 
	_update_auto_login( sDelta );
#endif

	if( g_Console._bRandomMoving )
		RandomMoving( dwDt );

//	if( sDelta > 5000 )
//	{
// 		++debugIndex;
// 				
// 		while( 1 )
// 		{
// 			BOOL bOK = PlayMusic( debugIndex, 1 );
// 			if( bOK )
// 			{
// 				g_Console.Log( " Started music : %d ", debugIndex );
// 				break;
// 			}
// 								
// 			++debugIndex;
// 			if( debugIndex > 270 )
// 			{
// 				debugIndex = 0;
// 				break;
// 			}
// 		}

// 		CWndChat* pWndChat = (CWndChat*) g_WndMng.GetApplet( APP_COMMUNICATION_CHAT );		
// 		if( sDelta < 5100 && pWndChat )
// 		{
// 			int x = rand() % 10000;
// 			int z = rand() % 10000;
// 			CString str;
// 			str.Format( "%s %d %d %d", "/텔레 ", 1, x, z );
// 			pWndChat->m_wndEdit.AddString( str ); 
// 			pWndChat->OnChildNotify( EN_RETURN, WIDC_EDIT, 0 );
// 
// 			g_Console.Log( "TELEPORTED to %d, %d", x, z );
// 
// 			sDelta = 5100;
// 
// // 			for( int i = 0; i < 26; ++i )
// // 			{
// // 				g_bKeyTable[ 0x41 + i ] = rand() %2;
// // 			}
// // 
// // 			g_bKeyTable[ 0x4d ] = FALSE;	//m
// // 			g_bKeyTable[ 0x4c ] = FALSE;
// 		}
// 
// 		if( sDelta > 6000 ) 
// 		{
// //  			CString str;
// //  			str.Format( "%s %s %d %d", "/엔생", "라울프", 500, 0 );
// //  			pWndChat->m_wndEdit.AddString( str );
// //  			pWndChat->OnChildNotify( EN_RETURN, WIDC_EDIT, 0 );
// //  
// //  			g_Console.Log( "Created monster" );
//  			sDelta = 0;
// // 
//  			if( rand() %2 )
//  				g_WndMng.ClearAllWnd();
// 		}
// 			
// 	}
// 		
	
// 
//  	if( sDelta > 6000 )
//  	{
// 		++gnReq;
// 		gConsole()->Log( "===START ACTION===( FAIL %d : %d )", gnFail, gnReq );
// 		CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
// 		if( pWndWorld )
// 			pWndWorld->OnKeyUp( 'C', 0, 0 );
// 	//	CWndWorld::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
//  		sDelta = 0;
//  	}

//	if( pSfx->m_pSfxObj->Process( ) )
//	{
//		pSfx->m_pSfxObj->
//	}
}

#endif //__BS_CONSOLE