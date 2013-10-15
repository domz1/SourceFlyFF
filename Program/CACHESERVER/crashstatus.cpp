#include "stdafx.h"
#include "crashstatus.h"

#ifdef __CRASH_0404

#include "player.h"
#include "dpmng.h"
#include "psapi.h"

#include <string>


CCrashStatus::CCrashStatus()
{
	m_nTask		= 0;
	m_nChildAllocated	= 0;
}

CCrashStatus::~CCrashStatus()
{
	Clear();
}

void CCrashStatus::Clear( void )
{
	m_mapLastPacket.clear();
}

CCrashStatus* CCrashStatus::GetInstance( void )
{
	static	CCrashStatus	sCrashStatus;
	return &sCrashStatus;
}

DWORD CCrashStatus::GetMemoryLoad( void )
{
	PROCESS_MEMORY_COUNTERS	psmemCounters;
	GetProcessMemoryInfo( GetCurrentProcess(), &psmemCounters, sizeof(PROCESS_MEMORY_COUNTERS) );
	return psmemCounters.WorkingSetSize;



/*
	MEMORYSTATUS ms;
	memset( &ms, 0, sizeof(MEMORYSTATUS) );
	ms.dwLength	= sizeof(MEMORYSTATUS);
	GlobalMemoryStatus( &ms );
	return ms.dwTotalPageFile - ms.dwAvailPageFile;// percent of memory in use
*/
}

int CCrashStatus::GetPlayerCount( CPlayerMng* pPlayerMng )
{
	return pPlayerMng->GetCount();
}

DWORD	CCrashStatus::GetLastPacket( CDPMng* pdpMng )
{
	CMclAutoLock	Lock( m_AccessLock );
	map<CDPMng*, DWORD>::iterator	i = m_mapLastPacket.find( pdpMng );
	if( i != m_mapLastPacket.end() )
		return i->second;
	return 0;
}

void CCrashStatus::SetLastPacket( CDPMng* pdpMng, DWORD dwHdr )
{
	CMclAutoLock	Lock( m_AccessLock );
	map<CDPMng*, DWORD>::iterator	i = m_mapLastPacket.find( pdpMng );
	if( i != m_mapLastPacket.end() )
		i->second	= dwHdr;
}

void	CCrashStatus::RegisterDPMng( LPCSTR szClass, CDPMng* pdpMng )
{
	CMclAutoLock	Lock( m_AccessLock );
	m_mapDPClass.insert( map<string, CDPMng*>::value_type( szClass, pdpMng ) );
	m_mapLastPacket.insert( map<CDPMng*, DWORD>::value_type( pdpMng, 0 ) );		// initialize
}

void CCrashStatus::PrintStatus( void )
{
	char szStatus[4096]	= { 0,};

	SYSTEMTIME time;
	GetLocalTime( &time );
	sprintf( szStatus, "%d/%2d/%2d %02d:%02d:%02d\r\nMemory Load : %lu byte(s)\r\nChild Allocated : %d\r\nPlayer Count : %d\r\nMain Thread Task : %d\r\n",
		time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, GetMemoryLoad(), GetChildAllocated(), GetPlayerCount( CPlayerMng::Instance() ), GetMainThreadTask() );

	for( map<string, CDPMng*>::iterator i = m_mapDPClass.begin(); i != m_mapDPClass.end(); ++i )
	{
		char s[128]	= { 0,};
		sprintf( s, "\r\n%s : %08x", i->first.data(), GetLastPacket( i->second ) );
		strcat( szStatus, s );
	}

	FILEOUT( "CrashStatus.txt", szStatus );
}

#endif	// __CRASH_0404