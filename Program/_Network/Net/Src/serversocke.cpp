#include "stdafx.h"
#include "dpsock.h"
#include <process.h>

u_int __stdcall ListenThreadE( LPVOID lpvThreadParameter );

#ifdef __CRC
CServerSockE::CServerSockE( DWORD dwcrc )
#else	// __CRC
CServerSockE::CServerSockE( BUFFER_TYPE nBufferType )
#endif	//__CRC
{
	m_hListenThread		= (HANDLE)NULL;
	m_hClose	= (HANDLE)NULL;
	m_hListen	= WSA_INVALID_EVENT;
	m_pDPSock	= NULL;
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

CServerSockE::~CServerSockE()
{
//	Close();
}

BOOL CServerSockE::Listen( void )
{
	if( m_hSocket == INVALID_SOCKET )
		return FALSE;

	if( SOCKET_ERROR == WSAEventSelect( m_hSocket, m_hListen, FD_ACCEPT | FD_CONNECT ) )
	{
		TRACE( " Can't set event with error %d\n", WSAGetLastError() );
		return FALSE;
	}
	if( listen( m_hSocket, 5 ) == SOCKET_ERROR )
	{
		TRACE( "Listen failed with error %d\n", WSAGetLastError() );
		return FALSE;
	}
	return TRUE;
}

BOOL CServerSockE::StartServer( CDPSock* pDPSock )
{
	Close();
	m_pDPSock	= pDPSock;
	m_hListen	= WSACreateEvent();
	ASSERT( m_hListen != WSA_INVALID_EVENT );
	m_hClose	= CreateEvent( NULL, FALSE, FALSE, NULL );
	ASSERT( m_hClose );
	DWORD dwThreadId;
	m_hListenThread		= chBEGINTHREADEX( NULL, 0, ListenThreadE, this, NULL, &dwThreadId );
	ASSERT( m_hListenThread );
	SetThreadPriority( m_hListenThread, THREAD_PRIORITY_BELOW_NORMAL );
	return TRUE;
}

void CServerSockE::Close( void )
{
	CLOSE_WSAEVENT( m_hListen );
	CLOSE_THREAD( m_hListenThread, m_hClose );
	CloseAllChild();
}

CClientSockE* CServerSockE::AddChild( SOCKET hSocket )
{
#ifdef __CRC
	CClientSockE* pChild		= new CClientSockE( m_dwcrc );
#else	// __CRC
	CClientSockE* pChild		= new CClientSockE( m_nBufferType );
#endif	// __CRC
	pChild->Attach( hSocket );
	m_mapChild.Lock();
#ifdef __STL_0402
	bool bResult	= m_mapChild.insert( CMapChildE::value_type( hSocket, pChild ) ).second;
	ASSERT( bResult );
#else	// __STL_0402
	m_mapChild.SetAt( hSocket, pChild );
#endif	// __STL_0402
	m_mapChild.Unlock();
	return pChild;
}

BOOL CServerSockE::CloseChild( SOCKET hSocket )
{
	CMclAutoLock	Lock( m_mapChild.m_AddRemoveLock );
#ifdef __STL_0402
	CMapChildE::iterator i	= m_mapChild.find( hSocket );
	if( i != m_mapChild.end() )
	{
		CClientSockE* pChild	= i->second;
		m_mapChild.erase( i );
		pChild->Close();
		SAFE_DELETE( pChild );
		return TRUE;
	}
	return FALSE;
#else	// __STL_0402
	CClientSockE* pChild;
	if( m_mapChild.Lookup( hSocket, pChild ) )
	{
		m_mapChild.RemoveKey( hSocket );
		if( pChild ) {
			pChild->Close();
			SAFE_DELETE( pChild );
			return( TRUE );
		}
	}
	return( FALSE );
#endif	// __STL_0402
}

void CServerSockE::CloseAllChild( void )
{
	CMclAutoLock	Lock( m_mapChild.m_AddRemoveLock );

#ifdef __STL_0402
	for( CMapChildE::iterator i = m_mapChild.begin(); i != m_mapChild.end(); ++i )
	{
		CClientSockE* pChild	= i->second;
		TRACE( "~%x, %d\n", pChild, pChild->GetHandle() );
		SAFE_DELETE( pChild );
	}
	m_mapChild.clear();
#else	// __STL_0402
	CMyBucket<CClientSockE*>* pBucket;
	CClientSockE* pChild;
	pBucket		= m_mapChild.GetFirstActive();
	while( pBucket )
	{
		pChild	= pBucket->m_value;
		TRACE( "~%x, %d\n", pChild, pChild->GetHandle() );
		SAFE_DELETE( pChild );
		pBucket	= pBucket->pNext;
	}
	m_mapChild.RemoveAll();
#endif	// __STL_0402
}

void CServerSockE::Send( char* lpData, DWORD dwDataSize, DPID dpidTo )
{
	if( dpidTo == DPID_ALLPLAYERS )
	{
		CMclAutoLock	Lock( m_mapChild.m_AddRemoveLock );

#ifdef __STL_0402
		for( CMapChildE::iterator i = m_mapChild.begin(); i != m_mapChild.end(); ++i )
		{
			CClientSockE* pChild	= i->second;
			pChild->Send( lpData, dwDataSize, pChild->GetHandle() );
		}
#else	// __STL_0402
		CClientSockE* pChild;
		CMyBucket<CClientSockE*>* pBucket	= m_mapChild.GetFirstActive();
		while( pBucket )
		{
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
		CClientSockE* pChild	= (CClientSockE*)Get( dpidTo );
		if( pChild )
			pChild->Send( lpData, dwDataSize, dpidTo );
#else	// __STL_0402
		CClientSockE* pChild;
		if( m_mapChild.Lookup( dpidTo, pChild ) )
			pChild->Send( lpData, dwDataSize, dpidTo );
#endif	// __STL_0402
	}
	
}

BOOL CServerSockE::CloseConnection( SOCKET hSocket )
{
	return CloseChild( hSocket );
}

BOOL CServerSockE::Shutdown( SOCKET hSocket )
{
	return FALSE;
}

HRESULT CServerSockE::GetPeerAddr( DPID dpid, LPVOID lpAddr, LPDWORD lpdwSize )
{
	ASSERT( lpAddr );

	CClientSockE* pSock;
	SOCKET hSocket	= (SOCKET)dpid;
	SOCKADDR_IN sin;

	ZeroMemory( &sin, sizeof(SOCKADDR_IN) );

#ifdef __STL_0402
	pSock	= (CClientSockE*)Get( hSocket );
#else	// __STL_0402
	m_mapChild.Lookup( hSocket, pSock );
#endif	// __STL_0402
	if( !pSock )
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

u_int __stdcall ListenThreadE( LPVOID lpvThreadParameter )
{
	CServerSockE* pSock	= (CServerSockE*) lpvThreadParameter;
	HANDLE handle[3];
	DWORD dw;
	WSANETWORKEVENTS ne;
	
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
					int nSize;
					SOCKADDR addr;
					SOCKET hSocket;
					
					nSize	= sizeof(SOCKADDR);
					WSAEnumNetworkEvents( pSock->GetHandle(), pSock->GetListenHandle(), &ne );
					if( ne.lNetworkEvents != FD_ACCEPT )
						break;

					if( ( hSocket	= WSAAccept( pSock->GetHandle(), &addr, &nSize, NULL, 0 ) ) == INVALID_SOCKET )
					{
						TRACE( "Accept failed with error %d\n", WSAGetLastError() );
						break;
					}
					CClientSockE* pChild	= pSock->AddChild( hSocket );
					pChild->SetID( DPID_SERVERPLAYER );
					pChild->SetPeerID( hSocket );

					ASSERT( pSock->m_pDPSock );
					pSock->m_pDPSock->CreateEventWorker( pChild );

					int zero	= 0;
					setsockopt( hSocket, SOL_SOCKET, SO_SNDBUF, (char*)&zero, sizeof(zero) );
					setsockopt( hSocket, SOL_SOCKET, SO_RCVBUF, (char*)&zero, sizeof(zero) );
					InterlockedIncrement( &pChild->m_l );

#ifdef __PROTOCOL0910
					if( pSock->m_dwcrc & CSock::crcRead )
						pChild->SendProtocolId();
#endif	// __PROTOCOL0910

					if( pChild->Recv() != 0 )
					{
						TRACE( "I/0 error %d\n", WSAGetLastError() );
						pSock->CloseChild( hSocket );
					}
					break;
				}
			case WAIT_TIMEOUT :
				break;
		}
	}
	return( 0 );
}