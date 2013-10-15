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
public:
	DWORD	m_dwcrc;
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
	BOOL	IsCrcRead( void )	{	return( m_dwcrc & crcRead );	}
	BOOL	IsCrcWrite( void )	{	return( m_dwcrc & crcWrite );	}
	
	virtual	BOOL	Create(	u_short uPort = 0, int type = SOCK_STREAM );
	virtual	void	Attach(	SOCKET hSocket );
	virtual	HRESULT	GetHostAddr( LPVOID lpAddr, LPDWORD lpdwSize );
	virtual	HRESULT	GetPeerAddr( DPID dpid, LPVOID lpAddr, LPDWORD lpdwSize )	= 0;
	virtual	void	Close( void )	= 0;
	virtual	BOOL	CloseConnection( SOCKET hSocket )	= 0;
	virtual	BOOL	Shutdown( SOCKET hSocket )	= 0;
	virtual	void	Send( char* lpData, DWORD dwDataSize, DPID dpidTo )	= 0;
	virtual	CSock*	Get( SOCKET hSocket );
	virtual	void CSock::Detach( void )
	{
		m_hSocket	= INVALID_SOCKET;
		m_dpid	= m_dpidpeer	= DPID_UNKNOWN;
	}

};
inline SOCKET CSock::GetHandle( void )	{	return m_hSocket;	}
inline CSock* CSock::Get( SOCKET hSocket )	{	return( m_hSocket == hSocket ? this : NULL );	}

inline void	CSock::SetID( DPID dpid )	{	m_dpid	= dpid;	}
inline DPID CSock::GetID( void )	{	return m_dpid;	}
inline void CSock::SetPeerID( DPID dpid )	{	m_dpidpeer	= dpid;	}
inline DPID CSock::GetPeerID( void )	{	return m_dpidpeer;	}

#endif //__SOCK_H__