#ifndef __BUFFER_H__
#define	__BUFFER_H__

#pragma once

//#include "mempool.h"
#include "mempooler.h"
#include "heapmng.h"
#include <dplay.h>

#include "crc.h"

#define	HEADERMARK	'^'	// Normal message header
#define	SYSHEADERMARK	'%'	// System message header

#define	MAX_BUFFER	8192
#define	MAX_WSABUF	128

template <class>
class CDPMng;

class CBuffer
{
public:
	DPID	dpid;
	u_long	cb;
	BYTE	m_lpBuf[MAX_BUFFER];
	LPBYTE	m_lpBufStart;
	LPBYTE	m_lpBufMax;
	LPBYTE	m_pHead;
	LPBYTE	m_pTail;
	CBuffer*	pNext;
	CBuffer*	pPrevious;
	CRC32*	m_pcrc;

public:
//	Constructions
	CBuffer( u_long uBufSize = MAX_BUFFER );
	virtual	~CBuffer();
//	Operations
	int		GetSize( void );
	LPBYTE	GetWritableBuffer( int* pnBufSize );
	int		GetWritableBufferSize( void );
	LPBYTE	GetReadableBuffer( int* pnBufSize );
	int		GetReadableBufferSize( void );

	void	crc( CRC32* pcrc )	{	m_pcrc	= pcrc;		}
	void	MessageHandler( CDPMng<CBuffer>* pDPMng, DPID dpidFrom );
	CBuffer*	Fetch( CBuffer** ppRecvBuffer, DWORD dwBytes, CRC32* pcrc );
	virtual	void	AddSysMsg( void* pMsg, u_long uMsgSize );
	static	u_long	GetHdrSize( BOOL bcrc )
		{	if( bcrc )
				return 13;
			return 5;	// sizeof(char) + sizeof(DWORD)
		}
	virtual	void	AddHdr( LPBYTE lpData, u_long uDataSize );
	virtual	void	AddData( LPBYTE lpData, u_long uDataSize, u_long & uRemnant );
	
public:
static	\
CHeapMng*	m_pHeapMng;

static	\
BOOL	IsHeader( BYTE cHeader );

#ifndef __VM_0820
#ifndef __MEM_TRACE
static	\
//CMemPool<CBuffer>*	m_pPool;
MemPooler<CBuffer>*	m_pPool;
void*	operator new( size_t nSize )	{	return CBuffer::m_pPool->Alloc();	}
void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CBuffer::m_pPool->Alloc();	}
void	operator delete( void* lpMem )	{	CBuffer::m_pPool->Free( (CBuffer*)lpMem );	}
void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CBuffer::m_pPool->Free( (CBuffer*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820
};
inline int CBuffer::GetSize( void )
	{	return (int)( m_lpBufMax - m_lpBufStart );	}
inline LPBYTE CBuffer::GetWritableBuffer( int* pnBufSize )
	{	*pnBufSize	= (int)( m_lpBufMax - m_pTail );	return m_pTail;	}
inline int CBuffer::GetWritableBufferSize( void )
	{	return (int)( m_lpBufMax - m_pTail );	}
inline LPBYTE CBuffer::GetReadableBuffer( int* pnBufSize )
	{	*pnBufSize	= (int)( m_pTail - m_pHead );	return m_pHead;	}
inline int CBuffer::GetReadableBufferSize( void )
	{	return (int)( m_pTail - m_pHead );	}
inline BOOL CBuffer::IsHeader( BYTE cHeader )
	{	return ( cHeader == HEADERMARK || cHeader == SYSHEADERMARK );	}

/*--------------------------------------------------------------------------------*/
class CBuffer2 : public CBuffer
{
public:
	CBuffer2( u_long uBufSize = MAX_BUFFER ) : CBuffer( uBufSize )	{}
	virtual	~CBuffer2()	{}

	void	MessageHandler( CDPMng<CBuffer2>* pDPMng, DPID dpidFrom );
	CBuffer2*	Fetch( CBuffer2** ppRecvBuffer, DWORD dwBytes, CRC32* pcrc  );
	virtual	void	AddSysMsg( void* pMsg, u_long uMsgSize );
	static	u_long	GetHdrSize( BOOL bcrc )
		{	if( bcrc )
				return 10;
			return 2;
		}
	virtual	void	AddHdr( LPBYTE lpData, u_long uDataSize );

public:

static	\
CHeapMng*	m_pHeapMng;

#ifndef __VM_0820
#ifndef __MEM_TRACE
static	\
//CMemPool<CBuffer>*	m_pPool;
MemPooler<CBuffer2>*	m_pPool2;
void*	operator new( size_t nSize )	{	return CBuffer2::m_pPool2->Alloc();	}
void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CBuffer2::m_pPool2->Alloc();	}
void	operator delete( void* lpMem )	{	CBuffer2::m_pPool2->Free( (CBuffer2*)lpMem );	}
void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CBuffer2::m_pPool2->Free( (CBuffer2*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820
};
/*--------------------------------------------------------------------------------*/

class CBuffer3 : public CBuffer
{
public:
	CBuffer3( u_long uBufSize = MAX_BUFFER ) : CBuffer( uBufSize )	{}
	virtual	~CBuffer3()	{}

	void	MessageHandler( CDPMng<CBuffer3>* pDPMng, DPID dpidFrom );
	CBuffer3*	Fetch( CBuffer3** ppRecvBuffer, DWORD dwBytes, CRC32* pcrc  );
	virtual	void	AddSysMsg( void* pMsg, u_long uMsgSize );
	static	u_long	GetHdrSize( BOOL bcrc )
		{	if( bcrc )
				return 10;
			return 2;
		}
	virtual	void	AddHdr( LPBYTE lpData, u_long uDataSize );

public:

static	\
CHeapMng*	m_pHeapMng;

#ifndef __VM_0820
#ifndef __MEM_TRACE
static	\
//CMemPool<CBuffer>*	m_pPool;
MemPooler<CBuffer3>*	m_pPool2;
void*	operator new( size_t nSize )	{	return CBuffer3::m_pPool2->Alloc();	}
void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CBuffer3::m_pPool2->Alloc();	}
void	operator delete( void* lpMem )	{	CBuffer3::m_pPool2->Free( (CBuffer3*)lpMem );	}
void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CBuffer3::m_pPool2->Free( (CBuffer3*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820
};
/*--------------------------------------------------------------------------------*/

template <class T>
class CBufferQueue
{
private:
	T*	m_pHead;
	T*	m_pTail;
	u_long	m_uCount;
public:
	CMclCritSec		m_cs;
	CRC32*	m_pcrc;
public:
//	Constructions
	CBufferQueue();
	virtual	~CBufferQueue();
//	Operations
	void	AddTail( T* pBuffer );
	void	RemoveHead( void );
	T*	GetHead( void );
	T*	GetTail( void );
	void	Clear( BOOL bDelete );
	BOOL	IsEmpty( void );
	u_long	GetCount( void );
	void	AddData( LPBYTE lpData, u_long uDataSize );
	void	GetData( LPWSABUF lpBuffers, LPDWORD lpdwBufferCount );
	void	RemoveData( u_long uDataSize );
	void	crc( CRC32* pcrc )	{	m_pcrc	= pcrc;		}
};

template <class T>
inline T* CBufferQueue<T>::GetHead( void )
	{	return m_pHead;	}

template <class T>
inline T* CBufferQueue<T>::GetTail( void )
	{	return m_pTail;	}

template <class T>
inline u_long CBufferQueue<T>::GetCount( void )
	{	return m_uCount;	}

template <class T>
inline BOOL CBufferQueue<T>::IsEmpty( void )
	{	return( m_uCount == 0 );	}

template <class T>
inline void CBufferQueue<T>::AddTail( T* pBuffer )
	{
		CMclAutoLock	Lock( m_cs );
	//	Add it to the active list	
		if( !m_pHead )
			m_pHead		= pBuffer;
		if( m_pTail )
			m_pTail->pNext	= pBuffer;
		pBuffer->pPrevious	= m_pTail;
		pBuffer->pNext	= NULL;
		m_pTail		= pBuffer;
		m_uCount++;
	}

template <class T>
inline void CBufferQueue<T>::RemoveHead( void )
	{
	//	CMclAutoLock	Lock( m_cs );
	//	Remove it from active list
		if( !IsEmpty() )
		{
			if( m_pTail == m_pHead )
				m_pTail	= NULL;
			if( m_pHead->pNext )
				m_pHead->pNext->pPrevious	= NULL;
			m_pHead		= (T*)m_pHead->pNext;
		}
		m_uCount--;
	}

template <class T>
CBufferQueue<T>::CBufferQueue()
	{
		m_uCount	= 0;
		m_pHead		= m_pTail	= NULL;
		m_pcrc	= NULL;
	}

template <class T>
CBufferQueue<T>::~CBufferQueue()
	{
		Clear( TRUE );
	}

template <class T>
void CBufferQueue<T>::Clear( BOOL bDelete )
	{
		CMclAutoLock	Lock( m_cs );

		if( bDelete )
		{
			T* pBuffer	= GetHead(), *ptrtmp;
			while( pBuffer )
			{
				ptrtmp	= (T*)pBuffer->pNext;
				SAFE_DELETE( pBuffer );
				pBuffer	= ptrtmp;
			}
		}
		m_uCount	= 0;
		m_pHead	= m_pTail	= NULL;
	}

template <class T>
void CBufferQueue<T>::AddData( LPBYTE lpData, u_long uDataSize )
	{
	//	CMclAutoLock	Lock( m_cs );
		u_long	uRemnant	= uDataSize;
		T* pBuffer	= GetTail();
	//	LPBYTE ptr;
		DWORD cb;

		if( pBuffer )
		{
			cb	= pBuffer->GetWritableBufferSize();
			if( cb < T::GetHdrSize( (BOOL)m_pcrc ) )
			{
				pBuffer	= new T;
				pBuffer->crc( m_pcrc );

				AddTail( pBuffer );
			}
		}
		else
		{
			pBuffer	= new T;
			pBuffer->crc( m_pcrc );

			AddTail( pBuffer );
		}
	/*
		ptr	= pBuffer->GetWritableBuffer( &cb );
	//	header
		*ptr	= HEADERMARK;
	//	ptr++;
	//	*(UNALIGNED DWORD*)ptr	= (DWORD)uDataSize;
	//	ptr	+= sizeof(DWORD);
	//	cb	= ( cb-HEADERSIZE < uRemnant? cb-HEADERSIZE: uRemnant );
	////	data
	//	ASSERT( ptr + cb <= pBuffer->m_lpBufMax );
	//	memcpy( (void*)ptr, lpData, cb );
	//	pBuffer->m_pTail	= ptr + cb;
	//	uRemnant	-= cb;

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
	*/
		pBuffer->AddHdr( lpData, uDataSize );
		pBuffer->AddData( lpData, uDataSize, uRemnant );
		while( uRemnant > 0 )
		{
			pBuffer	= new T;
			pBuffer->crc( m_pcrc );

			pBuffer->AddData( lpData, uDataSize, uRemnant );
			AddTail( pBuffer );
		}
	/*
	//	data
		cb	= ( cb-m_dwWriteHeaderSize < uRemnant? cb-m_dwWriteHeaderSize: uRemnant );
		ASSERT( &ptr[m_dwWriteHeaderSize] + cb <= pBuffer->m_lpBufMax );
		memcpy( (void*)&ptr[m_dwWriteHeaderSize], lpData, cb );
		pBuffer->m_pTail	= ptr + m_dwWriteHeaderSize + cb;

		uRemnant	-= cb;

		while( uRemnant > 0 )
		{
			pBuffer	= new CBuffer;
			ptr	= pBuffer->GetWritableBuffer( &cb );
			cb	= ( cb < uRemnant? cb: uRemnant );
			ASSERT( ptr + cb <= pBuffer->m_lpBufMax );
			memcpy( (void*)ptr, &lpData[uDataSize - uRemnant], cb );
			pBuffer->m_pTail	= ptr + cb;
			AddTail( pBuffer );
			uRemnant	-= cb;
		}
	*/
	}

template <class T>
void CBufferQueue<T>::RemoveData( u_long uDataSize )
	{
		T* pBuffer;
		
	//	CMclAutoLock	Lock( m_cs );
		u_long uRemnant;
		while( uDataSize > 0 )
		{
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

template <class T>
void CBufferQueue<T>::GetData( LPWSABUF lpBuffers, LPDWORD lpdwBufferCount )
	{
		T* pBuffer;
		int	i	= 0;
	//	*lpdwBufferCount	= 0;
	//	CMclAutoLock	Lock( m_cs );	
		pBuffer	= GetHead();
		while( pBuffer && i < MAX_WSABUF )
		{
			lpBuffers[i].buf	= (char*)pBuffer->GetReadableBuffer( (int*)&lpBuffers[i].len );
			i++;
			pBuffer	= (T*)pBuffer->pNext;
		}
		*lpdwBufferCount	= i;
	}

#endif	// __BUFFER_H__