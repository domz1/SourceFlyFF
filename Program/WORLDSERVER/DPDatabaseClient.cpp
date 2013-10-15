#include "StdAfx.h"
#include "DPSrvr.h"
#include "DPCoreClient.h"
#include "DPDatabaseClient.h"
#include "User.h"
#include "WorldMng.h"
#include "misc.h"

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

#include "eveschool.h"
#include "definetext.h"
#include "eveschool.h"
extern	CGuildCombat	g_GuildCombatMng;

extern	CDPSrvr		g_DPSrvr;
extern	CUserMng	g_UserMng;
extern	CWorldMng	g_WorldMng;
extern	CDPCoreClient	g_DPCoreClient;

#include "party.h"
extern	CPartyMng	g_PartyMng;
#include "guild.h"
extern	CGuildMng	g_GuildMng;

#include "post.h"
extern	BOOL CanAdd( DWORD dwGold, int nPlus );
#include "spevent.h"

#if __VER >= 12 // __LORD
#include "slord.h"
#endif	// __LORD

#if __VER >= 12 // __TAX
#include "Tax.h"
#endif // __TAX

extern char		g_szDBAddr[16];

#if __VER >= 13 // __COUPLE_1117
#include "couplehelper.h"
#endif	// __COUPLE_1117

#ifdef __FUNNY_COIN
#include "FunnyCoin.h"
#endif // __FUNNY_COIN

#if __VER >= 14 // __PCBANG
#include "PCBang.h"
#endif // __PCBANG

#if __VER >= 14 // __INSTANCE_DUNGEON
#include "InstanceDungeonBase.h"
#endif // __INSTANCE_DUNGEON

#ifdef __QUIZ
#include "Quiz.h"
#endif // __QUIZ

#if __VER >= 15 // __GUILD_HOUSE
#include "GuildHouse.h"
#endif // __GUILD_HOUSE

#if __VER >= 15 // __CAMPUS
#include "CampusHelper.h"
#endif // __CAMPUS

CDPDatabaseClient	g_dpDBClient;

CDPDatabaseClient::CDPDatabaseClient()
{
	BEGIN_MSG;
	ON_MSG( PACKETTYPE_JOIN, OnJoin );
#if __VER >= 11 // __SYS_PLAYER_DATA
	ON_MSG( PACKETTYPE_ALL_PLAYER_DATA, OnAllPlayerData );
	ON_MSG( PACKETTYPE_ADD_PLAYER_DATA, OnAddPlayerData );
	ON_MSG( PACKETTYPE_DELETE_PLAYER_DATA, OnDeletePlayerData );
	ON_MSG( PACKETTYPE_UPDATE_PLAYER_DATA, OnUpdatePlayerData );
#else	// __SYS_PLAYER_DATA
	ON_MSG( PACKETTYPE_ALLPLAYERID, OnAllPlayerID );
	ON_MSG( PACKETTYPE_PLAYERID, OnPlayerID );
	ON_MSG( PACKETTYPE_REMOVEPLAYERID, OnRemovePlayerID );
#endif	// __SYS_PLAYER_DATA
	ON_MSG( PACKETTYPE_GUILD_BANK, OnGuildBank );
	ON_MSG( PACKETTYPE_UPDATE_GUILD_RANKING_END, OnUpdateGuildRankingFinish );
	ON_MSG( PACKETTYPE_QUERYGUILDQUEST, OnQueryGuildQuest );
	ON_MSG( PACKETTYPE_BASEGAMESETTING, OnBaseGameSetting );
	ON_MSG( PACKETTYPE_MONSTERRESPAWNSETTING, OnMonsterRespawnSetting );
	ON_MSG( PACKETTYPE_MONSTERPROPGAMESETTING, OnMonsterPropSetting );
	ON_MSG( PACKETTYPE_GAMEMASTER_CHATTING, OnGMChat );
	ON_MSG( PACKETTYPE_PING, OnPing );
/*
#ifdef __S0114_RELOADPRO
	ON_MSG( PACKETTYPE_RELOAD_PROJECT, OnReloadProject );
#endif // __S0114_RELOADPRO
*/
	ON_MSG( PACKETTYPE_ADD_GUILDCOMBAT, OnAllGuildCombat );
	ON_MSG( PACKETTYPE_IN_GUILDCOMBAT, OnInGuildCombat );
	ON_MSG( PACKETTYPE_OUT_GUILDCOMBAT, OnOutGuildCombat );
	ON_MSG( PACKETTYPE_RESULT_GUILDCOMBAT, OnResultGuildCombat );
	ON_MSG( PACKETTYPE_PLAYERPOINT_GUILDCOMBAT, OnPlayerPointGuildCombat );
	ON_MSG( PACKETTYPE_GETPENYAGUILD_GUILDCOMBAT, OnGetPenyaGuildGC );
	ON_MSG( PACKETTYPE_GETPENYAPLAYER_GUILDCOMBAT, OnGetPenyaPlayerGC );
	ON_MSG( PACKETTYPE_CONTINUE_GUILDCOMBAT, OnContinueGC );
	ON_MSG( PACKETTYPE_QUERYPOSTMAIL, OnPostMail );
	ON_MSG( PACKETTYPE_QUERYREMOVEMAIL,	OnRemoveMail );
	ON_MSG( PACKETTYPE_QUERYGETMAILITEM,	OnGetMailItem );
	ON_MSG( PACKETTYPE_QUERYGETMAILGOLD,	OnGetMailGold );
	ON_MSG( PACKETTYPE_READMAIL,	OnReadMail );
	ON_MSG( PACKETTYPE_ALLMAIL, OnAllMail );
	ON_MSG( PACKETTYPE_QUERYMAILBOX, OnMailBox );
	

	//////////////////////////////////////////////////////////////////////////
	ON_MSG( PACKETTYPE_QUERYMAILBOX_REQ, OnMailBoxReq );
	//////////////////////////////////////////////////////////////////////////


	ON_MSG( PACKETTYPE_QUERY_REMOVE_GUILD_BANK_TBL, OnQueryRemoveGuildBankTbl );
	ON_MSG( PACKETTYPE_EVENT_GENERIC, OnEventGeneric );
	ON_MSG( PACKETTYPE_EVENT_FLAG, OnEventFlag );

#if __VER >= 9 // __EVENTLUA
	ON_MSG( PACKETTYPE_EVENTLUA_STATE, OnEventStateLua );
	ON_MSG( PACKETTYPE_EVENTLUA_CHANGED, OnEventLuaChanged );
#endif // __EVENTLUA

#if __VER >= 11 // __GUILD_COMBAT_1TO1
	ON_MSG( PACKETTYPE_GC1TO1_OPEN, OnGC1to1Open );
	ON_MSG( PACKETTYPE_GC1TO1_TENDERTOSRVR, OnGC1to1TenderGuildFromDB );
#endif // __GUILD_COMBAT_1TO1
#if __VER >= 11 // __MA_VER11_04	// 길드 창고 로그 기능 world,database,neuz
	ON_MSG( PACKETTYPE_GUILDLOG_VIEW, OnGuildBankLogViewFromDB );
#endif //__MA_VER11_04	// 길드 창고 로그 기능 world,database,neuz
#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
	ON_MSG( PACKETTYPE_SEALCHAR_REQ, OnSealCharFromDB );
	ON_MSG( PACKETTYPE_SEALCHARGET_REQ, OnSealCharGetFromDB );
#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz

#if __VER >= 12 // __LORD
	ON_MSG( PACKETTYPE_ELECTION_ADD_DEPOSIT, OnElectionAddDeposit );
	ON_MSG( PACKETTYPE_ELECTION_SET_PLEDGE, OnElectionSetPledge );
	ON_MSG( PACKETTYPE_ELECTION_INC_VOTE, OnElectionIncVote );
	ON_MSG( PACKETTYPE_ELECTION_BEGIN_CANDIDACY, OnElectionBeginCandidacy );
	ON_MSG( PACKETTYPE_ELECTION_BEGIN_VOTE, OnElectionBeginVote );
	ON_MSG( PACKETTYPE_ELECTION_END_VOTE, OnElectionEndVote );
	ON_MSG( PACKETTYPE_LORD, OnLord );
	ON_MSG( PACKETTYPE_L_EVENT_CREATE, OnLEventCreate );
	ON_MSG( PACKETTYPE_L_EVENT_INITIALIZE, OnLEventInitialize );
	ON_MSG( PACKETTYPE_LORD_SKILL_USE, OnLordSkillUse );
	ON_MSG( PACKETTYPE_LORD_SKILL_TICK, OnLordSkillTick );
	ON_MSG( PACKETTYPE_L_EVENT_TICK, OnLEventTick );
#endif	// __LORD

#if __VER >= 12 // __TAX
	ON_MSG( PACKETTYPE_TAX_ALLINFO, OnTaxInfo );
#endif // __TAX

#if __VER >= 12 // __SECRET_ROOM
	ON_MSG( PACKETTYPE_SECRETROOM_INFO_CLEAR, OnSecretRoomInfoClear );
	ON_MSG( PACKETTYPE_SECRETROOM_TENDERINFO_TO_WSERVER, OnSecretRoomTenderInfo );
#endif // __SECRET_ROOM

#if __VER >= 13 // __RAINBOW_RACE
	ON_MSG( PACKETTYPE_RAINBOWRACE_LOADDBTOWORLD, OnRainbowRaceInfo );
#endif // __RAINBOW_RACE

#if __VER >= 13 // __HOUSING
	ON_MSG( PACKETTYPE_HOUSING_LOADINFO, OnHousingLoadInfo );
	ON_MSG( PACKETTYPE_HOUSING_FURNITURELIST, OnHousingSetFunitureList );
	ON_MSG( PACKETTYPE_HOUSING_SETUPFURNITURE, OnHousingSetupFuniture );
	ON_MSG( PACKETTYPE_HOUSING_SETVISITALLOW, OnHousingSetVisitAllow );
	ON_MSG( PACKETTYPE_HOUSING_DBFAILED, OnHousingDBFailed );
#endif // __HOUSING
	m_bAlive	= TRUE;
	m_cbPing	= 0;
#if __VER >= 13 // __COUPLE_1117
	ON_MSG( PACKETTYPE_PROPOSE, OnProposeResult );
	ON_MSG( PACKETTYPE_COUPLE, OnCoupleResult );
	ON_MSG( PACKETTYPE_DECOUPLE, OnDecoupleResult );
	ON_MSG( PACKETTYPE_ALL_COUPLES, OnCouple );
#if __VER >= 13 // __COUPLE_1202
	ON_MSG( PACKETTYPE_ADD_COUPLE_EXPERIENCE, OnAddCoupleExperience );
#endif	// __COUPLE_1202
#endif	// __COUPLE_1117
#ifdef __FUNNY_COIN
	ON_MSG( PACKETTYPE_FUNNYCOIN_ACK_USE, OnFunnyCoinAckUse );
#endif // __FUNNY_COIN
#if __VER >= 14 // __PCBANG
	ON_MSG( PACKETTYPE_PCBANG_SETAPPLY, OnPCBangToggle );
#endif // __PCBANG
#ifdef __VTN_TIMELIMIT
	ON_MSG( PACKETTYPE_TIMELIMIT_INFO, OnTimeLimitInfoAck );
	ON_MSG( PACKETTYPE_TIMELIMIT_RESET, OnTimeLimitReset );
#endif // __VTN_TIMELIMIT
#ifdef __QUIZ
	ON_MSG( PACKETTYPE_QUIZ_OPEN, OnQuizEventOpen );
	ON_MSG( PACKETTYPE_QUIZ_LOADQUIZ, OnQuizList );
	ON_MSG( PACKETTYPE_QUIZ_NOTICE, OnQuizEventNotice );
	ON_MSG( PACKETTYPE_QUIZ_CHANGED, OnQuizEventChanged );
#endif // __QUIZ

#if __VER >= 15 // __GUILD_HOUSE
	ON_MSG( PACKETTYPE_GUILDHOUSE_LOAD, OnLoadGuildHouse );
	ON_MSG( PACKETTYPE_GUILDHOUSE_BUY, OnBuyGuildHouse );
	ON_MSG( PACKETTYPE_GUILDHOUSE_REMOVE, OnRemoveGuildHouse );
	ON_MSG( PACKETTYPE_GUILDHOUSE_PACKET, OnGuildHousePacket );
#ifdef __GUILD_HOUSE_MIDDLE
	ON_MSG( PACKETTYPE_GUILDHOUSE_TENDER_INFO, OnGuildHouseTenderInfo );
	ON_MSG( PACKETTYPE_GUILDHOUSE_TENDER_JOIN, OnGuildHouseTenderJoin );
	ON_MSG( PACKETTYPE_GUILDHOUSE_TENDER_STATE, OnGuildHouseTenderState );
	ON_MSG( PACKETTYPE_GUILDHOUSE_TENDER_RESULT, OnGuildHouseTenderResult );
	ON_MSG( PACKETTYPE_GUILDHOUSE_GRADE_UPDATE, OnGuildHouseGradeUpdate );
	ON_MSG( PACKETTYPE_GUILDHOUSE_LEVEL_UPDATE, OnGuildHouseLevelUpdate );
	ON_MSG( PACKETTYPE_GUILDHOUSE_EXPIRED, OnGuildHouseExpired );
#endif // __GUILD_HOUSE_MIDDLE
#endif // __GUILD_HOUSE

#if __VER >= 15 // __CAMPUS
	ON_MSG( PACKETTYPE_CAMPUS_ALL, OnAllCampus );
	ON_MSG( PACKETTYPE_CAMPUS_ADD_MEMBER, OnAddCampusMember );
	ON_MSG( PACKETTYPE_CAMPUS_REMOVE_MEMBER, OnRemoveCampusMember );
	ON_MSG( PACKETTYPE_CAMPUS_UPDATE_POINT, OnUpdateCampusPoint );
#endif // __CAMPUS
}

CDPDatabaseClient::~CDPDatabaseClient()
{
}

void CDPDatabaseClient::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_DESTROYPLAYERORGROUP:
			break;
	}
}

void CDPDatabaseClient::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	CAr ar( (LPBYTE)lpMsg + sizeof(DPID) + sizeof(DPID), dwMsgSize - ( sizeof(DPID) + sizeof(DPID) ) );
	GETTYPE( ar );

	static map<DWORD, CString> mapstrProfile;
	map<DWORD, CString>::iterator it = mapstrProfile.find( dw );
	if( it == mapstrProfile.end() )
	{
		CString strTemp;
		strTemp.Format("CDPDatabaseClient::UserMessageHandler(0x%08x)", dw );
		it = mapstrProfile.insert( make_pair( dw, strTemp ) ).first;
	}
	_PROFILE( it->second );

	void ( theClass::*pfn )( theParameters )	=	GetHandler( dw );
	
	if( pfn ) 
		( this->*( pfn ) )( ar, *(UNALIGNED LPDPID)lpMsg, *(UNALIGNED LPDPID)( (LPBYTE)lpMsg + sizeof(DPID) ) );
	else 
		Error( "Handler not found(%08x)\n",dw );
}

#ifdef __LAYER_1015
void CDPDatabaseClient::SavePlayer( CUser* pUser, DWORD dwWorldId, D3DXVECTOR3 & vPos, int nLayer, BOOL bLogout )
#else	// __LAYER_1015
void CDPDatabaseClient::SavePlayer( CUser* pUser, DWORD dwWorldId, const D3DXVECTOR3 & vPos, BOOL bLogout )
#endif	//__LAYER_1015
{
	BEFORESENDDUAL( ar, PACKETTYPE_SAVE_PLAYER, DPID_UNKNOWN, DPID_UNKNOWN );  

	ar << bLogout;

	ar.WriteString( pUser->m_playAccount.lpszAccount );
	ar << ((CMover*)pUser)->m_nSlot;

	pUser->m_dwTimeout4Save	= SEC_SAVEPLAYER;

#if __VER < 14 // __INSTANCE_DUNGEON
#if __VER >= 13 // __HOUSING
	if( nLayer > 0 )
	{
		dwWorldId	= WI_WORLD_MADRIGAL;
		vPos	= D3DXVECTOR3( 6968, 100, 3328 );
		nLayer	= nDefaultLayer;
	}
#endif	// __HOUSING
#endif // __INSTANCE_DUNGEON

	ar << dwWorldId;
	ar << vPos;
#ifdef __LAYER_1015
	ar << nLayer;
#endif	// __LAYER_1015

	CObj::SetMethod( METHOD_NONE );
	// raiders.2006.11.28  저장될 돈 = 인벤돈 + 내 거래창 돈
	int nTrade = pUser->m_vtInfo.TradeGetGold();
	pUser->AddGold( nTrade, FALSE );
	pUser->Serialize( ar );
	pUser->AddGold( -nTrade, FALSE );
	//

	pUser->m_playTaskBar.Serialize( ar );
#ifdef __RT_1025
	ar << pUser->m_RTMessenger.GetState();
#else	// __RT_1025
	pUser->m_Messenger.Serialize( ar );
	pUser->m_Messenger.SetSave();
#endif	// __RT_1025

#ifndef __JEFF_9_20
	if( pUser->m_pActMover->IsFly() )
	{
		pUser->SetFlyTimeOff();
		ar << pUser->GetFlyTime();
		pUser->m_dwSaveFlyTime += pUser->m_dwSendFlyTime;
		pUser->m_dwSendFlyTime = 0;
		pUser->SetFlyTimeOn();
	}
	else
	{
		ar << pUser->GetFlyTime();
		pUser->m_dwSaveFlyTime += pUser->m_dwSendFlyTime;
		pUser->m_dwSendFlyTime = 0;
	}
#endif	// __JEFF_9_20

	ar << pUser->m_dwReturnWorldID;
	ar << pUser->m_vReturnPos;

#ifdef __EVENT_1101
#ifdef __EVENT_1101_2
	ar << pUser->m_nEventFlag;
#else // __EVENT_1101_2
	ar << pUser->m_dwEventFlag;
#endif // __EVENT_1101_2
	ar << pUser->m_dwEventTime;
	ar << pUser->m_dwEventElapsed;
#endif	// __EVENT_1101
	
#ifdef __EXP_ANGELEXP_LOG
	ar << pUser->m_nExpLog;
	ar << pUser->m_nAngelExpLog;
#endif // __EXP_ANGELEXP_LOG

#ifdef __EVENTLUA_COUPON
	ar << pUser->m_nCoupon;
#endif // __EVENTLUA_COUPON

#if __VER >= 15 // __GUILD_HOUSE
	ar << pUser->GetRestPoint();
#endif // __GUILD_HOUSE

	SEND( ar, this, DPID_SERVERPLAYER );
#ifdef __VTN_TIMELIMIT
	//	mulcom	BEGIN100315	베트남 시간 제한
	if( ::GetLanguage() == LANG_VTN )
	{
		SendTimeLimitUpdateReq( pUser->m_playAccount.lpszAccount, pUser->m_nAccountPlayTime );
	}
	//	mulcom	END100315	베트남 시간 제한
#endif // __VTN_TIMELIMIT
}

#ifdef __S_NEW_SKILL_2
void CDPDatabaseClient::SaveSkill( CUser* pUser )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SAVE_SKILL, DPID_UNKNOWN, DPID_UNKNOWN );  

	ar << pUser->m_idPlayer;
	ar << pUser->m_nSkillLevel;
	ar << pUser->m_nSkillPoint;
	ar.Write( (void*)pUser->m_aJobSkill, sizeof(SKILL) *  ( MAX_SKILL_JOB ) );
	
	SEND( ar, this, DPID_SERVERPLAYER );	
}
#endif // __S_NEW_SKILL_2

#ifdef __S_RECOMMEND_EVE
void CDPDatabaseClient::SendRecommend( CUser* pUser, int nValue )
{
	BEFORESENDDUAL( ar, PACKETTYPE_EVE_RECOMMEND, DPID_UNKNOWN, DPID_UNKNOWN );  
	ar.WriteString( pUser->m_playAccount.lpszAccount );
	ar << pUser->m_idPlayer;
	ar << pUser->GetLevel();
	ar << pUser->GetSex();
	ar << nValue;
	SEND( ar, this, DPID_SERVERPLAYER );	
}
#endif // __S_RECOMMAED_EVE

void CDPDatabaseClient::SendITEM_TBL_Update()
{
	BEFORESENDDUAL( ar, PACKETTYPE_ITEM_TBL_UPDATE, DPID_UNKNOWN, DPID_UNKNOWN );
	int nQuestCount = 0;
	u_long uOffset	= ar.GetOffset();
	ar << nQuestCount;
	for( int i = 0; i < prj.m_aPropQuest.GetSize(); i++ )
	{
		QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( i );
		if( pQuestProp )
		{
			ar << i;
			ar.WriteString( pQuestProp->m_szTitle );
			nQuestCount++;
		}
	}
	int nBufSize1;
	LPBYTE lpBuf1	= ar.GetBuffer( &nBufSize1 );
	*(UNALIGNED int*)( lpBuf1 + uOffset )	= nQuestCount;

	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendPreventLogin( LPCTSTR szAccount, DWORD dwPreventTime )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PREVENT_LOGIN, DPID_UNKNOWN, DPID_UNKNOWN );
	ar.WriteString( szAccount );
	ar << dwPreventTime;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendLogConnect( CUser* pUser )
{
	BEFORESENDDUAL( ar, PACKETTYPE_LOG_PLAY_CONNECT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << pUser->GetWorld()->GetID();	// world id
	ar.Write( &pUser->m_playAccount.m_stLogin, sizeof(SYSTEMTIME) );
	ar.WriteString( pUser->m_playAccount.lpAddr );	// ip
	DWORD dwSeed = pUser->GetGold() + pUser->m_dwGoldBank[pUser->m_nSlot];
	ar << dwSeed;	// seed
	ar << pUser->m_idPlayer;
	ar.WriteString( pUser->m_playAccount.lpszAccount );
	ar << pUser->GetLevel();
	ar << pUser->GetJob();
#if __VER >= 14 // __PCBANG
	ar << CPCBang::GetInstance()->GetPCBangClass( pUser->m_idPlayer );
#endif // __PCBANG
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendInGuildCombat( u_long idGuild, DWORD dwPenya, DWORD dwExistingPenya )
{
	BEFORESENDDUAL( ar, PACKETTYPE_IN_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idGuild << dwPenya << dwExistingPenya;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendOutGuildCombat( u_long idGuild )
{
	BEFORESENDDUAL( ar, PACKETTYPE_OUT_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idGuild;
	SEND( ar, this, DPID_SERVERPLAYER );
}
void CDPDatabaseClient::SendGuildCombatStart( void )
{
	BEFORESENDDUAL( ar, PACKETTYPE_START_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_SERVERPLAYER );
}
void CDPDatabaseClient::SendGuildCombatResult( void )
{
	BEFORESENDDUAL( ar, PACKETTYPE_RESULT_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN );

	float fRequstCanclePercent = g_GuildCombatMng.m_nRequestCanclePercent / 100.0f;
	float fNotRequestPercent = g_GuildCombatMng.m_nNotRequestPercent / 100.0f;
	ar << g_GuildCombatMng.m_uWinGuildId;
	ar << g_GuildCombatMng.m_nWinGuildCount;
	ar << g_GuildCombatMng.m_uBestPlayer;
	
	// 길드 저장
#ifdef __S_BUG_GC
	ar << (u_long)g_GuildCombatMng.m_vecGuildCombatMem.size();
	for( int gcmi = 0 ; gcmi != g_GuildCombatMng.m_vecGuildCombatMem.size() ; ++gcmi )
	{
		CGuildCombat::__GuildCombatMember* pGCMember = g_GuildCombatMng.m_vecGuildCombatMem[gcmi];

		BOOL bSelectGuild = FALSE; // 출전한 길드인가
		for( int veci = 0 ; veci < (int)( g_GuildCombatMng.vecRequestRanking.size() ) ; ++veci )
		{
			if( veci >= g_GuildCombatMng.m_nMaxGuild )
				break;

			CGuildCombat::__REQUESTGUILD RequestGuild = g_GuildCombatMng.vecRequestRanking[veci];
			if( RequestGuild.uidGuild == pGCMember->uGuildId )
			{
				bSelectGuild = TRUE;
				break;
			}
		}

		ar << pGCMember->uGuildId;
		ar << pGCMember->nGuildPoint;
		if( bSelectGuild )	// 출전 길드
		{
			if( g_GuildCombatMng.m_uWinGuildId == pGCMember->uGuildId )	// 승리한 길드
			{
				ar << g_GuildCombatMng.GetPrizePenya( 0 );
				ar << (DWORD)0;
				ar << g_GuildCombatMng.m_nWinGuildCount;
			}
			else
			{
				ar << (__int64)0;
				ar << (DWORD)0;
				ar << (int)0;
			}
		}
		else
		{
			if( pGCMember->bRequest )	// 신청은 했는데 출전이 안된 길드 수수료 2%
			{
				ar << (__int64)0;
				ar << (DWORD)0; //(DWORD)(pGCMember->dwPenya * fNotRequestPercent );
				ar << (int)0;

				// 바로 우편함으로 지급
				CGuild* pGuild = g_GuildMng.GetGuild( pGCMember->uGuildId );
				if( pGuild )
				{
					CItemElem pItemElem;
					char szMsg[1000];
					sprintf( szMsg, "%s", prj.GetText( TID_GAME_GC_NOTREQUEST ) );
					char szMsg1[1000];
					sprintf( szMsg1, "%s", prj.GetText( TID_GAME_GC_NOTREQUEST1 ) );
					SendQueryPostMail( pGuild->m_idMaster, 0, pItemElem, (int)( pGCMember->dwPenya * fNotRequestPercent ), szMsg, szMsg1 );
				}
			}
			else						// 취소한 길드 수수료 20%
			{
				ar << (__int64)0;
				ar << (DWORD)0; //(DWORD)(pGCMember->dwPenya * fRequstCanclePercent );
				ar << (int)0;
			}
		}

		// 참가자 포인트
		ar << (u_long)pGCMember->vecGCSelectMember.size();
		for( int veci = 0 ; veci < (int)( pGCMember->vecGCSelectMember.size() ) ; ++veci )
		{
			CGuildCombat::__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];
			ar << pJoinPlayer->uidPlayer;
			ar << pJoinPlayer->nPoint;
			if( pJoinPlayer->uidPlayer == g_GuildCombatMng.m_uBestPlayer )
			{
				ar << g_GuildCombatMng.GetPrizePenya( 1 );
			}
			else 
			{
				ar << (__int64)0;
			}
		}
	}
#else // __S_BUG_GC
	ar << (u_long)g_GuildCombatMng.m_GuildCombatMem.size();
	for( map<u_long, CGuildCombat::__GuildCombatMember*>::iterator it = g_GuildCombatMng.m_GuildCombatMem.begin() ; 
		it != g_GuildCombatMng.m_GuildCombatMem.end() ; ++it )
	{
		BOOL bSelectGuild = FALSE; // 출전한 길드인가
		for( int veci = 0 ; veci < g_GuildCombatMng.vecRequestRanking.size() ; ++veci )
		{
			if( veci >= g_GuildCombatMng.m_nMaxGuild )
				break;

			CGuildCombat::__REQUESTGUILD RequestGuild = g_GuildCombatMng.vecRequestRanking[veci];
			if( RequestGuild.uidGuild == it->first )
			{
				bSelectGuild = TRUE;
				break;
			}
		}

		CGuildCombat::__GuildCombatMember* pGCMember = it->second;
		ar << (u_long)it->first;
		ar << pGCMember->nGuildPoint;
		if( bSelectGuild )	// 출전 길드
		{
			if( g_GuildCombatMng.m_uWinGuildId == it->first )	// 승리한 길드
			{
				ar << g_GuildCombatMng.GetPrizePenya( 0 );
				ar << (DWORD)0;
				ar << g_GuildCombatMng.m_nWinGuildCount;
			}
			else
			{
				ar << (__int64)0;
				ar << (DWORD)0;
				ar << (int)0;
			}
		}
		else
		{
			if( pGCMember->bRequest )	// 신청은 했는데 출전이 안된 길드 수수료 2%
			{
				ar << (__int64)0;
				ar << (DWORD)0; //(DWORD)(pGCMember->dwPenya * fNotRequestPercent );
				ar << (int)0;

				// 바로 우편함으로 지급
				CGuild* pGuild = g_GuildMng.GetGuild( it->first );
				if( pGuild )
				{
					CItemElem pItemElem;
					char szMsg[1000];
					sprintf( szMsg, "%s", prj.GetText( TID_GAME_GC_NOTREQUEST ) );
					char szMsg1[1000];
					sprintf( szMsg1, "%s", prj.GetText( TID_GAME_GC_NOTREQUEST1 ) );
					SendQueryPostMail( pGuild->m_idMaster, 0, pItemElem, pGCMember->dwPenya * fNotRequestPercent, szMsg, szMsg1 );
				}
			}
			else						// 취소한 길드 수수료 20%
			{
				ar << (__int64)0;
				ar << (DWORD)0; //(DWORD)(pGCMember->dwPenya * fRequstCanclePercent );
				ar << (int)0;
			}
		}

		// 참가자 포인트
		ar << (u_long)pGCMember->vecGCSelectMember.size();
		for( veci = 0 ; veci < pGCMember->vecGCSelectMember.size() ; ++veci )
		{
			CGuildCombat::__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];
			ar << pJoinPlayer->uidPlayer;
			ar << pJoinPlayer->nPoint;
			if( pJoinPlayer->uidPlayer == g_GuildCombatMng.m_uBestPlayer )
			{
				ar << g_GuildCombatMng.GetPrizePenya( 1 );
			}
			else 
			{
				ar << (__int64)0;
			}
		}
	}
#endif // __S_BUG_GC
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendGuildcombatContinue( int nGuildCombatID,  u_long uidGuild, int nWinCount )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CONTINUE_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nGuildCombatID;
	ar << uidGuild;
	ar << nWinCount;
	SEND( ar, this, DPID_SERVERPLAYER );
}
void CDPDatabaseClient::SendGCGetPenyaGuild( u_long uidPlayer, int nGuildCombatID, u_long uidGuild )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GETPENYAGUILD_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uidPlayer;
	ar << nGuildCombatID;
	ar << uidGuild;
	SEND( ar, this, DPID_SERVERPLAYER );
}
void CDPDatabaseClient::SendGCGetPenyaPlayer( u_long uidPlayer, int nGuildCombatID, u_long uidGuild )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GETPENYAPLAYER_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uidPlayer;
	ar << nGuildCombatID;
	ar << uidGuild;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnJoin( CAr & ar, DPID dpidCache, DPID dpidUser )
{
	DWORD dwWorldId, dwIndex;
	DWORD dwSavePlayerTime = 0;
	DWORD dwSaveFlyTime = 0;
	int nMaximumLevel = 0;

	char lpOutputString[128]	= { 0, };
	sprintf( lpOutputString, "WORLDSERVER.EXE\t// PACKETTYPE_JOIN\t// %d", g_uKey );
	OutputDebugString( lpOutputString );

	CUser* pUser = g_UserMng.GetUser( dpidCache, dpidUser );
	if( pUser )
	{

		DWORD dwAuthKey;
		ar >> dwAuthKey;
		if( pUser->m_dwAuthKey != dwAuthKey )
		{
			WriteLog( "OnJoin(): Different dwAuthKey" );
			return;		//
		}

		char lpAddrtmp[16];

#ifdef __ON_ERROR
		int nOnError	= 0;
		try
		{
#endif	// __ON_ERROR

		memcpy( lpAddrtmp, pUser->m_playAccount.lpAddr, 16 );
		ar >> pUser->m_playAccount;
		memcpy( pUser->m_playAccount.lpAddr, lpAddrtmp, 16 );

		ar >> dwWorldId;
#ifdef __LAYER_1015
		int nLayer;
		ar >> nLayer;
#if __VER < 14 // __INSTANCE_DUNGEON
		if( nLayer > 0 )
		{
			Error( "LAYER > 0" );
			return;
		}
#endif // __INSTANCE_DUNGEON
#endif // __LAYER_1015

		ar >> dwIndex;

#ifdef __ON_ERROR
		nOnError	= 1;
#endif	// __ON_ERROR

		pUser->SetIndex( NULL, dwIndex, FALSE );
		CObj::SetMethod( METHOD_NONE );
		pUser->Serialize( ar );
		pUser->InitMotion( pUser->m_dwMotion );
		pUser->m_nCheerPoint	= 0;
		pUser->m_dwTickCheer	= GetTickCount() + TICK_CHEERPOINT;
		
		pUser->SetStateNotMode( STATE_BASEMOTION_MODE, STATEMODE_BASEMOTION_CANCEL );	// 셑팅 빼기

#ifdef __3RD_LEGEND16
		if( pUser->GetLevel() > MAX_3RD_LEGEND_LEVEL )
#else // __3RD_LEGEND16
#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
		if( pUser->GetLevel() > MAX_LEGEND_LEVEL )
#else //__LEGEND	//	10차 전승시스템	Neuz, World, Trans
		if( pUser->GetLevel() > MAX_GENERAL_LEVEL )
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans
#endif // __3RD_LEGEND16
		{
			WriteError( "MAX_GENERAL_LEVEL//%s//%d", pUser->GetName(), pUser->GetLevel() );
			pUser->m_nLevel	= MAX_GENERAL_LEVEL;
		}

		pUser->RemoveBuff( BUFF_ITEM, II_CHEERUP );

		if( !pUser->GetPet() )
			pUser->SetPetId( NULL_ID );

		pUser->m_pActMover->RemoveStateFlag( OBJSTAF_ACC );			// 비행은 멈춤상태  		
		pUser->SetAngle( 0.0f );	// 각도 초기화, 임시 코드이므로 원인을 분석하여 수정하도록 하자.

		// 이상타 땜방 dwWorldId가 200이면서 못 들어가는 유저들이 생김 
		CWorld* pWorld = g_WorldMng.GetWorld( dwWorldId );
		if( pWorld && pWorld->VecInWorld( pUser->GetPos() ) == FALSE )
		{
			dwWorldId = 1;
			pUser->SetPos( D3DXVECTOR3( 6971.984f, 100.0f, 3336.884f ) );
		}

		pUser->AdjustGuildQuest( dwWorldId );
		pUser->m_nDuel	= pUser->m_idDuelParty = 0;

#ifdef __ON_ERROR
		nOnError	= 2;
#endif	// __ON_ERROR

		pUser->m_playTaskBar.Serialize( ar );

#ifdef __ON_ERROR
		nOnError	= 3; 
#endif	// __ON_ERROR

#ifdef __RT_1025
		pUser->m_RTMessenger.Serialize( ar );
#else	// __RT_1025
		pUser->m_Messenger.Serialize( ar );
#endif	// __RT_1025

#ifdef __ON_ERROR
		nOnError	= 4;
#endif	// __ON_ERROR
		ar.ReadString( pUser->m_szPartyName, 32 );
		ar.ReadString( pUser->m_szBankPass, 5 );
		ar >> dwSavePlayerTime;
		pUser->SetPlayerTime( dwSavePlayerTime );

#ifndef __JEFF_9_20
		ar >> dwSaveFlyTime;
		pUser->SetSaveFlyTime( dwSaveFlyTime );
#endif	// __JEFF_9_20

		ar >> nMaximumLevel;

		// 쪽지를 읽어들인다.
		short nTagCount;
		ar >> nTagCount;

		TAG_ENTRY tags[MAX_TAGS];
		for( short i=0; i<nTagCount; ++i )
		{
			ar >> tags[i].idFrom;					// 누구에게서 
			ar >> tags[i].dwDate;					// 언제 
			ar.ReadString( tags[i].szString, 256 );		// 쪽지 내용 
		}

		char GuildMemberTime[15] = {0,};
		ar.ReadString( GuildMemberTime, 15 );

		char cYear[5] = {0,};
		char cMonth[3] = {0,};
		char cDay[3] = {0,};
		char cHour[3] = {0,};
		char cMin[3] = {0,};
		
		strncpy(cYear, GuildMemberTime, 4);
		strncpy(cMonth, GuildMemberTime + 4, 2 );
		strncpy(cDay, GuildMemberTime + 6, 2 );
		strncpy(cHour, GuildMemberTime + 8, 2 );
		strncpy(cMin, GuildMemberTime + 10, 2 );
		
		CTime tGuildMember( atoi(cYear), atoi(cMonth), atoi(cDay), atoi(cHour), atoi(cMin), 0 );
		pUser->m_tGuildMember = tGuildMember;

		ar >> pUser->m_dwReturnWorldID;
		ar >> pUser->m_vReturnPos;

#ifdef __EVENT_1101
#ifdef __EVENT_1101_2
		ar >> pUser->m_nEventFlag;
#else // __EVENT_1101_2
		ar >> pUser->m_dwEventFlag;
#endif // __EVENT_1101_2
		ar >> pUser->m_dwEventTime;
		ar >> pUser->m_dwEventElapsed;
#endif	// __EVENT_1101

#ifdef __EXP_ANGELEXP_LOG
		ar >> pUser->m_nExpLog;
		ar >> pUser->m_nAngelExpLog;
#endif // __EXP_ANGELEXP_LOG

#ifdef __EVENTLUA_COUPON
		ar >> pUser->m_nCoupon;
#endif // __EVENTLUA_COUPON
#if __VER >= 14 // __PCBANG
		DWORD dwPCBangClass;
		ar >> dwPCBangClass;
#endif // __PCBANG

#if __VER >= 15 // __GUILD_HOUSE
		int nRestPoint; time_t tLogOut;
		ar >> nRestPoint >> tLogOut;
		if( pUser->m_idGuild != 0 )
			pUser->SetRestPoint( nRestPoint );
#endif // __GUILD_HOUSE

#ifdef __ON_ERROR
		nOnError	= 5;
#endif	// __ON_ERROR

		pUser->m_pActMover->ClearState();
		pUser->RedoEquip( FALSE );
		
		if( pUser->m_nHitPoint == 0 )
			pUser->m_nHitPoint	= pUser->GetMaxHitPoint();

		pUser->SetHitPoint( pUser->GetHitPoint() );			// max넘은 경우를 대비한다.
		pUser->SetManaPoint( pUser->GetManaPoint() );		// max넘은 경우를 대비한다.
		pUser->SetFatiguePoint( pUser->GetFatiguePoint() ); // max넘은 경우를 대비한다.

		pUser->SetId( NULL_ID );
#if __VER >= 13 // __HONORABLE_TITLE			// 달인
		pUser->CheckHonorStat();
#endif	// __HONORABLE_TITLE			// 달인

		if( prj.GetUserByID( pUser->m_idPlayer ) )
		{
			WriteLog( "OnJoin(): an ocular spectrum %d", pUser->m_idPlayer );
			pUser->m_idPlayer	= (u_long)0xffffffff;
/*
			1. 이 경우는 현재 발생하지 않는다. 그러나 만약을 대비하여 추적 로그를 남기도록 한다.
*/
			return;
		}

#ifdef __ON_ERROR
		nOnError	= 6;
#endif	// __ON_ERROR

		pUser->ValidateItem();
		pUser->m_nGuildCombatState	= 0;

#ifdef __ON_ERROR
		nOnError	= 700;
#endif	// __ON_ERROR
/*
		if( ::GetLanguage() == LANG_ENG && ::GetSubLanguage() == LANG_SUB_USA )
		{
			CString strIp	= pUser->m_playAccount.lpAddr;
			if( strIp.Find( "116.125.62.148" ) < 0 && strIp.Find( "64.71.27.34" ) < 0 )
				pUser->m_dwAuthorization	= AUTH_GENERAL;
		}
		else if( ::GetLanguage() == LANG_VTN )
		{
			CString strIp	= pUser->m_playAccount.lpAddr;
			if( strIp.Find( "116.125.62.148" ) < 0 && strIp.Find( "222.252.24.47" ) < 0 && strIp.Find( "222.255.38.14" ) < 0 )
				pUser->m_dwAuthorization	= AUTH_GENERAL;
		}
		//	mulcom	BEGIN100302	러시아 GM 권한 특정 IP로 제한 설정
		else if( ::GetLanguage() == LANG_RUS )
		{
			CString strIp	= pUser->m_playAccount.lpAddr;
			if( strIp.Find( "116.125.62.148" ) < 0 && strIp.Find( "86.62.94.50" ) < 0 )
			{
				pUser->m_dwAuthorization	= AUTH_GENERAL;
			}
		}
		//	mulcom	END100302	러시아 GM 권한 특정 IP로 제한 설정
		//	mulcom	BEGIN100304	대만 GM 권한 특정 IP로 제한 설정
		else if( ::GetLanguage() == LANG_TWN )
		{
			CString strIp	= pUser->m_playAccount.lpAddr;
			if( strIp.Find( "116.125.62.148" ) < 0 && strIp.Find( "60.251.100.51" ) < 0 && strIp.Find( "60.251.100.52" ) < 0 )
			{
				pUser->m_dwAuthorization	= AUTH_GENERAL;
			}
		}
		//	mulcom	END100304	대만 GM 권한 특정 IP로 제한 설정
		//	mulcom	BEGIN100305	일본 GM 권한 특정 IP로 제한 설정
		else if( ::GetLanguage() == LANG_JAP )
		{
			CString strIp	= pUser->m_playAccount.lpAddr;
			if( strIp.Find( "116.125.62.148" ) < 0 && strIp.Find( "219.111.2.70" ) < 0
				&& strIp.Find( "180.235.103." ) < 0 && strIp.Find( "192.168.160.212" ) < 0
				&& strIp.Find( "192.168.160.175" ) < 0 && strIp.Find( "192.168.160.249" ) < 0
				&& strIp.Find( "10.73.1.134" ) < 0 && strIp.Find( "10.73.1.68" ) < 0 
				&& strIp.Find( "122.211.251.254" ) < 0 && strIp.Find( "210.148.99.30" ) < 0
				&& strIp.Find( "210.148.99.131" ) < 0 )
			{
				pUser->m_dwAuthorization	= AUTH_GENERAL;
			}
		}
		//	mulcom	END100305	일본 GM 권한 특정 IP로 제한 설정
		//	mulcom	BEGIN100309	유럽(독일/프랑스) GM 권한 특정 IP로 제한 설정
		else if( ::GetLanguage() == LANG_GER || ::GetLanguage() == LANG_FRE )
		{
			CString strIp	= pUser->m_playAccount.lpAddr;
			if( strIp.Find( "116.125.62.148" ) < 0 && strIp.Find( "84.203.140.3" ) < 0 )
			{
				pUser->m_dwAuthorization	= AUTH_GENERAL;
			}
		}
		//	mulcom	END100309	유럽(독일/프랑스) GM 권한 특정 IP로 제한 설정
		//	mulcom	BEGIN100309	태국 GM 권한 특정 IP로 제한 설정
		else if( ::GetLanguage() == LANG_THA )
		{
			CString strIp	= pUser->m_playAccount.lpAddr;
			if( strIp.Find( "116.125.62.148" ) < 0 && strIp.Find( "203.107.158.41" ) < 0
				&& strIp.Find( "203.107.158.42" ) < 0 && strIp.Find( "203.107.158.43" ) < 0
				&& strIp.Find( "203.107.158.44" ) < 0 && strIp.Find( "203.107.158.45" ) < 0
				&& strIp.Find( "203.107.158.46" ) < 0 && strIp.Find( "58.64.24.253" ) < 0 )
			{
				pUser->m_dwAuthorization	= AUTH_GENERAL;
			}
		}
		//	mulcom	END100309	태국 GM 권한 특정 IP로 제한 설정
*/

#ifdef __INTERNALSERVER
//		pUser->m_dwAuthorization = AUTH_ADMINISTRATOR;
#endif // __INTERNALSERVER

		pUser->RemoveBuff( BUFF_ITEM, II_SYS_SYS_SCR_PARTYSUMMON );

#ifdef __ON_ERROR
		nOnError	= 701;
#endif	// __ON_ERROR

#ifdef __PKSERVER_USE_ANGEL
		if( !g_eLocal.GetState( EVE_PK ) || pUser->GetPKValue() == 0 )
			pUser->RemoveIk3Buffs( IK3_ANGEL_BUFF );
#endif // __PKSERVER_USE_ANGEL

#ifdef __ON_ERROR
		nOnError	= 702;
#endif	// __ON_ERROR

		pUser->AdjustMailboxState();

#ifdef __ON_ERROR
		nOnError	= 703;
#endif	// __ON_ERROR

#ifdef __LAYER_1015
#if __VER >= 14 // __INSTANCE_DUNGEON
		if( 
#if __VER >= 15 // __GUILD_HOUSE
			GuildHouseMng->IsLoginAble( pUser, dwWorldId, nLayer ) &&
#endif // __GUILD_HOUSE
			pWorld && pWorld->m_linkMap.GetLinkMap( nLayer )							// 해당 레이어가 존재하고
			&& !CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( dwWorldId )	// 인스턴스 던전이 아닌 경우...
#ifdef __QUIZ
			&& dwWorldId != WI_WORLD_QUIZ												// 퀴즈이벤트 지역이 아닌 경우
#endif // __QUIZ
			)
		{
			g_UserMng.AddPlayer( pUser, dwWorldId, nLayer );
		}
		else	// 레이어가 존재하지 않으면 무조건 마을로 이동한다.
		{
			pUser->m_vPos = D3DXVECTOR3( 6971.984f, 100.0f, 3336.884f );
			g_UserMng.AddPlayer( pUser, WI_WORLD_MADRIGAL, nDefaultLayer );
			pUser->REPLACE( g_uIdofMulti, WI_WORLD_MADRIGAL, D3DXVECTOR3( 6971.984f, 99.8f, 3336.884f ), REPLACE_NORMAL, nDefaultLayer );
		}
#else // __INSTANCE_DUNGEON
		g_UserMng.AddPlayer( pUser, dwWorldId, nLayer );
#endif // __INSTANCE_DUNGEON
#else	// __LAYER_1015
		g_UserMng.AddPlayer( pUser, dwWorldId );
#endif	// __LAYER_1015

		if( !pUser->IsAuthHigher( AUTH_GAMEMASTER ) )
		{
			// 무적, 투명, 초필, 반무적 
			if( pUser->m_dwMode & ( MATCHLESS_MODE | TRANSPARENT_MODE | ONEKILL_MODE | MATCHLESS2_MODE ) )
			{
				Error( "Abnormal User!! - %s(%07d), m_dwMode:%08x", pUser->GetName(), pUser->m_idPlayer, pUser->m_dwMode );
				pUser->SetNotMode( MATCHLESS_MODE | TRANSPARENT_MODE | ONEKILL_MODE | MATCHLESS2_MODE );
			}
		}

#ifdef __ON_ERROR
		nOnError	= 704;
#endif	// __ON_ERROR

		pUser->AddEventLuaDesc();

#ifdef __ON_ERROR
		nOnError	= 705;
#endif	// __ON_ERROR

		pUser->AdjustPartyQuest( dwWorldId );

#ifdef __ON_ERROR
		nOnError	= 706;
#endif	// __ON_ERROR

		pUser->AddTag( nTagCount, tags );	// 클라이언트에게 쪽지를 보낸다.

#ifdef __ON_ERROR
		nOnError	= 8;
#endif	// __ON_ERROR

		g_DPCoreClient.SendJoin( pUser->m_idPlayer, pUser->GetName(), ( pUser->m_dwAuthorization >= AUTH_GAMEMASTER ) );
		
		if( pUser->m_idGuild != 0 )
		{
			CGuild* pGuild	= g_GuildMng.GetGuild( pUser->m_idGuild );
			if( !pGuild || !pGuild->IsMember( pUser->m_idPlayer ) )
				pUser->m_idGuild	= 0;
		}

#ifdef __ON_ERROR
		nOnError	= 9;
#endif	// __ON_ERROR

		CParty* pParty	= g_PartyMng.GetParty( pUser->m_idparty );
		if( pParty && pParty->IsMember( pUser->m_idPlayer ) )
		{
#if __VER >= 11 // __SYS_PLAYER_DATA
			const char* pszLeader	= CPlayerDataCenter::GetInstance()->GetPlayerString( pParty->GetPlayerId( 0 ) );
			pUser->AddPartyMember( pParty, 0, ( pszLeader? pszLeader: "" ), "" );
#else	// __SYS_PLAYER_DATA
			pUser->AddPartyMember( pParty );
#endif	// __SYS_PLAYER_DATA
			if( pParty->m_idDuelParty )		// 듀얼중인 파티였다면.
			{
				pUser->m_nDuel = 2;			// 듀얼상태를 세팅
				pUser->m_idDuelParty = pParty->m_idDuelParty;
			}
		}
		else
		{
			pUser->m_idparty	= 0;
		}

#ifdef __ON_ERROR
		nOnError	= 10;
#endif	// __ON_ERROR

		pUser->AddSMModeAll();

		pUser->AddGameSetting();
#ifdef __S1108_BACK_END_SYSTEM
		pUser->AddMonsterProp();
#endif // __S1108_BACK_END_SYSTEM
		
#ifdef __S_SERVER_UNIFY
		// 서버 통합 작업
		pUser->SetAllAction();
#endif // __S_SERVER_UNIFY

#ifdef __ON_ERROR
		nOnError	= 11;
#endif	// __ON_ERROR

		pUser->AddGCIsRequest( g_GuildCombatMng.IsRequestWarGuild( pUser->m_idGuild, TRUE ) );
		pUser->AddGuildCombatState();
		if( g_GuildCombatMng.GuildCombatProcess[g_GuildCombatMng.m_nProcessGo - 1].dwParam == CGuildCombat::MAINTENANCE_STATE )
			pUser->AddGuildCombatEnterTime( g_GuildCombatMng.m_dwTime - GetTickCount() );
		else
			pUser->AddGuildCombatNextTime( g_GuildCombatMng.m_dwTime - GetTickCount(), g_GuildCombatMng.GuildCombatProcess[g_GuildCombatMng.m_nProcessGo - 1].dwParam );
		pUser->AddGCWinGuild();
		pUser->AddGCBestPlayer();
		//pUser->AddGCIsRequest( g_GuildCombatMng.IsRequestWarGuild( pUser->m_idGuild, TRUE ) );
		if( g_GuildCombatMng.m_nState != CGuildCombat::CLOSE_STATE )
		{
			g_UserMng.AddGCGuildStatus( pUser->m_idGuild, pUser );
			g_UserMng.AddGCWarPlayerlist( pUser->m_idGuild, pUser );
		}
#ifdef __ON_ERROR
		nOnError	= 12;
#endif	// __ON_ERROR

#ifdef __INVALID_LOGIN_0320
		CalluspXXXMultiServer( g_uIdofMulti, pUser );
#else	// __INVALID_LOGIN_0320
		CalluspXXXMultiServer( g_uIdofMulti, pUser->m_idPlayer );
#endif	// __INVALID_LOGIN_0320

#ifdef __ON_ERROR
		nOnError	= 13;
#endif	// __ON_ERROR

		pWorld = pUser->GetWorld();
#if __VER >= 11 // __GUILD_COMBAT_1TO1
		g_GuildCombat1to1Mng.SendNowState( pUser );
		if( g_eLocal.GetState( EVE_GUILDCOMBAT1TO1 ) )
		{
			if( pWorld  && pWorld->GetID() >= WI_WORLD_GUILDWAR1TO1_0
						&& pWorld->GetID() <= WI_WORLD_GUILDWAR1TO1_L )
			{
				pUser->REPLACE( g_uIdofMulti, WI_WORLD_MADRIGAL, D3DXVECTOR3( 6983.0f, 0.0f, 3330.0f ), REPLACE_NORMAL, nDefaultLayer );
			}
		}

// 		if( !pUser->GetWorld() )
// 			pUser->Replace( g_uIdofMulti, WI_WORLD_MADRIGAL, D3DXVECTOR3( 6983.0f, 0.0f, 3330.0f ) );
#endif // __GUILD_COMBAT_1TO1

#if __VER >= 11 // __SYS_COLLECTING
	if( pUser->IsCollecting() )
		pUser->StopCollecting();
#endif	// __SYS_COLLECTING

#ifdef __ON_ERROR
		nOnError	= 14;
#endif	// __ON_ERROR

#if __VER >= 12 // __SECRET_ROOM
		CSecretRoomMng::GetInstance()->GetAllInfo( pUser );
		if( CSecretRoomMng::GetInstance()->IsInTheSecretRoom( pUser ) )
			pUser->REPLACE( g_uIdofMulti, WI_WORLD_MADRIGAL, CContinent::GetInstance()->GetRevivalPos( CONT_FLARIS ), REPLACE_NORMAL, nDefaultLayer );

		if( pWorld->GetID() == WI_DUNGEON_SECRET_0 )
			pUser->REPLACE( g_uIdofMulti, WI_WORLD_MADRIGAL, CContinent::GetInstance()->GetRevivalPos( CONT_DARKON12 ), REPLACE_NORMAL, nDefaultLayer );
#endif // __SECRET_ROOM

#ifdef __LAYER_1015
#if __VER < 14 // __INSTANCE_DUNGEON
		if( nLayer > 0 )
		{
			pUser->REPLACE( g_uIdofMulti, WI_WORLD_MADRIGAL, D3DXVECTOR3( 6983.0f, 0.0f, 3330.0f ), REPLACE_FORCE, nDefaultLayer );
		}
		else
		{
			if( !pWorld || !pWorld->m_linkMap.GetLinkMap( nLayer ) )
			{
				Error( "LAYER NOT FOUND" );
				pUser->REPLACE( g_uIdofMulti, WI_WORLD_MADRIGAL, D3DXVECTOR3( 6983.0f, 0.0f, 3330.0f ), REPLACE_FORCE, nDefaultLayer );
			}
		}
#endif // __INSTANCE_DUNGEON
#endif	// __LAYER_1015

#ifdef __ON_ERROR
		nOnError	= 15;
#endif	// __ON_ERROR

#if __VER >= 12 // __TAX
		pUser->AddTaxInfo();
		CTax::GetInstance()->SendNoSetTaxRateOpenWnd( pUser );
#endif // __TAX

#ifdef __ON_ERROR
		nOnError	= 16;
#endif	// __ON_ERROR

#if __VER >= 12 // __PET_0519
		{
			// 게임 진입 시 시스템 펫 각성 효과 적용
			CItemElem* pItem	= pUser->GetPetItem();
			if( pItem )
				pUser->SetDestParamRandomOptExtension( pItem );
		}
#endif	// __PET_0519


#ifdef __ON_ERROR
		nOnError	= 17;
#endif	// __ON_ERROR

#if __VER >= 13 // __RAINBOW_RACE
		// 완주하지 않고 접속을 끊은 놈은 탈락이므로 완주한 놈만 정보를 받게된다. chipi_090317
		if( CRainbowRaceMng::GetInstance()->GetState() == CRainbowRaceMng::RR_PROGRESS )
		{
			CRainbowRace* pRainbowRace = CRainbowRaceMng::GetInstance()->GetRainbowRacerPtr( pUser->m_idPlayer );
			if( pRainbowRace )
				pUser->AddRainbowRaceState( pRainbowRace );
		}
#endif // __RAINBOW_RACE

#if __VER >= 13 // __HOUSING
		CHousingMng::GetInstance()->CreateRoomLayer( pUser->m_idPlayer );
		CHousingMng::GetInstance()->ReqLoadHousingInfo( pUser->m_idPlayer );
#endif // __HOUSING

#if __VER >= 14 // __PCBANG
		CPCBang::GetInstance()->SetPCBangPlayer( pUser, dwPCBangClass );
#endif // __PCBANG

#ifdef __VTN_TIMELIMIT
		//	mulcom	BEGIN100315	베트남 시간 제한
 		if( ::GetLanguage() == LANG_VTN )
		{
			SendTimeLimitInfoReq( pUser->m_idPlayer, pUser->m_playAccount.lpszAccount );
		}
		//	mulcom	END100315	베트남 시간 제한
#endif // __VTN_TIMELIMIT

#if __VER >= 14 // __INSTANCE_DUNGEON
		if( CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( dwWorldId ) )
		{
			// 인던 안에서 접속 종료 후 재접속 시 플라리스로 가지 않고 인던 출구로 강제 이동
			CInstanceDungeonHelper::GetInstance()->GoOut( pUser );
		}
#endif // __INSTANCE_DUNGEON
#if __VER >= 15 // __GUILD_HOUSE
		pUser->AddGuildHouseAllInfo( GuildHouseMng->GetGuildHouse( pUser->m_idGuild ) );
		GuildHouseMng->SetApplyDST( pUser );
		if( pWorld && GuildHouseMng->IsGuildHouse( pWorld->GetID() ) && pUser->GetLayer() == pUser->m_idGuild )	// 접속지가 길드하우스 이면..
			pUser->SetIncRestPoint( tLogOut / REST_POINT_TICK * REST_POINT_LOGOUT_INC );	// 로그아웃 휴식 포인트를 증가 시켜준다.
#endif // __GUILD_HOUSE

#if __VER >= 15 // __CAMPUS
		u_long idCampus = pUser->GetCampusId();
		if( idCampus )
		{
			CCampus* pCampus = CCampusHelper::GetInstance()->GetCampus( idCampus );
			if( pCampus && pCampus->IsMember( pUser->m_idPlayer ) )
				pUser->AddUpdateCampus( pCampus );
			else
				pUser->SetCampusId( 0 );
		}
#endif // __CAMPUS

#ifdef __ON_ERROR
		}
		catch( ... )
		{
			WriteLog( "OnJoin(): %s, %d", pUser->GetName(), nOnError );
		}
#endif	// __ON_ERROR
	}
}

void CDPDatabaseClient::OnGCResultValue( CAr & ar )
{
	u_long uSize;
	g_GuildCombatMng.m_GCResultValueGuild.clear();
	ar >> uSize;
	
	int	i = 0;
	
	for( i = 0 ; i < (int)( uSize ) ; ++i )
	{
		CGuildCombat::__GCRESULTVALUEGUILD ResultValueGuild;
		ar >> ResultValueGuild.nCombatID;
		ar >> ResultValueGuild.uidGuild;
		ar >> ResultValueGuild.nReturnCombatFee;
		ar >> ResultValueGuild.nReward;
		g_GuildCombatMng.m_GCResultValueGuild.push_back( ResultValueGuild );
	}
	
	// 베스트 플레이어 관련 정보
	g_GuildCombatMng.m_GCResultValuePlayer.clear();
	ar >> uSize;
	for( i = 0 ; i < (int)( uSize ) ; ++i )
	{
		CGuildCombat::__GCRESULTVALUEPLAYER ResultValuePlayer;
		ar >> ResultValuePlayer.nCombatID;
		ar >> ResultValuePlayer.uidGuild;
		ar >> ResultValuePlayer.uidPlayer;
		ar >> ResultValuePlayer.nReward;
		g_GuildCombatMng.m_GCResultValuePlayer.push_back( ResultValuePlayer );
	}
}
void CDPDatabaseClient::OnGCPlayerPoint( CAr & ar )
{
	g_GuildCombatMng.m_vecGCPlayerPoint.clear();
	// 길드대전 개인 포인트
	u_long uSize;
	ar >> uSize;
	for( int i = 0 ; i < (int)( uSize ) ; ++i )
	{
		CGuildCombat::__GCPLAYERPOINT GCPlayerPoint;
		ar >> GCPlayerPoint.uidPlayer;
		ar >> GCPlayerPoint.nJob;
		ar >> GCPlayerPoint.nPoint;
		g_GuildCombatMng.m_vecGCPlayerPoint.push_back( GCPlayerPoint );
	}
}
void CDPDatabaseClient::OnAllGuildCombat( CAr & ar, DPID, DPID )
{
	g_GuildCombatMng.GuildCombatClear(1);
	u_long uSize, idGuild;
	DWORD dwPenya;
	BOOL bRequest;
	ar >> g_GuildCombatMng.m_nGuildCombatIndex;
	ar >> g_GuildCombatMng.m_uWinGuildId;
	ar >> g_GuildCombatMng.m_nWinGuildCount;
	ar >> g_GuildCombatMng.m_uBestPlayer;

	// 참가한 인원수
	ar >> uSize;
	for( u_long i = 0 ; i < uSize ; ++i )
	{
		ar >> idGuild;
		ar >> dwPenya;
		ar >> bRequest;
		
		CGuild* pGuild = g_GuildMng.GetGuild( idGuild );
		if( pGuild )
		{
			g_GuildCombatMng.JoinGuildCombat( idGuild, dwPenya, bRequest );
		}
	}

	// 길드 보상 관련 정보
	OnGCResultValue( ar );

	// 길드대전 플레이어 포인트
	OnGCPlayerPoint( ar );
	
	g_GuildCombatMng.SetNpc();

}
// 참가 신청
void CDPDatabaseClient::OnInGuildCombat( CAr & ar, DPID, DPID )
{
	u_long idGuild;
	DWORD dwPenya, dwExistingPenya;
	ar >> idGuild;
	ar >> dwPenya;
	ar >> dwExistingPenya;
	
	CGuild* pGuild = g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		g_GuildCombatMng.JoinGuildCombat( idGuild, dwPenya, TRUE );
		CUser* pMaster = (CUser*)prj.GetUserByID( pGuild->m_idMaster );
		if( IsValidObj( pMaster ) )
		{
			CString str;

			// 전체 페냐가 0일경우
			if( dwExistingPenya == 0 )
			{
				str.Format( prj.GetText(TID_GAME_GUILDCOMBAT_APP_CONFIRM), 0, dwPenya );
			}
			else
			{
				// 전체 금액이 있으면 추가되는 페냐를 빼서 계산..
				str.Format( prj.GetText(TID_GAME_GUILDCOMBAT_APP_CONFIRM), dwExistingPenya, dwPenya-dwExistingPenya  );
			}

			pMaster->AddDiagText( str );
			g_UserMng.AddGCIsRequest( idGuild, g_GuildCombatMng.IsRequestWarGuild( idGuild, TRUE ) );
		}

		CString strMsg;
		// 길드대전 신청이 완료되었습니다.
		strMsg.Format( prj.GetText( TID_GAME_GUILDCOMBAT_REQUEST_GUILDCOMBAT ) );	// "수정해야함 : 길드대전에 신청하였습니다. 매주 토요일 17시에 길드대전이 시작됩니다 로 수정할까?? ㅎㅎ " );
		g_UserMng.AddGuildMsg( pGuild, strMsg );
		strMsg.Format( prj.GetText(TID_GAME_GUILDCOMBAT_APP) );
		g_UserMng.AddGuildMsg( pGuild, strMsg );
	}
}

void CDPDatabaseClient::OnOutGuildCombat( CAr & ar, DPID, DPID )
{
	u_long idGuild;
	ar >> idGuild;

	CGuild* pGuild = g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		BOOL bJoin = TRUE;
#ifdef __S_BUG_GC
		CGuildCombat::__GuildCombatMember* pGCMember = g_GuildCombatMng.FindGuildCombatMember( idGuild );
		if( pGCMember == NULL )
		{
			bJoin = FALSE;
		}
		else
		{
			if( pGCMember->bRequest == FALSE )
				bJoin = FALSE;
		}
#else // __S_BUG_GC
		map<u_long, CGuildCombat::__GuildCombatMember*>::iterator it = g_GuildCombatMng.m_GuildCombatMem.find( idGuild );
		if( it == g_GuildCombatMng.m_GuildCombatMem.end() )
		{
			bJoin = FALSE;
		}
		else
		{
			CGuildCombat::__GuildCombatMember* pGCMember = it->second;
			if( pGCMember->bRequest == FALSE )
				bJoin = FALSE;				
		}
#endif // __S_BUG_GC
		if( bJoin )
		{
			g_GuildCombatMng.OutGuildCombat( idGuild );
			CUser* pMaster = (CUser*)prj.GetUserByID( pGuild->m_idMaster );
			if( IsValidObj( pMaster ) )
			{
				CString strMsg;
				strMsg.Format( prj.GetText( TID_GAME_GUILDCOMBAT_EXPENSE_RETURN ) );	// "수정해야함 : 대전 참가비는 대전이 끝나고 반환됩니다" );
				pMaster->AddText( strMsg );
			}
			
			CString strMsg;
			//신청하신 길드대전을 취소했습니다.
			strMsg.Format( prj.GetText(TID_GAME_GUILDCOMBAT_CANCEL) );
			g_UserMng.AddGuildMsg( pGuild, strMsg );
			g_UserMng.AddGCIsRequest( idGuild, g_GuildCombatMng.IsRequestWarGuild( idGuild, TRUE ) );
		}
	}
}

void CDPDatabaseClient::OnResultGuildCombat( CAr & ar, DPID, DPID )
{
	ar >> g_GuildCombatMng.m_nGuildCombatIndex;
	ar >> g_GuildCombatMng.m_uWinGuildId;
	ar >> g_GuildCombatMng.m_nWinGuildCount;
	ar >> g_GuildCombatMng.m_uBestPlayer;

	OnGCResultValue( ar );
	g_GuildCombatMng.GuildCombatGameClear();

	g_UserMng.AddGCWinGuild();
	g_UserMng.AddGCBestPlayer();
}
void CDPDatabaseClient::OnPlayerPointGuildCombat( CAr & ar, DPID, DPID )
{
	OnGCPlayerPoint( ar );
}
void CDPDatabaseClient::OnGetPenyaGuildGC( CAr & ar, DPID, DPID )
{
	u_long uidPlayer, uidGuild;
	int nGuildCombatId;
	ar >> uidPlayer;
	ar >> nGuildCombatId;
	ar >> uidGuild;
	CMover* pMover = prj.GetUserByID( uidPlayer );
	if( IsValidObj( pMover ) )
	{
		if( uidGuild == pMover->m_idGuild )
		{
			int nGetResult = 0;
			__int64 nGetPenya = 0;

			BOOL bFind = FALSE;
			int nFindVeci = 0;
			CGuildCombat::__GCRESULTVALUEGUILD ResultValuetmp;
			for( int veci = 0 ; veci < (int)( g_GuildCombatMng.m_GCResultValueGuild.size() ) ; ++veci )
			{
				ResultValuetmp = g_GuildCombatMng.m_GCResultValueGuild[veci];
				if( nGuildCombatId == ResultValuetmp.nCombatID && uidGuild == ResultValuetmp.uidGuild )
				{
					bFind = TRUE;
					nFindVeci = veci;
					if( ResultValuetmp.nReturnCombatFee )
						nGetResult = 1;
					else
						nGetResult = 0;
					nGetPenya = ResultValuetmp.nReturnCombatFee + ResultValuetmp.nReward;
					break;
				}
			}

#if __VER >= 8 // __GUILDCOMBAT_85
			if( bFind && nGetResult )
#else // __VER >= 8
			if( bFind )
#endif // __VER >= 8
			{				
				__int64 nTotal = (__int64)pMover->GetGold() + nGetPenya;
				if( nGetPenya > INT_MAX || nTotal > INT_MAX )
				{
					((CUser*)pMover)->AddGCGetPenyaGuild( 3, nGetPenya );
					bFind = FALSE;
				}

				pMover->AddGold( (int)( nGetPenya ) );
				((CUser*)pMover)->AddGCGetPenyaGuild( nGetResult, nGetPenya );

				LogItemInfo aLogItem;
				aLogItem.Action		= "9";
				aLogItem.SendName	= pMover->GetName();
				aLogItem.RecvName	= "GUILDCOMBAT_VG";
				aLogItem.WorldId	= pMover->GetWorld()->GetID();
				aLogItem.Gold		= (DWORD)( pMover->GetGold() - nGetPenya );
				aLogItem.Gold2		= pMover->GetGold();
				//aLogItem.ItemName	= "SEED";
				_stprintf( aLogItem.szItemName, "%d", II_GOLD_SEED1 );
				aLogItem.itemNumber	= (DWORD)( nGetPenya );
				g_DPSrvr.OnLogItem( aLogItem );

				vector<CGuildCombat::__GCRESULTVALUEGUILD>::iterator ita = &(g_GuildCombatMng.m_GCResultValueGuild[nFindVeci]);
				g_GuildCombatMng.m_GCResultValueGuild.erase( ita );		
			}
		}
	}
}
void CDPDatabaseClient::OnGetPenyaPlayerGC( CAr & ar, DPID, DPID )
{
	u_long uidPlayer, uidGuild;
	int nGuildCombatId;
	ar >> uidPlayer;
	ar >> nGuildCombatId;
	ar >> uidGuild;
	CMover* pMover = prj.GetUserByID( uidPlayer );
	if( IsValidObj( pMover ) )
	{
		int nGetResult = 0;
		__int64 nGetPenya = 0;
		
		BOOL bFind = FALSE;
		int nFindVeci = 0;
		CGuildCombat::__GCRESULTVALUEPLAYER ResultValuetmp;
		for( int veci = 0 ; veci < (int)( g_GuildCombatMng.m_GCResultValuePlayer.size() ) ; ++veci )
		{
			ResultValuetmp = g_GuildCombatMng.m_GCResultValuePlayer[veci];
			if( nGuildCombatId == ResultValuetmp.nCombatID && uidPlayer == ResultValuetmp.uidPlayer && uidGuild == ResultValuetmp.uidGuild )
			{
				bFind = TRUE;
				nFindVeci = veci;
				nGetPenya = ResultValuetmp.nReward;
				break;
			}
		}
		
		if( bFind )
		{				
			__int64 nTotal = (__int64)pMover->GetGold() + nGetPenya;
			if( nGetPenya > INT_MAX || nTotal > INT_MAX )
			{
				((CUser*)pMover)->AddGCGetPenyaPlayer( 3, nGetPenya );
				bFind = FALSE;
			}
			
			if( bFind )
			{
				pMover->AddGold( (int)( nGetPenya ) );
				((CUser*)pMover)->AddGCGetPenyaPlayer( nGetResult, nGetPenya );

				LogItemInfo aLogItem;
				aLogItem.Action		= "9";
				aLogItem.SendName	= pMover->GetName();
				aLogItem.RecvName	= "GUILDCOMBAT_VP";
				aLogItem.WorldId	= pMover->GetWorld()->GetID();
				aLogItem.Gold		= (DWORD)( pMover->GetGold() - nGetPenya );
				aLogItem.Gold2		= pMover->GetGold();
				//aLogItem.ItemName	= "SEED";
				_stprintf( aLogItem.szItemName, "%d", II_GOLD_SEED1 );
				aLogItem.itemNumber	= (DWORD)( nGetPenya );
				g_DPSrvr.OnLogItem( aLogItem );
			}				
			vector<CGuildCombat::__GCRESULTVALUEPLAYER>::iterator ita = &(g_GuildCombatMng.m_GCResultValuePlayer[nFindVeci]);
			g_GuildCombatMng.m_GCResultValuePlayer.erase( ita );		
		}
	}
}

void CDPDatabaseClient::OnContinueGC( CAr & ar, DPID, DPID )
{
	g_GuildCombatMng.m_uWinGuildId = 0;
	g_GuildCombatMng.m_nWinGuildCount = 0;
	
	g_UserMng.AddGCWinGuild();
}

#if __VER >= 11 // __SYS_PLAYER_DATA
void CDPDatabaseClient::OnAllPlayerData( CAr & ar , DPID, DPID )
{
	CPlayerDataCenter::GetInstance()->Serialize( ar );
//	if( g_uKey == 101 )
//		SendITEM_TBL_Update();
}

void CDPDatabaseClient::OnAddPlayerData( CAr & ar, DPID, DPID )
{
	u_long idPlayer;
	PlayerData pd;
	ar >> idPlayer;
	ar.ReadString( pd.szPlayer, MAX_PLAYER );
	ar.Read( &pd.data, sizeof(sPlayerData) );
	CPlayerDataCenter::GetInstance()->AddPlayerData( idPlayer, pd );
}

void CDPDatabaseClient::OnDeletePlayerData( CAr & ar, DPID, DPID )
{
	u_long idPlayer;
	ar >> idPlayer;
	CPlayerDataCenter::GetInstance()->DeletePlayerData( idPlayer );
}

void CDPDatabaseClient::SendUpdatePlayerData( CUser* pUser )
{
	BEFORESENDDUAL( ar, PACKETTYPE_UPDATE_PLAYER_DATA, DPID_UNKNOWN, DPID_UNKNOWN );  
	sPlayerData data;
	data.nJob	= pUser->GetJob();
	data.nLevel	= pUser->GetLevel();
	data.nSex	= pUser->GetSex();
	ar << pUser->m_idPlayer;
	ar.Write( &data, sizeof(sPlayerData) );
	SEND( ar, this, DPID_SERVERPLAYER );	
}

void CDPDatabaseClient::OnUpdatePlayerData( CAr & ar , DPID, DPID )
{
	u_long idPlayer;
	sPlayerData data;
	ar >> idPlayer;
	ar.Read( &data, sizeof(sPlayerData) );
	
	PlayerData* pPlayerData		= CPlayerDataCenter::GetInstance()->GetPlayerData( idPlayer );
	if( pPlayerData )
	{
		memcpy( &pPlayerData->data, &data, sizeof(sPlayerData) );
#if __VER >= 13 // __COUPLE_1117
		CCoupleHelper::Instance()->OnUpdatePlayerData( idPlayer, pPlayerData );
#endif	// __COUPLE_1117
#if __VER >= 15 // __CAMPUS
		CCampusHelper::GetInstance()->OnUpdatePlayerData( idPlayer, pPlayerData );
#endif // __CAMPUS
	}
}
#else	// __SYS_PLAYER_DATA
void CDPDatabaseClient::OnAllPlayerID( CAr & ar, DPID, DPID )
{
//	CHAR lpszPlayer[36]	= { 0, };
	CHAR lpszPlayer[256]	= { 0, };
	u_long uSize, idPlayer;

	DWORD dwTickCount	= GetTickCount();

	ar >> uSize; 
	for( u_long i = 0; i < uSize; i++ )
	{
		ar >> idPlayer;
		ar.ReadString( lpszPlayer, 256 );
		prj.SetPlayerID( idPlayer, lpszPlayer );
	}
	
	if( g_uKey == 101 )
	{
		SendITEM_TBL_Update();
	}
}
void CDPDatabaseClient::OnPlayerID( CAr & ar, DPID, DPID )
{
	u_long idPlayer;
//	CHAR lpszPlayer[36]	= { 0, };
	CHAR lpszPlayer[256]	= { 0, };

	ar >> idPlayer;
	ar.ReadString( lpszPlayer, 256 );
	prj.SetPlayerID( idPlayer, lpszPlayer );
}

void CDPDatabaseClient::OnRemovePlayerID( CAr & ar, DPID, DPID )
{
	u_long idPlayer;

	ar >> idPlayer;
	prj.RemovePlayerID( idPlayer );
}
#endif	// __SYS_PLAYER_DATA

void CDPDatabaseClient::OnBaseGameSetting( CAr & ar, DPID, DPID )
{
#ifdef __S1108_BACK_END_SYSTEM
	BOOL bBaseGameSetting = FALSE;
	BOOL bFirst = FALSE;
	ar >> bFirst;
	ar >> bBaseGameSetting;
	if( bBaseGameSetting )
	{
		ar >> prj.m_fMonsterExpRate	>> prj.m_fGoldDropRate >> prj.m_fItemDropRate >> prj.m_fMonsterHitRate;
		ar >> prj.m_fShopCost >> prj.m_fMonsterRebirthRate >> prj.m_fMonsterHitpointRate >> prj.m_fMonsterAggressiveRate >> prj.m_fMonsterRespawnRate;
	}

	g_UserMng.AddGameRate( prj.m_fMonsterExpRate, GAME_RATE_MONSTEREXP );
	g_UserMng.AddGameRate( prj.m_fGoldDropRate, GAME_RATE_GOLDDROP );
	g_UserMng.AddGameRate( prj.m_fItemDropRate, GAME_RATE_ITEMDROP );	
	g_UserMng.AddGameRate( prj.m_fMonsterHitRate, GAME_RATE_MONSTERHIT );
	g_UserMng.AddGameRate( prj.m_fShopCost, GAME_RATE_SHOPCOST );
	g_UserMng.AddGameRate( prj.m_fMonsterRebirthRate, GAME_RATE_REBIRTH );
	g_UserMng.AddGameRate( prj.m_fMonsterHitpointRate, GAME_RATE_HITPOINT );
	g_UserMng.AddGameRate( prj.m_fMonsterAggressiveRate, GAME_RATE_AGGRESSIVE );
	g_UserMng.AddGameRate( prj.m_fMonsterRespawnRate, GAME_RATE_RESPAWN );	

	if( bFirst )
	{
		CWorld* pWorld;
		pWorld	= g_WorldMng.GetFirstActive();
		while( pWorld )
		{
			pWorld->LoadRegion();
#ifdef __EVENT_0117
		#ifdef __LAYER_1021
			CRespawner* pRespawner	= pWorld->m_respawner.Proto();
		#else	// __LAYER_1021
			CRespawner* pRespawner	= &pWorld->m_respawner;
		#endif	// __LAYER_1021
			for( int i = 0; i < (int)( pRespawner->m_vRespawnInfo[RESPAWNTYPE_REGION].size() ); ++i )
			{
				CRespawnInfo* pRespawnInfo	= &( pRespawner->m_vRespawnInfo[RESPAWNTYPE_REGION][i] );
		
				if( pRespawnInfo->m_dwType == OT_MOVER )	// 몬스터 리스폰
				{
					MoverProp* pMoverProp	= prj.GetMoverProp( pRespawnInfo->m_dwIndex );
#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
#if __VER >= 15 // __HERO129_VER15				// 15차 히어로 레벨확장
					if( pMoverProp && pMoverProp->dwFlying == 0 && pMoverProp->dwLevel > 0 && pMoverProp->dwLevel <= MAX_MONSTER_LEVEL )
		#else	// 15차 히어로 레벨확장
					if( pMoverProp && pMoverProp->dwFlying == 0 && pMoverProp->dwLevel > 0 && pMoverProp->dwLevel <= MAX_LEGEND_LEVEL )
		#endif	// 15차 히어로 레벨확장
#else //__LEGEND	//	10차 전승시스템	Neuz, World, Trans
					if( pMoverProp && pMoverProp->dwFlying == 0 && pMoverProp->dwLevel > 0 && pMoverProp->dwLevel <= MAX_GENERAL_LEVEL )
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans
					{
#ifdef _DEBUG
						TRACE( "%d, (%d, %d, %d, %d)\n", pRespawnInfo->m_dwIndex, pRespawnInfo->m_rect.left, pRespawnInfo->m_rect.top, pRespawnInfo->m_rect.right, pRespawnInfo->m_rect.bottom );
#endif	// _DEBUG
						CEventGeneric::GetInstance()->AddRegionGeneric( pMoverProp->dwLevel, pWorld->GetID(), pRespawnInfo );
#ifdef __EVENTLUA_SPAWN
						if( pWorld->GetID() == WI_WORLD_MADRIGAL )	// 현재까지는 마드리갈 대륙에만 출현한다.
							prj.m_EventLua.SetAddSpawnRegion( pWorld->GetID(), pRespawnInfo->m_rect );		
#endif // __EVENTLUA_SPAWN
					}	// if
				}	// if
			}	// for
#endif	// __EVENT_0117
			pWorld	= pWorld->nextptr;
		}	// while
#ifdef __EVENT_0117
		CEventGeneric::GetInstance()->SortRegionGeneric();
#endif	// __EVENT_0117
	}	// if
#endif // __S1108_BACK_END_SYSTEM
}

void CDPDatabaseClient::OnMonsterRespawnSetting( CAr & ar, DPID, DPID )
{
#ifdef __S1108_BACK_END_SYSTEM
	char szMonsterName[32];
	D3DXVECTOR3	vPos;
	int		nAddIndex;
	int		nQuantity;
	int		nAggressive;
	int		nRange;
	int		nTime;

	int nMonsterRespawnSize;
	ar >> nMonsterRespawnSize;
	for( int i = 0 ; i < nMonsterRespawnSize ; ++i )
	{
		ar >> nAddIndex;
		ar.ReadString( szMonsterName, 32 );
		ar >> vPos;
		ar >> nQuantity;
		ar >> nAggressive;
		ar >> nRange;
		ar >> nTime;

		MoverProp* pMoverProp	= NULL;
		pMoverProp	= prj.GetMoverProp( szMonsterName );
		
		if( pMoverProp )
		{
			CWorld* pWorld	= g_WorldMng.GetWorld( WI_WORLD_MADRIGAL );
			
			if( pWorld == NULL )
				return;
			
			CRespawnInfo ri;
			ri.m_dwType = OT_MOVER;
			ri.m_dwIndex = pMoverProp->dwID;
			ri.m_nMaxcb = nQuantity;
			ri.m_nMaxAttackNum = nAggressive;
			if( pMoverProp->dwFlying != 0 )
				ri.m_vPos = vPos;
			ri.m_rect.left		= (LONG)( vPos.x - nRange );
			ri.m_rect.right		= (LONG)( vPos.x + nRange );
			ri.m_rect.top		= (LONG)( vPos.z - nRange );
			ri.m_rect.bottom	= (LONG)( vPos.z + nRange );
			ri.m_uTime = nTime;
			ri.m_nGMIndex = nAddIndex;
			
			pWorld->m_respawner.Add( ri, TRUE );
		}
	} 

	int nRemoveSize;
	int nRemoveIndex;
	ar >> nRemoveSize;
	for( i = 0 ; i < nRemoveSize ; ++ i )
	{
		ar >> nRemoveIndex;
		
		CWorld* pWorld	= g_WorldMng.GetWorld( WI_WORLD_MADRIGAL );
		
		if( pWorld == NULL )
			return;
		pWorld->m_respawner.Remove( nRemoveIndex, TRUE );
	}
#endif // __S1108_BACK_END_SYSTEM
}

void CDPDatabaseClient::OnMonsterPropSetting( CAr & ar, DPID, DPID )
{
#ifdef __S1108_BACK_END_SYSTEM
	char	szMonsterName[32];
	int		nHitPoint;
	int		nAttackPower;
	int		nDefence;
	int		nExp;
	int		nItemDrop;
	int		nPenya;

	int		nMonsterPropSize;

	prj.m_nAddMonsterPropSize = 0;
	prj.m_nRemoveMonsterPropSize = 0;

	ar >> nMonsterPropSize;
	for( int i = 0 ; i < nMonsterPropSize ; ++i )
	{
		ar.ReadString( szMonsterName, 32 );
		ar >> nHitPoint;
		ar >> nAttackPower;
		ar >> nDefence;
		ar >> nExp;
		ar >> nItemDrop;
		ar >> nPenya;
		
		MoverProp* pMoverProp	= NULL;
		pMoverProp	= prj.GetMoverProp( szMonsterName );
		if( pMoverProp )
		{
			pMoverProp->m_fHitPoint_Rate = (float)nHitPoint / 100.0f;
			pMoverProp->m_fAttackPower_Rate = (float)nAttackPower / 100.0f;
			pMoverProp->m_fDefence_Rate = (float)nDefence / 100.0f;
			pMoverProp->m_fExp_Rate = (float)nExp / 100.0f;
			pMoverProp->m_fItemDrop_Rate = (float)nItemDrop / 100.0f;
			pMoverProp->m_fPenya_Rate = (float)nPenya / 100.0f;
			pMoverProp->m_bRate = TRUE;

			MONSTER_PROP MonsterProp;
			strcpy( MonsterProp.szMonsterName, szMonsterName );
			MonsterProp.nHitPoint = nHitPoint;
			MonsterProp.nAttackPower = nAttackPower;
			MonsterProp.nDefence = nDefence;
			MonsterProp.nExp = nExp;
			MonsterProp.nItemDrop = nItemDrop;
			MonsterProp.nPenya = nPenya;
			prj.AddMonsterProp( MonsterProp );
		}
	} 

	int	nRemovePropSize;
	ar >> nRemovePropSize;
	for( i = 0 ; i < nRemovePropSize ; ++i )
	{
		ar.ReadString( szMonsterName, 32 );
		MoverProp* pMoverProp	= NULL;
		pMoverProp	= prj.GetMoverProp( szMonsterName );
		if( pMoverProp )
		{
			pMoverProp->m_fHitPoint_Rate = 1.0f;
			pMoverProp->m_fAttackPower_Rate = 1.0f;
			pMoverProp->m_fDefence_Rate = 1.0f;
			pMoverProp->m_fExp_Rate = 1.0f;
			pMoverProp->m_fItemDrop_Rate = 1.0f;
			pMoverProp->m_fPenya_Rate = 1.0f;
			pMoverProp->m_bRate = FALSE;
			prj.RemoveMonsterProp( szMonsterName );
		}
	}
	
	g_UserMng.AddMonsterProp();
#endif // __S1108_BACK_END_SYSTEM
}

void CDPDatabaseClient::OnGMChat( CAr & ar, DPID, DPID )
{
#ifdef __S1108_BACK_END_SYSTEM
	int nSize;
	ar >> nSize;
	for( int i = 0 ; i < nSize ; ++i )
	{
		ar.ReadString( prj.m_chGMChat[i], 256 );
	}
	if( 0 < nSize )
		g_UserMng.AddGMChat( nSize );
#endif // __S1108_BACK_END_SYSTEM
}
/*
#ifdef __S0114_RELOADPRO
void CDPDatabaseClient::OnReloadProject( CAr & ar, DPID dpidCache, DPID dpidUser )
{
	prj.m_AddRemoveLock.Enter( theLineFile );	// lock1
	g_UserMng.m_AddRemoveLock.Enter( theLineFile );	// lock2
	
	CUser* pUser	= g_UserMng.GetUser( dpidCache, dpidUser );
	if( pUser )
	{
		g_DPSrvr.SendReloadProject( dpidCache, dpidUser );
	}
	g_UserMng.m_AddRemoveLock.Leave( theLineFile );	// unlock2
	prj.m_AddRemoveLock.Leave( theLineFile );	// unlock1
}
#endif // __S0114_RELOADPRO
*/
void CDPDatabaseClient::SendLogPlayDeath(CMover* pMover, CMover* pSender)
{
	BEFORESENDDUAL( ar, PACKETTYPE_LOG_PLAY_DEATH, DPID_UNKNOWN, DPID_UNKNOWN );

	ar.WriteString( pMover->GetName() );
	ar << pMover->GetWorld()->GetID();
	ar << pMover->GetPos();
	ar << pMover->GetLevel();
	ar << (int)0;	// MaxHitPower
	ar << pMover->GetMaxHitPoint();
	if( pSender )
		ar.WriteString( pSender->GetName() );
	else
		ar.WriteString( "" );
	ar << pMover->m_idPlayer;

	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendLogSkillPoint( int nAction, int nPoint, CMover* pMover, LPSKILL pSkill )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SKILLPOINTLOG, DPID_UNKNOWN, DPID_UNKNOWN );

	ar << nAction;
	ar << pMover->m_idPlayer;
	ar << pMover->m_nSkillLevel;
	ar << pMover->m_nSkillPoint;
	if( pSkill )
	{
		ar << pSkill->dwSkill;
		ar << pSkill->dwLevel;
	}
	else
	{
		ar << (DWORD)0;
		ar << (DWORD)0;
	}
	ar << nPoint;

	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendLogExpBox( u_long idPlayer, OBJID objid, EXPINTEGER iExp, BOOL bGet )
{
	BEFORESENDDUAL( ar, PACKETTYPE_LOG_EXPBOX, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << objid << iExp << bGet;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendLogLevelUp( CMover* pSender, int Action )
{
	BEFORESENDDUAL( ar, PACKETTYPE_LOG_LEVELUP, DPID_UNKNOWN, DPID_UNKNOWN );

	ar << pSender->GetExp1();
	ar << pSender->GetFlightLv();
	ar << pSender->GetLevel();
	ar << pSender->GetJob();
	ar << (DWORD)1;				// joblevel
	ar << pSender->m_nStr;
	ar << pSender->m_nDex;
	ar << pSender->m_nInt;
	ar << pSender->m_nSta;
	ar << pSender->m_nRemainGP;
	ar << (LONG)0;					// LP사용하지 않는다.
	ar << pSender->GetGold();
	ar << ( (CUser*)pSender )->GetPlayerTime();
	ar << Action;
	ar << pSender->m_idPlayer;
	
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendLogGamemaChat( CUser* pUser, LPCTSTR lpszString )
{
	BEFORESENDDUAL( ar, PACKETTYPE_LOG_GAMEMASTER_CHAT, DPID_UNKNOWN, DPID_UNKNOWN );
	
	ar << pUser->m_idPlayer;
	ar.WriteString( lpszString );
	
	SEND( ar, this, DPID_SERVERPLAYER );

}

void CDPDatabaseClient::SendLogItem( CUser* pUser, CMover* pSender )
{
}

void CDPDatabaseClient::SendLogSMItemUse( const char *Action, CUser* pUser, CItemElem* pItemElem, ItemProp* pItemProp, const char* szName1 )
{
	BEFORESENDDUAL( ar, PACKETTYPE_LOG_ALLITEM, DPID_UNKNOWN, DPID_UNKNOWN );
	ar.WriteString( Action );
	ar.WriteString( pUser->GetName() );
	if( strlen( szName1 ) > 0 )
		ar.WriteString( szName1 );
	else 
		ar.WriteString( pUser->GetName() );
	// chipi_090623 수정 - 첫 접속시 만료된 버프인 경우 월드가 없는 상태로 들어온다. // Begin
	// ar << pUser->GetWorld()->GetID();
	DWORD dwWorldId = pUser->GetWorld() ? pUser->GetWorld()->GetID() : WI_WORLD_NONE;
	ar << dwWorldId;
	// chipi_090623 // End
	ar << pUser->GetGold();
	ar << pUser->GetGold();
	

	if( pItemElem == NULL )
	{
		ar << (SERIALNUMBER)0;
		ar << (int)0;
		ar << (int)0;
		//ar.WriteString( pItemProp->szName );
		char szItemId[32] = {0, };
		_stprintf( szItemId, "%d", pItemProp->dwID );
		ar.WriteString( szItemId );
		ar << (DWORD)1;
		ar << (DWORD)0;
		ar << pUser->m_dwGoldBank[pUser->m_nSlot];
		ar << (int)0;
		ar << (int)0;
		ar << (int)0;
		ar << (int)0;
		ar << (BOOL)0;
		ar << (DWORD)0;
#if __VER >= 12 // __EXT_PIERCING
		ar << (int)0;
		ar << (int)0;
#else // __EXT_PIERCING
		ar << (int)0;
		ar << (DWORD)0;
		ar << (DWORD)0;
		ar << (DWORD)0;
		ar << (DWORD)0;
#if __VER >= 9 // __ULTIMATE
		ar << (DWORD)0;
#endif // __ULTIMATE
#endif // __EXT_PIERCING
		ar << (int)0;
	}
	else
	{
		ar << pItemElem->GetSerialNumber();
		ar << pItemElem->m_nHitPoint;
		ar << pItemElem->m_nRepair;
		//ar.WriteString( (char*)pItemElem->GetProp()->szName );
		char szItemId[32] = {0, };
		_stprintf( szItemId, "%d", pItemElem->GetProp()->dwID );
		ar.WriteString( szItemId );
		ar << (DWORD)pItemElem->m_nItemNum;
		ar << pItemElem->GetAbilityOption();
		ar << pUser->m_dwGoldBank[pUser->m_nSlot];
		ar << (int)0;
		ar << (int)0;
		ar << (int)pItemElem->m_bItemResist;
		ar << (int)pItemElem->m_nResistAbilityOption;
		ar << pItemElem->m_bCharged;
		ar << pItemElem->m_dwKeepTime;
#if __VER >= 12 // __EXT_PIERCING
		ar << pItemElem->GetPiercingSize();
		for( int i=0; i<pItemElem->GetPiercingSize(); i++ )
			ar << pItemElem->GetPiercingItem( i );
		ar << pItemElem->GetUltimatePiercingSize();
		for( i=0; i<pItemElem->GetUltimatePiercingSize(); i++ )
			ar << pItemElem->GetUltimatePiercingItem( i );
#else // __EXT_PIERCING
		ar << pItemElem->GetPiercingSize();
		ar << pItemElem->GetPiercingItem( 0 );
		ar << pItemElem->GetPiercingItem( 1 );
		ar << pItemElem->GetPiercingItem( 2 );
		ar << pItemElem->GetPiercingItem( 3 );
#if __VER >= 9 // __ULTIMATE
		ar << pItemElem->GetPiercingItem( 4 );
#endif // __ULTIMATE
#endif // __EXT_PIERCING
		ar << pItemElem->GetRandomOptItemId();
		// mirchang_100514 TransformVisPet_Log
#if __VER >= 15 // __PETVIS
		if( pItemElem->IsTransformVisPet() == TRUE )
		{
			ar << (BYTE)100;
		}
#endif // __PETVIS
		// mirchang_100514 TransformVisPet_Log
	}
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendChangeBankPass( const char* szName, const char *szNewPass, u_long uidPlayer )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CHANGEBANKPASS, DPID_UNKNOWN, DPID_UNKNOWN );
	
	ar.WriteString( szName );
	ar.WriteString( szNewPass );
	ar << uidPlayer;

	SEND( ar, this, DPID_SERVERPLAYER );
}
#if __VER >= 13 // __HONORABLE_TITLE			// 달인
void	CDPDatabaseClient::SendLogGetHonorTime(CMover* pMover, int nGetHonor )
{
	BEFORESENDDUAL( ar, PACKETTYPE_LOG_GETHONORTIME, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nGetHonor;
	ar << pMover->m_idPlayer;
	SEND( ar, this, DPID_SERVERPLAYER );
}
#endif	// __HONORABLE_TITLE			// 달인

void CDPDatabaseClient::SendLogUniqueItem(CMover* pMover, CItem* pItem, int nOption )
{
	SendLogUniqueItem2( pMover, pItem->GetProp(), nOption );
}

void CDPDatabaseClient::SendLogUniqueItem2( CMover* pMover, ItemProp* pItemProp, int nOption )
{
	BEFORESENDDUAL( ar, PACKETTYPE_LOG_UNIQUEITEM, DPID_UNKNOWN, DPID_UNKNOWN );
	ar.WriteString( pMover->GetName() );
	ar << pMover->GetWorld()->GetID();
	ar << pMover->GetPos();
//	if( rItemElem.m_szItemText[0] == '\0' )
		//ar.WriteString( pItemProp->szName );
	char szItemId[32] = {0, };
	::memset( szItemId, 0, sizeof( szItemId ) );
	_stprintf( szItemId, "%d", pItemProp->dwID );
	ar.WriteString( szItemId );
//	else
//		ar.WriteString( rItemElem.m_szItemText );
	ar << nOption;
	ar << pMover->m_idPlayer;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendSchoolReport( PSCHOOL_ENTRY pSchool )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SCHOOL_REPORT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar.Write( pSchool, sizeof(SCHOOL_ENTRY) * MAX_SCHOOL );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendBuyingInfo( PBUYING_INFO2 pbi2, SERIALNUMBER iSerialNumber )
{
	BEFORESENDDUAL( ar, PACKETTYPE_BUYING_INFO, DPID_UNKNOWN, DPID_UNKNOWN );
	ar.Write( (void*)pbi2, sizeof(BUYING_INFO2) );
	ar << iSerialNumber;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendLogPkPvp( CMover* pUser, CMover* pLose, int nPoint, char chState )
{
	if( !g_eLocal.GetState( EVE_PK ) )
		return;

	BEFORESENDDUAL( ar, PACKETTYPE_LOG_PK_PVP, DPID_UNKNOWN, DPID_UNKNOWN );

	int nSendBuf = 0;

	if( pUser == NULL || pLose == NULL )
	{
		if( pUser == NULL )
			nSendBuf = 1;
		else
			nSendBuf = 2;
	}
	else
	{
		nSendBuf = 3;
	}
	
	ar << nSendBuf;
	ar << nPoint;
	ar << chState;

	if( nSendBuf == 1 || nSendBuf == 3 )
	{
		ar << pLose->m_idPlayer;
		ar << pLose->GetWorld()->GetID();
		ar << pLose->GetLevel();
		ar << pLose->GetPos();
		ar << pLose->m_nFame;
#if __VER >= 8 // __S8_PK
		ar << pLose->m_nPKValue;
		ar << pLose->m_dwPKPropensity;
#else // __VER >= 8 // __S8_PK
		ar << pLose->m_nNumKill;
		ar << pLose->m_nSlaughter;
#endif // __VER >= 8 // __S8_PK
	}

	if( nSendBuf == 2 || nSendBuf == 3 )
	{
		ar << pUser->m_idPlayer;
		ar << pUser->GetWorld()->GetID();
		ar << pUser->GetLevel();
		ar << pUser->GetPos();
		ar << pUser->m_nFame;
#if __VER >= 8 // __S8_PK
		ar << pUser->m_nPKValue;
		ar << pUser->m_dwPKPropensity;
#else // __VER >= 8 // __S8_PK
		ar << pUser->m_nNumKill;
		ar << pUser->m_nSlaughter;
#endif // __VER >= 8 // __S8_PK
	}

	SEND( ar, this, DPID_SERVERPLAYER );		
}

void CDPDatabaseClient::SendLogSchool( u_long idPlayer, LPCTSTR szName )
{
	BEFORESENDDUAL( ar, PACKETTYPE_LOG_SCHOOL, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer;
	ar.WriteString( szName );
	SEND( ar, this, DPID_SERVERPLAYER );
}


void CDPDatabaseClient::SendGuildGetPay( u_long idGuild, DWORD nGoldGuild, DWORD dwPay )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILD_DB_REALPENYA, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idGuild << nGoldGuild << dwPay;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendGuildContribution( CONTRIBUTION_CHANGED_INFO & info, BYTE nLevelUp, LONG nMemberLevel )
{
	BEFORESENDDUAL( ar, PACKETTYPE_WD_GUILD_CONTRIBUTION, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << info; 
	ar << nLevelUp;	// 레벨업 1 : 0( 했으면 1 )
	ar << nMemberLevel;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendQueryGuildBank()
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILD_BANK_QUERY, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::UpdateGuildRanking()
{
	BEFORESENDDUAL( ar, PACKETTYPE_UPDATE_GUILD_RANKING, DPID_UNKNOWN, DPID_UNKNOWN );
	
	ar << static_cast<int>(1); // 월드 서버에서 요청
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::UpdateGuildRankingUpdate()
{
	BEFORESENDDUAL( ar, PACKETTYPE_UPDATE_GUILD_RANKING_DB, DPID_UNKNOWN, DPID_UNKNOWN );
	
	ar << static_cast<int>(1); // 월드 서버에서 요청
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnGuildBank( CAr & ar, DPID, DPID )
{
	int nCount;

	// 1. 몇개의 길드 뱅크 정보가 들어왔는지 Serialize한다.
	ar >> nCount;

	// 2. 루프를 돌며 길드창고 정보를 Serialize한다.
	for (int i=0; i<nCount; ++i)
	{
		int nGuildId = 0, nGoldGuild = 0;

		// 2-1 길드 핸들값을 받는다.
		ar >> nGuildId;
		ar >> nGoldGuild;

		// 2-2 핸들값으로 길드매니져에서 길드 정보를 포인트하여 창고정보를 Serialize한다.
		CGuild* pGuild = g_GuildMng.GetGuild( nGuildId );
		if( pGuild )
		{
			pGuild->m_nGoldGuild = nGoldGuild;
			pGuild->m_GuildBank.Serialize( ar );
		}
		else
		{
			// 임시로 템프변수 만들어서 받기 banktmp
			CGuild tempGuild;
			tempGuild.m_GuildBank.Serialize( ar );
		}
	}
}

void CDPDatabaseClient::OnUpdateGuildRankingFinish( CAr & ar, DPID, DPID )
{
	CGuildRank::Instance()->Serialize( ar );
}


void CDPDatabaseClient::SendQueryGuildQuest( void )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUERYGUILDQUEST, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendInsertGuildQuest( u_long idGuild, int nId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_INSERTGUILDQUEST, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idGuild << nId;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendUpdateGuildQuest( u_long idGuild, int nId, int nState )
{
	BEFORESENDDUAL( ar, PACKETTYPE_UPDATEGUILDQUEST, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idGuild << nId << nState;	
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnQueryGuildQuest( CAr & ar, DPID, DPID )
{
	int nCount;
	u_long idGuild;
	CGuild* pGuild;

	ar >> nCount;

	for( int i = 0; i < nCount; i++ )
	{
		ar >> idGuild;
		pGuild	= g_GuildMng.GetGuild( idGuild );	
		if( pGuild )
		{
			ar >> pGuild->m_nQuestSize;
			ar.Read( (void*)pGuild->m_aQuest, sizeof(GUILDQUEST) * pGuild->m_nQuestSize );
		}
		else
		{
			CGuild waste;
			ar >> waste.m_nQuestSize;
			ar.Read( (void*)waste.m_aQuest, sizeof(GUILDQUEST) * waste.m_nQuestSize );
		}
	}
}

void CDPDatabaseClient::SendQuerySetPlayerName( u_long idPlayer, const char* lpszPlayer, DWORD dwData )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUERYSETPLAYERNAME, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer;
	ar.WriteString( lpszPlayer );
	ar << dwData;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendPing( void )
{
	if( FALSE == m_bAlive )
	{
#ifndef _DEBUG
		Error( "TRANS : is not alive" );
		if( m_cbPing <= 1 )
		{
			ExitProcess( -1 );
			return;
		}
#endif	// _DEBUG
	}

	m_cbPing++;
	m_bAlive	= FALSE;
	BEFORESENDDUAL( ar, PACKETTYPE_PING, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << time_null();
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnPing( CAr & ar, DPID, DPID )
{
	time_t tSend, tTrans;
	ar >> tSend >> tTrans;

	time_t d	= time_null() - tSend;
	CTime tmTrans( tTrans );
#if !defined(_DEBUG)
	if( d >  10 )
		Error( "ping TRANS - %d, trans:%s", d, tmTrans.Format( "%Y/%m/%d %H:%M:%S" ) );
#endif 
	
	m_bAlive	= TRUE;
}

void CDPDatabaseClient::SendQueryMailBox( u_long idReceiver )
{
// 	//	BEGINTEST
// 	Error( "SendQueryMailBox [%d]", idReceiver );

	BEFORESENDDUAL( ar, PACKETTYPE_QUERYMAILBOX, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendQueryMailBoxReq( u_long idReceiver )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUERYMAILBOX_REQ, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendQueryMailBoxCount( u_long idReceiver, int nCount )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUERYMAILBOX_COUNT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver << nCount;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnMailBox( CAr & ar, DPID, DPID )
{
	u_long idReceiver;
	ar >> idReceiver;
	CMailBox* pMailBox	= CPost::GetInstance()->GetMailBox( idReceiver );
	if( pMailBox )
	{
// 		//	BEGINTEST
// 		Error( "OnMailBox [%d]", idReceiver );

		pMailBox->Read( ar );	// pMailBox->m_nStatus	= CMailBox::data;
		CUser* pUser	= g_UserMng.GetUserByPlayerID( idReceiver );
		if( IsValidObj( pUser ) )
		{
// 			//	BEGINTEST
// 			Error( "OnMailBox AddMailBox [%d]", idReceiver );

			pUser->AddMailBox( pMailBox );
			pUser->ResetCheckClientReq();
		}
	}
	else
	{
		Error( "GetMailBox - pMailBox is NULL. idReceiver : %d", idReceiver );
	}
}

void CDPDatabaseClient::OnMailBoxReq( CAr & ar, DPID, DPID )
{
	u_long idReceiver;
	BOOL bHaveMailBox = FALSE;
	ar >> idReceiver >> bHaveMailBox;

	CUser* pUser	= g_UserMng.GetUserByPlayerID( idReceiver );
	if( IsValidObj( pUser ) )
	{
		pUser->CheckTransMailBox( TRUE );
		pUser->SendCheckMailBoxReq( bHaveMailBox );
	}

	if( bHaveMailBox == TRUE )
	{
		CMailBox* pMailBox	= NULL;
		pMailBox	= CPost::GetInstance()->GetMailBox( idReceiver );

		if( pMailBox == NULL )
		{
			CMailBox* pNewMailBox = new CMailBox( idReceiver );
			if( pNewMailBox != NULL )
			{
				if( CPost::GetInstance()->AddMailBox( pNewMailBox ) == TRUE )
				{
					pNewMailBox->ReadReq( ar );	// pMailBox->m_nStatus	= CMailBox::data;

					CUser* pUser	= g_UserMng.GetUserByPlayerID( idReceiver );
					if( IsValidObj( pUser ) )
					{
						pUser->AddMailBox( pNewMailBox );
						pUser->ResetCheckClientReq();
					}
				}
				else
				{
					Error( "CDPDatabaseClient::OnMailBoxReq - AddMailBox Failed. idReceiver : %d", idReceiver );
				}
			}
			else
			{
				Error( "CDPDatabaseClient::OnMailBoxReq - MailBox Create Failed" );
			}
		}
		else
		{
			//////////////////////////////////////////////////////////////////////////
			//	??????????
			Error( "CDPDatabaseClient::OnMailBoxReq - pMailBox is NOT NULL. idReceiver : %d", idReceiver );
		}
	}
	else
	{
		CUser* pUser	= g_UserMng.GetUserByPlayerID( idReceiver );
		if( IsValidObj( pUser ) )
		{
			pUser->ResetCheckClientReq();
		}
	}
}

void CDPDatabaseClient::SendQueryPostMail( u_long idReceiver, u_long idSender, CItemElem& itemElem, int nGold, char* lpszTitle, char* lpszText )
{
// 	//	BEGINTEST
// 	Error( "SendQueryPostMail  Receiver[%d] Sender[%d]", idReceiver, idSender );

	BEFORESENDDUAL( ar, PACKETTYPE_QUERYPOSTMAIL, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver << idSender;
	if( FALSE == itemElem.IsEmpty() )
	{
		ar << (BYTE)1;
		itemElem.Serialize( ar );
	}
	else
	{
		ar << (BYTE)0;
	}
	ar << nGold;
	ar.WriteString( lpszTitle );
	ar.WriteString( lpszText );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendQueryRemoveMail( u_long idReceiver, u_long nMail )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUERYREMOVEMAIL, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver << nMail;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendQueryGetMailItem( u_long idReceiver, u_long nMail )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUERYGETMAILITEM, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver << nMail << g_uIdofMulti/*uQuery*/;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendQueryGetMailGold( u_long idReceiver, u_long nMail )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUERYGETMAILGOLD, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver << nMail << g_uIdofMulti/*uQuery*/;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendQueryReadMail( u_long idReceiver, u_long nMail )
{
	BEFORESENDDUAL( ar, PACKETTYPE_READMAIL, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver << nMail;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnPostMail( CAr & ar, DPID, DPID )
{
// 	//	BEGINTEST
// 	Error( "CDPDatabaseClient::OnPostMail" );

	BOOL	bResult;
	u_long idReceiver;
	ar >> bResult >> idReceiver;
	CMail* pMail	= new CMail;
	pMail->Serialize( ar );

	BOOL bBuying	= FALSE;

	if( TRUE == bResult )
	{
		if( CPost::GetInstance()->AddMail( idReceiver, pMail ) <= 0 )
		{
			Error( "OnPostMail - pMail->m_nMail : %d", pMail->m_nMail );
		}
		else	//SUCCESS
		{
// 			//	BEGINTEST
// 			Error( "CDPDatabaseClient::OnPostMail Receiver[%d] nMail[%d]", idReceiver, pMail->m_nMail );
		}

		CUser* pUser	= (CUser*)prj.GetUserByID( idReceiver );
		if( IsValidObj( pUser ) )
		{
			if( pUser->IsPosting() )
			{
				pUser->AddPostMail( pMail );
			}

//			if( pUser->IsMode( MODE_MAILBOX ) == FALSE )
			{
				pUser->SetMode( MODE_MAILBOX );
				g_UserMng.AddModifyMode( pUser );
			}
		}
		pUser	= (CUser*)prj.GetUserByID( pMail->m_idSender );
		if( IsValidObj( pUser ) && !bBuying )
			pUser->AddDiagText( prj.GetText(TID_MAIL_SEND_OK) );
	}	// FAIL
	else
	{
		Error( "OnPostMail - Send Mail Failed. idSender : %d, idReceiver : %d", pMail->m_idSender, idReceiver );
		CUser* pUser	= (CUser*)prj.GetUserByID( pMail->m_idSender );
		if( IsValidObj( pUser ) )
		{
			if( pMail->m_pItemElem )
				pUser->CreateItem( pMail->m_pItemElem );
			
			if( ::GetLanguage() == LANG_KOR )
				pUser->AddGold( 100 + pMail->m_nGold, TRUE );
			else
				pUser->AddGold( 500 + pMail->m_nGold, TRUE );
			
			pUser->AddDiagText( prj.GetText(TID_GAME_POST_ERROR) );
			CWorld* pWorld	= pUser->GetWorld();
			if( pWorld )
#ifdef __LAYER_1015
				g_dpDBClient.SavePlayer( pUser, pWorld->GetID(), pUser->GetPos(), pUser->GetLayer() );
#else	// __LAYER_1015
				g_dpDBClient.SavePlayer( pUser, pWorld->GetID(), pUser->GetPos() );
#endif	// __LAYER_1015
		}
		else
		{
			// 康
		}
		SAFE_DELETE( pMail );
	}
}

void CDPDatabaseClient::OnRemoveMail( CAr & ar, DPID, DPID )
{
	u_long idReceiver, nMail;
	ar >> idReceiver >> nMail;
	CMailBox* pMailBox	= CPost::GetInstance()->GetMailBox( idReceiver );

	if( pMailBox )
	{
		pMailBox->RemoveMail( nMail );

		CUser* pUser	= (CUser*)prj.GetUserByID( idReceiver );
		if( IsValidObj( pUser ) )
		{
			pUser->AddRemoveMail( nMail, CMail::mail );
			if( pMailBox->IsStampedMailExists() == FALSE && pUser->IsMode( MODE_MAILBOX ) )
			{
				pUser->SetNotMode( MODE_MAILBOX );
				g_UserMng.AddModifyMode( pUser );
			}
		}
	}
}

void CDPDatabaseClient::OnGetMailItem( CAr & ar, DPID, DPID )
{
	u_long idReceiver, nMail, uQuery;
	ar >> idReceiver >> nMail >> uQuery;
	CMailBox* pMailBox	= CPost::GetInstance()->GetMailBox( idReceiver );
	CItemElem	itemRestore;

	if( pMailBox )
	{
		CMail* pMail	= pMailBox->GetMail( nMail );
		if( pMail && pMail->m_pItemElem )
		{
			itemRestore		= *( pMail->m_pItemElem );
			CUser* pUser	= (CUser*)prj.GetUserByID( idReceiver );
			if( IsValidObj( pUser ) )
			{
				if( g_uIdofMulti== uQuery )		// 편지 받기 요청 주체면 아이템 생성, 우편 아이템 제거
				{
					if( pUser->CreateItem( pMail->m_pItemElem ) )
					{
						pUser->AddRemoveMail( nMail, CMail::item );
						itemRestore.Empty();
					}
				}
				else	// 우편 아이템 받기 요청 주체가 아니면, 우편 아이템 제거
				{
					pUser->AddRemoveMail( nMail, CMail::item );
					itemRestore.Empty();
				}
			}
		}
		pMailBox->RemoveMailItem( nMail );
	}
	if( !itemRestore.IsEmpty() && g_uIdofMulti== uQuery )
		g_dpDBClient.SendQueryPostMail( idReceiver, 0, itemRestore, 0, "", "" );
}

void CDPDatabaseClient::OnGetMailGold( CAr & ar, DPID, DPID )
{
	u_long idReceiver, nMail, uQuery;
	ar >> idReceiver >> nMail >> uQuery;
	CMailBox* pMailBox	= CPost::GetInstance()->GetMailBox( idReceiver );
	int nRestore	= 0;

	if( pMailBox )
	{
		CMail* pMail	= pMailBox->GetMail( nMail );
		if( pMail && pMail->m_nGold > 0 )
		{
			nRestore	= pMail->m_nGold;
			CUser* pUser	= (CUser*)prj.GetUserByID( idReceiver );
			if( IsValidObj( pUser ) )
			{
				if( g_uIdofMulti == uQuery )
				{
					if( CanAdd( pUser->GetGold(), pMail->m_nGold ) )
					{
						pUser->AddGold( pMail->m_nGold );
						pUser->AddRemoveMail( nMail, CMail::gold );
						nRestore	= 0;
					}
				}
				else
				{
					pUser->AddRemoveMail( nMail, CMail::gold );
					nRestore	= 0;
				}
			}
		}
		pMailBox->RemoveMailGold( nMail );
	}
	if( nRestore > 0 && g_uIdofMulti == uQuery )
	{
		CItemElem itemElem;		// empty item
		g_dpDBClient.SendQueryPostMail( idReceiver, 0, itemElem, nRestore, "", "" );
	}
}

void CDPDatabaseClient::OnReadMail( CAr & ar, DPID, DPID )
{
	u_long idReceiver, nMail;
	ar >> idReceiver >> nMail;
	CMailBox* pMailBox	= CPost::GetInstance()->GetMailBox( idReceiver );

	if( pMailBox )
	{
		pMailBox->ReadMail( nMail );
		CUser* pUser	= (CUser*)prj.GetUserByID( idReceiver );
		if( IsValidObj( pUser ) )
		{
			pUser->ResetCheckClientReq();
			pUser->AddRemoveMail( nMail, CMail::read );
			if( pMailBox->IsStampedMailExists() == FALSE && pUser->IsMode( MODE_MAILBOX ) )
			{
				pUser->SetNotMode( MODE_MAILBOX );
				g_UserMng.AddModifyMode( pUser );
			}
		}
	}
}

void CDPDatabaseClient::OnAllMail( CAr & ar , DPID, DPID )
{
#ifdef __CHIPI_ITEMUPDATE_080804
	if( ::GetLanguage() == LANG_KOR && g_uKey == 601 )
		SendITEM_TBL_Update();
	else
#endif // __CHIPI_ITEMUPDATE_080804
	if( g_uKey == 101 )
		SendITEM_TBL_Update();

	CPost::GetInstance()->Serialize( ar, FALSE );
}

void CDPDatabaseClient::OnQueryRemoveGuildBankTbl( CAr & ar, DPID, DPID )
{
	if( g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) == 0 )
		return;

	int nNo;
	u_long idGuild;
	DWORD dwItemId;
	SERIALNUMBER iSerialNumber;
	DWORD dwItemNum;
	ar >> nNo >> idGuild >> dwItemId >> iSerialNumber >> dwItemNum;

	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	DWORD dwRemoved	= 0;
	if( pGuild )
	{
		if( dwItemId == 0 )		// penya
		{
			dwRemoved	= dwItemNum;
			if( pGuild->m_nGoldGuild - dwRemoved > pGuild->m_nGoldGuild )	// overflow
				dwRemoved	= pGuild->m_nGoldGuild;
			pGuild->m_nGoldGuild	-= dwRemoved;
			if( dwRemoved > 0 )
			{
				g_DPSrvr.UpdateGuildBank( pGuild, GUILD_QUERY_REMOVE_GUILD_BANK );
				CUser* pUsertmp;
				CGuildMember* pMember;
				for( map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
				{
					pMember		= i->second;
					pUsertmp	= (CUser*)prj.GetUserByID( pMember->m_idPlayer );
					if( IsValidObj( pUsertmp ) && pUsertmp->m_bGuildBank )
						pUsertmp->AddRemoveGuildBankItem( idGuild,  NULL_ID, dwRemoved );
				}
			}
		}
		else	// item
		{
			CItemElem* pItemElem	= pGuild->GetItem( dwItemId, iSerialNumber );
			if( pItemElem )
			{
				DWORD dwId	= pItemElem->m_dwObjId;
				if( static_cast<short>( dwItemNum ) < 0 )
					dwItemNum	= 0;

				dwRemoved	= (DWORD)pGuild->RemoveItem( dwId, static_cast<short>( dwItemNum ) );
				if( dwRemoved > 0 )
				{
					g_DPSrvr.UpdateGuildBank( pGuild, GUILD_QUERY_REMOVE_GUILD_BANK );
#ifdef _DEBUG
					char lpOutputString[200];
					sprintf( lpOutputString, "WORLD: OnQueryRemoveGuildBankTbl: dwId = %d, nRemoved=%d", dwId, dwRemoved );
					OutputDebugString( lpOutputString );
#endif	// _DEBUG
					CUser* pUsertmp;
					CGuildMember* pMember;
					for( map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
					{
						pMember		= i->second;
						pUsertmp	= (CUser*)prj.GetUserByID( pMember->m_idPlayer );
						if( IsValidObj( pUsertmp ) && pUsertmp->m_bGuildBank )
							pUsertmp->AddRemoveGuildBankItem( idGuild,  dwId, dwRemoved );
					}
				}
			}
		}
	}
	SendQueryRemoveGuildBankTbl( nNo, dwRemoved );
}

void CDPDatabaseClient::SendQueryRemoveGuildBankTbl( int nNo, DWORD dwRemoved )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUERY_REMOVE_GUILD_BANK_TBL, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nNo << dwRemoved;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnEventGeneric( CAr & ar, DPID, DPID )
{
	DWORD dwFlag;
	CEventGeneric::GetInstance()->Serialize( ar );
	ar >> dwFlag;
	list<PEVENT_GENERIC>* pList	= CEventGeneric::GetInstance()->GetEventList();
	for( list<PEVENT_GENERIC>::iterator i = pList->begin(); i != pList->end(); ++i )
	{
		PEVENT_GENERIC pEvent	= *i;

		// 康
		char lpOutputString[512]	= { 0, };
		sprintf( lpOutputString, "dwFlag=0x%08x, nId=%d, nFlag=%d", dwFlag, pEvent->nId, pEvent->nFlag );
		OutputDebugString( lpOutputString );

		if( dwFlag & pEvent->nFlag )
			g_eLocal.SetState( pEvent->nId, 1 );
	}
}

void CDPDatabaseClient::OnEventFlag( CAr & ar, DPID, DPID )
{
	DWORD dwFlag;
	ar >> dwFlag;

	// 康
	char lpOutputString[100]	= { 0, };
	sprintf( lpOutputString, "OnEventFlag: dwFlag=0x%08x", dwFlag );
	OutputDebugString( lpOutputString );

	list<PEVENT_GENERIC>* pList	= CEventGeneric::GetInstance()->GetEventList();
	for( list<PEVENT_GENERIC>::iterator i = pList->begin(); i != pList->end(); ++i )
	{
		PEVENT_GENERIC pEvent	= *i;
		char lpOutputString[100]	= { 0, };
		sprintf( lpOutputString, "OnEventFlag: nId=%d, nFlag=%d", pEvent->nId, pEvent->nFlag );
		OutputDebugString( lpOutputString );

		if( dwFlag & pEvent->nFlag )
		{
			if( g_eLocal.GetState( pEvent->nId ) == 0 )
			{
				if( g_eLocal.SetState( pEvent->nId, 1 ) )
				{
					g_UserMng.AddSetLocalEvent( pEvent->nId, 1 );
					Error( "event: %d: 1", pEvent->nId );
				}
			}
		}
		else
		{
			if( g_eLocal.GetState( pEvent->nId ) == 1 )
			{
				if( g_eLocal.ClearEvent( pEvent->nId ) )
				{
					g_UserMng.AddSetLocalEvent( pEvent->nId, 0 );
					Error( "OnEvent: %d: 0", pEvent->nId );
				}
			}
		}
	}
}

#if __VER >= 9 // __EVENTLUA
void CDPDatabaseClient::OnEventStateLua( CAr & ar, DPID, DPID )
{
	BOOL bTextOut = FALSE;
	int nMapSize = 0;
	ar >> bTextOut;
	ar >> nMapSize;
	
	BYTE nId;
	BOOL bState;
	for( int i=0; i<nMapSize; i++ )
	{
		ar >> nId;
		ar >> bState;
		prj.m_EventLua.SetState( nId, bState );
		if( bTextOut )
			g_UserMng.AddEventLua( nId, bState );
	}
#if __VER >= 12 // __EVENTLUA_0826
	prj.m_EventLua.PrepareProxy();
#endif	// __EVENTLUA_0826	
}

void CDPDatabaseClient::OnEventLuaChanged( CAr & ar, DPID, DPID )
{
	prj.m_EventLua.LoadScript();
	if( prj.m_EventLua.m_bRun )
		Error( "Event.lua 재실행 완료!!!" );
}
#endif // __EVENTLUA

#if __VER >= 11 // __GUILD_COMBAT_1TO1
void CDPDatabaseClient::OnGC1to1Open( CAr & ar, DPID, DPID )
{
	g_GuildCombat1to1Mng.GuildCombat1to1Open();
}

void CDPDatabaseClient::OnGC1to1TenderGuildFromDB( CAr & ar, DPID, DPID )
{
	int nVecGuildSize = 0, nVecMemberSize = 0;
	u_long uGuildId = 0;
	int nPenya = 0;
	u_long uPlayerId = 0;

	CGuildCombat1to1Mng::__GC1TO1TENDER gc1to1Tender;
	
	g_GuildCombat1to1Mng.m_vecTenderGuild.clear();
	ar >> nVecGuildSize;
	for( int i=0; i<nVecGuildSize; i++ )
	{
		ar >> uGuildId >> nPenya;
		gc1to1Tender.ulGuildId = uGuildId;
		gc1to1Tender.nPenya = nPenya;
		if( g_eLocal.GetState( EVE_GUILDCOMBAT1TO1 ) )
			g_GuildCombat1to1Mng.m_vecTenderGuild.push_back( gc1to1Tender );
		ar >> nVecMemberSize;
		for( int j=0; j<nVecMemberSize; j++ )
		{
			ar >> uPlayerId;
			if( g_eLocal.GetState( EVE_GUILDCOMBAT1TO1 ) )
				g_GuildCombat1to1Mng.m_vecTenderGuild[i].vecMemberId.push_back( uPlayerId );
		}
	}

	g_GuildCombat1to1Mng.m_vecTenderFailGuild.clear();
	ar >> nVecGuildSize;
	for( i=0; i<nVecGuildSize; i++ )
	{
		ar >> uGuildId >> nPenya;
		gc1to1Tender.ulGuildId = uGuildId;
		gc1to1Tender.nPenya = nPenya;
		if( g_eLocal.GetState( EVE_GUILDCOMBAT1TO1 ) )
			g_GuildCombat1to1Mng.m_vecTenderFailGuild.push_back( gc1to1Tender );
	}
}
#endif // __GUILD_COMBAT_1TO1

void CDPDatabaseClient::CalluspLoggingQuest( u_long idPlayer, int nQuest, int nState, const char* pszComment )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CALL_USPLOGGINGQUEST, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << nQuest << nState;
	ar.WriteString( pszComment );
	SEND( ar, this, DPID_SERVERPLAYER );
}

#ifdef __INVALID_LOGIN_0320
void CDPDatabaseClient::CalluspXXXMultiServer( u_long uKey, CUser* pUser )
#else	// __INVALID_LOGIN_0320
void CDPDatabaseClient::CalluspXXXMultiServer( u_long uKey, u_long idPlayer )
#endif	// __INVALID_LOGIN_0320
{
	BEFORESENDDUAL( ar, PACKETTYPE_CALL_XXX_MULTI_SERVER, DPID_UNKNOWN, DPID_UNKNOWN );
#ifdef __INVALID_LOGIN_0320
	if( pUser )
	{
		ar << uKey << pUser->m_idPlayer;
		ar.WriteString( pUser->m_playAccount.lpszAccount );
	}
	else
	{
		ar << uKey << 0;
		ar.WriteString( "" );	// not used
	}
#else	// __INVALID_LOGIN_0320
	ar << uKey << idPlayer;
#endif	// __INVALID_LOGIN_0320
	SEND( ar, this, DPID_SERVERPLAYER );
}

#if __VER >= 9	// __PET_0410
void CDPDatabaseClient::CalluspPetLog( u_long idPlayer, SERIALNUMBER iSerial, DWORD dwData, int nType, CPet* pPet )
{
	if( pPet == NULL )
		return;
	BEFORESENDDUAL( ar, PACKETTYPE_CALL_USP_PET_LOG, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << iSerial << dwData << nType;
	pPet->Serialize( ar );
	SEND( ar, this, DPID_SERVERPLAYER );
}
#endif	// __PET_0410

#if __VER >= 9 // __EVENTLUA
void CDPDatabaseClient::SendEventLuaChanged( void )
{
	BEFORESENDDUAL( ar, PACKETTYPE_EVENTLUA_CHANGED, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_SERVERPLAYER );
}
#endif // __EVENTLUA

#if __VER >= 11 // __GUILD_COMBAT_1TO1
void CDPDatabaseClient::SendGC1to1StateToDBSrvr( void )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GC1TO1_STATETODB, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << g_GuildCombat1to1Mng.m_nState;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendGC1to1Tender( char cGU, u_long uGuildId, int nPenya, char cState )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GC1TO1_TENDERTODB, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << cGU << uGuildId << nPenya << cState;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendGC1to1LineUp( u_long uGuildId, vector<u_long>& vecMemberId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GC1TO1_LINEUPTODB, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uGuildId;
	ar << vecMemberId.size();
	for( int i=0; i<(int)( vecMemberId.size() ); i++ )
		ar << vecMemberId[i];
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendGC1to1WarPerson( u_long uGuildId, u_long uIdPlayer, char cState )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GC1TO1_WARPERSONTODB, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uGuildId << uIdPlayer << cState;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendGC1to1WarGuild( DWORD dwWorldId, u_long uGuildId_0, u_long uGuildId_1, char cState )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GC1TO1_WARGUILDTODB, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwWorldId << uGuildId_0 << uGuildId_1 << cState;
	SEND( ar, this, DPID_SERVERPLAYER );
}
#endif // __GUILD_COMBAT_1TO1


#if __VER >= 11 // __MA_VER11_04	// 길드 창고 로그 기능 world,database,neuz

void CDPDatabaseClient::SendQueryGetGuildBankLogList( u_long idReceiver, DWORD idGuild, BYTE byListType )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILDLOG_VIEW, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver << idGuild << byListType << g_uIdofMulti/*uQuery*/;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnGuildBankLogViewFromDB( CAr & ar, DPID, DPID )
{
	u_long idReceiver;
	int	nCount = 0;
	BYTE	byListType = 0;
	ar >> nCount;
	ar >> byListType;	
	ar >> idReceiver;	

	if( nCount > GUILDBANKLOGVIEW_MAX )
		return;

	CUser* pUser	= g_UserMng.GetUserByPlayerID( idReceiver );
	if( IsValidObj( pUser ) )
	{
		__GUILDBANKLOG_ENTRY	logs[GUILDBANKLOGVIEW_MAX];
		for(int i = 0; i < nCount ; ++i)
		{
			ar.ReadString( logs[i].szPlayer, 42 );		// 
			ar >> logs[i].nDate;					// 
			ar >> logs[i].nItemID;					// 
			ar >> logs[i].nItemAbilityOption;					// 
			ar >> logs[i].nItemCount;					// 
		}
		pUser->AddGuildBankLogView( byListType, (short)nCount, logs);
	}
}

#endif //__MA_VER11_04	// 길드 창고 로그 기능 world,database,neuz
#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz

void CDPDatabaseClient::SendQueryGetSealChar( u_long idReceiver ,const char* szAccount)
{
	BEFORESENDDUAL( ar, PACKETTYPE_SEALCHAR_REQ, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver << g_uIdofMulti/*uQuery*/;
	ar.WriteString( szAccount );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendQueryGetSealCharConm( u_long idReceiver )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SEALCHARCONM_REQ, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver << g_uIdofMulti/*uQuery*/;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnSealCharFromDB( CAr & ar, DPID, DPID )
{
	u_long idReceiver;
	int	nCount = 0;
	ar >> nCount;
	ar >> idReceiver;	

	CUser* pUser	= g_UserMng.GetUserByPlayerID( idReceiver );
	if( IsValidObj( pUser ) )
	{
		__SEALCHAR_ENTRY	seals[SEALCHAR_MAX];
		for(int i = 0; i < nCount ; ++i)
		{
			ar >> seals[i].nPlayerSlot;					// 
			ar >> seals[i].idPlayer;					// 
			ar.ReadString( seals[i].szPlayer, 42 );		// 
		}

		if(nCount < 2 )
			pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CHARSEND );
		else
			pUser->AddSealChar( (short)nCount, seals);
	}
}

void CDPDatabaseClient::SendQueryGetSealCharGet( u_long idReceiver ,const char* szAccount,DWORD dwGetId)
{
	BEFORESENDDUAL( ar, PACKETTYPE_SEALCHARGET_REQ, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver << g_uIdofMulti/*uQuery*/;
	ar.WriteString( szAccount );
	ar << dwGetId;
	SEND( ar, this, DPID_SERVERPLAYER );
}
void CDPDatabaseClient::SendQueryGetSealCharSet( u_long idReceiver ,const char* szAccount,LONG lSetPlayerSlot,DWORD dwSetPlayerId)
{
	BEFORESENDDUAL( ar, PACKETTYPE_SEALCHARSET_REQ, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver << g_uIdofMulti/*uQuery*/;
	ar.WriteString( szAccount );
	ar << lSetPlayerSlot;
	ar << dwSetPlayerId;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnSealCharGetFromDB( CAr & ar, DPID, DPID )
{
	u_long idReceiver;
	int	nCount = 0;
	LONG	lSetSlot = -1;
	DWORD dwGetID;
	ar >> nCount;
	ar >> idReceiver;
	ar >> dwGetID;

	CUser* pUser	= g_UserMng.GetUserByPlayerID( idReceiver );
	if( IsValidObj( pUser ) )
	{
		__SEALCHAR_ENTRY	seals[SEALCHAR_MAX];
		for(int i = 0; i < nCount ; ++i)
		{
			ar >> seals[i].nPlayerSlot;					// 
			ar >> seals[i].idPlayer;					// 
			ar.ReadString( seals[i].szPlayer, 42 );		// 
		}

		if( nCount > 2 || nCount < 1)
		{
			pUser->AddDefinedText( TID_GAME_SEALCHAR_NO_CHARSEND );
			g_DPSrvr.QueryDestroyPlayer( pUser->m_Snapshot.dpidCache, pUser->m_Snapshot.dpidUser, pUser->m_dwSerial, pUser->m_idPlayer ); // pUser->m_Snapshot.dpidUser에는 소켓번호가 들어가 있다.
		}
		else
		{
			if( pUser->IsUsableState( dwGetID ) == FALSE )
				return;

			CItemElem* pItemElem = pUser->m_Inventory.GetAtId( dwGetID );
			if( IsUsableItem( pItemElem ) )
			{
				if(pItemElem->m_dwItemId != II_SYS_SYS_SCR_SEALCHARACTER )
					return;
				if( nCount == 1 )
				{
					if(seals[0].nPlayerSlot == 0)
						lSetSlot = 1;
					else if(seals[0].nPlayerSlot == 1 || seals[0].nPlayerSlot == 2 )
						lSetSlot = 0;
				}
				else if(nCount == 2)
				{
					if(seals[0].nPlayerSlot == 0)
					{
						if(seals[1].nPlayerSlot == 1)
							lSetSlot = 2;
						else if(seals[1].nPlayerSlot == 2)
							lSetSlot = 1;
					}
					else if(seals[0].nPlayerSlot == 1 )
					{
						if(seals[1].nPlayerSlot == 0)
							lSetSlot = 2;
						else if(seals[1].nPlayerSlot == 2)
							lSetSlot = 0;
					}
					else if(seals[0].nPlayerSlot == 2 )
					{
						if(seals[1].nPlayerSlot == 0)
							lSetSlot = 1;
						else if(seals[1].nPlayerSlot == 1)
							lSetSlot = 0;
					}
				}
				g_dpDBClient.SendQueryGetSealCharSet( pUser->m_idPlayer,pUser->m_playAccount.lpszAccount,lSetSlot,pItemElem->m_nHitPoint);
				pUser->AddSealCharSet();

				ItemProp* pItemProp  = pItemElem->GetProp();
		
				g_dpDBClient.SendLogSMItemUse( "1", pUser, pItemElem, pItemProp );		
//				pItemElem->UseItem();
				OBJID       dwTmpObjId = pItemElem->m_dwObjId;
				pUser->RemoveItem( (BYTE)( dwTmpObjId ), (short)1 );
//		pUser->UpdateItem( pItemElemtmp->m_dwObjId, UI_NUM, pItemElemtmp->m_nItemNum );
//		g_UserMng.DestroyPlayer( pUser );
//		g_UserMng.RemoveUser( pUser->m_dwSerial );
//		g_DPCoreClient.SendKillPlayer( pUser->m_idPlayer, pUser->m_idPlayer );
				g_DPSrvr.QueryDestroyPlayer( pUser->m_Snapshot.dpidCache, pUser->m_Snapshot.dpidUser, pUser->m_dwSerial, pUser->m_idPlayer ); // pUser->m_Snapshot.dpidUser에는 소켓번호가 들어가 있다.
			}
		}
	}
}
#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz

#if __VER >= 12 // __LORD
void CDPDatabaseClient::OnElectionAddDeposit( CAr & ar, DPID, DPID )
{
	u_long idPlayer;
	__int64 iDeposit;
	time_t tCreate;
	BOOL bRet;
	ar >> idPlayer >> iDeposit >> tCreate >> bRet;

	CUser* pUser	= static_cast<CUser*>( prj.GetUserByID( idPlayer ) );
	if( IsValidObj( pUser ) )
		pUser->SetQuerying( FALSE );

	if( bRet )
	{
		IElection* pElection	= CSLord::Instance()->GetElection();
		pElection->AddDeposit( idPlayer, iDeposit, tCreate );
	}
	else
	{
		// *Restore
		if( IsValidObj( pUser ) )
			pUser->AddGold( static_cast<int>( iDeposit ) );
	}
}

void CDPDatabaseClient::OnElectionSetPledge( CAr & ar, DPID, DPID )
{
	u_long idPlayer;
	char szPledge[CCandidate::nMaxPledgeLen]	= {0, };
	BOOL bRet;
	ar >> idPlayer;
	ar.ReadString( szPledge, CCandidate::nMaxPledgeLen );
	ar >> bRet;

	CUser* pUser	= static_cast<CUser*>( prj.GetUserByID( idPlayer ) );
	if( IsValidObj( pUser ) )
		pUser->SetQuerying( FALSE );

	IElection* pElection	= CSLord::Instance()->GetElection();
	if( bRet )
	{
		pElection->SetPledge( idPlayer, szPledge );
	}
	else
	{
		// *Restore
		if( pElection->HasPledge( idPlayer ) )
		{
			if( IsValidObj( pUser ) )
				pUser->AddGold( static_cast<int>( CCandidate::nSetPledgeCost ) );
		}
	}
}

void CDPDatabaseClient::OnElectionIncVote( CAr & ar, DPID, DPID )
{
	u_long idPlayer, idElector;
	BOOL bRet;
	ar >> idPlayer >> idElector >> bRet;

	CUser* pUser	= g_UserMng.GetUserByPlayerID( idElector );
	if( IsValidObj( pUser ) )
		pUser->SetQuerying( FALSE );

	if( bRet )
	{
		IElection* pElection	= CSLord::Instance()->GetElection();
		pElection->IncVote( idPlayer, idElector );
		if( IsValidObj( pUser ) )	//
		{
			pUser->SetElection( pElection->GetId() );
			// 투표 참여 버프
			ItemProp* pProp		= prj.GetItemProp( II_SYS_SYS_VOTE_THANKS );
			if( pProp )
				pUser->DoApplySkill( pUser, pProp, NULL );
		}
	}
	else
	{
		// *print
		if( IsValidObj( pUser ) )
			pUser->AddDefinedText( TID_GAME_ELECTION_INC_VOTE_E004 );	// 이미 투표에 참여하셨습니다.
	}
}

void CDPDatabaseClient::OnElectionBeginCandidacy( CAr & ar, DPID, DPID )
{
	IElection* pElection	= CSLord::Instance()->GetElection();
	pElection->BeginCandidacy();
}

void CDPDatabaseClient::OnElectionBeginVote( CAr & ar, DPID, DPID )
{
	int nRequirement;
	ar >> nRequirement;

	IElection* pElection	= CSLord::Instance()->GetElection();
	pElection->BeginVote( nRequirement );
}

void CDPDatabaseClient::OnElectionEndVote( CAr & ar, DPID, DPID )
{
	u_long idPlayer;
	ar >> idPlayer;

	IElection* pElection	= CSLord::Instance()->GetElection();
	pElection->EndVote( idPlayer );
}

void CDPDatabaseClient::OnLord( CAr & ar, DPID, DPID )
{
	election::OutputDebugString( "CDPDatabaseClient.OnLord" );
	CSLord::Instance()->Serialize( ar );
}

void CDPDatabaseClient::OnLEventCreate( CAr & ar, DPID, DPID )
{
	election::OutputDebugString( "CDPDatabaseClient.OnLEventCreate" );
	CLEComponent* pComponent	= new CLEComponent;
	pComponent->Serialize( ar );
	BOOL bResult;
	ar >> bResult;

	CUser* pUser	= g_UserMng.GetUserByPlayerID( pComponent->GetIdPlayer() );
	if( IsValidObj( pUser ) )
		pUser->SetQuerying( FALSE );

	if( bResult )
	{
		ILordEvent* pEvent		= CSLord::Instance()->GetEvent();
		pEvent->AddComponent( pComponent );
	}
	else
		safe_delete( pComponent );
}

void CDPDatabaseClient::OnLEventInitialize( CAr & ar, DPID, DPID )
{
	election::OutputDebugString( "CDPDatabaseClient.OnLEventInitialize" );
	ILordEvent* pEvent		= CSLord::Instance()->GetEvent();
	pEvent->Initialize();
}

void CDPDatabaseClient::OnLordSkillUse( CAr & ar, DPID, DPID )
{
	election::OutputDebugString( "CDPDatabaseClient.OnLordSkillUse" );
	u_long idPlayer, idTarget;
	int nSkill, nRet;
	ar >> idPlayer >> idTarget >> nSkill >> nRet;

	CUser* pUser	= g_UserMng.GetUserByPlayerID( idPlayer );
	if( !nRet )
	{
		CLordSkill* pSkills		= CSLord::Instance()->GetSkills();
		CLordSkillComponentExecutable* pComponent	= pSkills->GetSkill( nSkill );
		pComponent->Execute( idPlayer, idTarget, NULL );
		if( IsValidObj( pUser ) )
			g_UserMng.AddLordSkillUse( pUser, idTarget, nSkill );
	}
	else
	{
		CUser* pUser	= g_UserMng.GetUserByPlayerID( idPlayer );
		if( IsValidObj( pUser ) )
			pUser->AddDefinedText( nRet );
	}
}

void CDPDatabaseClient::OnLordSkillTick( CAr & ar, DPID, DPID )
{
	election::OutputDebugString( "CDPDatabaseClient.OnLordSkillTick" );

	CLordSkill* pSkills		= CSLord::Instance()->GetSkills();
	pSkills->SerializeTick( ar );
	if( CSLord::Instance()->Get() == NULL_ID )
		return;
	CUser* pLord	= g_UserMng.GetUserByPlayerID( CSLord::Instance()->Get() );
	if( IsValidObj( pLord ) )
		pLord->AddLordSkillTick( pSkills );
}

void CDPDatabaseClient::OnLEventTick( CAr & ar, DPID, DPID )
{
	election::OutputDebugString( "CDPDatabaseClient.OnLEventTick" );
	ILordEvent* pEvent		= CSLord::Instance()->GetEvent();
	pEvent->SerializeTick( ar );
	g_UserMng.AddLEventTick( pEvent );
	pEvent->EraseExpiredComponents();
}

void CDPDatabaseClient::SendElectionAddDeposit( u_long idPlayer, __int64 iDeposit )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ELECTION_ADD_DEPOSIT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << iDeposit;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendElectionSetPledge( u_long idPlayer, const char* szPledge )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ELECTION_SET_PLEDGE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer;
	ar.WriteString( szPledge );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendElectionIncVote( u_long idPlayer, u_long idElector )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ELECTION_INC_VOTE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << idElector;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendLEventCreate( u_long idPlayer, int iEEvent, int iIEvent )
{
	BEFORESENDDUAL( ar, PACKETTYPE_L_EVENT_CREATE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << iEEvent << iIEvent;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendLordSkillUse( u_long idPlayer, u_long idTarget, int nSkill )
{
	BEFORESENDDUAL( ar, PACKETTYPE_LORD_SKILL_USE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << idTarget << nSkill;
	SEND( ar, this, DPID_SERVERPLAYER );
}

// operator commands
void CDPDatabaseClient::SendLEventInitialize( void )
{
	BEFORESENDDUAL( ar, PACKETTYPE_L_EVENT_INITIALIZE, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendElectionProcess( BOOL bRun )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ELECTION_PROCESS, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << bRun;
	SEND( ar, this, DPID_SERVERPLAYER );
}
void CDPDatabaseClient::SendElectionBeginCandidacy( void )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ELECTION_BEGIN_CANDIDACY, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_SERVERPLAYER );
}
void CDPDatabaseClient::SendElectionBeginVote( void )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ELECTION_BEGIN_VOTE, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_SERVERPLAYER );
}
void CDPDatabaseClient::SendElectionBeginEndVote( void )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ELECTION_END_VOTE, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_SERVERPLAYER );
}
#endif	// __LORD

#if __VER >= 12 // __TAX
void CDPDatabaseClient::SendSecretRoomWinGuild( BYTE nCont, DWORD dwGuildId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_TAX_SET_SECRETROOM_WINNER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nCont << dwGuildId;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendLord( DWORD dwIdPlayer )
{
	BEFORESENDDUAL( ar, PACKETTYPE_TAX_SET_LORD, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwIdPlayer;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendTaxRate( BYTE nCont, int nSalesTaxRate, int nPurchaseTaxRate )
{
	BEFORESENDDUAL( ar, PACKETTYPE_TAX_SET_TAXRATE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nCont;
	ar << nSalesTaxRate << nPurchaseTaxRate;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendApplyTaxRateNow()
{
	BEFORESENDDUAL( ar, PACKETTYPE_TAX_APPLY_TAXRATE_NOW, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnTaxInfo( CAr & ar, DPID, DPID )
{
	CTax::GetInstance()->Serialize( ar );
	
	BOOL bConnect, bToAllClient;
	ar >> bConnect;
	if( bConnect )
	{
		ar >> CTax::GetInstance()->m_nMinTaxRate;
		ar >> CTax::GetInstance()->m_nMaxTaxRate;
#ifdef __OCCUPATION_SHOPITEM
		size_t nSize;
		ar >> nSize;
		for( size_t i=0; i<nSize; i++ )
		{
			DWORD dwItemId;
			ar >> dwItemId;
			CTax::GetInstance()->m_vecdwOccupationShopItem.push_back( dwItemId );
		}
#endif // __OCCUPATION_SHOPITEM
	}
	ar >> bToAllClient;
	if( bToAllClient )	// 세율 변경 시점
	{
		g_UserMng.AddTaxInfo();
#if __VER >= 12 // __SECRET_ROOM
		if( g_eLocal.GetState( EVE_SECRETROOM ) )
			g_DPCoreClient.SendCaption( GETTEXT(TID_GAME_TAX_CHANGENEXT) );
#endif // __SECRET_ROOM
		// 사냥터용 비밀의 방에 있는 놈들을 쫓아낸다.
		CWorld* pWorld = g_WorldMng.GetWorld( WI_DUNGEON_SECRET_0 );
		if( !pWorld )
			return;

		CObj* pObj;
		for( int i=0; i<(int)( pWorld->m_dwObjNum ); i++ )
		{
			pObj	= pWorld->m_apObject[i];
			if( pObj && pObj->GetType() == OT_MOVER && ((CMover*)pObj)->IsPlayer() && !pObj->IsDelete() )
				((CUser*)pObj)->REPLACE( g_uIdofMulti, ((CUser*)pObj)->m_idMarkingWorld, ((CUser*)pObj)->m_vMarkingPos, REPLACE_NORMAL, nTempLayer );
		}
	}
}

void CDPDatabaseClient::SendAddTax( BYTE nCont, int nTax, BYTE nTaxKind )
{
	BEFORESENDDUAL( ar, PACKETTYPE_TAX_ADDTAX, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nCont << nTax << nTaxKind;
	SEND( ar, this, DPID_SERVERPLAYER );
}
#endif // __TAX

#if __VER >= 12 // __SECRET_ROOM
void CDPDatabaseClient::SendSecretRoomInsertToDB( BYTE nContinent, __SECRETROOM_TENDER & srTender )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SECRETROOM_TENDER_INSERTTODB, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nContinent << srTender.dwGuildId << srTender.nPenya;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendSecretRoomUpdateToDB( BYTE nContinent, __SECRETROOM_TENDER & srTender, char chState )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SECRETROOM_TENDER_UPDATETODB, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nContinent << srTender.dwGuildId << srTender.nPenya << chState;
	if( chState == 'W' || chState == 'L' )
		ar << srTender.dwWorldId << srTender.nWarState << srTender.nKillCount;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendSecretRoomInsertLineUpToDB( BYTE nContinent, __SECRETROOM_TENDER & srTender )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SECRETROOM_LINEUP_INSERTTODB, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nContinent << srTender.dwGuildId;
	ar << srTender.vecLineUpMember.size();
	for( int i=0; i<(int)( srTender.vecLineUpMember.size() ); i++ )
		ar << srTender.vecLineUpMember[i];
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendSecretRoomClosed()
{
	BEFORESENDDUAL( ar, PACKETTYPE_SECRETROOM_CLOSED, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnSecretRoomInfoClear( CAr & ar, DPID, DPID )
{
	CSecretRoomMng* pSRMng = CSecretRoomMng::GetInstance();
	map<BYTE, CSecretRoomContinent*>::iterator it = pSRMng->m_mapSecretRoomContinent.begin();
	for( ; it!=pSRMng->m_mapSecretRoomContinent.end(); it++ )
	{
   		CSecretRoomContinent* pSRCont = it->second;
		pSRCont->m_vecSecretRoomTender.clear();
	}
}

void CDPDatabaseClient::OnSecretRoomTenderInfo( CAr & ar, DPID, DPID )
{
	BYTE nContinent; DWORD dwGuildId, dwMemberId; int nPenya, nSize;
	vector<DWORD> vecMemberId;
	ar >> nContinent >> dwGuildId >> nPenya;
	ar >> nSize;
	for( int i=0; i<nSize; i++ )
	{
		ar >> dwMemberId;
		vecMemberId.push_back( dwMemberId );
	}

	if( !g_eLocal.GetState( EVE_SECRETROOM ) )
		return;

	CSecretRoomMng* pSRMng = CSecretRoomMng::GetInstance();
	map<BYTE, CSecretRoomContinent*>::iterator it = pSRMng->m_mapSecretRoomContinent.find( nContinent );
	if( it!=pSRMng->m_mapSecretRoomContinent.end() )
	{
		CSecretRoomContinent* pSRCont = it->second;
		__SECRETROOM_TENDER srTender;
		srTender.dwGuildId = dwGuildId;
		srTender.nPenya = nPenya;
		srTender.vecLineUpMember.assign( vecMemberId.begin(), vecMemberId.end() );
		pSRCont->m_vecSecretRoomTender.push_back( srTender );
	}
}
#endif // __SECRET_ROOM

#if __VER >= 13 // __RAINBOW_RACE
void CDPDatabaseClient::OnRainbowRaceInfo( CAr & ar, DPID, DPID )
{
	CRainbowRaceMng::GetInstance()->Clear();
	
	int nSize;
	DWORD dwPlayerId;
	
	// 신청자 목록을 받아서 등록한다.
	ar >> nSize;
	for( int i=0; i<nSize; i++ )
	{
		ar >> dwPlayerId;
		CRainbowRaceMng::GetInstance()->SetApplication( dwPlayerId );
	}

	// 이전 대회 랭킹 정보를 받아온다.
	vector<DWORD> vec_dwPrevRanking;
	ar >> nSize;
	for( i=0; i<nSize; i++ )
	{
		ar >> dwPlayerId;
		vec_dwPrevRanking.push_back( dwPlayerId );
	}
	CRainbowRaceMng::GetInstance()->SetPrevRanking( vec_dwPrevRanking );
}

void CDPDatabaseClient::SendRainbowRaceReqLoad()
{
	BEFORESENDDUAL( ar, PACKETTYPE_RAINBOWRACE_LOADDBTOWORLD, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendRainbowRaceApplication( DWORD dwPlayerId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_RAINBOWRACE_APPTODB, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwPlayerId;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendRainbowRaceFailedUser( DWORD dwPlayerId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_RAINBOWRACE_FAILEDTODB, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwPlayerId;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendRainbowRaceRanking( DWORD dwPlayerId, int nRanking )
{
	BEFORESENDDUAL( ar, PACKETTYPE_RAINBOWRACE_RANKINGTODB, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwPlayerId << nRanking;
	SEND( ar, this, DPID_SERVERPLAYER );
}
#endif // __RAINBOW_RACE

#if __VER >= 13 // __HOUSING
void CDPDatabaseClient::OnHousingLoadInfo( CAr & ar, DPID, DPID )
{
	DWORD dwPlayerId;
	ar >> dwPlayerId;

	CHousing* pHousing = CHousingMng::GetInstance()->GetHousing( dwPlayerId );
	if( !pHousing )
		pHousing = CHousingMng::GetInstance()->CreateHousing( dwPlayerId );
	else
		Error( "CDPDatabaseClient::OnHousingLoadInfo - %d", dwPlayerId );

	if( pHousing )
	{
		pHousing->Serialize( ar );
		CUser* pUser = static_cast<CUser*>( prj.GetUserByID( dwPlayerId ) );
		if( IsValidObj( pUser ) )
			pUser->AddHousingAllInfo( pHousing );
		pHousing->AddAllFurnitureControl();
	}
}

void CDPDatabaseClient::OnHousingSetFunitureList( CAr & ar, DPID, DPID )
{
	DWORD dwPlayerId;
	HOUSINGINFO housingInfo;
	BOOL bAdd;

	ar >> dwPlayerId;
	housingInfo.Serialize( ar );
	ar >> bAdd;

	CHousingMng::GetInstance()->SetFurnitureList( dwPlayerId, housingInfo, bAdd );
}

void CDPDatabaseClient::OnHousingSetupFuniture( CAr & ar, DPID, DPID )
{
	DWORD dwPlayerId;
	HOUSINGINFO housingInfo;
	ar >> dwPlayerId;
	housingInfo.Serialize( ar );

	CHousingMng::GetInstance()->SetupFurniture( dwPlayerId, housingInfo );
}

void CDPDatabaseClient::OnHousingSetVisitAllow( CAr & ar, DPID, DPID )
{
	DWORD dwPlayerId, dwTargetId;
	BOOL bAllow;
	ar >> dwPlayerId >> dwTargetId >> bAllow;

	CHousingMng::GetInstance()->SetVisitAllow( dwPlayerId, dwTargetId, bAllow );
}

void CDPDatabaseClient::OnHousingDBFailed( CAr & ar, DPID, DPID )
{
	DWORD dwPlayerId, dwItemId;
	ar >> dwPlayerId >> dwItemId;
	
	CHousing* pHousing = CHousingMng::GetInstance()->GetHousing( dwPlayerId );
	if( pHousing )	pHousing->Setting( FALSE );

	if( dwItemId != NULL_ID )
	{
		CUser* pUser = static_cast<CUser*>( prj.GetUserByID( dwPlayerId ) );
		if( IsValidObj( pUser ) )
		{
			CItemElem itemElem;
			itemElem.m_dwItemId = dwItemId;
			itemElem.m_nItemNum = 1;
			if( !pUser->CreateItem( &itemElem ) )
				Error( "CDPDatabaseClient::OnHousingDBFailed -> CreateItem Failed! - %d, %s, %d", dwPlayerId, pUser->GetName(), dwItemId );
		}
		else
			Error( "CDPDatabaseClient::OnHousingDBFailed -> Invalid User - %d, %d", dwPlayerId, dwItemId );
	}
}
#endif // __HOUSING

#if __VER >= 13 // __COUPLE_1117
void CDPDatabaseClient::SendPropose( u_long idProposer, u_long idTarget )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PROPOSE, DPID_UNKNOWN, DPID_UNKNOWN );  
	ar << idProposer << idTarget;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendCouple( u_long idProposer, u_long idTarget )
{
	BEFORESENDDUAL( ar, PACKETTYPE_COUPLE, DPID_UNKNOWN, DPID_UNKNOWN );  
	ar << idProposer << idTarget;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendDecouple( u_long idPlayer )
{
	BEFORESENDDUAL( ar, PACKETTYPE_DECOUPLE, DPID_UNKNOWN, DPID_UNKNOWN );  
	ar << idPlayer;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendClearPropose()
{
	BEFORESENDDUAL( ar, PACKETTYPE_CLEAR_PROPOSE, DPID_UNKNOWN, DPID_UNKNOWN );  
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnProposeResult( CAr & ar, DPID, DPID )
{
	CCoupleHelper::Instance()->OnProposeResult( ar );
}

void CDPDatabaseClient::OnCoupleResult( CAr & ar, DPID, DPID )
{
	CCoupleHelper::Instance()->OnCoupleResult( ar );
}

void CDPDatabaseClient::OnDecoupleResult( CAr & ar, DPID, DPID )
{
	CCoupleHelper::Instance()->OnDecoupleResult( ar );
}

void CDPDatabaseClient::OnCouple( CAr & ar, DPID, DPID )
{
	CCoupleHelper::Instance()->Serialize( ar );
}

#if __VER >= 13 // __COUPLE_1202
void CDPDatabaseClient::SendQueryAddCoupleExperience( u_long idPlayer, int nExperience )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ADD_COUPLE_EXPERIENCE, DPID_UNKNOWN, DPID_UNKNOWN );  
	ar << idPlayer << nExperience;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnAddCoupleExperience( CAr & ar, DPID, DPID )
{
	CCoupleHelper::Instance()->OnAddCoupleExperience( ar );
}
#endif	// __COUPLE_1202
#endif	// __COUPLE_1117

#ifdef __FUNNY_COIN
void CDPDatabaseClient::SendFunnyCoinReqUse( DWORD dwPlayerId, CItemElem* pItemElem )	// 퍼니코인을 사용했음을 TransServer에 알린다.
{
	BEFORESENDDUAL( ar, PACKETTYPE_FUNNYCOIN_REQ_USE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwPlayerId;
	ar << pItemElem->m_dwItemId << pItemElem->GetSerialNumber();
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnFunnyCoinAckUse( CAr & ar, DPID, DPID )	// 퍼니코인 사용에 대한 응답을 TranServer로 부터 받았다.
{
	CFunnyCoin::GetInstance()->OnFunnyCoinAckUse( ar );
}
#endif // __FUNNY_COIN
#if __VER >= 14 // __PCBANG
void CDPDatabaseClient::OnPCBangToggle( CAr & ar, DPID, DPID )
{
	BOOL bApply;
	ar >> bApply;
	CPCBang::GetInstance()->SetApply( bApply );
}
#endif // __PCBANG

#ifdef __VTN_TIMELIMIT
void CDPDatabaseClient::SendTimeLimitInfoReq( DWORD dwPlayerId, char* szAccount )
{
	BEFORESENDDUAL( ar, PACKETTYPE_TIMELIMIT_INFO, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwPlayerId;
	ar.WriteString( szAccount );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnTimeLimitInfoAck( CAr & ar, DPID, DPID )
{
	DWORD dwPlayerId;
	int nPlayTime;
	ar >> dwPlayerId >> nPlayTime;

	CUser* pUser = static_cast<CUser*>( prj.GetUserByID( dwPlayerId ) );
	if( IsValidObj( pUser ) )
	{
		pUser->m_nAccountPlayTime = nPlayTime;
		pUser->AddAccountPlayTime();

		//	mulcom	BEGIN100315	베트남 시간 제한
		if( ::GetLanguage() == LANG_VTN )
		{
			if( pUser->m_nAccountPlayTime != -1 )
			{
				pUser->AddBuff( BUFF_EQUIP, II_VIETNAM_BUFF01, 1, 999999999 );
			}
		}
		//	mulcom	END100315	베트남 시간 제한
	}
}

void CDPDatabaseClient::SendTimeLimitUpdateReq( char* szAccount, int nPlayTime )
{
	BEFORESENDDUAL( ar, PACKETTYPE_TIMELIMIT_UPDATE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar.WriteString( szAccount );
	ar << nPlayTime;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnTimeLimitReset( CAr & ar, DPID, DPID )
{
	g_UserMng.ResetAccountPlayTime();
}

#endif // __VTN_TIMELIMIT

#if __VER >= 14 // __INSTANCE_DUNGEON
void CDPDatabaseClient::SendLogInstanceDungeon( DWORD dwDungeonId, DWORD dwWorldId, u_long uChannel, int nDungeonType, char chState )
{
	BEFORESENDDUAL( ar, PACKETTYPE_INSTANCEDUNGEON_LOG, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwDungeonId << dwWorldId << uChannel << nDungeonType << chState;
	SEND( ar, this, DPID_SERVERPLAYER );
}
#endif // __INSTANCE_DUNGEON

#ifdef __QUIZ
void CDPDatabaseClient::OnQuizEventOpen( CAr & ar, DPID, DPID )
{
	CQuiz::GetInstance()->OpenQuizEvent();
}

void CDPDatabaseClient::OnQuizList( CAr & ar, DPID, DPID )
{
	CQuiz::QUIZLIST QL;
	QL.Serialize( ar );
	CQuiz::GetInstance()->MakeQuizList( QL );
	ar >> CQuiz::GetInstance()->m_nQuizSize;
}

void CDPDatabaseClient::OnQuizEventNotice( CAr & ar, DPID, DPID )
{
	CQuiz::GetInstance()->NoticeMessage();
}

void CDPDatabaseClient::OnQuizEventChanged( CAr & ar, DPID, DPID )
{
	CQuiz::GetInstance()->LoadScript();
	if( !CQuiz::GetInstance()->IsRun() )
		Error( "QuizEvent.lua 재실행 완료!!!" );
}

void CDPDatabaseClient::SendQuizEventOpen( int nType )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUIZ_OPEN, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nType;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendQuizEventState( int nState, int nChannel, int nWinnerCount, int nQuizCount )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUIZ_STATE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nState << nChannel << nWinnerCount << nQuizCount;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendQuizEventEntrance( u_long idPlayer, int nChannel )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUIZ_ENTRANCE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << nChannel;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendQuizEventSelect( u_long idPlayer, int nChannel, int nQuizId, int nSelect, int nAnswer )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUIZ_SELECT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << nChannel << nQuizId << nSelect << nAnswer;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendPostPrizeItem( u_long idPlayer, DWORD dwItemId, int nItemNum )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUIZ_PRIZEITEM, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << dwItemId << nItemNum;
	SEND( ar, this, DPID_SERVERPLAYER );
}
#endif // __QUIZ

#ifdef __ERROR_LOG_TO_DB
void CDPDatabaseClient::SendErrorLogToDB( CUser* pUser, char chType, LPCTSTR szError )
{
	if( !IsValidObj( pUser ) )
		return;
	BEFORESENDDUAL( ar, PACKETTYPE_ERROR_LOG_TO_DB, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << pUser->m_idPlayer;
	ar.WriteString( pUser->m_playAccount.lpszAccount );
	ar << g_uIdofMulti << chType;
	ar.WriteString( szError );
	SEND( ar, this, DPID_SERVERPLAYER );
}
#endif // __ERROR_LOG_TO_DB

#if __VER >= 15 // __GUILD_HOUSE
void CDPDatabaseClient::OnLoadGuildHouse( CAr & ar, DPID, DPID )
{
	GuildHouseMng->Serialize( ar );
}
void CDPDatabaseClient::OnBuyGuildHouse( CAr & ar, DPID, DPID )
{
	GuildHouseMng->OnBuyGuildHouse( ar );
}
void CDPDatabaseClient::OnRemoveGuildHouse( CAr & ar, DPID, DPID )
{
	GuildHouseMng->OnRemoveGuildHouse( ar );
}

void CDPDatabaseClient::OnGuildHousePacket( CAr & ar, DPID, DPID )
{
	BOOL bResult; DWORD dwGuildId;
	int nPacketType, nIndex;
	GH_Fntr_Info gfi;
	
	ar >> bResult >> dwGuildId;
	ar >> nPacketType >> nIndex;
	gfi.Serialize( ar );
	
	CGuildHouseBase* pGuildHouse = GuildHouseMng->GetGuildHouse( dwGuildId );
	if( pGuildHouse )
	{
		if( bResult )
		{
			if( pGuildHouse->OnGuildHousePacket( nPacketType, gfi, nIndex ) )
				pGuildHouse->SendWorldToClient( nPacketType, gfi, nIndex );
		}
		else	// 테스트용 코드
		{
			pGuildHouse->AfterFailedGuildHousePacket( nPacketType, gfi, nIndex );
		}
		pGuildHouse->SetWaitDBAckPlayerId( NULL_ID );
	}
}

void CDPDatabaseClient::SendLogGuildFurniture( DWORD dwGuildId, GH_Fntr_Info & gfi, char chState )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILDFURNITURE_LOG, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwGuildId;
	gfi.Serialize( ar );
	ar << chState;
	SEND( ar, this, DPID_SERVERPLAYER );
}

#ifdef __GUILD_HOUSE_MIDDLE
void CDPDatabaseClient::OnGuildHouseTenderInfo( CAr & ar, DPID, DPID )
{
	GuildHouseMng->OnGuildHouseTenderInfo( ar );
}

void CDPDatabaseClient::OnGuildHouseTenderJoin( CAr & ar, DPID, DPID )
{
	GuildHouseMng->OnGuildHouseTenderJoin( ar );
}

void CDPDatabaseClient::OnGuildHouseTenderState( CAr & ar, DPID, DPID )
{
	GuildHouseMng->OnGuildHouseTenderState( ar );
}

void CDPDatabaseClient::OnGuildHouseTenderResult( CAr & ar, DPID, DPID )
{
	GuildHouseMng->OnGuildHouseTenderResult( ar );
}

void CDPDatabaseClient::OnGuildHouseGradeUpdate( CAr & ar, DPID, DPID )
{
	GuildHouseMng->OnGuildHouseGradeUpdate( ar );
}

void CDPDatabaseClient::OnGuildHouseLevelUpdate( CAr & ar, DPID, DPID )
{
	GuildHouseMng->OnGuildHouseLevelUpdate( ar );
}

void CDPDatabaseClient::OnGuildHouseExpired( CAr & ar, DPID, DPID )
{
	GuildHouseMng->OnGuildHouseExpired( ar );
}
#endif // __GUILD_HOUSE_MIDDLE
#endif // __GUILD_HOUSE

#if __VER >= 15 // __CAMPUS
void CDPDatabaseClient::OnAllCampus( CAr & ar, DPID, DPID )
{
	CCampusHelper::GetInstance()->Serialize( ar );
}

void CDPDatabaseClient::OnAddCampusMember( CAr & ar, DPID, DPID )
{
	CCampusHelper::GetInstance()->OnAddCampusMember( ar );
}

void CDPDatabaseClient::OnRemoveCampusMember( CAr & ar, DPID, DPID )
{
	CCampusHelper::GetInstance()->OnRemoveCampusMember( ar );
}

void CDPDatabaseClient::OnUpdateCampusPoint( CAr & ar, DPID, DPID )
{
	CCampusHelper::GetInstance()->OnUpdateCampusPoint( ar );
}

void CDPDatabaseClient::SendAddCampusMember( u_long idMaster, int nMasterPoint, u_long idPupil, int nPupilPoint )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CAMPUS_ADD_MEMBER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idMaster << nMasterPoint << idPupil << nPupilPoint;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendRemoveCampusMember( u_long idCampus, u_long idPlayer )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CAMPUS_REMOVE_MEMBER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idCampus << idPlayer;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendUpdateCampusPoint( u_long idPlayer, int nCampusPoint, BOOL bAdd, char chState )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CAMPUS_UPDATE_POINT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << nCampusPoint << bAdd << chState;
	SEND( ar, this, DPID_SERVERPLAYER );
}

#endif // __CAMPUS