#include "stdafx.h"
#include "defineItem.h"

#if __VER >= 19
#include "defineItemGrade.h"
#include "defineItemType.h"
#endif//__VER >= 19

#include "lang.h"
#include "Ship.h"
#include "guild.h"
#include "spevent.h"

#if __VER >= 13 // __COUPLE_1117
#include "couplehelper.h"
#include "couple.h"
	#ifdef __WORLDSERVER
		#include "dpdatabaseclient.h"
		#include "dpsrvr.h"
		extern	CDPDatabaseClient	g_dpDBClient;
		extern	CDPSrvr	g_DPSrvr;
	#endif	// __WORLDSERVER
#endif	// __COUPLE_1117

#ifdef __WORLDSERVER
#if __VER >= 12 // __RANGDA_0521
#include "rangda.h"
#endif	// __RANGDA_0521
#endif // __WORLDSERVER

#include "definequest.h"

#ifdef __AZRIA_1023
#include "ticket.h"
#endif	// __AZRIA_1023

#ifdef __WORLDSERVER
	#include "worldmng.h"
	#include "guildquest.h"
	#include "MsgHdr.h"
	#include "partyquest.h"
#ifdef __SYS_TICKET
	#ifndef __AZRIA_1023
		#include "ticket.h"
	#endif	// __AZRIA_1023
#endif	// __SYS_TICKET
#endif	// __WORLDSERVER

#if __VER >= 9	// __PET_0410
#include "pet.h"
#endif	// __PET_0410
#if __VER >= 11 // __SYS_COLLECTING
#include "collecting.h"
#endif	// __SYS_COLLECTING
#if __VER >= 11 // __SYS_IDENTIFY
#include "randomoption.h"
#endif	// __SYS_IDENTIFY

#ifdef __EVENT_MONSTER
#include "EventMonster.h"
#endif // __EVENT_MONSTER

#if __VER >= 12 // __SECRET_ROOM
#include "SecretRoom.h"
#endif // __SECRET_ROOM

#if __VER >= 12 // __TAX
#include "Tax.h"
#endif // __TAX

#if __VER >= 12 // __NEW_ITEMCREATEMON_SERVER
#include "CreateMonster.h"
#endif // __NEW_ITEMCREATEMON_SERVER

/*
#ifdef __GIFTBOX0213
#include "dpaccountclient.h"
#include "msghdr.h"
#endif	// __GIFTBOX0213
*/

#ifdef __LANG_1013
#include "langman.h"
#endif	// __LANG_1013


#if __VER >= 12 // __LORD
#ifdef __WORLDSERVER
#include "slord.h"
#endif	// __WORLDSERVER
#ifdef __CLIENT
#include "clord.h"
#endif	// __CLIENT
#endif	// __LORD

#ifdef __WORLDSERVER
#if __VER >= 13 // __RAINBOW_RACE
#include "RainbowRace.h"
#endif // __RAINBOW_RACE
#endif // __WORLDSERVER

#if __VER >= 13 // __HONORABLE_TITLE
#include "honor.h"
#endif	// __HONORABLE_TITLE

#ifdef __WORLDSERVER
#ifdef __QUIZ
#include "Quiz.h"
#endif // __QUIZ

#if __VER >= 15 // __CAMPUS
#include "CampusHelper.h"
#endif // __CAMPUS

#if __VER >= 15 // __GUILD_HOUSE
#include "GuildHouse.h"
#endif // __GUILD_HOUSE

#ifdef __ENVIRONMENT_EFFECT
#include "Environment.h"
#endif // __ENVIRONMENT_EFFECT

#ifdef	__COLOSSEUM
#include "Colosseum.h"
#endif // __COLOSSEUM



#endif // __WORLDSERVER

extern	CWorldMng	g_WorldMng;

FLOAT	CProject::m_fItemDropRate = 1.0f;		// 몬스터 아이템 드롭률
FLOAT	CProject::m_fGoldDropRate = 1.0f;		// 몬스터 페냐 드롭률
FLOAT	CProject::m_fMonsterExpRate = 1.0f;		// 몬스터 경험치룰
FLOAT	CProject::m_fMonsterHitRate = 1.0f;		// 몬스터 공격률
FLOAT	CProject::m_fShopCost = 1.0f;			// 상점가격
FLOAT	CProject::m_fSkillExpRate = 1.0f;		
DWORD	CProject::m_dwVagSP = 1;				// 방랑자 스킬의 레벨업때 필요한 SP포인트
DWORD	CProject::m_dwExpertSP = 2;				// 1차직업 스킬의 레벨업때 필요한 SP포인트
DWORD	CProject::m_dwProSP = 3;				// 2차직업 스킬의 레벨업때 필요한 SP포인트

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
QuestGoalData::QuestGoalData( void ) : 
m_dwGoalIndex( 0 ), 
m_fGoalPositionX( 0.0F ), 
m_fGoalPositionZ( 0.0F ), 
m_dwGoalTextID( 0 )
{
}
#endif // __IMPROVE_QUEST_INTERFACE

////////////////////////////////////////////////////////////////////////////////////
// CHARACTER
////////////////////////////////////////////////////////////////////////////////////
void CHARACTER::Clear()
{
	m_nStructure        = -1;
	m_nEquipNum         = 0;
	m_dwHairMesh = 0;
	m_dwHairColor = 0;
	m_dwHeadMesh = 0;
	m_dwMoverIdx = 0;
	m_dwMusicId = 0;

	ZeroMemory( m_szKey, sizeof( m_szKey ) );
	ZeroMemory( m_szChar, sizeof( m_szChar ) );
	ZeroMemory( m_szDialog, sizeof( m_szDialog ) );
	ZeroMemory( m_szDlgQuest, sizeof( m_szDlgQuest ) );
	ZeroMemory( m_abMoverMenu, sizeof( m_abMoverMenu ) );
	ZeroMemory( m_adwEquip, sizeof( m_adwEquip ) );
#if __VER >= 15 // __TELEPORTER
	m_vecTeleportPos.clear();
#endif // __TELEPORTER
}

////////////////////////////////////////////////////////////////////////////////////
// CDropItemGenerator
////////////////////////////////////////////////////////////////////////////////////
DROPITEM* CDropItemGenerator::GetAt( int nIndex, BOOL bUniqueMode, float fProbability )
{
	//ASSERT( nIndex < m_dwSize );
	ASSERT( nIndex < (int)GetSize() );

	if( fProbability > 0.0f ) 
	{
		DROPITEM* lpDropItem	= &m_dropItems[ nIndex ];
		DWORD dwProbability		= lpDropItem->dwProbability;
		DWORD dwRand = xRandom( 3000000000 );
#if __VER >= 9 && defined(__WORLDSERVER) // __EVENTLUA, __WORLDSERVER
		dwRand = static_cast<DWORD>(dwRand / fProbability );
#endif // __EVENTLUA && __WORLDSERVER
		/*
		if( lpDropItem->dwLevel && bUniqueMode && lpDropItem->dwProbability <= 10000000 )
		{
			dwRand /= 2;
		}
		// 레벨(유니크)이 있나 검사
		if( lpDropItem->dwLevel != 0 )
		{
			ItemProp* pItemProp	= prj.GetItemProp( lpDropItem->dwIndex );
			if( pItemProp && (int)pItemProp->dwItemLV > 0 )
			{
				int nValue	= dwProbability / pItemProp->dwItemLV;
				if( nValue < 21 )	nValue	= 21;
				dwProbability	= nValue - 20;
			}
		}
		*/
		return( dwRand < dwProbability? lpDropItem: NULL );
	}
	else 
	{
		return &m_dropItems[ nIndex ];
	}
}

void CDropItemGenerator::AddTail( CONST DROPITEM & rDropItem, const char* s )
{
	m_dropItems.push_back( rDropItem );
}

// CQuestItemGenerator
QUESTITEM* CQuestItemGenerator::GetAt( int nIndex )
{
	ASSERT( nIndex < (int)( m_uSize ) );
	return &m_pQuestItem[nIndex];
}

void CQuestItemGenerator::AddTail( const QUESTITEM & rQuestItem )
{
	ASSERT( m_uSize < MAX_QUESTITEM );
	memcpy( &m_pQuestItem[m_uSize++], &rQuestItem, sizeof(QUESTITEM) );
}

void CDropKindGenerator::AddTail( const DROPKIND & rDropKind )
{
	ASSERT( m_nSize < MAX_DROPKIND );
	memcpy( &m_aDropKind[m_nSize++], &rDropKind, sizeof(DROPKIND) );
}

LPDROPKIND CDropKindGenerator::GetAt( int nIndex )
{
	ASSERT( nIndex >= 0 );
	ASSERT( nIndex < m_nSize );
	if( nIndex < 0 || nIndex >= m_nSize )
		return NULL;
	return &m_aDropKind[nIndex];
}


CProject::CProject()
#ifdef __WORLDSERVER
#if __VER >= 12 // __MOD_TUTORIAL
:
m_nMaxSequence( 0 )
#endif	// __MOD_TUTORIAL
#endif	// __WORLDSERVER
{
	memset( m_minMaxIdxAry, 0xff, sizeof(SIZE) * MAX_ITEM_KIND3 * MAX_UNIQUE_SIZE );
	memset( m_aExpUpItem, 0, sizeof(m_aExpUpItem) );
	memset( m_aGuildAppell, 0, sizeof(m_aGuildAppell) );


#ifdef __S1108_BACK_END_SYSTEM
	m_fMonsterRebirthRate = 1.0f;	
	m_fMonsterHitpointRate = 1.0f;	
	m_fMonsterAggressiveRate = 0.2f;
	m_fMonsterRespawnRate = 1.0f;
	m_nAddMonsterPropSize = 0;
	m_nRemoveMonsterPropSize = 0;
	m_nMonsterPropSize = 0;
#endif // __S1108_BACK_END_SYSTEM

	memset( m_aSetItemAvail, 0, sizeof(m_aSetItemAvail) );

	m_nMoverPropSize = 0;
	m_pPropMover = new MoverProp[MAX_PROPMOVER];
//	m_pPropMover = (MoverProp*)::VirtualAlloc( NULL, sizeof(MoverProp) * MAX_PROPMOVER, MEM_COMMIT, PAGE_READWRITE );
#ifdef __CLIENT
#ifdef __SHOP_COST_RATE
	m_fShopBuyRate = 1.0f;
	m_fShopSellRate = 1.0f;
#endif // __SHOP_COST_RATE
#endif // __CLIENT

#ifdef __IMPROVE_MAP_SYSTEM
#ifdef __CLIENT
	m_bMapTransparent = FALSE;
#endif // __CLIENT
#endif // __IMPROVE_MAP_SYSTEM
}

CProject::~CProject()
{
#ifdef __CLIENT
	m_mapHelp.RemoveAll();
	m_mapWordToolTip.RemoveAll();
#ifndef __RULE_0615
	m_sInvalidNames.clear();
#endif	// __RULE_0615
#endif	// __CLIENT

#ifdef __RULE_0615
	m_sInvalidNames.clear();
	m_sAllowedLetter.clear();
#ifdef __VENDOR_1106
	m_sAllowedLetter2.clear();
#endif	// __VENDOR_1106
#endif	// __RULE_0615

	int i, j;
	POSITION pos = m_mapCharacter.GetStartPosition();
	LPCHARACTER lpCharacter;
	CString string;

	while(pos)
	{
		m_mapCharacter.GetNextAssoc(pos,string,(void*&)lpCharacter);
		for( j = 0; j < MAX_VENDOR_INVENTORY_TAB; j++ )
		{
			for( i = 0; i < lpCharacter->m_venderItemAry[ j ].GetSize(); i++)
				safe_delete( (LPVENDOR_ITEM)lpCharacter->m_venderItemAry[ j ].GetAt(i) );
		}

#if __VER >= 11 // __CSC_VER11_3
		for( j = 0; j < MAX_VENDOR_INVENTORY_TAB; j++ )
		{
			for( i = 0; i < lpCharacter->m_venderItemAry2[ j ].GetSize(); i++)
				safe_delete( (LPVENDOR_ITEM)lpCharacter->m_venderItemAry2[ j ].GetAt(i) );
		}
#endif //__CSC_VER11_3
		safe_delete( lpCharacter );
	}

	m_mapCharacter.RemoveAll();

	for( i = 0; i < m_colorText.GetSize(); i++ )
	{
		tagColorText* lpText = m_colorText.GetAt(i);
		if( lpText && lpText->lpszData )
			free( lpText->lpszData ); 
	}
	for( i = 0; i < m_aPropQuest.GetSize(); i++ )
	{
		QuestProp* pQuestProp = (QuestProp*)m_aPropQuest.GetAt( i );
		if( pQuestProp )
		{
		#if defined( __WORLDSERVER ) 
			for( j = 0; j < 32; j++ )
//				SAFE_DELETE( pQuestProp->m_apQuestDialog[ j ] );
				SAFE_DELETE_ARRAY( pQuestProp->m_apQuestDialog[ j ] );
		#endif
			for( j = 0; j < 16; j++ )
				SAFE_DELETE( pQuestProp->m_questState[ j ] );
			SAFE_DELETE_ARRAY( pQuestProp->m_paBeginCondItem );
			SAFE_DELETE_ARRAY( pQuestProp->m_paEndCondItem );
			SAFE_DELETE_ARRAY( pQuestProp->m_paEndRewardItem );
			SAFE_DELETE_ARRAY( pQuestProp->m_lpszEndCondMultiCharacter );
#if __VER >= 8 // __S8_PK
			SAFE_DELETE_ARRAY( pQuestProp->m_paBeginCondNotItem );
			SAFE_DELETE_ARRAY( pQuestProp->m_paEndCondOneItem );
#endif // __VER >= 8 // __S8_PK
		}
	}

	SAFE_DELETE_ARRAY( m_pPropMover );
//	::VirtualFree( m_pPropMover, sizeof(MoverProp) * MAX_PROPMOVER, MEM_DECOMMIT );

#ifdef __YS_CHATTING_BLOCKING_SYSTEM
#ifdef __CLIENT
	m_setBlockedUserID.clear();
#endif // __CLIENT
#endif // __YS_CHATTING_BLOCKING_SYSTEM

#ifdef __TREASUREBOX
	m_vTreasureBox.clear();
#endif
#ifdef __WORLDSERVER

#ifdef __TREASUREBOX
	m_vTreasureItem.clear();
#endif//__TREASUREBOX

#endif//__WORLDSERVER

}
int SortJobSkill( const void *arg1, const void *arg2 )
{
	ItemProp* pItemProp1 = *(ItemProp**) arg1;
	ItemProp* pItemProp2 = *(ItemProp**) arg2;
	if( pItemProp1->dwReqDisLV < pItemProp2->dwReqDisLV )
		return -1;
	if( pItemProp1->dwReqDisLV > pItemProp2->dwReqDisLV )
		return 1;
	return 0;
}

void CProject::LoadSkill()
{
#ifdef __NEW_PROP_PARAMETER
	LoadPropItemEx( "propSkill.txt", &m_aPropSkill );
#else // __NEW_PROP_PARAMETER
	LoadPropItem( "propSkill.txt", &m_aPropSkill );
#endif // __NEW_PROP_PARAMETER
	ZeroMemory( m_aJobSkillNum, sizeof( m_aJobSkillNum ) );
	// 각각의 스킬을 정렬해서 배열에 담자 
	for( int i = 1; i < m_aPropSkill.GetSize(); i++ )
	{
		ItemProp* pItemProp = (ItemProp*)m_aPropSkill.GetAt( i );
		if( pItemProp )
		{
			if( pItemProp->dwItemKind1 != JTYPE_COMMON )
			{
				ItemProp** apJobSkill = m_aJobSkill[ pItemProp->dwItemKind2 ];
				apJobSkill[ m_aJobSkillNum[ pItemProp->dwItemKind2 ] ] = pItemProp;
				m_aJobSkillNum[ pItemProp->dwItemKind2 ]++;
			}
		}
	}

	// 소트하기 
	for( i = 0; i < MAX_JOB; i++ )
	{
		ItemProp** apJobSkill = m_aJobSkill[ i ];
		qsort( (void *)apJobSkill, (size_t) m_aJobSkillNum[ i ], sizeof( ItemProp* ), SortJobSkill );
	}
}

#ifdef __EVE_MINIGAME
BOOL CProject::LoadMiniGame()
{
	if( m_MiniGame.Load_Alphabet() == FALSE )
	{
		// 메세지 처리
	}

	if( m_MiniGame.Load_KawiBawiBo() == FALSE )
	{
		// ...
	}

	if( m_MiniGame.Load_FiveSystem() == FALSE )
	{
		// ...
	}

	if( m_MiniGame.Load_Reassemble() == FALSE )
	{
		// ...
	}
	
	return TRUE;
	
}
#endif // __EVE_MINIGAME

#if __VER >= 9 // __ULTIMATE
BOOL CProject::LoadUltimateWeapon()
{
#ifdef __WORLDSERVER
	if( m_UltimateWeapon.Load_UltimateWeapon() == FALSE )
	{
		return FALSE;
	}
#endif // __WORLDSERVER
	
	if( m_UltimateWeapon.Load_GemAbility() == FALSE )
	{
		return FALSE;
	}

	return TRUE;
}
#endif // __ULTIMATE

BOOL CProject::OpenProject( LPCTSTR lpszFileName )
{
	CScanner scanner;
	if(scanner.Load( lpszFileName )==FALSE)
		return FALSE;

#if !defined(__CLIENT)
	LoadPreFiles();
#endif

	do 
	{
		scanner.GetToken();
		if( scanner.Token == "propTroupeSkill" )
		{	
			scanner.GetToken();	
#ifdef __NEW_PROP_PARAMETER
			LoadPropItemEx( "propTroupeSkill.txt", &m_aPartySkill );
#else  // __NEW_PROP_PARAMETER
			LoadPropItem( "propTroupeSkill.txt", &m_aPartySkill );
#endif // __NEW_PROP_PARAMETER
		}
		else if( scanner.Token == "propSkillAdd" )
		{
			scanner.GetToken();
			LoadPropAddSkill( scanner.token );
		}
		else if( scanner.Token == "propQuest" )
		{
			scanner.GetToken();
			LoadPropQuest( scanner.token, FALSE );
		}
		else if( scanner.Token == "expTable" )
		{	
			scanner.GetToken();	
			LoadExpTable( scanner.token );
		}
		else if( scanner.Token == "MotionProp" )
		{	
			scanner.GetToken();	
			LoadMotionProp( scanner.token );
		}
#if __VER < 8 // __S8_PK
		else if( scanner.Token == "propKarma" )
		{
			scanner.GetToken();
			LoadPropKarma( "propKarma.txt" );
		}
#endif // __VER < 8 // __S8_PK
		else if( scanner.Token == "propJob" )
		{
			scanner.GetToken();
			LoadPropJob( "propJob.inc" );
		}
		else if( scanner.Token == "propSkill" )
		{	
			scanner.GetToken();	
			LoadSkill();
		}
		else if( scanner.Token == "model" )
		{	
			scanner.GetToken();	
			m_modelMng.LoadScript( scanner.token );
		}
		else if( scanner.Token == "billboard" )
		{	
			scanner.GetToken();	
		}
		else if( scanner.Token == "propMover" )
		{	
			scanner.GetToken();	
			if( !LoadPropMover( "propMover.txt" ) )
				return FALSE;
		}
		else if( scanner.Token == "propCtrl" )
		{
			scanner.GetToken();
			LoadPropCtrl( "propCtrl.txt", &m_aPropCtrl );
		}
#ifdef __NEW_PROP_PARAMETER
		else if( scanner.Token == "propItem" )
		{	
			scanner.GetToken();	
		
			LoadPropItem( "Spec_Item.txt", &m_aPropItem );
			OnAfterLoadPropItem();
		}
#else // __NEW_PROP_PARAMETER
		else if( scanner.Token == "propItem" )
		{	
			scanner.GetToken();	
		
			LoadPropItem( "propItem.txt", &m_aPropItem );
			OnAfterLoadPropItem();
		}
#endif // __NEW_PROP_PARAMETER
		else if( scanner.Token == "character" )
		{
			scanner.GetToken();
			LoadCharacter( scanner.token );
		}
		else if( scanner.Token == "world" )
		{	
			scanner.GetToken();	
			g_WorldMng.LoadScript( scanner.token );
		}
		else if( scanner.Token == "terrain" )
		{	
			scanner.GetToken();	
		#ifdef __CLIENT
#ifdef __FLYFF_INITPAGE_EXT
			if(!CWndBase::m_Theme.m_bLoadTerrainScript)
				m_terrainMng.LoadScript( scanner.token );
#else //__FLYFF_INITPAGE_EXT
			m_terrainMng.LoadScript( scanner.token );
#endif //__FLYFF_INITPAGE_EXT
		#endif
		}
		else if( scanner.Token == "help" )
		{	
			scanner.GetToken();	
		#ifdef __CLIENT
			LoadHelp( scanner.token );
		#endif
		}
	} while(scanner.tok!=FINISHED);

	LoadMotionProp( "propMotion.txt" );
	LoadPropMoverEx( "PropMoverEx.inc" );
	LoadJobItem( "jobItem.inc" );
	LoadEtc( "etc.inc" );

#if __VER >= 9 // __Y_ADV_ENCHANT_EFFECT
	LoadPropEnchant( "propEnchant.inc" );
#endif //__Y_ADV_ENCHANT_EFFECT

#ifdef __TREASUREBOX
	LoadTreasureCombine( &m_vTreasureBox );
#endif//__TREASUREBOX

#ifdef __WORLDSERVER
	LoadConstant( "Constant.inc" );
	LoadPropGuildQuest( "propGuildQuest.inc" );
	LoadPropPartyQuest( "propPartyQuest.inc" );

	LoadDropEvent( "propDropEvent.inc" );
	LoadGiftbox( "propGiftbox.inc" );
	LoadPackItem( "propPackItem.inc" );

	#if __VER >= 8  
		LoadScriptDiePenalty( "DiePenalty.inc" );
	#endif //  __VER >= 8  

#ifdef __TREASUREBOX
	LoadTreasureItem( &m_vTreasureItem );
#endif//__TREASUREBOX

#endif // __WORLDSERVER
#if __VER >= 8 // __S8_PK
	LoadScriptPK( "PKSetting.inc" );
#endif // __VER >= 8 // __S8_PK

	LoadPiercingAvail( "propItemEtc.inc" );

#ifdef __EVENT_0117
#ifdef __WORLDSERVER
	CEventGeneric::GetInstance()->LoadScript( "propEvent.inc" );
#endif	// __WORLDSERVER
#endif	// __EVENT_0117

#ifdef __CLIENT	// 클라이언트만 로드
	CEventGeneric::GetInstance()->LoadScript( "propEvent.inc" );
	LoadFilter( GetLangFileName( ::GetLanguage(), FILE_FILTER ) );
#ifndef __RULE_0615
	LoadInvalidName();
#endif	// __RULE_0615
#endif	// __CLIENT

#ifdef __RULE_0615
	LoadInvalidName();
	LoadAllowedLetter();
#ifdef __VENDOR_1106
	LoadAllowedLetter( TRUE );
#endif	// __VENDOR_1106
#endif	// __RULE_0615

#if __VER >= 9	// __PET_0410
	CPetProperty::GetInstance()->LoadScript( "pet.inc" );
#endif	// __PET_0410
#if __VER >= 11 // __SYS_COLLECTING
	CCollectingProperty::GetInstance()->LoadScript( "collecting.inc" );
	CAccessoryProperty::GetInstance()->LoadScript( "accessory.inc" );
#endif	// __SYS_COLLECTING
#if __VER >= 11 // __SYS_IDENTIFY
	g_xRandomOptionProperty->LoadScript( "randomoption.inc" );
#endif	// __SYS_IDENTIFY
#ifdef __SYS_TICKET
#ifdef __AZRIA_1023
	CTicketProperty::GetInstance()->LoadScript();
#else	// __AZRIA_1023
	#ifdef __WORLDSERVER
	CTicketProperty::GetInstance()->LoadScript();
	#endif	// __WORLDSERVER
#endif	// __AZRIA_1023
#endif	// __SYS_TICKET

	m_aPropQuest.Optimize();

#ifdef __EVE_MINIGAME
	#ifdef __WORLDSERVER
		LoadMiniGame();
	#endif // __WORLDSERVER
#endif // __EVE_MINIGAME

#if __VER >= 9 // __ULTIMATE
	LoadUltimateWeapon();
#endif // __ULTIMATE
		
#ifdef __TRADESYS
	m_Exchange.Load_Script();
#endif // __TRADESYS

#if __VER >= 9 && defined(__WORLDSERVER) // __EVENTLUA && __WORLDSERVER
	prj.m_EventLua.LoadScript();
#endif // __EVENTLUA && __WORLDSERVER

#if __VER >= 11 // __GUILD_COMBAT_1TO1
	#ifdef __WORLDSERVER
	g_GuildCombat1to1Mng.LoadScript();
	#endif // __WORLDSERVER
#endif // __GUILD_COMBAT_1TO1

#ifdef __WORLDSERVER
#ifdef __JEFF_11_3
	LoadServerScript( "s.txt" );
#endif	// __JEFF_11_3
#endif	// __WORLDSERVER

#ifdef __EVENT_MONSTER
	CEventMonster::GetInstance()->LoadScript();
#endif // __EVENT_MONSTER

#ifdef __WORLDSERVER
#if __VER >= 12 // __SECRET_ROOM
	CSecretRoomMng::GetInstance()->LoadScript();
#endif // __SECRET_ROOM
#if __VER >= 12 // __MONSTER_SKILL
	CMonsterSkill::GetInstance()->LoadScript();
#endif // __MONSTER_SKILL

#if __VER >= 12 // __NEW_ITEMCREATEMON_SERVER
	CCreateMonster::GetInstance()->LoadScript();
#endif // __NEW_ITEMCREATEMON_SERVER

#ifdef __QUIZ
	CQuiz::GetInstance()->LoadScript();
#endif // __QUIZ
#if __VER >= 15 // __CAMPUS
	CCampusHelper::GetInstance()->LoadScript();
#endif // __CAMPUS

#ifdef __ENVIRONMENT_EFFECT
	CEnvironment::GetInstance()->LoadScript();
#endif // __ENVIRONMENT_EFFECT

#ifdef __GUILD_HOUSE_MIDDLE
	GuildHouseMng->LoadScript();
#endif // __GUILD_HOUSE_MIDDLE
#endif // __WORLDSERVER

	ProtectPropMover();

#ifdef __PERF_0226
	CPartsItem::GetInstance()->Init( this );
#endif	// __PERF_0226

#if __VER >= 12 // __LORD
#ifdef __WORLDSERVER
	CSLord::Instance()->CreateColleagues();
#else	// __WORLDSERVER
	CCLord::Instance()->CreateColleagues();
#endif	// __WORLDSERVER
#endif	// __LORD
#ifdef __WORLDSERVER
#if __VER >= 12 // __RANGDA_0521
	CRangdaController::Instance()->LoadScript( "randomeventmonster.inc" );
	CTransformItemProperty::Instance()->LoadScript( "transformitem.inc" );
#endif	// __RANGDA_0521
#endif // __WORLDSERVER

#ifdef __WORLDSERVER
#if __VER >= 12 // __MOD_TUTORIAL
	LoadGuideMaxSeq();
#endif	// __MOD_TUTORIAL
#if __VER >= 13 // __RAINBOW_RACE
	CRainbowRaceMng::GetInstance()->LoadScript();
#endif // __RAINBOW_RACE
#endif	// __WORLDSERVER
//	OutputDropItem();
//	return FALSE;

//	CCoupleTestCase ctc;
//	CTestCaseMgr::Instance()->Test();

#if __VER >= 13 // __COUPLE_1117
#ifdef __WORLDSERVER
	CCoupleHelper::Instance()->Initialize( &g_dpDBClient, &g_DPSrvr );
#else	// __WORLDSERVER
	CCoupleHelper::Instance()->Initialize();
#endif	// __WORLDSERVER
#endif	// __COUPLE_1117

#if __VER >= 13 // __HONORABLE_TITLE
	CTitleManager::Instance()->LoadTitle("honorList.txt");
#endif	// __HONORABLE_TITLE
#if __VER >= 15 /* __IMPROVE_QUEST_INTERFACE */ && defined( __CLIENT )
	LoadQuestDestination();
	LoadPatrolDestination();
#endif // defined( __IMPROVE_QUEST_INTERFACE ) && defined( __CLIENT )

#ifdef __IMPROVE_MAP_SYSTEM
#ifdef __CLIENT
	m_MapInformationManager.LoadMapInformationData();
#endif // __CLIENT
#endif // __IMPROVE_MAP_SYSTEM

#ifdef __WORLDSERVER
#ifdef	__COLOSSEUM
	CColosseumMng::GetInstance()->LoadScript();
#endif // __COLOSSEUM
#endif



	return TRUE;
}

#ifdef __WORLDSERVER
void CProject::SetGlobal( UINT type, float fValue )
{
	switch( type )
	{
	case GAME_RATE_ITEMDROP:
		if( fValue > 20.0f )
			fValue = 20.0f;
		else if( fValue < 0.1f )
			fValue = 0.1f;
		m_fItemDropRate = fValue;
		break;
	case GAME_RATE_MONSTEREXP:
		if( fValue > 5.0f )
			fValue = 5.0f;
		else if( fValue < 0.1f )
			fValue = 0.1f;
		m_fMonsterExpRate = fValue;
		break;
	case GAME_RATE_MONSTERHIT:
		if( fValue > 5.0f )
			fValue = 5.0f;
		else if( fValue < 0.1f )
			fValue = 0.1f;
		m_fMonsterHitRate = fValue;
		break;
	case GAME_RATE_GOLDDROP:
		if( fValue > 5.0f )
			fValue = 5.0f;
		else if( fValue < 0.1f )
			fValue = 0.1f;
		m_fGoldDropRate = fValue;
		break;
	default:
		ASSERT( 0 );
	}

#ifdef __THA_0808
	if( ::GetLanguage() == LANG_THA )
	{
		m_fItemDropRate	= 1.0F;
		m_fMonsterExpRate	= 1.0F;
		m_fMonsterHitRate	= 1.0F;
		m_fGoldDropRate	= 1.0F;
	}
#endif	// __THA_0808
}

void CProject::ReadConstant( CScript& script )
{
	do 
	{
		script.GetToken(); 
		if( script.Token == "itemDropRate" )
		{	
			script.GetToken();// bypass '='
			SetGlobal( GAME_RATE_ITEMDROP, script.GetFloat() );
		}
		else if( script.Token == "monsterExpRate" )
		{	
			script.GetToken();// bypass '='
			SetGlobal( GAME_RATE_MONSTEREXP, script.GetFloat() );
		}
		else if( script.Token == "monsterHitRate" )
		{	
			script.GetToken();// bypass '='
			SetGlobal( GAME_RATE_MONSTERHIT, script.GetFloat() );
		}
		else if( script.Token == "goldDropRate" )
		{	
			script.GetToken();// bypass '='
			SetGlobal( GAME_RATE_GOLDDROP, script.GetFloat() );
		}
		else if( script.Token == "dwVagSP" )
		{
			script.GetToken();// bypass '='
			m_dwVagSP = script.GetNumber();
		}
		else if( script.Token == "dwExpertSP" )
		{
			script.GetToken();// bypass '='
			m_dwExpertSP = script.GetNumber();
		}
		else if( script.Token == "dwProSP" )
		{
			script.GetToken();// bypass '='
			m_dwProSP = script.GetNumber();
		}
	} 
	while( script.tok != FINISHED && *script.token != '}' );
}

BOOL CProject::LoadConstant( LPCTSTR lpszFileName )
{
	CScript script;
	if( script.Load( lpszFileName ) == FALSE )
		return FALSE;

	do 
	{
		if( script.Token == "lang" )
		{
			int nLang = script.GetNumber();
			if( ::GetLanguage() == nLang ) 
			{
				script.GetToken(); // {
				do 
				{
					if( script.Token == "formula" )
					{
						ReadConstant( script );
					}

					script.GetToken(); // server type
				} while( script.tok != FINISHED && *script.token != '}' );
				script.GetToken(); // }
			}
		}
		script.GetToken();
	} while( script.tok != FINISHED );

	return TRUE;
}
#endif //__WORLDSERVER

AddSkillProp* CProject::GetAddSkillProp( DWORD dwSubDefine, DWORD dwLevel )
{
	if( dwLevel < 1 )
		dwLevel = 1;
	
	int nskillIndex = dwSubDefine + dwLevel - 1;

	if( nskillIndex < 5)
		nskillIndex += 5;

	if( nskillIndex < 0 || nskillIndex >= m_aPropAddSkill.GetSize() )	
	{
		Error( "CProject::GetAddSkillProp 범위침범. %d", nskillIndex );
		return 0;
	}
	
	return m_aPropAddSkill.GetAt( nskillIndex );
}

AddSkillProp* CProject::GetAddSkillProp( DWORD dwSubDefine )
{
	return GetAddSkillProp( dwSubDefine, 1 );
}

BOOL CProject::LoadEtc( LPCTSTR lpszFileName )
{
	CScript script;
	if( script.Load( lpszFileName ) == FALSE )
		return FALSE;
	int id;
	do {
		script.GetToken();
		if( script.Token == _T( "job" ) )
		{	
			script.GetToken(); // skip {
			id = script.GetNumber();
			while( *script.token != '}' )
			{
				script.GetToken();
				_tcscpy( m_aJob[ id ].szName, script.token );
				m_aJob[ id ].dwJobBase = script.GetNumber();
				m_aJob[ id ].dwJobType = script.GetNumber();
				id = script.GetNumber();
			}
		}
		else
		if( script.Token == _T( "structure" ) )
		{	
			script.GetToken(); // skip {
			id = script.GetNumber();
			while( *script.token != '}' )
			{
				script.GetToken();
				_tcscpy( m_aStructure[ id ].szName, script.token );
				id = script.GetNumber();
			}
		}
		else
		if( script.Token == _T( "Guild" ) )
		{
			script.GetToken(); // skip {
			id = script.GetNumber();
			while( *script.token != '}' )
			{
				script.GetToken();
				_tcscpy( m_aGuildAppell[ id ].szName, script.token );
				id = script.GetNumber();
			}
		}
	} while( script.tok != FINISHED );
	return TRUE;
}

BOOL CProject::LoadMotionProp( LPCTSTR lpszFileName )
{
	// 버전  모션 인덱스  요구Lv  요구경험치  표시단어 루트명 연결단어 
	CScript script;
	if( script.Load( lpszFileName, FALSE ) == FALSE )
		return FALSE;
	MotionProp MotionProp;
	ZeroMemory( &MotionProp, sizeof( MotionProp ) );


	int nVer = script.GetNumber();  // version		061206 ma	8차에 들어갈 모션관리를 위해 버전 추가	propMotion.txt

	while( script.tok != FINISHED )
	{
		DWORD dwID = script.GetNumber();	// IID_
		if( dwID <= 0 )
		{
			Error( "%s ID:%d last_read(ID:%d name:%s)", lpszFileName, dwID, MotionProp.dwID, MotionProp.szName);
			return FALSE;
		}

		MotionProp.dwID = dwID;
		MotionProp.dwMotion = script.GetNumber();
		script.GetToken();  
		script.GetToken();  
		if( strlen(script.m_mszToken) >= sizeof(MotionProp.szIconName) )
			Error( "LoadMotionProp : 메모리침범. %s 버퍼크기(%d), 스트링길이(%d)", script.m_mszToken, sizeof(MotionProp.szIconName), strlen(script.m_mszToken) );
		strcpy( MotionProp.szIconName, script.m_mszToken );
		script.GetToken();  
		MotionProp.dwPlay = (DWORD)script.GetNumber();

		script.GetToken();
		_tcscpy( MotionProp.szName, script.m_mszToken );
		script.GetToken();
		_tcscpy( MotionProp.szDesc, script.m_mszToken );
		if( nVer <= __VER )
		{
			m_aPropMotion.SetAtGrow( MotionProp.dwID, &MotionProp);
		}
		
		nVer = script.GetNumber(); 
	}
	m_aPropMotion.Optimize();
	return TRUE;

}

#ifdef __WORLDSERVER
#if __VER >= 12 // __MOD_TUTORIAL
BOOL CProject::LoadGuideMaxSeq()
{

	CScript script;
	if( script.Load(MakePath(DIR_CLIENT, "Guide.inc")) == FALSE )
		return FALSE;

	script.tok = 0;
	m_nMaxSequence = 0;

	while( script.tok != FINISHED )
	{
		script.GetNumber();

		script.GetToken(); // {
		if( *script.token  == '{' )
		{
			script.GetToken(); // BEGINNER
			script.GetNumber();
			script.GetToken(); // SHOWLEVEL
			script.GetNumber();
			script.GetToken(); // FLAG
			script.GetNumber();
			script.GetToken(); // KEY
			script.GetNumber();
			script.GetToken();
			script.Token;
			script.GetToken();
			if( strcmpi( script.token, "SEQUENCE" ) == 0 )
			{
				script.GetNumber();
				script.GetToken();
			}
			if( strcmpi( script.token, "LEVEL" ) == 0 )
			{
				int	nCurSeq = script.GetNumber();
				if(m_nMaxSequence < nCurSeq) m_nMaxSequence = nCurSeq;
				script.GetToken();
			}
			if( strcmpi( script.token, "CONDITION" ) == 0 )
			{
				script.GetNumber();
				script.GetToken();
			}
			if( strcmpi( script.token, "INPUTCHAR" ) == 0 )
			{
				script.GetNumber();
				script.GetToken();
			}
			if( strcmpi( script.token, "INPUTSTR" ) == 0 )
			{
				script.GetToken();
				script.GetToken();	// }
				while(*script.token  != '}')
				{
					script.GetToken();	// }
				}
			}
			
		}
		else
			script.GetToken();
	}
	
	return TRUE;
}
#endif	// __MOD_TUTORIAL
#endif	// __WORLDSERVER

#ifdef __CLIENT
BOOL CProject::LoadHelp( LPCTSTR lpszFileName )
{
	CScript s;
	if( s.Load( lpszFileName ) == FALSE )
		return FALSE;
	CString strKeyword;
	s.GetToken();
	while( s.tok != FINISHED )
	{
		strKeyword	= s.token;
		s.GetToken();	// {
		s.GetToken();
		CString string = "   ";
		while( *s.token != '}' )
		{
			 string		+= s.Token;
			 string		+= "\n\n   ";
			s.GetToken();
		}
		m_mapHelp.SetAt( strKeyword, string );
		s.GetToken();
	}
	return TRUE;
}
CString CProject::GetHelp( LPCTSTR lpStr )
{
	CString string;
	m_mapHelp.Lookup( lpStr, string );
	return string;
}
CString CProject::GetWordToolTip( LPCTSTR lpStr )
{
	CString string;
	m_mapWordToolTip.Lookup( lpStr, string );
	return string;
}
BOOL CProject::LoadFilter( LPCTSTR lpszFileName )
{	
	CScanner scanner;
	if( scanner.Load( lpszFileName, FALSE ) == FALSE )
		return FALSE;
	FILTER filter;
	scanner.GetToken();

	while( scanner.tok != FINISHED )
	{
#ifdef __FILTER_0705
		BOOL bAlpha		= TRUE;
		int nLen	= lstrlen( scanner.token );
		for( int i = 0; i < nLen; i++ )
		{
#ifdef __VS2003
			if( !iswalpha( scanner.token[i] ) )
#else //__VS2003
			if( !isalpha( scanner.token[i] ) )
#endif //__VS2003
			{
				bAlpha	= FALSE;
				break;
			}
		}
		if( bAlpha )
			scanner.Token.MakeLower();
#endif	// __FILTER_0705
		strcpy( filter.m_szSrc, scanner.Token );
		scanner.GetToken();
		strcpy( filter.m_szDst, scanner.Token );
#ifdef __FILTER_0705
		if( bAlpha )
			m_mapAlphaFilter.insert( map<string, string>::value_type( filter.m_szSrc, filter.m_szDst ) );
		else
			m_mapNonalphaFilter.insert( map<string, string>::value_type( filter.m_szSrc, filter.m_szDst ) );
#else	// __FILTER_0705
		m_aWordFilter.Add( &filter );
#endif	// __FILTER_0705
		scanner.GetToken(); 
	}
#ifndef __FILTER_0705
	m_aWordFilter.Optimize();
#endif	// __FILTER_0705
	return TRUE;
}
BOOL CProject::LoadWordToolTip( LPCTSTR lpszFileName )
{
	CScanner scanner;
	if(scanner.Load(lpszFileName)==FALSE)
		return FALSE;
	CString strWord;
	CString strToolTip;
	scanner.GetToken();
	while( scanner.tok != FINISHED )
	{
		strWord = scanner.Token;
		scanner.GetToken();
		m_mapWordToolTip.SetAt( strWord, scanner.token );
		scanner.GetToken(); 
	}
	return TRUE;
}
#endif

BOOL CProject::LoadPropGuildQuest( LPCTSTR lpszFilename )
{
#ifdef __WORLDSERVER
	CGuildQuestProcessor* pProcessor	= CGuildQuestProcessor::GetInstance();
#endif	// __WORLDSERVER
	CScript s;
	if( s.Load( lpszFilename ) == FALSE )
		return FALSE;
	GUILDQUESTPROP prop;

	int nQuestId	= s.GetNumber();	// id

	while( s.tok != FINISHED )
	{
		ZeroMemory( &prop, sizeof(prop) );

		s.GetToken();
		while( s.Token[0] != '}' )
		{
			if( s.Token == "Title" )
			{
				s.GetToken();	// =
				s.GetToken();
				lstrcpy( prop.szTitle, s.Token );
			}
			else if( s.Token == "Level" )
			{
				s.GetToken();	// =
				prop.nLevel		= s.GetNumber();
			}
			else if( s.Token == "Wormon" )
			{
				s.GetToken();	// =
				prop.dwWormon	= s.GetNumber();
			}
			else if( s.Token == "World" )
			{
				s.GetToken();	// =
				prop.dwWorldId	= s.GetNumber();
			}
			else if( s.Token == "Position" )
			{
				s.GetToken();	// =
				prop.vPos.x		= s.GetFloat();
				s.GetToken();	// ,
				prop.vPos.y		= s.GetFloat();
				s.GetToken();	// ,
				prop.vPos.z		= s.GetFloat();
			}
			else if( s.Token == "Region" )
			{
				s.GetToken();	// =
				prop.x1	= s.GetNumber();
				s.GetToken();	// ,
				prop.y1	= s.GetNumber();
				s.GetToken();	// ,
				prop.x2	= s.GetNumber();
				s.GetToken();	// ,
				prop.y2	= s.GetNumber();
			#ifdef __WORLDSERVER
				pProcessor->AddQuestRect( nQuestId, prop.x1, prop.y1, prop.x2, prop.y2 );
			#endif	// __WORLDSERVER
			}
			else if( s.Token == "State" )
			{
				int nState	= s.GetNumber();
				s.GetToken();	// key or anything
				while( s.Token[0] != '}' )
				{
					if( s.Token == "desc" )
					{
						s.GetToken();	// =
						s.GetToken();
						lstrcpy( prop.szDesc[nState], s.Token );
					}
					s.GetToken();	// key or anything
				}
			}
			s.GetToken();
		}
		m_aPropGuildQuest.SetAtGrow( nQuestId, &prop );
		nQuestId	= s.GetNumber();	// id
	}
	m_aPropGuildQuest.Optimize();
	return TRUE;
}

BOOL CProject::LoadPropPartyQuest( LPCTSTR lpszFilename )
{
#ifdef __WORLDSERVER
	CPartyQuestProcessor* pProcessor	= CPartyQuestProcessor::GetInstance();
#endif	// __WORLDSERVER
	CScript s;
	if( s.Load( lpszFilename ) == FALSE )
		return FALSE;
//	PARTYQUESTPROP prop;
	
	int nQuestId	= s.GetNumber();	// id
	
	while( s.tok != FINISHED )
	{
		PARTYQUESTPROP prop;
//		ZeroMemory( &prop, sizeof(prop) );
		
		s.GetToken();
		while( s.Token[0] != '}' )
		{
			if( s.Token == "Title" )
			{
				s.GetToken();	// =
				s.GetToken();
				lstrcpy( prop.szTitle, s.Token );
			}
			else if( s.Token == "Level" )
			{
				s.GetToken();	// =
				prop.nLevel		= s.GetNumber();
			}
			else if( s.Token == "Wormon" )
			{
				prop.vecWormon.clear();
				int nErrCnt = 0;
				WORMON WorMon;
				s.GetToken();
				while( s.Token[0] != '}' )
				{
					memset( &WorMon, 0, sizeof(WORMON) );
					nErrCnt++;

					WorMon.dwWormon = s.GetNumber();
					s.GetToken();	// ,
					WorMon.vPos.x   = s.GetFloat();
					s.GetToken();	// ,
					WorMon.vPos.y   = s.GetFloat();
					s.GetToken();	// ,
					WorMon.vPos.z   = s.GetFloat();
					s.GetToken();	// ,
					
					prop.vecWormon.push_back( WorMon );

					if( nErrCnt > 1000 )
					{
						Error( "propPartyQuest.inc = %s Error", prop.szTitle );
						return FALSE;
					}
				}				
			}
			else if( s.Token == "World" )
			{
				s.GetToken();	// =
				prop.dwWorldId	= s.GetNumber();
			}
			else if( s.Token == "WorldKey" )
			{
				s.GetToken();	// =
				s.GetToken();
				lstrcpy( prop.szWorldKey, s.Token );
			}	
			else if( s.Token == "Region" )
			{
				s.GetToken();	// =
				prop.x1	= s.GetNumber();
				s.GetToken();	// ,
				prop.y1	= s.GetNumber();
				s.GetToken();	// ,
				prop.x2	= s.GetNumber();
				s.GetToken();	// ,
				prop.y2	= s.GetNumber();
#ifdef __WORLDSERVER
				pProcessor->AddQuestRect( nQuestId, prop.dwWorldId, prop.x1, prop.y1, prop.x2, prop.y2 );
#endif	// __WORLDSERVER
			}
			else if( s.Token == "State" )
			{
				int nState	= s.GetNumber();
				s.GetToken();	// key or anything
				while( s.Token[0] != '}' )
				{
					if( s.Token == "desc" )
					{
						s.GetToken();	// =
						s.GetToken();
						lstrcpy( prop.szDesc[nState], s.Token );
					}
					s.GetToken();	// key or anything
				}
			}
			s.GetToken();
		}
		bool bResult = m_propPartyQuest.insert( map<int, PARTYQUESTPROP>::value_type( nQuestId, prop) ).second;
		nQuestId	= s.GetNumber();	// id
	}
	return TRUE;
}


BOOL CProject::LoadPropQuest( LPCTSTR lpszFileName, BOOL bOptimize )
{
	CScript script;
	if( script.Load( lpszFileName ) == FALSE )
		return FALSE;
	QuestProp propQuest;
	TCHAR szLinkChar[ 10 ][ 64 ];
	int szLinkCharNum;
	int szEndCondCharNum;
	int nQuest = script.GetNumber();  // id
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	static DWORD dwGoalIndex = 0;
#endif // __IMPROVE_QUEST_INTERFACE

	while( script.tok != FINISHED )
	{
		ZeroMemory( &propQuest, sizeof( propQuest ) );

		BOOL bEndCondCharacter = FALSE;
		propQuest.m_nBeginCondSex = -1;
		propQuest.m_wId = nQuest;

		QuestPropItem aEndRewardItem[ MAX_QUESTCONDITEM ]; 
		int nEndRewardItemNum = 0;
		ZeroMemory( aEndRewardItem, sizeof( aEndRewardItem ) );

		QuestPropItem aBeginCondItem[ MAX_QUESTCONDITEM ]; 
		int nBeginCondItemNum = 0;
		ZeroMemory( aBeginCondItem, sizeof( aBeginCondItem ) );

#if __VER >= 8 // __S8_PK
		QuestPropItem aBeginCondNotItem[ MAX_QUESTCONDITEM ]; 
		int nBeginCondNotItemNum = 0;
		ZeroMemory( aBeginCondNotItem, sizeof( aBeginCondNotItem ) );		

		QuestPropItem aEndCondOneItem[ MAX_QUESTCONDITEM ]; 
		int nEndCondOneItemNum = 0;
		ZeroMemory( aEndCondOneItem, sizeof( aEndCondOneItem ) );				
#endif // __VER >= 8 // __S8_PK

		QuestPropItem aEndCondItem[ MAX_QUESTCONDITEM ]; 
		int nEndCondItemNum = 0;
		ZeroMemory( aEndCondItem, sizeof( aEndCondItem ) );

		ZeroMemory( szLinkChar, sizeof( szLinkChar ) );
		szLinkCharNum = 0;
		szEndCondCharNum = 0;

		#if defined( __WORLDSERVER ) 
			ZeroMemory( propQuest.m_apQuestDialog, sizeof( propQuest.m_apQuestDialog ) );
		#endif

#if __VER >= 9	// __PET_0410
		propQuest.m_nBeginCondPetLevel	= -1;	// PL_EGG가 0이므로 기본값은 -1
		propQuest.m_nEndCondPetLevel	= -1;	// PL_EGG가 0이므로 기본값은 -1
#endif	// __PET_0410
#if __VER >= 12 // __MOD_TUTORIAL
		propQuest.m_nBeginCondTutorialState		= -1;
#endif	// __MOD_TUTORIAL

		script.GetToken();
		int nBlock = 1;
		while( nBlock && script.tok != FINISHED )
		{
			script.GetToken();
			if( script.Token == "{" )			
				nBlock++;
			else
			if( script.Token == "}" )			
				nBlock--;
			else
			if( script.Token == "SetTitle" )
			{
				script.GetToken(); // (
				strcpy(	propQuest.m_szTitle, GetLangScript( script ) );
			}
			else
			if( script.Token == "m_szTitle" )
			{
				script.GetToken(); // (
				script.GetToken();
				strcpy(	propQuest.m_szTitle, script.Token );
			}
			else
			if( script.Token == "SetNPCName" )
			{
				script.GetToken(); // (
				strcpy(	propQuest.m_szNpcName, GetLangScript( script ) );
			}
			else
			if( script.Token == "m_szNPC" )
			{
				script.GetToken();
				script.GetToken();
			}
			else
			if( script.Token == "SetCharacter" )
			{
				script.GetToken(); // (
				script.GetToken(); // key
#if __VER >= 13 // chipi_090702 : 국가 코드 추가
				char szKey[512] = {0,};
				strcpy( szKey, script.token );
				
				script.GetToken(); // , or )
				if( script.Token ==  "," )
				{
					int nLang = script.GetNumber();
					script.GetToken(); // ,
					int nSubLang = 0;
					if( script.Token == "," )
						nSubLang = script.GetNumber();
					if( nLang != ::GetLanguage() || nSubLang != ::GetSubLanguage() )
						continue;
				}
				szLinkCharNum = 0;
				strcpy( szLinkChar[ szLinkCharNum ], szKey ); 
#else // chipi_090702 : 국가 코드 추가
				szLinkCharNum = 0;
				strcpy( szLinkChar[ szLinkCharNum ], script.token );
#endif // chipi_090702 : 국가 코드 추가
				LPCHARACTER lpCharacter = GetCharacter( szLinkChar[ szLinkCharNum++ ] );
				if( lpCharacter ) 
				{
					lpCharacter->m_awSrcQuest.Add( nQuest );
					lpCharacter->m_anSrcQuestItem.Add( 0 );
				}
			}
			else
			if( script.Token == "SetMultiCharacter" )
			{
				script.GetToken(); // (
				szLinkCharNum = 0;
				do
				{
					script.GetToken(); // key
					strcpy( szLinkChar[ szLinkCharNum ], script.token ); 
					script.GetToken(); // ,
					int nItem = script.GetNumber();
					LPCHARACTER lpCharacter = GetCharacter( szLinkChar[ szLinkCharNum ] );
					if( lpCharacter ) 
					{
						lpCharacter->m_awSrcQuest.Add( nQuest );
						lpCharacter->m_anSrcQuestItem.Add( nItem );
					}
					else
					{
						Error( "%s(%d) : %s가 존재하지 않음", lpszFileName, script.GetLineNum(), szLinkChar[ szLinkCharNum ] );
					}
					szLinkCharNum++;
					script.GetToken(); // , or )
				} while( *script.token != ')' && szLinkCharNum < 10 );
			}
			else
			// 시작 - 조건 
			if( script.Token == "SetBeginCondSex" )
			{
				script.GetToken(); // (
				propQuest.m_nBeginCondSex  = script.GetNumber();
			}
			else 
			if( script.Token == "SetBeginCondSkillLvl" )
			{
				script.GetToken(); // (
				propQuest.m_nBeginCondSkillIdx = script.GetNumber(); 
				script.GetToken(); // ,
				propQuest.m_nBeginCondSkillLvl = script.GetNumber();
			}
#if __VER >= 8 // __S8_PK
			else
			if( script.Token == "SetBeginCondPKValue" )
			{
				script.GetToken(); // (
				propQuest.m_nBeginCondPKValue = script.GetNumber();
			}
			else
			if( script.Token == "SetBeginCondNotItem" )
			{
				script.GetToken(); // (
				int nSex = script.GetNumber();
				script.GetToken(); // ,
				int nType = script.GetNumber();
				script.GetToken(); // ,
				int nJobOrItem = script.GetNumber();
				if( nType == 0 && nJobOrItem >= MAX_QUESTCONDITEM )
				{
					nJobOrItem = -1;
					Error( "%s(%d) SetBeginCondNotItem 범위 초과(0~%d)", lpszFileName, script.GetLineNum(), MAX_QUESTCONDITEM ); 
				}
				script.GetToken(); // ,
				aBeginCondNotItem[ nBeginCondNotItemNum ].m_nSex = nSex;
				aBeginCondNotItem[ nBeginCondNotItemNum ].m_nType = nType;
				aBeginCondNotItem[ nBeginCondNotItemNum ].m_nJobOrItem =  nJobOrItem;
				aBeginCondNotItem[ nBeginCondNotItemNum ].m_nItemIdx = script.GetNumber();
				script.GetToken(); // ,
				aBeginCondNotItem[ nBeginCondNotItemNum ].m_nItemNum = script.GetNumber();
				nBeginCondNotItemNum++;
			}
#else // __VER >= 8 // __S8_PK
			else 
			if( script.Token == "SetBeginCondKarmaPoint" )
			{
				script.GetToken(); // (
				propQuest.m_nBeginCondKarmaComp = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nBeginCondKarmaPoint = script.GetNumber();
			}
			else 
			if( script.Token == "SetBeginCondChaotic" )
			{
				script.GetToken(); // (
				propQuest.m_nBeginCondChaotic = script.GetNumber();
			}
#endif // __VER >= 8 // __S8_PK
			else
			if( script.Token == "SetBeginCondLevel" )
			{
				script.GetToken(); // (
				propQuest.m_nBeginCondLevelMin = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nBeginCondLevelMax = script.GetNumber();
			}
			else 
			if( script.Token == "SetBeginCondParty" ) 
			{
				script.GetToken(); // (
				propQuest.m_nBeginCondParty = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nBeginCondPartyNumComp = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nBeginCondPartyNum = script.GetNumber();
				script.GetToken(); // , 
				propQuest.m_nBeginCondPartyLeader = script.GetNumber(); 
			}
			else 
			if( script.Token == "SetBeginCondGuild" ) 
			{
				script.GetToken(); // (
				propQuest.m_nBeginCondGuild = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nBeginCondGuildNumComp = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nBeginCondGuildNum = script.GetNumber();
				script.GetToken(); // , 
				propQuest.m_nBeginCondGuildLeader = script.GetNumber(); 
			}
			else
			if( script.Token == "SetBeginCondJob" )
			{
				script.GetToken(); // (
				do
				{
					propQuest.m_nBeginCondJob[ propQuest.m_nBeginCondJobNum++ ] = script.GetNumber();
					script.GetToken(); // , or )
				} while( *script.token != ')' && propQuest.m_nBeginCondJobNum < MAX_JOB );
			}
			else
			if( script.Token == "SetBeginCondPreviousQuest" )
			{
				script.GetToken(); // (
				propQuest.m_nBeginCondPreviousQuestType = script.GetNumber(); // 0 시작중, 1 이전 퀘스트 완료
				script.GetToken(); // ,
				int nCnt = 0;
				do
				{
					propQuest.m_anBeginCondPreviousQuest[ nCnt++ ] = script.GetNumber();
					script.GetToken(); // , or )
				} while( *script.token != ')' && nCnt < 6 );
			}
			else
			if( script.Token == "SetBeginCondExclusiveQuest" )
			{
				script.GetToken(); // (
				int nCnt = 0;
				do
				{
					propQuest.m_anBeginCondExclusiveQuest[ nCnt++ ] = script.GetNumber();
					script.GetToken(); // , or )
				} while( *script.token != ')' && nCnt < 6 );
			}
			else
			if( script.Token == "SetBeginCondItem" )
			{
				script.GetToken(); // (
				int nSex = script.GetNumber();
				script.GetToken(); // ,
				int nType = script.GetNumber();
				script.GetToken(); // ,
				int nJobOrItem = script.GetNumber();
				if( nType == 0 && nJobOrItem >= MAX_QUESTCONDITEM )
				{
					nJobOrItem = -1;
					Error( "%s(%d) SetBeginCondItem 범위 초과(0~%d)", lpszFileName, script.GetLineNum(), MAX_QUESTCONDITEM ); 
				}
				script.GetToken(); // ,
				aBeginCondItem[ nBeginCondItemNum ].m_nSex = nSex;
				aBeginCondItem[ nBeginCondItemNum ].m_nType = nType;
				aBeginCondItem[ nBeginCondItemNum ].m_nJobOrItem =  nJobOrItem;
				aBeginCondItem[ nBeginCondItemNum ].m_nItemIdx = script.GetNumber();
				script.GetToken(); // ,
				aBeginCondItem[ nBeginCondItemNum ].m_nItemNum = script.GetNumber();
				nBeginCondItemNum++;
			}
			else
			if( script.Token == "SetBeginCondDisguise" )
			{
				script.GetToken(); // (
				propQuest.m_nBeginCondDisguiseMoverIndex = script.GetNumber();
			}
			else
			//////////////////////////////////////////////////////////////////
			// 시작 - 추가 및 변경 
			//////////////////////////////////////////////////////////////////
			if( script.Token == "SetBeginSetDisguise" )
			{
				script.GetToken(); // (
				propQuest.m_nBeginSetDisguiseMoverIndex = script.GetNumber();
			}
			else 
			if( script.Token == "SetBeginSetAddGold" )
			{
				script.GetToken(); // (
				propQuest.m_nBeginSetAddGold = script.GetNumber();
			}
			else 
			if( script.Token == "SetBeginSetAddItem" )
			{
				script.GetToken(); // (
				int nIdx = script.GetNumber();
				if( nIdx < 0 || nIdx > 3 )
				{
					nIdx = 0;
					Error( "%s(%d) SetBeginSetAddItem Idx 값 범위 초과(0~3)", lpszFileName, script.GetLineNum() ); 
				}
				script.GetToken(); // ,
				propQuest.m_nBeginSetAddItemIdx[nIdx] = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nBeginSetAddItemNum[nIdx] = script.GetNumber();
			}
#if __VER >= 9	// __PET_0410
			else if( script.Token == "SetBeginCondPetExp" )
			{
				script.GetToken();	// (
				propQuest.m_nBeginCondPetExp	= script.GetNumber();
			}
			else if( script.Token == "SetBeginCondPetLevel" )
			{
				script.GetToken();	// (
				propQuest.m_nBeginCondPetLevel	= script.GetNumber();
			}
#endif	// __PET_0410
#if __VER >= 12 // __MOD_TUTORIAL
			else if( script.Token == "SetBeginCondTutorialState" )
			{
				script.GetToken();	// (
				propQuest.m_nBeginCondTutorialState		= script.GetNumber();
			}
#endif	// __MOD_TUTORIAL
#if __VER >= 15 // __CAMPUS
			else if( script.Token == "SetBeginCondTSP" )
			{
				script.GetToken();	// (
				propQuest.m_nBeginCondTSP	= script.GetNumber();
			}
#endif // __CAMPUS
			else 
			////////////////////////// 종료 - 조건 
			if( script.Token == "SetEndCondParty" ) 
			{
				script.GetToken(); // (
				propQuest.m_nEndCondParty = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndCondPartyNumComp = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndCondPartyNum = script.GetNumber();
				script.GetToken(); // , 
				propQuest.m_nEndCondPartyLeader = script.GetNumber();
			}
			else
			if( script.Token == "SetEndCondGuild" ) 
			{
				script.GetToken(); // (
				propQuest.m_nEndCondGuild = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndCondGuildNumComp = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndCondGuildNum = script.GetNumber();
				script.GetToken(); // , 
				propQuest.m_nEndCondGuildLeader = script.GetNumber();
			}
			else
			if( script.Token == "SetEndCondState" ) 
			{
				script.GetToken(); // (
				propQuest.m_nEndCondState = script.GetNumber();
			}
			else 
			if( script.Token == "SetEndCondCompleteQuest" ) 
			{
				script.GetToken(); // (
				propQuest.m_nEndCondCompleteQuestOper = script.GetNumber();
				script.GetToken(); // ,
				int nIdx = 0;
				while( *script.token != ')' )
				{
					propQuest.m_nEndCondCompleteQuest[ nIdx++ ] = script.GetNumber();
					script.GetToken(); // ,
				}
			}
			else
			if( script.Token == "SetEndCondSkillLvl" )
			{
				script.GetToken(); // (
				propQuest.m_nEndCondSkillIdx = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndCondSkillLvl = script.GetNumber();
			}
#if __VER >= 8 // __S8_PK
			else
			if( script.Token == "SetEndCondLevel" )
			{
				script.GetToken(); // (
				propQuest.m_nEndCondLevelMin = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndCondLevelMax = script.GetNumber();
			}
#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
			else
			if( script.Token == "SetEndCondExpPercent" )
			{
				script.GetToken(); // (
				propQuest.m_nEndCondExpPercentMin = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndCondExpPercentMax = script.GetNumber();
			}
#endif//	__LEGEND	//	10차 전승시스템	Neuz, World, Trans
			else
			if( script.Token == "SetEndCondGold" )
			{
				script.GetToken(); // (
				propQuest.m_nEndCondGold = script.GetNumber();
			}
			else
			if( script.Token == "SetEndCondOneItem" )
			{
				script.GetToken(); // (
				int nSex = script.GetNumber();
				script.GetToken(); // ,
				int nType = script.GetNumber();
				script.GetToken(); // ,
				int nJobOrItem = script.GetNumber();
				if( nType == 0 && nJobOrItem >= MAX_QUESTCONDITEM )
				{
					nJobOrItem = -1;
					Error( "%s(%d) SetEndCondOneItem 범위 초과(0~%d)", lpszFileName, script.GetLineNum(), MAX_QUESTCONDITEM ); 
				}
				script.GetToken(); // ,
				
				aEndCondOneItem[ nEndCondOneItemNum ].m_nSex = nSex;
				aEndCondOneItem[ nEndCondOneItemNum ].m_nType = nType;
				aEndCondOneItem[ nEndCondOneItemNum ].m_nJobOrItem =  nJobOrItem;
				aEndCondOneItem[ nEndCondOneItemNum ].m_nItemIdx = script.GetNumber();
				script.GetToken(); // ,
				aEndCondOneItem[ nEndCondOneItemNum ].m_nItemNum = script.GetNumber();
				nEndCondOneItemNum++;
			}
#else // __VER >= 8 // __S8_PK
			else 
			if( script.Token == "SetEndCondKarmaPoint" )
			{
				script.GetToken(); // (
				propQuest.m_nEndCondKarmaComp = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndCondKarmaPoint = script.GetNumber();
			}
			else 
			if( script.Token == "SetEndCondChaotic" )
			{
				script.GetToken(); // (
				propQuest.m_nEndCondChaotic = script.GetNumber();
			}
#endif // __VER >= 8 // __S8_PK
			else
			if( script.Token == "SetEndCondLimitTime" )
			{
				script.GetToken(); // (
				propQuest.m_nEndCondLimitTime = script.GetNumber();
			}
			else
			if( script.Token == "SetEndCondItem" )
			{
				script.GetToken(); // (
				int nSex = script.GetNumber();
				script.GetToken(); // ,
				int nType = script.GetNumber();
				script.GetToken(); // ,
				int nJobOrItem = script.GetNumber();
				if( nType == 0 && nJobOrItem >= MAX_QUESTCONDITEM )
				{
					nJobOrItem = -1;
					Error( "%s(%d) SetEndCondItem 범위 초과(0~%d)", lpszFileName, script.GetLineNum(), MAX_QUESTCONDITEM ); 
				}
				script.GetToken(); // ,

				aEndCondItem[ nEndCondItemNum ].m_nSex = nSex;
				aEndCondItem[ nEndCondItemNum ].m_nType = nType;
				aEndCondItem[ nEndCondItemNum ].m_nJobOrItem =  nJobOrItem;
				aEndCondItem[ nEndCondItemNum ].m_nItemIdx = script.GetNumber();
				script.GetToken(); // ,
				aEndCondItem[ nEndCondItemNum ].m_nItemNum = script.GetNumber();
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
				script.GetToken(); // , or )
				if( script.Token ==  "," )
				{
					aEndCondItem[ nEndCondItemNum ].m_ItemGoalData.m_dwGoalIndex = ++dwGoalIndex;
					aEndCondItem[ nEndCondItemNum ].m_ItemGoalData.m_fGoalPositionX = script.GetFloat();
					script.GetToken(); // ,
					aEndCondItem[ nEndCondItemNum ].m_ItemGoalData.m_fGoalPositionZ = script.GetFloat();
					script.GetToken(); // ,
					script.GetToken_NoDef(); // szGoalTextID
					aEndCondItem[ nEndCondItemNum ].m_ItemGoalData.m_dwGoalTextID = atoi( script.Token.Right( 4 ) );
				}
#endif // __IMPROVE_QUEST_INTERFACE

				nEndCondItemNum++;
			}
			else 
			if( script.Token == "SetEndCondKillNPC" )
			{
				script.GetToken(); // (
				int nIdx = script.GetNumber();
				if( nIdx < 0 || nIdx > MAX_QUEST_COND_KILL )
				{
					nIdx = 0;
					Error( "%s(%d) SetEndCondKillNPC에서 Idx 값 범위 초과(0~1)", lpszFileName, script.GetLineNum() ); 
				}
				script.GetToken(); // ,
				propQuest.m_nEndCondKillNPCIdx[nIdx] = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndCondKillNPCNum[nIdx] = script.GetNumber();
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
				script.GetToken(); // , or )
				if( script.Token ==  "," )
				{
					propQuest.m_KillNPCGoalData[ nIdx ].m_dwGoalIndex = ++dwGoalIndex;
					propQuest.m_KillNPCGoalData[ nIdx ].m_fGoalPositionX = script.GetFloat();
					script.GetToken(); // ,
					propQuest.m_KillNPCGoalData[ nIdx ].m_fGoalPositionZ = script.GetFloat();
					script.GetToken(); // ,
					script.GetToken_NoDef(); // szGoalTextID
					propQuest.m_KillNPCGoalData[ nIdx ].m_dwGoalTextID = atoi( script.Token.Right( 4 ) );
				}
#endif // __IMPROVE_QUEST_INTERFACE
			}
			else
			if( script.Token == "SetEndCondPatrolZone" )
			{
				script.GetToken(); // (
				propQuest.m_dwEndCondPatrolWorld = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_rectEndCondPatrol.left = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_rectEndCondPatrol.top = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_rectEndCondPatrol.right = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_rectEndCondPatrol.bottom = script.GetNumber();
				propQuest.m_rectEndCondPatrol.NormalizeRect();
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
				script.GetToken(); // , or )
				if( script.Token ==  "," )
				{
					script.GetToken_NoDef();
					propQuest.m_dwPatrolDestinationID = atoi( script.Token.Right( 4 ) );
					script.GetToken(); // ,
					propQuest.m_PatrolWorldGoalData.m_dwGoalIndex = ++dwGoalIndex;
					propQuest.m_PatrolWorldGoalData.m_fGoalPositionX = script.GetFloat();
					script.GetToken(); // ,
					propQuest.m_PatrolWorldGoalData.m_fGoalPositionZ = script.GetFloat();
					script.GetToken(); // ,
					script.GetToken_NoDef(); // szGoalTextID
					propQuest.m_PatrolWorldGoalData.m_dwGoalTextID = atoi( script.Token.Right( 4 ) );
				}
#endif // __IMPROVE_QUEST_INTERFACE
			}
			else
			if( script.Token == "SetEndCondCharacter" )
			{
				script.GetToken(); // (
				script.GetToken(); // "key"
				// 스트링이 없으면 szLinkChar에 세팅된 것으로 대체한다. 즉 자기 자신이다.
				if( strlen( script.token ) == 0 )
					strcpy( propQuest.m_szEndCondCharacter, szLinkChar[ 0 ] );
				else
					strcpy( propQuest.m_szEndCondCharacter, script.token );
				LPCHARACTER lpCharacter = GetCharacter( propQuest.m_szEndCondCharacter );
				if( lpCharacter ) 
				{
					lpCharacter->m_awDstQuest.Add( nQuest );
					lpCharacter->m_anDstQuestItem.Add( 0 );
				}
				bEndCondCharacter = TRUE;
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
				script.GetToken(); // , or )
				if( script.Token ==  "," )
				{
					propQuest.m_MeetCharacterGoalData.m_dwGoalIndex = ++dwGoalIndex;
					propQuest.m_MeetCharacterGoalData.m_fGoalPositionX = script.GetFloat();
					script.GetToken(); // ,
					propQuest.m_MeetCharacterGoalData.m_fGoalPositionZ = script.GetFloat();
					script.GetToken(); // ,
					script.GetToken_NoDef(); // szGoalTextID
					propQuest.m_MeetCharacterGoalData.m_dwGoalTextID = atoi( script.Token.Right( 4 ) );
				}
#endif // __IMPROVE_QUEST_INTERFACE
			}
			else
			if( script.Token == "SetEndCondMultiCharacter" )
			{
				script.GetToken(); // (
				szEndCondCharNum = 0;
				propQuest.m_lpszEndCondMultiCharacter = new CHAR[ 64 * 10 ];
				ZeroMemory( propQuest.m_lpszEndCondMultiCharacter, 64 * 10 );
				do
				{
					script.GetToken(); // key
					strcpy( &propQuest.m_lpszEndCondMultiCharacter[ szEndCondCharNum * 64 ], script.token ); 

					script.GetToken(); // ,
					int nItem = script.GetNumber();

					LPCHARACTER lpCharacter = GetCharacter( &propQuest.m_lpszEndCondMultiCharacter[ szEndCondCharNum * 64 ] );
					if( lpCharacter ) 
					{
						lpCharacter->m_awDstQuest.Add( nQuest );
						lpCharacter->m_anDstQuestItem.Add( nItem );
					}
					else
					{
						Error( "%s(%d) : %s가 존재하지 않음", lpszFileName, script.GetLineNum(), szLinkChar[ szLinkCharNum ] );
					}
					szEndCondCharNum++;
					script.GetToken(); // , or )
				} while( *script.token != ')' && szEndCondCharNum < 10 );
				bEndCondCharacter = TRUE;
			}
			else
			if( script.Token == "SetEndCondDialog" )
			{
				script.GetToken(); // (
				script.GetToken(); 
				strcpy( propQuest.m_szEndCondDlgCharKey, script.token );
				script.GetToken(); // ,
				script.GetToken(); 
				strcpy( propQuest.m_szEndCondDlgAddKey, script.token );
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
				script.GetToken(); // , or )
				if( script.Token ==  "," )
				{
					propQuest.m_DialogCharacterGoalData.m_dwGoalIndex = ++dwGoalIndex;
					propQuest.m_DialogCharacterGoalData.m_fGoalPositionX = script.GetFloat();
					script.GetToken(); // ,
					propQuest.m_DialogCharacterGoalData.m_fGoalPositionZ = script.GetFloat();
					script.GetToken(); // ,
					script.GetToken_NoDef(); // szGoalTextID
					propQuest.m_DialogCharacterGoalData.m_dwGoalTextID = atoi( script.Token.Right( 4 ) );
				}
#endif // __IMPROVE_QUEST_INTERFACE
			}
#if __VER >= 9	// __PET_0410
			else if( script.Token == "SetEndCondPetLevel" )
			{	// SetEndCondPetLevel( 0 );
				script.GetToken();	// (
				propQuest.m_nEndCondPetLevel	= script.GetNumber();
			}
			else if( script.Token == "SetEndCondPetExp" )
			{
				// SetEndCondPetExp( 100 );
				script.GetToken();	// (
				propQuest.m_nEndCondPetExp	= script.GetNumber();
			}
#endif	// __PET_0410
			else
			if( script.Token == "SetEndCondDisguise" )
			{
				script.GetToken(); // (
				propQuest.m_nEndCondDisguiseMoverIndex = script.GetNumber();
			}
			else
			if( script.Token == "SetParam" )
			{
				script.GetToken(); // (
				int nIdx = script.GetNumber();
				if( nIdx < 0 || nIdx > 3 )
				{
					nIdx = 0;
					Error( "%s(%d) SetParam Idx 값 범위 초과(0~3)", lpszFileName, script.GetLineNum() ); 
				}
				script.GetToken(); // ,
				propQuest.m_nParam[ nIdx ] = script.GetNumber();
			}
#if __VER >= 15 // __CAMPUS
			else
			if( script.Token == "SetEndCondTSP" )
			{
				script.GetToken(); // (
				propQuest.m_nEndCondTSP  = script.GetNumber();
			}
#endif // __CAMPUS
			else
			////////////////////////// 대화 - 보상 
			if( script.Token == "SetDlgRewardItem" )
			{
				script.GetToken(); // (
				int nIdx = script.GetNumber();
				if( nIdx < 0 || nIdx > 3 )
				{
					nIdx = 0;
					Error( "%s(%d) SetDlgRewardItem Idx 값 범위 초과(0~3)", lpszFileName, script.GetLineNum() ); 
				}
				script.GetToken(); // ,
				propQuest.m_nDlgRewardItemIdx[ nIdx ] = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nDlgRewardItemNum[ nIdx ] = script.GetNumber();
			}
			else
			if( script.Token == "SetEndRewardItem" )
			{
				script.GetToken(); // ( 

				int nSex = script.GetNumber();
				script.GetToken(); // ,

				int nType = script.GetNumber();
				script.GetToken(); // ,
				int nJobOrItem = script.GetNumber();
				if( nType == 0 && nJobOrItem >= MAX_QUESTCONDITEM )
				{
					nJobOrItem = -1;
					Error( "%s(%d) SetEndRewardItem 값 범위 초과(0~%d)", lpszFileName, script.GetLineNum(), MAX_QUESTCONDITEM ); 
				}
				script.GetToken(); // ,

				aEndRewardItem[ nEndRewardItemNum ].m_nSex = nSex;
				aEndRewardItem[ nEndRewardItemNum ].m_nType = nType;
				aEndRewardItem[ nEndRewardItemNum ].m_nJobOrItem =  nJobOrItem;
				aEndRewardItem[ nEndRewardItemNum ].m_nItemIdx = script.GetNumber();
				script.GetToken(); // ,
				aEndRewardItem[ nEndRewardItemNum ].m_nItemNum = script.GetNumber();
#if __VER >= 13 // __CHIPI_QUESTITEM_FLAG
				script.GetToken();
				if( script.Token == "," )	// , (귀속설정)
					aEndRewardItem[ nEndRewardItemNum ].m_byFlag = static_cast<BYTE>( script.GetNumber() );
#endif // __CHIPI_QUESTITEM_FLAG
			
				nEndRewardItemNum++;
			}
#ifdef __JEFF_11
			else if( script.Token == "SetEndRewardItemWithAbilityOption" )
			{
				script.GetToken();	// ( 
				int nSex	= script.GetNumber();
				script.GetToken();	// ,
				int nType	= script.GetNumber();
				script.GetToken();	// ,
				int nJobOrItem	= script.GetNumber();
				if( nType == 0 && nJobOrItem >= MAX_QUESTCONDITEM )
				{
					nJobOrItem	= -1;
					Error( "%s(%d) SetEndRewardItem 값 범위 초과(0~%d)", lpszFileName, script.GetLineNum(), MAX_QUESTCONDITEM ); 
				}
				script.GetToken();	// ,
				aEndRewardItem[nEndRewardItemNum].m_nSex	= nSex;
				aEndRewardItem[nEndRewardItemNum].m_nType	= nType;
				aEndRewardItem[nEndRewardItemNum].m_nJobOrItem	=  nJobOrItem;
				aEndRewardItem[nEndRewardItemNum].m_nItemIdx	= script.GetNumber();
				script.GetToken();	// ,
				aEndRewardItem[nEndRewardItemNum].m_nItemNum	= script.GetNumber();
				script.GetToken();	// ,
				aEndRewardItem[nEndRewardItemNum].m_nAbilityOption	= script.GetNumber();
#if __VER >= 13 // __CHIPI_QUESTITEM_FLAG
				script.GetToken();
				if( script.Token == "," )	// , (귀속설정)
					aEndRewardItem[ nEndRewardItemNum ].m_byFlag = static_cast<BYTE>( script.GetNumber() );
#endif // __CHIPI_QUESTITEM_FLAG
			
				nEndRewardItemNum++;
			}
#endif	// __JEFF_11
			else
			if( script.Token == "SetEndRewardGold" )
			{
				script.GetToken(); // (
				propQuest.m_nEndRewardGoldMin = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndRewardGoldMax = script.GetNumber();
			}
#if __VER >= 9	// __PET_0410
			else if( script.Token == "SetEndRewardPetLevelup" )
			{
				propQuest.m_bEndRewardPetLevelup	= TRUE;
			}
#endif	// __PET_0410
			else
			if( script.Token == "SetEndRewardExp" )
			{
				script.GetToken(); // (
				propQuest.m_nEndRewardExpMin = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndRewardExpMax = script.GetNumber();
			}
			else
			if( script.Token == "SetEndRewardSkillPoint" )
			{
				script.GetToken(); // (
				propQuest.m_nEndRewardSkillPoint = script.GetNumber();
			}
#if __VER >= 8 // __S8_PK
			else
			if( script.Token == "SetEndRewardPKValue" )
			{
				script.GetToken(); // (
				propQuest.m_nEndRewardPKValueMin = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndRewardPKValueMax = script.GetNumber();
			}
#else // __VER >= 8 // __S8_PK
			else
			if( script.Token == "SetEndRewardKarmaPoint" )
			{
				script.GetToken(); // (
				propQuest.m_nEndRewardKarmaStyle = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndRewardKarmaPoint = script.GetNumber();
			}
#endif // __VER >= 8 // __S8_PK
#if __VER >= 9 // __S_9_ADD
			else
			if( script.Token == "SetEndRewardTeleport" )
			{
				script.GetToken(); // (
				propQuest.m_nEndRewardTeleport = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndRewardTeleportPos.x = script.GetFloat();
				script.GetToken(); // ,
				propQuest.m_nEndRewardTeleportPos.y = script.GetFloat();
				script.GetToken(); // ,
				propQuest.m_nEndRewardTeleportPos.z = script.GetFloat();
			}
#endif // __S_9_ADD
			else
			if( script.Token == "SetEndRewardHide" )
			{
				script.GetToken(); // (
				propQuest.m_bEndRewardHide = ( script.GetNumber() != 0 );
			}
			else
			// 종료 퀘스트 제거 
			if( script.Token == "SetEndRemoveQuest" )
			{
				script.GetToken(); // (
				int nRemoveQuestIdx = 0;
				do
				{
					propQuest.m_anEndRemoveQuest[ nRemoveQuestIdx++ ] = script.GetNumber();;
					script.GetToken(); // , or )
				} while( *script.token != ')' && nRemoveQuestIdx < MAX_QUESTREMOVE );
			}
			else
			// 종료 아이템 제거 
			if( script.Token == "SetEndRemoveItem" )
			{
				script.GetToken(); // (
				int nIdx = script.GetNumber();
				if( nIdx < 0 || nIdx > 7 )
				{
					nIdx = 0;
					Error( "%s(%d) SetEndRemoveItem Idx 값 범위 초과(0~3)", lpszFileName, script.GetLineNum() ); 
				}
				script.GetToken(); // ,
				propQuest.m_nEndRemoveItemIdx[ nIdx ] = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndRemoveItemNum[ nIdx ] = script.GetNumber();
			}
			else 
			if( script.Token == "SetEndRemoveGold" )
			{
				script.GetToken(); // (
				propQuest.m_nEndRemoveGold = script.GetNumber();
			}
			else 
			if( script.Token == "SetRepeat" )
			{
				script.GetToken(); // (
				propQuest.m_bRepeat = ( script.GetNumber() != 0 );
			}
			else 
			if( script.Token == "QuestItem" )
			{
				QUESTITEM qi;
				script.GetToken();	// (
				DWORD dwMoverIdx = script.GetNumber();
				qi.dwQuest	= nQuest;//script.GetNumber();
				qi.dwState	= 0;//script.GetNumber();
				script.GetToken();	// ,
				qi.dwIndex	= script.GetNumber();
				ASSERT( qi.dwIndex );
				script.GetToken();	// ,
				qi.dwProbability	= script.GetNumber();	//
				script.GetToken();	// ,
				qi.dwNumber	= script.GetNumber();	//
				script.GetToken();	// )
				MoverProp* pMoverProp = GetMoverProp( dwMoverIdx );
				pMoverProp->m_QuestItemGenerator.AddTail( qi );	// copy
			}
#if __VER >= 15 // __CAMPUS
			else
			if( script.Token == "SetEndRewardTSP" )
			{
				script.GetToken(); // (
				propQuest.m_nEndRewardTSP  = script.GetNumber();
			}
			else
			if( script.Token == "SetEndRemoveTSP" )
			{
				script.GetToken(); // (
				propQuest.m_nEndRemoveTSP  = script.GetNumber();
			}
#endif // __CAMPUS
			else
			//////////////////////////
			if( script.Token == "SetDialog" )
			{
				script.GetToken(); // (
				int nNum = script.GetNumber();
				script.GetToken(); // ,
				CString string = GetLangScript( script );
			#if defined( __WORLDSERVER )
				if( nNum < 32 )
				{
					propQuest.m_apQuestDialog[ nNum ] = new CHAR[ string.GetLength() + 1 ];
					strcpy(	propQuest.m_apQuestDialog[ nNum ], string );
				}
			#endif 
			}
			if( script.Token == "SetPatrolZoneName" )
			{
				script.GetToken(); // (
				//int nNum = script.GetNumber();
				//script.GetToken(); // ,
				CString string = GetLangScript( script );
				if( string.GetLength() < 64 )
					strcpy( propQuest.m_szPatrolZoneName, string );
				else
					Error( "%s(%d) SetPatrolZoneName 스트링 64바이트 초과", lpszFileName, script.GetLineNum() ); 
			}
			else
			if( script.Token == "SetHeadQuest" )
			{
				script.GetToken(); // (
				propQuest.m_nHeadQuest = script.GetNumber();
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
				if( propQuest.m_nHeadQuest >= 1800 && propQuest.m_nHeadQuest <= 1999 )
				{
					switch( propQuest.m_nHeadQuest )
					{
					case 1992:
						{
							propQuest.m_nHeadQuest = QUEST_KIND_EVENT;
							break;
						}
					case 1993:
					case 1994:
					case 1995:
					case 1997:
					case 1998:
						{
							propQuest.m_nHeadQuest = QUEST_KIND_NORMAL;
							break;
						}
					case 1996:
						{
							propQuest.m_nHeadQuest = QUEST_KIND_SCENARIO;
							break;
						}
					case 1999:
						{
							propQuest.m_nHeadQuest = QUEST_KIND_REQUEST;
							break;
						}
					default:
						{
							propQuest.m_nHeadQuest = QUEST_KIND_SCENARIO;
						}
					}
				}
#endif // __IMPROVE_QUEST_INTERFACE
			}
			else
			if( script.Token == "SetQuestType" )
			{
				script.GetToken(); // (
				propQuest.m_nQuestType = script.GetNumber();
			}
			else
			if( script.Token == "SetRemove" )
			{
				script.GetToken(); // (
				propQuest.m_bNoRemove = !script.GetNumber();
			}
			else
			if( script.Token == "state" )
			{
				int nState = script.GetNumber();

				QuestState* pQuestState = new QuestState;
				propQuest.m_questState[ nState ] = pQuestState;
				pQuestState->m_szDesc[ 0 ] = 0;
				script.GetToken(); // key or anything

				int temp=0;


				while( script.Token[0] != '}' )
				{
					temp++;
					
					if( script.Token == "QuestItem" )
					{
						QUESTITEM qi;
						script.GetToken();	// (
						DWORD dwMoverIdx = script.GetNumber();
						qi.dwQuest	= nQuest;//script.GetNumber();
						qi.dwState	= nState;//script.GetNumber();
						script.GetToken();	// ,
						qi.dwIndex	= script.GetNumber();
						script.GetToken();	// ,
						qi.dwProbability	= script.GetNumber();	//
						script.GetToken();	// ,
						qi.dwNumber	= script.GetNumber();	//
						script.GetToken();	// )
						MoverProp* pMoverProp = GetMoverProp( dwMoverIdx );
						pMoverProp->m_QuestItemGenerator.AddTail( qi );	// copy
					}
					else
					if( script.Token == "SetDesc" )
					{
						script.GetToken(); // (
						CString string = GetLangScript( script );
						if( string.GetLength() < 512 ) // null 때문에 255
							strcpy(	pQuestState->m_szDesc, string );
						else
							Error( "%s(%d) LoadPropQuest의 SetDesc 길이 초과", lpszFileName, script.GetLineNum() ); 

					}
					else
					if( script.Token == "SetCond" )
					{
						script.GetToken(); // (
						CString string = GetLangScript( script );
					}
					else
					if( script.Token == "SetStatus" )
					{
						script.GetToken(); // (
						CString string = GetLangScript( script );
					}
					script.GetToken(); // key or anything
				}
			}
		}
		// EndCondCharacter세팅이 안됐다면 LinkChar로 대체한다.
		if( bEndCondCharacter == FALSE )
		{
			// 스트링이 없으면 szLinkChar에 세팅된 것으로 대체한다. 즉 자기 자신이다.
			if( szLinkCharNum == 1 )
			{
				LPCHARACTER lpCharacter = GetCharacter( szLinkChar[ 0 ] );
				if( lpCharacter ) 
				{
					lpCharacter->m_awDstQuest.Add( nQuest );
					lpCharacter->m_anDstQuestItem.Add( lpCharacter->m_anSrcQuestItem.GetAt( 0 ) );
				}
				strcpy( propQuest.m_szEndCondCharacter, szLinkChar[ 0 ] );
			}
			else
			if( szLinkCharNum > 1 )
			{
				propQuest.m_lpszEndCondMultiCharacter = new CHAR[ 64 * 10 ];
				ZeroMemory( propQuest.m_lpszEndCondMultiCharacter, 64 * 10 );
				for( int i = 0; i < szLinkCharNum; i++ )
				{
					strcpy( &propQuest.m_lpszEndCondMultiCharacter[ i * 64 ], szLinkChar[ 0 ] );
					LPCHARACTER lpCharacter = GetCharacter( szLinkChar[ i ] );
					if( lpCharacter )
					{
						lpCharacter->m_awDstQuest.Add( nQuest );
						lpCharacter->m_anDstQuestItem.Add( lpCharacter->m_anSrcQuestItem.GetAt( i ) );
					}
				}
			}
		}
		if( nBeginCondItemNum )
		{
			propQuest.m_paBeginCondItem = new QuestPropItem[ nBeginCondItemNum ];
			memcpy( propQuest.m_paBeginCondItem, aBeginCondItem, sizeof( QuestPropItem ) * nBeginCondItemNum );
			propQuest.m_nBeginCondItemNum = nBeginCondItemNum;
		}
#if __VER >= 8 // __S8_PK
		if( nBeginCondNotItemNum )
		{
			propQuest.m_paBeginCondNotItem = new QuestPropItem[ nBeginCondNotItemNum ];
			memcpy( propQuest.m_paBeginCondNotItem, aBeginCondNotItem, sizeof( QuestPropItem ) * nBeginCondNotItemNum );
			propQuest.m_nBeginCondNotItemNum = nBeginCondNotItemNum;
		}
		
		if( nEndCondOneItemNum )
		{
			propQuest.m_paEndCondOneItem = new QuestPropItem[ nEndCondOneItemNum ];
			memcpy( propQuest.m_paEndCondOneItem, aEndCondOneItem, sizeof( QuestPropItem ) * nEndCondOneItemNum );
			propQuest.m_nEndCondOneItemNum = nEndCondOneItemNum;
		}		
#endif // __VER >= 8 // __S8_PK
		if( nEndCondItemNum )
		{
			propQuest.m_paEndCondItem = new QuestPropItem[ nEndCondItemNum ];
			memcpy( propQuest.m_paEndCondItem, aEndCondItem, sizeof( QuestPropItem ) * nEndCondItemNum );
			propQuest.m_nEndCondItemNum = nEndCondItemNum;
		}
		if( nEndRewardItemNum )
		{
			propQuest.m_paEndRewardItem = new QuestPropItem[ nEndRewardItemNum ];
			memcpy( propQuest.m_paEndRewardItem, aEndRewardItem, sizeof( QuestPropItem ) * nEndRewardItemNum );
			propQuest.m_nEndRewardItemNum = nEndRewardItemNum;
		}

		BOOL bAdd	= TRUE;
		// 가위 바위 보 퀘스트는 대만에 한함	// 퀘스트 삭제
		if( //::GetLanguage() != LANG_TWN && 
			nQuest == QUEST_KAWIBAWIBO01 )
			bAdd	= FALSE;
		if( bAdd )
			m_aPropQuest.SetAtGrow( nQuest, &propQuest );

		nQuest = script.GetNumber();  // id
	}
	if( bOptimize )
		m_aPropQuest.Optimize();

	return TRUE;
}

const DWORD	dwDefault	= (DWORD)0xFFFFFFFF;
// propSkill.txt 로딩 이후에 해야한다.
BOOL CProject::LoadPropAddSkill( LPCTSTR lpszFileName )
{
	CScript script;
	if( script.Load( lpszFileName ) == FALSE )
		return FALSE;
	AddSkillProp propAddSkill;
	int i = script.GetNumber( TRUE ); // id
	while ( script.tok != FINISHED )
	{
		if( i == 0 )			// 이거 없으니까 파일뒤에 공백있는것도 줄줄히 읽더라
			break;
		propAddSkill.dwID = i;
		propAddSkill.dwName = script.GetNumber( TRUE );
		propAddSkill.dwSkillLvl = script.GetNumber( TRUE );
		propAddSkill.dwAbilityMin = script.GetNumber( TRUE );
		propAddSkill.dwAbilityMax = script.GetNumber( TRUE );
#if __VER >= 9	// __SKILL_0706
		propAddSkill.dwAbilityMinPVP	= script.GetNumber( TRUE );		// 대인 효과 최소
		propAddSkill.dwAbilityMaxPVP	= script.GetNumber( TRUE );		// 대인 효과 최대
#endif	// __SKILL_0706
		propAddSkill.dwAttackSpeed = script.GetNumber( TRUE );
		propAddSkill.dwDmgShift = script.GetNumber( TRUE );
		propAddSkill.nProbability = script.GetNumber( TRUE );
#if __VER >= 9	// __SKILL_0706
		propAddSkill.nProbabilityPVP	= script.GetNumber( TRUE );
#endif	// __SKILL_0706
		propAddSkill.dwTaunt = script.GetNumber( TRUE );
		propAddSkill.dwDestParam1 = script.GetNumber( TRUE );
		propAddSkill.dwDestParam2 = script.GetNumber( TRUE );
		propAddSkill.nAdjParamVal1 = script.GetNumber( TRUE );
		propAddSkill.nAdjParamVal2 = script.GetNumber( TRUE );
		propAddSkill.dwChgParamVal1 = script.GetNumber( TRUE );
		propAddSkill.dwChgParamVal2 = script.GetNumber( TRUE );
		propAddSkill.nDestData1[0] = script.GetNumber( TRUE );
		propAddSkill.nDestData1[1] = script.GetNumber( TRUE );
		propAddSkill.nDestData1[2] = script.GetNumber( TRUE );
		propAddSkill.dwActiveSkill = script.GetNumber( TRUE );
		propAddSkill.dwActiveSkillRate = script.GetNumber( TRUE );
#if __VER >= 9	// __SKILL_0706
		propAddSkill.dwActiveSkillRatePVP	= script.GetNumber( TRUE );
#endif	// __SKILL_0706
		propAddSkill.nReqMp = script.GetNumber( TRUE );
		propAddSkill.nReqFp = script.GetNumber( TRUE );
#if __VER >= 9	// __SKILL_0706
		propAddSkill.dwCooldown		= script.GetNumber( TRUE );
		propAddSkill.dwCastingTime	= script.GetNumber( TRUE );
#else	// __SKILL_0706
		propAddSkill.dwSkillReady	= script.GetNumber( TRUE );
#endif	// __SKILL_0706
		propAddSkill.dwSkillRange = script.GetNumber( TRUE );
		propAddSkill.dwCircleTime = script.GetNumber( TRUE );
		propAddSkill.dwPainTime  = script.GetNumber( TRUE );
		propAddSkill.dwSkillTime = script.GetNumber( TRUE );
		propAddSkill.nSkillCount = script.GetNumber( TRUE );
		propAddSkill.dwSkillExp = script.GetNumber( TRUE );
		
		propAddSkill.dwExp = script.GetNumber( TRUE );
		propAddSkill.dwComboSkillTime = script.GetNumber( TRUE );

#if __VER >= 9	// __SKILL_0706
		if( propAddSkill.dwAbilityMinPVP == dwDefault )
			propAddSkill.dwAbilityMinPVP	= propAddSkill.dwAbilityMin;
		if( propAddSkill.dwAbilityMaxPVP == dwDefault )
			propAddSkill.dwAbilityMaxPVP	= propAddSkill.dwAbilityMax;

		ItemProp* pSkillProp	= GetSkillProp( propAddSkill.dwName );
		if( pSkillProp )
		{
			if( propAddSkill.dwCooldown == dwDefault )
			{
				propAddSkill.dwCooldown	= pSkillProp->dwSkillReady;
				if( pSkillProp->nVer < 9 && propAddSkill.dwCastingTime != dwDefault )
					propAddSkill.dwCastingTime	/= 4;
			}
		}
		
		if( propAddSkill.nProbabilityPVP == dwDefault )
			propAddSkill.nProbabilityPVP	= propAddSkill.nProbability;
		if( propAddSkill.dwActiveSkillRatePVP == dwDefault )
			propAddSkill.dwActiveSkillRatePVP	= propAddSkill.dwActiveSkillRate;
#endif	// __SKILL_0706

		m_aPropAddSkill.SetAtGrow( i, &propAddSkill );

		// TRACE( "PropAddSkill : %d %d %d\r\n", i, propAddSkill.dwName, propAddSkill.dwSkillLvl );
		i = script.GetNumber( TRUE ); 
	}
	m_aPropAddSkill.Optimize();
	return TRUE;
}

BOOL CProject::LoadPropMoverEx( LPCTSTR szFileName )
{
	CScript script;
	int		nBlock = 0;
 
	if(script.Load(szFileName)==FALSE)
		return FALSE;
	CString string;
	int nVal = script.GetNumber(); // Mover Id

	do 
	{
		MoverProp* pProp = GetMoverProp( nVal );
		if(pProp == NULL)
		{
			TRACE("MoverAddProp에서 존재해야될 base Property %s가 없다. %s\n",script.Token);
			continue;
		}
		
		// 초기화
		pProp->m_nEvasionHP        = 0;
		pProp->m_nEvasionSec       = 0;
		pProp->m_nRunawayHP        = -1;
		pProp->m_nCallHP           = -1;
		pProp->m_nCallHelperMax    = 0;
		ZeroMemory( pProp->m_nCallHelperIdx  , sizeof( pProp->m_nCallHelperIdx   ) );
		ZeroMemory( pProp->m_nCallHelperNum  , sizeof( pProp->m_nCallHelperNum   ) );
		ZeroMemory( pProp->m_bCallHelperParty, sizeof( pProp->m_bCallHelperParty ) );

		int nCallNum = 0;

		pProp->m_nChangeTargetRand = 10;
		pProp->m_dwAttackMoveDelay = 0;
		pProp->m_bPartyAttack      = 0;
		pProp->m_dwRunawayDelay    = SEC( 1 );
		pProp->m_nAttackFirstRange = 9999;

		script.GetToken(); // {
		script.GetToken(); 
		while(*script.token != '}')
		{
			if(script.Token == ";") 
			{ 
				script.GetToken(); 
				continue; 
			} 
			if( strcmpi( script.Token, "AI" ) == 0 )		// AI블럭
			{
				BOOL bRet = LoadPropMoverEx_AI( szFileName, script, nVal );
				if ( bRet == FALSE )
				{
					Error( "%s : MoverID %s(%d)의 AI{} 블럭 읽기 실패", szFileName, pProp->szName, nVal );
					return FALSE;
				}
			}

			if(script.Token == "m_nAttackFirstRange")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackFirstRange  = script.GetNumber();

				if( pProp->m_nAttackFirstRange > 9999 || pProp->m_nAttackFirstRange <= 0 )
					Error( "LoadPropMoverEx::m_nAttackFirstRange 이상:%d\n", pProp->m_nAttackFirstRange );
			}
			else
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
				if( script.Token == "," )
				{
					script.GetNumber();
					script.GetToken(); // skip ,
					script.GetNumber();
					script.GetToken(); // skip )
				}
			}
			else
			if(script.Token == "SetCallHelper")
			{
				script.GetToken(); // skip (
				pProp->m_nCallHP = script.GetNumber();
				script.GetToken(); // skip ,
				pProp->m_nCallHelperIdx[ pProp->m_nCallHelperMax ] = script.GetNumber();
				script.GetToken(); // skip ,
				pProp->m_nCallHelperNum[ pProp->m_nCallHelperMax ] = script.GetNumber();
				script.GetToken(); // skip ,
				pProp->m_bCallHelperParty[ pProp->m_nCallHelperMax ] = script.GetNumber();
				script.GetToken(); // skip )
				pProp->m_nCallHelperMax++;
				if( pProp->m_nCallHelperMax > 5 )
				{
					char pszText[100];
					sprintf( pszText, "ID = %d: helper too many", pProp->dwID );
					AfxMessageBox( pszText );
				}
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
			if(script.Token == "m_dwAttackMoveDelay")
			{
				script.GetToken(); // skip =
				pProp->m_dwAttackMoveDelay = script.GetNumber();
			}
			else
			if(script.Token == "m_dwRunawayDelay")
			{
				script.GetToken(); // skip =
				pProp->m_dwRunawayDelay = script.GetNumber();
			}
			else
			if(script.Token == "randomItem")
				InterpretRandomItem( &pProp->m_randomItem, script );
			else 
			if( script.Token == "Maxitem" )
			{
				script.GetToken();	// =
				pProp->m_DropItemGenerator.m_dwMax	= script.GetNumber();
			}
			else 
			if( script.Token == "DropItem" )
			{
				DROPITEM di;
				memset( &di, 0, sizeof(DROPITEM) );
				di.dtType = DROPTYPE_NORMAL;
				script.GetToken();	// (
				di.dwIndex	= script.GetNumber();	// specific item index
				if( di.dwIndex == 0 )
					Error( "%s : %s가 defineItem.h에 정의되지 않았음", szFileName, script.token );
				ASSERT( di.dwIndex != 0 );
				script.GetToken();	// ,
				di.dwProbability	= script.GetNumber();	// probability
				script.GetToken();	// ,
				di.dwLevel	= script.GetNumber();	// level
				script.GetToken();	// ,
				di.dwNumber	= script.GetNumber();	// number
				script.GetToken();	// )
			#ifdef __WORLDSERVER
				pProp->m_DropItemGenerator.AddTail( di, pProp->szName );	// copy
			#endif
				di.dwNumber2 = 0;
			}
			else 
			if( script.Token == "DropKind" )
			{
				DROPKIND dropKind;
				script.GetToken();	// (
				dropKind.dwIK3	= script.GetNumber();	//	dwIK3
				script.GetToken();	// ,
				int a = script.GetNumber();
				script.GetToken();	// ,
				int b = script.GetNumber();
				dropKind.nMinUniq = (short)( pProp->dwLevel - 5 );		// 자기레벨보다 2~5레벨 낮은 템이 드랍되도록.
				dropKind.nMaxUniq = (short)( pProp->dwLevel - 2 );
				if( dropKind.nMinUniq < 1 )		
					dropKind.nMinUniq = 1;
				if( dropKind.nMaxUniq < 1 )
					dropKind.nMaxUniq = 1;
				script.GetToken();	// )
			#ifdef __WORLDSERVER
				pProp->m_DropKindGenerator.AddTail( dropKind );	// copy
			#endif
			}
			else
			if( script.Token == "DropGold" )
			{
				DROPITEM di;
				memset( &di, 0, sizeof(DROPITEM) );	// clear
				di.dtType = DROPTYPE_SEED;
				di.dwProbability = 0xFFFFFFFF;	// 무조건 나오게.
				script.GetToken();	// (
				di.dwNumber = script.GetNumber();	// gold min
				script.GetToken();	// ,
				di.dwNumber2 = script.GetNumber();	// gold max
				script.GetToken();	// )
			#ifdef __WORLDSERVER
				pProp->m_DropItemGenerator.AddTail( di, pProp->szName );	// copy
			#endif
			}
#if __VER >= 14 // __INSTANCE_DUNGEON
			else if( script.Token == "Transform" )
			{
				script.GetToken(); // (
				pProp->m_MonsterTransform.fHPRate = script.GetFloat();
				script.GetToken(); // ,
				pProp->m_MonsterTransform.dwMonsterId = script.GetNumber();
				script.GetToken(); // )
			}
#endif // __INSTANCE_DUNGEON
			script.GetToken();
		} 

		nVal = script.GetNumber(); // Mover Id
	} while(script.tok != FINISHED);

	return TRUE;
}

BOOL CProject::LoadCharacter( LPCTSTR szFileName )
{
		CScript script;
 
	if(script.Load(szFileName)==FALSE)
		return FALSE;

	CString strName;
	LPCHARACTER lpCharacter;
	script.GetToken(); // Mover name
	strName = script.Token;
	while( script.tok != FINISHED)
	{
		lpCharacter = new CHARACTER;
		lpCharacter->Clear();
		// 맵에 추가되는 것은 lowercase다. 그런데, 유일한 key 스트링이 lowercase로 맵에 추가되면
		// 원본 자체가 손상되는 결과구 그 리스트를 뽑아낼 때도 모두 lowercase가 되므로 원본을 m_szKey에
		// 보관해두는 것이다.
		_tcscpy( lpCharacter->m_szKey, strName );
		strName.MakeLower();

#if __VER >= 11 // __CSC_VER11_3
		lpCharacter->m_nVenderType = 0;
#endif //__CSC_VER11_3

#ifdef __CHIPI_DYO
		lpCharacter->bOutput = TRUE;
#endif // __CHIPI_DYO

#ifdef _DEBUG
		if( m_mapCharacter.Lookup( strName, (void*&)lpCharacter ) )
		{
			Error( "%s에서 동일한 %s 발견", szFileName, strName );
		}
#endif
		m_mapCharacter.SetAt( strName, lpCharacter);
		script.GetToken(); // {

		int nBlock = 1;
		while( nBlock && script.tok != FINISHED )
		{ 
			script.GetToken();
			if( script.Token == "{" )			
				nBlock++;
			else
			if( script.Token == "}" )			
				nBlock--;
			else
			if(script.Token == "randomItem")
			{
				InterpretRandomItem(&lpCharacter->m_randomItem,script);
			}
			else
			if(script.Token == "SetEquip")
			{
				lpCharacter->m_nEquipNum = 0;
				ZeroMemory( lpCharacter->m_adwEquip, sizeof( lpCharacter->m_adwEquip ) );
				script.GetToken(); // (
				while( *script.token != ')' )
				{
					DWORD dwEquip = script.GetNumber();
					if( lpCharacter->m_nEquipNum < MAX_HUMAN_PARTS )
						lpCharacter->m_adwEquip[ lpCharacter->m_nEquipNum++ ] = dwEquip;
					script.GetToken(); // ,
				}
			}
			else
			if(script.Token == "m_szName")
			{
				script.GetToken(); // =
				script.GetToken(); // name
				lpCharacter->m_strName = script.Token; 
			}
			else
			if(script.Token == "SetName")
			{ 
				script.GetToken(); // (
				lpCharacter->m_strName = GetLangScript( script );
				if( strlen( lpCharacter->m_strName ) == 0 )
				{
					Error( _T( "Error SetName m_szName Not Data") );
				}				
			}
			else
			if( script.Token == "SetFigure" )
			{
				script.GetToken(); // (
				lpCharacter->m_dwMoverIdx = script.GetNumber();
				script.GetToken(); // ,
				lpCharacter->m_dwHairMesh = script.GetNumber();
				script.GetToken(); // ,
				lpCharacter->m_dwHairColor = script.GetNumber();
				script.GetToken(); // ,
				lpCharacter->m_dwHeadMesh = script.GetNumber();
			}

			else
			if( script.Token == "SetMusic" )
			{
				script.GetToken(); // (
				lpCharacter->m_dwMusicId = script.GetNumber();
			}			
			else
			if(script.Token == "m_nStructure")
			{
				script.GetToken();
				lpCharacter->m_nStructure = script.GetNumber();
			}
			else
			if(script.Token == "m_szChar") // "SetImage"랑 같다 차후 삭제 예정 
			{ 
				script.GetToken();
				CString filename = GetLangScript( script );
				strcpy( lpCharacter->m_szChar, filename.GetBuffer(0) );
			}
			else
			if(script.Token == "m_szDialog")
			{
				script.GetToken();
				script.GetToken();
				strcpy( lpCharacter->m_szDialog, script.Token );
			}
			else
			if(script.Token == "m_szDlgQuest")
			{
				script.GetToken();
				script.GetToken();
				strcpy( lpCharacter->m_szDlgQuest, script.Token );
			}
			else
			if(script.Token == "SetImage" )
			{ 
				script.GetToken();
				CString filename = GetLangScript( script );
				strcpy( lpCharacter->m_szChar, filename.GetBuffer(0) );
			}
			else if( script.Token == "AddMenuLang" )
			{
				script.GetToken(); // (
				int nLang	= script.GetNumber();
				script.GetToken(); // ,
#ifdef __NO_SUB_LANG
				int nSubLang = 0;
#else // __NO_SUB_LANG
				int nSubLang	= script.GetNumber();
				script.GetToken(); // ,
#endif // __NO_SUB_LANG
				int nMMI = script.GetNumber(); 
				script.GetToken(); // ) 
				if( ::GetLanguage() == nLang && ::GetSubLanguage() == nSubLang )
					lpCharacter->m_abMoverMenu[ nMMI ] = TRUE;
			}
			else
			if(script.Token == "AddMenu" )
			{
				script.GetToken(); // (
				int nMMI = script.GetNumber(); 
				script.GetToken(); // ) 

#if __VER >= 8 //__CSC_VER8_4
				if( ( GetLanguage() != LANG_TWN && GetLanguage() != LANG_HK ) || ( nMMI != MMI_BEAUTYSHOP && nMMI != MMI_BEAUTYSHOP_SKIN ) )
#else
				if( GetLanguage() != LANG_TWN || nMMI != MMI_BEAUTYSHOP )
#endif //__CSC_VER8_4
					lpCharacter->m_abMoverMenu[ nMMI ] = TRUE;
			}
			else
			if(script.Token == "AddVenderSlot" ) // "AddVendorSlot"랑 같다 삭제 예정 
			{
				script.GetToken(); // (
				int nSlot = script.GetNumber(); script.GetToken(); // 
				script.GetToken();
				lpCharacter->m_venderSlot[ nSlot ] = script.token;
				script.GetToken(); // 
			}
			else
			if(script.Token == "AddVendorSlot" )
			{
				script.GetToken(); // (
				int nSlot = script.GetNumber(); // slot
				script.GetToken(); // ,
				lpCharacter->m_venderSlot[ nSlot ] = GetLangScript( script );
			}
#ifdef __RULE_0615
			// 국가별 판매 아이템 등록
			else if( script.Token == "AddVendorSlotLang" )
			{
				script.GetToken(); // (
				int nLang	= script.GetNumber();	// lang
				script.GetToken();	//,
#ifdef __NO_SUB_LANG
				int nSubLang = 0;
#else // __NO_SUB_LANG
				int nSubLang	= script.GetNumber();
				script.GetToken(); // ,
#endif // __NO_SUB_LANG
				int nSlot = script.GetNumber(); // slot
				script.GetToken(); // ,
				CString strSlot	= GetLangScript( script );
				if( nLang == ::GetLanguage() && nSubLang == ::GetSubLanguage() )
					lpCharacter->m_venderSlot[ nSlot ] = strSlot;
			}
			else if( script.Token == "AddVendorItemLang" )
			{
				script.GetToken();	//(
				int nLang	= script.GetNumber();
				script.GetToken();	//,
#ifdef __NO_SUB_LANG
				int nSubLang = 0;
#else // __NO_SUB_LANG
				int nSubLang	= script.GetNumber();
				script.GetToken(); // ,
#endif // __NO_SUB_LANG
				int nSlot	= script.GetNumber(); script.GetToken(); // 
				int nItemKind3	= script.GetNumber(); script.GetToken(); // 
				int	nItemJob	= script.GetNumber(); script.GetToken();	//
				int nUniqueMin	= script.GetNumber(); script.GetToken(); // 
				int nUniqueMax	= script.GetNumber(); script.GetToken(); // 
				int nTotalNum	= script.GetNumber(); script.GetToken(); // 
				if( nLang == ::GetLanguage() && nSubLang == ::GetSubLanguage() )
				{
					LPVENDOR_ITEM pVendorItem	= new VENDOR_ITEM;
					pVendorItem->m_nItemkind3	= nItemKind3;
					pVendorItem->m_nItemJob		= nItemJob;
					pVendorItem->m_nUniqueMin	= nUniqueMin;
					pVendorItem->m_nUniqueMax	= nUniqueMax;
					pVendorItem->m_nTotalNum	= nTotalNum;
					lpCharacter->m_venderItemAry[ nSlot ].Add( pVendorItem );
				}
			}
#endif	// __RULE_0615
			else
			if( script.Token == "AddVenderItem" || script.Token == "AddVendorItem")
			{
				script.GetToken(); // (
				int nSlot      = script.GetNumber(); script.GetToken(); // 
				int nItemKind3  = script.GetNumber(); script.GetToken(); // 
				int	nItemJob	= script.GetNumber(); script.GetToken();	//
				int nUniqueMin = script.GetNumber(); script.GetToken(); // 
				int nUniqueMax = script.GetNumber(); script.GetToken(); // 
				int nTotalNum  = script.GetNumber(); script.GetToken(); // 

				LPVENDOR_ITEM pVendorItem = new VENDOR_ITEM;
				pVendorItem->m_nItemkind3  = nItemKind3;
				pVendorItem->m_nItemJob	= nItemJob;
				pVendorItem->m_nUniqueMin = nUniqueMin;
				pVendorItem->m_nUniqueMax = nUniqueMax;
				pVendorItem->m_nTotalNum  = nTotalNum;
				lpCharacter->m_venderItemAry[ nSlot ].Add( pVendorItem );
			}
#if __VER >= 11 // __CSC_VER11_3
			else if( script.Token == "AddVenderItem2" || script.Token == "AddVendorItem2")
			{
				script.GetToken(); // (
				int nSlot = script.GetNumber(); script.GetToken(); // 
				DWORD dwId = script.GetNumber(); script.GetToken(); //

				LPVENDOR_ITEM pVendorItem = new VENDOR_ITEM;
				pVendorItem->m_dwItemId = dwId;
				lpCharacter->m_venderItemAry2[ nSlot ].Add( pVendorItem );
			}
			else if( script.Token == "SetVenderType" )
			{
				script.GetToken();
				int nVenderType = script.GetNumber();
				lpCharacter->m_nVenderType = nVenderType;
				script.GetToken();
			}
#endif //__CSC_VER11_3
#ifdef __NPC_BUFF
			else if( script.Token == "SetBuffSkill" )
			{
				NPC_BUFF_SKILL NPCBuffSkill;
				script.GetToken(); // (
				NPCBuffSkill.dwSkillID = script.GetNumber(); script.GetToken(); //,
				NPCBuffSkill.dwSkillLV = script.GetNumber(); script.GetToken(); //,
				NPCBuffSkill.nMinPlayerLV = script.GetNumber(); script.GetToken(); //,
				NPCBuffSkill.nMaxPlayerLV = script.GetNumber(); script.GetToken(); //,
				NPCBuffSkill.dwSkillTime = script.GetNumber();
				script.GetToken(); // )
				lpCharacter->m_vecNPCBuffSkill.push_back( NPCBuffSkill );
			}
#endif // __NPC_BUFF
#ifdef __CHIPI_DYO
			else if( script.Token == "SetLang" )
			{
				script.GetToken(); // (
				DWORD wLang = script.GetNumber();
#ifdef __NO_SUB_LANG
				DWORD wSubLang = 0;
#else // __NO_SUB_LANG
				DWORD wSubLang = LANG_SUB_DEFAULT;
#endif // __NO_SUB_LANG				
				script.GetToken(); // , or )
				if( script.Token == "," )
					wSubLang = script.GetNumber();
				else
					script.GetToken(); // )

				lpCharacter->m_vecdwLanguage.push_back( MAKELONG( wSubLang, wLang ) );
			}
			else if( script.Token == "SetOutput" )
			{
				script.GetToken(); // (
				script.GetToken();
				script.Token.MakeUpper();
				if( script.Token == "FALSE" )
					lpCharacter->bOutput = FALSE;
				script.GetToken(); // )
			}
#endif // __CHIPI_DYO
#if __VER >= 15 // __TELEPORTER
			else if( script.Token == "AddTeleport" )
			{
				script.GetToken(); // (
				D3DXVECTOR3 vPos( 0.0f, 0.0f, 0.0f );
				vPos.x = script.GetFloat();
				script.GetToken(); // ,
				vPos.z = script.GetFloat();
				script.GetToken(); // )
				lpCharacter->m_vecTeleportPos.push_back( vPos );
			}
#endif // __TELEPORTER
		}
		script.GetToken(); // Mover name
		strName = script.Token;
	} 
	return TRUE;
}	

void CProject::InterpretRandomItem(LPRANDOM_ITEM pRandomItem,CScript& script)
{
	script.GetToken(); // {
	script.GetToken(); 
	while(*script.token != '}')
	{
		if(script.Token == ";") 
		{ 
			script.GetToken(); 
			continue; 
		} 
		script.GetToken();
	}
}

#if __VER < 11 // __SYS_PLAYER_DATA
u_long CProject::GetPlayerID( const CHAR* lpszPlayer )
{
	u_long idPlayer		= 0;
	STRING2ULONG::iterator i	= m_str2ulong.find( lpszPlayer );
	if( i != m_str2ulong.end() )
		idPlayer	= i->second;

	return idPlayer;
}

LPCSTR CProject::GetPlayerString( u_long idPlayer )
{
	LPCSTR pszPlayer	= NULL;
	ULONG2STRING::iterator i	= m_ulong2str.find( idPlayer );
	if( i != m_ulong2str.end() )
		pszPlayer	= i->second.data();

	return pszPlayer;
}

BOOL CProject::SetPlayerID( u_long idPlayer, const CHAR* lpszPlayer )
{
	if( GetPlayerString( idPlayer ) == NULL )
	{
		m_str2ulong.erase( lpszPlayer );	// 康	// 2006/09/18
		m_str2ulong.insert( STRING2ULONG::value_type( lpszPlayer, idPlayer ) );
		m_ulong2str.insert( ULONG2STRING::value_type( idPlayer, lpszPlayer ) );
		return TRUE;
	}
	ASSERT( 0 );
	return FALSE;
}

BOOL CProject::RemovePlayerID( u_long idPlayer )
{
	const char* lpszPlayer	= GetPlayerString( idPlayer );
	if( NULL != lpszPlayer )
	{
		m_str2ulong.erase( lpszPlayer );
		m_ulong2str.erase( idPlayer );
		return TRUE;
	}
	return FALSE;
}
#endif	// __SYS_PLAYER_DATA

BOOL CProject::LoadExpTable( LPCTSTR lpszFileName )
{	
	CScript script;
	if( script.Load(lpszFileName) == FALSE )
		return FALSE;

	DWORD dwVal;
	int i = 0;

	script.GetToken(); // subject or FINISHED
	while( script.tok != FINISHED )
	{
		i = 0;
		if( script.Token == _T( "expCharacter" ) )
		{
			EXPINTEGER nVal;
			ZeroMemory( m_aExpCharacter, sizeof( m_aExpCharacter ) );
			script.GetToken(); // { 
			nVal	= script.GetExpInteger();
			while( *script.token != '}' )
			{
				m_aExpCharacter[i].nExp1	= nVal;
				m_aExpCharacter[i].nExp2	= script.GetExpInteger();
				m_aExpCharacter[i].dwLPPoint	= script.GetNumber();
				m_aExpCharacter[i++].nLimitExp		= script.GetExpInteger();
				nVal	= script.GetExpInteger();
			}
		}
		else
		if( script.Token == _T( "expFlight" ) )
		{
			ZeroMemory( m_aFxpCharacter, sizeof( m_aFxpCharacter ) );
			script.GetToken(); // { 
			dwVal = script.GetNumber();
			while( *script.token != '}' )
			{
				m_aFxpCharacter[ i ].dwFxp = dwVal;
				m_aFxpCharacter[ i++ ].dwFxp2 = script.GetNumber();
				dwVal = script.GetNumber();
			}
		}
		else
		if( script.Token == _T( "expLPPoint" ) )
		{
			ZeroMemory( m_aExpLPPoint, sizeof( m_aExpLPPoint ) );
			script.GetToken(); // { 
			dwVal = script.GetNumber();
			while( *script.token != '}' )
			{
				m_aExpLPPoint[ i++ ] = dwVal;
				dwVal = script.GetNumber();
			}
		}
		else
		if( script.Token == _T( "expJobLevel" ) )
		{
			script.GetToken(); // { 
			dwVal = script.GetNumber();
			while( *script.token != '}' )
			{
				script.GetNumber();		// skip
				script.GetNumber();		// skip
			}
		}
#ifndef __S_ADD_RESTATE
		else
		if( script.Token == _T( "jobAbillity" ) )
		{
			ZeroMemory( m_aJobAbillity, sizeof( m_aJobAbillity ) );
			script.GetToken(); // { 
			dwJob = script.GetNumber();
			while( *script.token != '}' )
			{
				script.GetToken(); // { 
				dwVal = script.GetNumber();
				while( *script.token != '}' )
				{
					m_aJobAbillity[ i ].dwStr = dwVal;
					m_aJobAbillity[ i ].dwStrAdd = script.GetNumber();
					m_aJobAbillity[ i ].dwSta = script.GetNumber();
					m_aJobAbillity[ i ].dwStaAdd = script.GetNumber();
					m_aJobAbillity[ i ].dwDex = script.GetNumber();
					m_aJobAbillity[ i ].dwDexAdd = script.GetNumber();
					m_aJobAbillity[ i ].dwInt = script.GetNumber();
					m_aJobAbillity[ i++ ].dwIntAdd = script.GetNumber();

					dwVal = script.GetNumber();
				}
				dwJob = script.GetNumber();
			}
		}
#endif // __S_ADD_RESTATE
		else
		if( script.Token == _T( "expSkill" ) )
		{
			ZeroMemory( m_aExpSkill, sizeof( m_aExpSkill ) );
			script.GetToken(); // { 
			dwVal = script.GetNumber();
			while( *script.token != '}' )
			{
				m_aExpSkill[ i++ ] = dwVal;
				dwVal = script.GetNumber();
			}
		}
		else
		if( script.Token == _T( "attackStyle" ) )
		{
//			ZeroMemory( m_aAttackStyle, sizeof( m_aAttackStyle ) );
			script.GetToken(); // { 
			dwVal = script.GetNumber();
			while( *script.token != '}' )
			{
				script.GetNumber();
				script.GetNumber();
				dwVal = script.GetNumber();
			}
		}
		else
		if( script.Token == _T( "expParty" ) )
		{
			ZeroMemory( m_aExpParty, sizeof( m_aExpParty ) );
			script.GetToken(); // { 
			dwVal = script.GetNumber();
			while( *script.token != '}' )
			{
				m_aExpParty[ i ].Exp = dwVal;
				m_aExpParty[ i++ ].Point = script.GetNumber();
				dwVal = script.GetNumber();
			}
		}
		else
		if( script.Token == _T( "expPartySkill" ) )
		{
			ZeroMemory( m_aAddExpParty, sizeof( m_aAddExpParty ) );
			script.GetToken(); // { 
			dwVal = script.GetNumber();
			while( *script.token != '}' )
			{
				m_aAddExpParty[ i ].Exp = dwVal;
				m_aAddExpParty[ i++ ].Level = script.GetNumber();
				dwVal = script.GetNumber();
			}
		}
		else if( script.Token == _T( "expCompanyTest" ) )
		{
			CGuildTable::GetInstance().ReadBlock( script );
		}
		else if( script.Token == _T( "expDropLuck" ) )
		{
			memset( m_adwExpDropLuck, 0, sizeof(m_adwExpDropLuck) );
			script.GetToken();	// {
			DWORD* pdw	= &m_adwExpDropLuck[0][0];
			DWORD dwValue	= (DWORD)script.GetNumber();	//
			while( *script.token != '}' )
			{
				ASSERT( pdw <= &m_adwExpDropLuck[121][10] );
				*pdw	= dwValue;
				pdw++;
				dwValue	= script.GetNumber();
			}
		}
		else if( script.Token == _T( "Setitem" ) )
		{
			script.GetToken();	// {
			int	 nAbilityOption		= 0;
			int nVal	= script.GetNumber();
			while( *script.token != '}' )
			{
				m_aSetItemAvail[nAbilityOption].nHitRate	= nVal;
				m_aSetItemAvail[nAbilityOption].nBlock	= script.GetNumber();
				m_aSetItemAvail[nAbilityOption].nMaxHitPointRate	= script.GetNumber();
				m_aSetItemAvail[nAbilityOption].nAddMagic	= script.GetNumber();
				m_aSetItemAvail[nAbilityOption].nAdded	= script.GetNumber();
				nVal	= script.GetNumber();
				nAbilityOption++;
			}
		}
		else
		if( script.Token == _T( "expUpitem" ) )
		{
			ZeroMemory( m_aExpUpItem, sizeof( m_aExpUpItem ) );
			script.GetToken();		// {
			int i = 0, j = 0, nVal;
			while( 1 )
			{
				nVal = script.GetNumber();
				if( *script.token == '}' )	break;
				j = 0;
				m_aExpUpItem[ j++ ][ i ] = nVal;
				m_aExpUpItem[ j++ ][ i ] = script.GetNumber();
				m_aExpUpItem[ j++ ][ i ] = script.GetNumber();
				m_aExpUpItem[ j++ ][ i ] = script.GetNumber();
				m_aExpUpItem[ j++ ][ i ] = script.GetNumber();
				m_aExpUpItem[ j++ ][ i ] = script.GetNumber();
				
				i++;
			}

			if( j != 6 )
				Error( "CProject::LoadExpTable : expUpitem 블럭의 데이타 열개수가 맞지 않다. %d", j );
			if( i != 21 )
				Error( "CProject::LoadExpTable : expUpitem 블럭의 데이타 행개수가 맞지 않다. %d", j );
			

		} // expUpitem
#if __VER >= 15 // __GUILD_HOUSE
		else if( script.Token == _T( "RestExpFactorTable" ) )
		{
			int nMinLevel, nMaxLevel;
			float fFactor;
			script.GetToken(); // {
			while( 1 )
			{
				nMinLevel	= script.GetNumber();
				if( *script.token == '}' ) break;
				nMaxLevel	= script.GetNumber();
				fFactor		= script.GetFloat();
#ifdef __WORLDSERVER
				GuildHouseMng->AddRestExpFactorTable( nMinLevel, nMaxLevel, fFactor );
#endif // __WORLDSERVER
			}
		}
#endif // __GUILD_HOUSE

		script.GetToken(); // type name or }
	}
	return TRUE;
}

CCtrl* CProject::GetCtrl( OBJID objid )
{
	CCtrl* pCtrl;
	if( m_objmap.Lookup( objid, pCtrl ) )
		return pCtrl;
	return NULL;
}

#ifdef __CLIENT
CSfx* CProject::GetSfx( OBJID objid )
{
	CCtrl* pCtrl;
	if( m_objmap.Lookup( objid, pCtrl ) )
		return ( pCtrl->GetType() == OT_SFX ? (CSfx*)pCtrl : NULL );
	return NULL;
}
#endif

CItem* CProject::GetItem( OBJID objid )
{
	CCtrl* pCtrl;
	if( m_objmap.Lookup( objid, pCtrl ) )
		return ( pCtrl->GetType() == OT_ITEM ? (CItem*)pCtrl : NULL );
	return NULL;
}

CMover* CProject::GetMover( OBJID objid )
{
	CCtrl* pCtrl;
	if( m_objmap.Lookup( objid, pCtrl ) )
		return ( pCtrl->GetType() == OT_MOVER ? (CMover*)pCtrl : NULL );
	return NULL;
}

CShip* CProject::GetShip( OBJID objid )
{
	CCtrl* pCtrl;
	if( m_objmap.Lookup( objid, pCtrl ) )
		return ( pCtrl->GetType() == OT_SHIP ? (CShip*)pCtrl : NULL );
	return NULL;
}


#ifdef __WORLDSERVER

CUser* CProject::GetUser( OBJID objid )
{
	CCtrl* pCtrl;
	if( m_objmap.Lookup( objid, pCtrl ) )
		return ( ( pCtrl->GetType() == OT_MOVER && ( (CMover*)pCtrl )->IsPlayer() ) ? (CUser*)pCtrl : NULL );
	return NULL;
}
#endif	

ObjProp* CProject::GetProp( int nType, int nIndex )
{
	switch( nType )
	{
	case OT_CTRL: return m_aPropCtrl.GetAt( nIndex ); 
	case OT_ITEM: return m_aPropItem.GetAt( nIndex ); 
	case OT_MOVER: return GetMoverProp( nIndex ); 
	}
	return NULL;
}

// 프로퍼티 메모리를 읽기전용으로 변경한다.
void CProject::ProtectPropMover()
{
// 백엔드 시스템을 위해서 막아둔다.
//	DWORD dwOld;
//	::VirtualProtect( m_pPropMover, sizeof(MoverProp) * MAX_PROPMOVER, PAGE_READONLY, &dwOld );
}

#ifdef __S1108_BACK_END_SYSTEM
void CProject::AddMonsterProp( MONSTER_PROP MonsterProp )
{
	strcpy( m_aMonsterProp[ m_nMonsterPropSize ].szMonsterName, MonsterProp.szMonsterName );
	m_aMonsterProp[ m_nMonsterPropSize ].nHitPoint = MonsterProp.nHitPoint;
	m_aMonsterProp[ m_nMonsterPropSize ].nAttackPower = MonsterProp.nAttackPower;
	m_aMonsterProp[ m_nMonsterPropSize ].nDefence = MonsterProp.nDefence;
	m_aMonsterProp[ m_nMonsterPropSize ].nExp = MonsterProp.nExp;
	m_aMonsterProp[ m_nMonsterPropSize ].nItemDrop = MonsterProp.nItemDrop;
	m_aMonsterProp[ m_nMonsterPropSize ].nPenya = MonsterProp.nPenya;
	m_aAddProp[ m_nAddMonsterPropSize ] = m_aMonsterProp[ m_nMonsterPropSize ];
	++m_nMonsterPropSize;
	++m_nAddMonsterPropSize;
}

void CProject::RemoveMonsterProp( char* lpszMonsterName )
{
	BOOL bFind = FALSE;
	int nFindIndex = 0;
	for( int i = 0 ; i < m_nMonsterPropSize ; ++i )
	{
		if( strcmp( m_aMonsterProp[ i ].szMonsterName, lpszMonsterName ) == 0 )
		{
			nFindIndex = i;
			bFind = TRUE;
			break;
		}
	}
	
	if( bFind )
	{
		for( int i = 0 ; i < m_nMonsterPropSize - 1; ++ i )
		{
			m_aMonsterProp[i] = m_aMonsterProp[i + 1];					
		}
		strcpy( m_aRemoveProp[m_nRemoveMonsterPropSize], lpszMonsterName );
		++m_nRemoveMonsterPropSize;
		--m_nMonsterPropSize;
	}			
}
#endif // __S1108_BACK_END_SYSTEM

#ifdef __WORLDSERVER
BOOL CProject::LoadDropEvent( LPCTSTR lpszFileName )
{
	CScript s;
	if( s.Load( lpszFileName ) == FALSE )
		return FALSE;
	
	do {
		s.GetToken();
		if( s.Token == "DropItem" )
		{
			DROPITEM di;
			DWORD dwMinLevel, dwMaxLevel;
			memset( &di, 0, sizeof(DROPITEM) );
			di.dtType = DROPTYPE_NORMAL;
			s.GetToken();	// (
			di.dwIndex	= s.GetNumber();	// specific item index
			s.GetToken();	// ,
			di.dwProbability	= s.GetNumber();	// probability
			s.GetToken();	// ,
			di.dwLevel	= s.GetNumber();	// level
			s.GetToken();	// ,
			di.dwNumber	= s.GetNumber();	// number
			s.GetToken();	// ,
			dwMinLevel	= s.GetNumber();	// min
			s.GetToken();	// ,
			dwMaxLevel	= s.GetNumber();	// max
			s.GetToken();	// )

			set<DWORD>::iterator i2	= m_setExcept.find( di.dwIndex );
			if( i2 != m_setExcept.end() )
				continue;
			if( GetLanguage() != LANG_KOR )
			{
				if( di.dwIndex == II_GEN_SKILL_BUFFBREAKER )
					di.dwProbability	= (DWORD)( di.dwProbability	* 0.5 );
			}

			for( int i = 0; i < m_nMoverPropSize; i++ )
			{
				MoverProp* pProp	= m_pPropMover + i;
				if( pProp->dwID && pProp->dwLevel >= dwMinLevel && pProp->dwLevel <= dwMaxLevel	)
					pProp->m_DropItemGenerator.AddTail( di, pProp->szName );	// copy
			}
		}
	} while( s.tok != FINISHED );

	return TRUE;
}

BOOL CProject::SortDropItem( void )
{
	// DROPITEM의 dwProbability 내림차순으로 Sortting 
	for( int i = 0; i < m_nMoverPropSize; i++ )
	{
		MoverProp* pProp	= m_pPropMover + i;
		int nDropItemCount = pProp->m_DropItemGenerator.GetSize();

		DROPITEM *pDropItem0, *pDropItem1, *pDropItemTemp;		
		for( int i = 0 ; i < nDropItemCount - 1 ; ++i )
		{
			pDropItem0 = pProp->m_DropItemGenerator.GetAt( i, FALSE, 0.0f );
			for( int j = i + 1 ; j < nDropItemCount ; ++j )
			{
				pDropItem1 = pProp->m_DropItemGenerator.GetAt( j, FALSE, 0.0f );
				if( pDropItem0->dwProbability > pDropItem1->dwProbability )
				{
					pDropItemTemp = pDropItem0;
					pDropItem0 = pDropItem1;
					pDropItem1 = pDropItemTemp;
				}
			}
		}
	}
	return TRUE;
}
#endif	// __WORLDSERVER


#ifdef __WORLDSERVER
CGiftboxMan::CGiftboxMan()
{
#ifndef __STL_GIFTBOX_VECTOR
	m_nSize	= 0;
	memset( &m_giftbox, 0, sizeof(m_giftbox) );
#endif // __STL_GIFTBOX_VECTOR
	/*
#ifdef __GIFTBOX0213
	m_nQuery	= 0;
#endif	// __GIFTBOX0213
	*/
}

CGiftboxMan* CGiftboxMan::GetInstance( void )
{
	static CGiftboxMan sGiftboxMan;
	return &sGiftboxMan;
}

/*
#ifdef __GIFTBOX0213
BOOL CGiftboxMan::AddItem( DWORD dwGiftbox, DWORD dwItem, DWORD dwProbability, int nNum, BYTE nFlag, int nTotal )
#else	// __GIFTBOX0213
*/
BOOL CGiftboxMan::AddItem( DWORD dwGiftbox, DWORD dwItem, DWORD dwProbability, int nNum, BYTE nFlag, int nSpan, int nAbilityOption )
//#endif	// __GIFTBOX0213
{
	map<DWORD, int>::iterator i	= m_mapIdx.find( dwGiftbox );
	int nIdx1	= 0;
	if( i != m_mapIdx.end() )
	{
		nIdx1	= i->second;
	}
	else
	{
#ifdef __STL_GIFTBOX_VECTOR
		nIdx1	= m_vGiftBox.size();
		GIFTBOX giftBox;
		memset( &giftBox, 0, sizeof(GIFTBOX) );
		m_vGiftBox.push_back( giftBox );
#else // __STL_GIFTBOX_VECTOR
		nIdx1	= m_nSize++;
#endif // __STL_GIFTBOX_VECTOR
		m_mapIdx.insert( map<DWORD, int>::value_type( dwGiftbox, nIdx1 ) );
	}

#ifdef __STL_GIFTBOX_VECTOR
	m_vGiftBox[nIdx1].dwGiftbox	= dwGiftbox;
	int nIdx2	= m_vGiftBox[nIdx1].nSize++;
	m_vGiftBox[nIdx1].adwItem[nIdx2]	= dwItem;
	m_vGiftBox[nIdx1].anNum[nIdx2]	= nNum;
	m_vGiftBox[nIdx1].anFlag[nIdx2]	= nFlag;
	m_vGiftBox[nIdx1].anSpan[nIdx2]	= nSpan;
	m_vGiftBox[nIdx1].anAbilityOption[nIdx2]		= nAbilityOption;
	
	m_vGiftBox[nIdx1].nSum	+= dwProbability;
	m_vGiftBox[nIdx1].adwProbability[nIdx2]	= m_vGiftBox[nIdx1].nSum;
#else // __STL_GIFTBOX_VECTOR
	if( m_nSize >= MAX_GIFTBOX )
	{
		OutputDebugString( "TOO MANY GIFTBOX\n" );
		return FALSE;
	}

	m_giftbox[nIdx1].dwGiftbox	= dwGiftbox;
	int nIdx2	= m_giftbox[nIdx1].nSize++;
	m_giftbox[nIdx1].adwItem[nIdx2]	= dwItem;
	m_giftbox[nIdx1].anNum[nIdx2]	= nNum;
	m_giftbox[nIdx1].anFlag[nIdx2]	= nFlag;
	m_giftbox[nIdx1].anSpan[nIdx2]	= nSpan;
	m_giftbox[nIdx1].anAbilityOption[nIdx2]		= nAbilityOption;
	/*
#ifdef __GIFTBOX0213
	m_giftbox[nIdx1].anTotal[nIdx2]	= nTotal;
	if( nTotal > 0 )
		m_giftbox[nIdx1].bGlobal	= TRUE;
#endif	// __GIFTBOX0213
		*/
	m_giftbox[nIdx1].nSum	+= dwProbability;
	m_giftbox[nIdx1].adwProbability[nIdx2]	= m_giftbox[nIdx1].nSum;
#endif // __STL_GIFTBOX_VECTOR
	return TRUE;
}

/*
#ifdef __GIFTBOX0213
BOOL CGiftboxMan::Open( DWORD dwGiftbox, LPDWORD pdwItem, int* pnNum, u_long idPlayer, CItemElem* pItemElem )
#else	// __GIFTBOX0213
*/
BOOL CGiftboxMan::Open( DWORD dwGiftbox, PGIFTBOXRESULT pGiftboxResult )
//#endif	// __GIFTBOX0213
{
	DWORD dwRand	= xRandom( 1000000 );
	map<DWORD, int>::iterator i		= m_mapIdx.find( dwGiftbox );
	if( i == m_mapIdx.end() )
		return 0;
	int nIdx	= i->second;
#ifdef __STL_GIFTBOX_VECTOR
	PGIFTBOX pBox	= &m_vGiftBox[nIdx];
#else // __STL_GIFTBOX_VECTOR
	PGIFTBOX pBox	= &m_giftbox[nIdx];
#endif // __STL_GIFTBOX_VECTOR

	int low	= 0;
	for( int j = 0; j < pBox->nSize; j++ )
	{
		if( dwRand >= (DWORD)( low ) && dwRand < pBox->adwProbability[j] )
		{
			/*
#ifdef __GIFTBOX0213
			if( pBox->anTotal[j] > 0 )
			{
				Query( CDPAccountClient::GetInstance(), dwGiftbox, pBox->adwItem[j], pBox->anNum[j], idPlayer, pItemElem );
				return FALSE;
			}
#endif	// __GIFTBOX0213
			*/
//			*pdwItem	= pBox->adwItem[j];
//			*pnNum	= pBox->anNum[j];
//			*pnFlag	= pBox->anFlag[j];
//			*pnSpan	= pBox->anSpan[j];
			pGiftboxResult->dwItem	= pBox->adwItem[j];
			pGiftboxResult->nNum	= pBox->anNum[j];
			pGiftboxResult->nFlag	= pBox->anFlag[j];
			pGiftboxResult->nSpan	= pBox->anSpan[j];
			pGiftboxResult->nAbilityOption	= pBox->anAbilityOption[j];
			return TRUE;
		}
	}
	return FALSE;
}

/*
#ifdef __GIFTBOX0213
BOOL CGiftboxMan::OpenLowest( DWORD dwGiftbox, LPDWORD pdwItem, int* pnNum )
{
	map<DWORD, int>::iterator i		= m_mapIdx.find( dwGiftbox );
	if( i == m_mapIdx.end() )
		return FALSE;
	int nIdx	= i->second;
	PGIFTBOX pBox	= &m_giftbox[nIdx];

	int nLowest	= pBox->nSize - 1;
	if( nLowest < 0 )
		return FALSE;
	*pdwItem	= pBox->adwItem[nLowest];
	*pnNum	= pBox->anNum[nLowest];
	return TRUE;
}
#endif	// __GIFTBOX0213
*/

void CGiftboxMan::Verify( void )
{
#ifdef __STL_GIFTBOX_VECTOR
	for( DWORD i = 0; i < m_vGiftBox.size(); i++ )
	{
		TRACE( "GIFTBOX : %d, %d\n", m_vGiftBox[i].dwGiftbox, m_vGiftBox[i].nSum );
		m_vGiftBox[i].adwProbability[m_vGiftBox[i].nSize-1]	+= ( 1000000 - m_vGiftBox[i].nSum );
	}	
#else // __STL_GIFTBOX_VECTOR
	for( int i = 0; i < m_nSize; i++ )
	{
		TRACE( "GIFTBOX : %d, %d\n", m_giftbox[i].dwGiftbox, m_giftbox[i].nSum );
		m_giftbox[i].adwProbability[m_giftbox[i].nSize-1]	+= ( 1000000 - m_giftbox[i].nSum );
	}
#endif // __STL_GIFTBOX_VECTOR
}

BOOL CProject::LoadGiftbox( LPCTSTR lpszFileName )
{
	CScript s;
	if( s.Load( lpszFileName ) == FALSE )
		return FALSE;

	DWORD dwGiftbox, dwItem, dwProbability;
	int	nNum;
	BYTE	nFlag;
	int	nSpan;
	int	nAbilityOption;

	s.GetToken();	// subject or FINISHED
	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "GiftBox" ) )
		{
			dwGiftbox	= s.GetNumber();
			s.GetToken();	// {
			dwItem	= s.GetNumber();
			while( *s.token != '}' )
			{
				dwProbability	= s.GetNumber();
				nNum	= s.GetNumber();
				if( !CGiftboxMan::GetInstance()->AddItem( dwGiftbox, dwItem, dwProbability * 100, nNum ) )
					return FALSE;
				dwItem	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "GiftBox2" ) )
		{
			dwGiftbox	= s.GetNumber();
			s.GetToken();	// {
			dwItem	= s.GetNumber();
			while( *s.token != '}' )
			{
				dwProbability	= s.GetNumber();
				nNum	= s.GetNumber();
				if( !CGiftboxMan::GetInstance()->AddItem( dwGiftbox, dwItem, dwProbability, nNum ) )
					return FALSE;
				dwItem	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "GiftBox3" ) )
		{
			dwGiftbox	= s.GetNumber();
			s.GetToken();	// {
			dwItem	= s.GetNumber();
			while( *s.token != '}' )
			{
				dwProbability	= s.GetNumber();
				nNum	= s.GetNumber();
				nFlag	= s.GetNumber();
				if( !CGiftboxMan::GetInstance()->AddItem( dwGiftbox, dwItem, dwProbability * 100, nNum, nFlag ) )
					return FALSE;
				dwItem	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "GiftBox4" ) || s.Token == _T( "GiftBox5" ) )
		{
			DWORD dwPrecision	= ( s.Token == _T( "GiftBox4" )? 100: 10 );
			dwGiftbox	= s.GetNumber();
			s.GetToken();	// {
			dwItem	= s.GetNumber();
			while( *s.token != '}' )
			{
				dwProbability	= s.GetNumber();
				nNum	= s.GetNumber();
				nFlag	= s.GetNumber();
				nSpan	= s.GetNumber();	// 기간
				
				if( !CGiftboxMan::GetInstance()->AddItem( dwGiftbox, dwItem, dwProbability * dwPrecision, nNum, nFlag, nSpan ) )
					return FALSE;
				dwItem	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "GiftBox6" ) )
		{
			dwGiftbox	= s.GetNumber();
			s.GetToken();	// {
			dwItem	= s.GetNumber();
			while( *s.token != '}' )
			{
				dwProbability	= s.GetNumber();
				nNum	= s.GetNumber();
				nFlag	= s.GetNumber();
				nSpan	= s.GetNumber();	// 기간
				nAbilityOption	= s.GetNumber();	// +
				
				if( !CGiftboxMan::GetInstance()->AddItem( dwGiftbox, dwItem, dwProbability * 10, nNum, nFlag, nSpan, nAbilityOption ) )
					return FALSE;
				dwItem	= s.GetNumber();
			}
		}
		/*
#ifdef __GIFTBOX0213
		else if( s.Token == _T( "GlobalGiftbox" ) )
		{
			int nTotal;
			dwGiftbox	= s.GetNumber();
			s.GetToken();	// {
			dwItem	= s.GetNumber();
			while( *s.token != '}' )
			{
				dwProbability	= s.GetNumber();
				nNum	= s.GetNumber();
				nTotal	= s.GetNumber();

				if( !CGiftboxMan::GetInstance()->AddItem( dwGiftbox, dwItem, dwProbability, nNum, 0, nTotal ) )
					return FALSE;
				dwItem	= s.GetNumber();
			}
		}
#endif	// __GIFTBOX0213
		*/
		s.GetToken();
	}
	CGiftboxMan::GetInstance()->Verify();
	return TRUE;
}

/*
#ifdef __GIFTBOX0213
void CGiftboxMan::Upload( CDPMng* pdp )
{
	CAr ar;
	ar << PACKETTYPE_GLOBALGIFTBOX;
	u_long uOffset	= ar.GetOffset();
	int nSize	= 0;
	ar << nSize;
	for( int i = 0; i < m_nSize; i++ )
	{
		if( m_giftbox[i].bGlobal )
		{
			ar << m_giftbox[i].dwGiftbox;
			ar << m_giftbox[i].nSize;
			for( int j = 0; j < m_giftbox[i].nSize; j++ )
			{
				ar << m_giftbox[i].adwItem[j];
				ar << m_giftbox[i].anTotal[j];
			}
			nSize++;
		}
	}
	int nBufSize;
	LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );
	*(UNALIGNED int*)( lpBuf + uOffset )	= nSize;
	pdp->Send( lpBuf, nBufSize, DPID_SERVERPLAYER );
}

void CGiftboxMan::Query( CDPMng* pdp, DWORD dwGiftbox, DWORD dwItem, int nNum, u_long idPlayer, CItemElem* pItemElem )
{
	if( pItemElem->m_nQueryGiftbox > 0 )
		return;

	CAr ar;
	ar << PACKETTYPE_QUERYGLOBALGIFTBOX;
	pItemElem->m_nQueryGiftbox	= ++m_nQuery;
	ar << idPlayer << dwGiftbox << dwItem << nNum << pItemElem->m_dwObjId << pItemElem->m_nQueryGiftbox;
	int nBufSize;
	LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );
	pdp->Send( lpBuf, nBufSize, DPID_SERVERPLAYER );
}

void CGiftboxMan::Restore( CDPMng* pdp, DWORD dwGiftbox, DWORD dwItem )
{
	CAr ar;
	ar << PACKETTYPE_RESTOREGLOBALGIFTBOX;
	ar << dwGiftbox << dwItem;
	int nBufSize;
	LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );
	pdp->Send( lpBuf, nBufSize, DPID_SERVERPLAYER );
}
#endif	// __GIFTBOX0213
*/
#endif	// __WORLDSERVER

CPackItem::CPackItem()
{
	m_nSize	= 0;
	memset( &m_packitem, 0, sizeof(m_packitem) );
}

CPackItem* CPackItem::GetInstance( void )
{
	static CPackItem sPackItem;
	return &sPackItem;
}

BOOL CPackItem::AddItem( DWORD dwPackItem, DWORD dwItem, int nAbilityOption, int nNum )
{
	map<DWORD, int>::iterator i	= m_mapIdx.find( dwPackItem );
	int nIdx1	= 0;
	if( i != m_mapIdx.end() )
	{
		nIdx1	= i->second;
	}
	else
	{
		nIdx1	= m_nSize++;
		m_mapIdx.insert( map<DWORD, int>::value_type( dwPackItem, nIdx1 ) );
	}

	if( m_nSize >= MAX_PACKITEM )
	{
		OutputDebugString( "TOO MANY PACKITEM\n" );
		return FALSE;
	}

	if( m_packitem[nIdx1].nSize == MAX_ITEM_PER_PACK )
	{
		OutputDebugString( "TOO MANY ITEM PER PACK\n" );
		return FALSE;
	}

	m_packitem[nIdx1].dwPackItem	= dwPackItem;
	int nIdx2	= m_packitem[nIdx1].nSize++;
	m_packitem[nIdx1].adwItem[nIdx2]	= dwItem;
	m_packitem[nIdx1].anAbilityOption[nIdx2]	= nAbilityOption;
	m_packitem[nIdx1].anNum[nIdx2]	= nNum;
	return TRUE;
}

PPACKITEMELEM CPackItem::Open( DWORD dwPackItem )
{
	map<DWORD, int>::iterator i		= m_mapIdx.find( dwPackItem );
	if( i != m_mapIdx.end() )
		return &m_packitem[i->second];	
	return NULL;
}

BOOL CProject::LoadPackItem( LPCTSTR lpszFileName )
{
	CScript s;
	if( s.Load( lpszFileName ) == FALSE )
		return FALSE;

	DWORD dwPackItem, dwItem;
	int nAbilityOption, nNum;
	int	nSpan;

	char lpOutputString[100]	= { 0, };
	OutputDebugString( "packItem\n" );
	OutputDebugString( "----------------------------------------\n" );
	s.GetToken();	// subject or FINISHED
	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "PackItem" ) )
		{
			dwPackItem	= s.GetNumber();
			nSpan	= s.GetNumber();
			s.GetToken();	// {
			dwItem	= s.GetNumber();
			while( *s.token != '}' )
			{
				nAbilityOption	= s.GetNumber();
				nNum	= s.GetNumber();
				
				sprintf( lpOutputString, "%d\n", dwPackItem );
				OutputDebugString( lpOutputString );
				if( !CPackItem::GetInstance()->AddItem( dwPackItem, dwItem, nAbilityOption, nNum ) )
					return FALSE;
				dwItem	= s.GetNumber();
			}
			PPACKITEMELEM pPackItemElem	= CPackItem::GetInstance()->Open( dwPackItem );
			if( pPackItemElem )
				pPackItemElem->nSpan	= nSpan;
		}
		s.GetToken();
	}
	OutputDebugString( "----------------------------------------\n" );
	return TRUE;
}

PSETITEMAVAIL CProject::GetSetItemAvail( int nAbilityOption )
{
	if( nAbilityOption < 0 || nAbilityOption > 10 )
		return NULL;
	return &m_aSetItemAvail[nAbilityOption-1];
}

BOOL CProject::LoadPiercingAvail( LPCTSTR lpszFileName )
{
	CScript s;
	if( s.Load( lpszFileName ) == FALSE )
		return FALSE;

	DWORD dwItemId;
	int nDstParam, nAdjParam;

	s.GetToken();	// subject or FINISHED
	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "Piercing" ) )
		{
			dwItemId	= s.GetNumber();
			s.GetToken();	// {
			nDstParam	= s.GetNumber();
			while( *s.token != '}' )
			{
				nAdjParam	= s.GetNumber();
				if( !CPiercingAvail::GetInstance()->AddPiercingAvail( dwItemId, nDstParam, nAdjParam ) )
					return FALSE;
				nDstParam	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "SetItem" ) )
		{
			int nId;
			char pszString[MAX_SETITEM_STRING];
			nId		= s.GetNumber();

			s.GetToken();
			lstrcpy( pszString, s.Token );

			CSetItem* pSetItem	= new CSetItem( nId, pszString );

			s.GetToken();	// {
			s.GetToken();	// categori
			while( *s.token != '}' )
			{
				if( s.Token == _T( "Elem" ) )
				{
					s.GetToken();	// {
					DWORD dwItemId	= s.GetNumber();
					int nParts;
					while( *s.token != '}' )
					{
						nParts	= s.GetNumber();
						pSetItem->AddSetItemElem( dwItemId, nParts );
						dwItemId	= s.GetNumber();
					}
					s.GetToken();
				}
				else if( s.Token == _T( "Avail" ) )
				{
					s.GetToken();	// {
					int nDstParam	= s.GetNumber();
					while( *s.token != '}' )
					{
						int nAdjParam	= s.GetNumber();
						int nEquiped	= s.GetNumber();
						pSetItem->AddItemAvail( nDstParam, nAdjParam, nEquiped );
						nDstParam	= s.GetNumber();
					}
					s.GetToken();
				}
			}
			pSetItem->SortItemAvail();
			CSetItemFinder::GetInstance()->AddSetItem( pSetItem );
		}

		else if( s.Token == _T( "RandomOptItem" ) )
		{
			RANDOMOPTITEM randomOptItem;
			randomOptItem.nId	= s.GetNumber();
			s.GetToken();
			lstrcpy( randomOptItem.pszString, s.Token );
			randomOptItem.nLevel	= s.GetNumber();
			randomOptItem.dwProbability		= s.GetNumber();
			s.GetToken();	// {
			int nDstParam	= s.GetNumber();
			while( *s.token != '}' )
			{
				int nAdjParam	= s.GetNumber();
				randomOptItem.ia.anDstParam[randomOptItem.ia.nSize]		= nDstParam;
				randomOptItem.ia.anAdjParam[randomOptItem.ia.nSize]		= nAdjParam;
				randomOptItem.ia.nSize++;
				nDstParam	= s.GetNumber();
			}
			CRandomOptItemGen::GetInstance()->AddRandomOption( &randomOptItem );
		}
		s.GetToken();
	}
	CRandomOptItemGen::GetInstance()->Arrange();

	return TRUE;
}

CPiercingAvail::CPiercingAvail()
{
	m_nSize		= 0;
	memset( &m_pPiercingAvail, 0, sizeof(m_pPiercingAvail) );
}

CPiercingAvail::~CPiercingAvail()
{
}

CPiercingAvail* CPiercingAvail::GetInstance( void )
{
	static CPiercingAvail sPiercingAvail;
	return &sPiercingAvail;
}

BOOL CPiercingAvail::AddPiercingAvail( DWORD dwItemId, int nDstParam, int nAdjParam )
{
	map<DWORD, int>::iterator i	= m_mapIdx.find( dwItemId );
	int nIdx1	= 0;
	if( i != m_mapIdx.end() )
	{
		nIdx1	= i->second;
	}
	else
	{
		nIdx1	= m_nSize++;
		m_mapIdx.insert( map<DWORD, int>::value_type( dwItemId, nIdx1 ) );
	}

	if( m_nSize >= MAX_PIERCING_MATERIAL )
		return FALSE;

	if( m_pPiercingAvail[nIdx1].nSize == MAX_PIERCING_DSTPARAM )
		return FALSE;

	m_pPiercingAvail[nIdx1].dwItemId	= dwItemId;
	int nIdx2	= m_pPiercingAvail[nIdx1].nSize++;
	m_pPiercingAvail[nIdx1].anDstParam[nIdx2]	= nDstParam;
	m_pPiercingAvail[nIdx1].anAdjParam[nIdx2]	= nAdjParam;

	return TRUE;
}

PPIERCINGAVAIL CPiercingAvail::GetPiercingAvail( DWORD dwItemId  )
{
	map<DWORD, int>::iterator i	= m_mapIdx.find( dwItemId );
	if( i != m_mapIdx.end() )
		return &m_pPiercingAvail[i->second];
	return NULL;
}

CSetItem::CSetItem( int nId, const char* pszString )
{
	m_nId	= nId;
	lstrcpy( m_pszString, pszString );
	m_nElemSize		= 0;
}

BOOL CSetItem::AddSetItemElem( DWORD dwItemId, int nParts )
{
	if( m_nElemSize == MAX_SETITEM_ELEM )
	{
		TRACE( "too many setitem elements\n" );
		return FALSE;
	}
#ifdef _DEBUG
	for( int i = 0; i < m_nElemSize; i++ )
	{
		if( m_anParts[i] == nParts )
		{
			TRACE( "parts duplicated\n" );
			return FALSE;
		}
	}
#endif	// _DEBUG
	m_adwItemId[m_nElemSize]	= dwItemId;
	m_anParts[m_nElemSize]		= nParts;
	m_nElemSize++;
	return TRUE;
}

BOOL CSetItem::AddItemAvail( int nDstParam, int nAdjParam, int nEquiped )
{
	if( m_avail.nSize == MAX_ITEMAVAIL )
	{
		TRACE( "too many setitem avail\n" );
		return FALSE;
	}
	m_avail.anDstParam[m_avail.nSize]	= nDstParam;
	m_avail.anAdjParam[m_avail.nSize]	= nAdjParam;
	m_avail.anEquiped[m_avail.nSize]	= nEquiped;
	m_avail.nSize++;
	return TRUE;
}

void CSetItem::SortItemAvail( void )
{
	for( int i = 0; i < m_avail.nSize-1; i++ )
	{
		for( int j = i+1; j < m_avail.nSize; j++ )
		{
			if( m_avail.anEquiped[i] > m_avail.anEquiped[j] )
			{
				int nDstParam, nAdjParam, nEquiped;
				
				nDstParam	= m_avail.anDstParam[i];
				nAdjParam	= m_avail.anAdjParam[i];
				nEquiped	= m_avail.anEquiped[i];
				m_avail.anDstParam[i]	= m_avail.anDstParam[j];
				m_avail.anAdjParam[i]	= m_avail.anAdjParam[j];
				m_avail.anEquiped[i]	= m_avail.anEquiped[j];
				m_avail.anDstParam[j]	= nDstParam;
				m_avail.anAdjParam[j]	= nAdjParam;
				m_avail.anEquiped[j]	= nEquiped;
			}
		}
	}
}

void CSetItem::GetItemAvail( PITEMAVAIL pItemAvail, int nEquiped, BOOL bAll )
{
	for( int i = 0; i < m_avail.nSize; i++ )
	{
		if( m_avail.anEquiped[i] > nEquiped )
			return;

		if( !bAll && m_avail.anEquiped[i] != nEquiped )
			continue;

		int nFind	= -1;
		for( int j = 0; j < pItemAvail->nSize; j++ )
		{
			if( pItemAvail->anDstParam[j] == m_avail.anDstParam[i] )
			{
				nFind	= j;
				break;
			}
		}
		if( nFind < 0 )
			nFind	= pItemAvail->nSize++;
		pItemAvail->anDstParam[nFind]	= m_avail.anDstParam[i];
		pItemAvail->anAdjParam[nFind]	+= m_avail.anAdjParam[i];
	}
}

void CSetItemFinder::Free()
{
	for( map<int, CSetItem*>::iterator i	= m_mapSetId.begin(); i != m_mapSetId.end(); ++i )
	{
		CSetItem* pSetItem	= i->second;
		SAFE_DELETE( pSetItem );
	}
	m_mapSetId.clear();
	m_mapItemId.clear();
}

CSetItemFinder*	CSetItemFinder::GetInstance( void )
{
	static	CSetItemFinder	sSetItemFinder;
	return &sSetItemFinder;
}

void CSetItemFinder::AddSetItem( CSetItem* pSetItem )
{
	bool	bResult	= m_mapSetId.insert( map<int, CSetItem*>::value_type( pSetItem->m_nId, pSetItem ) ).second;
	if( !bResult )
	{
		TRACE( "adding setitem failed\t// 0\n" );
	}
	for( int i = 0; i < pSetItem->m_nElemSize; i++ )
	{
		bResult	= m_mapItemId.insert( map<DWORD, CSetItem*>::value_type( pSetItem->m_adwItemId[i], pSetItem ) ).second;
		if( !bResult )
		{
			TRACE( "adding setitem failed\t// 1\n" );
		}
	}
}

CSetItem* CSetItemFinder::GetSetItem( int nSetItemId )
{
	map<int, CSetItem*>::iterator i	= m_mapSetId.find( nSetItemId );
	if( i != m_mapSetId.end() )
		return i->second;
	return NULL;
}

CSetItem* CSetItemFinder::GetSetItemByItemId( DWORD dwItemId )
{
	map<DWORD, CSetItem*>::iterator i	= m_mapItemId.find( dwItemId );
	if( i != m_mapItemId.end() )
		return i->second;
	return NULL;
}	

CRandomOptItemGen::CRandomOptItemGen()
{
	int m_nSize		= 0;
	memset( m_aRandomOptItem, 0, sizeof(m_aRandomOptItem) );
	memset( m_anIndex, 0, sizeof(m_anIndex) );
}

void CRandomOptItemGen::Free( void )
{
	m_mapid.clear();
}

BOOL CRandomOptItemGen::AddRandomOption( PRANDOMOPTITEM pRandomOptItem )
{
	if( m_nSize == MAX_RANDOMOPTITEM )
	{
		TRACE( "adding randomoptitem\t// 1\n" );
		return FALSE;
	}
	memcpy( &m_aRandomOptItem[m_nSize], pRandomOptItem, sizeof(RANDOMOPTITEM) );
	m_nSize++;
	return TRUE;
}

CRandomOptItemGen* CRandomOptItemGen::GetInstance( void )
{
	static	CRandomOptItemGen sRandomOptItemGen;
	return &sRandomOptItemGen;
}

void CRandomOptItemGen::Arrange( void )
{
	// sort
	for( int i = 0; i < m_nSize-1; i++ )
	{
		for( int j = i + 1; j < m_nSize; j++ )
		{
			if( m_aRandomOptItem[i].nLevel > m_aRandomOptItem[j].nLevel )
			{
				RANDOMOPTITEM tmp;
				memcpy( &tmp, &m_aRandomOptItem[i], sizeof(RANDOMOPTITEM) );
				memcpy( &m_aRandomOptItem[i], &m_aRandomOptItem[j], sizeof(RANDOMOPTITEM) );
				memcpy( &m_aRandomOptItem[j], &tmp, sizeof(RANDOMOPTITEM) );
			}
		}
	}
	// make index & make lv index
	int nLevel	= 1, nPrevious	= -1;
	for( i = 0; i  < m_nSize; i++ )
	{
		bool bResult	= m_mapid.insert( map<int, int>::value_type( m_aRandomOptItem[i].nId, i ) ).second;
		if( !bResult )
		{
			TRACE( "adding randomoptitem failed\t// 0\n" );
		}
		
		if( m_aRandomOptItem[i].nLevel > nLevel )
		{
			for( int j = nLevel; j < m_aRandomOptItem[i].nLevel; j++ )
				m_anIndex[j-1]	= nPrevious;
			nLevel	= m_aRandomOptItem[i].nLevel;
		}
		nPrevious	= i;
	}

#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans

#if __VER >= 15 // __HERO129_VER15				// 15차 히어로 레벨확장
	
	for( i = nLevel; i <= MAX_MONSTER_LEVEL; i++ )
		m_anIndex[i-1]	= nPrevious;
	#else	// 15차 히어로 레벨확장
	for( i = nLevel; i <= MAX_LEGEND_LEVEL; i++ )
		m_anIndex[i-1]	= nPrevious;
	#endif	// 15차 히어로 레벨확장

#else //__LEGEND	//	10차 전승시스템	Neuz, World, Trans
	for( i = nLevel; i <= MAX_GENERAL_LEVEL; i++ )
		m_anIndex[i-1]	= nPrevious;
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans
}

int CRandomOptItemGen::GenRandomOptItem( int nLevel, FLOAT fPenalty, ItemProp* pItemProp, DWORD dwClass )
{
	if( !pItemProp )
		return 0;
	
	if( pItemProp->dwItemKind1 != IK1_WEAPON && pItemProp->dwItemKind1 != IK1_ARMOR )
		return 0;

#if __VER >= 15 // __HERO129_VER15				// 15차 히어로 레벨확장
	if( nLevel >= MAX_MONSTER_LEVEL )
		nLevel = MAX_MONSTER_LEVEL - 1 ;
#endif	// 15차 히어로 레벨확장

	int i	= m_anIndex[nLevel];

	if( i != -1 )
	{
		int nIndex	= xRandom( i + 1 );
		DWORD dwRandom	= xRandom( 3000000000 );
		if( dwClass == RANK_MIDBOSS )
			dwRandom	/= 5;	// 준 보스 몬스터 랜덤 옵션 확률 500%
		DWORD dwProbability	= (DWORD)(m_aRandomOptItem[nIndex].dwProbability * fPenalty);
		if( dwRandom < dwProbability )
			return m_aRandomOptItem[nIndex].nId;
	}
	return 0;
}

PRANDOMOPTITEM CRandomOptItemGen::GetRandomOptItem( int nId )
{
	map<int, int>::iterator i	= m_mapid.find( nId );
	if( i != m_mapid.end() )
		return &m_aRandomOptItem[i->second];
	return NULL;
}

const char* CRandomOptItemGen::GetRandomOptItemString( int nId )
{
	PRANDOMOPTITEM pRandomOptItem	= GetRandomOptItem( nId );
	if( pRandomOptItem )
		return pRandomOptItem->pszString;
	return NULL;
}

BOOL CProject::IsGuildQuestRegion( const D3DXVECTOR3 & vPos )
{
	CRect rect;
	POINT point;
	for( int i = 0; i < m_aPropGuildQuest.GetSize(); i++ )
	{
		GUILDQUESTPROP* pProp	= m_aPropGuildQuest.GetAt( i );
		if( pProp == NULL )
			continue;
		
		rect.SetRect( pProp->x1, pProp->y2, pProp->x2, pProp->y1 );
		point.x = (int)vPos.x;
		point.y = (int)vPos.z;
		if( rect.PtInRect( point ) )
			return TRUE;
	}
	return FALSE;
}


//LoadPropItem를 호출하고 후처리를 한다.
void CProject::OnAfterLoadPropItem()
{
	for( int i = 0; i < m_aPropItem.GetSize(); i++ )
	{
		ItemProp* pItemProp = (ItemProp*)m_aPropItem.GetAt( i );
		if( pItemProp && pItemProp->dwFlag == NULL_ID )
			pItemProp->dwFlag = 0;

		if( pItemProp && pItemProp->dwItemKind3 != NULL_ID )
		{
			m_itemKindAry[ pItemProp->dwItemKind3 ].Add( pItemProp );

			switch( pItemProp->dwItemKind3 )
			{
			case IK3_EVENTMAIN:
			case IK3_BINDS:
				pItemProp->dwFlag |= IP_FLAG_BINDS;
				break;
			}
		}
	}


	ItemProp* ptmp;
	for( i = 0; i < MAX_ITEM_KIND3; i++ )
	{
		// sort
		for( int j = 0; j < m_itemKindAry[i].GetSize() - 1; j++ )
		{
			for( int k = j + 1; k < m_itemKindAry[i].GetSize(); k++ )
			{
				if( ( (ItemProp*)m_itemKindAry[i].GetAt( k ) )->dwItemRare < ( (ItemProp*)m_itemKindAry[i].GetAt( j ) )->dwItemRare )
				{
					ptmp	= (ItemProp*)m_itemKindAry[i].GetAt( j );
					m_itemKindAry[i].SetAt( j, (void*)m_itemKindAry[i].GetAt( k ) );
					m_itemKindAry[i].SetAt( k, (void*)ptmp );
				}
			}
		}
		//
		DWORD dwItemRare	= (DWORD)-1;
		for( j = 0; j < m_itemKindAry[i].GetSize(); j++ )
		{
			if( dwItemRare != ( (ItemProp*)m_itemKindAry[i].GetAt( j ) )->dwItemRare )
			{
				dwItemRare	= ( (ItemProp*)m_itemKindAry[i].GetAt( j ) )->dwItemRare;
				if( dwItemRare != (DWORD)-1 )
				{
					m_minMaxIdxAry[i][dwItemRare].cx	= j;
					m_minMaxIdxAry[i][dwItemRare].cy	= j;
				}
			}
			else
			{
				if( dwItemRare != (DWORD)-1 )
					m_minMaxIdxAry[i][dwItemRare].cy	= j;
			}
		}
	}

	LoadExcept( "except.txt" );
}


BOOL CProject::LoadExcept( LPCTSTR lpszFilename )
{
	CScript s;
	if( s.Load( lpszFilename ) == FALSE )
		return FALSE;
	
	s.GetToken();	// subject or FINISHED
	while( s.tok != FINISHED )
	{
		int nLang	= atoi( s.token );
		int nSubLang	= s.GetNumber();	//

		s.GetToken();	// {
		s.GetToken();	// itemProp & moverProp
		while( *s.token != '}' )
		{
			if( s.Token == "ItemProp" )
			{
				s.GetToken();	// {
				s.GetToken();	// dwID
				while( *s.token != '}' )
				{
					DWORD dwID	= atoi( s.token );
					ItemProp* pItemProp	= GetItemProp( dwID );
					s.GetToken();	// {
					s.GetToken();	// attribute
					while( *s.token != '}' )
					{
						if( GetLanguage() != nLang || GetSubLanguage() != nSubLang )
						{
							s.GetToken();
							continue;
						}
						if( s.Token == "fFlightSpeed" )
						{
							s.GetToken();	// =
							pItemProp->fFlightSpeed	= s.GetFloat();
							s.GetToken();	// ;
						}
						else if( s.Token == "dwShopAble" )
						{
							s.GetToken();	// =
							pItemProp->dwShopAble	= s.GetNumber();
							s.GetToken();	// ;
						}
						else if( s.Token == "dwCircleTime" )
						{
							s.GetToken();	// =
							pItemProp->dwCircleTime	= s.GetNumber();
							s.GetToken();	// ;
						}
						else if( s.Token == "dwFlag" )
						{
							s.GetToken();	// =
							pItemProp->dwFlag	= s.GetNumber();
							s.GetToken();	// ;
						}
#ifdef __JEFF_9_20
						else if( s.Token == "dwLimitLevel1" )
						{
							s.GetToken();	// =
							pItemProp->dwLimitLevel1	= s.GetNumber();
							s.GetToken();	// ;
						}
#endif	// __JEFF_9_20
						else if( s.Token == "dwSkillReadyType" )
						{	// 금란지교
							s.GetToken();	// =
							pItemProp->dwSkillReadyType		= s.GetNumber();
							s.GetToken();	// ;
						}
						else if( s.Token == "worldDrop" )
						{
							s.GetToken();	// =
							BOOL b	= (BOOL)s.GetNumber();
							if( !b )
								m_setExcept.insert( dwID );	
							s.GetToken();	// ;
						}
						s.GetToken();	// attribute
					}
					s.GetToken();	// dwID
				}
			}
			s.GetToken();	// ItemProp OR moverProp
		}
		s.GetToken();	// lang
	}
	return TRUE;
}

JobProp* CProject::GetJobProp( int nIndex )
{
	if( nIndex < 0 || nIndex >= MAX_JOB )
	{
		LPCTSTR szErr = Error( "CProject::GetJobProp 범위침범. %d", nIndex );
		ADDERRORMSG( szErr );
		return NULL;
	}
	return &m_aPropJob[nIndex];
}

#if __VER >= 9 // __Y_ADV_ENCHANT_EFFECT
BOOL CProject::LoadPropEnchant( LPCTSTR lpszFileName )
{
	CScript scanner;
	if( scanner.Load( lpszFileName, FALSE ) == FALSE )
	{
#ifdef __CLIENT
		MessageBox( g_Neuz.GetSafeHwnd(), "프로퍼티 읽기실패 : Enchant", "프리프", MB_OK );
		ADDERRORMSG( "프로퍼티 읽기실패 : Enchant" );
#endif //__CLIENT
		return FALSE;
	}
	
	int nKind = 0;
	scanner.GetToken();
	while( scanner.tok != FINISHED )
	{
		if( scanner.Token == _T( "NORMAL" ) )
		{
			m_nEnchantLimitLevel[0] = scanner.GetNumber();
		}
		else if( scanner.Token == _T( "AL" ) )
		{
			m_nEnchantLimitLevel[1] = scanner.GetNumber();
		}
		else if( scanner.Token == _T( "NONE" ) )
		{
			m_nEnchantLimitLevel[2] = scanner.GetNumber();
		}
		else if( scanner.Token == _T( "NORMAL_SCAL" ) )
		{
			m_fEnchantLevelScal[0][0] = scanner.GetFloat();
			m_fEnchantLevelScal[0][1] = scanner.GetFloat();
			m_fEnchantLevelScal[0][2] = scanner.GetFloat();
			m_fEnchantLevelScal[0][3] = scanner.GetFloat();
			m_fEnchantLevelScal[0][4] = scanner.GetFloat();
			m_fEnchantLevelScal[0][5] = scanner.GetFloat();
			m_fEnchantLevelScal[0][6] = scanner.GetFloat();
			m_fEnchantLevelScal[0][7] = scanner.GetFloat();
			m_fEnchantLevelScal[0][8] = scanner.GetFloat();
			m_fEnchantLevelScal[0][9] = scanner.GetFloat();
		}
		else if( scanner.Token == _T( "ALL_SCAL" ) )
		{
			m_fEnchantLevelScal[1][0] = scanner.GetFloat();
			m_fEnchantLevelScal[1][1] = scanner.GetFloat();
			m_fEnchantLevelScal[1][2] = scanner.GetFloat();
			m_fEnchantLevelScal[1][3] = scanner.GetFloat();
			m_fEnchantLevelScal[1][4] = scanner.GetFloat();
			m_fEnchantLevelScal[1][5] = scanner.GetFloat();
			m_fEnchantLevelScal[1][6] = scanner.GetFloat();
			m_fEnchantLevelScal[1][7] = scanner.GetFloat();
			m_fEnchantLevelScal[1][8] = scanner.GetFloat();
			m_fEnchantLevelScal[1][9] = scanner.GetFloat();
		}

		scanner.GetToken();
	}
	
	return TRUE;
}
#endif //__Y_ADV_ENCHANT_EFFECT


// propJob.inc를 읽어들인다.
BOOL CProject::LoadPropJob( LPCTSTR lpszFileName )
{
	CScript scanner;
	if( scanner.Load( lpszFileName, FALSE ) == FALSE )
	{
#ifdef __CLIENT
		MessageBox( g_Neuz.GetSafeHwnd(), "프로퍼티 읽기실패 : Job", "프리프", MB_OK );
		ADDERRORMSG( "프로퍼티 읽기실패 : Job" );
#endif //__CLIENT
		return FALSE;
	}
	
	while( 1 )
	{
		int nJob = scanner.GetNumber();	
		if( scanner.tok == FINISHED )
			break;

		JobProp* pProperty = &m_aPropJob[nJob];
		pProperty->fAttackSpeed      = scanner.GetFloat();		
		pProperty->fFactorMaxHP      = scanner.GetFloat();		
		pProperty->fFactorMaxMP      = scanner.GetFloat();		
		pProperty->fFactorMaxFP      = scanner.GetFloat();		
		pProperty->fFactorDef        = scanner.GetFloat();		
		pProperty->fFactorHPRecovery = scanner.GetFloat();	
		pProperty->fFactorMPRecovery = scanner.GetFloat();	
		pProperty->fFactorFPRecovery = scanner.GetFloat();	
		pProperty->fMeleeSWD		 = scanner.GetFloat();	
		pProperty->fMeleeAXE		 = scanner.GetFloat();	
		pProperty->fMeleeSTAFF		 = scanner.GetFloat();	
		pProperty->fMeleeSTICK		 = scanner.GetFloat();	
		pProperty->fMeleeKNUCKLE	 = scanner.GetFloat();	
		pProperty->fMagicWAND		 = scanner.GetFloat();	
		pProperty->fBlocking		 = scanner.GetFloat();	
		pProperty->fMeleeYOYO        = scanner.GetFloat();	
		pProperty->fCritical         = scanner.GetFloat();	
	}
	return TRUE;
}

BOOL CProject::LoadPropCtrl( LPCTSTR lpszFileName, CFixedArray<CtrlProp>*	apObjProp)
{
	CScript scanner;
	// load unicode
	if( scanner.Load( lpszFileName, FALSE ) == FALSE )
	{
#ifdef __CLIENT
		MessageBox( g_Neuz.GetSafeHwnd(), "프로퍼티 읽기실패 : Ctrl", "프리프", MB_OK );
		ADDERRORMSG( "프로퍼티 읽기실패 : Ctrl" );
#endif //__CLIENT
		return FALSE;
	}
	
	CtrlProp ctrlProp;

	char szTemp[ 128 ];

	int i	= scanner.GetNumber();	// id

	while( scanner.tok != FINISHED )
	{
		ctrlProp.dwID = i;    
		if( i <= 0 )
			Error( "%s 읽다가 ID가 %d이나왔다. 마지막으로 읽은것 %s", lpszFileName, i, szTemp );
		scanner.GetToken();
		TRACE( "PropCtrl = %d, %s\n", i, scanner.token );
		_tcscpy( ctrlProp.szName, scanner.token );
		m_mapCtrl.insert( map<string, DWORD>::value_type( ctrlProp.szName, ctrlProp.dwID ) );
		ctrlProp.dwCtrlKind1 = scanner.GetNumber();
		ctrlProp.dwCtrlKind2 = scanner.GetNumber();
		ctrlProp.dwCtrlKind3 = scanner.GetNumber();
		ctrlProp.dwSfxCtrl = scanner.GetNumber();
		ctrlProp.dwSndDamage = scanner.GetNumber();

		scanner.GetToken();		// skip szCommand (사용하지 않는다. )
		apObjProp->SetAtGrow( i, &ctrlProp);
		i = scanner.GetNumber(); 
	}
	apObjProp->Optimize();
	return TRUE;
}

#if __VER >= 8  
BOOL CProject::LoadScriptDiePenalty( LPCTSTR lpszFileName )
{
	CScript scanner;
	if( scanner.Load( lpszFileName, FALSE ) == FALSE )
	{
		Error( "DiePenalty.ini not found" );
		return FALSE;
	}
	m_vecRevivalPenalty.clear();
	m_vecDecExpPenalty.clear();
	m_vecLevelDownPenalty.clear();
	
	scanner.GetToken();
	while( scanner.tok != FINISHED )
	{	
		if( scanner.Token == _T( "REVIVAL_PENALTY" ) )
		{
			scanner.GetToken();	// {
			scanner.GetToken();	// {
			
			while( *scanner.token != '}' )
			{
				if( scanner.Token == _T( "ADDPENALTY" ) )
				{
					DIE_PENALTY DiePenalty;
					DiePenalty.nLevel	= scanner.GetNumber();
					DiePenalty.nValue	= scanner.GetNumber();
					m_vecRevivalPenalty.push_back( DiePenalty );
				}
				scanner.GetToken();
			}
		}
		if( scanner.Token == _T( "DECEXP_PENALTY" ) )
		{
			scanner.GetToken();	// {
			scanner.GetToken();	// {
			
			while( *scanner.token != '}' )
			{
				if( scanner.Token == _T( "ADDPENALTY" ) )
				{
					DIE_PENALTY DiePenalty;
					DiePenalty.nLevel	= scanner.GetNumber();
					DiePenalty.nValue	= scanner.GetNumber();
					m_vecDecExpPenalty.push_back( DiePenalty );
				}
				scanner.GetToken();
			}
		}
		if( scanner.Token == _T( "LEVEL_DOWN_PENALTY" ) )
		{
			scanner.GetToken();	// {
			scanner.GetToken();	// {
			
			while( *scanner.token != '}' )
			{
				if( scanner.Token == _T( "ADDPENALTY" ) )
				{
					DIE_PENALTY DiePenalty;
					DiePenalty.nLevel	= scanner.GetNumber();
					DiePenalty.nValue	= scanner.GetNumber();
					m_vecLevelDownPenalty.push_back( DiePenalty );
				}
				scanner.GetToken();
			}
		}
		scanner.GetToken();
	}
	return TRUE;
}
#endif //  __VER >= 8  

#if __VER >= 8 // __S8_PK
BOOL CProject::LoadScriptPK( LPCTSTR lpszFileName )
{
	CScript scanner;
	if( scanner.Load( lpszFileName, FALSE ) == FALSE )
	{
		Error( "PKSetting.inc not found" );
		return FALSE;
	}

	scanner.GetToken();
	while( scanner.tok != FINISHED )
	{
		if( scanner.Token == _T( "GENERAL_COLOR" ) )
		{
			m_PKSetting.dwGeneralColor = scanner.GetNumber();
		}
		else if( scanner.Token == _T( "READY_COLOR" ) )
		{
			m_PKSetting.dwReadyColor = scanner.GetNumber();
		}
		else if( scanner.Token == _T( "CHAO_COLOR" ) )
		{
			m_PKSetting.dwChaoColor = scanner.GetNumber();
		}
		else if( scanner.Token == _T( "GENERAL_ATTACK_TIME" ) )
		{
			m_PKSetting.nGeneralAttack = scanner.GetNumber();
		}
		else if( scanner.Token == _T( "READY_ATTACK_TIME" ) )
		{
			m_PKSetting.nReadyAttack = scanner.GetNumber();
		}
		else if( scanner.Token == _T( "LIMITE_LEVEL" ) )
		{
			m_PKSetting.nLimitLevel = scanner.GetNumber();
		}
		else if( scanner.Token == _T( "DIE_PROPENSITY_MIN" ) )
		{
			m_PKSetting.nDiePropensityMin = scanner.GetNumber();
		}
		else if( scanner.Token == _T( "DIE_PROPENSITY_MAX" ) )
		{
			m_PKSetting.nDiePropensityMax = scanner.GetNumber();
		}
		else if( scanner.Token == _T( "PROPENSITY_PENALTY" ) )
		{
			scanner.GetToken();	// {
			scanner.GetToken();	// {
			
			while( *scanner.token != '}' )
			{
				if( scanner.Token == _T( "ADDPENALTY" ) )
				{
					int nPropensity;
					CHAO_PROPENSITY bufPropensity;
					nPropensity						= scanner.GetNumber();
					bufPropensity.dwPropensityMin	= scanner.GetNumber();
					bufPropensity.dwPropensityMax	= scanner.GetNumber();
					bufPropensity.nEquipDorpMin		= scanner.GetNumber();
					bufPropensity.nEquipDorpMax		= scanner.GetNumber();
					bufPropensity.nInvenDorpMin		= scanner.GetNumber();
					bufPropensity.nInvenDorpMax		= scanner.GetNumber();
					bufPropensity.nShop				= scanner.GetNumber();
					bufPropensity.nBank				= scanner.GetNumber();
					bufPropensity.nFly				= scanner.GetNumber();
					bufPropensity.nGuardAttack		= scanner.GetNumber();
					bufPropensity.nVendor			= scanner.GetNumber();
					m_PKSetting.mapPropensityPenalty.insert( map<int,CHAO_PROPENSITY>::value_type( nPropensity, bufPropensity ) );
				}
				scanner.GetToken();
			}
		}
		else if( scanner.Token == _T( "LEVEL_EXP" ) )
		{
			scanner.GetToken();	// {
			scanner.GetToken();	// {
			
			while( *scanner.token != '}' )
			{
				if( scanner.Token == _T( "ADDEXP" ) )
				{
					int nLevel = scanner.GetNumber();
					DWORD dwExp = scanner.GetNumber();
					m_PKSetting.mapLevelExp.insert( map<int,DWORD>::value_type( nLevel, dwExp ) );
				}
				scanner.GetToken();
			}
		}
		scanner.GetToken();
	}
	
	return TRUE;
}
#else // __VER >= 8 // __S8_PK
BOOL CProject::LoadPropKarma( LPCTSTR lpszFileName )
{
	ZeroMemory( m_aKarmaProp, sizeof( m_aKarmaProp ) );
				
	CScript scanner;
	// load unicode
	if( scanner.Load( lpszFileName, FALSE ) == FALSE )
	{
#ifdef __CLIENT
		MessageBox( g_Neuz.GetSafeHwnd(), "프로퍼티 읽기실패 : Karma", "프리프", MB_OK );
		ADDERRORMSG( "프로퍼티 읽기실패 : Karma" );
#endif //__CLIENT
		return FALSE;
	}
	
	KarmaProp karmaProp;

	DWORD dwCount = 0;
	DWORD i	= scanner.GetNumber();	// id

	while( scanner.tok != FINISHED )
	{
		if( dwCount >= MAX_KARMAPROP )
			Error( "최대읽을수 있는 배열 범위 벗어남(PropKarma) : Max %d", MAX_KARMAPROP );

		karmaProp.nGrade = i;
		scanner.GetToken();
		TRACE( "PropKarma = %d, %s\n", i, scanner.token );
		_tcscpy( karmaProp.szName, scanner.token );
		karmaProp.nKarmaPoint		= scanner.GetNumber();
		karmaProp.dwKarmaGrade      = scanner.GetNumber();
		karmaProp.dwColor			= scanner.GetNumber();
		karmaProp.dwKarmaRecoverPoint = scanner.GetNumber();
		karmaProp.fDiscountRate		= scanner.GetFloat();
		karmaProp.fSellPenaltyRate	= scanner.GetFloat();
		karmaProp.bGuardReaction	= scanner.GetNumber();
		karmaProp.nSubtractExpRate	= scanner.GetNumber();
		karmaProp.nDropGoldPercent	= scanner.GetNumber();
		karmaProp.nDropItem			= scanner.GetNumber();
		karmaProp.nDropPercent		= scanner.GetNumber();
		karmaProp.nKarmaRecoverNum	= scanner.GetNumber();
		karmaProp.dwStatLimitTime	= scanner.GetNumber();
		karmaProp.nStatLimitNum		= scanner.GetNumber();
		karmaProp.nStatLimitRate	= scanner.GetNumber();

		scanner.GetToken();
		TRACE( "szCommand = %s\n", scanner.token );
		_tcscpy( karmaProp.szCommand, scanner.token );
		i = scanner.GetNumber(); 

		m_aKarmaProp[dwCount++] = karmaProp;
	}
	return TRUE;
}
#endif // __VER >= 8 // __S8_PK


int	CProject::GetMinIdx( int nItemKind3, DWORD dwItemRare )
{
	if( dwItemRare >= MAX_UNIQUE_SIZE )
		return -1;
	return m_minMaxIdxAry[nItemKind3][dwItemRare].cx;
}

int CProject::GetMaxIdx( int nItemKind3, DWORD dwItemRare )
{
	if( dwItemRare >= MAX_UNIQUE_SIZE )
		return -1;
	return m_minMaxIdxAry[nItemKind3][dwItemRare].cy;
}

GUILDQUESTPROP*	CProject::GetGuildQuestProp( int nQuestId )
{
	VERIFY_RANGE( nQuestId, 0, m_aPropGuildQuest.GetSize(), "GetGuildQuestProp range_error", NULL );
	return m_aPropGuildQuest.GetAt( nQuestId );
}

PARTYQUESTPROP*	CProject::GetPartyQuestProp( int nQuestId )
{
	map<int, PARTYQUESTPROP>::iterator i = m_propPartyQuest.find( nQuestId );
	if( i != m_propPartyQuest.end() )
		return &i->second;
	return NULL;
}	

DWORD CProject::GetSkillPoint( ItemProp* pSkillProp )
{
	DWORD dwPoint = 0;
	if( pSkillProp->dwItemKind1 == JTYPE_BASE )
		dwPoint = m_dwVagSP;
	else if( pSkillProp->dwItemKind1 == JTYPE_EXPERT )
		dwPoint = m_dwExpertSP;
	else
		dwPoint = m_dwProSP;
	return dwPoint;
}

LPCTSTR CProject::GetGuildAppell( int nAppell )
{
	VERIFY_RANGE( nAppell, 0, MAX_GUILDAPPELL, "GetGuildAppell range_error", "error" );
	return m_aGuildAppell[ nAppell ].szName;
}


int CProject::GetExpUpItem( DWORD dwItemKind3, int nOption ) 
{ 
	if( nOption == 0 )	
		return 0;
#ifdef __NEW_ITEM_VARUNA
	if( nOption < 0 || nOption > 20 )	
#else // __NEW_ITEM_VARUNA
	if( nOption < 0 || nOption > 11 )	
#endif // __NEW_ITEM_VARUNA
	{
		Error( "CProject::GetExpUpItem range_error. %d", nOption );
		return 0;
	}

	switch( dwItemKind3 )
	{
	case IK3_SWD:
	case IK3_CHEERSTICK:
	case IK3_WAND:	
	case IK3_AXE:
	case IK3_KNUCKLEHAMMER:
	case IK3_STAFF:
	case IK3_YOYO: 
	case IK3_BOW:
#ifdef __CROSSBOW
	case IK3_CROSSBOW:
#endif // __CROSSBOW
#ifdef __NEW_WEAPON_GLOW
	case IK3_THAXE:
	case IK3_THSWD:
#endif // __NEW_WEAPON_GLOW
#if __VER >= 11 // __SYS_COLLECTING
	case IK3_COLLECTER:
#endif	// __SYS_COLLECTOR
		return m_aExpUpItem[0][ nOption ];
	case IK3_HELMET:
		return m_aExpUpItem[1][ nOption ];
	case IK3_SUIT:
		return m_aExpUpItem[2][ nOption ];
	case IK3_GAUNTLET:
		return m_aExpUpItem[3][ nOption ];
	case IK3_BOOTS:
		return m_aExpUpItem[4][ nOption ];
	case IK3_SHIELD:
#ifdef __NEW_ITEM_VARUNA
	case IK3_MAGICBARUNA:
	case IK3_ZEMBARUNA:
#endif // __NEW_ITEM_VARUNA
		return m_aExpUpItem[5][ nOption ];
	}

	Error( "GetExpUpItem : itemkind오류 %d, %d", dwItemKind3, nOption );
	return 0;
}

#ifdef __CLIENT
#ifndef __RULE_0615
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
	string str	= pszName;
	for( set<string>::iterator i = m_sInvalidNames.begin(); i != m_sInvalidNames.end(); ++i )
	{
		string strstr = *i;
		
		if( str.find( *i, 0 ) != -1 )
			return TRUE;
	}
	return FALSE;
}
#endif	// __RULE_0615
#endif	// __CLIENT

#ifdef __WORLDSERVER
#ifdef __JEFF_11_3
BOOL	CProject::LoadServerScript( const char* sFile )
{
	CScript s;
	if( s.Load( sFile ) == FALSE )
		return FALSE;

	s.GetToken();	// subject or FINISHED
	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "Accessory_Probability" ) )	// 아이템 각성
		{
			CAccessoryProperty* pProperty	= CAccessoryProperty::GetInstance();
			LPDWORD ptr	= pProperty->GetProbabilityPtr();
			int i	= 0;
			s.GetToken();	// {
			DWORD	dwProbability	= s.GetNumber();
			while( *s.token != '}' )
			{
				ptr[i++]	= dwProbability;
				dwProbability	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "Collecting_Enchant" ) )
		{
			CCollectingProperty* pProperty	= CCollectingProperty::GetInstance();
			vector<int>* ptr	= pProperty->GetEnchantProbabilityPtr();
			// 1 / 1000
			s.GetToken();	// {
			int nProb	= s.GetNumber();
			while( *s.token != '}' )
			{
				ptr->push_back( nProb );
				nProb	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "Collecting_Item" ) )
		{
			CCollectingProperty* pProperty	= CCollectingProperty::GetInstance();
			vector<COLLECTING_ITEM>* ptr	= pProperty->GetItemPtr();
			COLLECTING_ITEM item;
			DWORD dwTotal	= 0;
			s.GetToken();	// {
			item.dwItemId	= s.GetNumber();
			while( *s.token != '}' )
			{
				dwTotal		+= s.GetNumber();
				item.dwProb	= dwTotal;
				ptr->push_back( item );
				item.dwItemId	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "Pet_LevelupAvail" ) )
		{
			CPetProperty* pProperty		= CPetProperty::GetInstance();
			LPDWORD ptr1	= pProperty->GetLevelupAvailLevelProbabilityPtr();
			LPBYTE ptr2		= pProperty->GetLevelupAvailLevelMaxPtr();
			// 	10000	0	0	0	0	0	0	0	0
			int	nLevel	= (int)PL_D;
			s.GetToken();	// {{
			DWORD	dwProbability	= s.GetNumber();
			while( *s.token != '}' )
			{
//				ptr1[nLevel][0]	= dwProbability;
				ptr1[nLevel*MAX_PET_AVAIL_LEVEL]	= dwProbability;
				for( int i = 1; i < MAX_PET_AVAIL_LEVEL; i++ )
				{
//					ptr1[nLevel][i]	= s.GetNumber();
					ptr1[nLevel*MAX_PET_AVAIL_LEVEL+i]	= s.GetNumber();
//					if( ptr1[nLevel][i] > 0 )
					if( ptr1[nLevel*MAX_PET_AVAIL_LEVEL+i] > 0 )
						ptr2[nLevel]	= i;
				}
				nLevel++;
				dwProbability	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "Pet_AddLifeProbability" ) )
		{
			CPetProperty* pProperty		= CPetProperty::GetInstance();
			vector<WORD>* ptr	= pProperty->GetAddLifeProbabilityPtr();
			// 수명 회복액 확률	// 추가 될 경우 합이 100이 되도록 확장
			// 	50	// +1
			s.GetToken();	// {
			WORD wProbability	= s.GetNumber();
			while( *s.token != '}' )
			{
				ptr->push_back( wProbability );
				wProbability	= s.GetNumber();
			}
		}
		s.GetToken();
	}
	return TRUE;
}
#endif	// __JEFF_11_3
#endif	// __WORLDSERVER

#ifdef __PERF_0226
CPartsItem::CPartsItem()
{
	m_pProject	= NULL;
}

CPartsItem::~CPartsItem()
{
	for( int i = 0; i < 3; i++ )
	{
		for( int j = 0; j < MAX_HUMAN_PARTS; j++ )
			m_items[i][j].clear();
	}
}

void	CPartsItem::Init( CProject* pProject )
{
	m_pProject	= pProject;
	int nSize	= pProject->m_aPropItem.GetSize();
	for( int i = 0; i < nSize; i++ )
	{
		ItemProp* pProp	= pProject->GetItemProp( i );
		if( pProp && pProp->dwParts >= 0 && pProp->dwParts < MAX_HUMAN_PARTS )
		{
			int nSex	= pProp->dwItemSex;
			if( nSex < 0 )
				nSex	= SEX_SEXLESS;
			m_items[nSex][pProp->dwParts].push_back( pProp );
		}
	}
}

ItemProp*	CPartsItem::GetItemProp( int nSex, int nParts )
{
	int nIndex	= xRandom( m_items[nSex][nParts].size() );
	return	m_items[nSex][nParts][nIndex];
}

CPartsItem*	CPartsItem::GetInstance( void )
{
	static	CPartsItem	sPartsItem;
	return &sPartsItem;
}
#endif	// __PERF_0226

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
		if( pMoverProp && pMoverProp->dwID > 0 && pMoverProp->dwID < MAX_PROPMOVER && pMoverProp->dwBelligerence != BELLI_PEACEFUL && pMoverProp->dwBelligerence != BELLI_ALLIANCE )
		{
			TRACE( "%d\n", i );
			s.Format( "\n%s\t%d", pMoverProp->szName, pMoverProp->m_DropItemGenerator.m_dwMax );
			// dropitem
			{
				int cbDropItem	= pMoverProp->m_DropItemGenerator.GetSize();
				int nNumber	= 0;
				DROPITEM* lpDropItem;
				for( int i = 0; i < cbDropItem; i++ )
				{
					if( lpDropItem = pMoverProp->m_DropItemGenerator.GetAt( i, FALSE, 0.0f ) )
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
								str.Format( "\n \t%s\t%f%%\t%d\t%d", pItemProp->szName, (double)dwProbability / (double)30000000, lpDropItem->dwNumber, lpDropItem->dwLevel );
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
			// dropkind
			{
				int nSize	= pMoverProp->m_DropKindGenerator.GetSize();
				DROPKIND* pDropKind;
				CPtrArray* pItemKindAry;
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

#if __VER >= 15 /* __IMPROVE_QUEST_INTERFACE */ && defined( __CLIENT )
BOOL CProject::LoadQuestDestination( void )
{
	CScanner s;
	if( s.Load( "QuestDestination.txt.txt" ) == FALSE )
		return FALSE;

	s.GetToken();
	while( s.tok != FINISHED )
	{
		int nID = atoi( s.Token.Right( 4 ) );
		s.GetLastFull();
		CString strQuestion = s.Token;
		m_mapQuestDestination.insert( make_pair( nID, strQuestion ) );
		s.GetToken();
	}
	return TRUE;
}

const CString& CProject::GetQuestDestination( DWORD dwKey ) const
{
	static CString strEmpty = _T( "" );
	map< int, CString >::const_iterator i = m_mapQuestDestination.find( dwKey );
	if( i != m_mapQuestDestination.end() )
		return i->second;
	return strEmpty;
}

BOOL CProject::LoadPatrolDestination( void )
{
	CScanner s;
	if( s.Load( "PatrolDestination.txt.txt" ) == FALSE )
		return FALSE;

	s.GetToken();
	while( s.tok != FINISHED )
	{
		int nID = atoi( s.Token.Right( 4 ) );
		s.GetLastFull();
		CString strQuestion = s.Token;
		m_mapPatrolDestination.insert( make_pair( nID, strQuestion ) );
		s.GetToken();
	}
	return TRUE;
}

const CString& CProject::GetPatrolDestination( DWORD dwKey ) const
{
	static CString strEmpty = _T( "" );
	map< int, CString >::const_iterator i = m_mapPatrolDestination.find( dwKey );
	if( i != m_mapPatrolDestination.end() )
		return i->second;
	return strEmpty;
}
#endif // defined( __IMPROVE_QUEST_INTERFACE ) && defined( __CLIENT )