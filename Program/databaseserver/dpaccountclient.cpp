#include "StdAfx.h"
#include "dploginsrvr.h"
extern	CDPLoginSrvr	g_dpLoginSrvr;

#include "dpcoresrvr.h"
extern	CDPCoreSrvr		g_dpCoreSrvr;

extern	APP_INFO	g_appInfo;

#include "dpaccountclient.h"
#include "serialnumber.h"

CDPAccountClient::CDPAccountClient()
{
/*
#ifdef __S0114_RELOADPRO
	m_OutAccount_List.clear();
	m_tMemDelete = CTime::GetCurrentTime();
	m_bMemDelete = FALSE;
#endif // __S0114_RELOADPRO
*/
	BEGIN_MSG;
	ON_MSG( PACKETTYPE_GETPLAYERLIST, OnGetPlayerList );
#ifdef __REMOVE_PLAYER_0221
	ON_MSG( PACKETTYPE_DEL_PLAYER, OnRemovePlayer );
#endif	// __REMOVE_PLAYER_0221
	ON_MSG( PACKETTYPE_JOIN, OnJoin );
	ON_MSG( PACKETTYPE_PLAYER_COUNT, OnPlayerCount );
	ON_MSG( PACKETTYPE_CLOSE_EXISTING_CONNECTION, OnCloseExistingConnection );
	ON_MSG( PACKETTYPE_ONE_HOUR_NOTIFY, OnOneHourNotify )
	ON_MSG( PACKETTYPE_FAIL, OnFail );
	ON_MSG( PACKETTYPE_BUYING_INFO, OnBuyingInfo );
/*
#ifdef __S0114_RELOADPRO
	ON_MSG( PACKETTYPE_RELOAD_PROJECT, OnReloadProject );
#endif // __S0114_RELOADPRO
*/
}

CDPAccountClient::~CDPAccountClient()
{
}

void CDPAccountClient::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_DESTROYPLAYERORGROUP:
				break;
	}
}

void CDPAccountClient::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	CAr ar( (LPBYTE)lpMsg, dwMsgSize );
	GETTYPE( ar );
	void ( theClass::*pfn )( theParameters )
		=	GetHandler( dw );
	ASSERT( pfn );
	( this->*( pfn ) )( ar, (LPBYTE)lpMsg + sizeof(DWORD), dwMsgSize - sizeof(DWORD) );
}

#ifdef __SERVERLIST0911
void CDPAccountClient::SendServerEnable( int nMulti, long lEnable )
{
	BEFORESEND( ar, PACKETTYPE_SERVER_ENABLE  );
	ar << (int)g_appInfo.dwSys << nMulti << lEnable;
	SEND( ar, this, DPID_SERVERPLAYER );
}
#endif	// __SERVERLIST0911

void CDPAccountClient::SendAddIdofServer( DWORD dwIdofServer )
{
	BEFORESEND( ar, PACKETTYPE_MYREG );
	ar << dwIdofServer;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPAccountClient::SendRemoveAccount( char* lpszAccount )
{
	BEFORESEND( ar, PACKETTYPE_REMOVE_ACCOUNT );
	ar.WriteString( lpszAccount );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPAccountClient::SendRemoveAllAccounts( void )
{
	BEFORESEND( ar, PACKETTYPE_REMOVE_ALLACCOUNTS );
	SEND( ar, this, DPID_SERVERPLAYER );
}

#ifdef __REMOVE_PLAYER_0221
void CDPAccountClient::OnRemovePlayer( CAr & ar, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode = REMOVEPLAYER;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGet, 1, NULL, &lpDbOverlappedPlus->Overlapped );
/*
	account
	password
	szNo
	idPlayer
	dwAuthKey
	dpid
*/
}
#endif	// __REMOVE_PLAYER_0221

void CDPAccountClient::OnGetPlayerList( CAr & ar, LPBYTE, u_long )
{
	DWORD dwAuthKey;
	ar >> dwAuthKey;
	u_long uIdofMulti;
	ar >> uIdofMulti;
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	ar.ReadString( lpDbOverlappedPlus->AccountInfo.szAccount, MAX_ACCOUNT );
	ar.ReadString( lpDbOverlappedPlus->AccountInfo.szPassword, MAX_PASSWORD );
	ar >> lpDbOverlappedPlus->dpid;
	lpDbOverlappedPlus->nQueryMode	= SENDPLAYERLIST;
	lpDbOverlappedPlus->dwAuthKey	= dwAuthKey;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGet, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPAccountClient::OnJoin( CAr & ar, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= JOIN;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGet, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPAccountClient::OnPlayerCount( CAr & ar, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= SAVECONCURRENTUSERNUMBER;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPAccountClient::OnCloseExistingConnection( CAr & ar, LPBYTE lpBuf, u_long uBufSize )
{
	char lpszAccount[MAX_ACCOUNT]	= { 0, };
	LONG lError = 0;

	ar.ReadString( lpszAccount, MAX_ACCOUNT );
/*
#if defined( __BILLING0712 ) || defined( __S0114_RELOADPRO )
	ar >> lError;
#endif // defined( __BILLING0712 ) || defined( __S0114_RELOADPRO )
*/
	g_dpLoginSrvr.SendCloseExistingConnection( lpszAccount, lError );
}


void CDPAccountClient::OnOneHourNotify( CAr & ar, LPBYTE lpBuf, u_long uBufSize )
{
	char lpszAccount[MAX_ACCOUNT]	= { 0, };
	ar.ReadString( lpszAccount, MAX_ACCOUNT );
	g_dpLoginSrvr.SendOneHourNotify( lpszAccount );
}

void CDPAccountClient::OnFail( CAr & ar, LPBYTE, u_long )
{
	long lError;
	char lpszAccount[MAX_ACCOUNT]	= { 0, };
	ar >> lError;
	ar.ReadString( lpszAccount, MAX_ACCOUNT );
	switch( lError )
	{
		case ERROR_FLYFF_ACCOUNT:
			{
				g_dpLoginSrvr.SendFail( lError, lpszAccount );
				break;
			}
	}
}

void CDPAccountClient::OnBuyingInfo( CAr & ar, LPBYTE lpBuf, u_long uBufSize )	// 2
{
	OutputDebugString( "DATABASESERVER.EXE\t// recv from account\n" );

	BUYING_INFO2 bi2;
	ar.Read( &bi2, sizeof(BUYING_INFO2) );
	if( bi2.dwServerIndex == g_appInfo.dwSys )
		g_dpCoreSrvr.SendBuyingInfo( &bi2 );
}

void CDPAccountClient::SendBuyingInfo( PBUYING_INFO2 pbi2, SERIALNUMBER iSerialNumber )
{
	BEFORESEND( ar, PACKETTYPE_BUYING_INFO );
	ar.Write( (void*)pbi2, sizeof(BUYING_INFO2) );
	ar << iSerialNumber;
	SEND( ar, this, DPID_SERVERPLAYER );
}

/*
#ifdef __S0114_RELOADPRO
void CDPAccountClient::OnReloadProject( CAr& ar, LPBYTE lpBuf, u_long uBufSize )
{
	SET_STRING::iterator it;
	char szAccount[MAX_ACCOUNT];
	int nCount;
	ar >> nCount;

	g_DbManager.m_AddRemoveLock.Enter();
	for( int i = 0 ; i < nCount ; ++i )
	{
		ZeroMemory( szAccount, sizeof( szAccount ) );
		ar.ReadString( szAccount );
		it = m_OutAccount_List.find( szAccount );
		if( it == m_OutAccount_List.end() )
		{
			m_OutAccount_List.insert( szAccount );
			CTimeSpan timeSpan( 0, 0, 1, 0 );
			m_tMemDelete = CTime::GetCurrentTime() + timeSpan;
			m_bMemDelete = TRUE;
		}
	}
	g_DbManager.m_AddRemoveLock.Leave();
}

void CDPAccountClient::SendCompleteReloadProject()
{
	BEFORESEND( ar, PACKETTYPE_RELOAD_PROJECT );
	ar << g_appInfo.dwSys;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPAccountClient::ReloadProject( )
{
	SET_STRING::iterator it;
	if( m_bMemDelete && m_tMemDelete < CTime::GetCurrentTime() )
	{
		g_DbManager.m_AddRemoveLock.Enter();
		// 여기서 메모리 삭제
		m_bMemDelete = FALSE;
		ACCOUNT_CACHE* AccountCache;
		map<string, ACCOUNT_CACHE*>::iterator ia;
		for( it = m_OutAccount_List.begin() ; it != m_OutAccount_List.end() ; ++it )
		{
			ia = g_DbManager.m_2Account.find( it->data() );
			if( ia != g_DbManager.m_2Account.end() )
			{
				AccountCache = ia->second;
				AccountCache->Clear();
				g_DbManager.m_pAccountMemPooler->Free( AccountCache );
				// m_2Account.erase 를 호출해야 될 것 같음 
			}
		}
		m_OutAccount_List.clear();
		g_DbManager.m_AddRemoveLock.Leave();
		SendCompleteReloadProject();
	}
}
#endif // __S0114_RELOADPRO
*/
CDPAccountClient	g_dpAccountClient;