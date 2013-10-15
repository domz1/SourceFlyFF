#include "stdafx.h"
#include "ActionMover.h"
#include "DefineObj.h"


BOOL CanCollisionFly( CMover* pMover )
{
#ifdef __CLIENT
	if( CObj::m_bCollision && (pMover->IsActiveMover() || pMover->IsEmptyDest()) )
		return TRUE;
#else
	if( pMover->IsNPC() || pMover->IsEmptyDest() )
		return TRUE;
#endif
	return FALSE;
}

//
//
//
BOOL CActionMover::ProcessCollisionFly( D3DXVECTOR3 *pPos )
{
	if( m_fAccPower == 0.0f && m_vDeltaAccu == D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) )	// 정지상태?
		return FALSE;

	CMover*		pMover = m_pMover;
	D3DXVECTOR3	vStart = *pPos;
	D3DXVECTOR3 vDelta = m_vDeltaAccu;
	CWorld*		pWorld = GetWorld();

	pMover->SetIAObjLink( NULL );
	vStart.y += 0.5f;				// vStart 지면의 좌표 + 0.5 = 빗자루 높이
	
	BOOL fResult = FALSE;
	if( CanCollisionFly( pMover) )
	{
		D3DXVECTOR3 vDir = m_vDeltaAccu;
	#ifdef __WORLDSERVER
		vDir *= 0.25f;
	#endif
		float fLen = D3DXVec3Length( &vDir );		// 진행방향 벡터의 길이를 일단 구함.
		if( fLen > 0.001f )
		{
			FLOAT fR = 1.0f;
			if( fLen < 0.65f )							// 이 길이가 0.65f 보다도 짧으면
			{
				fR = 0.65f / fLen;						// 몇배나 작은지 계산해서
				vDir *= fR;								// 벡터를 그만큼 늘여줌.
			}
			fResult	= pWorld->ProcessCollisionReflection( &vDelta, vStart, vDir, 0 );
			if( fResult )	
			{
				vDelta /= fR;
				vDelta *= 0.7f;
				m_vDelta = vDelta;
			}
		}
	}

	// 1. y축 더함
	pPos->y += vDelta.y;

	// 2. x축 더함
	pPos->x += vDelta.x;
	pWorld->ClipX( pPos->x );
	int nAttr = pWorld->GetHeightAttribute( pPos->x, pPos->z );		// 이동할 위치의 속성 읽음.
	switch( nAttr )
	{
	case HATTR_NOMOVE:	// 이동 금지.
		pPos->x -= vDelta.x;		// 이동하려는 곳이 이동 금지 구역이면 좌표를 다시 뺌
		break;
	}

	// 3. z축 더함
	pPos->z += vDelta.z;
	pWorld->ClipZ( pPos->z );
	if( pWorld->m_bFly )
		nAttr = pWorld->GetHeightAttribute( pPos->x, pPos->z );		
	else
		nAttr = HATTR_NOFLY;

	switch( nAttr )
	{
	case HATTR_NOMOVE:	// 이동 금지.
		pPos->z -= vDelta.z;		// 이동하려는 곳이 이동 금지 구역이면 좌표를 다시 뺌
		break;
	case HATTR_DIE:		// 이영역에 들어가면 죽음.
		pMover->DoDie( NULL );
		break;
	case HATTR_NOFLY:	// 비행 금지 - 자동으로 비행모드 풀림.
		pPos->x -= vDelta.x;
		pPos->z -= vDelta.z;		// 이동하려는 곳이 이동 금지 구역이면 좌표를 다시 뺌
		pPos->y -= vDelta.y;
		pMover->UnequipRide();
		break;
	}

	m_fCurrentHeight = pWorld->GetFullHeight( D3DXVECTOR3(pPos->x, pPos->y + 1.0f, pPos->z) );

	if( pPos->y > pWorld->m_fMaxHeight )
		pPos->y = pWorld->m_fMaxHeight;
	else
	{
		LPWATERHEIGHT pWaterHeight = pWorld->GetWaterHeight( *pPos );
		if( pWaterHeight && pWaterHeight->byWaterTexture == WTYPE_CLOUD && m_fCurrentHeight == 0.0f )	
		{
			if( pPos->y < pWorld->m_fMinHeight )		
				pPos->y = pWorld->m_fMinHeight;
		}
		else
		{
			if( pPos->y < m_fCurrentHeight )		
			{
				pPos->y = m_fCurrentHeight;
				fResult = TRUE;
			}
		}
	}

	return fResult;
}


//
//
//
BOOL CActionMover::ProcessCollisionGround( D3DXVECTOR3 *pPos )
{
	CMover*			pMover = m_pMover;
	CModelObject	*pModel = (CModelObject *)pMover->m_pModel;
	D3DXVECTOR3		vDelta = m_vDeltaAccu;
	BOOL			fResult = FALSE;
	CWorld*			pWorld = GetWorld();

	if( m_bMove )	
	{
		D3DXVECTOR3	vKneePos, vDeltaXZ ;
		
		vKneePos = *pPos;	
		vKneePos.y += 0.4f;		// 무릎높이
		vDeltaXZ = vDelta;	
		vDeltaXZ.y = 0;			// 이동벡터에서 수평이동 성분만 취함


		//gmpbigsun : collision test-------------------------------------------------------------------------------
		float COLL_TEST_DIR = 6.5f;

#ifdef __BS_ADJUST_COLLISION

		if( pMover == g_pPlayer )		//주인공일경우 
		{
			COLL_TEST_DIR = 30.0f;

			DWORD dwMoveState = GetMoveState( );
			if( OBJSTA_FMOVE == dwMoveState )
			{
				if( !IsStateFlag( OBJSTAF_WALK ) ) 
					COLL_TEST_DIR = 8.0f;

			}

		}
#endif

		vDeltaXZ *= COLL_TEST_DIR;
	#ifdef __X15
		vDeltaXZ /= 4.0f;		// 서버가 15프레임으로 돌때는 vDeltaXZ도 4배로 커진다. 그래서 줄여줘야 함.
	#endif
	
		// 현재위치와 이동벡터를 넘겨주면 
		// 배경과의 충돌여부를 검사해서 바뀐 이동벡터를 계산한다.
		BOOL bColl	= TRUE;
		if( pMover->IsNPC() && (pMover->GetIndex() >= MI_PET_LAWOLF && pMover->GetIndex() <= MI_PET_LASTINDEX) )
		{
			bColl = FALSE;		// 변신구를 사용하지 않은 순수 펫은 오브젝트와 충돌검사 안함.
		}

#if __VER >= 9	//__AI_0509
		if( pMover->IsNPC() && ( pMover->GetSpeedFactor() > 1.9F || pMover->m_dwAIInterface == AII_PET ) )	// 귀환중이면
		{
			// 넉백같은 힘이 가해진 상황에는 충돌체크 진행
			if( pMover->m_pActMover->m_vDeltaE.x == 0 && pMover->m_pActMover->m_vDeltaE.y == 0 && pMover->m_pActMover->m_vDeltaE.z == 0 )
				bColl = FALSE;
		}
#endif	// __AI_0509
#ifdef __CLIENT
#if __VER >= 9	// __PET_0410
		if( pMover->GetId() == NULL_ID )	// 클라이언트에서만 생성한 객체면,
			bColl	= FALSE;
#endif	// __PET_0410
#endif	// __CLIENT

		bColl = TRUE;

		if( CObj::m_bCollision && bColl ) 
		{
			float fLen = D3DXVec3Length( &vDeltaXZ );		// 진행방향 벡터의 길이를 일단 구함.
			if( fLen != 0.0f )
			{	/*
				FLOAT fR = 1.0f;
				if( fLen < 0.65f )							// 이 길이가 0.65f 보다도 짧으면
				{
					fR = 0.65f / fLen;						// 몇배나 작은지 계산해서
					vDeltaXZ *= fR;							// 벡터를 그만큼 늘여줌.
				}*/
#ifdef __LAYER_1015
				fResult	= pWorld->ProcessCollision( &vDeltaXZ, vKneePos, vDeltaXZ, 0, pMover->GetLayer() );
#else	// __LAYER_1015
				fResult	= pWorld->ProcessCollision( &vDeltaXZ, vKneePos, vDeltaXZ, 0 );
#endif	// __LAYER_1015
				//vDeltaXZ /= fR;								// 충돌계산이 끝난후엔 다시 벡터 길이 원상복귀.
			}
		}

	#ifdef __X15
		vDeltaXZ *= 4.0f;	
	#endif
		vDeltaXZ /= COLL_TEST_DIR;

#ifdef __BS_CONSOLE
		if( pMover == g_pPlayer )
			vDeltaXZ *= gConsole()->_fSpeedMul;
#endif

	#if __VER >= 11 // __FIX_COLLISION
		if(fResult && (D3DXVec3Length(&vDeltaXZ) > 0.15f))
		{
			vDeltaXZ.y = m_vDeltaAccu.y = m_vDelta.y = 0;
			m_vDelta.x = vDeltaXZ.x;
			m_vDelta.z = vDeltaXZ.z;
		}
	#endif
		BOOL bCollision = FALSE;

		// x축 더하기
		pPos->x += vDeltaXZ.x;
		pWorld->ClipX( pPos->x );
		int nAttr = pWorld->GetHeightAttribute( pPos->x, pPos->z );		// 이동할 위치의 속성 읽음.
		switch( nAttr )
		{
		case HATTR_NOWALK:	// 걷기 금지
		case HATTR_NOMOVE:	// 이동 금지.
			if( pMover->GetIAObjLink() == NULL )	// 배를 타고 있을때는 지상에서의 속성이 무시된다.
			{
				pPos->x -= vDeltaXZ.x;		// 이동하려는 곳이 이동 금지 구역이면 좌표를 다시 뺌
				bCollision = TRUE;
			}
			break;
		}

		// z축 더하기
		pPos->z += vDeltaXZ.z;
		pWorld->ClipZ( pPos->z );	
		nAttr = pWorld->GetHeightAttribute( pPos->x, pPos->z );		// 이동할 위치의 속성 읽음.
		switch( nAttr )	
		{
		case HATTR_NOWALK:	// 걷기 금지
		case HATTR_NOMOVE:	// 이동 금지.
			if( pMover->GetIAObjLink() == NULL )
			{
				pPos->z -= vDeltaXZ.z;
				bCollision = TRUE;
			}
			break;
		case HATTR_DIE:		// 이영역에 들어가면 죽음. - die는 x, z가 다 더해지고 난후에 검사해야 한다.
			pMover->DoDie( NULL );
			break;
		}
		
	#ifdef __WORLDSERVER
		if( bCollision )
			pMover->PostAIMsg( AIMSG_COLLISION );
	#endif
	} // if( m_bMove )

	// 좌표 더해준후 보정(후처리)
	if( GetDmgState() == OBJSTA_DISAPPEAR )		// 죽어 사라지는 중엔 지형충돌검사 안함.
		return fResult;

	if( vDelta.y < 0.0f )		// 좌표가 내려갔음
	{
		if( m_bGround == 1 )	// 바닥에 그냥 서있는 상태(m_bGround == 1)는 추가적으로 충돌검사 할필요 없다.
			return fResult;

		// 캐릭터 머리에서 아래로 라인을 내리고 걸리는게 있으면 적당한 값으로 슬라이드 계산후 적용.
		// 걸리는게 없다면 공중에 떠있는것이므로 pPos->y += delta.y만 해주고 h값으로 계산.
		// 단 슬라이드 계산할때는 면이 일정각이상으로 기울어져 있을때만 적용. 안그러면 모든 경사면은 다 미끄러져 못올라간다.
		// 슬라이드를 더할때는 실제로 바닥에 닿았을때만 더해준다.

		D3DXVECTOR3 vSlide( 0.0f, 0.0f, 0.0f );		// slide벡터
		CObj *pObjColl = NULL;
		FLOAT h = pWorld->ProcessUnderCollision( &vSlide, &pObjColl, D3DXVECTOR3( pPos->x, pPos->y + 1.0f, pPos->z ) );
		m_fCurrentHeight = h;

		pPos->y += vDelta.y;
		
		BOOL bLanding = FALSE;
		if( GetJumpState() == OBJSTA_SJUMP3 )	// 하강동작 상태였을때
		{
 			if( pPos->y < h )	// 바닥에 닿으면
				bLanding = TRUE;
		} 
		else	
		{
			// 그냥 좌표만 내려가고 있던상태( 비탈길을 내려간다거나.  걸어다가 빠졌다거나)
			if( pPos->y - 0.25f > h )		// 발바닥 아래 xx지점을 더 내려서 검사했는데도 바닥보다 높다면 
			{
				if( vDelta.y < -0.1f )		// 떨어지는 속도가 0.x보다 클때만 하강동작.  살짝 떨어지는건 걍 좌표만 내려감.
				{
					#if __VER >= 11 // __SYS_COLLECTING
						if( (GetState() & OBJSTA_COLLECT)== 0 )
					#endif

#if __VER >= 15 // __BS_FIXED_KNOCKBACK
						if( pMover->IsPlayer() && !( GetDmgState() & OBJSTA_DMG_FLY_ALL ) )			//몬스터는 하강 애니 없음 
#endif
					SetJumpState( OBJSTA_SJUMP3 );		// 하강 상태로 전환

					pMover->OnActDrop();				// 이벤트 호출
				}
			} 
			else				// 발아래 0.25지점내에 바닥이 있다면 그냥 발바닥 붙여줌
				bLanding = TRUE;
		} 
		
		if( bLanding )			// 착지? 
		{
#ifdef __JEFF_11_6
			BOOL bReturn	= pMover->IsNPC() && pMover->GetSpeedFactor() > 1.9F;
#endif	// __JEFF_11_6
			if( vSlide.y
#ifdef __JEFF_11_6
				&& !bReturn
#endif	// __JEFF_11_6
				)
			{
				*pPos = vSlide;
				m_bGround = 2;	// 땅에 닿긴 했는데 미끄러 지고 있다.
			#ifdef __WORLDSERVER
				pMover->PostAIMsg( AIMSG_COLLISION );
			#endif
			} 
			else
			{
				pPos->y = h;
				m_bGround = 1;	// 미끄러 지지 않고 있다.
			}

			if( pObjColl )
			{
				if( pObjColl->GetType()	== OT_SHIP )	// 착지한 오브젝트가 SHIP이면
					pMover->OnActIALanding( pObjColl, *pPos );
				else
					pMover->SetIAObjLink( NULL );
			} 
			else
				pMover->SetIAObjLink( NULL );	// 착지 했는데 오브젝트가 아니면 IA에서 내린걸로 간주.
			
			m_vDelta.y = 0;
		}
	} 
	else if( vDelta.y > 0 )			// 좌표가 올라갔음
	{
#if __VER >= 15 // __BOUND_BOX_COLLISION
		FLOAT h = 65535.0f;			// pPos위치의 천장 높이
		if( pMover->IsPlayer() )
		{
			h = pWorld->GetOverHeightForPlayer( D3DXVECTOR3( pPos->x, pPos->y + 1.5f, pPos->z ), pMover );
		}
		else
		{
			h = pWorld->GetOverHeight( D3DXVECTOR3( pPos->x, pPos->y + 1.5f, pPos->z ), pMover );
		}
#else // __BOUND_BOX_COLLISION
		// pPos위치의 천장 높이
		FLOAT h	= pWorld->GetOverHeight( D3DXVECTOR3( pPos->x, pPos->y + 1.5f, pPos->z ) );
#endif // __BOUND_BOX_COLLISION
			
		pPos->y += vDelta.y;		// 운동량을 더해줌
		if( pPos->y + 1.5f > h )	// 머리 부분이 천장에 닿았으면 
		{
			pPos->y = h - 1.5f ;	// y좌표 보정
			m_vDelta.y = -0.01f;	// 올라가던거 멈춤
			fResult = TRUE;			// 충돌 
		}
	}
	
	return fResult;
}

