#include "stdafx.h"
#include "dpmonitorclient.h"

CDPMonitorClient::CDPMonitorClient()
{
	InitializeCriticalSectionAndSpinCount( &m_cs, 2000 );
	m_tmPing.Set( SEC( 5 ) );
}

CDPMonitorClient::~CDPMonitorClient()
{
	DeleteCriticalSection( &m_cs );
}

void CDPMonitorClient::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpid )
{
}

void CDPMonitorClient::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpid )
{
}

BOOL CDPMonitorClient::Run( LPCSTR lpAddr, u_long uPort, DWORD dwMonitorId, BOOL fsndPing )
{
	if( FALSE == ConnectToServer( lpAddr, uPort, FALSE ) )
		return FALSE;

	BEFORESEND( ar, PACKETTYPE_MONITOR_ID );
	ar << dwMonitorId << fsndPing;
	SEND( ar, this, DPID_SERVERPLAYER );

	return TRUE;
}

void CDPMonitorClient::Ping( void )
{
	EnterCriticalSection( &m_cs );
	if( m_tmPing.IsTimeOut() )
	{
		m_tmPing.Reset();
		LeaveCriticalSection( &m_cs );
		SendHdr( PACKETTYPE_PING );
		return;
	}
	LeaveCriticalSection( &m_cs );
}

CDPMonitorClient	g_dpMonitorClient;