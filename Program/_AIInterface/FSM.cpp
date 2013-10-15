#include "stdafx.h"
#include "FSM.h"
#include "FSMstate.h"


static CFSM g_fsm[ FSM_TYPE_MAX ];

CFSM* GetFSM( DWORD type )
{
	return &g_fsm[ type ];
}

//////////////////////////////////////////////////////////////////////
// CFSM
//////////////////////////////////////////////////////////////////////

CFSM::CFSM()
{
}

CFSM::~CFSM()
{
	map< int, CFSMstate* >::iterator it;
	for( it = m_map.begin(); it != m_map.end(); ++it )
	{
		CFSMstate *pState = (CFSMstate *)((*it).second);
		if( pState != NULL )
			safe_delete( pState );
	}
}

//////////////////////////////////////////////////////////////////////
// member
//////////////////////////////////////////////////////////////////////

BOOL CFSM::GetTransition( int nStateID, int nInput, int& nOutput )
{
	CFSMstate *pState = GetState( nStateID );
	if( pState == NULL )
		return FALSE;

	return pState->GetOutput( nInput, nOutput );
}

CFSMstate *CFSM::GetState( int nStateID )
{
	map< int, CFSMstate* >::iterator it = m_map.find( nStateID );
	if( it != m_map.end() )
		return it->second;
	return NULL;
}

BOOL CFSM::AddState( CFSMstate *pNewState )
{
	if( m_map.insert( make_pair(pNewState->GetID(), pNewState) ).second )
		return TRUE;
	else
		return FALSE;		// leak: pNewState
}



