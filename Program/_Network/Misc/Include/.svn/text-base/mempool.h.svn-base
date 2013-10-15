#ifndef __MEMPOOL_H__
#define __MEMPOOL_H__

#pragma once

#include <stack>
#include <list>
using namespace std;

template <class T> class CMemPool
{
private:
	stack<T*>	m_stackData;
	list<T*>	m_listBlock;
	size_t	m_nGrowSize;
	CMclCritSec		m_cs;

public:
//	Constructions
	CMemPool( size_t nGrowSize );
	virtual	~CMemPool();
//	Operations
//	T*	Alloc( size_t nSize = 0, LPCSTR lpszFileName = NULL, int nLine = 0 );
	T*	Alloc( void );
	void	Free( T* ptr );

private:
	void	Grow( void );
};

template <class T> CMemPool<T>::CMemPool( size_t nGrowSize )
{
	m_nGrowSize		= nGrowSize;
	Grow();
}

template <class T> CMemPool<T>::~CMemPool()
{
	CMclAutoLock Lock( m_cs );

	for( list<T*>::iterator i = m_listBlock.begin(); i != m_listBlock.end(); ++i )
		free( *i );
	m_listBlock.clear();

	while( !m_stackData.empty() )
		m_stackData.pop();
}

template <class T> inline void CMemPool<T>::Grow( void )
{
	T* pBlock	= (T*)malloc( sizeof(T) * m_nGrowSize );

	m_listBlock.push_back( pBlock );
	for( DWORD i = 0; i < m_nGrowSize; i++ )
		m_stackData.push( &pBlock[i] );
}

//template <class T> inline T* CMemPool<T>::Alloc( size_t nSize, LPCSTR lpszFileName, int nLine )
template <class T> inline T* CMemPool<T>::Alloc( void )
{
	CMclAutoLock Lock( m_cs );
	
	if( m_stackData.empty() )
		Grow();

	T* lpMem	= m_stackData.top();
	m_stackData.pop();

	return lpMem;
}

template <class T> inline void CMemPool<T>::Free( T* lpMem )
{
	CMclAutoLock Lock( m_cs );
	m_stackData.push( lpMem );
}

#endif	// __MEMPOOL_H__