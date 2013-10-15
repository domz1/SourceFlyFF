#include "stdafx.h"
#include "defineObj.h"
#include "defineskill.h"
#include "ActionMover.h"
#include "..\_Common\ParticleMng.h"

#ifdef __CLIENT
#include "Sfx.h"
#include "DPClient.h"
extern	CDPClient	g_DPlay;
#endif // CLIENT

#ifdef __WORLDSERVER
#include "User.h"
#include "..\_AIInterface\AIInterface.h"
extern	CUserMng			g_UserMng;
#endif

#if __VER >= 13 // __HONORABLE_TITLE			// 달인
	#include "honor.h"
#endif	// __HONORABLE_TITLE			// 달인

#include "party.h"
extern	CParty g_Party;



//
//	State process
//	가급적 이곳에선 pMover->SetMotion을 하지 말것
//
void	CActionMover::ProcessState1( CMover* pMover,  DWORD dwState, float fSpeed )
{
	int nParam = 0;
	FLOAT fTheta = D3DXToRadian( pMover->GetAngle() );
	CModelObject* pModel = (CModelObject*)pMover->m_pModel;

	switch( dwState )
	{
	// 제자리 대기
	case OBJSTA_STAND:
		if( GetState() == OBJSTA_STAND )		// 오직 서있는 상태에서만 서있는 동작을 수행
		{
			if( GetStateFlag() & OBJSTAF_ETC )
			{
#ifdef __CLIENT
				switch( pModel->m_nLoop )
				{
				case ANILOOP_1PLAY:
					if( pModel->IsEndFrame() )
						RemoveStateFlag( OBJSTAF_ETC );
					break;
				case ANILOOP_CONT:
				case ANILOOP_LOOP:
					if( pMover->IsActiveMover() && g_WndMng.m_pWndWorld->m_bLButtonDown )		// 마우스 클릭하면 풀림.
					{
						if( pModel->IsEndFrame() )
							RemoveStateFlag( OBJSTAF_ETC );
					}
					break;
				}
#else // client
				if( pModel->IsEndFrame() )
					RemoveStateFlag( OBJSTAF_ETC );
#endif // not client
			} 
			else
			{
				if( pModel->IsEndFrame() )
				{
					if( pMover->IsStateMode( STATE_BASEMOTION_MODE ) )
					{
						pMover->SetMotion( MTI_STAND );		// 평화모드 서있기
					} else
					{
						if( xRandom( 5000 ) < 2 )
							pMover->SetMotion( MTI_IDLE2, ANILOOP_1PLAY ); // idle2 액션 
						else
						if( xRandom( 2000 ) < 2 )
							pMover->SetMotion( MTI_IDLE1, ANILOOP_1PLAY ); // idle2 액션 
						else
							pMover->SetMotion( MTI_STAND );		// 평화모드 서있기
					}
				}
			}
		}
		if( m_bGround && m_vDelta.y <= 0 )		// 땅에 서있을때 STAND가 들어오면 멈춤, 상승중일땐 Ground상태가 아님.
			m_vDelta.x = m_vDelta.z = 0;
		break;
#ifdef __Y_INTERFACE_VER3		
		// 좌측전진
	case OBJSTA_LMOVE:
		{
			if( pMover->m_dwFlag & MVRF_NOMOVE )	
				break;		// 이동금지 상태면 걍 리턴.
#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
			if( pMover->GetAdjParam( DST_CHRSTATE ) & CHS_LOOT)			break;
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans

			
			FLOAT fT = pMover->GetAngle();

			fT += 90.0f;
			if( fT > 360.0f )
				fT -= 360.0f;
			
			fT = D3DXToRadian(fT);

			if( IsStateFlag( OBJSTAF_WALK ) )		// 걷는모드
			{				
				m_vDelta.x	= sinf( fT ) * (fSpeed/4.0f);
				m_vDelta.z	= -cos( fT ) * (fSpeed/4.0f);				

				if( GetState() != OBJSTA_LMOVE )		break;		// 전진중일때만 동작 나옴
				if( IsStateFlag( OBJSTAF_COMBAT ) )	// 전투모드
					pMover->SetMotion( MTI_WALK );		// 전투모드로 걷기
				else
					pMover->SetMotion( MTI_WALK );		// 걍 걷기
			} else
			{
				if( pMover->m_dwTypeFlag & OBJTYPE_FLYING )
				{
					FLOAT fAngXZ = pMover->GetAngle();
					FLOAT fAngH  = pMover->GetAngleX();
					AngleToVector( &m_vDelta, fAngXZ, -fAngH, fSpeed );
				} else
				{
					m_vDelta.x = sinf(fT) * fSpeed;
					m_vDelta.z = -cosf(fT) * fSpeed;
				}
				if( GetState() != OBJSTA_LMOVE )		break;		// 전진중일때만 동작 나옴
				pMover->SetMotion( MTI_RUN );
			}
#ifdef __CLIENT		
			if( pMover->IsNPC() )
				if( pModel->IsAttrHit() )
					GetWorld()->m_pCamera->SetQuake( 15 );
#endif // CLIENT
		}
		break;
		// 우측전진
	case OBJSTA_RMOVE:
		{
			if( pMover->m_dwFlag & MVRF_NOMOVE )	
				break;		// 이동금지 상태면 걍 리턴.
#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
			if( pMover->GetAdjParam( DST_CHRSTATE ) & CHS_LOOT)			break;
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans


			FLOAT fT = pMover->GetAngle();

			fT -= 90.0f;
			if( fT < 0 )
				fT += 360.0f;
			
			fT = D3DXToRadian(fT);			
			
			if( IsStateFlag( OBJSTAF_WALK ) )		// 걷는모드
			{
				m_vDelta.x = sinf(fT) * (fSpeed/4.0f);		// 걷는다....
				m_vDelta.z = -cosf(fT) * (fSpeed/4.0f);
				
				if( GetState() != OBJSTA_RMOVE )		break;		// 전진중일때만 동작 나옴
				if( IsStateFlag( OBJSTAF_COMBAT ) )	// 전투모드
					pMover->SetMotion( MTI_WALK );		// 전투모드로 걷기
				else
					pMover->SetMotion( MTI_WALK );		// 걍 걷기
			} else
			{
				if( pMover->m_dwTypeFlag & OBJTYPE_FLYING )
				{
					FLOAT fAngXZ = pMover->GetAngle();
					FLOAT fAngH  = pMover->GetAngleX();
					AngleToVector( &m_vDelta, fAngXZ, -fAngH, fSpeed );
				} else
				{
					m_vDelta.x = sinf(fT) * fSpeed;
					m_vDelta.z = -cosf(fT) * fSpeed;
				}
				if( GetState() != OBJSTA_RMOVE )		break;		// 전진중일때만 동작 나옴
				pMover->SetMotion( MTI_RUN );
			}
#ifdef __CLIENT		
			if( pMover->IsNPC() )
				if( pModel->IsAttrHit() )
					GetWorld()->m_pCamera->SetQuake( 15 );
#endif // CLIENT
		}
		break;
#endif //__Y_INTERFACE_VER3

	// 전진
	case OBJSTA_FMOVE:
		{
			if( pMover->m_dwFlag & MVRF_NOMOVE )	
				break;		// 이동금지 상태면 걍 리턴.
#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
			if( pMover->GetAdjParam( DST_CHRSTATE ) & CHS_LOOT)			break;
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans

			
		if( IsStateFlag( OBJSTAF_WALK ) )		// 걷는모드
		{
			if( pMover->m_dwTypeFlag & OBJTYPE_FLYING )
			{
				FLOAT fAngXZ = pMover->GetAngle();
				FLOAT fAngH  = pMover->GetAngleX();
				AngleToVector( &m_vDelta, fAngXZ, -fAngH, fSpeed/4.0f );
			} else
			{
				m_vDelta.x = sinf(fTheta) * (fSpeed/4.0f);		// 걷는다....
				m_vDelta.z = -cosf(fTheta) * (fSpeed/4.0f);
			}
			if( GetState() != OBJSTA_FMOVE )		break;		// 전진중일때만 동작 나옴
			if( IsStateFlag( OBJSTAF_COMBAT ) )	// 전투모드
				pMover->SetMotion( MTI_WALK );		// 전투모드로 걷기
			else
				pMover->SetMotion( MTI_WALK );		// 걍 걷기
		} else
		{
			if( pMover->m_dwTypeFlag & OBJTYPE_FLYING )
			{
				FLOAT fAngXZ = pMover->GetAngle();
				FLOAT fAngH  = pMover->GetAngleX();
				AngleToVector( &m_vDelta, fAngXZ, -fAngH, fSpeed );
			} else
			{
				m_vDelta.x = sinf(fTheta) * fSpeed;
				m_vDelta.z = -cosf(fTheta) * fSpeed;
			}
			if( GetState() != OBJSTA_FMOVE )		break;		// 전진중일때만 동작 나옴
			pMover->SetMotion( MTI_RUN );
		}
#ifdef __CLIENT		
		if( pMover->IsNPC() )
			if( pModel->IsAttrHit() )
				GetWorld()->m_pCamera->SetQuake( 15 );
#endif // CLIENT
		}
		break;
	// 제자리 뛰기
#ifdef __CLIENT
	case OBJSTA_STOP_RUN:
		if( IsActDamage() )	break;
		m_vDelta.x = 0.0f;
		m_vDelta.z = 0.0f;
		if( GetState() != OBJSTA_STOP_RUN )		break;
		pMover->SetMotion( MTI_RUN );
		break;
#endif //__CLIENT
		
	// 후진
	case OBJSTA_BMOVE:
		if( IsActDamage() )	break;
		m_vDelta.x =  sinf(fTheta) * (-fSpeed/5.0f);
		m_vDelta.z = -cosf(fTheta) * (-fSpeed/5.0f);
		if( GetState() != OBJSTA_BMOVE )		break;		// 전진중일때만 동작 나옴
		if( IsStateFlag( OBJSTAF_COMBAT ) )		
			pMover->SetMotion( MTI_BACK );	// 전투모드로 걷기
		else
			pMover->SetMotion( MTI_BACK );			// 걍 걷기
		break;
	// 수평이동
	case OBJSTA_LEFT:
	case OBJSTA_RIGHT:
		break;
	// 앉아있는 모션중.
	case OBJSTA_SIT:
		if( pMover->m_dwMotion == MTI_SIT )		// 앉는중
		{
			if( pModel->IsEndFrame() )		// 앉는모션 끝남.
			{
				pMover->SetMotion( MTI_SITSTAND, ANILOOP_LOOP );
#ifdef __CLIENT
				if( g_Party.IsMember( pMover->m_idPlayer) && g_Party.m_nModeTime[PARTY_STRETCHING_MODE] )
				{
					D3DXVECTOR3 v = pMover->GetPos();
					
					ItemProp* pItemProp = prj.GetPartySkill( ST_STRETCHING );
					
					CSfx *pSfx = CreateSfx( g_Neuz.m_pd3dDevice, pItemProp->dwSfxObj, v, pMover->GetId() );	// 머리위에 sfx생성.
					
					if( pSfx )
						pSfx->SetScale( D3DXVECTOR3(1.0f, 1.0f, 1.0f) );
				}
#endif	// __CLIENT
			}
		} else
		if( pMover->m_dwMotion == MTI_GETUP )		// 일어나는중
		{
			if( pModel->IsEndFrame() )
			{
				ResetState( OBJSTA_MOVE_ALL );		// 앉는상태 끝
			}
		} else
		if( pMover->m_dwMotion != MTI_SITSTAND )
			pMover->SetMotion( MTI_SITSTAND, ANILOOP_LOOP );	// 에러났을경우에.
		
		
		break;
	case OBJSTA_PICKUP:
		if( pModel->IsEndFrame() )
		{
			ResetState( OBJSTA_MOVE_ALL );
		}
		break;
	case OBJSTA_COLLECT:
		/*
		if( pMover->IsNPC() )
		{
			Error( "OBJSTA_COLLECT : 웬 몬스터가 채집으로 들어왔다 %s", pMover->GetName() );
			break;
		}
 #ifdef __WORLDSERVER
		if( pMover->OnActCollecting() == TRUE || IsMove() )		// 채집동작이 끝나면 / 채집하다 움직이면.
		{
			ResetState( OBJSTA_ACTION_ALL );			// 해제
			g_UserMng.AddHdr( (CUser *)pMover, SNAPSHOTTYPE_STOP_COLLECT );	// 채집끝남.
		}
 #elif defined(__CLIENT)
		pMover->OnActCollecting();
 #endif
		*/
		// 모션은 조건이 끝날때까지 계속 반복됨.
		break;
	case OBJSTA_STUN:
		if( pMover->m_dwMotion != MTI_GROGGY )		// 그로기 모션이 돌고 있지 않을때
			if( (GetState() & (~OBJSTA_ACTION_ALL)) != OBJSTA_NOT_STAND )		// 아무것도 안하고 대기하는 상태에선 그로기 모션으로 다시 되돌려 놓자.
				pMover->SetMotion( MTI_GROGGY, ANILOOP_LOOP, MOP_FIXED );
		if( (pMover->GetAdjParam( DST_CHRSTATE ) & CHS_STUN) == 0 )		// 스턴상태가 끝났으면
		{
			pMover->m_wStunCnt = 0;						// 스턴카운트 해제.
			pMover->m_dwFlag &= (~MVRF_NOACTION);		// 액션금지도 해제하고
			ResetState( OBJSTA_ACTION_ALL );			// 액션상태도 해제한다.
		}
		break;
		
	case OBJSTA_APPEAR:  // 등장씬.
		if( pModel->IsEndFrame() )
		{
		#ifdef __WORLDSERVER
			pMover->PostAIMsg( AIMSG_END_APPEAR );		// 등장씬 애니메이션이 끝났음을 알림.
		#endif
			ResetState( OBJSTA_ACTION_ALL );
		}
		break;
		// 좌/우 돌기
	case OBJSTA_LTURN:
		if( IsActAttack() )		break;
		pMover->AddAngle( 4 );
		break;
	case OBJSTA_RTURN:
		if( IsActAttack() )		break;
		pMover->AddAngle( -4 );
		break;
	// 피치들어올리기
	case OBJSTA_LOOKUP:
		if( IsActAttack() )		break;
		if( pMover->GetAngleX() > -45.0f )
			pMover->AddAngleX( -1 );
		break;
	case OBJSTA_LOOKDOWN:
		if( IsActAttack() )		break;
		if( pMover->GetAngleX() < 45.0f )
			pMover->AddAngleX( 1 );
		break;
		
	// 점프-발구름
	case OBJSTA_SJUMP1:
		if( pMover->m_dwFlag & MVRF_NOMOVE )	break;		// 이동금지 상태면 걍 리턴.
		SetJumpState( OBJSTA_SJUMP2 );
		m_vDelta.y = 0.2f + (pMover->GetParam( DST_JUMPING, 0 ) / 1000.0f);
		if( (GetState() & OBJSTA_COLLECT)== 0 )
		{
			pMover->SetMotion( MTI_JUMP2, ANILOOP_CONT );	// 상승동작 전환
		}
		pModel->SetMotionBlending( FALSE );
		break;
	// 점프-상승
	case OBJSTA_SJUMP2:
		if( pMover->m_dwFlag & MVRF_NOMOVE )	break;		// 이동금지 상태면 걍 리턴.
		if( (GetState() & OBJSTA_COLLECT)== 0 )
		{
			pMover->SetMotion( MTI_JUMP2, ANILOOP_CONT );	// 상승동작 전환
		}
		if( m_vDelta.y <= 0 )		// 아래로 떨어지기 시작했으면 하강으로 전환
		{
			SetJumpState( OBJSTA_SJUMP3 );
			if( (GetState() & OBJSTA_COLLECT)== 0 ) pMover->SetMotion( MTI_JUMP3, ANILOOP_CONT );
			pModel->SetMotionBlending( FALSE );
		}
		break;
	// 점프-하강
	case OBJSTA_SJUMP3:
		if( pMover->m_dwFlag & MVRF_NOMOVE )	break;		// 이동금지 상태면 걍 리턴.
		if( (GetState() & OBJSTA_DMG_ALL) == 0 || (GetState() & OBJSTA_COLLECT)== 0 )			// 데미지중이 아닐때만 하강동작 변환.
		{
			if( TRUE == pMover->SetMotion( MTI_JUMP3, ANILOOP_CONT ) )	// 하강동작 전환
				pModel->SetMotionBlending( FALSE );
		}
		if( m_bGround )		// 바닥에 닿았으면 착지상태로 넘어감
		{
			SetJumpState( OBJSTA_SJUMP4 );	// 착지 - SetAction( OBJMSG_JUMP4 ); 와 같은식으로 바꾸자.
			pMover->OnActLanding();			// 착지 핸들러 콜.
			if( GetState() & OBJSTA_DMG_ALL)	break;		// 데미지상태라면 착지 동작 하지 않음.
			if( TRUE == pMover->SetMotion( MTI_JUMP4, ANILOOP_1PLAY ) )	// 착지동작 전환
			{
				pModel->SetMotionBlending( FALSE );		// 착지 동작은 블렌딩을 하지 않는게 자연스럽다.
			}
		}
		break;
	// 점프-착지
	case OBJSTA_SJUMP4:
		//if( pMover->m_dwFlag & MVRF_NOMOVE )	break;		// 이동금지 상태면 걍 리턴.
		ResetState( OBJSTA_JUMP_ALL );
		if( GetState() & OBJSTA_DMG_ALL )	
		{
			ResetState( OBJSTA_JUMP_ALL );
			break;		// 데미지 중이었으면 착지 상태 해제시키고 끝냄.
		}
		if( pModel->IsEndFrame() )		// 착지동작 다 끝나면 점프상태 해제
		{
			ResetState( OBJSTA_JUMP_ALL );
			break;
		}
#if __VER >= 13 // __HONORABLE_TITLE			// 달인
	#ifdef __WORLDSERVER
#ifndef __MAINSERVER
if(!pMover->IsPlayer())
	FILEOUT( "..\\HonorError.txt", "processstate1()AddHonorListAck()\n" );
#endif // __MAINSERVER
		if(pMover->IsPlayer() )
            ((CUser*)pMover)->SetHonorAdd(HS_JUMP,HI_COUNT_CHECK);
	#endif	// __WORLDSERVER
#endif	// __HONORABLE_TITLE			// 달인
		break;
	//--------- 공격 ---------------------------
	case OBJSTA_ATK1:
	case OBJSTA_ATK2:
	case OBJSTA_ATK3:
	case OBJSTA_ATK4:
		_ProcessStateAttack( dwState, nParam );
		break;
		
	case OBJSTA_SP_ATK1:
	case OBJSTA_SP_ATK2:
		ProcessStateAttackSP( dwState, nParam );
		break;

	//--------- 마법 ---------------------------
	case OBJSTA_ATK_MAGIC1:
		ProcessStateAttackMagic( dwState, nParam );
		break;
	//--------- 원거리 물리공격 ------------------
	case OBJSTA_ATK_RANGE1:
		ProcessStateAttackRange( dwState, nParam );
		break;
		
	case OBJSTA_ATK_MELEESKILL:
		ProcessStateMeleeSkill( dwState, nParam );
		break;
	case OBJSTA_ATK_CASTING1:
	case OBJSTA_ATK_CASTING2:
		ProcessStateMagicCasting( dwState, nParam );
		break;
	case OBJSTA_ATK_MAGICSKILL:
		ProcessStateMagicSkill( dwState, nParam );
		break;
		
	//----------- 데미지 -------------------------
	case OBJSTA_DMG:
		if( pModel->IsEndFrame() )	
		{
			ResetState( OBJSTA_DMG_ALL );
		}
		break;
	case OBJSTA_DMG_FLY:
		if( m_bGround )
		{
			if( (GetState() & OBJSTA_STUN) )
			{
				ResetState( OBJSTA_DMG_FLY );			// 액션상태도 해제한다.
				break;
			}
			
			if( m_vDelta.y == 0 )	// 처음 뜰때도 m_bGround가 1이기땜에 하강하고 있을때 바닥에 닿은것만 유효!
			{
				m_vDelta.x = m_vDelta.z = 0;	// 떨어지면 걍 속도 없애자.
				m_vDeltaE.x = m_vDelta.x;	// m_vDeltaE는 자동적으로 마찰이 계산된다.
				m_vDeltaE.z = m_vDelta.z;
#if __VER >= 15 // __BS_FIXED_KNOCKBACK
				m_vDelta.y = 0.04f;
#else
				m_vDelta.y = 0.12f;	// bGround가 1이면 이미 .y값을 0으로 만들었기땜에 다시 상승 벡터를 넣어야 한다. 개선의 필요가 있음.
#endif
				SetState( OBJSTA_DMG_ALL, OBJSTA_DMG_DIE );	
				pMover->SetMotion( MTI_DMGDIE, ANILOOP_CONT );		// 털썩! 떨어진후 쓰러져있는 모습
				break;
			}
		}
		break;
	// 땅에 털썩 떨어지고 난후 미끄러지는 중
	case OBJSTA_DMG_DIE:
		m_vDelta.x = 0;
		m_vDelta.z = 0;
		
		if( pModel->IsEndFrame() )		// 프레임이 끝나면 죽어 널부러진 상태. 모션은 현재 모션 그대로 이어짐.
		{
			if( pMover->m_nHitPoint > 0 )	// 바닥에 떨어지고 난후 HP가 남아 있으면 
			{
				TRACE( "\r\n" );
				SetState( OBJSTA_DMG_ALL, OBJSTA_DMG_LIVE );	// 다시 일어나고
				pMover->SetMotion( MTI_DMGLIVE, ANILOOP_1PLAY );	// 일어나는 모션
			} 
			else
			{
				SetState( OBJSTA_DMG_ALL, OBJSTA_DEAD );		// HP가 없으면 바로 죽음.
			}
		}
		break;
	// 쓰러졌다 일어나는 상태 - 데미지 중으로 처리.
	case OBJSTA_DMG_LIVE:
		if( pModel->IsEndFrame() )
		{
			pMover->m_dwFlag &= (~MVRF_NOACTION);		// 액션금지도 해제하고
			ResetState( OBJSTA_DMG_ALL );	// 프레임 끝나면 클리어.
			ResetState( OBJSTA_ATK_ALL );
		}
		break;
	case OBJSTA_DEAD:	// 죽어 널부러진 상태
		if( pModel->m_nLoop != ANILOOP_CONT )	// 계속 죽어널부러지는 현상땜에 땜빵처리 했다 우하하하하
			pModel->m_nLoop = ANILOOP_CONT;

		pMover->m_dwFlag &= (~MVRF_NOACTION);		// 액션금지도 해제하고
 #ifdef __WORLDSERVER
		// 클라에선 오브젝트 지우는게 따로 오므로 이부분을 수행할 필요 없다.
		if( !pMover->IsPlayer() ) 
		{
			m_nDeadCnt--;
			/*
			if( pMover->IsResourceMonster() )		// 자원이 있는 몬스터냐
			{				
				if( pMover->m_nResource == 0 )		// 자원이 다 떨어졌냐
				{
					if( m_nDeadCnt > 10800 - 200 )	// 나타난지 얼마 안된건데 벌써 자원이 떨어졌다면 뭔가 잘못된거다.
						Error( "OBJSTA_DEAD:%d %s", m_nDeadCnt, pMover->GetName() );
					m_nDeadCnt = 0;			// 자원이 다 떨어졌으면 이제 없어져야줴~
				}
			}
			*/
			if( m_nDeadCnt <= 0 )
			{
				SetState( OBJSTA_DMG_ALL, OBJSTA_DISAPPEAR );	// 사라지는 상태로 전환
				m_nCount = 0;
			}
		} 
 #endif // WORLDSERVER

#ifdef __CLIENT
		if( pMover->IsNPC() )
		{
			if( pModel->IsAttrQuake() )	// 죽을때 쿵.
			{
				if( pMover->GetProp()->dwClass == RANK_MATERIAL )
					GetWorld()->m_pCamera->SetQuake( 60, 1.0f );
				else
					GetWorld()->m_pCamera->SetQuake( 15 );
			}
		}
#endif	// __CLIENT
		break;
	case OBJSTA_DISAPPEAR:	// 사라지는 상태
#ifndef __CLIENT
		if( m_nCount > FRAME_PER_SEC )	// 1초후 사라짐
//		if( m_nCount++ > SEC1 )
		{
			pMover->Delete();
		}
#endif	//__CLIENT
		break;
	case OBJSTA_RESURRECTION:		// 부활 하는 중.
		if( pModel->IsEndFrame() )
			ResetState( OBJSTA_DMG_ALL );
		break;
	}
}

//
//	전사들의 근접 전투스킬류....
//

void	CActionMover::ProcessStateMeleeSkill( DWORD dwState, int nParam )
{
	CMover* pMover = m_pMover;
	CModelObject *pModel = (CModelObject *)pMover->m_pModel;

	int nSkill = pMover->GetActParam(0);

	if( nSkill == SI_ACR_YOYO_COUNTER )	// 싸이런 스트라이크는 예외다.
	{
	}
	else		
	if( nSkill == SI_BLD_DOUBLESW_SILENTSTRIKE )	// 싸이런 스트라이크는 예외다.
	{
		if( pModel->IsEndFrame() )
		{
			ResetState( OBJSTA_ATK_ALL );
			return;
		}
	} 
	else
	{
		if( pModel->m_fFrameCurrent >= (pModel->m_nFrameMax * 0.65f) )	// 동작캔슬 할때 이방법말고 다른방법으로 하자!!!!!!!!!!!!!!!!!!!!!
		{
			pModel->m_bEndFrame = TRUE;		// 애니를 중간에 중단시켰으므로 반드시 이걸 TRUE로 해줘야 한다.
			ResetState( OBJSTA_ATK_ALL );
			return;
		}
	}

	if( (g_nProcessCnt & 3) == 0 )
	{
		if( m_nShootCnt > 0 )			// 연속발사가 세팅되면
		{
			pMover->OnMeleeSkill( 0 );	// 다시 발사시키고
			m_nShootCnt--;				// 개수 하나씩 뺌.
		}
	}
	

	if( pModel->m_nPause > 0 )
	{
		pModel->m_nPause --;
	} 
	else
	{
		if( pModel->IsAttrHit() )	// 현재 프레임에 타격속성이 있는가?
		{
			if( nSkill == SI_BLD_DOUBLESW_SILENTSTRIKE )	// 싸이런 스트라이크는 예외다.
			{
				D3DXVECTOR3 vForce;
				AngleToVectorXZ( &vForce, pMover->GetAngle(), 0.75f );		// 앞쪽으로 돌진하는 힘 설정.
				SetExternalForce( vForce );
			} 
			else if( nSkill == SI_BIL_KNU_BELIALSMESHING )	// 베리알 스매싱도 앞으로 전진.
			{
				D3DXVECTOR3 vForce;
				AngleToVectorXZ( &vForce, pMover->GetAngle(), 0.35f );		// 앞쪽으로 돌진하는 힘 설정.
				SetExternalForce( vForce );
			}

			pMover->OnMeleeSkill( 0, m_nMotionHitCount );	// 무버의 핸들러를 호출.
			m_nMotionHitCount++;

#ifdef __CLIENT			
			pModel->m_nPause = 4;	// frame 멈춤 - 클라일경우만 멈추자.
#endif

			if( pMover->GetAct() != OBJACT_USESKILL )	
				return;	// 스킬사용후 타점이 되기전에 다른 명령으로 바뀐경우. 이런상황이 와선 안된다.
			int		nSkill = pMover->GetActParam(0);
			OBJID	idTarget = pMover->GetActParam(1);
			DWORD	dwLevel = pMover->GetActParam(3);
			
			ItemProp* pSkillProp = NULL;
			AddSkillProp* pAddSkillProp = NULL;
			BOOL bRet = pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, nSkill, dwLevel, "ProcessStateMeleeSkill" );
			if( bRet == FALSE )
				return;
			
			if( pAddSkillProp->nSkillCount > 0 )		// 마법스킬의 경우 발사체개수가 있을때.
			{
				m_nShootCnt = pAddSkillProp->nSkillCount;		// 캐스팅발사동작 이후 xx프레임에 한번씩 차례로 발사.
				m_nShootCnt --;		// 여기서 이미 하나 발사했으니 하나 빼주는게 맞다.
			}
		}
	}
}

//
//	마법사들의 마법공격동작 중....
//

// 캐스팅 동작중.
void	CActionMover::ProcessStateMagicCasting( DWORD dwState, int nParam )
{
	CMover* pMover = m_pMover;
	CModelObject *pModel = (CModelObject *)pMover->m_pModel;
	
	if( dwState == OBJSTA_ATK_CASTING1 )	// 팔을 들어올리는 중
	{
		if( pModel->IsEndFrame() )	// 애니메이션이 끝났다면 다음.
		{
			pMover->SetMotion( pMover->m_dwMotion + 1, ANILOOP_LOOP, MOP_SWDFORCE | MOP_NO_TRANS | MOP_FIXED );		// 다음 동작으로
//			m_nCount = 0;
			SetState( OBJSTA_ATK_ALL, OBJSTA_ATK_CASTING2 );
		}
	} 
	else if( dwState == OBJSTA_ATK_CASTING2 )	// 시전시간 딜레이중
	{
//		if( ++m_nCount >= m_nCastingTime )
		if( m_nCount >= ( m_nCastingTime ) )
		{
			int dwMotion = pMover->m_dwMotion + 1;
		#ifdef __CLIENT
			if( pMover->HasBuffByIk3( IK3_TEXT_DISGUISE ) )
				dwMotion = MTI_ATK1;
		#endif
			pMover->SetMotion( dwMotion, ANILOOP_1PLAY, MOP_SWDFORCE | MOP_NO_TRANS | MOP_FIXED );		// 다음 동작으로
			SetState( OBJSTA_ATK_ALL, OBJSTA_ATK_MAGICSKILL );
		}
	}

#if defined(__CLIENT)
	pModel->m_SparkInfo.m_bUsed  = TRUE;
	pModel->m_SparkInfo.m_v3Color.x = 0.0f;
	pModel->m_SparkInfo.m_v3Color.y = 2.0f;
	pModel->m_SparkInfo.m_v3Color.z = 2.0f;
	pModel->m_SparkInfo.m_nCnt = 0;
	pModel->m_SparkInfo.m_fLerp= 1.0f;
	
	AngleToVectorXZ( &pModel->m_SparkInfo.m_v3SparkDir, pMover->GetAngle(), -1.0f );
#endif //defined(__CLIENT)
}


// 신버전. - 마법 발사동작중.
void	CActionMover::ProcessStateMagicSkill( DWORD dwState, int nParam )
{
	CMover* pMover = m_pMover;
	CModelObject *pModel = (CModelObject *)pMover->m_pModel;
	
	if( pModel->IsEndFrame() )	// 애니메이션이 끝났다면 상태 해제
	{
		ResetState( OBJSTA_ATK_ALL );
		return;
	}

	if( (g_nProcessCnt & 3) == 0 )
	{
		if( m_nShootCnt > 0 )		// 연속발사가 세팅되면
		{
			pMover->OnMagicSkill( 0 );	// 다시 발사시키고
			m_nShootCnt--;		// 개수 하나씩 뺌.
		}
	}

	if( pModel->IsAttrHit() )	// 현재 프레임에 타점이 있는가?
	{
		pMover->OnMagicSkill( 0, m_nMotionHitCount );	// 무버의 핸들러를 호출.
		m_nMotionHitCount++;
		
		if( pMover->GetAct() != OBJACT_USESKILL )	
			return;	// 스킬사용후 타점이 되기전에 다른 명령으로 바뀐경우. 이런상황이 와선 안된다.
		int		nSkill = pMover->GetActParam(0);
		OBJID	idTarget = pMover->GetActParam(1);
		DWORD	dwLevel = pMover->GetActParam(3);
		
		ItemProp* pSkillProp = NULL;
		AddSkillProp* pAddSkillProp = NULL;
		BOOL bRet = pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, nSkill, dwLevel, "ProcessStateMagicSkill" );
		if( bRet == FALSE )
			return;
		
		if( pAddSkillProp->nSkillCount > 0 )		// 마법스킬의 경우 발사체개수가 있을때.
		{
			m_nShootCnt = pAddSkillProp->nSkillCount;		// 캐스팅발사동작 이후 xx프레임에 한번씩 차례로 발사.
			m_nShootCnt --;		// 여기서 이미 하나 발사했으니 하나 빼주는게 맞다.
		}
		
	}
}

//
//		공격 액션 처리
//
void	CActionMover::_ProcessStateAttack( DWORD dwState, int nParam )
{

	CMover* pMover = m_pMover;
	CModelObject	*pModel = (CModelObject *)pMover->m_pModel;

	switch( dwState )
	{
	case OBJSTA_ATK1:
	case OBJSTA_ATK2:
	case OBJSTA_ATK3:
	case OBJSTA_ATK4:
		// 공격동작이 끝나면 공격상태 해제
		if( pMover->IsPlayer() )
		{
			int		nReqLevel = 0;		// 다음 연타로 넘어가기 위한 필요레벨.
			OBJMSG	dwNextMsg = OBJMSG_ATK2;
			switch( dwState )
			{
			case OBJSTA_ATK1:	
			case OBJSTA_ATK2:	
			case OBJSTA_ATK3:	
			case OBJSTA_ATK4:	

				switch( dwState )
				{
				case OBJSTA_ATK1:	nReqLevel = 2;	dwNextMsg = OBJMSG_ATK2;	break;
				case OBJSTA_ATK2:	nReqLevel = 8;	dwNextMsg = OBJMSG_ATK3;	break;
				case OBJSTA_ATK3:	nReqLevel = 12;	dwNextMsg = OBJMSG_ATK4;	break;
				case OBJSTA_ATK4:	nReqLevel = 999;	dwNextMsg = OBJMSG_ATK1;	break;
				}
				if( pModel->IsEndFrame() )		// 애니메이션이 끝났다. 연타로 넘어갈껀지 말껀지.
				{
				#ifdef __WORLDSERVER					
					m_objidHit = NULL_ID;
				#endif
					ResetState( OBJSTA_ATK_ALL );
				#ifdef __CLIENT
					if( GetMover()->IsActiveMover() )
					{
						if( (pMover->GetLevel() >= nReqLevel) && ((m_dwCtrlMsg & CTRLMSG_LDOWN) || ((CWndWorld *)g_WndMng.m_pWndWorld)->m_bAutoAttack) )	// 직업레벨 X이상이면 다음연타가능
						{
							ItemProp* pItemProp = pMover->GetActiveHandItemProp();
							if( pItemProp && pItemProp->dwItemKind3 == IK3_YOYO )
							{
								if( dwState == OBJSTA_ATK2 || dwState == OBJSTA_ATK3 || dwState == OBJSTA_ATK4 )
									break;
							}

							CMover* pHitObj	= prj.GetMover( m_objidHit );
							if( IsValidObj( (CObj*)pHitObj ) &&		// 다음 스킬이 예약되어있으면 여기서 끝낸다.
								g_WndMng.m_pWndWorld->GetNextSkill() == NEXTSKILL_NONE ) 
							{
								int nError	= SendActMsg( dwNextMsg, m_objidHit, MAKELONG( 0, 0 ), MAKELONG( 0, 0 ) );
								if( nError > 0 )	
								{
#ifdef __HACK_1023
									ItemProp* pHandItemProp		= pMover->GetActiveHandItemProp();
									FLOAT fVal	= pHandItemProp? pHandItemProp->fAttackSpeed: 0.0F;
									g_DPlay.SendMeleeAttack ( dwNextMsg, m_objidHit, MAKELONG( 0, 0 ), MAKELONG( 0, (WORD)nError ), fVal );
#else	// __HACK_1023
									g_DPlay.SendMeleeAttack ( dwNextMsg, m_objidHit, MAKELONG( 0, 0 ), MAKELONG( 0, (WORD)nError ) );
#endif	// __HACK_1023
								}
							} else
								m_objidHit = NULL_ID;
						}
					}
				#endif	// CLIENT
				}
				break;
			}
		} 
		else	
		// monster
		{
		#ifdef __CLIENT
			if( pMover->m_dwIndex == MI_CLOCKWORK1 )
			{
				if( dwState == OBJSTA_ATK3 )
				{
					if( (pMover->m_dwFlag & MVRF_LASERCHARGE) == 0 )	// 레이저충전 이펙트가 아직 생성 안되었음.
					{
						// 이마에서 기 모으기 이펙트.
						CSfx *pSfx = CreateSfx( D3DDEVICE, XI_NPCSP1DIRAMP, pMover->GetPos(), pMover->GetId(), D3DXVECTOR3(0,0,0), NULL_ID, -1 );			
						if( pSfx )
						{
							pSfx->SetScale( D3DXVECTOR3( 2.0f, 2.0f, 2.0f ) );
							pMover->m_dwFlag |= MVRF_LASERCHARGE;
						}
					}
				}
			}
			else
			if( pMover->m_dwIndex == MI_BIGMUSCLE )
			{
			}
			else
			if( pMover->m_dwIndex == MI_KRRR )
			{
			}
			else
			if( pMover->m_dwIndex == MI_MUSHMOOT )
			{
				if( dwState == OBJSTA_ATK3 )
				{
					if( (pMover->m_dwFlag & MVRF_LASERCHARGE) == 0 )	// 레이저충전 이펙트가 아직 생성 안되었음.
					{
						// 이마에서 기 모으기 이펙트.
						CSfxMushmootCharge *pSfx = (CSfxMushmootCharge*)CreateSfx( D3DDEVICE, XI_SKILL_MUSHMOOT_CHARGE, pMover->GetPos(), pMover->GetId(), D3DXVECTOR3(0,0,0), NULL_ID, -1 );			
						if( pSfx )
						{
							pSfx->m_nEventPos = 0;
							pSfx->SetScale( D3DXVECTOR3( 2.0f, 2.0f, 2.0f ) );
							pMover->m_dwFlag |= MVRF_LASERCHARGE;
						}
					}
				}
				else
				if( dwState == OBJSTA_ATK4 )
				{
					if( (pMover->m_dwFlag & MVRF_LASERCHARGE) == 0 )	// 레이저충전 이펙트가 아직 생성 안되었음.
					{
						CSfxMushmootCharge *pSfx = NULL;
						// 이마에서 기 모으기 이펙트.
						pSfx = (CSfxMushmootCharge*)CreateSfx( D3DDEVICE, XI_SKILL_MUSHMOOT_CHARGE, pMover->GetPos(), pMover->GetId(), D3DXVECTOR3(0,0,0), NULL_ID, -1 );			
						if( pSfx )
						{
							pSfx->m_nEventPos = 1;
							pSfx->SetScale( D3DXVECTOR3( 2.0f, 2.0f, 2.0f ) );
							pMover->m_dwFlag |= MVRF_LASERCHARGE;
						}
						pSfx = (CSfxMushmootCharge*)CreateSfx( D3DDEVICE, XI_SKILL_MUSHMOOT_CHARGE, pMover->GetPos(), pMover->GetId(), D3DXVECTOR3(0,0,0), NULL_ID, -1 );			
						if( pSfx )
						{
							pSfx->m_nEventPos = 2;
							pSfx->SetScale( D3DXVECTOR3( 2.0f, 2.0f, 2.0f ) );
							pMover->m_dwFlag |= MVRF_LASERCHARGE;
						}
						pSfx = (CSfxMushmootCharge*)CreateSfx( D3DDEVICE, XI_SKILL_MUSHMOOT_CHARGE, pMover->GetPos(), pMover->GetId(), D3DXVECTOR3(0,0,0), NULL_ID, -1 );			
						if( pSfx )
						{
							pSfx->m_nEventPos = 3;
							pSfx->SetScale( D3DXVECTOR3( 2.0f, 2.0f, 2.0f ) );
							pMover->m_dwFlag |= MVRF_LASERCHARGE;
						}
						pSfx = (CSfxMushmootCharge*)CreateSfx( D3DDEVICE, XI_SKILL_MUSHMOOT_CHARGE, pMover->GetPos(), pMover->GetId(), D3DXVECTOR3(0,0,0), NULL_ID, -1 );			
						if( pSfx )
						{
							pSfx->m_nEventPos = 4;
							pSfx->SetScale( D3DXVECTOR3( 2.0f, 2.0f, 2.0f ) );
							pMover->m_dwFlag |= MVRF_LASERCHARGE;
						}
					}
				}
			}	
		#endif // Client
			if( pMover->m_dwFlag & MVRF_HITCONT )
				pMover->OnAttackMeleeContinue( dwState );

			if( pModel->IsEndFrame() )
			{
				pMover->OnEndMeleeAttack( dwState );
				pMover->m_dwFlag &= (~MVRF_HITCONT);		// 애니끝났으면 플랙 해제.
				pMover->m_dwFlag &= (~MVRF_LASERCHARGE);	// 애니끝났으면 플랙 해제.
				m_objidHit	= NULL_ID;
				ResetState( OBJSTA_ATK_ALL );
			#ifdef __CLIENT
				extern CPartsLaser g_Laser;
				g_Laser.m_bActive = FALSE;		// 애니 끝나면 레이저 해제(땜빵)
			#endif
				pMover->m_pActMover->m_nMotionHitCount = 0;
			}
		}
		
		if( pModel->m_nPause > 0 )
		{
			CMover* pHitObj	= prj.GetMover( m_idTarget );
		#ifdef __CLIENT
			D3DXVECTOR3 vDist = pMover->GetPos() - CMover::GetActiveMover()->GetPos();
			float fDistSq = D3DXVec3LengthSq( &vDist );
			if( pHitObj && pMover->IsPlayer() && fDistSq < 32.0f * 32.0f )		// 멀리있는놈은 파티클 생성안함.
			{
				int i;
				D3DXVECTOR3	vPos = pHitObj->GetPos();

				D3DXVECTOR3 vLocal;
				AngleToVectorXZ( &vLocal, pMover->GetAngle(), 1.0f );		// 모션 방향쪽으로 1미터지점에 파티클 생성.
				vLocal += pMover->GetPos();
				vPos = vLocal;

				vPos.y += 0.7f;
				D3DXVECTOR3 vVel;
				float	fAngXZ, fAngH;
				float	fSpeed;
				float	fDist;
				float	fMoverAngle = pMover->GetAngle();
				CParticles *pParticles = NULL;

				int nOption = 0;
				int nEffLevel = 0;
				if( pMover->IsActiveMover() )
				{
					CItemElem *pItemElem = pMover->GetWeaponItem();
					if( pItemElem )
						nOption = pItemElem->GetAbilityOption();
				} else
				{	// 타 플레이어는 페이크아이템이므로 여기서에서 값을 빼온다.
					nOption = pMover->m_aEquipInfo[PARTS_RWEAPON].nOption & 0xFF;
				}

				if( nOption == 10 )
					nEffLevel = 5;
				else if( nOption == 9 )
					nEffLevel = 4;
				else if( nOption == 8 )
					nEffLevel = 3;
				else if( nOption == 7 )
					nEffLevel = 2;
				else if( nOption >= 4 )	// 4 ~ 6
					nEffLevel = 1;
				else 
					nEffLevel = 0;
					
				for( i = 0; i < 1; i ++ )
				{
					fAngXZ = xRandomF(360);
					
					fAngXZ = D3DXToRadian( fAngXZ );
					fAngH = 15.0f + xRandomF(45);
					fAngH = D3DXToRadian( fAngH );
					fSpeed = (0.1f + xRandomF(0.03f));// / 1.0f;
					
					fDist = cosf(-fAngH) * fSpeed;
					vVel.x = sinf(fAngXZ) * fDist;
					vVel.z = -cosf(fAngXZ) * fDist;
					vVel.y = -sinf(-fAngH) * fSpeed;
					if( g_Option.m_bSFXRenderOff != TRUE )
					{
						if( nEffLevel == 5 )
						{
							pParticles = g_ParticleMng.CreateParticle( nEffLevel + xRandom(6), vPos, vVel, CMover::GetActiveMover()->GetPos().y );
						} else
							pParticles = g_ParticleMng.CreateParticle( nEffLevel, vPos, vVel, CMover::GetActiveMover()->GetPos().y );
					}
					if( pParticles )
						pParticles->m_bGravity = TRUE;
				}
			}
		#endif // CLIENT
		
		// 포즈카운트가 다 끝나면 뒤로 밀리기 시작
		// 2006.09.16 raiders 막아둠. 이유: CActionMover의 m_fAccAng는 사용하지 않는 변수
			--pModel->m_nPause; 
//			if( --pModel->m_nPause == 0 )		
//			{
//				if( pHitObj )
//					pHitObj->m_pActMover->m_fAccAng = pMover->GetAngle();				// 때린놈이 보고있는 방향으로 밀림
//			}
		// Pause > 0 
		} else
		{
		#if defined(__CLIENT)
			ItemProp* pItemProp = pMover->GetActiveHandItemProp();

			// 요요는 일반 밀리 평타와는 다르게
			// 흔들림효과(Quake)를 기준으로 발사체를 발사하고
			// 실제 효과처리(데메지, 이펙트)는 타격속성에서 처리한다.
			if( pMover->IsPlayer() && pItemProp && pItemProp->dwItemKind3 == IK3_YOYO && pModel->IsAttrQuake() )
			{
				CMover* pHitObj	= prj.GetMover( m_idTarget );
				if( IsInvalidObj(pHitObj) )
					break;

				DWORD dwSfxObj = pItemProp->dwSfxObj2;
				
				if( dwSfxObj == NULL_ID )	
					break;
				
				D3DXVECTOR3 vPos;
				D3DXVECTOR3 vLocal;
				D3DXVECTOR3 vPosSrc   = pMover->GetPos() + D3DXVECTOR3( 0.0f, 1.0f, 0.0f ); // 발사 지점을 임의로 올려준다. 땜빵 
				D3DXVECTOR3 vPosDest  = pHitObj->GetPos() + D3DXVECTOR3( 0.0f, 1.0f, 0.0f ); // 목표 지점을 임의로 올려준다. 땜빵 
				
				CSfx* pSfx = NULL;
				
				if( pMover->m_dwMotion == MTI_ATK1 )
				{
					pModel->GetHandPos( &vPos, PARTS_RWEAPON, pMover->GetMatrixWorld() );
					pSfx = CreateSfxYoYo( D3DDEVICE, dwSfxObj, vPos, pMover->GetId(), vPosDest );	
					((CSfxItemYoyoAtk*)pSfx)->MakePath(PARTS_RWEAPON);					
				}
				else
				if( pMover->m_dwMotion == MTI_ATK2 )
				{
					pModel->GetHandPos( &vPos, PARTS_LWEAPON, pMover->GetMatrixWorld() );
					pSfx = CreateSfxYoYo( D3DDEVICE, dwSfxObj, vPos, pMover->GetId(), vPosDest );	
					((CSfxItemYoyoAtk*)pSfx)->MakePath(PARTS_LWEAPON);
				}
			}
		#endif //__CLIENT

			if( pModel->IsAttrHit() )	// 현재 프레임에 타격속성이 있는가?
			{
				pMover->m_dwFlag |= MVRF_HITCONT;
				
				CMover* pHitObj	= prj.GetMover( m_idTarget );
				if( IsValidObj( (CObj*)pHitObj ) && pHitObj->IsLive() )
				{
					// 핸들러 호출.
					pMover->OnAttackMelee( dwState, pHitObj );
				}
			} // is hitattr ?

		#if defined(__CLIENT)
			if( pModel->IsAttrQuake() )
			{
				if( pMover->m_dwIndex == MI_BIGMUSCLE || 
					pMover->m_dwIndex == MI_KRRR || 
					pMover->m_dwIndex == MI_MUSHMOOT )
					pMover->GetWorld()->m_pCamera->SetQuake( 60, 0.7f );	
			}
		#endif // defined(__CLIENT)
			
		}
		break;
	}
}

//
//		특수공격동작중.
//
void	CActionMover::ProcessStateAttackSP( DWORD dwState, int nParam )
{
	CMover* pMover = m_pMover;
	CModelObject	*pModel = (CModelObject *)pMover->m_pModel;
	
	switch( dwState )
	{
	case OBJSTA_SP_ATK1:
	case OBJSTA_SP_ATK2:
		if( pModel->IsEndFrame() )
		{
			ResetState( OBJSTA_ATK_ALL );
		}
		if( pModel->IsAttrHit() )			// 현재 프레임에 타격속성이 있는가? 
		{
			pMover->OnAttackSP();		// 무버의 특수공격 핸들러를 호출.
		}
		break;
	}
}


//
//		완드공격동작
//
void	CActionMover::ProcessStateAttackMagic( DWORD dwState, int nParam )
{
	CMover* pMover = m_pMover;
	CModelObject	*pModel = (CModelObject *)pMover->m_pModel;
	
	switch( dwState )
	{
	case OBJSTA_ATK_MAGIC1:
		if( pModel->IsEndFrame() )
			ResetState( OBJSTA_ATK_ALL );

		if( pModel->IsAttrHit() )			// 현재 프레임에 타격속성이 있는가? 
		{
			pMover->OnAttackMagic();		// 무버의 완드공격 핸들러를 호출.
		}
		break;
	}
}


//
//		원거리공격동작
//
void	CActionMover::ProcessStateAttackRange( DWORD dwState, int nParam )
{
	CMover* pMover = m_pMover;
	CModelObject	*pModel = (CModelObject *)pMover->m_pModel;
	
	switch( dwState )
	{
	case OBJSTA_ATK_RANGE1:
		// 동작이 끝나면 상태 해제
		if( pModel->IsEndFrame() )
		{
			ResetState( OBJSTA_ATK_ALL );
		}
		if( pModel->IsAttrHit() )			// 현재 프레임에 타격속성이 있는가? 
		{
			pMover->m_dwFlag |= MVRF_HITCONT;	
			CMover* pHitObj	= prj.GetMover( m_idTarget );

			if( IsValidObj( (CObj*)pHitObj ) && pHitObj->IsLive() )
				pMover->OnAttackRange();
		}
		break;
	}

}
