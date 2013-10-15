#include "stdafx.h"
#include "TailEffectMng.h"


#ifdef __CLIENT


const DWORD TAILVERTEX::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;


CTailModelMng::CTailModelMng()
{
	m_ListTailModel.clear();
}
CTailModelMng::~CTailModelMng()
{
}


//
//
//
CTailEffectBelt::CTailEffectBelt()
{
	Init();
}

CTailEffectBelt::~CTailEffectBelt()
{
	Destroy();
}

void CTailEffectBelt::Init( void )
{
	m_bActive	= FALSE;
	m_pPool		= NULL;
	m_nPoolPtr	= 0;
	m_pTexture = NULL;
	m_pVB		= NULL;
	m_pTails	= NULL;
	m_pTailsFree = NULL;
    m_dwBase		= 0;
    m_dwFlush		= 0;
	m_dwDiscard		= 0;
	m_nMaxTail		= 0;	
	m_nType			= 0;
	m_fFadeSpeed    = 0.0f;
}

// 할당한 메모리들은 그대로 두고 초기화
void CTailEffectBelt::Clear( void )
{
	m_bActive	= FALSE;
	m_nPoolPtr	= 0;
	m_pTails	= NULL;
	m_pTailsFree	= NULL;
	m_dwBase = 0;
	m_dwFlush = 0;
	m_dwDiscard = 0;
	m_nMaxTail = 0;
	m_nType = 0;
}

void CTailEffectBelt::Destroy( void )
{
	InvalidateDeviceObjects();
	SAFE_RELEASE( m_pTexture );
	SAFE_DELETE_ARRAY( m_pPool );
	Init();
}

#define		MAX_TAIL	512

void CTailEffectBelt::Create( int nType, FLOAT fFadeSpeed )
{
	if( m_pPool == NULL )
		Init();

	m_bActive = TRUE;
    m_dwBase         = MAX_TAIL;
    m_dwFlush        = 512;
	m_dwDiscard      = MAX_TAIL;
	m_nType			= nType;
	m_fFadeSpeed     = fFadeSpeed;

	if( m_pPool == NULL )
		m_pPool = new TAIL[ MAX_TAIL ];		// MAX_TAIL / 60fps = ??;  최대??초정도길이의 띠를 만들수있다.
	memset( m_pPool, 0, sizeof(TAIL) * MAX_TAIL );
	
}

//
// 꼬리 하나를 생성시킴.
//
int	CTailEffectBelt::CreateTail( const D3DXVECTOR3 &vPos1, const D3DXVECTOR3 &vPos2 )
{
	if( m_bActive == FALSE )	return 1;
	if( m_pTexture == NULL )	return 1;
	TAIL *pTail;
	
	// 꼬리 갯수가 맥스치를 넘치 않게.
	if( m_nMaxTail >= MAX_TAIL )		return 0;
    
    if( m_pTailsFree )		// 비어있는 파티클 포인터가 있는가.
    {
        pTail = m_pTailsFree;		// 생성될 파티클을 비어있는 포인터로 설정.
        m_pTailsFree = pTail->m_pNext;	// 파티클 리스트에 추가.
    }
    else
    {
		pTail = &m_pPool[ m_nPoolPtr++ ];
		if( m_nPoolPtr >= MAX_TAIL )
			m_nPoolPtr = 0;
    }
	
    pTail->m_pNext = m_pTails;	// 새로 생성된 꼬리의 다음노드에 현재 꼬리 설정.
    m_pTails = pTail;			// 새로 생성된 꼬리을 현재 노드에 설정.
    m_nMaxTail++;				// 꼬리 개수 증가.
	
	
    pTail->m_vPos1 = vPos1;		// 꼬리좌표 1
    pTail->m_vPos2 = vPos2;		// 2
	
	pTail->m_clrDiffuse = D3DXCOLOR( 1.0f,   1.0f,   1.0f,   1.0f );
	pTail->m_clrFade    = D3DXCOLOR( 1.0f,   1.0f,  1.0f,  0.0f );
    pTail->m_fFade      = 0.0f;
	
	return 1;
}

//
//
//
HRESULT CTailEffectBelt::FrameMove( void )
{
	if( m_bActive == FALSE )	return S_OK;
	if( m_pTexture == NULL )	return S_OK;
	
    TAIL *pTail, **ppTail;
	
    ppTail = &m_pTails;
	
    while( *ppTail )
    {
        pTail = *ppTail;
		
        // Calculate new position
//		pTail->m_fFade += 0.030f;
		pTail->m_fFade += m_fFadeSpeed;
		
//		pTail->m_vPos;		// 꼬리는 최초좌표가 변하지않음.
		
        if( pTail->m_fFade > 2.0f )
            pTail->m_fFade = 2.0F;
		
        // Kill old Tails
		// 희미해져 사라졌을때는 삭제됨.
        if( pTail->m_fFade >= 2.0f )
        {
            // Kill Tail
            *ppTail = pTail->m_pNext;
            pTail->m_pNext = m_pTailsFree;
            m_pTailsFree = pTail;
			m_nMaxTail--;
        }
        else
        {
            ppTail = &pTail->m_pNext;
        }
    }
	
	
    return S_OK;
}



//---------------------------------------------------------------------
//
//---------------------------------------------------------------------
HRESULT CTailEffectBelt::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName )
{
	HRESULT hr;
	
	if( m_bActive == FALSE )	return S_OK;
	if( m_pTexture )			return S_OK;
	
	// Create the texture using D3DX
	hr = LoadTextureFromRes( pd3dDevice, MakePath( DIR_MODELTEX, szFileName ), 
								D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, 
								D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
								D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 0, NULL, NULL, &m_pTexture );
	if( hr == E_FAIL )
	{
		Error( "%s 읽기 실패", MakePath( DIR_MODELTEX, szFileName ) );
		m_bActive = FALSE;
	}
	 
	return hr;
}

HRESULT CTailEffectBelt::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( m_bActive == FALSE )	return S_OK;
	if( m_pTexture == NULL )	return S_OK;
	if( m_pVB )					
		return S_OK;		// 이미 할당되어 있으면 다시 할당 안함.
    HRESULT hr;
	
    if(FAILED(hr = pd3dDevice->CreateVertexBuffer( (MAX_TAIL * 2) *		// 꼬랑지리스트 하나당 버텍스2개기땜에 * 2
													sizeof(TAILVERTEX), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 
													TAILVERTEX::FVF, D3DPOOL_DEFAULT, &m_pVB, NULL )))
	{
        return E_FAIL;
	}
	
    return S_OK;
}

HRESULT CTailEffectBelt::ChangeTexture( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName, int nType )
{
	SAFE_RELEASE( m_pTexture );
	m_nType = nType;
	return InitDeviceObjects( pd3dDevice, szFileName );
}

HRESULT CTailEffectBelt::InvalidateDeviceObjects()
{
    SAFE_RELEASE( m_pVB );
	
    return S_OK;
}


HRESULT CTailEffectBelt::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( m_bActive == FALSE )	return E_FAIL;
	if( m_pTexture == NULL )	return E_FAIL;
	if( m_nMaxTail < 2 )		return E_FAIL;		// 꼬리는 최소 2개노드(버텍스4개)가 있어야 삼각형스트립 2개를 만들어 낼수 있다.

	D3DXMATRIX mWorld;
	D3DXMatrixIdentity( &mWorld );
	pd3dDevice->SetTransform( D3DTS_WORLD, &mWorld );		// Set World Transform 
	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,   D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,   D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,   D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2,   D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	
//	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
//	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,  FALSE );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_NONE );
//    pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffffff );

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	
	pd3dDevice->SetTexture(0, m_pTexture );
	
    pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(TAILVERTEX) );
	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetVertexDeclaration( NULL );
    pd3dDevice->SetFVF( TAILVERTEX::FVF );
	
    HRESULT hr;
	
    TAIL*    pTail = m_pTails;
    TAILVERTEX* pVertices;
    DWORD        dwNumTailsToRender = 0;
	
	m_dwBase += m_dwFlush;
	
	if(m_dwBase >= m_dwDiscard)
		m_dwBase = 0;
	
	if( FAILED( hr = m_pVB->Lock( m_dwBase * sizeof(TAILVERTEX), m_dwFlush * sizeof(TAILVERTEX),
		(void**) &pVertices, m_dwBase ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD ) ) )
    {
		return hr;
	}

	D3DXCOLOR clrDiffuse;
	D3DXVECTOR3	vPos1, vPos2;
	int		nCount = 0;
	float	fTy;
	float	fFade;
	
    while( pTail )
	{
        vPos1 = pTail->m_vPos1;		// 꼬리좌표 1
		vPos2 = pTail->m_vPos2;		// 꼬리좌표 2

		if( pTail->m_fFade > 1.0f )
			fFade = 2.0f - pTail->m_fFade;
		else
			fFade = pTail->m_fFade;
        D3DXColorLerp( &clrDiffuse, &pTail->m_clrFade, &pTail->m_clrDiffuse, fFade );
        DWORD dwDiffuse = (DWORD) clrDiffuse;
		
		if( nCount & 1 )	fTy = 1.0f;
		else				fTy = 0.0f;

		// 버텍스 버퍼로 옮김.
		pVertices->v     = vPos1;
		pVertices->color = dwDiffuse;
		pVertices->tx	= 0.0f;
		pVertices->ty	= fTy;
		pVertices++;

		pVertices->v     = vPos2;
		pVertices->color = dwDiffuse;
		pVertices->tx	= 1.0f;
		pVertices->ty	= fTy;
		pVertices++;
		
		if( ++dwNumTailsToRender == m_dwFlush )
		{
			m_pVB->Unlock();
			
			if(FAILED(hr = pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, m_dwBase, (dwNumTailsToRender * 2) - 2 )))
				return hr;
			
			m_dwBase += m_dwFlush;
			
			if(m_dwBase >= m_dwDiscard)
				m_dwBase = 0;
			
			if( FAILED( hr = m_pVB->Lock( m_dwBase * sizeof(TAILVERTEX), m_dwFlush * sizeof(TAILVERTEX),
				(void**) &pVertices, m_dwBase ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD ) ) )
			{
				return hr;
			}
			
			dwNumTailsToRender = 0;
		}

        pTail = pTail->m_pNext;
		nCount ++;
	}

    m_pVB->Unlock();

#ifdef _DEBUG
	if( dwNumTailsToRender == 1 )
		Error( "TailRender : 렌더할 꼬리개수가 넘 작은경우 발생. %d", dwNumTailsToRender );
#endif

    // Render any remaining particles
    if( dwNumTailsToRender )		// 위에서 512개씩 출력해주고 남은 버텍스들을 여기서 마저 다 그려줌.
    {
		if(FAILED(hr = pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, m_dwBase, (dwNumTailsToRender * 2) - 2 )))	// 트라이앵글 스트립은 프리미티브 개수가 버텍스개수 - 2이다.
			return hr;
    }
	
	
	pd3dDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_CCW );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,  TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pd3dDevice->SetTexture(0, NULL );
	

    return S_OK;
}
	
CTailEffectModel::CTailEffectModel()
{
	Init();
}

CTailEffectModel::~CTailEffectModel()
{
	Destroy();
}

void CTailEffectModel::Init( void )
{
	m_bActive		= FALSE;
	m_nType			= 0;
	m_fFadeSpeed    = 0.0f;
	m_pModel     = NULL;
	m_nMaxTail   = 0;
}

// 할당한 메모리들은 그대로 두고 초기화
void CTailEffectModel::Clear( void )
{
	m_bActive	= FALSE;
	m_nType = 0;
	m_pModel    = NULL;
	m_nMaxTail  = 0;
}

void CTailEffectModel::Destroy( void )
{
	InvalidateDeviceObjects();
	m_pModel->DeleteDeviceObjects();
	SAFE_DELETE(m_pModel);	
	m_vecTail.clear();
	Init();
}

void CTailEffectModel::Create( int nType, FLOAT fFadeSpeed )
{
	Init();

	m_bActive = TRUE;
	m_nType			= nType;
	m_fFadeSpeed     = fFadeSpeed;
}

//
// 꼬리 하나를 생성시킴.
//
int	CTailEffectModel::CreateTail( D3DXMATRIX* mWorld )
{
	if( m_bActive == FALSE )	return 1;
	
	// 꼬리 갯수가 맥스치를 넘치 않게.
	if( m_vecTail.size() > MAX_TAIL )		return 0;

	TAILMODEL Tail;

	Tail.m_mWorld  = *mWorld;
	Tail.m_nFactor = 255;

	m_vecTail.push_back( Tail );
	
    m_nMaxTail++;				// 꼬리 개수 증가.

	return 1;
}

//
//
//
HRESULT CTailEffectModel::FrameMove( void )
{
	if( m_bActive == FALSE )	return S_OK;

	for( int i=0; i< (int)( m_vecTail.size() ); i++ )
	{
		if( m_vecTail[i].m_nFactor < 10 )
			continue;
			
		m_vecTail[i].m_nFactor	= (int)( m_vecTail[i].m_nFactor - m_fFadeSpeed );
	}
	
    return S_OK;
}



//---------------------------------------------------------------------
//
//---------------------------------------------------------------------
HRESULT CTailEffectModel::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName )
{
	if( m_bActive == FALSE )	return S_OK;

	m_pModel = new CModelObject;
	m_pModel->InitDeviceObjects(pd3dDevice);
	if( m_pModel->LoadElement(szFileName, 0 ) == SUCCESS )
		return S_OK;
	else
		return E_FAIL;
}

HRESULT CTailEffectModel::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( m_bActive == FALSE )	return S_OK;
	
    return S_OK;
}

HRESULT CTailEffectModel::InvalidateDeviceObjects()
{
    return S_OK;
}


HRESULT CTailEffectModel::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( m_bActive == FALSE )	return E_FAIL;

	D3DXMATRIX mWorld;
	D3DXMatrixIdentity( &mWorld );
				

	for( int i=0; i< (int)( m_vecTail.size() ); i++ )
	{
		if( m_vecTail[i].m_nFactor < 10 )
			continue;
			
		m_pModel->SetEffect( 0, XE_NOBUMP );
//		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE);
		//pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		m_pModel->SetBlendFactor(m_vecTail[i].m_nFactor);
		m_pModel->Render( pd3dDevice, &(m_vecTail[i].m_mWorld) );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
//		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);
		//pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	}
	
    return S_OK;
}
	
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//						class CTailEffectMng

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
CTailEffectMng	g_TailEffectMng;

CTailEffectMng::CTailEffectMng()
{
	Init();
}

CTailEffectMng::~CTailEffectMng()
{
	Destroy();
}

void CTailEffectMng::Init( void )
{
	m_bActive = TRUE;
	m_nMaxType = 0;

	for( int i = 0; i < MAX_TAILEFFECT; i ++ )
	{
		SAFE_DELETE( m_TailEffects[i] );
	}	
}

void CTailEffectMng::Destroy( void )
{
	// 이곳에 파괴 코드를 넣으셈.
	
	Init();
}

HRESULT CTailEffectMng::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	int		i;
	for( i = 0; i < MAX_TAILEFFECT; i ++ )
	{
		if( m_TailEffects[ i ] == NULL ) continue;
		m_TailEffects[i]->RestoreDeviceObjects( pd3dDevice );
	}
	
	return S_OK;
}

HRESULT CTailEffectMng::InvalidateDeviceObjects( void )
{
	int		i;
	for( i = 0; i < MAX_TAILEFFECT; i ++ )
	{
		if( m_TailEffects[ i ] == NULL ) continue;
		m_TailEffects[i]->InvalidateDeviceObjects();
	}
	
	return S_OK;
}

//
// 파티클 하나 생성.
//
CTailEffect *CTailEffectMng::AddEffect( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName, int nType, FLOAT fFadeSpeed )
{
	int		i;
	for( i = 0; i < MAX_TAILEFFECT; i ++ )
	{
		if( m_TailEffects[ i ] != NULL ) continue;

		if( nType != 100 )
			m_TailEffects[ i ] = new CTailEffectBelt;
		else
			m_TailEffects[ i ] = new CTailEffectModel;
		
		//if( m_TailEffects[ i ]->IsActive() == TRUE )	continue;

		m_TailEffects[ i ]->Create( nType, fFadeSpeed );		// 꼬리메모리 할당하고
		m_TailEffects[ i ]->InitDeviceObjects( pd3dDevice, szFileName );	// 텍스쳐 읽고
		m_TailEffects[ i ]->RestoreDeviceObjects( pd3dDevice );	// 버텍스 버퍼 할당하고.
		m_nMaxType ++;
		return m_TailEffects[ i ];
	}
	
	return NULL;
}

// pTail을 찾아서 지움.
int		CTailEffectMng::Delete( CTailEffect *pTail )
{
	int		i;
	for( i = 0; i < MAX_TAILEFFECT; i ++ )
	{
		if( m_TailEffects[i] == pTail )
		{
			m_TailEffects[i]->Destroy();		// 메모리는 재할당하지 않고 초기화만 시킨다.
			SAFE_DELETE( m_TailEffects[i] );
			m_nMaxType --;
			return 1;
		}
	}

	return 0;
}

void CTailEffectMng::Process( void )
{
	int		i;
	for( i = 0; i < MAX_TAILEFFECT; i ++ )
	{
		if( m_TailEffects[ i ] == NULL ) continue;
		if( m_TailEffects[i]->IsActive() == FALSE )	continue;
		m_TailEffects[i]->FrameMove();
	}
}

void CTailEffectMng::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	int		i;
	for( i = 0; i < MAX_TAILEFFECT; i ++ )
	{
		if( m_TailEffects[ i ] == NULL ) continue;
		if( m_TailEffects[i]->IsActive() == FALSE )	continue;
		m_TailEffects[i]->Render( pd3dDevice );
	}
}


#endif // CLIENT
