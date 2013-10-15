#ifndef __TAILEFFECTMNG_H__
#define __TAILEFFECTMNG_H__

#ifdef __CLIENT

struct TAILVERTEX
{
    D3DXVECTOR3 v;
    D3DCOLOR    color;
	FLOAT	tx, ty;
	
    static const DWORD FVF;
};

struct TAIL
{
    D3DXVECTOR3 m_vPos1;       // 꼬리 좌표 1
    D3DXVECTOR3 m_vPos2;       // 꼬리 좌표 2
	
    D3DXCOLOR   m_clrDiffuse; // Initial diffuse color
    D3DXCOLOR   m_clrFade;    // Faded diffuse color
    FLOAT       m_fFade;      // Fade progression
	
    TAIL*   m_pNext;      // Next Tail in list
};


class CTailModel
{
protected:
	CSfxModel	m_SfxModel[20];

public:
	CTailModel();
	~CTailModel();
};


class CTailModelMng
{
protected:
	list<CTailModel*>	m_ListTailModel;

public:
	CTailModelMng();
	~CTailModelMng();
};



class CTailEffect
{
protected:
	int		m_nType;
	BOOL	m_bActive;
	FLOAT	m_fFadeSpeed;
	int		m_nMaxTail;
public:
	virtual void Init( void ) { m_nType = 0; m_bActive = FALSE; };
	
	virtual void	Create( int nType, FLOAT fFadeSpeed = 0.030f ) {};
	virtual int		CreateTail( const D3DXVECTOR3 &vPos1, const D3DXVECTOR3 &vPos2 ) { return 0; }
	virtual void	Clear( void ) {};
	int				GetType( void ) { return m_nType; }
	BOOL			IsActive() { return m_bActive; }
	
	virtual void	Destroy( void ) = 0;
	virtual HRESULT FrameMove( void ) = 0;
	virtual HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName ) = 0;
	virtual HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice ) = 0;
    virtual HRESULT InvalidateDeviceObjects() = 0;
	virtual HRESULT Render( LPDIRECT3DDEVICE9 pd3dDevice ) = 0;

	virtual HRESULT ChangeTexture( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName, int nType ) { return S_OK; }
};
	
class CTailEffectBelt : public CTailEffect
{
protected:
	TAIL	*m_pPool;
	int		m_nPoolPtr;
    DWORD	m_dwBase;
	DWORD	m_dwFlush;
    DWORD	m_dwDiscard;
	
    TAIL* m_pTails;			// 리스트의 현재 포인터.
    TAIL* m_pTailsFree;		// 비어있는 포인터
	
    LPDIRECT3DVERTEXBUFFER9 m_pVB;
	
	virtual void Init( void );
	virtual void Destroy( void );
	
public:
	LPDIRECT3DTEXTURE9 m_pTexture;
	
	CTailEffectBelt();
	virtual ~CTailEffectBelt();
	
	virtual void	Create( int nType, FLOAT fFadeSpeed = 0.030f );
	virtual int		CreateTail( const D3DXVECTOR3 &vPos1, const D3DXVECTOR3 &vPos2 );
	virtual void	Clear( void );
	
	virtual HRESULT FrameMove( void );
	
	virtual HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName );
	virtual HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
    virtual HRESULT InvalidateDeviceObjects();
	HRESULT ChangeTexture( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName, int nType );
	
	virtual HRESULT Render( LPDIRECT3DDEVICE9 pd3dDevice );
};

//CModelObject* pMesh=g_SfxMeshMng.Mesh(m_strTex);
typedef struct TAILMODEL
{
	D3DXMATRIX  m_mWorld;
	int			m_nFactor;
} TAILMODEL;

class CTailEffectModel : public CTailEffect
{
protected:
	int					m_nMaxTail;
	CModelObject*		m_pModel;
	vector<TAILMODEL>	m_vecTail;
	
	virtual void Init( void );
	virtual void Destroy( void );
	
public:
	CTailEffectModel();
	virtual ~CTailEffectModel();
	
	virtual void	Create( int nType, FLOAT fFadeSpeed = 0.030f );
	virtual int		CreateTail( D3DXMATRIX* pTail );
	virtual void	Clear( void );
	
	virtual HRESULT FrameMove( void );
	
	virtual HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName );
	virtual HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
    virtual HRESULT InvalidateDeviceObjects();
	
	virtual HRESULT Render( LPDIRECT3DDEVICE9 pd3dDevice );
};


#define		MAX_TAILEFFECT	32		// 최대 파티클 종류.

class CTailEffectMng
{
	LPDIRECT3DDEVICE9 m_pd3dDevice;
	int		m_nMaxType;
	CTailEffect* m_TailEffects[ MAX_TAILEFFECT ];

	void	Init( void );
	void	Destroy( void );
public:
	BOOL	m_bActive;
	
	CTailEffectMng();
	~CTailEffectMng();
	
	HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice ) { m_pd3dDevice = pd3dDevice; return S_OK; }
	HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
    HRESULT InvalidateDeviceObjects();
		
	CTailEffect *AddEffect( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName, int nType, FLOAT fFadeSpeed = 0.030f );
	int			Delete( CTailEffect *pTail );
		
	void	Process( void );
	void	Render( LPDIRECT3DDEVICE9 pd3dDevice );
	
};




extern CTailEffectMng	g_TailEffectMng;

#endif // CLIENT

#endif // __TAILEFFECTMNG_H__