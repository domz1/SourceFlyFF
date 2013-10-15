// GuildCombat1to1.cpp: implementation of the CGuildCombat1to1 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GuildCombat1to1.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#if __VER >= 11 // __GUILD_COMBAT_1TO1

#ifdef __WORLDSERVER
	#include "worldmng.h"
	#include "guild.h"
	#include "user.h"	
	#include "DPCoreClient.h"
	#include "defineText.h"
	#include "DPDatabaseClient.h"
	#include "DPSrvr.h"
	extern	CWorldMng	g_WorldMng;
	extern	CDPSrvr		g_DPSrvr;
	extern	CGuildMng	g_GuildMng;
	extern	CUserMng	g_UserMng;
	extern  CDPCoreClient g_DPCoreClient;
	extern	CGuildCombat g_GuildCombatMng;
	extern  CDPDatabaseClient g_dpDBClient;
#endif // __WORLDSERVER
//////////////////////////////////////////////////////////////////////
//// CGuildCombat1To1 ////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
#ifdef __WORLDSERVER
// 1:1길드대전장 생성시 각 대전장별 저장할 정보
CGuildCombat1to1::CGuildCombat1to1( vector<CGuildCombat1to1Mng::__GC1TO1TENDER>& vecTenderGuild, int nStageId )
{
	ClearTime();
	m_nState = GC1TO1WAR_CLOSE;
	m_nProgWarCount = m_nTotalWarCount = 0;
//	m_nRealWarCount = g_GuildCombat1to1Mng.m_nMaxJoinPlayer;
	m_vecTenderGuild = vecTenderGuild;

	int nIndex = 0;
	for( int i=0; i<(int)( m_vecTenderGuild.size() ); i++ )
	{
		if( m_vecTenderGuild[i].nStageId == nStageId )
		{
			m_nIndex[nIndex] = i;
			m_vecTenderGuild[i].nPosition = nIndex;
			m_vecTenderGuild[i].nNowPlayerLife = g_GuildCombat1to1Mng.m_nPlayerLife;	// 생명수 초기화
			nIndex = 1;
//			if( m_vecTenderGuild[i].vecMemberId.size() < m_nRealWarCount )
//				m_nRealWarCount = m_vecTenderGuild[i].vecMemberId.size();
			if( (int)( m_vecTenderGuild[i].vecMemberId.size() ) > m_nTotalWarCount )
				m_nTotalWarCount = m_vecTenderGuild[i].vecMemberId.size();		
		}
	}
}

CGuildCombat1to1::~CGuildCombat1to1()
{

}

void	CGuildCombat1to1::ClearTime()
{
	m_nWaitTime = 0;
	m_dwTime = 0;
}

void	CGuildCombat1to1::Process()
{
	
	if( m_nWaitTime > 0 )
	{
		m_nWaitTime -= GetTickCount() - m_dwTime;
		m_dwTime = GetTickCount();	
	}
	
	switch( m_nState )
	{
		case GC1TO1WAR_CLOSE :	// 
			break;
		
		case GC1TO1WAR_WAIT :	// 현재 순번의 선수 대전 대기중...
		{
			if( m_nWaitTime <=0 )
			{
				ClearTime();
				GuildCombat1to1War();
			}
			break;
		}

		case GC1TO1WAR_WAR :	// 현재 순선의 선수 대전 중...
		{
			if( m_nWaitTime <= 0 )
			{
				ClearTime();
				GuildCombat1to1WarResultCheck();
			}
			break;
		}
		
		case GC1TO1WAR_FINISH :		// 현재 순번의 선수 대전 종료(결과 처리)
		{
			ClearTime();
			SendNowStateAllPlayer();
			SendWarResultAllPlayer();
			SetTeleportToWaitStageNowPlayers();
			m_nProgWarCount++;
			GuildCombat1to1WarWait();
			break;
		}

		case GC1TO1WAR_CLOSEWAIT:	// 현재 대전장의 대전이 모두 종료되고 마을로 텔로포트 하기전 대기 시간..
		{
			if( m_nWaitTime <= 0 )
				GuildCombat1to1Close();
			break;
		}
	}
}

// 현재 대전장에 존재하는 모든 유저에게 상태 전달
void	CGuildCombat1to1::SendNowStateAllPlayer()
{
	CUser* pUser = NULL;
	for( int i=0; i<2; i++ )
	{
		for( int j=0; j<(int)( m_vecTenderGuild[m_nIndex[i]].vecMemberId.size() ); j++ )
		{
			pUser = (CUser*)prj.GetUserByID( m_vecTenderGuild[m_nIndex[i]].vecMemberId[j] );
			if( IsPossibleUser( pUser ) )
				SendNowState( pUser );
		}
	}
}

// 현재 상태 전달
void	CGuildCombat1to1::SendNowState( CUser* pUser )
{
	switch( m_nState )
	{
		case GC1TO1WAR_WAIT :
		{
			if( IsPossibleUser( pUser ) )
				pUser->AddGC1to1NowState( m_nState, m_nWaitTime, m_nProgWarCount );
			break;
		}
		
		case GC1TO1WAR_WAR :
		case GC1TO1WAR_CLOSEWAIT :
		{
			if( IsPossibleUser( pUser ) )
				pUser->AddGC1to1NowState( m_nState, m_nWaitTime );
			break;
		}

		case GC1TO1WAR_FINISH :
		case GC1TO1WAR_CLOSE :
		{
			if( IsPossibleUser( pUser ) )
				pUser->AddGC1to1NowState( m_nState, 0 );
			break;
		}
	}
}

// 현재 대전의 경기 시작과 결과를 대전장의 모든 유저에게 전달
void	CGuildCombat1to1::SendWarResultAllPlayer()
{
	CUser* pUser = NULL;
	for( int i=0; i<2; i++ )
	{
		for( int j=0; j<(int)( m_vecTenderGuild[m_nIndex[i]].vecMemberId.size() ); j++ )
		{
			pUser = (CUser*)prj.GetUserByID( m_vecTenderGuild[m_nIndex[i]].vecMemberId[j] );
			if( IsPossibleUser( pUser ) )
			{
				u_long uIdPlayer = NULL_ID;
				if( (int)( m_vecTenderGuild[m_nIndex[i]].vecMemberId.size() ) > m_nProgWarCount	)
					uIdPlayer = m_vecTenderGuild[m_nIndex[i]].vecMemberId[m_nProgWarCount];
				//else
				//	continue;

				switch( m_nState )
				{
					case GC1TO1WAR_WAR :	// 경기 시작시
					{
						pUser->AddGC1to1WarResult( g_GuildCombat1to1Mng.GC1TO1_PLAYER_WAR, uIdPlayer, 0, 0 );
						break;
					}

					case GC1TO1WAR_FINISH :		// 경기 종료시
					{
						int nOurWinCount = m_vecTenderGuild[m_nIndex[i]].nWinCount;
						int nOtherWinCount = m_vecTenderGuild[m_nIndex[(i+1)%2]].nWinCount;
						
						pUser->AddGC1to1WarResult( m_vecTenderGuild[m_nIndex[i]].nLastWinState, uIdPlayer, nOurWinCount, nOtherWinCount );
						
						// 전체 메세지 출력
						int nIndex = 2;	// 비겼을 때 
						if( m_vecTenderGuild[m_nIndex[0]].nLastWinState == g_GuildCombat1to1Mng.GC1TO1_PLAYER_WIN )
							nIndex = 0; // 0길드가 승리했을 때
						else if( m_vecTenderGuild[m_nIndex[1]].nLastWinState == g_GuildCombat1to1Mng.GC1TO1_PLAYER_WIN )
							nIndex = 1;	// 1길드가 승리했을 때
						
						// 대전장의 모든 선수에게 메세지 출력	
						if( nIndex == 2 ) // 비겼을 때
							pUser->AddDefinedCaption( TRUE, TID_GAME_GUILDCOMBAT1TO1_DRAWMSG, "%d", m_nProgWarCount+1 );
						else
						{
							CUser* pWinUser = NULL;
							if( (int)( m_vecTenderGuild[m_nIndex[nIndex]].vecMemberId.size() ) > m_nProgWarCount	)								
								pWinUser = (CUser*)prj.GetUserByID( m_vecTenderGuild[m_nIndex[nIndex]].vecMemberId[m_nProgWarCount] );
							if( IsValidObj( pWinUser ) )
							{
								CGuild* pGuild = pWinUser->GetGuild();
								if( pGuild )
									pUser->AddDefinedCaption( TRUE, TID_GAME_GUILDCOMBAT1TO1_WINMSG, "%d %s %s", m_nProgWarCount+1, pGuild->m_szGuild, pWinUser->GetName() );
							}
						}
						break;
					}
				} // switch
			} // if
		} // for j
	} // for i
}

// 대전 가능한 대상인가?
BOOL	CGuildCombat1to1::IsPossibleUser( CUser* pUser )
{
	if( !pUser )
		return FALSE;
	
	int nIndex = g_GuildCombat1to1Mng.GetTenderGuildIndexByUser( pUser );
	if( nIndex != NULL_ID )
	{
		CWorld* pWorld = pUser->GetWorld();
		if( pWorld && pWorld->GetID() == m_vecTenderGuild[nIndex].dwWorldId )
			return TRUE;
	}
		
	return FALSE;
}

// 승리시 보상받는 칩갯수(루아 스크립트로)
int		CGuildCombat1to1::GetChipNum()
{
	return (int)g_GuildCombat1to1Mng.m_Lua.GetGlobalNumber( "WinChipNum" );
}

// 승자에게 칩지급하고 메세지 출력(패자는 메세지만 출력)
void	CGuildCombat1to1::SetWinChip( CUser* pUserWin, CUser* pUserLost, BOOL bDraw, BOOL bDiag )
{
	if( bDraw )	// 비겼을 때(칩 지급 안하고 메세지만 출력)
	{
		if( bDiag )	// 개인 승패만 메세지 박스 출력
		{
			if( IsPossibleUser( pUserWin ) )
				pUserWin->AddDiagText( prj.GetText( TID_GAME_GUILDCOMBAT_1TO1_WARDRAW ) );
			if( IsPossibleUser( pUserLost ) )
				pUserLost->AddDiagText( prj.GetText( TID_GAME_GUILDCOMBAT_1TO1_WARDRAW ) );
		}
		return;
	}

	if( IsPossibleUser( pUserWin ) )
	{
		CItemElem itemElem;
		itemElem.m_dwItemId = II_CHP_RED;
		itemElem.m_nItemNum = GetChipNum();
		if( itemElem.m_nItemNum < 1 )
			itemElem.m_nItemNum = 1;
		itemElem.m_bCharged = itemElem.GetProp()->bCharged;
		itemElem.SetSerialNumber();

		LogItemInfo aLogItem;
		aLogItem.Action = "W";
		aLogItem.Gold = pUserWin->GetItemNum( II_CHP_RED );
		aLogItem.Gold2 = pUserWin->GetItemNum( II_CHP_RED ) + itemElem.m_nItemNum;
		aLogItem.Gold_1 = itemElem.m_nItemNum;
		aLogItem.SendName = pUserWin->GetName();
		aLogItem.WorldId = pUserWin->GetWorld()->m_dwWorldID;
				
		if( pUserWin->CreateItem( &itemElem ) )
		{
			aLogItem.RecvName = "GUILDCOMBAT1TO1_CHIP";
			if( !bDiag )
				aLogItem.RecvName = "GUILDCOMBAT1TO1_G_CHIP";
		}
		else	// 지급 실패시(인벤토리가 꽉찼거나 등의 이유로...) 메일로 지급
		{
			g_dpDBClient.SendQueryPostMail( pUserWin->m_idPlayer, 0, itemElem, 0, itemElem.GetProp()->szName, (char*)GETTEXT( TID_GAME_GUILDCOMBAT1TO1_WINCHIPPOST ) );
			//pUserWin->AddDefinedText( TID_GAME_MINIGAME_ITEM_POST );
			aLogItem.RecvName = "GUILDCOMBAT1TO1_CHIP_POST";
			if( !bDiag )
				aLogItem.RecvName = "GUILDCOMBAT1TO1_CHIP_G_POST";
		}
		
		if( bDiag ) // 개인 승패만 메세지 박스 출력
		{
			CString strTemp;
			strTemp.Format( prj.GetText( TID_GAME_GUILDCOMBAT_1TO1_WARWIN ), GetChipNum() );
			pUserWin->AddDiagText( strTemp );
		}
			
		g_DPSrvr.OnLogItem( aLogItem, &itemElem, itemElem.m_nItemNum );
	}

	if( IsPossibleUser( pUserLost ) && bDiag )
		pUserLost->AddDiagText( prj.GetText( TID_GAME_GUILDCOMBAT_1TO1_WARLOST ) );
}

// 현재 대전장의 대전이 모두 끝나고 텔레포트전 대기 시간...
void	CGuildCombat1to1::GuildCombat1to1CloseWait()
{
	ClearTime();
	m_nState = GC1TO1WAR_CLOSEWAIT;
	m_nWaitTime = (int)( g_GuildCombat1to1Mng.m_Lua.GetGlobalNumber( "WarCloseWaitTime" ) );
	m_dwTime = GetTickCount();
	SendNowStateAllPlayer();

	// 승리 길드 판별
	int nWinIndex = NULL_ID;
	if( m_vecTenderGuild[m_nIndex[0]].nWinCount > m_vecTenderGuild[m_nIndex[1]].nWinCount )
		nWinIndex = m_nIndex[0];
	else if( m_vecTenderGuild[m_nIndex[0]].nWinCount < m_vecTenderGuild[m_nIndex[1]].nWinCount )
		nWinIndex = m_nIndex[1];
	
	// 길드 승패 메세지 출력(승리 길드는 칩 추가 지급)
	CUser* pUser = NULL;
	for( int i=0; i<2; i++ )
	{
		for( int j=0; j<(int)( m_vecTenderGuild[m_nIndex[i]].vecMemberId.size() ); j++ )
		{
			pUser = (CUser*)prj.GetUserByID( m_vecTenderGuild[m_nIndex[i]].vecMemberId[j] );
			if( IsPossibleUser( pUser ) )
			{
				if( m_nIndex[i] == nWinIndex )	// 승리 길드
				{
					SetWinChip( pUser, NULL, FALSE, FALSE );
					pUser->AddDefinedCaption( TRUE, TID_GAME_GUILDCOMBAT1TO1_WARGUILDWIN, "" );
					pUser->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_WARGUILDWINCHIP, "%d", GetChipNum() );
				}
				else if( nWinIndex == NULL_ID ) // 비김
				{
					SetWinChip( pUser, NULL, TRUE, FALSE );
					pUser->AddDefinedCaption( TRUE, TID_GAME_GUILDCOMBAT1TO1_WARGUILDDRAW, "" );
				}
				else							// 패배 길드
				{
					SetWinChip( NULL, pUser, FALSE, FALSE );
					pUser->AddDefinedCaption( TRUE, TID_GAME_GUILDCOMBAT1TO1_WARGUILDLOST, "" );
				}

				SendNowState( pUser );
			}
		}
	}

	g_dpDBClient.SendGC1to1Tender( 'U', m_vecTenderGuild[m_nIndex[0]].ulGuildId, m_vecTenderGuild[m_nIndex[0]].nPenya, 'E' );
	g_dpDBClient.SendGC1to1Tender( 'U', m_vecTenderGuild[m_nIndex[1]].ulGuildId, m_vecTenderGuild[m_nIndex[1]].nPenya, 'E' );
	g_dpDBClient.SendGC1to1WarGuild( m_vecTenderGuild[m_nIndex[0]].dwWorldId, 0, 0, 'E' );
}

// 현재 대전장 종료하고 마을로 텔레포트
void	CGuildCombat1to1::GuildCombat1to1Close()
{
	ClearTime();
	m_nState = GC1TO1WAR_CLOSE;

	CUser* pUser = NULL;
	for( int i=0; i<2; i++ )
	{
		for( int j=0; j<(int)( m_vecTenderGuild[m_nIndex[i]].vecMemberId.size() ); j++ )
		{
			pUser = (CUser*)prj.GetUserByID( m_vecTenderGuild[m_nIndex[i]].vecMemberId[j] );
			if( IsPossibleUser( pUser ) )
			{
				SendNowState( pUser );
				pUser->REPLACE( g_uIdofMulti, WI_WORLD_MADRIGAL, D3DXVECTOR3( 6983.0f, 0.0f, 3326.0f ), REPLACE_NORMAL, nDefaultLayer );
			}
		}
	}
}

// 대전하기전 대기시간.. 대전자가 없을 때의 승패처리도 같이 한다.
void	CGuildCombat1to1::GuildCombat1to1WarWait()
{
	// 모든 대전이 끝났을 때
	if( (m_nProgWarCount >= m_nTotalWarCount) || (m_nProgWarCount >= g_GuildCombat1to1Mng.m_nMaxJoinPlayer) )
	{
		GuildCombat1to1CloseWait();
		return;
	}
	
	CUser* pUser0 = NULL;
	CUser* pUser1 = NULL;
	
	// 한쪽 인원수가 많은 경우
/*
	if( m_nProgWarCount >= m_nRealWarCount )
	{
		if( m_vecTenderGuild[m_nIndex[0]].vecMemberId.size() > m_vecTenderGuild[m_nIndex[1]].vecMemberId.size() )
			pUser0 = (CUser*)prj.GetUserByID( m_vecTenderGuild[m_nIndex[0]].vecMemberId[m_nProgWarCount] );
		else
			pUser1 = (CUser*)prj.GetUserByID( m_vecTenderGuild[m_nIndex[1]].vecMemberId[m_nProgWarCount] );
	}
	else
	{
		pUser0 = (CUser*)prj.GetUserByID( m_vecTenderGuild[m_nIndex[0]].vecMemberId[m_nProgWarCount] );
		pUser1 = (CUser*)prj.GetUserByID( m_vecTenderGuild[m_nIndex[1]].vecMemberId[m_nProgWarCount] );
	}
*/
	if( (int)( m_vecTenderGuild[m_nIndex[0]].vecMemberId.size() ) > m_nProgWarCount	)
	{
		pUser0 = (CUser*)prj.GetUserByID( m_vecTenderGuild[m_nIndex[0]].vecMemberId[m_nProgWarCount] );
	}

	if( (int)( m_vecTenderGuild[m_nIndex[1]].vecMemberId.size() ) > m_nProgWarCount	)
	{
		pUser1 = (CUser*)prj.GetUserByID( m_vecTenderGuild[m_nIndex[1]].vecMemberId[m_nProgWarCount] );
	}
		
	// 양쪽 대전자가 모두 존재할 경우 전투대기
	if( IsPossibleUser( pUser0 ) && IsPossibleUser( pUser1 ) )
	{
		m_nState = GC1TO1WAR_WAIT;
		m_nWaitTime = (int)g_GuildCombat1to1Mng.m_Lua.GetGlobalNumber( "WarWaitTime" );
		m_dwTime	= GetTickCount();
		SendNowStateAllPlayer();
	}
	else	// 한쪽 대전자가 없을 경우 승패처리
		GuildCombat1to1WarResultCheck();
}

// 현재 대전자가 모두 존재하면 대전 시작
void	CGuildCombat1to1::GuildCombat1to1War()
{
	CUser* pUser0 = NULL;
	CUser* pUser1 = NULL;

	if( (int)( m_vecTenderGuild[m_nIndex[0]].vecMemberId.size() ) > m_nProgWarCount	)
		pUser0 = (CUser*)prj.GetUserByID( m_vecTenderGuild[m_nIndex[0]].vecMemberId[m_nProgWarCount] );
	if( (int)( m_vecTenderGuild[m_nIndex[1]].vecMemberId.size() ) > m_nProgWarCount	)
		pUser1 = (CUser*)prj.GetUserByID( m_vecTenderGuild[m_nIndex[1]].vecMemberId[m_nProgWarCount] );

	// 양쪽 대전자가 모두 존재하면 대전 시작
	if( IsPossibleUser( pUser0 ) && IsPossibleUser( pUser1 ) )
	{
		g_dpDBClient.SendGC1to1WarPerson( m_vecTenderGuild[m_nIndex[0]].ulGuildId, m_vecTenderGuild[m_nIndex[0]].vecMemberId[m_nProgWarCount], 'N' );
		g_dpDBClient.SendGC1to1WarPerson( m_vecTenderGuild[m_nIndex[1]].ulGuildId, m_vecTenderGuild[m_nIndex[1]].vecMemberId[m_nProgWarCount], 'N' );
		SetTeleportToWarStage( pUser0 );	// 대전장으로 텔레포트
		SetTeleportToWarStage( pUser1 );	// 대전장으로 텔레포트 

		m_nState = GC1TO1WAR_WAR;
		m_nWaitTime = (int)g_GuildCombat1to1Mng.m_Lua.GetGlobalNumber( "WarTime" );
		m_dwTime = GetTickCount();
		SendNowStateAllPlayer();
		SendWarResultAllPlayer();	// 대전 시작 알림
	}
	else
		GuildCombat1to1WarWait();

}

// 대전 결과 처리 및 대전중 로그 아웃시 처리
void	CGuildCombat1to1::GuildCombat1to1WarResultCheck( BOOL bLogOut, CUser* pUserLogOut )
{
	CUser* pUser0 = NULL;
	CUser* pUser1 = NULL;
	if( (int)( m_vecTenderGuild[m_nIndex[0]].vecMemberId.size() ) > m_nProgWarCount	)
		pUser0 = (CUser*)prj.GetUserByID( m_vecTenderGuild[m_nIndex[0]].vecMemberId[m_nProgWarCount] );
	if( (int)( m_vecTenderGuild[m_nIndex[1]].vecMemberId.size() ) > m_nProgWarCount	)
		pUser1 = (CUser*)prj.GetUserByID( m_vecTenderGuild[m_nIndex[1]].vecMemberId[m_nProgWarCount] );

	// 로그 아웃 상황일 때
	if( bLogOut )
	{
		if( pUser0 == pUserLogOut )
			pUser0 = NULL;
		if( pUser1 == pUserLogOut )
			pUser1 = NULL;
	}

	// 양쪽 선수가 모두 존재할 때
	if( IsPossibleUser( pUser0 ) && IsPossibleUser( pUser1 ) )
	{
		for( int i=0; i<2; i++ )
		{
			if( m_vecTenderGuild[m_nIndex[i]].nNowPlayerLife == 0 )
			{
				m_vecTenderGuild[m_nIndex[(i+1)%2]].nWinCount++;
				SetLastWinState( (i+1)%2 );
				m_nState = GC1TO1WAR_FINISH;
				// 승리( 칩 지급 )
				if( (i+1)%2 == 0 )
					SetWinChip( pUser0, pUser1 );
				else
					SetWinChip( pUser1, pUser0 );
				return;
			}
		}
		
		// 전쟁 시간이 종료되었을 때
		if( m_nWaitTime <= 0 )
		{
			if( m_vecTenderGuild[m_nIndex[0]].nNowPlayerLife > m_vecTenderGuild[m_nIndex[1]].nNowPlayerLife )
			{
				m_vecTenderGuild[m_nIndex[0]].nWinCount++;
				SetLastWinState( 0 );
				SetWinChip( pUser0, pUser1 );	// 칩 지급
			}
			else if( m_vecTenderGuild[m_nIndex[0]].nNowPlayerLife < m_vecTenderGuild[m_nIndex[1]].nNowPlayerLife )
			{
				m_vecTenderGuild[m_nIndex[1]].nWinCount++;
				SetLastWinState( 1 );
				SetWinChip( pUser1, pUser0 );	// 칩 지급
			}
			else
			{
				SetLastWinState( 2 );	// 2는 비겼을 때
				SetWinChip( pUser0, pUser1, TRUE );	// 칩 지급하지 않고 비김 메세지 출력
			}
				
			m_nState = GC1TO1WAR_FINISH;
		}
	}
	else if( IsPossibleUser( pUser0 ) && !IsPossibleUser( pUser1 ) )
	{
		m_vecTenderGuild[m_nIndex[0]].nWinCount++;
		SetLastWinState( 0 );
		m_nState = GC1TO1WAR_FINISH;
		SetWinChip( pUser0, pUser1 );	// 승리( 칩 지급 )
		pUser0->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_WARNOOTHER );
	}
	else if( !IsPossibleUser( pUser0 ) && IsPossibleUser( pUser1 ) )
	{
		m_vecTenderGuild[m_nIndex[1]].nWinCount++;
		SetLastWinState( 1 );
		m_nState = GC1TO1WAR_FINISH;
		SetWinChip( pUser1, pUser0 );	// 승리( 칩 지급 )
		pUser1->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_WARNOOTHER );
	}
	else
	{
		SetLastWinState( 2 );	// 2는 비겼을 때
		m_nState = GC1TO1WAR_FINISH;
		SetWinChip( pUser0, pUser1, TRUE );	// 비김
	}
}

// 최근 경기 결과를 저장한다.
void	CGuildCombat1to1::SetLastWinState( int nIndex )
{
	// 생명수 초기화
	for( int i=0; i<2; i++ )
		m_vecTenderGuild[m_nIndex[i]].nNowPlayerLife = g_GuildCombat1to1Mng.m_nPlayerLife;

	switch( nIndex )
	{
		case 0 :	// 0이 이겼을 때
			m_vecTenderGuild[m_nIndex[0]].nLastWinState = g_GuildCombat1to1Mng.GC1TO1_PLAYER_WIN;
			if( (int)( m_vecTenderGuild[m_nIndex[0]].vecMemberId.size() ) > m_nProgWarCount	)
				g_dpDBClient.SendGC1to1WarPerson( m_vecTenderGuild[m_nIndex[0]].ulGuildId, m_vecTenderGuild[m_nIndex[0]].vecMemberId[m_nProgWarCount], 'W' );
			m_vecTenderGuild[m_nIndex[1]].nLastWinState = g_GuildCombat1to1Mng.GC1TO1_PLAYER_LOSE;
			if( (int)( m_vecTenderGuild[m_nIndex[1]].vecMemberId.size() ) > m_nProgWarCount	)
				g_dpDBClient.SendGC1to1WarPerson( m_vecTenderGuild[m_nIndex[1]].ulGuildId, m_vecTenderGuild[m_nIndex[1]].vecMemberId[m_nProgWarCount], 'L' );
			break;

		case 1 :	// 1이 이겼을 때
			m_vecTenderGuild[m_nIndex[0]].nLastWinState = g_GuildCombat1to1Mng.GC1TO1_PLAYER_LOSE;
			if( (int)( m_vecTenderGuild[m_nIndex[0]].vecMemberId.size() ) > m_nProgWarCount	)
				g_dpDBClient.SendGC1to1WarPerson( m_vecTenderGuild[m_nIndex[0]].ulGuildId, m_vecTenderGuild[m_nIndex[0]].vecMemberId[m_nProgWarCount], 'L' );
			m_vecTenderGuild[m_nIndex[1]].nLastWinState = g_GuildCombat1to1Mng.GC1TO1_PLAYER_WIN;
			if( (int)( m_vecTenderGuild[m_nIndex[1]].vecMemberId.size() ) > m_nProgWarCount	)
				g_dpDBClient.SendGC1to1WarPerson( m_vecTenderGuild[m_nIndex[1]].ulGuildId, m_vecTenderGuild[m_nIndex[1]].vecMemberId[m_nProgWarCount], 'W' );
			break;

		case 2 :	// 비겼을 때
			m_vecTenderGuild[m_nIndex[0]].nLastWinState = g_GuildCombat1to1Mng.GC1TO1_PLAYER_DRAW;
			if( (int)( m_vecTenderGuild[m_nIndex[0]].vecMemberId.size() ) > m_nProgWarCount	)
				g_dpDBClient.SendGC1to1WarPerson( m_vecTenderGuild[m_nIndex[0]].ulGuildId, m_vecTenderGuild[m_nIndex[0]].vecMemberId[m_nProgWarCount], 'D' );
			m_vecTenderGuild[m_nIndex[1]].nLastWinState = g_GuildCombat1to1Mng.GC1TO1_PLAYER_DRAW;
			if( (int)( m_vecTenderGuild[m_nIndex[1]].vecMemberId.size() ) > m_nProgWarCount	)
				g_dpDBClient.SendGC1to1WarPerson( m_vecTenderGuild[m_nIndex[1]].ulGuildId, m_vecTenderGuild[m_nIndex[1]].vecMemberId[m_nProgWarCount], 'D' );
			break;
	}
}

// 졌을 때 생명감소 처리
void	CGuildCombat1to1::SetLost( CUser* pUserDef )
{
	CUser* pUser0 = NULL;
	CUser* pUser1 = NULL;
	
	if( (int)( m_vecTenderGuild[m_nIndex[0]].vecMemberId.size() ) > m_nProgWarCount	)
		pUser0 = (CUser*)prj.GetUserByID( m_vecTenderGuild[m_nIndex[0]].vecMemberId[m_nProgWarCount] );
	if( (int)( m_vecTenderGuild[m_nIndex[1]].vecMemberId.size() ) > m_nProgWarCount	)
		pUser1 = (CUser*)prj.GetUserByID( m_vecTenderGuild[m_nIndex[1]].vecMemberId[m_nProgWarCount] );

	if( IsPossibleUser( pUser0 ) && IsPossibleUser( pUser1 ) )
	{
		if( pUser0 == pUserDef )
			m_vecTenderGuild[m_nIndex[0]].nNowPlayerLife--;
		else if( pUser1 == pUserDef )
			m_vecTenderGuild[m_nIndex[1]].nNowPlayerLife--;
	}

	GuildCombat1to1WarResultCheck();
}

// 최근 대전자를 대전장에서 대기실로 텔레포트 한다.
void	CGuildCombat1to1::SetTeleportToWaitStageNowPlayers()
{
	CUser* pUser0 = NULL;
	CUser* pUser1 = NULL;

	if( (int)( m_vecTenderGuild[m_nIndex[0]].vecMemberId.size() ) > m_nProgWarCount	)
		pUser0 = (CUser*)prj.GetUserByID( m_vecTenderGuild[m_nIndex[0]].vecMemberId[m_nProgWarCount] );
	if( (int)( m_vecTenderGuild[m_nIndex[1]].vecMemberId.size() ) > m_nProgWarCount	)
		pUser1 = (CUser*)prj.GetUserByID( m_vecTenderGuild[m_nIndex[1]].vecMemberId[m_nProgWarCount] );
	
	if( IsPossibleUser( pUser0 ) )
		SetTeleportToWaitStage( pUser0 );
	if( IsPossibleUser( pUser1 ) )
		SetTeleportToWaitStage( pUser1 );
}

// 대기실로 텔레포트(입장시, 대전종료시)
void	CGuildCombat1to1::SetTeleportToWaitStage( CUser* pUser )
{
	int nIndex = g_GuildCombat1to1Mng.GetTenderGuildIndexByUser( pUser );
	if( !g_WorldMng.GetWorld( m_vecTenderGuild[nIndex].dwWorldId ) )
	{
		pUser->AddText( "Not Exist World" );
		return;
	}
	pUser->m_nGuildCombatState = 2; // 관전자 모드
	g_UserMng.AddGuildCombatUserState( pUser );
	
	int nRandx = xRandom(4) - 2;
	int nRandz = xRandom(4) - 2;
	if( m_vecTenderGuild[nIndex].nPosition == 0 )
	{
		pUser->SetAngle( 0 );
		pUser->REPLACE( g_uIdofMulti, m_vecTenderGuild[nIndex].dwWorldId, D3DXVECTOR3( (float)( 256+nRandx ), 90.0f, (float)( 277+nRandz ) ), REPLACE_NORMAL, nDefaultLayer );
	}
	else
	{
		pUser->SetAngle( 180 );
		pUser->REPLACE( g_uIdofMulti, m_vecTenderGuild[nIndex].dwWorldId, D3DXVECTOR3( (float)( 272+nRandx ), 90.0f, (float)( 212+nRandz ) ), REPLACE_NORMAL, nDefaultLayer );
	}
}

// 대전장으로 텔레포트
void	CGuildCombat1to1::SetTeleportToWarStage( CUser* pUser )
{
	int nIndex = g_GuildCombat1to1Mng.GetTenderGuildIndexByUser( pUser );

	pUser->m_nGuildCombatState = 1; // 길드대전 모드
	g_UserMng.AddGuildCombatUserState( pUser );
	if( m_vecTenderGuild[nIndex].nPosition == 0 )
	{
		pUser->SetAngle( 0 );
		pUser->REPLACE( g_uIdofMulti, m_vecTenderGuild[nIndex].dwWorldId, D3DXVECTOR3( 260, 75, 252 ), REPLACE_NORMAL, nDefaultLayer );
	}
	else
	{
		pUser->SetAngle( 180 );
		pUser->REPLACE( g_uIdofMulti, m_vecTenderGuild[nIndex].dwWorldId, D3DXVECTOR3( 260, 75, 235 ), REPLACE_NORMAL, nDefaultLayer );
	}
}

#endif // __WORLDSERVER



//////////////////////////////////////////////////////////////////////
//// CGuildCombat1To1Mng /////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
CGuildCombat1to1Mng::CGuildCombat1to1Mng()
{
	m_nJoinPenya = 0;
	m_nMaxJoinPlayer = 0;
	m_nMinJoinPlayer = 0;
	m_nMinJoinPlayerLevel = 0;
#ifdef __DBSERVER
	m_nCombatId = 0;
#endif // __DBSERVER
#if defined(__WORLDSERVER) || defined(__DBSERVER)
	m_nGuildLevel = 0;
	m_nPlayerLife = 0;
	m_nMaxJoinGuild = 0;
	m_nMinJoinGuild = 0;
	m_nCancelReturnRate = 0;
	m_nFailReturnRate = 0;
#endif // __WORLDSERVER || __DBSERVER
	ClearTime();
	m_nState = GC1TO1_CLOSE;
#ifdef __CLIENT
	m_vecGuildCombat1to1_Players.clear();
	m_nGuildCombat1to1Guild = 0;
	m_nMyGuildCount = 0;
	m_nVsGuildCount = 0;
#endif //__CLIENT
}

CGuildCombat1to1Mng::~CGuildCombat1to1Mng()
{

}

#ifdef __CLIENT
void CGuildCombat1to1Mng::AddGuildCombat1to1Player(u_long nPlayerId)
{
	__GC1TO1_PLAYER_INFO nTempInfo;
	nTempInfo.m_uidPlayer = nPlayerId;
	nTempInfo.m_nState = GC1TO1_PLAYER_READY;
	m_vecGuildCombat1to1_Players.push_back(nTempInfo);
}

void CGuildCombat1to1Mng::SetGuildCombat1to1Guild(u_long nGuildId)
{
	m_nGuildCombat1to1Guild = nGuildId;
}

void CGuildCombat1to1Mng::SetGuildCombat1to1State(int nState, u_long nPlayer, int nMyGuildPoint, int nVsGuildPoint)
{
	vector < __GC1TO1_PLAYER_INFO >::iterator it = m_vecGuildCombat1to1_Players.begin();
	for( ; it != m_vecGuildCombat1to1_Players.end(); ++it )
	{
		__GC1TO1_PLAYER_INFO &Playerinfo = *it;
		if( Playerinfo.m_uidPlayer ==  nPlayer )
			Playerinfo.m_nState = nState;
	}

	if(nState != GC1TO1_PLAYER_READY && nState != GC1TO1_PLAYER_WAR)
	{
		m_nMyGuildCount = nMyGuildPoint;
		m_nVsGuildCount = nVsGuildPoint;
	}
}

// 클라용 대전 가능한 유저인가?
BOOL	CGuildCombat1to1Mng::IsPossibleMover( CMover* pMover )
{
	if( !pMover )
		return FALSE;
	
	CWorld* pWorld = pMover->GetWorld();
	if( pWorld )
	{
		if( pWorld->GetID() >= WI_WORLD_GUILDWAR1TO1_0 &&
			pWorld->GetID() <= WI_WORLD_GUILDWAR1TO1_L )
		{
			if( pMover->m_nGuildCombatState != 0 )
				return TRUE;
		}
	}
	
	return FALSE;
}

#endif //__CLIENT

void	CGuildCombat1to1Mng::ClearTime()
{
	m_nWaitTime = 0;
	m_dwTime = 0;
}

#ifdef __DBSERVER
// 길드대전 오픈 시간 체크
int		CGuildCombat1to1Mng::CheckOpenTIme()
{
	int nReturnValue = 0;
	if( m_Lua.GetLuaFunction( "CheckOpenTime" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );
		
		nReturnValue = (int)( m_Lua.ToNumber( -1 ) );
	}
	m_Lua.Pop(0);

	return nReturnValue;
}
#endif // __DBSERVER

#if defined(__WORLDSERVER) || defined(__DBSERVER)
// 스크립트 로딩 (월드, 데이터베이스)
BOOL CGuildCombat1to1Mng::LoadScript()
{
	if( m_Lua.RunScript( "GuildCombat1to1.lua" ) != 0 )
		ASSERT(0);

	m_nJoinPenya = (int)m_Lua.GetGlobalNumber( "MinJoinPenya" );
	m_nGuildLevel = (int)m_Lua.GetGlobalNumber( "MinGuildLevel" );
	m_nMinJoinGuild = (int)m_Lua.GetGlobalNumber( "MinJoinGuild" );
	m_nMaxJoinGuild = (int)m_Lua.GetGlobalNumber( "MaxJoinGuild" );
	m_nMinJoinPlayer = (int)m_Lua.GetGlobalNumber( "MinJoinPlayer" );
#ifndef __MAINSERVER
	m_nMinJoinPlayer = 1;
#endif // __MAINSERVER
	m_nMaxJoinPlayer = (int)m_Lua.GetGlobalNumber( "MaxJoinPlayer" );
	m_nMinJoinPlayerLevel = (int)m_Lua.GetGlobalNumber( "MinJoinPlayerLevel" );
	m_nCancelReturnRate = (int)m_Lua.GetGlobalNumber( "CancelReturnRate" );
	m_nFailReturnRate = (int)m_Lua.GetGlobalNumber( "FailReturnRate" );
	m_nPlayerLife = (int)m_Lua.GetGlobalNumber( "PlayerLife" );

	return TRUE;
}
#endif // __WORLDSERVER || __DBSERVER

#ifdef __WORLDSERVER
// 1:1길드대전 오픈시 각 대전장 생성
void	CGuildCombat1to1Mng::CreateGuildCombat1to1()
{
	multimap<int, int> mapRandom;

	int	i = 0;
	for( i=0; i<(int)( m_vecTenderGuild.size() ); i++ )
	{
		mapRandom.insert( make_pair( xRandom(m_vecTenderGuild.size()), i/2 ) );
	}
	
	i = 0;
	for( multimap<int, int>::iterator it=mapRandom.begin(); it!=mapRandom.end(); it++ )
	{
		m_vecTenderGuild[i].nStageId = it->second;
		// 각 대전장의 WorldID를 저장
		m_vecTenderGuild[i].dwWorldId = WI_WORLD_GUILDWAR1TO1_0 + it->second;
		i++;
	}

	m_vecGuilCombat1to1.clear();
	for( i=0; i<(int)( m_vecTenderGuild.size()/2 ); i++ )
	{
		CGuildCombat1to1 GuildCombat1to1( m_vecTenderGuild, i );
		m_vecGuilCombat1to1.push_back( GuildCombat1to1 );
	}
}

// 입찰 길드(m_vecTenderGuild)의 인덱스 값
int		CGuildCombat1to1Mng::GetTenderGuildIndexByUser( CUser* pUser )
{
	CGuild* pGuild = pUser->GetGuild();
	if( pGuild )
	{
		for( int i=0; i<(int)( m_vecTenderGuild.size() ); i++ )
		{
			if( pGuild->GetGuildId() == m_vecTenderGuild[i].ulGuildId )
				return i;
		}
	}

	return NULL_ID;
}

// 상대 길드의 index 값
int		CGuildCombat1to1Mng::GetTargetTenderGuildIndexByUser( CUser* pUser )
{
	int nIndex = GetTenderGuildIndexByUser( pUser );
	for( int i=0; i<(int)( m_vecTenderGuild.size() ); i++ )
	{
		if( nIndex != i && m_vecTenderGuild[nIndex].nStageId == m_vecTenderGuild[i].nStageId )
			return i;
	}

	return NULL_ID;
}

// 참가자 구성에 포함된 유저인가?
BOOL	CGuildCombat1to1Mng::IsLineUpMember( CUser* pUser )
{
	int nIndex = GetTenderGuildIndexByUser( pUser );
	if( nIndex == NULL_ID )
		return FALSE;

	for( int i=0; i<(int)( m_vecTenderGuild[nIndex].vecMemberId.size() ); i++ )
	{
		if( m_vecTenderGuild[nIndex].vecMemberId[i] == pUser->m_idPlayer )
			return TRUE;
	}
	
	return FALSE;
}

// 길드마스터 인가?
BOOL	CGuildCombat1to1Mng::IsGuildMaster( CUser* pUser )
{
	CGuild* pGuild = pUser->GetGuild();
	if( pGuild && pGuild->IsMaster( pUser->m_idPlayer ) )
		return TRUE;

	return FALSE;
}

// 이미 길드대전에 신청한 길드인가?
BOOL	CGuildCombat1to1Mng::IsGCTenderGuild( CUser* pUser )
{
	if( g_GuildCombatMng.FindGuildCombatMember( pUser->m_idGuild ) &&
		g_GuildCombatMng.FindGuildCombatMember( pUser->m_idGuild )->bRequest )
		return TRUE;

	return FALSE;
}

// 현재 대전장의 대전 가능한 유저인가?
BOOL	CGuildCombat1to1Mng::IsPossibleUser( CUser* pUser )
{
	if( !pUser )
		return FALSE;

	int nIndex = GetTenderGuildIndexByUser( pUser );
	if( nIndex != NULL_ID )
	{
		int nStageId = m_vecTenderGuild[nIndex].nStageId;
		if( nStageId != NULL_ID && m_vecGuilCombat1to1[nStageId].IsPossibleUser( pUser ) )
			return TRUE;
	}

	return FALSE;
}

void	CGuildCombat1to1Mng::Process()
{
	if( !g_eLocal.GetState( EVE_GUILDCOMBAT1TO1 ) )
		return;

	if( m_nWaitTime > 0 )
	{
		m_nWaitTime -= GetTickCount() - m_dwTime;
		m_dwTime = GetTickCount();	
	}
	
	switch( m_nState )
	{
		case GC1TO1_CLOSE :	// 모든 대전장의 대전이 종료되었거나 오픈되지 않은 상태
			break;
		
		case GC1TO1_OPEN : // 1:1 길드대전 오픈된 상태...(참가자 구성시간)
		{
			if( m_nWaitTime <= 0 )
			{
				ClearTime();
				CreateGuildCombat1to1();
				GuildCombat1to1EntranceWait();
			}
			break;
		}

		case GC1TO1_ENTRANCE :	// 입장 대기 시간
		{
			if( m_nWaitTime <=0 )
			{
				ClearTime();
				GuildCombat1to1War();
			}
			break;
		}

		case GC1TO1_WAR :	// 모든 대전장 오픈 및 대전 중
		{
			int nCloseCount=0;
			for( int i=0; i<(int)( m_vecGuilCombat1to1.size() ); i++ )
			{
				m_vecGuilCombat1to1[i].Process();	// 각 대전장별 process
				if( m_vecGuilCombat1to1[i].IsClosed() )
					nCloseCount++;
			}

			if( nCloseCount >= (int)( m_vecGuilCombat1to1.size() ) )	// 모든 대전장의 대전이 종료되었을 때 처리
			{
				ClearTime();
				g_DPCoreClient.SendCaption( prj.GetText( TID_GAME_GUILDCOMBAT1TO1_ALLCLOSE ) );
				GuildCombat1to1AllClose();
			}
			break;
		}
	}
}

// 현재 State를 Client에 전송한다.(접속시, state 변경시)
void	CGuildCombat1to1Mng::SendNowState( CUser* pUser )
{
	if( !g_eLocal.GetState( EVE_GUILDCOMBAT1TO1 ) )
		return;
	
	switch( m_nState )
	{
		case GC1TO1_OPEN :
		{
			int nIndex = GetTenderGuildIndexByUser( pUser );
//			if( nIndex != NULL_ID && IsGuildMaster( pUser ) )
			if( nIndex != NULL_ID )
				pUser->AddGC1to1NowState( m_nState, m_nWaitTime );
			break;
		}
			
		case GC1TO1_ENTRANCE :
		{
			if( IsLineUpMember( pUser ) )
				pUser->AddGC1to1NowState( m_nState, m_nWaitTime );
			else if( IsGuildMaster( pUser ) )
				pUser->AddGC1to1NowState( GC1TO1_WAR, 0 );
			break;
		}		

		case GC1TO1_WAR :
		{
			int nIndex = GetTenderGuildIndexByUser( pUser );
			if( nIndex != NULL_ID )
				pUser->AddGC1to1NowState( m_nState, 0 );
			break;
		}

		case GC1TO1_CLOSE :
		{
			int nIndex = GetTenderGuildIndexByUser( pUser );
			if( nIndex != NULL_ID )
				pUser->AddGC1to1NowState( m_nState, 0 );
			break;
		}
	}
}

// 1대1 길드대전 오픈
void	CGuildCombat1to1Mng::GuildCombat1to1Open( BOOL bGMText )
{
	if( !g_eLocal.GetState( EVE_GUILDCOMBAT1TO1 ) )
		return;
	
	vector<__GC1TO1TENDER>::iterator it;
	if( !bGMText )
	{
		for( it=m_vecTenderFailGuild.begin(); it!=m_vecTenderFailGuild.end(); it++ )
		{
			// 이전 1:1길드대전에 입찰 실패 길드가 신청금을 수령하지 않았을 경우
			g_dpDBClient.SendGC1to1Tender( 'U', (*it).ulGuildId, (*it).nPenya, 'N' );
		}
		m_vecTenderFailGuild.clear();
	}
	
	// 신청한  길드수가 최소 참여 길드수보다 적을 때
	if( (int)( m_vecTenderGuild.size() ) < m_nMinJoinGuild )
	{
		// 1대1 길드대전 취소 및 연장 처리
		CString strTemp;
		strTemp.Format( prj.GetText( TID_GAME_GUILDCOMBAT1TO1_NOGAME ), m_nMinJoinGuild );
		g_DPCoreClient.SendCaption( strTemp );	
		ClearTime();
		m_nState = GC1TO1_CLOSE;
		return;
	}
	
	// 1:1길드대전 오픈 메세지..
	g_DPCoreClient.SendCaption( prj.GetText( TID_GAME_GUILDCOMBAT1TO1_OPEN ) );
	g_DPCoreClient.SendCaption( prj.GetText( TID_GAME_GUILDCOMBAT1TO1_LINEUP ) );
	
	// 참가자 구성 시간 설정
	m_nState = GC1TO1_OPEN;
	m_nWaitTime = (int)m_Lua.GetGlobalNumber( "MemberLineUpTime" );
	m_dwTime = GetTickCount();	

	// 입찰 순위에 들지 못한 길드 제거
	int nCount = 0;
	for( it=m_vecTenderGuild.begin(); it!=m_vecTenderGuild.end(); )
	{
		nCount++;
		if( nCount <= m_nMaxJoinGuild )
			it++;
		else // 입찰 순위에서 밀려난 경우
		{
			CGuild* pGuild = g_GuildMng.GetGuild( (*it).ulGuildId );
			if( pGuild )
			{
				CUser* pUser = (CUser*)prj.GetUserByID( pGuild->m_idMaster );
				if( IsValidObj( pUser ) )
					pUser->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_FAILTENDER );
			}
			
			g_dpDBClient.SendGC1to1Tender( 'U', (*it).ulGuildId, (*it).nPenya, 'F' );
			m_vecTenderFailGuild.push_back( (*it) );
			it = m_vecTenderGuild.erase( it );
		}
	}		

	// 입찰 순위에는 들어갔지만 상대 길드가 없는 경우(홀수)
	if( m_vecTenderGuild.size() % 2 != 0 )
	{
		it = m_vecTenderGuild.end(); it--;	// 마지막 순위		// 노트 :  rbegin()을 사용하시오.
		CGuild* pGuild = g_GuildMng.GetGuild( (*it).ulGuildId );
		if( pGuild )
		{
			CUser* pUser = (CUser*)prj.GetUserByID( pGuild->m_idMaster );
			if( IsValidObj( pUser ) )
				pUser->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_FAILTENDER );
		}

		g_dpDBClient.SendGC1to1Tender( 'U', (*it).ulGuildId, (*it).nPenya, 'F' );
		m_vecTenderFailGuild.push_back( (*it) );
		m_vecTenderGuild.erase( it );
	}

	// 출전 가능한 길드의 모든 길드원에게 OPEN STATE 알림(길드 마스터는 참가자 구성 시간 출력)
	for( it=m_vecTenderGuild.begin(); it!=m_vecTenderGuild.end(); it++ )
	{
		CGuild* pGuild = g_GuildMng.GetGuild( (*it).ulGuildId );
		if( pGuild )
		{
			for( map<u_long, CGuildMember*>::iterator it2=pGuild->m_mapPMember.begin(); it2!=pGuild->m_mapPMember.end(); it2++ )
			{
				CUser* pUser = (CUser*)prj.GetUserByID( it2->first );
				if( IsValidObj( pUser ) )
					SendNowState( pUser );
			}
		}
	}

	g_dpDBClient.SendGC1to1StateToDBSrvr();	// 1:1길드대전이 오픈되었다고 DBServer에 알림
}

// 각 대전장의 1:1길드대전 시작
void	CGuildCombat1to1Mng::GuildCombat1to1War()
{
	ClearTime();
	m_nState = GC1TO1_WAR;
	for( int i=0; i<(int)( m_vecTenderGuild.size() ); i++ )
	{
		for( int j=0; j<(int)( m_vecTenderGuild[i].vecMemberId.size() ); j++ )
		{
			CUser* pUser = (CUser*)prj.GetUserByID( m_vecTenderGuild[i].vecMemberId[j] );
			if( IsValidObj( pUser ) )
				SendNowState( pUser );
		}
	}

	for( int i=0; i<(int)( m_vecGuilCombat1to1.size() ); i++ )
	{
		DWORD dwWorldId = m_vecTenderGuild[m_vecGuilCombat1to1[i].m_nIndex[0]].dwWorldId;
		u_long uGuildId_0 = m_vecTenderGuild[m_vecGuilCombat1to1[i].m_nIndex[0]].ulGuildId;
		u_long uGuildId_1 = m_vecTenderGuild[m_vecGuilCombat1to1[i].m_nIndex[1]].ulGuildId;
		g_dpDBClient.SendGC1to1WarGuild( dwWorldId, uGuildId_0, uGuildId_1, 'W' );

		m_vecGuilCombat1to1[i].GuildCombat1to1WarWait();
	}
}

// 모든 대전장의 길드대전이 끝났다.
void	CGuildCombat1to1Mng::GuildCombat1to1AllClose()
{
	ClearTime();
	m_nState = GC1TO1_CLOSE;
	for( int i=0; i<(int)( m_vecTenderGuild.size() ); i++ )
	{
		CGuild* pGuild = g_GuildMng.GetGuild( m_vecTenderGuild[i].ulGuildId );
		if( pGuild )
		{
			for( map<u_long, CGuildMember*>::iterator it=pGuild->m_mapPMember.begin(); it!=pGuild->m_mapPMember.end(); it++ )
			{
				CUser* pUser = (CUser*)prj.GetUserByID( it->first );
				if( IsValidObj(pUser) )
					SendNowState( pUser );
			}
		}
	}

	m_vecGuilCombat1to1.clear();
	m_vecTenderGuild.clear();
	g_dpDBClient.SendGC1to1StateToDBSrvr();
}

// 입찰 신청 윈도우를 오픈할때 
void	CGuildCombat1to1Mng::SendTenderGuildOpenWnd( CUser* pUser )
{
	if( !g_eLocal.GetState( EVE_GUILDCOMBAT1TO1 ) )
		return;
	
	if( m_nState != GC1TO1_CLOSE )	// 입찰할 수 없다.
	{
		pUser->AddDiagText( prj.GetText( TID_GAME_GUILDCOMBAT1TO1_NOTENDER ) );
		return;
	}
	
	// 길드마스터만 입찰 가능
	if( !IsGuildMaster( pUser ) )
	{
		pUser->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_NOTMASTER );
		return;
	}

	// 길드대전에 입찰한 길드는 입찰할 수 없다.
	if( IsGCTenderGuild( pUser ) )
	{
		pUser->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_ISGCTENDER );
		return;
	}

	// 참가할 수 있는 최소 길드레벨 검사...
	if( pUser->GetGuild()->m_nLevel < m_nGuildLevel )
	{
		pUser->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_LOWGUILDLEVEL, "%d", m_nGuildLevel );
		return;
	}

	int nIndex = GetTenderGuildIndexByUser( pUser );
	if( nIndex != NULL_ID )
		pUser->AddGC1to1TenderOpenWnd( m_vecTenderGuild[nIndex].nPenya );
	else
		pUser->AddGC1to1TenderOpenWnd( 0 );
}

// 입찰 현황 보기
void	CGuildCombat1to1Mng::SendTenderGuildView( CUser* pUser )
{
	if( !g_eLocal.GetState( EVE_GUILDCOMBAT1TO1 ) )
		return;

	if( m_nState != GC1TO1_CLOSE )
	{
		pUser->AddDiagText( prj.GetText( TID_GAME_GUILDCOMBAT1TO1_NOTENDER ) );
		return;
	}

	int nTenderPenya = 0;
	int nTenderRanking = 0;

	// 입찰된 길드의 길드원이면..
	int nIndex = GetTenderGuildIndexByUser( pUser );
	if( nIndex != NULL_ID )
	{
		nTenderPenya = m_vecTenderGuild[nIndex].nPenya;
		nTenderRanking = nIndex+1;
	}
		
	time_t t;
	if( m_Lua.GetLuaFunction( "GetRemainNextTime" ) )	// 오픈까지 남은 시간 얻어옴(루아)
	{
		m_Lua.CallLuaFunction( 0, 1 );
		t = (time_t)( m_Lua.ToNumber( -1 ) );
	}
	m_Lua.Pop(0);
	
	pUser->AddGC1to1TenderGuildView( nTenderPenya, nTenderRanking, t, m_vecTenderGuild );
}

// 입찰 및 입찰금 순서로 정렬
void	CGuildCombat1to1Mng::SetTenderGuild( CUser* pUser, int nPenya )
{
	if( !g_eLocal.GetState( EVE_GUILDCOMBAT1TO1 ) )
		return;
	
	if( m_nState != GC1TO1_CLOSE )
	{
		pUser->AddDiagText( prj.GetText( TID_GAME_GUILDCOMBAT1TO1_NOTENDER ) );
		return;
	}
	
	// 길드대전에 입찰한 길드는 입찰할 수 없다.
	if( IsGCTenderGuild( pUser ) )
	{
		pUser->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_ISGCTENDER );
		return;
	}
	
	// 길드마스터만 입찰 가능
	if( !IsGuildMaster( pUser ) )
	{
		pUser->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_NOTMASTER );
		return;
	}

	// 참가할 수 있는 최소 길드레벨 검사...
	if( pUser->GetGuild()->m_nLevel < m_nGuildLevel )
	{
		pUser->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_LOWGUILDLEVEL, "%d", m_nGuildLevel );
		return;
	}
	
	// 최소 입찰 페냐 보다 많아야 한다.
	if( nPenya < m_nJoinPenya )
	{
		pUser->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_LIMIT_MIN );
		return;
	}

	// 로그 남길 준비(아이템로그)
	LogItemInfo aLogItem;
	aLogItem.Action = "9";
	aLogItem.SendName = pUser->GetName();
	aLogItem.RecvName = "GUILDCOMBAT1TO1";
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	//aLogItem.ItemName = "SEED";
	_stprintf( aLogItem.szItemName, "%d", II_GOLD_SEED1 );
	
	int nIndex = GetTenderGuildIndexByUser( pUser );
	if( nIndex != NULL_ID ) // 이미 입찰한 길드는 더많은 금액으로 입찰해야 한다.
	{
		if( m_vecTenderGuild[nIndex].nPenya < nPenya )
		{
			if( (DWORD)( nPenya ) > (DWORD)(pUser->GetGold() + m_vecTenderGuild[nIndex].nPenya) )
			{
				pUser->AddDefinedText( TID_GAME_LACKMONEY );	// 페냐가 부족하다.
				return;
			}

			pUser->AddGold( m_vecTenderGuild[nIndex].nPenya - nPenya );
			CString strTemp;
			strTemp.Format( prj.GetText( TID_GAME_GUILDCOMBAT1TO1_MORETENDER ), nPenya - m_vecTenderGuild[nIndex].nPenya, nPenya );
			pUser->AddDiagText( strTemp );
			m_vecTenderGuild[nIndex].nPenya = nPenya;
			// 로그 남김
			aLogItem.Gold = pUser->GetGold() + (nPenya - m_vecTenderGuild[nIndex].nPenya) ;
			aLogItem.Gold2 = pUser->GetGold();
			aLogItem.Gold_1 = m_vecTenderGuild[nIndex].nPenya - nPenya;
			g_DPSrvr.OnLogItem( aLogItem );
			g_dpDBClient.SendGC1to1Tender( 'U', pUser->m_idGuild, nPenya, 'T' );
		}
		else // 기존보다 더 많은 페냐 입찰 요구
		{
			pUser->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_MORE_CURRENT_REQUEST );
			return;
		}
	}
	else	// 처음 입찰하는 길드이면..
	{
		if( nPenya > pUser->GetGold() )	// 페냐 부족
		{
			pUser->AddDefinedText( TID_GAME_LACKMONEY );
			return;
		}

		__GC1TO1TENDER	TenderGuild;
		TenderGuild.ulGuildId = pUser->m_idGuild;
		TenderGuild.nPenya = nPenya;
		m_vecTenderGuild.push_back( TenderGuild );
		pUser->AddGold( -nPenya );
		CString strTemp;
		strTemp.Format( prj.GetText( TID_GAME_GUILDCOMBAT1TO1_TENDERPENYA ), nPenya );
		pUser->AddDiagText( strTemp );
		// 로그 남김
		aLogItem.Gold = pUser->GetGold() + nPenya;
		aLogItem.Gold2 = pUser->GetGold();
		aLogItem.Gold_1 = -nPenya;
		g_DPSrvr.OnLogItem( aLogItem );
		g_dpDBClient.SendGC1to1Tender( 'I', pUser->m_idGuild, nPenya, 'T' );
	}

	// 입찰금 순서로 정렬
	if( m_vecTenderGuild.size() < 2 )
		return;
	for( int i=0; i<(int)( m_vecTenderGuild.size()-1 ); i++ )
	{
		for( int j=0; j<(int)( m_vecTenderGuild.size()-i-1 ); j++ )
		{
			if( m_vecTenderGuild[j].nPenya < m_vecTenderGuild[j+1].nPenya )
			{
				__GC1TO1TENDER temp = m_vecTenderGuild[j];
				m_vecTenderGuild[j] = m_vecTenderGuild[j+1];
				m_vecTenderGuild[j+1] = temp;
			}
		}
	}
}

// 입찰 취소 
void	CGuildCombat1to1Mng::SetCancelTenderGuild( CUser* pUser )
{
	if( m_nState != GC1TO1_CLOSE )
	{
		pUser->AddDiagText( prj.GetText( TID_GAME_GUILDCOMBAT1TO1_NOTENDER ) );
		return;
	}

	// 길드마스터만 입찰 취소 가능
	if( !IsGuildMaster( pUser ) )
	{
		pUser->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_NOTMASTER );
		return;
	}

	CGuild* pGuild = pUser->GetGuild();
	if( pGuild )
	{
		for( vector<__GC1TO1TENDER>::iterator it=m_vecTenderGuild.begin(); it!=m_vecTenderGuild.end(); it++ )
		{
			if( (*it).ulGuildId == pGuild->GetGuildId() )
			{
				int nReturnGold = (int)( (*it).nPenya * ( (float)m_nCancelReturnRate / 100 ) );
				//인벤 꽉 찼을때 예외처리 해야함..
				int nTotal = pUser->GetGold() + nReturnGold;
				if( nReturnGold > 0 && nTotal < 0 )	// overflow
				{
					pUser->AddDefinedText( TID_GAME_LACKSPACE );
					return;
				}

				pUser->AddGold( nReturnGold );
			
				g_dpDBClient.SendGC1to1Tender( 'U', (*it).ulGuildId, (*it).nPenya, 'C' );
				m_vecTenderGuild.erase( it );
				
				CString strTemp;
				strTemp.Format( prj.GetText( TID_GAME_GUILDCOMBAT1TO1_CANCELTENDER ), nReturnGold );
				pUser->AddDiagText( strTemp );

				// 로그 남김(아이템로그)
				LogItemInfo aLogItem;
				aLogItem.Action = "9";
				aLogItem.SendName = pUser->GetName();
				aLogItem.RecvName = "GUILDCOMBAT1TO1_CANCELRETURN";
				aLogItem.WorldId = pUser->GetWorld()->GetID();
				//aLogItem.ItemName = "SEED";
				_stprintf( aLogItem.szItemName, "%d", II_GOLD_SEED1 );
				aLogItem.Gold = pUser->GetGold() - nReturnGold;
				aLogItem.Gold2 = pUser->GetGold();
				aLogItem.Gold_1 = nReturnGold;
				g_DPSrvr.OnLogItem( aLogItem );
				break;
			}
		}
	}
}

// 입찰 실패 길드의 신청금 수령
void	CGuildCombat1to1Mng::SetFailedTenderGuild( CUser* pUser )
{
	// 길드마스터만 입찰 수령 가능
	if( !IsGuildMaster( pUser ) )
	{
		pUser->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_NOTMASTER );
		return;
	}

	CGuild* pGuild = pUser->GetGuild();
	if( pGuild )
	{
		for( vector<__GC1TO1TENDER>::iterator it=m_vecTenderFailGuild.begin(); it!=m_vecTenderFailGuild.end(); it++ )
		{
			if( (*it).ulGuildId == pGuild->GetGuildId() )
			{
				int nReturnGold = (int)( (*it).nPenya * ( (float)m_nFailReturnRate / 100 ) );
				//인벤 꽉 찼을때 예외처리 해야함..
				int nTotal = pUser->GetGold() + nReturnGold;
				if( nReturnGold > 0 && nTotal < 0 )	// overflow
				{
					pUser->AddDefinedText( TID_GAME_LACKSPACE );
					return;
				}

				pUser->AddGold( nReturnGold );
				
				g_dpDBClient.SendGC1to1Tender( 'U', pUser->m_idGuild, (*it).nPenya, 'G' );
				m_vecTenderFailGuild.erase( it );
				
				CString strTemp;
				strTemp.Format( prj.GetText( TID_GAME_GUILDCOMBAT1TO1_FAILRETURN ), nReturnGold );
				pUser->AddDiagText( strTemp );

				// 로그 남김(아이템로그)
				LogItemInfo aLogItem;
				aLogItem.Action = "9";
				aLogItem.SendName = pUser->GetName();
				aLogItem.RecvName = "GUILDCOMBAT1TO1_FAILRETURN";
				aLogItem.WorldId = pUser->GetWorld()->GetID();
				//aLogItem.ItemName = "SEED";
				_stprintf( aLogItem.szItemName, "%d", II_GOLD_SEED1 );
				aLogItem.Gold = pUser->GetGold() - nReturnGold;
				aLogItem.Gold2 = pUser->GetGold();
				aLogItem.Gold_1 = nReturnGold;
				g_DPSrvr.OnLogItem( aLogItem );
				return;
			}
		}

		pUser->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_NOTRETURNGUILD );
		return;
	}
}

// 참가 구성원 작성 윈도우 오픈...
void	CGuildCombat1to1Mng::SendMemberLineUpOpenWnd( CUser* pUser )
{
	if( m_nState != GC1TO1_OPEN )
	{
		pUser->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_NOLINEUP );
		return;
	}
	
	int nIndex = GetTenderGuildIndexByUser( pUser );
	if( nIndex == NULL_ID )
	{
		pUser->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_CANNOT_MAKEUP );
		return;
	}
	
	// 길드마스터 또는 킹핀급만 가능
	if( !IsGuildMaster( pUser ) )
	{
		CGuild* pGuild = pUser->GetGuild();
		if( pGuild )
		{
			CGuildMember* pGuildMember = pGuild->GetMember( pUser->m_idPlayer );
			if( pGuildMember->m_nMemberLv != GUD_KINGPIN )
			{
				pUser->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_NOLINEUPAUTH );
				return;
			}
		}
	}

	pUser->AddGC1to1MemberLineUpOpenWnd( m_vecTenderGuild[nIndex].vecMemberId );
}

// 참가자 구성
void	CGuildCombat1to1Mng::SetMemberLineUp( CUser* pUser, vector<u_long>& vecMemberId )
{
	if( m_nState != GC1TO1_OPEN )	// 참가자 구성 시간이 아니다.
	{
		pUser->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_NOLINEUP );
		return;
	}

	int nIndex = GetTenderGuildIndexByUser( pUser );
	if( nIndex == NULL_ID )	// 입찰되지 않았거나 순위에서 밀린 길드
	{
		pUser->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_CANNOT_MAKEUP );
		return;
	}

	if( (int)( vecMemberId.size() ) < m_nMinJoinPlayer )	// 최소 참가자 수보다 적다.
	{
		pUser->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_LOWMINPLAYER, "%d", m_nMinJoinPlayer );
		return;
	}

	if( (int)( vecMemberId.size() ) > m_nMaxJoinPlayer )
	{
		pUser->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_OVERMAXPLAYER, "%d", m_nMaxJoinPlayer );
		return;
	}

	// 길드마스터 또는 킹핀급만 가능
	CGuild* pGuild = pUser->GetGuild();
	if( pGuild )
	{
		CGuildMember* pGuildMember = pGuild->GetMember( pUser->m_idPlayer );
		if( pGuildMember )
		{
			if( pGuildMember->m_nMemberLv != GUD_MASTER && pGuildMember->m_nMemberLv != GUD_KINGPIN )
			{
				pUser->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_NOLINEUPAUTH );
				return;
			}
		}
	}
			
	BOOL bMasterOrKingpin = FALSE;
	// 최소 참가 레벨 검사
	for( int i=0; i<(int)( vecMemberId.size() ); i++ )
	{
		CUser* pUserTemp = (CUser*)prj.GetUserByID( vecMemberId[i] );
		if( IsValidObj( pUserTemp ) && pUserTemp->GetLevel() < m_nMinJoinPlayerLevel )
		{
			pUser->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_LIMIT_LEVEL_NOTICE );
			return;
		}
		if( IsValidObj( pUserTemp ) && !bMasterOrKingpin )
		{
			CGuildMember* pGuildMember = pGuild->GetMember( pUserTemp->m_idPlayer );
			if( pGuildMember )
				if( pGuildMember->m_nMemberLv == GUD_MASTER || pGuildMember->m_nMemberLv == GUD_KINGPIN )
					bMasterOrKingpin = TRUE;	
		}
	}
	if( !bMasterOrKingpin )
	{
		pUser->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_HAVENOT_MASTER );
		return;
	}
			
	m_vecTenderGuild[nIndex].vecMemberId.clear();
	m_vecTenderGuild[nIndex].vecMemberId = vecMemberId;
	pUser->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_LINEUPCOMPLETE );
	g_dpDBClient.SendGC1to1LineUp( m_vecTenderGuild[nIndex].ulGuildId, vecMemberId );
}

// 입장전 대기...
void	CGuildCombat1to1Mng::GuildCombat1to1EntranceWait()
{
	m_nState = GC1TO1_ENTRANCE;
	m_nWaitTime = (int)m_Lua.GetGlobalNumber( "EntranceWaitTime" );
	m_dwTime = GetTickCount();

	for( int i=0; i<(int)( m_vecTenderGuild.size() ); i++ )
	{
		CGuild* pGuild = g_GuildMng.GetGuild( m_vecTenderGuild[i].ulGuildId );
		if( pGuild )
		{
			CUser* pUser = (CUser*)prj.GetUserByID( pGuild->m_idMaster );
			if( IsValidObj( pUser ) && !IsLineUpMember( pUser ) )
				SendNowState( pUser );
		}

		for( int j=0; j<(int)( m_vecTenderGuild[i].vecMemberId.size() ); j++ )
		{
			CUser* pUser = (CUser*)prj.GetUserByID( m_vecTenderGuild[i].vecMemberId[j] );
			if( IsValidObj( pUser ) )
			{
				pUser->AddDefinedCaption( TRUE, TID_GAME_GUILDCOMBAT1TO1_ENTRANCETIME, "" );
				SendNowState( pUser );
			}
		}
	}
}

// 입장하기 위해 NPC에게 텔레포트
void	CGuildCombat1to1Mng::SetTeleportToNPC( CUser* pUser )
{
	if( m_nState != GC1TO1_ENTRANCE )
	{
		pUser->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_NOENTRANCETIME );
		return;
	}
	
	PRegionElem pRgnElem = g_WorldMng.GetRevivalPos( WI_WORLD_MADRIGAL, "flaris" );
	if( pRgnElem )
		pUser->REPLACE( g_uIdofMulti, WI_WORLD_MADRIGAL, D3DXVECTOR3( 6983.0f, 0.0f, 3326.0f ), REPLACE_NORMAL, nDefaultLayer );
}

// 대전장 대기실로 입장
void	CGuildCombat1to1Mng::SetTeleportToStage( CUser* pUser )
{
	if( m_nState != GC1TO1_ENTRANCE )
	{
		pUser->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_NOENTRANCETIME );
		return;
	}
	
	if( !IsLineUpMember( pUser ) )
	{
		pUser->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_NOT_GUILD_MEMBER );
		return;
	}

	int nIndex = GetTenderGuildIndexByUser( pUser );
	if( nIndex == NULL_ID )
		return;
	int nStageId = m_vecTenderGuild[nIndex].nStageId;
	if( nStageId == NULL_ID )
		return;
	m_vecGuilCombat1to1[nStageId].SetTeleportToWaitStage( pUser );
	
	// 버프 없애기
	pUser->RemoveCommonBuffs();
	g_UserMng.AddRemoveAllSkillInfluence( pUser );
	// 파티 탈퇴
	if( pUser->GetPartyId() > 0 )
		g_DPCoreClient.SendGCRemoveParty( pUser->GetPartyId(), pUser->m_idPlayer );
}	
#endif // __WORLDSERVER

#endif // __GUILD_COMBAT_1TO1