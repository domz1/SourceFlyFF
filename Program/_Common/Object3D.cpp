#include "stdafx.h"

#include <stdio.h>
#include <dxerr9.h>
#include "Object3D.h"
#include "Material.h"
//#include "..\_DirectX\dxutil.h"
#include "xUtil3D.h"
#include "ModelObject.h"
#include "..\_UnhandledException\ExceptionHandler.h"

#ifdef __ENVIRONMENT_EFFECT

#include "Environment.h"

#endif // __ENVIRONMENT_EFFECT


LPDIRECT3DTEXTURE9	g_pReflectMap = NULL;

LPDIRECT3DVERTEXDECLARATION9 g_pSkinVertexDeclaration;
LPDIRECT3DVERTEXDECLARATION9 g_pShadowVertexDeclaration;
LPDIRECT3DVERTEXSHADER9 g_pSkiningVS;
LPDIRECT3DVERTEXSHADER9 g_pShadowVS;
//DWORD	g_dwSkiningVS = 0xFFFFFFFF;

D3DXMATRIX	g_mReflect = D3DXMATRIX(  0.50f, 0.00f, 0.00f, 0.00f,
									  0.00f,-0.50f, 0.00f, 0.00f,
									  0.00f, 0.00f, 1.00f, 0.00f,
									  0.50f, 0.50f, 0.00f, 1.00f );

extern float s_fDiffuse[];
extern float s_fAmbient[];
extern D3DXVECTOR4 s_vLight;		// 라이트 벡터
extern D3DXVECTOR4 s_vLightPos;		// 라이트 포지션
extern D3DXMATRIX s_mViewProj;		// 
extern D3DXMATRIX s_mView;		// 
extern D3DXMATRIX s_mProj;		// 
extern BOOL	s_bLight;
extern BOOL	s_bNight;
extern BOOL s_bFog;
extern float s_fFogStart;
extern float s_fFogEnd;


#ifndef	__WORLDSERVER
HRESULT		CreateSkinningVS( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName )
{
	HRESULT hr;
    LPD3DXBUFFER pCode;

    // Setup the vertex declaration
    D3DVERTEXELEMENT9 decl[] =
    {
        { 0,   0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION     , 0}, 
        { 0,  12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_BLENDWEIGHT  , 0}, 
        { 0,  20, D3DDECLTYPE_SHORT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_BLENDINDICES , 0}, 
        { 0,  24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_NORMAL       , 0}, 
        { 0,  36, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD     , 0}, 
        D3DDECL_END()
    };
	
#ifdef	_DEBUG
	// skinning shader
    if( FAILED( hr = pd3dDevice->CreateVertexDeclaration( decl, &g_pSkinVertexDeclaration ) ) )
        return hr;
    // Create vertex shader from a file
    // if( FAILED( hr = D3DXAssembleShaderFromFile( szFileName, NULL, NULL, 0, &pCode, NULL ) ) )
	if( FAILED( hr = D3DXAssembleShaderFromResource(NULL, MAKEINTRESOURCE(IDR_DATA1), NULL, NULL, 0, &pCode, NULL ) ) )
        return hr;
    if( FAILED( hr = pd3dDevice->CreateVertexShader( (DWORD*)pCode->GetBufferPointer(), &g_pSkiningVS ) ) )
    {
		SAFE_RELEASE( pCode );
        return hr;
    }
	SAFE_RELEASE( pCode );
#else
    if( FAILED( hr = pd3dDevice->CreateVertexDeclaration( decl, &g_pSkinVertexDeclaration ) ) )
        return hr;

	// Assemble the vertex shader file
	if( FAILED( hr = D3DXAssembleShaderFromResource(NULL, MAKEINTRESOURCE(IDR_DATA1), NULL, NULL, 0, &pCode, NULL ) ) )
		return hr;
	// Create the vertex shader
	if( FAILED( hr = pd3dDevice->CreateVertexShader( (DWORD*)pCode->GetBufferPointer(),
													&g_pSkiningVS ) ) ) 
	{
		SAFE_RELEASE( pCode );
		return hr;
	}
	SAFE_RELEASE( pCode );
#endif
	return S_OK;
}

HRESULT		CreateShadowVS( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName )
{
	HRESULT hr;
    LPD3DXBUFFER pCode;
	
    // Setup the vertex declaration
    D3DVERTEXELEMENT9 decl[] =
    {
        { 0,   0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION     , 0}, 
        { 0,  12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_NORMAL       , 0}, 
        { 0,  24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD     , 0}, 
        D3DDECL_END()
    };
	
#ifdef	_DEBUG
	// Shadow shader
    if( FAILED( hr = pd3dDevice->CreateVertexDeclaration( decl, &g_pShadowVertexDeclaration ) ) )
        return hr;
    // Create vertex shader from a file
    if( FAILED( hr = D3DXAssembleShaderFromFile( szFileName, NULL, NULL, 0, &pCode, NULL ) ) )
        return hr;
    if( FAILED( hr = pd3dDevice->CreateVertexShader( (DWORD*)pCode->GetBufferPointer(), &g_pShadowVS ) ) )
    {
		SAFE_RELEASE( pCode );
        return hr;
    }
	SAFE_RELEASE( pCode );
#else
    if( FAILED( hr = pd3dDevice->CreateVertexDeclaration( decl, &g_pShadowVertexDeclaration ) ) )
        return hr;
	
	// Assemble the vertex shader file
	if( FAILED( hr = D3DXAssembleShaderFromResource(NULL, MAKEINTRESOURCE(IDR_DATA1), NULL, NULL, 0, &pCode, NULL ) ) )
		return hr;
	// Create the vertex shader
	if( FAILED( hr = pd3dDevice->CreateVertexShader( (DWORD*)pCode->GetBufferPointer(),
		&g_pShadowVS ) ) ) 
	{
		SAFE_RELEASE( pCode );
		return hr;
	}
	SAFE_RELEASE( pCode );
#endif
	return S_OK;
}


void	DeleteVertexShader( LPDIRECT3DDEVICE9 pd3dDevice )
{
    if( g_pSkiningVS )
        SAFE_RELEASE( g_pSkiningVS );
    if( g_pShadowVS )
        SAFE_RELEASE( g_pShadowVS );
	if( g_pSkinVertexDeclaration )
		SAFE_RELEASE( g_pSkinVertexDeclaration );
	if( g_pShadowVertexDeclaration )
		SAFE_RELEASE( g_pShadowVertexDeclaration );
}

LPDIRECT3DVERTEXBUFFER9		g_pd3d_ShadowVB;

HRESULT		CreateShadowMask( LPDIRECT3DDEVICE9 pd3dDevice, int nWidth, int nHeight )
{
	HRESULT	hr;

	hr = pd3dDevice->CreateVertexBuffer( 4 * sizeof(SHADOWMASK_VERTEX),
										 D3DUSAGE_WRITEONLY, D3DFVF_SHADOWMASKVERTEX,
										 D3DPOOL_MANAGED, &g_pd3d_ShadowVB, NULL );
	if( FAILED(hr) )
		Error( "CreateShadowMask : 버텍스 버퍼 생성 실패" );

	SHADOWMASK_VERTEX *pVertex;
	hr = g_pd3d_ShadowVB->Lock(0, 4 * sizeof(SHADOWMASK_VERTEX), (void **)&pVertex, 0);
	if( FAILED(hr) )
		return hr;
	memset( pVertex, 0x00, 4 * sizeof(SHADOWMASK_VERTEX) );
	pVertex[0].x = 0.0f;	pVertex[0].y = 0.0f;
	pVertex[1].x = (float)nWidth; pVertex[1].y = 0.0f;
	pVertex[2].x = 0.0f; pVertex[2].y = (float)nHeight;
	pVertex[3].x = (float)nWidth; pVertex[3].y = (float)nHeight;
	
	pVertex[0].color = pVertex[1].color = pVertex[2].color = pVertex[3].color = 0x44000000;
	
	g_pd3d_ShadowVB->Unlock();
	
	return S_OK;
}

void	RenderShadowMask( LPDIRECT3DDEVICE9 pd3dDevice )
{
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	pd3dDevice->SetRenderState(D3DRS_STENCILFUNC,   D3DCMP_LESSEQUAL);
    pd3dDevice->SetRenderState(D3DRS_STENCILPASS,   D3DSTENCILOP_KEEP);
    pd3dDevice->SetRenderState(D3DRS_STENCILREF,    1);
	
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    pd3dDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
    pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	
	
	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetVertexDeclaration( NULL );
	pd3dDevice->SetFVF( D3DFVF_SHADOWMASKVERTEX );
	pd3dDevice->SetStreamSource(0, g_pd3d_ShadowVB, 0, sizeof(SHADOWMASK_VERTEX));

	pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	// reset
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
    pd3dDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
    pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void	DeleteShadowMask( void )
{
	SAFE_RELEASE( g_pd3d_ShadowVB );
}

#endif // !__WORLDSERVER
////////////////////////////////////////////////////////////////////////////////////
///////////////
///////////////
///////////////
///////////////
///////////////
///////////////
////////////////////////////////////////////////////////////////////////////////////
CObject3DMng		g_Object3DMng;

CObject3DMng :: CObject3DMng()
{
	Init();
}

CObject3DMng :: ~CObject3DMng()
{
	Destroy();
}

void	CObject3DMng :: Init( void )
{
#ifdef __JEFF_11_5
	m_mapObject3D.clear();
#else	// __JEFF_11_5
	int		i;
	m_nSize = 0;
	m_nMax = 0;
	m_nCachePos = 0;

	for( i = 0; i < MAX_OBJECT3D; i ++ )	m_pObject3DAry[i] = NULL;
	for( i = 0; i < MAX_OBJECT3D_CACHE; i ++ )	m_pCache[i] = NULL;
//	m_tmTimer	= timeGetTime();
#endif	// __JEFF_11_5
}

// 디바이스 자원과 메모리 모두를 날림.
void	CObject3DMng :: Destroy( void )
{
#ifdef __JEFF_11_5
	for( map<string, CObject3D*>::iterator i = m_mapObject3D.begin(); i != m_mapObject3D.end(); ++i )
	{
		i->second->DeleteDeviceObjects();
		SAFE_DELETE( i->second );
	}
#else	// __JEFF_11_5
	int		i;
	CObject3D		**pAry = m_pObject3DAry;
	CObject3D		*pObject3D;

	for( i = 0; i < MAX_OBJECT3D; i ++ )
	{
		pObject3D = *pAry++;
		if( pObject3D )
		{
			pObject3D->DeleteDeviceObjects();
			SAFE_DELETE( pObject3D );
			//TRACE( "Delete Object 3D%p\n", pObject3D);
		}
	}
#endif	// __JEFF_11_5

	Init();
}

HRESULT CObject3DMng :: InvalidateDeviceObjects()
{
	DeleteDeviceObjects();
	return  S_OK;
}

// 디바이스 자원만 날림.
HRESULT CObject3DMng :: DeleteDeviceObjects()
{
#ifdef __JEFF_11_5
	for( map<string, CObject3D*>::iterator i = m_mapObject3D.begin(); i != m_mapObject3D.end(); ++i )
		i->second->DeleteDeviceObjects();
#else	// __JEFF_11_5
	int		i;
	CObject3D		**pAry = m_pObject3DAry;
	CObject3D		*pObject3D;
	
	for( i = 0; i < MAX_OBJECT3D; i ++ )
	{
		pObject3D = *pAry++;
		if( pObject3D )
		{
			pObject3D->DeleteDeviceObjects();
		}
	}
#endif	// __JEFF_11_5
	return S_OK;
}

// pTexture를 사용하는 매터리얼을 찾아 삭제한다.
// 공유되어 있는 텍스쳐라면 사용카운터를 보고 1인것만 삭제한다..
int CObject3DMng::DeleteObject3D( CObject3D *pObject3D )
{
#ifdef __JEFF_11_5
	if( !pObject3D )
		return FALSE;
	if( m_mapObject3D.size() == 0 )
		return FALSE;
	if( pObject3D->m_nUseCnt > 1 )
		pObject3D->m_nUseCnt--;
	else
	{
		m_mapObject3D.erase( pObject3D->m_szFileName );
		SAFE_DELETE( pObject3D );
		return TRUE;
	}
	return FALSE;
#else	// __JEFF_11_5
	if( pObject3D == NULL )	return FALSE;
	if( m_nMax == 0 )	return FALSE;
	
	int	i, j;

	for( i = 0; i < MAX_OBJECT3D; i ++ )
	{
		if( m_pObject3DAry[i] )
		{
			if( m_pObject3DAry[i] == pObject3D )		// pObject3D를 찾았다.
			{
				if( m_pObject3DAry[i]->m_nUseCnt == 1 )			// 공유된게 아니다(usecnt == 1)
				{
					for( j = 0; j < MAX_OBJECT3D_CACHE; j ++ )
					{
						if( m_pCache[j] == pObject3D )
						{
							m_pCache[j] = NULL;			// 캐쉬에 있었는지 먼저 검사해보고 캐쉬를 먼저 치움.
							break;
						}
					}
					SAFE_DELETE( m_pObject3DAry[i] );			// 오브젝트 관리자에서도 삭제하고 실제 객체도 날려버림.
					m_nMax --;
					return TRUE;
				}
			}
		}
	}

	return FALSE;
#endif	// __JEFF_11_5
}



//
//
//
CObject3D		*CObject3DMng :: LoadObject3D( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName )
{
#ifdef __JEFF_11_5
	char sFile[MAX_PATH]	= { 0,};
	strcpy( sFile, szFileName );
	strlwr( sFile );

	map<string, CObject3D*>::iterator i		= m_mapObject3D.find( sFile );
	if( i != m_mapObject3D.end() )
	{
		i->second->m_nUseCnt++;
		return i->second;
	}
	CObject3D* pObject3D	= new CObject3D;
	pObject3D->InitDeviceObjects( pd3dDevice );
	if( pObject3D->LoadObject( szFileName ) == FAIL )
	{
		SAFE_DELETE( pObject3D );
		return NULL;
	}
	pObject3D->m_nUseCnt	= 1;
	bool bResult	= m_mapObject3D.insert( map<string, CObject3D*>::value_type( sFile, pObject3D ) ).second;

	return pObject3D;
#else	// __JEFF_11_5
	int			i;
	CObject3D	**pAry;
	CObject3D	*pObject3D;
	int			nIdx = -1;

	// 이미 메모리에 적재 되었는지 검색, 동시에 빈곳도 검색
	// 일단 캐쉬를 검색
	pAry = m_pCache;
	for( i = 0; i < MAX_OBJECT3D_CACHE; i ++ )
	{
		pObject3D = *pAry++;
		if( pObject3D )
		{
			if( strcmpi(pObject3D->m_szFileName, szFileName) == 0 && pObject3D->m_pd3dDevice == pd3dDevice )		// 같은걸 찾았으면 그걸 리턴
			{
				pObject3D->m_nUseCnt ++;	// 중복되어 사용되어지면 카운트 올림.
				return pObject3D;
			}
		}
	}
	// 이미 메모리에 적재 되었는지 검색, 동시에 빈곳도 검색
	pAry = m_pObject3DAry;
	for( i = 0; i < MAX_OBJECT3D; i ++ )
	{
		pObject3D = *pAry++;
		if( pObject3D )				// 널이 아닌것은 무슨 모션이든 로딩되어 있다는 뜻
		{
			if( strcmpi(pObject3D->m_szFileName, szFileName) == 0 && pObject3D->m_pd3dDevice == pd3dDevice )		// 같은걸 찾았으면 그걸 리턴
			{
				pObject3D->m_nUseCnt ++;	// 중복되어 사용되어지면 카운트 올림.
				return pObject3D;
			}
		} else
		{
			if( nIdx == -1 )		nIdx = i;			// 빈곳이 있으면 가장처음 빈곳이 나온곳을 기억해둠
		}
	}
	if( nIdx == -1 )	
	{
		Error( "%s : 읽을 수 있는 한계를 초과했다", szFileName );
	}

	pObject3D = new CObject3D;
//	pObject3D->m_tmCreate = timeGetTime();		// 객체가 생성된 당시시간을 기록
	pObject3D->InitDeviceObjects( pd3dDevice );

	// 로딩된게 아니었다면.  실제로 데이타 읽음.
	if( pObject3D->LoadObject( szFileName ) == FAIL )
	{
		SAFE_DELETE( pObject3D );
		return NULL;
	}

	pObject3D->m_nUseCnt = 1;	// 처음 로딩되었으면 1부터
	// 읽은 메쉬포인터를 리스트에 등록
	m_pObject3DAry[ nIdx ] = pObject3D;
	
	// 캐시에도 넣음
	m_pCache[ m_nCachePos++ ] = pObject3D;
	if( m_nCachePos >= MAX_OBJECT3D_CACHE )		m_nCachePos = 0;

	m_nMax ++;			// 가진 모션 갯수 증가

	return pObject3D;		// 읽은 모션 포인터 리턴
#endif	// __JEFF_11_5
}

// 주기적으로 검사해서 사용한지 오래된놈은 메모리에서 날림.
// 서버에서는 사용하지 말자.
void CObject3DMng :: Process( void )
{
}




////////////////////////////////////////////////////////////////////////////////////
///////////////
///////////////
///////////////
///////////////
///////////////
///////////////
////////////////////////////////////////////////////////////////////////////////////


CObject3D :: CObject3D()
{
	Init();
}

CObject3D :: ~CObject3D()
{
	Destroy();
}

void	CObject3D :: Init( void )
{
	m_pGroup	  = &m_Group[0];
	memset( m_Group, 0, sizeof(LOD_GROUP) * MAX_GROUP );
//	m_pObject	  = NULL;
//	_mUpdate	  = NULL;
	m_pmExternBone = m_pmExternBoneInv = NULL;
	m_pBaseBone	   = NULL;
	m_pBaseBoneInv = NULL;
	m_pMotion = NULL;
	memset( &m_CollObject, 0, sizeof(GMOBJECT) );
	m_CollObject.m_Type = GMT_ERROR;
	m_nID = 0;
	m_nHavePhysique = FALSE;
//	m_nMaxObject = 0;
	m_nMaxBone = 0;
	m_bSendVS = 0;
	m_bLOD = 0;
	memset( m_szFileName, 0, sizeof(m_szFileName) );

	m_vForce1.x = m_vForce1.y = m_vForce1.z = 0;
	m_vForce2.x = m_vForce2.y = m_vForce2.z = 0;
#if __VER >= 9 // __CSC_VER9_5
	m_vForce3.x = m_vForce3.y = m_vForce3.z = 0;
	m_vForce4.x = m_vForce4.y = m_vForce4.z = 0;
#endif //__CSC_VER9_5
	m_vBBMin.x = m_vBBMin.y = m_vBBMin.z = 65535.0;
	m_vBBMax.x = m_vBBMax.y = m_vBBMax.z = -65535.0;
	m_nMaxFace = 0;
	m_nNoTexture = 0;
	m_nNoEffect = 0;

	m_nMaxEvent = 0;
	memset( m_vEvent, 0, sizeof(m_vEvent) );

	m_fScrlU = m_fScrlV = 0.0f;

	m_nUseCnt = 0;	// 일단 초기화는 0
	
	m_fAmbient[0] = 1.0f;
	m_fAmbient[1] = 1.0f;
	m_fAmbient[2] = 1.0f;

	m_nTextureEx = 0;
	m_pAttr = NULL;
#ifdef __YENV
	m_pNormalDecl	= NULL;
#endif //__YENV	

#ifdef __BS_EFFECT_LUA
	m_dwEffect_EX = 0;

	m_pMteData = NULL;
#endif;
}

void	CObject3D :: Destroy( void )
{
	int		i, j, k;

	for( j = 0; j < MAX_GROUP; j ++ )
	{
		GMOBJECT *pObject = m_Group[j].m_pObject;
		for( i = 0; i < m_Group[j].m_nMaxObject; i ++ )
		{
			SAFE_DELETE_ARRAY( pObject[i].m_pVertexList );
			SAFE_DELETE_ARRAY( pObject[i].m_pPhysiqueVertex );
			for( k = 0; k < pObject[i].m_nMaxMtrlBlk; k ++ )
			{
			#if !defined(__WORLDSERVER) 
				g_TextureMng.DeleteMaterial( pObject[i].m_pMtrlBlkTexture[k] );	// 사용하던 텍스쳐는 삭제한다. 공유하는것이면 삭제 안함.
			#endif // !__WORLDSERVER

#ifdef __YENV
				if( g_Option.m_bSpecBump )
				{
					SAFE_RELEASE( pObject[i].m_pNormalTexture[k] );
					SAFE_RELEASE( pObject[i].m_pNoSpecTexture[k] );
				}
#endif //__YENV
			}
			SAFE_DELETE_ARRAY( pObject[i].m_pMtrlBlk );
			SAFE_DELETE_ARRAY( pObject[i].m_pMtrlBlkTexture );
			SAFE_DELETE_ARRAY( pObject[i].m_pFrame );

#ifdef __YENV
			SAFE_DELETE_ARRAY( pObject[i].m_pNormalTexture );		
			SAFE_DELETE_ARRAY( pObject[i].m_pNoSpecTexture );		
#endif //__YENV

			SAFE_DELETE_ARRAY( pObject[i].m_pVB );
			SAFE_DELETE_ARRAY( pObject[i].m_pIB );
			pObject[i].m_pIIB = NULL;		// IIB는 IB와 같은 블럭을 사용하므로 delete하면 안된다.
		}
	}
	
	SAFE_DELETE_ARRAY( m_CollObject.m_pVertexList );
	SAFE_DELETE_ARRAY( m_CollObject.m_pPhysiqueVertex );
	SAFE_DELETE_ARRAY( m_CollObject.m_pMtrlBlk );
	SAFE_DELETE_ARRAY( m_CollObject.m_pMtrlBlkTexture );
	SAFE_DELETE_ARRAY( m_CollObject.m_pFrame );

	
	SAFE_DELETE_ARRAY( m_pAttr );
	
	SAFE_DELETE_ARRAY( m_CollObject.m_pVB );
	SAFE_DELETE_ARRAY( m_CollObject.m_pIB );
	m_CollObject.m_pIIB = NULL;		// IIB는 IB와 같은 블럭을 사용하므로 delete하면 안된다.

	DeleteDeviceObjects();	// m_pObject[].m_pd3d_VB
	// m_pObject를 삭제하려면 DeleteDeviceObjets()를 먼저하고 해야한다.

#ifdef __YENV
	SAFE_DELETE_ARRAY( m_CollObject.m_pNormalTexture );		
	SAFE_DELETE_ARRAY( m_CollObject.m_pNoSpecTexture );		
#endif //__YENV
	
	
	SAFE_DELETE_ARRAY( m_Group[0].m_pObject );		// 통짜 메모리 풀이므로 [0]만 지우면 된다.
	m_Group[1].m_pObject = NULL;
	m_Group[2].m_pObject = NULL;
	SAFE_DELETE_ARRAY( m_Group[0]._mUpdate );		// 통짜 메모리 풀이므로 [0]만 지우면 된다.
	m_Group[1]._mUpdate = NULL;
	m_Group[2]._mUpdate = NULL;
	// 충돌용 메시는 d3d_VB를 생성하지 않기때문에 DeleteDevice를 안해도 된다.
	SAFE_DELETE_ARRAY( m_pBaseBone );
	SAFE_DELETE( m_pMotion );

#ifdef __BS_EFFECT_LUA
	m_dwEffect_EX = 0;

	SAFE_DELETE( m_pMteData );
#endif;

	Init();
}

int		CObject3D :: GetMaxVertex( void )
{
	int		i, nMax = 0;
	for( i = 0; i < m_pGroup->m_nMaxObject; i ++ )
		nMax += m_pGroup->m_pObject[i].m_nMaxVertexList;
	return nMax;
}

int		CObject3D :: GetMaxFace( void )
{
	int		i, nMax = 0;
	for( i = 0; i < m_pGroup->m_nMaxObject; i ++ )
		nMax += m_pGroup->m_pObject[i].m_nMaxFaceList;
	return nMax;
}

int		CObject3D :: GetMaxMtrlBlk( void )
{
	int		i, nMax = 0;
	for( i = 0; i < m_pGroup->m_nMaxObject; i ++ )
		nMax += m_pGroup->m_pObject[i].m_nMaxMtrlBlk;
	return nMax;
}


//
//	버텍스 버퍼를 생성/재생성.
//
HRESULT CObject3D::RestoreDeviceObjects( LPDIRECT3DVERTEXBUFFER9 *ppd3d_VB, D3DPOOL pool )
{
	int		i, j;
	GMOBJECT	*pObj;

	for( j = 0; j < MAX_GROUP; j ++ )
	{
		GMOBJECT	*pObject = m_Group[j].m_pObject;
		int			nMaxObject = m_Group[j].m_nMaxObject;
		for( i = 0; i < nMaxObject; i ++ )
		{
			pObj = &pObject[i];
			if( pObj->m_Type == GMT_SKIN )	
			{

				#ifdef __YENV
				if( pObj->m_bBump && g_Option.m_bSpecBump )
				{
					ExtractBuffers( GMT_SKIN, &ppd3d_VB[i], pObj );
				}
				#endif //__YENV

				// 스킨은 버텍스버퍼를 따로 써야 하므로 지정된 외부포인터에다 버텍스 버퍼를 받음.
				CreateDeviceBuffer( pObj, &ppd3d_VB[i], pool );

			} else
			{
				if( pObj->m_pd3d_VB )	
					continue;

				#ifdef __YENV
				if( pObj->m_bBump && g_Option.m_bSpecBump )
					ExtractBuffers( GMT_NORMAL, NULL, pObj );
				#endif //__YENV

				CreateDeviceBuffer( pObj, NULL, pool );

			}
		}
		ppd3d_VB += m_Group[j].m_nMaxObject;
	}


	return S_OK;
}	

HRESULT CObject3D::InvalidateDeviceObjects()
{
	DeleteDeviceObjects();
	return  S_OK;
}	

HRESULT CObject3D::DeleteDeviceObjects()
{
	int		i, j;
	GMOBJECT	*pObj;
	
	for( j = 0; j < MAX_GROUP; j ++ )
	{
		GMOBJECT	*pObject = m_Group[j].m_pObject;
		int			nMaxObject = m_Group[j].m_nMaxObject;
		for( i = 0; i < nMaxObject; i ++ )
		{
			pObj = &pObject[i];
			SAFE_RELEASE( pObj->m_pd3d_VB );
			SAFE_RELEASE( pObj->m_pd3d_IB );
			
#ifdef __YENV
			if( g_Option.m_bSpecBump )
			{
				for( int k=0; k < pObj->m_nMaxMtrlBlk; k++ )
				{
					if( pObj->m_pNormalTexture )
						SAFE_RELEASE( pObj->m_pNormalTexture[k] );
					
					if( pObj->m_pNoSpecTexture )
						SAFE_RELEASE( pObj->m_pNoSpecTexture[k] );
				}
			}
#endif //__YENV
			
			
			// MtrlBlk의 m_pTexture는 여기서 지우지 않는다.
		}
	}

#ifdef __YENV	
	SAFE_RELEASE( m_pNormalDecl );		// 정점선언
#endif //__YENV

	return  S_OK;
}	

int		g_MaxVB = 0;
//
//	버텍스 버퍼 & 인덱스 버퍼생성.
//
HRESULT	CObject3D :: CreateDeviceBuffer( GMOBJECT *pObject, LPDIRECT3DVERTEXBUFFER9 *ppd3d_VB, D3DPOOL pool )
{
#ifdef	__WORLDSERVER
	return S_OK;
#else
//	pool = D3DPOOL_MANAGED;
	
	HRESULT		hr;
	int			nVertexSize;
	DWORD		dwFVF;
	DWORD		dwUsage = D3DUSAGE_WRITEONLY;

	if( g_bUsableVS == FALSE && pObject->m_pPhysiqueVertex )	// 버텍스쉐이더 지원안되고 스키닝을 해야하는 오브젝트면
		dwUsage |= D3DUSAGE_SOFTWAREPROCESSING;		// 소프트웨어 버텍스버퍼로 생성.

	nVertexSize = pObject->m_nVertexSize;
	dwFVF		= pObject->m_dwFVF;

	if( ppd3d_VB == NULL )		// ppd3d_VB가 널이면 내부... this->m_pd3d_VB를 써야 한다.
	{
		// create vertexbuffer
		if( pObject->m_pd3d_VB == NULL )
		{
			// for Indexed Primitive
			g_MaxVB += (pObject->m_nMaxVB * nVertexSize);

	#ifdef __YENV_WITHOUT_BUMP
			hr = m_pd3dDevice->CreateVertexBuffer(  pObject->m_nMaxVB * nVertexSize,
													dwUsage, dwFVF,
													pool, &pObject->m_pd3d_VB, NULL );
	#else //__YENV_WITHOUT_BUMP
			hr = m_pd3dDevice->CreateVertexBuffer(  pObject->m_nMaxVB * nVertexSize,
				dwUsage, dwFVF,
				pool, &pObject->m_pd3d_VB, NULL );
	#endif //__YENV_WITHOUT_BUMP

			if( FAILED(hr) )
			{
				LPCTSTR szError = Error( "1 %s Object3D 버텍스 버퍼 생성 실패(%s) : m_nMaxVB=%d, nVertexSize=%d, dwFVF=%d, dwUsage=%d, pool=%d %08x", 
																		  m_szFileName, DXGetErrorString9(hr), pObject->m_nMaxVB, 
																		  nVertexSize, dwFVF, dwUsage, (int)pool, (int)m_pd3dDevice );
				ADDERRORMSG( szError );
				// Object3D관리자를 통해 메쉬를 로딩한 후 그 포인터를 받아온다.
				if( FAILED( hr = m_pd3dDevice->TestCooperativeLevel() ) )		// 디바이스가 허접하면 에러남김.
				{
					LPCTSTR szErr = Error( "%s CObject3D 디바이스실패 %08x", m_szFileName, (int)hr );
					ADDERRORMSG( szErr );
				}
				int *p = NULL;
				*p = 1;
				return E_FAIL;
			}
			SendVertexBuffer( pObject, pObject->m_pd3d_VB );
		}
	} else
	{
		if( *ppd3d_VB == NULL )		// 아직 할당 되지 않았음
		{
			// for Indexed Primitive
			g_MaxVB += (pObject->m_nMaxVB * nVertexSize);

	#ifdef __YENV_WITHOUT_BUMP
			hr = m_pd3dDevice->CreateVertexBuffer(  pObject->m_nMaxVB * nVertexSize,
													dwUsage, dwFVF,
													pool, ppd3d_VB, NULL );
	#else //__YENV_WITHOUT_BUMP
			hr = m_pd3dDevice->CreateVertexBuffer(  pObject->m_nMaxVB * nVertexSize,
				dwUsage, dwFVF,
				pool, ppd3d_VB, NULL );			
	#endif //__YENV_WITHOUT_BUMP

			if( FAILED(hr) )
			{
				LPCTSTR szError = Error( "2 %s Object3D 버텍스 버퍼 생성 실패(%s) : m_nMaxVB=%d, nVertexSize=%d, dwFVF=%d, dwUsage=%d, pool=%d %08x", 
																		  m_szFileName, DXGetErrorString9(hr), pObject->m_nMaxVB, 
																		  nVertexSize, dwFVF, dwUsage, (int)pool, (int)m_pd3dDevice );
				ADDERRORMSG( szError );
				if( FAILED( hr = m_pd3dDevice->TestCooperativeLevel() ) )		// 디바이스가 허접하면 에러남김.
				{
					LPCTSTR szErr = Error( "%s CObject3D 디바이스실패 %08x", m_szFileName, (int)hr );
					ADDERRORMSG( szErr );
				}
				int *p = NULL;
				*p = 1;
				return E_FAIL;
			}
			SendVertexBuffer( pObject, *ppd3d_VB );
		}
	}
	
	// create indexbuffer
	if( pObject->m_pd3d_IB == NULL )
	{
		g_MaxVB += (pObject->m_nMaxIB * sizeof(WORD));

		hr = m_pd3dDevice->CreateIndexBuffer(	pObject->m_nMaxIB * sizeof(WORD),
												dwUsage, D3DFMT_INDEX16,
												D3DPOOL_MANAGED, &pObject->m_pd3d_IB, NULL );
		if( FAILED(hr) )
		{
			Error( "%s Object3D 인덱스 버퍼 생성 실패(%s) : m_nMaxIB=%d", 
													m_szFileName, DXGetErrorString9(hr), pObject->m_nMaxIB );
			int *p = NULL;
			*p = 1;
			return E_FAIL;
		}
		SendIndexBuffer( pObject );
	}
	
	
	return S_OK;
#endif // !__WORLDSERVER
}

// szFileName이 애니메이션 파일인가?
int CObject3D::IsAnimateFile( LPCTSTR szFileName )
{
	CResFile resFp;

	BOOL bRet = resFp.Open( szFileName, "rb" );
	if( bRet == FALSE )	
	{
		Error( "IsAnimateFile : %s 읽기 실패", szFileName );
		return FAIL;
	}
	char c0;
	int d0;
	char buff[64];
	D3DXVECTOR3 v;
	float f0;

	resFp.Read( &c0, 1, 1 );		// 파일명 스트링 길이 일음.
	resFp.Read( buff, c0, 1 );	// 파일명 읽음.
	resFp.Read( &d0, 4, 1 );		// 버전
	
	resFp.Read( &d0, 4, 1 );		// Serial ID
	resFp.Read( &v, sizeof(D3DXVECTOR3), 1 );		// 검광1,2의 좌표인데 일단 이렇게 하자.
	resFp.Read( &v, sizeof(D3DXVECTOR3), 1 );
	resFp.Read( &f0, sizeof(float), 1 );
	resFp.Read( &f0, sizeof(float), 1 );
	resFp.Seek( 16, SEEK_CUR );		// reserved
	
	resFp.Read( &v, sizeof(D3DXVECTOR3), 1 );		// 대표 바운딩 박스
	resFp.Read( &v, sizeof(D3DXVECTOR3), 1 );
	resFp.Read( &f0, sizeof(float), 1 );		// per slerp
	resFp.Read( &d0, 4, 1 );					// ani frame 수.  애니가 없으면 0이되도록 저장할것.

	BOOL bAnimate = FALSE;
	if( d0 > 0 )			// MaxFrame이 있냐?
		bAnimate = TRUE;
	else
		bAnimate = FALSE;

	resFp.Close();

	return bAnimate;
}

//
//		Load Object
//
int		CObject3D :: LoadObject( LPCTSTR szFileName )
{
	int		j, nTemp;
	int		nVer;
	D3DXVECTOR3	vTemp;
	BOOL	bNormalObj = FALSE;

	CResFile resFp;

	BOOL bRet = resFp.Open( MakePath( DIR_MODEL, szFileName ), "rb" );
	if( bRet == FALSE )	
	{
		Error( "LoadObject : %s 읽기 실패", szFileName );
		return FAIL;
	}
	Init();

	// 파일명 카피
	char szName[MAX_PATH];
	strcpy( m_szFileName, szFileName );
	strlwr( m_szFileName );
//	strcpy( szName, GetFileName(szFileName) );
	GetFileName( szFileName, szName );

	char buff[MAX_PATH];
	char cLen;
	resFp.Read( &cLen, 1, 1 );		// 파일명 스트링 길이 일음.
	resFp.Read( buff, cLen, 1 );	// 파일명 읽음.
	for( j = 0; j < cLen; j ++ )
		buff[j] = buff[j] ^ (char)0xcd;	// 암호화 해제

	if( cLen >= 64 )
	{
		Error( "LoadObject3D : %s 파일명이 너무 길다 %d", szFileName, (int)cLen );
		return 0;
	}

	buff[cLen] = 0;	// 끝에 널 붙임.
	if( strcmpi( szName, buff ) != 0 )	// 헤더의 파일명과 비교해보고 틀리면 에러.
	{
		Error( "잘못된 파일 : %s, %s, %s", szFileName, szName, buff );
		return 0;
	}

	//--- 공통 헤더부
	resFp.Read( &nVer, 4, 1 );		// 버전
	if( nVer < VER_MESH )
	{
		Error( "%s의 버전은 %d.  최신버전은 %d", szFileName, nVer, VER_MESH );
		resFp.Close();
		return FAIL;
	}
		
	resFp.Read( &m_nID, 4, 1 );		// Serial ID
	resFp.Read( &m_vForce1, sizeof(D3DXVECTOR3), 1 );		// 검광1,2의 좌표인데 일단 이렇게 하자.
	resFp.Read( &m_vForce2, sizeof(D3DXVECTOR3), 1 );
#if __VER >= 9 // __CSC_VER9_5
	if(nVer >= 22)
	{
		resFp.Read( &m_vForce3, sizeof(D3DXVECTOR3), 1 );		// 검광3,4의 좌표인데 일단 이렇게 하자.
		resFp.Read( &m_vForce4, sizeof(D3DXVECTOR3), 1 );
	}
#endif //__CSC_VER9_5
	resFp.Read( &m_fScrlU, sizeof(float), 1 );
	resFp.Read( &m_fScrlV, sizeof(float), 1 );
	resFp.Seek( 16, SEEK_CUR );		// reserved

	resFp.Read( &m_vBBMin, sizeof(D3DXVECTOR3), 1 );		// 대표 바운딩 박스
	resFp.Read( &m_vBBMax, sizeof(D3DXVECTOR3), 1 );
	resFp.Read( &m_fPerSlerp, sizeof(float), 1 );		// per slerp
	resFp.Read( &m_nMaxFrame, 4, 1 );					// ani frame 수.  애니가 없으면 0이되도록 저장할것.

	resFp.Read( &m_nMaxEvent, 4, 1 );	// 이벤트 좌표
	if( m_nMaxEvent > 0 )
		resFp.Read( m_vEvent, sizeof(D3DXVECTOR3) * m_nMaxEvent, 1 );

	resFp.Read( &nTemp, 4, 1 );
	if( nTemp )
	{
		m_CollObject.m_Type = GMT_NORMAL;
		LoadGMObject( &resFp, &m_CollObject );		// 충돌용 메시
	}
	resFp.Read( &m_bLOD, 4, 1 );					// LOD가 있는가?

	//--- 자체내장 본 애니메이션이 있다면 본 개수가 있을것이다.  ex) Obj_풍선.o3d    parts_female.o3d, mvr_타조.o3d는 본파일이 따로 있음.
	resFp.Read( &m_nMaxBone, 4, 1 );

	if( m_nMaxBone > 0 )
	{
		m_pBaseBone = new D3DXMATRIX[ m_nMaxBone * 2 ];
		m_pBaseBoneInv = m_pBaseBone + m_nMaxBone;		// InverseTM은 뒤쪽에 붙는다.
		resFp.Read( m_pBaseBone,	   sizeof(D3DXMATRIX) * m_nMaxBone, 1 );		// 디폴트 뼈대 셋트 
		resFp.Read( m_pBaseBoneInv, sizeof(D3DXMATRIX) * m_nMaxBone, 1 );		// InverseTM 세트
		if( m_nMaxFrame > 0 )	// 본이 있고 MaxFrame이 있으면 애니메이션이 있다는걸로 간주.
		{
			m_pMotion = new CMotion;
			m_pMotion->ReadTM( &resFp, m_nMaxBone, m_nMaxFrame );		// 본 애니메이션 읽음.
		}
		resFp.Read( &m_bSendVS, 4, 1 );		// 본개수가 MAX_VS_BONE보다 적어 VS로 한번에 전송가능한 것인가?
	}

	int nMaxGroup = (m_bLOD) ? MAX_GROUP : 1;	// LOD가 있으면 3개다 읽음. 없으면 1개만 읽음.
	LOD_GROUP *pGroup;
	int nPoolSize, nDebugSize = 0;
	resFp.Read( &nPoolSize, 4, 1 );		// 메모리 풀 사이즈.
	GMOBJECT *pPool = new GMOBJECT[ nPoolSize ];	// 메모리 풀.
	if( pPool == NULL )
		Error( "메모리 할당 실패:CObject3D::LoadObject( %s ) %d", m_szFileName, nPoolSize );
	memset( pPool, 0, sizeof(GMOBJECT) * nPoolSize );
	for( int i = 0; i < nMaxGroup; i ++ )
	{
		pGroup = &m_Group[i];
		resFp.Read( &pGroup->m_nMaxObject, 4, 1 );		// geometry 갯수

		pGroup->m_pObject = pPool;		// 메모리 포인터 할당.
		pPool += pGroup->m_nMaxObject;

		nDebugSize += pGroup->m_nMaxObject;
		if( nDebugSize > nPoolSize )
			Error( "LoadObject : 메모리 오버플로우 %d, %d", nPoolSize, nDebugSize );

		memset( pGroup->m_pObject, 0, sizeof(GMOBJECT) * pGroup->m_nMaxObject );
		for( j = 0; j < pGroup->m_nMaxObject; j ++ )		pGroup->m_pObject[j].m_nID = -1;
		
		GMOBJECT	*pObject;
		int		nParentIdx;
		BOOL	bAnimate = FALSE;
		int		nType;
		//------  Geometry Object
		for( j = 0; j < pGroup->m_nMaxObject; j ++ )
		{
			pObject = &pGroup->m_pObject[j];

			resFp.Read( &nType, 4, 1 );	// Type
			pObject->m_Type = (GMTYPE)(nType & 0xffff);
			if( nType & 0x80000000 )
				pObject->m_bLight = TRUE;

			resFp.Read( &pObject->m_nMaxUseBone, 4, 1 );
			if( pObject->m_nMaxUseBone > 0 )
				resFp.Read( pObject->m_UseBone, sizeof(int) * pObject->m_nMaxUseBone, 1 );	// 오브젝트가 사용하는 본리스트
			
#if !defined(__YENV)
			if( pObject->m_Type == GMT_SKIN )
			{
				pObject->m_nVertexSize = sizeof(SKINVERTEX);
				pObject->m_dwFVF	   = D3DFVF_SKINVERTEX;
			} else
			{
				pObject->m_nVertexSize = sizeof(NORMALVERTEX);
				pObject->m_dwFVF	   = D3DFVF_NORMALVERTEX;
			}
#endif //__YENV

			resFp.Read( &pObject->m_nID, 4, 1 );	// Object ID
			resFp.Read( &nParentIdx, 4, 1 );			// parent idx
			pObject->m_nParentIdx = nParentIdx;		// save를 위해 백업 받아둠
			
			// 부모가 있다면 부모 포인터 지정
			if( nParentIdx != -1 )
			{
				resFp.Read( &pObject->m_ParentType, 4, 1 );		// 부모의 타입 읽음.
				// 부모의 포인터를 세팅
				switch( pObject->m_ParentType )
				{
				case GMT_BONE:		pObject->m_pParent = NULL;	break;		// 부모가 본이면 m_pParent를 사용하지 않음.
				case GMT_NORMAL:	pObject->m_pParent = &pGroup->m_pObject[ nParentIdx ];		break;	// 부모가 일반오브젝이라면 그 포인터 지정.
					break;
				}
			}

			// 원점기준의 LocalTM.  부모가 있다면 원점은 부모가 된다.
			resFp.Read( &pObject->m_mLocalTM, sizeof(D3DXMATRIX), 1 );

			// load geometry
			LoadGMObject( &resFp, pObject );			// Mesh부 읽음

#ifdef __YENV
			// 버택스 버퍼 타입을 일반으로..
			// 범프용 버퍼는 ExtractBuffers함수 호출함으로써 사이즈가 바뀐다
			if( g_Option.m_bSpecBump )
				pObject->m_VBType = VBT_NORMAL;
#endif //__YENV
			
			if( pObject->m_Type == GMT_SKIN )
			{
#ifdef __YENV
				if( pObject->m_bBump && g_Option.m_bSpecBump )
				{
	#ifdef __YENV_WITHOUT_BUMP
					pObject->m_nVertexSize = sizeof(SKINVERTEX);
					pObject->m_dwFVF	   = D3DFVF_SKINVERTEX;
	#else //__YENV_WITHOUT_BUMP
					pObject->m_nVertexSize = sizeof(SKINVERTEX_BUMP);
					pObject->m_dwFVF	   = D3DFVF_SKINVERTEX_BUMP;
	#endif //__YENV_WITHOUT_BUMP
				}
				else
#endif //__YENV
				{
					pObject->m_nVertexSize = sizeof(SKINVERTEX);
					pObject->m_dwFVF	   = D3DFVF_SKINVERTEX;
				}
			} else
			{
#ifdef __YENV
				if( pObject->m_bBump && g_Option.m_bSpecBump )
				{
	#ifdef __YENV_WITHOUT_BUMP
					pObject->m_nVertexSize = sizeof(NORMALVERTEX);
					pObject->m_dwFVF	   = D3DFVF_NORMALVERTEX;
	#else //__YENV_WITHOUT_BUMP
					pObject->m_nVertexSize = sizeof(NORMALVERTEX_BUMP);
					pObject->m_dwFVF	   = D3DFVF_NORMALVERTEX_BUMP;
	#endif //__YENV_WITHOUT_BUMP
				}
				else
#endif //__YENV
				{
					pObject->m_nVertexSize = sizeof(NORMALVERTEX);
					pObject->m_dwFVF	   = D3DFVF_NORMALVERTEX;
				}
			}
			
			if( pObject->m_pPhysiqueVertex == NULL )	bNormalObj = TRUE;	// 일반형오브젝트가 하나라도 있을때.
			else										m_nHavePhysique = TRUE;		// 피지크 오브젝트가 하나라도 있으면 TRUE가 된다.

			// load TM animation - 
			if( pObject->m_Type == GMT_NORMAL )	// 스킨에는 m_pFrame이 아예 없다.
			{
				if( m_nMaxFrame > 0 )	LoadTMAni( &resFp, pObject );			// TM Animation 데이타 부분.  maxframe이0이면 아예 읽지 않음.
			}

		}
	} // LOD_GROUP
	// boudbox vMin, vMax값을 이용해 8개의 벡터로 풀어냄
//	SetBB( m_vBBVList, m_vBBMin, m_vBBMax );

	// 갱신용 매트릭스 리스트 생성. - 일반형오브젝트가 하나라도 잇으면 생성.
	if( bNormalObj == TRUE )
	{
		D3DXMATRIX *pmPool = new D3DXMATRIX[ nPoolSize ];
		for( i = 0; i < nMaxGroup; i ++ )
		{
			m_Group[i]._mUpdate = pmPool;
			pmPool += m_Group[i].m_nMaxObject;
			for( j = 0; j < m_Group[i].m_nMaxObject; j ++ )
				D3DXMatrixIdentity( &m_Group[i]._mUpdate[j] );
		}
	}

	// version 21이상부터는 프레임속성 저장됨.
	if( nVer >= 21 )
	{
		int nAttr = 0;
		
		resFp.Read( &nAttr, 4, 1 );
		if( nAttr == m_nMaxFrame )		// 프레임 속성 있는지 검사
		{
			// 프레임 속성 읽음.
			if(m_nMaxFrame > 0)
				m_pAttr	= new MOTION_ATTR[ m_nMaxFrame ];
			else
				m_pAttr = NULL;

			resFp.Read( m_pAttr, sizeof(MOTION_ATTR) * m_nMaxFrame, 1 );
		}
	}
	

	resFp.Close();

	return SUCCESS;
}


//
//	Save
//
int		CObject3D::SaveObject( LPCTSTR szFileName )
{
/*
	FILE	*fp = fopen( szFileName, "wb" );
	int		nTemp;
	char	reserved[32];
	int		i;

	if( fp == NULL )
	{
		Error( "CObject3D::SaveMesh() : %를 찾을 수 없다.\r\n혹은 파일을 Check Out했는지 확인할 것!.", szFileName );
		return FAIL;
	}
	// common header
	memset( reserved, 0, 32 );
	nTemp = VER_MESH;
	fwrite( &nTemp, 4, 1, fp );		// version
	fwrite( &m_nID, 4, 1, fp );		// Serial ID
	fwrite( &m_fScrlU, sizeof(float), 1, fp );
	fwrite( &m_fScrlV, sizeof(float), 1, fp );
	fwrite( reserved, 16, 1, fp );			// reserved

	fwrite( &m_vBBMin, sizeof(D3DXVECTOR3), 1, fp );	// bounding box
	fwrite( &m_vBBMax, sizeof(D3DXVECTOR3), 1, fp );
	fwrite( &m_fPerSlerp, sizeof(float), 1, fp );	// 

	fwrite( &m_nMaxFrame,	4, 1, fp );					// ani frame 수 애니가 없으면 0

	fwrite( &m_nMaxObject, 4, 1, fp );	// geometry 개수

	GMOBJECT		*pObject;
	//------  Geometry Object
	for( i = 0; i < m_nMaxObject; i ++ )
	{
		pObject = &m_pObject[i];
		
		fwrite( &pObject->m_Type,	 4, 1, fp );	
		fwrite( &pObject->m_nID, 4, 1, fp );			// Object ID
		fwrite( &pObject->m_nParentIdx, 4, 1, fp );	// parent Idx

		fwrite( &pObject->m_mLocalTM, sizeof(D3DXMATRIX), 1, fp );	// LocalTM

		// save geometry
		SaveGMObject( fp, pObject );

		// save TM animation - 일반형메쉬만 TMAni를 저장
		SaveTMAni( fp, pObject );
	}

	fclose(fp);
*/
	return SUCCESS;
}

//
//  GMOBJECT리스트에서 nID를 찾아 그놈의 배열인덱스를 리턴
/*int		CObject3D::Find( int nID )
{
	int		i;
	GMOBJECT	*pObject;
	for( i = 0; i < m_nMaxObject; i ++ )
	{
		pObject = &m_pObject[i];
		if( pObject->m_nID >= 0 && pObject->m_nID == nID )
			return i;
	}

	return -1;	// 못찾으면 -1;
}*/


//
//		GMOBJECT 부 읽가
//
int		CObject3D::LoadGMObject( CResFile *file, GMOBJECT *pObject )
{
	DWORD	dwTemp;
	int		i;
	
	file->Read( &pObject->m_vBBMin, sizeof(D3DXVECTOR3), 1 );
	file->Read( &pObject->m_vBBMax, sizeof(D3DXVECTOR3), 1 );
	
	file->Read( &pObject->m_bOpacity,    4, 1 );
	file->Read( &pObject->m_bBump, 4, 1 );
	file->Read( &pObject->m_bRigid,		4, 1 );
	file->Seek( 28, SEEK_CUR );		// reserved
	
#ifdef __YENV_WITHOUT_BUMP
	pObject->m_bBump = TRUE;
#endif //__YENV_WITHOUT_BUMP

	// size of list
	file->Read( &pObject->m_nMaxVertexList,	4, 1 );		// 버텍스 개수
	file->Read( &pObject->m_nMaxVB,			4, 1 );		// 버텍스 버퍼 크기
	file->Read( &pObject->m_nMaxFaceList,	4, 1 );		// 페이스 개수
	file->Read( &pObject->m_nMaxIB,		4, 1 );		// indexed 인덱스 버퍼개수

	m_nMaxFace += pObject->m_nMaxFaceList;

	// 버텍스 리스트를 할당하고 버텍스개수만큼 통째로 읽음
	// Vertex list
	if( pObject->m_Type == GMT_SKIN )	// 이런 LoadObject()에서 이미 읽어서 온것이다.
	{
		pObject->m_pVertexList	= new D3DXVECTOR3[ pObject->m_nMaxVertexList ];
		SKINVERTEX *pVB			= new SKINVERTEX[ pObject->m_nMaxVB ];	// Indexed Vertex buffer -
		pObject->m_pVB = pVB;
//		pObject->_pVB			= new SKINVERTEX[ pObject->m_nMaxVB ];
	} else
	{
		pObject->m_pVertexList	= new D3DXVECTOR3[ pObject->m_nMaxVertexList ];
		NORMALVERTEX *pVB		= new NORMALVERTEX[ pObject->m_nMaxVB ];	// Indexed Vertex buffer

		pObject->m_pVB = pVB;
	}
	pObject->m_pIB  = new WORD[ pObject->m_nMaxIB + pObject->m_nMaxVB ];		// m_pIIB도 WORD형을  쓰므로 같이 할당해서 씀.
	pObject->m_pIIB = pObject->m_pIB + pObject->m_nMaxIB;

	file->Read( pObject->m_pVertexList, sizeof(D3DXVECTOR3) * pObject->m_nMaxVertexList, 1 );
	if( pObject->m_Type == GMT_SKIN )
		file->Read( pObject->m_pVB,  sizeof(SKINVERTEX) * pObject->m_nMaxVB, 1 );		// 버텍스 버퍼 읽음
	else
		file->Read( pObject->m_pVB,  sizeof(NORMALVERTEX) * pObject->m_nMaxVB, 1 );		// 버텍스 버퍼 읽음


	file->Read( pObject->m_pIB,  sizeof(WORD) * pObject->m_nMaxIB, 1 );			// 인덱스 버퍼 읽음
	file->Read( pObject->m_pIIB, sizeof(WORD) * pObject->m_nMaxVB, 1 );

	file->Read( &dwTemp, 4, 1 );			// 피지크가 있는가 없는가
	if( dwTemp )
	{
		// Physique
		pObject->m_pPhysiqueVertex = new int[ pObject->m_nMaxVertexList ];
		file->Read( pObject->m_pPhysiqueVertex, sizeof(int) * pObject->m_nMaxVertexList, 1 );	// 버텍스개수와 같은 피지크데이타 읽음

		// m_pVB에는 world로 변환된 버텍스들이 들어가있게 되고
		// _pVB에는 뼈대 기준 로컬로 들어있게 된다.
		// 스키닝 오브젝트의 intersect등을 처리하려면 m_pVB를 써야 한다.
	}


	// Material
	D3DMATERIAL9	mMaterial;
	char			szBitmap[256];
	int				nLen;
	MATERIAL		*mMaterialAry[16];
//nt				nIdx = 0;
	int				bIsMaterial;

	file->Read( &bIsMaterial, 4, 1 );		// ASE의 Main MaxMaterial을 저장했다.  이게 0이면 매터리얼이 없다는 것.
	pObject->m_bMaterial = bIsMaterial;				// 나중에 저장을 위해서 백업받아둔다.
	if( bIsMaterial )
	{
		memset( mMaterialAry, 0, sizeof(mMaterialAry) );
//		for( i = 0; i < 16; i ++ )	mMaterialAry[i] = NULL;
		
		file->Read( &pObject->m_nMaxMaterial, 4, 1 );				// 사용하는 매트리얼 개수 읽음

		if( pObject->m_nMaxMaterial == 0 )	pObject->m_nMaxMaterial = 1;	// CASEMesh의 Save부분을 참고할것.

		for( i = 0; i < pObject->m_nMaxMaterial; i ++ )
		{
			file->Read( &mMaterial, sizeof(D3DMATERIAL9), 1 );
			file->Read( &nLen, 4, 1 );		// bitmap filename length;  null 포함
			if( nLen > sizeof(szBitmap) )		
				Error( "CObject3D::LoadGMObject : %s 텍스쳐 파일명이 너무길다 : 길이 = %d", m_szFileName, nLen );
			file->Read( szBitmap, nLen, 1 );
			strlwr( szBitmap );		// 소문자로 변환
			
			pObject->m_MaterialAry[i].m_Material = mMaterial;

			if( strlen(szBitmap)+1 > sizeof(pObject->m_MaterialAry[i].strBitMapFileName) )
				Error( "CObject3D::LoadGeoMesh() : %s의 길이가 너무 길다", szBitmap );

			strcpy( pObject->m_MaterialAry[i].strBitMapFileName, szBitmap );
		#if	!defined(__WORLDSERVER)
			if( IsEmpty(szBitmap) == FALSE )
				mMaterialAry[i] = g_TextureMng.AddMaterial( m_pd3dDevice, &mMaterial, szBitmap );
		#endif
		}
	}

	// vertex buffer Material block
	file->Read( &pObject->m_nMaxMtrlBlk, 4, 1 );

	if( pObject->m_nMaxMtrlBlk >= 32 )
	{
		Error( "매터리얼 갯수가 32개를 넘었다. %s", m_szFileName );
		return FAIL;
	}
	if( pObject->m_nMaxMtrlBlk > 0 )
	{
		pObject->m_pMtrlBlk = new MATERIAL_BLOCK[ pObject->m_nMaxMtrlBlk ];		// 매터리얼 블럭을 할당하고 읽음
		pObject->m_pMtrlBlkTexture = new LPDIRECT3DTEXTURE9[ pObject->m_nMaxMtrlBlk * 8 ];	// 확장텍스쳐(최대8개)랑 같이 쓴다. 없으면 걍 널이다.
		memset( pObject->m_pMtrlBlkTexture, 0, sizeof(LPDIRECT3DTEXTURE9) * (pObject->m_nMaxMtrlBlk * 8) );

		file->Read( pObject->m_pMtrlBlk, sizeof(MATERIAL_BLOCK) * pObject->m_nMaxMtrlBlk, 1 );
#ifndef __WORLDSERVER

#ifdef __YENV
		if( g_Option.m_bSpecBump )
		{
			// 노말맵	
			pObject->m_pNormalTexture = new LPDIRECT3DTEXTURE9[ pObject->m_nMaxMtrlBlk ];
			memset( pObject->m_pNormalTexture, 0, sizeof(LPDIRECT3DTEXTURE9) * (pObject->m_nMaxMtrlBlk) );

			// 노스팩큘러맵	
			pObject->m_pNoSpecTexture = new LPDIRECT3DTEXTURE9[ pObject->m_nMaxMtrlBlk ];
			memset( pObject->m_pNoSpecTexture, 0, sizeof(LPDIRECT3DTEXTURE9) * (pObject->m_nMaxMtrlBlk) );
		}
#endif //__YENV
		
		
		for( i = 0; i < pObject->m_nMaxMtrlBlk; i ++ )
		{
			if( bIsMaterial )
			{
				if( mMaterialAry[ pObject->m_pMtrlBlk[i].m_nTextureID ] )
				{
					pObject->m_pMtrlBlkTexture[i] = mMaterialAry[ pObject->m_pMtrlBlk[i].m_nTextureID ]->m_pTexture;
						
			#ifdef __YENV
				#ifdef __YENV_WITHOUT_BUMP
					if( g_Option.m_bSpecBump )
				#else //__YENV_WITHOUT_BUMP
					if( pObject->m_pMtrlBlk[i].m_dwEffect & XE_BUMP && g_Option.m_bSpecBump )
				#endif //__YENV_WITHOUT_BUMP
					{
						char	szTexture[MAX_PATH], szFileExt[MAX_PATH];
						char*	strFileName = mMaterialAry[ pObject->m_pMtrlBlk[i].m_nTextureID ]->strBitMapFileName;

						::GetFileTitle( strFileName, szTexture );
						lstrcat( szTexture, "-n." );
						::GetFileExt( strFileName, szFileExt );
						lstrcat( szTexture, szFileExt );
							
						if( IsEmpty(szTexture) == FALSE )
						{
							 CreateNormalMap( pObject->m_Type, m_pd3dDevice, &(pObject->m_pNormalTexture[i]), szTexture );
						}
					}

				#ifdef __YENV_WITHOUT_BUMP
					if( g_Option.m_bSpecBump )
				#else //__YENV_WITHOUT_BUMP
					// 노스팩큘러맵
					if( pObject->m_pMtrlBlk[i].m_dwEffect & XE_BUMP && g_Option.m_bSpecBump )
				#endif //__YENV_WITHOUT_BUMP
					{
						char	szTexture[MAX_PATH], szFileExt[MAX_PATH];
						char*	strFileName		= mMaterialAry[ pObject->m_pMtrlBlk[i].m_nTextureID ]->strBitMapFileName;
						
						::GetFileTitle( strFileName, szTexture );
						lstrcat( szTexture, "-s." );
						::GetFileExt( strFileName, szFileExt );
						lstrcat( szTexture, szFileExt );
	
						if( IsEmpty(szTexture) == FALSE )
						{
					#ifdef _DEBUG
							if( (_access( MakePath( DIR_MODELTEX, szTexture ), 0 )) != -1 )
								LoadTextureFromRes( m_pd3dDevice, MakePath( DIR_MODELTEX, szTexture ), &(pObject->m_pNoSpecTexture[i] ) );
					#else //_DEBUG
							char szSerchPath[MAX_PATH];
							GetCurrentDirectory( sizeof( szSerchPath ), szSerchPath );
							TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR], name[ _MAX_FNAME ], ext[_MAX_EXT];
							_splitpath( szTexture, drive, dir, name, ext );
							
							TCHAR szFileName[ _MAX_PATH ];
							RESOURCE* lpRes;
							strcpy( szFileName, MakePath( DIR_MODELTEX, szTexture ) );
							strlwr( szFileName );
							
							if( CResFile::m_mapResource.Lookup( szFileName, (void*&) lpRes ) )
							{								
								LoadTextureFromRes( m_pd3dDevice, MakePath( DIR_MODELTEX, szTexture ), &(pObject->m_pNoSpecTexture[i] ) );
							}							
					#endif //_DEBUG
						}
					}
					
			#endif //__YENV
				}
//				if( (pObject->m_pMtrlBlk[i].m_nReflect & 0xfffffffe) == 0 )	// 마지막 1비트를 뺀 나머지에 아무값도 없으면 옛날버전일 가능성이있다.
//				{
//					// 이럴땐 컨버트.
//					if( pObject->m_pMtrlBlk[i].m_n2Side )			pObject->m_pMtrlBlk[i].m_nReflect |= XE_2SIDE;
//					if( pObject->m_pMtrlBlk[i].m_nOpacity )			pObject->m_pMtrlBlk[i].m_nReflect |= XE_OPACITY;
//					// XE_REFLECT는 m_nReflect에 값이 들어있는 상태이므로 따로 넣어줄필요 없다.
//				}


			}
		}
#endif	// __WORLDSERVER
	}

	return SUCCESS;
}

//
//	TM Animation data load
//
int		CObject3D::LoadTMAni( CResFile *file, GMOBJECT *pObject )
{
	int	bFrame;
	file->Read( &bFrame, 4, 1 );
	if( bFrame == 0 )	return FAIL;	// 프레임 없으면 읽지 않음.

	pObject->m_pFrame = new TM_ANIMATION[ m_nMaxFrame ];
	file->Read( pObject->m_pFrame, sizeof(TM_ANIMATION) * m_nMaxFrame, 1 );

	return SUCCESS;
} 

//
// 메쉬내에서 szSrc텍스쳐를 szDest텍스쳐로 바꾼다.
//
//
void	CObject3D::ChangeTexture( LPCTSTR szSrc, LPCTSTR szDest )
{
#ifdef __WORLDSERVER
	return;
#else
	int		i, j, k;
	GMOBJECT	*pObject;
	char	szBuff[128];

	strcpy( szBuff, szSrc );
	strlwr( szBuff );
	TCHAR szBitMapFileName[ 16 ][ 128 ];
	for( k = 0; k < MAX_GROUP; k++ )
	{
		for( i = 0; i < m_Group[k].m_nMaxObject; i ++ )
		{
			pObject = &m_Group[k].m_pObject[i];

			// 파일명 비교를 위해서 복사해 놓는다. 교체 처리후 
			// pObject->m_MaterialAry[ nID ].strBitMapFileName의 내용이 변경되기 때문에 오리지날 스트링을
			// 비교를 위해 임시 저장해둘 필요가 있다.
			for( j = 0; j < pObject->m_nMaxMtrlBlk; j ++ )		// 매터리얼 블럭을 돌면서
			{
				int nID = pObject->m_pMtrlBlk[j].m_nTextureID;
				_tcscpy( szBitMapFileName[ j ], pObject->m_MaterialAry[ nID ].strBitMapFileName );
			}
			for( j = 0; j < pObject->m_nMaxMtrlBlk; j ++ )		// 매터리얼 블럭을 돌면서
			{
				int nID = pObject->m_pMtrlBlk[j].m_nTextureID;
				if( strcmp( szBitMapFileName[j], szBuff ) == 0 )	// szSrc랑 같은 파일명이 있으면
				{
					MATERIAL	*pMtrl;
					D3DMATERIAL9	mMtrl;
					pMtrl = g_TextureMng.AddMaterial( m_pd3dDevice, &mMtrl, szDest );		// szDest로 읽어서
					pObject->m_pMtrlBlkTexture[j] = pMtrl->m_pTexture;	// 그놈으로 대체시키고
					strcpy( pObject->m_MaterialAry[ nID ].strBitMapFileName, szDest );	// 파일명 바꿔놓는다.
				}
			}
		}
	} // LOD_GROUP
#endif //!__WORLDSERVER
}

//
//	GMOBJECT부 저장
//
int		CObject3D::SaveGMObject( FILE *fp, GMOBJECT *pObject )
{
	return SUCCESS;
}



//
//
//
int		CObject3D::SaveTMAni( FILE *fp, GMOBJECT *pObject )
{
	int		bFrame;
	if( pObject->m_pFrame )	bFrame = 1;
	else					bFrame = 0;
	fwrite( &bFrame, 4, 1, fp );
	if( bFrame == 0 )	return FAIL;	// 프레임 없으면 쓰지 않음.

	if( m_nMaxFrame > 0 )
	{
		fwrite( pObject->m_pFrame, sizeof(TM_ANIMATION) * m_nMaxFrame, 1, fp );
	}

	return SUCCESS;
}

//static int	_nSlideCnt = 0;

//
// Line과 교차하는 삼각형을 찾아 Slide벡터를 계산.
// 주의 : SlideVectorXZ는 스키닝 오브젝트에는 사용금지다.
// XZ방향으로 이동하는 벡터에만 사용하는 것으로 일반적인 상황엔 SlideVector()를 써야 한다.
// bCollObj : 충돌메시로 검사하는가? (디폴트)
//
int		CObject3D::SlideVectorXZ( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vEnd , const D3DXMATRIX &mWorld, BOOL bCollObj, int nSlideCnt )
{
	D3DXVECTOR3	*v1, *v2, *v3;
	D3DXVECTOR3	vInvPos, vInvEnd, vInvDir, vDir, vIntersect;
	D3DXVECTOR3 vA, vB, vN, vTemp;
	D3DXMATRIX	mTM, mInv;
	int		nMaxFace;
	GMOBJECT* pObject = &m_CollObject;		// 충돌메시로 충돌검사.
//	GMOBJECT* pObject = m_Group[0].m_pObject;
	NORMALVERTEX *pVB;
	WORD		*pIB;
	int		j;
	FLOAT	fDist;
	BOOL	bRet;

	int i;
	LOD_GROUP *pGroup = &m_Group[0];
	int		nMaxObj = 1;
	if( bCollObj == FALSE )		// 충돌메시로 충돌검사 하지마라.
	{
		pObject = pGroup->m_pObject;
		nMaxObj = pGroup->m_nMaxObject;
		if( pGroup->_mUpdate == NULL )		
			return FAIL;
	}
	
	if( pObject->m_Type == GMT_ERROR )		return 0;

	for( i = 0; i < nMaxObj; i++ )
	{
		if( pObject->m_Type == GMT_SKIN )	
			Error( "SlideVectorXZ:스킨오브젝트 발견. 에러%s", m_szFileName ); 
		nMaxFace = pObject->m_nMaxFaceList;
		pIB = pObject->m_pIB;
		pVB = (NORMALVERTEX*)(pObject->m_pVB);

		if( bCollObj == FALSE )
			mTM = pGroup->_mUpdate[i] * mWorld;
		else
			mTM = mWorld;

		pObject++;
		D3DXMatrixInverse( &mInv, NULL, &mTM );
		D3DXVec3TransformCoord( &vInvPos, &vPos, &mInv );	// Line시작점을 Geometry기준으로 변환
		D3DXVec3TransformCoord( &vInvEnd, &vEnd, &mInv );	// Line끝점을 역시 변환
		vInvDir = vInvEnd - vInvPos;
		for( j = 0; j < nMaxFace; j++ )
		{
			v1 = &pVB[ *pIB++ ].position;
			v2 = &pVB[ *pIB++ ].position;
			v3 = &pVB[ *pIB++ ].position;

//			FLOAT fU, fV;
//			bRet = D3DXIntersectTri( v1, v2, v3, &vInvPos, &vInvDir, &fU, &fV, &fDist );
			bRet = IsTouchRayTri( v1, v2, v3, &vInvPos, &vInvDir, &fDist );

			if( bRet && fDist >= 0.0f )	// 반대방향 면은 검사하지 않음.
			{
				// 정확하게 하려면 모든 면을 다 검사해서 가장 가까운것을 골라야 하나
				// S자 형태로 구부러진 면이 없다는 가정하에 간략화 시킨다.
				if( fDist < 1.0f )
				{
					// Line과 닿은 삼각형을 찾아냈다.
					vDir = vEnd - vPos;
					fDist *= 0.5f;
					vIntersect = vPos + fDist * vDir;	// 교차점을 계산.

					vA = *v2 - *v1;
					vB = *v3 - *v1;
					D3DXVec3Cross( &vN, &vA, &vB );		// 충돌한 면의 노말 구함. 이건 나중에 미리 계산해두자.
					mInv = mTM;
					mInv._41 = mInv._42 = mInv._43 = 0;
					D3DXVec3TransformCoord( &vN, &vN, &mInv );	// 충돌한면의 노말을 원래대로(mWorld)돌림.

					if( vN.x == 0 && vN.z == 0 )
						Error( "CActionMover::ProcessCollisionGround : 충돌한 면의 노말이 완전 수직이다" );

					vN.y = 0;	// y성분을 없애서 수직면의 법선인것처럼 변환
					D3DXVec3Normalize( &vN, &vN );		// 최종 단위벡터로 변환
					
					// 자, 이제 vIntersect와 vN은 월드좌표계로 준비가 되었다.
					vTemp = vEnd - vIntersect;		// 교차점 - 라인끝 vector V라고 칭함
					CalcSlideVec( &vTemp, vTemp, vN );
//					vTemp += vIntersect;
//					*pOut = vTemp - vPos;
					*pOut = vTemp;

					vTemp += vPos;
					
					if( ++nSlideCnt < 3 )  // 무한 리커전 방지
						SlideVectorXZ( pOut, vPos, vTemp, mWorld, bCollObj, nSlideCnt );		// 이거왜 시작점이 vPos일까나 -.-;;;
					
//					nSlideCnt = 0;
					return 1;
				}
			}
		}
	}
//	_nSlideCnt = 0;
	return 0;
}

//
// 이 함수 자체로 리커시브콜을 하지 않는 버전.
//
int		CObject3D::SlideVectorXZ2( D3DXVECTOR3 *pOut, D3DXVECTOR3 *pIntersect, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vEnd , const D3DXMATRIX &mWorld, BOOL bCollObj )
{
	D3DXVECTOR3	*v1, *v2, *v3;
	D3DXVECTOR3	vInvPos, vInvEnd, vInvDir, vDir, vIntersect;
	D3DXVECTOR3 vA, vB, vN, vTemp;
	D3DXMATRIX	mTM, mInv;
	int		nMaxFace;
	GMOBJECT* pObject = &m_CollObject;		// 충돌메시로 충돌검사.
	NORMALVERTEX *pVB;
	WORD		*pIB;
	int		j;
	FLOAT	fDist;
	BOOL	bRet;

	int i;
	LOD_GROUP *pGroup = &m_Group[0];
	int		nMaxObj = 1;
	if( bCollObj == FALSE )		// 충돌메시로 충돌검사 하지마라.
	{
		pObject = pGroup->m_pObject;
		nMaxObj = pGroup->m_nMaxObject;
		if( pGroup->_mUpdate == NULL )		return FAIL;
	}

	
	if( pObject->m_Type == GMT_ERROR )		return 0;

	for( i = 0; i < nMaxObj; i++ )
	{
		if( pObject->m_Type == GMT_SKIN )	
			Error( "SlideVectorXZ:스킨오브젝트 발견. 에러%s", m_szFileName ); 

		nMaxFace = pObject->m_nMaxFaceList;
		pIB = pObject->m_pIB;
		pVB = (NORMALVERTEX*)(pObject->m_pVB);
		if( bCollObj == FALSE )
			mTM = pGroup->_mUpdate[i] * mWorld;
		else
			mTM = mWorld;

		pObject++;
		D3DXMatrixInverse( &mInv, NULL, &mTM );
		D3DXVec3TransformCoord( &vInvPos, &vPos, &mInv );	// Line시작점을 Geometry기준으로 변환
		D3DXVec3TransformCoord( &vInvEnd, &vEnd, &mInv );	// Line끝점을 역시 변환
		vInvDir = vInvEnd - vInvPos;
		for( j = 0; j < nMaxFace; j++ )
		{
			v1 = &pVB[ *pIB++ ].position;
			v2 = &pVB[ *pIB++ ].position;
			v3 = &pVB[ *pIB++ ].position;

			bRet = IsTouchRayTri( v1, v2, v3, &vInvPos, &vInvDir, &fDist );
#if __VER >= 11 //  __FIX_COLLISION
			if(bRet)
			{
				vDir = vEnd - vPos;
				float fDirLength = D3DXVec3Length(&vDir);
				// 정확하게 하려면 모든 면을 다 검사해서 가장 가까운것을 골라야 하나
				// S자 형태로 구부러진 면이 없다는 가정하에 간략화 시킨다.
				if(fDirLength > 3.0f)
				{
					if( fabs(fDist) < fDirLength)
					{
						// Line과 닿은 삼각형을 찾아냈다.
						
	//					fDist *= 0.5f;
						if(fDist < 0.0f)
							vIntersect = vPos - fDist * vDir;	// 교차점을 계산.
						else 
							vIntersect = vPos + fDist * vDir;	// 교차점을 계산.
						vA = *v2 - *v1;
						vB = *v3 - *v1;
						D3DXVec3Cross( &vN, &vA, &vB );		// 충돌한 면의 노말 구함. 이건 나중에 미리 계산해두자.
						mInv = mTM;
						mInv._41 = mInv._42 = mInv._43 = 0;
						D3DXVec3TransformCoord( &vN, &vN, &mInv );	// 충돌한면의 노말을 원래대로(mWorld)돌림.

						if( vN.x == 0 && vN.z == 0 )
							Error( "CActionMover::ProcessCollisionGround : 충돌한 면의 노말이 완전 수직이다" );

						vN.y = 0;	// y성분을 없애서 수직면의 법선인것처럼 변환
						D3DXVec3Normalize( &vN, &vN );		// 최종 단위벡터로 변환
						
						// 자, 이제 vIntersect와 vN은 월드좌표계로 준비가 되었다.
						vTemp = vEnd - vIntersect;		// 교차점 - 라인끝 vector V라고 칭함
						*pIntersect = vIntersect;		// 교차점을 받아둠
					
						vInvDir.x = -vDir.x;	vInvDir.y = -vDir.y;	vInvDir.z = -vDir.z;	// -D
						FLOAT fLenN = D3DXVec3Dot( &vInvDir, &vN );		// -D dot N
						vN *= (fLenN * 2.0f);							// (-D dot N) * 2  이때 N는 단위벡터 상태여야한다.
						vTemp = vDir + vN;								// 최종 반사벡터.
						D3DXVec3Scale(&vTemp, &vTemp, 0.2f);					
						
						*pOut = vTemp;		// 미끄러진 벡터를 결과로 받도록 바꿔보자.
						
						return 1;
					}
				}
				else
				{
					if(fDist >= 0.0f)
					{
						if( fDist < 1.0f )
						{
							// Line과 닿은 삼각형을 찾아냈다.
							vDir = vEnd - vPos;
							vIntersect = vPos + fDist * vDir;	// 교차점을 계산.

							vA = *v2 - *v1;
							vB = *v3 - *v1;
							D3DXVec3Cross( &vN, &vA, &vB );		// 충돌한 면의 노말 구함. 이건 나중에 미리 계산해두자.
							mInv = mTM;
							mInv._41 = mInv._42 = mInv._43 = 0;
							D3DXVec3TransformCoord( &vN, &vN, &mInv );	// 충돌한면의 노말을 원래대로(mWorld)돌림.

							if( vN.x == 0 && vN.z == 0 )
								Error( "CActionMover::ProcessCollisionGround : 충돌한 면의 노말이 완전 수직이다" );

							vN.y = 0;	// y성분을 없애서 수직면의 법선인것처럼 변환
							D3DXVec3Normalize( &vN, &vN );		// 최종 단위벡터로 변환
							
							// 자, 이제 vIntersect와 vN은 월드좌표계로 준비가 되었다.
							vTemp = vEnd - vIntersect;		// 교차점 - 라인끝 vector V라고 칭함
							*pIntersect = vIntersect;		// 교차점을 받아둠
							CalcSlideVec( &vTemp, vTemp, vN );
							
							*pOut = vTemp;		// 미끄러진 벡터를 결과로 받도록 바꿔보자.
							
							return 1;
						}
					}
				}
				
			}
		}
	}
	return 0;
#else
			if( bRet && fDist >= 0.0f )	// 반대방향 면은 검사하지 않음.
			{
				// 정확하게 하려면 모든 면을 다 검사해서 가장 가까운것을 골라야 하나
				// S자 형태로 구부러진 면이 없다는 가정하에 간략화 시킨다.
				if( fDist < 1.0f )
				{
					// Line과 닿은 삼각형을 찾아냈다.
					vDir = vEnd - vPos;
//					fDist *= 0.5f;
					vIntersect = vPos + fDist * vDir;	// 교차점을 계산.
					vA = *v2 - *v1;
					vB = *v3 - *v1;
					D3DXVec3Cross( &vN, &vA, &vB );		// 충돌한 면의 노말 구함. 이건 나중에 미리 계산해두자.
					mInv = mTM;
					mInv._41 = mInv._42 = mInv._43 = 0;
					D3DXVec3TransformCoord( &vN, &vN, &mInv );	// 충돌한면의 노말을 원래대로(mWorld)돌림.

					if( vN.x == 0 && vN.z == 0 )
						Error( "CActionMover::ProcessCollisionGround : 충돌한 면의 노말이 완전 수직이다" );

					vN.y = 0;	// y성분을 없애서 수직면의 법선인것처럼 변환
					D3DXVec3Normalize( &vN, &vN );		// 최종 단위벡터로 변환
					
					// 자, 이제 vIntersect와 vN은 월드좌표계로 준비가 되었다.
					vTemp = vEnd - vIntersect;		// 교차점 - 라인끝 vector V라고 칭함
					*pIntersect = vIntersect;		// 교차점을 받아둠
					CalcSlideVec( &vTemp, vTemp, vN );
					
					*pOut = vTemp;		// 미끄러진 벡터를 결과로 받도록 바꿔보자.
//					*pOut = vTemp;
					
					return 1;
				}
			}
		}
	}
	return 0;
#endif		
			
					

					
}


//
// Line과 교차하는 삼각형을 찾아 Slide벡터를 계산.
// 주의 : SlideVector는 스키닝 오브젝트에는 사용금지다.
// 중력벡터의 바닥 미끄러짐계산시 사용하는 전용함수.
// 리커전을 하지 않는다.  - 속도땜에 -
//
#if defined( __SLIDE_060502 )

extern BOOL CanSlide( const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2,
				 float fCosine, D3DXVECTOR3* pOut );

int	CObject3D::SlideVectorUnder( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vEnd , const D3DXMATRIX &mWorld, D3DXVECTOR3 *pIntersect )
{
	D3DXVECTOR3	*v1, *v2, *v3;
	D3DXVECTOR3	vInvPos, vInvEnd, vInvDir;
	D3DXMATRIX	mInv;
	FLOAT	    fDist;
	BOOL	    bRet;
	NORMALVERTEX *pVB;
	WORD		 *pIB;

	GMOBJECT* pObject = &m_CollObject;
	if( pObject->m_Type != GMT_NORMAL )
		return 0;

	pIB = pObject->m_pIB;
	pVB = (NORMALVERTEX*)(pObject->m_pVB);

	D3DXMatrixInverse( &mInv, NULL, &mWorld );
	D3DXVec3TransformCoord( &vInvPos, &vPos, &mInv );	// Line시작점을 Geometry기준으로 변환
	D3DXVec3TransformCoord( &vInvEnd, &vEnd, &mInv );	// Line끝점을 역시 변환
	vInvDir = vInvEnd - vInvPos;

	for( int j = 0; j < pObject->m_nMaxFaceList; ++j )
	{
		v1 = &pVB[ *pIB++ ].position;
		v2 = &pVB[ *pIB++ ].position;
		v3 = &pVB[ *pIB++ ].position;

		bRet = IsTouchRayTri( v1, v2, v3, &vInvPos, &vInvDir, &fDist );
		if( bRet && fDist >= 0.0f )	// 반대방향 면은 검사하지 않음.
		{
			// 정확하게 하려면 모든 면을 다 검사해서 가장 가까운것을 골라야 하나
			// S자 형태로 구부러진 면이 없다는 가정하에 간략화 시킨다.
			if( fDist < 1.0f )
			{
				// Line과 닿은 삼각형을 찾아냈다.
				D3DXVECTOR3 vDir = vEnd - vPos;
				*pIntersect = vPos + fDist * vDir;	// 교차점을 계산.

				// 101도 이상의 경사면만 미끄러진다.
				D3DXVECTOR3 vSlide;
				if( CanSlide( *v1, *v2, *v3, -0.19080f, &vSlide ) == FALSE )	// -0.19080f = cos(101)
				{
					*pOut = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
					return 1;
				}
				
				// 미끄러진 좌표를 얻는다.
				D3DXVec3TransformNormal( &vSlide, &vSlide, &mWorld );
				*pOut = *pIntersect + vSlide;
				return 1;
			}
		}
	}
	return 0;
}
#else // __SLIDE_060502

int		CObject3D::SlideVectorUnder( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vEnd , const D3DXMATRIX &mWorld, D3DXVECTOR3 *pIntersect )
{
	D3DXVECTOR3	*v1, *v2, *v3;
	D3DXVECTOR3	vInvPos, vInvEnd, vInvDir, vDir, vIntersect;
	D3DXVECTOR3 vA, vB, vN, vTemp;
	D3DXMATRIX	mTM, mInv;
	int		nMaxFace;
	GMOBJECT* pObject = &m_CollObject;
//	GMOBJECT* pObject = m_Group[0].m_pObject;
	NORMALVERTEX *pVB;
	WORD		*pIB;
	int		j;
	FLOAT	fDist;
	BOOL	bRet;

//	if( _mUpdate == NULL )		return FAIL;

	if( pObject->m_Type == GMT_ERROR )
		return 0;

//	for( i = 0; i < nMaxObj; i++ )
	{
		if( pObject->m_Type == GMT_SKIN )	
			Error( "SlideVectorUnder:스킨오브젝트 발견. 에러%s", m_szFileName ); 

		nMaxFace = pObject->m_nMaxFaceList;
		pIB = pObject->m_pIB;
		pVB = (NORMALVERTEX*)(pObject->m_pVB);
		pObject++;
		mTM = mWorld;		// 툴에선 LocalTM을 쓰지 않는다.
//		mTM = m_Group[0].m_pObject[0].m_mLocalTM * mWorld;

		D3DXMatrixInverse( &mInv, NULL, &mTM );
		D3DXVec3TransformCoord( &vInvPos, &vPos, &mInv );	// Line시작점을 Geometry기준으로 변환
		D3DXVec3TransformCoord( &vInvEnd, &vEnd, &mInv );	// Line끝점을 역시 변환
		vInvDir = vInvEnd - vInvPos;
		for( j = 0; j < nMaxFace; j++ )
		{
			v1 = &pVB[ *pIB++ ].position;
			v2 = &pVB[ *pIB++ ].position;
			v3 = &pVB[ *pIB++ ].position;

			bRet = IsTouchRayTri( v1, v2, v3, &vInvPos, &vInvDir, &fDist );

			if( bRet && fDist >= 0.0f )	// 반대방향 면은 검사하지 않음.
			{
				// 정확하게 하려면 모든 면을 다 검사해서 가장 가까운것을 골라야 하나
				// S자 형태로 구부러진 면이 없다는 가정하에 간략화 시킨다.
				if( fDist < 1.0f )
				{
					// Line과 닿은 삼각형을 찾아냈다.
					vDir = vEnd - vPos;
					*pIntersect = vPos + fDist * vDir;	// 교차점을 계산.
	
					vA = *v2 - *v1;		// 노말 계산 시작.
					vB = *v3 - *v1;
					D3DXVec3Cross( &vTemp, &vA, &vB );		// 충돌한 면의 노말 구함. 이건 나중에 미리 계산해두자.
					D3DXVec3Normalize( &vN, &vTemp );		// 단위벡터로 변환

					mInv = mTM;
					mInv._41 = mInv._42 = mInv._43 = 0;
					D3DXVec3TransformCoord( &vTemp, &vN, &mInv );	// 충돌한면의 노말을 원래대로(mWorld)돌림.
					vN = vTemp;		// 충돌면 노말.

					vDir.x = vDir.z = 0;	vDir.y = -1.0f;
					
					// 일정각 이상 기울어진 면만 슬라이드를 적용시킨다. 두벡터다 노말이어야 한다.
					if( D3DXVec3Dot( &vN, &vDir ) < -0.19080f )		// -0.19080f = cos(101)
						return 1;
					vDir.y = -0.01f;

					// 자, 이제 vIntersect와 vN은 월드좌표계로 준비가 되었다.
					CalcSlideVec( &vTemp, vDir, vN );	// 충돌점에서 미끄러진부분까지의 벡터
					*pOut = *pIntersect + vTemp;
					return 1;
				}
			}
		}
	}
	return 0;
}
#endif // not __SLIDE_060502

//
// Line과 교차하는 삼각형을 찾아 리턴
// pTriOut : D3DXVECTOR3 *pTri[3]; 의 시작 포인터.
// 주의 : 스키닝 오브젝트에는 사용 금지.
//
void CObject3D::FindTouchTriLine( D3DXVECTOR3 **pTriOut, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vEnd , const D3DXMATRIX &mWorld, FLOAT *pDist, BOOL bCollObj )
{
	D3DXVECTOR3	*v1, *v2, *v3;
	D3DXVECTOR3	vInvPos, vInvEnd, vInvDir, vDir;
	D3DXVECTOR3 vA, vB, vN, vTemp;
	D3DXMATRIX	mTM, mInv;
	int		nMaxFace;
	GMOBJECT* pObject = &m_CollObject;
	NORMALVERTEX *pVB;

	int i;
	LOD_GROUP *pGroup = &m_Group[0];
	int		nMaxObj = 1;
	if( bCollObj == FALSE )		// 충돌메시로 충돌검사 하지마라.
	{
		pObject = pGroup->m_pObject;
		nMaxObj = pGroup->m_nMaxObject;
		if( pGroup->_mUpdate == NULL )		
			return;
	}
	
	if( pObject->m_Type == GMT_ERROR )
	{
		*pTriOut = NULL;
		return;
	}

	WORD	*pIB;
	int		j;
	BOOL	bRet;

	for( i = 0; i < nMaxObj; i++ )
	{
		if( pObject->m_Type == GMT_SKIN )	
			Error( "FindTouchTriLine:스킨오브젝트 발견. 에러%s", m_szFileName );

		nMaxFace = pObject->m_nMaxFaceList;
		pVB = (NORMALVERTEX*)(pObject->m_pVB);
		pIB	= pObject->m_pIB;
		pObject++;
		if( bCollObj == FALSE )
			mTM = pGroup->_mUpdate[i] * mWorld;
		else
			mTM = mWorld;		// 충돌용 메시에는 LocalTM이 없다.

		D3DXMatrixInverse( &mInv, NULL, &mTM );
		D3DXVec3TransformCoord( &vInvPos, &vPos, &mInv );	// Line시작점을 Geometry기준으로 변환
		D3DXVec3TransformCoord( &vInvEnd, &vEnd, &mInv );	// Line끝점을 역시 변환
		vInvDir = vInvEnd - vInvPos;
		for( j = 0; j < nMaxFace; j++ )
		{
			v1 = &pVB[ *pIB++ ].position;
			v2 = &pVB[ *pIB++ ].position;
			v3 = &pVB[ *pIB++ ].position;

			bRet = IsTouchRayTri( v1, v2, v3, &vInvPos, &vInvDir, pDist );

			if( bRet && *pDist >= 0.0f )	// 반대방향 면은 검사하지 않음.
			{
				// 정확하게 하려면 모든 면을 다 검사해서 가장 가까운것을 골라야 하나
				// S자 형태로 구부러진 면이 없다는 가정하에 간략화 시킨다.
				if( *pDist < 1.0f )
				{
					// Line과 닿은 삼각형을 찾아냈다.
					// 이 좌표들 mTM으로 곱해서 넘겨야 하는거 아닌가?.
					pTriOut[0] = v1;
					pTriOut[1] = v2;
					pTriOut[2] = v3;
					return;
				}
			}
		}
	}

	*pTriOut = NULL;
}


#ifdef __CLIENT
// intersectRayTri에서 교차한 삼각형 데이타가 이곳으로 임시로 들어온다.
static D3DXVECTOR3	s_vLastPickTri[3];
D3DXVECTOR3 *GetLastPickTri( void )
{
	return s_vLastPickTri;
}
#endif // __CLIENT

// 용량이 큰 이유는 스킨오브젝트는 뼈대변환된 버텍스를 다시 계산 해야하므로 그 변환된 버텍스가 vPool로 들어온다.
static D3DXVECTOR3	_vPool[4696];

#if __VER >= 13 // __HOUSING

void	CObject3D::ComputeInterval(float fVV0,float fVV1,float fVV2,float fD0,float fD1,float fD2,float fD0D1,float fD0D2,float &fA,float &fB,float &fC,float &fX0,float &fX1)
{
	if(fD0D1 > 0.0f) 
	{ 
		fA = fVV2; fB = (fVV0 - fVV2) * fD2; fC = (fVV1 - fVV2) * fD2; fX0 = fD2 - fD0; fX1 = fD2 - fD1; 
	}
	else if(fD0D2 > 0.0f)
	{
		fA = fVV1; fB = (fVV0 - fVV1) * fD1; fC = (fVV2 - fVV1) * fD1; fX0 = fD1 - fD0; fX1 = fD1 - fD2;
	}
	else if(fD1 * fD2 > 0.0f || fD0 != 0.0f)
	{
		fA = fVV0; fB = (fVV1 - fVV0) * fD0; fC = (fVV2 - fVV0) * fD0; fX0 = fD0 - fD1; fX1 = fD0 - fD2;
	}
	else if(fD1 != 0.0f) 
	{
		fA = fVV1; fB = (fVV0 - fVV1) * fD1; fC = ( fVV2 - fVV1) * fD1; fX0 = fD1 - fD0; fX1 = fD1 - fD2;
	}
	else if(fD2 != 0.0f)
	{
		fA = fVV2; fB = (fVV0 - fVV2) * fD2; fC = (fVV1 - fVV2) * fD2; fX0 = fD2 - fD0; fX1 = fD2 - fD1;
	}
	else
	{
		return;
	}

}

BOOL	CObject3D::SimpleTriIntersect(D3DXMATRIX mWorld, GMOBJECT* pTargetObj, D3DXMATRIX mTargetWorld)
{

	D3DXVECTOR3	v1, v2, v3, tv1, tv2, tv3;
	D3DXMATRIX	mTM;
	int			nMax, nMax2;
	int			nMaxObj = m_Group[0].m_nMaxObject;
	GMOBJECT*	pObj = &m_Group[0].m_pObject[0];
	WORD		*pIB, *pIB2;
	int			i, j, k;
	NORMALVERTEX *pVB, *pVB2;
	
	if( m_CollObject.m_Type != GMT_ERROR )	// 충돌메시가 없으면 걍 함.
	{
		pObj = &m_CollObject;
		nMaxObj = 1;	// 충돌메시는 무조건 1개다.
	}

	// 스킨 오브젝트는 없을테니까 제끼고 하자...시간상
	for( i = 0; i < nMaxObj; i++ )
	{
		
		mTM = m_Group[0]._mUpdate[i] * mWorld;
		
		nMax = pObj->m_nMaxFaceList;
		pVB	= (NORMALVERTEX*)(pObj->m_pVB);
		pIB	= pObj->m_pIB;
		for( j = 0; j < nMax; j++ )
		{
			// 로컬좌표계에서 월드좌표로 변환..
			D3DXVec3TransformCoord( &v1, &pVB[ *pIB++ ].position, &mTM );
			D3DXVec3TransformCoord( &v2, &pVB[ *pIB++ ].position, &mTM );
			D3DXVec3TransformCoord( &v3, &pVB[ *pIB++ ].position, &mTM );
			
			D3DXVECTOR3	vw1, vw2, vNormal;
			float		fD;
			// 첫번째 폴리곤의 노멀을 구한다
			D3DXVec3Subtract(&vw1, &v2, &v1);
			D3DXVec3Subtract(&vw2, &v3, &v1);
			D3DXVec3Cross(&vNormal, &vw1, &vw2);
			// 평면의 방정식 1: N1.X+d1=0 
			fD = -D3DXVec3Dot(&vNormal, &v1);	// 평면의 방정식 D값 구함
			
			// 타겟오브젝트도 폴리곤 뽑자..
			nMax2 = pTargetObj->m_nMaxFaceList;
			pVB2  = (NORMALVERTEX*)(pTargetObj->m_pVB);
			pIB2  = pTargetObj->m_pIB;
			for( k = 0; k < nMax2; k++ )
			{
				// 주의! 오브젝트가 하나인 경우로 가정하고 했다..아니라면 다시 오브젝트수만큼 루프돌리고 매개변수 바꿔서 다시 짜야된다
				// 로컬좌표계에서 월드좌표로 변환..
				float fdu0, fdu1, fdu2, ftdu0, ftdu1, ftdu2, fdu01, fdu02, ftdu01, ftdu02; 
				D3DXVECTOR3	vtw1, vtw2, vtNormal, vCross;
				float		fD2;
				// 축관련
				float		fXLen, fYLen, fZLen, fCood1, fCood2, fCood3, ftCood1, ftCood2, ftCood3;

				D3DXVec3TransformCoord( &tv1, &pVB2[ *pIB2++ ].position, &mTargetWorld );
				D3DXVec3TransformCoord( &tv2, &pVB2[ *pIB2++ ].position, &mTargetWorld );
				D3DXVec3TransformCoord( &tv3, &pVB2[ *pIB2++ ].position, &mTargetWorld );
				
				// 다른 폴리곤의 각 정점에 대해서 계산해서 어느쪽에 있는지 판단
				fdu0 = D3DXVec3Dot(&vNormal, &tv1) + fD;
				fdu1 = D3DXVec3Dot(&vNormal, &tv2) + fD;
				fdu2 = D3DXVec3Dot(&vNormal, &tv3) + fD;

				fdu01 = fdu0*fdu1;
				fdu02 = fdu0*fdu2;
				if(fdu01  > 0.0f && fdu02 > 0.0f) 
					continue;                    // 모두 부호가 같으면 같은쪽에 있는것이므로 교차없음
				
				D3DXVec3Subtract(&vtw1, &tv2, &tv1);
				D3DXVec3Subtract(&vtw2, &tv3, &tv1);
				D3DXVec3Cross(&vtNormal, &vtw1, &vtw2);
				// 평면의 방정식 1: N1.X+d1=0 
				fD2 = -D3DXVec3Dot(&vtNormal, &tv1);	// 평면의 방정식 D값 구함

				// 다른 폴리곤의 각 정점에 대해서 계산해서 어느쪽에 있는지 판단
				ftdu0 = D3DXVec3Dot(&vtNormal, &v1) + fD2;
				ftdu1 = D3DXVec3Dot(&vtNormal, &v2) + fD2;
				ftdu2 = D3DXVec3Dot(&vtNormal, &v3) + fD2;

				ftdu01 = ftdu0*ftdu1;
				ftdu02 = ftdu0*ftdu2;
				if(ftdu01  > 0.0f && ftdu02 > 0.0f) 
					continue;                    // 모두 부호가 같으면 같은쪽에 있는것이므로 교차없음
				
				// 교차선의 방향을 구함 
				D3DXVec3Cross(&vCross,&vNormal,&vtNormal);

				// 교차선의 성분중 가장 긴 축을 구함
				fXLen = fabs(vCross.x);
				fYLen = fabs(vCross.y);
				fZLen = fabs(vCross.z);

				// 그축에 두개의 삼각형을 투영함
				if(fYLen > fXLen && fYLen > fZLen) // y
				{
					fCood1 = v1.y;
					fCood2 = v2.y;
					fCood3 = v3.y;
					ftCood1 = tv1.y;
					ftCood2 = tv2.y;
					ftCood3 = tv3.y;
				}
				if(fZLen > fXLen && fZLen > fYLen) // z
				{
					fCood1 = v1.z;
					fCood2 = v2.z;
					fCood3 = v3.z;
					ftCood1 = tv1.z;
					ftCood2 = tv2.z;
					ftCood3 = tv3.z;
				}
				else								// x
				{
					fCood1 = v1.x;
					fCood2 = v2.x;
					fCood3 = v3.x;
					ftCood1 = tv1.x;
					ftCood2 = tv2.x;
					ftCood3 = tv3.x;
				}

				float a,b,c,x0,x1, d,e,f,y0,y1;
				float xx,yy,xxyy,tmp;
				float isect1[2], isect2[2];

				// 폴리곤1의 간격 계산
				ComputeInterval(fCood1,fCood2,fCood3,fdu0,fdu1,fdu2,fdu01,fdu02,a,b,c,x0,x1);

				// 폴리곤2의 간격 계산
				ComputeInterval(ftCood1,ftCood2,ftCood3,ftdu0,ftdu1,ftdu2,ftdu01,ftdu02,d,e,f,y0,y1);

				xx = x0 * x1;
				yy = y0 * y1;
				xxyy = xx * yy;

				tmp = a * xxyy;
				isect1[0] = tmp + b * x1 * yy;
				isect1[1] = tmp + c * x0 * yy;

				tmp = d * xxyy;
				isect2[0] = tmp + e * xx * y1;
				isect2[1] = tmp + f * xx * y0;

				if(isect1[0] > isect1[1])
				 tmp = isect1[0];
				 isect1[0] = isect1[1];
				 isect1[1] = tmp;
               
				if(isect2[0] > isect2[1])
				 tmp = isect2[0];
				 isect2[0] = isect2[1];
				 isect2[1] = tmp;

				if(isect1[1] < isect2[0] || isect2[1] < isect1[0]) continue;
			
				return TRUE;
			}
		}	
		pObj++;
	}
	
	return FALSE;
}
#endif // __HOUSING
//
// 레이와 교차한 삼각형의 시작포인터를 리턴.
//
D3DXVECTOR3 *CObject3D::IntersectRayTri( const D3DXVECTOR3 &vRayOrig, const D3DXVECTOR3 &vRayDir, const D3DXMATRIX &mWorld, 
											  D3DXVECTOR3* pvIntersect, FLOAT* pfDist, BOOL bColl )
{
	D3DXVECTOR3	*v1, *v2, *v3, *v4;
	D3DXVECTOR3	vInvRayOrig, vInvRayDir, vw1, vw2;
	D3DXMATRIX	mTM, mInv, *pBone, *pBoneInv;
	int		nMax;
	int		nMaxObj = m_Group[0].m_nMaxObject;
	GMOBJECT* pObj = &m_Group[0].m_pObject[0];
	WORD		*pIB, *pIIB;
	int		i, j, nVIdx;
	FLOAT	fDist, fMinDist = 65535.0f;
	BOOL	bRet, bFlag = FALSE;

	if( bColl )		// 충돌메시로 검사하게 하는 옵션
	{
		if( m_CollObject.m_Type != GMT_ERROR )	// 충돌메시가 없으면 걍 함.
		{
			pObj = &m_CollObject;
			nMaxObj = 1;	// 충돌메시는 무조건 1개다.
		}
	}

	for( i = 0; i < nMaxObj; i++ )
	{
		if( pObj->m_Type == GMT_SKIN )	
		{
			SKINVERTEX *pVB;
			mTM = /*_mUpdate[i] * */mWorld;
			D3DXMatrixInverse( &mInv, NULL, &mTM );
			D3DXVec3TransformCoord( &vInvRayOrig, &vRayOrig, &mInv );
			
			mInv._41 = 0;	mInv._42 = 0;	mInv._43 = 0;	// vRayDir은 방향벡터만 있기땀시 위치는 없애준다.
			D3DXVec3TransformCoord( &vInvRayDir, &vRayDir, &mInv );
			int *pPhysique = pObj->m_pPhysiqueVertex;
			D3DXVECTOR3	*pVList = pObj->m_pVertexList;
			pIB	= pObj->m_pIB;
			pIIB = pObj->m_pIIB;
			pVB = (SKINVERTEX *)pObj->m_pVB;
			if( m_pmExternBone )	
			{
				pBone = m_pmExternBone;		// 외부지정 뼈대가 있다면 그걸 쓰고
				pBoneInv = m_pmExternBoneInv;
			}
			else					
			{
				pBone = m_pBaseBone;		// 없다면 디폴트를 쓴다.
				pBoneInv = m_pBaseBoneInv;
			}
			nMax = pObj->m_nMaxVB;

			if( nMax > sizeof(_vPool) / sizeof(D3DXVECTOR3) )
				Error( "IntersectRayTri : 버텍스버퍼의 개수가 너무 크다 %d", nMax );

			int	matIdx;
			v4 = _vPool;
			for( j = 0; j < nMax; j ++ )
			{
				nVIdx = *pIIB++;
//				D3DXVec3TransformCoord( v4, &pVList[nVIdx], &pBone[ pPhysique[nVIdx] ] );
				matIdx = pPhysique[ nVIdx ];	// 2link는 무시하고 그중 하나만 쓴다. 그래서 계산이 정확하지 않을 수 있다,.
				mTM = pBoneInv[ matIdx ] * pBone[ matIdx ];
				D3DXVec3TransformCoord( v4, &pVB[j].position, &mTM );
				v4 ++;
			}
			nMax = pObj->m_nMaxFaceList;
			v4 = _vPool;	// pVB
			for( j = 0; j < nMax; j++ )
			{
				v1 = &v4[ *pIB++ ];
				v2 = &v4[ *pIB++ ];
				v3 = &v4[ *pIB++ ];

				bRet = IsTouchRayTri( v1, v2, v3, &vInvRayOrig, &vInvRayDir, &fDist );
				if( bRet && fDist >= 0.0f )	// 반대방향 면(-)은 검사하지 않음.
				{
					if( fDist < fMinDist )	// 레이원점으로부터 가장 가까운 거리를 찾음.
					{
						fMinDist = fDist;
						bFlag = TRUE;
					#ifdef __CLIENT
						s_vLastPickTri[0] = *v1;	s_vLastPickTri[1] = *v2;	s_vLastPickTri[2] = *v3;
					#endif
					}
				}
			}
		} else
		{  // non-skin object
			NORMALVERTEX *pVB;
			mTM = m_Group[0]._mUpdate[i] * mWorld;
			D3DXMatrixInverse( &mInv, NULL, &mTM );
			D3DXVec3TransformCoord( &vInvRayOrig, &vRayOrig, &mInv );
			
			mInv._41 = 0;	mInv._42 = 0;	mInv._43 = 0;	// vRayDir은 방향벡터만 있기땀시 위치는 없애준다.
			D3DXVec3TransformCoord( &vInvRayDir, &vRayDir, &mInv );
			nMax = pObj->m_nMaxFaceList;
			pVB	= (NORMALVERTEX*)(pObj->m_pVB);
			pIB	= pObj->m_pIB;
			for( j = 0; j < nMax; j++ )
			{
				v1 = &pVB[ *pIB++ ].position;
				v2 = &pVB[ *pIB++ ].position;
				v3 = &pVB[ *pIB++ ].position;

				bRet = IsTouchRayTri( v1, v2, v3, &vInvRayOrig, &vInvRayDir, &fDist );

				if( bRet && fDist >= 0.0f )	// 반대방향 면(-)은 검사하지 않음.
				{
					if( fDist < fMinDist )	// 레이원점으로부터 가장 가까운 거리를 찾음.
					{
						fMinDist = fDist;
						bFlag = TRUE;
					#ifdef __CLIENT
						D3DXVec3TransformCoord( &s_vLastPickTri[0], v1, &m_Group[0]._mUpdate[i] );
						D3DXVec3TransformCoord( &s_vLastPickTri[1], v2, &m_Group[0]._mUpdate[i] );
						D3DXVec3TransformCoord( &s_vLastPickTri[2], v3, &m_Group[0]._mUpdate[i] );
					#endif	// __CLIENT
					}
				}
			}
		}
		pObj++;

	}
	if( bFlag )	// 한번이라도 교차된 삼각형을 찾았다면.
	{
		*pvIntersect = vRayOrig + fMinDist * vRayDir;		// 교차했던 가장 가까운거리로 충돌지점 계산.
		return _vPool;		// 그 삼각형 좌표 리턴
	}
	
	return NULL;
}

void	CObject3D::SetTexture( LPCTSTR szTexture )
{
#if !defined(__WORLDSERVER)
	MATERIAL	*pMtrl;
	D3DMATERIAL9	mMtrl;

	pMtrl = g_TextureMng.AddMaterial( m_pd3dDevice, &mMtrl, szTexture );
	m_Group[0].m_pObject[0].m_pMtrlBlkTexture[0] = pMtrl->m_pTexture;
#endif
}

void	CObject3D::SetTexture( LPDIRECT3DTEXTURE9 pTexture )
{
#if !defined(__WORLDSERVER)
	m_Group[0].m_pObject[0].m_pMtrlBlkTexture[0] = pTexture;
#endif
}

void	CObject3D::LoadTextureEx( int nNumEx, GMOBJECT *pObj, MATERIAL *pmMaterial[16] )
{
#if !defined(__WORLDSERVER)
	int		i;
	char	szTexture[MAX_PATH];
	char	szFileExt[MAX_PATH];
	char	szNum[16];
	D3DMATERIAL9	mMaterial;

	memset( &mMaterial, 0, sizeof(mMaterial) );

	// 사용하는 텍스쳐에 -et를 붙여서 확장 텍스쳐를 읽어들임.
	for( i = 0; i < pObj->m_nMaxMaterial; i ++ )
	{
		if( IsEmpty( pObj->m_MaterialAry[i].strBitMapFileName ) )	continue;
		strcpy( szTexture, pObj->m_MaterialAry[i].strBitMapFileName );		// mvr_bang.dds
//		strcpy( szTexture, ::GetFileTitle( pObj->m_MaterialAry[i].strBitMapFileName ) );	// mvr_bang
		GetFileTitle( pObj->m_MaterialAry[i].strBitMapFileName, szTexture );	// mvr_bang
		strcat( szTexture, "-et" );		// -et. 를 붙임.  mvr_bang-et.
		sprintf( szNum, "%02d.", nNumEx );	
		strcat( szTexture, szNum );			// -et01 ~ 07
//		strcat( szTexture, ::GetFileExt( pObj->m_MaterialAry[i].strBitMapFileName ) );		// mvr_bang-et.dds
		GetFileExt( pObj->m_MaterialAry[i].strBitMapFileName, szFileExt );	// mvr_bang-et.dds
		lstrcat( szTexture, szFileExt );

		if( IsEmpty(szTexture) == FALSE )
			pmMaterial[i] = g_TextureMng.AddMaterial( m_pd3dDevice, &mMaterial, szTexture );
	}
#endif // !__WORLDSERVER
}

// 텍스쳐를 확장텍스쳐로 지정한다.
// 기본은 확작텍스쳐를 로딩하지 않지만 요청이 들어오면
// 텍스쳐를 로딩하고 그포인트로 기본포인터로 바꾼다.
void	CObject3D::SetTextureEx( GMOBJECT *pObj, int nNumEx )
{
#if !defined(__WORLDSERVER)
	LPDIRECT3DTEXTURE9	*pTextureEx;		// 매터리얼 블럭내 텍스쳐포인터
	int		i;
	MATERIAL	*mMaterial[16];
	int		nID;
	
	if( nNumEx >= 8 )
	{
		Error( "CObject3D::SetTextureEx : nNumEx = %d", nNumEx );
		return;
	}
	pTextureEx = pObj->m_pMtrlBlkTexture + (pObj->m_nMaxMtrlBlk * nNumEx);		// 확장부분 포인터.
	
//	if( bUse )
	if( 1 )
	{
		memset( mMaterial, 0, sizeof(mMaterial) );
		
		if( pTextureEx[0] == NULL )		// 확장 텍스쳐가 로딩된적이 없다.
		{
			LoadTextureEx( nNumEx, pObj, mMaterial );		// 확장텍스쳐를 로딩함.
			for( i = 0; i < pObj->m_nMaxMtrlBlk; i ++ )
			{
				nID = pObj->m_pMtrlBlk[i].m_nTextureID;
				if( mMaterial[ nID ] )
					pTextureEx[i] = mMaterial[ nID ]->m_pTexture;		// 확장텍스쳐를 로딩함.
			}
		}
	}
	else
	{
		for( i = 0; i < pObj->m_nMaxMtrlBlk; i ++ )
			pTextureEx[i] = NULL;
	}
	
	m_nTextureEx = nNumEx;
#endif
}

void	CObject3D :: ClearNormal( void )
{
/*	int		i;
	D3DXVECTOR3		n = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

	for( int j = 0; j < m_nMaxObject; j++ )
	{
		GMOBJECT* pObject = &m_pObject[j];
		if( pObject->m_Type == GMT_SKIN )
		{
			SKINVERTEX *pVB = (SKINVERTEX*)(pObject->m_pVB);
			for( i = 0; i < pObject->m_nMaxVB; i ++ )		pVB[i].normal = n;
		}
		else
		{
			NORMALVERTEX *pVB = (NORMALVERTEX*)(pObject->m_pVB);
			for( i = 0; i < pObject->m_nMaxVB; i ++ )		pVB[i].normal = n;
		}

		SendVertexBuffer( pObject, NULL );
	}
	*/
}

//
// Animate()수행후 곧바로 Render를 호출해야 한다.
// 임시버퍼가 손상될 수 있기 때문이다.
//
// 스킨 
//
void	CObject3D::Animate( FLOAT fFrameCurrent, int nNextFrame )
{
#if !defined(__WORLDSERVER)
	int		i;
	TM_ANIMATION	*pFrame = NULL;					// 현재 프레임
	TM_ANIMATION	*pNext = NULL;					// 다음 프레임
	D3DXQUATERNION	qSlerp;
	D3DXVECTOR3		vSlerp;
	int		nCurrFrame;
	float	fSlp;
	D3DXMATRIX	m1, m2;
	D3DXMATRIX *mUpdate = m_pGroup->_mUpdate;
	D3DXMATRIX *mParent;

	GMOBJECT	*pObj;
	int nMaxObj = m_pGroup->m_nMaxObject;
	for( i = 0; i < nMaxObj; i ++ )
	{
		pObj = &m_pGroup->m_pObject[i];
		if( pObj->m_ParentType == GMT_BONE )
		{
			if( m_pmExternBone )	mParent = m_pmExternBone;
			else					mParent = m_pBaseBone;
		} else
			mParent = m_pGroup->_mUpdate;
		if( m_nMaxFrame > 0 )	// 프레임이 있을때
		{
			if( pObj->m_pFrame )
			{
				// 보간을 위한 Slerp 계산.
				nCurrFrame = (int)fFrameCurrent;			// 소숫점 떼내고 정수부만..
				fSlp = fFrameCurrent - (float)nCurrFrame;	// 소숫점 부분만 가져옴

				pFrame = &pObj->m_pFrame[ nCurrFrame ];		// 현재 프레임 포인터 받음
				pNext  = &pObj->m_pFrame[ nNextFrame ];		// 다음 프레임 포인터 받음

				D3DXQuaternionSlerp( &qSlerp, &pFrame->m_qRot, &pNext->m_qRot, fSlp );		// 회전 보간
				D3DXVec3Lerp( &vSlerp, &pFrame->m_vPos, &pNext->m_vPos, fSlp );					// 벡터 보간

				// matAniTM계산
				// 이동행렬 만듦
				D3DXMatrixTranslation( &m1,	 vSlerp.x,  vSlerp.y,  vSlerp.z );
			
				// 쿼터니온 보간된 회전키값을 회전행렬로 만듦
				D3DXMatrixRotationQuaternion( &m2, &qSlerp );
				mUpdate[i] = m2 * m1;		// 이동행렬 X 회전행렬 = 애니행렬

				if( pObj->m_nParentIdx != -1 )
					mUpdate[i] *= mParent[ pObj->m_nParentIdx ];
//				else
//					mUpdate[i] *= *mCenter;		// 루트는 센터랑 최초 곱함.
					
			} else
			// 이 오브젝트에 프레임이 없을때
			{
				if( pObj->m_nParentIdx != -1 )	// 부모가 있는 경우
					mUpdate[i] = pObj->m_mLocalTM * mParent[ pObj->m_nParentIdx ];
				else
					mUpdate[i] = pObj->m_mLocalTM;					// 부모가 없다면 루트다.
//					mUpdate[i] = pObj->m_mLocalTM * *mCenter;		// 부모가 없다면 루트다. 루트는 센터랑 최초 곱함.
			}
		} else
		// 프레임이 없을때.
		{
			if( pObj->m_nParentIdx != -1 )
				mUpdate[i] = pObj->m_mLocalTM * mParent[ pObj->m_nParentIdx ];
			else
				mUpdate[i] = pObj->m_mLocalTM;		// 부모가 없다면 루트다. 
//				mUpdate[i] = pObj->m_mLocalTM * *mCenter;		// 부모가 없다면 루트다. 루트는 센터랑 최초 곱함.
		}

		// mUpdate[i]  최종 WorldTM
	}
#endif // __WORLDSERVER
}

//
// 스키닝.
// 본의 애니메이션이 끝난후 뼈대와 로컬버텍스를 곱하여 최종 버텍스좌표를 계산한다.
// mBones : 계산이 끝난 뼈대들의 매트릭스 
//
void	CObject3D::Skining( GMOBJECT *pObj, const D3DXMATRIX *mBones )
{
/*
	D3DXVECTOR3	*vLocal, *vWorld;
	int			*pPhysique;
	int			nMax;

	// 버텍스 갯수만큼 돈다.
	// 스킨의 각 버텍스들은 자기가 소속된 BONE의 최종결과 매트릭스와 자기로컬 좌표를 곱해서
	// 최종 좌표를 생성해낸다.
	vLocal	  = pObj->m_pVertexList;
	pPhysique = pObj->m_pPhysiqueVertex;
	nMax	  = pObj->m_nMaxVertexList;
	vWorld	  = pObj->_pVertexUpdate;
	while( nMax-- )
	{
		D3DXVec3TransformCoord( vWorld, vLocal, &mBones[ *pPhysique ] );		// 일단은 영향받는 bone을 한개만 쓴다.
		// 여기서 노말도 같이 돌려야 한다.
		vLocal ++;
		vWorld ++;
		pPhysique ++;
	}
*/
}

// 본의 변환이 모두 끝난후 실행된다.
// 피지크 버텍스들을 본에 맞춰 다시 계산해서 월드 좌표로 생성
// 월드좌표로 생성된 버텍스들을 버텍스 버퍼에 카피
HRESULT		CObject3D::SetVertexBuffer( GMOBJECT *pObj )
{
/*
	CUSTOMVERTEX	*_pVB = pObj->_pVB;			// 스키닝을 위한 임시버퍼
	WORD			*pIB;
	D3DXVECTOR3		*pVList;
	int				nMax = pObj->m_nMaxVB;		// 

	// WorldPos값만 갱신되면 되므로 
	// Skining()에서 계산된 WorldPos값만 카피시켜 준다.
	pIB    = pObj->m_pIIB;
	pVList = pObj->_pVertexUpdate;
	while( nMax-- )
	{
		_pVB->position = pVList[ *pIB ];
		_pVB ++;
		pIB ++;
	}
*/
	return S_OK;
}

// 스키닝으로 갱신된 버텍스버퍼를 d3d버텍스 버퍼로 전송.
//
HRESULT CObject3D::SendVertexBuffer( GMOBJECT *pObj, LPDIRECT3DVERTEXBUFFER9 pd3d_VB )
{
	LPDIRECT3DDEVICE9 pd3dDevice = m_pd3dDevice;
	HRESULT	hr;
	VOID*	pVertices;
	int		nMax;
	int		nVertexSize;
	
	nVertexSize = pObj->m_nVertexSize;
	
	// 버텍스 버퍼 전송
	nMax = pObj->m_nMaxVB * nVertexSize;
/*
	char buff[256] = { 0 };
	sprintf( buff, "%s\t%d,%d : TYPE : %d\n", m_szFileName, pObj->m_nMaxVB, pObj->m_nVertexSize, pObj->m_VBType );
	OutputDebugString( buff );
*/				
				
	if( FAILED( hr = pd3d_VB->Lock( 0, nMax, (void**)&pVertices, 0 ) ) )		// send vertex buffer
		return hr;
	memcpy( pVertices, pObj->m_pVB, nMax );	
	
	pd3d_VB->Unlock();
	
	return S_OK;
}

//
//
//
HRESULT CObject3D::SendIndexBuffer( GMOBJECT *pObj )
{
	LPDIRECT3DDEVICE9 pd3dDevice = m_pd3dDevice;
	VOID*	pVertices;
	int		nMax;

	// 인덱스 버퍼 전송
	nMax = pObj->m_nMaxIB * sizeof(WORD);
	if( FAILED(	pObj->m_pd3d_IB->Lock( 0, nMax, (void**)&pVertices, 0 ) ) )	// send index buffer
		return FAIL;
	
	memcpy( pVertices, pObj->m_pIB, nMax );	
	pObj->m_pd3d_IB->Unlock();

	return S_OK;
}


#ifndef __WORLDSERVER
void CObject3D::SetState( MATERIAL_BLOCK* pBlock, int nEffect, DWORD dwBlendFactor )
{
	// set
	if( pBlock->m_dwEffect & XE_2SIDE )
		m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	//m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

	if( m_nNoEffect == 1 )	return;
	
	if( dwBlendFactor != 0xff000000 ) // 
	//if( nBlendFactor < 255 ) // 
	{
		if( m_nNoEffect )	return;
		DWORD dwBlendF =  dwBlendFactor;
		//m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF, 0 );
		//m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );		// 특정 알파키값 뺌.
		//m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		//m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0xb0 );
		//pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA  );
		//pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		//m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( nBlendF, 0, 0, 0) );
		m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, dwBlendF );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
		//pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
		//pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF, 0 );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );		// 특정 알파키값 뺌.
		
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR  );
	}
	else
	if( (pBlock->m_dwEffect & XE_OPACITY)  )	// 알파채널
	{
		m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0xb0 );
		//m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_ALWAYS   );		
		//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		if( m_nNoEffect )	return;
		
		if( pBlock->m_nAmount < 255 )		// 
		{
			m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( pBlock->m_nAmount, 0, 0, 0) );
			m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
			m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF, 0 );
			m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );		// 특정 알파키값 뺌.
			
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR  );
		}
		
	}	

#ifdef __BS_EFFECT_LUA
	if( nEffect & XE_MTE )
	{
		//animated texcoord ( alpha map - subtract )
   
		//check data
		if( !m_pMteData )
			return;

		m_pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
        m_pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		
		//D3DRS_ALPHATESTENABLE꺼줘야 블렌딩 효과를 기대할수있다.
		m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	    
		m_pd3dDevice->SetTexture( 0, m_pMteData->_pTex[0] );

//		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
//		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
//		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR  );

		m_pd3dDevice->SetTexture( 1, m_pMteData->_pTex[1] );

		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_ADDSIGNED2X );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SUBTRACT );

		// 20sec : 1.0f
		// 아래는 tool에서 구현된 것과 다른데, 실제게임에서는 시간으로 그 수치를 정하고 있기 때문이다. 
		// tool에서는 여러 개체수를 띄울필요가 없기에 시간대신 상수를 사용하였다.
		
		extern DWORD g_timeMTE;	// 20초 갱신용 타이머
		float fUV = (float)(g_timeMTE * 0.00005f);
		m_pMteData->_kUV[0].x = fUV;
		m_pMteData->_kUV[0].y = fUV;

		if( m_pMteData->_kUV[0].x > 1.0f )
			m_pMteData->_kUV[0] = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		m_pd3dDevice->SetVertexShaderConstantF( 91, (float*)&m_pMteData->_kUV[0], 1 );		//texture uv ( skinning2.vsh )


		if( g_timeMTE < 11000 ) // 서서히 밝아지게 하자
		{
			float v = (float)(g_timeMTE * 0.0001f);
			m_pMteData->_kUV[1] = D3DXVECTOR3( v, v, v );
		}
		else 
		{
			float v = (float)( ( 22000 - g_timeMTE ) * 0.0001f );
			m_pMteData->_kUV[1] = D3DXVECTOR3( v, v, v );
		}
	
		////light
		m_pd3dDevice->SetVertexShaderConstantF( 93, (float*)&m_pMteData->_kUV[1], 1 );
		m_pd3dDevice->SetVertexShaderConstantF( 94, (float*)&m_pMteData->_kUV[1], 1 );
	}
	else 
	
#endif //__BS_EFFECT_LUA

	// Set Reflection
	if( (pBlock->m_dwEffect & XE_REFLECT) || (nEffect & XE_REFLECT) )
	{
		if( m_nNoEffect )	return;
		m_pd3dDevice->SetTransform( D3DTS_TEXTURE1, &g_mReflect );
		extern LPDIRECT3DTEXTURE9 g_pReflectMap;
		m_pd3dDevice->SetTexture( 1, g_pReflectMap );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_ADDSIGNED2X );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	} else
	if( pBlock->m_dwEffect & XE_SELF_ILLUMINATE )
	{
		m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );	//
	} else
	// Set Specular
	if( (pBlock->m_dwEffect & XE_SPECULAR) || (nEffect & XE_SPECULAR) )
	{
		if( m_nNoEffect )	return;
		int a = 0;
	} else
	if( (pBlock->m_dwEffect & XE_BUMP) || (nEffect & XE_BUMP) )
	{
		if( m_nNoEffect )	return;
		int a = 0;
	}
	else
	if( (pBlock->m_dwEffect & XE_HIGHLIGHT_OBJ ) || (nEffect & XE_HIGHLIGHT_OBJ) ) 
	{
		if( m_nNoEffect )	return;
//		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ); 
		m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR ); 
		m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO ); 
		m_pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD); 
	}
	
//	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);
//	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

}

//
//
//
void CObject3D::ResetState( MATERIAL_BLOCK* pBlock,  int nEffect, DWORD dwBlendFactor )
{
	// reset
	if( pBlock->m_dwEffect & XE_2SIDE )
		m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	if( m_nNoEffect == 1 )	return;
	
	//if( nBlendFactor == 255 )
	if( dwBlendFactor == 0xff000000 )
	{
		m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
		if( m_nNoEffect )	return;
	}
	else
	{
		m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		if( m_nNoEffect )	return;
		m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0xb0 );
		m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( 255, 0, 0, 0) );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
		
		//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
		//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	}
	
	if( 0 ) //pBlock->m_nOpacity )
	{
		if( m_nNoEffect )	return;
		if( dwBlendFactor == 255 )
		{
			m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
			m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
		}
		else
		{
			m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
			m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0xb0 );
			m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( 255, 0, 0, 0) );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
			
			//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
			//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		}
	} else
	if( (pBlock->m_dwEffect & XE_REFLECT) || (nEffect & XE_REFLECT) )
	{
		if( m_nNoEffect )	return;
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );	
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	} else
	if( pBlock->m_dwEffect & XE_SELF_ILLUMINATE )
	{
		m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, s_bLight );	//
	} else
	// reset Specular
	if( (pBlock->m_dwEffect & XE_SPECULAR) || (nEffect & XE_SPECULAR) )
	{
		if( m_nNoEffect )	return;
	} else
	if( (pBlock->m_dwEffect & XE_BUMP) || (nEffect & XE_BUMP) )
	{
		if( m_nNoEffect )	return;
		int a = 0;
	}
	
#ifdef __BS_EFFECT_LUA
	if( nEffect & XE_MTE )
	{
		if( m_nNoEffect ) 
			return;

		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );	
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	}
#endif //__BS_EFFECT_LUA

	m_pd3dDevice->SetTexture( 0, NULL );
}
//
//	스킨형태의 오브젝트 렌더러
//  단독으로는 동작하지 못한다.
//
void	CObject3D::RenderSkin( LPDIRECT3DDEVICE9 pd3dDevice, LPDIRECT3DVERTEXBUFFER9 pd3d_VB, GMOBJECT *pObj, const D3DXMATRIX *mWorld, int nEffect, DWORD dwBlendFactor )
{
	//---- 그대! 스킨오브젝트만 화면에 안나오는가! 그러면 CModelObject::Render()의 설명을 읽어보고 확인해보아라!
	if( g_bUsableVS == FALSE )		// 버텍스쉐이더 1.1을 지원못하면
		pd3dDevice->SetSoftwareVertexProcessing(TRUE);		// 소프트웨어 모드로 바꿈
	
	MATERIAL_BLOCK	*pBlock;
	int		nMaxMtrl, nMaxVB, nMaxBone;
	D3DXMATRIX *pBone;
	D3DXMATRIX *pmBonesInv;
	int		i, idx;

	if( m_nNoEffect == 0 )
	{
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );
	}
	
	D3DXMATRIX	m1;

	if( m_pmExternBone )	
	{
		pBone = m_pmExternBone;		// 외부지정 뼈대가 있다면 그걸 쓰고
		pmBonesInv = m_pmExternBoneInv;
	}
	else					
	{
		pBone = m_pBaseBone;		// 없다면 디폴트를 쓴다.
		pmBonesInv = m_pBaseBoneInv;
	}

	if( pObj->m_nMaxUseBone )		// 뼈대개수가 많아서 따로 전송해야함.
	{
		nMaxBone = pObj->m_nMaxUseBone;
		for( i = 0; i < nMaxBone; i ++ )	// 
		{
			idx = pObj->m_UseBone[i];
			m1 = pmBonesInv[idx] * pBone[idx];

#ifdef	__YENV		
			CString str;
			str.Format( "mBoneMatrix[%d]", i );
			HRESULT hr = g_Neuz.m_pEffect->SetMatrix( str, &m1 );
#else //__YENV
			D3DXMatrixTranspose( &m1, &m1 );		// 매트릭스를 돌린다음.
			pd3dDevice->SetVertexShaderConstantF( i * 3, (float*)&m1, 3 );		// 상수레지스터에 집어넣음.
#endif //__YENV

		}
	}
//	pd3dDevice->SetVertexShader( D3DFVF_SKINVERTEX );
	pd3dDevice->SetVertexDeclaration( g_pSkinVertexDeclaration );
//	return;
#ifndef __YENV	
	HRESULT hr = pd3dDevice->SetVertexShader( g_pSkiningVS );
#endif //__YENV

#ifdef __YENV
	if( pObj->m_bBump && g_Option.m_bSpecBump )
	{
	#ifdef __YENV_WITHOUT_BUMP
		pd3dDevice->SetFVF( D3DFVF_SKINVERTEX );
		pd3dDevice->SetStreamSource( 0, pd3d_VB, 0,sizeof(SKINVERTEX) );
	#else 
		pd3dDevice->SetFVF( D3DFVF_SKINVERTEX_BUMP );
		pd3dDevice->SetStreamSource( 0, pd3d_VB, 0, sizeof(SKINVERTEX_BUMP) );
	#endif 
	}
	else
#endif //__YENV
	{
		pd3dDevice->SetStreamSource( 0, pd3d_VB, 0,sizeof(SKINVERTEX) );
	}


	pd3dDevice->SetIndices( pObj->m_pd3d_IB );

	pd3dDevice->SetTransform( D3DTS_WORLD, mWorld );	

	LPDIRECT3DTEXTURE9	*pBlkTexture = pObj->m_pMtrlBlkTexture;

	if( m_nTextureEx )
		pBlkTexture += (pObj->m_nMaxMtrlBlk * m_nTextureEx);
	pBlock   = pObj->m_pMtrlBlk;
	nMaxMtrl = pObj->m_nMaxMtrlBlk;
	nMaxVB	 = pObj->m_nMaxVB;
	while( nMaxMtrl-- )
	{
		if( pBlock->m_nMaxUseBone )		// 뼈대개수가 많아서 따로 전송해야함.
		{
			nMaxBone = pBlock->m_nMaxUseBone;
			for( i = 0; i < nMaxBone; i ++ )	// 
			{
				idx = pBlock->m_UseBone[i];
				m1 = pmBonesInv[idx] * pBone[idx];

	#ifdef	__YENV		
				CString str;
				str.Format( "mBoneMatrix[%d]", i );
				HRESULT hr = g_Neuz.m_pEffect->SetMatrix( str, &m1 );
	#else //__YENV
				D3DXMatrixTranspose( &m1, &m1 );		// 매트릭스를 돌린다음.
				pd3dDevice->SetVertexShaderConstantF( i * 3, (float*)&m1, 3 );		// 상수레지스터에 집어넣음.
	#endif //__YENV

			}
		}
		// 상태 세팅 
		SetState( pBlock, nEffect, dwBlendFactor );
#ifdef __BS_EFFECT_LUA
		if( XE_MTE != nEffect)
#endif //__BS_EFFECT_LUA
		{
			LPDIRECT3DTEXTURE9 pCloakTexture = g_ModelGlobal.GetTexture();
			if( pCloakTexture )		// 외부 지정 텍스쳐가 있을때.
				pd3dDevice->SetTexture( 0, pCloakTexture );
			else
			if( m_nNoTexture && pObj->m_bOpacity == 0 )
				pd3dDevice->SetTexture( 0, NULL );
			else
				pd3dDevice->SetTexture( 0, *pBlkTexture );
		}

#ifdef	__YENV
		LPDIRECT3DTEXTURE9	pNoSpecTexture;
		LPDIRECT3DTEXTURE9	pNormalTexture;
		if( g_Option.m_bSpecBump )
		{
			pNoSpecTexture = (pObj->m_pNoSpecTexture[0]);
			pNormalTexture = (pObj->m_pNormalTexture[0]);
		}
#endif //__YENV
		
#ifdef	__YENV		
		if( g_Option.m_bSpecBump )
		{
			//if( pNoSpecTexture )
				g_Neuz.m_pEffect->SetTexture( g_Neuz.m_hTexSpecular, pNoSpecTexture );
				g_Neuz.m_pEffect->SetTexture( "Tex_EnvironmentMap", g_Neuz.m_pEnvTex );
				
				//else
			//	g_Neuz.m_pEffect->SetTexture( g_Neuz.m_hTexSpecular, *pBlkTexture );
		}
#endif //__YENV			

#ifdef	__YENV	
		if( g_Neuz.m_pEffect != NULL )
		{
			g_Neuz.m_pEffect->SetTechnique(g_Neuz.m_hTechnique);
			g_Neuz.m_pEffect->Begin( NULL, 0 );

	#ifdef __YENV_WITHOUT_BUMP
			if( pNoSpecTexture && g_Option.m_bSpecBump )
	#else //__YENV_WITHOUT_BUMP
			if( pBlock->m_dwEffect & XE_BUMP && g_Option.m_bSpecBump )
	#endif //__YENV_WITHOUT_BUMP
			{
				if( dwBlendFactor != 0xff000000 )
					g_Neuz.m_pEffect->Pass(5);
				else
					g_Neuz.m_pEffect->Pass(4);
					
				D3DXMATRIX mWorld2 = *mWorld;
				D3DXMATRIX m;
				m = mWorld2 * s_mView * s_mProj;
				
				// 투영 설정...
				g_Neuz.m_pEffect->SetMatrix( g_Neuz.m_hmWVP, &m );
				
				if( m_nNoTexture )
				{
					g_Neuz.m_pEffect->Pass(3);
					g_Neuz.m_pEffect->SetTexture( g_Neuz.m_hTexDiffuse, NULL );
			#ifndef __YENV_WITHOUT_BUMP			
					g_Neuz.m_pEffect->SetTexture( g_Neuz.m_hTexBump, NULL );
			#endif //__YENV_WITHOUT_BUMP
				}
				else
				{
					g_Neuz.m_pEffect->SetTexture( g_Neuz.m_hTexDiffuse, *pBlkTexture );
					
					
					// 라이트 위치 설정
					D3DXVECTOR4 v;
					D3DXVECTOR4 vLight_Dir = s_vLight;//D3DXVECTOR4( 0.0f, 0.0f, -0.05f, 0 );
					D3DXMATRIX mLocal;
					D3DXMatrixInverse( &mLocal, NULL, &mWorld2 );						
					D3DXVec4Transform( &v, &vLight_Dir, &mLocal );						// 로컬좌표로 변환
					D3DXVec3Normalize( (D3DXVECTOR3*)&v, (D3DXVECTOR3*)&v );			// 정규화
					
					// 라이트 방향 설정

//					g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvLightDir, &D3DXVECTOR4( 0.0f, -1.0f, 0.0f, 1.0f) );
					g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvLightDir, &v );
					
					// 포그
					D3DXVECTOR4 vFog;
					vFog.x = s_fFogEnd/(s_fFogEnd-s_fFogStart);
					vFog.y = -1.0f/(s_fFogEnd-s_fFogStart);
					
					if( g_Neuz.m_hvFog != NULL ) 
						g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvFog, &vFog );
					
					DWORD dwColor = CWorld::GetDiffuseColor();
					pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,  dwColor );//CWorld::m_dwBgColor ) ;//CWorld::m_dwBgColor );
								
					g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvDiffuse, (D3DXVECTOR4*)&s_fDiffuse[0] );
					g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvAmbient, (D3DXVECTOR4*)&s_fAmbient[0] );
					
					// 카메라 위치
					m = mWorld2 * s_mView ;
					D3DXMatrixInverse( &m, NULL, &m );						
					v = D3DXVECTOR4( 0, 0, 0, 1 );
					D3DXVec4Transform( &v, &v, &m );
					g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvEyePos, &v );
					
					// 법선맵
			#ifndef __YENV_WITHOUT_BUMP			
					g_Neuz.m_pEffect->SetTexture( g_Neuz.m_hTexBump, pNormalTexture );
			#endif //__YENV_WITHOUT_BUMP
				}
				
				// 정점선언
				m_pd3dDevice->SetVertexDeclaration( m_pNormalDecl );
			}
			else
			{
				g_Neuz.m_pEffect->Pass(3);

				D3DXMATRIX mWorld2 = *mWorld;
				D3DXMATRIX m;
				m = mWorld2 * s_mView * s_mProj;
				
				// 투영 설정...
				g_Neuz.m_pEffect->SetMatrix( g_Neuz.m_hmWVP, &m );
				g_Neuz.m_pEffect->SetMatrix( "mView", &s_mView );
				g_Neuz.m_pEffect->SetMatrix( "mProj", &s_mProj );
				
				if( m_nNoTexture && pObj->m_bOpacity == 0 )
					g_Neuz.m_pEffect->SetTexture( g_Neuz.m_hTexDiffuse, NULL );
				else
					g_Neuz.m_pEffect->SetTexture( g_Neuz.m_hTexDiffuse, *pBlkTexture );
				
				// 라이트 위치 설정
				D3DXVECTOR4 v;
				D3DXVECTOR4 vLight_Dir = s_vLight;//D3DXVECTOR4( 0.0f, 0.0f, -0.05f, 0 );
				D3DXMATRIX mLocal;
				D3DXMatrixInverse( &mLocal, NULL, &mWorld2 );						
				D3DXVec4Transform( &v, &vLight_Dir, &mLocal );						// 로컬좌표로 변환
				D3DXVec3Normalize( (D3DXVECTOR3*)&v, (D3DXVECTOR3*)&v );			// 정규화
				
				// 라이트 방향 설정
				g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvLightDir, &v );
			
				g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvDiffuse, (D3DXVECTOR4*)&s_fDiffuse[0] );	
				g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvAmbient, (D3DXVECTOR4*)&s_fAmbient[0] );
				
				// 포그
				D3DXVECTOR4 vFog;
				vFog.x = s_fFogEnd/(s_fFogEnd-s_fFogStart);
				vFog.y = -1.0f/(s_fFogEnd-s_fFogStart);
				
				if( g_Neuz.m_hvFog != NULL ) 
					g_Neuz.m_pEffect->SetVector( "vFog", &vFog );
				
				DWORD dwColor = CWorld::GetDiffuseColor();
				pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,  dwColor );//CWorld::m_dwBgColor ) ;//CWorld::m_dwBgColor );
				
				m_pd3dDevice->SetVertexDeclaration( g_pSkinVertexDeclaration );
			}

			HRESULT hr;
			hr = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, nMaxVB, pBlock->m_nStartVertex, pBlock->m_nPrimitiveCount );

/*
	#ifdef __YENV_WITHOUT_BUMP
			g_Neuz.m_pEffect->Pass(6);		
			pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, nMaxVB, pBlock->m_nStartVertex, pBlock->m_nPrimitiveCount );
	#endif //__YENV_WITHOUT_BUMP
/**/

			g_Neuz.m_pEffect->End();
		}
#else //__YENV
		pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, nMaxVB, pBlock->m_nStartVertex, pBlock->m_nPrimitiveCount );
#endif //__YENV

		ResetState( pBlock, nEffect, dwBlendFactor );

		g_nMaxTri += pBlock->m_nPrimitiveCount;

		pBlock ++;
		pBlkTexture ++;
	}

	// 버텍스쉐이더 쓰는넘은 포그를 꺼야 제대로 나오더라.
//	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, s_bFog );
	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetVertexDeclaration( NULL );
	
#ifdef __CLIENT
	if( g_Neuz.m_d3dSettings.GetVertexProcessingType() != SOFTWARE_VP && g_bUsableVS == FALSE )		// 버텍스쉐이더 1.1을 지원못하면
		pd3dDevice->SetSoftwareVertexProcessing(FALSE);		// 다시 하드웨어처리로 돌림.
#endif	
	
}

//
// 단독으로는 동작하지 못한다. 사전에 SetRender등을 해줘야 한다.
// 일반형태의 렌더러
// mUpdate : 각 오브젝트가 Animate되고난 후, 최종 매트릭스의 리스트.
// mWorld : 기준 매트릭스.
//
void	CObject3D::RenderNormal( LPDIRECT3DDEVICE9 pd3dDevice, GMOBJECT *pObj, const D3DXMATRIX *mWorld, int nEffect, int nBlendFactor )
{
	MATERIAL_BLOCK	*pBlock;
	int		nMaxMtrl, nMaxVB;
//	int		i;
//	D3DXMATRIX	m1;
//	D3DXMATRIX *mUpdate = _mUpdate;
	pd3dDevice->SetVertexShader( NULL );

	pd3dDevice->SetVertexDeclaration( NULL );
	
	if( m_nNoEffect == 0 )
	{
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );

#ifdef __ENVIRONMENT_EFFECT

		if( CEnvironment::GetInstance()->GetSeason() == SEASON_SPRING )
		{
			if( stricmp( m_szFileName, "obj_macoprtr16.o3d" ) == 0 || stricmp( m_szFileName, "obj_macoprtr17.o3d" ) == 0 )
			{
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );
			}
			else
			{
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			}
		}

#else // __ENVIRONMENT_EFFECT

#ifdef __JAPAN_SAKURA
		if( stricmp( m_szFileName, "obj_macoprtr16.o3d" ) == 0 || stricmp( m_szFileName, "obj_macoprtr17.o3d" ) == 0 )
		{
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );
		}
		else
		{
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		}
#else
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
#endif //__JAPAN_SAKURA

#endif // __ENVIRONMENT_EFFECT
		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	}
	
	LPDIRECT3DTEXTURE9	*pBlkTexture = pObj->m_pMtrlBlkTexture;
	if( m_nTextureEx )
		pBlkTexture += (pObj->m_nMaxMtrlBlk * m_nTextureEx);
	
#ifdef	__YENV
	if( pObj->m_bBump && g_Option.m_bSpecBump )
	{
	#ifdef __YENV_WITHOUT_BUMP
		pd3dDevice->SetFVF( D3DFVF_NORMALVERTEX );
		pd3dDevice->SetStreamSource( 0, pObj->m_pd3d_VB, 0, sizeof(NORMALVERTEX) );
	#else //__YENV_WITHOUT_BUMP
		pd3dDevice->SetFVF( D3DFVF_NORMALVERTEX_BUMP );
		pd3dDevice->SetStreamSource( 0, pObj->m_pd3d_VB, 0, sizeof(NORMALVERTEX_BUMP) );
	#endif //__YENV_WITHOUT_BUMP
	}
	else
#endif //__YENV
	{
		pd3dDevice->SetFVF( D3DFVF_NORMALVERTEX );
		pd3dDevice->SetStreamSource( 0, pObj->m_pd3d_VB, 0, sizeof(NORMALVERTEX) );
	}
	
	pd3dDevice->SetIndices( pObj->m_pd3d_IB );
	
//	m1 = mUpdate[i] * mWorld;		// mUpdate에 미리 mWorld를 곱한상태로 와도 될것 같다.
	pd3dDevice->SetTransform( D3DTS_WORLD, mWorld );

	pBlock   = pObj->m_pMtrlBlk;
	nMaxMtrl = pObj->m_nMaxMtrlBlk;
	nMaxVB	 = pObj->m_nMaxVB;
	
	while( nMaxMtrl-- )
	{
		// 상태 세팅 
		if( pObj->m_bLight )
		{
			if( m_nNoEffect == 0 )
			{
				pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
				pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );	//
				pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
				pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE  );
				pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			}
		} else
			SetState( pBlock, nEffect, nBlendFactor );

		{
			if( m_nNoTexture && pObj->m_bOpacity == 0 )
				pd3dDevice->SetTexture( 0, NULL );
			else
				pd3dDevice->SetTexture( 0, *pBlkTexture );
		}

#ifdef	__YENV
		
		LPDIRECT3DTEXTURE9	pNoSpecTexture;
		LPDIRECT3DTEXTURE9	pNormalTexture;

		if( g_Option.m_bSpecBump )
		{
			pNoSpecTexture = pObj->m_pNoSpecTexture[0];
			pNormalTexture = pObj->m_pNormalTexture[0];
			
			//if( pNoSpecTexture )
			g_Neuz.m_pEffect->SetTexture( g_Neuz.m_hTexSpecular, pNoSpecTexture );
#ifdef __YENV
			g_Neuz.m_pEffect->SetTexture( "Tex_EnvironmentMap", g_Neuz.m_pEnvTex );
#endif //__YENV				
		}			

		if( g_dwUsablePS_Level == 2 && !(nEffect & XE_REFLECT) )
		{
			if( g_Neuz.m_pEffect != NULL )
			{
				if( !m_nNoTexture )
				{
					g_Neuz.m_pEffect->SetTechnique(g_Neuz.m_hTechnique);
					g_Neuz.m_pEffect->Begin( NULL, 0 );

					D3DXMATRIX mWorld2 = *mWorld;
					D3DXMATRIX m;
					m = mWorld2 * s_mView * s_mProj;
					
					// 투영 설정...
					g_Neuz.m_pEffect->SetMatrix( g_Neuz.m_hmWVP, &m );
					g_Neuz.m_pEffect->SetTexture( g_Neuz.m_hTexDiffuse, *pBlkTexture );
					
					// 라이트 위치 설정
					D3DXVECTOR4 v;
					D3DXVECTOR4 vLight_Dir = s_vLight;//D3DXVECTOR4( 0.0f, 0.0f, -0.05f, 0 );
					D3DXMATRIX mLocal;
					D3DXMatrixInverse( &mLocal, NULL, &mWorld2 );						
					D3DXVec4Transform( &v, &vLight_Dir, &mLocal );						// 로컬좌표로 변환
					D3DXVec3Normalize( (D3DXVECTOR3*)&v, (D3DXVECTOR3*)&v );			// 정규화
					v.w = -0.6f;														// 환경광의 밝기(Ambint) Def : -0.3f
					
					// 라이트 방향 설정
					g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvLightDir, &v );
					
					// 포그
					D3DXVECTOR4 vFog;
					vFog.x = s_fFogEnd/(s_fFogEnd-s_fFogStart);
					vFog.y = -1.0f/(s_fFogEnd-s_fFogStart);
					
					if( g_Neuz.m_hvFog != NULL ) 
						g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvFog, &vFog );
					
					DWORD dwColor = CWorld::GetDiffuseColor();
					pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,  dwColor );//CWorld::m_dwBgColor ) ;//CWorld::m_dwBgColor );
					
					//g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvDiffuse, &D3DXVECTOR4( 1.0f, 0.0f, 0.0f, 0.0f) );	
					g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvDiffuse, (D3DXVECTOR4*)&s_fDiffuse[0] );	
					g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvAmbient, (D3DXVECTOR4*)&s_fAmbient[0] );
					
					// 카메라 위치
					m = mWorld2 * s_mView ;
					D3DXMatrixInverse( &m, NULL, &m );						
					v = D3DXVECTOR4( 0, 0, 0, 1 );
					D3DXVec4Transform( &v, &v, &m );
					g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvEyePos, &v );
					
					// 버택스쉐이더가 1.1 지원하고 픽셀쉐이더가 2.0이상이어야 한다...
					if( nEffect & XE_NOBUMP )
						g_Neuz.m_pEffect->Pass(0);		
					else				
		#ifdef __YENV_WITHOUT_BUMP
					if( pNoSpecTexture && g_Option.m_bSpecBump  )
					{
						if( nBlendFactor != 0xff000000 )
							g_Neuz.m_pEffect->Pass(2);
						else
							g_Neuz.m_pEffect->Pass(1);					

						m_pd3dDevice->SetVertexDeclaration( m_pNormalDecl );
					}
					else
						g_Neuz.m_pEffect->Pass(0);		
		#else //__YENV_WITHOUT_BUMP
					if( pBlock->m_dwEffect & XE_BUMP && g_Option.m_bSpecBump  )
					{
						if( nBlendFactor != 0xff000000 )
							g_Neuz.m_pEffect->Pass(2);
						else
							g_Neuz.m_pEffect->Pass(1);
						
						// 법선맵
						g_Neuz.m_pEffect->SetTexture( g_Neuz.m_hTexBump, pNormalTexture );				
						
						// 정점선언
						m_pd3dDevice->SetVertexDeclaration( m_pNormalDecl );
					}
					else
						g_Neuz.m_pEffect->Pass(0);		
		#endif //__YENV_WITHOUT_BUMP
				}

				pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, nMaxVB, pBlock->m_nStartVertex, pBlock->m_nPrimitiveCount );			
	/*
		#ifdef __YENV_WITHOUT_BUMP
				g_Neuz.m_pEffect->Pass(6);		
				pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, nMaxVB, pBlock->m_nStartVertex, pBlock->m_nPrimitiveCount );
		#endif //__YENV_WITHOUT_BUMP
	/**/
				if( !m_nNoTexture )
					g_Neuz.m_pEffect->End();
			}
		}
		else
			pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, nMaxVB, pBlock->m_nStartVertex, pBlock->m_nPrimitiveCount );

#else //__YENV
		pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, nMaxVB, pBlock->m_nStartVertex, pBlock->m_nPrimitiveCount );
#endif //__YENV

		// 상태 복원 
		if( pObj->m_bLight )
		{
			if( m_nNoEffect == 0 )
			{
				pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
				pd3dDevice->SetRenderState( D3DRS_LIGHTING, s_bLight );	//
				pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
			}
		} else
			ResetState( pBlock, nEffect, nBlendFactor );

		g_nMaxTri += pBlock->m_nPrimitiveCount;

		pBlock ++;
		pBlkTexture ++;
	}
}

#if 0
// 충돌메시의 렌더.
void	CObject3D::RenderCollObject( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX *mWorld )
{
	GMOBJECT *pObj = &m_CollObject;
	int nEffect = 0;
	int nBlendFactor = 255;
	MATERIAL_BLOCK	*pBlock;
	int		nMaxMtrl, nMaxVB;

	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetVertexDeclaration( NULL );
	pd3dDevice->SetFVF( D3DFVF_NORMALVERTEX );

	if( m_nNoEffect == 0 )
	{
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	}
	
	LPDIRECT3DTEXTURE9	*pBlkTexture = pObj->m_pMtrlBlkTexture;

#ifdef __YENV
	if( pObj->m_bBump && g_Option.m_bSpecBump )
		pd3dDevice->SetStreamSource( 0, pObj->m_pd3d_VB, 0, sizeof(NORMALVERTEX_BUMP) );
	else
#endif //__YENV
		pd3dDevice->SetStreamSource( 0, pObj->m_pd3d_VB, 0, sizeof(NORMALVERTEX) );
	
	
	pd3dDevice->SetIndices( pObj->m_pd3d_IB );
	
	//	m1 = mUpdate[i] * mWorld;		// mUpdate에 미리 mWorld를 곱한상태로 와도 될것 같다.
	pd3dDevice->SetTransform( D3DTS_WORLD, mWorld );
	
	pBlock   = pObj->m_pMtrlBlk;
	nMaxMtrl = pObj->m_nMaxMtrlBlk;
	nMaxVB	 = pObj->m_nMaxVB;
	while( nMaxMtrl-- )
	{
		// 상태 세팅 
		if( pObj->m_bLight )
		{
			if( m_nNoEffect == 0 )
			{
				pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
				pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );	//
				pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
				pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE  );
				pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			}
		} else
			SetState( pBlock, nEffect, nBlendFactor );
		
		{
			if( m_nNoTexture && pObj->m_bOpacity == 0 )
				pd3dDevice->SetTexture( 0, NULL );
			else
				pd3dDevice->SetTexture( 0, *pBlkTexture );
		}
		
		pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, nMaxVB, pBlock->m_nStartVertex, pBlock->m_nPrimitiveCount );

		// 상태 복원 
		if( pObj->m_bLight )
		{
			if( m_nNoEffect == 0 )
			{
				pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
				pd3dDevice->SetRenderState( D3DRS_LIGHTING, s_bLight );	//
				pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
			}
		} else
			ResetState( pBlock, nEffect, nBlendFactor );

		g_nMaxTri += pBlock->m_nPrimitiveCount;

		pBlock ++;
		pBlkTexture ++;
	}
}
#endif // 0

//
// 자체 본애니메이션을 쓰는 배경오브젝트들은 이 함수로 본 매트릭스를 쉐이더에 보낸다.
//
void	CObject3D::SetShader( const D3DXMATRIX *mWorld )
{
	int		i;
	if( m_pBaseBone == NULL )	return;

	D3DXMATRIX *pmBones = m_pBaseBone;		// 뼈대모션이 로딩된게 없으면 디폴트를 쓴다.
	D3DXMATRIX *pmBonesInv = m_pBaseBoneInv ;
	D3DXMATRIX mWorldTranspose;
	
	if( m_bSendVS )	// 뼈대개수가 MAX_VS_BONE이하라서 한번에 다 전송한다.
	{
		int		nMaxBone = m_nMaxBone;
		if( nMaxBone > MAX_VS_BONE )	
			Error( "CModelObject::Render : 뼈대개수가 최대치를 초과했다. %d", nMaxBone );

		for( i = 0; i < nMaxBone; i ++ )	// MAX_VS_BONE개 이하	
		{
			mWorldTranspose = pmBonesInv[i] * pmBones[i];

#ifdef	__YENV		
			CString str;
			str.Format( "mBoneMatrix[%d]", i );
			HRESULT hr = g_Neuz.m_pEffect->SetMatrix( str, &mWorldTranspose );
#else //__YENV
			D3DXMatrixTranspose( &mWorldTranspose, &mWorldTranspose );		// 매트릭스를 돌린다음.
			m_pd3dDevice->SetVertexShaderConstantF( i * 3, (float*)&mWorldTranspose, 3 );		// 상수레지스터에 집어넣음.
#endif //__YENV

		}
	}
	
	D3DXMATRIX	mView, mProj;
	D3DXMATRIX	mViewProj, mViewProjTranspose, mInvWorld;
	
	//		static D3DXVECTOR4 vConst( 1.0f, 1.0f, 1.0f, 100.0f );
	D3DXVECTOR4 vLight = s_vLight;
//	D3DXVECTOR4 vLight( 0.0f, 0.0f, 1.0f, 0.0f );
//	D3DXVECTOR4 vEyePos( 0.0f, 1.0f, -5.0f, 0.0f );
//	D3DXVec4Normalize( &vLight, &vLight );
	//	FLOAT       fDiffuse[] = { 1.00f, 1.00f, 0.00f, 0.00f };
	//	FLOAT       fAmbient[] = { 0.25f, 0.25f, 0.25f, 0.25f };
//	static FLOAT       fDiffuse[] = { 1.00f, 1.00f, 1.00f, 1.00f };
	//		static FLOAT       fDiffuse[] = { 1.00f, 1.00f, 1.00f, 1.00f };
//	static FLOAT       fAmbient[] = { 1.00f, 1.00f, 1.00f, 1.00f };
//	static FLOAT       fSpecular[] ={ 1.00f, 1.00f, 1.00f, 128.00f };
//	static FLOAT       fLightCol[] ={ 1.00f, 1.00f, 1.00f, 1.00f };
//	m_pd3dDevice->GetTransform( D3DTS_VIEW,       &mView );
//	m_pd3dDevice->GetTransform( D3DTS_PROJECTION, &mProj );
//	mViewProj = *mWorld * mView * mProj;
	mViewProj = *mWorld * s_mView * s_mProj;
	D3DXMatrixTranspose( &mViewProjTranspose, &mViewProj );
	D3DXMatrixTranspose( &mWorldTranspose, mWorld );
	
	D3DXMatrixInverse( &mInvWorld, NULL, mWorld );
	D3DXVec4Transform( &vLight, &vLight, &mInvWorld );
//	D3DXVec4Transform( &vEyePos, &vEyePos, &mInvWorld );
	D3DXVec4Normalize( &vLight, &vLight );
	
#ifdef __YENV	
	/*
	// 투영 설정...
	g_Neuz.m_pEffect->SetMatrix( g_Neuz.m_hmWVP, &mViewProjTranspose );
	
	// 라이트 위치 설정
	D3DXVECTOR4 v;
	D3DXVECTOR4 vLight_Pos = s_vLight;
	D3DXMATRIX mLocal;
	D3DXMatrixInverse( &mLocal, NULL, &mViewProjTranspose );						
	D3DXVec4Transform( &v, &vLight_Pos, &mLocal );						// 로컬좌표로 변환
	D3DXVec3Normalize( (D3DXVECTOR3*)&v, (D3DXVECTOR3*)&v );			// 정규화
	v.w = -0.6f;														// 환경광의 밝기(Ambint) Def : -0.3f
	
	// 라이트 방향 설정
	g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvLightDir, &v );
	
	g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvDiffuse, (D3DXVECTOR4*)&s_fDiffuse[0] );	
	g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvAmbient, (D3DXVECTOR4*)&s_fAmbient[0] );	
	*/
#else //__YENV
	m_pd3dDevice->SetVertexShaderConstantF( 84, (float*)&mViewProjTranspose, 4 );
	//		m_pd3dDevice->SetVertexShaderConstantF( 88, (float*)&mWorldTranspose, 4 );
//	m_pd3dDevice->SetVertexShaderConstantF( 88, (float*)&vEyePos,  1 );
//	m_pd3dDevice->SetVertexShaderConstantF( 89, (float*)&fSpecular, 1 );
//	m_pd3dDevice->SetVertexShaderConstantF( 90, (float*)&fLightCol, 1 );
	m_pd3dDevice->SetVertexShaderConstantF( 92, (float*)&vLight,   1 );
	m_pd3dDevice->SetVertexShaderConstantF( 93, (float*)&s_fDiffuse, 1 );
	m_pd3dDevice->SetVertexShaderConstantF( 94, (float*)&s_fAmbient, 1 );
	//		m_pd3dDevice->SetVertexShaderConstant( 95, &vConst, 1 );
#endif //__YENV

}

//
//
//
void	CObject3D::Render( LPDIRECT3DDEVICE9 pd3dDevice, LPDIRECT3DVERTEXBUFFER9 *ppd3d_VB, FLOAT fFrameCurrent, int nNextFrame, const D3DXMATRIX *mWorld, int nEffect, DWORD dwBlendFactor )
{
	int			i;
	int			nMax = m_pGroup->m_nMaxObject;
	GMOBJECT	*pObj;
	D3DXMATRIX	m1;
	D3DXMATRIX	*pBone = NULL;

//	m_tmCreate = g_tmCurrent;	// 한번이라도 렌더링 되고 있다면 계속 사용하고 있는것으로 봐야 한다,.

	// 애니메이션이 있거나 외부지정뼈대가 있다면 애니메이트 돌아야 한다.
	if( m_pGroup->_mUpdate )		// maxframe이 있다면 외부본이건 내장본이건 Animate는 돌아야 한다.
		Animate( fFrameCurrent, nNextFrame );	// mUpdate 애니메이션 계산후 갱신된 매트릭스
	if( m_nMaxFrame > 0 )
	{
		if( m_pmExternBone == NULL )
		{
			if( m_pMotion )	
				m_pMotion->AnimateBone( m_pBaseBone, NULL, fFrameCurrent, nNextFrame, 0, FALSE, 0.0f );
		}
	}
	if( m_pBaseBone )	// 모션은 없고 본만 있더라도 쉐이더에 전송해야함.
		SetShader( mWorld );
	
	for( i = 0; i < nMax; i ++ )
	{
		pObj = &m_pGroup->m_pObject[i];

		if( pObj->m_nVertexSize == 0 || pObj->m_dwFVF == 0 )
			Error( "%s : FVF가 기술되지 않았다. VertexSize=%d, FVF=%d", m_szFileName, pObj->m_nVertexSize, pObj->m_dwFVF );

		if( pObj->m_Type == GMT_SKIN )
		{
			if( ppd3d_VB == NULL )
				Error( "CObject3D::Render : %s 스킨오브젝트인데 ppd3d_VB가 없다.", m_szFileName );

			RenderSkin( pd3dDevice, ppd3d_VB[i], pObj, mWorld, nEffect, dwBlendFactor );

#ifdef __BS_EFFECT_LUA
			if( m_dwEffect_EX & XE_MTE )
				RenderSkin( pd3dDevice, ppd3d_VB[i], pObj, mWorld, XE_MTE, dwBlendFactor );
#endif //__BS_EFFECT_LUA

		} else
		{
			m1 = m_pGroup->_mUpdate[i] * *mWorld;
			if( pObj->m_bLight && s_bNight == FALSE )	// 라이트 오브젝트는 낮엔 렌더링 되지 않음.
				continue;
			RenderNormal( pd3dDevice, pObj, &m1, nEffect, dwBlendFactor );
		}
	}
	if( m_nNoTexture == 0 )
	{
		pd3dDevice->SetTexture( 1, NULL );
		pd3dDevice->SetTexture( 0, NULL );
	}

}

//
//
//

#ifdef __YENV
HRESULT CObject3D::ExtractBuffers( int nType, LPDIRECT3DVERTEXBUFFER9 *ppd3d_VB, GMOBJECT *pObject )
{
	#ifdef __YENV_WITHOUT_BUMP
	return S_OK;
	#endif //__YENV_WITHOUT_BUMP

	int		nMax;
	GMOBJECT* pObj = pObject;
	WORD		*pIB;
	int		 j;

	if( pObj->m_VBType == VBT_BUMP )
		return S_OK;

	if( nType == GMT_SKIN )
	{
		SKINVERTEX_BUMP*  pVB_BUMP;

		{
			SKINVERTEX *pVB;
			nMax = pObj->m_nMaxFaceList;

			pVB	= (SKINVERTEX*)(pObj->m_pVB);
			pIB	= pObj->m_pIB;

			pVB_BUMP = new SKINVERTEX_BUMP[ pObj->m_nMaxVB ];

			for( j = 0; j < nMax; j++ )
			{
				int nIndex = *pIB++;
				pVB_BUMP[nIndex].position	= pVB[ nIndex ].position;
				pVB_BUMP[nIndex].normal		= pVB[ nIndex ].normal;
				pVB_BUMP[nIndex].w1			= pVB[ nIndex ].w1;
				pVB_BUMP[nIndex].w2			= pVB[ nIndex ].w2;
				pVB_BUMP[nIndex].matIdx		= pVB[ nIndex ].matIdx;
				pVB_BUMP[nIndex].tu			= pVB[ nIndex ].tu;
				pVB_BUMP[nIndex].tv			= pVB[ nIndex ].tv;
				SKINVERTEX_BUMP *pVertex1	= &pVB_BUMP[nIndex];
				nIndex = *pIB++;
				pVB_BUMP[nIndex].position	= pVB[ nIndex ].position;
				pVB_BUMP[nIndex].normal		= pVB[ nIndex ].normal;
				pVB_BUMP[nIndex].w1			= pVB[ nIndex ].w1;
				pVB_BUMP[nIndex].w2			= pVB[ nIndex ].w2;
				pVB_BUMP[nIndex].matIdx		= pVB[ nIndex ].matIdx;
				pVB_BUMP[nIndex].tu			= pVB[ nIndex ].tu;
				pVB_BUMP[nIndex].tv			= pVB[ nIndex ].tv;
				SKINVERTEX_BUMP *pVertex2	= &pVB_BUMP[nIndex];
				nIndex = *pIB++;
				pVB_BUMP[nIndex].position	= pVB[ nIndex ].position;
				pVB_BUMP[nIndex].normal		= pVB[ nIndex ].normal;
				pVB_BUMP[nIndex].w1			= pVB[ nIndex ].w1;
				pVB_BUMP[nIndex].w2			= pVB[ nIndex ].w2;
				pVB_BUMP[nIndex].matIdx		= pVB[ nIndex ].matIdx;
				pVB_BUMP[nIndex].tu			= pVB[ nIndex ].tu;
				pVB_BUMP[nIndex].tv			= pVB[ nIndex ].tv;
				SKINVERTEX_BUMP *pVertex3	= &pVB_BUMP[nIndex];

				//Compute the U and V in terms of X gradients
				D3DXVECTOR3 Side1 = D3DXVECTOR3(pVertex2->position.x - pVertex1->position.x, 
					pVertex2->tu - pVertex1->tu, 
					pVertex2->tv - pVertex1->tv);
				
				D3DXVECTOR3 Side2 = D3DXVECTOR3(pVertex3->position.x - pVertex1->position.x, 
					pVertex3->tu - pVertex1->tu, 
					pVertex3->tv - pVertex1->tv);
				
				//Get the cross product to find the X component of the U and V vectors
				D3DXVECTOR3 CrossProduct;
				D3DXVec3Cross(&CrossProduct, &Side1, &Side2);
				
				D3DXVECTOR3 U;
				D3DXVECTOR3 V;
				
				//Set the X components
				U.x = -CrossProduct.y / CrossProduct.x;
				V.x = -CrossProduct.z / CrossProduct.x;
				
				//repeat for y
				Side1 = D3DXVECTOR3(pVertex2->position.y - pVertex1->position.y, 
					pVertex2->tu - pVertex1->tu, 
					pVertex2->tv - pVertex1->tv);
				
				Side2 = D3DXVECTOR3(pVertex3->position.y - pVertex1->position.y, 
					pVertex3->tu - pVertex1->tu, 
					pVertex3->tv - pVertex1->tv);
				
				//Get the cross product to find the Y component of the U and V vectors
				D3DXVec3Cross(&CrossProduct, &Side1, &Side2);
				
				//Set the Y components
				U.y = -CrossProduct.y / CrossProduct.x;
				V.y = -CrossProduct.z / CrossProduct.x;
				
				
				//repeat for z
				Side1 = D3DXVECTOR3(pVertex2->position.z - pVertex1->position.z, 
					pVertex2->tu - pVertex1->tu, 
					pVertex2->tv - pVertex1->tv);
				
				Side2 = D3DXVECTOR3(pVertex3->position.z - pVertex1->position.z, 
					pVertex3->tu - pVertex1->tu, 
					pVertex3->tv - pVertex1->tv);
				
				D3DXVec3Cross(&CrossProduct, &Side1, &Side2);
				
				U.z = -CrossProduct.y / CrossProduct.x;
				V.z = -CrossProduct.z / CrossProduct.x;
				
				//Normalize the U and V vectors		
				D3DXVec3Normalize(&U, &U);
				D3DXVec3Normalize(&V, &V);
				
				//Now, compute the UxV vector
				D3DXVECTOR3 UxV;
				D3DXVec3Cross(&UxV, &U, &V);
				
				//Make sure UxT is in the same direction as the normal
				if (D3DXVec3Dot(&UxV, &D3DXVECTOR3(pVertex1->normal.x, pVertex1->normal.y, pVertex1->normal.z)) < 0.0f)
					UxV = -UxV;
				
				D3DXVec3Normalize( &pVertex1->normal, &pVertex1->normal );
				D3DXVec3Normalize( &pVertex2->normal, &pVertex2->normal );
				D3DXVec3Normalize( &pVertex3->normal, &pVertex3->normal );
				
				//Now, set each vertex component to the vectors
				pVertex1->tangent.x = pVertex2->tangent.x = pVertex3->tangent.x = UxV.x;
				pVertex1->tangent.y = pVertex2->tangent.y = pVertex3->tangent.y = UxV.y;
				pVertex1->tangent.z = pVertex2->tangent.z = pVertex3->tangent.z = UxV.z;
			}
			SAFE_DELETE_ARRAY( pObj->m_pVB );

			pObj->m_pVB = new SKINVERTEX_BUMP[ pObj->m_nMaxVB ];
			memcpy( pObj->m_pVB, pVB_BUMP, sizeof(SKINVERTEX_BUMP) * pObj->m_nMaxVB );

			pObj->m_nVertexSize = sizeof(SKINVERTEX_BUMP);		
			//SendVertexBuffer( pObj, *ppd3d_VB );


			SAFE_DELETE_ARRAY( pVB_BUMP );

		}
	}
	else
	if( nType == GMT_NORMAL )
	{
		NORMALVERTEX_BUMP*  pVB_BUMP;

		{
			NORMALVERTEX *pVB;
			nMax = pObj->m_nMaxFaceList;
			pVB	= (NORMALVERTEX*)(pObj->m_pVB);
			pIB	= pObj->m_pIB;

			pVB_BUMP = new NORMALVERTEX_BUMP[ pObj->m_nMaxVB ];

			for( j = 0; j < nMax; j++ )
			{
				int nIndex = *pIB++;
				pVB_BUMP[nIndex].position = pVB[ nIndex ].position;
				pVB_BUMP[nIndex].normal = pVB[ nIndex ].normal;
				pVB_BUMP[nIndex].tu = pVB[ nIndex ].tu;
				pVB_BUMP[nIndex].tv = pVB[ nIndex ].tv;
				NORMALVERTEX_BUMP *pVertex1 = &pVB_BUMP[nIndex];
				nIndex = *pIB++;
				pVB_BUMP[nIndex].position = pVB[ nIndex ].position;
				pVB_BUMP[nIndex].normal = pVB[ nIndex ].normal;
				pVB_BUMP[nIndex].tu = pVB[ nIndex ].tu;
				pVB_BUMP[nIndex].tv = pVB[ nIndex ].tv;
				NORMALVERTEX_BUMP *pVertex2 = &pVB_BUMP[nIndex];
				nIndex = *pIB++;
				pVB_BUMP[nIndex].position = pVB[ nIndex ].position;
				pVB_BUMP[nIndex].normal = pVB[ nIndex ].normal;
				pVB_BUMP[nIndex].tu = pVB[ nIndex ].tu;
				pVB_BUMP[nIndex].tv = pVB[ nIndex ].tv;
				NORMALVERTEX_BUMP *pVertex3 = &pVB_BUMP[nIndex];

				//Compute the U and V in terms of X gradients
				D3DXVECTOR3 Side1 = D3DXVECTOR3(pVertex2->position.x - pVertex1->position.x, 
					pVertex2->tu - pVertex1->tu, 
					pVertex2->tv - pVertex1->tv);
				
				D3DXVECTOR3 Side2 = D3DXVECTOR3(pVertex3->position.x - pVertex1->position.x, 
					pVertex3->tu - pVertex1->tu, 
					pVertex3->tv - pVertex1->tv);
				
				//Get the cross product to find the X component of the U and V vectors
				D3DXVECTOR3 CrossProduct;
				D3DXVec3Cross(&CrossProduct, &Side1, &Side2);
				
				D3DXVECTOR3 U;
				D3DXVECTOR3 V;
				
				//Set the X components
				U.x = -CrossProduct.y / CrossProduct.x;
				V.x = -CrossProduct.z / CrossProduct.x;
				
				//repeat for y
				Side1 = D3DXVECTOR3(pVertex2->position.y - pVertex1->position.y, 
					pVertex2->tu - pVertex1->tu, 
					pVertex2->tv - pVertex1->tv);
				
				Side2 = D3DXVECTOR3(pVertex3->position.y - pVertex1->position.y, 
					pVertex3->tu - pVertex1->tu, 
					pVertex3->tv - pVertex1->tv);
				
				//Get the cross product to find the Y component of the U and V vectors
				D3DXVec3Cross(&CrossProduct, &Side1, &Side2);
				
				//Set the Y components
				U.y = -CrossProduct.y / CrossProduct.x;
				V.y = -CrossProduct.z / CrossProduct.x;
				
				
				//repeat for z
				Side1 = D3DXVECTOR3(pVertex2->position.z - pVertex1->position.z, 
					pVertex2->tu - pVertex1->tu, 
					pVertex2->tv - pVertex1->tv);
				
				Side2 = D3DXVECTOR3(pVertex3->position.z - pVertex1->position.z, 
					pVertex3->tu - pVertex1->tu, 
					pVertex3->tv - pVertex1->tv);
				
				D3DXVec3Cross(&CrossProduct, &Side1, &Side2);
				
				U.z = -CrossProduct.y / CrossProduct.x;
				V.z = -CrossProduct.z / CrossProduct.x;
				
				//Normalize the U and V vectors		
				D3DXVec3Normalize(&U, &U);
				D3DXVec3Normalize(&V, &V);
				
				//Now, compute the UxV vector
				D3DXVECTOR3 UxV;
				D3DXVec3Cross(&UxV, &U, &V);
				
				//Make sure UxT is in the same direction as the normal
				if (D3DXVec3Dot(&UxV, &D3DXVECTOR3(pVertex1->normal.x, pVertex1->normal.y, pVertex1->normal.z)) < 0.0f)
					UxV = -UxV;
				
				D3DXVec3Normalize( &pVertex1->normal, &pVertex1->normal );
				D3DXVec3Normalize( &pVertex2->normal, &pVertex2->normal );
				D3DXVec3Normalize( &pVertex3->normal, &pVertex3->normal );
				
				//Now, set each vertex component to the vectors
				pVertex1->tangent.x = pVertex2->tangent.x = pVertex3->tangent.x = UxV.x;
				pVertex1->tangent.y = pVertex2->tangent.y = pVertex3->tangent.y = UxV.y;
				pVertex1->tangent.z = pVertex2->tangent.z = pVertex3->tangent.z = UxV.z;
			}
			SAFE_DELETE_ARRAY( pObj->m_pVB );

			pObj->m_pVB = new NORMALVERTEX_BUMP[ pObj->m_nMaxVB ];
			memcpy( pObj->m_pVB, pVB_BUMP, sizeof(NORMALVERTEX_BUMP) * pObj->m_nMaxVB );

			pObj->m_nVertexSize = sizeof(NORMALVERTEX_BUMP);		
			//SendVertexBuffer( pObj, pObj->m_pd3d_VB );


			SAFE_DELETE_ARRAY( pVB_BUMP );
		}
	}
			
	pObj->m_VBType = VBT_BUMP;

	return S_OK;
}

LPDIRECT3DTEXTURE9 CObject3D::CreateNormalMap( int nType, LPDIRECT3DDEVICE9 m_pd3dDevice, LPDIRECT3DTEXTURE9* pTexture, LPCTSTR strFileName, LPCTSTR szPath )
{
	HRESULT hr;
	
#ifdef __YENV_WITHOUT_BUMP
	D3DVERTEXELEMENT9 decl1[] =
	{
		{0,  0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,	0},
		{0, 24, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};
	
	D3DVERTEXELEMENT9 decl2[] =
	{
		{ 0,   0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION     , 0}, 
		{ 0,  12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_BLENDWEIGHT  , 0}, 
		{ 0,  20, D3DDECLTYPE_SHORT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_BLENDINDICES , 0}, 
		{ 0,  24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_NORMAL       , 0}, 
		{ 0,  36, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD     , 0}, 
		
		D3DDECL_END()
	};
#else //__YENV_WITHOUT_BUMP
	D3DVERTEXELEMENT9 decl1[] =
	{
		{0,  0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,	0},
		{0, 24, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,	0},
		{0, 36, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};

	D3DVERTEXELEMENT9 decl2[] =
	{
		{ 0,   0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION     , 0}, 
		{ 0,  12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_BLENDWEIGHT  , 0}, 
		{ 0,  20, D3DDECLTYPE_SHORT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_BLENDINDICES , 0}, 
		{ 0,  24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_NORMAL       , 0}, 
		{ 0,  36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT		, 0},
		{ 0,  48, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD     , 0}, 
		
		D3DDECL_END()
	};
#endif //__YENV_WITHOUT_BUMP	
	
	if( !m_pNormalDecl )
	{
		if( nType == GMT_NORMAL )
		{
			if( FAILED( hr = m_pd3dDevice->CreateVertexDeclaration(
				decl1, &m_pNormalDecl )))
				return NULL;
		}
		else
		if( nType == GMT_SKIN )
		{
			if( FAILED( hr = m_pd3dDevice->CreateVertexDeclaration(
				decl2, &m_pNormalDecl )))
				return NULL;
		}
	}

#ifndef __YENV_WITHOUT_BUMP
	{
		LoadTextureFromRes( m_pd3dDevice, MakePath( DIR_MODELTEX, strFileName ), pTexture );
	}
#endif //__YENV_WITHOUT_BUMP	

#ifdef __YENV_WITHOUT_BUMP
	if( pTexture == NULL )
		return NULL;
#endif //__YENV_WITHOUT_BUMP

	return *pTexture;
}

#endif //__YENV

#endif // __WORLDSERVER


