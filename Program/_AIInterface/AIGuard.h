#ifndef __AI_GUARD_H
#define __AI_GUARD_H

#include "mempooler.h"
//#include "Mover.h"

class CObj;

class CAIGuard : public CAIMonster
{
	BOOL	MoveProcessIdle( const AIMSG & msg );
	BOOL	MoveProcessRage( const AIMSG & msg );
	BOOL	MoveProcessRunaway();

	BOOL	StateInit( const AIMSG & msg );
	BOOL	StateIdle( const AIMSG & msg );
	BOOL	StateWander( const AIMSG & msg );
	BOOL	StateRunaway( const AIMSG & msg );
	BOOL	StateEvade( const AIMSG & msg );
	BOOL	StateRage( const AIMSG & msg );

public:

	CAIGuard();
	CAIGuard( CObj* pObj );
	virtual ~CAIGuard();

	virtual void InitAI();

public:
#ifndef __VM_0820
#ifndef __MEM_TRACE
	static	MemPooler<CAIGuard>*	m_pPool;
	void*	operator new( size_t nSize )	{	return CAIGuard::m_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CAIGuard::m_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CAIGuard::m_pPool->Free( (CAIGuard*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CAIGuard::m_pPool->Free( (CAIGuard*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820
	
	DECLARE_AISTATE()
};

#endif

