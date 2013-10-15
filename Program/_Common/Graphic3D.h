#ifndef	__GRAPHIC3D_H__
#define	__GRAPHIC3D_H__


#define D3DFVF_3DVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)

struct FVF_3DVERTEX
{
    D3DXVECTOR3 vPos;	// The 3D position for the vertex
    DWORD		dwColor;		// The vertex color
	DWORD       dwColorV19;
};

#define		MAX_3DVERTEX		1024

class CGraphic3D
{
	LPDIRECT3DDEVICE9 m_pd3dDevice;
	FVF_3DVERTEX	m_aVertex[ MAX_3DVERTEX ];

public:
	CGraphic3D() { Init(); }
	~CGraphic3D() { Destroy(); }

	void	SetDevice( LPDIRECT3DDEVICE9 d3dDevice ) { m_pd3dDevice = d3dDevice; }

	void	Init( void );
	void	Destroy( void );

	void	RenderPoint( D3DXVECTOR3 &v, DWORD dwColor );
	void	Render3DLine( D3DXVECTOR3 &vOrig, D3DXVECTOR3 &vTar, DWORD dwColor = 0xffffffff );//adeilson
	void	Render3DTri( D3DXVECTOR3 *vList, DWORD dwColor = 0xffffffff, BOOL bShowNormal = FALSE );
	void	RenderAABB( D3DXVECTOR3 &vMin, D3DXVECTOR3 &vMax, DWORD dwColor );
	void	RenderGrid( DWORD dwColor );

};

extern CGraphic3D	g_Grp3D;



#endif

