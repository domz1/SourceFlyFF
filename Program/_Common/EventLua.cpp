// EventLua.cpp: implementation of the CEventLua class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "EventLua.h"
#ifdef __WORLDSERVER
#include "defineText.h"
#include "User.h"
#include "DPSrvr.h"
#include "DPDatabaseClient.h"
extern	CDPSrvr				g_DPSrvr;
extern	CDPDatabaseClient	g_dpDBClient;
#ifdef __EVENTLUA_SPAWN
#include "worldmng.h"
extern	CWorldMng	g_WorldMng;
#endif // __EVENTLUA_SPAWN
#endif // __WORLDSERVER
#ifdef __VTN_TIMELIMIT
#ifdef __DBSERVER
#include "lang.h"
#endif // __DBSERVER
#endif // __VTN_TIMELIMIT
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEventLua::CEventLua()
{
	m_bRun = FALSE;
#ifdef __VTN_TIMELIMIT
#ifdef __DBSERVER
	m_bTimeLimit = TRUE;
#endif // __DBSERVER
#endif // __VTN_TIMELIMIT
}

CEventLua::~CEventLua()
{
#ifdef __WORLDSERVER
#ifdef __EVENTLUA_SPAWN
	m_mapSpawnList.clear();			// spawn 이벤트 진행시 생성될 아이템 및 몬스터 정보
	m_mapMonsterId.clear();			// spawn 이벤트 진행시 생성된 몬스터가 이벤트용 인지 검사하기 위해 필요
	m_vecSpawnRegion.clear();		// spawn 이벤트 진행시 아이템 및 몬스터의 생성위치(월드 및 좌표) 정보
	m_mapSpawnedMonster.clear();	// spawn 이벤트 진행시 실제로 생성된 몬스터들의 목록 저장(이벤트 종료시 삭제하기 위해 필요)
	m_vecEndSpawnEvent.clear();		// 종료된 이벤트 ID 목록( 남아있는 spawn 몬스터가 있으면 다 삭제한 후 ID를 제거한다
#endif // __EVENTLUA_SPAWN
#ifdef __EVENTLUA_KEEPCONNECT
	m_vecKeepConnectUser.clear();
	m_mapItemList.clear();
#endif // __EVENTLUA_KEEPCONNECT
#endif // __WORLDSERVER
}

#ifdef __DBSERVER
#ifdef __AUTO_NOTICE
BOOL CEventLua::IsNoticeTime()
{
	if( !IsPossible() )
		return FALSE;

	BOOL bIsNoticeTime = FALSE;
	if( m_Lua.GetLuaFunction("IsNoticeTime") )
	{
		m_Lua.CallLuaFunction( 0, 1 );
		bIsNoticeTime = static_cast<BOOL>( m_Lua.ToBool( -1 ) );
	}
	m_Lua.Pop(0);

	return bIsNoticeTime;
}

vector<string> CEventLua::GetNoticeMessage()
{
	vector<string> vecstrMessage;

	if( !IsPossible() )
		return vecstrMessage;

	if( m_Lua.GetLuaFunction( "GetNoticeMessage" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );

		m_Lua.PushNil();
		while( m_Lua.TableLoop( -2 ) )
		{
			string strMessage;
			strMessage = m_Lua.ToString( -1 );
			vecstrMessage.push_back( strMessage );

			m_Lua.Pop( 1 );
		}
	}
	m_Lua.Pop( 0 );

	return vecstrMessage;
}
#endif // __AUTO_NOTICE

BOOL CEventLua::CheckEventState()
{
	if( !IsPossible() )
		return FALSE;

	if( m_Lua.GetLuaFunction("GetEventState") )
	{
		m_Lua.CallLuaFunction( 0, 1 );
		if( m_Lua.IsNil( -1 ) )
		{
			m_Lua.Pop(0);
			return FALSE;
		}
		
		m_mapState.clear();
		m_Lua.PushNil();
		while( m_Lua.TableLoop(-2) )
		{
			BYTE nId = (BYTE)m_Lua.GetFieldToNumber( -1, "nId" );
			BOOL bState = (BOOL)m_Lua.GetFieldToNumber( -1, "State" );
			m_mapState.insert( make_pair( nId, bState ) );
			m_Lua.Pop( 1 );
		}
		
		if( m_mapState.empty() )
		{
			m_Lua.Pop(0);
			return FALSE;
		}
	}
	m_Lua.Pop(0);
	
	return TRUE;
}
#endif // __DBSERVER

#if defined(__WORLDSERVER) || defined(__DBSERVER)
vector<BYTE> CEventLua::GetEventList( BOOL bProxy )
{
	vector<BYTE> vecList;

	if( !IsPossible() )
		return vecList;

#ifdef __WORLDSERVER
#if __VER >= 12 // __EVENTLUA_0826
		if( bProxy )
			return m_proxy.GetEventList();
#endif	// __EVENTLUA_0826
#endif // __WORLDSERVER

	if( m_Lua.GetLuaFunction( "GetEventList" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );
		if( lua_isnil( m_Lua.GetLuaState(), -1 ) )
		{
			m_Lua.Pop(0);
			return vecList;
		}
		
		m_Lua.PushNil();
		while( m_Lua.TableLoop(-2) )
		{
			vecList.push_back( (BYTE)m_Lua.ToNumber(-1) );
			m_Lua.Pop(1);
		}
	}
	m_Lua.Pop(0);
	
	return vecList;
}

void CEventLua::LoadScript()
{
	m_bRun = FALSE;
	if( m_Lua.RunScript( "Event.lua" ) == 0 )
	{
		m_bRun = TRUE;
#ifdef __WORLDSERVER
#if __VER >= 12 // __EVENTLUA_0826
		PrepareProxy();
#endif	// __EVENTLUA_0826
#ifdef __EVENTLUA_SPAWN
		for( map<BYTE, VECSPAWNINFO>::iterator it=m_mapSpawnList.begin(); it!=m_mapSpawnList.end(); it++ )
			m_vecEndSpawnEvent.push_back( it->first );
		m_mapSpawnList.clear();
#endif // __EVENTLUA_SPAWN
#endif // __WORLDSERVER
#ifdef __DBSERVER
#ifdef __VTN_TIMELIMIT
		if( ::GetLanguage() == LANG_VTN )
			m_bTimeLimit = m_Lua.GetGlobalBool( "bTimeLimit" );
#endif // __VTN_TIMELIMIT
#endif // __DBSERVER
	}
	else
	{
		Error( "Event.lua 실행 실패!!!" );
	}
}
#endif // __WORLDSERVER, __DBSERVER

#ifdef __WORLDSERVER
void CEventLua::SetState( BYTE nId, BOOL bState )
{
	if( !IsPossible() )
		return;

	if( m_Lua.GetLuaFunction( "SetState" ) )
	{
		m_Lua.PushNumber( nId );
		m_Lua.PushNumber( bState ? 1 : 0 );
		m_Lua.CallLuaFunction( 2, 1 );
		m_bRun = m_Lua.ToBool( -1 );
	}

	m_Lua.Pop(0);
#ifdef __EVENTLUA_SPAWN
	SetSpawnEvent( nId, bState );
#endif // __EVENTLUA_SPAWN
}

string CEventLua::GetDesc( BYTE nId )
{
	string strDesc;

	if( !IsPossible() )
		return strDesc;

	if( m_Lua.GetLuaFunction( "GetDesc" ) )
	{
		m_Lua.PushNumber( nId );
		m_Lua.CallLuaFunction( 1, 1 );
		
		strDesc = m_Lua.ToString( -1 );
	}
	m_Lua.Pop( 0 );

	return strDesc;
}

map<DWORD, int> CEventLua::GetItem( DWORD dwLevel )
{
	map<DWORD, int> mapItem;

	if( !IsPossible() )
		return mapItem;

	DWORD dwTickCount	= GetTickCount();
	if( m_Lua.GetLuaFunction( "GetItem" ) )
	{
		m_Lua.PushNumber( dwTickCount );
		m_Lua.PushNumber( dwLevel );
		m_Lua.CallLuaFunction( 2, 1 );
		if( m_Lua.IsNil( -1 ) )
		{
			m_Lua.Pop(0);
			return mapItem;
		}

		m_Lua.PushNil();
		while( m_Lua.TableLoop(-2) )
		{
			DWORD dwItem = CScript::GetDefineNum( m_Lua.GetFieldToString( -1, "ItemId" ) );
			int	nNum = (int)m_Lua.GetFieldToNumber( -1, "ItemNum" );
			mapItem.insert( make_pair( dwItem, nNum ) );
			m_Lua.Pop(1);
		}
	}
	m_Lua.Pop(0);

	return mapItem;
}

float CEventLua::GetExpFactor( BOOL bProxy )
{
	float fFactor = 1;

	if( !IsPossible() )
		return fFactor;

#if __VER >= 12 // __EVENTLUA_0826
	if( bProxy )
		return m_proxy.GetExpFactor();
#endif	// __EVENTLUA_0826

	if( m_Lua.GetLuaFunction( "GetExpFactor" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );

		fFactor = (float)m_Lua.ToNumber( -1 );
	}
	m_Lua.Pop(0);

	return fFactor;
}

float CEventLua::GetItemDropRate( BOOL bProxy )
{
	float fFactor = 1;

	if( !IsPossible() )
		return fFactor;

#if __VER >= 12 // __EVENTLUA_0826
	if( bProxy )
		return m_proxy.GetItemDropRate();
#endif	// __EVENTLUA_0826

	if( m_Lua.GetLuaFunction( "GetItemDropRate" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );
		
		fFactor = (float)m_Lua.ToNumber( -1 );
	}
	m_Lua.Pop(0);
	
	return fFactor;
}

float CEventLua::GetPieceItemDropRate( BOOL bProxy )
{
	float fFactor = 1;

	if( !IsPossible() )
		return fFactor;

#if __VER >= 12 // __EVENTLUA_0826
	if( bProxy )
		return m_proxy.GetPieceItemDropRate();
#endif	// __EVENTLUA_0826

	if( m_Lua.GetLuaFunction( "GetPieceItemDropRate" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );
		
		fFactor = (float)m_Lua.ToNumber( -1 );
	}
	m_Lua.Pop(0);
	
	return fFactor;
}

float CEventLua::GetGoldDropFactor( BOOL bProxy )
{
	float fFactor = 1;

	if( !IsPossible() )
		return fFactor;

#if __VER >= 12 // __EVENTLUA_0826
	if( bProxy )
		return m_proxy.GetGoldDropFactor();
#endif	// __EVENTLUA_0826

	if( m_Lua.GetLuaFunction( "GetGoldDropFactor" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );
		
		fFactor = (float)m_Lua.ToNumber( -1 );
	}
	m_Lua.Pop(0);
	
	return fFactor;
}

void CEventLua::GetAllEventList( CUser* pUser )
{
	if( !IsPossible() )
		return;

	if( m_Lua.GetLuaFunction( "GetAllEventList" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );
		if( m_Lua.IsNil( -1 ) )
		{
			m_Lua.Pop(0);
			return;
		}
		
		m_Lua.PushNil();
		while( m_Lua.TableLoop(-2) )
		{
			int nId = (int)m_Lua.GetFieldToNumber( -1, "nId" );
			string strTitle = m_Lua.GetFieldToString( -1, "strTitle" );
			int nState = (int)m_Lua.GetFieldToNumber( -1, "nState" );
			
			CString strTemp;
			strTemp.Format("ID : %d,  Title : %s,  State : %d", nId, strTitle.c_str(), nState );
			pUser->AddText( strTemp );
			
			m_Lua.Pop(1);
		}
	}
	m_Lua.Pop(0);
}
void CEventLua::GetEventInfo( CUser* pUser, int nId )
{
	if( !IsPossible() )
		return;

	if( m_Lua.GetLuaFunction( "GetEventInfo" ) )
	{
		m_Lua.PushNumber( nId );
		m_Lua.CallLuaFunction( 1, 1 );
		if( m_Lua.IsNil( -1 ) )
		{
			m_Lua.Pop(0);
			return;
		}

		m_Lua.PushNil();
		while( m_Lua.TableLoop(-2) )
		{
			CString strTemp = m_Lua.ToString(-1);
			pUser->AddText( strTemp );

			m_Lua.Pop(1);
		}
	}
	m_Lua.Pop(0);
}

int CEventLua::GetAttackPower( BOOL bProxy )
{
	int nAttackPower = 0;
	if( !IsPossible() )
		return nAttackPower;

#if __VER >= 12 // __EVENTLUA_0826
	if( bProxy )
		return m_proxy.GetAttackPower();
#endif	// __EVENTLUA_0826

	if( m_Lua.GetLuaFunction( "GetAttackPower" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );
		nAttackPower = (int)m_Lua.ToNumber( -1 );
	}
	m_Lua.Pop(0);

	return nAttackPower;
}

int CEventLua::GetDefensePower( BOOL bProxy )
{
	int nDefensePower = 0;
	
	if( !IsPossible() )
		return nDefensePower;

#if __VER >= 12 // __EVENTLUA_0826
	if( bProxy )
		return m_proxy.GetDefensePower();
#endif	// __EVENTLUA_0826

	if( m_Lua.GetLuaFunction( "GetDefensePower" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );
		nDefensePower = (int)m_Lua.ToNumber( -1 );
	}
	m_Lua.Pop(0);

	return nDefensePower;
}

DWORD CEventLua::GetCouponEvent( BOOL bProxy )
{
	if( !IsPossible() )
		return 0;

#if __VER >= 12 // __EVENTLUA_0826
	if( bProxy )
		return m_proxy.GetCouponEvent();
#endif	// __EVENTLUA_0826

	DWORD dwCouponEvent	= 0;
	if( m_Lua.GetLuaFunction( "GetCouponEvent" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );
		dwCouponEvent	= (DWORD)m_Lua.ToNumber( -1 );
	}
	m_Lua.Pop(0);
	return dwCouponEvent;
}

#ifdef __EVENTLUA_COUPON
void CEventLua::SetCoupon( CUser* pUser, DWORD dwTick )
{
	if( !IsPossible() )
		return;

	if( !IsValidObj( pUser ) )
		return;

	DWORD dwCouponEvent	= GetCouponEvent();
	
	if( dwCouponEvent )
	{
		if( dwTick > pUser->m_dwTickCoupon + dwCouponEvent )
		{
			pUser->m_nCoupon++;
			pUser->m_dwTickCoupon = dwTick;
			pUser->AddCoupon( pUser->m_nCoupon );
		}
	}
	else
		pUser->m_dwTickCoupon	= dwTick;
}
#endif // __EVENTLUA_COUPON

#ifdef __EVENTLUA_GIFT
void CEventLua::SetLevelUpGift( CUser* pUser, int nLevel )
{
	if( !IsPossible() )
		return;

	if( m_Lua.GetLuaFunction( "GetLevelUpGift" ) )
	{
		m_Lua.PushNumber( nLevel );
		m_Lua.PushString( pUser->m_playAccount.lpszAccount );
		m_Lua.CallLuaFunction( 2, 1 );
		if( m_Lua.IsNil( -1 ) )
		{
			m_Lua.Pop(0);
			return;
		}

		m_Lua.PushNil();
		while( m_Lua.TableLoop(-2) )
		{
			DWORD dwItemId = CScript::GetDefineNum( m_Lua.GetFieldToString( -1, "strItemId" ) );
			int	nItemNum = (int)m_Lua.GetFieldToNumber( -1, "nItemNum" );

			if( !prj.GetItemProp( dwItemId ) )
			{
				Error( "CEventLua::SetLevelUpGift() - ItemProp is NULL. ItemId = \"%s\"", m_Lua.GetFieldToString( -1, "strItemId" ) );
				m_Lua.Pop( 1 );
				continue;
			}

			CItemElem itemElem;
			itemElem.m_dwItemId = dwItemId;
			itemElem.m_nItemNum	= nItemNum;
			itemElem.SetFlag( static_cast<BYTE>( m_Lua.GetFieldToNumber( -1, "byFlag" ) ) );
			itemElem.SetSerialNumber();
			
			LogItemInfo aLogItem;
			aLogItem.Action = "E";
			aLogItem.SendName = pUser->GetName();
			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();

			
			if( pUser->CreateItem( &itemElem ) )
			{	// 로그 남김
				aLogItem.RecvName = "EVENT_LEVELUP_GIFT";
				g_DPSrvr.OnLogItem( aLogItem, &itemElem, nItemNum );
			}
			else
			{	// 우편 지급
				g_dpDBClient.SendQueryPostMail( pUser->m_idPlayer, 0, itemElem, 0, "", "" );
				aLogItem.RecvName = "EVENT_LEVELUP_GIFT_POST";
				g_DPSrvr.OnLogItem( aLogItem, &itemElem, nItemNum );
			}

			pUser->AddDefinedText( TID_GAME_EVENT_LEVELUP_GIFT, "\"%s\"", itemElem.GetName() );
			
			m_Lua.Pop(1);
		}
	}
	m_Lua.Pop(0);
}
#endif // __EVENTLUA_GIFT

#ifdef __EVENTLUA_CHEEREXP
float CEventLua::GetCheerExpFactor( BOOL bProxy )
{
	float fFactor = 1;

	if( !IsPossible() )
		return fFactor;

#if __VER >= 12 // __EVENTLUA_0826
	if( bProxy )
		return m_proxy.GetCheerExpFactor();
#endif	// __EVENTLUA_0826

	if( m_Lua.GetLuaFunction( "GetCheerExpFactor" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );

		fFactor = (float)m_Lua.ToNumber( -1 );
	}
	m_Lua.Pop(0);

	return fFactor;
}
#endif // __EVENTLUA_CHEEREXP

#ifdef __EVENTLUA_SPAWN
void CEventLua::SetSpawnEvent( BYTE nId, BOOL bState )
{
	if( m_Lua.GetLuaFunction( "GetSpawn" ) )
	{
		m_Lua.PushNumber( nId );
		m_Lua.CallLuaFunction( 1, 2 );
		int nSize = static_cast<int>( m_Lua.ToNumber( -1 ) );	m_Lua.Pop( 1 );

		if( bState && nSize > 0 )
		{
			VECSPAWNINFO vecTemp;
			m_Lua.PushNil();
			while( m_Lua.TableLoop( -2 ) )
			{
				int nType = static_cast<int>( m_Lua.GetFieldToNumber( -1, "nType" ) );
				DWORD dwId = CScript::GetDefineNum( m_Lua.GetFieldToString( -1, "strId" ) );
				DWORD dwInterval = static_cast<DWORD>( m_Lua.GetFieldToNumber( -1, "nDayNum" ) );
				dwInterval = 86400000 / dwInterval;
				vecTemp.push_back( __SPAWNINFO( nType, dwId, dwInterval ) );
				if( nType == OT_MOVER )
					m_mapMonsterId.insert( make_pair( dwId, nId ) );
				
				m_Lua.Pop( 1 );
			}

			map<BYTE, VECSPAWNINFO>::iterator it = m_mapSpawnList.find( nId );
			if( it == m_mapSpawnList.end() )
				m_mapSpawnList.insert( make_pair( nId, vecTemp ) );
			else
				it->second.assign( vecTemp.begin(), vecTemp.end() );

			// 완료 리스트에서 재적용된 이벤트를 제거한다.
			for( vector<BYTE>::iterator itVec=m_vecEndSpawnEvent.begin(); itVec!=m_vecEndSpawnEvent.end(); )
			{
				if( (*itVec) == nId )
					itVec = m_vecEndSpawnEvent.erase( itVec );
				else
					itVec++;
			}
		}
		else
		{
			map<BYTE, VECSPAWNINFO>::iterator it = m_mapSpawnList.find( nId );
			if( it != m_mapSpawnList.end() )
			{
				m_vecEndSpawnEvent.push_back( it->first );	// 완료 리스트에 해당 이벤트 ID를 추가한다. 스폰된 몬스터들을 제거하기 위해 필요.
				m_mapSpawnList.erase( it );
			}
		}
	}
	
	m_Lua.Pop(0);
}

void CEventLua::EventSpawnProcess()
{
	//if( m_mapSpawnList.size() <= 0 )
	//	return;

	for( map<BYTE, VECSPAWNINFO>::iterator itMap=m_mapSpawnList.begin(); itMap!=m_mapSpawnList.end(); itMap++ )
	{
		VECSPAWNINFO* pVecSP = &itMap->second;
		for( VECSPAWNINFO::iterator itVec=pVecSP->begin(); itVec!=pVecSP->end(); itVec++ )
		{
			__SPAWNREGION sp = GetSpawnRegion();
				CWorld* pWorld	= g_WorldMng.GetWorld( sp.dwWorldId );
			if( !pWorld )
				continue;
			
			__SPAWNINFO* pSI = &(*itVec);
			if( !pSI->IsTimeOut() )
				continue;

			switch( pSI->nType )
			{
				case OT_ITEM:
				{
					ItemProp* pProp = prj.GetItemProp( pSI->dwId );
					if( pProp )
					{
						D3DXVECTOR3 v = sp.GetPos();
						v.y = pWorld->GetLandHeight( v.x, v.z ) + 1.0f;
						CObj* pObj = CreateObj( D3DDEVICE, OT_ITEM, pProp->dwID );
						if( pObj )
						{
							CItemElem* pItemElem	= new CItemElem;
							pItemElem->m_dwItemId	= pProp->dwID;
							pItemElem->m_nItemNum	= 1;
							pItemElem->m_nHitPoint	= pProp->dwEndurance;
							pItemElem->SetSerialNumber();
							( (CItem*)pObj )->m_dwDropTime	= g_tmCurrent + MIN( 1440 );	// 24시간 유지
							( (CItem*)pObj )->m_pItemBase	= pItemElem;
							pObj->SetPos( v );
							pWorld->ADDOBJ( pObj, TRUE, nDefaultLayer );
							OUTPUTDEBUGSTRING( "\nLua Spawn: type = ITEM, index = %d, world = %d, x = %f, z = %f", pItemElem->m_dwItemId, pWorld->GetID(), pObj->GetPos().x, pObj->GetPos().z );
						}
					}
					break;
				}
				case OT_MOVER:
				{
					MoverProp* pMoverProp = prj.GetMoverPropEx( pSI->dwId );
					if( pMoverProp )
					{
						CMover* pMover	= static_cast<CMover*>( CreateObj( D3DDEVICE, OT_MOVER, pMoverProp->dwID ) );
						if( pMover )	
						{
							D3DXVECTOR3 v = sp.GetPos();
							v.y = pWorld->GetLandHeight( v.x, v.z ) + 1.0f;
							pMover->SetPos( v );
							pMover->InitMotion( MTI_STAND );
							pMover->UpdateLocalMatrix();
							pMover->AddItToGlobalId();
							pWorld->ADDOBJ( pMover, FALSE, nDefaultLayer );
							m_mapSpawnedMonster.insert( make_pair( pMover->GetId(), itMap->first ) );
							OUTPUTDEBUGSTRING( "\nLua Spawn: type = MONSTER, index = %d, world = %d, x = %f, z = %f", pSI->dwId, pWorld->GetID(), pMover->GetPos().x, pMover->GetPos().z );
						}
					}
					break;
				}
			}	// switch
		}	// for vector
	}	// for map
	
	DeleteEndEventProcess();
}

void CEventLua::DeleteEndEventProcess()
{
	// 이벤트 완료 리스트에 있는 스폰된 몬스터들을 제거한다.(프로세스당 100개씩)
	for( vector<BYTE>::iterator itVec=m_vecEndSpawnEvent.begin(); itVec!=m_vecEndSpawnEvent.end(); )
	{
		int nDelCount = 0;
		for( map<OBJID, BYTE>::iterator itMSM=m_mapSpawnedMonster.begin(); itMSM!=m_mapSpawnedMonster.end(); )
		{
			if( itMSM->second == (*itVec) )
			{
				CMover* pMover = prj.GetMover( itMSM->first );
				if( IsValidObj( pMover ) )
				{
					pMover->Delete();
					OUTPUTDEBUGSTRING( "\nCEventLua::DeleteEndEventProcess() - ID:%d, %s(%d):%d, Remain:%d", (*itVec), pMover->GetName(), pMover->GetIndex(), pMover->GetId(), m_mapSpawnedMonster.size() - 1 );
				}
				m_mapSpawnedMonster.erase( itMSM++ );

				nDelCount++;
				if( nDelCount == (100/m_vecEndSpawnEvent.size()) )	// 100개가 삭제되었으면 다음 프로세스에 삭제한다.
					break;
			}
			else
				itMSM++;
		}
		
		if( nDelCount == 0 )	// 더이상 삭제될것이 없으면 이벤트 완료 목록에서 해당 ID를 제거한다.
		{
			for( map<DWORD, BYTE>::iterator itMId=m_mapMonsterId.begin(); itMId!=m_mapMonsterId.end(); )
			{
				if( itMId->second == (*itVec) )
					m_mapMonsterId.erase( itMId++ );
				else
					itMId++;
			}
			OUTPUTDEBUGSTRING( "\nCEventLua::DeleteEndEventProcess() - m_vecEndSpawnEvent : %d", (*itVec) );
			itVec = m_vecEndSpawnEvent.erase( itVec );
		}
		else
			itVec++;
	}
}
#endif // __EVENTLUA_SPAWN

#ifdef __EVENTLUA_KEEPCONNECT
void CEventLua::SetKeepConnectEvent( CUser* pUser, DWORD dwTick )
{
	if( !IsPossible() )
		return;

	if( !IsValidObj( pUser ) )
		return;

	DWORD dwKeepConnect	= GetKeepConnectTime();
	
	if( dwKeepConnect )
	{
		if( dwTick > pUser->m_dwTickKeepConnect + dwKeepConnect )
		{
			pUser->m_dwTickKeepConnect = dwTick;
			m_vecKeepConnectUser.push_back( pUser->m_idPlayer );
		}
	}
	else
		pUser->m_dwTickKeepConnect	= dwTick;
}

DWORD CEventLua::GetKeepConnectTime( BOOL bProxy )
{
	if( !IsPossible() )
		return 0;

	if( bProxy )
		return m_proxy.GetKeepConnectTime();

	DWORD dwKeepConnect	= 0;
	if( m_Lua.GetLuaFunction( "GetKeepConnectTime" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );
		dwKeepConnect	= (DWORD)m_Lua.ToNumber( -1 );
	}
	m_Lua.Pop(0);

	return dwKeepConnect;
}

void CEventLua::GetKeepConnectItem()
{
	if( !IsPossible() )
		return;
	
	if( m_Lua.GetLuaFunction( "GetKeepConnectItem" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );
		if( m_Lua.IsNil( -1 ) )
		{
			m_Lua.Pop(0);
			return;
		}
		m_mapItemList.clear();
		m_Lua.PushNil();
		while( m_Lua.TableLoop(-2) )
		{
			DWORD dwItemId = CScript::GetDefineNum( m_Lua.GetFieldToString( -1, "strItemId" ) );
			int	nItemNum = (int)m_Lua.GetFieldToNumber( -1, "nItemNum" );
			m_mapItemList.insert( map<DWORD, int>::value_type( dwItemId, nItemNum ) );
			m_Lua.Pop(1);
		}
	}
	m_Lua.Pop(0);
}

void CEventLua::KeepConnectEventProcess()
{
	if( !IsPossible() )
		return;

	int nVecSize = m_vecKeepConnectUser.size();
	if( !nVecSize )
		return;
	
	u_long idPlayer;
	int nCount = 0;

	for( int i = 0; i < nVecSize; ++i )
	{
		if( nCount >= 10 )
			break;
		
		idPlayer = m_vecKeepConnectUser.back();
		m_vecKeepConnectUser.pop_back();
		
		for( map<DWORD, int>::iterator it = m_mapItemList.begin(); it != m_mapItemList.end(); ++it )
		{
			CItemElem itemElem;
			itemElem.m_dwItemId = it->first;
			itemElem.m_nItemNum	= it->second;
			itemElem.SetSerialNumber();
			
			g_dpDBClient.SendQueryPostMail( idPlayer, 0, itemElem, 0, "", "" );
		}
		++nCount;
	}
}
#endif // __EVENTLUA_KEEPCONNECT

#ifdef __ENVIRONMENT_EFFECT

float CEventLua::GetWeatherEventExpFactor( BOOL bProxy )
{
	float fFactor = 1.0f;

	if( !IsPossible() )
		return fFactor;

#if __VER >= 12 // __EVENTLUA_0826
	if( bProxy )
		return m_proxy.GetWeatherEventExpFactor();
#endif	// __EVENTLUA_0826

	if( m_Lua.GetLuaFunction( "GetWeatherEventExpFactor" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );

		fFactor = (float)m_Lua.ToNumber( -1 );
	}
	m_Lua.Pop(0);

	return fFactor;
}

string CEventLua::GetWeatherEventTitle()
{
	string strTitle("");

	if( !IsPossible() )
		return strTitle;

	if( m_Lua.GetLuaFunction( "GetWeatherEventTitle" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );

		if( m_Lua.IsNil( -1 ) )
			return strTitle;
		strTitle = m_Lua.ToString( -1 );
	}
	m_Lua.Pop( 0 );

	return strTitle;
}

#else // __ENVIRONMENT_EFFECT

#ifdef __EVENTLUA_RAIN
float CEventLua::GetRainEventExpFactor( BOOL bProxy )
{
	float fFactor = 1.0f;

	if( !IsPossible() )
		return fFactor;

#if __VER >= 12 // __EVENTLUA_0826
	if( bProxy )
		return m_proxy.GetRainEventExpFactor();
#endif	// __EVENTLUA_0826

	if( m_Lua.GetLuaFunction( "GetRainEventExpFactor" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );

		fFactor = (float)m_Lua.ToNumber( -1 );
	}
	m_Lua.Pop(0);

	return fFactor;
}

string CEventLua::GetRainEventTitle()
{
	string strTitle("");

	if( !IsPossible() )
		return strTitle;

	if( m_Lua.GetLuaFunction( "GetRainEventTitle" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );

		if( m_Lua.IsNil( -1 ) )
			return strTitle;
		strTitle = m_Lua.ToString( -1 );
	}
	m_Lua.Pop( 0 );

	return strTitle;
}
#endif // __EVENTLUA_RAIN

#ifdef __EVENTLUA_SNOW
float CEventLua::GetSnowEventExpFactor( BOOL bProxy )
{
	float fFactor = 1.0f;

	if( !IsPossible() )
		return fFactor;

#if __VER >= 12 // __EVENTLUA_0826
	if( bProxy )
		return m_proxy.GetSnowEventExpFactor();
#endif	// __EVENTLUA_0826

	if( m_Lua.GetLuaFunction( "GetSnowEventExpFactor" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );

		fFactor = (float)m_Lua.ToNumber( -1 );
	}
	m_Lua.Pop(0);

	return fFactor;
}

string CEventLua::GetSnowEventTitle()
{
	string strTitle("");

	if( !IsPossible() )
		return strTitle;

	if( m_Lua.GetLuaFunction( "GetSnowEventTitle" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );

		if( m_Lua.IsNil( -1 ) )
			return strTitle;
		strTitle = m_Lua.ToString( -1 );
	}
	m_Lua.Pop( 0 );

	return strTitle;
}
#endif // __EVENTLUA_SNOW

#endif // __ENVIRONMENT_EFFECT

#ifdef __SHOP_COST_RATE
float CEventLua::GetShopBuyFactor( BOOL bProxy )
{
	float fFactor = 1.0f;

	if( !IsPossible() )
		return fFactor;

#if __VER >= 12 // __EVENTLUA_0826
	if( bProxy )
		return m_proxy.GetShopBuyFactor();
#endif	// __EVENTLUA_0826

	if( m_Lua.GetLuaFunction( "GetShopBuyFactor" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );

		fFactor = (float)m_Lua.ToNumber( -1 );
	}
	m_Lua.Pop(0);

	return fFactor;
}

float CEventLua::GetShopSellFactor( BOOL bProxy )
{
	float fFactor = 1.0f;

	if( !IsPossible() )
		return fFactor;

#if __VER >= 12 // __EVENTLUA_0826
	if( bProxy )
		return m_proxy.GetShopSellFactor();
#endif	// __EVENTLUA_0826

	if( m_Lua.GetLuaFunction( "GetShopSellFactor" ) )
	{
		m_Lua.CallLuaFunction( 0, 1 );

		fFactor = (float)m_Lua.ToNumber( -1 );
	}
	m_Lua.Pop(0);

	return fFactor;
}
#endif // __SHOP_COST_RATE

#endif // __WORLDSERVER

#ifdef __WORLDSERVER
#if __VER >= 12 // __EVENTLUA_0826
CEventLuaProxy::CEventLuaProxy()
:
m_fExpFactor( 1.0f ),
m_fItemDropRate( 1.0f ),
m_fPieceItemDropRate( 1.0f ),
m_fGoldDropFactor( 1.0f ),
m_nAttackPower( 0 ),
m_fDefensePower( 0 ),
m_dwCouponEvent( 0 )
#ifdef __EVENTLUA_CHEEREXP
,m_fCheerExpFactor( 1.0f )
#endif // __EVENTLUA_CHEEREXP
#ifdef __EVENTLUA_KEEPCONNECT
, m_dwKeepConnect( 0 )
#endif // __EVENTLUA_KEEPCONNECT

#ifdef __ENVIRONMENT_EFFECT

, m_fWeatherEventExpFactor( 1.0f )

#else // __ENVIRONMENT_EFFECT

#ifdef __EVENTLUA_RAIN
, m_fRainEventExpFactor( 1.0f )
#endif // __EVENTLUA_RAIN
#ifdef __EVENTLUA_SNOW
, m_fSnowEventExpFactor( 1.0f )
#endif // __EVENTLUA_SNOW

#endif // __ENVIRONMENT_EFFECT
#ifdef __SHOP_COST_RATE
, m_fShopBuyFactor( 1.0f )
, m_fShopSellFactor( 1.0f )
#endif // __SHOP_COST_RATE
{
}

CEventLuaProxy::~CEventLuaProxy()
{
}

void CEventLuaProxy::Initialize( CEventLua* pEventLua )
{
	OutputDebugString( "\nCEventLuaProxy::Initialize\n" );
	SetEventList( pEventLua->GetEventList( FALSE ) );
	SetExpFactor( pEventLua->GetExpFactor( FALSE ) );
	SetItemDropRate( pEventLua->GetItemDropRate( FALSE ) );
	SetPieceItemDropRate( pEventLua->GetPieceItemDropRate( FALSE ) );
	SetGoldDropFactor( pEventLua->GetGoldDropFactor( FALSE ) );
	SetAttackPower( pEventLua->GetAttackPower( FALSE ) );
	SetDefensePower( pEventLua->GetDefensePower( FALSE ) );
	SetCouponEvent( pEventLua->GetCouponEvent( FALSE ) );
#ifdef __EVENTLUA_CHEEREXP
	SetCheerExpFactor( pEventLua->GetCheerExpFactor( FALSE ) );
#endif // __EVENTLUA_CHEEREXP
#ifdef __EVENTLUA_KEEPCONNECT
	SetKeepConnectTime( pEventLua->GetKeepConnectTime( FALSE ) );
	pEventLua->GetKeepConnectItem();
#endif // __EVENTLUA_KEEPCONNECT

#ifdef __ENVIRONMENT_EFFECT

	SetWeatherEventExpFactor( pEventLua->GetWeatherEventExpFactor( FALSE ) );

#else // __ENVIRONMENT_EFFECT

#ifdef __EVENTLUA_RAIN
	SetRainEventExpFactor( pEventLua->GetRainEventExpFactor( FALSE ) );
#endif // __EVENTLUA_RAIN
#ifdef __EVENTLUA_SNOW
	SetSnowEventExpFactor( pEventLua->GetSnowEventExpFactor( FALSE ) );
#endif // __EVENTLUA_SNOW

#endif // __ENVIRONMENT_EFFECT
#ifdef __SHOP_COST_RATE
	SetShopBuyFactor( pEventLua->GetShopBuyFactor( FALSE ) );
	SetShopSellFactor( pEventLua->GetShopSellFactor( FALSE ) );
#endif // __SHOP_COST_RATE
}
#endif // __WORLDSERVER
#endif	// __EVENTLUA_0826
