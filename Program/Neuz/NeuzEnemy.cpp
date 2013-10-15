// NeuzEnemy.cpp: implementation of the CNeuzEnemy class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NeuzEnemy.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNeuzEnemy::CNeuzEnemy()
{

}

CNeuzEnemy::~CNeuzEnemy()
{
	Clear();
}

DWORD CNeuzEnemy::Add( OBJID objid )
{
	DWORD dwLast = 0;
	MAP_ENEMY::iterator it = m_Enemy.find( objid );
	if( it != m_Enemy.end() )
	{
		dwLast = it->second;
		it->second = GetTickCount();
	}
	else
		m_Enemy.insert( make_pair(objid, GetTickCount() ) );

	return dwLast;
}
void CNeuzEnemy::Remove( OBJID objid )
{
	m_Enemy.erase( objid );
}

DWORD CNeuzEnemy::Find( OBJID objid )
{
	MAP_ENEMY::iterator it = m_Enemy.find( objid );
	if( it != m_Enemy.end() )
	{
		return it->second;
	}
	
	return 0;	
}

BOOL CNeuzEnemy::IsPKing( OBJID objid )
{
	DWORD dwTick = Find( objid );
	if( dwTick == 0 )
		return FALSE;

	if( ( GetTickCount() - dwTick ) <= MIN( 1 ) )
	{
		return TRUE;
	}

	return FALSE;
}

void CNeuzEnemy::Clear()
{
	m_Enemy.clear();
}

void CNeuzEnemy::CheckInvalid()
{
	DWORD dwTick = GetTickCount();
	MAP_ENEMY::iterator it = m_Enemy.begin();
	for( ; it != m_Enemy.end(); )
	{
		if( ( dwTick - it->second ) > MIN( 1 ) )
		{
			m_Enemy.erase( it++ );
		}
		else
			++it;
	}
}