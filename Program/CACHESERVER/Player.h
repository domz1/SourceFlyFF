#ifndef __PLAYER_H__
#define __PLAYER_H__

#pragma once

#include <map>
using	namespace	std;
#include "dpclient.h"

class CPlayer
{
public:
	CPlayer( DPID dpidUser, DWORD dwSerial );
	virtual	~CPlayer();
	DWORD	GetAuthKey( void )	{	return m_dwAuthKey;		}
	const char*	GetPlayer( void )	{	return m_szPlayer;	}
	const char*	GetAccount( void )	{	return m_szAccount;	}
	const char*	GetPass( void )		{	return m_szPass;	}
	CDPClient*	GetClient( void )	{	return m_pClient;	}
	void	SetClient( CDPClient* pClient )		{	m_pClient	= pClient;	}
	BOOL	IsAlive( void )	{	return m_bAlive;	}
	void	SetAlive( BOOL bAlive )	{	m_bAlive	= bAlive;	}
	DPID	GetNetworkId( void )	{	return m_dpid;	}
	u_long	GetPlayerId( void )	{	return m_idPlayer;	}
	DWORD	GetSerial( void )	{	return m_dwSerial;	}
	BOOL	IsTimeover( DWORD dwTick )		{	return !m_pClient && m_dwCreation < dwTick;	}
	const char*		GetAddr( void )		{	return m_lpAddr;	}
	DWORD	GetWorld( void )	{	return m_dwWorldId;		};
	u_long		GetChannel( void )	{	return m_uChannel;	}
	u_long	GetParty( void )	{	return m_idParty;	}
	u_long	GetGuild( void )	{	return m_idGuild;	}
	u_long GetWar( void )	{	return m_idWar;	}
	BYTE	GetSlot( void )		{	return m_nSlot;		}
	void	Join( CAr & ar );
	void	SetAddr( CDPMng* pdpMng )	{	pdpMng->GetPlayerAddr( GetNetworkId(), m_lpAddr );	}
private:
	const	DWORD	m_dwSerial;
	const	DPID	m_dpid;
	DWORD	m_dwAuthKey;
	char	m_szPlayer[MAX_PLAYER];
	char	m_szAccount[MAX_ACCOUNT];
	char	m_szPass[MAX_PASSWORD];
	CDPClient*	m_pClient;
	BOOL	m_bAlive;
	u_long	m_idPlayer;
	const	DWORD	m_dwCreation;
	char	m_lpAddr[16];
	DWORD	m_dwWorldId;
	u_long	m_uChannel;
	u_long	m_idParty;
	u_long	m_idGuild;
	u_long	m_idWar;
	BYTE	m_nSlot;
};

typedef	map<DPID, CPlayer*>	MPP;
class CPlayerMng
{
public:
	CMclCritSec	m_AddRemoveLock;

public:
	CPlayerMng();
	virtual	~CPlayerMng();
	void	Clear( void );
	BOOL	AddPlayer( DPID dpid );
	BOOL	RemovePlayer( DPID dpid );
	CPlayer*	GetPlayer( DPID dpid );
	CPlayer*	GetPlayerBySerial( DWORD dwSerial );
	int		GetCount( void )	{	return m_mapPlayers.size();		}
	void	DestroyPlayer( CDPClient* pClient );
	void	DestroyGarbage( void );
	static	CPlayerMng*	Instance( void );
private:
	void	DestroyPlayersOnChannel( CDPClient* pClient );
	void	DestroyAllPlayers( void );
	void	DestroyGarbagePlayer( CPlayer* pPlayer );
	void	SendKeepAlive( CPlayer* pPlayer );
private:
	long	m_lSerial;
	MPP		m_mapPlayers;
};

#endif	// __PLAYER_H__