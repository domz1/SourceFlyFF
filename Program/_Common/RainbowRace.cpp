#include "stdafx.h"

#if __VER >= 13 // __RAINBOW_RACE

#include "RainbowRace.h"
#ifdef __CLIENT
#include "DPClient.h"
extern CDPClient g_DPlay;
#endif // __CLIENT

#ifdef __WORLDSERVER
#include "MiniGameGawibawibo.h"
#include "MiniGameDiceplay.h"
#include "MiniGameArithmetic.h"
#include "MiniGameStopwatch.h"
#include "MiniGameTyping.h"
#include "MiniGamePairgame.h"
#include "MiniGameLadder.h"

#include "defineObj.h"
#include "defineText.h"
#include "DPCoreClient.h"
extern CDPCoreClient g_DPCoreClient;
#include "worldmng.h"
extern CWorldMng g_WorldMng;
#include "User.h"
extern CUserMng g_UserMng;
#include "DPSrvr.h"
extern CDPSrvr g_DPSrvr;
#include "DPDatabaseClient.h"
extern CDPDatabaseClient g_dpDBClient;
#endif // __WORLDSERVER


//////////////////////////////////////////////////////////////////
// CRainbowRace //////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
CRainbowRace::CRainbowRace()
: m_wGameState(0x0000), m_wNowGame(0x0000)
{
#ifdef __CLIENT
	m_dwRemainTime = 0;
#endif // __CLIENT
#ifdef __WORLDSERVER
	m_bFinished = FALSE;
	m_pMiniGame = NULL;
	SetNextMiniGame();
#endif // __WORLDSERVER
}
CRainbowRace::~CRainbowRace()
{ 
	DestroyGame();	
}

void CRainbowRace::DestroyGame()
{
#ifdef __WORLDSERVER
	SAFE_DELETE( m_pMiniGame );
#endif // __WORLDSERVER
}


void CRainbowRace::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_wNowGame;
		ar << m_wGameState;
#ifdef __WORLDSERVER
		if( IsFinished() || (CRainbowRaceMng::GetInstance()->GetState() == CRainbowRaceMng::RR_CLOSED) )
			ar << (DWORD)0;
		else
			ar << CRainbowRaceMng::GetInstance()->GetNextTime() - GetTickCount();
#endif // __WORLDSERVER
	}
	else
	{
		ar >> m_wNowGame;
		ar >> m_wGameState;
#ifdef __CLIENT
		ar >> m_dwRemainTime;
		if( m_dwRemainTime > 0 )
			m_dwRemainTime += GetTickCount();
#endif // __CLIENT
	}
}

BOOL CRainbowRace::IsCompletedGame( int nGameNum )
{
#ifdef __WORLDSERVER
	if( nGameNum > CRainbowRaceMng::GetInstance()->GetMiniGameSize() )
		return FALSE;
#endif // __WORLDSERVER

	return ( m_wGameState & ( 0x0001 << nGameNum ) );
}

#ifdef __CLIENT
CRainbowRace* CRainbowRace::GetInstance()
{
	static CRainbowRace sRainbowRace;
	return & sRainbowRace;
}
int	CRainbowRace::GetGameKey()
{
	for( int i=0; i<RMG_MAX; i++ )
	{
		if( ( 0x0001 << i ) & m_wNowGame )
			return i;
	}

	return -1;
}

void CRainbowRace::SendMinigamePacket( int nGameKey, int nState, int nParam1, int nParam2 )
{
	__MINIGAME_PACKET MiniGamePacket( m_wNowGame, nState, nParam1, nParam2 );
	if( nGameKey != RMG_MAX )
		MiniGamePacket.wNowGame = GetGameKeyToNowGame( nGameKey );
    g_DPlay.SendRainbowRaceMiniGameState( MiniGamePacket );
}

void CRainbowRace::SendMinigameExtPacket( vector<string> & vecszData, int nGameKey, int nState, int nParam1, int nParam2 )
{
	__MINIGAME_EXT_PACKET MiniGameExtPacket( m_wNowGame, nState, nParam1, nParam2 );
	MiniGameExtPacket.vecszData.assign( vecszData.begin(), vecszData.end() );
	
	if( nGameKey != RMG_MAX )
		MiniGameExtPacket.wNowGame = GetGameKeyToNowGame( nGameKey );
	g_DPlay.SendRainbowRaceMiniGameExtState( MiniGameExtPacket );
}
#endif // __CLIENT

#ifdef __WORLDSERVER
void CRainbowRace::SetNextMiniGame()
{
	DestroyGame();
	if( IsAllCompleted() )
		return;
	
	int nGame = xRandom( CRainbowRaceMng::GetInstance()->GetMiniGameSize() );
	while( IsCompletedGame( nGame ) )
	{
		nGame--;
		if( nGame < 0 )
			nGame = CRainbowRaceMng::GetInstance()->GetMiniGameSize() - 1;
	}

	SetMiniGame( nGame );
}

void CRainbowRace::SetMiniGame( int nGameNum )
{
	m_wNowGame = 0x0001 << nGameNum;
	CMiniGameBase* pMiniGame = CRainbowRaceMng::GetInstance()->GetMiniGame( nGameNum );
	switch( nGameNum )
	{
		case RMG_GAWIBAWIBO:	m_pMiniGame = new CMiniGameGawibawibo( pMiniGame ); break;
		case RMG_DICEPLAY:		m_pMiniGame = new CMiniGameDiceplay( pMiniGame );	break;
		case RMG_ARITHMATIC:	m_pMiniGame = new CMiniGameArithmetic( pMiniGame );		break;
		case RMG_STOPWATCH:		m_pMiniGame = new CMiniGameStopwatch( pMiniGame );		break;
		case RMG_TYPING:		m_pMiniGame = new CMiniGameTyping( pMiniGame );		break;
		case RMG_PAIRGAME:		m_pMiniGame = new CMiniGamePairgame( pMiniGame );		break;
		case RMG_LADDER:		m_pMiniGame = new CMiniGameLadder( pMiniGame );		break;
	}
}

void CRainbowRace::OnMiniGamePacket( CUser* pUser, __MINIGAME_PACKET* pMiniGamePacket )
{
	if( m_wNowGame != pMiniGamePacket->wNowGame )	// 클라로 부터 받은 패킷에 지정된 미니게임이 서버와 다르면..
	{
		pUser->AddDefinedText( TID_GAME_RAINBOWRACE_ISNOTNOWMINIGAME );
		return;
	}

	if( m_pMiniGame && m_pMiniGame->Excute( pUser, pMiniGamePacket ) )	// 이번 미니게임이 완료되었으면...
	{
		SetNowGameComplete( pUser );	// 이번 게임을 완료상태로 만들고 다음 게임을 셋팅한다.
	}
}

void CRainbowRace::SetNowGameComplete( CUser* pUser )
{
	m_wGameState |= m_wNowGame;
	m_wNowGame = 0x0000;

	if( IsAllCompleted() )
		DestroyGame();
	else
		SetNextMiniGame();			// 다음 게임 세팅

	pUser->AddRainbowRaceState( this );	// 클라에 알려준다.
	CRainbowRaceMng::GetInstance()->SetMiniGamePrizeList( pUser->m_idPlayer, GetCompletedGameNum(), GetTickCount() );	// 미니게임 상품지급
}

int	CRainbowRace::GetCompletedGameNum()
{
	int nCount = 0;
	for( int i=0; i<CRainbowRaceMng::GetInstance()->GetMiniGameSize(); i++ )
	{
		if( IsCompletedGame( i ) )
			nCount++;
	}

	return nCount;
}
#endif // __WORLDSERVER

//////////////////////////////////////////////////////////////////
// CRainbowRaceMng ///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
#ifdef __WORLDSERVER
CRainbowRaceMng::CRainbowRaceMng(void)
{
	m_vecMiniGame.resize( RMG_MAX, NULL );
	m_vecMiniGame[RMG_GAWIBAWIBO]	= new CMiniGameGawibawibo;
	m_vecMiniGame[RMG_DICEPLAY]		= new CMiniGameDiceplay;
	m_vecMiniGame[RMG_ARITHMATIC]	= new CMiniGameArithmetic;
	m_vecMiniGame[RMG_STOPWATCH]	= new CMiniGameStopwatch;
	m_vecMiniGame[RMG_TYPING]		= new CMiniGameTyping;
	m_vecMiniGame[RMG_PAIRGAME]		= new CMiniGamePairgame;
	m_vecMiniGame[RMG_LADDER]		= new CMiniGameLadder;

	Init();
}

CRainbowRaceMng::~CRainbowRaceMng(void)
{
	Clear();

	for( DWORD i=0; i<m_vecMiniGame.size(); i++ )
		SAFE_DELETE( m_vecMiniGame[i] );
	
	m_vecMiniGame.clear();
}

BOOL CRainbowRaceMng::Init()
{
	Clear();

	return TRUE;
}

void CRainbowRaceMng::Clear()
{
	SetNextTime(0);
	SetState( RR_CLOSED );
	
	for( MAPRR::iterator it=m_mapRainbowRace.begin(); it!=m_mapRainbowRace.end(); it++ )
	{
		CRainbowRace* pRainbowRace = it->second;
		pRainbowRace->m_wNowGame = 0x0000;
		pRainbowRace->m_wGameState = 0x0000;
		CUser* pUser = static_cast<CUser*>( prj.GetUserByID( it->first ) );
		if( IsValidObj( pUser ) && !pRainbowRace->IsFinished() )
			pUser->AddRainbowRaceState( pRainbowRace );
		SAFE_DELETE( it->second );
	}
	m_mapRainbowRace.clear();
	m_vecdwRankingId.clear();
	m_vecMiniGamePrizeList.clear();
}

CRainbowRaceMng* CRainbowRaceMng::GetInstance()
{
	static CRainbowRaceMng sRainbowRace;
	return & sRainbowRace;
}

BOOL CRainbowRaceMng::LoadScript()
{
	if( m_Lua.RunScript( "RainbowRace.lua" ) != 0 )
	{
		Error( "RainbowRace.lua Load Failed!!" );	
		exit(0);
	}
	m_Lua.Pop( 0 );

	return TRUE;
}

void CRainbowRaceMng::Process()
{
	DWORD dwTick = GetTickCount();

	switch( GetState() )		// 현재 State에서 다음 State로 넘어갈때 처리, m_nState는 현재 State
	{
		case RR_CLOSED:
		{
			if( IsApplicationTime() )
			{
				g_DPCoreClient.SendCaption( GETTEXT( TID_GAME_RAINBOWRACE_CAPTION_APPLICATION ) );
				SetState( RR_APPLICATION );
				break;
			}
			
			if( IsOpenTime() )
			{
				SetState( RR_OPEN );
				break;
			}
			
			break;
		}

		case RR_APPLICATION:
		{
			if( !IsApplicationTime() )
			{
				//g_DPCoreClient.SendCaption( "레인보우 신청 시간이 끝났어!!" );	
				
				SetNextTime( 0 );
				SetState( RR_CLOSED );
			}
			break;
		}

		case RR_OPEN:
		{
			if( GetApplicantNum() > 0 )
			{
				SetNextTime( dwTick + static_cast<DWORD>( m_Lua.GetGlobalNumber( "Wait_Time" ) ) );
				g_DPCoreClient.SendCaption( GETTEXT( TID_GAME_RAINBOWRACE_CAPTION_WAIT ) );
				SetState( RR_WAIT );
			}
			else
			{
				SetNextTime( 0 );
				SetState( RR_CLOSED );
			}
			break;
		}

		case RR_WAIT:
		{
			if( GetNextTime() < dwTick )
			{
				SetNextTime( dwTick + static_cast<DWORD>( m_Lua.GetGlobalNumber( "Ready_Time" ) ) );
				g_DPCoreClient.SendCaption( GETTEXT( TID_GAME_RAINBOWRACE_CAPTION_READY01 ) );
				g_DPCoreClient.SendCaption( GETTEXT( TID_GAME_RAINBOWRACE_CAPTION_READY02 ) );
				SetState( RR_READY );
			}
			break;
		}

		case RR_READY:
		{
			if( GetNextTime() < dwTick )
			{
				SetNPC();	// 진행 NPC를 생성
				g_DPCoreClient.SendCaption( GETTEXT( TID_GAME_RAINBOWRACE_CAPTION_START ) );
				SetNextTime( dwTick + static_cast<DWORD>( m_Lua.GetGlobalNumber( "Progress_Time" ) ) );
				SetState( RR_PROGRESS );
				BeginRainbowRace();
			}
			break;
		}

		case RR_PROGRESS:
		{
			if( GetNextTime() < dwTick )
			{
				RemoveNPC();
				SetMiniGamePrize( 0 );	// 미니게임 상품지급 예약 된거 먼저 지급
				g_DPCoreClient.SendCaption( GETTEXT( TID_GAME_RAINBOWRACE_CAPTION_FINISH01 ) );
				if( m_vecdwRankingId.size() > 0 ) // 우승자 출력
				{
					char szFinish[256] = {0,};
					sprintf( szFinish, GETTEXT( TID_GAME_RAINBOWRACE_CAPTION_FINISH02 ), 
							CPlayerDataCenter::GetInstance()->GetPlayerString( m_vecdwRankingId[0] ) ); // 우승자 이름
					g_DPCoreClient.SendCaption( szFinish );
				}
				SetPrize();
				g_dpDBClient.SendRainbowRaceRanking( NULL_ID, 0 ); // 레인보우 레이스 종료 - 완주자 랭킹 및 패배자들을 DB에 저장한다.
				SetPrevRanking( m_vecdwRankingId );
				Clear();
			}
			else
				SetMiniGamePrize( dwTick );
			
			break;
		}
	}
}

BOOL CRainbowRaceMng::IsApplicationTime()
{
	BOOL bIsAppTime = FALSE;
	if( m_Lua.GetLuaFunction( "IsApplicationTime" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );
		bIsAppTime = static_cast<BOOL>( m_Lua.ToBool( -1 ) );
	}

	m_Lua.Pop( 0 );

	return bIsAppTime;
}

BOOL CRainbowRaceMng::IsOpenTime()
{
	BOOL bIsOpenTime = FALSE;
	if( m_Lua.GetLuaFunction( "IsOpenTime" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );
		bIsOpenTime = static_cast<BOOL>( m_Lua.ToBool( -1 ) );
	}

	m_Lua.Pop( 0 );

	return bIsOpenTime;
}

CRainbowRace* CRainbowRaceMng::GetRainbowRacerPtr( DWORD dwPlayerId )
{
	MAPRR::iterator it = m_mapRainbowRace.find( dwPlayerId );
	if( it == m_mapRainbowRace.end() )
		return NULL;

	return it->second;
}

BOOL CRainbowRaceMng::IsEntry( DWORD dwPlayerId )
{
	if( GetState() == RR_PROGRESS )
	{
		CRainbowRace* pRainbowRace = GetRainbowRacerPtr( dwPlayerId );
		if( pRainbowRace && !pRainbowRace->IsFinished() )
			return TRUE;
	}
	
	return FALSE;
}

void CRainbowRaceMng::SetApplicationUser( CUser* pUser )
{
	if( !g_eLocal.GetState( EVE_RAINBOWRACE ) )
		return;

	if( GetState() != RR_APPLICATION )
	{
		// GM은 CLOSE 상태에서도 신청 가능함(테스트 용)
		if( pUser->IsAuthHigher( AUTH_GAMEMASTER ) && GetState() == RR_CLOSED )
			pUser->AddText( "GM Application..." );
		else
		{
			pUser->AddDefinedText( TID_GAME_RAINBOWRACE_ISNOTAPPLICATIONTIME );
			return;
		}
	}

	int nPayPenya;
	if( m_Lua.GetLuaFunction( "GetPayPenya" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );
		nPayPenya = (int)( m_Lua.ToNumber( -1 ) );
	}
	m_Lua.Pop( 0 );

	if( pUser->GetGold() < nPayPenya )
	{
		pUser->AddDefinedText( TID_GAME_LACKMONEY );
		return;
	}

	if( IsApplicant( pUser->m_idPlayer ) )	// 이미 신청한 놈이면...
	{
		pUser->AddDefinedText( TID_GAME_RAINBOWRACE_ISAPPLICATIONED );
		return;
	}
	else	// 처음 신청하는 놈이면...
	{
		SetApplication( pUser->m_idPlayer );
		pUser->AddGold( -nPayPenya );
		pUser->AddDefinedText( TID_GAME_RAINBOWRACE_APPLICATION_COMPLETE );
		g_DPSrvr.PutPenyaLog( pUser, "a", "RAINBOW_RACE_APP", nPayPenya );

		g_dpDBClient.SendRainbowRaceApplication( pUser->m_idPlayer );
	}
}

void CRainbowRaceMng::SetApplication( DWORD dwPlayerId )
{
	if( !g_eLocal.GetState( EVE_RAINBOWRACE ) )
		return;
	
	m_mapRainbowRace.insert( make_pair( dwPlayerId, new CRainbowRace ) );
}

BOOL CRainbowRaceMng::SetDropOut( DWORD dwPlayerId )
{
	// 현재 참가자이고 완주하지 않은 놈은 탈락!!
	if( GetState() != RR_PROGRESS )
		return FALSE;
	
	MAPRR::iterator it = m_mapRainbowRace.find( dwPlayerId );
	if( it != m_mapRainbowRace.end() && !it->second->IsFinished() )
	{
		g_dpDBClient.SendRainbowRaceFailedUser( it->first );
		SAFE_DELETE( it->second );
		m_mapRainbowRace.erase( it );
		return TRUE;
	}

	return FALSE;
}

void CRainbowRaceMng::BeginRainbowRace()
{
	CUser* pUser = NULL;
	for( MAPRR::iterator it=m_mapRainbowRace.begin(); it!=m_mapRainbowRace.end(); )
	{
		pUser = static_cast<CUser*>( prj.GetUserByID( it->first ) );
		if( !IsValidObj(pUser) )
		{
			g_dpDBClient.SendRainbowRaceFailedUser( it->first );
			SAFE_DELETE( it->second );
			m_mapRainbowRace.erase( it++ );
		}
		else
		{
			pUser->AddRainbowRaceState( it->second );
			it++;
		}
	}
}

void CRainbowRaceMng::SetNPC()
{
	m_Lua.GetGloabal( "tNPC" );

	m_Lua.PushNil();
	while( m_Lua.TableLoop( -2 ) )
	{
		DWORD dwNPCID = CScript::GetDefineNum( m_Lua.GetFieldToString( -1, "strNPCId" ) );
		string strCharKey = m_Lua.GetFieldToString( -1, "strCharKey" );
		float x = static_cast<float>( m_Lua.GetFieldToNumber( -1, "xPos" ) );
		float y = static_cast<float>( m_Lua.GetFieldToNumber( -1, "yPos" ) );
		float z = static_cast<float>( m_Lua.GetFieldToNumber( -1, "zPos" ) );
		D3DXVECTOR3 vPos( x, y, z );
		
		CMover* pMover	= (CMover*)CreateObj( D3DDEVICE, OT_MOVER, dwNPCID );
		if( !pMover )
		{
			Error( "CRainbowRaceMng::SetNPC() - pMover is NULL - %s, %s", 
					m_Lua.GetFieldToString( -1, "strNPCId" ), strCharKey.c_str() );
			return;
		}
		lstrcpy( pMover->m_szCharacterKey, strCharKey.c_str() );
		pMover->InitNPCProperty();
		pMover->InitCharacter( pMover->GetCharacter() );
		pMover->SetPos( vPos );
		pMover->InitMotion( MTI_STAND );
		pMover->UpdateLocalMatrix();
		pMover->AddItToGlobalId();
		CWorld* pWorld	= g_WorldMng.GetWorld( WI_WORLD_MADRIGAL );
		if( pWorld )
			pWorld->ADDOBJ( pMover, FALSE, nDefaultLayer );

		m_vecNPCId.push_back( pMover->GetId() );
		m_Lua.Pop( 1 );
	}
	m_Lua.Pop( 0 );
}

void CRainbowRaceMng::RemoveNPC()
{
	for( DWORD i=0; i<m_vecNPCId.size(); i++ )
	{
		CMover* pMover = prj.GetMover( m_vecNPCId[i] );
		if( IsValidObj( pMover ) )
		{
			g_UserMng.AddCreateSfxObj( pMover, XI_SKILL_MER_ONE_SPECIALHIT02 );
			pMover->Delete();
		}
	}

	m_vecNPCId.clear();
}

void CRainbowRaceMng::OnMiniGamePacket( CUser* pUser, __MINIGAME_PACKET* pMiniGamePacket )
{
	CRainbowRace* pRainbowRace = GetRainbowRacerPtr( pUser->m_idPlayer );
	if( pRainbowRace )
		pRainbowRace->OnMiniGamePacket( pUser, pMiniGamePacket );
}

void CRainbowRaceMng::SetRanking( CUser* pUser )
{
	CRainbowRace* pRainbowRace = GetRainbowRacerPtr( pUser->m_idPlayer );
	if( pRainbowRace )
	{
		if( !pRainbowRace->IsFinished() )
		{
			if( pRainbowRace->IsAllCompleted() )
			{
				pRainbowRace->SetFinish();	// 완주 설정...
				pUser->AddRainbowRaceState( pRainbowRace );
				m_vecdwRankingId.push_back( pUser->m_idPlayer );
				g_dpDBClient.SendRainbowRaceRanking( pUser->m_idPlayer, m_vecdwRankingId.size() );
				char szText[255]  = {0,};
				sprintf( szText, GETTEXT( TID_GAME_RAINBOWRACE_FINISH ), m_vecdwRankingId.size() );
				pUser->AddDiagText( szText );
			}
			else
				pUser->AddDefinedText( TID_GAME_RAINBOWRACE_ISNOTALLCOMPLETED );
				
		}
		else
			pUser->AddDefinedText( TID_GAME_RAINBOWRACE_ISFINISHED );
	}
}

void CRainbowRaceMng::SetPrize()
{
	int nCount = 0;
	if( m_Lua.GetLuaFunction( "GetPrize" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );
		if( m_Lua.IsNil( -1 ) )
		{
			m_Lua.Pop( 0 );
			return;
		}
	}

	m_Lua.PushNil();
	while( m_Lua.TableLoop( -2 ) )
	{
		int nPenya = 0;
		CItemElem itemElem;
		
		DWORD dwItemId = static_cast<DWORD>( CScript::GetDefineNum( m_Lua.GetFieldToString( -1, "strItemId" ) ) );
		if( dwItemId != NULL_ID )
		{
			itemElem.m_dwItemId = dwItemId;
			itemElem.m_nItemNum = static_cast<int>( m_Lua.GetFieldToNumber( -1, "nItemNum" ) );
			itemElem.SetFlag( static_cast<BYTE>( m_Lua.GetFieldToNumber( -2, "byFlag" ) ) );
			itemElem.m_bCharged = itemElem.GetProp()->bCharged;
		}
		nPenya = static_cast<int>( m_Lua.GetFieldToNumber( -1, "nPenya" ) * GetApplicantNum() );
		if( nPenya < 0 ) // overflow이면 0페냐
			nPenya = 0;
		
		if( (int)( m_vecdwRankingId.size() ) <= nCount )	// 순위안에 든 사람이 상품 받을 수보다 적으면 중지.
			break;

		char szTitle[100] = {0,}, szText[255] = {0,}, szTemp[100] = {0,};
		sprintf( szTitle, GETTEXT( TID_GAME_RAINBOWRACE_POSTPRIZE_TITLE ) );
		sprintf( szText, GETTEXT( TID_GAME_RAINBOWRACE_POSTPRIZE_TEXT_RANKING ), nCount+1 );
		if( nPenya > 0 )	// 페냐가 지급되면 우편 내용에 페냐 금액 표시
		{
			sprintf( szTemp, GETTEXT( TID_GAME_RAINBOWRACE_POSTPRIZE_TEXT_PENYA ), nPenya );
			strcat( szText, szTemp );
		}

		if( dwItemId != NULL_ID )	// 아이템이 지급되면 우편 내용에 아이템명과 개수 표시
		{
			sprintf( szTemp, GETTEXT( TID_GAME_RAINBOWRACE_POSTPRIZE_TEXT_ITEM ), itemElem.GetName(), itemElem.m_nItemNum );
			strcat( szText, szTemp );
		}

		g_dpDBClient.SendQueryPostMail( m_vecdwRankingId[nCount], 0, itemElem, nPenya, szTitle, szText );
		nCount++;
		m_Lua.Pop( 1 );
	}

	m_Lua.Pop( 0 );
}

void CRainbowRaceMng::SetMiniGamePrize( DWORD dwTick )
{
	for( vector<MINIGMAME_PRIZE_LIST>::iterator it=m_vecMiniGamePrizeList.begin(); it!=m_vecMiniGamePrizeList.end(); )
	{
		if( (dwTick != 0) && ((*it).dwCompletedTick + SEC(15) > dwTick) )
		{
			it++;
			continue;
		}
	
		if( m_Lua.GetLuaFunction( "GetMiniGamePrize" ) )
		{
			m_Lua.PushNumber( (*it).nCompletedNum );
			m_Lua.CallLuaFunction( 1, 1 );
			if( !m_Lua.IsNil( -1 ) )
			{
				CItemElem itemElem;
				itemElem.m_dwItemId = static_cast<DWORD>( CScript::GetDefineNum( m_Lua.GetFieldToString( -1, "strItemId" ) ) );
				itemElem.m_nItemNum = static_cast<int>( m_Lua.GetFieldToNumber( -1, "nItemNum" ) );
				itemElem.SetFlag( static_cast<BYTE>( m_Lua.GetFieldToNumber( -1, "byFlag" ) ) );
				itemElem.m_bCharged = itemElem.GetProp()->bCharged;
				char szText[128] = {0,};
				strcpy( szText, GETTEXT( TID_GAME_RAINBOWRACE_MINIGAMEPRIZE ) );
				g_dpDBClient.SendQueryPostMail( (*it).dwPlayerId, 0, itemElem, 0, szText, szText );
			}
		}
		it = m_vecMiniGamePrizeList.erase( it );
		m_Lua.Pop( 0 );
	}
}

void CRainbowRaceMng::SetPrevRanking( vector<DWORD> & vecPrevRanking )
{
	if( !g_eLocal.GetState( EVE_RAINBOWRACE ) )
		return;

	m_vecPrevRanking.clear();
	m_vecPrevRanking.assign( vecPrevRanking.begin(), vecPrevRanking.end() );
}

vector<DWORD>	CRainbowRaceMng::GetPrevRanking()
{
	vector<DWORD> vecPrevRanking;
	for( DWORD i=0; (i<5 && i<m_vecPrevRanking.size()); i++ )		// 상위 5위까지만...
		vecPrevRanking.push_back( m_vecPrevRanking[i] );

	return vecPrevRanking;
}
#endif // __WORLDSERVER

#endif // __RAINBOW_RACE