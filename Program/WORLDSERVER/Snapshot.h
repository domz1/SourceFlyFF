#ifndef __SNAPSHOT_H__
#define	__SNAPSHOT_H__

#pragma once

#include "Ar.h"
//#include "BLQueue.h"
#include "mempooler.h"
#include <DPlay.h>

class CSnapshot
{
public:
//	static CMemPool<CSnapshot>*	m_pPool;

public:
	DPID	dpidCache;
	DPID	dpidUser;
	CAr		ar;
	short	cb;
public:
	// Constructions
	CSnapshot();
	CSnapshot( DPID idCache, DPID idUser, OBJID objid, DWORD dwHdr );
	virtual	~CSnapshot();

	// Operations
	void	SetSnapshot( OBJID objid, DWORD dwHdr );
	void	Flush( void )
		{
			dpidCache	= dpidUser	= DPID_UNKNOWN;
			ar.Flush();
			cb	= 0;
		}
};
#endif	// __SNAPSHOT_H__