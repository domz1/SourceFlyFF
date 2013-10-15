#ifndef __DPSOCK_H__
#define __DPSOCK_H__

#pragma once

#include "serversock.h"
#include "clientsock.h"
#include "serversocke.h"
#include "clientsocke.h"

#define	NEWSOCKETMSG	(DWORD)-3
#define	CLOSEIOWORKERMSG	(DWORD)-2
#define	INVALIDSOCKMSG	(DWORD)-4
#define NEWSOCKETMSG2	(DWORD)-5

class CDPSock
{
private:
	CSock*	m_pSock;
	u_short	m_uPort;
	char	m_lpAddr[32];
	u_long	m_uIoWorker;
	list<HANDLE>	m_listthread;
	CMclCritSec		m_locklistthread;
	HANDLE* m_phCompletionPort;
	WSAEVENT	m_hClose;
	HANDLE	m_hRecv;
	CSock*	m_pSockThreaded;
public:
	CBufferQueue	m_lspRecvBuffer;
	BOOL	m_fServer;
#ifdef __CRC
	DWORD	m_dwReadHeaderSize;
	DWORD	m_dwDataSizeOffset;
#endif	// __CRC
#ifdef __PROTOCOL0910
	HANDLE	m_hProtocolId;
#endif	// __PROTOCOL0910
	long	m_lActiveIoWorker;

private:
//	Operations
	BOOL	CreateIoWorker( u_long uIoWorker );
	BOOL	CloseIoWorker( void );
public:
//	Constructions
	CDPSock();
	virtual	~CDPSock();
//	Operations
	BOOL	Close( void );
	void	CloseConnection( DPID dpid );
	BOOL	Shutdown( DPID dpid );
	HANDLE	GetCompletionPort( int iIoWorker );
#ifdef __INFO_SOCKLIB0516
	DWORD	GetDebugInfo( DPID dpid );
#endif	// __INFO_SOCKLIB0516
	HANDLE	GetCloseHandle( void );
	HANDLE	GetRecvHandle( void );
	CSock*	GetSockThreaded( void );
	int		GetIoWorkerCount( void );
	BOOL	CreateEventWorker( CSock* pSock );
	void	RemoveThread( HANDLE hThread );
	CClientSock*	Get( SOCKET hSocket );
	
	BOOL	InitializeConnection( LPVOID lpConnection, DWORD dwFlags );
#ifdef __CRC
	BOOL	CreateServer( DWORD dwcrc );
	BOOL	JoinToServer( DWORD dwcrc, u_long uWaitingTime = 10000 );
	BOOL	CreateServerE( DWORD dwcrc );
	BOOL	JoinToServerE( DWORD dwcrc, u_long uWaitingTime = 10000 );
#else	// __CRC
	BOOL	CreateServer( BUFFER_TYPE nBufferType );
	BOOL	JoinToServer( BUFFER_TYPE nBufferType );
	BOOL	CreateServerE( BUFFER_TYPE nBufferType );
	BOOL	JoinToServerE( BUFFER_TYPE nBufferType );
#endif	// __CRC
	BOOL	Send( char* lpData, DWORD dwDataSize, DPID dpidTo );

	HRESULT	GetPlayerAddr( DPID dpid, LPVOID lpAddr, LPDWORD lpdwSize );
	DWORD	GetPlayerAddr( DPID dpid );

	HRESULT	GetHostAddr( LPVOID lpAddr, LPDWORD lpdwSize );
	CBuffer*	Receive( );

#ifdef __PROTOCOL0910
	void	SetProtocolId( DWORD dwProtocolId );
#endif	// __PROTOCOL0910

	void	AddCreatePlayerOrGroupMsg( DPID dpid, CBuffer* pBuffer );
	void	AddDestroyPlayerOrGroupMsg( DPID dpid, CBuffer* pBuffer );

	CSock*	GetServerSock( void )		{	return	( m_fServer? m_pSock: NULL );	}
};

inline HANDLE CDPSock::GetCompletionPort( int iIoWorker )
	{	return m_phCompletionPort[iIoWorker];	}
inline HANDLE CDPSock::GetCloseHandle( void )
	{	return m_hClose;	}
inline HANDLE CDPSock::GetRecvHandle( void )
	{	return m_hRecv;	}
inline int	CDPSock::GetIoWorkerCount( void )
	{	return (int)m_uIoWorker;	}
inline CSock* CDPSock::GetSockThreaded( void )
	{	return m_pSockThreaded;	}
inline CClientSock* CDPSock::Get( SOCKET hSocket )
	{	return ( m_pSock? (CClientSock*)m_pSock->Get( hSocket ): NULL );	}

inline HRESULT	CDPSock::GetHostAddr( LPVOID lpAddr, LPDWORD lpdwSize )
	{	return( m_pSock ? m_pSock->GetHostAddr( lpAddr, lpdwSize ):	DPERR_NOSESSIONS );	}
inline HRESULT CDPSock::GetPlayerAddr( DPID dpid, LPVOID lpAddr, LPDWORD lpdwSize )
	{	return m_pSock->GetPeerAddr( dpid, lpAddr, lpdwSize );	}
inline DWORD CDPSock::GetPlayerAddr( DPID dpid )
	{	return m_pSock->GetPeerAddr( dpid );	}

#endif //__DPSOCK_H__