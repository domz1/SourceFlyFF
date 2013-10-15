#ifndef __DPCACHESRVR_H__
#define __DPCACHESRVR_H__

#pragma once

#include "DPMng.h"
#include "ServerDesc.h"
#include "Player.h"
#include "guild.h"

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

#undef	theClass
#define theClass	CDPCacheSrvr
#undef	theParameters
#define theParameters	CAr & ar, DPID, DPID, u_long

class CGuild;

class CDPCacheSrvr : public CDPMng
{
private:
	CServerDescArray	m_apServer;

public:
//	Constructions
	CDPCacheSrvr();
	virtual	~CDPCacheSrvr();
//	Overrides
	virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
//	Operations
	void	SendProcServerList( DPID dpid );

	void	SendHdr( DWORD dwHdr, DPID dpidCache, DPID dpidUser );
	void	SendWhisper( const CHAR* sPlayerFrom, const CHAR* sPlayerTo, u_long idFrom, u_long idTo, const CHAR* lpString, CPlayer* pTo, int nSearch = 0 );
	void	SendSay( const CHAR* sPlayerFrom, const CHAR* sPlayerTo, u_long idFrom, u_long idTo, const CHAR* lpString, CPlayer* pTo, int nSearch = 0 );
	void	SendGMSay( const CHAR* sPlayerFrom, const CHAR* sPlayerTo, const CHAR* lpString, CPlayer* pTo );
	void	SendFriendState( CPlayer* pTo );
	void	SendFriendJoin( CPlayer* pTo, CPlayer* pFriend );
	void	SendFriendLogOut( CPlayer* pTo, u_long uidPlayer );
	void	SendSetFriendState( CPlayer* pTo );
	void	SendFriendNoIntercept( CPlayer* pTo, CPlayer* pFriend, int state );
	void	SendFriendIntercept( CPlayer* pPlayer, CPlayer* pFriend );
	void	SendFriendIntercept( CPlayer* pPlayer, u_long uFriendid );
	void	SendFriendNoIntercept( CPlayer* pTo, u_long uFriendid, int state );
	void	SendOneFriendState( CPlayer* pTo, u_long uidPlayer, DWORD dwState );
	void	SendModifyMode( DWORD dwMode, BYTE f, u_long idFrom, CPlayer* pTo );
#ifdef __LAYER_1015
	void	SendSummonPlayer( u_long idOperator, u_long uIdofMulti, DWORD dwWorldID, const D3DXVECTOR3 & vPos, CPlayer* pPlayer, int nLayer );
#else	// __LAYER_1015
	void	SendSummonPlayer( u_long idOperator, u_long uIdofMulti, DWORD dwWorldID, const D3DXVECTOR3 & vPos, CPlayer* pPlayer );
#endif	// __LAYER_1015
	void	SendTeleportPlayer( u_long idOperator, CPlayer* pPlayer );
	void	SendKillPlayer( CPlayer* pPlayer );
	void	SendGetPlayerAddr( const CHAR* lpszPlayer, const CHAR* lpAddr, CPlayer* pOperator );
	void	SendGetPlayerCount( u_short uCount, CPlayer* pOperator );
	void	SendGetCorePlayer( CPlayer* pOperator );
	void	SendSystem( const CHAR* lpString );
	void	SendCaption( const CHAR* lpString, DWORD dwWorldId, BOOL bSmall );
		
	void	SendDefinedText( int dwText, DPID dpidCache, DPID dpidUser, LPCSTR lpszFormat, ... );
	void	SendErrorParty( DWORD dwError, CPlayer* pPlayer );
	void	SendAddFriend( u_long uLeader, u_long uMember, LONG nLeaderJob, BYTE nLeaderSex, char * szLeaderName, CPlayer* pMember );
	void	SendAddFriendNotFound( char * szMemberName, CPlayer* pLeader );
	void	SendBlock( BYTE nGu, char *szName, CPlayer* pTo );
	void	SendTagResult( CPlayer* pPlayer, BYTE cbResult );
	void	SendGameRate( FLOAT fRate, BYTE nFlag );

	void	SendSetPlayerName( u_long idPlayer, const char* lpszPlayer );
	void	SendSnoop( const char* lpszString, CPlayer* pSnoop );

#if __VER >= 11 // __SYS_PLAYER_DATA
	void	SendUpdatePlayerData( u_long idPlayer, PlayerData* pPlayerData, CPlayer* pTo );
	void	SendLogout( u_long idPlayer, CPlayer* pTo );
#endif	// __SYS_PLAYER_DATA

#ifdef __QUIZ
	void	SendQuizSystemMessage( int nDefinedTextId, BOOL bAll, int nChannel, int nTime );
#endif // __QUIZ

	USES_PFNENTRIES;
//	Handlers
	void	OnAddConnection( DPID dpid );
	void	OnRemoveConnection( DPID dpid );

	void	OnAddPlayer( CAr & ar, DPID dpidCache, DPID dpidUser, u_long );
	void	OnQueryRemovePlayer( CAr & ar, DPID dpidCache, DPID dpidUser, u_long );

	void	OnAddPartyMember( CAr & ar, DPID dpidCache, DPID dpidUser, u_long );
	void	OnRemovePartyMember( CAr & ar, DPID dpidCache, DPID dpidUser, u_long );
	void	OnPartyChangeTroup( CAr & ar, DPID dpidCache, DPID dpidUser, u_long );
	void	OnPartyChangeName( CAr & ar, DPID dpidCache, DPID dpidUser, u_long );
	void	OnPartyChangeItemMode( CAr & ar, DPID dpidCache, DPID dpidUser, u_long );
	void	OnPartyChangeExpMode( CAr & ar, DPID dpidCache, DPID dpidUser, u_long );

	void	OnAddFriend( CAr & ar, DPID dpidCache, DPID dpidUser, u_long );
	void	OnGetFriendState( CAr & ar, DPID dpidCache, DPID dpidUser, u_long );
	void	OnSetFrinedState( CAr & ar, DPID dpidCache, DPID dpidUser, u_long );
	void	OnFriendInterceptState( CAr & ar, DPID dpidCache, DPID dpidUser, u_long );
	void	OnRemoveFriend( CAr & ar, DPID dpidCache, DPID dpidUser, u_long );
public:
	void	SendUpdateGuildRank();
	void	OnDestroyGuild( CAr & ar, DPID dpidCache, DPID dpidUser, u_long );
	void	OnAddGuildMember( CAr & ar, DPID dpidCache, DPID dpidUser, u_long );
	void	OnRemoveGuildMember( CAr & ar, DPID dpidCache, DPID dpidUser, u_long );
	void	OnGuildMemberLv( CAr & ar, DPID dpidCache, DPID dpidUser, u_long );
	void	OnGuildAuthority( CAr & ar, DPID dpidCache, DPID dpidUser, u_long );
	void	OnGuildPenya( CAr & ar, DPID dpidCache, DPID dpidUser, u_long );
	void	OnGuildSetName( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize );
	void	OnGuildRank( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize );
	void	OnGuildClass( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize );
	void	OnGuildNickName( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize );
	void	OnChgMaster( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize );
	void	SendAddGuildMember( const GUILD_MEMBER_INFO & info, const char* lpszPlayer, u_long idGuild, CPlayer* pPlayer );
public:
	void	SendRemoveGuildMember( u_long idPlayer, u_long idGuild, CPlayer* pPlayer );
//private:
	void	SendGuildMemberLv( u_long idPlayer, int nMemberLv, CPlayer* pPlayer );
	void	SendGuildSetName( u_long idGuild, const char* szName );
	void	SendGuildClass( u_long idPlayer, int nClass, CPlayer* pPlayer );
	void	SendGuildNickName( u_long idPlayer, LPCTSTR strNickName, CPlayer* pPlayer );
	void	SendChgMaster( u_long idPlayer, u_long idPlayer2, CPlayer* pPlayer );
public:
	void	SendGuild( CGuild* pGuild, CPlayer* pPlayer );
	void	SendGuildChat( const char* lpszPlayer, const char* sChat, CPlayer* pPlayer, OBJID objid );
	void	SendGuildMemberLogin( CPlayer* pTo, BYTE nLogin, u_long uPlayerId, u_long uMultiNo );
	void	SendGuildMemberGameJoin( CPlayer * pTo, int nMaxLogin, u_long uLoginPlayerId[], u_long uLoginGuildMulti[] );
	void	SendGuildError( CPlayer * pTo, int nError );
	void	SendGuildRank( CPlayer * pTo );
	void	SendGuildMsgControl( GUILD_MSG_HEADER* pHeader, DWORD pPenya, BYTE cbCloak );

private:
	void	SendDeclWar( u_long idDecl, const char* pszMaster, CPlayer* pPlayer );
	void	SendAcptWar( u_long idWar, u_long idDecl, u_long idAcpt );
	void	SendSurrender( u_long idWar, CGuild* pDecl, CGuild* pAcpt, u_long idPlayer, const char* sPlayer, BOOL bDecl );	// 1
	void	SendSurrender( u_long idWar, u_long idPlayer, const char* sPlayer, BOOL bDecl, CPlayer* pPlayer );	// 2
	void	OnDeclWar( CAr & ar, DPID dpidCache, DPID dpidUser, u_long );
	void	OnAcptWar( CAr & ar, DPID dpidCache, DPID dpidUser, u_long );
	void	OnSurrender( CAr & ar, DPID dpidCache, DPID dpidUser, u_long );
	void	OnQueryTruce( CAr & ar, DPID dpidCache, DPID dpidUser, u_long );
	void	OnAcptTruce( CAr & ar, DPID dpidCache, DPID dpidUser, u_long );
public:
	void	SendWarEnd( u_long idWar, int nWptDecl, int nWptAcpt, int nType );
	void	SendWarDead( u_long idWar, const char* lpszPlayer, BOOL bDecl, CPlayer* pPlayer );
	void	SendQueryTruce( CPlayer* pPlayer );
	void	SendBuyingInfo( PBUYING_INFO2 pbi2, CPlayer* pPlayer );

private:
	void	OnAddVote( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize );
	void	OnRemoveVote( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize );
	void	OnCloseVote( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize );
	void	OnCastVote( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize );
	void	OnPartyChangeLeader( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize );
	void	OnSendTag( CAr & ar, DPID dpidCache, DPID dpidUser, u_long uBufSize );
};

inline void CDPCacheSrvr::SendHdr( DWORD dwHdr, DPID dpidCache, DPID dpidUser )
{
	BEFORESENDSOLE( ar, dwHdr, dpidUser );
	SEND( ar, this, dpidCache );
}

#endif	// __DPCACHESRVR_H__