#ifndef __BUFFER_H__
#define	__BUFFER_H__

#pragma once

#include "mempooler.h"
#include "heapmng.h"
#include <dplay.h>

#ifdef __CRC
	#include "crc.h"
	using	namespace	std;
	#define	HEADERSIZE5		( sizeof(DWORD) + 1 )
#if defined( __PACKET_REPLAY_R ) || defined( __PACKET_REPLAY_W )
	#define	HEADERSIZE13	( sizeof(char) + sizeof(int) + sizeof(DWORD) + sizeof(DWORD) + sizeof(DWORD) )
#else	// __PACKET_REPLAY_R || __PACKET_REPLAY_W
	#define	HEADERSIZE13	( sizeof(char) + sizeof(DWORD) + sizeof(DWORD) + sizeof(DWORD) )
#endif	// __PACKET_REPLAY_R || __PACKET_REPLAY_W
#endif	// __CRC

#define	HEADERMARK		'^'		// Normal message header
#define	SYSHEADERMARK	'%'		// System message header
#define	MAX_BUFFER		8192
#define	MAX_WSABUF		128

enum BUFFER_TYPE 
{
	BUFFER_TYPE_5BYTE,		// 기존 1 + 4바이트 헤더 방식 
	BUFFER_TYPE_2BYTE,		// 2바이트 헤더 방식 
};



///////////////////////////////////////////////////////////////////////
// CBuffer
///////////////////////////////////////////////////////////////////////

class CBuffer
{
public:
	DPID		dpid;
	u_long		cb;
	BYTE		m_lpBuf[MAX_BUFFER];
	LPBYTE		m_lpBufStart;
	LPBYTE		m_lpBufMax;
	LPBYTE		m_pHead;
	LPBYTE		m_pTail;
	CBuffer*	pNext;
	CBuffer*	pPrevious;
	DWORD		m_dwHeaderSize;

public:
//	Constructions
	CBuffer( u_long uBufSize );
	virtual	~CBuffer();

//	Operations
	int		GetSize( void );
	LPBYTE	GetWritableBuffer( int* pnBufSize );
	int		GetWritableBufferSize( void );
	LPBYTE	GetReadableBuffer( int* pnBufSize );
	int		GetReadableBufferSize( void );
	
	DWORD	GetHeaderLength()
	{
		return m_dwHeaderSize;
	}

	virtual DWORD	GetPacketSize( BYTE* ptr ) 
	{
		return *(UNALIGNED LPDWORD)(ptr + 1);
	}

	virtual void SetHeader( DWORD uDataSize )
	{
		*m_pTail					= HEADERMARK;
		m_pTail++;

		*(UNALIGNED DWORD*)m_pTail	= (DWORD)uDataSize;
		m_pTail += sizeof(DWORD);
	}

public:
	static	CHeapMng*	m_pHeapMng;
#ifndef __VM_0820
#ifndef __MEM_TRACE
	static	MemPooler<CBuffer>*	m_pPool;
	void*	operator new( size_t nSize )	{	return CBuffer::m_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CBuffer::m_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CBuffer::m_pPool->Free( (CBuffer*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CBuffer::m_pPool->Free( (CBuffer*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820
};


inline int CBuffer::GetSize( void )
{	
	return (int)( m_lpBufMax - m_lpBufStart );	
}

inline LPBYTE CBuffer::GetWritableBuffer( int* pnBufSize )
{	
	*pnBufSize	= (int)( m_lpBufMax - m_pTail );	
	return m_pTail;	
}

inline int CBuffer::GetWritableBufferSize( void )
{	
	return (int)( m_lpBufMax - m_pTail );	
}

inline LPBYTE CBuffer::GetReadableBuffer( int* pnBufSize )
{	
	*pnBufSize	= (int)( m_pTail - m_pHead );	
	return m_pHead;	
}

inline int CBuffer::GetReadableBufferSize( void )
{	
	return (int)( m_pTail - m_pHead );	
}

///////////////////////////////////////////////////////////////////////
// CBuffer2
///////////////////////////////////////////////////////////////////////

class CBuffer2 : public CBuffer
{
public:
	CBuffer2( u_long uBufSize ) : CBuffer( uBufSize ) 
	{
		m_dwHeaderSize = 2;
	}

	virtual DWORD	GetPacketSize( BYTE* ptr ) 
	{
//		DWORD dwSize = *(WORD *)ptr;
//		dwSize -= 2;
//		return dwSize;
		DWORD dwSize = ntohs( *(WORD *)ptr );
		dwSize -= 2;
		return dwSize;
	}

	virtual void SetHeader( DWORD uDataSize )
	{
//		*(UNALIGNED WORD*)m_pTail	= (WORD)(uDataSize + 2);
//		m_pTail += sizeof(WORD);
		*(UNALIGNED WORD*)m_pTail	= htons( (WORD)(uDataSize + 2) );
		m_pTail += sizeof(WORD);
	}

#ifndef __VM_0820
#ifndef __MEM_TRACE
	static	MemPooler<CBuffer2>*	m_pPool2;
	void*	operator new( size_t nSize )	{	return CBuffer2::m_pPool2->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CBuffer2::m_pPool2->Alloc();	}
	void	operator delete( void* lpMem )	{	CBuffer2::m_pPool2->Free( (CBuffer2*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CBuffer2::m_pPool2->Free( (CBuffer2*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820
};

///////////////////////////////////////////////////////////////////////
// CBufferQueue
///////////////////////////////////////////////////////////////////////

class CBufferQueue
{
private:
	CBuffer*	m_pHead;
	CBuffer*	m_pTail;
	u_long	m_uCount;
public:
	CMclCritSec		m_cs;
#ifdef __CRC
	CRC32*	m_pcrc;
	DWORD	m_dwWriteHeaderSize;
#endif	// __CRC

#ifdef __PROTOCOL0910
	DWORD*	m_pdwProtocolId;
#endif	// __PROTOCOL0910

public:
//	Constructions
	CBufferQueue();
	virtual	~CBufferQueue();
//	Operations
	void	AddTail( CBuffer* pBuffer );
	void	RemoveHead( void );
	CBuffer*	GetHead( void );
	CBuffer*	GetTail( void );
	void	Clear( BOOL bDelete );
	BOOL	IsEmpty( void );
	u_long	GetCount( void );
	void	AddData( LPBYTE lpData, u_long uDataSize, BUFFER_TYPE type );
	void	GetData( LPWSABUF lpBuffers, LPDWORD lpdwBufferCount );
	void	RemoveData( u_long uDataSize );
#ifdef __CRC
#ifdef __PROTOCOL0910
	void	crc( CRC32* pcrc, LPDWORD pdwProtocolId )	{	m_pcrc	= pcrc;		m_pdwProtocolId	= pdwProtocolId;	m_dwWriteHeaderSize		= HEADERSIZE13;	}
#else	// __PROTOCOL0910
	void	crc( CRC32* pcrc )	{	m_pcrc	= pcrc;		m_dwWriteHeaderSize		= HEADERSIZE13;	}
#endif	// __PROTOCOL0910
#endif	// __CRC

#ifdef __PACKET_REPLAY_W
private:
	int		m_nPacket;
#endif	// __PACKET_REPLAY_W
};

inline CBuffer* CBufferQueue::GetHead( void )
	{	return m_pHead;	}
inline CBuffer* CBufferQueue::GetTail( void )
	{	return m_pTail;	}
inline u_long CBufferQueue::GetCount( void )
	{	return m_uCount;	}
inline BOOL CBufferQueue::IsEmpty( void )
	{	return( m_uCount == 0 );	}
inline void CBufferQueue::AddTail( CBuffer* pBuffer )
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
inline void CBufferQueue::RemoveHead( void )
{
//	CMclAutoLock	Lock( m_cs );
//	Remove it from active list
	if( !IsEmpty() )
	{
		if( m_pTail == m_pHead )
			m_pTail	= NULL;
		if( m_pHead->pNext )
			m_pHead->pNext->pPrevious	= NULL;
		m_pHead		= m_pHead->pNext;
	}
	m_uCount--;
}


///////////////////////////////////////////////////////////////////////
// CBufferFactory
///////////////////////////////////////////////////////////////////////

class CBufferFactory
{
public:
	CBuffer* CreateBuffer( BUFFER_TYPE type, u_long uBufSize = MAX_BUFFER );
	static CBufferFactory& GetInstance();

private:
	CBufferFactory();
};

#endif	// __BUFFER_H__