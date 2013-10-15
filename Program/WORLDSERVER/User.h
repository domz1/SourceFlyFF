#ifndef __USER_H__
#define __USER_H__

#pragma once

#include <DPlay.h>
#include "MsgHdr.h"
#include "Snapshot.h"
#include "UserTaskBar.h"
#include "Party.h"
#ifdef __RT_1025
#include "rtmessenger.h"
#else	// __RT_1025
#include "messenger.h"
#endif	// __RT_1025
#include "misc.h"
#include "guild.h"
#include "eveschool.h"
#include "Chatting.h"
#include "CooltimeMgr.h"

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

#if __VER >= 12 // __SECRET_ROOM
#include "SecretRoom.h"
#endif // __SECRET_ROOM
#if __VER >= 13 // __RAINBOW_RACE
#include "RainbowRace.h"
#endif // __RAINBOW_RACE
#if __VER >= 13 // __HOUSING
#include "Housing.h"
#endif // __HOUSING
#if __VER >= 14 // __PCBANG
#include "PCBang.h"
class CPCBangInfo;
#endif // __PCBANG

#if __VER >= 15 // __GUILD_HOUSE
#include "GuildHouse.h"
#endif // __GUILD_HOUSE

#if __VER >= 15 // __CAMPUS
#include "CampusHelper.h"
#endif // __CAMPUS

#define GETID( pCtrl )		( (pCtrl) ? (pCtrl)->GetId() : NULL_ID )
typedef	MemPooler<CUser>	CUserPool;

#ifdef __AZRIA_1023
#define	MAX_INPUT_LEN	100
#endif	// __AZRIA_1023



class	CShip;
class CMail;
class CMailBox;

//////////////////////////////////////////////////////////////////////////
#define CHECK_TICK_FROM_CLIENT	SEC(3)
//////////////////////////////////////////////////////////////////////////

#if __VER >= 12 // __LORD
class CLordSkill;
#endif	// __LORD
/// 플레이어를 나타내는 클래스 
class CUser : public CMover
{
public:
	CUser();
	CUser( DPID dpidCache, DPID dpidUser );
	virtual	~CUser();

	//////////////////////////////////////////////////////////////////////////

private:
	BOOL			m_bCheckTransMailBox;
	int				m_nCountFromClient;
	DWORD			m_dwTickFromClient;

	//////////////////////////////////////////////////////////////////////////

public:
	DWORD			m_dwSerial;							/// 캐쉬서버가 발급한 유저의 순차적 번호
	BOOL			m_bValid;							/// db server에서 join packet받고, 맵에 Add된후 TRUE
	CSnapshot		m_Snapshot;							/// SNAPSHOTTYPE_류 송신 누적 버퍼( Notify()에서 보내진다. )
	map<DWORD, CCtrl*>	m_2npc;							/// 주변 NPC를 담고 있는 container
	DWORD			m_dwTickSFS;						/// SubSMMode, Finite, SavePlyer()용 타이머 
	DWORD			m_dwTickNotify;						/// Notify()용 타이머 
	DWORD			m_dwTimeout4Save;					/// db 저장용 타이머 
	DWORD			m_dwLeavePenatyTime;				/// 종료요청 시각 	
	DWORD			m_dwDestroyTime;					/// Destory예약 시각 

	DWORD			m_dwAuthKey;						/// 인증용 key
	PLAY_ACCOUNT	m_playAccount;						/// Account정보 
	CUserTaskBar	m_playTaskBar;						/// player의 taskbar class(큇슬롯)
	OBJID			m_idSetTarget;						/// 이 유저가 잡고있는 타겟
#ifdef __RT_1025
	CRTMessenger	m_RTMessenger;
#else	// __RT_1025
	CMessenger		m_Messenger;						/// 메신져 
#endif	// __RT_1025
	char			m_szPartyName[32];					/// 파티 이름 
	char			m_szBankPass[5];					/// 은행 비빌번호 

	int				m_nOverHeal;						/// 오버힐 한 어시에게는 이 카운트가 붙는다.	
	CObj			*m_pWall[2];						/// 사이킥 월을 생성하면 여기에 저장
	BOOL			m_bPosting;							/// 메일 사용중?

	u_long			m_idChatting;						/// 채팅방에 들어갔을 때의 채팅방 id
	u_long			m_idSnoop;							/// 나를 감청하는자 id 
	BOOL			m_bInstantBank;						/// 즉시 사용 뱅크 사용중? 
	CCooltimeMgr	m_cooltimeMgr;						/// 아이템 쿨타임 관리자 
	DWORD			m_tmEscape;							/// 탈출 인터페이스 시작시각
	D3DXVECTOR3		m_vReturnPos;						/// '귀환 두루마리' - 목표지점  
	DWORD			m_dwReturnWorldID;					/// '귀환 두루마리' - 월드 id
	LPCTSTR			m_lpszVillage;						/// '귀환 두루마리' - 마을 이름 

	DWORD			m_dwPlayerTime;						/// DB저장용 총 플레이 시간 
	DWORD			m_dwSavePlayerTime;					/// 마지막 저장 플레이 시간의 총합 

#ifndef __JEFF_9_20
	DWORD			m_dwFlyTime;						/// DB저장용 총 비행 시간 
	DWORD			m_dwSaveFlyTime;					/// 비행시간 계산용 
	DWORD			m_dwSendFlyTime;					/// 비행시간 계산용 
#endif	// __JEFF_9_20

//	OBJID			m_idTargetCollect;					/// 자원몬스터 채집대상 - 채집중에 타겟을 바꿀 수도 있기때문
//	int				m_nCollect;							/// 현재 자원 채집량
//	DWORD			m_tmCollect;						/// 자원채집 타이머 - 1초에 한번씩

#ifdef __EVENT_1101
#ifdef __EVENT_1101_2
	__int64	m_nEventFlag;
#else // __EVENT_1101_2
	DWORD	m_dwEventFlag;	//	경과 일 번째 비트
#endif // __EVENT_1101_2
	DWORD	m_dwEventTime;	// 분 단위 계수
	DWORD	m_dwEventElapsed;	// 경과 일
#ifdef __EVENT_1101_2
	__int64	GetEventFlagBit( int nBit );
#else // __EVENT_1101_2
	int		GetEventFlagBit( int nBit );
#endif // __EVENT_1101_2
	int		SetEventFlagBit( int nBit );
	void	AddCallTheRoll( void );
#endif	// __EVENT_1101

#ifdef __HACK_1130
	DWORD	m_tmDuelRequest;
#endif	// __HACK_1130

#ifdef __QUEST_1208
	DWORD	m_tickScript;
#endif	// __QUEST_1208

#ifdef __S_SERVER_UNIFY
	BOOL	m_bAllAction;
#endif // __S_SERVER_UNIFY
	
#ifdef __EVE_MINIGAME
	int		m_nKawibawiboWin;
	int		m_nKawibawiboState;
	int		m_nBetFiveSystem[6];
#endif // __EVE_MINIGAME
	
#ifdef __EXP_ANGELEXP_LOG
	int		m_nExpLog;
	int		m_nAngelExpLog;
#endif // __EXP_ANGELEXP_LOG

#ifdef __EVENTLUA_COUPON
	int		m_nCoupon;
	DWORD	m_dwTickCoupon;
#endif // __EVENTLUA_COUPON

#ifdef __EVENTLUA_KEEPCONNECT
	DWORD	m_dwTickKeepConnect;
#endif // __EVENTLUA_KEEPCONNECT

#ifdef __PERIN_BUY_BUG
	DWORD	m_dwLastTryBuyItem;
	DWORD	m_dwLastBuyItemTick;
#endif // __PERIN_BUY_BUG

public:
	virtual void	Process();
	void			DoSMItemEquip( CItemElem* pItemElem, DWORD dwParts );
	void			DoSMItemUnEquip( CItemElem* pItemElem, DWORD dwParts );
	void			CheckFiniteItem();
	void			LevelUpSetting( void );
	void			ExpUpSetting( void );
	void			SetValid( BOOL bValid ) { m_bValid = bValid; }
	BOOL			IsValid();
	void			SetReturnPos( const D3DXVECTOR3& vPos );
	D3DXVECTOR3&	GetReturnPos( DWORD* pdwWorldID );
	void			SetPlayerTime( DWORD dwSaveTime ) { m_dwPlayerTime = timeGetTime(); m_dwSavePlayerTime = dwSaveTime; };
	DWORD			GetPlayerTime() { return (((timeGetTime() - m_dwPlayerTime) / 1000) + m_dwSavePlayerTime ); };
#ifndef __JEFF_9_20
	void			SetSaveFlyTime( DWORD dwFlyTime ) { m_dwFlyTime = m_dwSendFlyTime = 0; m_dwSaveFlyTime = dwFlyTime; };
	void			SetFlyTimeOn() { m_dwFlyTime = timeGetTime(); };
	void			SetFlyTimeOff() { m_dwSendFlyTime += (timeGetTime() - m_dwFlyTime ) / 1000; m_dwFlyTime = 0; };
	DWORD			GetFlyTime() { return m_dwSendFlyTime; };
#endif	// __JEFF_9_20
	void			NPCSetAt( OBJID objid, CCtrl* pCtrl )	{	m_2npc[objid] = pCtrl;	}
	BOOL			NPCRemoveKey( OBJID objid )		{	return m_2npc.erase( objid ) > 0; }
	void			RemoveItFromView2( BOOL bRemoveall = FALSE );
#ifdef __MAP_SECURITY
	void			Open( DWORD dwWorldId );
#else // __MAP_SECURITY
	void			Open();
#endif // __MAP_SECURITY
//	void			Notify();
	int		Notify( void );
//	void			Flush()	{	Notify();	m_Snapshot.Flush();		}
#ifdef __S_SERVER_UNIFY
	void			SetAllAction();
#endif // __S_SERVER_UNIFY

	void			AddPostMail( CMail* pMail );
	void			AddRemoveMail( u_long nMail, int nType );
	void			AddMailBox( CMailBox* pMailBox );




	//////////////////////////////////////////////////////////////////////////
    void			SendCheckMailBoxReq( BOOL bCheckTransMailBox );
	void			CheckTransMailBox( BOOL bCheckTransMailBox );
	BOOL			GetCheckTransMailBox();
	bool			CheckClientReq();
	void			ResetCheckClientReq();
	int				GetCountClientReq();
	//////////////////////////////////////////////////////////////////////////




	void			SetPosting( BOOL bPosting )		{	m_bPosting	= bPosting;	}
	BOOL			IsPosting( void )	{	return m_bPosting;	}
	BOOL			IsUsableState( DWORD dwId );
	void			OnDoUseItem( DWORD dwData, OBJID objid, int nPart );
	void			DoUsePackItem( CItemElem* pItemElem, PPACKITEMELEM pPackItemElem );
	BOOL			DoUseGiftbox( CItemElem* pItemElem, DWORD dwItemId );
	void			DoUseEveFBOX( CItemElem* pItemElem );
	void			OnMsgArrival( DWORD dwParam );
	void			ADDGameJoin();
	void			AddHdr( OBJID objid, WORD wHdr );
	void			AddBlock( LPBYTE lpBlock, u_long uBlockSize );
	void			AddAddObj( CCtrl* pCtrl );
	void			AddRemoveObj( OBJID objid )		{ AddHdr( objid, SNAPSHOTTYPE_DEL_OBJ );	}
	void			AddDiagText( LPCSTR lpszText );
	void			AddText( LPCSTR lpszText );
	void			AddReplace( DWORD dwWorldID, D3DXVECTOR3 & vPos );
	void			AddCreateItem( CItemBase* pItemBase, BYTE* pnId, short* pnNum, BYTE nCount );
	void			AddMoveItem( BYTE nItemType, BYTE nSrcIndex, BYTE nDestIndex );
	void			AddTrade( CUser* pTrader, u_long uidPlayer );
	void			AddComfirmTrade( OBJID objid );
	void			AddComfirmTradeCancel( OBJID objid );
	void			AddTradePut( OBJID objid, BYTE i, BYTE nItemType, BYTE nId, short nItemNum = 1 );
	void			AddTradePutError();
	void			AddTradePull( OBJID objid, BYTE i );
	void			AddTradePutGold( OBJID objid, DWORD dwGold );
//raiders.2006.11.28
//	void			AddTradeClearGold( OBJID objid );
	void			AddTradeCancel( OBJID objid, u_long uidPlayer, int nMode = 0 );
	void			AddTradeOk( OBJID objid )	{	AddHdr( objid, SNAPSHOTTYPE_TRADEOK );	}
	void			AddTradeConsent()		{	AddHdr( NULL_ID, SNAPSHOTTYPE_TRADECONSENT );	}
	void			AddTradelastConfirm()	{	AddHdr( NULL_ID, SNAPSHOTTYPE_TRADELASTCONFIRM );	}
	void			AddTradelastConfirmOk( OBJID objid )	{	AddHdr( objid, SNAPSHOTTYPE_TRADELASTCONFIRMOK );	}
	void			AddOpenShopWnd( CMover* pVendor );
#if __VER >= 15 // __PETVIS
	void			AddUpdateItem( CHAR cType, BYTE nId, CHAR cParam, DWORD dwValue, DWORD dwTime );
#else // __PETVIS
	void			AddUpdateItem( CHAR cType, BYTE nId, CHAR cParam, DWORD dwValue );
#endif // __PETVIS
#if __VER >= 11 // __SYS_IDENTIFY
	void			AddUpdateItemEx( unsigned char id, char cParam, __int64 iValue );
#endif	// __SYS_IDENTIFY
#ifdef __JEFF_11
	void			AddQuePetResurrectionResult( BOOL bResult );
#endif	// __JEFF_11
#if __VER >= 11 // __SYS_POCKET
	void			AddPocketAttribute( int nAttribute, int nPocket, int nData );
	void			AddPocketView( void );
	void			AddPocketAddItem( int nPocket, CItemElem* pItem );
	void			AddPocketRemoveItem( int nPocket, int nItem, short nNum );
#endif	// __SYS_POCKET
	void			AddSetFxp( int nFxp, int nFlightLv );
	void			AddSetGrowthLearningPoint( long nRemainGP );
	void			AddSetStatLevel( CHAR chID, long nValue, long nRemainGP );
	void			AddDefinedText( int dwText, LPCSTR lpszFormat, ... );
	void			AddChatText( int dwText, LPCSTR lpszFormat, ... );
	void			AddDefinedText( int dwText );
	void			AddDefinedCaption( BOOL bSmall, int dwText, LPCSTR lpszFormat, ... );
	void			AddGoldText( int nPlus );
	void			AddExpBoxInfo( OBJID objid, DWORD dwSet, DWORD dwTime, u_long idPlayer );
	void			AddSetQuest( LPQUEST lpQuest );
	void			AddSetGuildQuest( int nQuestId, int nState );
	void			AddRemoveGuildQuest( int nQuestId );
	void			AddSetChangeJob( int nJob );
	void			AddReturnSay( int ReturnFlag, const CHAR* lpszPlayer );
	void			AddGameTimer( double dCurrentTime );
	void			AddDoEquip( BYTE nId, DWORD dwItemId, BYTE fEquip );
	void			AddPartyChangeLeader( u_long uidChangeLeader );
	void			AddCancelQuest( DWORD dwQuestCancelID );
	void			AddRemoveQuest( DWORD dwQuestCancelID );
	void			AddRemoveAllQuest();
	void			AddRemoveCompleteQuest();
	void			AddGetItemBank( CItemElem* pItemElem );	
	void			AddMoveBankItem( BYTE nSrcIndex, BYTE nDestIndex );
	void			AddBankIsFull();
	void			AddRemoveGuildBankItem( u_long idGuild, DWORD dwId, DWORD dwItemNum );
	void			AddChangeBankPass( int nMode, DWORD dwId, DWORD dwItemId );
	void			AddBankWindow( int nMode, DWORD dwId, DWORD dwItemId );
	void			AddconfirmBankPass( int nMode, DWORD dwId, DWORD dwItemId );
	void			AddTaskBar();
	void			AddSendErrorParty( DWORD dw, DWORD dwSkill = 0 );
#if __VER >= 11 // __SYS_PLAYER_DATA
	void			AddPartyMember( CParty *pParty, u_long idPlayer, const char* pszLeader, const char* pszMember );
#else	// __SYS_PLAYER_DATA
	void			AddPartyMember( CParty *pParty, u_long uPlayerId = 0 );
#endif	// __SYS_PLAYER_DATA
	void			AddPartyRequest( CUser * pLeader, CUser * pMember, BOOL bTroup );
	void			AddPartyRequestCancel( u_long uLeaderid, u_long uMemberid, int nMode = 0 );
	void			AddPartyExpLevel( int Exp, int Level, int nPoint );
#if __VER < 11 // __SYS_PLAYER_DATA
	void			AddPartyMemberJob( int nMemberIndex, int nJob);
	void			AddPartyMemberLevel( int nMemberIndex, int nLevel);
#endif	// __SYS_PLAYER_DATA
	void			AddPartyMemberFlightLevel( int nMemberIndex, int nLevel);
	void			AddPartyChangeTroup( const char * szPartyName );
	void			AddPartyChangeName( const char * szPartyName );
#if __VER >= 12 // __JHMA_VER12_1	//12차 극단유료아이템
	void			AddSetPartyMode( int nMode, BOOL bOnOff, LONG nPoint , DWORD dwSkillTime );
#else // //12차 극단유료아이템
	void			AddSetPartyMode( int nMode, BOOL bOnOff, LONG nPoint );		
#endif // //12차 극단유료아이템
	void			AddPartyChangeItemMode( int nItemMode );
	void			AddPartyChangeExpMode( int nExpMode );
	void			AddSetPartyMemberParam( u_long idPlayer, BYTE nParam, int nVal );
	void			AddPartyName();
	void			AddPartySkillCall( const D3DXVECTOR3 &vLeader );
	void			AddPartySkillBlitz( OBJID idTarget );
#if __VER < 11 // __SYS_PLAYER_DATA
	void			AddFriendChangeJob( u_long uidPlayer, int nJob );
#endif	// __SYS_PLAYER_DATA
	void			AddFriendGameJoin();
	void			AddFriendReqest( u_long uLeader, LONG nJob, BYTE nSex, const char * szName );
	void			AddFriendCancel();
	void			AddFriendError( BYTE nError, const char * szName );
#if __VER < 11 // __SYS_PLAYER_DATA
	void			AddGetFriendName( int nCount, FRIEND* aFriend );
#endif	// __SYS_PLAYER_DATA

#ifdef __ENVIRONMENT_EFFECT
	void			AddEnvironmentSetting();
#endif // __ENVIRONMENT_EFFECT
	void			AddEnvironment();
	void			AddPartyChat( const CHAR* lpName, const CHAR* lpString, OBJID objid );
#ifdef __RT_1025
	void			AddAddFriend( u_long idPlayer, const char* lpszPlayer );
#else	// __RT_1025
	void			AddAddFriend( u_long idPlayer, LONG nJob, DWORD dwSex, const char* lpszPlayer );
#endif	// __RT_1025
	void			AddRemoveFriend( u_long uidSender );
	void			AddDuelCount( int nCount );
	void			AddDuelRequest( u_long uidSrc, u_long uidDst );
	void			AddDuelStart( u_long uidTarget, int bStart = 0 );
	void			AddDuelNo( OBJID idTarget );
	void			AddDuelCancel( OBJID idTarget );	// 듀얼중 취소시킴
	void			AddDuelPartyRequest( u_long uidSrc, u_long uidDst );
	void			AddDuelPartyStart( LPCTSTR szPartyName, int nMax, OBJID *pMembers, u_long idParty, int bFlag = 0 );
	void			AddDuelPartyNo( OBJID idTarget );
	void			AddDuelPartyCancel( CParty *pDuelOther );	// 듀얼중 취소시킴
	void			AddDuelPartyResult( CParty *pDuelOther, BOOL bWin );		// 듀얼결과 통보.
#if __VER >= 11 // __SYS_PLAYER_DATA
	void			AddQueryPlayerData( u_long idPlayer, PlayerData* pPlayerData );
	void			AddPlayerData( void );
#else	// __SYS_PLAYER_DATA
	void			AddQueryPlayerString( u_long idPlayer, LPCSTR lpszPlayer, BYTE nQuery );
	void			AddQueryPlayerListString( BYTE nQuery );
#endif	// __SYS_PLAYER_DATA
	void			AddSetState( LONG nStr, LONG nSta, LONG nDex, LONG nInt, LONG nGP );	
	void			AddSetSkill( DWORD dwSkill, DWORD dwSkillLevel );
	void			AddQueryGetPos( OBJID objid );
	void			AddGetPos( OBJID objid, const D3DXVECTOR3 & vPos, float fAngle );
	void			AddResurrectionMessage();
	void			AddCorrReq( CMover *pMover );
	void			AddSetFuel( int nFuel, DWORD tmAccFuel );
	void			AddTag( short nTagCount, const TAG_ENTRY* tags );
	void			AddSetSkillState( CMover *pMover, DWORD dwSkill, DWORD dwLevel, DWORD dwTime );
	void			AddPlaySound( u_long idSound );
	void			AddMoverFocus( CMover *pMover );
	void			AddPlayMusic( u_long idMusic );
	void			AddRegisterPVendorItem( BYTE iIndex, BYTE nType, BYTE nId, short nNum, int nCost );
	void			AddUnregisterPVendorItem( BYTE iIndex );
	void			AddPVendorItem( CUser* pUser, BOOL bState );	// locked
	void			AddPVendorClose( OBJID objidVendor );
	void			AddMotionError();
	void			AddPutItemBank( BYTE nSlot, CItemElem* pItemElem );
	void			AddPutGoldBank( BYTE nSlot, DWORD dwGold, DWORD dwGoldBank );
	void			AddUpdateBankItem( BYTE nSlot, BYTE nId, CHAR cParam, DWORD dwValue );
	void			AddGuildBankWindow(int nMode);
	void			AddPutItemGuildBank(CItemElem* pItemElem);
	void			AddGetItemGuildBank( CItemElem* pItemElem );
	void			AddGetGoldGuildBank( DWORD p_Gold, BYTE p_Mode, u_long playerID, BYTE cbCloak );
	void			AddContribution( CONTRIBUTION_CHANGED_INFO& info );
	void			AddSetNotice( u_long idGuild, const char* szNotice );
	void			AddInsertedVote( VOTE_INSERTED_INFO& info );
	void			AddModifyVote( u_long idVote, BYTE cbOperation, BYTE cbExtra );
	void			AddSetGuildAuthority( DWORD dwAuthority[] );
	void			AddSetGuildPenya( DWORD dwType, DWORD dwPenya );
	void			AddGuildRealPenya( int nGoldGuild, int nType );
#if __VER < 11 // __SYS_PLAYER_DATA
	void			AddChangeGuildJobLevel( u_long uidPlayer, LONG nJob, LONG nLevel );
#endif	// __SYS_PLAYER_DATA
	void			AddAllGuilds();
	void			AddMyGuild();
	void			AddGuildInvite( u_long idGuild, u_long idMaster );
	void			AddMyGuildWar();
	void			SendGuildRank();
	void			AddSMMode( int nType, DWORD dwTime );
	void			AddSMModeAll();
	void			AddResistSMMode( BYTE nAttackResistLeft, BYTE nAttackResistRight, BYTE nDefenceResist );
	void			AddCommercialElem( DWORD dwItemId, int nResistSMItemId );
	void			AddFlyffEvent();
#if __VER >= 9 // __EVENTLUA
	void			AddEventLuaDesc( int nState, string strDesc );
#endif // __EVENTLUA
	void			AddQueryGetDestObj( OBJID objid );
	void			AddGetDestObj( OBJID objid, OBJID objidDest, FLOAT fRange );
//	BOOL			DoCollect( CMover *pTarget );		// 채집시작.
//	int				OnActCollecting();					// 채집중 계속 호출.
	void			AddCommonSkill( DWORD dwSkill, DWORD dwLevel );
	void			AddExpBoxCoolTime( OBJID objid, DWORD dwTime );
	void			AddPlaySound2( const char* szSound );
	void			AddRunScriptFunc( const RunScriptFunc & runScriptFunc );
	void			AddRemoveSkillInfluence( WORD wType, WORD wID );
	void			AddGCWindow( __int64 dwPrizePenya, DWORD dwRequstPenya, DWORD dwMinRequestPenya );
	void			AddGCRequestStatus( __int64 nPrizePenya, vector<CGuildCombat::__REQUESTGUILD> vecRequestStatus );
	void			AddGCSelectPlayerWindow( vector<CGuildCombat::__JOINPLAYER> &vecSelectPlayer, u_long uidDefender, BOOL bWindow, BOOL bRequestWar );
	void			AddGCJoinWarWindow( int nMap, int nTelTime );
	void			AddGuildCombatNextTime( DWORD dwTime, DWORD dwState );
	void			AddGuildCombatEnterTime( DWORD dwTime );
	void			AddGCWinGuild( void );
	void			AddGCBestPlayer( void );
	void			AddGCIsRequest( BOOL bRequest );
	void			AddGCGetPenyaGuild( int nGetResult, __int64 nGetPenya );
	void			AddGCGetPenyaPlayer( int nGetResult, __int64 nGetPenya );
	void			AddGCDiagMessage( const char* str );
	void			AddGCTele( const char* str );
	void			AddGCWarPlayerlist( void );
	void			AddCtrlCoolTimeCancel( void );
	void			AddGuildCombatState( void );
	void			AddQuestTextTime( BOOL bFlag, int nState, DWORD dwTime );
	void			AddGCLog( void );
	void			AddGCPlayerPoint( void );
	void			AddFocusObj(OBJID objid);
	void			AddEnterChatting( CUser* pUser );
	void			AddNewChatting( CChatting* pChatting );
	void			AddRemoveChatting( u_long uidPlayer );
	void			AddDeleteChatting( void );
	void			AddChatting( u_long uidPlayer, char* pszChat );
	void			AddChttingRoomState( BOOL bState );
	void			AddGameRate( FLOAT fRate, BYTE nFlag );
	void			AddGameSetting();
	void			AddInitSkill();	
	void			AddSetNaviPoint( const NaviPoint & nv, OBJID objid, const char* Name );
	void			AddEscape( DWORD tmMaxEscape );
	void			AddSetActionPoint( int nAP );	
	void			AddSetTarget( OBJID idTarget );
	void			AddSetDuel( CMover* pMover );
#if __VER >= 8 // __S8_PK
	void			AddPKValue();
#else // __VER >= 8 // __S8_PK
	void			UpdatePlayerEnemy( BYTE byType, OBJID objid );
#endif // __VER >= 8 // __S8_PK
	void			ScheduleDestory( DWORD dwDestoryTime  );
	void			AddWantedInfo( const D3DXVECTOR3& vPos, BYTE byOnline, DWORD dwWorldID, LPCTSTR lpszWorld );
	void			AddReturnScroll();
	void			AddSnoop( const char* lpString );
	void			AddSetCheerParam( int nCheerPoint, DWORD dwRest, BOOL bAdd = FALSE );
	void			AddQueryEquip( CUser* pUser );
	void			AddSummonFriendUse( CItemElem* pItemElem );
	void			AddSummonFriendConfirm( OBJID objid, DWORD dwData, const char* szName, const char* szWorldName );
	void			AddSummonPartyConfirm( OBJID objid, DWORD dwData, const char * szWorldName );

	void			AddDoUseSkillPoint( SKILL aJobSkill[], int nSkillPoint );
	void			AddPartyMapInfo( int nIndex, D3DXVECTOR3 vPos );	// __S1005_PARTY_MAP
	void			AddSetExperience( EXPINTEGER nExp1, WORD wLevel, int nSkillPoint, int nSkillLevel, EXPINTEGER nDeathExp = (EXPINTEGER)-1, WORD wDeathLevel = 0xffff );
#ifdef __S_SERVER_UNIFY
	void			AddAllAction( BOOL bCharacter );
#endif // __S_SERVER_UNIFY

#ifdef __S1108_BACK_END_SYSTEM
	void			AddMonsterProp();
#endif // __S1108_BACK_END_SYSTEM
#if __VER >= 8 //__CSC_VER8_5
//	void			AddCreateAngel(BOOL isSuccess, char* createAngel);
	void			AddAngelInfo( BOOL bComplete = FALSE );
#endif //__CSC_VER8_5
	
#ifdef __EVE_MINIGAME
	void			AddKawibawiboResult( int nResult, int nWinCount, DWORD dwItemId = 0, int nItemCount = 0, DWORD dwNextItemId = 0, int nNextItemCount = 0 );
	void			AddReassembleResult( DWORD dwItemId, int nItemCount );
	void			AddReassembleOpenWnd( vector<DWORD> vecItemId );
	void			AddAlphabetOpenWnd( int nQuestionID, int nPublicAlphabet, char chPublicChar, CString strQuestion );
	void			AddAlphabetResult( int nResult, DWORD dwItemId = 0, int nItemCount = 0 );
	void			AddFiveSystemOpenWnd( int nBetMinPenya, int nBetMaxPenya, int nMultiple );
	void			AddFiveSystemResult( int nResult, int nGetPenya = 0, int nBetNum = 0 );
#endif // __EVE_MINIGAME

#if __VER >= 9 // __ULTIMATE
	void			AddUltimateMakeItem( BOOL bResult );
	void			AddUltimateMakeGem( int nResult, int nNum );
	void			AddUltimateWeapon( BYTE state, int nResult );
#endif // __ULTIMATE

#ifdef __TRADESYS
	void			AddExchangeResult( BYTE state, int nResult );
#endif // __TRADESYS

#if __VER >= 9	// __PET_0410
	void	AddPet( CPet* pPet, BYTE nPetLevelup = PF_PET );
	void	AddPetState( DWORD dwPetId, WORD wLife, WORD wEnergy, DWORD dwExp );
	void	AddPetSetExp( DWORD dwExp );
	void	AddPetFoodMill( int nResult, int nCount );
#endif	// __PET_0410

#if __VER >= 10 // __LEGEND	//	9차 전승시스템	Neuz, World, Trans
	void			AddLegendSkillResult( int nResult );
#endif	//__LEGEND	//	9차 전승시스템	Neuz, World, Trans
	
#if __VER >= 10 // __REMOVE_ATTRIBUTE
	void			AddRemoveAttribute( BOOL bSuccess );
#endif // __REMOVE_ATTRIBUTE

#if __VER >= 11 // __SYS_COLLECTING
	virtual	void	ProcessCollecting( void );
	virtual	void	StartCollecting( void );
	virtual	void	StopCollecting( void );
	BOOL	DoUseItemBattery( void );
#if __VER < 14 // __SMELT_SAFETY
	BOOL	PreRefine( DWORD dwTarget, DWORD dwMaterial );
	void	RefineCollector( CItemElem* pTarget, CItemElem* pMaterial );
	void	RefineAccessory( CItemElem* pTarget, CItemElem* pMaterial );
#endif // __SMELT_SAFETY
	void	AddRestartCollecting( DWORD dwItemId );
	int		m_nCollecting;
#endif	// __SYS_COLLECTING

#ifdef __AZRIA_1023
	BOOL	HasInput()	{	return m_szInput[0] != '\0';	}
	void	ResetInput()	{	m_szInput[0]	= '\0';	}
	void	SetInput( char* szInput )		{	strncpy( m_szInput, szInput, MAX_INPUT_LEN );	m_szInput[MAX_INPUT_LEN-1]	= '\0';		}
	char*	GetInput()	{	return m_szInput;	}
	int		DoUseItemInput( ItemProp* pProp, CItemElem* pItem );
#endif	// __AZRIA_1023

#ifdef __PET_1024
	int		DoUseItemPetNaming();
#endif	// __PET_1024

#ifdef __SYS_TICKET
private:
	CItemElem*	FindActiveTicket( DWORD dwItemId );
public:
	void	DoUseItemTicket( CItemElem* pItemElem );
#endif	// __SYS_TICKET

#if __VER >= 11 // __GUILD_COMBAT_1TO1
	void	AddGC1to1TenderOpenWnd( int nPenya );
	void	AddGC1to1TenderGuildView(  int nPenya, int nRanking, time_t t, vector<CGuildCombat1to1Mng::__GC1TO1TENDER>& vecTenderGuild );
	void	AddGC1to1MemberLineUpOpenWnd( vector<u_long>& vecMemberId );
	void	AddGC1to1NowState( int nState, int nTime, int nProgCount = 0 );
	void	AddGC1to1WarResult( int m_nState, u_long uIdPlayer, int nWinCount0, int nWinCount1 );
#endif // __GUILD_COMBAT_1TO1
#if __VER >= 8 // __CSC_VER8_5
	void	RemoveAngel( void );
#endif	// __CSC_VER8_5

#if __VER >= 11 // __MA_VER11_04	// 길드 창고 로그 기능 world,database,neuz
	void	AddGuildBankLogView( BYTE byListType, short nLogCount, const __GUILDBANKLOG_ENTRY* logs );
#endif //__MA_VER11_04	// 길드 창고 로그 기능 world,database,neuz
#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
	void	AddSealChar( short nLogCount, const __SEALCHAR_ENTRY* seals );
	void	AddSealCharSet();
#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz

#if __VER >= 13 // __HONORABLE_TITLE			// 달인
	void	AddHonorListAck();
#endif	// __HONORABLE_TITLE			// 달인

#ifdef __EVENTLUA_COUPON
	void	AddCoupon( int nCoupon );
#endif // __EVENTLUA_COUPON

#if __VER >= 12 // __SECRET_ROOM
	void	AddSecretRoomMngState( int nState, DWORD dwRemainTime );
	void	AddSecretRoomContInfo( BYTE nContinent, BYTE nType, vector<__SECRETROOM_TENDER> & vecSecreetRoomTender, int nIndex );
	void	AddSecretRoomTenderOpenWnd( int nTenderPenya );
	void	AddSecretRoomLineUpOpenWnd( vector<DWORD>& vecLineUpMember );
	void	AddSecretRoomTenderView( int nTenderPenya, int nRanking, time_t t, vector<__SECRETROOM_TENDER>& vecSRTender );
#endif // __SECRET_ROOM

#if __VER >= 12 // __LORD
	void	AddLord( void );
	void	AddLordSkillTick( CLordSkill* pSkills );
	int		GetMuteText( void );
	void	SetElection( u_long idElection )	{	m_idElection	= idElection;	}
	u_long	GetElection( void )	{	return m_idElection;	}
	BOOL	IsTeleportable( void );
	BOOL	IsQuerying( void )	{	return m_bQuerying;		}
	void	SetQuerying( BOOL bQuerying )	{	m_bQuerying		= bQuerying;	}
#endif	// __LORD

#if __VER >= 12 // __MOD_TUTORIAL
	void	AddSetTutorialState( void );
#endif	// __MOD_TUTORIAL

#if __VER >= 12 // __TAX
	void	AddTaxInfo( void );
	void	AddTaxSetTaxRateOpenWnd( BYTE nCont );
#endif // __TAX

	BOOL	IsGuildMaster();

	void	ValidateItem( void );
	void	AdjustGuildQuest( DWORD dwWorldId );
	void	AdjustMailboxState( void );
	void	AddEventLuaDesc( void );
	void	AdjustPartyQuest( DWORD dwWorldId );

public:
#ifndef __VM_0820
#ifndef __MEM_TRACE
	static CUserPool*	m_pPool;
	void*	operator new( size_t nSize )	{	return CUser::m_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CUser::m_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CUser::m_pPool->Free( (CUser*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CUser::m_pPool->Free( (CUser*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820

#if __VER >= 13 // __RAINBOW_RACE
	void AddRainbowRacePrevRankingOpenWnd();
	void AddRainbowRaceApplicationOpenWnd();
	void AddRainbowRaceState( CRainbowRace* pRainbowRace );
	void AddMiniGameState( __MINIGAME_PACKET MiniGamePacket );
	void AddMiniGameExtState( __MINIGAME_EXT_PACKET MiniGameExtPacket );
#endif // __RAINBOW_RACE

#if __VER >= 13 // __HOUSING
	void AddHousingAllInfo( CHousing* pHousing );
	void AddHousingSetFurnitureList( HOUSINGINFO& housingInfo, BOOL bAdd );
	void AddHousingSetupFurniture( HOUSINGINFO* pHousingInfo );
	void AddHousingPaperingInfo( DWORD dwItemId, BOOL bSetup );
	void AddHousingSetVisitAllow( DWORD dwTargetId, BOOL bAllow );
	void AddHousingVisitableList( vector<DWORD> & vecVisitable );
#endif // __HOUSING

#if __VER >= 13 // __QUEST_HELPER
	void AddNPCPos( const D3DXVECTOR3 & vPos );
#endif // __QUEST_HELPER

	void AddClearTarget();

private:
	void			Init( DPID dpidCache = DPID_UNKNOWN, DPID dpidUser = DPID_UNKNOWN );

#if __VER >= 12 // __LORD
	u_long	m_idElection;
	BOOL	m_bQuerying;
#endif	// __LORD
#ifdef __AZRIA_1023
	char	m_szInput[MAX_INPUT_LEN];
#endif	// __AZRIA_1023

#if __VER >= 13 // __COUPLE_1117
public:
	void	SetProposer( u_long idProposer )	{	m_idProposer	= idProposer;	}
	u_long	GetProposer()	{	return m_idProposer;	}
	void	AddCouple();
	void	AddProposeResult( u_long idProposer, const char* pszProposer );
	void	AddCoupleResult( u_long idPartner, const char* pszPartner );
	void	AddDecoupleResult();
#if __VER >= 13 // __COUPLE_1202
	void	ProcessCouple();
	void	ActiveCoupleBuff( int nLevel );
	void	AddAddCoupleExperience( int nExperience );
#endif	// __COUPLE_1202
#if __VER >= 14 // __PCBANG
	void	AddPCBangInfo( CPCBangInfo* pPI );
#endif // __PCBANG
#ifdef __VTN_TIMELIMIT
	void	AddAccountPlayTime();
#endif // __VTN_TIMELIMIT
#if __VER >= 14 // __SMELT_SAFETY
	void AddSmeltSafety( BYTE nResult );
#endif // __SMELT_SAFETY
#ifdef __MAP_SECURITY
	void AddWorldReadInfo( DWORD dwWorldId, D3DXVECTOR3 vPos );
#endif // __MAP_SECURITY
#ifdef __QUIZ
	void AddQuizEventState( int nType, int nState );
	void AddQuizEventMessage( const char* lpszMessage, int nState = 0 );
	void AddQuizQuestion( int nType, const char* lpszQuestion, int nCount );
#endif // __QUIZ

#if __VER >= 15 // __PETVIS
	void AddActivateVisPet( OBJID objIdVisPetItem, OBJID objIdVisPetId );
#endif // PETVIS

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	void AddCheckedQuest();
#endif // __IMPROVE_QUEST_INTERFACE

#if __VER >= 15 // __GUILD_HOUSE
public:
	void	AddGuildHousePakcet( int nPacketType, GH_Fntr_Info & gfi, int nIndex );
	void	AddGuildHouseAllInfo( CGuildHouseBase* pGuildHouse );
	void	AddGuildHouseRemvoe();
	int		GetRestPoint()						{ return m_nRestPoint; }
	int		SetIncRestPoint( int nRestPoint )	{ return SetAddRestPoint( (int)( nRestPoint * static_cast<float>( GetParam( DST_RESTPOINT_RATE, 0 ) / 100.0f ) ) ); }
	int		SetDecRestPoint( int nRestPoint )	{ return SetAddRestPoint( -nRestPoint ); }
	void	SetRestPoint( int nRestPoint )		{ m_nRestPoint = nRestPoint; }
	void	SetRestPointTick()					{ m_tRestPointTick = time_null() + REST_POINT_TICK; }
	EXPINTEGER	GetAddExpAfterApplyRestPoint( EXPINTEGER nExp );

private:
	int		SetAddRestPoint( int nRestPoint );
	void	AddRestPoint();
	
	int		m_nRestPoint;
	int		m_nPrevRestPoint;
	time_t	m_tRestPointTick;
#endif // __GUILD_HOUSE

#if __VER >= 15 // __CAMPUS
public:
	void	AddInviteCampusMember( CUser* pRequest );
	void	AddUpdateCampus( CCampus* pCampus );
	void	AddRemoveCampus( u_long idCampus );
	void	AddUpdateCampusPoint( int nCampusPoint );
	void	ProcessCampus();
	void	ActiveCampusBuff( int nLevel );
	DWORD	GetCampusTick()	{	return m_dwCampusTick;	}
	void	SetCampusTick( DWORD dwTick )		{	m_dwCampusTick = dwTick;	}
	
	int		GetPointByType( int nType );	// 100122 define.h 에 정의되어 있는 포인트 타입에 따른 유저의 포인트 리턴

private:
	DWORD	m_dwCampusTick;
#endif // __CAMPUS

#ifdef __GUILD_HOUSE_MIDDLE
public:
	void	AddGuildHouseTenderMainWnd( map<OBJID, char*>& mapGHList );
	void	AddGuildHouseTenderInfoWnd( OBJID objGHId, int nTenderMinPenya, __int64 nTenderPenya, vector<DWORD>& vecGuildId );
	void	AddGuildHouseTenderResult( OBJID objGHId, BOOL bResult );
#endif // __GUILD_HOUSE_MIDDLE

#ifdef	__COLOSSEUM
public:
	void	SendColosseumEnter();
	void	SendColosseumStart( int nTime );
	void	SendColosseumReady1( int nTime, BYTE nStage );
	void	SendColosseumReady2( int nTime );
	void	SendColosseumEndMatch( BOOL bWin, int nTime );
#endif //__COLOSSEUM

#ifdef __NEW_ITEM_VARUNA
public:
	void	AddActivateBarunaPet( DWORD dwItemId, DWORD dwBarunaPetID, BOOL bActivate );
#endif// __NEW_ITEM_VARUNA

#ifdef __TREASUREBOX
	void	SendTreasure( TreasureItem dwIDs[3], int nCount[3] );
#endif//__TREASUREBOX

private:
	u_long	m_idProposer;
#if __VER >= 13 // __COUPLE_1202
	int		m_cbProcessCouple;
#endif	// __COUPLE_1202
#endif	// __COUPLE_1117


	//	mulcom	BEGIN100405	각성 보호의 두루마리
public:
	void	SelectAwakeningValue( DWORD dwItemObjID, DWORD dwSerialNumber, BYTE bySelectFlag );
	void	SendNewRandomOption( unsigned char id, DWORD dwSerialNumber, __int64 n64NewRandomOption );
	//	mulcom	END100405	각성 보호의 두루마리

};

#define	SEC_SAVEPLAYER	900

class CDPSrvr;
#if __VER >= 12 // __LORD
class CLEComponent;
class ILordEvent;
#endif	// __LORD
	
class CUserMng
{
public:
	CUserMng();
	virtual	~CUserMng();

private:
	long				m_lCount;
	map<DWORD, CUser*>	m_users;

private:
	void			RemoveUserFromCacheMsg( CUser *pUser );

public:
	void			DestroyPlayer( CUser* pUser );
	LONG			GetCount()	{	return m_lCount;	}
	void			RemoveAllUsers();
	CUser*			AddUser( DPID dpidCache, DPID dpidUser, DPID dpidSocket );
	void			RemoveUser( DWORD dwSerial );
	CUser*			GetUser( DPID dpidCache, DPID dpidUser );
	CUser*			GetUserByPlayerID( u_long idPlayer );
//	void			Notify();
#ifdef __LAYER_1015
	BOOL			AddPlayer( CUser* pUser, DWORD dwWorldID, int nLayer );
#else	// __LAYER_1015
	BOOL			AddPlayer( CUser* pUser, DWORD dwWorldID );
#endif	// __LAYER_1015
	void			AddDisguise( CMover* pMover, DWORD dwMoverIdx );
	void			AddNoDisguise( CMover* pMover );
	void			AddSetStateLevel( CMover* pMover, CHAR chID, long nValue );
	void			AddHdr( CCtrl* pCtrl, WORD wHdr );

	void			AddChat( CCtrl* pCtrl, const TCHAR* szChat );

#if __VER >= 11 // __SYS_COLLECTING
	void			AddStartCollecting( CUser* pUser );
	void			AddStopCollecting( CUser* pUser );
#endif	// __SYS_COLLECTING
	void			AddDlgEmoticon( CCtrl* pCtrl, int nIdx );
	void			AddDamage( CMover* pMover, OBJID objidAttacker, DWORD dwHit, DWORD dwAtkFlags );
	void			AddDoEquip( CMover* pMover, int nPart, BYTE nId, const EQUIP_INFO & rEquipInfo, BYTE fEquip );
	void			AddDoEquip( CMover* pMover, int nPart, CItemElem *pItemElem, BYTE fEquip );	// 간단 파라메터 버전.
	void			AddVendor( CMover* pVendor );
	void			AddUpdateVendor( CMover* pVendor, CHAR cTab, BYTE nId, short nNum );
	void			AddSetDestParam( CMover* pMover, int nDstParameter, int nAdjParameterValue, int nChgParameterValue );
	void			AddResetDestParam( CMover* pMover, int nDstParamter, int nAdjParameterValue );
#ifdef __SPEED_SYNC_0108		// ResetDestParam speed 수정
	void			AddResetDestParamSync( CMover* pMover, int nDstParameter,int nAdjParameterValue, int nParameterValue );
#endif // __SPEED_SYNC_0108		// ResetDestParam speed 수정
	void			AddSetPointParam( CMover* pMover, int nDstParameter, int nValue );
	void			AddSetPos( CCtrl* pCtrl, D3DXVECTOR3 & vPos );
	void			AddSetPosAngle( CCtrl* pCtrl, const D3DXVECTOR3 &vPos, FLOAT fAngle );
	void			AddSetLevel( CMover* pMover, WORD wLevel );
	void			AddSetFlightLevel( CMover* pMover, int nFlightLv );
	void			AddSetSkillLevel( CMover* pMover, DWORD dwSkill, DWORD dwLevel );
	void			AddSetScale( CMover* pMover, float fScalePercent );
	void			AddWorldShout( const TCHAR* szName, const TCHAR* szShout, D3DXVECTOR3 vPos, CWorld* pWorld );
	void			AddSetSkillState( CMover* pCenter, CMover *pTarget, WORD wType, WORD wID, DWORD dwLevel, DWORD dwTime  );
	void			AddShipActMsg( CMover* pMover, CShip *pShip, DWORD dwMsg, int nParam1, int nParam2 );		
	void			AddMeleeAttack( CMover* pMover, DWORD dwAtkMsg, OBJID objid, int nParam2, int nParam3 );
	void			AddMeleeAttack2( CMover* pMover, DWORD dwAtkMsg, OBJID objid, int nParam2, int nParam3 );
	void			AddMagicAttack( CMover* pMover, DWORD dwAtkMsg, OBJID objid, int nParam2, int nParam3, int nMagicPower, int idSfxHit );
	void			AddRangeAttack( CMover* pMover, DWORD dwAtkMsg, OBJID objid, int nParam2, int nParam3, int idSfxHit );
	void			AddAttackSP( CMover* pMover, DWORD dwAtkMsg, OBJID objid, int nParam2, int nParam3 );		
	void			AddSetDestAngle( CMover* pMover, float fAngle, BYTE fLeft );
	void			AddMoverSetDestObj( CMover* pMover, OBJID objid, float fRange = 0.0f, BOOL fTransferToMe = FALSE );
	void			AddMoverDeath( CMover* pMover, CMover* pAttacker, DWORD dwMsg );
	void			AddSetMovePattern( CMover* pMover, int nPattern, const D3DXVECTOR3 &vPos, FLOAT fAngle, FLOAT fAngleX );
		
	void			AddMoverBehavior2( CMover* pMover, D3DXVECTOR3 & v, D3DXVECTOR3 &vd, float f,
										float fAngleX, float fAccPower, float fTurnAngle,
										DWORD dwState, DWORD dwStateFlag, DWORD dwMotion, int nMotionEx,
										int nLoop, DWORD dwMotionOption, __int64 nTickCount, BOOL fTransferToMe = FALSE );

	void			AddCreateSfxAllow( CMover *pMover, DWORD dwSfxObjArrow, DWORD dwSfxObjHit, D3DXVECTOR3 vPosDest, int idTarget );
	void			AddCreateSfxObj( CCtrl* pCtrl, DWORD dwSfxObj, float x = 0, float y = 0, float z = 0, BOOL bFlag = FALSE );
#if __VER >= 11 // __MA_VER11_06				// 확율스킬 효과수정 world,neuz
	void			AddRemoveSfxObj( CCtrl* pCtrl, DWORD dwSfxObj, float x = 0, float y = 0, float z = 0, BOOL bFlag = FALSE );
#endif // __MA_VER11_06				// 확율스킬 효과수정 world,neuz
	void			AddNearSetChangeJob( CMover* pMover, int nJob, LPSKILL lpSkill );
	void			AddModifyMode( CUser* pUser );
	void			AddStateMode( CUser* pUser, BYTE nFlag );
#if __VER < 8 // __S8_PK
	void			AddSetSlaughterPoint( CMover* pMover, int nVal, int nNumKill );
#endif // __VER < 8 // __S8_PK
	void			AddSetFame( CMover* pMover, int nFame );
	void			AddDefinedText( CMover* pMover, int dwText, LPCSTR lpszFormat, ... );
	void			AddBlock( LPBYTE lpBlock, u_long uBlockSize );
	void			AddBlock( LPBYTE lpBlock, u_long uBlockSize, CWorld* pWorld );
	void			AddBlock( CWorld* pWorld, const D3DXVECTOR3 & vPos, int nRange, LPBYTE lpBlock, u_long uBlockSize );
	void			AddPVendorOpen( CUser* pUser );
	void			AddPVendorClose( CUser* pUser );
#if __VER >= 11 // __MOD_VENDOR
	void			AddPVendorItemNum( CUser* pUser, BYTE nItem, short nVend, const char* sBuyer );
#else	// __MOD_VENDOR
	void			AddPVendorItemNum( CUser* pUser, BYTE nItem, short nVend );
#endif	// __MOD_VENDOR
	void			AddSetHair( CUser* pUser, BYTE nHair, BYTE r, BYTE g, BYTE b );

//	void			AddDoCollect( CUser* pUser, OBJID idTarget );
	void			AddCreateSkillEffect( CMover *pAttacker, OBJID idTarget, DWORD dwSkill, DWORD dwLevel );
	void			AddSetStun( CMover *pMover, int nMaxTime );
	void			AddSendActMsg( CMover *pMover, OBJMSG dwMsg, int nParam1 = 0, int nParam2 = 0, int nParam3 = 0 );
	void			AddPushPower( CMover *pMover, D3DXVECTOR3 vPos, FLOAT fAngle, FLOAT fPushAngle, FLOAT fPower );
	void			AddRemoveSkillInfluence( CMover *pMover, WORD wType, WORD wID );
	void			AddDoApplySkill( CCtrl *pCtrl, OBJID idTarget, DWORD dwSkill, DWORD dwLevel );
	void			AddWorldCreateSfxObj( DWORD dwSfxObj, float x, float y, float z, BOOL bFlag, DWORD dwWorldId );
	void			AddMoverMoved( CMover* pMover, D3DXVECTOR3 & v, D3DXVECTOR3 &vd, float f,
									DWORD dwState, DWORD dwStateFlag, DWORD dwMotion, int nMotionEx,
									int nLoop, DWORD dwMotionOption, __int64 nTickCount );
	void			AddMoverBehavior( CMover* pMover, D3DXVECTOR3 & v, D3DXVECTOR3 &vd, float f,
									DWORD dwState, DWORD dwStateFlag, DWORD dwMotion, int nMotionEx,
									int nLoop, DWORD dwMotionOption, __int64 nTickCount, BOOL fTransferToMe = FALSE );
	void			AddMoverMoved2( CMover* pMover, D3DXVECTOR3 & v, D3DXVECTOR3 &vd, float f,
									float fAngleX, float fAccPower, float fTurnAngle,
									DWORD dwState, DWORD dwStateFlag, DWORD dwMotion, int nMotionEx,
									int nLoop, DWORD dwMotionOption, __int64 nTickCount, BYTE nFrame = MAX_CORR_SIZE_150 );
	void			AddUseSkill( CMover* pMover, DWORD dwSkill, DWORD dwLevel, OBJID objid, int nUseType, int nCastingTime );
	void			AddEventMessage( CCtrl* pCtrl, const TCHAR* szChat, int nFlag, DWORD dwItemId = 0);
	void			AddMotion( CMover* pMover, DWORD dwMsg );
	void			AddRemoveAllSkillInfluence( CMover* pMover );
	void			AddMoverAngle( CMover* pMover, D3DXVECTOR3 & v, D3DXVECTOR3 & vd,
									float f, float fAngleX, float fAccPower, float fTurnAngle, __int64 nTickCount );
	void			AddMoverCorr( CMover* pMover, D3DXVECTOR3 & v, D3DXVECTOR3 &vd, float f,
									DWORD dwState, DWORD dwStateFlag, DWORD dwMotion, int nMotionEx,
									int nLoop, DWORD dwMotionOption, __int64 nTickCount );
	void			AddMoverCorr2( CMover* pMover, D3DXVECTOR3 & v, D3DXVECTOR3 &vd, float f,
									float fAngleX, float fAccPower, float fTurnAngle,
									DWORD dwState, DWORD dwStateFlag, DWORD dwMotion, int nMotionEx,
									int nLoop, DWORD dwMotionOption, __int64 nTickCount );
	void			AddCreateGuild( u_long idPlayer, const char* lpszPlayer, u_long idGuild, const char* szGuild );
	void			AddDestroyGuild( const char* lpszPlayer, u_long idGuild );
	void			AddSetGuild( CUser* pUser, u_long idGuild );
	void			AddSetLogo( u_long idGuild, DWORD dwLogo );
	void			AddGetItemElem( CUser* pUser, CItemElem* pItemElem );
	void			AddPutItemElem( CUser* pUser, CItemElem* pItemElem );
#if __VER >= 8 // __GUILDCOMBAT_85
	void			AddPutItemElem( u_long uidGuild, CItemElem* pItemElem );
#endif // __VER >= 8
	void			AddSetWar( CUser* pUser, u_long idWar );
	void			ModifyMode( DWORD dwMode );
	void			AddSchoolReport( PSCHOOL_ENTRY pSchool, short nElapse = 0 );
	void			AddSexChange( CMover* pMover );
	void			AddSetLocalEvent( short id, BYTE nState );
	void			AddGameRate( FLOAT fRate, BYTE nFlag );
	void			AddChangeFace( u_long uidPlayer, DWORD dwFace );
#if __VER >= 13
	void			AddShout( CUser* pUserSrc, int nRange, LPBYTE lpBlock, u_long uBlockSize );
#else // __VER >= 13
	void			AddShout( const D3DXVECTOR3 & vPos, int nRange, LPBYTE lpBlock, u_long uBlockSize );
#endif // __VER >= 13
	void			AddWorldMsg( const CRect* pRect, LPCTSTR lpszString );
	void			AddGameSetting();
	void			AddCommonPlace( CCtrl* pCtrl, BYTE nType );
	void			OutputStatistics( void );
	void			AddGuildCombatNextTimeWorld( DWORD dwTime, DWORD dwState );
	void			AddGuildCombatState( void );
	void			AddGuildCombatUserState( CMover* pMover );
#ifdef __LAYER_1015
	void			ReplaceWorld( DWORD dwWorldId, DWORD dwReplaceWorldId, float fReplaceX, float fReplaceZ, int nLayer );
	void			ReplaceWorldArea( u_long idParty, DWORD dwWorldId, DWORD dwReplaceWorldId, float fReplaceX, float fReplaceZ, float fArea, BOOL bDieFlag, int nLayer );
#else	// __LAYER_1015
	void			ReplaceWorld( DWORD dwWorldId, DWORD dwReplaceWorldId, float fReplaceX, float fReplaceZ );
	void			ReplaceWorldArea( u_long idParty, DWORD dwWorldId, DWORD dwReplaceWorldId, float fReplaceX, float fReplaceZ, float fArea, BOOL bDieFlag );
#endif	// __LAYER_1015
	void			AddWorldMsg( DWORD dwWorldId, LPCTSTR lpszString );
	void			AddGCGuildStatus( u_long uidGuild, CUser* pSendUser = NULL );
	void			AddGCGuildPrecedence( CUser* pSendUser = NULL );
	void			AddGCPlayerPrecedence( CUser* pSendUser = NULL );
	void			AddGCWinGuild( void );
	void			AddGCBestPlayer( void );
	void			AddGCWarPlayerlist( u_long uidGuild = 0, CUser* pSendUser = NULL  );
	void			AddGuildCombatEnterTime( DWORD dwTime );
	void			AddGCLogWorld( void );
	void			AddGCLogRealTimeWorld( CGuildCombat::__GCGETPOINT GCGetPoint );
	void			AddGCIsRequest( u_long uidGuild, BOOL bRequest );
	void			AddBlockNoLock( LPBYTE lpBlock, u_long uBlockSize );
	void			AddBlockNoLock( LPBYTE lpBlock, u_long uBlockSize, CWorld* pWorld );
	void			AddGuildMsg( u_long idGuild, LPCSTR lpsz );
	void			AddGuildMsg( CGuild* pGuild, LPCSTR lpsz );

#ifdef __S1108_BACK_END_SYSTEM
	void			AddMonsterProp( );
	void			AddGMChat( int nSize );
#endif // __S1108_BACK_END_SYSTEM

	void			AddMotionArrive( CMover* pMover, OBJMSG objmsg );

#if __VER >= 8 // __S8_PK
	void			AddPKPink( CMover* pMover, BYTE byPink );
	void			AddPKPropensity( CMover* pMover );
#endif // __VER >= 8 // __S8_PK
	
#ifdef __IAOBJ0622
	void			AddSetDestPos( CMover* pMover, CONST D3DXVECTOR3 & vPos, BYTE fForward, OBJID objidIAObj = NULL_ID );
#else	// __IAOBJ0622
	void			AddSetDestPos( CMover* pMover, CONST D3DXVECTOR3 & vPos, BYTE fForward );
#endif	// __IAOBJ0622
	void	AddAddRegion( DWORD dwWorldId, REGIONELEM & re );
#ifdef __EVENT_1101
	void	CallTheRoll( int nBit );
#endif	// __EVENT_1101
#if __VER >= 9	// __PET_0410
#ifdef __PET_1024
	void	AddPetCall( CMover* pMover, DWORD dwPetId, DWORD dwIndex, BYTE nPetLevel, const char* szPetName );
	void	AddSetPetName( CUser* pUser, const char* szPetName );
#else	// __PET_1024
	void	AddPetCall( CMover* pMover, DWORD dwPetId, DWORD dwIndex, BYTE nPetLevel );
#endif	// __PET_1024
	void	AddPetRelease( CMover* pMover );
	void	AddPetLevelup( CMover* pMover, DWORD dwPetId );
	void	AddPetFeed( CMover* pMover, WORD wEnergy );
#endif	// __PET_0410
#if __VER >= 13 // __HONORABLE_TITLE			// 달인
	void	AddHonorTitleChange( CMover* pMover, int nChange );
#endif	// __HONORABLE_TITLE			// 달인
#if __VER >= 9	//__AI_0509
	void	AddSetSpeedFactor( CMover* pMover, FLOAT fSpeedFactor );
#endif	// __AI_0509
	
#if __VER >= 9 // __EVENTLUA
	void	AddEventLua( BYTE nId, BOOL bState );
#endif // __EVENTLUA

#if __VER >= 12 // __LORD
	void	AddElectionBeginCandidacy( void );
	void	AddElectionBeginVote( int nRequirement );
	void	AddElectionEndVote( u_long idPlayer );
	void	AddElectionAddDeposit( u_long idPlayer, __int64 iDeposit, time_t tCreate );
	void	AddElectionSetPledge( u_long idPlayer, const char* szPledge );
	void	AddElectionIncVote( u_long idPlayer, u_long idElector );
	void	AddLEventCreate( CLEComponent* pComponent );
	void	AddLEventInitialize( void );
	void	AddLEventTick( ILordEvent* pEvent );
	void	AddLordSkillUse( CUser* pUser, u_long idTarget, int nSkill );
	void	AddQueryPlayerData( u_long idPlayer );
#endif	// __LORD
#if __VER >= 12 // __TAX
	void	AddTaxInfo( void );
#endif // __TAX
#if __VER >= 13 // __HOUSING
	void	AddHousingPaperingInfo( DWORD dwItemId, BOOL bSetup, DWORD dwMasterId );
#endif // __HOUSING
#ifdef __VTN_TIMELIMIT
	void	ResetAccountPlayTime();
#endif // __VTN_TIMELIMIT
#if __VER >= 15 // __PETVIS
	void	AddChangeMoverSfxId( CMover* pMover );
#endif // __PETVIS
#if __VER >= 15 // __GUILD_HOUSE
	BOOL	HasUserSameWorldnLayer( CUser* pUserSrc );
#endif // __GUILD_HOUSE

};

#endif	// __USER_H__
