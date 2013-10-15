#ifndef __SERVERSOCKE_H__
#define __SERVERSOCKE_H__

#pragma once

#include "clientsocke.h"
#include "mymap.h"
#include "dpsock.h"

template <class T>
class CDPSock;

#ifdef __STL_0402
template <class T>
class CMapChildE : public map<DPID,CClientSockE<T>*>
{
public:
	CMapChildE()	{}
	virtual	~CMapChildE()	{}
	void	Lock( void )	{	m_AddRemoveLock.Enter();	}
	void	Unlock( void )	{	m_AddRemoveLock.Leave();	}
public:
	CMclCritSec		m_AddRemoveLock;
};
#endif	// __STL_0402


template <class T>
class CServerSockE : public CSock  
{
public:
	HANDLE	m_hListenThread;
	HANDLE	m_hClose;
	WSAEVENT	m_hListen;
#ifdef __STL_0402
	CMapChildE<T>	m_mapChild;
#else	// __STL_0402
	CMyMap<CClientSockE<T>*>	m_mapChild;
#endif	// __STL_0402
	CDPSock<T>*		m_pDPSock;

public:
//	Constructions
	CServerSockE( DWORD dwcrc );
	virtual	~CServerSockE();
//	Operations
	BOOL	Listen( void );
	HANDLE	GetListenHandle( void );
	HANDLE	GetCloseHandle( void );
	CClientSockE<T>*	AddChild( SOCKET hSocket );
	BOOL	CloseChild( SOCKET hSocket );
	void	CloseAllChild( void );
	BOOL	StartServer( CDPSock<T>* pDPSock );

	static	UINT	_ListenThreadE( LPVOID pParam )
	{
		CServerSockE<T>* pSock	= (CServerSockE<T>*)pParam;
		return pSock->ListenThreadE();
	}
	int		ListenThreadE( void );
//	Override
	virtual	void	Close( void );
	virtual	BOOL	CloseConnection( SOCKET hSocket );
	virtual	BOOL	Shutdown( SOCKET hSocket );
	virtual	void	Send( char* lpData, DWORD dwDataSize, DPID dpidTo );
	virtual	HRESULT		GetPeerAddr( DPID dpid, LPVOID lpAddr, LPDWORD lpdwSize );
	virtual	CSock*	Get( SOCKET hSocket );
};

template <class T>
inline HANDLE CServerSockE<T>::GetListenHandle( void )
	{	return m_hListen; }

template <class T>
inline HANDLE CServerSockE<T>::GetCloseHandle( void )
	{	return m_hClose;	}

template <class T>
inline CSock* CServerSockE<T>::Get( SOCKET hSocket )
	{
#ifdef __STL_0402
		CMapChildE<T>::iterator i	= m_mapChild.find( hSocket );
		if( i != m_mapChild.end() )
			return i->second;
		return NULL;
#else	// __STL_0402
		CClientSockE<T>* pChild;
		if( m_mapChild.Lookup( hSocket, pChild ) )
			return pChild;
		return NULL;
#endif	// __STL_0402
	}


template <class T>
CServerSockE<T>::CServerSockE( DWORD dwcrc )
	{
		m_dwcrc		= dwcrc;
		m_hListenThread		= (HANDLE)NULL;
		m_hClose	= (HANDLE)NULL;
		m_hListen	= WSA_INVALID_EVENT;
		m_pDPSock	= NULL;
#ifndef __STL_0402
		m_mapChild.SetSize( 512, 4096, 128 );
#endif	// __STL_0402
	}

template <class T>
CServerSockE<T>::~CServerSockE()
	{}

template <class T>
BOOL CServerSockE<T>::Listen( void )
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

template <class T>
BOOL CServerSockE<T>::StartServer( CDPSock<T>* pDPSock )
	{
		Close();
		m_pDPSock	= pDPSock;
		m_hListen	= WSACreateEvent();
		ASSERT( m_hListen != WSA_INVALID_EVENT );
		m_hClose	= CreateEvent( NULL, FALSE, FALSE, NULL );
		ASSERT( m_hClose );
		DWORD dwThreadId;
		m_hListenThread		= chBEGINTHREADEX( NULL, 0, _ListenThreadE, this, NULL, &dwThreadId );
		ASSERT( m_hListenThread );
		SetThreadPriority( m_hListenThread, THREAD_PRIORITY_BELOW_NORMAL );
		return TRUE;
	}

template <class T>
void CServerSockE<T>::Close( void )
	{
		CLOSE_WSAEVENT( m_hListen );
		CLOSE_THREAD( m_hListenThread, m_hClose );
		CloseAllChild();
	}

template <class T>
CClientSockE<T>* CServerSockE<T>::AddChild( SOCKET hSocket )
	{
		CClientSockE<T>* pChild		= new CClientSockE<T>( m_dwcrc );
		pChild->Attach( hSocket );
		m_mapChild.Lock();
#ifdef __STL_0402
		bool bResult	= m_mapChild.insert( CMapChildE<T>::value_type( hSocket, pChild ) ).second;
		ASSERT( bResult );
#else	// __STL_0402
		m_mapChild.SetAt( hSocket, pChild );
#endif	// __STL_0402
		m_mapChild.Unlock();
		return pChild;
	}

template <class T>
BOOL CServerSockE<T>::CloseChild( SOCKET hSocket )
	{
		CMclAutoLock	Lock( m_mapChild.m_AddRemoveLock );
#ifdef __STL_0402
		CMapChildE<T>::iterator i	= m_mapChild.find( hSocket );
		if( i != m_mapChild.end() )
		{
			CClientSockE<T>* pChild	= i->second;
			m_mapChild.erase( i );
			pChild->Close();
			SAFE_DELETE( pChild );
			return TRUE;
		}
		return FALSE;
#else	// __STL_0402
		CClientSockE<T>* pChild;
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
void CServerSockE<T>::CloseAllChild( void )
	{
		CMclAutoLock	Lock( m_mapChild.m_AddRemoveLock );
#ifdef __STL_0402
		for( CMapChildE<T>::iterator i = m_mapChild.begin(); i != m_mapChild.end(); ++i )
		{
			CClientSockE<T>* pChild	= i->second;
			TRACE( "~%x, %d\n", pChild, pChild->GetHandle() );
			SAFE_DELETE( pChild );
		}
		m_mapChild.clear();
#else	// __STL_0402
		CClientSockE<T>* pChild;
		CMyBucket<CClientSockE<T>*>* pBucket	= m_mapChild.GetFirstActive();
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

template <class T>
void CServerSockE<T>::Send( char* lpData, DWORD dwDataSize, DPID dpidTo )
	{
		

		if( dpidTo == DPID_ALLPLAYERS )
		{
			CMclAutoLock	Lock( m_mapChild.m_AddRemoveLock );
#ifdef __STL_0402
			for( CMapChildE<T>::iterator i = m_mapChild.begin(); i != m_mapChild.end(); ++i )
			{
				CClientSockE<T>* pChild	= i->second;
				pChild->Send( lpData, dwDataSize, pChild->GetHandle() );
			}
#else	// __STL_0402
			CClientSockE<T>* pChild;
			CMyBucket<CClientSockE<T>*>* pBucket	= m_mapChild.GetFirstActive();
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
			CClientSockE<T>* pChild		= (CClientSockE<T>*)Get( dpidTo );
			if( pChild )
				pChild->Send( lpData, dwDataSize, dpidTo );
#else	// __STL_0402
			CClientSockE<T>* pChild;
			if( m_mapChild.Lookup( dpidTo, pChild ) )
				pChild->Send( lpData, dwDataSize, dpidTo );
#endif	// __STL_0402
		}
	}

template <class T>
BOOL CServerSockE<T>::CloseConnection( SOCKET hSocket )
	{	return CloseChild( hSocket );	}

template <class T>
BOOL CServerSockE<T>::Shutdown( SOCKET hSocket )
	{	return FALSE;	}

template <class T>
HRESULT CServerSockE<T>::GetPeerAddr( DPID dpid, LPVOID lpAddr, LPDWORD lpdwSize )
	{
		ASSERT( lpAddr );

		CClientSockE<T>* pSock;
		SOCKET hSocket	= (SOCKET)dpid;
		SOCKADDR_IN sin;

		ZeroMemory( &sin, sizeof(SOCKADDR_IN) );

#ifdef __STL_0402
		pSock	= (CClientSockE<T>*)Get( hSocket );
#else	//	__STL_0402
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

template <class T>
CServerSockE<T>::ListenThreadE( void )
	{
		HANDLE handle[3];
		DWORD dw;
		WSANETWORKEVENTS ne;
		
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
						CClientSockE<T>* pChild	= AddChild( hSocket );
						pChild->SetID( DPID_SERVERPLAYER );
						pChild->SetPeerID( hSocket );

						ASSERT( m_pDPSock );
						m_pDPSock->CreateEventWorker( pChild );

						int zero	= 0;
						setsockopt( hSocket, SOL_SOCKET, SO_SNDBUF, (char*)&zero, sizeof(zero) );
						setsockopt( hSocket, SOL_SOCKET, SO_RCVBUF, (char*)&zero, sizeof(zero) );
						InterlockedIncrement( &pChild->m_l );
						if( pChild->Recv() != 0 )
						{
							TRACE( "I/0 error %d\n", WSAGetLastError() );
							CloseChild( hSocket );
						}
						break;
					}
				case WAIT_TIMEOUT :
					break;
			}
		}
		return( 0 );
	}

#endif //__SERVERSOCKE_H__