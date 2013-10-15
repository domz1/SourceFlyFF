#ifndef __SOCK_H__
#define __SOCK_H__

#pragma once

#include <dplay.h>
#include <list>
using namespace std;

class CSock
{
public:
	enum	{	crcRead	= 1, crcWrite	= 2,	};
protected:
	SOCKET	m_hSocket;
private:
	DPID	m_dpid;
	DPID	m_dpidpeer;

public:
//	Constructions
	CSock();
	virtual	~CSock();
//	Operations
	SOCKET	GetHandle( void );
	void	Clear( void );
	void	SetID( DPID dpid );
	DPID	GetID( void );
	void	SetPeerID( DPID dpid );
	DPID	GetPeerID( void );
	
	virtual	BOOL	Create(	u_short uPort = 0, int type = SOCK_STREAM );
	virtual	void	Attach(	SOCKET hSocket );
	virtual	void	Detach( void );
	virtual	HRESULT	GetHostAddr( LPVOID lpAddr, LPDWORD lpdwSize );
	virtual	HRESULT	GetPeerAddr( DPID dpid, LPVOID lpAddr, LPDWORD lpdwSize )	= 0;
	virtual	DWORD	GetPeerAddr( DPID dpid ) { return 0; }
	virtual	void	Close( void )	= 0;
	virtual	BOOL	CloseConnection( SOCKET hSocket )	= 0;
	virtual	BOOL	Shutdown( SOCKET hSocket )	= 0;
	virtual	void	Send( char* lpData, DWORD dwDataSize, DPID dpidTo )	= 0;
	virtual	CSock*	Get( SOCKET hSocket );
#ifdef __PROTOCOL0910
	virtual	void	SetProtocolId( DWORD dwProtocolId )		{}
#endif	// __PROTOCOL0910
#ifdef __INFO_SOCKLIB0516
	virtual	DWORD	GetDebugInfo( SOCKET hSocket )	{	return 0;	}
#endif	// __INFO_SOCKLIB0516
};
inline SOCKET CSock::GetHandle( void )	{	return m_hSocket;	}
inline CSock* CSock::Get( SOCKET hSocket )	{	return( m_hSocket == hSocket ? this : NULL );	}

inline void	CSock::SetID( DPID dpid )	{	m_dpid	= dpid;	}
inline DPID CSock::GetID( void )	{	return m_dpid;	}
inline void CSock::SetPeerID( DPID dpid )	{	m_dpidpeer	= dpid;	}
inline DPID CSock::GetPeerID( void )	{	return m_dpidpeer;	}

#endif //__SOCK_H__