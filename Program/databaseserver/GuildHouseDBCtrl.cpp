// GuildHouseDBCtrl.cpp: implementation of the CGuildHouseDBCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GuildHouseDBCtrl.h"

#if __VER >= 15 // __GUILD_HOUSE
#include "MsgHdr.h"
#include "dptrans.h"
extern AppInfo g_appInfo;

//////////////////////////////////////////////////////////////////////
// CGuildHouseDBCtrl::CDbController
//////////////////////////////////////////////////////////////////////
CGuildHouseDBCtrl::CGuildHouseDBCtrl()
{

}

CGuildHouseDBCtrl::~CGuildHouseDBCtrl()
{

}

void CGuildHouseDBCtrl::Handler( LPDB_OVERLAPPED_PLUS pov, DWORD dwCompletionKey )
{
	CAr ar( pov->lpBuf, pov->uBufSize );

	switch( pov->nQueryMode )
	{
		case GUILDHOUSE_LOAD:
			if( !LoadGuildHouse() )
				AfxMessageBox( "GuildHouse Load Failed!!!" );
			break;

		case GUILDHOUSE_SEND:
			SendGuildHouse( dwCompletionKey );
#ifdef __GUILD_HOUSE_MIDDLE
			SendGuildHouseTenderInfo( dwCompletionKey );
#endif // GUILDHOUSE_SEND
			break;

		case GUILDHOUSE_CREATE:
			CreateGuildHouse( ar, dwCompletionKey );
			break;

		case GUILDHOUSE_REMOVE:
			RemoveGuildHouse( dwCompletionKey );
			break;

		case GUILDHOUSE_DEFAULT_PACKET:
			RunDefaultPacket( ar, dwCompletionKey );
			break;	

		case GUILDHOUSE_SAVE_RESTPOINT:
			DWORD dwPlayerId; int nRestPoint; time_t tLogOut;
			ar >> dwPlayerId >> nRestPoint >> tLogOut;
			GetQueryObject()->Execute( "usp_RestPoint_Update '%02d', '%07d', %d, %d", g_appInfo.dwSys, dwPlayerId, nRestPoint, tLogOut );
			break;

#ifdef __GUILD_HOUSE_MIDDLE
		case GUILDHOUSE_TENDER_JOIN:
			{
				JoinGuildHouseTender( ar, dwCompletionKey );
			}
			break;

		case GUILDHOUSE_GRADE_UPDATE:
			{
				UpgradeGuildHouseGrade( ar, dwCompletionKey );
			}
			break;

		case GUILDHOUSE_LEVEL_UPDATE:
			{
				UpgradeGuildHouseLevel( ar, dwCompletionKey );
			}
			break;
		case GUILDHOUSE_EXPIRED:
			{
				UpdateGuildHouseExpired( ar );
			}
			break;
#endif // __GUILD_HOUSE_MIDDLE
	}
}

void CGuildHouseDBCtrl::OnTimer()
{ 
	GuildHouseMng->ProcessExpired();

#ifdef __GUILD_HOUSE_MIDDLE
//	GuildHouseMng->ProcessTender( WI_GUILDHOUSE_MIDDLE );
#endif // __GUILD_HOUSE_MIDDLE
}

void CGuildHouseDBCtrl::SendGuildHouse( DPID dpId )
{
	if( !GuildHouseMng->IsEmpty() )
	{
		BEFORESENDDUAL( ar, PACKETTYPE_GUILDHOUSE_LOAD, DPID_UNKNOWN, DPID_UNKNOWN );
		GuildHouseMng->Serialize( ar );
		SEND( ar, CDPTrans::GetInstance(), dpId );
	}
}

void CGuildHouseDBCtrl::CreateGuildHouse( CAr & ar, DPID dpId )
{
	DWORD dwPlayerId, dwGuildId;
	ar >> dwPlayerId >> dwGuildId;
	
	BOOL bResult = FALSE;
	CQuery* pQuery = GetQueryObject();

	BEFORESENDDUAL( arCreate, PACKETTYPE_GUILDHOUSE_BUY, DPID_UNKNOWN, DPID_UNKNOWN );
	{
		arCreate << dwPlayerId << dwGuildId;
		CGuildHouseBase* pGuildHouse = GuildHouseMng->MakeGuildHouse( dwGuildId, WI_GUILDHOUSE_SMALL );
		if( pGuildHouse )
		{
			pGuildHouse->SetUpkeeptime( time_null() + ( UPKEEP_DAY_TIME * 7 ) );	// 기본 유지기간을 제공한다.
			if( pQuery->Execute( "usp_GuildHouse_Insert '%02d', '%06d', %d, %d", g_appInfo.dwSys, dwGuildId, WI_GUILDHOUSE_SMALL, pGuildHouse->GetUpkeepTime() ) // DB에 추가 성공
				&& GuildHouseMng->AddGuildHouse( dwGuildId, pGuildHouse ) )
			{
				arCreate << TRUE;
				dpId = DPID_ALLPLAYERS;	// 모든 월드 서버로 전송한다.
				GH_Fntr_Info gfi( II_GHOU_FUR_NPC_TELEPORTER, TRUE, pGuildHouse->GetTeleporterPos(), 0.0f, ( UPKEEP_DAY_TIME * 7 ) );
				if( ExcuteQuery( dwGuildId, pGuildHouse, GUILDHOUSE_PCKTTYPE_LISTUP, gfi, NULL_ID ) )
					pGuildHouse->OnGuildHousePacket( GUILDHOUSE_PCKTTYPE_LISTUP, gfi, NULL_ID );
				pGuildHouse->SerializeAllInfo( arCreate );
			}
			else
			{
				arCreate << FALSE;
				SAFE_DELETE( pGuildHouse );
			}
		}
		else
			arCreate << FALSE;
	}
	SEND( arCreate, CDPTrans::GetInstance(), dpId );
}

void CGuildHouseDBCtrl::RemoveGuildHouse( DWORD dwGuildId )
{
	if( GetQueryObject()->Execute( "usp_GuildHouse_Delete '%02d', '%06d'", g_appInfo.dwSys, dwGuildId ) )
		GuildHouseMng->RemoveGuildHouse( dwGuildId );
	
	BEFORESENDDUAL( arOut, PACKETTYPE_GUILDHOUSE_REMOVE, DPID_UNKNOWN, DPID_UNKNOWN );
	arOut << dwGuildId;
	SEND( arOut, CDPTrans::GetInstance(), DPID_ALLPLAYERS );

}

void CGuildHouseDBCtrl::RunDefaultPacket( CAr & ar, DPID dpId )
{
	DWORD dwGuildId;
	int nPacketType, nIndex;
	GH_Fntr_Info gfi;

	ar >> dwGuildId >> nPacketType >> nIndex;
	gfi.Serialize( ar );

	CGuildHouseBase* pGuildHouse = GuildHouseMng->GetGuildHouse( dwGuildId );
	if( pGuildHouse )
	{
		BOOL bResult = FALSE;
		if( ExcuteQuery( dwGuildId, pGuildHouse, nPacketType, gfi, nIndex ) )
			 bResult = pGuildHouse->OnGuildHousePacket( nPacketType, gfi, nIndex );

		if( bResult ) dpId = DPID_ALLPLAYERS;
		BEFORESENDDUAL( arOut, PACKETTYPE_GUILDHOUSE_PACKET, DPID_UNKNOWN, DPID_UNKNOWN );
		arOut << bResult << dwGuildId;
		arOut << nPacketType << nIndex;
		gfi.Serialize( arOut );
		SEND( arOut, CDPTrans::GetInstance(), dpId );
	}
}

BOOL CGuildHouseDBCtrl::ExcuteQuery( DWORD dwGuildId, CGuildHouseBase* pGuildHouse, int nPacketType, GH_Fntr_Info & gfi, int nIndex )
{
	switch( nPacketType )
	{
		case GUILDHOUSE_PCKTTYPE_LISTUP:
			if( GetQueryObject()->Execute( "usp_GuildFurniture_Insert '%02d', '%06d', %d, %d, %d, %d,\
											%f, %f, %f, %f", 
											g_appInfo.dwSys, dwGuildId,	GuildHouseMng->GetSeqNum() + 1, gfi.dwItemId, gfi.tKeepTime, static_cast<int>( gfi.bSetup ), 
											gfi.vPos.x, gfi.vPos.y, gfi.vPos.z, gfi.fAngle ) )
			{		
				gfi.nSeqNum = GuildHouseMng->AddSeqNum();
				return TRUE;
			}
			else
				return FALSE;

		case GUILDHOUSE_PCKTTYPE_LISTDROP:
			if( nIndex >= pGuildHouse->GetFurnitureListSize() )
				return FALSE;
			gfi.nSeqNum = pGuildHouse->GetFurnitureInfo( nIndex ).nSeqNum;
			return GetQueryObject()->Execute( "usp_GuildFurniture_Delete '%02d', '%06d', %d", g_appInfo.dwSys, dwGuildId, gfi.nSeqNum );
		
		case GUILDHOUSE_PCKTTYPE_SETUP:
		case GUILDHOUSE_PCKTTYPE_REMOVE:
		case GUILDHOUSE_PCKTTYPE_RESET:
			if( nIndex >= pGuildHouse->GetFurnitureListSize() )
				return FALSE;
			gfi.nSeqNum = pGuildHouse->GetFurnitureInfo( nIndex ).nSeqNum;
			return GetQueryObject()->Execute(  "usp_GuildFurniture_Update '%02d', '%06d', %d, %d, %d, \
												%f, %f, %f, %f, %d", 
												g_appInfo.dwSys, dwGuildId, gfi.nSeqNum, gfi.dwItemId, static_cast<int>( gfi.bSetup ), 
												gfi.vPos.x, gfi.vPos.y, gfi.vPos.z, gfi.fAngle, gfi.tKeepTime );	

		case GUILDHOUSE_PCKTTYPE_UPKEEP:
			return GetQueryObject()->Execute( "usp_GuildHouse_Update '%02d', '%06d', %d", g_appInfo.dwSys, dwGuildId, gfi.tKeepTime );

		case GUILDHOUSE_PCKTTYPE_EXPIRED:
			return GetQueryObject()->Execute( "usp_GuildHouse_Expired '%02d', '%06d'", g_appInfo.dwSys, dwGuildId );
	}

	return FALSE;
}

#ifdef __GUILD_HOUSE_MIDDLE
void CGuildHouseDBCtrl::SendGuildHouseTenderInfo( DPID dpId )
{
	GuildHouseMng->SendGuildHouseTenderInfo( dpId );
}

void CGuildHouseDBCtrl::JoinGuildHouseTender( CAr & ar, DPID dpId )
{
	DWORD dwPlayerId;
	CGuildHouseMng::GUILDHOUSE_TENDER GHT;
	ar >> dwPlayerId;
	GHT.Serialize( ar );

	CGuildHouseMng::GUILDHOUSE_DATA* pGHData = GuildHouseMng->GetGuildHouseData( GHT.objGHId );
	if( pGHData == NULL )
	{
		Error( "SetGuildHouseTenderJoin - Invalid GuildHouse Data. GHId : %d, GuildId : %06d", GHT.objGHId, GHT.dwGuildId );
		return;
	}

	GHT.dwGHType = pGHData->dwGHType;
	GHT.nAreaIndex = GuildHouseMng->GetTenderArea( pGHData->dwGHType );
	GHT.tTenderTime = time_null();
	BEFORESENDDUAL( arTender, PACKETTYPE_GUILDHOUSE_TENDER_JOIN, DPID_UNKNOWN, DPID_UNKNOWN );
	{
		arTender << dwPlayerId << GHT.objGHId;
		if( InsertGuildHouseTender( GHT ) == TRUE && GuildHouseMng->AddGuildHouseTender( GHT.dwGuildId, GHT ) == TRUE )
		{
			arTender << TRUE;
			GHT.Serialize( arTender );
			dpId = DPID_ALLPLAYERS;
		}
		else
		{
			arTender << FALSE;
		}
	}
	SEND( arTender, CDPTrans::GetInstance(), dpId );
}

bool CGuildHouseDBCtrl::InsertGuildHouseTender( CGuildHouseMng::GUILDHOUSE_TENDER& GHT )
{
	if( GHT.dwGHType == WI_GUILDHOUSE_MIDDLE )
	{
		if( GetQueryObject()->Execute( "usp_GuildHouse_Tender_Insert %d, %d, '%02d', '%06d', %d, %d, %d, '%c'",
			GuildHouseMng->GetTenderId( GHT.dwGHType ), GHT.nAreaIndex, g_appInfo.dwSys, GHT.dwGuildId, GHT.objGHId,
			GHT.nTenderPerin, GHT.nTenderPenya, GHT.chState ) == TRUE )
		{
			return true;
		}
	}
	return true;
}

void CGuildHouseDBCtrl::UpgradeGuildHouseGrade( CAr & ar, DPID dpId )
{
	DWORD dwGHType;
	int nSize;
	ar >> dwGHType >> nSize;
	for( int i = 0; i < nSize; ++i )
	{
		DWORD dwGuildId = 0;
		OBJID objGHId = 0;
		ar >> dwGuildId >> objGHId;

		RemoveAllFurniture( dwGuildId );
		UpdateGuildHouseGrade( dwGuildId, dwGHType, objGHId );
	}
}

void CGuildHouseDBCtrl::RemoveAllFurniture( DWORD dwGuildId )
{
	CGuildHouseBase* pGuildHouse = GuildHouseMng->GetGuildHouse( dwGuildId );
	if( pGuildHouse != NULL )
	{
		for( int i = 0; i < pGuildHouse->GetFurnitureListSize(); ++i )
		{
			GH_Fntr_Info gfi = pGuildHouse->GetFurnitureInfo( i );
			if( gfi.bSetup == TRUE )
			{
				gfi.objId = NULL_ID;
				gfi.bSetup = FALSE;
				ExcuteQuery( dwGuildId, pGuildHouse, GUILDHOUSE_PCKTTYPE_REMOVE, gfi, i );
			}
		}
	}
}

void CGuildHouseDBCtrl::UpdateGuildHouseGrade( DWORD dwGuildId, DWORD dwWorldId, OBJID objGHId )
{
	CGuildHouseMng::GUILDHOUSE_DATA* pGHData = GuildHouseMng->GetGuildHouseData( objGHId );
	if( pGHData != NULL )
	{
		time_t tKeepTime = 0;
		CGuildHouseBase* pGuildHouse = GuildHouseMng->GetGuildHouse( dwGuildId );
		// keeptime
		if( pGuildHouse != NULL )
		{
			int nKeepDay = ( pGuildHouse->GetUpkeepTime() - time_null() ) / UPKEEP_DAY_TIME + 1;
			tKeepTime = time_null() + ( UPKEEP_DAY_TIME * nKeepDay ) + ( UPKEEP_DAY_TIME * 7 );
		}

		if( GetQueryObject()->Execute( "usp_GuildHouse_Grade_Update '%02d', '%06d', %d, %d, %d, %d, %d, '%s'",
			g_appInfo.dwSys, dwGuildId, pGHData->dwGHType, tKeepTime, objGHId, pGHData->nAreaIndex, 0, pGHData->szTitle ) == TRUE )
		{
			// remove guildhouse
			pGuildHouse = ReMakeGuildHouse( dwGuildId, dwWorldId );
			if( pGuildHouse != NULL )
			{
				pGuildHouse->SetGuildHouseData( objGHId, pGHData->nAreaIndex, 0, tKeepTime, pGHData->szTitle );
				// send world
				BEFORESENDDUAL( ar, PACKETTYPE_GUILDHOUSE_GRADE_UPDATE, DPID_UNKNOWN, DPID_UNKNOWN );
				ar << dwGuildId << dwWorldId;
				pGuildHouse->SerializeAllInfo( ar );
				SEND( ar, CDPTrans::GetInstance(), DPID_ALLPLAYERS );
			}
			else
			{
				Error( "CGuildHouseDBCtrl::UpdateGuildHouseGrade - ReMakeGuildHouse Failed. Guild : [%06d]", dwGuildId );
			}
		}
		else
		{
			Error( "CGuildHouseDBCtrl::UpdateGuildHouseGrade - Insert Failed. Guild : [%06d], GHType : [%d], KeepTime : [%d], GHId : [%d], AreaIndex : [%d]",
				dwGuildId, pGHData->dwGHType, tKeepTime, objGHId, pGHData->nAreaIndex );
		}
	}
}

void CGuildHouseDBCtrl::UpgradeGuildHouseLevel( CAr & ar, DPID dpId )
{
	DWORD dwPlayerId, dwGuildId;
	int nLevel;
	ar >> dwPlayerId >> dwGuildId >> nLevel;

	CGuildHouseBase* pGuildHouse = GuildHouseMng->GetGuildHouse( dwGuildId );
	if( pGuildHouse != NULL )
	{
		BEFORESENDDUAL( arUpdate, PACKETTYPE_GUILDHOUSE_LEVEL_UPDATE, DPID_UNKNOWN, DPID_UNKNOWN );
		{
			arUpdate << dwPlayerId;
			if( pGuildHouse->GetGuildHouseLevel() < nLevel )
			{
				if( GetQueryObject()->Execute( "usp_GuildHouse_Level_Update '%02d', '%06d', %d", g_appInfo.dwSys, dwGuildId, nLevel ) == TRUE )
				{
					pGuildHouse->SetGuildHouseLevel( nLevel );
					arUpdate << TRUE;
					arUpdate << dwGuildId << nLevel;
					dpId = DPID_ALLPLAYERS;
				}
				else
				{
					arUpdate << FALSE;
				}
			}
			else
			{
				Error( "SetGuildHouseLevel - GHLevel is Not Lower. User : %07d, Guild : %06d", dwPlayerId, dwGuildId );
				arUpdate << FALSE;
			}
		}
		SEND( arUpdate, CDPTrans::GetInstance(), dpId );
	}
}

void CGuildHouseDBCtrl::UpdateGuildHouseExpired( CAr & ar )
{
	DWORD dwGuildId, dwWorldId;
	ar >> dwGuildId >> dwWorldId;

	CGuildHouseBase* pGuildHouse = GuildHouseMng->GetGuildHouse( dwGuildId );
	if( pGuildHouse != NULL )
	{
		if( GetQueryObject()->Execute( "usp_GuildHouse_Expired '%02d', '%06d'", g_appInfo.dwSys, dwGuildId ) == TRUE )
		{
			pGuildHouse->SetUpkeeptime( 0 );
			// send world
			BEFORESENDDUAL( ar, PACKETTYPE_GUILDHOUSE_EXPIRED, DPID_UNKNOWN, DPID_UNKNOWN );
			ar << dwGuildId << dwWorldId;
			if( dwWorldId >= WI_GUILDHOUSE_MIDDLE && dwWorldId <= WI_GUILDHOUSE_LARGE )
			{
				pGuildHouse = ReMakeGuildHouse( dwGuildId, dwWorldId );
				if( pGuildHouse != NULL )
				{
					pGuildHouse->SetUpkeeptime( 0 );
					pGuildHouse->SerializeAllInfo( ar );
				}
			}
			SEND( ar, CDPTrans::GetInstance(), DPID_ALLPLAYERS );
		}
	}
	else
	{
		Error( "GetGuildHouse Failed. Guild : [%d]", dwGuildId );
	}
}

CGuildHouseBase* CGuildHouseDBCtrl::ReMakeGuildHouse( DWORD dwGuildId, DWORD dwWorldId )
{
	if( GuildHouseMng->RemoveGuildHouse( dwGuildId ) == TRUE )
	{
		CGuildHouseBase* pGuildHouse = GuildHouseMng->MakeGuildHouse( dwGuildId, dwWorldId );
		if( pGuildHouse != NULL && GuildHouseMng->AddGuildHouse( dwGuildId, pGuildHouse ) == TRUE )
		{
			CQuery* pQuery = GetQueryObject();
			if( pQuery->Execute( "usp_GuildFurniture_Load '%02d', '%06d'", g_appInfo.dwSys, dwGuildId ) == TRUE )
			{
				while( pQuery->Fetch() )
				{
					GH_Fntr_Info gfi;
					gfi.nSeqNum		= pQuery->GetInt( "SEQ" );
					gfi.dwItemId	= static_cast<DWORD>( pQuery->GetInt( "ItemIndex" ) );
					gfi.bSetup		= static_cast<BOOL>( pQuery->GetInt( "bSetup" ) );
					gfi.vPos.x		= static_cast<float>( pQuery->GetFloat( "x_Pos" ) );
					gfi.vPos.y		= static_cast<float>( pQuery->GetFloat( "y_Pos" ) );
					gfi.vPos.z		= static_cast<float>( pQuery->GetFloat( "z_Pos" ) );
					gfi.fAngle		= static_cast<float>( pQuery->GetFloat( "fAngle" ) );
					gfi.tKeepTime	= pQuery->GetInt( "tKeepTime" );
					pGuildHouse->OnGuildHousePacket( GUILDHOUSE_PCKTTYPE_LISTUP, gfi, NULL_ID );
				}
				return pGuildHouse;
			}
			else
			{
				Error( "CGuildHouseDBCtrl::ReMakeGuildHouse - usp_GuildFurniture_Load Failed. Guild : [%06d], World : [%d]", dwGuildId, dwWorldId );
			}
		}
		else
		{
			Error( "CGuildHouseDBCtrl::ReMakeGuildHouse - MakeGuildHouse Failed. Guild : [%06d], World : [%d]", dwGuildId, dwWorldId );
		}
	}
	return NULL;
}
#endif // __GUILD_HOUSE_MIDDLE

//////////////////////////////////////////////////////////////////////
// CGuildHouseDBMng
//////////////////////////////////////////////////////////////////////
CGuildHouseDBMng::CGuildHouseDBMng()
{
	if( !m_GHDBCtrl.CreateDbHandler( SEC( 5 ) ) )
		Error( "CGuildHouseDBMng - m_GHDBCtrl.CreateDbHandler() FAILED!" );
}

CGuildHouseDBMng::~CGuildHouseDBMng()
{
	m_GHDBCtrl.CloseDbHandler();
}

CGuildHouseDBMng* CGuildHouseDBMng::GetInstance()
{
	static CGuildHouseDBMng sGHDBMng;
	return & sGHDBMng;
}
#endif // __GUILD_HOUSE