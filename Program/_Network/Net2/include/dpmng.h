#ifndef __DPMNG_H__
#define __DPMNG_H__

#pragma once

#include "dpsock.h"
#include "ar.h"
#include "mymap.h"

#define LOAD_WS2_32_DLL	\
	WSADATA wsaData;	\
	int err;	\
	err	= WSAStartup( 0x0202, &wsaData );	\
	if( err == SOCKET_ERROR )	\
	{	\
		TRACE( "WSAStartup() failed with error %ld\n", WSAGetLastError() );	\
		return FALSE;	\
	}

#define UNLOAD_WS2_32_DLL	\
	WSACleanup();

#define BEFORESEND( ar, dw )	\
	CAr ar;	\
	int nBufSize;	\
	ar << dw;

#define	BEFORESENDSOLE( ar, dw, dpid_ )	\
	CAr ar;		\
	int nBufSize;	\
	ar << dpid_ << dw;

#define BEFORESENDDUAL( ar, dw, dpid_, dpid__ )	\
	CAr ar;		\
	int nBufSize;	\
	ar << dpid_ << dpid__ << dw;

#define	SEND( ar, pDPMng, idTo ) \
	LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );	\
	(pDPMng)->Send( (LPVOID)lpBuf, nBufSize, idTo );

#define GETTYPE(ar)		\
	DWORD dw;	\
	ar >> dw;

#ifdef __STL_0402
	#define	USES_PFNENTRIES	\
		private:	\
		map<DWORD, void (theClass::*)( theParameters )>	m_pfnEntries;	\
		void ( theClass::*GetHandler( DWORD dwType ) )( theParameters )	\
			{	\
				map<DWORD, void (theClass::*)( theParameters )>::iterator i = m_pfnEntries.find( dwType );	\
				if( i != m_pfnEntries.end() )	\
					return i->second;	\
				return NULL;	\
			}
				
	#define BEGIN_MSG	\
		void ( theClass::*pfn )( theParameters );

	#define	ON_MSG( dwKey, hndlr )	\
		pfn		= hndlr;	\
		m_pfnEntries.insert( map<DWORD, void (theClass::*)( theParameters )>::value_type( dwKey, pfn ) );
#else	// __STL_0402
	#define	USES_PFNENTRIES	\
		private:	\
		CMyMap<void (theClass::*)( theParameters )>	m_pfnEntries;	\
		void ( theClass::*GetHandler( DWORD dwType ) )( theParameters )	\
			{ void ( theClass::*pfn )( theParameters );	return ( m_pfnEntries.Lookup( dwType, pfn ) ? pfn : NULL ); }

	#define BEGIN_MSG	\
		m_pfnEntries.SetSize( 64, 128, 64 );	\
		void ( theClass::*pfn )( theParameters );

	#define	ON_MSG( dwKey, hndlr )	\
		pfn		= hndlr;	\
		m_pfnEntries.SetAt( dwKey, pfn );
#endif	// __STL_0402

template <class T>
class CDPMng
{
private:
	HANDLE	m_hRecvThread;
	HANDLE	m_hClose;
public:
	CDPSock<T>*	m_pDPSock;
//	Constructions
	CDPMng();
	virtual	~CDPMng();

//	Operations
	HANDLE	GetRecvHandle( void );
	HANDLE	GetCloseHandle( void );
	BOOL	CreateDPObject( BOOL bSingleThreaded = FALSE );
	BOOL	DeleteDPObject( void );
	BOOL	InitConnection( LPVOID lpConnection, u_short uPort = 0 );
	BOOL	ReceiveMessage( void );

	BOOL	CreateSession( LPCSTR lpSession, DWORD dwcrc = 0 );
	BOOL	JoinSession( LPCSTR lpSession, DWORD dwcrc = 0, u_long uWaitingTime = 10000 );
	BOOL	StartServer( u_short uPort, BOOL bSingleThreaded = FALSE, DWORD dwcrc = 0 );
	BOOL	ConnectToServer( LPCSTR lpConnection, u_short uPort, BOOL bSingleThreaded = FALSE, DWORD dwcrc = 0, u_long uWaitingTime = 10000 );

	BOOL	Send( LPVOID lpData, DWORD dwDataSize, DPID dpidTo );
	BOOL	DestroyPlayer( DPID dpid );

	void	GetHostAddr( LPSTR lpAddr );
	void	GetPlayerAddr( DPID dpid, LPSTR lpAddr );


	static	UINT	_ReceiveThread( LPVOID pParam )
	{
		CDPMng<T>* pdpMng	= (CDPMng<T>*)pParam;
		return pdpMng->ReceiveThread();
	}
	int		ReceiveThread( void );
	
//	Overrides
	virtual	void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )		= 0;
	virtual	void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )	= 0;
};

/*--------------------------------------------------------------------------------*/
template <class T>
inline HANDLE CDPMng<T>::GetRecvHandle( void )
	{	return	m_pDPSock->GetRecvHandle();	}

template <class T>
inline HANDLE CDPMng<T>::GetCloseHandle( void )
	{	return m_hClose;	}

template <class T>
inline BOOL CDPMng<T>::Send( LPVOID lpData, DWORD dwDataSize, DPID dpidTo )
	{	
		if( !m_pDPSock )
			return FALSE;
		return m_pDPSock->Send( (char*)lpData, dwDataSize, dpidTo );
	}

template <class T>
BOOL CDPMng<T>::StartServer( u_short uPort, BOOL bSingleThreaded, DWORD dwcrc )
	{	return( CreateDPObject( bSingleThreaded ) && InitConnection( (LPVOID)NULL, uPort ) && CreateSession( NULL, dwcrc ) );	}

template <class T>
BOOL CDPMng<T>::ConnectToServer( LPCSTR lpConnection, u_short uPort, BOOL bSingleThreaded, DWORD dwcrc, u_long uWaitingTime )
	{	return( CreateDPObject( bSingleThreaded ) && InitConnection( (LPVOID)lpConnection, uPort ) && JoinSession( NULL, dwcrc, uWaitingTime ) );	}

template <class T>
CDPMng<T>::CDPMng()
	{
		m_pDPSock	= NULL;
		m_hRecvThread	= m_hClose	= (HANDLE)NULL;
	}

template <class T>
CDPMng<T>::~CDPMng()
	{}

template <class T>
BOOL CDPMng<T>::CreateDPObject( BOOL bSingleThreaded )
	{
		DeleteDPObject();
		if( !m_pDPSock )
			m_pDPSock	= new CDPSock<T>;
		if( !bSingleThreaded )
		{
			// Create event used to signal that the receive thread should exit
			if( !m_hClose )
			{
				m_hClose	= CreateEvent( NULL, FALSE, FALSE, NULL );
				ASSERT( m_hClose );			
			}
			// Create thread to receive player messages
			if( !m_hRecvThread )
			{
				DWORD dwThreadId;
				m_hRecvThread	= chBEGINTHREADEX( NULL, 0, _ReceiveThread, this, 0, &dwThreadId );
				ASSERT( m_hRecvThread );
			}
		}
		return TRUE;
	}

template <class T>
BOOL CDPMng<T>::DeleteDPObject( void )
	{
		// Wake up receive thread and wait for it to quit
		CLOSE_THREAD( m_hRecvThread, m_hClose );
		if( m_pDPSock ) {
			m_pDPSock->Close();
			SAFE_DELETE( m_pDPSock );
		}
		return TRUE;
	}

template <class T>
BOOL CDPMng<T>::InitConnection( LPVOID lpConnection, u_short uPort )
	{	return	m_pDPSock->InitializeConnection( lpConnection, (DWORD)uPort );	}

template <class T>
BOOL CDPMng<T>::CreateSession( LPCSTR lpSession, DWORD dwcrc )
	{
		if( ( chWindows9x()? m_pDPSock->CreateServerE( dwcrc ): m_pDPSock->CreateServer( dwcrc ) ) )
		{
			TRACE( "%s server launched.\n", lpSession );
			return TRUE;
		}
		TRACE( "%s server launching failed.\n", lpSession );
		return FALSE;
	}

template <class T>
BOOL CDPMng<T>::JoinSession( LPCSTR lpSession, DWORD dwcrc, u_long uWaitingTime )
	{
		if( ( chWindows9x()? m_pDPSock->JoinToServerE( dwcrc, uWaitingTime ): m_pDPSock->JoinToServer( dwcrc, uWaitingTime ) ) )
		{
			TRACE( "%s server connected.\n", lpSession );
			return TRUE;
		}
		TRACE( "%s server connection failed.\n", lpSession );
		return FALSE;
	}

template <class T>
void CDPMng<T>::GetHostAddr( LPSTR lpAddr )
	{
		if( m_pDPSock )
		{
			u_long uBufSize		= sizeof(lpAddr);
			m_pDPSock->GetHostAddr( (LPVOID)lpAddr, (LPDWORD)&uBufSize );
		}
	}

template <class T>
void CDPMng<T>::GetPlayerAddr( DPID dpid, LPSTR lpAddr )
	{
		if( m_pDPSock )
		{
			u_long uBufSize		= sizeof(lpAddr);
			m_pDPSock->GetPlayerAddr( dpid, (LPVOID)lpAddr, (LPDWORD)&uBufSize );
		}
	}

template <class T>
BOOL CDPMng<T>::ReceiveMessage( void )
	{
		if( !m_pDPSock )
			return FALSE;

		DPID dpidFrom, dpidTo;
		HRESULT hr;
		T* pBuffer;
//		LPBYTE ptr;
//		u_long uDataSize;

		while( 1 )
		{
			dpidFrom	= dpidTo	= 0;
			hr = m_pDPSock->Receive( &dpidFrom, &dpidTo, 0, pBuffer );
			
			if( FAILED( hr ) )
				break;

			pBuffer->MessageHandler( this, dpidFrom );
			/*
			ptr		= pBuffer->m_lpBufStart;
			while( pBuffer->cb > 0 )
			{
				uDataSize	= *(UNALIGNED LPDWORD)( ptr + m_pDPSock->m_dwDataSizeOffset );
				if( dpidFrom == DPID_SYSMSG )
					SysMessageHandler( (LPDPMSG_GENERIC)&ptr[m_pDPSock->m_dwReadHeaderSize], uDataSize, dpidFrom );
				else
					UserMessageHandler( (LPDPMSG_GENERIC)&ptr[m_pDPSock->m_dwReadHeaderSize], uDataSize, dpidFrom );
				ptr	+=	m_pDPSock->m_dwReadHeaderSize + uDataSize;
				pBuffer->cb--;
			}
			*/
			safe_delete( pBuffer );
		}

		return TRUE;
	}

template <class T>
BOOL CDPMng<T>::DestroyPlayer( DPID dpid )
	{
		if( m_pDPSock )
			return m_pDPSock->Shutdown( dpid );
		return FALSE;
	}

template <class T>
int CDPMng<T>::ReceiveThread( void )
	{
		HANDLE lpHandle[2];
		lpHandle[0]		= GetCloseHandle();
		lpHandle[1]		= GetRecvHandle();
		// loop waiting for player events. If the kill event is signaled, the thread will exit
		while( WaitForMultipleObjects( 2, lpHandle, FALSE, INFINITE ) != WAIT_OBJECT_0 )
		{
			// receive any messages in the queue
			ReceiveMessage();
		}
		return 0;
	}

#endif //__DPMNG_H__