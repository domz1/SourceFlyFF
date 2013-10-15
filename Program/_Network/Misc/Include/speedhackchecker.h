#ifndef __SPEEDHACKCHECKER_H__
#define	__SPEEDHACKCHECKER_H__

#include "dpmng.h"
#include "msghdr.h"

class CSpeedHackChecker
{
private:
//	time_t	m_tBaseOfServer;
//	time_t	m_tBaseOfClient;
	DWORD m_tBaseOfServer;
	DWORD m_tBaseOfClient;
	BOOL	m_fKill;
public:
//	Constructions
	CSpeedHackChecker();
	~CSpeedHackChecker();
//	Operations
	void	sETbaseOfServer( CDPMng* pDPMng, DPID dpidPlayer );
//	void	sETbaseOfClient( time_t tBaseOfClient );
	void	sETbaseOfClient( DWORD tBaseOfClient );
	void	rEQoffsetOfClient( CDPMng* pDPMng, DPID dpidPlayer );
//	BOOL	CheckClock( time_t tCurClient, CDPMng* pDPMng, DPID dpidPlayer );
	BOOL	CheckClock( DWORD tCurClient, CDPMng* pDPMng, DPID dpidPlayer );
	BOOL	fsETbaseOfClient( void )	{	return(BOOL)m_tBaseOfClient;	}
	void	Kill( void )	{	m_fKill	= TRUE;	}
};

#endif	// __SPEEDHACKCHECKER_H__