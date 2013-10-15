////////////////////////////////////////////////////////////////////
PROJECT.CPP 에서 사용하는 코드를 
필요하다면 2006.07.22 project.cpp를 get lastest version해서 사용한다.
////////////////////////////////////////////////////////////////////
 
// state를 사용하는 퀘스트를 저장한다
// 최신 버젼의 퀘스트 시스템은 state 사용을 지양한다.
// 따라서 기존에 state를 사용하던 테이블을 저장하여 향후 임의로 state가 사용될 때
// 이것과 비교하여 에러를 발생시켜, 개발자들로 하여금 state 사용을 원천 봉쇄한다.
// 만약 불가피하게 사용할 필요가 있다면 별도로 프로그래머들에게 요청한다.
#ifdef __CLIENT
//#define __WRITE_STATE_QUEST 
//#define __CHECK_STATE_QUEST
#endif


BOOL CProject::OpenProject( LPCTSTR lpszFileName )
{

#if defined( __CHECK_STATE_QUEST ) && !defined( __WRITE_STATE_QUEST )
	CFileIO fileState;
	if( fileState.Open( "questState.dat", "rb" ) )
	{
		int nSize;
		fileState.Read( &nSize, sizeof( nSize ) );
		for( int i = 0;i < nSize; i++ )
		{
			int nQuest;
			fileState.Read( &nQuest, sizeof( nQuest ) );
			m_aStateQuest.Add( nQuest );
		}
	}
#endif


   ....

#ifdef __WRITE_STATE_QUEST
	CFileIO fileState;
	if( fileState.Open( "questState.dat", "wb" ) )
	{
		int nSize = m_aStateQuest.GetSize();
		fileState.Write( &nSize, sizeof( nSize ) );
		for( int i = 0;i < m_aStateQuest.GetSize(); i++ )
		{
			int nQuest = m_aStateQuest.GetAt( i );
			fileState.Write( &nQuest, sizeof( nQuest ) );
		}
	}
#endif

#ifdef __CHECK_STATE_QUEST
	m_aStateQuest.RemoveAll();
#endif


#ifdef __PRINT_ITEM0718
/*
3. 상점
4. 드롭
5. 제련
6. 피어싱
*/
// 1	// set
	int cbMonster	= 0;
	MoverProp*	apMonster[4096];
	for( int i = 0; i < m_nMoverPropSize; i++ )
	{
		MoverProp* pMoverProp	= GetMoverProp( i );
		if( pMoverProp && pMoverProp->dwID && pMoverProp->dwBelligerence != BELLI_PEACEFUL && pMoverProp->dwBelligerence != BELLI_ALLIANCE )
			apMonster[cbMonster++]	= pMoverProp;
	}

	int cbWeapon	= 0;
	ItemProp*	apWeapon[1024];
	int cbArmor	= 0;
	ItemProp*	apArmor[4096];

	for( i = 0; i < m_aPropItem.GetSize(); i++ )
	{
		ItemProp* pItemProp	= m_aPropItem.GetAt( i );
		if( pItemProp && pItemProp->dwItemKind3 != IK3_VIRTUAL )
		{
			if( pItemProp->dwItemKind1 == IK1_WEAPON )
				apWeapon[cbWeapon++]	= pItemProp;
			else if( pItemProp->dwItemKind1 == IK1_ARMOR )
				apArmor[cbArmor++]	= pItemProp;
		}
	}
//	1	// sort
	for( i = 0; i < cbMonster - 1; i++ )
	{
		for( int j = i + 1; j < cbMonster; j++ )
		{
			if( (int)apMonster[i]->dwLevel > (int)apMonster[j]->dwLevel )
			{
				MoverProp* tmp	= apMonster[i];
				apMonster[i]	= apMonster[j];
				apMonster[j]	= tmp;
			}
		}
	}

	for( i = 0; i < cbWeapon - 1; i++ )
	{
		for( int j = i + 1; j < cbWeapon; j++ )
		{
			if( (int)apWeapon[i]->dwItemLV > (int)apWeapon[j]->dwItemLV )
			{
				ItemProp* tmp	= apWeapon[i];
				apWeapon[i]	= apWeapon[j];
				apWeapon[j]	= tmp;
			}
		}
	}

	for( i = 0; i < cbArmor - 1; i++ )
	{
		for( int j = i + 1; j < cbArmor; j++ )
		{
			if( (int)apArmor[i]->dwItemLV > (int)apArmor[j]->dwItemLV )
			{
				ItemProp* tmp	= apArmor[i];
				apArmor[i]	= apArmor[j];
				apArmor[j]	= tmp;
			}
		}
	}
// print
	for( int k = 0; k < cbMonster; k++ )
	{
		char lpString[2048]	= { 0, };
		sprintf( lpString, "%d\t%d\t\"%s\"\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d",
			apMonster[k]->dwLevel, apMonster[k]->dwID, apMonster[k]->szName, apMonster[k]->dwHR, apMonster[k]->dwStr, apMonster[k]->dwSta, apMonster[k]->dwDex, apMonster[k]->dwInt,
			apMonster[k]->dwAddHp,
			apMonster[k]->dwNaturalArmor, apMonster[k]->dwResisMgic, apMonster[k]->dwER, apMonster[k]->dwAtk1, apMonster[k]->dwAtkMin, apMonster[k]->dwAtkMax,
			apMonster[k]->eElementType, apMonster[k]->wElementAtk );
		FILEOUT( "monster.txt", lpString );
	}

	for( k = 0; k < MAX_JOB; k++ )
	{
		for( i = 0; i < cbWeapon; i++ )
		{
			if( apWeapon[i]->dwItemJob == k )
			{
				char lpString[2046]	= { 0, };
				sprintf( lpString, "%d\t%d\t%d\t\"%s\"\t%d\t%d\t%d\t%d\t%d\t%f\t%d\t%d", 
					k, apWeapon[i]->dwLimitLevel1, apWeapon[i]->dwID, apWeapon[i]->szName,
					apWeapon[i]->nAddSkillMin, apWeapon[i]->nAddSkillMax,
					apWeapon[i]->dwWeaponType,
					apWeapon[i]->dwAbilityMin, apWeapon[i]->dwAbilityMax, apWeapon[i]->fAttackSpeed, apWeapon[i]->dwItemKind3, apWeapon[i]->dwItemJob );
				FILEOUT( "item.txt", lpString );

			}
		}
	}
	for( i = 0; i < m_aPropAddSkill.GetSize(); i++ )
	{
		AddSkillProp* pAddSkillProp		= m_aPropAddSkill.GetAt( i );
		if( pAddSkillProp )
		{
			char lpString[2048]	= { 0, };

			sprintf( lpString,  "%d\t%d\t%d\t%d\t%d\t%d", 
				i,
				pAddSkillProp->dwID,
				pAddSkillProp->dwSkillLvl,
				pAddSkillProp->dwAbilityMin,
				pAddSkillProp->dwAbilityMax,
				pAddSkillProp->dwSkillReady );
			FILEOUT( "pas.txt", lpString );
		}
	}

	for( i = 0; i < MAX_JOB; i++ )
	{
		for( int j = 0; j < MAX_JOB; j++ )
		{
			ItemProp** apSkillProp	= prj.m_aJobSkill[j];
			int nJobNum	= prj.m_aJobSkillNum[j];

			BOOL b	= FALSE;
			if( j == JOB_VAGRANT || i == j )
				b	= TRUE;
			if( m_aJob[i].dwJobType == JTYPE_PRO && JOB_VAGRANT < j && j < MAX_EXPERT )
			{
				if( j * 2 + 4 == i || j * 2 + 5 == i )
					b	= TRUE;
			}
			if( b )
			{
				for( int k = 0; k < nJobNum; k++ )
				{
					ItemProp* pSkillProp = apSkillProp[k];
					if( ( pSkillProp->dwExeTarget == EXT_MAGICATK || pSkillProp->dwExeTarget == EXT_MAGICATKSHOT ) && pSkillProp->dwSubDefine < 4000 )
					{
						char lpString[2048]	= { 0, };
						sprintf( lpString,  "%d\t%d\t\"%s\"\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d",
							i, pSkillProp->dwID, pSkillProp->szName, pSkillProp->dwSpellType, pSkillProp->dwSubDefine,
							pSkillProp->dwReferStat1, pSkillProp->dwReferStat2, pSkillProp->dwReferTarget1, pSkillProp->dwReferTarget2,
							pSkillProp->dwReferValue1, pSkillProp->dwReferValue2 );
						FILEOUT( "skill.txt", lpString );
					}
				}
			}
		}
	}

	return FALSE;
#endif	// __PRINT_ITEM0718

/*
#ifdef __SHOP_AND_DROP_0802
//1. 상점 아이템
//	- 입력 : 캐릭터 식별자
//	- 출력 : 판매 물품 리스트
//2. 드롭 아이템
//	- 입력 : 몬스터 식별자, 반복자
//	- 출력 : 드롭 물품 리스트

	POSITION pos	= m_mapCharacter.GetStartPosition();
	LPCHARACTER lpCharacter;
	CString string;
	while( pos )
	{
		m_mapCharacter.GetNextAssoc( pos, string, (void*&)lpCharacter );
		FILEOUT( "shop.txt", "[%s]", lpCharacter->m_szKey );
		for( int i = 0; i < MAX_VENDOR_INVENTORY_TAB; i++ )
		{
			int cbSize	= 0;
			ItemProp* apItemProp[MAX_VENDOR_INVENTORY];
			if( lpCharacter->m_venderItemAry[i].GetSize() )
			{

				for( int j = 0; j < lpCharacter->m_venderItemAry[i].GetSize(); j++ )
				{
					LPVENDOR_ITEM pVendor	= (LPVENDOR_ITEM)lpCharacter->m_venderItemAry[i].GetAt( j );
					CPtrArray* pItemKindAry		= GetItemKindAry( pVendor->m_nItemkind3 );
					ItemProp* pItemProp	= NULL;
					
					if( pVendor->m_nUniqueMin < 0 )
					{
						TRACE( "%s, pVendor->m_nUniqueMin < 0\n", lpCharacter->m_szKey );
						continue;
					}
					if( pVendor->m_nUniqueMax < 0 )
					{
						TRACE( "%s, pVendor->m_nUniqueMax < 0\n", lpCharacter->m_szKey );
						continue;
					}

					if( cbSize >= MAX_VENDOR_INVENTORY )
					{
						TRACE( "%s, cbSize >= MAX_VENDOR_INVENTORY\n", lpCharacter->m_szKey );
						continue;
					}
					
					int nMinIdx	= -1, nMaxIdx	= -1;
					
					{
						for( int j = pVendor->m_nUniqueMin; j <= pVendor->m_nUniqueMax; j++ )
						{
							nMinIdx		= prj.GetMinIdx( pVendor->m_nItemkind3, j );
							if( nMinIdx != -1 )
								break;
						}
						for( j = pVendor->m_nUniqueMax; j >= pVendor->m_nUniqueMin; j-- )
						{
							nMaxIdx		= prj.GetMaxIdx( pVendor->m_nItemkind3, j );
							if( nMaxIdx != -1 )
								break;
						}
					}
					
					if( nMinIdx < 0 )
					{
						TRACE( "%s, nMinIdx < 0\n", lpCharacter->m_szKey );
						continue;
					}
					if( nMaxIdx < 0 )
					{
						TRACE( "%s, nMaxIdx < 0\n", lpCharacter->m_szKey );
						continue;
					}

					{
						for( int k = nMinIdx; k <= nMaxIdx; k++ )
						{
							pItemProp	= (ItemProp*)pItemKindAry->GetAt( k );
							if( ( NULL == pItemProp ) ||
								( pItemProp->dwShopAble == (DWORD)-1 ) ||
								( pVendor->m_nItemJob != -1 && (DWORD)pItemProp->dwItemJob != pVendor->m_nItemJob ) )
								continue;
							
							if( cbSize >= MAX_VENDOR_INVENTORY )
								break;

							apItemProp[cbSize]	= pItemProp;
							cbSize++;
						}
					}

					for( int l = 0; l < cbSize - 1; l++ )
					{
						for( int k = l + 1; k < cbSize; k++ )
						{
							if( ( apItemProp[k]->dwItemKind1 < apItemProp[l]->dwItemKind1 ) ||
								( apItemProp[k]->dwItemKind1 == apItemProp[l]->dwItemKind1 && apItemProp[k]->dwItemRare < apItemProp[l]->dwItemRare ) )
							{
								ItemProp* ptmp	= apItemProp[l];
								apItemProp[l]	= apItemProp[k];
								apItemProp[k]	= ptmp;
							}
						}
					}
				}
			}
			//
			{
				for( int i = 0; i < cbSize; i++ )
				{
					FILEOUT( "shop.txt", " \t%s\t%d\t%d\t%d\t%d", apItemProp[i]->szName, apItemProp[i]->dwLimitLevel1, apItemProp[i]->dwItemLV, apItemProp[i]->dwItemRare, apItemProp[i]->dwLimitJob1 );
				}
			}
		}	// for
	}	// while
	return FALSE;
#endif	// __SHOP_AND_DROP_0802
*/

#ifdef __OUTPUT_INFO_0803
	OutputDropItem();
	return FALSE;
#endif	// __OUTPUT_INFO_0803


#ifdef __ADDSKILLPROP0811
	OutputSkill();
	return FALSE;
#endif	// __ADDSKILLPROP0811




#ifdef __ADDSKILLPROP0811
void CProject::OutputSkill( void )
{
	for( int i = 0; i < m_aPropSkill.GetSize(); i++ )
	{
		ItemProp* pSkillProp	= m_aPropSkill.GetAt( i );
		if( pSkillProp && pSkillProp->dwID )
		{
			AddSkillProp* pAddSkillProp	= GetAddSkillProp( pSkillProp->dwSubDefine );
			if( pAddSkillProp )
				FILEOUT( "skilltable.txt", "%d->%d", pSkillProp->dwID, pAddSkillProp->dwID );
		}
	}
}
#endif	// __ADDSKILLPROP0811

#ifdef __OUTPUT_INFO_0803
void CProject::OutputStore( const char* lpStrKey, CMover* pMover  )
{
	LPCHARACTER lpCharacter	= GetCharacter( lpStrKey );
	if( !lpCharacter )
		return;

	FILEOUT( "shop.txt", "%s\t%s\t%.2f\t%.2f", pMover->GetName(), lpCharacter->m_szKey, pMover->GetPos().x, pMover->GetPos().z );
	for( int i = 0; i < MAX_VENDOR_INVENTORY_TAB; i++ )
	{
		int cbSize	= 0;
		ItemProp* apItemProp[MAX_VENDOR_INVENTORY];
		if( lpCharacter->m_venderItemAry[i].GetSize() )
		{

			for( int j = 0; j < lpCharacter->m_venderItemAry[i].GetSize(); j++ )
			{
				LPVENDOR_ITEM pVendor	= (LPVENDOR_ITEM)lpCharacter->m_venderItemAry[i].GetAt( j );
				CPtrArray* pItemKindAry		= GetItemKindAry( pVendor->m_nItemkind3 );
				ItemProp* pItemProp	= NULL;
				
				if( pVendor->m_nUniqueMin < 0 )
				{
					TRACE( "%s, pVendor->m_nUniqueMin < 0\n", lpCharacter->m_szKey );
					continue;
				}
				if( pVendor->m_nUniqueMax < 0 )
				{
					TRACE( "%s, pVendor->m_nUniqueMax < 0\n", lpCharacter->m_szKey );
					continue;
				}

				if( cbSize >= MAX_VENDOR_INVENTORY )
				{
					TRACE( "%s, cbSize >= MAX_VENDOR_INVENTORY\n", lpCharacter->m_szKey );
					continue;
				}
				
				int nMinIdx	= -1, nMaxIdx	= -1;
				
				{
					for( int j = pVendor->m_nUniqueMin; j <= pVendor->m_nUniqueMax; j++ )
					{
						nMinIdx		= prj.GetMinIdx( pVendor->m_nItemkind3, j );
						if( nMinIdx != -1 )
							break;
					}
					for( j = pVendor->m_nUniqueMax; j >= pVendor->m_nUniqueMin; j-- )
					{
						nMaxIdx		= prj.GetMaxIdx( pVendor->m_nItemkind3, j );
						if( nMaxIdx != -1 )
							break;
					}
				}
				
				if( nMinIdx < 0 )
				{
					TRACE( "%s, nMinIdx < 0\n", lpCharacter->m_szKey );
					continue;
				}
				if( nMaxIdx < 0 )
				{
					TRACE( "%s, nMaxIdx < 0\n", lpCharacter->m_szKey );
					continue;
				}

				{
					for( int k = nMinIdx; k <= nMaxIdx; k++ )
					{
						pItemProp	= (ItemProp*)pItemKindAry->GetAt( k );
						if( ( NULL == pItemProp ) ||
							( pItemProp->dwShopAble == (DWORD)-1 ) ||
							( pVendor->m_nItemJob != -1 && (DWORD)pItemProp->dwItemJob != pVendor->m_nItemJob ) )
							continue;
						
						if( cbSize >= MAX_VENDOR_INVENTORY )
							break;

						apItemProp[cbSize]	= pItemProp;
						cbSize++;
					}
				}

				for( int l = 0; l < cbSize - 1; l++ )
				{
					for( int k = l + 1; k < cbSize; k++ )
					{
						if( ( apItemProp[k]->dwItemKind1 < apItemProp[l]->dwItemKind1 ) ||
							( apItemProp[k]->dwItemKind1 == apItemProp[l]->dwItemKind1 && apItemProp[k]->dwItemRare < apItemProp[l]->dwItemRare ) )
						{
							ItemProp* ptmp	= apItemProp[l];
							apItemProp[l]	= apItemProp[k];
							apItemProp[k]	= ptmp;
						}
					}
				}
			}
		}
		//
		{
			for( int i = 0; i < cbSize; i++ )
			{
				FILEOUT( "shop.txt", " \t%s\t%d\t%d\t%d\t%d", apItemProp[i]->szName, apItemProp[i]->dwLimitLevel1, apItemProp[i]->dwItemLV, apItemProp[i]->dwItemRare, apItemProp[i]->dwLimitJob1 );
			}
		}
	}	// for
}



LPCTSTR GetElementTypeString( int n )
{
	char* szString[] = {
		"NEUTRAL",
		"FIRE",
		"WATER",
		"ELECTRICITY",
		"EARTH"
	};

	return szString[n];
}

void CProject::OutputDropItem( void )
{
	const	DROP_COUNT	= 1;
	CString sLog[100];
	int nCount	= 0;
	int nWrite	= 0;
	for( int i = 0; i < m_nMoverPropSize; i++ )
	{
		
		if( (++nCount % 10) == 0 )
		{
			FILE* fp;
			if(!(fp = fopen("drop.txt","a")))
				if(!(fp = fopen("drop.txt" ,"wt")))
					return;
			fprintf(fp,sLog[nWrite++]);
			fclose(fp);
		}
		CString s;
		MoverProp* pMoverProp	= prj.GetMoverProp( i );
		if( pMoverProp && pMoverProp->dwID && pMoverProp->dwBelligerence != BELLI_PEACEFUL && pMoverProp->dwBelligerence != BELLI_ALLIANCE )
		{
			TRACE( "%d\n", i );
			// 최소 공격력	최대 공격력	HP	지급 경험치	속성
			s.Format( "\n%s\t%d\t%d\t%d\t%d\t%d\t%I64d\t%s", pMoverProp->szName, pMoverProp->m_DropItemGenerator.m_dwMax, pMoverProp->dwLevel, pMoverProp->dwAtkMin, pMoverProp->dwAtkMax,  pMoverProp->dwAddHp, pMoverProp->nExpValue, GetElementTypeString( pMoverProp->eElementType ) );
//			QUESTITEM
			/*
			int cbQuest	= (int)pMoverProp->m_QuestItemGenerator.GetSize();
			for( int j = 0; j < cbQuest; j++ )
			{
				QUESTITEM* pQuestItem	= pMoverProp->m_QuestItemGenerator.GetAt( j );
				ItemProp* pItemProp	= prj.GetItemProp( pQuestItem->dwIndex );
				FILEOUT( "drop.txt", "QUESTITEM\t%d\t%d\t%s\t%d\t%f", pQuestItem->dwQuest, pQuestItem->dwState, pItemProp->szName, pQuestItem->dwNumber, pQuestItem->dwProbability / 3000000000 );
			}
			*/
//			DROPITEM
			{
				int cbDropItem	= pMoverProp->m_DropItemGenerator.GetSize();
				int nNumber	= 0;
				DROPITEM* lpDropItem;
				for( int i = 0; i < cbDropItem; i++ )
				{
					if( lpDropItem = pMoverProp->m_DropItemGenerator.GetAt( i, FALSE, false ) )
					{
						if( lpDropItem->dtType == DROPTYPE_NORMAL )
						{
							DWORD dwProbability		= lpDropItem->dwProbability;
							ItemProp* pItemProp	= prj.GetItemProp( lpDropItem->dwIndex );
							if( lpDropItem->dwLevel != 0 )
							{
								if( pItemProp && (int)pItemProp->dwItemLV > 0 )
								{
									int nValue	= dwProbability / pItemProp->dwItemLV;
									if( nValue < 21 )	nValue	= 21;
									dwProbability	= nValue - 20;
								}
							}
							if( pItemProp )
							{
								CString str;
								str.Format( "\n \t%s\t%f%%\t%d", pItemProp->szName, (double)dwProbability / (double)30000000, lpDropItem->dwNumber );
								s	+= str;
							}
						}
						else if( lpDropItem->dtType == DROPTYPE_SEED )
						{
							CString str;
							str.Format(" \tPENYA %d-%d", lpDropItem->dwNumber, lpDropItem->dwNumber2 ); 
							s	+= str;
						}
					}
				}
				sLog[nWrite]	+= s;
			}

//			DROPKIND
			{
				int nSize	= pMoverProp->m_DropKindGenerator.GetSize();
				DROPKIND* pDropKind;
				CPtrArray* pItemKindAry;
				int nAbilityOption, nDropLuck;
				for( int i = 0; i < nSize; i++ )
				{
					pDropKind	= pMoverProp->m_DropKindGenerator.GetAt( i );
					pItemKindAry	= prj.GetItemKindAry( pDropKind->dwIK3 );
					int nMinIdx	= -1,	nMaxIdx		= -1;
					for( int j = pDropKind->nMinUniq; j <= pDropKind->nMaxUniq; j++ )
					{
						nMinIdx		= prj.GetMinIdx( pDropKind->dwIK3, j );
						if( nMinIdx != -1 )
							break;
					}
					for( j = pDropKind->nMaxUniq; j >= pDropKind->nMinUniq; j-- )
					{
						nMaxIdx		= prj.GetMaxIdx( pDropKind->dwIK3, j );
						if( nMaxIdx != -1 )
							break;
					}
					if( nMinIdx < 0 || nMaxIdx < 0 )
						continue;

					for( int a = nMinIdx; a <= nMaxIdx; a++ )
					{
						ItemProp* pItemProp		= (ItemProp*)pItemKindAry->GetAt( a );
						if( !pItemProp )
							continue;

						DWORD dwAdjRand;
						CString s1;
						s1.Format( "\n \t%s", pItemProp->szName );
						for( int k = 10; k >= 0; k-- )
						{
							DWORD dwPrabability		= (DWORD)( prj.m_adwExpDropLuck[( pItemProp->dwItemLV > 120? 119: pItemProp->dwItemLV-1 )][k] * ( (float)pMoverProp->dwCorrectionValue / 100.0f ) );
							CString s2;
							s2.Format( "\t%d=%f%%", k, (double)dwPrabability / (double)30000000 );
							s1	+= s2;
						}
						sLog[nWrite]	+= s1;
					}
				}
			}
		}
	}

	FILE* fp;
	if(!(fp = fopen("drop.txt","a")))
		if(!(fp = fopen("drop.txt" ,"wt")))
			return;
	fprintf(fp,sLog[nWrite]);
	fclose(fp);
}
#endif	// __OUTPUT_INFO_0803


LoadPropQuest()
{

#if defined( __WRITE_STATE_QUEST ) || defined( __CHECK_STATE_QUEST )
				if( nState != 0 && nState != QS_END )
				{
					for( int i = 0; i < m_aStateQuest.GetSize(); i++ )
					{
						if( m_aStateQuest.GetAt( i ) == nQuest )
							break;
					}
#			ifdef __WRITE_STATE_QUEST  
					if( i == m_aStateQuest.GetSize() )
						m_aStateQuest.Add( nQuest );
#			else
#			ifdef __CHECK_STATE_QUEST
					if( i == m_aStateQuest.GetSize() )
					{
						CString strQuest;
						strQuest.Format( "%d Quest에서 %d State 사용됐음! 누가 Quest에서 State 사용하라고 했어! 새 퀘스트에는 사용하지좀 마!", nQuest, nState );
						AfxMessageBox( strQuest );
					}
#			endif
#			endif
				}
#endif

}


/*
void CProject::InterpretMoverAddProp(MoverProp* pMoverPropAry,CScript& script)
{
//	CWordArray* pWordArray;
	CString string;
	int nVal;
	script.GetToken(); // {
	nVal = script.GetNumber(); // Mover Id
	while(*script.token != '}')
	{
		MoverProp* pProp = &pMoverPropAry[nVal];
		if(pProp == NULL)
		{
			TRACE("MoverAddProp에서 존재해야될 base Property %s가 없다. %s\n",script.Token);
			continue;
		}
		script.GetToken(); // {
		script.GetToken(); 
		while(*script.token != '}')
		{
			if(script.Token == ";") 
			{ 
				script.GetToken(); 
				continue; 
			} 
			if(script.Token == "SetEvasion")
			{
				script.GetToken(); // skip (
				pProp->m_nEvasionHP  = script.GetNumber();
				script.GetToken(); // skip ,
				pProp->m_nEvasionSec = script.GetNumber();
				script.GetToken(); // skip )
			}
			else
			if(script.Token == "SetRunAway")
			{
				script.GetToken(); // skip (
				pProp->m_nRunawayHP = script.GetNumber();
				script.GetToken(); // skip ,
				pProp->m_bRunawayToHelper = script.GetNumber();
				script.GetToken(); // skip )
			}
			else
			if(script.Token == "SetCallHelper")
			{
				script.GetToken(); // skip (
				pProp->m_nCallHelperHP = script.GetNumber();
				script.GetToken(); // skip ,
				pProp->m_nCallHelperSec = script.GetNumber();
				script.GetToken(); // skip ,
				pProp->m_nCallHelperId = script.GetNumber();
				script.GetToken(); // skip )
			}
			else
			if(script.Token == "m_nAttackItemNear")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackItemNear  = script.GetNumber();
			}
			else
			if(script.Token == "m_nAttackItemFar")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackItemFar  = script.GetNumber();
			}
			else
			if(script.Token == "m_nAttackItem1")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackItem1  = script.GetNumber();
			}
			else
			if(script.Token == "m_nAttackItem2")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackItem2  = script.GetNumber();
			}
			else
			if(script.Token == "m_nAttackItem3")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackItem3  = script.GetNumber();
			}
			else
			if(script.Token == "m_nAttackItem4")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackItem4  = script.GetNumber();
			}
			else
			if(script.Token == "m_nAttackItemSec")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackItemSec = script.GetNumber();
			}
			else
			if(script.Token == "m_nMagicReflection")
			{
				script.GetToken(); // skip =
				pProp->m_nMagicReflection = script.GetNumber();
			}
			else
			if(script.Token == "m_nImmortality")
			{
				script.GetToken(); // skip =
				pProp->m_nImmortality = script.GetNumber();
			}
			else
			if(script.Token == "m_bBlow")
			{
				script.GetToken(); // skip =
				pProp->m_bBlow = script.GetNumber();
			}
			else
			if(script.Token == "m_nChangeTargetRand")
			{
				script.GetToken(); // skip =
				pProp->m_nChangeTargetRand = script.GetNumber();
			}
			else
			if(script.Token == "randomItem")
				;//InterpretRandomItem(&pProp->m_randomItem,script);
			script.GetToken();
		}
		nVal = script.GetNumber(); // Mover Id
	}
}
*/
/*
struct tagExpCharacter
{
	DWORD dwExp;
	DWORD dwMaxExp;
	DWORD dwLPPoint;

} EXPCHARACTER,* LPEXPCHARACTER

struct tagJobAbility
{
	DWORD dwStr;
	DWORD dwSta;
	DWORD dwDex;
	DWORD dwInt;

} JOBABILITY,* LPJOBABILITY


LPEXPCHARACTER m_apExpCharacter;
LPJOBABILITY m_apJobAbillity[ MAX_JOB ];
LPDWORD m_apExpLPPoint;
LPDWORD m_apExpSkill;

expCharacter // 1 based
{
	   0	0	  1    //1
	1000	0	  2    //2
	1818	0	  2    //3
}
expLPPoint // 0 based
{
1
1
2
}
jobAbillity
{
  JOB_MOOYEGA // 무예가 
  {
    1 2 // str
	1 1 // sta
	1 1 // dex
	2 1 // int
  }
  JOB_VAGRANT // 방랑자
  {
    1 1 // str
	1 1 // sta
	1 1 // dex
	1 1 // int
  }
}



expSkill // 1 based
{
0
4000
9300
}



*/


#ifdef __WORLDSERVER
	BOOL	LoadReNewEventRealItem( LPCTSTR lpszFileName );
	BOOL	LoadReNewEvent( LPCTSTR lpszFileName );
#endif // __WORLDSERVER


/*BOOL	CProject::LoadReNewEventTime( LPCTSTR lpszFileName )
{
	CScript script;
	if( script.Load(lpszFileName) == FALSE )
	{
		Error( "LoadReNewEventTime 열수 없음." );
		return FALSE;	
	}
	int nCount = 0;

	while( script.tok != FINISHED )
	{	
		script.GetToken();
		strcpy( m_aReNewEventTime[nCount].strTime, script.Token );
		for( int i = 0 ; i < MAX_REALITEM ; i++ )
		{
			m_aReNewEventTime[nCount].nRealitem[i] = script.GetNumber();
		}
		++nCount;
	}

	return TRUE;
}
*/
BOOL	CProject::LoadReNewEventRealItem( LPCTSTR lpszFileName )
{
	m_nRealItem[ 0 ] = II_SYS_SYS_EVE_0504M1NOTEBOOK;
	m_nRealItem[ 1 ] = II_SYS_SYS_EVE_0504M2DCAMARA;
	m_nRealItem[ 2 ] = II_SYS_SYS_EVE_0504M3AIRSHIP;   
	m_nRealItem[ 3 ] = II_SYS_SYS_EVE_0504M4USBFANLIGHT;
	m_nRealItem[ 4 ] = II_SYS_SYS_EVE_0504M5BALLOON;
	m_nRealItem[ 5 ] = II_SYS_SYS_EVE_0504M6GIFTTICKET;
	m_nRealItem[ 6 ] = II_SYS_SYS_EVE_0504M7MOVIETICKET;
	m_nRealItem[ 7 ] = II_SYS_SYS_EVE_0504M8OST;
	
	ZeroMemory( m_aRealEventItem, sizeof( m_aRealEventItem ) );
	
	
	CScript script;


#ifdef __IDC
	char FileNameBuf[MAX_PATH] = {0,};
	sprintf( FileNameBuf, "..\\script\\%s", lpszFileName );
	if( script.Load(FileNameBuf) == FALSE )
#else	// __IDC
	if( script.Load(lpszFileName) == FALSE )
#endif	// __IDC
	{
		Error( "0504Event_LevelUp_RealItemList 열수 없음." );
		return FALSE;	
	}

	m_nMaxReNewEventItem = 0;

	int nPercent = 0;
	
#ifdef __S0517
	int nEndTime = script.GetNumber();
#else // __S0517
	script.GetToken();
	CString BufStr = script.Token;	
#endif // __S0517
	int bufLevel = script.GetNumber();
	while( script.tok != FINISHED )
	{	
#ifdef __S0517
		m_aRealEventItem[ m_nMaxReNewEventItem ].EndTime = CTime::CTime( nEndTime / 10000, (nEndTime % 10000) / 100, (nEndTime % 10000) % 100, 0, 0, 0 );
#else // __S0517
		strcpy( m_aRealEventItem[ m_nMaxReNewEventItem ].strTime, BufStr );
#endif // __S0517
		m_aRealEventItem[ m_nMaxReNewEventItem ].nLevel = bufLevel;
		int nTotalPercent = 0;
		for( int i = 0 ; i < MAX_REALITEM ; i++ )
		{ 
			m_aRealEventItem[m_nMaxReNewEventItem].nRealitem[i] = script.GetNumber();
			m_aRealEventItem[m_nMaxReNewEventItem].nRealitemCount[i] = script.GetNumber();
			m_aRealEventItem[m_nMaxReNewEventItem].nSendRealitemCount[i] = 0;
#ifdef __S0512
			nPercent = script.GetFloat() * 100;
			m_aRealEventItem[m_nMaxReNewEventItem].nPercent[i] = nPercent;
#else // __S0512
			nTotalPercent += script.GetFloat() * 100;

			if( nTotalPercent > 10000 )
			{
				AfxMessageBox( "Real Event Percent 100 orverflow" );
			}
			m_aRealEventItem[m_nMaxReNewEventItem].nPercent[i] = nTotalPercent;
#endif // __S0512

#ifdef __S0517
			nEndTime = script.GetNumber();
#else // __S05717
			script.GetToken();
			BufStr = script.Token;
#endif // __S0517
			bufLevel = script.GetNumber();
			if( bufLevel != m_aRealEventItem[ m_nMaxReNewEventItem ].nLevel )
			{
				break;
			}
		}
		m_aRealEventItem[m_nMaxReNewEventItem].nNextIndex = m_nMaxReNewEventItem + 1;
		++m_nMaxReNewEventItem;
		
	} 
	
	return TRUE;
}
BOOL	CProject::LoadReNewEvent( LPCTSTR lpszFileName )
{
	CScript script;

	if( script.Load(lpszFileName) == FALSE )
	{
		Error( "LoadReNewEvent Not Open" );
		return FALSE;	
	}

	DWORD dwCount = 0;
	ZeroMemory( m_aReNewEvent, sizeof( m_aReNewEvent ) );
	ZeroMemory( m_nReNewItemFound, sizeof( m_nReNewItemFound ) );
	m_tReNewEventStartTime = CTime::GetCurrentTime();
	m_tReNewEventEndTime = CTime::GetCurrentTime();

	int nlastLevel = 0;
	float fTotalPercent = 10000;
	
	int nLevel = script.GetNumber();
	while( script.tok != FINISHED )
	{
		m_aReNewEvent[dwCount].nLevel = nLevel;
		if( nlastLevel != m_aReNewEvent[dwCount].nLevel )
		{
			if( fTotalPercent < 9000 )
			{
				Error( "LoadReNewEvent Not 100%" );
				return FALSE;
			}
			nlastLevel = m_aReNewEvent[dwCount].nLevel;
			m_nReNewItemFound[ nlastLevel ] = dwCount;
			fTotalPercent = 0;
		}

		m_aReNewEvent[dwCount].dwItemId = script.GetNumber();
		script.GetToken();
		if( strlen( script.Token ) > 64 )
		{
			Error( "LoadReNewEvent Not Name > 64 %s", script.Token );
			return FALSE;
		}
		strcpy( m_aReNewEvent[dwCount].strItemName, script.Token );
		fTotalPercent += ( script.GetFloat() * 100 );
		m_aReNewEvent[dwCount].fPercent = fTotalPercent;
		nLevel = script.GetNumber();
		dwCount++;
		if( MAX_RENEWEVENT <= dwCount )
		{
			Error( "LoadReNewEvent %d Overflow", MAX_RENEWEVENT );
			return FALSE;
		}
	}
	return TRUE;
}



/*
CPartsMng::CPartsMng()
{
}
CPartsMng::~CPartsMng()
{
}
BOOL CPartsMng::LoadScript( LPCTSTR lpszFileName )
{
	CScript script;
	if( script.Load( lpszFileName ) == FALSE )
		return FALSE;
	//LPHEADSET lpHeadset;
	LPSKINSET lpSkinset;
	int nIndex1;
	int nIndex2;
	m_Gender[ 0 ].m_nHairMeshSize = 0;
	m_Gender[ 1 ].m_nHairMeshSize = 0;
	m_Gender[ 0 ].m_nSkinSetSize = 0;
	m_Gender[ 1 ].m_nSkinSetSize = 0;
	script.GetToken(); 
	int nSex = 0;
	while( script.tok != FINISHED )
	{
		if( script.Token == _T( "gender" ) )
			nSex = script.GetNumber();
		if( script.Token == _T( "hairMesh" ) )
		{
			script.GetToken(); // {
			nIndex1 = script.GetNumber();
			while( *script.token != '}' )
			{
				if( nIndex1 > m_Gender[ nSex ].m_nHairMeshSize )
					m_Gender[ nSex ].m_nHairMeshSize = nIndex1;
				script.GetToken();
				_tcscpy( m_Gender[ nSex ].m_szHairMeshName[ nIndex1 ],  script.token );
				nIndex1 = script.GetNumber();
			}
		}
		else

		if( script.Token == _T( "skinSet" ) )
		{
			script.GetToken(); // {
			nIndex1 = script.GetNumber();
			while( *script.token != '}' )
			{
				if( nIndex1 > m_Gender[ nSex ].m_nSkinSetSize )
					m_Gender[ nSex ].m_nSkinSetSize = nIndex1;
				lpSkinset = &m_Gender[ nSex ].m_skinSet[ nIndex1 ];
				script.GetToken(); // {
				script.GetToken(); _tcscpy( lpSkinset->szHeadMeshName, script.token );
				script.GetToken(); _tcscpy( lpSkinset->szUpperMeshName, script.token );
				script.GetToken(); _tcscpy( lpSkinset->szLowerMeshName, script.token );
				script.GetToken(); _tcscpy( lpSkinset->szHandMeshName, script.token );
				script.GetToken(); _tcscpy( lpSkinset->szFootMeshName, script.token );

				script.GetToken(); _tcscpy( lpSkinset->szUpperTextureName, script.token );
				script.GetToken(); _tcscpy( lpSkinset->szLowerTextureName, script.token );
				script.GetToken(); _tcscpy( lpSkinset->szHandTextureName, script.token );
				script.GetToken(); _tcscpy( lpSkinset->szFootTextureName, script.token );

				//script.GetToken(); _tcscpy( lpSkinset->szUpperUnderName, script.token );
				//script.GetToken(); _tcscpy( lpSkinset->szLowerUnderName, script.token );

				nIndex2 = script.GetNumber();
				while( *script.token != '}' )
				{
					script.GetToken();
					_tcscpy( lpSkinset->szHeadTextureName[ nIndex2 ], script.token );
					nIndex2 = script.GetNumber();
				}
				nIndex1 = script.GetNumber();
			}
		}
		script.GetToken();
	}
	m_Gender[ 0 ].m_nHairMeshSize++;
	m_Gender[ 1 ].m_nHairMeshSize++;
	m_Gender[ 0 ].m_nSkinSetSize++;
	m_Gender[ 1 ].m_nSkinSetSize++;
	return TRUE;
}
LPCTSTR CPartsMng::GetHairMeshName( int nSex, DWORD dwHairMeshIdx )
{
	return m_Gender[ nSex ].m_szHairMeshName[ dwHairMeshIdx ];
}
LPSKINSET CPartsMng::GetSkinSet( int nSex, DWORD dwSkinIdx )
{
	return &m_Gender[ nSex ].m_skinSet[ dwSkinIdx ];
}
*/