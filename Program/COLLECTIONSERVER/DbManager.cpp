// DbManager.cpp: implementation of the CDbManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DbManager.h"
#include "DPCollectionSrvr.h"

HANDLE	s_hHandle	= (HANDLE)NULL;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDbManager::CDbManager()
{
	m_pDbIOData		= new CMemPool<DB_OVERLAPPED_PLUS>(512);
	for( int i = 0; i < DEFAULT_DB_WORKER_THREAD_NUM; i++ )
	{
		m_hIOCP[i]	= NULL;
		m_hDbWorkerThreadTerminate[i] = NULL;
	}
	m_szDSN_NAME_COLLECTION[0] = '\0';
	m_szDB_ADMIN_ID_COLLECTION[0] = '\0';
	m_szDB_ADMIN_PASS_COLLECTION[0] = '\0';
}

CDbManager::~CDbManager()
{
	for( int i = 0; i < DEFAULT_DB_WORKER_THREAD_NUM; i++ )
	{
		PostQueuedCompletionStatus( m_hIOCP[i], 0, NULL, NULL );
		CLOSE_HANDLE( m_hIOCP[i] );
	}

	WaitForMultipleObjects( DEFAULT_DB_WORKER_THREAD_NUM, m_hDbWorkerThreadTerminate, TRUE, INFINITE );

	for( i = 0; i < DEFAULT_DB_WORKER_THREAD_NUM; i++ ) {
		CLOSE_HANDLE( m_hDbWorkerThreadTerminate[i] );
	}

	SAFE_DELETE( m_pDbIOData );
}

CDbManager* CDbManager::GetInstance()
{
	static CDbManager sDbM;
	return & sDbM;
}

BOOL CDbManager::CreateDbWorkers()
{
	s_hHandle = CreateEvent( NULL, FALSE, FALSE, NULL );

	for( int i = 0; i < DEFAULT_DB_WORKER_THREAD_NUM; i++ )
	{
		m_hIOCP[i]	= CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );
		ASSERT( m_hIOCP[i] );

		HANDLE hThread	= chBEGINTHREADEX( NULL, 0, DbWorkerThread, (LPVOID)i, 0, NULL );
		ASSERT( hThread );

		m_hDbWorkerThreadTerminate[i]	= hThread;
		if( WaitForSingleObject( s_hHandle, SEC( 10 ) ) == WAIT_TIMEOUT )
		{
			OutputDebugString( "COLLECTIONSERVER.EXE\t// TIMEOUT\t// ODBC" );
			return FALSE;
		}
	}

	CloseHandle( s_hHandle );
	return TRUE;
}

void CDbManager::PostQ( LPDB_OVERLAPPED_PLUS pData )
{
	srand( time( NULL ) );
	int nIOCP = rand() % DEFAULT_DB_WORKER_THREAD_NUM;

	::PostQueuedCompletionStatus( GetIOCPHandle( nIOCP ), 1, NULL, (LPOVERLAPPED)pData );
}

HANDLE CDbManager::GetIOCPHandle( int n )
{
	return m_hIOCP[n];
}

u_int __stdcall DbWorkerThread( LPVOID nIndex )
{
	CQuery query;
	if( FALSE == query.Connect( 3, DbManager->m_szDSN_NAME_COLLECTION, DbManager->m_szDB_ADMIN_ID_COLLECTION, DbManager->m_szDB_ADMIN_PASS_COLLECTION ) )
	{
		AfxMessageBox( "Error : Not Connect Collection DB" );
	}

	SetEvent( s_hHandle );

	HANDLE hIOCP				= DbManager->GetIOCPHandle( (int)nIndex );
	DWORD dwBytesTransferred	= 0;
	LPDWORD lpCompletionKey		= NULL;
	LPDB_OVERLAPPED_PLUS pData	= NULL;

	while( 1 )
	{
		BOOL fOk = GetQueuedCompletionStatus( hIOCP,
										 &dwBytesTransferred,
										(LPDWORD)&lpCompletionKey,
										(LPOVERLAPPED*)&pData,
										INFINITE );

		if( fOk == FALSE ) 
		{	
			ASSERT( 0 );				// fatal error
			continue;
		}

		if( dwBytesTransferred == 0 )	// terminate
			return( 0 );
		
		switch( pData->nQueryMode )
		{
			case LOG_CERTIFY_INFO:
				DbManager->LogCertifyInfo( query, pData );
				break;
		}
	}
	return( 0 );
}

void CDbManager::LogCertifyInfo( CQuery & query, LPDB_OVERLAPPED_PLUS pData )
{
	query.Clear();

	char	szQuery[256];

	sprintf( szQuery, "usp_tblCollection_Insert @szCertifyIP = '%s', @szClientVer = '%s', @nFlyffVer = %d, @nLang = %d, @szClientIP = '%s'", 
		pData->szCertifyIP, pData->szClientVer, pData->nFlyffVer, pData->nLang, pData->szClientIP );

	if( !query.Exec( szQuery ) )
	{
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );
		m_pDbIOData->Free( pData );
		return;
	}
	m_pDbIOData->Free( pData );
}