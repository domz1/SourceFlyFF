#ifndef __DPDATABASESRVR_H__
#define __DPDATABASESRVR_H__

#pragma once

#include "DPMng.h"
#include "MsgHdr.h"

#if __VER >= 12 // __LORD
#include "lord.h"
#endif	// __LORD

#undef	theClass
#define theClass	CDPTrans
#undef	theParameters
#define theParameters	CAr & ar, DPID, DPID, DPID, LPBYTE lpBuf, u_long uBufSize

class CMail;
class CMailBox;

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

#if __VER >= 13 // __HOUSING
#include "Housing.h"
#endif // __HOUSING

#if __VER >= 12 // __LORD
class CLordSkill;
#endif	// __LORD
#if __VER >= 13 // __COUPLE_1117
class CCoupleMgr;
#endif	// __COUPLE_1117

#ifdef __QUIZ
#include "Quiz.h"
#endif // __QUIZ
class CDPTrans : public CDPMng
{
public:
//	Constructions
	CDPTrans();
	virtual	~CDPTrans();
//	Overrides
	virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
//	Operations
	void	SendAllGuildCombat( DPID dpId );

	static	CDPTrans*	GetInstance( void );
#if __VER >= 11 // __SYS_PLAYER_DATA
	void	SendAllPlayerData( DPID dpid );
	void	SendAddPlayerData( u_long idPlayer, PlayerData* pPlayerData );
	void	SendDeletePlayerData( u_long idPlayer );
	void	SendUpdatePlayerData( u_long idPlayer, PlayerData* pPlayerData );
#else	// __SYS_PLAYER_DATA
//{{AFX
	void	SendAllPlayerID( DPID dpId );
	void	SendPlayerID( u_long idPlayer, const CHAR* lpszPlayer );
	void	SendRemovePlayerID( u_long idPlayer );
//}}AFX
#endif	// __SYS_PLAYER_DATA
	void	SendHdr( DWORD dwHdr, DPID dpid );

	void	SendUpdateGuildRankFinish();
	void	SendBaseGameSetting( BOOL bFirst, DPID dpid );
	void	SendMonsterRespawnSetting( int nRemoveSize, int nRemoveRespawn[], DPID dpid );
	void	SendMonsterPropSetting( BOOL bFirst, DPID dpid );
	void	SendGMChat( int nCount );
#if __VER >= 11 // __GUILD_COMBAT_1TO1
	void	SendGC1to1Open( void );
	void	SendGC1to1TenderGuild( vector<CGuildCombat1to1Mng::__GC1TO1TENDER>& vecT, vector<CGuildCombat1to1Mng::__GC1TO1TENDER>& vecF, DPID dpId );
#endif // __GUILD_COMBAT_1TO1

	void	OnQueryMailBox( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );

	
	//////////////////////////////////////////////////////////////////////////
	void	OnQueryMailBoxCount( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnQueryMailBoxReq( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	SendMailBoxReq( u_long idReceiver, DPID dpid, BOOL bHaveMailBox, CMailBox* pMailBox );
	//////////////////////////////////////////////////////////////////////////


	void	SendMailBox( CMailBox* pMailBox, DPID dpid );
	void	OnQueryPostMail( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnQueryRemoveMail( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnQueryGetMailItem( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnQueryGetMailGold( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnReadMail( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	SendAllMail( DPID dpid );
	void	SendPostMail( BOOL bResult, u_long idReceiver, CMail* pMail );
	void	SendRemoveMail( u_long idReceiver, u_long nMail );
	void	SendGetMailItem( u_long idReceiver, u_long nMail, u_long uQuery );
	void	SendGetMailGold( u_long idReceiver, u_long nMail, u_long uQuery );
	void	SendReadMail( u_long idreceiver, u_long nMail );
	void	SendEventFlag( DWORD dwFlag );
	void	SendEventGeneric( DPID dpid );

#if __VER >= 9 // __EVENTLUA
	void	SendEventLuaState( map<BYTE, BOOL> mapState, BOOL bLuaChanged, DPID dpid = DPID_ALLPLAYERS );
	void	SendEventLuaChanged( void );
	void	OnEventLuaChanged( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif // __EVENTLUA

#ifdef __S_RECOMMEND_EVE
	void	OnEveRecommend( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif // __S_RECOMMEND_EVE
	// 나중에 USES_PFNENTRIES; 밑으로 옮기자 
	void	OnSaveConcurrentUserNumber( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	
	USES_PFNENTRIES;
//	Handlers
	void	OnRemoveConnection( DPID dpid );
	void	OnJoin( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnSavePlayer( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#ifdef __S_NEW_SKILL_2
	void	OnSaveSkill( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif // __S_NEW_SKILL_2
//
	void	OnLogItem( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnLogQuest( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );

	void	OnCalluspXXXMultiServer( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnLogExpBox( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );

	void	OnLogPlayConnect( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnLogPlayDeath( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnLogLevelUp( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnLogServerDeath( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnLogUniqueItem( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnChangeBankPass( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnGammaChat( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnLogPkPvp( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnLogSchool( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnPing( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnLogSkillPoint( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	// WorldServer에게 길드창고를 쿼리하라는 패킷을 받고 쿼리를 진행하는 함수
	void	OnQueryGuildBank( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnGuildBankUpdate( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnGuildRealPay( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnGuildContribution( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnUpdateGuildRanking( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnUpdateGuildRankingDB( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );

	void	OnSchoolReport( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnBuyingInfo( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnOpenBattleServer( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnCloseBattleServer( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );

	void	OnQueryGuildQuest( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnInsertGuildQuest( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnUpdateGuildQuest( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );

	void	OnQuerySetPlayerName( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnItemTBLUpdate( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnPreventLogin( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );

#if __VER >= 9	// __PET_0410
	void	OnCalluspPetLog( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif	// __PET_0410

#if __VER >= 11 // __GUILD_COMBAT_1TO1
	void	OnGC1to1StateToDBSrvr( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnGC1to1Tender( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnGC1to1LineUp( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnGC1to1WarPerson( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnGC1to1WarGuild( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif // __GUILD_COMBAT_1TO1

#if __VER >= 11 // __MA_VER11_04	// 길드 창고 로그 기능 world,database,neuz
	void	OnGuildBankLogView( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif //__MA_VER11_04	// 길드 창고 로그 기능 world,database,neuz
#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
	void	OnSealChar( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnSealCharConm( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnSealCharGet( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnSealCharSet( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz

#if __VER >= 11 // __SYS_PLAYER_DATA
	void	OnUpdatePlayerData( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif	// __SYS_PLAYER_DATA

	void	OnQueryRemoveGuildBankTbl( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnCalluspLoggingQuest( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
public:
	void	OnGuidCombatInGuild( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnGuidCombatOutGuild( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnResultGuildCombat( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnStartGuildCombat( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnGCGetPenyaGuild( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnGCGetPenyaPlayer( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnGCContinue( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	SendInGuildCombat( u_long idGuild, DWORD dwPenya, DWORD dwExistingPenya );
	void	SendOutGuildCombat( u_long idGuild );
	void	SendResultGuildCombat( void );
	void	SendPlayerPointGuildCombat( void );
	void	SendGetPenyaGuildGC( u_long uidPlayer, int nGuildCombatID, u_long uidGuild );
	void	SendGetPenyaPlayerGC( u_long uidPlayer, int nGuildCombatID, u_long uidGuild );
	void	SendContinueGC( void );
	void	SendQueryRemoveGuildBankTbl( int nNo, u_long idGuild, DWORD dwItemId, SERIALNUMBER iSerialNumber, DWORD dwItemNum );

#if __VER >= 12 // __LORD
public:
	// 월드 서버에 군주입찰 결과 전송
	void	SendElectionAddDeposit( u_long idPlayer, __int64 iDeposit, time_t tCreate, BOOL bRet );
	// 월드 서버에 공약 설정 결과 전송
	void	SendElectionSetPledge( u_long idPlayer, const char* szPledge, BOOL bRet );
	// 월드 서버에 투표 결과 전송
	void	SendElectionIncVote( u_long idPlayer, u_long idElector, BOOL bRet );
	// 월드 서버에 입후보 시작 상태 전송
	void	SendElectionBeginCandidacy( void );
	// 월드 서버에 투표 시작 상태 전송
	void	SendElectionBeginVote( int nRequirement );
	// 월드 서버에 투표 종료 상태 전송
	void	SendElectionEndVote( u_long idPlayer );
	// 모든 군주 시스템 정보를 월드 서버에 전송
	void	SendLord( DPID dpid );
	// 월드 서버에 군주 이벤트 현재 상태를 전송
	void	SendLEventCreate( CLEComponent* pComponent, BOOL bResult );
	// 월드 서버에 군주 이벤트 초기화를 전송
	void	SendLEventInitialize( void );
	// 월드 서버에 군주 스킬 사용 결과를 전송
	// 결과가 참이면 월드 서버는 실제 군주 스킬 효과를 적용
	void	SendLordSkillUse( u_long idPlayer, u_long idTarget, int nSkill, int nRet );
	// 해당 군주 스킬의 재사용 대기 시간을 전송
	void	SendLordSkillTick( CLordSkill* pSkills );
	// 해당 군주 이벤트의 남아있는 틱을 전송
	void	SendLEventTick( ILordEvent* pEvent ); 
private:
	// /군주입찰
	void	OnElectionAddDeposit( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	// /공약설정
	void	OnElectionSetPledge( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	// /군주투표
	void	OnElectionIncVote( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	// /군주이벤트시작
	void	OnLEventCreate( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	// /군주스킬
	void	OnLordSkillUse( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	// /군주프로세스
	void	OnElectionProcess( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	// /입후보시작
	void	OnElectionBeginCandidacy( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	// /투표시작
	void	OnElectionBeginVote( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	// /투표종료
	void	OnElectionEndVote( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	// /군주이벤트초기화
	void	OnLEventInitialize( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif	// __LORD

#if __VER >= 12 // __TAX
public:
	void	SendTaxInfo( DPID dpId, BOOL bConnect = FALSE, BOOL bToAllClient = FALSE );
private:
	void	OnSetSecretRoomWinGuild( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnSetLord( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnTaxRate( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnAddTax( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnApplyTaxRateNow( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif // __TAX

#if __VER >= 12 // __SECRET_ROOM
public:
	void	SendSecretRoomInfoClear( DPID dpId );
	void	SendSecretRoomTenderInfo( BYTE nContinent, DWORD dwGuildId, int nPenya, vector<DWORD> & vecMemberId, DPID dpId );
private:
	void	OnSecretRoomInsertToDB( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnSecretRoomUpdateToDB( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnSecretRoomLineUpToDB( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnSecretRoomClosed( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif // __SECRET_ROOM

#if __VER >= 13 // __RAINBOW_RACE
public:
	void	SendRainbowRaceInfo( vector<DWORD> & vec_dwNowPlayerId, vector<DWORD> & vec_prevRanking, DPID dpId );

private:
	void	OnRainbowRaceLoadInfo( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnRainbowRaceApplication( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnRainbowRaceFailedUser( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnRainbowRaceRanking( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif // __RAINBOW_RACE0

#if __VER >= 13 // __HOUSING
public:
	void	SendHousingLoadInfo( DWORD dwPlayerId, CHousing* pHousing, DPID dpId );
	void	SendHousingFurnitureList( DWORD dwPlayerId, HOUSINGINFO& housingInfo, BOOL bAdd, DPID dpId );
	void	SendHousingSetupFurniture( DWORD dwPlayerId, HOUSINGINFO housingInfo, DPID dpId );
	void	SendHousingSetVisitAllow( DWORD dwPlayerId, DWORD dwTargetId, BOOL bAllow, DPID dpId );
	void	SendHousingDBFailed( DWORD dwPlayerId, DWORD dwItemId, DPID dpId );
private:
	void	OnHousingLoadInfo( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnHousingReqSetFurnitureList( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnHousingReqSetupFurniture( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnHousingReqSetVisitAllow( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnHousingReqGMRemoveAll( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif // __HOUSING

#if __VER >= 13 // __COUPLE_1117
public:
	void	OnPropose( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnCouple( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnDecouple( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnClearPropose( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	SendCouple( CCoupleMgr* pMgr, DPID dpid );
	void	SendProposeResult( u_long idProposer, u_long idTarget, int nResult, time_t t, DPID dpid );
	void	SendCoupleResult( u_long idProposer, u_long idTarget, int nResult );
	void	SendDecoupleResult( u_long idPlayer, int nResult );
#if __VER >= 13 // __COUPLE_1202
	void	OnQueryAddCoupleExperience( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	SendAddCoupleExperience( u_long idPlayer, int nExperience );
#endif	// __COUPLE_1202
#endif	// __COUPLE_1117

#if __VER >= 13 // __HONORABLE_TITLE			// 달인
	void	OnLogGetHonorTime( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif	// __HONORABLE_TITLE			// 달인

#ifdef __FUNNY_COIN
	void	OnFunnyCoinReqUse( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	SendFunnyCoinAckUse( DWORD dwPlayerId, DWORD dwItemId, SERIALNUMBER dwSerialNumber, int nResult, DPID dpId );
#endif // __FUNNY_COIN
#if __VER >= 14 // __PCBANG
	void	SendPCBangSetApply( DPID dpId );
	BOOL	m_bPCBangApply;
#endif // __PCBANG

#ifdef __VTN_TIMELIMIT
	void	OnTimeLimitInfoReq( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	SendTimeLimitAck( DWORD dwPlayerId, int nPlayTime, DPID dpId );
	void	OnTimeLimitUpdate( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	SendTimeLimitReset();
#endif // __VTN_TIMELIMIT

#if __VER >= 14 // __INSTANCE_DUNGEON
	void	OnLogInstanceDungeon( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif // __INSTANCE_DUNGEON

#ifdef __QUIZ
	void	OnQuizEventOpen( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnQuizEventState( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnQuizEventEntrance( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnQuizEventSelect( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnPostPrizeItem( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	SendQuizEventOpen( DPID dpId );
	void	SendQuizList( DPID dpId, CQuiz::QUIZLIST & QL, int nSize );
	void	SendQuizEventNotice( DPID dpId );
	void	SendQuizEventChanged();
#endif // __QUIZ
#ifdef __ERROR_LOG_TO_DB
	void	OnErrorLog( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif // __ERROR_LOG_TO_DB
#if __VER >= 15 // __GUILD_HOUSE
	void	OnBuyGuildHouse( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnGuildHousePacket( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnLogGuildFurniture( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#ifdef __GUILD_HOUSE_MIDDLE
	void	OnGuildHouseTenderJoin( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnGuildHouseLevelUpdate( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif // __GUILD_HOUSE_MIDDLE
#endif // __GUILD_HOUSE
#if __VER >= 15 // __CAMPUS
private:
	void	OnAddCampusMember( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnRemoveCampusMember( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnUpdateCampusPoint( CAr & ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
public:
	void	SendAllCampus( DPID dpId );
	void	SendAddCampusMember( u_long idCampus, u_long idMaster, u_long idPupil );
	void	SendRemoveCampusMember( u_long idCampus, u_long idPlayer );
	void	SendUpdateCampusPoint( u_long idPlayer, int nCampusPoint );
#endif // __CAMPUS
};

#endif	// __DPDATABASESRVR_H__