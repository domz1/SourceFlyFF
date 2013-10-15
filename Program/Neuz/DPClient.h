#ifndef __DPCLIENT_H__
#define __DPCLIENT_H__

#include "DPMng.h"
#include "Ar.h"
//#include "MsgHdr.h"		// 이거 include 시키지 말것. 빌드 느려짐.
#include "Obj.h"
#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

#if __VER >= 13 // __RAINBOW_RACE
#include "RainbowRace.h"
#endif // __RAINBOW_RACE

#if __VER >= 13 // __HOUSING
#include "Housing.h"
#endif // __HOUSING

#undef	theClass
#define	theClass	CDPClient
#undef theParameters
#define theParameters	CAr & ar
   
typedef	struct	tagPLAYERPOS
{
	D3DXVECTOR3 vPos;
	D3DXVECTOR3	vDelta;
	BOOL fValid;
}
PLAYERPOS, *LPPLAYERPOS;

typedef	struct	tagPLAYERDESTPOS
{
	D3DXVECTOR3 vPos;
	BYTE	fForward;
	BOOL	fValid;
	FLOAT	d;
#ifdef __IAOBJ0622
	OBJID	objidIAObj;
#endif	// __IAOBJ0622
}
PLAYERDESTPOS, *LPPLAYERDESTPOS;

typedef	struct	tagPLAYERDESTANGLE
{
	float	fAngle;
	BOOL	fValid;
	BYTE	fLeft;
}
PLAYERDESTANGLE, *LPPLAYERDESTANGLE;

typedef	struct	tagPLAYERMOVINGACTMSG
{
	D3DXVECTOR3 vPos;
	BYTE	fMoving;
	BYTE	fForward;
	DWORD	dwMsg;
	int		nParam1;
	int		nParam2;
	float	fAngle;
	BOOL	fValid;
}
PLAYERMOVINGACTMSG, *LPPLAYERMOVINGACTMSG;

typedef	struct	tagPLAYERANGLE
{
	BOOL	fValid;
	int		nCounter;
}
PLAYERANGLE, *PPLAYERANGLE;

typedef struct tagSNAPSHOT
{
	PLAYERPOS	playerpos;
	PLAYERDESTPOS	playerdestpos;
	PLAYERDESTANGLE		playerdestangle;
	PLAYERMOVINGACTMSG	playermovingactmsg;
	u_long	uFrameMove;
}
SNAPSHOT, *LPSNAPSHOT;

class CWndGuildVote;
class CDPClient : public CDPMng
{
private:
	SNAPSHOT	m_ss;
	LONG		m_lError; 
	PLAYERANGLE		m_pa;

public:
	BOOL	m_fConn;
	BYTE	*m_pDump;
	int		m_nDumpSize;

#ifdef __EVE_NEWYEAR
	int		m_nCountdown;
#endif	// __EVE_NEWYEAR


	int		m_nMaxLoginGuild;
	u_long  m_uLoginPlayerIdGuild[ 200 ];
	u_long  m_uLoginGuildMulti[ 200 ];

	DWORD	m_dwReturnScroll;		// 귀환의 두루마리 

#ifdef __TRAFIC_1218
	CTraficLog	m_traficLog;
#endif	// __TRAFIC_1218

public:
//	Constructions
	CDPClient();
	virtual	~CDPClient();
//	Override
	virtual	void SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual void UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
//	Operations
	BOOL	Connect( LPSTR lpszAddr, USHORT uPort );
//	Writes
	void	SendHdr( DWORD dwHdr );
	LONG	GetErrorCode() { return m_lError; }
	LONG	GetNetError();
#ifdef __RT_1025
	void	SendJoin( BYTE nSlot, DWORD dwWorldID, CMover* pMover, CRTMessenger* pRTMessenger, u_long uIdofMulti );
#else	// __RT_1025
	void	SendJoin( BYTE nSlot, DWORD dwWorldID, CMover* pMover, CMessenger* pMessenger, u_long uIdofMulti );
#endif	// __RT_1025

	void	PostPlayerAngle( BOOL f );
	void	FlushPlayerAngle( void );
	void	SendBlock( BYTE Gu, const char *szName, const char *szFrom );
	void	SendSkillFlag( int nSkill );
	void	SendChat( LPCSTR lpszChat );
	void	SendActMsg( CMover* pMover, DWORD dwMsg, int nParam1 = 0, int nParam2 = 0 );
	void	SendDoEquip( CItemElem* pItemElem, int nPart = -1, BOOL bResult = TRUE );
	void	SendError( int nCode, int nData );
	void	SendShipActMsg( CShip *pShip, DWORD dwMsg, int nParam1, int nParam2 );
	void	SendLocalPosFromIA( const D3DXVECTOR3 &vLocal, OBJID idIA );
	void	SendRemoveItem( CItemElem* pItemElem, int nNum );
	void	SendQueryPostMail( BYTE nItem, short nItemNum, char* lpszReceiver, int nGold, char* lpszTitle, char* lpszText );
	void	SendQueryRemoveMail( u_long nMail );
	void	SendQueryGetMailItem( u_long nMail );
	void	SendQueryGetMailGold( u_long nMail );
	void	SendQueryReadMail( u_long nMail );
	void	SendQueryMailBox( void );
	void	SendMoveItem( BYTE nItemType, BYTE nSrcIndex, BYTE nDestIndex );
	void	SendDropItem( DWORD dwItemType, DWORD dwItemId, short nITemNum, const D3DXVECTOR3 & vPos );
	void	SendDropGold( DWORD dwGold, const D3DXVECTOR3 & vPlayerPos, const D3DXVECTOR3 & vPos );
	void	SendConfirmPKPVP( u_long uidPlayer );
	void	OnSetDuel( OBJID objid, CAr & ar );

#ifdef __FASTJOBCHANGE
	void	UpdateJob( int nJob );
#endif//__FASTJOBCHANGE

#ifdef __16_SHOP
	void	SendBuyItemEx( char cTab[MAX_CART], CItemElem* pItemElem[MAX_CART], int nBuy[MAX_CART] );
#endif

#if __VER >= 8 // __S8_PK
	void	OnPKRelation( OBJID objid, CAr & ar );
	void	OnPKPink( OBJID objid, CAr & ar );
	void	OnPKPropensity( OBJID objid, CAr& ar );
	void	OnPKValue( OBJID objid, CAr& ar );
#else // __VER >= 8 __S8_PK
	void	OnUpdatePlayerEnemy( OBJID objid, CAr & ar );
#endif // __VER >= 8 __S8_PK

	void	SendConfirmTrade( CMover* pTrader );
	void	SendConfirmTradeCancel( OBJID objid );
	void	SendTrade( CMover* pTrader );
	void	SendTradePut( BYTE i, BYTE ItemType, BYTE nId, short ItemNum = 1 );
	void	SendTradePull( int i );
	void	SendTradeOk( void );//	{	SendHdr( PACKETTYPE_TRADEOK );	}
	void	SendTradeCancel( int nMode = 0 );
	void	SendTradeConfirm( void );
	void	SendMessageNote( u_long uidTo, LPSTR strMessage );
	void	SendTradePutGold( DWORD dwGold );
//raiders.2006.11.28
//	void	SendTradeClearGold( void );//	{	SendHdr( PACKETTYPE_TRADECLEARGOLD );	}

	void	SendPVendorOpen( const char* szVendor );
	void	SendPVendorClose( OBJID objidVendor );
	void	SendRegisterPVendorItem( BYTE iIndex, BYTE nType, BYTE nId, short nNum, int nCost );
	void	SendUnregisterPVendorItem( BYTE iIndex );
	void	SendQueryPVendorItem( OBJID objidVendor );
	void	SendBuyPVendorItem( OBJID objidVendor, BYTE nItem, DWORD dwItemId, short nNum );
	void	SendRepairItem( DWORD* pdwIdRepair );
	void	SendMotion( DWORD dwMsg );
	void	OnMotion( OBJID objid, CAr & ar );
	void	SendSetHair( BYTE nHair, float r, float g, float b );	//, int nCost );
	void	SendPartySkillUse( int nSkill );
	void	SendPartyMemberCancle( u_long uLeader, u_long uMember, int nMode = 0 );
	void	SendPartyMemberRequest( CMover* pLeader, u_long uMemberId, BOOL bTroup );
	void	SendAddPartyMember( u_long uLeader, LONG nLLevel, LONG nLJob, DWORD dwLSex, u_long uMember, LONG nMLevel, LONG nMJob, DWORD dwMSex );
	void	SendRemovePartyMember( u_long LeaderId, u_long MemberId );
	void	SendChangeShareItem( int nItemMode );
	void	SendChangeShareExp( int nExpMode );
	void	SendExpBoxInfo( OBJID objid );		
	void	SendChangeTroup( BOOL bSendName, const char * szPartyName = "" );
	void	SendChangePartyName( const char * szPartyName );
	void	SendDuelRequest( CMover* pSrc, CMover* pDst );
	void	SendDuelYes( CMover* pSrc, CMover* pDst );
	void	SendDuelNo( CMover* pSrc );
	void	SendDuelPartyRequest( CMover* pSrc, CMover* pDst );
	void	SendDuelPartyYes( CMover* pSrc, CMover* pDst );
	void	SendDuelPartyNo( CMover* pSrc );
	void	SendPartyChangeLeader( u_long uLeaderId, u_long uChangerLeaderid );
	void	SendMoverFocus( u_long uidPlayer );
	void	SendChangeFace( u_long objid, DWORD dwFaceNum, int cost);
	
	void	SendScriptDialogReq( OBJID objid, LPCTSTR lpKey, int nGlobal1, int nGlobal2, int nGlobal3, int nGlobal4 );
	void	SendOpenShopWnd( OBJID objid );
	void	SendCloseShopWnd( void );
	void	SendBuyItem( CHAR cTab, BYTE nId, short nNum, DWORD dwItemId );

#ifdef __SHOPPING_CART
	void	SendBuyItemCart( CART_ITEM pCartItem );
#endif //__SHOPPING_CART

#if __VER >= 11 // __GUILDCOMBATCHIP
	void	SendBuyChipItem( CHAR cTab, BYTE nId, short nNum, DWORD dwItemId );
#endif // __GUILDCOMBATCHIP
	void	SendSellItem( BYTE nId, short nNum );
#ifdef __HACK_1023
	void	SendMeleeAttack( OBJMSG dwAtkMsg, OBJID objid, int nParam2, int nParam3, FLOAT fVal );
#else	// __HACK_1023
	void	SendMeleeAttack( OBJMSG dwAtkMsg, OBJID objid, int nParam2, int nParam3 );
#endif	// __HACK_1023
	void	SendMeleeAttack2( OBJMSG dwAtkMsg, OBJID objid, int nParam2, int nParam3 );
	void	SendMagicAttack( OBJMSG dwAtkMsg, OBJID objid, int nParam2, int nParam3, int nMagicPower, int idSfxHit );
	void	SendRangeAttack( OBJMSG dwAtkMsg, OBJID objid, DWORD dwItemID, int idSfxHit );

#if __VER >= 8 // __S8_PK
	void	SendUseSkill( WORD wType, WORD wId, OBJID objid, int nUseType = 0, int bControl = FALSE );
#else // __VER >= 8 __S8_PK
	void	SendUseSkill( WORD wType, WORD wId, OBJID objid, int nUseType = 0 );
#endif // __VER >= 8 __S8_PK
	void	SendSfxID( OBJID idTarget, int idSfxHit, DWORD dwType, DWORD dwSkill = NULL_ID, int nMaxDmgCnt = 1 );
	void	SendSetTarget( OBJID idTarget, BYTE bClear );
	void	SendTeleSkill( OBJID objid, D3DXVECTOR3 vPos );
		
	void	SendIncStatLevel( CHAR chID );
	void	SendIncJobLevel( CHAR chID );
	void	SendExp( EXPINTEGER nExp );
	void	SendChangeJob( int nJob, BOOL bGama = TRUE );

	void	SendOpenBankWnd( DWORD dwId, DWORD dwItemId );
	
	void	SendFocusObj();

	void	SendOpenGuildBankWnd();
	void	SendCloseGuildBankWnd();
	void	SendCloseBankWnd( void );

	void	SendDoUseSkillPoint( SKILL aJobSkill[] );
	void	SendEnterChattingRoom( u_long uidChatting );
	void	SendChatting( char * pszChat );
	void	SendOpenChattingRoom( void );
	void	SendCloseChattingRoom( void );

	void	SendPutItemGuildBank( BYTE nId, DWORD ItemNum, BYTE p_Mode );
	void	SendGetItemGuildBank( BYTE nId, DWORD ItemNum, BYTE p_Mode );
	void	SendPutItemBank( BYTE nSlot, BYTE nId, short ItemNum );
	void	SendGetItemBank( BYTE nSlot, BYTE nId, short ItemNum );
	void	SendPutItemBankToBank( BYTE nPutSlot, BYTE nSlot, BYTE nId, short ItemNum );
	void	SendPutGoldBank( BYTE nSlot, DWORD dwGold );
	void	SendGetGoldBank( BYTE nSlot, DWORD dwGold );
	void	SendPutGoldBankToBank( BYTE nPutSlot, BYTE nSlot, DWORD dwGold );
	void	SendStateModeCancel( DWORD dwStateMode, BYTE nFlag );

	
	void	SendMoveBankItem( BYTE nSrcIndex, BYTE nDestIndex );

	void	SendChangeBankPass( const char *szLastPass, const char *szNewPass, DWORD dwId, DWORD dwItemId );
	void	SendConfirmBank( const char *szPass, DWORD dwId, DWORD dwItemId );

	void	SendCorrReq( CObj *pObj );
	void	SendCorrReq( OBJID idObj );
	void	SendCommandPlace( BYTE nType );
	void	SendScriptRemoveAllItem( DWORD dwItemId );
	void	SendScriptEquipItem( DWORD dwItemId, int nOption );
	void	SendScriptCreateItem( BYTE nItemType, DWORD dwItemId, short nNum, int nOption );
	void	SendScriptAddGold( int nGold );
	void	SendScriptRemoveQuest( int nQuestId );
	void	SendSetQuest( int nQuestIdx, int nQuest );		
	void	SendScriptReplace( DWORD dwWorld, D3DXVECTOR3 vPos );
	void	SendScriptReplaceKey( DWORD dwWorld, LPCSTR lpszKey );
	void    SendScriptAddExp( int nExp );
	void	SendCreateGuildCloak( void );
//________________________________________________________________________________
	void	SendPlayerMoved( void );
	void	SendPlayerBehavior( void );
	void	SendPlayerMoved2( BYTE nFrame = MAX_CORR_SIZE_150 );
	void	SendPlayerBehavior2( void );
	void	SendPlayerAngle( void );
		
	void	SendPlayerCorr( void );
	void	SendPlayerCorr2( void );
	void	SendPlayerDestObj( OBJID objid, float fRange = 0.0f );
	void	SendQueryGetPos( CMover* pMover );
	void	SendGetPos( const D3DXVECTOR3 & vPos, float fAngle, OBJID objid );
	void	SendCtrlCoolTimeCancel();		
	void	SendQueryGetDestObj( CMover* pMover );
	void	SendGetDestObj( OBJID objid, OBJID objidDest );
	void	SendSkillTaskBar( void );
	void	SendRemoveAppletTaskBar( BYTE nIndex );
	void	SendAddAppletTaskBar( BYTE nIndex, LPSHORTCUT pAppletShortcut );
	void	SendRemoveItemTaskBar( BYTE nSlotIndex, BYTE nIndex );
	void	SendAddItemTaskBar( BYTE nSlotIndex, BYTE nIndex, LPSHORTCUT pItemShortcut );
	void	SendAddFriend( u_long uidPlayer, LONG nJob, BYTE nSex );
	void	SendAddFriendReqest( u_long uidPlayer );
	void	SendAddFriendNameReqest( const char * szName );
	void	SendFriendCancel( u_long uidLeader, u_long uidMember );
#if __VER < 11 // __SYS_PLAYER_DATA
	void	SendGetFriendName();
#endif	// __SYS_PLAYER_DATA
#if __VER >= 11 // __SYS_POCKET
	void	SendAvailPocket( int nPocket, int nItem );
	void	SendMoveItem_Pocket( int nPocket1, int nItem, short nNum, int nPocket2 );
#endif	// __SYS_POCKET
#ifdef __JEFF_11
	void	SendQuePetResurrection( int nItem );
#endif	// __JEFF_11
	void	SendGetFriendState();
	void	SendSetState( int state );
	void	SendFriendInterceptState( u_long uidPlayer );
	void	SendRemoveFriend( u_long uidPlayer );
	void	SendUpgradeBase( DWORD dwItemId0, DWORD dwItemId1, 
							DWORD dwItemId2, DWORD dwItemCount2, 
							DWORD dwItemId3, DWORD dwItemCount3, 
							DWORD dwItemId4, DWORD dwItemCount4,
							DWORD dwItemId5, DWORD dwItemCount5 );

	void	SendRandomScroll( OBJID objid, OBJID objid2 );
	void	SendEnchant( OBJID objid, OBJID objMaterialId );
#if __VER >= 10 // __REMOVE_ATTRIBUTE
	void	SendRemoveAttribute( OBJID objid );
#endif // __REMOVE_ATTRIBUTE
#if __VER >= 13 // __EXT_ENCHANT
	void	SendChangeAttribute( OBJID objTargetItem, OBJID objMaterialItem, int nAttribute );
#endif // __EXT_ENCHANT
	void	SendPiercingSize( OBJID objid1, OBJID objid2, OBJID objid3 );
#ifdef __SYS_ITEMTRANSY
	void	SendItemTransy( OBJID objid0, OBJID objid1, DWORD dwChangeId = NULL_ID, BOOL bCash = TRUE );
#else // __SYS_ITEMTRANSY
	void	SendItemTransy( OBJID objid0, OBJID objid1 );
#endif // __SYS_ITEMTRANSY
	void	SendPiercing( OBJID objid1, OBJID objid2 );
#if __VER >= 11 // __PIERCING_REMOVE
	void	SendPiercingRemove( OBJID objid );
#endif // __PIERCING_REMOVE
	void	SendCommercialElem( DWORD dwItemId, DWORD dwItemId1 );
	void	SendCreateSfxObj( DWORD dwSfxId, u_long idPlayer = NULL_ID, BOOL bFlag = FALSE );
	void	SendSetNaviPoint( const D3DXVECTOR3 & Pos, OBJID objidTarget );
	void	OnSetNaviPoint( OBJID objid, CAr & ar );
	void	SendGuildInvite( OBJID objid );
	void	SendIgnoreGuildInvite( u_long idPlayer );
	void	SendCreateGuild( const char* szGuild );
	void	SendDestroyGuild( u_long idMaster );
	void	SendAddGuildMember( u_long idMaster, const GUILD_MEMBER_INFO & info, BOOL bGM );
	void	SendRemoveGuildMember( u_long idMaster, u_long idPlayer );
	void	SendGuildLogo( DWORD dwLogo );
	void	SendGuildContribution( BYTE cbPxpCount, int nGold, BYTE cbItemFlag = 0);
	void    SendGuildNotice( const char* szNotice );
	void	SendGuildAuthority( u_long uGuildId, DWORD dwAuthority[] );
	void	SendGuilPenya( u_long uGuildId, DWORD dwType, DWORD dwPenty );
	void	SendGuildSetName( LPCTSTR szName );
	void	SendGuildRank( DWORD nVer );
	void	SendGuildMemberLv( u_long idMaster, u_long idPlayer, int nMemberLv );
	void	SendAddVote( const char* szTitle, const char* szQuestion, const char* szSelections[] );
	void	SendRemoveVote( u_long idVote );
	void	SendCloseVote( u_long idVote );
	void	SendCastVote( u_long idVote, BYTE cbSelection );
	void	UpdateGuildWnd();
	void	SendGuildClass( u_long idMaster, u_long idPlayer, BYTE nFlag );
	void	SendGuildNickName( u_long idPlayer, LPCTSTR strNickName );
	void	SendChgMaster( u_long idPlayer, u_long idPlayer2 );
	void	OnChgMaster( CAr & ar );
	void	OnSetWar( OBJID objid, CAr & ar );
	void	SendDeclWar( u_long idMaster, const char* szGuild );
	void	SendAcptWar( u_long idMaster, u_long idDecl );
	
	void	SendSurrender( u_long idPlayer );	// 항복
	void	SendQueryTruce( u_long idPlayer );	// 정전 요청
	void	SendAcptTruce( u_long idPlayer );	// 정전 수락
	void	OnSurrender( CAr & ar );	// 항복
	void	OnQueryTruce( CAr & ar );

	void	OnDeclWar( CAr & ar );
	void	OnAcptWar( CAr & ar );
	void	OnWar( CAr & ar );
	void	OnWarDead( CAr & ar );
	void	OnWarEnd( CAr & ar );

	void	OnRemoveSkillInfluence( OBJID objid, CAr & ar );
	void	OnRemoveAllSkillInfluence( OBJID objid, CAr & ar );
	
	void	OnRequestGuildRank( CAr & ar );
	void	SendActionPoint( int nAP );
		
//________________________________________________________________________________

	void	SendDoUseItem( DWORD dwItemId, OBJID objid, int nPart = -1 , BOOL bResult = TRUE );
#if __VER >= 11 // __SYS_IDENTIFY
	void	SendDoUseItemTarget( DWORD dwMaterial, DWORD dwTarget );
	void	SendRemoveItemLevelDown( DWORD dwId );
	void	SendAwakening( int nItem );
	void	SendBlessednessCancel( int nItem );
#endif	// __SYS_IDENTIFY

	void	SendSnapshot( BOOL fUnconditional = FALSE );
#ifdef __IAOBJ0622
	void	PutPlayerDestPos( CONST D3DXVECTOR3 & vPos, bool bForward, BYTE f = 0, OBJID objidIAObj = NULL_ID );
#else	// __IAOBJ0622
	void	PutPlayerDestPos( CONST D3DXVECTOR3 & vPos, bool bForward, BYTE f = 0 );
#endif	// __IAOBJ0622
	void	PutPlayerDestAngle( float fAnlge, BYTE fLeft, BYTE f = 0 );
	void	ClearPlayerDestPos( void );
	void	SendSfxHit( int idSfxHit, int nMagicPower, DWORD dwSkill = NULL_ID, OBJID idAttacker = NULL_ID, int nDmgCnt = 0, float fDmgAngle = 0, float fDmgPower = 0 );
	void	SendSfxClear( int idSfxHit, OBJID idMover = NULL_ID );
	void	SendQuerySetPlayerName( DWORD dwData, const char* lpszPlayer );
	void	SendSummonFriend( DWORD dwData, const char* lpszPlayer );
	void	SendSummonFriendConfirm( OBJID objid, DWORD dwData );
	void	SendSummonFriendCancel( OBJID objid, DWORD dwData );
	void	SendSummonParty( DWORD dwData );
	void	SendSummonPartyConfirm( OBJID objid, DWORD dwData );
#if __VER >= 9	// __PET_0410
	void	SendPetRelease( void );
#if __VER >= 12 // __PET_0519
	void	SendUsePetFeed( DWORD dwFeedId );	// dwFeedId : 먹이	// 입력 개수 제거
#else	// __PET_0519
	void	SendUsePetFeed( DWORD dwFeedId, short nNum );	// dwFeedId : 먹이
#endif	// __PET_0519
	void	SendMakePetFeed( DWORD dwMaterialId, short nNum, DWORD dwToolId );	// dwFeedId : 무기/방어구/전리품, dwToolId : 분쇄기( npc일 경우 NULL_ID )
	void	SendPetTamerMistake( DWORD dwId );
	void	SendPetTamerMiracle( DWORD dwId );
	void	SendFeedPocketInactive( void );
#endif	// __PET_0410
#if __VER >= 10 // __CSC_VER9_1
	void	SendLegendSkillUp(OBJID* pdwItemId, int count);
#endif //__CSC_VER9_1
#if __VER >= 9 // __CSC_VER9_2
	void	SendModifyStatus(int nStrCount, int nStaCount, int nDexCount, int nIntCount);
#endif //__CSC_VER9_2
	void	SendRemoveQuest( DWORD dwQuest );
	void	SendWantedGold( int nGold, LPCTSTR szMsg );
	void	SendWantedList();
	void	SendWantedName();

	void	SendWantedInfo( LPCTSTR szPlayer );
	void	SendReqLeave();	
	void	SendResurrectionOK();
	void	SendResurrectionCancel();
	void	SendChangePKPVPMode( DWORD dwMode, int nFlag );
	void	SendDoEscape( void );
	void	SendCheering( OBJID objid );
	void	OnSetCheerParam( OBJID objid, CAr & ar );
	void	SendQuerySetGuildName( LPCSTR pszGuild, BYTE nId );
	void	SendQueryEquip( OBJID objid );
	void	SendQueryEquipSetting( BOOL bAllow );
	void	SendReturnScroll( int nSelect );
	void	SendEndSkillQueue( void );
	void	SendGuildCombatWindow( void );
	void	SendGuildCombatApp( DWORD dwPenya );
	void	SendGuildCombatCancel( void );
	void	SendGCRequestStatusWindow( void );
	void	SendGCSelectPlayerWindow( void );
	void	SendGCSelectPlayer( vector<u_long> vecSelectPlayer, u_long uidDefender );
	void	SendGCSelectMap( int nMap );
	void	SendGCJoin( void );
	void	SendGCGetPenyaGuild( void );
	void	SendGCGetPenyaPlayer( void );
#if __VER < 8 // #ifndef __GUILDCOMBAT_85
	void	SendGCGetItem( void );
#endif // __VER < 8
	void	SendGCTele( void );
	void	SendGCPlayerPoint( void );
#if __VER >= 12 // __ITEMCREATEMON_S0602
	void	SendCreateMonster( DWORD dwItemId, D3DXVECTOR3 vPos );
#endif // __ITEMCREATEMON_S0602

#ifdef __EVE_MINIGAME
	void SendKawibawiboStart();
	void SendKawibawiboGetItem();
	void SendReassembleStart(OBJID* pdwItemId, int count);
	void SendReassembleOpenWnd();
	void SendAlphabetOpenWnd();
	void SendAlphabetStart(OBJID* pdwItemId, int count, int nQuestionID);
	void SendFiveSystemOpenWnd();
	void SendFiveSystemBet( int nBetNum, int nBetPenya );
	void SendFiveSystemStart();
	void SendFiveSystemDestroyWnd();
#endif // __EVE_MINIGAME

#if __VER >= 9 // __ULTIMATE
	void OnUltimateWeapon( OBJID objid, CAr & ar );
	void OnUltimateMakeItem( OBJID objid, CAr & ar );
	void OnUltimateTransWeapon( OBJID objid, CAr & ar );
	void OnUltimateMakeGem( OBJID objid, CAr & ar );
	void OnUltimateSetGem( OBJID objid, CAr & ar );
	void OnUltimateRemoveGem( OBJID objid, CAr & ar );
	void OnUltimateEnchantWeapon( OBJID objid, CAr & ar );

	void SendUltimateMakeItem( OBJID* pdwItemId );
	void SendUltimateMakeGem( OBJID objItemId );
	void SendUltimateTransWeapon( OBJID objItemWeapon, OBJID objItemGem1, OBJID objItemGem2 );
	void SendUltimateSetGem( OBJID objItemWeapon, OBJID objItemGem );
	void SendUltimateRemoveGem( OBJID objItemWeapon, OBJID objItemGem );
	void SendUltimateEnchantWeapon( OBJID objItemWeapon, OBJID objItemGem );
#endif // __ULTIMATE

#ifdef __TRADESYS
	void OnExchange( OBJID objid, CAr & ar );
	void OnExchangeResult( CAr & ar );
	
	void SendExchange( int nMMIId, int nListNum );
#endif // __TRADESYS

#if __VER >= 11 // __GUILD_COMBAT_1TO1
	void	SendGC1to1TenderOpenWnd();
	void	SendGC1to1TenderView();
	void	SendGC1to1Tender( int nPenya );
	void	SendGC1to1TenderCancel();
	void	SendGC1to1TenderFailed();
	void	OnGC1to1TenderOpenWnd( CAr & ar );
	void	OnGC1to1TenderView( CAr & ar );
	
	void	OnGC1to1NowState( CAr & ar );
	void	OnGC1to1WarResult( CAr & ar );
	
	void	SendGC1to1MemberLienUpOpenWnd();
	void	SendGC1to1MemberLienUp( vector<u_long>& vecMemberId );
	void	OnGC1to1MemberLineUpOpenWnd( CAr & ar );

	void	SendGC1to1TeleportToNPC();
	void	SendGC1to1TeleportToStage();
#endif // __GUILD_COMBAT_1TO1

	
#if __VER >= 11 // __MA_VER11_04	// 길드 창고 로그 기능 world,database,neuz
	void	SendReqGuildBankLogList( BYTE byListType );
	void	OnGuildBankLogList( CAr & ar );
#endif //__MA_VER11_04	// 길드 창고 로그 기능 world,database,neuz
#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
	void	SendSealChar(  );
	void	OnSealChar( CAr & ar );
	void	SendSealCharConm( OBJID objid  );
	void	OnSealCharGet( CAr & ar );
	void	SendSealCharSet( DWORD dwData );
#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz

#if __VER >= 13 // __HONORABLE_TITLE			// 달인
	void	SendReqHonorList();
	void	SendReqHonorTitleChange( int nChange );
	void	OnHonorListAck( CAr & ar ); 
	void	OnHonorChangeAck( OBJID objid,CAr & ar ); 
#endif	// __HONORABLE_TITLE			// 달인

#if __VER >= 8 //__CSC_VER8_5
	void SendCreateAngel(CString sendstr);
	void SendAngelBuff();
	void OnAngel( OBJID objid, CAr& ar );
	void OnCreateAngel( OBJID objid, CAr & ar );
	void OnAngelInfo( OBJID objid, CAr & ar );
#endif //__CSC_VER8_5
	// Handlers
	USES_PFNENTRIES;
	void	OnGetClock( CAr & ar );
	void	OnKeepAlive( CAr & ar );
	void	OnError( CAr & ar );
	void	OnSnapshot( CAr & ar );
	void	OnJoin( CAr & ar );
	void	OnReplace( CAr & ar );
	void	OnWhisper( CAr & ar );
	void	OnSay( CAr & ar );
	void	OnGMSay( CAr & ar );
//	Snapshot handlers
	void	OnAddObj( OBJID objid, CAr & ar );
	void	OnRemoveObj( OBJID objid );
	void	OnChat( OBJID objid, CAr & ar );
	void	OnEventMessage( OBJID objid, CAr & ar );
	void	OnDamage( OBJID objid, CAr & ar );
	void	OnMoverDeath( OBJID objid, CAr & ar );
	void	OnCreateItem( OBJID objid, CAr & ar );
	void	OnMoveItem( CAr & ar );
	void	OnDoEquip( OBJID objid, CAr & ar );
	void	OnShipActMsg( OBJID objid, CAr & ar );
		
	void	OnTrade( OBJID objid, CAr & ar );
	void	OnConfirmTrade( OBJID objid, CAr & ar );
	void	OnConfirmTradeCancel( OBJID objid, CAr & ar );
	void	OnTradePut( OBJID objid, CAr & ar );
	void	OnTradePutError( OBJID objid, CAr & ar );
	void	OnTradePull( OBJID objid, CAr & ar );
	void	OnTradePutGold( OBJID objid, CAr & ar );
//raiders.2006.11.28
//	void	OnTradeClearGold( OBJID objid );
	void	OnTradeCancel( OBJID objid, CAr & ar );
	void	OnTradeOk( OBJID objid, CAr & ar );
	void	OnTradeConsent( void );
	void	OnTradelastConfirm( void );
	void	OnTradelastConfirmOk( OBJID objid, CAr & ar );

	void	OnOpenShopWnd( OBJID objid, CAr & ar );
	void	OnPutItemBank( OBJID objid, CAr & ar );
	void	OnGetItemBank( OBJID objid, CAr & ar );
	void	OnPutGoldBank( OBJID objid, CAr & ar );
	void	OnMoveBankItem( OBJID objid, CAr & ar );
	void	OnUpdateBankItem( OBJID objid, CAr & ar );
	void	OnErrorBankIsFull( OBJID objid, CAr & ar );
	void	OnBankWindow( OBJID objid, CAr & ar );

	void    OnFocusObj( CAr & ar );
	
	void	OnGuildBankWindow( OBJID objid, CAr & ar );
	void	OnPutItemGuildBank( OBJID objid, CAr & ar );
	void	OnGetItemGuildBank( OBJID objid, CAr & ar );
	void	OnChangeBankPass( OBJID objid, CAr & ar );
	void	OnConfirmBankPass( OBJID objid, CAr & ar );
	void	OnVendor( OBJID objid, CAr & ar );
	void	OnUpdateVendor( OBJID objid, CAr & ar );
	void	OnUpdateItem( OBJID objid, CAr & ar );
#if __VER >= 11 // __SYS_IDENTIFY
	void	OnUpdateItemEx( OBJID objid, CAr & ar );
#endif	// __SYS_IDENTIFY
#if __VER >= 11 // __SYS_POCKET
	void	OnPocketAttribute( CAr & ar );
	void	OnPocketAddItem( CAr & ar );
	void	OnPocketRemoveItem( CAr & ar );
#endif	// __SYS_POCKET
#ifdef __JEFF_11
	void	OnQuePetResurrectionResult( CAr & ar );
#endif	// __JEFF_11
	void	OnSetDestParam( OBJID objid, CAr & ar );
	void	OnResetDestParam( OBJID objid, CAr & ar );
#ifdef __SPEED_SYNC_0108		// ResetDestParam speed 수정
	void	OnResetDestParamSync( OBJID objid, CAr & ar );
#endif // __SPEED_SYNC_0108		// ResetDestParam speed 수정
	void	OnSetPointParam( OBJID objid, CAr & ar );
	void	OnSetScale( OBJID objid, CAr & ar );
		
	void	OnSetPos( OBJID objid, CAr & ar );
	void	OnSetPosAngle( OBJID objid, CAr & ar );
	void	OnSetLevel( OBJID objid, CAr & ar );
	void	OnSetFlightLevel( OBJID objid, CAr & ar );
	void	OnSetExperience( OBJID objid, CAr & ar );
	void	OnSetFxp( OBJID objid, CAr & ar );
	void	OnSetSkillLevel( OBJID objid, CAr & ar );
	void	OnSetSkillExp( OBJID objid, CAr & ar );
	void	OnText( CAr & ar );
#ifndef __S_SERVER_UNIFY
	void	OnDiagText( CAr & ar );
#endif // __S_SERVER_UNIFY
#ifdef __S_SERVER_UNIFY
	void	OnAllAction( CAr & ar );
#endif // __S_SERVER_UNIFY
	
	void	OnRevival( OBJID objid );
	void	OnRevivalLodestar( OBJID objid );
	void	OnRevivalLodelight( OBJID objid );

	void	OnSetGrowthLearningPoint( OBJID objid, CAr & ar );
	void	OnSetStatLevel( OBJID objid, CAr & ar );

	void	OnSetDestPos( OBJID objid, CAr & ar );
	void	OnSetDestAngle( OBJID objid, CAr & ar );
	void	OnSetMovePattern( OBJID objid, CAr & ar );
	void	OnMeleeAttack( OBJID objid, CAr & ar );
	void	OnMeleeAttack2( OBJID objid, CAr & ar );
	void	OnMagicAttack( OBJID objid, CAr & ar );
	void	OnRangeAttack( OBJID objid, CAr & ar );
	void	OnAttackSP( OBJID objid, CAr & ar );
	void	OnMoverSetDestObj( OBJID objid, CAr & ar );
	void	OnUseSkill( OBJID objid, CAr & ar );
	void	OnCreateSfxObj( OBJID objid, CAr & ar );
#if __VER >= 11 // __MA_VER11_06				// 확율스킬 효과수정 world,neuz
	void	OnRemoveSfxObj( OBJID objid, CAr & ar );
#endif // __MA_VER11_06				// 확율스킬 효과수정 world,neuz
	void	OnCreateSfxAllow( OBJID objid, CAr & ar );				
	void	OnDefinedText( CAr & ar );
	void	OnChatText( CAr & ar );
	void	OnDefinedText1( CAr & ar );
	void	OnDefinedCaption( CAr & ar );
	void	OnCtrlCoolTimeCancel( OBJID objid, CAr & ar );
	void	OnGameTimer( CAr & ar );
	void	OnGameJoin( CAr & ar );
	void	OnResurrection( OBJID objid );
	void	OnTaskBar( CAr & ar );
	void	OnErrorParty( CAr & ar );
	void	OnAddPartyMember( CAr & ar );
	void	OnPartyRequest( CAr & ar );
	void	OnPartyRequestCancel( CAr & ar );
	void	OnPartyExpLevel( CAr & ar );
	void	OnPartyChangeTroup( CAr & ar );
	void	OnPartyChangeName( CAr & ar );
	void	OnPartySkillCall( OBJID objid, CAr & ar );
	void	OnPartySkillBlitz( CAr & ar );
	void	OnPartySkillRetreat( OBJID objid );
	void	OnPartySkillSphereCircle( OBJID objid );
	void	OnSetPartyMode( CAr & ar );
	void	OnPartyChangeItemMode( CAr & ar );
	void	OnPartyChangeExpMode( CAr & ar );

	void	OnSetPartyMemberParam( CAr & ar );

#ifdef __ENVIRONMENT_EFFECT

	void	OnEnvironmentSetting( CAr & ar );

	void	OnEnvironmentEffect( CAr & ar );

#else // __ENVIRONMENT_EFFECT

	void	OnEnvironment( CAr & ar );
	void	OnEnvironmentSnow( CAr & ar );
	void	OnEnvironmentRain( CAr & ar );
	void	OnEnvironmentAll( CAr & ar );

#endif // __ENVIRONMENT_EFFECT

	void	OnPartyChat( CAr & ar );
	void	OnMyPartyName( CAr & ar );
	void	OnPartyChangeLeader( CAr  & ar );
	void	OnSMMode( CAr & ar );
	void	OnSMModeAll( CAr & ar );
	void	OnResistSMMode( CAr & ar );
	void	OnCommercialElem( CAr & ar );
	void	OnMoverFocus( CAr & ar );
	void	OnPartyMapInfo( CAr & ar );

	void	OnChatting( OBJID objid, CAr & ar );
	
	void	OnCommonPlace( OBJID objid, CAr & ar );
	void	OnDoApplySkill( OBJID objid, CAr & ar );
	void	OnCommonSkill( OBJID objid, CAr & ar );
	void	OnFlyffEvent( CAr & ar );
	void	OnSetLocalEvent( CAr & ar );
	void	OnGameRate( CAr & ar );
	void	OnClearTarget( CAr & ar );
	
#if __VER >= 9 // __EVENTLUA
	void	OnEventLuaDesc( CAr & ar );
#endif // __EVENTLUA
#if __VER >= 10 // __REMOVE_ATTRIBUTE
	void	OnRemoveAttributeResult( CAr & ar );
#endif // __REMOVE_ATTRIBUTE
	void    OnMotionArrive( OBJID objid, CAr & ar );
#ifdef __S1108_BACK_END_SYSTEM
	void	OnMonsterProp( CAr & ar );
	void	OnGMChat( CAr & ar );
#endif // __S1108_BACK_END_SYSTEM

	void	OnChangeFace( CAr & ar );
	
	void	OnGuildCombat( CAr& ar );
	void	OnQuestTextTime( CAr& ar );
	
	void	OnGCInWindow( CAr& ar );
	void	OnGCRequestStatus( CAr& ar );
	void	OnGCSelectPlayer( CAr& ar );	
	void	OnGuildCombatEnterTime( CAr & ar );
	void	OnGuildCombatNextTimeState( CAr & ar );
	void	OnGuildCombatState( CAr & ar );
	void	OnGCUserState( CAr & ar );
	void	OnGCGuildStatus( CAr & ar );
	void	OnGCGuildPrecedence( CAr & ar );
	void	OnGCPlayerPrecedence( CAr & ar );
	void	OnGCJoinWarWindow( CAr & ar );
	void	OnGCGetPenyaGuild( CAr & ar );
	void	OnGCGetPenyaPlayer( CAr & ar );
	void	OnGCWinGuild( CAr & ar );
	void	OnGCBestPlayer( CAr & ar );
	void	OnGCWarPlayerList( CAr & ar );
	void	OnGCTele( CAr & ar );
	void	OnGCDiagMessage( CAr & ar );	
	void	OnIsRequest( CAr & ar );
	void	OnGCLog( CAr & ar );
	void	OnGCLogRealTime( CAr & ar );
	void	OnGCPlayerPoint( CAr & ar );

#ifdef __EVE_MINIGAME
	void	OnMiniGame( OBJID objid, CAr & ar );
	void	OnKawibawibo_Result( CAr & ar );
	void	OnReassemble_Result( CAr & ar );
	void	OnReassemble_OpenWnd( CAr & ar );
	void	OnAlphabet_OpenWnd( CAr & ar );
	void	OnAlphabet_Result( CAr & ar );
	void	OnFiveSystem_OpenWnd( CAr & ar );
	void	OnFiveSystem_Result( CAr & ar );
#endif // __EVE_MINIGAME
	void	OnSExpBoxInfo( OBJID objid, CAr & ar );
	void	OnSExpBoxCoolTime( OBJID objid, CAr & ar );
	

/*
#ifdef __S0114_RELOADPRO
	void	OnReloadProject( CAr & ar );
#endif // __S0114_RELOADPRO
*/
	void	OnFriendGameJoin( CAr & ar );
	void	OnAddFriend( CAr & ar );
	void	OnRemoveFriend( CAr & ar );
	void	OnAddFriendReqest( CAr & ar );
	void	OnAddFriendCancel( CAr & ar );
	void	OnFriendJoin( CAr & ar );
	void	OnFriendLogOut( CAr & ar );
	void	OnFriendNoIntercept( CAr & ar );
	void	OnFriendIntercept( CAr & ar );
	void	OnGetFriendState( CAr & ar );
	void	OnSetFriendState( CAr & ar );
	void	OnAddFriendError( CAr & ar );
	void	OnAddFriendNameReqest( CAr & ar );
	void	OnAddFriendNotConnect( CAr & ar );
	void	OnOneFriendState( CAr & ar );
	void	OnRemoveFriendState( CAr & ar );
	void	OnBlock( CAr & ar );
	void	OnDuelCount( CAr & ar );
	void	OnDuelRequest( CAr & ar );
	void	OnDuelStart( CAr & ar );
	void	OnDuelNo( CAr & ar );
	void	OnDuelCancel( CAr & ar );
	void	OnDuelPartyRequest( CAr & ar );
	void	OnDuelPartyStart( CAr & ar );
	void	OnDuelPartyNo( CAr & ar );
	void	OnDuelPartyCancel( CAr & ar );
	void	OnDuelPartyResult( CAr & ar );
	void	OnSkillFlag( CAr & ar );
	void	OnSetSkillState( CAr & ar );
	void	OnTagResult( CAr & ar );
	void	OnChangeShopCost( CAr & ar );
	void	OnItemDropRate( CAr & ar );
	void	OnSetGuildQuest( CAr & ar );
	void	OnRemoveGuildQuest( CAr & ar );
	void	OnSetQuest( OBJID objid, CAr & ar );
	void	OnScriptRemoveQuest( OBJID objid, CAr & ar );
	void	OnSetChangeJob( OBJID objid, CAr & ar );
	void	OnSetNearChangeJob( OBJID objid, CAr & ar );
	void	OnModifyMode( OBJID objid, CAr & ar );
	void	OnStateMode( OBJID objid, CAr & ar );
	void	OnReturnSay( OBJID objid, CAr & ar );
	void	OnClearUseSkill( OBJID objid );
#if __VER < 8 // __S8_PK
	void	OnSetSlaughterPoint( OBJID objid, CAr & ar );
#endif // __VER < 8 __S8_PK
	void	OnSetFame( OBJID objid, CAr & ar );
	void	OnSetFuel( OBJID objid, CAr & ar );
		
		//________________________________________________________________________________
	void	OnMoverMoved( OBJID objid, CAr & ar );
	void	OnMoverBehavior( OBJID objid, CAr & ar );
	void	OnMoverMoved2( OBJID objid, CAr & ar );
	void	OnMoverBehavior2( OBJID objid, CAr & ar );
	void	OnMoverAngle( OBJID objid, CAr & ar );					
	void	OnMoverCorr( OBJID objid, CAr & ar );
	void	OnMoverCorr2( OBJID objid, CAr & ar );	
	void	OnQueryGetPos( CAr & ar );
	void	OnGetPos( OBJID objid, CAr & ar );
	void	OnQueryGetDestObj( CAr & ar );
	void	OnGetDestObj( OBJID objid, CAr & ar );
#if __VER >= 11 // __SYS_PLAYER_DATA
	void	OnQueryPlayerData( CAr & ar );
	void	OnLogout( CAr & ar );
#else	// __SYS_PLAYER_DATA
	void	OnQueryPlayerString( CAr & ar );
	void	OnQueryPlayerListString( CAr & ar );
	void	OnFriendChangeJob( CAr & ar );
	void	OnGuildChangeJobLevel( CAr & ar );
	void	OnPartyMemberJob( CAr & ar );
	void	OnPartyMemberLevel( CAr & ar );
	void	OnGetFriendName( CAr & ar );
#endif	// __SYS_PLAYER_DATA
	void	OnCreateGuild( CAr & ar );
	void	OnDestroyGuild( CAr & ar );
	void	OnGuild( CAr & ar );
	void	OnSetGuild( OBJID objid, CAr & ar );
	void	OnAddGuildMember( CAr & ar );
	void	OnRemoveGuildMember( CAr & ar );
	void	OnGuildInvite( CAr & ar );
	void	OnAllGuilds( CAr & ar );
	void	OnGuildChat( CAr & ar );
	void	OnGuildMemberLv( CAr & ar );
	void	OnGuildClass( CAr & ar );
	void	OnGuildNickName( CAr & ar );
	void	OnGuildMemberLogin( CAr & ar );
	void	OnGuldMyGameJoin( CAr & ar );
	void	OnGuildError( CAr & ar );
	void	OnGuildLogo( CAr & ar );
	void	OnGuildContribution( CAr & ar );
	void	OnGuildNotice( CAr & ar );
	void	OnGuildAuthority( CAr & ar );
	void	OnGuildPenya( CAr & ar );
	void	OnGuildRealPenya( CAr & ar );
	void	OnGuildSetName( CAr & ar );
	void	OnGuildAddVote( CAr & ar );
	void	OnGuildModifyVote( CAr & ar );

	void	OnGuildRank( CAr & ar );
	void	OnCorrReq( OBJID objid, CAr & ar );
	void	OnPVendorOpen( OBJID objid, CAr & ar );
	void	OnPVendorClose( OBJID objid, CAr & ar );
	void	OnRegisterPVendorItem( OBJID objid, CAr & ar );
	void	OnUnregisterPVendorItem( OBJID objid, CAr & ar );
	void	OnPVendorItem( OBJID objid, CAr & ar );
	void	OnPVendorItemNum( OBJID objid, CAr & ar );

	void	OnSetHair( OBJID objid, CAr & ar );
	void	OnSetState( CAr & ar );
	void	OnCmdSetSkillLevel( CAr & ar );
	void	OnCreateSkillEffect( OBJID objid, CAr & ar );
	void	OnSetStun( OBJID objid, CAr & ar );
	void	OnSendActMsg( OBJID objid, CAr & ar );
	void	OnPushPower( OBJID objid, CAr & ar );
		
public:
//	void	SendDoCollect( CObj *pObj );
private:
//	void	OnDoCollect( OBJID objid, CAr & ar );
//	void	OnStopCollect( OBJID objid, CAr & ar );


	void	OnTag( OBJID objid, CAr & ar );
	void	OnRunScriptFunc( OBJID objid, CAr & ar );
	void	OnSchoolReport( CAr & ar );
	void	OnSexChange( OBJID objid, CAr & ar );
	void	OnRemoveQuest( CAr & ar );
	void	OnInitSkillPoint( CAr & ar );
	void	OnDoUseSkillPoint( CAr & ar );

#ifdef __EVE_NEWYEAR
	void	OnNewYear( CAr & ar )	{	m_nCountdown	= 60;	}
#endif	// __EVE_NEWYEAR
	void	OnResetBuffSkill( OBJID objid, CAr & ar );
//	void	OnEndRecoverMode( OBJID objid, CAr & ar );
	void	OnWantedInfo( CAr & ar );
	void	OnWantedList( CAr & ar );
	void	OnWantedName( CAr & ar );		
	void	OnResurrectionMessage();
	void	OnWorldMsg( OBJID objid, CAr & ar );
	void	OnSetPlayerName( CAr & ar );
#if __VER >= 11 // __SYS_PLAYER_DATA
	void	OnUpdatePlayerData( CAr & ar );
#endif	// __SYS_PLAYER_DATA
	void	OnEscape( OBJID objid, CAr & ar );
	void	OnSetActionPoint( OBJID objid, CAr & ar );
	void	OnEndSkillQueue( OBJID objid );
	void	OnSnoop( CAr & ar );
	void	OnQueryEquip( OBJID objid, CAr & ar );
	void	OnReturnScrollACK( CAr & ar );
	void	OnSetTarget( OBJID objid, CAr & ar );
	void	OnPostMail( CAr & ar );
	void	OnRemoveMail( CAr & ar );
	void	OnRemoveMailItem( CAr & ar );
	void	OnMailBox( CAr & ar );


	//////////////////////////////////////////////////////////////////////////
	void	OnMailBoxReq( CAr & ar );
	//////////////////////////////////////////////////////////////////////////


	void	OnSummon( CAr & ar );
	void	OnSummonFriend( CAr & ar );
	void	OnSummonFriendConfirm( CAr & ar );
	void	OnSummonPartyConfirm( CAr & ar );
	void	OnRemoveGuildBankItem( CAr & ar );
	void	OnAddRegion( CAr & ar );
#ifdef __EVENT_1101
	void	OnCallTheRoll( CAr & ar );
#endif	// __EVENT_1101

#ifdef __TRAFIC_1218
	void	OnTraficLog( CAr & ar );
#endif	// __TRAFIC_1218	

#if __VER >= 9	// __PET_0410
	void	OnPetCall( OBJID objid, CAr & ar );
	void	OnPetRelease( OBJID objid, CAr & ar );
	void	OnPetSetExp( OBJID objid, CAr & ar );
	void	OnPet( OBJID objid, CAr & ar );
	void	OnPetLevelup( OBJID objid, CAr & ar );
	void	OnPetState( OBJID objid, CAr & ar ); 
	void	OnPetFeed( OBJID objid, CAr & ar );
	void	OnPetFoodMill( OBJID objid, CAr & ar );
#endif	// __PET_0410

#if __VER >= 9	//__AI_0509
	void	OnSetSpeedFactor( OBJID objid, CAr & ar );
#endif	// __AI_0509
	
#if __VER >= 10 // __CSC_VER9_1
	void OnLegendSkillUp( CAr & ar );
#endif //__CSC_VER9_1

public:
#if __VER >= 11 // __SYS_COLLECTING
	void	SendQueryStartCollecting( void );
	void	SendQueryStopCollecting( void );
	void	OnStartCollecting( OBJID objid );
	void	OnStopCollecting( OBJID objid );
	void	OnRestartCollecting( OBJID objid, CAr & ar );
#endif	// __SYS_COLLECTING

#if __VER >= 12 // __UPDATE_OPT
	void	SendOptionEnableRenderMask( BOOL bEnable );
#endif	// __UPDATE_OPT

#ifdef __NPC_BUFF
	void	SendNPCBuff( const char* szKey );
#endif // __NPC_BUFF

#ifdef __EVENTLUA_COUPON
	void	OnEventCoupon( CAr & ar );
#endif // __EVENTLUA_COUPON

#if __VER >= 11 // __SYS_PLAYER_DATA
	void	SendQueryPlayerData( u_long idPlayer, int nVer = 0 );
	void	SendQueryPlayerData( const vector<PDVer> & vecPlayer );
#else	// __SYS_PLAYER_DATA
public:
	void	SendQueryPlayerString( u_long idPlayer, BYTE nQuery );
	void	SendQueryPlayerListString( BYTE nQuery, const vector<u_long>& vecPlayer );
private:
#endif	// __SYS_PLAYER_DATA

#if __VER >= 12 // __SECRET_ROOM
public:
	void	SendSecretRoomTender( int nPenya );
	void	SendSecretRoomTenderCancelReturn();
	void	SendSecretRoomLineUpMember( vector<DWORD> vecLineUpMember );
	void	OnSecretRoomMngState( CAr & ar );
	void	OnSecretRoomInfo( CAr & ar );
	void	SendSecretRoomTenderOpenWnd();
	void 	OnSecretRoomTenderOpenWnd( CAr & ar );
	void	SendSecretRoomLineUpOpenWnd();
	void	OnSecretRoomLineUpOpenWnd( CAr & ar );
	void	SendSecretRoomEntrance();
	void	SendSecretRoomTeleportToNPC();
	void	SendSecretRoomTenderView();
	void	OnSecretRoomTenderView( CAr & ar );
	void	SendTeleportToSecretRoomDungeon();
private:
#endif // __SECRET_ROOM

#if __VER >= 12 // __TAX
public:
	void	OnTaxInfo( CAr & ar );
	void	OnTaxSetTaxRateOpenWnd( CAr & ar );
	void	SendTaxRate( BYTE nCont, int nSalesTaxRate, int nPurchaseTaxRate );
private:
#endif // __TAX

#if __VER >= 12 // __HEAVEN_TOWER
public:
	void	SendTeleportToHeavenTower( int nFloor );
private:
#endif // __HEAVEN_TOWER

#if __VER >= 12 // __LORD
private:
	// 군주 입찰 결과 갱신
	void	OnElectionAddDeposit( CAr & ar );
	// 공약 설정 결과 갱신
	void	OnElectionSetPledge( CAr & ar );
	// 투표 결과 갱신
	void	OnElectionIncVote( CAr & ar );
	// 입후보 시작 상태로 변경
	void	OnElectionBeginCandidacy( CAr & ar );
	// 투표 시작 상태로 변경
	void	OnElectionBeginVote( CAr & ar );
	// 투표 종료 상태로 변경
	void	OnElectionEndVote( CAr & ar );
	// 군주 시스템 정보를 수신하여 복원
	void	OnLord( CAr & ar );
	// 군주 이벤트 시작 처리
	void	OnLEventCreate( CAr & ar );
	// 군주 이벤트 초기화 처리
	void	OnLEventInitialize( CAr & ar );
	// 군주 스킬 재사용 대기 시간 처리
	void	OnLordSkillTick( CAr & ar );
	// 군주 이벤트 지속 시간 처리
	void	OnLEventTick( CAr & ar );
	// 군주 스킬 사용
	void	OnLordSkillUse( OBJID objid, CAr & ar );
public:
	// 군주 입찰 처리 요청
	void	SendElectionAddDeposit( __int64 iDeposit );
	// 공약 설정 처리 요청
	void	SendElectionSetPledge( const char* szPledge );
	// 투표 처리 요청
	void	SendElectionIncVote( u_long idPlayer );
	// 군주 이벤트 시작 요청
	void	SendLEventCreate( int iEEvent, int iIEvent );
	// 군주 스킬 사용 요청
	void	SendLordSkillUse( int nSkill, const char* szTarget = "" );
#endif	// __LORD
#if __VER >= 12 // __PET_0519
	void	SendTransformItem( CTransformStuff & stuff );	// 알변환 요청
	void	SendPickupPetAwakeningCancel( DWORD dwItem );	// 픽업펫 각성 취소 요청
#endif	// __PET_0519

#ifdef __AZRIA_1023
	void	SendDoUseItemInput( DWORD dwData, char* szInput );
#endif	// __AZRIA_1023
#ifdef __PET_1024
	void	SendClearPetName();
	void	OnSetPetName( OBJID objid, CAr & ar );
#endif	// __PET_1024

#if __VER >= 12 // __MOD_TUTORIAL
	void	SendTutorialState( int nState );
	void	OnTutorialState( CAr & ar );
#endif	// __MOD_TUTORIAL
#if __VER >= 13 // __RAINBOW_RACE
private:
	void	OnRainbowRacePrevRankingOpenWnd( CAr & ar );
	void	OnRainbowRaceApplicationOpenWnd( CAr & ar );
	void	OnRainbowRaceNowState( CAr & ar );
	void	OnRainbowRaceMiniGameState( CAr & ar, BOOL bExt );
public:
	void	SendRainbowRacePrevRankingOpenWnd();
	void	SendRainbowRaceApplicationOpenWnd();
	void	SendRainbowRaceApplication();
	void	SendRainbowRaceMiniGameState( __MINIGAME_PACKET MiniGamePacket );
	void	SendRainbowRaceMiniGameExtState( __MINIGAME_EXT_PACKET MiniGameExtPacket );
	void	SendRainbowRaceReqFinish();
#endif // __RAINBOW_RACE

#if __VER >= 13 // __HOUSING
private:
	void	OnHousingAllInfo( CAr & ar );
	void	OnHousingSetFunitureList( CAr & ar );
	void	OnHousingSetupFurniture( CAr & ar );
	void	OnHousingPaperingInfo( CAr & ar );
	void	OnHousingSetVisitAllow( CAr & ar );
	void	OnHousingVisitableList( CAr & ar );
public:
	void	SendHousingReqSetupFurniture( HOUSINGINFO housingInfo );
	void	SendHousingReqSetVisitAllow( DWORD dwPlayerId, BOOL bAllow );
	void	SendHousingVisitRoom( DWORD dwPlayerId );
	void	SendHousingReqVisitableList();
	void	SendHousingGoOut();
#endif // __HOUSING

#if __VER >= 13 // __COUPLE_1117
	void	SendPropose( const char* pszTarget );
	void	SendRefuse();
	void	SendCouple();
	void	SendDecouple();
	void	OnCouple( CAr & ar );
	void	OnProposeResult( CAr & ar );
	void	OnCoupleResult( CAr & ar );
	void	OnDecoupleResult( CAr & ar );
#if __VER >= 13 // __COUPLE_1202
	void	OnAddCoupleExperience( CAr & ar );
#endif	// __COUPLE_1202
#endif	// __COUPLE_1117

#if __VER >= 13 // __QUEST_HELPER
private:
	void	OnNPCPos( CAr & ar );
public:
	void	SendReqNPCPos( const char* szCharKey );
#endif // __QUEST_HELPER
#if __VER >= 14 // __PCBANG
	void	OnPCBangInfo( CAr & ar );
#endif // __PCBANG
#ifdef __VTN_TIMELIMIT
	void	OnAccountPlayTime( CAr & ar );
#endif // __VTN_TIMELIMIT
#if __VER >= 14 // __SMELT_SAFETY
	void	SendSmeltSafety( OBJID objid, OBJID objMaterialId, OBJID objProtScrId, OBJID objSmeltScrId = NULL_ID );
	void	OnSmeltSafety( CAr & ar );
#endif // __SMELT_SAFETY
#ifdef __MAP_SECURITY
	void	OnWorldReadInfo( CAr & ar );
	void	SendMapKey( const char* szFileName, const char* szMapKey );
#endif // __MAP_SECURITY
#ifdef __QUIZ
	void	OnQuizSystemMessage( CAr & ar );
	void	OnQuizEventState( CAr & ar );
	void	OnQuizEventMessage( CAr & ar );
	void	OnQuizQuestion( CAr & ar );
	void	SendQuizEventEntrance();
	void	SendQuizEventTeleport( int nZone );
#endif // __QUIZ

#if __VER >= 15 // __PETVIS
	void	SendRemoveVis( int nPos );
	void	SendSwapVis( int nPos1, int nPos2 );
	void	OnActivateVisPet( CAr & ar );
	void	OnChangeMoverSfx( OBJID objId, CAr & ar );
#endif // __PETVIS

#if __VER >= 15 // __GUILD_HOUSE
	void	SendBuyGuildHouse();
	void	SendGuildHouseEnter();
	void	SendGuildHouseGoOut();
	void	OnGuildHousePacket( CAr & ar );
	void	OnGuildHouseAllInfo( CAr & ar );
	void	OnGuildHouseRemove( CAr & ar );
	void	OnRestPoint( CAr & ar );
#endif // __GUILD_HOUSE
#if __VER >= 15 // __TELEPORTER
	void	SendTeleporterReq( const char* szCharKey, int nIndex );
#endif // __TELEPORTER

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	void	OnCheckedQuest( CAr & ar );
	void	SendCheckedQuestId( int nQuestId, BOOL bCheck );
#endif // __IMPROVE_QUEST_INTERFACE

#if __VER >= 15 // __CAMPUS
	void	OnInviteCampusMember( CAr & ar );
	void	OnUpdateCampus( CAr & ar );
	void	OnRemoveCampus( CAr & ar );
	void	OnUpdateCampusPoint( CAr & ar );
	void	SendInviteCampusMember( u_long idTarget );
	void	SendAcceptCampusMember( u_long idRequest );
	void	SendRefuseCampusMember( u_long idRequest );
	void	SendRemoveCampusMember( u_long idTarget );
#endif // __CAMPUS

#ifdef __PROTECT_AWAKE
	void	SendSelectedAwakeningValue( DWORD dwObjID, DWORD dwSerialNum, BYTE bySelectFlag );
	void	OnSafeAwakening( CAr& ar );
#endif

#ifdef __GUILD_HOUSE_MIDDLE
private:
	void	OnGuildHouseTenderMainWnd( CAr & ar );
	void	OnGuildHouseTenderInfoWnd( CAr & ar );
	void	OnGuildHouseTenderResult( CAr & ar );
public:
	void	SendGuildHouseTenderMainWnd( DWORD dwGHType, OBJID objNpcId );
	void	SendGuildHouseTenderInfoWnd( OBJID objGHId );
	void	SendGuildHouseTenderJoin( OBJID objGHId, int nTenderPerin, int nTenderPenya );
#endif // __GUILD_HOUSE_MIDDLE

#ifdef	__COLOSSEUM
	void	SendColosseumStart( BYTE nState, BYTE byDifficult, BYTE byMenu );
	void	SendColosseumEnter( BYTE nState );
	void	SendColosseumEndWait( BYTE nState );
	void	OnColosseumEnter( CAr & ar );
	void	OnColosseumStart( CAr & ar );
	void	OnColosseumEndWait( CAr & ar );
	void	OnColosseumReady1( CAr & ar );
	void	OnColosseumReady2( CAr & ar );
	void	OnColosseumEndMatch( CAr & ar );
#endif // __COLOSSEUM

#ifdef __NEW_ITEM_VARUNA
	void	SendBaruna( BYTE nState, DWORD dwItem01,  DWORD dwItem02 = NULL_ID,  DWORD dwItem03 = NULL_ID,  DWORD dwItem04 = NULL_ID );
	void	OnActivateBarunaPet( CAr & ar );
#endif // __NEW_ITEM_VARUNA

#ifdef __FASHION_COMBINE
	void	SendCombineFashion( DWORD dwItem1,DWORD dwItem2,DWORD dwItem3,DWORD dwItem4,DWORD dwItem5,DWORD dwItem6,DWORD dwItem7,DWORD dwItem8 );
#endif // __FASHION_COMBINE

#ifdef __TREASUREBOX
	void	SendTreasureBoxOpen( CItemElem* pBox, CItemElem* pKey );
	void	OnRecieveTreasure( CAr &ar );
#endif//__TREASUREBOX
//________________________________________________________________________________
//	Operator commands
	void	OnShout( CAr & ar );
	void	OnPlayMusic( CAr & ar );
	void	OnPlaySound( CAr & ar );
	void	OnGetPlayerAddr( CAr & ar );
	void	OnGetPlayerCount( CAr & ar );
	void	OnGetCorePlayer( CAr & ar );
	void	OnSystem( CAr & ar );
	void	OnCaption( CAr & ar );
	
	void    OnDisguise( OBJID objid, CAr & ar );
	void    OnNoDisguise( OBJID objid, CAr & ar );
private:
	BOOL	m_bEventTextColor;
};

#endif	// __DPCLIENT_H__