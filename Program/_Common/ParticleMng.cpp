#include "stdafx.h"
#include "ParticleMng.h"


const DWORD POINTVERTEX::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

D3DXCOLOR g_clrColor = D3DXCOLOR( 1.0f,   0.5f,   0.5f,   1.0f );
DWORD g_clrColorFade = D3DXCOLOR( 1.0f,   0.25f,  0.25f,  1.0f );

//D3DXCOLOR g_clrColor2 = D3DXCOLOR( 0.125f,   0.5f,   1.0f,   1.0f );
D3DXCOLOR g_clrColor2 = D3DXCOLOR( 0.35f,   0.35f,   1.00f,   1.0f );
DWORD g_clrColorFade2 = D3DXCOLOR( 0.0f,   0.0f,  0.0f,  1.0f );


//CParticles		g_Particle( 512, 2048, 0 );
//CParticles		g_Particle2( 512, 2048, 1 );

//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------

CParticles::CParticles() 
{
	Init();
}

CParticles::CParticles( DWORD dwFlush, DWORD dwDiscard, int nType )
{
	Create( dwFlush, dwDiscard, nType );
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
CParticles::~CParticles()
{
	Destroy();
}


void CParticles::Init( void )
{
	m_bActive = 0;	
	m_bGravity = 0;
    m_dwBase		= 0;
    m_dwFlush		= 0;
	m_dwDiscard		= 0;
	m_fSize			= 0.3f;
	
    m_dwParticles    = 0;
    m_dwParticlesLim = 0;
	m_nType = 0;
	
    m_pParticles     = NULL;
    m_pParticlesFree = NULL;
	m_pVB            = NULL;
	m_pParticleTexture = NULL;
	m_pPool	= NULL;
//	m_pPool = new PARTICLE[ m_dwParticlesLim ];
//	memset( m_pPool, 0, sizeof(PARTICLE) * m_dwParticlesLim );
	m_nPoolPtr = 0;
}

void CParticles::Destroy( void )
{
	InvalidateDeviceObjects();
	SAFE_RELEASE( m_pParticleTexture );
	SAFE_DELETE_ARRAY( m_pPool );
}

void CParticles::Create( DWORD dwFlush, DWORD dwDiscard, int nType )
{
//	int		i;

	Init();

	m_bActive = TRUE;	
    m_dwBase         = dwDiscard;
    m_dwFlush        = dwFlush;
	m_dwDiscard      = dwDiscard;
	
    m_dwParticlesLim = dwDiscard;
	m_nType = nType;
	
	m_pPool = new PARTICLE[ m_dwParticlesLim ];
	memset( m_pPool, 0, sizeof(PARTICLE) * m_dwParticlesLim );
}



HRESULT CParticles::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName )
{
	HRESULT hr;

	if( m_bActive == FALSE )	return S_OK;

#ifdef __CLIENT
			
	// Create the texture using D3DX
	hr = LoadTextureFromRes( pd3dDevice, MakePath( DIR_MODELTEX, szFileName ), 
									   D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, 
									   D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
									   D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 0, NULL, NULL, &m_pParticleTexture );
	if( hr == E_FAIL )
	{
		Error( "%s 읽기 실패", MakePath( DIR_MODELTEX, szFileName ) );
		m_bActive = FALSE;
	}

	return hr;
#else
	return E_FAIL;
#endif
}



//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT CParticles::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( m_bActive == FALSE )			return S_OK;
	if( m_pParticleTexture == NULL )	return S_OK;
    HRESULT hr;

    // Create a vertex buffer for the particle system.  The size of this buffer
    // does not relate to the number of particles that exist.  Rather, the
    // buffer is used as a communication channel with the device.. we fill in 
    // a bit, and tell the device to draw.  While the device is drawing, we
    // fill in the next bit using NOOVERWRITE.  We continue doing this until 
    // we run out of vertex buffer space, and are forced to DISCARD the buffer
    // and start over at the beginning.

    if(FAILED(hr = pd3dDevice->CreateVertexBuffer( m_dwDiscard * 
		sizeof(POINTVERTEX), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY | D3DUSAGE_POINTS, 
        POINTVERTEX::FVF, D3DPOOL_DEFAULT, &m_pVB, NULL )))
	{
        return E_FAIL;
	}

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT CParticles::InvalidateDeviceObjects()
{
    SAFE_RELEASE( m_pVB );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------

HRESULT CParticles::Update( void )
{
//	if( m_nType == 1 )	return Update2();
	if( m_bActive == FALSE )	return S_OK;
#ifdef __WORLDSERVER
	return S_OK;
#endif
	if( m_pParticleTexture == NULL )	return S_OK;

    PARTICLE *pParticle, **ppParticle;

    ppParticle = &m_pParticles;

    while( *ppParticle )
    {
        pParticle = *ppParticle;

        // Calculate new position
		pParticle->m_fFade -= 0.015f;

        pParticle->m_vPos	+= pParticle->m_vVel;
		if( m_bGravity )
			pParticle->m_vVel.y	-= 0.005f;		// 중력.

        if( pParticle->m_fFade < 0.0f )
            pParticle->m_fFade = 0;

        // Kill old particles
		// 희미해져 사라졌을때나 바닥에 떨어졌을때는 삭제됨.
        if( pParticle->m_fFade <= 0 || (m_bGravity && pParticle->m_vPos.y < pParticle->m_fGroundY) )
        {
            // Kill particle
            *ppParticle = pParticle->m_pNext;
            pParticle->m_pNext = m_pParticlesFree;
            m_pParticlesFree = pParticle;

//            if(!pParticle->m_bSpark)
                m_dwParticles--;
        }
        else
        {
            ppParticle = &pParticle->m_pNext;
        }
    }


    return S_OK;
}

HRESULT CParticles::Update2( void )
{
	if( m_bActive == FALSE )	return S_OK;
#ifdef __WORLDSERVER
	return S_OK;
#endif
	if( m_pParticleTexture == NULL )	return S_OK;

    PARTICLE *pParticle, **ppParticle;

    ppParticle = &m_pParticles;

    while( *ppParticle )
    {
        pParticle = *ppParticle;

        // Calculate new position
		pParticle->m_fFade -= 0.015f;

		pParticle->m_vPos	+= pParticle->m_vVel;

        if( pParticle->m_fFade < 0 )
            pParticle->m_fFade = 0;

        // Kill old particles
        if( pParticle->m_fFade <= 0 )
        {
            // Kill particle
            *ppParticle = pParticle->m_pNext;
            pParticle->m_pNext = m_pParticlesFree;
            m_pParticlesFree = pParticle;

//            if(!pParticle->m_bSpark)
                m_dwParticles--;
        }
        else
        {
            ppParticle = &pParticle->m_pNext;
        }
    }


    return S_OK;
}


// 파티클 객체 하나를 생성한다.
// vPos : 생성되는 시작위치
// vVel : 날아갈 방향.
HRESULT CParticles::CreateParticle( int nType, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vVel, FLOAT fGroundY )
{
	if( m_bActive == FALSE )	return S_OK;
#ifdef __WORLDSERVER
	return S_OK;
#endif
	if( m_pParticleTexture == NULL )	return S_OK;
	PARTICLE *pParticle;
//    static float fTime = 0.0f;
    //fTime += fSecsPerFrame;
	
    // Emit new particles - 새로운 파티클을 뿜어내다.
//    DWORD dwParticlesEmit = m_dwParticles + dwNumParticlesToEmit;
	// 파티클 갯수가 맥스치를 넘치 않게.
	if( m_dwParticles >= m_dwParticlesLim )		return E_FAIL;
    
    if( m_pParticlesFree )		// 비어있는 파티클 포인터가 있는가.
    {
        pParticle = m_pParticlesFree;		// 생성될 파티클을 비어있는 포인터로 설정.
        m_pParticlesFree = pParticle->m_pNext;	// 파티클 리스트에 추가.
    }
    else
    {
//        if( NULL == ( pParticle = new PARTICLE ) )		// 나중에 메모리 풀로 바꾸자.,
  //          return E_OUTOFMEMORY;
		pParticle = &m_pPool[ m_nPoolPtr++ ];
		if( m_nPoolPtr >= (int)m_dwParticlesLim )
			m_nPoolPtr = 0;
    }
	
    pParticle->m_pNext = m_pParticles;	// 새로 생성된 파티클의 다음노드에 현재 파티클 설정.
    m_pParticles = pParticle;			// 새로 생성된 파티클을 현재 노드에 설정.
    m_dwParticles++;					// 파티클 개수 증가.
	
    // Emit new particle
	
	// 최초 위치 세팅
    pParticle->m_vPos = vPos;
	// 속도벡터 생성.
	pParticle->m_vVel = vVel;
	
	if( m_nType == 0 )
	{
		pParticle->m_clrDiffuse = g_clrColor;
		pParticle->m_clrFade    = g_clrColorFade;
	} else
	if( m_nType < 13 )
	{
		pParticle->m_clrDiffuse = g_clrColor2;
		pParticle->m_clrFade    = g_clrColorFade2;
	} else
	{
		pParticle->m_clrDiffuse = D3DXCOLOR( 1.0f, 1.0f,  1.0f,  1.0f );;
		pParticle->m_clrFade    = D3DXCOLOR( 0.0f, 0.0f,  0.0f,  1.0f );
	}
    pParticle->m_fFade      = 1.0f;
	pParticle->m_fGroundY	= fGroundY;
	
	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Renders the particle system using either pointsprites (if supported)
//       or using 4 vertices per particle
//-----------------------------------------------------------------------------
HRESULT CParticles::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( m_bActive == FALSE )	return S_OK;
	if( m_pParticleTexture == NULL )	return S_OK;
	if( m_dwParticles <= 0 )	return S_OK;

	D3DXMATRIX mWorld;
	D3DXMatrixIdentity( &mWorld );
	pd3dDevice->SetTransform( D3DTS_WORLD, &mWorld );		// Set World Transform 
	
	// Draw particles
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,   D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,   D3DTA_CURRENT );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,  FALSE );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_CCW );
    pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffffff );
//	pd3dDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_FLAT );
	
	//
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	
	pd3dDevice->SetTexture(0, m_pParticleTexture );
	
    HRESULT hr;

    // Set the render states for using point sprites
    pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  TRUE );
	pd3dDevice->SetRenderState( D3DRS_POINTSIZE, FtoDW(m_fSize) );
		
    pd3dDevice->SetRenderState( D3DRS_POINTSIZE_MIN, FtoDW(1.00f) );
    pd3dDevice->SetRenderState( D3DRS_POINTSCALE_A,  FtoDW(0.00f) );
    pd3dDevice->SetRenderState( D3DRS_POINTSCALE_B,  FtoDW(0.00f) );
    pd3dDevice->SetRenderState( D3DRS_POINTSCALE_C,  FtoDW(1.00f) );

    // Set up the vertex buffer to be rendered
    pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(POINTVERTEX) );
	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetVertexDeclaration( NULL );
    pd3dDevice->SetFVF( POINTVERTEX::FVF );

    PARTICLE*    pParticle = m_pParticles;
    POINTVERTEX* pVertices;
    DWORD        dwNumParticlesToRender = 0;



	// Lock the vertex buffer.  We fill the vertex buffer in small
	// chunks, using D3DLOCK_NOOVERWRITE.  When we are done filling
	// each chunk, we call DrawPrim, and lock the next chunk.  When
	// we run out of space in the vertex buffer, we start over at
	// the beginning, using D3DLOCK_DISCARD.

	m_dwBase += m_dwFlush;

	if(m_dwBase >= m_dwDiscard)
		m_dwBase = 0;

	if( FAILED( hr = m_pVB->Lock( m_dwBase * sizeof(POINTVERTEX), m_dwFlush * sizeof(POINTVERTEX),
		(void**) &pVertices, m_dwBase ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD ) ) )
    {
		return hr;
	}

		
	D3DXCOLOR clrDiffuse;
	D3DXVECTOR3 vPos;
	D3DXVECTOR3 vVel;
	
    // 파티클 리스트를 따라 파티클을 렌더링한다.
    while( pParticle )
    {
        vPos = pParticle->m_vPos;
        vVel = pParticle->m_vVel;
/*
		// 파티클의 잔상을 보여주기 위한 부분.
        FLOAT       fLengthSq = D3DXVec3LengthSq(&vVel);
        UINT        dwSteps;

        if( fLengthSq < 1.0f )        dwSteps = 2;
        else if( fLengthSq <  4.00f ) dwSteps = 3;
        else if( fLengthSq <  9.00f ) dwSteps = 4;
        else if( fLengthSq < 12.25f ) dwSteps = 5;
        else if( fLengthSq < 16.00f ) dwSteps = 6;
        else if( fLengthSq < 20.25f ) dwSteps = 7;
        else                          dwSteps = 8;

        vVel *= -0.04f / (FLOAT)dwSteps;
*/
        D3DXColorLerp( &clrDiffuse, &pParticle->m_clrFade, &pParticle->m_clrDiffuse, pParticle->m_fFade );
        DWORD dwDiffuse = (DWORD) clrDiffuse;

        // Render each particle a bunch of times to get a blurring effect
//		dwSteps = 1;
//        for( DWORD i = 0; i < dwSteps; i++ )
        {
            pVertices->v     = vPos;
            pVertices->color = dwDiffuse;
            pVertices++;

            if( ++dwNumParticlesToRender == m_dwFlush )
            {
                // Done filling this chunk of the vertex buffer.  Lets unlock and
                // draw this portion so we can begin filling the next chunk.

                m_pVB->Unlock();

                if(FAILED(hr = pd3dDevice->DrawPrimitive( D3DPT_POINTLIST, m_dwBase, dwNumParticlesToRender)))
					return hr;

                // Lock the next chunk of the vertex buffer.  If we are at the 
                // end of the vertex buffer, DISCARD the vertex buffer and start
                // at the beginning.  Otherwise, specify NOOVERWRITE, so we can
                // continue filling the VB while the previous chunk is drawing.
				m_dwBase += m_dwFlush;

				if(m_dwBase >= m_dwDiscard)
					m_dwBase = 0;

				if( FAILED( hr = m_pVB->Lock( m_dwBase * sizeof(POINTVERTEX), m_dwFlush * sizeof(POINTVERTEX),
		            (void**) &pVertices, m_dwBase ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD ) ) )
                {
					return hr;
                }

                dwNumParticlesToRender = 0;
            }

//            vPos += vVel;		// 잔상처리를 위한 부분.
        } // for dwSteps

        pParticle = pParticle->m_pNext;
    } // particle list

    // Unlock the vertex buffer
    m_pVB->Unlock();

    // Render any remaining particles
    if( dwNumParticlesToRender )		// 위에서 512개씩 출력해주고 남은 파티클을 여기서 마저 다 그려줌.
    {
        if(FAILED(hr = pd3dDevice->DrawPrimitive( D3DPT_POINTLIST, m_dwBase, dwNumParticlesToRender )))
			return hr;
    }

    // Reset render states
    pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
    pd3dDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  FALSE );

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
//	pd3dDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
	
	
    return S_OK;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//						class CParticleMng

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
CParticleMng	g_ParticleMng;

CParticleMng::CParticleMng()
{
	Init();
}

CParticleMng::~CParticleMng()
{
	Destroy();
}

void CParticleMng::Init( void )
{
	m_bActive = TRUE;
	m_nMaxType = 0;
}

void CParticleMng::Destroy( void )
{
	// 이곳에 파괴 코드를 넣으셈.

	Init();
}

HRESULT CParticleMng::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	int		i;
	for( i = 0; i < MAX_PARTICLE_TYPE; i ++ )
		m_Particles[i].RestoreDeviceObjects( pd3dDevice );

	return S_OK;
}

HRESULT CParticleMng::InvalidateDeviceObjects( void )
{
	int		i;
	for( i = 0; i < MAX_PARTICLE_TYPE; i ++ )
		m_Particles[i].InvalidateDeviceObjects();
	
	return S_OK;
}

//
// 파티클 하나 생성.
//
CParticles *CParticleMng::CreateParticle( int nType, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vVel, FLOAT fGroundY )
{
	if( nType < 0 || nType >= MAX_PARTICLE_TYPE )
		return NULL;
	CParticles	*pParticles = &m_Particles[ nType ];

	if( pParticles->m_bActive == FALSE )		// 생성시킨적이 없는 파티클이면 최초 생성.
	{
		pParticles->Create( 512, 512, nType );
		char szFileName[32];
		switch( nType )
		{
		case 0:	pParticles->m_fSize = 0.3f;	strcpy( szFileName, "Sfx_ItemPatical01.dds" );	break;
		case 1:	pParticles->m_fSize = 0.2f;	strcpy( szFileName, "etc_Particle2.bmp" );	break;
		case 2:	pParticles->m_fSize = 0.5f;	strcpy( szFileName, "etc_Particle11.bmp" );	break;
		case 3:	pParticles->m_fSize = 0.3f;	strcpy( szFileName, "etc_Particle11.bmp" );	break;
		case 4:	pParticles->m_fSize = 0.2f;	strcpy( szFileName, "etc_Particle11.bmp" );	break;
		case 5:	pParticles->m_fSize = 0.3f;	strcpy( szFileName, "Sfx_ItemPatical06-01.dds" );	break;
		case 6:	pParticles->m_fSize = 0.3f;	strcpy( szFileName, "Sfx_ItemPatical06-02.dds" );	break;
		case 7:	pParticles->m_fSize = 0.3f;	strcpy( szFileName, "Sfx_ItemPatical06-03.dds" );	break;
		case 8:	pParticles->m_fSize = 0.3f;	strcpy( szFileName, "Sfx_ItemPatical06-04.dds" );	break;
		case 9:	pParticles->m_fSize = 0.3f;	strcpy( szFileName, "Sfx_ItemPatical06-05.dds" );	break;
		case 10:	pParticles->m_fSize = 0.5f;	strcpy( szFileName, "etc_ParticleCloud01.bmp" );	break;
		case 11:	pParticles->m_fSize = 0.3f;	strcpy( szFileName, "etc_ParticleCloud01.bmp" );	break;
		case 12:	pParticles->m_fSize = 0.2f;	strcpy( szFileName, "etc_ParticleCloud01.bmp" );	break;

		// blue
		case 13:	pParticles->m_fSize = 0.15f;		strcpy( szFileName, "etc_Particle11.bmp" );	break;
		case 14:	pParticles->m_fSize = 0.1f;	strcpy( szFileName, "etc_Particle11.bmp" );	break;
		case 15:	pParticles->m_fSize = 0.07f;	strcpy( szFileName, "etc_Particle11.bmp" );	break;

		// red
		case 16:	pParticles->m_fSize = 0.15f;		strcpy( szFileName, "etc_Particle12.bmp" );	break;
		case 17:	pParticles->m_fSize = 0.1f;	strcpy( szFileName, "etc_Particle12.bmp" );	break;
		case 18:	pParticles->m_fSize = 0.07f;	strcpy( szFileName, "etc_Particle12.bmp" );	break;

		// white
		case 19:	pParticles->m_fSize = 0.15f;		strcpy( szFileName, "etc_Particle13.bmp" );	break;
		case 20:	pParticles->m_fSize = 0.1f;	strcpy( szFileName, "etc_Particle13.bmp" );	break;
		case 21:	pParticles->m_fSize = 0.07f;	strcpy( szFileName, "etc_Particle13.bmp" );	break;

		// green
		case 22:	pParticles->m_fSize = 0.15f;		strcpy( szFileName, "etc_Particle14.bmp" );	break;
		case 23:	pParticles->m_fSize = 0.1f;	strcpy( szFileName, "etc_Particle14.bmp" );	break;
		case 24:	pParticles->m_fSize = 0.07f;	strcpy( szFileName, "etc_Particle14.bmp" );	break;

		default:
				strcpy( szFileName, "etc_Particle2.bmp" );	break;
		}

		pParticles->InitDeviceObjects( m_pd3dDevice, szFileName );		// 파티클 텍스쳐 로딩.
		pParticles->RestoreDeviceObjects( m_pd3dDevice );
		m_nMaxType ++;

	}

	pParticles->CreateParticle( nType, vPos, vVel, fGroundY );		// nType의 파티클 하나 생성.

	return pParticles;
}

void CParticleMng::Process( void )
{
	int		i;
	for( i = 0; i < MAX_PARTICLE_TYPE; i ++ )
		m_Particles[i].Update();
}

void CParticleMng::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	int		i;
	for( i = 0; i < MAX_PARTICLE_TYPE; i ++ )
		m_Particles[i].Render( pd3dDevice );
}
