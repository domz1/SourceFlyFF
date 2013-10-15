#include "stdafx.h"
#include "clientsock.h"
#include "xutil.h"

#ifdef	__VERIFY_LOOP041010
	#undef INIT_LOOP
	#undef VERIFY_LOOP

	#define	INIT_LOOP					int __loop = 0;
	#define VERIFY_LOOP(file, line)		if( ++__loop == 1000 ) {	Error( "%s %d", file, line ); }
#else
	#undef INIT_LOOP
	#undef VERIFY_LOOP

	#define	INIT_LOOP					((void)0);
	#define VERIFY_LOOP(file, line)		((void)0);
#endif	


#ifndef __VM_0820
#ifndef __MEM_TRACE
#ifdef MASSIVE
	#ifdef __VM_0819
	MemPooler<CClientSock>*	CClientSock::m_pPool	= new MemPooler<CClientSock>( 1024, "CClientSock" );
	#else	// __VM_0819
	MemPooler<CClientSock>*	CClientSock::m_pPool	= new MemPooler<CClientSock>( 1024 );
	#endif	// __VM_0819
#else	// MASSIVE
	#ifdef __VM_0819
	MemPooler<CClientSock>*	CClientSock::m_pPool	= new MemPooler<CClientSock>( 4, "CClientSock" );
	#else	// __VM_0819
	MemPooler<CClientSock>*	CClientSock::m_pPool	= new MemPooler<CClientSock>( 4 );
	#endif	// __VM_0819
#endif	// MASSIVE
#endif	// __MEM_TRACE
#endif	// __VM_0820

#ifdef __TRAFIC_1218
//DWORD	CClientSock::sm_adwBytes[2]	= { 0, 0	};
#endif	// __TRAFIC_1218

#define	MAX_READ_BYTES_A_SEC	0XFFFFFFFF
#define	MAX_READ_PACKS_A_SEC	0XFFFFFFFF

#ifdef __CRC
CClientSock::CClientSock( DWORD dwcrc )
#else	// __CRC
CClientSock::CClientSock( BUFFER_TYPE type )
#endif	// __CRC
{
#ifdef __CRC
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
	m_nBufferType = BUFFER_TYPE_5BYTE;
#else	// __CRC
//	m_l	= 0;
	m_pRecvBuffer	= CBufferFactory::GetInstance().CreateBuffer( type );
	m_nBufferType = type;
#endif	// __CRC

#ifdef __PACKET_REPLAY_R
	m_nPacket	= 0;
#endif	// __PACKET_REPLAY_R

	m_dwAddr	= 0;

	m_bDoNotWrite	= FALSE;
	m_nReference	= 0;
#ifdef __INFO_SOCKLIB0516
	m_dwDbgInfo		= 0;
#endif	// __INFO_SOCKLIB0516

#ifdef __INFO_SOCKLIB0102
	m_dwReadBytes	= 0;
	m_dwReadPacks	= 0;
	m_tRead	= time( NULL );
#endif	// __INFO_SOCKLIB0102
}

CClientSock::~CClientSock()
{
	Close();
}

void CClientSock::Close( void )
{
	CLOSE_SOCKET( m_hSocket );
	SAFE_DELETE( m_pRecvBuffer );
}

BOOL CClientSock::CloseConnection( SOCKET hSocket )
{
	ASSERT( hSocket == m_hSocket );
	Close();
	return( TRUE );
}

BOOL CClientSock::Shutdown( SOCKET hSocket )
{
	return FALSE;
}

#ifdef __INFO_SOCKLIB0516
DWORD CClientSock::GetDebugInfo( SOCKET hSocket )
{
	return m_dwDbgInfo; // + m_nReference;
}
#endif	// __INFO_SOCKLIB0516

BOOL CClientSock::Connect( char* lpAddr, u_short uPort )
{
	if( m_hSocket == INVALID_SOCKET )
	{
		TRACE( "Creation needed\n" );
		return FALSE;
	}

	SOCKADDR_IN sin;
	memset( &sin, 0, sizeof(sin) );
	sin.sin_family		= AF_INET;
	sin.sin_addr.s_addr	= inet_addr( lpAddr );
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
		if( ( err = WSAGetLastError() ) == WSAEWOULDBLOCK )		// ??
			return TRUE;
		#ifdef __INFO_SOCKLIB0516
			m_dwDbgInfo = err;
		#endif

		TRACE( "Can't connect with error %d: ip(%s)n", err, lpAddr );
		return FALSE;
	}
	return TRUE;
}

void CClientSock::Send( char* lpData, DWORD dwDataSize, DPID dpidTo )
{
//	ASSERT( m_hSocket != INVALID_SOCKET );
	if( m_hSocket == INVALID_SOCKET )
		return;

	DWORD dwBufferCount, dwBytes;
	CMclAutoLock	Lock1( m_AccessLock );
	if( m_bDoNotWrite )
		return;
	CMclAutoLock	Lock( m_lspSendBuffer.m_cs );

	if( m_lspSendBuffer.IsEmpty() )
	{
		m_lspSendBuffer.AddData( (LPBYTE)lpData, dwDataSize, m_nBufferType );
		m_lspSendBuffer.GetData( m_lpBuffers, &dwBufferCount );

		memset( (void*)&m_ovSend, 0, sizeof(WSAOVERLAPPED) );
		int err;
		m_nReference++;
		if( WSASend( GetHandle(), m_lpBuffers, dwBufferCount, &dwBytes, 0, &m_ovSend, NULL ) != 0 && ( err = WSAGetLastError() ) != WSA_IO_PENDING )
		{
			int nReference	= --m_nReference;
#ifdef __INFO_SOCKLIB0516
			m_dwDbgInfo	= err; // * 1000 + 6 * 100 + nReference * 10;
#endif	// __INFO_SOCKLIB0516
		}
		
	}
	else
	{
		m_lspSendBuffer.AddData( (LPBYTE)lpData, dwDataSize, m_nBufferType );
	}
}

int CClientSock::Recv( void )
{
	ASSERT( m_hSocket != INVALID_SOCKET );
	DWORD dwBytesRecvd	= 0, dwFlags	= 0;

	WSABUF buffer;
	buffer.buf	= (char*)m_pRecvBuffer->GetWritableBuffer( (int*)&buffer.len );

	memset( &m_ovRecv, 0, sizeof(WSAOVERLAPPED) );

	int err;
	if( WSARecv( GetHandle(), &buffer, (DWORD)1, &dwBytesRecvd, &dwFlags, &m_ovRecv, NULL ) != 0 && ( err = WSAGetLastError() ) != WSA_IO_PENDING )
	{
//		TRACE( "I/o error, close socket %d, %x //REF:%d\n", GetHandle(), this, m_l-1 );
		return err;
	}

	return 0;
}

int CClientSock::SendRemnant( DWORD dwBytes )
{
	ASSERT( m_hSocket != INVALID_SOCKET );
	DWORD dwBufferCount;
	
	CMclAutoLock	Lock( m_lspSendBuffer.m_cs );

	m_lspSendBuffer.RemoveData( dwBytes );
	
	if( m_lspSendBuffer.IsEmpty() )
	{
		if( !m_bDoNotWrite )
			m_nReference--;
		return m_bDoNotWrite;
	}

	m_lspSendBuffer.GetData( m_lpBuffers, &dwBufferCount );

	memset( (void*)&m_ovSend, 0, sizeof(WSAOVERLAPPED) );
	int err;
	if( WSASend( GetHandle(), m_lpBuffers, dwBufferCount, &dwBytes, 0, &m_ovSend, NULL ) != 0 && ( err = WSAGetLastError() ) != WSA_IO_PENDING )
	{
//		TRACE( "i/O error, close socket %d //REF:%d\n", GetHandle(), m_l-1 );
		return err;
	}
	return 0;
}

CBuffer* CClientSock::Fetch( DWORD dwBytes )
{
#ifdef __INFO_SOCKLIB0102
	if( m_dwReadHeaderSize == HEADERSIZE13 )
	{
		m_dwReadBytes	+= dwBytes;
		m_dwReadPacks++;
		time_t t	= time( NULL );
		if( t != m_tRead )
		{
			if( m_dwReadBytes > MAX_READ_BYTES_A_SEC )
			{
				OutputDebugString( "MAX_READ_BYTES_A_SEC" );
				return NULL;
			}
			if( m_dwReadPacks > MAX_READ_PACKS_A_SEC )
			{
				OutputDebugString( "MAX_READ_PACK_A_SEC" );
				return NULL;
			}
			char lpOutputString[100];
			sprintf( lpOutputString, "%d(bytes), %d(packets)", m_dwReadBytes, m_dwReadPacks );
			OutputDebugString( lpOutputString );

			m_dwReadBytes	= m_dwReadPacks	= 0;
			m_tRead		= t;
		}
	}
#endif	// __INFO_SOCKLIB0102

#ifdef __SO1014
	if( m_hSocket == INVALID_SOCKET )
	{
		WriteError( "FETCH//0" );
		WSASetLastError( ERROR_BAD_NET_NAME );
		return NULL;
	}
/*
	if( m_dwReadHeaderSize == HEADERSIZE13 )
	{
		if( dwBytes > MAX_BUFFER )	//
		{
			WriteError( "FETCH//200//%d", dwBytes );
			WSASetLastError( ERROR_BAD_NET_NAME );
			return NULL;
		}
	}
*/
#endif	// __SO1014

	m_pRecvBuffer->m_pTail	+=	dwBytes;

//	ASSERT( m_pRecvBuffer->m_pTail <= m_pRecvBuffer->m_lpBufMax );
//#ifdef __SO1014
//	if( m_pRecvBuffer->m_pTail > m_pRecvBuffer->m_lpBufMax )
//	{
//		WriteError( "FETCH//1" );
//		WSASetLastError( ERROR_BAD_NET_NAME );
//		return NULL;
//	}
//#endif	// __SO1014
	
	int nRemnant;
	LPBYTE ptr	= m_pRecvBuffer->GetReadableBuffer( &nRemnant );

	CBuffer* pOld	= NULL;
	u_long uPacketSize;

#ifdef __CRC
	DWORD dwCrc, dwDataSize;
	byte digest[4];
#endif	// __CRC

	INIT_LOOP;
	while( 1 )
	{
		VERIFY_LOOP( __FILE__, __LINE__ );
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
//				ASSERT( m_pRecvBuffer->m_pTail + nRemnant <= m_pRecvBuffer->m_lpBufMax ); 
#ifdef __SO1014
//				if( m_pRecvBuffer->m_pTail + nRemnant > m_pRecvBuffer->m_lpBufMax )
//				{
//					WriteError( "FETCH//2" );
//					WSASetLastError( ERROR_BAD_NET_NAME );
//					return NULL;
//				}
				if( !m_pRecvBuffer->m_lpBufStart )
				{
					WriteError( "FETCH//2" );
					WSASetLastError( ERROR_BAD_NET_NAME );
					return NULL;
				}
#endif	// __SO1014
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
				int nOffset	= sizeof(char);
#ifdef __PACKET_REPLAY_R
				int nPacket	= *(UNALIGNED int*)( ptr + nOffset );
				nOffset	+= sizeof(int);
				if( nPacket != m_nPacket + 1 )
				{
					char lpAddr[16]		= { 0,};
					GetPeerAddr( DPID_UNKNOWN, lpAddr, NULL );
					Error( "CClientSock.Fetch: replay: %s", lpAddr );
					WSASetLastError( ERROR_BAD_NET_NAME );
					return NULL;
				}
				//m_nPacket++;
#endif	// __PACKET_REPLAY_R
				dwCrc	= *(UNALIGNED LPDWORD)( ptr + nOffset );
				nOffset	+= sizeof(DWORD);
				dwDataSize	= *(UNALIGNED LPDWORD)( ptr + nOffset );
				nOffset	+= sizeof(DWORD);
				m_crcRead.Restart();
				m_crcRead.Update( (const byte*)( &dwDataSize ), sizeof(DWORD) );
				m_crcRead.Final( digest );
#ifdef __PACKET_REPLAY_R
				if( ~( *(UNALIGNED LPDWORD)digest ^ ( m_dwProtocolId + nPacket ) ) != dwCrc )
#else	// __PACKET_REPLAY_R
				if( ~( *(UNALIGNED LPDWORD)digest ^ m_dwProtocolId ) != dwCrc )
#endif	// __PACKET_REPLAY_R
				{
					WSASetLastError( ERROR_BAD_NET_NAME );
					return NULL;
				}
				dwCrc	= *(UNALIGNED LPDWORD)( ptr + nOffset );
				nOffset	+= sizeof(DWORD);
				uPacketSize		= m_dwReadHeaderSize + dwDataSize;
#ifdef __SO1014
				if( dwDataSize > MAX_BUFFER )	// if( uPacketSize > MAX_BUFFER )	// kang: 2008/08/14
				{
					char lpAddr[16]		= { 0,};
					GetPeerAddr( DPID_UNKNOWN, lpAddr, NULL );
					Error( "CClientSocket.Fetch: ip = %s, size = %d", lpAddr, dwDataSize );
					WSASetLastError( ERROR_BAD_NET_NAME );
					return NULL;
				}
#endif	// __SO1014
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
				if( (int)( uPacketSize ) > m_pRecvBuffer->GetSize() || m_pRecvBuffer->cb > 0 )
				{
					pOld	= m_pRecvBuffer;
					m_pRecvBuffer	= CBufferFactory::GetInstance().CreateBuffer( m_nBufferType, uPacketSize ); 
					if( !m_pRecvBuffer->m_lpBufStart )
					{
						char lpAddr[16]		= { 0,};
						GetPeerAddr( DPID_UNKNOWN, lpAddr, NULL );
						Error( "CClientSocket.Fetch: ip = %s, size = %d", lpAddr, uPacketSize );
						WSASetLastError( ERROR_BAD_NET_NAME );
						return NULL;
					}
				}
				if( pOld )
				{
#ifdef __SO1014
					if( !m_pRecvBuffer->m_lpBufStart )
					{
						WriteError( "FETCH//3" );
						WSASetLastError( ERROR_BAD_NET_NAME );
						return NULL;
					}
#endif	// __SO1014
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
#ifdef __PACKET_REPLAY_R
					if( ~( *(UNALIGNED LPDWORD)digest ^ ( m_dwProtocolId + m_nPacket + 1 ) ) != dwCrc )
#else	// __PACKET_REPLAY_R
					if( ~( *(UNALIGNED LPDWORD)digest ^ m_dwProtocolId ) != dwCrc )
#endif	// __PACKET_REPLAY_R
					{
						char lpAddr[16]		= { 0,};
						GetPeerAddr( DPID_UNKNOWN, lpAddr, NULL );
						Error( "CClientSock.Fetch: replay: %s", lpAddr );
						WSASetLastError( ERROR_BAD_NET_NAME );
						return NULL;
					}
				}
#endif	// __CRC
#ifdef __PACKET_REPLAY_R
				m_nPacket++;
#endif // __PACKET_REPLAY_R
				m_pRecvBuffer->cb++;
				nRemnant	-= ( uPacketSize );
				ptr		+= ( uPacketSize );
			}
		}
	}



	return NULL;
}

HRESULT CClientSock::GetPeerAddr( DPID dpidPlayer, LPVOID lpAddr, LPDWORD lpdwSize )
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

CBuffer* CClientSock::CreateBuffer()
{
	return CBufferFactory::GetInstance().CreateBuffer( m_nBufferType );
}

#ifdef __PROTOCOL0910
void CClientSock::SendProtocolId( void )
{
#ifdef __SO1014
	if( m_hSocket == INVALID_SOCKET )
	{
		WriteError( "INVALID_SOCKET//0" );
		return;
	}
#endif	// __SO1014
	m_dwProtocolId	= GetTickCount();
	char lpData[8];
	*( (UNALIGNED DWORD*)lpData )	= 0x00000000;
	*( (UNALIGNED DWORD*)&lpData[4] )	= m_dwProtocolId;
	Send( lpData, 8, m_hSocket );
}
#endif	// __PROTOCOL0910

#ifdef __TRAFIC_1218
/*
void	CClientSock::AddTraficLog( DWORD dwBytes, int nType )
{
	sm_adwBytes[nType]	+= dwBytes;
}

void	CClientSock::PrintTraficLog( void )
{
	char lpOutputString[100]	= { 0,};
	sprintf( lpOutputString, "read\t%d\tbytes\twrite\t%d\tbytes", 
		sm_adwBytes[CClientSock::read], sm_adwBytes[CClientSock::write] );
	memset( sm_adwBytes, 0, sizeof(sm_adwBytes) );
	Error( lpOutputString );
}
*/
#endif	// __TRAFIC_1218