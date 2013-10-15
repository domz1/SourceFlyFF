#include "stdafx.h"
#include "dpcachesrvr.h"
#include "dpdatabaseclient.h"
#include "dpcoresrvr.h"
#include "msghdr.h"
#include "player.h"
#include "mytrace.h"
#include "misc.h"
#include "definetext.h"
#include "party.h"
#include "guild.h"
#include "guildwar.h"
#include "WantedList.h"
#include "Environment.h"
extern	CPlayerMng			g_PlayerMng;
extern	CDPCacheSrvr		g_DPCacheSrvr;
extern	CDPDatabaseClient	g_dpDatabaseClient;
extern	CMyTrace			g_MyTrace;
extern	CPartyMng			g_PartyMng;
extern	CGuildMng			g_GuildMng;
extern	CGuildWarMng		g_GuildWarMng;
extern	DWORD				s_dwEvent;

#ifndef __ENVIRONMENT_EFFECT
extern	CEnvironment		g_Environment;
#endif // __ENVIRONMENT_EFFECT

#ifdef __EVENT0913
extern	BOOL	s_bEvent0913;
#endif	// __EVENT0913
#ifdef __EVENT1206
extern	BOOL	s_bEvent1206;
#endif	// __EVENT1206

#if __VER >= 14 // __INSTANCE_DUNGEON
#include "InstanceDungeonParty.h"
#endif // __INSTANCE_DUNGEON

CDPCoreSrvr		g_dpCoreSrvr;

#define VERIFYBUFFERSIZE( uRecvd, uBufSize )	\
		if( uRecvd >= ( uBufSize ) )	{	WriteLog( "%s, %d", __FILE__, __LINE__ );	return;		}

		
CDPCoreSrvr::CDPCoreSrvr()
{
	BEGIN_MSG;
	ON_MSG( PACKETTYPE_MYREG, OnAddConnection )
	ON_MSG( PACKETTYPE_RECHARGE_IDSTACK, OnRecharge )
	ON_MSG( PACKETTYPE_JOIN, OnJoin )

	ON_MSG( PACKETTYPE_WHISPER, OnWhisper );
	ON_MSG( PACKETTYPE_SAY, OnSay );
	ON_MSG( PACKETTYPE_MODIFYMODE, OnModifyMode );
	ON_MSG( PACKETTYPE_SHOUT, OnShout );
	ON_MSG( PACKETTYPE_GMSAY, OnGMSay );
	ON_MSG( PACKETTYPE_PLAYMUSIC, OnPlayMusic );
	ON_MSG( PACKETTYPE_PLAYSOUND, OnPlaySound );
	ON_MSG( PACKETTYPE_SUMMONPLAYER, OnSummonPlayer );
	ON_MSG( PACKETTYPE_TELEPORTPLAYER, OnTeleportPlayer );
	ON_MSG( PACKETTYPE_KILLPLAYER, OnKillPlayer );
	ON_MSG( PACKETTYPE_GETPLAYERADDR, OnGetPlayerAddr );
	ON_MSG( PACKETTYPE_GETPLAYERCOUNT, OnGetPlayerCount );
	ON_MSG( PACKETTYPE_GETCOREPLAYER, OnGetCorePlayer );
	ON_MSG( PACKETTYPE_SYSTEM, OnSystem );	
	ON_MSG( PACKETTYPE_CAPTION, OnCaption );	
	ON_MSG( PACKETTYPE_ADDPARTYEXP, OnAddPartyExp );
	ON_MSG( PACKETTYPE_REMOVEPARTYPOINT, OnRemovePartyPoint );
#if __VER < 11 // __SYS_PLAYER_DATA
	ON_MSG( PACKETTYPE_MEMBERLEVEL, OnMemberLevel );
	ON_MSG( PACKETTYPE_MEMBERJOB, OnMemberJob );
	ON_MSG( PACKETTYPE_CHANGEJOB, OnFriendChangeJob );
#endif	// __SYS_PLAYER_DATA
#ifdef __ENVIRONMENT_EFFECT
	ON_MSG( PACKETTYPE_FALLRAIN, OnFallRain );
	ON_MSG( PACKETTYPE_STOPRAIN, OnStopRain );
#else // __ENVIRONMENT_EFFECT
	ON_MSG( PACKETTYPE_FALLSNOW, OnFallSnow );
	ON_MSG( PACKETTYPE_FALLRAIN, OnFallRain );
	ON_MSG( PACKETTYPE_STOPSNOW, OnStopSnow );
	ON_MSG( PACKETTYPE_STOPRAIN, OnStopRain );
#endif // __ENVIRONMENT_EFFECT
	ON_MSG( PACKETTYPE_PARTYCHAT, OnPartyChat );
	ON_MSG( PACKETTYPE_PARTYSKILLUSE, OnPartySkillUse );
	ON_MSG( PACKETTYPE_PARTYLEVEL, OnPartyLevel );

	ON_MSG( PACKETTYPE_LOADCONSTANT, OnLoadConstant );
	ON_MSG( PACKETTYPE_GAMERATE, OnGameRate );
	ON_MSG( PACKETTYPE_SETMONSTERRESPAWN, OnSetMonsterRespawn );

	ON_MSG( PACKETTYPE_CREATE_GUILD, OnCreateGuild );
	ON_MSG( PACKETTYPE_GUILD_CHAT, OnGuildChat );
	ON_MSG( PACKETTYPE_GUILD_MSG_CONTROL, OnGuildMsgControl );

	ON_MSG( PACKETTYPE_ADDFRIENDNAMEREQEST, OnAddFriendNameReqest );
	ON_MSG( PACKETTYPE_BLOCK, OnBlock );
	ON_MSG( PACKETTYPE_WC_GUILDLOGO, OnGuildLogo );				// 로고 변경 
	ON_MSG( PACKETTYPE_WC_GUILDCONTRIBUTION, OnGuildContribution );	// 공헌도 
	ON_MSG( PACKETTYPE_WC_GUILDNOTICE, OnGuildNotice );			// 공지사항  
	ON_MSG( PACKETTYPE_GUILD_DB_REALPENYA, OnGuildRealPenya );	// 월급을 주었다는것을 코어와 유저에게 알려줌
#if __VER < 11 // __SYS_PLAYER_DATA
	ON_MSG( PACKETTYPE_CHANGEGUILDJOBLEVEL, OnChangeGuildJobLevel ); // 길드원이 레벨이나 직업이 바뀌었음
#endif	// __SYS_PLAYER_DATA
	ON_MSG( PACKETTYPE_WAR_DEAD, OnWarDead );
	ON_MSG( PACKETTYPE_WAR_MASTER_ABSENT, OnWarMasterAbsent );
	ON_MSG( PACKETTYPE_WAR_TIMEOUT, OnWarTimeout );
	ON_MSG( PACKETTYPE_WC_WANTED_GOLD, OnWCWantedGold );
	ON_MSG( PACKETTYPE_WC_WANTED_REWARD, OnWCWantedReward );
	ON_MSG( PACKETTYPE_SETPARTYDUEL, OnSetPartyDuel );
	ON_MSG( PACKETTYPE_QUERYSETGUILDNAME, OnQuerySetGuildName );
	ON_MSG( PACKETTYPE_SETSNOOP, OnSetSnoop );
	ON_MSG( PACKETTYPE_SETSNOOPGUILD, OnSetSnoopGuild );
	ON_MSG( PACKETTYPE_CHAT, OnChat );
	ON_MSG( PACKETTYPE_PING, OnPing );
	ON_MSG( PACKETTYPE_GUILDCOMBAT_STATE, OnGCState );
	ON_MSG( PACKETTYPE_REMOVEPARTY_GUILDCOMBAT, OnGCRemoveParty );
	ON_MSG( PACKETTYPE_ADDPARTY_GUILDCOMBAT, OnGCAddParty );
#if __VER >= 14 // __INSTANCE_DUNGEON
	ON_MSG( PACKETTYPE_INSTANCEDUNGEON_CREATE, OnInstanceDungeonCreate );
	ON_MSG( PACKETTYPE_INSTANCEDUNGEON_DESTROY, OnInstanceDungeonDestroy );
	ON_MSG( PACKETTYPE_INSTANCEDUNGEON_SETCOOLTIME, OnInstanceDungeonSetCoolTimeInfo );
#endif // __INSTANCE_DUNGEON
#ifdef __QUIZ
	ON_MSG( PACKETTYPE_QUIZ_NOTICE, OnQuizSystemMessage );
#endif // __QUIZ
	m_nGCState = 1;
#ifndef __STL_0402
	m_toHandle.SetSize( 16, 16, 8 );
#endif	// __STL_0402
}

CDPCoreSrvr::~CDPCoreSrvr()
{

}

void CDPCoreSrvr::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_CREATEPLAYERORGROUP:
			{
				break;
			}
		case DPSYS_DESTROYPLAYERORGROUP:
			{
				LPDPMSG_DESTROYPLAYERORGROUP lpDestroyPlayer	= (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
				OnRemoveConnection( lpDestroyPlayer->dpId );
				break;
			}
	}
}

void CDPCoreSrvr::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	static	DPID	dpid;
	static	DWORD	dwWorldID;
	static	D3DXVECTOR3		vPos;
	static	BYTE	lptmp[8];
	static	size_t	nSize	= sizeof(DPID);
	
	CAr ar( (LPBYTE)lpMsg + nSize + nSize, dwMsgSize - ( nSize + nSize ) );
	GETTYPE( ar );	// dw
	void ( theClass::*pfn )( theParameters )	=	GetHandler( dw );
	
	if( pfn ) 
	{
		( this->*( pfn ) )( ar, idFrom, *(UNALIGNED LPDPID)lpMsg, *(UNALIGNED LPDPID)( (LPBYTE)lpMsg + nSize ), ( dwMsgSize - nSize - nSize ) );
	}
	else {
		switch( dw )
		{
			case PACKETTYPE_PASSAGE:
				{
					WORD wWorldSrvr		= LOWORD( *(UNALIGNED DWORD*)lpMsg );
//					ASSERT( wWorldSrvr >= 0 );

					if( wWorldSrvr > 0 ) {	// explicit
						dpid	= GetWorldSrvrDPID( (u_long)wWorldSrvr );
					}
					else if( wWorldSrvr == 0 ) {
						ar.Read( lptmp, 8 );	// skip
						ar >> dwWorldID;
						ar >> vPos;
						u_long uIdofMulti	= GetIdofMulti( idFrom );
						if( uIdofMulti == NULL_ID )		break;
						dpid	= GetWorldSrvrDPID( uIdofMulti, dwWorldID, vPos );
					}
					else {
						dpid	= DPID_UNKNOWN;
						ASSERT( 0 );
					}

					if( DPID_UNKNOWN != dpid )
						Send( lpMsg, dwMsgSize, dpid );
					break;
				}
			case PACKETTYPE_BROADCAST:
				{
					Send( lpMsg, dwMsgSize, DPID_ALLPLAYERS );
					break;
				}
			default:
				TRACE( "Handler not found(%08x)\n", dw );
				break;
		}
	}
}

void CDPCoreSrvr::OnAddConnection( CAr & ar, DPID dpid, DPID, DPID, u_long )
{
	ULONG uWorldSrvr;
	ar >> uWorldSrvr;

	DWORD dwTime;
	ar >> dwTime;
	SendQueryTickCount( dwTime, dpid, g_GameTimer.GetCurrentTime() );

	CMclAutoLock Lock( m_AccessLock );

#ifdef __STL_0402
	CServerDescArray::iterator i	= m_apSleepServer.find( uWorldSrvr );
	if( i != m_apSleepServer.end() )
	{
		CServerDesc* pServerDesc	= i->second;
		m_apSleepServer.erase( i );
#else	// __STL_0402
	CServerDesc* pServerDesc;
	if( m_apSleepServer.Lookup( uWorldSrvr, pServerDesc ) )
	{
		m_apSleepServer.RemoveKey( uWorldSrvr );
#endif	// __STL_0402

		pServerDesc->SetKey( uWorldSrvr );
		GetPlayerAddr( dpid, pServerDesc->m_szAddr );

		
#ifdef __STL_0402
		bool bResult	= m_apServer.insert( CServerDescArray::value_type( dpid, pServerDesc ) ).second;
		ASSERT( bResult );
		bResult	= m_toHandle.insert( map<u_long, DPID>::value_type( uWorldSrvr, dpid ) ).second;
		ASSERT( bResult );
#else	// __STL_0402
		m_apServer.SetAt( dpid, pServerDesc );
		m_toHandle.SetAt( uWorldSrvr, dpid );
#endif	// __STL_0402
		SendRecharge( (u_long)10240, dpid );

		BEFORESENDDUAL( ar, PACKETTYPE_LOAD_WORLD, DPID_UNKNOWN, DPID_UNKNOWN );
		pServerDesc->Serialize( ar );
		g_PartyMng.Serialize( ar );
		g_GuildMng.m_AddRemoveLock.Enter( theLineFile );
		g_GuildMng.Serialize( ar, FALSE );
		g_GuildWarMng.Serialize( ar );
		g_GuildMng.m_AddRemoveLock.Leave( theLineFile );

#ifdef __ENVIRONMENT_EFFECT
		CEnvironment::GetInstance()->Serialize( ar );
#else // __ENVIRONMENT_EFFECT
		g_Environment.Serialize( ar );
#endif // __ENVIRONMENT_EFFECT

#ifdef __EVENT0913
		ar << s_bEvent0913;
#endif	// __EVENT0913
#ifdef __EVENT1206
		ar << s_bEvent1206;
#endif	// __EVENT1206

		ar << s_dwEvent;

		SEND( ar, this, dpid );
		g_DPCacheSrvr.Send( lpBuf + sizeof(DPID), nBufSize - sizeof(DPID), DPID_ALLPLAYERS );

		g_MyTrace.Add( uWorldSrvr, FALSE, "%04d", uWorldSrvr );

	#if defined(__INTERNALSERVER)
		SendCWWantedList( dpid );
	#endif

#ifdef __SERVERLIST0911
		g_dpDatabaseClient.SendServerEnable( uWorldSrvr, 1L );
#endif	// __SERVERLIST0911

#if __VER >= 14 // __INSTANCE_DUNGEON
		CInstanceDungeonHelper::GetInstance()->SendInstanceDungeonAllInfo( dpid );
#endif // __INSTANCE_DUNGEON
	}
	
	CHAR lpAddr[16];
	GetPlayerAddr( dpid, lpAddr );
}

void CDPCoreSrvr::OnRemoveConnection( DPID dpid )
{
	CMclAutoLock Lock( m_AccessLock );

#ifdef __STL_0402
	CServerDescArray::iterator i	= m_apServer.find( dpid );
	if( i != m_apServer.end() )
	{
		CServerDesc* pServerDesc	= i->second;
#else	// __STL_0402
	CServerDesc* pServerDesc;
	if( m_apServer.Lookup( dpid, pServerDesc ) )
	{
#endif	// __STL_0402

		ULONG uWorldSrvr	= pServerDesc->GetKey();

#ifdef __STL_0402
		m_apServer.erase( i );
		bool bResult	= m_apSleepServer.insert( CServerDescArray::value_type( uWorldSrvr, pServerDesc ) ).second;
		m_toHandle.erase( uWorldSrvr );
#else	// __STL_0402
		m_apServer.RemoveKey( dpid );
		m_apSleepServer.SetAt( uWorldSrvr, pServerDesc );
		m_toHandle.RemoveKey( uWorldSrvr );
#endif	// __STL_0402
//		SendUnify( (u_short)uWorldSrvr );
		g_MyTrace.Add( uWorldSrvr, TRUE, "%04d", uWorldSrvr );

#ifdef __SERVERLIST0911
		g_dpDatabaseClient.SendServerEnable( uWorldSrvr, 0L );
#endif	// __SERVERLIST0911

#if __VER >= 14 // __INSTANCE_DUNGEON
		CInstanceDungeonHelper::GetInstance()->DestroyAllDungeonByMultiKey( uWorldSrvr );
#endif // __INSTANCE_DUNGEON
	}
}

void CDPCoreSrvr::SendQueryTickCount( DWORD dwTime, DPID dpid, double dCurrentTime )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUERYTICKCOUNT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwTime << g_TickCount.GetTickCount() << dCurrentTime;
	SEND( ar, this, dpid );
}

void CDPCoreSrvr::SendRecharge( u_long uBlockSize, DPID dpid )
{
	BEFORESENDDUAL( ar, PACKETTYPE_RECHARGE_IDSTACK, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << m_objmap.m_idStack.GetIdBlock( uBlockSize );	// idBase
	ar << uBlockSize;
	SEND( ar, this, dpid );
}

void CDPCoreSrvr::OnRecharge( CAr & ar, DPID dpid, DPID, DPID, u_long )
{
	u_long uBlockSize;
	ar >> uBlockSize;
	SendRecharge( uBlockSize, dpid );
}

void CDPCoreSrvr::OnJoin( CAr & ar, DPID dpid, DPID, DPID, u_long )
{
	u_long idPlayer;
	char szPlayer[MAX_PLAYER]	= { 0,};
	ar >> idPlayer;
	ar.ReadString( szPlayer, MAX_PLAYER );
	BOOL bOperator;
	ar >> bOperator;

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CPlayer* pPlayer	= g_PlayerMng.GetPlayer( idPlayer );
	if( !pPlayer || lstrcmp( szPlayer, pPlayer->lpszPlayer ) != 0 )
	{
		SendDestroyPlayer( idPlayer, dpid );
		return;
	}
	if( bOperator )
		g_PlayerMng.AddItToSetofOperator( idPlayer );

#if __VER >= 12 // __PARSKILL1001 090917 mirchang - 파스킬 아이템 수정
	if( pPlayer )
	{
		CParty* pParty	= g_PartyMng.GetParty( pPlayer->m_uPartyId );
		if( pParty )
			g_dpCoreSrvr.SendSetPartyMode( pParty->m_uPartyId, PARTY_PARSKILL_MODE, FALSE );
	}
#endif // __PARSKILL1001 090917 mirchang - 파스킬 아이템 수정
}

void CDPCoreSrvr::SendDuplicate( u_long uWorldSrvr )
{
	BEFORESENDDUAL( ar, PACKETTYPE_DUPLICATE, DPID_UNKNOWN, (DPID)uWorldSrvr );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::OnWhisper( CAr & ar, DPID dpidFrom, DPID dpidCache, DPID dpidUser, u_long uBufSize )
{
	CHAR lpString[260], lpSnoopString[512];
	u_long idFrom, idTo;

	ar >> idFrom >> idTo;
	ar.ReadString( lpString, 260 );

	CPlayer* pFrom, *pTo;
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	pFrom	= g_PlayerMng.GetPlayer( idFrom );
	pTo	= g_PlayerMng.GetPlayer( idTo );

	if( pFrom && pTo )
	{
		g_DPCacheSrvr.SendWhisper( pFrom->lpszPlayer, pTo->lpszPlayer, idFrom, idTo, lpString, pTo );		// 상대방에게 귓말 보내기
		g_DPCacheSrvr.SendWhisper( pFrom->lpszPlayer, pTo->lpszPlayer, idFrom, idTo, lpString, pFrom );	// 나에게 보냄

		CPlayer* pSnoop;
		if( pFrom->m_idSnoop > 0 )
		{
			pSnoop	= g_PlayerMng.GetPlayer( pFrom->m_idSnoop );
			if( pSnoop )
			{
				sprintf( lpSnoopString, "[w][%s->%s] %s", pFrom->lpszPlayer, pTo->lpszPlayer, lpString );
				g_DPCacheSrvr.SendSnoop( lpSnoopString, pSnoop );
			}
		}
		if( pTo->m_idSnoop > 0 )
		{
			pSnoop	= g_PlayerMng.GetPlayer( pTo->m_idSnoop );
			if( pSnoop )
			{
				sprintf( lpSnoopString, "[w][%s->%s] %s", pFrom->lpszPlayer, pTo->lpszPlayer, lpString );
				g_DPCacheSrvr.SendSnoop( lpSnoopString, pSnoop );
			}
		}
	}
	else
	{
		// player not found
		g_DPCacheSrvr.SendSay( "", "", idFrom, idTo, "", pFrom, 1 ); // 플레이어가 접속중이지 않습니다. 
	}
}

// Operator commands
void CDPCoreSrvr::OnSay( CAr & ar, DPID dpidFrom, DPID dpidCache, DPID dpidUser, u_long uBufSize )
{
	CHAR	lpString[260];
	u_long idFrom, idTo;

	ar >> idFrom >> idTo;
	ar.ReadString( lpString, 260 );

	CPlayer* pFrom, *pTo;
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	pFrom	= g_PlayerMng.GetPlayer( idFrom );
	pTo	= g_PlayerMng.GetPlayer( idTo );

	if( pFrom && pTo )
	{
		g_DPCacheSrvr.SendSay( pFrom->lpszPlayer, pTo->lpszPlayer, idFrom, idTo, lpString, pTo );		// 상대방에게 귓말 보내기
		g_DPCacheSrvr.SendSay( pFrom->lpszPlayer, pTo->lpszPlayer, idFrom, idTo, lpString, pFrom );	// 나에게 보냄
	}
	else
	{
		g_DPCacheSrvr.SendSay( "", "", idFrom, idTo, "", pFrom, 1 ); // 플레이어가 접속중이지 않습니다. 
		// player not found
	}
}

void CDPCoreSrvr::OnModifyMode( CAr & ar, DPID dpidFrom, DPID dpidCache, DPID dpidUser, u_long )
{
	u_long idFrom, idTo;
	DWORD dwMode;
	BYTE f;
	ar >> dwMode >> f >> idFrom >> idTo;

	CPlayer* pFrom, *pTo;
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	pFrom	= g_PlayerMng.GetPlayer( idFrom );
	pTo	= g_PlayerMng.GetPlayer( idTo );

	if( pFrom && pTo )
	{
		g_DPCacheSrvr.SendModifyMode( dwMode, f, idFrom, pTo );
	}
	else
	{
		g_DPCacheSrvr.SendSay( "", "", idFrom, idTo, "", pFrom, 1 ); // 플레이어가 접속중이지 않습니다. 
	}
}

void CDPCoreSrvr::OnShout( CAr & ar, DPID, DPID, DPID, u_long uBufSize )
{
	CHAR	lpString[1024];
	u_long idPlayer;
	ar >> idPlayer;
	ar.ReadString( lpString, 1024 );
	SendShout( idPlayer, lpString, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::OnGMSay( CAr& ar, DPID, DPID, DPID, u_long uBufSize )
{
	CHAR	lpString[260];

	DWORD	dwWorldID;
	u_long idPlayer;
	
	ar >> idPlayer;
	ar >> dwWorldID;
	ar.ReadString( lpString, 260 );

	CPlayer* pFrom, *pTo	= NULL;
	CMclAutoLock Lock( g_PlayerMng.m_AddRemoveLock );
	pFrom	= g_PlayerMng.GetPlayer( idPlayer );

	for( set<u_long>::iterator i = g_PlayerMng.m_set.begin(); i != g_PlayerMng.m_set.end(); ++i )
	{
		pTo	= g_PlayerMng.GetPlayer( u_long(*i) );
		
		if( pFrom && pTo )
		{
			g_DPCacheSrvr.SendGMSay( pFrom->lpszPlayer, pTo->lpszPlayer, lpString, pTo );
		}
	}
	if( pFrom && pTo )
		g_DPCacheSrvr.SendGMSay( "건의", pTo->lpszPlayer, lpString, pFrom );	// 나에게 보냄
}

void CDPCoreSrvr::OnPlayMusic( CAr & ar, DPID, DPID, DPID, u_long )
{
	DWORD dwWorldID;
	u_long idmusic;
	u_long uIdofMulti;
	ar >> uIdofMulti;
	ar >> dwWorldID;
	ar >> idmusic;

	CMclAutoLock	Lock( m_AccessLock );

#ifdef __STL_0402
	for( CServerDescArray::iterator i = m_apServer.begin(); i != m_apServer.end(); ++i )
	{
		CServerDesc* pServer	= i->second;
		if( pServer->GetIdofMulti() == uIdofMulti && pServer->IsIntersected( dwWorldID ) )
		{
			SendPlayMusic( idmusic, dwWorldID, GetWorldSrvrDPID( pServer->GetKey() ) );
		}
	}
#else	// __STL_0402

	CServerDesc* pServer;
	CMyBucket<CServerDesc*>* pBucket	= m_apServer.GetFirstActive();
	while( pBucket )
	{
		pServer		= pBucket->m_value;
		ASSERT( pServer );
		if( pServer->GetIdofMulti() == uIdofMulti && pServer->IsIntersected( dwWorldID ) )
		{
			SendPlayMusic( idmusic, dwWorldID, GetWorldSrvrDPID( pServer->GetKey() ) );
		}
		pBucket		= pBucket->pNext;
	}
#endif	// __STL_0402
}

void CDPCoreSrvr::OnPlaySound( CAr & ar, DPID, DPID, DPID, u_long )
{
	DWORD dwWorldID;
	u_long idsound;
	u_long uIdofMulti;
	ar >> uIdofMulti;
	ar >> dwWorldID;
	ar >> idsound;

	CMclAutoLock	Lock( m_AccessLock );

#ifdef __STL_0402
	for( CServerDescArray::iterator i = m_apServer.begin(); i != m_apServer.end(); ++i )
	{
		CServerDesc* pServer	= i->second;
		if( pServer->GetIdofMulti() == uIdofMulti && pServer->IsIntersected( dwWorldID ) )
		{
			SendPlaySound( idsound, dwWorldID, GetWorldSrvrDPID( pServer->GetKey() ) );
		}
	}
#else	// __STL_0402
	CServerDesc* pServer;
	CMyBucket<CServerDesc*>* pBucket	= m_apServer.GetFirstActive();
	while( pBucket )
	{
		pServer		= pBucket->m_value;
		ASSERT( pServer );
		if( pServer->GetIdofMulti() == uIdofMulti && pServer->IsIntersected( dwWorldID ) )
		{
			SendPlaySound( idsound, dwWorldID, GetWorldSrvrDPID( pServer->GetKey() ) );
		}
		pBucket		= pBucket->pNext;
	}
#endif	// __STL_0402
}

void CDPCoreSrvr::OnSummonPlayer( CAr & ar, DPID, DPID, DPID, u_long )
{
	DWORD dwWorldID;
	u_long idOperator, idPlayer;
	D3DXVECTOR3 vPos;

	ar >> idOperator;
	u_long uIdofMulti;
	ar >> uIdofMulti;
	ar >> dwWorldID;
	ar >> vPos >> idPlayer;
#ifdef __LAYER_1015
	int nLayer;
	ar >> nLayer;
#endif	// __LAYER_1015

	CPlayer* pOperator, *pPlayer;
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	pOperator	= g_PlayerMng.GetPlayer( idOperator );
	pPlayer		= g_PlayerMng.GetPlayer( idPlayer );

	if( pOperator && pPlayer )
	{
#ifdef __LAYER_1015
		g_DPCacheSrvr.SendSummonPlayer( idOperator, uIdofMulti, dwWorldID, vPos, pPlayer, nLayer );
#else	// __LAYER_1015
		g_DPCacheSrvr.SendSummonPlayer( idOperator, uIdofMulti, dwWorldID, vPos, pPlayer );
#endif	// __LAYER_1015
	}
	else
	{
		g_DPCacheSrvr.SendSay( "", "", idOperator, idPlayer, "", pOperator, 1 ); // 플레이어가 접속중이지 않습니다. 
		// player not found
	}
}

void CDPCoreSrvr::OnTeleportPlayer( CAr & ar, DPID, DPID, DPID, u_long )
{
	u_long idOperator, idPlayer;
	ar >> idOperator >> idPlayer;

	CPlayer* pOperator, *pPlayer;
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	pOperator	= g_PlayerMng.GetPlayer( idOperator );
	pPlayer		= g_PlayerMng.GetPlayer( idPlayer );
	if( pOperator && pPlayer )
	{
		g_DPCacheSrvr.SendTeleportPlayer( idOperator, pPlayer );
	}
	else
	{
		g_DPCacheSrvr.SendSay( "", "", idOperator, idPlayer, "", pOperator, 1 ); // 플레이어가 접속중이지 않습니다. 
	}
}

void CDPCoreSrvr::OnKillPlayer( CAr & ar, DPID, DPID, DPID, u_long )
{
	u_long idOperator, idPlayer;
	ar >> idOperator >> idPlayer;

	CPlayer* pOperator, *pPlayer;
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	pOperator	= g_PlayerMng.GetPlayer( idOperator );
	pPlayer		= g_PlayerMng.GetPlayer( idPlayer );
	if( pOperator && pPlayer )
	{
		g_DPCacheSrvr.SendKillPlayer( pPlayer );
	}
	else
	{
		g_DPCacheSrvr.SendSay( "", "", idOperator, idPlayer, "", pOperator, 1 ); // 플레이어가 접속중이지 않습니다. 
	}
}

void CDPCoreSrvr::OnGetPlayerAddr( CAr & ar, DPID dpidFrom, DPID dpidCache, DPID dpidUser, u_long )
{
	u_long idOperator, idPlayer;
	ar >> idOperator >> idPlayer;

	CPlayer* pOperator, *pPlayer;
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	pOperator	= g_PlayerMng.GetPlayer( idOperator );
	pPlayer		= g_PlayerMng.GetPlayer( idPlayer );
	if( pOperator && pPlayer )
	{
		g_DPCacheSrvr.SendGetPlayerAddr( pPlayer->lpszPlayer, pPlayer->lpAddr, pOperator );
	}
	else
	{
		g_DPCacheSrvr.SendSay( "", "", idOperator, idPlayer, "", pOperator, 1 ); // 플레이어가 접속중이지 않습니다. 
	}
}

void CDPCoreSrvr::OnGetPlayerCount( CAr & ar, DPID, DPID, DPID, u_long )
{
	u_long idOperator;
	ar >> idOperator;

	CPlayer* pOperator;
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	pOperator	= g_PlayerMng.GetPlayer( idOperator );
	if( pOperator )
	{
		u_short uCount	= (u_short)g_PlayerMng.GetCount();
		g_DPCacheSrvr.SendGetPlayerCount( uCount, pOperator );
	}
	else
	{
		// operator not found
	}
}

void CDPCoreSrvr::OnSystem( CAr & ar, DPID, DPID, DPID, u_long uBufSize )
{
	CHAR	lpString[512];
	ar.ReadString( lpString, 512 );
	g_DPCacheSrvr.SendSystem( lpString );
}

void CDPCoreSrvr::OnCaption( CAr & ar, DPID, DPID, DPID, u_long uBufSize )
{
	CHAR	lpString[256];
	DWORD dwWorldId;
	BOOL	bSmall;
	
	ar >> bSmall;
	ar >> dwWorldId;
	ar.ReadString( lpString, 256 );
	
	g_DPCacheSrvr.SendCaption( lpString, dwWorldId, bSmall );
}


void CDPCoreSrvr::OnGetCorePlayer( CAr & ar, DPID, DPID, DPID, u_long )
{
	u_long idOperator;
	ar >> idOperator;

	CPlayer* pOperator;
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	pOperator	= g_PlayerMng.GetPlayer( idOperator );
	if( pOperator )
	{
		g_DPCacheSrvr.SendGetCorePlayer( pOperator );
	}
	else
	{
		// operator not found
	}
}

void CDPCoreSrvr::OnAddPartyExp( CAr & ar, DPID, DPID, DPID, u_long )
{
	u_long uPartyId;
	int nMonLv;
	BOOL bSuperLeader = FALSE;
#if __VER >= 12 // __JHMA_VER12_1	//12차 극단유료아이템
	BOOL bLeaderSMPartyExpUp = FALSE;
#endif // //12차 극단유료아이템

	ar >> uPartyId;
	ar >> nMonLv;
	ar >> bSuperLeader;
#if __VER >= 12 // __JHMA_VER12_1	//12차 극단유료아이템
	ar >> bLeaderSMPartyExpUp;
#endif // //12차 극단유료아이템
	
	CMclAutoLock	Lock( g_PartyMng.m_AddRemoveLock );
	CParty* pParty	= g_PartyMng.GetParty( uPartyId );
	if( pParty )
	{
		if( pParty->m_nKindTroup == 0 )
		{
			if( pParty->m_nLevel < MAX_PARTYLEVEL )
			{
				int nAddExp	= (int)( (nMonLv / 25 + 1) * 10 );
				nAddExp		= (int)( nAddExp * s_fPartyExpRate );
				if( bSuperLeader )
					nAddExp		*= 2;
#if __VER >= 12 // __JHMA_VER12_1	//12차 극단유료아이템
				if( bLeaderSMPartyExpUp )
					nAddExp	= (int)( nAddExp * 1.5 );
#endif // //12차 극단유료아이템

				pParty->m_nExp += nAddExp;

				if( pParty->m_nExp >= (int)( g_PartyMng.m_aExpParty[pParty->m_nLevel].Exp ) )
				{
					pParty->m_nExp	-= g_PartyMng.m_aExpParty[pParty->m_nLevel].Exp;
					pParty->m_nPoint	+= g_PartyMng.m_aExpParty[pParty->m_nLevel].Point;
					pParty->m_nLevel++;
				}
			}
		}
		else
		{
			int nAddExp	= (int)( (nMonLv / 25 + 1 ) * 10 );
			nAddExp		= (int)( nAddExp * s_fPartyExpRate );

			if( bSuperLeader )
				nAddExp		*= 2;
#if __VER >= 12 // __JHMA_VER12_1	//12차 극단유료아이템
			if( bLeaderSMPartyExpUp )
				nAddExp		= (int)( nAddExp * 1.5 );
#endif // //12차 극단유료아이템

			pParty->m_nExp += nAddExp;
			
			if( pParty->m_nExp >= int( ( ( 50 + pParty->GetLevel() ) * pParty->GetLevel() / 13 ) * 10 ) )
			{
				pParty->m_nExp	-= int( ( ( 50 + pParty->GetLevel() ) * pParty->GetLevel() / 13 ) * 10 );
				pParty->m_nLevel++;
#ifdef __MA_VER11_01	// 극단레벨업포인트변경
				pParty->m_nPoint	+= 15;
#else	//__MA_VER11_01	// 극단레벨업포인트변경
				if( 11 <= pParty->m_nLevel && pParty->m_nLevel <= 40 )
				{
					pParty->m_nPoint	+= 15;//2;//g_PartyMng.m_aExpParty[nIndex].Point;
				}
				else
				if( 40 < pParty->m_nLevel )
				{
					pParty->m_nPoint	+= 10;//g_PartyMng.m_aExpParty[nIndex].Point;
				}
#endif	//__MA_VER11_01	// 극단레벨업포인트변경

			}
		}

		BEFORESENDDUAL( ar, PACKETTYPE_SETPARTYEXP, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << uPartyId << pParty->m_nExp << pParty->m_nPoint << pParty->m_nLevel;
		SEND( ar, this, DPID_ALLPLAYERS );
	}
}

void CDPCoreSrvr::OnRemovePartyPoint( CAr & ar, DPID, DPID, DPID, u_long )
{
	u_long uPartyId;
	int nRemovePoint;
	ar >> uPartyId;
	ar >> nRemovePoint;
	
	CMclAutoLock	Lock( g_PartyMng.m_AddRemoveLock );
	CParty* pParty = g_PartyMng.GetParty( uPartyId );
	if( pParty )
	{
		pParty->m_nPoint -= nRemovePoint;
		BEFORESENDDUAL( ar, PACKETTYPE_REMOVEPARTYPOINT, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << uPartyId << pParty->m_nPoint;
		SEND( ar, this, DPID_ALLPLAYERS );
	}
}

#if __VER < 11 // __SYS_PLAYER_DATA
void CDPCoreSrvr::OnMemberLevel( CAr & ar, DPID, DPID, DPID, u_long )
{
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CMclAutoLock	Lock2( g_PartyMng.m_AddRemoveLock );

	u_long idParty, idPlayer;
	int nLevel;
	ar >> idParty >> idPlayer >> nLevel;
	CParty* pParty = g_PartyMng.GetParty( idParty );
	if( pParty )
	{
		int Member = pParty->FindMember( idPlayer );
		if( Member > -1 )
		{
			pParty->m_aMember[Member].m_nLevel = nLevel;
			BEFORESENDDUAL( ar, PACKETTYPE_MEMBERLEVEL, DPID_UNKNOWN, DPID_UNKNOWN );
			ar << idParty << idPlayer << nLevel;
			SEND( ar, this, DPID_ALLPLAYERS );
		}
	}
}

void CDPCoreSrvr::OnMemberJob( CAr & ar, DPID, DPID, DPID, u_long )
{
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CMclAutoLock	Lock2( g_PartyMng.m_AddRemoveLock );

	u_long idParty, idPlayer;
	int nJob;
	ar >> idParty >> idPlayer >> nJob;

	CParty* pParty = g_PartyMng.GetParty( idParty );
	if( pParty )
	{
		int Member = pParty->FindMember( idPlayer );
		if( Member > -1 )
		{
			pParty->m_aMember[Member].m_nJob = nJob;
			BEFORESENDDUAL( ar, PACKETTYPE_MEMBERJOB, DPID_UNKNOWN, DPID_UNKNOWN );
			ar << idParty << idPlayer << nJob;
			SEND( ar, this, DPID_ALLPLAYERS );
		}
	}
}
#endif	// __SYS_PLAYER_DATA

void CDPCoreSrvr::OnPartyLevel( CAr & ar, DPID, DPID, DPID, u_long )
{
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CMclAutoLock	Lock2( g_PartyMng.m_AddRemoveLock );

	u_long idParty, idPlayer;
	DWORD dwLevel, dwPoint, dwExp;
	ar >> idParty >> idPlayer >> dwLevel >> dwPoint >> dwExp;

	CParty* pParty = g_PartyMng.GetParty( idParty );
	if( pParty )
	{
		pParty->m_nLevel = dwLevel;
		pParty->m_nPoint = dwPoint;				// 극단 레벨, 경험치, 포인트
		pParty->m_nExp = dwExp;

		if( pParty->m_nLevel < MAX_PARTYLEVEL )
		{
			if( (int)( g_PartyMng.m_aExpParty[pParty->m_nLevel].Exp ) <= pParty->m_nExp )
				pParty->m_nExp   = g_PartyMng.m_aExpParty[pParty->m_nLevel].Exp - 1;
		}
		else
		{
			if( int( ( ( 50 + pParty->GetLevel() ) * pParty->GetLevel() / 13 ) * 10 ) <= pParty->m_nExp )
				pParty->m_nExp = int( ( ( 50 + pParty->GetLevel() ) * pParty->GetLevel() / 13 ) * 10 ) - 1;
		}
		BEFORESENDDUAL( ar, PACKETTYPE_SETPARTYEXP, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << idParty << pParty->m_nExp << pParty->m_nPoint << pParty->m_nLevel;
		SEND( ar, this, DPID_ALLPLAYERS );
	}
}

void CDPCoreSrvr::OnGameRate( CAr & ar, DPID, DPID, DPID, u_long )
{
	FLOAT fRate;
	BYTE nFlag;
	ar >> fRate;
	ar >> nFlag;

	{
		BEFORESENDDUAL( ar, PACKETTYPE_GAMERATE, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << fRate;
		ar << nFlag;
		SEND( ar, this, DPID_ALLPLAYERS );
	}

	g_DPCacheSrvr.SendGameRate( fRate, nFlag );	
}

void CDPCoreSrvr::OnLoadConstant( CAr & ar, DPID, DPID, DPID, u_long )
{
	BEFORESENDDUAL( ar1, PACKETTYPE_LOADCONSTANT, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar1, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::OnSetMonsterRespawn( CAr & ar, DPID, DPID, DPID, u_long )
{
	u_long uidPlayer;
	DWORD dwMonsterID;
	DWORD dwRespawnNum;
	DWORD dwAttackNum;
	DWORD dwRect;
	DWORD dwRespawnTime;
	BOOL  bFlying;
	ar >> uidPlayer;
	ar >> dwMonsterID >> dwRespawnNum >> dwAttackNum >> dwRect >> dwRespawnTime;
	ar >> bFlying;
	
	{
		BEFORESENDDUAL( ar, PACKETTYPE_SETMONSTERRESPAWN, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << uidPlayer;
		ar << dwMonsterID << dwRespawnNum << dwAttackNum << dwRect << dwRespawnTime;
		ar << bFlying;
		SEND( ar, this, DPID_ALLPLAYERS );
	}
}

#if __VER < 11 // __SYS_PLAYER_DATA
void CDPCoreSrvr::OnFriendChangeJob( CAr & ar, DPID, DPID, DPID, u_long )
{
	vector< u_long > vecIdFriend;
	u_long uidPlayer;
	int nJob;
	ar >> uidPlayer >> nJob;

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CPlayer* pPlayer = g_PlayerMng.GetPlayer( uidPlayer );
	if( pPlayer )
	{
		pPlayer->Lock();
		for( C2FriendPtr::iterator i = pPlayer->m_Messenger.m_adifferntFriend.begin() ; i != pPlayer->m_Messenger.m_adifferntFriend.end() ; i++ )
		{
			LPFRIEND pFriend = (LPFRIEND)i->second;
			if( !pFriend )
				continue;

			vecIdFriend.push_back( pFriend->dwUserId );			
		}
		pPlayer->Unlock();

		for( int j = 0 ; j < vecIdFriend.size() ; j++ )
		{
			CPlayer* pPlayFriend = g_PlayerMng.GetPlayer( vecIdFriend[j] );
			if( pPlayFriend )
			{
				pPlayFriend->Lock();
				LPFRIEND pFriendbuf = pPlayFriend->m_Messenger.GetFriend( uidPlayer );
				if( pFriendbuf )
				{
					pFriendbuf->nJob = nJob;
				}
				pPlayFriend->Unlock();
			}
		}

		BEFORESENDDUAL( ar, PACKETTYPE_CHANGEJOB, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << uidPlayer << nJob;
		SEND( ar, this, DPID_ALLPLAYERS );
	}
}
#endif	// __SYS_PLAYER_DATA

void CDPCoreSrvr::OnPartySkillUse( CAr & ar, DPID, DPID, DPID, u_long uBufSize )
{
	u_long uidPlayer;
	int nMode;
	DWORD dwSkillTime;
	int nRemovePoint;
#if __VER >= 12 // __JHMA_VER12_1	//12차 극단유료아이템
	int nCachMode;
#endif // //12차 극단유료아이템

	ar >> uidPlayer;
	ar >> nMode;
	ar >> dwSkillTime;
	ar >> nRemovePoint;
#if __VER >= 12 // __JHMA_VER12_1	//12차 극단유료아이템
	ar >> nCachMode;
#endif // //12차 극단유료아이템

	CPlayer* pSender;
	
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	pSender = g_PlayerMng.GetPlayer( uidPlayer );
	
	if( !pSender )
	{
		// 사용자 찾기 실패
		return;		
	}
	
	if( pSender->m_uPartyId != 0 )
	{
		CParty* pParty;
		
		CMclAutoLock	Lock1( g_PartyMng.m_AddRemoveLock );
		pParty	= g_PartyMng.GetParty( pSender->m_uPartyId );
		
		if( pParty )
		{
			if( pParty->m_nPoint < nRemovePoint)
				return;
			if( nCachMode == 0 && pParty->m_nModeTime[nMode] != 0 )
				return;

			pParty->m_nPoint -= nRemovePoint;
#if __VER >= 12 // __JHMA_VER12_1	//12차 극단유료아이템
	#if __VER >= 12 // __PARSKILL1001	//12차 파스킬 아이템 수정  world,core,neuz
			if( nMode == PARTY_PARSKILL_MODE && dwSkillTime == 0 )
			{
			    pParty->SetPartyMode( nMode, dwSkillTime , 0 );	
				SendSetPartyMode( pParty->m_uPartyId, nMode, FALSE );
				return;
			}
	#endif //__PARSKILL1001	//12차 파스킬 아이템 수정  world,core,neuz
            pParty->SetPartyMode( nMode, dwSkillTime , nCachMode );		
			SendSetPartyMode( pParty->m_uPartyId, nMode, TRUE, pParty->m_nPoint, (DWORD)pParty->GetPartyModeTime(nMode) );
#else // //12차 극단유료아이템
			pParty->SetPartyMode( nMode, dwSkillTime );
			SendSetPartyMode( pParty->m_uPartyId, nMode, TRUE, pParty->m_nPoint );
#endif // //12차 극단유료아이템
		}
		else
		{
			// 극단이 없거나 극단에 소속되어 있지 않다.
		}
	}
	else
	{
		//극단에 소속되어 있지 않다
	}
}

void CDPCoreSrvr::OnPartyChat( CAr & ar, DPID, DPID, DPID, u_long uBufSize )
{
	CHAR	lpString[260];

	u_long idPlayer;
		
	OBJID objid;
	ar >> objid;
	ar >> idPlayer;
	ar.ReadString( lpString, 260 );

	CPlayer* pSender;

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	pSender = g_PlayerMng.GetPlayer( idPlayer );

	if( !pSender )
	{
		// 사용자 찾기 실패
		return;		
	}

	if( pSender->m_uPartyId != 0 )
	{
		CParty* pParty;
		
		CMclAutoLock	Lock1( g_PartyMng.m_AddRemoveLock );
		pParty	= g_PartyMng.GetParty( pSender->m_uPartyId );
		
		if( pParty && pParty->IsMember( idPlayer ) )
		{
			SendPartyChat( pParty->m_uPartyId, pSender->lpszPlayer, lpString, objid );
		}
		else
		{
			// 극단이 없거나 극단에 소속되어 있지 않다.
		}
	}
	else
	{
		//극단에 소속되어 있지 않다
	}
}

#ifdef __ENVIRONMENT_EFFECT

void CDPCoreSrvr::OnFallRain( CAr & ar, DPID, DPID, DPID, u_long )
{
	CEnvironment::GetInstance()->m_Authority = TRUE;
	CEnvironment::GetInstance()->SetEnvironmentEffect( TRUE );
}

void CDPCoreSrvr::OnStopRain( CAr & ar, DPID, DPID, DPID, u_long )
{
	CEnvironment::GetInstance()->m_Authority = FALSE;
	CEnvironment::GetInstance()->SetEnvironmentEffect( FALSE );
	SendEnvironmentEffect();
}

#else // __ENVIRONMENT_EFFECT

void CDPCoreSrvr::OnFallSnow( CAr & ar, DPID, DPID, DPID, u_long )
{
	g_Environment.m_Authority = TRUE;
	g_Environment.m_bSnow = TRUE;
	g_Environment.m_bRain = FALSE;
}
void CDPCoreSrvr::OnFallRain( CAr & ar, DPID, DPID, DPID, u_long )
{
	g_Environment.m_Authority = TRUE;
	g_Environment.m_bRain = TRUE;
	g_Environment.m_bSnow = FALSE;
}
void CDPCoreSrvr::OnStopSnow( CAr & ar, DPID, DPID, DPID, u_long )
{
	g_Environment.m_Authority = FALSE;
	g_Environment.m_bSnow = FALSE;
	g_Environment.m_bRain = FALSE;
	SendEnvironment( g_Environment.m_bRain, g_Environment.m_bSnow );
}
void CDPCoreSrvr::OnStopRain( CAr & ar, DPID, DPID, DPID, u_long )
{
	g_Environment.m_Authority = FALSE;
	g_Environment.m_bRain = FALSE;
	g_Environment.m_bSnow = FALSE;
	SendEnvironment( g_Environment.m_bRain, g_Environment.m_bSnow );
}

#endif // __ENVIRONMENT_EFFECT

void CDPCoreSrvr::SendShout( u_long idPlayer, const CHAR* lpString, DPID dpid )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SHOUT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer;
	ar.WriteString( lpString );
	SEND( ar, this, dpid );
}

void CDPCoreSrvr::SendPartyChat( u_long idParty, const CHAR* lpName, const CHAR* lpString, OBJID objid )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PARTYCHAT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << objid;
	ar << idParty;
	ar.WriteString( lpName );
	ar.WriteString( lpString );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendPlayMusic( u_long idmusic, DWORD dwWorldID, DPID dpid )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PLAYMUSIC, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idmusic;
	ar << dwWorldID;
	SEND( ar, this, dpid );
}

void CDPCoreSrvr::SendPlaySound( u_long idsound, DWORD dwWorldID, DPID dpid )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PLAYSOUND, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idsound;
	ar << dwWorldID;
	SEND( ar, this, dpid );
}
#if __VER >= 12 // __JHMA_VER12_1	//12차 극단유료아이템
void CDPCoreSrvr::SendSetPartyMode( u_long idParty, int nMode, BOOL bOnOff, LONG nPoint ,DWORD dwSkillTime )
#else // //12차 극단유료아이템
void CDPCoreSrvr::SendSetPartyMode( u_long idParty, int nMode, BOOL bOnOff, LONG nPoint )
#endif // //12차 극단유료아이템
{
	BEFORESENDDUAL( ar, PACKETTYPE_SETPARTYMODE, DPID_UNKNOWN, DPID_UNKNOWN );
#if __VER >= 12 // __JHMA_VER12_1	//12차 극단유료아이템
	ar << idParty << nMode << dwSkillTime << bOnOff;
#else // //12차 극단유료아이템
	ar << idParty << nMode << bOnOff;
#endif // //12차 극단유료아이템
	if( bOnOff == TRUE )
	{
		ar << nPoint;
	}
	SEND( ar, this, DPID_ALLPLAYERS );
}
void CDPCoreSrvr::SendRemoveParty( u_long idParty, u_long idLeader, u_long idMember )
{
	BEFORESENDDUAL( ar, PACKETTYPE_REMOVEPARTYMEMBER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idParty << idLeader << idMember;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendPartyChangeTroup( u_long idParty, const char * szPartyName )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CHANGETROUP, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idParty;
	ar.WriteString( szPartyName );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendPartyChangeName( u_long idParty, const char * szPartyName )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CHANPARTYNAME, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idParty;
	ar.WriteString( szPartyName );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendPartyChangeExpMode( u_long idParty, int nExpMode )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PARTYCHANGEEXPMODE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idParty << nExpMode;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendPartyChangeItemMode( u_long idParty, int nItemMode )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PARTYCHANGEITEMMODE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idParty << nItemMode;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendAddPartyMember( u_long uPartyId, u_long idLeader, long nLeaderLevel, long nLeaderJob, BYTE nLeaderSex, u_long idMember, long nMemberLevel, long nMemberJob, BYTE nMemberSex )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ADDPARTYMEMBER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uPartyId;
	ar << idLeader << nLeaderLevel << nLeaderJob << nLeaderSex;
	ar << idMember << nMemberLevel << nMemberJob << nMemberSex;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendAddFriend( u_long uidSender, u_long uidFriend, BYTE nSenderSex, BYTE nFriendSex, LONG nSendJob, LONG nFriendJob )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ADDFRIEND, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uidSender << uidFriend;
	ar << nSenderSex << nFriendSex;
	ar << nSendJob << nFriendJob;
	SEND( ar, this, DPID_ALLPLAYERS );
}


void CDPCoreSrvr::SendRemoveFriend( u_long uidSender, u_long uidFriend )
{
	BEFORESENDDUAL( ar, PACKETTYPE_REMOVEFRIEND, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uidSender << uidFriend;
	SEND( ar, this, DPID_ALLPLAYERS );
}

#ifdef __ENVIRONMENT_EFFECT

void CDPCoreSrvr::SendEnvironmentEffect()
{
	BEFORESENDDUAL( ar, PACKETTYPE_ENVIRONMENTALL, DPID_UNKNOWN, DPID_UNKNOWN );
	CEnvironment::GetInstance()->Serialize( ar );
	SEND( ar, this, DPID_ALLPLAYERS );
}

#else // __ENVIRONMENT_EFFECT

void CDPCoreSrvr::SendEnvironmentSnow( BOOL bSnow )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ENVIRONMENTSNOW, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << bSnow;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendEnvironmentRain( BOOL bRain )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ENVIRONMENTRAIN, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << bRain;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendEnvironment( BOOL bRain, BOOL bSnow )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ENVIRONMENTALL, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << bRain << bSnow;
	SEND( ar, this, DPID_ALLPLAYERS );
}

#endif // __ENVIRONMENT_EFFECT

void CDPCoreSrvr::OnGuildMsgControl( CAr & ar, DPID, DPID, DPID, u_long )
{//*
	GUILD_MSG_HEADER	Header;

	ar.Read(&Header, sizeof(GUILD_MSG_HEADER));

	if ( Header.HeadBMain == GUILD_MSG_HEADER::GUILD_BANK )
	{
		DWORD		dwPenya = 0;
		BYTE		cbCloak;
//		CItemElem	ItemElem;

		if ( Header.HeadBSub & GUILD_MSG_HEADER::PENYA )
		{
			ar >> dwPenya;
			ar >> cbCloak;
			g_DPCacheSrvr.SendGuildMsgControl( &Header, dwPenya, cbCloak );
		}
	}//*/
}

void CDPCoreSrvr::OnCreateGuild( CAr & ar, DPID, DPID, DPID, u_long )
{
	int nSize;
	GUILD_MEMBER_INFO	info[MAX_PTMEMBER_SIZE];
	char szGuild[MAX_G_NAME];

	ar >> nSize;
	if( nSize == 0 )
		return;
	ar.Read( info, sizeof(GUILD_MEMBER_INFO)*nSize );
	ar.ReadString( szGuild, MAX_G_NAME );

	CPlayer* pPlayer;
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	pPlayer		= g_PlayerMng.GetPlayer( info[0].idPlayer );	// master
	if( !pPlayer )	// Player X
		return;

	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );

	CGuild* pGuild	= g_GuildMng.GetGuild( pPlayer->m_idGuild );
	if( pGuild && pGuild->IsMember( pPlayer->uKey ) )
	{
		// guild exists
		g_DPCacheSrvr.SendDefinedText( TID_GAME_COMCREATECOM, pPlayer->dpidCache, pPlayer->dpidUser, "" );
		return;
	}
	pPlayer->m_idGuild	= 0;

	pGuild	= g_GuildMng.GetGuild( szGuild );
	if( pGuild )
	{
		// guild name exists
		g_DPCacheSrvr.SendDefinedText( TID_GAME_COMOVERLAPNAME, pPlayer->dpidCache, pPlayer->dpidUser, "" );
		return;
	}

	pGuild	= new CGuild;
	lstrcpy( pGuild->m_szGuild, szGuild );
	pGuild->m_idMaster		= pPlayer->uKey;
	pGuild->m_adwPower[ GUD_MASTER ] = 0x000000FF;
	u_long idGuild	= g_GuildMng.AddGuild( pGuild );
	if( idGuild > 0 )
	{
		CGuildMember* pMember	= new CGuildMember;
		pMember->m_idPlayer	= pPlayer->uKey;
		pMember->m_nMemberLv	= GUD_MASTER;
#if __VER < 11 // __SYS_PLAYER_DATA
		pMember->m_nLevel	= info[0].nLevel;
		pMember->m_nJob	= info[0].nJob;
		pMember->m_dwSex	= info[0].dwSex;
		pMember->m_nMultiNo		= info[0].nMultiNo	= pPlayer->m_uIdofMulti;
		pMember->m_nLogin	= 1;
#endif	// __SYS_PLAYER_DATA
		if( pGuild->AddMember( pMember ) )	// master
		{
			pPlayer->m_idGuild	= idGuild;
//			apPlayer[cb++]	= pPlayer;
			// member
			for( int i = 1; i < nSize; i++ )
			{
				pMember		= new CGuildMember;
				pMember->m_idPlayer		= info[i].idPlayer;
				pMember->m_nMemberLv	= GUD_ROOKIE;
#if __VER < 11 // __SYS_PLAYER_DATA
				pMember->m_nLevel	= info[i].nLevel;
				pMember->m_nJob	= info[i].nJob;
				pMember->m_dwSex	= info[i].dwSex;
#endif	// __SYS_PLAYER_DATA
				if( pGuild->AddMember( pMember ) )
				{
					CPlayer* pPlayertmp		= g_PlayerMng.GetPlayer( pMember->m_idPlayer );
					if( pPlayertmp )
					{
						pPlayertmp->m_idGuild	= idGuild;
#if __VER < 11 // __SYS_PLAYER_DATA
						pMember->m_nMultiNo		= info[i].nMultiNo	= pPlayertmp->m_uIdofMulti;
						pMember->m_nLogin	= 1;
#endif	// __SYS_PLAYER_DATA
//						apPlayer[cb++]	= pPlayertmp;
					}
				}
				else
				{
					info[i].idPlayer	= 0;
					SAFE_DELETE( pMember );
				}
			}
			SendCreateGuild( info, nSize, idGuild, szGuild );
			g_dpDatabaseClient.SendCreateGuild( info, nSize, idGuild, szGuild );
		}
		else
		{
			SAFE_DELETE( pMember );
			g_GuildMng.RemoveGuild( idGuild );
		}
	}
	else
	{
		SAFE_DELETE( pGuild );
	}
//	for( int i = 0; i < cb; i++ )
//		g_DPCacheSrvr.SendGuild( pGuild, apPlayer[i] );
}

void CDPCoreSrvr::OnGuildChat( CAr & ar, DPID, DPID, DPID, u_long )
{
	char	lpSnoopString[2048];

	u_long idPlayer;
	OBJID objid;
	ar >> objid;
	ar >> idPlayer;
	char sChat[1024]	= { 0, };

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );

	CPlayer* pPlayer	= g_PlayerMng.GetPlayer( idPlayer );
	if( pPlayer )
	{
		CGuild* pGuild	= g_GuildMng.GetGuild( pPlayer->m_idGuild );
		if( pGuild && pGuild->IsMember( pPlayer->uKey ) )
		{
			ar.ReadString( sChat, 1024 );

			CGuildMember* pMember;
			CPlayer* pPlayertmp;
			for( map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin();
				i != pGuild->m_mapPMember.end(); ++i )
			{
				pMember		= i->second;
				pPlayertmp	= g_PlayerMng.GetPlayer( pMember->m_idPlayer );
				if( pPlayertmp )
				{
					g_DPCacheSrvr.SendGuildChat( pPlayer->lpszPlayer, sChat, pPlayertmp, objid );

					if( pPlayertmp->m_idSnoop > 0 )
					{
						CPlayer* pSnoop		= g_PlayerMng.GetPlayer( pPlayertmp->m_idSnoop );
						if( pSnoop )
						{
							sprintf( lpSnoopString, "[g][%s-%s] %s", pPlayer->lpszPlayer, pPlayertmp->lpszPlayer, sChat );
							g_DPCacheSrvr.SendSnoop( lpSnoopString, pSnoop );
						}
					}
				}
			}
			if( pGuild->m_bLog )
				g_dpDatabaseClient.SendSnoopGuild( pGuild->m_idGuild, idPlayer, sChat );
		}
	}
}

void CDPCoreSrvr::SendGuildAuthority( u_long uGuildId, DWORD dwAuthority[] )
{
	//SendGuildAuthority
	BEFORESENDDUAL( ar, PACKETTYPE_GUILD_AUTHORITY, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uGuildId;
	
	ar.Write( dwAuthority, sizeof(DWORD) * MAX_GM_LEVEL );

	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendGuildPenya( u_long uGuildId, DWORD dwType, DWORD dwPenya )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILD_PENYA, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uGuildId;
	ar << dwType << dwPenya;	
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendChgMaster( u_long idPlayer, u_long idPlayer2, u_long idGuild )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CHG_MASTER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << idPlayer2 << idGuild;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendGuildSetName( u_long idGuild, const char* lpszGuild, u_long idPlayer, BYTE nId, BYTE nError )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILD_SETNAME, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idGuild;
	ar.WriteString( lpszGuild );
	ar << idPlayer << nId << nError;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendCreateGuild( GUILD_MEMBER_INFO* info, int nSize, u_long idGuild, const char* szGuild )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CREATE_GUILD, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nSize;
	ar.Write( info, sizeof(GUILD_MEMBER_INFO)*nSize );
	ar << idGuild;
	ar.WriteString( szGuild );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendDestroyGuild( u_long idGuild )
{
	BEFORESENDDUAL( ar, PACKETTYPE_DESTROY_GUILD, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idGuild;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendAddGuildMember( const GUILD_MEMBER_INFO & info, u_long idGuild )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ADD_GUILD_MEMBER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar.Write( &info, sizeof(GUILD_MEMBER_INFO) );
	ar << idGuild;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendRemoveGuildMember( u_long idPlayer, u_long idGuild )
{
	BEFORESENDDUAL( ar, PACKETTYPE_REMOVE_GUILD_MEMBER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << idGuild;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendGuildMemberLv( u_long idPlayer, int nMemberLv, u_long idGuild )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILD_MEMBER_LEVEL, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << nMemberLv << idGuild;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendGuildClass( u_long idPlayer, int nClass, u_long idGuild )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILD_CLASS, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << nClass << idGuild;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendGuildNickName( u_long idPlayer, LPCTSTR strNickName, u_long idGuild )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILD_NICKNAME, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << idGuild;
	ar.WriteString( strNickName );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendGuildMemberLogOut( u_long idGuild, u_long idPlayer )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILD_MEMBERLOGOUT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idGuild << idPlayer;
	SEND( ar, this, DPID_ALLPLAYERS );	
}

void CDPCoreSrvr::SendAcptWar( u_long idWar, u_long idDecl, u_long idAcpt )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ACPT_GUILD_WAR, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idWar << idDecl << idAcpt;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendWarDead( u_long idWar, BOOL bDecl )
{
	BEFORESENDDUAL( ar, PACKETTYPE_WAR_DEAD, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idWar << bDecl;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendWarEnd( u_long idWar, int nWptDecl, int nWptAcpt, int nType )
{
	BEFORESENDDUAL( ar, PACKETTYPE_WAR_END, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idWar << nWptDecl << nWptAcpt << nType;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendSurrender( u_long idWar, u_long idPlayer, BOOL bDecl )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SURRENDER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idWar << idPlayer << bDecl;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::OnWarDead( CAr & ar, DPID, DPID, DPID, u_long )
{
	u_long idPlayer;
	ar >> idPlayer;

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );
	
	CPlayer* pPlayer	= g_PlayerMng.GetPlayer( idPlayer );
	if( !pPlayer )
		return;
	
	CGuildWar* pWar	= g_GuildWarMng.GetWar( pPlayer->m_idWar );
	if( !pWar )
		return;

	CGuild* pDecl	= g_GuildMng.GetGuild( pWar->m_Decl.idGuild );
	CGuild* pAcpt	= g_GuildMng.GetGuild( pWar->m_Acpt.idGuild );
	if( !pDecl || !pAcpt )
		return;

	BOOL bDecl	= pWar->IsDecl( pPlayer->m_idGuild );
	if( pDecl->IsMaster( pPlayer->uKey ) || pAcpt->IsMaster( pPlayer->uKey ) )
	{
		g_GuildWarMng.Result( pWar, pDecl, pAcpt, (int)bDecl/*nType*/ );
	}
	else
	{
		if( bDecl )
		{
			pWar->m_Decl.nDead++;
			g_dpDatabaseClient.SendWarDead( pWar->m_idWar, pDecl->m_idGuild );	// trans
		}
		else
		{
			pWar->m_Acpt.nDead++;
			g_dpDatabaseClient.SendWarDead( pWar->m_idWar, pAcpt->m_idGuild );	// trans
		}

		SendWarDead( pWar->m_idWar, bDecl );	// worldserver
		for( map<u_long, CGuildMember*>::iterator i = pDecl->m_mapPMember.begin();
			i != pDecl->m_mapPMember.end(); ++i )
		{
			CPlayer* pPlayertmp	= g_PlayerMng.GetPlayer( i->second->m_idPlayer );
			if( pPlayertmp )
				g_DPCacheSrvr.SendWarDead( pWar->m_idWar, pPlayer->lpszPlayer, bDecl, pPlayertmp );
		}
		for( i	= pAcpt->m_mapPMember.begin(); i != pAcpt->m_mapPMember.end(); ++i )
		{
			CPlayer* pPlayertmp	= g_PlayerMng.GetPlayer( i->second->m_idPlayer );
			if( pPlayertmp )
				g_DPCacheSrvr.SendWarDead( pWar->m_idWar, pPlayer->lpszPlayer, bDecl, pPlayertmp );
		}
	}
}

void CDPCoreSrvr::OnWarMasterAbsent( CAr & ar, DPID, DPID, DPID, u_long )
{
	u_long idWar;
	BOOL bDecl;
	ar >> idWar >> bDecl;
	
	CMclAutoLock	Lock( g_GuildMng.m_AddRemoveLock );
	CGuildWar* pWar	= g_GuildWarMng.GetWar( idWar );
	if( pWar )
	{
		if( bDecl )
		{
			pWar->m_Decl.nAbsent++;
			g_dpDatabaseClient.SendWarMasterAbsent( idWar, pWar->m_Decl.idGuild );
		}
		else
		{
			pWar->m_Acpt.nAbsent++;
			g_dpDatabaseClient.SendWarMasterAbsent( idWar, pWar->m_Acpt.idGuild );
		}
	}
}

void CDPCoreSrvr::OnWarTimeout( CAr & ar, DPID, DPID, DPID, u_long )
{
	u_long idWar;
	ar >> idWar;

	TRACE( "OnWarTimeout, %d\n", idWar );

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );

	CGuildWar* pWar	= g_GuildWarMng.GetWar( idWar );
	if( !pWar )
		return;

	CGuild* pDecl	= g_GuildMng.GetGuild( pWar->m_Decl.idGuild );
	CGuild* pAcpt	= g_GuildMng.GetGuild( pWar->m_Acpt.idGuild );
	if( !pDecl || !pAcpt )
		return;

//	if( pWar )
	{
		if( pWar->m_Decl.nAbsent > pWar->m_Acpt.nAbsent )
		{
			g_GuildWarMng.Result( pWar, pDecl, pAcpt, WR_ACPT_AB );
//			g_GuildWarMng.Result( pWar, pDecl, pAcpt, WR_ACPT_GN );
		}
		else if( pWar->m_Decl.nAbsent < pWar->m_Acpt.nAbsent )
		{
			g_GuildWarMng.Result( pWar, pDecl, pAcpt, WR_DECL_AB );
//			g_GuildWarMng.Result( pWar, pDecl, pAcpt, WR_DECL_GN );
		}
		else
		{
			if( pWar->m_Decl.nDead > pWar->m_Acpt.nDead )
			{
				g_GuildWarMng.Result( pWar, pDecl, pAcpt, WR_ACPT_DD );
//				g_GuildWarMng.Result( pWar, pDecl, pAcpt, WR_ACPT_GN );
			}
			else if( pWar->m_Decl.nDead < pWar->m_Acpt.nDead )
			{
				g_GuildWarMng.Result( pWar, pDecl, pAcpt, WR_DECL_DD );
//				g_GuildWarMng.Result( pWar, pDecl, pAcpt, WR_DECL_GN );
			}
			else
			{
				g_GuildWarMng.Result( pWar, pDecl, pAcpt, WR_DRAW );
			}
		}
	}
}

void CDPCoreSrvr::SendWarMasterAbsent( u_long idWar, BOOL bDecl )
{
	BEFORESENDDUAL( ar, PACKETTYPE_WAR_MASTER_ABSENT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idWar << bDecl;
	SEND( ar, this, DPID_ALLPLAYERS );
}


void CDPCoreSrvr::SendSetFriendState( u_long uidPlayer, DWORD dwState )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SETFRIENDSTATE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uidPlayer << dwState;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendFriendInterceptState( u_long uIdPlayer, u_long uIdFriend )
{
	BEFORESENDDUAL( ar, PACKETTYPE_FRIENDINTERCEPTSTATE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uIdPlayer << uIdFriend;
	SEND( ar , this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendChangeLeader( u_long uPartyId, u_long uidChangeLeader )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PARTYCHANGELEADER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uPartyId << uidChangeLeader;
	SEND( ar, this, DPID_ALLPLAYERS );
}


void CDPCoreSrvr::OnAddFriendNameReqest( CAr & ar, DPID, DPID, DPID, u_long )
{
	u_long uLeader, uMember;
	LONG nLeaderJob;
	BYTE nLeaderSex;
	char szLeaderName[MAX_PLAYER] = {0,};
	char szMemberName[MAX_PLAYER] = {0,};
	ar >> uLeader >> uMember;
	ar >> nLeaderJob >> nLeaderSex;
	ar.ReadString( szLeaderName, MAX_PLAYER );
	ar.ReadString( szMemberName, MAX_PLAYER );

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );

	CPlayer* pLeader = g_PlayerMng.GetPlayer( uLeader );
	CPlayer* pMember = g_PlayerMng.GetPlayer( uMember );
	if( pLeader && pMember )
	{
		// 물어봄 친구등록을 할껀지
		g_DPCacheSrvr.SendAddFriend( uLeader, uMember, nLeaderJob, nLeaderSex, szLeaderName, pMember );
	}
	else
	{
		// 접속되어 있지 않습니다.
		if( pLeader )
			g_DPCacheSrvr.SendAddFriendNotFound( szMemberName, pLeader );
	}
}

void CDPCoreSrvr::OnBlock( CAr & ar, DPID, DPID, DPID, u_long )
{
	BYTE nGu;
	u_long uidPlayerTo, uidPlayerFrom;
	char szNameTo[MAX_PLAYER] = {0,};
	ar >> nGu;
	ar >> uidPlayerTo >> uidPlayerFrom;
	ar.ReadString( szNameTo, MAX_PLAYER );

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	
	CPlayer* pTo = g_PlayerMng.GetPlayer( uidPlayerFrom );
	if( pTo )
	{
		g_DPCacheSrvr.SendBlock( nGu, szNameTo, pTo );
	}
}


void CDPCoreSrvr::SendAddVoteResult( VOTE_INSERTED_INFO& info )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CW_ADDVOTERESULT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << info;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendAddRemoveVote( u_long idGuild, u_long idVote )		
{
	BEFORESENDDUAL( ar, PACKETTYPE_CW_MODIFYVOTE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idGuild;
	ar << idVote;
	ar << REMOVE_VOTE;
	ar << (BYTE)0;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendAddCloseVote( u_long idGuild, u_long idVote )		
{
	BEFORESENDDUAL( ar, PACKETTYPE_CW_MODIFYVOTE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idGuild;
	ar << idVote;
	ar << CLOSE_VOTE;
	ar << (BYTE)0;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendAddCastVote( u_long idGuild, u_long idVote, BYTE cbSelection )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CW_MODIFYVOTE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idGuild;
	ar << idVote;
	ar << CAST_VOTE;
	ar << (BYTE)cbSelection;
	SEND( ar, this, DPID_ALLPLAYERS );
}

// 길드로고 변경 
void CDPCoreSrvr::OnGuildLogo( CAr & ar, DPID, DPID, DPID, u_long )
{
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );

	u_long idGuild, idPlayer;
	DWORD dwLogo;

	ar >> idGuild >> idPlayer;
	ar >> dwLogo;
	
	CGuild* pGuild = g_GuildMng.GetGuild( idGuild );
	if( pGuild == NULL )
		return;

	if( pGuild->m_nLevel < 2 )						// 2미만?
		return;

	if( pGuild->IsMaster( idPlayer ) == FALSE )		// 길마?
		return;

	if( pGuild->GetWar() )
	{
		CPlayer* pPlayer	= g_PlayerMng.GetPlayer( idPlayer );
		if( pPlayer )
			g_DPCacheSrvr.SendDefinedText( TID_GAME_GUILDWARNOLOGO, pPlayer->dpidCache, pPlayer->dpidUser, "" );
		return;
	}

	if( pGuild->SetLogo( dwLogo ) == FALSE )
		return;

	g_dpDatabaseClient.SendGuildLogo( idGuild, dwLogo );

	{
		BEFORESENDDUAL( ar, PACKETTYPE_CW_GUILDLOGO, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << idGuild << dwLogo;
		SEND( ar, this, DPID_ALLPLAYERS );
	}
}

// 길드공헌도
void CDPCoreSrvr::OnGuildContribution( CAr & ar, DPID, DPID, DPID, u_long )
{
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );

	ULONG uServerID;
	CONTRIBUTION_CHANGED_INFO info;

	ar >> uServerID;
	ar >> info;

	CGuild* pGuild = g_GuildMng.GetGuild( info.idGuild );
	if( pGuild == NULL )
		return;

	if( pGuild->m_szGuild[0] == '\0' )	// 이름이 없으면 공헌할 수 없다.
		return;
	
	pGuild->SetContribution( info );

	{
		BEFORESENDDUAL( ar, PACKETTYPE_CW_GUILDCONTRIBUTION, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << uServerID;
		ar << info;
		SEND( ar, this, DPID_ALLPLAYERS );
	}
}

// 길드 공지사항 변경 
void CDPCoreSrvr::OnGuildNotice( CAr & ar, DPID, DPID, DPID, u_long )
{
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );

	u_long idGuild, idPlayer;
	char szNotice[MAX_BYTE_NOTICE];
	ar >> idGuild >> idPlayer;
	ar.ReadString( szNotice, MAX_BYTE_NOTICE );
	
	g_dpDatabaseClient.SendGuildNotice( idGuild, szNotice );

	{
		BEFORESENDDUAL( ar, PACKETTYPE_CW_GUILDNOTICE, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << idGuild;
		ar.WriteString(szNotice);
		SEND( ar, this, DPID_ALLPLAYERS );
	}
}

void CDPCoreSrvr::OnGuildRealPenya( CAr & ar, DPID, DPID, DPID, u_long )
{
	u_long idGuild;
	int nGoldGuild;
	ar >> idGuild >> nGoldGuild;


	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );

	CGuild* pGuild = g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		pGuild->m_nGoldGuild = nGoldGuild;
	}
	
	{
		BEFORESENDDUAL( ar, PACKETTYPE_GUILD_DB_REALPENYA, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << idGuild << nGoldGuild;
		SEND( ar, this, DPID_ALLPLAYERS );
	}
}

#if __VER < 11 // __SYS_PLAYER_DATA
void CDPCoreSrvr::OnChangeGuildJobLevel( CAr & ar, DPID, DPID, DPID, u_long )
{
	u_long uidGuild, uidPlayer;
	LONG nJob, nLevel;
	ar >> uidGuild >> uidPlayer >> nJob >> nLevel;
	
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );
	
	CGuild* pGuild = g_GuildMng.GetGuild( uidGuild );
	if( pGuild && pGuild->IsMember( uidPlayer ) )
	{
		CGuildMember* pGuildMember = pGuild->GetMember( uidPlayer );
		if( pGuildMember )
		{
			pGuildMember->m_nJob = nJob;
			pGuildMember->m_nLevel = nLevel;

			BEFORESENDDUAL( ar, PACKETTYPE_CHANGEGUILDJOBLEVEL, DPID_UNKNOWN, DPID_UNKNOWN );
			ar << uidGuild << uidPlayer << nJob << nLevel;
			SEND( ar, this, DPID_ALLPLAYERS );
		}
	}
}
#endif	// __SYS_PLAYER_DATA

void CDPCoreSrvr::UpdateWantedList()
{
#ifdef __INTERNALSERVER
	SendCWWantedList();
#endif
}

// 현상금을 누적 요청
void CDPCoreSrvr::OnWCWantedGold( CAr & ar, DPID, DPID, DPID, u_long )
{
	u_long	idPlayer;
	int		nGold;
	char    szPlayer[MAX_PLAYER];
	char    szMsg[WANTED_MSG_MAX + 1];

	ar.ReadString( szPlayer, MAX_PLAYER );
	ar >> idPlayer >> nGold;
	ar.ReadString( szMsg, WANTED_MSG_MAX + 1 );

	ASSERT( nGold > 0 );

	CWantedList& wantedList = CWantedList::GetInstance();
	if( wantedList.SetEntry( idPlayer, szPlayer, nGold, szMsg ) )
		UpdateWantedList();
}

// 코어서버에 (현상금얻기 & 현상금 clear) 요청  
void CDPCoreSrvr::OnWCWantedReward( CAr & ar, DPID dpid, DPID, DPID, u_long )
{
	u_long	idPlayer, idAttacker;
	DWORD	dwWorldID;		// 월드아이디 
	D3DXVECTOR3 vPos;		// 아이템을 떨굴 위치  
	char szFormat[256];
	char szReward[64];

	ar >> idPlayer >> idAttacker >> dwWorldID >> vPos;
	ar.ReadString( szFormat, 256 );
#ifdef __LAYER_1015
	int nLayer;
	ar >> nLayer;
#endif	// __LAYER_1015

	CWantedList& wantedList = CWantedList::GetInstance();
	BOOL bUpdatable;
	__int64 nGold = wantedList.GetReward( idPlayer, szReward, bUpdatable );
	
	if( nGold > 0 )
	{
		BEFORESENDDUAL( out, PACKETTYPE_CW_WANTED_REWARD, DPID_UNKNOWN, DPID_UNKNOWN );
		out << idPlayer << idAttacker << nGold << dwWorldID << vPos;
#ifdef __LAYER_1015
		out << nLayer;
#endif	// __LAYER_1015
		SEND( out, this, dpid );		

		char szMsg[1024];
		char szGold[64];
		sprintf( szGold, "%I64d", nGold );

		CString strGold = GetNumberFormatEx( szGold );
		//sprintf( szMsg, "%s님이 현상범 %s를 잡아 현상금 %s페냐를 얻었습니다.", szAttacker, szReward, strGold );
		sprintf( szMsg, szFormat, szReward, strGold );
		g_DPCacheSrvr.SendSystem( szMsg );

		if( bUpdatable )
			UpdateWantedList();
	}
}

// 현상금 리스트를 월드서버들에 보낸다.
void CDPCoreSrvr::SendCWWantedList( DPID dpid )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CW_WANTED_LIST, DPID_UNKNOWN, DPID_UNKNOWN );
	
	CWantedList& wantedList = CWantedList::GetInstance();
	wantedList.Write( ar );

	SEND( ar, this, dpid );		
}

void CDPCoreSrvr::OnSetPartyDuel( CAr & ar, DPID, DPID, DPID, u_long )
{
	u_long idParty1, idParty2;
	BOOL bDuel;
	ar >> idParty1 >> idParty2 >> bDuel;
	
	CMclAutoLock	Lock( g_PartyMng.m_AddRemoveLock );

	CParty* pParty	= g_PartyMng.GetParty( idParty1 );
	if( pParty )
		pParty->m_idDuelParty	= ( bDuel? idParty2: 0 );
	pParty	= g_PartyMng.GetParty( idParty2 );
	if( pParty )
		pParty->m_idDuelParty	= ( bDuel? idParty1: 0 );
}

void CDPCoreSrvr::SendSetPlayerName( u_long idPlayer, const char* lpszPlayer, DWORD dwObjId, BOOL f )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUERYSETPLAYERNAME, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer;
	ar.WriteString( lpszPlayer );
	ar << dwObjId << f;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendDestroyPlayer( u_long idPlayer, DPID dpid )
{
	BEFORESENDDUAL( ar, PACKETTYPE_DESTROY_PLAYER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer;
	SEND( ar, this, dpid );
}

void CDPCoreSrvr::OnQuerySetGuildName( CAr & ar, DPID, DPID, DPID, u_long )
{
	u_long idPlayer;
	u_long idGuild;
	char lpszGuild[MAX_G_NAME];
	BYTE nId;

	ar >> idPlayer >> idGuild;
	ar.ReadString( lpszGuild, MAX_G_NAME );
	ar >> nId;

	BYTE err	= 0;

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );
	CPlayer* pPlayer	= g_PlayerMng.GetPlayer( idPlayer );

	if( !pPlayer )
		return;

	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		if( pGuild->IsMaster( idPlayer )  )
		{
			if( g_GuildMng.SetName( pGuild, lpszGuild ) )
			{
				g_dpDatabaseClient.SendGuildSetName( idGuild, pGuild->m_szGuild );	// trans
				g_DPCacheSrvr.SendGuildSetName( idGuild, pGuild->m_szGuild );	// client
			}
			else
			{
				err		= 3;	// already exists
			}
		}
		else
		{
			err		= 2;	// is not kingpin
		}
	}
	else
	{
		err		= 1;		// guild not found
	}
	g_dpCoreSrvr.SendGuildSetName( idGuild, lpszGuild, idPlayer, nId, err );		// world
}


void CDPCoreSrvr::OnSetSnoop( CAr & ar, DPID, DPID, DPID, u_long )
{
	u_long idPlayer, idSnoop;
	BOOL bRelease;

	ar >> idPlayer >> idSnoop >> bRelease;

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CPlayer* pPlayer	= g_PlayerMng.GetPlayer( idPlayer );
	if( pPlayer )
	{
		OutputDebugString( "CORESERVER.EXE\t// PACKETTYPE_SETSNOOP" );
		pPlayer->m_idSnoop	= ( bRelease? 0: idSnoop );
		SendSetSnoop( idPlayer, idSnoop, bRelease );
	}
	else
	{
		CPlayer* pSnoop	= g_PlayerMng.GetPlayer( idSnoop );
		if( pSnoop )
			g_DPCacheSrvr.SendSay( "", "", idSnoop, idPlayer, "", pSnoop, 1 );
	}
}

void CDPCoreSrvr::OnChat( CAr & ar, DPID, DPID, DPID, u_long )
{
	char lpSnoopString[2048];

	u_long idPlayer1, idPlayer2;
	char lpszChat[1024]		= {  0, };
	
	ar >> idPlayer1 >> idPlayer2;
	ar.ReadString( lpszChat, 1024 );

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CPlayer* pPlayer1	= g_PlayerMng.GetPlayer( idPlayer1 );
	CPlayer* pPlayer2	= g_PlayerMng.GetPlayer( idPlayer2 );
	if( pPlayer1 && pPlayer2 && pPlayer2->m_idSnoop > 0 )
	{
		CPlayer* pSnoop	= g_PlayerMng.GetPlayer( pPlayer2->m_idSnoop );
		if( pSnoop )
		{
			sprintf( lpSnoopString, "[c][%s->%s] %s", pPlayer1->lpszPlayer, pPlayer2->lpszPlayer, lpszChat );
			g_DPCacheSrvr.SendSnoop( lpSnoopString, pSnoop );
		}
	}
}

void CDPCoreSrvr::OnGCState( CAr & ar, DPID, DPID, DPID, u_long )
{
	ar >> m_nGCState;
	SendGuildCombatState();
}
void CDPCoreSrvr::OnGCRemoveParty( CAr & ar, DPID, DPID, DPID, u_long )
{
	u_long uPartyid;
	u_long idMember;
	
	ar >> uPartyid >> idMember;
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CMclAutoLock	Lock1( g_PartyMng.m_AddRemoveLock );
	CParty* pParty	= g_PartyMng.GetParty( uPartyid );
	if( pParty )
	{
		if( pParty->DeleteMember( idMember ) )
		{
			g_dpCoreSrvr.SendRemoveParty( pParty->m_uPartyId, pParty->m_aMember[0].m_uPlayerId, idMember );
			
			if( pParty->GetSizeofMember() < 2 )
			{
				CPlayer* pMember		= g_PlayerMng.GetPlayer( pParty->GetPlayerId( 0 ) );
				if( pMember )
					pMember->m_uPartyId	= 0;
				g_PartyMng.DeleteParty( pParty->m_uPartyId );
			}
		}
	}
}
void CDPCoreSrvr::OnGCAddParty( CAr & ar, DPID, DPID, DPID, u_long )
{
	u_long idLeader, idMember;
	LONG nLeaderLevel, nMemberLevel, nLeaderJob, nMemberJob;
	DWORD dwLSex, dwMSex;
	ar >> idLeader >> nLeaderLevel >> nLeaderJob >> dwLSex;
	ar >> idMember >> nMemberLevel >> nMemberJob >> dwMSex;
	CPlayer* pLeader;
	CPlayer* pMember;

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	
	pLeader = g_PlayerMng.GetPlayer( idLeader );
	pMember = g_PlayerMng.GetPlayer( idMember );

	if( !pLeader || !pMember )
	{
//		player not found
		return;
	}

	CMclAutoLock	Lock1( g_PartyMng.m_AddRemoveLock );

	CParty* pParty	= g_PartyMng.GetParty( pMember->m_uPartyId );	// member's party
	if( pParty && pParty->IsMember( pMember->uKey ) )
	{
		g_DPCacheSrvr.SendErrorParty( ERROR_DIFFRENTPARTY, pLeader );
		return;
	}

	pMember->m_uPartyId	= 0;
	pParty	= g_PartyMng.GetParty( pLeader->m_uPartyId );	// leader's party
	if( pParty && pParty->IsMember( pLeader->uKey ) )	// add
	{
		if( pParty->m_nSizeofMember >= MAX_PTMEMBER_SIZE )
		{
			g_DPCacheSrvr.SendErrorParty( ERROR_FULLPARTY, pLeader );
			return;
		}
		if( pParty->m_idDuelParty > 0 )
		{
			return;
		}
#if __VER >= 11 // __SYS_PLAYER_DATA
		if( FALSE == pParty->NewMember( idMember ) )
#else	// __SYS_PLAYER_DATA
		if( FALSE == pParty->NewMember( idMember, nMemberLevel, nMemberJob, dwMSex, pMember->lpszPlayer ) )
#endif	// __SYS_PLAYER_DATA
		{
			// error
		}
		else	// ok
		{
			pMember->m_uPartyId		= pParty->m_uPartyId;
			g_dpCoreSrvr.SendAddPartyMember( pParty->m_uPartyId, idLeader, nLeaderLevel, nLeaderJob, (BYTE)( dwLSex ), idMember, nMemberLevel, nMemberJob, (BYTE)( dwMSex ) );
		}
	}
	else	// new
	{
		u_long uPartyId		=
			g_PartyMng.NewParty( idLeader, nLeaderLevel, nLeaderJob, (BYTE)( dwLSex ), pLeader->lpszPlayer,
			idMember, nMemberLevel, nMemberJob, (BYTE)( dwMSex ), pMember->lpszPlayer );
		if( uPartyId != 0 )
		{
			pLeader->m_uPartyId		= pMember->m_uPartyId	= uPartyId;
			g_dpCoreSrvr.SendAddPartyMember( uPartyId, idLeader, nLeaderLevel, nLeaderJob, (BYTE)( dwLSex ), idMember, nMemberLevel, nMemberJob, (BYTE)( dwMSex ) );
		}
		else
		{
//			error
		}
	}
}

void CDPCoreSrvr::SendSetSnoop( u_long idPlayer, u_long idSnoop, BOOL bRelease )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SETSNOOP, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << idSnoop << bRelease;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::OnSetSnoopGuild( CAr & ar, DPID, DPID, DPID, u_long )
{
	u_long idGuild;
	BOOL bRelease;

	ar >> idGuild >> bRelease;

	CMclAutoLock	Lock( g_GuildMng.m_AddRemoveLock );
	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		OutputDebugString( "CORESERVER.EXE\t// PACKETTYPE_SETSNOOPGUILD" );
		pGuild->m_bLog	= !bRelease;
	}
}


void CDPCoreSrvr::SendRemoveUser( DWORD dwSerial )
{
	BEFORESENDDUAL( ar, PACKETTYPE_UNIFY, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwSerial;
	SEND( ar, this, DPID_ALLPLAYERS );
}


#ifdef __EVENT0913
void CDPCoreSrvr::SendEvent0913( BOOL f )
{
	BEFORESENDDUAL( ar, PACKETTYPE_EVENT0913, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << f;
	SEND( ar, this, DPID_ALLPLAYERS );
}
#endif	// __EVENT0913
#ifdef __EVENT1206
void CDPCoreSrvr::SendEvent1206( BOOL f )
{
	BEFORESENDDUAL( ar, PACKETTYPE_EVENT1206, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << f;
	SEND( ar, this, DPID_ALLPLAYERS );
}
#endif	// __EVENT1206

void CDPCoreSrvr::SendEvent( DWORD dwEvent )
{
	BEFORESENDDUAL( ar, PACKETTYPE_EVENT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwEvent;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendGuildCombatState( void )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILDCOMBAT_STATE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << m_nGCState;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::OnPing( CAr & ar, DPID dpid, DPID, DPID, u_long )
{
	time_t tSend;
	ar >> tSend;

	{
		BEFORESENDDUAL( ar, PACKETTYPE_PING, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << tSend << time( NULL );
		SEND( ar, this, dpid );
	}
}

#if __VER >= 14 // __INSTANCE_DUNGEON
void CDPCoreSrvr::SendInstanceDungeonAllInfo( int nType, CInstanceDungeonBase* pID, DPID dpId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_INSTANCEDUNGEON_ALLINFO, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nType;
	pID->SerializeAllInfo( ar );
	SEND( ar, this, dpId );
}

void CDPCoreSrvr::OnInstanceDungeonCreate( CAr & ar, DPID, DPID, DPID, u_long )
{
	int nType;
	DWORD dwDungeonId;
	ID_INFO ID_Info( NULL_ID, NULL_ID );

	ar >> nType >> dwDungeonId;
	ID_Info.Serialize( ar );
	
	CInstanceDungeonHelper::GetInstance()->OnCreateDungeon( nType, ID_Info, dwDungeonId );
}

void CDPCoreSrvr::SendInstanceDungeonCreate( int nType, DWORD dwDungeonId, ID_INFO & ID_Info )
{
	BEFORESENDDUAL( ar, PACKETTYPE_INSTANCEDUNGEON_CREATE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nType << dwDungeonId;
	ID_Info.Serialize( ar );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::OnInstanceDungeonDestroy( CAr & ar, DPID, DPID, DPID, u_long )
{
	int nType;
	DWORD dwDungeonId;
	ID_INFO ID_Info( NULL_ID, NULL_ID );
	
	ar >> nType >> dwDungeonId;
	ID_Info.Serialize( ar );
	
	CInstanceDungeonHelper::GetInstance()->OnDestroyDungeon( nType, ID_Info, dwDungeonId );
}

void CDPCoreSrvr::SendInstanceDungeonDestroy( int nType, DWORD dwDungeonId, ID_INFO & ID_Info )
{
	BEFORESENDDUAL( ar, PACKETTYPE_INSTANCEDUNGEON_DESTROY, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nType << dwDungeonId;
	ID_Info.Serialize( ar );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::OnInstanceDungeonSetCoolTimeInfo( CAr & ar, DPID, DPID, DPID, u_long )
{
	int nType;
	DWORD dwPlayerId;
	COOLTIME_INFO CT_Info;
	ULONG uKey;
	
	ar >> uKey >> nType >> dwPlayerId;
	CT_Info.Serialize( ar );
	
	CInstanceDungeonHelper::GetInstance()->OnSetDungeonCoolTimeInfo( uKey, nType, CT_Info, dwPlayerId );
}

void CDPCoreSrvr::SendInstanceDungeonSetCoolTimeInfo( ULONG uKey, int nType, DWORD dwPlayerId, COOLTIME_INFO CT_Info )
{
	BEFORESENDDUAL( ar, PACKETTYPE_INSTANCEDUNGEON_SETCOOLTIME, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uKey << nType << dwPlayerId;
	CT_Info.Serialize( ar );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendInstanceDungeonDeleteCoolTimeInfo( int nType, DWORD dwPlayerId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_INSTANCEDUNGEON_DELETECOOLTIME, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nType << dwPlayerId;
	SEND( ar, this, DPID_ALLPLAYERS );
}
#endif // __INSTANCE_DUNGEON
#ifdef __QUIZ
void CDPCoreSrvr::OnQuizSystemMessage( CAr & ar, DPID, DPID, DPID, u_long )
{
	BOOL bAll;
	int nDefinedTextId, nChannel, nTime;
	ar >> nDefinedTextId >> bAll >> nChannel >> nTime;
	g_DPCacheSrvr.SendQuizSystemMessage( nDefinedTextId, bAll, nChannel, nTime );
}
#endif // __QUIZ
