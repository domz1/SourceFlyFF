#ifndef __NPCHECKER_H__
#define	__NPCHECKER_H__

#define	MAX_LEN_MOVER_MENU	1024	// 32
#define	MAX_NPC_RADIUS	9	// 3

class CNpcChecker
{
public:
	CNpcChecker();
	virtual	~CNpcChecker();
	static	CNpcChecker*	GetInstance( void );
	void	AddNpc( CObj* pObj );
	void	RemoveNpc( CObj* pObj );
	BOOL	IsCloseNpc( int nMenu, CWorld* pWorld, D3DXVECTOR3 & v );
	BOOL	IsCloseNpc( CWorld* pWorld, D3DXVECTOR3 & v );
private:
	set<OBJID>	m_aset[MAX_MOVER_MENU];
};

#endif	// __NPCHECKER_H__