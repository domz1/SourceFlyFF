#ifndef __DPBILLING_H__
#define	__DPBILLING_H__

#include "dpmng.h"
#include "msghdr.h"
#include "misc.h"
#include <map>

using	namespace	std;

#undef	theClass
#define theClass	CDPBilling
#undef theParameters
#define theParameters	CAr & ar, DPID

// 일본 빌링통신용 
class CDPBilling : public CDPMng<CBuffer2>
{
private:
	HWND	m_hWnd;
public:
	BOOL	m_bConnected;
	
public:
//	Constructions
	CDPBilling();
	virtual	~CDPBilling();
//	Overrides
	virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
// Operations
	void	Init( HWND hWnd ) { m_hWnd = hWnd; }
	void	SendGameStart( LPCTSTR lpszUserID, LPCTSTR lpszUserIP );
	void	SendKeepAlive( );
	void	SendGSCSStart( );
//	Handlers
	USES_PFNENTRIES;
	void	OnKeepAliveReceive( CAr & ar, DPID dpid );
	void	OnGameStartReceive( CAr & ar, DPID dpid );
};

#endif	// __DPBILLING_H__