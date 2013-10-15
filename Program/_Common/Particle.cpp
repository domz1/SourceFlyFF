// Particle.cpp: implementation of the CParticle class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Particle.h"

extern CNeuzApp         g_Neuz;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#if 0
float RandomNumber(float fMin, float fMax)
{
    if (fMin == fMax) 
        return(fMin);
    
    float fRandom = (float)rand() / (float)RAND_MAX;
    return((fRandom * (float)fabs(fMax-fMin))+fMin);
}

CUnit::CUnit()
{
    m_v3Pos      = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    m_v3Velocity = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    m_v3Accel    = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    m_v3Gravity  = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    m_v3Scal     = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );
    m_v3Rot      = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    m_v3StartRot = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	
    m_fLife       = 1.0f;
    m_fCurrTime   = 0.0f;

    m_Color       = D3DXCOLOR( 255.0f, 255.0f, 255.0f, 255.0f );

    m_fAniTime     = 5.0f;   
    m_fCurrAniTime = 0.0f;
    m_AniIndex     = 0;
}

CUnit::~CUnit()
{
}

HRESULT CUnit::FrameMove( FLOAT fElapsedTime, CEmitter* pEmitter )
{
    if( m_fLife != 0.0f )
    {
        m_fCurrTime += fElapsedTime;

        if( m_fCurrTime > m_fLife )
            return S_FALSE;
    }

    m_fCurrAniTime += (fElapsedTime * 50.0f);
    
    if( m_fCurrAniTime > m_fAniTime )
    {
        m_fCurrAniTime = 0.0f;

        m_AniIndex++;

        if( pEmitter->m_pvecTexture.size() <= m_AniIndex )
            m_AniIndex = 0;
    }
        
    FLOAT	Lerp = m_fCurrTime / m_fLife;
    
    D3DXColorLerp( &m_Color, &pEmitter->m_pUnit1->m_Color, &pEmitter->m_pUnit2->m_Color, Lerp );

    D3DXVec3Lerp( &m_v3Scal, &pEmitter->m_pUnit1->m_v3Scal, &pEmitter->m_pUnit2->m_v3Scal, Lerp );
    D3DXVec3Lerp( &m_v3Rot, &pEmitter->m_pUnit1->m_v3Rot, &pEmitter->m_pUnit2->m_v3Rot, Lerp );
    
    m_v3Velocity += ( m_v3Accel * fElapsedTime );
    m_v3Pos      += ( m_v3Velocity * fElapsedTime );

    return S_OK;
}


CEmitter::CEmitter()
{
	m_fCurrCps  = 0.0f;
    m_fCps      = 1.0f;
    m_ffCps     = 0.0f;
    m_fTerm     = 0.0f;
    m_fCurrTerm = 0.0f;
    
    m_bBillboard = TRUE;
    m_bActive    = TRUE;
    
    m_v3VelocityRot = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    m_v3AccelRot    = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    m_v3Pos         = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXMatrixIdentity( &m_WorldM );

    m_plistUnit.clear();
    m_pvecTexture.clear();

    m_vecvecVertex.resize( MAX_ANI );

    m_dwBlend1 = 1;
    m_dwBlend2 = 1;

    m_Kind     = 0;

    m_pUnit1 = new CUnit;
    m_pUnit2 = new CUnit;
    
    memset( m_pName, 0, sizeof(TCHAR) * 256 );


    // Rect
    m_TriRect[0].m_v3Pos.x = - 1.0f;
    m_TriRect[0].m_v3Pos.y = - 1.0f;
    m_TriRect[0].m_v3Pos.z = 0.0f;

    m_TriRect[1].m_v3Pos.x = - 1.0f;
    m_TriRect[1].m_v3Pos.y = 1.0f;
    m_TriRect[1].m_v3Pos.z = 0.0f;
    
    m_TriRect[2].m_v3Pos.x = 1.0f;
    m_TriRect[2].m_v3Pos.y = 1.0f;
    m_TriRect[2].m_v3Pos.z = 0.0f;
    
    m_TriRect[3].m_v3Pos.x = 1.0f;
    m_TriRect[3].m_v3Pos.y = - 1.0f;
    m_TriRect[3].m_v3Pos.z = 0.0f;


    // Cylinder
    int posindex = 0;

    for( int i=0; i<=360; i+=10 )
    {
        FLOAT Angle = D3DXToRadian(i);
        
        m_TriCylinder[posindex].m_v3Pos.x = 1.0f * cos(Angle);
        m_TriCylinder[posindex].m_v3Pos.y = 1.0f * -sin(Angle);
        m_TriCylinder[posindex].m_v3Pos.z = 0.0f;
        
        posindex++;
        
        m_TriCylinder[posindex].m_v3Pos.x = m_TriCylinder[posindex-1].m_v3Pos.x;
        m_TriCylinder[posindex].m_v3Pos.y = m_TriCylinder[posindex-1].m_v3Pos.y;
        m_TriCylinder[posindex].m_v3Pos.z = 1.0f;

        posindex++;
    }

}

CEmitter::~CEmitter()
{
    SAFE_DELETE( m_pUnit1 );
    SAFE_DELETE( m_pUnit2 );
    
    {
        list< CUnit* >::iterator iter       =  m_plistUnit.begin();
        list< CUnit* >::iterator iterEnd    =  m_plistUnit.end();
    
        while( iter != iterEnd )
        {
            CUnit* pUnit = *iter;
            SAFE_DELETE( pUnit );
            iter++;
        }
    
        m_plistUnit.clear();
    }

	ClearTexture();

    m_vecvecVertex.clear();
}

void CEmitter::ClearTexture()
{
	vector< CTextureParticle >::iterator iter       =  m_pvecTexture.begin();
	vector< CTextureParticle >::iterator iterEnd    =  m_pvecTexture.end();
    
	while( iter != iterEnd )
	{
		CTextureParticle Texture = *iter;
		SAFE_RELEASE( Texture.m_pTex );
		iter++;
	}
    
	m_pvecTexture.clear();
}


HRESULT CEmitter::SetTexture(LPDIRECT3DDEVICE9 pd3dDevice, TCHAR *pPath, TCHAR *pFilename)
{
	TCHAR		pFullFile[2048];

	if( pPath != NULL )
		_stprintf( pFullFile, "\\%s\\%s", pPath, pFilename );
	else
		_stprintf( pFullFile, "%s", pFilename );	

    LPDIRECT3DTEXTURE9 pTexture;

    HRESULT hr;
    hr = D3DXCreateTextureFromFile( pd3dDevice, pFullFile, &pTexture );
    
	CTextureParticle Texture;

    memset( &Texture, 0, sizeof(CTextureParticle) );

	strcpy( Texture.m_pName, pFilename );
	Texture.m_pTex = pTexture;

    m_pvecTexture.push_back( Texture );

	return S_OK;
	
}

HRESULT CEmitter::Push_Unit()
{
    CUnit* cDestUnit = new CUnit;
    
    cDestUnit->m_fLife        = RandomNumber( m_pUnit1->m_fLife, m_pUnit2->m_fLife );

    cDestUnit->m_v3Pos.x      = RandomNumber( m_pUnit1->m_v3Pos.x, m_pUnit2->m_v3Pos.x );
    cDestUnit->m_v3Pos.y      = RandomNumber( m_pUnit1->m_v3Pos.y, m_pUnit2->m_v3Pos.y );
    cDestUnit->m_v3Pos.z      = RandomNumber( m_pUnit1->m_v3Pos.z, m_pUnit2->m_v3Pos.z );

#if 1
	cDestUnit->m_v3Pos.x += m_WorldM._41;
	cDestUnit->m_v3Pos.y += m_WorldM._42;
	cDestUnit->m_v3Pos.z += m_WorldM._43;
#endif     

    cDestUnit->m_fAniTime     = RandomNumber( m_pUnit1->m_fAniTime, m_pUnit2->m_fAniTime );
    
    cDestUnit->m_v3Velocity.x = RandomNumber( m_pUnit1->m_v3Velocity.x, m_pUnit2->m_v3Velocity.x );
    cDestUnit->m_v3Velocity.y = RandomNumber( m_pUnit1->m_v3Velocity.y, m_pUnit2->m_v3Velocity.y );
    cDestUnit->m_v3Velocity.z = RandomNumber( m_pUnit1->m_v3Velocity.z, m_pUnit2->m_v3Velocity.z );

    cDestUnit->m_v3Accel.x = RandomNumber( m_pUnit1->m_v3Accel.x, m_pUnit2->m_v3Accel.x );
    cDestUnit->m_v3Accel.y = RandomNumber( m_pUnit1->m_v3Accel.y, m_pUnit2->m_v3Accel.y );
    cDestUnit->m_v3Accel.z = RandomNumber( m_pUnit1->m_v3Accel.z, m_pUnit2->m_v3Accel.z );
  
    cDestUnit->m_v3Gravity.x = RandomNumber( m_pUnit1->m_v3Gravity.x, m_pUnit2->m_v3Gravity.x );
    cDestUnit->m_v3Gravity.y = RandomNumber( m_pUnit1->m_v3Gravity.y, m_pUnit2->m_v3Gravity.y );
    cDestUnit->m_v3Gravity.z = RandomNumber( m_pUnit1->m_v3Gravity.z, m_pUnit2->m_v3Gravity.z );

    cDestUnit->m_v3StartRot.x = RandomNumber( m_pUnit1->m_v3StartRot.x, m_pUnit2->m_v3StartRot.x );
    cDestUnit->m_v3StartRot.y = RandomNumber( m_pUnit1->m_v3StartRot.y, m_pUnit2->m_v3StartRot.y );
    cDestUnit->m_v3StartRot.z = RandomNumber( m_pUnit1->m_v3StartRot.z, m_pUnit2->m_v3StartRot.z );
    
    m_plistUnit.push_back( cDestUnit );

    return S_OK;
}

HRESULT CEmitter::FrameMove( FLOAT fElapsedTime )
{
    m_fCurrTerm += fElapsedTime;
	m_fCurrCps  += fElapsedTime;

    if( m_fCurrTerm > m_fTerm && m_fTerm != 0.0f )
		goto g_Skip;

	if( m_fCurrCps >= m_ffCps )
	{
		m_fCurrCps = 0.0f;
		Push_Unit();
	}
	
g_Skip:
    list< CUnit* >::iterator    iter    = m_plistUnit.begin();
    list< CUnit* >::iterator    iterEnd = m_plistUnit.end();
    CUnit*  pUnit;
    
    while( iter != iterEnd )
    {
        pUnit = *iter;

        if( pUnit->FrameMove( fElapsedTime, this ) == S_FALSE )
        {
            list< CUnit* >::iterator iterDel = iter++;
            SAFE_DELETE(pUnit);
            m_plistUnit.erase( iterDel );
            continue;
        }

        SetVertex(pUnit);

        iter++;
    }

    return S_OK;
}

HRESULT CEmitter::SetVertex(CUnit *pUnit)
{
    D3DXVECTOR3 Pos = pUnit->m_v3Pos;

    if( m_Kind == KIND::RECTANGLE )
    {
        PARTICLE_VERTEX Tri[4];
    
        D3DXMATRIX  WorldM, ScalM, AniRotM, RotM;
        D3DXMatrixScaling( &ScalM, pUnit->m_v3Scal.x, pUnit->m_v3Scal.y, pUnit->m_v3Scal.z );
        D3DXMatrixRotationYawPitchRoll( &AniRotM, D3DXToRadian(pUnit->m_v3Rot.y), D3DXToRadian(pUnit->m_v3Rot.x), D3DXToRadian(pUnit->m_v3Rot.z) );
        D3DXMatrixRotationYawPitchRoll( &RotM, D3DXToRadian(pUnit->m_v3StartRot.y), D3DXToRadian(pUnit->m_v3StartRot.x), D3DXToRadian(pUnit->m_v3StartRot.z) );
		
        WorldM = ScalM  * AniRotM * RotM;

        if( m_bBillboard )
        {
            D3DXMATRIX InvView;
            
			InvView = g_matInvView;
            InvView._41 = InvView._42 = InvView._43 = 0.0f;

            WorldM *= InvView;
        }

#if 1
//        WorldM = WorldM * m_WorldM;
#else 
        WorldM = WorldM * m_WorldM;
#endif 

        D3DXVec3TransformCoord( &Tri[0].m_v3Pos, &m_TriRect[0].m_v3Pos, &WorldM );
        D3DXVec3TransformCoord( &Tri[1].m_v3Pos, &m_TriRect[1].m_v3Pos, &WorldM );
        D3DXVec3TransformCoord( &Tri[2].m_v3Pos, &m_TriRect[2].m_v3Pos, &WorldM );
        D3DXVec3TransformCoord( &Tri[3].m_v3Pos, &m_TriRect[3].m_v3Pos, &WorldM );

        Tri[0].m_v3Pos.x += Pos.x;
        Tri[0].m_v3Pos.y += Pos.y;
        Tri[0].m_v3Pos.z += Pos.z;
    
        Tri[1].m_v3Pos.x += Pos.x;
        Tri[1].m_v3Pos.y += Pos.y;
        Tri[1].m_v3Pos.z += Pos.z;
    
        Tri[2].m_v3Pos.x += Pos.x;
        Tri[2].m_v3Pos.y += Pos.y;
        Tri[2].m_v3Pos.z += Pos.z;
    
        Tri[3].m_v3Pos.x += Pos.x;
        Tri[3].m_v3Pos.y += Pos.y;
        Tri[3].m_v3Pos.z += Pos.z;

		FLOAT fc = pUnit->m_Color.a / 255.0f;

		BYTE a = (BYTE)pUnit->m_Color.a * fc;
		BYTE r = (BYTE)pUnit->m_Color.r * fc;
		BYTE g = (BYTE)pUnit->m_Color.g * fc;
		BYTE b = (BYTE)pUnit->m_Color.b * fc;
		
        Tri[0].m_dwColor = D3DCOLOR_RGBA( r, g, b, a );
        Tri[1].m_dwColor = D3DCOLOR_RGBA( r, g, b, a );
        Tri[2].m_dwColor = D3DCOLOR_RGBA( r, g, b, a );
        Tri[3].m_dwColor = D3DCOLOR_RGBA( r, g, b, a );
    
        Tri[0].m_v2Tex   = D3DXVECTOR2( 0.0f, 0.0f );
        Tri[1].m_v2Tex   = D3DXVECTOR2( 1.0f, 0.0f );
        Tri[2].m_v2Tex   = D3DXVECTOR2( 1.0f, 1.0f );
        Tri[3].m_v2Tex   = D3DXVECTOR2( 0.0f, 1.0f );

		BYTE Index = pUnit->m_AniIndex;

        m_vecvecVertex[Index].push_back( Tri[0] );
        m_vecvecVertex[Index].push_back( Tri[1] );
        m_vecvecVertex[Index].push_back( Tri[2] );

        m_vecvecVertex[Index].push_back( Tri[0] );
        m_vecvecVertex[Index].push_back( Tri[2] );
        m_vecvecVertex[Index].push_back( Tri[3] );
    }
    else
    if( m_Kind == KIND::CYLINDER )
    {
        PARTICLE_VERTEX Tri[MAX_CYLINDER];

        D3DXMATRIX  WorldM, ScalM, AniRotM, Trans, RotM;
        D3DXMatrixScaling( &ScalM, pUnit->m_v3Scal.x, pUnit->m_v3Scal.y, pUnit->m_v3Scal.z );
        D3DXMatrixRotationYawPitchRoll( &AniRotM, D3DXToRadian(pUnit->m_v3Rot.y), D3DXToRadian(pUnit->m_v3Rot.x), D3DXToRadian(pUnit->m_v3Rot.z) );
		D3DXMatrixRotationYawPitchRoll( &RotM, D3DXToRadian(pUnit->m_v3StartRot.y), D3DXToRadian(pUnit->m_v3StartRot.x), D3DXToRadian(pUnit->m_v3StartRot.z) );
		D3DXMatrixTranslation( &Trans, Pos.x, Pos.y, Pos.z );
        
        WorldM = ScalM  * AniRotM * RotM * Trans;

        if( m_bBillboard )
        {
            D3DXMATRIX InvView;
            
            g_Neuz.m_pd3dDevice->GetTransform( D3DTS_VIEW , &InvView );

            D3DXMatrixInverse( &InvView, NULL, &InvView );
            
            InvView._41 = InvView._42 = InvView._43 = 0.0f;
            
            WorldM *= InvView ;
        }
        
#if 0
        WorldM = WorldM * m_WorldM;
#endif      
		
		FLOAT fc = pUnit->m_Color.a / 255.0f;
		
		BYTE a = (BYTE)pUnit->m_Color.a * fc;
		BYTE r = (BYTE)pUnit->m_Color.r * fc;
		BYTE g = (BYTE)pUnit->m_Color.g * fc;
		BYTE b = (BYTE)pUnit->m_Color.b * fc;

        // Strip º¸°£
        if( m_vecvecVertex[pUnit->m_AniIndex].size() )
        {
            D3DXVec3TransformCoord( &Tri[0].m_v3Pos, &m_TriCylinder[0].m_v3Pos, &WorldM );
            
            Tri[0].m_dwColor = D3DCOLOR_RGBA( r, g, b, a );
            Tri[0].m_v2Tex.x = 0.0f;
            Tri[0].m_v2Tex.y = m_TriCylinder[0].m_v3Pos.z;
            
            m_vecvecVertex[pUnit->m_AniIndex].push_back( Tri[0] );
            m_vecvecVertex[pUnit->m_AniIndex].push_back( Tri[0] );

            D3DXVec3TransformCoord( &Tri[1].m_v3Pos, &m_TriCylinder[1].m_v3Pos, &WorldM );
            
            Tri[1].m_dwColor = D3DCOLOR_RGBA( r, g, b, a );
            Tri[1].m_v2Tex.x = (FLOAT)1 / MAX_CYLINDER;
            Tri[1].m_v2Tex.y = m_TriCylinder[1].m_v3Pos.z;
            
            m_vecvecVertex[pUnit->m_AniIndex].push_back( Tri[1] );
        }
		
        for( int i=0; i<MAX_CYLINDER; i++ )
        {
            D3DXVec3TransformCoord( &Tri[i].m_v3Pos, &m_TriCylinder[i].m_v3Pos, &WorldM );

			Tri[i].m_dwColor = D3DCOLOR_RGBA( r, g, b, a );
            Tri[i].m_v2Tex.x = (FLOAT)i / MAX_CYLINDER;
            Tri[i].m_v2Tex.y = m_TriCylinder[i].m_v3Pos.z;

            m_vecvecVertex[pUnit->m_AniIndex].push_back( Tri[i] );
        }
        
        m_vecvecVertex[pUnit->m_AniIndex].push_back( Tri[MAX_CYLINDER-1] );
    }
    
    return S_OK;
}








CParticle::CParticle()
{
    m_listEmitter.clear();
    m_v3Pos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    m_pVB = NULL;

	D3DXMatrixIdentity( &m_mWorld );
}

CParticle::~CParticle()
{
	Delete();
}

HRESULT	CParticle::Delete()
{
    list< CEmitter* >::iterator iter       =  m_listEmitter.begin();
    list< CEmitter* >::iterator iterEnd    =  m_listEmitter.end();
    
    while( iter != iterEnd )
    {
        CEmitter* pEmitter = *iter;
        SAFE_DELETE( pEmitter );
        iter++;
    }
    
    m_listEmitter.clear();
	
	SAFE_RELEASE( m_pVB );

	return S_OK;
}


HRESULT CParticle::Push_Emitter( CEmitter* pEmitter )
{
    pEmitter->Reset();

	m_listEmitter.push_back( pEmitter );

	return S_OK;
}


HRESULT CParticle::FrameMove( FLOAT fElapsedTime )
{
    list< CEmitter* >::iterator    iter    = m_listEmitter.begin();
    list< CEmitter* >::iterator    iterEnd = m_listEmitter.end();

    CEmitter* pEmitter;

    while( iter != iterEnd )
    {
        pEmitter = *iter;

        if( pEmitter->m_bActive )
		{
			pEmitter->SetWorldMatrix( m_mWorld );
            pEmitter->FrameMove( fElapsedTime );
		}

        iter++;
    }

    return S_OK;
}

HRESULT CParticle::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
    HRESULT hr;
/*
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,   D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,   D3DTA_CURRENT );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE);
    pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );	

	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetVertexDeclaration( NULL );
*/
	
	D3DXMATRIX mWorld;
	D3DXMatrixIdentity( &mWorld );
	pd3dDevice->SetTransform( D3DTS_WORLD, &mWorld );		// Set World Transform 

    list< CEmitter* >::iterator    iterBegin  = m_listEmitter.begin();
    list< CEmitter* >::iterator    iterEnd    = m_listEmitter.end();

    CEmitter* pEmitter;
    
    while( iterBegin != iterEnd )
    {
        pEmitter = *iterBegin;

        if( !pEmitter->m_bActive )
        {
            iterBegin++;
            continue;
        }

        pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  pEmitter->m_dwBlend1+1 );
        pd3dDevice->SetRenderState( D3DRS_DESTBLEND, pEmitter->m_dwBlend2+1 );
        
        for( int i=0; i<pEmitter->m_vecvecVertex.size(); i++ )
        {
            if( i > pEmitter->m_pvecTexture.size() )
                break;
            
            if( pEmitter->m_vecvecVertex[i].size() )
            {
                int  dwSize = pEmitter->m_vecvecVertex[i].size();

                if( pEmitter->m_pvecTexture.size() )
                    hr = pd3dDevice->SetTexture(0, pEmitter->m_pvecTexture[i].m_pTex );
                else
                    hr = pd3dDevice->SetTexture(0, NULL );

                hr = pd3dDevice->CreateVertexBuffer( 
                    sizeof( PARTICLE_VERTEX ) * dwSize,
                    D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
                    PARTICLE_VERTEX::FVF,
                    D3DPOOL_DEFAULT,
                    &m_pVB,
                    NULL
                    );
                
                void *vertex;
                
                hr = m_pVB->Lock( 0, sizeof(PARTICLE_VERTEX)*dwSize,
                    (void**)&vertex, D3DLOCK_DISCARD );
                
                memcpy( vertex, &(pEmitter->m_vecvecVertex[i][0]), sizeof(PARTICLE_VERTEX)*dwSize );
                
                hr = m_pVB->Unlock();
                
                pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(PARTICLE_VERTEX) );
                pd3dDevice->SetFVF( PARTICLE_VERTEX::FVF );

                if( pEmitter->m_Kind == CEmitter::RECTANGLE )
                {
                    if(FAILED(hr = pd3dDevice->DrawPrimitive( 
                        D3DPT_TRIANGLELIST, 0, 2 * (dwSize / 6 ) ) ) )
                        return hr;
                }
                else
                if( pEmitter->m_Kind == CEmitter::CYLINDER )
                {
                    if(FAILED(hr = pd3dDevice->DrawPrimitive( 
                        D3DPT_TRIANGLESTRIP, 0, dwSize-2 ) ) )
                        return hr;
                }
                
                SAFE_RELEASE( m_pVB );

                pEmitter->m_vecvecVertex[i].clear();
            }
        }
        
        iterBegin++;
    }
/*    
	pd3dDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_CCW );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ); 
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
*/	
    return S_OK;
}
    

CEmitter* CParticle::GetEmitter(int index)
{
    list< CEmitter* >::iterator    iter    = m_listEmitter.begin();
    list< CEmitter* >::iterator    iterEnd = m_listEmitter.end();

    CEmitter* pEmitter;
    int Count = 0;

    while( iter != iterEnd )
    {
        pEmitter = *iter++;

        if( Count == index )
            break;

        Count++;
    }

    return pEmitter;
}

void CParticle::DeleteEmitter(int index)
{
    //g_MyDialog->Pause( TRUE );
    
    list< CEmitter* >::iterator    iter    = m_listEmitter.begin();
    list< CEmitter* >::iterator    iterEnd = m_listEmitter.end();
    
    CEmitter* pEmitter;
    int Count = 0;

    while( iter != iterEnd )
    {
        list< CEmitter* >::iterator    iterCurr    = iter++;
        pEmitter = *iterCurr;
        
        if( Count == index )
        {
            SAFE_DELETE( pEmitter );
            m_listEmitter.erase(iterCurr);
            break;
        }
        
        Count++;
    }

    //g_MyDialog->Pause( FALSE );
}

HRESULT CEmitter::Reset()
{
    m_ffCps = 1.0f / m_fCps;
    
    return S_OK;    
}

HRESULT CParticle::ResetAllActive(BOOL bactive)
{
    list< CEmitter* >::iterator    iter    = m_listEmitter.begin();
    list< CEmitter* >::iterator    iterEnd = m_listEmitter.end();
    
    CEmitter* pEmitter;
    
    while( iter != iterEnd )
    {
        pEmitter = *iter++;

        pEmitter->m_bActive = bactive;
    }

    return S_OK;
}

BOOL CParticle::Load(char *FileName)
{
	FILE* fp = fopen( FileName, "rb" );
	
	if( NULL == fp )
		return FALSE;
	
	int EmitterCount;
	PARTICLE_HEADERS Header;
	
	fread( &Header, sizeof(PARTICLE_HEADERS), 1, fp );
	
	if( _stricmp( Header.m_ID, "F4_PARTICLE" ) != 0 )
	{
		return FALSE;
	}
	
	fread( &EmitterCount, sizeof(int), 1, fp );
	
	while( EmitterCount-- )
	{
		CEmitter* pEmitter = new CEmitter;
		
		fread( &(pEmitter->m_bActive),       sizeof(BOOL), 1, fp );
		fread( &(pEmitter->m_bBillboard),    sizeof(BOOL), 1, fp );
		fread( &(pEmitter->m_dwBlend1),      sizeof(DWORD), 1, fp );
		fread( &(pEmitter->m_dwBlend2),      sizeof(DWORD), 1, fp );
		fread( &(pEmitter->m_fCps),          sizeof(FLOAT), 1, fp );
		fread( &(pEmitter->m_ffCps),         sizeof(FLOAT), 1, fp );
		fread( &(pEmitter->m_fTerm),	     sizeof(FLOAT), 1, fp );
		fread( &(pEmitter->m_Kind),          sizeof(BYTE), 1, fp );
		fread( pEmitter->m_pName,            sizeof(TCHAR), 256, fp );
		fread( &(pEmitter->m_v3AccelRot),    sizeof(D3DXVECTOR3), 1, fp );
		fread( &(pEmitter->m_v3Pos),         sizeof(D3DXVECTOR3), 1, fp );
		fread( &(pEmitter->m_v3VelocityRot), sizeof(D3DXVECTOR3), 1, fp );
		
		// Unit
		//*
		DataUnit Unit;
		fread( &Unit, sizeof(DataUnit), 1, fp );
		DataUnitCopy( pEmitter->m_pUnit1, Unit );
		
		fread( &Unit, sizeof(DataUnit), 1, fp );
		DataUnitCopy( pEmitter->m_pUnit2, Unit );
		/**/
		
		// Texture 
		pEmitter->ClearTexture();
		
		int TexCount;
		
		fread( &TexCount, sizeof(int), 1, fp );
		
		while( TexCount-- )
		{
			TCHAR TextureName[2048];
			
			fread( TextureName, sizeof(TCHAR), 2048, fp );
			
			pEmitter->SetTexture( g_Neuz.m_pd3dDevice, NULL, TextureName );
		}
		
		Push_Emitter( pEmitter );
	}
	
	fclose(fp);

	return TRUE;
}


void DataUnitCopy( CUnit* pUnit, DataUnit src )
{
    pUnit->m_v3Pos = src.m_v3Pos;
    pUnit->m_v3Velocity = src.m_v3Velocity;
    pUnit->m_v3Accel = src.m_v3Accel;
    pUnit->m_v3Gravity = src.m_v3Gravity;
    pUnit->m_v3Scal = src.m_v3Scal;
    pUnit->m_v3Rot = src.m_v3Rot;
    pUnit->m_v3StartRot = src.m_v3StartRot;
    pUnit->m_Color = src.m_Color;
    pUnit->m_fLife = src.m_fLife;
    pUnit->m_fCurrTime = src.m_fCurrTime;
    pUnit->m_fAniTime = src.m_fAniTime;
    pUnit->m_fCurrAniTime = src.m_fCurrAniTime;
    pUnit->m_AniIndex = src.m_AniIndex;
}

void UnitCopy( CUnit* pUnit, DataUnit& src )
{
    src.m_v3Pos = pUnit->m_v3Pos;
    src.m_v3Velocity = pUnit->m_v3Velocity;
    src.m_v3Accel = pUnit->m_v3Accel;
    src.m_v3Gravity = pUnit->m_v3Gravity;
    src.m_v3Scal = pUnit->m_v3Scal;
    src.m_v3Rot = pUnit->m_v3Rot;
    src.m_v3StartRot = pUnit->m_v3StartRot;
    src.m_Color = pUnit->m_Color;
    src.m_fLife = pUnit->m_fLife;
    src.m_fCurrTime = pUnit->m_fCurrTime;
    src.m_fAniTime = pUnit->m_fAniTime;
    src.m_fCurrAniTime = pUnit->m_fCurrAniTime;
    src.m_AniIndex = pUnit->m_AniIndex;
}






HRESULT CParticleExMng::FrameMove( FLOAT fElapsedTime )
{
    list< CParticle* >::iterator    iter    = m_listParticle.begin();
    list< CParticle* >::iterator    iterEnd = m_listParticle.end();
	
    CParticle* pParticle;
	
    while( iter != iterEnd )
    {
        pParticle = *iter;
		
        if( pParticle )
		{
            pParticle->FrameMove( fElapsedTime );
		}
		
        iter++;
    }
	
    return S_OK;
}

CParticleExMng::CParticleExMng()
{
	m_listParticle.clear();
}
CParticleExMng::~CParticleExMng()
{
	Delete();
}

CParticle*	CParticleExMng::AddParticle		( char* FileName )
{
	CParticle* Particle = new CParticle;

	if( Particle->Load( FileName ) )
	{
		m_listParticle.push_back( Particle );
		return Particle;
	}

	return NULL;
}

HRESULT	CParticleExMng::Delete()
{
    list< CParticle* >::iterator    iter    = m_listParticle.begin();
    list< CParticle* >::iterator    iterEnd = m_listParticle.end();
    
    while( iter != iterEnd )
    {
        CParticle* pParticle = *iter;

		if( pParticle )
		{
			pParticle->Delete();
			SAFE_DELETE( pParticle );
		}

        iter++;
    }
    
    m_listParticle.clear();
	
	return S_OK;
}

HRESULT	CParticleExMng::Restore( LPDIRECT3DDEVICE9 pDevice )
{
/*
    list< CParticle* >::iterator    iter    = m_listParticle.begin();
    list< CParticle* >::iterator    iterEnd = m_listParticle.end();
    
    while( iter != iterEnd )
    {
        CParticle* pParticle = *iter;
		if( pParticle )
			pParticle->Restore(pDevice);
        iter++;
    }
*/    
	return S_OK;
}

HRESULT	CParticleExMng::Invalidate()
{
/*	
    list< CParticle* >::iterator    iter    = m_listParticle.begin();
    list< CParticle* >::iterator    iterEnd = m_listParticle.end();
    
    while( iter != iterEnd )
    {
        CParticle* pParticle = *iter;
		if( pParticle )
			pParticle->Invalidate();
        iter++;
    }
*/    
	return S_OK;
}

HRESULT CParticleExMng::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
//    HRESULT hr;

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,   D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,   D3DTA_CURRENT );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE);
    pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );	

	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetVertexDeclaration( NULL );
	
    list< CParticle* >::iterator    iterBegin  = m_listParticle.begin();
    list< CParticle* >::iterator    iterEnd    = m_listParticle.end();

    CParticle* pParticle;
    
    while( iterBegin != iterEnd )
    {
        pParticle = *iterBegin;

        if( pParticle )
        {
			pParticle->Render( pd3dDevice );
        }

		iterBegin++;
    }
    
	pd3dDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_CCW );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ); 
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	
    return S_OK;
}
#endif 