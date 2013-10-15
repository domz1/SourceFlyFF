#ifndef __ACTIONSHIP_H__
#define __ACTIONSHIP_H__

#include "Action.h"

class CShip;

class CActionShip : public CAction
{
private:
	CShip			*m_pShip;

	void Init( void );
	void Destroy( void );

public:
	CActionShip();
	virtual	~CActionShip();

	CShip*	GetShip( void ) { return m_pShip; }
	void	SetShip( CShip *pShip ) { m_pShip = pShip; }
	
	int		Process( void );
	int		SendActMsg( OBJMSG dwMsg, int nParam1 = 0, int nParam2 = 0, int nParam3 = 0 );
	void	ProcessState( DWORD dwState, float fSpeed = 0.0f );
	
};

#endif // actionship.h