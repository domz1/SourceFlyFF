#include "stdafx.h"
#include "user.h"
#include "msghdr.h"
#ifdef _DEBUG
#include "mytrace.h"
extern	CMyTrace	g_MyTrace;
#endif	// _DEBUG

CUser::CUser( DPID dpid )
:
m_dpid( dpid ),
m_bValid( TRUE )
{
	m_dwTick	= GetTickCount();
	*m_pszAccount	= '\0';
}

CUserMng::CUserMng()
{
	
}

CUserMng::~CUserMng()
{
	CMclAutoLock	Lock( m_AddRemoveLock );

	for( map<DPID, CUser*>::iterator i	= begin(); i != end(); ++i )
		safe_delete( i->second );
	clear();
}

BOOL CUserMng::AddUser( DPID dpid )
{
	CUser* pUser	= new CUser( dpid );

	CMclAutoLock	Lock( m_AddRemoveLock );

	bool bResult	= insert( map<DPID, CUser*>::value_type( dpid, pUser ) ).second;
	if( bResult == false )
	{
		WriteError( "ADD//0" );
		SAFE_DELETE( pUser );
		return FALSE;
	}
#ifdef _DEBUG
	g_MyTrace.Add( CMyTrace::Key( "count" ), FALSE, "// %04d", size() );
#endif	// _DEBUG
	return TRUE;
}

BOOL CUserMng::RemoveUser( DPID dpid )
{
	CMclAutoLock	Lock( m_AddRemoveLock );
	map<DPID, CUser*>::iterator i	= find( dpid );
	if( i != end() )
	{
		CUser* pRemoved		= i->second;
		SAFE_DELETE( pRemoved );
		erase( dpid );
#ifdef _DEBUG
	g_MyTrace.Add( CMyTrace::Key( "count" ), FALSE, "// %04d", size() );
#endif	// _DEBUG
		return TRUE;
	}
	WriteError( "REMOVE//0" );
	return FALSE;
}

CUser* CUserMng::GetUser( DPID dpid )
{
	map<DPID, CUser*>::iterator i	= find( dpid );
	if( i != end() )
		return i->second;
	return NULL;
}

void CUserMng::ClearDum( CDPMng* pdp )
{
	BEFORESEND( ar, PACKETTYPE_KEEP_ALIVE );

	CUser* pUsertmp;
	DWORD dwTick	= GetTickCount() - SEC( 10 );

	CMclAutoLock	Lock( m_AddRemoveLock );

	for( map<DPID, CUser*>::iterator i = begin(); i != end(); ++i )
	{
		pUsertmp	= i->second;
		if( pUsertmp->m_dwTick < dwTick )
		{
			pdp->DestroyPlayer( pUsertmp->m_dpid );
		}
		else if( pUsertmp->m_bValid == FALSE )
		{
			pUsertmp->m_bValid	= TRUE;
			pdp->DestroyPlayer( pUsertmp->m_dpid );
		}
		else
		{
			pUsertmp->m_bValid	= FALSE;
			SEND( ar, pdp, pUsertmp->m_dpid );
		}
	}
}

CUserMng* CUserMng::GetInstance( void )
{
	static	CUserMng	sUserMng;
	return	&sUserMng;
}