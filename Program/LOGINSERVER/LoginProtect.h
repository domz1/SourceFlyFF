// LoginProtect.h: interface for the CLoginProtect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGINPROTECT_H__818DA819_B42E_4A41_9CEF_53B0C13A0F51__INCLUDED_)
#define AFX_LOGINPROTECT_H__818DA819_B42E_4A41_9CEF_53B0C13A0F51__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
#include <dplay.h>

#define	MAX_NUMPADID	999
#define	MAX_SECRETNUM	9999

enum LOGIN_CHECK
{
	LOGIN_OK,				// 틀린 적이 없거나, 처음 
	LOGIN_3TIMES_ERROR,		// 3회 틀림 
};

class CLoginProtect
{
public:
	CLoginProtect();
	~CLoginProtect();

	static CLoginProtect* GetInstance();

	void	Clear();
	int		GetNumPad2PW( u_long idNumPad, u_long uSecretNum );
	u_long	GetNumPadId( DPID dpId );
	u_long	SetNumPadId( DPID dpId, BOOL bLogin );
	LOGIN_CHECK	Check( u_long idPlayer );
	void	SetError( u_long idPlayer, BOOL bLogin );

private:
	struct LOGIN_CACHE
	{
		int		nError;
		time_t	tmError;
		LOGIN_CACHE() : nError( 0 ), tmError( time( NULL ) )	{};
	};
	typedef map<u_long, LOGIN_CACHE>	MAP_CACHE;
	MAP_CACHE	m_mapCache;

	typedef map<DPID, u_long>	MAP_NUMPAD_ID;
	MAP_NUMPAD_ID	m_mapNumPadId;
};

#endif // __2ND_PASSWORD_SYSTEM
#endif // !defined(AFX_LOGINPROTECT_H__818DA819_B42E_4A41_9CEF_53B0C13A0F51__INCLUDED_)
