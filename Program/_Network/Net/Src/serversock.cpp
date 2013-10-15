#include "stdafx.h"
#include "dpsock.h"
#include <process.h>
#ifdef __DOS1101
#include "dos2.h"
#endif	// __DOS1101

#ifdef __NEWDPMNG
#include <mswsock.h>
#pragma comment( lib, "mswsock.lib" )
u_int	__stdcall	AcptThread( LPVOID lpParam );
#else	// __NEWDPMNG
u_int __stdcall ListenThread( LPVOID lpParam );
#endif	// __NEWDPMNG

#ifdef __CRASH_0404
#include "crashstatus.h"
#endif	// __CRASH_0404

#ifdef	__VERIFY_LOOP041010
	#undef INIT_LOOP
	#undef VERIFY_LOOP

	#define	INIT_LOOP					int __loop = 0;
	#define VERIFY_LOOP(file, line)		if( ++__loop == 1000 ) {	Error( "%s %d", file, line ); }
#else
	#undef INIT_LOOP
	#undef VERIFY_LOOP

	#define	INIT_LOOP					((void)0);
	#define VERIFY_LOOP(file, line)		((void)0);
#endif	




#ifdef __CRC
CServerSock::CServerSock( DWORD dwcrc )
#else	// __CRC
CServerSock::CServerSock( BUFFER_TYPE nBufferType )
#endif	// __CRC
{
#ifdef __NEWDPMNG
	m_hIocpAcpt		= (HANDLE)NULL;
	m_hAcptThread	= (HANDLE)NULL;
	for( int i = 0; i < MAX_IO_PEND; i++ )
		m_socklist[i].sock	= INVALID_SOCKET;		
#else	// __NEWDPMNG
	m_hListenThread		= (HANDLE)NULL;
	m_hClose	= (HANDLE)NULL;
	m_hListen	= WSA_INVALID_EVENT;
#endif	// __NEWDPMNG

	m_phCompletionPort	= NULL;

	m_uIoWorker		= 0;

#ifndef __STL_0402
	m_mapChild.SetSize( 512, 4096, 128 );
#endif	// __STL_0402

#ifdef __CRC
	m_dwcrc		= dwcrc;
	m_nBufferType = BUFFER_TYPE_5BYTE;
#else
	m_nBufferType = nBufferType;
#endif	// __CRC
}

CServerSock::~CServerSock()
{
	Close();
}

BOOL CServerSock::Listen( void )
{
	if( m_hSocket == INVALID_SOCKET )
		return FALSE;

#ifndef __NEWDPMNG
	if( SOCKET_ERROR == WSAEventSelect( m_hSocket, m_hListen, FD_ACCEPT | FD_CONNECT ) )
	{
		TRACE( " Can't set event with error %d\n", WSAGetLastError() );
		return FALSE;
	}
#endif	// __NEWDPMNG

	if( listen( m_hSocket, /*5*/SOMAXCONN ) == SOCKET_ERROR )
	{
		TRACE( "Listen failed with error %d\n", WSAGetLastError() );
		return FALSE;
	}

#ifdef __NEWDPMNG
	m_hIocpAcpt		= CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );
	ASSERT( m_hIocpAcpt != NULL );
	CreateIoCompletionPort( (HANDLE)m_hSocket, m_hIocpAcpt, 0, 0 );
	for( int i = 0; i < MAX_IO_PEND; i++ )
		AcptRequest( i );
	ResumeThread( m_hAcptThread );
#endif	// __NEWDPMNG

	return TRUE;
}

#ifdef __NEWDPMNG
BOOL CServerSock::AcptRequest( int iIndex )
{
	if( iIndex >= MAX_IO_PEND )
		return FALSE;

	static	char	pData[MAX_BYTES];
	PSocklist pSocklist		= &m_socklist[iIndex];
	memset( pSocklist, 0, sizeof(Socklist) );

//	pSocklist->sock	= socket( AF_INET, SOCK_STREAM, 0 );
	if( ( pSocklist->sock = WSASocket( AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED ) ) == INVALID_SOCKET )
	{
		TRACE( "Can't create socket with error %d\n", WSAGetLastError() );
		return FALSE;
	}
	pSocklist->iIndex	= iIndex;
	DWORD dwBytes	= 0;
	if( AcceptEx( m_hSocket, pSocklist->sock, pData, 0, PADDED_ADDR_SIZE, PADDED_ADDR_SIZE,
			&dwBytes, (LPOVERLAPPED)pSocklist ) == FALSE )
	{
		if( WSAGetLastError() != ERROR_IO_PENDING )
		{
			TRACE( "WSAGetLastError: %d\nError on AcceptEx()\n", WSAGetLastError() );
			return FALSE;
		}
	}
	/*
	In our contrived scenario, we expect GetLastError to be
	ERROR_IO_PENDING.  A real app would do other work.
	Since this is a simple sample, we will just block waiting
	for some input to come through the socket.
	*/
	return TRUE;
}
#endif	// __NEWDPMNG

BOOL CServerSock::StartServer( HANDLE* phCompletionPort, u_long uIoWorker )
{
	Close();

	if( phCompletionPort == NULL )
		return FALSE;
	for( DWORD i = 0; i < uIoWorker; i++ )
	{
		if( phCompletionPort[i] == (HANDLE)NULL )
			return FALSE;
	}
	m_phCompletionPort	= phCompletionPort;
	m_uIoWorker	= uIoWorker;

#ifdef __NEWDPMNG
	DWORD dwThreadId;
	m_hAcptThread	= chBEGINTHREADEX( NULL, 0, AcptThread, this, NULL, &dwThreadId );
	ASSERT( m_hAcptThread );
	SetThreadPriority( m_hAcptThread, THREAD_PRIORITY_BELOW_NORMAL );
	SuspendThread( m_hAcptThread );
#else	// __NEWDPMNG
	m_hListen	= WSACreateEvent();
	ASSERT( m_hListen != WSA_INVALID_EVENT );
	m_hClose	= CreateEvent( NULL, FALSE, FALSE, NULL );
	ASSERT( m_hClose );
	DWORD dwThreadId;
	m_hListenThread		= chBEGINTHREADEX( NULL, 0, ListenThread, this, NULL, &dwThreadId );
	ASSERT( m_hListenThread );
	SetThreadPriority( m_hListenThread, THREAD_PRIORITY_BELOW_NORMAL );
#endif	// __NEWDPMNG

	return TRUE;
}

void CServerSock::Close( void )
{
#ifdef __NEWDPMNG
	if( m_hAcptThread )
	{
		PostQueuedCompletionStatus( m_hIocpAcpt, 0, 0, NULL );
		WaitForSingleObject( m_hAcptThread, INFINITE );
		CloseHandle( m_hIocpAcpt );
		CloseHandle( m_hAcptThread );
		m_hIocpAcpt		= m_hAcptThread		= (HANDLE)NULL;
	}
	for( int i = 0; i < MAX_IO_PEND; i++ )
	{
		if( m_socklist[i].sock != INVALID_SOCKET )
		{
			shutdown( m_socklist[i].sock, SD_BOTH );
			closesocket( m_socklist[i].sock );
			m_socklist[i].sock	= INVALID_SOCKET;
		}
	}
#else	// __NEWDPMNG
	CLOSE_WSAEVENT( m_hListen );
	CLOSE_THREAD( m_hListenThread, m_hClose );
#endif	// __NEWDPMNG

	CloseAllChild();
}

CClientSock* CServerSock::AddChild( SOCKET hSocket )
{
#ifdef __CRC
	CClientSock* pChild		= new CClientSock( m_dwcrc );
#else	// __CRC
	CClientSock* pChild		= new CClientSock( m_nBufferType );
#endif	// __CRC
	pChild->Attach( hSocket );

	m_mapChild.Lock();

#ifdef __US_LOGIN_0223

#ifdef __STL_0402
	CMapChild::iterator i	= m_mapChild.find( hSocket );
	if( i != m_mapChild.end() )
	{
		CClientSock* ptr	= i->second;
		ptr->Detach();
		m_mapChild.erase( i );
		SAFE_DELETE( ptr );
#ifdef __CRASH_0404
		CCrashStatus::GetInstance()->FreeChild();
#endif	// __CRASH_0404
	}
#else	// __STL_0402
	CClientSock* ptr	= NULL;
	if( m_mapChild.Lookup( hSocket, ptr ) )
	{
		ptr->Detach();	//
		m_mapChild.RemoveKey( hSocket );
		SAFE_DELETE( ptr );
#ifdef __CRASH_0404
		CCrashStatus::GetInstance()->FreeChild();
#endif	// __CRASH_0404
	}
#endif	// __STL_0402
#endif	// __US_LOGIN_0223

#ifdef __STL_0402
	bool bResult	= m_mapChild.insert( CMapChild::value_type( hSocket, pChild ) ).second;
//	ASSERT( bResult );
#else	// __STL_0402
	m_mapChild.SetAt( hSocket, pChild );
#endif	// __STL_0402

	m_mapChild.Unlock();
	return pChild;
}

BOOL CServerSock::CloseChild( SOCKET hSocket )
{
	CMclAutoLock	Lock( m_mapChild.m_AddRemoveLock );

#ifdef __STL_0402
	CMapChild::iterator i	= m_mapChild.find( hSocket );
	if( i != m_mapChild.end() )
	{
		CClientSock* pChild	= i->second;
		m_mapChild.erase( i );
		pChild->Close();
		SAFE_DELETE( pChild );
#ifdef __CRASH_0404
		CCrashStatus::GetInstance()->FreeChild();
#endif	// __CRASH_0404
		return TRUE;
	}
	return FALSE;
#else	// __STL_0402
	CClientSock* pChild;
	if( m_mapChild.Lookup( hSocket, pChild ) )
	{
		m_mapChild.RemoveKey( hSocket );
		if( pChild ) {
			pChild->Close();
			SAFE_DELETE( pChild );
#ifdef __CRASH_0404
			CCrashStatus::GetInstance()->FreeChild();
#endif	// __CRASH_0404
			return( TRUE );
		}
	}
	return( FALSE );
#endif	// __STL_0402
}

void CServerSock::CloseAllChild( void )
{
	CMclAutoLock	Lock( m_mapChild.m_AddRemoveLock );

#ifdef __STL_0402
	for( CMapChild::iterator i = m_mapChild.begin(); i != m_mapChild.end(); ++i )
	{
		CClientSock* pChild	= i->second;
		TRACE( "~%x, %d\n", pChild, pChild->GetHandle() );
		pChild->Close();
		SAFE_DELETE( pChild );
#ifdef __CRASH_0404
		CCrashStatus::GetInstance()->FreeChild();
#endif	// __CRASH_0404
	}
	m_mapChild.clear();
#else	// __STL_0402
	CMyBucket<CClientSock*>* pBucket;
	CClientSock* pChild;

	pBucket		= m_mapChild.GetFirstActive();

	INIT_LOOP;
	while( pBucket )
	{
		VERIFY_LOOP( __FILE__, __LINE__ );

		pChild	= pBucket->m_value;
		TRACE( "~%x, %d\n", pChild, pChild->GetHandle() );
		pChild->Close();
		SAFE_DELETE( pChild );
#ifdef __CRASH_0404
		CCrashStatus::GetInstance()->FreeChild();
#endif	// __CRASH_0404
		pBucket	= pBucket->pNext;
	}
	m_mapChild.RemoveAll();
#endif	// __STL_0402
}

#ifdef __DOS1101
void CServerSock::ShutdownAddr( DWORD dwAddr )
{
	CMclAutoLock	Lock( m_mapChild.m_AddRemoveLock );

#ifdef __STL_0402
	for( CMapChild::iterator i = m_mapChild.begin(); i != m_mapChild.end(); ++i )
	{
		CClientSock* pChild	= i->second;
		if( pChild->m_dwAddr == dwAddr )
			shutdown( pChild->GetHandle(), SD_BOTH );
	}
#else	// __STL_0402
	CClientSock* pChild;
	CMyBucket<CClientSock*>* pBucket	= m_mapChild.GetFirstActive();
	while( pBucket )
	{
		pChild	= pBucket->m_value;
		if( pChild->m_dwAddr == dwAddr )
			shutdown( pChild->GetHandle(), SD_BOTH );
		pBucket	= pBucket->pNext;
	}
#endif	// __STL_0402
}
#endif	// __DOS1101

void CServerSock::Send( char* lpData, DWORD dwDataSize, DPID dpidTo )
{
	if( dpidTo == DPID_ALLPLAYERS )
	{
		CMclAutoLock	Lock( m_mapChild.m_AddRemoveLock );
#ifdef __STL_0402
		for( CMapChild::iterator i = m_mapChild.begin(); i != m_mapChild.end(); ++i )
		{
			CClientSock* pChild	= i->second;
			pChild->Send( lpData, dwDataSize, pChild->GetHandle() );
		}
#else	// __STL_0402
		CClientSock* pChild;
		CMyBucket<CClientSock*>* pBucket	= m_mapChild.GetFirstActive();
		INIT_LOOP;
		while( pBucket )
		{
			VERIFY_LOOP( __FILE__, __LINE__ );
			pChild	= pBucket->m_value;
			pChild->Send( lpData, dwDataSize, pChild->GetHandle() );
			pBucket	= pBucket->pNext;
		}
#endif	// __STL_0402
	}
	else
	{
		CMclAutoLock	Lock( m_mapChild.m_AddRemoveLock );
#ifdef __STL_0402
		CClientSock* pChild		= (CClientSock*)Get( dpidTo );
		if( pChild )
			pChild->Send( lpData, dwDataSize, dpidTo );
#else	// __STL_0402
		CClientSock* pChild;
		if( m_mapChild.Lookup( dpidTo, pChild ) )
			pChild->Send( lpData, dwDataSize, dpidTo );
#endif	// __STL_0402
	}
	
}

BOOL CServerSock::CloseConnection( SOCKET hSocket )
{
	return CloseChild( hSocket );
}

BOOL CServerSock::Shutdown( SOCKET hSocket )
{
	CMclAutoLock	Lock( m_mapChild.m_AddRemoveLock );
#ifdef __STL_0402
	CClientSock* pChild		= (CClientSock*)Get( hSocket );
	if( pChild )
	{
		closesocket( pChild->GetHandle() );
		return TRUE;
	}
	return FALSE;
#else	// __STL_0402
	CClientSock* pChild;
	if( m_mapChild.Lookup( hSocket, pChild ) )
	{
		ASSERT( pChild );
		closesocket( pChild->GetHandle() );
		return TRUE;
	}
	return( FALSE );
#endif	// __STL_0402
}

#ifdef __INFO_SOCKLIB0516
DWORD CServerSock::GetDebugInfo( SOCKET hSocket )
{
	CMclAutoLock	Lock( m_mapChild.m_AddRemoveLock );
#ifdef __STL_0402
	CClientSock* pChild	= (CClientSock*)Get( hSocket );
	if( pChild )
		return pChild->GetDebugInfo( hSocket );
	return 0;
#else	// __STL_0402
	CClientSock* pChild;
	if( m_mapChild.Lookup( hSocket, pChild ) )
	{
		ASSERT( pChild );
		return pChild->GetDebugInfo( hSocket );
	}
	return 0;
#endif	// __STL_0402
}
#endif	// __INFO_SOCKLIB0516

HRESULT CServerSock::GetPeerAddr( DPID dpid, LPVOID lpAddr, LPDWORD lpdwSize )
{
	ASSERT( lpAddr );
	CMclAutoLock	Lock( m_mapChild.m_AddRemoveLock );

	CClientSock* pSocket;
	SOCKET hSocket	= (SOCKET)dpid;
	SOCKADDR_IN sin;

	ZeroMemory( &sin, sizeof(SOCKADDR_IN) );

#ifdef __STL_0402
	pSocket		= (CClientSock*)Get( hSocket );
#else	// __STL_0402
	m_mapChild.Lookup( hSocket, pSocket );
#endif	// __STL_0402
	if( !pSocket )
		return DPERR_INVALIDPARAM;

	int namelen	= sizeof(sin);
	if( getpeername( hSocket, (SOCKADDR *)&sin, &namelen ) == SOCKET_ERROR )
		return DPERR_GENERIC;

	sprintf( (char *)lpAddr, "%d.%d.%d.%d", sin.sin_addr.s_net
																	, sin.sin_addr.s_host
																	, sin.sin_addr.s_lh
																	, sin.sin_addr.s_impno );
	return DP_OK;
}

DWORD CServerSock::GetPeerAddr( DPID dpid )
{
	CMclAutoLock	Lock( m_mapChild.m_AddRemoveLock );

#ifdef __STL_0402
	CClientSock* pSocket	= (CClientSock*)Get( dpid );
#else	// __STL_0402
	CClientSock* pSocket = NULL;
	m_mapChild.Lookup( (SOCKET)dpid, pSocket );
#endif	// __STL_0402

	if( !pSocket )
		return 0;

	return pSocket->GetPeerAddr( dpid );
}


HRESULT CServerSock::GetPeerAddr( SOCKET hSocket, u_long* paddr )
{
	SOCKADDR_IN sin;
	ZeroMemory( &sin, sizeof(SOCKADDR_IN) );
	int namelen	= sizeof(sin);
	if( getpeername( hSocket, (SOCKADDR *)&sin, &namelen ) == SOCKET_ERROR )
		return DPERR_GENERIC;
	*paddr	= sin.sin_addr.S_un.S_addr;
	return DP_OK;
}

BOOL CServerSock::AllocAndAssoc( SOCKET hSocket, DWORD dwAddr )
{
	CClientSock* pChild	= AddChild( hSocket );
	pChild->SetID( DPID_SERVERPLAYER );
	pChild->SetPeerID( hSocket );
	pChild->m_dwAddr	= dwAddr;

	int iIoWorker	= ( pChild->GetHandle() / 4 ) % GetIoWorkerCount();
	if( !CreateIoCompletionPort( (HANDLE) pChild->GetHandle(), GetCompletionPort( iIoWorker ), (DWORD)hSocket, GetIoWorkerCount() ) )
	{
		TRACE( "CreateIoCompletionPort Failure with error =%d\n", WSAGetLastError() );
		CloseChild( hSocket );
		return FALSE;
	}
#ifdef __CRASH_0404
	CCrashStatus::GetInstance()->AllocChild();
#endif	// __CRASH_0404
	return TRUE;
}

#ifdef __NEWDPMNG
//{{AFX
u_int	__stdcall	AcptThread( LPVOID lpParam )
{
	CServerSock* pSock	= (CServerSock*)lpParam;
	DWORD dwCmpKey;
	DWORD dwBytes;
	PSocklist pSocklist;

	while( 1 )
	{
		if( GetQueuedCompletionStatus(
							pSock->m_hIocpAcpt,
							&dwBytes,
							(LPDWORD)&dwCmpKey,
							(LPOVERLAPPED *)&pSocklist,
							INFINITE ) 
							)
		{
			if( !pSocklist )
				return 0;

			int zero	= 0;
			setsockopt( pSocklist->sock, SOL_SOCKET, SO_SNDBUF, (char *)&zero, sizeof(zero) );
			setsockopt( pSocklist->sock, SOL_SOCKET, SO_RCVBUF, (char *)&zero, sizeof(zero) );

			CClientSock* pChild	= pSock->AddChild( pSocklist->sock );
			pChild->SetID( DPID_SERVERPLAYER );
			pChild->SetPeerID( pSocklist->sock );


			if( !CreateIoCompletionPort( (HANDLE) pChild->GetHandle(), pSock->GetCompletionPort(), (DWORD)pSocklist->sock, pSock->GetIoWorkerCount() ) )
			{
				TRACE( "CreateIoCompletionPort Failure with error =%d\n", WSAGetLastError() );
				pSock->CloseChild( pSocklist->sock );
			}
			else
			{
				PostQueuedCompletionStatus( pSock->GetCompletionPort(), NEWSOCKETMSG, (DWORD)pSocklist->sock, NULL );
			}

			pSock->AcptRequest( pSocklist->iIndex );
		}
	}
}
//}}AFX
#else	// __NEWDPMNG
u_int __stdcall ListenThread( LPVOID lpParam )
{
	CServerSock* pSock	= (CServerSock*)lpParam;
	HANDLE handle[3];
	DWORD dw;
	WSANETWORKEVENTS ne;
	DWORD	dwAddr;
#ifdef __DOS1101
	CDos2	dos2;
#endif	// __DOS1101
	
	handle[0]	= pSock->GetCloseHandle();
	handle[1]	= pSock->GetListenHandle();
	handle[2]	= NULL;
	while( 1 )
	{
		dw	= WaitForMultipleObjects( 2, handle, FALSE, INFINITE );
		switch( dw )
		{
			case ( WAIT_OBJECT_0 ):
				return( 0 );
			case ( WAIT_OBJECT_0+1 ) :
				{
					WSAEnumNetworkEvents( pSock->GetHandle(), pSock->GetListenHandle(), &ne );
					if( ne.lNetworkEvents != FD_ACCEPT )
						break;

					SOCKADDR_IN addr;
					memset( &addr, 0x00, sizeof( addr ) );
					int nSize = sizeof(addr);
					SOCKET hSocket = WSAAccept( pSock->GetHandle(), (SOCKADDR *)&addr, &nSize, NULL, 0 ); 
					if( hSocket  == INVALID_SOCKET )
						break;

					dwAddr = addr.sin_addr.S_un.S_addr;

#ifdef __DOS1101
					int i = dos2.IsOver( dwAddr );
					if( i < 0 )
					{	// close
						pSock->ShutdownAddr( dwAddr );
						break;
					}
					else if( i > 0 )
					{	// close this socket
						CLOSE_SOCKET( hSocket );
						break;
					}
#endif	// __DOS1101

					int zero = 0;
					setsockopt( hSocket, SOL_SOCKET, SO_SNDBUF, (char *)&zero, sizeof(zero) );
					setsockopt( hSocket, SOL_SOCKET, SO_RCVBUF, (char *)&zero, sizeof(zero) );

					int iIoWorker	= ( hSocket / 4 ) % pSock->GetIoWorkerCount();
#ifndef __PROTOCOL0910
					PostQueuedCompletionStatus( pSock->GetCompletionPort( iIoWorker ), NEWSOCKETMSG, (DWORD)hSocket, (LPOVERLAPPED)dwAddr );
#else	// __PROTOCOL0910
					if( pSock->m_dwcrc & CSock::crcRead )
					{
						PostQueuedCompletionStatus( pSock->GetCompletionPort( iIoWorker ), NEWSOCKETMSG2, (DWORD)hSocket, (LPOVERLAPPED)dwAddr );
					}
					else
					{
						PostQueuedCompletionStatus( pSock->GetCompletionPort( iIoWorker ), NEWSOCKETMSG, (DWORD)hSocket, (LPOVERLAPPED)dwAddr );
					}
#endif	// __PROTOCOL0910
					break;
				}
			case WAIT_TIMEOUT :
				break;
		}
	}
	return( 0 );
}
#endif	// __NEWDPMNG
