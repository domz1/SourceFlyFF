#ifndef __AI_CLOCKWORKS_H
#define __AI_CLOCKWORKS_H

#include "mempooler.h"

class CAIClockWorks : public CAIInterface
{
	D3DXVECTOR3 m_vPosBegin;		// 최초 생성된 좌표.
	int		m_nEvent;				// 현재 패턴 상태.
	int		m_nAttackType;			// 공격 방식.
	DWORD	m_tmReattack;			// 재공격 타이머.
	DWORD	m_tmAddReattack;		// 재공격 타이머의 랜덤변위값.(이 랜덤값으로 더 빨리 쏘는 경우도 생김)
	DWORD	m_tmTrace;				// 추적시간.
	DWORD	m_tmTimeOver;			// 예상치 못한 상황으로 더이상 공격을 안하던가 하는경우를 감별
	OBJID	m_idTarget;				// 공격 대상
	D3DXVECTOR3	m_vTarget;			// 공격 위치.
	int		m_nAppearCnt;			// 등장 카운터.
	
	OBJID	m_idLastAttacker;		// 날 마지막으로 공격한 쉐리.

	BOOL	m_bDefenseMode;			// 방어태세 모드.
	
	BOOL	MoveProcessIdle();
	BOOL	MoveProcessRage();
	BOOL	MoveProcessRunaway();
	
	BOOL	StopProcessIdle();

	BOOL	SelectTarget( void );		// 리어택 타이밍이 됬을때 타겟을 선정함.
		
	void Init( void );
	void Destroy( void );

public:
	
	CAIClockWorks();
	CAIClockWorks( CObj* pObj );
	virtual ~CAIClockWorks();
	
	virtual void InitAI();

	int GetEvent( void ) { return m_nEvent; }
	BOOL	StateInit( const AIMSG & msg );
	BOOL	StateIdle( const AIMSG & msg );
	BOOL	StateWander( const AIMSG & msg );
	BOOL	StateRunaway( const AIMSG & msg );
	BOOL	StateEvade( const AIMSG & msg );
	BOOL	StateRage( const AIMSG & msg );
	
public:
#ifndef __VM_0820
#ifndef __MEM_TRACE
	static	MemPooler<CAIClockWorks>*	m_pPool;
	void*	operator new( size_t nSize )	{	return CAIClockWorks::m_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CAIClockWorks::m_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CAIClockWorks::m_pPool->Free( (CAIClockWorks*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CAIClockWorks::m_pPool->Free( (CAIClockWorks*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820
	
	DECLARE_AISTATE()
};

#endif

