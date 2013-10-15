#include "stdafx.h"


DWORD  aaa = 100;
CLight::CLight()
{
	m_dwIndex = 0;
	m_dwIndex = aaa;
	aaa++;
}
CLight::CLight( LPCTSTR lpszKey, D3DLIGHTTYPE ltType, FLOAT x, FLOAT y, FLOAT z )
{
	m_strKey = lpszKey;
	D3DUtil_InitLight( *this, ltType, x, y, z );
	m_dwIndex = aaa;
	aaa++;
}
void CLight::SetLight( LPCTSTR lpszKey, D3DLIGHTTYPE ltType, FLOAT x, FLOAT y, FLOAT z )
{
	m_strKey = lpszKey;
	D3DUtil_InitLight( *this, ltType, x, y, z );
	//m_dwIndex = aaa;
}
void CLight::Appear( LPDIRECT3DDEVICE9 pd3dDevice, BOOL bEnable )
{
	if( bEnable )
	{
		if( D3D_OK != pd3dDevice->SetLight( m_dwIndex, this ) )
		{
			ASSERT( 0 );
		}
	}
	if( D3D_OK != pd3dDevice->LightEnable( m_dwIndex, bEnable ) )
	{
		ASSERT( 0 );
	}
}

