#ifndef __USER_H__
#define __USER_H__

#pragma once

#include <DPlay.h>
#include <map>
#include <string>
using	namespace	std;
#include "MyMap.h"
#include "mempooler.h"
#include "ggsrv26.h"
#include "dpmng.h"

#define	AUTH_PERIOD		300

class CUser
{
public:
	DPID		m_dpid;
	DWORD		m_dwTime;
	int			m_nIndexOfCache;
	char		m_pKey[MAX_ACCOUNT];
	DWORD		m_idPlayer;
	DWORD		m_tPingRecvd;
	DWORD		m_dwAuthKey;
	BOOL		m_bIllegal;
	CCSAuth2	m_csa;
	time_t	m_tAuth;
	int			m_nWrite;
	int			m_nRead;
	BOOL		m_bAuth;

public:
	// Constructions
	CUser( DPID dpid );
	virtual	~CUser();

	void	SetExtra( const char* pKey, DWORD dwAuthKey );
	void	SetAuth( BOOL bAuth ) { m_bAuth = bAuth; }
	BOOL	GetAuth() { return m_bAuth; }

public:
#ifndef __VM_0820
#ifndef __MEM_TRACE
	static MemPooler<CUser>*	m_pPool;
	void*	operator new( size_t nSize )	{	return CUser::m_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CUser::m_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CUser::m_pPool->Free( (CUser*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CUser::m_pPool->Free( (CUser*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820
};

#ifdef __STL_0402
typedef map<DPID, CUser*>	C2User;
#else	// __STL_0402
typedef	CMyMap<CUser*>	C2User;
typedef	CMyBucket<CUser*>	CUserBucket;
#endif	// __STL_0402

class CUserMng
{
private:
	C2User	m_dpid2User;
	map<string, CUser*>	m_ac2User;
	u_long	m_uCount;

public:
	CMclCritSec		m_AddRemoveLock;
	
public:
//	Constructions
	CUserMng();
	virtual	~CUserMng();
//	Operations

	void	Free( void );
	BOOL	AddUser( DPID dpid, CUser* pUser );
	BOOL	AddUser( const char* pKey, CUser* pUser );
	BOOL	RemoveUser( DPID dpid );
	CUser*	GetUser( const char* pKey );
	CUser*	GetUser( DPID dpid );
	u_long	GetCount( void );

	void	DestroyAbnormalPlayer( void );
	void	CheckAll();
};

inline u_long CUserMng::GetCount( void )
{	
	CMclAutoLock	Lock( m_AddRemoveLock );
	return m_uCount;
}

#endif	// __USER_H__