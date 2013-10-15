#include "stdafx.h"
#include "dbmanager.h"
#include "account.h"
#include "BillingMgr.h"
#include <afxdisp.h>
#include "lang.h"

CDbManager	g_DbManager;

extern	CAccountMng		g_AccountMng;
#include "dpdbsrvr.h"
extern	CDPDBSrvr	g_dpDbSrvr;
///////////////////////////////////////////////////////////////////////////////////////////////////
// CDbManager
///////////////////////////////////////////////////////////////////////////////////////////////////
CDbManager::CDbManager()
{
	m_pDbIOData	= new CMemPool<DB_OVERLAPPED_PLUS>(512);
	m_bTracking = FALSE;   
	m_bLogItem  = TRUE;
#ifdef __INTERNALSERVER
	m_bLogItem = FALSE;
#endif

	m_szLoginPWD[0] = '\0';
}

CDbManager::~CDbManager()
{
	SAFE_DELETE( m_pDbIOData );
}

HANDLE s_hHandle	= (HANDLE)NULL;

void CDbManager::CreateDbWorkers( void )
{
	s_hHandle	= CreateEvent( NULL, FALSE, FALSE, NULL );

	m_hDbCompletionPort		= CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );
	ASSERT( m_hDbCompletionPort );
	for( int i = 0; i < DEFAULT_DB_WORKER_THREAD_NUM; i++ )
	{
		HANDLE hThread	= chBEGINTHREADEX( NULL, 0, DbWorkerThread, (LPVOID)this, 0, NULL );
		ASSERT( hThread );
		m_hDbWorkerThreadTerminate[i]	= hThread;

		if( WaitForSingleObject( s_hHandle, SEC( 3 ) ) == WAIT_TIMEOUT )
			OutputDebugString( "ACCOUNTSERVER.EXE\t// TIMEOUT\t// ODBC" );
	}

	CloseHandle( s_hHandle );
}

void CDbManager::CloseDbWorkers( void )
{
	for( int i = 0; i < DEFAULT_DB_WORKER_THREAD_NUM; i++ )
		PostQueuedCompletionStatus( m_hDbCompletionPort, 0, NULL, NULL );
	
	WaitForMultipleObjects( DEFAULT_DB_WORKER_THREAD_NUM, m_hDbWorkerThreadTerminate, TRUE, INFINITE );
	
	CLOSE_HANDLE( m_hDbCompletionPort );
	
	for( i = 0; i < DEFAULT_DB_WORKER_THREAD_NUM; i++ ) {
		CLOSE_HANDLE( m_hDbWorkerThreadTerminate[i] );
	}
	
	SAFE_DELETE( m_pDbIOData );
}

void CDbManager::SetTracking( BOOL bEnable )
{
	m_bTracking = bEnable;  
	if( GetLanguage() == LANG_TWN )
		m_bTracking	= TRUE;
}

void CDbManager::SetLogging( BOOL bEnable )
{
	m_bLogItem = bEnable;
}

void CDbManager::GetStrTime( CTime *pTime, char *strbuf )
{
	char sYear[5]	= { 0, };
	char sMonth[3]	= { 0, };
	char sDay[3]	= { 0, };
	char sHour[3]	= { 0, };
	char sMin[3]	= { 0, };

	strncpy( sYear, strbuf, 4 );
	strncpy( sMonth, strbuf + 4, 2 );
	strncpy( sDay, strbuf + 6, 2 );
	strncpy( sHour, strbuf + 8, 2 );
	strncpy( sMin, strbuf + 10, 2 );

	*pTime	= CTime( atoi( sYear ), atoi( sMonth ), atoi( sDay ), atoi( sHour ), atoi( sMin ), 0 );
}

void CDbManager::DBQryAccount( char* qryAccount, char* Gu, char* szAccount )
{
	sprintf( qryAccount, "LOGINJOIN_STR '%s', '%s'", Gu, szAccount );
}

void CDbManager::DBQryLogSMItem( char* szSql, const LOG_SM_ITEM_INFO & info)
{
	CString strItem	= info.pszItem_Name;
	strItem.Replace( '\'', ' ' );

	sprintf( szSql, "LOG_STR '%s',@im_idPlayer='%07d',@iserverindex='%02d',@im_nExp1=%d,@im_nLevel=%d,"
		"@im_nJob=%d,@im_nJobLv=%d,@im_nFlightLv=%d,@im_nStr=%d,@im_nDex=%d,"
		"@im_nInt=%d,@im_nSta=%d,@im_nRemainGP=%d,@im_nRemainLP=%d,@iState='%s',"
		"@idwWorldID=%d,@ikilled_m_szName='%s',@im_vPos_x=%f,@im_vPos_y=%f,@im_vPos_z=%f,"
		"@iattackPower=%d,@imax_HP=%d,@im_GetszName='%s',@im_dwGold=%d,@im_nRemainGold=%d,"
		"@iItem_UniqueNo=%d,@iItem_Name='%s',@iItem_durability=%d,@iItem_count=%d,@iItem_AddLv=%d,"
		"@iStart_Time='%s',@iTotalPlayTime=%d,@iremoteIP='%s',@iQuest_Index=%d,@ikilled_m_idPlayer='%02d'",
		info.pszType, info.uidPlayer, info.nserverindex, info.nExp1, info.nLevel,
		info.nJob, info.JobLv, info.nFlightLv, info.nStr, info.nDex,
		info.nInt, info.nSta, info.nRemainGP, info.nRemainLP, info.pszState,
		info.nWorldID, info.pszkilled_szName, info.vPos_x, info.vPos_y, info.vPos_z,
		info.nAttackPower, info.nMax_HP, info.pszGetidPlayer, info.dwGold, info.nRemainGold,
//		info.nItem_UniqueNo, info.pszItem_Name, info.nItem_durability, info.nItem_count, info.nItemAddLv,
		info.nItem_UniqueNo, (LPCSTR)strItem, info.nItem_durability, info.nItem_count, info.nItemAddLv,
		info.pszStart_Time, info.nTotalPlayerTime, info.pszRemoteIP, info.nQuest_Index, info.uKill_idPlayer );

	if( strlen(szSql) > 4096 )
		ASSERT(0);
}

/*
void CDbManager::DBQryLog( char* qryLog, char* Gu, u_long idPlayer, int nserverindex, int nExp1, int nLevel,
						  int nJob, int JobLv, int FlightLv, int nStr, int nDex,
						  int nInt, int nSta, int nRemainGP, int nRemainLP, char szState,
						  int WorldID, char* szkilled_szName, float vPos_x, float vPos_y, float vPos_z,
						  int nAttackPower, int nMax_HP, char *uGetidPlayer, DWORD dwGold, int nRemainGold,
						  int nItem_UniqueNo, char *szItem_Name, int Item_durability, int Item_count, int ItemAddLv,
						  char* szStart_Time, int TotalPlayerTime, char *szRemoteIP, int nQuest_Index, u_long uKill_idPlayer )
{
	sprintf( qryLog, "LOG_STR '%s','%06d','%02d',%d,%d,  %d,%d,%d,%d,%d,  %d,%d,%d,%d,'%c',  %d,'%s',%f,%f,%f,  %d,%d,'%s',%d,%d,  %d,'%s',%d,%d,%d,  '%s',%d,'%s',%d,'%02d'",
		Gu, idPlayer, nserverindex, nExp1, nLevel,
		nJob, JobLv, FlightLv, nStr, nDex,
		nInt, nSta, nRemainGP, nRemainLP, szState,
		WorldID, szkilled_szName, vPos_x, vPos_y, vPos_z,
		nAttackPower, nMax_HP, uGetidPlayer, dwGold, nRemainGold,
		nItem_UniqueNo, szItem_Name, Item_durability, Item_count, ItemAddLv,
		szStart_Time, TotalPlayerTime, szRemoteIP, nQuest_Index, uKill_idPlayer );	
	if( strlen(qryLog) > 40960 )
		ASSERT(0);
	
}
*/

/*
#ifdef __S0114_RELOADPRO
void CDbManager::Load_ReloadAccount()
{
	LPDB_OVERLAPPED_PLUS pOV = m_pDbIOData->Alloc();
	pOV->nQueryMode	= RELOAD_PROJECT;	// call CDBManager::QueryBillingInfo
	PostQueuedCompletionStatus( m_hDbCompletionPort, 1, NULL, &pOV->Overlapped );
}
#endif // __S0114_RELOADPRO
*/
void CDbManager::UpdateTracking( BOOL bON, LPCTSTR lpszAccount )
{
	if( m_bTracking == FALSE )
		return;

	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= m_pDbIOData->Alloc();
	strcpy( lpDbOverlappedPlus->szAccount, lpszAccount );
	if( bON )
		lpDbOverlappedPlus->nQueryMode	= ACCOUNT_ON;
	else
		lpDbOverlappedPlus->nQueryMode	= ACCOUNT_OFF;

	PostQueuedCompletionStatus( m_hDbCompletionPort, 1, NULL, &lpDbOverlappedPlus->Overlapped );		
}

// 모두 OFF로 해줌
BOOL CDbManager::AllOff()
{
	if( m_bTracking == FALSE )
		return TRUE;

	CQuery qry;
	if( FALSE == qry.Connect( 3, DSN_NAME_LOGIN, DB_ADMIN_ID_LOGIN, m_szLoginPWD ) )
	{
		AfxMessageBox( "Error AllOff: DB Connect Loing InitInstance Failed " );
		return FALSE;
	}
	
	char szQuery[1024] = {0,};
	DBQryAccount( szQuery, "A3" );

	if( FALSE == qry.Exec( szQuery ) )
	{
		AfxMessageBox( " DB Qry : ALL_OFF 구문 실패 " );
		return FALSE;
	}
	return TRUE;
}

void CDbManager::AccountOn( CQuery & qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	DBQryAccount( lpDbOverlappedPlus->szQuery, "A1", lpDbOverlappedPlus->szAccount );
	if( FALSE == qry.Exec( lpDbOverlappedPlus->szQuery ) )
		WriteLog( "%s, %d\r\n\t%s", __FILE__, __LINE__, lpDbOverlappedPlus->szQuery );
	m_pDbIOData->Free( lpDbOverlappedPlus );
}


void CDbManager::AccountOff( CQuery & qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	// 康
	if( GetLanguage() == LANG_TWN )
	{
		sprintf( lpDbOverlappedPlus->szQuery, "dbo.uspExtendSession @paccount = '%s'", lpDbOverlappedPlus->szAccount );
		if( FALSE == qry.Exec( lpDbOverlappedPlus->szQuery ) )
			WriteLog( "%s, %d\r\n\t%s", __FILE__, __LINE__, lpDbOverlappedPlus->szQuery );
	}
	DBQryAccount( lpDbOverlappedPlus->szQuery, "A2", lpDbOverlappedPlus->szAccount );
	if( FALSE == qry.Exec( lpDbOverlappedPlus->szQuery ) )
		WriteLog( "%s, %d\r\n\t%s", __FILE__, __LINE__, lpDbOverlappedPlus->szQuery );
	m_pDbIOData->Free( lpDbOverlappedPlus );
}

void CDbManager::LogSMItem( CQuery & qryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	
	/*
	bi2.szBxaid		// 구매 번호
	bi2.dwItemId	// 아이템 인덱스
	bi2.dwItemNum	// 아이템 개수
	bi2.dwServerIndex	// 서버 인덱스
	bi2.dwPlayerId		// 플레이어 식별자
	bi2.dwRetVal	// 지급 성공 여부, 성공 : 1, 실패 : 0
	sn		// 시리얼 번호
	*/
	BUYING_INFO2	bi2;
	SERIALNUMBER iSerialNumber;
	arRead.Read( (void*)&bi2, sizeof(BUYING_INFO2) );
	arRead >> iSerialNumber;
	
	// 구매번호는 : pszGetidPlayer
	// 아이템인덱스 : dwGold // ItemName은 형식에 안맞음
	LOG_SM_ITEM_INFO qi( "L3" );
	if( bi2.dwRetVal )
		qi.pszState = "3";		// 성공
	else
		qi.pszState = "4";		// 실패
	qi.nserverindex = bi2.dwServerIndex;
	char szPlayerId[128] = {0,};
	sprintf( szPlayerId, "%07d", bi2.dwPlayerId );
	qi.pszkilled_szName = szPlayerId;
	qi.nItem_UniqueNo = iSerialNumber;
	qi.nItem_count = bi2.dwItemNum;
	qi.pszGetidPlayer = bi2.szBxaid;
	qi.dwGold = bi2.dwItemId;
	
	DBQryLogSMItem( lpDbOverlappedPlus->szQuery, qi);
	
	if( FALSE == qryLog.Exec( lpDbOverlappedPlus->szQuery ) )
	{
		WriteLog( "%s, %d\r\n\t%s", __FILE__, __LINE__, lpDbOverlappedPlus->szQuery );
		m_pDbIOData->Free( lpDbOverlappedPlus );
		return;
	}
	m_pDbIOData->Free( lpDbOverlappedPlus );
}

void CDbManager::QueryBillingInfo( CQuery& query, LPDB_OVERLAPPED_PLUS pOV )
{
	GetBillingMgr()->OnDBQuery( query, pOV );
	m_pDbIOData->Free( pOV );
}


// 유료유저는 빌링정보를 DB에 쿼리 
void CDbManager::PostBillingQuery( const char* szAccount, DWORD dwKey, int nExtra )
{
	LPDB_OVERLAPPED_PLUS pOV = m_pDbIOData->Alloc();
	strcpy( pOV->szAccount, szAccount );
	pOV->nQueryMode	= QUERY_BILLINGINFO;	// call CDBManager::QueryBillingInfo
	pOV->dwKey		= dwKey;
	pOV->nExtra		= nExtra;
	PostQueuedCompletionStatus( m_hDbCompletionPort, 1, NULL, &pOV->Overlapped );
}

/*
#ifdef __S0114_RELOADPRO
void CDbManager::QueryReloadProject( CQuery& query, LPDB_OVERLAPPED_PLUS pOV )
{
	char szQuery[1024] = {0,};
	sprintf( szQuery, "LOGIN_RELOAD_STR" );
	
	if( FALSE == query.Exec( szQuery ) )
	{
		Error( " DB Qry : Load_ReloadAccount 구문 실패 : LOGIN_RELOAD_STR" );
		m_pDbIOData->Free( pOV );
		return;
	}

	m_OutAccount_List.clear();
	char szAccount[64] = {0,};
	BOOL bOutAccount = FALSE;
	while( query.Fetch() )
	{
		query.GetStr( "account", szAccount );
		m_OutAccount_List.insert( szAccount );
		bOutAccount = TRUE;
	}
	m_pDbIOData->Free( pOV );

	if( bOutAccount )
		g_dpDbSrvr.SendReloadAccount();
}
#endif // __S0114_RELOADPRO
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
// DbWorkerThread
///////////////////////////////////////////////////////////////////////////////////////////////////

u_int __stdcall DbWorkerThread( LPVOID lpvDbManager )
{
	CDbManager* pDbManager	= (CDbManager*)lpvDbManager;

	CQuery qryLogin;
	CQuery qryLog;
	BOOL bLongConnect = FALSE;

/*
#ifdef __S0114_RELOADPRO
	bLongConnect = TRUE;
#endif // __S0114_RELOADPRO
*/
	if( bLongConnect || pDbManager->m_bTracking )
	{
		if( FALSE == qryLogin.Connect( 3, DSN_NAME_LOGIN, DB_ADMIN_ID_LOGIN, pDbManager->m_szLoginPWD ) )
		{
			AfxMessageBox( " DB Login Connect Failed " );
			return 0;
		}
	}


	if( pDbManager->m_bLogItem )
	{
		if( FALSE == qryLog.Connect( 3, DSN_NAME_LOG, DB_ADMIN_ID_LOG, pDbManager->m_szLogPWD ) )
		{
			AfxMessageBox( " DB Log Connect Failed " );
			return 0;
		}
	}

#ifdef __BILLING2_041021
	CQuery qryBilling;
	char* pszPWD = (char*)GetBillingPWD();
	if( FALSE == qryBilling.Connect( 3, DSN_NAME_BILLING, DB_ADMIN_ID_BILLING, pszPWD ) )
	{
		AfxMessageBox( " DB Billing Connect Failed " );
		return 0;
	}
#endif

	SetEvent( s_hHandle );

	BOOL fOk;
	HANDLE hCompletionPort	= pDbManager->m_hDbCompletionPort;
	DWORD dwBytesTransferred	= 0;
	LPDWORD lpCompletionKey		= NULL;
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= NULL;

	while( 1 )
	{
		fOk	= GetQueuedCompletionStatus( hCompletionPort,
		  								 &dwBytesTransferred,
										 (LPDWORD)&lpCompletionKey,
										 (LPOVERLAPPED*)&lpDbOverlappedPlus,
										 INFINITE );
		if( fOk == FALSE ) 
		{	
			ASSERT( 0 );
			continue;
		}

		if( dwBytesTransferred == 0 )	// terminate
			return( 0 );
		
		switch( lpDbOverlappedPlus->nQueryMode )
		{
			case ACCOUNT_ON:
				pDbManager->AccountOn( qryLogin, lpDbOverlappedPlus );
				break;
			case ACCOUNT_OFF:
				pDbManager->AccountOff( qryLogin, lpDbOverlappedPlus );
				break;
			case LOG_SM_ITEM:
				if( pDbManager->m_bLogItem )
					pDbManager->LogSMItem( qryLog, lpDbOverlappedPlus );
				break;

#ifdef __BILLING2_041021
			case QUERY_BILLINGINFO:
				pDbManager->QueryBillingInfo( qryBilling, lpDbOverlappedPlus );
				break;
#endif

/*
#ifdef __S0114_RELOADPRO
			case RELOAD_PROJECT:
				pDbManager->QueryReloadProject( qryLogin, lpDbOverlappedPlus );
				break;
#endif // __S0114_RELOADPRO
*/
			default:
				break;
		}
	}
	return( 0 );
}

