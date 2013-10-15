#include "stdafx.h"
#include "defineSkill.h"
#include "defineText.h"
#ifdef __CLIENT
#include "AppDefine.h"
#endif

#include "ModelObject.h"

#ifdef __CLIENT
#include "DPClient.h"
extern	CDPClient	g_DPlay;
#else	// __CLIENT
#ifdef __WORLDSERVER
#include "User.h"
#include "WorldMng.h"
#include "DPSrvr.h"
#include "DPCoreClient.h"
extern	CUserMng	g_UserMng;
extern	CWorldMng	g_WorldMng;
extern	CDPSrvr		g_DPSrvr;
extern	CDPCoreClient	g_DPCoreClient;
#endif	// __WORLDSERVER
#endif	// __CLIENT

#include "ship.h"


/*--------------------------------------------------------------------------------------------------------


											Mover act command


----------------------------------------------------------------------------------------------------------*/

void	CMover::SetCmd( OBJACT cmd, int nParam1, int nParam2, int nParam3 )
{
	m_oaCmd = cmd;
	m_nCParam[0] = nParam1;		// 명령 파라메터.
	m_nCParam[1] = nParam2;
	m_nCParam[2] = nParam3;
}

//
// fRange = 도달범위.  디폴트는 0.0f은 상대방의 반경까지 붙는거다.
void CMover::SetDestObj( OBJID objid, float fRange, BOOL bSend )
{
#ifdef __WORLDSERVER
	if( FALSE == IsPlayer() && m_idDest != objid )
		m_nCorr	= -1;
#else	// __WORLDSERVER
	if( bSend && IsActiveMover() && m_idDest != objid )
	{
#ifdef __BS_ADJUST_SYNC
		CMover* pTarget = prj.GetMover( m_idDest );
		if( pTarget )
		{
			if( pTarget->IsPlayer( ) )
				g_DPlay.SendPlayerBehavior();	// 새로운 Obj로 이동할경우 동기화 한번 맞춰주고....
		}
#endif
		g_DPlay.SendPlayerDestObj( objid, fRange );
		m_nCorr	= -1;
	}
#endif	// not __WORLDSERVER

	m_idDest	    = objid;
	m_fArrivalRange = fRange;
	ClearDestPos();

#ifdef __WORLDSERVER
	// 서버에서 SetDestObj를 실행하면 클라들에게 보내주는게 없어서 이걸 넣음.
	if( bSend )
		g_UserMng.AddMoverSetDestObj( this, objid, fRange, FALSE );
#endif	// __WORLDSERVER

#ifdef __CLIENT
	m_oaCmd = OBJACT_NONE;		// CMD_ 시리즈를 거치지 않고 이것만 호출되는 경우는 Cmd를 해제시키자.
#endif // Client

//	if( IsPlayer() )
//		TRACE( "SetDestObj %08x\n", objid );
}

//
//
void CMover::SetDestPos( const D3DXVECTOR3 & vDestPos, bool bForward, BOOL fTransfer )
{
	m_vDestPos	= vDestPos;
	
#ifdef __BS_ADJUST_SYNC
	m_bForward	= true;				//sun
#else
	m_bForward	= bForward;
#endif

	D3DXVECTOR3	vDir = GetPos() - m_vDestPos;

	m_bPositiveX = ( vDir.x > 0.0f );
	m_bPositiveZ = ( vDir.z > 0.0f );
	
	ClearDestObj();

#ifdef __CLIENT
	if( IsActiveMover() && fTransfer ) 
	{
		g_DPlay.PutPlayerDestPos( vDestPos, bForward );
	}
#endif
}

// IA오브젝트 위로 이동클릭을 했을때....
#ifdef __IAOBJ0622
void CMover::SetDestPos( CShip *pIAObj, const D3DXVECTOR3 &vDestPos )
{
	m_pIADestPos = pIAObj;
	
	D3DXMATRIX mTrans, mInvIA, mLocal;
	D3DXMatrixTranslation( &mTrans, vDestPos.x, vDestPos.y, vDestPos.z );		// 월드좌표를 매트릭스로 변환
	D3DXMatrixInverse( &mInvIA, NULL, pIAObj->GetMatrixWorldPtr() );			// IAObj의 역행렬.
	D3DXMatrixMultiply( &mLocal, &mTrans, GetIAObjLink()->GetInvTM() );	// IA오브젝트로부터의 로컬 행렬.

	D3DXVECTOR3 vLocal;
	vLocal.x = mLocal._41;
	vLocal.y = mLocal._42;
	vLocal.z = mLocal._43;
	
	SetDestPos( vLocal, true, FALSE );	// do not transfer

#ifdef __CLIENT
	if( IsActiveMover() )
	{
		g_DPlay.PutPlayerDestPos( vLocal, true, 0, pIAObj->GetId() );
	}
#endif	// __CLIENT
}
#endif	// __IAOBJ0622

FLOAT	CMover::GetAttackRange( DWORD dwAttackRange )
{
	float fAttRange = 0.0f;
	switch( dwAttackRange )
	{
	case AR_SHORT:	fAttRange = 2.0f;	break;
	case AR_LONG:	fAttRange = 3.0f;		break;
	case AR_FAR:	fAttRange = 4.0f;		break;
	case AR_RANGE:	fAttRange = 10.0f;		break;
	case AR_WAND:	fAttRange = 15.0f;		break;
	case AR_HRANGE: fAttRange = 6.0f;	break;
#if __VER >= 10 // __LEGEND		//10차 전승 마스터스킬
	case AR_HWAND:	fAttRange = 18.0f;		break;
#endif //__LEGEND		//10차 전승 마스터스킬
	default:		fAttRange = 0.0f;		break;
	}
#if __VER >= 10 // __LEGEND		//10차 전승 마스터스킬
	int  nTmpATR = GetParam( DST_HAWKEYE_RATE, 0 );
	if( nTmpATR > 0 )
	{
		nTmpATR += 100;
		fAttRange =  fAttRange * nTmpATR / 100;
	}
#endif //__LEGEND		//10차 전승 마스터스킬

	return fAttRange;
}

//
// 외부에서 UseSkill을 명령할땐 이것으로 호출하자.
// sutType : 스킬을 사용할때 스킬큐에서 연타로 사용한건가 일반적인 사용을 한건가.c
//
int	CMover::CMD_SetUseSkill( OBJID idTarget, int nSkillIdx, SKILLUSETYPE sutType )
{
	m_oaCmd = OBJACT_NONE;
	TRACE( "CMD_SetUseSkill( " );
	if( m_pActMover->IsFly() )	return 0;				// 비행중엔 스킬사용 금지.
	if( m_pActMover->IsActAttack() )	return 0;
	if( m_pActMover->IsActJump() )	return 0;			// 점프중엔 사용금지.
	if( m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL )		return 0;	// 데미지 플라이중엔 스킬사용금지.
	if( IsDie() )	return 0;		// 죽었을때 사용금지.

	LPSKILL pSkill	= GetSkill( 0, nSkillIdx );		// this가 가진 스킬중 nIdx에 해당하는 스킬을 꺼낸다.
	if( pSkill == NULL )
	{
		Error( "CMD_SetUseSkill : %s skill(%d) not found", m_szName, nSkillIdx );
		return 0;	// 
	}

	ItemProp* pSkillProp = pSkill->GetProp();
	if( pSkillProp == NULL )		// JobSkill 리스트에서 꺼낸 스킬의 프로퍼티를 꺼냄.
	{
		Error( "CMD_SetUseSkill : %s. skill(%d) property not found", m_szName, pSkill->dwSkill );
		return 0;	// 
	}
	
	if( IsPlayer() && IsStateMode( STATE_BASEMOTION_MODE ) )	// 시전중(준비시간)일땐 사용금지.
	{
	#ifdef __CLIENT
		g_DPlay.SendStateModeCancel( STATE_BASEMOTION_MODE, STATEMODE_BASEMOTION_CANCEL );
	#endif 
		return 0;		
	}

	// 도달범위 - 얼마나 가까이 근접해야하는가. 미터단위
	float	fArrivalRange = 0.0f;		
	fArrivalRange = GetAttackRange( pSkillProp->dwAttackRange );
	
	switch( pSkillProp->dwUseChance )
	{
	case WUI_NOW:	// 타겟팅과 상관없이 자기자신에게 쓰는 방식.
		idTarget = GetId();
		break;
	case WUI_TARGETINGOBJ:	// 셀렉트 되어 있는 타겟에게 사용.
		{
#ifdef __CLIENT
		CObj *pFocusObj = GetWorld()->GetObjFocus();
		if( pFocusObj && pFocusObj->GetType() == OT_MOVER )
			idTarget = ((CMover*)pFocusObj)->GetId();
#else
		if( IsPlayer() )
			idTarget = ((CUser *)this)->m_idSetTarget;
#endif // __CLIENT
		}

		break;
#ifdef __CLIENT
	case WUI_TARGETCURSORPTZ:
		{
			idTarget = GetId();
			
			CRect rect;
			D3DXVECTOR3 vPos;
			CWndWorld* pWndWorld;
			
			pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );

			rect = pWndWorld->GetClientRect();

			if( GetWorld()->ClientPointToVector( NULL, rect, pWndWorld->GetMousePoint(), &GetWorld()->m_matProj, &GetWorld()->GetCamera()->m_matView, &vPos, TRUE ) )
			{
#ifdef __SKILL0517
				AddSkillProp* pAddSkillProp = prj.GetAddSkillProp( pSkillProp->dwSubDefine, GetSkillLevel( pSkill ) );		// UseSkill에서 사용한 스킬의 프로퍼티 꺼냄
#else	// __SKILL0517
				AddSkillProp* pAddSkillProp = prj.GetAddSkillProp( pSkillProp->dwSubDefine, pSkill->dwLevel );		// UseSkill에서 사용한 스킬의 프로퍼티 꺼냄
#endif	// __SKILL0517
				if( pAddSkillProp == NULL )
				{
					Error( "CMover::OnMagicSkill : %s. add스킬(%d)의 프로퍼티가 없다.", m_szName, nSkillIdx );
					return 0;	// property not found
				}
				
				FLOAT	fDist;
				FLOAT	fMaxDistSq;
				D3DXVECTOR3	vDist;
				fMaxDistSq  = (float)pAddSkillProp->dwSkillRange;
				fMaxDistSq *= fMaxDistSq;
				vDist = vPos - GetPos();
				fDist = D3DXVec3LengthSq( &vDist );	

				SetAngle( GetDegree(vPos, GetPos()) );		// 목표쪽으로 몸을 돌림.
				
				// 텔레포트 할 위치가 멀경우 현제 스킬에 해당하는 거리로 바꿔준다
				if( fDist > fMaxDistSq )
				{
					FLOAT fLength;
					D3DXVECTOR3 vDirNor;
					D3DXVec3Normalize( &vDirNor, &vDist );
					fLength = (float)pAddSkillProp->dwSkillRange;
					float y = vPos.y;
					vPos = GetPos() + (vDirNor * fLength);
					vPos.y	= y;

					// 스킬에 해당하는 거리로 바꾼곳이 못가는 지역이라면 갈수 있는 지역을 검사한다.
					int nAttr = GetWorld()->GetHeightAttribute( vPos.x, vPos.z );

					if( nAttr != HATTR_NONE ) 
					{
						while( nAttr != HATTR_NONE )
						{
							if( nAttr == HATTR_NOFLY )
								break;
							
							fLength -= 1.0f; // 1미터씩 줄여가며 계산한다.
							vPos  = GetPos() + (vDirNor * fLength);
							nAttr = GetWorld()->GetHeightAttribute( vPos.x, vPos.z );

							// 캐릭터의 앞 뒤로 이동불가 일 경우 뒷쪽이 이동불가 해제 될때 까지 계속 계산하여 이동시킴
							// 그러므로 텔레포트 스킬 범위를 넘어설 경우 원래 자리로 텔레포트 하도록 처리
							D3DXVECTOR3 vTemp = vPos - GetPos();
							float fTemp = D3DXVec3LengthSq( &vTemp );
							if(fTemp > fMaxDistSq)
							{
								vPos = GetPos();
								break;
							}
						}
						
						// 한번더 줄여줌
						fLength -= 1.0f; 
						vPos = GetPos() + (vDirNor * fLength);

						// 줄인 곳이 이동불가 지역일 수 있다.
						nAttr = GetWorld()->GetHeightAttribute( vPos.x, vPos.z );
						if( nAttr != HATTR_NONE )
						{
							vPos = GetPos();
						}
					}
				}
				else // 텔레포트 할 위치가 해당스킬 거리보다 작을경우
				{
					int nAttr = GetWorld()->GetHeightAttribute( vPos.x, vPos.z );

					FLOAT fLength;
					D3DXVECTOR3 vDirNor;
					D3DXVec3Normalize( &vDirNor, &vDist );
					fLength = 0.0f;

					while( nAttr != HATTR_NONE )
					{
						if( nAttr == HATTR_NOFLY )
							break;

						fLength -= 1.0f;
						vPos = GetPos() + (vDirNor * fLength);
						
						nAttr = GetWorld()->GetHeightAttribute( vPos.x, vPos.z );

						// 캐릭터의 앞 뒤로 이동불가 일 경우 뒷쪽이 이동불가 해제 될때 까지 계속 계산하여 이동시킴
						// 그러므로 텔레포트 스킬 범위를 넘어설 경우 원래 자리로 텔레포트 하도록 처리
						D3DXVECTOR3 vTemp = vPos - GetPos();
						float fTemp = D3DXVec3LengthSq( &vTemp );
						if(fTemp > fMaxDistSq)
						{
							vPos = GetPos();
							break;
						}
					}
				}

				if( IsActiveMover() && g_eLocal.GetState( EVE_SCHOOL ) )	// 학교이벤섭이면.
				{
					D3DXVECTOR3 v1, v2;
					v1 = GetPos();	v1.y += 0.1f;
					v2 = vPos;		v2.y += 0.1f;
					if( GetWorld()->IntersectObjLine( NULL, v1, v2, FALSE, FALSE ) )	// 텔레포트는 라인체크함.
					{
						g_WndMng.PutString( prj.GetText(TID_GAME_NOMOVING), NULL, prj.GetTextColor(TID_GAME_NOMOVING) );
						g_WndMng.m_pWndWorld->SetNextSkill( NEXTSKILL_NONE );
						return 0;
					}
				}

				pWndWorld->m_vTelePos = vPos;
			}
			else
			{
				g_WndMng.m_pWndWorld->SetNextSkill( NEXTSKILL_NONE );
				g_WndMng.PutString( prj.GetText(TID_GAME_NOMOVING), NULL, prj.GetTextColor(TID_GAME_NOMOVING) );
				return 0;
			}
		}
		break;
#endif // __CLIENT
	}

	// 타인에게 쓰는경우에만 검사...
	if( idTarget != GetId() )		
	{
		CMover *pTarget = prj.GetMover( idTarget );
		if( IsValidObj(pTarget) )
		{
			if( pSkillProp->nEvildoing < 0 )	// 나쁜 스킬은 
				if( IsAttackAble(pTarget) == FALSE )	// 공격허용이 되지 않으면 사용할 수 없음.
					return 0;
				
			if( pSkill->dwSkill == SI_ASS_HEAL_RESURRECTION )	// 부활을 사용했을때
			{
				if( pTarget->IsNPC() || pTarget->IsDie() == FALSE )		// 상대가 NPC거나 상대가 죽어있지 않다면 취소
					return 0;
			} else
			{
				if( pTarget->IsDie() )		// 부활이 아닌 스킬을 사용했을때 상대가 죽어있으면 사용안됨.
					return 0;
			}
		}
	}

	// 타겟 근접 방식.
	switch( pSkillProp->dwExeTarget )
	{
	case EXT_SELFCHGPARAMET:	// 시전자 자신에게 사용하는 종류
		idTarget = GetId();		// 타겟을 자기자신으로 설정.
		SetDestObj( idTarget, fArrivalRange, TRUE );		// 이동할 목표물을 idTarget으로 설정. << 이게 왜 필요하지? ㅡ.ㅡ?
		break;
	case EXT_MAGICATKSHOT:
	case EXT_MAGICATK:			// 원거리에서 마법으로 타겟을 공격
	case EXT_MAGICSHOT:
		if( idTarget == NULL_ID )	return 0;			// 타겟이 없거나 유효하지 않으면 실패.
		SetDestObj( idTarget, fArrivalRange, TRUE );		// 이동할 목표물을 idTarget으로 설정. 다가가는 범위는 fArrivalRange값으로..
		break;
	case EXT_ANOTHERWITH:		// 나 혹은 다른사람에게 시전
		if( idTarget == NULL_ID )	// 타겟이 잡혀있지 않으면
			idTarget = GetId();		// 자신을 타겟으로 잡음
		SetDestObj( idTarget, fArrivalRange, TRUE );		// 이동할 목표물을 idTarget으로 설정.
		break;
	case EXT_AROUNDATK:			// 내 주위적들을 대상.
		idTarget = GetId();		// 타겟을 자기자신으로 설정.
		SetDestObj( idTarget, fArrivalRange, TRUE );		// 이동할 목표물을 idTarget으로 설정.
		break;
	case EXT_OBJCHGPARAMET:		// 타인에게 사용
	default:	// 그외는 모두 근접하자.
		if( idTarget == NULL_ID )	return 0;			// 타겟이 없거나 유효하지 않으면 실패.
		SetDestObj( idTarget, fArrivalRange, TRUE );		// 이동할 목표물을 idTarget으로 설정.
		break;
	}
	
	ClearActParam();
	SetCmd( OBJACT_USESKILL, nSkillIdx, idTarget, sutType );		// 사정거리가 되었을때 실행할 명령 셋팅.
	TRACE( "\n)CMD_SetUseSkill\n" );

	return 1;
}

class CWndWorld;

void	CMover::CMD_SetMeleeAttack( OBJID idTarget, FLOAT fRange )
{
	ItemProp *pProp = GetActiveHandItemProp();
	if( pProp->dwItemKind2 == IK2_TOOLS )			// 툴을 들고선 공격못한다,.
		return;

	if( m_pActMover->IsActAttack() )	
		return;
	if( IsDie() )	
		return;										// 죽었을때 사용금지.

	// 요요는 밀리어택이긴 하나 약간 거리를 두고 공격한다.( 프로퍼티 참조함 )
	if( pProp->dwItemKind3 == IK3_YOYO )
		fRange = GetAttackRange( pProp->dwAttackRange );
	
#ifdef __CLIENT
	ClearActParam();
	SetDestObj( idTarget, fRange );		// 이동할 목표물을 idTarget으로 설정.
	SetCmd( OBJACT_MELEE_ATTACK, idTarget );		// 사정거리가 되었을때 실행할 명령 셋팅.
#elif defined(__WORLDSERVER)
#ifdef __TRAFIC_1222
	if( GetDestId() == idTarget )
		return;
#endif	// __TRAFIC_1222
	SetDestObj( idTarget, fRange );		// 이동할 목표물을 idTarget으로 설정.
	g_UserMng.AddMoverSetDestObj( this, idTarget, fRange );
#endif // WORLDSERVER
}

/*
void	CMover::CMD_SetCollect( CObj *pObj )
{
	if( IsInvalidObj( pObj ) )	return;
	if( pObj->GetType() != OT_MOVER )	return;
	if( m_pActMover->IsActAttack() )	return;
	if( m_pActMover->IsActJump() )		return;
	if( m_pActMover->IsActDamage() )		return;
	if( IsDie() )	return;		// 죽었을때 사용금지.
	
	CMover *pTarget = (CMover *)pObj;
	ClearActParam();
	SetDestObj( pTarget->GetId(), 3.0f );		// 이동할 목표물을 idTarget으로 설정.
	SetCmd( OBJACT_COLLECT, pTarget->GetId() );		// 사정거리가 되었을때 실행할 명령 셋팅.
}
*/
//
//
//
void	CMover::CMD_SetRangeAttack( OBJID idTarget, int nPower )
{
	TRACE("CMD_SetRangeAttack(%d, %d)\n", idTarget, nPower );

	if( m_pActMover->IsActAttack() )		// 이미 어택중이면 취소.
		return;

	if( IsDie() )	
		return;								// 죽었을때 사용금지.
	
	ItemProp *pItemProp = GetActiveHandItemProp();
	if( pItemProp == NULL )		
		return;
#ifdef __CROSSBOW	
	if( pItemProp->dwItemKind3 != IK3_BOW &&  pItemProp->dwItemKind3 != IK3_CROSSBOW )	// 들고있는 무기가 보우가 아니면 에러
		return;
#else // __CROSSBOW
	if( pItemProp->dwItemKind3 != IK3_BOW )
		return;
#endif // __CROSSBOW

	if( IsBullet( pItemProp ) == FALSE )
		return;
	
	ClearActParam();
	SetDestObj( idTarget, GetAttackRange( pItemProp->dwAttackRange ), TRUE );		// 이동할 목표물(idTarget)과 그 거리를 설정 
	SetCmd( OBJACT_RANGE_ATTACK, idTarget, nPower, 0 );		// 사정거리가 되었을때 실행할 명령 셋팅.
}

void	CMover::CMD_SetMagicAttack( OBJID idTarget, int nMagicPower )
{
	TRACE("CMD_SetMagicAttack(%d, %d)\n", idTarget, nMagicPower );

	if( m_pActMover->IsActAttack() )		// 이미 어택중이면 취소.
		return;
	if( IsDie() )	return;		// 죽었을때 사용금지.
	
	// 손에 들고 있는 완드의 프로퍼티 꺼냄
	// 사정거리 계산함
	ItemProp *pItemProp = GetActiveHandItemProp();
	if( pItemProp == NULL )		return;

	if( pItemProp->dwItemKind3 != IK3_WAND /*&& pItemProp->dwItemKind3 != IK3_STAFF*/ )	//Staff->ata2k	// 들고있는 무기가 완드가 아니면 취소
		return;
	
	// 도달범위 - 얼마나 가까이 근접해야하는가. 미터단위
	float	fArrivalRange = GetAttackRange( AR_WAND );
	
	ClearActParam();
	SetDestObj( idTarget, fArrivalRange );		// 이동할 목표물을 idTarget으로 설정.
	SetCmd( OBJACT_MAGIC_ATTACK, idTarget, nMagicPower );		// 사정거리가 되었을때 실행할 명령 셋팅.
}

// vPos위치로 이동명령
void	CMover::CMD_SetMoveToPos( const D3DXVECTOR3 &vPos )
{
	SetDestPos( vPos );
}

// idObj위치로 이동
void	CMover::CMD_SetMoveToObj( OBJID idObj )
{
	SetDestObj( idObj );
}

void	CMover::CMD_SetUseItem( CCtrl *pCtrl )
{
	ClearActParam();
	if( m_pActMover->IsFly() )
		SetDestObj( pCtrl, 20.0f );
	else
		SetDestObj( pCtrl );
	SetCmd( OBJACT_USEITEM );		// 사정거리가 되었을때 실행할 명령 셋팅.
}



/*--------------------------------------------------------------------------------------------------------


											Mover act parameter
											

----------------------------------------------------------------------------------------------------------*/

void	CMover::SetActParam( OBJACT act, int nParam1, int nParam2, int nParam3 )
{
	m_oaAct = act;				
	m_nAParam[0] = nParam1;		// 명령 파라메터.
	m_nAParam[1] = nParam2;
	m_nAParam[2] = nParam3;
}


