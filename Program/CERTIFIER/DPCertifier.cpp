#include "stdafx.h"
#include "dpaccountclient.h"
#include "dpcertifier.h"
#include "dbmanager.h"
#include "..\Resource\Lang.h"

#ifdef __TRACE1027
#include "..\_UnhandledException\exceptionhandler.h"
#endif	// __TRACE1027

#include "user.h"

extern	CDPAccountClient	g_dpAccountClient;
extern	CDbManager			g_DbManager;

CLoginLimit::CLoginLimit()
{
	m_nLimitation		= INT_MAX;
	m_dwTick			= 0;
	m_nRequestPerSecond	= 0;
}

// 초당 login시도가 제한을 넘는지 체크한다.  ( true - 제한을 넘는다 )
bool CLoginLimit::Check()
{
	m_nRequestPerSecond++;
	if( GetTickCount() > m_dwTick )
	{
		m_dwTick = GetTickCount() + 1000;
		m_nRequestPerSecond = 1;
	}

	return ( m_nRequestPerSecond > m_nLimitation );
}

CDPCertifier::CDPCertifier()
{
	m_dwSizeofServerset		= 0;
	*m_szVer	= '\0';
#ifdef __SECURITY_0628
	*m_szResVer		= '\0';
#endif	// __SECURITY_0628

	BEGIN_MSG;
	ON_MSG( PACKETTYPE_CERTIFY, OnCertify );
	ON_MSG( PACKETTYPE_PING, OnPing );
	ON_MSG( PACKETTYPE_CLOSE_EXISTING_CONNECTION, OnCloseExistingConnection );
	ON_MSG( PACKETTYPE_KEEP_ALIVE, OnKeepAlive );
	ON_MSG( PACKETTYPE_ERROR, OnError );
}

CDPCertifier::~CDPCertifier()
{

}

void CDPCertifier::SetLimitation( int nLimit )
{
//	if( ::GetLanguage() == LANG_JAP )
//	{
//		if( nLimit > 0 )
//			m_limit.SetLimitation( nLimit );
//	}
}

void CDPCertifier::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpid )
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

void CDPCertifier::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpid )
{
	CAr ar( (LPBYTE)lpMsg, dwMsgSize );
	
	if( dwMsgSize < 4 )
	{
		WriteError( "PACKET//0" );
		return;
	}

	GETTYPE( ar );
	void ( theClass::*pfn )( theParameters )
		=	GetHandler( dw );

//	ASSERT( pfn );
	if( pfn )
		( this->*( pfn ) )( ar, dpid, (LPBYTE)lpMsg + sizeof(DWORD), dwMsgSize - sizeof(DWORD) );
}

void CDPCertifier::OnAddConnection( DPID dpid )
{
#ifdef __US_LOGIN_0223
	if( !CUserMng::GetInstance()->AddUser( dpid ) )
		DestroyPlayer( dpid );
#else	// __US_LOGIN_0223
	CUserMng::GetInstance()->AddUser( dpid );
#endif	// __US_LOGIN_0223
}

// 태국의 경우 돌아오는 22:00시까지의 남은 시간을 초단위로 보낸다.
long GetPlayLeftTime( BYTE cbAccountFlag )
{
	if( cbAccountFlag & ACCOUNT_FLAG_18 )	// 성인은 남은 22:00시에 끊기지 않기에, 시간을 알리지 않게함 
		return 0;		

	CTime cur = CTime::GetCurrentTime();
	
	CTimeSpan span; 
	if( cur.GetHour() < 22 )
	{
		CTime target( cur.GetYear(), cur.GetMonth(), cur.GetDay(), 22, 0, 0 );
		span = target - cur;
	}
	else
	{
		CTime target( cur.GetYear(), cur.GetMonth(), cur.GetDay(), 24, 0, 0 );
		span = target - cur;

		CTimeSpan ts( 0, 22, 0, 0 ); 
		span += ts;
	}

	return max( (long)( span.GetTotalSeconds() ), 1 );	// 적어도 1초 이상 ( 0 과 구분하기 위해서 )
}

#ifdef __GPAUTH_01
#ifdef __GPAUTH_02
#ifdef __EUROPE_0514
void CDPCertifier::SendServerList( DPID dpId, DWORD dwAuthKey, BYTE cbAccountFlag, long lTimeSpan, const char* szGPotatoNo, const char* szCheck, const char* szBak )
#else	// __EUROPE_0514
void CDPCertifier::SendServerList( DPID dpId, DWORD dwAuthKey, BYTE cbAccountFlag, long lTimeSpan, const char* szGPotatoNo, const char* szCheck )
#endif	// __EUROPE_0514
#else	// __GPAUTH_02
void CDPCertifier::SendServerList( DPID dpId, DWORD dwAuthKey, BYTE cbAccountFlag, long lTimeSpan, const char* szGPotatoNo )
#endif	// __GPAUTH_02
#else	// __GPAUTH_01
void CDPCertifier::SendServerList( DPID dpId, DWORD dwAuthKey, BYTE cbAccountFlag, long lTimeSpan )
#endif	// __GPAUTH_01
{
	BEFORESEND( ar, PACKETTYPE_SRVR_LIST );
	ar << dwAuthKey;
	ar << cbAccountFlag;

#ifdef __BILLING0712	   
	ar << lTimeSpan;
#endif

#ifdef __GPAUTH_01
	BOOL bGPotatoAuth	= ::GetLanguage() == LANG_GER || ::GetLanguage() == LANG_FRE;
	if( bGPotatoAuth )
	{
		ar.WriteString( szGPotatoNo );
#ifdef __GPAUTH_02
		ar.WriteString( szCheck );
#endif	// __GPAUTH_02
	}
#ifdef __EUROPE_0514
	ar.WriteString( szBak );
#endif	// __EUROPE_0514

#endif	// __GPAUTH_01

	if( ::GetLanguage() == LANG_THA )
	{
		long lTimeLeft = GetPlayLeftTime( cbAccountFlag );	// 태국의 경우 돌아오는 22:00시까지의 남은 시간을 보낸다.
		ar << lTimeLeft;
	}

	ar << m_dwSizeofServerset;
	for( DWORD i = 0; i < m_dwSizeofServerset; i++ )
	{
		LPSERVER_DESC pServer = m_aServerset + i;

		ar << pServer->dwParent;
		ar << pServer->dwID;
		ar.WriteString( pServer->lpName );
		ar.WriteString( pServer->lpAddr );
		ar << pServer->b18;
		ar << pServer->lCount;
		ar << pServer->lEnable;
		ar << pServer->lMax;
	}
	SEND( ar, this, dpId );
}

void CDPCertifier::SendError( LONG lError, DPID dpId )
{
	BEFORESEND( ar, PACKETTYPE_ERROR );
	ar << lError;
	SEND( ar, this, dpId );
//	DestroyPlayer( dpId ); shutdown을 호출하지 못하는 이 아픔 
}

#ifdef __GPAUTH
void CDPCertifier::SendErrorString( const char* szError, DPID dpid )
{
	BEFORESEND( ar, PACKETTYPE_ERROR_STRING );
	ar.WriteString( szError );
	SEND( ar, this, dpid );
}
#endif	// __GPAUTH

// Handlers
/*________________________________________________________________________________*/
void CDPCertifier::OnRemoveConnection( DPID dpid )
{
	CUserMng::GetInstance()->RemoveUser( dpid );
	g_dpAccountClient.SendRemoveAccount( dpid );
}

void CDPCertifier::OnError( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
	DestroyPlayer( dpid );
}

void CDPCertifier::OnCertify( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
	char pszVer[32]	= { 0, };
	ar.ReadString( pszVer, 32 );

	if( strcmp( m_szVer, pszVer ) )
	{
		SendError( ERROR_ILLEGAL_VER, dpid );
		return;
	}
#ifdef __SECURITY_0628
	char pszResVer[100]	= { 0,};
	ar.ReadString( pszResVer, 100 );
	if( lstrlen( m_szResVer ) > 0 && lstrcmp( m_szResVer, pszResVer ) )
	{
		SendError( ERROR_FLYFF_RESOURCE_MODIFIED, dpid );
		return;
	}
#endif	// __SECURITY_0628

	char pszAccount[MAX_ACCOUNT] = {0, }, pszPwd[MAX_PASSWORD] = {0, };
	ar.ReadString( pszAccount, MAX_ACCOUNT );

	//////////////////////////////////////////////////////////////////////////
	//	mulcom	BEGIN100218	패스워드 암호화
#ifdef __ENCRYPT_PASSWORD
	char		szEnc[ 16 * MAX_PASSWORD ] = {0, };
	char		szDec[ 16 * MAX_PASSWORD ] = {0, };
	
	ar.Read( szEnc, 16 * MAX_PASSWORD );
	
	g_xRijndael->ResetChain();
	g_xRijndael->Decrypt( szEnc, szDec, 16 * MAX_PASSWORD, CRijndael::CBC );
	
	::memcpy( pszPwd, szDec, MAX_PASSWORD );
#else
	ar.ReadString( pszPwd, MAX_PASSWORD );
#endif
	//	mulcom	END100218	패스워드 암호화
	//////////////////////////////////////////////////////////////////////////


	if( pszAccount[0] == '\0' || StringFind( pszAccount, '\'' ) >= 0 || StringFind( pszPwd, '\'' ) >= 0 )
	{
		DestroyPlayer( dpid );
		return;
	}

#ifdef __EUROPE_0514
	{
		CMclAutoLock Lock( CUserMng::GetInstance()->m_AddRemoveLock );
		CUser* pUser	= CUserMng::GetInstance()->GetUser( dpid );
		if( pUser )
			pUser->SetAccount( pszAccount );
		else
			return;
	}
#endif	// __EUROPE_0514

	LPDB_OVERLAPPED_PLUS pData	= g_DbManager.m_pDbIOData->Alloc();
	memset( &pData->AccountInfo, 0, sizeof(ACCOUNT_INFO) );
	strcpy( pData->AccountInfo.szAccount, pszAccount );
	strcpy( pData->AccountInfo.szPassword, pszPwd );
	_tcslwr( pData->AccountInfo.szAccount );
#ifdef __TWN_LOGIN0816
	char pszSessionPwd[MAX_SESSION_PWD]	= { 0,};
	if( GetLanguage() == LANG_TWN )
		ar.ReadString( pszSessionPwd, MAX_SESSION_PWD );
	lstrcpy( pData->AccountInfo.szSessionPwd, pszSessionPwd );
#endif	// __TWN_LOGIN0816
	pData->dwIP         = GetPlayerAddr( dpid );
	pData->dpId			= dpid;
	pData->nQueryMode	= CERTIFY;
	g_DbManager.PostQ( pData );
}

void CDPCertifier::OnPing( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
	g_dpAccountClient.SendPing( dpid );
}

void CDPCertifier::OnCloseExistingConnection( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
	char pszAccount[MAX_ACCOUNT], pszPwd[MAX_PASSWORD];
	ar.ReadString( pszAccount, MAX_ACCOUNT );
	ar.ReadString( pszPwd, MAX_PASSWORD );

	if( pszAccount[0] == '\0' )
		return;

	LPDB_OVERLAPPED_PLUS pData	= g_DbManager.m_pDbIOData->Alloc();
	strcpy( pData->AccountInfo.szAccount, pszAccount );
	strcpy( pData->AccountInfo.szPassword, pszPwd );
	_tcslwr( pData->AccountInfo.szAccount );
	pData->dwIP			= 0;		// don't use
	pData->dpId			= dpid;
	pData->nQueryMode	= CLOSE_EXISTING_CONNECTION;
	g_DbManager.PostQ( pData );
}

void CDPCertifier::OnKeepAlive( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
	CUserMng* pMng	= CUserMng::GetInstance();
	CMclAutoLock	Lock( pMng->m_AddRemoveLock );
	
	CUser* pUser	= pMng->GetUser( dpid );
	if( pUser )
		pUser->m_bValid		= TRUE;
}
/*________________________________________________________________________________*/
CDPCertifier	g_dpCertifier;