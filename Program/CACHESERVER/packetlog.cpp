#include "stdafx.h"
#include "packetlog.h"
#include <vector>
#include <algorithm>

CPacketLog::CPacketLog()
{
}

CPacketLog::~CPacketLog()
{
}

CPacketLog* CPacketLog::Instance()
{
	static CPacketLog	sPacketLog;
	return &sPacketLog;
}

void CPacketLog::Add( DPID dpid, DWORD dwHdr, DWORD dwBytes )
{
	CMclAutoLock lock( m_AccessLock );

	MPLR::iterator i	= m_players.find( dpid );
	if( i == m_players.end() )
	{
		MPAR mpar;
		mpar.insert( MPAR::value_type( dwHdr, PacketRecvd( dwHdr, dwBytes ) ) );
		m_players.insert( MPLR::value_type( dpid, PlayerRecvd( dpid, dwBytes, mpar ) ) );
	}
	else
	{
		i->second.dwTotalBytes	+= dwBytes;
		++i->second.dwTotalPackets;
		MPAR& mpar	= i->second.mpar;
		MPAR::iterator j = mpar.find( dwHdr );
		if( j == mpar.end() )
		{
			mpar.insert( MPAR::value_type( dwHdr, PacketRecvd( dwHdr, dwBytes ) ) );
		}
		else
		{
			j->second.dwPackets++;
			j->second.dwBytes	+= dwBytes;
		}
	}
}

void CPacketLog::Reset()
{
	CMclAutoLock lock( m_AccessLock );
	m_players.clear();
	Error( "[CPacketLog.Reset]" );
}

bool pllpr1( PlayerRecvd & l, PlayerRecvd & r )
{
	return l.dwTotalBytes > r.dwTotalBytes;
}

bool pllpr2( PacketRecvd & l, PacketRecvd & r )
{
	return l.dwBytes > r.dwBytes;
}

#define	MAX_DETAIL_PACKETLOG	100
void CPacketLog::Print()
{
	CMclAutoLock lock( m_AccessLock );

	VPLR	vPlayers;
	DWORD dwTotalBytes	= 0, dwTotalPackets	= 0;
	for( MPLR::iterator i = m_players.begin(); i != m_players.end(); ++i )
	{
		dwTotalBytes	+= i->second.dwTotalBytes;
		dwTotalPackets	+= i->second.dwTotalPackets;
		vPlayers.push_back( i->second );
	}
	std::sort( vPlayers.begin(), vPlayers.end(), pllpr1 );

	Error( "\r\n\r\nTotalBytesReceived: %d(bytes)\t%d(packets)", dwTotalBytes, dwTotalPackets );

	if( dwTotalBytes == 0 )
		dwTotalBytes	= 1;
	FILE* f;
	if( ( f = fopen( "error.txt","a" ) ) )
	{
		int c	= 0;
		for( VPLR::iterator j = vPlayers.begin(); j != vPlayers.end(); ++j )
		{
			VPAR vPackets;
			for( MPAR::iterator k = j->mpar.begin(); k != j->mpar.end(); ++k )
				vPackets.push_back( k->second );
			std::sort( vPackets.begin(), vPackets.end(), pllpr2 );

			fprintf( f, "\t(%3.2f%%) %d(socket) %d(bytes) %d(packets)\r\n", (float)j->dwTotalBytes * 100 / (float)dwTotalBytes, j->dpid, j->dwTotalBytes, j->dwTotalPackets );
			if( j->dwTotalBytes == 0 )
				j->dwTotalBytes	= 1;
			if( c++ < MAX_DETAIL_PACKETLOG )
			{
				for( VPAR::iterator m = vPackets.begin(); m != vPackets.end(); ++m )
					fprintf( f, "\t\t(%3.2f%%)\t%08X: %d(bytes) %d(packets)\r\n", (float)m->dwBytes * 100 / (float)j->dwTotalBytes, m->dwHdr, m->dwBytes, m->dwPackets );
			}
		}
	}
	fclose( f );
}