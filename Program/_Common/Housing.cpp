#include "stdafx.h"
#include "resData.h"
#include "defineText.h"

#if __VER >= 13 // __HOUSING
#include ".\housing.h"

#ifdef __WORLDSERVER
#include "DPDatabaseClient.h"
extern CDPDatabaseClient g_dpDBClient;
#include "User.h"
extern CUserMng g_UserMng;
#include "worldmng.h"
extern CWorldMng g_WorldMng;
#include "npchecker.h"
#endif // __WORLDSERVER

#ifdef __DBSERVER
#include "HousingDBCtrl.h"
#endif // __DBSERVER

#ifdef __CLIENT
#if __VER >= 15 // __GUILD_HOUSE
#include "WndHousing.h"
#include "GuildHouse.h"
#endif
#endif

/////////////////////////////////////////////////////////////////////
//// CHousing ///////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
CHousing::CHousing( DWORD dwPlayerId ) :
#ifdef __WORLDSERVER
m_bSetting( FALSE ),
#endif // __WORLDSERVER
m_dwMasterId( dwPlayerId )
{
}

CHousing::~CHousing(void)
{
#ifdef __WORLDSERVER
	// 방에 있는 놈들을 다 쫓아낸 후 레이어를 파괴한다.
	CWorld* pWorld = g_WorldMng.GetWorld( WI_WORLD_MINIROOM );
	if( pWorld )
		pWorld->Invalidate( static_cast<int>( m_dwMasterId ) );

	for( DWORD i=0; i<m_vecIdVisitAllow.size(); i++ )
		CHousingMng::GetInstance()->SetRemoveVisitable( m_dwMasterId, m_vecIdVisitAllow[i] );
#endif // __WORLDSERVER
	m_vecHousingInfo.clear();
	m_vecIdVisitAllow.clear();
}

#ifdef __CLIENT
CHousing* CHousing::GetInstance()
{
	static CHousing sHousing( g_pPlayer->m_idPlayer );
	return & sHousing;
}

void CHousing::GetHousingList( vector<HOUSINGINFO> & vHousingList )
{
	vHousingList.assign( m_vecHousingInfo.begin(), m_vecHousingInfo.end() );
}

void CHousing::GetVisitAllow( vector<DWORD> & vVisitAllow )
{
	vVisitAllow.assign( m_vecIdVisitAllow.begin(), m_vecIdVisitAllow.end() );
}
#endif // __CLIENT

void CHousing::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_vecHousingInfo.size();	// 하우징 가구 정보..
		for( DWORD i=0; i<m_vecHousingInfo.size(); i++ )
			m_vecHousingInfo[i].Serialize( ar );
		ar << m_vecIdVisitAllow.size(); // 방문 허용 목록...
		for( DWORD i=0; i<m_vecIdVisitAllow.size(); i++ )
			ar << m_vecIdVisitAllow[i];
	}
	else
	{
		m_vecHousingInfo.clear();
		int nSize = 0;
		ar >> nSize;
		for( int i=0; i<nSize; i++ )
		{
			HOUSINGINFO housingInfo;
			housingInfo.Serialize( ar );
			m_vecHousingInfo.push_back( housingInfo );
		}
#ifdef __WORLDSERVER
		CUser* pUser = static_cast<CUser*>( prj.GetUserByID( m_dwMasterId ) );
#endif // __WORLDSERVER
		m_vecIdVisitAllow.clear();
		ar >> nSize;
		for( i=0; i<nSize; i++ )
		{
			DWORD dwPlayerId;
			ar >> dwPlayerId;
			m_vecIdVisitAllow.push_back( dwPlayerId );
#ifdef __WORLDSERVER
			CHousingMng::GetInstance()->SetAddVisitable( m_dwMasterId, dwPlayerId );
			if( IsValidObj( pUser ) && !pUser->m_RTMessenger.GetFriend( dwPlayerId ) )
			{
				BEFORESENDDUAL( ar, PACKETTYPE_HOUSING_SETVISITALLOW, DPID_UNKNOWN, DPID_UNKNOWN );
				ar << pUser->m_idPlayer << dwPlayerId << FALSE;
				SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );
			}
#endif // __WORLDSERVER
		}
	}
}

int CHousing::GetIndexFromList( DWORD dwItemId )
{
	for( int i=0; i<(int)( m_vecHousingInfo.size() ) ; i++ )
	{
		if( dwItemId == m_vecHousingInfo[i].dwItemId )
			return i;
	}

	return NULL_ID;
}

void CHousing::SetFurnitureList( HOUSINGINFO& housingInfo, BOOL bAdd )
{
	if( bAdd )	// 리스트에 추가하는 거...
		m_vecHousingInfo.push_back( housingInfo );
	else
		RemoveFurnitureList( housingInfo.dwItemId );

#ifdef __WORLDSERVER
	Setting( FALSE );
	CUser* pUser = static_cast<CUser*>( prj.GetUserByID( m_dwMasterId ) );
	if( IsValidObj( pUser ) )
		pUser->AddHousingSetFurnitureList( housingInfo, bAdd );
#endif // __WORLDSERVER
}

BOOL CHousing::RemoveFurnitureList( DWORD dwItemId )
{
	int nIndex = GetIndexFromList( dwItemId );
	if( nIndex != NULL_ID )
	{
#ifdef __WORLDSERVER
		if( m_vecHousingInfo[nIndex].bSetup )
			RemoveFurnitureControl( nIndex );
#endif // __WORLDSERVER
		vector<HOUSINGINFO>::iterator it = m_vecHousingInfo.begin();
		it += nIndex;
		m_vecHousingInfo.erase( it );
		return TRUE;
	}
		
	return FALSE;
}

void CHousing::SetupFurniture( HOUSINGINFO housingInfo )
{
	int nIndex = GetIndexFromList( housingInfo.dwItemId );
	if( nIndex != NULL_ID )
	{
		m_vecHousingInfo[nIndex].bSetup = housingInfo.bSetup;
		if( housingInfo.bSetup )
		{
			m_vecHousingInfo[nIndex].vPos = housingInfo.vPos;
			m_vecHousingInfo[nIndex].fAngle = housingInfo.fAngle;
		}
#ifdef __WORLDSERVER
		Setting( FALSE );
		CUser* pUser = static_cast<CUser*>( prj.GetUserByID( m_dwMasterId ) );
		if( IsValidObj( pUser ) )
		{
			pUser->AddHousingSetupFurniture( &m_vecHousingInfo[nIndex] );
			if( m_vecHousingInfo[nIndex].bSetup )
				AddFurnitureControl( nIndex );
			else
				RemoveFurnitureControl( nIndex );
		}
#endif // __WORLDSERVER
	}
}

void CHousing::SetVisitAllow( DWORD dwTargetId, BOOL bAllow )
{
	if( bAllow )
		m_vecIdVisitAllow.push_back( dwTargetId );
	else
	{
		for( vector<DWORD>::iterator it=m_vecIdVisitAllow.begin(); it!=m_vecIdVisitAllow.end(); it++ )
		{
			if( (*it) == dwTargetId )
			{
				m_vecIdVisitAllow.erase( it );
				break;
			}
		}
	}
#ifdef __WORLDSERVER
	Setting( FALSE );
	CUser* pUser = static_cast<CUser*>( prj.GetUserByID( m_dwMasterId ) );
	if( IsValidObj( pUser ) )
		pUser->AddHousingSetVisitAllow( dwTargetId, bAllow );

	if( bAllow )
		CHousingMng::GetInstance()->SetAddVisitable( m_dwMasterId, dwTargetId );
	else
	{
		CHousingMng::GetInstance()->SetRemoveVisitable( m_dwMasterId, dwTargetId );
		pUser = static_cast<CUser*>( prj.GetUserByID( dwTargetId ) );
		if( IsValidObj( pUser ) && pUser->GetWorld()
			&& pUser->GetWorld()->GetID() == WI_WORLD_MINIROOM && pUser->GetLayer() == m_dwMasterId )
			CHousingMng::GetInstance()->GoOut( pUser );
	}
#endif // __WORLDSERVER
}

#ifdef __WORLDSERVER
BOOL CHousing::IsListUpAble( CUser* pUser, DWORD dwItemId )
{
	if( m_bSetting )
		return FALSE;

	if( m_vecHousingInfo.size() >= 99 )
	{
		pUser->AddDefinedText( TID_GAME_HOUSING_OVERMAX );
		return FALSE;
	}

	for( DWORD i=0; i<m_vecHousingInfo.size(); i++ )
	{
		if( m_vecHousingInfo[i].dwItemId == dwItemId )
		{
			pUser->AddDefinedText( TID_GAME_HOUSING_ADDITION_FAIL );
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CHousing::IsSetupAble( CUser* pUser, HOUSINGINFO housingInfo )
{
	if( m_bSetting )
		return FALSE;

	// 플레이어가 방에 있어야 하고 자신의 레이어에 들어가 있는 경우만 가능...
	CWorld* pWorld = pUser->GetWorld();
	if( !pWorld || ( pWorld->GetID() != WI_WORLD_MINIROOM ) 
		|| ( pUser->GetLayer() != static_cast<int>( pUser->m_idPlayer ) ) )
		return FALSE;
	
	// 해당 가구가 목록에 있고 이미 설치되어 있는지 확인...
	int nIndex = GetIndexFromList( housingInfo.dwItemId );
	if( nIndex == NULL_ID )
		return FALSE;
	// 동일한 아이템은 설치할 수 없다.( 이 경우는 발생하지 않음.. )
	if( m_vecHousingInfo[nIndex].bSetup == housingInfo.bSetup )
		return FALSE;
	
	ItemProp* pItemPropReq = prj.GetItemProp( housingInfo.dwItemId );
	if( !pItemPropReq )
		return FALSE;

	// 같은 종류의 가구가 몇개 설치되어 있는지 확인...
	int nCount = 0;	
	for( DWORD i=0; i<m_vecHousingInfo.size(); i++ )
	{
		if( housingInfo.bSetup && m_vecHousingInfo[i].bSetup )
		{
			ItemProp* pItemProp = prj.GetItemProp( m_vecHousingInfo[i].dwItemId );
			if( pItemProp && pItemProp->dwItemKind3 == pItemPropReq->dwItemKind3 )
				nCount++;
		}
	}
	
	// 최대 몇개 설치 가능한가?
	switch( pItemPropReq->dwItemKind3 )
	{
		case IK3_PROPS :	// 소품은 최대 2
			if( nCount >= 2 )
			{
				pUser->AddDefinedText( TID_GAME_HOUSING_INSTALL_FAIL02 );
				return FALSE;	
			}

		default :	// 그외는 1개만 설치 가능
			if( nCount >= 1 )
			{
				pUser->AddDefinedText( TID_GAME_HOUSING_INSTALL_FAIL02 );
				return FALSE;	
			}
	}

	return TRUE;
}

BOOL CHousing::IsAllowVisit( DWORD dwPlayerId )
{
	// 자기 방이면 입장 가능
	if( m_dwMasterId == dwPlayerId )
		return TRUE;

	// 허용 목록에 있으면 입장 가능
	for( DWORD i=0; i<m_vecIdVisitAllow.size(); i++ )
	{
		if( m_vecIdVisitAllow[i] == dwPlayerId )
			return TRUE;
	}

	return FALSE;
}

BOOL CHousing::AddFurnitureControl( int nIndex )
{
	ItemProp* pItemProp = prj.GetItemProp( m_vecHousingInfo[nIndex].dwItemId );
	CWorld* pWorld = g_WorldMng.GetWorld( WI_WORLD_MINIROOM );

	if( pWorld && pItemProp )
	{
		// 레이어가 존재하는지 확인하고 가구 컨트롤 생성...
		if( pWorld->m_linkMap.GetLinkMap( static_cast<int>( m_dwMasterId ) ) )
		{
			if( pItemProp->dwItemKind2 == IK2_PAPERING )	// 벽지, 장판(컨트롤을 생성하지 않고 정보를 클라에 전달)
			{
				g_UserMng.AddHousingPaperingInfo( pItemProp->dwID, TRUE, m_dwMasterId );
			}
			else
			{
				CCtrl* pCtrl	= (CCtrl*)CreateObj( D3DDEVICE, OT_CTRL, pItemProp->dwLinkKind );
				if( !pCtrl )
					return FALSE;

				pCtrl->SetPos( m_vecHousingInfo[nIndex].vPos );
				pCtrl->SetAngle( m_vecHousingInfo[nIndex].fAngle );
				pCtrl->AddItToGlobalId();
				m_vecHousingInfo[nIndex].objId = pCtrl->GetId();
				pWorld->ADDOBJ( pCtrl, FALSE, static_cast<int>( m_dwMasterId ) );
			}
		}
		else
			return FALSE;
		
		SetBuff( m_vecHousingInfo[nIndex].dwItemId, TRUE );
	}
	else
		return FALSE;
	
	return TRUE;
}

void CHousing::RemoveFurnitureControl( int nIndex )
{
	ItemProp* pItemProp = prj.GetItemProp( m_vecHousingInfo[nIndex].dwItemId );
	CWorld* pWorld = g_WorldMng.GetWorld( WI_WORLD_MINIROOM );

	if( pWorld && pItemProp )
	{
		if( pWorld->m_linkMap.GetLinkMap( static_cast<int>( m_dwMasterId ) ) )
		{
			if( pItemProp->dwItemKind2 == IK2_PAPERING )	// 벽지, 장판(컨트롤을 생성하지 않고 정보를 클라에 전달)
			{
				g_UserMng.AddHousingPaperingInfo( pItemProp->dwID, FALSE, m_dwMasterId );
			}
			else
			{
				CCtrl* pCtrl = prj.GetCtrl( m_vecHousingInfo[nIndex].objId );
				if( pCtrl )
				{
					pCtrl->Delete();
					m_vecHousingInfo[nIndex].objId = NULL_ID;
				}
			}
		}
		
		SetBuff( m_vecHousingInfo[nIndex].dwItemId, FALSE );
	}

}

void CHousing::AddAllFurnitureControl()
{
	for( DWORD i=0; i<m_vecHousingInfo.size(); i++ )
	{
		if( m_vecHousingInfo[i].bSetup )
			AddFurnitureControl( i );
	}
}

void CHousing::SetBuff( DWORD dwItemId, BOOL bSet )
{
	CUser* pUser = static_cast<CUser*>( prj.GetUserByID( m_dwMasterId ) );
	ItemProp* pItemProp = prj.GetItemProp( dwItemId );
	if( IsValidObj( pUser ) && pItemProp )
	{
		if( bSet )
			pUser->ApplySkill( pUser, pItemProp, NULL );
		else
			pUser->RemoveBuff( BUFF_ITEM, (WORD)( dwItemId ) );
	}
}

vector<DWORD> CHousing::GetAllPaperingInfo()
{
	vector<DWORD> vecTemp;
	for( DWORD i=0; i<m_vecHousingInfo.size(); i++ )
	{
		ItemProp* pItemProp = prj.GetItemProp( m_vecHousingInfo[i].dwItemId );
		if( pItemProp && m_vecHousingInfo[i].bSetup && pItemProp->dwItemKind2 == IK2_PAPERING )
			vecTemp.push_back( m_vecHousingInfo[i].dwItemId );
	}

	return vecTemp;
}
#endif // __WORLDSERVER

#ifdef __DBSERVER
void CHousing::ProcessExpiredFurniture( time_t time, BOOL bGMRemove )
{
	vector<HOUSINGINFO> vecTemp;
	for( DWORD i=0; i<m_vecHousingInfo.size(); i++ )
	{
		if( time >= m_vecHousingInfo[i].tKeepTime || bGMRemove )
			vecTemp.push_back( m_vecHousingInfo[i] );
	}

	for( DWORD i=0; i<vecTemp.size(); i++ )
		CHousingDBMng::GetInstance()->DeleteFurnitureList( m_dwMasterId, vecTemp[i] );
}
#endif // __DBSERVER

#ifndef __CLIENT
/////////////////////////////////////////////////////////////////////
//// CHousingMng ////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

CHousingMng::CHousingMng(void)
{
}

CHousingMng::~CHousingMng(void)
{
	Clear();
}

void CHousingMng::Clear()
{
	for( MAP_HP::iterator it=m_mapHousing.begin(); it!=m_mapHousing.end(); it++ )
		SAFE_DELETE( it->second );
	m_mapHousing.clear();
#ifdef __WORLDSERVER
	m_mapVisitable.clear();
#endif // __WORLDSERVER
}

CHousingMng* CHousingMng::GetInstance()
{
	static CHousingMng sHousingMng;
	return & sHousingMng;
}

#ifdef __WORLDSERVER
void CHousingMng::ReqLoadHousingInfo( DWORD dwPlayerId )
{
	BEFORESENDDUAL( ar, PACKETTYPE_HOUSING_LOADINFO, DPID_UNKNOWN, DPID_UNKNOWN );
	ar << dwPlayerId;
	SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );
}

BOOL CHousingMng::ReqSetFurnitureList( CUser* pUser, DWORD dwItemId )
{
	CHousing* pHousing = GetHousing( pUser->m_idPlayer );
	if( pHousing && pHousing->IsListUpAble( pUser, dwItemId ) )
	{
		pHousing->Setting( TRUE );
		BEFORESENDDUAL( ar, PACKETTYPE_HOUSING_FURNITURELIST, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << pUser->m_idPlayer << dwItemId;
		SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );
	}
	else
		return FALSE;

	return TRUE;
}

BOOL CHousingMng::ReqSetupFurniture( CUser* pUser, HOUSINGINFO housingInfo )
{
	CHousing* pHousing = GetHousing( pUser->m_idPlayer );
	if( pHousing && pHousing->IsSetupAble( pUser, housingInfo ) )
	{
		pHousing->Setting( TRUE );
		BEFORESENDDUAL( ar, PACKETTYPE_HOUSING_SETUPFURNITURE, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << pUser->m_idPlayer;
		housingInfo.Serialize( ar );
		SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );
	}
	else
		return FALSE;

	return TRUE;
}

BOOL CHousingMng::ReqSetAllowVisit( CUser* pUser, DWORD dwPlayerId, BOOL bAllow )
{
	ASSERT( pUser );
	CHousing* pHousing = GetHousing( pUser->m_idPlayer );
	if( pUser->m_RTMessenger.GetFriend( dwPlayerId ) && pHousing && ( pHousing->IsAllowVisit( dwPlayerId ) != bAllow ) )
	{
		pHousing->Setting( TRUE );
		BEFORESENDDUAL( ar, PACKETTYPE_HOUSING_SETVISITALLOW, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << pUser->m_idPlayer << dwPlayerId << bAllow;
		SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );
	}
	else
		return FALSE;

	return TRUE;
}

void CHousingMng::ReqGMFunrnitureListAll( CUser* pUser )
{
	CHousing* pHousing = GetHousing( pUser->m_idPlayer );
	if( pHousing )
	{
		BEFORESENDDUAL( ar, PACKETTYPE_HOUSING_GM_REMOVEALL, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << pUser->m_idPlayer;
		SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );
	}
}

void CHousingMng::CreateRoomLayer( DWORD dwPlayerId )
{
	CWorld* pWorld = g_WorldMng.GetWorld( WI_WORLD_MINIROOM );
	if( pWorld && !pWorld->m_linkMap.GetLinkMap( static_cast<int>( dwPlayerId ) ) )
		pWorld->CreateLayer( static_cast<int>( dwPlayerId ) );
}

BOOL CHousingMng::DestroyHousing( DWORD dwPlayerId )
{
	MAP_HP::iterator it = m_mapHousing.find( dwPlayerId );
	if( it != m_mapHousing.end() )
	{
		SAFE_DELETE( it->second );
		m_mapHousing.erase( it );
		return TRUE;
	}
	Error( "CHousingMng::DestroyHousing() - Housing not found : %d", dwPlayerId );
	return FALSE;
}

void CHousingMng::SetVisitRoom( CUser* pUser, DWORD dwPlayerId )
{
	CWorld* pWorld = g_WorldMng.GetWorld( WI_WORLD_MINIROOM );
	ASSERT( pWorld );
	CHousing* pHousing = GetHousing( dwPlayerId );
	if( IsValidObj( pUser ) && pHousing )
	{
		// GM이거나 방문 허용된 사람만 입장 가능...
		if( !pUser->IsAuthHigher( AUTH_GAMEMASTER ) )	// GM이 아니라면...
		{
			if( !pHousing->IsAllowVisit( pUser->m_idPlayer )	// 방문 허용 검사...
				|| !CNpcChecker::GetInstance()->IsCloseNpc( MMI_VISIT_FRIEND, pUser->GetWorld(), pUser->GetPos() ) ) // NPC 근처에 있는지 검사..
				return;
		}
		// 레이어가 존재하는지 확인하고 입장...
		if( pWorld->m_linkMap.GetLinkMap( static_cast<int>( dwPlayerId ) ) )
		{
			if( pUser->REPLACE( g_uIdofMulti, WI_WORLD_MINIROOM, D3DXVECTOR3( 253.0f, 105.0f, 243.0f ), REPLACE_NORMAL, static_cast<int>( dwPlayerId ) ) )
			{
				pUser->SetAngle( 175.0f );
				pUser->AddDefinedCaption( FALSE, TID_GAME_HOUSING_ENTER, "\"%s\"", CPlayerDataCenter::GetInstance()->GetPlayerString( dwPlayerId ) );
			}
		}
	}
}

void CHousingMng::SetAddVisitable( DWORD dwPlayerId, DWORD dwTargetId )
{
	CUser* pUser = static_cast<CUser*>( prj.GetUserByID( dwPlayerId ) );
	if( !IsValidObj( pUser ) || !pUser->m_RTMessenger.GetFriend( dwTargetId ) )
		return;
	MAP_VSTABLE::iterator it = m_mapVisitable.find( dwTargetId );
	if( it != m_mapVisitable.end() )
		it->second.push_back( dwPlayerId );
	else
	{
		vector<DWORD> vecTemp;
		vecTemp.push_back( dwPlayerId );
		m_mapVisitable.insert( MAP_VSTABLE::value_type( dwTargetId, vecTemp ) );
	}
}

void CHousingMng::SetRemoveVisitable( DWORD dwPlayerId, DWORD dwTargetId )
{
	MAP_VSTABLE::iterator it = m_mapVisitable.find( dwTargetId );
	if( it != m_mapVisitable.end() )
	{
		for( vector<DWORD>::iterator it2=it->second.begin(); it2!=it->second.end(); it2++ )
		{
			if( (*it2) == dwPlayerId )
			{
				it->second.erase( it2 );
				if( it->second.size() == 0 )
					m_mapVisitable.erase( it );
				break;
			}
		}
	}
}

void CHousingMng::OnReqVisitableList( CUser* pUser )
{
	MAP_VSTABLE::iterator it = m_mapVisitable.find( pUser->m_idPlayer );
	if( it != m_mapVisitable.end() )
		pUser->AddHousingVisitableList( it->second );
}

void CHousingMng::GoOut( CUser* pUser )
{
	pUser->REPLACE( g_uIdofMulti, WI_WORLD_MADRIGAL, D3DXVECTOR3( 6985.0f, 0.0f, 3234.0f ), REPLACE_FORCE, nDefaultLayer );
	pUser->SetAngle( 50.0f );
}
#endif // __WORLDSERVER

void CHousingMng::SetFurnitureList( DWORD dwPlayerId, HOUSINGINFO& housingInfo, BOOL bAdd )
{
	CHousing* pHousing = GetHousing( dwPlayerId );
	if( pHousing )
		pHousing->SetFurnitureList( housingInfo, bAdd );
}

void CHousingMng::SetupFurniture( DWORD dwPlayerId, HOUSINGINFO housingInfo )
{
	CHousing* pHousing = GetHousing( dwPlayerId );
	if( pHousing )
		pHousing->SetupFurniture( housingInfo );
}

void CHousingMng::SetVisitAllow( DWORD dwPlayerId, DWORD dwTargetId, BOOL bAllow )
{
	CHousing* pHousing = GetHousing( dwPlayerId );
	if( pHousing )
		pHousing->SetVisitAllow( dwTargetId, bAllow );
}

CHousing* CHousingMng::CreateHousing( DWORD dwPlayerId )
{
	pair<MAP_HP::iterator, bool> p	= m_mapHousing.insert( MAP_HP::value_type( dwPlayerId, new CHousing( dwPlayerId ) ) );
	if( !p.second )
	{
		Error( "CHousingMng::CreateHousing() - Housing Create Failed!!" );
		return NULL;
	}
	
	return p.first->second;
}

CHousing* CHousingMng::GetHousing( DWORD dwPlayerId )
{
	MAP_HP::iterator it = m_mapHousing.find( dwPlayerId );
	if( it == m_mapHousing.end() )
		return NULL;

	return it->second;
}
#endif // n__CLIENT

#ifdef __DBSERVER
void CHousingMng::ProcessRemoveExpiredFurniture()
{
	time_t time = time_null();

	for( MAP_HP::iterator it=m_mapHousing.begin(); it!=m_mapHousing.end(); it++ )
		it->second->ProcessExpiredFurniture( time );
}
#endif // __DBSERVER

#ifdef __CLIENT

// CDeployManager
CDeployManager::CDeployManager(void)
{
	if( g_Neuz.m_pd3dDevice )	//gmpbigsun(20100421) : 사운드 초기화등이 실패등으로 메시지가 발생할때  CNeuzApp에서 이놈의 생성자를 호출하고있다. 아직 디바이스는 노생성 
	{
		m_pRed		  = ((CTexture*)CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_MODELTEX, _T("red.tga")), 0xffff00ff ))->m_pTexture;
		m_pOriginWall = ((CTexture*)CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_MODELTEX, _T("Obj_MiniWall01.dds")), 0xffff00ff ))->m_pTexture;
		m_pOriginTile = ((CTexture*)CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_WORLDTEX, _T("Miniroom_floor01.dds")), 0xffff00ff ))->m_pTexture;
	}

	m_fAngle		= 0.0f;
	m_pTargetObj	= NULL;
	m_pWallObj		= NULL;
	m_bIsColl		= FALSE;
	m_bIsMyRoom		= FALSE;
	m_nNumTex		= 0;
	m_vecOriginal.clear();
}

CDeployManager::~CDeployManager(void)
{
	SAFE_DELETE(m_pTargetObj);
}

CDeployManager* CDeployManager::GetInstance()
{
	static CDeployManager DeployManager;
	return &DeployManager;
}

BOOL	CDeployManager::LoadToDeploy(int nItemId, HOUSINGINFO housingInfo)
{
	CWorld* pWorld	= g_WorldMng.Get();

	if(m_pTargetObj) 
	{
		pWorld->DeleteObj(m_pTargetObj);
		m_pTargetObj = NULL;
	}

	m_ItemInfo.bSetup    = TRUE;
	m_ItemInfo.dwItemId  = housingInfo.dwItemId;
	m_ItemInfo.fAngle    = housingInfo.fAngle;
	m_ItemInfo.tKeepTime = housingInfo.tKeepTime;
	m_ItemInfo.vPos      = housingInfo.vPos;

	m_pTargetObj = CreateObj( g_Neuz.m_pd3dDevice,OT_CTRL, nItemId);
	
	if(m_pTargetObj)
	{	
		m_pWallObj = pWorld->GetObjByName("obj_miniwall01.o3d");
		m_nBlendFactor = 80;
		CWorld* pWorld	= g_WorldMng.Get();
		pWorld->AddObj(m_pTargetObj, FALSE);
		m_pTargetObj->SetWorld( pWorld);
		m_pTargetObj->SetAngle(0.0f);
		m_pTargetObj->ResetScale();
		m_pTargetObj->UpdateBoundBox();

		m_nNumTex = ((CModelObject*)m_pTargetObj->m_pModel)->GetObject3D()->GetMaxObject();
		for(int i = 0; i < m_nNumTex; ++i)
		{
			LPDIRECT3DTEXTURE9 pTexture = ((CModelObject*)m_pTargetObj->m_pModel)->GetObject3D()->GetGMOBJECT(i)->m_pMtrlBlkTexture[0];
			int nMrt = ((CModelObject*)m_pTargetObj->m_pModel)->GetObject3D()->GetGMOBJECT(i)->m_nMaxMtrlBlk;
			LPDIRECT3DTEXTURE9	*pMtrlBlkTexture = ((CModelObject*)m_pTargetObj->m_pModel)->GetObject3D()->GetGMOBJECT(i)->m_pMtrlBlkTexture;
			if(pTexture)
			m_vecOriginal.push_back(pTexture);
		}
		return TRUE;
	}

	return FALSE;

}

HOUSINGINFO*	CDeployManager::EndDeploy()
{
	if(m_pTargetObj) 
	{
		m_ItemInfo.fAngle = m_pTargetObj->GetAngle();
		m_ItemInfo.vPos = m_pTargetObj->GetPos();
		CWorld* pWorld	= g_WorldMng.Get();
		m_pTargetObj->Delete();
		m_pTargetObj = NULL;
		m_pWallObj	 = NULL;
	}

	Init();

	return &m_ItemInfo;
}

BOOL	CDeployManager::IsMyRoom()
{
	return m_bIsMyRoom;
}

BOOL	CDeployManager::SetMyRoom(BOOL bFlag)
{
	m_bIsMyRoom = bFlag;

	return m_bIsMyRoom;
}

BOOL	CDeployManager::IsReady()
{
	if(m_pTargetObj)	return TRUE;
	else			return FALSE;
}

BOOL	CDeployManager::ChangeObjMode(int nType)
{
	if(m_pTargetObj)
	{
		switch(nType)
		{
			case RED_MODE:
				{
					m_nBlendFactor = 110;
					for(int i = 0; i < m_nNumTex; ++i)
						((CModelObject*)m_pTargetObj->m_pModel)->GetObject3D()->GetGMOBJECT(i)->m_pMtrlBlkTexture[0] = m_pRed;
				}
				break;
			case NORMAL_MODE:
				{
					m_nBlendFactor = 80;
					for(int i = 0; i < m_nNumTex; ++i)
					{
						vector<LPDIRECT3DTEXTURE9>::iterator	iter = 	m_vecOriginal.begin();
						if(*iter && iter != m_vecOriginal.end())
							((CModelObject*)m_pTargetObj->m_pModel)->GetObject3D()->GetGMOBJECT(i)->m_pMtrlBlkTexture[0] = *iter;
						iter++;
					}
				}
				break;

		}

		return TRUE;
	}

	return FALSE;
}

void	CDeployManager::Init()
{
	m_pTargetObj	= NULL;
	m_pWallObj		= NULL;
	m_bIsColl		= FALSE;
	m_nNumTex		= 0;
	m_nBlendFactor  = 80;
	m_fAngle		= 0.0f;
	m_vecOriginal.clear();
}

void	CDeployManager::Process()
{
	if(m_pTargetObj)
	{
		D3DXVECTOR3 vPos, vOut;
		CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
		CRect rect = pWndWorld->GetClientRect();
		CWorld* pWorld	= g_WorldMng.Get();
		pWorld->ClientPointToVector( NULL, rect, pWndWorld->GetMousePoint(), &pWorld->m_matProj, &pWorld->m_pCamera->m_matView, &vPos, FALSE );
		m_pTargetObj->SetPos(vPos);
		m_pTargetObj->UpdateLocalMatrix();
		m_pTargetObj->m_wBlendFactor = m_nBlendFactor;
		if(CheckCollision())
		{
			if(!m_bIsColl) ChangeObjMode(RED_MODE);
			m_bIsColl = TRUE;
		}
		else
		{
			if(m_bIsColl) ChangeObjMode(NORMAL_MODE);
			m_bIsColl = FALSE;
		}
	}
}

BOOL	CDeployManager::IsCollide()
{
	return m_bIsColl;
}

BOOL	CDeployManager::CheckCollision()
{
	 /*
	 이 충돌검출방식은 마이룸의 특성에 따라서 최소한의 계산만 하도록 작성된 것이므로
	 다른 오브젝트에는 사용이 부적절하다
	 */
	if(m_pWallObj && m_pTargetObj)
	{
		CWorld* pWorld	= g_WorldMng.Get();
		return pWorld->ProcessObjCollision(m_pTargetObj->GetPos(), m_pTargetObj, m_pWallObj);
	}

	return FALSE;
}

BOOL	CDeployManager::ChangeWallTex(TCHAR* pTexName)
{
	CWorld* pWorld	= g_WorldMng.Get();
	CObj*	pObj	= NULL;

	// 미니룸 벽에 해당하는 특정 오브젝트를 이름으로 찾아서 바꿈
	if(pWorld)
	{
		pObj = pWorld->GetObjByName("obj_miniwall01.o3d"); //->> 확정오브젝트 나오면 이름 바꿔서 너줘야 됨

		if(pObj)
		{
			if(pTexName)
				((CModelObject*)pObj->m_pModel)->GetObject3D()->SetTexture(((CTexture*)CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_MODELTEX, _T(pTexName)), 0xffff00ff ))->m_pTexture);
			else
				((CModelObject*)pObj->m_pModel)->GetObject3D()->SetTexture(m_pOriginWall);
			return TRUE;
		}
	}

	return FALSE;
	
}

BOOL	CDeployManager::ChangeTileTex(TCHAR* pTexName)
{
	// 주의 : 맵내의 모든지형 텍스쳐가 요놈으로 싹바뀌니 마이룸에서만 호출할 것!
	CWorld* pWorld	= g_WorldMng.Get();
	
	if(pWorld)
	{
		if(pTexName)
			pWorld->ForceTexture(((CTexture*)CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_MODELTEX, _T(pTexName)), 0xffff00ff ))->m_pTexture);
		else
			pWorld->ForceTexture(m_pOriginTile);
		return TRUE;
	}

	return FALSE;
}

#endif //__CLIENT

#endif // __VER >= 13 // __HOUSING

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CGuildDeployManager
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if __VER >= 15 // __GUILD_HOUSE

#ifdef __CLIENT

CGuildDeployManager::CGuildDeployManager(void)
{
	m_pRed = NULL;
	m_pOriginWall = NULL;
	m_pOriginTile = NULL;
	
	m_fAngle		= 0.0f;
	m_pTargetObj	= NULL;
	m_pWallObj		= NULL;
	m_bIsColl		= FALSE;
	m_bIsManagement	= FALSE;
	m_nNumTex		= 0;
	m_vecOriginal.clear();

	m_pItem = new HOUSING_ITEM;
	m_pClonedModel = NULL;
}

CGuildDeployManager::~CGuildDeployManager(void)
{
	if( m_pTargetObj )
		m_pTargetObj->Delete();

	m_pTargetObj = NULL;
	
	SAFE_DELETE( m_pItem );

	m_pOriginTile = NULL;
	m_pOriginWall = NULL;
//	SAFE_DELETE(m_pClonedModel);
}

CGuildDeployManager* CGuildDeployManager::GetInstance()
{
	static CGuildDeployManager sGuildDeployManager;
	return &sGuildDeployManager;
}

BOOL	CGuildDeployManager::LoadToDeploy(int nItemId, const HOUSING_ITEM& housingInfo)
{
	CWorld* pWorld	= g_WorldMng.Get();
	if( !pWorld )
		return FALSE;

	//um...
	std::string strHouseObjName = GetNameHouseObj( );
	m_pWallObj = pWorld->GetObjByName( (TCHAR*)strHouseObjName.c_str() );

	if( !m_pWallObj )
	{
		AfxMessageBox( "cannot find house object" );
		return FALSE;
	}

	if(m_pTargetObj) 
	{
		pWorld->DeleteObj(m_pTargetObj);
		m_pTargetObj = NULL;
	}

	*m_pItem = housingInfo;
	m_pItem->m_bDeploy = TRUE;

	m_pTargetObj = CreateObj( g_Neuz.m_pd3dDevice,OT_CTRL, nItemId, TRUE );
	if( !m_pTargetObj )
	{
		assert( 0 && "CreateObj failed" );
		return FALSE;
	}

// gmpbigun:  clone model!!
 	LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem( OT_CTRL, nItemId );
 
//	SAFE_DELETE( m_pClonedModel );
 	m_pClonedModel = new CModelObject;
 	m_pClonedModel->SetModelType( OT_CTRL ); 
 	m_pClonedModel->m_pModelElem = lpModelElem;
 	HRESULT hr = m_pClonedModel->InitDeviceObjects( D3DDEVICE );
 	TCHAR szFileName[ MAX_PATH ];
 	prj.m_modelMng.MakeModelName( szFileName, OT_CTRL, nItemId );
	m_pClonedModel->LoadClonedElement( szFileName );
	m_pTargetObj->m_pModel = m_pClonedModel;

	if(m_pTargetObj)
	{	
		m_nBlendFactor = 80;
		CWorld* pWorld	= g_WorldMng.Get();
		pWorld->AddObj(m_pTargetObj, FALSE);
		m_pTargetObj->SetWorld( pWorld);
		m_pTargetObj->SetAngle(m_pItem->m_fAngle);
		m_pTargetObj->ResetScale();
		m_pTargetObj->UpdateBoundBox();

		m_nNumTex = ((CModelObject*)m_pTargetObj->m_pModel)->GetObject3D()->GetMaxObject();
		for(int i = 0; i < m_nNumTex; ++i)
		{
			LPDIRECT3DTEXTURE9 pTexture = ((CModelObject*)m_pTargetObj->m_pModel)->GetObject3D()->GetGMOBJECT(i)->m_pMtrlBlkTexture[0];
			int nMrt = ((CModelObject*)m_pTargetObj->m_pModel)->GetObject3D()->GetGMOBJECT(i)->m_nMaxMtrlBlk;
			LPDIRECT3DTEXTURE9	*pMtrlBlkTexture = ((CModelObject*)m_pTargetObj->m_pModel)->GetObject3D()->GetGMOBJECT(i)->m_pMtrlBlkTexture;
			if(pTexture)
			m_vecOriginal.push_back(pTexture);
		}

		return TRUE;
	}

	return FALSE;

}

HOUSING_ITEM*	CGuildDeployManager::EndDeploy()
{
	if(m_pTargetObj) 
	{
		m_pItem->m_fAngle = m_pTargetObj->GetAngle();
		m_pItem->m_vPos = m_pTargetObj->GetPos();
		CWorld* pWorld	= g_WorldMng.Get();
		pWorld->DeleteObj( m_pTargetObj );
		m_pTargetObj = NULL;
		m_pWallObj	 = NULL;
	}

	Init();

	return m_pItem;
}


BOOL	CGuildDeployManager::IsReady()
{
	if(m_pTargetObj)	return TRUE;
	else				return FALSE;
}

BOOL	CGuildDeployManager::ChangeObjMode(int nType)
{
	if( !m_pRed )
		m_pRed		  = ((CTexture*)CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_MODELTEX, _T("red.tga")), 0xffff00ff ))->m_pTexture;

	if(m_pTargetObj)
	{
		switch(nType)
		{
			case RED_MODE:
				{
					m_nBlendFactor = 110;
					for(int i = 0; i < m_nNumTex; ++i)
						((CModelObject*)m_pTargetObj->m_pModel)->GetObject3D()->GetGMOBJECT(i)->m_pMtrlBlkTexture[0] = m_pRed;
				}
				break;
			case NORMAL_MODE:
				{
					m_nBlendFactor = 80;
					for(int i = 0; i < m_nNumTex; ++i)
					{
						vector<LPDIRECT3DTEXTURE9>::iterator	iter = 	m_vecOriginal.begin();
						if(*iter && iter != m_vecOriginal.end())
							((CModelObject*)m_pTargetObj->m_pModel)->GetObject3D()->GetGMOBJECT(i)->m_pMtrlBlkTexture[0] = *iter;
						iter++;
					}
				}
				break;
		}

		return TRUE;
	}

	return FALSE;
}

void	CGuildDeployManager::Init()
{
	if( m_pTargetObj )
		m_pTargetObj->Delete();

	m_pTargetObj	= NULL;
	m_pWallObj		= NULL;
	m_bIsColl		= FALSE;
	m_nNumTex		= 0;
	m_nBlendFactor  = 80;
//	m_fAngle		= 0.0f;
	m_vecOriginal.clear();

//	SAFE_DELETE( m_pClonedModel );
//	m_pClonedModel = NULL;

	m_pOriginWall = NULL;
	m_pOriginTile = NULL;
}

void	CGuildDeployManager::Process()
{
	if(m_pTargetObj)
	{
		D3DXVECTOR3 vPos;
		CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
		CRect rect = pWndWorld->GetClientRect();
		CWorld* pWorld	= g_WorldMng.Get();
		pWorld->ClientPointToVector( NULL, rect, pWndWorld->GetMousePoint(), &pWorld->m_matProj, &pWorld->m_pCamera->m_matView, &vPos, FALSE );
		m_pTargetObj->SetPos(vPos);
		m_pTargetObj->UpdateLocalMatrix();
		m_pTargetObj->m_wBlendFactor = m_nBlendFactor;
		if(CheckCollision())
		{
			if(!m_bIsColl) 
				ChangeObjMode(RED_MODE);

			m_bIsColl = TRUE;
		}
		else
		{
			if(m_bIsColl) 
				ChangeObjMode(NORMAL_MODE);
			
			m_bIsColl = FALSE;
		}
	}
}


BOOL	CGuildDeployManager::CheckCollision()
{
	//재설치의 경우 같은 아이디의 오브젝트는 충돌검출에서 제외한다.
	if(m_pWallObj && m_pTargetObj)
	{
		CWorld* pWorld	= g_WorldMng.Get();

		// 못가는 지역 검사 
		// gmpbigsun : 중형일경우 설치하려는 오브젝트의 각 바운딩 박스 꼭지점검사까지 해줘야 한다.( 통짜이기 때문에 )
		// if( 중형이면 )
		//   8개의 꼭지점 검사 
		D3DXVECTOR3 vPos = m_pTargetObj->GetPos( );
		int nAttr = pWorld->GetHeightAttribute( vPos.x, vPos.z );
		if( HATTR_NOWALK == nAttr || HATTR_NOMOVE == nAttr || HATTR_NOFLY == nAttr )		
			return TRUE;

		return pWorld->ProcessObjCollision(m_pTargetObj->GetPos(), m_pTargetObj, m_pWallObj);
	}

	return FALSE;
}

BOOL	CGuildDeployManager::ChangeWallTex(TCHAR* pTexName)
{
	CWorld* pWorld	= g_WorldMng.Get();
	CObj*	pObj	= NULL;

	if( !m_pOriginWall )
	{
		std::string filename = GetNameHouseWallTex( );
		CTexture* pCTex = (CTexture*)CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_MODELTEX, (LPCTSTR)filename.c_str() ), 0xffff00ff );
		m_pOriginWall = pCTex->m_pTexture;
		assert( m_pOriginWall );
	}

	// 미니룸 벽에 해당하는 특정 오브젝트를 이름으로 찾아서 바꿈
	if(pWorld)
	{
		std::string strNameHouseObj = GetNameHouseObj( );
		pObj = pWorld->GetObjByName( (TCHAR*)strNameHouseObj.c_str() ); //->> 확정오브젝트 나오면 이름 바꿔서 너줘야 됨

		if(pObj)
		{
			if(pTexName)
				((CModelObject*)pObj->m_pModel)->GetObject3D()->SetTexture(((CTexture*)CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_MODELTEX, _T(pTexName)), 0xffff00ff ))->m_pTexture);
			else
				((CModelObject*)pObj->m_pModel)->GetObject3D()->SetTexture(m_pOriginWall);
			return TRUE;
		}
	}

	return FALSE;
	
}

BOOL	CGuildDeployManager::ChangeTileTex(TCHAR* pTexName)
{
	if( !m_pOriginTile )
	{
		std::string filename = GetNameHouseTileTex( );
		CTexture* pCTex = (CTexture*)CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_WORLDTEX, (LPCTSTR)filename.c_str() ), 0xffff00ff );
		m_pOriginTile = pCTex->m_pTexture;

		assert( m_pOriginTile );
	}

	CWorld* pWorld	= g_WorldMng.Get();
	
	if(pWorld)
	{
		if(pTexName)
		{
			IDirect3DTexture9* pApplyTex = ((CTexture*)CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_MODELTEX, _T(pTexName)), 0xffff00ff ))->m_pTexture;
			pWorld->ForceTexture( pApplyTex );
		}
		else
			pWorld->ForceTexture(m_pOriginTile);
		return TRUE;
	}

	return FALSE;
}

std::string CGuildDeployManager::GetNameHouseObj( )
{
	std::string str;

	DWORD dwGHouseType = GuildHouse->GetType( );

	switch( dwGHouseType )
	{
	case WI_GUILDHOUSE_SMALL: str = "obj_miniwall01.o3d";	break;
	case WI_GUILDHOUSE_MIDDLE: str = "obj_miniwall02.o3d";	break;
	}

	return str;
}

std::string CGuildDeployManager::GetNameHouseWallTex( )
{
	std::string str;

	DWORD dwGHouseType = GuildHouse->GetType( );

	switch( dwGHouseType )
	{
	case WI_GUILDHOUSE_SMALL: str = "Obj_MiniWall01.dds";	break;
	case WI_GUILDHOUSE_MIDDLE: str = "Obj_MiniWall02.dds";  break;
	}

	if( str.empty() )
		str = "Obj_MiniWall01.dds";

	return str;
}

std::string CGuildDeployManager::GetNameHouseTileTex( )
{
	std::string str;

	DWORD dwGHouseType = GuildHouse->GetType( );

	switch( dwGHouseType )
	{
	case WI_GUILDHOUSE_SMALL: str = "Miniroom_floor01.dds";	break;
	case WI_GUILDHOUSE_MIDDLE: str = "Miniroom_floor02.dds";  break;
	}

	if( str.empty() )
		str = "Miniroom_floor01.dds";

	return str;
}

BOOL CGuildDeployManager::MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	BOOL bProcessed = FALSE;

	switch( msg )
	{
	case WM_KEYDOWN:
		{
			if( g_WndMng.m_pLogOutWaitting ) // 종료중이면 키보드 입력 불가
				break;
		
			int nVirtKey = (int) wParam;    // virtual-key code

			if(nVirtKey == VK_ESCAPE)
			{
				if( IsReady() )
				{
					EndDeploy();
					GuildHouse->m_iMode = 0;
					bProcessed = TRUE;
				}
			}
		}
		break;

	case WM_KEYUP:
		{
			if( !IsReady() )
				break;

			int nVirtKey = (int) wParam;    // virtual-key code

		//	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetApplet( APP_WORLD );
		//	if( pWndWorld && pWndWorld->IsFocusWnd() == FALSE )
		//		pWndWorld->OnKeyUp( nVirtKey, 0, 0 );

			if(g_bKeyTable[VK_NEXT])
			{
				m_fAngle += 45.0f;
				if( m_fAngle > 359.0f) 
					m_fAngle = 0.0f;

				m_pTargetObj->SetAngle(m_fAngle);
				m_pTargetObj->UpdateLocalMatrix();
				bProcessed = TRUE;
			}
			else if(g_bKeyTable[VK_PRIOR])
			{
				m_fAngle -= 45.0f;
				if( m_fAngle < -359.0f ) 
					m_fAngle = 0.0f;

				m_pTargetObj->SetAngle( m_fAngle );
				m_pTargetObj->UpdateLocalMatrix();

				bProcessed = TRUE;
			}

			g_bKeyTable[ nVirtKey ] = FALSE;
			g_WndMng.SetMessengerAutoState();
		}

		break;

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_MOUSEWHEEL:
		if(IsReady() && msg == WM_MOUSEWHEEL)
		{
			int nDelta = (short)HIWORD(wParam);
			if(nDelta < 0)
			{
				m_pTargetObj->AddAngle(2.5f);
				m_pTargetObj->UpdateLocalMatrix();
				bProcessed = TRUE;
			}
			else
			{
				m_pTargetObj->AddAngle(-2.5f);
				m_pTargetObj->UpdateLocalMatrix();
				bProcessed = TRUE;
			}
		}

		break;
	
	}	//end of switch

	return bProcessed;
}

#endif	// __CLIENT

#endif //__GUILD_HOUSE


