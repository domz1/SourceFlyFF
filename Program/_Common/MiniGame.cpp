// MiniGame.cpp: implementation of the CMiniGame class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MiniGame.h"
#ifdef __WORLDSERVER
#include "User.h"
#include "DPSrvr.h"
#include "dpdatabaseclient.h"
#endif // __WORLDSERVER
#include "defineText.h"

#ifdef __WORLDSERVER
extern	CDPSrvr		g_DPSrvr;
extern	CDPDatabaseClient	g_dpDBClient;
#endif // __WORLDSERVER

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#ifdef __EVE_MINIGAME
CMiniGame::CMiniGame()
{
	__KAWIBAWIBORESULT KawibawiboResult;
	// 가위 기준
	KawibawiboResult.nMy = KAWI;
	KawibawiboResult.nYou	= BAWI;
	KawibawiboResult.nResult = KAWIBAWIBO_LOST;
	m_vecKawibawiboResult.push_back( KawibawiboResult );
	KawibawiboResult.nYou	= BO;
	KawibawiboResult.nResult = KAWIBAWIBO_WIN;
	m_vecKawibawiboResult.push_back( KawibawiboResult );

	// 바위 기준
	KawibawiboResult.nMy = BAWI;
	KawibawiboResult.nYou	= KAWI;
	KawibawiboResult.nResult = KAWIBAWIBO_WIN;
	m_vecKawibawiboResult.push_back( KawibawiboResult );
	KawibawiboResult.nYou	= BO;
	KawibawiboResult.nResult = KAWIBAWIBO_LOST;
	m_vecKawibawiboResult.push_back( KawibawiboResult );

	// 보 기준
	KawibawiboResult.nMy = BO;
	KawibawiboResult.nYou	= KAWI;
	KawibawiboResult.nResult = KAWIBAWIBO_LOST;
	m_vecKawibawiboResult.push_back( KawibawiboResult );
	KawibawiboResult.nYou	= BAWI;
	KawibawiboResult.nResult = KAWIBAWIBO_WIN;
	m_vecKawibawiboResult.push_back( KawibawiboResult );
	
	m_nBetMinPenya = 1000000;
	m_nBetMaxPenya = 100000000;
	m_nMultiple = 5;
}

CMiniGame::~CMiniGame()
{

}

// A~Z 카드를 주면 카드에 맞는 문자열을 리턴
char CMiniGame::GetCharacter( DWORD ItemId )
{
	int nchar = 0;
	if( II_SYS_SYS_EVE_A_CARD <= ItemId && ItemId <= II_SYS_SYS_EVE_Z_CARD )
		nchar = ItemId - II_SYS_SYS_EVE_A_CARD;

	return ('A' + nchar);
}

#ifdef __WORLDSERVER
int CMiniGame::Result_Kawibawibo( CUser* pUser )
{
	CItemElem* pItemElem;
	if( pUser->m_nKawibawiboState != KAWIBAWIBO_DRAW && pUser->m_nKawibawiboState != KAWIBAWIBO_WIN )
	{
		pItemElem	= (CItemElem*)pUser->m_Inventory.GetAtItemId( II_SYS_SYS_EVE_KAWIBAWIBO );
		if( IsUsableItem( pItemElem ) == FALSE )
			return KAWIBAWIBO_FAILED;
	}

	__KAWIBAWIBO Kawibawibo = FindKawibawibo( pUser->m_nKawibawiboWin );

	if( Kawibawibo.nItemCount != 0 )	// 아이템 갯수는 0일수 없음
	{
		if( pUser->m_nKawibawiboState != KAWIBAWIBO_DRAW && pUser->m_nKawibawiboState != KAWIBAWIBO_WIN )
		{
			LogItemInfo aLogItem;
			aLogItem.Action = "+";
			aLogItem.SendName = pUser->GetName();
			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
			aLogItem.RecvName = "KAWIBAWIBO_USE_ITEM";
			g_DPSrvr.OnLogItem( aLogItem, pItemElem, 1 );

			pUser->RemoveItem( (BYTE)( pItemElem->m_dwObjId ), (short)1 );		// 가위바위보 쿠폰 아이템 삭제
		}

		// 연승 최대치를 넘었을 때 패 처리
		if( pUser->m_nKawibawiboWin >= nKawiBawiBo_MaxWin )
		{
			pUser->m_nKawibawiboWin = 0;
			LogItemInfo aLogItem;
			aLogItem.Action = "+";
			aLogItem.SendName = pUser->GetName();
			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
			aLogItem.RecvName = "KAWIBAWIBO_LOST";
			g_DPSrvr.OnLogItem( aLogItem );
			return KAWIBAWIBO_LOST;
		}

		int nRandom = xRandom( 1000000 );
		// 비겼을 때
		if( (DWORD)( nRandom ) < Kawibawibo.dwDraw )	// Draw
		{
			LogItemInfo aLogItem;
			aLogItem.Action = "+";
			aLogItem.SendName = pUser->GetName();
			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
			aLogItem.RecvName = "KAWIBAWIBO_DRAW";
			g_DPSrvr.OnLogItem( aLogItem );
			return KAWIBAWIBO_DRAW;
		}
		// 졌을 때
		else if( (DWORD)( nRandom ) < Kawibawibo.dwDraw + Kawibawibo.dwLost ) // Lost
		{
			pUser->m_nKawibawiboWin = 0;
			LogItemInfo aLogItem;
			aLogItem.Action = "+";
			aLogItem.SendName = pUser->GetName();
			aLogItem.WorldId = pUser->GetWorld()->GetID();
			aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
			aLogItem.RecvName = "KAWIBAWIBO_LOST";
			g_DPSrvr.OnLogItem( aLogItem );
			return KAWIBAWIBO_LOST;
		}
		// 이겼을 때
		++pUser->m_nKawibawiboWin;
		LogItemInfo aLogItem;
		aLogItem.Action = "+";
		aLogItem.SendName = pUser->GetName();
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
		aLogItem.RecvName = "KAWIBAWIBO_WIN";
		aLogItem.Gold_1 = pUser->m_nKawibawiboWin;
		g_DPSrvr.OnLogItem( aLogItem );
		return KAWIBAWIBO_WIN;
	}
	return KAWIBAWIBO_FAILED;
}

BOOL CMiniGame::ResultItem_Kawibawibo( CUser* pUser )
{
	if( 0 >= pUser->m_nKawibawiboWin )
		return FALSE;
	
	__KAWIBAWIBO Kawibawibo = FindKawibawibo( pUser->m_nKawibawiboWin );
	if( Kawibawibo.nItemCount != 0 )	// 아이템 갯수는 0일수 없음
	{
		LogItemInfo aLogItem;
		aLogItem.Gold_1 = pUser->m_nKawibawiboWin;

		pUser->m_nKawibawiboWin = 0;
		pUser->m_nKawibawiboState = KAWIBAWIBO_GETITEM;

		CItemElem itemElem;
		itemElem.m_dwItemId = Kawibawibo.dwItemId;
		itemElem.m_nItemNum		= Kawibawibo.nItemCount;
		itemElem.SetSerialNumber();
		ItemProp* pItemProp		= itemElem.GetProp();
		if( pItemProp )
			itemElem.m_nHitPoint	= pItemProp->dwEndurance;
		else
			itemElem.m_nHitPoint	= 0;
		itemElem.m_bCharged = pItemProp->bCharged;

		if( pUser->CreateItem( &itemElem ) )
		{
			aLogItem.RecvName = "KAWIBAWIBO_PRIZE";
			pUser->AddKawibawiboResult( KAWIBAWIBO_GETITEM, aLogItem.Gold_1, Kawibawibo.dwItemId, Kawibawibo.nItemCount );		
		}
		else
		{
			aLogItem.RecvName = "KAWIBAWIBO_PRIZE_POST";
			g_dpDBClient.SendQueryPostMail( pUser->m_idPlayer, 0, itemElem, 0, itemElem.GetProp()->szName, (char*)GETTEXT( TID_MMI_KAWIBAWIBO ) );
			pUser->AddDefinedText( TID_GAME_MINIGAME_ITEM_POST, "" );
		}
		aLogItem.Action = "+";
		aLogItem.SendName = pUser->GetName();
		aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		g_DPSrvr.OnLogItem( aLogItem, &itemElem, Kawibawibo.nItemCount );
	}
	return TRUE;
}

void CMiniGame::OpenWnd_Reassemble( CUser* pUser )
{
	pUser->AddReassembleOpenWnd( m_vecReassembleCard );
}

BOOL CMiniGame::Result_Reassemble( CUser* pUser, OBJID* objItemId, int nCount )
{
	CItemElem* pItemElem[9];
	// 일치하는지 검사 (인벤토리에서 검사)
	for( DWORD i=0; i<m_vecReassembleCard.size(); ++i )
	{
		pItemElem[i]	= (CItemElem*)pUser->m_Inventory.GetAtId( objItemId[i] );
		if( IsUsableItem( pItemElem[i] ) == FALSE )
			return FALSE;

		if( pItemElem[i]->GetProp()->dwID != m_vecReassembleCard[i] )
			return FALSE;
	}

	// 모두 일치하면 카드 아이템 삭제
	for( int i=0; i<nCount ; ++i )
	{
		LogItemInfo aLogItem;
		aLogItem.Action = "+";
		aLogItem.SendName = pUser->GetName();
		aLogItem.RecvName = "REASSEMBLE_USE";
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		g_DPSrvr.OnLogItem( aLogItem, pItemElem[i], 1 );

		pUser->RemoveItem( (BYTE)( objItemId[i] ), (short)1 );
	}
	return TRUE;
}
BOOL CMiniGame::ResultItem_Reassemble( CUser* pUser )
{
	// 아이템 지급 확률에 따라 아이템을 얻어온다.
	__ITEMGAME ItemGame = GetReassembleRandomItem();
	
	CItemElem itemElem;
	itemElem.m_dwItemId = ItemGame.dwItemId;
	itemElem.m_nItemNum	= ItemGame.nItemCount;
	itemElem.SetSerialNumber();
	ItemProp* pItemProp		= itemElem.GetProp();
	if( pItemProp )
		itemElem.m_nHitPoint	= pItemProp->dwEndurance;
	else
		itemElem.m_nHitPoint	= 0;
	itemElem.m_bCharged = pItemProp->bCharged;

	LogItemInfo aLogItem;
	if( pUser->CreateItem( &itemElem ) )
	{
		aLogItem.RecvName = "REASSEMBLE_PRIZE";
		pUser->AddReassembleResult( ItemGame.dwItemId, ItemGame.nItemCount );
	}
	else
	{
		aLogItem.RecvName = "REASSEMBLE_PRIZE_POST";
		g_dpDBClient.SendQueryPostMail( pUser->m_idPlayer, 0, itemElem, 0, itemElem.GetProp()->szName, (char*)GETTEXT( TID_MMI_REASSEMBLE ) );
		pUser->AddDefinedText( TID_GAME_MINIGAME_ITEM_POST, "" );
	}
	aLogItem.Action = "+";
	aLogItem.SendName = pUser->GetName();
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	g_DPSrvr.OnLogItem( aLogItem, &itemElem, ItemGame.nItemCount );

	return TRUE;
}

CMiniGame::__ITEMGAME CMiniGame::GetReassembleRandomItem()
{
	int nRandom = xRandom( 1000000 );
	int nSum = 0;

	__ITEMGAME temp;
	for(int i=0; m_vecReassembleItem.size(); ++i )
	{
		temp = m_vecReassembleItem[i];
		nSum += temp.dwPer;
		if( nRandom < nSum )
			return temp;
	}

	return m_vecReassembleItem[0];
}

int CMiniGame::Result_Alphabet( CUser* pUser, OBJID* objItemId, int nCount, int nQuestionID )
{
	// 페냐 검사..
	if( pUser->GetGold() < m_nPayPenya )
		return ALPHABET_NOTENOUGH_MONEY;

	// 일치하는지 검사 (인벤토리에서 검사)
	if( nQuestionID < 0 || nQuestionID >= (int)( m_vecAlphabet.size() ) )
		return ALPHABET_FAILED;
	
	CString strWord;
	CItemElem* pItemElem;
	map<OBJID, int> map_Item;
	for( int i=0; i<nCount; ++i )
	{
		pItemElem	= (CItemElem*)pUser->m_Inventory.GetAtId( objItemId[i] );
		if( !IsUsableItem( pItemElem ) )
			return ALPHABET_FAILED;
		
		map<OBJID, int>::iterator it = map_Item.find( objItemId[i] );
		if( it == map_Item.end() )
			it = map_Item.insert( make_pair( objItemId[i], 1 ) ).first;
		else
			it->second++;

		if( pItemElem->m_nItemNum < it->second )
		{
			Error( "ItemNum - %d, %d", pItemElem->m_nItemNum, it->second );
			return ALPHABET_FAILED;
		}
	
		char chAlphabet = GetCharacter( pItemElem->GetProp()->dwID );
		strWord += chAlphabet;
	}

	pUser->AddGold( -m_nPayPenya );	// 페냐 지불
	//g_DPSrvr.PutPenyaLog( pUser, "+", "ALPHABET_USE", m_nPayPenya );
	// 사용된 알파벳 아이템 삭제
	LogItemInfo aLogItem;
	aLogItem.Action = "+";
	aLogItem.SendName = pUser->GetName();
	char szTemp[32] = {0,};
	sprintf( szTemp, "ALPHABET_USE_%s", strWord );
	aLogItem.RecvName = szTemp;
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = pUser->GetGold() + m_nPayPenya;
	aLogItem.Gold2 = pUser->GetGold();
	aLogItem.Gold_1 = -m_nPayPenya;
	g_DPSrvr.OnLogItem( aLogItem );
	
	for( i=0; i<nCount; ++i )
	{
		/*
		LogItemInfo aLogItem;
		aLogItem.Action = "+";
		aLogItem.SendName = pUser->GetName();
		aLogItem.RecvName = "ALPHABET_USE";
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		aLogItem.Gold = pUser->GetGold() + m_nPayPenya;
		aLogItem.Gold2 = pUser->GetGold();
		aLogItem.Gold_1 = -m_nPayPenya;
		g_DPSrvr.OnLogItem( aLogItem, pItemElem[i], 1 );
		*/
		
		pUser->RemoveItem( (BYTE)( objItemId[i] ), (short)1 );
	}
	
	// 단어가 정답인지 확인
	__ALPHABET temp = m_vecAlphabet[nQuestionID];
	if( strWord == temp.strWord )
		return nQuestionID;
	
	return ALPHABET_FAILED;
}

char CMiniGame::GetPublicCharacter( int nQuestionNum, int nPublicAlphabet )
{
	return m_vecAlphabet[nQuestionNum].strWord[nPublicAlphabet-1];
}

CMiniGame::__ITEMGAME CMiniGame::GetAlphabetRandomItem( int nNum )
{
	int nRandom = xRandom( 1000000 );
	int nSum = 0;
	__ALPHABET alphabet;

	if( (int)( m_vecAlphabet.size() ) > nNum && nNum >= 0 )
	{
		alphabet = m_vecAlphabet[nNum];
		__ITEMGAME temp;
		for( int i=0; alphabet.vecItem.size(); ++i )
		{
			temp = alphabet.vecItem[i];
			nSum += temp.dwPer;
			if( nRandom < nSum )
				return temp;
		}
	}		
	return alphabet.vecItem[0];
}

void CMiniGame::OpenWnd_Alphabet( CUser* pUser )
{
	int nRandom = xRandom( m_vecAlphabet.size() );
	int nPublicAlphabet = m_vecAlphabet[nRandom].nPublic;
	pUser->AddAlphabetOpenWnd( nRandom, nPublicAlphabet-1, GetPublicCharacter( nRandom, nPublicAlphabet ), 
								m_vecAlphabet[nRandom].strQuestion );
}

BOOL CMiniGame::ResultItem_Alphabet( CUser* pUser, int nNum )
{
	// 아이템 지급 확률에 따라 아이템을 얻어온다.
	__ITEMGAME ItemGame = GetAlphabetRandomItem( nNum );
	
	CItemElem itemElem;
	itemElem.m_dwItemId = ItemGame.dwItemId;
	itemElem.m_nItemNum	= ItemGame.nItemCount;
	itemElem.SetSerialNumber();
	ItemProp* pItemProp		= itemElem.GetProp();
	if( pItemProp )
		itemElem.m_nHitPoint	= pItemProp->dwEndurance;
	else
		itemElem.m_nHitPoint	= 0;
	itemElem.m_bCharged = pItemProp->bCharged;
	
	LogItemInfo aLogItem;
	if( pUser->CreateItem( &itemElem ) )
	{
		aLogItem.RecvName = "ALPABET_PRIZE";
		pUser->AddDefinedText( TID_GAME_REAPITEM, "\"%s\"", pItemProp->szName );
		pUser->AddAlphabetResult( nNum, ItemGame.dwItemId, ItemGame.nItemCount );
	}
	else
	{
		aLogItem.RecvName = "ALPABET_PRIZE_POST";
		g_dpDBClient.SendQueryPostMail( pUser->m_idPlayer, 0, itemElem, 0, itemElem.GetProp()->szName, (char*)GETTEXT( TID_MMI_FINDWORD ) );
		pUser->AddDefinedText( TID_GAME_MINIGAME_ITEM_POST, "" );
	}
	aLogItem.Action = "+";
	aLogItem.SendName = pUser->GetName();
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	g_DPSrvr.OnLogItem( aLogItem, &itemElem, ItemGame.nItemCount );
	
	return TRUE;
}

void CMiniGame::OpenWnd_FiveSystem( CUser* pUser )
{
	// pUser->m_nBetFiveSystem[]를 초기화 할 필요가 있음.
	for(int i=0; i<6; ++i )
		pUser->m_nBetFiveSystem[i] = 0;
	
	pUser->AddFiveSystemOpenWnd( m_nBetMinPenya, m_nBetMaxPenya, m_nMultiple );
}

void CMiniGame::DestroyWnd_FiveSystem( CUser* pUser )
{
	int nPenya = 0;
	LogItemInfo aLogItem;

	for(int i=0; i<6; ++i )
	{
		nPenya += pUser->m_nBetFiveSystem[i];
		pUser->m_nBetFiveSystem[i] = 0;
	}
	
	// 입찰된 금액 없이 종료할 때(로그 X)
	if( nPenya <= 0 )
		return;
	
	int nTotal = pUser->GetGold() + nPenya;
	if( nTotal < 0 )		// overflow?
	{
		aLogItem.RecvName = "FIVESYSTEM_CANCEL_POST";
		CItemElem ItemElem;
		g_dpDBClient.SendQueryPostMail( pUser->m_idPlayer, 0, ItemElem, nPenya, (char*)GETTEXT( TID_GAME_SELLING_COST_0 ), (char*)GETTEXT( TID_MMI_FIVESYSTEM ) );
		pUser->AddDefinedText( TID_GAME_MINIGAME_PENYA_POST, "" );
	}
	else
	{
		aLogItem.RecvName = "FIVESYSTEM_CANCEL";
		pUser->AddGold( nPenya );
	}

	aLogItem.Action = "+";
	aLogItem.SendName = pUser->GetName();
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = pUser->GetGold() - nPenya;
	aLogItem.Gold2 = pUser->GetGold();
	//aLogItem.ItemName = "SEED";
	_stprintf( aLogItem.szItemName, "%d", II_GOLD_SEED1 );
	aLogItem.itemNumber = nPenya;
	g_DPSrvr.OnLogItem( aLogItem );	
}

int CMiniGame::Bet_FiveSystem( CUser* pUser, int nBetNum, int nBetPenya )
{
	// 입찰 번호에 이전 값이 0이고 현재 입찰금이 0일때 아무일도 안함(로그 X)
	if( pUser->m_nBetFiveSystem[nBetNum] == 0 && nBetPenya == 0 )
		return TRUE;
	// 잘못된 입찰 번호가 들어 왔을 때
	if( nBetNum < 0 || 5 < nBetNum )
		return FIVESYSTEM_FAILED;
	// 입찰금은 0보다 작을 수 없다.
	if(nBetPenya < 0)
		return FIVESYSTEM_FAILED;

	// 최소 입찰금과 최대 입찰금의 범위를 벗어났을 경우 ( Client 에서 이미 검사.. )
	if( ( nBetPenya < m_nBetMinPenya || nBetPenya > m_nBetMaxPenya ) && nBetPenya != 0 )
		return FIVESYSTEM_NOTENOUGH;
	
	// 입찰 또는 재입찰 금액이 부족한지 검사
	DWORD nGold = pUser->GetGold() + pUser->m_nBetFiveSystem[nBetNum];
	if( (int)( nGold ) < nBetPenya )
		return FIVESYSTEM_NOTENOUGH;

	LogItemInfo aLogItem;
	aLogItem.RecvName = "FIVESYSTEM_USE";
	BOOL bAddGold = TRUE;
	if( 0 < pUser->m_nBetFiveSystem[nBetNum] - nBetPenya )		// 기존에 걸은것보다 적게 걸아서 보상금액
	{
		int nTotal = pUser->GetGold() + pUser->m_nBetFiveSystem[nBetNum] - nBetPenya;
		if( nTotal < 0 )		// overflow?
		{
			CItemElem ItemElem;
			g_dpDBClient.SendQueryPostMail( pUser->m_idPlayer, 0, ItemElem, pUser->m_nBetFiveSystem[nBetNum] - nBetPenya, (char*)GETTEXT( TID_GAME_SELLING_COST_0 ), (char*)GETTEXT( TID_MMI_FIVESYSTEM ) );
			pUser->AddDefinedText( TID_GAME_MINIGAME_PENYA_POST, "" );
			aLogItem.RecvName = "FIVESYSTEM_USE_POST";
			bAddGold = FALSE;
		}
	}
	
	if( bAddGold )
		pUser->AddGold( pUser->m_nBetFiveSystem[nBetNum] - nBetPenya );

	aLogItem.Action = "+";
	aLogItem.SendName = pUser->GetName();
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = pUser->GetGold() - ( pUser->m_nBetFiveSystem[nBetNum] - nBetPenya );
	aLogItem.Gold2 = pUser->GetGold();
	//aLogItem.ItemName = "SEED";
	_stprintf( aLogItem.szItemName, "%d", II_GOLD_SEED1 );
	aLogItem.itemNumber = pUser->m_nBetFiveSystem[nBetNum] - nBetPenya;
	g_DPSrvr.OnLogItem( aLogItem );

	pUser->m_nBetFiveSystem[nBetNum] = nBetPenya;
	pUser->AddFiveSystemResult( FIVESYSTEM_BETOK, nBetPenya, nBetNum );

	return TRUE;
}
int CMiniGame::Result_FiveSystem( CUser* pUser )
{
	// 최고 입찰금 및 갯수
	int nMaxBet = 0;
	int nMaxCount = 0;

	for( int i=0; i<6; ++i )
	{
		if( pUser->m_nBetFiveSystem[i] > nMaxBet )
		{
			nMaxBet = pUser->m_nBetFiveSystem[i];
			nMaxCount = 1;
		}
		else if( pUser->m_nBetFiveSystem[i] == nMaxBet )
			++nMaxCount;
	}
		
	if( nMaxCount == 6 )	nMaxCount = 0;

	map<int, __FIVESYSTEM>::iterator it = m_mapFiveSystem.find( nMaxCount );
	if( it == m_mapFiveSystem.end() )	return 0;
	
	__FIVESYSTEM prob = it->second;

	int nRandom = xRandom( 1000000 );
	int nSum = 0;

	for( i=0; i<6; ++i )
	{
		if( pUser->m_nBetFiveSystem[i] == nMaxBet )
			nSum += prob.dwMin;
		else
			nSum += prob.dwMax;

		if( nSum > nRandom )
			return i;
	}

	return 0;
}

BOOL CMiniGame::ResultPenya_FiveSystem( CUser* pUser, int nResult )
{
	int SetPenya = pUser->m_nBetFiveSystem[nResult] * m_nMultiple;
	
	// 입찰금 초기화
	for( int i=0; i<6; ++i )
		pUser->m_nBetFiveSystem[i] = 0;

	LogItemInfo aLogItem;
	aLogItem.RecvName = "FIVESYSTEM_PRIZE";

	int nTotal = pUser->GetGold() + SetPenya;
	if( nTotal < 0 )		// overflow?
	{
		CItemElem ItemElem;
		g_dpDBClient.SendQueryPostMail( pUser->m_idPlayer, 0, ItemElem, SetPenya, (char*)GETTEXT( TID_GAME_SELLING_COST_0 ), (char*)GETTEXT( TID_MMI_FIVESYSTEM ) );
		pUser->AddDefinedText( TID_GAME_MINIGAME_PENYA_POST, "" );
		aLogItem.RecvName = "FIVESYSTEM_PRIZE_POST";
	}
	else
		pUser->AddGold( SetPenya );

	aLogItem.Action = "+";
	aLogItem.SendName = pUser->GetName();
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = pUser->GetGold() - SetPenya;
	aLogItem.Gold2 = pUser->GetGold();
	//aLogItem.ItemName = "SEED";
	_stprintf( aLogItem.szItemName, "%d", II_GOLD_SEED1 );
	aLogItem.itemNumber = SetPenya;
	g_DPSrvr.OnLogItem( aLogItem );
	
	pUser->AddFiveSystemResult( nResult, SetPenya );
	
	return TRUE;
}

				
CMiniGame::__KAWIBAWIBO CMiniGame::FindKawibawibo( int nWinCount )
{
	__KAWIBAWIBO Kawibawibo;
	int nUserWin = nWinCount;
	if( 0 <= nUserWin )
	{
		if( nKawiBawiBo_MaxWin < nUserWin )
			nUserWin = nKawiBawiBo_MaxWin;
		
		map<int, __KAWIBAWIBO>::iterator it = m_mapKawiBawiBo.find( nUserWin );
		if( it != m_mapKawiBawiBo.end() )
			Kawibawibo = it->second;
	}
	
	return Kawibawibo;	
}
#endif // __WORLDSERVER

int CMiniGame::GetKawibawiboYou( int nMy, int nResult )
{
	for( DWORD i = 0 ; i < m_vecKawibawiboResult.size() ; ++i )
	{
		if( nMy == m_vecKawibawiboResult[i].nMy && nResult == m_vecKawibawiboResult[i].nResult )
			return m_vecKawibawiboResult[i].nYou;
	}

	return nMy;
}

BOOL CMiniGame::Load_Alphabet()
{
	CScanner s;
	if( !s.Load( "MiniGame_Alphabet.txt.txt" ) )
		return FALSE;

	map< int, CString > mapQuestion;
	s.GetToken();
	while( s.tok != FINISHED )
	{
		int nID = atoi( s.Token.Right(4) );
		s.GetLastFull();
		CString strQuestion;
		strQuestion = s.Token;

		mapQuestion.insert( make_pair( nID, strQuestion ) );
		s.GetToken();
	}

	s.Free();
	
	if( !s.Load( "MiniGame_Alphabet.txt", 0 ) )
		return FALSE;

	s.GetToken();

	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "PENYA" ) )
		{
			m_nPayPenya  = s.GetNumber();
		}
		else if( s.Token == _T( "WORD" ) )
		{
			s.GetToken();

			__ALPHABET	Alpabet;
			Alpabet.strWord = s.Token;
			Alpabet.strWord.MakeUpper();
			Alpabet.nPublic = s.GetNumber();
			s.GetLastFull();
			map< int, CString >::iterator it = mapQuestion.find( atoi( s.Token.Right(4) ) );
			if( it != mapQuestion.end() )
				Alpabet.strQuestion = it->second;
			else
				Error( "존재하지 않는 문제 ID -> %s", s.Token );
			
			s.GetToken();	// {
			s.GetToken();
			while( *s.token != '}' )
			{
				__ITEMGAME	ItemGame;
				ItemGame.dwItemId = CScript::GetDefineNum( s.Token );
				if( ItemGame.dwItemId == -1 )
				{
					Error( "CMiniGame::Load_Alphabet() 아이템이 없는것을 설정했음" );
					return FALSE;
				}
				ItemGame.nItemCount = s.GetNumber();
				ItemGame.dwPer = s.GetNumber();
				Alpabet.vecItem.push_back( ItemGame );
				s.GetToken();
			}
			m_vecAlphabet.push_back( Alpabet );
		}
		s.GetToken();
	}	
	return TRUE;
}


BOOL CMiniGame::Load_KawiBawiBo()
{
	CScanner s;
	
	if( !s.Load( "MiniGame_KawiBawiBo.txt", 0 ) )
		return FALSE;
	
	s.GetToken();
	
	int nMaxWin = 0;
	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "SENDITEM" ))
		{
			__KAWIBAWIBO	KawiBawiBo;
		
			int nWinCount = s.GetNumber();
			KawiBawiBo.dwDraw	= s.GetNumber();
			KawiBawiBo.dwLost	= s.GetNumber();
			KawiBawiBo.dwWin	= s.GetNumber();
			s.GetToken();
			KawiBawiBo.dwItemId = CScript::GetDefineNum( s.Token );
			if( KawiBawiBo.dwItemId == -1 )
			{
				Error( "CMiniGame::Load_KawiBawiBo() 아이템이 없는것을 설정했음" );
				return FALSE;
			}

			KawiBawiBo.nItemCount = s.GetNumber();

			m_mapKawiBawiBo.insert( make_pair( nWinCount, KawiBawiBo) );
//			m_mapKawiBawiBo.insert( map<int, __KAWIBAWIBO>::value_type( nWinCount, KawiBawiBo ) );

			if( nMaxWin < nWinCount )
				nMaxWin = nWinCount;
		}
		s.GetToken();
	}	

	nKawiBawiBo_MaxWin = nMaxWin;
	return TRUE;
}

BOOL CMiniGame::Load_FiveSystem()
{
	CScanner s;
	
	if( !s.Load( "MiniGame_FiveSystem.txt", 0 ) )
		return FALSE;

	s.GetToken();
	
	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "MINPENYA" ) )
		{
			m_nBetMinPenya = s.GetNumber();
		}

		else if( s.Token == _T( "MAXPENYA" ) )
		{
			m_nBetMaxPenya = s.GetNumber();
		}
		else if( s.Token == _T( "PERCENT" ) )
		{
			__FIVESYSTEM	FiveSystem;

			int nCount = s.GetNumber();
			FiveSystem.dwMin = s.GetNumber();
			FiveSystem.dwMax = s.GetNumber();

			m_mapFiveSystem.insert( map<int, __FIVESYSTEM>::value_type( nCount, FiveSystem ) );
		}
		else if( s.Token == _T( "MULTIPLE" ) )
		{
			m_nMultiple = s.GetNumber();
		}

		s.GetToken();
	}	
	return TRUE;

}


BOOL CMiniGame::Load_Reassemble()
{
	CScanner s;
	
	if( !s.Load( "MiniGame_Reassemble.txt", 0 ) )
		return FALSE;

	s.GetToken();
	
	
	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "CARD" ) )
		{
			s.GetToken();	// {
			s.GetToken();
			while( *s.token != '}' )
			{
				DWORD dwCard = CScript::GetDefineNum( s.Token );
				if( dwCard == -1 )
				{
					Error( "CMiniGame::Load_Reassemble() \"CARD\" 아이템이 없는것을 설정했음" );
					return FALSE;
				}
				// 9개 이상 안들어 가도록..
				if( m_vecReassembleCard.size() <= 9 )
					m_vecReassembleCard.push_back( dwCard );
		
				s.GetToken();
			}
		}
		else if( s.Token == _T( "ITEM" ) )
		{
			s.GetToken();	// {
			s.GetToken();	// {
			while( *s.token != '}' )
			{
				__ITEMGAME	ItemGame;
				ItemGame.dwItemId = CScript::GetDefineNum( s.Token );
				if( ItemGame.dwItemId == -1 )
				{
					Error( "CMiniGame::Load_Reassemble() \"ITEM\" 아이템이 없는것을 설정했음" );
					return FALSE;
				}
				ItemGame.nItemCount = s.GetNumber();
				ItemGame.dwPer = s.GetNumber();
				m_vecReassembleItem.push_back( ItemGame );
				s.GetToken();
			}
		}
		s.GetToken();
	}	
	return TRUE;
}

#endif // __EVE_MINIGAME