#ifndef	__DPACCOUNTCLIENT_H__
#define	__DPACCOUNTCLIENT_H__

#include "dpmng.h"
#include "msghdr.h"

#undef	theClass
#define theClass	CDPAccountClient
#undef theParameters
#define theParameters	CAr & ar, LPBYTE lpBuf, u_long uBufSize

class CDPAccountClient : public CDPMng
{
public:
/*
#ifdef __S0114_RELOADPRO
	SET_STRING		m_OutAccount_List;
	CTime			m_tMemDelete;
	BOOL			m_bMemDelete;
#endif // __S0114_RELOADPRO
*/
//	Constructions
	CDPAccountClient();
	virtual	~CDPAccountClient();
//	Overrides
	virtual	void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual	void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
//	Operations
	void	SendAddIdofServer( DWORD dwIdofServer );
	void	SendRemoveAccount( char* lpString );
	void	SendRemoveAllAccounts( void );
	void	SendBuyingInfo( PBUYING_INFO2 pbi2, SERIALNUMBER iSerialNumber );
#ifdef __SERVERLIST0911
	void	SendServerEnable( int nMulti, long lEnable );
#endif	// __SERVERLIST0911
/*
#ifdef __S0114_RELOADPRO
	void	SendCompleteReloadProject();
	void	ReloadProject();
#endif // __S0114_RELOADPRO
*/
//	Handlers
	USES_PFNENTRIES;
	void	OnGetPlayerList( CAr & ar, LPBYTE, u_long );
#ifdef __REMOVE_PLAYER_0221
	void	OnRemovePlayer( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
#endif	// __REMOVE_PLAYER_0221
	void	OnJoin( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnPlayerCount( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnCloseExistingConnection( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnOneHourNotify( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnFail( CAr & ar, LPBYTE, u_long );
	void	OnBuyingInfo( CAr & ar, LPBYTE lpBuf, u_long uBufSize );	// 2
/*
#ifdef __S0114_RELOADPRO
	void	OnReloadProject( CAr& ar, LPBYTE lpBuf, u_long uBufSize );
#endif // __S0114_RELOADPRO
*/
};

#endif	// __DPACCOUNTCLIENT_H__