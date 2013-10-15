#include "stdafx.h"
#include "xUtil3D.h"
#include "..\_DirectX\d3dutil.h"


// QuaternionRotationToYPW -> 쿼터니온에서 Yaw, Pitch, Roll을 갖고 온다
void QuaternionRotationToYPW( const D3DXQUATERNION& qRot, D3DXVECTOR3& vYPW )
{
	FLOAT pitch, yaw, roll;
	FLOAT sqw = qRot.w*qRot.w;
	FLOAT sqx = qRot.x*qRot.x;
	FLOAT sqy = qRot.y*qRot.y;
	FLOAT sqz = qRot.z*qRot.z;
	
	pitch = asinf(2.0f * (qRot.w*qRot.x - qRot.y*qRot.z)); 
	yaw   = atan2f(2.0f * (qRot.x*qRot.z + qRot.w*qRot.y),(-sqx - sqy + sqz + sqw)); 
	roll  = atan2f(2.0f * (qRot.x*qRot.y + qRot.w*qRot.z),(-sqx + sqy - sqz + sqw));
	
	vYPW.x = pitch;
	vYPW.y = yaw;
	vYPW.z = roll;
}


// boundbox vMin, vMax값을 8개의 벡터 좌표로 풀어냄
void	SetBB( D3DXVECTOR3 *pBBVList, const D3DXVECTOR3 &vMin, const D3DXVECTOR3 &vMax )
{
	pBBVList[0].x = vMin.x;
	pBBVList[0].y = vMax.y;
	pBBVList[0].z = vMin.z;

	pBBVList[1].x = vMax.x;
	pBBVList[1].y = vMax.y;
	pBBVList[1].z = vMin.z;

	pBBVList[2].x = vMax.x;
	pBBVList[2].y = vMax.y;
	pBBVList[2].z = vMax.z;

	pBBVList[3].x = vMin.x;
	pBBVList[3].y = vMax.y;
	pBBVList[3].z = vMax.z;

	pBBVList[4].x = vMin.x;
	pBBVList[4].y = vMin.y;
	pBBVList[4].z = vMin.z;

	pBBVList[5].x = vMax.x;
	pBBVList[5].y = vMin.y;
	pBBVList[5].z = vMin.z;

	pBBVList[6].x = vMax.x;
	pBBVList[6].y = vMin.y;
	pBBVList[6].z = vMax.z;

	pBBVList[7].x = vMin.x;
	pBBVList[7].y = vMin.y;
	pBBVList[7].z = vMax.z;
}


//
//
// Axis Aligned Bounding Boxes(AABB) 방식 충돌 검출
int		IsTouchAABB( const D3DXVECTOR3 &vMin1, const D3DXVECTOR3 &vMax1,  
					 const D3DXVECTOR3 &vMin2, const D3DXVECTOR3 &vMax2 )
{
	if( vMin2.x > vMax1.x )		return 0;		// v2가 x축 오른쪽으로 벗어났다.
	if( vMax2.x < vMin1.x )		return 0;		// v2가 x축 왼쪽으로 벗어났다.
	if( vMin2.z > vMax1.z )		return 0;		// v2가 z축 앞으로 벗어났다.
	if( vMax2.z < vMin1.z )		return 0;		// v2가 z축 뒤로으로 벗어났다.
	if( vMin2.y > vMax1.y )		return 0;		// v2가 y축 위로 벗어났다.
	if( vMax2.y < vMin1.y )		return 0;		// v2가 y축 아래로 벗어났다.

	return 1;
}



#define CLIP_RIGHT	0x00000001
#define CLIP_LEFT	0x00000002
#define CLIP_TOP	0x00000004
#define CLIP_BOTTOM	0x00000008
#define CLIP_FRONT	0x00000010
#define CLIP_BACK	0x00000020

// calculates the cohen-sutherland outcode for a point and a bounding box.
//
// vMin:	min vector of the bounding box
// vMax:	max vector of the bounding box
// vPnt:			the point to check
//
// returns:		the outcode
//
static int CalcOutCode( const D3DXVECTOR3 &vMin, const D3DXVECTOR3 &vMax, const D3DXVECTOR3 &v )
{
	int nOutCode = 0;

	if( v.x > vMax.x ) {
		nOutCode |= CLIP_RIGHT;
	} else if( v.x < vMin.x ) {
		nOutCode |= CLIP_LEFT;
	}
	if( v.y > vMax.y ) {
		nOutCode |= CLIP_TOP;
	} else if( v.y < vMin.y ) {
		nOutCode |= CLIP_BOTTOM;
	}
	if( v.z > vMax.z ) {
		nOutCode |= CLIP_BACK;
	} else if( v.z < vMin.z ) {
		nOutCode |= CLIP_FRONT;
	}

	return nOutCode;
}

//
// 코헨 서더랜드 라인클리핑 알고리즘
//
// vMin:	AABB min
// vMax:	AABB max
// v1:		라인 시작점
// v2:		라인 끝점
// pvIntercept(OUT):	실제 교차점
//
BOOL	IsTouchAABB_Line( const D3DXVECTOR3 &vMin, const D3DXVECTOR3 &vMax, 
						  const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, 
						  D3DXVECTOR3 *pvIntersect )
{
	int nOutCode1, nOutCode2;

	nOutCode1 = CalcOutCode( vMin, vMax, v1 );
	if( nOutCode1 == 0 ) {
		// point inside bounding box
		*pvIntersect = v1;
		return TRUE;
	}

	nOutCode2 = CalcOutCode( vMin, vMax, v2 );
	if( nOutCode2 == 0 ) {
		// point inside bounding box
		*pvIntersect = v2;
		return TRUE;
	}

	if( (nOutCode1 & nOutCode2) > 0 ) {
		// both points on same side of box
		return FALSE;
	}

	// check intersections
	if( nOutCode1 & (CLIP_RIGHT | CLIP_LEFT) ) {
		if( nOutCode1 & CLIP_RIGHT ) {
			pvIntersect->x = vMax.x;
		} else {
			pvIntersect->x = vMin.x;
		}
		float x1 = v2.x - v1.x;
		float x2 = pvIntersect->x - v1.x;
		pvIntersect->y = v1.y + x2 * (v2.y - v1.y) / x1;
		pvIntersect->z = v1.z + x2 * (v2.z - v1.z) / x1;

		if( pvIntersect->y <= vMax.y && pvIntersect->y >= vMin.y && pvIntersect->z <= vMax.z && pvIntersect->z >= vMin.z ) {
			return TRUE;
		}
	}

	if( nOutCode1 & (CLIP_TOP | CLIP_BOTTOM) ) {
		if( nOutCode1 & CLIP_TOP ) {
			pvIntersect->y = vMax.y;
		} else {
			pvIntersect->y = vMin.y;
		}
		float y1 = v2.y - v1.y;
		float y2 = pvIntersect->y - v1.y;
		pvIntersect->x = v1.x + y2 * (v2.x - v1.x) / y1;
		pvIntersect->z = v1.z + y2 * (v2.z - v1.z) / y1;

		if( pvIntersect->x <= vMax.x && pvIntersect->x >= vMin.x && pvIntersect->z <= vMax.z && pvIntersect->z >= vMin.z ) {
			return TRUE;
		}
	}

	if( nOutCode1 & (CLIP_FRONT | CLIP_BACK) ) {
		if( nOutCode1 & CLIP_BACK ) {
			pvIntersect->z = vMax.z;
		} else {
			pvIntersect->z = vMin.z;
		}
		float z1 = v2.z - v1.z;
		float z2 = pvIntersect->z - v1.z;
		pvIntersect->x = v1.x + z2 * (v2.x - v1.x) / z1;
		pvIntersect->y = v1.y + z2 * (v2.y - v1.y) / z1;

		if( pvIntersect->x <= vMax.x && pvIntersect->x >= vMin.x && pvIntersect->y <= vMax.y && pvIntersect->y >= vMin.y ) {
			return TRUE;
		}
	}

	// nothing found
	return FALSE;
}

//
//	OBB - Line 충돌 검사
//	vMin, vMax : Local좌표
//	m : 월드매트릭스
//	v1, v2 : 월드 좌표
//
BOOL	IsTouchOBB_Line( const D3DXVECTOR3 &vMin, const D3DXVECTOR3 &vMax, const D3DXMATRIX &m, 
						 const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, D3DXVECTOR3 *pvIntersect )
{

	D3DXMATRIX	mInv;
	D3DXMatrixInverse( &mInv, NULL, &m );	// AABB의 회전 매트릭스인 m을 역회전 시키기 위해 역행렬을 만든다.

	D3DXVECTOR3	rv1, rv2;
	D3DXVec3TransformCoord( &rv1, &v1, &mInv );		// 선의 양 끝점을 역회전 시킨다.
	D3DXVec3TransformCoord( &rv2, &v2, &mInv );

	BOOL bResult = IsTouchAABB_Line( vMin, vMax, rv1, rv2, pvIntersect );	// AABB - Line 교차를 테스트
	if( bResult) 
		D3DXVec3TransformCoord( pvIntersect, pvIntersect, &m );

	return bResult;
}


BOOL	IsTouchRayTri( const D3DXVECTOR3 *v0, const D3DXVECTOR3 *v1, const D3DXVECTOR3 *v2,
					   const D3DXVECTOR3 *vOrig, const D3DXVECTOR3 *vDir, 
					   FLOAT* pfDist )
{
    // Find vectors for two edges sharing vert0
	FLOAT t,u,v;
    D3DXVECTOR3 edge1 = *v1 - *v0;
    D3DXVECTOR3 edge2 = *v2 - *v0;

    // Begin calculating determinant - also used to calculate U parameter
    D3DXVECTOR3 pvec;
    D3DXVec3Cross( &pvec, vDir, &edge2 );

    // If determinant is near zero, ray lies in plane of triangle
    FLOAT det = D3DXVec3Dot( &edge1, &pvec );

    D3DXVECTOR3 tvec;
    if( det > 0 )
        tvec = *vOrig - *v0;
    else
		return FALSE;	// 면이 반대다.

    if( det < 0.0001f )
        return FALSE;

    // Calculate U parameter and test bounds
    u = D3DXVec3Dot( &tvec, &pvec );
    if( u < 0.0f || u > det )
        return FALSE;

    // Prepare to test V parameter
    D3DXVECTOR3 qvec;
    D3DXVec3Cross( &qvec, &tvec, &edge1 );

    // Calculate V parameter and test bounds
    v = D3DXVec3Dot( vDir, &qvec );
    if( v < 0.0f || u + v > det )
        return FALSE;

    // Calculate t, scale parameters, ray intersects triangle
	t = D3DXVec3Dot( &edge2, &qvec );
	FLOAT fInvDet = 1.0f / det;
	t *= fInvDet;
	*pfDist = t;
//	u *= fInvDet;
//	v *= fInvDet;

//	*pvIntersect = vOrig + t * vDir;
    return TRUE;
}

//
//	vN을 법선벡터로 갖는 면에 vDir벡터가 부딪혔을 때 Slide벡터를 계산해서 pOut으로 돌려준다. 
//	vN은 단위벡터.
//

void	CalcSlideVec( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vDir, const D3DXVECTOR3 &vN )
{
	D3DXVECTOR3 vInvN;
	vInvN.x = -vN.x;	vInvN.y = -vN.y;	vInvN.z = -vN.z;	// 노말을 뒤집음.
	FLOAT	b = D3DXVec3Dot( &vDir, &vInvN );		// (V dot -N)	직각삼각형의 밑변의 길이.
	*pOut = vDir - (vInvN * b);		// slide vector
}

//////////////////////////////////////////////////////////////////////

Segment3::Segment3( const D3DXVECTOR3& v1, const D3DXVECTOR3& v2 )
{
	Origin = (v1 + v2) * 0.5f;
	Direction = v2-v1;
	Extent = D3DXVec3Length( &Direction ) * 0.5f;
	D3DXVec3Normalize( &Direction, &Direction );
} 

// vMin, vMax - AABB 로컬좌표 
void BBOX::UpdateMartix( const D3DXVECTOR3& vMin, const D3DXVECTOR3& vMax, 
						 const D3DXMATRIX& matScale, const D3DXMATRIX& matRotation, const D3DXMATRIX& matWorld )
{
	//center
	Center = ( vMin + vMax ) * 0.5f;
	D3DXVec3TransformCoord( &Center, &Center, &matWorld );

	//extent
	D3DXVECTOR3	vExtent = ( vMax - vMin ) * 0.5f;

	Extent[0] = vExtent.x * matScale._11;
	Extent[1] = vExtent.y * matScale._22;
	Extent[2] = vExtent.z * matScale._33;
	
	//axis
	Axis[0].x = matRotation._11;
	Axis[0].y = matRotation._12;
	Axis[0].z = matRotation._13;

	Axis[1].x = matRotation._21;
	Axis[1].y = matRotation._22;
	Axis[1].z = matRotation._23;

	Axis[2].x = matRotation._31;
	Axis[2].y = matRotation._32;
	Axis[2].z = matRotation._33;
}

bool IntrSegment3Box3_Test( const Segment3& rkSegment, const BBOX& rkBox )
{
    float afAWdU[3], afADdU[3], afAWxDdU[3], fRhs;

    D3DXVECTOR3 kDiff = rkSegment.Origin - rkBox.Center;

	for( int i=0; i<3; ++i )
	{
		afAWdU[i] = fabs( D3DXVec3Dot(&rkSegment.Direction, &rkBox.Axis[i]) );
		afADdU[i] = fabs( D3DXVec3Dot(&kDiff, &rkBox.Axis[i]) );

#if __VER >= 15 // __BS_BBOX_ABS_EXTENT
		fRhs = fabs(rkBox.Extent[i]) + rkSegment.Extent*afAWdU[i];
#else
		fRhs = rkBox.Extent[i] + rkSegment.Extent*afAWdU[i];
#endif //__BS_BBOX_ABS_EXTENT
		
		if (afADdU[i] > fRhs)
		{
			return false;
		}
	}

	D3DXVECTOR3 kWxD;
    D3DXVec3Cross( &kWxD, &rkSegment.Direction, &kDiff );
	
    afAWxDdU[0] = fabs( D3DXVec3Dot(&kWxD, &rkBox.Axis[0]) );
  
#if __VER >= 15 // __BS_BBOX_ABS_EXTENT
	fRhs = fabs(rkBox.Extent[1])*afAWdU[2] + fabs(rkBox.Extent[2])*afAWdU[1];
#else
	fRhs = rkBox.Extent[1]*afAWdU[2] + rkBox.Extent[2]*afAWdU[1];
#endif //__BS_BBOX_ABS_EXTENT

    if (afAWxDdU[0] > fRhs)
    {
        return false;
    }

    afAWxDdU[1] = fabs( D3DXVec3Dot(&kWxD, &rkBox.Axis[1]) );
   
#if __VER >= 15 // __BS_BBOX_ABS_EXTENT
	fRhs = fabs(rkBox.Extent[0])*afAWdU[2] + fabs(rkBox.Extent[2])*afAWdU[0];
#else
	fRhs = rkBox.Extent[0]*afAWdU[2] + rkBox.Extent[2]*afAWdU[0];
#endif //__BS_BBOX_ABS_EXTENT

    if (afAWxDdU[1] > fRhs)
    {
        return false;
    }

    afAWxDdU[2] = fabs( D3DXVec3Dot(&kWxD, &rkBox.Axis[2]) );
   
#if __VER >= 15 // __BS_BBOX_ABS_EXTENT
	fRhs = fabs(rkBox.Extent[0])*afAWdU[1] + fabs(rkBox.Extent[1])*afAWdU[0];
#else
	fRhs = rkBox.Extent[0]*afAWdU[1] + rkBox.Extent[1]*afAWdU[0];
#endif //__BS_BBOX_ABS_EXTENT

    if (afAWxDdU[2] > fRhs)
    {
        return false;
    }

    return true;
}





#if 0

/*
IF (
    (|D.x| > BoxHalfSize.x  and  D.x * V.x >= 0 ) or
    (|D.y| > BoxHalfSize.y  and  D.y * V.y >= 0 ) or
    (|D.z| > BoxHalfSize.z  and  D.z * V.z >= 0 ) or
    | (V x D).x | > BoxHalfSize.y * V.z + BoxHalfSize.z * V.y or
    | (V x D).y | > BoxHalfSize.z * V.x + BoxHalfSize.x * V.z or
    | (V x D).z | > BoxHalfSize.x * V.y + BoxHalfSize.y * V.x 
) 교차하지 않음.

D : ( Ray의 시작점 - Box의 중심점 )
V : Ray의 방향벡터
BoxHalfSize : ( Box의 크기 / 2 )
*/

//
//	Ray와 AABB간의 충돌
//
//	Ray : 중심벡터, 방향벡터
//	AABB : 박스 중심점, 박스 크기
//
BOOL	IsTouchRayAABB( const D3DXVECTOR3& vRayOrig, const D3DXVECTOR3& vRayDir, 
					    const D3DXVECTOR3& vBoxOrig, const D3DXVECTOR3& vBoxSize )
{
/*
	float	hx, hy, hz;
	D3DXVECTOR3	vDist = vRayOrig - vBoxOrig;
	D3DXVECTOR3	vCross;

	hx = vBoxSize.x / 2;
	hy = vBoxSize.y / 2;
	hz = vBoxSize.z / 2;

	if( fabs(vDist.x) > hx  &&  vDist.x * vRayDir.x >= 0 )	return FALSE;
	if( fabs(vDist.y) > hy  &&  vDist.y * vRayDir.y >= 0 )	return FALSE;
	if( fabs(vDist.z) > hz  &&  vDist.z * vRayDir.z >= 0 )	return FALSE;
	
	// vRayDir x vDist, Cross product
	D3DXVec3Cross( &vCross, &vRayDir, &vDist );
	if( fabs(vCross.x) > hy * vRayDir.z + hz * vRayDir.y )	return FALSE;
	if( fabs(vCross.y) > hz * vRayDir.x + hx * vRayDir.z )	return FALSE;
	if( fabs(vCross.z) > hx * vRayDir.y + hy * vRayDir.x )	return FALSE;
*/

	return TRUE;
}

				 
//
//	Ray와 구(Sphere)의 충돌을 검사
//
BOOL	IsTouchRaySphere( const D3DXVECTOR3& vRayOrig, const D3DXVECTOR3& vRayDir, 
						  const D3DXVECTOR3& vPos, float fR )
{
	D3DXVECTOR3	vQ;

	vQ = vRayOrig - vPos;		// ray - sphere(스피어기준 Orig좌표)
	float a0 = D3DXVec3Dot( &vQ, &vQ ) - (fR * fR);		// Orig이 구체안에 있는가?
	if( a0 <= 0 )
		return TRUE;
	float a1 = D3DXVec3Dot( &vRayDir, &vQ );			// RayDir이 반대방향을 보고 있는가
	if( a1 >= 0 )
		return FALSE;

	return( a1 * a1 >= a0 );

}

//
//	Ray와 구(Sphere)의 충돌을 검사
//
BOOL	IsTouchRaySphere2( const D3DXVECTOR3& vRayOrig, const D3DXVECTOR3& vRayDir, 
						  const D3DXVECTOR3& vPos, float fR )
{
	D3DXVECTOR3	vDiff = vPos - vRayOrig;
	float		fT = D3DXVec3Dot( &vDiff, &vRayDir );

	if( fT <= 0.0f )
	{
		fT = 0.0f;
	} else
	{
		
		fT /= D3DXVec3LengthSq( &vRayDir );
		vDiff -= (fT * vRayDir);
	}

	float fSqrDist = D3DXVec3LengthSq( &vDiff );

	return fSqrDist <= fR;
}
/*
MgcReal MgcSqrDistance (const MgcVector3& rkPoint, const MgcRay3& rkRay,
    MgcReal* pfParam)
{
    MgcVector3 kDiff = rkPoint - rkRay.Origin();
    MgcReal fT = kDiff.Dot(rkRay.Direction());

    if ( fT <= 0.0 )
    {
        fT = 0.0;
    }
    else
    {
        fT /= rkRay.Direction().SquaredLength();
        kDiff -= fT*rkRay.Direction();
    }

    if ( pfParam )
        *pfParam = fT;

    return kDiff.SquaredLength();
}
bool MgcTestIntersection (const MgcRay3& rkRay, const MgcSphere& rkSphere)
{
    MgcReal fSqrDist = MgcSqrDistance(rkSphere.Center(),rkRay);
    return fSqrDist <= rkSphere.Radius();
}
*/

//
//	2D 스크린 좌표를 3D좌표계의 레이(ray)로 바꾼다
//
//	재료 : 스크린크기 w, h, 마우스좌표 x, y, 매트릭스 투영, 뷰
//			
//
void	CalcRay( D3DXVECTOR3 *pvRayOrig, D3DXVECTOR3 *pvRayDir, 
				 int nScrW, int nScrH,
				 int nMouX, int nMouY, 
				 const D3DXMATRIX &mProj, const D3DXMATRIX &mView, const D3DXMATRIX *pmWorld )
{
	D3DXVECTOR3 v;

	// Compute the vector of the pick ray in screen space
	// 마우스 좌표를 공간상의 레이로 바꾼다
	v.x =  ( ( ( (float)nMouX * 2 ) / nScrW ) - 1 ) / mProj._11;
	v.y = -( ( ( (float)nMouY * 2 ) / nScrH ) - 1 ) / mProj._22;
	v.z =  1.0f;

	D3DXMATRIX	m;
	if( pmWorld )
	{
		D3DXMATRIX	m2;
		m2 = *pmWorld * mView;
		D3DXMatrixInverse( &m, NULL, &m2 );
	} else
	{
		D3DXMatrixInverse( &m, NULL, &mView );
	}

	// Transform the screen space pick ray into 3D space
	// 카메라 좌표계의 레이를 월드좌표계로 바꾼다, 레이시작점과 방향을 계산
	pvRayDir->x  = v.x * m._11  +  v.y * m._21  +  v.z * m._31;
	pvRayDir->y  = v.x * m._12  +  v.y * m._22  +  v.z * m._32;
	pvRayDir->z  = v.x * m._13  +  v.y * m._23  +  v.z * m._33;
	pvRayOrig->x = m._41;
	pvRayOrig->y = m._42;
	pvRayOrig->z = m._43;
}

BOOL	IsTouchRayAABB2( const D3DXVECTOR3& vRayOrig, const D3DXVECTOR3& vRayDir, 
					     const D3DXVECTOR3& vBoxOrig, const D3DXVECTOR3& vBoxSize )
{
/*
	double	vMin[3];
	double	vMax[3];
	double	vOrig[3];
	double	vDir[3];
	double	vCood[3];

	float	hx = vBoxSize.x / 2;
	float	hy = vBoxSize.y / 2;
	float	hz = vBoxSize.z / 2;

	vMin[0] = vBoxOrig.x - hx;
	vMin[1] = vBoxOrig.x - hy;
	vMin[2] = vBoxOrig.x - hz;
	vMax[0] = vBoxOrig.x + hx;
	vMax[1] = vBoxOrig.x + hy;
	vMax[2] = vBoxOrig.x + hz;
	
	vOrig[0] = vRayOrig.x;
	vOrig[1] = vRayOrig.y;
	vOrig[2] = vRayOrig.z;
	vDir[0] = vRayDir.x;
	vDir[1] = vRayDir.y;
	vDir[2] = vRayDir.z;

	vCood[0] = vCood[1] = vCood[2] = 0.0f;

	if( HitBoundingBox( vMin, vMax, vOrig, vDir, vCood ) )
		return TRUE;
*/
	return FALSE;
}

/*
bool Meshmerizer::RayAABB(const Ray& ray, const AABB& aabb)
{
	Point BoxExtents, Diff;

	Diff.x = ray.mOrig.x - aabb.GetCenter(0);
	BoxExtents.x = aabb.GetExtents(0);
	if(fabsf(Diff.x)>BoxExtents.x && Diff.x*ray.mDir.x>=0.0f)	return false;

	Diff.y = ray.mOrig.y - aabb.GetCenter(1);
	BoxExtents.y = aabb.GetExtents(1);
	if(fabsf(Diff.y)>BoxExtents.y && Diff.y*ray.mDir.y>=0.0f)	return false;

	Diff.z = ray.mOrig.z - aabb.GetCenter(2);
	BoxExtents.z = aabb.GetExtents(2);
	if(fabsf(Diff.z)>BoxExtents.z && Diff.z*ray.mDir.z>=0.0f)	return false;

	float fAWdU[3];
	fAWdU[0] = fabsf(ray.mDir.x);
	fAWdU[1] = fabsf(ray.mDir.y);
	fAWdU[2] = fabsf(ray.mDir.z);

	float f;
	f = ray.mDir.y * Diff.z - ray.mDir.z * Diff.y;	if(fabsf(f)>BoxExtents.y*fAWdU[2] + BoxExtents.z*fAWdU[1])	return false;
	f = ray.mDir.z * Diff.x - ray.mDir.x * Diff.z;	if(fabsf(f)>BoxExtents.x*fAWdU[2] + BoxExtents.z*fAWdU[0])	return false;
	f = ray.mDir.x * Diff.y - ray.mDir.y * Diff.x;	if(fabsf(f)>BoxExtents.x*fAWdU[1] + BoxExtents.y*fAWdU[0])	return false;

	return true;
}

*/

BOOL	IsTouchRayAABB3( const D3DXVECTOR3& vRayOrig, const D3DXVECTOR3& vRayDir, 
					     const D3DXVECTOR3& vBoxOrig, const D3DXVECTOR3& vBoxSize )
{
	D3DXVECTOR3	vDiff;
	
//	vDiff = vRayOrig - vBoxOrig;
	vDiff.x = vRayOrig.x - vBoxOrig.x;
	if(fabsf(vDiff.x)>vBoxSize.x && vDiff.x*vRayDir.x>=0.0f)	return false;

	vDiff.y = vRayOrig.y - vBoxOrig.y;
	if(fabsf(vDiff.y)>vBoxSize.y && vDiff.y*vRayDir.y>=0.0f)	return false;

	vDiff.z = vRayOrig.z - vBoxOrig.z;
	if(fabsf(vDiff.z)>vBoxSize.z && vDiff.z*vRayDir.z>=0.0f)	return false;

	float fAWdU[3];
	fAWdU[0] = fabsf(vRayDir.x);
	fAWdU[1] = fabsf(vRayDir.y);
	fAWdU[2] = fabsf(vRayDir.z);

	float f;
	f = vRayDir.y * vDiff.z - vRayDir.z * vDiff.y;	if(fabsf(f)>vBoxSize.y*fAWdU[2] + vBoxSize.z*fAWdU[1])	return false;
	f = vRayDir.z * vDiff.x - vRayDir.x * vDiff.z;	if(fabsf(f)>vBoxSize.x*fAWdU[2] + vBoxSize.z*fAWdU[0])	return false;
	f = vRayDir.x * vDiff.y - vRayDir.y * vDiff.x;	if(fabsf(f)>vBoxSize.x*fAWdU[1] + vBoxSize.y*fAWdU[0])	return false;

	return true;
}
//
// 삼각형의 꼭지점 v1, v2, v3로부터 법선 벡터를 계산해서 pNormal에 넣어준다.
// 법선을 단위벡터로 만들어 주진 않는다.
//
void	CalcFaceNormal( D3DXVECTOR3 *pNormal, const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, const D3DXVECTOR3 &v3 )
{
	D3DXVECTOR3 vA, vB;

	vA = v2 - v1;
	vB = v3 - v1;
	D3DXVec3Cross( pNormal, &vA, &vB );
	
}

//
// 삼각형의 꼭지점 v1, v2, v3로부터 법선 벡터(단위벡터)를 계산해서 pNormal에 넣어준다.
//
void	CalcFaceUnitNormal( D3DXVECTOR3 *pNormal, const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, const D3DXVECTOR3 &v3 )
{
	D3DXVECTOR3 vA, vB;

	vA = v2 - v1;
	vB = v3 - v1;
	D3DXVec3Cross( pNormal, &vA, &vB );
	D3DXVec3Normalize( pNormal, pNormal );
}

#endif // if 0