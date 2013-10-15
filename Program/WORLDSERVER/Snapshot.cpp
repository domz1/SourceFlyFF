#include "stdafx.h"
#include "Snapshot.h"

CSnapshot::CSnapshot()
{
	dpidCache	= dpidUser	= DPID_UNKNOWN;
	cb	= 0;
}

CSnapshot::CSnapshot( DPID idCache, DPID idUser, OBJID objid, DWORD dwHdr )
{
	SetSnapshot( objid, dwHdr );	
}

CSnapshot::~CSnapshot()
{
	ar.Flush();
}

void CSnapshot::SetSnapshot( OBJID objid, DWORD dwHdr )
{
	ASSERT( DPID_UNKNOWN != dpidUser );
	cb	= 0;
	ar.Flush();
	ar << dpidUser << dwHdr << objid << cb;
}