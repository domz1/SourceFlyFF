#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "dpcertified.h"
#include "wndbase.h"
#include "wndcloseexistingconnection.h"
#include "Network.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
static BOOL		g_bRecvSvrList	= FALSE;
CDPCertified	g_dpCertified;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDPCertified::CDPCertified()
{
	m_timer.Set( MIN( 1 ) );
	m_lError = 0;

	BEGIN_MSG;
	ON_MSG( PACKETTYPE_SRVR_LIST, OnSrvrList );
	ON_MSG( PACKETTYPE_ERROR, OnError );
#ifdef __GPAUTH
	ON_MSG( PACKETTYPE_ERROR_STRING, OnErrorString );
#endif	// __GPAUTH
	ON_MSG( PACKETTYPE_KEEP_ALIVE, OnKeepAlive );
}

CDPCertified::~CDPCertified()
{

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////



LONG CDPCertified::GetNetError()
{
	return m_lError;
}

// 접속 끊김을 표시해야 하는가?
BOOL CDPCertified::CheckNofityDisconnected()
{
	//서버 리스트를 받았으면 서버 끊김을 표시하지 않는다.
	if( g_bRecvSvrList )
		return FALSE;

	// 에러로 끊기면 표시하지 않는다.
	if( m_lError )
		return FALSE;

	return TRUE;
}

void CDPCertified::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpId )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_CREATEPLAYERORGROUP:
			{
				m_fConn		= TRUE;
				g_bRecvSvrList	= FALSE;
			}
			break;
		case DPSYS_DESTROYPLAYERORGROUP:
			{
				LPDPMSG_DESTROYPLAYERORGROUP lpDestroyPlayer	= (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
				m_lError = lpDestroyPlayer->dwFlags;
			}
			CNetwork::GetInstance().OnEvent( CERT_DISCONNECT );

			m_fConn		= FALSE;

			if( CheckNofityDisconnected() )		// 접속 끊김을 표시해야 하는가?
			{
				g_WndMng.CloseMessageBox();
				g_WndMng.OpenMessageBox( prj.GetText(TID_DIAG_0023) );

				CWndLogin* pWndLogin	= (CWndLogin*)g_WndMng.GetWndBase( APP_LOGIN );
				if( pWndLogin )
					pWndLogin->GetDlgItem( WIDC_OK )->EnableWindow( TRUE );
			}

			m_lError = 0;		// 에러코드 clear
			break;
	}
}

void CDPCertified::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpId )
{
	CAr ar( (LPBYTE)lpMsg, dwMsgSize );

	GETTYPE( ar );
	
	void ( theClass::*pfn )( theParameters )	=	GetHandler( dw );

	if( pfn ) {
		( this->*( pfn ) )( ar, dpId );
	}
	else {
		/*
		switch( dw ) {
			default:
				TRACE( "Handler not found(%08x)\n", dw );
				break;
		}
		*/
	}
}

void CDPCertified::SendNewAccount( LPCSTR lpszAccount, LPCSTR lpszpw )
{
	BEFORESEND( ar, PACKETTYPE_NEW_ACCOUNT );
	ar.WriteString( lpszAccount );
	ar.WriteString( lpszpw );
	SEND( ar, this, DPID_SERVERPLAYER );
}

// 2006.03 MSG_VER를 보내는 부분이 5차 국내에는 없음
void CDPCertified::SendCertify()
{
#ifdef __NO_SUB_LANG
	if( ::GetLanguage() == LANG_USA )
#else // __NO_SUB_LANG
	if( ::GetLanguage() == LANG_USA && ::GetSubLanguage() == LANG_SUB_USA )
#endif // __NO_SUB_LANG
		g_Neuz.m_dwTimeOutDis = GetTickCount() + SEC( 40 );
	else
		g_Neuz.m_dwTimeOutDis = GetTickCount() + SEC( 20 );
		
	BEFORESEND( ar, PACKETTYPE_CERTIFY );
	ar.WriteString( ::GetProtocolVersion() );		// MSG_VER 
#ifdef __SECURITY_0628
	ar.WriteString( CResFile::m_szResVer );
#endif	// __SECURITY_0628
	ar.WriteString( g_Neuz.m_szAccount );

	//////////////////////////////////////////////////////////////////////////
	//	mulcom	BEGIN100218	패스워드 암호화
#ifdef __ENCRYPT_PASSWORD
	char	szPlain[ 16 * MAX_PASSWORD ] = {0, };
	char	szEnc[ 16 * MAX_PASSWORD ] = {0, };
	
	::memcpy( szPlain, g_Neuz.m_szPassword, sizeof(g_Neuz.m_szPassword) );
	
	
	g_xRijndael->ResetChain();
	g_xRijndael->Encrypt( szPlain, szEnc, 16 * MAX_PASSWORD, CRijndael::CBC );
	
	ar.Write( szEnc, 16 * MAX_PASSWORD );
#else
	ar.WriteString( g_Neuz.m_szPassword );
#endif
	//	mulcom	END100218	패스워드 암호화


#ifdef __TWN_LOGIN0816
	if( GetLanguage() == LANG_TWN )
	{
		ar.WriteString( g_Neuz.m_szSessionPwd );
//		char lpOutputString[256]	= { 0,};
//		sprintf( lpOutputString, "account = %s, pwd = %s, session = %s", g_Neuz.m_szAccount, g_Neuz.m_szPassword, g_Neuz.m_szSessionPwd );
//		OutputDebugString( lpOutputString );
	}
#endif	// __TWN_LOGIN0816
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCertified::SendCloseExistingConnection( const char* lpszAccount, const char* lpszpw )
{
	BEFORESEND( ar, PACKETTYPE_CLOSE_EXISTING_CONNECTION );
	ar.WriteString( lpszAccount );
	ar.WriteString( lpszpw );
	SEND( ar, this, DPID_SERVERPLAYER );
}

// 서버인덱스를 가지고 서버의 이름을  얻는다.
// nServerIndex - 서버 ListBox에서 선택한 행의 번호 ( 0부터 시작 )
LPCTSTR CDPCertified::GetServerName( int nServerIndex )
{
	int nCount = 0;		// 서버셋의 count
	for( int i = 0; i < (int)( m_dwSizeofServerset ); i++ )
	{
		if( m_aServerset[i].dwParent == NULL_ID )
		{
			if( nCount++ == nServerIndex )	 // nServerIndex(=서버인덱스)는 서버셋의 인덱스와 같은 의미 
			{
				return m_aServerset[i].lpName;
			}
		}
	}
	return "Unknown";
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CDPCertified::OnKeepAlive( CAr & ar, DPID )
{
//	BEFORESENDSOLE( ar2, PACKETTYPE_KEEP_ALIVE, DPID_UNKNOWN );
	BEFORESEND( ar2, PACKETTYPE_KEEP_ALIVE );
	SEND( ar2, this, DPID_SERVERPLAYER );
}


//	Handlers
void CDPCertified::OnSrvrList( CAr & ar, DPID )
{
	g_bRecvSvrList	= TRUE;

	ar >> g_Neuz.m_dwAuthKey;
	ar >> g_Neuz.m_cbAccountFlag;
	long lTimeSpan = 0;
#ifdef __BILLING0712
	// 결재가 1일 미만 남은경우, 유저가 로그인 할때 몇시간 몇분 남았습니다 라는 메세지를 보여주는 것
	ar >> lTimeSpan;
#endif	// __BILLING0712

#ifdef __GPAUTH_01
	if( g_Neuz.m_bGPotatoAuth )
	{
		ar.ReadString( g_Neuz.m_szGPotatoNo, 10 );
#ifdef __GPAUTH_02
		ar.ReadString( g_Neuz.m_szCheck, 255 );
#endif	// __GPAUTH_02
	}
#ifdef __EUROPE_0514
	char szBak[MAX_ACCOUNT]	= { 0,};
	ar.ReadString( szBak, MAX_ACCOUNT );
	if( lstrcmp( g_Neuz.m_szAccount, szBak ) )
	{
		Error( "CDPCertified.OnSrvrList" );
		exit( 0 );
	}
#endif	// __EUROPE_0514		
#endif	// __GPAUTH_01

	if( ::GetLanguage() == LANG_THA )
	{
		long lTimeLeft;	
		ar >> lTimeLeft;		// 태국의 경우 돌아오는 22:00시까지의 남은 시간을 받는다.
		g_Neuz.SetLeftTime( lTimeLeft );
	}

	// 독일 테스트 서버 현지 접속인가?
	CString strAddr	= g_Neuz.m_lpCertifierAddr;
	BOOL bPrivate	= ( ::GetLanguage() == LANG_GER && strAddr.Find( "192.168", 0 ) == 0 );

	ar >> m_dwSizeofServerset;
	LPSERVER_DESC pServer;
	for( int i = 0; i < (int)( m_dwSizeofServerset ); i++ )
	{
		pServer		= m_aServerset + i;
		ar >> pServer->dwParent;
		ar >> pServer->dwID;
		ar.ReadString( pServer->lpName, 36 );
		ar.ReadString( pServer->lpAddr, 16 );

		// 독일 테스트 서버로의 현지 접속이고 ip가 포함된 서버 정보라면,
		if( bPrivate && pServer->lpAddr[0] != '\0' )
			lstrcpy( pServer->lpAddr, g_Neuz.m_lpCertifierAddr );

		ar >> pServer->b18;
		ar >> pServer->lCount;
		ar >> pServer->lEnable;
		ar >> pServer->lMax;
	}
	CNetwork::GetInstance().OnEvent( CERT_SRVR_LIST );

	CWndBase* pWndBase	= g_WndMng.GetWndBase( APP_LOGIN );
	if( pWndBase )
		( (CWndLogin*)pWndBase )->Connected( lTimeSpan );
}

#ifdef __GPAUTH
void CDPCertified::OnErrorString( CAr & ar, DPID dpid )
{
#ifdef __JAPAN_AUTH
	g_Neuz.m_dwTimeOutDis = 0xffffffff;			// 타임 아웃 메세지 박스 표시를 막는다.
#endif // __JAPAN_AUTH

	char szError[256]	= { 0,};
	ar.ReadString( szError, 256 );

	g_WndMng.CloseMessageBox();
	g_WndMng.OpenMessageBox( szError );
		
	CWndLogin* pWndLogin	= (CWndLogin*)g_WndMng.GetWndBase( APP_LOGIN );
	if( pWndLogin )
		pWndLogin->GetDlgItem( WIDC_OK )->EnableWindow( TRUE );

	// 에러 표시 후 소켓이 끊기게 패킷을 보낸다. shutdown이 되지 않는 이 아픔 
	// BEFORESENDSOLE( arWrite, PACKETTYPE_ERROR, DPID_UNKNOWN );
	BEFORESEND( arWrite, PACKETTYPE_ERROR);		// chipi_090219
	SEND( arWrite, this, DPID_SERVERPLAYER );
}
#endif	// __GPAUTH

void CDPCertified::OnError( CAr & ar, DPID dpid )
{
	g_Neuz.m_dwTimeOutDis = 0xffffffff;			// 타임 아웃 메세지 박스 표시를 막는다.
	int nText = 0;

	ar >> m_lError;

	CNetwork::GetInstance().OnEvent( CERT_ERROR );

	switch( m_lError )
	{
		case ERROR_DUPLICATE_ACCOUNT:			// 103L
			{
				g_WndMng.CloseMessageBox();
				g_WndMng.m_pWndCloseExistingConnection	= new CWndCloseExistingConnection;
				g_WndMng.m_pWndCloseExistingConnection->Initialize();
				return;
			}

		case ERROR_ILLEGAL_VER:
			nText = TID_DIAG_0035;
			break;
#ifdef __SECURITY_0628
		case ERROR_FLYFF_RESOURCE_MODIFIED:
			nText	= TID_GAME_RESOURCE_MODIFIED;
			break;
#endif	// __SECURITY_0628
		case ERROR_ACCOUNT_EXISTS:				// 100L 이미 같은 이름의 계정이 있습니다.
			nText = TID_DIAG_0032;
			break;
		case ERROR_FLYFF_PASSWORD:				// 120L (비밀 번호가 틀립니다.)
			nText = TID_DIAG_0016;
			break;
		case ERROR_FLYFF_ACCOUNT:				// 121L (잘못된 계정입니다.)
			nText = TID_DIAG_0038;
			break;
		case ERROR_OVERFLOW:					// 108L 접속자가 너무 많습니다.
			nText = TID_DIAG_0041;
			break;
		case ERROR_EXTERNAL_ADDR:				// 109L 현재 서비스중이 아닙니다.
			nText = TID_DIAG_0053;
			break;
		case ERROR_BLOCKGOLD_ACCOUNT:			// 119L 블럭된 계정입니다.
			nText = TID_DIAG_0015;
			break;
		case ERROR_FLYFF_AUTH:					// 122L 실명인증후 게임접속이 가능합니다 www.flyff.com으로 접속해주십시오
			nText = TID_DIAG_0026;
			break;
		case ERROR_FLYFF_PERMIT:				// 123L 프리프는 12세 이상 이용가 이므로 게임접속을 할수 없습니다.
			nText = TID_DIAG_0050;
			break;		
		case ERROR_FLYFF_NEED_AGREEMENT:		// 124L 14세 미만 가입자 분들은 부모님 동의서를 보내주셔야 게임 접속이 가능합니다. www.flyff.com 으로 접속하셔서 확인해 주세요.
			nText = TID_DIAG_0001;
			break;
		case ERROR_FLYFF_NO_MEMBERSHIP:			// 125L	 웹에서 탈퇴한 계정입니다. www.flyff.com 으로 접속하셔서 확인해 주세요." ) );
			nText = TID_GAME_ACCOUNTWEBRETIRE;
			break;		
		case ERROR_BILLING_INFO_FAILED:			// 빌링 정보 없음
			nText = TID_DIAG_NOCHARGING;
			break;
		case ERROR_BILLING_DATABASE_ERROR:		// 빌링 DB 에러 
			nText = TID_DIAG_DBERROR1;
			break;
		case ERROR_BILLING_TIME_OVER:			// 빌링 사용시간 만료 
			nText = TID_DIAG_EXPIRY;
			break;		
		case ERROR_BILLING_OTHER_ERROR:			// 빌링 기타 다른 에러 
			nText = TID_DIAG_DBERROR2;
			break;
		case ERROR_BILLING_DISCONNECTED:
			nText = TID_DIAG_DBERROR2;
			break;
		case ERROR_TOO_LATE_PLAY:				// 131L
			nText = TID_GAME_LIMITCONNECTION;
			break;
		case ERROR_IP_CUT:						// 132L
			nText = TID_GAME_OTHERCOUNTRY; 
			break;
		case ERROR_FLYFF_DB_JOB_ING:			// 133L
			nText = TID_DB_INSPECTION;
			break;
		case ERROR_15SEC_PREVENT:				// 134L
			nText = TID_15SEC_PREVENT;
			break;
		case ERROR_15MIN_PREVENT:				// 135L 
			nText = TID_15MIN_PREVENT;
			break;
		case ERROR_CERT_GENERAL:
			nText = TID_ERROR_CERT_GENERAL;
			break;
		case ERROR_FLYFF_EXPIRED_SESSION_PASSWORD:
			nText	= TID_ERROR_EXPIRED_SESSION_PASSWORD;
			break;
	}

	if( nText > 0 )
	{
		g_WndMng.CloseMessageBox();
		g_WndMng.OpenMessageBox( _T( prj.GetText(nText) ) );
		
		CWndLogin* pWndLogin	= (CWndLogin*)g_WndMng.GetWndBase( APP_LOGIN );
		if( pWndLogin )
			pWndLogin->GetDlgItem( WIDC_OK )->EnableWindow( TRUE );

		// 에러 표시 후 소켓이 끊기게 패킷을 보낸다. shutdown이 되지 않는 이 아픔 
		//BEFORESENDSOLE( ar, PACKETTYPE_ERROR, DPID_UNKNOWN );
		BEFORESEND( ar, PACKETTYPE_ERROR );		// chipi_090219 
		SEND( ar, this, DPID_SERVERPLAYER );
	}
}


