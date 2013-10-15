#ifndef		__MODELGLOBAL_H__
#define		__MODELGLOBAL_H__

#define XE_NONE				0
#define XE_REFLECT			(0x00000001)
#define XE_OPACITY			(0x00000002)
#define XE_2SIDE			(0x00000004)
#define XE_SELF_ILLUMINATE	(0x00000008)
#define XE_SPECULAR			(0x00000010)
#define XE_BUMP				(0x00000020)
#define XE_HIDE				(0x00000040)			// 렌더링 금지. 잠시 꺼두셔도 좋습니다.
//#define XE_NO_OPTION_EFFECT	(0x10000000)		// CreateParticle()을 실행안하는 옵션.
#define XE_ITEM_FIRE		(0x00000080)		// 불 파티클을 생성하는 옵션
#define XE_ITEM_ELEC		(0x00000100)		// 전기 이펙트를 생성하는 옵션.
#define XE_ITEM_WATER		(0x00000200)		// 물 이펙트를 생성하는 옵션.
#define XE_ITEM_WIND		(0x00000400)		// 바람 이펙트를 생성하는 옵션.
#define XE_ITEM_EARTH		(0x00000800)		// 땅 이펙트를 생성하는 옵션.
#define XE_NOBUMP			(0x00001000)		// 범프사용않음
#define XE_HIGHLIGHT_OBJ	(0x00002000)		// 하이라이트 오브젝트(선택시)
#define XE_ITEM_GEN			(0x00004000)		// 일반속성

#if __VER >= 9 // __Y_ADV_ENCHANT_EFFECT
#define XE_ITEM_FIRE_AL		(0x00008000)		// Ultimate용
#define XE_ITEM_ELEC_AL		(0x00010000)		// Ultimate용
#define XE_ITEM_WATER_AL	(0x00020000)		// Ultimate용
#define XE_ITEM_WIND_AL		(0x00040000)		// Ultimate용
#define XE_ITEM_EARTH_AL	(0x00080000)		// Ultimate용
#define XE_ITEM_GEN_AL		(0x00100000)		// Ultimate용
#endif //__Y_ADV_ENCHANT_EFFECT

#ifdef __BS_EFFECT_LUA
#define XE_MTE				(0x00200000)		// multi texturing effect
#endif //__BS_EFFECT_LUA

// 상위 8비트는 쓰지 말것.  속성레벨로 쓸꺼다. 0 ~ 5범위.

extern double		g_dUps;		// update per sec - 화면 갱신 시간
extern double		g_dKps;		// key per sec - 한 애니메이션 키가 넘어가는데 기다리는 시간
extern DWORD	g_tmCurrent;		// 전역으로 사용할 현재 시간.

void	InitUPS( void );
void	CalcUPS( void );

// 스키닝 오브젝트들은 SetRenderState()등에 영향받지 않으므로 외부에서 직접 값을 넣어줘야 한다.
void	SetDiffuse( float r, float g, float b );
void	SetAmbient( float r, float g, float b );
void	SetLightVec( const D3DVECTOR &vLight );
void	SetLightPos( const D3DVECTOR &vLight );
void	SetTransformView( const D3DXMATRIX &mView );
void	SetTransformProj( const D3DXMATRIX &mProj );
void	SetLight( BOOL bLight );
void	SetNight( BOOL bNight );
void	SetFog( BOOL bFog );

// shadow
#define		SHADOWMAP_WIDTH		2048
#define		SHADOWMAP_HEIGHT	2048

#define		MAX_ELETEXTURE		18

extern LPD3DXRENDERTOSURFACE   g_pRenderToSurface;
extern LPDIRECT3DSURFACE9      g_pShadowSurface;
extern LPDIRECT3DTEXTURE9      g_pShadowTexture; 

extern D3DXMATRIX g_mViewLight;		// 빛으로부터 플레이어쪽으로 보는 뷰 매트릭스.
extern D3DXMATRIX g_mShadowProj;		// 쉐도우 프로젝션

// 쉐도우맵 텍스쳐 생성.
BOOL CreateShadowMap( LPDIRECT3DDEVICE9 pd3dDevice, D3DFORMAT backBufferFormat );
void DeleteShadowMap( LPDIRECT3DDEVICE9 pd3dDevice );
void RenderShadowMapInfo( LPDIRECT3DDEVICE9 pd3dDevice );
void SetStateShadowMap( LPDIRECT3DDEVICE9 pd3dDevice, int nShadowStage, const D3DXMATRIX &mView );
void ResetStateShadowMap( LPDIRECT3DDEVICE9 pd3dDevice, int nShadowStage );

class CModelGlobal
{
	LPDIRECT3DTEXTURE9	m_pExtTexture;		// 외부 지정 텍스쳐.
	
public:
	D3DXVECTOR3		m_vCameraPos, m_vCameraDest, m_vCameraForward;
	LPDIRECT3DTEXTURE9	m_pElecTexture[MAX_ELETEXTURE];
	BOOL	m_bDepthBias;
	
	CModelGlobal() 
	{
		memset( m_pElecTexture, 0, sizeof(m_pElecTexture) );
		m_bDepthBias = FALSE;
	}
	~CModelGlobal() {}
	
	void	SetCamera( const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDest );
	
	// 외부 지정 텍스쳐
	// 이것을 지정한 후 CObject3D::Render()를 하면 지정된 텍스쳐로 나온다.
	LPDIRECT3DTEXTURE9	GetTexture( void ) { return m_pExtTexture; }
	void	SetTexture( LPDIRECT3DTEXTURE9 pTexture ) { m_pExtTexture = pTexture; }		// 외부지정 텍스쳐.
		
	HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT InvalidateDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
};

extern CModelGlobal g_ModelGlobal;

extern int		g_nRenderCnt;	// 렌더화면때마다 하나씩 카운트 되는 변수
extern int		g_nProcessCnt;	// 프로세스마다 하나씩 카운트 되는 변수.


#define		PE_FIRE		1
#define		PE_ELEC		2
#define		PE_LASER	3
#define		PE_WATER	4
#define		PE_WIND		5
#define		PE_EARTH	6

#if __VER >= 9 // __Y_ADV_ENCHANT_EFFECT
#define		PE_FIRE_AL	7
#define		PE_ELEC_AL	8
#define		PE_LASER_AL	9
#define		PE_WATER_AL	10
#define		PE_WIND_AL	11
#define		PE_EARTH_AL	12
#define		PE_GEN		13
#define		PE_GEN_AL	14
#endif //__Y_ADV_ENCHANT_EFFECT

class CPartsEffect
{
public:
	int		m_nType;

	CPartsEffect() { m_nType = 0; }
	virtual	~CPartsEffect() {}

	virtual HRESULT InvalidateDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice ) { return S_OK; }
	virtual HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice ) { return S_OK; }
	
};


/////////////////////////////////////////////////////////////////////////////////////
#ifdef __CLIENT

#if __VER >= 9 // __Y_ADV_ENCHANT_EFFECT
#define MAX_PARTS_FIRE		28
#ifdef __CSC_ENCHANT_EFFECT_2
#define MAX_PARTS_FIRE_2	16
#endif //__CSC_ENCHANT_EFFECT_2
#else //__Y_ADV_ENCHANT_EFFECT
#define MAX_PARTS_FIRE		16
#endif //__Y_ADV_ENCHANT_EFFECT

class CSfxModel;

#ifdef __NEW_WEAPON_GLOW
class CPartsWeaponEffect : public CPartsEffect
{
	D3DXVECTOR3 m_vEffect;
	CSfxModel *m_pEffect;

public:
	CPartsWeaponEffect();
	CPartsWeaponEffect( int nType )
	{
		m_nType = nType;	
		m_pEffect = NULL;
	}
	virtual	~CPartsWeaponEffect();

	void	Create( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXVECTOR3 &vPos, DWORD dwSfx );
	void	Render( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX *mWorld );
};
#endif // __NEW_WEAPON_GLOW

class CPartsFire : public CPartsEffect
{
	D3DXVECTOR3	m_vList[ MAX_PARTS_FIRE ];
	CSfxModel *m_pList[ MAX_PARTS_FIRE ];
#ifdef __CSC_ENCHANT_EFFECT_2
	D3DXVECTOR3	m_vList_2[ MAX_PARTS_FIRE_2 ];
	CSfxModel *m_pList_2[ MAX_PARTS_FIRE_2 ];
#endif //__CSC_ENCHANT_EFFECT_2
public:
	CPartsFire() 
	{
		m_nType = PE_FIRE;
		memset( m_pList, 0, sizeof(m_pList) );
#ifdef __CSC_ENCHANT_EFFECT_2
		memset( m_pList_2, 0, sizeof(m_pList_2) );
#endif //__CSC_ENCHANT_EFFECT_2
	}
	CPartsFire( int nType )		// PE_ 시리즈.
	{
		m_nType = nType;
		memset( m_pList, 0, sizeof(m_pList) );
#ifdef __CSC_ENCHANT_EFFECT_2
		memset( m_pList_2, 0, sizeof(m_pList_2) );
#endif //__CSC_ENCHANT_EFFECT_2
	}
	virtual	~CPartsFire();
#if __VER >= 9 // __Y_ADV_ENCHANT_EFFECT
	void	Create( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXVECTOR3 vPos, DWORD dwSfx, const D3DXVECTOR3 &vScale );
#ifdef __CSC_ENCHANT_EFFECT_2
	void	Create( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXVECTOR3 vPos, DWORD dwSfx );
#endif //__CSC_ENCHANT_EFFECT_2
#else //__Y_ADV_ENCHANT_EFFECT
	void	Create( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXVECTOR3 &vPos, DWORD dwSfx );
#endif //__Y_ADV_ENCHANT_EFFECT
	void	Render( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX *mWorld );
};

#if __VER >= 10 // __Y_DRAGON_FIRE

#define MAX_PARTS_FIRE_DRAGON		256

struct PARTICLE_DRAGON
{
    D3DXVECTOR3 m_vPos;       // Current position
    D3DXVECTOR3 m_vVel;       // Current velocity
	
    D3DXCOLOR   m_clrDiffuse; // Initial diffuse color
    D3DXCOLOR   m_clrFade;    // Faded diffuse color
    FLOAT       m_fFade;      // Fade progression
	FLOAT		m_fGroundY;	
	FLOAT       m_fScale;
	int			m_nAni;
};

struct FIREVERTEX
{
    D3DXVECTOR3 v;
    D3DCOLOR    color;
    D3DXVECTOR2				    m_v2Tex;
	
    enum { 
        FVF = (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)
    };
};

#define BASE_VERTEX		6

class CPartsFireDragon : public CPartsEffect
{
	LPDIRECT3DTEXTURE9	m_pTexture;
	FIREVERTEX			m_vFan[BASE_VERTEX];
	LPDIRECT3DVERTEXBUFFER9 m_pParticleVB;
	PARTICLE_DRAGON*	m_pList[ MAX_PARTS_FIRE_DRAGON ];

public:
	CPartsFireDragon(); 
	~CPartsFireDragon() {}
	void	Create( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXVECTOR3 &vPos, DWORD dwSfx, const D3DXVECTOR3 &vScale, const D3DXVECTOR3 &vVel );
	void	Render( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX *mWorld );
	void	Process( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT InvalidateDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
};

#endif //__Y_DRAGON_FIRE

/////////////////////////////////////////////////////////////////////////////////////
struct FVF_BEAM
{
    D3DXVECTOR3 pos;	// The 3D position for the vertex
	FLOAT	tu, tv;
};
#define	D3DFVF_BEAM			(D3DFVF_XYZ | D3DFVF_TEX1)

#define MAX_BEAM	32

class CPartsBeam : public CPartsEffect
{
	BOOL	m_bActive;
	D3DXVECTOR3 m_vBeam[MAX_BEAM * 2];		// 빔의 시작과 끝점.
	D3DXVECTOR3 m_vCenter[MAX_BEAM];		// 빔의 중심.
//	float		m_fScale[MAX_BEAM];
	float		m_fSize[MAX_BEAM];
	FVF_BEAM m_vList[MAX_BEAM * 2 * 2];		// 카메라 뷰에 따라 변환되는 실제 빔버텍스들..
	int			m_nLevel[ MAX_BEAM ];
	
public:
	int		m_nMax;		// 빔 개수(버텍스2개가 한세트)
	D3DXVECTOR3 m_vLast;
	
	CPartsBeam() 
	{ 
		m_nType = PE_ELEC;
		m_bActive = FALSE; m_nMax = 0; 
		memset( m_vBeam, 0, sizeof(m_vBeam) );
		memset( m_vList, 0, sizeof(m_vList) );
		memset( m_vCenter, 0, sizeof(m_vCenter) );
		memset( m_nLevel, 0, sizeof(m_nType) );
	}
	~CPartsBeam() {};
	
	void	Create( const D3DXVECTOR3 &v );
	void	Create( const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, float fSize = 1.0f, int nLevel = 0 );
	void	Generate( const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, int nLevel );
	void	Render( LPDIRECT3DDEVICE9 pd3dDevice, 
					const D3DXMATRIX *mWorld, const D3DXVECTOR3 &vEye, const D3DXVECTOR3 &vForward, 
					const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, int nLevel );
};

//
//
//
class CPartsLaser : public CPartsEffect
{
	D3DXVECTOR3 m_vBeam[2];				// 레이저의 시작과 끝점.
//	D3DXVECTOR3 m_vCenter;				// 레이저의 중심
	float		m_fSize;
	FVF_BEAM m_vList[2 * 2];		// 카메라 뷰에 따라 변환되는 실제 빔버텍스들..
	int		m_nCount;	
	FLOAT	m_fRadian;				// 커졌다 작아졌다에 쓰이는 sin 라디안값.
	LPDIRECT3DTEXTURE9	m_pTexture;
	
public:
	BOOL	m_bActive;
	
	CPartsLaser() 
	{ 
		m_nType = PE_LASER;
		m_bActive = FALSE;
		m_nCount = 0;
		m_fRadian = 0;
		m_pTexture = NULL;
		memset( m_vBeam, 0, sizeof(m_vBeam) );
		memset( m_vList, 0, sizeof(m_vList) );
//		memset( m_vCenter, 0, sizeof(m_vCenter) );
	}
	~CPartsLaser() {}
	
	HRESULT InvalidateDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
		
	void	Create( const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, float fSize = 1.0f );
	void	SetPos( const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, float fSize );
//	void	Generate( const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2 );
	void	Render( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX *mWorld, const D3DXVECTOR3 &vEye, const D3DXVECTOR3 &vForward/*, const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, FLOAT fSize = 0*/ );
};

#endif // __CLIENT

//
//
//
class CTextureSurface
{
	LPDIRECT3DDEVICE9 m_pd3dDevice;
	LPD3DXRENDERTOSURFACE   m_pRenderToSurface;
	LPDIRECT3DSURFACE9      m_pSurface;
	LPDIRECT3DVERTEXBUFFER9		m_pd3dVB;
	
	void Init();
	void Destroy();
	
public:
	LPDIRECT3DTEXTURE9      m_pTexture; 
	int		m_nWidth, m_nHeight;
	BOOL	bTest;
	
	CTextureSurface();
	~CTextureSurface();
	
	HRESULT Create( LPDIRECT3DDEVICE9 pd3dDevice, D3DFORMAT backBufferFormat, int nWidth, int nHeight, BOOL bDepth = FALSE );
	void DeleteDeviceObjects( void );
	
	void BeginScene( void );
	void EndScene( void );
	
	void SetTexture( void );

	void	RenderNormal( LPDIRECT3DDEVICE9 pd3dDevice, CTextureSurface *pDst = NULL, BOOL bBlend = FALSE );
	void	DownSampling( LPDIRECT3DDEVICE9 pd3dDevice, CTextureSurface *pDst, float fOffsetU, float fOffsetV );
	void	RenderTargetBlur( LPDIRECT3DDEVICE9 pd3dDevice, CTextureSurface *pDst );
	void	RenderTargetBlurH( LPDIRECT3DDEVICE9 pd3dDevice, CTextureSurface *pDst );
	void	RenderTargetBlurV( LPDIRECT3DDEVICE9 pd3dDevice, CTextureSurface *pDst );
	
};


//
//
//
class CGlareLevel
{
	LPDIRECT3DDEVICE9 m_pd3dDevice;
	int		m_nWidth, m_nHeight;
	
	CTextureSurface		m_Surface[5];
	CTextureSurface		m_Black;

	void Init();
	void Destroy();
public:
	BOOL	m_bActive;
	CTextureSurface		m_Src;

	CGlareLevel();
	~CGlareLevel();
	
	void Create( LPDIRECT3DDEVICE9 pd3dDevice, D3DFORMAT backBufferFormat, int nWidth, int nHeight );
	void DeleteDeviceObjects( void );
	void SetState( LPDIRECT3DDEVICE9 pd3dDevice );
	void ResetState( LPDIRECT3DDEVICE9 pd3dDevice );
	void RenderWorld( LPDIRECT3DDEVICE9 pd3dDevice, CObj **pList, int nMax );
//	void DownSampling( LPDIRECT3DDEVICE9 pd3dDevice, CTextureSurface *pSrc, float fOffsetU, float fOffsetV );
	void RenderGlareEffect( LPDIRECT3DDEVICE9 pd3dDevice );
	void Blur( LPDIRECT3DDEVICE9 pd3dDevice );
		
};

extern CGlareLevel		g_Glare;






#endif

