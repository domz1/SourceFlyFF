#ifndef __PLAYER_H__
#define __PLAYER_H__

#pragma once

#include "mempooler.h"
#include "CMclCritSec.h"
#ifdef __RT_1025
#include "rtmessenger.h"
#else	// __RT_1025
#include "messenger.h"
#endif	// __RT_1025

enum TAG_RESULT 
{
	TAG_NOTFRIEND, 
	TAG_BLOCKED, 
	TAG_OK
};

class CPlayer
{
public:
	CMclCritSec		m_AccessLock;
	DWORD			m_dwSerial;
	DPID			dpid;
	DPID			dpidCache;
	DPID			dpidUser;
	u_long			uKey;						// uPlayerId
	CHAR			lpszPlayer[MAX_PLAYER];		// name
	CHAR			lpAddr[16];					// ip
	CHAR			lpszAccount[MAX_ACCOUNT];	// account
#ifdef __RT_1025
	CRTMessenger	m_RTMessenger;
#else	// __RT_1025
	CMessenger		m_Messenger;
#endif	// __RT_1025
	u_long			m_uPartyId;	
	u_long			m_idGuild;
	u_long			m_idWar;
	u_long			m_uIdofMulti;
	CTime			m_tGuildMember;
	u_long			m_idSnoop;

public:
	CPlayer( u_long idPlayer, const CHAR* pszPlayer, const CHAR* pszAccount );
	virtual	~CPlayer();

	void		Lock()		{ m_AccessLock.Enter();	}
	void		Unlock()	{ m_AccessLock.Leave();	}
	TAG_RESULT	IsTagSendable( u_long idTo );

public:
#ifndef __VM_0820
#ifndef __MEM_TRACE
	static	MemPooler<CPlayer>*	m_pPool;
	void*	operator new( size_t nSize )	{	return CPlayer::m_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CPlayer::m_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CPlayer::m_pPool->Free( (CPlayer*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CPlayer::m_pPool->Free( (CPlayer*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820
};


typedef map< u_long, CPlayer* >		ULONG2PTR;

class CPlayerMng
{
public:
	CMclCritSec				m_AddRemoveLock;
	map< DWORD, CPlayer*>	m_players;			// serial value key
	ULONG2PTR				m_ulong2;			// player id key
	set<u_long>				m_set;				// set of operators
	u_long					m_uCount;			

public:
	CPlayerMng();
	virtual	~CPlayerMng();

	void		Free( void );
	BOOL		AddCache( DPID dpidCache );
	BOOL		RemoveCache( DPID dpidCache );
	u_long		GetCache( DPID dpidCache );
	BOOL		AddPlayer( u_long idPlayer, const CHAR* lpszPlayer, const CHAR* lpszAccount );
	void		RemovePlayer( CPlayer* pPlayer, BOOL bNotify = TRUE );
	BOOL		RegisterPlayerInfo( CPlayer* pPlayer );
	BOOL		UnregisterPlayerInfo( CPlayer* pPlayer, BOOL bNotify );
	CPlayer*	GetPlayerBySerial( DWORD dwSerial );
	CPlayer*	GetPlayerBySocket( DPID dpidSocket );
	CPlayer*	GetPlayer( u_long uKey );
	u_long		GetCount( void );
	void		PackName( CAr & ar );
	void		AddItToSetofOperator( u_long uPlayerId );
	BOOL	IsOperator( u_long idPlayer );
#if __VER >= 11 // __SYS_PLAYER_DATA
	void	Logout( CPlayer* pPlayer );
#endif	// __SYS_PLAYER_DATA
};


#endif	// __PLAYER_H__