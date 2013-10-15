#ifndef __CLIENTSOCKE_H__
#define __CLIENTSOCKE_H__

#pragma once

#include "sock.h"
#include "buffer.h"

#ifdef __CRC
#include "crc.h"
//using	namespace	CryptoPP;
using	namespace	std;
#endif	// __CRC

class CClientSockE : public CSock
{
public:
	CBuffer*	m_pRecvBuffer;
	WSAOVERLAPPED	m_ovRecv;
	WSAOVERLAPPED	m_ovSend;
	WSAEVENT	m_hRecv;
	WSAEVENT	m_hSend;
	HANDLE	m_hWorker;
	volatile LONG	m_l;

#ifdef __PROTOCOL0910
	DWORD	m_dwProtocolId;
	void	SendProtocolId( void );
	virtual	void	SetProtocolId( DWORD dwProtocolId )	{	m_dwProtocolId	= dwProtocolId;	};
	BOOL	IsCrcWrite( void )	{	return (BOOL)m_lspSendBuffer.m_pcrc;	}
#endif	// __PROTOCOL0910

#ifdef __INFO_SOCKLIB0516
	DWORD	m_dwDbgInfo;
#endif	// __INFO_SOCKLIB0516

private:
	CBufferQueue	m_lspSendBuffer;
	WSABUF	m_lpBuffers[MAX_WSABUF];
	BUFFER_TYPE		m_nBufferType;
#ifdef __CRC
	CRC32	m_crcRead;
	CRC32	m_crcWrite;
	DWORD	m_dwReadHeaderSize;
#endif	// __CRC

public:
//	Constructions
#ifdef __CRC
	CClientSockE( DWORD dwcrc );
#else	// __CRC
	CClientSockE( BUFFER_TYPE type );
#endif	// __CRC
	virtual	~CClientSockE();
//	Operations
	CBuffer* CreateBuffer();
	BOOL	Connect( char* lpAddr, u_short uPort );
	int		Recv( void );
	int		SendRemnant( DWORD dwBytesSent );
	CBuffer*	Fetch( DWORD dwBytes );
	WSAEVENT	GetRecvEvent( void );
	WSAEVENT	GetSendEvent( void );
//	Override
	virtual	BOOL	Create( u_short uPort = 0, int type = SOCK_STREAM );
	virtual	void	Attach( SOCKET hSocket );
	virtual	void	Close( void );
	virtual	BOOL	CloseConnection( SOCKET hSocket );
	virtual	BOOL	Shutdown( SOCKET hSocket );
	virtual	void	Send( char* lpData, DWORD dwDataSize, DPID dpidTo );
	virtual	HRESULT		GetPeerAddr( DPID dpid, LPVOID lpAddr, LPDWORD lpdwSize );
#ifdef __INFO_SOCKLIB0516
	virtual	DWORD	GetDebugInfo( SOCKET hSocket );
#endif	// __INFO_SOCKLIB0516

public:
#ifndef __VM_0820
#ifndef __MEM_TRACE
static	\
//CMemPool<CClientSockE>*	m_pPool;
	MemPooler<CClientSockE>*	m_pPool;
public:
	void*	operator new( size_t nSize )	{	return CClientSockE::m_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CClientSockE::m_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CClientSockE::m_pPool->Free( (CClientSockE*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CClientSockE::m_pPool->Free( (CClientSockE*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820
};

inline WSAEVENT CClientSockE::GetRecvEvent( void )
	{	return m_hRecv;	}
inline WSAEVENT CClientSockE::GetSendEvent( void )
	{	return m_hSend;	}

#endif //__CLIENTSOCKE_H__