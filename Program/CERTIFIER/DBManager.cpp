#include "stdafx.h"
#include "dbmanager.h"

#ifdef __GPAUTH
#include "afxinet.h"
#define		s_sUrl	"spe.gpotato.eu"
#define	VALID_CRLF( n, r, s )	if( ( n ) < 0 )	{	r.nResult	= -1;	*r.szResult	= '\0';	OutputDebugString( (LPCTSTR)s );	return;		}
#define	VALID_STR( len, max, r, s )		if( ( len ) > ( max ) )	{	r.nResult	= -1;	*r.szResult	= '\0';	OutputDebugString( (LPCTSTR)s );	return;		}
#endif	// __GPAUTH

#include "dpcertifier.h"
#include "dpaccountclient.h"
#include "AccountMgr.h"
#include "..\Resource\Lang.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
// extern
extern	CDPCertifier		g_dpCertifier;
extern	CDPAccountClient	g_dpAccountClient;
extern	UINT				HashKey( const char* key );

/////////////////////////////////////////////////////////////////////////////////////////////////
// global
CDbManager	g_DbManager;
HANDLE		s_hHandle	= (HANDLE)NULL;
#ifdef __JAPAN_AUTH
string	g_strWebCertURL;
#endif // __JAPAN_AUTH
/////////////////////////////////////////////////////////////////////////////////////////////////
CDbManager::CDbManager()
{
	m_pDbIOData		= new CMemPool<DB_OVERLAPPED_PLUS>(512);
	for( int i = 0; i < DEFAULT_DB_WORKER_THREAD_NUM; i++ )
	{
		m_hIOCP[i]	= NULL;
		m_hDbWorkerThreadTerminate[i] = NULL;
	}
	m_szLoginPWD[0] = '\0';
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

BOOL CDbManager::CreateDbWorkers()
{
#ifdef __GPAUTH
//	BOOL bGPAuth	= TRUE;
	BOOL bGPAuth	= ( ::GetLanguage() == LANG_FRE ); // ( ::GetLanguage() == LANG_GER || ::GetLanguage() == LANG_FRE )
#ifdef __INTERNALSERVER
	bGPAuth = FALSE;
#endif // __INTERNALSERVER
#endif	// __GPAUTH
	s_hHandle = CreateEvent( NULL, FALSE, FALSE, NULL );

	for( int i = 0; i < DEFAULT_DB_WORKER_THREAD_NUM; i++ )
	{
		m_hIOCP[i]	= CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );
		ASSERT( m_hIOCP[i] );
#ifdef __GPAUTH
		HANDLE hThread;
		if( bGPAuth )
			hThread	= chBEGINTHREADEX( NULL, 0, GPotatoAuthWorker, (LPVOID)i, 0, NULL );
#ifdef __JAPAN_AUTH
		else if( ::GetLanguage() == LANG_JAP && !g_strWebCertURL.empty() )
			hThread	= chBEGINTHREADEX( NULL, 0, JapanAuthWorker, (LPVOID)i, 0, NULL );
#endif // __JAPAN_AUTH
		else
			hThread	= chBEGINTHREADEX( NULL, 0, DbWorkerThread, (LPVOID)i, 0, NULL );
#else	// __GPAUTH
		HANDLE hThread	= chBEGINTHREADEX( NULL, 0, DbWorkerThread, (LPVOID)i, 0, NULL );
#endif	// __GPAUTH
		ASSERT( hThread );
		m_hDbWorkerThreadTerminate[i]	= hThread;
		if( WaitForSingleObject( s_hHandle, SEC( 10 ) ) == WAIT_TIMEOUT )
		{
			OutputDebugString( "CERTIFIER.EXE\t// TIMEOUT\t// ODBC" );
			return FALSE;
		}
	}

	CloseHandle( s_hHandle );
	return TRUE;
}

#if __VER >= 14 // __PCBANG
void CDbManager::DBQryAccount( char* qryAccount, LPDB_OVERLAPPED_PLUS pData )
{
	char lpAddr[16] = {0,};
	g_dpCertifier.GetPlayerAddr( pData->dpId, lpAddr );

	if( GetLanguage() == LANG_TWN )
		wsprintf( qryAccount, "LOGIN_STR @iaccount='%s', @ipassword='%s', @isession='%s', @i_IPAddress='%s'", 
					pData->AccountInfo.szAccount, pData->AccountInfo.szPassword, pData->AccountInfo.szSessionPwd, lpAddr );
//		wsprintf( qryAccount, "LOGIN_STR @iaccount='%s', @ipassword='%s', @isession='%s'", 
//					pData->AccountInfo.szAccount, pData->AccountInfo.szPassword, pData->AccountInfo.szSessionPwd );
	else
		wsprintf( qryAccount, "LOGIN_STR '%s', '%s', '%s'", pData->AccountInfo.szAccount, pData->AccountInfo.szPassword, lpAddr );
//		wsprintf( qryAccount, "LOGIN_STR '%s', '%s'", pData->AccountInfo.szAccount, pData->AccountInfo.szPassword );
}
#else // __PCBANG
#ifdef __TWN_LOGIN0816
void CDbManager::DBQryAccount( char* qryAccount, char* szAccount, char* szPass, char* szSessionPwd )
{
	if( GetLanguage() == LANG_TWN )
		wsprintf( qryAccount, "LOGIN_STR @iaccount='%s', @ipassword='%s', @isession='%s'", szAccount, szPass, szSessionPwd );
	else
		wsprintf( qryAccount, "LOGIN_STR '%s', '%s'", szAccount, szPass );
}
#else	// __TWN_LOGIN0816
void CDbManager::DBQryAccount( char* qryAccount, char* szAccount, char* szPass )
{
	wsprintf( qryAccount, 
		"LOGIN_STR '%s', '%s'", szAccount, szPass );
}
#endif	// __TWN_LOGIN0816
#endif // __PCBANG

// AccountFlag를 얻는다.
// f18 - 디비에서 얻는 데이타 
//       태국: 2 - 미성년자, 4 - 미등록 성인, 8 - 등록된 성인 
//       그외: 0 - 미성년자, 1 - 성인 

BYTE CDbManager::GetAccountFlag( int f18, LPCTSTR szAccount )
{
	BYTE cb18 = (BYTE)f18;
	BYTE cbAccountFlag = 0x00;

	if( ::GetLanguage() == LANG_THA )
	{
		if( cb18 & 0x04 )
			cbAccountFlag |= ACCOUNT_FLAG_UNREGISTER18;		// 미등록 성인
		else if( cb18 & 0x08 )
			cbAccountFlag |= ACCOUNT_FLAG_18;
	}
	else
	{
		if( cb18 & 0x01 )
			cbAccountFlag |= ACCOUNT_FLAG_18;
	}

	if( IsEveSchoolAccount( szAccount ) )
		cbAccountFlag |= ACCOUNT_FLAG_SCHOOLEVENT;

//	char szBuffer[256];
//	sprintf( szBuffer, "%s - f18:%02x cbAccountFlag:%02x\n", szAccount, cb18, cbAccountFlag );
//	OutputDebugString( szBuffer );

	return cbAccountFlag;
}

void CDbManager::OnCertifyQueryOK( CQuery & query, LPDB_OVERLAPPED_PLUS pData, const char* szCheck )
{
	BOOL bGPotatoAuth	= ::GetLanguage() == LANG_FRE; // ::GetLanguage() == LANG_GER || ::GetLanguage() == LANG_FRE;
#ifdef __JAPAN_AUTH
	if( !bGPotatoAuth && ::GetLanguage() == LANG_JAP )
		bGPotatoAuth = TRUE;
#endif // __JAPAN_AUTH
	int n18	= 1;
	if( !bGPotatoAuth )
		n18	= query.GetInt( "f18" );
	BYTE cbAccountFlag	= GetAccountFlag( n18, pData->AccountInfo.szAccount );
	if( ::GetLanguage() == LANG_THA )
	{
		if( (cbAccountFlag & ACCOUNT_FLAG_18) == 0x00 )
		{
			CTime cur = CTime::GetCurrentTime();
			if( cur.GetHour() >= 22 || cur.GetHour() < 6 ) 
			{
				g_dpCertifier.SendError( ERROR_TOO_LATE_PLAY, pData->dpId );
				return;
			}
		}
	}


	int fCheck = 0;
#ifdef __BILLING0712
	if( !bGPotatoAuth )
		fCheck	= query.GetInt( "fCheck" );
#endif//__BILLING0712

	char lpAddr[16]	= { 0, };
	g_dpCertifier.GetPlayerAddr( pData->dpId, lpAddr );				

#ifdef __GPAUTH_02
#ifdef __EUROPE_0514
#if __VER >= 14 // __PCBANG
	g_dpAccountClient.SendAddAccount( lpAddr, pData->AccountInfo.szAccount, cbAccountFlag, pData->dpId, fCheck, szCheck, pData->AccountInfo.szBak, pData->AccountInfo.dwPCBangClass );
#else // __PCBANG
	g_dpAccountClient.SendAddAccount( lpAddr, pData->AccountInfo.szAccount, cbAccountFlag, pData->dpId, fCheck, szCheck, pData->AccountInfo.szBak );
#endif // __PCBANG
#else	// __EUROPE_0514
	g_dpAccountClient.SendAddAccount( lpAddr, pData->AccountInfo.szAccount, cbAccountFlag, pData->dpId, fCheck, szCheck );
#endif	// __EUROPE_0514
#else	// __GPAUTH_02
	g_dpAccountClient.SendAddAccount( lpAddr, pData->AccountInfo.szAccount, cbAccountFlag, pData->dpId, fCheck );
#endif	// __GPAUTH_02
}

void CDbManager::Certify( CQuery & query, LPDB_OVERLAPPED_PLUS pData, CAccountMgr& accountMgr )
{
	ACCOUNT_CHECK result = CHECK_OK;
	if( pData->dwIP )
	{
		result = accountMgr.Check( pData->dwIP );
		switch( result )
		{
		case CHECK_1TIMES_ERROR:
			g_dpCertifier.SendError( ERROR_15SEC_PREVENT, pData->dpId );
			m_pDbIOData->Free( pData );
			return;
		case CHECK_3TIMES_ERROR:
			g_dpCertifier.SendError( ERROR_15MIN_PREVENT, pData->dpId );
			m_pDbIOData->Free( pData );
			return;
		}
	}

	query.Clear();

	char	szQuery[256];
#if __VER >= 14 // __PCBANG
	DBQryAccount( szQuery, pData );
#else // __PCBANG
#ifdef __TWN_LOGIN0816
	DBQryAccount( szQuery, pData->AccountInfo.szAccount, pData->AccountInfo.szPassword, pData->AccountInfo.szSessionPwd );
#else	// __TWN_LOGIN0816
	DBQryAccount( szQuery, pData->AccountInfo.szAccount, pData->AccountInfo.szPassword );
#endif	// __TWN_LOGIN0816
#endif // __PCBANG

	int nCode = ERROR_CERT_GENERAL;		

	if( query.Exec( szQuery ) )
	{
		if( query.Fetch() )
		{
			int nError	= query.GetInt( "fError" );
			switch( nError )
			{
			case 0:
#if __VER >= 14 // __PCBANG
				pData->AccountInfo.dwPCBangClass = query.GetInt( "fPCZone" );
#endif // __PCBANG
				if( pData->dwIP )
					accountMgr.SetError( 0 );
#ifdef __EUROPE_0514
				lstrcpy( pData->AccountInfo.szBak, pData->AccountInfo.szAccount );
#endif	// __EUROPE_0514
				OnCertifyQueryOK( query, pData );
				m_pDbIOData->Free( pData );
				return;
			case 1:	// 암호틀림
				if( pData->dwIP )
					accountMgr.SetError( 1 );
				nCode = ERROR_FLYFF_PASSWORD;
				break;
			case 3:	// 계정블럭이거나 유료화 초과
				nCode = ERROR_BLOCKGOLD_ACCOUNT;				
				break;
			case 4:	// 실명인증후 게임접속이 가능합니다 www.flyff.com으로 접속해주십시오
				nCode = ERROR_FLYFF_AUTH;				
				break;
			case 5: // 프리프는 12세 이상 이용가 이므로 게임접속을 할수 없습니다.
				nCode = ERROR_FLYFF_PERMIT;
				break;
			case 6: // 14세 미만 가입자 분들은 부모님 동의서를 보내주셔야 게임 접속이 가능합니다. www.flyff.com 으로 접속하셔서 확인해 주세요.
				nCode = ERROR_FLYFF_NEED_AGREEMENT;
				break;
			case 7:	// Web탈퇴자 회원
				nCode = ERROR_FLYFF_NO_MEMBERSHIP;
				break;
			case 9:	// 실시간 데이터 작업 유저
				nCode = ERROR_FLYFF_DB_JOB_ING;
				break;
			case 91:
				nCode	= ERROR_FLYFF_EXPIRED_SESSION_PASSWORD;
				break;
			default: 
				nCode = ERROR_FLYFF_ACCOUNT;				
				break;
			}
		}
	}

	g_dpCertifier.SendError( nCode, pData->dpId );
	m_pDbIOData->Free( pData );
}

void CDbManager::CloseExistingConnection( CQuery & query, LPDB_OVERLAPPED_PLUS pData )
{
	query.Clear();

	char	szQuery[128];
#if __VER >= 14 // __PCBANG
	DBQryAccount( szQuery, pData );
#else // __PCBANG
	DBQryAccount( szQuery, pData->AccountInfo.szAccount, pData->AccountInfo.szPassword );
#endif // __PCBANG

	if( query.Exec( szQuery ) )
	{
		if( query.Fetch() )
		{
			if( 0 == query.GetInt( "fError" ) )
				g_dpAccountClient.SendCloseExistingConnection( pData->AccountInfo.szAccount );
		}
	}
	m_pDbIOData->Free( pData );
}

u_int __stdcall DbWorkerThread( LPVOID nIndex )
{
	CAccountMgr mgr;

	CQuery query;
	if( FALSE == query.Connect( 3, "login", "account", g_DbManager.m_szLoginPWD ) )
	{
		AfxMessageBox( "Error : Not Connect Login DB" );
	}

	SetEvent( s_hHandle );

	HANDLE hIOCP				= g_DbManager.GetIOCPHandle( (int)nIndex );
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
			case CERTIFY:
				g_DbManager.Certify( query, pData, mgr );
				break;
			case CLOSE_EXISTING_CONNECTION:
				g_DbManager.CloseExistingConnection( query, pData );
				break;
		}
	}
	return( 0 );
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

BOOL CDbManager::LoadEveSchoolAccount( void )
{
	CScanner s;
	if( s.Load( "EveSchoolAccount.txt" ) )
	{
		s.GetToken();
		while( s.tok != FINISHED )
		{
			m_eveSchoolAccount.insert( (const char*)s.Token );
			s.GetToken();
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CDbManager::IsEveSchoolAccount( const char* pszAccount )
{
	set<string>::iterator i	= m_eveSchoolAccount.find( pszAccount );
	return( i != m_eveSchoolAccount.end() );
}

void CDbManager::PostQ( LPDB_OVERLAPPED_PLUS pData )
{
	UINT nKey = ::HashKey( pData->AccountInfo.szAccount );
	int nIOCP = nKey % DEFAULT_DB_WORKER_THREAD_NUM;

	::PostQueuedCompletionStatus( GetIOCPHandle( nIOCP ), 1, NULL, (LPOVERLAPPED)pData );
}

HANDLE CDbManager::GetIOCPHandle( int n )
{
	return m_hIOCP[n];
}

#ifdef __GPAUTH
u_int __stdcall GPotatoAuthWorker( LPVOID pParam )
{
	CAccountMgr mgr;

	CQuery query;
	if( FALSE == query.Connect( 3, "login", "account", g_DbManager.m_szLoginPWD ) )
		AfxMessageBox( "can't connect to database : login" );

	SetEvent( s_hHandle );

	HANDLE hIOCP	= g_DbManager.GetIOCPHandle( (int)pParam );
	DWORD dwBytesTransferred	= 0;
	LPDWORD lpCompletionKey		= NULL;
	LPDB_OVERLAPPED_PLUS pov	= NULL;

	while( 1 )
	{
		BOOL fOk = GetQueuedCompletionStatus( hIOCP,
										 &dwBytesTransferred,
										(LPDWORD)&lpCompletionKey,
										(LPOVERLAPPED*)&pov,
										INFINITE );

		if( fOk == FALSE ) 
		{	
			ASSERT( 0 );				// fatal error
			continue;
		}

		if( dwBytesTransferred == 0 )	// terminate
			return( 0 );
		
		switch( pov->nQueryMode )
		{
			case CERTIFY:
				g_DbManager.Certify2( query, pov, mgr );
				break;
			case CLOSE_EXISTING_CONNECTION:
				g_DbManager.CloseExistingConnection2( query, pov );
				break;
		}
	}
	return( 0 );
}

void CDbManager::Certify2( CQuery & query, LPDB_OVERLAPPED_PLUS pov, CAccountMgr & mgr )
{
	ACCOUNT_CHECK result	= CHECK_OK;
	if( pov->dwIP )
	{
		result	= mgr.Check( pov->dwIP );
		switch( result )
		{
			case CHECK_1TIMES_ERROR:
				g_dpCertifier.SendError( ERROR_15SEC_PREVENT, pov->dpId );
				m_pDbIOData->Free( pov );
				return;
			case CHECK_3TIMES_ERROR:
				g_dpCertifier.SendError( ERROR_15MIN_PREVENT, pov->dpId );
				m_pDbIOData->Free( pov );
				return;
		}
	}
	char sAddr[16]	= { 0,};
	g_dpCertifier.GetPlayerAddr( pov->dpId, sAddr );
	
	GPAUTH_RESULT r;
//	GetGPAuthResult( s_sUrl, 1, ( GetLanguage() == LANG_GER? 1: 2 ), pov->AccountInfo.szAccount, pov->AccountInfo.szPassword, sAddr, r );
	GetGPAuthResult( s_sUrl, 1, ( GetLanguage() == LANG_FRE? 1: 2 ), pov->AccountInfo.szAccount, pov->AccountInfo.szPassword, sAddr, r );
	char	lpOutputString[200]	= { 0,};

#ifdef __GPAUTH_02
	sprintf( lpOutputString, "%d, %s, %s, %d, %d, %s", r.nResult, r.szResult, r.szGPotatoID, r.nGPotatoNo, r.bGM, r.szCheck );
#else	// __GPAUTH_02
	sprintf( lpOutputString, "%d, %s, %s, %d, %d", r.nResult, r.szResult, r.szGPotatoID, r.nGPotatoNo, r.bGM );
#endif	// __GPAUTH_02
	OutputDebugString( lpOutputString );
	if( r.nResult== 0 )
	{
		if( pov->dwIP )
			mgr.SetError( 0 );

#ifdef __EUROPE_0514
		lstrcpy( pov->AccountInfo.szBak, pov->AccountInfo.szAccount );
#endif	// __EUROPE_0514
		sprintf( pov->AccountInfo.szAccount, "%d", r.nGPotatoNo );
		
		#ifdef __GPAUTH_02
		OnCertifyQueryOK( query, pov, r.szCheck );
		#else	// __GPAUTH_02
		OnCertifyQueryOK( query, pov );
		#endif	// __GPAUTH_02

		#ifdef __GPAUTH_03
		SQLAddAccount( query, pov->AccountInfo.szAccount, pov->AccountInfo.szPassword, r.bGM );
		#else	// __GPAUTH_03
		SQLAddAccount( query, pov->AccountInfo.szAccount, pov->AccountInfo.szPassword );
		#endif	// __GPAUTH_03
	}
	else
	{
		g_dpCertifier.SendErrorString( r.szResult, pov->dpId );
	}
	m_pDbIOData->Free( pov );
}

void CDbManager::CloseExistingConnection2( CQuery & query, LPDB_OVERLAPPED_PLUS pov )
{
	GPAUTH_RESULT r;

	char sAddr[16]	= { 0,};
	g_dpCertifier.GetPlayerAddr( pov->dpId, sAddr );

//	GetGPAuthResult( s_sUrl, 1, ( GetLanguage() == LANG_GER? 1: 2 ), pov->AccountInfo.szAccount, pov->AccountInfo.szPassword, sAddr, r );
	GetGPAuthResult( s_sUrl, 1, ( GetLanguage() == LANG_FRE? 1: 2 ), pov->AccountInfo.szAccount, pov->AccountInfo.szPassword, sAddr, r );
	if( r.nResult == 0 )
	{
//		lstrcpy( pov->AccountInfo.szAccount, r.szGPotatoID );
		sprintf( pov->AccountInfo.szAccount, "%d", r.nGPotatoNo );

		g_dpAccountClient.SendCloseExistingConnection( pov->AccountInfo.szAccount );
#ifdef __GPAUTH_03
		SQLAddAccount( query, pov->AccountInfo.szAccount, pov->AccountInfo.szPassword, r.bGM );
#else	// __GPAUTH_03
		SQLAddAccount( query, pov->AccountInfo.szAccount, pov->AccountInfo.szPassword );
#endif	// __GPAUTH_03
	}
	m_pDbIOData->Free( pov );
}

#ifdef __GPAUTH_03
void CDbManager::SQLAddAccount( CQuery & query, char* szAccount, char* szPassword, BOOL bGM )
#else	// __GPAUTH_03
void CDbManager::SQLAddAccount( CQuery & query, char* szAccount, char* szPassword )
#endif	// __GPAUTH_03
{
	query.Clear();
	char szSQL[100]	= { 0,};
#ifdef __GPAUTH_03
	char chAuth	= ( bGM? 'P': 'F' );
	sprintf( szSQL, "uspAddAccount '%s', '%s', '%c'", szAccount, szPassword, chAuth );
#else	// __GPAUTH_03
	sprintf( szSQL, "uspAddAccount '%s', '%s'", szAccount, szPassword );
#endif	// __GPAUTH_03
	if( !query.Exec( szSQL ) )
	{
		// error
	}
}
#endif	// __GPAUTH

#ifdef __GPAUTH
void	GetGPAuthResult( const char* szUrl, int nMode, int nGameMode, const char* sAccount, const char* sPassword, const char* sAddr, GPAUTH_RESULT & result )
{
	CInternetSession	session( _T( "GPAuth" ) );
	CHttpConnection*	pServer		= NULL;

	CHttpFile*	pFile	= NULL;
	char pszSendMsg[255]	= { 0,};
	char pszHeader[255]	= { 0,};
	CString strHeaders, strResult;
	int nReadSize	= 0;

	BOOL b	= FALSE;

	sprintf( pszSendMsg, "MODE=%c&GAMECODE=G00%d&GPOTATOID=%s&PASSWORD=%s&USERIP=%s", ( nMode == 1 ? 'L': 'O' ), nGameMode, sAccount, sPassword, sAddr );
	strHeaders	= "Content-Type: application/x-www-form-urlencoded";
	try
	{
		pServer		= session.GetHttpConnection( szUrl );
		if( !pServer )
		{
			Error( "\"%s\" Connection Failed!", szUrl );
			goto DONE;
		}
		pFile	= pServer->OpenRequest( CHttpConnection::HTTP_VERB_POST, "/Account/Authentication.Aspx" );
		if( !pFile  )
		{
			Error( "OpenRequest Failed!" );
			goto DONE;
		}
		if( !( pFile->SendRequest( strHeaders, (LPVOID)pszSendMsg, lstrlen( pszSendMsg ) ) ) )
		{
			Error( "SendRequest Failed! -> Msg : %s", pszSendMsg ); 	
			goto DONE;
		}
	}
	catch( CInternetException* e )
	{
		TCHAR	szError[255]	= { 0,};
		e->GetErrorMessage( szError, 255 );
		goto DONE;
	}

	nReadSize	= (int)pFile->Read( strResult.GetBuffer( (int)( pFile->GetLength() ) ), (unsigned int)( pFile->GetLength() ) );

	strResult.ReleaseBuffer();
	if( nReadSize = strResult.GetLength() )
		b	= TRUE;

DONE:
	if( pFile )
	{
		pFile->Close();
		safe_delete( pFile );
		pFile	= NULL;
	}
	if( pServer )
	{
		pServer->Close();
		safe_delete( pServer );
		pServer		= NULL;
	}
	session.Close();
	if( b )
	{
		// 1. nFirst > 0
		// 2. len <= max
		// 3. if nResult	= 0, szResult = SUCCESS
		int nFirst	= strResult.Find( "\r\n", 0 );
		if( nFirst < 0 )
			nFirst	= strResult.Find( "\n\r", 0 );
		VALID_CRLF( nFirst, result, strResult )

		result.nResult	= atoi( (LPCSTR)strResult.Left( nFirst ) );
		nFirst	+= 2;	// \r\n
		int nLast	= strResult.Find( "\r\n", nFirst );
		if( nLast < 0 )
			nLast	= strResult.Find( "\n\r", nFirst );
		VALID_CRLF( nLast, result, strResult )
		VALID_STR( nLast - nFirst, 255, result, strResult )

		lstrcpy( result.szResult, (LPCSTR)strResult.Mid( nFirst, nLast - nFirst ) );
		if( result.nResult == 0 && lstrcmp( result.szResult, "SUCCESS" ) )
		{
			result.nResult	= -1;
			*result.szResult	= '\0';
			OutputDebugString( (LPCTSTR)strResult );
			return;
		}

		nFirst	= nLast + 2;	// \r\n
		nLast	= strResult.Find( "\r\n", nFirst );
		if( nLast < 0 )
			nLast	= strResult.Find( "\n\r", nFirst );
		VALID_CRLF( nLast, result, strResult )
		VALID_STR( nLast - nFirst, 20, result, strResult )

		lstrcpy( result.szGPotatoID, (LPCSTR)strResult.Mid( nFirst, nLast - nFirst ) );
		nFirst	= nLast + 2;	// \r\n
		nLast	= strResult.Find( "\r\n", nFirst );
		if( nLast < 0 )
			nLast	= strResult.Find( "\n\r", nFirst );
		VALID_CRLF( nLast, result, strResult )

		result.nGPotatoNo	= atoi( (LPCSTR)strResult.Mid( nFirst, nLast - nFirst ) );
		nFirst	= nLast + 2;	// \r\n
		nLast	= strResult.Find( "\r\n", nFirst );
		if( nLast < 0 )
			nLast	= strResult.Find( "\n\r", nFirst );
		VALID_CRLF( nLast, result, strResult )
		VALID_STR( nLast - nFirst, 20, result, strResult )

		lstrcpy( result.szNickName, (LPCSTR)strResult.Mid( nFirst, nLast - nFirst ) );

	#ifdef __GPAUTH_02
		nFirst	= nLast + 2;	// \r\n
		nLast	= strResult.Find( "\r\n", nFirst );
		if( nLast < 0 )
			nLast	= strResult.Find( "\n\r", nFirst );
		VALID_CRLF( nLast, result, strResult )

		result.bGM	= ( strResult.Mid( nFirst, nLast - nFirst ) == "Y" );
		nFirst	= nLast + 2;	// \r\n
		VALID_STR( strResult.GetLength() - nFirst, 254, result, strResult )

		lstrcpy( result.szCheck, (LPCSTR)strResult.Right( strResult.GetLength() - nFirst ) );
	#else	// __GPAUTH_02
		result.bGM	= ( strResult.Right( 1 ) == "Y" );
	#endif	// __GPAUTH_02
	}
	else
	{
		result.nResult	= -1;
		lstrcpy( result.szResult, "Could not establish a connection with the server..." );
	}
	switch( result.nResult )
	{
		case 1000:
			lstrcpy( result.szResult, "Unexpected system error occurred." );
			break;
		case 2000:
			lstrcpy( result.szResult, "GPotato is running maintenance from 10 am t0 10 pm." );
			break;
		case 3000:
			lstrcpy( result.szResult, "Your account was blocked by GPOTATO" );
			break;
	}
}
#endif	// __GPAUTH

#ifdef __JAPAN_AUTH
u_int __stdcall JapanAuthWorker( LPVOID pParam )
{
	CAccountMgr mgr;

	CQuery query;
	if( FALSE == query.Connect( 3, "login", "account", g_DbManager.m_szLoginPWD ) )
		AfxMessageBox( "can't connect to database : login" );

	SetEvent( s_hHandle );

	HANDLE hIOCP	= g_DbManager.GetIOCPHandle( (int)pParam );
	DWORD dwBytesTransferred	= 0;
	LPDWORD lpCompletionKey		= NULL;
	LPDB_OVERLAPPED_PLUS pov	= NULL;

	while( 1 )
	{
		BOOL fOk = GetQueuedCompletionStatus( hIOCP,
										 &dwBytesTransferred,
										(LPDWORD)&lpCompletionKey,
										(LPOVERLAPPED*)&pov,
										INFINITE );

		if( fOk == FALSE ) 
		{	
			ASSERT( 0 );				// fatal error
			continue;
		}

		if( dwBytesTransferred == 0 )	// terminate
			return( 0 );
		
		switch( pov->nQueryMode )
		{
			case CERTIFY:
				g_DbManager.Certify_Japan( query, pov, mgr );
				break;
			case CLOSE_EXISTING_CONNECTION:
				g_DbManager.CloseExistingConnection_Japan( query, pov );
				break;
		}
	}
	return( 0 );
}

void CDbManager::Certify_Japan( CQuery & query, LPDB_OVERLAPPED_PLUS pov, CAccountMgr & mgr )
{
	ACCOUNT_CHECK result	= CHECK_OK;
	if( pov->dwIP )
	{
		result	= mgr.Check( pov->dwIP );
		switch( result )
		{
			case CHECK_1TIMES_ERROR:
				g_dpCertifier.SendError( ERROR_15SEC_PREVENT, pov->dpId );
				m_pDbIOData->Free( pov );
				return;
			case CHECK_3TIMES_ERROR:
				g_dpCertifier.SendError( ERROR_15MIN_PREVENT, pov->dpId );
				m_pDbIOData->Free( pov );
				return;
		}
	}
	
	JAPAN_AUTH_RESULT r = GetJapanAuthResult( pov );
	if( r.nResult == JAPAN_AUTH_RESULT_AGREE )
	{
		if( pov->dwIP )
			mgr.SetError( 0 );

#ifdef __EUROPE_0514
		lstrcpy( pov->AccountInfo.szBak, pov->AccountInfo.szAccount );
#endif	// __EUROPE_0514
		OnCertifyQueryOK( query, pov );
	}
	else
	{
		g_dpCertifier.SendErrorString( r.szResultMsg, pov->dpId );
	}
	m_pDbIOData->Free( pov );
}

void CDbManager::CloseExistingConnection_Japan( CQuery & query, LPDB_OVERLAPPED_PLUS pov )
{
	JAPAN_AUTH_RESULT r = GetJapanAuthResult( pov );

	if( r.nResult == JAPAN_AUTH_RESULT_AGREE )
		g_dpAccountClient.SendCloseExistingConnection( pov->AccountInfo.szAccount );

	m_pDbIOData->Free( pov );
}

JAPAN_AUTH_RESULT GetJapanAuthResult( LPDB_OVERLAPPED_PLUS pov )
{
	JAPAN_AUTH_RESULT result;
	
	CInternetSession	session( _T( "Japan_Auth" ) );
	CHttpConnection*	pServer		= NULL;

	CHttpFile*	pFile	= NULL;
	char pszSendMsg[255]	= { 0,};
	char pszHeader[255]	= { 0,};
	CString strHeaders, strResult;
	int nReadSize	= 0;
	char szHeader[256] = "X-RESULT";
	DWORD dwHeaderSize = 255;
	
	char sAddr[16]	= { 0,};
	g_dpCertifier.GetPlayerAddr( pov->dpId, sAddr );
	
	sprintf( pszSendMsg, "/api/aeonsoft/auth.asp?ID=%s&PW=%s&IP=%s", pov->AccountInfo.szAccount, pov->AccountInfo.szPassword, sAddr );
	strHeaders	= "Content-Type: application/x-www-form-urlencoded";

	try
	{
		pServer		= session.GetHttpConnection( g_strWebCertURL.c_str() );
		if( !pServer )
		{
			Error( "\"%s\" Connection Failed!", g_strWebCertURL.c_str() );
			goto DONE;
		}
		
		pFile	= pServer->OpenRequest( CHttpConnection::HTTP_VERB_GET, pszSendMsg );
		if( !pFile  )
		{
			Error( "OpenRequest Failed!" );
			goto DONE;
		}

		if( !( pFile->SendRequest() ) )
		{
			Error( "SendRequest Failed! -> Msg : %s", pszSendMsg );
			goto DONE;
		}
	}
	catch( CInternetException* e )
	{
		TCHAR	szError[255]	= { 0,};
		e->GetErrorMessage( szError, 255 );
		goto DONE;
	}

	::HttpQueryInfo( *pFile, HTTP_QUERY_CUSTOM, (LPVOID)szHeader, &dwHeaderSize, NULL );
	if( lstrcmp( szHeader, "agree" ) == 0 )
		result.nResult = JAPAN_AUTH_RESULT_AGREE;
	else if( lstrcmp( szHeader, "reject" ) == 0 )
		result.nResult = JAPAN_AUTH_RESULT_REJECT;

	nReadSize	= (int)pFile->Read( strResult.GetBuffer( (int)( pFile->GetLength() ) ), (UINT)( pFile->GetLength() ) );

	strResult.ReleaseBuffer();
	if( nReadSize == strResult.GetLength() )
	{
		lstrcpy( result.szResultMsg, (LPCSTR)strResult );
	}

DONE:
	if( pFile )
	{
		pFile->Close();
		safe_delete( pFile );
		pFile	= NULL;
	}
	if( pServer )
	{
		pServer->Close();
		safe_delete( pServer );
		pServer		= NULL;
	}
	session.Close();
	
	return result;
}
#endif // __JAPAN_AUTH