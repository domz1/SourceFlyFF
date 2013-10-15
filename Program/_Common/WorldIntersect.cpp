#include "stdafx.h"

#if __VER >= 13 // __HOUSING
#include "housing.h"
#endif // __HOUSING
#if __VER >= 15 // __GUILD_HOUSE
#include "GuildHouse.h"
#endif
//
//	vPos지점의 지형 높이를 계산.  지형, 배경오브젝트 모두 포함이다.
//
FLOAT CWorld::GetFullHeight( const D3DXVECTOR3& vPos )
{
	static D3DXVECTOR3 vDir( 0.0f, -2000.0f, 0.0f );	// 아래로 2000
	D3DXVECTOR3 vEnd = vPos + vDir;
	D3DXVECTOR3 vIntersect;
	FLOAT fDist, fDistMin = 4194304.0f;

	CObj* pObj;
	CObj* pMinObj = NULL;
	CModel* pModel;
	int nRange = 0;

	FOR_LINKMAP( this, vPos, pObj, nRange, CObj::linkStatic, nDefaultLayer )
	{
		// 레이(vPos-vDir)와 오브젝트OBB의 검사.  
		pModel = pObj->m_pModel;
		if( pModel->IsTouchOBB_Line( vPos, vEnd, pObj->GetMatrixWorld(), &vIntersect ) == TRUE )
		{
			vIntersect -= vPos;							// vPos로부터의 상대벡터
			fDist = D3DXVec3LengthSq( &vIntersect );	// 상대적인 거리
			if( fDist < fDistMin )						// 더 가까운놈이 있을지 모르니 가장 가까운 오브젝트를 개려낸다.
			{
				fDistMin = fDist;
				pMinObj = pObj;
			}
		}
	}
	END_LINKMAP
	
	if( pMinObj )	// 레이에 걸린 가장 가까운 오브젝트가 있는가	
	{
		pModel = pMinObj->m_pModel;
		// 레이와 삼각형으로 세밀 검사
	#ifdef __CPU_UTILDOWN_060502
		D3DXVECTOR3	*pTris[3];
		((CModelObject *)pModel)->GetObject3D()->FindTouchTriLine( pTris, vPos, vEnd, pMinObj->GetMatrixWorld(), &fDist );
		if( *pTris )
		{
			D3DXVECTOR3 v = vPos + fDist * vDir;
			return v.y;
		}
	#else
		D3DXVECTOR3 *pTri = pModel->IntersectRayTri( vPos, vDir, pMinObj->GetMatrixWorld(), &vIntersect, &fDist );
		if( pTri )
			return vIntersect.y;
	#endif
	}
	return GetLandHeight( vPos.x, vPos.z );
}

#if __VER >= 13 // __HOUSING
#ifdef __CLIENT
// 현재 캐릭터가 있는 위치의 맵에서 해당이름을 가진 오브젝트를 찾는다
CObj *CWorld::GetObjByName(TCHAR* ObjName)
{
	CObj* pObj;
	if(g_pPlayer)
	{
		FOR_LINKMAP( this, g_pPlayer->m_vPos, pObj, 0, CObj::linkStatic, nDefaultLayer )
		{
			char* pString = ((CModelObject *)pObj->m_pModel)->GetObject3D()->m_szFileName;
			if( strcmp(ObjName, pString) == 0 )
			{
				return pObj;
			}
		}
		END_LINKMAP
	}
	
	return NULL;
}

void CWorld::ForceTexture(LPDIRECT3DTEXTURE9 pNewTex)
{
	for( int i = 0; i < m_nLandWidth * m_nLandHeight; i++)
	{
		if(m_apLand[ i ]) m_apLand[ i ]->ForceTexture(pNewTex);
	}

}


BOOL	CWorld::ProcessObjCollision(D3DXVECTOR3 vPos, CObj* pTargetObj, CObj* pWallObj)
{
	// gmpbigsun: 본함수는 하우징 전용 충돌검출 함수임 
	CObj	*pObj	= NULL;

	if( TestOBBIntersect( &pWallObj->m_OBB, &pTargetObj->m_OBB ) )  // 벽 과 충돌이 실패라는것은 룸을 벗어났다는 ..
	{
		if( TestTriIntersect( pWallObj, pTargetObj ) )
			return TRUE;
	}
	else
		return TRUE;		// out of room

	// 오브젝트를 폴리곤 단위로 교차검사한다
	int nRange = OLD_MPU / m_iMPU;					// gmpbigsun:MPU가 4 이하일경우 그 범위수치가 보정되어야 함.
	FOR_LINKMAP( this, vPos, pObj, nRange, CObj::linkDynamic, 0 )
	{
		if( pObj->GetType() == OT_CTRL )
		{
#if __VER >= 15 // __GUILD_HOUSE
			if( ((CCtrl*)pObj)->GetId( ) != GuildHouse->m_dwSelectedObjID )		//gmpbigsun: 음 재설치의 경우 복사본이 원본과 충돌패스 
#endif
			{
				if(pObj != pTargetObj)
				{
					// 오브젝트 OBB끼리 검사
					if(TestOBBIntersect(&pObj->m_OBB, &pTargetObj->m_OBB))
						return TRUE;
				}
			}
		}
	}
	END_LINKMAP
	
// 	FOR_LINKMAP( this, vPos, pObj, nRange, CObj::linkStatic, 0)
// 	{
// 		if(pWallObj == pObj)
// 		{
// 			// pWallObj는 특수하게 벽전체를 감싸고 있는 박스이기때문에 충돌에 실패한다는 것은 룸에서 벗어난 곳이라는 의미
// 			if(TestOBBIntersect(&pObj->m_OBB, &pTargetObj->m_OBB))
// 			{
// 				if(TestTriIntersect(pObj, pTargetObj))
// 						return TRUE;	
// 			}
// 			else	
// 				return TRUE;
// 		}
// 	}
// 	END_LINKMAP

	return FALSE;
}

BOOL	CWorld::TestTriIntersect(CObj* pWallObj, CObj* pTargetObj)
{
	GMOBJECT* pTargetGm;

	if(((CModelObject*)pTargetObj->m_pModel)->GetObject3D()->m_CollObject.m_Type != GMT_ERROR)
	{
		// 충돌 오브젝트가 있으면 충돌 오브젝트로
		pTargetGm = &((CModelObject*)pTargetObj->m_pModel)->GetObject3D()->m_CollObject;
	}
	else
		pTargetGm = ((CModelObject*)pTargetObj->m_pModel)->GetObject3D()->GetGMOBJECT();
	
	if( ((CModelObject*)pWallObj->m_pModel)->GetObject3D()->SimpleTriIntersect(pWallObj->GetMatrixWorld(), pTargetGm, pTargetObj->GetMatrixWorld()))
		return TRUE;
	
	return FALSE;

}

BOOL	CWorld::TestOBBIntersect(BBOX* BoxA, BBOX* BoxB)
{
	float		fReal[3][3];
	float		ra, rb, t;
	long		i, k;
	D3DXVECTOR3 vDist = BoxB->Center - BoxA->Center;	// 두 박스간의 라인
	float		fTrans[3]; 

#if __VER >= 15 // __BS_BBOX_ABS_EXTENT
 	for( i = 0; i < 3; ++i )			//backface가 없는경우 Extent가 음수로 들어간다. Export할때 해결하는게...
 	{
		if( BoxA->Extent[i] < 0 )
 			BoxA->Extent[i] = -( BoxA->Extent[i] );
		if( BoxB->Extent[i] < 0 )
 			BoxB->Extent[i] = -( BoxB->Extent[i] );
 	}
#endif

	for( i = 0; i < 3; ++i)	fTrans[i] = D3DXVec3Dot(&vDist, &BoxA->Axis[i]);
	//회전 매트릭스 계산
	for( i=0 ; i<3 ; i++ )
		for( k=0 ; k<3 ; k++ )
		{
			fReal[i][k] = D3DXVec3Dot(&BoxA->Axis[i], &BoxB->Axis[k]); 
		}
	
	//A박스의 기본 벡터들 계산
	for( i=0 ; i<3 ; i++ )
	{
		ra = BoxA->Extent[i];
		rb = BoxB->Extent[0]*fabs(fReal[i][0]) + BoxB->Extent[1]*fabs(fReal[i][1]) + BoxB->Extent[2]*fabs(fReal[i][2]);
		t = fabs( fTrans[i] );
		if( t > ra + rb )	return FALSE;
	}

	//B박스의 기본 벡터들 계산
	for( k=0 ; k<3 ; k++ )
	{
		ra = BoxA->Extent[0]*fabs(fReal[0][k]) + BoxA->Extent[1]*fabs(fReal[1][k]) + BoxA->Extent[2]*fabs(fReal[2][k]);
		rb = BoxB->Extent[k];
		t = fabs( fTrans[0]*fReal[0][k] + fTrans[1]*fReal[1][k] + fTrans[2]*fReal[2][k] );
		if( t > ra + rb )	return FALSE;
	}

	//외적 구하기
	ra = BoxA->Extent[1]*fabs(fReal[2][0]) + BoxA->Extent[2]*fabs(fReal[1][0]);
	rb = BoxB->Extent[1]*fabs(fReal[0][2]) + BoxB->Extent[2]*fabs(fReal[0][1]);
	t = fabs( fTrans[2]*fReal[1][0] - fTrans[1]*fReal[2][0] );

	if( t > ra + rb )	return FALSE;

	//L = A0 x B1
	ra = BoxA->Extent[1]*fabs(fReal[2][1]) + BoxA->Extent[2]*fabs(fReal[1][1]);
	rb = BoxB->Extent[0]*fabs(fReal[0][2]) + BoxB->Extent[2]*fabs(fReal[0][0]);
	t = fabs( fTrans[2]*fReal[1][1] - fTrans[1]*fReal[2][1] );

	if( t > ra + rb )	return FALSE;

	//L = A0 x B2
	ra = BoxA->Extent[1]*fabs(fReal[2][2]) + BoxA->Extent[2]*fabs(fReal[1][2]);
	rb = BoxB->Extent[0]*fabs(fReal[0][1]) + BoxB->Extent[1]*fabs(fReal[0][0]);
	t = fabs( fTrans[2]*fReal[1][2] - fTrans[1]*fReal[2][2] );

	if( t > ra + rb )	return FALSE;

	//L = A1 x B0
	ra = BoxA->Extent[0]*fabs(fReal[2][0]) + BoxA->Extent[2]*fabs(fReal[0][0]);
	rb = BoxB->Extent[1]*fabs(fReal[1][2]) + BoxB->Extent[2]*fabs(fReal[1][1]);
	t = fabs( fTrans[0]*fReal[2][0] - fTrans[2]*fReal[0][0] );

	if( t > ra + rb )	return FALSE;

	//L = A1 x B1
	ra = BoxA->Extent[0]*fabs(fReal[2][1]) + BoxA->Extent[2]*fabs(fReal[0][1]);
	rb = BoxB->Extent[0]*fabs(fReal[1][2]) + BoxB->Extent[2]*fabs(fReal[1][0]);
	t = fabs( fTrans[0]*fReal[2][1] - fTrans[2]*fReal[0][1] );

	if( t > ra + rb )	return FALSE;

	//L = A1 x B2
	ra = BoxA->Extent[0]*fabs(fReal[2][2]) + BoxA->Extent[2]*fabs(fReal[0][2]);
	rb = BoxB->Extent[0]*fabs(fReal[1][1]) + BoxB->Extent[1]*fabs(fReal[1][0]);
	t = fabs( fTrans[0]*fReal[2][2] - fTrans[2]*fReal[0][2] );

	if( t > ra + rb )	return FALSE;

	//L = A2 x B0
	ra = BoxA->Extent[0]*fabs(fReal[1][0]) + BoxA->Extent[1]*fabs(fReal[0][0]);
	rb = BoxB->Extent[1]*fabs(fReal[2][2]) + BoxB->Extent[2]*fabs(fReal[2][1]);
	t = fabs( fTrans[1]*fReal[0][0] - fTrans[0]*fReal[1][0] );

	if( t > ra + rb )	return FALSE;

	//L = A2 x B1
	ra = BoxA->Extent[0]*fabs(fReal[1][1]) + BoxA->Extent[1]*fabs(fReal[0][1]);
	rb = BoxB->Extent[0] *fabs(fReal[2][2]) + BoxB->Extent[2]*fabs(fReal[2][0]);
	t = fabs( fTrans[1]*fReal[0][1] - fTrans[0]*fReal[1][1] );

	if( t > ra + rb )	return FALSE;

	//L = A2 x B2
	ra = BoxA->Extent[0]*fabs(fReal[1][2]) + BoxA->Extent[1]*fabs(fReal[0][2]);
	rb = BoxB->Extent[0]*fabs(fReal[2][1]) + BoxB->Extent[1]*fabs(fReal[2][0]);
	t = fabs( fTrans[1]*fReal[0][2] - fTrans[0]*fReal[1][2] );

	if( t > ra + rb )	return FALSE;

	// 일치하는 분리축이 하나도 없다~ 

	return TRUE;
}
#endif // __CLIENT
#endif // __HOUSING

//
//	vPos바로 아래쪽의 높이 계산.  바로 아래쪽에 걸리는게 없다면 무조건 땅바닥 높이로 계산.
//
FLOAT CWorld::GetUnderHeight( const D3DXVECTOR3 &vPos )
{
	static D3DXVECTOR3 vDir( 0.0f, -1000.0f, 0.0f );	
	D3DXVECTOR3 vEnd = vPos + vDir;
	Segment3 segment( vPos, vEnd );

	CObj* pObj;
	CObj* pMinObj = NULL;
	CModel* pModel;

	int nRange = 0;
	FOR_LINKMAP( this, vPos, pObj, nRange, CObj::linkStatic, nDefaultLayer )
	{
		// 레이(vPos-vDir)와 오브젝트OBB의 검사.  
		pModel = pObj->m_pModel;
		if( pModel->TestIntersectionOBB_Line( segment, pObj ) == TRUE )
		{
			pMinObj = pObj;		// 찾았으면 바로 튀어나옴 - 속도땜에 이랬다...
			goto LP1;
		}
	}
	END_LINKMAP

LP1:
	if( pMinObj )	// 레이에 걸린 가장 가까운 오브젝트가 있는가	
	{
		FLOAT fDist;
		pModel = pMinObj->m_pModel;
		// 레이와 삼각형으로 세밀 검사
		D3DXVECTOR3	*pTri[3];
		((CModelObject *)pModel)->GetObject3D()->FindTouchTriLine( pTri, vPos, vEnd, pMinObj->GetMatrixWorld(), &fDist );
		if( *pTri )
		{
			D3DXVECTOR3 vIntersect = vPos + fDist * vDir;
			return vIntersect.y;
		}
	}
	return GetLandHeight( vPos.x, vPos.z );
}

FLOAT	CWorld::GetItemHeight( const D3DXVECTOR3 & vPos )
{
	D3DXVECTOR3 vDir( 0.0f, -3.0f, 0.0f );
	D3DXVECTOR3 vIntersect;
	float fMaxY	= -65535.0f;
	D3DXVECTOR3 vOut( 0.0f, 0.0f, 0.0f );
	CObj* pObj;
	CModel* pModel;
	int nRange = 0;

	D3DXVECTOR3 vEnd( vPos + vDir );
	Segment3 segment( vPos, vEnd );
	FOR_LINKMAP( this, vPos, pObj, nRange, CObj::linkStatic, nDefaultLayer )
	{
		// 레이(vPos-vDir)와 오브젝트OBB의 검사.  
		pModel	= pObj->m_pModel;
		if( pModel->TestIntersectionOBB_Line( segment, pObj ) == TRUE )
		{
			// 중력방향 라인과 교차하는 삼각형을 찾고 교차점(높이)을 찾은 후 슬라이딩 벡터를 vOut에 받는다.
			if( ((CModelObject *)pModel)->GetObject3D()->SlideVectorUnder( &vOut, vPos, vEnd, pObj->GetMatrixWorld(), &vIntersect ) == TRUE )
			{
				if( vIntersect.y > fMaxY )
					fMaxY = vIntersect.y;	// 충돌한 폴리곤중에 가장 높은 값을 쓰자....이렇게 되서 졸라 느려졌다.
			}
		}
	}
	END_LINKMAP
	if( fMaxY != -65535.0f )
		return fMaxY;
	return GetLandHeight( vPos );
}

#if __VER >= 15 // __BOUND_BOX_COLLISION
FLOAT CWorld::GetOverHeightForPlayer( D3DXVECTOR3 &vPos, CObj* pExceptionObj )
{
	static D3DXVECTOR3 vDir( 0.0f, 1.0f, 0.0f );
	D3DXVECTOR3 vIntersect, vEnd;
	FLOAT fDist;

	CObj* pObj;
	CObj* pMinObj = NULL;
	CModel* pModel;
	int nRange = 0;
	vEnd = vPos + vDir;
	Segment3 segment( vPos, vEnd );
	Segment3 point( vPos, vPos );

	FOR_LINKMAP( this, vPos, pObj, nRange, CObj::linkStatic, nDefaultLayer )
	{
		pModel = pObj->m_pModel;
		if( pModel->TestIntersectionOBB_Line( segment, pObj ) == TRUE )
		{
			if( pModel->TestIntersectionOBB_Line( point, pObj ) == TRUE )
			{
				pMinObj = pObj;
			}
			else
			{
				pMinObj = pObj;
				goto LP1;
			}
		}
	}
	END_LINKMAP
LP1:
	if( pMinObj )	// 레이에 걸린 가장 가까운 오브젝트가 있는가
	{
		pModel = pMinObj->m_pModel;
		// 레이와 삼각형으로 세밀 검사
		D3DXVECTOR3	*pTri[3];
		((CModelObject *)pModel)->GetObject3D()->FindTouchTriLine( pTri, vPos, vEnd, pMinObj->GetMatrixWorld(), &fDist );
		if( *pTri )
		{
			vIntersect = vPos + fDist * vDir;
			return vIntersect.y;
		}
	}
	return 65535.0f;	// 걸리는 게 없으면 최대치로 설정
}
#endif // __BOUND_BOX_COLLISION

//
//	vPos바로 위쪽의 높이 계산
//
FLOAT CWorld::GetOverHeight( D3DXVECTOR3 &vPos, CObj* pExceptionObj )
{
	static D3DXVECTOR3 vDir( 0.0f, 1.0f, 0.0f );	
	D3DXVECTOR3 vIntersect, vEnd;
	FLOAT fDist;

	CObj* pObj;
	CObj* pMinObj = NULL;
	CModel* pModel;
	int nRange = 0;
	vEnd = vPos + vDir;
	Segment3 segment( vPos, vEnd );

#if __VER >= 15 // __BOUND_BOX_COLLISION
	FOR_LINKMAP( this, vPos, pObj, nRange, CObj::linkStatic, nDefaultLayer )
	{
		pModel = pObj->m_pModel;
		if( pModel->TestIntersectionOBB_Line( segment, pObj ) == TRUE )
		{
			pMinObj = pObj;		// 찾았으면 바로 튀어나옴
			goto LP1;
		}
	}
	END_LINKMAP
#else // __BOUND_BOX_COLLISION
	FOR_LINKMAP( this, vPos, pObj, nRange, CObj::linkStatic, nDefaultLayer )
	{
		// 레이(vPos-vDir)와 오브젝트OBB의 검사.  
		pModel = pObj->m_pModel;
#if __VER >= 13 &&  defined(__CLIENT) // __HOUSING
		if(GetID() == WI_WORLD_MINIROOM)
		{
			if(pObj != GetObjByName("obj_miniwall01.o3d") &&pModel->TestIntersectionOBB_Line( segment, pObj ) == TRUE)
			{
				pMinObj = pObj;		// 찾았으면 바로 튀어나옴
				goto LP1;
			}
		}
#if __VER >= 14 // __INSTANCE_DUNGEON
		else if(GetID() == WI_INSTANCE_OMINOUS || GetID() == WI_INSTANCE_OMINOUS_1 )
		{
			char* pString = ((CModelObject *)pObj->m_pModel)->GetObject3D()->m_szFileName;
			if(strcmp(pString, "obj_ceiling_01.o3d") == 0 || strcmp(pString, "obj_ceiling_02.o3d") == 0)
			{
				if(pModel->TestIntersectionOBB_Line( segment, pObj ) == TRUE)
				{
					pMinObj = pObj;		// 찾았으면 바로 튀어나옴
					goto LP1;
				}
			}
		}
#endif // __INSTANCE_DUNGEON
		else
		{
			if( pModel->TestIntersectionOBB_Line( segment, pObj ) == TRUE )
			{
				pMinObj = pObj;		// 찾았으면 바로 튀어나옴
				goto LP1;
			}
		}
#else
		if( pModel->TestIntersectionOBB_Line( segment, pObj ) == TRUE )
		{
			pMinObj = pObj;		// 찾았으면 바로 튀어나옴
			goto LP1;
		}
#endif
	}
	END_LINKMAP
#endif // __BOUND_BOX_COLLISION
LP1:
	if( pMinObj )	// 레이에 걸린 가장 가까운 오브젝트가 있는가
	{
		pModel = pMinObj->m_pModel;
		// 레이와 삼각형으로 세밀 검사
		D3DXVECTOR3	*pTri[3];
		((CModelObject *)pModel)->GetObject3D()->FindTouchTriLine( pTri, vPos, vEnd, pMinObj->GetMatrixWorld(), &fDist );
		if( *pTri )
		{
			vIntersect = vPos + fDist * vDir;
			return vIntersect.y;
		}
	}
	return 65535.0f;	// 걸리는게 없으면 최대치로 설정
}

//
//	vPos-vDir의 이동벡터를 받아 배경과의 충돌을 검사하고
//	그 후처리를 계산하여 pOut으로 넘겨준다.
//
#ifdef __LAYER_1015
BOOL	CWorld::ProcessCollision( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir, int nSlideCnt, int nLayer )
#else	// __LAYER_1015
BOOL	CWorld::ProcessCollision( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir, int nSlideCnt )
#endif	// __LAYER_1015
{
	_PROFILE("CWorld::ProcessCollision()");

	int nRange = 0;
	CObj	*pObj = NULL;
	CModel	*pModel = NULL;
	D3DXVECTOR3	vEnd, vIntersect, vSlide;
	
	vEnd = vPos + vDir;		// 레이를 선분으로 바꾼다.
	Segment3 segment( vPos, vEnd );
	
	*pOut = vDir;

	FOR_LINKMAP( this, vPos, pObj, nRange, CObj::linkDynamic, nLayer )
	{
		if( pObj->GetType() == OT_CTRL ) 
		{
			// 선분과 오브젝트OBB의 검사.
			pModel = pObj->m_pModel;
			if( pModel->TestIntersectionOBB_Line( segment, pObj, FALSE ) == TRUE )
			{
#if __VER >= 13 // __HOUSING
				BOOL IsColl;

				if(GetID() == WI_WORLD_MINIROOM)
					IsColl = ((CModelObject *)pModel)->GetObject3D()->SlideVectorXZ2( &vSlide, &vIntersect, vPos, vEnd, pObj->GetMatrixWorld());
				else
					IsColl = ((CModelObject *)pModel)->GetObject3D()->SlideVectorXZ2( &vSlide, &vIntersect, vPos, vEnd, pObj->GetMatrixWorld(), 0);

				if(IsColl == TRUE)
#else // __HOUSING
				if( ((CModelObject *)pModel)->GetObject3D()->SlideVectorXZ2( &vSlide, &vIntersect, vPos, vEnd, pObj->GetMatrixWorld(), 0 ) == TRUE )
#endif // __HOUSING
				{
					// 진행방향으로 뭔가 충돌했으면 한번더 돈다. 리커시브로.
					D3DXVECTOR3 vE = vIntersect + vSlide;		// 미끄러진 끝점.
					*pOut = vSlide;
					if( ++nSlideCnt <= 1 )
					{
#ifdef __LAYER_1015
						int r = ProcessCollision( pOut, vPos, vSlide, nSlideCnt, nLayer );	// 충돌한 지점에서 미끄러진지점까지의 벡터로 다시한번 검사.
#else	//__LAYER_1015
						int r = ProcessCollision( pOut, vPos, vSlide, nSlideCnt );	// 충돌한 지점에서 미끄러진지점까지의 벡터로 다시한번 검사.
#endif	// __LAYER_1015
						if( r )
						{
							pOut->x = pOut->y = pOut->z = 0;	// 2번 부딪혔으면 앞으로 나가지 않음.
						}
					}
					return 1;
				}
			}
		}
	}
	END_LINKMAP
		
		
 	FOR_LINKMAP( this, vPos, pObj, nRange, CObj::linkStatic, nDefaultLayer )
	{
		// 선분과 오브젝트OBB의 검사.
		pModel = pObj->m_pModel;
		
		//sun!!
		if( pModel->TestIntersectionOBB_Line( segment, pObj ) == TRUE )
		{
			// 선분과 삼각형의 교차 검사.  교차한다면 slide vector를 구하고
			// 교차하지 않는다면 걍 FALSE를 리턴한다.
			if( ((CModelObject *)pModel)->GetObject3D()->SlideVectorXZ2( &vSlide, &vIntersect, vPos, vEnd, pObj->GetMatrixWorld() ) == TRUE )
			{
				//D3DXVECTOR3 vE = vIntersect + vSlide;
				*pOut = vSlide;
			#if __VER >= 11 // __FIX_COLLISION	
				if( (++nSlideCnt <= 1) && ((D3DXVec3Length(&vDir) < 3.0f)) )
				{
#ifdef __LAYER_1015
					int r = ProcessCollision( pOut, vPos, vSlide, nSlideCnt, nLayer );	// 충돌한 지점에서 미끄러진지점까지의 벡터로 다시한번 검사.
#else	// __LAYER_1015
					int r = ProcessCollision( pOut, vPos, vSlide, nSlideCnt );	// 충돌한 지점에서 미끄러진지점까지의 벡터로 다시한번 검사.
#endif	// __LAYER_1015
					if( r )	// 충돌한 지점에서 미끄러진지점까지의 벡터로 다시한번 검사.
					{
						pOut->x = pOut->y = pOut->z = 0;	// 2번 부딪혔으면 앞으로 나가지 않음.
					}
				}
				else if( nSlideCnt <= 1 )
				{	
#ifdef __LAYER_1015
					int r = ProcessCollision( pOut, vPos, vSlide, nSlideCnt, nLayer );	// 충돌한 지점에서 미끄러진지점까지의 벡터로 다시한번 검사.
#else	// __LAYER_1015
					int r = ProcessCollision( pOut, vPos, vSlide, nSlideCnt );	// 충돌한 지점에서 미끄러진지점까지의 벡터로 다시한번 검사.
#endif	// __LAYER_1015
					if( r )	// 충돌한 지점에서 미끄러진지점까지의 벡터로 다시한번 검사.
					{
						pOut->x = pOut->y = pOut->z = 0;	// 2번 부딪혔으면 앞으로 나가지 않음.
					}
				}
			#else
				D3DXVECTOR3 vE = vIntersect + vSlide;
				*pOut = vSlide;

				if( ++nSlideCnt <= 1 )
				{	
#ifdef __LAYER_1015
					int r = ProcessCollision( pOut, vPos, vSlide, nSlideCnt, nLayer );	// 충돌한 지점에서 미끄러진지점까지의 벡터로 다시한번 검사.
#else	// __LAYER_1015
					int r = ProcessCollision( pOut, vPos, vSlide, nSlideCnt );	// 충돌한 지점에서 미끄러진지점까지의 벡터로 다시한번 검사.
#endif	// __LAYER_1015
					if( r )	// 충돌한 지점에서 미끄러진지점까지의 벡터로 다시한번 검사.
					{
						pOut->x = pOut->y = pOut->z = 0;	// 2번 부딪혔으면 앞으로 나가지 않음.
					}
				}
#endif
				return 2;
			}
		}
	}
	END_LINKMAP
		
	return FALSE;
}


//
//	vPos-vDir의 이동벡터를 받아 배경과의 충돌을 검사하고
//	그 후처리를 계산하여 pOut으로 넘겨준다.
//
BOOL	CWorld::ProcessCollisionReflection( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir, int nRecusCnt )
{
	int nRange = 0;
	CObj	*pObj = NULL;
	CModel	*pModel = NULL;
	D3DXVECTOR3	vEnd, vIntersect, vN, v1;
	D3DXMATRIX mTM;
	FLOAT	fDist;
	D3DXVECTOR3	*pTri[3];

	vEnd = vPos + vDir;		// 레이를 선분으로 바꾼다.
	Segment3 segment( vPos, vEnd );

	FOR_LINKMAP( this, vPos, pObj, nRange, CObj::linkStatic, nDefaultLayer )
	{
		// 선분과 오브젝트OBB의 검사.
		pModel = pObj->m_pModel;
		if( pModel->TestIntersectionOBB_Line( segment, pObj ) == TRUE )
		{
			// 선분과 삼각형의 교차 검사.  pTri에 담겨나온다.
			((CModelObject *)pModel)->GetObject3D()->FindTouchTriLine( pTri, vPos, vEnd, pObj->GetMatrixWorld(), &fDist );
			if( *pTri )
			{
				// (2*(-D dot N)) * N + D
				v1 = *pTri[1] - *pTri[0];
				vN = *pTri[2] - *pTri[0];
				D3DXVec3Cross( &vN, &v1, &vN );
				mTM = pObj->GetMatrixWorld();
				mTM._41 = mTM._42 = mTM._43 = 0;
				D3DXVec3TransformCoord( &vN, &vN, &mTM );	// mTM에 스케일이 있다면 노말도 같이 커지기땜에 Normalize를 나중에 해야 한다.
				D3DXVec3Normalize( &vN, &vN );
				
				v1.x = -vDir.x;	v1.y = -vDir.y;	v1.z = -vDir.z;	// -D
				FLOAT fLenN = D3DXVec3Dot( &v1, &vN );			// -D dot N
				vN *= (fLenN * 2.0f);							// (-D dot N) * 2  이때 N는 단위벡터 상태여야한다.
				*pOut = vDir + vN;								// 최종 반사벡터.

				if( nRecusCnt++ < 3 )
					ProcessCollisionReflection( pOut, vPos, *pOut, nRecusCnt );
				
				return TRUE;
			}
		}
	}
	END_LINKMAP

	FOR_LINKMAP( this, vPos, pObj, nRange, CObj::linkAirShip, nDefaultLayer )
	{
		// 선분과 오브젝트OBB의 검사.
		pModel = pObj->m_pModel;
		if( pModel->TestIntersectionOBB_Line( segment, pObj ) == TRUE )
		{
			// 선분과 삼각형의 교차 검사.  pTri에 담겨나온다.
			((CModelObject *)pModel)->GetObject3D()->FindTouchTriLine( pTri, vPos, vEnd, pObj->GetMatrixWorld(), &fDist );
			if( *pTri )
			{
				// (2*(-D dot N)) * N + D
				v1 = *pTri[1] - *pTri[0];
				vN = *pTri[2] - *pTri[0];
				D3DXVec3Cross( &vN, &v1, &vN );
				mTM = pObj->GetMatrixWorld();
				mTM._41 = mTM._42 = mTM._43 = 0;
				D3DXVec3TransformCoord( &vN, &vN, &mTM );	// mTM에 스케일이 있다면 노말도 같이 커지기땜에 Normalize를 나중에 해야 한다.
				D3DXVec3Normalize( &vN, &vN );
				
				v1.x = -vDir.x;	v1.y = -vDir.y;	v1.z = -vDir.z;	// -D
				FLOAT fLenN = D3DXVec3Dot( &v1, &vN );			// -D dot N
				vN *= (fLenN * 2.0f);							// (-D dot N) * 2  이때 N는 단위벡터 상태여야한다.
				*pOut = vDir + vN;								// 최종 반사벡터.

				if( nRecusCnt++ < 3 )
					ProcessCollisionReflection( pOut, vPos, *pOut, nRecusCnt );
				
				return TRUE;
			}
		}
	}
	END_LINKMAP

	return FALSE;
}


// 좌표 주위 3x3그리드 영역의 삼각형좌표를 스캔해 vDir과 충돌하는 삼각형을 찾아낸다.
D3DXVECTOR3 *FindTouchLandTri( D3DXVECTOR3 *pTris, int nMaxTri, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir, FLOAT *pfDist )
{
	int		i;
	D3DXVECTOR3	*pv0, *pv1, *pv2;
	D3DXVECTOR3	v1, v2;
	FLOAT	fDist;
	BOOL	bRet;
	
	for( i = 0; i < nMaxTri; i ++ )
	{
		pv0 = pTris++;		
		pv1 = pTris++;		
		pv2 = pTris++;
		
		bRet = IsTouchRayTri( pv0, pv1, pv2, &vPos, &vDir, &fDist );
		if( bRet && fDist >= 0.0f )	// 반대방향 면은 검사하지 않음.
		{
			*pfDist = fDist;
			return pv0;
		}
	}
	
	return NULL;
}


// bSkipTrans:반투명되는 오브젝트는 스킵함.  bWithTerrain:지형까지 포함해서 검사함.
//
//	라인과 주변오브젝트와의 충돌을 검사해서, 교차점을 계산한다.
// 주의 : 검색대상 오브젝트는 시야절두체에 들어오는 오브젝트만 대상으로 한다.
// 카메라 충돌은 이걸로 하면 안된다. 카메라가 vPos라서 절두체에 속하는 오브젝트가 계속 바뀐다.
//
BOOL	CWorld::IntersectObjLine( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vEnd, BOOL bSkipTrans, BOOL bWithTerrain, BOOL bWithObject )
{
	D3DXVECTOR3	vDir;
#ifdef __CLIENT
	if( bWithObject )
	{
		CObj* pObj;
		CObj* pNonCullObjs[ 10000 ];
		CObj **pList = m_aobjCull;
		int nNonCullNum = 0;
		for( int i = 0; i < m_nObjCullSize; i++ )
		{
			pObj = *pList++;
			if( pObj )
			{
				if( pObj->GetType() == OT_OBJ || pObj->GetType() == OT_CTRL )
				{
					if( pObj->IsCull() == FALSE /*&& pObj->GetModel()->m_pModelElem->m_dwDistant <= 1*/ ) 
					{
						if( nNonCullNum >= 10000 )
						{
							Error( "CWorld::IntersectObjLine : 범위를 넘어섬." );
							break;
						}
						if( bSkipTrans && pObj->m_pModel->m_pModelElem->m_bTrans )	// 반투명이 되는 오브젝트는 검사대상에서 제외함.
							continue;
						pNonCullObjs[ nNonCullNum++ ] = pObj;
					}
				}
			}
		}
		
		int ObjSortNearToFar( const VOID* arg1, const VOID* arg2 );
		qsort( pNonCullObjs, nNonCullNum, sizeof(CObj*), ObjSortNearToFar ); 

		CModel	*pModel = NULL;
		D3DXVECTOR3	vIntersect;
		FLOAT fDist;
		Segment3 segment( vPos, vEnd );

		for( i = nNonCullNum - 1; i >= 0; i-- )
		{
			pObj = (CObj*)pNonCullObjs[ i ];
			pModel = pObj->m_pModel;
			BOOL bCollObj = TRUE;
			if( pObj->GetType() == OT_CTRL )	bCollObj = FALSE;
			if( pModel->TestIntersectionOBB_Line( segment, pObj, bCollObj ) == TRUE )
			{
				D3DXVECTOR3	*pTri[3];
				((CModelObject *)pModel)->GetObject3D()->FindTouchTriLine( pTri, vPos, vEnd, pObj->GetMatrixWorld(), &fDist, bCollObj );
				if( *pTri )
				{
					if( pOut )
					{
						vDir = vEnd - vPos;
						*pOut = vPos + fDist * vDir;
					}
					return TRUE;
				}
			}
		}
	} // bWithObject
#endif // CLIENT

	BOOL bRet = FALSE;
	
	if( bWithTerrain )		// 지형까지 검사함.
	{
		vDir = vEnd - vPos;		// 레이를 얻음.
		D3DXVec3Normalize( &vDir, &vDir );
		FLOAT fDist = IntersectRayTerrain2( vPos, vDir );
		if( fDist )
		{
			*pOut = vPos + (vDir * fDist);
			bRet = TRUE;
		}
	}

	return bRet;
}



//
// vPos를 중심으로 일정한 반경내의 오브젝트를 대상으로 검색
// 카메라 충돌은 이걸로 해야한다.
// CLIENT 전용
BOOL	CWorld::IntersectObjLine2( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vEnd, BOOL bSkipTrans, BOOL bWithTerrain, BOOL bWithObject )
{
	BOOL bRet = FALSE;
#ifdef __CLIENT
	int nRange = 0;
	CObj	*pObj = NULL;
	CModel	*pModel = NULL;
	D3DXVECTOR3	vDir, vIntersect;
	FLOAT fDist;
	BOOL bAble = TRUE;
	Segment3 segment( vPos, vEnd );

#if __VER >= 12 // __CAM_FAST_RECOVER
	bool			bIsCol    = false;
	D3DXVECTOR3		vTempEnd  = vEnd;
	float			fShortest = 0.0f;

	if(m_nLandWidth < 4) nRange = 5;
	nRange = 5;
	FOR_LINKMAP( this, vPos, pObj, nRange, CObj::linkStatic, nDefaultLayer )
	{
		bAble = TRUE;
		if( bSkipTrans && pObj->m_pModel->m_pModelElem->m_bTrans )	// 반투명이 되는 오브젝트는 검사대상에서 제외함.
			bAble = FALSE;
		
		if( bAble )
		{
			// 선분과 오브젝트OBB의 검사.
			pModel = pObj->m_pModel;
			if( pModel->TestIntersectionOBB_Line( segment, pObj ) == TRUE )
			{
				D3DXVECTOR3	*pTri[3];
				((CModelObject *)pModel)->GetObject3D()->FindTouchTriLine( pTri, vPos, vTempEnd, pObj->GetMatrixWorld(), &fDist);
				if( *pTri )
				{
					if( pOut )
					{
						vDir = vTempEnd - vPos;
						if(bIsCol)
						{
							if(fDist < fShortest)
								fShortest = fDist;
							*pOut = vPos + fShortest * vDir;
							segment.Direction = vTempEnd;
						}
						else
						{
							*pOut = vPos + fDist * vDir;
							segment.Direction = vTempEnd;
							fShortest = fDist;
							bIsCol = true;
						}
					}
				}
			}
		}
	}
	END_LINKMAP
	
	if(bIsCol) return TRUE;
#else
	FOR_LINKMAP( this, vPos, pObj, nRange, CObj::linkStatic, nDefaultLayer )
	{
		bAble = TRUE;
		if( bSkipTrans && pObj->m_pModel->m_pModelElem->m_bTrans )	// 반투명이 되는 오브젝트는 검사대상에서 제외함.
			bAble = FALSE;
		
		if( bAble )
		{
			// 선분과 오브젝트OBB의 검사.
			pModel = pObj->m_pModel;
			if( pModel->TestIntersectionOBB_Line( segment, pObj ) == TRUE )
			{
				D3DXVECTOR3	*pTri[3];
				((CModelObject *)pModel)->GetObject3D()->FindTouchTriLine( pTri, vPos, vEnd, pObj->GetMatrixWorld(), &fDist);
				if( *pTri )
				{
					if( pOut )
					{
						vDir = vEnd - vPos;
						*pOut = vPos + fDist * vDir;
					}
					return TRUE;
				}
			}
		}
	}
	END_LINKMAP
#endif
	// 康: 카메라 충돌에서 사용하는 클라이언트 코드이므로 계층 값을 0으로 설정
	FOR_LINKMAP( this, vPos, pObj, nRange, CObj::linkDynamic, nTempLayer )
	{
		if( pObj->GetType() == OT_CTRL ) 
		{
			bAble = TRUE;
			if( bSkipTrans && pObj->m_pModel->m_pModelElem->m_bTrans )	// 반투명이 되는 오브젝트는 검사대상에서 제외함.
				bAble = FALSE;
			
			if( bAble )
			{
				// 선분과 오브젝트OBB의 검사.
				pModel = pObj->m_pModel;
				if( pModel->TestIntersectionOBB_Line( segment, pObj, FALSE ) == TRUE )
				{
					D3DXVECTOR3	*pTri[3];		// 컨트롤 오브젝트는 충돌메시 안씀.
					((CModelObject *)pModel)->GetObject3D()->FindTouchTriLine( pTri, vPos, vEnd, pObj->GetMatrixWorld(), &fDist, TRUE);
					if( *pTri )
					{
						if( pOut )
						{
							vDir = vEnd - vPos;
							*pOut = vPos + fDist * vDir;
						}
						return TRUE;
					}
				}
			}
		}
	}
	END_LINKMAP
	
	if( bWithTerrain )		// 지형까지 검사함.
	{
		vDir = vEnd - vPos;		// 레이를 얻음.
		D3DXVec3Normalize( &vDir, &vDir );
		FLOAT fDist = IntersectRayTerrain2( vPos, vDir );
		if( fDist )
		{
			*pOut = vPos + (vDir * fDist);
			bRet = TRUE;
		}
	}
#endif	// __CLIENT
	return bRet;
}

// 주어진 레이와 충돌하는 바닥을 체크. - 레이저 공격에 쓰임.
FLOAT CWorld::IntersectRayTerrain2( const D3DXVECTOR3 &vPickRayOrig, const D3DXVECTOR3 &vPickRayDir )
{
#ifdef __CLIENT	
	D3DXVECTOR3 vPickRayAdd ;
	D3DXVECTOR3 vPickRayCur;
	D3DXVECTOR3 vLength;
	D3DXVECTOR3 v1, v2, v3, v4;
	D3DXVECTOR3 _v1, _v2, _v3, _v4;
	BOOL bTriangle1 = FALSE;
	D3DXVECTOR3 vecIntersect;
	FLOAT fDist;               // Ray-Intersection Parameter Distance
	FLOAT fNearDist = m_fFarPlane;                    
	
	vPickRayCur = vPickRayOrig;
	vPickRayAdd = vPickRayDir;
	vPickRayAdd.x *= (FLOAT)MPU * 2;
	vPickRayAdd.z *= (FLOAT)MPU * 2;
	vPickRayAdd.y *= (FLOAT)MPU * 2;
	
	FLOAT fFarPlane = m_fFarPlane;
	fFarPlane /= 2;
	do 
	{
		vPickRayCur += vPickRayAdd;
		if( VecInWorld( vPickRayCur ) )
		{
			CLandscape* pLand = GetLandscape( vPickRayCur );
			if( pLand  && pLand->isVisibile() )
			{
				for( int z = -MPU; z <= MPU; z += MPU )
				{
					for( int x = -MPU; x <= MPU; x += MPU )
					{
						int tempx = (int)( vPickRayCur.x - x );
						int tempy = (int)( vPickRayCur.z - z );
						v1=D3DXVECTOR3( (FLOAT)( tempx       ), GetLandHeight( ( FLOAT ) tempx      , ( FLOAT ) tempy       ), (FLOAT)( tempy       ) );
						v2=D3DXVECTOR3( (FLOAT)( tempx + MPU ), GetLandHeight( ( FLOAT ) tempx + MPU, ( FLOAT ) tempy       ), (FLOAT)( tempy       ) );
						v3=D3DXVECTOR3( (FLOAT)( tempx       ), GetLandHeight( ( FLOAT ) tempx      , ( FLOAT ) tempy + MPU ), (FLOAT)( tempy + MPU ) );
						v4=D3DXVECTOR3( (FLOAT)( tempx + MPU ), GetLandHeight( ( FLOAT ) tempx + MPU, ( FLOAT ) tempy + MPU ), (FLOAT)( tempy + MPU ) );
						if( IntersectTriangle( v1, v2, v3, vPickRayOrig, vPickRayDir, &vecIntersect, &fDist ) ) 
						{
							if( fDist < fNearDist ) { fNearDist = fDist; bTriangle1 = TRUE; }
						}
						else
							if( IntersectTriangle( v2, v4, v3, vPickRayOrig, vPickRayDir, &vecIntersect, &fDist ) ) 
							{
								if( fDist < fNearDist ) {	fNearDist = fDist; bTriangle1 = TRUE; }
							}
					}
				}
			}
		}
		vLength = vPickRayOrig - vPickRayCur;
	} while( D3DXVec3LengthSq( &vLength ) < (fFarPlane * fFarPlane) );
	if( bTriangle1 )
		return fNearDist;
#else // CLIENT
	D3DXVECTOR3 vTri[6];
	FLOAT	x, z;
	int		i, j;
	FLOAT	fDist = 0, fMinDist = 65535.0f;
	BOOL	bRet = FALSE; 

	x = vPickRayOrig.x;		z = vPickRayOrig.z;

	for( i = -10; i < 10; i ++ )
	{
		for( j = -10; j < 10; j ++ )
		{
			if( GetLandTri2( x + (m_iMPU * j), z + (m_iMPU * i), vTri ) )
			{
				bRet = IsTouchRayTri( &vTri[0], &vTri[1], &vTri[2], &vPickRayOrig, &vPickRayDir, &fDist );
				if( bRet && fDist >= 0 )
				{
					if( fDist < fMinDist )
					{
						fMinDist = fDist;
					}
				}
				bRet = IsTouchRayTri( &vTri[3], &vTri[4], &vTri[5], &vPickRayOrig, &vPickRayDir, &fDist );
				if( bRet && fDist >= 0 )
				{
					if( fDist < fMinDist )
					{
						fMinDist = fDist;
					}
				}
			}
		}
	}

	if( fDist )
		return fMinDist;

#endif // not CLIENT

	return 0;
}

#ifdef __SLIDE_060502

// fCosine : -0.19080 == cos(101)
//           -0.37864 == cos(112)
BOOL CanSlide( const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2,
				 float fCosine, D3DXVECTOR3* pOut )
{
	// 1. 노말 계산 
	D3DXVECTOR3	u = v1 - v0;		
	D3DXVECTOR3	v = v2 - v0;

	D3DXVECTOR3 vN;
	D3DXVec3Cross( &vN, &u, &v );					
	D3DXVec3Normalize( &vN, &vN );

	// 2. 일정각 이상 기울어진 면만 슬라이드 벡터 구함 
	D3DXVECTOR3	vDir( 0.0f, -1.0f, 0.0f );		
	if( D3DXVec3Dot( &vN, &vDir ) > fCosine )
	{
		vDir.y = -0.2f;									// 미끄러지는 속도 
		CalcSlideVec( pOut, vDir, vN );					// 미끄러짐 벡터 구함
		return TRUE;
	}
	else
		return FALSE;
}

//
// 중력방향의 충돌검사 및 미끄러짐 처리.
// 중력방향의 라인과 충돌하는 삼각형을 찾고 그 지점의 좌표를 얻고 Slide계산후의 좌표도 얻음.
FLOAT CWorld::ProcessUnderCollision( D3DXVECTOR3 *pOut, CObj **ppObj, D3DXVECTOR3 &vPos )
{
	D3DXVECTOR3		vDir( 0.0f, -1.5f, 0.0f );	// 키높이(1.5)
	D3DXVECTOR3		vIntersect;
	float			fMaxY = -65535.0f;			
	D3DXVECTOR3		vOut( 0.0f, 0.0f, 0.0f );	
	CObj*			pObj;
	CModel*			pModel;
	int				nRange = 0;

	D3DXVECTOR3 vEnd( vPos + vDir );
	Segment3 segment( vPos, vEnd );
#if __VER >= 13 // __HOUSING

#if __VER >= 15 // __GUILD_HOUSE
	if(GetID() == WI_WORLD_MINIROOM || IsWorldGuildHouse() )
#else
	if(GetID() == WI_WORLD_MINIROOM )
#endif //__GUILD_HOUSE
	{
		FOR_LINKMAP( this, vPos, pObj, nRange, CObj::linkDynamic, nDefaultLayer )
		{

			// 레이(vPos-vDir)와 오브젝트OBB의 검사.  
			pModel = pObj->m_pModel;
			if( pObj->GetType() == OT_CTRL ) 
			{
				if( pModel->TestIntersectionOBB_Line( segment, pObj ) == TRUE )
				{
					// 중력방향 라인과 교차하는 삼각형을 찾고 교차점(높이)을 찾은 후 슬라이딩 벡터를 vOut에 받는다.
					if( ((CModelObject *)pModel)->GetObject3D()->SlideVectorUnder( &vOut, vPos, vEnd, pObj->GetMatrixWorld(), &vIntersect ) == TRUE )
					{
						if( vIntersect.y > fMaxY )
						{
							fMaxY = vIntersect.y;	// 충돌한 폴리곤중에 가장 높은 값을 쓰자....이렇게 되서 졸라 느려졌다.
							*pOut = vOut;
							*ppObj = pObj;			// 충돌한 오브젝트.
						}
					}
				}
			}
		}
		END_LINKMAP
	}
#endif // __HOUSING
	FOR_LINKMAP( this, vPos, pObj, nRange, CObj::linkStatic, nDefaultLayer )
	{
		// 레이(vPos-vDir)와 오브젝트OBB의 검사.  
		pModel = pObj->m_pModel;
		if( pModel->TestIntersectionOBB_Line( segment, pObj ) == TRUE )
		{
			// 중력방향 라인과 교차하는 삼각형을 찾고 교차점(높이)을 찾은 후 슬라이딩 벡터를 vOut에 받는다.
			if( ((CModelObject *)pModel)->GetObject3D()->SlideVectorUnder( &vOut, vPos, vEnd, pObj->GetMatrixWorld(), &vIntersect ) == TRUE )
			{
				if( vIntersect.y > fMaxY )
				{
					fMaxY = vIntersect.y;	// 충돌한 폴리곤중에 가장 높은 값을 쓰자....이렇게 되서 졸라 느려졌다.
					*pOut = vOut;
					*ppObj = pObj;			// 충돌한 오브젝트.
				}
			}
		}
	}
	END_LINKMAP

	FOR_LINKMAP( this, vPos, pObj, nRange, CObj::linkAirShip, nDefaultLayer )
	{
		// 레이(vPos-vDir)와 오브젝트OBB의 검사.  
		pModel = pObj->m_pModel;
		if( pModel->TestIntersectionOBB_Line( segment, pObj ) == TRUE )
		{
			// 중력방향 라인과 교차하는 삼각형을 찾고 교차점(높이)을 찾은 후 슬라이딩 벡터를 vOut에 받는다.
			if( ((CModelObject *)pModel)->GetObject3D()->SlideVectorUnder( &vOut, vPos, vEnd, pObj->GetMatrixWorld(), &vIntersect ) == TRUE )
			{
				if( vIntersect.y > fMaxY )
				{
					fMaxY = vIntersect.y;	// 충돌한 폴리곤중에 가장 높은 값을 쓰자....이렇게 되서 졸라 느려졌다.
					*pOut = vOut;
					*ppObj = pObj;			// 충돌한 오브젝트.
				}
			}
		}
	}
	END_LINKMAP

	// 수직벡터와 지형의 충돌지점 구함.
	FLOAT fDist;
	D3DXVECTOR3 vLandTri[3];
	GetLandTri( vPos.x, vPos.z, vLandTri );		// 현재 위치의 하이트맵 삼각형 읽음
	BOOL b = IsTouchRayTri( &vLandTri[0], &vLandTri[1], &vLandTri[2], &vPos, &vDir, &fDist );
	
#ifdef _DEBUG
	if( b == FALSE )
		Error( "ProcessUnderCollision - D3DXIntersectTri x:%f z:%f", vPos.x, vPos.z );
#endif
	vIntersect.x = vPos.x;
	vIntersect.y = (vPos.y + fDist * vDir.y);		// 수직레이와 지형삼각형의 교차점 - 이것은 높이로도 쓰인다.
	vIntersect.z = vPos.z;

	if( fMaxY > vIntersect.y )  // 지형보다 위에서 교차지점이 있으면?
	{		
		if( pOut->y )			// 지형보다 아래로 미끌어 질 수 없다.
		{
			if( pOut->y < vIntersect.y )
				pOut->y = vIntersect.y;
		}
	}
	else
	{
		*pOut = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		fMaxY = vIntersect.y;
	
		// 지형에서 미끌림 - 112도 이상 기울어져 있을 경우만 미끌어진다. 
		if( CanSlide( vLandTri[0], vLandTri[1], vLandTri[2], -0.37864f, &vDir ) )	 // -0.37864f = cos(112) 
		{
			D3DXVECTOR3 vTris[3 * 9 * 2];							// 삼각형 54개
			int nCount = GetLandTris( vPos.x, vPos.z, vTris ) / 3;	// vPos주위의 9개 그리드삼각형 읽음.
			// 교차점에서 slide방향쪽의 삼각형이 충돌하는게 있는지...
			D3DXVECTOR3* p = FindTouchLandTri( vTris, nCount, vIntersect, vDir, &fDist );	
			if( p )
				*pOut = vIntersect + vDir * fDist;
			else
				*pOut = vIntersect + vDir;
		}
	}

	return fMaxY;
}

#else // !__SLIDE_060502

//
// 중력방향의 충돌검사 및 미끄러짐 처리.
// 중력방향의 라인과 충돌하는 삼각형을 찾고 그 지점의 좌표를 얻고 Slide계산후의 좌표도 얻음.
// pType : 충돌한 오브젝트의 타입. OT_OBJ냐 OT_SHIP이냐.
FLOAT CWorld::ProcessUnderCollision( D3DXVECTOR3 *pOut, CObj **pObjColl, D3DXVECTOR3 &vPos )
{
	D3DXVECTOR3 vDir( 0.0f, -1.5f, 0.0f );	// 키높이(1.5) +	여유분(0.5)
	D3DXVECTOR3 vIntersect, vEnd;
	float	fMaxY = -65535.0f;	// 충돌한 좌표중 가장 높은 위치를 찾기 위한...
	D3DXVECTOR3 vOut( 0, 0, 0 );		// *pOut에 최종적으로 넘겨줄 값.

	CObj* pObj;
	CObj* pMinObj = NULL;
	CModel* pModel;
	int nRange = 0;
	//return GetLandHeight( vPos.x, vPos.z );
	vEnd = vPos + vDir;
	int		nCount = 0;
	Segment3 segment( vPos, vEnd );

#if __VER >= 13 // __HOUSING

#if __VER >= 15 // __GUILD_HOUSE
	if(GetID() == WI_WORLD_MINIROOM || IsWorldGuildHouse() )
#else
	if(GetID() == WI_WORLD_MINIROOM )
#endif //__GUILD_HOUSE
	{
		FOR_LINKMAP( this, vPos, pObj, nRange, CObj::linkDynamic, nDefaultLayer )
		{

			// 레이(vPos-vDir)와 오브젝트OBB의 검사.  
			pModel = pObj->m_pModel;
			if( pObj->GetType() == OT_CTRL ) 
			{
				if( pModel->TestIntersectionOBB_Line( segment, pObj ) == TRUE )
				{
					// 중력방향 라인과 교차하는 삼각형을 찾고 교차점(높이)을 찾은 후 슬라이딩 벡터를 vOut에 받는다.
					if( ((CModelObject *)pModel)->GetObject3D()->SlideVectorUnder( &vOut, vPos, vEnd, pObj->GetMatrixWorld(), &vIntersect ) == TRUE )
					{
						if( vIntersect.y > fMaxY )
						{
							fMaxY = vIntersect.y;	// 충돌한 폴리곤중에 가장 높은 값을 쓰자....이렇게 되서 졸라 느려졌다.
							*pOut = vOut;
							*pObjColl = pObj;			// 충돌한 오브젝트.
						}
					}
				}
			}
		}
		END_LINKMAP
	}
#endif // __HOUSING
	FOR_LINKMAP( this, vPos, pObj, nRange, CObj::linkStatic, nDefaultLayer )
	{
		// 레이(vPos-vDir)와 오브젝트OBB의 검사.  
		nCount ++;
		pModel = pObj->m_pModel;
		if( pModel->TestIntersectionOBB_Line( segment, pObj ) == TRUE )
		{
			// 중력방향 라인과 교차하는 삼각형을 찾고 교차점(높이)을 찾은 후 슬라이딩 벡터를 vOut에 받는다.
			if( ((CModelObject *)pModel)->GetObject3D()->SlideVectorUnder( &vOut, vPos, vEnd, pObj->GetMatrixWorld(), &vIntersect ) == TRUE )
			{
				if( vIntersect.y > fMaxY )
				{
					fMaxY = vIntersect.y;	// // 충돌한 폴리곤중에 가장 높은 값을 쓰자....이렇게 되서 졸라 느려졌다.
					*pOut = vOut;
					*pObjColl = pObj;	// 충돌한 오브젝트.
				}
			}
		}
	}
	END_LINKMAP

	FOR_LINKMAP( this, vPos, pObj, nRange, CObj::linkAirShip, nDefaultLayer )
	{
		// 레이(vPos-vDir)와 오브젝트OBB의 검사.  
		nCount ++;
		pModel = pObj->m_pModel;
		if( pModel->TestIntersectionOBB_Line( segment, pObj ) == TRUE )
		{
			// 중력방향 라인과 교차하는 삼각형을 찾고 교차점(높이)을 찾은 후 슬라이딩 벡터를 vOut에 받는다.
			if( ((CModelObject *)pModel)->GetObject3D()->SlideVectorUnder( &vOut, vPos, vEnd, pObj->GetMatrixWorld(), &vIntersect ) == TRUE )
			{
				if( vIntersect.y > fMaxY )
				{
					fMaxY = vIntersect.y;	// // 충돌한 폴리곤중에 가장 높은 값을 쓰자....이렇게 되서 졸라 느려졌다.
					*pOut = vOut;
					*pObjColl = pObj;	// 충돌한 오브젝트.
				}
			}
		}
	}
	END_LINKMAP

	D3DXVECTOR3	v1, v2;
	// 현재 위치의 하이트맵 삼각형 읽음
	// 수직벡터로 인터섹트 지점 구하고(높이) 슬라이드 계산.
	// 
	D3DXVECTOR3 pTri[3 * 9 * 2];		// 삼각형 54개
	FLOAT	fDist;
	FLOAT	vx, vz;

	// 바닥 노말 구함
	vx = vPos.x;	vz = vPos.z;
	GetLandTri( vx, vz, pTri );
	v1 = pTri[1] - pTri[0];		// 노말 계산 시작.
	v2 = pTri[2] - pTri[0];
	D3DXVec3Cross( &v1, &v1, &v2 );		// 바닥의 노말 계산.
	D3DXVec3Normalize( &v1, &v1 );		// 단위벡터로 변환

	// 수직벡터와 바닥면의 충돌지점 구함.
	BOOL b = IsTouchRayTri( &pTri[0], &pTri[1], &pTri[2], &vPos, &vDir, &fDist );
	vIntersect.x = vx;
	vIntersect.z = vz;
	vIntersect.y = (vPos.y + fDist * vDir.y); + 0.01f;		// 수직레이와 바닥삼각형의 교차점 - 이것은 높이로도 쓰인다.
	
#ifdef _XDEBUG
	if( b == FALSE )
		Error( "ProcessUnderCollision : Intersect실패. 캐릭터의 좌표가 맵을 벗어났다. %d %d", vPos.x, vPos.z );
#endif
	
	
	v2.x = v2.z = 0;	v2.y = -1.0f;	// 이건 반드시 단위벡터여야 한다.
	// -0.19080f == cos(101)   -0.37864 == cos(112)
	D3DXVECTOR3 *p = NULL, vTemp9;
	int nMaxTri = 0;
	if( D3DXVec3Dot( &v1, &v2 ) > -0.37864f )	// dot > cos(theta) 일정각 이상 기울어진 면만 슬라이드를 적용시킨다. 두벡터다 노말이어야 한다.
	{
		v2.y = -0.2f;
		CalcSlideVec( &vOut, v2, v1 );		// 미끄러짐 벡터 구함
		vTemp9 = vOut;
		nMaxTri = GetLandTris( vx, vz, pTri ) / 3;	// vPos주위의 9개 그리드삼각형 읽음.
		p = FindTouchLandTri( pTri, nMaxTri, vIntersect, vOut, &fDist );	// 교차점에서 slide방향쪽의 삼각형이 충돌하는게 있는지...
		if( p )
			vOut = vIntersect + vOut * fDist;
		else
			vOut = vIntersect + vOut;

#ifdef __XUZHU
		if( fabs(vPos.z - vOut.z) > 10.0f )
		{
			int a=0;
		}
#endif
	}

	if( vIntersect.y > fMaxY )
	{
		fMaxY = vIntersect.y;
		*pOut = vOut;
	}

	return fMaxY;
}

#endif // not __SLIDE_060502