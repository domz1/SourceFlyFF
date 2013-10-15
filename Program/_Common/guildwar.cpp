#include "stdafx.h"

#include "guildwar.h"
#include "guild.h"
extern	CGuildMng	g_GuildMng;

#ifdef __CORESERVER
#include "dpcoresrvr.h"
#include "dpcachesrvr.h"
#include "dpdatabaseclient.h"
#include "player.h"
extern	CPlayerMng	g_PlayerMng;
extern	CDPCoreSrvr	g_dpCoreSrvr;
extern	CDPCacheSrvr	g_DPCacheSrvr;
extern	CDPDatabaseClient	g_dpDatabaseClient;
#endif	// __CORESERVER
#ifdef __WORLDSERVER
#include "user.h"
#include "dpcoreclient.h"
extern	CUserMng	g_UserMng;
extern	CDPCoreClient	g_DPCoreClient;
#endif	// __WORLDSERVER

#ifdef __CLIENT
#include "definetext.h"
#endif	// __CLIENT

CGuildWar::CGuildWar()
{
	m_idWar		= 0;
	ZeroMemory( &m_Decl, sizeof(m_Decl) );
	ZeroMemory( &m_Acpt, sizeof(m_Acpt) );
	m_nFlag		= 0;
}

CGuildWar::~CGuildWar()
{

}

void CGuildWar::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_idWar;
		ar.Write( &m_Decl, sizeof(m_Decl) );
		ar.Write( &m_Acpt, sizeof(m_Acpt) );
		ar << m_nFlag;
		ar << (time_t)m_time.GetTime();
	}
	else
	{
		ar >> m_idWar;
		ar.Read( &m_Decl, sizeof(m_Decl) );
		ar.Read( &m_Acpt, sizeof(m_Acpt) );
		ar >> m_nFlag;
		time_t time;
		ar >> time;
		m_time	= CTime( time );
	}
}

#ifdef __WORLDSERVER
void CGuildWar::Process( CTime & time )
{
	if( m_nFlag == WF_WARTIME )
	{
		if( GetEndTime() < time )
		{
			m_nFlag	= WF_END;
			g_DPCoreClient.SendWarTimeout( m_idWar );
			TRACE( "SendWarTimeout, %d\n", m_idWar );
		}
		else
		{
			CGuild* pDecl	= g_GuildMng.GetGuild( m_Decl.idGuild );
			CGuild* pAcpt	= g_GuildMng.GetGuild( m_Acpt.idGuild );
			if( pDecl && pAcpt )
			{
				CUser* pMaster	= g_UserMng.GetUserByPlayerID( pDecl->m_idMaster );
				if( IsInvalidObj( pMaster ) )
					g_DPCoreClient.SendWarMasterAbsent( m_idWar, TRUE );
				pMaster	= g_UserMng.GetUserByPlayerID( pAcpt->m_idMaster );
				if( IsInvalidObj( pMaster ) )
					g_DPCoreClient.SendWarMasterAbsent( m_idWar, FALSE );
			}
		}
	}
}
#endif	// __WORLDSERVER

CGuildWarMng::CGuildWarMng()
{
	m_id	= 0;
}

CGuildWarMng::~CGuildWarMng()
{
	Clear();
}

void CGuildWarMng::Clear( void )
{
	for( map<u_long, CGuildWar*>::iterator i = m_mapPWar.begin(); i != m_mapPWar.end(); ++i )
		safe_delete( i->second );
	m_mapPWar.clear();
}

u_long	CGuildWarMng::AddWar( CGuildWar* pWar )
{
	m_id	= ( pWar->m_idWar != 0? pWar->m_idWar: m_id + 1 );
	if( GetWar( m_id ) )
		return 0;
	pWar->m_idWar	= m_id;
	m_mapPWar.insert( map<u_long, CGuildWar*>::value_type( m_id, pWar ) );
	return m_id;
}

BOOL CGuildWarMng::RemoveWar( u_long idWar )
{
	CGuildWar* pWar	= GetWar( idWar );
	if( pWar )
	{
		m_mapPWar.erase( pWar->m_idWar );
		safe_delete( pWar );
		return TRUE;
	}
	return FALSE;
}

CGuildWar* CGuildWarMng::GetWar( u_long idWar )
{
	map<u_long, CGuildWar*>::iterator i		= m_mapPWar.find( idWar );
	if( i != m_mapPWar.end() )
		return i->second;
	return NULL;
}

void CGuildWarMng::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_id;
		ar << GetSize();
		for( map<u_long, CGuildWar*>::iterator i = m_mapPWar.begin(); i != m_mapPWar.end(); ++i )
			( i->second )->Serialize( ar );
	}
	else
	{
#ifdef __CLIENT
		Clear();
#endif	// __CLIENT
		ar >> m_id;
		int nSize;
		ar >> nSize;
		for( int i = 0; i < nSize; i++ )
		{
			CGuildWar* pWar	= new CGuildWar;
			pWar->Serialize( ar );
			m_mapPWar.insert( map<u_long, CGuildWar*>::value_type( pWar->m_idWar, pWar ) );
		}
	}
}

void CGuildWarMng::Result( CGuildWar* pWar, CGuild* pDecl, CGuild* pAcpt, int nType, int nWptDecl, int nWptAcpt )
{
	CGuild* pWin, *pLose;
	switch( nType )
	{
		case WR_DECL_GN:
		case WR_DECL_SR:
		case WR_DECL_AB:
		case WR_DECL_DD:
		case WR_TRUCE:
		case WR_DRAW:
			{
				pWin	= pDecl;
				pLose	= pAcpt;
				break;
			}
		case WR_ACPT_GN:
		case WR_ACPT_SR:
		case WR_ACPT_AB:
		case WR_ACPT_DD:
			{
				pWin	= pAcpt;
				pLose	= pDecl;
				break;
			}
	}

	int nWinPointDecl = 0;
	int nWinPoint = 0;
	
	if( nType < WR_TRUCE )
	{
#ifdef __CORESERVER
		// nWinPoint
		nWinPoint	= (int)( pWin->m_nWinPoint + ( (float)( 10 + pWin->m_nWinPoint ) / (float)( ( (float)( pWin->m_nWinPoint + 4 ) / (float)( pLose->m_nWinPoint + 1 ) ) * 0.05 ) ) + 1 );
		int nMaxWinPoint	= (int)( ( 10 + pWin->m_nWinPoint * 0.05 ) * 5 );
		if( nWinPoint > nMaxWinPoint )
		{
			pWin->m_nWinPoint	+= nMaxWinPoint;
			nWinPointDecl = nMaxWinPoint;
		}
		else
		{
			pWin->m_nWinPoint	+= nWinPoint;
			nWinPointDecl = nWinPoint;
		}
		pWin->m_nWin++;
		nWinPoint	= (int)( ( 10 + pLose->m_nWinPoint * 0.02 ) );
		pLose->m_nWinPoint	-= nWinPoint;
		if( pLose->m_nWinPoint < 0 )
			pLose->m_nWinPoint	= 0;
		pLose->m_nLose++;
#else	// __CORESERVER
		pDecl->m_nWinPoint	= nWptDecl;
		pAcpt->m_nWinPoint	= nWptAcpt;
		pWin->m_nWin++;
		pLose->m_nLose++;
#endif	// __CORESERVER
	}

//	Clean up
	pWin->m_idWar	= 0;
	pWin->m_idEnemyGuild	= 0;
	pLose->m_idWar	= 0;
	pLose->m_idEnemyGuild	= 0;

	for( map<u_long, CGuildMember*>::iterator i = pWin->m_mapPMember.begin();
		i != pWin->m_mapPMember.end(); ++i )
	{
#ifdef __CORESERVER
		CPlayer* pPlayer	= g_PlayerMng.GetPlayer( i->second->m_idPlayer );
		if( pPlayer )	pPlayer->m_idWar	= 0;
#endif	// __CORESERVER
#ifdef __WORLDSERVER
		CUser* pUser	= g_UserMng.GetUserByPlayerID( i->second->m_idPlayer );
		if( IsValidObj( pUser ) )
		{
			pUser->m_idWar	= 0;
			g_UserMng.AddSetWar( pUser, 0 );
			pUser->SetPKTargetLimit( 10 );
		}
#endif	// __WORLDSERVER
	}
	for( i	= pLose->m_mapPMember.begin(); i != pLose->m_mapPMember.end(); ++i )
	{
#ifdef __CORESERVER
		CPlayer* pPlayer	= g_PlayerMng.GetPlayer( i->second->m_idPlayer );
		if( pPlayer )	pPlayer->m_idWar	= 0;
#endif	// __CORESERVER
#ifdef __WORLDSERVER
		CUser* pUser	= g_UserMng.GetUserByPlayerID( i->second->m_idPlayer );
		if( IsValidObj( pUser ) )
		{
			pUser->m_idWar	= 0;
			g_UserMng.AddSetWar( pUser, 0 );
			pUser->SetPKTargetLimit( 10 );
		}
#endif	// __WORLDSERVER
	}
#ifdef __CORESERVER
	g_dpCoreSrvr.SendWarEnd( pWar->m_idWar, pDecl->m_nWinPoint, pAcpt->m_nWinPoint, nType );
	g_DPCacheSrvr.SendWarEnd( pWar->m_idWar, pDecl->m_nWinPoint, pAcpt->m_nWinPoint, nType );
	int nDBType	= nType;
	switch( nType )
	{
		case WR_DECL_AB:
		case WR_DECL_DD:
			nDBType	= WR_DECL_GN;
			break;
		case WR_ACPT_AB:
		case WR_ACPT_DD:
			nDBType	= WR_ACPT_GN;
			break;
	}
	g_dpDatabaseClient.SendWarEnd( pWar->m_idWar, pDecl->m_idGuild, pAcpt->m_idGuild, pDecl->m_nWinPoint, pAcpt->m_nWinPoint, nDBType, pWar->m_time, pDecl->m_nWinPoint, pAcpt->m_nWinPoint, nWinPointDecl, nWinPoint );
#endif	// __CORESERVER
#ifdef __CLIENT
	char lpString[1024]	= { 0, };
	switch ( nType )
	{
		case WR_DECL_GN:
		case WR_ACPT_GN:
			{
				sprintf( lpString, prj.GetText( TID_GAME_GUILDWARWIN ), pDecl->m_szGuild, pAcpt->m_szGuild, pWin->m_szGuild, prj.GetText( TID_GAME_GUILDWARWIN_MASTER ) );
				g_WndMng.PutString( lpString , NULL, prj.GetTextColor( TID_GAME_GUILDWARWIN ) );
				break;
			}
		case WR_DECL_SR:
		case WR_ACPT_SR:
			{
				sprintf( lpString, prj.GetText( TID_GAME_GUILDWARWIN ), pDecl->m_szGuild, pAcpt->m_szGuild, pWin->m_szGuild, prj.GetText( TID_GAME_GUILDWARWIN_GIVEUP ) );
				g_WndMng.PutString( lpString , NULL, prj.GetTextColor( TID_GAME_GUILDWARWIN ) );
				break;
			}
		case WR_DECL_AB:
		case WR_ACPT_AB:
			{
				sprintf( lpString, prj.GetText( TID_GAME_GUILDWARWIN ), pDecl->m_szGuild, pAcpt->m_szGuild, pWin->m_szGuild, prj.GetText( TID_GAME_GUILDWARWIN_TIMEUP ) );
				g_WndMng.PutString( lpString , NULL, prj.GetTextColor( TID_GAME_GUILDWARWIN ) );
				break;
			}
		case WR_ACPT_DD:
		case WR_DECL_DD:
			{
				sprintf( lpString, prj.GetText( TID_GAME_GUILDWARWIN ), pDecl->m_szGuild, pAcpt->m_szGuild, pWin->m_szGuild, prj.GetText( TID_GAME_GUILDWARWIN_KILLNUM ) );
				g_WndMng.PutString( lpString , NULL, prj.GetTextColor( TID_GAME_GUILDWARWIN ) );
				break;
			}
		case WR_TRUCE:
			{
				char lpString[1024]	= { 0, };
				sprintf( lpString, prj.GetText( TID_GAME_GUILDWARPEACE ), pDecl->m_szGuild, pAcpt->m_szGuild );
				g_WndMng.PutString( lpString , NULL, prj.GetTextColor( TID_GAME_GUILDWARPEACE ) );
				break;
			}
		case WR_DRAW:
			{
				char lpString[1024]	= { 0, };
				sprintf( lpString, prj.GetText( TID_GAME_GUILDWARDRAW ), pDecl->m_szGuild, pAcpt->m_szGuild );
				g_WndMng.PutString( lpString , NULL, prj.GetTextColor( TID_GAME_GUILDWARDRAW ) );
				break;
			}
	}
#endif	// __CLIENT
	RemoveWar( pWar->m_idWar );
}

#ifdef __WORLDSERVER
void CGuildWarMng::Process( void )
{
	CTime time	= CTime::GetCurrentTime();
	for( map<u_long, CGuildWar*>::iterator i = m_mapPWar.begin(); i != m_mapPWar.end(); ++i )
	{
		CGuildWar* pWar	= i->second;
		pWar->Process( time );
	}
}
#endif	// __WORLDSERVER

CGuildWarMng	g_GuildWarMng;
