// GuildHouse.cpp: implementation of the CGuildHouse class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#if __VER >= 15 // __GUILD_HOUSE
#include "GuildHouse.h"
#include "defineText.h"
#ifdef __CLIENT
#include "MsgHdr.h"
#include "DPClient.h"
#include "WndHousing.h"
extern CDPClient g_DPlay;
#endif // __CLIENT
#ifdef __WORLDSERVER
#include "WorldMng.h"
extern CWorldMng g_WorldMng;
#include "guild.h"
extern CGuildMng g_GuildMng;
#include "user.h"
extern CUserMng g_UserMng;
#include "DPSrvr.h"
extern CDPSrvr g_DPSrvr;
#include "DPDatabaseClient.h"
extern CDPDatabaseClient g_dpDBClient;
#include "npchecker.h"
#ifdef __GUILD_HOUSE_MIDDLE
#include "post.h"
#endif // __GUILD_HOUSE_MIDDLE
#endif // __WORLDSERVER
#ifdef __DBSERVER
#include "GuildHouseDBCtrl.h"
#ifdef __GUILD_HOUSE_MIDDLE
#include "dptrans.h"
#include "DPCoreSrvr.h"
extern	CDPCoreSrvr	g_dpCoreSrvr;
#endif // __GUILD_HOUSE_MIDDLE
#endif // __DBSERVER
//////////////////////////////////////////////////////////////////////
// CGuildHouseBase::GUILDHOUSE_FURNITURE_INFO
//////////////////////////////////////////////////////////////////////
CGuildHouseBase::GuildHouse_Furniture_Info::GuildHouse_Furniture_Info( DWORD dwII , BOOL bS, D3DXVECTOR3 v, float fA, time_t tK )
:dwItemId( dwII ), bSetup( bS ), vPos( v ), fAngle( fA ), objId( NULL_ID ), tKeepTime( time_null() + tK )
#ifdef __DBSERVER
, nSeqNum( 0 )
#endif // __DBSERVER
{
}

void CGuildHouseBase::GuildHouse_Furniture_Info::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << dwItemId << bSetup << vPos << fAngle << objId;
		if( tKeepTime > 0 )
			ar << tKeepTime - time_null();
		else
			ar << static_cast<time_t>( 0 );
	}
	else	
	{
		ar >> dwItemId >> bSetup >> vPos >> fAngle >> objId;
		ar >> tKeepTime;
		if( tKeepTime > 0 )
			tKeepTime += time_null();
	}
}


//////////////////////////////////////////////////////////////////////
// CGuildHouseBase
//////////////////////////////////////////////////////////////////////
CGuildHouseBase::CGuildHouseBase( DWORD dwGuildId, DWORD dwWorldId )
:m_dwGuildId( dwGuildId ), m_dwWorldId( dwWorldId ), m_tUpkeepTime( 0 )
#ifdef __GUILD_HOUSE_MIDDLE
, m_objGHId( 0 ), m_nAreaIndex( 0 ), m_nGHLevel( 0 )
#endif // __GUILD_HOUSE_MIDDLE
{
#ifdef __WORLDSERVER
	m_dwWaitDBAckPlayerId = NULL_ID;
#endif // __WORLDSERVER

#ifdef __CLIENT
	m_dwSelectedObjID = NULL_ID;
	m_nExtraExp = 0;
	m_nWndDeployingIndex  = -1;
#endif
#ifdef __GUILD_HOUSE_MIDDLE
	::memset( m_szGHName, 0, sizeof( m_szGHName ) );
#endif // __GUILD_HOUSE_MIDDLE
}

CGuildHouseBase::~CGuildHouseBase()
{
	Clear();
}

void CGuildHouseBase::Clear()
{
#ifdef __WORLDSERVER
	DestroyGuildHouseRoom();
	for( int i=0; i<(int)( m_vecFntInfo.size() ); i++ )
	{
		if( m_vecFntInfo[i].bSetup )
			ResetDSTFunriture( prj.GetItemProp( m_vecFntInfo[i].dwItemId ) );
	}
#endif // __WORLDSERVER
	m_vecFntInfo.clear();
}

#ifdef __CLIENT
CGuildHouseBase* CGuildHouseBase::GetInstance()
{
	static CGuildHouseBase sGHB( NULL_ID, NULL_ID );
	return & sGHB;
}

void CGuildHouseBase::SendClientToWorld( int nPacketType, GH_Fntr_Info& gfi, int nIndex )
{
	BEFORESENDSOLE( ar, PACKETTYPE_GUILDHOUSE_PACKET, DPID_UNKNOWN );
	ar << nPacketType << nIndex;
	gfi.Serialize( ar );
	SEND( ar, &g_DPlay, DPID_SERVERPLAYER );
}

struct IsSameID : public binary_function< GH_Fntr_Info, OBJID, bool > 
{
	bool operator( ) ( const GH_Fntr_Info& kInfo, OBJID id_ ) const 
	{
		return ( kInfo.objId == id_ );
	}
};

GH_Fntr_Info*  CGuildHouseBase::Find( OBJID objID_ )
{
	VECFurnitureIter finder = find_if( m_vecFntInfo.begin(), m_vecFntInfo.end(), bind2nd( IsSameID(), objID_ ) );
	if( finder != m_vecFntInfo.end() )
		return &( *finder );

	return NULL;
}

int CGuildHouseBase::FindIndex( OBJID objID_ )
{
	for( int i = 0; i < GetFurnitureListSize( ); ++i )
	{
		GH_Fntr_Info* pInfo = GetFurnitureInfoPtr( i );
		assert( pInfo );

		if( objID_ == pInfo->objId )
		{
			return i;
		}
	}
	
	return -1;
}

void CGuildHouseBase::Setup( const HOUSING_ITEM& kInfo )
{
	GH_Fntr_Info kSendInfo;
	kSendInfo.dwItemId = kInfo.m_nIndex;
	kSendInfo.bSetup = kInfo.m_bDeploy;	
	kSendInfo.vPos = kInfo.m_vPos;
	kSendInfo.fAngle = kInfo.m_fAngle;
	kSendInfo.objId = kInfo.dwItemId;
	kSendInfo.tKeepTime = kInfo.m_dwTime;
	
	SendClientToWorld( GUILDHOUSE_PCKTTYPE_SETUP, kSendInfo, kInfo.m_nSlotIndex );
	m_dwSelectedObjID = NULL_ID;
	m_iMode = 0;
	//m_nExtraExp = 0;		//gmpbigsun: 버블타임 게이지가 초기화되는 현상-_-;생각없이 초기화를 한것 같군...
}

void CGuildHouseBase::Reset( )
{
	// 가구 편집모드를 불러주는 함수 
	GH_Fntr_Info* pInfo = Find( m_dwSelectedObjID );
	if( !pInfo )
		return;

	int slotIndex = FindIndex( m_dwSelectedObjID );
	if( slotIndex < 0 )
		return;

	HOUSING_ITEM kItem;
	kItem.m_bDeploy = pInfo->bSetup;
	kItem.dwItemId = pInfo->objId;
	kItem.m_nIndex = pInfo->dwItemId;
	kItem.m_vPos = pInfo->vPos;
	kItem.m_fAngle = pInfo->fAngle;
	kItem.m_nSlotIndex = slotIndex;

	ItemProp* pItemProp = prj.GetItemProp( pInfo->dwItemId );
	assert( pItemProp );

	GuildDeploy()->LoadToDeploy(pItemProp->dwLinkKind, kItem );

	m_iMode = GUILDHOUSE_PCKTTYPE_RESET;		//패킷을 실제로 보내기전에 모드를 세팅 ( ACQ 후에는 아무 의미없는 값이다 )
}

void CGuildHouseBase::Reset( const HOUSING_ITEM& kInfo )
{
	// 실제로 재설치 패킷을 전송하는 함수 
	GH_Fntr_Info kSendInfo;
	kSendInfo.dwItemId = kInfo.m_nIndex;
	kSendInfo.bSetup = kInfo.m_bDeploy;	
	kSendInfo.vPos = kInfo.m_vPos;
	kSendInfo.fAngle = kInfo.m_fAngle;
	kSendInfo.objId = kInfo.dwItemId;
	kSendInfo.tKeepTime = kInfo.m_dwTime;

	SendClientToWorld( GUILDHOUSE_PCKTTYPE_RESET, kSendInfo, kInfo.m_nSlotIndex );
	m_dwSelectedObjID = NULL_ID;
	m_iMode = 0;
}

void CGuildHouseBase::Remove( )
{
	int slotIndex = FindIndex( m_dwSelectedObjID );
	
	if( slotIndex < 0 )
		return;
	
	GH_Fntr_Info* pInfo = Find( m_dwSelectedObjID );
	if( !pInfo )
		return;

	//gmpbigsun: testing
	SendClientToWorld( GUILDHOUSE_PCKTTYPE_REMOVE, *pInfo, slotIndex );
	m_dwSelectedObjID = NULL_ID;
	m_iMode = 0;
}

void CGuildHouseBase::ApplyEFTexture( )
{
	CWorld* pWorld = g_WorldMng.Get();
	if( !pWorld )
		return;

	if( !pWorld->IsWorldGuildHouse() )
		return;

	// 벽지, 장판 해체시 복구flag
	BOOL bWallTex = FALSE;
	BOOL bTileTex = FALSE;

	int iMaxSize = GetFurnitureListSize( );
	for( int i = 0; i < iMaxSize; ++i )
	{
		GH_Fntr_Info* pInfo = GetFurnitureInfoPtr( i );
		if( !pInfo )
			continue;
		
		ItemProp* pProp = prj.GetItemProp( pInfo->dwItemId );
		if( !pProp )
		{
			assert( 0 );
			continue;
		}

		if( IK2_GUILDHOUSE_FURNITURE != pProp->dwItemKind2 && IK2_GUILDHOUSE_PAPERING != pProp->dwItemKind2 )  
			continue;

		if( pInfo->bSetup )
		{
			if( IK3_WALLPAPER == pProp->dwItemKind3 )
			{
				GuildDeploy()->ChangeWallTex( pProp->szTextFileName );
				bWallTex = TRUE;
			}
					

			if( IK3_CARPET == pProp->dwItemKind3 )
			{
				GuildDeploy()->ChangeTileTex( pProp->szTextFileName );
				bTileTex = TRUE;
			}
		}
	}

		//장판, 벽지가 설치된게 없다면 default로 
	if( !bWallTex )
		GuildDeploy()->ChangeWallTex( );

	if( !bTileTex )
		GuildDeploy()->ChangeTileTex( );
}

void CGuildHouseBase::ResetValues( )
{
	m_dwGuildId = 0;
	m_dwWorldId = 0;
	m_vecFntInfo.clear();

	m_dwSelectedObjID = 0;
	m_iMode = 0;
	m_nExtraExp = 0;
	m_nWndDeployingIndex = 0;
	m_bSetFurnitureChannel = FALSE;
}

#endif // __CLIENT

#ifdef __WORLDSERVER
BOOL CGuildHouseBase::CreateGuildHouseRoom()
{
	CWorld* pWorld = g_WorldMng.GetWorld( m_dwWorldId );
	if( !pWorld ) return FALSE;
		
	if( pWorld->m_linkMap.GetLinkMap( static_cast<int>( m_dwGuildId ) ) )
		return TRUE;

	if( pWorld->CreateLayer( static_cast<int>( m_dwGuildId ) ) )
	{
		for( int i=0; i<(int)( m_vecFntInfo.size() ); i++ )
		{
			if( m_vecFntInfo[i].bSetup )
			{
				SetupFurnitureCtrl( i, m_vecFntInfo[i] );
				SendWorldToClient( GUILDHOUSE_PCKTTYPE_REFRESH, m_vecFntInfo[i], i );	// OBJID와 남은 시간을 갱신시킨다.
			}
		}
		return TRUE;
	}

	return FALSE;
}

BOOL CGuildHouseBase::DestroyGuildHouseRoom()
{
	CWorld* pWorld = g_WorldMng.GetWorld( m_dwWorldId );
	if( !pWorld || !pWorld->m_linkMap.GetLinkMap( static_cast<int>( m_dwGuildId ) ) )
		return FALSE;

	for( int i=0; i<(int)( m_vecFntInfo.size() ); i++ )
	{
		if( m_vecFntInfo[i].bSetup )
		{
			m_vecFntInfo[i].objId = NULL_ID;
			SendWorldToClient( GUILDHOUSE_PCKTTYPE_REFRESH, m_vecFntInfo[i], i );	// OBJID를 초기화 시키고 남은 시간을 갱신시킨다.
		}
	}

	pWorld->Invalidate( static_cast<int>( m_dwGuildId ) );
	return TRUE;
}

BOOL CGuildHouseBase::IsEnteranceAble( CUser* pUser )
{
	if( m_tUpkeepTime == 0 )	// 유지 기간이 만료되었다.
	{
		pUser->AddDefinedText( TID_GAME_GUILDHOUSE_EXPIRATION );
		return FALSE;
	}
	
	if( !IsValidObj( pUser ) || pUser->m_idGuild != m_dwGuildId )
		return FALSE;

	if( CreateGuildHouseRoom() )	// 입장자가 한명도 없어서 방이 생성되지 않은 경우 생성시킨다.
		return TRUE;

	return FALSE;
}

BOOL CGuildHouseBase::EnteranceGuildHouseRoom( CUser* pUser )
{
	if( IsEnteranceAble( pUser ) )	
	{
		pUser->SetRestPointTick();
		return pUser->REPLACE( g_uIdofMulti, m_dwWorldId, GetEnterPos(), REPLACE_NORMAL, static_cast<int>( m_dwGuildId ) );
	}

	return FALSE;
}

void CGuildHouseBase::GoOutGuildHouseRoom( CUser* pUser )
{
	if( IsValidObj( pUser ) && pUser->GetWorld()->GetID() == m_dwWorldId && pUser->GetLayer() == m_dwGuildId )
		pUser->REPLACE( g_uIdofMulti, WI_WORLD_MADRIGAL, pUser->m_vMarkingPos, REPLACE_FORCE, nDefaultLayer );
}

void CGuildHouseBase::CheckDestroyRoom( CUser* pUser )
{
	CWorld* pWorld = g_WorldMng.GetWorld( m_dwWorldId );
	if( pWorld && pWorld->m_linkMap.GetLinkMap( static_cast<int>( m_dwGuildId ) ) )
	{
	/*
		int nWidth = pWorld->WORLD_WIDTH * pWorld->m_iMPU;
		int nHeight = pWorld->WORLD_HEIGHT * pWorld->m_iMPU;
		D3DXVECTOR3 vPosCenter( nWidth / 2, 0.0f, nHeight / 2 );
		int nRange = max( nWidth, nHeight ) / 2;
		CObj* pObj;
		FOR_LINKMAP( pWorld, vPosCenter, pObj, nRange, CObj::linkPlayer, static_cast<int>( m_dwGuildId ) )
		{
			if( pObj->GetType() == OT_MOVER && static_cast<CMover*>( pObj )->IsPlayer() && pUser != pObj )	// 내가 아닌 다른 사람이 남아있는 경우다.
				return;
		}
		END_LINKMAP
	*/
		if( g_UserMng.HasUserSameWorldnLayer( pUser ) )
			return;

		DestroyGuildHouseRoom();	// 한명도 남아있지 않은 경우 - 방만 파괴시킨다.
	}
}

BOOL CGuildHouseBase::IsAuthority( CUser* pUser, int nPacketType )
{
	if( !IsValidObj( pUser ) || pUser->m_idGuild != m_dwGuildId )	return FALSE;
	CGuild* pGuild = g_GuildMng.GetGuild( m_dwGuildId );
	if( !pGuild )	return FALSE;

	if( nPacketType == GUILDHOUSE_PCKTTYPE_UPKEEP ) // 유지비는 권한만 있으면 OK!
	{
		if( !pGuild->IsAuthority( pUser->m_idPlayer, PF_GUILDHOUSE_UPKEEP ) )
		{
			pUser->AddDefinedText( TID_GAME_GUILDHOUSE_TAX_LEVEL );
			return FALSE;
		}
	}
	else 
	{
		if( !pGuild->IsAuthority( pUser->m_idPlayer, PF_GUILDHOUSE_FURNITURE ) )	// 가구 제어 권한 검사(텔레포터 포함)
		{
			pUser->AddDefinedText( TID_GAME_GUILDHOUSE_INSIDE_INSTALL_LEVEL );
			return FALSE;
		}
		else if( pUser->GetWorld()->GetID() != m_dwWorldId || pUser->GetLayer() != m_dwGuildId )	// 현재 유저가 길드하우스 내에 있는가?
			return FALSE;
	}
	
	return TRUE;
}

BOOL CGuildHouseBase::PreCheckPacket( CUser* pUser, int nPacketType, GH_Fntr_Info& gfi, int nIndex )
{
	// 길드하우스 관련 권한 검사
	if( !IsAuthority( pUser, nPacketType ) )
		return FALSE;
					
	switch( nPacketType )
	{
		case GUILDHOUSE_PCKTTYPE_LISTUP:
			{
				if( !IsListUpAble( gfi.dwItemId ) )
				{
					pUser->AddDefinedText( TID_GAME_GUILDHOUSE_INSIDE_SPACE );
					return FALSE;
				}
				break;
			}

		case GUILDHOUSE_PCKTTYPE_SETUP:
			{
				if( !g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) )
					return FALSE;

				if( nIndex >= (int)( m_vecFntInfo.size() ) )	return FALSE;
				if( m_vecFntInfo[nIndex].bSetup )	return FALSE;

				gfi.bSetup = TRUE;
				ItemProp* pItemProp = prj.GetItemProp( gfi.dwItemId );
				if( !pItemProp ) return FALSE;
				if( pItemProp->dwItemKind3 == IK3_TELEPORTER )
				{
					gfi.vPos = GetTeleporterPos();
					gfi.fAngle = 0.0f;
				}
				if( m_vecFntInfo[nIndex].tKeepTime == 0 )
					gfi.tKeepTime = time_null() + ( pItemProp->dwAbilityMin * 60 );
				
				return IsSetupAble( pUser, nIndex );
			}

		case GUILDHOUSE_PCKTTYPE_REMOVE:
				if( !g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) )
					return FALSE;

				if( nIndex >= (int)( m_vecFntInfo.size() ) )	return FALSE;
				if( !m_vecFntInfo[nIndex].bSetup )	return FALSE;
				gfi.objId = NULL_ID;
				gfi.bSetup = FALSE;
				break;
		
		case GUILDHOUSE_PCKTTYPE_RESET:
				if( !g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) )
					return FALSE;

				if( nIndex >= (int)( m_vecFntInfo.size() ) )	return FALSE;
				if( !m_vecFntInfo[nIndex].bSetup )	return FALSE;
				gfi.bSetup = TRUE;
				gfi.tKeepTime = m_vecFntInfo[nIndex].tKeepTime;
				break;
	
		case GUILDHOUSE_PCKTTYPE_UPKEEP:
			{
				gfi.tKeepTime = gfi.dwItemId * UPKEEP_DAY_TIME + ( GetUpkeepTime() ? GetUpkeepTime() : time_null() );	// 최종 설정될 유지기간(초) : 만료시간
				int nTotalDays = ( gfi.tKeepTime - time_null() ) / UPKEEP_DAY_TIME;	// 총 남은 유지기간(일) : 현재 + 추가
				if( nTotalDays >= MAX_UPKEEP_DAY || nTotalDays <= 0 )
				{
					pUser->AddDefinedText( TID_GAME_GUILDHOUSE_TAX_MAX, "%d", MAX_UPKEEP_DAY );
					return FALSE;
				}
				
				int nPayPenya = gfi.dwItemId * GetUpkeepPenya();
				if( pUser->GetGold() >= nPayPenya )
				{
					pUser->AddGold( -nPayPenya );
					g_DPSrvr.PutPenyaLog( pUser, "f", "GUILDHOUSE_UPKEEP", nPayPenya );
				}
				else
				{
					pUser->AddDefinedText( TID_GAME_GUILDHOUSE_BUY_EXPENSE );
					return FALSE;
				}
				break;
			}
	}

	return TRUE;
}

void CGuildHouseBase::AfterFailedGuildHousePacket( int nPacketType, GH_Fntr_Info& gfi, int nIndex )
{
	CUser* pUser = static_cast<CUser*>( prj.GetUserByID( GetWaitDBAckPlayerId() ) );
	if( !IsValidObj( pUser ) )
	{
		Error( "CGuildHouseBase::AfterFailedGuildHousePacket() : Invalid User! [Packet:%d], [User:%d], [ItemId:%d]", nPacketType, GetWaitDBAckPlayerId(), gfi.dwItemId );
		return;
	}

	pUser->AddDefinedText( TID_GAME_GUILDHOUSE_LATER );
	switch( nPacketType )
	{
		case GUILDHOUSE_PCKTTYPE_LISTUP:
			{
				CItemElem itemElem;
				itemElem.m_dwItemId = gfi.dwItemId;
				itemElem.m_nItemNum	= 1;
				if( pUser->CreateItem( &itemElem ) )
					g_DPSrvr.PutItemLog( pUser, "f", "GUILDHOUSE_LISTUP_REPAIR", &itemElem, 1 );
				else
					Error( "CGuildHouseBase::AfterFailedGuildHousePacket() : CreateItem Failed! [User:%d], [ItemId:%d]", pUser->m_idPlayer, gfi.dwItemId );
				break;
			}

		case GUILDHOUSE_PCKTTYPE_UPKEEP:
			{
				pUser->AddGold( gfi.dwItemId * GetUpkeepPenya() );
				g_DPSrvr.PutPenyaLog( pUser, "f", "GUILDHOUSE_UPKEEP_REPAIR", (int)( gfi.dwItemId ) * GetUpkeepPenya() * ( -1 ) );		//sun : safecast

				break;
			}
	}
}

BOOL CGuildHouseBase::SendWorldToDatabase( CUser* pUser, int nPacketType, GH_Fntr_Info& gfi, int nIndex )
{
	if( !IsValidObj( pUser ) )
		return FALSE;
	
	if( IsWaitDBAck() )
	{
		pUser->AddDefinedText( TID_GAME_GUILDHOUSE_LATER );
		return FALSE;
	}

	if( !PreCheckPacket( pUser, nPacketType, gfi, nIndex ) )
		return FALSE;
	
	SetWaitDBAckPlayerId( pUser->m_idPlayer );
	{
		BEFORESENDDUAL( ar, PACKETTYPE_GUILDHOUSE_PACKET, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << pUser->m_idGuild;
		ar << nPacketType << nIndex;
		gfi.Serialize( ar );
		SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );
	}

	return TRUE;
}

void CGuildHouseBase::SendWorldToClient( int nPacketType, GH_Fntr_Info& gfi, int nIndex )
{
	CGuild* pGuild = g_GuildMng.GetGuild( m_dwGuildId );
	if( pGuild )
	{
		for( map<u_long, CGuildMember*>::iterator it=pGuild->m_mapPMember.begin(); it!=pGuild->m_mapPMember.end(); it++ )
		{
			CUser* pUser = static_cast<CUser*>( prj.GetUserByID( it->first ) );
			if( IsValidObj( pUser ) )
				pUser->AddGuildHousePakcet( nPacketType, gfi, nIndex );
		}
	}
}

void CGuildHouseBase::SetApplyDST( CUser* pUser )
{
	if( !IsValidObj( pUser ) )
		return;

	for( int i=0; i<(int)( m_vecFntInfo.size() ); i++ )
	{
		if( m_vecFntInfo[i].bSetup )
		{
			ItemProp* pItemProp = prj.GetItemProp( m_vecFntInfo[i].dwItemId );
			if( pItemProp )
			{
				pUser->SetDestParam( 0, pItemProp );
				pUser->SetDestParam( 1, pItemProp );
				pUser->SetDestParam( 2, pItemProp );
			}
		}
	}
}

void CGuildHouseBase::ResetApplyDST( CUser* pUser )
{
	if( !IsValidObj( pUser ) )
		return;

	for( int i=0; i<(int)( m_vecFntInfo.size() ); i++ )
	{
		if( m_vecFntInfo[i].bSetup )
		{
			ItemProp* pItemProp = prj.GetItemProp( m_vecFntInfo[i].dwItemId );
			if( pItemProp )
			{
				pUser->ResetDestParam( 0, pItemProp );
				pUser->ResetDestParam( 1, pItemProp );
				pUser->ResetDestParam( 2, pItemProp );
			}
		}
	}
}

void CGuildHouseBase::SetDSTFunriture( ItemProp* pItemProp )
{
	if( !pItemProp )
		return;
	
	CGuild* pGuild = g_GuildMng.GetGuild( m_dwGuildId );
	if( pGuild )
	{
		for( map<u_long, CGuildMember*>::iterator it=pGuild->m_mapPMember.begin(); it!=pGuild->m_mapPMember.end(); it++ )
		{
			CUser* pUser = static_cast<CUser*>( prj.GetUserByID( it->first ) );
			if( IsValidObj( pUser ) )
			{
				pUser->SetDestParam( 0, pItemProp );
				pUser->SetDestParam( 1, pItemProp );
				pUser->SetDestParam( 2, pItemProp );
			}
		}
	}
}

void CGuildHouseBase::ResetDSTFunriture( ItemProp* pItemProp )
{
	if( !pItemProp )
		return;

	CGuild* pGuild = g_GuildMng.GetGuild( m_dwGuildId );
	if( pGuild )
	{
		for( map<u_long, CGuildMember*>::iterator it=pGuild->m_mapPMember.begin(); it!=pGuild->m_mapPMember.end(); it++ )
		{
			CUser* pUser = static_cast<CUser*>( prj.GetUserByID( it->first ) );
			if( IsValidObj( pUser ) )
			{
				pUser->ResetDestParam( 0, pItemProp );
				pUser->ResetDestParam( 1, pItemProp );
				pUser->ResetDestParam( 2, pItemProp );
			}
		}
	}
}

BOOL CGuildHouseBase::IsListUpAble( DWORD dwItemId )
{
	if( m_vecFntInfo.size() >= MAX_LISTUP_NUM )
		return FALSE;

	ItemProp* pItemProp = prj.GetItemProp( dwItemId );
	if( !pItemProp )
		return FALSE;

	int nMaxNum = GetMaxListUpNum( pItemProp );
	int nItemCount = 0;
	for( int i=0; i<(int)( m_vecFntInfo.size() ); i++ )
	{
		ItemProp* pTargetProp = prj.GetItemProp( m_vecFntInfo[i].dwItemId );
		if( pTargetProp && pItemProp->dwItemKind3 == pTargetProp->dwItemKind3 )
			nItemCount++;

		if( nItemCount >= nMaxNum )
			return FALSE;
	}

	return TRUE;
}
#endif // __WORLDSERVER

#ifdef __DBSERVER
int	CGuildHouseBase::GetFirstExpiredFurnitureIndex()
{
	for( int i=0; i<(int)( m_vecFntInfo.size() ); i++ )
	{
		if( m_vecFntInfo[i].tKeepTime != 0 && m_vecFntInfo[i].tKeepTime <= time_null() )
			return i;
	}

	return NULL_ID;
}
#endif // __DBSERVER

void CGuildHouseBase::SerializeAllInfo( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_dwGuildId << m_dwWorldId;
		if( m_tUpkeepTime > 0 )
			ar << m_tUpkeepTime - time_null();
		else
			ar << static_cast<time_t>( 0 );
#ifdef __GUILD_HOUSE_MIDDLE
		ar << m_objGHId << m_nAreaIndex << m_nGHLevel;
		ar.WriteString( m_szGHName );
#endif // __GUILD_HOUSE_MIDDLE
		ar << m_vecFntInfo.size();
		for( int i=0; i<(int)( m_vecFntInfo.size() ); i++ )
			m_vecFntInfo[i].Serialize( ar );
	}
	else
	{
		ar >> m_dwGuildId >> m_dwWorldId >> m_tUpkeepTime;
		if( m_tUpkeepTime > 0 )
			m_tUpkeepTime += time_null();
#ifdef __GUILD_HOUSE_MIDDLE
		ar >> m_objGHId >> m_nAreaIndex >> m_nGHLevel;
		ar.ReadString( m_szGHName, MAX_GH_NAME );
#endif // __GUILD_HOUSE_MIDDLE
		int nSize;
		ar >> nSize;
		m_vecFntInfo.clear(); m_vecFntInfo.resize( nSize );
		for( int i=0; i<nSize; i++ )
			m_vecFntInfo[i].Serialize( ar );
	}
}

BOOL CGuildHouseBase::OnGuildHousePacket( int nPacketType, GuildHouse_Furniture_Info & gfi, int nIndex )
{
	switch( nPacketType )
	{
		case GUILDHOUSE_PCKTTYPE_LISTUP:
			return ListUpFurniture( gfi );

		case GUILDHOUSE_PCKTTYPE_LISTDROP:
			return ListDropFurniture( nIndex );

		case GUILDHOUSE_PCKTTYPE_SETUP:
#ifdef __CLIENT
			{
				ItemProp* pProp = prj.GetItemProp( gfi.dwItemId );
				if( pProp )
				{
					CString str;
					str.Format( GETTEXT( TID_GAME_GUILDHOUSE_FUR_INSTALL_SUCCESS ), pProp->szName );		//설치성공 
					g_WndMng.PutString( str, NULL, prj.GetTextColor( TID_GAME_GUILDHOUSE_FUR_INSTALL_SUCCESS ) );
				}
			}

#endif // __CLIENT
#ifdef __WORLDSERVER
			g_dpDBClient.SendLogGuildFurniture( m_dwGuildId, gfi, 'S' );
#endif // __WORLDSERVER
			return SetupFurnitureCtrl( nIndex, gfi );

		case GUILDHOUSE_PCKTTYPE_REMOVE:
#ifdef __CLIENT
			{
				ItemProp* pProp = prj.GetItemProp( gfi.dwItemId );
				if( pProp )
				{
					CString str;
					str.Format( GETTEXT( TID_GAME_GUILDHOUSE_FUR_DISMANTLE_SUCCESS ), pProp->szName );	//해체 성공  
					g_WndMng.PutString( str, NULL, prj.GetTextColor( TID_GAME_GUILDHOUSE_FUR_DISMANTLE_SUCCESS ) );
				}
			}
#endif // __CLIENT
#ifdef __WORLDSERVER
			g_dpDBClient.SendLogGuildFurniture( m_dwGuildId, gfi, 'R' );
#endif // __WORLDSERVER
			return RemoveFurnitureCtrl( nIndex, gfi );

		case GUILDHOUSE_PCKTTYPE_RESET:
#ifdef __CLIENT
			{
				ItemProp* pProp = prj.GetItemProp( gfi.dwItemId );
				if( pProp )
				{
					CString str;
					str.Format( GETTEXT( TID_GAME_GUILDHOUSE_FUR_REINSTALL_SUCCESS ), pProp->szName );	//재설치 성공 
					g_WndMng.PutString( str, NULL, prj.GetTextColor( TID_GAME_GUILDHOUSE_FUR_REINSTALL_SUCCESS ) );
				}
			}
#endif
#ifdef __WORLDSERVER
			g_dpDBClient.SendLogGuildFurniture( m_dwGuildId, gfi, 'C' );
#endif // __WORLDSERVER
			return ResetFurnitureCtrl( nIndex, gfi );
#ifdef __CLIENT
		case GUILDHOUSE_PCKTTYPE_REFRESH:
			return SetupFurnitureCtrl( nIndex, gfi );
#endif // __CLIENT
		
		case GUILDHOUSE_PCKTTYPE_UPKEEP:
			SetUpkeeptime( gfi.tKeepTime );
			return TRUE;

		case GUILDHOUSE_PCKTTYPE_EXPIRED:
			return ExpiredGuildHouse();
	}

	return FALSE;
}

BOOL CGuildHouseBase::ListUpFurniture( GuildHouse_Furniture_Info & gfi )
{
	m_vecFntInfo.push_back( gfi );
#ifdef __WORLDSERVER
	g_dpDBClient.SendLogGuildFurniture( m_dwGuildId, gfi, 'I' );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL CGuildHouseBase::ListDropFurniture( int nIndex )
{
	if( (int)( m_vecFntInfo.size() ) <= nIndex )
		return FALSE;
#ifdef __WORLDSERVER
	if( m_vecFntInfo[nIndex].bSetup == TRUE )
		RemoveFurnitureCtrl( nIndex, m_vecFntInfo[nIndex] );

	g_dpDBClient.SendLogGuildFurniture( m_dwGuildId, m_vecFntInfo[nIndex], 'D' );
#endif // __WORLDSERVER
	m_vecFntInfo.erase( m_vecFntInfo.begin() + nIndex );
	return TRUE;
}

BOOL CGuildHouseBase::SetupFurnitureCtrl( int nIndex, GuildHouse_Furniture_Info & gfi )
{
	if( (int)( m_vecFntInfo.size() ) <= nIndex || m_vecFntInfo[nIndex].dwItemId != gfi.dwItemId )	return FALSE;
	ItemProp* pItemProp = prj.GetItemProp( gfi.dwItemId );
	if( !pItemProp )	return FALSE;

#ifdef __WORLDSERVER
	// 레이어가 존재하는지 확인하고 가구 컨트롤 생성...
	CWorld* pWorld = g_WorldMng.GetWorld( m_dwWorldId );
	if( pWorld && pWorld->m_linkMap.GetLinkMap( static_cast<int>( m_dwGuildId ) ) )
	{
		CCtrl* pCtrl = NULL;
		if( pItemProp->dwItemKind2 == IK2_GUILDHOUSE_NPC )	// 텔레포터는 NPC이기 때문에 무버를 생성해야 한다.
		{
			pCtrl = static_cast<CCtrl*>( CreateObj( D3DDEVICE, OT_MOVER, pItemProp->dwLinkKind ) );
			CMover* pMover = static_cast<CMover*>( pCtrl );
			if( pMover )
			{
				lstrcpy( pMover->m_szCharacterKey, pItemProp->szTextFileName );
				pMover->InitNPCProperty();
				pMover->InitCharacter( pMover->GetCharacter() );
				pMover->InitMotion( MTI_STAND );
				pMover->UpdateLocalMatrix();
			}
		}
		else if( pItemProp->dwItemKind2 == IK2_GUILDHOUSE_FURNITURE )
			pCtrl = static_cast<CCtrl*>( CreateObj( D3DDEVICE, OT_CTRL, pItemProp->dwLinkKind ) );

		if( pCtrl )
		{
			pCtrl->SetPos( gfi.vPos );
			pCtrl->SetAngle( gfi.fAngle );
			pCtrl->AddItToGlobalId();
			gfi.objId = pCtrl->GetId();
			pWorld->ADDOBJ( pCtrl, FALSE, static_cast<int>( m_dwGuildId ) );
		}
	}

	if( !m_vecFntInfo[nIndex].bSetup )	// 현재 해제되어 있던거만 능력치 설정
		SetDSTFunriture( pItemProp );	// 가구 능력치 설정
#endif // __WORLDSERVER

	m_vecFntInfo[nIndex] = gfi;
	return TRUE;
}

BOOL CGuildHouseBase::RemoveFurnitureCtrl( int nIndex, GuildHouse_Furniture_Info & gfi )
{
	if( (int)( m_vecFntInfo.size() ) <= nIndex || m_vecFntInfo[nIndex].dwItemId != gfi.dwItemId )	return FALSE;
	ItemProp* pItemProp = prj.GetItemProp( m_vecFntInfo[nIndex].dwItemId );
	if( !pItemProp )	return FALSE;

#ifdef __WORLDSERVER
	CWorld* pWorld = g_WorldMng.GetWorld( m_dwWorldId );
	if( pWorld && pWorld->m_linkMap.GetLinkMap( static_cast<int>( m_dwGuildId ) ) )
	{
		// 가구 컨트롤을 찾아서 삭제
		CCtrl* pCtrl = prj.GetCtrl( m_vecFntInfo[nIndex].objId );
		if( pCtrl )
			pCtrl->Delete();
	}
	
	if( m_vecFntInfo[nIndex].bSetup )	// 현재 설치되어 있던거만 능력치 해제
		ResetDSTFunriture( pItemProp );	// 가구 능력치 해제
#endif // __WORLDSERVER
	m_vecFntInfo[nIndex] = gfi;
	return TRUE;
}

BOOL CGuildHouseBase::ResetFurnitureCtrl( int nIndex, GuildHouse_Furniture_Info & gfi )
{
	if( (int)( m_vecFntInfo.size() ) <= nIndex || m_vecFntInfo[nIndex].dwItemId != gfi.dwItemId )	return FALSE;
	ItemProp* pItemProp = prj.GetItemProp( gfi.dwItemId );
	if( !pItemProp )	return FALSE;
	if( pItemProp->dwItemKind3 == IK3_TELEPORTER )	return FALSE;

#ifdef __WORLDSERVER
	CWorld* pWorld = g_WorldMng.GetWorld( m_dwWorldId );
	if( pWorld && pWorld->m_linkMap.GetLinkMap( static_cast<int>( m_dwGuildId ) ) )
	{
		// 일단 먼저 재배치할 가구를 삭제한다.
		CCtrl* pCtrl	= prj.GetCtrl( m_vecFntInfo[nIndex].objId );
		if( pCtrl )
		{
			pCtrl->Delete();	// 삭제
			// 그리고 다시 생성!!!
			pCtrl = (CCtrl*)CreateObj( D3DDEVICE, OT_CTRL, pItemProp->dwLinkKind );
			if( pCtrl )
			{
				pCtrl->SetPos( gfi.vPos );
				pCtrl->SetAngle( gfi.fAngle );
				pCtrl->AddItToGlobalId();
				gfi.objId = pCtrl->GetId();
				pWorld->ADDOBJ( pCtrl, FALSE, static_cast<int>( m_dwGuildId ) );
			}
		}
	}
#endif // __WORLDSERVER
	m_vecFntInfo[nIndex] = gfi;
	return TRUE;	
}

BOOL CGuildHouseBase::ExpiredGuildHouse()
{
	for( int i=0; i<(int)( m_vecFntInfo.size() ); i++ )
	{
		if( m_vecFntInfo[i].bSetup )
		{
			RemoveFurnitureCtrl( i, m_vecFntInfo[i] );
			m_vecFntInfo[i].bSetup = FALSE;
			m_vecFntInfo[i].objId = NULL_ID;
		}
	}
	m_tUpkeepTime = 0;
#ifdef __WORLDSERVER
	DestroyGuildHouseRoom();
#endif // __WORLDSERVER
	return TRUE;
}

#ifdef __GUILD_HOUSE_MIDDLE
#ifndef __CLIENT
void CGuildHouseBase::SetGuildHouseData( OBJID objGHId, int nAreaIndex, int nGHLevel, time_t tUpkeepTime, const char* szGHName )
{
	m_objGHId = objGHId;
	m_nAreaIndex = nAreaIndex;
	m_nGHLevel = nGHLevel;
	m_tUpkeepTime = tUpkeepTime;
	::memset( m_szGHName, 0, sizeof( m_szGHName ) );
	_tcscpy( m_szGHName, szGHName );
}
#endif // __nCLIENT
#endif // __GUILD_HOUSE_MIDDLE

#ifndef __CLIENT
//////////////////////////////////////////////////////////////////////
// CGuildHouseSmall : CGuildBase
//////////////////////////////////////////////////////////////////////
CGuildHouseSmall::CGuildHouseSmall( DWORD dwGuildId )
: CGuildHouseBase( dwGuildId, WI_GUILDHOUSE_SMALL )
{
}

CGuildHouseSmall::~CGuildHouseSmall()
{
}


#ifdef __WORLDSERVER
int	CGuildHouseSmall::GetMaxListUpNum( ItemProp* pItemProp )
{
	if( !pItemProp )
		return 0;

	return MAX_LISTUP_NUM;
}

// 소형 길드하우스는 종류별 개수 제한만 있다.
BOOL CGuildHouseSmall::IsSetupAble( CUser* pUser, int nIndex )
{	
	ItemProp* pItemProp = prj.GetItemProp( m_vecFntInfo[nIndex].dwItemId );
	if( !pItemProp )
		return FALSE;

	if( pItemProp->dwAbilityMax > 1 )
	{
		pUser->AddDefinedText( TID_GAME_GUILDHOUSE_FUR_INSTALL_LEVEL, "%d", pItemProp->dwAbilityMax );
		return FALSE;
	}

	int nMaxSetupNum = 0;
	switch( pItemProp->dwItemKind3 )
	{
		case IK3_CHAIR:	nMaxSetupNum = 4;	break;
		default:		nMaxSetupNum = 1;	break;
	}

	int nSetupNum = 0;
	for( int i=0; i<(int)( m_vecFntInfo.size() ); i++ )
	{
		if( !m_vecFntInfo[i].bSetup )
			continue;
		
		ItemProp* pSetup = prj.GetItemProp( m_vecFntInfo[i].dwItemId );
		if( pItemProp->dwItemKind3 == pSetup->dwItemKind3 )
			nSetupNum++;

		if( nSetupNum >= nMaxSetupNum )
		{
			pUser->AddDefinedText( TID_GAME_GUILDHOUSE_INSIDE_INSTALL_KIND, "%d", nMaxSetupNum );	
			return FALSE;
		}
	}

	return TRUE;
}
#endif // __WORLDSERVER

//////////////////////////////////////////////////////////////////////
// CGuildHouseMiddle : CGuildBase
//////////////////////////////////////////////////////////////////////
CGuildHouseMiddle::CGuildHouseMiddle( DWORD dwGuildId )
: CGuildHouseBase( dwGuildId, WI_GUILDHOUSE_MIDDLE )
{
}

CGuildHouseMiddle::~CGuildHouseMiddle()
{
}



/*
//////////////////////////////////////////////////////////////////////
// CGuildHouseLarge : CGuildBase
//////////////////////////////////////////////////////////////////////
CGuildHouseLarge::CGuildHouseLarge( DWORD dwGuildId )
: CGuildHouseBase( dwGuildId, WI_GUILDHOUSE_LARGE )
{
}

CGuildHouseLarge::~CGuildHouseLarge()
{
}
*/



//////////////////////////////////////////////////////////////////////
// CGuildHouseMng
//////////////////////////////////////////////////////////////////////
CGuildHouseMng::CGuildHouseMng()
#ifdef __DBSERVER
: m_nSeqNum( 0 )
#endif // __DBSERVER

{
}

CGuildHouseMng::~CGuildHouseMng()
{
	for( MapGuildHouse::iterator it=m_mapGuildHouse.begin(); it!=m_mapGuildHouse.end(); it++ )
		delete it->second;
	m_mapGuildHouse.clear();
#ifdef __GUILD_HOUSE_MIDDLE
	m_mapTenderData.clear();
	m_mapGHData.clear();
	m_mapTenderGuild.clear();
#endif // __GUILD_HOUSE_MIDDLE
}

CGuildHouseMng* CGuildHouseMng::GetInstance()
{
	static CGuildHouseMng sGHM;
	return & sGHM;
}

CGuildHouseBase* CGuildHouseMng::MakeGuildHouse( DWORD dwGuildId, DWORD dwWorldId )
{
	switch( dwWorldId )
	{
		case WI_GUILDHOUSE_SMALL:	return new CGuildHouseSmall( dwGuildId );
		case WI_GUILDHOUSE_MIDDLE:	return new CGuildHouseMiddle( dwGuildId );
//		case WI_GUILDHOUSE_LARGE:	return new CGuildHouseLarge( dwGuildId );	// 15차 현재 - 아직 필요하지 않다.
	}

	return NULL;
}

BOOL CGuildHouseMng::RemoveGuildHouse( DWORD dwGuildId )
{
	MapGuildHouse::iterator it = m_mapGuildHouse.find( dwGuildId );
	if( it == m_mapGuildHouse.end() )
		return FALSE;
	
	SAFE_DELETE( it->second );
	m_mapGuildHouse.erase( it );
	return TRUE;
}

CGuildHouseBase* CGuildHouseMng::GetGuildHouse( DWORD dwGuildId )
{
	MapGuildHouse::iterator it = m_mapGuildHouse.find( dwGuildId );
	if( it == m_mapGuildHouse.end() )
		return NULL;

	return it->second;
}

void CGuildHouseMng::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_mapGuildHouse.size();
		for( MapGuildHouse::iterator it=m_mapGuildHouse.begin(); it!=m_mapGuildHouse.end(); it++ )
		{
			ar << it->first << it->second->GetType();
			it->second->SerializeAllInfo( ar );
		}
#ifdef __GUILD_HOUSE_MIDDLE
		ar << m_mapTenderGuild.size();
		for( MAP_TENDER_GUILD::iterator it = m_mapTenderGuild.begin(); it != m_mapTenderGuild.end(); ++it )
		{
			ar << it->first;
			it->second.Serialize( ar );
		}
#endif // __GUILD_HOUSE_MIDDLE
	}
	else
	{
		m_mapGuildHouse.clear();
		int nSize;
		DWORD dwGuildId, dwType;
		ar >> nSize;
		for( int i=0; i<nSize; i++ )
		{
			ar >> dwGuildId >> dwType;
			CGuildHouseBase* pGuildHouse = MakeGuildHouse( dwGuildId, dwType );
			
			if( pGuildHouse && AddGuildHouse( dwGuildId, pGuildHouse ) )
				pGuildHouse->SerializeAllInfo( ar );
			else
			{
				SAFE_DELETE( pGuildHouse );
				CGuildHouseSmall temp( NULL_ID );
				temp.SerializeAllInfo( ar );
			}
		}
#ifdef __GUILD_HOUSE_MIDDLE
		m_mapTenderGuild.clear();
		int nTenderSize;
		DWORD dwTenderGuildId;
		ar >> nTenderSize;
		for( int i = 0; i < nTenderSize; ++i )
		{
			GUILDHOUSE_TENDER GHT;
			ar >> dwTenderGuildId;
			GHT.Serialize( ar );
			m_mapTenderGuild.insert( MAP_TENDER_GUILD::value_type( dwTenderGuildId, GHT ) );
		}
#endif // __GUILD_HOUSE_MIDDLE
	}
}

#ifdef __WORLDSERVER
BOOL CGuildHouseMng::IsBuyAble( CUser* pUser )
{
	if( !IsValidObj( pUser ) )
		return FALSE;

	CGuild* pGuild = pUser->GetGuild();
	if( pGuild )
	{
		if( !pGuild->IsMaster( pUser->m_idPlayer ) )
		{
			pUser->AddDefinedText( TID_GAME_GUILDHOUSE_BUY_MASTER );
			return FALSE;
		}
		
		if( pGuild->m_nLevel < BUY_MIN_LEVEL )
		{
			pUser->AddDefinedText( TID_GAME_GUILDHOUSE_BUY_LEVEL, "%d", BUY_MIN_LEVEL );
			return FALSE;
		}

		if( pGuild->GetSize() < BUY_MIN_MEMBERSIZE )
		{
			pUser->AddDefinedText( TID_GAME_GUILDHOUSE_BUY_POPULATION, "%d", BUY_MIN_MEMBERSIZE );
			return FALSE;
		}

		if( pUser->GetGold() < BUY_PENYA )
		{
			pUser->AddDefinedText( TID_GAME_GUILDHOUSE_BUY_EXPENSE );
			return FALSE;
		}

		if( m_mapGuildHouse.find( pGuild->GetGuildId() ) != m_mapGuildHouse.end() )
		{
			pUser->AddDefinedText( TID_GAME_GUILDHOUSE_BUY_HAVE );
			return FALSE;
		}
	}
	else
	{
		pUser->AddDefinedText( TID_GAME_GUILDHOUSE_BUY_MASTER );
		return FALSE;
	}

	return TRUE;
}

void CGuildHouseMng::ReqBuyGuildHouse( CUser* pUser )
{
	if( IsBuyAble( pUser ) )
	{
		pUser->AddGold( -BUY_PENYA );
		g_DPSrvr.PutPenyaLog( pUser, "f", "GUILDHOUSE_BUY", BUY_PENYA ); // 페냐 로그

		BEFORESENDDUAL( ar, PACKETTYPE_GUILDHOUSE_BUY, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << pUser->m_idPlayer << pUser->m_idGuild;
		SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );
	}
}

void CGuildHouseMng::OnBuyGuildHouse( CAr & ar )
{
	BOOL bResult;
	DWORD dwPlayerId, dwGuildId;
	
	ar >> dwPlayerId >> dwGuildId >> bResult;
	if( bResult )
	{
		CGuildHouseBase* pGuildHouse = MakeGuildHouse( dwGuildId, WI_GUILDHOUSE_SMALL );
		if( pGuildHouse && AddGuildHouse( dwGuildId, pGuildHouse ) )
		{
			pGuildHouse->SerializeAllInfo( ar );
			CUser* pUser = static_cast<CUser*>( prj.GetUserByID( dwPlayerId ) );
			//if( IsValidObj( pUser ) )
			//	pUser->AddDefinedText( TID_GAME_GUILDHOUSE_PURCHASE );

			CGuild* pGuild = g_GuildMng.GetGuild( dwGuildId );
			if( pGuild )
			{
				for( map<u_long, CGuildMember*>::iterator it=pGuild->m_mapPMember.begin(); it!=pGuild->m_mapPMember.end(); it++ )
				{
					pUser = static_cast<CUser*>( prj.GetUserByID( it->first ) );
					if( IsValidObj( pUser ) )
					{
						pUser->AddGuildHouseAllInfo( pGuildHouse );
						pUser->AddDefinedText( TID_GAME_GUILDHOUSE_PURCHASE );
						pGuildHouse->SetApplyDST( pUser );
					}
				}
			}
		}
		else
		{
			SAFE_DELETE( pGuildHouse );
			Error( "CGuildHouseMng::BuyGuildHouse - CreateGuildHouse Failed!!! [User:%07d, Guild:%06d]", dwPlayerId, dwGuildId ); 
			CGuildHouseSmall temp( NULL_ID );
			temp.SerializeAllInfo( ar );
		}
	}
	else
	{
		CUser* pUser = static_cast<CUser*>( prj.GetUserByID( dwPlayerId ) );
		if( IsValidObj( pUser ) )
		{
			pUser->AddGold( BUY_PENYA );
			// g_DPSrvr.PutPenyaLog( pUser, '1', "BuyGuildHouse_Reapir", BUY_PENYA ); // 페냐 로그
			pUser->AddDefinedText( TID_GAME_GUILDHOUSE_BUY_HAVE );
		}
	}
}

void CGuildHouseMng::OnRemoveGuildHouse( CAr & ar )
{
	DWORD dwGuildId;
	ar >> dwGuildId;

	RemoveGuildHouse( dwGuildId );
	
	CGuild* pGuild = g_GuildMng.GetGuild( dwGuildId );
	if( pGuild )
	{
		for( map<u_long, CGuildMember*>::iterator it=pGuild->m_mapPMember.begin(); it!=pGuild->m_mapPMember.end(); it++ )
		{
			CUser* pUser = static_cast<CUser*>( prj.GetUserByID( it->first ) );
			if( IsValidObj( pUser ) )
			{
				pUser->SetRestPoint( 0 );
				pUser->AddGuildHouseRemvoe();
			}
		}
	}
}

BOOL CGuildHouseMng::SendWorldToDatabase( CUser* pUser, int nPacketType, GH_Fntr_Info& gfi, int nIndex )
{
	if( !IsValidObj( pUser ) )
		return FALSE;

	CGuildHouseBase* pGuildHouse = GetGuildHouse( pUser->m_idGuild );
	if( pGuildHouse )
		return pGuildHouse->SendWorldToDatabase( pUser, nPacketType, gfi, nIndex );

	return FALSE;
}

BOOL CGuildHouseMng::IsGuildHouse( DWORD dwWorldId )
{
	switch( dwWorldId )
	{
		case WI_GUILDHOUSE_SMALL:
		case WI_GUILDHOUSE_MIDDLE:
		case WI_GUILDHOUSE_LARGE:
			return TRUE;
	}

	return FALSE;
}

BOOL CGuildHouseMng::IsLoginAble( CUser* pUser, DWORD dwWorldId, int nLayer )
{
	if( !IsGuildHouse( dwWorldId ) )	// 접속할때 World가 길드하우스가 아니면 패스!!!
		return TRUE;

	if( pUser->m_idGuild != nLayer )	// 일단 길드하우스라면 최종 접속 레이어가 캐릭터의 길드ID와 일치해야 한다.
		return FALSE;
	
	CGuildHouseBase* pGuildHouse = GetGuildHouse( pUser->m_idGuild );
	if( pGuildHouse )	// 입장 가능한 캐릭터이면 휴식 포인트를 요청한다.
		return pGuildHouse->IsEnteranceAble( pUser );
	
	return FALSE;
}

BOOL CGuildHouseMng::EnteranceGuildHouse( CUser* pUser, DWORD dwComebackItemId )
{
	CGuildHouseBase* pGuildHouse = GetGuildHouse( pUser->m_idGuild );
	if( !pGuildHouse )
	{
		pUser->AddDefinedText( TID_GAME_GUILDHOUSE_ENTER_HAVE );
		return FALSE;
	}

	switch( dwComebackItemId )
	{
		case NULL_ID :	// 길드하우스 귀환 주문서를 사용하지 않은 경우(NPC 입장 메뉴 선택)
			{
				if( CNpcChecker::GetInstance()->IsCloseNpc( MMI_GUILDHOUSE_ENTER, pUser->GetWorld(), pUser->GetPos() ) )
					pUser->SetMarkingPos();
				else
					return FALSE;
				break;
			}

		default:
			{
				if( pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_MADRIGAL )
				{
					pUser->m_idMarkingWorld = WI_WORLD_MADRIGAL;
					pUser->m_vMarkingPos = D3DXVECTOR3( 6968.0f, 0.0f, 3328.8f );
				}
				else
				{
					pUser->AddDefinedText( TID_GAME_GUILDHOUSE_COMEBACK_ERROR01 );
					return FALSE;
				}
				break;
			}
	}

	return pGuildHouse->EnteranceGuildHouseRoom( pUser );
}

void CGuildHouseMng::GoOutGuildHouse( CUser* pUser )
{
	CGuildHouseBase* pGuildHouse = GetGuildHouse( pUser->m_idGuild );
	if( pGuildHouse )
		pGuildHouse->GoOutGuildHouseRoom( pUser );
}

void CGuildHouseMng::CheckDestroyGuildHouse( CUser* pUser )
{
	CGuildHouseBase* pGuildHouse = GetGuildHouse( pUser->m_idGuild );
	if( pGuildHouse )
		pGuildHouse->CheckDestroyRoom( pUser );
}

void CGuildHouseMng::SetApplyDST( CUser* pUser )
{
	CGuildHouseBase* pGuildHouse = GetGuildHouse( pUser->m_idGuild );
	if( pGuildHouse )
		pGuildHouse->SetApplyDST( pUser );
}

void CGuildHouseMng::ResetApplyDST( CUser* pUser )
{
	CGuildHouseBase* pGuildHouse = GetGuildHouse( pUser->m_idGuild );
	if( pGuildHouse )
		pGuildHouse->ResetApplyDST( pUser );
}

float CGuildHouseMng::GetRestPointFactor( CUser* pUser )
{
	CGuildHouseBase* pGuildHouse = GetGuildHouse( pUser->m_idGuild );
	if( pGuildHouse )
		return pGuildHouse->GetRestPointFactor();
	
	return 0.0f;
}

void CGuildHouseMng::AddRestExpFactorTable( int nMinLevel, int nMaxLevel, float fFactor )
{
	for( int i=nMinLevel; i<=nMaxLevel; i++ )
	{
		if( !m_mapRestExpFactorTable.insert( make_pair( i, fFactor ) ).second )
			Error( "CGuildHouseMng::AddRestExpFactorTable() Insert Failed! - [Level:%d]", i );
	}
}

float CGuildHouseMng::GetRestExpFactor( int nLevel )
{
	MapRestExpFactor::iterator it = m_mapRestExpFactorTable.find( nLevel );
	if( it == m_mapRestExpFactorTable.end() )
		return 0.0f;

	return it->second;
}
#endif // __WORLDSERVER

#ifdef __DBSERVER
extern	APP_INFO	g_appInfo;
void CGuildHouseMng::ProcessExpired()
{
	for( MapGuildHouse::iterator it=m_mapGuildHouse.begin(); it!=m_mapGuildHouse.end(); it++ )
	{
		int nIndex = it->second->GetFirstExpiredFurnitureIndex();	// 기간이 만료된 가구 한개를 가져온다.
		if( nIndex != NULL_ID )	// 기간이 만료된 가구가 있는 경우 리스트에서 제거한다.
		{
			CAr ar;
			ar << it->first << GUILDHOUSE_PCKTTYPE_LISTDROP << nIndex;
			it->second->GetFurnitureInfo( nIndex ).Serialize( ar );
			int nBufSize;
			LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );
			GuildHouseDBMng->PostRequest( GUILDHOUSE_DEFAULT_PACKET, lpBuf, nBufSize );
		}

		if( it->second->GetUpkeepTime() <= time_null() )	// 길드하우스 유지 기간이 만료된 경우
		{
			if( it->second->GetUpkeepTime() > 0 )	// 길드하우스가 활성화 되어있는 상태이면 비활성화 시킨다.
			{
#ifdef __GUILD_HOUSE_MIDDLE
				CAr ar;
				ar << it->first << it->second->GetType();
				int nBufSize;
				LPBYTE lpBuf = ar.GetBuffer( &nBufSize );
				GuildHouseDBMng->PostRequest( GUILDHOUSE_EXPIRED, lpBuf, nBufSize );
#else // __GUILD_HOUSE_MIDDLE
				GH_Fntr_Info gfiTemp;
				CAr ar;
				ar << it->first << GUILDHOUSE_PCKTTYPE_EXPIRED << NULL_ID;
				gfiTemp.Serialize( ar );
				int nBufSize;
				LPBYTE lpBuf = ar.GetBuffer( &nBufSize );
				GuildHouseDBMng->PostRequest( GUILDHOUSE_DEFAULT_PACKET, lpBuf, nBufSize );
#endif // __GUILD_HOUSE_MIDDLE
			}
		//	기능 제거(나중에 필요할지 몰라서... 남겨둠...
		//	else if( it->second->GetFurnitureListSize() == 0 )	// 길드하우스가 비활성회 상태이고 가구 목록이 비어있는 경우는 길드하우스를 파괴한다.
		//		GuildHouseDBMng->PostRequest( GUILDHOUSE_REMOVE, NULL, 0, it->first );
		}
	}
}

BOOL CGuildHouseMng::DBLoadGuildHouse( CQuery* pQuery )
{
	if( pQuery->Execute( "usp_GuildHouse_MaxSEQ '%02d'", g_appInfo.dwSys ) )
	{
		if( pQuery->Fetch() )
			m_nSeqNum = pQuery->GetInt( "SEQ" );
	}
	else
		return FALSE;
	
	if( pQuery->Execute( "usp_GuildHouse_Load '%02d'", g_appInfo.dwSys ) )
	{
		while( pQuery->Fetch() )
		{
			DWORD dwGuildId = pQuery->GetInt( "m_idGuild" );
			DWORD dwWorldId = pQuery->GetInt( "dwWorldID" );
			time_t tKeepTime = pQuery->GetInt( "tKeepTime" );
#ifdef __GUILD_HOUSE_MIDDLE
			OBJID objGHId = pQuery->GetInt( "dwGuildHouseId" );
			int nAreaIndex = pQuery->GetInt( "nAreaIndex" );
			int nGHLevel = pQuery->GetInt( "nGuildHouseLevel" );
			char szGHName[MAX_GH_NAME] = {0, };
			::memset( szGHName, 0, sizeof( szGHName ) );
			pQuery->GetStr( "szGuildHouseName", szGHName );
#endif // __GUILD_HOUSE_MIDDLE
			
			CGuildHouseBase* pGuildHouse = MakeGuildHouse( dwGuildId, dwWorldId );
			if( pGuildHouse && AddGuildHouse( dwGuildId, pGuildHouse ) )
			{
				pGuildHouse->SetUpkeeptime( tKeepTime );
#ifdef __GUILD_HOUSE_MIDDLE
				if( pGuildHouse->GetType() >= WI_GUILDHOUSE_MIDDLE && pGuildHouse->GetType() <= WI_GUILDHOUSE_LARGE )
				{
					pGuildHouse->SetGuildHouseData( objGHId, nAreaIndex, nGHLevel, tKeepTime, szGHName );
				}
#endif // __GUILD_HOUSE_MIDDLE
			}
			else
				Error( "CGuildHouseMng::DBLoadGuildHouse() : AddGuildHouse() Failed!!! [GuildId:%06d]", dwGuildId );
		}
	}
	else
		return FALSE;

	for( MapGuildHouse::iterator it=m_mapGuildHouse.begin(); it!=m_mapGuildHouse.end(); it++ )
	{
		if( pQuery->Execute( "usp_GuildFurniture_Load  '%02d', '%06d'", g_appInfo.dwSys, it->first ) )
		{
			while( pQuery->Fetch() )
			{
				GH_Fntr_Info gfiData;
				gfiData.nSeqNum		= pQuery->GetInt( "SEQ" );
				gfiData.dwItemId	= static_cast<DWORD>( pQuery->GetInt( "ItemIndex" ) );
				gfiData.bSetup		= static_cast<BOOL>( pQuery->GetInt( "bSetup" ) );
				gfiData.vPos.x		= static_cast<float>( pQuery->GetFloat( "x_Pos" ) );
				gfiData.vPos.y		= static_cast<float>( pQuery->GetFloat( "y_Pos" ) );
				gfiData.vPos.z		= static_cast<float>( pQuery->GetFloat( "z_Pos" ) );
				gfiData.fAngle		= static_cast<float>( pQuery->GetFloat( "fAngle" ) );
				gfiData.tKeepTime	= pQuery->GetInt( "tKeepTime" );
				it->second->OnGuildHousePacket( GUILDHOUSE_PCKTTYPE_LISTUP, gfiData, NULL_ID );
			}
		}
		else
			return FALSE;
	}

#ifdef __GUILD_HOUSE_MIDDLE
	TENDER_DATA* pTenderData = GetTenderData( WI_GUILDHOUSE_MIDDLE );
	if( pTenderData != NULL )
	{
		if( pQuery->Execute( "usp_GuildHouse_MaxTender_Load '%02d'", g_appInfo.dwSys ) == TRUE )
		{
			if( pQuery->Fetch() == TRUE )
			{
				pTenderData->nTenderId = pQuery->GetInt( "nTenderId" );
				pTenderData->nTenderArea = pQuery->GetInt( "nTenderArea" );
			}
		}
		else
		{
			//return FALSE;
		}

		if( pQuery->Execute( "usp_GuildHouse_Tender_Load %d, '%02d', '%c'", pTenderData->nTenderId, g_appInfo.dwSys, 'T' ) )
		{
			while( pQuery->Fetch() )
			{
				GUILDHOUSE_TENDER GHT;
				GHT.dwGHType = WI_GUILDHOUSE_MIDDLE;
				GHT.dwGuildId = static_cast<DWORD>( pQuery->GetInt( "m_idGuild" ) );
				GHT.nAreaIndex = static_cast<int>( pQuery->GetInt( "nTenderArea" ) );
				GHT.objGHId = static_cast<OBJID>( pQuery->GetInt( "dwGuildHouseId" ) );
				GHT.nTenderPerin = static_cast<int>( pQuery->GetInt( "nTenderPerin" ) );
				GHT.nTenderPenya = static_cast<int>( pQuery->GetInt( "nTenderPenya" ) );
				GHT.tTenderTime = static_cast<time_t>( pQuery->GetInt( "tTenderTime" ) );
				GHT.chState = static_cast<char>( pQuery->GetChar( "chState" ) );

				m_mapTenderGuild.insert( MAP_TENDER_GUILD::value_type( GHT.dwGuildId, GHT ) );
			}
		}
		else
		{
			//return FALSE;
		}
	}
	else
	{
		Error( "DBLoadGuildHouse - TenderData Not Found" );
		return FALSE;
	}
#endif // __GUILD_HOUSE_MIDDLE

	return TRUE;
}
#endif // \__DBSERVER

#ifdef __GUILD_HOUSE_MIDDLE
int CGuildHouseMng::GetTenderState( DWORD dwGHType )
{
	TENDER_DATA* pTenderData = GetTenderData( dwGHType );
	if( pTenderData != NULL )
	{
		return pTenderData->nTenderState;
	}
	Error( "GetTenderState - TenderData Not Found. GHType : %d", dwGHType );
	return 0;
}

void CGuildHouseMng::SetTenderState( DWORD dwGHType, int nState )
{
	TENDER_DATA* pTenderData = GetTenderData( dwGHType );
	if( pTenderData != NULL )
	{
		pTenderData->nTenderState = nState;
#ifdef __DBSERVER
		BEFORESENDDUAL( ar, PACKETTYPE_GUILDHOUSE_TENDER_STATE, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << dwGHType << nState;
		SEND( ar, CDPTrans::GetInstance(), DPID_ALLPLAYERS );
#endif // __DBSERVER
	}
	else
	{
		Error( "CGuildHouseMng::SetTenderState - TenderData Not Found. GHType : %d, State : %d", dwGHType, nState );
	}
}

#ifdef __DBSERVER
void CGuildHouseMng::UpdateGuildHouseTender( DWORD dwGHType, CQuery* pQuery )
{
	TENDER_DATA* pTenderData = GetTenderData( dwGHType );
	if( pTenderData != NULL )
	{
		for( MAP_TENDER_GUILD::iterator it = m_mapTenderGuild.begin(); it != m_mapTenderGuild.end(); ++it )
		{
			if( it->second.dwGHType == dwGHType )
			{
				pQuery->Execute( "usp_GuildHouse_Tender_Update %d, '%02d', '%06d', '%c'",
					pTenderData->nTenderId, g_appInfo.dwSys, it->first, it->second.chState );
			}
		}
	}
}

void CGuildHouseMng::SendGuildHouseTenderInfo( DPID dpId )
{
	//for( DWORD dwGHType = WI_GUILDHOUSE_MIDDLE; dwGHType <= WI_GUILDHOUSE_LARGE; ++dwGHType )
	//{
	//}
	DWORD dwGHType = WI_GUILDHOUSE_MIDDLE;
	CGuildHouseMng::TENDER_DATA* pTenderData = GuildHouseMng->GetTenderData( dwGHType );
	if( pTenderData != NULL )
	{
		BEFORESENDDUAL( ar, PACKETTYPE_GUILDHOUSE_TENDER_INFO, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << dwGHType << pTenderData->nTenderId << pTenderData->nTenderArea << pTenderData->nTenderState;
		MAP_TENDERABLE_GH::iterator it = m_mapTenderAbleGH.find( dwGHType );
		if( it != m_mapTenderAbleGH.end() )
		{
			if( it->second.size() > 0 )
			{
				ar << TRUE << it->second.size();
				for( DWORD i = 0; i < it->second.size(); ++i )
				{
					ar << it->second[i];
				}
			}
			else
			{
				ar << FALSE;
			}
		}
		SEND( ar, CDPTrans::GetInstance(), dpId );
	}
}

bool CGuildHouseMng::CheckTenderOpenAble( DWORD dwGHType )
{
	map<OBJID, DWORD> mapGuildHouse;
	TENDER_DATA* pTenderData = GetTenderData( dwGHType );
	if( pTenderData != NULL )
	{
		for( MapGuildHouse::iterator it = m_mapGuildHouse.begin(); it != m_mapGuildHouse.end(); ++it )
		{
			if( it->second->GetType() == dwGHType )
			{
				mapGuildHouse.insert( map<OBJID, DWORD>::value_type( it->second->GetGuildHouseId(), it->first ) );
			}
		}

		int nTenderArea = pTenderData->nTenderArea;
		vector<OBJID> vecGHId;
		vecGHId.clear();
		for( int i = 1; i < GH_AREA_MAX; ++i )
		{
			int nArea = ++nTenderArea % GH_AREA_MAX;
			for( MAP_GH_DATA::iterator it = m_mapGHData.begin(); it != m_mapGHData.end(); ++it )
			{
				if( it->second.nAreaIndex == nArea )
				{
					map<OBJID, DWORD>::iterator it2 = mapGuildHouse.find( it->first );
					if( it2 == mapGuildHouse.end() )
					{
						vecGHId.push_back( it->first );
					}
				}
			}
			if( vecGHId.size() > 0 )
			{
				m_mapTenderAbleGH.insert( MAP_TENDERABLE_GH::value_type( dwGHType, vecGHId ) );
				SetTenderInfo( dwGHType, nArea );
				SendGuildHouseTenderInfo( DPID_ALLPLAYERS );
				return true;
			}
		}
	}
	else
	{
		Error( "CheckTenderOpenAble - TenderData Not Found" );
	}
	return false;
}

void CGuildHouseMng::ProcessTender( DWORD dwGHType )
{
	DWORD dwTick = GetTickCount();
	TENDER_DATA* pTenderData = GetTenderData( dwGHType );
	if( pTenderData == NULL )
	{
		Error( "ProcessTender - TenderData Not Found. GHType : %d", dwGHType );
		return;
	}

	switch( GetTenderState( dwGHType ) )
	{
	case GH_TENDER_CLOSE:
		{
			if( IsBeginNoticeTime( dwGHType ) == TRUE )
			{
				if( CheckTenderOpenAble( dwGHType ) == TRUE )
				{
					SetTenderState( dwGHType, GH_TENDER_OPEN );
				}
				else
				{
					g_dpCoreSrvr.SendNoticeMessage( "금주 중형길드하우스 입찰 없음" );
					SetTenderState( dwGHType, GH_TENDER_CANCEL );
				}
			}
			else
			{
				if( IsTenderProcess( dwGHType ) == TRUE )
				{
					if( IsEndNoticeTime( dwGHType ) == TRUE )
					{
						SetTenderState( dwGHType, GH_TENDER_READY );
					}
					else if( IsBeginTenderTime( dwGHType ) == TRUE )
					{
						SetTenderState( dwGHType, GH_TENDER_TENDER );
					}
				}
			}
		}
		break;

	case GH_TENDER_CANCEL:
		{
			if( IsEndNoticeTime( dwGHType ) == TRUE )
			{
				SetTenderState( dwGHType, GH_TENDER_CLOSE );
			}
		}
		break;

	case GH_TENDER_OPEN:
		{
			if( IsEndNoticeTime( dwGHType ) == TRUE )
			{
				SetTenderState( dwGHType, GH_TENDER_READY );
			}
			if( dwTick >= pTenderData->dwNoticeTick + pTenderData->dwPrevNoticeTick )
			{
				g_dpCoreSrvr.SendNoticeMessage( "금주 중형길드하우스 입찰 있음" );
				pTenderData->dwNoticeTick = dwTick;
			}
		}
		break;

	case GH_TENDER_READY:
		{
			if( IsBeginTenderTime( dwGHType ) == TRUE )
			{
				SetTenderState( dwGHType, GH_TENDER_TENDER );
			}
		}
		break;

	case GH_TENDER_TENDER:
		{
			if( IsEndTenderTime( dwGHType ) == TRUE )
			{
				g_dpCoreSrvr.SendNoticeMessage( "금주 중형길드하우스 입찰 마감" );
				GetHighestTenderGuild( dwGHType );
				ReturnPenyaTenderFailGuild( dwGHType );
				CloseTender( dwGHType );
			}
			if( dwTick >= pTenderData->dwNoticeTick + pTenderData->dwTenderNoticeTick )
			{
				g_dpCoreSrvr.SendNoticeMessage( "금주 중형길드하우스 입찰 진행중" );
				pTenderData->dwNoticeTick = dwTick;
			}
		}
		break;

	default:
		break;
	}
}

bool CGuildHouseMng::IsTenderProcess( DWORD dwGHType )
{
	for( MAP_TENDER_GUILD::iterator it = m_mapTenderGuild.begin(); it != m_mapTenderGuild.end(); ++it )
	{
		if( it->second.dwGHType == dwGHType && it->second.chState == 'T' )
		{
			return TRUE;
		}
	}
	return FALSE;
}

void CGuildHouseMng::GetHighestTenderGuild( DWORD dwGHType )
{
	map<DWORD, OBJID> mapHighestTenderGuild;
	DWORD	dwHighestGuildId = 0;
	__int64	nHighestPenya = 0;
	time_t	tHighestTime = 0;

	{
		for( MAP_TENDER_GUILD::iterator it = m_mapTenderGuild.begin(); it != m_mapTenderGuild.end(); ++it )
		{
			if( it->second.dwGHType == dwGHType )
			{
				it->second.chState = 'L';
			}
		}
	}

	vector<OBJID> vecGHId;
	MAP_TENDERABLE_GH::iterator it = m_mapTenderAbleGH.find( dwGHType );
	if( it != m_mapTenderAbleGH.end() )
	{
		vecGHId = it->second;
		for( DWORD i = 0; i < vecGHId.size(); ++i )
		{
			for( MAP_TENDER_GUILD::iterator it = m_mapTenderGuild.begin(); it != m_mapTenderGuild.end(); ++it )
			{
				if( vecGHId[i] == it->second.objGHId )
				{
					__int64 nTenderPenya = ( it->second.nTenderPerin * PERIN_VALUE ) + it->second.nTenderPenya;
					if( nTenderPenya > nHighestPenya )
					{
						nHighestPenya = nTenderPenya;
						dwHighestGuildId = it->first;
						tHighestTime = it->second.tTenderTime;
					}
					else if( nTenderPenya == nHighestPenya )
					{
						if( it->second.tTenderTime < tHighestTime )
						{
							nHighestPenya = nTenderPenya;
							dwHighestGuildId = it->first;
							tHighestTime = it->second.tTenderTime;
						}
					}
				}
			}
			if( dwHighestGuildId > 0 )
			{
				MAP_TENDER_GUILD::iterator it = m_mapTenderGuild.find( dwHighestGuildId );
				if( it != m_mapTenderGuild.end() )
				{
					it->second.chState = 'W';
					mapHighestTenderGuild.insert( map<DWORD, OBJID>::value_type( dwHighestGuildId, vecGHId[i] ) );
				}
			}
			dwHighestGuildId = 0;
			nHighestPenya = 0;
			tHighestTime = 0;
		}

		// tender end
		CAr ar;
		ar << dwGHType << mapHighestTenderGuild.size();
		for( map<DWORD, OBJID>::iterator it = mapHighestTenderGuild.begin(); it != mapHighestTenderGuild.end(); ++it )
		{
			ar << it->first << it->second;
		}
		int nBufSize;
		LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );
		GuildHouseDBMng->PostRequest( GUILDHOUSE_TENDER_END, lpBuf, nBufSize );
	}
	else
	{
		Error( "CGuildHouseMng::GetHighestTenderGuild - Invalid GHType : %d", dwGHType );
	}
}

void CGuildHouseMng::ReturnPenyaTenderFailGuild( DWORD dwGHType )
{
	BEFORESENDDUAL( ar, PACKETTYPE_GUILDHOUSE_TENDER_RESULT, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwGHType << m_mapTenderGuild.size();
	for( MAP_TENDER_GUILD::iterator it = m_mapTenderGuild.begin(); it != m_mapTenderGuild.end(); ++it )
	{
		ar << it->first;
		it->second.Serialize( ar );
	}
	SEND( ar, CDPTrans::GetInstance(), DPID_ALLPLAYERS );
}
#endif // __DBSERVER

#ifdef __WORLDSERVER
map<OBJID, char*> CGuildHouseMng::GetTenderAbleGuildHouse( DWORD dwGHType )
{
	map<OBJID, char*> mapGHList;
	int nAreaIndex = GetTenderArea( dwGHType );
	if( nAreaIndex > 0 )
	{
		MAP_TENDERABLE_GH::iterator it = m_mapTenderAbleGH.find( dwGHType );
		if( it != m_mapTenderAbleGH.end() )
		{
			vector<OBJID> vecGHId = it->second;
			for( DWORD i = 0; i < vecGHId.size(); ++i )
			{
				MAP_GH_DATA::iterator it = m_mapGHData.find( vecGHId[i] );
				if( it != m_mapGHData.end() )
				{
					mapGHList.insert( map<OBJID, char*>::value_type( it->first, it->second.szTitle ) );
				}
				else
				{
					Error( "Invalid GuildHouse Id : %d", vecGHId[i] );
				}
			}
		}
	}
	return mapGHList;
}

bool CGuildHouseMng::ReqTenderGuildHouseList( CUser* pUser, DWORD dwGHType, OBJID objNpcId )
{
	if( IsValidObj( pUser ) == TRUE )
	{
		if( GetTenderState( dwGHType ) != GH_TENDER_TENDER )
		{
			pUser->AddText( "입찰기간 아님" );
			return FALSE;
		}

		TENDER_DATA* pTenderData = GetTenderData( dwGHType );
		if( pTenderData != NULL )
		{
			map<OBJID, int>::iterator it = pTenderData->mapNpc.find( objNpcId );
			if( it != pTenderData->mapNpc.end() )
			{
				if( GetTenderArea( dwGHType ) == it->second )
				{
					map<OBJID, char*> mapGHList = GetTenderAbleGuildHouse( dwGHType );
					pUser->AddGuildHouseTenderMainWnd( mapGHList );
					return TRUE;
				}
				else
				{
					pUser->AddText( "입찰 가능한 마을 아님" );
				}
			}
			else
			{
				Error( "Invalid NpcId : %d", objNpcId );
			}
		}
	}
	return FALSE;
}

void CGuildHouseMng::ReqTenderGuildHouseInfo( CUser* pUser, OBJID objGHId )
{
	if( IsValidObj( pUser ) == TRUE )
	{
		GUILDHOUSE_DATA* pGHData =  GetGuildHouseData( objGHId );
		if( pGHData != NULL )
		{
			if( GetTenderState( pGHData->dwGHType ) != GH_TENDER_TENDER )
			{
				pUser->AddText( "입찰기간 아님" );
				return;
			}
			TENDER_DATA* pTenderData = GetTenderData( pGHData->dwGHType );
			if( pTenderData != NULL )
			{
				__int64 nTenderPenya = 0;
				CGuild* pGuild = g_GuildMng.GetGuild( pUser->m_idGuild );
				if( pGuild != NULL && pGuild->IsMaster( pUser->m_idPlayer ) == TRUE )
				{
					GUILDHOUSE_TENDER* pGHTender = GetGuildHouseTender( pGuild->GetGuildId() );
					if( pGHTender != NULL )
					{
						nTenderPenya = ( pGHTender->nTenderPerin * PERIN_VALUE ) + pGHTender->nTenderPenya;
					}
				}
				vector<DWORD> vecTenderGuild = GetTenderView( objGHId );
				pUser->AddGuildHouseTenderInfoWnd( objGHId, pTenderData->nMinPenya, nTenderPenya, vecTenderGuild );
			}
			else
			{
				Error( "Invalid Tender Data" );
			}
		}
		else
		{
			Error( "Invalid GuildHouse Data, GHId : %d", objGHId );
		}
	}
}

bool CGuildHouseMng::IsTenderAbleGuildHouse( DWORD dwGHType, OBJID objGHId )
{
	MAP_TENDERABLE_GH::iterator it = m_mapTenderAbleGH.find( dwGHType );
	if( it != m_mapTenderAbleGH.end() )
	{
		vector<OBJID> vecGHId = it->second;
		for( DWORD i = 0; i < vecGHId.size(); ++i )
		{
			if( vecGHId[i] == objGHId )
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

bool CGuildHouseMng::IsTenderAble( CUser* pUser, OBJID objGHId, int nTenderPerin, int nTenderPenya )
{
	if( IsValidObj( pUser ) == TRUE )
	{
		GUILDHOUSE_DATA* pGHData = GetGuildHouseData( objGHId );
		if( pGHData == NULL )
		{
			Error( "IsTenderAble - Invalid Guildhouse Data. GHId : %d", objGHId );
			return FALSE;
		}

		TENDER_DATA* pTenderData = GetTenderData( pGHData->dwGHType );
		if( pTenderData == NULL )
		{
			Error( "IsTenderAble - Invalid Tender Data. GHType : %d", pGHData->dwGHType );
			return FALSE;
		}

		if( GetTenderState( pGHData->dwGHType ) != GH_TENDER_TENDER )
		{
			pUser->AddText( "입찰기간 아님" );
			return FALSE;
		}

		CGuild* pGuild = g_GuildMng.GetGuild( pUser->m_idGuild );
		if( pGuild == NULL || pGuild->IsMaster( pUser->m_idPlayer ) == FALSE )
		{
			pUser->AddText( "길드가 없거나 길마가 아님" );
			return FALSE;
		}

		if( GetGuildHouseTender( pGuild->m_idGuild ) != NULL )
		{
			pUser->AddText( "이미 입찰했음" );
			return FALSE;
		}

		CGuildHouseBase* pGuildHouse = GetGuildHouse( pGuild->m_idGuild );
		if( pGuildHouse != NULL )
		{
			if( pGuildHouse->GetType() >= pGHData->dwGHType )
			{
				pUser->AddText( "이미 동급이상 길드하우스 소유" );
				return FALSE;
			}
			if( pGuildHouse->GetUpkeepTime() <= 0 )
			{
				pUser->AddText( "소유한 길드하우스의 유지기간 종료" );
				return FALSE;
			}
		}
		else
		{
			pUser->AddText( "소유한 길드하우스가 없음" );
			return FALSE;
		}

		if( IsTenderAbleGuildHouse( pGHData->dwGHType, objGHId ) == FALSE )
		{
			pUser->AddText( "입찰가능한 길드하우스가 아님" );
			Error( "IsTenderAble - Invalid GHId. User : %07d, GHId : %d", pUser->m_idPlayer, objGHId );
			return FALSE;
		}

		if( nTenderPerin > 0 )
		{
			CItemElem* pItemElem = (CItemElem*)pUser->m_Inventory.GetAtItemId( II_SYS_SYS_SCR_PERIN );
			if( IsUsableItem( pItemElem ) == TRUE )
			{
				if( pItemElem->m_nItemNum < nTenderPerin )
				{
					Error( "Perin Num Different" );
					return FALSE;
				}
			}
			else
			{
				Error( "Not Perin" );
				return FALSE;
			}
		}
		if( pUser->GetGold() < nTenderPenya )
		{
			Error( "Lack Penya" );
			return FALSE;
		}
		__int64 nTotalPenya = ( PERIN_VALUE * nTenderPerin ) + nTenderPenya;
		if( nTotalPenya < pTenderData->nMinPenya )
		{
			pUser->AddText( "최소 입찰 금액보다 적음" );
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

void CGuildHouseMng::OnGuildHouseTenderJoin( CUser* pUser, OBJID objGHId, int nTenderPerin, int nTenderPenya )
{
	if( IsValidObj( pUser ) == TRUE )
	{
		if( IsTenderAble( pUser, objGHId, nTenderPerin, nTenderPenya ) == FALSE )
		{
			return;
		}

		if( nTenderPerin > 0 )
		{
			CItemElem* pItemElem = (CItemElem*)pUser->m_Inventory.GetAtItemId( II_SYS_SYS_SCR_PERIN );
			if( IsUsableItem( pItemElem ) == TRUE && pItemElem->m_nItemNum >= nTenderPerin  )
			{
				LogItemInfo aLogItem;
				aLogItem.SendName = pUser->GetName();
				aLogItem.RecvName = "GUILDHOUSE_TENDER_PERIN";
				aLogItem.WorldId = pUser->GetWorld()->GetID();
				aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
				aLogItem.Action = "G";
				g_DPSrvr.OnLogItem( aLogItem, pItemElem );
				pUser->RemoveItem( (BYTE)( pItemElem->m_dwObjId ), (short)nTenderPerin );
			}
			else
			{
				Error( "OnGuildHouseTenderJoin - Not Perin or Lack Perin. UserId : %07d", pUser->m_idPlayer );
				return;
			}
		}
		if( pUser->GetGold() >= nTenderPenya )
		{
			LogItemInfo aLogItem;
			aLogItem.SendName = pUser->GetName();
			aLogItem.RecvName = "GUILDHOUSE_TENDER_PENYA";
			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = pUser->GetGold();
			pUser->AddGold( -nTenderPenya );
			aLogItem.Gold2 = pUser->GetGold();
			aLogItem.Action = "G";
			_stprintf( aLogItem.szItemName, "%d", II_GOLD_SEED1 );
			g_DPSrvr.OnLogItem( aLogItem );
		}
		else
		{
			Error( "OnGuildHouseTenderJoin - Lack Tender Penya. UserId : %07d", pUser->m_idPlayer );
			return;
		}

		BEFORESENDDUAL( ar, PACKETTYPE_GUILDHOUSE_TENDER_JOIN, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << pUser->m_idPlayer;
		GUILDHOUSE_TENDER GHT( objGHId, pUser->m_idGuild, nTenderPerin, nTenderPenya );
		GHT.Serialize( ar );
		SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );
	}
}

CGuildHouseMng::GUILDHOUSE_TENDER* CGuildHouseMng::GetGuildHouseTender( DWORD dwGuildId )
{
	map<DWORD, GUILDHOUSE_TENDER>::iterator it = m_mapTenderGuild.find( dwGuildId );
	if( it != m_mapTenderGuild.end() )
	{
		return &(it->second);
	}
	return NULL;
}

vector<DWORD> CGuildHouseMng::GetTenderView( OBJID objGHId )
{
	vector<DWORD> vecGuildId;
	for( MAP_TENDER_GUILD::iterator it = m_mapTenderGuild.begin(); it != m_mapTenderGuild.end(); ++it )
	{
		if( it->second.objGHId == objGHId )
		{
			vecGuildId.push_back( it->first );
		}
	}
	return vecGuildId;
}

void CGuildHouseMng::OnGuildHouseTenderInfo( CAr & ar )
{
	DWORD dwGHType;
	int nTenderId, nTenderArea, nTenderState;
	ar >> dwGHType >> nTenderId >> nTenderArea >> nTenderState;

	TENDER_DATA* pTenderData = GetTenderData( dwGHType );
	if( pTenderData != NULL )
	{
		pTenderData->nTenderId = nTenderId;
		pTenderData->nTenderArea = nTenderArea;
		pTenderData->nTenderState = nTenderState;
	}
	else
	{
		Error( "OnGuildHouseTenderInfo - TenderData Not Found. GHType : %d", dwGHType );
	}

	BOOL bTender;
	ar >> bTender;
	if( bTender == TRUE )
	{
		vector<OBJID> vecGHId;
		int nSize;
		ar >> nSize;
		for( int i = 0; i < nSize; ++i )
		{
			OBJID objGHId;
			ar >> objGHId;
			vecGHId.push_back( objGHId );
		}
		m_mapTenderAbleGH.insert( MAP_TENDERABLE_GH::value_type( dwGHType, vecGHId ) );
	}
}

void CGuildHouseMng::OnGuildHouseTenderJoin( CAr & ar )
{
	DWORD dwPlayerId;
	OBJID objGHId;
	BOOL bTender;

	ar >> dwPlayerId >> objGHId >> bTender;
	if( bTender == TRUE )
	{
		GUILDHOUSE_TENDER GHT;
		GHT.Serialize( ar );

		if( AddGuildHouseTender( GHT.dwGuildId, GHT ) == TRUE )
		{
			CUser* pUser = static_cast<CUser*>( prj.GetUserByID( dwPlayerId ) );
			if( IsValidObj( pUser ) == TRUE )
			{
				pUser->AddGuildHouseTenderResult( objGHId, TRUE );
				pUser->AddText( "입찰 성공!" );
			}
		}
		else
		{
			Error( "OnSetGuildHouseTender - AddGuildHouseTender Failed. [User : %07d, Guild : %06d, GHId : %d]", dwPlayerId, GHT.dwGuildId, objGHId ); 
		}
	}
	else
	{
		CUser* pUser = static_cast<CUser*>( prj.GetUserByID( dwPlayerId ) );
		if( IsValidObj( pUser ) == TRUE )
		{
			pUser->AddGuildHouseTenderResult( objGHId, FALSE );
			pUser->AddText( "입찰 실패!" );
		}
	}
}

void CGuildHouseMng::OnGuildHouseTenderState( CAr & ar )
{
	DWORD dwGHType;
    int nState;
	ar >> dwGHType >> nState;

	TENDER_DATA* pTenderData = GetTenderData( dwGHType );
	if( pTenderData != NULL )
	{
		pTenderData->nTenderState = nState;
	}
	else
	{
		Error( "OnGuildHouseTenderState - TenderData Not Found. GHType : %d", dwGHType );
	}
}

void CGuildHouseMng::OnGuildHouseTenderResult( CAr & ar )
{
	DWORD dwGHType;
	int nSize;
	ar >> dwGHType >> nSize;
	for( int i = 0; i < nSize; ++i )
	{
		DWORD dwGuildId;
		ar >> dwGuildId;
		MAP_TENDER_GUILD::iterator it = m_mapTenderGuild.find( dwGuildId );
		if( it != m_mapTenderGuild.end() )
		{
			it->second.Serialize( ar );
		}
	}

	if( g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) == TRUE )
	{
		TENDER_DATA* pTenderData = GetTenderData( dwGHType );
		if( pTenderData == NULL )
		{
			Error( "CGuildHouseMng::OnGuildHouseTenderResult - TenderData Not Found. GHType : %d", dwGHType );
			return;
		}

		CGuild* pGuild = NULL;
		for( MAP_TENDER_GUILD::iterator it = m_mapTenderGuild.begin(); it != m_mapTenderGuild.end(); ++it )
		{
			if( it->second.dwGHType == dwGHType )
			{
				if( it->second.chState == 'W' )
				{
					pGuild = g_GuildMng.GetGuild( it->first );
					if( pGuild != NULL )
					{
						// send post
						char szTitle[MAX_MAILTITLE] = {0, };
						char szSub[MAX_MAILTEXT] = {0, };
						_stprintf( szTitle, "길드하우스 입찰 성공" );
						//_stprintf( szTitle, prj.GetText(  ) );
						//_stprintf( szSub, prj.GetText(  ) );
						CItemElem itemElem;
						//::memset( &itemElem, 0, sizeof( itemElem ) );
						g_dpDBClient.SendQueryPostMail( pGuild->m_idMaster, 0, itemElem, 0, szTitle, szSub );
					}

				}
				else if( it->second.chState == 'L' )
				{
					pGuild = g_GuildMng.GetGuild( it->first );
					if( pGuild != NULL )
					{
						// send post
						int nChargeRate = pTenderData->nChargeRate;
						__int64 nTenderPenya = ( it->second.nTenderPerin * PERIN_VALUE ) + it->second.nTenderPenya;
						__int64 nReturnPenya = nTenderPenya - ( (nTenderPenya * pTenderData->nChargeRate) / 100 );
						int nPerinNum = (int)( nReturnPenya / PERIN_VALUE );
						if( nPerinNum > it->second.nTenderPerin )
						{
							nPerinNum = it->second.nTenderPerin;
						}
						int nPenya = (int)( nReturnPenya - ( nPerinNum * PERIN_VALUE ) );
						if( nPenya < 0 )
						{
							Error( "Invalid Tender Penya - GuildId : %d, nTenderPerin : %d, nTenderPenya : %d", it->first, nPerinNum, nPenya );
							nPenya = 0;
						}

						char szTitle[MAX_MAILTITLE] = {0, };
						char szSub[MAX_MAILTEXT] = {0, };
						_stprintf( szTitle, "길드하우스 입찰 실패" );
						//_stprintf( szTitle, prj.GetText(  ) );
						//_stprintf( szSub, prj.GetText(  ) );
						CItemElem itemElem;
						if( nPerinNum > 0 )
						{
							itemElem.m_dwItemId = II_SYS_SYS_SCR_PERIN;
							itemElem.m_nItemNum = nPerinNum;
							itemElem.SetSerialNumber();
							g_dpDBClient.SendQueryPostMail( pGuild->m_idMaster, 0, itemElem, nPenya, szTitle, szSub );
						}
						else
						{
							g_dpDBClient.SendQueryPostMail( pGuild->m_idMaster, 0, itemElem, nPenya, szTitle, szSub );
						}
					}
				}
			}
		}
	}
}

void CGuildHouseMng::OnGuildHouseGradeUpdate( CAr & ar )
{
	DWORD dwGuildId, dwWorldId;
	ar >> dwGuildId >> dwWorldId;

	CGuildHouseBase* pGuildHouse = ReMakeGuildHouse( dwGuildId, dwWorldId );
	if( pGuildHouse != NULL )
	{
		pGuildHouse->SerializeAllInfo( ar );

		CGuild* pGuild = g_GuildMng.GetGuild( dwGuildId );
		if( pGuild != NULL )
		{
			for( map<u_long, CGuildMember*>::iterator it=pGuild->m_mapPMember.begin(); it!=pGuild->m_mapPMember.end(); it++ )
			{
				CUser* pUser = static_cast<CUser*>( prj.GetUserByID( it->first ) );
				if( IsValidObj( pUser ) == TRUE )
				{
					pUser->AddGuildHouseAllInfo( pGuildHouse );
					pUser->AddDefinedText( TID_GAME_GUILDHOUSE_PURCHASE );
				}
			}
		}
	}
	else
	{
		Error( "CGuildHouseMng::OnGuildHouseGradeUpdate - ReMakeGuildHouse Failed. Guild : [%06d], World : [%d]", dwGuildId, dwWorldId ); 
		CGuildHouseSmall temp( NULL_ID );
		temp.SerializeAllInfo( ar );
	}
}

void CGuildHouseMng::OnGuildHouseLevelUpdate( CAr & ar )
{
	DWORD dwPlayerId;
	BOOL bLevelUp;
	ar >> dwPlayerId >> bLevelUp;

	if( bLevelUp == TRUE )
	{
		DWORD dwGuildId;
		int nLevel;
		ar >> dwGuildId >> nLevel;

		CGuildHouseBase* pGuildHouse = GetGuildHouse( dwGuildId );
		if( pGuildHouse != NULL )
		{
			pGuildHouse->SetGuildHouseLevel( nLevel );
			CUser* pUser = static_cast<CUser*>( prj.GetUserByID( dwPlayerId ) );
			if( IsValidObj( pUser ) == TRUE )
			{
				pUser->AddText( "길드하우스 레벨업 성공" );
				pUser->AddGuildHouseAllInfo( pGuildHouse );
			}
		}
	}
	else
	{
		CUser* pUser = static_cast<CUser*>( prj.GetUserByID( dwPlayerId ) );
		if( IsValidObj( pUser ) == TRUE )
		{
			pUser->AddText( "길드하우스 레벨업 실패" );
		}
	}
}

void CGuildHouseMng::OnGuildHouseExpired( CAr & ar )
{
	DWORD dwGuildId, dwWorldId;
	ar >> dwGuildId >> dwWorldId;

	CGuildHouseBase* pGuildHouse = GetGuildHouse( dwGuildId );
	if( pGuildHouse != NULL )
	{
		if( dwWorldId == WI_GUILDHOUSE_SMALL )
		{
			pGuildHouse->ExpiredGuildHouse();
		}
		else if( dwWorldId >= WI_GUILDHOUSE_MIDDLE && dwWorldId <= WI_GUILDHOUSE_LARGE )
		{
			// remake
			pGuildHouse = ReMakeGuildHouse( dwGuildId, dwWorldId );
			if( pGuildHouse != NULL )
			{
				pGuildHouse->SerializeAllInfo( ar );
			}
		}
	}
}

CGuildHouseBase* CGuildHouseMng::ReMakeGuildHouse( DWORD dwGuildId, DWORD dwWorldId )
{
	if( RemoveGuildHouse( dwGuildId ) == TRUE )
	{
		CGuildHouseBase* pGuildHouse = MakeGuildHouse( dwGuildId, dwWorldId );
		if( pGuildHouse != NULL && AddGuildHouse( dwGuildId, pGuildHouse ) == TRUE )
		{
			return pGuildHouse;
		}
		else
		{
			Error( "CGuildHouseMng::ReMakeGuildHouse - MakeGuildHouse Failed. Guild : [%06d], World : [%d]", dwGuildId, dwWorldId );
		}
	}
	else
	{
		Error( "CGuildHouseMng::ReMakeGuildHouse - RemoveGuildHouse Failed. Guild : [%06d], World : [%d]", dwGuildId, dwWorldId );
	}
	return NULL;
}

CGuildHouseMng::GUILDHOUSE_QUEST* CGuildHouseMng::GetGuildHouseQuestData( int nQuestId )
{
	MAP_GH_QUEST::iterator it = m_mapGHQuest.find( nQuestId );
	if( it != m_mapGHQuest.end() )
	{
		return &(it->second);
	}
	return NULL;
}

void CGuildHouseMng::CheckGuildHouseQuest( CUser* pUser, int nQuestId )
{
	if( IsValidObj( pUser ) == TRUE )
	{
		GUILDHOUSE_QUEST* pGHQuest = GetGuildHouseQuestData( nQuestId );
		if( pGHQuest != NULL )
		{
			CGuild* pGuild = g_GuildMng.GetGuild( pUser->m_idGuild );
			if( pGuild != NULL )
			{
				CGuildHouseBase* pGuildHouse = GetGuildHouse( pGuild->GetGuildId() );
				if( pGuildHouse != NULL )
				{
					if( pGuildHouse->GetGuildHouseLevel() < pGHQuest->nLevel )
					{
						// send trans
						BEFORESENDDUAL( ar, PACKETTYPE_GUILDHOUSE_LEVEL_UPDATE, DPID_UNKNOWN, DPID_UNKNOWN );
						ar << pGuild->GetGuildId() << pGHQuest->nLevel;
						SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );
					}
					else
					{
						pUser->AddText( "해당 길드하우스 레벨과 동급이상" );
					}
				}
			}
		}
	}
}

#endif // __WORLDSERVER

void CGuildHouseMng::LoadScript()
{
	if( m_Lua.RunScript( "GuildHouseTender.lua" ) != 0 )
	{
		Error( "GuildHouseTender.lua Load Failed!!!" );
		exit(0);
	}

	m_Lua.GetGloabal( "tTender" );
	m_Lua.PushNil();
	while( m_Lua.TableLoop( -2 ) )
	{
		DWORD dwGHType				= static_cast<DWORD>( CScript::GetDefineNum( m_Lua.GetFieldToString( -1, "strGHType" ) ) );
		TENDER_DATA TenderData;
		TenderData.nMaxHouse		= static_cast<int>( m_Lua.GetFieldToNumber( -1, "nMaxHouse" ) );
		TenderData.nMinPenya		= static_cast<int>( m_Lua.GetFieldToNumber( -1, "nMinPenya" ) );
		TenderData.nChargeRate		= static_cast<int>( m_Lua.GetFieldToNumber( -1, "nChargeRate" ) );
		TenderData.dwPrevNoticeTick = static_cast<DWORD>( m_Lua.GetFieldToNumber( -1, "nPrevNoticeTick" ) );
		TenderData.dwTenderNoticeTick = static_cast<DWORD>( m_Lua.GetFieldToNumber( -1, "nTenderNoticeTick" ) );
		MAP_TENDER_DATA::iterator it = m_mapTenderData.insert( MAP_TENDER_DATA::value_type( dwGHType, TenderData ) ).first;

		m_Lua.GetField( -1, "tNpc" );
		m_Lua.PushNil();
		while( m_Lua.TableLoop( -2 ) )
		{
			OBJID objNpcId	= static_cast<OBJID>( CScript::GetDefineNum( m_Lua.GetFieldToString( -1, "strNpcId" ) ) );
			int nAreaIndex	= static_cast<int>( m_Lua.GetFieldToNumber( -1, "nAreaIndex" ) );
			if( prj.GetMoverProp( objNpcId ) == NULL )
			{
				Error( "Invalid GuildHouse NpcId : %d", objNpcId );
			}
			it->second.mapNpc.insert( map<OBJID, int>::value_type( objNpcId, nAreaIndex ) );

			m_Lua.Pop( 1 );
		}
		m_Lua.Pop( 1 );

		m_Lua.Pop( 1 );
	}

	m_Lua.GetGloabal( "tGHInfo" );
	m_Lua.PushNil();
	while( m_Lua.TableLoop( -2 ) )
	{
		GUILDHOUSE_DATA GHData;
		GHData.objGHId	= static_cast<OBJID>( CScript::GetDefineNum( m_Lua.GetFieldToString( -1, "strNpcId" ) ) );
		GHData.dwGHType	= static_cast<DWORD>( CScript::GetDefineNum( m_Lua.GetFieldToString( -1, "strGHType" ) ) );
		GHData.nAreaIndex	= static_cast<int>( m_Lua.GetFieldToNumber( -1, "nAreaIndex" ) );
		_tcscpy( GHData.szTitle, m_Lua.GetFieldToString( -1, "strTitle" ) );
		m_mapGHData.insert( MAP_GH_DATA::value_type( GHData.objGHId, GHData ) );
		m_Lua.Pop( 1 );
	}
#ifdef __WORLDSERVER
	m_Lua.GetGloabal( "tGHQuest" );
	m_Lua.PushNil();
	while( m_Lua.TableLoop( -2 ) )
	{
		int nQuestId	= static_cast<OBJID>( CScript::GetDefineNum( m_Lua.GetFieldToString( -1, "strQuestId" ) ) );
		GUILDHOUSE_QUEST GHQuest;
		GHQuest.dwGHType	= static_cast<DWORD>( m_Lua.GetFieldToNumber( -1, "dwGHType" ) );
		GHQuest.nLevel	= static_cast<int>( m_Lua.GetFieldToNumber( -1, "nLevel" ) );
		m_mapGHQuest.insert( MAP_GH_QUEST::value_type( nQuestId, GHQuest ) );
		m_Lua.Pop( 1 );
	}
#endif // __WORLDSERVER

	m_Lua.Pop( 0 );
}

void CGuildHouseMng::SetTenderInfo( DWORD dwGHType, int nTenderArea )
{
	TENDER_DATA* pTenderData = GetTenderData( dwGHType );
	if( pTenderData != NULL )
	{
		++pTenderData->nTenderId;
		pTenderData->nTenderArea = nTenderArea;
	}
	else
	{
		Error( "SetTenderInfo - TenderData Not Found. GHType : %d", dwGHType );
	}
}

int CGuildHouseMng::GetTenderArea( DWORD dwGHType )
{
	TENDER_DATA* pTenderData = GetTenderData( dwGHType );
	if( pTenderData != NULL )
	{
		return pTenderData->nTenderArea;
	}
	return 0;
}

int CGuildHouseMng::GetTenderId( DWORD dwGHType )
{
	TENDER_DATA* pTenderData = GetTenderData( dwGHType );
	if( pTenderData != NULL )
	{
		return pTenderData->nTenderId;
	}
	return 0;
}

CGuildHouseMng::TENDER_DATA* CGuildHouseMng::GetTenderData( DWORD dwGHType )
{
	MAP_TENDER_DATA::iterator it = m_mapTenderData.find( dwGHType );
	if( it != m_mapTenderData.end() )
	{
		return &(it->second);
	}
	return NULL;
}

CGuildHouseMng::GUILDHOUSE_DATA* CGuildHouseMng::GetGuildHouseData( OBJID objGHId )
{
	MAP_GH_DATA::iterator it = m_mapGHData.find( objGHId );
	if( it != m_mapGHData.end() )
	{
		return &(it->second);
	}
	return NULL;
}

BOOL CGuildHouseMng::IsBeginNoticeTime( DWORD dwGHType )
{
	BOOL bIsNoticeTime = FALSE;

	if( m_Lua.GetLuaFunction( "IsBeginNoticeTime" ) )
	{
		//m_Lua.PushNumber( dwGHType );
		//m_Lua.CallLuaFunction( 1, 1 );
		m_Lua.CallLuaFunction( 0, 1 );
		bIsNoticeTime = static_cast<BOOL>( m_Lua.ToBool( -1 ) );
	}
	m_Lua.Pop( 0 );

	return bIsNoticeTime;
}

BOOL CGuildHouseMng::IsEndNoticeTime( DWORD dwGHType )
{
	BOOL bIsNoticeTime = FALSE;

	if( m_Lua.GetLuaFunction( "IsEndNoticeTime" ) )
	{
		//m_Lua.PushNumber( dwGHType );
		//m_Lua.CallLuaFunction( 1, 1 );
		m_Lua.CallLuaFunction( 0, 1 );
		bIsNoticeTime = static_cast<BOOL>( m_Lua.ToBool( -1 ) );
	}
	m_Lua.Pop( 0 );

	return bIsNoticeTime;
}

BOOL CGuildHouseMng::IsBeginTenderTime( DWORD dwGHType )
{
	BOOL bIsTenderTime = FALSE;

	if( m_Lua.GetLuaFunction( "IsBeginTenderTime" ) )
	{
		//m_Lua.PushNumber( dwGHType );
		//m_Lua.CallLuaFunction( 1, 1 );
		m_Lua.CallLuaFunction( 0, 1 );
		bIsTenderTime = static_cast<BOOL>( m_Lua.ToBool( -1 ) );
	}
	m_Lua.Pop( 0 );

	return bIsTenderTime;
}

BOOL CGuildHouseMng::IsEndTenderTime( DWORD dwGHType )
{
	BOOL bIsTenderTime = FALSE;

	if( m_Lua.GetLuaFunction( "IsEndTenderTime" ) )
	{
		//m_Lua.PushNumber( dwGHType );
		//m_Lua.CallLuaFunction( 1, 1 );
		m_Lua.CallLuaFunction( 0, 1 );
		bIsTenderTime = static_cast<BOOL>( m_Lua.ToBool( -1 ) );
	}
	m_Lua.Pop( 0 );

	return bIsTenderTime;
}

void CGuildHouseMng::CloseTender( DWORD dwGHType )
{
	SetTenderState( dwGHType, GH_TENDER_CLOSE );
	m_mapTenderAbleGH.erase( dwGHType );
	for( MAP_TENDER_GUILD::iterator it = m_mapTenderGuild.begin(); it != m_mapTenderGuild.end(); )
	{
		if( it->second.dwGHType == dwGHType )
		{
			m_mapTenderGuild.erase( it++ );
		}
		else
		{
			++it;
		}
	}
}

#endif // __GUILD_HOUSE_MIDDLE

#endif // n__CLIENT

#endif // __GUILD_HOUSE