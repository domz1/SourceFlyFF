#include "stdafx.h"
#include "ticket.h"

CTicketProperty::CTicketProperty()
{
}

CTicketProperty::~CTicketProperty()
{
	m_mapTicketProp.clear();
#ifdef __PAID_SET
	m_mapTicketLink.clear();
#endif
}

BOOL	CTicketProperty::IsTarget( DWORD dwWorldId )
{
	for( map<DWORD, TicketProp>::iterator i = m_mapTicketProp.begin(); i != m_mapTicketProp.end(); ++i )
	{
		if( i->second.dwWorldId == dwWorldId )
			return TRUE;
	}
	return FALSE;
}

TicketProp*	CTicketProperty::GetTicketProp( DWORD dwItemId )
{
#ifdef __PAID_SET
	map<DWORD,DWORD>::iterator it = m_mapTicketLink.find( dwItemId );
	if( it == m_mapTicketLink.end() )
		return NULL;
	map<DWORD,TicketProp>::iterator i = m_mapTicketProp.find( it->second );
	if( i != m_mapTicketProp.end() )
		return &i->second;
	return NULL;
#else
	map<DWORD, TicketProp>::iterator i	= m_mapTicketProp.find( dwItemId );
	if( i != m_mapTicketProp.end() )
		return &i->second;
	return NULL;
#endif
}

CTicketProperty*	CTicketProperty::GetInstance( void )
{
	static	CTicketProperty sTicketProperty;
	return &sTicketProperty;
}

BOOL	CTicketProperty::LoadScript()
{
	CScript s;
#ifdef __PAID_SET
	if( s.Load( "PaidWorldSet.txt" ) == FALSE )
		return FALSE;

	DWORD dwIndex = s.GetNumber();
	while( s.tok != FINISHED )
	{
		TicketProp tp;
		tp.dwWorldId = s.GetNumber();
		tp.vPos.x	= s.GetFloat();
		tp.vPos.y	= s.GetFloat();
		tp.vPos.z	= s.GetFloat();
		s.GetToken();
		_tcscpy( tp.szWorldText, s.token );
		bool b	= m_mapTicketProp.insert( map<DWORD, TicketProp>::value_type( dwIndex, tp ) ).second;
		ASSERT( b == true );
		dwIndex = s.GetNumber();
	}

	if( s.Load("PaidWorldTicket.txt") == FALSE )
		return FALSE;
	dwIndex = s.GetNumber();
	DWORD dwWorldId = 0;
	while( s.tok != FINISHED )
	{
		dwWorldId = s.GetNumber();
		bool b	= m_mapTicketLink.insert( map<DWORD, DWORD>::value_type( dwIndex, dwWorldId ) ).second;
		ASSERT( b == true );
		dwIndex = s.GetNumber();
	}
#else
	if( s.Load( "ticket.inc" ) == FALSE )
		return FALSE;

	DWORD dwItemId	= s.GetNumber();
	while( s.tok != FINISHED )
	{
		TicketProp	tp;
		tp.dwWorldId	= s.GetNumber();
		tp.vPos.x	= s.GetFloat();
		tp.vPos.y	= s.GetFloat();
		tp.vPos.z	= s.GetFloat();
		bool b	= m_mapTicketProp.insert( map<DWORD, TicketProp>::value_type( dwItemId, tp ) ).second;
		ASSERT( b );
		dwItemId	= s.GetNumber();
	}
#endif

#ifdef __AZRIA_1023
	return m_lp.LoadScript();
#endif	// __AZRIA_1023
}

#ifdef __AZRIA_1023
CLayerProperty::CLayerProperty()
{
}

CLayerProperty::~CLayerProperty()
{
}

BOOL CLayerProperty::LoadScript()
{
	CScript s;
	if( s.Load( "layer.inc" ) == FALSE )
		return FALSE;
	DWORD dwWorldId	= s.GetNumber();
	while( s.tok != FINISHED )
	{
		LayerStruct ls;
		ls.dwWorldId	= dwWorldId;
		ls.nExpand	= s.GetNumber();
		m_vLayers.push_back( ls );
		dwWorldId	= s.GetNumber();
	}
	return TRUE;
}

int CLayerProperty::GetExpanedLayer( DWORD dwWorldId )
{
	for( VLS::iterator i = m_vLayers.begin(); i != m_vLayers.end(); ++i )
		if( ( *i ).dwWorldId == dwWorldId )
			return ( *i ).nExpand;
	return 0;
}
#endif	// __AZRIA_1023