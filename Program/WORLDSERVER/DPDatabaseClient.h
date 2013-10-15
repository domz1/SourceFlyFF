#ifndef __DPDATABASECLIENT_H__
#define __DPDATABASECLIENT_H__

#include "DPMng.h"
#include "MsgHdr.h"
#include "guild.h"

#include "eveschool.h"
#if __VER >= 12 // __SECRET_ROOM
#include "SecretRoom.h"
#endif // __SECRET_ROOM
#if __VER >= 15 // __GUILD_HOUSE
#include "GuildHouse.h"
#endif // __GUILD_HOUSE

#undef	theClass
#define theClass	CDPDatabaseClient
#undef theParameters
#define theParameters CAr & ar, DPID, DPID

class CUser;
class CDPDatabaseClient : public CDPMng
{
private:
	int		m_cbPing;
	BOOL	m_bAlive;

public:
//	Constructions
	CDPDatabaseClient();
	virtual	~CDPDatabaseClient();

//	Operations
	virtual	void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual	void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );

	void	SendITEM_TBL_Update( void );
#ifdef __LAYER_1015
	void	SavePlayer( CUser* pUser, DWORD dwWorldId, D3DXVECTOR3 & vPos, int nLayer, BOOL bLogout = FALSE );
#else	// __LAYER_1015
	void	SavePlayer( CUser* pUser, DWORD dwWorldId, const D3DXVECTOR3 & vPos, BOOL bLogout = FALSE );
#endif	// __LAYER_1015
	
#ifdef __S_NEW_SKILL_2
	void	SaveSkill( CUser* pUser );
#endif // __S_NEW_SKILL_2
	
#ifdef __S_RECOMMEND_EVE
	void	SendRecommend( CUser* pUser, int nValue = 0 );
#endif // __S_RECOMMEND_EVE
	void	SendLogConnect( CUser* pUser );
	void	SendLogPlayDeath( CMover* pMover, CMover* pSender );
	void	SendLogLevelUp( CMover* pSender, int Action );
	void	SendLogSkillPoint( int nAction, int nPoint, CMover* pMover, LPSKILL pSkill );
	void	SendLogItem( CUser* pUser, CMover* pSender );
	void	SendLogSMItemUse( const char *Action, CUser* pUser, CItemElem* pItemElem, ItemProp* pItemProp, const char* szName1 = "" );
	void	SendLogGamemaChat( CUser* pUser, LPCTSTR lpszString );
	void	SendLogUniqueItem(CMover* pMover, CItem* pItem, int nOption );
	void	SendLogUniqueItem2( CMover* pMover, ItemProp* pItemProp, int nOption );
	void	SendChangeBankPass( const char* szName, const char *szNewPass, u_long uidPlayer );
	void	SendLogPkPvp( CMover* pUser, CMover* pLose, int nPoint, char chState );
	void	SendLogSchool( u_long idPlayer, LPCTSTR szName );
	void	SendPing( void );
	void	SendInGuildCombat( u_long idGuild, DWORD dwPenya, DWORD dwExistingPenya );
	void	SendOutGuildCombat( u_long idGuild );
	void	SendGuildCombatResult( void );
	void	SendGuildCombatStart( void );
	void	SendGCGetPenyaGuild( u_long uidPlayer, int nGuildCombatID, u_long uidGuild );
	void	SendGCGetPenyaPlayer( u_long uidPlayer, int nGuildCombatID, u_long uidGuild );
	void	SendGuildcombatContinue( int nGuildCombatID,  u_long uidGuild, int nWinCount );
#ifdef __INVALID_LOGIN_0320
	void	CalluspXXXMultiServer( u_long uKey, CUser* pUser );
#else	// __INVALID_LOGIN_0320
	void	CalluspXXXMultiServer( u_long uKey, u_long idPlayer );
#endif	// __INVALID_LOGIN_0320
	void	SendLogExpBox( u_long idPlayer, OBJID objid, EXPINTEGER iExp, BOOL bGet=FALSE );
	void	UpdateGuildRanking(); // TRANS서버에게 길드랭킹을 업데잇하라는 패킷을 보내는 함수
	void	UpdateGuildRankingUpdate();
	void	SendQueryGuildQuest( void );
	void	SendInsertGuildQuest( u_long idGuild, int nId );
	void	SendUpdateGuildQuest( u_long idGuild, int nId, int nState );
	void	SendQueryGuildBank(); // TRANS서버에게 길드창고를 쿼리하라는 패킷을 보내는 함수
	void	SendGuildContribution( CONTRIBUTION_CHANGED_INFO & info, BYTE nLevelUp, LONG nMemberLevel );
	void	SendGuildGetPay( u_long uGuildId, DWORD nGoldGuild, DWORD dwPay );
	void	SendQuerySetPlayerName( u_long idPlayer, const char* lpszPlayer, DWORD dwData );
	void	SendQueryMailBox( u_long idReceiver );


	//////////////////////////////////////////////////////////////////////////
	void	SendQueryMailBoxReq( u_long idReceiver );
	void	SendQueryMailBoxCount( u_long idReceiver, int nCount );
	void	OnMailBoxReq( CAr & ar, DPID, DPID );
	//////////////////////////////////////////////////////////////////////////


	void	OnMailBox( CAr & ar, DPID, DPID ); 
	void	SendQueryPostMail( u_long idReceiver, u_long idSender, CItemElem& itemElem, int nGold, char* lpszTitle, char* lpszText );
	void	SendQueryRemoveMail( u_long idReceiver, u_long nMail );
	void	SendQueryGetMailItem( u_long idReceiver, u_long nMail );
	void	SendQueryGetMailGold( u_long idReceiver, u_long nMail );
	void	SendQueryReadMail( u_long idReceiver, u_long nMail );
	void	OnPostMail( CAr & ar, DPID, DPID );
	void	OnRemoveMail( CAr & ar, DPID, DPID );
	void	OnGetMailItem( CAr & ar, DPID, DPID );
	void	OnGetMailGold( CAr & ar, DPID, DPID );
	void	OnReadMail( CAr & ar, DPID, DPID );
	void	OnAllMail( CAr & ar, DPID, DPID );
	void	CalluspLoggingQuest( u_long idPlayer, int nQuest, int nState, const char* pszComment = "" );
	void	SendPreventLogin( LPCTSTR szAccount, DWORD dwPreventTime );
	void	SendSchoolReport( PSCHOOL_ENTRY pSchool );
	void	SendBuyingInfo( PBUYING_INFO2 pbi2, SERIALNUMBER iSerialNumber );
	
#if __VER >= 11 // __MA_VER11_04	// 길드 창고 로그 기능 world,database,neuz
	void	SendQueryGetGuildBankLogList( u_long idReceiver, DWORD	idGuild,BYTE byListType );
#endif //__MA_VER11_04	// 길드 창고 로그 기능 world,database,neuz
#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
	void	SendQueryGetSealChar( u_long idReceiver ,const char* szAccount);
	void	SendQueryGetSealCharConm( u_long idReceiver );
	void	SendQueryGetSealCharGet( u_long idReceiver ,const char* szAccount,DWORD dwGetId);
	void	SendQueryGetSealCharSet( u_long idReceiver ,const char* szAccount,LONG lSetPlayerSlot,DWORD dwSetPlayerId);
#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz

#if __VER >= 9	// __PET_0410
	void	CalluspPetLog( u_long idPlayer, SERIALNUMBER iSerial, DWORD dwData, int nType, CPet* pPet );
#endif	// __PET_0410
	
#if __VER >= 9 // __EVENTLUA
	void	SendEventLuaChanged( void );
#endif // __EVENTLUA

#if __VER >= 11 // __GUILD_COMBAT_1TO1
	void	SendGC1to1StateToDBSrvr( void );
	void	SendGC1to1Tender( char cGU, u_long uGuildId, int nPenya, char cState );
	void	SendGC1to1LineUp( u_long uGuildId, vector<u_long>& vecMemberId );
	void	SendGC1to1WarPerson( u_long uGuildId, u_long uIdPlayer, char cState );
	void	SendGC1to1WarGuild( DWORD dwWorldId, u_long uGuildId_0, u_long uGuildId_1, char cState );
#endif // __GUILD_COMBAT_1TO1

#if __VER >= 11 // __SYS_PLAYER_DATA
	void	SendUpdatePlayerData( CUser* pUser );
#endif	// __SYS_PLAYER_DATA

private:
	USES_PFNENTRIES;

//	Handlers
	void	OnJoin( CAr & ar, DPID dpidCache, DPID dpidUser );
#if __VER >= 11 // __SYS_PLAYER_DATA
	void	OnAllPlayerData( CAr & ar, DPID, DPID );
	void	OnAddPlayerData( CAr & ar, DPID, DPID );
	void	OnDeletePlayerData( CAr & ar, DPID, DPID );
	void	OnUpdatePlayerData( CAr & ar, DPID, DPID );
#else	// __SYS_PLAYER_DATA
	void	OnAllPlayerID( CAr & ar, DPID, DPID );
	void	OnPlayerID( CAr & ar, DPID, DPID );
	void	OnRemovePlayerID( CAr & ar, DPID, DPID );
#endif	// __SYS_PLAYER_DATA
	void	OnGCPlayerPoint( CAr & ar );
	void	OnGCResultValue( CAr & ar );
	void	OnAllGuildCombat( CAr & ar, DPID, DPID );
	void	OnInGuildCombat( CAr & ar, DPID, DPID );
	void	OnOutGuildCombat( CAr & ar, DPID, DPID );
	void	OnResultGuildCombat( CAr & ar, DPID, DPID );
	void	OnPlayerPointGuildCombat( CAr & ar, DPID, DPID );
	void	OnGetPenyaGuildGC( CAr & ar, DPID, DPID );
	void	OnGetPenyaPlayerGC( CAr & ar, DPID, DPID );
	void	OnContinueGC( CAr & ar, DPID, DPID );
	void	OnBaseGameSetting( CAr & ar, DPID, DPID );
	void	OnMonsterRespawnSetting( CAr & ar, DPID, DPID );
	void	OnMonsterPropSetting( CAr & ar, DPID, DPID );
	void	OnGMChat( CAr & ar, DPID, DPID );
	void	OnGuildBank( CAr & ar, DPID, DPID );
	void	OnUpdateGuildRankingFinish( CAr & ar, DPID, DPID );
	void	OnQueryGuildQuest( CAr & ar, DPID, DPID );
	void	OnPing( CAr & ar, DPID, DPID );
	void	OnQueryRemoveGuildBankTbl( CAr & ar, DPID, DPID );
	void	SendQueryRemoveGuildBankTbl( int nNo, DWORD dwRemoved );
	void	OnEventGeneric( CAr & ar, DPID, DPID );
	void	OnEventFlag( CAr & ar, DPID, DPID );

#if __VER >= 9 // __EVENTLUA
	void	OnEventStateLua( CAr & ar, DPID, DPID );
	void	OnEventLuaChanged( CAr & ar, DPID, DPID );
#endif // __EVENTLUA

#if __VER >= 11 // __GUILD_COMBAT_1TO1
	void	OnGC1to1Open( CAr & ar, DPID, DPID );
	void	OnGC1to1TenderGuildFromDB( CAr & ar, DPID, DPID );
#endif // __GUILD_COMBAT_1TO1

#if __VER >= 11 // __MA_VER11_04	// 길드 창고 로그 기능 world,database,neuz
	void	OnGuildBankLogViewFromDB( CAr & ar, DPID, DPID );
#endif //__MA_VER11_04	// 길드 창고 로그 기능 world,database,neuz
#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
	void	OnSealCharFromDB( CAr & ar, DPID, DPID );
	void	OnSealCharGetFromDB( CAr & ar, DPID, DPID );
#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz

/*
#ifdef __S0114_RELOADPRO
	void	OnReloadProject( CAr & ar, DPID dpidCache, DPID dpidUser );
#endif // __S0114_RELOADPRO
*/

#if __VER >= 12 // __LORD
private:
	void	OnElectionAddDeposit( CAr & ar, DPID, DPID );
	void	OnElectionSetPledge( CAr & ar, DPID, DPID );
	void	OnElectionIncVote( CAr & ar, DPID, DPID );
	void	OnElectionBeginCandidacy( CAr & ar, DPID, DPID );
	void	OnElectionBeginVote( CAr & ar, DPID, DPID );
	void	OnElectionEndVote( CAr & ar, DPID, DPID );
	void	OnLord( CAr & ar, DPID, DPID );
	void	OnLEventCreate( CAr & ar, DPID, DPID );
	void	OnLEventInitialize( CAr & ar, DPID, DPID );
	void	OnLordSkillUse( CAr & ar, DPID, DPID );
	void	OnLordSkillTick( CAr & ar, DPID, DPID );
	void	OnLEventTick( CAr & ar, DPID, DPID );
public:
	void	SendElectionAddDeposit( u_long idPlayer, __int64 iDeposit );
	void	SendElectionSetPledge( u_long idPlayer, const char* szPledge );
	void	SendElectionIncVote( u_long idPlayer, u_long idElector );
	void	SendLEventCreate( u_long idPlayer, int iEEvent, int iIEvent );
	void	SendLordSkillUse( u_long idPlayer, u_long idTarget, int nSkill );
// operator commands
	void	SendElectionProcess( BOOL bRun );
	void	SendElectionBeginCandidacy( void );
	void	SendElectionBeginVote( void );
	void	SendElectionBeginEndVote( void );
	void	SendLEventInitialize( void );
#endif	// __LORD

#if __VER >= 12 // __TAX
private:
	void	OnTaxInfo( CAr & ar, DPID, DPID );
public:
	void	SendSecretRoomWinGuild( BYTE nCont, DWORD dwGuildId );
	void	SendLord( DWORD dwIdPlayer );
	void	SendTaxRate( BYTE nCont, int nSalesTaxRate, int nPurchaseTaxRate );
	void	SendAddTax( BYTE nCont, int nTax, BYTE nTaxKind );
	void	SendApplyTaxRateNow();
#endif // __TAX

#if __VER >= 12 // __SECRET_ROOM
private:
	void	OnSecretRoomInfoClear( CAr & ar, DPID, DPID );
	void	OnSecretRoomTenderInfo( CAr & ar, DPID, DPID );
public:
	void	SendSecretRoomInsertToDB( BYTE nContinent, __SECRETROOM_TENDER & srTender );
	void	SendSecretRoomUpdateToDB( BYTE nContinent, __SECRETROOM_TENDER & srTender, char chState );
	void	SendSecretRoomInsertLineUpToDB( BYTE nContinent, __SECRETROOM_TENDER & srTender );
	void	SendSecretRoomClosed();
#endif // __SECRET_ROOM

#if __VER >= 13 // __RAINBOW_RACE
private:
	void	OnRainbowRaceInfo( CAr & ar, DPID, DPID );

public:
	void	SendRainbowRaceReqLoad();
	void	SendRainbowRaceApplication( DWORD dwPlayerId );
	void	SendRainbowRaceFailedUser( DWORD dwPlayerId );
	void	SendRainbowRaceRanking( DWORD dwPlayerId, int nRanking );
#endif // __RAINBOW_RACE

#if __VER >= 13 // __HOUSING
	void	OnHousingLoadInfo( CAr & ar, DPID, DPID );
	void	OnHousingSetFunitureList( CAr & ar, DPID, DPID );
	void	OnHousingSetupFuniture( CAr & ar, DPID, DPID );
	void	OnHousingSetVisitAllow( CAr & ar, DPID, DPID );
	void	OnHousingDBFailed( CAr & ar, DPID, DPID );
#endif // __HOUSING

#if __VER >= 13 // __COUPLE_1117
	void	SendPropose( u_long idProposer, u_long idTarget );
	void	SendCouple( u_long idProposer, u_long idTarget );
	void	SendDecouple( u_long idPlayer );
	void	SendClearPropose();
	void	OnProposeResult( CAr & ar, DPID, DPID );
	void	OnCoupleResult( CAr & ar, DPID, DPID );
	void	OnDecoupleResult( CAr & ar, DPID, DPID );
	void	OnCouple( CAr & ar, DPID, DPID );
#if __VER >= 13 // __COUPLE_1202
	void	SendQueryAddCoupleExperience( u_long idPlayer, int nExperience );
	void	OnAddCoupleExperience( CAr & ar, DPID, DPID );
#endif	// __COUPLE_1202
#endif	// __COUPLE_1117
#if __VER >= 13 // __HONORABLE_TITLE			// 달인
	void	SendLogGetHonorTime(CMover* pMover, int nGetHonor );
#endif	// __HONORABLE_TITLE			// 달인

#ifdef __FUNNY_COIN
	void	SendFunnyCoinReqUse( DWORD dwPlayerId, CItemElem* pItemElem );	// 퍼니코인을 사용했음을 TransServer에 알린다.
	void	OnFunnyCoinAckUse( CAr & ar, DPID, DPID );		// 퍼니코인 사용에 대한 응답을 TranServer로 부터 받았다.
#endif // __FUNNY_COIN
#if __VER >= 14 // __PCBANG
	void	OnPCBangToggle( CAr & ar, DPID, DPID );
#endif // __PCBANG
#ifdef __VTN_TIMELIMIT
	void	SendTimeLimitInfoReq( DWORD dwPlayerId, char* szAccount );
	void	OnTimeLimitInfoAck( CAr & ar, DPID, DPID );
	void	SendTimeLimitUpdateReq( char* szAccount, int nPlayTime );
	void	OnTimeLimitReset( CAr & ar, DPID, DPID );
#endif // __VTN_TIMELIMIT
#if __VER >= 14 // __INSTANCE_DUNGEON
	void	SendLogInstanceDungeon( DWORD dwDungeonId, DWORD dwWorldId, u_long uChannel, int nDungeonType, char chState );
#endif // __INSTANCE_DUNGEON
#ifdef __QUIZ
	void	OnQuizEventOpen( CAr & ar, DPID, DPID );
	void	OnQuizList( CAr & ar, DPID, DPID );
	void	OnQuizEventNotice( CAr & ar, DPID, DPID );
	void	OnQuizEventChanged( CAr & ar, DPID, DPID );
	void	SendQuizEventOpen( int nType );
	void	SendQuizEventState( int nState, int nChannel, int nWinnerCount = 0, int nQuizCount = 0 );
	void	SendQuizEventEntrance( u_long idPlayer, int nChannel );
	void	SendQuizEventSelect( u_long idPlayer, int nChannel, int nQuizId, int nSelect, int nAnswer );
	void	SendPostPrizeItem( u_long idPlayer, DWORD dwItemId, int nItemNum );
#endif // __QUIZ

#ifdef __ERROR_LOG_TO_DB
	void	SendErrorLogToDB( CUser* pUser, char chType, LPCTSTR szError );
#endif // __ERROR_LOG_TO_DB
#if __VER >= 15 // __GUILD_HOUSE
	void	OnLoadGuildHouse( CAr & ar, DPID, DPID );
	void	OnBuyGuildHouse( CAr & ar, DPID, DPID );
	void	OnRemoveGuildHouse( CAr & ar, DPID, DPID );
	void	OnGuildHousePacket( CAr & ar, DPID, DPID );
	void	SendLogGuildFurniture( DWORD dwGuildId, GH_Fntr_Info & gfi, char chState );
#ifdef __GUILD_HOUSE_MIDDLE
private:
	void	OnGuildHouseTenderInfo( CAr & ar, DPID, DPID );
	void	OnGuildHouseTenderJoin( CAr & ar, DPID, DPID );
	void	OnGuildHouseTenderState( CAr & ar, DPID, DPID );
	void	OnGuildHouseTenderResult( CAr & ar, DPID, DPID );
	void	OnGuildHouseGradeUpdate( CAr & ar, DPID, DPID );
	void	OnGuildHouseLevelUpdate( CAr & ar, DPID, DPID );
	void	OnGuildHouseExpired( CAr & ar, DPID, DPID );
public:
#endif // __GUILD_HOUSE_MIDDLE
#endif // __GUILD_HOUSE

#if __VER >= 15 // __CAMPUS
private:
	void	OnAllCampus( CAr & ar, DPID, DPID );
	void	OnAddCampusMember( CAr & ar, DPID, DPID );
	void	OnRemoveCampusMember( CAr & ar, DPID, DPID );
	void	OnUpdateCampusPoint( CAr & ar, DPID, DPID );
public:
	void	SendAddCampusMember( u_long idMaster, int nMasterPoint, u_long idPupil, int nPupilPoint );
	void	SendRemoveCampusMember( u_long idCampus, u_long idPlayer );
	void	SendUpdateCampusPoint( u_long idPlayer, int nCampusPoint, BOOL bAdd, char chState );
#endif // __CAMPUS
};

#endif	// __DPDATABASECLIENT_H__