#ifndef __MYMAP_H__
#define __MYMAP_H__

#pragma once

#include <stack>
#include <list>
using namespace std;

#define INVALID_KEY	(DWORD)0xFFFFFFFF



#ifdef	__VERIFY_LOOP041010
	#undef INIT_LOOP
	#undef VERIFY_LOOP

	#define	INIT_LOOP(n)				int __loop = (n);
	#define VERIFY_LOOP(file, line)		if( --__loop == 0 ) { FILEOUT( "..\\mymap.txt", "%s, %d", file, line ); }	
#else
	#undef INIT_LOOP
	#undef VERIFY_LOOP

	#define	INIT_LOOP(n)				((void)0);
	#define VERIFY_LOOP(file, line)		((void)0);
#endif

/*--------------------------------------------------------------------------------*/
#ifndef __STL_0402

template <class T> class CMyBucket
{
public:
	DWORD	m_dwKey;
	T	m_value;
	CMyBucket<T>*	m_pNext;	// map
	CMyBucket<T>*	pPrevious;	// list
	CMyBucket<T>*	pNext;
	
public:
//	Constructions
	CMyBucket();
	virtual	~CMyBucket();
		
//	Operations
	void	Empty( void );
	BOOL	IsEqual( DWORD dwKey );
	BOOL	IsEmpty( void );
};

template <class T> CMyBucket<T>::CMyBucket()
{
	m_dwKey		= INVALID_KEY;
	m_pNext		= NULL;	// map
	pPrevious	= pNext		= NULL;	// list
}

template <class T> CMyBucket<T>::~CMyBucket()
{

}

template <class T> inline void CMyBucket<T>::Empty( void )
{
	m_dwKey		= INVALID_KEY;
}

template <class T> inline BOOL CMyBucket<T>::IsEqual( DWORD dwKey )
{
	return( dwKey == m_dwKey );
}

template <class T> inline BOOL CMyBucket<T>::IsEmpty( void )
{
	return( m_dwKey == INVALID_KEY );
}

/*--------------------------------------------------------------------------------*/
template <class T> class CMyBucket2
{
public:
	char	m_pKey[64];
	size_t	m_nSize;
	T	m_value;
	CMyBucket2<T>*	m_pNext;	// map
	CMyBucket2<T>*	pPrevious;	// list
	CMyBucket2<T>*	pNext;

public:
//	Constructions
	CMyBucket2();
	virtual	~CMyBucket2();
		
//	Operations
	void	Empty( void );
	BOOL	IsEqual( const char* pKey, size_t nSize );
	BOOL	IsEmpty( void );
};

template <class T> CMyBucket2<T>::CMyBucket2()
{
	m_nSize	= 0;
	m_pNext		= NULL;
	pPrevious	= pNext		= NULL;
}

template <class T> CMyBucket2<T>::~CMyBucket2()
{

}

template <class T> inline void CMyBucket2<T>::Empty( void )
{
	m_nSize		= 0;
}

template <class T> inline BOOL CMyBucket2<T>::IsEqual( const char *pKey, size_t nSize )
{
	if( nSize == m_nSize )
		return !memcmp( pKey, m_pKey, nSize );
	return FALSE;
}

template <class T> inline BOOL CMyBucket2<T>::IsEmpty( void )
{
	return( !m_nSize );
}

/*--------------------------------------------------------------------------------*/
template <class T> class CMyPool
{
private:
	stack<T*>	m_stackData;
	list<T*>	m_listBlock;
	size_t	m_nGrowSize;
	size_t	m_nInitialSize;

public:
//	Constructions
	CMyPool();	
	CMyPool( size_t nInitialSize, size_t nGrowSize );
	virtual	~CMyPool();
//	Operations
	void	Clear( void );
	void	SetSize( size_t nInitialSize, size_t nGrowSize );
	T*	Pop( void );
	void	Push( T* lpMem );
	size_t	GetInitialSize( void );
	size_t	GetGrowSize( void );
private:
//	Operations
	void	Grow( size_t nGrowSize );
};

template <class T> CMyPool<T>::CMyPool()
{
	m_nInitialSize	= m_nGrowSize	= 0;
}

template <class T> CMyPool<T>::CMyPool( size_t nInitialSize, size_t nGrowSize )
{
	SetSize( nInitialSize, nGrowSize );
}

template <class T> CMyPool<T>::~CMyPool()
{
	Clear();
}

template <class T> void CMyPool<T>::Clear( void )
{
	INIT_LOOP(1000);
	for( list<T*>::iterator i = m_listBlock.begin(); i != m_listBlock.end(); ++i )
	{
		VERIFY_LOOP( __FILE__, __LINE__ );
		safe_delete_array( *i );
	}

	m_listBlock.clear();

	{
		INIT_LOOP(1000);
		while( !m_stackData.empty() )
		{
			VERIFY_LOOP( __FILE__, __LINE__ );
			m_stackData.pop();
		}
	}

	m_nInitialSize	= m_nGrowSize	= 0;
}

template <class T> void CMyPool<T>::SetSize( size_t nInitialSize, size_t nGrowSize )
{
	ASSERT( nInitialSize );
	ASSERT( nGrowSize );

	Clear();

	Grow( m_nInitialSize = nInitialSize );
	m_nGrowSize		= nGrowSize;
}

template <class T> inline T* CMyPool<T>::Pop( void )
{
	ASSERT( m_nGrowSize > 0 );

	if( m_stackData.empty() )
		Grow( m_nGrowSize );

	T* lpMem	= m_stackData.top();
	m_stackData.pop();

	return lpMem;
}

template <class T> inline void CMyPool<T>::Push( T* lpMem )
{
	ASSERT( m_nGrowSize > 0 );
	m_stackData.push( lpMem );
}

template <class T> inline size_t CMyPool<T>::GetInitialSize( void )
{
	return m_nInitialSize;
}

template <class T> inline size_t CMyPool<T>::GetGrowSize( void )
{
	return m_nGrowSize;
}

template <class T> inline void CMyPool<T>::Grow( size_t nGrowSize )
{
	ASSERT( nGrowSize > 0 );
	T* pBlock	= new T[nGrowSize];
	m_listBlock.push_back( pBlock );

	INIT_LOOP(5000);
	for( size_t i = 0; i < nGrowSize; i++ )
	{
		VERIFY_LOOP( __FILE__, __LINE__ );
		m_stackData.push( &pBlock[i] );
	}
}

/*--------------------------------------------------------------------------------*/
template <class T> class CMyMap
{
private:
	CMyBucket<T>*	m_pBuckets;
	CMyPool< CMyBucket<T> >		m_pool;
	CMyBucket<T>*	m_pFirstActive;	// list
	size_t	m_nHashSize;
	u_long	m_nTotal;
public:
	CMclCritSec		m_AddRemoveLock;

public:
//	Constructions
	CMyMap();
	CMyMap( size_t nHashSize, size_t nInitialSize, size_t nGrowSize );
	virtual	~CMyMap();
//	Operations
	int		GetCount( void );
	void	SetSize( size_t nHashSize, size_t nInitialSize, size_t nGrowSize = 1 );
	BOOL	Lookup( DWORD dwKey, T &value );
	T	operator[]( DWORD dwKey );
	void	SetAt( DWORD dwKey, const T & value );
	void	RemoveAll( void );
	BOOL	RemoveKey( DWORD dwKey );
	void	AddItToActiveList( CMyBucket<T>* pBucket );
	void	RemoveItFromActiveList( CMyBucket<T>* pBucket );
	CMyBucket<T>*	GetFirstActive( void );
	void	ClearActiveList( void );
	void	Lock( void );
	void	Unlock( void );
private:
//	Operations
};

template <class T> CMyMap<T>::CMyMap()
{
	m_pBuckets	= NULL;
	m_nHashSize		= 0;
	m_nTotal	= 0;
	m_pFirstActive	= NULL;
}

template <class T> CMyMap<T>::CMyMap( size_t nHashSize, size_t nInitialSize, size_t nGrowSize )
{
	SetSize( nHashSize, nInitialSize, nGrowSize );
}

template <class T> CMyMap<T>::~CMyMap()
{
	SAFE_DELETE_ARRAY( m_pBuckets );
}

template <class T> inline int CMyMap<T>::GetCount( void )
{
	return (int)m_nTotal;
}

template <class T> void CMyMap<T>::SetSize( size_t nHashSize, size_t nInitialSize, size_t nGrowSize )
{
	SAFE_DELETE_ARRAY( m_pBuckets );
	m_pFirstActive	= NULL;

	INIT_LOOP(10000);
	for( int i = (int)nHashSize; ; i++ )
	{
		VERIFY_LOOP( __FILE__, __LINE__ );
		if( IsPrime( i ) )
			break;
	}

	m_pBuckets	= new CMyBucket<T>[m_nHashSize=i];
	m_nTotal	= 0;
	m_pool.SetSize( nInitialSize, nGrowSize );
}

template <class T> inline BOOL CMyMap<T>::Lookup( DWORD dwKey, T & value )
{
	ASSERT( m_nHashSize > 0 );

	if( dwKey == INVALID_KEY )
		return FALSE;
	
	INIT_LOOP(1000);
	CMyBucket<T>* pBucket	= &m_pBuckets[IDHash( dwKey, m_nHashSize )];
	while( pBucket )
	{
		VERIFY_LOOP( __FILE__, __LINE__ );
		if( pBucket->IsEqual( dwKey ) )
		{
			value	= pBucket->m_value;
			return TRUE;
		}
		pBucket		= pBucket->m_pNext;
	}
	return FALSE;
}

template <class T> inline T CMyMap<T>::operator []( DWORD dwKey )
{
	T value;
	Lookup( dwKey, value );
	return value;
}

template <class T> inline void CMyMap<T>::SetAt( DWORD dwKey, const T & value )
{
	ASSERT( m_nHashSize > 0 );

	CMyBucket<T> *pBucket, *pBucketPrev;
	m_nTotal++;

	INIT_LOOP(1000);
	for( pBucket = &m_pBuckets[(int)IDHash( dwKey, m_nHashSize )]; pBucket != NULL; pBucket = pBucket->m_pNext )
	{
		VERIFY_LOOP( __FILE__, __LINE__ );
		if( pBucket->IsEmpty() )
		{
			pBucket->m_dwKey	= dwKey;
			pBucket->m_value	= value;
			AddItToActiveList( pBucket );
			return;
		}
		pBucketPrev		= pBucket;
	}

//	Collision
	pBucket		= pBucketPrev-> m_pNext		= m_pool.Pop();
	pBucket->m_dwKey	= dwKey;
	pBucket->m_value	= value;
	AddItToActiveList( pBucket );
}

template <class T> void CMyMap<T>::RemoveAll( void )
{
	m_pFirstActive	= NULL;

	CMyBucket<T>* pBucket, *pNext;
	INIT_LOOP(1000);
	for( int i = 0; i < (int)( m_nHashSize ); i++ )
	{
		VERIFY_LOOP( __FILE__, __LINE__ );
		pBucket		= &m_pBuckets[i];
		if( !pBucket->IsEmpty() )
			pBucket->Empty();

		int __loop2 = 1000;
		while( pBucket->m_pNext )
		{
			if( --__loop2 == 0 ) { FILEOUT( "mymap.txt", "%s, %d", __FILE__, __LINE__ ); }
			pNext	= pBucket->m_pNext;
			pBucket->m_pNext	= NULL;
			pBucket		= pNext;
			m_pool.Push( pBucket );
		}
	}
	m_nTotal	= 0;
}

template <class T> inline BOOL CMyMap<T>::RemoveKey( DWORD dwKey )
{
	ASSERT( m_nHashSize > 0 );

	CMyBucket<T> *pBucket = &m_pBuckets[(int)IDHash( dwKey, m_nHashSize )], *pBucketPrev;
	if( pBucket->IsEqual( dwKey ) )
	{
		pBucket->Empty();
		RemoveItFromActiveList( pBucket );
		m_nTotal--;
		return TRUE;
	}

	INIT_LOOP(1000);
	while( TRUE )
	{
		VERIFY_LOOP( __FILE__, __LINE__ );
		pBucketPrev		= pBucket;
		pBucket		= pBucket->m_pNext;
		if( !pBucket )
			return FALSE;

		if( pBucket->IsEqual( dwKey ) )
		{
			pBucketPrev->m_pNext	= pBucket->m_pNext;
			pBucket->m_pNext	= NULL;
			RemoveItFromActiveList( pBucket );
			m_pool.Push( pBucket );
			m_nTotal--;
			return TRUE;
		}
	}
	return FALSE;
}

template <class T> inline void CMyMap<T>::AddItToActiveList( CMyBucket<T>* pBucket )
{
	// Add it to the active list
	if( m_pFirstActive )
		m_pFirstActive->pPrevious	= pBucket;
	pBucket->pNext	= m_pFirstActive;
	pBucket->pPrevious	= NULL;
	m_pFirstActive	= pBucket;
}

template <class T> inline void CMyMap<T>::RemoveItFromActiveList( CMyBucket<T>* pBucket )
{
	// Remove it from active list
	if( pBucket->pPrevious )
		pBucket->pPrevious->pNext	= pBucket->pNext;
	if( pBucket->pNext )
		pBucket->pNext->pPrevious	= pBucket->pPrevious;
	if( m_pFirstActive == pBucket )
		m_pFirstActive	= pBucket->pNext;
}

template <class T> inline CMyBucket<T>* CMyMap<T>::GetFirstActive( void )
{
	return m_pFirstActive;
}

template <class T> inline void CMyMap<T>::ClearActiveList( void )
{
	m_pFirstActive	= NULL;
}

template <class T> inline void CMyMap<T>::Lock( void )
{
	m_AddRemoveLock.Enter();
}

template <class T> inline void CMyMap<T>::Unlock( void )
{
	m_AddRemoveLock.Leave();
}

/*--------------------------------------------------------------------------------*/
template <class T> class CMyMap2
{
private:
	CMyBucket2<T>*	m_pBuckets;
	CMyPool< CMyBucket2<T> >	m_pool;
	CMyBucket2<T>*	m_pFirstActive;	// list
	UINT	m_nHashSize;
	UINT	m_nTotal;
public:
	CMclCritSec		m_AddRemoveLock;

public:
//	Constructions
	CMyMap2();
	CMyMap2( size_t nHashSize, size_t nInitialSize, size_t nGrowSize );
	virtual	~CMyMap2();
//	Operations
	int		GetCount( void );
	void	SetSize( size_t nHashSize, size_t nInitialSize, size_t nGrowSize = 1 );
	BOOL	Lookup( const char* pKey, T & value );
	void	SetAt( const char* pKey, const T & value );
	void	RemoveAll( void );
	BOOL	RemoveKey( const char* pKey );
	void	AddItToActiveList( CMyBucket2<T>* pBucket );
	void	RemoveItFromActiveList( CMyBucket2<T>* pBucket );
	CMyBucket2<T>*	GetFirstActive( void );
	void	ClearActiveList( void );
	void	Lock( void );
	void	Unlock( void );

private:
//	Operations
	DWORD	MAKESTR2DWORD( const char* pKey, size_t nSize );
};

template <class T> CMyMap2<T>::CMyMap2()
{
	m_pBuckets	= NULL;
	m_nHashSize		= 0;
	m_nTotal	= 0;
	m_pFirstActive	= NULL;
}

template <class T> CMyMap2<T>::CMyMap2( size_t nHashSize, size_t nInitialSize, size_t nGrowSize )
{
	SetSize( nHashSize, nInitialSize, nGrowSize );
}

template <class T> CMyMap2<T>::~CMyMap2()
{
	SAFE_DELETE_ARRAY( m_pBuckets );
}

template <class T> inline int CMyMap2<T>::GetCount( void )
{
	return (int)m_nTotal;
}

template <class T> void CMyMap2<T>::SetSize( size_t nHashSize, size_t nInitialSize, size_t nGrowSize )
{
	SAFE_DELETE_ARRAY( m_pBuckets );
	m_pFirstActive	= NULL;
	
	INIT_LOOP(10000);
	for( int i = (int)nHashSize; ; i++ )
	{
		VERIFY_LOOP( __FILE__, __LINE__ );
		if( IsPrime( i ) )
			break;
	}

	m_pBuckets	= new CMyBucket2<T>[m_nHashSize=i];
	m_nTotal	= 0;
	m_pool.SetSize( nInitialSize, nGrowSize );
}

template <class T> inline BOOL CMyMap2<T>::Lookup( const char* pKey, T & value )
{
	ASSERT( m_nHashSize > 0 );

	if( pKey == NULL )
		return FALSE;

	size_t nSize	= strlen( pKey );

	CMyBucket2<T>* pBucket	= &m_pBuckets[(int)IDHash( MAKESTR2DWORD( pKey, nSize ), m_nHashSize )];
	INIT_LOOP(1000);
	while( pBucket )
	{
		VERIFY_LOOP( __FILE__, __LINE__ );
		if( pBucket->IsEqual( pKey, nSize ) )
		{
			value	= pBucket->m_value;
			return TRUE;
		}
		pBucket		= pBucket->m_pNext;
	}
	return FALSE;
}

template <class T> inline void CMyMap2<T>::SetAt( const char* pKey, const T & value )
{
	ASSERT( m_nHashSize > 0 );

	CMyBucket2<T> *pBucket, *pBucketPrev;
	size_t nSize	= strlen( pKey );
	m_nTotal++;

	INIT_LOOP(10000);
	for( pBucket = &m_pBuckets[(int)IDHash( MAKESTR2DWORD( pKey, nSize ), m_nHashSize )]; pBucket != NULL; pBucket = pBucket->m_pNext )
	{
		VERIFY_LOOP( __FILE__, __LINE__ );
		if( pBucket->IsEmpty() )
		{
			strcpy( pBucket->m_pKey, pKey );
			pBucket->m_nSize	= nSize;
			pBucket->m_value	= value;
			AddItToActiveList( pBucket );
			return;
		}
		pBucketPrev		= pBucket;
	}

	pBucket		= pBucketPrev->m_pNext	= m_pool.Pop();
	strcpy( pBucket->m_pKey, pKey );
	pBucket->m_nSize	= nSize;
	pBucket->m_value	= value;
	AddItToActiveList( pBucket );
}

template <class T> void CMyMap2<T>::RemoveAll( void )
{
	m_pFirstActive	= NULL;
	CMyBucket2<T> *pBucket, *pNext;

	INIT_LOOP(1000);
	for( int i = 0; i < m_nHashSize; i++ )
	{
		VERIFY_LOOP( __FILE__, __LINE__ );
		pBucket		= &m_pBuckets[i];
		
		if( !pBucket->IsEmpty() )
			pBucket->Empty();
		
		int __loop2 = 1000;
		while( pBucket->m_pNext )
		{
			if( --__loop2 == 0 ) { FILEOUT( "mymap.txt", "%s, %d", __FILE__, __LINE__ ); }
			pNext	= pBucket->m_pNext;
			pBucket->m_pNext	= NULL;
			pBucket		= pNext;
			m_pool.Push( pBucket );
		}
	}
	m_nTotal	= 0;
}

template <class T> inline BOOL CMyMap2<T>::RemoveKey( const char* pKey )
{
	ASSERT( m_nHashSize > 0 );

	size_t nSize	= strlen( pKey );
	CMyBucket2<T> *pBucket	= &m_pBuckets[(int)IDHash( MAKESTR2DWORD( pKey, nSize ), m_nHashSize )], *pBucketPrev;
	if( pBucket->IsEqual( pKey, nSize ) )
	{
		pBucket->Empty();
		RemoveItFromActiveList( pBucket );
		m_nTotal--;
		return TRUE;
	}

	INIT_LOOP(1000);
	while( TRUE )
	{
		VERIFY_LOOP( __FILE__, __LINE__ );
		pBucketPrev		= pBucket;
		pBucket		= pBucket->m_pNext;
		if( !pBucket )
			return FALSE;
		if( pBucket->IsEqual( pKey, nSize ) )
		{
			pBucketPrev->m_pNext	= pBucket->m_pNext;
			pBucket->m_pNext	= NULL;
			RemoveItFromActiveList( pBucket );
			m_pool.Push( pBucket );
			m_nTotal--;
			return TRUE;
		}
	}
	return FALSE;
}

template <class T> inline void CMyMap2<T>::AddItToActiveList( CMyBucket2<T>* pBucket )
{
	// Add it to the active list
	if( m_pFirstActive )
		m_pFirstActive->pPrevious	= pBucket;
	pBucket->pNext	= m_pFirstActive;
	pBucket->pPrevious	= NULL;
	m_pFirstActive	= pBucket;
}

template <class T> inline void CMyMap2<T>::RemoveItFromActiveList( CMyBucket2<T>* pBucket )
{
	// Remove it from active list
	if( pBucket->pPrevious )
		pBucket->pPrevious->pNext	= pBucket->pNext;
	if( pBucket->pNext )
		pBucket->pNext->pPrevious	= pBucket->pPrevious;
	if( m_pFirstActive == pBucket )
		m_pFirstActive	= pBucket->pNext;
}

template <class T> inline CMyBucket2<T>* CMyMap2<T>::GetFirstActive( void )
{
	return m_pFirstActive;
}

template <class T> inline void CMyMap2<T>::ClearActiveList( void )
{
	m_pFristActive	= NULL;
}

template <class T> inline void CMyMap2<T>::Lock( void )
{
	m_AddRemoveLock.Enter();
}

template <class T> inline void CMyMap2<T>::Unlock( void )
{
	m_AddRemoveLock.Leave();
}


template <class T> inline DWORD CMyMap2<T>::MAKESTR2DWORD( const char* pKey, size_t nSize )
{
	DWORD x	= 0, y	= 0;
	for( int i =0; ( i + sizeof(DWORD) ) < nSize; i += sizeof(DWORD) )
		x	+= *(LPDWORD)( pKey + i );
	for(; i < nSize; i++ )
		y	+= *(char*)( pKey + i );
	return ( x + y );
}
#endif	// __STL_0402

#endif // __MYMAP_H__