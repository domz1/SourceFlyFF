#include "stdafx.h"
#include "defineObj.h"
#include "ActionMover.h"
#include "..\_Common\ParticleMng.h"
#ifdef __CLIENT
#include "..\_Common\TailEffectMng.h"
#include "DPClient.h"
#endif
#ifdef __WORLDSERVER
#include "User.h"
#include "DPCoreClient.h"
#include "DPDatabaseClient.h"
#include "DPSrvr.h"
#endif


const float FRIC_AIR = 0.011f;		// 비행중 마찰계수


#ifdef __CLIENT
extern	CDPClient	g_DPlay;
extern int			g_nFlySpeed;
extern int			g_nDrift;
#endif // CLIENT

#ifdef __WORLDSERVER
extern	CUserMng			g_UserMng;
extern	CDPCoreClient		g_DPCoreClient;
extern	CDPDatabaseClient	g_dpDBClient;
extern	CDPSrvr				g_DPSrvr;
#endif // __WORLDSERVER

#ifdef __CLIENT
void CActionMover::PresupposePos2( D3DXVECTOR3* pv, D3DXVECTOR3* pvd, float* pf, float* pfAngleX, float* pfAccPower, u_long uTickCount )
{
	CMover* pMover	= GetMover();
	D3DXVECTOR3 v	= pMover->GetPos();
	float fAngle	= pMover->GetAngle();
	D3DXVECTOR3 vDelta	= m_vDelta;
	
	if( prj.GetItemProp( pMover->m_dwRideItemIdx ) == NULL )
		return;

	float fAcc = prj.GetItemProp( pMover->m_dwRideItemIdx )->fFlightSpeed;	// 무버가 타고있는 아이템의 인덱스에서 추진력을 꺼내옴.

	fAcc	*= 0.75f;
	float fTheta, fThetaX, d;

	DWORD dwMoveState	= GetMoveState();
	DWORD dwTurnState	= GetTurnState();
	DWORD dwLookState	= GetLookState();

	float fX = pMover->GetAngleX();
	float fAccPower	= m_fAccPower;
	float fTurnAngle;
	
	D3DXVECTOR3	vAcc;
	for( u_long i = 0; i < uTickCount; i++ )
	{
		fTheta	= D3DXToRadian( fAngle );
		switch( dwMoveState )
		{
			case OBJSTA_STAND:
				fAccPower	= 0;
				break;
			case OBJSTA_FMOVE:
				fAccPower	= fAcc;
				break;
		}
		switch( dwTurnState )
		{
			case OBJSTA_LTURN:
				fTurnAngle	= m_fTurnAngle;
				if( ( GetStateFlag() & OBJSTAF_ACC ) == 0 )
					fTurnAngle *= 2.5f;
				fAngle	+= fTurnAngle;
				if( fAngle > 360.0f )
					fAngle -= 360.0f;
				break;
			case OBJSTA_RTURN:
				fTurnAngle	= m_fTurnAngle;
				if( ( GetStateFlag() & OBJSTAF_ACC ) == 0 )
					fTurnAngle *= 2.5f;
				fAngle	-= fTurnAngle;
				if( fAngle < 0.0f )
					fAngle	+= 360.0f;
				break;
		}
		switch( dwLookState )
		{
			case OBJSTA_LOOKUP:
				if( fX > -45.0f )	fX	-= 1.0f;
				break;
			case OBJSTA_LOOKDOWN:
				if( fX < 45.0f )	fX	+= 1.0f;
				break;
		}

		fThetaX	= D3DXToRadian( fX );
		if( GetStateFlag() & OBJSTAF_TURBO )
			fAccPower	*= 1.5f;
		d	= cosf( fThetaX ) * fAccPower;
		fTheta	= D3DXToRadian( fAngle );
		
		vAcc.x	= sinf( fTheta ) * d;
		vAcc.z	= -cosf( fTheta ) * d;
		vAcc.y	= -sinf( fThetaX ) * fAccPower;

		float fLenSq	= D3DXVec3LengthSq( &vDelta );
		D3DXVECTOR3 vDeltaNorm, vAccNorm;
		D3DXVec3Normalize( &vDeltaNorm, &vDelta );
		D3DXVec3Normalize( &vAccNorm, &vAcc );
		float fDot	= D3DXVec3Dot( &vDeltaNorm, &vAccNorm );
		if( fAccPower > 0.0f )
		{
			if( fLenSq > 0.01f )
			{
				if( fDot < cosf( 70.0f ) )
				{
					vAcc	*= 2.0f;
					vDelta	*= 0.985f;
				}
			}
		}
		fLenSq	= D3DXVec3Length( &vDelta );
		if( fLenSq < 0.3f )
		{
			vDelta	+= vAcc;
		}
		vDelta	*= ( 1.0f - FRIC_AIR );
		v	+= vDelta;
		if( v.y > MAX_MOVE_HEIGHT )
			v.y	= MAX_MOVE_HEIGHT;

		CWorld* pWorld = GetWorld();
		pWorld->ClipX( v.x );
		pWorld->ClipZ( v.z );
	}

	*pv	        = v;
	*pf	        = fAngle;
	*pvd	    = vDelta;
	*pfAngleX	= fX;
	*pfAccPower	= fAccPower;

	pMover->m_nCorr = (int)uTickCount;
}

void CreateFlyParticle( CMover *pMover, float fAngX, int nType )
{
	FLOAT fAngXZ, fAngH, fDist, fSpeed;
	D3DXVECTOR3 vVel;
	D3DXVECTOR3 vPos = pMover->GetPos();
	if( nType == 0 )
		vPos.y += 0.8f;
	
	fAngXZ = pMover->GetAngle();
	fAngXZ += 180.0f; 
	fAngXZ = D3DXToRadian( fAngXZ );
	
	vPos.x += (xRandom(100) / 100.0f) - 0.50f;
	vPos.y += (xRandom(100) / 100.0f) - 0.50f;
	vPos.z += (xRandom(100) / 100.0f) - 0.50f;
	
	for( int i = 0; i < 2; i ++ )
	{
		fAngXZ = (float)(-45.0f + xRandomF(90));
		fAngXZ += pMover->GetAngle();
		fAngXZ += 180.0f;
		
		fAngXZ = D3DXToRadian( fAngXZ );
		fAngH = (float)(fAngX + xRandomF(10));
		fAngH = D3DXToRadian( fAngH );
		fSpeed = xRandomF(0.02f);
		
		fDist = cosf(-fAngH) * fSpeed;
		vVel.x = sinf(fAngXZ) * fDist;
		vVel.z = -cosf(fAngXZ) * fDist;
		vVel.y = -sinf(-fAngH) * fSpeed;
		if( nType == 1 ) // 근두운용.
			g_ParticleMng.CreateParticle( 10 + xRandom(3), vPos, vVel, CMover::GetActiveMover()->GetPos().y + 0.5f );
		else
			g_ParticleMng.CreateParticle( 2 + xRandom(3), vPos, vVel, CMover::GetActiveMover()->GetPos().y + 0.5f );
	}
	
}

void CActionMover::ProcessFlyParticle( float fLenSq )
{
	CMover *pMover = m_pMover;

	// 운영자 투명모드때는 파티클 안나옴.
	if( (pMover->IsMode( TRANSPARENT_MODE ) ) == 0 )
	{
		ItemProp* pRideProp = prj.GetItemProp( m_pMover->m_dwRideItemIdx );		// 현재 타고있는 탈것의 프로퍼티.
		// 비행 파티클 처리.
		if( fLenSq > 0.01f )
		{
			if( (pMover->IsActiveMover() && g_Neuz.m_camera.m_fZoom > 2.0f) || pMover->IsActiveMover() == FALSE )		// 일정속도 이상이 되면 꼬리에 파티클이 나오기 시작.
			{
				int nType = 0;
				if( pRideProp && pRideProp->dwID == II_RID_RID_BOR_RIDINGCLOUD )
					nType = 1;
				CreateFlyParticle( pMover, pMover->GetAngleX(), nType );
			}
		}
		
		if( fLenSq > 0.001f  )
		{
			if( (pMover->IsActiveMover() && g_Neuz.m_camera.m_fZoom > 1.0f) || pMover->IsActiveMover() == FALSE )
			{
				if( pRideProp && pRideProp->dwItemKind3 == IK3_BOARD )		// 보드만 꼬리가 나온다.
				{
					if( m_pTail )
					{
						if( pRideProp->dwID == II_RID_RID_BOR_RIDINGCLOUD ) // 근두운일때
						{
							if( m_pTail->GetType() != 2 )	// 생성되었던 꼬리고 근두운용이 아니면
								m_pTail->ChangeTexture( D3DDEVICE, "etc_Tail2.bmp", 2 );
						} else
						{
							if( m_pTail->GetType() != 1 )	// 생성되었던 꼬리고 일반보드용이 아니면 
								m_pTail->ChangeTexture( D3DDEVICE, "etc_Tail1.bmp", 1 );	// 일반보드용으로 텍스쳐 교체.
						}

							
					}
					if( m_pTail == NULL )	// 아직 할당 안됐으면 할당하고.
					{
						if( pRideProp->dwID == II_RID_RID_BOR_RIDINGCLOUD ) // 근두운...
						{
							m_pTail = (CTailEffectBelt*)g_TailEffectMng.AddEffect( g_Neuz.m_pd3dDevice, "etc_Tail2.bmp", 2 );
						}
						else
						{
							m_pTail = (CTailEffectBelt*)g_TailEffectMng.AddEffect( g_Neuz.m_pd3dDevice, "etc_Tail1.bmp", 1 );
						}
					}
					D3DXVECTOR3	vPos1, vPos2;
					D3DXVECTOR3	vLocal;
					FLOAT		fAngXZ = pMover->GetAngle();
					FLOAT		fAngH  = pMover->GetAngleX();
					
					AngleToVectorXZ( &vLocal, fAngXZ, -1.0f );

					fAngXZ -= 90.0f;
					if( fAngXZ < 0 )
						fAngXZ += 360.0f;
					AngleToVector( &vPos1, fAngXZ, -fAngH, 0.5f );
					vPos1 += pMover->GetPos();

					vPos1 += vLocal;
				
					fAngXZ = pMover->GetAngle();
					fAngH  = pMover->GetAngleX();
					
					AngleToVectorXZ( &vLocal, fAngXZ, -1.0f );

					fAngXZ += 90.0f;
					if( fAngXZ > 360.0f )
						fAngXZ -= 360.0f;
					AngleToVector( &vPos2, fAngXZ, -fAngH, 0.5f );
					vPos2 += pMover->GetPos();
					vPos2 += vLocal;
						
					if( m_pTail )
						m_pTail->CreateTail( vPos1, vPos2 );
				}
			}
		}
	} // 투명모드가 아닐때
}		

void CActionMover::ProcessFlyTracking()
{
	CMover* pMover = m_pMover;

	// 자동 추적 모드. g_pPlayer만 실행된다.
	if( pMover->IsActiveMover() && (pMover->m_dwFlag & MVRF_TRACKING) )
	{
		static float s_fTurnAcc = 0, s_fTurnAccH = 0;
		{
			CMover *pTarget = prj.GetMover( pMover->m_idTracking );		// 추적할 목표.
			if( pTarget )
			{
				D3DXVECTOR3	vDist = pTarget->GetPos() - pMover->GetPos();		// 나를 원점으로 타겟까지의 벡터.
				FLOAT fAngXZ, fAngH;
				
				xGetDegree( &fAngXZ, &fAngH, vDist );		// 타겟과의 각도 구함.
				// 남쪽이 0도 기준. 시계방향 -180까지 시계반대방향 +180
				// 3도 이하는 무시.
				FLOAT fMoverAng = pMover->GetAngle();
				if( fMoverAng > 180.0f )	// 계산하기 좋게 좌표계를 +,- 로 바꿈.
					fMoverAng -= 360.0f;
				FLOAT fSubAng = fAngXZ - fMoverAng;
				if( fSubAng > 180.0f )
					fSubAng -= 360.0f;
				else if( fSubAng < -180.0f )
					fSubAng += 360.0f;

			#ifdef _DEBUG 
			#ifdef __XUZHU				
				extern float _g_fReg[];
				_g_fReg[1] = fSubAng;
				_g_fReg[2] = fAngXZ;
			#endif
			#endif
				
				if( fSubAng < -3.0f )		// 오른쪽으로 돌아야 한다.
					s_fTurnAcc = -2.5f;
				else 
				if( fSubAng > 3.0f )	// 왼쪽으로 돌아야 한다.
					s_fTurnAcc = 2.5f;
				else
					s_fTurnAcc = 0;

				FLOAT fMoverAngX = pMover->GetAngleX();
				FLOAT fSubAngH = fAngH - fMoverAngX;
				if( fSubAngH > 180.0f )
					fSubAngH -= 360.0f;
				else if( fSubAngH < -180.0f )
					fSubAngH += 360.0f;

				if( fSubAngH < -3.0f )
					s_fTurnAccH = -1.5f;
				else
				if( fSubAngH > 3.0f )
					s_fTurnAccH = 1.5f;
				else
					s_fTurnAccH = 0;

				pMover->SetAngle( pMover->GetAngle() + s_fTurnAcc );
				pMover->SetAngleX( pMover->GetAngleX() + s_fTurnAccH );

				if( s_fTurnAcc || s_fTurnAccH )	// 값이 달라지면 전송함.
					g_DPlay.PostPlayerAngle( TRUE );
			}
		}
	}
}
#endif // __CLIENT

void	CActionMover::ProcessFlyMove( void )
{
#ifdef __CLIENT	
	g_nDrift = 0;									// 드리프트 플랙 클리어 
#endif
	float fLenSq = D3DXVec3LengthSq( &m_vDelta );
	if( fLenSq == 0.0f && (GetStateFlag() & OBJSTAF_ACC ) == 0 )	  
		return;										// 멈춤 상태면 리턴 

	CMover* pMover = m_pMover;
	FLOAT fAccPwr = m_fAccPower;

#ifdef __CLIENT
	ProcessFlyTracking();
#endif // client
	
	// 터보모드 처리 
	if( (GetStateFlag() & OBJSTAF_TURBO) && (GetStateFlag() & OBJSTAF_ACC) )		// 터보모드 & 전진중
	{
	#ifdef __WORLDSERVER
		pMover->m_tmAccFuel	=  (int)( pMover->m_tmAccFuel - (1000.0f / (float)FRAME_PER_SEC) );		// 1/60만큼 깎음
	#endif
		if( pMover->m_tmAccFuel <= 0 )				// 가속연료가 다 떨어지면
		{
			pMover->m_tmAccFuel = 0;
			SendActMsg( OBJMSG_MODE_TURBO_OFF );	// 터보모드 중지 
		#ifdef __WORLDSERVER
			g_UserMng.AddSendActMsg( pMover, OBJMSG_MODE_TURBO_OFF );
		#endif
		}
		else
			fAccPwr *= 1.2f;						// 가속연료가 남아있다면 터보모드
	}

#ifdef __CLIENT
	ProcessFlyParticle( fLenSq );
#endif

	// 관성처리 
	if( fAccPwr > 0.0f )
	{
		// 힘벡터 생성
		FLOAT fAngX  = D3DXToRadian( pMover->GetAngleX() );		
		FLOAT fAng   = D3DXToRadian( pMover->GetAngle() );
		FLOAT fDist  = cosf(fAngX) * fAccPwr;		

		D3DXVECTOR3	vAcc;
		vAcc.x = sinf( fAng ) * fDist;
		vAcc.z = -cosf( fAng ) * fDist;
		vAcc.y = -sinf( fAngX ) * fAccPwr;

		// 관성벡터와 추진력벡터가 각도가 50도 이하면 급추진
		if( fLenSq > 0.01f )
		{
			D3DXVECTOR3 vDeltaNorm, vAccNorm;
			D3DXVec3Normalize( &vDeltaNorm, &m_vDelta );
			D3DXVec3Normalize( &vAccNorm, &vAcc );
			float fDot = D3DXVec3Dot( &vDeltaNorm, &vAccNorm );	

			if( fDot < 0.633319f )					// 이전코드: cosf(70.0f)  값으로는 대략 50도 
			{
				vAcc *= 2.0f;						
				m_vDelta *= 0.985f;
			#ifdef __CLIENT
				g_nDrift = 1;
				
				if( ! ( pMover->IsMode( TRANSPARENT_MODE ) ) )		// 투명상태가 아닐때만 렌더.
				{	
					if( (g_nProcessCnt & 3) == 0 )
						CreateSfx( g_Neuz.m_pd3dDevice, XI_NAT_DUST_RUN, pMover->GetPos() );
				}
			#endif
			}
		}
			
		fLenSq = D3DXVec3LengthSq( &m_vDelta );		// 1/60 sec 속도
		float fMaxSpeed = 0.3f;
		if( GetStateFlag() & OBJSTAF_TURBO )		// 터보모드에선 MAX속도가 1.1배
			fMaxSpeed *= 1.1f;
			
		if( fLenSq < (fMaxSpeed * fMaxSpeed) )		// 일정이상 속도를 넘지 않게 하자.
			m_vDelta += vAcc;						// 관성벡터 += 추진력벡터
	}

	m_vDelta *= (1.0f - FRIC_AIR);					// 마찰력에 의한 감소  

	// raiders - 수치적 안정성을 위해서 적은 수치가 계산되는 것을 피한다.
	fLenSq = D3DXVec3LengthSq( &m_vDelta );
	if( m_fAccPower == 0.0f && fLenSq < 0.0002f * 0.0002f )		
	{
		fLenSq = 0;
		m_vDelta = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );		
		RemoveStateFlag( OBJSTAF_ACC );				// 가속상태 해제 
	}

#ifdef __CLIENT
	if( pMover->IsActiveMover() )
	{
		g_nFlySpeed = (int)( (sqrt(fLenSq) * 60.0f) * 60.0f * 60.0f );
		g_nFlySpeed	= (int)( g_nFlySpeed / 200.0f );
	}
#endif
}

//
//	State process
//	가급적 이곳에선 pMover->SetMotion을 하지 말것
//	빗자루 비행모드
//
void	CActionMover::ProcessState2( CMover* pMover, DWORD dwState, float fSpeed )
{
	int nParam = 0;
	float fAng = pMover->GetAngleX();
	CModelObject* pModel = (CModelObject*)pMover->m_pModel;

	ItemProp* pItemProp = prj.GetItemProp( m_pMover->m_dwRideItemIdx );
	if( !pItemProp )
	{
		Error( "ProcessState2(): %s, m_dwRideItemIdx == %d", m_pMover->m_szName, m_pMover->m_dwRideItemIdx );
		return;
	}

	switch( dwState )
	{
	// 제자리 대기 / 정지
	case OBJSTA_STAND:
		m_fAccPower = 0;		// 대기/정지상태에선 힘을 더이상 가하지 않는다.
		if( GetState() & OBJSTA_ATK_ALL )	break;
		if( GetState() & OBJSTA_TURN_ALL )	break;
		if( GetState() & OBJSTA_DMG_ALL )	break;
		{
			if( pMover->SetMotion( pItemProp->dwUseMotion + MTA_FSTAND1, ANILOOP_LOOP, MOP_FIXED ) == TRUE )		// 대기상태
				if( pMover->m_pRide )	
					pMover->m_pRide->m_fFrameCurrent = 0;
		}
		break;
	// 전진
	case OBJSTA_FMOVE:
		// 무버가 타고있는 아이템의 인덱스에서 추진력을 꺼내옴.
		m_fAccPower = pItemProp->fFlightSpeed * 0.75f;

		if( GetState() & OBJSTA_ATK_ALL )	break;
		if( GetState() & OBJSTA_TURN_ALL )	break;
		{
			if( pMover->SetMotion( pItemProp->dwUseMotion + MTA_FRUNNING1, ANILOOP_LOOP, MOP_FIXED ) == TRUE )		// 대기상태
				if( pMover->m_pRide )		
					pMover->m_pRide->m_fFrameCurrent = 0;
		}
		break;
	// 좌/우 돌기
	case OBJSTA_LTURN:
		{
			float fTurnAng = m_fTurnAngle;
			if( (GetStateFlag() & OBJSTAF_ACC) == 0 )		// 가속상태가 아닐때는 2배로 빨리 돈다.
				fTurnAng *= 2.5f;
			pMover->AddAngle( fTurnAng );
		}
		if( IsActAttack() )	break;
		pMover->SetMotion( pItemProp->dwUseMotion + MTA_FLTURN1, ANILOOP_LOOP, MOP_FIXED );
		
		break;
	case OBJSTA_RTURN:
		{
			float fTurnAng = m_fTurnAngle;
			if( (GetStateFlag() & OBJSTAF_ACC) == 0 )		// 가속상태가 아닐때는 2배로 빨리 돈다.
				fTurnAng *= 2.5f;
			pMover->AddAngle( -fTurnAng );
		}
		if( IsActAttack() )	break;
		pMover->SetMotion( pItemProp->dwUseMotion + MTA_FRTURN1, ANILOOP_LOOP, MOP_FIXED );
		break;
	case OBJSTA_LOOKUP:
		if( fAng > -45.0f )
			pMover->AddAngleX( -pItemProp->fFlightTBAngle );
		break;
	case OBJSTA_LOOKDOWN:
		if( fAng < 45.0f )
			pMover->AddAngleX( pItemProp->fFlightTBAngle );
		break;
	//--------- 공격 ---------------------------
	case OBJSTA_ATK1:
	case OBJSTA_ATK2:
	case OBJSTA_ATK3:
	case OBJSTA_ATK4:
		_ProcessStateAttack2( dwState, nParam );
		break;
	case OBJSTA_ATK_MAGIC1:		// 원거리 완드 공격.
		ProcessStateAttackMagic( dwState, nParam );
		break;

	//----------- 데미지 -------------------------
	case OBJSTA_DMG:
		if( pModel->IsEndFrame() )	
			ResetState( OBJSTA_DMG_ALL );
		break;
	case OBJSTA_DEAD:	// 죽어 널부러진 상태
		#ifdef __WORLDSERVER
			if( !pMover->IsPlayer() ) 
			{
				m_nDeadCnt--;
				if( m_nDeadCnt < 0 )
				{
					SetState( OBJSTA_DMG_ALL, OBJSTA_DISAPPEAR );	// 사라지는 상태로 전환
					m_nCount = 0;
				}
			}
		#endif
		break;
	}
}

//
//		비행중 공격 액션 처리
//
void	CActionMover::_ProcessStateAttack2( DWORD dwState, int nParam )
{
	CMover* pMover = m_pMover;
	CModelObject	*pModel = (CModelObject *)pMover->m_pModel;
	
	switch( dwState )
	{
	case OBJSTA_ATK1:
		if( pModel->IsEndFrame() )
			ResetState( OBJSTA_ATK_ALL );

		if( pModel->m_nPause > 0 )
		{
			--pModel->m_nPause;
		} 
		else
		{
			if( pModel->IsAttrHit() )
			{
				CMover* pHitObj	= prj.GetMover( m_objidHit );
				if( IsInvalidObj( pHitObj ) || pHitObj->IsDie() )	// 타겟이 거시기하거나 죽었으면 취소
					return;

				BOOL bSuccess = pHitObj->m_pActMover->SendDamage( AF_GENERIC, pMover->GetId() );
				if( bSuccess == FALSE )
					return;
			#ifdef __CLIENT
				ItemProp* pItemProp = pMover->GetActiveHandItemProp();
				if( pItemProp )
				{
					D3DXVECTOR3 v = pMover->GetPos();
					PLAYSND( pItemProp->dwSndAttack1, &v );
				}
			#endif
				if( GetMover()->IsPlayer() )
				{
					pModel->m_nPause = 5;	// frame 멈춤
				}
				else
				{
					pModel->m_nPause = 0;	// 몬스터는 멈추지 않음
					pHitObj->m_pModel->m_nPause = 0;
				}
					
			}
		}
		break;
	}
}
