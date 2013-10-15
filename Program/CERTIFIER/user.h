#ifndef __USER_H__
#define	__USER_H__

#include <dplay.h>
#include <map>
#include <string>
#include "dpmng.h"

using	namespace	std;

class CUser
{
public:
	DPID	m_dpid;
	DWORD	m_dwTick;
	BOOL	m_bValid;

public:
	CUser( DPID dpid );
	virtual	~CUser()	{}

	void SetAccount( const char* szAccount )
		{	strncpy( m_pszAccount, szAccount, MAX_ACCOUNT );	m_pszAccount[MAX_ACCOUNT-1]		= '\0';		}
	const char*	GetAccount( void )	{	return m_pszAccount;	}

private:
	char	m_pszAccount[MAX_ACCOUNT];
};

class CUserMng : public map<DPID, CUser*>
{
public:
	CMclCritSec		m_AddRemoveLock;
public:
	CUserMng();
	virtual	~CUserMng();

	BOOL	AddUser( DPID dpid );
	BOOL	RemoveUser( DPID dpid );
	CUser*	GetUser( DPID dpid );
	void	ClearDum( CDPMng* pdp );

	static	CUserMng*	GetInstance( void );
};
#endif	// __USER_H__