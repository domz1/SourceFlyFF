#ifndef __DPACCOUNTCLIENT_H__
#define	__DPACCCOUNTCLIENT_H__

#include "dpmng.h"
#include "msghdr.h"

#undef	theClass
#define theClass	CDPAccountClient
#undef theParameters
#define theParameters	CAr & ar, DPID

class CDPAccountClient : public CDPMng
{
public:
//	Constructions
	CDPAccountClient();
	virtual	~CDPAccountClient();
//	Overrides
	virtual	void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual	void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
//	Operations

#ifdef __GPAUTH_02
#ifdef __EUROPE_0514
#if __VER >= 14 // __PCBANG
	void SendAddAccount( char* lpAddr, LPCTSTR lpszAccount, BYTE cbAccountFlag, DPID idFrom, int fCheck, const char* szCheck, const char* szBak, DWORD dwPCBangClass );
#else // __PCBANG
	void	SendAddAccount( char* lpAddr, LPCTSTR lpszAccount, BYTE b18, DPID idFrom, int fCheck, const char* szCheck, const char* szBak );
#endif // __PCBANG
#else	// __EUROPE_0514
	void	SendAddAccount( char* lpAddr, LPCTSTR lpszAccount, BYTE b18, DPID idFrom, int fCheck, const char* szCheck );
#endif	// __EUROPE_0514
#else	// __GPAUTH_02
	void	SendAddAccount( char* lpAddr, LPCTSTR lpszAccount, BYTE b18, DPID idFrom, int fCheck );
#endif	// __GPAUTH_02

	void	SendRemoveAccount( DPID idFrom );
	void	SendRoute( DWORD dwIdofServer, u_long uIdofMulti, DPID idFrom );
	void	SendPing( DPID idFrom );
	void	SendCloseExistingConnection( const char* lpszAccount );

	USES_PFNENTRIES;
//	Handlers
	void	OnAddAccount( CAr & ar, DPID dpid );
	void	OnDestroyPlayer( CAr & ar, DPID dpid );
	void	OnServersetList( CAr & ar, DPID dpid );
	void	OnPlayerCount( CAr & ar, DPID dpid );

	void	OnEnableServer( CAr & ar, DPID dpid );
};

#endif	// __DPACCOUNTCLIENT_H__