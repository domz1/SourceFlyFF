#ifndef __DPCLIENT_H__
#define __DPCLIENT_H__

#pragma once

#include "DPMng.h"
#include "ServerDesc.h"

#undef	theClass
#define	theClass	CDPClient
#undef theParameters
#define theParameters	CAr & ar, DPID dpidUser, LPVOID lpBuffer, u_long uBufSize

class CPlayer;
class CDPClient : public CDPMng
{
public:
	CServerDesc*	m_pServer;
	CDPClient*	pNext;

public:
	// Constructions
	CDPClient();
	virtual	~CDPClient();

	// Operations
	virtual	void SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual void UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );

	void	SendToServer( DPID dpidUser, LPVOID pData, DWORD dwDataSize )
		{
			*reinterpret_cast<UNALIGNED DPID*>( pData )	= dpidUser;
			Send( pData, dwDataSize, DPID_SERVERPLAYER );
		}

	void	SendJoin( CPlayer* pPlayer );

	USES_PFNENTRIES;

	// Handlers
//	void	OnControl( CAr & ar, DPID dpidUser, LPVOID lpBuffer, u_long uBufSize );
	void	OnReplace( CAr & ar, DPID dpidUser, LPVOID lpBuffer, u_long uBufSize);
//	void	OnDestroyPlayer( CAr & ar, DPID dpidUser, LPVOID lpBuffer, u_long uBufSize);
	void	OnQueryDestroyPlayer( CAr & ar, DPID dpidUser, LPVOID lpBuffer, u_long uBufSize);
};

class CDPClientArray
{
public:
	CDPClient*	m_pFirstActive;
	CDPClient*	m_pFirstFree;
	CMclCritSec		m_AddRemoveLock;
	CServerDescArray	m_apServer;

public:
	// Constructions
	CDPClientArray();
	virtual	~CDPClientArray();

	// Operations
	void	Free( void );
	BOOL	Connect( CServerDesc* pServer );

	void	Add( CDPClient* pClient );
	BOOL	Remove( CDPClient* pRemove );
	CDPClient*	GetClient( u_long uIdofMulti, DWORD dwWorldID, const D3DXVECTOR3 & vPos );
	void	SendToServer( DPID dpidUser, LPVOID lpMsg, DWORD dwMsgSize );
};

#endif	// __DPCLIENT_H__
