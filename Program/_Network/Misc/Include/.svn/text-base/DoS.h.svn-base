#ifndef __DOS_H__
#define	__DOS_H__

#pragma once

#include <map>
#include <deque>
#include <mmsystem.h>
#include "mempooler.h"

using	namespace	std;

typedef	struct	_CONNECTIONREQUEST
{
	u_long	ip;
	DWORD	dwHit;
	int	nHit;
	BOOL	bBlock;
}	CONNECTIONREQUEST, *PCONNECTIONREQUEST;

typedef	map<u_long, PCONNECTIONREQUEST>	ULONGMAP;
typedef deque<u_long, allocator<u_long> >	ULONGDEQUE; 

#define	MAX_QUEUEITEM	8196
#define	MAX_HIT		4

class CDoS : public ULONGMAP
{
public:
//	Constructions
	CDoS();
	virtual	~CDoS();
//	Operations
	BOOL	IsDoS( u_long addr );
private:
	PCONNECTIONREQUEST	Get( u_long addr );
	void	Add( u_long addr );
private:
	MemPooler<CONNECTIONREQUEST>*	m_pPool;
	ULONGDEQUE	m_q;
};

#endif	// __DOS_H__