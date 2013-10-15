// InstanceDungeonParty.cpp: implementation of the CInstanceDungeonParty class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InstanceDungeonParty.h"
#if __VER >= 14 // __INSTANCE_DUNGEON

#ifdef __WORLDSERVER
#include "Mover.h"
#include "Obj.h"
#include "User.h"
#endif // __WORLDSERVER

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInstanceDungeonParty::CInstanceDungeonParty()
: CInstanceDungeonBase( IDTYPE_PARTY )
{
}

CInstanceDungeonParty::~CInstanceDungeonParty()
{
#ifdef __WORLDSERVER
	m_mapPLTime.clear();
#endif // __WORLDSERVER
}

CInstanceDungeonParty* CInstanceDungeonParty::GetInstance()
{
	static CInstanceDungeonParty sIDP;
	return & sIDP;
}

#ifdef __WORLDSERVER
BOOL CInstanceDungeonParty::EnteranceDungeon( CUser* pUser, DWORD dwWorldId )
{
	if( pUser->m_idparty > 0 )
		return TeleportToDungeon( pUser, dwWorldId, pUser->m_idparty );
	else
		pUser->AddDefinedText( TID_GAME_INSTANCE_PARTY );
	
	return FALSE;
}

BOOL CInstanceDungeonParty::LeaveDungeon( CUser* pUser, DWORD dwWorldId )
{
	return LeaveToOutside( pUser, dwWorldId, pUser->GetLayer() );
}

BOOL CInstanceDungeonParty::SetPartyLeaveTime( DWORD dwPlayerId )
{
	CUser* pUser = static_cast<CUser*>( prj.GetUserByID( dwPlayerId ) );
	if( !IsValidObj( pUser ) )
		return FALSE;
	
	CWorld* pWorld = pUser->GetWorld();
	if( !pWorld )
		return FALSE;
	
	if( CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( pWorld->GetID() ) )
	{
		MAP_PLTIME::iterator it = m_mapPLTime.find( dwPlayerId );
		if( it == m_mapPLTime.end() )
			m_mapPLTime.insert( MAP_PLTIME::value_type( dwPlayerId, GetTickCount() + MIN( 3 ) ) );
		else
			it->second = GetTickCount() + MIN( 3 );
	}
	return TRUE;
}

void CInstanceDungeonParty::Process()
{
	DWORD dwTickCount = GetTickCount();
	for( MAP_PLTIME::iterator it=m_mapPLTime.begin(); it!=m_mapPLTime.end(); )
	{
		if( it->second < dwTickCount )
		{
			CUser* pUser = static_cast<CUser*>( prj.GetUserByID( it->first ) );
			m_mapPLTime.erase( it++ );

			if( !IsValidObj( pUser ) )
				return;
			
			CWorld* pWorld = pUser->GetWorld();
			if( !pWorld )
				return;
			
			if( pUser->GetLayer() != pUser->m_idparty && IsPartyDungeon( pWorld->GetID() ) )
				CInstanceDungeonHelper::GetInstance()->GoOut( pUser );
		}
		else
			it++;
	}
}
#endif // __WORLDSERVER

#endif // __INSTANCE_DUNGEON