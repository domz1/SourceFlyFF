#include "stdafx.h"
#include "buffer.h"

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
#ifdef __VM_0819
MemPooler<CBuffer2>*	CBuffer2::m_pPool2	= new MemPooler<CBuffer2>( 512, "CBuffer2" );
MemPooler<CBuffer>*	CBuffer::m_pPool	= new MemPooler<CBuffer>( 512, "CBuffer" );
#else	// __VM_0819
MemPooler<CBuffer2>*	CBuffer2::m_pPool2	= new MemPooler<CBuffer2>(512);
MemPooler<CBuffer>*	CBuffer::m_pPool	= new MemPooler<CBuffer>(512);
#endif	// __VM_0819
#endif	// __MEM_TRACE
#endif	// __VM_0820

#ifdef __VM_0819
CHeapMng*	CBuffer::m_pHeapMng		= new CHeapMng( "CBuffer" );
#else	// __VM_0819
CHeapMng*	CBuffer::m_pHeapMng		= new CHeapMng;
#endif	// __VM_0819

CBuffer::CBuffer( u_long uBufSize )
{
	if( uBufSize > MAX_BUFFER ) {
		m_lpBufStart	= (LPBYTE)CBuffer::m_pHeapMng->Malloc( uBufSize );
//		ASSERT( m_lpBufStart );
	}
	else {
		m_lpBufStart	= m_lpBuf;
		uBufSize	= MAX_BUFFER;
	}
	m_pHead		= m_pTail	= m_lpBufStart;
	m_lpBufMax	= m_lpBufStart + uBufSize;
	cb	= 0;
	dpid	= DPID_UNKNOWN;
	pPrevious	= pNext		= NULL;
	m_dwHeaderSize = sizeof(DWORD) + 1;
}

CBuffer::~CBuffer()
{
	if( m_lpBufMax - m_lpBufStart > MAX_BUFFER )
	{
#ifdef __SO1014
		if( m_lpBufStart )
#endif	// __SO1014
#ifdef __VM_0819
			CBuffer::m_pHeapMng->Free( m_lpBufStart, GetSize() );
#else	// __VM_0819
			CBuffer::m_pHeapMng->Free( m_lpBufStart );
#endif	// __VM_0819
	}
}

/*--------------------------------------------------------------------------------*/
CBufferQueue::CBufferQueue()
{
	m_uCount	= 0;
	m_pHead		= m_pTail	= NULL;
#ifdef __CRC
	m_pcrc	= NULL;
	m_dwWriteHeaderSize		= HEADERSIZE5;
#endif	// __CRC

#ifdef __PROTOCOL0910
	m_pdwProtocolId		= NULL;
#endif	// __PROTOCOL0910

#ifdef __PACKET_REPLAY_W
	m_nPacket	= 0;
#endif	// __PACKET_REPLAY_W
}

CBufferQueue::~CBufferQueue()
{
	Clear( TRUE );	// 2004/01/19
}

void CBufferQueue::Clear( BOOL bDelete )
{
	CMclAutoLock	Lock( m_cs );

	if( bDelete )
	{
		CBuffer* pBuffer	= GetHead(), *ptrtmp;

		INIT_LOOP;
		while( pBuffer )
		{
			VERIFY_LOOP( __FILE__, __LINE__ );

			ptrtmp	= pBuffer->pNext;
			SAFE_DELETE( pBuffer );
			pBuffer	= ptrtmp;
		}
	}
	m_uCount	= 0;
	m_pHead	= m_pTail	= NULL;
}

void CBufferQueue::AddData( LPBYTE lpData, u_long uDataSize, BUFFER_TYPE type )
{
//	CMclAutoLock	Lock( m_cs );
	u_long	uRemnant	= uDataSize;
	CBuffer* pBuffer	= GetTail();
	LPBYTE ptr;
	int cb;

	if( pBuffer )
	{
		cb	= pBuffer->GetWritableBufferSize();
#ifdef __CRC
		if( cb < (int)( m_dwWriteHeaderSize ) )
#else	// __CRC
		if( cb < (int)( pBuffer->GetHeaderLength() ) )
#endif	// __CRC
		{
			pBuffer	= CBufferFactory::GetInstance().CreateBuffer( type );
			AddTail( pBuffer );
		}
	}
	else
	{
		pBuffer	= CBufferFactory::GetInstance().CreateBuffer( type );
		AddTail( pBuffer );
	}
	
	ptr	= pBuffer->GetWritableBuffer( &cb );

#ifdef __CRC
//	header
	*ptr	= HEADERMARK;

	if( m_pcrc )
	{
		BYTE digest[sizeof(DWORD)];
		m_pcrc->Restart();
		m_pcrc->Update( (const byte*)( &uDataSize ), sizeof(u_long) );
		m_pcrc->Final( digest );
		int nOffset	= sizeof(char);
#ifdef __PACKET_REPLAY_W
		*(UNALIGNED int*)( ptr + nOffset )		= ++m_nPacket;
		nOffset	+= sizeof(int);
		*(UNALIGNED DWORD*)( ptr + nOffset )	= ~( *(UNALIGNED DWORD*)digest ^ ( *m_pdwProtocolId + m_nPacket ) );
#else	// __PACKET_REPLAY_W
		*(UNALIGNED DWORD*)( ptr + nOffset )	= ~( *(UNALIGNED DWORD*)digest ^ *m_pdwProtocolId );
#endif	// __PACKET_REPLAY_W
		nOffset	+= sizeof(DWORD);
		*(UNALIGNED DWORD*)( ptr + nOffset )	= (DWORD)uDataSize;
		nOffset	+= sizeof(DWORD);
		m_pcrc->Restart();
		m_pcrc->Update( (const byte*)( lpData ), (u_int)uDataSize );
		m_pcrc->Final( digest );
#ifdef __PACKET_REPLAY_W
		*(UNALIGNED DWORD*)( ptr + nOffset )	= ~( *(UNALIGNED DWORD*)digest ^ ( *m_pdwProtocolId + m_nPacket ) );
#else	// __PACKET_REPLAY_W
		*(UNALIGNED DWORD*)( ptr + nOffset )	= ~( *(UNALIGNED DWORD*)digest ^ *m_pdwProtocolId );
#endif	// __PACKET_REPLAY_W
		nOffset	+= sizeof(DWORD);
	}
	else
	{
		*(UNALIGNED DWORD*)&ptr[1]	= (DWORD)uDataSize;
	}
//	data
	cb	= ( cb-m_dwWriteHeaderSize < uRemnant? cb-m_dwWriteHeaderSize: uRemnant );
	ASSERT( &ptr[m_dwWriteHeaderSize] + cb <= pBuffer->m_lpBufMax );
	memcpy( (void*)&ptr[m_dwWriteHeaderSize], lpData, cb );
	pBuffer->m_pTail	= ptr + m_dwWriteHeaderSize + cb;

#else	// __CRC

//	header
	pBuffer->SetHeader( uDataSize );	
	cb -= pBuffer->GetHeaderLength();

	if( cb >= (int)( uRemnant ) )
		cb = uRemnant;

	memcpy( pBuffer->m_pTail, lpData, cb );
	pBuffer->m_pTail +=  cb;

#endif	// __CRC

	uRemnant	-= cb;

	INIT_LOOP;
	while( uRemnant > 0 )
	{
		VERIFY_LOOP( __FILE__, __LINE__ );

		pBuffer	= CBufferFactory::GetInstance().CreateBuffer( type );
		ptr	= pBuffer->GetWritableBuffer( &cb );
		cb	= ( cb < (int)( uRemnant ) ? cb: uRemnant );
		
		memcpy( (void*)ptr, &lpData[uDataSize - uRemnant], cb );
		pBuffer->m_pTail	= ptr + cb;
		AddTail( pBuffer );
		uRemnant	-= cb;
	}
}

void CBufferQueue::RemoveData( u_long uDataSize )
{
	CBuffer* pBuffer;
	
//	CMclAutoLock	Lock( m_cs );
	u_long uRemnant;

	INIT_LOOP;
	while( uDataSize > 0 )
	{
		VERIFY_LOOP( __FILE__, __LINE__ );

		pBuffer	= GetHead();
//		ASSERT( pBuffer );
		if( NULL == pBuffer )
			return;

		uRemnant	= pBuffer->GetReadableBufferSize();
		if( uRemnant > uDataSize )
		{
			pBuffer->m_pHead	+= uDataSize;
			uDataSize	= 0;
		}
		else
		{
			RemoveHead();
//			SAFE_DELETE( pBuffer );
			safe_delete( pBuffer );
			uDataSize	-= uRemnant;
		}
	}
}

void CBufferQueue::GetData( LPWSABUF lpBuffers, LPDWORD lpdwBufferCount )
{
	CBuffer* pBuffer;
	int	i	= 0;
//	*lpdwBufferCount	= 0;
//	CMclAutoLock	Lock( m_cs );	
	pBuffer	= GetHead();

	while( pBuffer && i < MAX_WSABUF )
	{
		lpBuffers[i].buf	= (char*)pBuffer->GetReadableBuffer( (int*)&lpBuffers[i].len );
		i++;
		pBuffer	= pBuffer->pNext;
	}
	*lpdwBufferCount	= i;
}

/*--------------------------------------------------------------------------------*/
CBufferFactory::CBufferFactory()
{
}

CBufferFactory& CBufferFactory::GetInstance()
{
	static CBufferFactory g_factory;
	return g_factory;
}

CBuffer* CBufferFactory::CreateBuffer( BUFFER_TYPE type, u_long uBufSize )
{
	CBuffer* pBuffer = NULL;
	switch( type )
	{
	case BUFFER_TYPE_5BYTE:
		pBuffer = new CBuffer( uBufSize );
		break;
	case BUFFER_TYPE_2BYTE:
		pBuffer = new CBuffer2( uBufSize );
		break;
	default:
		ASSERT(0);
		break;
	}
	return pBuffer;
}


