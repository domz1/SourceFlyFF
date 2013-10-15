#include "stdafx.h"
#include "DoS.h"

CDoS::CDoS()
{
	m_pPool		= new MemPooler<CONNECTIONREQUEST>( 1024 );
}

CDoS::~CDoS()
{
	for( ULONGMAP::iterator i = begin(); i != end(); ++i )
		m_pPool->Free( i->second );
	clear();
	m_q.clear();

	SAFE_DELETE( m_pPool );
}

PCONNECTIONREQUEST CDoS::Get( u_long addr )
{
	ULONGMAP::iterator i	= find( addr );
	if( i != end() )
		return i->second;
	return NULL;
}

void CDoS::Add( u_long addr )
{
	PCONNECTIONREQUEST ptrtmp;
	if( m_q.size() == MAX_QUEUEITEM )
	{
		u_long ip	= m_q.front();
		ptrtmp	= Get( ip );
		m_pPool->Free( ptrtmp );	// delete
		erase( ip );	// erase
		m_q.pop_front();	// erase
	}
	ptrtmp	= (PCONNECTIONREQUEST)m_pPool->Alloc();	// add
	ZeroMemory( ptrtmp, sizeof(CONNECTIONREQUEST) );
	ptrtmp->dwHit	= timeGetTime();
	insert( ULONGMAP::value_type( addr, ptrtmp ) );
	m_q.push_back( addr );
}

BOOL CDoS::IsDoS( u_long addr )
{
	PCONNECTIONREQUEST ptr	= Get( addr );
	if( ptr )
	{
		DWORD dwHit	= timeGetTime();
		if( dwHit - ptr->dwHit < 400 )
			ptr->nHit++;
		else
			ptr->dwHit	= dwHit;

		if( ptr->nHit < MAX_HIT )
			return FALSE;
		return TRUE;
	}
	else
	{
		Add( addr );
		return FALSE;
	}
}