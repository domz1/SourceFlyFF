// Particle.h: interface for the CParticle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARTICLE_H__6B6A0809_A096_454E_BCBE_825439696C0F__INCLUDED_)
#define AFX_PARTICLE_H__6B6A0809_A096_454E_BCBE_825439696C0F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if 0
#include <d3dx9math.h>
#include <list>
#include <vector>
#include <map>

using namespace std;


#define MAX_CYLINDER    74
#define MAX_ANI         100

float RandomNumber(float fMin, float fMax);

struct PARTICLE_HEADERS
{
	char m_ID[12];    // 'F4_PARTICLE'
};

struct DataUnit
{
    D3DXVECTOR3		m_v3Pos;
    
    D3DXVECTOR3		m_v3Velocity;
    D3DXVECTOR3		m_v3Accel;
    D3DXVECTOR3		m_v3Gravity;
    D3DXVECTOR3     m_v3Scal;
    D3DXVECTOR3     m_v3Rot;
    D3DXVECTOR3     m_v3StartRot;
    
    D3DXCOLOR       m_Color;
    
    FLOAT			m_fLife;
    FLOAT			m_fCurrTime;
    
    FLOAT           m_fAniTime;
    FLOAT           m_fCurrAniTime;
    BYTE            m_AniIndex;
};

class PARTICLE_VERTEX
{
public:
    D3DXVECTOR3				    m_v3Pos;
    D3DCOLOR				    m_dwColor;
    D3DXVECTOR2				    m_v2Tex;
    
    enum { 
        FVF = (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)
    };
};

class CTextureParticle
{
public:
	LPDIRECT3DTEXTURE9		m_pTex;
	TCHAR					m_pName[2048];
};


class CUnit  
{
protected:
public:
    friend class CEmitter;
    friend class CParticle;
	
	D3DXVECTOR3		m_v3Pos;
	
	D3DXVECTOR3		m_v3Velocity;
	D3DXVECTOR3		m_v3Accel;
	D3DXVECTOR3		m_v3Gravity;
	D3DXVECTOR3     m_v3Scal;
    D3DXVECTOR3     m_v3Rot;
    D3DXVECTOR3     m_v3StartRot;
	
    D3DXCOLOR       m_Color;
	
	FLOAT			m_fLife;
    FLOAT			m_fCurrTime;
	
    FLOAT           m_fAniTime;
    FLOAT           m_fCurrAniTime;
    BYTE            m_AniIndex;
    
public:
	CUnit           ();
	virtual         ~CUnit          ();
	
    HRESULT         FrameMove       ( FLOAT fElapsedTime, CEmitter* pEmitter  );
};

void DataUnitCopy( CUnit* pUnit, DataUnit src );
void UnitCopy( CUnit* pUnit, DataUnit& src );

class CEmitter
{
public:
    enum    KIND { RECTANGLE = 0, CYLINDER = 1 };
    
    friend class CUnit;
    friend class CParticle;

    TCHAR                       m_pName[256];
    
    FLOAT                       m_fCps;
    FLOAT                       m_ffCps;
    FLOAT                       m_fCurrCps;
	
    FLOAT                       m_fTerm;
    FLOAT                       m_fCurrTerm;

    BOOL                        m_bBillboard;
    BOOL                        m_bActive;

    D3DXVECTOR3		            m_v3Pos;
    D3DXVECTOR3                 m_v3VelocityRot;
    D3DXVECTOR3                 m_v3AccelRot;

    D3DXMATRIX                  m_WorldM;
    
    list< CUnit* >              m_plistUnit;
    
    vector< CTextureParticle >			m_pvecTexture;

    PARTICLE_VERTEX             m_TriRect[4];
    PARTICLE_VERTEX             m_TriCylinder[MAX_CYLINDER];
    
    DWORD                       m_dwBlend1;
    DWORD                       m_dwBlend2;

    BYTE                        m_Kind;

    vector< vector<PARTICLE_VERTEX> > m_vecvecVertex;
    
    
public:
    CUnit*                      m_pUnit1;
    CUnit*                      m_pUnit2;

public:
	CEmitter        ();
    virtual          ~CEmitter       ();

    HRESULT			 SetVertex	     ( CUnit* pUnit );
    
    HRESULT          Push_Unit       ();
    HRESULT          FrameMove       ( FLOAT fElapsedTime );
	HRESULT			 SetTexture	     ( LPDIRECT3DDEVICE9 pd3dDevice, TCHAR *pPath, TCHAR *pFilename );
	HRESULT			 Reset();

	void			 SetWorldMatrix  ( D3DXMATRIX WorldM ) { m_WorldM = WorldM; }

	void			 ClearTexture    ();
};


class CParticle
{
public:
	D3DXVECTOR3						m_v3Pos;
	D3DXMATRIX						m_mWorld;
    CParticle       ();
    virtual         ~CParticle      ();
    
public:

protected:
    list< CEmitter* >               m_listEmitter;
    LPDIRECT3DVERTEXBUFFER9         m_pVB;
    
public:
	BOOL Load( char* FileName );
	HRESULT				ResetAllActive	( BOOL bactive );
    list< CEmitter* >&  GetEmitterList  ( ) { return m_listEmitter; }
    CEmitter*			GetEmitter      ( int index );
	void				DeleteEmitter   ( int index );
    int					GetEmitterSize  ( ) { return m_listEmitter.size(); }

    HRESULT				Push_Emitter    ( CEmitter* pEmitter );
    HRESULT				Delete			( );
    HRESULT				Render          ( LPDIRECT3DDEVICE9 pd3dDevice );
    HRESULT				Restore			( LPDIRECT3DDEVICE9 pDevice );
    HRESULT				Invalidate		( );
    HRESULT				FrameMove       ( FLOAT fElapsedTime );
	void				SetPos( D3DXVECTOR3 v3Pos )
	{
		m_mWorld._41 = v3Pos.x;
		m_mWorld._42 = v3Pos.y;
		m_mWorld._43 = v3Pos.z;
	}
};

class CParticleExMng
{
public:
    CParticleExMng       ();
    virtual         ~CParticleExMng      ();
    
public:
	
protected:
    list< CParticle* >              m_listParticle;
    
public:
	CParticle*			AddParticle		( char* FileName );
    HRESULT				Delete			( );
    HRESULT				Render          ( LPDIRECT3DDEVICE9 pd3dDevice );
    HRESULT				Restore			( LPDIRECT3DDEVICE9 pDevice );
    HRESULT				Invalidate		( );
    HRESULT				FrameMove       ( FLOAT fElapsedTime );
};

#endif
#endif // !defined(AFX_PARTICLE_H__6B6A0809_A096_454E_BCBE_825439696C0F__INCLUDED_)
