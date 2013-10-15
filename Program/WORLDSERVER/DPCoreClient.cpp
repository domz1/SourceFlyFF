#include "StdAfx.h"
#include "defineText.h"
#include "defineSkill.h"
#include "DPDatabaseClient.h"
#include "DPSrvr.h"
#include "DPCoreClient.h"
#include "User.h"
#include "ServerDesc.h"
#include "WorldMng.h"

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

#include "eveschool.h"
extern	CGuildCombat	g_GuildCombatMng;

extern	CUserMng	g_UserMng;
extern	CWorldMng	g_WorldMng;
extern	CDPSrvr		g_DPSrvr;

extern  CDPDatabaseClient	g_dpDBClient;

#include "Party.h"
extern	CPartyMng	g_PartyMng;
#include "Environment.h"

#ifndef __ENVIRONMENT_EFFECT
extern	CEnvironment	g_Environment;
#endif // __ENVIRONMENT_EFFECT

#include "guild.h"
extern	CGuildMng	g_GuildMng;
#include "guildwar.h"
extern	CGuildWarMng	g_GuildWarMng;

#include "WantedListSnapshot.h"

#ifdef __COLOSSEUM
#include "Colosseum.h"
#endif // __COLOSSEUM

#ifndef __MEM_TRACE
#ifdef _DEBUG
#define new new( __FILE__, __LINE__ )
#endif
#endif	// __MEM_TRACE

#ifndef GETBLOCK
#define	GETBLOCK( ar, lpBuf, nBufSize )	\
										int nBufSize;	\
										LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );
#endif	// __GETBLOCK


CDPCoreClient	g_DPCoreClient;


CDPCoreClient::CDPCoreClient()
{
	BEGIN_MSG;

	ON_MSG( PACKETTYPE_LOAD_WORLD, OnLoadWorld );
	ON_MSG( PACKETTYPE_QUERYTICKCOUNT, OnQueryTickCount );
	ON_MSG( PACKETTYPE_RECHARGE_IDSTACK, OnRecharge );
	ON_MSG( PACKETTYPE_SHOUT, OnShout );
	ON_MSG( PACKETTYPE_PLAYMUSIC, OnPlayMusic );
	ON_MSG( PACKETTYPE_PLAYSOUND, OnPlaySound );
//	ON_MSG( PACKETTYPE_MODIFYMODE, OnModifyMode );
	
	ON_MSG( PACKETTYPE_ERRORPARTY, OnErrorParty );
	ON_MSG( PACKETTYPE_ADDPARTYMEMBER, OnAddPartyMember );
	ON_MSG( PACKETTYPE_REMOVEPARTYMEMBER, OnRemovePartyMember );
	ON_MSG( PACKETTYPE_ADDPLAYERPARTY, OnAddPlayerParty );
	ON_MSG( PACKETTYPE_REMOVEPLAYERPARTY, OnRemovePlayerParty );
#if __VER < 11 // __SYS_PLAYER_DATA
	ON_MSG( PACKETTYPE_MEMBERLEVEL, OnPartyMemberLevel );
	ON_MSG( PACKETTYPE_MEMBERJOB, OnPartyMemberJob );
	ON_MSG( PACKETTYPE_CHANGEGUILDJOBLEVEL, OnChangeGuildJobLevel );
	ON_MSG( PACKETTYPE_CHANGEJOB, OnFriendChangeJob );
#endif	// __SYS_PLAYER_DATA
	ON_MSG( PACKETTYPE_GUILD_MEMBER_LEVEL, OnGuildMemberLv );
	ON_MSG( PACKETTYPE_SETPARTYEXP, OnSetPartyExp );
	ON_MSG( PACKETTYPE_REMOVEPARTYPOINT, OnRemovePartyPoint );
	ON_MSG( PACKETTYPE_CHANGETROUP, OnPartyChangeTroup );
	ON_MSG( PACKETTYPE_CHANPARTYNAME, OnPartyChangeName );
	ON_MSG( PACKETTYPE_SETPARTYMODE, OnSetPartyMode );
	ON_MSG( PACKETTYPE_PARTYCHANGEITEMMODE, OnPartyChangeItemMode );
	ON_MSG( PACKETTYPE_PARTYCHANGEEXPMODE, OnPartyChangeExpMode );
	ON_MSG( PACKETTYPE_ADDFRIEND, OnAddFriend );
	ON_MSG( PACKETTYPE_REMOVEFRIEND, OnRemovefriend );

#ifdef __ENVIRONMENT_EFFECT

	ON_MSG( PACKETTYPE_ENVIRONMENTALL, OnEnvironmentEffect );

#else // __ENVIRONMENT_EFFECT

	ON_MSG( PACKETTYPE_ENVIRONMENTSNOW, OnEnvironmentSnow );
	ON_MSG( PACKETTYPE_ENVIRONMENTRAIN, OnEnvironmentRain );
	ON_MSG( PACKETTYPE_ENVIRONMENTALL, OnEnvironmentAll );

#endif // __ENVIRONMENT_EFFECT

	ON_MSG( PACKETTYPE_PARTYCHAT, OnPartyChat );

	ON_MSG( PACKETTYPE_CREATE_GUILD, OnCreateGuild );
	ON_MSG( PACKETTYPE_DESTROY_GUILD, OnDestroyGuild );
	ON_MSG( PACKETTYPE_ADD_GUILD_MEMBER, OnAddGuildMember );
	ON_MSG( PACKETTYPE_REMOVE_GUILD_MEMBER, OnRemoveGuildMember );
	ON_MSG( PACKETTYPE_CW_GUILDLOGO, OnGuildLogoACK );
	ON_MSG( PACKETTYPE_CW_GUILDCONTRIBUTION, OnGuildContributionACK );
	ON_MSG( PACKETTYPE_CW_GUILDNOTICE, OnGuildNoticeACk );
	ON_MSG( PACKETTYPE_CW_ADDVOTERESULT, OnAddVoteResultACk );
	ON_MSG( PACKETTYPE_CW_MODIFYVOTE, OnModifyVote );
	ON_MSG( PACKETTYPE_GUILD_AUTHORITY, OnGuildAuthority );
	ON_MSG( PACKETTYPE_GUILD_PENYA, OnGuildPenya );
	ON_MSG( PACKETTYPE_GUILD_DB_REALPENYA, OnGuildRealPenya );
	ON_MSG( PACKETTYPE_GUILD_SETNAME, OnGuildSetName );
	ON_MSG( PACKETTYPE_GUILD_MSG_CONTROL, OnGuildMsgControl)
	ON_MSG( PACKETTYPE_GUILD_CLASS, OnGuildClass );
	ON_MSG( PACKETTYPE_GUILD_NICKNAME, OnGuildNickName );

	ON_MSG( PACKETTYPE_GUILD_MEMBERLOGOUT, OnGuildMemberLogOut );
	ON_MSG( PACKETTYPE_ACPT_GUILD_WAR, OnAcptWar );
	ON_MSG( PACKETTYPE_WAR_DEAD, OnWarDead );
	ON_MSG( PACKETTYPE_WAR_END, OnWarEnd );
	ON_MSG( PACKETTYPE_SURRENDER, OnSurrender );

	ON_MSG( PACKETTYPE_FRIENDINTERCEPTSTATE, OnFriendInterceptState );
	ON_MSG( PACKETTYPE_SETFRIENDSTATE, OnSetFriendState );

	ON_MSG( PACKETTYPE_GAMERATE, OnGameRate );
	ON_MSG( PACKETTYPE_LOADCONSTANT, OnLoadConstant );
	ON_MSG( PACKETTYPE_SETMONSTERRESPAWN, OnSetMonsterRespawn );
	ON_MSG( PACKETTYPE_PARTYCHANGELEADER, OnPartyChangeLeader );

	ON_MSG( PACKETTYPE_CW_WANTED_LIST, OnCWWantedList );
	ON_MSG( PACKETTYPE_CW_WANTED_REWARD, OnCWWantedReward );
	ON_MSG( PACKETTYPE_QUERYSETPLAYERNAME, OnSetPlayerName );
	ON_MSG( PACKETTYPE_SETSNOOP, OnSetSnoop );

#ifdef __EVENT0913
	ON_MSG( PACKETTYPE_EVENT0913, OnEvent0913 );
#endif	// __EVENT0913

#ifdef __EVENT1206
	ON_MSG( PACKETTYPE_EVENT1206, OnEvent1206 );
#endif	// __EVENT1206
	ON_MSG( PACKETTYPE_EVENT, OnEvent );
	ON_MSG( PACKETTYPE_CHG_MASTER, OnChgMaster );
	ON_MSG( PACKETTYPE_GUILDCOMBAT_STATE, OnGuildCombatState );
	ON_MSG( PACKETTYPE_UNIFY, OnRemoveUserFromCORE );
	ON_MSG( PACKETTYPE_PING, OnPing );

	ON_MSG( PACKETTYPE_DESTROY_PLAYER, OnDestroyPlayer );
    //////////////////////////////////////////////////
#if __VER >= 14 // __INSTANCE_DUNGEON
	ON_MSG( PACKETTYPE_INSTANCEDUNGEON_ALLINFO, OnInstanceDungeonAllInfo );
	ON_MSG( PACKETTYPE_INSTANCEDUNGEON_CREATE, OnInstanceDungeonCreate );
	ON_MSG( PACKETTYPE_INSTANCEDUNGEON_DESTROY, OnInstanceDungeonDestroy );
	ON_MSG( PACKETTYPE_INSTANCEDUNGEON_SETCOOLTIME, OnInstanceDungeonSetCoolTimeInfo );
	ON_MSG( PACKETTYPE_INSTANCEDUNGEON_DELETECOOLTIME, OnInstanceDungeonDeleteCoolTimeInfo );
#endif // __INSTANCE_DUNGEON
	
	m_bAlive	= TRUE;
	m_hWait		= WSACreateEvent();
	m_uRecharge		= 0;
}

CDPCoreClient::~CDPCoreClient()
{
	CLOSE_HANDLE( m_hWait );
}

void CDPCoreClient::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{

}

void CDPCoreClient::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	CAr ar( (LPBYTE)lpMsg + sizeof(DPID) + sizeof(DPID), dwMsgSize - ( sizeof(DPID) + sizeof(DPID) ) );
	GETTYPE( ar );

	static map<DWORD, CString> mapstrProfile;
	map<DWORD, CString>::iterator it = mapstrProfile.find( dw );
	if( it == mapstrProfile.end() )
	{
		CString strTemp;
		strTemp.Format("CDPCoreClient::UserMessageHandler(0x%08x)", dw );
		it = mapstrProfile.insert( make_pair( dw, strTemp ) ).first;
	}
	_PROFILE( it->second );

	void ( theClass::*pfn )( theParameters )	=	GetHandler( dw );
	
	if( pfn ) {
		( this->*( pfn ) )( ar, *(UNALIGNED LPDPID)lpMsg, *(UNALIGNED LPDPID)( (LPBYTE)lpMsg + sizeof(DPID) ), NULL_ID );
	}
	else {
		switch( dw )
		{
			case PACKETTYPE_PASSAGE:
			{
				DWORD objid, dwtmp;
				ar >> objid >> dwtmp;

				pfn		= GetHandler( dwtmp );
				ASSERT( pfn != NULL );
				( this->*( pfn ) )( ar, *(UNALIGNED LPDPID)lpMsg, *(UNALIGNED LPDPID)( (LPBYTE)lpMsg + sizeof(DPID) ), (OBJID)objid );
				break;
			}
			case PACKETTYPE_BROADCAST:
				{
					DWORD dwtmp;
					ar >> dwtmp;

					pfn		= GetHandler( dwtmp );
					ASSERT( pfn != NULL );
					( this->*( pfn ) )( ar, *(UNALIGNED LPDPID)lpMsg, *(UNALIGNED LPDPID)( (LPBYTE)lpMsg + sizeof(DPID) ), NULL_ID );
					break;
				}
			default:
				Error( "Handler not found(%08x)\n",dw );
				break;
		}
	}


}

BOOL CDPCoreClient::Run( LPSTR lpszAddr, USHORT uPort, u_long uKey )
{
	if( ConnectToServer( lpszAddr, uPort, TRUE ) )
	{
		MyRegister( uKey );
		return ( WaitForSingleObject( m_hWait, INFINITE ) != WAIT_TIMEOUT );
	}
	return FALSE;
}

void CDPCoreClient::MyRegister( u_long uKey )
{
	BEFORESENDDUAL( ar, PACKETTYPE_MYREG, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uKey;	// uKey는 g_uKey와 동일한 값 
	ar << (DWORD)timeGetTime();
	SEND( ar, this, DPID_SERVERPLAYER );
}

BOOL CDPCoreClient::CheckIdStack( void )
{
	int cbUser	= g_UserMng.GetCount();
	u_long uMax		= cbUser * 128 + 20480;
	int cbStack		= prj.m_objmap.m_idStack.GetCount();
	
	if( cbStack + m_uRecharge < uMax * 5 / 10 )	// 50%
	{
		m_uRecharge		= uMax - cbStack;
		Recharge( m_uRecharge );
		return FALSE;
	}
	return ( m_uRecharge == 0 );
}

void CDPCoreClient::Recharge( u_long uBlockSize )
{
	BEFORESENDDUAL( ar, PACKETTYPE_RECHARGE_IDSTACK, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uBlockSize;
	SEND( ar, this, DPID_SERVERPLAYER );
}


void CDPCoreClient::SendToServer( LPBYTE lpBuffer, u_long uBufSize, DPID dpidCache, DPID dpidUser )
{
	CAr ar;
	int nBufSize;
	BYTE* lpBuf;

	ar << dpidCache << dpidUser;
	ar.Write( lpBuffer, uBufSize );	// overhead

	lpBuf	= ar.GetBuffer( &nBufSize );
	Send( (LPVOID)lpBuf, nBufSize, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendEventRealItem( u_long uIdPlayer, int nRealItemIndex, int nRealItemCount )
{
	BEFORESENDDUAL( ar, PACKETTYPE_RENEWEVNET, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uIdPlayer << nRealItemIndex << nRealItemCount;
	PASS( ar );
}

#if __VER < 11 // __SYS_PLAYER_DATA
void CDPCoreClient::SendPartyMemberLevel( CUser* pUser )
{
	BEFORESENDDUAL( ar, PACKETTYPE_MEMBERLEVEL, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << pUser->m_idparty << pUser->m_idPlayer << pUser->GetLevel();
	PASS( ar );
}

void CDPCoreClient::SendPartyMemberJob( CUser* pUser )
{
	BEFORESENDDUAL( ar, PACKETTYPE_MEMBERJOB, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << pUser->m_idparty << pUser->m_idPlayer << pUser->GetJob();
	PASS( ar );
}
#endif	// SYS_PLAYER_DATA

void CDPCoreClient::SendPartyLevel( CUser* pUser, DWORD dwLevel, DWORD dwPoint, DWORD dwExp )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PARTYLEVEL, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << pUser->m_idparty << pUser->m_idPlayer << dwLevel << dwPoint << dwExp;	
	PASS( ar );
}
#if __VER >= 12 // __JHMA_VER12_1	//12차 극단유료아이템
void CDPCoreClient::SendAddPartyExp( u_long uPartyId, int nMonLv, BOOL bSuperLeader , BOOL bLeaderSMExpUp )
{
	//극단에 속해있으면 포인트를 올려줌( core에서는 포인터만 가지고 있고 월드에서는 포인터를 이용하여~ 극단레벨을 구함)
	BEFORESENDDUAL( ar, PACKETTYPE_ADDPARTYEXP, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uPartyId << nMonLv << bSuperLeader << bLeaderSMExpUp;
	PASS( ar );
}
#else // //12차 극단유료아이템
void CDPCoreClient::SendAddPartyExp( u_long uPartyId, int nMonLv, BOOL bSuperLeader )
{
	//극단에 속해있으면 포인트를 올려줌( core에서는 포인터만 가지고 있고 월드에서는 포인터를 이용하여~ 극단레벨을 구함)
	BEFORESENDDUAL( ar, PACKETTYPE_ADDPARTYEXP, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uPartyId << nMonLv << bSuperLeader;
	PASS( ar );
}
#endif // //12차 극단유료아이템

void CDPCoreClient::SendRemovePartyPoint( u_long uPartyId, int nRemovePoint )
{
	if( nRemovePoint != 0 )
	{
		//극단에 속해있으면 포인트를 올려줌( core에서는 포인터만 가지고 있고 월드에서는 포인터를 이용하여~ 극단레벨을 구함)
		BEFORESENDDUAL( ar, PACKETTYPE_REMOVEPARTYPOINT, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << uPartyId << nRemovePoint;
		PASS( ar );
	}
}

#if __VER < 11 // __SYS_PLAYER_DATA
void CDPCoreClient::SendFriendChangeJob( CUser* pUser )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CHANGEJOB, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << pUser->m_idPlayer << pUser->GetJob();
	PASS( ar );
}
#endif	// __SYS_PLAYER_DATA

void CDPCoreClient::SendGameRate( FLOAT fRate, BYTE nFlag )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GAMERATE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << fRate;
	ar << nFlag;
	PASS( ar );	
}

void CDPCoreClient::SendLoadConstant()
{
	BEFORESENDDUAL( ar, PACKETTYPE_LOADCONSTANT, DPID_UNKNOWN, DPID_UNKNOWN );
	PASS( ar );		
}

void CDPCoreClient::SendSetMonsterRespawn( u_long uidPlayer, DWORD dwMonsterID, DWORD dwRespawnNum, DWORD dwAttackNum, DWORD dwRect, DWORD dwRespawnTime, BOOL bFlying )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SETMONSTERRESPAWN, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uidPlayer;
	ar << dwMonsterID << dwRespawnNum << dwAttackNum << dwRect << dwRespawnTime;
	ar << bFlying;
	PASS( ar );	
}

#if __VER < 11 // __SYS_PLAYER_DATA
void CDPCoreClient::SendGuildChangeJobLevel( CUser * pUser )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CHANGEGUILDJOBLEVEL, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << pUser->m_idGuild << pUser->m_idPlayer << pUser->GetJob() << pUser->GetLevel();
	PASS( ar );
}
#endif	// __SYS_PLAYER_DATA

void CDPCoreClient::SendGuildMsgControl_Bank_Item( CUser* pUser, CItemElem* pItemElem, BYTE p_Mode )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILD_MSG_CONTROL, DPID_UNKNOWN, DPID_UNKNOWN );

	GUILD_MSG_HEADER	Header;
	Header.HeadAMain	= p_Mode;
	Header.HeadASub		= (WORD)( pUser->m_idGuild );
	Header.HeadBMain	= GUILD_MSG_HEADER::GUILD_BANK;
	Header.HeadBSub		= GUILD_MSG_HEADER::ITEM;
	
	if (pUser->GetGuild())
	{
		ar.Write(&Header, sizeof(GUILD_MSG_HEADER));
		pItemElem->Serialize( ar );
	}
	
	PASS( ar );
}

void CDPCoreClient::SendGuildMsgControl_Bank_Penya( CUser* pUser, DWORD p_Penya, BYTE p_Mode, BYTE cbCloak )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILD_MSG_CONTROL, DPID_UNKNOWN, DPID_UNKNOWN );

	GUILD_MSG_HEADER	Header;
	Header.HeadAMain	= p_Mode;
	Header.HeadASub		= (WORD)( pUser->m_idGuild );
	Header.HeadBMain	= GUILD_MSG_HEADER::GUILD_BANK;
	Header.HeadBSub		= GUILD_MSG_HEADER::PENYA;
	
	if (pUser->GetGuild())
	{
		ar.Write(&Header, sizeof(GUILD_MSG_HEADER));
		ar << p_Penya;
		ar << cbCloak;
	}
	
	PASS( ar );
}

void CDPCoreClient::SendBlock( BYTE nGu, u_long uidPlayerTo, char *szNameTo, u_long uidPlayerFrom )
{
	BEFORESENDDUAL( ar, PACKETTYPE_BLOCK, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nGu;
	ar << uidPlayerTo << uidPlayerFrom;
	ar.WriteString( szNameTo );
	PASS( ar );
	
}

// Handlers
void CDPCoreClient::OnLoadWorld( CAr & ar, DPID, DPID, OBJID )
{
	CServerDesc desc;
	desc.Serialize( ar );
	g_PartyMng.Serialize( ar );
	g_GuildMng.Serialize( ar, FALSE );
	g_GuildWarMng.Serialize( ar );

#ifdef __ENVIRONMENT_EFFECT
	
	CEnvironment::GetInstance()->Serialize( ar );

#else // __ENVIRONMENT_EFFECT

	g_Environment.Serialize( ar );

#endif // __ENVIRONMENT_EFFECT

#ifdef __EVENT0913
	BOOL bEvent0913;
	ar >> bEvent0913;
	if( bEvent0913 )
	{
		prj.m_fItemDropRate		*= 2;
		g_eLocal.SetState( EVE_EVENT0913, 1 );
	}
	Error( "OnLoadWorld-%d, %f", g_eLocal.GetState( EVE_EVENT0913 ), prj.m_fItemDropRate );
#endif	// __EVENT0913

#ifdef __EVENT1206
	BOOL bEvent1206;
	ar >> bEvent1206;
	if( bEvent1206 )
	{
		g_eLocal.SetState( EVE_EVENT1206, 1 );
	}
#endif	// __EVENT1206

	DWORD dwEvent;
	ar >> dwEvent;
	if( dwEvent & EVE_FLAG1219 )
	{
		g_eLocal.SetState( EVE_EVENT1219, 1 );
	}
	if( dwEvent & EVE_FLAG0127 )
	{
		g_eLocal.SetState( EVE_EVENT0127, 1 );
	}
	if( dwEvent & EVE_FLAG0214 )
	{
		g_eLocal.SetState( EVE_EVENT0214, 1 );
	}
	
	char szMsg[1024] = "";
	for( list<CJurisdiction*>::iterator i = desc.m_lspJurisdiction.begin(); i != desc.m_lspJurisdiction.end(); ++i )
	{
		CJurisdiction* pJurisdiction = *i;
		
		LPWORLD lpWorld	= g_WorldMng.GetWorldStruct( pJurisdiction->m_dwWorldID );
		if( strlen( szMsg ) + strlen( lpWorld->m_szFileName ) < 1024 )
		{
			strcat( szMsg, lpWorld->m_szFileName );
			strcat( szMsg, " " );
		}

		g_WorldMng.Add( pJurisdiction );
	}
	SetLogInfo( LOGTYPE_MAP, szMsg );
	g_WorldMng.ReadObject();

	WSASetEvent( m_hWait );
}

void CDPCoreClient::OnRecharge( CAr & ar, DPID, DPID, OBJID )
{
	OBJID idBase;
	u_long uBlockSize;
	
	ar >> idBase >> uBlockSize;
//	u_long uMax		= 4096 * 32 + 10240;	// MAX_USER
	u_long uMax		= 2048 * 128 + 20480;	// MAX_USER
	if( uBlockSize > uMax )
	{
		WriteLog( "OnRecharge(): BLOCK SIZE TOO BIG, %d", uBlockSize );
		uBlockSize	= uMax;
	}
	prj.m_objmap.m_idStack.PushIdBlock( idBase, uBlockSize );
	m_uRecharge	= 0;
}
/*
void CDPCoreClient::OnModifyMode( CAr & ar, DPID, DPID, OBJID objid )
{
	CUser* pUser;

	pUser	= prj.GetUser( objid );
	if( IsValidObj( (CObj*)pUser ) )
	{
		DWORD dwMode;
		BYTE f;
		u_long idFrom;

		ar >> dwMode >> f >> idFrom;

		if( f )
			pUser->m_dwMode		|= dwMode;
		else
			pUser->m_dwMode		&= ~dwMode;

		g_UserMng.AddModifyMode( pUser );
	}
}
*/

void CDPCoreClient::SendJoin( u_long idPlayer, const char* szPlayer, BOOL bOperator )
{
	BEFORESENDDUAL( ar, PACKETTYPE_JOIN, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer;
	ar.WriteString( szPlayer );
	ar << bOperator;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendWhisper( u_long idFrom, u_long idTo, const CHAR* lpString )
{
	BEFORESENDDUAL( ar, PACKETTYPE_WHISPER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idFrom << idTo;
	ar.WriteString( lpString );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendSay( u_long idFrom, u_long idTo, const CHAR* lpString )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SAY, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idFrom << idTo;
	ar.WriteString( lpString );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendModifyMode( DWORD dwMode, BYTE fAdd, u_long idFrom, u_long idTo )
{
	BEFORESENDDUAL( ar, PACKETTYPE_MODIFYMODE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwMode << fAdd << idFrom << idTo;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendShout( CUser* pUser, const CHAR* lpString )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SHOUT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << pUser->m_idPlayer;
	ar.WriteString( lpString );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendPartyChat( CUser* pUser, const CHAR* lpString )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PARTYCHAT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << GETID( pUser );
	ar << pUser->m_idPlayer;
	ar.WriteString( lpString );
	SEND( ar, this, DPID_SERVERPLAYER );
}
#if __VER >= 12 // __JHMA_VER12_1	//12차 극단유료아이템
void CDPCoreClient::SendUserPartySkill( u_long uidPlayer, int nMode, DWORD dwSkillTime, int nRemovePoint ,int nCachMode )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PARTYSKILLUSE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uidPlayer;
	ar << nMode;
	ar << dwSkillTime;
	ar << nRemovePoint;
	ar << nCachMode;
	SEND( ar, this, DPID_SERVERPLAYER );
}
#else	//12차 극단유료아이템
void CDPCoreClient::SendUserPartySkill( u_long uidPlayer, int nMode, DWORD dwSkillTime, int nRemovePoint )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PARTYSKILLUSE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uidPlayer;
	ar << nMode;
	ar << dwSkillTime;
	ar << nRemovePoint;
	SEND( ar, this, DPID_SERVERPLAYER );
}
#endif // //12차 극단유료아이템
void CDPCoreClient::SendGMSay( u_long idPlayer, DWORD dwWorldID, const CHAR* lpString )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GMSAY, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer;
	ar << dwWorldID;
	ar.WriteString( lpString );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendPlayMusic( DWORD dwWorldID, u_long idmusic )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PLAYMUSIC, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << g_uIdofMulti;
	ar << dwWorldID;
	ar << idmusic;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendPlaySound( DWORD dwWorldID, u_long idsound )
{
	BEFORESENDDUAL( ar, PACKETTYPE_PLAYSOUND, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << g_uIdofMulti;
	ar << dwWorldID;
	ar << idsound;
	SEND( ar, this, DPID_SERVERPLAYER );
}

#ifdef __LAYER_1015
void CDPCoreClient::SendSummonPlayer( u_long idOperator, DWORD dwWorldID, const D3DXVECTOR3 & vPos, u_long idPlayer, int nLayer )
#else	// __LAYER_1015
void CDPCoreClient::SendSummonPlayer( u_long idOperator, DWORD dwWorldID, const D3DXVECTOR3 & vPos, u_long idPlayer )
#endif	// __LAYER_1015
{
	BEFORESENDDUAL( ar, PACKETTYPE_SUMMONPLAYER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idOperator;
	ar << g_uIdofMulti;
	ar << dwWorldID;
	ar << vPos << idPlayer;
#ifdef __LAYER_1015
	ar << nLayer;
#endif	// __LAYER_1015
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendTeleportPlayer( u_long idOperator, u_long idPlayer )
{
	BEFORESENDDUAL( ar, PACKETTYPE_TELEPORTPLAYER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idOperator << idPlayer;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendKillPlayer( u_long idOperator, u_long idPlayer )
{
	BEFORESENDDUAL( ar, PACKETTYPE_KILLPLAYER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idOperator << idPlayer;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendGetPlayerAddr( u_long idOperator, u_long idPlayer )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GETPLAYERADDR, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idOperator << idPlayer;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendGetPlayerCount( u_long idOperator )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GETPLAYERCOUNT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idOperator;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendGetCorePlayer( u_long idOperator )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GETCOREPLAYER, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idOperator;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendSystem( const CHAR* lpString )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SYSTEM, DPID_UNKNOWN, DPID_UNKNOWN );
	ar.WriteString( lpString );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendCaption( const CHAR* lpString, DWORD dwWorldId, BOOL bSmall )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CAPTION, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << bSmall;
	ar << dwWorldId;
	ar.WriteString( lpString );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendFallSnow( )
{
	BEFORESENDDUAL( ar, PACKETTYPE_FALLSNOW, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_SERVERPLAYER );
}
void CDPCoreClient::SendFallRain( )
{
	BEFORESENDDUAL( ar, PACKETTYPE_FALLRAIN, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_SERVERPLAYER );
}
void CDPCoreClient::SendStopSnow( )
{
	BEFORESENDDUAL( ar, PACKETTYPE_STOPSNOW, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_SERVERPLAYER );
}
void CDPCoreClient::SendStopRain( )
{
	BEFORESENDDUAL( ar, PACKETTYPE_STOPRAIN, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendGuildCombatState( int nState )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILDCOMBAT_STATE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nState;
	SEND( ar, this, DPID_SERVERPLAYER );
}
void CDPCoreClient::SendGCRemoveParty( u_long uidPartyid, u_long uidPlayer )
{
	BEFORESENDDUAL( ar, PACKETTYPE_REMOVEPARTY_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uidPartyid << uidPlayer;
	SEND( ar, this, DPID_SERVERPLAYER );
}
void CDPCoreClient::SendGCAddParty( u_long idLeader, LONG nLeaderLevel, LONG nLeaderJob, DWORD dwLSex, 
								   u_long idMember, LONG nMemberLevel, LONG nMemberJob, DWORD dwMSex )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ADDPARTY_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idLeader << nLeaderLevel << nLeaderJob << dwLSex;
	ar << idMember << nMemberLevel << nMemberJob << dwMSex;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::OnShout( CAr & ar, DPID, DPID, OBJID )
{
	char	lpString[1024];
	u_long idPlayer;
	ar >> idPlayer;
	ar.ReadString( lpString, 1024 );

#if __VER >= 11 // __SYS_PLAYER_DATA
	const char*	lpszPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( idPlayer );
#else	// __SYS_PLAYER_DATA
	LPCSTR lpszPlayer	= prj.GetPlayerString( idPlayer );
#endif	// __SYS_PLAYER_DATA
	if( lpszPlayer )
	{
		CAr ar1;
		ar1 << NULL_ID << SNAPSHOTTYPE_SHOUT;
		ar1 << NULL_ID;	//GETID( pUser );
		ar1.WriteString( lpszPlayer );
		ar1.WriteString( lpString );
#if __VER >= 12 // __LORD
		ar1 << (DWORD)0xffff99cc;
#endif	// __LORD
		GETBLOCK( ar1, lpBlock, uBlockSize );
		g_UserMng.AddBlock( lpBlock, uBlockSize );
	}
}

void CDPCoreClient::OnPlayMusic( CAr & ar, DPID, DPID, OBJID )
{
	DWORD dwWorldID;
	u_long idmusic;

	ar >> idmusic;
	ar >> dwWorldID;

	CWorld* pWorld	= g_WorldMng.GetWorld( dwWorldID );

	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_PLAYMUSIC << idmusic;
	GETBLOCK( arBlock, lpBlock, uBlockSize );
	g_UserMng.AddBlock( lpBlock, uBlockSize, pWorld );
}

void CDPCoreClient::OnPlaySound( CAr & ar, DPID, DPID, OBJID )
{
	DWORD dwWorldID;
	u_long idsound;

	ar >> idsound;
	ar >> dwWorldID;

	CWorld* pWorld	= g_WorldMng.GetWorld( dwWorldID );
	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_PLAYSOUND;
	arBlock << (BYTE)0;
	arBlock << idsound;
	GETBLOCK( arBlock, lpBlock, uBlockSize );
	g_UserMng.AddBlock( lpBlock, uBlockSize, pWorld );
}

void CDPCoreClient::OnErrorParty( CAr & ar, DPID, DPID, OBJID )
{
	DWORD dw;
	u_long uLeader;
	ar >> dw;
	ar >> uLeader;

	CUser* pUser	= (CUser*)prj.GetUserByID( uLeader );
	if( IsValidObj( (CObj*)pUser ) )
		pUser->AddSendErrorParty( dw );		
}

void CDPCoreClient::OnAddPartyMember( CAr & ar, DPID, DPID, OBJID )
{
	u_long	idParty, idLeader, idMember;
	LONG nLeaderLevel, nMemberLevel, nLeaderJob, nMemberJob;
	BYTE nLeaderSex, nMemberSex;
	
	ar >> idParty;
	ar >> idLeader >> nLeaderLevel >> nLeaderJob >> nLeaderSex;
	ar >> idMember >> nMemberLevel >> nMemberJob >> nMemberSex;

#if __VER >= 11 // __SYS_PLAYER_DATA
	char pszLeader[MAX_PLAYER]	= { 0,};
	char pszMember[MAX_PLAYER]	= { 0,};
	const char* lpPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( idLeader );
	if( lpPlayer )
		lstrcpy( pszLeader, lpPlayer );
	lpPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( idMember );
	if( lpPlayer )
		lstrcpy( pszMember, lpPlayer );
#endif	// __SYS_PLAYER_DATA
		
	CParty* pParty;

	pParty	= g_PartyMng.GetParty( idParty );
	if( pParty )
	{
#if __VER >= 11 // __SYS_PLAYER_DATA
		if( pParty->NewMember( idMember ) )
#else	// __SYS_PLAYER_DATA
		if( TRUE == pParty->NewMember( idMember, nMemberLevel, nMemberJob, nMemberSex, (char*)prj.GetPlayerString( idMember ) ) )
#endif	// __SYS_PLAYER_DATA
		{
			CUser* pMember	= NULL;
			for( int i = 0; i < pParty->m_nSizeofMember; i++ )
			{
				pMember		= (CUser*)prj.GetUserByID( pParty->m_aMember[i].m_uPlayerId );
				if( IsValidObj( (CObj*)pMember ) )
#if __VER >= 11 // __SYS_PLAYER_DATA
					pMember->AddPartyMember( pParty, idMember, pszLeader, pszMember );
#else	// __SYS_PLAYER_DATA
					pMember->AddPartyMember( pParty, idMember );
#endif	// __SYS_PLAYER_DATA
			}
			if( IsValidObj( (CObj*)pMember ) )
			{
				pMember->m_idparty	= idParty;
			
#ifdef __COLOSSEUM
				CColosseumMatch* ColoMatch = CColosseumMng::GetInstance()->GetMatchByParty( pParty );
				if( ColoMatch && ColoMatch->m_bWaitTime )
					if( ColoMatch->m_byMenu == 2 )
					{
						if( ColoMatch->m_uGuildID == pMember->m_idGuild )
							pMember->SendColosseumEnter();
					}
					else
						pMember->SendColosseumEnter();
#endif //__COLOSSEUM
			}
		}
		else
		{
			// error
		}
	}
	else
	{
#if __VER >= 11 // __SYS_PLAYER_DATA
		if( idParty == g_PartyMng.NewParty( idLeader, nLeaderLevel, nLeaderJob, nLeaderSex, (char*)CPlayerDataCenter::GetInstance()->GetPlayerString( idLeader ), idMember, nMemberLevel, nMemberJob, nMemberSex, (char*)CPlayerDataCenter::GetInstance()->GetPlayerString( idMember ), idParty ) )
#else	// __SYS_PLAYER_DATA
		if( idParty == g_PartyMng.NewParty( idLeader, nLeaderLevel, nLeaderJob, nLeaderSex, (char*)prj.GetPlayerString( idLeader ), idMember, nMemberLevel, nMemberJob, nMemberSex, (char*)prj.GetPlayerString( idMember ), idParty ) )
#endif	// __SYS_PLAYER_DATA
		{
			pParty	= g_PartyMng.GetParty( idParty );
			if( pParty )
			{
				CUser* pMember;
				for( int i = 0; i < pParty->m_nSizeofMember; i++ )
				{
					pMember	= g_UserMng.GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
					if( IsValidObj( (CObj*)pMember ) )
					{
						pMember->m_idparty	= idParty;
#if __VER >= 11 // __SYS_PLAYER_DATA
						pMember->AddPartyMember( pParty, idMember, pszLeader, pszMember );
#else	// __SYS_PLAYER_DATA
						pMember->AddPartyMember( pParty, idMember );
#endif	// __SYS_PLAYER_DATA
					}
				}
			}
			else
			{
				// error
			}
		}
		else
		{
			// error
		}
	}
}

void CDPCoreClient::OnRemovePartyMember( CAr & ar, DPID, DPID, OBJID )
{
	u_long idParty, idLeader, idMember;
	ar >> idParty >> idLeader >> idMember;

	CParty* pParty;
	pParty	= g_PartyMng.GetParty( idParty );
	if( pParty )
	{
#if __VER >= 11 // __SYS_PLAYER_DATA
		char pszLeader[MAX_PLAYER]	= { 0,};
		char pszMember[MAX_PLAYER]	= { 0,};
		const char* lpPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( idLeader );
		if( lpPlayer )
			lstrcpy( pszLeader, lpPlayer );
		lpPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( idMember );
		if( lpPlayer )
			lstrcpy( pszMember, lpPlayer );
#endif	// __SYS_PLAYER_DATA

#ifdef __COLOSSEUM
		CColosseumMatch* pColoMatch = CColosseumMng::GetInstance()->GetMatchByParty( pParty );
		if( pColoMatch )
			pColoMatch->KickUser( idMember );
#endif // __COLOSSEUM

		if( pParty->DeleteMember( idMember ) )
		{
			if( pParty->GetSizeofMember() < 2 )
			{
				CUser* pMember	= g_UserMng.GetUserByPlayerID( pParty->GetPlayerId( 0 ) );
				if( IsValidObj( pMember ) )
				{
#if __VER >= 11 // __SYS_PLAYER_DATA
					pMember->AddPartyMember( NULL, 0, pszLeader, pszMember );
#else	// __SYS_PLAYER_DATA
					pMember->AddPartyMember( NULL );
#endif	// __SYS_PLAYER_DATA
					pMember->m_idparty	= 0;
				}
				pMember	= g_UserMng.GetUserByPlayerID( idMember );	// 먼저 삭제됐던 넘의 포인터.
				if( IsValidObj( pMember ) && pMember->m_nDuel )
				{
					CParty* pDuelParty = g_PartyMng.GetParty( pParty->m_idDuelParty );
					if( pDuelParty )
						pDuelParty->DoDuelPartyCancel( pParty );
					else
						Error( "CDPCoreClient::OnRemovePartyMember : 파티멤버 %s의 정보이상. %d %d", pMember->GetName(), pMember->m_idDuelParty, pParty->m_idDuelParty );
				}
				g_PartyMng.DeleteParty( pParty->m_uPartyId );
			}
			else
			{
				// leave
				CUser* pMember;
				for( int i = 0; i < pParty->m_nSizeofMember; i++ )
				{
					pMember		= g_UserMng.GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
					if( IsValidObj( (CObj*)pMember ) )
#if __VER >= 11 // __SYS_PLAYER_DATA
						pMember->AddPartyMember( pParty, idMember, pszLeader, pszMember );
#else	// __SYS_PLAYER_DATA
						pMember->AddPartyMember( pParty, idMember );
#endif	// __SYS_PLAYER_DATA
				}
			}

			CUser* pRemovd	= g_UserMng.GetUserByPlayerID( idMember ); 
			if( IsValidObj( (CObj*)pRemovd ) )
			{
				pRemovd->m_idparty	= 0;
#if __VER >= 11 // __SYS_PLAYER_DATA
				pRemovd->AddPartyMember( NULL, idMember, pszLeader, pszMember );
#else	// __SYS_PLAYER_DATA
				pRemovd->AddPartyMember( NULL, idMember );
#endif	// __SYS_PLAYER_DATA
			}
		}
		else
		{
			Error( "OnRemovePartyMember:: Not DeleteMember" );
		}
	}
	else
	{
		Error( "OnRemovePartyMember:: Not Party" );
	}
}

void CDPCoreClient::OnAddPlayerParty( CAr & ar, DPID, DPID, OBJID )
{
	u_long idParty, idPlayer;
	ar >> idParty >> idPlayer;

	CParty* pParty;

	pParty	= g_PartyMng.GetParty( idParty );
	if( pParty )
	{
		int i	= pParty->FindMember( idPlayer );
		if( i < 0 )
			return;	//

		pParty->m_aMember[i].m_bRemove	= FALSE;

		CUser* pMember;
		for( int j = 0 ; j < pParty->m_nSizeofMember ; j++ )
		{
			pMember		= g_UserMng.GetUserByPlayerID( pParty->GetPlayerId( j ) );
			if( IsValidObj( (CObj*)pMember ) )
				pMember->AddSetPartyMemberParam( idPlayer, PP_REMOVE, 0 );
		}
	}
}

void CDPCoreClient::OnRemovePlayerParty( CAr & ar, DPID, DPID, OBJID )
{
	u_long idParty, idPlayer;
	ar >> idParty >> idPlayer;

	CParty* pParty;

	pParty	= g_PartyMng.GetParty( idParty );
	if( pParty )
	{
		int i	= pParty->FindMember( idPlayer );
		if( i < 0 )
			return;

		pParty->m_aMember[i].m_bRemove	= TRUE;

		CUser* pMember;
		for( int j = 0; j < pParty->m_nSizeofMember; j++ )
		{
			pMember		= g_UserMng.GetUserByPlayerID( pParty->GetPlayerId( j ) );
			if( IsValidObj( (CObj*)pMember ) )
				pMember->AddSetPartyMemberParam( idPlayer,PP_REMOVE, 1 );
		}

		if( i == 0 )		// 극단장이 나갈경우
		{
			if( pParty->m_idDuelParty > 0 )
			{
				CParty* pDuelParty	= g_PartyMng.GetParty( pParty->m_idDuelParty );
				if( pDuelParty )
					pParty->DoDuelPartyCancel( pDuelParty );
			}

			bool fRemoveParty	= true;
			for( j = 1; j < pParty->m_nSizeofMember; j++ )
			{
				if( pParty->m_aMember[j].m_bRemove == FALSE )
				{
					fRemoveParty	= false;
					pParty->SwapPartyMember( 0, j );
					// swap
					break;
				}
			}
#if __VER >= 12 // __PARSKILL1001	//12차 파스킬 아이템 수정  world,core,neuz
			for( int k = 0 ; k < MAX_PARTYMODE ; k++ )
			{
				if( pParty->m_nModeTime[k] )
				{
					if( k == PARTY_PARSKILL_MODE)
						continue;
					pParty->m_nModeTime[k] = 0;
				}
			}

			CMover* pMover = pParty->GetLeader();
			if( pMover )
			{
				if( pMover->IsSMMode( SM_PARTYSKILL1 ) || pMover->IsSMMode( SM_PARTYSKILL15 ) || pMover->IsSMMode( SM_PARTYSKILL30 ) )
					g_DPCoreClient.SendUserPartySkill( pMover->m_idPlayer, PARTY_PARSKILL_MODE, 1000, 0, 1 );
				else
					g_DPCoreClient.SendUserPartySkill( pMover->m_idPlayer, PARTY_PARSKILL_MODE, 0, 0, 1 );
			}
#endif //__PARSKILL1001	//12차 파스킬 아이템 수정  world,core,neuz
			if( fRemoveParty )
			{
				CUser* pMember;
				for( int j = 0; j < pParty->m_nSizeofMember; j++ )
				{
					pMember		= g_UserMng.GetUserByPlayerID( pParty->GetPlayerId( j ) );
					if( IsValidObj( (CObj*)pMember ) )
						pMember->m_idparty = 0;
				}
				g_PartyMng.DeleteParty( pParty->m_uPartyId );
			}
		}
	}
}

#if __VER < 11 // __SYS_PLAYER_DATA
void CDPCoreClient::OnPartyMemberLevel( CAr & ar, DPID, DPID, OBJID )
{
	u_long idParty, idPlayer;
	int nLevel;
	ar >> idParty >> idPlayer >> nLevel;
	
	CParty* pParty = g_PartyMng.GetParty( idParty );
	if( pParty )
	{
		int findIndex = pParty->FindMember( idPlayer );
		if( findIndex < 0 )
			return;	//

		pParty->m_aMember[findIndex].m_nLevel = nLevel;

		for( int i = 0 ; i < pParty->m_nSizeofMember ; i++ )
		{
			CUser* pUser = g_UserMng.GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
			if( IsValidObj( (CObj*)pUser ) )
			{
//				pUser->m_idparty = pParty->m_uPartyId;
				pUser->AddPartyMemberLevel( findIndex, nLevel );
			}
		}
	}
}

void CDPCoreClient::OnPartyMemberJob( CAr & ar, DPID, DPID, OBJID )
{
	u_long idParty, idPlayer;
	int nJob;
	ar >> idParty >> idPlayer >> nJob;
	
	CParty* pParty = g_PartyMng.GetParty( idParty );
	if( pParty )
	{
		int findIndex = pParty->FindMember( idPlayer );
		if( findIndex < 0 )
			return;	//

		pParty->m_aMember[findIndex].m_nJob = nJob;

		for( int i = 0 ; i < pParty->m_nSizeofMember ; i++ )
		{
			CUser* pUser = g_UserMng.GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
			if( IsValidObj( (CObj*)pUser ) )
			{
//				pUser->m_idparty = pParty->m_uPartyId;
				pUser->AddPartyMemberJob( findIndex, nJob );
			}
		}
	}
}
#endif	// __SYS_PLAYER_DATA

void CDPCoreClient::OnSetPartyMode( CAr & ar, DPID, DPID, OBJID )
{
	u_long uPartyId;
	int nMode;
	BOOL bOnOfff;
	LONG nPoint;
#if __VER >= 12 // __JHMA_VER12_1	//12차 극단유료아이템
	DWORD	dwSkillTime;
	ar >> uPartyId >> nMode >> dwSkillTime >> bOnOfff;
#else//12차 극단유료아이템
	ar >> uPartyId >> nMode >> bOnOfff;
#endif // //12차 극단유료아이템
	if( bOnOfff == TRUE )
		ar >> nPoint;

	CParty* pParty = g_PartyMng.GetParty( uPartyId );
	if( pParty )
	{
		if( bOnOfff == TRUE )
			pParty->m_nPoint = nPoint;
#if __VER >= 12 // 12차 극단유료아이템
		// 090917 mirchang - 모드가 파스킬풀 이고 bOnOfff가 FALSE 일땐 극단원중 파스킬풀 아이템 사용중인지 체크하여 사용중인 극단원이 있을땐 다시 모드 설정
		if( nMode == PARTY_PARSKILL_MODE && !bOnOfff )
		{
			for( int j = 0; j < pParty->m_nSizeofMember; ++j )
			{
				if( pParty->m_aMember[j].m_bRemove )
					continue;
				CUser* pUser = g_UserMng.GetUserByPlayerID( pParty->m_aMember[j].m_uPlayerId );
				if( IsValidObj( pUser ) && ( pUser->IsSMMode( SM_PARTYSKILL30 ) || pUser->IsSMMode( SM_PARTYSKILL15 ) || pUser->IsSMMode( SM_PARTYSKILL1 ) ) )
				{
					bOnOfff = TRUE;
					g_DPCoreClient.SendUserPartySkill( pUser->m_idPlayer, PARTY_PARSKILL_MODE, 1000, 0, 1 );
					break;
				}
			}
		}
#endif // 12차 극단유료아이템
		pParty->m_nModeTime[nMode] = bOnOfff;
		
		for( int i = 0 ; i < pParty->m_nSizeofMember ; i++ )
		{
			CUser* pUser = g_UserMng.GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
			if( IsValidObj( (CObj*)pUser ) )
#if __VER >= 12 // __JHMA_VER12_1	//12차 극단유료아이템
				pUser->AddSetPartyMode( nMode, bOnOfff, pParty->m_nPoint , dwSkillTime );
#else // //12차 극단유료아이템
				pUser->AddSetPartyMode( nMode, bOnOfff, pParty->m_nPoint );
#endif // //12차 극단유료아이템
		}
	}
}

void CDPCoreClient::OnPartyChangeItemMode( CAr & ar, DPID, DPID, OBJID )
{
	u_long uPartyId;
	int nMode;
	ar >> uPartyId >> nMode;
	
	CParty* pParty = g_PartyMng.GetParty( uPartyId );
	if( pParty )
	{
		pParty->m_nTroupeShareItem = nMode;

		for( int i = 0 ; i < pParty->m_nSizeofMember ; i++ )
		{
			CUser* pUser = g_UserMng.GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
			if( IsValidObj( (CObj*)pUser ) )
				pUser->AddPartyChangeItemMode( nMode );
		}
	}
}

void CDPCoreClient::OnPartyChangeExpMode( CAr & ar, DPID, DPID, OBJID )
{
	u_long uPartyId;
	int nMode;
	ar >> uPartyId >> nMode;
	
	CParty* pParty = g_PartyMng.GetParty( uPartyId );
	if( pParty )
	{
		pParty->m_nTroupsShareExp = nMode;
		
		for( int i = 0 ; i < pParty->m_nSizeofMember ; i++ )
		{
			CUser* pUser = g_UserMng.GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
			if( IsValidObj( (CObj*)pUser ) )
				pUser->AddPartyChangeExpMode( nMode );
		}
	}
}


void CDPCoreClient::OnSetPartyExp( CAr & ar, DPID, DPID, OBJID )
{
	u_long uPartyId;
	LONG nExp, nPoint, nLevel;
	ar >> uPartyId >> nExp >> nPoint >> nLevel;

	CParty* pParty	= g_PartyMng.GetParty( uPartyId );
	if( pParty )
	{
		pParty->m_nExp	= nExp;
		pParty->m_nPoint	= nPoint;
		pParty->m_nLevel	= nLevel;
		for( int i = 0; i < pParty->m_nSizeofMember; i++ )
		{
			CUser* pUser	= g_UserMng.GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
			if( IsValidObj( (CObj*)pUser ) )
				pUser->AddPartyExpLevel( pParty->GetExp(), pParty->GetLevel(), pParty->GetPoint() );
		}
	}
	else
	{
		// error
	}
}

void CDPCoreClient::OnRemovePartyPoint( CAr & ar, DPID, DPID, OBJID )
{
	u_long uPartyId;
	LONG nPartyPoint;
	ar >> uPartyId;
	ar >> nPartyPoint;
	
	CParty* pParty = g_PartyMng.GetParty( uPartyId );
	if( pParty )
	{
		pParty->m_nPoint = nPartyPoint;
	
		for( int i = 0 ; i < pParty->m_nSizeofMember ; i++ )
		{
			CUser* pUser = g_UserMng.GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
			if( IsValidObj( (CObj*)pUser ) )
				pUser->AddPartyExpLevel( pParty->GetExp(), pParty->GetLevel(), pParty->GetPoint() );
		}
	}
	else
	{
		// error
	}
}

void CDPCoreClient::OnPartyChangeName( CAr & ar, DPID, DPID, OBJID )
{
	u_long uidParty;
	TCHAR	sParty[128];

	ar >> uidParty;	
	ar.ReadString( sParty, 128 );
	
	CParty* pParty;

	pParty	= g_PartyMng.GetParty( uidParty );
	if( pParty )
	{
		strcpy( pParty->m_sParty, sParty );
		for( int i = 0 ; i < pParty->m_nSizeofMember ; i++ )
		{
			CUser* pUser = g_UserMng.GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
			if( IsValidObj( (CObj*)pUser ) )
				pUser->AddPartyChangeName( pParty->m_sParty );
		}
	}
}

void CDPCoreClient::OnPartyChangeTroup( CAr & ar, DPID, DPID, OBJID )
{
	u_long uidParty;
	TCHAR	sParty[33];		
	
	ar >> uidParty;	
	ar.ReadString( sParty, 33 );
		
	CParty* pParty;

	pParty	= g_PartyMng.GetParty( uidParty );
	if( pParty )
	{
		pParty->m_nKindTroup = 1;
		strcpy( pParty->m_sParty, sParty );
		for( int i = 0 ; i < pParty->m_nSizeofMember ; i++ )
		{
			CUser* pUser = g_UserMng.GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
			if( IsValidObj( (CObj*)pUser ) )
				pUser->AddPartyChangeTroup( pParty->m_sParty );
		}
/*
#if __VER >= 12 // __PARSKILL1001	//12차 파스킬 아이템 수정  world,core,neuz
		CMover* pMover = pParty->GetLeader();
		if( pMover )
		{
			if( pMover->IsSMMode( SM_PARTYSKILL1 ) || pMover->IsSMMode( SM_PARTYSKILL15 ) || pMover->IsSMMode( SM_PARTYSKILL30 ) )
                g_DPCoreClient.SendUserPartySkill( pMover->m_idPlayer, PARTY_PARSKILL_MODE, 1000, 0, 1 );
		}
#endif //__PARSKILL1001	//12차 파스킬 아이템 수정  world,core,neuz
*/
	}
}

void CDPCoreClient::OnAddFriend( CAr & ar, DPID, DPID, OBJID )
{
	int bAdd = 0; // 0이면 추가를 아무도 안한것 1 Sender만 추가, 2 Friend만 추가, 3 : 두명다 추가됨
	u_long uidSend, uidFriend;
	BYTE nSendSex, nFriendSex;
	LONG nSendJob, nFriendJob;
	ar >> uidSend >> uidFriend;
	ar >> nSendSex >> nFriendSex;
	ar >> nSendJob >> nFriendJob;

	CUser* pSender;
	CUser* pFriend;
	
	pSender = g_UserMng.GetUserByPlayerID( uidSend );
	pFriend = g_UserMng.GetUserByPlayerID( uidFriend );
	
#if __VER >= 11 // __SYS_PLAYER_DATA
	const char* lpszFriendPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( uidFriend );
#else	// __SYS_PLAYER_DATA
	const char* lpszFriendPlayer	= prj.GetPlayerString( uidFriend );
#endif	// __SYS_PLAYER_DATA
	if( lpszFriendPlayer == NULL )	//
		return;
	char lpszFriend[MAX_PLAYER];
	strcpy( lpszFriend, lpszFriendPlayer );

	if( IsValidObj( (CObj*)pSender ) )
	{
#ifdef __RT_1025
		if( MAX_FRIEND <= pSender->m_RTMessenger.size() )
#else	// __RT_1025
		if( MAX_FRIEND <= pSender->m_Messenger.GetSize() )
#endif	// __RT_1025
		{
			pSender->AddDefinedText( TID_GAME_MSGMAXUSER, "" );
		}
		else
		{
#ifdef __RT_1025
			pSender->m_RTMessenger.SetFriend( uidFriend, NULL );
			pSender->AddAddFriend( uidFriend, lpszFriend ); 
			bAdd++;
#else	// __RT_1025
			if( pSender->m_Messenger.AddFriend( uidFriend, nFriendJob, nFriendSex ) )
			{
				pSender->m_Messenger.AddDefferntFriend( uidFriend );	
				pSender->AddAddFriend( uidFriend, nFriendJob, nFriendSex, lpszFriend );
				bAdd += 1;
			}
			else
			{
				// 
			}
#endif	// __RT_1025
		}
	}

#if __VER >= 11 // __SYS_PLAYER_DATA
	const char* lpszSendPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( uidSend );
#else	// __SYS_PLAYER_DATA
	const char* lpszSendPlayer	= prj.GetPlayerString( uidSend );
#endif	// __SYS_PLAYER_DATA
	if( lpszSendPlayer == NULL )
		return;
	char lpszSend[MAX_PLAYER];
	strcpy( lpszSend, lpszSendPlayer );

	if( IsValidObj( (CObj*)pFriend ) )
	{
#ifdef __RT_1025
		if( MAX_FRIEND <= pFriend->m_RTMessenger.size() )
#else	// __RT_1025
		if( MAX_FRIEND <= pFriend->m_Messenger.GetSize() )
#endif	// __RT_1025
		{
			if( IsValidObj( (CObj*)pSender ) )
				pSender->AddDefinedText( TID_GAME_MSGMAXUSER, "" );
		}
		else
		{
#ifdef __RT_1025
			pFriend->m_RTMessenger.SetFriend( uidSend, NULL );
			pFriend->AddAddFriend( uidSend, lpszSend );
			bAdd	+= 2;
#else	//__RT_1025
			if( pFriend->m_Messenger.AddFriend( uidSend, nSendJob, nSendSex ) )
			{
				pFriend->m_Messenger.AddDefferntFriend( uidSend );
				pFriend->AddAddFriend( uidSend, nSendJob, nSendSex, lpszSend );
				bAdd += 2;
			}
			else
			{
			}
#endif	// __RT_1025
		}
	}

	if( IsValidObj( (CObj*)pSender ) && strlen( lpszFriend ) && ( bAdd == 2 || bAdd == 3 ) )
		pSender->AddDefinedText( TID_GAME_MSGINVATECOM, "%s", lpszFriend );

	if( IsValidObj( (CObj*)pFriend ) && strlen( lpszSend ) && ( bAdd == 1 || bAdd == 3 )  )
		pFriend->AddDefinedText( TID_GAME_MSGINVATECOM, "%s", lpszSend );
}

void CDPCoreClient::OnRemovefriend( CAr & ar, DPID, DPID, OBJID )
{
	u_long uidSend, uidFriend;
	ar >> uidSend >> uidFriend;
	
	CUser* pSender;

	pSender		= (CUser*)prj.GetUserByID( uidSend );
	if( FALSE == IsValidObj( (CObj*)pSender ) )
		return;

#ifdef __RT_1025
#if __VER >= 13 // __HOUSING
	CHousingMng::GetInstance()->ReqSetAllowVisit( pSender, uidFriend, FALSE );
#endif //__HOUSING
	pSender->m_RTMessenger.RemoveFriend( uidFriend );
	pSender->AddRemoveFriend( uidFriend );
	CUser* pFriend	= g_UserMng.GetUserByPlayerID( uidFriend );
	if( IsValidObj( pFriend ) )
	{
#if __VER >= 13 // __HOUSING
		CHousingMng::GetInstance()->ReqSetAllowVisit( pFriend, uidSend, FALSE );
#endif //__HOUSING
		pFriend->m_RTMessenger.RemoveFriend( uidSend );
		pFriend->AddRemoveFriend( uidSend );
	}
#else	// __RT_1025
	if( pSender->m_Messenger.RemoveFriend( uidFriend ) )
	{
		pSender->AddRemoveFriend( uidFriend );

		CUser* pFriend	= g_UserMng.GetUserByPlayerID( uidFriend );
		if( IsValidObj( (CObj*)pFriend ) )
			pFriend->m_Messenger.RemoveDifferntFriend( uidSend );
	}
#endif	// __RT_1025
}

void CDPCoreClient::OnQueryTickCount( CAr & ar, DPID, DPID, OBJID )
{
	DWORD dwTime;
	__int64 nTickCount;
	double dCurrentTime;
	ar >> dwTime >> nTickCount >> dCurrentTime;

	dwTime	= timeGetTime() - dwTime;
	nTickCount	+= ( dwTime * 5000 );	// ( dwTime / 2 ) * 10000

	g_TickCount.Start( nTickCount );
	g_GameTimer.SetCurrentTime( dCurrentTime );
}

#ifdef __ENVIRONMENT_EFFECT

void CDPCoreClient::OnEnvironmentEffect( CAr & ar, DPID, DPID, OBJID )
{
	CEnvironment::GetInstance()->Serialize( ar );

	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_ENVIRONMENT;
	CEnvironment::GetInstance()->Serialize( arBlock );

	if( CEnvironment::GetInstance()->GetEnvironmentEffect() == TRUE )
	{
		arBlock.WriteString( prj.m_EventLua.GetWeatherEventTitle().c_str() );
	}

	GETBLOCK( arBlock, lpBlock, uBlockSize );
	g_UserMng.AddBlock( lpBlock, uBlockSize );
}

#else // __ENVIRONMENT_EFFECT

void CDPCoreClient::OnEnvironmentSnow( CAr & ar, DPID, DPID, OBJID )
{
	ar >> g_Environment.m_bSnow;

	if( g_Environment.m_bSnow )
	{
		g_Environment.m_bRain = FALSE;
	}
	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_ENVIRONMENTSNOW;
	arBlock << g_Environment.m_bSnow;
#ifdef __EVENTLUA_SNOW
	arBlock.WriteString( prj.m_EventLua.GetSnowEventTitle().c_str() );
#endif // __EVENTLUA_SNOW
	GETBLOCK( arBlock, lpBlock, uBlockSize );
	g_UserMng.AddBlock( lpBlock, uBlockSize );
}

void CDPCoreClient::OnEnvironmentRain( CAr & ar, DPID, DPID, OBJID )
{
	ar >> g_Environment.m_bRain;

	if( g_Environment.m_bRain )
		g_Environment.m_bSnow = FALSE;

	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_ENVIRONMENTRAIN;
	arBlock << g_Environment.m_bRain;
#ifdef __EVENTLUA_RAIN
	arBlock.WriteString( prj.m_EventLua.GetRainEventTitle().c_str() );
#endif // __EVENTLUA_RAIN
	GETBLOCK( arBlock, lpBlock, uBlockSize );
	g_UserMng.AddBlock( lpBlock, uBlockSize );
}

void CDPCoreClient::OnEnvironmentAll( CAr & ar, DPID, DPID, OBJID )
{
	ar >> g_Environment.m_bRain;
	ar >> g_Environment.m_bSnow;
	
	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_ENVIRONMENTALL;
	arBlock << g_Environment.m_bRain;
	arBlock << g_Environment.m_bSnow;
#ifdef __EVENTLUA_RAIN
	arBlock.WriteString( prj.m_EventLua.GetRainEventTitle().c_str() );
#endif // __EVENTLUA_RAIN
#ifdef __EVENTLUA_SNOW
	arBlock.WriteString( prj.m_EventLua.GetSnowEventTitle().c_str() );
#endif // __EVENTLUA_SNOW
	GETBLOCK( arBlock, lpBlock, uBlockSize );
	g_UserMng.AddBlock( lpBlock, uBlockSize );
}

#endif // __ENVIRONMENT_EFFECT

void CDPCoreClient::OnPartyChat( CAr & ar , DPID, DPID, OBJID )
{
	u_long idParty;
static	\
	CHAR	lpName[MAX_PLAYER], lpString[260], lpSnoopString[512];
	
	OBJID objid;
	ar >> objid;
	ar >> idParty;
	ar.ReadString( lpName, MAX_PLAYER );
	ar.ReadString( lpString, 260 );

	CParty* pParty;
	
	pParty	= g_PartyMng.GetParty( idParty );
	if( pParty )
	{
		CUser* pMember	= NULL;
		for( int i = 0; i < pParty->m_nSizeofMember; i++ )
		{
			pMember		= (CUser*)prj.GetUserByID( pParty->m_aMember[i].m_uPlayerId );
			if( IsValidObj( (CObj*)pMember ) )
			{
				pMember->AddPartyChat( lpName, lpString, objid );
				if( pMember->m_idSnoop )
				{
					CUser* pSnoop		= g_UserMng.GetUserByPlayerID( pMember->m_idSnoop );
					if( IsValidObj( (CObj*)pSnoop ) )
					{
						sprintf( lpSnoopString, "[p][%s->%s] %s", lpName, pMember->GetName(), lpString );
						pSnoop->AddSnoop( lpSnoopString );
					}
				}
			}
		}
	}
}

void CDPCoreClient::SendAddFriendNameReqest( u_long uLeaderid, LONG nLeaderJob, BYTE nLeaderSex, u_long uMember, const char * szLeaderName, const char * szMemberName )
{
	BEFORESENDDUAL( ar, PACKETTYPE_ADDFRIENDNAMEREQEST, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uLeaderid << uMember;
	ar << nLeaderJob << nLeaderSex;
	ar.WriteString( szLeaderName );
	ar.WriteString( szMemberName );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendCreateGuild( GUILD_MEMBER_INFO* info, int nSize, const char* szGuild )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CREATE_GUILD, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nSize;
	ar.Write( info, sizeof(GUILD_MEMBER_INFO)*nSize );
	ar.WriteString( szGuild );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendGuildChat( CUser* pUser, const char* sChat )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILD_CHAT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << pUser->GetId();
	ar << pUser->m_idPlayer;
	ar.WriteString( sChat );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::OnCreateGuild( CAr & ar, DPID, DPID, OBJID )
{
	int nSize;
	GUILD_MEMBER_INFO	info[MAX_PTMEMBER_SIZE];
	u_long idGuild;
	char szGuild[MAX_G_NAME];

	ar >> nSize;
	ar.Read( info, sizeof(GUILD_MEMBER_INFO)*nSize );
	ar >> idGuild;
	ar.ReadString( szGuild, MAX_G_NAME );

	int	cb	= 0;
	CUser* apUser[MAX_PTMEMBER_SIZE];

	CGuild* pGuild	= new CGuild;
	pGuild->m_idGuild	= idGuild;
	lstrcpy( pGuild->m_szGuild, szGuild );
	pGuild->m_idMaster		= info[0].idPlayer;
	pGuild->m_adwPower[ GUD_MASTER ] = 0x000000FF;
	idGuild	= g_GuildMng.AddGuild( pGuild );
	if( idGuild > 0 )
	{
		// master
		CGuildMember* pMember	= new CGuildMember;
		pMember->m_idPlayer	= info[0].idPlayer;
		pMember->m_nMemberLv	= GUD_MASTER;
#if __VER < 11 // __SYS_PLAYER_DATA
		pMember->m_nLevel	= info[0].nLevel;
		pMember->m_nJob		= info[0].nJob;
		pMember->m_dwSex	= info[0].dwSex;
		pMember->m_nMultiNo	= info[0].nMultiNo;
		pMember->m_nLogin	= 1;
#endif	// __SYS_PLAYER_DATA
		if( pGuild->AddMember( pMember ) )
		{
			CUser* pUser	= (CUser*)prj.GetUserByID( info[0].idPlayer );
			if( IsValidObj( pUser ) )
			{
				pUser->m_idGuild	= idGuild;
				apUser[cb++]	= pUser;
			}
			// member
			for( int i = 1; i < nSize; i++ )
			{
				pMember		= new CGuildMember;
				pMember->m_idPlayer		= info[i].idPlayer;
				pMember->m_nMemberLv	= GUD_ROOKIE;
#if __VER < 11 // __SYS_PLAYER_DATA
				pMember->m_nLevel	= info[i].nLevel;
				pMember->m_nJob		= info[i].nJob;
				pMember->m_dwSex	= info[i].dwSex;
				pMember->m_nMultiNo	= info[i].nMultiNo;
				pMember->m_nLogin	= 1;
#endif	// __SYS_PLAYER_DATA
				if( pGuild->AddMember( pMember ) )
				{
					pUser	= (CUser*)prj.GetUserByID( info[i].idPlayer );
					if( IsValidObj( pUser ) )
					{
						pUser->m_idGuild	= idGuild;
						apUser[cb++]	= pUser;
					}
				}
				else
				{
					info[i].idPlayer	= 0;
					SAFE_DELETE( pMember );
				}
			}
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

	if( cb > 0 )
	{
#if __VER >= 11 // __SYS_PLAYER_DATA
		g_UserMng.AddCreateGuild( info[0].idPlayer, CPlayerDataCenter::GetInstance()->GetPlayerString( info[0].idPlayer ), idGuild, szGuild );	// g_UserMng.Lock();
#else	// __SYS_PLAYER_DATA
		g_UserMng.AddCreateGuild( info[0].idPlayer, prj.GetPlayerString( info[0].idPlayer ), idGuild, szGuild );	// g_UserMng.Lock();
#endif	// __SYS_PLAYER_DATA
	}

	for( int i = 0; i < cb; i++ )
	{
		g_UserMng.AddSetGuild( apUser[i], idGuild );
		apUser[i]->AddMyGuild();
	}
}

void CDPCoreClient::OnDestroyGuild( CAr & ar, DPID, DPID, OBJID )
{
	u_long idGuild;
	ar >> idGuild;

	int nSize = 0;
	CUser* pUserIk3[ MAX_GM_SIZE ];

	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		CUser* pUsertmp;
		CGuildMember* pMember;
		for( map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin();
		i != pGuild->m_mapPMember.end(); ++i )
		{
			pMember		= i->second;

			pUsertmp	= (CUser*)prj.GetUserByID( pMember->m_idPlayer );
			if( IsValidObj( pUsertmp ) )
			{
#if __VER >= 15 // __GUILD_HOUSE
				pUsertmp->SetRestPoint( 0 );
				GuildHouseMng->ResetApplyDST( pUsertmp );
#endif // __GUILD_HOUSE
				pUsertmp->m_idGuild		= 0;
				g_UserMng.AddSetGuild( pUsertmp, 0 );
				pUserIk3[nSize] = pUsertmp;
				++nSize;				
			}
		}

#if __VER >= 11 // __SYS_PLAYER_DATA
		const char* lpszPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( pGuild->m_idMaster );
#else	// __SYS_PLAYER_DATA
		LPCSTR lpszPlayer	= prj.GetPlayerString( pGuild->m_idMaster );
#endif	// __SYS_PLAYER_DATA
#if __VER >= 12 // __SECRET_ROOM
		/*
		if( CSecretRoomMng::GetInstance()->m_nState == SRMNG_WAR )
		{
			CUser* pUserMaster = (CUser*)prj.GetUserByID( pGuild->m_idMaster );
			CSecretRoomMng::GetInstance()->SetFailGuild( pUserMaster );
		}
		*/
#endif // __SECRET_ROOM
		
		g_GuildMng.RemoveGuild( pGuild->m_idGuild );
		g_UserMng.AddDestroyGuild( lpszPlayer, idGuild );	// g_UserMng.Lock();

		for( int cou = 0 ; cou < nSize ; ++cou )
		{
			if( IsValidObj( pUserIk3[cou] ) )
			{
				pUserIk3[cou]->RemoveItemIK3( IK3_CLOAK );
				pUserIk3[cou]->m_tGuildMember = CTime::GetCurrentTime();
				pUserIk3[cou]->m_tGuildMember += CTimeSpan( 2, 0, 0, 0 );
				////////////////
				// 康
				if( pUserIk3[cou]->GetWorld() )
				{
					////
					PRegionElem pRgnElem	= NULL;
					DWORD dwWorldID	= pUserIk3[cou]->GetWorld()->GetID();
					CGuildQuestProcessor* pProcessor	= CGuildQuestProcessor::GetInstance();
					int nId	= pProcessor->PtInQuestRect( pUserIk3[cou]->GetPos() );
					if( nId > -1 )
					{
						PGUILDQUESTELEM pElem	= pProcessor->GetGuildQuest( nId );
						if( !pElem || pElem->idGuild != pUserIk3[cou]->m_idGuild )
						{
							CWorld* pWorld	= g_WorldMng.GetWorld( dwWorldID );
							if( pWorld )
							{
								if( pUserIk3[cou]->IsChaotic() )
								{
#if __VER >= 8 // __S8_PK
									if( pWorld->GetID() != pWorld->m_dwIdWorldRevival && pWorld->m_dwIdWorldRevival != 0 )
										pRgnElem	= g_WorldMng.GetRevivalPosChao( pWorld->m_dwIdWorldRevival, pWorld->m_szKeyRevival );
									if( !pRgnElem )	// Find near revival pos
										pRgnElem	= g_WorldMng.GetNearRevivalPosChao( pWorld->GetID(), pUserIk3[cou]->GetPos() );
#else // __VER >= 8 //__S8_PK
									pRgnElem	= g_WorldMng.GetNearRevivalPosChao( pWorld->GetID(), pUserIk3[cou]->GetPos() );
#endif // __VER >= 8 //__S8_PK
								}
								else
								{
									if( pWorld->GetID() != pWorld->m_dwIdWorldRevival && pWorld->m_dwIdWorldRevival != 0 )
										pRgnElem	= g_WorldMng.GetRevivalPos( pWorld->m_dwIdWorldRevival, pWorld->m_szKeyRevival );
									if( !pRgnElem )	// Find near revival pos
										pRgnElem	= g_WorldMng.GetNearRevivalPos( pWorld->GetID(), pUserIk3[cou]->GetPos() );
								}
							}
						}
					}
					////
					if( pRgnElem )
						pUserIk3[cou]->REPLACE( g_uIdofMulti, pRgnElem->m_dwWorldId, pRgnElem->m_vPos, REPLACE_NORMAL, nRevivalLayer );
				}
				////////////////
			}
		}	
		return;
	}
}

void CDPCoreClient::OnAddGuildMember( CAr & ar, DPID, DPID, OBJID )
{
	u_long idGuild;
//	ar >> idPlayer >> idGuild;
	GUILD_MEMBER_INFO info;
	ar.Read( &info, sizeof(GUILD_MEMBER_INFO) );
	ar >> idGuild;

	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		if( pGuild->GetSize() >= MAX_GM_SIZE )
			return;

		CGuildMember* pMember	= new CGuildMember;
		pMember->m_idPlayer	= info.idPlayer;
		pMember->m_nMemberLv	= GUD_ROOKIE;
#if __VER < 11 // __SYS_PLAYER_DATA
		pMember->m_nLevel	= info.nLevel;
		pMember->m_nJob		= info.nJob;
		pMember->m_dwSex	= info.dwSex;
		pMember->m_nMultiNo	= info.nMultiNo;
		pMember->m_nLogin	= 1;
#endif	// __SYS_PLAYER_DATA
		if( pGuild->AddMember( pMember ) )
		{
			CUser* pUser	= g_UserMng.GetUserByPlayerID( info.idPlayer );
			if( IsValidObj( pUser ) )
			{
				pUser->m_idGuild	= idGuild;
				g_UserMng.AddSetGuild( pUser, idGuild );
#if __VER >= 15 // __GUILD_HOUSE
				pUser->AddGuildHouseAllInfo( GuildHouseMng->GetGuildHouse( idGuild ) );
				GuildHouseMng->SetApplyDST( pUser );
#endif // __GUILD_HOUSE
			}
		}
		else
		{
			SAFE_DELETE( pMember );
		}
	}
}

void CDPCoreClient::OnRemoveGuildMember( CAr & ar, DPID, DPID, OBJID )
{
	u_long idPlayer, idGuild;
	ar >> idPlayer >> idGuild;

	BOOL bRemove = FALSE;

	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	CUser* pUser;
	if( pGuild )
	{
		if( pGuild->RemoveMember( idPlayer ) )
		{
			pUser	= (CUser*)prj.GetUserByID( idPlayer );
			if( IsValidObj(pUser) )
			{
#if __VER >= 15 // __GUILD_HOUSE
				pUser->SetRestPoint( 0 );
				GuildHouseMng->ResetApplyDST( pUser );
				GuildHouseMng->GoOutGuildHouse( pUser );
#endif // __GUILD_HOUSE
				pUser->m_idGuild	= 0;
				g_UserMng.AddSetGuild( pUser, 0 );
				bRemove = TRUE;
			}
		}
	}

	if( bRemove )
	{
		if( IsValidObj(pUser) )
		{
			pUser->RemoveItemIK3( IK3_CLOAK );
			pUser->m_tGuildMember = CTime::GetCurrentTime();
			pUser->m_tGuildMember += CTimeSpan( 2, 0, 0, 0 );
			// 康
			if( pUser->GetWorld() )
			{
				////
				PRegionElem pRgnElem	= NULL;
				DWORD dwWorldID	= pUser->GetWorld()->GetID();
				CGuildQuestProcessor* pProcessor	= CGuildQuestProcessor::GetInstance();
				int nId	= pProcessor->PtInQuestRect( pUser->GetPos() );
				if( nId > -1 )
				{
					PGUILDQUESTELEM pElem	= pProcessor->GetGuildQuest( nId );
					if( !pElem || pElem->idGuild != pUser->m_idGuild )
					{
						CWorld* pWorld	= g_WorldMng.GetWorld( dwWorldID );
						if( pWorld )
						{
							if( pUser->IsChaotic() )
							{
#if __VER >= 8 // __S8_PK
								if( pWorld->GetID() != pWorld->m_dwIdWorldRevival && pWorld->m_dwIdWorldRevival != 0 )
									pRgnElem	= g_WorldMng.GetRevivalPosChao( pWorld->m_dwIdWorldRevival, pWorld->m_szKeyRevival );
								if( !pRgnElem )	// Find near revival pos
									pRgnElem	= g_WorldMng.GetNearRevivalPosChao( pWorld->GetID(), pUser->GetPos() );
#else // __VER >= 8 __S8_PK
								pRgnElem	= g_WorldMng.GetNearRevivalPosChao( pWorld->GetID(), pUser->GetPos() );
#endif // __VER >= 8 __S8_PK
							}
							else
							{
								if( pWorld->GetID() != pWorld->m_dwIdWorldRevival && pWorld->m_dwIdWorldRevival != 0 )
									pRgnElem	= g_WorldMng.GetRevivalPos( pWorld->m_dwIdWorldRevival, pWorld->m_szKeyRevival );
								if( !pRgnElem )	// Find near revival pos
									pRgnElem	= g_WorldMng.GetNearRevivalPos( pWorld->GetID(), pUser->GetPos() );
							}
						}
					}
				}
				////
				if( pRgnElem )
					pUser->REPLACE( g_uIdofMulti, pRgnElem->m_dwWorldId, pRgnElem->m_vPos, REPLACE_NORMAL, nRevivalLayer );
			}
		}
	}
}

void CDPCoreClient::OnGuildMemberLv( CAr & ar, DPID, DPID, OBJID )
{
	u_long idPlayer, idGuild;
	int nMemberLv;
	ar >> idPlayer >> nMemberLv >> idGuild;
	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		CGuildMember* pMember	= pGuild->GetMember( idPlayer );
		if( pMember )
		{
			pMember->m_nMemberLv	= nMemberLv;
			pMember->m_nClass = 0;
		}
	}
}

void CDPCoreClient::OnGuildClass( CAr & ar, DPID, DPID, OBJID )
{
	u_long idPlayer, idGuild;
	int nClass;
	ar >> idPlayer >> nClass >> idGuild;
	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		CGuildMember* pMember	= pGuild->GetMember( idPlayer );
		if( pMember )
			pMember->m_nClass	= nClass;
	}
}

void CDPCoreClient::OnGuildNickName( CAr & ar, DPID, DPID, OBJID )
{
	u_long idPlayer, idGuild;
	char strNickName[MAX_GM_ALIAS] = {0,};
	ar >> idPlayer >> idGuild;
	ar.ReadString( strNickName, MAX_GM_ALIAS );
	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		CGuildMember* pMember	= pGuild->GetMember( idPlayer );
		if( pMember )
			strcpy( pMember->m_szAlias, strNickName );
	}
}

void CDPCoreClient::OnChgMaster( CAr & ar, DPID, DPID, OBJID )
{
	u_long idPlayer, idPlayer2, idGuild;
	ar >> idPlayer >> idPlayer2 >> idGuild;
	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		pGuild->m_idMaster	= idPlayer2;
		CGuildMember* pMember	= pGuild->GetMember( idPlayer );
		if( pMember )
		{
			pMember->m_nMemberLv	= GUD_ROOKIE;
			pMember->m_nClass = 0;
		}
		pMember	= pGuild->GetMember( idPlayer2 );
		if( pMember )
		{
			pMember->m_nMemberLv	= GUD_MASTER;
			pMember->m_nClass = 0;
		}
	}
}

void CDPCoreClient::OnGuildMemberLogOut( CAr & ar, DPID, DPID, OBJID )
{
	u_long idPlayer, idGuild;
	ar >> idGuild >> idPlayer;
#if __VER < 11 // __SYS_PLAYER_DATA
	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		CGuildMember* pMember	= pGuild->GetMember( idPlayer );
		if( pMember )
			pMember->m_nLogin = 0;
	}
#endif	// __SYS_PLAYER_DATA
}

void CDPCoreClient::SendWarDead( u_long idPlayer )
{
	BEFORESENDDUAL( ar, PACKETTYPE_WAR_DEAD, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendWarMasterAbsent( u_long idWar, BOOL bDecl )
{
	BEFORESENDDUAL( ar, PACKETTYPE_WAR_MASTER_ABSENT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idWar << bDecl;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendWarTimeout( u_long idWar )
{
	BEFORESENDDUAL( ar, PACKETTYPE_WAR_TIMEOUT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idWar;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::OnAcptWar( CAr & ar, DPID, DPID, OBJID )
{
	u_long idWar, idDecl, idAcpt;
	ar >> idWar >> idDecl >> idAcpt;

	CGuild* pDecl	= g_GuildMng.GetGuild( idDecl );
	CGuild* pAcpt	= g_GuildMng.GetGuild( idAcpt );

	if( !pDecl || !pAcpt )
		return;

	CGuildWar* pWar		= new CGuildWar;
	pWar->m_idWar	= idWar;
	pWar->m_Decl.idGuild	= pDecl->m_idGuild;
	pWar->m_Decl.nSize	= pDecl->GetSize();
	pWar->m_Acpt.idGuild	= pAcpt->m_idGuild;
	pWar->m_Acpt.nSize	= pAcpt->GetSize();
	pWar->m_nFlag	= WF_WARTIME;
	pWar->m_time	= CTime::GetCurrentTime();

	idWar	= g_GuildWarMng.AddWar( pWar );
	if( idWar > 0 )
	{
		pDecl->m_idWar	= idWar;
		pDecl->m_idEnemyGuild	= pAcpt->m_idGuild;
		pAcpt->m_idWar	= idWar;
		pAcpt->m_idEnemyGuild	= pDecl->m_idGuild;

		CUser* pUser;
		for( map<u_long, CGuildMember*>::iterator i = pDecl->m_mapPMember.begin();
			i != pDecl->m_mapPMember.end(); ++i )
		{
			pUser	= (CUser*)prj.GetUserByID( i->second->m_idPlayer );
			if( IsValidObj( pUser ) )
			{
				pUser->m_idWar	= idWar;
				g_UserMng.AddSetWar( pUser, idWar );
			}
		}
		for( i = pAcpt->m_mapPMember.begin();
			i != pAcpt->m_mapPMember.end(); ++i )
		{
			pUser	= (CUser*)prj.GetUserByID( i->second->m_idPlayer );
			if( IsValidObj( pUser ) )
			{
				pUser->m_idWar	= idWar;
				g_UserMng.AddSetWar( pUser, idWar );
			}
		}
	}
	else
	{
		Error( "OnAcptWar()" );
		SAFE_DELETE( pWar );
	}
}

void CDPCoreClient::OnSurrender( CAr & ar, DPID, DPID, OBJID )
{
	u_long idWar, idPlayer;
	BOOL bDecl;
	ar >> idWar >> idPlayer >> bDecl;

	CGuildWar* pWar	= g_GuildWarMng.GetWar( idWar );
	if( pWar )
	{
		CGuild* pGuild;
		if( bDecl )
		{
			pWar->m_Decl.nSurrender++;
			pGuild	= g_GuildMng.GetGuild( pWar->m_Decl.idGuild );
		}
		else
		{
			pWar->m_Acpt.nSurrender++;
			pGuild	= g_GuildMng.GetGuild( pWar->m_Acpt.idGuild );
		}
		if( pGuild )
		{
			CGuildMember* pMember	= pGuild->GetMember( idPlayer );
			if( pMember )
				pMember->m_nSurrender++;
		}
		CUser* pUser	= g_UserMng.GetUserByPlayerID( idPlayer );
		if( IsValidObj( (CObj*)pUser ) )
		{
			pUser->m_idWar	= 0;
			g_UserMng.AddSetWar( pUser, 0 );
		}
	}
}

void CDPCoreClient::OnWarDead( CAr & ar, DPID, DPID, OBJID )
{
	u_long idWar;
	BOOL bDecl;
	ar >> idWar >> bDecl;

	CGuildWar* pWar	= g_GuildWarMng.GetWar( idWar );
	if( pWar )
	{
		if( bDecl )
			pWar->m_Decl.nDead++;
		else
			pWar->m_Acpt.nDead++;
	}
}

void CDPCoreClient::OnWarEnd( CAr & ar, DPID, DPID, OBJID )
{
	u_long idWar;
	int nWptDecl, nWptAcpt;
	int nType;
	ar >> idWar >> nWptDecl >> nWptAcpt >> nType;
	

	CGuildWar* pWar	= g_GuildWarMng.GetWar( idWar );
	if( !pWar )
		return;
	CGuild* pDecl	= g_GuildMng.GetGuild( pWar->m_Decl.idGuild );
	CGuild* pAcpt	= g_GuildMng.GetGuild( pWar->m_Acpt.idGuild );
	if( !pDecl || !pAcpt )
		return;
	g_GuildWarMng.Result( pWar, pDecl, pAcpt, nType, nWptDecl, nWptAcpt );
}

void CDPCoreClient::OnGuildLogoACK( CAr & ar, DPID, DPID, OBJID )
{
/*  // 시야안의 유저에게 로고가 변경됨을 알린다.
	u_long idGuild;
	DWORD dwLogo;

	ar >> idGuild >> dwLogo;
	TRACE("PACKETTYPE_CW_GUILDLOGO - GUILD:%d LOGO:%d\n", idGuild, dwLogo);

	prj.m_AddRemoveLock.Enter( theLineFile );	// lock1
	g_GuildMng.m_AddRemoveLock.Enter( theLineFile );	// lock2

	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		pGuild->SetLogo( dwLogo );

		CUser* pUsertmp;
		CGuildMember* pMember;
		for( map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
		{
			pMember		= i->second;
			pUsertmp	= (CUser*)prj.GetUserByID( pMember->m_idPlayer );
			if( pUsertmp )
				g_UserMng.AddSetLogo( pUsertmp, dwLogo );
		}
	}
	g_GuildMng.m_AddRemoveLock.Leave( theLineFile );	// unlock2
	prj.m_AddRemoveLock.Leave( theLineFile );	// unlock1
*/
	// 전체유저에게 로고가 변경됨을 알린다.
	u_long idGuild;
	DWORD dwLogo;

	ar >> idGuild >> dwLogo;
	TRACE("PACKETTYPE_CW_GUILDLOGO - GUILD:%d LOGO:%d\n", idGuild, dwLogo);

	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
		pGuild->SetLogo( dwLogo );
	g_UserMng.AddSetLogo( idGuild, dwLogo );	// g_UserMng와 교착 상태를 피하기 위해서 unlock된후에 한다.
}

void CDPCoreClient::OnGuildContributionACK( CAr & ar, DPID, DPID, OBJID )
{
	CONTRIBUTION_CHANGED_INFO info;
	ULONG uServerID;

	ar >> uServerID;
	ar >> info;

	TRACE("PACKETTYPE_CW_GUILDCONTRIBUTION GUILD:%d idPlayer:%d dwPxpCount:%d dwPenya:%d " 
          "dwGuildPxpCount:%d dwGuildPenya:%d nGuildLevel:%d\n", 
		info.idGuild, info.idPlayer, info.dwPxpCount, info.dwPenya, 
		info.dwGuildPxpCount, info.dwGuildPenya, info.nGuildLevel);

	CGuild* pGuild	= g_GuildMng.GetGuild( info.idGuild );
	if( pGuild )
	{
		if( uServerID != ::g_uKey )		// 이 메세지의 원본서버가 아닌 경우만 update
			pGuild->SetContribution( info );	

		CUser* pUser;
		CGuildMember* pMember;
		for( map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
		{
			pMember		= i->second;
			pUser	= (CUser*)prj.GetUserByID( pMember->m_idPlayer );
		
			if( IsValidObj( pUser ) )
				pUser->AddContribution( info );
		}
	}
}

void CDPCoreClient::OnModifyVote( CAr & ar, DPID, DPID, OBJID )
{
	u_long idVote, idGuild;
	BYTE cbOperation;
	BYTE cbExtra;

	ar >> idGuild;
	ar >> idVote;
	ar >> cbOperation;
	ar >> cbExtra;

	TRACE("PACKETTYPE_CW_MODIFYVOTE guild:%d vote:%d case:%d extra:%d\n", idGuild, idVote, cbOperation, cbExtra );

	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		if( pGuild->ModifyVote( idVote, cbOperation, cbExtra ) )
		{
			CUser* pUser;
			CGuildMember* pMember;
			for( map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
			{
				pMember		= i->second;
				pUser	= (CUser*)prj.GetUserByID( pMember->m_idPlayer );
				if( IsValidObj( pUser ) ) 
					pUser->AddModifyVote( idVote, cbOperation, cbExtra );
			}
		}
	}
}

void CDPCoreClient::OnAddVoteResultACk( CAr & ar, DPID, DPID, OBJID )
{
	VOTE_INSERTED_INFO	info;
	ar >> info;

	TRACE("PACKETTYPE_CW_ADDVOTERESULT - GUILD:%d VOTE:%d\n",  info.idGuild, info.idVote);
	if( info.idVote == 0 )
		return;

	CGuild* pGuild	= g_GuildMng.GetGuild( info.idGuild );
	if( pGuild )
	{
		CUser* pUser;
		if( info.idVote )
		{
			pGuild->AddVote( info );

			CGuildMember* pMember;
			for( map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
			{
				pMember		= i->second;
				pUser	= (CUser*)prj.GetUserByID( pMember->m_idPlayer );
				if( IsValidObj( pUser ) ) 
					pUser->AddInsertedVote( info );
			}
		}
		else
		{
			// 마스터에게 실패를 알린다.
			pUser	= (CUser*)prj.GetUserByID( pGuild->m_idMaster );
			if( IsValidObj( pUser ) ) 
				pUser->AddInsertedVote( info );
		}
	}
}

void CDPCoreClient::OnGuildNoticeACk( CAr & ar, DPID, DPID, OBJID )
{
	u_long idGuild;
	char szNotice[MAX_BYTE_NOTICE];

	ar >> idGuild;
	ar.ReadString( szNotice, MAX_BYTE_NOTICE );
	TRACE("PACKETTYPE_CW_GUILDNOTICE - GUILD:%d Notice:%s\n", idGuild, szNotice);

	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		pGuild->SetNotice( szNotice );

		CUser* pUser;
		CGuildMember* pMember;
		for( map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
		{
			pMember		= i->second;
			pUser	= (CUser*)prj.GetUserByID( pMember->m_idPlayer );
			if( IsValidObj(pUser) ) 
				pUser->AddSetNotice( idGuild, szNotice );
		}
	}
}

void CDPCoreClient::OnGuildAuthority( CAr & ar, DPID, DPID, OBJID )
{
	u_long uGuildId;
	DWORD dwAuthority[MAX_GM_LEVEL];
	
	ar >> uGuildId;

	ar.Read( dwAuthority, sizeof(dwAuthority) );

	CGuild* pGuild	= g_GuildMng.GetGuild( uGuildId );
	if( pGuild )
	{
		memcpy( pGuild->m_adwPower, dwAuthority, sizeof(dwAuthority) );

		CUser* pUser;
		CGuildMember* pMember;
		for( map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
		{
			pMember		= i->second;
			pUser	= (CUser*)prj.GetUserByID( pMember->m_idPlayer );
			if( IsValidObj(pUser) ) 
				pUser->AddSetGuildAuthority( dwAuthority );
		}
	}
}

void CDPCoreClient::OnGuildPenya( CAr & ar, DPID, DPID, OBJID )
{
	u_long uGuildId;
	DWORD dwType, dwPenya;

	ar >> uGuildId;
	ar >> dwType >> dwPenya;

	CGuild* pGuild	= g_GuildMng.GetGuild( uGuildId );
	if( pGuild )
	{
		pGuild->m_adwPenya[dwType] = dwPenya;
		CUser* pUser;
		CGuildMember* pMember;
		for( map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
		{
			pMember		= i->second;
			pUser	= (CUser*)prj.GetUserByID( pMember->m_idPlayer );
			if( IsValidObj(pUser) ) 
				pUser->AddSetGuildPenya( dwType, dwPenya );
		}
	}
}

void CDPCoreClient::OnGuildRealPenya( CAr & ar, DPID, DPID, OBJID )
{
	u_long uGuildId;
	int nGoldGuild;
	ar >> uGuildId >> nGoldGuild;

	CGuild* pGuild	= g_GuildMng.GetGuild( uGuildId );
	if( pGuild )
	{
		if( !(g_eLocal.GetState( ENABLE_GUILD_INVENTORY )) )
		{
			pGuild->m_nGoldGuild = nGoldGuild;
		}

		CUser* pUser;
		CGuildMember* pMember;
		for( map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
		{
			pMember		= i->second;
			pUser	= (CUser*)prj.GetUserByID( pMember->m_idPlayer );
			if( IsValidObj(pUser) ) 
			{
				pUser->AddGuildRealPenya( nGoldGuild, pMember->m_nMemberLv );
			}				
		}
	}
}

#if __VER < 11 // __SYS_PLAYER_DATA
void CDPCoreClient::OnChangeGuildJobLevel( CAr & ar, DPID, DPID, OBJID )
{
	u_long uidGuild, uidPlayer;
	LONG nJob, nLevel;

	ar >> uidGuild >> uidPlayer >> nJob >> nLevel;

	CGuild* pGuild	= g_GuildMng.GetGuild( uidGuild );
	if( pGuild && pGuild->IsMember( uidPlayer ) )
	{
		CGuildMember* pMember;
		pMember = pGuild->GetMember( uidPlayer );
		if( pMember )
		{
			pMember->m_nJob = nJob;
			pMember->m_nLevel = nLevel;
		}

		CUser* pUser;
		for( map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
		{
			pMember		= i->second;
			pUser	= (CUser*)prj.GetUserByID( pMember->m_idPlayer );
			if( IsValidObj(pUser) ) 
			{
				pUser->AddChangeGuildJobLevel( uidPlayer, nJob, nLevel );
			}				
		}
	}
}
#endif	// __SYS_PLAYER_DATA

// raiders_test 유저가 아이템을 사용하고 나가면?
void CDPCoreClient::OnGuildSetName( CAr & ar, DPID, DPID, OBJID )
{
	u_long idGuild;
	char lpszGuild[MAX_G_NAME];
	
	ar >> idGuild;
	ar.ReadString( lpszGuild, MAX_G_NAME );
	u_long idPlayer;
	BYTE nId, nError;
	ar >> idPlayer >> nId >> nError;

	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
	{
		if( nError == 0 )	// no error
			g_GuildMng.SetName( pGuild, lpszGuild );
		CUser* pUser	= g_UserMng.GetUserByPlayerID( idPlayer );
		if( IsValidObj( (CObj*)pUser ) )
		{
			CItemElem* pItemElem;
#ifdef __S_SERVER_UNIFY
			if( pUser->m_bAllAction )
#endif // __S_SERVER_UNIFY
			{
				pItemElem	= (CItemElem*)pUser->GetItemId( nId );
				if( IsUsableItem( pItemElem ) )
					pItemElem->m_bQuery	= FALSE;
			}
			switch( nError )
			{
				case 1:
					// error message	// have not guild
					pUser->AddDefinedText( TID_GAME_COMNOHAVECOM );
					break;
				case 2:
					// error message	// is not kingpin
					pUser->AddDefinedText( TID_GAME_COMDELNOTKINGPIN );
					break;
				case 3:
					// error message	// already exists
					pUser->AddDefinedText( TID_GAME_COMOVERLAPNAME );
					break;
				case 0:
#ifdef __S_SERVER_UNIFY
					if( pUser->m_bAllAction )
#endif // __S_SERVER_UNIFY
					{
						if( IsUsableItem( pItemElem ) )
							pUser->UpdateItem( nId, UI_NUM, pItemElem->m_nItemNum - 1 );
					}
					break;
			}
#ifdef __S_SERVER_UNIFY
			pUser->SetAllAction();
#endif // __S_SERVER_UNIFY
		}
	}
}

void CDPCoreClient::OnGuildMsgControl( CAr & ar, DPID, DPID, OBJID )
{
	if ( !g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) )
	{
		GUILD_MSG_HEADER	Header;
		DWORD				dwPenya;
		BYTE				cbCloak;

		ar.Read( &Header, sizeof(GUILD_MSG_HEADER));
		ar >> dwPenya;
		ar >> cbCloak;		// 망토의 경우 

		CGuild* pGuild	= g_GuildMng.GetGuild( Header.HeadASub );
		if( pGuild )
		{
			CGuildMember*	pMember;
			CUser*			pUsertmp;
			for( map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin();	i != pGuild->m_mapPMember.end(); ++i )
			{
				pMember		= i->second;
				pUsertmp	= (CUser*)prj.GetUserByID( pMember->m_idPlayer );
				if( IsValidObj( pUsertmp ) ) 
				{
					pUsertmp->AddGetGoldGuildBank( dwPenya, 2, pMember->m_idPlayer, cbCloak );	// 2는 업데이트 해야할 클라이게
				}
			}
		}
	}
}

BOOL CDPCoreClient::Contribute( CUser* pUser, DWORD dwPxpCount, DWORD dwPenya )
{
	u_long idGuild, idPlayer;
	idGuild = pUser->m_idGuild;
	idPlayer = pUser->m_idPlayer;

	CGuild* pGuild = g_GuildMng.GetGuild( idGuild );
	if( pGuild == NULL )
		return FALSE;

	CGuildMember* pGuildMember = pGuild->GetMember( idPlayer );
	if( pGuildMember == NULL )
		return FALSE;
		

	int nLastGuildLv = pGuild->m_nLevel;	// 길드 레벨업을 판단하기 위하여 저장
	if( pGuild->AddContribution( dwPxpCount, dwPenya, idPlayer ) == FALSE )
		return FALSE;




	CONTRIBUTION_CHANGED_INFO info;

	info.idGuild		= idGuild;
	info.idPlayer		= idPlayer;
	info.dwPxpCount		= dwPxpCount;
	info.dwPenya		= dwPenya;
	info.dwGuildPxpCount= pGuild->m_dwContributionPxp;
	info.dwGuildPenya	= pGuild->m_nGoldGuild;
	info.nGuildLevel	= pGuild->m_nLevel;

#if __VER >= 11 // __SYS_PLAYER_DATA
	PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->GetPlayerData( idPlayer );
	if( pPlayerData )
		g_dpDBClient.SendGuildContribution( info, ( nLastGuildLv < pGuild->m_nLevel? 1: 0 ), pPlayerData->data.nLevel );
#else	// __SYS_PLAYER_DATA
	g_dpDBClient.SendGuildContribution( info, ( nLastGuildLv < pGuild->m_nLevel? 1: 0 ), pGuildMember->m_nLevel );
#endif	// __SYS_PLAYER_DATA

	{
		BEFORESENDDUAL( ar, PACKETTYPE_WC_GUILDCONTRIBUTION, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << ::g_uKey;
		ar << info;
		SEND( ar, this, DPID_SERVERPLAYER );
	}
	return TRUE;
}

// 길드 스탯변경 요청 
BOOL CDPCoreClient::SendGuildStat( CUser* pUser, GUILD_STAT stat, DWORD data )
{
	BOOL bResult = TRUE;

	switch (stat)
	{
	case GUILD_STAT_LOGO:		// 로고 변경 
		{
			BEFORESENDDUAL( ar, PACKETTYPE_WC_GUILDLOGO, DPID_UNKNOWN, DPID_UNKNOWN );
			ar << pUser->m_idGuild << pUser->m_idPlayer << data;	
			SEND( ar, this, DPID_SERVERPLAYER );
		}
		break;

	case GUILD_STAT_PXPCOUNT:
		bResult =  Contribute( pUser, data, 0 );
		break;

	case GUILD_STAT_PENYA:
		bResult = Contribute( pUser, 0, data );
		break;

	case GUILD_STAT_NOTICE:		// 공지사항 변경 
		{
			BEFORESENDDUAL( ar, PACKETTYPE_WC_GUILDNOTICE, DPID_UNKNOWN, DPID_UNKNOWN );
			ar << pUser->m_idGuild << pUser->m_idPlayer;
			
			// 128바이트보다 큰 경우를 예방하기 위해서 버퍼에 복사한 후에 send
			char szNotice[MAX_BYTE_NOTICE];
			strncpy(szNotice, (char *)data, MAX_BYTE_NOTICE);
			szNotice[MAX_BYTE_NOTICE-1] = '\0';

			ar.WriteString(szNotice);	
			SEND( ar, this, DPID_SERVERPLAYER );
		}
		break;
	default:
		ASSERT( 0 );
	}

	return bResult;
}

void CDPCoreClient::SendGuildGetPay( u_long uGuildId, DWORD nGoldGuild )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILD_DB_REALPENYA, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << uGuildId << nGoldGuild;
	SEND( ar, this, DPID_SERVERPLAYER );		
}






void CDPCoreClient::OnSetFriendState( CAr & ar, DPID, DPID, OBJID )
{
	CUser* pUser;
	u_long uidPlayer;
	DWORD dwState;
	ar >> uidPlayer >> dwState;

	pUser	= (CUser*)prj.GetUserByID( uidPlayer );
	if( IsValidObj( pUser ) )
#ifdef __RT_1025
		pUser->m_RTMessenger.SetState( dwState );
#else	// __RT_1025
		pUser->m_Messenger.m_dwMyState = dwState;
#endif	// __RT_1025
}

void CDPCoreClient::OnFriendInterceptState( CAr & ar, DPID, DPID, OBJID )
{
	u_long uidPlayer;
	u_long uidFriend;
	ar >> uidPlayer >> uidFriend;
	
	CUser* pUser, *pUserFriend;

	pUser = (CUser*)prj.GetUserByID( uidPlayer );
	pUserFriend = (CUser*)prj.GetUserByID( uidFriend );

	if( IsValidObj( pUser ) == FALSE )
		return;

#ifdef __RT_1025
	Friend* pFriend	= pUser->m_RTMessenger.GetFriend( uidFriend );
	if( pFriend )
	{
		if( pFriend->bBlock )
		{
			pFriend->bBlock		= FALSE;
			if( IsValidObj( pUserFriend ) )
				pFriend->dwState	= pUserFriend->m_RTMessenger.GetState();
			else
				pFriend->dwState	= FRS_OFFLINE;
		}
		else
		{
			pFriend->bBlock		= TRUE;
			pFriend->dwState	= 0;
		}
	}	
#else	// __RT_1025
	LPFRIEND pFriend	= pUser->m_Messenger.GetFriend( uidFriend );
	if( pFriend )
	{
		if( pFriend->dwState == FRS_BLOCK )	// 차단상태
		{
			// 차단해제를 하려고함 :: 차단해제를 하면 그넘의 상태를 가지고 와서 나에게만 보내면 됨 : 나한테만 보내줌
			if( pUserFriend )
			{
				pFriend->dwState	= pUserFriend->m_Messenger.m_dwMyState;
			}
			else
			{
				pFriend->dwState	= FRS_OFFLINE;
			}
			
			LPFRIEND pDFriend = pUser->m_Messenger.GetDefferntFriend( uidFriend );
			if( pDFriend )
			{
				pDFriend->dwState = 0;
			}

		}
		else	// 차단해제 상태
		{
			// 차단을 하려고함 :: 나는 그넘을 블럭상태라고 나에게 보내주고 그넘에게는 나를 로그아웃이라고 함 : 나에게 보내주고 그넘한태두 보내줌
			pFriend->dwState	= FRS_BLOCK;
			LPFRIEND pDFriend = pUser->m_Messenger.GetDefferntFriend( uidFriend );
			if( pDFriend )
			{
				pDFriend->dwState = FRS_BLOCK;
			}
		}
		pFriend->bSave = TRUE;
	}
#endif	// __RT_1025
}

#if __VER < 11 // __SYS_PLAYER_DATA
void CDPCoreClient::OnFriendChangeJob( CAr & ar, DPID, DPID, OBJID )
{
	vector< u_long > vecIdFriend;
	CUser* pUser;
	u_long uidPlayer;
	int nJob;
	ar >> uidPlayer >> nJob;

	pUser = (CUser*)prj.GetUserByID( uidPlayer );
	if( IsValidObj( pUser ) )
	{
		for( C2FriendPtr::iterator i = pUser->m_Messenger.m_adifferntFriend.begin() ; i != pUser->m_Messenger.m_adifferntFriend.end() ; i++ )
		{
			LPFRIEND pFriend = (LPFRIEND)i->second;
			if( pFriend )
				vecIdFriend.push_back( pFriend->dwUserId );
		}

		for( int j = 0 ; j < vecIdFriend.size() ; j++ )
		{
			CUser* pPlayFriend = (CUser*)prj.GetUserByID( vecIdFriend[j] );
			if( IsValidObj( pPlayFriend ) )
			{
				LPFRIEND pFriendbuf = pPlayFriend->m_Messenger.GetFriend( uidPlayer );
				if( pFriendbuf )
				{
					pFriendbuf->bSave = TRUE;
					pFriendbuf->nJob = nJob;
				}
				pPlayFriend->AddFriendChangeJob( uidPlayer, nJob );
			}
		}		
	}
}
#endif	// __SYS_PLAYER_DATA

void CDPCoreClient::OnPartyChangeLeader( CAr & ar, DPID, DPID, OBJID )
{
	u_long uPartyId, idChangeLeader;
	ar >> uPartyId >> idChangeLeader;
	
	CParty* pParty;

	pParty	= g_PartyMng.GetParty( uPartyId );
	if( pParty )
	{
		pParty->ChangeLeader( idChangeLeader );

		CUser* pMember;
		for( int i = 0; i < pParty->m_nSizeofMember; i++ )
		{
			pMember		= (CUser*)prj.GetUserByID( pParty->m_aMember[i].m_uPlayerId );
			if( IsValidObj( pMember ) )
				pMember->AddPartyChangeLeader( idChangeLeader );
		}
/*
#if __VER >= 12 // __PARSKILL1001	//12차 파스킬 아이템 수정  world,core,neuz
		CMover* pMover = pParty->GetLeader();
		if( pMover )
		{
			if( pMover->IsSMMode( SM_PARTYSKILL1 ) || pMover->IsSMMode( SM_PARTYSKILL15 ) || pMover->IsSMMode( SM_PARTYSKILL30 ) )
                g_DPCoreClient.SendUserPartySkill( pMover->m_idPlayer, PARTY_PARSKILL_MODE, 1000, 0, 1 );
			else
                g_DPCoreClient.SendUserPartySkill( pMover->m_idPlayer, PARTY_PARSKILL_MODE, 0, 0, 1 );
		}
#endif //__PARSKILL1001	//12차 파스킬 아이템 수정  world,core,neuz
*/
	}
}

void CDPCoreClient::OnGameRate( CAr & ar, DPID, DPID, OBJID )
{
	FLOAT fRate;
	BYTE nFlag;
	ar >> fRate;
	ar >> nFlag;
	switch( nFlag )
	{
	case GAME_RATE_SHOPCOST:
		{
			prj.m_fShopCost = fRate;
		}
		break;
	case GAME_RATE_ITEMDROP:
		{
			prj.m_fItemDropRate = fRate;
		}
		break;
	case GAME_RATE_GOLDDROP:
		{
			prj.m_fGoldDropRate = fRate;
		}
		break;
	case GAME_RATE_MONSTEREXP:
		{
			prj.m_fMonsterExpRate = fRate;
		}
		break;
	case GAME_RATE_MONSTERHIT:
		{
			prj.m_fMonsterHitRate = fRate;
		}
		break;
	case GAME_RATE_X3:
		{
			prj.m_fItemDropRate = fRate;
			prj.m_fGoldDropRate = fRate;
			prj.m_fMonsterExpRate = fRate;
		}
		break;
	}
}

void CDPCoreClient::OnLoadConstant( CAr & ar, DPID, DPID, OBJID )
{
	CProject::LoadConstant( "Constant.inc" );
	g_UserMng.AddGameSetting();
}

void CDPCoreClient::OnSetMonsterRespawn( CAr & ar, DPID, DPID, OBJID )
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

	D3DXVECTOR3 vPos;
	CWorld* pWorld = NULL;
	
	CUser* pUser	= (CUser*)prj.GetUserByID( uidPlayer );
	if( IsValidObj( (CObj*)pUser ) )
	{
		vPos = pUser->GetPos();
		pWorld = pUser->GetWorld();
	}

	if( pWorld != NULL )
	{
		CRespawnInfo ri;
		ri.m_dwType = OT_MOVER;
		ri.m_dwIndex = dwMonsterID;
		ri.m_cb = dwRespawnNum;
		ri.m_nActiveAttackNum = dwAttackNum;
		if( bFlying )
			ri.m_vPos = vPos;
		ri.m_rect.left		= (LONG)( vPos.x - dwRect );
		ri.m_rect.right		= (LONG)( vPos.x + dwRect );
		ri.m_rect.top		= (LONG)( vPos.z - dwRect );
		ri.m_rect.bottom	= (LONG)( vPos.z + dwRect );
		ri.m_uTime			= (u_short)( dwRespawnTime );
		ri.m_cbTime = 0;
		
#ifdef __S1108_BACK_END_SYSTEM
		pWorld->m_respawner.Add( ri, TRUE );
#else // __S1108_BACK_END_SYSTEM
		pWorld->m_respawner.Add( ri );
#endif // __S1108_BACK_END_SYSTEM
		
		char chMessage[512] = {0,};
		sprintf( chMessage, "Add Respwan MonsterID = %d(%d/%d) Rect(%d, %d, %d, %d) Time = %d", 
			ri.m_dwIndex, ri.m_cb, ri.m_nActiveAttackNum, ri.m_rect.left, ri.m_rect.right, ri.m_rect.top, ri.m_rect.bottom, ri.m_uTime );
		pUser->AddText( chMessage );
		
	}
}


// 코어서버로부터 리스트를 받을 때 
void CDPCoreClient::OnCWWantedList( CAr & ar, DPID, DPID, DPID )
{
	CWantedListSnapshot& wantedListSnapshot = CWantedListSnapshot::GetInstance();
	wantedListSnapshot.Read( ar );
}

// 코어서버로 부터 현상금을 받을 때 
void CDPCoreClient::OnCWWantedReward( CAr & ar, DPID, DPID, DPID )
{
	u_long		idPlayer, idAttacker;
	__int64		nGold;
	DWORD		dwWorldID;
	D3DXVECTOR3 vPos;

	ar >> idPlayer >> idAttacker >> nGold >> dwWorldID >> vPos;
#ifdef __LAYER_1015
	int nLayer;
	ar >> nLayer;
#endif	// __LAYER_1015

	CWorld* pWorld	= g_WorldMng.GetWorld( dwWorldID );
	if( !pWorld )
	{
		Error( "OnCWWantedReward cannot found world.\n", dwWorldID );
		return;
	}

	int nDrop = 0;
	while( nGold > 0 )
	{
		if( nGold > MAX_GOLD_REWARD )
			nDrop = MAX_GOLD_REWARD;
		else
			nDrop = (int)( nGold );
		
		nGold -= nDrop;

		CItemElem* pItemElem	= new CItemElem;
		pItemElem->m_dwItemId	= II_GOLD_REWARD;
		pItemElem->m_nItemNum	= 0;					// 보통 돈 계열에서 사용되는 변수지만 II_GOLD_REWARD는 사용하지 않는다.
		pItemElem->m_nHitPoint  = nDrop;				// II_GOLD_REWARD는 여기서 사용한다. 

		CItem* pItem			= new CItem;
		pItem->m_pItemBase		= pItemElem;
		pItem->SetIndex( D3DDEVICE, pItemElem->m_dwItemId );
		pItem->SetPos( vPos );
		pItem->SetAngle( (float)xRandom( 360 ) );
		pWorld->ADDOBJ( pItem, TRUE, nLayer );
	}
}

// 코어서버에 현상금을 누적 요청
void CDPCoreClient::SendWCWantedGold( LPCTSTR szPlayer, u_long idPlayer, int nGold, LPCTSTR szMsg )
{
	BEFORESENDDUAL( ar, PACKETTYPE_WC_WANTED_GOLD, DPID_UNKNOWN, DPID_UNKNOWN );
	ar.WriteString( szPlayer );
	ar << idPlayer << nGold;
	ar.WriteString( szMsg );

	SEND( ar, this, DPID_SERVERPLAYER );
}

// 코어서버에 (현상금얻기 & 현상금 clear) 요청  
#ifdef __LAYER_1015
void CDPCoreClient::SendWCWantedReward( u_long idPlayer, u_long idAttacker, LPCTSTR szFormat, DWORD dwWorldID, const D3DXVECTOR3& vPos, int nLayer )
#else	// __LAYER_1015
void CDPCoreClient::SendWCWantedReward( u_long idPlayer, u_long idAttacker, LPCTSTR szFormat, DWORD dwWorldID, const D3DXVECTOR3& vPos )
#endif	// __LAYER_1015
{
	BEFORESENDDUAL( ar, PACKETTYPE_WC_WANTED_REWARD, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << idAttacker << dwWorldID << vPos;
	ar.WriteString( szFormat );
#ifdef __LAYER_1015
	ar << nLayer;
#endif	// __LAYER_1015
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendSetPartyDuel( u_long idParty1, u_long idParty2, BOOL bDuel )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SETPARTYDUEL, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idParty1 << idParty2 << bDuel;
	SEND( ar, this, DPID_SERVERPLAYER );
}

// raiders_test 사용하고 나가면?
void CDPCoreClient::OnSetPlayerName( CAr& ar, DPID, DPID, OBJID )
{
	u_long idPlayer;
	char lpszPlayer[MAX_PLAYER]	= { 0, };
	DWORD dwData;
	BOOL f;

	ar >> idPlayer;
	ar.ReadString( lpszPlayer, MAX_PLAYER );
	ar >> dwData >> f;
	WORD wId	= LOWORD( dwData );
	WORD wMode	= HIWORD( dwData );

	char lpOutputString[256]	= { 0, };
	sprintf( lpOutputString, "WORLDSERVER.EXE\t// %d\t// %s\t// %08x\t// %d", idPlayer, lpszPlayer, dwData, f );
	OutputDebugString( lpOutputString );

	CUser* pUser	= (CUser*)prj.GetUserByID( idPlayer );
	if( IsValidObj( (CObj*)pUser ) )
	{
		if( f )
		{
			if( wMode )
			{
				pUser->SetNotMode( QUERYSETPLAYERNAME_MODE );
				g_UserMng.AddModifyMode( pUser );
			}
			pUser->SetName( lpszPlayer );

#ifdef __S_SERVER_UNIFY
			if( pUser->m_bAllAction )
#endif // __S_SERVER_UNIFY
			{
				CItemElem* pItemElem	= (CItemElem*)pUser->GetItemId( wId );
				if( pItemElem )
				{
					ASSERT( pItemElem->m_bQuery );
					pItemElem->m_bQuery	= FALSE;
					pUser->UpdateItem( (BYTE)( wId ), UI_NUM, pItemElem->m_nItemNum - 1 );
				}
			}
		}
		else
		{
#ifdef __S_SERVER_UNIFY
			if( pUser->m_bAllAction )
#endif // __S_SERVER_UNIFY
			{
				if( (int)wId >= 0 )
				{
					CItemElem* pItemElem	= (CItemElem*)pUser->GetItemId( wId );
					if( pItemElem )
					{
						ASSERT( pItemElem->m_bQuery );
						pItemElem->m_bQuery	= FALSE;
					}
				}
			}
			pUser->AddDefinedText( TID_DIAG_0034 );
		}
#ifdef __S_SERVER_UNIFY
		pUser->SetAllAction();
#endif // __S_SERVER_UNIFY
	}

	if( f )
	{
		char lpOutputString[128]	= { 0, };
		sprintf( lpOutputString, "WORLDSERVER.EXE\tPACKETTYPE_QUERYSETPLAYERNAME\t//%d:%s\t-%08x", idPlayer, lpszPlayer, dwData );
		OutputDebugString( lpOutputString );
//		char* lpszOldPlayer	= prj.GetPlayerString( idPlayer );
//		if( lpszOldPlayer )
//			CWantedListSnapshot::GetInstance().Rename( lpszOldPlayer, lpszPlayer );
#if __VER >= 11 // __SYS_PLAYER_DATA
		CPlayerDataCenter*	pPlayerDataCenter	= CPlayerDataCenter::GetInstance();
		PlayerData* pPlayerData	= pPlayerDataCenter->GetPlayerData( idPlayer );
		if( pPlayerData )
		{
			PlayerData pd;
			memcpy( &pd, pPlayerData, sizeof(PlayerData) );
			pPlayerDataCenter->DeletePlayerData( idPlayer );
			lstrcpy( pd.szPlayer, lpszPlayer );
			pPlayerDataCenter->AddPlayerData( idPlayer, pd );
		}
#else	// __SYS_PLAYER_DATA
		prj.RemovePlayerID( idPlayer );
		prj.SetPlayerID( idPlayer, lpszPlayer );
#endif	// __SYS_PLAYER_DATA
	}
}

void CDPCoreClient::SendQuerySetGuildName( u_long idPlayer, u_long idGuild, const char* lpszPlayer, BYTE nId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUERYSETGUILDNAME, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << idGuild;
	ar.WriteString( lpszPlayer );
	ar << nId;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendSetSnoop( u_long idPlayer, u_long idSnoop, BOOL bRelease )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SETSNOOP, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer << idSnoop << bRelease;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendChat( u_long idPlayer1, u_long idPlayer2, const char* lpszChat )
{
	BEFORESENDDUAL( ar, PACKETTYPE_CHAT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idPlayer1 << idPlayer2;
	ar.WriteString( lpszChat );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::OnSetSnoop( CAr & ar, DPID, DPID, OBJID )
{
//	OutputDebugString( "WORLDSERVER.EXE\t// PACKETTYPE_SETSNOOP" );
	u_long idPlayer, idSnoop;
	BOOL bRelease;
	ar >> idPlayer >> idSnoop >> bRelease;

	CUser* pUser	= g_UserMng.GetUserByPlayerID( idPlayer );
	if( IsValidObj( (CObj*)pUser ) )
		pUser->m_idSnoop	= idSnoop;
}

void CDPCoreClient::SendSetSnoopGuild( u_long idGuild, BOOL bRelease )
{
	BEFORESENDDUAL( ar, PACKETTYPE_SETSNOOPGUILD, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << idGuild << bRelease;
	SEND( ar, this, DPID_SERVERPLAYER );
}

#ifdef __EVENT0913
void CDPCoreClient::OnEvent0913( CAr & ar, DPID, DPID, DPID )
{
	BOOL f;
	ar >> f;
	if( f )
	{
		prj.m_fItemDropRate		*= 2;
		if( g_eLocal.SetState( EVE_EVENT0913, 1 ) )
			g_UserMng.AddSetLocalEvent( EVE_EVENT0913, 1 );
	}
	else
	{
		prj.m_fItemDropRate		/= 2;
		if( g_eLocal.ClearEvent( EVE_EVENT0913 ) )
			g_UserMng.AddSetLocalEvent( EVE_EVENT0913, 0 );
	}
	Error( "OnEvent0913-%d, %f", g_eLocal.GetState( EVE_EVENT0913 ), prj.m_fItemDropRate );
}
#endif	// __EVENT0913

#ifdef __EVENT1206
void CDPCoreClient::OnEvent1206( CAr & ar, DPID, DPID, DPID )
{
	BOOL f;
	ar >> f;
	if( f )
	{
		if( g_eLocal.SetState( EVE_EVENT1206, 1 ) )
			g_UserMng.AddSetLocalEvent( EVE_EVENT1206, 1 );
	}
	else
	{
		if( g_eLocal.ClearEvent( EVE_EVENT1206 ) )
			g_UserMng.AddSetLocalEvent( EVE_EVENT1206, 0 );
	}
	Error( "OnEvent1206-%d", g_eLocal.GetState( EVE_EVENT1206 ) );
}
#endif	// __EVENT1206

void CDPCoreClient::OnEvent( CAr & ar, DPID, DPID, DPID )
{
	DWORD dwEvent;
	ar >> dwEvent;
	// xmas
	if( dwEvent & EVE_FLAG1219 )
	{
		if( g_eLocal.GetState( EVE_EVENT1219 ) == 0 )
		{
			if( g_eLocal.SetState( EVE_EVENT1219, 1 ) )
			{
				g_UserMng.AddSetLocalEvent( EVE_EVENT1219, 1 );
				Error( "OnEvent-Xmas:1" );
			}
		}
	}
	else
	{
		if( g_eLocal.GetState( EVE_EVENT1219 ) == 1 )
		{
			if( g_eLocal.ClearEvent( EVE_EVENT1219 ) )
			{
				g_UserMng.AddSetLocalEvent( EVE_EVENT1219, 0 );
				Error( "OnEvent-Xmas:0" );
			}
		}
	}
	//
	if( dwEvent & EVE_FLAG0127 )
	{
		if( g_eLocal.GetState( EVE_EVENT0127 ) == 0 )
		{
			if( g_eLocal.SetState( EVE_EVENT0127, 1 ) )
			{
				g_UserMng.AddSetLocalEvent( EVE_EVENT0127, 1 );
				Error( "OnEvent-NewYear:1" );
			}
		}
	}
	else
	{
		if( g_eLocal.GetState( EVE_EVENT0127 ) == 1 )
		{
			if( g_eLocal.ClearEvent( EVE_EVENT0127 ) )
			{
				g_UserMng.AddSetLocalEvent( EVE_EVENT0127, 0 );
				Error( "OnEvent-NewYear:0" );
			}
		}
	}
	//
	if( dwEvent & EVE_FLAG0214 )
	{
		if( g_eLocal.GetState( EVE_EVENT0214 ) == 0 )
		{
			if( g_eLocal.SetState( EVE_EVENT0214, 1 ) )
			{
				g_UserMng.AddSetLocalEvent( EVE_EVENT0214, 1 );
				Error( "OnEvent-ValentinesDay:1" );
			}
		}
	}
	else
	{
		if( g_eLocal.GetState( EVE_EVENT0214 ) == 1 )
		{
			if( g_eLocal.ClearEvent( EVE_EVENT0214 ) )
			{
				g_UserMng.AddSetLocalEvent( EVE_EVENT0214, 0 );
				Error( "OnEvent-ValentinesDay:0" );
			}
		}
	}
}

void CDPCoreClient::OnGuildCombatState( CAr & ar, DPID, DPID, DPID )
{
	int nState;
	ar >> nState;
	
	g_GuildCombatMng.m_nState = nState;
	if( g_GuildCombatMng.m_nState == CGuildCombat::OPEN_STATE )
		g_GuildCombatMng.m_dwTime = 0;

	g_UserMng.AddGuildCombatState();
}

void CDPCoreClient::OnRemoveUserFromCORE( CAr & ar, DPID, DPID, DPID )
{
	DWORD dwSerial;
	ar >> dwSerial;		// CACHE에서 생성된 serial한 값 

	g_UserMng.RemoveUser( dwSerial ); 
}

void CDPCoreClient::SendPing( void )
{
	if( FALSE == m_bAlive )
	{
#ifndef _DEBUG
		Error( "CORE : is not alive" );
#endif	// _DEBUG
	}

	m_bAlive	= FALSE;
	BEFORESENDDUAL( ar, PACKETTYPE_PING, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << time_null();
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::OnPing( CAr & ar, DPID, DPID, DPID )
{
	time_t tSend, tTrans;
	ar >> tSend >> tTrans;

	time_t d	= time_null() - tSend;
	CTime tmTrans( tTrans );
#if !defined(_DEBUG)
	if( d > 10 )
		Error( "ping CORE - %d, trans:%s", d, tmTrans.Format( "%Y/%m/%d %H:%M:%S" ) );
#endif
	
	m_bAlive	= TRUE;
}

void CDPCoreClient::OnDestroyPlayer( CAr & ar, DPID, DPID, OBJID )
{
	u_long idPlayer;
	ar >> idPlayer;
	CUser* pUser	= g_UserMng.GetUserByPlayerID( idPlayer );
	if( pUser )
		g_DPSrvr.QueryDestroyPlayer( pUser->m_Snapshot.dpidCache, pUser->m_Snapshot.dpidUser, pUser->m_dwSerial, pUser->m_idPlayer );
}

#if __VER >= 14 // __INSTANCE_DUNGEON
void CDPCoreClient::OnInstanceDungeonAllInfo( CAr & ar, DPID, DPID, OBJID )
{
	CInstanceDungeonHelper::GetInstance()->OnInstanceDungeonAllInfo( ar );
}

void CDPCoreClient::OnInstanceDungeonCreate( CAr & ar, DPID, DPID, OBJID )
{
	int nType;
	DWORD dwDungeonId;
	ID_INFO ID_Info( NULL_ID, NULL_ID );
	
	ar >> nType >> dwDungeonId;
	ID_Info.Serialize( ar );
	
	CInstanceDungeonHelper::GetInstance()->OnCreateDungeon( nType, ID_Info, dwDungeonId );
}

void CDPCoreClient::SendInstanceDungeonCreate( int nType, DWORD dwDungeonId, ID_INFO & ID_Info )
{
	BEFORESENDDUAL( ar, PACKETTYPE_INSTANCEDUNGEON_CREATE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nType << dwDungeonId;
	ID_Info.Serialize( ar );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::OnInstanceDungeonDestroy( CAr & ar, DPID, DPID, OBJID )
{
	int nType;
	DWORD dwDungeonId;
	ID_INFO ID_Info( NULL_ID, NULL_ID );
	
	ar >> nType >> dwDungeonId;
	ID_Info.Serialize( ar );
	
	CInstanceDungeonHelper::GetInstance()->OnDestroyDungeon( nType, ID_Info, dwDungeonId );
}

void CDPCoreClient::SendInstanceDungeonDestroy( int nType, DWORD dwDungeonId, ID_INFO & ID_Info )
{
	BEFORESENDDUAL( ar, PACKETTYPE_INSTANCEDUNGEON_DESTROY, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nType << dwDungeonId;
	ID_Info.Serialize( ar );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::OnInstanceDungeonSetCoolTimeInfo( CAr & ar, DPID, DPID, OBJID )
{
	int nType;
	DWORD dwPlayerId;
	COOLTIME_INFO CT_Info;
	ULONG uKey;
	
	ar >> uKey >> nType >> dwPlayerId;
	CT_Info.Serialize( ar );
	
	CInstanceDungeonHelper::GetInstance()->OnSetDungeonCoolTimeInfo( uKey, nType, CT_Info, dwPlayerId );
}

void CDPCoreClient::SendInstanceDungeonSetCoolTimeInfo( int nType, DWORD dwPlayerId, COOLTIME_INFO & CT_Info )
{
	BEFORESENDDUAL( ar, PACKETTYPE_INSTANCEDUNGEON_SETCOOLTIME, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << g_uKey << nType << dwPlayerId;
	CT_Info.Serialize( ar );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::OnInstanceDungeonDeleteCoolTimeInfo( CAr & ar, DPID, DPID, OBJID )
{
	int nType;
	DWORD dwPlayerId;
	
	ar >> nType >> dwPlayerId;
	
	CInstanceDungeonHelper::GetInstance()->OnDeleteDungeonCoolTimeInfo( nType, dwPlayerId );
}
#endif // __INSTANCE_DUNGEON

#ifdef __QUIZ
void CDPCoreClient::SendQuizSystemMessage( int nDefinedTextId, BOOL bAll, int nChannel, int nTime )
{
	BEFORESENDDUAL( ar, PACKETTYPE_QUIZ_NOTICE, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << nDefinedTextId << bAll << nChannel << nTime;
	SEND( ar, this, DPID_SERVERPLAYER );
}
#endif // __QUIZ
