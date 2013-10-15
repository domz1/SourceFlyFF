#include "stdafx.h"
#include "defineObj.h"
#include "defineSkill.h"
#include "ActionMover.h"
#include "ErrorCode.h"
#include "party.h"

#ifdef __CLIENT
	#include "DPClient.h"
#else
	#include "user.h"
	#include "dpcoreclient.h"
	#include "dpdatabaseclient.h"
	#include "..\_AIInterface\AIInterface.h"
#endif	

#if __VER >= 12 // __NEW_ITEMCREATEMON_SERVER
#include "CreateMonster.h"
#endif // __NEW_ITEMCREATEMON_SERVER



extern	CPartyMng	g_PartyMng;

#ifdef __CLIENT
	extern	CDPClient	g_DPlay;
#else
	extern	CUserMng			g_UserMng;
	extern	CDPDatabaseClient	g_dpDBClient;
	extern	CDPCoreClient		g_DPCoreClient;
#endif 

// 현재 상태를 무시하고 새로운 동작으로 강제 세팅.
// 서버와 동기화 할때외엔 사용하지 말것.
void	CActionMover::ForcedSet( D3DXVECTOR3 &vDelta, DWORD dwState, DWORD dwStateFlag, int nMotionEx )
{
	if( ( GetStateFlag() & OBJSTAF_FLY ) == 0 && ( dwStateFlag & OBJSTAF_FLY ) )
		return;
	if( ( GetStateFlag() & OBJSTAF_FLY ) && ( dwStateFlag & OBJSTAF_FLY ) == 0 )
		return;
	if( ( GetStateFlag() & OBJSTAF_SIT ) == 0 && ( dwStateFlag & OBJSTAF_SIT ) )
		return;
	if( ( GetStateFlag() & OBJSTAF_SIT ) && ( dwStateFlag & OBJSTAF_SIT ) == 0 )
		return;

	m_vDelta = vDelta;
	__ForceSetState( dwState );	// 여기 이외엔 이함수 사용하지 말것.
			
	ClearStateFlag();
	AddStateFlag( dwStateFlag );

	if( (GetStateFlag() & OBJSTAF_SIT) && ((dwState & OBJSTA_MOVE_ALL) != OBJSTA_SIT) )	// IsSit()인데
		Error( "ForcedSet : dwState=0x%08x, dwStateFlag=0x%08x", GetState(), GetStateFlag() );
	else if( (GetStateFlag() & OBJSTAF_SIT) == 0 && ((dwState & OBJSTA_MOVE_ALL) == OBJSTA_SIT) )	// IsSit()아닌데 SIT이면
		Error( "ForcedSet : dwState=0x%08x, dwStateFlag=0x%08x", GetState(), GetStateFlag() );

	m_nMotionEx = nMotionEx;
	DefaultSet();
}

void	CActionMover::ForcedSet2( D3DXVECTOR3 &vDelta, float fAccPower, float fTurnAngle, DWORD dwState, DWORD dwStateFlag, int nMotionEx )
{
	if( ( GetStateFlag() & OBJSTAF_FLY ) == 0 && ( dwStateFlag & OBJSTAF_FLY ) )
		return;
	if( ( GetStateFlag() & OBJSTAF_FLY ) && ( dwStateFlag & OBJSTAF_FLY ) == 0 )
		return;

	m_vDelta = vDelta;
	m_fAccPower	= fAccPower;
	m_fTurnAngle	= fTurnAngle;
	__ForceSetState( dwState );
	ClearStateFlag();
	AddStateFlag( dwStateFlag );
	m_nMotionEx = nMotionEx;
	DefaultSet();
	if( (GetStateFlag() & OBJSTAF_SIT) && (dwState != OBJSTA_SIT))	// IsSit()인데
		Error( "ForcedSet2 : dwState=0x%08x, dwStateFlag=0x%08x", GetState(), GetStateFlag() );
	else if( (GetStateFlag() & OBJSTAF_SIT) == 0 && (dwState == OBJSTA_SIT) )	// IsSit()아닌데 SIT이면
		Error( "ForcedSet2 : dwState=0x%08x, dwStateFlag=0x%08x", GetState(), GetStateFlag() );
}

void CActionMover::HalfForcedSet( D3DXVECTOR3 & vd, float fAccPower, float fTurnAngle )
{
	m_vDelta	 = vd;
	m_fAccPower	 = fAccPower;
	m_fTurnAngle = fTurnAngle;
	DefaultSet();
}

void	CActionMover::DefaultSet( void )
{
	m_pMover->m_fCrrSpd	= 0.0f;
}

//
//	Action Message Process
//	액션 메시지를 받아 처리한다.
//	어떤 행위가 발생하는 시점에 대한 처리를 담당.
//	최적화를 위해서 이미 설정되어 있는 상태면 중복 처리 하지 않음
//
int		CActionMover::ProcessActMsg1( CMover* pMover,  OBJMSG dwMsg, int nParam1, int nParam2, int nParam3, int nParam4, int nParam5 )
{
	CModelObject* pModel = (CModelObject*)pMover->m_pModel;

	switch( dwMsg )
	{
	// 평화모드 제자리에 서있어라!
	case OBJMSG_STAND:
#ifdef __Y_INTERFACE_VER3
		if( (GetMoveState() == OBJSTA_FMOVE) || (GetMoveState() == OBJSTA_BMOVE) || (GetMoveState() == OBJSTA_LMOVE) || (GetMoveState() == OBJSTA_RMOVE) )	// 전/후진중일때 제자리에 세운다.
#else //__Y_INTERFACE_VER3
		if( (GetMoveState() == OBJSTA_FMOVE) || (GetMoveState() == OBJSTA_BMOVE) )	// 전/후진중일때 제자리에 세운다.
#endif //__Y_INTERFACE_VER3
		{
			ResetState( OBJSTA_MOVE_ALL );		
			m_vDelta.x = m_vDelta.z = 0;
			if( pMover->IsFlyingNPC() )
			{
				m_vDelta.y = 0;
				pMover->SetAngleX(0);
			}
		}

		if( GetMoveState() == OBJSTA_STAND )	return 0;
		if( GetMoveState() == OBJSTA_PICKUP )	return 0;
		if( IsActJump() )		return -1;
		if( IsActAttack() )		return -2;
		if( IsActDamage() )		return -3;
		if( IsDie() )			return -4;
		if( IsAction() )		return 0;
		SetMoveState( OBJSTA_STAND );
		pMover->SetMotion( MTI_STAND );
		RemoveStateFlag( OBJSTAF_ETC );
		break;
	case OBJMSG_STOP:		
	case OBJMSG_ASTOP:
	#ifdef __Y_INTERFACE_VER3
		if( (GetMoveState() == OBJSTA_FMOVE) || (GetMoveState() == OBJSTA_BMOVE) || (GetMoveState() == OBJSTA_LMOVE) || (GetMoveState() == OBJSTA_RMOVE) )	// 전/후진중일때 제자리에 세운다.
	#else //__Y_INTERFACE_VER3
		if( (GetMoveState() == OBJSTA_FMOVE) || (GetMoveState() == OBJSTA_BMOVE) )	// 전/후진중일때 제자리에 세운다.
	#endif //__Y_INTERFACE_VER3
		{
			ResetState( OBJSTA_MOVE_ALL );		
			m_vDelta.x = m_vDelta.z = 0;
			if( pMover->IsFlyingNPC() )
			{
				m_vDelta.y = 0;
				pMover->SetAngleX(0);
			}
		}
		pMover->ClearDest();
		return 0;	// ControlGround에서 키입력없을때 STOP을 부르는데 거기서 리턴값을 맞추기 위해 이렇게 했음. 
		break;
	case OBJMSG_SITDOWN:
		if( GetStateFlag() & OBJSTAF_SIT )	return 0;		// 이미 앉은모드면 리턴 - 플래그로 검사하지 말고 state로 직접검사하자.

		if( nParam3 == 0 )
		{
			if( IsActJump() )	return 0;
			if( IsActAttack() )	return 0;
			if( IsActDamage() )	return 0;
			if( IsAction() )	return 0;
		#ifdef __CLIENT
			if( pMover->IsActiveMover() && (pMover->m_dwReqFlag & REQ_USESKILL) )	return 0;		// 서버로부터 useskill응답이 오기전까진 액션해선 안됨.
		#endif	// __CLIENT
		}
		AddStateFlag( OBJSTAF_SIT );		
		SendActMsg( OBJMSG_STOP );
		pMover->SetMotion( MTI_SIT, ANILOOP_CONT );		// 앉기 모션시작
		SetMoveState( OBJSTA_SIT );
		break;
	case OBJMSG_STANDUP:
		if( (GetStateFlag() & OBJSTAF_SIT) == 0 )	return 0;	

		if( nParam3 == 0 )
		{
			if( IsActJump() )	return 0;
			if( IsActAttack() )	return 0;
			if( IsActDamage() )	return 0;
			if( IsAction() )	return 0;
#ifdef __CLIENT
			if( pMover->IsActiveMover() && (pMover->m_dwReqFlag & REQ_USESKILL) )	return 0;		// 서버로부터 useskill응답이 오기전까진 액션해선 안됨.
#endif	// __CLIENT
		}
		SetMoveState( OBJSTA_SIT );
		AddStateFlag( OBJSTAF_SIT );		 
		pMover->SetMotion( MTI_GETUP, ANILOOP_CONT );
		break;
	case OBJMSG_PICKUP:
		if( IsSit() )	return 0;
		if( IsAction() )	return 0;
		if( IsActJump() )	return 0;
		if( IsActAttack() )	return 0;
		if( IsActDamage() )	return 0;
#ifdef __CLIENT
		if( pMover->IsActiveMover() )
		{
			if( pMover->m_dwReqFlag & REQ_USESKILL )	return 0;		// 서버로부터 useskill응답이 오기전까진 액션해선 안됨.
			if( pMover->m_dwReqFlag & REQ_MOTION )		return 0;	// 서버로부터 모션 응답이 오기전까지 액션 금지.
		}
#endif	// __CLIENT
		if( GetMoveState() == OBJSTA_PICKUP )	return 0;	// 이미 집고 있으면 추가로 실행되지 않게 .
		SetMoveState( OBJSTA_PICKUP );
		pMover->SetMotion( MTI_PICKUP, ANILOOP_1PLAY, MOP_FIXED );
		break;

	case OBJMSG_COLLECT:
		
		//if( IsSit() )		return 0;
		//if( IsAction() )	return 0;
		//if( IsActJump() )	return 0;
		//if( IsActAttack() )	return 0;
		//if( IsActDamage() )	return 0;
		//SetState( OBJSTA_ACTION_ALL, OBJSTA_COLLECT );
		//pMover->SetMotion( MTI_COLLECT, ANILOOP_LOOP, MOP_FIXED );
		//pMover->RemoveInvisible();
		
		// 康	// 채집
		ClearState();
		ResetState( OBJSTA_ACTION_ALL );
		SetState( OBJSTA_ACTION_ALL, OBJSTA_COLLECT );
		pMover->m_dwFlag |= MVRF_NOACTION;
		pMover->SetMotion( MTI_COLLECT, ANILOOP_LOOP, MOP_FIXED );
		pMover->RemoveInvisible();
		break;

	case OBJMSG_STUN:
		SetState( OBJSTA_ACTION_ALL, OBJSTA_STUN );		// 스턴은 어떤 상태에서도 걸릴 수 있으니 상태는 무조것 세팅하자.
		pMover->m_dwFlag |= MVRF_NOACTION;				// 액션 금지 상태로 전환.
		pMover->m_wStunCnt = (short)nParam1;			// 얼마동안이나 스턴상태가 되느냐.

		if( GetState() == OBJSTA_STAND )
			pMover->SetMotion( MTI_GROGGY, ANILOOP_LOOP, MOP_FIXED );		// 아무것도 안하는 스탠딩 상태면 스턴 모션 내자.
		break;
		
	case OBJMSG_FALL:
		if( IsActJump() )	return 0;

		// 무적이면 여기서 리턴.
		if( pMover->m_dwMode & MATCHLESS_MODE )		
			return 0;
		
		pMover->m_dwFlag |= MVRF_NOACTION;				// 액션 금지 상태로 전환.
		
		if( GetState() != OBJSTA_DMG_FLY )
		{
			SetState( OBJSTA_DMG_ALL, OBJSTA_DMG_FLY );	
			pMover->SetMotion( MTI_DMGFLY, ANILOOP_LOOP, MOP_FIXED );
		}
		break;
		
	// 전진해라!
	case OBJMSG_TURNMOVE:
		if( GetMoveState() == OBJSTA_FMOVE && (int)pMover->GetAngle() == nParam1 )	return 0;	// 전진 상태이며 각도가 같으면 메시지 무효 
		if( pMover->m_dwFlag & MVRF_NOMOVE )	
			return 0;		// 이동금지 상태면 걍 리턴.
#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
		if( pMover->GetAdjParam( DST_CHRSTATE ) & CHS_LOOT)			return 0;
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans
		
		if( GetState() & OBJSTA_DMG_FLY_ALL )	// 날라가는 동작중이면 취소
			return 0;
		
		if( IsActAttack() )		return -2;
		if( GetDmgState() == OBJSTA_DMG )	// 피격 중 명령이면 피격 상태를 클리어 시킨다.
			ResetState( OBJSTA_DMG_ALL );
		else if( GetState() & OBJSTA_DMG_ALL )	// 그외 피격동작은 다 취소
			return -3;
		pMover->SetAngle( (float)nParam1 );
		if( pMover->IsFlyingNPC() )
			pMover->SetAngleX( (float)nParam2 );
		SendActMsg( OBJMSG_STOP_TURN );
		SendActMsg( OBJMSG_FORWARD );
		break;
	case OBJMSG_TURNMOVE2:
		if( GetMoveState() == OBJSTA_FMOVE && (int)pMover->GetAngle() == nParam1 )	return 0;	// 전진 상태이며 각도가 같으면 메시지 무효 
		if( pMover->m_dwFlag & MVRF_NOMOVE )	return 0;		// 이동금지 상태면 걍 리턴.
#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
		if( pMover->GetAdjParam( DST_CHRSTATE ) & CHS_LOOT)			return 0;
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans

		if( IsAction() )	return 0;

		if( GetState() & OBJSTA_DMG_FLY_ALL )	// 날라가는 동작중이면 취소
			return 0;

		if( IsActJump() )	return -1;	// 점프거나 공격 상태면 메시지 무효 처리
		if( IsActAttack() )		return -2;
		if( GetDmgState() == OBJSTA_DMG )	// 피격 중 명령이면 피격 상태를 클리어 시킨다.
			ResetState( OBJSTA_DMG_ALL );
		else if( GetState() & OBJSTA_DMG_ALL )	// 그외 피격동작은 다 취소
			return -3;
		pMover->SetAngle( (float)nParam1 );
		if( pMover->IsFlyingNPC() )
			pMover->SetAngleX( (float)nParam2 );
		SendActMsg( OBJMSG_STOP_TURN );
		SendActMsg( OBJMSG_BACKWARD );
		break;

#ifdef __Y_INTERFACE_VER3
	case OBJMSG_LFORWARD:
		if( IsActJump() )	return -1;
		if( IsActAttack() )		return -2;
		if( GetDmgState() == OBJSTA_DMG )	// 피격 중 명령이면 피격 상태를 클리어 시킨다.
			ResetState( OBJSTA_DMG_ALL );
		else
			if( GetState() & OBJSTA_DMG_ALL )	// 그외 피격동작은 다 취소
				return -3;
		if( GetMoveState() == OBJSTA_LMOVE )	return 0;	// 이미 전진상태면 추가로 처리 하지 않음
		if( IsSit() )		return 0;		// 리턴 0으로 해도 되나?.
		if( pMover->m_dwFlag & MVRF_NOMOVE )	return 0;		// 이동금지 상태면 걍 리턴.
#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
		if( pMover->GetAdjParam( DST_CHRSTATE ) & CHS_LOOT)			return 0;
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans

		SetMoveState( OBJSTA_LMOVE );
		if( IsStateFlag( OBJSTAF_WALK ) )	// 걷는모드
		{
			pMover->SetMotion( MTI_WALK );		// 전투모드로 걷기
		} 
		else
		{
			if( pMover->SetMotion( MTI_RUN ) )
				pModel->SetMotionBlending( TRUE );
		}
		break;
	case OBJMSG_RFORWARD:
		if( IsActJump() )	return -1;
		if( IsActAttack() )		return -2;
		if( GetDmgState() == OBJSTA_DMG )	// 피격 중 명령이면 피격 상태를 클리어 시킨다.
			ResetState( OBJSTA_DMG_ALL );
		else
			if( GetState() & OBJSTA_DMG_ALL )	// 그외 피격동작은 다 취소
				return -3;
		if( GetMoveState() == OBJSTA_RMOVE )	return 0;	// 이미 전진상태면 추가로 처리 하지 않음
		if( IsSit() )		return 0;		// 리턴 0으로 해도 되나?.
		if( pMover->m_dwFlag & MVRF_NOMOVE )	return 0;		// 이동금지 상태면 걍 리턴.
#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
		if( pMover->GetAdjParam( DST_CHRSTATE ) & CHS_LOOT)			return 0;
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans

		SetMoveState( OBJSTA_RMOVE );
		if( IsStateFlag( OBJSTAF_WALK ) )	// 걷는모드
		{
			pMover->SetMotion( MTI_WALK );		// 전투모드로 걷기
		} 
		else
		{
			if( pMover->SetMotion( MTI_RUN ) )
				pModel->SetMotionBlending( TRUE );
		}
		break;
#endif //__Y_INTERFACE_VER3

	case OBJMSG_FORWARD:
		if( IsActJump() )	return -1;
		if( IsActAttack() )		return -2;
		if( GetDmgState() == OBJSTA_DMG )	// 피격 중 명령이면 피격 상태를 클리어 시킨다.
			ResetState( OBJSTA_DMG_ALL );
		else if( GetState() & OBJSTA_DMG_ALL )	// 그외 피격동작은 다 취소
				return -3;
		if( GetMoveState() == OBJSTA_FMOVE )	return 0;	// 이미 전진상태면 추가로 처리 하지 않음
		if( IsSit() )		return 0;		// 리턴 0으로 해도 되나?.
		if( pMover->m_dwFlag & MVRF_NOMOVE )	return 0;		// 이동금지 상태면 걍 리턴.
#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
		if( pMover->GetAdjParam( DST_CHRSTATE ) & CHS_LOOT)			return 0;
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans

		SetMoveState( OBJSTA_FMOVE );
		if( IsStateFlag( OBJSTAF_WALK ) )	// 걷는모드
		{
			pMover->SetMotion( MTI_WALK );	
		} 
		else
		{
			if( pMover->SetMotion( MTI_RUN ) )
				pModel->SetMotionBlending( TRUE );
		}
		pMover->OnActFMove();
		break;
		
	case OBJMSG_STOP_RUN:
	#ifdef __CLIENT
		if( IsActJump() )	return -1;
		if( IsActAttack() )		return -2;
		if( GetDmgState() == OBJSTA_DMG )	// 피격 중 명령이면 피격 상태를 클리어 시킨다.
			ResetState( OBJSTA_DMG_ALL );
		else if( GetState() & OBJSTA_DMG_ALL )	// 그외 피격동작은 다 취소
			return -3;
		if( GetMoveState() == OBJSTA_STOP_RUN )	return 0;	// 이미 전진상태면 추가로 처리 하지 않음
		if( IsSit() )		return 0;						// 리턴 0으로 해도 되나?.
		if( pMover->m_dwFlag & MVRF_NOMOVE )	return 0;	// 이동금지 상태면 걍 리턴.
#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
		if( pMover->GetAdjParam( DST_CHRSTATE ) & CHS_LOOT)			return 0;
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans

		SetMoveState( OBJSTA_STOP_RUN );
	#endif
		break;

	
	// 후진해라!
	case OBJMSG_BACKWARD:
		if( IsActJump() )	return -1;	// if jump, x
		if( IsActAttack() )		return -2;	// 공격동작중이면 취소됨.
		if( IsActDamage() )		return -3;
		if( GetMoveState() == OBJSTA_BMOVE )	return 0;
		if( IsSit() )		return 0;		// 리턴 0으로 해도 되나?.
		if( IsAction() )	return 0;
		if( pMover->m_dwFlag & MVRF_NOMOVE )	return 0;		// 이동금지 상태면 걍 리턴.
#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
		if( pMover->GetAdjParam( DST_CHRSTATE ) & CHS_LOOT)			return 0;
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans

		SetMoveState( OBJSTA_BMOVE );
		pMover->SetMotion( MTI_WALK );		
		break;
	// 좌/우 턴 해라!
	case OBJMSG_LTURN:
		if( IsActJump() )	return -1;
		if( IsActAttack() )		return -2;
		if( GetDmgState() == OBJSTA_DMG )	// 피격 중 명령이면 피격 상태를 클리어 시킨다.
			ResetState( OBJSTA_DMG_ALL );
		else if( GetState() & OBJSTA_DMG_ALL )	// 그외 피격동작은 다 취소
				return -3;
		if( GetTurnState() == OBJSTA_LTURN )	return 0;
		if( IsSit() )		return 0;		// 리턴 0으로 해도 되나?.
		if( IsAction() )	return 0;
		if( pMover->m_dwFlag & MVRF_NOMOVE )	return 0;		// 이동금지 상태면 걍 리턴.
#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
		if( pMover->GetAdjParam( DST_CHRSTATE ) & CHS_LOOT)			return 0;
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans

		SetTurnState( OBJSTA_LTURN );
		break;
	case OBJMSG_RTURN:
		if( IsActJump() )	return -1;
		if( IsActAttack() )		return -2;
		if( GetDmgState() == OBJSTA_DMG )	// 피격 중 명령이면 피격 상태를 클리어 시킨다.
			ResetState( OBJSTA_DMG_ALL );
		else if( GetState() & OBJSTA_DMG_ALL )	// 그외 피격동작은 다 취소
			return -3;
		if( GetTurnState() == OBJSTA_RTURN )	return 0;
		if( IsSit() )		return 0;		// 리턴 0으로 해도 되나?.
		if( IsAction() )	return 0;
		if( pMover->m_dwFlag & MVRF_NOMOVE )	return 0;		// 이동금지 상태면 걍 리턴.
#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
		if( pMover->GetAdjParam( DST_CHRSTATE ) & CHS_LOOT)			return 0;
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans

		SetTurnState( OBJSTA_RTURN );
		break;
	case OBJMSG_STOP_TURN:
		if( GetTurnState() == 0 )		return 0;		// 이미 턴 상태가 없다면 처리 안함
		SetTurnState( 0 );		// 턴을 중지
		break;

	// 피치를 들어올림. 비행몹의 경우만 사용.
	case OBJMSG_LOOKUP:
		if( IsActJump() )	return -1;
		if( IsActAttack() )		return -2;
		if( GetDmgState() == OBJSTA_DMG )	// 피격 중 명령이면 피격 상태를 클리어 시킨다.
			ResetState( OBJSTA_DMG_ALL );
		else if( GetState() & OBJSTA_DMG_ALL )	// 그외 피격동작은 다 취소
			return -3;
		if( GetTurnState() == OBJSTA_LOOKUP )	return 0;
		if( IsSit() )		return 0;		// 리턴 0으로 해도 되나?.
		if( IsAction() )	return 0;
		if( pMover->m_dwFlag & MVRF_NOMOVE )	return 0;		// 이동금지 상태면 걍 리턴.
#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
		if( pMover->GetAdjParam( DST_CHRSTATE ) & CHS_LOOT)			return 0;
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans

		SetLookState( OBJSTA_LOOKUP );
		break;
	case OBJMSG_LOOKDOWN:
		if( IsActJump() )	return -1;
		if( IsActAttack() )		return -2;
		if( GetDmgState() == OBJSTA_DMG )	// 피격 중 명령이면 피격 상태를 클리어 시킨다.
			ResetState( OBJSTA_DMG_ALL );
		else if( GetState() & OBJSTA_DMG_ALL )	// 그외 피격동작은 다 취소
			return -3;
		if( GetTurnState() == OBJSTA_LOOKDOWN )	return 0;
		if( IsSit() )		return 0;		// 리턴 0으로 해도 되나?.
		if( IsAction() )	return 0;
		if( pMover->m_dwFlag & MVRF_NOMOVE )	return 0;		// 이동금지 상태면 걍 리턴.
#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
		if( pMover->GetAdjParam( DST_CHRSTATE ) & CHS_LOOT)			return 0;
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans

		SetLookState( OBJSTA_LOOKDOWN );
		break;
	case OBJMSG_STOP_LOOK:
		if( GetLookState() == 0 )		return 0;
		SetLookState( 0 );		// 룩 중지
		break;
		// 점프
	case OBJMSG_JUMP:
		if( IsActJump() )		return 0;
		if( IsActAttack() )		return -2;
		if( IsActDamage() )		return -3;
		if( IsSit() )			return 0;		// 리턴 0으로 해도 되나?.
		if( IsAction() )		return 0;
		if( pMover->m_dwFlag & MVRF_NOMOVE )	return 0;		// 이동금지 상태면 걍 리턴.
#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
		if( pMover->GetAdjParam( DST_CHRSTATE ) & CHS_LOOT)			return 0;
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans

#ifdef __CLIENT
		if( pMover->IsActiveMover() )
		{
			if( pMover->m_dwReqFlag & REQ_USESKILL )	return 0;	// 서버로부터 useskill응답이 오기전까진 점프해선 안됨.
			if( pMover->m_dwReqFlag & REQ_MOTION )		return 0;	// 서버로부터 모션 응답이 오기전까지 점프 금지.
		}
#endif	// __CLIENT
		SendActMsg( OBJMSG_STOP_TURN );
		SetJumpState( OBJSTA_SJUMP1 );
		if( (GetState() & OBJSTA_COLLECT)== 0 ) pMover->SetMotion( MTI_JUMP1, ANILOOP_1PLAY );
		pModel->SetMotionBlending( FALSE );
		break;
	// 완드공격
	case OBJMSG_ATK_MAGIC1:	// 공격 1
		{
			if( IsAction() )	return 0;
			if( IsSit() )		// 앉아있는상태였으면 해제
				ResetState( OBJSTA_MOVE_ALL );
			if( GetState() & OBJSTA_ATK_ALL )	return 0;		// 이미 공격동작을 하고 있으면 취소.
			if( pMover->m_dwFlag & MVRF_NOATTACK )	return -1;		// 공격금지 상태면 걍 리턴.
			if( pMover->IsMode( NO_ATTACK_MODE ) ) return -1;
			CMover* pHitObj		= prj.GetMover( nParam1 );
			if( IsInvalidObj( pHitObj ) )	return -1;
			if( pHitObj->IsDie() )	return -1;
#ifdef __WORLDSERVER
			if( !g_eLocal.GetState( EVE_STEAL ) )
			{
				if( pMover->IsSteal( pHitObj ) )	// pHitObj를 스틸하려하면 이공격은 무시.
					return -1;
			}
#endif
			if( IsActDamage() )
				ResetState( OBJSTA_DMG_ALL );
			if( IsStateFlag( OBJSTAF_COMBAT ) == FALSE )
				SendActMsg( OBJMSG_MODE_COMBAT );
		
			SendActMsg( OBJMSG_STOP_TURN );
			SendActMsg( OBJMSG_STAND );
			SetState( OBJSTA_ATK_ALL, OBJSTA_ATK_MAGIC1 );
			pMover->SetMotion( MTI_ATK1, ANILOOP_1PLAY );		// 완드동작이 없으므로 공격동작으로 대신함.
			pHitObj->SetJJim( pMover );
			pMover->m_nAtkCnt = 1;		// 카운트 시작.
			pMover->OnAttackStart( pHitObj, dwMsg );			// 공격시작 핸들러.
		}
		break;
		// 레인지공격
	case OBJMSG_ATK_RANGE1:	// 공격 1
		{
			if( IsAction() )	return 0;
			if( pMover->m_dwFlag & MVRF_NOATTACK )	return -1;		// 공격금지 상태면 걍 리턴.
			if( pMover->IsMode( NO_ATTACK_MODE ) ) return -1;
			if( IsSit() )		// 앉아있는상태였으면 해제
				ResetState( OBJSTA_MOVE_ALL );
			if( GetState() & OBJSTA_ATK_ALL )	return 0;		// 이미 공격동작을 하고 있으면 취소.
			CMover* pHitObj		= prj.GetMover( nParam1 );
			if( IsValidObj( (CObj*)pHitObj ) == FALSE )	
				return -1;
			if( pHitObj->IsDie() )
				return -1;

			m_idTarget = (DWORD)nParam1;		// 공격 타겟.

#ifdef __WORLDSERVER
			if( !g_eLocal.GetState( EVE_STEAL ) )
			{
				if( pMover->IsSteal( pHitObj ) )	// pHitObj를 스틸하려하면 이공격은 무시.
					return -1;
			}
#endif
			if( GetDmgState() == OBJSTA_DMG )	// 피격 중 공격명령이면 피격 상태를 클리어 시킨다.
				ResetState( OBJSTA_DMG_ALL );
			else if( GetState() & OBJSTA_DMG_ALL )	// 그외 피격동작은 다 취소
				return -1;
			if( IsStateFlag( OBJSTAF_COMBAT ) == FALSE )
				SendActMsg( OBJMSG_MODE_COMBAT );
			
			int nUseMotion = nParam2;
//			SendActMsg( OBJMSG_STOP_TURN );
//			SendActMsg( OBJMSG_STAND );
			SetState( OBJSTA_ATK_ALL, OBJSTA_ATK_RANGE1 );
			pMover->SetMotion( nUseMotion, ANILOOP_1PLAY );		// 완드동작이 없으므로 공격동작으로 대신함.
			pHitObj->SetJJim( pMover );			// 공격이 시작되면 타겟에다가 내가 찜했다는걸 표시.
			pMover->OnAttackStart( pHitObj, dwMsg );			// 공격시작 핸들러.
			pMover->m_nAtkCnt = 1;		// 카운트 시작.
		}
		break;
		// 공격 - 리턴값 -2는 클라로 부터 받은 명령을 완전 무시한다.
	case OBJMSG_ATK1:	// 공격 1
	case OBJMSG_ATK2:	// 공격 2
	case OBJMSG_ATK3:	// 공격 3	
	case OBJMSG_ATK4:	// 공격 4
		{
			if( IsAction() )
				return 0;
			if( pMover->m_dwFlag & MVRF_NOATTACK )
				return -2;		// 공격금지 상태면 걍 리턴.
			if( pMover->IsMode( NO_ATTACK_MODE ) )
				return -2;

#ifdef __CLIENT
			if( pMover->IsActiveMover() )
#endif
			{
				// 기본은 오른손 검사( 완드용 )
				CItemElem* pItemElem = pMover->GetWeaponItem();
#ifdef __CROSSBOW
				if( pItemElem && (pItemElem->GetProp()->dwItemKind3 == IK3_WAND || pItemElem->GetProp()->dwItemKind3 == IK3_BOW || pItemElem->GetProp()->dwItemKind3 == IK3_CROSSBOW ))
#else // __CROSSBOW
				if( pItemElem && (pItemElem->GetProp()->dwItemKind3 == IK3_WAND || pItemElem->GetProp()->dwItemKind3 == IK3_BOW ) )
#endif // __CROSSBOW	
					return -2;
			}
				
			CMover* pHitObj		= prj.GetMover( nParam1 );
			if( IsValidObj( (CObj*)pHitObj ) == FALSE )
				return( -1 );

#ifdef __XUZHU
			if( pMover->IsPlayer() )
			{
				int a = 0;
			}
#endif // xuzhu
			m_idTarget = (DWORD)nParam1;		// 공격 타겟.
#ifdef __WORLDSERVER
			if( !g_eLocal.GetState( EVE_STEAL ) )
			{
				if( pMover->IsSteal( pHitObj ) )	// pHitObj를 스틸하려하면 이공격은 무시.
					return -2;
			}
#endif
			if( pHitObj->m_pActMover->IsDie() )	return( -1 );
			if( GetState() & OBJSTA_ATK_ALL )	return( 0 );	// 이미 공격중이면 리턴
			if( IsSit() )		// 앉아있는상태였으면 해제
				ResetState( OBJSTA_MOVE_ALL );
			if( GetDmgState() == OBJSTA_DMG )	// 피격 중 공격명령이면 피격 상태를 클리어 시킨다.
				ResetState( OBJSTA_DMG_ALL );
			else
			if( GetState() & OBJSTA_DMG_ALL )	// 그외 피격동작은 다 취소
				return -3;
			if( IsStateFlag( OBJSTAF_COMBAT ) == FALSE )
				SendActMsg( OBJMSG_MODE_COMBAT );
			SendActMsg( OBJMSG_STOP_TURN );	// 공격할땐 턴을 정지시킴
			SendActMsg( OBJMSG_STAND );
			

			DWORD dwOption = MOP_SWDFORCE | MOP_NO_TRANS;
			
#ifdef __XSLOW1018
			int nMin, nTemp;
			pMover->GetHitMinMax( &nMin, &nTemp );
			if( dwMsg == OBJMSG_ATK3 || dwMsg == OBJMSG_ATK4 )
				if( pMover->GetLevel() >= 12 && pHitObj->GetHitPoint() < (int)(nMin * 1.5f) )
					dwOption |= MOP_HITSLOW;
#endif // xSlow1018
			if( pMover->IsPlayer() )
				pMover->m_fAniSpeed = pMover->GetAttackSpeed();
			
			switch( dwMsg )
			{
				case OBJMSG_ATK1:
					SetState( OBJSTA_ATK_ALL, OBJSTA_ATK1 );
					pMover->SetMotion( MTI_ATK1, ANILOOP_1PLAY, dwOption );		// 최초 메시지 입력시 동작 설정
					break;
				case OBJMSG_ATK2:	
					SetState( OBJSTA_ATK_ALL, OBJSTA_ATK2 );
					pMover->SetMotion( MTI_ATK2, ANILOOP_1PLAY, dwOption );		// 최초 메시지 입력시 동작 설정
					break;
				case OBJMSG_ATK3:	
					SetState( OBJSTA_ATK_ALL, OBJSTA_ATK3 );
					pMover->SetMotion( MTI_ATK3, ANILOOP_1PLAY, dwOption );		// 최초 메시지 입력시 동작 설정
					break;
				case OBJMSG_ATK4:	
					SetState( OBJSTA_ATK_ALL, OBJSTA_ATK4 );
					pMover->SetMotion( MTI_ATK4, ANILOOP_1PLAY, dwOption );		// 최초 메시지 입력시 동작 설정
					break;
			}
			pMover->SetAngle( GetDegree( pHitObj->GetPos(), m_pMover->GetPos() ) );
			pHitObj->SetJJim( pMover );

			pMover->m_nAtkCnt = 1;		// 카운트 시작.
			pMover->OnAttackStart( pHitObj, dwMsg );			// 공격시작 핸들러.
			m_objidHit	= (OBJID)nParam1;	// 타겟의 아이디.	

			m_dwAtkFlags	= (DWORD)LOWORD( (DWORD)nParam3 );
#ifdef __WORLDSERVER
			if( m_dwAtkFlags == 0 ) 
			{
				if( pMover->GetAttackResult( pHitObj, dwOption ) )		// 공격 성공률을 얻는다.
					m_dwAtkFlags = AF_GENERIC;
				else
					m_dwAtkFlags = AF_MISS;
			}
#else
			m_dwAtkFlags = AF_GENERIC;		// 구조가 좀 이상한데. 클라에서는 명중판정을 하지 않으므로 무조건 평타명중으로 인정.
#endif
			
			return( (int)m_dwAtkFlags );
			break;
		}	
	// 몬스터들의 특수공격.
	case OBJMSG_SP_ATK1:
	case OBJMSG_SP_ATK2:
		{
			CMover* pHitObj		= prj.GetMover( nParam1 );
			//nParam2;		// 특수공격에 사용할 무기. 보통은 몬스터이므로 가상아이템이 들어온다.
			int		nUseMotion	= nParam3;		// 사용할 모션.
			if( IsValidObj( (CObj*)pHitObj ) == FALSE )	return( -1 );
			if( pHitObj->m_pActMover->IsDie() )	return( -1 );
			if( GetState() & OBJSTA_ATK_ALL )	return( 0 );	// 이미 공격중이면 리턴
			if( pMover->m_dwFlag & MVRF_NOATTACK )	return 0;		// 공격금지 상태면 걍 리턴.
			if( GetDmgState() == OBJSTA_DMG )	// 피격 중 공격명령이면 피격 상태를 클리어 시킨다.
				ResetState( OBJSTA_DMG_ALL );
			else
			if( GetState() & OBJSTA_DMG_ALL )	// 그외 피격동작은 다 취소
				return -3;
			if( IsStateFlag( OBJSTAF_COMBAT ) == FALSE )
				SendActMsg( OBJMSG_MODE_COMBAT );
			SendActMsg( OBJMSG_STOP_TURN );	// 공격할땐 턴을 정지시킴
			SendActMsg( OBJMSG_STAND );

			DWORD dwOption = MOP_SWDFORCE /*| MOP_NO_TRANS*/;	// 몬스터니까 공격동작도 모션트랜지션 되게 해봤다.
			SetState( OBJSTA_ATK_ALL, OBJSTA_SP_ATK1 );
			pMover->SetMotion( nUseMotion, ANILOOP_1PLAY, dwOption );		// 최초 메시지 입력시 동작 설정
			pMover->SetAngle( GetDegree( pHitObj->GetPos(), m_pMover->GetPos() ) );
			pMover->OnAttackStart( pHitObj, dwMsg );			// 공격시작 핸들러.
			return 1;
		}
		break;

	// 전사들의 근접 전투스킬류
	case OBJMSG_MELEESKILL:
		{
			if( IsAction() )	return -2;	
			if( pMover->m_dwFlag & MVRF_NOATTACK )	return -2;		// 공격금지 상태면 걍 리턴.
			CMover* pHitObj		= prj.GetMover( nParam2 );
			if( IsValidObj( pHitObj ) == FALSE )	return -2;
#ifdef __WORLDSERVER
			if( GetState() & OBJSTA_ATK_RANGE1 )
				ResetState( OBJSTA_ATK_RANGE1 );
			if( GetState() & OBJSTA_ATK_ALL )	return -2;	// 이미 공격중이면 리턴. 서버에선 씹히지 않게 하자.
			pMover->m_dwMotion = -1;	// 나머지 경우는 간단하게 생각하기 위해 모두 클리어 시키고 스킬만 실행하자.
			ClearState();				// 상태 클리어하고 다시 맞춤,.
#else
			// 이리저리 해보니 액티브 무버건 아니건 서버에서 신호오면 바로 시작하는게 좋을거 같애서 이렇게 함.
			pMover->m_dwMotion = -1;	
			ClearState();				// 상태 클리어하고 다시 맞춤,.
#endif
			SetState( OBJSTA_ATK_ALL, OBJSTA_ATK_MELEESKILL );			// 근접전투스킬상태 설정.
			DWORD dwMotion = (DWORD)nParam1;
			int	nLoop = ANILOOP_1PLAY;

			int	nSkill = pMover->GetActParam( 0 );
			if( nSkill == SI_BLD_DOUBLESW_SILENTSTRIKE || nSkill == SI_ACR_YOYO_COUNTER )
				nLoop = ANILOOP_CONT;

#ifdef __CLIENT			
			if( pMover->HasBuffByIk3( IK3_TEXT_DISGUISE ) )
				dwMotion = MTI_ATK1;
#endif
#ifdef __CROSSBOW
			CItemElem* pItemElem = pMover->GetWeaponItem();
			if( pItemElem && ( pItemElem->GetProp()->dwItemKind3 == IK3_CROSSBOW ))
				dwMotion += 200;
#endif // __CROSSBOW

			pMover->SetMotion( dwMotion, nLoop, MOP_SWDFORCE | MOP_NO_TRANS | MOP_FIXED );		// 해당 동작애니 시작.
			pMover->OnAttackStart( pHitObj, dwMsg );			// 공격시작 핸들러.
			
			m_nMotionHitCount = 0;
			
			break;
		}
	// 마법사들의 마법스킬류.
	case OBJMSG_MAGICSKILL:
	case OBJMSG_MAGICCASTING:
		{
			if( IsAction() )	return -2;
			if( pMover->m_dwFlag & MVRF_NOATTACK )	return -2;		// 공격금지 상태면 걍 리턴.
			CMover* pHitObj		= prj.GetMover( nParam2 );
			if( IsValidObj( pHitObj ) == FALSE )	return -2;
		#ifdef __WORLDSERVER
			if( GetState() & OBJSTA_ATK_ALL )	return -2;	// 이미 공격중이면 리턴
		#endif  

			pMover->m_dwMotion = -1;	
			ClearState();				// 상태 클리어하고 다시 맞춤.

			m_nCastingTime	= nParam3 * 4;	// ProcessAction은 서버/클라 호출 회수 동일하므로, SEC1 사용이 적당하지 않다.

#if __VER >= 10 // __LEGEND	//	9차 전승시스템	Neuz, World, Trans
			m_dwCastingEndTick = (DWORD)( GetTickCount() + nParam3 * 66.66F );
			m_nCastingTick	= (int)( nParam3 * 66.66F );
			m_nCastingSKillID = nParam4;
#endif	//__LEGEND	//	9차 전승시스템	Neuz, World, Trans
			if( m_nCastingTime > 0 )	// 캐스팅 타임이 있을때만...
			{
				m_nCount	= 0;	// 전체 캐스팅 타임은 동작1+동작2의 합이어야 하므로 여기서 카운터를 초기화 한다.
				SetState( OBJSTA_ATK_ALL, OBJSTA_ATK_CASTING1 );			// 캐스팅부터 시작 -> 발사모션.
			}
			else
				SetState( OBJSTA_ATK_ALL, OBJSTA_ATK_MAGICSKILL );			// 발사모션만.

		#ifdef __CLIENT
			if( dwMsg == OBJMSG_MAGICSKILL && pMover->HasBuffByIk3( IK3_TEXT_DISGUISE ) )
				nParam1 = MTI_ATK1;
		#endif

			pMover->SetMotion( nParam1, ANILOOP_1PLAY, MOP_SWDFORCE | MOP_NO_TRANS | MOP_FIXED );		// 해당 동작애니 시작.
			pMover->OnAttackStart( pHitObj, dwMsg );			// 공격시작 핸들러.
			m_nMotionHitCount = 0;
			break;
		}
			

	//-------------------- 맞았다.
	case OBJMSG_DAMAGE:
	case OBJMSG_DAMAGE_FORCE:	// 강공격에 맞았다. 날아감.
		{
			CMover* pAttacker = PreProcessDamage( pMover, (OBJID)nParam2, nParam4, nParam5 );
			if( pAttacker == NULL )
				return 0;

			if( dwMsg == OBJMSG_DAMAGE_FORCE )			// 강공격으로 들어왔을때
				if( pMover->CanFlyByAttack() == FALSE )	// 날아가지 못하는 상태면.
					dwMsg = OBJMSG_DAMAGE;				// 일반 데미지로 바꿈.

			// 실제 가격한 데미지를 리턴함.
			int nDamage = _ProcessMsgDmg( dwMsg, pAttacker, (DWORD)nParam1, nParam3, nParam4, nParam5 );	
#if __VER >= 10	// __METEONYKER_0608
#ifdef __WORLDSERVER
#if __VER >= 12 // __NEW_ITEMCREATEMON_SERVER
			if( pAttacker->IsPlayer() && CCreateMonster::GetInstance()->IsAttackAble( static_cast<CUser*>(pAttacker), pMover ) )
#endif // __NEW_ITEMCREATEMON_SERVER
#endif // __WORLDSERVER
				pMover->PostAIMsg( AIMSG_DAMAGE, pAttacker->GetId(), nDamage );
#endif	// __METEONYKER_0608
			return nDamage;
		}
		break;

	// HP가 0이 된 순간 호출
	case OBJMSG_DIE:
	{
		if( IsSit() )						// 앉아있는상태였으면 해제
			ResetState( OBJSTA_MOVE_ALL );
		
		ClearState();						// 죽을땐 다른 동작 하고 있던거 다 클리어 시켜버리자.
		m_vDelta.x = m_vDelta.z = 0;		// 죽었으면 델타값 클리어.

		pMover->RemoveInvisible();
#ifdef __WORLDSERVER
		pMover->PostAIMsg( AIMSG_DIE, nParam2 );
#endif
		CMover *pAttacker = (CMover *)nParam2;	
		if( pAttacker )
		{
			if( pMover->IsPlayer() && pAttacker->IsPlayer() )	// 죽은놈과 죽인놈이 플레이어고
			{
#ifdef __WORLDSERVER
				if( pMover->m_nDuel == 1 && pMover->m_idDuelOther == pAttacker->GetId() )	// 둘이 듀얼중이었으면
				{
					pMover->ClearDuel();
					pAttacker->ClearDuel();
					((CUser*)pMover)->AddSetDuel( pMover );
					((CUser*)pAttacker)->AddSetDuel( pAttacker );
				}
				else if( pMover->m_nDuel == 2 && pMover->m_idDuelParty == pAttacker->m_idparty )
				{
					pMover->m_nDuelState	= 300;		
					CMover *pMember;
					CParty* pParty	= g_PartyMng.GetParty( pMover->m_idparty );

					if( pParty )
					{
						for( int i = 0; i < pParty->m_nSizeofMember; i ++ )
						{
							pMember = (CMover *)g_UserMng.GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
							if( IsValidObj( pMember ) )
								((CUser*)pMember)->AddSetDuel( pMover );
						}
					}

					CParty* pParty2		= g_PartyMng.GetParty( pMover->m_idDuelParty );
					if( pParty2 )
					{
						for( int i = 0; i < pParty2->m_nSizeofMember; i ++ )
						{
							pMember = (CMover *)g_UserMng.GetUserByPlayerID( pParty2->m_aMember[i].m_uPlayerId );
							if( IsValidObj( pMember ) )
								((CUser*)pMember)->AddSetDuel( pMover );
						}
					}
				}
#endif	// __WORLDSERVER
			}

			if( pAttacker->IsNPC() )	// 공격자가 몹.
			{	// 날때린몹의 어태커가 나일경우는 죽기전에 클리어시켜준다.
				if( pAttacker->m_idAttacker == pMover->GetId() )	// 공격한몹을 친사람이 나일경우는 스틸체크 해제.
				{
					pAttacker->m_idAttacker = NULL_ID;
					pAttacker->m_idTargeter = NULL_ID;
				}
			}
		}
		
		switch( nParam1 )
		{
#ifdef __BS_DEATH_ACTION
		case OBJMSG_DAMAGE:
		case OBJMSG_DAMAGE_FORCE:
		{
			int rAngle = 0;
			int rPower = 0;
			int nCase = rand() % 6;
			if( nCase < 3 )
			{
				rAngle = -2;
				rPower = 10 + rand() % 50;
			}
			else if( nCase < 4 )
			{
				rAngle = 10;
				rPower = 10 + rand() % 50;
			}
			else if( nCase < 5 )
			{
				rAngle = 50;
				rPower = 10 + rand() % 50;
			}
			else
			{
				rAngle = rand() % 30;
				rPower = rand() % 200;
			}
			
			if( OBJMSG_DAMAGE_FORCE == nParam1 )
				rPower += 50;
		
			CMover *pAttacker = (CMover *)nParam2;
			SetState( OBJSTA_DMG_ALL, OBJSTA_DMG_FLY );
			pMover->SetMotion( MTI_DMGFLY, ANILOOP_CONT );
			if( pAttacker )
			{
				float faddPower = rPower * 0.01f;
				float faddAngle = (float)( rAngle );

				DoDamageFly( pAttacker->GetAngle(), 100.0f + faddAngle, 0.05f + faddPower );	// 어태커가 보는쪽으로 날려보냄.
			}
		}
		break;
#else
		case OBJMSG_DAMAGE_FORCE:	// 강 공격으로 죽었다면 날아가서 죽기
			{
			CMover *pAttacker = (CMover *)nParam2;
			SetState( OBJSTA_DMG_ALL, OBJSTA_DMG_FLY );
			pMover->SetMotion( MTI_DMGFLY, ANILOOP_CONT );
			if( pAttacker )
				DoDamageFly( pAttacker->GetAngle(), 145.0f, 0.25f );	// 어태커가 보는쪽으로 날려보냄.
			}
			break;
		case OBJMSG_DAMAGE:		// 걍 일반 데미지로 죽었다면 그냥 쓰러져 죽기
#endif
		default:	
			SetState( OBJSTA_DMG_ALL, OBJSTA_DEAD );
			pMover->SetMotion( MTI_DIE1, ANILOOP_CONT );
			break;

		}

		if( !pMover->IsPlayer() )	// 몬스터일 경우.
		{
		#ifdef __WORLDSERVER			
			m_nDeadCnt	= 60*3;	// 디폴트 - 3초 후에 없어짐.
			MoverProp *pProp = pMover->GetProp();
			if( pProp )
			{
				/*
				if( pProp->dwSourceMaterial != NULL_ID || pProp->dwClass == RANK_MATERIAL )	// 자원 몹이면..
				{
					if( pProp->dwHoldingTime == NULL_ID )
						Error( "%s 의 MoverProp::dwHoldingTime 값이 -1", pMover->GetName() );
					m_nDeadCnt = (int)(60.0f * (pProp->dwHoldingTime / 1000.0f));
					if( pProp->dwHoldingTime < 10000 )		// 10초 이하일때
						Error( "dwHoldingTime : %d %s", pProp->dwHoldingTime, pMover->GetName() );
					pMover->m_nCollectOwnCnt = PROCESS_COUNT * 40;
					CMover *pAttacker = (CMover *)nParam2;	
					if( pAttacker )
						pMover->m_idCollecter = pAttacker->GetId();		// 공격자가 소유자가 된다.
					
				}
				{
					if( pProp->dwSourceMaterial == NULL_ID && pProp->dwClass == RANK_MATERIAL )	// 자원몹인데 자원값이 없는경우
						Error( "OBJMSG_DIE : %s %d", pMover->GetName(), pProp->dwSourceMaterial );
					if( pProp->dwSourceMaterial != NULL_ID && pProp->dwClass != RANK_MATERIAL )	// 자원몹이 아닌데 자원값이 있는경우.
						Error( "OBJMSG_DIE 2 : %s %d", pMover->GetName(), pProp->dwSourceMaterial );
				}
				*/
			}
		#endif // WorldServer
		}
		break;
	}
		
	case OBJMSG_APPEAR:
		SetState( OBJSTA_ACTION_ALL, OBJSTA_APPEAR );
		pMover->SetMotion( MTI_APPEAR2, ANILOOP_1PLAY, MOP_FIXED | MOP_NO_TRANS );
		break;
	//---- 모드전환 ------------------------
	case OBJMSG_MODE_COMBAT:
		if( GetStateFlag() & OBJSTAF_COMBAT )	return 0;		// 이미 전투모드면 리턴
		if( IsActJump() )	return 0;
		if( IsActAttack() )	return 0;
		if( IsActDamage() )	return 0;
		if( IsSit() )		return 0;
		if( IsAction() )	return 0;
		AddStateFlag( OBJSTAF_COMBAT );	// 전투모드로 서라가 아니고 전투모드로 바꿔라기땜에 서다 명령은 발생시키지 않는다.
		pMover->m_dwMotion = -1;	// 같은 모션을 하라고 한거라 모션이 안바뀌므로 이렇게...
		pMover->SetMotion( MTI_STAND );
		break;
	case OBJMSG_MODE_PEACE:
		if( (GetStateFlag() & OBJSTAF_COMBAT) == 0 )	return 0;
		if( IsActJump() )	return 0;
		if( IsActAttack() )	return 0;
		if( IsActDamage() )	return 0;
		if( IsSit() )		return 0;
		if( IsAction() )	return 0;
		RemoveStateFlag( OBJSTAF_COMBAT );
		pMover->m_dwMotion = -1;
		pMover->SetMotion( MTI_STAND );
		break;
	case OBJMSG_MODE_WALK:
		if( IsSit() )		return 0;
		if( IsAction() )	return 0;
		if( GetStateFlag() & OBJSTAF_WALK )	return 0;		// 이미 걷기모드면 리턴
		AddStateFlag( OBJSTAF_WALK );
		break;
	case OBJMSG_MODE_RUN:
		if( IsSit() )		return 0;
		if( IsAction() )	return 0;
		if( (GetStateFlag() & OBJSTAF_WALK) == 0 )	return 0;		// 
		RemoveStateFlag( OBJSTAF_WALK );
		break;
	case OBJMSG_MODE_FLY:
	{
		if( nParam3 == 0 )
		{
			if( IsSit() )		return 0;
			if( IsAction() )	return 0;
			if( IsActAttack() )	return 0;
			if( pMover->m_dwFlag & MVRF_NOMOVE )	return 0;		// 이동금지 상태면 걍 리턴.
#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
			if( pMover->GetAdjParam( DST_CHRSTATE ) & CHS_LOOT)			return 0;
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans

			{
				CWorld* pWorld	= pMover->GetWorld();
				if( pWorld )
				{
					int nAttr = pMover->GetWorld()->GetHeightAttribute( pMover->GetPos().x, pMover->GetPos().z );		// 이동할 위치의 속성 읽음.
					if( nAttr == HATTR_NOFLY )		// 비행 금지 지역에서 타려고 하면 못탄다.
						return 0;
				}
			}
		}
		AddStateFlag( OBJSTAF_FLY );
		ItemProp *pItemProp = prj.GetItemProp( nParam1 );	// 빗자루 프로퍼티.
		if( pItemProp )
		{
			if( pMover->m_nFuel == -1 )		// 초기값이면
				pMover->m_nFuel = (int)pItemProp->dwFFuelReMax;	// 빗자루 최대 연료량을 세팅.
			// -1일때만 세팅해야지 연료를 반쯤 쓰다가 빗자루를 바꿔도 새로 세팅 되지 않는다.
		}
#ifndef __JEFF_9_20
#ifdef __WORLDSERVER
		if( pMover->IsPlayer() )
		{
			((CUser*)pMover)->SetFlyTimeOn();
		}
#endif // __WORLDSERVER
#endif	// __JEFF_9_20
		ClearState();
#ifdef __CLIENT
		if( m_pMover == CMover::GetActiveMover() )
		{
			m_pMover->GetWorld()->SetObjFocus( NULL );	// 비행직전에 기존타겟팅을 클리어해준다.
			g_Neuz.m_camera.Unlock();
			g_WndMng.m_pWndTaskBar->OnCancelSkill();	// 비행직전엔 스킬바 사용 취소.
		}
#endif

		CModel* pModel = prj.m_modelMng.LoadModel( D3DDEVICE, OT_ITEM, (DWORD) nParam1 );
#if __VER >= 14 // __WING_ITEM
		CModelObject* pModelObject = (CModelObject*)pModel;
		if( pModelObject->m_pBone )
		{
			CString strMotion = pModelObject->GetMotionFileName( _T("stand") );
			assert( strMotion != _T("") );
			pModelObject->LoadMotion( strMotion );
		}
#endif // __WING_ITEM
		m_pMover->SetRide( pModel, (DWORD) nParam1 );
		break;
	}
	case OBJMSG_MOTION:		// 단순 모션 플레이
		{
			if( IsSit() )		return 0;
			if( IsAction() )	return 0;
			if( IsActJump() )	return 0;
			if( IsActAttack() )	return 0;
			if( IsActDamage() )	return 0;
			if( pMover->m_dwFlag & MVRF_NOMOVE )	return 0;		// 이동금지 상태면 걍 리턴.
#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
		if( pMover->GetAdjParam( DST_CHRSTATE ) & CHS_LOOT)			return 0;
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans

			AddStateFlag( OBJSTAF_ETC );		// 단순 모션 플레이 상태
			m_nMotionEx = nParam1;				// 모션 인덱스
			int nPlayMode = nParam2;			// 루핑 모드.
			if( m_nMotionEx != -1 )
				pMover->SetMotion( m_nMotionEx, nPlayMode, MOP_FIXED/* | MOP_SWDFORCE*/ );

			pMover->RemoveInvisible();
		}
		break;
	// 일반 액션.
	case OBJMSG_RESURRECTION:		// 부활.
		ClearState();
		SetState( OBJSTA_DMG_ALL, OBJSTA_RESURRECTION );
		pMover->SetMotion( MTI_ASS_RESURRECTION, ANILOOP_1PLAY, MOP_FIXED );
		break;


#if	defined(_DEBUG) && defined(__CLIENT)
	case OBJMSG_TEMP:
		{
			for( int i=0; i < 1; i ++ )
			{

				CMover* pObj = new CMover;
				D3DXVECTOR3 vPos = pMover->GetPos();
				pObj->SetPos( vPos );
				pObj->SetIndex( g_Neuz.m_pd3dDevice, MI_AIBATT1, TRUE );
				pObj->SetMotion( 0 );
				g_WorldMng.Get()->AddObj( pObj, TRUE ); 
			}
		}
		break;
		
	case OBJMSG_TEMP2:	// 디버깅용 공격동작만 볼때.
		if( CMover::GetActiveMover()->IsAuthHigher( AUTH_GAMEMASTER ) )	// 영자 계정일때
		{
			CObj *pObj = pMover->GetWorld()->GetObjFocus();
			g_DPlay.SendCorrReq( pObj );
			g_DPlay.SendError( FE_GENERAL, 0 );
		}
		break;
	
	case OBJMSG_TEMP3:
		{
		#ifdef __XUZHU			
			g_DPlay.SendError( 0, 0 );
		#endif
		}
		break;
#endif // defined(_DEBUG) && defined(__CLIENT)
		
	}
	return 1;
}


/*

  p0 + tD0 = -e0

  t = -( p0 + e0 ) / d0

void	Test( RECT r, Point p, Point.d )
{
	Point t;	// 교차점

	tx = (p0 + 
	
}

*/

// return: attacker의 포인터 
CMover* CActionMover::PreProcessDamage( CMover* pMover, OBJID idAttacker, BOOL bTarget, int nReflect )
{
	if( pMover->m_dwMode & MATCHLESS_MODE )		// 무적이면 여기서 리턴.	
		return NULL;
#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
	if( pMover->GetAdjParam( DST_CHRSTATE ) & CHS_SETSTONE)
		return NULL;
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans

	CCtrl* pCtrl = prj.GetCtrl( idAttacker );	// 공격자 
	if( IsValidObj( pCtrl ) == FALSE )
		return NULL;

	CMover* pAttacker = NULL;
	if( pCtrl->GetType() == OT_MOVER )
		pAttacker = (CMover*)pCtrl; 
	else
		return NULL;							

	if( pMover->IsNPC() )						// 맞는넘이 NPC고
	{
		MoverProp *pProp = pMover->GetProp();
		if( pProp && pProp->bKillable != 1 )	// 죽이기가 불가능 한넘이면 여기서 리턴.
			return NULL;
	}

#ifdef __WORLDSERVER
	HITTYPE ht = pAttacker->GetHitType( pMover, bTarget, nReflect );
	if( ht == HITTYPE_FAIL )
		return 0;
#if __VER < 10	// __METEONYKER_0608
	pMover->PostAIMsg( AIMSG_DAMAGE, idAttacker ); 
#endif	// __METEONYKER_0608

#endif

	pAttacker->RemoveInvisible();
	return pAttacker;
}