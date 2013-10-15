#include "StdAfx.h"
#include "DPClient.h"
#include "DPCoreClient.h"
#include "DPCacheSrvr.h"
#include "Player.h"
#ifdef __TRACE1027
#include "..\_UnhandledException\exceptionhandler.h"
#endif	// __TRACE1027

#ifdef __PL_0917
#include "packetlog.h"
#endif	// __PL_0917
#include "mytrace.h"
extern	CMyTrace	g_MyTrace;

extern	CDPClientArray	g_DPClientArray;
extern	CDPCoreClient	g_DPCoreClient;

#include "messenger.h"

#ifdef __CRASH_0404
#include "crashstatus.h"
#endif	// __CRASH_0404

CDPCacheSrvr::CDPCacheSrvr()
{
	BEGIN_MSG;
	ON_MSG( PACKETTYPE_JOIN, OnAddConnection );
	ON_MSG( PACKETTYPE_KEEP_ALIVE, OnKeepAlive );
#ifdef __CRASH_0404
	CCrashStatus::GetInstance()->RegisterDPMng( "CDPCacheSrvr", this );
#endif	// __CRASH_0404
}

CDPCacheSrvr::~CDPCacheSrvr()
{

}

void CDPCacheSrvr::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	static	int	cbPlayer	= 0;

	switch( lpMsg->dwType )
	{
		case DPSYS_CREATEPLAYERORGROUP:
			{
#ifdef __CRASH_0404
				CCrashStatus::GetInstance()->SetLastPacket( this, 0xCCCCCCCC );
#endif	// __CRASH_0404
				LPDPMSG_CREATEPLAYERORGROUP lpCreatePlayer	= (LPDPMSG_CREATEPLAYERORGROUP)lpMsg;
				g_MyTrace.Add( CMyTrace::Key( "so" ), FALSE, "//%04d//", ++cbPlayer );
				CPlayerMng::Instance()->AddPlayer( lpCreatePlayer->dpId );
				break;
			}
		case DPSYS_DESTROYPLAYERORGROUP:
			{
#ifdef __CRASH_0404
				CCrashStatus::GetInstance()->SetLastPacket( this, 0xDDDDDDDD );
#endif	// __CRASH_0404
				g_MyTrace.Add( CMyTrace::Key( "so" ), FALSE, "//%04d//", --cbPlayer );
				LPDPMSG_DESTROYPLAYERORGROUP lpDestroyPlayer	= (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
				OnRemoveConnection( lpDestroyPlayer->dpId );
				break;
			}
	}

#ifdef __CRASH_0404
	CCrashStatus::GetInstance()->SetLastPacket( this, 0xFFFFFFFF );
#endif	// __CRASH_0404
}


void CDPCacheSrvr::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	static size_t	nSize	= sizeof(DPID);

#ifdef __CRASH_0404
	CCrashStatus::GetInstance()->SetLastPacket( this, 0xEEEEEEEE );
#endif	// __CRASH_0404

	CAr ar( (LPBYTE)lpMsg + nSize, dwMsgSize - nSize );		// Skip dpid

	if( dwMsgSize < 8 )
		return;

	GETTYPE( ar );

	void ( theClass::*pfn )( theParameters )	=	GetHandler( dw );
	
#ifdef __CRASH_0404
	CCrashStatus::GetInstance()->SetLastPacket( this, dw );
#endif	// __CRASH_0404

#ifdef __PL_0917
	CPacketLog::Instance()->Add( idFrom, dw, dwMsgSize );
#endif	// __PL_0917

	if( pfn ) {
		( this->*( pfn ) )( ar, idFrom, (BYTE*)lpMsg + sizeof(DPID), dwMsgSize - sizeof(DPID) );
	}
	else {
		switch( dw )
		{
			case PACKETTYPE_ADDPARTYMEMBER:
			case PACKETTYPE_REMOVEPARTYMEMBER:
			case PACKETTYPE_CHANGETROUP:
			case PACKETTYPE_CHANPARTYNAME:
			case PACKETTYPE_PARTYCHANGEITEMMODE:
			case PACKETTYPE_PARTYCHANGEEXPMODE:
			case PACKETTYPE_ADDFRIEND:
			case PACKETTYPE_GETFRIENDSTATE:
			case PACKETTYPE_SETFRIENDSTATE:
			case PACKETTYPE_FRIENDINTERCEPTSTATE:
			case PACKETTYPE_REMOVEFRIEND:
			case PACKETTYPE_PARTYCHANGELEADER:
			case PACKETTYPE_DESTROY_GUILD:
			case PACKETTYPE_ADD_GUILD_MEMBER:
			case PACKETTYPE_REMOVE_GUILD_MEMBER:
			case PACKETTYPE_GUILD_MEMBER_LEVEL:
			case PACKETTYPE_GUILD_AUTHORITY:	
			case PACKETTYPE_GUILD_PENYA:
			case PACKETTYPE_GUILD_SETNAME:
			case PACKETTYPE_GUILD_CLASS:
			case PACKETTYPE_GUILD_NICKNAME:
			case PACKETTYPE_DECL_GUILD_WAR:		// 선전 포고
			case PACKETTYPE_ACPT_GUILD_WAR:		// 개전
			case PACKETTYPE_SURRENDER:	// 항복
			case PACKETTYPE_QUERY_TRUCE:	// 휴전
			case PACKETTYPE_ACPT_TRUCE:		// 휴전

#ifdef __GUILDVOTE
			case PACKETTYPE_NC_ADDVOTE:	   // 투표 입력 
			case PACKETTYPE_NC_REMOVEVOTE: // 투표 취소 
			case PACKETTYPE_NC_CLOSEVOTE:  // 투표 종료 
			case PACKETTYPE_NC_CASTVOTE:   // 투표하기 
#endif  // __GUILDVOTE
			case PACKETTYPE_CHG_MASTER:   // 길드 마스터 변경 
			case PACKETTYPE_SENDTAG:
				g_DPCoreClient.SendToServer( idFrom, lpMsg, dwMsgSize );
				break;
			default:
				g_DPClientArray.SendToServer( idFrom, lpMsg, dwMsgSize );
				break;
		}
	}

#ifdef __CRASH_0404
	CCrashStatus::GetInstance()->SetLastPacket( this, 0xFFFFFFFF );
#endif	// __CRASH_0404
}


void CDPCacheSrvr::OnAddConnection( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
	CMclAutoLock	Lock( CPlayerMng::Instance()->m_AddRemoveLock );
	CPlayer* pPlayer	= CPlayerMng::Instance()->GetPlayer( dpid );
	if( pPlayer )
	{
		pPlayer->SetAddr( this );
		pPlayer->Join( ar );
		CRTMessenger rtmessenger;
		int nSize	= rtmessenger.Serialize( ar );
		if( nSize )
		{
			Error( "CRTMessenger.Serialize: %s(%d)", pPlayer->GetAddr(), nSize ); 
			DestroyPlayer( dpid );
		}
		else
			g_DPCoreClient.SendAddPlayer( pPlayer, rtmessenger );
	}
	else
		DestroyPlayer( dpid );
}

void CDPCacheSrvr::OnRemoveConnection( DPID dpid )
{
	CMclAutoLock	Lock( CPlayerMng::Instance()->m_AddRemoveLock );
	
	CPlayer* pPlayer	= CPlayerMng::Instance()->GetPlayer( dpid );
	if( pPlayer )
	{
		if( pPlayer->GetClient() )	// JOIN패킷을 보낸 플레이어만 LEAVE패킷을 보낸다.
		{
			BEFORESENDSOLE( ar, PACKETTYPE_LEAVE, DPID_UNKNOWN );
			ar << pPlayer->GetPlayerId();
			LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );
			g_DPClientArray.SendToServer( dpid, lpBuf, nBufSize );
		}
		CPlayerMng::Instance()->RemovePlayer( dpid );
	}
	else
	{
		WriteError( "CDPCacheSrvr::OnRemoveConnection GetPlayer return NULL." );
	}
}

void CDPCacheSrvr::OnKeepAlive( CAr & ar, DPID dpid, LPBYTE, u_long )
{
	CMclAutoLock	Lock( CPlayerMng::Instance()->m_AddRemoveLock );
	CPlayer* pPlayer	= CPlayerMng::Instance()->GetPlayer( dpid );
	if( pPlayer )
		pPlayer->SetAlive( TRUE );
}

CDPCacheSrvr	g_DPCacheSrvr;