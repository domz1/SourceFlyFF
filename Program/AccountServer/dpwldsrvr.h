#ifndef __DPWLDSRVR_H__
#define	__DPWLDSRVR_H__

#include "dpmng.h"
#include "msghdr.h"

#undef	theClass
#define theClass	CDPWldSrvr
#undef theParameters
#define theParameters	CAr & ar, DPID dpid

class CDPWldSrvr : public CDPMng<CBuffer>
{
public:
//	Constructions
	CDPWldSrvr();
	virtual	~CDPWldSrvr();

//	Overrides
	virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpId );
	virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpId );

	static	CDPWldSrvr*		GetInstance( void );
//	Handlers
	USES_PFNENTRIES;
	void	OnUpload( CAr & ar, DPID dpid );
	void	OnQuery( CAr & ar, DPID dpid );
	void	OnRestore( CAr & ar, DPID dpid );
};

#endif	// __DPWLDSRVR_H__