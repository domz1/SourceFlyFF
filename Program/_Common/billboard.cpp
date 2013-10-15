// Billboard.cpp : implementation of the CBeastView class
//

#include "stdafx.h"
#include "billboard.h"


struct BILLBOARDVERTEX
{
	D3DXVECTOR3 p; 
	//D3DXVECTOR3 n;
	DWORD   color;
	FLOAT u, v;
};
//#define D3DFVF_BILLBOARDVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1) 
#define D3DFVF_BILLBOARDVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1) 

CBillboard::CBillboard()
{
	m_pVertexBuffer = NULL;
	m_nVertexNum = 0;
	m_nModelType = MODELTYPE_BILLBOARD;
	m_pBillboard = NULL;
	m_pTexture = NULL;
	m_fScale = 1.0f;
}
CBillboard::~CBillboard()
{
	DeleteDeviceObjects();
}
HRESULT CBillboard::RestoreDeviceObjects()
{
	HRESULT hr;
#ifdef	__WORLDSERVER
	if( m_pd3dDevice == NULL )	
		return S_OK;
#endif
	if( m_pVertexBuffer )
		return S_OK;
	m_nVertexNum = 4;
	// 버텍스 버퍼 만들기 
	if( m_pBillboard->bAnimation )
	{
		hr = m_pd3dDevice->CreateVertexBuffer
			(	
				m_nVertexNum * sizeof(BILLBOARDVERTEX),
				D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, D3DFVF_BILLBOARDVERTEX,
				D3DPOOL_SYSTEMMEM, &m_pVertexBuffer, NULL );
	}
	else
	{
		hr = m_pd3dDevice->CreateVertexBuffer
			(	
				m_nVertexNum * sizeof(BILLBOARDVERTEX),
				0, D3DFVF_BILLBOARDVERTEX,
				D3DPOOL_MANAGED, &m_pVertexBuffer ,
				NULL
			);
	}
	if( FAILED(hr) ) return hr;

	BILLBOARDVERTEX* pVertices;

	hr = m_pVertexBuffer->Lock( 0, m_nVertexNum * sizeof(BILLBOARDVERTEX), (void**)&pVertices, 0 );
	if( FAILED(hr) ) return hr;

	// size.cx : v1 = 1 : x

	D3DXVECTOR3 v1;
	D3DXVECTOR3 v2;
 	D3DXVECTOR3 v3;
	D3DXVECTOR3 v4;

	D3DXVECTOR3 vCenter;
	

	CRect rect = m_pBillboard->rect;
	CPoint ptCenter = m_pBillboard->ptCenter;
	rect.OffsetRect( -m_pBillboard->rect.TopLeft() );
	ptCenter -= m_pBillboard->rect.TopLeft();


	FLOAT fxsize;
	FLOAT fysize;

	if( m_pTexture->m_size.cx > m_pTexture->m_size.cy )
	{
		fxsize = (float)( m_pTexture->m_size.cx );
		fysize = (float)( m_pTexture->m_size.cx );
	}
	else
	{
		fxsize = (float)( m_pTexture->m_size.cy );
		fysize = (float)( m_pTexture->m_size.cy );
	}

	v1.x = 0;//(FLOAT)rect.left / (FLOAT)m_pTexture->m_size.cx;
	v1.y = 0;//(FLOAT)rect.top  / (FLOAT)m_pTexture->m_size.cy;
	v1.z = 0;
	v2.x = (FLOAT)rect.right / fxsize;
	v2.y = 0;//(FLOAT)rect.top / (FLOAT)m_pTexture->m_size.cy;
	v2.z = 0;
	v3.x = 0;//(FLOAT)rect.left / (FLOAT)m_pTexture->m_size.cx;
	v3.y = -( (FLOAT)rect.bottom / fysize);
	v3.z = 0;
	v4.x = (FLOAT)rect.right / fxsize;
	v4.y = -( (FLOAT)rect.bottom / fysize );
	v4.z = 0;

	vCenter.x = (FLOAT)ptCenter.x / fxsize;
	vCenter.y = -( (FLOAT)ptCenter.y / fysize );
	vCenter.z = 0;

	v1.x *= m_fScale;
	v1.y *= m_fScale;
	v1.z *= m_fScale;
	v2.x *= m_fScale;
	v2.y *= m_fScale;
	v2.z *= m_fScale;
	v3.x *= m_fScale;
	v3.y *= m_fScale;
	v3.z *= m_fScale;
	v4.x *= m_fScale;
	v4.y *= m_fScale;
	v4.z *= m_fScale;

	vCenter.x *= m_fScale;
	vCenter.y *= m_fScale;
	vCenter.z *= m_fScale;

	v1.x -= vCenter.x;
	v1.y -= vCenter.y;
	v1.z -= vCenter.z;
	v2.x -= vCenter.x;
	v2.y -= vCenter.y;
	v2.z -= vCenter.z;
	v3.x -= vCenter.x;
	v3.y -= vCenter.y;
	v3.z -= vCenter.z;
	v4.x -= vCenter.x;
	v4.y -= vCenter.y;
	v4.z -= vCenter.z;
	
	FLOAT fU1 = (FLOAT)m_pBillboard->rect.left / (FLOAT)m_pTexture->m_size.cx;
	FLOAT fV1 = (FLOAT)m_pBillboard->rect.top / (FLOAT)m_pTexture->m_size.cy;
	FLOAT fU2 = (FLOAT)m_pBillboard->rect.right / (FLOAT)m_pTexture->m_size.cx;
	FLOAT fV2 = (FLOAT)m_pBillboard->rect.bottom / (FLOAT)m_pTexture->m_size.cy;

	pVertices->p = v1;
	//pVertices->n = D3DXVECTOR3(0,1,0);//D3DXVECTOR3(0,0,-1);
	pVertices->u = fU1;
	pVertices->v = fV1;
	pVertices->color = 0x7fffffff;
	pVertices++;
	pVertices->p = v2;
	//pVertices->n = D3DXVECTOR3(0,1,0);//D3DXVECTOR3(0,0,-1);
	pVertices->u = fU2;
	pVertices->v = fV1;
	pVertices->color = 0x7fffffff;
	pVertices++;
	pVertices->p = v3;
	//pVertices->n = D3DXVECTOR3(0,1,0);//D3DXVECTOR3(0,0,-1);
	pVertices->u = fU1;
	pVertices->v = fV2;
	pVertices->color = 0x7fffffff;
	pVertices++;
	pVertices->p = v4;
	//pVertices->n = D3DXVECTOR3(0,1,0);//D3DXVECTOR3(0,0,-1);
	pVertices->u = fU2;
	pVertices->v = fV2;
	pVertices->color = 0x7fffffff;
	pVertices++;

	m_pVertexBuffer->Unlock();
	// BoundBox의 Vertex순서 
	//     - z
	//   3 | 2
	// - --+-- + x
	//   0 | 1
	//
	//   7 | 6
	// - --+-- + x
	//   4 | 5
	m_BB.m_vPos[0] = v1;
	m_BB.m_vPos[1] = v2;
	m_BB.m_vPos[4] = v3;
	m_BB.m_vPos[5] = v4;
	m_BB.m_vPos[3] = v1;
	m_BB.m_vPos[2] = v2;
	m_BB.m_vPos[7] = v3;
	m_BB.m_vPos[6] = v4;

	if( m_pBillboard->bEffect )
		m_dwRenderEffect = REFF_SFX;

	// 애니메이션 빌보드이면 총 몇플레임인지 계산한다.
	if( m_pBillboard->bAnimation  )
	{
		int nWidth = m_pTexture->m_size.cx / rect.Width();
		int nHeight = m_pTexture->m_size.cy / rect.Height();
		m_nFrameMax = nWidth * nHeight;
	}
	return hr;
}
HRESULT CBillboard::InvalidateDeviceObjects()
{
	HRESULT hr = S_OK;
	SAFE_RELEASE(m_pVertexBuffer);
	return hr;
}
HRESULT CBillboard::DeleteDeviceObjects()
{
	HRESULT hr = S_OK;
	SAFE_RELEASE(m_pVertexBuffer);
	return hr;
}
HRESULT CBillboard::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice, LPBILLBOARD lpBillboard, CTexture* pTexture )
{
	HRESULT hr = S_OK;
	m_pd3dDevice = pd3dDevice;
	m_pBillboard = lpBillboard;
	m_pTexture = pTexture;
	return hr;
}
BOOL CBillboard::Render( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX* pmatWorld )
{
	if( m_pBillboard->bAnimation )
	{
	//f( (int)pAnimation->m_fFrameOld != (int)pAnimation->m_fFrameCurrent )
		{
			CRect rect = m_pBillboard->rect;
			rect.OffsetRect( -m_pBillboard->rect.TopLeft() );

			int nWidth = m_pTexture->m_size.cx / rect.Width();
			int nHeight = m_pTexture->m_size.cy / rect.Height();

			int nDiv = (int)m_fFrameCurrent / nWidth;
			int nRemain = (int)m_fFrameCurrent % nWidth;

			int x = nRemain * rect.Width();
			int y = nDiv * rect.Height();

			rect.OffsetRect( x, y );

			FLOAT fU1 = (FLOAT)rect.left / (FLOAT)m_pTexture->m_size.cx;
			FLOAT fV1 = (FLOAT)rect.top / (FLOAT)m_pTexture->m_size.cy;
			FLOAT fU2 = (FLOAT)rect.right / (FLOAT)m_pTexture->m_size.cx;
			FLOAT fV2 = (FLOAT)rect.bottom / (FLOAT)m_pTexture->m_size.cy;
			BILLBOARDVERTEX* pVertices;

			m_pVertexBuffer->Lock( 0, m_nVertexNum * sizeof(BILLBOARDVERTEX), (void**)&pVertices, 0 );
			//if( FAILED(hr) ) return hr;

			//pVertices->p = v1;
			pVertices->u = fU1;
			pVertices->v = fV1;
			pVertices++;
			//pVertices->p = v2;
			pVertices->u = fU2;
			pVertices->v = fV1;
			pVertices++;
			//pVertices->p = v3;
			pVertices->u = fU1;
			pVertices->v = fV2;
			pVertices++;
			//pVertices->p = v4;
			pVertices->u = fU2;
			pVertices->v = fV2;
			pVertices++;

			m_pVertexBuffer->Unlock();
		}
	}
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );

	//pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );

	// Set diffuse blending for alpha set in vertices.

	if( m_pBillboard->bEffect )
	{
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	}
	else
	{
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	}
//	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
//	pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x08 );
//	pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
/*

	D3DMATERIAL9 material;	
	ZeroMemory( &material, sizeof(material) );
	material.Diffuse.r = 1.0f;
	material.Diffuse.g = 1.0f;
	material.Diffuse.b = 1.0f;
	material.Diffuse.a = 1.0f;
	material.Ambient.r  = 1.0f;
	material.Ambient.g  = 1.0f;
	material.Ambient.b  = 1.0f;
	material.Ambient.a  = 1.0f;
	material.Specular.r  = 1.0f;
	material.Specular.g  = 1.0f;
	material.Specular.b  = 1.0f;
	material.Specular.a  = 1.0f;
	material.Power = 50.0f;   
	pd3dDevice->SetMaterial( &material );
	*/
	// 오브젝트의 반투명 효과 세팅 
	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( m_dwBlendFactor, 0, 0, 0) );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );
	if( m_dwBlendFactor < 255 )
	{
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

//		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
//		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );
//		pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
//		pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_CURRENT );
//		pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );

		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( m_dwBlendFactor, 0, 0, 0) );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );

	}
	else
	{
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	}
	pd3dDevice->SetTexture( 0, m_pTexture->m_pTexture );
	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetFVF( D3DFVF_BILLBOARDVERTEX );
	pd3dDevice->SetStreamSource( 0, m_pVertexBuffer, 0,sizeof( BILLBOARDVERTEX ) );
	pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, m_nVertexNum / 2 );

	return TRUE;
}

//////////////////////

