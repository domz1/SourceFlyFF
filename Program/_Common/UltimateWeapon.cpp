// UltimateWeapon.cpp: implementation of the CUltimateWeapon class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UltimateWeapon.h"
#ifdef __WORLDSERVER
#include "User.h"

#include "DPSrvr.h"
#include "dpdatabaseclient.h"
#include "definetext.h"
#if __VER >= 14 // __SMELT_SAFETY
extern CUserMng g_UserMng;
#include "defineSound.h"
#include "defineObj.h"
#endif // __SMELT_SAFETY
#endif // __WORLDSERVER

#ifdef __WORLDSERVER
extern	CDPSrvr		g_DPSrvr;
extern	CDPDatabaseClient	g_dpDBClient;
#endif // __WORDLSERVER

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#if __VER >= 9 // __ULTIMATE
CUltimateWeapon::CUltimateWeapon()
{
#ifdef __WORLDSERVER
	m_nSetGemProb = 0;
	m_nRemoveGemProb = 0;
	m_nGen2UniProb = 0;
	m_nUni2UltiProb= 0;
#endif // __WORLDSERVER
}

CUltimateWeapon::~CUltimateWeapon()
{

}

BOOL CUltimateWeapon::Load_GemAbility()
{
	CScanner s;
	
	if( !s.Load( "Ultimate_GemAbility.txt" ) )
		return FALSE;
	
	int nMaxGemNum = 0;
	s.GetToken();
	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "ABILITY" ) )
		{
			__GEMABILITYKIND GemAbilityKind;
			s.GetToken();
			GemAbilityKind.nAbility = CScript::GetDefineNum( s.Token );
			
			s.GetToken(); // {
			s.GetToken();
			while( *s.token != '}' )
			{
				__GEMABILITY GemAbility;
				GemAbility.dwGemItemId = CScript::GetDefineNum( s.Token );
				s.GetToken();
				GemAbility.dwAbilityItemId = CScript::GetDefineNum( s.Token );
				for( int i=0; i<5; i++ )
					GemAbility.vecAbility.push_back( s.GetNumber() );
				GemAbilityKind.vecAbilityKind.push_back( GemAbility );
				s.GetToken();
			}
			m_vecGemAbilityKind.push_back( GemAbilityKind );
		}
		else if( s.Token == _T( "LEVEL_GEM" ) )
		{
			__MAKEGEMITEM	MakeGemItem;
			s.GetToken(); // {
			s.GetToken();
			while( *s.token != '}' )
			{
				MakeGemItem.dwItemID = CScript::GetDefineNum( s.Token );
				if( MakeGemItem.dwItemID == (DWORD)-1 )		// cr
				{
					Error( "UltimateWeapon::Load_UltimateWeapon() 아이템이 없는것을 설정했음" );
					return FALSE;
				}
				MakeGemItem.nMinLevel = s.GetNumber();
				MakeGemItem.nMaxLevel = s.GetNumber();

				m_vecMakeGemItem.push_back( MakeGemItem );
				s.GetToken();
			}
		}
		s.GetToken();
	}	
	return TRUE;
}

// 아이템 착용 레벨에 따른 보석 종류
DWORD CUltimateWeapon::GetGemKind( DWORD dwLimitLevel )
{
	for( DWORD i=0; i<m_vecMakeGemItem.size(); i++ )
	{
		if( (int)( dwLimitLevel ) <= m_vecMakeGemItem[i].nMaxLevel &&
			(int)( dwLimitLevel ) >= m_vecMakeGemItem[i].nMinLevel )
			return m_vecMakeGemItem[i].dwItemID;
	}

	return NULL_ID;
}

#ifdef __WORLDSERVER
BOOL CUltimateWeapon::Load_UltimateWeapon()
{
	CScanner s;
	
	if( !s.Load( "Ultimate_UltimateWeapon.txt" ) )
		return FALSE;
	

	s.GetToken();
	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "SET_GEM" ) )
			m_nSetGemProb = s.GetNumber();
		else if( s.Token == _T( "REMOVE_GEM" ) )
			m_nRemoveGemProb = s.GetNumber();
		else if( s.Token == _T( "GENERAL2UNIQUE" ) )
			m_nGen2UniProb = s.GetNumber();
		else if( s.Token == _T( "UNIQUE2ULTIMATE" ) )
			m_nUni2UltiProb = s.GetNumber();
		else if( s.Token == _T( "MAKE_GEM" ) )
		{
			__MAKEGEMPROB infoMakeGem;
			s.GetToken();	// {
			s.GetToken();
			while( *s.token != '}' )
			{
				int nEnchant = atoi( s.Token );
				infoMakeGem.dwGeneralProb = s.GetNumber();
				infoMakeGem.nGeneralNum = s.GetNumber();
				infoMakeGem.dwUniqueProb = s.GetNumber();
				infoMakeGem.nUniqueNum = s.GetNumber();

				m_mapMakeGemProb.insert( map<int, __MAKEGEMPROB>::value_type( nEnchant, infoMakeGem ) );
				s.GetToken();
			}
		}
		else if( s.Token == _T( "ULTIMATE_ENCHANT" ) )
		{
			int nNum = 0;
			int nProb= 0;
			s.GetToken(); // {
			s.GetToken();
			while( *s.token != '}' )
			{
				nNum = atoi( s.Token );
				nProb = s.GetNumber();
				m_mapUltimateProb.insert( map<int, int>::value_type( nNum, nProb ) );
				s.GetToken();
			}
		}
		s.GetToken();
	}	
	return TRUE;
}

// 보석 합성시 부여 되는 능력(능력)
DWORD CUltimateWeapon::GetGemAbilityKindRandom( DWORD dwGemItemid )
{
	if( m_vecGemAbilityKind.empty() == TRUE )
		return NULL_ID;

	int nRandom = xRandom( m_vecGemAbilityKind.size() );

	for(DWORD i=0; i<m_vecGemAbilityKind[nRandom].vecAbilityKind.size(); i++ )
	{
		if( m_vecGemAbilityKind[nRandom].vecAbilityKind[i].dwGemItemId == dwGemItemid )
			return m_vecGemAbilityKind[nRandom].vecAbilityKind[i].dwAbilityItemId;
	}
	return NULL_ID;
}

// 보석 생성 - 무기 아이템 파괴
int CUltimateWeapon::MakeGem( CUser* pUser, OBJID objItemId, int & nNum )
{
	CItemElem* pItemElem	= pUser->m_Inventory.GetAtId( objItemId );
	if( !IsUsableItem( pItemElem ) )
		return ULTIMATE_CANCEL;
	// 무기가 아닐 때
	if( pItemElem->GetProp()->dwItemKind1 != IK1_WEAPON )
	{
#ifdef __INTERNALSERVER
		pUser->AddText( "무기가 아님." );
#endif // __INTERNALSERVER
		return ULTIMATE_CANCEL;
	}
	// 일반무기나 유니크 아이템이 아닐 때
	if(	pItemElem->GetProp()->dwReferStat1 != WEAPON_GENERAL &&
		pItemElem->GetProp()->dwReferStat1 != WEAPON_UNIQUE )
	{
#ifdef __INTERNALSERVER
	pUser->AddText( "dwReferStat1 값이 WEAPON_GENERAL, WEAPON_UNIQUE 가 아님." );
#endif // __INTERNALSERVER
		return ULTIMATE_CANCEL;
	}
	
	// 장착되어 있으면 취소
	if( pUser->m_Inventory.IsEquip( objItemId ) )
	{
		pUser->AddDefinedText( TID_GAME_ULTIMATE_ISEQUIP , "" );
		return ULTIMATE_CANCEL;
	}
	// 레벨에 해당하는 보석 
	DWORD dwItemId = GetGemKind( pItemElem->GetProp()->dwLimitLevel1 );
	if( dwItemId == NULL_ID )
		return ULTIMATE_CANCEL;

	int nOpt = pItemElem->GetAbilityOption();
	map<int, __MAKEGEMPROB>::iterator it = m_mapMakeGemProb.find( nOpt );
	
	if( it == m_mapMakeGemProb.end() )
		return ULTIMATE_CANCEL;

	DWORD dwProb = it->second.dwGeneralProb;
	int nItemNum = it->second.nGeneralNum;
	if( pItemElem->GetProp()->dwReferStat1 == WEAPON_UNIQUE )
	{
		dwProb = it->second.dwUniqueProb;
		nItemNum = it->second.nUniqueNum;
	}
	// 인벤토리가 부족할 때 - 취소
	CItemElem itemElemTemp;
	itemElemTemp.m_dwItemId = dwItemId;
	ItemProp* pItemprop = itemElemTemp.GetProp();
	if( !pItemprop )
		return ULTIMATE_CANCEL;

	// 무기를 삭제되서 1칸이 생기므로 2칸이상 일때만 검사
	if( (DWORD)( nItemNum ) > pItemprop->dwPackMax 
		&& pUser->m_Inventory.IsFull( &itemElemTemp, pItemprop, (short)( nItemNum - pItemprop->dwPackMax ) ) )
		return ULTIMATE_INVENTORY;
	
	LogItemInfo aLogItem;
	aLogItem.Action = "-";
	aLogItem.SendName = pUser->GetName();
	aLogItem.RecvName = "ULTIMATE_MAKEGEM";
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
	g_DPSrvr.OnLogItem( aLogItem, pItemElem, 1 );	
	pUser->RemoveItem( (BYTE)( objItemId ), 1 );
	
	nNum = 0;
	int nRandom = xRandom( 1000000 );
	if( (DWORD)( nRandom ) < dwProb )
	{
		nNum = nItemNum;
		CItemElem itemElem;
		itemElem.m_dwItemId = dwItemId;
		itemElem.m_nItemNum	= nItemNum;
		itemElem.SetSerialNumber();
		itemElem.m_nHitPoint	= 0;

		pUser->CreateItem( &itemElem );
		aLogItem.RecvName = "ULTIMATE_MAKEGEM_SUCCESS";
		g_DPSrvr.OnLogItem( aLogItem, &itemElem, nItemNum );
		return ULTIMATE_SUCCESS;
	}
	aLogItem.RecvName = "ULTIMATE_MAKEGEM_FAILED";
	g_DPSrvr.OnLogItem( aLogItem );
	return ULTIMATE_FAILED;
}

// 무기에 보석 합성
int CUltimateWeapon::SetGem( CUser* pUser, OBJID objItemId, OBJID objGemItemId )
{
	CItemElem* pItemElem	= pUser->m_Inventory.GetAtId( objItemId );
	CItemElem* pGemItemElem	= pUser->m_Inventory.GetAtId( objGemItemId );
	if( !IsUsableItem( pItemElem ) || !IsUsableItem( pGemItemElem ) )
		return ULTIMATE_CANCEL;

	// 얼터멋 웨폰이 아닐 때
	if( pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE )
		return ULTIMATE_ISNOTULTIMATE;


	// 장착되어 있으면 중단
	if( pUser->m_Inventory.IsEquip( objItemId ) )
	{
		pUser->AddDefinedText( TID_GAME_ULTIMATE_ISEQUIP , "" );
		return ULTIMATE_CANCEL;
	}
		
#if __VER >= 12 // __EXT_PIERCING
	for( int nCount=0; nCount < pItemElem->GetUltimatePiercingSize(); nCount++ )
		if( pItemElem->GetUltimatePiercingItem( nCount ) == 0 )
#else // __EXT_PIERCING
	for( int nCount=0; nCount < pItemElem->GetPiercingSize(); nCount++ )
		if( pItemElem->GetPiercingItem( nCount ) == 0 )
#endif // __EXT_PIERCING
			break;
		
	// 빈곳이 없으면 중단
#if __VER >= 12 // __EXT_PIERCING
	if( nCount == pItemElem->GetUltimatePiercingSize() )
#else //__EXT_PIERCING
	if( nCount == pItemElem->GetPiercingSize() )
#endif //__EXT_PIERCING
	{
		pUser->AddDefinedText( TID_GAME_ULTIMATE_GEMSPACE, "" );
		return ULTIMATE_CANCEL;
	}

	DWORD dwSetItemId = GetGemAbilityKindRandom( pGemItemElem->m_dwItemId );
	if( dwSetItemId == NULL_ID )
		return ULTIMATE_CANCEL;
	
	// 보석 삭제
	LogItemInfo aLogItem;
	aLogItem.Action = "-";
	aLogItem.SendName = pUser->GetName();
	aLogItem.RecvName = "ULTIMATE_PIERCING";
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
	g_DPSrvr.OnLogItem( aLogItem, pGemItemElem, 1 );	
	pUser->RemoveItem( (BYTE)( objGemItemId ), 1 );
	
	int nRandom = xRandom( 1000000 );
	if( nRandom < m_nSetGemProb )
	{
		// 보석 합성
#if __VER >= 12 // __EXT_PIERCING
		pUser->UpdateItem( (BYTE)( pItemElem->m_dwObjId ), UI_ULTIMATE_PIERCING, MAKELONG( nCount, dwSetItemId ) );
#else // __EXT_PIERCING
		pUser->UpdateItem( (BYTE)( pItemElem->m_dwObjId ), UI_PIERCING, MAKELONG( nCount, dwSetItemId ) );
#endif // __EXT_PIERCING
		aLogItem.RecvName = "ULTIMATE_PIERCING_SUCCESS";
		g_DPSrvr.OnLogItem( aLogItem, pItemElem, 1 );
		return ULTIMATE_SUCCESS;	
	}
	aLogItem.RecvName = "ULTIMATE_PIERCING_FAILED";
	g_DPSrvr.OnLogItem( aLogItem, pItemElem );
	return ULTIMATE_FAILED;
}

// 무기에 마지막으로 합성된 보석 제거
int CUltimateWeapon::RemoveGem( CUser* pUser, OBJID objItemId, OBJID objItemGem )
{
	CItemElem* pItemElem	= pUser->m_Inventory.GetAtId( objItemId );
	CItemElem* pItemElemGem	= pUser->m_Inventory.GetAtId( objItemGem );
	if( !IsUsableItem( pItemElem ) || !IsUsableItem( pItemElemGem ) )
		return ULTIMATE_CANCEL;
	
	if( pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE )
		return ULTIMATE_ISNOTULTIMATE;
	
	// 합성된 보석이 없을 때 X
#if __VER >= 12 // __EXT_PIERCING
	if( pItemElem->GetUltimatePiercingItem( 0 ) == 0 )
#else // __EXT_PIERCING
	if( pItemElem->GetPiercingItem( 0 ) == 0 )
#endif // __EXT_PIERCING
		return ULTIMATE_CANCEL;

	if( pItemElemGem->m_dwItemId != II_GEN_MAT_MOONSTONE
		&& pItemElemGem->m_dwItemId != II_GEN_MAT_MOONSTONE_1 )
		return ULTIMATE_CANCEL;
 
	// 장착 되어 있을 때 X
	if( pUser->m_Inventory.IsEquip( objItemId ) )
	{
		pUser->AddDefinedText( TID_GAME_ULTIMATE_ISEQUIP , "" );
		return ULTIMATE_CANCEL;
	}	

	// 문스톤 삭제
	LogItemInfo aLogItem;
	aLogItem.Action = "-";
	aLogItem.SendName = pUser->GetName();
	aLogItem.RecvName = "ULTIMATE_PIERCING_REMOVE";
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
	g_DPSrvr.OnLogItem( aLogItem, pItemElemGem, 1 );
	pUser->RemoveItem( (BYTE)( objItemGem ), 1 );
	
	int nRandom = xRandom( 1000000 );
	if( nRandom < m_nRemoveGemProb )
	{
		// 성공시 - 보석 제거
#if __VER >= 12 // __EXT_PIERCING
		for( int i=pItemElem->GetUltimatePiercingSize()-1; i>=0; i-- )
		{
			if( pItemElem->GetUltimatePiercingItem( i ) != 0 )
			{
				pUser->UpdateItem( (BYTE)( pItemElem->m_dwObjId ), UI_ULTIMATE_PIERCING, MAKELONG( i, 0 ) );
#else // __EXT_PIERCING
		for( int i=pItemElem->GetPiercingSize()-1; i>=0; i-- )
		{
			if( pItemElem->GetPiercingItem( i ) != 0 )
			{
				pUser->UpdateItem( pItemElem->m_dwObjId, UI_PIERCING, MAKELONG( i, 0 ) );
#endif // __EXT_PIERCING
				aLogItem.RecvName = "ULTIMATE_PIERCING_REMOVE_SUC";
				g_DPSrvr.OnLogItem( aLogItem, pItemElem, 1 );
				break;
			}
		}
		return ULTIMATE_SUCCESS;
	}
	aLogItem.RecvName = "ULTIMATE_PIERCING_REMOVE_FAIL";
	g_DPSrvr.OnLogItem( aLogItem, pItemElem );
	return ULTIMATE_FAILED;
}

// 빛나는 오리칼쿰 생성 - 오리칼쿰 5개, 문스톤 5개 합성
int CUltimateWeapon::MakeItem( CUser* pUser, OBJID* objItemId )
{
	map<OBJID, int> mapObjId1;
	map<OBJID, int> mapObjId2;
	for( int i=0; i<5; i++ )
	{
		map<OBJID, int>::iterator* it = &mapObjId1.find(objItemId[i]);
		if( *it != mapObjId1.end() )
			++(*it)->second;
		else
			mapObjId1.insert(map<OBJID, int>::value_type( objItemId[i], 1 ));
		
		it = &mapObjId2.find(objItemId[i+5]);
		if( *it != mapObjId2.end() )
			++(*it)->second;
		else
			mapObjId2.insert(map<OBJID, int>::value_type( objItemId[i+5], 1 ));
	}

	int nItemCount = 0;
	for( map<OBJID, int>::iterator it = mapObjId1.begin() ; it != mapObjId1.end() ; ++it )
	{
		CItemElem* pItemElem;
		pItemElem = pUser->m_Inventory.GetAtId( it->first );
		if( !IsUsableItem( pItemElem ) )
			return ULTIMATE_CANCEL;
		
		if( pItemElem->GetProp()->dwID != II_GEN_MAT_ORICHALCUM01
			&& pItemElem->GetProp()->dwID != II_GEN_MAT_ORICHALCUM01_1 )
			return ULTIMATE_CANCEL;
		if( it->second > pItemElem->m_nItemNum )
			return ULTIMATE_CANCEL;

		nItemCount += it->second;
	}
	if( nItemCount != 5 )
		return ULTIMATE_CANCEL;

	nItemCount = 0;
	for( it = mapObjId2.begin() ; it != mapObjId2.end() ; ++it )
	{
		CItemElem* pItemElem;
		pItemElem = pUser->m_Inventory.GetAtId( it->first );
		if( !IsUsableItem( pItemElem ) )
			return ULTIMATE_CANCEL;
		
		if( pItemElem->GetProp()->dwID != II_GEN_MAT_MOONSTONE
			&& pItemElem->GetProp()->dwID != II_GEN_MAT_MOONSTONE_1 )
			return ULTIMATE_CANCEL;
		if( it->second > pItemElem->m_nItemNum )
			return ULTIMATE_CANCEL;
		
		nItemCount += it->second;
	}

	if( nItemCount != 5 )
		return ULTIMATE_CANCEL;
	
	//인벤토리가 부족할 때 - 취소
	CItemElem itemElemTemp;
	itemElemTemp.m_dwItemId = II_GEN_MAT_ORICHALCUM02;
	if( !itemElemTemp.GetProp() || pUser->m_Inventory.IsFull( &itemElemTemp, itemElemTemp.GetProp(), 1 ) )
		return ULTIMATE_INVENTORY;
	
	LogItemInfo aLogItem;
	aLogItem.Action = "-";
	aLogItem.SendName = pUser->GetName();
	aLogItem.RecvName = "ULTIMATE_MAKEITEM";
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
	
	CItemElem* pItemElem;
	for( it = mapObjId1.begin() ; it != mapObjId1.end() ; ++it )
	{
		pItemElem = pUser->m_Inventory.GetAtId( it->first );
		g_DPSrvr.OnLogItem( aLogItem, pItemElem, it->second );	
		pUser->RemoveItem( (BYTE)( it->first ), it->second );
	}
	for( it = mapObjId2.begin() ; it != mapObjId2.end() ; ++it )
	{
		pItemElem = pUser->m_Inventory.GetAtId( it->first );
		g_DPSrvr.OnLogItem( aLogItem, pItemElem, it->second );	
		pUser->RemoveItem( (BYTE)( it->first ), it->second );
	}

	CItemElem itemElem;
	itemElem.m_dwItemId = II_GEN_MAT_ORICHALCUM02;
	itemElem.m_nItemNum	= 1;
	itemElem.SetSerialNumber();
	itemElem.m_nHitPoint	= 0;
	
	pUser->CreateItem( &itemElem );
	aLogItem.RecvName = "ULTIMATE_MAKEITEM_SUCCESS";
	g_DPSrvr.OnLogItem( aLogItem, &itemElem, 1 );
	
	return ULTIMATE_SUCCESS;
}

// 무기변환(일반->유니크, 유니크->얼터멋)
int CUltimateWeapon::TransWeapon( CUser* pUser, OBJID objItem, OBJID objGem1, OBJID objGem2 )
{
	CItemElem* pItemElemWeapon	= pUser->m_Inventory.GetAtId( objItem );
	CItemElem* pItemElemGem1	= pUser->m_Inventory.GetAtId( objGem1 );
	CItemElem* pItemElemGem2	= pUser->m_Inventory.GetAtId( objGem2 );
	if( IsUsableItem( pItemElemWeapon ) == FALSE
		|| IsUsableItem( pItemElemGem1 ) == FALSE
		|| IsUsableItem( pItemElemGem2 ) == FALSE )
		return ULTIMATE_CANCEL;
	
	// 장착하고 있으면 취소
	if( pUser->m_Inventory.IsEquip( objItem ) )
	{
		pUser->AddDefinedText( TID_GAME_ULTIMATE_ISEQUIP , "" );
		return ULTIMATE_CANCEL;
	}
		
	// 무기가 아니면 취소
	if( pItemElemWeapon->GetProp()->dwItemKind1 != IK1_WEAPON )
	{
#ifdef __INTERNALSERVER
		pUser->AddText( "무기가 아님." );
#endif // __INTERNALSERVER
		return ULTIMATE_CANCEL;
	}

	// 타겟 ID 가 없을 때
	if( pItemElemWeapon->GetProp()->dwReferTarget1 == NULL_ID )
	{
#ifdef __INTERNALSERVER
		pUser->AddText( "dwReferTarget1 에 변환될 아이템 ID가 없음." );
#endif // __INTERNALSERVER
		return ULTIMATE_CANCEL;	
	}
	
	// 일반, 유니크 아이템이 아닐 때
	if( pItemElemWeapon->GetProp()->dwReferStat1 != WEAPON_GENERAL &&
		  pItemElemWeapon->GetProp()->dwReferStat1 != WEAPON_UNIQUE )
	{
#ifdef __INTERNALSERVER
		pUser->AddText( "dwReferStat1 값이 WEAPON_GENERAL, WEAPON_UNIQUE 가 아님." );
#endif // __INTERNALSERVER
		return ULTIMATE_CANCEL;
	}

	DWORD dwItemId = GetGemKind( pItemElemWeapon->GetProp()->dwLimitLevel1 );
	if( dwItemId != pItemElemGem1->m_dwItemId || pItemElemGem2->m_dwItemId != II_GEN_MAT_ORICHALCUM02 )
		return ULTIMATE_CANCEL;

	CString strTemp = "UNIQUE_"; 
	CString strLog;
	int nProb = m_nGen2UniProb;
	if( pItemElemWeapon->GetProp()->dwReferStat1 == WEAPON_UNIQUE )
	{
		if( pItemElemWeapon->GetAbilityOption() != 10 )
			return ULTIMATE_CANCEL;
		nProb = m_nUni2UltiProb;
		strTemp = "ULTIMATE_";
	}
	// propItem.txt -> dwReferTarget2(참조타겟2)에 값이 변환 확률이 된다.
	// 값이 없으면 Ultimate_UltimateWeapon.txt에 정의된 값이 변환 확률이 된다.
	if( pItemElemWeapon->GetProp()->dwReferTarget2 != NULL_ID )
		nProb = pItemElemWeapon->GetProp()->dwReferTarget2;
	
	BOOL bSmelprot3	= FALSE;
	if( pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT3 )
		&& pItemElemWeapon->GetProp()->dwReferStat1 == WEAPON_UNIQUE )
	{
		bSmelprot3	= TRUE;
		pUser->RemoveBuff( BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT3 );
		
		ItemProp* pItemProp = prj.GetItemProp( II_SYS_SYS_SCR_SMELPROT3 );
		if( pItemProp )
			g_dpDBClient.SendLogSMItemUse( "2", pUser, NULL, pItemProp );
	}

#ifdef __UPGRADE_SUCCESS_SCROLL
	if( pUser->HasBuffByIk3( IK3_ULTIMATE_UPGRADE_RATE ))
	{
		IBuff* pBuff = pUser->m_buffs.GetBuffByIk3( IK3_ULTIMATE_UPGRADE_RATE );
		if( pBuff != NULL )
		{
			ItemProp *pItemProp = pBuff->GetProp();
			if( pItemProp )
			{
				if( pItemElemWeapon->GetAbilityOption() >= pItemProp->nTargetMinEnchant && pItemElemWeapon->GetAbilityOption() <= pItemProp->nTargetMaxEnchant )
				{
					nProb += ((WORD)pItemProp->nEffectValue * 100 );
					pUser->RemoveBuff( BUFF_ITEM,(WORD)pItemProp->dwID );
					g_dpDBClient.SendLogSMItemUse( "2", pUser, NULL, pItemProp );
				}
			}
		}
	}
#endif // __UPGRADE_SUCCESS_SCROLL

	LogItemInfo aLogItem;
	aLogItem.Action = "-";
	aLogItem.SendName = pUser->GetName();
	strLog = strTemp + "TRANS";
	aLogItem.RecvName = (LPCTSTR)strLog;
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
	g_DPSrvr.OnLogItem( aLogItem, pItemElemGem1, 1 );
	pUser->RemoveItem( (BYTE)( objGem1 ), 1 );
	g_DPSrvr.OnLogItem( aLogItem, pItemElemGem2, 1 );
	pUser->RemoveItem( (BYTE)( objGem2 ), 1 );

	int nRandom = xRandom( 1000000 );
	if( nRandom < nProb )
	{
		CItemElem CreateItem;
		CreateItem	= *pItemElemWeapon;
		CreateItem.m_dwItemId = pItemElemWeapon->GetProp()->dwReferTarget1;
		CreateItem.SetSerialNumber();

		if( pItemElemWeapon->GetProp()->dwReferStat1 == WEAPON_UNIQUE )
			CreateItem.SetAbilityOption( 0 );
		

		g_DPSrvr.OnLogItem( aLogItem, pItemElemWeapon, 1 );
		pUser->RemoveItem( (BYTE)( objItem ), 1 );
		if( pUser->CreateItem( &CreateItem ) )
		{
			//aLogItem.RecvName = "ULTIMATE_TRANS_SUCCESS";
			strLog = strTemp + "TRANS_SUCCESS";
			aLogItem.RecvName = (LPCTSTR)strLog;
			g_DPSrvr.OnLogItem( aLogItem, &CreateItem, 1 );
			return ULTIMATE_SUCCESS;
		}
	}
	else //실패
	{
		if( !bSmelprot3 )
		{
			//aLogItem.RecvName = "ULTIMATE_TRANS_FAILED";
			strLog = strTemp + "TRANS_FAILED";
			aLogItem.RecvName = (LPCTSTR)strLog;
			g_DPSrvr.OnLogItem( aLogItem, pItemElemWeapon, 1 );
			pUser->RemoveItem( (BYTE)( objItem ), 1 );
		}
		else
		{
			//aLogItem.RecvName = "ULTIMATE_TRANS_PROTECT";
			strLog = strTemp + "TRANS_PROTECT";
			aLogItem.RecvName = (LPCTSTR)strLog;
			g_DPSrvr.OnLogItem( aLogItem, pItemElemWeapon );
		}
	}
	
	return ULTIMATE_FAILED;
}

// 얼터멋 웨폰 제련
int CUltimateWeapon::EnchantWeapon( CUser* pUser, OBJID objItem, OBJID objItemGem )
{
	CItemElem* pItemElemWeapon	= pUser->m_Inventory.GetAtId( objItem );
	CItemElem* pItemElemGem	= pUser->m_Inventory.GetAtId( objItemGem );
	if( IsUsableItem( pItemElemWeapon ) == FALSE || IsUsableItem( pItemElemGem ) == FALSE )
		return ULTIMATE_CANCEL;

	//얼터멋 웨폰이 아니거나 빛나는 오리칼쿰이 아니면 취소
	if( pItemElemWeapon->GetProp()->dwReferStat1 != WEAPON_ULTIMATE ||
		pItemElemGem->m_dwItemId != II_GEN_MAT_ORICHALCUM02 )
		return ULTIMATE_ISNOTULTIMATE;

	// 장착하고 있으면 취소
	if( pUser->m_Inventory.IsEquip( objItem ) )
	{
		pUser->AddDefinedText( TID_GAME_ULTIMATE_ISEQUIP , "" );
		return ULTIMATE_CANCEL;
	}

	int pAbilityOpt = pItemElemWeapon->GetAbilityOption();
	if( (pAbilityOpt+1) > 10 )
	{
		pUser->AddDefinedText( TID_UPGRADE_MAXOVER , "" );
		return ULTIMATE_CANCEL;
	}

	BOOL bSmelprot3	= FALSE;
	if( pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT3 ) )
	{
		bSmelprot3	= TRUE;
		pUser->RemoveBuff( BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT3 );
		
		ItemProp* pItemProp = prj.GetItemProp( II_SYS_SYS_SCR_SMELPROT3 );
		if( pItemProp )
			g_dpDBClient.SendLogSMItemUse( "2", pUser, NULL, pItemProp );
	}
	
	//보석 소진
	LogItemInfo aLogItem;
	aLogItem.Action = "-";
	aLogItem.SendName = pUser->GetName();
	aLogItem.RecvName = "ULTIMATE_ENCHANT";
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
	g_DPSrvr.OnLogItem( aLogItem, pItemElemGem, 1 );	
	pUser->RemoveItem( (BYTE)( objItemGem ), 1 );

	int nRandom = xRandom( 1000000 );
	map<int, int>::iterator it = m_mapUltimateProb.find( pAbilityOpt+1 );
	if( it == m_mapUltimateProb.end() )
		return ULTIMATE_CANCEL;
	
	if( nRandom < it->second )
	{
		pUser->UpdateItem( (BYTE)pItemElemWeapon->m_dwObjId, UI_AO, ++pAbilityOpt );
		if( pAbilityOpt > 5 )
#if __VER >= 12 // __EXT_PIERCING
			pUser->UpdateItem( (BYTE)pItemElemWeapon->m_dwObjId, UI_ULTIMATE_PIERCING_SIZE, pAbilityOpt - 5 );
#else //__EXT_PIERCING
			pUser->UpdateItem( (BYTE)pItemElemWeapon->m_dwObjId, UI_PIERCING_SIZE, pAbilityOpt - 5 );
#endif // __EXT_PIERCING
		aLogItem.RecvName = "ULTIMATE_ENCHANT_SUCCESS";
		g_DPSrvr.OnLogItem( aLogItem, pItemElemWeapon, 1 );
		return ULTIMATE_SUCCESS;
	}
	else
	{
		if( bSmelprot3 )	// 보호의 두루마리(최상급) 사용시
		{
			aLogItem.RecvName = "ULTIMATE_ENCHANT_PROTECT";
			g_DPSrvr.OnLogItem( aLogItem, pItemElemWeapon );
		}
		else	// 얼터멋 웨폰 파괴
		{
			aLogItem.RecvName = "ULTIMATE_ENCHANT_FAILED";
			g_DPSrvr.OnLogItem( aLogItem, pItemElemWeapon, 1 );
			pUser->RemoveItem( (BYTE)( objItem ), 1 );
		}
	}	
	return ULTIMATE_FAILED;
}


#if __VER >= 14 // __SMELT_SAFETY
BYTE CUltimateWeapon::SmeltSafetyUltimate( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial, CItemElem* pItemProtScr )
{
	//	얼터멋웨폰이 아니면 리턴
	if( pItemMain->GetProp()->dwReferStat1 != WEAPON_ULTIMATE )
	{
		//pUser->AddDefinedText( TID_GAME_NOTEQUALITEM );
		return 0;
	}

	//	최상급 보호의 두루마리가 아니면 리턴
	if( pItemProtScr->GetProp()->dwID != II_SYS_SYS_SCR_SMELPROT3)
	{
		//pUser->AddDefinedText( TID_GAME_NOTEQUALITEM );
		return 0;
	}

	//	제련수치가 max치를 넘었을때 리턴
	if( pItemMain->GetAbilityOption() >= 10 )
	{
		//pUser->AddDefinedText( TID_UPGRADE_MAXOVER );
		return 3;
	}


	LogItemInfo aLogItem;
	aLogItem.Action = "-";
	aLogItem.SendName = pUser->GetName();
	aLogItem.RecvName = "ULTIMATE_ENC_SMELTSAFETY";
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = pUser->GetGold();
	aLogItem.Gold2 = pUser->GetGold();
	
	// 제련아템 삭제 - 성공이던, 실패던...
	g_DPSrvr.OnLogItem( aLogItem, pItemMaterial, pItemMaterial->m_nItemNum );
	pUser->RemoveItem( (BYTE)( pItemMaterial->m_dwObjId ), 1 );
	//	두루마리 로그남길것
	ItemProp* pItemProp = pItemProtScr->GetProp();
	if( pItemProp )
	{
		g_dpDBClient.SendLogSMItemUse( "1", pUser, pItemProtScr, pItemProp );
		g_dpDBClient.SendLogSMItemUse( "2", pUser, NULL, pItemProp );
	}
	pUser->RemoveItem( (BYTE)( pItemProtScr->m_dwObjId ), 1 );

	// 제련 확률
	map<int, int>::iterator it = m_mapUltimateProb.find( pItemMain->GetAbilityOption() + 1 );
	if( it == m_mapUltimateProb.end() )
		return 0;
	
	if( (int)( xRandom( 1000000 ) ) > it->second )
	{
		// 실패
		pUser->AddPlaySound( SND_INF_UPGRADEFAIL );
		if( ( pUser->IsMode( TRANSPARENT_MODE ) ) == 0 )
			g_UserMng.AddCreateSfxObj( ( CMover * )pUser, XI_INT_FAIL, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );

		aLogItem.RecvName = "ULTIMATE_ENC_FAIL_SMELTSAFETY";
		g_DPSrvr.OnLogItem( aLogItem, pItemMain, pItemMain->m_nItemNum );

		return 2;
	}
	else
	{
		// 성공
		pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );		
		if((pUser->IsMode( TRANSPARENT_MODE ) ) == 0 )
			g_UserMng.AddCreateSfxObj( ( CMover * )pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );
		
		pUser->UpdateItem( (BYTE)pItemMain->m_dwObjId, UI_AO, pItemMain->GetAbilityOption() + 1 );
		if( pItemMain->GetAbilityOption() > 5 )
			pUser->UpdateItem( (BYTE)pItemMain->m_dwObjId, UI_ULTIMATE_PIERCING_SIZE, pItemMain->GetAbilityOption() - 5 );
		
		aLogItem.RecvName = "ULTIMATE_ENC_SUC_SMELTSAFETY";
		g_DPSrvr.OnLogItem( aLogItem, pItemMain, pItemMain->m_nItemNum );

		return 1;
	}
	return 0;
}
#endif // __SMELT_SAFETY
#endif // __WORLDSERVER

// 보석 합성 후 능력치 적용
void CUltimateWeapon::SetDestParamUltimate( CMover* pMover, CItemElem* pItemElem, BOOL bEquip, DWORD dwParts )
{
	
	if( dwParts != PARTS_RWEAPON )
		return;
	
	map<int, int> mapDst = GetDestParamUltimate( pItemElem );
	for(map<int, int>::iterator it = mapDst.begin(); it != mapDst.end(); it++ )
	{
		if( bEquip )
			pMover->SetDestParam( it->first, it->second, NULL_CHGPARAM );
		else 
			pMover->ResetDestParam( it->first, it->second, TRUE );
	}
}

// 합성된 보석들의 종류 및 능력
map<int, int> CUltimateWeapon::GetDestParamUltimate( CItemElem* pItemElem )
{
	map<int, int> mapDst;  
	if( !pItemElem || pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE )
		return mapDst;
	
	map<DWORD, int> mapItem;		
#if __VER >= 12 // __EXT_PIERCING
	for( int i=0; i<pItemElem->GetUltimatePiercingSize(); i++ )
	{
		if( pItemElem->GetUltimatePiercingItem( i ) == 0 )
			break;

		map<DWORD, int>::iterator it=mapItem.find( pItemElem->GetUltimatePiercingItem( i ) );
		if( it==mapItem.end() )
			mapItem.insert( map<DWORD, int>::value_type( pItemElem->GetUltimatePiercingItem( i ), 1 ) );
#else // __EXT_PIERCING
	for( int i=0; i<pItemElem->GetPiercingSize(); i++ )
	{
		if( pItemElem->GetPiercingItem( i ) == 0 )
			break;
		
		map<DWORD, int>::iterator it=mapItem.find( pItemElem->GetPiercingItem( i ) );
		if( it==mapItem.end() )
			mapItem.insert( map<DWORD, int>::value_type( pItemElem->GetPiercingItem( i ), 1 ) );
#endif // __EXT_PIERCING
		else
			it->second++;
	}
	
	for( map<DWORD, int>::iterator it=mapItem.begin(); it!=mapItem.end(); it++ )
	{
		ItemProp* pItemProp	= prj.GetItemProp( it->first );
		int nResultDst = GetDST( it->first, it->second, pItemProp->dwReferStat2 );
		
		map<int, int>::iterator iter=mapDst.find( pItemProp->dwReferStat2 );
		if( iter==mapDst.end() )
			mapDst.insert( map<int, int>::value_type( pItemProp->dwReferStat2, nResultDst ) );
		else
			iter->second += nResultDst;
	}
	return mapDst;
}

// 합성된 보석의 종류와 갯수에 따른 능력치
int CUltimateWeapon::GetDST( DWORD dwItemId, int nItemNum, int nDSTInfo )
{
	int nResultDST = 0;
	__GEMABILITYKIND	GemAbilityKind;

	BOOL bFind = FALSE;
	for( DWORD i=0; i<m_vecGemAbilityKind.size(); i++ )
	{
		if( m_vecGemAbilityKind[i].nAbility == nDSTInfo )
		{
			GemAbilityKind = m_vecGemAbilityKind[i];
			bFind = TRUE;
			break;
		}
	}

	if( !bFind )
		return 0;

	for( DWORD i = 0 ; i < GemAbilityKind.vecAbilityKind.size() ; ++i )
	{
		if( dwItemId == GemAbilityKind.vecAbilityKind[i].dwAbilityItemId )
		{
			nResultDST = GemAbilityKind.vecAbilityKind[i].vecAbility[nItemNum-1];
			break;
		}
	}	
	return nResultDST;
}
#endif //__ULTIMATE
