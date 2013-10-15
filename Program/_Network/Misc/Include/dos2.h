#ifndef __DOS2_H__
#define	__DOS2_H__

#pragma once

#include <map>
#include "mempooler.h"

using	namespace	std;

typedef struct	_CONNECTIONREQUEST2
{
	u_long	ip;
	DWORD	dwHit;
	int	nHit;
	BOOL	b;
}
CONNECTIONREQUEST2,	*PCONNECTIONREQUEST2;

#define	MAX_ADDR	1024
#undef MAX_HIT
#define	MAX_HIT		50

class CDos2 : public map<u_long, PCONNECTIONREQUEST2>
{
public:
	CDos2();
	virtual	~CDos2();
	int		IsOver( u_long addr );
private:
	CTimer	m_timer;
	DWORD	m_adwAddr[MAX_ADDR];
	int		m_nAddr;
	MemPooler<CONNECTIONREQUEST2>*	m_pPool;
	void	Add( u_long addr );
	PCONNECTIONREQUEST2		Get( u_long addr );
	void	Release( void );
};

#endif	// __DOS2_H__
