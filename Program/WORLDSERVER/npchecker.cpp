#include "stdafx.h"
#include "npchecker.h"

CNpcChecker::CNpcChecker()
{
}

CNpcChecker::~CNpcChecker()
{
}

CNpcChecker* CNpcChecker::GetInstance( void )
{
	static	CNpcChecker	sNpcChecker;
	return &sNpcChecker;
}

void CNpcChecker::AddNpc( CObj* pObj )
{
	if( pObj->GetType() != OT_MOVER )
		return;
	CMover* pNpc	= (CMover*)pObj;
	if( !pNpc->IsNPC() )
		return;
	LPCHARACTER	pCharacter	= pNpc->GetCharacter();
	if( !pCharacter )
		return;

	for( int i = 0; i < MAX_MOVER_MENU; i++ )
	{
		if( pCharacter->m_abMoverMenu[i] )
			m_aset[i].insert( pNpc->GetId() );
	}
}

void CNpcChecker::RemoveNpc( CObj* pObj )
{
	if( pObj->GetType() != OT_MOVER )
		return;
	CMover* pNpc	= (CMover*)pObj;
	if( !pNpc->IsNPC() )
		return;
	LPCHARACTER	pCharacter	= pNpc->GetCharacter();
	if( !pCharacter )
		return;

	OBJID objid		= pNpc->GetId();
	for( int j = 0; j < MAX_MOVER_MENU; j++ )
	{
		set<OBJID>::iterator	i = m_aset[j].find( objid );
		if( i != m_aset[j].end() )
			m_aset[j].erase( i );		
	}
}

BOOL CNpcChecker::IsCloseNpc( int nMenu, CWorld* pWorld, D3DXVECTOR3 & v )
{
// CProject::m_AddRemoveLock
	for( set<OBJID>::iterator i = m_aset[nMenu].begin(); i != m_aset[nMenu].end(); ++i )
	{
		CMover* pNpc	= prj.GetMover( *i );
		if( IsValidObj( pNpc ) )
		{
			if( pWorld != pNpc->GetWorld() )
				continue;
			D3DXVECTOR3 vOut	= v - pNpc->GetPos();
			if( fabs( (double)D3DXVec3LengthSq( &vOut ) ) < MAX_LEN_MOVER_MENU )
				return TRUE;
		}
	}
	return FALSE;
}

BOOL CNpcChecker::IsCloseNpc( CWorld* pWorld, D3DXVECTOR3 & v )
{
	for( int nMenu = 0; nMenu < MAX_MOVER_MENU; nMenu++ )
	{
		for( set<OBJID>::iterator i = m_aset[nMenu].begin(); i != m_aset[nMenu].end(); ++i )
		{
			CMover* pNpc	= prj.GetMover( *i );
			if( IsValidObj( pNpc ) )
			{
				if( pWorld != pNpc->GetWorld() )
					continue;
				D3DXVECTOR3 vOut	= v - pNpc->GetPos();
				vOut.y = 0.0f;
				if( fabs( (double)D3DXVec3LengthSq( &vOut ) ) < MAX_NPC_RADIUS )
					return TRUE;
			}
		}
	}
	return FALSE;
}