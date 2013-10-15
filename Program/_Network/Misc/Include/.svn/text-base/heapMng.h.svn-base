#ifndef __HEAPMNG_H__
#define __HEAPMNG_H__

#pragma once

class CHeapMng
{
public:
	virtual	~CHeapMng();
	void*	Malloc( size_t nSize );
#ifdef __VM_0819
	CHeapMng( const char* szHeap );
	void	Free( LPVOID lpMem, size_t nSize );
	void*	Realloc( LPVOID lpMem, size_t nSize, size_t nOldSize );
	void	Log();
#else	// __VM_0819
	CHeapMng ();
	void	Free( LPVOID lpMem );
	void*	Realloc( LPVOID lpMem, size_t nSize );
#endif	// __VM_0819
private:
	HANDLE	m_hHeap;
#ifdef __VM_0819
	CRITICAL_SECTION	m_cs;
	char	m_szHeap[100];
	int	m_nAlloc;
	size_t	m_nTotal;
#endif	// __VM_0819
};
#endif //__HEAPMNG_H__