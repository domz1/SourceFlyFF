#include "stdafx.h"
#include "AIInterface.h"
#include "DefineSkill.h"

#ifndef __CLIENT
#include "User.h"
#endif	// __CLIENT

#ifdef __AI_AGGRO
#include "Party.h"
extern CPartyMng g_PartyMng;
#endif


BEGIN_AISTATE_MAP( CAIInterface, CAIInterface )
END_AISTATE_MAP()

CAIInterface::CAIInterface()
{
	m_pObj = NULL;
	m_pStateFunc = NULL;
#if __VER >= 9	//__AI_0509
	m_uParty	= 0;
#endif	// __AI_0509
}
CAIInterface::CAIInterface( CObj* pObj ) 
{ 
	m_pObj = pObj;
	m_pStateFunc = NULL;
#if __VER >= 9	//__AI_0509
	m_uParty	= 0;
#endif	// __AI_0509
}

CAIInterface::~CAIInterface()
{
}



FUNCAISTATE CAIInterface::GetAIStateFunc( DWORD dwState)
{
	const STATEMAP* lpCurrentStateMap = GetStateMap();
	const STATEMAP_ENTRIES* lpEntries = lpCurrentStateMap->lpStateMapEntries;
	// 하부 객체부터 일치하는 매시지를 찾는다. 
	while( lpCurrentStateMap->lpBaseStateMap->lpStateMapEntries != lpEntries )
	{
		int i = 0;
		while(  lpEntries[ i ].pStateFunc )
		{
			if( lpEntries[ i ].dwAIStateId == dwState )
			{
				return lpEntries[ i ].pStateFunc;
			}
			i++;
		}
		// 발견되지 않았다! 상위 객체로 포인터를 바꾼다.
		lpCurrentStateMap = lpCurrentStateMap->lpBaseStateMap;
		lpEntries = lpCurrentStateMap->lpStateMapEntries;
	}
	return NULL;
}


void CAIInterface::RouteMessage()
{
	AIMSG msg;

	while( !m_MsgQueue.empty() )
	{
		msg	= m_MsgQueue.front();
		m_MsgQueue.pop();
		if( msg.dwMessage == AIMSG_SETSTATE )
		{
			msg.dwMessage	= AIMSG_INIT;
			m_pStateFunc	= GetAIStateFunc( msg.dwParam1 );
		}
		if( m_pStateFunc )
			( this->*( m_pStateFunc ) ) ( msg );
	}
	if( m_pStateFunc )
	{
		msg.dwMessage	= AIMSG_PROCESS;
		msg.dwParam1	= 0;
		msg.dwParam2	= 0;
#ifdef __INSTANCE_AGGRO_SYSTEM
		msg.dwParam3	= 0;
#endif // __INSTANCE_AGGRO_SYSTEM
		( this->*( m_pStateFunc ) ) ( msg );
	}
}
#ifdef __INSTANCE_AGGRO_SYSTEM
void CAIInterface::SendAIMsg( DWORD dwMessage, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3 )
#else // __INSTANCE_AGGRO_SYSTEM
void CAIInterface::SendAIMsg( DWORD dwMessage, DWORD dwParam1, DWORD dwParam2 )
#endif // __INSTANCE_AGGRO_SYSTEM
{
	AIMSG msg;
	msg.dwMessage	= dwMessage;
	msg.dwParam1	= dwParam1;
	msg.dwParam2	= dwParam2;
#ifdef __INSTANCE_AGGRO_SYSTEM
	msg.dwParam3	= dwParam3;
#endif // __INSTANCE_AGGRO_SYSTEM
	if( dwMessage == AIMSG_SETSTATE )
	{
		msg.dwMessage	= AIMSG_INIT;
		m_pStateFunc	= GetAIStateFunc( dwParam1 );
	}
	if( m_pStateFunc )
		( this->*( m_pStateFunc ) ) ( msg );
}
#ifdef __INSTANCE_AGGRO_SYSTEM
void CAIInterface::PostAIMsg( DWORD dwMessage, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3 )
#else // __INSTANCE_AGGRO_SYSTEM
void CAIInterface::PostAIMsg( DWORD dwMessage, DWORD dwParam1, DWORD dwParam2 )
#endif // __INSTANCE_AGGRO_SYSTEM
{
#if __VER >= 9	//__AI_0509
	switch( dwMessage )
	{
		case AIMSG_ARRIVAL:
			{
				AIMSG msg;
				msg.dwMessage	= dwMessage;
				msg.dwParam1	= dwParam1;
				msg.dwParam2	= dwParam2;
#ifdef __INSTANCE_AGGRO_SYSTEM
				msg.dwParam3	= dwParam3;
#endif // __INSTANCE_AGGRO_SYSTEM
				m_MsgQueue.push( msg );
				break;
			}
#ifdef __INSTANCE_AGGRO_SYSTEM
		default:	SendAIMsg( dwMessage, dwParam1, dwParam2, dwParam3 );	break;
#else // __INSTANCE_AGGRO_SYSTEM
		default:	SendAIMsg( dwMessage, dwParam1, dwParam2 );	break;
#endif // __INSTANCE_AGGRO_SYSTEM
	}
#else	// __AI_0509
	AIMSG msg;
	msg.dwMessage	= dwMessage;
	msg.dwParam1	= dwParam1;
	msg.dwParam2	= dwParam2;
	m_MsgQueue.push( msg );
#endif	// __AI_0509
}

#if __VER >= 9	//__AI_0509
CMover* CAIInterface::ScanTargetNext( CObj* pObjCenter, int nRange, OBJID dwIdTarget, u_long uParty )
{
	// 타겟이 없어졌을 때, 공격 조건을 만족하는 주변에 같은 파티원을 검색한다.
	// 기존 타겟이 파티가 없었거나, 가드면 무시
	if( uParty == 0 || nRange <= 0 || GetMover()->GetProp()->dwClass == RANK_GUARD )
		return NULL;

	TRACE( "ScanTargetNext\n" );
	CObj* pObj;
	D3DXVECTOR3 vPos	= pObjCenter->GetPos();
	D3DXVECTOR3 vDist;
	CMover *pTarget	= NULL;

	BOOL bFlyMob	= GetMover()->IsFlyingNPC();
	float fRadius	= (float)( nRange * nRange );

	FOR_LINKMAP( pObjCenter->GetWorld(), vPos, pObj, nRange, CObj::linkPlayer, GetMover()->GetLayer() )
	{
		pTarget	= (CMover *)pObj;
		if( pTarget->IsLive() && pTarget->GetId() != dwIdTarget ) 
		{
			if( bFlyMob == pTarget->IsFly() || bFlyMob == FALSE  )
			{
				vDist	= pTarget->GetPos() - vPos;				
				if( D3DXVec3LengthSq( &vDist ) < fRadius && pTarget->m_idparty == uParty )
				{
					if( pTarget->IsMode( TRANSPARENT_MODE ) == FALSE )
						if( pTarget->HasBuffByIk3( IK3_TEXT_DISGUISE ) == FALSE )
							return pTarget;
				}
			}
		}
	}
	END_LINKMAP
	return NULL;
}
#endif	// __AI_0509

// pObjCenter를 중심으로 nRangeMeter반경내에 들어오는넘들중 가장먼저 걸리는 넘 골라냄/
// nJobCond : 해당 직업만 감시함. 직업검사 안함.
CMover* CAIInterface::ScanTarget( CObj* pObjCenter, int nRangeMeter, int nJobCond, DWORD dwQuest, DWORD dwItem, int nChao )
{
	CObj* pObj;
	D3DXVECTOR3 vPos = pObjCenter->GetPos();
	D3DXVECTOR3 vDist;
	CMover *pTarget = NULL;
	int nAble = 0;
	BOOL bCondScan = FALSE;
	if( nJobCond || dwQuest || dwItem || nChao )
		bCondScan = TRUE;

	BOOL bGuard = ( GetMover()->GetProp()->dwClass == RANK_GUARD );
	BOOL bFlyMob = GetMover()->IsFlyingNPC();

	float fRadius = (float)( nRangeMeter * nRangeMeter );	// 거리 

	FOR_LINKMAP( pObjCenter->GetWorld(), vPos, pObj, nRangeMeter, CObj::linkPlayer, GetMover()->GetLayer() )
	{
		ASSERT( pObj->GetType() == OT_MOVER && ((CMover *)pObj)->IsPlayer() );
		pTarget = (CMover *)pObj;

		// 조건 스켄이 아니라면 늘 가능하다.
		nAble = 0;
		// 조건 스켄이라면 프랍과 일치하는 놈만 가능하다.
		if( bCondScan == TRUE && pTarget->IsLive() ) 
		{
			// 1. 직업 조건 체크    
			if( nJobCond == 0 )
				nAble++;
			else if( nJobCond == JOB_ALL || pTarget->GetJob() == nJobCond )		// JOB_ALL : 모든직업
				nAble++;
			// 2. 아이템 조건 체크  
			if( dwItem == 0 )
				nAble++;
			else if( pTarget->GetItemNum( dwItem ) != 0 )	
				nAble++;
			// 3. 퀘스트 조건 체크 
			if( dwQuest == 0 )
				nAble++;
			else if( pTarget->GetQuest( dwQuest ) != NULL )
				nAble++;	
			//4. 카오 조건 체크  
			if( nChao == 0 )
				nAble++;
			else
			{
				// 100 = 카오유저, 101 = 비카오
				if( nChao == 100 )
				{
#if __VER >= 8 // __S8_PK
					if( pTarget->IsChaotic() )
#else // __VER >= 8 // __S8_PK
					if( pTarget->IsGuardReactionChao() )
#endif // __VER >= 8 // __S8_PK
						nAble++;
				}
				else if( nChao == 101 )
				{
#if __VER >= 8 // __S8_PK
					if( !pTarget->IsChaotic() )
#else // __VER >= 8 // __S8_PK
					if( pTarget->IsGuardReactionNormal() )
#endif // __VER >= 8 // __S8_PK
						nAble++;
				}
			}
		}
		if( nAble == 4 )
		{
			// 가드는 무조건 공격			
			// 비행몹은 비행플레이어만 공격, 지상몹은 지상플레이어만만 공격 - 1. true true  2. false false

#ifdef __JHMA_VER_8_6     // 8차 지상몬스터가 저공비행유저를 공격가능하게함   World
			if( bGuard || bFlyMob == pTarget->IsFly() || bFlyMob == FALSE  )		
#else	// __VER >= 8  
			if( bGuard || bFlyMob == pTarget->IsFly() )		
#endif	// __VER >= 8  
			{
				vDist = pTarget->GetPos() - vPos;				
				if( D3DXVec3LengthSq( &vDist ) < fRadius )	// 두 객체간의 거리가 범위 이내이면 
				{
					if( pTarget->IsMode( TRANSPARENT_MODE ) == FALSE )
						if( pTarget->HasBuffByIk3( IK3_TEXT_DISGUISE ) == FALSE )
							return pTarget;
				}
			}
		}
	}
	END_LINKMAP
	return NULL;
}

// 반경내에서 가장 강한넘(레벨로)을 골라냄.
CMover* CAIInterface::ScanTargetStrong( CObj* pObjCenter, FLOAT fRangeMeter  )
{
	CObj *pObj;
	CObj *pObjMax = NULL;	// 가장쎈넘 포인터.
	D3DXVECTOR3 vPos = pObjCenter->GetPos();
	D3DXVECTOR3 vDist;
	
	// 지름 
	FLOAT fRadius = fRangeMeter * fRangeMeter;
	
	FOR_LINKMAP( pObjCenter->GetWorld(), vPos, pObj, (int)( fRangeMeter / MPU ), CObj::linkPlayer, GetMover()->GetLayer() )
	{
		vDist = pObj->GetPos() - vPos;	// 두좌표간 벡터
		float fDistSq = D3DXVec3LengthSq( &vDist );		// 두오브젝트간의 거리Sq
		if( fDistSq < fRadius )	
		{
			if(  !( ((CMover*)pObj)->IsMode( TRANSPARENT_MODE ) ) )
			{
				if( pObjMax )
				{
					if( ((CMover *)pObj)->GetLevel() > ((CMover *)pObjMax)->GetLevel() )		// 더 쎈넘을 찾았다.
						pObjMax = pObj;
				} else
					pObjMax = pObj;

			}
		}
	}
	END_LINKMAP

	return (CMover *)pObjMax;
}

// 반경내에서 오버힐 하는 어시 찾아서 죽이자.
CMover* CAIInterface::ScanTargetOverHealer( CObj* pObjCenter, FLOAT fRangeMeter  )
{
#ifndef __CLIENT
	CObj *pObj;
	D3DXVECTOR3 vPos = pObjCenter->GetPos();
	D3DXVECTOR3 vDist;
	
	// 지름 
	FLOAT fRadius = fRangeMeter * fRangeMeter;
	
	FOR_LINKMAP( pObjCenter->GetWorld(), vPos, pObj, (int)( fRangeMeter / MPU ), CObj::linkPlayer, GetMover()->GetLayer() )
	{
		vDist = pObj->GetPos() - vPos;	// 두좌표간 벡터
		float fDistSq = D3DXVec3LengthSq( &vDist );		// 두오브젝트간의 거리Sq
		if( fDistSq < fRadius )	
		{
			if(  !( ((CMover*)pObj)->IsMode( TRANSPARENT_MODE ) ) )
			{
				CMover *pMover = (CMover *)pObj;

				if( pMover->IsPlayer() )
					if( ((CUser *)pMover)->m_nOverHeal > 0 )		// 오버힐러를 찾았다.
						return pMover;
			}
		}
	}
	END_LINKMAP
#endif	// __CLIENT
	return NULL;
}
#ifdef __AI_AGGRO
CMover* CAIInterface::ScanTargetAggro( CObj* pObjCenter, FLOAT fRangeMeter, DWORD dwAIInterface, DWORD dwParty )
{
	switch( dwAIInterface )
	{
		case AII_AGGRO_NORMAL:
			return ScanTargetOverAggro( pObjCenter, fRangeMeter );
			break;
		case AII_PARTY_AGGRO_LEADER:
			return ScanTargetOverLeader( pObjCenter, fRangeMeter, dwParty );
			break;
		case AII_PARTY_AGGRO_SUB:
			return ScanTargetOverParty( pObjCenter, fRangeMeter, dwParty );
			break;
		case AII_ARENA_REAPER:
			return ScanTargetOverAggro( pObjCenter, fRangeMeter );
			break;
	}
	return NULL;
}
BOOL CAIInterface::IsInRange( D3DXVECTOR3& vDistant, FLOAT fRange )
{
	vDistant.y	= 0.0f;
	float fDistSq = D3DXVec3LengthSq( &vDistant );	
	if( fDistSq > fRange * fRange )	
		return FALSE;
	return TRUE;
}
CMover* CAIInterface::ScanTargetOverLeader( CObj* pObjCenter, FLOAT fRangeMeter, DWORD idParty )
{
#ifndef __CLIENT
	CParty* pParty = g_PartyMng.GetParty( idParty );
	CMover* pMover = NULL;
	if( pParty )	
		pMover = pParty->GetLeader();
	else
		return ScanTargetOverAggro( pObjCenter, fRangeMeter );
	if( pMover && pMover->IsLive() && IsInRange( ( pObjCenter->GetPos() - pMover->GetPos() ), fRangeMeter ) )
	{
		pMover->SetAggro( pMover->GetAggro() + 1 );
		return pMover;
	}
	else
		return ScanTargetOverParty( pObjCenter, fRangeMeter, idParty );
#endif
	return NULL;
}
CMover* CAIInterface::ScanTargetOverParty( CObj* pObjCenter, FLOAT fRangeMeter, DWORD idParty )
{
#ifndef __CLIENT
	CObj *pObj;
	CObj *pObjMax = NULL;
	D3DXVECTOR3 vPos = pObjCenter->GetPos();
	D3DXVECTOR3 vDist;
	
	FLOAT fRadius = fRangeMeter * fRangeMeter;
	
	FOR_LINKMAP( pObjCenter->GetWorld(), vPos, pObj, (int)( fRangeMeter / MPU ), CObj::linkPlayer, GetMover()->GetLayer() )
	{
		ASSERT( pObj->GetType() == OT_MOVER && ((CMover *)pObj)->IsPlayer() );
		vDist = pObj->GetPos() - vPos;
		float fDistSq = D3DXVec3LengthSq( &vDist );
		if( fDistSq < fRadius )	
		{
			if( ((CMover*)pObj)->IsLive() && !((CMover*)pObj)->IsMode( TRANSPARENT_MODE ) && ((CMover*)pObj)->GetPartyId() == idParty )
			{
				((CMover *)pObj)->SetAggro( ((CMover *)pObj)->GetAggro() + 1 );
				if( pObjMax )
				{
					if( ((CMover *)pObj)->GetAggro() > ((CMover *)pObjMax)->GetAggro() )		// 더 쎈넘을 찾았다.
						pObjMax = pObj;
				} else
					pObjMax = pObj;

			}
		}
	}
	END_LINKMAP
	return (CMover *)pObjMax;
#else
	return NULL;
#endif
}
CMover* CAIInterface::ScanTargetOverAggro( CObj* pObjCenter, FLOAT fRangeMeter )
{
#ifndef __CLIENT
	CObj *pObj;
	CObj *pObjMax = NULL;
	D3DXVECTOR3 vPos = pObjCenter->GetPos();
	D3DXVECTOR3 vDist;
	
	FLOAT fRadius = fRangeMeter * fRangeMeter;
	
	FOR_LINKMAP( pObjCenter->GetWorld(), vPos, pObj, (int)( fRangeMeter / MPU ), CObj::linkPlayer, GetMover()->GetLayer() )
	{
		ASSERT( pObj->GetType() == OT_MOVER && ((CMover *)pObj)->IsPlayer() );
		vDist = pObj->GetPos() - vPos;
		float fDistSq = D3DXVec3LengthSq( &vDist );
		if( fDistSq < fRadius )	
		{
			if( ((CMover*)pObj)->IsLive() && !( ((CMover*)pObj)->IsMode( TRANSPARENT_MODE ) ) )
			{
				((CMover *)pObj)->SetAggro( ((CMover *)pObj)->GetAggro() + 1 );
				if( pObjMax )
				{
					if( ((CMover *)pObj)->GetAggro() > ((CMover *)pObjMax)->GetAggro() )		// 더 쎈넘을 찾았다.
						pObjMax = pObj;
				} else
					pObjMax = pObj;

			}
		}
	}
	END_LINKMAP
	return (CMover *)pObjMax;
#else
	return NULL;
#endif
}
#endif
BOOL CAIInterface::IsState( int nState )
{
	if( m_pStateFunc == GetAIStateFunc( nState ) )
		return TRUE;
	return FALSE;
}

