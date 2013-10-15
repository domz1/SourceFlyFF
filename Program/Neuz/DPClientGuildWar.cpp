#include "stdafx.h"
#include "defineText.h"
#include "defineSound.h"
#include "defineItem.h"

#if __VER >= 19
#include "defineItemGrade.h"
#include "defineItemType.h"
#endif//__VER >= 19

#include "defineSkill.h"
#include "appdefine.h"
#include "dploginclient.h"
#include "dpclient.h"
#include "MsgHdr.h"
#include "dialogmsg.h"
#include "misc.h"
#include "defineObj.h"
#include "wndmessenger.h"
#include "WndQuest.h"
#include "mover.h"
#include "Ship.h"

#include "..\_UnhandledException\ExceptionHandler.h"

#include "..\_Network\ErrorCode.h"

extern	CDPLoginClient	g_dpLoginClient;

#include "guild.h"
#include "guildwar.h"
extern	CGuildMng	g_GuildMng;
extern	CGuildWarMng	g_GuildWarMng;

void CDPClient::SendDeclWar( u_long idMaster, const char* szGuild )
{
	BEFORESENDSOLE( ar, PACKETTYPE_DECL_GUILD_WAR, DPID_UNKNOWN );
	ar << idMaster;
	ar.WriteString( szGuild );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPClient::SendAcptWar( u_long idMaster, u_long idDecl )
{
	BEFORESENDSOLE( ar, PACKETTYPE_ACPT_GUILD_WAR, DPID_UNKNOWN );
	ar << idMaster << idDecl;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPClient::SendSurrender( u_long idPlayer )
{
	BEFORESENDSOLE( ar, PACKETTYPE_SURRENDER, DPID_UNKNOWN );
	ar << idPlayer;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPClient::OnSurrender( CAr & ar )
{
	TRACE( "OnSurrender\n" );

	u_long idWar, idPlayer;
	char sPlayer[MAX_PLAYER]	= { 0, };
	BOOL bDecl;
	
	ar >> idWar >> idPlayer;
	ar.ReadString( sPlayer, MAX_PLAYER );
	ar >> bDecl;

	CGuildWar* pWar	= g_GuildWarMng.GetWar( idWar );
	if( !pWar )
		return;

	CGuild* pGuild;
	if( bDecl )
	{
		pWar->m_Decl.nSurrender++;
		pGuild	= g_GuildMng.GetGuild( pWar->m_Decl.idGuild );
	}
	else
	{
		pWar->m_Acpt.nSurrender++;
		pGuild	= g_GuildMng.GetGuild( pWar->m_Acpt.idGuild );
	}

	if( !pGuild )
		return;

	if( pGuild->m_idGuild == g_pPlayer->m_idGuild )
	{
		CGuildMember* pMember	= pGuild->GetMember( idPlayer );
		if( pMember )
			pMember->m_nSurrender++;
	}

	char lpString[128]	= { 0, };
	sprintf( lpString, prj.GetText( TID_GAME_GUILDWARGIVEUP ), pGuild->m_szGuild, sPlayer );
	g_WndMng.PutString( lpString , NULL, prj.GetTextColor( TID_GAME_GUILDWARGIVEUP ) );
}

void CDPClient::SendQueryTruce( u_long idPlayer )
{
	BEFORESENDSOLE( ar, PACKETTYPE_QUERY_TRUCE, DPID_UNKNOWN );
	ar << idPlayer;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPClient::SendAcptTruce( u_long idPlayer )
{
	BEFORESENDSOLE( ar, PACKETTYPE_ACPT_TRUCE, DPID_UNKNOWN );
	ar << idPlayer;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPClient::OnQueryTruce( CAr & ar )
{
	TRACE( "OnQueryTruce\n" );

	g_WndMng.m_pWndGuildWarPeaceConfirm = new CWndGuildWarPeaceConfirm;
	g_WndMng.m_pWndGuildWarPeaceConfirm->Initialize();
	
}
	
void CDPClient::OnDeclWar( CAr & ar )
{
	TRACE( "OnDeclWar\n" );
	
	u_long idDecl;
	char szMaster[MAX_PLAYER];
	ar >> idDecl;
	ar.ReadString( szMaster, MAX_PLAYER );

	g_WndMng.m_pWndGuildWarRequest = new CWndGuildWarRequest;
	g_WndMng.m_pWndGuildWarRequest->Set( idDecl, szMaster );
	g_WndMng.m_pWndGuildWarRequest->Initialize();
}

void CDPClient::OnAcptWar( CAr & ar )
{
	TRACE( "OnAcpWar\n" );
	
	u_long idWar, idDecl, idAcpt;
	ar >> idWar >> idDecl >> idAcpt;
	
	CGuild* pDecl	= g_GuildMng.GetGuild( idDecl );
	CGuild* pAcpt	= g_GuildMng.GetGuild( idAcpt );
	
	if( !pDecl || !pAcpt )
		return;
	
	CGuildWar* pWar		= new CGuildWar;
	pWar->m_idWar	= idWar;
	pWar->m_Decl.idGuild	= pDecl->m_idGuild;
	pWar->m_Decl.nSize	= pDecl->GetSize();
	pWar->m_Acpt.idGuild	= pAcpt->m_idGuild;
	pWar->m_Acpt.nSize	= pAcpt->GetSize();
	pWar->m_nFlag	= WF_WARTIME;
	pWar->m_time	= CTime::GetCurrentTime();
	
	idWar	= g_GuildWarMng.AddWar( pWar );
	if( idWar > 0 )
	{
		pDecl->m_idWar	= idWar;
		pDecl->m_idEnemyGuild	= pAcpt->m_idGuild;
		pAcpt->m_idWar	= idWar;
		pAcpt->m_idEnemyGuild	= pDecl->m_idGuild;

		char lpString[128]	= { 0, };
		sprintf( lpString, prj.GetText( TID_GAME_GUILDWARSTART ), pDecl->m_szGuild, pAcpt->m_szGuild );
		g_WndMng.PutString( lpString , NULL, prj.GetTextColor( TID_GAME_GUILDWARSTART ) );
		if( g_pPlayer->m_idGuild == pDecl->m_idGuild || g_pPlayer->m_idGuild == pAcpt->m_idGuild )
		{
			CWndGuild* pWndGuild = (CWndGuild*)g_WndMng.GetWndBase( APP_GUILD );
			if( pWndGuild )
			{
				pWndGuild->m_WndGuildTabWar.UpdateData();
			}
		}
	}
	else
	{
		SAFE_DELETE( pWar );
	}
}

void CDPClient::OnSetWar( OBJID objid, CAr & ar )
{
	TRACE( "OnSetWar\n" );
	
	u_long idWar;
	ar >> idWar;
	
	CMover* pMover	= prj.GetMover( objid );
	if( IsValidObj( pMover ) ) 
	{
		pMover->m_idWar	= idWar;
	}
}

void CDPClient::OnWar( CAr & ar )
{
	TRACE( "OnWar\n" );

	u_long idWar;
	ar >> idWar;
	
	CGuildWar* pWar	= g_GuildWarMng.GetWar( idWar );
	if( !pWar )
	{
		pWar	= new CGuildWar;
		pWar->Serialize( ar );
		g_GuildWarMng.AddWar( pWar );
	}
	else
	{
		pWar->Serialize( ar );
	}

	char lpString[128]	= { 0, };
	CGuild* pDecl	= g_GuildMng.GetGuild( pWar->m_Decl.idGuild );
	CGuild* pAcpt	= g_GuildMng.GetGuild( pWar->m_Acpt.idGuild );
	
	if( pDecl && pAcpt )
	{
		sprintf( lpString, prj.GetText( TID_GAME_GUILDWARSTILL ), pDecl->m_szGuild, pAcpt->m_szGuild );
		g_WndMng.PutString( lpString , NULL, prj.GetTextColor( TID_GAME_GUILDWARSTILL ) );
		g_WndMng.PutString( prj.GetText( TID_GAME_GUILDWARDOING ) , NULL, prj.GetTextColor( TID_GAME_GUILDWARDOING ) );

		CWndGuild* pWndGuild = (CWndGuild*)g_WndMng.GetWndBase( APP_GUILD );
		if( pWndGuild )
			pWndGuild->m_WndGuildTabWar.UpdateData();
	}
}

void CDPClient::OnWarDead( CAr & ar )
{
	TRACE( "OnWarDead\n" );

	u_long idWar;
	char sPlayer[MAX_PLAYER]	= { 0, };
	BOOL bDecl;
	ar >> idWar;
	ar.ReadString( sPlayer, MAX_PLAYER );
	ar >> bDecl;

	CGuildWar* pWar	= g_GuildWarMng.GetWar( idWar );
	if( pWar )
	{
		if( bDecl )
		{
			pWar->m_Decl.nDead++;
			char lpString[128]	= { 0, };
			CGuild* pGuild	= g_GuildMng.GetGuild( pWar->m_Decl.idGuild );
			if( pGuild )
			{
				sprintf( lpString, prj.GetText( TID_GAME_GUILDWARDEAD ), pGuild->m_szGuild, sPlayer );
				g_WndMng.PutString( lpString , NULL, prj.GetTextColor( TID_GAME_GUILDWARDEAD ) );
			}
		}
		else
		{
			pWar->m_Acpt.nDead++;
			char lpString[128]	= { 0, };
			CGuild* pGuild	= g_GuildMng.GetGuild( pWar->m_Acpt.idGuild );
			if( pGuild )
			{
				sprintf( lpString, prj.GetText( TID_GAME_GUILDWARDEAD ), pGuild->m_szGuild, sPlayer );
				g_WndMng.PutString( lpString , NULL, prj.GetTextColor( TID_GAME_GUILDWARDEAD ) );
			}
		}
	}
}

void CDPClient::OnWarEnd( CAr & ar )
{
	TRACE( "OnWarEnd\n" );

	u_long idWar;
	int nWptDecl, nWptAcpt;
	int nType;
	ar >> idWar >> nWptDecl >> nWptAcpt >> nType;

	CGuildWar* pWar	= g_GuildWarMng.GetWar( idWar );
	if( !pWar )
		return;
	CGuild* pDecl	= g_GuildMng.GetGuild( pWar->m_Decl.idGuild );
	CGuild* pAcpt	= g_GuildMng.GetGuild( pWar->m_Acpt.idGuild );
	if( !pDecl || !pAcpt )
		return;

	g_GuildWarMng.Result( pWar, pDecl, pAcpt, nType, nWptDecl, nWptAcpt );

	CGuild* pGuild	= g_pPlayer->GetGuild();
	if( pGuild == pDecl || pGuild == pAcpt )
	{
		CWndGuild* pWndGuild = (CWndGuild*)g_WndMng.GetWndBase( APP_GUILD );
		if( pWndGuild )
		{
			pWndGuild->m_WndGuildTabApp.UpdateData();
			pWndGuild->m_WndGuildTabInfo.UpdateData();
			pWndGuild->m_WndGuildTabMember.UpdateData();
			pWndGuild->m_WndGuildTabWar.UpdateData();
		}
	}
}
