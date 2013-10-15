#ifndef __SHIP_H__
#define __SHIP_H__

#include "Ctrl.h"
#include "..\_AIInterface\ActionShip.h"

#define MAX_LINKCTRL	128

/// 비공정 
class CShip : public CCtrl
{
private:
	CMover *m_pMover;			// 이 배의 쥔장. 조종하는 사람.
	CActionShip m_Act;
	
protected:
	int		m_nCount;
	D3DXVECTOR3		m_vDelta;
	D3DXVECTOR3		m_vDeltaUnit, m_vAcc;		// 관성, 매프레임 힘
	FLOAT			m_fDeltaAng;
	FLOAT	m_fAccAng;		// 회전 가속도.
	OBJID	m_LinkCtrl[ MAX_LINKCTRL ];
	
	virtual void Init( void );
	virtual void Destroy( void );
public:
	friend CActionShip;
	
	CShip();
	virtual ~CShip();
	
	CMover *GetMover( void ) { return m_pMover; }				// 이 배의 쥔장을 리턴함.
	void SetMover( CMover *pMover ) { m_pMover = pMover; }		// 배 조종하는 사람을 세팅.
	int		SendActMsg( OBJMSG dwMsg, int nParam1 = 0, int nParam2 = 0, int nParam3 = 0 )
	{
		return m_Act.SendActMsg( dwMsg, nParam1, nParam2, nParam3 );
	}
	void	AddCtrl( OBJID idCtrl );
	OBJID*	FindCtrl( OBJID idCtrl );
	void	RemoveCtrl( OBJID idCtrl );
	D3DXVECTOR3 GetDelta( void ) { return m_vDelta; }
	FLOAT		GetDeltaAng( void ) { return m_fDeltaAng; }

	void Control( void );
	virtual void Process();
		
};

// 플라리스 위에서 뺑뺑이 도는 배
class CShipLoop : public CShip
{
private:
	
protected:
	void Init( void );
	void Destroy( void );
public:
	
	CShipLoop();
	virtual ~CShipLoop();
	
	void Process();
	
};


#ifdef __CLIENT
extern CShip *g_pShip;
#endif


#endif // SHIP_h