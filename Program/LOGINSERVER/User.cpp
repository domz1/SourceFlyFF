#include "stdafx.h"
#include "user.h"
#include "dpdatabaseclient.h"
#include "dpcoreclient.h"
#include "dploginsrvr.h"

extern	CDPLoginSrvr		g_dpLoginSrvr;
extern	CDPDatabaseClient	g_dpDBClient;
extern	CDPCoreClient		g_dpCoreClient;

#ifndef __VM_0820
MemPooler<CUser>*	CUser::m_pPool	= new MemPooler<CUser>( 1024 );
#endif	// __VM_0820

CUser::CUser( DPID dpid )
{
	m_pKey[0] = '\0';
	m_dwAuthKey	= 0;
	m_idPlayer	= 0;
	m_dpid	= dpid;
	m_nIndexOfCache	= 0;
	m_tPingRecvd	= m_dwTime	= timeGetTime();
	m_bIllegal	= FALSE;
	if( g_bNProtectAuth )
	{
		m_nWrite	= m_nRead	= 0;
		SetAuth( FALSE );		// 인증이 되어 있지 않음 
	}
	else
	{
		SetAuth( TRUE );		// 인증이 되어 있음 
	}
	m_tAuth		= time( NULL ) + AUTH_PERIOD;
}

CUser::~CUser()
{
}

void CUser::SetExtra( const char* pKey, DWORD dwAuthKey )
{
	strcpy( m_pKey, pKey );
	m_dwAuthKey	= dwAuthKey;
}

CUserMng::CUserMng()
{
#ifndef __STL_0402
	m_dpid2User.SetSize( 1024, 4096, 1024 );
#endif	// __STL_0402
	m_uCount	= 0;
}

CUserMng::~CUserMng()
{
	Free();
}

void CUserMng::Free( void )
{
	CMclAutoLock Lock( m_AddRemoveLock );

#ifdef __STL_0402
	for( C2User::iterator i = m_dpid2User.begin(); i != m_dpid2User.end(); ++i )
	{
		CUser* pUser	= i->second;
		SAFE_DELETE( pUser );
	}
	m_dpid2User.clear();
#else	// __STL_0402
	CUserBucket* pBucket	= m_dpid2User.GetFirstActive();
	while( pBucket )
	{
		safe_delete( pBucket->m_value );
		pBucket		= pBucket->pNext;
	}
	m_dpid2User.ClearActiveList();
#endif	// __STL_0402
	m_ac2User.clear();
}

// dpid를 키로 콘테이너에 넣는다.
BOOL CUserMng::AddUser( DPID dpid, CUser* pUser )
{
	ASSERT( dpid == pUser->m_dpid );

	CMclAutoLock Lock( m_AddRemoveLock );

	if( GetUser( dpid ) == NULL )
	{
#ifdef __STL_0402
		bool bResult	= m_dpid2User.insert( C2User::value_type( dpid, pUser ) ).second;
//		ASSERT( bResult );
#else	// __STL_0402
		m_dpid2User.SetAt( dpid, pUser );
#endif	// __STL_0402
		m_uCount++;
		return TRUE;
	}
	else
		return FALSE;
}

// pKey 키로 콘테이너에 넣는다.
BOOL CUserMng::AddUser( const char* pKey, CUser* pUser )
{
	// lock이 걸린 상태라고 가정됨 
	bool result = m_ac2User.insert( map<string, CUser*>::value_type( pKey, pUser ) ).second;
	return ( result == true );
}

CUser* CUserMng::GetUser( const char* pKey )
{
	map<string, CUser*>::iterator i	= m_ac2User.find( pKey );
	if( i != m_ac2User.end() )
		return i->second;
	return NULL;
}

CUser* CUserMng::GetUser( DPID dpid )
{
#ifdef __STL_0402
	C2User::iterator i = m_dpid2User.find( dpid );
	if( i != m_dpid2User.end() )
		return i->second;
	return NULL;
#else	// __STL_0402
	CUser* pUser;
	if( m_dpid2User.Lookup( dpid, pUser ) )
		return pUser;
	return NULL;
#endif	// __STL_0402
}

BOOL CUserMng::RemoveUser( DPID dpid )
{
	CMclAutoLock Lock( m_AddRemoveLock );
#ifdef __STL_0402
	C2User::iterator i = m_dpid2User.find( dpid );
	if( i != m_dpid2User.end() )
	{
		CUser* pUser	= i->second;
		m_dpid2User.erase( i );
#else	// __STL_0402
	CUser* pUser;
	if( m_dpid2User.Lookup( dpid, pUser ) )
	{
		m_dpid2User.RemoveKey( dpid );
#endif	// __STL_0402
		m_ac2User.erase( pUser->m_pKey );

		if( !pUser->m_bIllegal )
		{
			if( *pUser->m_pKey != '\0' )
			{
				g_dpDBClient.SendLeave( pUser->m_pKey, pUser->m_idPlayer, timeGetTime() - pUser->m_dwTime );
			}
			else
				WriteLog( "RemoveUser(): *pUser->m_pKey is '\0'" );

			if( pUser->m_idPlayer > 0 )
				g_dpCoreClient.SendLeave( pUser->m_idPlayer );
		}
		else
		{
			WriteLog( "RemoveUser(): Illegal" );
		}
		g_dpLoginSrvr.CacheOut( pUser->m_nIndexOfCache );

		SAFE_DELETE( pUser );
		m_uCount--;
		return TRUE;
	}
	WriteLog( "RemoveUser(): dpid not found" );
	return FALSE;
}

void CUserMng::DestroyAbnormalPlayer( void )
{
#ifdef __INTERNALSERVER
	return;
#endif	

	DWORD t		= timeGetTime();
	CTime time	= CTime::GetCurrentTime();
	//CString strTime	= time.Format( "%Y/%m/%d %H:%M:%S" );
	CMclAutoLock	Lock( m_AddRemoveLock );

#ifdef __STL_0402
	for( C2User::iterator i = m_dpid2User.begin(); i != m_dpid2User.end(); ++i )
	{
		CUser* pUser	= i->second;
		if( ( t - pUser->m_tPingRecvd ) > 90000 ) // 90
			g_dpLoginSrvr.DestroyPlayer( pUser->m_dpid );
	}
#else	// __STL_0402
	CUser* pUser;
	CUserBucket* pBucket	= m_dpid2User.GetFirstActive();
	while( pBucket )
	{
		pUser	= pBucket->m_value;
		if( ( t - pUser->m_tPingRecvd ) > 90000 ) // 90
		{
			//FILEOUT( "error.log", "%s - %s", (LPCTSTR)strTime, pUser->m_pKey );
			g_dpLoginSrvr.DestroyPlayer( pUser->m_dpid );
		}
		pBucket		= pBucket->pNext;
	}
#endif	// __STL_0402
}

// nprotect관련 
// ggsrvlib.lib 링크 
// ggauth.dll

// 서버에서 검사하기 위해서 주기적으로 불려지는 함수 
void CUserMng::CheckAll()
{
	time_t	t	= time( NULL );
	CMclAutoLock	Lock( m_AddRemoveLock );

#ifdef __STL_0402
	for( C2User::iterator i = m_dpid2User.begin(); i != m_dpid2User.end(); ++i )
	{
		CUser* pUser	= i->second;
		if( pUser->GetAuth() )		// 인증을 받은 자들은 검사 
		{
			if( pUser->m_tAuth < t )
			{
				DWORD dwError	= pUser->m_csa.GetAuthQuery();
				if( dwError == ERROR_SUCCESS )
				{
					pUser->m_nWrite++;
					pUser->m_tAuth	= t + AUTH_PERIOD;
					BEFORESEND( ar, PACKETTYPE_AUTHQUERY );
					ar.Write( (void*)&pUser->m_csa.m_AuthQuery, sizeof(GG_AUTH_DATA) );
					SEND( ar, &g_dpLoginSrvr, pUser->m_dpid );
				}
				else
				{
					Error( "CSAUTH//0//%d, %d, %d//%d, %d, %d, %d", dwError, pUser->m_nWrite, pUser->m_nRead, pUser->m_csa.m_AuthQuery.dwIndex, pUser->m_csa.m_AuthQuery.dwValue1, pUser->m_csa.m_AuthQuery.dwValue2, pUser->m_csa.m_AuthQuery.dwValue3 );
					g_dpLoginSrvr.DestroyPlayer( pUser->m_dpid );
				}
			}
		}
	}
#else	// __STL_0402
	CUser* pUser;
	CUserBucket* pBucket	= m_dpid2User.GetFirstActive();
	while( pBucket )
	{
		pUser = pBucket->m_value;
		if( pUser->GetAuth() )		// 인증을 받은 자들은 검사 
		{
			if( pUser->m_tAuth < t )
			{
				DWORD dwError	= pUser->m_csa.GetAuthQuery();
				if( dwError == ERROR_SUCCESS )
				{
					pUser->m_nWrite++;
					pUser->m_tAuth	= t + AUTH_PERIOD;
					BEFORESEND( ar, PACKETTYPE_AUTHQUERY );
					ar.Write( (void*)&pUser->m_csa.m_AuthQuery, sizeof(GG_AUTH_DATA) );
					SEND( ar, &g_dpLoginSrvr, pUser->m_dpid );
				}
				else
				{
					Error( "CSAUTH//0//%d, %d, %d//%d, %d, %d, %d", dwError, pUser->m_nWrite, pUser->m_nRead, pUser->m_csa.m_AuthQuery.dwIndex, pUser->m_csa.m_AuthQuery.dwValue1, pUser->m_csa.m_AuthQuery.dwValue2, pUser->m_csa.m_AuthQuery.dwValue3 );
					g_dpLoginSrvr.DestroyPlayer( pUser->m_dpid );
				}
			}
		}
		pBucket		= pBucket->pNext;
	}
#endif	// __STL_0402
}

CUserMng	g_UserMng;