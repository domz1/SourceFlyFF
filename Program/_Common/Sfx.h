// Sfx.h: interface for the CSfx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SFX_H__D652787A_4E67_419F_AB52_0A8E2FED08AE__INCLUDED_)
#define AFX_SFX_H__D652787A_4E67_419F_AB52_0A8E2FED08AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "light.h"
class CSfxGenMoveMark;
extern CSfxGenMoveMark* g_pMoveMark;

#ifdef __CLIENT
#include "mempooler.h"
#endif	// __CLIENT

class CSfxModel;

/// 효과(파티클, 발사체)를 처리하는 base class
class CSfx  : public CCtrl
{
protected:
	CSfxModel m_SfxObj2; // 연속 효과 : 사용하던지 말던지  
	CSfxModel m_SfxObj3; // 연속 효과 : 사용하던지 말던지 

#if defined( __CLIENT ) 
	DWORD	m_dwSkill;		// 스킬발사체면 스킬 아이디를..
#endif	// 
	
public:
	int m_nSec;
	int m_nFrame;

	CSfxModel* m_pSfxObj; // 이펙트 모델 포인터
	OBJID       m_idSrc;    // 쏜놈 ID
	OBJID       m_idDest;   // 맞을놈 ID
	D3DXVECTOR3 m_vPosDest; // 맞을 위치
//	D3DXVECTOR3 m_vPosCur;  // 현재 위치 - 이게 왜 필요한걸까.
#if defined( __CLIENT ) 
	CLight  m_light;
	int		m_idSfxHit;
	int		m_nMagicPower;	// 충전 단계
//	DWORD	m_dwSkillLevel;	// 스킬레벨.
#endif	// 

	CSfx();
	CSfx( int nIndex, const OBJID idSrc, const D3DXVECTOR3& vPosSrc, const OBJID idDest, const D3DXVECTOR3& vPosDest );
	virtual ~CSfx();

#if defined( __CLIENT ) 
	void	SetSkill( DWORD dwSkill )
	{
		m_dwSkill = dwSkill;
	}
	DWORD	GetSkill()
	{
		return m_dwSkill;
	}
#endif
	
	void Process(); // 각 프레임마다 위치이동, 애니메이션 등 처리
	void DamageToTarget( int nDmgCnt = 0, float fDmgAngle = 0, float fDmgPower = 0, int nMaxDmgCnt = 1 ); // 목표물에 맞았을때 데미지를 준다
	BOOL SetIndex( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwIndex, BOOL bInitProp = FALSE );

	virtual	int SetSfx( LPDIRECT3DDEVICE9 pd3dDevice, int nIndex, D3DXVECTOR3& vPosSrc, OBJID idSrc, D3DXVECTOR3& vPosDest, OBJID idDest, int nSec = 0 ); // 사용할 SFX 지정
	virtual void ShootSfx( float fAngXZ, float fAngY, float fSpeed ) {}
	virtual void SetPartLink( int nPart ) {}
#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice ); // 렌더
#endif	// __WORLDSERVER

};

// 쏘고 날아가서 맞고 터지는 일반적인 이펙트
class CSfxShoot : public CSfx
{
public:
	BOOL	m_bHit; // 명중했는지의 여부
	BOOL	m_bDir;
	DWORD	m_dwSndHit;		// 맞았을때 소리.
	DWORD	m_dwSfxHit;		// 폭발 이펙트
	FLOAT	m_fHitScale;	// 폭발씬 스케일링.
	
	CSfxShoot();
	virtual ~CSfxShoot();
	
	void	SetHitSfx( LPCTSTR szFileName, OBJID idSrc, OBJID idDst, FLOAT fScale = 1.0f );	// 맞았을때 이펙트 등록
	void	SetHitSfx( DWORD dwIndex, OBJID idSrc, OBJID idDst, FLOAT fScale = 1.0f );	// 맞았을때 이펙트 등록
	void	SetDir( BOOL bDir );		// 방향성있는 sfx 인가.
	void	SetSndHit( DWORD dwSndHit ) { m_dwSndHit = dwSndHit; }
	
	virtual void Process();
#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
#endif	// __WORLDSERVER
};


// CSfx랑 똑같고 idSrc의 회전방향까지 똑같은거.
class CSfxRotate : public CSfx
{
public:
	CSfxRotate() {}
	~CSfxRotate() {}

	void Process();
#ifndef __WORLDSERVER
	void Render( LPDIRECT3DDEVICE9 pd3dDevice );
#endif
};

class CSfxDuelParty : public CSfx
{
public:
	int	m_nType;
	CSfxDuelParty() { m_nType = 0; };	// 0:듀얼  1:전쟁.
	~CSfxDuelParty() {};

	void Process();
	void DeleteSfx( CMover* pEnemy );
#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
#endif		
};

class CSfxSetItem : public CSfx
{
public:
	CSfxSetItem() {};
	~CSfxSetItem();
	void Process();
#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
#endif		
};


class CSfxCollect : public CSfx
{
public:
	CSfxCollect() {};	
	~CSfxCollect() {};
	
	void Process();
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
};

// 클락워크 보스 레이저 충전.
class CSfxClockWorksCharge : public CSfx
{
public:
	CSfxClockWorksCharge() {};	
	~CSfxClockWorksCharge() {};
	
	void Process();
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
};

// 클락워크 보스 왼팔에서 발사할때 나오는 이펙트.
class CSfxClockWorksCannon : public CSfx
{
public:
	CSfxClockWorksCannon() {};	
	~CSfxClockWorksCannon() {};
	
	void Process();
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
};

// 머쉬무트 보스 레이저 충전.
class CSfxMushmootCharge : public CSfx
{
public:
	int	m_nEventPos;
	CSfxMushmootCharge() {};	
	~CSfxMushmootCharge() {};
	
	void Process();
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __WORLDSERVER
// 일반 이펙트 : 통상 데미지
class CSfxGenNormalDmg01 : public CSfx
{
public:
	CSfxGenNormalDmg01();
	virtual ~CSfxGenNormalDmg01();
	virtual void Process();
#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
#endif	// __WORLDSERVER
#ifdef __CLIENT
public:
#ifndef __VM_0820
#ifndef __MEM_TRACE
	static	MemPooler<CSfxGenNormalDmg01>* m_pPool;
	void*	operator new( size_t nSize )	{	return CSfxGenNormalDmg01::m_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CSfxGenNormalDmg01::m_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CSfxGenNormalDmg01::m_pPool->Free( (CSfxGenNormalDmg01*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CSfxGenNormalDmg01::m_pPool->Free( (CSfxGenNormalDmg01*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820
#endif	// __CLIENT
};

// 극단스킬 이펙트 : 스트레칭01
class CSfxTroStretching01 : public CSfx
{
public:
	CSfxTroStretching01();
	virtual ~CSfxTroStretching01();
	virtual void Process();
#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
#endif	// __WORLDSERVER
};
// 극단스킬 이펙트 : 스트레칭02
class CSfxTroStretching02 : public CSfx
{
public:
	CSfxTroStretching02();
	virtual ~CSfxTroStretching02();
	virtual void Process();
#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
#endif	// __WORLDSERVER
};
// 극단스킬 이펙트 : 집중공격
class CSfxTroBlitz : public CSfx
{
public:
	CSfxTroBlitz();
	virtual ~CSfxTroBlitz();
	virtual void Process();
#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
#endif	// __WORLDSERVER
};

// 일반 이펙트 : 아직 안씀
class CSfxGenSuperDmg01 : public CSfx
{
public:
	CSfxGenSuperDmg01();
	virtual ~CSfxGenSuperDmg01();
	virtual void Process();
	#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	#endif	// __WORLDSERVER
};
// 일반 이펙트 : 아직 안씀
class CSfxGenSkillDmg01 : public CSfx
{
public:
	CSfxGenSkillDmg01();
	virtual ~CSfxGenSkillDmg01();
	virtual void Process();
	#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	#endif	// __WORLDSERVER
};
// 일반 이펙트 : 아직 안씀
class CSfxGenMonsterDmg01 : public CSfx
{
public:
	CSfxGenMonsterDmg01();
	virtual ~CSfxGenMonsterDmg01();
	virtual void Process();
#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
#endif	// __WORLDSERVER
};
// 일반 이펙트 : 부활
class CSfxGenRestoration01 : public CSfx
{
public:
	CSfxGenRestoration01();
	virtual ~CSfxGenRestoration01();
	virtual void Process();
	#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	#endif	// __WORLDSERVER
};
// 일반 이펙트 : 치료
class CSfxGenCure: public CSfx
{
public:
	CSfxGenCure();
	virtual ~CSfxGenCure();
	virtual void Process();
	#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	#endif	// __WORLDSERVER
};
// 일반 이펙트 : 강화
class CSfxGenIncrease01 : public CSfx
{
public:
	CSfxGenIncrease01();
	virtual ~CSfxGenIncrease01();
	virtual void Process();
	#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	#endif	// __WORLDSERVER
};
// 일반 이펙트 : 레벨업
class CSfxGenLevelUp : public CSfx
{
public:
	CSfxGenLevelUp();
	virtual ~CSfxGenLevelUp();
	virtual void Process();
	#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	#endif	// __WORLDSERVER
};
// 일반 이펙트 : 로그인
class CSfxGenLogin : public CSfx
{
public:
	CSfxGenLogin();
	virtual ~CSfxGenLogin();
	virtual void Process();
	#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	#endif	// __WORLDSERVER
};
// 일반 이펙트 : 워프
class CSfxGenWarp : public CSfx
{
public:
	CSfxGenWarp();
	virtual ~CSfxGenWarp();
	virtual void Process();
	#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	#endif	// __WORLDSERVER
};
// 일반 이펙트 : 사망
class CSfxGenPcDie : public CSfx
{
public:
	CSfxGenPcDie();
	virtual ~CSfxGenPcDie();
	virtual void Process();
	#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	#endif	// __WORLDSERVER
};
// 일반 이펙트 : 몹 리스폰
class CSfxGenMonsterSpawn : public CSfx
{
public:
	CSfxGenMonsterSpawn();
	virtual ~CSfxGenMonsterSpawn();
	virtual void Process();
	#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	#endif	// __WORLDSERVER
};
// 일반 이펙트 : 이동위치
class CSfxGenMoveMark: public CSfx
{
public:
	CSfxGenMoveMark();
	virtual ~CSfxGenMoveMark();
	virtual void Process();
#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
#endif	// __WORLDSERVER
#ifdef __CLIENT
public:
#ifndef __VM_0820
#ifndef __MEM_TRACE
	static MemPooler<CSfxGenMoveMark>*	m_pPool;
	void*	operator new( size_t nSize )	{	return CSfxGenMoveMark::m_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CSfxGenMoveMark::m_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CSfxGenMoveMark::m_pPool->Free( (CSfxGenMoveMark*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CSfxGenMoveMark::m_pPool->Free( (CSfxGenMoveMark*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820
#endif	// __CLIENT
};
// 일반 이펙트 : 물의 파장
class CSfxGenWaterCircle: public CSfx
{
public:
	CSfxGenWaterCircle();
	virtual ~CSfxGenWaterCircle();
	virtual void Process();
#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
#endif	// __WORLDSERVER
#ifdef __CLIENT
public:
#ifndef __VM_0820
#ifndef __MEM_TRACE
	static MemPooler<CSfxGenWaterCircle>*	m_pPool;
	void*	operator new( size_t nSize )	{	return CSfxGenWaterCircle::m_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CSfxGenWaterCircle::m_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CSfxGenWaterCircle::m_pPool->Free( (CSfxGenWaterCircle*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CSfxGenWaterCircle::m_pPool->Free( (CSfxGenWaterCircle*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820
#endif	// __CLIENT
};

// 일반 이펙트 : 물의 파장
class CSfxGenRainCircle: public CSfx
{
public:
	CSfxGenRainCircle();
	virtual ~CSfxGenRainCircle();
	virtual void Process();
#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
#endif	// __WORLDSERVER
#ifdef __CLIENT
public:
#ifndef __VM_0820
#ifndef __MEM_TRACE
	static MemPooler<CSfxGenRainCircle>*	m_pPool;
	void*	operator new( size_t nSize )	{	return CSfxGenRainCircle::m_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CSfxGenRainCircle::m_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CSfxGenRainCircle::m_pPool->Free( (CSfxGenRainCircle*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CSfxGenRainCircle::m_pPool->Free( (CSfxGenRainCircle*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820
#endif	// __CLIENT
};

// 일반 이펙트 : 물 튀기기
class CSfxGenWaterCrown: public CSfx
{
public:
	CSfxGenWaterCrown();
	virtual ~CSfxGenWaterCrown();
	virtual void Process();
	#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	#endif	// __WORLDSERVER
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



class CSfxMagicMiAtk1 : public CSfxShoot
{
public:
	FLOAT m_fCenter;		// 타겟의 좌표.y 보정치.
	FLOAT m_fRadiusXZ;		// 타겟의 XZ평면의 반지름.
	//CSfxModel m_SfxObj2; // 명중시 이펙트 표시용
	CSfxMagicMiAtk1();
	virtual ~CSfxMagicMiAtk1();
	virtual void Process();
#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
#endif	// __WORLDSERVER
};


// 완드는 모으기 공격 단계별로 4가지
// 일반 이펙트 : 완드1
class CSfxItemWandAtk1 : public CSfxShoot
{
public:
	FLOAT m_fCenter;		// 타겟의 좌표.y 보정치.
	FLOAT m_fRadiusXZ;		// 타겟의 XZ평면의 반지름.
	//CSfxModel m_SfxObj2; // 명중시 이펙트 표시용
	CSfxItemWandAtk1();
	virtual ~CSfxItemWandAtk1();
	virtual void Process();
	#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	#endif	// __WORLDSERVER
};

class CSfxItemRangeAtk1 : public CSfxShoot
{
public:
#if 1 
#ifdef __CLIENT
	CTailEffectBelt*	m_pTail;		// 꼬랑지 이펙트	
#endif
	FLOAT			m_fOriLen;
	D3DXVECTOR3		m_v3SrcPos;
#endif
	FLOAT m_fCenter;		// 타겟의 좌표.y 보정치.
	FLOAT m_fRadiusXZ;		// 타겟의 XZ평면의 반지름.
	//CSfxModel m_SfxObj2; // 명중시 이펙트 표시용
	CSfxItemRangeAtk1();
	virtual ~CSfxItemRangeAtk1();
	virtual void Process();
#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
#endif	// __WORLDSERVER
};

// 아래로 떨어지는 화살 이펙트
class CSfxItemRangeAtk1_Allow : public CSfxShoot
{
public:
	FLOAT	m_fSpeed;
	CSfxItemRangeAtk1_Allow();
	virtual ~CSfxItemRangeAtk1_Allow();
	virtual void Process();
#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
#endif	// __WORLDSERVER
};

// CSfxItemRangeAtk1_Allow생성 관리 - 실제 사용되어지는 부분
class CSfxItemRangeAtk1_AllowRain : public CSfxShoot
{
public:
	int		m_nCount;
	CSfxItemRangeAtk1_AllowRain();
	virtual ~CSfxItemRangeAtk1_AllowRain();
	virtual void Process();
#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
#endif	// __WORLDSERVER
};

// CSfxItemRangeAtk1_Allow생성 관리 - 실제 사용되어지는 부분
class CSfxItemRangeAtk1_Stone : public CSfxShoot
{
public:
	FLOAT	m_fSpeed;
	CSfxItemRangeAtk1_Stone();
	virtual ~CSfxItemRangeAtk1_Stone();
	virtual void Process();
#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
#endif	// __WORLDSERVER
};


class CSfxItemRangeAtk1_StoneRain : public CSfxShoot
{
public:
	int		m_nCount;
	CSfxItemRangeAtk1_StoneRain();
	virtual ~CSfxItemRangeAtk1_StoneRain();
	virtual void Process();
#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
#endif	// __WORLDSERVER
};

class CSfxItemRangeAtk_JunkBow : public CSfxShoot
{
public:
#if 1 
#ifdef __CLIENT
	CTailEffectBelt*	m_pTail[3];		// 꼬랑지 이펙트	
#endif
	FLOAT			m_fOriLen;
	D3DXVECTOR3		m_v3SrcPos;
#endif
	FLOAT m_fCenter;		// 타겟의 좌표.y 보정치.
	FLOAT m_fRadiusXZ;		// 타겟의 XZ평면의 반지름.
	CSfxItemRangeAtk_JunkBow();
	virtual ~CSfxItemRangeAtk_JunkBow();
	virtual void Process();
#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
#endif	// __WORLDSERVER
};

class CSfxItemYoyoAtk : public CSfxShoot
{
public:
#ifdef __CLIENT
	int				m_nMaxSpline;
	CTailEffectModel*	m_pTail;		// 꼬랑지 이펙트	
	D3DXVECTOR3		m_aSpline[30];
	int				m_nStep;
	int				m_nType;
	int				m_nCount;

	int				m_nTailIndex;
	CSfxModel		m_pSfxTailModel[20];
	void			ProcessTail();
	void			AddSfxTail( CSfxModel* pSfxModel );	// 맞았을때 이펙트 등록
	
#endif
	FLOAT			m_fMaxLength;
	D3DXVECTOR3		m_v3SrcPos;
	D3DXVECTOR3		m_vDelta;
	int				m_nDelayCount;
	
	

	
	FLOAT m_fCenter;		// 타겟의 좌표.y 보정치.
	FLOAT m_fRadiusXZ;		// 타겟의 XZ평면의 반지름.

	void  MakePath(int nType);
	D3DXVECTOR3	SplineSlerp( D3DXVECTOR3 *v1, D3DXVECTOR3 *v2, D3DXVECTOR3 *v3, D3DXVECTOR3 *v4, float fSlerp );
	
	CSfxItemYoyoAtk();
	virtual ~CSfxItemYoyoAtk();
	virtual void Process();
#ifndef __WORLDSERVER
	void	RenderTail( LPDIRECT3DDEVICE9 pd3dDevice );
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
#endif	// __WORLDSERVER
};


// 일직선으로 발사하는 장거리 탄환류.
class CSfxAtkStraight : public CSfxShoot
{
	D3DXVECTOR3 m_vDelta;			// 이동 증가량.
	DWORD		m_dwExplosion;		// 맞고난후 생성될 폭발 아이디.
public:
	//CSfxModel m_SfxObj2; // 명중시 이펙트 표시용
	CSfxAtkStraight();
	virtual ~CSfxAtkStraight();
	virtual void Process();
	void ShootSfx( float fAngXZ, float fAngH, float fSpeed, DWORD dwExplosion );		// 발사체의 이동증가량을 선계산함. 
	
#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
#endif	// __WORLDSERVER
};

class CSfxItemWandAtkAir : public CSfxShoot
{
	D3DXVECTOR3		m_vDelta;		// 관성.
public:
	//CSfxModel m_SfxObj2; // 명중시 이펙트 표시용
	BOOL m_bHit; // 명중했는지의 여부
	CSfxItemWandAtkAir();
	virtual ~CSfxItemWandAtkAir();
	virtual void Process();
	virtual	int SetSfx( LPDIRECT3DDEVICE9 pd3dDevice, int nIndex, D3DXVECTOR3& vPosSrc, OBJID idSrc, D3DXVECTOR3& vPosDest, OBJID idDest, int nSec = 0 ); // 사용할 SFX 지정
	virtual void ShootSfx( float fAngXZ, float fAngY, float fSpeed );
#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
#endif	// __WORLDSERVER
};


// 일반 이펙트 : 완드2
class CSfxItemWandAtk2 : public CSfxShoot
{
public:
	//CSfxModel m_SfxObj2; // 명중시 이펙트 표시용
	BOOL m_bHit; // 명중했는지의 여부
	CSfxItemWandAtk2();
	virtual ~CSfxItemWandAtk2();
	virtual void Process();
	#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	#endif	// __WORLDSERVER
};
// 일반 이펙트 : 완드3
class CSfxItemWandAtk3 : public CSfxShoot
{
public:
	//CSfxModel m_SfxObj2; // 명중시 이펙트 표시용
	BOOL m_bHit; // 명중했는지의 여부
	CSfxItemWandAtk3();
	virtual ~CSfxItemWandAtk3();
	virtual void Process();
	#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	#endif	// __WORLDSERVER
};
// 일반 이펙트 : 완드4
class CSfxItemWandAtk4 : public CSfxShoot
{
public:
	//CSfxModel m_SfxObj2; // 명중시 이펙트 표시용
	BOOL m_bHit; // 명중했는지의 여부
	CSfxItemWandAtk4();
	virtual ~CSfxItemWandAtk4();
	virtual void Process();
	#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	#endif	// __WORLDSERVER
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 방랑자 스킬 : 오버커터
class CSfxSkillVagOverCutter: public CSfx
{
public:
	//CSfxModel m_SfxObj2; // 명중시 이펙트 표시용
	CSfxSkillVagOverCutter();
	virtual ~CSfxSkillVagOverCutter();
	virtual void Process();
	#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	#endif	// __WORLDSERVER
};
// 방랑자 스킬 : 클린히트
class CSfxSkillVagCleanHit : public CSfx
{
public:
	CSfxSkillVagCleanHit();
	virtual ~CSfxSkillVagCleanHit();
	virtual void Process();
	#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	#endif	// __WORLDSERVER
};
// 방랑자 스킬 : 브랜디쉬
class CSfxSkillVagBrandish : public CSfx
{
public:
	CSfxSkillVagBrandish();
	virtual ~CSfxSkillVagBrandish();
	virtual void Process();
	#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	#endif	// __WORLDSERVER
};

// 머셔너리 스킬 : 킨휠
class CSfxSkillMerKeenWheel : public CSfx
{
public:
	CSfxSkillMerKeenWheel();
	virtual ~CSfxSkillMerKeenWheel();
	virtual void Process();
	#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	#endif	// __WORLDSERVER
};
// 머셔너리 스킬 : 스플매쉬
class CSfxSkillMerSplmash : public CSfx
{
public:
	//CSfxModel m_SfxObj2; // 명중시 이펙트 표시용
	CSfxSkillMerSplmash();
	virtual ~CSfxSkillMerSplmash();
	virtual void Process();
	#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	#endif	// __WORLDSERVER
};
// 머셔너리 스킬 : 블라인드 사이드
class CSfxSkillMerBlindSide : public CSfx
{
public:
	CSfxSkillMerBlindSide();
	virtual ~CSfxSkillMerBlindSide();
	virtual void Process();
	#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	#endif	// __WORLDSERVER
};
// 머셔너리 스킬 : 팬바리어
class CSfxSkillMerPanBarrier : public CSfx
{
public:
	//CTimer m_timer;
	//BOOL m_bSfxObj2;
	//CSfxModel m_SfxObj2;
	CSfxSkillMerPanBarrier();
	virtual ~CSfxSkillMerPanBarrier();
	virtual void Process();
	#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	#endif	// __WORLDSERVER
};
// 머셔너리 스킬 : 프로텍션
class CSfxSkillMerProtection : public CSfx
{
public:
	//CTimer m_timer;
	//BOOL m_bSfxObj2;
	//CSfxModel m_SfxObj2;
	CSfxSkillMerProtection();
	virtual ~CSfxSkillMerProtection();
	virtual void Process();
	#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	#endif	// __WORLDSERVER
};


// 매지션 주문외기
class CSfxSkillMagFireCasting : public CSfx
{
public:
	CSfxSkillMagFireCasting();
	virtual ~CSfxSkillMagFireCasting();
	virtual void Process();
	#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	#endif	// __WORLDSERVER
};
#ifdef __3RD_LEGEND16
class CSfxSkillCastingFrontDynamic : public CSfx
{
public:
	CSfxSkillCastingFrontDynamic();
	virtual ~CSfxSkillCastingFrontDynamic();
	virtual void Process();
	#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	#endif	// __WORLDSERVER
};
class CSfxSkillCastingLSideDynamic : public CSfx
{
public:
	CSfxSkillCastingLSideDynamic();
	virtual ~CSfxSkillCastingLSideDynamic();
	virtual void Process();
	#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	#endif	// __WORLDSERVER
};
class CSfxSkillAboveTargetDynamic : public CSfx
{
public:
	CSfxSkillAboveTargetDynamic();
	virtual ~CSfxSkillAboveTargetDynamic();
	virtual void Process();
	#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	#endif	// __WORLDSERVER
};
#endif // __3RD_LEGEND16
class CSfxSkillMagWindCasting : public CSfx
{
public:
	CSfxSkillMagWindCasting();
	virtual ~CSfxSkillMagWindCasting();
	virtual void Process();
	#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	#endif	// __WORLDSERVER
};

// 어시 너클추가..
#ifdef __CLIENT

class CSfxSkillAssBurstcrack : public CSfx
{
public:
	CSfxSkillAssBurstcrack();
	virtual ~CSfxSkillAssBurstcrack();
	virtual void Process();
#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
#endif	// __WORLDSERVER
};

class CSfxSkillAssTampinghole : public CSfx
{
public:
	CSfxSkillAssTampinghole();
	virtual ~CSfxSkillAssTampinghole();
	virtual void Process();
#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
#endif	// __WORLDSERVER
};

class CSfxNpcDirSteam : public CSfx
{
public:
	BOOL m_fInit;

	CSfxNpcDirSteam();
	virtual ~CSfxNpcDirSteam();
	virtual void Process();
#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
#endif	// __WORLDSERVER
};

#endif // __CLIENT
	

// 매지션 스킬 : 스트롱윈드
class CSfxSkillMagStrongWind : public CSfxShoot
{
	float m_fAngle;	
public:
	//CSfxModel m_SfxObj2; // 명중시 이펙트 표시용
	BOOL m_bHit; // 명중했는지의 여부
	CSfxSkillMagStrongWind();
	virtual ~CSfxSkillMagStrongWind();
//	int SetSfx( LPDIRECT3DDEVICE9 pd3dDevice, int nIndex, D3DXVECTOR3& vPosSrc, OBJID idSrc, D3DXVECTOR3& vPosDest, OBJID idDest, int nSec );
	virtual void Process();
	#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	#endif	// __WORLDSERVER
};
// 매지션 스킬 : 소드윈드
class CSfxSkillMagSwordWind : public CSfxShoot
{
public:
	//CSfxModel m_SfxObj2; // 명중시 이펙트 표시용
	BOOL m_bHit; // 명중했는지의 여부
	CSfxSkillMagSwordWind();
	virtual ~CSfxSkillMagSwordWind();
	virtual void Process();
	#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	#endif	// __WORLDSERVER
};
// 매지션 스킬 : 파이어부메랑
class CSfxSkillMagFireBoomerang : public CSfxShoot
{
public:
	//CSfxModel m_SfxObj2; // 명중시 이펙트 표시용
	BOOL m_bHit; // 명중했는지의 여부
	CSfxSkillMagFireBoomerang();
	virtual ~CSfxSkillMagFireBoomerang();
	virtual void Process();
	#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	#endif	// __WORLDSERVER
};
// 매지션 스킬 : 파이어봄버
class CSfxSkillMagFireBomb : public CSfx
{
public:
	CSfxSkillMagFireBomb();
	virtual ~CSfxSkillMagFireBomb();
	virtual void Process();
	#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	#endif	// __WORLDSERVER
};
// 매지션 스킬 : 핫에어
class CSfxSkillMagHotAir : public CSfxShoot
{
	int m_nDmgCnt;		// 지속데미지의 데미지 카운트. 데미지 먹일때마다 하나씩 올라간다.
public:
	//CSfxModel m_SfxObj2; // 명중시 이펙트 표시용
	CSfxSkillMagHotAir();
	virtual ~CSfxSkillMagHotAir();
	virtual void Process();
	#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	#endif	// __WORLDSERVER
};
#endif	// __WORLDSERVER



// 매지션 스킬 : 아이스미사일
class CSfxSkillMagIceMissile : public CSfxShoot
{
public:
	BOOL m_bHit; // 명중했는지의 여부
	CSfxSkillMagIceMissile();
	virtual ~CSfxSkillMagIceMissile();
	virtual void Process();
#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
#endif	// __WORLDSERVER
};

// 매지션 스킬 : 라이트닝 볼
class CSfxSkillMagLightningBall : public CSfxShoot
{
public:
	//CSfxModel m_SfxObj2; // 명중시 이펙트 표시용
	BOOL m_bHit; // 명중했는지의 여부
	CSfxSkillMagLightningBall();
	virtual ~CSfxSkillMagLightningBall();
	virtual void Process();
#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
#endif	// __WORLDSERVER
};


// 매지션 스킬 : 스파이크 스톤
class CSfxSkillMagSpikeStone : public CSfxShoot
{
public:
	//CSfxModel m_SfxObj2; // 명중시 이펙트 표시용
	BOOL m_bHit; // 명중했는지의 여부
	CSfxSkillMagSpikeStone();
	virtual ~CSfxSkillMagSpikeStone();
	virtual void Process();
#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
#endif	// __WORLDSERVER
};


#define MAX_SHOOTWAVE_TAIL		8
// 바닥에 붙어서 날아가는 이펙트.
class CSfxShootWave : public CSfxShoot
{
public:
	BOOL	m_bHit; // 명중했는지의 여부
	BOOL	m_bDir;
	DWORD	m_dwSndHit;		// 맞았을때 소리.
	FLOAT	m_fHitScale;	// 폭발씬 스케일링.
	D3DXVECTOR3 m_vTail[MAX_SHOOTWAVE_TAIL];	// 지나갔던 좌표를 차례로 쌓음.
	int		m_nTailFrame[ MAX_SHOOTWAVE_TAIL ];	// 프레임 번호.

	CSfxShootWave();
	virtual ~CSfxShootWave();
	
	void	SetHitSfx( LPCTSTR szFileName, OBJID idSrc, OBJID idDst, FLOAT fScale = 1.0f );	// 맞았을때 이펙트 등록
	void	SetHitSfx( DWORD dwIndex, OBJID idSrc, OBJID idDst, FLOAT fScale = 1.0f );	// 맞았을때 이펙트 등록
	void	SetDir( BOOL bDir );		// 방향성있는 sfx 인가.
	void	SetSndHit( DWORD dwSndHit ) { m_dwSndHit = dwSndHit; }
	
	virtual void Process();
#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
#endif	// __WORLDSERVER
};


// 고정좌표 기준 범용 sfx
class CSfxFixed : public CSfx
{
	float m_fAngle;
public:
	CSfxFixed();
	virtual ~CSfxFixed();
	virtual void Process();
#ifndef __WORLDSERVER
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
#endif	// __WORLDSERVER
};

class CSfxPartsLink : public CSfx	// 계속유지되는 이펙트		
{
public:
	BOOL	m_bEndFrame;
	int		m_nPartsLink;		// 링크될 위치. 0:오른손 1:왼손
	CSfxPartsLink() 
	{ 
		m_bEndFrame = FALSE;
		m_nPartsLink = 0; 
	}
	~CSfxPartsLink() {};
	
	void Process();
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	virtual void SetPartLink( int nPart ) { m_nPartsLink = nPart; }
	
};

// 파츠에 링크되는 sfx(블레이드의 손에 사용)
class CSfxPartsLinkBlade : public CSfxPartsLink		// 블레이드 파트 링크 이펙 : 밀리 모션이 끝나면 소멸
{
public:
	CSfxPartsLinkBlade() {};
	~CSfxPartsLinkBlade() {};
	
	void Process();
};

class CSfxPartsLinkJst : public CSfxPartsLink		// 제스터 파츠 링크 이펙 : 이펙트가 EndFrame이면 소멸
{
public:
	CSfxPartsLinkJst() {};
	~CSfxPartsLinkJst() {};
	
	void Process();
};



// 파츠에 링크되는 sfx(아크로뱃의 손에 사용)
class CSfxAllowPartsLink : public CSfx
{
public:
	int		m_nPartsLink;		// 링크될 위치. 0:오른손 1:왼손
	CSfxAllowPartsLink() { m_nPartsLink = 0; }
	~CSfxAllowPartsLink() {};
	
	void Process();
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	virtual void SetPartLink( int nPart ) { m_nPartsLink = nPart; }
	
};

#ifndef __WORLDSERVER
class CSfxPartsLinkShoulder : public CSfxPartsLink		// 블레이드 파트 링크 이펙 : 밀리 모션이 끝나면 소멸
{
public:
	int				m_nOldAbilityOption;
	D3DXVECTOR3		m_v3Offset;

	CSfxPartsLinkShoulder();
	~CSfxPartsLinkShoulder();

	void Process();
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
};
#endif //__WORLDSERVER

// 아이템 시전 이펙트
class CSfxReady : public CSfx
{
public:
	CSfxReady() {}
	~CSfxReady() {}

	void Process();
};

#if __VER >= 12 // __ITEMCREATEMON_S0602
class CSfxCursor : public CSfx
{
public:
	CSfxCursor();
	virtual ~CSfxCursor();
	virtual void Process();
};
#endif // __ITEMCREATEMON_S0602

class CSfxLinkMover : public CSfx			
{
public:
	//gmpbigsun : 무버에 붙어 다니는 일반적인 상황연출용
	CSfxLinkMover( );
	virtual ~CSfxLinkMover( );
	virtual void Process( );
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );

protected:
	DWORD  _idSFX;
};

#endif // !defined(AFX_SFX_H__D652787A_4E67_419F_AB52_0A8E2FED08AE__INCLUDED_)
