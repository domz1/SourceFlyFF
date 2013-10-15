#include "stdafx.h"
#include "flyffevent.h"

CFlyffEvent::CFlyffEvent()
{
	memset( (void*)m_aEvent, 0, sizeof(BYTE) * MAX_EVENT );
}

BOOL CFlyffEvent::SetState( int id, BYTE nState )
{
	if( id < 0 || id >= MAX_EVENT )
		return FALSE;
	m_aEvent[id]	= nState;
	return TRUE;
}

BYTE CFlyffEvent::GetState( int id )
{
	if( id < 0 || id >= MAX_EVENT )
		return (BYTE)0;
	return m_aEvent[id];
}

void CFlyffEvent::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar.Write( (const void*)m_aEvent, sizeof(BYTE) * MAX_EVENT );
	}
	else
	{
		ar.Read( (void*)m_aEvent, sizeof(BYTE) * MAX_EVENT );
	}
}
