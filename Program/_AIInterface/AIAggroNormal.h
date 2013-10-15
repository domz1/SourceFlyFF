#ifndef __AI_AGGRO_NORMAL_H
#define __AI_AGGRO_NORMAL_H

#include "mempooler.h"
#include "Mover.h"
#include "PathObj.h"

class CAIAggroNormal : public CAIInterface
{
protected:
	DWORD 			m_tmMove;
	DWORD			m_tmAttackDelay;
	DWORD			m_tmAttack;		
	DWORD			m_tmSummon;					// 소환 타이머.
	DWORD			m_tmHelp;					// 도움 요청 타이머.
	DWORD			m_tmReturnToBegin;			// 리젠지점으로 돌아가는 시간측정.

	D3DXVECTOR3		m_vPosBegin;
	D3DXVECTOR3		m_vDestPos;
	D3DXVECTOR3		m_vOldPos;					// 이전 프레임 좌표

#if __VER >= 9	//__AI_0509
	D3DXVECTOR3		m_vPosDamage;
#endif	// __AI_0509
	
	_VECINFO		m_vecPatrolInfo;
	FLOAT			m_fAngleBegine;

	BOOL			m_bReturnToBegin;
	BOOL			m_bGoTarget;
	BOOL			m_bTargetNoMovePos;
	BOOL			m_bFirstRunaway;
	BOOL			m_bCallHelper;
	BOOL			m_bRangeAttack;				// 레인지 어택이냐 아니냐.
	BOOL			m_bLootMove;				// 루팅하러 가는중.

	DWORD			m_dwIdTarget;
	DWORD			m_dwAtkMethod;
#if __VER >= 12 // __NEW_SUMMON_RULE
	vector<OBJID>	m_vecIdSummon;					// 소환된 몬스터는 아이디가 채워진다.
#else // __NEW_SUMMON_RULE
	OBJID			m_idSummon[ MAX_SUMMON ];	// 소환된 몬스터는 아이디가 채워진다.
#endif // __NEW_SUMMON_RULE
	DWORD			m_idLootItem;				// 루팅할 아이템.

	DWORD			GetAtkMethod_Near();
	DWORD			GetAtkMethod_Far();
	DWORD			GetAtkRange( DWORD dwAtkMethod );
	ItemProp*		GetAtkProp( DWORD dwAtkMethod );
	DWORD			GetAtkItemId( DWORD dwAtkMethod );
	void			DoAttack( DWORD dwAtkMethod, CMover* pTarget );
	void			DoReturnToBegin( BOOL bReturn = TRUE );
	void			MoveToDst( OBJID dwIdTarget );
	void			MoveToDst( const D3DXVECTOR3& vDst );
	void			MoveToRandom( UINT nState );
	void			CallHelper( const MoverProp* pMoverProp );
	void			SetStop( DWORD dwTime );
	BOOL			IsEndStop();
	BOOL			IsMove() { return GetMover()->m_pActMover->IsMove(); }
	BOOL			IsInRange( D3DXVECTOR3& vDistant, FLOAT fRange );
	int				SelectAttackType( CMover *pTarget );	// AI에 따른 공격방식을 선택.
	BOOL			MoveProcessIdle( const AIMSG & msg );
	BOOL			MoveProcessRage( const AIMSG & msg );
	BOOL			SubAttackChance( const AIMSG & msg, CMover *pTarget );
	BOOL			MoveProcessRunaway();
	BOOL			StopProcessIdle();
	void			SubSummonProcess( CMover *pTarget );
	BOOL			SubItemLoot();
	void			Init();
	BOOL			StateInit( const AIMSG & msg );
	BOOL			StateIdle( const AIMSG & msg );
	BOOL			StateWander( const AIMSG & msg );
	BOOL			StateRunaway( const AIMSG & msg );
	BOOL			StateEvade( const AIMSG & msg );
	BOOL			StateRage( const AIMSG & msg );

	BOOL			StateRagePatrol( const AIMSG & msg );
	BOOL			MoveProcessRagePatrol( const AIMSG & msg );	
	BOOL			StateStand( const AIMSG & msg );
	BOOL			StatePatrol( const AIMSG & msg );
	BOOL			MoveProcessStand( const AIMSG & msg );
	BOOL			MoveProcessPatrol( const AIMSG & msg );

	void	SetTarget( OBJID dwIdTarget, u_long uParty );

#if __VER >= 9	//__AI_0509
public:
	virtual	BOOL	IsReturnToBegin( void )		{	return m_bReturnToBegin;	}
protected:
#endif	// __AI_0509
	
public:
	CAIAggroNormal();
	CAIAggroNormal( CObj* pObj );
	virtual ~CAIAggroNormal();
	
	virtual void	InitAI();

#ifndef __VM_0820
#ifndef __MEM_TRACE
	static	MemPooler<CAIAggroNormal>*	m_pPool;
	void*	operator new( size_t nSize )	{	return CAIAggroNormal::m_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CAIAggroNormal::m_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CAIAggroNormal::m_pPool->Free( (CAIAggroNormal*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CAIAggroNormal::m_pPool->Free( (CAIAggroNormal*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820
	
	DECLARE_AISTATE()
};
#endif

