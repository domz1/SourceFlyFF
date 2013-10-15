#ifndef __DPMONITORCLIENT_H__
#define	__DPMONITORCLIENT_H__

#include "dpmng.h"
#include "msghdr.h"

#undef	theClass
#define theClass	CDPMonitorClient
#undef theParameters
#define theParameters	CAr & ar

class CDPMonitorClient : public CDPMng
{
private:
	CTimer	m_tmPing;
	CRITICAL_SECTION m_cs;
public:
//	Constructions
	CDPMonitorClient();
	virtual	~CDPMonitorClient();
//	Overrides
	virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpid );
	virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpid );
//	Operations
	BOOL	Run( LPCSTR lpAddr, u_long uPort, DWORD dwMonitorId, BOOL fsndPing = FALSE );
	void	Ping( void );

	void	SendHdr( DWORD dwHdr );
};

inline void CDPMonitorClient::SendHdr( DWORD dwHdr )
{
	BEFORESEND( ar, dwHdr );
	SEND( ar, this, DPID_SERVERPLAYER );
}

#endif	// __DPMONITORCLIENT_H__