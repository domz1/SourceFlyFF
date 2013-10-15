#include "stdafx.h"
#include "dpsrvr.h"
#include "dpadbill.h"
#include "dpdbsrvr.h"
#include "account.h"
#include "misc.h"

#include "dbmanager.h"
extern	CDbManager	g_DbManager;

extern	CDPSrvr		g_dpSrvr;
extern	CAccountMng		g_AccountMng;

/*
#ifdef __S0114_RELOADPRO
void ReloadLog( LPCTSTR lpszFormat, ... )
{
	TCHAR szBuffer[4096];
	
	va_list args;
	va_start( args, lpszFormat );
	_vsntprintf( szBuffer, 4095, lpszFormat, args );
	va_end( args );
	
	TRACE( szBuffer );
	
	FILE *fp = fopen( "ReloadLog.txt", "a" );
	if( fp )
	{
		SYSTEMTIME	time;
		GetLocalTime( &time );
		fprintf( fp, "%02d/%02d-%02d:%02d:%02d\t%s\n", 
			time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond,
			szBuffer );
		fclose( fp );
	}
}
#endif // __S0114_RELOADPRO
*/
CDPDBSrvr::CDPDBSrvr()
{
	BEGIN_MSG;
	ON_MSG( PACKETTYPE_MYREG, OnAddConnection );
	ON_MSG( PACKETTYPE_REMOVE_ACCOUNT, OnRemoveAccount );
	ON_MSG( PACKETTYPE_GETPLAYERLIST, OnGetPlayerList );
#ifdef __REMOVE_PLAYER_0221
	ON_MSG( PACKETTYPE_DEL_PLAYER, OnRemovePlayer );
#endif	// __REMOVE_PLAYER_0221
	ON_MSG( PACKETTYPE_JOIN, OnJoin );
	ON_MSG( PACKETTYPE_REMOVE_ALLACCOUNTS, OnRemoveAllAccounts );
	ON_MSG( PACKETTYPE_BUYING_INFO, OnBuyingInfo );
//	ON_MSG( PACKETTYPE_OPEN_BATTLESERVER, OnOpenBattleServer );
//	ON_MSG( PACKETTYPE_CLOSE_BATTLESERVER, OnCloseBattleServer );
#ifdef __SERVERLIST0911
	ON_MSG( PACKETTYPE_SERVER_ENABLE, OnServerEnable );
#endif	// __SERVERLIST0911
/*
#ifdef __S0114_RELOADPRO
	ON_MSG( PACKETTYPE_RELOAD_PROJECT, OnCompleteReloadProject );
#endif // __S0114_RELOADPRO
*/
}

CDPDBSrvr::~CDPDBSrvr()
{

}

void CDPDBSrvr::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpId )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_CREATEPLAYERORGROUP:
//			{
//				LPDPMSG_CREATEPLAYERORGROUP lpCreatePlayer	= (LPDPMSG_CREATEPLAYERORGROUP)lpMsg;
				break;
//			}
		case DPSYS_DESTROYPLAYERORGROUP:
			{
				LPDPMSG_DESTROYPLAYERORGROUP lpDestroyPlayer	= (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
				OnRemoveConnection( lpDestroyPlayer->dpId );
				break;
			}
	}
}

void CDPDBSrvr::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpId )
{
	CAr ar( (LPBYTE)lpMsg, dwMsgSize );
	GETTYPE( ar );
	void ( theClass::*pfn )( theParameters )
		=	GetHandler( dw );
	ASSERT( pfn );
	( this->*( pfn ) )( ar, dpId, (LPBYTE)lpMsg, (u_long)dwMsgSize );
}

void CDPDBSrvr::OnAddConnection( CAr & ar, DPID dpid, LPBYTE, u_long )
{
	DWORD dwIdofServer;
	ar >> dwIdofServer;
	g_AccountMng.AddIdofServer( dpid, dwIdofServer );

#ifdef __SERVERLIST0911
	g_dpSrvr.EnableServer( (DWORD)-1, dwIdofServer, 1L );
	char lpOutputString[100]	= { 0, };
	sprintf( lpOutputString, "EnableServer, %d, %d, %d", -1, dwIdofServer, 1L );
	OutputDebugString( lpOutputString );
#endif	// __SERVERLIST0911
}

void CDPDBSrvr::OnRemoveConnection( DPID dpid )
{
#ifdef __SERVERLIST0911
	DWORD dwIdofServer	= g_AccountMng.RemoveIdofServer( dpid );
	if( dwIdofServer == (DWORD)-1 )
		return;
	g_dpSrvr.EnableServer( (DWORD)-1, dwIdofServer, 0L );
	char lpOutputString[100]	= { 0, };
	sprintf( lpOutputString, "EnableServer, %d, %d, %d", -1, dwIdofServer, 0L );
	OutputDebugString( lpOutputString );
#else	// __SERVERLIST0911
	g_AccountMng.RemoveIdofServer( dpid );
#endif	// __SERVERLIST0911
}

void CDPDBSrvr::OnRemoveAccount( CAr & ar, DPID dpid, LPBYTE, u_long )
{
	TCHAR lpszAccount[MAX_ACCOUNT];
	ar.ReadString( lpszAccount, MAX_ACCOUNT );
	g_AccountMng.RemoveAccount( lpszAccount );
}

#ifdef __REMOVE_PLAYER_0221
void CDPDBSrvr::OnRemovePlayer( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
	char szAccount[MAX_ACCOUNT]	= { 0,}, szTemp[255]	= { 0,};
	ar.ReadString( szAccount, MAX_ACCOUNT );
	ar.ReadString( szTemp, 255 );	// szPassword
	ar.ReadString( szTemp, 255 );	// szNo
	u_long idPlayer;
	ar >> idPlayer;		// x
	DWORD dwAuthKey;
	ar>> dwAuthKey;		// o
#ifdef __RT_1025
	size_t nSize; u_long uTemp;
	ar >> nSize;
	for( size_t i=0; i<nSize; i++ )
		ar >> uTemp;	// 그냥 받기만 한다.(쓸모없음)
#endif // __RT_1025

	CMclAutoLock	Lock( g_AccountMng.m_AddRemoveLock );
	CAccount* pAccount	= g_AccountMng.GetAccount( szAccount );
	if( pAccount && pAccount->m_dwAuthKey == dwAuthKey && pAccount->m_cbRef == 1/*step : select character*/ )
	{
		Send( (LPVOID)lpBuf, (DWORD)uBufSize, dpid );
		OutputDebugString( "ACCOUNTSERVER.EXE\t// PACKETTYPE_DEL_PLAYER" );
	}
}
#endif	// __REMOVE_PLAYER_0221

void CDPDBSrvr::OnGetPlayerList( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD dwAuthKey;
	char lpszAccount[MAX_ACCOUNT]	= { 0, };

	ar >> dwAuthKey;	// o
	u_long uIdofMulti;
	ar >> uIdofMulti;
	ar.ReadString( lpszAccount, MAX_ACCOUNT );	// o

	CMclAutoLock	Lock( g_AccountMng.m_AddRemoveLock );

	CAccount* pAccount	= g_AccountMng.GetAccount( lpszAccount );
	if( pAccount && pAccount->m_dwAuthKey == dwAuthKey && pAccount->m_cbRef == 0 )
	{
		pAccount->m_cbRef++;
		Send( (LPVOID)lpBuf, (DWORD)uBufSize, dpid );
		pAccount->m_fRoute	= TRUE;
		pAccount->m_dwIdofServer	= g_AccountMng.GetIdofServer( dpid );
		pAccount->m_uIdofMulti	= uIdofMulti;
		u_long uId	= pAccount->m_dwIdofServer * 100 + uIdofMulti;
		map<u_long, LPSERVER_DESC>::iterator i	= g_dpSrvr.m_2ServersetPtr.find( uId );
		if( i != g_dpSrvr.m_2ServersetPtr.end() )
		{
			g_dpSrvr.DestroyPlayer( pAccount->m_dpid1, pAccount->m_dpid2 );
		}
	}
	else
	{
		WriteLog( "^OnGetPlayerList(), %s", lpszAccount );
		SendFail( ERROR_FLYFF_ACCOUNT, lpszAccount, dpid );		// ^
	}
}

void CDPDBSrvr::OnJoin( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD dwAuthKey;
	char lpszAccount[MAX_ACCOUNT]	= { 0, };

	ar >> dwAuthKey;	// o
	ar.ReadString( lpszAccount, MAX_ACCOUNT );	// o

	CMclAutoLock	Lock( g_AccountMng.m_AddRemoveLock );
	CAccount* pAccount	= g_AccountMng.GetAccount( lpszAccount );
	if( pAccount && pAccount->m_dwAuthKey == dwAuthKey && pAccount->m_cbRef == 1 )
	{
		pAccount->m_cbRef++;
#ifdef __BILLING0712
		*(UNALIGNED DWORD*)( lpBuf + uBufSize - sizeof(DWORD) )	= pAccount->m_dwBillingClass;
#endif	// __BILLING0712
#if __VER >= 14 // __PCBANG
		*(UNALIGNED DWORD*)( lpBuf + uBufSize - sizeof(DWORD) )	= g_AccountMng.PopPCBangPlayer( dwAuthKey );
#endif // __PCBANG
		Send( (LPVOID)lpBuf, (DWORD)uBufSize, dpid );
		OutputDebugString( "ACCOUNTSERVER.EXE\t// PACKETTYPE_JOIN" );
		
		// 동접을 보낸다.
		u_long uId	= pAccount->m_dwIdofServer * 100 + pAccount->m_uIdofMulti;
		map<u_long, LPSERVER_DESC>::iterator i	= g_dpSrvr.m_2ServersetPtr.find( uId );
		if( i != g_dpSrvr.m_2ServersetPtr.end() )
		{
			long lCount	= InterlockedIncrement( &i->second->lCount );
			g_dpSrvr.SendPlayerCount( uId, lCount );
		}
	}
}

void CDPDBSrvr::SendCloseExistingConnection( const char* lpszAccount, LONG lError )
{
	BEFORESEND( ar, PACKETTYPE_CLOSE_EXISTING_CONNECTION );
	ar.WriteString( lpszAccount );
/*
#if defined( __BILLING0712 ) || defined( __S0114_RELOADPRO )
	ar << lError;
#endif // defined( __BILLING0712 ) || defined( __S0114_RELOADPRO )
*/
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPDBSrvr::SendPlayerCount( void )
{
#ifdef __LOG_PLAYERCOUNT_CHANNEL
	for( int nChannel = 0; nChannel < (int)( m_vecstrChannelAccount.size()+1 ); nChannel++ )
#endif // __LOG_PLAYERCOUNT_CHANNEL
	{

		DWORD dwParent	= NULL_ID;
		int cbSize;
		int anCount[64];
		DPID adpid[64];
	
		memset( (void*)adpid, 0xff, sizeof(DPID) * 64 );
		DWORD dwIdofServer;
		g_AccountMng.m_AddRemoveLock.Enter();
		for( map<DWORD, DWORD>::iterator i2 = g_AccountMng.m_2IdofServer.begin(); i2 != g_AccountMng.m_2IdofServer.end(); ++i2 )
		{
			dwIdofServer	= i2->second;
			if( dwIdofServer >= 0 && dwIdofServer < 64 )
				adpid[dwIdofServer]		= i2->first;
		}
		g_AccountMng.m_AddRemoveLock.Leave();

		LPSERVER_DESC pServer;
		for( int i = 0; i < (int)( g_dpSrvr.m_dwSizeofServerset ); i++ )
		{
			pServer		= &g_dpSrvr.m_aServerset[i];
			if( pServer->dwParent == NULL_ID )
			{
	//			if( dwParent != NULL_ID )
				if( dwParent >= 0 && dwParent < 64 && adpid[dwParent] != DPID_UNKNOWN )
				{
					BEFORESEND( ar, PACKETTYPE_PLAYER_COUNT );
#ifdef __LOG_PLAYERCOUNT_CHANNEL
					ar << nChannel;
#endif // __LOG_PLAYERCOUNT_CHANNEL
					ar <<cbSize;
					ar.Write( (void*)anCount, sizeof(int) * cbSize );
					SEND( ar, this, adpid[dwParent] );
				}
				dwParent	= pServer->dwID;
				cbSize	= 0;
			}
			else
			{
#ifdef __LOG_PLAYERCOUNT_CHANNEL
				if( nChannel == 0 )
#endif // __LOG_PLAYERCOUNT_CHANNEL
					anCount[cbSize++]		= pServer->lCount;
#ifdef __LOG_PLAYERCOUNT_CHANNEL
				else
				{
					CMclAutoLock	Lock( g_AccountMng.m_AddRemoveLock );
					long lCount = 0;
					map<string, CAccount*> mapTemp = g_AccountMng.GetMapAccount();
					for( map<string, CAccount*>::iterator it=mapTemp.begin(); it!=mapTemp.end(); it++ )
					{
						if( it->first.find( m_vecstrChannelAccount[nChannel-1] ) != -1
							&& it->second->m_dwIdofServer == pServer->dwParent
							&& it->second->m_uIdofMulti == pServer->dwID )
						{
							lCount++;
						}
					}
					anCount[cbSize++] = lCount;
				}
#endif // __LOG_PLAYERCOUNT_CHANNEL
			}
		}
	//	if( dwParent != NULL_ID )
		if( dwParent >= 0 && dwParent < 64 && adpid[dwParent] != DPID_UNKNOWN )
		{
			BEFORESEND( ar, PACKETTYPE_PLAYER_COUNT );
#ifdef __LOG_PLAYERCOUNT_CHANNEL
			ar << nChannel;
#endif // __LOG_PLAYERCOUNT_CHANNEL
			ar << cbSize;
			ar.Write( (void*)anCount, sizeof(int) * cbSize );
			SEND( ar, this, adpid[dwParent] );
		}
	}
}

void CDPDBSrvr::OnRemoveAllAccounts( CAr & ar, DPID dpid, LPBYTE, u_long )
{
	g_AccountMng.RemoveIdofServer( dpid, FALSE );
}


void CDPDBSrvr::SendFail( long lError, const char* lpszAccount, DPID dpid )
{
	BEFORESEND( ar, PACKETTYPE_FAIL );
	ar << lError;
	ar.WriteString( lpszAccount );
	SEND( ar, this, dpid );
}




void CDPDBSrvr::SendOneHourNotify( CAccount* pAccount )
{
	BEFORESEND( ar, PACKETTYPE_ONE_HOUR_NOTIFY );
	ar.WriteString( pAccount->m_lpszAccount );
	
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPDBSrvr::SendBuyingInfo( PBUYING_INFO2 pbi2 )
{
	BEFORESEND( ar, PACKETTYPE_BUYING_INFO );
	ar.Write( pbi2, sizeof(BUYING_INFO2) );
	SEND( ar, this, DPID_ALLPLAYERS );
}


void CDPDBSrvr::OnBuyingInfo( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
	BUYING_INFO2	bi2;
	SERIALNUMBER iSerialNumber;
	ar.Read( (void*)&bi2, sizeof(BUYING_INFO2) );
	ar >> iSerialNumber;

	CBuyingInfoMng::GetInstance()->m_AddRemoveLock.Enter();
	PBUYING_INFO3 pbi3	= CBuyingInfoMng::GetInstance()->Get( bi2.dwKey );
	if( pbi3 )
	{
		static char lpOutputString[260]	= { 0, };
		sprintf( lpOutputString, "ACCOUNTSERVER.EXE\t// Recv from trans\t// dwRetVal = %d", bi2.dwRetVal );
		OutputDebugString( lpOutputString );

		CDPAdbill::GetInstance()->Send( &bi2, sizeof(BUYING_INFO), bi2.dpid );
		CBuyingInfoMng::GetInstance()->Remove( bi2.dwKey );
		safe_delete( pbi3 );
		/*
		bi2.szBxaid		// 구매 번호
		bi2.dwItemId	// 아이템 인덱스
		bi2.dwItemNum	// 아이템 개수
		bi2.dwServerIndex	// 서버 인덱스
		bi2.dwPlayerId		// 플레이어 식별자
		bi2.dwRetVal	// 지급 성공 여부, 성공 : 1, 실패 : 0
		iSerialNumber		// 시리얼 번호
		*/

		LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.m_pDbIOData->Alloc();
		memcpy( lpDbOverlappedPlus->lpBuf, (LPBYTE)lpBuf + sizeof(DWORD), uBufSize - sizeof(DWORD) );
		lpDbOverlappedPlus->uBufSize	= uBufSize - sizeof(DWORD);
		lpDbOverlappedPlus->nQueryMode	= LOG_SM_ITEM;
		PostQueuedCompletionStatus( g_DbManager.m_hDbCompletionPort, 1, NULL, &lpDbOverlappedPlus->Overlapped );
	}
	else
	{
		// 치명적 오류 : 어카운트 서버에는 지급 명령에 대한 정보가 없다.
	}
	CBuyingInfoMng::GetInstance()->m_AddRemoveLock.Leave();
	TRACE( "RECV PACKETTYPE_BUYING_INFO FROM TRANS\n" );
}

/*
void CDPDBSrvr::OnOpenBattleServer( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
	g_dpSrvr.EnableServer( (DWORD)-1, (DWORD)0, 1L );
	TRACE( "PACKETTYPE_OPEN_BATTLESERVER\n" );
}

void CDPDBSrvr::OnCloseBattleServer( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
	g_dpSrvr.EnableServer( (DWORD)-1, (DWORD)0, 0L );
	TRACE( "PACKETTYPE_CLOSE_BATTLESERVER\n" );
}
*/
void CDPDBSrvr::SendLogSMItem()
{
}

/*
#ifdef __S0114_RELOADPRO
void CDPDBSrvr::SendReloadAccount()
{
	BEFORESEND( ar, PACKETTYPE_RELOAD_PROJECT );
	ar << g_DbManager.m_OutAccount_List.size();
	for( SET_STRING::iterator i = g_DbManager.m_OutAccount_List.begin() ; i != g_DbManager.m_OutAccount_List.end() ; ++i )
	{
		ar.WriteString( i->data() );
	}	
	SEND( ar, this, DPID_ALLPLAYERS );

	for( i = g_DbManager.m_OutAccount_List.begin() ; i != g_DbManager.m_OutAccount_List.end() ; ++i )
	{
		g_dpSrvr.CloseExistingConnection( i->data(), ERROR_FLYFF_DB_JOB_ING );
	}	
}

void CDPDBSrvr::OnCompleteReloadProject( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD dwDBServer;
	ar >> dwDBServer;
	char szMessage[1024];
	sprintf( szMessage, "CompleteReloadProject -> %d ", dwDBServer );
	ReloadLog( szMessage );
}
#endif // __S0114_RELOADPRO
*/

#ifdef __SERVERLIST0911
void CDPDBSrvr::OnServerEnable( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
	int nServer, nMulti;
	long lEnable;
	ar >> nServer >> nMulti >> lEnable;
	g_dpSrvr.EnableServer( (DWORD)nServer, (DWORD)nMulti, lEnable );

	char lpOutputString[100]	= { 0, };
	sprintf( lpOutputString, "EnableServer, %d, %d, %d", nServer, nMulti, lEnable );
	OutputDebugString( lpOutputString );
}
#endif	// __SERVERLIST0911

CDPDBSrvr	g_dpDbSrvr;