#include "stdafx.h"

#include <stdio.h>
#include "modelSfx.h"

#if 0
using namespace std ;

CSfxObj::CSfxObj() 
{ 
	m_bDelete = FALSE; 
	m_pModel = NULL; 
	m_fAngle = 0.0f; 
	m_vScale = D3DXVECTOR3( 1.0f, 1.0f, 1.0f ); 
}
CSfxObj::~CSfxObj() 
{ 
}
CModel* CSfxObj::LoadModel( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwType, DWORD dwIndex ) 
{
	return ( m_pModel = prj.m_modelMng.LoadModel( pd3dDevice, dwType, dwIndex ) );
}
void CSfxObj::Process() 
{
}
#ifdef __CLIENT
void CSfxObj::Render( LPDIRECT3DDEVICE9 pd3dDevice ) 
{
}
#endif	// __WORLDSERVER
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////
///////////
///////////
///////////
CSfxParticleMng::CSfxParticleMng()
{
}
CSfxParticleMng::~CSfxParticleMng()
{
}
void CSfxParticleMng::Process()
{
	list<CSfxObj*>::iterator i;
	for( i = m_listParticle.begin(); i != m_listParticle.end(); ++i )
	{
		(*i)->Process();
		if( (*i)->m_bDelete )
		{
			safe_delete( *i );
			m_listParticle.erase( i-- );
		}

	}
}

#ifdef __CLIENT
void CSfxParticleMng::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	list<CSfxObj*>::iterator i;
	for( i = m_listParticle.begin(); i != m_listParticle.end(); ++i )
	{
		(*i)->Render( pd3dDevice );

	}
}
#endif	// __WORLDSERVER

CSfxJetParticleMng::CSfxJetParticleMng()
{
}
CSfxJetParticleMng::~CSfxJetParticleMng()
{
}
void CSfxJetParticleMng::GenerateParticle( D3DXVECTOR3* pvPos )
{
#ifdef __CLIENT
	D3DXVECTOR3 vPos = *pvPos;
	DWORD nAngle=rand(); FLOAT fSpeed=(float)((rand()%(m_nMaxSpeed+1))+m_nMinSpeed)/10000.0f; FLOAT fSpeedY=(float)((rand()%(m_nMaxYSpeed+1))+m_nMinYSpeed)/10000.0f;
	FLOAT dx=sin(nAngle)*fSpeed;
	FLOAT dz=cos(nAngle)*fSpeed;
	float fRadius=(float)((rand()%(m_nMaxRadius+1))+m_nMinRadius)/10000.0f;
	vPos +=D3DXVECTOR3(sin(nAngle)*fRadius,.0f,cos(nAngle)*fRadius);
	CSfxParticle* pObj = new CSfxParticle(m_nAppear,m_nKeep,m_nDisappear,vPos,D3DXVECTOR3(dx,fSpeedY,dz),PARTICLEACCELTYPE_VECTOR,D3DXVECTOR3(0,m_fGravity,0),0);
	pObj->m_vScale = m_vScale;
	pObj->m_pModel = prj.m_modelMng.LoadModel( D3DDEVICE, OT_SFX, m_nResource);//XI_DMG_EFFECT03 );
	pObj->m_nEffect=m_nEffect;
	m_listParticle.insert( m_listParticle.end(), pObj );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////
///////////
///////////
///////////

CSfxParticle::CSfxParticle(WORD nAppear,WORD nKeep,WORD nDisappear,D3DXVECTOR3 vPos,D3DXVECTOR3 vDirection,PARTICLEACCELTYPE nAccelType,D3DXVECTOR3 vAccel,FLOAT fAccel)
{
	m_nFrame=0;
	m_nAppear=nAppear;
	m_nKeep=nKeep;
	m_nDisappear=nDisappear;

	m_vPos = vPos;
	m_vDirection=vDirection;

	m_nAccelType=nAccelType;
	m_vAccel=vAccel;
	m_fAccel=fAccel;
	m_nEffect=0;
}
CSfxParticle::~CSfxParticle()
{
}
void CSfxParticle::Process()
{
	DWORD nBlendFactor=255;
	if(m_nFrame<m_nAppear)
		nBlendFactor=255*m_nFrame/m_nAppear;
	else if(m_nFrame<m_nKeep)
		nBlendFactor=255;
	else if(m_nFrame<m_nDisappear)
		nBlendFactor=255-255*(m_nFrame-m_nKeep)/(m_nDisappear-m_nKeep);
	else
		m_bDelete = TRUE;
	//(	m_nFrame > 60 ) m_bDelete = TRUE;
	if(nBlendFactor<255)
	{
		nBlendFactor=nBlendFactor;
	}
	m_pModel->SetBlendFactor(nBlendFactor);

	m_vPos +=	m_vDirection;
	switch(m_nAccelType) {
		case PARTICLEACCELTYPE_VECTOR:
			{
				m_vDirection+=m_vAccel;
			}
			break;
		case PARTICLEACCELTYPE_FACTOR:
			{
				m_vDirection*=m_fAccel;
			}
			break;
	}
	m_nFrame++;
}
#ifdef __CLIENT
void CSfxParticle::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	CModel* pModel = m_pModel;
	DWORD dwModelType = pModel->GetModelType();
	//m_vScale = D3DXVECTOR3( 3.0f, 13.0f, 13.0f );
	D3DXMATRIX matTrans;
	D3DXMATRIX matScale;
	D3DXMATRIX matRotation;
	D3DXMatrixScaling( &matScale, m_vScale.x, m_vScale.y, m_vScale.z );
	D3DXMatrixRotationY( &matRotation, D3DXToRadian( -m_fAngle ) );
	D3DXMatrixTranslation( &matTrans, m_vPos.x, m_vPos.y, m_vPos.z);

	static FLOAT m_fRadian = 0;
	static FLOAT m_fScane = 0;
	D3DXMATRIX matWorld   ;
/*
	D3DXVECTOR3 vDir = (g_WorldMng()->m_pCamera->m_vLookAt) - (g_WorldMng()->m_pCamera->m_vPos2);
	D3DXVec3Normalize( &vDir, &vDir );
	FLOAT y = ( vDir.x > 0.0f ) ? -atanf( vDir.z / vDir.x) + D3DX_PI / 2 : -atanf( vDir.z / vDir.x) - D3DX_PI / 2; // y
	FLOAT x =  -asinf( vDir.y);
	D3DXMatrixRotationYawPitchRoll( &matRotation, y, x, m_fRadian	);
*/

//	D3DXMatrixRotationZ(&matRotation,D3DXToRadian(45.0f));
//	D3DXMatrixRotationZ(&matRotation,0);
//	matRotation*=g_WorldMng()->m_pCamera->m_matInvView;
	matRotation=g_WorldMng()->m_pCamera->m_matInvView;

	m_fRadian += 0.05f;
	if( m_fRadian > D3DX_PI * 2 ) 
		m_fRadian -= D3DX_PI * 2;
	//D3DXVECTOR3 vScale( m_fScane, m_fScane, m_fScane );
/*
	D3DXMatrixIdentity( &matWorld );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matScale );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matRotation );
	D3DXMatrixMultiply( &matWorld, &matWorld, &(g_WorldMng()->m_pCamera->m_matInvView) );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matTrans );
*/
	matWorld=matScale*matRotation*matTrans;
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	m_nEffect=0;
	m_pModel->SetFrame( m_nEffect );
	m_pModel->Render( pd3dDevice, NULL ); 
}
#endif

////////////////////////////////////////////////////////////////////////////////
// ¸Å½¬ 

CSfxMesh::CSfxMesh()
{
}
CSfxMesh::~CSfxMesh()
{
}
void CSfxMesh::Process()
{
	m_pModel->FrameMove();
}
#ifdef __CLIENT
void CSfxMesh::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	DWORD dwModelType = m_pModel->GetModelType();
	if( dwModelType == MODELTYPE_MESH || dwModelType == MODELTYPE_MESH )
	{
		((CModelObject*)m_pModel)->m_nNoEffect = TRUE;
	}
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

	// Set diffuse blending for alpha set in vertices.

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCCOLOR );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	CModel* pModel = m_pModel;
	//m_vScale = D3DXVECTOR3( 10.0f, 10.0f, 10.0f );
	D3DXMATRIX matWorld;
	D3DXMATRIX matTrans;
	D3DXMATRIX matScale;
	D3DXMATRIX matRotation;
	D3DXMatrixScaling( &matScale, m_vScale.x, m_vScale.y, m_vScale.z );
	D3DXMatrixRotationY( &matRotation, D3DXToRadian( -m_fAngle ) );
	D3DXMatrixTranslation( &matTrans, m_vPos.x, m_vPos.y, m_vPos.z);

	//D3DXVECTOR3 vScale( m_fScane, m_fScane, m_fScane );
	D3DXMatrixIdentity( &matWorld );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matScale );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matRotation );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matTrans );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	//m_nEffect=0;
	m_pModel->SetFrame( 0 );
	m_pModel->Render( pd3dDevice, &matWorld );
}
#endif
////////////////////////////////////////////////////////////////////////////////
// ModelSfx
//

int CModelSfx::LoadModel( LPCTSTR szFileName )
{
	return 1;
}
CModelSfx::CModelSfx()
{
}
CModelSfx::~CModelSfx()
{
}
/*
void CModelSfx::GenerateParticle( D3DXVECTOR3* pvPos ) 
{
	D3DXVECTOR3 vPos = *pvPos;
	DWORD nAngle=rand(); FLOAT fSpeed=((float)(rand()%100)+100.0f)/2000.0f; FLOAT fSpeedY=((float)(rand()%100)+200.0f)/2000.0f;
	FLOAT dx=sin(nAngle)*fSpeed;
	FLOAT dz=cos(nAngle)*fSpeed;
	CSfxParticle* pObj = new CSfxParticle(0,30,60,vPos,D3DXVECTOR3(dx,fSpeedY,dz),PARTICLEACCELTYPE_VECTOR,D3DXVECTOR3(0,-0.005f,0),0);
	pObj->m_vScale = D3DXVECTOR3(3.5f,3.5f,3.5f);
	pObj->m_pModel = prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_SFX, XI_DMG_EFFECT03 );
	m_listParticle.insert( m_listParticle.end(), pObj );
}*/
void CModelSfx::AddSfxObj( CSfxObj* pSfxObj )
{
	m_listSfx.insert( m_listSfx.end(), pSfxObj );
}
void CModelSfx::FrameMove( D3DXVECTOR3 *pvSndPos )
{

	list<CSfxObj*>::iterator i;
	for( i = m_listSfx.begin(); i != m_listSfx.end(); ++i )
	{
		(*i)->Process();
		if( (*i)->m_bDelete )
		{
			safe_delete( *i );
			m_listSfx.erase( i-- );
		}
	}
}
#ifdef __CLIENT
BOOL CModelSfx::Render( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX* pmWorld )
{
///	for( int i = 0; i < m_nNumParticle; i++ )
//	{
//		m_apSfxParticle[ i ]->Render( pd3dDevice );
//	} 

	list<CSfxObj*>::iterator i;
	for( i = m_listSfx.begin(); i != m_listSfx.end(); ++i )
	{
		(*i)->Render( pd3dDevice );

	}
	return TRUE;
	/*

   LISTINT listInt;
    LISTINT listAnother;
    LISTINT::iterator i;

    // Insert one at a time
    listInt.insert (listInt.begin(), 2);
    listInt.insert (listInt.begin(), 1);
    listInt.insert (listInt.end(), 3);

    // 1 2 3
    for (i = listInt.begin(); i != listInt.end(); ++i)
        cout << *i << " ";
    cout << endl;
*/
}
#endif	// __WORLDSERVER
#endif