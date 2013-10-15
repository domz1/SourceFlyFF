#ifndef __DPCORECLIENT_H__
#define __DPCORECLIENT_H__

#pragma once

#include "DPMng.h"

#include "rtmessenger.h"

#undef	theClass
#define	theClass	CDPCoreClient
#undef theParameters
#define theParameters	CAr & ar, DPID dpidUser

class CPlayer;
class CDPCoreClient : public CDPMng
{
public:
//	Constructions
	CDPCoreClient();
	virtual	~CDPCoreClient();
//	Overrides
	virtual	void SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual void UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
//	Operations
	void	SendToServer( DPID dpidUser, LPVOID lpMsg, DWORD dwMsgSize );
	void	SendAddPlayer( CPlayer* pPlayer, CRTMessenger & rtmessenger );
	void	SendDestroyPlayer( CPlayer* pPlayer );
// Handlers
	USES_PFNENTRIES;
protected:
	void	OnProcServerList( CAr & ar, DPID );
	void	OnProcServer( CAr & ar, DPID );
	void	OnLeave( CAr & ar, DPID dpid );
	void	OnDestroyAllPlayers( CAr & ar, DPID dpid );
	void	OnJoin( CAr & ar, DPID dpid );
//	Operator commands
	void	OnSay( CAr & ar, DPID dpid );
	void	OnKillPlayer( CAr & ar, DPID dpid );
};


#endif	// __DPCORECLIENT_H__