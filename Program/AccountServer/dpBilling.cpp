#include "stdafx.h"
#include "account.h"
#include "dpsrvr.h"
#include "DPBilling.h"
#include "BillingMgr.h"
// TODO_BILLING : KEEPALIVE 5분마다 보내기만 한다. 이를 개선해야 

extern	CDPSrvr			g_dpSrvr;
extern	CAccountMng		g_AccountMng;
static char				g_szServerName[] = "AccountServer";

const DWORD				MAX_ID_LENGTH	= 30;
const DWORD				MAX_IP_LENGTH	= 16;
const DWORD				MAX_SERVERNAME	= 34;
const int				ENDMARK_LENGTH  = 9;		// @flyff.jp의 길이 

enum 
{ 
	GAME_START_SEND		= 0x00000001,
	GAME_START_RECEIVE	= 0x00010001,
	KEEP_ALIVE_RECEIVE	= 0x00010004,
	KEEP_ALIVE_SEND		= 0x00000004,
	GSCS_START_SEND		= 0x00000005,
};

// 문자열에 @flyff.jp를 붙인다.
void AppendEndMark( LPTSTR lpszDst, LPCTSTR szSrc, size_t length )
{
	static char szEndMark[] = "@flyff.jp";

	char* p = StrCpyExcludeNull( lpszDst, szSrc );
	memcpy( p, szEndMark, ENDMARK_LENGTH );
	p += ENDMARK_LENGTH;

	AppendSpace( p, lpszDst + length );
}

////////////////////////////////////////////////////////////////////////////////////////////
// CDPBilling
////////////////////////////////////////////////////////////////////////////////////////////
CDPBilling::CDPBilling()
{
	m_hWnd			= NULL;
	m_bConnected	= FALSE;

	BEGIN_MSG;
	ON_MSG( KEEP_ALIVE_RECEIVE, OnKeepAliveReceive );
	ON_MSG( GAME_START_RECEIVE, OnGameStartReceive );
}

CDPBilling::~CDPBilling()
{
}

void CDPBilling::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_CREATEPLAYERORGROUP:
			{
				LPDPMSG_CREATEPLAYERORGROUP lpCreatePlayer	= (LPDPMSG_CREATEPLAYERORGROUP)lpMsg;
				TRACE("billing server connected.\n");
				SendGSCSStart( );
				SetTimer( m_hWnd, IDT_KEEPALIVE, 1000 * 60 * 5, NULL );

				m_bConnected = TRUE;
				break;
			}
		case DPSYS_DESTROYPLAYERORGROUP:
			{
				LPDPMSG_DESTROYPLAYERORGROUP lpDestroyPlayer	= (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
				TRACE("billing server disconnected.\n");
				KillTimer( m_hWnd, IDT_KEEPALIVE );
				::PostMessage( m_hWnd, WM_CONNENT_BILLING, 0, 0 );

				m_bConnected = FALSE;
				break;
			}
	}
}

void CDPBilling::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	CAr ar( (LPBYTE)lpMsg, dwMsgSize );
	DWORD dw;
	ar >> dw;
	
	void ( theClass::*pfn )( theParameters ) =	GetHandler( ntohl( dw ) );
	
	if( pfn )
		( this->*( pfn ) )( ar, idFrom );	
	else
	{
		char szBuffer[1024];
		sprintf( szBuffer, "abnormal billing protocol: %x\n", ntohl( dw ) ); 
		OutputDebugString( szBuffer );
	}
}

// 과금정보 REQ
void CDPBilling::SendGameStart( LPCTSTR lpszUserID, LPCTSTR lpszUserIP )
{
	OutputDebugString("send 0x00000001 - GAME_START\n");

	char cbUserID[MAX_ID_LENGTH];
	char cbUserIP[MAX_IP_LENGTH];

	AppendEndMark( cbUserID, lpszUserID, MAX_ID_LENGTH );
	char* p = StrCpyExcludeNull( cbUserIP, lpszUserIP );
	AppendSpace( p, cbUserIP + MAX_IP_LENGTH );

	CAr	ar;
	int nBufSize;
	ar << htonl( static_cast<DWORD>(GAME_START_SEND) );
	ar.Write( cbUserID, MAX_ID_LENGTH );
	ar.Write( cbUserIP, MAX_IP_LENGTH );
	SEND( ar, this, DPID_SERVERPLAYER );
}

// 과금정보의 ACK
void CDPBilling::OnGameStartReceive( CAr & ar, DPID dpid )
{
	char			szAccount[MAX_ID_LENGTH];
	char			szTimeOverDays[13];
	long			lRemainDays;
	long			lResult;

	ar >> lResult;
	ar.Read( szAccount, MAX_ID_LENGTH );
	ar >> lRemainDays;
	ar.Read( szTimeOverDays, 12 );
	szTimeOverDays[12] = '\0';

	lResult		= ntohl( lResult );		// 호스트 바이 오더로 변환한다.
	lRemainDays = ntohl( lRemainDays );

	// @flyff.jp가 붙은 유저 문자열에서 @flyff.jp를 제거한다.
	for( int i=0; i<MAX_ID_LENGTH; ++i )
	{
		if( szAccount[i] == 0x20 )
			break;
	}
	i -= ENDMARK_LENGTH;				// ENDMARK_LENGTH = strlen("@flyff.jp");
	szAccount[i] = '\0';

	char szBuffer[1024];
	sprintf(szBuffer, "recv 0x00010001 - GAME_START_RECEIVE\nstatus:%d user:%s remainday:%d enddate:%s\n",		     
		               lResult, szAccount, lRemainDays, szTimeOverDays );
	OutputDebugString( szBuffer );

	CTime tmOverDays = 0;
	if( lResult == SUCCESS )
	{
		char sYear[3]	= { 0, };
		char sMonth[3]	= { 0, };
		char sDay[3]	= { 0, };
		char sHour[3]	= { 0, };
		char sMin[3]	= { 0, };
		
		strncpy( sYear, szTimeOverDays, 2 );
		strncpy( sMonth, szTimeOverDays + 2, 2 );
		strncpy( sDay, szTimeOverDays + 4, 2 );
		strncpy( sHour, szTimeOverDays + 6, 2 );
		strncpy( sMin, szTimeOverDays + 8, 2 );
		
		tmOverDays	= CTime( 2000 + atoi( sYear ), atoi( sMonth ), atoi( sDay ), 0, 0, 0 );
	}

	BILLING_INFO info;
	info.szAccount = szAccount;
	info.dwKey = 0;
	info.lResult = lResult;
	info.pTimeOverDays = &tmOverDays;	// tmOverDays는 lResult가 SUCCESS인 경우에만 설정되면 된다.
	
	g_AccountMng.SendBillingResult( &info );
}

// KEEPALIVE REQUEST
void CDPBilling::SendKeepAlive()
{
	if( m_bConnected == false )
	{
		OutputDebugString("send KEEP_ALIVE FAIL because disconnected.\n");
		return;
	}

	OutputDebugString("send 0x00000004 - KEEP_ALIVE\n");

	CAr	ar;
	int nBufSize;

	ar << htonl( static_cast<DWORD>(KEEP_ALIVE_SEND) );

	SEND( ar, this, DPID_SERVERPLAYER );
}

// KEEPALIVE ACK
void CDPBilling::OnKeepAliveReceive( CAr & ar, DPID dpid )
{
	OutputDebugString("recv 0x00010004 - KEEP_ALIVE\n");
}

// 빌링서버에 connect될 때, 이를 빌링서버에 알린다.
void CDPBilling::SendGSCSStart()
{
	OutputDebugString("send 0x00000005 - GSCS_START\n");

	char cbServerName[MAX_SERVERNAME];
	AppendEndMark( cbServerName, g_szServerName, MAX_SERVERNAME );
	
	CAr	ar;
	int nBufSize;
	
	ar << htonl( static_cast<DWORD>(GSCS_START_SEND) );
	ar.Write( cbServerName, MAX_SERVERNAME );
	
	SEND( ar, this, DPID_SERVERPLAYER );
}

