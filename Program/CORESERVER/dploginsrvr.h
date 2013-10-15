#ifndef __DPLOGINSRVR_H__
#define __DPLOGINSRVR_H__

#pragma once

#include "DPMng.h"

#undef	theClass
#define theClass	CDPLoginSrvr
#undef	theParameters
#define theParameters	CAr & ar

class CDPLoginSrvr : public CDPMng
{
public:
//	Constructions
	CDPLoginSrvr();
	virtual	~CDPLoginSrvr();
//	Overrides
	virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
//	Operations

	void	SendQueryRemovePlayer( const CHAR* lpszAccount );
//	Handlers
	USES_PFNENTRIES;

	void	OnAddConnection( DPID dpid );
	void	OnRemoveConnection( DPID dpid );

	void	OnQueryTickCount( CAr & ar );
	void	OnPreJoin( CAr & ar );
	void	OnLeave( CAr & ar );
};

#endif	// __DPLOGINSRVR_H__