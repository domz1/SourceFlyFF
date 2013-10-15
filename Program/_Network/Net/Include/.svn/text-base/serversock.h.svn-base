#ifndef __SERVERSOCK_H__
#define __SERVERSOCK_H__

#pragma once

#include "clientsock.h"
#include "mymap.h"

#ifdef __NEWDPMNG
typedef struct _socklist : public OVERLAPPED
{
	SOCKET	sock;
	int		iIndex;
}	Socklist, *PSocklist;
#define	MAX_IO_PEND	128		// maximum pending I/0 requests
#define	PADDED_ADDR_SIZE	sizeof(SOCKADDR_IN)+16
#define	MAX_BYTES	2000
#endif	// __NEWDPMNG

#ifdef __STL_0402
class CMapChild : public map<DPID, CClientSock*>
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

class CServerSock : public CSock  
{
public:
#ifndef __NEWDPMNG
	HANDLE	m_hListenThread;
	HANDLE	m_hClose;
	WSAEVENT	m_hListen;
#else	// __NEWDPMNG
	HANDLE	m_hAcptThread;
	HANDLE	m_hIocpAcpt;
	Socklist	m_socklist[MAX_IO_PEND];
	BOOL	AcptRequest( int iIndex );
#endif	// __NEWDPMNG

#ifdef __STL_0402
	CMapChild	m_mapChild;
#else	// __STL_0402
	CMyMap<CClientSock*>	m_mapChild;
#endif	// __STL_0402

	HANDLE* m_phCompletionPort;
	u_long	m_uIoWorker;
#ifdef __CRC
	DWORD	m_dwcrc;
#endif	// __CRC
	BUFFER_TYPE		m_nBufferType;

public:
//	Constructions
#ifdef __CRC
	CServerSock( DWORD dwcrc );
#else	// __CRC
	CServerSock( BUFFER_TYPE nBufferType );
#endif	// __CRC
	virtual	~CServerSock();
//	Operations
	BOOL	Listen( void );

	HANDLE	GetListenHandle( void );
	HANDLE	GetCloseHandle( void );

	HANDLE	GetCompletionPort( int iIoWorker );
	BOOL	StartServer( HANDLE* phCompletionPort, u_long uIoWorker );
	int		GetIoWorkerCount( void );
	CClientSock*	AddChild( SOCKET hSocket );
	BOOL	CloseChild( SOCKET hSocket );
	void	CloseAllChild( void );
#ifdef __DOS1101
	void	ShutdownAddr( DWORD dwAddr );
#endif	// __DOS1101
	HRESULT	GetPeerAddr( SOCKET hSocket, u_long* paddr );
	BOOL	AllocAndAssoc( SOCKET hSocket, DWORD dwAddr );

//	Override
	virtual	void	Close( void );
	virtual	BOOL	CloseConnection( SOCKET hSocket );
	virtual	BOOL	Shutdown( SOCKET hSocket );
#ifdef __INFO_SOCKLIB0516
	virtual	DWORD	GetDebugInfo( SOCKET hSocket );
#endif	// __INFO_SOCKLIB0516
	virtual	void	Send( char* lpData, DWORD dwDataSize, DPID dpidTo );
	virtual	HRESULT	GetPeerAddr( DPID dpid, LPVOID lpAddr, LPDWORD lpdwSize );
	virtual DWORD	GetPeerAddr( DPID dpid );
	virtual	CSock*	Get( SOCKET hSocket );
};

#ifndef __NEWDPMNG
inline HANDLE CServerSock::GetListenHandle( void )	{	return m_hListen; }
inline HANDLE CServerSock::GetCloseHandle( void )	{	return m_hClose;	}
#endif	// __NEWDPMNG

inline HANDLE CServerSock::GetCompletionPort( int iIoWorker )	{	ASSERT( m_phCompletionPort );	return m_phCompletionPort[iIoWorker];	}

inline int CServerSock::GetIoWorkerCount( void )	{	return m_uIoWorker;	}

inline CSock* CServerSock::Get( SOCKET hSocket )
{
#ifdef __STL_0402
	CMapChild::iterator i = m_mapChild.find( hSocket );
	if( i != m_mapChild.end() )
		return i->second;
	return NULL;
#else	// __STL_0402
	CClientSock* pChild;
	if( m_mapChild.Lookup( hSocket, pChild ) )
		return pChild;
	return NULL;
#endif	// __STL_0402
}

#endif //__SERVERSOCK_H__