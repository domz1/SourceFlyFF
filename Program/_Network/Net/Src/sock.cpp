#include "stdafx.h"
#include "sock.h"

CSock::CSock()
{
	m_hSocket	= INVALID_SOCKET;
	m_dpid	= m_dpidpeer	= DPID_UNKNOWN;
}

CSock::~CSock()
{
	Clear();
}

BOOL CSock::Create( u_short uPort, int type )
{
	SOCKADDR_IN sin;

	Clear();

	if( ( m_hSocket = WSASocket( AF_INET, type, 0, NULL, 0, WSA_FLAG_OVERLAPPED ) ) == INVALID_SOCKET )
	{
		TRACE( "Can't create socket with error %d\n", WSAGetLastError() );
		return FALSE;
	}

	int reuse	= 1;
	setsockopt( m_hSocket, SOL_SOCKET,SO_REUSEADDR, (char FAR*)&reuse, sizeof(int) );
	
	LINGER lLinger;
	lLinger.l_onoff		= 0;
	lLinger.l_linger	= 0;
	setsockopt( m_hSocket, SOL_SOCKET, SO_LINGER, (const char *)&lLinger, sizeof(LINGER) );

	sin.sin_family	= PF_INET;
	sin.sin_addr.s_addr		= htonl( INADDR_ANY );
	sin.sin_port	= htons( uPort );

	if( bind( m_hSocket, (LPSOCKADDR) &sin, sizeof(sin) ) == SOCKET_ERROR )
	{
		TRACE( "Can't bind socket with error %d\n", WSAGetLastError() );
		return FALSE;
	}

	return TRUE;
}

void CSock::Clear( void )
{
	CLOSE_SOCKET( m_hSocket );
	m_dpid	= m_dpidpeer	= DPID_UNKNOWN;
}

void CSock::Attach( SOCKET hSocket )
{
	Clear();
	m_hSocket	= hSocket;
}

void CSock::Detach( void )
{
	m_hSocket	= INVALID_SOCKET;
	m_dpid	= m_dpidpeer	= DPID_UNKNOWN;
}

HRESULT	CSock::GetHostAddr( LPVOID lpAddr, LPDWORD lpdwSize )
{
	ASSERT( lpAddr );

	char name[255];
	LPHOSTENT lphost;

	gethostname( name, sizeof(char)*255 );

	SOCKADDR_IN sin;
	ZeroMemory( &sin, sizeof(SOCKADDR_IN) );

	sin.sin_family	= AF_INET;
	lphost	= gethostbyname( name );

	if( lphost != NULL )
		sin.sin_addr.s_addr		= ( (LPIN_ADDR)lphost->h_addr )->s_addr;
	else
		return DPERR_INVALIDPARAM;

	sprintf( (char *)lpAddr, "%d.%d.%d.%d", sin.sin_addr.s_net
																	, sin.sin_addr.s_host
																	, sin.sin_addr.s_lh
																	, sin.sin_addr.s_impno );
	return DP_OK;
}
