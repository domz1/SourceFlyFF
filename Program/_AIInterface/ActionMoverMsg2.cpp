#include "stdafx.h"
#include "ActionMover.h"

#ifdef __CLIENT
#include "DPClient.h"
extern	CDPClient	g_DPlay;
#endif

#ifdef __WORLDSERVER
#include "user.h"
#include "dpcoreclient.h"
#include "dpdatabaseclient.h"
#include "..\_AIInterface\AIInterface.h"

extern	CUserMng			g_UserMng;
extern	CDPDatabaseClient	g_dpDBClient;
extern	CDPCoreClient		g_DPCoreClient;
#endif	// __WORLDSERVER




//
//	Action Message Process
//	액션 메시지를 받아 처리한다.
//	어떤 행위가 발생하는 시점에 대한 처리를 담당.
//	최적화를 위해서 이미 설정되어 있는 상태면 중복 처리 하지 않음
//	빗자루 비행용
//
int		CActionMover::ProcessActMsg2(  CMover* pMover, OBJMSG dwMsg, int nParam1, int nParam2, int nParam3, int nParam4, int nParam5 )
{
	ItemProp* pItemProp = prj.GetItemProp( m_pMover->m_dwRideItemIdx );

	switch( dwMsg )
	{
	// 평화모드 빗자루 제자리 대기
	case OBJMSG_STAND:
		{
		#ifdef __Y_INTERFACE_VER3
			if( (GetMoveState() == OBJSTA_FMOVE) || (GetMoveState() == OBJSTA_BMOVE) || (GetMoveState() == OBJSTA_LMOVE) || (GetMoveState() == OBJSTA_RMOVE) )	// 전/후진중일때 제자리에 세운다.
		#else //__Y_INTERFACE_VER3
			if( (GetMoveState() == OBJSTA_FMOVE) || (GetMoveState() == OBJSTA_BMOVE) )	// 전/후진중일때 제자리에 세운다.
		#endif //__Y_INTERFACE_VER3
			{
				ResetState( OBJSTA_MOVE_ALL );		
			}
			if( GetMoveState() == OBJSTA_STAND )	return 0;	// 이미 대기상태이면 취소
			if( IsActAttack() )		return -2;	// 공격 동작상태면 취소
			if( IsActTurn() )		return 0;
			if( !pItemProp )
				Error( "ItemProp is NULL in ProcessActMsg2() %d", m_pMover->m_dwRideItemIdx );
			SetMoveState( OBJSTA_STAND );	// 상태설정
			BOOL ret;
			if( pItemProp )
				ret = pMover->SetMotion( pItemProp->dwUseMotion + MTA_FSTAND1, ANILOOP_LOOP, MOP_FIXED );		// 애니메이션 설정
			if( ret == TRUE )
				if( pMover->m_pRide )	pMover->m_pRide->m_fFrameCurrent	= 0;
			break;
		}
	case OBJMSG_STOP:		
	case OBJMSG_ASTOP:
	#ifdef __Y_INTERFACE_VER3
		if( (GetMoveState() == OBJSTA_FMOVE) || (GetMoveState() == OBJSTA_BMOVE) || (GetMoveState() == OBJSTA_LMOVE) || (GetMoveState() == OBJSTA_RMOVE) )	// 전/후진중일때 제자리에 세운다.
	#else //__Y_INTERFACE_VER3
		if( (GetMoveState() == OBJSTA_FMOVE) || (GetMoveState() == OBJSTA_BMOVE) )	// 전/후진중일때 제자리에 세운다.
	#endif //__Y_INTERFACE_VER3
		{
			pMover->ClearDest();				// 목표 좌표 클리어.
			SendActMsg( OBJMSG_ACC_STOP );		// 가속 중지
			ResetState( OBJSTA_MOVE_ALL );		
		}
		break;
		// 전진해라!
	case OBJMSG_FORWARD:
		if( GetMoveState() == OBJSTA_FMOVE )	return 0;// 이미 전진상태면 추가로 처리 하지 않음
		SetMoveState( OBJSTA_FMOVE );		// 
		if( IsActTurn() )	break;
		if( GetState() & OBJSTA_ATK_ALL )	
			return 1;
		if( !pItemProp )
			Error( "ItemProp is NULL in ProcessActMsg2()\n" );
		pMover->SetMotion( pItemProp->dwUseMotion + MTA_FRUNNING1, ANILOOP_LOOP, MOP_FIXED );	// 일단은 다 대기 동작으로 쓴다
		break;
	// 좌/우 턴 해라! 
	case OBJMSG_LTURN:
		{
		FLOAT fTurnAngle = (FLOAT) nParam1 / 100.0f;
		if( GetTurnState() == OBJSTA_LTURN && m_fTurnAngle == fTurnAngle )	return 0;
		m_fTurnAngle = fTurnAngle;
		SetTurnState( OBJSTA_LTURN );
		if( GetState() & OBJSTA_ATK_ALL )	return 0;
		if( !pItemProp )
			Error( "ItemProp is NULL in ProcessActMsg2()\n" );
		if( fTurnAngle > 1.0f )
			pMover->SetMotion( pItemProp->dwUseMotion + MTA_FLTURN1, ANILOOP_LOOP, MOP_FIXED );
		}
		break;
	case OBJMSG_RTURN:
		{
		FLOAT fTurnAngle = (FLOAT) nParam1 / 100.0f;
		if( GetTurnState() == OBJSTA_RTURN && m_fTurnAngle == fTurnAngle )	return 0;
		m_fTurnAngle = fTurnAngle;
		SetTurnState( OBJSTA_RTURN );
		if( GetState() & OBJSTA_ATK_ALL )	return 0;	// 공격중엔 회전모션 하지 않음.
		if( !pItemProp )
			Error( "ItemProp is NULL in ProcessActMsg2()\n" );
		if( fTurnAngle > 1.0f )
			pMover->SetMotion( pItemProp->dwUseMotion + MTA_FRTURN1, ANILOOP_LOOP, MOP_FIXED );
		}
		break;
	case OBJMSG_STOP_TURN:
		if( GetTurnState() == 0 )		return 0;		// 이미 턴 상태가 없다면 처리 안함
		m_fTurnAngle = 0.0f;
		SetTurnState( 0 );		// 턴을 중지
		if( GetState() & OBJSTA_ATK_ALL )	return 0;
		if( !pItemProp )
			Error( "ItemProp is NULL in ProcessActMsg2()\n" );
		if( GetMoveState() == OBJSTA_FMOVE )
			pMover->SetMotion( pItemProp->dwUseMotion + MTA_FRUNNING1, ANILOOP_LOOP, MOP_FIXED );
		else
			pMover->SetMotion( pItemProp->dwUseMotion + MTA_FSTAND1, ANILOOP_LOOP, MOP_FIXED );
		break;
	// 기수 위아래로 들기
	case OBJMSG_LOOKUP:
		if( GetLookState() == OBJSTA_LOOKUP )	return 0;
		SetTurnState( OBJSTA_LOOKUP );
		break;
	case OBJMSG_LOOKDOWN:
		if( GetLookState() == OBJSTA_LOOKDOWN )	return 0;
		SetLookState( OBJSTA_LOOKDOWN );
		break;
	case OBJMSG_STOP_LOOK:
		if( GetLookState() == 0 )		return 0;
		SetLookState( 0 );		// 룩 중지
		break;

	// 공격
	case OBJMSG_ATK1:	// 공격 1
		{
			if( (GetState() & OBJSTA_ATK_ALL) == OBJSTA_ATK1 )	// 이미 공격중이면 리턴
				return 0;
			m_objidHit	= (OBJID)nParam1;	// 타겟의 아이디.
			CMover* pHitObj	= prj.GetMover( m_objidHit );
			if( IsInvalidObj( pHitObj ) || pHitObj->IsDie() )	// 타겟이 거시기하거나 죽었으면 취소
				return -2;

#ifdef __CLIENT				
			if( pMover->IsActiveMover() && pMover->IsRangeObj( pHitObj, 3.0f ) == FALSE )	return 0;	// x미터 이내에 들어와야 휘두름.
#endif
#ifdef __WORLDSERVER
			if( !g_eLocal.GetState( EVE_STEAL ) )
			{
				if( pMover->IsSteal( pHitObj ) )	// pHitObj를 스틸하려하면 이공격은 무시.
					return -2;
			}
#endif
			
			AddStateFlag( OBJSTAF_COMBAT );	// 전투모드로 전환.
			SetState( OBJSTA_ATK_ALL, OBJSTA_ATK1 );
			if( pItemProp )
				pMover->SetMotion( pItemProp->dwUseMotion + MTA_FATK1, ANILOOP_1PLAY, MOP_NO_TRANS | MOP_SWDFORCE | MOP_FIXED );		// 최초 메시지 입력시 동작 설정
			else
				Error( "ItemProp is NULL in ProcessActMsg2()\n" );
			pHitObj->SetJJim( pMover );
			pMover->RemoveInvisible();
			
#ifdef __CLIENT
			if( pMover->IsActiveMover() )
				g_DPlay.SendMeleeAttack2( OBJMSG_ATK1, m_objidHit, 0, 0 );
#endif
		}
		
		break;
	case OBJMSG_ATK_MAGIC1:	// 공격 1
		{
			if( IsAction() )	return 0;
			if( GetState() & OBJSTA_ATK_ALL )	return 0;		// 이미 공격동작을 하고 있으면 취소.
			if( !pItemProp )
				Error( "ItemProp is NULL in ProcessActMsg2()\n" );
			CMover* pHitObj		= prj.GetMover( nParam1 );
			if( IsInvalidObj( pHitObj ) )	return -1;
			
#ifdef __WORLDSERVER
			if( !g_eLocal.GetState( EVE_STEAL ) )
			{
				if( pMover->IsSteal( pHitObj ) )	// pHitObj를 스틸하려하면 이공격은 무시.
					return -2;
			}
#endif
			if( IsActDamage() )
				ResetState( OBJSTA_DMG_ALL );
			AddStateFlag( OBJSTAF_COMBAT );	// 전투모드로 전환.
			
			SendActMsg( OBJMSG_STOP_TURN );
			SendActMsg( OBJMSG_STAND );
			SetState( OBJSTA_ATK_ALL, OBJSTA_ATK_MAGIC1 );
			if( pItemProp )
				pMover->SetMotion( pItemProp->dwUseMotion + MTA_FATK1, ANILOOP_1PLAY, MOP_NO_TRANS | MOP_SWDFORCE | MOP_FIXED );		// 
			pHitObj->SetJJim( pMover );
			pMover->RemoveInvisible();
			
		}
		break;
		

	// 점프
	case OBJMSG_JUMP:
		break;
	case OBJMSG_DAMAGE:
		{
			CMover* pAttacker = PreProcessDamage( pMover, (OBJID)nParam2, nParam4, nParam5 );
			if( pAttacker == NULL )
				return 0;

			int nDamage = _ProcessMsgDmg( dwMsg, pAttacker, (DWORD)nParam1, nParam3, nParam4, nParam5 );
#if __VER >= 10	// __METEONYKER_0608
			pMover->PostAIMsg( AIMSG_DAMAGE, pAttacker->GetId(), nDamage );
#endif	// __METEONYKER_0608
			pAttacker->RemoveInvisible();
			return nDamage;
		}
		break;
	case OBJMSG_DIE:
	#ifdef __WORLDSERVER
		pMover->PostAIMsg( AIMSG_DIE, nParam2 );
	#endif
		SendActMsg( OBJMSG_STAND );	// 죽을땐 제자리에 서라
		ResetState( OBJSTA_TURN_ALL );	// 죽을땐 턴 없애라.
		SetState( OBJSTA_DMG_ALL, OBJSTA_DEAD );
		
		m_vDelta.x = m_vDelta.z = 0;		// 죽었으면 델타값 클리어.
		{
			CMover *pAttacker = (CMover *)nParam2;	
			if( pAttacker )
			{
				if( pMover->IsPlayer() && pAttacker->IsPlayer() )	// 죽은놈과 죽인놈이 플레이어고
				{
					if( pMover->m_nDuel == 1 && pMover->m_idDuelOther == pAttacker->GetId() )	// 둘이 듀얼중이었으면
					{
						pMover->m_nDuel		= 0;	// 어느한쪽이 죽으면 듀얼모드 해제
						pAttacker->m_nDuel	= 0;

						TRACE( "m_nDuel		= 0		// OBJMSG_DIE	// ProcessActMsg2\n" );

						pMover->m_idDuelOther = NULL_ID;
						pAttacker->m_idDuelOther = NULL_ID;
					}
				}
			}
		}
		
		pMover->SetMotion( MTI_DIE1, ANILOOP_CONT );
		if( !pMover->IsPlayer() ) 
		{
		#ifdef __WORLDSERVER
			m_nDeadCnt	= 60*3;	// 5 sec
		#endif // WorldServer
		}
		break;

	case OBJMSG_ACC_START:
// 비행연료 
//		if( pMover->m_nFuel <= 0 )
//			return 0;
		AddStateFlag( OBJSTAF_ACC );
		break;
	case OBJMSG_ACC_STOP:
		RemoveStateFlag( OBJSTAF_ACC );
		break;

	case OBJMSG_MODE_GROUND:
		if( nParam3 == 0 )
		{
			CWorld* pWorld	= pMover->GetWorld();
			if( pWorld )
			{
				int nAttr = pMover->GetWorld()->GetHeightAttribute( pMover->GetPos().x, pMover->GetPos().z );		// 이동할 위치의 속성 읽음.
				if( nAttr == HATTR_NOWALK )		// 걷기 금지 지역에서 내리려고하면 못내린다.
					return 0;
			}
		}
		RemoveStateFlag( OBJSTAF_FLY );
		RemoveStateFlag( OBJSTAF_ACC );		// 가속 정지.
		ClearState();
#ifdef __CLIENT
		if( pMover->IsActiveMover() && g_WndMng.m_pWndWorld )
		{
			g_WndMng.m_pWndWorld->SetMouseMode( 0 );		// 비행모드 해제되면 FPS모드해제
			g_WndMng.m_pWndWorld->m_fRollAng = 0;
		}
#endif		

#ifndef __JEFF_9_20
#ifdef __WORLDSERVER
		if( pMover->IsPlayer() )
		{
			((CUser*)pMover)->SetFlyTimeOff();
		}
#endif // __WORLDSERVER
#endif	// __JEFF_9_20
		pMover->SetAngleX(0);
		#ifdef __CLIENT
		if( m_pMover == CMover::GetActiveMover() )
			g_Neuz.m_camera.Lock();
		#endif
		m_pMover->SetRide( NULL );
		break;
	case OBJMSG_MODE_TURBO_ON:		// 터보모드 상태.
		if( GetStateFlag() & OBJSTAF_TURBO )		return 0;		// 이미 실행중
		if( pMover->m_tmAccFuel <= 0 )	return 0;				// 가속연료없으면 터보 못함.
		AddStateFlag( OBJSTAF_TURBO );
		break;
	case OBJMSG_MODE_TURBO_OFF:		// 터보모드 해제.
		if( (GetStateFlag() & OBJSTAF_TURBO) == 0 )		return 0;		// 이미 해제
		RemoveStateFlag( OBJSTAF_TURBO );
		break;


	case OBJMSG_TURNMOVE:
	{
		if( IsActAttack() )	return -2;
		if( IsActDamage() )	return -3;
		SendActMsg( OBJMSG_STOP_TURN );
		SendActMsg( OBJMSG_ACC_START );
		pMover->SetAngle( (float)nParam1 );
		pMover->SetAngleX( (float)nParam2 );

		{
			float fAng	= D3DXToRadian( nParam1 );
			float fAngX		= D3DXToRadian( nParam2 );
			int nRemnant	= pMover->m_uRemnantCorrFrm;
			if( nRemnant == 0 )
			{
				Error( "OBJMSG_TURNMOVE" );
				return 0;
			}
			float f		= 2.0f * m_fDistance / ( 0.10f * nRemnant * nRemnant );
			float fDist	= cosf( fAngX ) * f;
			m_vDelta.x		= sinf( fAng ) * fDist;
			m_vDelta.z		= -cosf( fAng ) * fDist;
			m_vDelta.y		= -sinf( fAngX ) * ( f * cosf( fAngX ) );
		}
		break;
	}
	case OBJMSG_TEMP2:	// 디버깅용 공격동작만 볼때.
#ifdef __CLIENT
		if( CMover::GetActiveMover()->IsAuthHigher( AUTH_GAMEMASTER ) )	// 영자 계정일때
		{
			CObj *pObj = pMover->GetWorld()->GetObjFocus();
			g_DPlay.SendCorrReq( pObj );
		}
#endif
		break;
		
	}
	return 1;
}

