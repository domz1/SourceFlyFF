#ifndef __MEM_TRACE_H
#define	__MEM_TRACE_H

#pragma warning( disable:4001 )	// nonstandard extension 'single line comment' was used
#pragma warning( disable:4100 )	// unreferenced formal parameter
#pragma warning( disable:4699 )	// Note: Creating precompiled header 
#pragma warning( disable:4710 )	// function not inlined
#pragma warning( disable:4514 )	// unreferenced inline function has been removed
#pragma warning( disable:4512 )	// assignment operator could not be generated
#pragma warning( disable:4310 )	// cast truncates constant value
#pragma warning( disable:4786 )	// identifier was truncated to '255' characters in the browser information

#include <crtdbg.h>
#include <string>
#include <map>
#include "cmcl.h"

using	namespace	std;

typedef struct	_AllocInfo
{
	size_t	nSize;
	int	nAlloc;
	_AllocInfo( size_t nSize ) : nSize( nSize ), nAlloc( 0 )	{}
}	AllocInfo;

typedef map<string, AllocInfo>	MSA;
typedef	map<void*, AllocInfo*>	MPA;

class CMemTrace
{
private:
	CMemTrace();
public:
	~CMemTrace();
	static	CMemTrace*	Instance();
private:
#if 0	//
	static	int __cdecl	DefautAllocHook( int, void *, size_t, int, long, const unsigned char *, int );
	static	int __cdecl	MyAllocHook( int, void *, size_t, int, long, const unsigned char *, int );
#endif	// 0
public:
#if 0
	void	Initialize();
#endif	// 0
	void	Alloc( const char* szFile, int nLine, void* ptr, size_t nSize );
	void	Free( void* ptr );
	void	Print();
private:
	MSA	m_mapAlloc;
	MPA	m_mapFree;
	CMclCritSec	m_Access;
};

#ifndef _DEBUG
void*	operator new( size_t nSize, const char* szFile, int nLine );
void	operator delete( void* ptr, const char* szFile, int nLine );
#endif	// _DEBUG

#define	MT_NEW	new(__FILE__, __LINE__ )

#ifdef __MEM_TRACE
	#ifndef _DEBUG
		#define	MT_FREE( p )	CMemTrace::Instance()->Free( (p) )
	#else	// _DEBUG
		#define	MT_FREE( p )	((void)0)
	#endif	// _DEBUG
#else	// __MEM_TRACE
		#define	MT_FREE( p )	((void)0)
#endif	// __MEM_TRACE

#define	safe_delete( p )	\
{	\
	MT_FREE(p);	\
	delete (p);	\
}

#define	safe_delete_array( p )	\
{	\
	MT_FREE(p);	\
	delete[] (p);	\
}

#endif	// __MEM_TRACE_H