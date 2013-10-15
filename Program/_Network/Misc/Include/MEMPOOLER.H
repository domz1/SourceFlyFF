#ifndef __MEMPOOLER__
#define __MEMPOOLER__

#pragma	once
#include <assert.h>
#include <list>

template <class T>	class MemPooler
{
	struct	BlockNode
	{
		BlockNode* pNext;
		BlockNode()
			{	pNext	= NULL;	}
	};

protected:
	BlockNode*	m_pFreeList;
	list<void*>		m_ListOfMemBlockPtr;
	int		m_nNumOfBlock;
	int		m_nListBlockSize;	// size per block
	int		m_nAllocCount;
	CRITICAL_SECTION	m_cs;

private:
	char m_szClass[100];

public:
//	Constructions
#ifdef __VM_0819
	MemPooler( int nNumOfBlock, const char* szClass ): m_nNumOfBlock( nNumOfBlock ),
													m_pFreeList( NULL ),
													m_nAllocCount( 0 )
#else	// __VM_0819
	MemPooler( int nNumOfBlock ): m_nNumOfBlock( nNumOfBlock ),
													m_pFreeList( NULL ),
													m_nAllocCount( 0 )
#endif	// __VM_0819
		{
#ifdef __VERIFY_MEMPOOL
			return;
#endif
#ifdef __VM_0819
			strcpy( m_szClass, szClass );
#endif	// __VM_0819
			assert( nNumOfBlock > 0 );
			m_nListBlockSize	= sizeof(BlockNode) + sizeof(T);
			InitializeCriticalSectionAndSpinCount( &m_cs, 2000 );
			Create();
		}

	~MemPooler()
		{	Destroy();	}
//	Operations
	T*	Alloc( void )
		{
#ifdef __VERIFY_MEMPOOL
			return (T*) ::operator new( sizeof(T) );
#endif

			BlockNode* pNode	= NULL;
			T* lpMem	= NULL;

			EnterCriticalSection( &m_cs );

			if( !m_pFreeList )
				Create();

			pNode	= m_pFreeList;
			m_pFreeList		= m_pFreeList->pNext;
			m_nAllocCount++;
			lpMem	= reinterpret_cast<T*>( pNode + 1 );

			LeaveCriticalSection( &m_cs );

			return lpMem;
		}

	BOOL	Free( T* lpMem )
		{
#ifdef __VERIFY_MEMPOOL
			::operator delete(lpMem);
			return TRUE;
#endif

			BlockNode* pNode	= NULL;

			EnterCriticalSection( &m_cs );

			pNode	= ( reinterpret_cast<BlockNode*>( lpMem ) ) - 1;
			if( m_nAllocCount > 0 )
			{
				pNode->pNext	= m_pFreeList;
				m_pFreeList		= pNode;
				m_nAllocCount--;
			}

			//	mulcom	BEGIN100422	메모리풀 초기화
			::memset( lpMem, 0, sizeof(T) );
			//	mulcom	END100422	메모리풀 초기화

			LeaveCriticalSection( &m_cs );

			return TRUE;
		}

#ifdef __VM_0819
	void	Log( const char* szTitle )
		{
			EnterCriticalSection( &m_cs );
			const int nAllocCount	= m_nAllocCount;
			const int nTotal	= m_ListOfMemBlockPtr.size() * m_nNumOfBlock;
			LeaveCriticalSection( &m_cs );
			FILEOUT( "../vm.txt", "MEMPOOLER[%s][%s]\tnAllocCount : %d\tnPool: %d\tBlock: %d\tnSize: %d", m_szClass, szTitle, nAllocCount, nTotal, m_nNumOfBlock, sizeof(T) );
		}
#endif	// __VM_0819

protected:
	void	Create( void )
		{
#ifdef __VERIFY_MEMPOOL
			return;
#endif
#ifdef __VM_0819
			Log( "Create" );
#endif	// __VM_0819
			const int AllocationSize	= m_nListBlockSize * m_nNumOfBlock;
			void* pMemBlock		= VirtualAlloc( NULL, AllocationSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );
			ASSERT( pMemBlock );
			if( !pMemBlock )
				Error( "bad alloc" );
			m_ListOfMemBlockPtr.push_back( pMemBlock );
			BlockNode* pNode	= reinterpret_cast<BlockNode*>(pMemBlock);
			pNode	= reinterpret_cast<BlockNode*>( ( reinterpret_cast<DWORD>( pNode ) ) + ( m_nNumOfBlock - 1 ) * ( m_nListBlockSize ) );	// set pNode to last
			for( int i = m_nNumOfBlock - 1; i >= 0; i-- )
			{
				pNode->pNext	= m_pFreeList;
				m_pFreeList	= pNode;
				pNode	= reinterpret_cast<BlockNode*>( ( reinterpret_cast<DWORD>( pNode ) ) - m_nListBlockSize );	// set pNode to previous
			}
		}

	void	Destroy( void )
	{
#ifdef __VERIFY_MEMPOOL
		return;
#endif
		for( list<void*>::iterator i = m_ListOfMemBlockPtr.begin(); i != m_ListOfMemBlockPtr.end(); ++i )
			VirtualFree( *i, 0, MEM_RELEASE );
		m_ListOfMemBlockPtr.clear();

		DeleteCriticalSection( &m_cs );		
	}
};

#endif