#ifndef __DPCORESRVR_H__
#define __DPCORESRVR_H__

#pragma once

#include "DPMng.h"
#include "MsgHdr.h"
#include "guild.h"

#undef	theClass
#define theClass	CDPCoreSrvr
#undef	theParameters
#define theParameters	CAr & ar, LPBYTE lpBuf, u_long uBufSize

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

class CDPCoreSrvr : public CDPMng
{
public:
//	Constructions
	CDPCoreSrvr();
	virtual	~CDPCoreSrvr();
	
//	Operations
	virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpid );
	virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpid );

	void	SendPartyName( DPID dpid );
	void	SendTagResult( u_long idFrom, BYTE cbResult );
	void	OnAddConnection( DPID dpid );
	void	OnRemoveConnection( DPID dpid );
	void	SendAddVoteResult( VOTE_INSERTED_INFO & info );
	void	SendDelPlayer( u_long idPlayer, u_long idGuild );
	void	SendMemberTime( u_long idPlayer, LPCTSTR tGuildMember );
	void	SendBuyingInfo( PBUYING_INFO2 pbi2 );	// 3
	void	SendSetPlayerName( u_long idPlayer, const char* lpszPlayer, DWORD dwData, BOOL f );

#if __VER >= 11 // __SYS_PLAYER_DATA
	void	SendUpdatePlayerData( u_long idPlayer, PlayerData* pPlayerData );
#endif	// __SYS_PLAYER_DATA
#ifdef __AUTO_NOTICE
	void	SendEventLuaNotice();
#endif // __AUTO_NOTICE
#ifdef __GUILD_HOUSE_MIDDLE
	void	SendNoticeMessage( const char* szNotice );
#endif // __GUILD_HOUSE_MIDDLE

//	Handlers	
	USES_PFNENTRIES;

	void	OnCreateGuild( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnDestroyGuild( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnAddGuildMember( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnRemoveGuildMember( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnGuildMemberLv( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnGuildClass( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnGuildNickName( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnChgMaster( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnGuildLogo( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnGuildNotice( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnAddVote( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnRemoveVote( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnCloseVote( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnCastVote( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnGuildPenya( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnGuildAuthority( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnGuildSetName( CAr & ar, LPBYTE lpBuf, u_long uBufSize );

	void	OnAcptWar( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnWarEnd( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnSurrender( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnWarDead( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnWarMasterAbsent( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
#ifdef __RT_1025
	void	OnAddMessenger( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnDeleteMessenger( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnUpdateMessenger( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
public:
	void	SendRemovePlayerFriend( u_long uPlayerId, u_long uFriendId );
private:
#else	// __RT_1025
	void	OnRemoveFriend( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
#endif	// __RT_1025
	void	OnAddPartyName( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnTag( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnWanted( CAr & ar, LPBYTE lpBuf, u_long uBufSize );	
	void	OnSnoopGuild( CAr & ar, LPBYTE lpBuf, u_long uBufSize );	
#ifdef __SERVERLIST0911
	void	OnServerEnable( CAr & ar, LPBYTE lpBuf, u_long uBufSize );	
#endif	// __SERVERLIST0911
};

#endif	// __DPLOGINSRVR_H__