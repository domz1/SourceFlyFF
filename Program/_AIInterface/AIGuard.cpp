#include "stdafx.h"

#include "AIGuard.h"
#include "lang.h"
#include "defineObj.h"
#include "dialogMsg.h"
#include "User.h"
#include "dpcoreclient.h"
#include "Party.h"

extern	CUserMng		g_UserMng;
extern	CDPCoreClient	g_DPCoreClient;
extern	CPartyMng		g_PartyMng;


#define RANGE_MOVE   15.0f

enum
{
	STATE_INIT = 1,
	STATE_IDLE,
	STATE_WANDER,
	STATE_PURSUE,
	STATE_EVADE,
	STATE_RUNAWAY,
	STATE_RAGE
};
BEGIN_AISTATE_MAP( CAIGuard, CAIInterface )

	ON_STATE( STATE_INIT   , StateInit   )
	ON_STATE( STATE_IDLE   , StateIdle   )
	ON_STATE( STATE_WANDER , StateWander )
	ON_STATE( STATE_EVADE  , StateEvade  )
	ON_STATE( STATE_RAGE   , StateRage   )
	ON_STATE( STATE_RUNAWAY, StateRunaway  )

END_AISTATE_MAP()

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __VM_0820
#ifndef __MEM_TRACE
	#ifdef __VM_0819
	MemPooler<CAIGuard>*	CAIGuard::m_pPool		= new MemPooler<CAIGuard>( 16, "CAIGuard" );
	#else	// __VM_0819
	MemPooler<CAIGuard>*	CAIGuard::m_pPool		= new MemPooler<CAIGuard>( 16 );
	#endif	// __VM_0819
#endif	// __MEM_TRACE
#endif	// __VM_0820

static DWORD s_tmAttack = SEC(10);		//  m_tmAttack 10초 

CAIGuard::CAIGuard()
{
}
CAIGuard::CAIGuard( CObj* pObj ) : CAIMonster( pObj )
{
}

CAIGuard::~CAIGuard()
{
}

void CAIGuard::InitAI()
{
	// 상위클래스 InitAI()부르는지 확인할 것
}


BOOL CAIGuard::StateInit( const AIMSG & msg )
{
	return TRUE;
}

BOOL CAIGuard::MoveProcessIdle( const AIMSG & msg )
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	MoverProp *pProp = pMover->GetProp();

	// 데미지 상태거나 죽었을 경우 이동 처리 수행 않음 
	if( pMover->IsDie() || (pMover->m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL) )
		return FALSE;

/*
	if( 이동경로 있는가? )
	{
		비스트상의 이동경로 노드를 따라서 이동을 시킴(마을 순찰)
	} else
		현재 자리에서 대기;
*/

	{
		// 루팅옵션이 있는 몹이다.
/*		if( pProp->m_nLoot )
		{
			if( (pMover->GetCount() & 63) == 0 )		// 가끔씩 주변을 스캔해서.(63이면 약 4.2초)
			{	
				if( pProp->m_nLootProb == 0 || xRandom(100) < pProp->m_nLootProb  )	// 디폴트값(0)은 100% / 이거나 루팅확률에 들어가면 루팅시도
					if( m_bLootMove == FALSE )		// 루팅하러 가고 있을때 또 체크하면 안뒘
						SubItemLoot();		// 아이템을 루팅함.
			}
		}

		if( m_bLootMove == TRUE )
		{
			CCtrl *pCtrl = prj.GetCtrl( m_idLootItem );		// 그아이템이 사라졌을수 있으니까 검사함.
			if( IsInvalidObj(pCtrl) )		// 아이템 집으러 이동중에 아템이 없어지면
			{
				MoveToDst( pMover->GetPos() );	// 제자리에 섬.
			}
		} */

		// 선공속성이면 주변을 늘 감시한다.
		if( GetMover()->m_bActiveAttack )
		{
			int nAttackFirstRange = pMover->GetProp()->m_nAttackFirstRange;

			if( pMover->GetProp()->m_nAttackFirstRange > 10 || pMover->GetProp()->m_nAttackFirstRange <= 0 )
			{
				Error( "m_nAttackFirstRange 이상:%d %s\n", pMover->GetProp()->m_nAttackFirstRange, pMover->GetName() );
				return TRUE;
			}

			MoverProp *pProp = GetMover()->GetProp();
			CMover* pTarget = ScanTarget( pMover, nAttackFirstRange, pProp->m_nScanJob );		// 자기 카르마와 반대되는 유저 검색기능을 추가.
			if( IsValidObj( (CObj*)pTarget ) )
			{
				// this가 비행형 몬스터거나 || 타겟이 비행중이 아닐때만 공격.
				if( pMover->IsFlyingNPC() || pTarget->m_pActMover->IsFly() == FALSE )	
				{
					m_dwIdTarget = pTarget->GetId();
					// 타겟이 존재하면 추적 모드로 변경 
					if( m_dwIdTarget != NULL_ID )	
					{
						g_UserMng.AddDlgEmoticon( pMover, DLGEMOT_ATTACK );
						SendAIMsg( AIMSG_SETSTATE, STATE_RAGE, NULL_ID );
					}
				}
			}
		}
	}
	return TRUE;
}

BOOL CAIGuard::StateIdle( const AIMSG & msg )
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();

	BeginAIHandler( )

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT ) 
		float fRangeMove = RANGE_MOVE;
		if( pMover->IsFlyingNPC() )
			fRangeMove *= 3.0f;
		D3DXVECTOR3	vTargetTo = pMover->GetPos() - m_vPosBegin;
		if( IsInRange( vTargetTo, fRangeMove ) == FALSE )
			m_bReturnToBegin = TRUE;
		SetStop( SEC( 0 ) );
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT_TARGETCLEAR )
		m_dwIdTarget = NULL_ID;
			
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_PROCESS ) 
		MoveProcessIdle( msg );

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DAMAGE ) 
		//분노 모드로 변경!
		if( m_bReturnToBegin == FALSE || (m_bReturnToBegin && (int)(g_tmCurrent - m_tmReturnToBegin) > SEC(5)) )
		{
			m_dwIdTarget = msg.dwParam1;
			MoveToDst( m_dwIdTarget );
			SendAIMsg( AIMSG_SETSTATE, STATE_RAGE, NULL_ID );
			m_tmAttack = timeGetTime() + s_tmAttack;	// 대기중일때 공격받으면 10초 타이머 시작
		}


	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DIE ) 
		SendAIMsg( AIMSG_EXIT );

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_COLLISION )
		MoveToRandom( STATE_IDLE );

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_ARRIVAL )
		if( pMover->IsFlyingNPC() )		
		{
			// 비행몹은 도착하고 나서 멈추지 않는다. 8자비행으로 이동패턴이 바뀐다.
			if( msg.dwParam2 == 1 )
			{
				int nPattern = 2;
				pMover->SetMovePattern( nPattern );
				// 클라로 전송
				g_UserMng.AddSetMovePattern( pMover, nPattern, pMover->GetPos(), pMover->GetAngle(), pMover->GetAngleX() );
			}
		} else
		{
			if( m_bReturnToBegin )
				SetStop( xRandom( SEC( 1 ) ) ); // 1초 동안 멈춰라 
			else
				SetStop( SEC( 5 ) + xRandom( SEC( 1 ) ) ); // 1초 동안 멈춰라 

			// --------
			// 이부분은 CAIMonster의 루틴을 그대로 쓸수 있게 하자.
/*			if( m_bLootMove )	// 아이템 루팅모드였음.
			{
				CCtrl *pCtrl = prj.GetCtrl( m_idLootItem );		// 그아이템이 사라졌을수 있으니까 검사함.
				if( IsValidObj(pCtrl) )
				{
					CItem *pItem = (CItem *)pCtrl;
					D3DXVECTOR3 vDist = pCtrl->GetPos() - pMover->GetPos();
					FLOAT fDistSq = D3DXVec3LengthSq( &vDist );
					if( fDistSq < 3.0f * 3.0f )		// 도착해서 거리 검사 한번더 해서 
					{
						pItem->Lock( theLineFile );	// lock
						if( pItem->IsDelete() )
						{
							pItem->Unlock( theLineFile );	// unlock
							return TRUE;
						}
						CItemElem* pItemElem = (CItemElem *)pItem->m_pItemBase;
						ItemProp *pItemProp = pItem->GetProp();
						BOOL bSuccess = FALSE;
						if( pItemProp->dwItemKind1 == IK1_GOLD )	// 주은아이템이 돈이냐?
						{
							//pMover->SetPointParam( DST_GOLD, pMover->GetGold() + pItemElem->m_nItemNum );	// 돈증가.
							pMover->AddGold( pItemElem->m_nItemNum );	// 돈증가.
							bSuccess = TRUE;
						} else
						// not GOLD
						{
							bSuccess = pMover->CreateItem( pItemElem );			// 주은 아이템 생성.
						}

						if( bSuccess )
						{
							pItem->Delete();		// 인벤에까지 넣는데 성공하면 바닥의 아이템은 지움.
						}
						pItem->Unlock( theLineFile );	// unlock
					}
				}
				m_bLootMove = FALSE;		// this가 아이템 집는중이라는거 해제.
				m_idLootItem = NULL_ID;
			} */
		}
		
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_EXIT )	

	EndAIHandler( )
	return TRUE;
}

BOOL CAIGuard::StateWander( const AIMSG & msg )
	{	return TRUE;	}
BOOL CAIGuard::StateEvade( const AIMSG & msg )
	{	return TRUE;	}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////



// 전투모드에서의 프로세스.
BOOL CAIGuard::MoveProcessRage( const AIMSG & msg )
{
	CAIMonster::MoveProcessRage( msg );		// 몬스터의 전투패턴을 그대로 사용.
	return TRUE;
}

BOOL CAIGuard::StateRage( const AIMSG & msg )
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	if( IsInvalidObj(pMover) )	return FALSE;

	BeginAIHandler( )

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT ) 
		if( msg.dwParam2 != NULL_ID )
			m_dwIdTarget = msg.dwParam2;
		m_tmAttack = timeGetTime() + s_tmAttack;
		m_tmAttackDelay = timeGetTime(); // 공격 딜레이를 초기화. 그렇지 않으면 처음 공격이 딜레이 먹고 들어간다.
		SetStop( 0 ); // 0으로 세팅하면 정지 상태가 바로 끝나게 되어 공격이 가능하게 된다. 
		              // 정지 상태에서는 정지 상태를 일정 시간동안 유지해주어야하는데, 그렇게 되면 공격을 못할 수 있다.

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT_TARGETCLEAR )		// 타겟을 클리어하고 대기모드로 돌아감.
		m_dwIdTarget = NULL_ID;
		SendAIMsg( AIMSG_SETSTATE, STATE_IDLE );
		
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_PROCESS ) 
		MoveProcessRage( msg );
		
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DAMAGE ) 
		if( IsMove() && m_bGoTarget == FALSE )
		{
			if( xRandom( 2 ) == 0 )		// 1/2확률로 공격자를 때림.
			{
				m_dwIdTarget = msg.dwParam1;
				// 공격자에게 목표를 설정 
				MoveToDst( m_dwIdTarget );
			}
			m_bGoTarget = TRUE;
		}
		MoverProp* pMoverProp = pMover->GetProp();
		int nHPPercent = pMover->GetHitPointPercent( 100 );
		// 도주 HP 퍼센트보다 현재 HP퍼센트가 작을 경우 도망가자.
		if( nHPPercent < pMoverProp->m_nRunawayHP )
		{
			// 도주 모드로 세팅 
			SendAIMsg( AIMSG_SETSTATE, STATE_RUNAWAY );
			g_UserMng.AddDlgEmoticon( pMover, DLGEMOT_EVADE );
		}
		// Call HP 퍼센트보다 현재 HP퍼센트가 작을 경우 동료를 부르자.
		if( pMoverProp->m_bHelpWho )	// 헬프AI가 있을경우.
		{
			if( timeGetTime() - m_tmHelp > pMoverProp->m_tmUnitHelp )	// 최초 m_tmHelp는 0이므로 헬프를 한다.
			{
				if( m_bCallHelper == FALSE )
				{
					CallHelper( pMoverProp );
					g_UserMng.AddDlgEmoticon( pMover, DLGEMOT_HELPER );
					m_tmHelp = timeGetTime();
					if( pMoverProp->m_tmUnitHelp == 0 )		// 0이면 한번만 부르고 안부름.
						m_bCallHelper = TRUE;
				}
			}
		}
		else
			m_bCallHelper = FALSE;
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_COLLISION )
		MoveToRandom( STATE_RAGE );
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DIE ) 
	
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DSTDIE ) 
		if( m_dwIdTarget == msg.dwParam1 )
			m_dwIdTarget = NULL_ID;
		TRACE( "TARGET CLEARED\n" );
		SendAIMsg( AIMSG_SETSTATE, STATE_IDLE );

	OnMessage( AIMSG_BEGINMOVE )
		m_bGoTarget = FALSE; 

	OnMessage( AIMSG_ARRIVAL )

	OnMessage( AIMSG_EXIT )	

	EndAIHandler( )
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CAIGuard::MoveProcessRunaway()
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();

	// 데미지 상태거나 죽었을 경우 이동 처리 수행 않음 
	if( pMover->IsDie() || (pMover->m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL))
		return FALSE;

	CCtrl* pTarget = prj.GetCtrl( m_dwIdTarget ); // 목표를 재설정해준다.

	// 타겟이 무효인가?
	if( IsValidObj( pTarget ) == FALSE )
	{
		m_dwIdTarget = NULL_ID;
		SendAIMsg( AIMSG_SETSTATE, STATE_IDLE );
	}
	return TRUE;
}

BOOL CAIGuard::StateRunaway( const AIMSG & msg )
{

	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();

	BeginAIHandler( )

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT ) 
		// 이경우 정상적인 옵션에 의한 도주가 아니라, 상대가 공격할 수 없는, 지리상 잇점이 있는 장소에
		// 있기 때문에 줄행랑을 치는 경우다. 
		m_bTargetNoMovePos	= msg.dwParam2;
		// 현재의 반대 방향으로 도주 시작 
		// 5도 폭을 가지고 각도 변경. 폭이 크면 클수록 지그재그 형태로 움직인다.
		pMover->AddAngle( (float)( 180 + ( 20 - xRandom( 40 ) ) ) );
		// 각도를 꺼낸 후에 연장선에 있는 좌표를 구한다.
		CMover* pMoverTarget = (CMover*)prj.GetCtrl( m_dwIdTarget ); // 목표를 재설정해준다.
		if( IsValidObj(pMoverTarget) )
		{
			DWORD dwAttackRange = pMoverTarget->GetActiveHandItemProp()->dwAttackRange;
			FLOAT fRange = pMoverTarget->GetAttackRange( dwAttackRange ) * 1.7f;
			FLOAT fAngle = pMover->GetAngle();
			D3DXVECTOR3 vDst = pMover->GetPos() + VelocityToVec( fAngle, fRange );
			MoveToDst( vDst );
//			SendAIMsg( AIMSG_SETPROCESS, TRUE );
			m_bFirstRunaway = TRUE; // 처음 도주인가?
		}
		else
			SendAIMsg( AIMSG_SETSTATE, STATE_IDLE );
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_PROCESS ) 
		MoveProcessRunaway();
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DAMAGE ) 
		if( m_bTargetNoMovePos == TRUE && m_bFirstRunaway == FALSE )
			SendAIMsg( AIMSG_SETSTATE, STATE_RAGE, NULL_ID );

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_COLLISION )
		// 현재의 반대 방향으로 도주 시작 
		// 5도 폭을 가지고 각도 변경. 폭이 크면 클수록 지그재그 형태로 움직인다.
		pMover->AddAngle( (float)( 180 + ( 20 - xRandom( 40 ) ) ) );
		// 각도를 꺼낸 후에 연장선에 있는 좌표를 구한다.
		FLOAT fAngle = pMover->GetAngle();
		D3DXVECTOR3 vDst = pMover->GetPos() + VelocityToVec( fAngle, 5 );
		MoveToDst( vDst );
		// m_bFirstRunaway = FALSE; <-- 04.10.08 데드락을 유발하는 것으로 생각됨.
	
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DIE ) 
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DSTDIE ) 
		if( m_dwIdTarget == msg.dwParam1 )
			m_dwIdTarget = NULL_ID;
		SendAIMsg( AIMSG_SETSTATE, STATE_IDLE );

	OnMessage( AIMSG_ARRIVAL )
		m_bFirstRunaway = FALSE;
		//SetStop( xRandom( SEC( 2 ) ) + SEC( 1 ) ); // 2초~3초 동안 멈춰라 
		MoverProp* pMoverProp = pMover->GetProp();
		if( pMoverProp->m_dwRunawayDelay )
			SetStop( pMover->GetProp()->m_dwRunawayDelay + xRandom( SEC( 1 ) ) ); // + xRandom은 약간의 가변 차이를 주기 위한 것.

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_EXIT )	
//		SendAIMsg( AIMSG_SETPROCESS, FALSE );

	EndAIHandler( )

	return TRUE;
}

