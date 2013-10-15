#ifndef __SERVERSOCK_H__
#define __SERVERSOCK_H__

#pragma once

#include "clientsock.h"
#include "mymap.h"

#define	NEWSOCKETMSG	(DWORD)-3
#define	CLOSEIOWORKERMSG	(DWORD)-2

#ifdef __DPMNG0412
#include <mswsock.h>
#pragma comment( lib, "mswsock.lib" )

typedef struct _socklist : public OVERLAPPED
{
	SOCKET	sock;
	int		iIndex;
}	Socklist, *PSocklist;
#define	MAX_IO_PEND	128		// maximum pending I/0 requests
#define	PADDED_ADDR_SIZE	sizeof(SOCKADDR_IN)+16
#define	MAX_BYTES	2000
#endif	// __DPMNG0412

#ifdef __STL_0402
template <class T>
class CMapChild : public map<DPID,CClientSock<T>*>
{
public:
	CMapChild()	{}
	virtual	~CMapChild()	{}
	void	Lock( void )	{	m_AddRemoveLock.Enter();	}
	void	Unlock( void )	{	m_AddRemoveLock.Leave();	}
public:
	CMclCritSec		m_AddRemoveLock;
};
#endif	// __STL_0402

template <class T>
class CServerSock : public CSock  
{
public:
#ifndef __DPMNG0412
	HANDLE	m_hListenThread;
	HANDLE	m_hClose;
	WSAEVENT	m_hListen;
#else	// __DPMNG0412
	HANDLE	m_hAcptThread;
	HANDLE	m_hIocpAcpt;
	Socklist	m_socklist[MAX_IO_PEND];
	BOOL	AcptRequest( int iIndex );
#endif	// __DPMNG0412

#ifdef __STL_0402
	CMapChild<T>	m_mapChild;
#else	// __STL_0402
	CMyMap<CClientSock<T>*>	m_mapChild;
#endif	// __STL_0402


	HANDLE	m_hCompletionPort;
	u_long	m_uIoWorker;

public:
//	Constructions
	CServerSock( DWORD dwcrc );
	virtual	~CServerSock();
//	Operations
	BOOL	Listen( void );

	HANDLE	GetListenHandle( void );
	HANDLE	GetCloseHandle( void );

	HANDLE	GetCompletionPort( void );
	int		GetIoWorkerCount( void );
	CClientSock<T>*	AddChild( SOCKET hSocket );
	BOOL	CloseChild( SOCKET hSocket );
	void	CloseAllChild( void );
	BOOL	StartServer( HANDLE hCompletionPort, u_long uIoWorker );
//	HRESULT	GetPeerAddr( SOCKET hSocket, u_long* paddr );

#ifdef __DPMNG0412
	static	UINT	_AcptThread( LPVOID pParam )
		{
			CServerSock<T>* pSock	= (CServerSock<T>*)pParam;
			return pSock->AcptThread();
		}
	int		AcptThread( void );
#else	// __DPMNG0412
	static	UINT	_ListenThread( LPVOID pParam )
		{
			CServerSock<T>* pSock	= (CServerSock<T>*)pParam;
			return pSock->ListenThread();
		}
	int		ListenThread( void );
#endif	// __DPMNG0412

//	Override
	virtual	void	Close( void );
	virtual	BOOL	CloseConnection( SOCKET hSocket );
	virtual	BOOL	Shutdown( SOCKET hSocket );
	virtual	void	Send( char* lpData, DWORD dwDataSize, DPID dpidTo );
	virtual	HRESULT		GetPeerAddr( DPID dpid, LPVOID lpAddr, LPDWORD lpdwSize );
	virtual	CSock*	Get( SOCKET hSocket );
};

#ifndef __DPMNG0412
template <class T>
inline HANDLE CServerSock<T>::GetListenHandle( void )
	{	return m_hListen; }

template <class T>
inline HANDLE CServerSock<T>::GetCloseHandle( void )
	{	return m_hClose;	}
#endif	// __DPMNG0412

template <class T>
inline HANDLE CServerSock<T>::GetCompletionPort( void )
	{	return m_hCompletionPort;	}

template <class T>
inline int CServerSock<T>::GetIoWorkerCount( void )
	{	return m_uIoWorker;	}

template <class T>
inline CSock* CServerSock<T>::Get( SOCKET hSocket )
	{
#ifdef __STL_0402
	CMapChild<T>::iterator i	= m_mapChild.find( hSocket );
	if( i != m_mapChild.end() )
		return (CSock*)i->second;
	return NULL;
#else	// __STL_0402
		CClientSock<T>* pChild;
		if( m_mapChild.Lookup( hSocket, pChild ) )
			return (CSock*)pChild;
		return NULL;
#endif	// __STL_0402
	}

template <class T>
CServerSock<T>::CServerSock( DWORD dwcrc )
	{
		m_dwcrc		= dwcrc;

	#ifdef __DPMNG0412
		m_hIocpAcpt		= (HANDLE)NULL;
		m_hAcptThread	= (HANDLE)NULL;
		for( int i = 0; i < MAX_IO_PEND; i++ )
			m_socklist[i].sock	= INVALID_SOCKET;		
	#else	// __DPMNG0412
		m_hListenThread		= (HANDLE)NULL;
		m_hClose	= (HANDLE)NULL;
		m_hListen	= WSA_INVALID_EVENT;
	#endif	// __DPMNG0412

		m_hCompletionPort	= (HANDLE)NULL;
		m_uIoWorker		= 0;
#ifndef __STL_0402
		m_mapChild.SetSize( 512, 4096, 128 );
#endif	// __STL_0402
	}

template <class T>
CServerSock<T>::~CServerSock()
	{	Close();	}

template <class T>
BOOL CServerSock<T>::Listen( void )
	{
		if( m_hSocket == INVALID_SOCKET )
			return FALSE;

	#ifndef __DPMNG0412
		if( SOCKET_ERROR == WSAEventSelect( m_hSocket, m_hListen, FD_ACCEPT | FD_CONNECT ) )
		{
			TRACE( " Can't set event with error %d\n", WSAGetLastError() );
			return FALSE;
		}
	#endif	// __DPMNG0412

		if( listen( m_hSocket, /*5*/SOMAXCONN ) == SOCKET_ERROR )
		{
			TRACE( "Listen failed with error %d\n", WSAGetLastError() );
			return FALSE;
		}

	#ifdef __DPMNG0412
		m_hIocpAcpt		= CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );
		ASSERT( m_hIocpAcpt != NULL );
		CreateIoCompletionPort( (HANDLE)m_hSocket, m_hIocpAcpt, 0, 0 );
		for( int i = 0; i < MAX_IO_PEND; i++ )
			AcptRequest( i );
		ResumeThread( m_hAcptThread );
	#endif	// __DPMNG0412

		return TRUE;
	}

#ifdef __DPMNG0412
template <class T>
BOOL CServerSock<T>::AcptRequest( int iIndex )
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
#endif	// __DPMNG0412

template <class T>
BOOL CServerSock<T>::StartServer( HANDLE hCompletionPort, u_long uIoWorker )
	{
		Close();

		if( hCompletionPort == (HANDLE)NULL )
			return FALSE;

		m_hCompletionPort	= hCompletionPort;
		m_uIoWorker	= uIoWorker;

	#ifdef __DPMNG0412
		DWORD dwThreadId;
		m_hAcptThread	= chBEGINTHREADEX( NULL, 0, _AcptThread, this, NULL, &dwThreadId );
		ASSERT( m_hAcptThread );
		SetThreadPriority( m_hAcptThread, THREAD_PRIORITY_BELOW_NORMAL );
		SuspendThread( m_hAcptThread );
	#else	// __DPMNG0412
		m_hListen	= WSACreateEvent();
		ASSERT( m_hListen != WSA_INVALID_EVENT );
		m_hClose	= CreateEvent( NULL, FALSE, FALSE, NULL );
		ASSERT( m_hClose );
		DWORD dwThreadId;
		m_hListenThread		= chBEGINTHREADEX( NULL, 0, _ListenThread, this, NULL, &dwThreadId );
		ASSERT( m_hListenThread );
		SetThreadPriority( m_hListenThread, THREAD_PRIORITY_BELOW_NORMAL );
	#endif	// __DPMNG0412

		return TRUE;
	}

template <class T>
void CServerSock<T>::Close( void )
	{
	#ifdef __DPMNG0412
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
	#else	// __DPMNG0412
		CLOSE_WSAEVENT( m_hListen );
		CLOSE_THREAD( m_hListenThread, m_hClose );
	#endif	// __DPMNG0412

		CloseAllChild();
	}

template <class T>
CClientSock<T>* CServerSock<T>::AddChild( SOCKET hSocket )
	{
		CClientSock<T>* pChild	= new CClientSock<T>( m_dwcrc );
		pChild->Attach( hSocket );
		m_mapChild.Lock();

#ifdef __US_LOGIN_0223

#ifdef __STL_0402
	CMapChild<T>::iterator i	= m_mapChild.find( hSocket );
	if( i != m_mapChild.end() )
	{
		CClientSock<T>* ptr	= i->second;
		ptr->Detach();
		m_mapChild.erase( i );
		SAFE_DELETE( ptr );
	}
#else	// __STL_0402
	CMapChild<T>* ptr	= NULL;
	if( m_mapChild.Lookup( hSocket, ptr ) )
	{
		ptr->Detach();	//
		m_mapChild.RemoveKey( hSocket );
		SAFE_DELETE( ptr );
	}
#endif	// __STL_0402
#endif	// __US_LOGIN_0223

#ifdef __STL_0402
		bool bResult	= m_mapChild.insert( CMapChild<T>::value_type( hSocket, pChild ) ).second;
//		ASSERT( bResult );
#else	// __STL_0402
		m_mapChild.SetAt( hSocket, pChild );
#endif	// __STL_0402
		m_mapChild.Unlock();
		return pChild;
	}

template <class T>
BOOL CServerSock<T>::CloseChild( SOCKET hSocket )
	{
		CMclAutoLock	Lock( m_mapChild.m_AddRemoveLock );
#ifdef __STL_0402
		CMapChild<T>::iterator i = m_mapChild.find( hSocket );
		if( i != m_mapChild.end() )
		{
			CClientSock<T>* pChild	= i->second;
			m_mapChild.erase( i );
			pChild->Close();
			SAFE_DELETE( pChild );
			return TRUE;
		}
		return FALSE;
#else	// __STL_0402
		CClientSock<T>* pChild;
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

template <class T>
void CServerSock<T>::CloseAllChild( void )
	{
		CMclAutoLock	Lock( m_mapChild.m_AddRemoveLock );
#ifdef __STL_0402
		for( CMapChild<T>::iterator i = m_mapChild.begin(); i != m_mapChild.end(); ++i )
		{
			CClientSock<T>* pChild	= i->second;
			TRACE( "~%x, %d\n", pChild, pChild->GetHandle() );
			pChild->Close();
			SAFE_DELETE( pChild );
		}
		m_mapChild.clear();
#else // __STL_0402
		CMyBucket<CClientSock<T>*>* pBucket;
		CClientSock<T>* pChild;
		pBucket		= m_mapChild.GetFirstActive();
		while( pBucket )
		{
			pChild	= pBucket->m_value;
			TRACE( "~%x, %d\n", pChild, pChild->GetHandle() );
			pChild->Close();
			SAFE_DELETE( pChild );
			pBucket	= pBucket->pNext;
		}
		m_mapChild.RemoveAll();
#endif	// __STL_0402
	}

template <class T>
void CServerSock<T>::Send( char* lpData, DWORD dwDataSize, DPID dpidTo )
	{
		if( dpidTo == DPID_ALLPLAYERS )
		{
			CMclAutoLock	Lock( m_mapChild.m_AddRemoveLock );
#ifdef __STL_0402
			for( CMapChild<T>::iterator i = m_mapChild.begin(); i != m_mapChild.end(); ++i )
			{
				CClientSock<T>* pChild	= i->second;
				pChild->Send( lpData, dwDataSize, pChild->GetHandle() );
			}
#else	// __STL_0402
			CClientSock<T>* pChild;
			CMyBucket<CClientSock<T>*>* pBucket	= m_mapChild.GetFirstActive();
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
			CClientSock<T>* pChild	= (CClientSock<T>*)Get( dpidTo );
			if( pChild )
				pChild->Send( lpData, dwDataSize, dpidTo );
#else	// __STL_0402
			CClientSock<T>* pChild;
			if( m_mapChild.Lookup( dpidTo, pChild ) )
				pChild->Send( lpData, dwDataSize, dpidTo );
#endif	// __STL_0402
		}
	}

template <class T>
BOOL CServerSock<T>::CloseConnection( SOCKET hSocket )
	{	return CloseChild( hSocket );	}

template <class T>
BOOL CServerSock<T>::Shutdown( SOCKET hSocket )
	{
		CMclAutoLock	Lock( m_mapChild.m_AddRemoveLock );
#ifdef __STL_0402
		CClientSock<T>* pChild	= (CClientSock<T>*)Get( hSocket );
		if( pChild )
		{
			shutdown( hSocket, SD_BOTH );
			return TRUE;
		}
		return FALSE;
#else	// __STL_0402
		CClientSock<T>* pChild;
		if( m_mapChild.Lookup( hSocket, pChild ) )
		{
			ASSERT( pChild );
			shutdown( hSocket, SD_BOTH );
			return TRUE;
		}
		return( FALSE );
#endif	// __STL_0402
	}

template <class T>
HRESULT CServerSock<T>::GetPeerAddr( DPID dpid, LPVOID lpAddr, LPDWORD lpdwSize )
	{
		ASSERT( lpAddr );

		CClientSock<T>* pSocket;
		SOCKET hSocket	= (SOCKET)dpid;
		SOCKADDR_IN sin;

		ZeroMemory( &sin, sizeof(SOCKADDR_IN) );

#ifdef __STL_0402
		pSocket		= (CClientSock<T>*)Get( hSocket );
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
/*
template <class T>
HRESULT CServerSock<T>::GetPeerAddr( SOCKET hSocket, u_long* paddr )
	{
		SOCKADDR_IN sin;
		ZeroMemory( &sin, sizeof(SOCKADDR_IN) );
		int namelen	= sizeof(sin);
		if( getpeername( hSocket, (SOCKADDR *)&sin, &namelen ) == SOCKET_ERROR )
			return DPERR_GENERIC;
		*paddr	= sin.sin_addr.S_un.S_addr;
		return DP_OK;
	}
*/
#ifdef __DPMNG0412
template <class T>
int CServerSock<T>::AcptThread( void )
{
	DWORD dwCmpKey;
	DWORD dwBytes;
	PSocklist pSocklist;

	while( 1 )
	{
		if( GetQueuedCompletionStatus(
							m_hIocpAcpt,
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

			CClientSock<T>* pChild	= AddChild( pSocklist->sock );
			pChild->SetID( DPID_SERVERPLAYER );
			pChild->SetPeerID( pSocklist->sock );


			if( !CreateIoCompletionPort( (HANDLE) pChild->GetHandle(), GetCompletionPort(), (DWORD)pSocklist->sock, GetIoWorkerCount() ) )
			{
				TRACE( "CreateIoCompletionPort Failure with error =%d\n", WSAGetLastError() );
				CloseChild( pSocklist->sock );
			}
			else
			{
				PostQueuedCompletionStatus( GetCompletionPort(), NEWSOCKETMSG, (DWORD)pSocklist->sock, NULL );
			}

			AcptRequest( pSocklist->iIndex );
		}
	}
}

#else	// __DPMNG0412
template <class T>
int CServerSock<T>::ListenThread( void )
{
	HANDLE handle[3];
	DWORD dw;
	WSANETWORKEVENTS ne;
//	CDoS	dos;
//	u_long	ip;

	handle[0]	= GetCloseHandle();
	handle[1]	= GetListenHandle();
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
					WSAEnumNetworkEvents( GetHandle(), GetListenHandle(), &ne );
					if( ne.lNetworkEvents != FD_ACCEPT )
						break;

					if( ( hSocket	= WSAAccept( GetHandle(), &addr, &nSize, NULL, 0 ) ) == INVALID_SOCKET )
					{
						TRACE( "Accept failed with error %d\n", WSAGetLastError() );
						break;
					}

//					if( DP_OK == GetPeerAddr( hSocket, &ip ) )
//					{
//						if( dos.IsDoS( ip ) )
//						{
//							CLOSE_SOCKET( hSocket );
//							break;
//						}
//					}

					int zero	= 0;
					setsockopt( hSocket, SOL_SOCKET, SO_SNDBUF, (char *)&zero, sizeof(zero) );
					setsockopt( hSocket, SOL_SOCKET, SO_RCVBUF, (char *)&zero, sizeof(zero) );

					CClientSock<T>* pChild	= AddChild( hSocket );
					pChild->SetID( DPID_SERVERPLAYER );
					pChild->SetPeerID( hSocket );

					if( !CreateIoCompletionPort( (HANDLE) pChild->GetHandle(), GetCompletionPort(), (DWORD)hSocket, GetIoWorkerCount() ) )
					{
						TRACE( "CreateIoCompletionPort Failure with error =%d\n", WSAGetLastError() );
						CloseChild( hSocket );
						break;
					}

					PostQueuedCompletionStatus( GetCompletionPort(), NEWSOCKETMSG, (DWORD)hSocket, NULL );
					break;
				}
			case WAIT_TIMEOUT :
				break;
		}
	}
	return( 0 );
}
#endif	// __DPMNG0412

#endif //__SERVERSOCK_H__