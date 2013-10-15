#include "stdafx.h"
#include "Model.h"
#include "..\_DirectX\dxutil.h"


CModel::~CModel()
{
//	SAFE_DELETE( m_pBillboard );
	DeleteDeviceObjects();
}

// fSpeed : 애니메이션 속도를 조절하고 싶을때. 1.0이 기본값 2.0은 두배 빨라짐.
void CModel::FrameMove( D3DXVECTOR3 *pvSndPos, float fSpeed )
{
	if( m_nPause )		
		return;
	if( m_nFrameMax == 0 )	
		return;		// 프레임이 없는건 돌필요 없다.

	int		nCurrFrame;
	// 보간을 위한 Slerp 계산.

	m_fFrameOld = m_fFrameCurrent;			// 바로 이전 프레임 번호를 받아둠.

	if( m_fFrameOld > (float)(m_nFrameMax - 1) )
		m_fFrameOld -= (float)m_nFrameMax;

	if( m_bSlow )
		m_fFrameCurrent += 0.1f;			// 애니메이션 진행
	else
		m_fFrameCurrent += (m_fPerSlerp * fSpeed);			// 애니메이션 진행


	nCurrFrame = (int)m_fFrameCurrent;				// 소숫점 떼내고 정수부만..
	m_fSlp = m_fFrameCurrent - (float)nCurrFrame;	// 소숫점 부분만 가져옴

	if( m_nLoop & ANILOOP_LOOP )		// 루핑모드일때
	{
		if( m_fFrameCurrent > (float)(m_nFrameMax - 1) )
		{
			m_bEndFrame = TRUE;
			nCurrFrame %= m_nFrameMax;		// 처음으로 되돌림
		}
	} 
	else		// 루핑모드가 아니라면 마지막 프레임에서 멈춘다.
	{
		if( m_fFrameCurrent > (float)(m_nFrameMax - 1) )
		{
			m_bEndFrame = TRUE;
			m_fSlp = 0.0f;
			nCurrFrame = m_nFrameMax - 1;
		}
	}
	m_fFrameCurrent = (float)nCurrFrame + m_fSlp;	// 편의상 시각적으로 보기 쉽도록 수치 보정
}

void CModel::AddFrame( FLOAT fAddFrame )
{
}
void CModel::SetFrame( FLOAT fFrame )
{
}
BOOL CModel::Render( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX* pmWorld )
{
	return TRUE;
}
BOOL CModel::IsAniable()
{
	if( m_nModelType == MODELTYPE_MESH || m_nModelType == MODELTYPE_BILLBOARD ) 
		return FALSE;
	return TRUE;
}

//
//	현재 프레임의 다음 프레임번호를 리턴
//
int	CModel::GetNextFrame( void )
{
	int		nCurrFrm = (int)m_fFrameCurrent;
	int		nNextFrm;

	nNextFrm = nCurrFrm + 1;
	if( nNextFrm >= m_nFrameMax )		
	{
		if( m_nLoop & ANILOOP_LOOP )	
			nNextFrm = 0;			// 루핑애니메이션이면 처음프레임과 연결될수 있도록 한다.
		else													
			nNextFrm = m_nFrameMax - 1;		// 아니면 마지막 프레임에서 멈춘다
	}

	return nNextFrm;
}

//
//	바운드박스 vMin,vMax를 기준으로 반지름을 돌려줌
//
float CModel::GetRadius( void )
{
	D3DXVECTOR3 vLen = m_vMax - m_vMin;
	float	fLen = D3DXVec3Length( &vLen );
	return fLen / 2.0f;
}

BOOL CModel::IntersectBB( const D3DXVECTOR3 &vRayOrig, const D3DXVECTOR3 &vRayDir, const D3DXMATRIX &mWorld, D3DXVECTOR3* pvIntersect, FLOAT* pfDist )
{
	// Collect all intersections
	D3DXVECTOR3 v1, v2, v3;
	D3DXVECTOR4 vOut;
	D3DXVECTOR3 vPos;

	//  3|2  
	//  -+-
	//	0|1
	//  
	//  7|6
	//  -+-
	//	4|5

	static int anIntexTable[ 12 * 3 ] =
	{
		0, 1, 2,  0, 2, 3,
		0, 4, 5,  0, 5, 1,
		4, 5, 6,  4, 6, 7,
		3, 2, 6,  3, 6, 7,
		0, 3, 7,  0, 7, 4,
		1, 2, 6,  1, 6, 5
	};
	const BOUND_BOX* pBB = GetBBVector();

	for( int i = 0; i < 12 * 3; i += 3 )
	{
		v1 = pBB->m_vPos[ anIntexTable[ i + 0 ] ];
		v2 = pBB->m_vPos[ anIntexTable[ i + 1 ] ];
		v3 = pBB->m_vPos[ anIntexTable[ i + 2 ] ];					

		D3DXVec3TransformCoord( &v1, &v1, &mWorld ); 
		D3DXVec3TransformCoord( &v2, &v2, &mWorld ); 
		D3DXVec3TransformCoord( &v3, &v3, &mWorld ); 
		
		if( IntersectTriangle( v1, v2, v3, vRayOrig, vRayDir, pvIntersect, pfDist ) ) 
			return TRUE;
	}
	return FALSE;
}
