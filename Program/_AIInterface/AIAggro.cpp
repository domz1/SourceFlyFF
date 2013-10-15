#include "stdafx.h"
#include "AIAggro.h"
#include "lang.h"
#include "defineObj.h"
#include "dialogMsg.h"
#include "User.h"
#include "dpcoreclient.h"
#include "Party.h"
#include "DefineSkill.h"
#include "PathObj.h"



extern	CUserMng		g_UserMng;
extern	CDPCoreClient	g_DPCoreClient;
extern	CPartyMng		g_PartyMng;

#ifdef __AI_AGGRO

#define RANGE_CANCEL	120.0f
#define RANGE_MOVE	30.0f

#define	RANGE_RETURN_TO_BEGIN	120.0F



#define	TIMEGETTIME		g_tmCurrent

enum
{
	STATE_INIT = 1,
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

BEGIN_AISTATE_MAP( CAIAggro, CAIInterface )

	ON_STATE( STATE_INIT   , StateInit   )
	ON_STATE( STATE_IDLE   , StateIdle   )
	ON_STATE( STATE_WANDER , StateWander )
	ON_STATE( STATE_EVADE  , StateEvade  )
	ON_STATE( STATE_RAGE   , StateRage   )
	ON_STATE( STATE_RUNAWAY, StateRunaway  )
	ON_STATE( STATE_STAND  , StateStand   )
	ON_STATE( STATE_PATROL , StatePatrol  )
	ON_STATE( STATE_RAGE_PATROL , StateRagePatrol  )
	
END_AISTATE_MAP()

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	const int INITIAL_AI_MONSTER = 1024*5;

#ifndef __VM_0820
#ifndef __MEM_TRACE
	#ifdef __VM_0819
	MemPooler<CAIAggro>*	CAIAggro::m_pPool		= new MemPooler<CAIAggro>( INITIAL_AI_MONSTER, "CAIAggro" );
	#else	// __VM_0819
	MemPooler<CAIAggro>*	CAIAggro::m_pPool		= new MemPooler<CAIAggro>( INITIAL_AI_MONSTER );
	#endif	// __VM_0819
#endif	// __MEM_TRACE
#endif	// __VM_0820

static DWORD s_tmAttack = SEC(15);

CAIAggro::CAIAggro()
{
	m_dwAIInterface = 0;
	Init();
}

CAIAggro::CAIAggro( CObj* pObj, DWORD dwAIInterface ) : CAIInterface( pObj )
{
	m_dwAIInterface = dwAIInterface;
	Init();
}

void CAIAggro::Init()
{
	SetTarget( NULL_ID, 0 );
	m_bReturnToBegin	= FALSE;
	m_bGoTarget		= FALSE;
	m_bTargetNoMovePos	= FALSE;
	m_dwAtkMethod	= 0;
	m_bFirstRunaway		= FALSE;
	m_bCallHelper	= FALSE;
	m_tmAttack	= TIMEGETTIME + s_tmAttack;
	m_vOldPos.x	= m_vOldPos.y = m_vOldPos.z = 0;
	m_bRangeAttack	= FALSE;

	m_tmSummon = TIMEGETTIME;
	m_tmHelp = 0;

	m_bLootMove	= FALSE;
	m_idLootItem	= NULL_ID;
	m_tmAttackDelay	= m_tmMove = m_tmReturnToBegin = TIMEGETTIME;
	m_fAngleBegine	= 0.0f;

	m_vPosDamage	= D3DXVECTOR3( 0, 0, 0 );
}

CAIAggro::~CAIAggro()
{
}

void CAIAggro::InitAI()
{
	m_vPosBegin = GetMover()->GetPos();
	m_fAngleBegine = GetMover()->GetAngle();
	PostAIMsg( AIMSG_SETSTATE, GetMover()->m_dwAIInterfaceState ); 	
}

void CAIAggro::SetStop( DWORD dwTime ) 
{ 
	m_tmMove = TIMEGETTIME + dwTime;
	GetMover()->SetStop(); 
} 

BOOL CAIAggro::IsEndStop() 
{ 
	return ( TIMEGETTIME > m_tmMove ) && ( IsMove() == FALSE ) ; 
}

void CAIAggro::MoveToDst(	const D3DXVECTOR3& vDst )
{
	CMover* pMover = GetMover();
	if( pMover->GetDestPos() == vDst )
		return;

	pMover->SetDestPos( vDst );
	pMover->m_nCorr		= -1;
	g_UserMng.AddSetDestPos( pMover, vDst, 1 );
}

void CAIAggro::MoveToDst(	OBJID dwIdTarget )
{
	CMover* pMover = GetMover();
	if( pMover->GetDestId() == dwIdTarget )
		return;

	GetMover()->SetSpeedFactor( 2.0F );

	CMover* pTarget	= prj.GetMover( dwIdTarget );
	if( IsValidObj( pTarget ) )
		SetTarget( pTarget->GetId(), pTarget->m_idparty );

	pMover->SetDestObj( dwIdTarget );
	g_UserMng.AddMoverSetDestObj( (CMover*)pMover, dwIdTarget );
}

void CAIAggro::MoveToRandom( UINT nState )
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();

	D3DXVECTOR3 vPos = pMover->GetPos();
	DWORD x1 = xRand();
	DWORD x2 = xRand();

	vPos.x += int( x1 % 21 ) - 10;
	vPos.z += int( x2 % 21 ) - 10;

	if( nState == STATE_IDLE )
	{
		D3DXVECTOR3 vDistant = vPos - m_vPosBegin;
		vDistant.y = 0.0f;

		float fRange = RANGE_MOVE;
		if( pMover->IsFlyingNPC() )			
			fRange *= 3.0f;
		
		if( IsInRange( vDistant, fRange ) )	
		{
			MoveToDst( vPos );
		}
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

BOOL CAIAggro::IsInRange( D3DXVECTOR3& vDistant, FLOAT fRange )
{
	vDistant.y	= 0.0f;
	float fDistSq = D3DXVec3LengthSq( &vDistant );	
	if( fDistSq > fRange * fRange )	
		return FALSE;
	return TRUE;
}

BOOL CAIAggro::SubItemLoot()
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	MoverProp *pProp = pMover->GetProp();
	D3DXVECTOR3 vPos = pMover->GetPos();
	CObj *pObj = NULL;
	int nRange = 0;
	D3DXVECTOR3 vDist;
	FLOAT fDistSq;
	CObj *pMinObj = NULL;

	FOR_LINKMAP( pWorld, vPos, pObj, nRange, CObj::linkDynamic, pMover->GetLayer() )
	{
		if( pObj->GetType() == OT_ITEM )
		{
			ItemProp* pItemProp	= ( (CItem*)pObj )->GetProp();
			if( pItemProp && pItemProp->dwItemKind3 != IK3_QUEST && pItemProp->dwItemKind1 != IK1_GOLD
#ifdef __EVENT_0117
				&& pItemProp->dwID != II_SYS_SYS_SCR_BXPIG
				&& pItemProp->dwID != II_SYS_SYS_SCR_BXPIG_TWN
#endif	// __EVENT_0117
				)
			{
				vDist = pObj->GetPos() - pMover->GetPos();
				fDistSq = D3DXVec3LengthSq( &vDist );
				if( fDistSq < 10.0f * 10.0f )
				{
					pMinObj = pObj;			
					goto NEXT;
				}
			}
		}
	}
	END_LINKMAP

NEXT:
	if( pMinObj )
	{
		MoveToDst( pMinObj->GetPos() );
		m_idLootItem = ((CItem *)pMinObj)->GetId();
		m_bLootMove = TRUE;
		g_UserMng.AddDlgEmoticon( pMover, DLGEMOT_LOOT );
	}

	return TRUE;
}

BOOL CAIAggro::StateInit( const AIMSG & msg )
{
	return TRUE;
}

void CAIAggro::DoReturnToBegin( BOOL bReturn )
{
#ifndef __CLIENT
	if( m_dwIdTarget != NULL_ID && m_dwIdTarget != NULL )
	{
		CMover* pAggro = prj.GetMover( m_dwIdTarget );
		if( pAggro && pAggro->IsPlayer() )
			pAggro->SetAggro( pAggro->GetAggro() - 1 );
	}
#endif
	if( bReturn )
	{
		m_bReturnToBegin	= TRUE;
		m_tmReturnToBegin	= TIMEGETTIME;
		SetTarget( NULL_ID, 0 );

		CMover* pMover	= GetMover();
		pMover->SetSpeedFactor( 2.66F );

		MoveToDst( m_vPosBegin );
	}
	else
	{
		m_bReturnToBegin	= FALSE;
		SetTarget( NULL_ID, 0 );

		CMover* pMover	= GetMover();
		pMover->SetPointParam( DST_HP, pMover->GetMaxHitPoint() );
		pMover->RemoveAllEnemies();
	}
}

BOOL CAIAggro::MoveProcessIdle( const AIMSG & msg )
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	MoverProp *pProp = pMover->GetProp();

	if( pMover->IsDie() || (pMover->m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL) )
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
		if( IsInRange( m_vPosBegin - pMover->GetPos(), 7.0f ) )
		{
			DoReturnToBegin( FALSE );
			SendAIMsg( AIMSG_ARRIVAL, NULL_ID );
		}


		if( TIMEGETTIME - m_tmReturnToBegin >= SEC(20) )
		{
			pMover->SetPos( m_vPosBegin );
			g_UserMng.AddSetPos( pMover, m_vPosBegin );
			m_tmReturnToBegin	= TIMEGETTIME;
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

		if( pProp->m_nLoot )
		{
			if( (pMover->GetCount() & 63) == 0 )
			{	
				if(  pProp->m_nLootProb == 0 || xRandom(100) < (DWORD)( pProp->m_nLootProb )  )
					if( m_bLootMove == FALSE )	
						SubItemLoot();	
			}
		}

		if( m_bLootMove == TRUE )
		{
			CCtrl *pCtrl = prj.GetCtrl( m_idLootItem );	
			if( IsInvalidObj(pCtrl) )	
			{
				MoveToDst( pMover->GetPos() );
				m_idLootItem = NULL_ID;
				m_bLootMove = FALSE;
			}
		}

		if( pProp->m_nScanJob != -1 )
		{
			CMover* pTarget = NULL;
			int nAttackFirstRange = pProp->m_nAttackFirstRange;

			if( nAttackFirstRange <= 0 )
			{
				Error( "m_nAttackFirstRange 이상:%d %s\n", pProp->m_nAttackFirstRange, pMover->GetName() );
				return TRUE;
			}

			if( pProp->m_nScanJob || pProp->m_dwScanQuestId || pProp->m_dwScanItemIdx || pProp->m_nScanChao )
				pTarget = ScanTargetAggro( pMover, (FLOAT)nAttackFirstRange, m_dwAIInterface, m_uParty );

			if( pTarget == NULL && GetMover()->m_bActiveAttack )
				pTarget = ScanTargetAggro( pMover, (FLOAT)nAttackFirstRange, m_dwAIInterface, m_uParty );

			if( pTarget )
			{
				if( IsValidObj( (CObj*)pTarget ) )
				{

#ifdef __JHMA_VER_8_6   
					if( pMover->IsFlyingNPC() == FALSE || pMover->IsFlyingNPC() == pTarget->m_pActMover->IsFly() || pProp->dwClass == RANK_GUARD )	// 가드는 무조건 공격.
#else	// __VER >= 8  
					if( (pMover->IsFlyingNPC() || pTarget->m_pActMover->IsFly() == FALSE) || pProp->dwClass == RANK_GUARD )	// 가드는 무조건 공격.
#endif	// __VER >= 8  
					{
						if( pTarget )	
						{
							g_UserMng.AddDlgEmoticon( pMover, DLGEMOT_ATTACK );
							SendAIMsg( AIMSG_SETSTATE, STATE_RAGE, pTarget->GetId() );
						}
					}
				}
			}	// if( target)
		}	// if( scan )

	}
	return TRUE;
}

BOOL CAIAggro::StateIdle( const AIMSG & msg )
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();

	BeginAIHandler( )

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT ) 
		int nRangeMove = (int)( RANGE_MOVE );
		if( pMover->IsFlyingNPC() )
			nRangeMove *= 3;

		D3DXVECTOR3	vTargetTo = pMover->GetPos() - m_vPosBegin;
		if( IsInRange( vTargetTo, (float)( nRangeMove ) ) == FALSE )
			DoReturnToBegin();
		SetStop( SEC( 0 ) );

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT_TARGETCLEAR )
		SetTarget( NULL_ID, 0 );

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_PROCESS ) 
		MoveProcessIdle( msg );

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DAMAGE ) 

		if( m_bReturnToBegin == FALSE )
		{
			MoveToDst( msg.dwParam1 );
			SendAIMsg( AIMSG_SETSTATE, STATE_RAGE, msg.dwParam1 );
			m_tmAttack	= TIMEGETTIME + s_tmAttack;
			m_vPosDamage	= pMover->GetPos();
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
			if( msg.dwParam2 == 1 )
			{
				int nPattern = 2;
				pMover->SetMovePattern( nPattern );
				g_UserMng.AddSetMovePattern( pMover, nPattern, pMover->GetPos(), pMover->GetAngle(), pMover->GetAngleX() );
			}
		} 
		else
		{
			if( m_bReturnToBegin )
				SetStop( xRandom( SEC( 1 ) ) );
			else
				SetStop( SEC( 5 ) + xRandom( SEC( 1 ) ) );

			if( m_bLootMove )	
			{
				CCtrl *pCtrl = prj.GetCtrl( m_idLootItem );	
				if( IsValidObj(pCtrl) )
				{
					CItem *pItem = (CItem *)pCtrl;
					D3DXVECTOR3 vDist = pCtrl->GetPos() - pMover->GetPos();
					FLOAT fDistSq = D3DXVec3LengthSq( &vDist );
					if( fDistSq < 3.0f * 3.0f )
					{
						if( pItem->IsDelete() )
							return TRUE;
						CItemElem* pItemElem = (CItemElem *)pItem->m_pItemBase;
						ItemProp *pItemProp = pItem->GetProp();
						BOOL bSuccess = FALSE;
						if( pItemProp->dwItemKind1 == IK1_GOLD )
						{
							pMover->AddGold( pItemElem->GetGold() );
							bSuccess = TRUE;
						} 
						else
						{
							bSuccess = pMover->CreateItem( pItemElem );
						}

						if( bSuccess )
						{
							pItem->Delete();
						}
					}
				}
				m_bLootMove = FALSE;
				m_idLootItem = NULL_ID;
			}
		}
		
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_EXIT )	

	EndAIHandler( )
	return TRUE;
}

BOOL CAIAggro::StateWander( const AIMSG & msg )
	{	return TRUE;	}
BOOL CAIAggro::StateEvade( const AIMSG & msg )
	{	return TRUE;	}

BOOL CAIAggro::StateStand( const AIMSG & msg )
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();

	BeginAIHandler( )

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT ) 
		D3DXVECTOR3	vTargetTo = pMover->GetPos() - m_vPosBegin;
		int nRangeMove = (int)( RANGE_MOVE );
		if( pMover->IsFlyingNPC() )
			nRangeMove *= 3;
		if( IsInRange( vTargetTo, (float)( nRangeMove ) ) == FALSE )
			DoReturnToBegin();
		
		SetStop( SEC( 0 ) );

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT_TARGETCLEAR )
		SetTarget( NULL_ID, 0 );
			
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_PROCESS ) 
		MoveProcessStand( msg );

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DAMAGE ) 


		if( m_bReturnToBegin == FALSE )
		{
			MoveToDst( msg.dwParam1 );
			SendAIMsg( AIMSG_SETSTATE, STATE_RAGE, msg.dwParam1 );
			m_tmAttack	= TIMEGETTIME + s_tmAttack;	
			m_vPosDamage	= pMover->GetPos();
		}


	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DIE )
		SendAIMsg( AIMSG_EXIT );

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_COLLISION )

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_ARRIVAL )
		if( pMover->IsFlyingNPC() )		
		{
			if( msg.dwParam2 == 1 )
			{
				int nPattern = 2;
				pMover->SetMovePattern( nPattern );
				g_UserMng.AddSetMovePattern( pMover, nPattern, pMover->GetPos(), pMover->GetAngle(), pMover->GetAngleX() );
			}
		} 
		else
		{
			if( m_bReturnToBegin )
			{
				SetStop( xRandom( SEC( 1 ) ) );
			}
			else
			{
				SetStop( SEC( 5 ) + xRandom( SEC( 1 ) ) );
			}

			if( m_bLootMove )
			{
				CCtrl *pCtrl = prj.GetCtrl( m_idLootItem );	
				if( IsValidObj(pCtrl) )
				{
					CItem *pItem = (CItem *)pCtrl;
					D3DXVECTOR3 vDist = pCtrl->GetPos() - pMover->GetPos();
					FLOAT fDistSq = D3DXVec3LengthSq( &vDist );
					if( fDistSq < 3.0f * 3.0f )	
					{
						if( pItem->IsDelete() )
							return TRUE;
						CItemElem* pItemElem = (CItemElem *)pItem->m_pItemBase;
						ItemProp *pItemProp = pItem->GetProp();
						BOOL bSuccess = FALSE;
						if( pItemProp->dwItemKind1 == IK1_GOLD )	
						{
							pMover->AddGold( pItemElem->GetGold() );	
							bSuccess = TRUE;
						} 
						else
						{
							bSuccess = pMover->CreateItem( pItemElem );		
						}

						if( bSuccess )
						{
							pItem->Delete();
						}
					}
				}
				m_bLootMove = FALSE;
				m_idLootItem = NULL_ID;
			}
		}
		
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_EXIT )	

	EndAIHandler( )
	return TRUE;
}
BOOL CAIAggro::StatePatrol( const AIMSG & msg )
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();

	BeginAIHandler( )

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT ) 
		D3DXVECTOR3	vTargetTo	= pMover->GetPos() - m_vPosBegin;
		int nRangeMove	= (int)( RANGE_MOVE );
		if( pMover->IsFlyingNPC() )
			nRangeMove	*= 3;
		if( IsInRange( vTargetTo, (float)( nRangeMove ) ) == FALSE )
			DoReturnToBegin();
		SetStop( SEC( 0 ) );
		if( pMover->m_dwPatrolIndex != NULL_ID && m_bReturnToBegin == FALSE )
		{
			CPatrolPath::GetInstance()->GetNextPosInfo( (CObj*)pMover, pMover->GetPos(), m_vDestPos, m_vecPatrolInfo );
			g_UserMng.AddMotionArrive( pMover, (m_vecPatrolInfo.m_dwDelay == 0 ) ? OBJMSG_STOP_RUN: OBJMSG_STOP );
			m_vPosBegin	= m_vDestPos;			
		}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT_TARGETCLEAR )
		SetTarget( NULL_ID, 0 );
			
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_PROCESS ) 
		MoveProcessPatrol( msg );

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_TIMEOUT )
		
		
	///////////////////////////////////////////////////////////////////////////////////////////////////			
	OnMessage( AIMSG_DAMAGE ) 

		if( m_bReturnToBegin == FALSE )
		{
			MoveToDst( msg.dwParam1 );
			SendAIMsg( AIMSG_SETSTATE, STATE_RAGE, msg.dwParam1 );
			m_tmAttack	= TIMEGETTIME + s_tmAttack;
			m_vPosDamage	= pMover->GetPos();
		}


	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DIE )	
		SendAIMsg( AIMSG_EXIT );

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_COLLISION )

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_ARRIVAL )
	{
		g_UserMng.AddMotionArrive( pMover, (m_vecPatrolInfo.m_dwDelay == 0 ) ? OBJMSG_STOP_RUN: OBJMSG_STOP );
		SetStop( SEC(m_vecPatrolInfo.m_dwDelay) );
		D3DXVECTOR3 v3Pos = m_vDestPos;
		CPatrolPath::GetInstance()->GetNextPosInfo( (CObj*)pMover, v3Pos, m_vDestPos, m_vecPatrolInfo );
		m_vPosBegin = m_vDestPos;			
	}
		
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_EXIT )	

	EndAIHandler( )
	return TRUE;
}

BOOL CAIAggro::MoveProcessStand( const AIMSG & msg )
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	MoverProp *pProp = pMover->GetProp();

	if( pMover->IsDie() || (pMover->m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL) )
		return FALSE;

	if( m_bReturnToBegin )
	{
		if( IsInRange( m_vPosBegin - pMover->GetPos(), 0.1f ) )	
		{
			DoReturnToBegin( FALSE );
			pMover->SetAngle( m_fAngleBegine );	
			g_UserMng.AddSetDestAngle( pMover, m_fAngleBegine, 0 );			
			SendAIMsg( AIMSG_ARRIVAL, NULL_ID );
		}

#ifdef __AI_0905
		if( TIMEGETTIME - m_tmReturnToBegin >= SEC(20) )
#else	// __AI_0905
		if( TIMEGETTIME - m_tmReturnToBegin >= SEC(10) )
#endif	// __AI_0905
		{
			pMover->SetPos( m_vPosBegin );
			g_UserMng.AddSetPos( pMover, m_vPosBegin );
			m_tmReturnToBegin	= TIMEGETTIME;
		}
	}
	else
	{
		if( pMover->GetMovePattern() == 0 )
		{
		}
		else							
		{
			if( pMover->GetMoveEvent() == 99 )	
				pMover->SetMovePattern( 0 );	
		}

		if( pProp->m_nLoot )
		{
			if( (pMover->GetCount() & 63) == 0 )	
			{	
				if( pProp->m_nLootProb == 0 || (int)( xRandom(100) ) < pProp->m_nLootProb  )
					if( m_bLootMove == FALSE )		
						SubItemLoot();	
			}
		}

		if( m_bLootMove == TRUE )
		{
			CCtrl *pCtrl = prj.GetCtrl( m_idLootItem );	
			if( IsInvalidObj(pCtrl) )	
			{
				MoveToDst( pMover->GetPos() );
				m_idLootItem = NULL_ID;
				m_bLootMove = FALSE;
			}
		}

		if( pProp->m_nScanJob != -1 )
		{
			CMover* pTarget = NULL;
			int nAttackFirstRange = pProp->m_nAttackFirstRange;

			if( nAttackFirstRange <= 0 )
			{
				Error( "m_nAttackFirstRange 이상:%d %s\n", pProp->m_nAttackFirstRange, pMover->GetName() );
				return TRUE;
			}

			if( pProp->m_nScanJob || pProp->m_dwScanQuestId || pProp->m_dwScanItemIdx || pProp->m_nScanChao )
				pTarget = ScanTargetAggro( pMover, (FLOAT)nAttackFirstRange, m_dwAIInterface, m_uParty );

			if( pTarget == NULL && GetMover()->m_bActiveAttack )
				pTarget = ScanTargetAggro( pMover, (FLOAT)nAttackFirstRange, m_dwAIInterface, m_uParty );

			if( pTarget )
			{
				if( IsValidObj( (CObj*)pTarget ) )
				{

#ifdef __JHMA_VER_8_6    
					if( pMover->IsFlyingNPC() == FALSE || pMover->IsFlyingNPC() == pTarget->m_pActMover->IsFly() || pProp->dwClass == RANK_GUARD )	// 가드는 무조건 공격.
#else	// __VER >= 8  
					if( (pMover->IsFlyingNPC() || pTarget->m_pActMover->IsFly() == FALSE) || pProp->dwClass == RANK_GUARD )	// 가드는 무조건 공격.
#endif	// __VER >= 8  
					{
						if( pTarget )	
						{
							g_UserMng.AddDlgEmoticon( pMover, DLGEMOT_ATTACK );
							SendAIMsg( AIMSG_SETSTATE, STATE_RAGE, pTarget->GetId() );
						}
					}
				}
			}	// if( target)
		}	// if( scan )

	}
	return TRUE;
}
BOOL CAIAggro::MoveProcessPatrol( const AIMSG & msg )
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	MoverProp *pProp = pMover->GetProp();

	if( pMover->IsDie() || (pMover->m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL) )
		return FALSE;

	if( m_bReturnToBegin )
	{
		if( IsInRange( m_vPosBegin - pMover->GetPos(), 3.0f ) )	
		{
			DoReturnToBegin( FALSE );
		}


		if( TIMEGETTIME - m_tmReturnToBegin >= SEC(10) )
		{
			pMover->SetPos( m_vPosBegin );
			g_UserMng.AddSetPos( pMover, m_vPosBegin );
			m_tmReturnToBegin	= TIMEGETTIME;
		}
	}
	else
	{
		if( pMover->GetMovePattern() == 0 )
		{
			D3DXVECTOR3 vPos1 = pMover->GetPos();
			D3DXVECTOR3 vPos2 = m_vDestPos;

			if( IsEndStop() )
			{
				MoveToDst( m_vDestPos );
			}
		}
		else				
		{
			if( pMover->GetMoveEvent() == 99 )	
				pMover->SetMovePattern( 0 );	
		}

		if( pProp->m_nLoot )
		{
			if( (pMover->GetCount() & 63) == 0 )	
			{	
				if( pProp->m_nLootProb == 0 || (int)( xRandom(100) ) < pProp->m_nLootProb  )
					if( m_bLootMove == FALSE )		
						SubItemLoot();		
			}
		}

		if( m_bLootMove == TRUE )
		{
			CCtrl *pCtrl = prj.GetCtrl( m_idLootItem );		
			if( IsInvalidObj(pCtrl) )
			{
				MoveToDst( pMover->GetPos() );
				m_idLootItem = NULL_ID;
				m_bLootMove = FALSE;
			}
		}

		if( pProp->m_nScanJob != -1 )
		{
			CMover* pTarget = NULL;
			int nAttackFirstRange = pProp->m_nAttackFirstRange;

			if( nAttackFirstRange <= 0 )
			{
				Error( "m_nAttackFirstRange 이상:%d %s\n", pProp->m_nAttackFirstRange, pMover->GetName() );
				return TRUE;
			}

			if( pProp->m_nScanJob || pProp->m_dwScanQuestId || pProp->m_dwScanItemIdx || pProp->m_nScanChao )
				pTarget = ScanTargetAggro( pMover, (FLOAT)nAttackFirstRange, m_dwAIInterface, m_uParty );

			if( pTarget == NULL && GetMover()->m_bActiveAttack )
				pTarget = ScanTargetAggro( pMover, (FLOAT)nAttackFirstRange, m_dwAIInterface, m_uParty );

			if( pTarget )
			{
				if( IsValidObj( (CObj*)pTarget ) )
				{

#ifdef __JHMA_VER_8_6    
					if( pMover->IsFlyingNPC() == FALSE || pMover->IsFlyingNPC() == pTarget->m_pActMover->IsFly() || pProp->dwClass == RANK_GUARD )	// 가드는 무조건 공격.
#else	// __VER >= 8  
					if( (pMover->IsFlyingNPC() || pTarget->m_pActMover->IsFly() == FALSE) || pProp->dwClass == RANK_GUARD )	// 가드는 무조건 공격.
#endif	// __VER >= 8  
					{
						if( pTarget )	
						{
							g_UserMng.AddDlgEmoticon( pMover, DLGEMOT_ATTACK );
							SendAIMsg( AIMSG_SETSTATE, STATE_RAGE_PATROL, pTarget->GetId() );
						}
					}
				}
			}	// if( target)
		}	// if( scan )

	}
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#define MAX_ARRAY 40

void CAIAggro::CallHelper( const MoverProp* pMoverProp ) 
{
	CMover* pMover = GetMover();
	D3DXVECTOR3 vPos = pMover->GetPos();
	int anCountMonster[ 5 ];
	int nCountPlayer = 0;

	ZeroMemory( anCountMonster, sizeof( anCountMonster ) );

	CMover* pTarget = NULL;
	CParty* pParty = NULL;
	BOOL bParty = FALSE;

	CObj* pObj;

	CMover* apMonster[ MAX_ARRAY ][ 5 ];
	CMover* apPlayer[ MAX_ARRAY ];

	///////////////////////////////////////////////////////

	int nRange = 20;
	switch( pMoverProp->m_nHelpRangeMul )
	{
	case 0: nRange = pMoverProp->m_nAttackFirstRange / 2;	break;
	default: nRange = pMoverProp->m_nAttackFirstRange * pMoverProp->m_nHelpRangeMul;	break;	// 시야범위의 배수.
	}

	FOR_LINKMAP( pMover->GetWorld(), vPos, pObj, nRange / MPU, CObj::linkDynamic, pMover->GetLayer() )
	{
		if( pObj != pMover && pObj->GetType() == OT_MOVER )
		{
			for( int i = 0; i < pMoverProp->m_nCallHelperMax; i++ )
			{
				int nCallHelperIdx = pMoverProp->m_nCallHelperIdx[ i ];
				int nCallHelperNum = pMoverProp->m_nCallHelperNum[ i ];

				if( (pMoverProp->m_bHelpWho == 1 || (pMoverProp->m_bHelpWho == 2 && pObj->GetIndex() == pMover->GetIndex())) &&
					( anCountMonster[ i ] < nCallHelperNum || nCallHelperNum == 0 )	)	
				{
					apMonster[ anCountMonster[ i ] ][ i ] = (CMover*)pObj;
					anCountMonster[ i ]++;
					if( anCountMonster[ i ] >= MAX_ARRAY ) 
						goto NEXT;
				}
			}
		}
	}
	END_LINKMAP
NEXT:
	pTarget = prj.GetMover( m_dwIdTarget );
	for( int i = 0; i < pMoverProp->m_nCallHelperMax; i++ )
		bParty |= pMoverProp->m_bCallHelperParty[ i ] ? TRUE : FALSE;

	if( bParty && IsValidObj( (CObj*)pTarget ) )
	{
		pParty = g_PartyMng.GetParty( pTarget->m_idparty );
		if( pParty )
		{
			FOR_LINKMAP( pMover->GetWorld(), vPos, pObj, 20 / MPU, CObj::linkPlayer, pMover->GetLayer() )
			{
				if( pObj->GetType() == OT_MOVER && ((CMover*)pObj)->IsPlayer() && pParty->IsMember( ((CMover*)pObj)->m_idPlayer ) )
				{
					apPlayer[ nCountPlayer++ ] = (CMover*)pObj;
					if( nCountPlayer >= MAX_ARRAY )
						goto NEXT2;
				}
			}
			END_LINKMAP
		}
	}
NEXT2:
	for( i = 0; i < pMoverProp->m_nCallHelperMax; i++ )
	{
		if( pParty  )
		{
			if( nCountPlayer )
			{
				for( int j = 0; j < anCountMonster[ i ]; j++ )
				{
					CMover* pNewTarget = (CMover*)apPlayer[ rand() % nCountPlayer ];
					if( pNewTarget )
						apMonster[ j ][ i ]->PostAIMsg( AIMSG_SETSTATE, STATE_RAGE, pNewTarget->GetId() );
				}
			}
		}
		else
		{ 
			for( int j = 0; j < anCountMonster[ i ]; j++ )
				apMonster[ j ][ i ]->PostAIMsg( AIMSG_SETSTATE, STATE_RAGE, m_dwIdTarget );
		}
	}
}

DWORD CAIAggro::GetAtkMethod_Far()
{
	CMover* pMover = GetMover();
	MoverProp* pMoverProp = pMover->GetProp();
	DWORD dwRandom = xRandom( 100 );

	int nDeathblow = (int)( pMover->GetHitPointPercent() / 4.5f );
	if( nDeathblow )
	{
		if( !xRandom( nDeathblow ) )
			return 3;
	}

	switch( pMover->m_dwBelligerence )
	{
	case BELLI_MELEE2X:
		if( dwRandom < 50 )
			return 1;
		return 2;
		break;
	case BELLI_MELEE:
		if( dwRandom < 30 )
			return 1;
		return 2;
		break;
	case BELLI_RANGE:
		if( dwRandom < 20 )
			return 1;
		return 2;
	}
	return 1;
}
DWORD CAIAggro::GetAtkMethod_Near()
{
	CMover* pMover = GetMover();
	MoverProp* pMoverProp = pMover->GetProp();
	DWORD dwRandom = xRandom( 100 );
	int nDeathblow = (int)( pMover->GetHitPointPercent() / 4.5f );
	if( nDeathblow )
	{
		if( !xRandom( nDeathblow ) )
			return 3;
	}

	switch( pMover->m_dwBelligerence )
	{
	case BELLI_MELEE2X:
		if( dwRandom < 60 )
			return 1;
		return 2;
		break;
	case BELLI_MELEE:
		if( dwRandom < 80 )
			return 1;
		return 2;
		break;
	case BELLI_RANGE:
		if( dwRandom < 50 )
			return 1;
		return 2;
		break;
	}
	return 1;
}
DWORD CAIAggro::GetAtkRange( DWORD dwAtkMethod )
{
	ItemProp* pItemProp = GetAtkProp( dwAtkMethod );
#ifdef _DEBUG
	if( pItemProp == NULL )
	{
		Error( "CAIAggro::GetAtkRange : ItemProp->dwAtk%d이 NULL이다. (%s)", dwAtkMethod, GetMover()->GetName() );
	}
#endif
	return pItemProp->dwAttackRange;
}

ItemProp* CAIAggro::GetAtkProp( DWORD dwAtkMethod )
{
	return prj.GetItemProp( GetAtkItemId( dwAtkMethod ) );
}

DWORD CAIAggro::GetAtkItemId( DWORD dwAtkMethod )
{
	CMover* pMover = GetMover();
	switch( dwAtkMethod )
	{ 
	case 1:
		return pMover->GetProp()->dwAtk1;
	case 2:
		return pMover->GetProp()->dwAtk2;
	case 3:
		return pMover->GetProp()->dwAtk3;
	}
	return 0;
}

void CAIAggro::DoAttack( DWORD dwAtkMethod, CMover* pTarget )
{
	CMover* pMover = GetMover();
	if( IsInvalidObj( pMover ) || dwAtkMethod == 0 )	
		return;

	DWORD dwAtkItem = GetAtkItemId( dwAtkMethod );
	ItemProp* pItemProp = prj.GetItemProp( dwAtkItem );
	if( pItemProp == NULL )
	{
		Error( "CAIAggro::DoAttack : %s %d %d", pMover->GetName(), dwAtkMethod, dwAtkItem );
		return;
	}
	
	if( pItemProp->dwAttackRange == AR_RANGE || pItemProp->dwAttackRange == AR_WAND )
	{
		int id = pMover->m_sfxHitArray.Add( 0, pTarget->GetId(), AF_GENERIC, 0 );
		DWORD dwItemID = dwAtkItem;
		if( dwItemID == NULL_ID )
			Error( "경고 : %s의 dwAtk2가 지정되지 않았다", pMover->GetName() );

		if( pMover->DoAttackRange( pTarget, dwItemID, id ) != 1 )
			pMover->m_sfxHitArray.RemoveSfxHit( id, FALSE );	
	}
	else
	{
		DWORD dwItemID = dwAtkItem;
		if( dwItemID == NULL_ID )
			Error( "경고 : %s의 공격%d가 지정되지 않았다", pMover->GetName(), dwAtkMethod );
		if( dwAtkMethod == 3 )
			pMover->DoAttackSP( pTarget, dwItemID );
		else
		{
			int nError = pMover->SendActMsg( OBJMSG_ATK1, pTarget->GetId(), 0, 0 );
			if( nError > 0 ) 
				g_UserMng.AddMeleeAttack( pMover, OBJMSG_ATK1, pTarget->GetId(), 0, MAKELONG( 0, (WORD)nError ) );
			else
				nError = -1;
		}
	}

	m_tmAttack	= TIMEGETTIME + s_tmAttack;	
}


BOOL CAIAggro::SubAttackChance( const AIMSG & msg, CMover *pTarget )
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	MoverProp *pProp = pMover->GetProp();
	int		nTargetJob = 0;
	if( pTarget->IsPlayer() )
		nTargetJob = pTarget->GetJob();		
	
	BOOL bAttack = TRUE;
	D3DXVECTOR3	vTargetTo = pTarget->GetPos() - pMover->GetPos();	
	
	BOOL bRangeAttack = (pProp->m_bRangeAttack[ nTargetJob ] & 0x7F) ? TRUE : FALSE;	

	if( pProp->m_bRecvCond )	
	{
		if( pMover->GetHitPointPercent( 100 ) <= pProp->m_nRecvCondMe )	
		{
			if( pMover->GetManaPoint() > 0 )
			{
				int nHP = (int)(pMover->GetMaxHitPoint() * (pProp->m_nRecvCondHow / 100.0f));	
				pMover->IncHitPoint( nHP );
				
				g_UserMng.AddCreateSfxObj( pMover, XI_GEN_CURE01, pMover->GetPos().x, pMover->GetPos().y, pMover->GetPos().z );	
				if( pProp->m_nRecvCondMP > 0 )		
				{
					int nMP = pProp->m_nRecvCondMP;			
					pMover->IncManaPoint( -nMP );
				} 
				else
				{
					int nMP = pMover->GetMaxManaPoint();	
					pMover->IncManaPoint( -nMP );
				}
				bAttack = FALSE;
			}
		}
		m_tmAttackDelay = TIMEGETTIME + SEC( 3 );
	}

	if( bAttack )
	{
		if( m_bRangeAttack )
			m_dwAtkMethod = GetAtkMethod_Far();	
		else
			m_dwAtkMethod = GetAtkMethod_Near();

		DWORD dwAtkRange = GetAtkRange( m_dwAtkMethod );
		if( dwAtkRange == AR_RANGE || dwAtkRange == AR_WAND )
		{
			FLOAT fRange = (float)(pProp->m_bRangeAttack[ nTargetJob ] & 0x7F);
			fRange += 1.0f;		
			if( IsInRange( vTargetTo, fRange ) == TRUE )	
			{
				DoAttack( m_dwAtkMethod, (CMover*)pTarget );	
				m_tmAttackDelay	= TIMEGETTIME + SEC( 3 );	
			}
		} 
		else
		{
			FLOAT fRange = pMover->GetAttackRange( dwAtkRange );
			if( pMover->IsRangeObj( pTarget, fRange ) )
			{
				DoAttack( m_dwAtkMethod, pTarget );		
				m_bGoTarget = TRUE; 
				int nDelay = (int)pProp->dwReAttackDelay - SEC( 1 )  + xRandom( SEC( 2 ) );
				m_tmAttackDelay		= TIMEGETTIME + nDelay;
				TRACE( "AtkDelay:%d\n", (int)nDelay );
			}
		}
	}
	
	return TRUE;
}



int CAIAggro::SelectAttackType( CMover *pTarget )
{
	int nAttackType = 0;

	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	MoverProp *pProp = pMover->GetProp();
	
	int		nTargetJob = 0;
	int		nRange = pProp->m_bRangeAttack[ nTargetJob ] & 0x7F;
	
	if( pTarget->IsPlayer() )		nTargetJob = pTarget->GetJob();	

	BOOL bRange = ( pProp->m_bRangeAttack[ nTargetJob ] ) ? TRUE : FALSE;	
	BOOL bMelee = (BOOL)pProp->m_bMeleeAttack;							


	if( bRange && bMelee )	
	{
		D3DXVECTOR3	vDist = pTarget->GetPos() - pMover->GetPos();
		FLOAT		fDistSq = D3DXVec3LengthSq( &vDist );

		int nHalfRange = nRange / 2;	
		if( fDistSq < (nHalfRange * nHalfRange) )	
			nAttackType = 0;	
		else
			nAttackType = 1;	
	} else
	if( bRange && bMelee == FALSE )		
	{
		nAttackType = 1;
	} else
	if( bRange == FALSE && bMelee )	
	{
		nAttackType = 0;
	}

	return nAttackType;
}


void CAIAggro::SubSummonProcess( CMover *pTarget )
{
	if( TIMEGETTIME - m_tmSummon < SEC(10) )	
		return;

	m_tmSummon = TIMEGETTIME;

	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	MoverProp *pProp = pMover->GetProp();
	if( pProp && pProp->m_nSummProb )
	{
		for( vector<OBJID>::iterator it = m_vecIdSummon.begin(); it!=m_vecIdSummon.end(); )
		{
			CMover *pZako = prj.GetMover( (*it) );
			if( IsValidObj(pZako) && pZako->IsLive() )
				it++;
			else
				it = m_vecIdSummon.erase( it );
		}
		
		if( pProp->m_nSummNum > (int)( m_vecIdSummon.size() ) )
		{
			if( (int)( xRandom(100) ) < pProp->m_nSummProb )	
			{
				D3DXVECTOR3 vLocal, vPos;
				CObj* pObj	= CreateObj( D3DDEVICE, OT_MOVER, (DWORD)pProp->m_nSummID );
				if( NULL == pObj )	
				{
					Error( "SubSummonProcess : 소환 실패 %d", pProp->m_nSummID );
					return;
				}
				vLocal.x = 1.5f - xRandomF( 3.0f );		
				vLocal.y = 0;
				vLocal.z = 1.5f - xRandomF( 3.0f );
				vPos = pMover->GetPos();
				vPos += vLocal;
				pObj->SetPos( vPos );
				pObj->InitMotion( MTI_STAND );
				pObj->UpdateLocalMatrix();
				((CMover*)pObj)->m_bActiveAttack = TRUE;	
				((CMover*)pObj)->AddItToGlobalId();
				pWorld->ADDOBJ( pObj, FALSE, pMover->GetLayer() );
				m_vecIdSummon.push_back( ((CMover *)pObj)->GetId() );
				g_UserMng.AddDlgEmoticon( pMover, DLGEMOT_SUMMON );
			}
		}
	}
}




BOOL CAIAggro::MoveProcessRage( const AIMSG & msg )
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	MoverProp *pProp = pMover->GetProp();
	
	if( pMover->IsDie() || (pMover->m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL) )
		return FALSE;
	
	CMover* pTarget = prj.GetMover( m_dwIdTarget ); 

	DWORD dwAIState = pMover->m_dwAIInterfaceState;

	if( IsValidObj( pTarget ) == FALSE || ((CMover*)pTarget)->IsDie()
		|| pTarget->GetWorld() != pMover->GetWorld()
		)	
	{
		int nAttackFirstRange	= pProp->m_nAttackFirstRange;
		CMover* pTarget	= NULL;
		if( CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( pWorld->GetID() ) )
			pTarget	= ScanTargetAggro( pMover, 160, m_dwAIInterface, m_uParty );
		else
			pTarget	= ScanTargetAggro( pMover, 15, m_dwAIInterface, m_uParty );

		if( pTarget	== NULL )
		{
			if( dwAIState == STATE_STAND )
				DoReturnToBegin();
			SendAIMsg( AIMSG_SETSTATE, dwAIState );
			return TRUE;
		}
		else
		{
			SendAIMsg( AIMSG_SETSTATE, STATE_RAGE, pTarget->GetId() );
			return TRUE;
		}
	}

	if( pTarget->IsRegionAttr( RA_SAFETY ) && pProp->dwClass != RANK_GUARD )
	{
		DoReturnToBegin();
		SendAIMsg( AIMSG_SETSTATE, dwAIState );
		return TRUE;
	}

#ifdef __JHMA_VER_8_6     
	if( pMover->IsFlyingNPC() != ((CMover*)pTarget)->m_pActMover->IsFly() && pMover->IsFlyingNPC() )	
#else	// __VER >= 8  
	if( pMover->IsFlyingNPC() != ((CMover*)pTarget)->m_pActMover->IsFly() )	
#endif	// __VER >= 8  
	{
		if( dwAIState == STATE_STAND )
			DoReturnToBegin();
		
		SendAIMsg( AIMSG_SETSTATE, dwAIState );
		return TRUE;
	}

	if( TIMEGETTIME > m_tmAttack )	
	{
		m_tmAttack	= TIMEGETTIME + s_tmAttack;	
		D3DXVECTOR3 vDist = pMover->GetPos() - m_vOldPos;
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

		m_vOldPos = pMover->GetPos();
	}

	D3DXVECTOR3	vTargetTo = pTarget->GetPos() - pMover->GetPos();	

	D3DXVECTOR3 v1	= pMover->GetPos() - m_vPosBegin;
	
	FLOAT fRange	= RANGE_MOVE;
	if( pMover->IsFlyingNPC() )
		fRange	*= 3.0F;
	
	if( IsInRange( v1, RANGE_RETURN_TO_BEGIN + fRange ) == FALSE
		&& !CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( pWorld->GetID() )
		)
	{
		DoReturnToBegin();
		SendAIMsg( AIMSG_SETSTATE, dwAIState );
		return TRUE;
	}
	if( m_vPosDamage == D3DXVECTOR3( 0, 0, 0 ) )	
		m_vPosDamage	= pMover->GetPos();

	D3DXVECTOR3 v2	= pMover->GetPos() - m_vPosDamage;
	if( IsInRange( v2, RANGE_RETURN_TO_BEGIN ) == FALSE
		&& !CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( pWorld->GetID() )
		)
	{
		DoReturnToBegin();
		SendAIMsg( AIMSG_SETSTATE, dwAIState );
		return TRUE;
	}
	else if( pTarget->GetType() == OT_MOVER && ((CMover *)pTarget)->IsMode( TRANSPARENT_MODE ) )		
	{
		if( CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( pWorld->GetID() ) )
		{
			CMover* pTarget = ScanTargetAggro( pMover, 160, m_dwAIInterface, m_uParty );	
			if( pTarget	)
			{
				SendAIMsg( AIMSG_SETSTATE, STATE_RAGE, pTarget->GetId() );
				return TRUE;
			}
		}
		if( dwAIState == STATE_STAND )
			DoReturnToBegin();	
		SendAIMsg( AIMSG_SETSTATE, dwAIState );
		return TRUE;
	}
	else
	{
		m_bRangeAttack = SelectAttackType( pTarget );		

		if( m_bRangeAttack )	
		{
			int nTargetJob = pTarget->GetJob();
			if( nTargetJob < 0 )	nTargetJob = 0;		
			if( nTargetJob >= MAX_JOB )
			{
				Error( "CAIAggro::MoverProcessRage : 범위침범 %d", nTargetJob );
				nTargetJob = 0;
			}

			FLOAT	fRange = (FLOAT)(pProp->m_bRangeAttack[ nTargetJob ] & 0x7F);	
			BOOL	bKeepRange = (pProp->m_bRangeAttack[ nTargetJob ] & 0x80) ? TRUE : FALSE;	
			FLOAT	fDist = D3DXVec3Length( &vTargetTo );		
			if( bKeepRange )
			{
				if( fabs(fDist - fRange) < 1.0f )	
				{
					if( pMover->IsEmptyDest() == FALSE )	
						MoveToDst( pMover->GetPos() );		
				} else
				{
					if( fDist < fRange )	
					{
						FLOAT fAngle = GetDegreeX( pMover->GetPos(), pTarget->GetPos() );
						D3DXVECTOR3 vDst = pMover->GetPos() + VelocityToVec( fAngle, 10 );
						MoveToDst( vDst );		
					} else
						MoveToDst( pTarget->GetId() );	
				}
			} else
			{
				if( fDist < fRange )		
				{
					if( pMover->IsEmptyDest() == FALSE )	
						MoveToDst( pMover->GetPos() );	
				} else
					MoveToDst( pTarget->GetId() );	
			}
		} else
		{
			MoveToDst( pTarget->GetId() );		
		}

		if( pProp->m_nSummProb )		
			SubSummonProcess( pTarget );	
		
		if( TIMEGETTIME > m_tmAttackDelay )
		{
			if( SubAttackChance( msg, pTarget ) == FALSE )
				return FALSE;
		}
	}

	CMover* pTargetNext;
	if( CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( pWorld->GetID() ) )
		pTargetNext	= ScanTargetAggro( pMover, 160, m_dwAIInterface, m_uParty );
	else
		pTargetNext	= ScanTargetAggro( pMover, 15, m_dwAIInterface, m_uParty );
	if( pTargetNext && pTarget && pTargetNext->GetId() != pTarget->GetId() )
		SetTarget( pTargetNext->GetId(), pTargetNext->GetPartyId() );

	return TRUE;
}

BOOL CAIAggro::MoveProcessRagePatrol( const AIMSG & msg )
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	MoverProp *pProp = pMover->GetProp();
	
	if( pMover->IsDie() || (pMover->m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL) )
		return FALSE;
	
	CMover* pTarget = prj.GetMover( m_dwIdTarget ); 

	DWORD dwAIState = pMover->m_dwAIInterfaceState;

	if( IsValidObj( pTarget ) == FALSE )
	{
		DoReturnToBegin();
		SendAIMsg( AIMSG_SETSTATE, dwAIState );
		return TRUE;
	}
	
	if( ((CMover*)pTarget)->IsDie() )
	{
		DoReturnToBegin();	
		SendAIMsg( AIMSG_SETSTATE, dwAIState );
		return TRUE;
	}

#ifdef __JHMA_VER_8_6     
	if( pMover->IsFlyingNPC() == TRUE && ((CMover*)pTarget)->m_pActMover->IsFly() == FALSE )	
#else	// __VER >= 8  
	if( pMover->IsFlyingNPC() == FALSE && ((CMover*)pTarget)->m_pActMover->IsFly() )	
#endif	// __VER >= 8  
	{
		DoReturnToBegin();
		SendAIMsg( AIMSG_SETSTATE, dwAIState );
		return TRUE;
	}

	if( TIMEGETTIME > m_tmAttack )
	{
		m_tmAttack = TIMEGETTIME + s_tmAttack;		
		D3DXVECTOR3 vDist = pMover->GetPos() - m_vOldPos;
		vDist.y = 0;					
		if( IsInRange( vDist, 2.0f ) )	
		{
			if( pMover->GetSpeed( pMover->m_pActMover->m_fSpeed ) > 0 )		
			{
				D3DXVECTOR3 vTarget = pTarget->GetPos();
				pMover->SetPos( vTarget );		
				g_UserMng.AddSetPos( pMover, pMover->GetPos() );
			} 
		} 
		else
		{
			m_tmReturnToBegin = TIMEGETTIME;		
			DoReturnToBegin();
			SendAIMsg( AIMSG_SETSTATE, dwAIState );
			return TRUE;
		}
		m_vOldPos = pMover->GetPos();	
	}

	D3DXVECTOR3	vTargetTo = pTarget->GetPos() - pMover->GetPos();	
	if( IsInRange( vTargetTo, RANGE_CANCEL ) == FALSE )
	{
		DoReturnToBegin();
		SendAIMsg( AIMSG_SETSTATE, dwAIState );
		return TRUE;
	}
	else if( pTarget->GetType() == OT_MOVER && ((CMover *)pTarget)->IsMode( TRANSPARENT_MODE ) )	
	{

		if( CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( pWorld->GetID() ) )
		{
			CMover* pTarget = ScanTargetAggro( pMover, 160, m_dwAIInterface, m_uParty );	
			if( pTarget	)
			{
				SendAIMsg( AIMSG_SETSTATE, STATE_RAGE, pTarget->GetId() );
				return TRUE;
			}
		}

		DoReturnToBegin();	
		SendAIMsg( AIMSG_SETSTATE, dwAIState );
		return TRUE;
	}
	else
	{
		m_bRangeAttack = SelectAttackType( pTarget );		

		if( m_bRangeAttack )
		{
			int nTargetJob = pTarget->GetJob();
			if( nTargetJob < 0 )	nTargetJob = 0;	
			if( nTargetJob >= MAX_JOB )
			{
				Error( "CAIAggro::MoverProcessRage : 범위침범 %d", nTargetJob );
				nTargetJob = 0;
			}

			FLOAT	fRange = (FLOAT)(pProp->m_bRangeAttack[ nTargetJob ] & 0x7F);	
			BOOL	bKeepRange = (pProp->m_bRangeAttack[ nTargetJob ] & 0x80) ? TRUE : FALSE;
			FLOAT	fDist = D3DXVec3Length( &vTargetTo );	
			if( bKeepRange )
			{
				if( fabs(fDist - fRange) < 1.0f )	
				{
					if( pMover->IsEmptyDest() == FALSE )	
						MoveToDst( pMover->GetPos() );		
				} else
				{
					if( fDist < fRange )	
					{
						FLOAT fAngle = GetDegreeX( pMover->GetPos(), pTarget->GetPos() );	
						D3DXVECTOR3 vDst = pMover->GetPos() + VelocityToVec( fAngle, 10 );
						MoveToDst( vDst );		
					} else
						MoveToDst( pTarget->GetId() );	
				}
			} else
			{
				if( fDist < fRange )	
				{
					if( pMover->IsEmptyDest() == FALSE )	
						MoveToDst( pMover->GetPos() );	
				} else
					MoveToDst( pTarget->GetId() );
			}
		} else
		{
			MoveToDst( pTarget->GetId() );	
		}

		if( pProp->m_nSummProb )			
			SubSummonProcess( pTarget );	
		
		if( TIMEGETTIME > m_tmAttackDelay )
		{
			if( SubAttackChance( msg, pTarget ) == FALSE )
				return FALSE;
		}
	}
	return TRUE;
}

BOOL CAIAggro::StateRagePatrol( const AIMSG & msg )
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	if( IsInvalidObj(pMover) )	return FALSE;

	BeginAIHandler( )

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT ) 
		if( msg.dwParam2 != NULL_ID )
		{
			CMover* pTarget	= prj.GetMover( msg.dwParam2 );
			if( IsValidObj( pTarget ) )
				SetTarget( msg.dwParam2, pTarget->m_idparty );
		}
		m_tmAttack = TIMEGETTIME + s_tmAttack;
		m_tmAttackDelay = TIMEGETTIME; 
		SetStop( 0 ); 
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT_TARGETCLEAR )		
		SetTarget( NULL_ID, 0 );
		SendAIMsg( AIMSG_SETSTATE, STATE_PATROL );
		
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_PROCESS ) 
		MoveProcessRagePatrol( msg );
		
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DAMAGE ) 
		if( IsMove() && m_bGoTarget == FALSE )
		{
			if( xRandom( 2 ) == 0 )		
			{
				MoveToDst( msg.dwParam1 );
			}
			m_bGoTarget = TRUE;
		}
		MoverProp* pMoverProp = pMover->GetProp();
		int nHPPercent = pMover->GetHitPointPercent( 100 );

		if( pMoverProp->m_bHelpWho )
		{
			if( TIMEGETTIME - m_tmHelp > pMoverProp->m_tmUnitHelp )	
			{
				if( m_bCallHelper == FALSE )
				{
					CallHelper( pMoverProp );
					g_UserMng.AddDlgEmoticon( pMover, DLGEMOT_HELPER );
					m_tmHelp = TIMEGETTIME;
					if( pMoverProp->m_tmUnitHelp == 0 )	
						m_bCallHelper = TRUE;
				}
			}
		}
		else
			m_bCallHelper = FALSE;

		m_vPosDamage	= pMover->GetPos();

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_COLLISION )

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DIE ) 

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DSTDIE ) 
		if( m_dwIdTarget == msg.dwParam1 )
		{
			CMover* pTarget	= ScanTargetAggro( pMover, /*pProp->m_nAttackFirstRange*/ 15, m_dwAIInterface, m_uParty );
			if( pTarget )
			{
				SendAIMsg( AIMSG_SETSTATE, STATE_RAGE, pTarget->GetId() );
				return TRUE;
			}
			else
			{
				SetTarget( NULL_ID, 0 );
			}
		}
		TRACE( "TARGET CLEARED\n" );
		SendAIMsg( AIMSG_SETSTATE, STATE_PATROL );

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_BEGINMOVE )
		m_bGoTarget = FALSE; 

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_ARRIVAL )

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_EXIT )	

	EndAIHandler( )
	return TRUE;
}

BOOL CAIAggro::StateRage( const AIMSG & msg )
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	MoverProp *pProp	= pMover->GetProp();
	if( IsInvalidObj(pMover) )	return FALSE;

	DWORD dwAIState = pMover->m_dwAIInterfaceState;

	BeginAIHandler( )

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT ) 
		if( msg.dwParam2 != NULL_ID )
		{
			CMover* pTarget	= prj.GetMover( msg.dwParam2 );
			if( IsValidObj( pTarget ) )
			{
#ifndef __CLIENT
				if( pTarget->IsPlayer() )
					pTarget->SetAggro( pTarget->GetAggro() + 1 );
#endif
				SetTarget( msg.dwParam2, pTarget->m_idparty );
			}
		}
		m_tmAttack = TIMEGETTIME + s_tmAttack;
		m_tmAttackDelay = TIMEGETTIME; 
		SetStop( 0 ); 
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT_TARGETCLEAR )		
		CMover* pTarget	= ScanTargetAggro( pMover, /*pProp->m_nAttackFirstRange*/ 15, m_dwAIInterface, m_uParty );
		if( pTarget )
		{
			SendAIMsg( AIMSG_SETSTATE, STATE_RAGE, pTarget->GetId() );
			return TRUE;
		}
		else
		{
			SetTarget( NULL_ID, 0 );
			SendAIMsg( AIMSG_SETSTATE, dwAIState );
		}
		
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_PROCESS ) 
		MoveProcessRage( msg );
		
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DAMAGE ) 
		if( IsMove() && m_bGoTarget == FALSE )
		{
			if( xRandom( 2 ) == 0 )	
			{
				MoveToDst( msg.dwParam1 );
			}
			m_bGoTarget = TRUE;
		}
		MoverProp* pMoverProp = pMover->GetProp();
		int nHPPercent = pMover->GetHitPointPercent( 100 );

		if( nHPPercent < pMoverProp->m_nRunawayHP )
		{
			SendAIMsg( AIMSG_SETSTATE, STATE_RUNAWAY );
			g_UserMng.AddDlgEmoticon( pMover, DLGEMOT_EVADE );
		}
		if( pMoverProp->m_bHelpWho )
		{
			if( TIMEGETTIME - m_tmHelp > pMoverProp->m_tmUnitHelp )
			{
				if( m_bCallHelper == FALSE )
				{
					CallHelper( pMoverProp );
					g_UserMng.AddDlgEmoticon( pMover, DLGEMOT_HELPER );
					m_tmHelp = TIMEGETTIME;
					if( pMoverProp->m_tmUnitHelp == 0 )	
						m_bCallHelper = TRUE;
				}
			}
		}
		else
			m_bCallHelper = FALSE;

		m_vPosDamage	= pMover->GetPos();

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_COLLISION )
		MoveToRandom( STATE_RAGE );

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DIE ) 

	///////////////////////////////////////////////////////////////////////////////////////////////////
	
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DSTDIE ) 
		if( m_dwIdTarget == msg.dwParam1 )
		{
			CMover* pTarget	= ScanTargetAggro( pMover, /*pProp->m_nAttackFirstRange*/ 15, m_dwAIInterface, m_uParty );
			if( pTarget )
			{
				SendAIMsg( AIMSG_SETSTATE, STATE_RAGE, pTarget->GetId() );
				return TRUE;
			}
			else
			{
				SetTarget( NULL_ID, 0 );
			}
		}
		SendAIMsg( AIMSG_SETSTATE, dwAIState );
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_BEGINMOVE )
		m_bGoTarget = FALSE; 

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_ARRIVAL )

	///////////////////////////////////////////////////////////////////////////////////////////////////
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

BOOL CAIAggro::MoveProcessRunaway()
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();

	if( pMover->IsDie() || (pMover->m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL))
		return FALSE;

	DWORD dwAIState = pMover->m_dwAIInterfaceState;

	CCtrl* pTarget = prj.GetCtrl( m_dwIdTarget ); 

	if( IsValidObj( pTarget ) == FALSE )
	{
		SetTarget( NULL_ID, 0 );
		SendAIMsg( AIMSG_SETSTATE, dwAIState );
	}
	else
	if( IsEndStop()  )
	{
		CMover* pMoverTarget = (CMover*)pTarget;
		DWORD dwAttackRange = pMoverTarget->GetActiveHandItemProp()->dwAttackRange;
		FLOAT fRange = pMoverTarget->GetAttackRange( dwAttackRange ) * 1.7f;

		if( IsInRange( pTarget->GetPos() - pMover->GetPos(), fRange ) == TRUE )
		{
			pMover->AddAngle( (float)( 20 - xRandom( 40 ) ) );
			FLOAT fAngle = pMover->GetAngle();
			D3DXVECTOR3 vDst = pMover->GetPos() + VelocityToVec( fAngle, fRange );
			MoveToDst( vDst );
		}
		if( m_bTargetNoMovePos == FALSE )
		{
			if( ::TIMEGETTIME > pMover->m_dwTickRecovery )
			{
				pMover->m_dwTickRecovery = ::TIMEGETTIME + NEXT_TICK_RECOVERY;
				pMover->IncHitPoint( pMover->GetHPRecovery() );
				if( pMover->GetHitPointPercent( 100 ) >= 50 )
					SendAIMsg( AIMSG_SETSTATE, STATE_RAGE, NULL_ID );
			}
		}
	}
	return TRUE;
}

BOOL CAIAggro::StateRunaway( const AIMSG & msg )
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();

	DWORD dwAIState = pMover->m_dwAIInterfaceState;

	BeginAIHandler( )

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT ) 
		m_bTargetNoMovePos	= msg.dwParam2;
		pMover->AddAngle( (float)( 180 + ( 20 - xRandom( 40 ) ) ) );
		CMover* pMoverTarget = (CMover*)prj.GetCtrl( m_dwIdTarget );
		if( IsValidObj(pMoverTarget) )
		{
			DWORD dwAttackRange = pMoverTarget->GetActiveHandItemProp()->dwAttackRange;
			FLOAT fRange = pMoverTarget->GetAttackRange( dwAttackRange ) * 1.7f;
			FLOAT fAngle = pMover->GetAngle();
			D3DXVECTOR3 vDst = pMover->GetPos() + VelocityToVec( fAngle, fRange );
			MoveToDst( vDst );
			m_bFirstRunaway = TRUE; 
		}
		else
		{
			SendAIMsg( AIMSG_SETSTATE, dwAIState );
		}
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_PROCESS ) 
		MoveProcessRunaway();
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DAMAGE ) 
		if( m_bTargetNoMovePos == TRUE && m_bFirstRunaway == FALSE )
			SendAIMsg( AIMSG_SETSTATE, STATE_RAGE, NULL_ID );

		m_vPosDamage	= pMover->GetPos();

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_COLLISION )
		pMover->AddAngle( (float)( 180 + ( 20 - xRandom( 40 ) ) ) );
		FLOAT fAngle = pMover->GetAngle();
		D3DXVECTOR3 vDst = pMover->GetPos() + VelocityToVec( fAngle, 5 );
		MoveToDst( vDst );
	
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DIE ) 
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DSTDIE ) 
		if( m_dwIdTarget == msg.dwParam1 )
		{
			CMover* pTarget	= ScanTargetAggro( pMover, /*pProp->m_nAttackFirstRange*/ 15, m_dwAIInterface, m_uParty );
			if( pTarget )
			{
				SendAIMsg( AIMSG_SETSTATE, STATE_RAGE, pTarget->GetId() );
				return TRUE;
			}
			else
			{
				SetTarget( NULL_ID, 0 );
			}
		}
		SendAIMsg( AIMSG_SETSTATE, dwAIState );
		
	OnMessage( AIMSG_ARRIVAL )
		m_bFirstRunaway = FALSE;
		MoverProp* pMoverProp = pMover->GetProp();
		if( pMoverProp->m_dwRunawayDelay )
			SetStop( pMover->GetProp()->m_dwRunawayDelay + xRandom( SEC( 1 ) ) );

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_EXIT )	

	EndAIHandler( )

	return TRUE;
}

void	CAIAggro::SetTarget( OBJID dwIdTarget, u_long uParty )
{
	m_dwIdTarget	= dwIdTarget;
#ifdef __AI_SUMMON
	CMover* pMover = prj.GetMover( m_dwIdTarget );
	if( pMover )
	{
		if( pMover->HasActivatedSummon() )
		{
			CMover* pSummon = prj.GetMover( pMover->GetSummon() );
			if( pSummon && pSummon->IsDie() == FALSE )
				m_dwIdTarget = pMover->GetSummon();
		}
	}
#endif
	m_uParty	= uParty;
	if( dwIdTarget == NULL_ID )
		GetMover()->SetSpeedFactor( 1.0F );
}

#endif