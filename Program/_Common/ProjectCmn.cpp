#include "stdafx.h"
#include "lang.h"
#ifdef __LANG_1013
#include "langman.h"
#endif	// __LANG_1013

#if !( defined(__DBSERVER) || defined(__VPW) ) 
	#include "project.h"
#endif	// __DBSERVER

CString GetLangFileName( int nLang, int nType )
{
	char* file[] =
		{ "Filter", "InvalidName", "Notice", "GuildCombatTEXT_1", "GuildCombatTEXT_2", "GuildCombatTEXT_3", "GuildCombatTEXT_4", "GuildCombatTEXT_5", "GuildCombatTEXT_6" 
#ifdef __RULE_0615
			,"Letter"
#endif	// __RULE_0615
#if __VER >= 11 // __GUILD_COMBAT_1TO1
			,"GuildCombat1to1_TEXT1", "GuildCombat1to1_TEXT2", "GuildCombat1to1_TEXT3", "GuildCombat1to1_TEXT4", "GuildCombat1to1_TEXT5"
#endif //__GUILD_COMBAT_1TO1
#ifdef __VENDOR_1106
			, "Letter1"
#endif	// __VENDOR_1106
		};

	CString fileName = file[nType];

#ifdef __LANG_1013
	fileName	= fileName + CLangMan::GetInstance()->GetLangData( nLang )->szFileName;
#else	// __LANG_1013
	switch( nLang )
	{
	case LANG_KOR:
		fileName = fileName+"_KOR.inc";
		break;
	case LANG_USA:
		fileName = fileName+"_ENG.inc";
		break;
	case LANG_JAP:
		fileName = fileName+"_JAP.inc";
		break;
	case LANG_CHI:
		fileName = fileName+"_CHI.inc";
		break;
	case LANG_THA:
		fileName = fileName+"_THA.inc";
		break;
	case LANG_TWN:
	case LANG_HK:
		fileName = fileName+"_TWN.inc";
		break;
	case LANG_VTN:
		fileName = fileName+"_VTN.inc";
		break;
	case LANG_RUS:
		fileName = fileName+"_RUS.inc";
		break;
	default:
		Error( "%s File Error = %d", file[nType], nLang );
		break;
	}
#endif	// __LANG_1013

	return fileName;
}

BOOL CProject::LoadJobItem( LPCTSTR lpszFileName )
{
	CScript script;
	if( script.Load( lpszFileName ) == FALSE )
		return FALSE;
	script.GetToken();
	int nJob, nSex, i, j, nItem;
	memset( m_jobItem, 0xff, sizeof( m_jobItem ) );
	while( script.tok != FINISHED )
	{
		if( script.Token == _T( "job" ) )
		{
			nJob = script.GetNumber();
			script.GetToken(); // {
			script.GetToken(); // sex
			j = 0;
			while( script.Token != _T( "}" ) )
			{
				if( script.Token == _T( "sex" ) )
				{
					nSex = script.GetNumber();
					script.GetToken(); // {
					i = 0; nItem = script.GetNumber();
					while( script.Token != _T( "}" ) )
					{
						m_jobItem[ nJob ].adwMale[ i ][ nSex ] = nItem;
						nItem = script.GetNumber();
						i++;
					} 
				}
				if( script.Token == _T( "item" ) )
				{
					m_jobItem[ nJob ].beginItem[ j ].dwItemType = script.GetNumber();
					m_jobItem[ nJob ].beginItem[ j ].dwItemId   = script.GetNumber();
					m_jobItem[ nJob ].beginItem[ j ].dwNum      = script.GetNumber();
					j++;
				}
				script.GetToken();
			}
		}
		script.GetToken();
	}
	return TRUE;
}

BOOL CProject::LoadPropMover( LPCTSTR lpszFileName )
{
	CScript scanner;
	if( scanner.Load( lpszFileName, FALSE ) == FALSE )
	{
#ifdef __CLIENT
		MessageBox( g_Neuz.GetSafeHwnd(), "프로퍼티 읽기실패 : Mover", "프리프", MB_OK );
		ADDERRORMSG( "프로퍼티 읽기실패 : Mover" );
#endif //__CLIENT
		return FALSE;
	}

	std::set<int> checker;

	TCHAR szLastName[256];
	int i = 0;
	memset( szLastName, 0, sizeof(szLastName) );
	int old;
	while( 1 )
	{
		old = i;
		i = scanner.GetNumber();		// id
		if( scanner.tok == FINISHED )
			break;

		if( i == 0 )
			continue;

		if( i<0 || i>=MAX_PROPMOVER )		// 범위체크 
		{
			Error( "too many mover property or invalid id:%d FILE:%s\n", i, lpszFileName );
			return FALSE;
		}

		if( !checker.insert( i ).second )	// id중복체크 
		{
			Error( "Last read:%s, ID:%d is duplicated. 컬럼이 밀려도 이런에러가 난다. FILE:%s\n", szLastName, i, lpszFileName );
			return FALSE;
		}

		MoverProp* pProperty = &m_pPropMover[i];
		pProperty->dwID	= i;
		
		scanner.GetToken();
		_tcscpy( szLastName, scanner.token );
		_tcscpy( pProperty->szName, scanner.token );

#if !( defined(__DBSERVER) || defined(__VPW) )
		m_mapMVI.insert( map<string, DWORD>::value_type( pProperty->szName, pProperty->dwID ) );
#endif	// __DBSERVER

		pProperty->dwAI					= scanner.GetNumber();
		pProperty->dwStr				= scanner.GetNumber();
		pProperty->dwSta				= scanner.GetNumber();
		pProperty->dwDex				= scanner.GetNumber();
		pProperty->dwInt				= scanner.GetNumber();
		pProperty->dwHR					= scanner.GetNumber();
		pProperty->dwER					= scanner.GetNumber();
		pProperty->dwRace				= scanner.GetNumber();
		pProperty->dwBelligerence		= scanner.GetNumber();
		pProperty->dwGender				= scanner.GetNumber();
		pProperty->dwLevel				= scanner.GetNumber();
		pProperty->dwFlightLevel		= scanner.GetNumber();
		pProperty->dwSize				= scanner.GetNumber();
		pProperty->dwClass				= scanner.GetNumber();
		pProperty->bIfParts				= scanner.GetNumber();

		if( pProperty->bIfParts == NULL_ID )
			Error( "LoadPropMover : bIfParts엔 -1을 넣으면 안된다. %s \r\n컬럼이 밀렸거나 워킹폴더를 잘못지정했을수도있음 ", pProperty->szName );

		pProperty->nChaotic				= scanner.GetNumber();
#ifdef __S1108_BACK_END_SYSTEM
		pProperty->dwUseable			= scanner.GetNumber();
#else // __S1108_BACK_END_SYSTEM
		pProperty->dwDefExtent			= scanner.GetNumber();
#endif // __S1108_BACK_END_SYSTEM
		pProperty->dwActionRadius		= scanner.GetNumber();
		pProperty->dwAtkMin				= scanner.GetNumber();
		pProperty->dwAtkMax				= scanner.GetNumber();
		pProperty->dwAtk1				= scanner.GetNumber();
		pProperty->dwAtk2				= scanner.GetNumber();
		pProperty->dwAtk3				= scanner.GetNumber();
		pProperty->dwAtk4				= scanner.GetNumber();
#if __VER >= 9	//__AI_0509
		pProperty->fFrame	= scanner.GetFloat();	// -1이면 가중치 영향 없음. 1.0 기본 값
		if( abs( -1.0F  - pProperty->fFrame ) < 0.000001F )
			pProperty->fFrame	= 1.0F;
		pProperty->dwOrthograde	= scanner.GetNumber();
#else	// __AI_0509
		pProperty->dwVerticalRate		= scanner.GetNumber();
		pProperty->dwDiagonalRate		= scanner.GetNumber();
#endif	// __AI_0509
		pProperty->dwThrustRate			= scanner.GetNumber();

		pProperty->dwChestRate			= scanner.GetNumber();
		pProperty->dwHeadRate			= scanner.GetNumber();
		pProperty->dwArmRate			= scanner.GetNumber();
		pProperty->dwLegRate			= scanner.GetNumber();

		pProperty->dwAttackSpeed		= scanner.GetNumber();
		pProperty->dwReAttackDelay		= scanner.GetNumber();
		pProperty->dwAddHp				= scanner.GetNumber();
		pProperty->dwAddMp				= scanner.GetNumber();
		pProperty->dwNaturalArmor		= scanner.GetNumber();
		pProperty->nAbrasion			= scanner.GetNumber();
		pProperty->nHardness			= scanner.GetNumber();
		pProperty->dwAdjAtkDelay		= scanner.GetNumber();
		
		pProperty->eElementType			= static_cast<SAI79::ePropType>(scanner.GetNumber());	// 정수타입과 enum	타입이 틀려서 타입 변환을 허용함.
		pProperty->wElementAtk			= static_cast<short>(scanner.GetNumber());				// 정수타입과 short	타입이 틀려서 타입 변환을 허용함.

		pProperty->dwHideLevel			= scanner.GetNumber();
		pProperty->fSpeed				= scanner.GetFloat();
		pProperty->dwShelter			= scanner.GetNumber();
		pProperty->dwFlying				= scanner.GetNumber();
		pProperty->dwJumpIng			= scanner.GetNumber();
		pProperty->dwAirJump			= scanner.GetNumber();
		pProperty->bTaming				= scanner.GetNumber();
		pProperty->dwResisMgic			= scanner.GetNumber();

		pProperty->nResistElecricity	= (int)(scanner.GetFloat() * 100.0f);
		pProperty->nResistFire			= (int)(scanner.GetFloat() * 100.0f);
		pProperty->nResistWind			= (int)(scanner.GetFloat() * 100.0f);
		pProperty->nResistWater			= (int)(scanner.GetFloat() * 100.0f);
		pProperty->nResistEarth			= (int)(scanner.GetFloat() * 100.0f);

		pProperty->dwCash				= scanner.GetNumber();
		pProperty->dwSourceMaterial		= scanner.GetNumber();
		pProperty->dwMaterialAmount		= scanner.GetNumber();
		pProperty->dwCohesion			= scanner.GetNumber();
		pProperty->dwHoldingTime		= scanner.GetNumber();
		pProperty->dwCorrectionValue	= scanner.GetNumber();
		pProperty->nExpValue			= scanner.GetExpInteger();
		pProperty->nFxpValue			= scanner.GetNumber();
		pProperty->nBodyState			= scanner.GetNumber();
		pProperty->dwAddAbility			= scanner.GetNumber();
		pProperty->bKillable			= scanner.GetNumber();
		
		pProperty->dwVirtItem[0]		= scanner.GetNumber();
		pProperty->dwVirtItem[1]		= scanner.GetNumber();
		pProperty->dwVirtItem[2]		= scanner.GetNumber();
		pProperty->bVirtType[0]			= scanner.GetNumber();
		pProperty->bVirtType[1]			= scanner.GetNumber();
		pProperty->bVirtType[2]			= scanner.GetNumber();

		pProperty->dwSndAtk1			= scanner.GetNumber();
		pProperty->dwSndAtk2 			= scanner.GetNumber();

		pProperty->dwSndDie1 			= scanner.GetNumber();
		pProperty->dwSndDie2 			= scanner.GetNumber();

		pProperty->dwSndDmg1 			= scanner.GetNumber();
		pProperty->dwSndDmg2 			= scanner.GetNumber();
		pProperty->dwSndDmg3 			= scanner.GetNumber();

		pProperty->dwSndIdle1 			= scanner.GetNumber();
		pProperty->dwSndIdle2 			= scanner.GetNumber();


		scanner.GetToken();

#if __VER >= 19
		pProperty->dwAreaColor			= scanner.GetNumber();
		scanner.GetToken();
		_tcscpy( pProperty->szNpcMark, scanner.token );
		pProperty->dwMadrigalGiftPoint	= scanner.GetNumber();
#endif

#ifdef __S1108_BACK_END_SYSTEM
		pProperty->m_fHitPoint_Rate = 1.0f;		
		pProperty->m_fAttackPower_Rate = 1.0f;
		pProperty->m_fDefence_Rate = 1.0f;		
		pProperty->m_fExp_Rate = 1.0f;			
		pProperty->m_fItemDrop_Rate = 1.0f;
		pProperty->m_fPenya_Rate = 1.0f;			
		pProperty->m_bRate = FALSE;
#endif // __S1108_BACK_END_SYSTEM
	}

	if( checker.size() > 0 )
		m_nMoverPropSize = *(checker.rbegin()) + 1;
	
	/*
	#ifdef __INTERNALSERVER
	{
		int nIndex = 20; // MI_AIBATT1 
		MoverProp* pProperty = &m_pPropMover[ nIndex ];
		pProperty->dwAI = AII_VER2_TYPE0; 
	}	
	#endif
	*/

	return TRUE;
}

#ifdef __TREASUREBOX
BOOL CProject::LoadTreasureCombine( vector<TreasureBox>* s )
{
	CScript scanner;
	if( scanner.Load( "TreasureCombine.txt" ) == FALSE )
		return FALSE;

	TreasureBox box;
	int nBoxID = 0;
	nBoxID = scanner.GetNumber();

	while( scanner.tok != FINISHED )
	{
		box.dwBoxID = nBoxID;
		for( int i=0;i<3;i++ )
			box.dwKeyID[i] = scanner.GetNumber();

		s->push_back( box );

		nBoxID = 0;
		nBoxID = scanner.GetNumber();
	}

	return TRUE;
}
#ifdef __WORLDSERVER
BOOL CProject::LoadTreasureItem( vector<TreasureItem>* s )
{
	CScript scanner;
	if( scanner.Load( "TreasureItem.txt" ) == FALSE )
		return FALSE;

	TreasureItem item;
	int nKeyID = 0;
	nKeyID = scanner.GetNumber();

	while( scanner.tok != FINISHED )
	{
		item.dwKeyID = nKeyID;
		item.dwItemID = scanner.GetNumber();
		item.nChance = scanner.GetNumber();
		item.nMaxamount = scanner.GetNumber();
		item.nRepeat = scanner.GetNumber();
		item.nLimitamount = scanner.GetNumber();
		item.bNotice = (scanner.GetNumber()!=0?1:0);

		s->push_back( item );

		nKeyID = 0;
		nKeyID = scanner.GetNumber();
	}

	return TRUE;
}
#endif//__WORLDSERVER
#endif//__TREASUREBOX

#ifdef __NEW_PROP_PARAMETER
BOOL CProject::LoadPropItem( LPCTSTR lpszFileName, CFixedArray< ItemProp >* apObjProp )
{
	CScript scanner;
	if( scanner.Load( lpszFileName, FALSE ) == FALSE )
	{
	#ifdef __CLIENT
		MessageBox( g_Neuz.GetSafeHwnd(), "LoadPropItem", "flyff", MB_OK );
		ADDERRORMSG( "LoadPropItem file open error" );
	#endif //__CLIENT
		return FALSE;
	}

	ItemProp prop;
	ZeroMemory( &prop, sizeof(prop) );

	int nVer = scanner.GetNumber();	// version
#ifdef __NEW_STACKABLE_AMPS
	int nAmpCopyID = 60000;
#endif // __NEW_STACKABLE_AMPS
	while( scanner.tok != FINISHED )
	{
		DWORD dwID = scanner.GetNumber();	// IID_
		if( dwID <= 0 )
		{
			Error( "%s ID:%d last_read(ID:%d name:%s)", lpszFileName, dwID, prop.dwID, prop.szName);
			return FALSE;
		}

//#ifdef __CLIENT
		prop.nVer	= nVer;
//#endif	// __CLIENT
		prop.dwID = dwID;
		scanner.GetToken();
//		_tcscpy( prop.szName, scanner.token );
		scanner.Token.TrimRight();
		_tcscpy( prop.szName, (LPCSTR)scanner.Token );

 		prop.dwNum				= scanner.GetNumber();            
		prop.dwPackMax			= scanner.GetNumber();      
		prop.dwItemKind1		= scanner.GetNumber();      
		prop.dwItemKind2		= scanner.GetNumber();      
		prop.dwItemKind3		= scanner.GetNumber();      
		prop.dwItemJob			= scanner.GetNumber();      
		prop.bPermanence		= scanner.GetNumber();      
		prop.dwUseable			= scanner.GetNumber();      
		prop.dwItemSex			= scanner.GetNumber();      
		prop.dwCost				= scanner.GetNumber();      
		prop.dwEndurance		= scanner.GetNumber(); 
		prop.nAbrasion			= scanner.GetNumber();
		prop.nMaxRepair			= scanner.GetNumber();
		prop.dwHanded			= scanner.GetNumber(); 
		prop.dwFlag				= scanner.GetNumber();      
		prop.dwParts			= scanner.GetNumber();      
		prop.dwPartsub			= scanner.GetNumber();      
		prop.bPartsFile			= scanner.GetNumber();      
		prop.dwExclusive		= scanner.GetNumber();      
		prop.dwBasePartsIgnore	= scanner.GetNumber();      
		prop.dwItemLV			= scanner.GetNumber();      
		prop.dwItemRare			= scanner.GetNumber();   
		prop.dwShopAble			= scanner.GetNumber();
		prop.nLog				= scanner.GetNumber();
		prop.bCharged			= scanner.GetNumber();
		prop.bCharged	= ( prop.bCharged != 1? 0: 1 );
		prop.dwLinkKindBullet	= scanner.GetNumber();      
		prop.dwLinkKind			= scanner.GetNumber();      
		prop.dwAbilityMin		= scanner.GetNumber();      
		prop.dwAbilityMax		= scanner.GetNumber();      
		prop.eItemType			= static_cast<SAI79::ePropType>(scanner.GetNumber());
		prop.wItemEatk			= static_cast<short>(scanner.GetNumber());
		prop.dwParry			= scanner.GetNumber();
		prop.dwblockRating		= scanner.GetNumber();
		prop.nAddSkillMin		= scanner.GetNumber();
		prop.nAddSkillMax		= scanner.GetNumber();
		prop.dwAtkStyle			= scanner.GetNumber();      
		prop.dwWeaponType		= scanner.GetNumber();      
		prop.dwItemAtkOrder1	= scanner.GetNumber();      	
		prop.dwItemAtkOrder2	= scanner.GetNumber();      	
		prop.dwItemAtkOrder3	= scanner.GetNumber();      	
		prop.dwItemAtkOrder4	= scanner.GetNumber();      	
		prop.tmContinuousPain	= scanner.GetNumber();      
		prop.nShellQuantity		= scanner.GetNumber();      
		prop.dwRecoil			= scanner.GetNumber();      
		prop.dwLoadingTime		= scanner.GetNumber();      
		prop.nAdjHitRate		= scanner.GetNumber();      
		prop.fAttackSpeed		= scanner.GetFloat();      
		prop.dwDmgShift			= scanner.GetNumber();      
		prop.dwAttackRange		= scanner.GetNumber();      
		prop.nProbability		= scanner.GetNumber();      
		prop.dwDestParam[0]		= scanner.GetNumber();      
		prop.dwDestParam[1]		= scanner.GetNumber();      
#ifdef __PROP_0827
		prop.dwDestParam[2]		= scanner.GetNumber();      
#endif	// __PROP_0827

#if __VER >= 19
		prop.dwDestParam[3]		= scanner.GetNumber(); 
		prop.dwDestParam[4]		= scanner.GetNumber(); 
		prop.dwDestParam[5]		= scanner.GetNumber(); 
#endif//__VER >= 19

		prop.nAdjParamVal[0]	= scanner.GetNumber();      
		prop.nAdjParamVal[1]	= scanner.GetNumber();      
#ifdef __PROP_0827
		prop.nAdjParamVal[2]	= scanner.GetNumber();      
#endif	// __PROP_0827

#if __VER >= 19
		prop.nAdjParamVal[3]	= scanner.GetNumber();   
		prop.nAdjParamVal[4]	= scanner.GetNumber();   
		prop.nAdjParamVal[5]	= scanner.GetNumber();   
#endif//__VER >= 19

		prop.dwChgParamVal[0]	= scanner.GetNumber();      
		prop.dwChgParamVal[1]	= scanner.GetNumber();      
#ifdef __PROP_0827
		prop.dwChgParamVal[2]	= scanner.GetNumber();      
#endif	// __PROP_0827

#if __VER >= 19
		prop.dwChgParamVal[3]	= scanner.GetNumber(); 
		prop.dwChgParamVal[4]	= scanner.GetNumber(); 
		prop.dwChgParamVal[5]	= scanner.GetNumber(); 
#endif//__VER >= 19

		prop.nDestData1[0]		= scanner.GetNumber();      
		prop.nDestData1[1]		= scanner.GetNumber();   
		prop.nDestData1[2]		= scanner.GetNumber(); 

#if __VER >= 19
		prop.nDestData1[3]		= scanner.GetNumber(); 
		prop.nDestData1[4]		= scanner.GetNumber();
		prop.nDestData1[5]		= scanner.GetNumber(); 
#endif//__VER >= 19

		prop.dwActiveSkill		= scanner.GetNumber();
		prop.dwActiveSkillLv	= scanner.GetNumber();
		prop.dwActiveSkillRate	= scanner.GetNumber();      
		prop.dwReqMp			= scanner.GetNumber();      
		prop.dwReqFp			= scanner.GetNumber();      
        prop.dwReqDisLV			= scanner.GetNumber();      
		prop.dwReSkill1			= scanner.GetNumber();
		prop.dwReSkillLevel1	= scanner.GetNumber();
		prop.dwReSkill2			= scanner.GetNumber();
		prop.dwReSkillLevel2	= scanner.GetNumber();
        prop.dwSkillReadyType	= scanner.GetNumber();      
		prop.dwSkillReady		= scanner.GetNumber();      
		prop._dwSkillRange		= scanner.GetNumber();      
		prop.dwSfxElemental		= scanner.GetNumber();      
		prop.dwSfxObj			= scanner.GetNumber();      
		prop.dwSfxObj2			= scanner.GetNumber();      
		prop.dwSfxObj3			= scanner.GetNumber();      
		prop.dwSfxObj4			= scanner.GetNumber();      
		prop.dwSfxObj5			= scanner.GetNumber();      
		prop.dwUseMotion		= scanner.GetNumber();      
		prop.dwCircleTime		= scanner.GetNumber();      
		prop.dwSkillTime		= scanner.GetNumber();
		prop.dwExeTarget		= scanner.GetNumber();      
		prop.dwUseChance		= scanner.GetNumber();      
		prop.dwSpellRegion		= scanner.GetNumber();   
		prop.dwSpellType		= scanner.GetNumber();
		prop.dwReferStat1		= scanner.GetNumber();      
		prop.dwReferStat2		= scanner.GetNumber();      
		prop.dwReferTarget1		= scanner.GetNumber();      
		prop.dwReferTarget2		= scanner.GetNumber();      
		prop.dwReferValue1		= scanner.GetNumber();      
		prop.dwReferValue2		= scanner.GetNumber();      
		prop.dwSkillType		= scanner.GetNumber();      
		prop.nItemResistElecricity	= (int)(scanner.GetFloat() * 100.0f);
		prop.nItemResistFire	    = (int)(scanner.GetFloat() * 100.0f);      
		prop.nItemResistWind	    = (int)(scanner.GetFloat() * 100.0f);      
		prop.nItemResistWater	    = (int)(scanner.GetFloat() * 100.0f);      
		prop.nItemResistEarth	    = (int)(scanner.GetFloat() * 100.0f);      
		prop.nEvildoing			= scanner.GetNumber();      
		prop.dwExpertLV			= scanner.GetNumber();      
		prop.dwExpertMax		= scanner.GetNumber();   
		prop.dwSubDefine		= scanner.GetNumber();
		prop.dwExp				= scanner.GetNumber();      
		prop.dwComboStyle		= scanner.GetNumber();      
		prop.fFlightSpeed		= scanner.GetFloat();      
		prop.fFlightLRAngle		= scanner.GetFloat();      
		prop.fFlightTBAngle		= scanner.GetFloat();      
		prop.dwFlightLimit		= scanner.GetNumber();      
		prop.dwFFuelReMax		= scanner.GetNumber();      
		prop.dwAFuelReMax		= scanner.GetNumber();      
		prop.dwFuelRe			= scanner.GetNumber();      
		prop.dwLimitLevel1		= scanner.GetNumber();
		prop.nReflect			= scanner.GetNumber();      
		prop.dwSndAttack1		= scanner.GetNumber();     
		prop.dwSndAttack2 		= scanner.GetNumber();    
		
		// szIcon
		scanner.GetToken();
		scanner.GetToken();	
	#ifdef __CLIENT		
		_tcsncpy( prop.szIcon, scanner.token, sizeof(prop.szIcon)-1 );
		prop.szCommand[sizeof(prop.szIcon)-1] = 0;
	#endif 

#ifdef __VERIFY_0201
		_tcsncpy( prop.szIcon, scanner.token, sizeof(prop.szIcon)-1 );
		prop.szCommand[sizeof(prop.szIcon)-1] = 0;
#endif	// __VERIFY_0201

		scanner.GetToken();

		// dwQuestId
		prop.dwQuestId	= scanner.GetNumber(  );

		// szTextFileName 
		scanner.GetToken();
		scanner.GetToken(); 
		_tcscpy( prop.szTextFileName, scanner.token );
		scanner.GetToken();

		// szCommand
		scanner.GetToken();		
	#ifdef __CLIENT		
		_tcsncpy( prop.szCommand, scanner.token, sizeof(prop.szCommand)-1 );
		prop.szCommand[sizeof(prop.szCommand)-1] = 0;
	#endif // __CLIENT

		prop.nMinLimitLevel 		= scanner.GetNumber();
		prop.nMaxLimitLevel 		= scanner.GetNumber();
		prop.nItemGroup 			= scanner.GetNumber();
		prop.nUseLimitGroup 		= scanner.GetNumber();
		prop.nMaxDuplication 		= scanner.GetNumber();
		prop.nEffectValue 			= scanner.GetNumber();
		prop.nTargetMinEnchant 		= scanner.GetNumber();
		prop.nTargetMaxEnchant 		= scanner.GetNumber();
		prop.bResetBind	 			= scanner.GetNumber();
		prop.nBindCondition 		= scanner.GetNumber();
		prop.nResetBindCondition 	= scanner.GetNumber();
		prop.dwHitActiveSkillId 	= scanner.GetNumber();
		prop.dwHitActiveSkillLv 	= scanner.GetNumber();
		prop.dwHitActiveSkillProb 	= scanner.GetNumber();
		prop.dwHitActiveSkillTarget = scanner.GetNumber();
		prop.dwDamageActiveSkillId 	= scanner.GetNumber();
		prop.dwDamageActiveSkillLv 	= scanner.GetNumber();
		prop.dwDamageActiveSkillProb	= scanner.GetNumber();
		prop.dwDamageActiveSkillTarget	= scanner.GetNumber();
		prop.dwEquipActiveSkillId 	= scanner.GetNumber();
		prop.dwEquipActiveSkillLv 	= scanner.GetNumber();
		prop.dwSmelting 			= scanner.GetNumber();
		prop.dwAttsmelting 			= scanner.GetNumber();
		prop.dwGemsmelting 			= scanner.GetNumber();
		prop.dwPierce		 		= scanner.GetNumber();
		prop.dwUprouse		 		= scanner.GetNumber();
		prop.bAbsoluteTime		 	= scanner.GetNumber();

#if __VER >= 18
		prop.dwItemGrade			= scanner.GetNumber();
		prop.bCanTrade				= scanner.GetNumber();
		prop.dwMainCategory			= scanner.GetNumber();
		prop.dwSubCategory			= scanner.GetNumber();
		prop.bCanHaveServerTransform = scanner.GetNumber();
		prop.bCanSavePotion	 		= scanner.GetNumber();
		prop.bCanLooksChange		= scanner.GetNumber();
		prop.blsLooksChangeMaterial = scanner.GetNumber();
#endif//__VER >= 18

	#ifdef __WORLDSERVER
		#ifdef __INTERNALSERVER
			prop.nLog = -1;		// 개발섭에선 이거 무시.
		#endif
		if( g_eLocal.GetState( EVE_SCHOOL ) )
		{
			prop.dwShopAble	= 1;
			if( prop.dwItemRare == (DWORD)-1 )
				prop.dwItemRare		= prop.dwItemLV;
		}
	#endif	// __WORLDSERVER

		if( ::GetLanguage() == LANG_THA )
		{
			if( prop.dwLimitLevel1 > 0 && prop.dwLimitLevel1 <= 5 )
				prop.dwLimitLevel1	= 1;
			else if( prop.dwLimitLevel1 > 5 && prop.dwLimitLevel1 <= 15 )
				prop.dwLimitLevel1	= 5;
			else if( prop.dwLimitLevel1 > 15 && prop.dwLimitLevel1 <= 30 )
				prop.dwLimitLevel1	= 15;
			else if( prop.dwLimitLevel1 > 30 && prop.dwLimitLevel1 <= 45 )
				prop.dwLimitLevel1	= 30;
			else if( prop.dwLimitLevel1 > 45 && prop.dwLimitLevel1 < 60 )
				prop.dwLimitLevel1	= 45;
		}
// temp crossbow fix
		if( prop.dwID == II_WEA_BOW_BEHECROSSBOW ) 
			prop.dwItemKind3 = IK3_CROSSBOW;
// end fix	
		if( nVer <= __VER )
		{
//			TRACE( "PropItem = %d, %s\n", prop.dwID, prop.szName );
			m_mapII.insert( map<string, DWORD>::value_type( prop.szName, prop.dwID ) );
#if __VER >= 9	// __ULTIMATE
#ifdef __CLIENT
			if( prop.IsUltimate() )
			{
				CString strName	= prop.szName;
				int	nFind	= strName.Find( "@", 0 );
				if( nFind > -1 )
				{
					strName.Delete( nFind - 1, 2 );
					lstrcpy( prop.szName, (LPCTSTR)strName );
				}
			}
#endif	// __CLIENT
#endif	// __ULTIMATE
			apObjProp->SetAtGrow( prop.dwID, &prop);
#ifdef __NEW_STACKABLE_AMPS
			if( prop.IsExperienceScroll())
			{
				if( prop.nMaxDuplication > 1 )
				{
					int nOrginalID = prop.dwID;
					int nCopy[4];
					nCopy[0], nCopy[1], nCopy[2], nCopy[3] = 0;
					prop.dwID = nAmpCopyID;

					for( int i = 0; i < (prop.nMaxDuplication-1); i++ )
					{
						nCopy[i] = prop.dwID;
						m_mapII.insert( map<string, DWORD>::value_type( prop.szName, prop.dwID ) );
						apObjProp->SetAtGrow( prop.dwID, &prop );

						prop.dwID++;
						nAmpCopyID++;
					}
					AMP_STORE_INFO* pAmpInfo = new AMP_STORE_INFO;
					prj.m_mapAmpStore.insert( make_pair( nOrginalID, pAmpInfo ));
					pAmpInfo->dwIDSecond	= nCopy[0];
					pAmpInfo->dwIDThird		= nCopy[1];
					pAmpInfo->dwIDFourth	= nCopy[2];
					pAmpInfo->dwIDFifth		= nCopy[3];
				}
			}
#endif // __NEW_STACKABLE_AMPS
		}

		nVer = scanner.GetNumber();	// version; 
	}
	apObjProp->Optimize();
	return TRUE;
}
#endif // __NEW_PROP_PARAMETER

#ifdef __NEW_PROP_PARAMETER
BOOL CProject::LoadPropItemEx( LPCTSTR lpszFileName, CFixedArray< ItemProp >* apObjProp )
#else // __NEW_PROP_PARAMETER
BOOL CProject::LoadPropItem( LPCTSTR lpszFileName, CFixedArray< ItemProp >* apObjProp )
#endif // __NEW_PROP_PARAMETER
{
	CScript scanner;
	if( scanner.Load( lpszFileName, FALSE ) == FALSE )
	{
	#ifdef __CLIENT
		MessageBox( g_Neuz.GetSafeHwnd(), "LoadPropItem", "flyff", MB_OK );
		ADDERRORMSG( "LoadPropItem file open error" );
	#endif //__CLIENT
		return FALSE;
	}

	ItemProp prop;
	ZeroMemory( &prop, sizeof(prop) );

	int nVer = scanner.GetNumber();	// version
	while( scanner.tok != FINISHED )
	{
		DWORD dwID = scanner.GetNumber();	// IID_
		if( dwID <= 0 )
		{
			Error( "%s ID:%d last_read(ID:%d name:%s)", lpszFileName, dwID, prop.dwID, prop.szName);
			return FALSE;
		}

//#ifdef __CLIENT
		prop.nVer	= nVer;
//#endif	// __CLIENT
		prop.dwID = dwID;
		scanner.GetToken();
//		_tcscpy( prop.szName, scanner.token );
		scanner.Token.TrimRight();
		_tcscpy( prop.szName, (LPCSTR)scanner.Token );

 		prop.dwNum				= scanner.GetNumber();            
		prop.dwPackMax			= scanner.GetNumber();      
		prop.dwItemKind1		= scanner.GetNumber();      
		prop.dwItemKind2		= scanner.GetNumber();      
		prop.dwItemKind3		= scanner.GetNumber();      
		prop.dwItemJob			= scanner.GetNumber();      
		prop.bPermanence		= scanner.GetNumber();      
		prop.dwUseable			= scanner.GetNumber();      
		prop.dwItemSex			= scanner.GetNumber();      
		prop.dwCost				= scanner.GetNumber();      
		prop.dwEndurance		= scanner.GetNumber(); 
		prop.nAbrasion			= scanner.GetNumber();
		prop.nMaxRepair			= scanner.GetNumber();
		prop.dwHanded			= scanner.GetNumber();      
		prop.dwFlag				= scanner.GetNumber();      
		prop.dwParts			= scanner.GetNumber();      
		prop.dwPartsub			= scanner.GetNumber();      
		prop.bPartsFile			= scanner.GetNumber();      
		prop.dwExclusive		= scanner.GetNumber();      
		prop.dwBasePartsIgnore	= scanner.GetNumber();      
		prop.dwItemLV			= scanner.GetNumber();      
		prop.dwItemRare			= scanner.GetNumber();   
		prop.dwShopAble			= scanner.GetNumber();
		prop.nLog				= scanner.GetNumber();
		prop.bCharged			= scanner.GetNumber();
		prop.bCharged	= ( prop.bCharged != 1? 0: 1 );
		prop.dwLinkKindBullet	= scanner.GetNumber();      
		prop.dwLinkKind			= scanner.GetNumber();      
		prop.dwAbilityMin		= scanner.GetNumber();      
		prop.dwAbilityMax		= scanner.GetNumber();      
		prop.eItemType			= static_cast<SAI79::ePropType>(scanner.GetNumber());
		prop.wItemEatk			= static_cast<short>(scanner.GetNumber());
		prop.dwParry			= scanner.GetNumber();
		prop.dwblockRating		= scanner.GetNumber();
		prop.nAddSkillMin		= scanner.GetNumber();
		prop.nAddSkillMax		= scanner.GetNumber();
		prop.dwAtkStyle			= scanner.GetNumber();      
		prop.dwWeaponType		= scanner.GetNumber();      
		prop.dwItemAtkOrder1	= scanner.GetNumber();      	
		prop.dwItemAtkOrder2	= scanner.GetNumber();      	
		prop.dwItemAtkOrder3	= scanner.GetNumber();      	
		prop.dwItemAtkOrder4	= scanner.GetNumber();      	
		prop.tmContinuousPain	= scanner.GetNumber();      
		prop.nShellQuantity		= scanner.GetNumber();      
		prop.dwRecoil			= scanner.GetNumber();      
		prop.dwLoadingTime		= scanner.GetNumber();      
		prop.nAdjHitRate		= scanner.GetNumber();      
		prop.fAttackSpeed		= scanner.GetFloat();      
		prop.dwDmgShift			= scanner.GetNumber();      
		prop.dwAttackRange		= scanner.GetNumber();      
		prop.nProbability		= scanner.GetNumber();      
		prop.dwDestParam[0]		= scanner.GetNumber();      
		prop.dwDestParam[1]		= scanner.GetNumber();      
#ifdef __PROP_0827
		prop.dwDestParam[2]		= scanner.GetNumber();      
#endif	// __PROP_0827

		prop.nAdjParamVal[0]	= scanner.GetNumber();      
		prop.nAdjParamVal[1]	= scanner.GetNumber();      
#ifdef __PROP_0827
		prop.nAdjParamVal[2]	= scanner.GetNumber();      
#endif	// __PROP_0827

		prop.dwChgParamVal[0]	= scanner.GetNumber();      
		prop.dwChgParamVal[1]	= scanner.GetNumber();      
#ifdef __PROP_0827
		prop.dwChgParamVal[2]	= scanner.GetNumber();      
#endif	// __PROP_0827

		prop.nDestData1[0]		= scanner.GetNumber();      
		prop.nDestData1[1]		= scanner.GetNumber();   
		prop.nDestData1[2]		= scanner.GetNumber();  

		prop.dwActiveSkill		= scanner.GetNumber();
		prop.dwActiveSkillLv	= scanner.GetNumber();
		prop.dwActiveSkillRate	= scanner.GetNumber();      
		prop.dwReqMp			= scanner.GetNumber();      
		prop.dwReqFp			= scanner.GetNumber();      
        prop.dwReqDisLV			= scanner.GetNumber();      
		prop.dwReSkill1			= scanner.GetNumber();
		prop.dwReSkillLevel1	= scanner.GetNumber();
		prop.dwReSkill2			= scanner.GetNumber();
		prop.dwReSkillLevel2	= scanner.GetNumber();
        prop.dwSkillReadyType	= scanner.GetNumber();      
		prop.dwSkillReady		= scanner.GetNumber();      
		prop._dwSkillRange		= scanner.GetNumber();      
		prop.dwSfxElemental		= scanner.GetNumber();      
		prop.dwSfxObj			= scanner.GetNumber();      
		prop.dwSfxObj2			= scanner.GetNumber();      
		prop.dwSfxObj3			= scanner.GetNumber();      
		prop.dwSfxObj4			= scanner.GetNumber();      
		prop.dwSfxObj5			= scanner.GetNumber();      
		prop.dwUseMotion		= scanner.GetNumber();      
		prop.dwCircleTime		= scanner.GetNumber();      
		prop.dwSkillTime		= scanner.GetNumber();
		prop.dwExeTarget		= scanner.GetNumber();      
		prop.dwUseChance		= scanner.GetNumber();      
		prop.dwSpellRegion		= scanner.GetNumber();   
		prop.dwSpellType		= scanner.GetNumber();
		prop.dwReferStat1		= scanner.GetNumber();      
		prop.dwReferStat2		= scanner.GetNumber();      
		prop.dwReferTarget1		= scanner.GetNumber();      
		prop.dwReferTarget2		= scanner.GetNumber();      
		prop.dwReferValue1		= scanner.GetNumber();      
		prop.dwReferValue2		= scanner.GetNumber();      
		prop.dwSkillType		= scanner.GetNumber();      
		prop.nItemResistElecricity	= (int)(scanner.GetFloat() * 100.0f);
		prop.nItemResistFire	    = (int)(scanner.GetFloat() * 100.0f);      
		prop.nItemResistWind	    = (int)(scanner.GetFloat() * 100.0f);      
		prop.nItemResistWater	    = (int)(scanner.GetFloat() * 100.0f);      
		prop.nItemResistEarth	    = (int)(scanner.GetFloat() * 100.0f);      
		prop.nEvildoing			= scanner.GetNumber();      
		prop.dwExpertLV			= scanner.GetNumber();      
		prop.dwExpertMax		= scanner.GetNumber();   
		prop.dwSubDefine		= scanner.GetNumber();
		prop.dwExp				= scanner.GetNumber();      
		prop.dwComboStyle		= scanner.GetNumber();      
		prop.fFlightSpeed		= scanner.GetFloat();      
		prop.fFlightLRAngle		= scanner.GetFloat();      
		prop.fFlightTBAngle		= scanner.GetFloat();      
		prop.dwFlightLimit		= scanner.GetNumber();      
		prop.dwFFuelReMax		= scanner.GetNumber();      
		prop.dwAFuelReMax		= scanner.GetNumber();      
		prop.dwFuelRe			= scanner.GetNumber();      
		prop.dwLimitLevel1		= scanner.GetNumber();
		prop.nReflect			= scanner.GetNumber();      
		prop.dwSndAttack1		= scanner.GetNumber();     
		prop.dwSndAttack2		= scanner.GetNumber();     
		
		// szIcon
		scanner.GetToken();
		scanner.GetToken();	
	#ifdef __CLIENT		
		_tcsncpy( prop.szIcon, scanner.token, sizeof(prop.szIcon)-1 );
		prop.szCommand[sizeof(prop.szIcon)-1] = 0;
	#endif 

#ifdef __VERIFY_0201
		_tcsncpy( prop.szIcon, scanner.token, sizeof(prop.szIcon)-1 );
		prop.szCommand[sizeof(prop.szIcon)-1] = 0;
#endif	// __VERIFY_0201

		scanner.GetToken();

		// dwQuestId
		prop.dwQuestId	= scanner.GetNumber(  );

		// szTextFileName 
		scanner.GetToken();
		scanner.GetToken(); 
		_tcscpy( prop.szTextFileName, scanner.token );
		scanner.GetToken();

		// szCommand
		scanner.GetToken();		
	#ifdef __CLIENT		
		_tcsncpy( prop.szCommand, scanner.token, sizeof(prop.szCommand)-1 );
		prop.szCommand[sizeof(prop.szCommand)-1] = 0;
	#endif // __CLIENT

#ifdef __NEW_PROP_PARAMETER
		prop.dwBuffTickType		= scanner.GetNumber();
#endif // __NEW_PROP_PARAMETER

#if __VER >= 18
		prop.dwMonsterGrade		= scanner.GetNumber();
		prop.dwEquipItemKeepSkill = scanner.GetNumber();
#endif//__VER >= 18

#if __VER >= 19
		prop.bCanUseActionSlot = scanner.GetNumber();
#endif//__VER >= 19

	#ifdef __WORLDSERVER
		#ifdef __INTERNALSERVER
			prop.nLog = -1;		// 개발섭에선 이거 무시.
		#endif
		if( g_eLocal.GetState( EVE_SCHOOL ) )
		{
			prop.dwShopAble	= 1;
			if( prop.dwItemRare == (DWORD)-1 )
				prop.dwItemRare		= prop.dwItemLV;
		}
	#endif	// __WORLDSERVER

		if( ::GetLanguage() == LANG_THA )
		{
			if( prop.dwLimitLevel1 > 0 && prop.dwLimitLevel1 <= 5 )
				prop.dwLimitLevel1	= 1;
			else if( prop.dwLimitLevel1 > 5 && prop.dwLimitLevel1 <= 15 )
				prop.dwLimitLevel1	= 5;
			else if( prop.dwLimitLevel1 > 15 && prop.dwLimitLevel1 <= 30 )
				prop.dwLimitLevel1	= 15;
			else if( prop.dwLimitLevel1 > 30 && prop.dwLimitLevel1 <= 45 )
				prop.dwLimitLevel1	= 30;
			else if( prop.dwLimitLevel1 > 45 && prop.dwLimitLevel1 < 60 )
				prop.dwLimitLevel1	= 45;
		}
		
		if( nVer <= __VER )
		{
//			TRACE( "PropItem = %d, %s\n", prop.dwID, prop.szName );
			m_mapII.insert( map<string, DWORD>::value_type( prop.szName, prop.dwID ) );
#if __VER >= 9	// __ULTIMATE
#ifdef __CLIENT
			if( prop.IsUltimate() )
			{
				CString strName	= prop.szName;
				int	nFind	= strName.Find( "@", 0 );
				if( nFind > -1 )
				{
					strName.Delete( nFind - 1, 2 );
					lstrcpy( prop.szName, (LPCTSTR)strName );
				}
			}
#endif	// __CLIENT
#endif	// __ULTIMATE
			apObjProp->SetAtGrow( prop.dwID, &prop);
		}

		nVer = scanner.GetNumber();	// version; 
	}
	apObjProp->Optimize();
	return TRUE;
}

BOOL CProject::LoadText( LPCTSTR lpszFileName )
{
	tagColorText colorText, *pColorText;

	CScript scanner;
	if( scanner.Load( lpszFileName ) == FALSE )
		return FALSE;

	CStringArray strArray;
	CDWordArray  colorArray;
	DWORD dwId = scanner.GetNumber();

	do {
		DWORD dwColor = scanner.GetNumber(); // color
		scanner.GetToken();
		if( *scanner.token  == '{' )
		{

			scanner.GetToken();
			CString str	= scanner.token;
			str.Replace( "\"", "" );
//			if( str.IsEmpty() )
//				str	= "Empty";
			#ifdef _DEBUG
			if( strArray.GetSize() > (int)( dwId ) )
				if( strArray.GetAt( dwId ).IsEmpty() == FALSE )
					Error( "CProject::LoadText : 같은 아이디 존재 %d - %s", dwId, str );						
			#endif	// _DEBUG	
			strArray.SetAtGrow( dwId, str );
			colorArray.SetAtGrow( dwId, dwColor );
			scanner.GetToken();	// }
		}
		dwId = scanner.GetNumber();	// next
	} while( scanner.tok != FINISHED );

	for( int i = 0; i < strArray.GetSize(); i++ )
	{
		if( strArray.GetAt( i ).IsEmpty() == FALSE )
		{
			m_colorText.SetAtGrow( i, &colorText );

			pColorText = m_colorText.GetAt( i );
			pColorText->dwColor = colorArray.GetAt( i );
			pColorText->lpszData = strdup( strArray.GetAt( i ) ) ;
		}
	}
	m_colorText.Optimize();
	return TRUE;
}

CString CProject::GetLangScript( CScript& script )
{
	CString string;
	script.GetToken();
	string	= script.Token;
	script.GetToken();	// )
	script.GetToken(); // ;
	if( string.IsEmpty() )
		string = " ";
	return string;
}

void CProject::LoadStrings()
{
	char* lpszFiles[] = {
		"character.txt.txt",
		"character-etc.txt.txt",
		"character-school.txt.txt",
		"etc.txt.txt",
		"propCtrl.txt.txt",
		"propItem.txt.txt",
		"propItemEtc.txt.txt",
		"propKarma.txt.txt",
		"propMotion.txt.txt",
		"propMover.txt.txt",
		"propQuest.txt.txt",
		"propQuest-DungeonandPK.txt.txt",
		"propQuest-RequestBox2.txt.txt",
		"propQuest-RequestBox.txt.txt",
		"propQuest-Scenario.txt.txt",
		"propSkill.txt.txt",
		"propTroupeSkill.txt.txt",
		"resData.txt.txt",
		"textClient.txt.txt",
		"textEmotion.txt.txt",
		"world.txt.txt",
		"Client\\treeHelp.txt.txt",
		"Client\\tip.txt.txt",
		"Client\\PatchClient.txt.txt",
		"Client\\InstantHelp.txt.txt",
		"Client\\help.txt.txt",
		"Client\\Guide.txt.txt",
#if __VER >= 12 // __MOD_TUTORIAL
		"Client\\tutorial.txt.txt",
#endif
//		"Client\\GameGuard.txt.txt",
		"Client\\faq.txt.txt",
		"World\\WdVolcane\\WdVolcane.txt.txt",
		"World\\WdMadrigal\\wdMadrigal.txt.txt",
		"World\\WdKebaras\\WdKebaras.txt.txt",
		"World\\WdGuildWar\\WdGuildWar.txt.txt",
		"World\\WdEvent01\\WdEvent01.txt.txt",
		"World\\DuMuscle\\DuMuscle.txt.txt",
		"World\\DuKrr\\DuKrr.txt.txt",
		"World\\DuFlMas\\DuFlMas.txt.txt",
		"World\\DuDaDk\\DuDaDk.txt.txt",
		"World\\DuBear\\DuBear.txt.txt",
		"World\\DuSaTemple\\DuSaTemple.txt.txt",
		"World\\DuSaTempleBoss\\DuSaTempleBoss.txt.txt"
#if __VER >= 9	// __JEFF_9
		,"World\\WdVolcane\\WdVolcane.txt.txt"
		,"World\\WdVolcaneRed\\WdVolcaneRed.txt.txt"
		,"World\\WdVolcaneYellow\\WdVolcaneYellow.txt.txt"
#endif	// __JEFF_9
#ifdef __JEFF_11_4
		,"World\\WdArena\\WdArena.txt.txt"
#endif	// __JEFF_11_4
#if __VER >= 12 // __LORD
		,"lordskill.txt.txt"
#endif	// __LORD
#if __VER >= 12 // __SECRET_ROOM
		,"World\\WdHeaven01\\wdheaven01.txt.txt"
		,"World\\WdHeaven02\\wdheaven02.txt.txt"
		,"World\\WdHeaven03\\wdheaven03.txt.txt"
		,"World\\WdHeaven04\\wdheaven04.txt.txt"
		,"World\\WdHeaven05\\wdheaven05.txt.txt"
		,"World\\WdHeaven06\\wdheaven06.txt.txt"
		,"World\\WdHeaven06_1\\wdheaven06_1.txt.txt"
#endif // __SECRET_ROOM
#if __VER >= 13 // __HONORABLE_TITLE
		, "honorList.txt.txt"
#endif	// __HONORABLE_TITLE
#ifdef __AZRIA_1023
		, "World\\WdCisland\\WdCisland.txt.txt"
#endif // __AZRIA_1023
#if __VER >= 14 // __INSTANCE_DUNGEON
		, "World\\DuOminous\\duominous.txt.txt"
		, "World\\DuOminous_1\\duominous_1.txt.txt"
#endif // __INSTANCE_DUNGEON
#if __VER >= 15 // __GUILD_HOUSE
		, "World\\WdGuildhousesmall\\WdGuildhousesmall.txt.txt"		// 소형 길드하우스
		, "World\\WdGuildhousemiddle\\WdGuildhousemiddle.txt.txt"	// 중형 길드하우스
		, "World\\WdGuildhouselarge\\WdGuildhouselarge.txt.txt"		// 대형 길드하우스
		, "World\\DuDreadfulCave\\DuDreadfulCave.txt.txt"			// 추가 인던(드래드풀 케이브)
		, "World\\DuRustia\\DuRustia.txt.txt"						// 추가 인던(러스티아 일반)
		, "World\\DuRustia_1\\DuRustia_1.txt.txt"					// 추가 인던(러스티아 마스터)
#endif // __GUILD_HOUSE
#ifdef __IMPROVE_MAP_SYSTEM
		, "propMapComboBoxData.txt.txt"
#endif // __IMPROVE_MAP_SYSTEM
#if __VER >= 16
		, "World\\DuBehamah\\DuBehamah.txt.txt"
#endif // __VER 16
#if __VER >= 17
		, "World\\WdColosseum\\WdColosseum.txt.txt"
		, "World\\DuKalgas\\DuKalgas.txt.txt"
//		, "World\\WdArena_1\\WdArena_1.txt.txt"
#endif // __VER 17
#if __VER >= 18
		, "World\\DuHerneos\\DuHerneos.txt.txt"
		, "World\\DuHerneos_1\\DuHerneos_1.txt.txt"
		, "World\\DuSanpres\\DuSanpres.txt.txt"
		, "World\\DuSanpres_1\\DuSanpres_1.txt.txt"
		, "World\\DuUpresia\\DuUpresia.txt.txt"
		, "World\\DuUpresia_1\\DuUpresia_1.txt.txt"
		, "World\\WdRartesia\\WdRartesia.txt.txt"
#endif // __VER 18
#if __VER >= 19
		, "World\\WdDarkRartesia\\WdDarkRartesia.txt.txt"
		, "World\\WdFwc\\WdFwc.txt.txt"
		, "World\\WdMarket\\WdMarket.txt.txt"
#endif // __VER 19
	};

	int n = sizeof(lpszFiles) / sizeof(lpszFiles[0]);
	for( int i=0; i<n; ++i )
	{
		CScript s;
		if( s.Load( lpszFiles[i] ) == TRUE )
			s.LoadString();
	}		

	LoadText( "textClient.inc" );
}

void CProject::LoadDefines()
{
	char* lpszDefines[] = {
		"define.h",
		"defineNeuz.h",
		"defineQuest.h",
		"defineJob.h",
		"defineItem.h",
		"defineWorld.h",
		"defineItemkind.h",
		"lang.h",
		"defineObj.h",
		"defineAttribute.h",
		"defineSkill.h",
		"defineText.h",
		"defineSound.h",
		"resdata.h",
		"WndStyle.h"
#if __VER >= 12 // __LORD
		, "definelordskill.h"
#endif	// __LORD
#if __VER >= 13 // __HONORABLE_TITLE
		, "defineHonor.h"
#endif	// __HONORABLE_TITLE
#ifdef __IMPROVE_MAP_SYSTEM
		, "ContinentDef.h"
		, "defineMapComboBoxData.h"
#endif // __IMPROVE_MAP_SYSTEM

#if __VER >= 19
		, "defineItemGrade.h"
		, "defineItemType.h"
#endif//__VER >= 19

	};
	int n, i;

	n = sizeof(lpszDefines) / sizeof(lpszDefines[0]);
	for( i=0; i<n; ++i )
	{
		CScript script;
		if( script.Load( lpszDefines[i] ) == TRUE )
			script.PreScan();
	}	
}

void CProject::LoadPreFiles()
{
#ifdef __SECURITY_0628
#ifdef __CLIENT
	CResFile::LoadAuthFile();
#endif	// __CLIENT
#endif	// __SECURITY_0628

	LoadDefines();
	LoadStrings();
	
#ifdef __LANG_1013
	CLangMan::GetInstance()->Load( "propLang.txt" );
#endif	// __LANG_1013
}


#ifdef __RULE_0615
BOOL CProject::LoadInvalidName( void )
{
	CScanner s;
	TCHAR szName[64];
	
	CString strFilter;
	strFilter	= GetLangFileName( ::GetLanguage(), FILE_INVALID );

	if( s.Load( strFilter ) ) 
	{
		s.GetToken();
		while( s.tok != FINISHED )
		{
			strcpy( szName, s.Token );
			strlwr( szName );
			
			m_sInvalidNames.insert( (LPCSTR)szName );
			s.GetToken();
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CProject::IsInvalidName( LPCSTR szName )
{
	TCHAR pszName[ 64 ];
	strcpy( pszName, szName );
	strlwr( pszName );
//	string str	= pszName;
	CString str = pszName;
	for( set<string>::iterator i = m_sInvalidNames.begin(); i != m_sInvalidNames.end(); ++i )
	{
		string strstr = *i;
		
//		if( str.find( *i, 0 ) != -1 )
		if( str.Find( (*i).c_str(), 0 ) != -1 )
			return TRUE;
	}
	return FALSE;
}

#ifdef __VENDOR_1106
BOOL	CProject::LoadAllowedLetter( BOOL bVendor )
#else	// __VENDOR_1106
BOOL	CProject::LoadAllowedLetter( void )
#endif	// __VENDOR_1106
{
	CScanner s;
	CString strFile;

#ifdef __VENDOR_1106
	set<char>*	ptr	= NULL;
	if( bVendor )
	{
		ptr		= &m_sAllowedLetter2;
		strFile		= GetLangFileName( ::GetLanguage(), FILE_ALLOWED_LETTER2 );
	}
	else
	{
		ptr		= &m_sAllowedLetter;
		strFile		= GetLangFileName( ::GetLanguage(), FILE_ALLOWED_LETTER );
	}
#else	// __VENDOR_1106
	strFile		= GetLangFileName( ::GetLanguage(), FILE_ALLOWED_LETTER );
	set<char>*	ptr		= &m_sAllowedLetter;
#endif	// __VENDOR_1106
	if( s.Load( strFile ) ) 
	{
		s.GetToken();
		while( s.tok != FINISHED )
		{
			if( s.Token.GetLength() )
				ptr->insert( s.Token.GetAt( 0 ) );
			s.GetToken();
		}
		return TRUE;
	}
	return FALSE;
}

BOOL	CProject::IsAllowedLetter( LPCSTR szName, BOOL bVendor )	
{
#ifdef __VENDOR_1106
	set<char>* ptr		= NULL;
	if( bVendor )
		ptr		= &m_sAllowedLetter2;
	else
		ptr		= &m_sAllowedLetter;
#else	// __VENDOR_1106
	set<char>* ptr		= &m_sAllowedLetter;
#endif	// __VENDOR_1106

	if( !ptr->size() )	// 규칙이 없으면 무시
		return TRUE;

	int nLen	= lstrlen( szName );
	for( int i = 0; i < nLen; i++ )
	{
		char chLetter	= szName[i];
		set<char>::iterator it	= ptr->find( chLetter );
		if( it == ptr->end() )
		{
#ifndef __VENDOR_1106
			if( bVendor && ( chLetter == ' ' || chLetter == '+' ) )
				continue;
#endif	// __VENDOR_1106
			return FALSE;
		}
	}
	return TRUE;
}

void	CProject::Formalize( LPSTR szName )
{
	int nLen	= lstrlen( szName );
	if( nLen == 0 )
		return;

	DWORD dwLanguage	= ::GetLanguage();
	switch( dwLanguage )
	{
//		case LANG_USA:
		case LANG_GER:
		case LANG_FRE:
			{
//				if( ::GetLanguage() == LANG_USA && ::GetSubLanguage() == LANG_SUB_PHP )
//					break;
				_strlwr( szName );	// 소문자로 변경
				char szBuffer[2];
				szBuffer[0]	= szName[0];
				szBuffer[1]	= '\0';
				_strupr( szBuffer );
				szName[0]	= szBuffer[0];
				break;
			}
		default:
			{
				break;
			}
	}
}
#endif	// __RULE_0615
