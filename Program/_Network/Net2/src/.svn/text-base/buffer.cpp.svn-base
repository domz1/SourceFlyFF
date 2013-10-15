#include "stdafx.h"
#include "buffer.h"
#include "dpmng.h"

#include "buyinginfo.h"

#ifndef __VM_0820
MemPooler<CBuffer>*	CBuffer::m_pPool	= new MemPooler<CBuffer>(512);
#endif	// __VM_0820
CHeapMng*	CBuffer::m_pHeapMng		= new CHeapMng;

#ifndef __VM_0820
MemPooler<CBuffer2>*	CBuffer2::m_pPool2	= new MemPooler<CBuffer2>(128);
MemPooler<CBuffer3>*	CBuffer3::m_pPool2	= new MemPooler<CBuffer3>(128);
#endif	// __VM_0820

CBuffer::CBuffer( u_long uBufSize )
{
	if( uBufSize > MAX_BUFFER ) {
		m_lpBufStart	= (LPBYTE)CBuffer::m_pHeapMng->Malloc( uBufSize );
	}
	else {
		m_lpBufStart	= m_lpBuf;
		uBufSize	= MAX_BUFFER;
	}

	ASSERT( m_lpBufStart );

	m_pHead		= m_pTail	= m_lpBufStart;
	m_lpBufMax	= m_lpBufStart + uBufSize;
	cb	= 0;
	dpid	= DPID_UNKNOWN;
	pPrevious	= pNext		= NULL;

	m_pcrc	= NULL;
}

CBuffer::~CBuffer()
{
	if( m_lpBufMax - m_lpBufStart > MAX_BUFFER )
		CBuffer::m_pHeapMng->Free( m_lpBufStart );
}

void CBuffer::AddSysMsg( void* pMsg, u_long uMsgSize )
{
	dpid	= DPID_SYSMSG;
	cb++;
	*m_pTail	= SYSHEADERMARK;
	m_pTail++;
	if( m_pcrc )
		m_pTail	+= sizeof(DWORD);	// + 4
	*(UNALIGNED DWORD*)m_pTail	= uMsgSize;
	m_pTail	+= sizeof(DWORD);
	if( m_pcrc )
		m_pTail	+= sizeof(DWORD);	// + 4

	memcpy( m_pTail, pMsg, uMsgSize );
	m_pTail	+= uMsgSize;
}

void CBuffer::AddHdr( LPBYTE lpData, u_long uDataSize )
{
	int cb;
	LPBYTE ptr	= GetWritableBuffer( &cb );
//	header
	*ptr	= HEADERMARK;

	if( m_pcrc )
	{
		BYTE digest[sizeof(DWORD)];
		m_pcrc->Restart();
		m_pcrc->Update( (const byte*)( &uDataSize ), sizeof(u_long) );
		m_pcrc->Final( digest );
		*(UNALIGNED DWORD*)( ptr + sizeof(char) )	= *(UNALIGNED DWORD*)digest;
		*(UNALIGNED DWORD*)( ptr + sizeof(char) + sizeof(DWORD) )	= (DWORD)uDataSize;
		m_pcrc->Restart();
		m_pcrc->Update( (const byte*)( lpData ), (u_int)uDataSize );
		m_pcrc->Final( digest );
		*(UNALIGNED DWORD*)( ptr + sizeof(char) +sizeof(DWORD) + sizeof(DWORD) )	= *(UNALIGNED DWORD*)digest;
	}
	else
	{
		*(UNALIGNED DWORD*)&ptr[1]	= (DWORD)uDataSize;
	}
	m_pTail		= ptr + CBuffer::GetHdrSize( (BOOL)m_pcrc );
}

void CBuffer::AddData( LPBYTE lpData, u_long uDataSize, u_long &uRemnant )
{
	DWORD cb;
	LPBYTE ptr	= GetWritableBuffer( (int *)&cb );

	cb	= ( cb < uRemnant? cb: uRemnant );
	ASSERT( ptr + cb <= m_lpBufMax );
	memcpy( (void*)ptr, &lpData[uDataSize - uRemnant], cb );
	m_pTail	= ptr + cb;
	uRemnant	-= cb;
}

void CBuffer::MessageHandler( CDPMng<CBuffer>* pDPMng, DPID dpidFrom )
{
	BYTE* ptr	= m_lpBufStart;
	u_long uHdrSize	= CBuffer::GetHdrSize( (BOOL)m_pcrc );
	u_long uDataSizeOffset	= ( m_pcrc? 5: 1 );
	while( cb > 0 )
	{
		u_long uDataSize	= *(UNALIGNED LPDWORD)( ptr + uDataSizeOffset );
		if( dpidFrom == DPID_SYSMSG )
			pDPMng->SysMessageHandler( (LPDPMSG_GENERIC)&ptr[uHdrSize], uDataSize, dpidFrom );
		else
			pDPMng->UserMessageHandler( (LPDPMSG_GENERIC)&ptr[uHdrSize], uDataSize, dpidFrom );
		ptr	+=	uHdrSize + uDataSize;
		cb--;
	}
}

CBuffer* CBuffer::Fetch( CBuffer** ppRecvBuffer, DWORD dwBytes, CRC32* pcrc )
{
	crc( pcrc );	// m_pcrc

	m_pTail		+= dwBytes;
	ASSERT( m_pTail <= m_lpBufMax );
	
	int nRemnant;
	LPBYTE ptr	= GetReadableBuffer( &nRemnant );

	CBuffer* pOld	= NULL, *pBuffer;
	u_long uPacketSize;

	DWORD dwCrc, dwDataSize;
	byte digest[4];
	int	uHdrSize	= (int)( CBuffer::GetHdrSize( (BOOL)pcrc ) );

	while( 1 )
	{
		if( nRemnant < uHdrSize )
		{
			if( cb > 0 ) {
				pOld	= *ppRecvBuffer;
				pOld->m_pTail	-= nRemnant;	// remove remnant from old buffer
				pBuffer	= new CBuffer;
				
				ASSERT( pBuffer->m_pTail + nRemnant <= pBuffer->m_lpBufMax ); 
				memcpy( pBuffer->m_pTail, ptr, nRemnant );
				pBuffer->m_pTail	+=	nRemnant;
				*ppRecvBuffer	= pBuffer;
			}
			return pOld;
		}
		else
		{
			if( pcrc )
			{
				dwCrc	= *(UNALIGNED LPDWORD)( ptr + sizeof(char) );
				dwDataSize	= *(UNALIGNED LPDWORD)( ptr + sizeof(char) + sizeof(DWORD) );
				pcrc->Restart();
				pcrc->Update( (const byte*)( &dwDataSize ), sizeof(DWORD) );
				pcrc->Final( digest );
				if( *(UNALIGNED LPDWORD)digest != dwCrc )
				{
					WSASetLastError( ERROR_BAD_NET_NAME );
					return NULL;
				}
				dwCrc	= *(UNALIGNED LPDWORD)( ptr + sizeof(char) + sizeof(DWORD) + sizeof(DWORD) );
				uPacketSize		= uHdrSize + dwDataSize;
			}
			else
			{
				uPacketSize	= uHdrSize + *(UNALIGNED LPDWORD)&ptr[1];
			}
			if( nRemnant < (int)( uPacketSize ) )
			{
				if( (int)( uPacketSize ) > (*ppRecvBuffer)->GetSize() )
				{
					pOld	= *ppRecvBuffer;
					pBuffer	= new CBuffer( uPacketSize );
				}
				else
				{
					if( (*ppRecvBuffer)->cb > 0 )
					{
						pOld	= *ppRecvBuffer;
						pBuffer	= new CBuffer;
					}
				}

				if( pOld )
				{
					ASSERT( pBuffer->m_pTail + nRemnant <= pBuffer->m_lpBufMax );
					memcpy( pBuffer->m_pTail, ptr, nRemnant );
//					if( nRemnant > 0 )
//					{
//						ASSERT( CBuffer::IsHeader( *ptr ) );
//					}
					pBuffer->m_pTail	+=	nRemnant;
					*ppRecvBuffer	= pBuffer;
				}

				return pOld;
			}
			else	// completion
			{
				if( pcrc )
				{
					pcrc->Restart();
					pcrc->Update( (const byte*)( ptr + uHdrSize ), dwDataSize );
					pcrc->Final( digest );
					if( *(UNALIGNED LPDWORD)digest != dwCrc )
					{
						WSASetLastError( ERROR_BAD_NET_NAME );
						return NULL;
					}
				}
				(*ppRecvBuffer)->cb++;
				nRemnant	-= ( uPacketSize );
				ptr		+= ( uPacketSize );
			}
		}
	}
	return NULL;
}
/*--------------------------------------------------------------------------------*/
void CBuffer2::AddSysMsg( void* pMsg, u_long uMsgSize )
{
	dpid	= DPID_SYSMSG;
	cb++;

	if( m_pcrc )
		m_pTail	+= sizeof(DWORD);	// + 4
	*(UNALIGNED WORD*)m_pTail	= htons( (WORD)( uMsgSize + 2 ) );
	m_pTail	+= sizeof(WORD);
	if( m_pcrc )
		m_pTail	+= sizeof(DWORD);	// + 4

	memcpy( m_pTail, pMsg, uMsgSize );
	m_pTail	+= uMsgSize;
}

void CBuffer2::AddHdr( LPBYTE lpData, u_long uDataSize )
{
	int cb;
	LPBYTE ptr	= GetWritableBuffer( &cb );

	if( m_pcrc )
	{
		BYTE digest[sizeof(DWORD)];
		m_pcrc->Restart();
		m_pcrc->Update( (const byte*)( &uDataSize ), sizeof(u_long) );
		m_pcrc->Final( digest );
		*(UNALIGNED DWORD*)ptr	= *(UNALIGNED DWORD*)digest;
//		*(UNALIGNED WORD*)( ptr + sizeof(DWORD) )	= (WORD)uDataSize;
		*(UNALIGNED WORD*)( ptr + sizeof(DWORD) )	= htons( (WORD)( uDataSize + 2 ) );
		m_pcrc->Restart();
		m_pcrc->Update( (const byte*)( lpData ), (u_int)uDataSize );
		m_pcrc->Final( digest );
		*(UNALIGNED DWORD*)( ptr + sizeof(DWORD) + sizeof(WORD) )	= *(UNALIGNED DWORD*)digest;
	}
	else
	{
//		*(UNALIGNED WORD*)ptr	= (WORD)uDataSize;
		*(UNALIGNED WORD*)ptr	= htons( (WORD)( uDataSize + 2 ) );
	}
	m_pTail		= ptr + CBuffer2::GetHdrSize( (BOOL)m_pcrc );
}

void CBuffer2::MessageHandler( CDPMng<CBuffer2>* pDPMng, DPID dpidFrom )
{
	BYTE* ptr	= m_lpBufStart;
	u_long uHdrSize	= CBuffer2::GetHdrSize( (BOOL)m_pcrc );
	u_long uDataSizeOffset	= ( m_pcrc? 4: 0 );
	while( cb > 0 )
	{
		u_long uDataSize	= ntohs( *(UNALIGNED LPWORD)( ptr + uDataSizeOffset ) ) - 2;
		if( dpidFrom == DPID_SYSMSG )
			pDPMng->SysMessageHandler( (LPDPMSG_GENERIC)&ptr[uHdrSize], uDataSize, dpidFrom );
		else
			pDPMng->UserMessageHandler( (LPDPMSG_GENERIC)&ptr[uHdrSize], uDataSize, dpidFrom );
		ptr	+=	uHdrSize + uDataSize;
		cb--;
	}
}

CBuffer2* CBuffer2::Fetch( CBuffer2** ppRecvBuffer, DWORD dwBytes, CRC32* pcrc )
{
	crc( pcrc );	// m_pcrc

	m_pTail		+= dwBytes;
	ASSERT( m_pTail <= m_lpBufMax );
	
	int nRemnant;
	LPBYTE ptr	= GetReadableBuffer( &nRemnant );

	CBuffer2* pOld	= NULL, *pBuffer;
	u_long uPacketSize;

	DWORD dwCrc, dwDataSize;
	byte digest[4];
	u_long uHdrSize	= CBuffer2::GetHdrSize( (BOOL)pcrc );

	while( 1 )
	{
		if( nRemnant < (int)( uHdrSize ) )
		{
			if( cb > 0 ) {
				pOld	= *ppRecvBuffer;
				pOld->m_pTail	-= nRemnant;	// remove remnant from old buffer
				pBuffer	= new CBuffer2;
				
				ASSERT( pBuffer->m_pTail + nRemnant <= pBuffer->m_lpBufMax ); 
				memcpy( pBuffer->m_pTail, ptr, nRemnant );
				pBuffer->m_pTail	+=	nRemnant;
				*ppRecvBuffer	= pBuffer;
			}
			return pOld;
		}
		else
		{
			if( pcrc )
			{
				dwCrc	= *(UNALIGNED LPDWORD)ptr;
				dwDataSize	= ntohs( *(UNALIGNED LPWORD)( ptr + sizeof(DWORD) ) ) -2;
				pcrc->Restart();
				pcrc->Update( (const byte*)( &dwDataSize ), sizeof(DWORD) );
				pcrc->Final( digest );
				if( *(UNALIGNED LPDWORD)digest != dwCrc )
				{
					WSASetLastError( ERROR_BAD_NET_NAME );
					return NULL;
				}
				dwCrc	= *(UNALIGNED LPDWORD)( ptr + sizeof(DWORD) + sizeof(WORD) );
				uPacketSize		= uHdrSize + dwDataSize;
			}
			else
			{
				uPacketSize	= uHdrSize + ntohs( *(UNALIGNED LPWORD)ptr ) - 2;
			}
			if( nRemnant < (int)( uPacketSize ) )
			{
				if( (int)( uPacketSize ) > (*ppRecvBuffer)->GetSize() )
				{
					pOld	= *ppRecvBuffer;
					pBuffer	= new CBuffer2( uPacketSize );
				}
				else
				{
					if( (*ppRecvBuffer)->cb > 0 )
					{
						pOld	= *ppRecvBuffer;
						pBuffer	= new CBuffer2;
					}
				}

				if( pOld )
				{
					ASSERT( pBuffer->m_pTail + nRemnant <= pBuffer->m_lpBufMax );
					memcpy( pBuffer->m_pTail, ptr, nRemnant );
//					if( nRemnant > 0 )
//					{
//						ASSERT( CBuffer::IsHeader( *ptr ) );
//					}
					pBuffer->m_pTail	+=	nRemnant;
					*ppRecvBuffer	= pBuffer;
				}

				return pOld;
			}
			else	// completion
			{
				if( pcrc )
				{
					pcrc->Restart();
					pcrc->Update( (const byte*)( ptr + uHdrSize ), dwDataSize );
					pcrc->Final( digest );
					if( *(UNALIGNED LPDWORD)digest != dwCrc )
					{
						WSASetLastError( ERROR_BAD_NET_NAME );
						return NULL;
					}
				}
				(*ppRecvBuffer)->cb++;
				nRemnant	-= ( uPacketSize );
				ptr		+= ( uPacketSize );
			}
		}
	}
	return NULL;
}
/*--------------------------------------------------------------------------------*/
void CBuffer3::AddSysMsg( void* pMsg, u_long uMsgSize )
{
	dpid	= DPID_SYSMSG;
	cb++;

//	if( m_pcrc )
//		m_pTail	+= sizeof(DWORD);	// + 4
	*(UNALIGNED WORD*)m_pTail	= (WORD)uMsgSize;
	m_pTail	+= sizeof(WORD);
//	if( m_pcrc )
//		m_pTail	+= sizeof(DWORD);	// + 4
	memcpy( m_pTail, pMsg, uMsgSize );
	m_pTail	+= uMsgSize;
}

void CBuffer3::AddHdr( LPBYTE lpData, u_long uDataSize )
{
/*
	int cb;
	LPBYTE ptr	= GetWritableBuffer( &cb );

	if( m_pcrc )
	{
		BYTE digest[sizeof(DWORD)];
		m_pcrc->Restart();
		m_pcrc->Update( (const byte*)( &uDataSize ), sizeof(u_long) );
		m_pcrc->Final( digest );
		*(UNALIGNED DWORD*)ptr	= *(UNALIGNED DWORD*)digest;
//		*(UNALIGNED WORD*)( ptr + sizeof(DWORD) )	= (WORD)uDataSize;
		*(UNALIGNED WORD*)( ptr + sizeof(DWORD) )	= htons( (WORD)( uDataSize + 2 ) );
		m_pcrc->Restart();
		m_pcrc->Update( (const byte*)( lpData ), (u_int)uDataSize );
		m_pcrc->Final( digest );
		*(UNALIGNED DWORD*)( ptr + sizeof(DWORD) + sizeof(WORD) )	= *(UNALIGNED DWORD*)digest;
	}
	else
	{
//		*(UNALIGNED WORD*)ptr	= (WORD)uDataSize;
		*(UNALIGNED WORD*)ptr	= htons( (WORD)( uDataSize + 2 ) );
	}
	m_pTail		= ptr + CBuffer3::GetHdrSize( (BOOL)m_pcrc );
*/
}

void CBuffer3::MessageHandler( CDPMng<CBuffer3>* pDPMng, DPID dpidFrom )
{
	BYTE* ptr	= m_lpBufStart;
	u_long uPacketSize;
	while( cb > 0 )
	{
		uPacketSize		= 0;
		if( dpidFrom == DPID_SYSMSG )
		{
			uPacketSize		= sizeof(WORD) + *(UNALIGNED LPWORD)ptr;
			pDPMng->SysMessageHandler( (LPDPMSG_GENERIC)&ptr[sizeof(WORD)], *(UNALIGNED LPWORD)ptr, dpidFrom );
		}
		else
		{
			uPacketSize		= sizeof(BUYING_INFO);
			pDPMng->UserMessageHandler( (LPDPMSG_GENERIC)ptr, sizeof(BUYING_INFO), dpidFrom );
		}
		ptr	+=	uPacketSize;
		cb--;
	}
}

CBuffer3* CBuffer3::Fetch( CBuffer3** ppRecvBuffer, DWORD dwBytes, CRC32* pcrc )
{
	crc( pcrc );	// m_pcrc

	m_pTail		+= dwBytes;
	ASSERT( m_pTail <= m_lpBufMax );
	
	int nRemnant;
	LPBYTE ptr	= GetReadableBuffer( &nRemnant );

	CBuffer3* pOld	= NULL, *pBuffer;

	while( 1 )
	{
		if( nRemnant < sizeof(BUYING_INFO) )
		{
			if( (*ppRecvBuffer)->cb > 0 )
			{
				pOld	= *ppRecvBuffer;
				pBuffer	= new CBuffer3;
			}
			if( pOld )
			{
				ASSERT( pBuffer->m_pTail + nRemnant <= pBuffer->m_lpBufMax );
				memcpy( pBuffer->m_pTail, ptr, nRemnant );
				
				pBuffer->m_pTail	+=	nRemnant;
				*ppRecvBuffer	= pBuffer;
			}
			return pOld;
		}
		else	// completion
		{
			(*ppRecvBuffer)->cb++;
			nRemnant	-= sizeof(BUYING_INFO);
			ptr		+= sizeof(BUYING_INFO);
		}
	}
	return NULL;
}