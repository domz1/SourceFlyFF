#ifndef __DPLOGINSRVR_H__
#define __DPLOGINSRVR_H__

#pragma once

#include "DPMng.h"
#include "MsgHdr.h"

#undef	theClass
#define theClass	CDPLoginSrvr
#undef	theParameters
#define theParameters	CAr & ar, DPID, LPBYTE lpBuf, u_long uBufSize

//class CAccount;
class CDPLoginSrvr : public CDPMng
{
public:
//	Constructions
	CDPLoginSrvr();
	virtual	~CDPLoginSrvr();
	
//	Operations
	virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );

//	void	SendPlayerList( CAccount* pAccount, DPID dpid );
	void	SendPlayerList( const char* lpszAccount, const char* lpszPassword, DPID dpid, DWORD dwAuthKey );
	void	SendError( LONG lError, DPID dpid );	//
	void	SendCloseExistingConnection( const char* lpszAccount, LONG lError );
	void	SendFail( long lError, const char*lpszAccount );
	void	SendOneHourNotify( const char* lpszAccount);

	void	OnAddConnection( DPID dpid );
	void	OnRemoveConnection( DPID dpid );

	USES_PFNENTRIES;
public:
//	Handlers
	void	OnGetPlayerList( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
	void	OnCreatePlayer( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
	void	OnRemovePlayer( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
	void	OnLeave( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
	void	OnCloseError( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
	void	OnLoginProtect( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
	void	SendLoginProtect( const char* lpszAccount, const char* lpszPlayer, u_long idPlayer, BOOL bLogin, DPID dpId );
#endif // __2ND_PASSWORD_SYSTEM
};

#endif	// __DPLOGINSRVR_H__