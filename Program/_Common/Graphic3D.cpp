#include "stdafx.h"
#include "Graphic3D.h"


CGraphic3D	g_Grp3D;

//
//
//
void	CGraphic3D :: Init( void )
{
	int		i;

	m_pd3dDevice = NULL;
	memset( m_aVertex, 0, sizeof(FVF_3DVERTEX) * MAX_3DVERTEX );

	for( i = 0; i < MAX_3DVERTEX; i ++ )
	{
		m_aVertex[i].vPos    = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		m_aVertex[i].dwColor = 0xffffffff;
	}
}

void	CGraphic3D :: Destroy( void )
{
	Init();
}

void	CGraphic3D :: RenderPoint( D3DXVECTOR3 &v, DWORD dwColor )
{
	FVF_3DVERTEX	aList[1];

	aList[0].vPos = v;
	aList[0].dwColor = dwColor;

	m_pd3dDevice->SetFVF( D3DFVF_3DVERTEX );
	m_pd3dDevice->DrawPrimitiveUP( D3DPT_POINTLIST, 1, aList, sizeof(FVF_3DVERTEX) );
}

void	CGraphic3D :: Render3DLine( D3DXVECTOR3 &vOrig, D3DXVECTOR3 &vTar, DWORD dwColor )
{
	FVF_3DVERTEX	aList[2];

	aList[0].vPos = vOrig;
	aList[1].vPos = vTar;

	aList[0].dwColor = dwColor;
	aList[1].dwColor = dwColor;

	m_pd3dDevice->SetFVF( D3DFVF_3DVERTEX );
	m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, aList, sizeof(FVF_3DVERTEX) );

}

// 
// vList : 삼각형 3점의 시작포인터. vList[0], [1], [2]
void	CGraphic3D :: Render3DTri( D3DXVECTOR3 *vList, DWORD dwColor, BOOL bShowNormal )
{
	FVF_3DVERTEX	aList[3];

	aList[0].vPos = vList[0];	aList[0].dwColor = dwColor;
	aList[1].vPos = vList[1];	aList[1].dwColor = dwColor;
	aList[2].vPos = vList[2];	aList[2].dwColor = dwColor;

	m_pd3dDevice->SetFVF( D3DFVF_3DVERTEX );
	m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, 1, aList, sizeof(FVF_3DVERTEX) );
	
	if( bShowNormal )
	{
		D3DXVECTOR3 vPos, v1, v2;
		vPos = (vList[0] + vList[1] + vList[2]) / 3;
		v1 = vList[1] - vList[0];		// 노말 계산 시작.
		v2 = vList[2] - vList[0];
		D3DXVec3Cross( &v1, &v1, &v2 );		// 바닥의 노말 계산.
		D3DXVec3Normalize( &v1, &v1 );		// 단위벡터로 변환
		v1 += vPos;
		Render3DLine( vPos, v1, COL_WHITE );
	}
}

void	CGraphic3D :: RenderAABB( D3DXVECTOR3 &vMin, D3DXVECTOR3 &vMax, DWORD dwColor )
{
	FVF_3DVERTEX	aList[24];
	D3DXVECTOR3		vAABB[8];

	vAABB[0].x = vMin.x;
	vAABB[0].y = vMax.y;
	vAABB[0].z = vMin.z;

	vAABB[1].x = vMax.x;
	vAABB[1].y = vMax.y;
	vAABB[1].z = vMin.z;

	vAABB[2].x = vMax.x;
	vAABB[2].y = vMax.y;
	vAABB[2].z = vMax.z;

	vAABB[3].x = vMin.x;
	vAABB[3].y = vMax.y;
	vAABB[3].z = vMax.z;

	vAABB[4].x = vMin.x;
	vAABB[4].y = vMin.y;
	vAABB[4].z = vMin.z;

	vAABB[5].x = vMax.x;
	vAABB[5].y = vMin.y;
	vAABB[5].z = vMin.z;

	vAABB[6].x = vMax.x;
	vAABB[6].y = vMin.y;
	vAABB[6].z = vMax.z;

	vAABB[7].x = vMin.x;
	vAABB[7].y = vMin.y;
	vAABB[7].z = vMax.z;

	aList[0].vPos = vAABB[0];
	aList[1].vPos = vAABB[1];
	aList[2].vPos = vAABB[1];
	aList[3].vPos = vAABB[2];
	aList[4].vPos = vAABB[2];
	aList[5].vPos = vAABB[3];
	aList[6].vPos = vAABB[3];
	aList[7].vPos = vAABB[0];

	aList[8].vPos = vAABB[4];
	aList[9].vPos = vAABB[5];
	aList[10].vPos = vAABB[5];
	aList[11].vPos = vAABB[6];
	aList[12].vPos = vAABB[6];
	aList[13].vPos = vAABB[7];
	aList[14].vPos = vAABB[7];
	aList[15].vPos = vAABB[4];

	aList[16].vPos = vAABB[0];
	aList[17].vPos = vAABB[4];
	aList[18].vPos = vAABB[1];
	aList[19].vPos = vAABB[5];
	aList[20].vPos = vAABB[2];
	aList[21].vPos = vAABB[6];
	aList[22].vPos = vAABB[3];
	aList[23].vPos = vAABB[7];

	for( int i = 0; i < 24; i ++ )
		aList[i].dwColor = dwColor;

	m_pd3dDevice->SetFVF( D3DFVF_3DVERTEX );
	m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 12, aList, sizeof(FVF_3DVERTEX) );
}

//
//	3차원 공간상에 Y=0인 그리드를 만든다
//
void	CGraphic3D :: RenderGrid( DWORD dwColor )
{
	FVF_3DVERTEX	*aList = m_aVertex;
	int		i;
	int		nCnt = 0;

	// 세로선
	for( i = 0; i < 21; i ++ )
	{
		aList[ nCnt ].vPos.x = (float)(-10 + i * 1);	// 시작
		aList[ nCnt ].vPos.z = (float)10;
		aList[ nCnt ].vPos.y = 0;
		aList[ nCnt ].dwColor = dwColor;
		nCnt ++;
		aList[ nCnt ].vPos.x = (float)(-10 + i * 1);	// 끝
		aList[ nCnt ].vPos.z = (float)-10;
		aList[ nCnt ].vPos.y = 0;
		aList[ nCnt ].dwColor = dwColor;
		nCnt ++;
	}
	// 가로선
	for( i = 0; i < 21; i ++ )
	{
		aList[ nCnt ].vPos.x = (float)-10;	// 시작
		aList[ nCnt ].vPos.z = (float)(-10 + i * 1);
		aList[ nCnt ].vPos.y = 0;
		aList[ nCnt ].dwColor = dwColor;
		nCnt ++;
		aList[ nCnt ].vPos.x = (float)10;	// 끝
		aList[ nCnt ].vPos.z = (float)(-10 + i * 1);
		aList[ nCnt ].vPos.y = 0;
		aList[ nCnt ].dwColor = dwColor;
		nCnt ++;
	}

	m_pd3dDevice->SetFVF( D3DFVF_3DVERTEX );

	int nShare = nCnt / 64;
	int nRest  = nCnt % 64;

	for( i = 0; i < nShare; i ++ )
		m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 64, aList + (i * 64), sizeof(FVF_3DVERTEX) );
	if( nRest )
		m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, nRest, aList + (nShare * 64), sizeof(FVF_3DVERTEX) );



}
