#include "stdafx.h"
#include "Shader.h"



CShader::CShader()
{
	Init();
}

CShader::~CShader()
{
	Destroy();
}

void CShader::Init( void )
{
	m_pEffect = NULL;
}

void CShader::Destroy( void )
{
	// 이곳에 파괴 코드를 넣으시오.
	DeleteDeviceObjects();
	Init();
}

void	CShader::DeleteDeviceObjects( void )
{
	SAFE_RELEASE( m_pEffect );
}

HRESULT CShader::LoadEffect( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName )
{
    HRESULT hr;
    LPD3DXEFFECT pEffect;

	D3DVERTEXELEMENT9	decl[MAX_FVF_DECL_SIZE];

	D3DXDeclaratorFromFVF( D3DFVF_XYZRHW | D3DFVF_TEX1, decl );
	pd3dDevice->CreateVertexDeclaration( decl, &m_pDecl );
	
#ifdef _DEBUG
    if(FAILED(hr = D3DXCreateEffectFromFile( D3DDEVICE, szFileName, NULL, NULL, 0, NULL, &pEffect, NULL )))
	{
		Error( "CShader::LoadEffect : %s 생성 실패", szFileName );
        return hr;
	}
#else

	if(FAILED(hr = D3DXCreateEffectFromResource( D3DDEVICE, NULL, MAKEINTRESOURCE(IDR_DATA2), NULL, NULL, 0, NULL, &pEffect, NULL )))
	{
		Error( "CShader::LoadEffect : %s 생성 실패", szFileName );
		return hr;
	}
#endif

	m_pEffect = pEffect;
	
    D3DXHANDLE hTechnique;
    if( FAILED(hr = m_pEffect->FindNextValidTechnique(NULL, &hTechnique)) )
	{
		Error( "CShader::LoadEffect : %s Not Valid", szFileName );
        return hr;
	}
	
	m_pEffect->SetTechnique( hTechnique );
//	m_pEffect->SetTexture( "DiffuseTexture", m_pTexture0 );

	return S_OK;
}

HRESULT CShader::Begin( UINT *uPasses, DWORD dwFlag )
{
	HRESULT hr = m_pEffect->Begin( uPasses, dwFlag );
    return hr;
}

HRESULT CShader::End( void )
{
    return m_pEffect->End();
}

HRESULT CShader::Pass( UINT nPass )
{
	return m_pEffect->Pass( nPass );
}

