#include "stdafx.h"
#include "dpdbsrvr.h"
extern	CDPDBSrvr	g_dpDbSrvr;

#ifdef __GPAUTH_01
#include "lang.h"
#endif	// __GPAUTH_01

#include "BillingMgr.h"
#include "dpsrvr.h"
#include "account.h"
#include "mytrace.h"

#include "dbmanager.h"
extern	CDbManager	g_DbManager;

const int MAX_CONN = 50000;

extern	CAccountMng		g_AccountMng;
extern	CMyTrace		g_MyTrace;

CDPSrvr::CDPSrvr()
{
	m_bCheckAddr	= true;
	m_nMaxConn		=	MAX_CONN;
	m_bReloadPro	= TRUE;
	memset( m_sAddrPmttd, 0, sizeof(m_sAddrPmttd) );
	m_nSizeofAddrPmttd	= 0;

	m_dwSizeofServerset	= 0;

#ifdef __SECURITY_0628
	*m_szResVer	= '\0';
#endif	// __SECURITY_0628

	strcpy( m_szVer, "20040706" );	// default value

	BEGIN_MSG;
	ON_MSG( PACKETTYPE_ADD_ACCOUNT, OnAddAccount );
	ON_MSG( PACKETTYPE_REMOVE_ACCOUNT, OnRemoveAccount );
	ON_MSG( PACKETTYPE_PING, OnPing );
	ON_MSG( PACKETTYPE_CLOSE_EXISTING_CONNECTION, OnCloseExistingConnection );
}

CDPSrvr::~CDPSrvr()
{

}

void CDPSrvr::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_CREATEPLAYERORGROUP:
			{
				LPDPMSG_CREATEPLAYERORGROUP lpCreatePlayer	= (LPDPMSG_CREATEPLAYERORGROUP)lpMsg;
				OnAddConnection( lpCreatePlayer->dpId );
				break;
			}
		case DPSYS_DESTROYPLAYERORGROUP:
			{
				LPDPMSG_DESTROYPLAYERORGROUP lpDestroyPlayer	= (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
				OnRemoveConnection( lpDestroyPlayer->dpId );
				break;
			}
	}
}

void CDPSrvr::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	static size_t	nSize	= sizeof(DPID);
	
	LPBYTE lpBuffer		= (LPBYTE)lpMsg + nSize;
	u_long uBufSize		= dwMsgSize - nSize;

	CAr ar( lpBuffer, uBufSize );
	GETTYPE( ar )
	void ( theClass::*pfn )( theParameters )	=	GetHandler( dw );
	ASSERT( pfn );
	( this->*( pfn ) )( ar, idFrom, *(UNALIGNED LPDPID)lpMsg );
}

void CDPSrvr::OnAddConnection( DPID dpid )
{
	g_AccountMng.AddConnection( dpid );
	SendServersetList( dpid );
}

void CDPSrvr::OnRemoveConnection( DPID dpid )
{
	g_AccountMng.RemoveConnection( dpid );
}

void CDPSrvr::DestroyPlayer( DPID dpid1, DPID dpid2 )
{
	BEFORESENDSOLE( ar, PACKETTYPE_DESTROY_PLAYER, dpid2 );
	SEND( ar, this, dpid1 );
}

void CDPSrvr::OnAddAccount( CAr & ar, DPID dpid1, DPID dpid2 )
{
	TCHAR	lpszAccount[MAX_ACCOUNT];
	char	lpAddr[16];
	DWORD	dwAuthKey = 0;
	BYTE	cbAccountFlag = 0;
	int		fCheck = 0;

	ar.ReadString( lpAddr, 16 );	
	ar.ReadString( lpszAccount, MAX_ACCOUNT );
	ar >> cbAccountFlag;
#ifdef __BILLING0712
	ar >> fCheck;
#endif//__BILLING0712

#ifdef __GPAUTH_02
	char szCheck[255]	= { 0,};
#ifdef __EUROPE_0514
	char szBak[MAX_ACCOUNT]	= { 0,};
#endif	// __EUROPE_0514
	BOOL bGPotatoAuth	= ::GetLanguage() == LANG_GER || ::GetLanguage() == LANG_FRE;
	if( bGPotatoAuth )
		ar.ReadString( szCheck, 255 );
#ifdef __EUROPE_0514
	ar.ReadString( szBak, MAX_ACCOUNT );
#endif	// __EUROPE_0514
#if __VER >= 14 // __PCBANG
	DWORD dwPCBangClass;
	ar >> dwPCBangClass;
#endif // __PCBANG

#endif	// __GPAUTH_02
	BYTE cbResult = ACCOUNT_CHECK_OK;		 

	// 1. 외부 아이피 검사 
	if( m_bCheckAddr )							
	{
		CMclAutoLock	Lock( m_csAddrPmttd );
		for( int i = 0; i < m_nSizeofAddrPmttd; i++ )
		{
			if( strstr( lpAddr, m_sAddrPmttd[i] ) )
				break;		// 검사 통과 
		}
		if( i == m_nSizeofAddrPmttd )
			cbResult = ACCOUNT_EXTERNAL_ADDR;	 
	}

	CMclAutoLock	Lock( m_csIPCut );
	if( lpAddr[0] == '\0' || IsABClass(lpAddr) == TRUE )
		cbResult = ACCOUNT_IPCUT_ADDR;		

	if( lpAddr[0] == '\0' )
		Error( "Not Addr : %s", lpAddr );
	
	// 2. MAX, 중복검사  
	if( cbResult == ACCOUNT_CHECK_OK )			
	{
		if( g_AccountMng.m_nCount < m_nMaxConn )	
		{
			cbResult = g_AccountMng.AddAccount( lpszAccount, dpid1, dpid2, &dwAuthKey, cbAccountFlag, fCheck );
#if __VER >= 14 // __PCBANG
			if( dwPCBangClass != 0 )
				g_AccountMng.PushPCBangPlayer( dwAuthKey, dwPCBangClass );
#endif // __PCBANG
		}
		else
			cbResult = ACCOUNT_OVERFLOW;
	}

#ifdef __BILLING0712
	// 3. 빌링 검사 	
	if( cbResult == ACCOUNT_CHECK_OK )
	{
		cbResult = GetBillingMgr()->CheckAccount( fCheck, dwAuthKey, lpszAccount, lpAddr );
		if( cbResult == ACCOUNT_BILLING_WAIT_ACK )
			return;
	}
#endif

#ifdef __GPAUTH_02
#ifdef __EUROPE_0514
	OnAfterChecking( cbResult, lpszAccount, dpid1, dpid2, dwAuthKey, cbAccountFlag, 0, szCheck, szBak );
#else	// __EUROPE_0514
	OnAfterChecking( cbResult, lpszAccount, dpid1, dpid2, dwAuthKey, cbAccountFlag, 0, szCheck );
#endif	// __EUROPE_0514
#else	// __GPAUTH_02
	OnAfterChecking( cbResult, lpszAccount, dpid1, dpid2, dwAuthKey, cbAccountFlag, 0 );
#endif	// __GPAUTH_02
}

#ifdef __GPAUTH_02
#ifdef __EUROPE_0514
void CDPSrvr::OnAfterChecking( BYTE cbResult, LPCTSTR lpszAccount, DPID dpid1, DPID dpid2, 
							   DWORD dwAuthKey, BYTE cbAccountFlag, long lTimeSpan, const char* szCheck, const char* szBak )
#else	// __EUROPE_0514
void CDPSrvr::OnAfterChecking( BYTE cbResult, LPCTSTR lpszAccount, DPID dpid1, DPID dpid2, 
							   DWORD dwAuthKey, BYTE cbAccountFlag, long lTimeSpan, const char* szCheck )
#endif	// __EUROPE_0514
#else	// __GPAUTH_02
void CDPSrvr::OnAfterChecking( BYTE cbResult, LPCTSTR lpszAccount, DPID dpid1, DPID dpid2, 
							   DWORD dwAuthKey, BYTE cbAccountFlag, long lTimeSpan )
#endif	// __GPAUTH_02
{
	if( cbResult == ACCOUNT_CHECK_OK )
	{
		g_DbManager.UpdateTracking( TRUE, lpszAccount );  // 유저가 login 했음을 디비에 쓴다.
	}

	BEFORESENDSOLE( ar, PACKETTYPE_ADD_ACCOUNT, dpid2 );
	ar << cbResult;
	ar << dwAuthKey;
	ar << cbAccountFlag;

#ifdef __BILLING0712
	ar << lTimeSpan;
#endif	// __BILLING0712

#ifdef __GPAUTH_01
	BOOL bGPotatoAuth	= ::GetLanguage() == LANG_GER || ::GetLanguage() == LANG_FRE;
	if( bGPotatoAuth )
	{
		ar.WriteString( lpszAccount );
#ifdef __GPAUTH_02
		ar.WriteString( szCheck );
#endif	// __GPAUTH_02
	}
#ifdef __EUROPE_0514
	ar.WriteString( szBak );
#endif	// __EUROPE_0514
#endif	// __GPAUTH_01

	SEND( ar, this, dpid1 );
}

void CDPSrvr::OnRemoveAccount( CAr & ar, DPID dpid1, DPID dpid2 )
{
	g_AccountMng.RemoveAccount( dpid1, dpid2 );
}


void CDPSrvr::OnPing( CAr & ar, DPID dpid1, DPID dpid2 )
{
	CMclAutoLock	Lock( g_AccountMng.m_AddRemoveLock );

	CAccount* pAccount	= g_AccountMng.GetAccount( dpid1, dpid2 );
	if( NULL != pAccount )
		pAccount->m_dwPing	= timeGetTime();
}

bool CDPSrvr::LoadAddrPmttd( LPCTSTR lpszFileName )
{
	CMclAutoLock	Lock( m_csAddrPmttd );

	m_nSizeofAddrPmttd	= 0;

	CScanner s;

	if( s.Load( lpszFileName ) )
	{
		s.GetToken();
		while( s.tok != FINISHED )
		{
			strcpy( m_sAddrPmttd[m_nSizeofAddrPmttd++], (LPCSTR)s.Token );
			s.GetToken();
		}
		return true;
	}
	return false;
}


BOOL CDPSrvr::LoadIPCut( LPCSTR lpszFileName )
{
	CMclAutoLock	Lock( m_csIPCut );

	InitIPCut();

	char strABClass[MAX_PATH];

	CScanner s;
	if( s.Load( lpszFileName ) )
	{
		s.GetToken();
		while( s.tok != FINISHED )
		{
			ZeroMemory( strABClass, sizeof( strABClass ) );
			
			GetABCClasstoString( s.Token, strABClass, m_nIPCut[m_nSizeofIPCut][0] );
			s.GetToken();
			s.GetToken();
			GetABCClasstoString( s.Token, strABClass, m_nIPCut[m_nSizeofIPCut][1] );

			int nFindLast = -1;
			STRING2INT::iterator iFind = m_sIPCut.find( strABClass );
			if( iFind != m_sIPCut.end() )
			{	
				int nFind = iFind->second;
				int nBufFind = m_nIPCut[nFind][2];
				nFindLast = nFind;
				while( nBufFind != -1 )
				{
					nFindLast = nBufFind;
					nBufFind = m_nIPCut[nBufFind][2];
				}
			}
			m_sIPCut.insert( map<string, int>::value_type( strABClass, m_nSizeofIPCut ) );
			++m_nSizeofIPCut;

			if( nFindLast != -1 )
			{
				m_nIPCut[nFindLast][2] = m_nSizeofIPCut - 1;
			}
			s.GetToken();
		}
		return TRUE;
	}
	
	return FALSE;
}

BOOL CDPSrvr::IsABClass( LPCSTR lpszIP )
{
	char strABClass[MAX_PATH] = {0,};
	int nCClass = 0;
	GetABCClasstoString( lpszIP, strABClass, nCClass );
	STRING2INT::iterator iFind = m_sIPCut.find( strABClass );
	if( iFind != m_sIPCut.end() )
	{
		int nFind = iFind->second;
		if( m_nIPCut[nFind][0] <= nCClass && nCClass <= m_nIPCut[nFind][1] )
		{
			return TRUE;
		}

		while( m_nIPCut[nFind][2] != -1 )
		{
			nFind = m_nIPCut[nFind][2];
			if( m_nIPCut[nFind][0] <= nCClass && nCClass <= m_nIPCut[nFind][1] )
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

void CDPSrvr::GetABCClasstoString( LPCSTR lpszIP, char * lpABClass, int &nCClass )
{
	int nFindAB, nFindC;
	CString strBuf1;
	char strCClass[MAX_PATH];
	ZeroMemory( strCClass, sizeof( strCClass ) );
	strBuf1 = lpszIP;
	nFindAB = strBuf1.Find( "." );
	if( nFindAB == -1 )
		AfxMessageBox( strBuf1 );
	nFindAB = strBuf1.Find( ".", nFindAB + 1 );
	if( nFindAB == -1 )
		AfxMessageBox( strBuf1 );
	
	memcpy( lpABClass, lpszIP, nFindAB );
	nFindC = strBuf1.Find( ".", nFindAB + 1 );
	if( nFindC == -1 )
		AfxMessageBox( strBuf1 );
	for( int i = nFindAB + 1, j = 0 ; i < nFindC ; ++i, ++j )
	{
		strCClass[j] = lpszIP[i];
	}
	nCClass = atoi( strCClass );
}


void CDPSrvr::InitIPCut( void )
{
	m_nSizeofIPCut = 0;
	for( int i = 0 ; i < MAX_IP ; ++i )
	{
		m_nIPCut[i][0] = 0;
		m_nIPCut[i][1] = 0;
		m_nIPCut[i][2] = -1;
	}
	m_sIPCut.clear();
}

void CDPSrvr::SendServersetList( DPID dpid )
{
	BEFORESENDSOLE( ar, PACKETTYPE_SRVR_LIST, DPID_UNKNOWN );
	ar.WriteString( m_szVer );
#ifdef __SECURITY_0628
	ar.WriteString( m_szResVer );
#endif	// __SECURITY_0628
	ar << m_dwSizeofServerset;
	LPSERVER_DESC pServer;
	for( DWORD i = 0; i < m_dwSizeofServerset; i++ )
	{
		pServer		= m_aServerset + i;
		ar << pServer->dwParent;
		ar << pServer->dwID;
		ar.WriteString( pServer->lpName );
		ar.WriteString( pServer->lpAddr );
		ar << pServer->b18;
		ar << pServer->lCount;
		ar << pServer->lEnable;
		ar << pServer->lMax;
	}
	SEND( ar, this, dpid );
}

void CDPSrvr::SendPlayerCount( u_long uId, long lCount )
{
	BEFORESENDSOLE( ar, PACKETTYPE_PLAYER_COUNT, DPID_UNKNOWN );
	ar << uId << lCount;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPSrvr::SendEnableServer( u_long uId, long lEnable )
{
	BEFORESENDSOLE( ar, PACKETTYPE_ENABLE_SERVER, DPID_UNKNOWN );
	ar << uId << lEnable;
	SEND( ar, this, DPID_ALLPLAYERS );
}

BOOL CDPSrvr::EnableServer( DWORD dwParent, DWORD dwID, long lEnable )
{
//	if( dwParent != NULL_ID )
	{
		u_long uId	= dwParent * 100 + dwID;
		map<u_long, LPSERVER_DESC>::iterator i2	= m_2ServersetPtr.find( uId );
		if( i2 != m_2ServersetPtr.end() )
		{
			InterlockedExchange( &i2->second->lEnable, lEnable );
			SendEnableServer( uId, lEnable );
			g_MyTrace.Add( uId, FALSE, "%d/%d - %s", dwParent, dwID, lEnable? "o": "x" );
			return TRUE;
		}
	}
	return FALSE;
}

void CDPSrvr::OnCloseExistingConnection( CAr & ar, DPID dpid1, DPID dpid2 )
{
	char lpszAccount[MAX_ACCOUNT]	= { 0, };
	ar.ReadString( lpszAccount, MAX_ACCOUNT );
	CloseExistingConnection( lpszAccount, 0 );	// 0 - 에러가 아니다.
}

// 현재 접속한 어카운트를 끊는다.
// 접속은 2가지 경우가 있다. - 접속 후 플레이를 하는 경우 
//                           - 접속 과정 
void CDPSrvr::CloseExistingConnection( LPCTSTR lpszAccount, LONG lError )
{
	CMclAutoLock	Lock( g_AccountMng.m_AddRemoveLock );
	CAccount* pAccount	= g_AccountMng.GetAccount( lpszAccount );
	if( pAccount )
	{
		if( pAccount->m_fRoute )	// 접속 후 플레이를 하는 경우 
		{
			g_dpDbSrvr.SendCloseExistingConnection( lpszAccount, lError );
		}
		else						// 접속 과정 
		{
			DestroyPlayer( pAccount->m_dpid1, pAccount->m_dpid2 );
			g_AccountMng.RemoveAccount( pAccount->m_dpid1, pAccount->m_dpid2 );
		}
	}
}

CDPSrvr		g_dpSrvr;
