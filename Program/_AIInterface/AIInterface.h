#ifndef __AI_INTERFACE_H
#define __AI_INTERFACE_H

#include "MoverMsg.h"

class CAIInterface;

struct AIMSG
{
	DWORD dwMessage;
	DWORD dwParam1;
	DWORD dwParam2;
#ifdef __INSTANCE_AGGRO_SYSTEM
	DWORD dwParam3;
#endif // __INSTANCE_AGGRO_SYSTEM
};

typedef BOOL (CAIInterface::*FUNCAISTATE)( const AIMSG & );

struct STATEMAP_ENTRIES
{
	DWORD dwAIStateId;
	FUNCAISTATE pStateFunc;
};

struct STATEMAP
{
	STATEMAP* lpBaseStateMap;
	STATEMAP_ENTRIES* lpStateMapEntries;
};

enum
{
	AIMSG_INIT = 1,
	AIMSG_PROCESS, 
	AIMSG_TIMEOUT,
	AIMSG_SETSTATE,
	AIMSG_SETPROCESS,
	AIMSG_DSTDAMAGE,
	AIMSG_DSTDIE,
	AIMSG_DAMAGE,
	AIMSG_DIE,
	AIMSG_COLLISION,		// 충돌 Collision
	AIMSG_ARRIVAL,			// 이동 목표에 도착 
	AIMSG_NOMOVE,			// 갈 수 없음 
	AIMSG_BEGINMOVE,		// 이동 시작시 
	AIMSG_ATTACK_MELEE,		// 일반공격시 타점에서 호출.
	AIMSG_END_MELEEATTACK,	// 일반공격 끝났을때 호출.
	AIMSG_END_APPEAR,		// 등장씬 끝남.
	AIMSG_INIT_TARGETCLEAR,
	AIMSG_EXIT,
	AIMSG_TARGET_FIND,		// 타겟을 찾았다.
	AIMSG_REATTACK_TIMEOUT,  
	AIMSG_INVALID_TARGET,	// 타겟이 날거나, 없어지거나, 죽거나 .. 등등으로 공격 불가하다. 
#ifdef __INSTANCE_AGGRO_SYSTEM
	AIMSG_SKILLSUPPORT,
#endif // __INSTANCE_AGGRO_SYSTEM
};

enum AI2_STATE
{
	AI2_IDLE,
	AI2_MOVE,
	AI2_RAGE,
	AI2_SEARCH,			// 탐색 상태 
	AI2_TRACKING,		// 추적 상태 
	AI2_ATTACK,			// 공격 상태 
};

//////////////////////////////////////////////////////////////////////////////
// AI 메시지 핸들러 매크로 정의 
#define BeginAIHandler() if( 0 ) { 
#define OnMessage( x ) return TRUE; } else if( msg.dwMessage == x )	{
#define EndAIHandler() return TRUE; }

// State Map 핸들러 매크로 정의 
#define BEGIN_AISTATE_MAP( theClass, baseClass ) \
	const STATEMAP* theClass::GetStateMap() const  \
		{ return (STATEMAP*)&theClass::stateMap; } \
	const STATEMAP theClass::stateMap = \
		{ (STATEMAP*)&baseClass::stateMap, (STATEMAP_ENTRIES*)&theClass::stateMapEntries[0] }; \
	const STATEMAP_ENTRIES theClass::stateMapEntries[] = { \

#define END_AISTATE_MAP() 0, NULL };
#define ON_STATE( id, pStateFunc ) id, (FUNCAISTATE)pStateFunc, 
#define DECLARE_AISTATE() \
	static const STATEMAP_ENTRIES stateMapEntries[]; \
	static const STATEMAP stateMap; \
	virtual const STATEMAP* GetStateMap() const; 

//////////////////////////////////////////////////////////////////////////////

class CAIInterface
{
	FUNCAISTATE GetAIStateFunc( DWORD dwState );

protected:
	queue< AIMSG >	m_MsgQueue;
//	CRIT_SEC		m_AddRemoveLock;
	FUNCAISTATE		m_pStateFunc;
	CObj*			m_pObj;

public:
	DECLARE_AISTATE()

	CAIInterface();
	CAIInterface( CObj* pObj );
	virtual	~CAIInterface();

	// 매시지 관련 처리 매소드 
	virtual void	RouteMessage();
#ifdef __INSTANCE_AGGRO_SYSTEM
	virtual void	SendAIMsg( DWORD dwMessage, DWORD dwParam1 = 0, DWORD dwParam2 = 0, DWORD dwParam3 = 0 );
	virtual void	PostAIMsg( DWORD dwMessage, DWORD dwParam1 = 0, DWORD dwParam2 = 0, DWORD dwParam3 = 0 );
#else // __INSTANCE_AGGRO_SYSTEM
	virtual void	SendAIMsg( DWORD dwMessage, DWORD dwParam1 = 0, DWORD dwParam2 = 0 );
	virtual void	PostAIMsg( DWORD dwMessage, DWORD dwParam1 = 0, DWORD dwParam2 = 0 );
#endif // __INSTANCE_AGGRO_SYSTEM
	virtual void	InitAI() { }
#if __VER >= 9	//__AI_0509
	virtual		BOOL	IsReturnToBegin( void )		{	return FALSE;	}
#endif	// __AI_0509

	virtual BOOL	IsState( int nState );

protected:
	CMover* GetMover() { return (CMover*)m_pObj; }
	CWorld* GetWorld() { return m_pObj->GetWorld(); }

	//기타 잡다한 매소드 
	CMover* ScanTarget( CObj* pObjCenter, int nRangeMeter = 5, int nJobCond = 0, DWORD dwQuest = 0, DWORD dwItem = 0, int nChao = 0 );
	CMover* ScanTargetStrong( CObj* pObjCenter, FLOAT fRangeMeter );
	CMover* ScanTargetOverHealer( CObj* pObjCenter, FLOAT fRangeMeter  );
#ifdef __AI_AGGRO
	CMover* ScanTargetAggro( CObj* pObjCenter, FLOAT fRangeMeter, DWORD dwAIInterface, DWORD dwParty );
	CMover* ScanTargetOverLeader( CObj* pObjCenter, FLOAT fRangeMeter, DWORD idParty );
	CMover* ScanTargetOverParty( CObj* pObjCenter, FLOAT fRangeMeter, DWORD idParty );
	CMover* ScanTargetOverAggro( CObj* pObjCenter, FLOAT fRangeMeter );
	BOOL	IsInRange( D3DXVECTOR3& vDistant, FLOAT fRange );
#endif
#if __VER >= 9	//__AI_0509
	CMover*	ScanTargetNext( CObj* pObjCenter, int nRange,  OBJID dwIdTarget, u_long uParty );
	u_long	m_uParty;
#endif	// __AI_0509
};
//////////////////////////////////////////////////////////////////////////////


#endif

