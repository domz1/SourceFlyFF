#include "stdafx.h"
#ifdef __MEM_TRACE
#include "memtrace.h"
#include <new.h>
#include <malloc.h>

CMemTrace::CMemTrace()
{
}

CMemTrace::~CMemTrace()
{
//{{AFX
#if 0
	_CrtSetAllocHook( CMemTrace::DefautAllocHook );
#endif	// 0
//}}AFX
}

CMemTrace*	CMemTrace::Instance()
{
	static CMemTrace sMemTrace;
	return &sMemTrace;
}

//{{AFX
#if 0	// 
int __cdecl CMemTrace::DefautAllocHook( int, void *, size_t, int, long, const unsigned char *, int )
{
	return 1;
}

int __cdecl CMemTrace::MyAllocHook(
		int nAllocType,
        void * pvData,
        size_t nSize,
        int nBlockUse,
        long lRequest,
        const unsigned char * szFileName,
        int nLine
		)
{
	if( nAllocType == _HOOK_FREE )
	{
		_CRT_ALLOC_HOOK pfnOldHook	= _CrtSetAllocHook( CMemTrace::DefautAllocHook );
		CMemTrace::Instance()->Free( pvData );
		_CrtSetAllocHook( pfnOldHook );
	}
	return 1;
}

void CMemTrace::Initialize()
{
	_CrtSetAllocHook( CMemTrace::MyAllocHook );
}
#endif	// 0
//}}AFX

void CMemTrace::Alloc( const char *szFile, int nLine, void * ptr, size_t nSize )
{
	char szDrive[_MAX_DRIVE];
	char szDir[_MAX_DIR];
	char szName[_MAX_FNAME];
	char szExt[_MAX_EXT];
	_splitpath( szFile, szDrive, szDir, szName, szExt );
	char pKey[MAX_PATH]	= { 0};
	sprintf( pKey, "%s%s%d", szName, szExt, nLine );

	CMclAutoLock	lock( m_Access );

	MSA::iterator i	= m_mapAlloc.find( pKey );
	if( i == m_mapAlloc.end() )
		i	= m_mapAlloc.insert( MSA::value_type( pKey, AllocInfo( nSize ) ) ).first;
	else
		i->second.nSize	+= nSize;
	i->second.nAlloc++;
	m_mapFree.insert( MPA::value_type( ptr, &i->second ) );
}

void CMemTrace::Free( void * ptr )
{
	CMclAutoLock	lock( m_Access );

	MPA::iterator i	= m_mapFree.find( ptr );
	if( i == m_mapFree.end() )
		i	= m_mapFree.find( (void*)( (int)ptr - 4 ) );

	if( i != m_mapFree.end() )
	{
		AllocInfo* ptr	= i->second;
		
		if( i->second->nAlloc > 0 )
			i->second->nAlloc--;

		if( i->second->nAlloc == 0 )
			i->second->nSize	= 0;
		else
			i->second->nSize	-= ( i->second->nSize / i->second->nAlloc );

		m_mapFree.erase( i );
	}
}

void CMemTrace::Print()
{
	CMclAutoLock	lock( m_Access );

	int nTotal	= 0;
	for( MSA::iterator i = m_mapAlloc.begin(); i != m_mapAlloc.end(); ++i )
		nTotal	+= i->second.nSize;

	FILE* f	= fopen( "../memtrace.txt", "a" );
	if( f == NULL )
		return;

	SYSTEMTIME	time;
	GetLocalTime( &time );
	fprintf( f, "\n%d/%02d/%02d\t%02d:%02d:%02d\n", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond );
	
	for( i = m_mapAlloc.begin(); i != m_mapAlloc.end(); ++i )
	{
		if( i->second.nAlloc == 0 )
			continue;
		fprintf( f, "%3.2f%%, %s:%d:%d\n", (float)( i->second.nSize ) / (float)nTotal, i->first.c_str(), i->second.nSize /  i->second.nAlloc, i->second.nAlloc );
	}

	fclose( f );
}

#ifndef _DEBUG
#undef new
void* operator new( size_t nSize, const char* szFile, int nLine )
{
	void* ptr	= ::operator new( nSize );
	CMemTrace::Instance()->Alloc( szFile, nLine, ptr, nSize );
	return ptr;
}

void	operator delete( void* ptr, const char* szFile, int nLine )
{
	::operator delete( ptr );
}
#define	new	MT_NEW
#endif	// _DEBUG

#endif	// __MEM_TRACE
