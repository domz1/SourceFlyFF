#include "stdafx.h"
#include "Sfx.h"


//  width unit
//	128  1   = 0  1 ~ 1.9
//	64   2   = 1  2 ~ 3 
//	32   4   = 2  4 ~ 7
//	16   8   = 3  8 ~ 15  
//	8    16  = 4
//	4    32  = 5
//	2    64  = 6
//	1    128 = 7 128 ~ 255

int CLandscape::m_nWidthLinkMap[ MAX_LINKLEVEL ];
 
/*
	100 0 "water_riverAni0039.dds" 0 ""
    101 0 "water_riverAni0040.dds" 0 ""
    102 0 "water_riverAni0041.dds" 0 ""
    103 0 "water_riverAni0042.dds" 0 ""
    104 0 "water_riverAni0043.dds" 0 ""
    105 0 "water_riverAni0044.dds" 0 ""
    106 0 "water_riverAni0045.dds" 0 ""
    107 0 "water_riverAni0046.dds" 0 ""
    108 0 "water_riverAni0047.dds" 0 ""
    109 0 "water_riverAni0048.dds" 0 ""
    110 0 "water_riverAni0049.dds" 0 ""
    111 0 "water_riverAni0050.dds" 0 ""
    112 0 "water_riverAni0051.dds" 0 ""
    113 0 "water_riverAni0052.dds" 0 ""
    114 0 "water_riverAni0053.dds" 0 ""
    115 0 "water_riverAni0054.dds" 0 ""
*/
#define D3DCOLOR_ARGB16( a, r, g, b ) ( ( a >> 4 ) << 12 ) | ( ( r >> 4 ) << 8 ) | ( ( g >> 4 ) << 4 ) | ( b >> 4 )

FLOAT CLandscape::m_fCloud_u1 = 0.0f;
FLOAT CLandscape::m_fCloud_v1 = 0.0f;
FLOAT CLandscape::m_fCloud_u2 = 0.0f;
FLOAT CLandscape::m_fCloud_v2 = 0.0f;

#if __VER < 14 // __WATER_EXT
FLOAT CLandscape::m_fWaterFrame = 0.0f;
#endif //__WATER_EXT

DWORD LODLEVELRANGE1 = (32*MPU);
DWORD LODLEVELRANGE2 = (64*MPU);
DWORD LODLEVELRANGE3 = (128*MPU);

void SetLODDetail( int nDetail )
{
	switch( nDetail ) 
	{
	case 0:
		LODLEVELRANGE1 = (64*MPU);
		LODLEVELRANGE2 = (128*MPU);
		LODLEVELRANGE3 = (256*MPU);
		break;
	case 1:
		LODLEVELRANGE1 = (32*MPU);
		LODLEVELRANGE2 = (64*MPU);
		LODLEVELRANGE3 = (128*MPU);
		break;
	case 2:
		LODLEVELRANGE1 = (16*MPU);
		LODLEVELRANGE2 = (32*MPU);
		LODLEVELRANGE3 = (64*MPU);
		break;
	}
}
// -------------------------------------------------------------------------------------------------
//	PATCH CLASS
// -------------------------------------------------------------------------------------------------

CPatch::~CPatch()
{
}

void CPatch::Init( int heightX, int heightY, int worldX, int worldY, FLOAT *hMap )
{
	m_nWorldX = worldX;
	m_nWorldY = worldY;

	m_pHeightMap = &hMap[heightY * (MAP_SIZE+1) + heightX];

	m_bVisible=FALSE;
	m_bDirty=TRUE;
	CalculateBound();
}

void CPatch::Render(LPDIRECT3DDEVICE9 pd3dDevice,int X,int Y)
{
	if( pd3dDevice ) 
	{
		DWORD nBaseVertexIndex = ((PATCH_SIZE+1)*(PATCH_SIZE+1)) * (Y*NUM_PATCHES_PER_SIDE+X);
		pd3dDevice->SetIndices( g_pIB );
		if( CWorld::m_bViewLODTerrain ) 
		{
			if(m_nLevel==3) 
			{
				pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, nBaseVertexIndex, 0, (PATCH_SIZE+1)*(PATCH_SIZE+1),g_anStartIndex[m_nLevel], 2 );
			}
			else 
			{
				if(m_nLevel!=m_nTopLevel)
					pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, nBaseVertexIndex, 0,(PATCH_SIZE+1)*(PATCH_SIZE+1),g_anStartIndex[m_nLevel]+(g_anPrimitive[m_nLevel]*4+g_anPrimitiveA[m_nLevel]*0)*3, g_anPrimitiveA[m_nLevel] );
				else
					pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, nBaseVertexIndex, 0,(PATCH_SIZE+1)*(PATCH_SIZE+1),g_anStartIndex[m_nLevel], g_anPrimitive[m_nLevel] );
					//pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, nBaseVertexIndex, 0,(PATCH_SIZE+1)*(PATCH_SIZE+1),g_anStartIndex[m_nLevel]+(g_anPrimitive[m_nLevel]*0)*3, g_anPrimitive[m_nLevel] );
				if(m_nLevel!=m_nLeftLevel)
					pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, nBaseVertexIndex, 0,(PATCH_SIZE+1)*(PATCH_SIZE+1),g_anStartIndex[m_nLevel]+(g_anPrimitive[m_nLevel]*4+g_anPrimitiveA[m_nLevel]*1)*3, g_anPrimitiveA[m_nLevel] );
				else
					pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, nBaseVertexIndex, 0,(PATCH_SIZE+1)*(PATCH_SIZE+1),g_anStartIndex[m_nLevel]+(g_anPrimitive[m_nLevel]*1)*3, g_anPrimitive[m_nLevel] );
				if(m_nLevel!=m_nRightLevel)
					pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, nBaseVertexIndex, 0,(PATCH_SIZE+1)*(PATCH_SIZE+1),g_anStartIndex[m_nLevel]+(g_anPrimitive[m_nLevel]*4+g_anPrimitiveA[m_nLevel]*2)*3, g_anPrimitiveA[m_nLevel] );
				else
					pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, nBaseVertexIndex, 0,(PATCH_SIZE+1)*(PATCH_SIZE+1),g_anStartIndex[m_nLevel]+(g_anPrimitive[m_nLevel]*2)*3, g_anPrimitive[m_nLevel] );
				if(m_nLevel!=m_nBottomLevel)
					pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, nBaseVertexIndex, 0,(PATCH_SIZE+1)*(PATCH_SIZE+1),g_anStartIndex[m_nLevel]+(g_anPrimitive[m_nLevel]*4+g_anPrimitiveA[m_nLevel]*3)*3, g_anPrimitiveA[m_nLevel] );
				else
					pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, nBaseVertexIndex, 0,(PATCH_SIZE+1)*(PATCH_SIZE+1),g_anStartIndex[m_nLevel]+(g_anPrimitive[m_nLevel]*3)*3, g_anPrimitive[m_nLevel] );
			}
		}
		else 
		{
			pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, nBaseVertexIndex, 0, (PATCH_SIZE+1)*(PATCH_SIZE+1),0, 128 );
		}
	}
}

void CPatch::CalculateBound()
{
	float maxy=-9999999.0f;
	float miny=9999999.0f;
	for(int i=0;i<=PATCH_SIZE;i++)
		for(int j=0;j<=PATCH_SIZE;j++)
		{
			float y = (float)m_pHeightMap[(i *(MAP_SIZE+1))+j ];
			// extra height
			if( y >= HGT_NOWALK )
			{
				if( y >= HGT_DIE )
					y -= HGT_DIE;
				else
				if( y >= HGT_NOMOVE )
					y -= HGT_NOMOVE;
				else
				if( y >= HGT_NOFLY )
					y -= HGT_NOFLY;
				else
					y -= HGT_NOWALK;
			}
			if(y>maxy) maxy=y;
			if(y<miny) miny=y;
		}
	m_avBounds[0] = D3DXVECTOR3( (float)(m_nWorldX) * MPU, miny, (float)(m_nWorldY) * MPU ); // xyz
	m_avBounds[1] = D3DXVECTOR3( (float)(m_nWorldX+PATCH_SIZE) * MPU, miny, (float)(m_nWorldY) * MPU ); // Xyz
	m_avBounds[2] = D3DXVECTOR3( (float)(m_nWorldX) * MPU, maxy, (float)(m_nWorldY) * MPU ); // xYz
	m_avBounds[3] = D3DXVECTOR3( (float)(m_nWorldX+PATCH_SIZE) * MPU, maxy, (float)(m_nWorldY) * MPU ); // XYz
	m_avBounds[4] = D3DXVECTOR3( (float)(m_nWorldX) * MPU, miny, (float)(m_nWorldY+PATCH_SIZE) * MPU ); // xyZ
	m_avBounds[5] = D3DXVECTOR3( (float)(m_nWorldX+PATCH_SIZE) * MPU, miny, (float)(m_nWorldY+PATCH_SIZE) * MPU ); // XyZ
	m_avBounds[6] = D3DXVECTOR3( (float)(m_nWorldX) * MPU, maxy, (float)(m_nWorldY+PATCH_SIZE) * MPU ); // xYZ
	m_avBounds[7] = D3DXVECTOR3( (float)(m_nWorldX+PATCH_SIZE) * MPU, maxy, (float)(m_nWorldY+PATCH_SIZE) * MPU ); // XYZ

	m_vCenter=(m_avBounds[0]+m_avBounds[1]+m_avBounds[2]+m_avBounds[3]+m_avBounds[4]+m_avBounds[5]+m_avBounds[6]+m_avBounds[7])/8;

	D3DXPlaneFromPoints( &m_aplaneBounds[0], &m_avBounds[0], 
		&m_avBounds[1], &m_avBounds[2] ); // Near
	D3DXPlaneFromPoints( &m_aplaneBounds[1], &m_avBounds[6], 
		&m_avBounds[7], &m_avBounds[5] ); // Far
	D3DXPlaneFromPoints( &m_aplaneBounds[2], &m_avBounds[2], 
		&m_avBounds[6], &m_avBounds[4] ); // Left
	D3DXPlaneFromPoints( &m_aplaneBounds[3], &m_avBounds[7], 
		&m_avBounds[3], &m_avBounds[5] ); // Right
	D3DXPlaneFromPoints( &m_aplaneBounds[4], &m_avBounds[2], 
		&m_avBounds[3], &m_avBounds[6] ); // Top
	D3DXPlaneFromPoints( &m_aplaneBounds[5], &m_avBounds[1], 
		&m_avBounds[0], &m_avBounds[4] ); // Bottom
}

void CPatch::Cull()
{
    BYTE bOutside[8];
    ZeroMemory( &bOutside, sizeof(bOutside) );
    for( int iPoint = 0; iPoint < 8; iPoint++ )
    {
        for( int iPlane = 0; iPlane < 6; iPlane++ )
        {
            if( g_cullinfo.planeFrustum[iPlane].a * m_avBounds[iPoint].x +
                g_cullinfo.planeFrustum[iPlane].b * m_avBounds[iPoint].y +
                g_cullinfo.planeFrustum[iPlane].c * m_avBounds[iPoint].z +
                g_cullinfo.planeFrustum[iPlane].d < 0)
            {
                bOutside[iPoint] |= (1 << iPlane);
            }
        }
        if( bOutside[iPoint] == 0 )
		{
			m_bVisible=TRUE;
			CalculateLevel();
            return ;
		}
    }
    if( (bOutside[0] & bOutside[1] & bOutside[2] & bOutside[3] & 
        bOutside[4] & bOutside[5] & bOutside[6] & bOutside[7]) != 0 )
    {
		m_bVisible=FALSE;
        return ;
    }
	m_bVisible=TRUE;
	if(CWorld::m_bViewLODTerrain) CalculateLevel();
}

void CPatch::CalculateLevel()
{
	float dist=D3DXVec3Length(&(CWorld::m_pCamera->m_vPos-m_vCenter));
	if(dist>LODLEVELRANGE3) m_nLevel=3;
	else if(dist>LODLEVELRANGE2) m_nLevel=2;
	else if(dist>LODLEVELRANGE1) m_nLevel=1;
	else m_nLevel=0;
}



// -------------------------------------------------------------------------------------------------
//	CLandscape CLASS
// -------------------------------------------------------------------------------------------------

CLandscape::CLandscape()
{
	m_dwVersion = 0;
	m_pd3dDevice = NULL;
	m_pHeightMap = NULL;
	m_pVB=NULL;
	m_pWorld = NULL;
	m_pCloudVertexBuffer = NULL;
	m_pWaterVertexBuffer = NULL;
//	m_pHgtAttrVertexBuffer = NULL;
	m_nCloudVertexNum = 0;
	m_nWaterVertexNum = 0;
	m_bVisible=TRUE;

	//	버텍스버퍼를 그림의 종류마다 만들어준다. WATERVERTEXBUFFER, * LPWATERVB new를 하라는 말이다.
	m_pWaterVB = new WATERVERTEXBUFFER[ prj.m_terrainMng.m_nWaterFrame ];
	for ( int loop = 0 ; loop < prj.m_terrainMng.m_nWaterFrame ; loop++ )
		ZeroMemory( &m_pWaterVB[ loop ], sizeof( WATERVERTEXBUFFER ) );
	
	//ZeroMemory( m_aObjLinkMap, sizeof(CObj*) * MAP_SIZE * MAP_SIZE );

	// 128 -> 32
	// 32  -> 16
	// 8   -> 4
	// 2   -> 0 -> 1
	int nWidth	= MAP_SIZE * 2;
	for( int i = 0; i < MAX_LINKLEVEL; i++ )
	{
		nWidth /= 2;
		if( nWidth == 0 ) nWidth = 1;
		// 128	// 64	// 32	// 16 // 8 // 4  -	// 4	// 8	// 16	// 32 // 64 // 128
		for( int j = 0; j < MAX_LINKTYPE; j++ )
		{
			m_apObjLink[j][i]	= new CObj*[nWidth*nWidth];
			memset( m_apObjLink[j][i], 0, sizeof(CObj*) * nWidth*nWidth );
		}
		m_nWidthLinkMap[i] = nWidth;
	}
	for( i = 0; i < MAX_OBJARRAY; i++)
	{
		m_apObject[ i ] = new CObj * [ 5000 ];
		m_adwObjNum[ i ] = 0;
	}

}
CLandscape::~CLandscape()
{
	FreeTerrain();
	SAFE_DELETE_ARRAY( m_pWaterVB );
	// 오브젝트 파괴 
	for( int i = 0; i < MAX_OBJARRAY; i++)
	{
		CObj** apObject = m_apObject[ i ];
		for( int j = 0; j < int( m_adwObjNum[ i ] ); j++ )
		{
			if( m_pWorld->m_pObjFocus == apObject[ j ] )
				m_pWorld->SetObjFocus( NULL );
			if( CObj::m_pObjHighlight == apObject[ j ] )
				CObj::m_pObjHighlight = NULL;
			SAFE_DELETE( apObject[ j ] );
		}
		SAFE_DELETE_ARRAY( apObject );
	}
	for( int j = 0; j < MAX_LINKTYPE; j++ ) 
	{
		for( i = 0; i < MAX_LINKLEVEL; i++ ) 
			SAFE_DELETE_ARRAY( m_apObjLink[j][i] );
	}

	for( i = 0; i < m_aLayer.GetSize(); i++) {
		safe_delete( (CLandLayer*)m_aLayer.GetAt(i) );
	}
}

//
// 디바이스 관련
//
HRESULT CLandscape::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice, CWorld* pWorld )
{
	m_pd3dDevice = pd3dDevice;
	m_pWorld = pWorld;

	FreeTerrain();
	m_pHeightMap = (FLOAT *)malloc( (MAP_SIZE+1) * (MAP_SIZE+1) * sizeof(FLOAT) );
	ZeroMemory( m_aWaterHeight, NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE * sizeof( WATERHEIGHT) );
	
	return S_OK;
}

HRESULT CLandscape::RestoreDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice)
{
	if( m_pVB ) 
		return S_OK;

	if(pd3dDevice) 
		pd3dDevice->CreateVertexBuffer( (PATCH_SIZE+1) * (PATCH_SIZE+1) * (NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE) *sizeof(D3DLANDSCAPEVERTEX),
										  D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &m_pVB , NULL);
	
	CPatch *patch;
	m_bDirty=TRUE;
	for ( int Y=0; Y < NUM_PATCHES_PER_SIDE; Y++)
	{
		for ( int X=0; X < NUM_PATCHES_PER_SIDE; X++ )
		{
			patch = &(m_aPatches[Y][X]);
			patch->Init( X*PATCH_SIZE, Y*PATCH_SIZE, m_nWorldX+X*PATCH_SIZE, m_nWorldY+Y*PATCH_SIZE, m_pHeightMap );
		}
	}
	CalculateBound();
	pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );		
	pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );		

	for( int i = 0; i < MAX_OBJARRAY; i++)
	{
		CObj** apObject = m_apObject[ i ];
		for( int j = 0; j < int( m_adwObjNum[ i ] ); j++ )
		{
			if( apObject[ j ] && apObject[ j ]->m_pModel->IsAniable() )
				apObject[ j ]->m_pModel->RestoreDeviceObjects();
		}
	}
	// 물 버텍스 버퍼 만들기 
	MakeWaterVertexBuffer();
	MakeHgtAttrVertexBuffer();
	MakeLandAttrVertexBuffer();

	return S_OK;
}
HRESULT CLandscape::MakeWaterVertexBuffer()
{
#ifdef __FULLWORLD 
	return S_OK;	// 물,구름,높이속성,위치정보의 버텍스를 만들지 않는다.
#endif
	SAFE_RELEASE( m_pWaterVertexBuffer );
	SAFE_RELEASE( m_pCloudVertexBuffer );
	// 물 버텍스 버퍼 만들기 
	WATERVERTEX* pVerticesCloud;
	m_nWaterVertexNum = 0;
	m_nCloudVertexNum = 0;
	
	for ( int loop = 0 ; loop < prj.m_terrainMng.m_nWaterFrame ; loop++ )
	{
		SAFE_RELEASE( m_pWaterVB[ loop ].pVB );
		m_pWaterVB[ loop ].WaterVertexNum = 0;
	}
	WATERVERTEX**	pVAWater;
	byte byWaterTexture;
	pVAWater = new WATERVERTEX*[ prj.m_terrainMng.m_nWaterFrame ];
	for( int i = 0; i < NUM_PATCHES_PER_SIDE; i++ )
	{
		for( int j = 0; j < NUM_PATCHES_PER_SIDE; j++ )
		{
			byWaterTexture = m_aWaterHeight[ j + i * NUM_PATCHES_PER_SIDE ].byWaterTexture;
			if( ( byWaterTexture & (byte)(~MASK_WATERFRAME)) == WTYPE_WATER )
			{
				m_nWaterVertexNum += 6;
				m_pWaterVB[ (byWaterTexture >> 2) ].WaterVertexNum += 6;
			}
			if( byWaterTexture == WTYPE_CLOUD )
				m_nCloudVertexNum += 6;
		}
	}
	HRESULT hr;

	for ( loop = 0 ; loop < prj.m_terrainMng.m_nWaterFrame ; loop++ )
	{
		if ( !m_pWaterVB[ loop ].WaterVertexNum )
			continue;
		hr = m_pd3dDevice->CreateVertexBuffer( m_pWaterVB[ loop ].WaterVertexNum * sizeof( WATERVERTEX ),
			D3DUSAGE_WRITEONLY, D3DFVF_WATERVERTEX, D3DPOOL_MANAGED, 
			&m_pWaterVB[ loop ].pVB, NULL );
		if ( SUCCEEDED( hr ) )
		{
			hr = m_pWaterVB[ loop ].pVB->Lock( 0, m_pWaterVB[ loop ].WaterVertexNum * sizeof( WATERVERTEX ),
				(void**)&pVAWater[ loop ], 0 );
		}
		if ( FAILED( hr ) )
			return hr;
	}
	if( m_nCloudVertexNum )
	{
		hr = m_pd3dDevice->CreateVertexBuffer( m_nCloudVertexNum * sizeof( WATERVERTEX ),
			D3DUSAGE_WRITEONLY, D3DFVF_WATERVERTEX, D3DPOOL_MANAGED, &m_pCloudVertexBuffer, NULL );
		if( SUCCEEDED( hr ) ) 
			hr = m_pCloudVertexBuffer->Lock( 0, m_nCloudVertexNum * sizeof( WATERVERTEX ) , (void**)&pVerticesCloud, 0 );
		if( FAILED( hr ) ) 
			return hr;
	}
	int nWorldX = m_nWorldX * MPU;
	int nWorldZ = m_nWorldY * MPU;
	for( i = 0; i < NUM_PATCHES_PER_SIDE; i++ )
	{
		for( int j = 0; j < NUM_PATCHES_PER_SIDE; j++ )
		{
			LPWATERHEIGHT lpWaterHeight = &m_aWaterHeight[ j + i * NUM_PATCHES_PER_SIDE ];
			if( ( lpWaterHeight->byWaterTexture & (byte)(~MASK_WATERFRAME) ) == WTYPE_WATER )
			{
#if __VER >= 14 // __WATER_EXT
				int loop = lpWaterHeight->byWaterTexture >> 2;

				if ( loop >= 0 && loop < prj.m_terrainMng.m_nWaterFrame && m_pWaterVB[ loop ].WaterVertexNum > 0)
				{
#else //__WATER_EXT
				for ( loop = 0 ; loop < prj.m_terrainMng.m_nWaterFrame ; loop++ )
				{
					if ( !m_pWaterVB[ loop ].WaterVertexNum )
						continue;
#endif //__WATER_EXT
					pVAWater[ loop ]->p.x   = (FLOAT)( nWorldX + ( j * PATCH_SIZE * MPU ) );
					pVAWater[ loop ]->p.y   = (FLOAT)( lpWaterHeight->byWaterHeight );////96.0f;
					pVAWater[ loop ]->p.z   = (FLOAT)( nWorldZ + ( i * PATCH_SIZE * MPU ) );
					pVAWater[ loop ]->n.x   = 0;
					pVAWater[ loop ]->n.y   = 1.0;
					pVAWater[ loop ]->n.z   = 0;
					pVAWater[ loop ]->u     = 0.0f;
					pVAWater[ loop ]->v     = 0.0f;
					pVAWater[ loop ]->color  = 0xaaaaaaaa;
					pVAWater[ loop ]++;

					pVAWater[ loop ]->p.x   = (FLOAT)( nWorldX + ( j * PATCH_SIZE * MPU ) );
					pVAWater[ loop ]->p.y   = (FLOAT)( lpWaterHeight->byWaterHeight );////96.0f;
					pVAWater[ loop ]->p.z   = (FLOAT)( nWorldZ + ( i * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );
					pVAWater[ loop ]->n.x   = 0;
					pVAWater[ loop ]->n.y   = 1.0;
					pVAWater[ loop ]->n.z   = 0;
					pVAWater[ loop ]->u     = 0.0f;
					pVAWater[ loop ]->v     = 3.0f;
					pVAWater[ loop ]->color  = 0xaaaaaaaa;
					pVAWater[ loop ]++;
					
					pVAWater[ loop ]->p.x   = (FLOAT)( nWorldX + ( j * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );
					pVAWater[ loop ]->p.y   = (FLOAT)( lpWaterHeight->byWaterHeight );////96.0f;
					pVAWater[ loop ]->p.z   = (FLOAT)( nWorldZ + ( i * PATCH_SIZE * MPU ) );
					pVAWater[ loop ]->n.x   = 0;
					pVAWater[ loop ]->n.y   = 1.0;
					pVAWater[ loop ]->n.z   = 0;
					pVAWater[ loop ]->u     = 3.0f;
					pVAWater[ loop ]->v     = 0.0f;
					pVAWater[ loop ]->color  = 0xaaaaaaaa;
					pVAWater[ loop ]++;
					
					///////////////////////////////////////////
					
					pVAWater[ loop ]->p.x   = (FLOAT)( nWorldX + ( j * PATCH_SIZE * MPU ) );
					pVAWater[ loop ]->p.y   = (FLOAT)( lpWaterHeight->byWaterHeight );////96.0f;
					pVAWater[ loop ]->p.z   = (FLOAT)( nWorldZ + ( i * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );
					pVAWater[ loop ]->n.x   = 0;
					pVAWater[ loop ]->n.y   = 1.0;
					pVAWater[ loop ]->n.z   = 0;
					pVAWater[ loop ]->u     = 0.0f;
					pVAWater[ loop ]->v     = 3.0f;
					pVAWater[ loop ]->color  = 0xaaaaaaaa;
					pVAWater[ loop ]++;
					
					pVAWater[ loop ]->p.x   = (FLOAT)( nWorldX + ( j * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );
					pVAWater[ loop ]->p.y   = (FLOAT)( lpWaterHeight->byWaterHeight );////96.0f;
					pVAWater[ loop ]->p.z   = (FLOAT)( nWorldZ + ( i * PATCH_SIZE * MPU ) );
					pVAWater[ loop ]->n.x   = 0;
					pVAWater[ loop ]->n.y   = 1.0;
					pVAWater[ loop ]->n.z   = 0;
					pVAWater[ loop ]->u     = 3.0f;
					pVAWater[ loop ]->v     = 0.0f;
					pVAWater[ loop ]->color  = 0xaaaaaaaa;
					pVAWater[ loop ]++;
					
					pVAWater[ loop ]->p.x   = (FLOAT)( nWorldX + ( j * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );
					pVAWater[ loop ]->p.y   = (FLOAT)( lpWaterHeight->byWaterHeight );////96.0f;
					pVAWater[ loop ]->p.z   = (FLOAT)( nWorldZ + ( i * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );
					pVAWater[ loop ]->n.x   = 0;
					pVAWater[ loop ]->n.y   = 1.0;
					pVAWater[ loop ]->n.z   = 0;
					pVAWater[ loop ]->u     = 3.0f;
					pVAWater[ loop ]->v     = 3.0f;
					pVAWater[ loop ]->color  = 0xaaaaaaaa;
					pVAWater[ loop ]++;
				}
			}
			else
			if( lpWaterHeight->byWaterTexture == WTYPE_CLOUD && m_nCloudVertexNum )
			{
				// 1 3
				// 0 2
				pVerticesCloud->p.x   = (FLOAT)( nWorldX + ( j * PATCH_SIZE * MPU ) );
				pVerticesCloud->p.y   = 0.0f;
				pVerticesCloud->p.z   = (FLOAT)( nWorldZ + ( i * PATCH_SIZE * MPU ) );
				pVerticesCloud->n.x   = 0;
				pVerticesCloud->n.y   = 1.0;
				pVerticesCloud->n.z   = 0;
				pVerticesCloud->u     = 0.0f;
				pVerticesCloud->v     = 0.0f;
				pVerticesCloud->color  = 0xaaaaaaaa;
				pVerticesCloud++;
				
				pVerticesCloud->p.x   = (FLOAT)( nWorldX + ( j * PATCH_SIZE * MPU ) );
				pVerticesCloud->p.y   = 0.0f;
				pVerticesCloud->p.z   = (FLOAT)( nWorldZ + ( i * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );
				pVerticesCloud->n.x   = 0;
				pVerticesCloud->n.y   = 1.0;
				pVerticesCloud->n.z   = 0;
				pVerticesCloud->u     = 0.0f;
				pVerticesCloud->v     = 1.0f;
				pVerticesCloud->color  = 0xaaaaaaaa;
				pVerticesCloud++;
				
				pVerticesCloud->p.x   = (FLOAT)( nWorldX + ( j * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );
				pVerticesCloud->p.y   = 0.0f;
				pVerticesCloud->p.z   = (FLOAT)( nWorldZ + ( i * PATCH_SIZE * MPU ) );
				pVerticesCloud->n.x   = 0;
				pVerticesCloud->n.y   = 1.0;
				pVerticesCloud->n.z   = 0;
				pVerticesCloud->u     = 1.0f;
				pVerticesCloud->v     = 0.0f;
				pVerticesCloud->color  = 0xaaaaaaaa;
				pVerticesCloud++;
				
				//////////////////////////////////
				
				pVerticesCloud->p.x   = (FLOAT)( nWorldX + ( j * PATCH_SIZE * MPU ) );
				pVerticesCloud->p.y   = 0.0f;
				pVerticesCloud->p.z   = (FLOAT)( nWorldZ + ( i * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );
				pVerticesCloud->n.x   = 0;
				pVerticesCloud->n.y   = 1.0;
				pVerticesCloud->n.z   = 0;
				pVerticesCloud->u     = 0.0f;
				pVerticesCloud->v     = 1.0f;
				pVerticesCloud->color  = 0xaaaaaaaa;
				pVerticesCloud++;
				
				pVerticesCloud->p.x   = (FLOAT)( nWorldX + ( j * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );
				pVerticesCloud->p.y   = 0.0f;
				pVerticesCloud->p.z   = (FLOAT)( nWorldZ + ( i * PATCH_SIZE * MPU ) );
				pVerticesCloud->n.x   = 0;
				pVerticesCloud->n.y   = 1.0;
				pVerticesCloud->n.z   = 0;
				pVerticesCloud->u     = 1.0f;
				pVerticesCloud->v     = 0.0f;
				pVerticesCloud->color  = 0xaaaaaaaa;
				pVerticesCloud++;
				
				pVerticesCloud->p.x   = (FLOAT)( nWorldX + ( j * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );
				pVerticesCloud->p.y   = 0.0f;
				pVerticesCloud->p.z   = (FLOAT)( nWorldZ + ( i * PATCH_SIZE * MPU ) + ( PATCH_SIZE * MPU ) );
				pVerticesCloud->n.x   = 0;
				pVerticesCloud->n.y   = 1.0;
				pVerticesCloud->n.z   = 0;
				pVerticesCloud->u     = 1.0f;
				pVerticesCloud->v     = 1.0f;
				pVerticesCloud->color  = 0xaaaaaaaa;
				pVerticesCloud++;
			}
		}
	}
	
	for ( loop = 0 ; loop < prj.m_terrainMng.m_nWaterFrame ; loop++ )
	{
		if ( !m_pWaterVB[ loop ].WaterVertexNum )
			continue;
		m_pWaterVB[ loop ].pVB->Unlock();
	}
	
	if( m_nCloudVertexNum )
		m_pCloudVertexBuffer->Unlock();

	SAFE_DELETE_ARRAY( pVAWater );
	return S_OK;
}
HRESULT CLandscape::MakeLandAttrVertexBuffer()
{
	return S_OK;
}

HRESULT CLandscape::MakeHgtAttrVertexBuffer()
{
	return S_OK;
}

HRESULT CLandscape::InvalidateDeviceObjects()
{
	SAFE_RELEASE( m_pVB );
	SAFE_RELEASE( m_pCloudVertexBuffer );// = NULL;
	SAFE_RELEASE( m_pWaterVertexBuffer );// = NULL;
	for ( int loop =0 ; loop < prj.m_terrainMng.m_nWaterFrame ; loop++ )
		SAFE_RELEASE( m_pWaterVB[ loop ].pVB );

	
	// 매모리풀이 default이므로 비디오메모리에 잡힌다. 따라서 
	// InvalidateDeviceObjects에서 파괴해야한다.
	//SAFE_RELEASE( m_texMiniMap.m_pTexture );
	for( int i = 0; i < MAX_OBJARRAY; i++)
	{
		CObj** apObject = m_apObject[ i ];
		for( int j = 0; j < int( m_adwObjNum[ i ] ); j++ )
		{
			if( apObject[ j ] && apObject[ j ]->m_pModel->IsAniable() )
				apObject[ j ]->m_pModel->InvalidateDeviceObjects();
		}
	}
	return S_OK;
}
HRESULT CLandscape::DeleteDeviceObjects()
{
	SAFE_RELEASE( m_texMiniMap.m_pTexture );
	for( int i = 0; i < MAX_OBJARRAY; i++)
	{
		CObj** apObject = m_apObject[ i ];
		for( int j = 0; j < int( m_adwObjNum[ i ] ); j++ )
		{
			if( apObject[ j ] && apObject[ j ]->m_pModel->IsAniable() )
				apObject[ j ]->m_pModel->DeleteDeviceObjects();
		}
	}
	for( i = 0; i < m_aLayer.GetSize(); i++ ) 
	{
		CLandLayer* pLandLayer = (CLandLayer*)m_aLayer.GetAt( i );
		SAFE_RELEASE( pLandLayer->m_pLightMap );
	}
	return S_OK;
}

//
// 높이 맵 초기화 등
//
void CLandscape::FreeTerrain()
{
	if( m_pHeightMap )
	{
		free( m_pHeightMap );
		m_pHeightMap = NULL;
	}
}
void CLandscape::ResetTerrain(DWORD dwInitHeight, BYTE* pHeightMap)
{
	int i,j;
	if(pHeightMap) 
	{
		for( i = 0; i < (MAP_SIZE+1) * (MAP_SIZE+1); i++ )
		{
			m_pHeightMap[ i ] = (FLOAT)(pHeightMap[i]);
		}
	}
	else 
	{
		for( i = 0; i < (MAP_SIZE+1) * (MAP_SIZE+1); i++ )
			m_pHeightMap[ i ] = (FLOAT)dwInitHeight;
	}

	for(i=0;i<NUM_PATCHES_PER_SIDE;i++)
	{
		for(j=0;j<NUM_PATCHES_PER_SIDE;j++)
			m_aPatches[i][j].SetDirty(TRUE);
	}
	m_bDirty = TRUE;

}


//
// 렌더 관련
//
void CLandscape::SetVertices()
{
	CPatch *patch;
	D3DLANDSCAPEVERTEX* pVB;
	if(m_pVB) 
	{
		m_pVB->Lock( 0, 0, (void**)&pVB, 0 );
		int nV=0;
		FLOAT fTemp=1.0f/MAP_SIZE;
		FLOAT fTemp2=fTemp*(PATCH_SIZE-1)*NUM_PATCHES_PER_SIDE/MAP_SIZE;
		D3DXVECTOR3 v1,v2,v3,v4,vTemp,vTempPos,vTempNormal;
		for ( int Y = 0; Y < NUM_PATCHES_PER_SIDE; Y++)
		{
			for ( int X = 0; X < NUM_PATCHES_PER_SIDE; X++ )
			{
				patch = &(m_aPatches[ Y ][ X ]);
				if( patch->isDirty() ) {
					patch->SetDirty( FALSE );
					for( int i = 0; i < PATCH_SIZE + 1; i++ )
					{
						for( int j = 0; j < PATCH_SIZE + 1; j++ )
						{
							float fHeight  = GetHeightMap( ( ( i + Y * PATCH_SIZE ) * ( MAP_SIZE + 1 ) ) + ( j + X * PATCH_SIZE ) );
							vTempPos=pVB[nV].p=D3DXVECTOR3((float) ( X*PATCH_SIZE+j+m_nWorldX ) * MPU,fHeight,(float) ( Y * PATCH_SIZE + i + m_nWorldY ) * MPU );
							//if() // ( i % 8 ) && !( j % 8 ) )
							{
								vTempNormal=D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
								if((j-1+X*PATCH_SIZE)>0)
									v1=D3DXVECTOR3((float) (X*PATCH_SIZE+j-1+m_nWorldX) * MPU,(float)GetHeightMap(((i+Y*PATCH_SIZE)*(MAP_SIZE+1))+(j-1+X*PATCH_SIZE)),(float) (Y*PATCH_SIZE+i+m_nWorldY) * MPU);
								if((i+1+Y*PATCH_SIZE)<MAP_SIZE+1)
									v2=D3DXVECTOR3((float) (X*PATCH_SIZE+j+m_nWorldX) * MPU,(float)GetHeightMap(((i+1+Y*PATCH_SIZE)*(MAP_SIZE+1))+(j+X*PATCH_SIZE)),(float) (Y*PATCH_SIZE+i+1+m_nWorldY) * MPU);
								if((j+1+X*PATCH_SIZE)<MAP_SIZE+1)
									v3=D3DXVECTOR3((float) (X*PATCH_SIZE+j+1+m_nWorldX) * MPU,(float)GetHeightMap(((i+Y*PATCH_SIZE)*(MAP_SIZE+1))+(j+1+X*PATCH_SIZE)),(float) (Y*PATCH_SIZE+i+m_nWorldY) * MPU);
								if((i-1+Y*PATCH_SIZE)>0)
									v4=D3DXVECTOR3((float) (X*PATCH_SIZE+j+m_nWorldX) * MPU,(float)GetHeightMap(((i-1+Y*PATCH_SIZE)*(MAP_SIZE+1))+(j+X*PATCH_SIZE)),(float) (Y*PATCH_SIZE+i-1+m_nWorldY) * MPU);

								if( (j-1+X*PATCH_SIZE)>0 && (i+1+Y*PATCH_SIZE)<MAP_SIZE+1 ) {
									D3DXVec3Cross(&vTemp,&(vTempPos-v1),&(v1-v2));
									vTempNormal+=vTemp;
								}
								if( (i+1+Y*PATCH_SIZE)<MAP_SIZE+1 && (j+1+X*PATCH_SIZE)<MAP_SIZE+1 ) {
									D3DXVec3Cross(&vTemp,&(vTempPos-v2),&(v2-v3));
									vTempNormal+=vTemp;
								}
								if( (j+1+X*PATCH_SIZE)<MAP_SIZE+1 && (i-1+Y*PATCH_SIZE)>0 ) {
									D3DXVec3Cross(&vTemp,&(vTempPos-v3),&(v3-v4));
									vTempNormal+=vTemp;
								}
								if( (i-1+Y*PATCH_SIZE)>0 && (j-1+X*PATCH_SIZE)>0 ) {
									D3DXVec3Cross(&vTemp,&(vTempPos-v4),&(v4-v1));
									vTempNormal+=vTemp; 
								}
								D3DXVec3Normalize(&(vTempNormal),&(vTempNormal));
								pVB[nV].n=vTempNormal;
							}
		/*
							float alpha=(float)(j+m_nWorldX)/PATCH_SIZE;
							float alpha1;
							alpha1= ((int)alpha)%2 + (alpha-(int)alpha);
							alpha=(alpha1<1.0f)?alpha1:2.0f-alpha1;
							alpha=1.0f;
		*/
							/*
							if( GetLandMark(X,Y)) {
								pVB[nV].tu1=(float)( (float)((FLOAT)j)/(PATCH_SIZE) );
								pVB[nV].tv1=(float)( (float)((FLOAT)i)/(PATCH_SIZE) );
								pVB[nV].tu2=(float)( (float)(j*fTemp2) + ((float)X*PATCH_SIZE/MAP_SIZE) + (fTemp/2) ) * 1;
								pVB[nV].tv2=(float)( (float)(i*fTemp2) + ((float)Y*PATCH_SIZE/MAP_SIZE) + (fTemp/2) ) * 1;
							}
							else
							*/
							{
								pVB[nV].tu1=(float)( (float)((FLOAT)j)/(PATCH_SIZE) ) * 3;//MPU;
								pVB[nV].tv1=(float)( (float)((FLOAT)i)/(PATCH_SIZE) ) * 3;//MPU;
								pVB[nV].tu2=(float) (fTemp2*(j+X*(PATCH_SIZE)) + (fTemp/2) ) * 1;
								pVB[nV].tv2=(float) (fTemp2*(i+Y*(PATCH_SIZE)) + (fTemp/2) ) * 1;

								/*
								pVB[nV].tu1=(float)( (float)((FLOAT)j)/(PATCH_SIZE) ) * MPU;
								pVB[nV].tv1=(float)( (float)((FLOAT)i)/(PATCH_SIZE) ) * MPU;
								pVB[nV].tu2=(float) (fTemp2*(j+X*(PATCH_SIZE)) + (fTemp/2) ) * 1;
								pVB[nV].tv2=(float) (fTemp2*(i+Y*(PATCH_SIZE)) + (fTemp/2) ) * 1;
								*/
							}
							nV++;
						}
					}
				}
				else nV+=(PATCH_SIZE+1)*(PATCH_SIZE+1);
			}
		}
		m_pVB->Unlock();
	}
	m_bDirty=FALSE;
}

#if __VER >= 13 // __HOUSING
BOOL CLandscape::ForceTexture(LPDIRECT3DTEXTURE9 pNewTex)
{

	CLandLayer* pLayer;


	for(int i = 0; i < m_aLayer.GetSize(); i++ ) 
	{
		pLayer = (CLandLayer*)(m_aLayer[i]);

		if( pLayer->m_bVisible == FALSE )
			continue;

		prj.m_terrainMng.GetTerrain( pLayer->m_nTex )->m_pTexture = pNewTex;
	}	

	return TRUE;
}
#endif // __HOUSING

void CLandscape::RenderPatches()
{
	int X, Y;
	CPatch* patch;
	CLandLayer* pLayer;
	int nBlendStatus=0;
	
	for(int i = 0; i < NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE; i++ )
		m_abPatchRendered[ i ] = FALSE;

	for( i = 0; i < m_aLayer.GetSize(); i++ ) 
	{
		pLayer = (CLandLayer*)(m_aLayer[i]);

		if( pLayer->m_bVisible == FALSE )
			continue;

		if( prj.m_terrainMng.GetTerrain( pLayer->m_nTex )->m_pTexture == NULL )
			prj.m_terrainMng.LoadTexture( pLayer->m_nTex );

		if( prj.m_terrainMng.GetTerrain( pLayer->m_nTex )->m_pTexture == NULL )
			continue;

		m_pd3dDevice->SetTexture( 0, prj.m_terrainMng.GetTerrain( pLayer->m_nTex )->m_pTexture );
		prj.m_terrainMng.GetTerrain( pLayer->m_nTex )->m_pTexture->GetLevelCount();
		m_pd3dDevice->SetTexture( 1, pLayer->m_pLightMap );
		m_pd3dDevice->SetIndices( g_pIB );

		for ( Y=0; Y < NUM_PATCHES_PER_SIDE; Y++)
		{
			for ( X=0; X < NUM_PATCHES_PER_SIDE; X++ )
			{
				patch = &(m_aPatches[Y][X]);
				if( patch->isVisibile() ) 
				{
					if(pLayer->m_aPatchEnable[X+Y*NUM_PATCHES_PER_SIDE] ) 
					{
						if(m_abPatchRendered[X+Y*NUM_PATCHES_PER_SIDE]) 
						{
							if( nBlendStatus == 0 )  
							{
								m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
								nBlendStatus=1;
							}
						}
						else 
						{
							m_abPatchRendered[X+Y*NUM_PATCHES_PER_SIDE]=TRUE;
							if( nBlendStatus == 1 ) 
							{
								m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
								nBlendStatus=0;
							}
						}
						patch->Render(m_pd3dDevice,X,Y);
					}
				}
			}
		}
	}
}
HRESULT CLandscape::Render(LPDIRECT3DDEVICE9 pd3dDevice,BOOL bLod)
{
	/*
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

	pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
//	pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_NONE );
//	pd3dDevice->SetRenderState( D3DRS_FILLMODE,   D3DFILL_WIREFRAME );
*/
	//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
//	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT);
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

//	m_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_DISABLE);
//	m_pd3dDevice->SetTextureStageState( 2, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

    //m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    //m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    //m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	
	//m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_SRCALPHA    );
	//m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	m_pd3dDevice->SetVertexDeclaration( NULL );
	m_pd3dDevice->SetVertexShader( NULL );
	m_pd3dDevice->SetFVF( D3DFVF_D3DLANDSCAPEVERTEX );
	m_pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof( D3DLANDSCAPEVERTEX ) );

	//m_pd3dDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_POINT);
	//m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_POINT );


//	m_pd3dDevice->SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, 2);
	//float fBias=-.7f;
	//m_pd3dDevice->SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS , *((LPDWORD) (&fBias)));
	//m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPMAPLODBIAS , *((LPDWORD) (&fBias)));
	
/*
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,   TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x08 );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL   );		
*/
//	m_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	RenderPatches();

//	m_pd3dDevice->SetRenderState( D3DRS_FILLMODE,   D3DFILL_SOLID );
	//m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	//m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	//m_pd3dDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE);
	//m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );		
	return S_OK;
	
}
//////////////////////////////////////////////////////////////
HRESULT CLandscape::RenderWater( LPDIRECT3DDEVICE9 pd3dDevice )
{
//	return S_OK;
	D3DXMATRIX mat,matView,matProj;
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	D3DXMatrixIdentity( &mat );
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &mat );
/*
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
	m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
*/
	// 랜더 워터 
/*
	int nDivWater1 = m_nWaterFrame / 4;
	int nDivWater2 = m_nWaterFrame % 4;
	mat._31 = ( 1.0f / 4.0f ) * (FLOAT)nDivWater2;  //m_fCloud_u1;
	mat._32 = ( 1.0f / 4.0f ) * (FLOAT)nDivWater1;  //m_fCloud_v1;
	m_pd3dDevice->SetTransform( D3DTS_TEXTURE0, &mat );
	m_nWaterFrame++;
	if( m_nWaterFrame >= 16 )
	m_nWaterFrame = 0;
	m_pd3dDevice->SetTexture( 0, prj.m_terrainMng.GetTerrain( 11 )->m_pTexture );
	*/
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( 200, 0, 0, 0) );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,   D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,   D3DTA_DIFFUSE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2,   D3DTA_TFACTOR);
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	
	m_pd3dDevice->SetVertexShader( NULL );
	m_pd3dDevice->SetFVF ( D3DFVF_WATERVERTEX ); 
	//	이 버텍스 버퍼를 여러개를 만들어서 뿌릴수 있도록 만들어 주어야 한다.

	for ( int i = 0 ; i < prj.m_terrainMng.m_nWaterFrame ; i++)
    {
		if ( !m_pWaterVB[ i ].WaterVertexNum )
			continue;

		m_pd3dDevice->SetStreamSource( 0, m_pWaterVB[ i ].pVB, 0, sizeof(WATERVERTEX) );
		m_pd3dDevice->SetTransform( D3DTS_TEXTURE0, &mat );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );

#if __VER >= 14 // __WATER_EXT
		if(prj.m_terrainMng.m_fWaterFrame[i] >= static_cast<FLOAT>(prj.m_terrainMng.m_pWaterIndexList[i].ListCnt))
			prj.m_terrainMng.m_fWaterFrame[i] = static_cast<FLOAT>(prj.m_terrainMng.m_pWaterIndexList[i].ListCnt - 1);

		//gmpbigsun : 함수분해함, 배열 인덱스가 0보다 작은경우가 발생할 경우 안전조치 
		int a = static_cast<int>(prj.m_terrainMng.m_fWaterFrame[i]);
		if( a > -1 )
		{
			DWORD dwID = prj.m_terrainMng.m_pWaterIndexList[ i ].pList[ a ];
			LPDIRECT3DTEXTURE9 pTexture = prj.m_terrainMng.GetTerrain( dwID )->m_pTexture;
			m_pd3dDevice->SetTexture(0, pTexture );

		//	prj.m_terrainMng.GetTerrain( prj.m_terrainMng.m_pWaterIndexList[ i ].pList[ static_cast<int>(prj.m_terrainMng.m_fWaterFrame[i]) ] )->m_pTexture);  
		}
#else //__WATER_EXT
		if ( m_fWaterFrame > 40)
			m_fWaterFrame = m_fWaterFrame;
		LPDIRECT3DTEXTURE9 Texture = prj.m_terrainMng.GetTerrain( prj.m_terrainMng.m_pWaterIndexList[ i ].pList[ (int)m_fWaterFrame ] )->m_pTexture;
		m_pd3dDevice->SetTexture(0, prj.m_terrainMng.GetTerrain( prj.m_terrainMng.m_pWaterIndexList[ i ].pList[ (int)m_fWaterFrame ] )->m_pTexture);
#endif //__WATER_EXT
		m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, m_pWaterVB[ i ].WaterVertexNum / 3);
    }

	//3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( nTemp * 255 / 10, 0, 0, 0 ) );
	//nt nWaterFrame = m_fWaterFrame + 1;
	//if( nWaterFrame >= 16 ) nWaterFrame = 0;
	//m_pd3dDevice->SetTexture( 0, prj.m_terrainMng.GetTerrain( 20 + nWaterFrame )->m_pTexture );
	//m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, m_nWaterVertexNum/3);
	
/*
	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
	// 시간에 따라 하늘 텍스쳐를 자연스럽게 바꿔치기한다.
	if(g_GameTimer.m_nHour<6) {
		pd3dDevice->SetTexture( 0, m_pSkyBoxTexture3);
		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 56);
	}
	else if(g_GameTimer.m_nHour==6) {
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( 255-(g_GameTimer.m_nMin*255/59), 0, 0, 0 ) );
		pd3dDevice->SetTexture( 0, m_pSkyBoxTexture3);
		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 56);
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( g_GameTimer.m_nMin*255/59, 0, 0, 0 ) );
		pd3dDevice->SetTexture( 0, m_pSkyBoxTexture);
		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 56);
*/
#if __VER >= 14 // __CLOUD_ANIMATION_BUG
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
#endif // __CLOUD_ANIMATION_BUG
	if( m_nCloudVertexNum )
	{
		// 랜더 구름 
		m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

#ifdef __BS_CHANGEABLE_WORLD_SEACLOUD
		if( m_pWorld->m_kSeaCloud._bUse )
			m_pd3dDevice->SetTexture( 0, m_pWorld->m_kSeaCloud._pTexture );
#else
		m_pd3dDevice->SetTexture( 0, prj.m_terrainMng.GetTerrain( 10 )->m_pTexture );
#endif // __BS_CHANGEABLE_WORLD_SEACLOUD
		
		m_pd3dDevice->SetVertexShader( NULL );
		m_pd3dDevice->SetFVF ( D3DFVF_WATERVERTEX );
		m_pd3dDevice->SetStreamSource( 0, m_pCloudVertexBuffer, 0, sizeof( WATERVERTEX ) );

		mat._31 = m_fCloud_u1;
		mat._32 = m_fCloud_v1;
		m_pd3dDevice->SetTransform( D3DTS_TEXTURE0, &mat );
		D3DXMatrixTranslation( &mat, 0.0f, 40.0f, 0.0f );
		m_pd3dDevice->SetTransform( D3DTS_WORLD, &mat );
		m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, m_nCloudVertexNum / 3 );

		mat._31 = m_fCloud_u2;
		mat._32 = m_fCloud_v2;
		m_pd3dDevice->SetTransform( D3DTS_TEXTURE0, &mat );
		D3DXMatrixTranslation( &mat, 0.0f, 80.0f, 0.0f );
		m_pd3dDevice->SetTransform( D3DTS_WORLD, &mat );
		m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, m_nCloudVertexNum / 3 );

		m_fCloud_u1 += 0.001f;
		m_fCloud_v1 += 0.001f;
		m_fCloud_u2 += 0.0015f;
		m_fCloud_v2 += 0.0015f;
	}
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	return S_OK;
}

//
// 컬링 관련
//
void CLandscape::CalculateBound()
{
	if( m_bDirty ) 
	{
		int i,j;
		SetVertices();
		float maxy=-9999999.0f;
		float miny=9999999.0f;

		for(i=0;i<=MAP_SIZE;i++)
			for(j=0;j<=MAP_SIZE;j++)
			{
				float y = (float)GetHeightMap((i *(MAP_SIZE+1))+j );
				if(y>maxy) maxy=y;
				if(y<miny) miny=y;
			}
		m_avBounds[0] = D3DXVECTOR3( (float)(m_nWorldX)*MPU, miny, (float)m_nWorldY * MPU ); // xyz
		m_avBounds[1] = D3DXVECTOR3( (float)(m_nWorldX+MAP_SIZE) * MPU, miny, (float)m_nWorldY * MPU ); // Xyz
		m_avBounds[2] = D3DXVECTOR3( (float)(m_nWorldX) * MPU, maxy, (float)m_nWorldY * MPU ); // xYz
		m_avBounds[3] = D3DXVECTOR3( (float)(m_nWorldX+MAP_SIZE) * MPU, maxy, (float)m_nWorldY * MPU ); // XYz
		m_avBounds[4] = D3DXVECTOR3( (float)(m_nWorldX) * MPU, miny, (float)(m_nWorldY+MAP_SIZE) * MPU); // xyZ
		m_avBounds[5] = D3DXVECTOR3( (float)(m_nWorldX+MAP_SIZE) * MPU, miny, (float)(m_nWorldY+MAP_SIZE) * MPU); // XyZ
		m_avBounds[6] = D3DXVECTOR3( (float)(m_nWorldX) * MPU, maxy, (float)(m_nWorldY+MAP_SIZE) * MPU ); // xYZ
		m_avBounds[7] = D3DXVECTOR3( (float)(m_nWorldX+MAP_SIZE) * MPU, maxy, (float)(m_nWorldY+MAP_SIZE) * MPU); // XYZ

		D3DXPlaneFromPoints( &m_aplaneBounds[0], &m_avBounds[0], 
			&m_avBounds[1], &m_avBounds[2] ); // Near
		D3DXPlaneFromPoints( &m_aplaneBounds[1], &m_avBounds[6], 
			&m_avBounds[7], &m_avBounds[5] ); // Far
		D3DXPlaneFromPoints( &m_aplaneBounds[2], &m_avBounds[2], 
			&m_avBounds[6], &m_avBounds[4] ); // Left
		D3DXPlaneFromPoints( &m_aplaneBounds[3], &m_avBounds[7], 
			&m_avBounds[3], &m_avBounds[5] ); // Right
		D3DXPlaneFromPoints( &m_aplaneBounds[4], &m_avBounds[2], 
			&m_avBounds[3], &m_avBounds[6] ); // Top
		D3DXPlaneFromPoints( &m_aplaneBounds[5], &m_avBounds[1], 
			&m_avBounds[0], &m_avBounds[4] ); // Bottom

		for(i=0;i<NUM_PATCHES_PER_SIDE;i++)
			for(j=0;j<NUM_PATCHES_PER_SIDE;j++)
//				m_aPatches[j][i].CalculateBound();
				m_aPatches[i][j].CalculateBound();
	}
}

void CLandscape::UpdateCull()
{
	Cull();
	if(m_bVisible)
		for(int i=0;i<NUM_PATCHES_PER_SIDE;i++)
			for(int j=0;j<NUM_PATCHES_PER_SIDE;j++)
//				m_aPatches[j][i].Cull();
				m_aPatches[i][j].Cull();
	for(int j=0;j<NUM_PATCHES_PER_SIDE;j++)
		for(int i=0;i<NUM_PATCHES_PER_SIDE;i++)
		{
			if(m_aPatches[j][i].isVisibile()) {
				if(j>0) {
					if(m_aPatches[j][i].m_nLevel<m_aPatches[j-1][i].m_nLevel)
						m_aPatches[j][i].m_nTopLevel=m_aPatches[j-1][i].m_nLevel;
					else m_aPatches[j][i].m_nTopLevel=m_aPatches[j][i].m_nLevel;
				}
				else
					m_aPatches[j][i].m_nTopLevel=m_aPatches[j][i].m_nLevel;

				if(i<NUM_PATCHES_PER_SIDE-1) {
					if(m_aPatches[j][i].m_nLevel<m_aPatches[j][i+1].m_nLevel)
						m_aPatches[j][i].m_nLeftLevel=m_aPatches[j][i+1].m_nLevel;
					else
						m_aPatches[j][i].m_nLeftLevel=m_aPatches[j][i].m_nLevel;
				}
				else
					m_aPatches[j][i].m_nLeftLevel=m_aPatches[j][i].m_nLevel;

				if(i>0) {
					if(m_aPatches[j][i].m_nLevel<m_aPatches[j][i-1].m_nLevel)
						m_aPatches[j][i].m_nRightLevel=m_aPatches[j][i-1].m_nLevel;
					else
						m_aPatches[j][i].m_nRightLevel=m_aPatches[j][i].m_nLevel;
				}
				else
					m_aPatches[j][i].m_nRightLevel=m_aPatches[j][i].m_nLevel;

				if(j<NUM_PATCHES_PER_SIDE-1) {
					if(m_aPatches[j][i].m_nLevel<m_aPatches[j+1][i].m_nLevel)
						m_aPatches[j][i].m_nBottomLevel=m_aPatches[j+1][i].m_nLevel;
					else
						m_aPatches[j][i].m_nBottomLevel=m_aPatches[j][i].m_nLevel;
				}
				else
					m_aPatches[j][i].m_nBottomLevel=m_aPatches[j][i].m_nLevel;
			}
		}
}

void CLandscape::Cull()
{
    BYTE bOutside[8];
    ZeroMemory( &bOutside, sizeof(bOutside) );
    for( int iPoint = 0; iPoint < 8; iPoint++ )
    {
        for( int iPlane = 0; iPlane < 6; iPlane++ )
        {
            if( g_cullinfo.planeFrustum[iPlane].a * m_avBounds[iPoint].x +
                g_cullinfo.planeFrustum[iPlane].b * m_avBounds[iPoint].y +
                g_cullinfo.planeFrustum[iPlane].c * m_avBounds[iPoint].z +
                g_cullinfo.planeFrustum[iPlane].d < 0)
            {
                bOutside[iPoint] |= (1 << iPlane);
            }
        }
        // If any point is inside all 6 frustum planes, it is inside
        // the frustum, so the object must be rendered.
        if( bOutside[iPoint] == 0 )
		{
			m_bVisible=TRUE;
            return ;
		}
    }
    if( (bOutside[0] & bOutside[1] & bOutside[2] & bOutside[3] & 
        bOutside[4] & bOutside[5] & bOutside[6] & bOutside[7]) != 0 )
    {
		m_bVisible=FALSE;
        return ;
    }
	m_bVisible=TRUE;
}

FLOAT CLandscape::GetHeight(FLOAT x,FLOAT z)
{
	if(x<0 || x>MAP_SIZE || z<0 || z>MAP_SIZE) return .0f;
	int px,pz;
	FLOAT dx,dz;
	FLOAT dy1,dy2,dy3,dy4;
	px=(int)x;
	pz=(int)z;
	dx=x-px;
	dz=z-pz;

	FLOAT y1=GetHeightMap(px+pz*(MAP_SIZE+1));
	FLOAT y2=GetHeightMap(px+1+pz*(MAP_SIZE+1));
	FLOAT y3=GetHeightMap(px+(pz+1)*(MAP_SIZE+1));
	FLOAT y4=GetHeightMap(px+1+(pz+1)*(MAP_SIZE+1));
	dy1=y1*(1-dx)*(1-dz);
	dy2=y2*dx*(1-dz);
	dy3=y3*(1-dx)*dz;
	dy4=y4*dx*dz;
	return dy1+dy2+dy3+dy4;
}

FLOAT CLandscape::GetHeight_Fast(FLOAT x,FLOAT z)
{
	if( x < 0 || x > MAP_SIZE || z < 0 || z > MAP_SIZE) 
		return .0f;
	int px,pz;
	FLOAT dx,dz;
	px=(int)x;
	pz=(int)z;
	dx=x-px;
	dz=z-pz;
	return GetHeightMap( px + pz * ( MAP_SIZE + 1 ) );
}

FLOAT CLandscape::GetHeight(DWORD x,DWORD z)
{
	if(x<0 || x>MAP_SIZE || z<0 || z>MAP_SIZE) 
		return .0f;

	return GetHeightMap(x+z*(MAP_SIZE+1));
}

FLOAT CLandscape::GetHeight(POINT pos)
{
	return GetHeight((DWORD)pos.x,(DWORD)pos.y);
}


void CLandscape::SetUsedAllObjects()
{
	for( int i = 0; i < MAX_OBJARRAY; i++ )
	{
		CObj** apObjs = m_apObject[ i ];
		for( int j = 0; j < (int)( m_adwObjNum[ i ] ); j++ )
		{
			if( apObjs[ j ] )
				apObjs[ j ]->m_pModel->m_pModelElem->m_bUsed = TRUE;
		}
	}
}
BOOL CLandscape::LoadLandscape( LPCTSTR lpszFileName, int xx, int yy )
{
	CResFile file;
	if( file.Open( lpszFileName, "rb" ) == FALSE )
		return FALSE;
	ResetTerrain( INIT_HEIGHT );

	int xLand, yLand;
	file.Read( &m_dwVersion, sizeof( DWORD) );
	// 1 버젼일 때는 사이즈 읽어야함
	if( m_dwVersion >= 1 )
	{
		file.Read( &xLand, sizeof( xLand ) );
		file.Read( &yLand, sizeof( yLand ) );
	}
	file.Read( m_pHeightMap, sizeof( FLOAT ), ( MAP_SIZE + 1 ) * ( MAP_SIZE + 1 ) );

	file.Read( m_aWaterHeight, sizeof( WATERHEIGHT ), NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE );
	// 2 버젼일 때는 지역맵 읽어야함
	if( m_dwVersion >= 2 )
	{
		file.Read( m_aLandAttr, sizeof( BYTE ), NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE );
		if( m_dwVersion == 2 )
			ZeroMemory( m_aLandAttr, sizeof( NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE ) );
	}
	else
	{
		ZeroMemory( m_aLandAttr, sizeof( NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE ) );
	}
	BYTE nLayer;
	file.Read( &nLayer, sizeof(BYTE) , 1 );
	CLandLayer* pLayer;
	WORD nTex;
	for(int j = 0; j < nLayer; j++ )  
	{
		file.Read( &( nTex ), sizeof(WORD), 1 );

		pLayer = NewLayer( nTex );
		file.Read( pLayer->m_aPatchEnable, sizeof( BOOL ), NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE );
		D3DLOCKED_RECT rectLock;
		pLayer->m_pLightMap->LockRect( 0, &rectLock, 0, 0 );
#ifdef __16BITLIGHT
		DWORD dwLightMap[ MAP_SIZE*MAP_SIZE ];
		file.Read( dwLightMap, sizeof( DWORD ), MAP_SIZE * MAP_SIZE );
		for( int i = 0; i < MAP_SIZE * MAP_SIZE; i++ )
		{
			((WORD *)rectLock.pBits)[i] = 
			D3DCOLOR_ARGB16
			( 
				( dwLightMap[ i ] & 0xff000000 ) >> ( 24 ), 
				( dwLightMap[ i ] & 0x00ff0000 ) >> ( 16 ),
				( dwLightMap[ i ] & 0x0000ff00 ) >> (  8 ),
				( dwLightMap[ i ] & 0x000000ff )
			);
		}
#else	//!__16BITLIGHT
		file.Read(((DWORD *)rectLock.pBits),sizeof(DWORD),MAP_SIZE*MAP_SIZE);
#endif	//!__16BITLIGHT
		pLayer->m_pLightMap->UnlockRect(0);
	}
	// 해당 레이어에 존재하는 텍스춰가 없을 시에는 삭제 
	for( j = 0; j < nLayer; j++ )  
	{
		CLandLayer* pLayer = (CLandLayer*)m_aLayer.GetAt( j );
		if( prj.m_terrainMng.GetTerrain( pLayer->m_nTex )->m_pTexture == NULL )
			prj.m_terrainMng.LoadTexture( pLayer->m_nTex );
		
		if( prj.m_terrainMng.GetTerrain( pLayer->m_nTex )->m_pTexture == NULL )
		{
			safe_delete( pLayer );
			m_aLayer.RemoveAt( j );
			nLayer--;
			j--;
		}
	}
	MakeHgtAttrVertexBuffer();
	MakeWaterVertexBuffer();
	MakeLandAttrVertexBuffer();

//	CalculateBound();
	

	// Read Objects
	CObj** apObject = m_apObject[ OT_OBJ ];
	DWORD dwObjNum;
	file.Read( &dwObjNum, sizeof( DWORD ), 1 );
	TRACE( "Load Object Num = %d\n", dwObjNum );
	for( int i = 0; i < (int)( dwObjNum ); i++ )
	{
		CObj* pObj = ::ReadObj( &file );
		if( m_dwVersion >= 1 )	
		{
			pObj->m_vPos.x += xx * LANDREALSCALE;
			pObj->m_vPos.z += yy * LANDREALSCALE;
		}
		if( pObj->m_dwIndex  == 0 )
		{
			int a=0;
			a++;

		}
		//pObj->m_vPos += D3DXVECTOR3( 1024, 0, 0 );
		if( pObj->SetIndex( m_pd3dDevice, pObj->m_dwIndex ) == TRUE)
		{
			if( ::GetLanguage() == LANG_JAP && pObj->m_dwIndex == 360 )
			{
				safe_delete( pObj );
				dwObjNum--;
			}
			else
			{
				if( pObj->GetType() == OT_MOVER )
					pObj->InitMotion( ((CMover*)pObj)->m_dwMotion );
				if( InsertObjLink( pObj ) )
				{
					AddObjArray( pObj );
					pObj->m_pWorld = m_pWorld;
					pObj->UpdateLocalMatrix();
				}
				else
				{
					safe_delete( pObj );
					dwObjNum--;
				}
			}
		}
		else
		{
			safe_delete( pObj );
			dwObjNum--;
		}
	}
	size_t size = file.Read( &dwObjNum, sizeof( DWORD ), 1 );
	if( size )
	{
		for( i = 0; i < (int)( dwObjNum ); i++ )
		{
			CSfx* pObj = (CSfx*)::ReadObj( &file );
			if( m_dwVersion >= 1 )	
			{
				pObj->m_vPos.x += xx  * LANDREALSCALE;
				pObj->m_vPos.z += yy * LANDREALSCALE;
			}
			//pObj->m_vPos += D3DXVECTOR3( 1024, 0, 0 );
			if( pObj->SetIndex( m_pd3dDevice, pObj->m_dwIndex ) == TRUE )
			{
				InsertObjLink( pObj );
				AddObjArray( pObj );
				pObj->m_pWorld = m_pWorld;
				pObj->UpdateLocalMatrix();
			}
		}
	}
	// MiniMap 만들기 

	CString strDDSName = lpszFileName;
	strDDSName = strDDSName.Left( strDDSName.GetLength() - 3 );
	strDDSName += "dds";
	m_texMiniMap.LoadTexture( m_pd3dDevice, strDDSName, 0, FALSE );
	return TRUE;
}
//
//
//
/*
	1 = 0  1 ~ 1.9
	2 = 1  2 ~ 3 
	4 = 2  4 ~ 7
	8  = 3  8 ~ 15  
	16 = 4
	32 = 5
	64  = 6
	128 = 7 128 ~ 255
*/
BOOL CLandscape::InsertObjLink( CObj* pObj )
{
	if( pObj->m_pModel == NULL )
		return 1;
	D3DXVECTOR3 vPos = pObj->GetPos();
	vPos.x /= MPU;
	vPos.z /= MPU;
	vPos.x -= m_nWorldX;
	vPos.z -= m_nWorldY;

	CObj** aObjLinkMap	= GetObjLink( pObj->GetLinkType(), pObj->GetLinkLevel() );
	int nMaxWidth = m_nWidthLinkMap[ pObj->GetLinkLevel() ];
	int nUnit = MAP_SIZE / nMaxWidth;
	int nPos = ((int)vPos.z / nUnit ) * nMaxWidth + ((int)vPos.x / nUnit );
	if( nPos >= 0 && nPos < nMaxWidth * nMaxWidth )
	{
		CObj* pBegObj 
			= aObjLinkMap[ nPos ];
		if( pBegObj )
		{

			if( pObj->m_pNext || pObj->m_pPrev )
			{
				TRACE("헉 InsertObjWorld 새로 추가된 오브젝트가 Next와 Prev에 값이 있네?? Type = %d \n", pObj->GetType());
			}
			pBegObj->InsNextNode(pObj);
		}
		else 
		{
			aObjLinkMap[nPos] = pObj;
			if( pObj->m_pNext || pObj->m_pPrev )
			{
				TRACE("헉 InsertObjWorld 새로 추가된 오브젝트가 Next와 Prev에 값이 있네?? Type = %d \n", pObj->GetType());
			}
		}
	}
	else
	{
		TRACE("헉 InsObjInFld에서 범위를 벗어난게 추가되려고 하네?\n");
		return FALSE;
	}
	return TRUE;
}
BOOL CLandscape::RemoveObjLink( CObj* pObj )
{
	D3DXVECTOR3 vPos = pObj->GetPos();
	vPos.x /= MPU;
	vPos.z /= MPU;
	vPos.x -= m_nWorldX;
	vPos.z -= m_nWorldY;

	CObj** aObjLinkMap	= GetObjLink( pObj->GetLinkType(), pObj->GetLinkLevel() );
	int nMaxWidth = m_nWidthLinkMap[ pObj->GetLinkLevel() ];
	int nUnit = MAP_SIZE / nMaxWidth;
	//int nPos = int( vPos.z / nUnit ) * nMaxWidth + int( vPos.x / nUnit );
	int nPos = ((int)vPos.z / nUnit ) * nMaxWidth + ((int)vPos.x / nUnit );

	if( nPos >= 0 && nPos < nMaxWidth * nMaxWidth )
	{
		if( aObjLinkMap[ nPos ] == pObj )
			aObjLinkMap[ nPos ] = pObj->GetNextNode();
		pObj->DelNode();
		return TRUE;
	}
	return FALSE;
}

CObj* CLandscape::GetObjInLinkMap( D3DXVECTOR3 vPos, DWORD dwLinkType, int nLinkLevel )
{
	vPos.x /= MPU;
	vPos.z /= MPU;
	vPos.x -= m_nWorldX;
	vPos.z -= m_nWorldY;

	CObj** aObjLinkMap	= GetObjLink( dwLinkType, nLinkLevel );
	int nMaxWidth = m_nWidthLinkMap[ nLinkLevel ];
	int nUnit = MAP_SIZE / nMaxWidth;
	//int nPos = int( vPos.z / nUnit ) * nMaxWidth + int( vPos.x / nUnit );
	int nPos = ((int)vPos.z / nUnit ) * nMaxWidth + ((int)vPos.x / nUnit );
	if( nPos >= 0 && nPos < nMaxWidth * nMaxWidth )
		return aObjLinkMap[ nPos ];
	return NULL;
}

BOOL CLandscape::SetObjInLinkMap( D3DXVECTOR3 vPos, DWORD dwLinkType, int nLinkLevel, CObj* pObj )
{
	vPos.x /= MPU;
	vPos.z /= MPU;
	vPos.x -= m_nWorldX;
	vPos.z -= m_nWorldY;

	CObj** aObjLinkMap	= GetObjLink( dwLinkType, nLinkLevel );
	int nMaxWidth = m_nWidthLinkMap[ nLinkLevel ];
	int nUnit = MAP_SIZE / nMaxWidth;
	//int nPos = int( vPos.z / nUnit ) * nMaxWidth + int( vPos.x / nUnit );
	int nPos = ((int)vPos.z / nUnit ) * nMaxWidth + ((int)vPos.x / nUnit );
	if( nPos >= 0 && nPos < nMaxWidth * nMaxWidth )
	{
		aObjLinkMap[ nPos ] = pObj;
		return TRUE;
	}
	return FALSE;
}
void CLandscape::AddObjArray( CObj* pObj )
{
	int nType = pObj->GetType();
	if( nType == OT_OBJ && pObj->GetModel()->IsAniable() )
		nType = OT_ANI;
	CObj** apObject = m_apObject[ nType ];
	CObj* pTemp = NULL;
	int nIndex = -1;
	if( m_aObjStack[ nType ].GetCount() )
	{
		nIndex = m_aObjStack[ nType ].Pop();
		if( nIndex == -1 )
		{
			int a	= 0;
		}
		ASSERT( !apObject[ nIndex ] );
		apObject[ nIndex ] = pObj;
	}
	else
	{
		nIndex = m_adwObjNum[ nType ]++;
		if( nIndex == -1 )
		{
			int a	= 0;
		}
		apObject[ nIndex ] = pObj;
	}
	pObj->m_pWorld = m_pWorld;
	pObj->m_dwObjAryIdx = nIndex;
	//InsertObjLink( pObj );
}

void CLandscape::RemoveObjArray( CObj* pObj )
{
	int nType = pObj->GetType();
	CObj** apObject = m_apObject[ nType ];
	if( apObject[ pObj->m_dwObjAryIdx ] == pObj )
	{
		m_aObjStack[ nType ].Push( pObj->m_dwObjAryIdx );
		apObject[ pObj->m_dwObjAryIdx ] = NULL;
	}
	else
	{
		ASSERT( 0 );
	}
}

int CLandscape::GetObjArraySize()
{
	int nSum = 0;
	for( int i = 0; i < MAX_OBJARRAY; i++ )
		nSum += int( m_adwObjNum[ i ] );
	return nSum;
}

CLandLayer* CLandscape::NewLayer( WORD nTex )
{
	CLandLayer* pLayer = NULL;
	for(int i = 0; i < m_aLayer.GetSize(); i++) 
	{
		pLayer = (CLandLayer*)(m_aLayer.GetAt(i));
		if( pLayer->m_nTex == nTex ) 
			return pLayer;
	}
	pLayer = new CLandLayer(m_pd3dDevice,nTex);

	if( m_aLayer.GetSize() == 0 ) 
	{
		for(i=0;i<NUM_PATCHES_PER_SIDE*NUM_PATCHES_PER_SIDE;i++) 
			pLayer->m_aPatchEnable[i]=TRUE;
	}
	else 
	{
		DWORD color = 0;
		CLandLayer* pTempLayer = (CLandLayer*)m_aLayer[0];

		D3DLOCKED_RECT rectLock;
		pLayer->m_pLightMap->LockRect( 0 , &rectLock, 0, 0);
		for( int i = 0; i < MAP_SIZE * MAP_SIZE; i++ ) 
		{
			((WORD *)rectLock.pBits)[i]=0;//color;
		}
		pLayer->m_pLightMap->UnlockRect(0);
		for(i=0;i<NUM_PATCHES_PER_SIDE*NUM_PATCHES_PER_SIDE;i++) 
			pLayer->m_aPatchEnable[i]=FALSE;
	}
	m_aLayer.Add(pLayer);
	return pLayer;

}

int CLandscape::GetHeightAttribute( int x, int z )
{
	if( x < 0 || x > MAP_SIZE || z < 0 || z > MAP_SIZE ) 
		return -1;
	FLOAT fHeight = m_pHeightMap[ x + z * ( MAP_SIZE + 1 ) ];
	if( fHeight >= HGT_NOWALK )
	{
		if( fHeight >= HGT_DIE )
			return HATTR_DIE;
		if( fHeight >= HGT_NOMOVE )
			return HATTR_NOMOVE;
		if( fHeight >= HGT_NOFLY )
			return HATTR_NOFLY;
		return HATTR_NOWALK;
	}
	return HATTR_NONE;
}


LPWATERHEIGHT CLandscape::GetWaterHeight( int x, int z )
{
	return &m_aWaterHeight[ x + z * NUM_PATCHES_PER_SIDE ]; 
}

FLOAT CLandscape::GetHeightMap( int nOffset ) 
{ 
	FLOAT fHeight = m_pHeightMap[ nOffset ];
	if( fHeight >= HGT_NOWALK ) {
		if( fHeight >= HGT_DIE )
			return m_pHeightMap[ nOffset ] - HGT_DIE; 
		if( fHeight >= HGT_NOMOVE )
			return m_pHeightMap[ nOffset ] - HGT_NOMOVE; 
		if( fHeight >= HGT_NOFLY )
			return m_pHeightMap[ nOffset ] - HGT_NOFLY; 
		return m_pHeightMap[ nOffset ] - HGT_NOWALK; 
	}
	return fHeight;	
}


CLandLayer::CLandLayer(LPDIRECT3DDEVICE9 pd3dDevice,WORD nTex) 
{
	m_pLightMap = NULL;
	m_bVisible = TRUE;
	m_nTex = nTex;

	if( pd3dDevice ) 
	{
	#ifdef __16BITLIGHT
		pd3dDevice->CreateTexture( MAP_SIZE, MAP_SIZE, 1, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED,
										   &m_pLightMap, NULL );
	#else
		pd3dDevice->CreateTexture( MAP_SIZE, MAP_SIZE, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
										   &m_pLightMap, NULL );
	#endif
		D3DLOCKED_RECT rectLock;
		m_pLightMap->LockRect( 0, &rectLock, 0, 0 );
	#ifdef __16BITLIGHT
		for( int i = 0; i < MAP_SIZE * MAP_SIZE; i++ )
			((WORD *)rectLock.pBits)[i] = D3DCOLOR_ARGB16( 0, 127, 127, 127 );
	#else
		for( int i = 0; i < MAP_SIZE * MAP_SIZE; i++ )
			((DWORD *)rectLock.pBits)[i] = D3DCOLOR_ARGB16( 0, 127, 127, 127 );
	#endif
		m_pLightMap->UnlockRect( 0 );

		for( i = 0; i < NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE; i++ ) 
			m_aPatchEnable[ i ]=FALSE;
	}
}

CLandLayer::~CLandLayer() 
{
	SAFE_RELEASE( m_pLightMap );
}











































