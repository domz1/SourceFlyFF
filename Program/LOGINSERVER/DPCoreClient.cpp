#include "stdafx.h"
#include "user.h"
extern	CUserMng	g_UserMng;
#include "dploginsrvr.h"
extern	CDPLoginSrvr	g_dpLoginSrvr;
#include "dpcoreclient.h"
#include "msghdr.h"

CDPCoreClient::CDPCoreClient()
{
	BEGIN_MSG;
	ON_MSG( PACKETTYPE_QUERYTICKCOUNT, OnQueryTickCount );
	ON_MSG( PACKETTYPE_PRE_JOIN, OnPreJoin );
	ON_MSG( PACKETTYPE_DESTROY_PLAYER, OnQueryRemovePlayer );
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
				break;
			}
	}
}

void CDPCoreClient::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	CAr ar( (LPBYTE)lpMsg, dwMsgSize );
	GETTYPE( ar );
	void ( theClass::*pfn )( theParameters )	=	GetHandler( dw );
	
	if( pfn ) {
		( this->*( pfn ) )( ar );
	}
	else {
	}
}

HANDLE	hWait;
void CDPCoreClient::QueryTickCount( void )
{
	hWait	= CreateEvent( NULL, FALSE, FALSE, NULL );

	BEFORESEND( ar, PACKETTYPE_QUERYTICKCOUNT );
	ar << (DWORD)timeGetTime();
	SEND( ar, this, DPID_SERVERPLAYER );

	WaitForSingleObject( hWait, INFINITE );
	CloseHandle( hWait );
}

void CDPCoreClient::OnQueryTickCount( CAr & ar )
{
	DWORD dwTime;
	__int64 nTickCount;
	ar >> dwTime >> nTickCount;

	dwTime	= timeGetTime() - dwTime;
	nTickCount	+= ( dwTime * 5000 );	// ( dwTime / 2 ) * 10000

	g_TickCount.Start( nTickCount );

	SetEvent( hWait );
}

void CDPCoreClient::SendPreJoin( DWORD dwAuthKey, const TCHAR* lpszAccount, u_long idPlayer, const TCHAR* lpszPlayer )
{
	BEFORESEND( ar, PACKETTYPE_PRE_JOIN );
	ar << dwAuthKey;
	ar.WriteString( lpszAccount );
	ar << idPlayer;
	ar.WriteString( lpszPlayer );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::OnPreJoin( CAr & ar )
{
	CHAR lpszAccount[MAX_ACCOUNT], lpszPlayer[MAX_PLAYER];
	DWORD dwAuthKey;
	u_long idPlayer;
	BYTE f;
	ar >> dwAuthKey;
	ar.ReadString( lpszAccount, MAX_ACCOUNT );
	ar >> idPlayer;
	ar.ReadString( lpszPlayer, MAX_PLAYER );
	ar >> f;

	CMclAutoLock	Lock( g_UserMng.m_AddRemoveLock );

	CUser* pUser	= g_UserMng.GetUser( lpszAccount );
	if( pUser )
	{
		if( pUser->m_dwAuthKey == dwAuthKey )
		{
			if( f )	// o
				g_dpLoginSrvr.SendHdr( PACKETTYPE_PRE_JOIN, pUser->m_dpid );
			else
			{
				WriteLog( "OnPreJoin(): recv 0 from CORE, %s", lpszAccount );
				pUser->m_idPlayer	= 0;
				g_dpLoginSrvr.DestroyPlayer( pUser->m_dpid );
			}
		}
		else
		{
			WriteLog( "OnPreJoin(): Different dwAuthKey" );
			g_dpLoginSrvr.DestroyPlayer( pUser->m_dpid );
		}
	}
	else
	{
		WriteLog( "OnPreJoin(): Account not found, %s", lpszAccount );
		//
	}
}

void CDPCoreClient::OnQueryRemovePlayer( CAr & ar )
{
	char lpszAccount[MAX_ACCOUNT] = { 0, };
	ar.ReadString( lpszAccount, MAX_ACCOUNT );

	CMclAutoLock	Lock( g_UserMng.m_AddRemoveLock );
	CUser* pUser	= g_UserMng.GetUser( lpszAccount );
	if( pUser )
	{
		pUser->m_idPlayer	= 0;
		g_dpLoginSrvr.DestroyPlayer( pUser->m_dpid );
	}
	else
	{
		WriteLog( "OnQueryRemovePlayer(): lpszAccount not found, %s", lpszAccount );
	}
}

void CDPCoreClient::SendLeave( u_long idPlayer )
{
	BEFORESEND( ar, PACKETTYPE_LEAVE );
	ar << idPlayer;
	SEND( ar, this, DPID_SERVERPLAYER );
}

CDPCoreClient	g_dpCoreClient;