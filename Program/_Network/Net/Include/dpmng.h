#ifndef __DPMNG_H__
#define __DPMNG_H__

#pragma once

#include "dpsock.h"
#include "ar.h"
#include "mymap.h"

extern void UninitializeNetLib();
extern BOOL InitializeNetLib();
extern	void	TestNetLib( const char* lpAddr, u_short uPort );

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

class CDPMng
{
private:
	CDPSock*	m_pDPSock;
	HANDLE	m_hRecvThread;
	HANDLE	m_hClose;
public:
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

#ifdef __CRC
	BOOL	CreateSession( LPCSTR lpSession, DWORD dwcrc = 0 );
	BOOL	JoinSession( LPCSTR lpSession, DWORD dwcrc = 0, u_long uWaitingTime = 10000 );
	BOOL	StartServer( u_short uPort, BOOL bSingleThreaded = FALSE, DWORD dwcrc = 0 );
	BOOL	ConnectToServer( LPCSTR lpConnection, u_short uPort, BOOL bSingleThreaded = FALSE, DWORD dwcrc = 0, u_long uWaitingTime = 10000 );
#else	// __CRC
	BOOL	CreateSession( LPCSTR lpSession, BUFFER_TYPE nBufferType = BUFFER_TYPE_5BYTE );
	BOOL	JoinSession( LPCSTR lpSession, BUFFER_TYPE nBufferType = BUFFER_TYPE_5BYTE );
	BOOL	StartServer( u_short uPort, BOOL bSingleThreaded = FALSE, BUFFER_TYPE nBufferType = BUFFER_TYPE_5BYTE );
	BOOL	ConnectToServer( LPCSTR lpConnection, u_short uPort, BOOL bSingleThreaded = FALSE, BUFFER_TYPE nBufferType = BUFFER_TYPE_5BYTE );
#endif	// __CRC
	BOOL	Send( LPVOID lpData, DWORD dwDataSize, DPID dpidTo );
	BOOL	DestroyPlayer( DPID dpid );

#ifdef __INFO_SOCKLIB0516
	DWORD	GetDebugInfo( DPID dpid );
#endif	// __INFO_SOCKLIB0516

	void	GetHostAddr( LPSTR lpAddr );
	void	GetPlayerAddr( DPID dpid, LPSTR lpAddr );
	DWORD	GetPlayerAddr( DPID dpid );


//	Override
	virtual	void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )		= 0;
	virtual	void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )	= 0;
};

inline HANDLE CDPMng::GetRecvHandle( void )
{	
	return	m_pDPSock->GetRecvHandle();	
}

inline HANDLE CDPMng::GetCloseHandle( void )
{	
	return m_hClose;	
}

#ifdef __CRC
	inline BOOL CDPMng::StartServer( u_short uPort, BOOL bSingleThreaded, DWORD dwcrc )
	{	
		return( CreateDPObject( bSingleThreaded ) && 
			    InitConnection( (LPVOID)NULL, uPort ) && 
				CreateSession( NULL, dwcrc ) );	
	}
	inline BOOL CDPMng::ConnectToServer( LPCSTR lpConnection, u_short uPort, BOOL bSingleThreaded, DWORD dwcrc, u_long uWaitingTime )
	{	
		return( CreateDPObject( bSingleThreaded ) && 
			    InitConnection( (LPVOID)lpConnection, uPort ) && 
				JoinSession( NULL, dwcrc, uWaitingTime ) );	
	}
#else	
	inline BOOL CDPMng::StartServer( u_short uPort, BOOL bSingleThreaded, BUFFER_TYPE nBufferType )
	{	
		return( CreateDPObject( bSingleThreaded ) && 
			    InitConnection( (LPVOID)NULL, uPort ) && 
				CreateSession( NULL, nBufferType ) );	
	}
	inline BOOL CDPMng::ConnectToServer( LPCSTR lpConnection, u_short uPort, BOOL bSingleThreaded, BUFFER_TYPE nBufferType )
	{	
		return( CreateDPObject( bSingleThreaded ) && 
			    InitConnection( (LPVOID)lpConnection, uPort ) && 
				JoinSession( NULL, nBufferType ) );	
	}
#endif	// __CRC

inline BOOL CDPMng::Send( LPVOID lpData, DWORD dwDataSize, DPID dpidTo )
{	
	if( !m_pDPSock )
		return FALSE;
	return m_pDPSock->Send( (char*)lpData, dwDataSize, dpidTo );
}

#endif //__DPMNG_H__