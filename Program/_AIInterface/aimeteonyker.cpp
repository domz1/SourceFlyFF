#include "stdafx.h"
#include "aimeteonyker.h"

#if __VER >= 10	// __METEONYKER_0608

#include "user.h"
#include "dialogmsg.h"
extern	CUserMng	g_UserMng;
#include "definetext.h"
#include "dpcoreclient.h"
extern	CDPCoreClient	g_DPCoreClient;

enum
{
	STATE_INIT	= 1,
	STATE_IDLE,
	STATE_WANDER,
	STATE_PURSUE,
	STATE_EVADE,
	STATE_RUNAWAY,
	STATE_RAGE
	, STATE_STAND
	, STATE_PATROL	
	, STATE_RAGE_PATROL	
};

#define RANGE_CANCEL	120.0f
#define RANGE_MOVE	30.0f
#define	RANGE_RETURN_TO_BEGIN	120.0F

static DWORD s_tmAttack = SEC(20);	//  m_tmAttack 20초 

BEGIN_AISTATE_MAP( CAIMeteonyker, CAIInterface )
	ON_STATE( STATE_INIT, StateInit )
	ON_STATE( STATE_IDLE, StateIdle )
	ON_STATE( STATE_RAGE, StateRage )
END_AISTATE_MAP()

CAIMeteonyker::CAIMeteonyker()
{
	Init();
}

CAIMeteonyker::CAIMeteonyker( CObj* pObj ) : CAIInterface( pObj )
{
	Init();
}

CAIMeteonyker::~CAIMeteonyker()
{
	Destroy();
}

void CAIMeteonyker::Destroy( void )
{
	m_mapDamage.clear();
}

#define	CB_RETALIATION	10	// 30초당 1회
void CAIMeteonyker::Init( void )
{
	SetTarget( NULL_ID, 0 );
	m_cbRetaliation	= CB_RETALIATION;	// 30초당 1회 발동
//	m_bHeal		= FALSE;	// 치유했나?
	m_dwAtk		= OBJMSG_NONE;
	m_idLastAttacker	= NULL_ID;

	m_bReturnToBegin	= FALSE;
	m_tmAttack	= g_tmCurrent + s_tmAttack;	// 공격 후 20초간 추적해옴.
	m_vOldPos.x	= m_vOldPos.y	= m_vOldPos.z	= 0;
	m_tmRecovery	= m_tmAttackDelay	= m_tmMove = m_tmReturnToBegin	= g_tmCurrent;
	m_vPosDamage	= D3DXVECTOR3( 0, 0, 0 );
	m_cbAttack	= 0;
	m_nSPAttackType	= 0;
	m_dwHPOld	= 100;
	m_bCritical		= FALSE;
}

void CAIMeteonyker::InitAI( void )
{
	m_vPosBegin	= GetMover()->GetPos();
	PostAIMsg( AIMSG_SETSTATE, GetMover()->m_dwAIInterfaceState ); 	
}

void CAIMeteonyker::SetStop( DWORD dwTime ) 
{ 
	m_tmMove	= g_tmCurrent + dwTime;
	GetMover()->SetStop(); 
} 

BOOL CAIMeteonyker::IsEndStop() 
{ 
	return ( g_tmCurrent > (DWORD)( m_tmMove ) ) && ( IsMove() == FALSE ); 
}

void CAIMeteonyker::MoveToDst(	const D3DXVECTOR3& vDst )
{
	TRACE( "moveTodst\n" );
	CMover* pMover	= GetMover();
	if( pMover->GetDestPos() == vDst )
		return;
	pMover->SetDestPos( vDst );
	pMover->m_nCorr		= -1;
	g_UserMng.AddSetDestPos( pMover, vDst, 1 );
}

void CAIMeteonyker::MoveToRandom( UINT nState )
{
	CMover* pMover	= GetMover();
	CWorld* pWorld	= GetWorld();

	D3DXVECTOR3 vPos	= pMover->GetPos();
	DWORD x1	= xRand();
	DWORD x2	= xRand();

	vPos.x	+= int( x1 % 21 ) - 10;
	vPos.z	+= int( x2 % 21 ) - 10;

	if( nState == STATE_IDLE )
	{
		D3DXVECTOR3 vDistant	= vPos - m_vPosBegin;
		vDistant.y	= 0.0f;

		float fRange	= RANGE_MOVE;
		if( IsInRange( vDistant, fRange ) )
			MoveToDst( vPos );
		else
		{
			if( m_bReturnToBegin )
				MoveToDst( vPos );
			else
			{
				vDistant = pMover->GetPos() - m_vPosBegin;
				if( IsInRange( vDistant, fRange ) == FALSE )
					DoReturnToBegin();
			}
		}
	}
	else
	{
		MoveToDst( vPos );
	}
}

BOOL CAIMeteonyker::IsInRange( const D3DXVECTOR3& vDistant, FLOAT fRange )
{
	float fDistSq	= D3DXVec3LengthSq( &vDistant );	
	if( fDistSq > fRange * fRange )	
		return FALSE;
	return TRUE;
}

void CAIMeteonyker::DoReturnToBegin( BOOL bReturn )
{
	if( bReturn )
	{
		m_bReturnToBegin	= TRUE;
		m_tmReturnToBegin	= g_tmCurrent;
		SetTarget( NULL_ID, 0 );
		CMover* pMover	= GetMover();
		pMover->SetSpeedFactor( 2.66F );
		MoveToDst( m_vPosBegin );
	}
	else
	{
		m_bReturnToBegin	= FALSE;
		SetTarget( NULL_ID, 0 );
//		CMover* pMover	= GetMover();
//		pMover->SetPointParam( DST_HP, pMover->GetMaxHitPoint() );
//		pMover->RemoveAllEnemies();
//		m_bHeal		= FALSE;
	}
}

BOOL CAIMeteonyker::StateInit( const AIMSG & msg )
{
	return TRUE;
}

BOOL CAIMeteonyker::MoveProcessIdle( void )
{
	CMover* pMover	= GetMover();
	CWorld* pWorld	= GetWorld();
	MoverProp *pProp	= pMover->GetProp();

	if( pMover->IsDie() || ( pMover->m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL) )
		return FALSE;

#ifdef __MOVER_STATE_EFFECT
	if( pMover->m_pActMover->IsStateFlag( OBJSTAF_COMBAT ) == TRUE )
	{
		if( pMover->m_pActMover->SendActMsg( OBJMSG_MODE_PEACE ) == TRUE )
		{
			g_UserMng.AddSendActMsg( pMover, OBJMSG_MODE_PEACE );
		}
	}
#endif // __MOVER_STATE_EFFECT

	if( m_bReturnToBegin )
	{
		if( IsInRange( m_vPosBegin - pMover->GetPos(), 10.0f ) )
		{
			DoReturnToBegin( FALSE );
			SendAIMsg( AIMSG_ARRIVAL, NULL_ID );
		}
		if( g_tmCurrent - m_tmReturnToBegin >= SEC(20) )
		{
			pMover->SetPos( m_vPosBegin );
			g_UserMng.AddSetPos( pMover, m_vPosBegin );
			m_tmReturnToBegin	= g_tmCurrent;
		}
	}
	else
	{
		if( pMover->GetMovePattern() == 0 )
		{
			if( IsEndStop() )
				MoveToRandom( STATE_IDLE );
		}
		else		
		{
			if( pMover->GetMoveEvent() == 99 )
				pMover->SetMovePattern( 0 );
		}

		if( pProp->m_nScanJob != -1 )
		{
			if( GetMover()->m_bActiveAttack )
				SelectTarget();
			CMover* pTarget		= GetTarget( m_dwIdTarget );
			if( IsValidObj( (CObj*)pTarget ) )
			{
				if( pMover->IsFlyingNPC() == FALSE || pMover->IsFlyingNPC() == pTarget->m_pActMover->IsFly() || pProp->dwClass == RANK_GUARD )	// 가드는 무조건 공격.
				{
					if( pTarget )	
					{
						g_UserMng.AddDlgEmoticon( pMover, DLGEMOT_ATTACK );
						SendAIMsg( AIMSG_SETSTATE, STATE_RAGE, pTarget->GetId() );
					}
				}
			}
		}
	}
	ProcessRecovery();

	return TRUE;
}

BOOL CAIMeteonyker::StateIdle( const AIMSG & msg )
{
	CMover* pMover	= GetMover();
	CWorld* pWorld	= GetWorld();

	BeginAIHandler()
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT )
		int nRangeMove	= (int)( RANGE_MOVE );
		D3DXVECTOR3	vTargetTo	= pMover->GetPos() - m_vPosBegin;
		if( IsInRange( vTargetTo, (float)( nRangeMove ) ) == FALSE )
			DoReturnToBegin();
		SetStop( SEC( 0 ) );
	OnMessage( AIMSG_INIT_TARGETCLEAR )
		SetTarget( NULL_ID, 0 );

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_PROCESS )
		MoveProcessIdle();
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DAMAGE )
		if( m_bReturnToBegin == FALSE )
		{
			MoveToDst( msg.dwParam1 );
			SendAIMsg( AIMSG_SETSTATE, STATE_RAGE, msg.dwParam1 );
			m_tmAttack	= g_tmCurrent + s_tmAttack;
			m_vPosDamage	= pMover->GetPos();
		}
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DIE )
		SendAIMsg( AIMSG_EXIT );
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_COLLISION )
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_ARRIVAL )
		if( m_bReturnToBegin )
			SetStop( xRandom( SEC( 1 ) ) );
		else
			SetStop( SEC( 5 ) + xRandom( SEC( 1 ) ) );
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_EXIT )
	
	EndAIHandler()
	return TRUE;
}

BOOL CAIMeteonyker::StateRage( const AIMSG & msg )
{
	CMover* pMover	= GetMover();
	CWorld* pWorld	= GetWorld();

	BeginAIHandler()
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT )
		if( msg.dwParam2 != NULL_ID )
		{
			CMover* pTarget	= GetTarget( msg.dwParam2 );
			if( IsValidObj( pTarget ) )
				SetTarget( msg.dwParam2, pTarget->m_idparty );
		}
		m_tmAttack = g_tmCurrent + s_tmAttack;
		m_tmAttackDelay		= g_tmCurrent;
		SetStop( 0 );

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT_TARGETCLEAR )
		CMover* pTarget	= ScanTargetNext( pMover, 15, m_dwIdTarget, m_uParty );
		if( pTarget )
		{
			SendAIMsg( AIMSG_SETSTATE, STATE_RAGE, pTarget->GetId() );
			return TRUE;
		}
		else
		{
			SetTarget( NULL_ID, 0 );
			SendAIMsg( AIMSG_SETSTATE, pMover->m_dwAIInterfaceState );
		}
	///////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_PROCESS )
		MoveProcessRage();

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DAMAGE )
		m_idLastAttacker	= msg.dwParam1;
		AddDamage( msg.dwParam1, msg.dwParam2 );

		//   '일반 공격2' 추가 사항 : 불의 메테오니커 HP 80%, 50%, 30%,10%마다 '원킬'
		DWORD dwHPCur	= (DWORD)( ( pMover->GetHitPoint() / ( pMover->GetMaxHitPoint() * 0.01F ) ) );
		if( dwHPCur != m_dwHPOld )
		{
			switch( dwHPCur )
			{
				case 80:
				case 50:
				case 30:
				case 10:
					m_bCritical		= TRUE;					
					break;
			}
			m_dwHPOld	= dwHPCur;
		}
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DIE )
		SendAIMsg( AIMSG_EXIT );
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_COLLISION )
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_ARRIVAL )
		{
			DWORD dwItemId	= 0;
			MoverProp* pProp	= pMover->GetProp();
			// 준비된 공격 타입에 의한 공격 개시
			switch( m_dwAtk )
			{
				case OBJMSG_ATK1:	dwItemId	= pProp->dwAtk1;	break;
				case OBJMSG_ATK2:	dwItemId	= pProp->dwAtk2;	break;
				case OBJMSG_ATK3:	dwItemId	= pProp->dwAtk3;	break;
				case OBJMSG_ATK4:	dwItemId	= pProp->dwAtk4;	break;
			}
			if( pMover->DoAttackMelee( m_dwIdTarget, (OBJMSG)m_dwAtk, dwItemId ) == 1 )
			{
				// 연출
				m_tmAttack	= g_tmCurrent + s_tmAttack;	// 공격 후 20초간 추적해옴.
				m_dwAtk		= OBJMSG_NONE;
			}
		}
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_ATTACK_MELEE )
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_END_APPEAR )
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_END_MELEEATTACK )
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_EXIT )

	EndAIHandler()

	return TRUE;
}

BOOL CAIMeteonyker::MoveProcessRage( void )
{
	CMover* pMover	= GetMover();
	if( pMover->IsDie() || (pMover->m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL) )
		return FALSE;
	CMover* pTarget	= GetTarget( m_dwIdTarget ); 
	DWORD dwAIState	= pMover->m_dwAIInterfaceState;

	if( IsValidObj( pTarget ) == FALSE || ((CMover*)pTarget)->IsDie() )	
	{
		CMover* pTarget	= ScanTargetNext( pMover, 15/*nAttackFirstRange*/, m_dwIdTarget, m_uParty );
		if( pTarget	== NULL )
		{
			if( dwAIState == STATE_STAND )
				DoReturnToBegin();	// SetTarget( NULL_ID, 0 );
			SendAIMsg( AIMSG_SETSTATE, dwAIState );
		}
		else
		{
			SendAIMsg( AIMSG_SETSTATE, STATE_RAGE, pTarget->GetId() );
		}
		return TRUE;
	}
	if( pTarget->IsRegionAttr( RA_SAFETY ) )	// 타겟 안전 영역 內
	{
		DoReturnToBegin();
		SendAIMsg( AIMSG_SETSTATE, dwAIState );
		return TRUE;
	}

	if( pMover->IsFlyingNPC() != ((CMover*)pTarget)->m_pActMover->IsFly() && pMover->IsFlyingNPC() )	
	{
		if( dwAIState == STATE_STAND )
			DoReturnToBegin();
		SendAIMsg( AIMSG_SETSTATE, dwAIState );
		return TRUE;
	}
	if( g_tmCurrent > (DWORD)( m_tmAttack ) )
	{
		m_tmAttack	= g_tmCurrent + s_tmAttack;
		D3DXVECTOR3 vDist	= pMover->GetPos() - m_vOldPos;
		vDist.y	= 0;
		if( IsInRange( vDist, 2.0f ) )
		{
			if( pMover->GetSpeed( pMover->m_pActMover->m_fSpeed ) > 0 )
			{
				D3DXVECTOR3 vTarget	= pTarget->GetPos();
				pMover->SetPos( vTarget );
				g_UserMng.AddSetPos( pMover, pMover->GetPos() );
			} 
		} 
		else
		{
			m_tmReturnToBegin	= g_tmCurrent;
			if( dwAIState == STATE_STAND )
				DoReturnToBegin();
			SendAIMsg( AIMSG_SETSTATE, dwAIState );
			return TRUE;
		}
		m_vOldPos	= pMover->GetPos();
	}
	D3DXVECTOR3	vTargetTo = pTarget->GetPos() - pMover->GetPos();	
	
	D3DXVECTOR3 v1	= pMover->GetPos() - m_vPosBegin;
	FLOAT fRange	= RANGE_MOVE;
	/*
	if( IsInRange( v1, RANGE_RETURN_TO_BEGIN + fRange ) == FALSE )
	{
		DoReturnToBegin();
		SendAIMsg( AIMSG_SETSTATE, dwAIState );
		return TRUE;
	}
	*/
	if( m_vPosDamage == D3DXVECTOR3( 0, 0, 0 ) )
		m_vPosDamage	= pMover->GetPos();

	D3DXVECTOR3 v2	= pMover->GetPos() - m_vPosDamage;
	/*
	if( IsInRange( v2, RANGE_RETURN_TO_BEGIN ) == FALSE )
	{
		DoReturnToBegin();
		SendAIMsg( AIMSG_SETSTATE, dwAIState );
		return TRUE;
	}
	else 
	*/
	if( pTarget->GetType() == OT_MOVER && ((CMover *)pTarget)->IsMode( TRANSPARENT_MODE ) )
	{
		if( dwAIState == STATE_STAND )
			DoReturnToBegin();	// SetTarget( NULL_ID, 0 );
		SendAIMsg( AIMSG_SETSTATE, dwAIState );
		return TRUE;
	}
	else
	{
		// 공격 시작하면 공격 지연 재설정
		if( g_tmCurrent > (DWORD)( m_tmAttackDelay ) )
		{
			CMover* pMover	= GetMover();
			m_tmAttackDelay	= g_tmCurrent + SEC( 3 );

			 pMover->SetNotMode( ONEKILL_MODE );

			if( ProcessSPAttack() )
				return TRUE;
//			//	4. 자신의 HP가 30%이하가 되었을 시 1회에 한해 HP를 50% 회복 한다.
//			if( ProcessHeal() )
//				return TRUE;
			//	5. 자신의 HP가 2%가 되었을 시 모든 공격력 2배 증가	- propMoverEx.inc
			// 일반 공격
			if( m_dwIdTarget == NULL_ID )
				SelectTarget();
			if( m_dwIdTarget != NULL_ID )
			{
				m_dwAtk	= OBJMSG_ATK1;
				pMover->CMD_SetMeleeAttack( m_dwIdTarget, 0.0F );
			}
			else
			{
				if( dwAIState == STATE_STAND )
					DoReturnToBegin();
				SendAIMsg( AIMSG_SETSTATE, dwAIState );
				return TRUE;
			}
		}
	}
	return FALSE;
}

void CAIMeteonyker::AddDamage( OBJID idAttacker, int nDamage )
{
	if( nDamage == 0 )
		return;
	map<OBJID, int>::iterator	i = m_mapDamage.find( idAttacker );
	if( i == m_mapDamage.end() )
		m_mapDamage.insert( map<OBJID, int>::value_type( idAttacker, nDamage ) );
	else
		i->second	+= nDamage;
}

void CAIMeteonyker::ResetDamage( void )
{
	m_mapDamage.clear();
}

OBJID CAIMeteonyker::GetDealer( void )
{
	int nDamage	= 0;
	OBJID objid	= NULL_ID;
	for( map<OBJID, int>::iterator i = m_mapDamage.begin(); i != m_mapDamage.end(); ++i )
	{
		if( i->second >= nDamage )
		{
			CMover* pPlayer	= GetTarget( i->first );
			if( IsValidObj( pPlayer ) && pPlayer->IsLive() && GetMover()->GetWorld() == pPlayer->GetWorld() )
			{
				objid	= i->first;
				nDamage	= i->second;
			}
		}
	}
	return objid;
}
	
BOOL	CAIMeteonyker::ProcessRetaliation( void )
{
	if( --m_cbRetaliation <= 0 )
	{
		m_cbRetaliation		= CB_RETALIATION;
		OBJID objid	= GetDealer();
		if( objid != NULL_ID )
		{
			CMover* pMover	= GetMover();
			// Mvr_Meteonyker_dmg3.ani
			// 3번째 공격 패턴 시작
			// 대상은 objid
			CMover* pDealer	= GetTarget( objid );

			// 30초 최고 딜러 공격 신호
			char szText[256]	= { 0,};
			sprintf( szText, prj.GetText( TID_GAME_BOSS_METEONYKER_MSG_03 ), pDealer->GetName() );
			g_DPCoreClient.SendCaption( szText, pMover->GetWorld()->GetID() );	
			SetTarget( objid, pDealer->m_idparty );

			m_dwAtk		= OBJMSG_ATK3;
			pMover->CMD_SetMeleeAttack( m_dwIdTarget, 0.0F );
			
			//   '일반 공격2' 추가 사항 : 불의 메테오니커 HP 80%, 50%, 30%,10%마다 '원킬'
			if( m_bCritical )
			{
				pMover->SetMode( ONEKILL_MODE );
				m_bCritical	= FALSE;
			}
			ResetDamage();
			return TRUE;
		}
	}
	return FALSE;
}

BOOL	CAIMeteonyker::ProcessSummon( BOOL bUnconditional ) 
{
	CMover* pTarget	= GetTarget( m_dwIdTarget );
	if( IsValidObj( pTarget ) == FALSE || pTarget->IsDie() )
		return FALSE;

	// 30m 범위 내의 모든 캐릭터를 소환하여 공격한다. 매 공격 시 5%의 확률로 발동.
	DWORD dwRand	= xRandom( 1, 101 );	// 1 ~ 100
	if( bUnconditional || dwRand <= 5 )
	{
		// 여기서 공격 타입을 설정한다.
		CMover* pMover	= GetMover();
		CWorld* pWorld	= pMover->GetWorld();
		D3DXVECTOR3 vPos	= pMover->GetPos();

		FLOAT	fTheta	= D3DXToRadian( pMover->GetAngle() );
		D3DXVECTOR3 vDelta;
		vDelta.y	= 7.0F;
		vDelta.x	= sin( fTheta ) * 7.0F;
		vDelta.z	= -cos( fTheta ) * 7.0F;

		vPos	+= vDelta;

		D3DXVECTOR3 vDist;
		CMover* pPlayer	= NULL;
		CObj* pObj;

		// 반경 30 모든 사용자 소환
		FOR_LINKMAP( pWorld, vPos, pObj, 15, CObj::linkPlayer, pMover->GetLayer() )
		{
			pPlayer	= (CMover*)pObj;
			if( pPlayer->IsLive() )
			{
				// 플레이어 링크맵 기본 단위가 32미터이므로 거리 재검사
				D3DXVECTOR3 vDist	= pMover->GetPos() - pPlayer->GetPos();
				FLOAT fDistSq	= D3DXVec3LengthSq( &vDist );
				if( fDistSq <= 225.0F )
				{
					int nAttr = pWorld->GetHeightAttribute( vPos.x, vPos.z );
					if(nAttr == HATTR_NOWALK || nAttr == HATTR_NOMOVE)
					{
						vPos = pMover->GetPos();
						vPos.y += 7.0f;
					}

					pPlayer->REPLACE( g_uIdofMulti, pWorld->GetID(), vPos, REPLACE_NORMAL, pMover->GetLayer() );	// 일단 몬스터 좌표로 워프 시키자.
				}
			}
		}
		END_LINKMAP
		// Mvr_Meteonyker_dmg2.ani
		// 공격 패턴은 주변 사용자에 대한 광역이다.
		m_dwAtk		= OBJMSG_ATK2;
		MoveToDst( pMover->GetPos() );
//		pMover->PostAIMsg( AIMSG_ARRIVAL );
		return TRUE;
	}
	return FALSE;
}

BOOL	CAIMeteonyker::ProcessBurn( BOOL bUnconditional )
{
	CMover* pTarget	= GetTarget( m_dwIdTarget );
	if( IsValidObj( pTarget ) == FALSE || pTarget->IsDie() )
		return FALSE;

	// WdVolcaneRed 맵에 존재하는 모든 캐릭터에게 데미지를 주며, '화상'에 걸리게 한다. 메테오니커R 
	// 자신의 HP가 20%가 이하가 되었을 시 부터 매 공격 시 2% 확률로 발동된다.
	CMover* pMover	= GetMover();
	if( bUnconditional || pMover->GetHitPointPercent( 100 ) <= 20 )
	{
		DWORD dwRand	= xRandom( 1, 101 );	// 1 ~ 100
		if( bUnconditional || dwRand <= 2 )
		{
			// 광역 공격 X 130%
			// Mvr_Meteonyker_dmg4.ani 
			m_dwAtk	= OBJMSG_ATK4;
//			PostAIMsg( AIMSG_ARRIVAL );
			MoveToDst( pTarget->GetId() );
			return TRUE;
		}
	}
	return FALSE;
}

/*
BOOL	CAIMeteonyker::ProcessHeal( void )
{
	CMover* pMover	= GetMover();
	// 자신의 HP가 30%이하가 되었을 시 1회에 한해 HP를 50% 회복 한다.
	if( m_bHeal	== FALSE && pMover->GetHitPointPercent() <= 30 )
	{
		m_bHeal	= TRUE;
		int nHeal	= pMover->GetMaxHitPoint() / 2;
		pMover->IncHitPoint( nHeal );
		return TRUE;
	}
	return FALSE;
}
*/
void	CAIMeteonyker::ProcessRecovery( void )
{
// idle 상태에서만 동작
// 마지막 피격 60초 후 2초 당 50의 HP가 회복 된다.
//	m_tmAttack은 마지막 공격 후, 20초 경과 시간
//	time_t	tm	= m_tmAttack + SEC( 40 );	// 마지막 공격 후, 1분 경과 시간이다.
//	if( g_tmCurrent > tm )	// 마지막 공격 후, 1분이 경과 되었다면, 회복 가능하다.
//	{
		if( g_tmCurrent > (DWORD)( m_tmRecovery ) )	// 회복 틱 2초가 경과한 상태라면,
		{
			m_tmRecovery	= g_tmCurrent + SEC( 2 );
			CMover* pMover	= GetMover();
			pMover->IncHitPoint( 50 );
		}
//	}
}

BOOL CAIMeteonyker::SelectTarget( void )
{
	CMover	*pMover	= GetMover();
	CWorld	*pWorld	= GetWorld();
	CModelObject *pModel	= (CModelObject *)pMover->GetModel();

	FLOAT fRadius	= pMover->GetRadiusXZ();
	FLOAT fRadiusSq	= fRadius * fRadius;
	
	CMover *pLastAttacker	= GetTarget( m_idLastAttacker );
	if( IsValidObj( pLastAttacker ) == FALSE || pLastAttacker->IsDie()  )
	{
		m_idLastAttacker	= NULL_ID;
		pLastAttacker	= NULL;
	} 
	else
	{		
		D3DXVECTOR3 vDist	= pLastAttacker->GetPos() - pMover->GetPos();
		FLOAT fDistSq	= D3DXVec3LengthSq( &vDist );
		if( fDistSq >= fRadiusSq * 10.0f )
		{
			m_idLastAttacker	= NULL_ID;
			pLastAttacker	= NULL;
		}
	}

	SetTarget( NULL_ID, 0 );
	if( m_idLastAttacker == NULL_ID )
	{
		CMover* pTarget	= NULL;
		pTarget	= ScanTarget( pMover, 40, JOB_ALL );
		if( pTarget )
		{
			if( pMover->IsFlyingNPC() || pTarget->m_pActMover->IsFly() == FALSE )
				SetTarget( pTarget->GetId(), pTarget->m_idparty );
			else
				return FALSE;
		} 
		else
			return FALSE;
	}
	else
	{
		DWORD dwNum	= xRandom( 100 );		// 0 ~ 99까지으 난수.
		DWORD dwAggroRate	= 50;
		if( IsValidObj( pLastAttacker ) )
		{
			if( pLastAttacker->GetJob() == JOB_MERCENARY )
				dwAggroRate = 70;
		}
		if( dwNum < dwAggroRate )		
		{
			SetTarget( pLastAttacker->GetId(), pLastAttacker->m_idparty );
		}
		else if( dwNum < 75 )
		{
			CMover *pTarget	= ScanTargetStrong( pMover, 50 );		
			if( pTarget )
			{
				if( pMover->IsFlyingNPC() || pTarget->m_pActMover->IsFly() == FALSE )	
					SetTarget( pTarget->GetId(), pTarget->GetPartyId() );
				else
					SetTarget( pLastAttacker->GetId(), pLastAttacker->GetPartyId() );
			}
			else
				SetTarget( pLastAttacker->GetId(), pLastAttacker->GetPartyId() );
		}
		else if( dwNum < 100 )
		{
			CMover *pTarget = ScanTargetOverHealer( pMover, 50 );		
			if( pTarget )
			{
				if( pMover->IsFlyingNPC() || pTarget->m_pActMover->IsFly() == FALSE )	
					SetTarget( pTarget->GetId(), pTarget->GetPartyId() );
				else
					SetTarget( pLastAttacker->GetId(), pLastAttacker->GetPartyId() );
			}
			else
				SetTarget( pLastAttacker->GetId(), pLastAttacker->GetPartyId() );
		}
	}
	return TRUE;
}

void CAIMeteonyker::MoveToDst(	OBJID dwIdTarget )
{
	CMover* pMover = GetMover();
	if( pMover->GetDestId() == dwIdTarget )
		return;
//	GetMover()->SetSpeedFactor( 2.0F );

	CMover* pTarget	= GetTarget( dwIdTarget );
	if( IsValidObj( pTarget ) )
		SetTarget( pTarget->GetId(), pTarget->GetPartyId() );
	pMover->SetDestObj( dwIdTarget );
	g_UserMng.AddMoverSetDestObj( (CMover*)pMover, dwIdTarget );
}

void CAIMeteonyker::SetTarget( OBJID dwIdTarget, u_long uParty )
{
	m_dwIdTarget	= dwIdTarget;
	m_uParty	= uParty;
	if( dwIdTarget == NULL_ID )
		GetMover()->SetSpeedFactor( 1.0F );
}

BOOL CAIMeteonyker::ProcessSPAttack( void )
{
	++m_cbAttack;
	int nCount	= m_cbAttack % 20;
	if( nCount == 19 )	// 광역 예고
	{
		CMover* pMover	= GetMover();

		m_nSPAttackType	= xRand() % 2;
		if( m_nSPAttackType == 0 )
		{
			// 소환 공격 예고
			g_DPCoreClient.SendCaption( prj.GetText( TID_GAME_BOSS_METEONYKER_MSG_01 ), pMover->GetWorld()->GetID() );	
		}
		else
		{
			// 광역 버프 예고
			g_DPCoreClient.SendCaption( prj.GetText( TID_GAME_BOSS_METEONYKER_MSG_02 ), pMover->GetWorld()->GetID() );	
		}
	}
	else if( nCount == 0 )
	{
		if( m_nSPAttackType == 0 )
		{
			if( ProcessSummon( TRUE ) )
				return TRUE;
		}
		else
		{
			if( ProcessBurn( TRUE ) )
				return TRUE;
		}
	}
	if( ProcessRetaliation() )
		return TRUE;

	return FALSE;
}

CMover* CAIMeteonyker::GetTarget( OBJID objid )
{
	CMover* pTarget	= prj.GetMover( objid );
	if( IsValidObj( pTarget ) && GetMover()->GetWorld() == pTarget->GetWorld() )
		return pTarget;
	return NULL;
}

#endif	// __METEONYKER_0608