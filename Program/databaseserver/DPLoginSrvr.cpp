#include "StdAfx.h"
#include "dpaccountclient.h"
#include "DPLoginSrvr.h"
#include "MyTrace.h"
#include "..\_Network\Objects\Obj.h"
#include "misc.h"
#include <mmsystem.h>

extern	CMyTrace	g_MyTrace;
extern	CProject	prj;
extern	APP_INFO	g_appInfo;
extern	CDPAccountClient	g_dpAccountClient;

CDPLoginSrvr::CDPLoginSrvr()
{
	BEGIN_MSG;
	ON_MSG( PACKETTYPE_GETPLAYERLIST, OnGetPlayerList );
	ON_MSG( PACKETTYPE_CREATE_PLAYER, OnCreatePlayer );
	ON_MSG( PACKETTYPE_DEL_PLAYER, OnRemovePlayer );
	ON_MSG( PACKETTYPE_LEAVE, OnLeave );
	ON_MSG( PACKETTYPE_CLOSE_ERROR, OnCloseError );
#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
	ON_MSG( PACKETTYPE_LOGIN_PROTECT_CERT, OnLoginProtect );
#endif // __2ND_PASSWORD_SYSTEM
}

CDPLoginSrvr::~CDPLoginSrvr()
{

}

void CDPLoginSrvr::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_CREATEPLAYERORGROUP:
			{
				LPDPMSG_CREATEPLAYERORGROUP lpCreatePlayer	= (LPDPMSG_CREATEPLAYERORGROUP)lpMsg;
//				g_MyTrace.Add( CMyTrace::Key("Login" ), FALSE, "" );
				OnAddConnection( lpCreatePlayer->dpId );
				break;
			}
		case DPSYS_DESTROYPLAYERORGROUP:
			{
				LPDPMSG_DESTROYPLAYERORGROUP lpDestroyPlayer	= (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
//				g_MyTrace.Add( CMyTrace::Key("Login" ), TRUE, "" );
				OnRemoveConnection( lpDestroyPlayer->dpId );
				break;
			}
	}
}

void CDPLoginSrvr::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	static size_t	nSize	= sizeof(DPID);
	
	CAr ar( (LPBYTE)lpMsg + nSize, dwMsgSize - nSize );
	GETTYPE( ar );
	void ( theClass::*pfn )( theParameters )	=	GetHandler( dw );
	
	if( pfn ) {
		( this->*( pfn ) )( ar, *(UNALIGNED LPDPID)lpMsg, (LPBYTE)lpMsg + nSize + nSize, dwMsgSize - nSize - nSize );
	}
	else {
		TRACE( "Handler not found(%08x)\n", dw );
	}
}

DPID	s_dpid	= 0xFFFFFFFF;
void CDPLoginSrvr::OnAddConnection( DPID dpid )
{
	if( s_dpid == 0xFFFFFFFF )
		s_dpid	= dpid;
}

void CDPLoginSrvr::OnRemoveConnection( DPID dpid )
{
	if( s_dpid == dpid )
	{
		g_dpAccountClient.SendRemoveAllAccounts();
		s_dpid	= 0xFFFFFFFF;
	}
}

void CDPLoginSrvr::SendError( LONG lError, DPID dpid )
{
	BEFORESENDSOLE( ar, PACKETTYPE_ERROR, dpid );
	ar << lError;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPLoginSrvr::SendFail( long lError, const char* lpszAccount )
{
	BEFORESENDSOLE( ar, PACKETTYPE_FAIL, DPID_UNKNOWN );
	ar << lError;
	ar.WriteString( lpszAccount );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPLoginSrvr::SendCloseExistingConnection( const char* lpszAccount, LONG lError )
{
	BEFORESENDSOLE( ar, PACKETTYPE_CLOSE_EXISTING_CONNECTION, DPID_UNKNOWN );
	ar.WriteString( lpszAccount );
	ar << lError;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPLoginSrvr::SendOneHourNotify( const char* lpszAccount)
{
	BEFORESENDSOLE( ar, PACKETTYPE_ONE_HOUR_NOTIFY, DPID_UNKNOWN );
	ar.WriteString( lpszAccount );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPLoginSrvr::SendPlayerList( const char* lpszAccount, const char* lpszPassword, DPID dpid, DWORD dwAuthKey )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus	= g_DbManager.AllocRequest();
#ifdef __SECURITY_FIXES
	if(!prj.IsAllowedLetter(lpszAccount,FALSE) || 
		!prj.IsAllowedLetter(lpszPassword,FALSE))
	{
		return;
	}
#endif // __SECURITY_FIXES
	lpDbOverlappedPlus->dpid	= dpid;
	lpDbOverlappedPlus->nQueryMode	= SENDPLAYERLIST;
	lpDbOverlappedPlus->dwAuthKey	= dwAuthKey;
	strcpy( lpDbOverlappedPlus->AccountInfo.szAccount, lpszAccount );
	strcpy( lpDbOverlappedPlus->AccountInfo.szPassword, lpszPassword );
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGet, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPLoginSrvr::OnCreatePlayer( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
#ifdef __SECURITY_FIXES
	CAr arRead(lpBuf,uBufSize);
	char szAccount[MAX_ACCOUNT] = {0,};
	char szPassword[MAX_PASSWORD] = {0,};
	arRead.ReadString(szAccount,MAX_ACCOUNT);
	arRead.ReadString(szPassword,MAX_PASSWORD);
	if(!prj.IsAllowedLetter(szAccount,FALSE) || 
		!prj.IsAllowedLetter(szPassword,FALSE))
	{
		return;
	}
#endif // __SECURITY_FIXES
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid = dpid;
	lpDbOverlappedPlus->nQueryMode = CREATEPLAYER;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGet, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPLoginSrvr::OnRemovePlayer( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
#ifdef __REMOVE_PLAYER_0221
#ifdef __SECURITY_FIXES
	CAr arRead(lpBuf,uBufSize);
	char szAccount[MAX_ACCOUNT] = {0,};
	char szPassword[MAX_PASSWORD] = {0,};
	char szNo[MAX_PASSWORD] = {0,};

	arRead.ReadString( szAccount,MAX_ACCOUNT );
	arRead.ReadString( szPassword,MAX_PASSWORD );
	arRead.ReadString( szNo, MAX_PASSWORD );
	if(!prj.IsAllowedLetter(szAccount,FALSE) || 
		!prj.IsAllowedLetter(szPassword,FALSE) || 
		!prj.IsAllowedLetter(szNo,FALSE))
	{
		return;
	}
#endif // __SECURITY_FIXES
	CAr ar1;
	ar1 << PACKETTYPE_DEL_PLAYER;
	ar1.Write( lpBuf, uBufSize );
	ar1 << dpid;
	int nBlockSize;
	LPBYTE pBlock	= ar1.GetBuffer( &nBlockSize );
	g_dpAccountClient.Send( pBlock, nBlockSize, DPID_SERVERPLAYER );
#else	// __REMOVE_PLAYER_0221
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid = dpid;
	lpDbOverlappedPlus->nQueryMode = REMOVEPLAYER;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGet, 1, NULL, &lpDbOverlappedPlus->Overlapped );
#endif	// __REMOVE_PLAYER_0221
}

void CDPLoginSrvr::OnGetPlayerList( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
#ifdef __SECURITY_FIXES
	CAr arRead(lpBuf,uBufSize);
	DWORD dwAuthKey;
	arRead >> dwAuthKey;
	u_long uIdofMulti;
	arRead >> uIdofMulti;
	char szAccount[MAX_ACCOUNT] = {0,};
	char szPassword[MAX_PASSWORD] = {0,};
	arRead.ReadString( szAccount, MAX_ACCOUNT );
	arRead.ReadString( szPassword, MAX_PASSWORD );
	if(!prj.IsAllowedLetter(szAccount,FALSE) || 
		!prj.IsAllowedLetter(szPassword,FALSE))
	{
		return;
	}
#endif // __SECURITY_FIXES
	CAr ar1;
	ar1 << PACKETTYPE_GETPLAYERLIST;
	ar1.Write( lpBuf, uBufSize );
	ar1 << dpid;
	int nBlockSize;
	LPBYTE pBlock	= ar1.GetBuffer( &nBlockSize );
	g_dpAccountClient.Send( pBlock, nBlockSize, DPID_SERVERPLAYER );
}

void CDPLoginSrvr::OnLeave( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
#ifdef __SECURITY_FIXES
	CAr arRead(lpBuf,uBufSize);
	char szAccount[MAX_ACCOUNT] = {0,};
	arRead.ReadString( szAccount, MAX_ACCOUNT );
	if(!prj.IsAllowedLetter(szAccount,FALSE))
	{
		return;
	}
#endif // __SECURITY_FIXES
	char lpszAccount[MAX_ACCOUNT] = { 0, };
	ar.ReadString( lpszAccount, MAX_ACCOUNT );
#ifdef __SECURITY_FIXES
	if(!prj.IsAllowedLetter(lpszAccount,FALSE))
	{
		return;
	}
#endif // __SECURITY_FIXES
	g_dpAccountClient.SendRemoveAccount( lpszAccount );

	u_long idPlayer;
	ar >> idPlayer;
	if( idPlayer == 0 )
		return;

	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid	= dpid;
	lpDbOverlappedPlus->nQueryMode = SAVE_PLAY_TIME;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );

#ifdef __INTERNALSERVER
	time_t cur = time(NULL);
	WriteLog( "CDPLoginSrvr::OnLeave(%s) - %s\n", lpszAccount, ctime( &cur ));
#endif
}

void CDPLoginSrvr::OnCloseError( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
	char lpszAccount[MAX_ACCOUNT]	= { 0, };
	ar.ReadString( lpszAccount, MAX_ACCOUNT );
#ifdef __SECURITY_FIXES
	if(!prj.IsAllowedLetter(lpszAccount,FALSE))
	{
		return;
	}
#endif // __SECURITY_FIXES
	g_dpAccountClient.SendRemoveAccount( lpszAccount );

#ifdef __INTERNALSERVER
	time_t cur = time(NULL);
	WriteLog( "CDPLoginSrvr::OnCloseError(%s) - %s\n", lpszAccount, ctime( &cur ));
#endif
}

#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
void CDPLoginSrvr::OnLoginProtect( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid = dpid;
	lpDbOverlappedPlus->nQueryMode = QM_LOGIN_PROTECT;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGet, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPLoginSrvr::SendLoginProtect( const char* lpszAccount, const char* lpszPlayer, u_long idPlayer, BOOL bLogin, DPID dpId )
{
	BEFORESENDSOLE( ar, PACKETTYPE_LOGIN_PROTECT_CERT, dpId );
	ar.WriteString( lpszAccount );
	ar.WriteString( lpszPlayer );
	ar << idPlayer << bLogin;
	SEND( ar, this, DPID_ALLPLAYERS );
}
#endif // __2ND_PASSWORD_SYSTEM

CDPLoginSrvr	g_dpLoginSrvr;