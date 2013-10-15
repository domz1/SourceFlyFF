#ifndef __PARTICLEMNG_H__
#define __PARTICLEMNG_H__

struct POINTVERTEX
{
    D3DXVECTOR3 v;
    D3DCOLOR    color;
	
    static const DWORD FVF;
};

struct PARTICLE
{
    D3DXVECTOR3 m_vPos;       // Current position
    D3DXVECTOR3 m_vVel;       // Current velocity
	
    D3DXCOLOR   m_clrDiffuse; // Initial diffuse color
    D3DXCOLOR   m_clrFade;    // Faded diffuse color
    FLOAT       m_fFade;      // Fade progression
	FLOAT		m_fGroundY;	
	
    PARTICLE*   m_pNext;      // Next particle in list
};

class CParticles
{
	PARTICLE	*m_pPool;
	int			m_nPoolPtr;
protected:
//    FLOAT     m_fRadius;
	int		m_nType;
    DWORD	m_dwBase;
	DWORD	m_dwFlush;
    DWORD	m_dwDiscard;
//	float	m_fTime;
	
    DWORD     m_dwParticles;
    DWORD     m_dwParticlesLim;
    PARTICLE* m_pParticles;
    PARTICLE* m_pParticlesFree;
	
    // Geometry
    LPDIRECT3DVERTEXBUFFER9 m_pVB;

	void Init( void );
	void Destroy( void );
	
public:
	BOOL	m_bActive;
	BOOL	m_bGravity;
	FLOAT	m_fSize;
	LPDIRECT3DTEXTURE9 m_pParticleTexture;
	
	CParticles();
    CParticles( DWORD dwFlush, DWORD dwDiscard, int nType );
	~CParticles();

	void Create( DWORD dwFlush, DWORD dwDiscard, int nType );
	
	HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName );
//	HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
    HRESULT InvalidateDeviceObjects();
	
	HRESULT CreateParticle( int nType, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vVel, FLOAT fGroundY );
	HRESULT Update( void );
	HRESULT Update2( void );
		
    HRESULT Render( LPDIRECT3DDEVICE9 pd3dDevice );
};

#define		MAX_PARTICLE_TYPE	32		// 최대 파티클 종류.

class CParticleMng
{
	LPDIRECT3DDEVICE9 m_pd3dDevice;
	int		m_nMaxType;
	CParticles m_Particles[ MAX_PARTICLE_TYPE ];

	void	Init( void );
	void	Destroy( void );
public:
	BOOL	m_bActive;

	CParticleMng();
	~CParticleMng();

	HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice ) { m_pd3dDevice = pd3dDevice; return S_OK; }
	HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
    HRESULT InvalidateDeviceObjects();

	CParticles *CreateParticle( int nType, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vVel, FLOAT fGroundY );
		
	void	Process( void );
	void	Render( LPDIRECT3DDEVICE9 pd3dDevice );
	
};

extern D3DXCOLOR g_clrColor;
extern DWORD g_clrColorFade;

//extern CParticles		g_Particle;
//extern CParticles		g_Particle2;

extern CParticleMng	g_ParticleMng;

#endif // PARTICLE
