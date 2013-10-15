#include "stdafx.h"
#include "clientsocke.h"

#ifndef __VM_0820
#ifndef __MEM_TRACE
#ifdef MASSIVE
	#ifdef __VM_0819
	MemPooler<CClientSockE>*	CClientSockE::m_pPool	= new MemPooler<CClientSockE>( 1024, "CClientSockE" );
	#else	// __VM_0819
	MemPooler<CClientSockE>*	CClientSockE::m_pPool	= new MemPooler<CClientSockE>( 1024 );
	#endif	// __VM_0819
#else	// MASSIVE
	#ifdef __VM_0819
	MemPooler<CClientSockE>*	CClientSockE::m_pPool	= new MemPooler<CClientSockE>( 4, "CClientSockE" );
	#else	// __VM_0819
	MemPooler<CClientSockE>*	CClientSockE::m_pPool	= new MemPooler<CClientSockE>( 4 );
	#endif	// __VM_0819
#endif	// MASSIVE
#endif	// __MEM_TRACE
#endif	// __VM_0820

#ifdef __CRC
CClientSockE::CClientSockE( DWORD dwcrc )
#else	// __CRC
CClientSockE::CClientSockE( BUFFER_TYPE type )
#endif	// __CRC
{
#ifdef __CRC
	m_hRecv		= m_hSend	= WSA_INVALID_EVENT;
	m_hWorker	= (HANDLE)NULL;
	m_l	= 0;
	m_pRecvBuffer	= CBufferFactory::GetInstance().CreateBuffer( BUFFER_TYPE_5BYTE );

	if( CSock::crcRead & dwcrc )
		m_dwReadHeaderSize	= HEADERSIZE13;
	else
		m_dwReadHeaderSize	= HEADERSIZE5;

#ifdef __PROTOCOL0910
	m_dwProtocolId	= 0;
#endif	// __PROTOCOL0910

	if( CSock::crcWrite & dwcrc )
#ifdef __PROTOCOL0910
		m_lspSendBuffer.crc( &m_crcWrite, &m_dwProtocolId );
#else	// __PROTOCOL0910
		m_lspSendBuffer.crc( &m_crcWrite );
#endif	// __PROTOCOL0910

#else	// __CRC
	m_hRecv		= m_hSend	= WSA_INVALID_EVENT;
	m_hWorker	= (HANDLE)NULL;
	m_l	= 0;
	m_pRecvBuffer	= CBufferFactory::GetInstance().CreateBuffer( type );

	m_nBufferType = type;

#endif	// __CRC

#ifdef __INFO_SOCKLIB0516
	m_dwDbgInfo		= 0;
#endif	// __INFO_SOCKLIB0516
}

CClientSockE::~CClientSockE()
{
	Close();
}

void CClientSockE::Close( void )
{
	CLOSE_SOCKET( m_hSocket );
	CLOSE_WSAEVENT( m_hRecv );
	CLOSE_WSAEVENT( m_hSend );
}

BOOL CClientSockE::CloseConnection( SOCKET hSocket )
{
	ASSERT( hSocket == m_hSocket );
	Close();
	return( TRUE );
}

BOOL CClientSockE::Shutdown( SOCKET hSocket )
{
	return FALSE;
}

#ifdef __INFO_SOCKLIB0516
DWORD CClientSockE::GetDebugInfo( SOCKET hSocket )
{
	return m_dwDbgInfo; // + m_nReference;
}
#endif	// __INFO_SOCKLIB0516

BOOL CClientSockE::Create( u_short uPort, int type )
{
	if( m_hRecv == WSA_INVALID_EVENT )
		m_hRecv		= WSACreateEvent();
	if( m_hSend == WSA_INVALID_EVENT )
		m_hSend		= WSACreateEvent();
	
	return CSock::Create( uPort, type );
}

void CClientSockE::Attach( SOCKET hSocket )
{
	Clear();

	if( m_hRecv == WSA_INVALID_EVENT )
		m_hRecv		= WSACreateEvent();
	if( m_hSend == WSA_INVALID_EVENT )
		m_hSend		= WSACreateEvent();

	CSock::Attach( hSocket );
}

BOOL CClientSockE::Connect( char* lpAddr, u_short uPort )
{
	if( m_hSocket == INVALID_SOCKET )
	{
		TRACE( "Creation needed\n" );
		return FALSE;
	}

	SOCKADDR_IN sin;
	memset( &sin, 0, sizeof(sin) );
	sin.sin_family	= AF_INET;
	sin.sin_addr.s_addr		= inet_addr( lpAddr );

	if( sin.sin_addr.s_addr == INADDR_NONE )
	{
		LPHOSTENT lphost;
		lphost	= gethostbyname( lpAddr );
		if( lphost )
			sin.sin_addr.s_addr		= ((LPIN_ADDR)lphost->h_addr)->s_addr;
		else
		{
			#ifdef __INFO_SOCKLIB0516
				m_dwDbgInfo = WSAEINVAL;
			#endif

			WSASetLastError( WSAEINVAL );
			return FALSE;
		}
	}

	sin.sin_port = htons( uPort );

	if( connect( m_hSocket, (LPSOCKADDR) &sin, sizeof(sin) ) == SOCKET_ERROR )
	{
		int	err;
		if( ( err = WSAGetLastError() ) == WSAEWOULDBLOCK )
			return TRUE;
		#ifdef __INFO_SOCKLIB0516
			m_dwDbgInfo = err;
		#endif
		TRACE( "Can't connect with error %d\n", err );
		return FALSE;
	}
	return TRUE;
}

void CClientSockE::Send( char* lpData, DWORD dwDataSize, DPID dpidTo )
{
	ASSERT( m_hSocket != INVALID_SOCKET );
	DWORD dwBufferCount, dwBytes;

	CMclAutoLock	Lock( m_lspSendBuffer.m_cs );

	if( m_lspSendBuffer.IsEmpty() )
	{
		m_lspSendBuffer.AddData( (LPBYTE)lpData, dwDataSize, m_nBufferType );
		m_lspSendBuffer.GetData( m_lpBuffers, &dwBufferCount );

		int err;
		memset( (void*)&m_ovSend, 0, sizeof(WSAOVERLAPPED) );
		m_ovSend.hEvent		= m_hSend;
		InterlockedIncrement( &m_l );
		if( WSASend( GetHandle(), m_lpBuffers, dwBufferCount, &dwBytes, 0, &m_ovSend, NULL ) != 0 && ( err = WSAGetLastError() ) != WSA_IO_PENDING )
		{
			InterlockedDecrement( &m_l );
//			ASSERT( 0 );
		}
	}
	else
	{
		m_lspSendBuffer.AddData( (LPBYTE)lpData, dwDataSize, m_nBufferType );
	}
}

int CClientSockE::Recv( void )
{
	ASSERT( m_hSocket != INVALID_SOCKET );
	DWORD dwBytesRecvd, dwFlags	= 0;

	WSABUF buffer;
	buffer.buf	= (char*)m_pRecvBuffer->GetWritableBuffer( (int*)&buffer.len );

	memset( (void*)&m_ovRecv, 0, sizeof(WSAOVERLAPPED) );
	m_ovRecv.hEvent		= m_hRecv;

	int err;
	if( WSARecv( GetHandle(), &buffer, (DWORD)1, &dwBytesRecvd, &dwFlags, &m_ovRecv, NULL ) != 0 && ( err = WSAGetLastError() ) != WSA_IO_PENDING )
	{
		TRACE( "I/o error, close socket %d, %x //REF:%d\n", GetHandle(), this, m_l-1 );
		InterlockedDecrement( &m_l );
		return err;
	}
	return 0;
}

int CClientSockE::SendRemnant( DWORD dwBytes )
{
	ASSERT( m_hSocket != INVALID_SOCKET );
	DWORD dwBufferCount;
	
	CMclAutoLock	Lock( m_lspSendBuffer.m_cs );

	m_lspSendBuffer.RemoveData( dwBytes );
	if( m_lspSendBuffer.IsEmpty() )
	{
		InterlockedDecrement( &m_l );
		return 0;
	}
	m_lspSendBuffer.GetData( m_lpBuffers, &dwBufferCount );

	int err;
	memset( (void*)&m_ovSend, 0, sizeof(WSAOVERLAPPED) );
	m_ovSend.hEvent		= m_hSend;
	if( WSASend( GetHandle(), m_lpBuffers, dwBufferCount, &dwBytes, 0, &m_ovSend, NULL ) != 0 && ( err = WSAGetLastError() ) != WSA_IO_PENDING )
	{
		TRACE( "i/O error, close socket %d, %x //REF:%d\n", GetHandle(), this, m_l-1 );
		InterlockedDecrement( &m_l );
		return err;
	}
	return 0;
}

CBuffer* CClientSockE::Fetch( DWORD dwBytes )
{
	m_pRecvBuffer->m_pTail	+=	dwBytes;
	ASSERT( m_pRecvBuffer->m_pTail <= m_pRecvBuffer->m_lpBufMax );
	
	int nRemnant;
	LPBYTE ptr	= m_pRecvBuffer->GetReadableBuffer( &nRemnant );

	CBuffer* pOld	= NULL;
	u_long uPacketSize;

#ifdef __CRC
	DWORD dwCrc, dwDataSize;
	byte digest[4];
#endif	// __CRC

	while( 1 )
	{
#ifdef __CRC
		if( nRemnant < (int)( m_dwReadHeaderSize ) )
#else	// __CRC
		if( nRemnant < (int)( m_pRecvBuffer->GetHeaderLength() ) )
#endif	// __CRC
		{
			if( m_pRecvBuffer->cb > 0 ) {
				pOld	= m_pRecvBuffer;
				pOld->m_pTail	-= nRemnant;	// remove remnant from old buffer
				m_pRecvBuffer	= CBufferFactory::GetInstance().CreateBuffer( m_nBufferType );
				ASSERT( m_pRecvBuffer->m_pTail + nRemnant <= m_pRecvBuffer->m_lpBufMax ); 
				memcpy( m_pRecvBuffer->m_pTail, ptr, nRemnant );

				m_pRecvBuffer->m_pTail	+=	nRemnant;
			}
			return pOld;
		}
		else
		{
#ifdef __CRC
			if( m_dwReadHeaderSize == HEADERSIZE13 )
			{
				dwCrc	= *(UNALIGNED LPDWORD)( ptr + sizeof(char) );
				dwDataSize	= *(UNALIGNED LPDWORD)( ptr + sizeof(char) + sizeof(DWORD) );
				m_crcRead.Restart();
				m_crcRead.Update( (const byte*)( &dwDataSize ), sizeof(DWORD) );
				m_crcRead.Final( digest );
#ifdef __PROTOCOL0910
#ifdef __PROTOCOL1021
				if( ~( *(UNALIGNED LPDWORD)digest ^ m_dwProtocolId ) != dwCrc )
#else	// __PROTOCOL1021
				if( ( *(UNALIGNED LPDWORD)digest ^ m_dwProtocolId ) != dwCrc )
#endif	// __PROTOCOL1021
#else	// __PROTOCOL0910
				if( *(UNALIGNED LPDWORD)digest != dwCrc )
#endif	// __PROTOCOL0910
				{
					WSASetLastError( ERROR_BAD_NET_NAME );
					return NULL;
				}
				dwCrc	= *(UNALIGNED LPDWORD)( ptr + sizeof(char) + sizeof(DWORD) + sizeof(DWORD) );
				uPacketSize		= m_dwReadHeaderSize + dwDataSize;
			}
			else
			{
				uPacketSize	= m_dwReadHeaderSize + *(UNALIGNED LPDWORD)&ptr[1];
			}
#else	// __CRC
			uPacketSize	= m_pRecvBuffer->GetHeaderLength() + m_pRecvBuffer->GetPacketSize( ptr );
#endif	// __CRC
			if( nRemnant < (int)( uPacketSize ) )
			{
				if( (int)( uPacketSize ) > m_pRecvBuffer->GetSize() )
				{
					pOld	= m_pRecvBuffer;
					m_pRecvBuffer	= CBufferFactory::GetInstance().CreateBuffer( m_nBufferType, uPacketSize );
				}
				else
				{
					if( m_pRecvBuffer->cb > 0 )
					{
						pOld	= m_pRecvBuffer;
						m_pRecvBuffer	= CBufferFactory::GetInstance().CreateBuffer( m_nBufferType );
					}
				}

				if( pOld )
				{
					ASSERT( m_pRecvBuffer->m_pTail + nRemnant <= m_pRecvBuffer->m_lpBufMax );
					memcpy( m_pRecvBuffer->m_pTail, ptr, nRemnant );
					m_pRecvBuffer->m_pTail	+=	nRemnant;
				}

				return pOld;
			}
			else	// completion
			{
#ifdef __CRC
				if( m_dwReadHeaderSize == HEADERSIZE13 )
				{
					m_crcRead.Restart();
					m_crcRead.Update( (const byte*)( ptr + m_dwReadHeaderSize ), dwDataSize );
					m_crcRead.Final( digest );
#ifdef __PROTOCOL0910
#ifdef __PROTOCOL1021
					if( ~( *(UNALIGNED LPDWORD)digest ^ m_dwProtocolId ) != dwCrc )
#else	// __PROTOCOL1021
					if( ( *(UNALIGNED LPDWORD)digest ^ m_dwProtocolId ) != dwCrc )
#endif	// __PROTOCOL1021
#else	// __PROTOCOL0910
					if( *(UNALIGNED LPDWORD)digest != dwCrc )
#endif	// __PROTOCOL0910
					{
						WSASetLastError( ERROR_BAD_NET_NAME );
						return NULL;
					}
				}
#endif	// __CRC
				m_pRecvBuffer->cb++;
				nRemnant	-= ( uPacketSize );
				ptr		+= ( uPacketSize );
			}
		}
	}

	return NULL;
}

HRESULT CClientSockE::GetPeerAddr( DPID dpidPlayer, LPVOID lpAddr, LPDWORD lpdwSize )
{
	ASSERT( lpAddr );

	SOCKADDR_IN sin;
	ZeroMemory( &sin, sizeof( SOCKADDR_IN) );

	int namelen	= sizeof(sin);
	if( getpeername( m_hSocket, (SOCKADDR *)&sin, &namelen ) == SOCKET_ERROR )
		return DPERR_GENERIC;

	sprintf( (char *)lpAddr, "%d.%d.%d.%d", sin.sin_addr.s_net
																	, sin.sin_addr.s_host
																	, sin.sin_addr.s_lh
																	, sin.sin_addr.s_impno );
	return DP_OK;
}

CBuffer* CClientSockE::CreateBuffer()
{
	return CBufferFactory::GetInstance().CreateBuffer( m_nBufferType );
}

#ifdef __PROTOCOL0910
void CClientSockE::SendProtocolId( void )
{
	m_dwProtocolId	= GetTickCount();
	char lpData[8];
	*( (DWORD*)lpData )	= 0x00000000;
	*( (DWORD*)&lpData[4] )	= m_dwProtocolId;
	Send( lpData, 8, m_hSocket );
}
#endif	// __PROTOCOL0910