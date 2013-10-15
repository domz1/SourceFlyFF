#ifndef __DPBillingTH_H__
#define	__DPBillingTH_H__

#include "dpmng.h"
#include "msghdr.h"
#include "misc.h"
#include <map>

using	namespace	std;

#undef	theClass
#define theClass	CDPBillingTH
#undef theParameters
#define theParameters	CAr & ar, DPID


class CDPBillingTH : public CDPMng<CBuffer2>
{
public:
//	Constructions
	CDPBillingTH();
	virtual	~CDPBillingTH();

private:
	HWND	m_hWnd;
	int		m_nKeepAlive;

public:
	BOOL	m_bConnected;

public:
//	Overrides
	virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );

// Operations
	void	Init( HWND hWnd ) { m_hWnd = hWnd; }
	void	ReqBillingInfo( LPCTSTR lpszUserID, LPCTSTR lpszUserIP, DWORD dwSession );
	void	SendKeepAlive();
	void	NotifyLogout( LPCTSTR lpszAccount, DWORD dwSession );

//	Handlers
	USES_PFNENTRIES;
	void	AckKeepAlive( CAr & ar, DPID dpid );
	void	AckBillingInfo( CAr & ar, DPID dpid );
	void	ReqIsAlive( CAr & ar, DPID dpid );
};

#endif	// __DPBillingTH_H__