// AttackArbiter.h: interface for the CAttackArbiter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ATTACKARBITER_H__AC3D1A9E_3828_4E22_94B4_DACA62D3AE0E__INCLUDED_)
#define AFX_ATTACKARBITER_H__AC3D1A9E_3828_4E22_94B4_DACA62D3AE0E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CActionMover;



class CAttackArbiter  
{
public:
	CAttackArbiter( DWORD dwMsg, CMover* pAttacker, CMover* pDefender, DWORD dwAtkFlags, int nParam, BOOL bTarget, int nReflect );
	virtual ~CAttackArbiter();

private:
	DWORD			m_dwMsg;
	CMover*			m_pAttacker;		// 공격자 
	CMover*			m_pDefender;		// 피격자 
	DWORD			m_dwAtkFlags; 
	int				m_nParam;
	BOOL			m_bTarget;			// 타켓팅되었는가? - 광역일 경우 m_bTarget가 FALSE이면 주변에 있는 넘
	int				m_nReflect;			// 반사데미지 0 일반, 1 : 당한넘(공격한넘), 2 : 준넘(방어한넘)
#if __VER >= 11 // __REFLECTDMG_AFTER
	int m_nReflectDmg;
#endif // __REFLECTDMG_AFTER

public:
	int				OnDamageMsgW();

private:
	BOOL			CheckValidDamageMsg();
	int				CalcDamage( ATTACK_INFO* pInfo );
	void			ChanceSkill( int nDestParam, CCtrl *m_pAttacker, CCtrl *m_pDefender );
	void			ProcessAbnormal( int nDamage, ATTACK_INFO* pInfo );
	int				MinusHP( int *pnDamage );
#if __VER >= 9 // __ULTIMATE
	void			StealHP( int nDamage, ATK_TYPE type );
#else // __ULTIMATE
	void			StealHP( int nDamage );
#endif // __ULTIMATE	
	void			ChangeExpRatio();
	void			OnDamaged( int nDamage );
	void			OnDied();

#if __VER >= 8     // 8차 듀얼존에 관계없이 PVP가능하게함   Neuz, World
	void			OnDiedPVP();
#endif	// __VER >= 8  

	void			OnDiedSchoolEvent();
	int				PostAsalraalaikum();
	BYTE			GetHandFlag( void );
	int				CalcATK( ATTACK_INFO* pInfo );
	int				PostCalcDamage( int nATK, ATTACK_INFO* pInfo );
	int				OnAfterDamage( ATTACK_INFO* pInfo, int nDamage );
};


#endif // !defined(AFX_ATTACKARBITER_H__AC3D1A9E_3828_4E22_94B4_DACA62D3AE0E__INCLUDED_)
