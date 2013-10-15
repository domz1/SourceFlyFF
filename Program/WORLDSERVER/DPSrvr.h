#ifndef __DPSRVR_H__
#define __DPSRVR_H__

#pragma once

#include "DPMng.h"
#include "MsgHdr.h"
#include "Snapshot.h"

#undef	theClass
#define theClass	CDPSrvr
#undef theParameters
#define theParameters	CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize

class CDPSrvr : public CDPMng
{
public:
	// Constructions
	CDPSrvr();
	virtual	~CDPSrvr();

	// Operations
	virtual	void SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual void UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );

	USES_PFNENTRIES;

	// Handlers
	void	OnAddConnection( DPID dpid );
	void	OnRemoveConnection( DPID dpid );
	void	OnAddUser( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
	void	OnRemoveUser( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
	void	OnChat( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnDoEquip( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnCtrlCoolTimeCancel( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );		
	void	OnMoveItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnDropItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnDropGold( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnTrade( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnConfirmTrade( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnConfirmTradeCancel( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnTradePut( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnTradePull( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnTradePutGold( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnTradeClearGold( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnTradeCancel( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnTradeOk( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnTradelastConfrim( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnPVendorOpen( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnPVendorClose( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnRegisterPVendorItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnUnregisterPVendorItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnQueryPVendorItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnBuyPVendorItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnEnchant( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);

#ifdef __FASTJOBCHANGE
	void	OnUpdateJob( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
#endif//__FASTJOBCHANGE

#if __VER >= 14 // __SMELT_SAFETY
	void	OnSmeltSafety( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
#endif // __SMELT_SAFETY
#if __VER >= 10 // __REMOVE_ATTRIBUTE
	void	OnRemoveAttribute( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
#endif // __REMOVE_ATTRIBUTE
#if __VER >= 13 // __EXT_ENCHANT
	void	OnChangeAttribute( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpbuf, u_long uBufSize );
#endif // __EXT_ENCHANT
	void	OnCreateSfxObj( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnPiercing( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnPiercingSize( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
#if __VER >= 11 // __PIERCING_REMOVE
	void	OnPiercingRemove( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
#endif // __PIERCING_REMOVE
	void	OnExpBoxInfo( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnItemTransy( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnUpgradeBase( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnCommercialElem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnMotion( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnRepairItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnScriptDialogReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnReplace( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
	void	OnOpenShopWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnCloseShopWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnBuyItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#if __VER >= 11 // __GUILDCOMBATCHIP
	void	OnBuyChipItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif // __GUILDCOMBATCHIP
	void	OnSellItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnOpenBankWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnOpenGuildBankWnd(CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnCloseGuildBankWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnPutItemGuildBank( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnGetItemGuildBank( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnGuildBankMoveItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnCloseBankWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnDoUseSkillPoint( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnBankToBank( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnPutItemBank( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnGetItemBank( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnPutGoldBank( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpbuf, u_long uBufSize );
	void	OnGetGoldBank( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpbuf, u_long uBufSize );
	void	OnMoveBankItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnChangeBankPass( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnConfirmBank( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnRevival( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnRevivalLodestar( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnRevivalLodelight( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize ); 
	void	OnSetLodelight( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnIncStatLevel( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnPlayerMoved( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnPlayerBehavior( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnPlayerMoved2( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnPlayerBehavior2( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnPlayerAngle( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnPlayerCorr( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnPlayerCorr2( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnPlayerSetDestObj( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnCorrReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnCreateGuildCloak( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnQueryGetPos( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnGetPos( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnQueryGetDestObj( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnGetDestObj( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnSkillTaskBar( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnAddAppletTaskBar( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnRemoveAppletTaskBar( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnAddItemTaskBar( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnRemoveItemTaskBar( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnPartyRequest( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnAddFriendNameReqest( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnPartyRequestCancle( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnPartySkillUse( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnAddFriendReqest( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnAddFriendCancel( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnGetFriendName( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnDuelRequest( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnDuelYes( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnDuelNo( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnDuelPartyRequest( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnDuelPartyYes( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnDuelPartyNo( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnMoverFocus( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnRemoveQuest( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#if __VER >= 11 // __SYS_PLAYER_DATA
	void	OnQueryPlayerData( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnQueryPlayerData2( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#else	// __SYS_PLAYER_DATA
	void	OnQueryPlayerString( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnQueryPlayerListString( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif	// __SYS_PLAYER_DATA
#if __VER >= 11 // __SYS_POCKET
	void	OnAvailPocket( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnMoveItemOnPocket( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif	// __SYS_POCKET
#ifdef __JEFF_11
	void	OnQuePetResurrection( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif	// __JEFF_11
#ifdef __JEFF_11_4
	void	OnArenaEnter( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnArenaExit( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif	// __JEFF_11_4
	void	OnGuildInvite( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnIgnoreGuildInvite( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnEndSkillQueue( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnActionPoint( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnDoUseItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#if __VER >= 11 // __SYS_IDENTIFY
	void	OnDoUseItemTarget( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnAwakening( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnRemoveItemLevelDown( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnBlessednessCancel( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif	// __SYS_IDENTIFY
	void	OnMeleeAttack( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnMeleeAttack2( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnMagicAttack( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnRangeAttack( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnUseSkill( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnSfxID( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnSfxHit( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnSfxClear( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnSnapshot( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnSetTarget( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
//	void	OnDoCollect( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnTeleSkill( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnPlayerPos( CAr & ar, CUser* pUser );
	void	OnPlayerDestPos( CAr & ar, CUser* pUser );
//	void	OnPlayerDestAngle( CAr & ar, CUser* pUser );
	void	OnModifyMode( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
	void	OnSummonPlayer( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
	void	OnTeleportPlayer( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
	void	OnExpUp( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnChangeJob( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnSetHair( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnBlock( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnError( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnShipActMsg( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnLocalPosFromIA( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnGuildLogo( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnGuildContribution( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnGuildNotice( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnRequestGuildRank( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnEnterChattingRoom( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnChatting( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnOpenChattingRoom( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnCloseChattingRoom( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnCommonPlace( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnSetNaviPoint( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnGameMasterWhisper( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnNWWantedGold( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnNWWantedList( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnNWWantedInfo( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnNWWantedName( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnReqLeave( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnResurrectionOK( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnResurrectionCancel( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnChangeMode( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnStateMode( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnQuerySetPlayerName( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnQuerySetGuildName( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnDoEscape( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnCheering( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnQueryEquip( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#if __VER >= 12 // __UPDATE_OPT
	void	OnOptionEnableRenderMask( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif	// __UPDATE_OPT
	void	OnQueryEquipSetting( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnFoucusObj( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnReturnScroll ( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnChangeFace( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnQueryPostMail( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnQueryRemoveMail( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnQueryGetMailItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnQueryGetMailGold( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnQueryReadMail( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnQueryMailBox( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnGCApp( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnGCCancel( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnGCRequestStatus( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnGCSelectPlayer( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnGCSelectMap( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnGCJoin( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnGCGetPenyaGuild( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnGCGetPenyaPlayer( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#if __VER < 8 // #ifndef __GUILDCOMBAT_85
	void	OnGCGetItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif // __VER < 8
	void	OnGCTele( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnGCPlayerPoint( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnSummonFriend( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnSummonFriendConfirm( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnSummonFriendCancel( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnSummonParty( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnSummonPartyConfirm( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );

	void	OnRemoveInvenItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnRandomScroll( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	
#if __VER >= 12 // __ITEMCREATEMON_S0602
	void	OnCreateMonster( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif // __ITEMCREATEMON_S0602

	void	OnBuyingInfo( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	
#if __VER >= 8 //__CSC_VER8_5
	void	OnCreateAngel(CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnAngleBuff(CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif //__CSC_VER8_5
	
#ifdef __EVE_MINIGAME
	void OnKawibawiboStart( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void OnKawibawiboGetItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void OnReassembleStart( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void OnReassembleOpenWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void OnAlphabetOpenWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void OnAlphabetStart( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void OnFiveSystemOpenWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void OnFiveSystemBet( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void OnFiveSystemStart( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void OnFiveSystemDestroyWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif // __EVE_MINIGAME

#if __VER >= 9 // __ULTIMATE
	void OnUltimateMakeItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void OnUltimateMakeGem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void OnUltimateTransWeapon( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void OnUltimateSetGem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void OnUltimateRemoveGem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void OnUltimateEnchantWeapon( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif // __ULTIMATE
	
#ifdef __TRADESYS
	void OnExchange( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif // __TRADESYS

#if __VER >= 9	// __PET_0410
	void	OnPetRelease( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnUsePetFeed( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnMakePetFeed( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnPetTamerMistake( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnPetTamerMiracle( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnFeedPocketInactive( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif	// __PET_0410

#if __VER >= 11 // __SYS_COLLECTING
	void	OnQueryStartCollecting( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnQueryStopCollecting( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif	// __SYS_COLLECTING

#if __VER >= 10 // __LEGEND	//	9차 전승시스템	Neuz, World, Trans
	void OnLegendSkillStart( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif	//__LEGEND	//	9차 전승시스템	Neuz, World, Trans
	
#if __VER >= 9 // __CSC_VER9_2
	void OnModifyStatus( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif //__CSC_VER9_2

#if __VER >= 11 // __GUILD_COMBAT_1TO1
	void OnGC1to1TenderOpenWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void OnGC1to1TenderView( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void OnGC1to1Tender( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void OnGC1to1CancelTender( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void OnGC1to1FailedTender( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void OnGC1to1MemberLineUpOpenWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void OnGC1to1MemberLineUp( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void OnGC1to1TeleportToNPC( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void OnGC1to1TeleportToStage( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif // __GUILD_COMBAT_1TO1

#if __VER >= 11 // __MA_VER11_04	// 길드 창고 로그 기능 world,database,neuz
	void OnQueryGuildBankLogList( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif //__MA_VER11_04	// 길드 창고 로그 기능 world,database,neuz
#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
	void OnSealCharReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void OnSealCharConmReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void OnSealCharGetReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
#if __VER >= 13 // __HONORABLE_TITLE			// 달인
	void OnHonorListReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void OnHonorChangeReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif	// __HONORABLE_TITLE			// 달인
#ifdef __NPC_BUFF
	void OnNPCBuff( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif // __NPC_BUFF

#if __VER >= 12 // __SECRET_ROOM
	void OnSecretRoomTenderOpenWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void OnSecretRoomLineUpOpenWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void OnSecretRoomTender( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void OnSecretRoomTenderCancelReturn( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void OnSecretRoomLineUpMember( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void OnSecretRoomEntrance( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void OnSecretRoomTeleportToNPC( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void OnSecretRoomTenderView( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void OnTeleportSecretRoomDungeon( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );

	BOOL IsInviteAbleGuild( CUser* pUser );	//길드원 변경가능한 길드인가?(초대 및 탈퇴)
#endif // __SECRET_ROOM

#if __VER >= 12 // __TAX
	void OnSetTaxRate( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif // __TAX

#if __VER >= 12 // __HEAVEN_TOWER
	void OnTeleportToHeavenTower( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif // __HEAVEN_TOWER

public:
	void	QueryDestroyPlayer( DPID dpidCache, DPID dpidUser, DWORD dwAuthKey, u_long idPlayer );
	void	PutCreateItemLog( CUser* pUser, CItemElem* pItemElem, const char* szAction, const char* recv ) ;
	void	OnLogItem( LogItemInfo & info, CItemElem* pItemElem = NULL, int nItemCount = 0 );
	void	InviteParty( u_long uLeaderid, u_long uMemberid, BOOL bTroup );
	void	InviteCompany( CUser* pUser, OBJID objid );
	void	UpdateGuildBank(CGuild* p_GuildBank, int p_Mode, BYTE cbUpdate = 0, u_long idPlayer = 0, CItemElem* pItemElem = NULL, DWORD dwPenya = 0, short nItemCount = 0 );
	BOOL	ClosePVendor( CUser* pUser, OBJID objidVendor );
	BOOL	IsPiercingSize( CUser* pUser, CItemElem* pItemElem0, CItemElem* pItemElem1, CItemElem* pItemElem2, int& nError );

	BOOL	DoUseItemTarget_GenRandomOption( CUser* pUser, CItemElem* pTarget, int nKind,
		int nOk, int nMismatchKind, int nHasOption,
		const char* szOperation, const char* szReceive );
	BOOL	DoUseItemTarget_InitializeRandomOption( CUser* pUser, CItemElem* pTarget, int nKind,
		int nOk, int nError,
		const char* szOperation, const char* szRecv );
	BOOL	DoUseItemTarget_ItemLevelDown( CUser* pUser, CItemElem* pMaterial, CItemElem* pTarget );

#if __VER >= 12 // __LORD
public:
	void	PutItemLog( CUser* pUser, const char* szAction, const char* szContext, CItemElem* pItem, int nNum = 0 );
	void	PutPenyaLog( CUser* pUser, const char* szAction, const char* szContext, int nCost );
private:
	void	OnElectionAddDeposit( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnElectionSetPledge( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnElectionIncVote( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnLEventCreate( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
	void	OnLordSkillUse( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif	// __LORD
#if __VER >= 12 // __PET_0519
	void	OnTransformItem( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );		// 알변환 핸들러
	void	OnPickupPetAwakeningCancel( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );		// 픽업펫 각성 취소
#endif	// __PET_0519

#if __VER >= 12 // __MOD_TUTORIAL
	void	OnTutorialState( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
#endif	// __MOD_TUTORIAL

#ifdef __AZRIA_1023
	void	OnDoUseItemInput( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
#endif	// __AZRIA_1023
#ifdef __PET_1024
	void	OnClearPetName( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
#endif	// __PET_1024

#if __VER >= 13 // __RAINBOW_RACE
	void	OnRainbowRacePrevRankingOpenWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
	void	OnRainbowRaceApplicationOpenWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
	void	OnRainbowRaceApplication( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
	void	OnRainbowRaceMiniGamePacket( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
	void	OnRainbowRaceReqFinish( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
#endif // __RAINBOW_RACE

#if __VER >= 13 // __HOUSING
	void	OnHousingSetupFurniture( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
	void	OnHousingSetVisitAllow( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
	void	OnHousingVisitRoom( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
	void	OnHousingVisitableList( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
	void	OnHousingGoOut( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
#endif // __HOUSING

#if __VER >= 13 // __QUEST_HELPER
	void	OnReqQuestNPCPos( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
#endif // __QUEST_HELPER

#if __VER >= 13 // __COUPLE_1117
	void	OnPropose( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
	void	OnRefuse( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
	void	OnCouple( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
	void	OnDecouple( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
#endif	// __COUPLE_1117
#ifdef __MAP_SECURITY
	void	OnMapKey( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
#endif // __MAP_SECURITY
#ifdef __QUIZ
	void	OnQuizEventEntrance( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
	void	OnQuizEventTeleport( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
#endif // __QUIZ
#if __VER >= 15 // __PETVIS
	void	OnRemoveVis( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
	void	OnSwapVis( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
#endif // __PETVIS
#if __VER >= 15 // __GUILD_HOUSE
	void	OnBuyGuildHouse( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
	void	OnGuildHousePacket( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
	void	OnGuildHouseEnter( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
	void	OnGuildHouseGoOut( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
#endif // __GUILD_HOUSE
#if __VER >= 15 // __TELEPORTER
	void	OnTeleporterReq( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
#endif // __TELEPORTER
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	void	OnCheckedQuest( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
#endif // __IMPROVE_QUEST_INTERFACE
#if __VER >= 15 // __CAMPUS
	void	OnInviteCampusMember( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
	void	OnAcceptCampusMember( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
	void	OnRefuseCampusMember( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
	void	OnRemoveCampusMember( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
#endif // __CAMPUS


	//	mulcom	BEGIN100405	각성 보호의 두루마리
	void	OnItemSelectAwakeningValue( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
	//	mulcom	END100405	각성 보호의 두루마리


#ifdef __GUILD_HOUSE_MIDDLE
	void	OnGuildHouseTenderMainWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
	void	OnGuildHouseTenderInfoWnd( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
	void	OnGuildHouseTenderJoin( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
#endif // __GUILD_HOUSE_MIDDLE

#ifdef	__COLOSSEUM
	void	OnColosseum( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
#endif //__COLOSSEUM

#ifdef __NEW_ITEM_VARUNA
	void	OnBaruna( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
#endif // __NEW_ITEM_VARUNA

#ifdef __BARUNA_V18
/*		void	OnBarunaSeed( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
		void	OnBarunaOffer( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
		void	OnBarunaExtractOffer( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
		void	OnBarunaOfferSeed( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
		void	OnBarunaEnchant( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
		void	OnBarunaPierce( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );*/
		void	OnBarunaElement( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
		void	OnBarunaAwake( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
		void	OnBarunaAwakeCancel( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif//__BARUNA_V18

#ifdef __FASHION_COMBINE
	void	OnFashionCombine( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
#endif // __FASHION_COMBINE

#ifdef __16_SHOP
	void	OnBuyItemEx( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize );
#endif//__16_SHOP

#ifdef __TREASUREBOX
	void	OnTreasureBoxOpen( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE, u_long );
#endif//__TREASUREBOX

private:
	DPID	m_dpidCache;		// 캐쉬서버 DPID
};


#endif // __DPSRVR_H__