#include "stdafx.h"
#include <dxerr9.h>
#include "defineObj.h"
#include "path.h"
#include "ModelGlobal.h"
#ifdef __CLIENT
#include "Shader.h"
#endif
#include "..\_UnhandledException\ExceptionHandler.h"
#include "Project.h"



LARGE_INTEGER		g_lnFreq;
LARGE_INTEGER		g_lnPrev, g_lnCurr;

double		g_dUps;		// update per sec - 화면 갱신 시간
//double		g_dKps;		// key per sec - 한 애니메이션 키가 넘어가는데 기다리는 시간
//DWORD	g_tmCurrent = 0;		// 전역으로 사용할 현재 시간.

//int		g_nRenderCnt = 0;	// 렌더화면때마다 하나씩 카운트 되는 변수
//int		g_nProcessCnt = 0;	// 프로세스마다 하나씩 카운트 되는 변수.

void	InitUPS( void )
{
//	g_dKps = (double)1 / 60;	 								// 한개의 애니메이션 키가 넘어가는데 걸리는 시간.
	QueryPerformanceFrequency( &g_lnFreq );			// 초당 주파수 계산

	QueryPerformanceCounter( &g_lnCurr );				// 카운터를 똑같이 맞춘다.
	g_lnPrev = g_lnCurr;
}
// 한 화면 갱신에 걸리는 시간 계산
void	CalcUPS( void )
{
	g_lnPrev = g_lnCurr;
	QueryPerformanceCounter( &g_lnCurr );
	// 이전 화면에서 지금 화면까지 걸린 시간
	g_dUps = (double)(g_lnCurr.QuadPart - g_lnPrev.QuadPart) / g_lnFreq.QuadPart;		
}

float g_fHairLight  = 1.5f;
float s_fDiffuse[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
float s_fAmbient[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
D3DXVECTOR4 s_vLight( 0.0f, 0.0f, 1.0f, 0.0f );		// 라이트 벡터
D3DXVECTOR4 s_vLightPos( 0.0f, 5000.0f, 0.0f, 0.0f );		// 라이트 포지션
D3DXMATRIX s_mViewProj;		// 
D3DXMATRIX s_mView;		// 
D3DXMATRIX s_mProj;		// 
BOOL	s_bLight;
BOOL	s_bNight;
BOOL	s_bFog;

float s_fFogStart = 0;
float s_fFogEnd   = 100;


void	SetDiffuse( float r, float g, float b )
{
	s_fDiffuse[0] = r;	s_fDiffuse[1] = g;	s_fDiffuse[2] = b;
}

void	SetAmbient( float r, float g, float b )
{
	s_fAmbient[0] = r;	s_fAmbient[1] = g;	s_fAmbient[2] = b;
}

void	SetLightVec( const D3DVECTOR &vLight )
{
	s_vLight.x = vLight.x;
	s_vLight.y = vLight.y;
	s_vLight.z = vLight.z;
}

void	SetLightPos( const D3DVECTOR &vLight )
{
	s_vLightPos.x = vLight.x;
	s_vLightPos.y = vLight.y;
	s_vLightPos.z = vLight.z;
}

void	SetTransformViewProj( const D3DXMATRIX &mViewProj )
{
	s_mViewProj = mViewProj;
}

void	SetTransformView( const D3DXMATRIX &mView )
{
	s_mView = mView;
}

void	SetTransformProj( const D3DXMATRIX &mProj )
{
	s_mProj = mProj;
}

// 렌더스테이트 라이트가 켜져있는가 아닌가.
void	SetLight( BOOL bLight )
{
	s_bLight = bLight;
}

// 밤이면 트루.
void	SetNight( BOOL bNight )
{
	s_bNight = bNight;
}

void	SetFog( BOOL bFog )
{
	s_bFog = bFog;
}

CModelGlobal g_ModelGlobal;

void	CModelGlobal::SetCamera( const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDest )
{
	m_vCameraPos = vPos;
	m_vCameraDest = vDest;
	m_vCameraForward = vDest - vPos;
	D3DXVec3Normalize( &m_vCameraForward, &m_vCameraForward );
}

extern CPartsLaser g_Laser;
#if __VER >= 10 // __Y_DRAGON_FIRE
extern CPartsFireDragon	g_FireDragon;
#endif //__Y_DRAGON_FIRE

HRESULT CModelGlobal::InvalidateDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
#ifdef __CLIENT
	for( int i=0; i<MAX_ELETEXTURE; i++ )
	{
		SAFE_RELEASE(m_pElecTexture[i]);
	}	

	g_Laser.InvalidateDeviceObjects( pd3dDevice );
#if __VER >= 10 // __Y_DRAGON_FIRE
	g_FireDragon.InvalidateDeviceObjects( pd3dDevice );
#endif //__Y_DRAGON_FIRE
#endif //__CLIENT
	
	return S_OK;
}

#ifdef __CLIENT
CPartsLaser		g_Laser;
#if __VER >= 10 // __Y_DRAGON_FIRE
CPartsFireDragon	g_FireDragon;
#endif //__Y_DRAGON_FIRE
#endif

HRESULT CModelGlobal::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	LoadTextureFromRes( pd3dDevice, MakePath( DIR_MODELTEX, "etc_elec01.tga" ), &m_pElecTexture[0] );
	LoadTextureFromRes( pd3dDevice, MakePath( DIR_MODELTEX, "etc_elec02.tga" ), &m_pElecTexture[1] );
	LoadTextureFromRes( pd3dDevice, MakePath( DIR_MODELTEX, "etc_elec03.tga" ), &m_pElecTexture[2] );

	char buffer[64] = {0};
	for( int i=3; i< MAX_ELETEXTURE; i++ )
	{
		sprintf( buffer, "etc_elec%.2d.tga", i+1 );

		LoadTextureFromRes( pd3dDevice, MakePath( DIR_MODELTEX, buffer ), &m_pElecTexture[i] );
	}

#ifdef __CLIENT
	g_Laser.RestoreDeviceObjects( pd3dDevice );
#if __VER >= 10 // __Y_DRAGON_FIRE
	g_FireDragon.RestoreDeviceObjects( pd3dDevice );
#endif //__Y_DRAGON_FIRE
#endif // Client
	return S_OK;
}

//----------------------------------------------------------------------
//	 shadow
//----------------------------------------------------------------------
LPD3DXRENDERTOSURFACE   g_pRenderToSurface = NULL;
LPDIRECT3DSURFACE9      g_pShadowSurface   = NULL;
LPDIRECT3DTEXTURE9      g_pShadowTexture   = NULL; 

D3DXMATRIX g_mViewLight;		// 빛으로부터 플레이어쪽으로 보는 뷰 매트릭스.
D3DXMATRIX g_mShadowProj;		// 쉐도우 프로젝션
BOOL	g_bShadow = TRUE;

// 쉐도우맵 텍스쳐 생성.
BOOL CreateShadowMap( LPDIRECT3DDEVICE9 pd3dDevice, D3DFORMAT backBufferFormat )
{  
#ifndef __WORLDSERVER
	if ( g_pRenderToSurface )	return TRUE;	// 이미 있으면 생성안함.

	TRACE( "%d", (int)backBufferFormat );
	HRESULT hr;
	hr = D3DXCreateRenderToSurface( pd3dDevice, SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, backBufferFormat, FALSE, D3DFMT_UNKNOWN, &g_pRenderToSurface);
	hr = D3DXCreateTexture( pd3dDevice, SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, 1, D3DUSAGE_RENDERTARGET, backBufferFormat,  D3DPOOL_DEFAULT, &g_pShadowTexture );
	if( hr == E_OUTOFMEMORY )
	{
		Error( "쉐도우 맵 생성중 메모리 부족 %d, %d", g_Option.m_nShadow, g_Option.m_nTextureQuality );
		DeleteShadowMap( pd3dDevice );
		return FALSE;
	}
	if ( hr == D3DERR_OUTOFVIDEOMEMORY )
	{
		Error( "쉐도우 맵 생성중 비디오 메모리 부족 %d, %d", g_Option.m_nShadow, g_Option.m_nTextureQuality );
		DeleteShadowMap( pd3dDevice );
		return FALSE;
	}
	if( FAILED( hr ) )
	{
		LPCTSTR szErr = Error( "쉐도우 맵 생성 에러 %s, %d, %d", DXGetErrorString9(hr), g_Option.m_nShadow, g_Option.m_nTextureQuality );
		ADDERRORMSG( szErr );
		DeleteShadowMap( pd3dDevice );
		return FALSE;
	}
	if( g_pShadowTexture )
	{
		g_pShadowTexture->GetSurfaceLevel( 0, &g_pShadowSurface );
	}
	else
	{
		DeleteShadowMap( pd3dDevice );
		return FALSE;
	}

	g_bShadow = TRUE;
#endif // not WorldServer
	return TRUE;
}

void DeleteShadowMap( LPDIRECT3DDEVICE9 pd3dDevice )
{
	SAFE_RELEASE( g_pShadowSurface );
	SAFE_RELEASE( g_pShadowTexture );
	SAFE_RELEASE( g_pRenderToSurface );
	g_bShadow = FALSE;
}
// 쉐도우맵용 렌더타겟에 쉐도우맵 렌더링
void RenderShadowMap( LPDIRECT3DDEVICE9 pd3dDevice )
{
}

// 디버깅용 쉐도우맵창을 그린다.
void RenderShadowMapInfo( LPDIRECT3DDEVICE9 pd3dDevice )
{
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	float vertex2D[4][4+2] = 
	{
		{   0,  0,0,1, 0,0 },
		{ 128,  0,0,1, 1,0 },
		{ 128,128,0,1, 1,1 },
		{   0,128,0,1, 0,1 },
	};
	pd3dDevice->SetTexture( 0, g_pShadowTexture );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	
	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetVertexDeclaration( NULL );
	pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
	pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, vertex2D, sizeof(float)*(4+2) );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, s_bLight );
}


//
// 그림자 리시버들을 렌더링 하기전에 이것을 호출한다.
//
void SetStateShadowMap( LPDIRECT3DDEVICE9 pd3dDevice, int nShadowStage, const D3DXMATRIX &mView )
{
	if( g_bShadow == FALSE )	return;
	D3DXMATRIX mShadowUV; 

	D3DXMATRIX mCameraToWorld;
	D3DXMatrixInverse( &mCameraToWorld, NULL, &mView );

	D3DXMATRIX scale,offset;
	D3DXMatrixScaling( &scale, 0.5f, -0.5f, 1.0f );
	D3DXMatrixTranslation( &offset, 0.5f, 0.5f, 0.0f );

	D3DXMatrixMultiply( &mShadowUV, &mCameraToWorld, &g_mViewLight );
	D3DXMatrixMultiply( &mShadowUV, &mShadowUV, &g_mShadowProj );
	D3DXMatrixMultiply( &mShadowUV, &mShadowUV, &scale );
	D3DXMatrixMultiply( &mShadowUV, &mShadowUV, &offset );

	pd3dDevice->SetTexture( 0, NULL );
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_CURRENT );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	pd3dDevice->SetTexture( 1, NULL );
//	pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	HRESULT hr;
	hr = pd3dDevice->SetTexture( nShadowStage, g_pShadowTexture );
	
	hr = pd3dDevice->SetTextureStageState( nShadowStage, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION );
	hr = pd3dDevice->SetTextureStageState( nShadowStage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3 | D3DTTFF_PROJECTED );
	hr = pd3dDevice->SetTransform( (D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0 + nShadowStage), &mShadowUV );
	// 여기까지 괜찮음.
	hr = pd3dDevice->SetTextureStageState( nShadowStage, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	hr = pd3dDevice->SetTextureStageState( nShadowStage, D3DTSS_COLORARG2, D3DTA_CURRENT );
	hr = pd3dDevice->SetTextureStageState( nShadowStage, D3DTSS_COLOROP, D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( nShadowStage, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
//	return;
//	hr = pd3dDevice->SetTextureStageState( nShadowStage, D3DTSS_COLORARG2, D3DTA_CURRENT );
	//	hr = pd3dDevice->SetTextureStageState( nShadowStage, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	hr = pd3dDevice->SetSamplerState( nShadowStage, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	hr = pd3dDevice->SetSamplerState( nShadowStage, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	hr = pd3dDevice->SetSamplerState( nShadowStage, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);
	pd3dDevice->SetSamplerState( nShadowStage, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );		
	pd3dDevice->SetSamplerState( nShadowStage, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );		
	pd3dDevice->SetSamplerState( nShadowStage, D3DSAMP_MIPFILTER, D3DTEXF_POINT );
}

void ResetStateShadowMap( LPDIRECT3DDEVICE9 pd3dDevice, int nShadowStage )
{
	if( g_bShadow == FALSE )	return;
//	D3DXMATRIX m;
//	D3DXMatrixIdentity( &m );
//	pd3dDevice->SetTransform( (D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0 + nShadowStage), &m );
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU );
//	pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU );
//	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
//	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( nShadowStage, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU );
	pd3dDevice->SetTextureStageState( nShadowStage, D3DTSS_COLOROP, D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( nShadowStage, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	pd3dDevice->SetTexture( nShadowStage, NULL );
	return;
}

#ifdef __CLIENT

////////////////////////////////////////////////////////////////////////////
#if __VER >= 9 // __Y_ADV_ENCHANT_EFFECT
void	CPartsFire::Create( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXVECTOR3 vPos, DWORD dwSfx, const D3DXVECTOR3 &vScale )
#else //__Y_ADV_ENCHANT_EFFECT
void	CPartsFire::Create( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXVECTOR3 &vPos, DWORD dwSfx )
#endif //__Y_ADV_ENCHANT_EFFECT
{
	int		i;
	D3DXMATRIX	m_mScale;
	
	D3DXMatrixIdentity( &m_mScale );
	
	for( i = 0; i < MAX_PARTS_FIRE; i ++ )
	{
		if( m_pList[i] )	continue;
		m_pList[i] = (CSfxModel *)prj.m_modelMng.LoadModel( pd3dDevice, OT_SFX, dwSfx );
		m_pList[i]->m_matScale = m_mScale;

#if __VER >= 9 // __Y_ADV_ENCHANT_EFFECT
		m_pList[i]->m_vScale = vScale;
#endif //__Y_ADV_ENCHANT_EFFECT

		m_vList[i] = vPos;
		break;
	}
}

CPartsFire::~CPartsFire()
{
	for( int i = 0; i < MAX_PARTS_FIRE; i ++ )
		SAFE_DELETE( m_pList[i] );
	for( i = 0; i < MAX_PARTS_FIRE_2; i ++ )
		SAFE_DELETE( m_pList_2[i] );
}

#ifdef __CSC_ENCHANT_EFFECT_2
void	CPartsFire::Create( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXVECTOR3 vPos, DWORD dwSfx )
{
	int		i;
	D3DXMATRIX	m_mScale;
	
	D3DXMatrixIdentity( &m_mScale );
	
	for( i = 0; i < MAX_PARTS_FIRE_2; i ++ )
	{
		if( m_pList_2[i] )	continue;
		m_pList_2[i] = (CSfxModel *)prj.m_modelMng.LoadModel( pd3dDevice, OT_SFX, dwSfx );
		m_pList_2[i]->m_matScale = m_mScale;
		m_vList_2[i] = vPos;
		break;
	}
}
#endif //__CSC_ENCHANT_EFFECT_2

void	CPartsFire::Render( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX *mWorld )
{
	int		i;
	CSfxModel *pSfxModel = NULL;
	int nEffCount = 0;

#ifdef __SFX_OPT
	switch(g_Option.m_nSfxLevel)
	{
		case 1:
			nEffCount = MAX_PARTS_FIRE/3;
			break;
		case 2:
			nEffCount = MAX_PARTS_FIRE/2;
			break;
		case 3:
			nEffCount = (MAX_PARTS_FIRE/3) * 2;
			break;
		case 4:
			nEffCount = (MAX_PARTS_FIRE/5) * 4;
			break;
		case 5:
			nEffCount = MAX_PARTS_FIRE;
			break;
	};
	for( i = 0; i < nEffCount; i ++ )
#else	
	for( i = 0; i < MAX_PARTS_FIRE; i ++ )
#endif
	{
		pSfxModel = m_pList[i];
		if( pSfxModel == NULL )		continue;
		
		if( pSfxModel->Process() )	// 1play 끝남.
		{
			SAFE_DELETE(m_pList[i]);
			continue;
		}
		D3DXVec3TransformCoord( &pSfxModel->m_vPos, &m_vList[i], mWorld );
		
#if __VER < 9 // __Y_ADV_ENCHANT_EFFECT
		pSfxModel->m_vScale.x = pSfxModel->m_vScale.y = pSfxModel->m_vScale.z = pSfxModel->m_pModelElem->m_fScale;
#endif //__Y_ADV_ENCHANT_EFFECT

		pSfxModel->Render( pd3dDevice, NULL );	
	}

#ifdef __CSC_ENCHANT_EFFECT_2
#ifdef __SFX_OPT
	switch(g_Option.m_nSfxLevel)
	{
		case 1:
			nEffCount = MAX_PARTS_FIRE_2/3;
			break;
		case 2:
			nEffCount = MAX_PARTS_FIRE_2/2;
			break;
		case 3:
			nEffCount = (MAX_PARTS_FIRE_2/3) * 2;
			break;
		case 4:
			nEffCount = (MAX_PARTS_FIRE_2/5) * 4;
			break;
		case 5:
			nEffCount = MAX_PARTS_FIRE_2;
			break;
		default:
			nEffCount = 0;
			break;
	};
	for( i = 0; i < nEffCount; i ++ )
#else	
	for( i = 0; i < MAX_PARTS_FIRE_2; i ++ )
#endif
	{
		pSfxModel = m_pList_2[i];
		if( pSfxModel == NULL )		continue;
		
		if( pSfxModel->Process() )	// 1play 끝남.
		{
			SAFE_DELETE(m_pList_2[i]);
			continue;
		}
		D3DXVec3TransformCoord( &pSfxModel->m_vPos, &m_vList_2[i], mWorld );
		
		if(pSfxModel->m_pModelElem)
		{
			pSfxModel->m_vScale.x = pSfxModel->m_pModelElem->m_fScale;
			pSfxModel->m_vScale.y = pSfxModel->m_pModelElem->m_fScale;
			pSfxModel->m_vScale.z = pSfxModel->m_pModelElem->m_fScale;
		}
		
		pSfxModel->Render( pd3dDevice, NULL );	
	}
#endif //__CSC_ENCHANT_EFFECT_2
}

#ifdef __NEW_WEAPON_GLOW

CPartsWeaponEffect::~CPartsWeaponEffect()
{
	SAFE_DELETE(m_pEffect);
}
CPartsWeaponEffect::CPartsWeaponEffect()
{
	m_nType = PE_FIRE;
	m_pEffect = NULL;
}
void CPartsWeaponEffect::Create( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXVECTOR3 &vPos, DWORD dwSfx )
{
	if( m_pEffect == NULL )
	{
		m_pEffect = (CSfxModel *)prj.m_modelMng.LoadModel( pd3dDevice, OT_SFX, dwSfx );
		m_vEffect = vPos;
	}
}
void CPartsWeaponEffect::Render( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX *mWorld )
{
	CSfxModel *pSfxModel = NULL;
	pSfxModel = m_pEffect;
	
	if ( pSfxModel && pSfxModel->Process() ) // loop
		pSfxModel->m_nCurFrame = 0;

	D3DXVec3TransformCoord( &pSfxModel->m_vPos, &m_vEffect, mWorld );

	D3DXQUATERNION qRot;
	D3DXVECTOR3 vRot;
	
	D3DXQuaternionRotationMatrix( &qRot, mWorld );		
	QuaternionRotationToYPW( qRot, vRot );

	pSfxModel->m_vRotate.x = D3DXToDegree(vRot.x);
	pSfxModel->m_vRotate.y = D3DXToDegree(vRot.y);
	pSfxModel->m_vRotate.z = D3DXToDegree(vRot.z);

	pSfxModel->Render2( pd3dDevice, NULL );	
}
#endif // __NEW_WEAPON_GLOW

#if __VER >= 10 // __Y_DRAGON_FIRE
////////////////////////////////////////////////////////////////////////////
void	CPartsFireDragon::Create( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXVECTOR3 &vPos, DWORD dwSfx, const D3DXVECTOR3 &vScale, const D3DXVECTOR3 &vVel  )
{
	int		i;
	D3DXMATRIX	m_mScale;
	
	D3DXMatrixIdentity( &m_mScale );
	
	for( i = 0; i < MAX_PARTS_FIRE_DRAGON; i ++ )
	{
		if( m_pList[i] )	continue;
		m_pList[i] = new PARTICLE_DRAGON;
		m_pList[i]->m_fScale = vScale.x;
		m_pList[i]->m_vVel   = vVel;
		m_pList[i]->m_vPos = vPos;
		m_pList[i]->m_fFade = 0.7f;
		break;
	}
}

CPartsFireDragon::CPartsFireDragon() 
{
	m_pTexture = NULL;
	m_pParticleVB = NULL;
	memset( m_pList, 0, sizeof(m_pList) );
	memset( m_vFan, 0, sizeof(m_vFan) );

	m_vFan[0].v = D3DXVECTOR3( -0.5f, -0.5f, -0.5f );
	m_vFan[1].v = D3DXVECTOR3( -0.5f, 0.5f, -0.5f );
	m_vFan[2].v = D3DXVECTOR3( 0.5f, 0.5f, -0.5f );
	m_vFan[3].v = D3DXVECTOR3( -0.5f, -0.5f, -0.5f );
	m_vFan[4].v = D3DXVECTOR3( 0.5f, 0.5f, -0.5f );
	m_vFan[5].v = D3DXVECTOR3( 0.5f, -0.5f, -0.5f );
}

void CPartsFireDragon::Process( LPDIRECT3DDEVICE9 pd3dDevice )
{
	int		i;
	HRESULT hr;

	PARTICLE_DRAGON *pParticle = NULL;
	
	hr = pd3dDevice->CreateVertexBuffer( MAX_PARTS_FIRE_DRAGON * 6 * sizeof(FIREVERTEX),
										D3DUSAGE_WRITEONLY, FIREVERTEX::FVF,
										 D3DPOOL_MANAGED, &m_pParticleVB, NULL );

	if( FAILED(hr) )
		Error( "CreateShadowMask : 버텍스 버퍼 생성 실패" );

	FIREVERTEX *pVertex;
	hr = m_pParticleVB->Lock(0, MAX_PARTS_FIRE_DRAGON * 6 * sizeof(FIREVERTEX), (void **)&pVertex, 0);

	if( FAILED(hr) )
		return ;

	D3DXVECTOR3 vPos;
	D3DXMATRIX mScal;
	memset( pVertex, 0x00, MAX_PARTS_FIRE_DRAGON * 6 * sizeof(FIREVERTEX) );
    D3DXMATRIX  WorldM, ScalM, AniRotM, Trans, RotM;

	for( i = 0; i < MAX_PARTS_FIRE_DRAGON; i ++ )
	{
		pParticle = m_pList[i];

		if( pParticle == NULL )		continue;
		
		pParticle->m_vPos   += pParticle->m_vVel;
		pParticle->m_fScale += 0.05f;
		pParticle->m_fFade  -= (0.004f + xRandomF( 0.010f ));

		if( pParticle->m_fFade < 0 )
			pParticle->m_fFade = 0;

		if( pParticle->m_fFade == 0 )
		{
			SAFE_DELETE(m_pList[i]);
			continue;
		}

		D3DCOLOR FadeColor = D3DCOLOR_COLORVALUE( 1.0f, 1.0f, 1.0f, pParticle->m_fFade );

		D3DXMatrixScaling( &mScal, pParticle->m_fScale, pParticle->m_fScale, pParticle->m_fScale );

		mScal = mScal*g_matInvView;

		D3DXVec3TransformCoord( &(vPos), &(m_vFan[0].v), &mScal );
		vPos += pParticle->m_vPos;
		pVertex->v = D3DXVECTOR3( vPos.x, vPos.y, vPos.z );
		pVertex->color = FadeColor;
		pVertex->m_v2Tex = D3DXVECTOR2( 0.0f, 1.0f );
		pVertex++;

		D3DXVec3TransformCoord( &(vPos), &(m_vFan[1].v), &mScal );
		vPos += pParticle->m_vPos;
		pVertex->v = D3DXVECTOR3( vPos.x, vPos.y, vPos.z );
		pVertex->color = FadeColor;
		pVertex->m_v2Tex = D3DXVECTOR2( 0.0f, 0.0f );
		pVertex++;

		D3DXVec3TransformCoord( &(vPos), &(m_vFan[2].v), &mScal );
		vPos += pParticle->m_vPos;
		pVertex->v = D3DXVECTOR3( vPos.x, vPos.y, vPos.z );
		pVertex->color = FadeColor;
		pVertex->m_v2Tex = D3DXVECTOR2( 1.0f, 0.0f );
		pVertex++;

		D3DXVec3TransformCoord( &(vPos), &(m_vFan[3].v), &mScal );
		vPos += pParticle->m_vPos;
		pVertex->v = D3DXVECTOR3( vPos.x, vPos.y, vPos.z );
		pVertex->color = FadeColor;
		pVertex->m_v2Tex = D3DXVECTOR2( 0.0f, 1.0f );
		pVertex++;

		D3DXVec3TransformCoord( &(vPos), &(m_vFan[4].v), &mScal );
		vPos += pParticle->m_vPos;
		pVertex->v = D3DXVECTOR3( vPos.x, vPos.y, vPos.z );
		pVertex->color = FadeColor;
		pVertex->m_v2Tex = D3DXVECTOR2( 1.0f, 0.0f );
		pVertex++;

		D3DXVec3TransformCoord( &(vPos), &(m_vFan[5].v), &mScal );
		vPos += pParticle->m_vPos;
		pVertex->v = D3DXVECTOR3( vPos.x, vPos.y, vPos.z );
		pVertex->color = FadeColor;
		pVertex->m_v2Tex = D3DXVECTOR2( 1.0f, 1.0f );
		pVertex++;
	}

	m_pParticleVB->Unlock();
}

void	CPartsFireDragon::Render( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX *mWorld )
{
	HRESULT hr;

	Process( pd3dDevice );

	pd3dDevice->SetTransform( D3DTS_WORLD, mWorld );		// Set World Transform 

 	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

    // Set miscellaneous render states
 	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE);
    pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );	

	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetVertexDeclaration( NULL );

    hr = pd3dDevice->SetTexture(0, m_pTexture );

    pd3dDevice->SetFVF( FIREVERTEX::FVF );
	pd3dDevice->SetStreamSource( 0, m_pParticleVB, 0, sizeof(FIREVERTEX) );

    if(FAILED(hr = pd3dDevice->DrawPrimitive( 
            D3DPT_TRIANGLELIST, 0, MAX_PARTS_FIRE_DRAGON*2 )))//(4 * MAX_PARTS_FIRE_DRAGON) / 2 ) ) )
            return;

	pd3dDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_CCW );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ); 
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	
	SAFE_RELEASE( m_pParticleVB );

}

HRESULT CPartsFireDragon::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	HRESULT hr;
	char szFileName[128] = {0};

	hr = LoadTextureFromRes( pd3dDevice, MakePath( DIR_SFXTEX, "Sfx_NpcSp1DiePartbo03.dds" ), &m_pTexture );

	if( hr == E_FAIL )
	{
		Error( "%s 읽기 실패", MakePath( DIR_SFXTEX, szFileName ) );
	}
	return S_OK;
}

HRESULT CPartsFireDragon::InvalidateDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	SAFE_RELEASE( m_pTexture );
	SAFE_RELEASE( m_pParticleVB );

	for( int i = 0; i < MAX_PARTS_FIRE_DRAGON; i ++ )
	{
		if( m_pList[i] )
		{
			SAFE_DELETE( m_pList[i] );
		}
	}

	return S_OK;
}

#endif //__Y_DRAGON_FIRE

////////////////////////////////////////////////////////////////////////////
void CPartsBeam :: Create( const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, float fSize, int nLevel )
{
	if( m_nMax >= MAX_BEAM )	m_nMax = 0;
	m_vBeam[m_nMax * 2 + 0] = v1;
	m_vBeam[m_nMax * 2 + 1] = v2;

	D3DXVECTOR3 v = v2 - v1;
	v /= 2.0f;

	m_vCenter[m_nMax] = v1 + v;
//	m_vBeam[m_nMax * 2 + 0] -= m_vCenter[m_nMax];
//	m_vBeam[m_nMax * 2 + 1] -= m_vCenter[m_nMax];
	m_fSize[ m_nMax ] = fSize;
	m_nLevel[ m_nMax ] = nLevel;
	m_vLast = v2;
	
	m_bActive = TRUE;
	m_nMax ++;
}

// 연속으로 빔생성.
// 최초 발생지점은 원점.
void CPartsBeam :: Create( const D3DXVECTOR3 &v )
{
	if( m_nMax >= MAX_BEAM )	m_nMax = 0;
	if( m_bActive == FALSE )	// 최초 시작.
	{
		m_vBeam[ 0 ] = v;	// 최초 시작위치
		m_bActive = TRUE;
	} else
	{
		m_vBeam[ m_nMax * 2 + 0 ] = m_vLast;		// 시작좌표는 바로 이전 빔의 마지막 좌표로 한다.
	}

	m_vBeam[ m_nMax * 2 + 1 ] = v;

	D3DXVECTOR3 vh = m_vBeam[ m_nMax * 2 + 1 ] - m_vBeam[ m_nMax * 2 + 0 ];
	vh /= 2.0f;

	m_vCenter[m_nMax] = m_vBeam[ m_nMax * 2 + 0 ] + vh;
	m_vBeam[ m_nMax * 2 + 0 ] -= m_vCenter[m_nMax];
	m_vBeam[ m_nMax * 2 + 1 ] -= m_vCenter[m_nMax];
//	m_fScale[ m_nMax ] = 1.0f;
	m_fSize[ m_nMax ] = 0.05f;

	m_vLast = v;
	
	m_nMax ++;
}

// 두점 사이의 임의의 점에서 빔을 생성시킴.
void	CPartsBeam :: Generate( const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, int nLevel )
{
/*
	D3DXVECTOR3 v;
	FLOAT		fSlp = xRandomF(1);
	D3DXVec3Lerp( &v, &g_vForce1, &g_vForce2, fSlp );		// 검광 사이의 임의의 점 선택
	
	v.x += ((2.0f - xRandomF(4)) / 20.0f);
	v.y += ((2.0f - xRandomF(4)) / 20.0f);
	v.z += ((2.0f - xRandomF(4)) / 20.0f);
	
	Create( v );
*/

	D3DXVECTOR3 v, vv;
	float fSize = 0.025f + (xRandomF(1) / 10.0f);

	D3DXVec3Lerp( &v, &v1, &v2, 0.25f );		// 검광 사이의 임의의 점 선택
	v.x += ((2.0f - xRandomF(4)) / 20.0f);
	v.y += ((2.0f - xRandomF(4)) / 20.0f);
	v.z += ((2.0f - xRandomF(4)) / 20.0f);
	Create( v1, v, fSize, nLevel );
	vv = v;

	D3DXVec3Lerp( &v, &v1, &v2, 0.5f );		// 검광 사이의 임의의 점 선택
	v.x += ((2.0f - xRandomF(4)) / 20.0f);
	v.y += ((2.0f - xRandomF(4)) / 20.0f);
	v.z += ((2.0f - xRandomF(4)) / 20.0f);
	Create( vv, v, fSize, nLevel );
	vv = v;
	
	D3DXVec3Lerp( &v, &v1, &v2, 0.75f );		// 검광 사이의 임의의 점 선택
	v.x += ((2.0f - xRandomF(4)) / 20.0f);
	v.y += ((2.0f - xRandomF(4)) / 20.0f);
	v.z += ((2.0f - xRandomF(4)) / 20.0f);
	Create( vv, v, fSize, nLevel );
	vv = v;

	Create( vv, v2, fSize, nLevel );
}

void CPartsBeam :: Render( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX *mWorld, const D3DXVECTOR3 &vEye, const D3DXVECTOR3 &vForward,
					  const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, int nLevel )
{
	Generate( v1, v2, nLevel );		// 번개 이펙트 생성.

	if( m_bActive == FALSE )	return;
	int		i;

	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetFVF( D3DFVF_BEAM );
	
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,   FALSE );
//	pd3dDevice->SetRenderState( D3DRS_ZENABLE,   TRUE );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,   FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,   FALSE );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );		
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );		
	
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( 128, 0, 0, 0 ) );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	//	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR  );
	//	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE   );
	//	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE   );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	
	//	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	//	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	pd3dDevice->SetTransform( D3DTS_WORLD, mWorld );		// Set World Transform 
	

	for( i = 0; i < MAX_BEAM; i ++ )
	{
		if( m_vCenter[i].x == 0 )	continue;
		D3DXVECTOR3 vB = m_vBeam[i * 2 + 0] - m_vBeam[i * 2 + 1];		// 빔의 방향벡터.
		D3DXVECTOR3 vE = vEye - m_vBeam[i * 2 + 0];	// 카메라에서 빔의 시작점을 바라보는 벡터
		D3DXVECTOR3 vP;
		D3DXVec3Cross( &vP, &vB, &vE );		// 빔방향벡터와 시선벡터의 외적.

		D3DXVECTOR3 vF = vForward;		// 앞쪽 벡터.
		D3DXVECTOR3 vFxP;
		D3DXVec3Cross( &vFxP, &vF, &vP );
		D3DXVec3Normalize( &vFxP, &vFxP );

		D3DXVECTOR3 vR;
		D3DXVec3Cross( &vR, &vF, &vFxP );		// 오른쪽 벡터

		D3DXMATRIX m1, m2;
		m1._31 = vF.x;	m1._21 = vFxP.x;	m1._11 = vR.x;		m1._41 = m_vBeam[i * 2 + 0].x;
		m1._32 = vF.y;	m1._22 = vFxP.y;	m1._12 = vR.y;		m1._42 = m_vBeam[i * 2 + 0].y;
		m1._33 = vF.z;	m1._23 = vFxP.z;	m1._13 = vR.z;		m1._43 = m_vBeam[i * 2 + 0].z;
		m1._34 = 0;		m1._24 = 0;			m1._14 = 0;			m1._44 = 1;
		m2 = m1;
		m2._41 = m_vBeam[i * 2 + 1].x;
		m2._42 = m_vBeam[i * 2 + 1].y;
		m2._43 = m_vBeam[i * 2 + 1].z;
		m2._44 = 1;

//		float fSize = 0.05f;
		float fSize = m_fSize[i];
		
	//	if( nCnt & 1 )
	//	{
	//		fSize = 0.12f;
	//	}
	//	fSize += (nCnt * 0.01f);
		
		D3DXVec3TransformCoord( &m_vList[i * 4 + 0].pos, &(D3DXVECTOR3( -fSize, 0, 0 )), &m1 );
		D3DXVec3TransformCoord( &m_vList[i * 4 + 1].pos, &(D3DXVECTOR3(  fSize, 0, 0 )), &m1 );
		D3DXVec3TransformCoord( &m_vList[i * 4 + 2].pos, &(D3DXVECTOR3( -fSize, 0, 0 )), &m2 );
		D3DXVec3TransformCoord( &m_vList[i * 4 + 3].pos, &(D3DXVECTOR3(  fSize, 0, 0 )), &m2 );

//		m_fScale[i] += 0.01f;
		float f2 = 1.0f; //m_fScale[i];
//		D3DXMATRIX /*mScale,*/ mTrans;
//		D3DXMatrixScaling( &mScale, f2, f2, f2 );
//		D3DXMatrixTranslation( &mTrans, m_vCenter[i].x, m_vCenter[i].y, m_vCenter[i].z );
//		mTrans *= *mWorld;

//		mScale *= mTrans;

//		pd3dDevice->SetTransform( D3DTS_WORLD,	  &mTrans );		// Set World Transform 
		
	//	D3DXVec3TransformCoord( &m_vList[0].pos, &m_vList[0].pos, &mScale );
	//	D3DXVec3TransformCoord( &m_vList[1].pos, &m_vList[1].pos, &mScale );
	//	D3DXVec3TransformCoord( &m_vList[2].pos, &m_vList[2].pos, &mScale );
	//	D3DXVec3TransformCoord( &m_vList[3].pos, &m_vList[3].pos, &mScale );
		
	//  m_vList[0].pos /= 100.0f;
		m_vList[i * 4 + 0].tu = 0.0f;
		m_vList[i * 4 + 0].tv = 0.0f;
		m_vList[i * 4 + 1].tu = 1.0f;
		m_vList[i * 4 + 1].tv = 0.0f;
		m_vList[i * 4 + 2].tu = 0.0f;
		m_vList[i * 4 + 2].tv = 1.0f;
		m_vList[i * 4 + 3].tu = 1.0f;
		m_vList[i * 4 + 3].tv = 1.0f;

		pd3dDevice->SetTexture( 0, g_ModelGlobal.m_pElecTexture[m_nLevel[i] * 3 + xRandom(3)] );

		pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, &m_vList[i * 4], sizeof(FVF_BEAM) );
	}
	
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,   TRUE );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,   s_bFog );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, s_bLight );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );
	
//	nCnt ++;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
HRESULT CPartsLaser::InvalidateDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	SAFE_RELEASE( m_pTexture );
	
	return S_OK;
}


HRESULT CPartsLaser::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( FAILED( LoadTextureFromRes( pd3dDevice, MakePath( DIR_MODELTEX, "etc_Laser01.tga" ), &m_pTexture ) ) )
		Error( "Read not Texture etc_Laser01.tga" );
	return S_OK;
}


void CPartsLaser::Create( const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, float fSize )
{
	m_vBeam[0] = v1;	// 레이저 시작과 끝점.
	m_vBeam[1] = v2;
	m_fSize = fSize;
	
	m_bActive = TRUE;
}

void CPartsLaser::SetPos( const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, float fSize )
{
	FLOAT fExtent = 0.875f + (sinf( m_fRadian ) / 8.0f);	// 0.875 + -0.125 ~ 0.125(0.25범위)
	fSize *= fExtent;			// 기본폭을 퍼센트로 약간씩 조절함.
	Create( v1, v2, fSize );	// 레이저 이펙트 생성.
}

//
// v1, v2 레이저 시작과 끝. 로컬좌표
// fSize : 레이저 폭, 여기에 내부적으로 커졌다 작아졌다하는 크기가 더해진다.
//
void CPartsLaser :: Render( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX *mWorld, const D3DXVECTOR3 &vEye, const D3DXVECTOR3 &vForward
					  /*const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, FLOAT fSize*/ )
{
//	Generate( v1, v2 );		// 레이저 이펙트 생성.

	if( m_bActive == FALSE )	return;
	m_fRadian += 0.2f;
	
	//	int		i;

	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetFVF( D3DFVF_BEAM );
	
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,   FALSE );
//	pd3dDevice->SetRenderState( D3DRS_ZENABLE,   TRUE );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,   FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,   FALSE );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );		
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );		
	
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( 128, 0, 0, 0 ) );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR  );
//	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE   );
//	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE   );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	pd3dDevice->SetTransform( D3DTS_WORLD, mWorld );		// Set World Transform 
	

	{
//		if( m_vCenter[i].x == 0 )	continue;
		D3DXVECTOR3 vB = m_vBeam[0] - m_vBeam[1];		// 빔의 방향벡터.
		D3DXVECTOR3 vE = vEye - m_vBeam[0];	// 카메라에서 빔의 시작점을 바라보는 벡터
		D3DXVECTOR3 vP;
		D3DXVec3Cross( &vP, &vB, &vE );		// 빔방향벡터와 시선벡터의 외적.

		D3DXVECTOR3 vF = vForward;		// 앞쪽 벡터.
		D3DXVECTOR3 vFxP;
		D3DXVec3Cross( &vFxP, &vF, &vP );
		D3DXVec3Normalize( &vFxP, &vFxP );

		D3DXVECTOR3 vR;
		D3DXVec3Cross( &vR, &vF, &vFxP );		// 오른쪽 벡터

		D3DXMATRIX m1, m2;
		m1._31 = vF.x;	m1._21 = vFxP.x;	m1._11 = vR.x;		m1._41 = m_vBeam[0].x;
		m1._32 = vF.y;	m1._22 = vFxP.y;	m1._12 = vR.y;		m1._42 = m_vBeam[0].y;
		m1._33 = vF.z;	m1._23 = vFxP.z;	m1._13 = vR.z;		m1._43 = m_vBeam[0].z;
		m1._34 = 0;		m1._24 = 0;			m1._14 = 0;			m1._44 = 1;
		m2 = m1;
		m2._41 = m_vBeam[1].x;
		m2._42 = m_vBeam[1].y;
		m2._43 = m_vBeam[1].z;
		m2._44 = 1;

		FLOAT fSize = m_fSize;
		
		D3DXVec3TransformCoord( &m_vList[0].pos, &(D3DXVECTOR3( -fSize, 0, 0 )), &m1 );
		D3DXVec3TransformCoord( &m_vList[1].pos, &(D3DXVECTOR3(  fSize, 0, 0 )), &m1 );
		D3DXVec3TransformCoord( &m_vList[2].pos, &(D3DXVECTOR3( -fSize, 0, 0 )), &m2 );
		D3DXVec3TransformCoord( &m_vList[3].pos, &(D3DXVECTOR3(  fSize, 0, 0 )), &m2 );

		float f2 = 1.0f;
		
		m_vList[0].tu = 0.0f;		m_vList[0].tv = 0.0f;
		m_vList[1].tu = 1.0f;		m_vList[1].tv = 0.0f;
		m_vList[2].tu = 0.0f;		m_vList[2].tv = 1.0f;
		m_vList[3].tu = 1.0f;		m_vList[3].tv = 1.0f;

		pd3dDevice->SetTexture( 0, m_pTexture );

		pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, m_vList, sizeof(FVF_BEAM) );
	}
	
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,   TRUE );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,   s_bFog );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, s_bLight );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );

	m_nCount ++;
} // CPartsLaser::Render


//----------------------------------------------------------------------
//	 shadow
//----------------------------------------------------------------------

class CVertexShader
{
	DWORD dwFVF;
	
public:
	LPDIRECT3DVERTEXDECLARATION9	m_pDecl;
	LPDIRECT3DVERTEXSHADER9 m_pVS;
	
	CVertexShader();
	~CVertexShader();

	HRESULT Create( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName, DWORD dwFVF );
	HRESULT DeleteDeviceObjects( void );
};

CVertexShader::CVertexShader()
{
	m_pDecl = NULL;
	dwFVF = 0;
}

CVertexShader::~CVertexShader()
{
	m_pDecl = NULL;
	dwFVF = 0;
}

HRESULT CVertexShader::Create( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName, DWORD dwFVF )
{
	HRESULT hr;
    LPD3DXBUFFER pCode;
//	D3DVERTEXELEMENT9	decl[MAX_FVF_DECL_SIZE];
	
//	hr = D3DXDeclaratorFromFVF( dwFVF, decl );

    D3DVERTEXELEMENT9 decl[] =
    {
        { 0,   0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION     , 0}, 
        { 0,  16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD     , 0}, 
        D3DDECL_END()
    };
	
	hr = pd3dDevice->CreateVertexDeclaration( decl, &m_pDecl );
#ifdef _DEBUG
    if( FAILED( hr = D3DXAssembleShaderFromFile( szFileName, NULL, NULL, D3DXSHADER_DEBUG, &pCode, NULL ) ) )
        return hr;
#else
    if( FAILED( hr = D3DXAssembleShaderFromFile( szFileName, NULL, NULL, 0, &pCode, NULL ) ) )
#endif

	if( FAILED( hr = pd3dDevice->CreateVertexShader( (DWORD*)pCode->GetBufferPointer(), &m_pVS ) ) ) 
	{
		SAFE_RELEASE( pCode );
		return hr;
	}
	SAFE_RELEASE( pCode );
	

	return S_OK;
}


HRESULT CVertexShader::DeleteDeviceObjects( void )
{
	SAFE_RELEASE( m_pDecl );
	SAFE_RELEASE( m_pVS );

	return S_OK;
}


class CPixelShader
{
public:
	LPDIRECT3DPIXELSHADER9 m_pPS;

	CPixelShader();
	~CPixelShader();
	
	HRESULT Create( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName );
	HRESULT DeleteDeviceObjects( void );
};

CPixelShader::CPixelShader()
{
	m_pPS = NULL;
}

CPixelShader::~CPixelShader()
{
	m_pPS = NULL;
}

HRESULT CPixelShader::Create( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName )
{
	LPD3DXBUFFER pCode;
	
	if( FAILED( D3DXAssembleShaderFromFile( szFileName, NULL, NULL, 0, &pCode, NULL ) ) )
		return E_FAIL;
	
	pd3dDevice->CreatePixelShader( (DWORD*)pCode->GetBufferPointer(), &m_pPS );
	
	SAFE_RELEASE( pCode );

	return S_OK;
}

HRESULT CPixelShader::DeleteDeviceObjects( void )
{
	SAFE_RELEASE( m_pPS );
	return S_OK;
}


CVertexShader	g_BlurVS;
CPixelShader	g_BlurPS;



CShader g_ShaderBlur;

#endif //__CLIENT




#define D3DFVF_SURFACEVERTEX (D3DFVF_XYZRHW | D3DFVF_TEX1)
struct SURFACE_VERTEX
{
	float x, y, z, w;
	float tu, tv;
};


CTextureSurface::CTextureSurface()
{
	Init();
}

CTextureSurface::~CTextureSurface()
{
	Destroy();
}

void CTextureSurface::Init( void )
{
	m_pRenderToSurface = NULL;
	m_pSurface = NULL;
	m_pTexture = NULL;
	m_pd3dVB = NULL;
	m_nWidth = m_nHeight = 0;
	bTest = FALSE;
}

void CTextureSurface::Destroy( void )
{
	DeleteDeviceObjects();
	Init();
}

HRESULT CTextureSurface::Create( LPDIRECT3DDEVICE9 pd3dDevice, D3DFORMAT backBufferFormat, int nWidth, int nHeight, BOOL bDepth )
{
	if ( m_pRenderToSurface )	return E_FAIL;	// 이미 있으면 생성안함.
	
	HRESULT hr;
	if( bDepth )
		hr = D3DXCreateRenderToSurface( pd3dDevice, nWidth, nHeight, backBufferFormat, TRUE, D3DFMT_D16, &m_pRenderToSurface);
	else
		hr = D3DXCreateRenderToSurface( pd3dDevice, nWidth, nHeight, backBufferFormat, FALSE, D3DFMT_UNKNOWN, &m_pRenderToSurface);

	if( FAILED(hr) )
	{
		return E_FAIL;
	}
	
	hr = D3DXCreateTexture( pd3dDevice, nWidth, nHeight, 1, D3DUSAGE_RENDERTARGET, backBufferFormat,  D3DPOOL_DEFAULT, &m_pTexture );
	if( hr == E_OUTOFMEMORY )
	{
		Error( "Out of Memory GlareMap Memory %d %d", nWidth, nHeight );
		Destroy();
		return E_FAIL;
	}
	if ( hr == D3DERR_OUTOFVIDEOMEMORY )
	{
		Error( "Out of Memory GlareMap Memory %d %d", nWidth, nHeight );
		Destroy();
		return E_FAIL;
	}
	if( FAILED( hr ) )
	{
		LPCTSTR szErr = Error( "Create TextureSurface Error %s", DXGetErrorString9(hr) );
		ADDERRORMSG( szErr );
		Destroy();
		return E_FAIL;
	}
	if( m_pTexture )
	{
		m_pTexture->GetSurfaceLevel( 0, &m_pSurface );
	}
	else
	{
		Destroy();
		return E_FAIL;
	}

#if 1
	{
		SURFACE_VERTEX	vTris[4];
		
		vTris[0].x = 0;					vTris[0].y = 0;							vTris[0].z = 0;		vTris[0].w = 1.0f;
		vTris[0].tu = 0;				vTris[0].tv = 0;		// 좌상귀
		
		vTris[1].x = (float)( nWidth );	vTris[1].y = 0;							vTris[1].z = 0;		vTris[1].w = 1.0f;		
		vTris[1].tu = 1.0f;				vTris[1].tv = 0;		// 우상귀
		
		vTris[2].x = (float)( nWidth );	vTris[2].y = (float)( nHeight );		vTris[2].z = 0;		vTris[2].w =  1.0f;
		vTris[2].tu = 1.0f;				vTris[2].tv = 1.0f;		// 우하귀
		
		vTris[3].x = 0;					vTris[3].y = (float)( nHeight );		vTris[3].z = 0;		vTris[3].w = 1.0f;
		vTris[3].tu = 0;				vTris[3].tv = 1.0f;		// 좌하귀

		
//		LPDIRECT3DVERTEXBUFFER9		m_pd3dVB;
		
		HRESULT	hr;
		
		hr = pd3dDevice->CreateVertexBuffer( 4 * sizeof(SURFACE_VERTEX),
											D3DUSAGE_WRITEONLY, D3DFVF_SURFACEVERTEX,
											D3DPOOL_MANAGED, &m_pd3dVB, NULL );
		if( FAILED(hr) )
		{
			Error( "CTextureSurface::Create : 버텍스 버퍼 생성 실패" );
			return E_FAIL;
		}
		
		SURFACE_VERTEX *pVertex;
		hr = m_pd3dVB->Lock(0, 4 * sizeof(SURFACE_VERTEX), (void **)&pVertex, 0);
		if( FAILED(hr) )
		{
			Error( "CTextureSurface::Create : 락 실패" );
			return E_FAIL;
		}
		memcpy( pVertex, vTris, sizeof(SURFACE_VERTEX) * 4 );
		
		m_pd3dVB->Unlock();
		
//		return S_OK;
	}
	
#endif

	
	m_pd3dDevice = pd3dDevice;
	m_nWidth = nWidth;
	m_nHeight = nHeight;

	return S_OK;
}

void CTextureSurface::DeleteDeviceObjects( void )
{
	SAFE_RELEASE( m_pd3dVB );
	SAFE_RELEASE( m_pSurface );
	SAFE_RELEASE( m_pTexture );
	SAFE_RELEASE( m_pRenderToSurface );
}

void CTextureSurface::BeginScene( void )
{
	HRESULT hr = m_pRenderToSurface->BeginScene( m_pSurface, NULL );
	if( FAILED(hr) )
		Error( "CTextureSurface:BeginScene : %s", DXGetErrorString9(hr) );
}

void CTextureSurface::EndScene( void )
{
	m_pRenderToSurface->EndScene( D3DX_FILTER_LINEAR );
}

void CTextureSurface::SetTexture( void )
{
	m_pd3dDevice->SetTexture( 0, m_pTexture );
}

void CTextureSurface::DownSampling( LPDIRECT3DDEVICE9 pd3dDevice, CTextureSurface *pDst, float fOffsetU, float fOffsetV )
{
	float ooWidth = 1.0f / (float)m_nWidth;
	float ooHeight = 1.0f / (float)m_nHeight;
	
	typedef struct 
	{
		float x, y, z, w;
		float tu, tv;
	} VERTEX;
	
	VERTEX	vTris[4];
	
	vTris[0].x = -0.5f;
	vTris[0].y = -0.5f;
	vTris[0].z =  0.5f;
	vTris[0].w =  1.0f;
	vTris[0].tu = fOffsetU * ooWidth;
	vTris[0].tv = fOffsetV * ooHeight;
	
	vTris[1].x = pDst->m_nWidth -0.5f;
	vTris[1].y = -0.5f;
	vTris[1].z =  0.5f;
	vTris[1].w =  1.0f;
	vTris[1].tu = 1.0f + fOffsetU * ooWidth;
	vTris[1].tv = fOffsetV * ooHeight;
	
	vTris[2].x = pDst->m_nWidth - 0.5f;
	vTris[2].y = pDst->m_nHeight - 0.5f;
	vTris[2].z =  0.5f;
	vTris[2].w =  1.0f;
	vTris[2].tu = 1.0f + fOffsetU * ooWidth;
	vTris[2].tv = 1.0f + fOffsetV * ooHeight;
	
	vTris[3].x = -0.5f;
	vTris[3].y = pDst->m_nHeight - 0.5f;
	vTris[3].z =  0.5f;
	vTris[3].w =  1.0f;
	vTris[3].tu = fOffsetU * ooWidth;
	vTris[3].tv = 1.0f + fOffsetV * ooHeight;
	
	
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	if( bTest )
	{
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ZERO );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR );
	} else
	{
		if( fOffsetU == 0 && fOffsetV == 0 )
		{
			pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
		} else
		{
			pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCCOLOR );
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
		}
	}
	
	pd3dDevice->SetTexture( 0, m_pTexture );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	
	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetVertexDeclaration( NULL );
	pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
	pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, vTris, sizeof( VERTEX ) );
	
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, s_bLight );

}

// 현재 디바이스에 m_pTexture내용을 카피하는 일반적인 렌더링.
void CTextureSurface::RenderNormal( LPDIRECT3DDEVICE9 pd3dDevice, CTextureSurface *pDst, BOOL bBlend )
{
	int nWidth = m_nWidth;
	int nHeight = m_nHeight;

	if( pDst )
	{
		nWidth = pDst->m_nWidth;
		nHeight = pDst->m_nHeight;
	}
	HRESULT	hr;
	
	if( 1 )
	{
		SURFACE_VERTEX	vTris[4];
		
		memset( vTris, 0, sizeof(vTris) );

		vTris[0].x = -0.0;					vTris[0].y = -0.0;						vTris[0].z = 0;		vTris[0].w = 1.0f;
		vTris[0].tu = 0;					vTris[0].tv = 0;		// 좌상귀
		
		vTris[1].x = (float)( nWidth-0.0 );	vTris[1].y = -0.0;						vTris[1].z = 0;		vTris[1].w = 1.0f;		
		vTris[1].tu = 1.0f;					vTris[1].tv = 0.0;		// 우상귀
		
		vTris[2].x = (float)( nWidth-0.0 );	vTris[2].y = (float)( nHeight-0.0 );	vTris[2].z = 0;		vTris[2].w =  1.0f;
		vTris[2].tu = 1.0f;					vTris[2].tv = 1.0f;		// 우하귀
		
		vTris[3].x = -0.0;					vTris[3].y = (float)( nHeight-0.0 );	vTris[3].z = 0;		vTris[3].w = 1.0f;
		vTris[3].tu = 0.0f;					vTris[3].tv = 1.0f;		// 좌하귀

		HRESULT	hr;

		// 이부분은 밖으로 빠져야 한다.
		SURFACE_VERTEX *pVertex;
		hr = m_pd3dVB->Lock(0, 4 * sizeof(SURFACE_VERTEX), (void **)&pVertex, 0);
		if( FAILED(hr) )
		{
			Error( "CTextureSurface::Create : 락 실패" );
			return;
		}
		memcpy( pVertex, vTris, sizeof(SURFACE_VERTEX) * 4 );
		
		m_pd3dVB->Unlock();
	}
	
//	pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0,0,0,0), 1.0f, 0 );
	
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, bBlend );
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	
	if( bTest )
	{
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_NONE );		
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_NONE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ZERO );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR );
	} else
	{
//		if( bBlend )
//		{
//			pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );		
//			pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );		
//			pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCCOLOR );
//			pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
//		} else
		{
			pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );		
			pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );		
			pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
		}
	}

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 2, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	
    if( FAILED(hr = pd3dDevice->SetStreamSource(0, m_pd3dVB, 0, sizeof(SURFACE_VERTEX) )) )
        return;
	
	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetVertexDeclaration( NULL );
	pd3dDevice->SetPixelShader( NULL );
	pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );

	pd3dDevice->SetTexture( 0, m_pTexture );
	pd3dDevice->SetTexture( 1, NULL );
	pd3dDevice->SetTexture( 2, NULL );
	pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );

	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
}

// this Surface내용을 pDst서피스에 렌더링 하는데...
// Blur를 적용하여 렌더링 한다.
void CTextureSurface::RenderTargetBlur( LPDIRECT3DDEVICE9 pd3dDevice, CTextureSurface *pDst)
{
	HRESULT	hr;
	
	if( 0 )
	{
		SURFACE_VERTEX	vTris[4];
		
		memset( vTris, 0, sizeof(vTris) );

		vTris[0].x = -0.0;							vTris[0].y = -0.0;								vTris[0].z = 0;		vTris[0].w = 1.0f;
		vTris[0].tu = 0;							vTris[0].tv = 0;		// 좌상귀
		
		vTris[1].x = (float)( pDst->m_nWidth-0.0 );	vTris[1].y = -0.0;								vTris[1].z = 0;		vTris[1].w = 1.0f;		
		vTris[1].tu = 1.0f;							vTris[1].tv = 0.0;		// 우상귀
		
		vTris[2].x = (float)( pDst->m_nWidth-0.0 );	vTris[2].y = (float)( pDst->m_nHeight-0.0 );	vTris[2].z = 0;		vTris[2].w =  1.0f;
		vTris[2].tu = 1.0f;							vTris[2].tv = 1.0f;		// 우하귀
		
		vTris[3].x = -0.0;							vTris[3].y = (float)( pDst->m_nHeight-0.0 );		vTris[3].z = 0;		vTris[3].w = 1.0f;
		vTris[3].tu = 0.0f;							vTris[3].tv = 1.0f;		// 좌하귀
		
		//		LPDIRECT3DVERTEXBUFFER9		m_pd3dVB;
		
		HRESULT	hr;
		
		
		SURFACE_VERTEX *pVertex;
		hr = m_pd3dVB->Lock(0, 4 * sizeof(SURFACE_VERTEX), (void **)&pVertex, 0);
		if( FAILED(hr) )
		{
			Error( "CTextureSurface::Create : 락 실패" );
			return;
		}
		memcpy( pVertex, vTris, sizeof(SURFACE_VERTEX) * 4 );
		
		m_pd3dVB->Unlock();
	}
	

//	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_NONE );		
//	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_NONE );		

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	
	pd3dDevice->SetVertexDeclaration( g_ShaderBlur.m_pDecl );
//	pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
    if( FAILED(hr = pd3dDevice->SetStreamSource(0, m_pd3dVB, 0, sizeof(SURFACE_VERTEX) )) )
        return;
	

	hr = g_ShaderBlur.m_pEffect->SetTechnique( "BlurTech" );

	UINT nMaxPass;
	g_ShaderBlur.Begin( &nMaxPass, 0 );

	int i = 0;
//	for( i = 0; i < nMaxPass; i ++ )
//	if( m_nWidth == 256 )		i = 1;
//	else if( m_nWidth == 128 )	i = 2;

//	{
//		g_ShaderBlur.SetFloat( "fPixelSizeX", 1.0f / m_nWidth );
//		g_ShaderBlur.SetFloat( "fPixelSizeY", 1.0f / m_nHeight );
		g_ShaderBlur.Pass( 0 );

		pd3dDevice->SetTexture( 0, m_pTexture );
//		pd3dDevice->SetTexture( 1, m_pTexture );
//		pd3dDevice->SetTexture( 2, m_pTexture );

//		pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );		
//		pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );		
//		pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );		
//		pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );		
//		pd3dDevice->SetSamplerState( 2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );		
//		pd3dDevice->SetSamplerState( 2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );		
		
		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );
//	}
	g_ShaderBlur.End();

	pd3dDevice->SetTexture( 0, NULL );
	pd3dDevice->SetTexture( 1, NULL );
	pd3dDevice->SetTexture( 2, NULL );
	

	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, s_bLight );
	
}

// VS,PS 사용버전.
void CTextureSurface::RenderTargetBlurH( LPDIRECT3DDEVICE9 pd3dDevice, CTextureSurface *pDst)
{
	HRESULT	hr;

	if( 1 )
	{
		SURFACE_VERTEX	vTris[4];
		
		memset( vTris, 0, sizeof(vTris) );
		int nWidth = pDst->m_nWidth;
		int nHeight = pDst->m_nHeight;
//		nWidth = 1;
//		nHeight = 1;
/*		
		vTris[0].x = -0.0;		vTris[0].y = -0.0;		vTris[0].z = 0;		vTris[0].w = 1.0f;
		vTris[0].tu = 0.0f;		vTris[0].tv = 0.0f;		// 좌상귀
		
		vTris[1].x = nWidth-0.0;	vTris[1].y = -0.0;		vTris[1].z = 0;		vTris[1].w = 1.0f;		
		vTris[1].tu = 1.0f;		vTris[1].tv = 0.0f;		// 우상귀
		
		vTris[2].x = nWidth-0.0;	vTris[2].y = nHeight-0.0;		vTris[2].z = 0;		vTris[2].w =  1.0f;
		vTris[2].tu = 1.0f;		vTris[2].tv = 1.0f;		// 우하귀
		
		vTris[3].x = -0.0;			vTris[3].y = nHeight-0.0;		vTris[3].z = 0;		vTris[3].w = 1.0f;
		vTris[3].tu = 0.0f;		vTris[3].tv = 1.0f;		// 좌하귀
*/
		vTris[0].x = -1.0f;		vTris[0].y = +1.0f;		vTris[0].z = 0.1f;		vTris[0].w = 1.0f;
		vTris[0].tu = 0.0f;		vTris[0].tv = 0.0f;		// 좌상귀
		
		vTris[1].x = +1.0f;		vTris[1].y = +1.0f;		vTris[1].z = 0.1f;		vTris[1].w = 1.0f;		
		vTris[1].tu = 1.0f;		vTris[1].tv = 0.0f;		// 우상귀
		
		vTris[2].x = +1.0f;		vTris[2].y = -1.0f;		vTris[2].z = 0.1f;		vTris[2].w =  1.0f;
		vTris[2].tu = 1.0f;		vTris[2].tv = 1.0f;		// 우하귀
		
		vTris[3].x = -1.0f;		vTris[3].y = -1.0f;		vTris[3].z = 0.1f;		vTris[3].w = 1.0f;
		vTris[3].tu = 0.0f;		vTris[3].tv = 1.0f;		// 좌하귀
		
		
		//		LPDIRECT3DVERTEXBUFFER9		m_pd3dVB;
		
		HRESULT	hr;
		
		
		SURFACE_VERTEX *pVertex;
		hr = m_pd3dVB->Lock(0, 4 * sizeof(SURFACE_VERTEX), (void **)&pVertex, 0);
		if( FAILED(hr) )
		{
			Error( "CTextureSurface::Create : 락 실패" );
			return;
		}
		memcpy( pVertex, vTris, sizeof(SURFACE_VERTEX) * 4 );
		
		m_pd3dVB->Unlock();
	}
	

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	
	pd3dDevice->SetVertexDeclaration( g_BlurVS.m_pDecl );
	pd3dDevice->SetVertexShader( g_BlurVS.m_pVS );
	pd3dDevice->SetPixelShader( g_BlurPS.m_pPS );
//	pd3dDevice->SetPixelShader( NULL );
//	pd3dDevice->SetVertexShader( NULL );
//	pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
    if( FAILED(hr = pd3dDevice->SetStreamSource(0, m_pd3dVB, 0, sizeof(SURFACE_VERTEX) )) )
        return;
	
	// 가로만
	D3DXVECTOR4 vC0 = D3DXVECTOR4( -0.0009765625f, 0, 0, 0 );
	D3DXVECTOR4 vC1 = D3DXVECTOR4(			   0, 0, 0, 0 );
	D3DXVECTOR4 vC2 = D3DXVECTOR4( +0.0009765625f, 0, 0, 0 );
	pd3dDevice->SetVertexShaderConstantF( 0, (float*)&vC0, 1 );
	pd3dDevice->SetVertexShaderConstantF( 1, (float*)&vC1, 1 );
	pd3dDevice->SetVertexShaderConstantF( 2, (float*)&vC2, 1 );

	vC0 = D3DXVECTOR4( 0.240f, 0.240f, 0.240f, 1.0f );
	vC1 = D3DXVECTOR4( 0.45f,   0.45f,  0.45f, 1.0f );
	vC2 = D3DXVECTOR4( 0.240f, 0.240f, 0.240f, 1.0f );
//	vC0 = D3DXVECTOR4( 0.25, 0.25, 0.25, 0.25 );
//	vC1 = D3DXVECTOR4( 0.50,   0.50,  0.50, 0.50 );
//	vC2 = D3DXVECTOR4( 0.25, 0.25, 0.25, 0.25 );
	pd3dDevice->SetPixelShaderConstantF( 0, (float*)&vC0, 1 );
	pd3dDevice->SetPixelShaderConstantF( 1, (float*)&vC1, 1 );
	pd3dDevice->SetPixelShaderConstantF( 2, (float*)&vC2, 1 );
	
	// 세로는 따로

	pd3dDevice->SetTexture( 0, m_pTexture );
	pd3dDevice->SetTexture( 1, m_pTexture );
	pd3dDevice->SetTexture( 2, m_pTexture );

	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );		
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );		
	pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_POINT );		
	pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_POINT );		
	pd3dDevice->SetSamplerState( 2, D3DSAMP_MINFILTER, D3DTEXF_POINT );		
	pd3dDevice->SetSamplerState( 2, D3DSAMP_MAGFILTER, D3DTEXF_POINT );		
	
	pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );

	pd3dDevice->SetTexture( 0, NULL );
	pd3dDevice->SetTexture( 1, NULL );
	pd3dDevice->SetTexture( 2, NULL );

	pd3dDevice->SetVertexDeclaration( NULL );
	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetPixelShader( NULL );
	
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, s_bLight );
	
}

// VS,PS 사용버전.
void CTextureSurface::RenderTargetBlurV( LPDIRECT3DDEVICE9 pd3dDevice, CTextureSurface *pDst)
{
	HRESULT	hr;

	if( 1 )
	{
		SURFACE_VERTEX	vTris[4];
		
		memset( vTris, 0, sizeof(vTris) );
		int nWidth = pDst->m_nWidth;
		int nHeight = pDst->m_nHeight;
//		nWidth = 1;
//		nHeight = 1;
/*		
		vTris[0].x = -0.0;		vTris[0].y = -0.0;		vTris[0].z = 0;		vTris[0].w = 1.0f;
		vTris[0].tu = 0.0f;		vTris[0].tv = 0.0f;		// 좌상귀
		
		vTris[1].x = nWidth-0.0;	vTris[1].y = -0.0;		vTris[1].z = 0;		vTris[1].w = 1.0f;		
		vTris[1].tu = 1.0f;		vTris[1].tv = 0.0f;		// 우상귀
		
		vTris[2].x = nWidth-0.0;	vTris[2].y = nHeight-0.0;		vTris[2].z = 0;		vTris[2].w =  1.0f;
		vTris[2].tu = 1.0f;		vTris[2].tv = 1.0f;		// 우하귀
		
		vTris[3].x = -0.0;			vTris[3].y = nHeight-0.0;		vTris[3].z = 0;		vTris[3].w = 1.0f;
		vTris[3].tu = 0.0f;		vTris[3].tv = 1.0f;		// 좌하귀
*/
		vTris[0].x = -1.0f;		vTris[0].y = +1.0f;		vTris[0].z = 0.1f;		vTris[0].w = 1.0f;
		vTris[0].tu = 0.0f;		vTris[0].tv = 0.0f;		// 좌상귀
		
		vTris[1].x = +1.0f;		vTris[1].y = +1.0f;		vTris[1].z = 0.1f;		vTris[1].w = 1.0f;		
		vTris[1].tu = 1.0f;		vTris[1].tv = 0.0f;		// 우상귀
		
		vTris[2].x = +1.0f;		vTris[2].y = -1.0f;		vTris[2].z = 0.1f;		vTris[2].w =  1.0f;
		vTris[2].tu = 1.0f;		vTris[2].tv = 1.0f;		// 우하귀
		
		vTris[3].x = -1.0f;		vTris[3].y = -1.0f;		vTris[3].z = 0.1f;		vTris[3].w = 1.0f;
		vTris[3].tu = 0.0f;		vTris[3].tv = 1.0f;		// 좌하귀
		
		
		//		LPDIRECT3DVERTEXBUFFER9		m_pd3dVB;
		
		HRESULT	hr;
		
		
		SURFACE_VERTEX *pVertex;
		hr = m_pd3dVB->Lock(0, 4 * sizeof(SURFACE_VERTEX), (void **)&pVertex, 0);
		if( FAILED(hr) )
		{
			Error( "CTextureSurface::Create : 락 실패" );
			return;
		}
		memcpy( pVertex, vTris, sizeof(SURFACE_VERTEX) * 4 );
		
		m_pd3dVB->Unlock();
	}
	

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	
	pd3dDevice->SetVertexDeclaration( g_BlurVS.m_pDecl );
	pd3dDevice->SetVertexShader( g_BlurVS.m_pVS );
	pd3dDevice->SetPixelShader( g_BlurPS.m_pPS );
//	pd3dDevice->SetPixelShader( NULL );
//	pd3dDevice->SetVertexShader( NULL );
//	pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
    if( FAILED(hr = pd3dDevice->SetStreamSource(0, m_pd3dVB, 0, sizeof(SURFACE_VERTEX) )) )
        return;
	
	// 가로만
	D3DXVECTOR4 vC0 = D3DXVECTOR4( 0, -0.0009765625f, 0, 0 );
	D3DXVECTOR4 vC1 = D3DXVECTOR4( 0,			  0, 0, 0 );
	D3DXVECTOR4 vC2 = D3DXVECTOR4( 0, +0.0009765625f, 0, 0 );
	pd3dDevice->SetVertexShaderConstantF( 0, (float*)&vC0, 1 );
	pd3dDevice->SetVertexShaderConstantF( 1, (float*)&vC1, 1 );
	pd3dDevice->SetVertexShaderConstantF( 2, (float*)&vC2, 1 );

	// 세로는 따로
	vC0 = D3DXVECTOR4( 0.240f, 0.240f, 0.240f, 1.0f );
	vC1 = D3DXVECTOR4( 0.45f,   0.45f,  0.45f, 1.0f );
	vC2 = D3DXVECTOR4( 0.240f, 0.240f, 0.240f, 1.0f );
	//	vC0 = D3DXVECTOR4( 0.125, 0.125, 0.125, 0.125 );
	//	vC1 = D3DXVECTOR4( 0.25,   0.25,  0.25, 0.25 );
	//	vC2 = D3DXVECTOR4( 0.125, 0.125, 0.125, 0.125 );
	pd3dDevice->SetPixelShaderConstantF( 0, (float*)&vC0, 1 );
	pd3dDevice->SetPixelShaderConstantF( 1, (float*)&vC1, 1 );
	pd3dDevice->SetPixelShaderConstantF( 2, (float*)&vC2, 1 );
	
	pd3dDevice->SetTexture( 0, m_pTexture );
	pd3dDevice->SetTexture( 1, m_pTexture );
	pd3dDevice->SetTexture( 2, m_pTexture );

	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );		
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );		
	pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_POINT );		
	pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_POINT );		
	pd3dDevice->SetSamplerState( 2, D3DSAMP_MINFILTER, D3DTEXF_POINT );		
	pd3dDevice->SetSamplerState( 2, D3DSAMP_MAGFILTER, D3DTEXF_POINT );		
	
	pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );

	pd3dDevice->SetTexture( 0, NULL );
	pd3dDevice->SetTexture( 1, NULL );
	pd3dDevice->SetTexture( 2, NULL );

	pd3dDevice->SetVertexDeclaration( NULL );
	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetPixelShader( NULL );
	
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, s_bLight );
	
}


//------------------------------------------------------------------------------------
//
//
//------------------------------------------------------------------------------------

CGlareLevel		g_Glare;

CGlareLevel::CGlareLevel()
{
	Init();
}

CGlareLevel::~CGlareLevel()
{
	Destroy();
}

void CGlareLevel::Init( void )
{
	m_bActive = FALSE;
	m_pd3dDevice = NULL;
	m_nWidth = m_nHeight = 0;

//	m_Src.Init();
//	m_Surface[0].Init();
//	m_Surface[1].Init();
//	m_Surface[2].Init();
//	m_Surface[3].Init();
//	m_Composite.Init();
}

void CGlareLevel::Destroy( void )
{
//	m_Surface[0].Destroy();
//	m_Surface[1].Destroy();
//	m_Surface[2].Destroy();
//	m_Surface[3].Destroy();

	Init();
}

void CGlareLevel::DeleteDeviceObjects( void )
{
	g_BlurVS.DeleteDeviceObjects();
	g_BlurPS.DeleteDeviceObjects();
	
	g_ShaderBlur.DeleteDeviceObjects();

	m_Src.DeleteDeviceObjects();
	m_Surface[0].DeleteDeviceObjects();
	m_Surface[1].DeleteDeviceObjects();
	m_Surface[2].DeleteDeviceObjects();
	m_Surface[3].DeleteDeviceObjects();
	m_Surface[4].DeleteDeviceObjects();
//	m_Surface[5].DeleteDeviceObjects();
	m_Black.DeleteDeviceObjects();

	m_bActive = FALSE;
}

//
// nWidth, nHeight는 원본 스크린 사이즈.
//
void CGlareLevel::Create( LPDIRECT3DDEVICE9 pd3dDevice, D3DFORMAT backBufferFormat, int nWidth, int nHeight )
{
	if( m_bActive )		return;	
	HRESULT hr;

	int noWidth = nWidth, noHeight = nHeight;	// 오리지날 크기.
#ifdef __XGLARE2
//	nWidth = 512;	nHeight = noHeight * nWidth / noWidth;
#endif
	hr = m_Src.Create( pd3dDevice, backBufferFormat, nWidth, nHeight, TRUE );		// 게임화면을 렌더링한 복사본
	if( FAILED(hr) )	return;
//	m_Black.Create( pd3dDevice, backBufferFormat, nWidth, nHeight );
		
//	nWidth = 256;	nHeight = noHeight * nWidth / noWidth;
	hr = m_Surface[0].Create( pd3dDevice, backBufferFormat, nWidth, nHeight );		// m_Src를 블러시키는 메모리. 최종결과는 [0]에 들어감.
	if( FAILED(hr) )	return;
	hr = m_Surface[1].Create( pd3dDevice, backBufferFormat, nWidth, nHeight );
	if( FAILED(hr) )	return;
#if 0
	nWidth = 256;	nHeight = noHeight * nWidth / noWidth;
	m_Surface[2].Create( pd3dDevice, backBufferFormat, nWidth, nHeight );
	
	nWidth = 128;	nHeight = noHeight * nWidth / noWidth;
	m_Surface[3].Create( pd3dDevice, backBufferFormat, nWidth, nHeight );
	m_Surface[4].Create( pd3dDevice, backBufferFormat, nWidth, nHeight );
	
	nWidth = 128;	nHeight = noHeight * nWidth / noWidth;
//	m_Surface[3].Create( pd3dDevice, backBufferFormat, nWidth, nHeight );
#endif
	
#ifdef __XGLARE2

	hr = g_BlurVS.Create( pd3dDevice, "blur.vs", D3DFVF_XYZRHW | D3DFVF_TEX1 );
	if( FAILED(hr) )
	{
		Error( "blur.vs 읽기 실패" );
	}
	hr = g_BlurPS.Create( pd3dDevice, "blur.ps" );
	if( FAILED(hr) )
	{
		Error( "blur.ps 읽기 실패" );
	}
#else
	g_ShaderBlur.LoadEffect( pd3dDevice, "blur.fx" );
	
#endif // xGlare2	

	m_bActive = TRUE;
	m_pd3dDevice = pd3dDevice;
	
}

// 풀스크린으로 덧씌울때 이걸 호출하고 바르면 된다.
void CGlareLevel::SetState( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( m_bActive == FALSE )	return;

	HRESULT hr;


	// 늘여서 스크린톤으로 발라버리면 된다.
	hr = pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	hr = pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	hr = pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
		 pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	
}

void CGlareLevel::ResetState( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( m_bActive == FALSE )	return;
}

//
// 작은 텍스쳐에 월드를 똑같이 그린다.
// 약간 어둡게해서 렌더링하자.
void CGlareLevel::RenderWorld( LPDIRECT3DDEVICE9 pd3dDevice, CObj **pList, int nMax )
{
}

void CGlareLevel::Blur( LPDIRECT3DDEVICE9 pd3dDevice )
{
	float fVal = 1.0f;

#ifdef __CLIENT
	if( m_bActive == FALSE )	return;
	if( g_Option.m_nBloom == 0 )		return;

#if 0
	// 원본(m_Src)크기의 블러[0]를 만듬.
	m_Surface[0].BeginScene();
//	pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0,0,0,0), 0, 0 );
	m_Src.RenderTargetBlur( pd3dDevice, &m_Surface[0] );		// 게임화면을 블러해서 [0]에 넣음
	m_Surface[0].EndScene();
	
	// 중간크기의 원본을 만듬.
	m_Surface[1].BeginScene();
	m_Src.RenderNormal( pd3dDevice, &m_Surface[1] );
	m_Surface[1].EndScene();
	// 중간크기의 블러 만듬.
	m_Surface[2].BeginScene();
	m_Surface[1].RenderTargetBlur( pd3dDevice, &m_Surface[2] );	
	m_Surface[2].EndScene();

	// 낮은크기의 원본을 만듬.
	m_Surface[3].BeginScene();
	m_Src.RenderNormal( pd3dDevice, &m_Surface[3] );
	m_Surface[3].EndScene();
	// 낮은크기의 블러 만듬.
	m_Surface[4].BeginScene();
	m_Surface[3].RenderTargetBlur( pd3dDevice, &m_Surface[4] );	
	m_Surface[4].EndScene();

	// 합성
	m_Surface[0].BeginScene();
	m_Surface[4].RenderNormal( pd3dDevice, &m_Surface[0], TRUE );	// 높음블러에 낮은 블러를 블렌딩 한다
	m_Surface[2].RenderNormal( pd3dDevice, &m_Surface[0], TRUE );	// 높음블러에 중간 블러를 블렌딩 한다
	m_Surface[0].EndScene();
#endif // 0

//	m_Black.BeginScene();
//	pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0,128,128,128), 0, 0 );
//	pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0,255,255,255), 0, 0 );
//	m_Black.EndScene();
	
	m_Surface[0].BeginScene();
#ifdef __XGLARE2
	pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0,0,0,0), 0, 0 );
	m_Src.RenderTargetBlurH( pd3dDevice, &m_Surface[0] );		// 게임화면을 블러해서 [0]에 넣음
#else	
	m_Src.RenderTargetBlur( pd3dDevice, &m_Surface[0] );		// 게임화면을 블러해서 [0]에 넣음
#endif
	m_Surface[0].EndScene();
	
	int i;
	// 블러 강도를 높이기위해 반복함.

	int nMax = g_Option.m_nBloom * 3;
#ifdef __XGLARE2
	nMax = 1;
	for( i = 0; i < nMax; i ++ )
	{
		m_Surface[1].BeginScene();
		m_Surface[0].RenderTargetBlurH( pd3dDevice, &m_Surface[1] );		// 0의 내용을 1로 블러
		m_Surface[1].EndScene();
		
		m_Surface[0].BeginScene();
		m_Surface[1].RenderTargetBlurH( pd3dDevice, &m_Surface[0] );		// 다시 1의 내용을 0으로 블러
		m_Surface[0].EndScene();
	}
	for( i = 0; i < nMax; i ++ )
	{
		m_Surface[1].BeginScene();
		m_Surface[0].RenderTargetBlurV( pd3dDevice, &m_Surface[1] );		// 0의 내용을 1로 블러
		m_Surface[1].EndScene();
		
		m_Surface[0].BeginScene();
		m_Surface[1].RenderTargetBlurV( pd3dDevice, &m_Surface[0] );		// 다시 1의 내용을 0으로 블러
		m_Surface[0].EndScene();
	}
#else
	for( i = 0; i < nMax; i ++ )
	{
		m_Surface[1].BeginScene();
		m_Surface[0].RenderTargetBlur( pd3dDevice, &m_Surface[1] );		// 
		m_Surface[1].EndScene();
		
		m_Surface[0].BeginScene();
		m_Surface[1].RenderTargetBlur( pd3dDevice, &m_Surface[0] );
		m_Surface[0].EndScene();
	}
#endif



//	m_Surface[0].BeginScene();
//	m_Black.bTest = TRUE;
//	m_Black.RenderNormal( pd3dDevice, &m_Surface[0], TRUE );
//	m_Black.bTest = FALSE;
//	m_Surface[0].EndScene();
		

	
#endif // Client
}




//
//
void CGlareLevel::RenderGlareEffect( LPDIRECT3DDEVICE9 pd3dDevice )
{
//	return;
	float	fOffset = g_Option.m_nBloom * 3.0f;
#ifdef __XGLARE2
	fOffset = 1;
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
#else
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
#endif
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	int nWidth = g_Option.m_nResWidth;
	int nHeight = g_Option.m_nResHeight - 48;
	float vertex2D[4][4+2] = 
	{
//		{   -0.0f,  -0.0f,0,1, 0,0 },
//		{ nWidth-0.0f,  -0.0f,0,1, 1,0 },
//		{ nWidth-0.0f,nHeight-0.0f,0,1, 1,1 },
//		{   -0.0f,nHeight-0.0f,0,1, 0,1 },
		{   -fOffset,  -fOffset,0,1, 0,0 },
		{ nWidth-fOffset,  -fOffset,0,1, 1,0 },
		{ nWidth-fOffset,nHeight-fOffset,0,1, 1,1 },
		{   -fOffset,nHeight-fOffset,0,1, 0,1 },
	};
	pd3dDevice->SetTexture( 0, m_Surface[0].m_pTexture );
	pd3dDevice->SetTexture( 1, NULL );
	pd3dDevice->SetTexture( 2, NULL );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 2, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	
	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetVertexDeclaration( NULL );
	pd3dDevice->SetPixelShader( NULL );
	pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
	pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, vertex2D, sizeof(float)*(4+2) );

	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, s_bLight );
	pd3dDevice->SetTexture( 0, NULL );
	pd3dDevice->SetTexture( 1, NULL );
	pd3dDevice->SetTexture( 2, NULL );
	
}








