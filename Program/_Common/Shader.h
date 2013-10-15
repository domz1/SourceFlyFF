#ifndef __SHADER_H__
#define __SHADER_H__

class CShader
{
	void Init( void );
	void Destroy( void );
public:
	LPD3DXEFFECT	m_pEffect;
	LPDIRECT3DVERTEXDECLARATION9	m_pDecl;
	
	CShader();
	~CShader();
	
	HRESULT Begin( UINT *uPasses, DWORD dwFlag );
	HRESULT End( void );
	HRESULT Pass( UINT nPass );

	HRESULT SetFloat( D3DXHANDLE hParameter, FLOAT f ) { return m_pEffect->SetFloat( hParameter, f ); }
	
	
	HRESULT		LoadEffect( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName );
	void		DeleteDeviceObjects( void );
		
	
};

#endif // Shader.h