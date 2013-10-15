#include "stdafx.h"
#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
#include "dpcoreclient.h"
#endif // __2ND_PASSWORD_SYSTEM
#include "dploginsrvr.h"
#include "dpdatabaseclient.h"
#include "mytrace.h"
#include "user.h"
#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
#include "LoginProtect.h"
extern	CDPCoreClient	g_dpCoreClient;
#endif // __2ND_PASSWORD_SYSTEM

extern	CDPLoginSrvr	g_dpLoginSrvr;
extern	CMyTrace	g_MyTrace;
extern	char	g_lpDBAddr[16];

extern	CUserMng	g_UserMng;

CDPDatabaseClient::CDPDatabaseClient()
{
	BEGIN_MSG;
	ON_MSG( PACKETTYPE_CLOSE_EXISTING_CONNECTION, OnCloseExistingConnection );
	ON_MSG( PACKETTYPE_ONE_HOUR_NOTIFY, OnOneHourNotify );
	ON_MSG( PACKETTYPE_PLAYER_LIST, OnPlayerList );
	ON_MSG( PACKETTYPE_FAIL, OnFail );
#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
	ON_MSG( PACKETTYPE_LOGIN_PROTECT_CERT, OnLoginProtect );
#endif // __2ND_PASSWORD_SYSTEM
}

CDPDatabaseClient::~CDPDatabaseClient()
{

}

void CDPDatabaseClient::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_DESTROYPLAYERORGROUP:
			{
				break;
			}
	}
}

void CDPDatabaseClient::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	static size_t	nSize	= sizeof(DPID);

	LPBYTE lpBuf	= (LPBYTE)lpMsg + nSize;
	ULONG	uBufSize	= dwMsgSize - nSize;
	DPID dpidUser	= *(UNALIGNED LPDPID)lpMsg;

	CAr ar( lpBuf, uBufSize );
	GETTYPE( ar );
	void ( theClass::*pfn )( theParameters )	=	GetHandler( dw );
	
	if( pfn ) {
		( this->*( pfn ) )( ar, dpidUser, lpBuf, uBufSize );
	}
	else {
		g_dpLoginSrvr.Send( lpBuf, uBufSize, dpidUser );
	}
}

void CDPDatabaseClient::SendLeave( const char* lpszAccount, u_long idPlayer, DWORD dwPlayTime )
{
#ifdef __INTERNALSERVER
	time_t cur = time(NULL);
	WriteLog( "Login CDPDatabaseClient::SendLeave (%s) - %s\n", lpszAccount, ctime( &cur ) );
#endif

//	if( idPlayer == 0 )
//		return;
			
	BEFORESENDSOLE( ar, PACKETTYPE_LEAVE, DPID_UNKNOWN );
	ar.WriteString( lpszAccount );
	ar << idPlayer;
	ar << dwPlayTime;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendCloseError( const char* lpszAccount )
{
	BEFORESENDSOLE( ar, PACKETTYPE_CLOSE_ERROR, DPID_UNKNOWN );
	ar.WriteString( lpszAccount );
	SEND( ar, this, DPID_SERVERPLAYER );
}
/*
void CDPDatabaseClient::OnDisconnCertifier( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
static	\
	TCHAR	lpszAccount[MAX_ACCOUNT];

	ar.ReadString( lpszAccount );

	CUser* pUser;
	CMclAutoLock	Lock( g_UserMng.m_AddRemoveLock );

	pUser	= g_UserMng.GetUser( lpszAccount );
	if( pUser )
		g_dpLoginSrvr.Send( lpBuf, uBufSize, pUser->m_dpid );
}
*/
void CDPDatabaseClient::OnCloseExistingConnection( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
	char lpszAccount[MAX_ACCOUNT]	= { 0, };
	LONG lError = 0;

	ar.ReadString( lpszAccount, MAX_ACCOUNT );
	ar >> lError;

	CMclAutoLock	Lock( g_UserMng.m_AddRemoveLock );
	CUser* pUser	= g_UserMng.GetUser( lpszAccount );
	if( pUser )
	{
		if( lError )
			g_dpLoginSrvr.SendError( lError, pUser->m_dpid );

		g_dpLoginSrvr.DestroyPlayer( pUser->m_dpid );
	}
	else
	{
		SendCloseError( lpszAccount );	// not found
		WriteLog( "OnCloseExistingConnection(): pUser is NULL, %s", lpszAccount );
	}
}

void CDPDatabaseClient::OnOneHourNotify( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
	char lpszAccount[MAX_ACCOUNT]	= { 0, };
	ar.ReadString( lpszAccount, MAX_ACCOUNT );
	
	CMclAutoLock	Lock( g_UserMng.m_AddRemoveLock );
	CUser* pUser	= g_UserMng.GetUser( lpszAccount );
	if( pUser )
	{
		g_dpLoginSrvr.SendHdr( PACKETTYPE_ONE_HOUR_NOTIFY, pUser->m_dpid );
	}
}

void CDPDatabaseClient::SendGetPlayerList( DPID idFrom, const char* lpszAccount, const char* lpszPassword, DWORD dwAuthKey, u_long uIdofMulti )
{
	BEFORESENDSOLE( ar, PACKETTYPE_GETPLAYERLIST, idFrom );
	ar << dwAuthKey;
	ar << uIdofMulti;

	ar.WriteString( lpszAccount );
	ar.WriteString( lpszPassword );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnPlayerList( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD dwAuthKey;
	ar >> dwAuthKey;

	CUser* pUser;
	CMclAutoLock	Lock( g_UserMng.m_AddRemoveLock );

	pUser	= g_UserMng.GetUser( dpid );
	if( pUser && pUser->m_dwAuthKey == dwAuthKey )
		g_dpLoginSrvr.Send( lpBuf, uBufSize, dpid );
}

void CDPDatabaseClient::OnFail( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
	long lError;
	char lpszAccount[MAX_ACCOUNT]	= { 0, };
	ar >> lError;
	ar.ReadString( lpszAccount, MAX_ACCOUNT );
	switch( lError )
	{
		case ERROR_FLYFF_ACCOUNT:
			{
				CMclAutoLock	Lock( g_UserMng.m_AddRemoveLock );
				CUser* pUser	= g_UserMng.GetUser( lpszAccount );
				if( pUser )
				{
					pUser->m_bIllegal	= TRUE;
					g_dpLoginSrvr.DestroyPlayer( pUser->m_dpid );
					WriteLog( "OnFail(): ERROR_FLYFF_ACCOUNT, %s", lpszAccount );
				}
				else
				{
					WriteLog( "OnFail(): pUser is NULL, %s", lpszAccount );
				}
				break;
			}
	}
}

#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
void CDPDatabaseClient::OnLoginProtect( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
	char lpszAccount[MAX_ACCOUNT] = {0, };
	char lpszPlayer[MAX_PLAYER] = {0, };
	u_long idPlayer;
	BOOL bLogin;

	ar.ReadString( lpszAccount, MAX_ACCOUNT );
	ar.ReadString( lpszPlayer, MAX_PLAYER );
	ar >> idPlayer >> bLogin;

	CLoginProtect::GetInstance()->SetError( idPlayer, bLogin );

	if( bLogin )
	{
		CMclAutoLock	Lock( g_UserMng.m_AddRemoveLock );
		CUser* pUser	= g_UserMng.GetUser( dpid );
		if( pUser )
		{
			if( lstrcmp( lpszAccount, pUser->m_pKey ) == 0 )
			{
				pUser->m_idPlayer	= idPlayer;
				g_dpCoreClient.SendPreJoin( pUser->m_dwAuthKey, lpszAccount, idPlayer, lpszPlayer );	// o
				CLoginProtect::GetInstance()->SetNumPadId( dpid, TRUE );
			}
			else
				WriteLog( "OnLoginProtect(): Different lpszAccount, %s", lpszAccount );
		}
		else
			WriteLog( "OnLoginProtect(): dpid not found, %s", lpszAccount );
	}
	else
		g_dpLoginSrvr.SendLoginProtect( FALSE, dpid );
}

void CDPDatabaseClient::SendLoginProtect( const char* lpszAccount, const char* lpszPlayer, u_long idPlayer, int nBankPW, DPID dpid )
{
	BEFORESENDSOLE( ar, PACKETTYPE_LOGIN_PROTECT_CERT, dpid );
	ar.WriteString( lpszAccount );
	ar.WriteString( lpszPlayer );
	ar << idPlayer << nBankPW;
	SEND( ar, this, DPID_SERVERPLAYER );
}
#endif // __2ND_PASSWORD_SYSTEM

CDPDatabaseClient	g_dpDBClient;
