#ifndef __SERVERSOCKE_H__
#define __SERVERSOCKE_H__

#pragma once

#include "clientsocke.h"
#include "mymap.h"

#ifdef __STL_0402
class CMapChildE : public map<DPID, CClientSockE*>
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

class CDPSock;
class CServerSockE : public CSock  
{
public:
	HANDLE	m_hListenThread;
	HANDLE	m_hClose;
	WSAEVENT	m_hListen;
#ifdef __STL_0402
	CMapChildE	m_mapChild;
#else	// __STL_0402
	CMyMap<CClientSockE*>	m_mapChild;
#endif	// __STL_0402
	CDPSock*	m_pDPSock;
#ifdef __CRC
	DWORD	m_dwcrc;
#endif	// __CRC
	BUFFER_TYPE		m_nBufferType;

public:
//	Constructions
#ifdef __CRC
	CServerSockE( DWORD dwcrc );
#else	// __CRC
	CServerSockE( BUFFER_TYPE nBufferType );
#endif	// __CRC
	virtual	~CServerSockE();
//	Operations
	BOOL	Listen( void );
	HANDLE	GetListenHandle( void );
	HANDLE	GetCloseHandle( void );
	CClientSockE*	AddChild( SOCKET hSocket );
	BOOL	CloseChild( SOCKET hSocket );
	void	CloseAllChild( void );
	BOOL	StartServer( CDPSock* pDPSock );
//	Override
	virtual	void	Close( void );
	virtual	BOOL	CloseConnection( SOCKET hSocket );
	virtual	BOOL	Shutdown( SOCKET hSocket );
	virtual	void	Send( char* lpData, DWORD dwDataSize, DPID dpidTo );
	virtual	HRESULT		GetPeerAddr( DPID dpid, LPVOID lpAddr, LPDWORD lpdwSize );
	virtual	CSock*	Get( SOCKET hSocket );
};

inline HANDLE CServerSockE::GetListenHandle( void )	{	return m_hListen; }
inline HANDLE CServerSockE::GetCloseHandle( void )	{	return m_hClose;	}

inline CSock* CServerSockE::Get( SOCKET hSocket )
{
#ifdef __STL_0402
	CMapChildE::iterator i	= m_mapChild.find( hSocket );
	if( i != m_mapChild.end() )
		return i->second;
	return NULL;
#else	// __STL_0402
	CClientSockE* pChild;
	if( m_mapChild.Lookup( hSocket, pChild ) )
		return pChild;
	return NULL;
#endif	// __STL_0402
}

#endif //__SERVERSOCKE_H__