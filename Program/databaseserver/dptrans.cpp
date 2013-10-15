#include "stdafx.h"
#if __VER >= 11 // __SYS_PLAYER_DATA
#include "dpcoresrvr.h"
extern	CDPCoreSrvr	g_dpCoreSrvr;
#endif	// __SYS_PLAYER_DATA
#include "dpaccountclient.h"
extern	CDPAccountClient	g_dpAccountClient;
#include "dptrans.h"

#include "..\_network\objects\Obj.h"
#include "guild.h"
#include "post.h"
#include "spevent.h"

#if __VER >= 12 // __LORD
#include "tlord.h"
#endif	// __LORD
#if __VER >= 13 // __COUPLE_1117
#include "couplehelper.h"
#endif	// __COUPLE_1117

extern	APP_INFO	g_appInfo;

#if __VER >= 12 // __SECRET_ROOM
#include "SecretRoomDBMng.h"
#endif // __SECRET_ROOM
#if __VER >= 12 // __TAX
#include "Tax.h"
#endif // __TAX
#if __VER >= 13 // __RAINBOW_RACE
#include "RainbowRaceDBCtrl.h"
#endif // __RAINBOW_RACE
#if __VER >= 13 // __HOUSING
#include "HousingDBCtrl.h"
#endif // __HOUSING
#ifdef __FUNNY_COIN
#include "FunnyCoin.h"
#endif // __FUNNY_COIN
#ifdef __VTN_TIMELIMIT
#include "AccountTimeLimitDBCtrl.h"
#endif // __VTN_TIMELIMIT
#ifdef __QUIZ
#include "QuizDBCtrl.h"
#endif // __QUIZ
#if __VER >= 15 // __GUILD_HOUSE
#include "GuildHouseDBCtrl.h"
#endif // __GUILD_HOUSE
#if __VER >= 15 // __CAMPUS
#include "CampusDBCtrl.h"
#endif // __CAMPUS

CDPTrans::CDPTrans()
#if __VER >= 14 // __PCBANG
:m_bPCBangApply( TRUE )
#endif // __PCBANG
{
	BEGIN_MSG;
	ON_MSG( PACKETTYPE_JOIN, OnJoin );
	ON_MSG( PACKETTYPE_SAVE_PLAYER, OnSavePlayer ); 
	ON_MSG( PACKETTYPE_SAVE_CONCURRENT_USER_NUMBER, OnSaveConcurrentUserNumber );
#ifdef __S_NEW_SKILL_2
	ON_MSG( PACKETTYPE_SAVE_SKILL, OnSaveSkill );
#endif // __S_NEW_SKILL_2
	ON_MSG( PACKETTYPE_LOG_PLAY_CONNECT, OnLogPlayConnect );
	ON_MSG( PACKETTYPE_PREVENT_LOGIN, OnPreventLogin );
	ON_MSG( PACKETTYPE_LOG_PLAY_DEATH, OnLogPlayDeath );
	ON_MSG( PACKETTYPE_LOG_LEVELUP, OnLogLevelUp );
	ON_MSG( PACKETTYPE_LOG_SERVER_DEATH, OnLogServerDeath );
	ON_MSG( PACKETTYPE_LOG_UNIQUEITEM, OnLogUniqueItem );
	ON_MSG( PACKETTYPE_LOG_ALLITEM, OnLogItem );
	ON_MSG( PACKETTYPE_LOG_QUEST, OnLogQuest );
	ON_MSG( PACKETTYPE_CHANGEBANKPASS, OnChangeBankPass );
	ON_MSG( PACKETTYPE_LOG_GAMEMASTER_CHAT, OnGammaChat );
	ON_MSG( PACKETTYPE_LOG_PK_PVP, OnLogPkPvp );
	ON_MSG( PACKETTYPE_LOG_SCHOOL, OnLogSchool );
	ON_MSG( PACKETTYPE_SKILLPOINTLOG, OnLogSkillPoint );
	ON_MSG( PACKETTYPE_GUILD_BANK_QUERY, OnQueryGuildBank);
	ON_MSG( PACKETTYPE_GUILD_BANK_UPDATE, OnGuildBankUpdate);
	ON_MSG( PACKETTYPE_GUILD_DB_REALPENYA, OnGuildRealPay );
	ON_MSG( PACKETTYPE_WD_GUILD_CONTRIBUTION, OnGuildContribution );

	ON_MSG( PACKETTYPE_UPDATE_GUILD_RANKING, OnUpdateGuildRanking );
	ON_MSG( PACKETTYPE_UPDATE_GUILD_RANKING_DB, OnUpdateGuildRankingDB );

	ON_MSG( PACKETTYPE_SCHOOL_REPORT, OnSchoolReport );
	ON_MSG( PACKETTYPE_BUYING_INFO, OnBuyingInfo );

	ON_MSG( PACKETTYPE_QUERYGUILDQUEST, OnQueryGuildQuest );
	ON_MSG( PACKETTYPE_INSERTGUILDQUEST, OnInsertGuildQuest );
	ON_MSG( PACKETTYPE_UPDATEGUILDQUEST, OnUpdateGuildQuest );
	ON_MSG( PACKETTYPE_QUERYSETPLAYERNAME, OnQuerySetPlayerName );
	ON_MSG( PACKETTYPE_ITEM_TBL_UPDATE, OnItemTBLUpdate );
	ON_MSG( PACKETTYPE_PING, OnPing );
	ON_MSG( PACKETTYPE_QUERYMAILBOX, OnQueryMailBox );


	//////////////////////////////////////////////////////////////////////////
	ON_MSG( PACKETTYPE_QUERYMAILBOX_COUNT, OnQueryMailBoxCount );
	ON_MSG( PACKETTYPE_QUERYMAILBOX_REQ, OnQueryMailBoxReq );
	//////////////////////////////////////////////////////////////////////////
	

	ON_MSG( PACKETTYPE_QUERYPOSTMAIL, OnQueryPostMail );
	ON_MSG( PACKETTYPE_QUERYREMOVEMAIL, OnQueryRemoveMail );
	ON_MSG( PACKETTYPE_QUERYGETMAILITEM, OnQueryGetMailItem );
	ON_MSG( PACKETTYPE_QUERYGETMAILGOLD, OnQueryGetMailGold );
	ON_MSG( PACKETTYPE_READMAIL, OnReadMail );

	ON_MSG( PACKETTYPE_START_GUILDCOMBAT, OnStartGuildCombat )
	ON_MSG( PACKETTYPE_IN_GUILDCOMBAT, OnGuidCombatInGuild );
	ON_MSG( PACKETTYPE_OUT_GUILDCOMBAT, OnGuidCombatOutGuild );
	ON_MSG( PACKETTYPE_RESULT_GUILDCOMBAT, OnResultGuildCombat );
	ON_MSG( PACKETTYPE_GETPENYAGUILD_GUILDCOMBAT, OnGCGetPenyaGuild );
	ON_MSG( PACKETTYPE_GETPENYAPLAYER_GUILDCOMBAT, OnGCGetPenyaPlayer );
	ON_MSG( PACKETTYPE_CONTINUE_GUILDCOMBAT, OnGCContinue );

	ON_MSG( PACKETTYPE_QUERY_REMOVE_GUILD_BANK_TBL, OnQueryRemoveGuildBankTbl );
	ON_MSG( PACKETTYPE_CALL_USPLOGGINGQUEST, OnCalluspLoggingQuest );
	ON_MSG( PACKETTYPE_CALL_XXX_MULTI_SERVER, OnCalluspXXXMultiServer );
	ON_MSG( PACKETTYPE_LOG_EXPBOX, OnLogExpBox );

#if __VER >= 9	// __PET_0410
	ON_MSG( PACKETTYPE_CALL_USP_PET_LOG, OnCalluspPetLog );
#endif	// __PET_0410
	
#if __VER >= 9 // __EVENTLUA
	ON_MSG( PACKETTYPE_EVENTLUA_CHANGED, OnEventLuaChanged);
#endif // __EVENTLUA
#ifdef __S_RECOMMEND_EVE
	ON_MSG( PACKETTYPE_EVE_RECOMMEND, OnEveRecommend );
#endif // __S_RECOMMEND_EVE

#if __VER >= 11 // __GUILD_COMBAT_1TO1
	ON_MSG( PACKETTYPE_GC1TO1_STATETODB, OnGC1to1StateToDBSrvr );
	ON_MSG( PACKETTYPE_GC1TO1_TENDERTODB, OnGC1to1Tender );
	ON_MSG( PACKETTYPE_GC1TO1_LINEUPTODB, OnGC1to1LineUp );
	ON_MSG( PACKETTYPE_GC1TO1_WARPERSONTODB, OnGC1to1WarPerson );
	ON_MSG( PACKETTYPE_GC1TO1_WARGUILDTODB, OnGC1to1WarGuild );
#endif // __GUILD_COMBAT_1TO1

#if __VER >= 11 // __SYS_PLAYER_DATA
	ON_MSG( PACKETTYPE_UPDATE_PLAYER_DATA, OnUpdatePlayerData );
#endif	// __SYS_PLAYER_DATA

#if __VER >= 11 // __MA_VER11_04	// 길드 창고 로그 기능 world,database,neuz
	ON_MSG( PACKETTYPE_GUILDLOG_VIEW, OnGuildBankLogView );
#endif //__MA_VER11_04	// 길드 창고 로그 기능 world,database,neuz
#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
	ON_MSG( PACKETTYPE_SEALCHAR_REQ, OnSealChar );
	ON_MSG( PACKETTYPE_SEALCHARCONM_REQ, OnSealCharConm );
	ON_MSG( PACKETTYPE_SEALCHARGET_REQ, OnSealCharGet );
	ON_MSG( PACKETTYPE_SEALCHARSET_REQ, OnSealCharSet );
#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz

#if __VER >= 12 // __LORD
	// /군주입찰
	ON_MSG( PACKETTYPE_ELECTION_ADD_DEPOSIT, OnElectionAddDeposit );
	// /공약설정
	ON_MSG( PACKETTYPE_ELECTION_SET_PLEDGE, OnElectionSetPledge );
	// /군주투표
	ON_MSG( PACKETTYPE_ELECTION_INC_VOTE, OnElectionIncVote );
	// /군주이벤트시작
	ON_MSG( PACKETTYPE_L_EVENT_CREATE,	OnLEventCreate );
	// /군주스킬
	ON_MSG( PACKETTYPE_LORD_SKILL_USE,	OnLordSkillUse );
	// /군주프로세스
	ON_MSG( PACKETTYPE_ELECTION_PROCESS, OnElectionProcess );
	// /입후보시작
	ON_MSG( PACKETTYPE_ELECTION_BEGIN_CANDIDACY, OnElectionBeginCandidacy );
	// /투표시작
	ON_MSG( PACKETTYPE_ELECTION_BEGIN_VOTE, OnElectionBeginVote );
	// /투표종료
	ON_MSG( PACKETTYPE_ELECTION_END_VOTE, OnElectionEndVote );
	// /군주이벤트초기화
	ON_MSG( PACKETTYPE_L_EVENT_INITIALIZE,	OnLEventInitialize );
#endif	// __LORD

#if __VER >= 12 // __TAX
	ON_MSG( PACKETTYPE_TAX_SET_SECRETROOM_WINNER, OnSetSecretRoomWinGuild );
	ON_MSG( PACKETTYPE_TAX_SET_LORD, OnSetLord );
	ON_MSG( PACKETTYPE_TAX_SET_TAXRATE, OnTaxRate );
	ON_MSG( PACKETTYPE_TAX_ADDTAX, OnAddTax );
	ON_MSG( PACKETTYPE_TAX_APPLY_TAXRATE_NOW, OnApplyTaxRateNow );
#endif // __TAX

#if __VER >= 12 // __SECRET_ROOM
	ON_MSG( PACKETTYPE_SECRETROOM_TENDER_INSERTTODB, OnSecretRoomInsertToDB );
	ON_MSG( PACKETTYPE_SECRETROOM_TENDER_UPDATETODB, OnSecretRoomUpdateToDB );
	ON_MSG( PACKETTYPE_SECRETROOM_LINEUP_INSERTTODB, OnSecretRoomLineUpToDB );
	ON_MSG( PACKETTYPE_SECRETROOM_CLOSED, OnSecretRoomClosed );
#endif // __SECRET_ROOM

#if __VER >= 13 // __RAINBOW_RACE
	ON_MSG( PACKETTYPE_RAINBOWRACE_LOADDBTOWORLD, OnRainbowRaceLoadInfo );
	ON_MSG( PACKETTYPE_RAINBOWRACE_APPTODB, OnRainbowRaceApplication );
	ON_MSG( PACKETTYPE_RAINBOWRACE_FAILEDTODB, OnRainbowRaceFailedUser );
	ON_MSG( PACKETTYPE_RAINBOWRACE_RANKINGTODB, OnRainbowRaceRanking );
#endif // __RAINBOW_RACE

#if __VER >= 13 // __HOUSING
	ON_MSG( PACKETTYPE_HOUSING_LOADINFO, OnHousingLoadInfo );
	ON_MSG( PACKETTYPE_HOUSING_FURNITURELIST, OnHousingReqSetFurnitureList );
	ON_MSG( PACKETTYPE_HOUSING_SETUPFURNITURE, OnHousingReqSetupFurniture );
	ON_MSG( PACKETTYPE_HOUSING_SETVISITALLOW, OnHousingReqSetVisitAllow );
	ON_MSG( PACKETTYPE_HOUSING_GM_REMOVEALL, OnHousingReqGMRemoveAll );
#endif // __HOUSING

#if __VER >= 13 // __COUPLE_1117
	ON_MSG( PACKETTYPE_PROPOSE, OnPropose );
	ON_MSG( PACKETTYPE_COUPLE, OnCouple );
	ON_MSG( PACKETTYPE_DECOUPLE, OnDecouple );
	ON_MSG( PACKETTYPE_CLEAR_PROPOSE, OnClearPropose );
#if __VER >= 13 // __COUPLE_1202
	ON_MSG( PACKETTYPE_ADD_COUPLE_EXPERIENCE, OnQueryAddCoupleExperience );
#endif	// __COUPLE_1202
#endif	// __COUPLE_1117
#if __VER >= 13 // __HONORABLE_TITLE			// 달인
	ON_MSG( PACKETTYPE_LOG_GETHONORTIME, OnLogGetHonorTime );
#endif	// __HONORABLE_TITLE			// 달인

#ifdef __FUNNY_COIN
	ON_MSG( PACKETTYPE_FUNNYCOIN_REQ_USE, OnFunnyCoinReqUse );
#endif // __FUNNY_COIN	

#ifdef __VTN_TIMELIMIT
	ON_MSG( PACKETTYPE_TIMELIMIT_INFO, OnTimeLimitInfoReq );
	ON_MSG( PACKETTYPE_TIMELIMIT_UPDATE, OnTimeLimitUpdate );
#endif // __VTN_TIMELIMIT

#if __VER >= 14 // __INSTANCE_DUNGEON
	ON_MSG( PACKETTYPE_INSTANCEDUNGEON_LOG, OnLogInstanceDungeon );
#endif // __INSTANCE_DUNGEON

#ifdef __QUIZ
	ON_MSG( PACKETTYPE_QUIZ_OPEN, OnQuizEventOpen );
	ON_MSG( PACKETTYPE_QUIZ_STATE, OnQuizEventState );
	ON_MSG( PACKETTYPE_QUIZ_ENTRANCE, OnQuizEventEntrance );
	ON_MSG( PACKETTYPE_QUIZ_SELECT, OnQuizEventSelect );
	ON_MSG( PACKETTYPE_QUIZ_PRIZEITEM, OnPostPrizeItem );
#endif // __QUIZ

#ifdef __ERROR_LOG_TO_DB
	ON_MSG( PACKETTYPE_ERROR_LOG_TO_DB, OnErrorLog );
#endif // __ERROR_LOG_TO_DB

#if __VER >= 15 // __GUILD_HOUSE
	ON_MSG( PACKETTYPE_GUILDHOUSE_BUY, OnBuyGuildHouse );
	ON_MSG( PACKETTYPE_GUILDHOUSE_PACKET, OnGuildHousePacket );
	ON_MSG( PACKETTYPE_GUILDFURNITURE_LOG, OnLogGuildFurniture );
#ifdef __GUILD_HOUSE_MIDDLE
	ON_MSG( PACKETTYPE_GUILDHOUSE_TENDER_JOIN, OnGuildHouseTenderJoin );
	ON_MSG( PACKETTYPE_GUILDHOUSE_LEVEL_UPDATE, OnGuildHouseLevelUpdate );
#endif // __GUILD_HOUSE_MIDDLE
#endif // __GUILD_HOUSE

#if __VER >= 15 // __CAMPUS
	ON_MSG( PACKETTYPE_CAMPUS_ADD_MEMBER, OnAddCampusMember );
	ON_MSG( PACKETTYPE_CAMPUS_REMOVE_MEMBER, OnRemoveCampusMember );
	ON_MSG( PACKETTYPE_CAMPUS_UPDATE_POINT, OnUpdateCampusPoint );
#endif // __CAMPUS
}

CDPTrans::~CDPTrans()
{
}

CDPTrans* CDPTrans::GetInstance( void )
{
	static	CDPTrans sDPTrans;
	return &sDPTrans;
}

void CDPTrans::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
//	SYSTEMTIME st;

	switch( lpMsg->dwType )
	{
		case DPSYS_CREATEPLAYERORGROUP:
			{
				LPDPMSG_CREATEPLAYERORGROUP lpCreatePlayer	= (LPDPMSG_CREATEPLAYERORGROUP)lpMsg;
#if __VER >= 11 // __SYS_PLAYER_DATA
		#ifdef __JEFF_FIX_0
				LPDB_OVERLAPPED_PLUS pov	= g_DbManager.AllocRequest();
				pov->nQueryMode	= QM_ALL_PLAYER_DATA;
				pov->dpid	= lpCreatePlayer->dpId;
				PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &pov->Overlapped );
		#else	// __JEFF_FIX_0
				SendAllPlayerData( lpCreatePlayer->dpId );
		#endif	// __JEFF_FIX_0
#else	// __SYS_PLAYER_DATA
				SendAllPlayerID( lpCreatePlayer->dpId );
#endif	// __SYS_PLAYER_DATA

#if __VER >= 12 // __LORD
				// 월드 서버에 군주 정보 전송
				CTLord::Instance()->PostRequest( CTLord::eInit, NULL, 0, lpCreatePlayer->dpId );
#endif	// __LORD

#if __VER >= 13 // __COUPLE_1117
				CCoupleHelper::Instance()->PostRequest( CCoupleHelper::eTransfer, NULL, 0, lpCreatePlayer->dpId );
#endif	// __COUPLE_1117

#ifdef __S1108_BACK_END_SYSTEM
				SendBaseGameSetting( TRUE, lpCreatePlayer->dpId );
				SendMonsterRespawnSetting( 0, NULL, lpCreatePlayer->dpId );
				SendMonsterPropSetting( TRUE, lpCreatePlayer->dpId );
#endif // __S1108_BACK_END_SYSTEM
				
				PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, lpCreatePlayer->dpId, NULL );

		#ifndef __JEFF_FIX_0
				SendAllMail( lpCreatePlayer->dpId );
		#endif	// __JEFF_FIX_0

				SendEventGeneric( lpCreatePlayer->dpId );
#if __VER >= 9 // __EVENTLUA
				prj.m_EventLua.m_Access.Enter();
				vector<BYTE> vecEventList = prj.m_EventLua.GetEventList();
				prj.m_EventLua.m_Access.Leave();
				if( !vecEventList.empty() )
				{
					map<BYTE, BOOL> mapEventList;
					for( DWORD i=0; i<vecEventList.size(); i++ )
						mapEventList.insert( make_pair( vecEventList[i], TRUE ) );
					SendEventLuaState( mapEventList, FALSE, lpCreatePlayer->dpId );
				}
#endif// __EVENTLUA
#if __VER >= 11 // __GUILD_COMBAT_1TO1
				//g_DbManager.LoadGC1to1TenderGuild();
				LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
				lpDbOverlappedPlus->nQueryMode	= GC1TO1_LOAD;
				PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, lpCreatePlayer->dpId, &lpDbOverlappedPlus->Overlapped );
#endif // __GUILD_COMBAT_1TO1
			
#if __VER >= 12 // __SECRET_ROOM
				CSecretRoomDBMng::GetInstance()->PostRequest( QUERY_SECRETROOM_LOAD, NULL, 0, lpCreatePlayer->dpId );
#endif // __SECRET_ROOM
#if __VER >= 12 // __TAX
				CTax::GetInstance()->LoadTaxInfo( lpCreatePlayer->dpId );
#endif // __TAX
#if __VER >= 14 // __PCBANG
				SendPCBangSetApply( lpCreatePlayer->dpId );
#endif // __PCBANG
#if __VER >= 15 // __GUILD_HOUSE
				GuildHouseDBMng->PostRequest( GUILDHOUSE_SEND, NULL, 0, lpCreatePlayer->dpId );
#ifdef __GUILD_HOUSE_MIDDLE
				GuildHouseDBMng->PostRequest( GUILDHOUSE_SEND, NULL, 0, lpCreatePlayer->dpId );
#endif // __GUILD_HOUSE_MIDDLE
#endif // __GUILD_HOUSE
#if __VER >= 15 // __CAMPUS
				CCampusHelper::GetInstance()->PostRequest( CAMPUS_SEND, NULL, 0, lpCreatePlayer->dpId );
#endif // __CAMPUS
				break;
			}
		case DPSYS_DESTROYPLAYERORGROUP:
			{
				LPDPMSG_DESTROYPLAYERORGROUP lpDestroyPlayer	= (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
				OnRemoveConnection( lpDestroyPlayer->dpId );
#ifdef __QUIZ
				if( CQuiz::GetInstance()->IsRun() )
					CQuizDBMng::GetInstance()->PostRequest( QUERY_QUIZ_CLOSE, 0, 0, lpDestroyPlayer->dpId );
#endif // __QUIZ
				break;
			}
	}
}

void CDPTrans::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	static size_t	nSize	= sizeof(DPID);

	CAr ar( (LPBYTE)lpMsg + ( nSize + nSize ), dwMsgSize - ( nSize + nSize ) );
	GETTYPE( ar );
	void ( theClass::*pfn )( theParameters )	=	GetHandler( dw );
	
	if( pfn ) {
		( this->*( pfn ) )( ar, idFrom, *(UNALIGNED LPDPID)lpMsg, *(UNALIGNED LPDPID)( (LPBYTE)lpMsg + nSize ), (LPBYTE)lpMsg + nSize + nSize + nSize, dwMsgSize - ( nSize + nSize + nSize ) );
	}
	else {

	}
}

void CDPTrans::OnRemoveConnection( DPID dpid )
{

}

void CDPTrans::SendAllGuildCombat( DPID dpId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ADD_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN );
	g_DbManager.SerializeGuildCombat( ar );
	SEND( ar, this, dpId );
}

#if __VER >= 11 // __SYS_PLAYER_DATA
void CDPTrans::SendAllPlayerData( DPID dpid )
{
/*
//	BEFORESENDDUAL( ar, PACKETTYPE_ALL_PLAYER_DATA, DPID_UNKNOWN, DPID_UNKNOWN );
	CAr ar;
	ar.Reserve( 27002420 );
	int nBufSize;
	ar << DPID_UNKNOWN << DPID_UNKNOWN << PACKETTYPE_ALL_PLAYER_DATA;

	CPlayerDataCenter::GetInstance()->Serialize( ar );
	SEND( ar, this, dpid );
*/
	int nTotal	= CPlayerDataCenter::GetInstance()->size();
	int nFirst	= 0;
	int nCount	= 10000;
	while( nFirst < nTotal )
	{
		BEFORESENDDUAL( ar, PACKETTYPE_ALL_PLAYER_DATA, DPID_UNKNOWN, DPID_UNKNOWN );
		int nRet	= CPlayerDataCenter::GetInstance()->Serialize( ar, nFirst, nCount );
		nFirst	+= nRet;
		SEND( ar, this, dpid );
	}

//	char lpString[100]	= { 0,};
//	sprintf( lpString, "SendAllPlayerData: nTotal = %d, nFirst = %d", nTotal, nFirst );
//	OutputDebugString( lpString );
}

void CDPTrans::SendAddPlayerData( u_long idPlayer, PlayerData* pPlayerData )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ADD_PLAYER_DATA, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer;
	ar.WriteString( pPlayerData->szPlayer );
	ar.Write( &pPlayerData->data, sizeof(sPlayerData) );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendDeletePlayerData( u_long idPlayer )
{
	BEFORESENDDUAL( ar, PACKETTYPE_DELETE_PLAYER_DATA, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendUpdatePlayerData( u_long idPlayer, PlayerData* pPlayerData )
{
	BEFORESENDDUAL( ar, PACKETTYPE_UPDATE_PLAYER_DATA, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer;
	ar.Write( &pPlayerData->data, sizeof(sPlayerData) );
	SEND( ar, this, DPID_ALLPLAYERS );
}
#else	// __SYS_PLAYER_DATA
//{{AFX
void CDPTrans::SendAllPlayerID( DPID dpId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ALLPLAYERID, DPID_UNKNOWN, DPID_UNKNOWN );
	g_DbManager.SerializePlayerID( ar );
	SEND( ar, this, dpId );
}

void CDPTrans::SendPlayerID( u_long idPlayer, const CHAR* lpszPlayer )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PLAYERID, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer;
	ar.WriteString( lpszPlayer );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendRemovePlayerID( u_long idPlayer )
{
	BEFORESENDDUAL( ar, PACKETTYPE_REMOVEPLAYERID, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer;
	SEND( ar, this, DPID_ALLPLAYERS );
}
//}}AFX
#endif	// __SYS_PLAYER_DATA

void CDPTrans::SendHdr( DWORD dwHdr, DPID dpid )
{
	BEFORESENDDUAL( ar, dwHdr, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, dpid );
}

void CDPTrans::SendUpdateGuildRankFinish()
{
	BEFORESENDDUAL( ar, PACKETTYPE_UPDATE_GUILD_RANKING_END, DPID_UNKNOWN, DPID_UNKNOWN );
	CGuildRank::Instance()->Serialize( ar );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendBaseGameSetting( BOOL bFirst, DPID dpid )
{
#ifdef __S1108_BACK_END_SYSTEM
	BEFORESENDDUAL( ar, PACKETTYPE_BASEGAMESETTING, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << bFirst;
	ar << prj.m_bBaseGameSetting;
	if( prj.m_bBaseGameSetting )
	{
		ar << prj.m_fMonsterExpRate	<< prj.m_fGoldDropRate << prj.m_fItemDropRate << prj.m_fMonsterHitRate;
		ar << prj.m_fShopCost << prj.m_fMonsterRebirthRate << prj.m_fMonsterHitpointRate << prj.m_fMonsterAggressiveRate << prj.m_fMonsterRespawnRate;
	}
//	SEND( ar, this, DPID_ALLPLAYERS );
	SEND( ar, this, dpid );
#endif // __S1108_BACK_END_SYSTEM
}

void CDPTrans::SendMonsterRespawnSetting( int nRemoveSize, int nRemoveRespawn[], DPID dpid )
{
#ifdef __S1108_BACK_END_SYSTEM
	BEFORESENDDUAL( ar, PACKETTYPE_MONSTERRESPAWNSETTING, DPID_UNKNOWN, DPID_UNKNOWN );
	if( nRemoveRespawn == NULL )
	{
		ar << prj.m_nMonsterRespawnSize;
		for( DWORD i = 0 ; i < prj.m_nMonsterRespawnSize ; ++i )
		{
			ar << prj.m_aMonsterRespawn[ i ].nRespawnIndex;
			ar.WriteString( prj.m_aMonsterRespawn[ i ].szMonsterName );
			ar << prj.m_aMonsterRespawn[ i ].vPos;
			ar << prj.m_aMonsterRespawn[ i ].nQuantity;
			ar << prj.m_aMonsterRespawn[ i ].nAggressive;
			ar << prj.m_aMonsterRespawn[ i ].nRange;
			ar << prj.m_aMonsterRespawn[ i ].nTime;
		}
	}
	else
	{
		ar << prj.m_nAddMonsterRespawnSize;
		for( int i = 0 ; i < prj.m_nAddMonsterRespawnSize ; ++i )
		{
			ar << prj.n_aAddRespawn[ i ].nRespawnIndex;
			ar.WriteString( prj.n_aAddRespawn[ i ].szMonsterName );
			ar << prj.n_aAddRespawn[ i ].vPos;
			ar << prj.n_aAddRespawn[ i ].nQuantity;
			ar << prj.n_aAddRespawn[ i ].nAggressive;
			ar << prj.n_aAddRespawn[ i ].nRange;
			ar << prj.n_aAddRespawn[ i ].nTime;
		}
	}
	
	ar << nRemoveSize;
	for( int i = 0 ; i < nRemoveSize ; ++i )
	{
		ar << nRemoveRespawn[i];
	}

	SEND( ar, this, dpid );
#endif // __S1108_BACK_END_SYSTEM
}

void CDPTrans::SendMonsterPropSetting( BOOL bFirst, DPID dpid )
{
#ifdef __S1108_BACK_END_SYSTEM
	BEFORESENDDUAL( ar, PACKETTYPE_MONSTERPROPGAMESETTING, DPID_UNKNOWN, DPID_UNKNOWN );
	if( bFirst == TRUE )
	{
		ar << prj.m_nMonsterPropSize;
		for( DWORD i = 0 ; i < prj.m_nMonsterPropSize ; ++i )
		{
			ar.WriteString( prj.m_aMonsterProp[ i ].szMonsterName );
			ar << prj.m_aMonsterProp[ i ].nHitPoint;
			ar << prj.m_aMonsterProp[ i ].nAttackPower;
			ar << prj.m_aMonsterProp[ i ].nDefence;
			ar << prj.m_aMonsterProp[ i ].nExp;
			ar << prj.m_aMonsterProp[ i ].nItemDrop;
			ar << prj.m_aMonsterProp[ i ].nPenya;
		}
	}
	else
	{
		ar << prj.m_nAddMonsterPropSize;
		for( int i = 0 ; i < prj.m_nAddMonsterPropSize ; ++i )
		{
			ar.WriteString( prj.m_aAddProp[ i ].szMonsterName );
			ar << prj.m_aAddProp[ i ].nHitPoint;
			ar << prj.m_aAddProp[ i ].nAttackPower;
			ar << prj.m_aAddProp[ i ].nDefence;
			ar << prj.m_aAddProp[ i ].nExp;
			ar << prj.m_aAddProp[ i ].nItemDrop;
			ar << prj.m_aAddProp[ i ].nPenya;
		}
	}
	
	ar << prj.m_nRemoveMonsterPropSize;
	for( int i = 0 ; i < prj.m_nRemoveMonsterPropSize ; ++i )
	{
		ar.WriteString( prj.m_aRemoveProp[i] );
	}

	SEND( ar, this, dpid );
#endif // __S1108_BACK_END_SYSTEM
}

void CDPTrans::SendGMChat( int nCount )
{
#ifdef __S1108_BACK_END_SYSTEM
	BEFORESENDDUAL( ar, PACKETTYPE_GAMEMASTER_CHATTING, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nCount;
	for( int i = 0 ; i < nCount ; ++i )
	{
		ar.WriteString( prj.m_chGMChat[i] );
	}
	SEND( ar, this, DPID_ALLPLAYERS );
#endif // __S1108_BACK_END_SYSTEM
}

void CDPTrans::OnJoin( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	char lpOutputString[128]	= { 0, };
	sprintf( lpOutputString, "DATABASESERVER.EXE\t// PACKETTYPE_JOIN\t// %d\n", dpid );
	OutputDebugString( lpOutputString );

	CAr ar1;
	ar1 << PACKETTYPE_JOIN;
	ar1.Write( lpBuf, uBufSize );
	ar1 << dpid << dpidCache << dpidUser;
#ifdef __BILLING0712
	ar1 << (DWORD)0;	// retrieve dwBilling;
#endif	// __BILLING0712
#if __VER >= 14 // __PCBANG
	ar1 << (DWORD)0;
#endif // __PCBANG

	int nBlockSize;
	LPBYTE pBlock	= ar1.GetBuffer( &nBlockSize );
	g_dpAccountClient.Send( pBlock, nBlockSize, DPID_SERVERPLAYER );
}

void CDPTrans::OnSavePlayer( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CHAR sAccount[MAX_ACCOUNT]	= { 0, };
	DWORD dwWorldID;
	D3DXVECTOR3 vPos;
	BOOL bLogout = FALSE;
	CMover* pMover	= NULL;
	BYTE nSlot;
	ACCOUNT_CACHE*	pCache;

	ar >> bLogout;
	ar.ReadString( sAccount, MAX_ACCOUNT );
	ar >> nSlot;
	ar >> dwWorldID >> vPos;
#ifdef __LAYER_1015
	int nLayer;
	ar >> nLayer;
#endif	// __LAYER_1015

	g_DbManager.m_AddRemoveLock.Enter();
	pCache	= g_DbManager.m_AccountCacheMgr.Find( sAccount );
	if( pCache )
		pMover	= pCache->pMover[nSlot];

	if( pCache == NULL || pMover == NULL )
	{
		Error( "CDPTrans::OnSavePlayer - Cache Hit Failed, AccountCahce = %p, pMover = %p, Account = %s, slot = %d", pCache, pMover, sAccount, nSlot );
		g_DbManager.m_AddRemoveLock.Leave();
		return;
	}


	pCache->m_tmLastAccess	= ::time( NULL );

	pMover->Lock();
	pMover->Serialize( ar );
	pMover->m_fAngle	= 0.0f;
	pMover->m_UserTaskBar.Serialize( ar );
#ifdef __RT_1025
	DWORD dwState;
	ar >> dwState;
	pMover->m_RTMessenger.SetState( dwState );
#else	// __RT_1025
	pMover->m_Messenger.Serialize( ar );
#endif	// __RT_1025
#ifndef __JEFF_9_20
	ar >> pMover->m_dwFlyTime;
#endif	// __JEFF_9_20
	ar >> pMover->m_dwReturnWorldID;
	ar >> pMover->m_vReturnPos;

	pMover->m_dwWorldID		= dwWorldID;
	pMover->m_vPos	= vPos;
#ifdef __LAYER_1015
	pMover->SetLayer( nLayer );
#endif	// __LAYER_1015
	pMover->m_dwMode	&= ~FRESH_MODE;	// ata2k

#ifdef __EVENT_1101
#ifdef __EVENT_1101_2
	ar >> pMover->m_nEventFlag;
#else // __EVENT_1101_2
	ar >> pMover->m_dwEventFlag;
#endif // __EVENT_1101_2
	ar >> pMover->m_dwEventTime;
	ar >> pMover->m_dwEventElapsed;
#endif	// __EVENT_1101
#ifdef __EXP_ANGELEXP_LOG
	ar >> pMover->m_nExpLog;
	ar >> pMover->m_nAngelExpLog;
#endif // __EXP_ANGELEXP_LOG

#ifdef __EVENTLUA_COUPON
	ar >> pMover->m_nCoupon;
#endif // __EVENTLUA_COUPON

#if __VER >= 15 // __GUILD_HOUSE
	ar >> pMover->m_nRestPoint;
	pMover->m_tLogOut = time_null();
#endif // __GUILD_HOUSE

	for( int j = 0 ; j < 3 ; ++j )
	{
		pCache->m_Bank[j].Copy( pMover->m_Bank[j] );
		pCache->m_idPlayerBank[j] = pMover->m_idPlayerBank[j];
		pCache->m_dwGoldBank[j] = pMover->m_dwGoldBank[j];
	}

	if( bLogout )
		pMover->m_dwPeriodicTick	= 0;

//	u_long idPlayer		= pMover->m_idPlayer;
	CMover* pMover2	= new CMover;
	pMover2->Copy( pMover, TRUE );
	pMover->PeriodTick();

	pMover->Unlock();

	g_DbManager.m_AddRemoveLock.Leave();
#ifdef __INVALID_LOGIN_0612
	pMover2->m_bLogout	= bLogout;
	lstrcpy( pMover2->m_szAccount, sAccount );
#endif	// __INVALID_LOGIN_0612
#if __VER >= 15 // __GUILD_HOUSE
	CAr arRestPoint;
	arRestPoint << pMover2->m_idPlayer << pMover2->m_nRestPoint << pMover2->m_tLogOut;
	int nBufRestPointSize;	LPBYTE lpBufRestPoint = arRestPoint.GetBuffer( &nBufRestPointSize );
	GuildHouseDBMng->PostRequest( GUILDHOUSE_SAVE_RESTPOINT, lpBufRestPoint, nBufRestPointSize );
#endif // __GUILD_HOUSE
	
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPUpdate, (DWORD)pMover2, (DWORD)0, NULL );
}

#ifdef __S_NEW_SKILL_2
void CDPTrans::OnSaveSkill( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid	= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= SAVE_SKILL;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}
#endif // __S_NEW_SKILL_2

void CDPTrans::OnSaveConcurrentUserNumber( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
#ifdef __TOOMANY_PENDINGLOG1115
	if( CDbManager::IsTooManyPending() )
		return;
#endif	// __TOOMANY_PENDINGLOG1115
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );

	CAr arRead( lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid	= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= SAVECONCURRENTUSERNUMBER;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnLogPlayConnect( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
#ifdef __TOOMANY_PENDINGLOG1115
	if( CDbManager::IsTooManyPending() )
		return;
#endif	// __TOOMANY_PENDINGLOG1115
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid	= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= LOG_PLAY_CONNECT;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}


void CDPTrans::OnLogPlayDeath( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
#ifdef __TOOMANY_PENDINGLOG1115
	if( CDbManager::IsTooManyPending() )
		return;
#endif	// __TOOMANY_PENDINGLOG1115
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid	= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= LOG_PLAY_DEATH;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnLogLevelUp( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
#ifdef __TOOMANY_PENDINGLOG1115
	if( CDbManager::IsTooManyPending() )
		return;
#endif	// __TOOMANY_PENDINGLOG1115
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid	= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= LOG_LEVELUP;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnLogServerDeath( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
#ifdef __TOOMANY_PENDINGLOG1115
	if( CDbManager::IsTooManyPending() )
		return;
#endif	// __TOOMANY_PENDINGLOG1115
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid	= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= LOG_SERVER_DEATH;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}
#if __VER >= 13 // __HONORABLE_TITLE			// 달인
void	CDPTrans::OnLogGetHonorTime( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= LOG_GETHONORTIME;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}
#endif	// __HONORABLE_TITLE			// 달인

void CDPTrans::OnLogUniqueItem( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
#ifdef __TOOMANY_PENDINGLOG1115
	if( CDbManager::IsTooManyPending() )
		return;
#endif	// __TOOMANY_PENDINGLOG1115
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid	= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= LOG_UNIQUEITEM;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnLogItem( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
#ifdef __TOOMANY_PENDINGLOG1115
	if( CDbManager::IsTooManyPending() )
		return;
#endif	// __TOOMANY_PENDINGLOG1115
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid	= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= LOG_ITEM;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnLogQuest( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
#ifdef __TOOMANY_PENDINGLOG1115
	if( CDbManager::IsTooManyPending() )
		return;
#endif	// __TOOMANY_PENDINGLOG1115
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid	= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= LOG_QUEST;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );	
}

void CDPTrans::OnChangeBankPass( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid	= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= CHANGE_BANKPASS;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );	
}

void CDPTrans::OnGammaChat( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
#ifdef __TOOMANY_PENDINGLOG1115
	if( CDbManager::IsTooManyPending() )
		return;
#endif	// __TOOMANY_PENDINGLOG1115
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid	= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= GAMEMA_CHAT;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );		
}

void CDPTrans::OnLogPkPvp( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
#ifdef __TOOMANY_PENDINGLOG1115
	if( CDbManager::IsTooManyPending() )
		return;
#endif	// __TOOMANY_PENDINGLOG1115
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid	= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= LOG_PK_PVP;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnLogSchool( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
#ifdef __TOOMANY_PENDINGLOG1115
	if( CDbManager::IsTooManyPending() )
		return;
#endif	// __TOOMANY_PENDINGLOG1115
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid	= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= LOG_SCHOOL;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnLogSkillPoint( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid	= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= LOG_SKILLPOINT;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnQueryGuildBank( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid		= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= GUILD_BANK_QUERY;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnUpdateGuildRanking( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid		= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= GUILD_RANKING_UPDATE;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnUpdateGuildRankingDB( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid		= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= GUILD_RANKING_UPDATE_DB;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}


void CDPTrans::OnGuildBankUpdate( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid		= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= GUILD_BANK_UPDAATE;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnGuildRealPay( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	TRACE( "D CDPCoreSrvr::OnGuildRealPay\n" );
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid		= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= UPDATE_GUILD_REALPAY;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}
void CDPTrans::OnGuildContribution( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid		= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= UPDATE_GUILD_CONTRIBUTION;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnSchoolReport( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
#ifdef __TOOMANY_PENDINGLOG1115
	if( CDbManager::IsTooManyPending() )
		return;
#endif	// __TOOMANY_PENDINGLOG1115
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= SCHOOL_REPORT;
	PostQueuedCompletionStatus(  g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnBuyingInfo( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OutputDebugString( "DATABASESERVER.EXE\t// recv from world\n" );

	BUYING_INFO2 bi2;
	SERIALNUMBER iSerialNumber;
	ar.Read( (void*)&bi2, sizeof(BUYING_INFO2) );
	ar >> iSerialNumber;
	g_dpAccountClient.SendBuyingInfo( &bi2, iSerialNumber );
}

void CDPTrans::OnQueryGuildQuest( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid		= dpid;
	lpDbOverlappedPlus->nQueryMode	= QM_QUERYGUILDQUEST;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnInsertGuildQuest( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= QM_INSERTGUILDQUEST;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnUpdateGuildQuest( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= QM_UPDATEGUILDQUEST;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}


void CDPTrans::OnQuerySetPlayerName( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OutputDebugString( "DATABASESERVER.EXE\t// PACKETTYPE_QUERYSETPLAYERNAME\n" );
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= QM_SET_PLAYER_NAME;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnItemTBLUpdate( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	if( prj.m_bItemUpdate == FALSE )
		return;

	int nQuestCount;
	ar >> nQuestCount;

	int nQuestID;
	char szQuestName[64];
	for( int i = 0 ; i < nQuestCount ; ++i )
	{
		ZeroMemory( szQuestName, sizeof( szQuestName ) );
		ar >> nQuestID;
		ar.ReadString( szQuestName, 64 );
		g_DbManager.m_int2StrItemUpdate.insert( map<int, string>::value_type( nQuestID, szQuestName ) );
	}
	g_DbManager.m_nItemUpdate = 1;
}

void CDPTrans::OnPing( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	time_t tSend;
	ar >> tSend;

	{
		BEFORESENDDUAL( ar, PACKETTYPE_PING, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << tSend << time( NULL );
		SEND( ar, this, dpid );
	}
}

void CDPTrans::OnQueryMailBox( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
#ifdef __JEFF_FIX_0
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= QM_QUERY_MAIL_BOX;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
#else	// __JEFF_FIX_0
	u_long idReceiver;
	ar >> idReceiver;

	CPost *pPost    = CPost::GetInstance();

	CMclAutoLock	Lock( pPost->m_csPost );

	CMailBox* pMailBox	= NULL;
	pMailBox = pPost->GetMailBox( idReceiver );
	
	if( pMailBox != NULL )
	{
		SendMailBox( pMailBox, dpid );
	}
	else
	{
		SendMailBoxReq( idReceiver, dpid, FALSE, pMailBox );
	}

#endif	// __JEFF_FIX_0
}


void CDPTrans::OnQueryMailBoxCount( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long idReceiver;
	int nCount;
	ar >> idReceiver >> nCount;

	CPost *pPost    = CPost::GetInstance();

	CMclAutoLock	Lock( pPost->m_csPost );

	CMailBox* pMailBox	= NULL;
	pMailBox = pPost->GetMailBox( idReceiver );
	
	if( pMailBox != NULL )
	{
		SendMailBox( pMailBox, dpid );
	}
	else
	{
		SendMailBoxReq( idReceiver, dpid, FALSE, pMailBox );
	}

	return;
}

void CDPTrans::OnQueryMailBoxReq( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long idReceiver;
	ar >> idReceiver;

	CPost *pPost    = CPost::GetInstance();

	CMclAutoLock	Lock( pPost->m_csPost );

	CMailBox* pMailBox	= NULL;
	pMailBox = pPost->GetMailBox( idReceiver );

	BOOL	bHaveMailBox = FALSE;
	if( pMailBox != NULL )
	{
		bHaveMailBox = TRUE;
	}

	SendMailBoxReq( idReceiver, dpid, bHaveMailBox, pMailBox );

	return;
}

void CDPTrans::SendMailBox( CMailBox* pMailBox, DPID dpid )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUERYMAILBOX, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << pMailBox->m_idReceiver;
	pMailBox->Write( ar );
	SEND( ar, this, dpid );
}

void CDPTrans::SendMailBoxReq( u_long idReceiver, DPID dpid, BOOL bHaveMailBox, CMailBox* pMailBox )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUERYMAILBOX_REQ, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver;
	ar << bHaveMailBox;
	if( pMailBox != NULL )
	{
		pMailBox->Write( ar );
	}
	SEND( ar, this, dpid );
}

void CDPTrans::OnQueryPostMail( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= ADD_MAIL;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnQueryRemoveMail( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= REMOVE_MAIL;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnQueryGetMailItem( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= REMOVE_MAIL_ITEM;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnQueryGetMailGold( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= REMOVE_MAIL_GOLD;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnReadMail( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= READ_MAIL;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::SendPostMail( BOOL bResult, u_long idReceiver, CMail* pMail )
{
// 	//	BEGINTEST
// 	Error( "CDPTrans::SendPostMail Result[%d], Receiver[%d]", bResult, idReceiver );

	BEFORESENDDUAL( ar, PACKETTYPE_QUERYPOSTMAIL, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << bResult << idReceiver;
	pMail->Serialize( ar );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendRemoveMail( u_long idReceiver, u_long nMail )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUERYREMOVEMAIL, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver << nMail;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendGetMailItem( u_long idReceiver, u_long nMail, u_long uQuery )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUERYGETMAILITEM, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver << nMail << uQuery;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendGetMailGold( u_long idReceiver, u_long nMail, u_long uQuery )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUERYGETMAILGOLD, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver << nMail << uQuery;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendReadMail( u_long idReceiver, u_long nMail )
{
	BEFORESENDDUAL( ar, PACKETTYPE_READMAIL, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idReceiver << nMail;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendAllMail( DPID dpid )
{
//	BEFORESENDDUAL( ar, PACKETTYPE_ALLMAIL, DPID_UNKNOWN, DPID_UNKNOWN );

	CAr ar;
	ar.Reserve( 5100016 );

	int nBufSize;
	ar << DPID_UNKNOWN << DPID_UNKNOWN << PACKETTYPE_ALLMAIL;

	CPost::GetInstance()->m_csPost.Enter();
	CPost::GetInstance()->Serialize( ar, FALSE );
	CPost::GetInstance()->m_csPost.Leave();

	SEND( ar, this, dpid );
}

void CDPTrans::OnPreventLogin( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD dwPreventTime;
	char  szAccount[MAX_ACCOUNT]	= { 0, };

	ar.ReadString( szAccount, MAX_ACCOUNT );
	ar >> dwPreventTime;

	g_DbManager.m_AddRemoveLock.Enter();
	ACCOUNT_CACHE *pCache = g_DbManager.m_AccountCacheMgr.Find( szAccount );
	if( pCache )
		pCache->m_tmPrevent = time( NULL ) + (long)dwPreventTime;
	g_DbManager.m_AddRemoveLock.Leave();
}

void CDPTrans::OnGuidCombatInGuild( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= IN_GUILDCOMBAT;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnGuidCombatOutGuild( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= OUT_GUILDCOMBAT;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}
void CDPTrans::OnStartGuildCombat( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= START_GUILDCOMBAT;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}
void CDPTrans::OnResultGuildCombat( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= RESULT_GUILDCOMBAT;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}
void CDPTrans::OnGCGetPenyaGuild( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= GETPENYAGUILD_GUILDCOMBAT;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}
void CDPTrans::OnGCGetPenyaPlayer( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= GETPENYAPLAYER_GUILDCOMBAT;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}
void CDPTrans::OnGCContinue( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= CONTINUE_GUILDCOMBAT;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}
void CDPTrans::SendInGuildCombat( u_long idGuild, DWORD dwPenya, DWORD dwExistingPenya )
{
	BEFORESENDDUAL( ar, PACKETTYPE_IN_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idGuild;
	ar << dwPenya;
	ar << dwExistingPenya;
	SEND( ar, this, DPID_ALLPLAYERS );	
}
void CDPTrans::SendOutGuildCombat( u_long idGuild )
{
	BEFORESENDDUAL( ar, PACKETTYPE_OUT_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idGuild;
	SEND( ar, this, DPID_ALLPLAYERS );	
}
void CDPTrans::SendResultGuildCombat( void )
{
	BEFORESENDDUAL( ar, PACKETTYPE_RESULT_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << g_DbManager.m_nGuildCombatIndex;
	ar << g_DbManager.m_uWinGuildId;
	ar << g_DbManager.m_nWinGuildCount;
	ar << g_DbManager.m_uBestPlayer;
	g_DbManager.SerializeResultValue( ar );
	SEND( ar, this, DPID_ALLPLAYERS );	
}
void CDPTrans::SendPlayerPointGuildCombat( void )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PLAYERPOINT_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN );
	g_DbManager.SerializePlayerPoint( ar );
	SEND( ar, this, DPID_ALLPLAYERS );	
}
void CDPTrans::SendGetPenyaGuildGC( u_long uidPlayer, int nGuildCombatID, u_long uidGuild )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GETPENYAGUILD_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uidPlayer;
	ar << nGuildCombatID;
	ar << uidGuild;
	SEND( ar, this, DPID_ALLPLAYERS );	
}
void CDPTrans::SendGetPenyaPlayerGC( u_long uidPlayer, int nGuildCombatID, u_long uidGuild )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GETPENYAPLAYER_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uidPlayer;
	ar << nGuildCombatID;
	ar << uidGuild;
	SEND( ar, this, DPID_ALLPLAYERS );	
}

void CDPTrans::SendContinueGC()
{
	BEFORESENDDUAL( ar, PACKETTYPE_CONTINUE_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_ALLPLAYERS );	
}

void CDPTrans::SendQueryRemoveGuildBankTbl( int nNo, u_long idGuild, DWORD dwItemId, SERIALNUMBER iSerialNumber, DWORD dwItemNum )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUERY_REMOVE_GUILD_BANK_TBL, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nNo << idGuild << dwItemId << iSerialNumber << dwItemNum;
	SEND( ar, this, DPID_ALLPLAYERS );	
}

void CDPTrans::OnQueryRemoveGuildBankTbl( CAr & ar, DPID, DPID, DPID, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= QM_DELETE_REMOVE_GUILD_BANK_TBL;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::SendEventGeneric( DPID dpid )
{
	BEFORESENDDUAL( ar, PACKETTYPE_EVENT_GENERIC, DPID_UNKNOWN, DPID_UNKNOWN );
	CEventGeneric::GetInstance()->Serialize( ar );
	ar << CEventGeneric::GetInstance()->GetFlag();
	SEND( ar, this, dpid );
}

void CDPTrans::SendEventFlag( DWORD dwFlag )
{
	BEFORESENDDUAL( ar, PACKETTYPE_EVENT_FLAG, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwFlag;
	SEND( ar, this, DPID_ALLPLAYERS );
}

#if __VER >= 9 // __EVENTLUA
void CDPTrans::SendEventLuaState( map<BYTE, BOOL> mapState, BOOL bTextOut, DPID dpId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_EVENTLUA_STATE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << bTextOut;
	ar << mapState.size();
	for( map<BYTE, BOOL>::iterator it = mapState.begin(); it != mapState.end(); it++ )
		ar << it->first << it->second;
	SEND( ar, this, dpId );
}

void CDPTrans::SendEventLuaChanged( void )
{
	BEFORESENDDUAL( ar, PACKETTYPE_EVENTLUA_CHANGED, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::OnEventLuaChanged( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	prj.m_EventLua.m_Access.Enter();
	prj.m_EventLua.LoadScript();
	SendEventLuaChanged();
	if( prj.m_EventLua.CheckEventState() )
		SendEventLuaState( prj.m_EventLua.m_mapState, FALSE );
	prj.m_EventLua.m_Access.Leave();
}
#endif // __EVENTLUA

#ifdef __S_RECOMMEND_EVE
void CDPTrans::OnEveRecommend( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= RECOMMEND_EVE;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );	
}
#endif // __S_RECOMMEND_EVE

void CDPTrans::OnCalluspLoggingQuest( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= QM_CALL_USPLOGGINGQUEST;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnCalluspXXXMultiServer( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
#ifdef __INVALID_LOGIN_0320
	char	szAccount[MAX_ACCOUNT];
	u_long uKey, idPlayer;
	ar >> uKey >> idPlayer;
	ar.ReadString( szAccount, MAX_ACCOUNT );
	if( uKey > 0 )
	{
		if( idPlayer > 0 )
			g_DbManager.m_AccountCacheMgr.ChangeMultiServer( szAccount, uKey );
		else
			g_DbManager.m_AccountCacheMgr.InitMultiServer( uKey );
	}
	else
	{
#ifndef __INVALID_LOGIN_0612
		// 로그 아웃
		if( idPlayer > 0 )	// 수정 예정	// 康	// 접속중인 멀티 서버 번호를 초기화 하고 있지만, 실제로 아직 저장 전 가능성이 있다.
			g_DbManager.m_AccountCacheMgr.ChangeMultiServer( szAccount, 0 );
#endif	// __INVALID_LOGIN_0612
	}
#endif	// __INVALID_LOGIN_0320
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= QM_CALL_XXX_MULTI_SERVER;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );

#ifdef __RT_1025
	if( idPlayer > 0 )
	{
		CPlayerDataCenter::GetInstance()->m_Access.Enter();
		PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->GetPlayerData( idPlayer );
		if( pPlayerData )
		{
			pPlayerData->data.uLogin	= (BYTE)( uKey );
			pPlayerData->data.nVer++;
			SendUpdatePlayerData( idPlayer, pPlayerData );
			g_dpCoreSrvr.SendUpdatePlayerData( idPlayer, pPlayerData );
		}
		CPlayerDataCenter::GetInstance()->m_Access.Leave();
	}
#endif	// __RT_1025
}

void CDPTrans::OnLogExpBox( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= QM_LOG_EXPBOX;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

#if __VER >= 9	// __PET_0410
void CDPTrans::OnCalluspPetLog( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= QM_CALL_USP_PET_LOG;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}
#endif	// __PET_0410

#if __VER >= 11 // __GUILD_COMBAT_1TO1
void CDPTrans::SendGC1to1Open( void )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GC1TO1_OPEN, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendGC1to1TenderGuild( vector<CGuildCombat1to1Mng::__GC1TO1TENDER>& vecT, vector<CGuildCombat1to1Mng::__GC1TO1TENDER>& vecF, DPID dpId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GC1TO1_TENDERTOSRVR, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << vecT.size();
	for( DWORD i=0; i<vecT.size(); i++ )
	{
		ar << vecT[i].ulGuildId << vecT[i].nPenya;
		ar << vecT[i].vecMemberId.size();
		for( DWORD j=0; j<vecT[i].vecMemberId.size(); j++ )
			ar << vecT[i].vecMemberId[j];
	}

	ar << vecF.size();
	for( DWORD i=0; i<vecF.size(); i++ )
		ar << vecF[i].ulGuildId << vecF[i].nPenya;
	SEND( ar, this, dpId );
}

void CDPTrans::OnGC1to1StateToDBSrvr( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= GC1TO1_STATE;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnGC1to1Tender( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{	
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= GC1TO1_TENDER;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnGC1to1LineUp( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= GC1TO1_LINEUP;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnGC1to1WarPerson( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= GC1TO1_WARPERSON;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPTrans::OnGC1to1WarGuild( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= GC1TO1_WARGUILD;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}
#endif // __GUILD_COMBAT_1TO1

#if __VER >= 11 // __SYS_PLAYER_DATA
void CDPTrans::OnUpdatePlayerData( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	u_long idPlayer;
	sPlayerData	data;
	ar >> idPlayer;
	ar.Read( &data, sizeof(sPlayerData) );

	CPlayerDataCenter* pPlayerDataCenter	= CPlayerDataCenter::GetInstance();
	pPlayerDataCenter->m_Access.Enter();
	PlayerData* pPlayerData		= pPlayerDataCenter->GetPlayerData( idPlayer );
	if( !pPlayerData )
	{
		pPlayerDataCenter->m_Access.Leave();
		return;
	}
	
	pPlayerData->data.nJob	= data.nJob;
	pPlayerData->data.nLevel	= data.nLevel;
	pPlayerData->data.nSex	= data.nSex;
	pPlayerData->data.nVer++;
	PlayerData pd;
	memcpy( &pd, pPlayerData, sizeof(PlayerData) );
	pPlayerDataCenter->m_Access.Leave();
	SendUpdatePlayerData( idPlayer, &pd );
	g_dpCoreSrvr.SendUpdatePlayerData( idPlayer, &pd );
}
#endif	// __SYS_PLAYER_DATA

#if __VER >= 11 // __MA_VER11_04	// 길드 창고 로그 기능 world,database,neuz
void CDPTrans::OnGuildBankLogView( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= QM_GUILDBANK_LOG_VIEW;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}
#endif //__MA_VER11_04	// 길드 창고 로그 기능 world,database,neuz
#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
void CDPTrans::OnSealChar( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= QM_SEALCHAR;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}
void CDPTrans::OnSealCharConm( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= QM_SEALCHARCONM;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}
void CDPTrans::OnSealCharGet( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= QM_SEALCHARGET;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}
void CDPTrans::OnSealCharSet( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= QM_SEALCHARSET;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}
#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz

#if __VER >= 12 // __LORD
void CDPTrans::SendElectionAddDeposit( u_long idPlayer, __int64 iDeposit, time_t tCreate, BOOL bRet )
{	// 월드 서버에 군주입찰 결과 전송
	BEFORESENDDUAL( ar, PACKETTYPE_ELECTION_ADD_DEPOSIT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << iDeposit << tCreate << bRet;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendElectionSetPledge( u_long idPlayer, const char* szPledge, BOOL bRet )
{	// 월드 서버에 공약 설정 결과 전송
	BEFORESENDDUAL( ar, PACKETTYPE_ELECTION_SET_PLEDGE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer;
	ar.WriteString( szPledge );
	ar << bRet;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendElectionIncVote( u_long idPlayer, u_long idElector, BOOL bRet )
{	// 월드 서버에 투표 결과 전송
	BEFORESENDDUAL( ar, PACKETTYPE_ELECTION_INC_VOTE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << idElector << bRet;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendElectionBeginCandidacy( void )
{	// 월드 서버에 입후보 시작 상태 전송
	SendHdr( PACKETTYPE_ELECTION_BEGIN_CANDIDACY, DPID_ALLPLAYERS );
}

void CDPTrans::SendElectionBeginVote( int nRequirement )
{	// 월드 서버에 투표 시작 상태 전송
	BEFORESENDDUAL( ar, PACKETTYPE_ELECTION_BEGIN_VOTE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nRequirement;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendElectionEndVote( u_long idPlayer )
{	// 월드 서버에 투표 종료 상태 전송
	BEFORESENDDUAL( ar, PACKETTYPE_ELECTION_END_VOTE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::OnElectionAddDeposit( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{	// 군주 스레드에 군주 입찰 처리를 요청
	election::OutputDebugString( "CDPTrans::OnElectionAddDeposit()" );
	CTLord::Instance()->PostRequest( CTLord::eAddDeposit, lpBuf, uBufSize );
}

void CDPTrans::OnElectionSetPledge( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{	// 군주 스레드에 공약 설정 처리를 요청
	election::OutputDebugString( "CDPTrans::OnElectionSetPledge()" );
	CTLord::Instance()->PostRequest( CTLord::eSetPledge, lpBuf, uBufSize );
}

void CDPTrans::OnElectionIncVote( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{	// 군주 스레드에 투표 처리를 요청
	election::OutputDebugString( "CDPTrans::OnElectionIncVote()" );
	CTLord::Instance()->PostRequest( CTLord::eIncVote, lpBuf, uBufSize );
}

void CDPTrans::OnLEventCreate( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{	// 군주 스레드에 군주 이벤트 시작을 요청
	election::OutputDebugString( "CDPTrans::OnLEventCreate()" );
	CTLord::Instance()->PostRequest( CTLord::eLEventCreate, lpBuf, uBufSize );
}

void CDPTrans::OnLordSkillUse( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{	// 군주 스레드에 군주 스킬 사용을 요청
	election::OutputDebugString( "CDPTrans::OnLordSkillUse()" );
	CTLord::Instance()->PostRequest( CTLord::eLordSkillUse, lpBuf, uBufSize );
}

void CDPTrans::OnLEventInitialize( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{	// 군주 스레드에 군주 이벤트 초기화를 요청
	election::OutputDebugString( "CDPTrans::OnLEventInitialize()" );
	CTLord::Instance()->PostRequest( CTLord::eLEventInitialize, lpBuf, uBufSize );
}								 

// operator commands
void CDPTrans::OnElectionProcess( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{	// 군주 프로세스 제어 처리
	election::OutputDebugString( "CDPTrans::OnElectionProcess()" );
	BOOL bRun;
	ar >> bRun;
	CTElection* pElection	=static_cast<CTElection*>( CTLord::Instance()->GetElection() );
	pElection->SetRun( bRun );
}
void CDPTrans::OnElectionBeginCandidacy( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{	// 군주 스레드에 입후보 시작을 요청
	election::OutputDebugString( "CDPTrans::OnElectionBeginCandidacy()" );
	CTLord::Instance()->PostRequest( CTLord::eBeginCandidacy );
}
void CDPTrans::OnElectionBeginVote( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{	// 군주 스레드에 투표 시작을 요청
	election::OutputDebugString( "CDPTrans::OnElectionBeginVote()" );
	CTLord::Instance()->PostRequest( CTLord::eBeginVote );
}
void CDPTrans::OnElectionEndVote( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{	// 군주 스레드에 투표 종료를 요청
	election::OutputDebugString( "CDPTrans::OnElectionEndVote()" );
	CTLord::Instance()->PostRequest( CTLord::eEndVote );
}

void CDPTrans::SendLord( DPID dpid )
{	// 모든 군주 시스템 정보를 월드 서버에 전송
	BEFORESENDDUAL( ar, PACKETTYPE_LORD, DPID_UNKNOWN, DPID_UNKNOWN );
	CTLord::Instance()->Serialize( ar );
	SEND( ar, this, dpid );
}

void CDPTrans::SendLEventCreate( CLEComponent* pComponent, BOOL bResult )
{	// 월드 서버에 군주 이벤트 현재 상태를 전송
	BEFORESENDDUAL( ar, PACKETTYPE_L_EVENT_CREATE, DPID_UNKNOWN, DPID_UNKNOWN );
	pComponent->Serialize( ar );
	ar << bResult;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendLEventInitialize( void )
{	// 월드 서버에 군주 이벤트 초기화를 전송
	BEFORESENDDUAL( ar, PACKETTYPE_L_EVENT_INITIALIZE, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendLordSkillUse( u_long idPlayer, u_long idTarget, int nSkill, int nRet )
{	// 월드 서버에 군주 스킬 사용 결과를 전송
	// 결과가 참이면 월드 서버는 실제 군주 스킬 효과를 적용
	BEFORESENDDUAL( ar, PACKETTYPE_LORD_SKILL_USE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << idTarget << nSkill << nRet;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendLordSkillTick( CLordSkill* pSkills )
{	// 해당 군주 스킬의 재사용 대기 시간을 전송
	BEFORESENDDUAL( ar, PACKETTYPE_LORD_SKILL_TICK, DPID_UNKNOWN, DPID_UNKNOWN );
	pSkills->SerializeTick( ar );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendLEventTick( ILordEvent* pEvent )
{	// 해당 군주 이벤트의 남아있는 틱을 전송
	BEFORESENDDUAL( ar, PACKETTYPE_L_EVENT_TICK, DPID_UNKNOWN, DPID_UNKNOWN );
	pEvent->SerializeTick( ar );
	SEND( ar, this, DPID_ALLPLAYERS );
}
#endif	// __LORD

#if __VER >= 12 // __TAX
void CDPTrans::SendTaxInfo( DPID dpId, BOOL bConnect, BOOL bToAllClient )
{
	BEFORESENDDUAL( ar, PACKETTYPE_TAX_ALLINFO, DPID_UNKNOWN, DPID_UNKNOWN );
	CTax::GetInstance()->Serialize( ar );
	ar << bConnect;
	if( bConnect )
	{
		ar << CTax::GetInstance()->m_nMinTaxRate;
		ar << CTax::GetInstance()->m_nMaxTaxRate;
#ifdef __OCCUPATION_SHOPITEM
		ar << CTax::GetInstance()->m_vecdwOccupationShopItem.size();
		for( size_t i=0; i<CTax::GetInstance()->m_vecdwOccupationShopItem.size(); i++ )
			ar << CTax::GetInstance()->m_vecdwOccupationShopItem[i];
#endif // __OCCUPATION_SHOPITEM
	}
	ar << bToAllClient;
	SEND( ar, this, dpId );
}

void CDPTrans::OnSetSecretRoomWinGuild( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CTax::GetInstance()->m_taxDBController.PostRequest( QUERY_TAX_SETNEXT_SECRETROOMWINGUILD, lpBuf, uBufSize, 0 );
}

void CDPTrans::OnSetLord( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CTax::GetInstance()->m_taxDBController.PostRequest( QUERY_TAX_SETNEXT_LORD, lpBuf, uBufSize, 0 );
}

void CDPTrans::OnTaxRate( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CTax::GetInstance()->m_taxDBController.PostRequest( QUERY_TAX_SETNEXT_TAXRATE, lpBuf, uBufSize, 0 );
}

void CDPTrans::OnAddTax( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CTax::GetInstance()->m_taxDBController.PostRequest( QUERY_TAX_ADDTAX, lpBuf, uBufSize, 0 );
}

void CDPTrans::OnApplyTaxRateNow( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CTax::GetInstance()->SetApplyTaxRateNow();
}
#endif // __TAX

#if __VER >= 12 // __SECRET_ROOM
void CDPTrans::OnSecretRoomInsertToDB( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CSecretRoomDBMng::GetInstance()->PostRequest( QUERY_SECRETROOM_TENDER_INSERT, lpBuf, uBufSize );
}

void CDPTrans::OnSecretRoomUpdateToDB( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CSecretRoomDBMng::GetInstance()->PostRequest( QUERY_SECRETROOM_TENDER_UPDATE, lpBuf, uBufSize );
}

void CDPTrans::OnSecretRoomLineUpToDB( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CSecretRoomDBMng::GetInstance()->PostRequest( QUERY_SECRETROOM_LINEUP_INSERT, lpBuf, uBufSize );
}

void CDPTrans::OnSecretRoomClosed( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CSecretRoomDBMng::GetInstance()->PostRequest( QUERY_SECRETROOM_CLOSED, lpBuf, uBufSize );
}

void CDPTrans::SendSecretRoomInfoClear( DPID dpId )
{	
	BEFORESENDDUAL( ar, PACKETTYPE_SECRETROOM_INFO_CLEAR, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, dpId );
}

void CDPTrans::SendSecretRoomTenderInfo( BYTE nContinent, DWORD dwGuildId, int nPenya, vector<DWORD> & vecMemberId, DPID dpId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SECRETROOM_TENDERINFO_TO_WSERVER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nContinent << dwGuildId << nPenya;
	ar << vecMemberId.size();
	for( DWORD i=0; i<vecMemberId.size(); i++ )
		ar << vecMemberId[i];
	SEND( ar, this, dpId );
}
#endif // __SECRET_ROOM

#if __VER >= 13 // __RAINBOW_RACE
void CDPTrans::SendRainbowRaceInfo( vector<DWORD> & vec_dwNowPlayerId, vector<DWORD> & vec_prevRanking, DPID dpId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_RAINBOWRACE_LOADDBTOWORLD, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << vec_dwNowPlayerId.size();
	for( DWORD i=0; i<vec_dwNowPlayerId.size(); i++ )
		ar << vec_dwNowPlayerId[i];
	ar << vec_prevRanking.size();
	for( DWORD i=0; i<vec_prevRanking.size(); i++ )
		ar << vec_prevRanking[i];
	SEND( ar, this, dpId );
}

void CDPTrans::OnRainbowRaceLoadInfo( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CRainbowRaceDBMng::GetInstance()->PostRequest( QUERY_RR_LOAD, lpBuf, uBufSize, dpid );
}

void CDPTrans::OnRainbowRaceApplication( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CRainbowRaceDBMng::GetInstance()->PostRequest( QUERY_RR_APPLICATION, lpBuf, uBufSize );
}

void CDPTrans::OnRainbowRaceFailedUser( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CRainbowRaceDBMng::GetInstance()->PostRequest( QUERY_RR_FAILEDUSER, lpBuf, uBufSize );
}
void CDPTrans::OnRainbowRaceRanking( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CRainbowRaceDBMng::GetInstance()->PostRequest( QUERY_RR_RANKING, lpBuf, uBufSize );
}
#endif // __RAINBOW_RACE

#if __VER >= 13 // __HOUSING
void CDPTrans::SendHousingLoadInfo( DWORD dwPlayerId, CHousing* pHousing, DPID dpId )
{
	ASSERT( pHousing );
	BEFORESENDDUAL( ar, PACKETTYPE_HOUSING_LOADINFO, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwPlayerId;
	pHousing->Serialize( ar );
	SEND( ar, this, dpId );
}

void CDPTrans::SendHousingFurnitureList( DWORD dwPlayerId, HOUSINGINFO& housingInfo, BOOL bAdd, DPID dpId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_HOUSING_FURNITURELIST, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwPlayerId;
	housingInfo.Serialize( ar );
	ar << bAdd;
	SEND( ar, this, dpId );
}

void CDPTrans::SendHousingSetupFurniture( DWORD dwPlayerId, HOUSINGINFO housingInfo, DPID dpId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_HOUSING_SETUPFURNITURE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwPlayerId;
	housingInfo.Serialize( ar );
	SEND( ar, this, dpId );
}

void CDPTrans::SendHousingSetVisitAllow( DWORD dwPlayerId, DWORD dwTargetId, BOOL bAllow, DPID dpId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_HOUSING_SETVISITALLOW, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwPlayerId << dwTargetId << bAllow;
	SEND( ar, this, dpId );
}

void CDPTrans::SendHousingDBFailed( DWORD dwPlayerId, DWORD dwItemId, DPID dpId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_HOUSING_DBFAILED, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwPlayerId << dwItemId;
	SEND( ar, this, dpId );
}

void CDPTrans::OnHousingLoadInfo( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CHousingDBMng::GetInstance()->PostRequest( QUERY_HOUSING_LOAD, lpBuf, uBufSize, dpid );
}
void CDPTrans::OnHousingReqSetFurnitureList( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CHousingDBMng::GetInstance()->PostRequest( QUERY_HOUSING_INSERT_LIST, lpBuf, uBufSize, dpid );
}

void CDPTrans::OnHousingReqSetupFurniture( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CHousingDBMng::GetInstance()->PostRequest( QUERY_HOUSING_SETUP_FURNITURE, lpBuf, uBufSize, dpid );
}

void CDPTrans::OnHousingReqSetVisitAllow( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CHousingDBMng::GetInstance()->PostRequest( QUERY_HOUSING_SET_VISITALLOW, lpBuf, uBufSize, dpid );
}

void CDPTrans::OnHousingReqGMRemoveAll( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CHousingDBMng::GetInstance()->PostRequest( QUERY_HOUSING_GM_REMOVEALL, lpBuf, uBufSize );
}
#endif // __HOUSING

#if __VER >= 13 // __COUPLE_1117
void CDPTrans::OnPropose( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OutputDebugString( "\nCDPTrans.OnPropose\n" );
	CCoupleHelper::Instance()->PostRequest( CCoupleHelper::ePropose, lpBuf, uBufSize, dpid );
}

void CDPTrans::OnCouple( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OutputDebugString( "\nCDPTrans.OnCouple\n" );
	CCoupleHelper::Instance()->PostRequest( CCoupleHelper::eCouple, lpBuf, uBufSize );
}

void CDPTrans::OnDecouple( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OutputDebugString( "\nCDPTrans.OnDecouple\n" );
	CCoupleHelper::Instance()->PostRequest( CCoupleHelper::eDecouple, lpBuf, uBufSize );
}

void CDPTrans::OnClearPropose( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OutputDebugString( "\nCDPTrans.OnClearPropose\n" );
	CCoupleHelper::Instance()->PostRequest( CCoupleHelper::eClearPropose, lpBuf, uBufSize );
}

#if __VER >= 13 // __COUPLE_1202
void CDPTrans::OnQueryAddCoupleExperience( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	OutputDebugString( "\nCDPTrans.OnQueryAddCoupleExperience\n" );
	CCoupleHelper::Instance()->PostRequest( CCoupleHelper::eAddExperience, lpBuf, uBufSize );
}

void CDPTrans::SendAddCoupleExperience( u_long idPlayer, int nExperience )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ADD_COUPLE_EXPERIENCE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << nExperience;
	SEND( ar, this, DPID_ALLPLAYERS );
}
#endif	// __COUPLE_1202

void CDPTrans::SendCouple( CCoupleMgr* pMgr, DPID dpid )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ALL_COUPLES, DPID_UNKNOWN, DPID_UNKNOWN );
	CCoupleHelper::Instance()->Serialize( ar );
	SEND( ar, this, dpid );
}

void CDPTrans::SendProposeResult( u_long idProposer, u_long idTarget, int nResult, time_t t, DPID dpid )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PROPOSE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idProposer << idTarget << nResult << t;
	SEND( ar, this, dpid );
}

void CDPTrans::SendCoupleResult( u_long idProposer, u_long idTarget, int nResult )
{
	BEFORESENDDUAL( ar, PACKETTYPE_COUPLE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idProposer << idTarget << nResult;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendDecoupleResult( u_long idPlayer, int nResult )
{
	BEFORESENDDUAL( ar, PACKETTYPE_DECOUPLE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << nResult;
	SEND( ar, this, DPID_ALLPLAYERS );
}

#endif	// __COUPLE_1117

#ifdef __FUNNY_COIN
void CDPTrans::OnFunnyCoinReqUse( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CFunnyCoinDbMng::GetInstance()->PostRequest( CFunnyCoinDbCtrl::FC_USE, lpBuf, uBufSize, dpid );
}

void CDPTrans::SendFunnyCoinAckUse( DWORD dwPlayerId, DWORD dwItemId, SERIALNUMBER dwSerialNumber, int nResult, DPID dpId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_FUNNYCOIN_ACK_USE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwPlayerId << dwItemId << dwSerialNumber << nResult;
	SEND( ar, this, dpId );
}
#endif // __FUNNY_COIN

#if __VER >= 14 // __PCBANG
void CDPTrans::SendPCBangSetApply( DPID dpId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PCBANG_SETAPPLY, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << m_bPCBangApply;
	SEND( ar, this, dpId );
}
#endif // __PCBANG

#ifdef __VTN_TIMELIMIT
void CDPTrans::OnTimeLimitInfoReq( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CAccountTimeLimitDBMng::GetInstance()->PostRequest( QUERY_TIMELIMIT_LOAD, lpBuf, uBufSize, dpid );
}

void CDPTrans::SendTimeLimitAck( DWORD dwPlayerId, int nPlayTime, DPID dpId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_TIMELIMIT_INFO, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwPlayerId << nPlayTime;
	SEND( ar, this, dpId );
}

void CDPTrans::OnTimeLimitUpdate( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CAccountTimeLimitDBMng::GetInstance()->PostRequest( QUERY_TIMELIMIT_UPDATE, lpBuf, uBufSize, dpid );
}

void CDPTrans::SendTimeLimitReset()
{
	BEFORESENDDUAL( ar, PACKETTYPE_TIMELIMIT_RESET, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_ALLPLAYERS );
}
#endif // __VTN_TIMELIMIT

#if __VER >= 14 // __INSTANCE_DUNGEON
void CDPTrans::OnLogInstanceDungeon( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid	= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= LOG_INSTANCEDUNGEON;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );	
}
#endif // __INSTANCE_DUNGEON

#ifdef __QUIZ
void CDPTrans::OnQuizEventOpen( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CQuizDBMng::GetInstance()->PostRequest( QUERY_QUIZ_OPEN, lpBuf, uBufSize, dpid );
}

void CDPTrans::OnQuizEventState( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CQuizDBMng::GetInstance()->PostRequest( QUERY_QUIZ_STATE, lpBuf, uBufSize, dpid );
}

void CDPTrans::OnQuizEventEntrance( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CQuizDBMng::GetInstance()->PostRequest( QUERY_QUIZ_ENTRANCE, lpBuf, uBufSize, dpid );
}

void CDPTrans::OnQuizEventSelect( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CQuizDBMng::GetInstance()->PostRequest( QUERY_QUIZ_SELECT, lpBuf, uBufSize, dpid );
}

void CDPTrans::OnPostPrizeItem( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CQuizDBMng::GetInstance()->PostRequest( QUERY_QUIZ_POST_PRIZE, lpBuf, uBufSize, dpid );
}

void CDPTrans::SendQuizEventOpen( DPID dpId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUIZ_OPEN, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, dpId );
}

void CDPTrans::SendQuizList( DPID dpId, CQuiz::QUIZLIST & QL, int nSize )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUIZ_LOADQUIZ, DPID_UNKNOWN, DPID_UNKNOWN );
	QL.Serialize( ar );
	ar << nSize;
	SEND( ar, this, dpId );
}

void CDPTrans::SendQuizEventNotice( DPID dpId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUIZ_NOTICE, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, dpId );
}

void CDPTrans::SendQuizEventChanged()
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUIZ_CHANGED, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_ALLPLAYERS );
}
#endif // __QUIZ

#ifdef __ERROR_LOG_TO_DB
void CDPTrans::OnErrorLog( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid	= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= LOG_ERROR;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );	
}
#endif // __ERROR_LOG_TO_DB

#if __VER >= 15 // __GUILD_HOUSE
void CDPTrans::OnBuyGuildHouse( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	GuildHouseDBMng->PostRequest( GUILDHOUSE_CREATE, lpBuf, uBufSize, dpid );
}

void CDPTrans::OnGuildHousePacket( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	GuildHouseDBMng->PostRequest( GUILDHOUSE_DEFAULT_PACKET, lpBuf, uBufSize, dpid );
}

void CDPTrans::OnLogGuildFurniture( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid	= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= LOG_GUILDFURNITURE;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );	
}

#ifdef __GUILD_HOUSE_MIDDLE
void CDPTrans::OnGuildHouseTenderJoin( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	GuildHouseDBMng->PostRequest( GUILDHOUSE_TENDER_JOIN, lpBuf, uBufSize, dpid );
}

void CDPTrans::OnGuildHouseLevelUpdate( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	GuildHouseDBMng->PostRequest( GUILDHOUSE_LEVEL_UPDATE, lpBuf, uBufSize, dpid );
}
#endif // __GUILD_HOUSE_MIDDLE
#endif // __GUILD_HOUSE

#if __VER >= 15 // __CAMPUS
void CDPTrans::OnAddCampusMember( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CCampusHelper::GetInstance()->PostRequest( CAMPUS_ADD_MEMBER, lpBuf, uBufSize, dpid );
}

void CDPTrans::OnRemoveCampusMember( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CCampusHelper::GetInstance()->PostRequest( CAMPUS_REMOVE_MEMBER, lpBuf, uBufSize, dpid );
}

void CDPTrans::OnUpdateCampusPoint( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
	CCampusHelper::GetInstance()->PostRequest( CAMPUS_UPDATE_POINT, lpBuf, uBufSize, dpid );
}

void CDPTrans::SendAllCampus( DPID dpId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CAMPUS_ALL, DPID_UNKNOWN, DPID_UNKNOWN );
	CCampusHelper::GetInstance()->Serialize( ar );
	SEND( ar, this, dpId );
}

void CDPTrans::SendAddCampusMember( u_long idCampus, u_long idMaster, u_long idPupil )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CAMPUS_ADD_MEMBER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idCampus << idMaster << idPupil;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendRemoveCampusMember( u_long idCampus, u_long idPlayer )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CAMPUS_REMOVE_MEMBER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idCampus << idPlayer;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPTrans::SendUpdateCampusPoint( u_long idPlayer, int nCampusPoint )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CAMPUS_UPDATE_POINT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << nCampusPoint;
	SEND( ar, this, DPID_ALLPLAYERS );
}
#endif // __CAMPUS