#include "stdafx.h"
#include "WantedListSnapshot.h"

CWantedListSnapshot& CWantedListSnapshot::GetInstance()
{
	static CWantedListSnapshot wantedList;
	return wantedList;
}
//////////////////////////////////////////////////////////////////////
// CWantedListSnapshot
//////////////////////////////////////////////////////////////////////

CWantedListSnapshot::CWantedListSnapshot()
{
	m_lRecvTime = 0;
}

CWantedListSnapshot::~CWantedListSnapshot()
{
}

//////////////////////////////////////////////////////////////////////
// members
//////////////////////////////////////////////////////////////////////

void CWantedListSnapshot::Read( CAr& ar )
{
	m_wantedList.clear();

	int nCount;				// 리스트의 갯수 

	ar >> m_lRecvTime;
	ar >> nCount;						
	for( int i=0; i<nCount; i++)
	{
		WANTED_ENTRY	entry;
		ar.ReadString( entry.szPlayer, 64 );
		ar >> entry.nGold;
		ar >> entry.nEnd; 
		ar.ReadString( entry.szMsg, WANTED_MSG_MAX + 1 );

		m_wantedList.push_back( entry );
	}	
}

void CWantedListSnapshot::Write( CAr& ar )
{
	ar << m_lRecvTime;
	ar << (int)m_wantedList.size();

	WANTED_ENTRY_VECTOR::iterator it;
	for( it = m_wantedList.begin(); it != m_wantedList.end(); ++it )
	{
		ar.WriteString( (*it).szPlayer );
		ar << (*it).nGold;
		ar << (*it).nEnd; 
		ar.WriteString( (*it).szMsg );
	}
}

// nIndex에 해당되는 플레이어의 이름을 얻는다.
int CWantedListSnapshot::GetPlayerIndex( LPCTSTR szPlayer )
{
	int nCount = m_wantedList.size();

	for( int i=0; i<nCount; ++i )
	{
		if( strcmp( szPlayer, m_wantedList[i].szPlayer) == 0 )
			return i;
	}

	return -1;
}