#include "stdafx.h"
#include "CreateMonster.h"
#include "user.h"

#if __VER >= 12 // __NEW_ITEMCREATEMON_SERVER
#include "defineText.h"
#include "DPSrvr.h"
extern CDPSrvr g_DPSrvr;

CCreateMonster::CCreateMonster()
:
m_nMaxCreateNum( 0 )
{

}

CCreateMonster::~CCreateMonster()
{
	for( MAPPROP::iterator it=m_mapCreateMonsterProp.begin(); it!=m_mapCreateMonsterProp.end(); it++ )
	{
		it->second->mapMonster.clear();
		safe_delete( it->second );
	}
	m_mapCreateMonsterProp.clear();
	m_mapCreateMonsterInfo.clear();
}

CCreateMonster* CCreateMonster::GetInstance( void )
{
	static CCreateMonster sCreateMonster;
	return &sCreateMonster;
}

void CCreateMonster::LoadScript()
{
	CLuaBase lua;

	if( lua.RunScript( "CreateMonster.lua" ) != 0 )
	{
		Error( "CreateMonser.lua Load Failed!!!" );
		exit(0);
	}

	m_nMaxCreateNum = static_cast<int>( lua.GetGlobalNumber( "m_nMaxCreateNum" ) );

	lua.GetGloabal( "tCreateMonster" );
	lua.PushNil();
	while( lua.TableLoop( -2 ) )
	{
		DWORD dwItemId = static_cast<DWORD>( CScript::GetDefineNum( lua.GetFieldToString( -1, "strItemId" ) ) );
		CREATE_MONSTER_PROP* pCreateMonsterProp = new CREATE_MONSTER_PROP;
		m_mapCreateMonsterProp.insert( make_pair( dwItemId, pCreateMonsterProp ) );
		pCreateMonsterProp->dwKeepTime = static_cast<DWORD>( lua.GetFieldToNumber( -1, "dwKeepTime" ) );

		//lua_getfield( lua.GetLuaState(), -1, "tMonster" );
		lua.GetField( -1, "tMonster" );
		lua.PushNil();
		while( lua.TableLoop( -2 ) )
		{
			DWORD dwMonsterId = static_cast<DWORD>( CScript::GetDefineNum( lua.GetFieldToString( -1, "strMonsterId" ) ) );
			int nProbability = static_cast<int>( lua.GetFieldToNumber( -1, "nProbability" ) );
			pCreateMonsterProp->mapMonster.insert( make_pair( dwMonsterId, nProbability ) );
			lua.Pop( 1 );
		}
		lua.Pop( 2 );
	}

	lua.Pop( 0 );	
}

void CCreateMonster::SetState( OBJID objId, const char chState )
{
	MAPINFO::iterator it = m_mapCreateMonsterInfo.find( objId );
	if( it == m_mapCreateMonsterInfo.end() )
		return;

	it->second.chState = chState;
}

CREATE_MONSTER_PROP* CCreateMonster::GetCreateMonsterProp( CItemElem* pItemElem )
{
	MAPPROP::iterator it = m_mapCreateMonsterProp.find( pItemElem->m_dwItemId );
	if( it == m_mapCreateMonsterProp.end() )
        return NULL;

	return it->second;
}

void CCreateMonster::ProcessRemoveMonster()
{
	if( m_mapCreateMonsterInfo.empty() )
		return;

	DWORD dwTick = GetTickCount();

	for( MAPINFO::iterator it=m_mapCreateMonsterInfo.begin(); it!=m_mapCreateMonsterInfo.end(); it++ )
	{
		CREATE_MONSTER_INFO* pCreateMonsterInfo = &it->second;
		if( dwTick >= pCreateMonsterInfo->dwEndTick )
		{
			CMover* pMover = prj.GetMover( it->first );
			if( IsValidObj( pMover ) )
			{
				if( pMover->IsLive() && !pMover->IsDelete() )
				{
					pCreateMonsterInfo->chState = 'R';
					pMover->Delete();
				}
			}
		}
	}
}

BOOL CCreateMonster::IsAttackAble( CUser* pUser, CMover* pTarget, BOOL bTextOut )
{
	if( !IsValidObj( pUser ) || !IsValidObj( pTarget ) )
		return FALSE;
	
	// 아이템에 의해 소환된 몬스터가 아니면 공격가능
	MAPINFO::iterator it = m_mapCreateMonsterInfo.find( pTarget->GetId() );
	if( it == m_mapCreateMonsterInfo.end() )
		return TRUE;

	// 소환 주체가 접속중이 아니면 아무도 공격못한다.
	CUser* pOwner = static_cast<CUser*>( prj.GetUserByID( it->second.dwOwnerId ) );
	// 공격자가 소환 주체이거나 같은 파티원인 경우만 공격가능...
	if( IsValidObj( pOwner )
		&& ( ( pOwner == pUser ) || ( pOwner->GetPartyId() && pOwner->GetPartyId() == pUser->GetPartyId() ) ) )
		return TRUE;

	if( bTextOut )
		pUser->AddDefinedText( TID_GAME_PRIORITYSUMMONMOB, "\"%s\"", CPlayerDataCenter::GetInstance()->GetPlayerString( it->second.dwOwnerId ) );

	return FALSE;
}

void CCreateMonster::CreateMonster( CUser* pUser, DWORD dwObjId, D3DXVECTOR3 vPos )
{
	if( (int)( m_mapCreateMonsterInfo.size() ) >= m_nMaxCreateNum )
	{
		pUser->AddDefinedText( TID_GAME_CREATEMON_LIMIT );
		return;
	}
	
	CItemElem* pItemElem = pUser->m_Inventory.GetAtId( dwObjId );
	if( !IsUsableItem( pItemElem ) )
		return;

	CREATE_MONSTER_PROP* pCreateMonsterProp = GetCreateMonsterProp( pItemElem );
	if( !pCreateMonsterProp )
		return;

	DWORD dwMonsterId = pCreateMonsterProp->GetRandomMonsterId();
	if( dwMonsterId == NULL_ID )
		return;

	CWorld* pWorld = pUser->GetWorld();
	MoverProp* pMoverProp = prj.GetMoverProp( dwMonsterId );

	if( pWorld && pMoverProp && pMoverProp->dwID != 0 )
	{
		D3DXVECTOR3 vDist2 = pUser->GetPos() - vPos;
		float fDist = D3DXVec3Length( &vDist2 );			// 두좌표간의 거리
		if( 15.f < fDist )
		{
			pUser->AddDefinedText( TID_GAME_CREATEMON_F_15 );
			return;
		}

		int nAttr = pWorld->GetHeightAttribute( vPos.x, vPos.z );		// 이동할 위치의 속성 읽음.
		if( nAttr == HATTR_NOWALK || nAttr == HATTR_NOMOVE )		// 못 움직이는 곳이면 Pass
		{
			pUser->AddDefinedText( TID_GAME_CREATEMON_F_AREA );
			return;
		}
		if( pUser->IsRegionAttr( RA_SAFETY ))		// 안전지역이면 Pass
		{
			pUser->AddDefinedText( TID_GAME_CREATEMON_F_AREA );
			return;
		}
		if( pWorld->GetID() != WI_WORLD_MADRIGAL )
		{
			pUser->AddDefinedText( TID_GAME_CREATEMON_F_AREA );
			return;
		}

		CObj* pObj	= CreateObj( D3DDEVICE, OT_MOVER, pMoverProp->dwID );
		if( NULL == pObj )	return;	// ASSERT( pObj );
		pObj->SetPos( vPos );
		pObj->InitMotion( MTI_STAND );
		pObj->UpdateLocalMatrix();
		((CMover*)pObj)->m_bActiveAttack = FALSE;
		((CMover*)pObj)->AddItToGlobalId();
		pWorld->ADDOBJ( pObj, FALSE, pUser->GetLayer() );
		
		LogItemInfo aLogItem;
		aLogItem.Action = "i";
		aLogItem.SendName = pUser->GetName();
		char szTemp[128] = {0,};
		sprintf( szTemp, "M_C_%s", pMoverProp->szName ); 
		memset( szTemp+31, 0, sizeof(szTemp)-31 );

		aLogItem.RecvName = szTemp;
		aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
		aLogItem.Gold_1 = ((CMover*)pObj)->GetId();
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		g_DPSrvr.OnLogItem( aLogItem, pItemElem, 1 );

		pUser->AddDefinedText( TID_GAME_CREATEMON_S, "\"%s\"", pMoverProp->szName );
		pUser->RemoveItem( (BYTE)( dwObjId ), (short)( 1 ) );
		
		CREATE_MONSTER_INFO createMonsterInfo;
		createMonsterInfo.chState = 'N';
		createMonsterInfo.dwOwnerId = pUser->m_idPlayer;
		createMonsterInfo.dwEndTick = pCreateMonsterProp->dwKeepTime + GetTickCount();
		m_mapCreateMonsterInfo.insert( make_pair( ((CMover*)pObj)->GetId(), createMonsterInfo ) );
	}
}

void CCreateMonster::RemoveInfo( CMover* pMover )
{
	MAPINFO::iterator it=m_mapCreateMonsterInfo.find( pMover->GetId() );
	if( it == m_mapCreateMonsterInfo.end() )
		return;
	
	CREATE_MONSTER_INFO createMonsterInfo = it->second;
	if( pMover )
	{
		LogItemInfo aLogItem;
		aLogItem.Action = "i";
		aLogItem.SendName = CPlayerDataCenter::GetInstance()->GetPlayerString( createMonsterInfo.dwOwnerId );
		char szTemp[128] = {0,};
		sprintf( szTemp, "M_%c_%s", createMonsterInfo.chState, pMover->GetName() ); 
		memset( szTemp+31, 0, sizeof(szTemp)-31 );
		aLogItem.RecvName = szTemp;
		aLogItem.Gold = aLogItem.Gold2 = 0;
		aLogItem.Gold_1 = pMover->GetId();
		aLogItem.WorldId = pMover->GetWorld()->GetID();
		g_DPSrvr.OnLogItem( aLogItem );
	}

	m_mapCreateMonsterInfo.erase( it );
}
#endif // __NEW_ITEMCREATEMON_SERVER





///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
#if __VER < 12 // __NEW_ITEMCREATEMON_SERVER
CCreateMonster::CCreateMonster()
{
}

CCreateMonster::~CCreateMonster()
{

}

BOOL CCreateMonster::LoadScript( LPCSTR lpszFileName )
{
	CScript Script;

	if( !Script.Load( lpszFileName ) )
		return FALSE;

	Script.GetToken();
	while( Script.tok != FINISHED )
	{
		DWORD dwMonsterId = atoi( Script.Token );
		Script.GetToken();	// {
		vector< __MONSTERPERSENT > VecMonsterPersent;
		Script.GetToken();
		while( *Script.token != '}' )
		{
			__MONSTERPERSENT OneMosterPersent;
			OneMosterPersent.dwMonsterId = atoi( Script.Token );
			OneMosterPersent.nPersent = Script.GetNumber();
			VecMonsterPersent.push_back( OneMosterPersent );
			Script.GetToken();
		}
		Script.GetToken();	
		pMonsterItem.insert( map< DWORD, vector<__MONSTERPERSENT > >::value_type( dwMonsterId, VecMonsterPersent ) );
	}
	return TRUE;
}

DWORD CCreateMonster::GetCreateMonster(DWORD dwItemId)
{
	int nMax = 0;
	int nRandom = xRandom( 100 );
	map<DWORD, vector<__MONSTERPERSENT> >::iterator i	= pMonsterItem.find( dwItemId );
	if( i != pMonsterItem.end() )
	{
		vector<__MONSTERPERSENT> OneItem = i->second;
		for( vector<__MONSTERPERSENT>::iterator  itv = OneItem.begin() ; itv != OneItem.end() ; ++itv )
		{
			int nPersent = ((__MONSTERPERSENT)*itv).nPersent;
			nMax += nPersent;
			if( nRandom <= nMax )
				return ((__MONSTERPERSENT)*itv).dwMonsterId;
		}
	}
	return 0;
}

CCreateMonster g_CreateMonster;
#endif // __NEW_ITEMCREATEMON_SERVER


