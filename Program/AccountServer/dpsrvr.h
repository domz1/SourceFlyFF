#ifndef __DPSRVR_H__
#define	__DPSRVR_H__

#include "dpmng.h"
#include "msghdr.h"
#include "misc.h"
#include <map>

using	namespace	std;

#undef	theClass
#define theClass	CDPSrvr
#undef theParameters
#define theParameters	CAr & ar, DPID, DPID

#define MAX_IP 10240

typedef map<string, int>	STRING2INT;

class CDPSrvr : public CDPMng<CBuffer>
{
public:
	bool	m_bCheckAddr;		// 접속하는 account의 address를 검사해야 하는가?
	int		m_nMaxConn;
	BOOL	m_bReloadPro;
	CMclCritSec		m_csAddrPmttd;
	char	m_sAddrPmttd[128][16];
	int		m_nSizeofAddrPmttd;
	

	CMclCritSec		m_csIPCut;
	STRING2INT m_sIPCut;
	int		m_nIPCut[MAX_IP][3];
	int		m_nSizeofIPCut;

	DWORD	m_dwSizeofServerset;
	SERVER_DESC		m_aServerset[128];
	map<u_long, LPSERVER_DESC>	m_2ServersetPtr;
	char	m_szVer[32];
#ifdef __SECURITY_0628
	char	m_szResVer[100];
#endif	// __SECURITY_0628

public:
//	Constructions
	CDPSrvr();
	virtual	~CDPSrvr();
//	Overrides
	virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
// Operations
	void	CloseExistingConnection( LPCTSTR lpszAccount, LONG lError );
	bool	LoadAddrPmttd( LPCTSTR lpszFileName );
	BOOL	LoadIPCut( LPCSTR lpszFileName );
	void	DestroyPlayer( DPID dpid1, DPID dpid2 );

	void	OnAddConnection( DPID dpid );
	void	OnRemoveConnection( DPID dpid );

	void	SendServersetList( DPID dpid );
	void	SendPlayerCount( u_long uId, long lCount );

	BOOL	EnableServer( DWORD dwParent, DWORD dwID, long lEnable );
	void	SendEnableServer( u_long uId, long lEnable );
#ifdef __GPAUTH_02
#ifdef __EUROPE_0514
	void	OnAfterChecking( BYTE f, LPCTSTR lpszAccount, DPID dpid1, DPID dpid2, DWORD dwAuthKey, BYTE cbAccountFlag, long lTimeSpan, const char* szCheck, const char* szBak );
#else	// __EUROPE_0514
	void	OnAfterChecking( BYTE f, LPCTSTR lpszAccount, DPID dpid1, DPID dpid2, DWORD dwAuthKey, BYTE cbAccountFlag, long lTimeSpan, const char* szCheck );
#endif	// __EUROPE_0514
#else	// __GPAUTH_02
	void	OnAfterChecking( BYTE f, LPCTSTR lpszAccount, DPID dpid1, DPID dpid2, DWORD dwAuthKey, BYTE cbAccountFlag, long lTimeSpan );
#endif	// __GPAUTH_02

	BOOL	IsABClass( LPCSTR lpszIP );
	void	GetABCClasstoString( LPCSTR lpszIP, char * lpABClass, int &nCClass );
	void	InitIPCut( void );
//	Handlers
	USES_PFNENTRIES;
	void	OnAddAccount( CAr & ar, DPID dpid1, DPID dpid2 );
	void	OnRemoveAccount( CAr & ar, DPID dpid1, DPID dpid2 );
	void	OnPing( CAr & ar, DPID dpid1, DPID dpid2 );
	void	OnCloseExistingConnection( CAr & ar, DPID dpid1, DPID dpid2 );
};

#endif	// __DPCRTFRSRVR_H__