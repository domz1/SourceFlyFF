#include "stdafx.h"
#include "speedhackchecker.h"

CSpeedHackChecker::CSpeedHackChecker()
{
	m_tBaseOfServer	= m_tBaseOfClient	= 0;
	m_fKill		= FALSE;
}

CSpeedHackChecker::~CSpeedHackChecker()
{

}

void CSpeedHackChecker::sETbaseOfServer( CDPMng* pDPMng, DPID dpidPlayer )
{
	m_tBaseOfServer		= timeGetTime();

	BEFORESEND( ar, PACKETTYPE_GET_CLOCK );
	ar << (BYTE)1;
	SEND( ar, pDPMng, dpidPlayer );
}

//void CSpeedHackChecker::sETbaseOfClient( time_t tBaseOfClient )
void CSpeedHackChecker::sETbaseOfClient( DWORD tBaseOfClient )
{
	m_tBaseOfClient	= tBaseOfClient;
}

void CSpeedHackChecker::rEQoffsetOfClient( CDPMng* pDPMng, DPID dpidPlayer )
{
	BEFORESEND( ar, PACKETTYPE_GET_CLOCK );
	ar << (BYTE)0;
	SEND( ar, pDPMng, dpidPlayer );
}

//BOOL CSpeedHackChecker::CheckClock( time_t tCurClient, CDPMng* pDPMng, DPID dpidPlayer )
BOOL CSpeedHackChecker::CheckClock( DWORD tCurClient, CDPMng* pDPMng, DPID dpidPlayer )
{
	if( m_fKill )
		return pDPMng->DestroyPlayer( dpidPlayer );

	DWORD tOffsetOfServer	= timeGetTime() - m_tBaseOfServer;	// > 0
	DWORD tOffsetOfClient	= tCurClient - m_tBaseOfClient;	// > 0
//	if( tOffsetOfClient > tOffsetOfServer + 1000 )
	if( tOffsetOfClient > tOffsetOfServer + 3000 )	//	10%, 30(s)
	{
		BEFORESEND( ar, PACKETTYPE_ERROR );
		ar << ERROR_INVALID_CLOCK;
		SEND( ar, pDPMng, dpidPlayer );
		return TRUE;
	}
	return FALSE;
}