// Exchange.cpp: implementation of the CExchange class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Exchange.h"
#ifdef __WORLDSERVER
#include "defineText.h"
#include "User.h"
#include "DPSrvr.h"

extern	CDPSrvr		g_DPSrvr;
#if __VER >= 15 // __CAMPUS
#include "DPDatabaseClient.h"
extern	CDPDatabaseClient	g_dpDBClient;
#endif // __CAMPUS
#endif // __WORLDSERVER

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#ifdef __TRADESYS
typedef CExchange::PSETLIST PSETLIST;
CExchange::CExchange()
{

}

CExchange::~CExchange()
{

}

BOOL CExchange::Load_Script()
{
	CScanner s;
	
	if( !s.Load( "Exchange_Script.txt" ) )
		return FALSE;
	
	CString strErrorMMIId;
	s.GetToken(); 
	while( s.tok != FINISHED )
	{
		strErrorMMIId = s.Token;
		int nMMIId = CScript::GetDefineNum( s.Token ); //MMI ID
		s.GetToken(); // {
		s.GetToken();
		__SETLIST create;
		int nCount = 0;
		while( *s.token != '}' )
		{
			if( s.Token == _T( "DESCRIPTION" ) )
			{
				s.GetToken(); // {
				s.GetToken(); // 설명 TEXT ID
				while( *s.token != '}' )
				{
					create.vecDesciprtionId.push_back( CScript::GetDefineNum( s.Token ) );
					s.GetToken();
				}
			}
			else if( s.Token == _T( "SET" ) )
			{
				nCount++;
				__SET Set;
				s.GetToken();	// List Text ID;
				Set.nSetTextId = CScript::GetDefineNum( s.Token );
				s.GetToken(); // {
				s.GetToken();
				while( *s.token != '}' )
				{
					if( s.Token == _T( "RESULTMSG" ) )
					{
						s.GetToken(); // {
						s.GetToken();
						while( *s.token != '}' )
						{
							Set.vecResultMsg.push_back( CScript::GetDefineNum( s.Token ) ); // 조건 아이템
							s.GetToken();
						} // while - RESULTMSG
					}
					else if( s.Token == _T( "CONDITION" ) )
					{
						s.GetToken(); // {
						s.GetToken();
						while( *s.token != '}' )
						{
							__ITEM item;
							if( s.Token == "PENYA" )
								item.dwItemId = II_GOLD_SEED1;
							else
								item.dwItemId = CScript::GetDefineNum( s.Token ); // 조건 아이템
							item.nItemNum = s.GetNumber(); // 조건 갯수
							Set.vecCondItem.push_back( item ); 
							s.GetToken();
						} // while - CONDITION
					}
					else if( s.Token == _T( "REMOVE" ) )
					{
						s.GetToken(); // {
						s.GetToken();
						while( *s.token != '}' )
						{
							__ITEM item;
							if( s.Token == "PENYA" )
								item.dwItemId = II_GOLD_SEED1;
							else
								item.dwItemId = CScript::GetDefineNum( s.Token ); // 조건 아이템
							item.nItemNum = s.GetNumber(); // 삭제 갯수
							Set.vecRemoveItem.push_back( item );
							s.GetToken();
						} // while - REMOVE
					}
#if __VER >= 15 // __CAMPUS
					else if( s.Token == _T( "CONDITION_POINT" ) )
					{
						s.GetToken(); // {
						s.GetToken();
						while( *s.token != '}' )
						{
							__POINT point;
							point.nType = CScript::GetDefineNum( s.Token );
							point.nPoint = s.GetNumber();	// 조건 포인트
							if( point.nType > 0 )
								Set.vecCondPoint.push_back( point );
							else
								Error( "CExchange::Load_Script() - Invalid CONDITION_POINT Type" );
							s.GetToken();
						} // while - CONDITION_POINT
					}
					else if( s.Token == _T( "REMOVE_POINT" ) )
					{
						s.GetToken(); // {
						s.GetToken();
						while( *s.token != '}' )
						{
							__POINT point;
							point.nType = CScript::GetDefineNum( s.Token );
							point.nPoint = s.GetNumber();	// 차감 포인트
							if( point.nType > 0 )
								Set.vecRemovePoint.push_back( point );
							else
								Error( "CExchange::Load_Script() - Invalid REMOVE_POINT Type" );
							s.GetToken();
						} // while - REMOVE_POINT
					}
#endif // __CAMPUS
					else if( s.Token == _T( "PAY" ) )
					{
						int nSumProb = 0;	// 지급 확률의 합이 1000000인지 확인하기 위해
						BOOL bProb = TRUE;	// 지급 리스트를 추가 할것인지 검사(1000000이 넘고 리스트가 더 있을 경우)
						
						Set.nPayNum = s.GetNumber(); // 지급될 아이템 종류 수
						s.GetToken(); // {
						s.GetToken();
						while( *s.token != '}' )
						{
							__ITEM item;
							item.dwItemId = CScript::GetDefineNum( s.Token ); // 지급 아이템
							item.nItemNum = s.GetNumber(); // 지급 갯수
							item.nPayProb = s.GetNumber(); // 지급 확률
							s.SetMark();
							s.GetToken();
							s.GoMark();
							if( s.tokenType == NUMBER )
								item.byFalg = static_cast<BYTE>( s.GetNumber() );
															
							nSumProb += item.nPayProb;
							if( nSumProb > 1000000 ) // 총 확률의 합이 1000000(100%)가 넘을 경우
							{
								if( bProb )
								{
									item.nPayProb -= nSumProb - 1000000; // 마지막 리스트에 넘는 만큼 뺀다.
									Set.vecPayItem.push_back( item );
									bProb = FALSE;
								#ifdef __WORLDSERVER
									Error( "Exchange_Script.txt -> PAY 확률 합계가 1000000 초과 : %s", strErrorMMIId );
								#endif // __WORLDSERVER
								}
							}
							else
							{
								Set.vecPayItem.push_back( item ); 
								if( nSumProb == 1000000 )
									bProb = FALSE;
							}
							s.GetToken();
						} // while - PAY
						if( nSumProb < 1000000 ) // 총 확률의 합이 1000000 보다 적을 경우
						{
							Set.vecPayItem[Set.vecPayItem.size()-1].nPayProb += 1000000 - nSumProb; // 모자라는 만큼 더한다.
						#ifdef __WORLDSERVER
							Error( "Exchange_Script.txt -> PAY 확률 합계가 1000000 미만 : %s", strErrorMMIId );
						#endif // __WORLDSERVER
						}
					#ifdef __WORLDSERVER
						if( Set.nPayNum > (int)( Set.vecPayItem.size() ) ) // GetPayItemList()에서 처리
							Error( "Exchange_Script.txt -> PAY (리스트 갯수 < 지급 갯수) : %s", strErrorMMIId );
					#endif // __WORLDSERVER
					}
					s.GetToken();
				} // while - SET
				if( nCount <= 15 )
					create.vecSet.push_back( Set ); // SetList에 추가(최대 15개)
			}
			s.GetToken();
		} // while - MMI_ID
		m_mapExchange.insert( map<int, __SETLIST>::value_type(nMMIId, create) );
		s.GetToken(); //get MMI ID
	}
	return TRUE;
}

PSETLIST CExchange::FindExchange( int nMMIId )
{
	map<int, __SETLIST>::iterator it = m_mapExchange.find( nMMIId );
	if( it != m_mapExchange.end() )
		return &it->second;
	return NULL;
}
		
// 리스트에 들어갈 TEXT ID 목록을 얻어온다. vector<int>
vector<int> CExchange::GetListTextId( int nMMIId )
{
	vector<int> list;
	PSETLIST pSetList = FindExchange( nMMIId );
	if( pSetList )
	{
		for( int i=0;i<(int)( pSetList->vecSet.size() ); i++ )
			list.push_back( pSetList->vecSet[i].nSetTextId );
	}
	return list;
}

// 전체 설명 TEXT ID 목록을 얻어온다. vector<int>
vector<int> CExchange::GetDescId( int nMMIId )
{
	PSETLIST pSetList = FindExchange( nMMIId );
	if( pSetList )
		return pSetList->vecDesciprtionId;

	vector<int> vectmp;
	return vectmp;
}

vector<int> CExchange::GetResultMsg( int nMMIId, int nListNum )
{
	PSETLIST pSetList = FindExchange( nMMIId );
	if( pSetList )
		return pSetList->vecSet[nListNum].vecResultMsg;

	vector<int> vectmp;
	return vectmp;
}

#ifdef __WORLDSERVER
// 조건이 만족 하는지 검사
BOOL CExchange::CheckCondition( CUser* pUser, int nMMIId, int nListNum )
{
	PSETLIST pSetList = FindExchange( nMMIId );
	if( !pSetList )
		return FALSE;
	
	if( nListNum > (int)( pSetList->vecSet.size()-1 ) )
		return FALSE;
	
	BOOL bReturn = TRUE;
	vector<__ITEM> vecItem = pSetList->vecSet[nListNum].vecCondItem;
	for( int i=0; i<(int)( vecItem.size() ); i++ )
	{
		int nItemCount = 0;
		if( vecItem[i].dwItemId == II_GOLD_SEED1 )
			nItemCount = pUser->GetGold();
		else
			nItemCount = pUser->GetItemNum( vecItem[i].dwItemId );
		
		if( nItemCount < vecItem[i].nItemNum )
		{
			if( GetResultMsg( nMMIId, nListNum ).size() < 2 )
			{
				ItemProp* pItemProp = prj.GetItemProp( vecItem[i].dwItemId );
				if( pItemProp )
					pUser->AddDefinedText( TID_EXCHANGE_FAIL, "\"%s\" %d", pItemProp->szName, vecItem[i].nItemNum );
			}

			bReturn = FALSE;
		}
	}
#if __VER >= 15 // __CAMPUS
	vector<__POINT> vecPoint = pSetList->vecSet[nListNum].vecCondPoint;
	int nPoint = 0;
	for( i = 0; i < (int)( vecPoint.size() ); ++i )
	{
		nPoint = pUser->GetPointByType( vecPoint[i].nType );
		if( nPoint < vecPoint[i].nPoint )
		{
			if( GetResultMsg( nMMIId, nListNum ).size() < 2 )
			{
				switch( vecPoint[i].nType )
				{
				case POINTTYPE_CAMPUS:
					pUser->AddDefinedText( TID_GAME_EXCHANGE_CAMPUSPOINT_FAIL, "%d", vecPoint[i].nPoint );
					break;

				default:
					break;
				}
			}
			bReturn = FALSE;
		}
	}
#endif // __CAMPUS
	return bReturn;
}

// 확률에 따라 정해진 갯수 만큼 지급될 아이템 목록을 만든다. vector<__ITEM>
vector<CExchange::__ITEM>	CExchange::GetPayItemList( int nMMIId, int nListNum )
{
	vector<__ITEM> vecList;
	PSETLIST pSetList = FindExchange( nMMIId );
	if( !pSetList )
		return vecList;

	if( nListNum > (int)( pSetList->vecSet.size()-1 ) )
		return vecList;

	vector<__ITEM> payItem = pSetList->vecSet[nListNum].vecPayItem;
	int nProb = 1000000;
	int nRandom = xRandom( 1000000 );
	int nSumProb = 0;
	int nCount = 0;

	for( vector<__ITEM>::iterator i = payItem.begin(); i!=payItem.end(); )
	{
		nSumProb += i->nPayProb;
		if( nRandom < nSumProb )
		{
			vecList.push_back(*i); // 지급 목록에 추가
			nCount++;
			if( nCount == pSetList->vecSet[nListNum].nPayNum )
				break;
			nProb -= i->nPayProb;
			if( nProb <= 0 ) // 지급 갯수가 리스트 수보다 많은 경우
				break;
			nRandom = xRandom( nProb ); // 지급된 아이템을 제외한 나머지로 반복
			nSumProb = 0;
			payItem.erase(i);
			i = payItem.begin();
		}
		else
			i++;
	}
	return vecList;
}

// 삭제될 아이템 목록을 얻어온다. map<OBJID, int>
vector<CExchange::__ITEM> CExchange::GetRemoveItemList( int nMMIId, int nListNum )
{
	vector<__ITEM> vecList;
	PSETLIST pSetList = FindExchange( nMMIId );
	if( !pSetList )
		return vecList;
	
	if( nListNum > (int)( pSetList->vecSet.size()-1 ) )
		return vecList;

	vecList = pSetList->vecSet[nListNum].vecRemoveItem;
	return vecList;
}

#if __VER >= 15 // __CAMPUS
vector<CExchange::__POINT> CExchange::GetRemovePointList( int nMMIId, int nListNum )
{
	vector<__POINT> vecList;
	PSETLIST pSetList = FindExchange( nMMIId );
	if( !pSetList )
		return vecList;

	if( nListNum > (int)( pSetList->vecSet.size()-1 ) )
		return vecList;

	vecList = pSetList->vecSet[nListNum].vecRemovePoint;
	return vecList;
}
#endif // __CAMPUS

// 인벤토리에 여유가 있는지 확인
BOOL CExchange::IsFull( CUser* pUser, vector<__ITEM> vecRemoveItem, vector<__ITEM> vecPayItem )
{
	int nEmptyCount = pUser->m_Inventory.GetEmptyCount();
	
	for( int i=0; i<(int)( vecRemoveItem.size() ); i++ )
	{
		int nRemainRemoveItemNum = vecRemoveItem[i].nItemNum;
		for( int j=0; j<pUser->m_Inventory.GetSize(); j++ )
		{
			CItemElem* pItemElem = pUser->m_Inventory.GetAtId( j );
			if( IsUsableItem( pItemElem ) && vecRemoveItem[i].dwItemId == pItemElem->m_dwItemId )
			{
				if( pItemElem->m_nItemNum <= nRemainRemoveItemNum )
					nEmptyCount++;
				nRemainRemoveItemNum -= pItemElem->m_nItemNum;
				if( nRemainRemoveItemNum <= 0 )
					break;
			}
		}
	}

	for( int i=0; i<(int)( vecPayItem.size() ); i++ )
	{
		ItemProp* pItemProp = prj.GetItemProp( vecPayItem[i].dwItemId );
		if( pItemProp )
			nEmptyCount -= (int)(vecPayItem[i].nItemNum / pItemProp->dwPackMax);
	}

	if( nEmptyCount <= 0 )
		return TRUE;

	return FALSE;
}


int CExchange::ResultExchange( CUser* pUser, int nMMIId, int nListNum )
{
	PSETLIST pSetList = FindExchange( nMMIId );
	if( !pSetList )
		return EXCHANGE_FAILED;

	if( nListNum > (int)( pSetList->vecSet.size()-1 ) )
		return EXCHANGE_FAILED;
	
	// 조건 비교
	if( !CheckCondition( pUser, nMMIId, nListNum ) )
		return EXCHANGE_CONDITION_FAILED;
	
	vector<__ITEM> vecRemoveItem = GetRemoveItemList( nMMIId, nListNum );
#if __VER >= 15 // __CAMPUS
	vector<__POINT> vecRemovePoint = GetRemovePointList( nMMIId, nListNum );
#endif // __CAMPUS
	vector<__ITEM> vecPayItem = GetPayItemList( nMMIId, nListNum );
	
	// 인벤토리 공간 검사
	if( IsFull( pUser, vecRemoveItem, vecPayItem ) )
		return EXCHANGE_INVENTORY_FAILED;

	// 아이템 삭제
	for( int i=0; i<(int)( vecRemoveItem.size() ); i++ )
	{
		if( vecRemoveItem[i].dwItemId == II_GOLD_SEED1 )
		{
			pUser->AddGold( -vecRemoveItem[i].nItemNum );
			g_DPSrvr.PutPenyaLog( pUser, "=", "EXCHANGE_REMOVE", vecRemoveItem[i].nItemNum );
		}
		else
		{
			int nItemNum = vecRemoveItem[i].nItemNum;
			if( nItemNum > 0x7fff )
			{
				for( ; nItemNum > 0x7fff; )
				{
					pUser->RemoveItemA( vecRemoveItem[i].dwItemId, 0x7fff );
					nItemNum -= 0x7fff;
				}
			}
			int nRemove = pUser->RemoveItemA( vecRemoveItem[i].dwItemId, nItemNum );
		
			if( nRemove != 0 )
			{
				CItemElem itemElem;
				itemElem.m_dwItemId = vecRemoveItem[i].dwItemId;
				itemElem.m_nItemNum	= vecRemoveItem[i].nItemNum;
				
				LogItemInfo aLogItem;
				aLogItem.Action = "=";
				aLogItem.SendName = pUser->GetName();
				aLogItem.RecvName = "EXCHANGE_REMOVE";
				aLogItem.WorldId = pUser->GetWorld()->GetID();
				aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
				g_DPSrvr.OnLogItem( aLogItem, &itemElem, itemElem.m_nItemNum );	
			}
		}
	}
#if __VER >= 15 // __CAMPUS
	// 포인트 차감
	for( int i = 0; i < (int)( vecRemovePoint.size() ); ++i )
	{
		if( vecRemovePoint[i].nType == POINTTYPE_CAMPUS )
			g_dpDBClient.SendUpdateCampusPoint( pUser->m_idPlayer, vecRemovePoint[i].nPoint, FALSE, 'E' );
	}
#endif // __CAMPUS
	
	// 아이템 지급
	for( int i=0; i<(int)( vecPayItem.size() ); i++ )
	{
		CItemElem itemElem;
		itemElem.m_dwItemId = vecPayItem[i].dwItemId;
		itemElem.m_nItemNum	= vecPayItem[i].nItemNum;
		itemElem.m_byFlag = vecPayItem[i].byFalg;
		itemElem.SetSerialNumber();
		itemElem.m_bCharged = itemElem.GetProp()->bCharged;
		if( pUser->CreateItem( &itemElem ) )
		{
			LogItemInfo aLogItem;
			aLogItem.Action = "=";
			aLogItem.SendName = pUser->GetName();
			aLogItem.RecvName = "EXCHANGE_CREATE";
			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
			g_DPSrvr.OnLogItem( aLogItem, &itemElem, itemElem.m_nItemNum );

			if( GetResultMsg( nMMIId, nListNum ).size() < 2 )
				pUser->AddDefinedText( TID_EXCHANGE_SUCCESS, "\"%s\" %d", itemElem.GetName(), itemElem.m_nItemNum );
		}
		else
			Error( "CExchange::ResultExchange() 아이템 지급 실패 - %s, %s, %d, %d", pUser->GetName(), itemElem.GetName(), itemElem.m_dwItemId, itemElem.m_nItemNum );
	}
	return EXCHANGE_SUCCESS;
}
#endif // __WORLDSERVER

#endif // __TRADESYS