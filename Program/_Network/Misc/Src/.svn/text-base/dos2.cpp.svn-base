#include "stdafx.h"
#include "dos2.h"

CDos2::CDos2()
{
	m_pPool		= new MemPooler<CONNECTIONREQUEST2>( 1024 );
	m_nAddr		= 0;
	m_timer.Set( MIN( 10 ) );
//	m_timer.Set( SEC( 10 ) );
}

CDos2::~CDos2()
{
	for( map<u_long, PCONNECTIONREQUEST2>::iterator i = begin(); i != end(); ++i )
		m_pPool->Free( i->second );
	clear();
	SAFE_DELETE( m_pPool );
}

PCONNECTIONREQUEST2 CDos2::Get( u_long addr )
{
	map<u_long, PCONNECTIONREQUEST2>::iterator i	= find( addr );
	if( i != end() )
		return i->second;
	return NULL;
}

void CDos2::Add( u_long addr )
{
	PCONNECTIONREQUEST2 ptrtmp	= (PCONNECTIONREQUEST2)m_pPool->Alloc();	// add
	memset( ptrtmp, 0, sizeof(CONNECTIONREQUEST2 ) );
	ptrtmp->ip	= addr;
	ptrtmp->dwHit	= GetTickCount();
	insert( map<u_long, PCONNECTIONREQUEST2>::value_type( addr, ptrtmp ) );
}

int CDos2::IsOver( u_long addr )
{
	Release();

	PCONNECTIONREQUEST2 ptr		= Get( addr );
	if( ptr )
	{
		DWORD dwHit	= GetTickCount();
		if( dwHit - ptr->dwHit < 200 )
			ptr->nHit++;
		else
			ptr->dwHit	= dwHit;

		if( ptr->nHit < MAX_HIT )
		{
			return 0;
		}
		else
		{
			if( !ptr->b )
			{
				if( m_nAddr < MAX_ADDR )
				{
					m_adwAddr[m_nAddr++]	= ptr->ip;
					ptr->b	= TRUE;
					return -1;
				}
				return 1;
			}
			else	// > 0
			{
				return 1;
			}
		}
	}
	else
	{
		Add( addr );
		return 0;
	}
}

void CDos2::Release( void )
{
	if( m_timer.IsTimeOut() )
	{
		m_timer.Reset();
		for( int i = 0; i < m_nAddr; i++ )
		{
			PCONNECTIONREQUEST2 ptrtmp	= Get( m_adwAddr[i] );
			if( ptrtmp )
			{
				ptrtmp->b	= FALSE;
				ptrtmp->nHit	= 0;
			}
		}
		m_nAddr		= 0;
	}
}
