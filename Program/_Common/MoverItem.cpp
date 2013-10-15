#include "stdafx.h"
#include "mover.h"
#include "defineText.h"
#ifdef __WORLDSERVER
#include "user.h"
extern	CUserMng			g_UserMng;
#endif	// WORLDSERVER


// pc, npc의 분리 
// m_nCost삭제 

// 거래 상대방을 얻는다.
CMover* CVTInfo::GetOther()
{
//	return m_pOther;
	if( m_objId == NULL_ID )
		return NULL;

	return prj.GetMover( m_objId );
}

// 거래 상대방을 정한다.
void CVTInfo::SetOther( CMover* pMover )
{
//	m_pOther = pMover;
	if( pMover )
	{
		m_objId = pMover->GetId();
	}
	else
	{
		m_objId = NULL_ID;
	}
}

CItemBase* CVTInfo::GetItem( BYTE i )
{
	return m_apItem_VT[i];
}

void CVTInfo::SetItem( BYTE i, CItemBase* pItemBase )
{
	m_apItem_VT[i] = pItemBase;
}

LPCTSTR	CVTInfo::GetTitle()
{
	return m_strTitle.c_str();
}

void CVTInfo::SetTitle( LPCTSTR szTitle )
{
	m_strTitle = szTitle;
}

BOOL CVTInfo::IsVendorOpen()
{
	return (m_strTitle.empty() != true);
}

void CVTInfo::Init( CMover* pOwner )
{
	m_pOwner = pOwner;
	ZeroMemory( m_apItem_VT, sizeof( m_apItem_VT ) );
	TradeSetGold( 0 );							// raiders.2006.11.28 
	TradeClear();
	m_strTitle = "";
}




void CVTInfo::TradeClear()
{
	SetOther( NULL );
	for( int i = 0; i < MAX_TRADE; i++ )
	{
		if( m_apItem_VT[i] )
		{
			m_apItem_VT[i]->SetExtra( 0 );
			m_apItem_VT[i] = NULL;
		}
	}
#ifdef __WORLDSERVER	
	// raiders.2006.11.28	인벤돈 = 인벤돈 + 내 거래창 돈
	int nGold = TradeGetGold();
	if( nGold > 0 && m_pOwner )
		m_pOwner->AddGold( nGold );
	//
#endif

	TradeSetGold( 0 );
	TradeSetState( TRADE_STEP_ITEM );
}

void CVTInfo::TradeSetGold( DWORD dwGold )
{
	m_dwTradeGold = dwGold;
}

int CVTInfo::TradeGetGold()
{
	return m_dwTradeGold;
}

void CVTInfo::TradeSetItem( BYTE nId, BYTE i, short nItemNum )
{
	CItemBase* pItemBase = m_pOwner->GetItemId( nId );
	if( pItemBase )
	{
		m_apItem_VT[i] = pItemBase;
		pItemBase->SetExtra( nItemNum );
	}
}

BOOL CVTInfo::TradeClearItem( BYTE i )
{
	CItemBase* pItemBase = m_apItem_VT[i];
	if( IsUsingItem( pItemBase ) )
	{
		pItemBase->SetExtra( 0 );		// clear - using flag 
		m_apItem_VT[i] = NULL;

		return TRUE;
	}
	else
		return FALSE;
}

//raiders.2006.11.28  계산과정 변경 ( 인벤돈 = 인벤돈 + 상대방 거래창 돈 )
BOOL CVTInfo::TradeConsent()
{
	CMover* pTrader	= GetOther();
	if( pTrader == NULL )
		return FALSE;

	int cbI	= 0, cbYou	= 0;
	CItemContainer<CItemElem> a;
	a.SetItemContainer( ITYPE_ITEM, MAX_TRADE );
			
	CItemBase* pItemBase;
	for( int i = 0; i < MAX_TRADE; i++ )
	{
		pItemBase = m_apItem_VT[i];
		if( !pItemBase )
			continue;

		m_apItem_VT[i] = NULL;
		CItemElem* pItemElem = ( CItemElem* )pItemBase;
		if( pItemElem->GetProp()->dwPackMax > 1 )
		{
			short nTradeNum = pItemElem->m_nItemNum - (short)pItemBase->GetExtra();
			pItemElem->m_nItemNum = pItemBase->GetExtra();
			a.Add( pItemElem );
			pItemElem->m_nItemNum = nTradeNum;
			pItemElem->SetExtra( 0 );
			if( nTradeNum == 0 )
				m_pOwner->m_Inventory.RemoveAtId( pItemBase->m_dwObjId );	// 제거
		}
		else
		{
			a.Add( pItemElem );
			m_pOwner->m_Inventory.RemoveAtId( pItemBase->m_dwObjId );
		}
	}
		
	for( i = 0; i < MAX_TRADE; i++ )
	{
		pItemBase = pTrader->m_vtInfo.GetItem( i );
		if( pItemBase == NULL )
			continue;

		pTrader->m_vtInfo.SetItem( i, NULL );
		CItemElem* pItemElem = ( CItemElem* )pItemBase;
		if( pItemElem->GetProp()->dwPackMax > 1 )
		{
			short nTradeNum = pItemElem->m_nItemNum - (short)pItemBase->GetExtra();
			pItemElem->m_nItemNum = pItemBase->GetExtra();
			m_pOwner->m_Inventory.Add( pItemElem );
			pItemElem->m_nItemNum = nTradeNum;
			pItemElem->SetExtra( 0 );
			if( nTradeNum == 0 )
				pTrader->m_Inventory.RemoveAtId( pItemBase->m_dwObjId );	// 제거
		}
		else
		{
			m_pOwner->m_Inventory.Add( pItemElem );
			pTrader->m_Inventory.RemoveAtId( pItemBase->m_dwObjId );
		}
	}

	cbI		= a.GetCount();
	for( i = 0; i < cbI; i++ )
	{
		pItemBase	= a.GetAtId( i );
		pTrader->m_Inventory.Add( (CItemElem*)pItemBase );
	}
	
	// step1. 줄돈과 뺄돈을 구해둔다.
	int nThisGold = pTrader->m_vtInfo.TradeGetGold();
	int nTraderGold = TradeGetGold();

	// step2. m_dwTradeGold를 clear
	TradeSetGold( 0 );						// 원복 안되게 
	TradeClear();
	pTrader->m_vtInfo.TradeSetGold( 0 );	// 원복 안되게 
	pTrader->m_vtInfo.TradeClear();

	// step3. 돈을 더한다.
	m_pOwner->AddGold( nThisGold, FALSE );						
	pTrader->AddGold( nTraderGold, FALSE );

	return TRUE;
}

DWORD CVTInfo::TradeSetItem2( BYTE nId, BYTE i, short & nItemNum )
{
	CItemBase* pItemBase = m_pOwner->GetItemId( nId );
	if( IsUsableItem( pItemBase ) == FALSE || m_apItem_VT[i] != NULL )  
		return (DWORD)TID_GAME_CANNOTTRADE_ITEM;

	if( m_pOwner->m_Inventory.IsEquip( pItemBase->m_dwObjId ) ) 
		return (DWORD)TID_GAME_CANNOTTRADE_ITEM;
	
	if( ( (CItemElem*)pItemBase )->IsQuest() )
		return (DWORD)TID_GAME_CANNOTTRADE_ITEM;

	if( ( (CItemElem*)pItemBase )->IsBinds() )
		return (DWORD)TID_GAME_CANNOTTRADE_ITEM;

	if( m_pOwner->IsUsing( (CItemElem*)pItemBase ) )
		return (DWORD)TID_GAME_CANNOT_DO_USINGITEM;

	if( pItemBase->GetProp()->dwItemKind3 == IK3_CLOAK && ( (CItemElem*)pItemBase )->m_idGuild != 0 )
		return (DWORD)TID_GAME_CANNOTTRADE_ITEM;
	
	if( pItemBase->GetProp()->dwParts == PARTS_RIDE && pItemBase->GetProp()->dwItemJob == JOB_VAGRANT )
		return (DWORD)TID_GAME_CANNOTTRADE_ITEM;
	
	if( nItemNum < 1)
		nItemNum = 1;
	if( nItemNum > ( (CItemElem*)pItemBase )->m_nItemNum )
		nItemNum = ( (CItemElem*)pItemBase )->m_nItemNum;
	
	TradeSetItem( nId, i, nItemNum );
	return 0;
}

void TradeLog( CAr & ar, CItemBase* pItemBase, short nItemCount )
{
	ar << nItemCount;
	ar << ( (CItemElem*)pItemBase )->GetAbilityOption();
	ar << int(( (CItemElem*)pItemBase )->m_bItemResist);
	ar << int(( (CItemElem*)pItemBase )->m_nResistAbilityOption);
	ar << ( (CItemElem*)pItemBase )->m_nHitPoint;
	ar << ( (CItemElem*)pItemBase )->m_nRepair;
	ar << ( (CItemElem*)pItemBase )->m_bCharged;
	ar << ( (CItemElem*)pItemBase )->m_dwKeepTime;
#if __VER >= 12 // __EXT_PIERCING
	ar << ( (CItemElem*)pItemBase )->GetPiercingSize();
	for( int i=0; i<( (CItemElem*)pItemBase )->GetPiercingSize(); i++ )
		ar << ( (CItemElem*)pItemBase )->GetPiercingItem( i );
	ar << ( (CItemElem*)pItemBase )->GetUltimatePiercingSize();
	for( i=0; i<( (CItemElem*)pItemBase )->GetUltimatePiercingSize(); i++ )
		ar << ( (CItemElem*)pItemBase )->GetUltimatePiercingItem( i );
#else // __EXT_PIERCING
	ar << ( (CItemElem*)pItemBase )->GetPiercingSize();
	ar << ( (CItemElem*)pItemBase )->GetPiercingItem( 0 );
	ar << ( (CItemElem*)pItemBase )->GetPiercingItem( 1 );
	ar << ( (CItemElem*)pItemBase )->GetPiercingItem( 2 );
	ar << ( (CItemElem*)pItemBase )->GetPiercingItem( 3 );
#if __VER >= 9 // __ULTIMATE
	ar << ( (CItemElem*)pItemBase )->GetPiercingItem( 4 );
#endif // __ULTIMATE
#endif // __EXT_PIERCING
	ar << ( (CItemElem*)pItemBase )->GetRandomOptItemId();
#if __VER >= 9 // __PET_0410
		if( ((CItemElem*)pItemBase)->m_pPet )
		{
			CPet* pPet = ((CItemElem*)pItemBase)->m_pPet;

			ar << pPet->GetKind();
			ar << pPet->GetLevel();
			ar << pPet->GetExp();
			ar << pPet->GetEnergy();
			ar << pPet->GetLife();
			ar << pPet->GetAvailLevel( PL_D );
			ar << pPet->GetAvailLevel( PL_C );
			ar << pPet->GetAvailLevel( PL_B );
			ar << pPet->GetAvailLevel( PL_A );
			ar << pPet->GetAvailLevel( PL_S );
		}
		else
		{
			// mirchang_100514 TransformVisPet_Log
#if __VER >= 15 // __PETVIS
			if( ((CItemElem*)pItemBase)->IsTransformVisPet() == TRUE )
			{
				ar << (BYTE)100;
			}
			else
			{
				ar << (BYTE)0;
			}
#else // __PETVIS
			ar << (BYTE)0;
#endif // __PETVIS
			// mirchang_100514 TransformVisPet_Log
			ar << (BYTE)0;
			ar << (DWORD)0;
			ar << (WORD)0;
			ar << (WORD)0;
			ar << (BYTE)0;
			ar << (BYTE)0;
			ar << (BYTE)0;
			ar << (BYTE)0;
			ar << (BYTE)0;
		}
#endif // __PET_0410
}

// nMinus - 나갈 돈 
// nPlus  - 들어올 돈 
BOOL CheckTradeGold( CMover* pMover, int nMinus, int nPlus )
{
	if( nMinus >= 0 )
	{
		if( pMover->GetGold() >= nMinus )
		{
			int nGold = pMover->GetGold() - nMinus;
			int nResult = nGold + nPlus;
			if( nPlus >= 0 )
			{
				if( nResult >= nGold )	// overflow가 아니면?
					return TRUE;
			}
			else
			{
				if( nResult >= 0 )		// underflow가 아니면?
					return TRUE;
			}

		}
	}

	return FALSE;
}
//raiders.2006.11.28  계산과정 변경 
//로그와 체크를 때어내면 클라와 같다. 통합해서 refactoring하자.
TRADE_CONFIRM_TYPE CVTInfo::TradeLastConfirm( CAr & ar )
{
	TRADE_CONFIRM_TYPE		result = TRADE_CONFIRM_ERROR;
	CMover*					pTrader	= GetOther();

	// gold_step1. GetGold(), AddGold() 함수가 원하는데로 동작되도록 보관 후에 clear해둔다.
	int nTraderGold        = pTrader->m_vtInfo.TradeGetGold();
	int nUserGold          = TradeGetGold();
	
	// gold_step2. 줄돈과 뺄돈을 구해둔다.
	int nThisAdd   = nTraderGold;
	int nTraderAdd = nUserGold;

	// gold_step3. overflow를 검사 
	if( CheckTradeGold( m_pOwner, 0, nTraderGold ) == FALSE ||
		CheckTradeGold( pTrader, 0, nUserGold ) == FALSE )
	{
		TradeClear();
		pTrader->m_vtInfo.TradeClear();
		return result;
	}

	// 교환 할 만큼 양쪽의 인벤토리 슬롯이 여유가 있는 지를 검사한다. 
	int nPlayers = 0;
	int nTraders = 0;

	for( int i = 0; i < MAX_TRADE; i++ )
	{
		if( GetItem( i ) )
			nPlayers++;

		if( pTrader->m_vtInfo.GetItem( i ) )
			nTraders++;
	}

	if( ( pTrader->m_Inventory.GetSize() - pTrader->m_Inventory.GetCount() ) < nPlayers )
		result = TRADE_CONFIRM_ERROR;
	else if( ( m_pOwner->m_Inventory.GetSize() - m_pOwner->m_Inventory.GetCount() ) < nTraders )
		result = TRADE_CONFIRM_ERROR;
	else
		result = TRADE_CONFIRM_OK;

	if( result == TRADE_CONFIRM_OK )
	{
		CItemContainer<CItemElem> u;
		u.SetItemContainer( ITYPE_ITEM, MAX_TRADE );

		// gold_step4. 돈을 더한다.
		m_pOwner->AddGold( nThisAdd, FALSE );						
		pTrader->AddGold( nTraderAdd, FALSE );
		// TradeClear에서 원복이 안되기 위해서 
		pTrader->m_vtInfo.TradeSetGold( 0 );
		TradeSetGold( 0 );

		ar.WriteString( "T" );
		ar.WriteString( pTrader->GetName() );
		ar.WriteString( m_pOwner->GetName() );
		ar << m_pOwner->GetWorld()->GetID();
		ar << nTraderGold << nUserGold;
		ar << pTrader->GetGold() << m_pOwner->GetGold();
		ar << m_pOwner->m_idPlayer << m_pOwner->GetLevel() << m_pOwner->GetJob();
		ar << pTrader->m_idPlayer << pTrader->GetLevel() << pTrader->GetJob();
#ifdef __WORLDSERVER
		ar.WriteString( ( (CUser*)m_pOwner )->m_playAccount.lpAddr );
		ar.WriteString( ( (CUser*)pTrader )->m_playAccount.lpAddr );
#endif	// __WORLDSERVER
		u_long uSize1	= 0;
		u_long uOffset1	= ar.GetOffset();
		ar << (DWORD)0;
		u_long uSize2	= 0;
		u_long uOffset2	= ar.GetOffset();
		ar << (DWORD)0;

		// item_step1. m_pOwner->임시 
		CItemBase* pItemBase;
		for( int i = 0; i < MAX_TRADE; i++ )
		{
			pItemBase = m_apItem_VT[i];
			if( pItemBase == NULL )
				continue;

			m_apItem_VT[i] = NULL;
			CItemElem* pItemElem = ( CItemElem* )pItemBase;
			if( pItemElem->GetProp()->dwPackMax > 1 )
			{
				short nTradeNum = pItemElem->m_nItemNum - pItemBase->GetExtra();
				pItemElem->m_nItemNum = pItemBase->GetExtra();
				u.Add( pItemElem );
				pItemElem->m_nItemNum = nTradeNum;
				pItemElem->SetExtra( 0 );
				if( nTradeNum == 0 )
					m_pOwner->m_Inventory.RemoveAtId( pItemBase->m_dwObjId );	// 제거
			}
			else
			{
				u.Add( pItemElem );	// 임시 버퍼에 추가
				m_pOwner->m_Inventory.RemoveAtId( pItemBase->m_dwObjId );	// 제거
			}
		}
		
		// item_step2. pTrader -> m_pOwner
		for( i = 0; i < MAX_TRADE; i++ )
		{
			pItemBase = pTrader->m_vtInfo.GetItem( i );
			if( pItemBase == NULL )
				continue;

			pTrader->m_vtInfo.SetItem( i, NULL );
			uSize1++;
			ar << pItemBase->m_dwItemId;
			ar << pItemBase->GetSerialNumber();
			//ar.WriteString( pItemBase->GetProp()->szName );
			char szItemId[32] = {0, };
			_stprintf( szItemId, "%d", pItemBase->GetProp()->dwID );
			ar.WriteString( szItemId );

			CItemElem* pItemElem = ( CItemElem* )pItemBase;
			if( pItemElem->GetProp()->dwPackMax > 1 )
			{
				int nTradeNum = pItemElem->m_nItemNum - pItemBase->GetExtra();
				pItemElem->m_nItemNum = pItemBase->GetExtra();
				m_pOwner->m_Inventory.Add( pItemElem );
				pItemElem->m_nItemNum = (short)nTradeNum;

				TradeLog( ar, pItemBase, pItemBase->GetExtra() );
				
				pItemElem->SetExtra( 0 );
				if( nTradeNum == 0 )
					pTrader->m_Inventory.RemoveAtId( pItemBase->m_dwObjId );	
			}
			else
			{
				TradeLog( ar, pItemBase, 1 );
				
				m_pOwner->m_Inventory.Add( pItemElem );	// pUser에 pTrader가 준 아이템을 추가
				pTrader->m_Inventory.RemoveAtId( pItemBase->m_dwObjId );	
			}
		}

		// item_step3. 임시 -> pTrader
		nPlayers = u.GetCount();	// 합침을 고려해서 구해둔다.
		for( i = 0; i < nPlayers; i++ )
		{
			pItemBase = u.GetAtId( i );
			pTrader->m_Inventory.Add( (CItemElem*)pItemBase );
			uSize2++;
			ar << pItemBase->m_dwItemId;
			ar << pItemBase->GetSerialNumber();
			//ar.WriteString( pItemBase->GetProp()->szName );
			char szItemId[32] = {0, };
			_stprintf( szItemId, "%d", pItemBase->GetProp()->dwID );
			ar.WriteString( szItemId );
			TradeLog( ar, pItemBase, ((CItemElem*)pItemBase)->m_nItemNum );
		}

//		GETBLOCK( ar, lpBlock, nBlockSize );
		int nBufSize;	
		LPBYTE lpBlock	= ar.GetBuffer( &nBufSize );

		*(UNALIGNED u_long*)( lpBlock + uOffset1 )	= uSize1;
		*(UNALIGNED u_long*)( lpBlock + uOffset2 )	= uSize2;	
	}


	TradeClear();
	pTrader->m_vtInfo.TradeClear();
	return result;
}

TRADE_STATE CVTInfo::TradeGetState()
{
	return m_state;
}

void CVTInfo::TradeSetState( TRADE_STATE state )
{
	m_state = state;
}

///////////////////////////////////////////////////////////////////////////////
// 개인상점 
///////////////////////////////////////////////////////////////////////////////

//void CDPClient::OnUnregisterPVendorItem( OBJID objid, CAr & ar )
BOOL CVTInfo::VendorClearItem( BYTE i )
{
	CItemBase* pItemBase = m_apItem_VT[i];
	if( pItemBase )
	{
		pItemBase->SetExtra( 0 );
		pItemBase->m_nCost = 0;
		m_apItem_VT[i]     = NULL;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

// void CDPClient::OnRegisterPVendorItem( OBJID objid, CAr & ar )
void CVTInfo::VendorSetItem( BYTE nId, BYTE i, short nNum, int nCost )
{
	CItemBase* pItemBase = m_pOwner->GetItemId( nId );
	if( pItemBase )
	{
		m_apItem_VT[i] = pItemBase;
		pItemBase->SetExtra( nNum );
		pItemBase->m_nCost = nCost;
	}
}

//void CDPClient::OnPVendorItemNum( OBJID objid, CAr & ar )
// nNum - 남은 갯수 
void CVTInfo::VendorItemNum( BYTE i, short nNum )
{
	CItemBase* pItemBase = m_apItem_VT[i];
	if( pItemBase )
	{
		pItemBase->SetExtra( nNum );
		if( nNum == 0 )
		{
		#ifdef __CLIENT
			if( m_pOwner->IsActiveMover() == FALSE )
				SAFE_DELETE( m_apItem_VT[i] );
		#endif
			m_apItem_VT[i] = NULL;
		}
	}
}

// 데이타 카피를 해서 보관?
// void CDPClient::OnPVendorItem( OBJID objid, CAr & ar )
void CVTInfo::VendorCopyItems( CItemBase** ppItemVd )
{
	memcpy( (void*)m_apItem_VT, ppItemVd, sizeof(m_apItem_VT) );
}


void CVTInfo::VendorClose( BOOL bClearTitle )
{
	for( int i = 0; i < MAX_VENDITEM; i++ )
	{
		if( m_apItem_VT[i] )
		{
			m_apItem_VT[i]->SetExtra( 0 );
			m_apItem_VT[i]->m_nCost	= 0;
		#ifdef __CLIENT
			if( FALSE == m_pOwner->IsActiveMover() )
				SAFE_DELETE( m_apItem_VT[i] );
		#endif	// __CLIENT
			m_apItem_VT[i]	= NULL;
		}
	}

	if( bClearTitle )
		m_strTitle = "";

	SetOther( NULL );	
}


// 나는 판매자 인가? 
BOOL CVTInfo::VendorIsVendor() 
{
	for( int i=0; i<MAX_VENDITEM; ++i )
	{
		if( m_apItem_VT[i] )	// 등록한 아이템이 있는가?
			return TRUE;
	}

	return FALSE;
}

BOOL CVTInfo::IsTrading( CItemElem* pItemElem )
{
	for( int i = 0; i < MAX_VENDITEM; i++ )
	{
		if( m_apItem_VT[i] == pItemElem )
			return TRUE;
	}
	return FALSE;
}

//CDPSrvr::OnBuyPVendorItem
#ifdef __WORLDSERVER
BOOL CVTInfo::VendorSellItem( CMover* pBuyer, BYTE i, DWORD dwItemId, short nNum, VENDOR_SELL_RESULT& result )
{
	result.nRemain    = 0;
	result.nErrorCode = 0;

	if( IsVendorOpen() == FALSE )
		return FALSE;

	CItemBase* pItemBase = m_apItem_VT[i];
	if( IsUsingItem( pItemBase ) == FALSE || pItemBase->m_dwItemId != dwItemId )
		return FALSE;

	if( nNum < 1 )
		nNum = 1;
	if( nNum > pItemBase->GetExtra() )
		nNum = (short)pItemBase->GetExtra();

//	06.10.26
	if( pItemBase->m_nCost > 0 && (float)pBuyer->GetGold() < (float)nNum * (float)pItemBase->m_nCost )
	{
#if __VER >= 8 // __S8_VENDOR_REVISION
		result.nErrorCode = TID_GAME_LACKMONEY;
		return FALSE;
#else // __VER >= 8 // __S8_VENDOR_REVISION
		nNum = (short)( pBuyer->GetGold() / pItemBase->m_nCost );
#endif // __VER >= 8 // __S8_VENDOR_REVISION
	}
	
	if( nNum == 0 )
	{
		result.nErrorCode = TID_GAME_LACKMONEY;
		return FALSE;
	}

	CItemElem* pItemElem = (CItemElem*)pItemBase;
	CItemElem itemElem;
	itemElem	= *pItemElem;
	itemElem.m_nItemNum	 = nNum;

	if( pBuyer->CreateItem( &itemElem ) == FALSE )
	{
		result.nErrorCode = TID_GAME_LACKSPACE;
		return FALSE;
	}

	// CItemElem의 복사 연산자에 m_nCost는 제외되어 있다.
	int nCost	= pItemBase->m_nCost;
	pBuyer->AddGold( -( pItemBase->m_nCost * nNum ) );
	m_pOwner->AddGold( pItemBase->m_nCost * nNum );

	pItemBase->SetExtra( pItemBase->GetExtra() - nNum );
	int nRemain = pItemBase->GetExtra();
	if( nRemain <= 0 )
		m_apItem_VT[i] = NULL;

#if __VER >= 11 // __MOD_VENDOR
#ifdef __WORLDSERVER
	g_UserMng.AddPVendorItemNum( (CUser*)m_pOwner, i, nRemain, pBuyer->GetName() );
#endif	// __WORLDSERVER
#endif	// __MOD_VENDOR

	m_pOwner->RemoveItem( (BYTE)pItemBase->m_dwObjId, nNum );

	result.item = itemElem;
	result.item.m_nCost	= nCost;
	result.nRemain = nRemain;
	return TRUE;
}
#endif // __WORLDSERVER