#ifndef __DPACCOUNTCLIENT_H__
#define __DPACCOUNTCLIENT_H__

#include "DPMng.h"
#include "MsgHdr.h"

#undef	theClass
#define theClass	CDPAccountClient
#undef theParameters
#define theParameters CAr & ar

class CDPAccountClient : public CDPMng
{
public:
//	Constructions
	CDPAccountClient();
	virtual	~CDPAccountClient();
//	Override
	virtual	void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual	void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
//	Operations
	static	CDPAccountClient*	GetInstance( void );
protected:
	// Handlers
	USES_PFNENTRIES;
	void OnQueryGlobalGiftbox( CAr & ar );
};

#endif	// __DPACCOUNTCLIENT_H__