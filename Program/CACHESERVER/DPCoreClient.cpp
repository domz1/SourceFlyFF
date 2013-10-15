#include "StdAfx.h"
#include "DPCacheSrvr.h"
#include "DPClient.h"
#include "DPCoreClient.h"
#include "ServerDesc.h"
#include "MyTrace.h"
#include "Player.h"

#ifdef __TRACE1027
#include "..\_UnhandledException\exceptionhandler.h"
#endif	// __TRACE1027

#ifdef __CRASH_0404
#include "crashstatus.h"
#endif	// __CRASH_0404

extern CDPCacheSrvr		g_DPCacheSrvr;
extern	CDPClientArray	g_DPClientArray;
extern	CMyTrace	g_MyTrace;

CDPCoreClient::CDPCoreClient()
{
	BEGIN_MSG;
	ON_MSG( PACKETTYPE_PROCSERVER_LIST, OnProcServerList );
	ON_MSG( PACKETTYPE_LOAD_WORLD, OnProcServer );
	ON_MSG( PACKETTYPE_JOIN, OnJoin );
	ON_MSG( PACKETTYPE_LEAVE, OnLeave );
	ON_MSG( PACKETTYPE_DESTROY_ALLPLAYERS, OnDestroyAllPlayers );
	ON_MSG( PACKETTYPE_KILLPLAYER, OnKillPlayer );

#ifdef __CRASH_0404
	CCrashStatus::GetInstance()->RegisterDPMng( "CDPCoreClient", this );
#endif	// __CRASH_0404
}

CDPCoreClient::~CDPCoreClient()
{

}

void CDPCoreClient::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_DESTROYPLAYERORGROUP:
			{
				g_MyTrace.Add( CMyTrace::Key( "main" ), TRUE, "destroy player or group." );
				break;
			}
	}
}

void CDPCoreClient::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	static size_t	nSize	= sizeof(DPID);

#ifdef __CRASH_0404
	CCrashStatus::GetInstance()->SetLastPacket( this, 0xEEEEEEEE );
#endif	// __CRASH_0404

	LPBYTE lpBuf	= (LPBYTE)lpMsg + nSize;
	ULONG	uBufSize	= dwMsgSize - nSize;
	DPID dpidUser	= *(UNALIGNED LPDPID)lpMsg;

	CAr ar( lpBuf, uBufSize );
	GETTYPE( ar );

#ifdef __CRASH_0404
	CCrashStatus::GetInstance()->SetLastPacket( this, dw );
#endif	// __CRASH_0404

	void ( theClass::*pfn )( theParameters )	=	GetHandler( dw );
	
	if( pfn )
		( this->*( pfn ) )( ar, dpidUser );
	else
	{
		switch( dw )
		{
			case PACKETTYPE_SUMMONPLAYER:
			case PACKETTYPE_TELEPORTPLAYER:
			case PACKETTYPE_MODIFYMODE:
			case PACKETTYPE_BUYING_INFO:
				{
					CMclAutoLock	Lock( CPlayerMng::Instance()->m_AddRemoveLock );
					g_DPClientArray.SendToServer( dpidUser, lpMsg, dwMsgSize );
					break;
				}
			default:
				g_DPCacheSrvr.Send( lpBuf, uBufSize, dpidUser );
				break;
		}
	}
#ifdef __CRASH_0404
	CCrashStatus::GetInstance()->SetLastPacket( this, 0xFFFFFFFF );
#endif	// __CRASH_0404
}

void CDPCoreClient::SendDestroyPlayer( CPlayer* pPlayer )
{
	BEFORESENDSOLE( ar, PACKETTYPE_DESTROY_PLAYER, pPlayer->GetSerial() );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::SendAddPlayer( CPlayer* pPlayer, CRTMessenger & rtmessenger )
{
	BEFORESENDSOLE( ar, PACKETTYPE_JOIN, pPlayer->GetSerial() );
	ar << pPlayer->GetAuthKey();
	ar << pPlayer->GetPlayerId();
	ar << pPlayer->GetNetworkId();
	ar << pPlayer->GetParty();
	ar << pPlayer->GetGuild();
	ar << pPlayer->GetWar();
	ar << pPlayer->GetChannel();
	rtmessenger.Serialize( ar );
	ar.WriteString( pPlayer->GetPlayer() );
	ar.WriteString( pPlayer->GetAddr() );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::OnProcServerList( CAr & ar, DPID )
{
	CServerDesc *pServer;
	short nSize;

	ar >> nSize;
	for( int i = 0; i < nSize; i++ )
	{
		pServer		= new CServerDesc;
		pServer->Serialize( ar );

		if( !g_DPClientArray.Connect( pServer ) ) {
			SAFE_DELETE( pServer );
		}
	}
	g_MyTrace.AddLine( '-' );
}

void CDPCoreClient::OnProcServer( CAr & ar, DPID )
{
	CServerDesc* pServer	= new CServerDesc;
	pServer->Serialize( ar );
	if( !g_DPClientArray.Connect( pServer ) ) {
		SAFE_DELETE( pServer );
	}
}

void CDPCoreClient::OnJoin( CAr & ar, DPID dpid )
{
	CMclAutoLock	Lock( CPlayerMng::Instance()->m_AddRemoveLock );

	DWORD dwSerial;
	BYTE byData;
	ar >> dwSerial;
	ar >> byData;	// 사용하지 않는다.

	CPlayer* pPlayer	= CPlayerMng::Instance()->GetPlayerBySerial( dwSerial );
	if( pPlayer == NULL )
	{
		WriteLog( "CDPCoreClient::OnJoin - player not found" );
		return;
	}

	CDPClient* pClient = g_DPClientArray.GetClient( pPlayer->GetChannel(), pPlayer->GetWorld(), D3DXVECTOR3(0.0f, 0.0f, 0.0f) );
	if( pClient )
	{
		pPlayer->SetClient( pClient );
		pClient->SendJoin( pPlayer );
	}
	else
	{
		WriteLog( "CDPCoreClient.OnJoin: server not found id: %d account: %s player: %s world: %d",
			      pPlayer->GetPlayerId(), pPlayer->GetAccount(), pPlayer->GetPlayer(), pPlayer->GetWorld() );
		g_DPCacheSrvr.DestroyPlayer( dpid );
	}
}

void CDPCoreClient::OnLeave( CAr & ar, DPID dpid )
{
	CMclAutoLock	Lock( CPlayerMng::Instance()->m_AddRemoveLock );

	DWORD dwSerial;
	ar >> dwSerial;
	CPlayer* pPlayer = CPlayerMng::Instance()->GetPlayerBySerial( dwSerial );  // dpid가 serial번호이다.
	if( pPlayer )
		g_DPCacheSrvr.DestroyPlayer( dpid );
}

void CDPCoreClient::OnDestroyAllPlayers( CAr & ar, DPID dpid )
{
	CPlayerMng::Instance()->DestroyPlayer( NULL );
}

void CDPCoreClient::OnKillPlayer( CAr & ar, DPID dpid )
{
	g_DPCacheSrvr.DestroyPlayer( dpid );
}

void CDPCoreClient::SendToServer( DPID dpidUser, LPVOID lpMsg, DWORD dwMsgSize )
{
	CPlayer* pPlayer	= CPlayerMng::Instance()->GetPlayer( dpidUser );
	if( !pPlayer )
		return;
	*reinterpret_cast<UNALIGNED DPID*>( lpMsg )		= pPlayer->GetSerial();
	Send( lpMsg, dwMsgSize, DPID_SERVERPLAYER );
}

CDPCoreClient	g_DPCoreClient;