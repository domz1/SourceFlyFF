#ifndef __AI_AGGRO_H
#define __AI_AGGRO_H

#include "..\_AIInterface\AIInterface.h"
#include "mempooler.h"
#include "Mover.h"
#include "PathObj.h"

#ifdef __AI_AGGRO

class CAIAggro : public CAIInterface
{
protected:
	DWORD			m_dwAIInterface;
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
	BOOL			m_bRangeAttack;				
	BOOL			m_bLootMove;				

	DWORD			m_dwIdTarget;
	DWORD			m_dwAtkMethod;
	vector<OBJID>	m_vecIdSummon;					
	DWORD			m_idLootItem;				

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
	int				SelectAttackType( CMover *pTarget );
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


public:
	virtual	BOOL	IsReturnToBegin( void )		{	return m_bReturnToBegin;	}
protected:

	
public:
	CAIAggro();
	CAIAggro( CObj* pObj, DWORD dwAIInterface );
	virtual ~CAIAggro();
	
	virtual void	InitAI();

public:
#ifndef __VM_0820
#ifndef __MEM_TRACE
	static	MemPooler<CAIAggro>*	m_pPool;
	void*	operator new( size_t nSize )	{	return CAIAggro::m_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CAIAggro::m_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CAIAggro::m_pPool->Free( (CAIAggro*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CAIAggro::m_pPool->Free( (CAIAggro*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820
	
	DECLARE_AISTATE()
};

#endif
#endif
