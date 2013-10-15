// Sfx.h: interface for the CSfx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MODELSFX_H__D652787A_4E67_419F_AB52_0A8E2FED08AE__INCLUDED_)
#define AFX_MODELSFX_H__D652787A_4E67_419F_AB52_0A8E2FED08AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/*
               CSfxObj
			      |
		 +--------+--------+--------------+
         |                 |              |
   CSfxParticle            |              |
         |                 |           CSfxMesh
       <list>              |
                            |
   CSfxParticleMng      CSfxBill
         |                 |
		 +-----------------+
               
			   CModel 
                  |
              CModelSfx // sfx manager

 */
// sfx obj base class
#if 0
class CSfxObj 
{
public:
	D3DXVECTOR3		m_vPos  ; // 오브젝트 좌표 
	FLOAT			m_fAngle; // 오브젝트 각도 
	D3DXVECTOR3		m_vScale; // 오브젝트 사이즈 
	CModel*         m_pModel;
	BOOL            m_bDelete;
	//DWORD           m_dwIndex;
	//DWORD           m_dwType;

	CSfxObj();
	~CSfxObj();
virtual	CModel* LoadModel( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwType, DWORD dwIndex );

	virtual void Process();
#ifdef __CLIENT
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
#endif	// __WORLDSERVER
};
enum PARTICLEACCELTYPE 
{
	PARTICLEACCELTYPE_FACTOR,
	PARTICLEACCELTYPE_VECTOR,
};
// 파티클 낱개 
class CSfxParticle: public CSfxObj
{
public:
	int m_nEffect;

	WORD m_nFrame;

	WORD m_nAppear; // 나타나는 시간(프레임)
	WORD m_nKeep; // 유지시간(프레임)
	WORD m_nDisappear; // 사라지는 시간(프레임)

	D3DXVECTOR3 m_vDirection; // 움직이는 방향

	PARTICLEACCELTYPE m_nAccelType; // 가속도 타입
	D3DXVECTOR3 m_vAccel;  // 가속도 벡터
	FLOAT m_fAccel;  // 현재속도에 곱하는 가속도

	CSfxParticle(WORD nAppear,WORD nKeep,WORD nDisappear,D3DXVECTOR3 vPos,D3DXVECTOR3 vDirection,PARTICLEACCELTYPE nAccelType,D3DXVECTOR3 vAccel,FLOAT fAccel);
	virtual ~CSfxParticle();
	virtual void Process();
#ifdef __CLIENT
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
#endif	// __WORLDSERVER
};
// 파티클 매니져 오브젝트 
class CSfxParticleMng : public CSfxObj
{
public:

	CSfxParticleMng();
	virtual ~CSfxParticleMng();

	list<CSfxObj*> m_listParticle;
	virtual void GenerateParticle( D3DXVECTOR3* pvPos ) {}
	virtual void Process();
#ifdef __CLIENT
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
#endif
};

class CSfxJetParticleMng: public CSfxParticleMng
{
public:
	WORD m_nAppear; // 나타나는 시간(프레임)
	WORD m_nKeep; // 유지시간(프레임)
	WORD m_nDisappear; // 사라지는 시간(프레임)
	DWORD m_nMinSpeed;
	DWORD m_nMaxSpeed;
	DWORD m_nMinYSpeed;
	DWORD m_nMaxYSpeed;
	DWORD m_nMaxRadius;
	DWORD m_nMinRadius;
	FLOAT m_fGravity;
	D3DXVECTOR3 m_vScale;
	DWORD m_nResource;
	DWORD m_nEffect;
	CSfxJetParticleMng();
	~CSfxJetParticleMng();
	void GenerateParticle( D3DXVECTOR3* pvPos ) ;
};


// 빌보드 
class CSfxBill : public CSfxObj
{

};
// 매쉬 
class CSfxMesh : public CSfxObj
{
public:
	CSfxMesh();
	virtual ~CSfxMesh();
	virtual void Process();
#ifdef __CLIENT
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
#endif
};
// 전체 관리자 모델 
class CModelSfx : public CModel
{
public:
	list<CSfxObj*> m_listSfx;
	//list<CSfxObj*> m_listParticle;
	//int m_nNumParticle;

	int LoadModel( LPCTSTR szFileName );
	void AddSfxObj( CSfxObj* pSfxObj );
	CModelSfx();
	~CModelSfx();
	//void GenerateParticle( D3DXVECTOR3* pvPos ) ;
	virtual void FrameMove(D3DXVECTOR3 *pvSndPos = NULL );
#ifdef __CLIENT
	virtual BOOL Render( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX* pmWorld = NULL );
#endif	// __WORLDSERVER
};

#endif
#endif // !defined(AFX_SFX_H__D652787A_4E67_419F_AB52_0A8E2FED08AE__INCLUDED_)
