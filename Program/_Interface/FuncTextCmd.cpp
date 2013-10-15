#include "stdafx.h"
#include "defineText.h"
#include "defineObj.h"
#include "FuncTextCmd.h"
#include "WorldMng.h"
#include "definequest.h"

#ifdef __CLIENT
#include "AppDefine.h"
#include "WndAdminCreateItem.h"
#include "WndIndirectTalk.h"
#include "WndChangeFace.h"
#include "dpclient.h"
#include "timeLimit.h"
#endif // __CLIENT

#ifdef __WORLDSERVER
#include "User.h"
#include "UserMacro.h"
#include "DPCoreClient.h"
#include "dpdatabaseclient.h"
#include "dpSrvr.h"
#include "eveschool.h"
#include "WorldDialog.h"
#if __VER >= 13 // __EXT_ENCHANT
#include "ItemUpgrade.h"
#endif // __EXT_ENCHANT
#endif	// __WORLDSERVER

#if __VER >= 11 // __SYS_IDENTIFY
#include "randomoption.h"
#endif	// __SYS_IDENTIFY

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

#if __VER >= 12 // __SECRET_ROOM
#include "SecretRoom.h"
#endif // __SECRET_ROOM

#if __VER >= 12 // __LORD
#ifdef __CLIENT
#include "clord.h"
#endif	// __CLIENT
#endif	// __LORD

#if __VER >= 12 // __TAX
#include "Tax.h"
#endif // __TAX

#if __VER >= 13 // __HONORABLE_TITLE			// 달인
	#include "honor.h"
#endif	// __HONORABLE_TITLE			// 달인

#if __VER >= 13 // __RAINBOW_RACE
#ifdef __WORLDSERVER
#include "RainbowRace.h"
#endif // __WORLDSERVER
#endif // __RAINBOW_RACE

#include "guild.h"
#include "party.h"
#include "post.h"

#if __VER >= 13 // __COUPLE_1117
#include "couplehelper.h"
#include "couple.h"
#endif	// __COUPLE_1117

#ifdef __QUIZ
#ifdef __WORLDSERVER
#include "Quiz.h"
#endif // __WORLDSERVER
#endif // __QUIZ

#if __VER >= 15 // __GUILD_HOUSE
#include "GuildHouse.h"
#endif // __GUILD_HOUSE

#if __VER >= 15 // __CAMPUS
#ifdef __WORLDSERVER
#include "CampusHelper.h"
#endif // __WORLDSERVER
#endif // __CAMPUS

extern	CPartyMng			g_PartyMng;
extern	CGuildMng			g_GuildMng;

#ifdef __CLIENT
extern	CParty				g_Party;
extern  CDPClient			g_DPlay;
#endif // __CLITEM

#ifdef __WORLDSERVER
extern	CDPDatabaseClient	g_dpDBClient;
extern	CDPCoreClient		g_DPCoreClient;
extern	CUserMng			g_UserMng;
extern  CWorldMng			g_WorldMng;
extern	CDPSrvr				g_DPSrvr;
extern	CGuildCombat		g_GuildCombatMng;
#endif

#if __VER >= 11 // __SYS_COLLECTING
#include "definesound.h"
#endif	// __SYS_COLLECTING


#define TCM_CLIENT 0
#define TCM_SERVER 1
#define TCM_BOTH   2


#define BEGINE_TEXTCMDFUNC_MAP TextCmdFunc m_textCmdFunc[] = {
#define END_TEXTCMDFUNC_MAP 0, 0, 0, 0, 0, 0, AUTH_GENERAL, 0 };
#define ON_TEXTCMDFUNC( a, b, c, d, e, f, g, h ) a, b, c, d, e, f, g, h,

BOOL TextCmd_outall( CScanner& scanner )            
{ 
#ifdef __WORLDSERVER
    g_DPCoreClient.SendSystem( "Alle Spieler wurden vom Server verwiesen." );
    g_UserMng.RemoveAllUsers();
#endif    // __WORLDSERVER
    return TRUE;
}  

BOOL TextCmd_InvenClear( CScanner& scanner )       
{ 
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	D3DXVECTOR3 vPos	= pUser->GetPos();
	CWorld* pWorld	= pUser->GetWorld();
	
	if( !pUser->HasActivatedSystemPet() )
	{
		int nNum = 0;
		int	nSize	= pUser->m_Inventory.GetMax();
		
		for( int i = 0 ; i < nSize; ++i )
		{
			CItemElem* pItemElem = pUser->m_Inventory.GetAtId( i );
			if( pItemElem )
			{
				pUser->UpdateItem( i, UI_NUM, 0 );		// remove
			}
		}
	}
	else
		pUser->AddDefinedText( TID_GAME_PET_NOWUSE );
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_CommandList( CScanner& scanner )  
{ 
	return TRUE;
}

BOOL TextCmd_Open( CScanner& scanner )  
{ 
#ifdef __CLIENT
	scanner.GetToken();
	DWORD dwIdApplet = g_WndMng.GetAppletId( scanner.token );
	g_WndMng.CreateApplet( dwIdApplet );
#endif
	return TRUE;
}
BOOL TextCmd_Close( CScanner& scanner )  
{ 
#ifdef __CLIENT
	scanner.GetToken();
	DWORD dwIdApplet = g_WndMng.GetAppletId( scanner.token );
	CWndBase* pWndBase = g_WndMng.GetWndBase( dwIdApplet );
	if( pWndBase ) pWndBase->Destroy();
#endif
	return TRUE;
}

BOOL TextCmd_Time( CScanner& scanner )  
{ 
#ifdef __CLIENT
	CString string;
	CTime time = CTime::GetCurrentTime();
	//time.Get
	string = time.Format( "Real Time - %H:%M:%S" );
	g_WndMng.PutString( string );
	string.Format( "Madrigal Time - %d:%d:%d\n", g_GameTimer.m_nHour, g_GameTimer.m_nMin, g_GameTimer.m_nSec );
	g_WndMng.PutString( string );
	
#endif
	return TRUE;
}

BOOL TextCmd_ChangeShopCost( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	FLOAT f = scanner.GetFloat();
	int nAllServer = scanner.GetNumber();

	if( f > 2.0f )
		f = 1.0f;
	else if( f < 0.5f )
		f = 1.0f;

	if( nAllServer != 0 )
	{
		g_DPCoreClient.SendGameRate( f, GAME_RATE_SHOPCOST );
		return TRUE;
	}

	prj.m_fShopCost = f;	
	g_UserMng.AddGameRate( prj.m_fShopCost, GAME_RATE_SHOPCOST );
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_SetMonsterRespawn( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	D3DXVECTOR3 vPos	= pUser->GetPos();
	CWorld* pWorld	= pUser->GetWorld();
	
	MoverProp* pMoverProp	= NULL;

	scanner.GetToken();
	if( scanner.tokenType == NUMBER ) 
	{
		DWORD dwID	= _ttoi( scanner.Token );
		pMoverProp = prj.GetMoverPropEx( dwID );

	}
	else
		pMoverProp	= prj.GetMoverProp( scanner.Token );

	if( pMoverProp && pMoverProp->dwID != 0 )
	{
		DWORD dwNum	= scanner.GetNumber();
		if( dwNum > 30 ) dwNum = 30;
		if( dwNum < 1 ) dwNum = 1;

		DWORD dwAttackNum	= scanner.GetNumber();
		if( dwAttackNum > dwNum ) dwAttackNum = dwNum;
		if( dwAttackNum < 1 ) dwAttackNum = 0;

		DWORD dwRect = scanner.GetNumber();
		if( dwRect > 255 ) dwRect = 255;
		if( dwRect < 1 ) dwRect = 1;

		DWORD dwTime = scanner.GetNumber();
		if( dwTime > 10800 ) dwTime = 10800;
		if( dwTime < 10 ) dwTime = 10;

		int nAllServer = scanner.GetNumber();
		if( nAllServer != 0 )
		{
			BOOL bFlying = FALSE;
			if( pMoverProp->dwFlying )
				bFlying = TRUE;
			g_DPCoreClient.SendSetMonsterRespawn( pUser->m_idPlayer, pMoverProp->dwID, dwNum, dwAttackNum, dwRect, dwTime, bFlying );
			return TRUE;
		}

		CRespawnInfo ri;
		ri.m_dwType = OT_MOVER;
		ri.m_dwIndex = pMoverProp->dwID;
		ri.m_cb = dwNum;
		ri.m_nActiveAttackNum = dwAttackNum;
		if( pMoverProp->dwFlying != 0 )
			ri.m_vPos = vPos;
		ri.m_rect.left		= (LONG)( vPos.x - dwRect );
		ri.m_rect.right		= (LONG)( vPos.x + dwRect );
		ri.m_rect.top		= (LONG)( vPos.z - dwRect );
		ri.m_rect.bottom	= (LONG)( vPos.z + dwRect );
		ri.m_uTime			= (u_short)( dwTime );
		ri.m_cbTime			= 0;

		char chMessage[512] = {0,};
#ifdef __S1108_BACK_END_SYSTEM
			pWorld->m_respawner.Add( ri, TRUE );
#else // __S1108_BACK_END_SYSTEM
		pWorld->m_respawner.Add( ri );
#endif // __S1108_BACK_END_SYSTEM

		sprintf( chMessage, "Add Respwan Monster : %s(%d/%d) Rect(%d, %d, %d, %d) Time : %d", 
			pMoverProp->szName, ri.m_cb, ri.m_nActiveAttackNum, ri.m_rect.left, ri.m_rect.right, ri.m_rect.top, ri.m_rect.bottom, ri.m_uTime );
		pUser->AddText( chMessage );
	}
#endif	// __WORLDSERVER
	return TRUE;
}

#ifdef __S1108_BACK_END_SYSTEM

BOOL TextCmd_PropMonster( CScanner & scanner )
{
#ifdef __CLIENT
	char chMessage[1024] = {0,};
	if( 0 < prj.m_nAddMonsterPropSize )
	{
		for( int i = 0 ; i < prj.m_nAddMonsterPropSize ; ++i )
		{
			sprintf( chMessage, "Monster Prop(%s) AttackPower(%d), Defence(%d), Exp(%d), Hitpioint(%d), ItemDorp(%d), Penya(%d)", 
				prj.m_aAddProp[i].szMonsterName, prj.m_aAddProp[i].nAttackPower, prj.m_aAddProp[i].nDefence, prj.m_aAddProp[i].nExp,
				prj.m_aAddProp[i].nHitPoint, prj.m_aAddProp[i].nItemDrop, prj.m_aAddProp[i].nPenya	);
			g_WndMng.PutString( chMessage, NULL, 0xffff0000, CHATSTY_GENERAL );
		}
	}
	else
	{
		sprintf( chMessage, "Monster Prop Not Data" );
		g_WndMng.PutString( chMessage, NULL, 0xffff0000, CHATSTY_GENERAL );
	}
#endif	// __CLIENT
	return TRUE;
}
#endif // __S1108_BACK_END_SYSTEM

BOOL TextCmd_GameSetting( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	pUser->AddGameSetting();	
#endif // __WORLDSERVER
	return TRUE;	
}

BOOL TextCmd_ChangeFace( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;

	DWORD dwFace = scanner.GetNumber();
	if( dwFace < 0 || 4 < dwFace )
		return TRUE;
	
	if( (pUser->m_dwMode & NOTFRESH_MODE) || (pUser->m_dwMode & NOTFRESH_MODE2) )
	{
		pUser->m_dwHeadMesh = dwFace;
		g_UserMng.AddChangeFace( pUser->m_idPlayer, dwFace );
		if( pUser->m_dwMode & NOTFRESH_MODE )
		{
			pUser->m_dwMode &= ~NOTFRESH_MODE;
			pUser->AddDefinedText( TID_CHANGEFACE_ONE, "" );
//			pUser->AddText( "얼굴변경을 한번 사용하였습니다" );
		}
		else
		{
			pUser->m_dwMode &= ~NOTFRESH_MODE2;
			pUser->AddDefinedText( TID_CHANGEFACE_TWO, "" );
//			pUser->AddText( "얼굴변경을 2번 사용하였습니다" );
		}
	}
	else
	{
		pUser->AddDefinedText( TID_CHANGEFACE_THREE, "" );
//		pUser->AddText( "얼굴변경을 2번 모두 사용하여 사용할수 없습니다" );
	}
#else // __WORLDSERVER
	CWndChangeSex* pWndChangeSex	= (CWndChangeSex*)g_WndMng.GetWndBase( APP_CHANGESEX );
	if( NULL == pWndChangeSex )
	{
		pWndChangeSex	= new CWndChangeSex;
		pWndChangeSex->Initialize();
	}
	pWndChangeSex->SetData( NULL_ID, NULL_ID );
	return FALSE;
#endif
	return TRUE;
}

BOOL TextCmd_AroundKill( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	if( pUser->GetWeaponItem() == NULL )
		return TRUE;

	CWorld* pWorld	= pUser->GetWorld();
	if( pWorld )
		pUser->SendDamageAround( AF_MAGICSKILL, (CMover*)pUser, OBJTYPE_MONSTER, 1, 64.0f, 0.0, 1.0f );
#endif // __WORLDSERVER
	return TRUE;
}

#if __VER >= 9	// __PET_0410
BOOL	TextCmd_PetLevel( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	CPet* pPet	= pUser->GetPet();
	if( pPet && pPet->GetExpPercent() == 100 )
		pUser->PetLevelup();
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL	TextCmd_MakePetFeed( CScanner & s )
{
#ifdef __CLIENT
	if( g_WndMng.m_pWndPetFoodMill == NULL )
	{
		SAFE_DELETE( g_WndMng.m_pWndPetFoodMill );
		g_WndMng.m_pWndPetFoodMill = new CWndPetFoodMill;
		g_WndMng.m_pWndPetFoodMill->Initialize( &g_WndMng, APP_PET_FOOD );
		return FALSE;
	}
#endif	// __CLIENT
	return TRUE;
}

BOOL	TextCmd_PetExp( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	CPet* pPet	= pUser->GetPet();
	if( pPet && pPet->GetLevel() != PL_S )
	{
		pPet->SetExp( MAX_PET_EXP );
		pUser->AddPetSetExp( pPet->GetExp() );
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_Pet( CScanner & s )
{
	// /pet 1 100
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	s.GetToken();
	if( s.tok == FINISHED )
		return TRUE;
#if __VER >= 11 // __SYS_PLAYER_DATA
	DWORD idPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerId( s.token );
#else	// __SYS_PLAYER_DATA
	DWORD idPlayer	= prj.GetPlayerID( s.token );
#endif	// __SYS_PLAYER_DATA
	if( idPlayer == 0 )	//
		return TRUE;
	CUser* pTarget	= (CUser*)prj.GetUserByID( idPlayer );
	if( IsValidObj( pTarget ) == FALSE )
		return TRUE;
	CPet* pPet	= pTarget->GetPet();
	if( pPet == NULL )	//
		return TRUE;

	// kind
	s.GetToken();
	if( s.tok == FINISHED )
		return TRUE;
	BYTE nKind	= atoi( s.token );
	if( nKind >= PK_MAX )
		return TRUE;

	// exp
	s.GetToken();
	if( s.tok == FINISHED )
		return TRUE;
	BYTE nExpRate	= atoi( s.token );

	s.GetToken();
	if( s.tok == FINISHED )
		return TRUE;
	BYTE nLevel		= s.Token.GetLength();

	if( nLevel > PL_S )
		return TRUE;

	BYTE anAvail[PL_MAX - 1]	= { 0,};
	char sAvail[2]	= { 0,};
	
	for( int i = 0; i < nLevel; i++ )
	{
		sAvail[0]	= s.Token.GetAt( i );
		sAvail[1]	= '\0';
		anAvail[i]	= atoi( sAvail );
		if( anAvail[i] < 1 || anAvail[i] > 9 )
			return TRUE;
	}

	s.GetToken();
	if( s.tok == FINISHED )
		return TRUE;
	BYTE nLife	= atoi( s.token );
	if( nLife > 99 )
		nLife	= 99;

	CItemElem* pItemElem	= pTarget->GetPetItem();
	pPet->SetKind( nKind );
	pPet->SetLevel( nLevel );
	if( nLevel == PL_EGG )
		pPet->SetKind( 0 );	// initialize
	pItemElem->m_dwItemId	= pPet->GetItemId();
	pPet->SetEnergy( pPet->GetMaxEnergy() );
	DWORD dwExp		= pPet->GetMaxExp() * nExpRate / 100;
	pPet->SetExp( dwExp );

	for( i = PL_D; i <= nLevel; i++ )
		pPet->SetAvailLevel( i, anAvail[i-1] );
	for( i = nLevel + 1; i <= PL_S; i++ )
		pPet->SetAvailLevel( i, 0 );

	pPet->SetLife( nLife );

	if( pTarget->HasPet() )
		pTarget->RemovePet();

	g_dpDBClient.CalluspPetLog( pTarget->m_idPlayer, pItemElem->GetSerialNumber(), 0, PETLOGTYPE_LEVELUP, pPet );

	pTarget->AddPet( pPet, PF_PET_GET_AVAIL );	// 自
	g_UserMng.AddPetLevelup( pTarget, MAKELONG( (WORD)pPet->GetIndex(), (WORD)pPet->GetLevel() ) );	// 他
#endif	// __WORLDSERVER
	return TRUE;
}

#endif	// __PET_0410

#if __VER >= 11 // __SYS_POCKET
BOOL	TextCmd_MoveItem_Pocket( CScanner & s )
{
#ifdef __CLIENT
	int	nPocket1	= s.GetNumber();
	int nData	= s.GetNumber();
	int nNum	= s.GetNumber();
	int	nPocket2	= s.GetNumber();
	CItemElem* pItem	= NULL;
	if( nPocket1 < 0 )
		pItem	= g_pPlayer->m_Inventory.GetAt( nData );
	else
	{
		pItem	= g_pPlayer->m_Pocket.GetAtId( nPocket1, nData );
	}
	if( pItem )
		g_DPlay.SendMoveItem_Pocket( nPocket1, pItem->m_dwObjId, nNum, nPocket2 );
#endif	// __CLIENT
	return TRUE;
}

BOOL TextCmd_AvailPocket( CScanner & s )
{
#ifdef __CLIENT
	int nPocket		= s.GetNumber();
	CItemElem* pItemElem	= g_pPlayer->m_Inventory.GetAt( 0 );
	if( pItemElem )
		g_DPlay.SendAvailPocket( nPocket, pItemElem->m_dwObjId );
#endif	// __CLIENT
	return TRUE;
}

BOOL TextCmd_PocketView( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	pUser->AddPocketView();
#endif	// __WORLDSERVER
	return TRUE;
}
#endif	// __SYS_POCKET

#if __VER >= 11 // __SYS_COLLECTING
BOOL TextCmd_RefineCollector( CScanner & s )
{
// 0번째
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	int nAbilityOption	= s.GetNumber();
	if( s.tok == FINISHED )
		nAbilityOption	= 0;
	if( nAbilityOption > 5 )
		nAbilityOption	= 5;
	CItemElem* pTarget	= pUser->m_Inventory.GetAt( 0 );
	if( pTarget && pTarget->IsCollector( TRUE ) )
	{
		pUser->AddDefinedText( TID_UPGRADE_SUCCEEFUL );
		pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );
		if( pUser->IsMode( TRANSPARENT_MODE ) == 0 )
			g_UserMng.AddCreateSfxObj( pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );
		pUser->UpdateItem( (BYTE)( pTarget->m_dwObjId ), UI_AO,  nAbilityOption );
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_StartCollecting( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	pUser->StartCollecting();
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_StopCollecting( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	pUser->StopCollecting();
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_DoUseItemBattery( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	pUser->DoUseItemBattery();
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_RefineAccessory( CScanner & s )
{
	// 0번째
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	int nAbilityOption	= s.GetNumber();
	if( s.tok == FINISHED )
		nAbilityOption	= 0;
	CItemElem* pTarget	= pUser->m_Inventory.GetAt( 0 );
	if( pTarget && pTarget->IsAccessory() )
		pUser->UpdateItem( (BYTE)( pTarget->m_dwObjId ), UI_AO,  nAbilityOption );
#endif	// __WORLDSERVER
	return TRUE;
}
#endif	// __SYS_COLLECTING

#if __VER >= 11 // __SYS_IDENTIFY
BOOL TextCmd_SetRandomOption( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	CItemElem* pItemElem	= pUser->m_Inventory.GetAt( 0 );
	if( pItemElem )
	{
		int nRandomOptionKind	= g_xRandomOptionProperty->GetRandomOptionKind( pItemElem );
		if( nRandomOptionKind >= 0 )
		{
			g_xRandomOptionProperty->InitializeRandomOption( pItemElem->GetRandomOptItemIdPtr() );
			int nDst, nAdj;
			int cb	= 0;
			nDst	= s.GetNumber();
			while( s.tok != FINISHED )
			{
				switch( nDst )
				{
					case 1:
					case 2:
					case 3:
					case 4:
					case 9:
					case 11:
					case 24:
					case 26:
					case 35:
					case 36:
					case 37:
					case 75:
					case 77:
					case 83:
						break;
					default:
						pUser->UpdateItemEx( (BYTE)( pItemElem->m_dwObjId ), UI_RANDOMOPTITEMID, pItemElem->GetRandomOptItemId() );
						return TRUE;
				}
				nAdj	= s.GetNumber();
				if( s.tok == FINISHED )
					break;
				g_xRandomOptionProperty->SetParam( pItemElem->GetRandomOptItemIdPtr(), nDst, nAdj );
				cb++;
				if( cb >= MAX_RANDOM_OPTION )
					break;
				nDst	= s.GetNumber();
			}
			pUser->UpdateItemEx( (BYTE)( pItemElem->m_dwObjId ), UI_RANDOMOPTITEMID, pItemElem->GetRandomOptItemId() );
		}
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_GenRandomOption( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
//	int i	= s.GetNumber();
	CItemElem* pItemElem	= pUser->m_Inventory.GetAt( 0 );
	if( pItemElem )
	{
		int nRandomOptionKind	= g_xRandomOptionProperty->GetRandomOptionKind( pItemElem );
		if( nRandomOptionKind >= 0 )	// 아이템 각성, 여신의 축복이 가능한 대상
		{
			g_xRandomOptionProperty->InitializeRandomOption( pItemElem->GetRandomOptItemIdPtr() );
			g_xRandomOptionProperty->GenRandomOption( pItemElem->GetRandomOptItemIdPtr(), nRandomOptionKind, pItemElem->GetProp()->dwParts );
			pUser->UpdateItemEx( (BYTE)( pItemElem->m_dwObjId ), UI_RANDOMOPTITEMID, pItemElem->GetRandomOptItemId() );
		}
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_PickupPetAwakeningCancel( CScanner & s )
{
#ifdef __CLIENT
	SAFE_DELETE( g_WndMng.m_pWndPetAwakCancel );
	g_WndMng.m_pWndPetAwakCancel = new CWndPetAwakCancel;
	g_WndMng.m_pWndPetAwakCancel->Initialize(&g_WndMng);
#endif	// __CLIENT
	return TRUE;
}

BOOL TextCmd_InitializeRandomOption( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
//	int i	= s.GetNumber();
	CItemElem* pItemElem	= pUser->m_Inventory.GetAt( 0 );
	if( pItemElem )
	{
		int nRandomOptionKind	= g_xRandomOptionProperty->GetRandomOptionKind( pItemElem );
		if( nRandomOptionKind >= 0 )	// 아이템 각성, 여신의 축복이 가능한 대상
		{
			g_xRandomOptionProperty->InitializeRandomOption( pItemElem->GetRandomOptItemIdPtr() );
			pUser->UpdateItemEx( (BYTE)( pItemElem->m_dwObjId ), UI_RANDOMOPTITEMID, pItemElem->GetRandomOptItemId() );
		}
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_ItemLevel( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	int i	= s.GetNumber();
	if( s.tok == FINISHED )
		i	= 0;
	CItemElem* pTarget	= pUser->m_Inventory.GetAt( 0 );
	if( pTarget )
	{
		ItemProp* pProp	= pTarget->GetProp();
		if( pProp->dwParts != NULL_ID && pProp->dwLimitLevel1 != 0xFFFFFFFF )
		{
			pTarget->SetLevelDown( i );
			pUser->UpdateItemEx( (BYTE)( pTarget->m_dwObjId ), UI_RANDOMOPTITEMID, pTarget->GetRandomOptItemId() );
		}
	}
#endif	// __WORLDSERVER
	return TRUE;
}
#endif	// __SYS_IDENTIFY

BOOL TextCmd_Level( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;

	scanner.GetToken();
	CString strJob = scanner.Token;

	int nJob = JOB_VAGRANT;

	LONG nLevel = scanner.GetNumber();
	
	if( nLevel == 0 )
		nLevel = 1;

	for( int i = 0 ; i < MAX_JOB ; i++ )
	{
		if( strcmp( strJob, prj.m_aJob[i].szName ) == 0 || strcmp( strJob, prj.m_aJob[i].szEName ) == 0 )
		{
			nJob = i;
			break;
		}
	}

	char chMessage[MAX_PATH] = {0,};
	if( MAX_JOB_LEVEL < nLevel && nJob == 0 )
	{
		sprintf( chMessage, prj.GetText(TID_GAME_CHOICEJOB) );
		pUser->AddText( chMessage );		
		return TRUE;
	}

#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
	LONG	nLegend = scanner.GetNumber();
	if( ( nLegend > 0 ) && ( nLegend < 3 ) )
	{
		for( int i = nJob + 1 ; i < MAX_JOB ; i++ )
		{
			if( strcmp( strJob, prj.m_aJob[i].szName ) == 0 || strcmp( strJob, prj.m_aJob[i].szEName ) == 0 )
			{
				nJob = i;
				if( nLegend == 1 )
					break;
				else
					nLegend--;
			}
		}

		pUser->InitLevel( nJob, nLevel );	// lock
		return	TRUE;
	}
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans
	
	if( nLevel <= MAX_JOB_LEVEL )	
	{
		pUser->InitLevel( JOB_VAGRANT, nLevel );	// lock
	}
	else
	if( MAX_JOB_LEVEL < nLevel &&  nLevel <= MAX_JOB_LEVEL + MAX_EXP_LEVEL )
	{
		if( MAX_JOBBASE <= nJob && nJob < MAX_EXPERT)
		{
			pUser->InitLevel( nJob, nLevel );	// lock
		}
		else
		{
			sprintf( chMessage, "Not Expert Job" );
			pUser->AddText( chMessage );
			sprintf( chMessage, "Expert Job : " );
			for( int i = MAX_JOBBASE ; i < MAX_EXPERT ; ++i )
			{
				if( strlen( prj.m_aJob[i].szName ) < 15 )
				{
					strcat( chMessage, prj.m_aJob[i].szName );
					if( i + 1 != MAX_EXPERT )
					{
						strcat( chMessage, ", ");
					}
				}
			}
			pUser->AddText( chMessage );
			sprintf( chMessage, "Expert Level : %d ~ %d", MAX_JOB_LEVEL + 1, MAX_JOB_LEVEL + MAX_EXP_LEVEL );
			pUser->AddText( chMessage );
			return TRUE;
		}
	}
	else
	if( MAX_JOB_LEVEL + MAX_EXP_LEVEL < nLevel && nLevel < MAX_GENERAL_LEVEL + 1 )
	{
		if( MAX_EXPERT <= nJob && nJob < MAX_PROFESSIONAL )
		{
			pUser->InitLevel( nJob, nLevel );	// lock
		}
		else
		{
			sprintf( chMessage, "Not Professional Job" );
			pUser->AddText( chMessage );
			sprintf( chMessage, "Professional Job : " );
			for( int i = MAX_EXPERT ; i < MAX_PROFESSIONAL ; ++i )
			{
				if( strlen( prj.m_aJob[i].szName ) < 15 )
				{
					strcat( chMessage, prj.m_aJob[i].szName );
					if( i + 1 != MAX_PROFESSIONAL )
					{
						strcat( chMessage, ", ");
					}
				}
			}
			pUser->AddText( chMessage );
			sprintf( chMessage, "Professional Level : %d ~~~ ", MAX_JOB_LEVEL + MAX_EXP_LEVEL + 1 );
			pUser->AddText( chMessage );
			return TRUE;
		}
	}
#endif // __WORLDSERVER
	return TRUE;
}

#ifdef __SFX_OPT
BOOL TextCmd_SfxLv( CScanner & scanner )
{
	int nLevel = scanner.GetNumber();
	if(nLevel > 5) nLevel = 5;
	if(nLevel < 0) nLevel = 0;
	g_Option.m_nSfxLevel = nLevel;

	return TRUE;
}
#endif

BOOL TextCmd_ChangeJob( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	
	scanner.GetToken();
	CString strJob = scanner.Token;
	
	int nJob = JOB_VAGRANT;
	
	for( int i = 0 ; i < MAX_JOB ; i++ )
	{
		if( strcmp( strJob, prj.m_aJob[i].szName ) == 0 || strcmp( strJob, prj.m_aJob[i].szEName ) == 0 )
		{
			nJob = i;
			break;
		}
	}
	
	char chMessage[MAX_PATH] = {0,};
	if( nJob == 0 )
	{
		sprintf( chMessage, "Error Job Name or Number" );
		pUser->AddText( chMessage );		
		return TRUE;
	}
	
	if( pUser->AddChangeJob( nJob ) )
	{
		( (CUser*)pUser )->AddSetChangeJob( nJob );
		g_UserMng.AddNearSetChangeJob( (CMover*)pUser, nJob, &pUser->m_aJobSkill[MAX_JOB_SKILL] );
		g_dpDBClient.SendLogLevelUp( (CUser*)pUser, 4 );
#if __VER >= 11 // __SYS_PLAYER_DATA
		g_dpDBClient.SendUpdatePlayerData( pUser );
#else	// __SYS_PLAYER_DATA
		g_DPCoreClient.SendPartyMemberJob( (CUser*)pUser );
		g_DPCoreClient.SendFriendChangeJob( (CUser*)pUser );
		if( pUser->m_idGuild != 0 )
			g_DPCoreClient.SendGuildChangeJobLevel( (CUser*)pUser );
#endif	// __SYS_PLAYER_DATA
		return TRUE;
	}
	else
	{
		sprintf( chMessage, "Error 1ch -> 2ch" );
		pUser->AddText( chMessage );		
		return TRUE;
	}

#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_stat( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;

	scanner.GetToken();
	CString strstat = scanner.Token;
	
	DWORD dwNum	= scanner.GetNumber();

	if( 2 <= strstat.GetLength() && strstat.GetLength() <= 7)
	{
		strstat.MakeLower();

		if( strcmp( strstat, "str" ) == 0 )
		{
			pUser->m_nStr = dwNum;
		}
		else
		if( strcmp( strstat, "sta" ) == 0 )
		{
			pUser->m_nSta = dwNum;
		}
		else
		if( strcmp( strstat, "dex" ) == 0 )
		{
			pUser->m_nDex = dwNum;
		}
		else
		if( strcmp( strstat, "int" ) == 0 )
		{
			pUser->m_nInt = dwNum;
		}
		else
		if( strcmp( strstat, "gp" ) == 0 )
		{
			pUser->m_nRemainGP = dwNum;
		}
		else
		if( strcmp( strstat, "restate" ) == 0 )
		{
			pUser->ReState();
			return FALSE;
		}
#if __VER >= 8 //__CSC_VER8_6
		else if( strcmp( strstat, "all" ) == 0 )
		{
			pUser->m_nStr = dwNum;
			pUser->m_nSta = dwNum;
			pUser->m_nDex = dwNum;
			pUser->m_nInt = dwNum;
		}
#endif //__CSC_VER8_6
		else
		{
			strstat += "unknown setting target";
			pUser->AddText( strstat );
			return FALSE;
		}
	}
	else
	{
		strstat += "unknown setting target";
		pUser->AddText( strstat );
		return FALSE;
	}

	pUser->AddSetState( pUser->m_nStr, pUser->m_nSta, pUser->m_nDex, pUser->m_nInt, pUser->m_nRemainGP );
#if __VER >= 13 // __HONORABLE_TITLE			// 달인
	pUser->CheckHonorStat();
	pUser->AddHonorListAck();
	g_UserMng.AddHonorTitleChange( pUser, pUser->m_nHonor);
#endif	// __HONORABLE_TITLE			// 달인
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_SetSnoop( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;

	s.GetToken();
	if( s.tok != FINISHED )
	{
		if( lstrcmp( pUser->GetName(), s.Token ) )
		{
#if __VER >= 11 // __SYS_PLAYER_DATA
			u_long idPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerId( s.token );
#else	// __SYS_PLAYER_DATA
			u_long idPlayer	= prj.GetPlayerID( s.Token );
#endif	// __SYS_PLAYER_DATA
			if( idPlayer > 0 )
			{
				BOOL bRelease	= FALSE;
				s.GetToken();
				if( s.tok != FINISHED )
					bRelease	= (BOOL)atoi( s.Token );
				g_DPCoreClient.SendSetSnoop( idPlayer, pUser->m_idPlayer,  bRelease );
			}
			else
				pUser->AddReturnSay( 3, s.Token );
		}
	}
#endif	// __WORLDSERVER
	return FALSE;
}

BOOL TextCmd_SetSnoopGuild( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;

	s.GetToken();
	if( s.tok != FINISHED )
	{
		CGuild* pGuild	= g_GuildMng.GetGuild( s.Token );
		if( pGuild )
		{
			BOOL bRelease	= FALSE;
			s.GetToken();
			if( s.tok != FINISHED )
				bRelease	= (BOOL)atoi( s.Token );
			g_DPCoreClient.SendSetSnoopGuild( pGuild->m_idGuild, bRelease );
		}
	}
#endif	// __WORLDSERVER
	return FALSE;
}

BOOL TextCmd_QuerySetPlayerName( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	scanner.GetToken();
	CString strPlayer	= scanner.Token;
	strPlayer.TrimLeft();
	strPlayer.TrimRight();
	g_dpDBClient.SendQuerySetPlayerName( pUser->m_idPlayer, strPlayer, MAKELONG( 0xffff, 0 ) );
	return TRUE;
#endif	// __WORLDSERVER
	return FALSE;
}

BOOL TextCmd_QuerySetGuildName( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	scanner.GetToken();
	CString strGuild	= scanner.Token;
	strGuild.TrimLeft();
	strGuild.TrimRight();
	CGuild* pGuild	= g_GuildMng.GetGuild( pUser->m_idGuild );
	if( pGuild && pGuild->IsMaster( pUser->m_idPlayer ) )
	{
		g_DPCoreClient.SendQuerySetGuildName( pUser->m_idPlayer, pUser->m_idGuild, (LPSTR)(LPCSTR)strGuild, 0xff );
	}
	else
	{
		// is not kingpin
	}
	return TRUE;
#endif	// __WORLDSERVER
	return FALSE;
}

BOOL TextCmd_CreateGuild( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	scanner.GetToken();
	GUILD_MEMBER_INFO	info;
	info.idPlayer	= pUser->m_idPlayer;
#if __VER < 11 // __SYS_PLAYER_DATA
	info.nLevel	= pUser->GetLevel();
	info.nJob	= pUser->GetJob();
	info.dwSex	= pUser->GetSex();
#endif	// __SYS_PLAYER_DATA
	g_DPCoreClient.SendCreateGuild( &info, 1, scanner.Token );
	return TRUE;
#endif	// __WORLDSERVER
	return FALSE;
}

BOOL TextCmd_DestroyGuild( CScanner & scanner )
{
#ifdef __CLIENT
	g_DPlay.SendDestroyGuild( g_pPlayer->m_idPlayer );
#endif
	return TRUE; 
}

BOOL TextCmd_RemoveGuildMember( CScanner & scanner )
{
#ifdef __CLIENT
	scanner.GetToken();
	char lpszPlayer[MAX_PLAYER]	= { 0, };
	lstrcpy( lpszPlayer, scanner.Token );
#if __VER >= 11 // __SYS_PLAYER_DATA
	u_long idPlayer		= CPlayerDataCenter::GetInstance()->GetPlayerId( lpszPlayer );
#else	// __SYS_PLAYER_DATA
	u_long idPlayer	= prj.GetPlayerID( lpszPlayer );
#endif	// __SYS_PLAYER_DATA
	if( idPlayer != 0 )
		g_DPlay.SendRemoveGuildMember( g_pPlayer->m_idPlayer, idPlayer );
	return TRUE;
#endif	// __CLIENT
	return FALSE;
}

BOOL TextCmd_GuildChat( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;

#ifdef __JEFF_9_20
#if __VER >= 12 // __LORD
	int nText	= pUser->GetMuteText();
	if(  nText )
	{
		pUser->AddDefinedText( nText );
		return TRUE;
	}
#else	// __LORD
	if( pUser->IsMute() )
		return TRUE;
#endif	// __LORD
#endif	// __JEFF_9_20

	char sChat[260]		= { 0, };
	scanner.GetLastFull();
	if( strlen( scanner.token ) >= 260 )
		return TRUE;
	strcpy( sChat, scanner.token );

	StringTrimRight( sChat );
	if( !(pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_FONTEDIT)))
		ParsingEffect(sChat, strlen(sChat) );
	
	RemoveCRLF( sChat );

	g_DPCoreClient.SendGuildChat( pUser, sChat );
	return TRUE;
#else	// __WORLDSERVER
#ifdef __CLIENT
	CString string	= scanner.m_pProg;
	g_WndMng.WordChange( string );

	if( ::GetLanguage() == LANG_THA )
		string = '"'+string+'"';

	CString strCommand;
	strCommand.Format( "/g %s", string );
	g_DPlay.SendChat( strCommand );
	return FALSE;
#endif	// __CLIENT
#endif	// __WORLDSERVER
	return FALSE;
}

BOOL TextCmd_DeclWar( CScanner & scanner )
{
#ifdef __CLIENT
	char szGuild[MAX_G_NAME]	= { 0, };
	scanner.GetLastFull();
	if( strlen( scanner.token ) >= MAX_G_NAME )
		return TRUE;
	strcpy( szGuild, scanner.token );
	StringTrimRight( szGuild );
	g_DPlay.SendDeclWar( g_pPlayer->m_idPlayer, szGuild );
#endif	// __CLIENT
	return FALSE;
}


// 길드 랭킹정보를 업데이트 시키는 명령어이다.
BOOL TextCmd_GuildRanking( CScanner & scanner )
{
#ifdef __WORLDSERVER
	// TRANS 서버에게 길드 랭킹 정보가 업데이트 되야함을 알린다.
	g_dpDBClient.UpdateGuildRanking();
#endif

	return TRUE;
}

// 길드 랭킹정보 DB를 업데이트 시키는 명령어이다.
BOOL TextCmd_GuildRankingDBUpdate( CScanner & scanner )
{
#ifdef __WORLDSERVER
	// TRANS 서버에게 길드 랭킹 정보가 업데이트 되야함을 알린다.
	g_dpDBClient.UpdateGuildRankingUpdate();
#endif
	
	return TRUE;
}

BOOL TextCmd_GuildStat( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;

	scanner.GetToken();
	CString strstat = scanner.Token;
	
	strstat.MakeLower();

	if( strstat == "logo" )
	{
		DWORD dwLogo = scanner.GetNumber();
		TRACE("guild Logo:%d\n", dwLogo);
		g_DPCoreClient.SendGuildStat( pUser, GUILD_STAT_LOGO, dwLogo );
	}
	else
	if( strstat == "pxp" )
	{
		DWORD dwPxpCount = scanner.GetNumber();
		TRACE("guild pxpCount:%d\n", dwPxpCount);
		g_DPCoreClient.SendGuildStat( pUser, GUILD_STAT_PXPCOUNT, dwPxpCount );
	}
	else
	if( strstat == "penya" )
	{
		DWORD dwPenya = scanner.GetNumber();
		TRACE("guild dwPenya:%d\n", dwPenya);
		g_DPCoreClient.SendGuildStat( pUser, GUILD_STAT_PENYA, dwPenya);
	}
	else
	if( strstat == "notice" )
	{
		scanner.GetToken();
		TRACE("guild notice:%s\n", scanner.Token);
		g_DPCoreClient.SendGuildStat( pUser, GUILD_STAT_NOTICE, (DWORD)(LPCTSTR)scanner.Token);
	}
	else
	{
		strstat += "syntax error guild stat command.";
		pUser->AddText( strstat );
	}

#endif 
	return FALSE;
}

BOOL TextCmd_SetGuildQuest( CScanner & s )
{
#ifdef __WORLDSERVER

	s.GetToken();
	char sGuild[MAX_G_NAME];
	lstrcpy( sGuild, s.Token );
	int nQuestId	= s.GetNumber();
	int nState		= s.GetNumber();
	GUILDQUESTPROP* pProp	= prj.GetGuildQuestProp( nQuestId );
	if( !pProp )
		return FALSE;

	CGuild* pGuild	= g_GuildMng.GetGuild( sGuild );
	if( pGuild )
	{
		if( nState < QS_BEGIN || nState > QS_END )
		{
		}
		else
		{
			pGuild->SetQuest( nQuestId, nState );
			g_dpDBClient.SendUpdateGuildQuest( pGuild->m_idGuild, nQuestId, nState );
		}
	}
	return TRUE;

#endif	// __WORLDSERVER
	return FALSE;
}

BOOL TextCmd_PartyLevel( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	DWORD dwLevel = scanner.GetNumber();
	DWORD dwPoint = scanner.GetNumber();
	if( dwPoint == 0 )
		dwPoint = 100;

	TRACE("plv LV:%d POINT:%d\n", dwLevel, dwPoint);
	
	CParty* pParty;
	pParty = g_PartyMng.GetParty( pUser->GetPartyId() );
	if( pParty )
	{
		pParty->SetPartyLevel( pUser, dwLevel, dwPoint, pParty->GetExp() );
	}
#endif // __WORLDSERVER
	return TRUE;
}

BOOL  TextCmd_InitSkillExp( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;

	if( pUser->InitSkillExp() == TRUE )
		pUser->AddInitSkill();
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_SkillLevel( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	
	DWORD dwSkillKind	= scanner.GetNumber();
	DWORD dwSkillLevel = scanner.GetNumber();

	LPSKILL pSkill = pUser->GetSkill( dwSkillKind );
	if( pSkill )
	{
		pSkill->dwLevel = dwSkillLevel;
	}
	else
	{
		return FALSE;
	}
	
	pUser->AddSetSkill( pSkill->dwSkill, pSkill->dwLevel );
#endif // __WORLDSERVER
#ifdef __CLIENT

	DWORD dwSkillLevel = scanner.GetNumber();

#if __VER >= 10 // __CSC_VER9_1
#ifdef __NEW_SKILL_TREE
	CWndSkillTreeEx* pSkill = (CWndSkillTreeEx*)g_WndMng.GetWndBase( APP_SKILL4 );
#else // __NEW_SKILL_TREE
	CWndSkillTreeEx* pSkill = (CWndSkillTreeEx*)g_WndMng.GetWndBase( APP_SKILL3 );
#endif // __NEW_SKILL_TREE
#else
	CWndSkillTreeEx* pSkill = (CWndSkillTreeEx*)g_WndMng.GetWndBase( APP_SKILL1 );
#endif //__CSC_VER9_1
	if( pSkill )
	{
		int nIndex = pSkill->GetCurSelect();
		if( nIndex == -1 )
		{
			g_WndMng.PutString( prj.GetText(TID_GAME_CHOICESKILL), NULL, 0xffff0000 );
			//g_WndMng.PutString( "스킬창에 있는 스킬을 선택하여 주십시요", NULL, 0xffff0000 );
			return FALSE;
		}
		LPSKILL pSkillbuf = pSkill->GetSkill( nIndex );
		if( pSkillbuf == NULL ) 
		{
			g_WndMng.PutString( prj.GetText(TID_GAME_CHOICESKILL), NULL, 0xffff0000 );
			//g_WndMng.PutString( "스킬창에 있는 스킬을 선택하여 주십시요", NULL, 0xffff0000 );
			return FALSE;
		}

		ItemProp* pSkillProp = prj.GetSkillProp( pSkillbuf->dwSkill );

		if( pSkillProp->dwExpertMax < 1 || pSkillProp->dwExpertMax < dwSkillLevel )
		{
			char szMessage[MAX_PATH];
			sprintf ( szMessage, prj.GetText(TID_GAME_SKILLLEVELLIMIT), pSkillProp->szName, pSkillProp->dwExpertMax );
//			sprintf ( szMessage, "%s' 스킬은 1 ~ %d 로만 레벨을 올릴수 있습니다", pSkillProp->szName, pSkillProp->dwExpertMax );

			g_WndMng.PutString( szMessage, NULL, 0xffff0000 );
			return FALSE;
		}
		char szSkillLevel[MAX_PATH];
		sprintf( szSkillLevel, prj.GetText(TID_GAME_GAMETEXT001), pSkillbuf->dwSkill, dwSkillLevel );
		scanner.SetProg( szSkillLevel );		
		//sprintf( scanner.pBuf, "/스렙 %d %d", pSkillbuf->dwSkill, dwSkillLevel );
	}
	else
	{
		g_WndMng.PutString( prj.GetText(TID_GAME_CHOICESKILL), NULL, 0xffff0000 );
//		g_WndMng.PutString( "스킬창에 있는 스킬을 선택하여 주십시요", NULL, 0xffff0000 );
		return FALSE;
	}
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_SkillLevelAll( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;

	LPSKILL pSkill = NULL;
	ItemProp* pSkillProp = NULL;

	for( int i = 0; i < MAX_SKILL_JOB; i++ )	
	{
		pSkill = &(pUser->m_aJobSkill[i]);

		if( pSkill == NULL || pSkill->dwSkill == 0xffffffff )
			continue;

		pSkillProp = prj.GetSkillProp( pSkill->dwSkill );

		if( pSkillProp == NULL )
			continue;

		pSkill->dwLevel = pSkillProp->dwExpertMax;
		pUser->AddSetSkill( pSkill->dwSkill, pSkill->dwLevel );
	}
#endif // __WORLDSERVER

	return TRUE;
}

BOOL TextCmd_whisper( CScanner& scanner )              
{ 
#ifdef __WORLDSERVER
	static	CHAR	lpString[260];

	CUser* pUser	= (CUser*)scanner.dwValue;
	if( pUser->IsMode( SAYTALK_MODE ) )
		return TRUE;
#ifdef __JEFF_9_20
#if __VER >= 12 // __LORD
	int nText	= pUser->GetMuteText();
	if(  nText )
	{
		pUser->AddDefinedText( nText );
		return TRUE;
	}
#else	// __LORD
	if( pUser->IsMute() )
		return TRUE;
#endif	// __LORD
#endif	// __JEFF_9_20

	scanner.GetToken();

	if( strcmp( pUser->GetName(), scanner.Token ) )
	{
#if __VER >= 11 // __SYS_PLAYER_DATA
			u_long idPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerId( scanner.token );
#else	// __SYS_PLAYER_DATA
			u_long idPlayer	= prj.GetPlayerID( scanner.Token );
#endif	// __SYS_PLAYER_DATA
			if( idPlayer > 0 ) 
			{
				scanner.GetLastFull();
				if( strlen( scanner.token ) >= 260 )	
					return TRUE;
				strcpy( lpString, scanner.token );
				
				StringTrimRight( lpString );
				
				if( !(pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_FONTEDIT)))
				{
					ParsingEffect(lpString, strlen(lpString) );
				}
				RemoveCRLF( lpString );
				
				if( 0 < strlen(lpString) )
					g_DPCoreClient.SendWhisper( pUser->m_idPlayer, idPlayer, lpString );
			}
			else 
			{
				//scanner.Token라는 이름을 가진 사용자는 이 게임에 존재하지 않는다.
				pUser->AddReturnSay( 3, scanner.Token );
			}
	}
	else
	{
		pUser->AddReturnSay( 2, " " );  	// 자기 자신에게 명령했다.
	}
#endif	// __WORLDSERVER

	return TRUE;
}

BOOL TextCmd_say( CScanner& scanner )              
{ 
#ifdef __WORLDSERVER
	static	CHAR	lpString[1024];
	CUser* pUser	= (CUser*)scanner.dwValue;

	if( pUser->IsMode( SAYTALK_MODE ) )
		return TRUE;
#ifdef __JEFF_9_20
#if __VER >= 12 // __LORD
	int nText	= pUser->GetMuteText();
	if(  nText )
	{
		pUser->AddDefinedText( nText );
		return TRUE;
	}
#else	// __LORD
	if( pUser->IsMute() )
		return TRUE;
#endif	// __LORD
#endif	// __JEFF_9_20

	scanner.GetToken();
	if( strcmp( pUser->GetName(), scanner.Token ) )
	{
#if __VER >= 11 // __SYS_PLAYER_DATA
		u_long idPlayer		= CPlayerDataCenter::GetInstance()->GetPlayerId( scanner.token );
#else	// __SYS_PLAYER_DATA
		u_long idPlayer		= prj.GetPlayerID( scanner.Token );
#endif	// __SYS_PLAYER_DATA
		if( idPlayer > 0 ) 
		{
			scanner.GetLastFull();
			if( strlen( scanner.token ) > 260 )	//
				return TRUE;

			strcpy( lpString, scanner.token );

			StringTrimRight( lpString );
			
			if( !(pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_FONTEDIT)))
			{
				ParsingEffect(lpString, strlen(lpString) );
			}
			RemoveCRLF( lpString );
			
			g_DPCoreClient.SendSay( pUser->m_idPlayer, idPlayer, lpString );
		}
		else 
		{
			//scanner.Token라는 이름을 가진 사용자는 이 게임에 존재하지 않는다.
			pUser->AddReturnSay( 3, scanner.Token );
		}
	}
	else
	{
		pUser->AddReturnSay( 2, " " );  	// 자기 자신에게 명령했다.
	}
	
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_ResistItem( CScanner& scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;

	DWORD dwObjId	= scanner.GetNumber();
	BYTE bItemResist = scanner.GetNumber();
	int nResistAbilityOption = scanner.GetNumber();
	int nAbilityOption	= scanner.GetNumber();

	if( bItemResist < 0 || 5 < bItemResist )
	{
		return FALSE;
	}
#if __VER >= 13 // __EXT_ENCHANT
#ifdef __NEW_ITEM_VARUNA
	if( nResistAbilityOption < 0 || 20 < nResistAbilityOption || nAbilityOption < 0 || 20 < nAbilityOption )
#else // __NEW_ITEM_VARUNA
	if( nResistAbilityOption < 0 || CItemUpgrade::GetInstance()->GetMaxAttributeEnchantSize() < nResistAbilityOption 
		|| nAbilityOption < 0 || CItemUpgrade::GetInstance()->GetMaxGeneralEnchantSize() < nAbilityOption )
#endif // __NEW_ITEM_VARUNA
#else // __EXT_ENCHANT
	if( nResistAbilityOption < 0 || 10 < nResistAbilityOption || nAbilityOption < 0 || 10 < nAbilityOption )
#endif // __EXT_ENCHANT
	{
		return FALSE;
	}

	if( bItemResist == 0 )
	{
		nResistAbilityOption = 0;
	}

	CItemElem* pItemElem0	= pUser->m_Inventory.GetAtId( dwObjId );
	if( NULL == pItemElem0 )
		return FALSE;
	if( pUser->m_Inventory.IsEquip( dwObjId ) )
	{
		pUser->AddDefinedText( TID_GAME_EQUIPPUT , "" );
		return FALSE;
	}
	
	pUser->UpdateItem( (BYTE)( pItemElem0->m_dwObjId ), UI_IR,  bItemResist );
	pUser->UpdateItem( (BYTE)( pItemElem0->m_dwObjId ), UI_RAO,  nResistAbilityOption );
	pUser->UpdateItem( (BYTE)( pItemElem0->m_dwObjId ), UI_AO,  nAbilityOption );
#if __VER >= 9 // __ULTIMATE
	if( nAbilityOption > 5 && pItemElem0->GetProp()->dwReferStat1 == WEAPON_ULTIMATE )
#if __VER >= 12 // __EXT_PIERCING
		pUser->UpdateItem( (BYTE)pItemElem0->m_dwObjId, UI_ULTIMATE_PIERCING_SIZE, nAbilityOption - 5 );
#else // __EXT_PIERCING
		pUser->UpdateItem( (BYTE)pItemElem0->m_dwObjId, UI_PIERCING_SIZE, nAbilityOption - 5 );
#endif // __EXT_PIERCING
#endif //__ULTIMATE
#else // __WORLDSEVER
#ifdef __CLIENT
	if( g_WndMng.m_pWndUpgradeBase == NULL )
	{
		SAFE_DELETE( g_WndMng.m_pWndUpgradeBase );
		g_WndMng.m_pWndUpgradeBase = new CWndUpgradeBase;
		g_WndMng.m_pWndUpgradeBase->Initialize( &g_WndMng, APP_TEST );
		return FALSE;
	}

	BYTE bItemResist = scanner.GetNumber();
	int nResistAbilityOption = scanner.GetNumber();
	int nAbilityOption	= scanner.GetNumber();
	if( bItemResist < 0 || 5 < bItemResist )
	{
		return FALSE;
	}
#if __VER >= 13 // __EXT_ENCHANT
#ifdef __NEW_ITEM_VARUNA
	if( nResistAbilityOption < 0 || 20 < nResistAbilityOption || nAbilityOption < 0 || 20 < nAbilityOption )
#else // __NEW_ITEM_VARUNA
	if( nResistAbilityOption < 0 || 20 < nResistAbilityOption || nAbilityOption < 0 || 10 < nAbilityOption )
#endif // __NEW_ITEM_VARUNA
#else	// __EXT_ENCHANT
	if( nResistAbilityOption < 0 || 10 < nResistAbilityOption || nAbilityOption < 0 || 10 < nAbilityOption )
#endif	// __EXT_ENCHANT
	{
		return FALSE;
	}

	if( bItemResist == 0 )
	{
		nResistAbilityOption = 0;
	}

	if( g_WndMng.m_pWndUpgradeBase )
	{
		if( g_WndMng.m_pWndUpgradeBase->m_pItemElem[0] )
		{
			DWORD dwObjId = g_WndMng.m_pWndUpgradeBase->m_pItemElem[0]->m_dwObjId;
			char szSkillLevel[MAX_PATH];
			sprintf( szSkillLevel, "/ritem %d %d %d %d", dwObjId, bItemResist, nResistAbilityOption, nAbilityOption );
			scanner.SetProg( szSkillLevel );		
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
#endif // __CLIENT
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_CommercialElem( CScanner& scanner )
{
#ifdef __CLIENT
	SAFE_DELETE( g_WndMng.m_pWndCommerialElem );
	g_WndMng.m_pWndCommerialElem = new CWndCommercialElem;
	g_WndMng.m_pWndCommerialElem->Initialize( &g_WndMng, APP_COMMERCIAL_ELEM );
#endif // __CLIENT
	return FALSE;
}

BOOL TextCmd_Piercing( CScanner& scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	
	DWORD dwObjId	= scanner.GetNumber();
	BYTE bPierNum = scanner.GetNumber();

	CItemElem* pItemElem0	= pUser->m_Inventory.GetAtId( dwObjId );
	if( NULL == pItemElem0 )
		return FALSE;

	if( bPierNum < 0 || bPierNum > MAX_PIERCING )
		return FALSE;

	if( pUser->m_Inventory.IsEquip( dwObjId ) )
	{
		pUser->AddDefinedText( TID_GAME_EQUIPPUT , "" );
		return FALSE;
	}

	if( bPierNum < pItemElem0->GetPiercingSize() )
	{
		pUser->UpdateItem( (BYTE)pItemElem0->m_dwObjId, UI_PIERCING_SIZE, bPierNum );
		return TRUE;
	}

	for( int i=1; i<=bPierNum; i++ )
	{
#if __VER >= 12 // __EXT_PIERCING
		if( pItemElem0->IsPierceAble( NULL_ID, TRUE ) )
#endif // __EXT_PIERCING
			pUser->UpdateItem( (BYTE)pItemElem0->m_dwObjId, UI_PIERCING_SIZE, i );
	}
#else // __WORLDSEVER
#ifdef __CLIENT
	BYTE bPierNum = scanner.GetNumber();

	if( bPierNum < 0 || bPierNum > MAX_PIERCING )
	{
		return FALSE;
	}	

	CWndPiercing* pWndPiercing = (CWndPiercing*)g_WndMng.GetWndBase(APP_PIERCING);
	if( pWndPiercing )
	{
		if( pWndPiercing->m_pItemElem[0] )
		{
			DWORD dwObjId = pWndPiercing->m_pItemElem[0]->m_dwObjId;
			char szSkillLevel[MAX_PATH];
			sprintf( szSkillLevel, "/pier %d %d", dwObjId, bPierNum );
			scanner.SetProg( szSkillLevel );		
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
#endif // __CLIENT
#endif // __WORLDSERVER
	return TRUE;
}

BOOL IsRight( CString &str )
{
	if( str.GetLength() == 1 )
	{
		if( str.Find( "#", 0 ) == 0 )
			return FALSE;
	}
	else
	if( str.GetLength() == 2 )
	{
		if( str.Find( "#u", 0 ) == 0 )
			return FALSE;
		
		if( str.Find( "#b", 0 ) == 0 )
			return FALSE;
	}
	
	int nFind = str.Find( "#c", 0 );	
	if( nFind != -1 )
	{
		if( (str.GetLength() - nFind) < 8 )
			return FALSE;
	}
		
		return TRUE;
}

BOOL TextCmd_shout( CScanner& scanner )            
{ 
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	if( pUser->IsMode( SHOUTTALK_MODE ) )
		return FALSE;
#ifdef __QUIZ
	if( pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_QUIZ )
		return FALSE;
#endif // __QUIZ
#ifdef __JEFF_9_20
#if __VER >= 12 // __LORD
	int nText	= pUser->GetMuteText();
	if(  nText )
	{
		pUser->AddDefinedText( nText );
		return FALSE;
	}
#else	// __LORD
	if( pUser->IsMute() )
		return FALSE;
#endif	// __LORD
	if( ( // 미국 & 유럽
#ifdef __NO_SUB_LANG
		( ::GetLanguage() == LANG_USA )
		|| ( ::GetLanguage() == LANG_ID )
#else // __NO_SUB_LANG
		( ::GetLanguage() == LANG_USA && ::GetSubLanguage() == LANG_SUB_USA )
		|| ( ::GetLanguage() == LANG_USA && ::GetSubLanguage() == LANG_SUB_IND )
#endif // __NO_SUB_LANG		
		|| ::GetLanguage() == LANG_GER
		|| ::GetLanguage() == LANG_FRE
		|| ::GetLanguage() == LANG_VTN
		|| ::GetLanguage() == LANG_POR
		|| ::GetLanguage() == LANG_RUS
		)
		&& pUser->GetLevel() < 20 )
	{
		pUser->AddDefinedText( TID_GAME_CANT_SHOUT_BELOW_20 );
		return FALSE;
	}
#endif	// __JEFF_9_20

	if( ( GetLanguage() == LANG_TWN || GetLanguage() == LANG_HK )  && pUser->GetLevel() < 15 )
	{
		pUser->AddDefinedText( TID_GAME_CANNOT_SHOUT_BELOW_15_LEVEL );
		return FALSE;
	}

	char szString[1024];
	szString[0] = '\0';

	scanner.GetLastFull();
	if( strlen( scanner.token ) > 260 )	//
		return TRUE;

	strcpy( szString, scanner.token );
	StringTrimRight( szString );

	if( !(pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_FONTEDIT)))
	{
		ParsingEffect( szString, strlen(szString) );
	}
	RemoveCRLF( szString );

	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_SHOUT;
	arBlock << GETID( pUser );
	arBlock.WriteString(pUser->GetName());
	arBlock.WriteString( szString );
#if __VER >= 12 // __LORD
	DWORD dwColor	= 0xffff99cc;
	if( pUser->HasBuff(  BUFF_ITEM, II_SYS_SYS_LS_SHOUT ) )
		dwColor		= 0xff00ff00;
	arBlock << dwColor;
#endif	// __LORD
	GETBLOCK( arBlock, lpBlock, uBlockSize );

#if  __VER >= 13
	int nRange = 0x000000ff;
	if( pUser->IsShoutFull() )
		nRange = 0;
	g_UserMng.AddShout( pUser, nRange, lpBlock, uBlockSize );
#else // __VER >= 13
	if( pUser->IsShoutFull() )	// 여기서 유료 아이템 사용중인지 확인
		g_UserMng.AddShout( pUser->GetPos(), 0, lpBlock, uBlockSize );
	else
		g_UserMng.AddShout( pUser->GetPos(), 0xff, lpBlock, uBlockSize );
#endif // __VER >= 13

#else	// __WORLDSERVER
#ifdef __CLIENT

	static CTimer timerDobe;
	static CString stringBack;
	static CTimeLimit timeLimit( g_Neuz.m_nShoutLimitCount, g_Neuz.m_dwShoutLimitSecond );

	CString string = scanner.m_pProg; 

	CString strTrim = string;
	strTrim.TrimLeft();
		
	if( IsRight( strTrim ) == FALSE )
		return FALSE;

#ifdef __BAN_CHATTING_SYSTEM
	if( g_WndMng.GetBanningTimer().IsTimeOut() == FALSE )
	{
		CWndChat* pWndChat = ( CWndChat* )g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
		if( pWndChat )
		{
			int nOriginalSecond = static_cast< int >( CWndMgr::BANNING_MILLISECOND - static_cast< int >( g_WndMng.GetBanningTimer().GetLeftTime() ) ) / 1000;
			int nMinute = static_cast< int >( nOriginalSecond / 60 );
			int nSecond = static_cast< int >( nOriginalSecond % 60 );
			CString strMessage = _T( "" );
			// 현재 채팅 금지 페널티를 받고 있습니다. (남은 시간: %d분 %d초)
			strMessage.Format( prj.GetText( TID_GAME_ERROR_CHATTING_3 ), nMinute, nSecond );
			pWndChat->PutString( strMessage, 0xffff0000 );
			return FALSE;
		}
	}
	else
	{
		if( stringBack != string || timerDobe.TimeOut() )
		{
			if( !g_pPlayer->IsShoutFull()
#ifdef __NO_SUB_LANG
				|| ( ::GetLanguage() == LANG_USA )
#else // __NO_SUB_LANG
				|| ( ::GetLanguage() == LANG_USA && ::GetSubLanguage() == LANG_SUB_USA )
				|| ( ::GetLanguage() == LANG_USA && ::GetSubLanguage() == LANG_SUB_IND )
#endif // __NO_SUB_LANG
				|| ::GetLanguage() == LANG_GER
				|| ::GetLanguage() == LANG_FRE
				|| ::GetLanguage() == LANG_JAP
				|| ::GetLanguage() == LANG_VTN
				|| ::GetLanguage() == LANG_POR
				|| ::GetLanguage() == LANG_RUS
				)
			{
				if( g_Neuz.m_nShoutLimitCount > 0 && timeLimit.Check() == TRUE ) // 제한을 넘었다.
				{
					//외치기는 %n초안에 %d번만 허용됩니다.
					char szMsg[256];
					sprintf( szMsg, prj.GetText( TID_GAME_LIMIT_SHOUT ), g_Neuz.m_dwShoutLimitSecond / 1000, g_Neuz.m_nShoutLimitCount );
					g_WndMng.PutString( szMsg, NULL, prj.GetTextColor( TID_GAME_LIMIT_SHOUT ) );
					return FALSE;
				}
			}

			stringBack = string;
			timerDobe.Set( SEC( 3 ) );
			g_WndMng.WordChange( string );

			if( g_WndMng.IsShortcutCommand() == TRUE )
			{
				if( g_WndMng.GetShortcutWarningTimer().IsTimeOut() == FALSE )
				{
					g_WndMng.SetWarningCounter( g_WndMng.GetWarningCounter() + 1 );
					CWndChat* pWndChat = ( CWndChat* )g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
					if( pWndChat )
					{
						if( g_WndMng.GetWarningCounter() >= CWndMgr::BANNING_POINT )
						{
							// 과도한 채팅으로 인하여 %d분 동안 채팅 금지 페널티를 받으셨습니다.
							CString strChattingError1 = _T( "" );
							strChattingError1.Format( prj.GetText( TID_GAME_ERROR_CHATTING_2 ), CWndMgr::BANNING_MILLISECOND / 1000 / 60 );
							pWndChat->PutString( strChattingError1, prj.GetTextColor( TID_GAME_ERROR_CHATTING_2 ) );
							g_WndMng.SetWarningCounter( 0 );
							g_WndMng.GetBanningTimer().Reset();
						}
						else
						{
							// 연속 채팅으로 인하여 메시지가 출력되지 않았습니다.
							pWndChat->PutString( prj.GetText( TID_GAME_ERROR_CHATTING_1 ), 0xffff0000 );
						}
					}
				}
				else
				{
					CString strCommand = _T( "" );
					strCommand.Format( "/s %s", string );
					g_DPlay.SendChat( strCommand );
				}
				g_WndMng.GetShortcutWarningTimer().Reset();
			}
			else
			{
				if( g_WndMng.GetWarningTimer().IsTimeOut() == FALSE )
				{
					g_WndMng.SetWarningCounter( g_WndMng.GetWarningCounter() + 1 );
					CWndChat* pWndChat = ( CWndChat* )g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
					if( pWndChat )
					{
						if( g_WndMng.GetWarningCounter() >= CWndMgr::BANNING_POINT )
						{
							// 과도한 채팅으로 인하여 %d분 동안 채팅 금지 페널티를 받으셨습니다.
							CString strChattingError1 = _T( "" );
							strChattingError1.Format( prj.GetText( TID_GAME_ERROR_CHATTING_2 ), CWndMgr::BANNING_MILLISECOND / 1000 / 60 );
							pWndChat->PutString( strChattingError1, prj.GetTextColor( TID_GAME_ERROR_CHATTING_2 ) );
							g_WndMng.SetWarningCounter( 0 );
							g_WndMng.GetBanningTimer().Reset();
						}
						else
						{
							// 연속 채팅으로 인하여 메시지가 출력되지 않았습니다.
							pWndChat->PutString( prj.GetText( TID_GAME_ERROR_CHATTING_1 ), 0xffff0000 );
						}
					}
				}
				else
				{
					if( g_WndMng.GetWarning2Timer().IsTimeOut() == FALSE )
					{
						g_WndMng.SetWarning2Counter( g_WndMng.GetWarning2Counter() + 1 );
						CWndChat* pWndChat = ( CWndChat* )g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
						if( pWndChat )
						{
							if( g_WndMng.GetWarning2Counter() >= CWndMgr::BANNING_2_POINT )
							{
								// 과도한 채팅으로 인하여 %d분 동안 채팅 금지 페널티를 받으셨습니다.
								CString strChattingError1 = _T( "" );
								strChattingError1.Format( prj.GetText( TID_GAME_ERROR_CHATTING_2 ), CWndMgr::BANNING_MILLISECOND / 1000 / 60 );
								pWndChat->PutString( strChattingError1, prj.GetTextColor( TID_GAME_ERROR_CHATTING_2 ) );
								g_WndMng.SetWarning2Counter( 0 );
								g_WndMng.GetBanningTimer().Reset();
							}
							else
							{
								CString strCommand = _T( "" );
								strCommand.Format( "/s %s", string );
								g_DPlay.SendChat( strCommand );
							}
						}
					}
					else
					{
						CString strCommand = _T( "" );
						strCommand.Format( "/s %s", string );
						g_DPlay.SendChat( strCommand );
					}
				}
				g_WndMng.GetWarningTimer().Reset();
				g_WndMng.GetWarning2Timer().Reset();
			}
		}
		else
		{
			CWndChat* pWndChat = (CWndChat*)g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
			if( pWndChat )
				g_WndMng.PutString( prj.GetText( TID_GAME_CHATSAMETEXT ), NULL, prj.GetTextColor( TID_GAME_CHATSAMETEXT ) );
		}
	}
#else // __BAN_CHATTING_SYSTEM
	if( stringBack != string || timerDobe.TimeOut() )
	{
		if( !g_pPlayer->IsShoutFull()
			|| ( ::GetLanguage() == LANG_USA && ::GetSubLanguage() == LANG_SUB_USA )
			|| ( ::GetLanguage() == LANG_USA && ::GetSubLanguage() == LANG_SUB_IND )
			|| ::GetLanguage() == LANG_GER
			|| ::GetLanguage() == LANG_FRE
			|| ::GetLanguage() == LANG_JAP
			|| ::GetLanguage() == LANG_VTN
			|| ::GetLanguage() == LANG_POR
			|| ::GetLanguage() == LANG_RUS
		)
			if( g_Neuz.m_nShoutLimitCount > 0 && timeLimit.Check() == TRUE ) // 제한을 넘었다.
			{
				//외치기는 %n초안에 %d번만 허용됩니다.
				char szMsg[256];
				sprintf( szMsg, prj.GetText( TID_GAME_LIMIT_SHOUT ), g_Neuz.m_dwShoutLimitSecond / 1000, g_Neuz.m_nShoutLimitCount );
				g_WndMng.PutString( szMsg, NULL, prj.GetTextColor( TID_GAME_LIMIT_SHOUT ) );
				return FALSE;
			}

		stringBack = string;
		timerDobe.Set( SEC( 3 ) );
		g_WndMng.WordChange( string );

		CString strCommand;
		strCommand.Format( "/s %s", string );
		g_DPlay.SendChat( strCommand );
	}
	else
	{
		CWndChat* pWndChat = (CWndChat*)g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
		if( pWndChat )
			g_WndMng.PutString( prj.GetText( TID_GAME_CHATSAMETEXT ), NULL, prj.GetTextColor( TID_GAME_CHATSAMETEXT ) );
	}
#endif // __BAN_CHATTING_SYSTEM
	return FALSE;
#endif // __CLIENT
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_PartyChat( CScanner& scanner )            
{ 
#ifdef __WORLDSERVER
	static	\
		CHAR lpString[260];
	
	CUser* pUser	= (CUser*)scanner.dwValue;

#ifdef __JEFF_9_20
#if __VER >= 12 // __LORD
	int nText	= pUser->GetMuteText();
	if(  nText )
	{
		pUser->AddDefinedText( nText );
		return TRUE;
	}
#else	// __LORD
	if( pUser->IsMute() )
		return TRUE;
#endif	// __LORD
#endif	// __JEFF_9_20
	
	lpString[0] = '\0';
	
	scanner.GetLastFull();
	if( lstrlen( scanner.token ) >= 260 )
		return TRUE;
	strcpy( lpString, scanner.token );

	StringTrimRight( lpString );	

	if( !(pUser->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_FONTEDIT)))
	{
		ParsingEffect(lpString, strlen(lpString) );
	}
	RemoveCRLF( lpString );
	
	CParty* pParty;
	
	pParty	= g_PartyMng.GetParty( pUser->GetPartyId() );
	if( pParty && pParty->IsMember( pUser->m_idPlayer ))
	{
		// 파티가 있어서 파티원들에게 보냄
		g_DPCoreClient.SendPartyChat( pUser, lpString );
	}
	else
	{
		// 월드서버에서 파티가 없는경우
		pUser->AddSendErrorParty( ERROR_NOPARTY );
	}

	// 클라이언트에서 극단참여중이니즐 먼저 검색함
#else // __WORLDSERVER
#ifdef __CLIENT
	if( g_Party.GetSizeofMember() >= 2 )
	{
		CString string = scanner.m_pProg;
		g_WndMng.WordChange( string );

		if( ::GetLanguage() == LANG_THA )
			string = '"'+string+'"';
		
		CString strCommand;
		strCommand.Format( "/p %s", string );
		g_DPlay.SendChat( strCommand );
	}
	else
	{
		// 극단원이 아니므로 
		// 극단에 포함되지 않아 극단채팅을 할수 없습니다.
		//g_WndMng.PutString( "극단에 포함되지 않아 극단채팅을 할수 없습니다", NULL, 0xff99cc00 );
		g_WndMng.PutString( prj.GetText( TID_GAME_PARTYNOTCHAT ), NULL, prj.GetTextColor( TID_GAME_PARTYNOTCHAT ) );
		
	}
	return FALSE;
#endif // __CLIENT
#endif // __WORLDSERVER
	return TRUE;
}


BOOL TextCmd_Music( CScanner& scanner )            
{ 
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;

	u_long idmusic	= scanner.GetNumber();
	g_DPCoreClient.SendPlayMusic( pUser->GetWorld()->GetID(), idmusic );
#endif	// __WORLDSERVER
	return TRUE;
}
BOOL TextCmd_Sound( CScanner& scanner )            
{ 
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	u_long idsound	= scanner.GetNumber();
	g_DPCoreClient.SendPlaySound( pUser->GetWorld()->GetID(), idsound );
#endif	// __WORLDSERVER
	return TRUE;
}
BOOL TextCmd_Summon( CScanner& scanner )           
{
#ifdef __WORLDSERVER
	TCHAR lpszPlayer[32];

	scanner.GetToken();
	CUser* pUser	= (CUser*)scanner.dwValue;
	if( strcmp( pUser->GetName(), scanner.Token) )
	{
#if __VER >= 11 // __SYS_PLAYER_DATA
		u_long idPlayer		= CPlayerDataCenter::GetInstance()->GetPlayerId( scanner.token );
#else	// __SYS_PLAYER_DATA
		u_long idPlayer		= prj.GetPlayerID( scanner.Token );
#endif	// __SYS_PLAYER_DATA
		if( idPlayer > 0 ){
			strcpy( lpszPlayer, scanner.Token );
#ifdef __SECURITY_FIXES
		CUser* pUserTarget = static_cast<CUser*>( prj.GetUserByID( idPlayer ) );
		if( pUserTarget )
			pUserTarget->REPLACE( g_uIdofMulti, pUser->GetWorld()->GetID(),  pUser->GetPos(), REPLACE_FORCE,  pUser->GetLayer() );
#else// __SECURITY_FIXES
#ifdef __LAYER_1015
			g_DPCoreClient.SendSummonPlayer( pUser->m_idPlayer, pUser->GetWorld()->GetID(), pUser->GetPos(), idPlayer, pUser->GetLayer() );
#else	// __LAYER_1015
			g_DPCoreClient.SendSummonPlayer( pUser->m_idPlayer, pUser->GetWorld()->GetID(), pUser->GetPos(), idPlayer );
#endif	// __LAYER_1015
#endif // __SECURITY_FIXES
		}
		else {
//			scanner.Token라는 이름을 가진 사용자는 이 게임에 존재하지 않는다.
			pUser->AddReturnSay( 3, scanner.Token );
		}
	}
	else 
	{
		pUser->AddReturnSay( 2, " " );  		// 자기 자신에게 명령했다.		
	}
	
#endif	// __WORLDSERVER
	return TRUE;
}

#ifdef __PET_1024
BOOL TextCmd_ClearPetName( CScanner & s )
{
#ifdef __CLIENT
	g_DPlay.SendClearPetName();
#endif	// __CLIENT
	return TRUE;
}

BOOL TextCmd_SetPetName( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	CPet* pPet	= pUser->GetPet();
	if( !pPet )
		return TRUE;
	s.GetToken();
	pPet->SetName( s.token );
	g_UserMng.AddSetPetName( pUser, s.token );
#endif	// __WORLDSERVER
	return TRUE;
}
#endif	// __PET_1024

#ifdef __LAYER_1020
BOOL TextCmd_CreateLayer( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	DWORD dwWorld	= s.GetNumber();
	CWorld* pWorld	= g_WorldMng.GetWorld( dwWorld );
	if( pWorld )
	{
		int nLayer	= s.GetNumber();
//		pWorld->m_linkMap.CreateLinkMap( nLayer );
		pWorld->CreateLayer( nLayer );
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_DeleteLayer( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	DWORD dwWorld	= s.GetNumber();
	CWorld* pWorld	= g_WorldMng.GetWorld( dwWorld );
	if( pWorld )
	{
		int nLayer	= s.GetNumber();
//		pWorld->m_linkMap.DeleteLinkMap( nLayer );
		pWorld->ReleaseLayer( nLayer );		// do not call ReleaseLayer directly
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_Layer( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	DWORD dwWorld	= s.GetNumber();
	CWorld* pWorld	= g_WorldMng.GetWorld( dwWorld );
	if( pWorld )
	{
		int nLayer	= s.GetNumber();
		CLinkMap* pLinkMap	= pWorld->m_linkMap.GetLinkMap( nLayer );
		if( pLinkMap )
		{
			FLOAT x	= s.GetFloat();
			FLOAT z	= s.GetFloat();
			if( pWorld->VecInWorld( x, z ) && x > 0 && z > 0 )	
				pUser->REPLACE( g_uIdofMulti, pWorld->GetID(), D3DXVECTOR3( x, 0, z ), REPLACE_NORMAL, nLayer );
			else
				pUser->AddText( "OUT OF WORLD" );
		}
		else
		{
			pUser->AddText( "LAYER NO EXISTS" );
		}
	}
	else
	{
		pUser->AddText( "UNDEFINED WORLD" );
	}
#endif	// __WORLDSERVER
	return TRUE;
}
#endif	// __LAYER_1020

#if __VER >= 13 // __COUPLE_1117
#if __VER >= 13 // __COUPLE_1202
BOOL TextCmd_NextCoupleLevel( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	CCouple* pCouple	= CCoupleHelper::Instance()->GetCouple( pUser->m_idPlayer );
	if( pCouple )
	{
		if( pCouple->GetLevel() < CCouple::eMaxLevel )
		{
			int nExperience	= CCoupleProperty::Instance()->GetTotalExperience( pCouple->GetLevel() + 1 ) - pCouple->GetExperience();
			g_dpDBClient.SendQueryAddCoupleExperience( pUser->m_idPlayer, nExperience );
		}
		else
			pUser->AddText( "MAX COUPLE LEVEL" );
	}
	else
		pUser->AddText( "COUPLE NOT FOUND" );
#endif	// __WORLDSERVER
	return TRUE;
}
#endif	// __COUPLE_1202

BOOL TextCmd_Propose( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	s.GetToken();
	CCoupleHelper::Instance()->OnPropose( pUser, s.token );
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_Refuse( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	CCoupleHelper::Instance()->OnRefuse( pUser );
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_Couple( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	CCoupleHelper::Instance()->OnCouple( pUser );
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_Decouple( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	CCoupleHelper::Instance()->OnDecouple( pUser );
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_ClearPropose( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	g_dpDBClient.SendClearPropose();
#endif	// __WORLDSERVER
	return TRUE;
}
/*
BOOL TextCmd_CoupleState( CScanner & s )
{
#ifdef __CLIENT
	CCouple* pCouple	= CCoupleHelper::Instance()->GetCouple();
	if( pCouple )
	{
		char szText[200]	= { 0,};
		const char* pszPartner	= CPlayerDataCenter::GetInstance()->GetPlayerString( pCouple->GetPartner( g_pPlayer->m_idPlayer ) );
		if( !pszPartner )	pszPartner	= "";
		sprintf( szText, "%s is partner.", pszPartner );
		g_WndMng.PutString( szText );
	}
	else
	{
		g_WndMng.PutString( "null couple." );
	}
#endif	// __CLIENT
	return TRUE;
}
*/
#endif	// __COUPLE_1117

BOOL TextCmd_Teleport( CScanner& scanner )         
{ 
#ifdef __WORLDSERVER
	TCHAR *lpszPlayer = NULL;
	int x, z;
	CUser* pUser	= (CUser*)scanner.dwValue;

	// 플레이어에게 바로 텔레포트 
	int nTok = scanner.GetToken();
	if( nTok != NUMBER )
	{
		// player
#if __VER >= 11 // __SYS_PLAYER_DATA
		u_long idPlayer		= CPlayerDataCenter::GetInstance()->GetPlayerId( scanner.token );
#else	// __SYS_PLAYER_DATA
		u_long idPlayer	= prj.GetPlayerID( scanner.token );
#endif	// __SYS_PLAYER_DATA
#if __VER >= 14 // __INSTANCE_DUNGEON
		CUser* pUserTarget = static_cast<CUser*>( prj.GetUserByID( idPlayer ) );
		if( IsValidObj( pUserTarget ) )
		{
			CWorld* pWorld = pUserTarget->GetWorld();
			if( pWorld )
			{
				if( CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( pWorld->GetID() ) )
					if( pWorld != pUser->GetWorld() || pUser->GetLayer() != pUserTarget->GetLayer() )
						return TRUE;

				pUser->REPLACE( g_uIdofMulti, pWorld->GetID(), pUserTarget->GetPos(), REPLACE_NORMAL, pUserTarget->GetLayer() );
			}
		}
#else // __INSTANCE_DUNGEON
		if( idPlayer > 0 ) 
		{
			g_DPCoreClient.SendTeleportPlayer( pUser->m_idPlayer, idPlayer );
		}
#endif // __INSTANCE_DUNGEON
		else 
		{
		#ifdef _DEBUG
			// 플레이어를 못찾으면 NPC로 찾는다.
			CWorld* pWorld	= pUser->GetWorld();
			CMover* pMover = pWorld->FindMover( scanner.Token );
			if( pMover )
			{
				pUser->REPLACE( g_uIdofMulti, pWorld->GetID(), pMover->GetPos(), REPLACE_NORMAL, pMover->GetLayer() );
				return TRUE;
			}
		#endif // _DEBUG
			pUser->AddReturnSay( 3, scanner.m_mszToken );
		}
	}
	// 첫번째 파라메타는 월드 번호.
	DWORD dwWorldId = atoi( scanner.token );
#if __VER >= 14 // __INSTANCE_DUNGEON
	if( CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( dwWorldId ) )
	{
		if( pUser->GetWorld() && pUser->GetWorld()->GetID() != dwWorldId )
			return TRUE;
	}
#endif // __INSTANCE_DUNGEON
	if( g_WorldMng.GetWorldStruct( dwWorldId ) )
	{
		// 두번째 파라메타가 스트링이면 리젼 키
		if( scanner.GetToken() != NUMBER )
		{
			PRegionElem pRgnElem = g_WorldMng.GetRevivalPos( dwWorldId, scanner.token );
			if( NULL != pRgnElem )
				pUser->REPLACE( g_uIdofMulti, pRgnElem->m_dwWorldId, pRgnElem->m_vPos, REPLACE_NORMAL, nRevivalLayer );
		}
		// 스트링이 아니면 좌표 
		else
		{
			x = atoi( scanner.token );
			z = scanner.GetNumber();

			CWorld* pWorld = g_WorldMng.GetWorld( dwWorldId );
			if( pWorld && pWorld->VecInWorld( (FLOAT)( x ), (FLOAT)( z ) ) && x > 0 && z > 0 )
			{
				int nLayer	= pWorld == pUser->GetWorld()? pUser->GetLayer(): nDefaultLayer;
				pUser->REPLACE( g_uIdofMulti, dwWorldId, D3DXVECTOR3( (FLOAT)x, 0, (FLOAT)z ), REPLACE_NORMAL, nLayer );
			}
		}
	}
#endif // __WORLDSERVER
	return TRUE;
}
BOOL TextCmd_Out( CScanner& scanner )              
{ 
#ifdef __WORLDSERVER
//	TCHAR lpszPlayer[MAX_PLAYER];
	scanner.GetToken();

	CUser* pUser	= (CUser*)scanner.dwValue;
	if( strcmp( pUser->GetName(), scanner.Token) )
	{	
#if __VER >= 11 // __SYS_PLAYER_DATA
		u_long idPlayer		= CPlayerDataCenter::GetInstance()->GetPlayerId( scanner.token );
#else	// __SYS_PLAYER_DATA
		u_long idPlayer		= prj.GetPlayerID( scanner.Token );
#endif	// __SYS_PLAYER_DATA
		if( idPlayer > 0 ) {
			g_DPCoreClient.SendKillPlayer( pUser->m_idPlayer, idPlayer );
		}
		else {
//			scanner.Token라는 이름을 가진 사용자는 이 게임에 존재하지 않는다.
			pUser->AddReturnSay( 3, scanner.Token );
		}
	}
	else
	{
		pUser->AddReturnSay( 2, " " );  		// 자기 자신에게 명령했다.
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_RemoveNpc( CScanner& s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	OBJID objid	= (OBJID)s.GetNumber();

	CMover* pMover	= prj.GetMover( objid );
	if( IsValidObj( (CObj*)pMover ) )
	{
		if( pMover->IsNPC() )
			pMover->Delete();
	}
#endif	// __WORLDSERVER
	return TRUE;
}


BOOL TextCmd_CreateItem2( CScanner & s )
{
#ifdef __WORLDSERVER
	s.GetToken();
	ItemProp* pItemProp	= prj.GetItemProp( s.Token );
	if( pItemProp && pItemProp->dwItemKind3 != IK3_VIRTUAL )
	{
		int nRandomOptItemId	= s.GetNumber();
		PRANDOMOPTITEM pRandomOptItem	= CRandomOptItemGen::GetInstance()->GetRandomOptItem( nRandomOptItemId );
		if( pRandomOptItem )
		{
			CItemElem itemElem;
			itemElem.m_dwItemId		= pItemProp->dwID;
			itemElem.m_nItemNum	= 1;
			itemElem.m_nHitPoint	= -1;
			itemElem.SetRandomOpt( pRandomOptItem->nId );
			CUser* pUser	= (CUser*)s.dwValue;
			pUser->CreateItem( &itemElem );
		}
	}
	return TRUE;
#else	// __WORLDSERVER
	return TRUE;
#endif	// __WORLDSERVER
}

BOOL TextCmd_CreateItem( CScanner& scanner )       
{
	scanner.GetToken();

#ifdef __CLIENT
	// 클라이언트에서
	if( scanner.tok == FINISHED )
	{
		if( g_WndMng.GetWndBase( APP_ADMIN_CREATEITEM ) == NULL )
		{
			CWndAdminCreateItem* pWndAdminCreateItem = new CWndAdminCreateItem;
			pWndAdminCreateItem->Initialize();
		}
		return FALSE;
	}
	return TRUE;
#else   // __CLIENT
	DWORD dwNum;
	DWORD dwCharged		= 0;
	ItemProp* pProp	= NULL;

	if( scanner.tokenType == NUMBER )
		pProp	= prj.GetItemProp( _ttoi( scanner.Token ) );
	else
		pProp	= prj.GetItemProp( scanner.Token );

	if( pProp && pProp->dwItemKind3 != IK3_VIRTUAL )
	{
		if( pProp->dwItemKind3 == IK3_EGG && pProp->dwID != II_PET_EGG )	// 리어펫을 생성하려고 할 경우 "알"인 경우만 생성 가능하다.
			return TRUE;

		dwNum	= scanner.GetNumber();
		dwNum	= ( dwNum == 0? 1: dwNum );
		dwCharged	= scanner.GetNumber();
		dwCharged	= ( dwCharged == 0 ? 0 : 1 );
		
		CItemElem itemElem;
		itemElem.m_dwItemId		= pProp->dwID;
		itemElem.m_nItemNum		= (short)( dwNum );
		itemElem.m_nHitPoint	= -1;
		itemElem.m_bCharged		= dwCharged;

		CUser* pUser	= (CUser*)scanner.dwValue;
		pUser->CreateItem( &itemElem );
	}
#endif	// !__CLIENT 
	return TRUE;
}

BOOL TextCmd_LocalEvent( CScanner & s )
{
#ifdef __WORLDSERVER
	int id	= s.GetNumber();
	if( id != EVE_18 )	// 이 식별자는 18세 서버를 나타내는 식별자므로 운영자의 명령에 의한 설정 불가
	{
		BYTE nState		= (BYTE)s.GetNumber();
		if( g_eLocal.SetState( id, nState ) )		g_UserMng.AddSetLocalEvent( id, nState );
	}
#endif	// __WORLDSERVER
	return TRUE;
}

// 무버이름 캐릭터키 갯수 선공 
BOOL TextCmd_CreateChar( CScanner& scanner )       
{ 
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	D3DXVECTOR3 vPos = pUser->GetPos();
	CWorld* pWorld	= pUser->GetWorld();
	
	MoverProp* pMoverProp	= NULL;

	scanner.GetToken();
	if( scanner.tokenType == NUMBER ) 
	{
		DWORD dwID	= _ttoi( scanner.Token );
		pMoverProp = prj.GetMoverPropEx( dwID );
	}
	else
		pMoverProp	= prj.GetMoverProp( scanner.Token );

	scanner.GetToken();
	CString strName = scanner.Token;

	if( pMoverProp && pMoverProp->dwID != 0 )
	{
		DWORD dwNum	= scanner.GetNumber();
		if( dwNum > 100 ) dwNum = 100;
		if( dwNum == 0 ) dwNum = 1;

		BOOL bActiveAttack = scanner.GetNumber();
		for( DWORD dw = 0; dw < dwNum; dw++ )
		{
			CMover* pMover = (CMover*)CreateObj( D3DDEVICE, OT_MOVER, pMoverProp->dwID );
			if( NULL == pMover ) return FALSE; // ASSERT( pObj );
			strcpy( pMover->m_szCharacterKey, strName );
			pMover->InitNPCProperty();
			pMover->InitCharacter( pMover->GetCharacter() );
			pMover->SetPos( vPos );
			pMover->InitMotion( MTI_STAND );
			pMover->UpdateLocalMatrix();
			if( bActiveAttack )
				pMover->m_bActiveAttack = bActiveAttack;
			pWorld->ADDOBJ( pMover, TRUE, pUser->GetLayer() );
		}
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_CreateCtrl( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	D3DXVECTOR3 vPos	= pUser->GetPos();
	CWorld* pWorld	= pUser->GetWorld();

	DWORD dwID	= s.GetNumber();
	
	if( dwID == 0 )
		return FALSE;

	CCtrl* pCtrl	= (CCtrl*)CreateObj( D3DDEVICE, OT_CTRL, dwID );
	if( !pCtrl )
		return FALSE;

	pCtrl->SetPos( vPos );
	pWorld->ADDOBJ( pCtrl, TRUE, pUser->GetLayer() );
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_PostMail( CScanner & s )
{
#ifdef __CLIENT
	DWORD dwIndex	= s.GetNumber();
	short nItemNum	= s.GetNumber();
	char lpszReceiver[MAX_PLAYER]	= { 0, };
	s.GetToken();
	lstrcpy( lpszReceiver, s.Token );
	int nGold	= s.GetNumber();
	char lpszTitle[MAX_MAILTITLE]	= { 0, };
	s.GetToken();
	lstrcpy( lpszTitle, s.Token );
	char lpszText[MAX_MAILTEXT]	= { 0, };
	s.GetToken();
	lstrcpy( lpszText, s.Token );

	CItemElem* pItemElem	= g_pPlayer->m_Inventory.GetAt( dwIndex );
	if( pItemElem )
	{
		g_DPlay.SendQueryPostMail( (BYTE)( pItemElem->m_dwObjId ), nItemNum, lpszReceiver, nGold, lpszTitle, lpszText );
	}
#endif	// __CLIENT
	return TRUE;
}

BOOL TextCmd_RemoveMail( CScanner & s )
{
#ifdef __CLIENT
	u_long nMail	= s.GetNumber();
	g_DPlay.SendQueryRemoveMail( nMail );
#endif	// __CLIENT
	return TRUE;
}

BOOL TextCmd_GetMailItem( CScanner & s )
{
#ifdef __CLIENT
	u_long nMail	= s.GetNumber();
	g_DPlay.SendQueryGetMailItem( nMail );
#endif	// __CLIENT
	return TRUE;
}

BOOL TextCmd_GetMailGold( CScanner & s )
{
#ifdef __CLIENT
	u_long nMail	= s.GetNumber();
	g_DPlay.SendQueryGetMailGold( nMail );
#endif	// __CLIENT
	return TRUE;
}

#if __VER >= 9 // __EVENTLUA
BOOL TextCmd_Lua( CScanner& s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	s.GetToken();
	s.Token.MakeLower();
		
	if( s.Token == "event" )
	{
		pUser->AddText( "Event.lua Reload..." );
		Error( "Event.lua Reload... - %s", pUser->GetName() );
		g_dpDBClient.SendEventLuaChanged();
	}
#if __VER >= 12 // __MONSTER_SKILL
	else if( s.Token == "ms" )
	{
		CMonsterSkill::GetInstance()->Clear();
		CMonsterSkill::GetInstance()->LoadScript();
	}
#endif // __MONSTER_SKILL
#if __VER >= 13 // __RAINBOW_RACE
	else if( s.Token == "rr" )
	{
		CRainbowRaceMng::GetInstance()->LoadScript();
	}
#endif // __RAINBOW_RACE
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_LuaEventList( CScanner& s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	prj.m_EventLua.GetAllEventList( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_LuaEventInfo( CScanner& s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	prj.m_EventLua.GetEventInfo( pUser, s.GetNumber() );
#endif // __WORLDSERVER
	return TRUE;
}
#endif // __EVENTLUA

#ifdef __JEFF_9_20
BOOL TextCmd_Mute( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	s.GetToken();
	if( s.tok == FINISHED )
		return TRUE;
#if __VER >= 11 // __SYS_PLAYER_DATA
	u_long idPlayer		= CPlayerDataCenter::GetInstance()->GetPlayerId( s.token );
#else	// __SYS_PLAYER_DATA
	u_long uidPlayer	= prj.GetPlayerID( s.token );
#endif	// __SYS_PLAYER_DATA
	
	if( idPlayer == 0 )
	{
		pUser->AddText( "player not found" );
		return TRUE;
	}
	CUser* pTarget	= g_UserMng.GetUserByPlayerID( idPlayer );
	if( IsValidObj( pTarget ) )
	{
		DWORD dwMute	= (DWORD)s.GetNumber();
		if( s.tok == FINISHED )
			return TRUE;
		pTarget->m_dwMute	= dwMute;
	}
#endif	// __WORLDSERVER
	return TRUE;
}
#endif	// __JEFF_9_20

#if __VER >= 8 // __CSC_VER8_5
BOOL TextCmd_AngelExp( CScanner& s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	int nAddExp	= s.GetNumber();

	if( pUser->HasBuffByIk3( IK3_ANGEL_BUFF ) )
	{
		int nAngel = 100;
	#ifdef __BUFF_1107
		IBuff* pBuff	= pUser->m_buffs.GetBuffByIk3( IK3_ANGEL_BUFF );
		WORD wId	= ( pBuff? pBuff->GetId(): 0 );
	#else	// __BUFF_1107
		LPSKILLINFLUENCE lpSkillIn = pUser->m_SkillState.GetItemBuf( IK3_ANGEL_BUFF );
		WORD wId	= ( lpSkillIn? lpSkillIn->wID: 0 );
	#endif	// __BUFF_1107
		if( wId )
		{
			ItemProp* pItemProp = prj.GetItemProp( wId );
			if( pItemProp )
				nAngel = (int)( (float)pItemProp->nAdjParamVal1 );
		}
		if( nAngel <= 0 || 100 < nAngel  )
			nAngel = 100;

		EXPINTEGER nMaxAngelExp = prj.m_aExpCharacter[pUser->m_nAngelLevel].nExp1 / 100 * nAngel;
		if( pUser->m_nAngelExp < nMaxAngelExp )
		{
			pUser->m_nAngelExp += nAddExp;
			BOOL bAngelComplete = FALSE;
			if( pUser->m_nAngelExp > nMaxAngelExp )
			{
				pUser->m_nAngelExp = nMaxAngelExp;
				bAngelComplete = TRUE;
			}
			pUser->AddAngelInfo( bAngelComplete );
		}
	}
#endif // __WORLDSERVER
	return TRUE;	
}
#endif // __CSC_VER8_5

#ifdef __EVENT_1101
BOOL TextCmd_CallTheRoll( CScanner& s )  
{ 
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)s.dwValue;
	int nCount	= s.GetNumber();
#ifdef __EVENT_1101_2
	pUser->m_nEventFlag = 0;
#else // __EVENT_1101_2
	pUser->m_dwEventFlag	= 0;
#endif // __EVENT_1101_2
	pUser->m_dwEventTime	= 0;
	pUser->m_dwEventElapsed	= 0;
	for(  int i = 0; i < nCount; i++ )
#ifdef __EVENT_1101_2
		pUser->SetEventFlagBit( 62 - i );
#else // __EVENT_1101_2
		pUser->SetEventFlagBit( 31 - i );
#endif // __EVENT_1101_2
#endif	// __WORLDSERVER
	return TRUE;
}

#endif	// __EVENT_1101

BOOL TextCmd_CreatePc( CScanner& scanner )  
{ 
#ifdef __PERF_0226
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	int nNum = scanner.GetNumber();
	for( int i=0; i<nNum; i++ )
	{
		int nSex	= xRandom( 2 );
		DWORD dwIndex	= ( nSex == SEX_FEMALE? MI_FEMALE: MI_MALE );

		CMover* pMover	= (CMover*)CreateObj( D3DDEVICE, OT_MOVER, dwIndex );
		if( NULL == pMover )	
			return FALSE;
		pMover->SetPos( pUser->GetPos() );
		pMover->InitMotion( MTI_STAND );
		pMover->UpdateLocalMatrix();
		SAFE_DELETE( pMover->m_pAIInterface );
		pMover->SetAIInterface( AII_MONSTER );
		pMover->m_Inventory.SetItemContainer( ITYPE_ITEM, MAX_INVENTORY, MAX_HUMAN_PARTS ); 

		static DWORD adwParts[5]	= {	PARTS_CAP, PARTS_HAND, PARTS_UPPER_BODY, PARTS_FOOT, PARTS_RWEAPON };
		for( int i = 0; i < 5; i++ )
		{
			CItemElem itemElem;
			ItemProp* pProp	= CPartsItem::GetInstance()->GetItemProp( ( i == 4? SEX_SEXLESS: nSex ), adwParts[i] );
			if( pProp )
			{
				CItemElem	itemElem;
				itemElem.m_dwItemId	= pProp->dwID;
				itemElem.m_nItemNum	= 1;
				itemElem.SetAbilityOption( xRandom( 10 ) );
				BYTE nId, nCount;
				short nNum;
				pMover->m_Inventory.Add( &itemElem, &nId, &nNum, &nCount );
				CItemElem* pAddItem	= pMover->m_Inventory.GetAtId( nId );
				pMover->m_Inventory.DoEquip( pAddItem->m_dwObjIndex, pProp->dwParts );
			}
		}
		pUser->GetWorld()->ADDOBJ( pMover, TRUE, pUser->GetLayer() );
	}
#endif	// __WORLDSERVER
#endif	// __PERF_0226
	return TRUE;
}

BOOL TextCmd_CreateNPC( CScanner& scanner )  
{ 
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;


	D3DXVECTOR3 vPos	= pUser->GetPos();
	CWorld* pWorld	= pUser->GetWorld();

	MoverProp* pMoverProp	= NULL;

	scanner.GetToken();
	if( scanner.tokenType == NUMBER ) 
	{
		DWORD dwID	= _ttoi( scanner.Token );
		pMoverProp = prj.GetMoverPropEx( dwID );
	}
	else
		pMoverProp	= prj.GetMoverProp( scanner.Token );

	CString strName = scanner.Token;

	if( pMoverProp && pMoverProp->dwID != 0 )
	{
		if( pMoverProp->dwAI != AII_MONSTER
			&& pMoverProp->dwAI != AII_CLOCKWORKS
			&& pMoverProp->dwAI != AII_BIGMUSCLE
			&& pMoverProp->dwAI != AII_KRRR
			&& pMoverProp->dwAI != AII_BEAR
			&& pMoverProp->dwAI != AII_METEONYKER
#ifdef __INSTANCE_AGGRO_SYSTEM
			&& pMoverProp->dwAI != AII_AGGRO_NORMAL
			&& pMoverProp->dwAI != AII_PARTY_AGGRO_LEADER
			&& pMoverProp->dwAI != AII_PARTY_AGGRO_SUB
#endif // __INSTANCE_AGGRO_SYSTEM
		)
			return TRUE;

		DWORD dwNum	= scanner.GetNumber();
		if( dwNum > 100 ) dwNum = 100;
		if( dwNum == 0 ) dwNum = 1;

		BOOL bActiveAttack = scanner.GetNumber();
		for( DWORD dw = 0; dw < dwNum; dw++ )
		{
			CObj* pObj	= CreateObj( D3DDEVICE, OT_MOVER, pMoverProp->dwID );
			if( NULL == pObj )	
				return FALSE;	
			pObj->SetPos( vPos );
			pObj->InitMotion( MTI_STAND );
			pObj->UpdateLocalMatrix();

			if( bActiveAttack )
				((CMover*)pObj)->m_bActiveAttack = bActiveAttack;
			
			((CMover*)pObj)->SetGold(((CMover*)pObj)->GetLevel()*15);  // 몬스터 생성시 기본 페냐를 설정
			pWorld->ADDOBJ( pObj, TRUE, pUser->GetLayer() );
		}
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_Invisible( CScanner& scanner )        
{ 
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	pUser->m_dwMode |= TRANSPARENT_MODE;
	g_UserMng.AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}
BOOL TextCmd_NoInvisible( CScanner& scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	pUser->m_dwMode &= (~TRANSPARENT_MODE);
	g_UserMng.AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}
BOOL TextCmd_NoUndying( CScanner& scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	pUser->m_dwMode &= (~MATCHLESS_MODE);
	pUser->m_dwMode &= (~MATCHLESS2_MODE);
	g_UserMng.AddModifyMode( pUser );
#endif
	return TRUE;
}

// exp상승 금지 명령. 토글방식으로 동작.
BOOL TextCmd_ExpUpStop( CScanner& scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	if( pUser->m_dwMode & MODE_EXPUP_STOP )
		pUser->m_dwMode &= (~MODE_EXPUP_STOP);
	else
		pUser->m_dwMode |= MODE_EXPUP_STOP;
	
	g_UserMng.AddModifyMode( pUser );
#endif // __WORLDSERVER
	
	return TRUE;
}
BOOL TextCmd_PartyInvite( CScanner& scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	MoverProp* pMoverProp = NULL;
	scanner.GetToken();
#if __VER >= 11 // __SYS_PLAYER_DATA
	u_long uidPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerId( scanner.token );
#else	// __SYS_PLAYER_DATA
	u_long uidPlayer = prj.GetPlayerID( scanner.Token );
#endif	// __SYS_PLAYER_DATA
	if( 0 < uidPlayer )
	{
		CUser* pUser2	= g_UserMng.GetUserByPlayerID( uidPlayer );	
		if( IsValidObj( pUser2 ) )
			g_DPSrvr.InviteParty( pUser->m_idPlayer, pUser2->m_idPlayer, FALSE );
		else
			pUser->AddDefinedText( TID_DIAG_0060, "\"%s\"", scanner.Token );
	}
	else
	{
		pUser->AddDefinedText( TID_DIAG_0061, "\"%s\"", scanner.Token );
	}
#endif // __WORLDSERVER

	return TRUE;
}
BOOL TextCmd_GuildInvite( CScanner& scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	MoverProp* pMoverProp = NULL;
	scanner.GetToken();
#if __VER >= 11 // __SYS_PLAYER_DATA
	u_long uidPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerId( scanner.token );
#else	// __SYS_PLAYER_DATA
	u_long uidPlayer = prj.GetPlayerID( scanner.Token );
#endif	// __SYS_PLAYER_DATA
	if( 0 < uidPlayer )
	{
		CUser* pUser2	= g_UserMng.GetUserByPlayerID( uidPlayer );	
		if( IsValidObj( pUser2 ) )
		{
			g_DPSrvr.InviteCompany( pUser, pUser2->GetId() );
		}
		else
			pUser->AddDefinedText( TID_DIAG_0060, "\"%s\"", scanner.Token );
	}
	else
	{
		pUser->AddDefinedText( TID_DIAG_0061, "\"%s\"", scanner.Token );
	}
#endif // __WORLDSERVER

	return TRUE;
}

BOOL bCTDFlag	= FALSE;

BOOL TextCmd_CTD( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)s.dwValue;
	if( g_eLocal.GetState( EVE_WORMON ) == 0 )
	{
		CGuildQuestProcessor* pProcessor	= CGuildQuestProcessor::GetInstance();
		CRect* pRect	= pProcessor->GetQuestRect( QUEST_WARMON_LV1 );
		if( pRect )
		{
			OutputDebugString( "recv /ctd" );
			REGIONELEM re;
			memset( &re, 0, sizeof(REGIONELEM) );
			re.m_uItemId	= 0xffffffff;
			re.m_uiItemCount	= 0xffffffff;
			re.m_uiMinLevel	= 0xffffffff;
			re.m_uiMaxLevel	= 0xffffffff;
			re.m_iQuest	= 0xffffffff;
			re.m_iQuestFlag	= 0xffffffff;
			re.m_iJob	= 0xffffffff;
			re.m_iGender	= 0xffffffff;
			re.m_dwAttribute	= RA_DANGER | RA_FIGHT;
			re.m_dwIdMusic	= 121;
			re.m_bDirectMusic	= TRUE;
			re.m_dwIdTeleWorld	= 0;
			re.m_rect.SetRect( pRect->TopLeft(), pRect->BottomRight() );
			lstrcpy( re.m_szTitle, "Duel Zone" );

			CWorld* pWorld	= g_WorldMng.GetWorld( WI_WORLD_MADRIGAL );
			if( pWorld )
			{
				LPREGIONELEM ptr	= pWorld->m_aRegion.GetAt( pWorld->m_aRegion.GetSize() - 1 );
				if( ptr->m_dwAttribute != ( RA_DANGER | RA_FIGHT ) )
					pWorld->m_aRegion.AddTail( &re );
				pUser->AddText( "recv /ctd" );
				g_UserMng.AddAddRegion( WI_WORLD_MADRIGAL, re );
			}
		}
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_Undying( CScanner& scanner )          
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	pUser->m_dwMode &= (~MATCHLESS2_MODE);
	pUser->m_dwMode |= MATCHLESS_MODE;
	g_UserMng.AddModifyMode( pUser );
#else // __WORLDSERVER
#ifndef __CLIENT
	CMover* pUser = (CMover*)scanner.dwValue;
	pUser->m_dwMode &= (~MATCHLESS2_MODE);
	pUser->m_dwMode |= MATCHLESS_MODE;
#endif
#endif
	return TRUE;
}
BOOL TextCmd_Undying2( CScanner& scanner )          
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	pUser->m_dwMode &= (~MATCHLESS_MODE);
	pUser->m_dwMode |= MATCHLESS2_MODE;
	g_UserMng.AddModifyMode( pUser );
#else // __WORLDSERVER
#ifndef __CLIENT
	CMover* pUser = (CMover*)scanner.dwValue;
	pUser->m_dwMode &= (~MATCHLESS_MODE);
	pUser->m_dwMode |= MATCHLESS2_MODE;
#endif
#endif
	return TRUE;
}


BOOL TextCmd_NoDisguise( CScanner& scanner )         
{ 
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	pUser->NoDisguise( NULL );
	g_UserMng.AddNoDisguise( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

#ifdef __WORLDSERVER
BOOL DoDisguise( CUser* pUser, DWORD dwIndex )
{
	pUser->Disguise( NULL, dwIndex );
	g_UserMng.AddDisguise( pUser, dwIndex );
	return TRUE;
}
#endif // __WORLDSERVER


BOOL TextCmd_Disguise( CScanner& scanner )         
{ 
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	MoverProp* pMoverProp = NULL;
	scanner.GetToken();
	if( scanner.tokenType == NUMBER ) 
	{
		DWORD dwID	= _ttoi( scanner.Token );
		pMoverProp = prj.GetMoverPropEx( dwID );
	}
	else
		pMoverProp	= prj.GetMoverProp( scanner.Token );

	if( pMoverProp )
		DoDisguise( pUser, pMoverProp->dwID );
#endif
	return TRUE;
}
BOOL TextCmd_Freeze( CScanner& scanner )           
{ 
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	
	scanner.GetToken();

	if( strcmp( pUser->GetName(), scanner.Token) )
	{
		u_long idFrom, idTo;
#if __VER >= 11 // __SYS_PLAYER_DATA
		idFrom	= CPlayerDataCenter::GetInstance()->GetPlayerId( (char*)pUser->GetName() );
		idTo	= CPlayerDataCenter::GetInstance()->GetPlayerId( scanner.token );
#else	// __SYS_PLAYER_DATA
		idFrom	= prj.GetPlayerID( pUser->GetName() );
		idTo	= prj.GetPlayerID( scanner.Token );
#endif	// __SYS_PLAYER_DATA
		if( idFrom > 0 && idTo > 0 ) 
		{
			// 1 : 추가 m_dwMode
			g_DPCoreClient.SendModifyMode( DONMOVE_MODE, (BYTE)1, idFrom, idTo );					
		}
		else 
		{
//			scanner.Token라는 이름을 가진 사용자는 이 게임에 존재하지 않는다.
			pUser->AddReturnSay( 3, scanner.Token );
		}
	}
	else
	{
		pUser->AddReturnSay( 2, " " );		// 자기 자신에게 명령했다.
	}
#endif	// __WORLDSERVER	
	return TRUE;
}

BOOL TextCmd_NoFreeze( CScanner& scanner )           
{ 
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	
	scanner.GetToken();
	if( strcmp( pUser->GetName(), scanner.Token) )
	{
		u_long idFrom, idTo;
#if __VER >= 11 // __SYS_PLAYER_DATA
		idFrom	= CPlayerDataCenter::GetInstance()->GetPlayerId( (char*)pUser->GetName() );
		idTo	= CPlayerDataCenter::GetInstance()->GetPlayerId( scanner.token );
#else	// __SYS_PLAYER_DATA
		idFrom	= prj.GetPlayerID( pUser->GetName() );
		idTo	= prj.GetPlayerID( scanner.Token );
#endif	// __SYS_PLAYER_DATA
		if( idFrom > 0 && idTo > 0 ) 
		{
			g_DPCoreClient.SendModifyMode( DONMOVE_MODE, (BYTE)0, idFrom, idTo );	// 0 : 뺌 m_dwMode
		}
		else 
		{
			//scanner.Token라는 이름을 가진 사용자는 이 게임에 존재하지 않는다.
			pUser->AddReturnSay( 3, scanner.Token );
		}
	}
	else
	{
		pUser->AddReturnSay( 2, " " );		// 자기 자신에게 명령했다.
	}
#endif	// __WORLDSERVER	
	return TRUE;
}

BOOL TextCmd_Talk( CScanner& scanner )           
{ 
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	
	scanner.GetToken();

	u_long idFrom, idTo;
#if __VER >= 11 // __SYS_PLAYER_DATA
	idFrom	= CPlayerDataCenter::GetInstance()->GetPlayerId( (char*)pUser->GetName() );
	idTo	= CPlayerDataCenter::GetInstance()->GetPlayerId( scanner.token );
#else	// __SYS_PLAYER_DATA
	idFrom	= prj.GetPlayerID( pUser->GetName() );
	idTo	= prj.GetPlayerID( scanner.Token );
#endif	// __SYS_PLAYER_DATA
	if( idFrom > 0 && idTo > 0 ) 
	{
		g_DPCoreClient.SendModifyMode( DONTALK_MODE, (BYTE)0, idFrom, idTo );	// 0 : 뺌 m_dwMode
	}
	else 
	{
		//scanner.Token라는 이름을 가진 사용자는 이 게임에 존재하지 않는다.
		pUser->AddReturnSay( 3, scanner.Token );
	}
#endif	// __WORLDSERVER	
	return TRUE;
}

BOOL TextCmd_NoTalk( CScanner& scanner )           
{ 
#ifdef __WORLDSERVER

	CUser* pUser	= (CUser*)scanner.dwValue;
	
	scanner.GetToken();

	{
		u_long idFrom, idTo;
#if __VER >= 11 // __SYS_PLAYER_DATA
		idFrom	= CPlayerDataCenter::GetInstance()->GetPlayerId( (char*)pUser->GetName() );
		idTo	= CPlayerDataCenter::GetInstance()->GetPlayerId( scanner.token );
#else	// __SYS_PLAYER_DATA
		idFrom	= prj.GetPlayerID( pUser->GetName() );
		idTo	= prj.GetPlayerID( scanner.Token );
#endif	// __SYS_PLAYER_DATA
		if( idFrom > 0 && idTo > 0 ) 
		{
			g_DPCoreClient.SendModifyMode( DONTALK_MODE, (BYTE)1, idFrom, idTo );	// 1 : 추가
		}
		else 
		{
			//scanner.Token라는 이름을 가진 사용자는 이 게임에 존재하지 않는다.
			pUser->AddReturnSay( 3, scanner.Token );
		}
	}
#endif	// __WORLDSERVER	
	return TRUE;
}

BOOL TextCmd_GetGold( CScanner& scanner )           
{ 
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	int nGold = scanner.GetNumber();	
	pUser->AddGold( nGold );
#endif	// __WORLDSERVER	
	return TRUE;
}

// /간접 npcId "대사"
BOOL TextCmd_indirect( CScanner& scanner )         
{ 
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	DWORD dwIdNPC = scanner.GetNumber();
	TCHAR szString[ 1024 ];

	scanner.GetLastFull();
	if( strlen( scanner.token ) > 260 )
		return TRUE;
	strcpy( szString, scanner.token );
	StringTrimRight( szString );

	if( szString[ 0 ] )
	{
		CMover* pMover = prj.GetMover( dwIdNPC );
		if( pMover )
			g_UserMng.AddChat( (CCtrl*)pMover, (LPCSTR)szString );
	}
#else // __WORLDSERVER
	scanner.GetToken();
	if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
	{
		if( scanner.tok == FINISHED )
		{
			if( g_WndMng.GetWndBase( APP_ADMIN_INDIRECT_TALK ) == NULL )
			{
				CWndIndirectTalk* pWndIndirectTalk = new CWndIndirectTalk;
				pWndIndirectTalk->Initialize();
			}
			return FALSE;
		}
	}
#endif // __WORLDSERVER
	return TRUE;
}
/*
BOOL TextCmd_sbreport( CScanner & scanner )
{
#ifdef __WORLDSERVER
	if( !g_eLocal.GetState( EVE_SCHOOL ) )
		return FALSE;
	return CEveSchool::GetInstance()->Report();
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_sbready( CScanner & scanner )
{
#ifdef __WORLDSERVER
	if( !g_eLocal.GetState( EVE_SCHOOL ) )
		return FALSE;
	return CEveSchool::GetInstance()->Ready();
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_sbstart( CScanner & scanner )
{
#ifdef __WORLDSERVER
	if( !g_eLocal.GetState( EVE_SCHOOL ) )
		return FALSE;
	return CEveSchool::GetInstance()->Start();
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_sbstart2( CScanner & scanner )
{
#ifdef __WORLDSERVER
	if( !g_eLocal.GetState( EVE_SCHOOL ) )
		return FALSE;
	return CEveSchool::GetInstance()->Start2();
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_sbend( CScanner & scanner )
{
#ifdef __WORLDSERVER
	if( !g_eLocal.GetState( EVE_SCHOOL ) )
		return FALSE;
	return CEveSchool::GetInstance()->End();
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_OpenBattleServer( CScanner & s )
{
#ifdef __WORLDSERVER
	BEFORESENDDUAL( ar, PACKETTYPE_OPEN_BATTLESERVER, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_CloseBattleServer( CScanner & s )
{
#ifdef __WORLDSERVER
	BEFORESENDDUAL( ar, PACKETTYPE_CLOSE_BATTLESERVER, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );
#endif	// __WORLDSERVER
	return TRUE;
}
*/

BOOL TextCmd_ItemMode( CScanner& scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	pUser->SetMode( ITEM_MODE );
	g_UserMng.AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_ItemNotMode( CScanner& scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	pUser->SetNotMode( ITEM_MODE );
	g_UserMng.AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_AttackMode( CScanner& scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	pUser->SetMode( NO_ATTACK_MODE );
	g_UserMng.AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_EscapeReset( CScanner& scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	CMover* pMover = prj.GetUserByID( scanner.GetNumber() );
	if( IsValidObj( pMover ) ) {
		pMover->SetSMMode( SM_ESCAPE, 0 );
	}
	else {
		pUser->SetSMMode( SM_ESCAPE, 0 );
	}
#else	// __WORLDSERVER
#ifdef __CLIENT
	CWorld* pWorld	= g_WorldMng.Get();
	CObj*	pObj;
	
	pObj = pWorld->GetObjFocus();
	if( pObj && pObj->GetType() == OT_MOVER && ((CMover*)pObj)->IsPlayer() )
	{
		CMover* pMover = (CMover*)pObj;
		CString strSend;
		if( pMover->IsPlayer() )
		{
			strSend.Format( "/EscapeReset %d", pMover->m_idPlayer  );
		}

		g_DPlay.SendChat( (LPCSTR)strSend );

		return FALSE;
	}
#endif //__CLIENT
#endif	// __WORLDSERVER
	return TRUE;
}



BOOL TextCmd_AttackNotMode( CScanner& scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	pUser->SetNotMode( NO_ATTACK_MODE );
	g_UserMng.AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_CommunityMode( CScanner& scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	pUser->SetMode( COMMUNITY_MODE );
	g_UserMng.AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_CommunityNotMode( CScanner& scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	pUser->SetNotMode( COMMUNITY_MODE );
	g_UserMng.AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_ObserveMode( CScanner& scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	pUser->SetMode( OBSERVE_MODE );
	g_UserMng.AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_ObserveNotMode( CScanner& scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	pUser->SetNotMode( OBSERVE_MODE );
	g_UserMng.AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_Onekill( CScanner& scanner )          
{ 
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	pUser->m_dwMode |= ONEKILL_MODE;
	g_UserMng.AddModifyMode( pUser );
#else // __WORLDSERVER
#ifndef __CLIENT
	CMover* pUser = (CMover*)scanner.dwValue;
	pUser->m_dwMode |= ONEKILL_MODE;
#endif
#endif
	return TRUE;
}
BOOL TextCmd_Position( CScanner& scanner )          
{ 
#ifdef __CLIENT
	CString string;
	D3DXVECTOR3 vPos = g_pPlayer->GetPos();
	//string.Format( "현재좌표 : x = %f, y = %f, z = %f", vPos.x, vPos.y, vPos.z );
	string.Format( prj.GetText(TID_GAME_NOWPOSITION), vPos.x, vPos.y, vPos.z );
	g_WndMng.PutString( string, NULL, prj.GetTextColor( TID_GAME_NOWPOSITION ) );
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_NoOnekill( CScanner& scanner )          
{ 
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	pUser->m_dwMode &= (~ONEKILL_MODE);
	g_UserMng.AddModifyMode( pUser );
#else // __WORLDSERVER
#ifndef __CLIENT
	CMover* pUser = (CMover*)scanner.dwValue;
	pUser->m_dwMode &= (~ONEKILL_MODE);
#endif
#endif
	return TRUE;
}
BOOL TextCmd_ip( CScanner& scanner )               
{ 
#ifdef __WORLDSERVER
	scanner.GetToken();

	CUser* pUser = (CUser*)scanner.dwValue;
#if __VER >= 11 // __SYS_PLAYER_DATA
	u_long idPlayer		= CPlayerDataCenter::GetInstance()->GetPlayerId( scanner.token );
#else	// __SYS_PLAYER_DATA
	u_long idPlayer		= prj.GetPlayerID( scanner.Token );
#endif	// __SYS_PLAYER_DATA
	if( idPlayer > 0 )
		g_DPCoreClient.SendGetPlayerAddr( pUser->m_idPlayer, idPlayer );
	else 
		pUser->AddReturnSay( 3, scanner.Token );
#else	// __WORLDSERVER
	#ifdef __CLIENT
	CWorld* pWorld	= g_WorldMng.Get();
	CObj*	pObj;
	
	pObj = pWorld->GetObjFocus();
	if( pObj && pObj->GetType() == OT_MOVER )
	{
		CMover* pMover = (CMover*)pObj;
		CString strSend;
		if( pMover->IsPlayer() )
		{
			strSend.Format( "/ip %s", pMover->GetName() );
		}
		else
		{
			strSend.Format( "%s", scanner.m_pBuf );
		}
		g_DPlay.SendChat( (LPCSTR)strSend );
		return FALSE;
	}
	#endif //__CLIENT
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_userlist( CScanner& scanner )         
{ 
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	g_DPCoreClient.SendGetCorePlayer( pUser->m_idPlayer );
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_count( CScanner& scanner )            
{ 
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	g_DPCoreClient.SendGetPlayerCount( pUser->m_idPlayer );

	char szCount[128]	= { 0, };
	sprintf( szCount, "Players online: %d", g_UserMng.GetCount() );
	pUser->AddText( szCount );
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_System( CScanner & scanner )
{
#ifdef __WORLDSERVER
    CHAR szString[512] = "";
    CUser* pUser    = (CUser*)scanner.dwValue;
CString strName;
    strName = pUser->GetName();

    scanner.GetLastFull();
    if( strlen( scanner.token ) >= 512 )
        return TRUE;
    strcpy( szString, scanner.token );
    StringTrimRight( szString );

    g_DPCoreClient.SendSystem( "[" + strName + "]" +   szString );

#endif    // __WORLDSERVER
    return TRUE;
}

BOOL TextCmd_SystemName( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CHAR szString[512] = "";

	CUser* pUser	= (CUser*)scanner.dwValue;

	scanner.GetLastFull();
	if( strlen( scanner.token ) >= 512 )
		return TRUE;
	strcpy( szString, scanner.token );
	StringTrimRight( szString );
	
	CHAR szSend[512] = "";
	sprintf( szSend, "[%s]: %s", ((CMover *)pUser)->GetName(), szString );
	g_DPCoreClient.SendSystem( szSend );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_LoadScript( CScanner & scanner )
{
#ifdef __WORLDSERVER
	#if defined(__REMOVE_SCIRPT_060712)
		if( CWorldDialog::GetInstance().Reload() == FALSE )
			Error("WorldScript.dll reload error");
	#else
		CUser* pUser	 = (CUser*)scanner.dwValue;
		pUser->GetWorld()->LoadAllMoverDialog();
	#endif
#endif
	return TRUE;
}

BOOL TextCmd_FallSnow( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	 = (CUser*)scanner.dwValue;
	g_DPCoreClient.SendFallSnow();
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_FallRain( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	 = (CUser*)scanner.dwValue;
	g_DPCoreClient.SendFallRain();
#endif // __WORLDSERVER
	return TRUE;
}
BOOL TextCmd_StopSnow( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	 = (CUser*)scanner.dwValue;
	g_DPCoreClient.SendStopSnow();
#endif // __WORLDSERVER
	return TRUE;
}
BOOL TextCmd_StopRain( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	 = (CUser*)scanner.dwValue;
	g_DPCoreClient.SendStopRain();
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_JobName( CScanner & scanner )
{
#ifdef __CLIENT
	char chMessage[MAX_PATH] = {0,};
	sprintf( chMessage, "Expert Job : " );
	for( int i = MAX_JOBBASE ; i < MAX_EXPERT ; ++i )
	{
		if( strlen( prj.m_aJob[i].szName ) < 15 )
		{
			strcat( chMessage, prj.m_aJob[i].szName );
			if( i + 1 != MAX_EXPERT )
			{
				strcat( chMessage, ", ");
			}
		}
	}
	g_WndMng.PutString( chMessage, NULL, 0xffff0000 );
	sprintf( chMessage, "Expert Level : %d ~ %d", MAX_JOB_LEVEL + 1, MAX_JOB_LEVEL + MAX_EXP_LEVEL );
	g_WndMng.PutString( chMessage, NULL, 0xffff0000 );

	sprintf( chMessage, "Professional Job : " );
	for( i = MAX_EXPERT ; i < MAX_PROFESSIONAL ; ++i )
	{
		if( strlen( prj.m_aJob[i].szName ) < 15 )
		{
			strcat( chMessage, prj.m_aJob[i].szName );
			if( i + 1 != MAX_PROFESSIONAL )
			{
				strcat( chMessage, ", ");
			}
		}
	}
	g_WndMng.PutString( chMessage, NULL, 0xffff0000 );
	sprintf( chMessage, "Professional Level : %d ~~~ ", MAX_JOB_LEVEL + MAX_EXP_LEVEL );
	g_WndMng.PutString( chMessage, NULL, 0xffff0000 );
#endif // __CLIENT
	return TRUE;
}

#ifdef __CLIENT

BOOL TextCmd_tradeagree( CScanner & scanner )
{
	g_Option.m_bTrade = 1;
	g_WndMng.PutString( prj.GetText( TID_GAME_TRADEAGREE ), NULL, prj.GetTextColor( TID_GAME_TRADEAGREE ) );
	return TRUE;
}
BOOL TextCmd_traderefuse( CScanner & scanner )
{
	g_Option.m_bTrade = 0;
	g_WndMng.PutString( prj.GetText( TID_GAME_TRADEREFUSE ), NULL, prj.GetTextColor( TID_GAME_TRADEREFUSE ) );
	return TRUE;
}
BOOL TextCmd_whisperagree( CScanner & scanner )
{
	g_Option.m_bSay = 1;
	g_WndMng.PutString( prj.GetText( TID_GAME_WHISPERAGREE ), NULL, prj.GetTextColor( TID_GAME_WHISPERAGREE ) );
	return TRUE;
}
BOOL TextCmd_whisperrefuse( CScanner & scanner )
{
	g_Option.m_bSay = 0;
	g_WndMng.PutString( prj.GetText( TID_GAME_WHISPERREFUSE ), NULL, prj.GetTextColor( TID_GAME_WHISPERREFUSE ) );
	return TRUE;
}
BOOL TextCmd_messengeragree( CScanner & scanner )
{
	g_Option.m_bMessenger = 1;
	g_WndMng.PutString( prj.GetText( TID_GAME_MSGERAGREE ), NULL, prj.GetTextColor( TID_GAME_MSGERAGREE ) );
	return TRUE;
}
BOOL TextCmd_messengerrefuse( CScanner & scanner )
{
	g_Option.m_bMessenger = 0;
	g_WndMng.PutString( prj.GetText( TID_GAME_MSGERREFUSE ), NULL, prj.GetTextColor( TID_GAME_MSGERREFUSE ) );
	return TRUE;
}
BOOL TextCmd_stageagree( CScanner & scanner )
{
	g_Option.m_bParty = 1;
	g_WndMng.PutString( prj.GetText( TID_GAME_STAGEAGREE ), NULL, prj.GetTextColor( TID_GAME_STAGEAGREE ) );
	return TRUE;
}
BOOL TextCmd_stagerefuse( CScanner & scanner )
{
	g_Option.m_bParty = 0;
	g_WndMng.PutString( prj.GetText( TID_GAME_STAGEREFUSE ), NULL, prj.GetTextColor( TID_GAME_STAGEREFUSE ) );
	return TRUE;
}
BOOL TextCmd_connectagree( CScanner & scanner )
{
	g_Option.m_bMessengerJoin = 1;
	g_WndMng.PutString( prj.GetText( TID_GAME_CONNAGREE ), NULL, prj.GetTextColor( TID_GAME_CONNAGREE ) );
	return TRUE;
}
BOOL TextCmd_connectrefuse( CScanner & scanner )
{
	g_Option.m_bMessengerJoin = 0;
	g_WndMng.PutString( prj.GetText( TID_GAME_CONNREFUSE ), NULL, prj.GetTextColor( TID_GAME_CONNREFUSE ) );
	return TRUE;
}
BOOL TextCmd_shoutagree( CScanner & scanner )
{
	g_Option.m_bShout = 1;
	g_WndMng.PutString( prj.GetText( TID_GAME_SHOUTAGREE ), NULL, prj.GetTextColor( TID_GAME_SHOUTAGREE ) );
	return TRUE;
}
BOOL TextCmd_shoutrefuse( CScanner & scanner )
{
	g_Option.m_bShout = 0;
	g_WndMng.PutString( prj.GetText( TID_GAME_SHOUTREFUSE ), NULL, prj.GetTextColor( TID_GAME_SHOUTREFUSE ) );
	return TRUE;
}

#ifdef __YS_CHATTING_BLOCKING_SYSTEM
BOOL TextCmd_BlockUser( CScanner & scanner )
{
	if( prj.m_setBlockedUserID.size() >= CProject::BLOCKING_NUMBER_MAX )
	{
		// 차단 가능한 유저 수를 초과하였습니다. 차단 목록을 정리한 후에 다시 사용해 주십시오.
		g_WndMng.PutString( prj.GetText( TID_GAME_ERROR_FULL_BLOCKED_USER_LIST ), NULL, prj.GetTextColor( TID_GAME_ERROR_FULL_BLOCKED_USER_LIST ) );
		return FALSE;
	}
	scanner.GetToken();
	CString strUserName = scanner.token;
	if( strUserName == _T( "" ) )
	{
		// 존재하지 않은 아이디입니다. 아이디를 확인해 주십시오.
		g_WndMng.PutString( prj.GetText( TID_GAME_ERROR_INVALID_USER_ID ), NULL, prj.GetTextColor( TID_GAME_ERROR_INVALID_USER_ID ) );
		return FALSE;
	}
	if( g_pPlayer && g_pPlayer->GetName( TRUE ) == strUserName )
	{
		// 자기 캐릭터는 차단할 수 없습니다. 아이디를 확인해 주십시오.
		g_WndMng.PutString( prj.GetText( TID_GAME_ERROR_MY_CHARACTER_CANT_BLOCKING ), NULL, prj.GetTextColor( TID_GAME_ERROR_MY_CHARACTER_CANT_BLOCKING ) );
		return FALSE;
	}
	set< CString >::iterator BlockedUserIterator = prj.m_setBlockedUserID.find( strUserName );
	if( BlockedUserIterator != prj.m_setBlockedUserID.end() )
	{
		// 이미 채팅 차단되어 있는 대상입니다.
		g_WndMng.PutString( prj.GetText( TID_GAME_ERROR_ALREADY_BLOCKED ), NULL, prj.GetTextColor( TID_GAME_ERROR_ALREADY_BLOCKED ) );
	}
	else
	{
		prj.m_setBlockedUserID.insert( strUserName );

		if( g_WndMng.m_pWndChattingBlockingList )
		{
			g_WndMng.m_pWndChattingBlockingList->UpdateInformation();
		}

		CString strMessage = _T( "" );
		// %s 님의 채팅을 차단하였습니다.
		strMessage.Format( prj.GetText( TID_GAME_USER_CHATTING_BLOCKING ), strUserName );
		g_WndMng.PutString( strMessage, NULL, prj.GetTextColor( TID_GAME_USER_CHATTING_BLOCKING ) );
	}
	return TRUE;
}

BOOL TextCmd_CancelBlockedUser( CScanner & scanner )
{
	scanner.GetToken();
	CString strUserName = scanner.token;
	set< CString >::iterator BlockedUserIterator = prj.m_setBlockedUserID.find( strUserName );
	if( BlockedUserIterator != prj.m_setBlockedUserID.end() )
	{
		prj.m_setBlockedUserID.erase( strUserName );

		if( g_WndMng.m_pWndChattingBlockingList )
		{
			g_WndMng.m_pWndChattingBlockingList->UpdateInformation();
		}

		CString strMessage = _T( "" );
		// %s 님의 채팅 차단을 해제하였습니다.
		strMessage.Format( prj.GetText( TID_GAME_USER_CHATTING_UNBLOCKING ), strUserName );
		g_WndMng.PutString( strMessage, NULL, prj.GetTextColor( TID_GAME_USER_CHATTING_UNBLOCKING ) );
	}
	else
	{
		// 채팅 차단 목록에 없는 대상입니다.
		g_WndMng.PutString( prj.GetText( TID_GAME_ERROR_THERE_IS_NO_BLOCKED_TARGET ), NULL, prj.GetTextColor( TID_GAME_ERROR_THERE_IS_NO_BLOCKED_TARGET ) );
	}
	return TRUE;
}

BOOL TextCmd_IgnoreList( CScanner & scanner )
{
	if( g_WndMng.m_pWndChattingBlockingList )
	{
		SAFE_DELETE( g_WndMng.m_pWndChattingBlockingList );
	}

	g_WndMng.m_pWndChattingBlockingList = new CWndChattingBlockingList;
	if( g_WndMng.m_pWndChattingBlockingList )
	{
		g_WndMng.m_pWndChattingBlockingList->Initialize();
	}

	return TRUE;
}
#endif // __YS_CHATTING_BLOCKING_SYSTEM

#endif // __CLIENT

BOOL TextCmd_QuestState( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pAdmin	= (CUser*)s.dwValue;
	CUser* pUser	= NULL;
	int nQuest	= s.GetNumber();
	int nState = s.GetNumber();
	s.GetToken();
	if( s.tok != FINISHED )
	{
#if __VER >= 11 // __SYS_PLAYER_DATA
		u_long idPlayer		= CPlayerDataCenter::GetInstance()->GetPlayerId( s.token );
#else	// __SYS_PLAYER_DATA
		u_long idPlayer		= prj.GetPlayerID( s.Token );
#endif	// __SYS_PLAYER_DATA
		if( idPlayer )
			pUser	= g_UserMng.GetUserByPlayerID( idPlayer );
		if( pUser == NULL )
		{
			pAdmin->AddDefinedText( TID_DIAG_0061, "%s", s.Token );
			return TRUE;
		}
	}
	else
	{
		pUser	= pAdmin;
	}
	if( nState >= QS_BEGIN && nState < QS_END )
	{
		QUEST quest;
		if( pUser->SetQuest( nQuest, nState, &quest ) )
		{
			pUser->AddSetQuest( &quest );

			char pszComment[100]	= { 0, };
			sprintf( pszComment, "%s %d", pAdmin->GetName(), nState );
			g_dpDBClient.CalluspLoggingQuest(  pUser->m_idPlayer, nQuest, 11, pszComment );
		}
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_BeginQuest( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)s.dwValue;
	int nQuest = s.GetNumber();
	QUEST quest;
	if( pUser->SetQuest( nQuest, 0, &quest ) )
		pUser->AddSetQuest( &quest );
#endif
	return TRUE;
}
BOOL TextCmd_EndQuest( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)s.dwValue;
	int nQuest = s.GetNumber();
	QUEST quest;
	if( pUser->SetQuest( nQuest, QS_END, &quest ) )
		pUser->AddSetQuest( &quest );
#endif
	return TRUE;
}
// 지정한 것, 현재와 완료 다 뒤져서 삭제 
BOOL TextCmd_RemoveQuest( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pAdmin	= (CUser*)s.dwValue;
	CUser* pUser	= NULL;
	int nQuest	= s.GetNumber();
	s.GetToken();
	if( s.tok != FINISHED )
	{
#if __VER >= 11 // __SYS_PLAYER_DATA
		u_long idPlayer		= CPlayerDataCenter::GetInstance()->GetPlayerId( s.token );
#else	// __SYS_PLAYER_DATA
		u_long idPlayer		= prj.GetPlayerID( s.Token );
#endif	// __SYS_PLAYER_DATA
		if( idPlayer )
			pUser	= g_UserMng.GetUserByPlayerID( idPlayer );
		if( pUser == NULL )
		{
			pAdmin->AddDefinedText( TID_DIAG_0061, "%s", s.Token );
			return TRUE;
		}
	}
	else
	{
		pUser	= pAdmin;
	}

	LPQUEST pQuest	= pUser->GetQuest(nQuest );
	char pszComment[100]	= { 0, };
	sprintf( pszComment, "%s %d", pAdmin->GetName(), ( pQuest? pQuest->m_nState: -1 ) );
	g_dpDBClient.CalluspLoggingQuest(  pUser->m_idPlayer, nQuest, 40, pszComment );

	pUser->RemoveQuest( nQuest );
	pUser->AddRemoveQuest( nQuest );
#endif	// __WORLDSERVER
	return TRUE;
}
// 현재, 완료 모두 삭제 
BOOL TextCmd_RemoveAllQuest( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)s.dwValue;
	int nQuest = s.GetNumber();
	pUser->RemoveAllQuest();
	pUser->AddRemoveAllQuest();
#endif
	return TRUE;
}
// 완료만 삭제 
BOOL TextCmd_RemoveCompleteQuest( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)s.dwValue;
	int nQuest = s.GetNumber();
	pUser->RemoveCompleteQuest();
	pUser->AddRemoveCompleteQuest();
#endif
	return TRUE;
}

BOOL TextCmd_PvpParam( CScanner& scanner )
{
#if defined(__WORLDSERVER)
	CUser* pUser	= (CUser*)scanner.dwValue;
	int	nFame       = scanner.GetNumber();
	int nSlaughter  = scanner.GetNumber();

	pUser->m_nFame  = nFame;
	g_UserMng.AddSetFame( pUser, nFame );

#if __VER < 8 // __S8_PK
	pUser->ChangeSlaughter( CHANGE_SLAUGHTER_SET, NULL, nSlaughter );
#endif // __VER < 8 // __S8_PK
#endif
	return TRUE;
}
#if __VER >= 8 // __S8_PK
BOOL TextCmd_PKParam( CScanner& scanner )
{
#ifdef __WORLDSERVER
	CUser*	pUser			= (CUser*)scanner.dwValue;
	int		nPKValue		= scanner.GetNumber();
	int		nPKPropensity	= scanner.GetNumber();

	if( nPKValue >= 0 )
	{
		pUser->SetPKValue( nPKValue );
		pUser->AddPKValue();
#if __VER >= 13 // __HONORABLE_TITLE			// 달인
		pUser->CheckHonorStat();
		pUser->AddHonorListAck();
		g_UserMng.AddHonorTitleChange( pUser, pUser->m_nHonor);
#endif	// __HONORABLE_TITLE			// 달인
	}

	if( nPKPropensity >= 0 )
	{
		pUser->SetPKPropensity( nPKPropensity );
		g_UserMng.AddPKPropensity( pUser );
	}
#endif // __WORLDSERVER
	return TRUE;
}
#endif // __VER >= 8 // __S8_PK

#ifdef _DEBUG
BOOL TextCmd_TransyItemList( CScanner& scanner )
{
#ifdef __CLIENT
	CString szMsg;
	scanner.GetToken();
	if( scanner.tokenType == STRING )
	{
		szMsg.Format( "Wait : Write %s", scanner.Token );
		g_WndMng.PutString( szMsg );

		for( int i = 0; i < prj.m_aPropItem.GetSize(); i++ )
		{
			ItemProp* pItemProp =  prj.GetItemProp( i );
			g_pPlayer->GetTransyItem( pItemProp, TRUE, scanner.Token );
		}
		szMsg.Format( "Finish : Finish %s", scanner.Token );
		g_WndMng.PutString( szMsg );
	}
	else
	{
		g_WndMng.PutString( "Error : Ex) /TransyItemList Transy.txt&" );
	}
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_LoadToolTipColor( CScanner& scanner )
{
#ifdef __CLIENT
	return g_Option.LoadToolTip( "ToolTip.ini" );
#else
	return TRUE;
#endif // __CLIENT
}

#endif //_DEBUG


BOOL TextCmd_ReloadConstant( CScanner& scanner )
{
#ifdef __THA_0808
	if( ::GetLanguage() == LANG_THA )
		return TRUE;
#endif	// __THA_0808
#ifdef __WORLDSERVER
	g_DPCoreClient.SendLoadConstant();
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_GuildCombatRequest( CScanner& scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	DWORD dwPenya = scanner.GetNumber();	
	g_GuildCombatMng.GuildCombatRequest( pUser, dwPenya );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_GuildCombatCancel( CScanner& scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	g_GuildCombatMng.GuildCombatCancel( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_GuildCombatOpen( CScanner& scanner )
{
#ifdef __CLIENT
#endif // __CLINET
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	if( g_GuildCombatMng.m_nState != CGuildCombat::CLOSE_STATE )
	{
		char chMessage[128] = {0,};
		sprintf( chMessage, "Not GuildCombat Open :: Not CLOSE_STATE" );
		pUser->AddText( chMessage );
		return TRUE;
	}

	g_GuildCombatMng.GuildCombatOpen();
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_GuildCombatIn( CScanner& scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	g_GuildCombatMng.GuildCombatEnter( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_GuildCombatClose( CScanner& scanner )
{
#ifdef __WORLDSERVER
	int	nClose       = scanner.GetNumber();

	CUser* pUser	= (CUser*)scanner.dwValue;
	if( g_GuildCombatMng.m_nState == CGuildCombat::CLOSE_STATE )
	{
		char chMessage[128] = {0,};
		sprintf( chMessage, "Not GuildCombat Close :: Is CLOSE_STATE" );
		pUser->AddText( chMessage );
		return TRUE;
	}

	if( nClose == 0 )
		g_GuildCombatMng.SetGuildCombatClose( TRUE );
	else
		g_GuildCombatMng.SetGuildCombatCloseWait( TRUE );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_GuildCombatNext( CScanner& scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	if( g_GuildCombatMng.m_nState == CGuildCombat::CLOSE_STATE )
	{
		char chMessage[128] = {0,};
		sprintf( chMessage, "Not GuildCombat Close :: Is CLOSE_STATE" );
		pUser->AddText( chMessage );
		return TRUE;
	}
	
	g_GuildCombatMng.m_dwTime = GetTickCount();
#endif // __WORLDSERVER
	return TRUE;
}

#if __VER >= 10 // __REMOVE_ATTRIBUTE
BOOL TextCmd_RemoveAttribute( CScanner& scanner )
{
#ifdef __CLIENT
	if( g_WndMng.m_pWndUpgradeBase == NULL )
	{
		SAFE_DELETE( g_WndMng.m_pWndUpgradeBase );
		g_WndMng.m_pWndUpgradeBase = new CWndUpgradeBase;
		g_WndMng.m_pWndUpgradeBase->Initialize( &g_WndMng, APP_TEST );
		return FALSE;
	}
	
	if( g_WndMng.m_pWndUpgradeBase )
	{
		if( g_WndMng.m_pWndUpgradeBase->m_pItemElem[0] )
		{
			DWORD dwObjId = g_WndMng.m_pWndUpgradeBase->m_pItemElem[0]->m_dwObjId;
			g_DPlay.SendRemoveAttribute( dwObjId );
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
#endif // __CLIENT
	return TRUE;	
}
#endif // __REMOVE_ATTRIBUTE

#if __VER >= 11 // __GUILD_COMBAT_1TO1
BOOL	TextCmd_GC1to1Open( CScanner& scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	if( g_GuildCombat1to1Mng.m_nState != g_GuildCombat1to1Mng.GC1TO1_OPEN )
	{
		BOOL bSrvrDown = FALSE;
		if( scanner.GetNumber() == 1 )
			bSrvrDown = TRUE;
		g_GuildCombat1to1Mng.GuildCombat1to1Open( bSrvrDown );
		return TRUE;
	}

	pUser->AddText( "already OPEN State!!!" );
#endif //__WORLDSERVER
	return TRUE;
}

BOOL	TextCmd_GC1to1Close( CScanner& scanner )
{
#ifdef __WORLDSERVER
	g_GuildCombat1to1Mng.m_nState = g_GuildCombat1to1Mng.GC1TO1_WAR;
	for( DWORD i=0; i<g_GuildCombat1to1Mng.m_vecGuilCombat1to1.size(); i++ )
	{
		if( g_GuildCombat1to1Mng.m_vecGuilCombat1to1[i].m_nState != CGuildCombat1to1::GC1TO1WAR_CLOSEWAIT )
			g_GuildCombat1to1Mng.m_vecGuilCombat1to1[i].GuildCombat1to1CloseWait();
		g_GuildCombat1to1Mng.m_vecGuilCombat1to1[i].m_nWaitTime = -1;
	}
#endif // __WORLDSERVER
	return TRUE;
}

BOOL	TextCmd_GC1to1Next( CScanner& scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	if( g_GuildCombat1to1Mng.m_nState == g_GuildCombat1to1Mng.GC1TO1_CLOSE ) // GC1TO1_CLOSE
	{
		pUser->AddText( "Is CLOSE State!!!" );
		return TRUE;
	}
	if( g_GuildCombat1to1Mng.m_nState == g_GuildCombat1to1Mng.GC1TO1_WAR )
	{
		for( DWORD i=0; i<g_GuildCombat1to1Mng.m_vecGuilCombat1to1.size(); i++ )
			g_GuildCombat1to1Mng.m_vecGuilCombat1to1[i].m_nWaitTime = -1;
		return TRUE;
	}

	g_GuildCombat1to1Mng.m_nWaitTime = -1;
#endif // __WORLDERVER
	return TRUE;
}
#endif // __GUILD_COMBAT_1TO1

#ifdef __EVENTLUA_COUPON
BOOL TextCmd_Coupon( CScanner& s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	pUser->m_nCoupon = s.GetNumber();
#endif // __WORLDSERVER
	return TRUE;
}
#endif // __EVENTLUA_COUPON

#ifdef __NPC_BUFF
BOOL TextCmd_RemoveAllBuff( CScanner& s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	pUser->RemoveAllBuff();
	pUser->ClearAllSMMode();
#endif // __WORLDSERVER
	return TRUE;
}
#endif // __NPC_BUFF

#if __VER >= 12 // __HEAVEN_TOWER
BOOL TextCmd_HeavenTower( CScanner& s )
{
#ifdef __CLIENT
	if(g_WndMng.m_pWndHeavenTower)
		SAFE_DELETE(g_WndMng.m_pWndHeavenTower);

	g_WndMng.m_pWndHeavenTower = new CWndHeavenTower;

	if(g_WndMng.m_pWndHeavenTower)
		g_WndMng.m_pWndHeavenTower->Initialize(NULL);
#endif // __CLIENT
	return TRUE;
}
#endif //__HEAVEN_TOWER

#if __VER >= 12 // __CSC_VER12_4
BOOL TextCmd_RemoveJewel( CScanner& s )
{
#ifdef __CLIENT
	if(g_WndMng.m_pWndRemoveJewel)
		SAFE_DELETE(g_WndMng.m_pWndRemoveJewel);

	g_WndMng.m_pWndRemoveJewel = new CWndRemoveJewel;

	if(g_WndMng.m_pWndRemoveJewel)
		g_WndMng.m_pWndRemoveJewel->Initialize(NULL);
#endif // __CLIENT
	return TRUE;
}
#endif //__CSC_VER12_4

#if __VER >= 12 // __CSC_VER12_5
BOOL TextCmd_TransEggs( CScanner& s )
{
#ifdef __CLIENT
	g_WndMng.CreateApplet( APP_INVENTORY );

	if(g_WndMng.m_pWndPetTransEggs)
		SAFE_DELETE(g_WndMng.m_pWndPetTransEggs);
	
	g_WndMng.m_pWndPetTransEggs = new CWndPetTransEggs;

	if(g_WndMng.m_pWndPetTransEggs)
		g_WndMng.m_pWndPetTransEggs->Initialize(NULL);
#endif // __CLIENT
	return TRUE;
}
#endif //__CSC_VER12_5

#if __VER >= 12 // __SECRET_ROOM
BOOL TextCmd_SecretRoomOpen( CScanner& s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	if( CSecretRoomMng::GetInstance()->m_nState == SRMNG_CLOSE )
		CSecretRoomMng::GetInstance()->SecretRoomOpen();
	else
		pUser->AddText( "Is Not Close State!!!" );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_SecretRoomEntrance( CScanner& s )
{
#ifdef __CLIENT
	if( s.GetNumber() == 1 )
	{
		g_DPlay.SendTeleportToSecretRoomDungeon();
		return TRUE;
	}

	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
	if(pWndWorld)
	{
		for(int i=0; i<MAX_KILLCOUNT_CIPHERS; i++)
		{
			pWndWorld->m_stKillCountCiphers[i].bDrawMyGuildKillCount = TRUE;
			pWndWorld->m_stKillCountCiphers[i].szMyGuildKillCount = '0';
			pWndWorld->m_stKillCountCiphers[i].ptPos = CPoint(0,0);
			pWndWorld->m_stKillCountCiphers[i].fScaleX = 1.0f;
			pWndWorld->m_stKillCountCiphers[i].fScaleY = 1.0f;
			pWndWorld->m_stKillCountCiphers[i].nAlpha = 255;

//			pWndWorld->m_bDrawMyGuildKillCount[i] = TRUE;
//			pWndWorld->m_szMyGuildKillCount[i] = '0';
		}
	}
	
	if(g_WndMng.m_pWndSecretRoomMsg)
		SAFE_DELETE( g_WndMng.m_pWndSecretRoomMsg );

	g_DPlay.SendSecretRoomEntrance();
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_SecretRoomNext( CScanner& s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	CSecretRoomMng::GetInstance()->m_dwRemainTime = 0;
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_SecretRoomTender( CScanner& s )
{
#ifdef __CLIENT
	g_DPlay.SendSecretRoomTenderOpenWnd();
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_SecretRoomLineUp( CScanner& s )
{
#ifdef __CLIENT
	g_DPlay.SendSecretRoomLineUpOpenWnd();
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_SecretRoomClose( CScanner& s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	
	if( CSecretRoomMng::GetInstance()->m_nState == SRMNG_WAR )
	{
		map<BYTE, CSecretRoomContinent*>::iterator it = CSecretRoomMng::GetInstance()->m_mapSecretRoomContinent.begin();
		for( ; it!=CSecretRoomMng::GetInstance()->m_mapSecretRoomContinent.end(); it++ )
		{
			CSecretRoomContinent* pSRCont = it->second;
			if( pSRCont && pSRCont->m_nState != SRCONT_CLOSE )
				pSRCont->m_dwRemainTime = 0;
				//pSRCont->SetContCloseWait();
		}
	}
	else	
		pUser->AddText( "Is Not War State!!!" );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_SecretRoomTenderView( CScanner& s )
{
#ifdef __CLIENT
	g_DPlay.SendSecretRoomTenderView();

	SAFE_DELETE(g_WndMng.m_pWndSecretRoomCheckTaxRate);
	g_WndMng.m_pWndSecretRoomCheckTaxRate = new CWndSecretRoomCheckTaxRate;

	if(g_WndMng.m_pWndSecretRoomCheckTaxRate)
	{
		g_WndMng.m_pWndSecretRoomCheckTaxRate->Initialize(NULL);
	}
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_SecretRoomTenderCancelReturn( CScanner& s )
{
#ifdef __CLIENT
	g_DPlay.SendSecretRoomTenderCancelReturn();
#endif // __CLIENT
	return TRUE;
}
#endif // __SECRET_ROOM

#if __VER >= 12 // __LORD
BOOL TextCmd_ElectionRequirement( CScanner& s )
{
#ifdef __CLIENT
	IElection* pElection	= CCLord::Instance()->GetElection();
	char lpString[100]		= { 0,};
	sprintf( lpString, "election state : total(%d)/requirement(%d)", pElection->GetVote(), pElection->GetRequirement() );
	g_WndMng.PutString( lpString );
#endif	// __CLIENT
	return TRUE;
}

//#ifdef __INTERNALSERVER
BOOL TextCmd_RemoveTotalGold( CScanner& s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	__int64 iGold	= static_cast<__int64>( s.GetInt64() );
	if( iGold > pUser->GetTotalGold() )
	{
		char szText[100]	= { 0,};
		sprintf( szText, "TextCmd_RemoveTotalGold: %I64d", pUser->GetTotalGold() );
		pUser->AddText( szText );
	}
	else
		pUser->RemoveTotalGold( iGold );
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_ElectionAddDeposit( CScanner& s )
{
#ifdef __CLIENT
	__int64 iDeposit	= static_cast<__int64>( s.GetNumber() );
	g_DPlay.SendElectionAddDeposit( iDeposit );
#endif	// __CLIENT
	return TRUE;
}
BOOL TextCmd_ElectionSetPledge( CScanner& s )
{
#ifdef __CLIENT
	char szPledge[CCandidate::nMaxPledgeLen]	= { 0,};
	
	s.GetLastFull();
	if( lstrlen( s.token ) >= CCandidate::nMaxPledgeLen )
		return TRUE;
	lstrcpy( szPledge, s.token );
	StringTrimRight( szPledge );
//	RemoveCRLF( szPledge );
	g_DPlay.SendElectionSetPledge( szPledge );
#endif	// __CLIENT
	return TRUE;
}

BOOL TextCmd_ElectionIncVote( CScanner& s )
{
#ifdef __CLIENT
	s.GetToken();
	u_long idPlayer		= CPlayerDataCenter::GetInstance()->GetPlayerId( s.token );
	if( idPlayer > 0 )
		g_DPlay.SendElectionIncVote( idPlayer );
	else
		g_WndMng.PutString( prj.GetText( TID_GAME_ELECTION_INC_VOTE_E001 ) );
#endif	// __CLIENT
	return TRUE;
}

BOOL TextCmd_ElectionProcess( CScanner& s )
{
#ifdef __WORLDSERVER
	BOOL bRun	= static_cast<BOOL>( s.GetNumber() );
	g_dpDBClient.SendElectionProcess( bRun );
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_ElectionBeginCandidacy( CScanner& s )
{
#ifdef __WORLDSERVER
	g_dpDBClient.SendElectionBeginCandidacy();
#endif	// __WORLDSERVER
	return TRUE;
}
BOOL TextCmd_ElectionBeginVote( CScanner& s )
{
#ifdef __WORLDSERVER
	g_dpDBClient.SendElectionBeginVote();
#endif	// __WORLDSERVER
	return TRUE;
}
BOOL TextCmd_ElectionEndVote( CScanner& s )
{
#ifdef __WORLDSERVER
	g_dpDBClient.SendElectionBeginEndVote();
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_ElectionState( CScanner& s )
{
#ifdef __CLIENT
	CCElection* pElection	= static_cast<CCElection*>( CCLord::Instance()->GetElection() );
	pElection->State();
#endif	// __CLIENT
	return TRUE;
}

BOOL TextCmd_LEventCreate( CScanner & s )
{
#ifdef __CLIENT
	int iEEvent		= s.GetNumber();
	int iIEvent		= s.GetNumber();
	g_DPlay.SendLEventCreate( iEEvent, iIEvent );
#endif	// __CLIENT
	return TRUE;
}

BOOL TextCmd_LEventInitialize( CScanner & s )
{
#ifdef __WORLDSERVER
	g_dpDBClient.SendLEventInitialize();
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_LSkill( CScanner & s )
{
#ifdef __CLIENT
	int nSkill	= s.GetNumber();
	s.GetToken();
	char szTarget[MAX_PLAYER]	= { 0,};
	strncpy( szTarget, s.token, MAX_PLAYER );
	szTarget[MAX_PLAYER-1]	= '\0';
	g_DPlay.SendLordSkillUse( nSkill, szTarget );
#endif	// __CLIENT
	return TRUE;
}
//#endif	// __INTERNALSERVER
#endif	// __LORD

#if __VER >= 12 // __MOD_TUTORIAL
BOOL TextCmd_SetTutorialState( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	int nTutorialState	= s.GetNumber();
	pUser->SetTutorialState( nTutorialState );
	pUser->AddSetTutorialState();
#endif	// __WORLDSERVER
	return TRUE;
}
#endif	// __MOD_TUTORIAL

#if __VER >= 12 // __TAX
BOOL TextCmd_TaxApplyNow( CScanner& s )
{
#ifdef __WORLDSERVER
	CTax::GetInstance()->SetApplyTaxRateNow();
#endif // __WORLDSERVER
	return TRUE;
}
#endif // __TAX

#if __VER >= 13 // __HONORABLE_TITLE			// 달인
BOOL TextCmd_HonorTitleSet( CScanner& s )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)s.dwValue;
	int nIdx = -1;
	s.GetToken();
	if( s.tokenType == NUMBER ) 
	{
		nIdx	= _ttoi( s.Token );
	}
	else
	{
		nIdx	= CTitleManager::Instance()->GetIdxByName( s.Token );
	}

	if( nIdx < 0 ||  nIdx >= MAX_HONOR_TITLE )
		return FALSE;

	DWORD dwNum	= s.GetNumber();
	if( dwNum > 100000000 ) dwNum = 100000000;
	if( dwNum < 0 ) dwNum = 0;

	pUser->SetHonorCount(nIdx,dwNum);
	pUser->AddHonorListAck();
	g_dpDBClient.SendLogGetHonorTime(pUser,nIdx);
#endif // __WORLDSERVER
	return TRUE;
}

#endif	// __HONORABLE_TITLE			// 달인

#if __VER >= 13 // __RAINBOW_RACE
BOOL TextCmd_RainbowRaceApp( CScanner& s )
{
#ifdef __CLIENT
	g_DPlay.SendRainbowRaceApplicationOpenWnd();
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_RainbowRacePass( CScanner& s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	CRainbowRace* pRainbowRace = CRainbowRaceMng::GetInstance()->GetRainbowRacerPtr( pUser->m_idPlayer );
	if( pRainbowRace )
	{
		pRainbowRace->SetNowGameComplete( pUser );
	}
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_RainbowRaceOpen( CScanner& s )
{
#ifdef __WORLDSERVER
	if( CRainbowRaceMng::GetInstance()->GetState() == CRainbowRaceMng::RR_CLOSED )
		CRainbowRaceMng::GetInstance()->SetState( CRainbowRaceMng::RR_OPEN );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_RainbowRaceNext( CScanner& s )
{
#ifdef __WORLDSERVER
	CRainbowRaceMng::GetInstance()->SetNextTime( 0 );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_RainbowRaceInfo( CScanner& s )
{
#ifdef __CLIENT
	if(g_WndMng.m_pWndRainbowRaceInfo)
		SAFE_DELETE(g_WndMng.m_pWndRainbowRaceInfo);

	g_WndMng.m_pWndRainbowRaceInfo = new CWndRainbowRaceInfo;

	if(g_WndMng.m_pWndRainbowRaceInfo)
		g_WndMng.m_pWndRainbowRaceInfo->Initialize(NULL);
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_RainbowRaceRule( CScanner& s )
{
#ifdef __CLIENT
	if(g_WndMng.m_pWndRainbowRaceRule)
		SAFE_DELETE(g_WndMng.m_pWndRainbowRaceRule);

	g_WndMng.m_pWndRainbowRaceRule = new CWndRainbowRaceRule;

	if(g_WndMng.m_pWndRainbowRaceRule)
		g_WndMng.m_pWndRainbowRaceRule->Initialize(NULL);
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_RainbowRaceRanking( CScanner& s )
{
#ifdef __CLIENT
	g_DPlay.SendRainbowRacePrevRankingOpenWnd();
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_RainbowRacePrize( CScanner& s )
{
#ifdef __CLIENT
	if(g_WndMng.m_pWndRainbowRacePrize)
		SAFE_DELETE(g_WndMng.m_pWndRainbowRacePrize);

	g_WndMng.m_pWndRainbowRacePrize = new CWndRainbowRacePrize;

	if(g_WndMng.m_pWndRainbowRacePrize)
		g_WndMng.m_pWndRainbowRacePrize->Initialize(NULL);
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_RainbowRaceKawiBawiBo( CScanner& s )
{
#ifdef __CLIENT
	CRainbowRace::GetInstance()->SendMinigamePacket( RMG_GAWIBAWIBO, MP_OPENWND );
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_RainbowRaceDice( CScanner& s )
{
#ifdef __CLIENT
	CRainbowRace::GetInstance()->SendMinigamePacket( RMG_DICEPLAY, MP_OPENWND );
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_RainbowRaceArithmetic( CScanner& s )
{
#ifdef __CLIENT
	CRainbowRace::GetInstance()->SendMinigamePacket( RMG_ARITHMATIC, MP_OPENWND );
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_RainbowRaceStopWatch( CScanner& s )
{
#ifdef __CLIENT
	CRainbowRace::GetInstance()->SendMinigamePacket( RMG_STOPWATCH, MP_OPENWND );
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_RainbowRaceTyping( CScanner& s )
{
#ifdef __CLIENT
	CRainbowRace::GetInstance()->SendMinigamePacket( RMG_TYPING, MP_OPENWND );
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_RainbowRaceCard( CScanner& s )
{
#ifdef __CLIENT
	CRainbowRace::GetInstance()->SendMinigamePacket( RMG_PAIRGAME, MP_OPENWND );
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_RainbowRaceLadder( CScanner& s )
{
#ifdef __CLIENT
	CRainbowRace::GetInstance()->SendMinigamePacket( RMG_LADDER, MP_OPENWND );
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_RainbowRaceReqFininsh( CScanner& s )
{
#ifdef __CLIENT
	g_DPlay.SendRainbowRaceReqFinish();
#endif // __CLIENT
	return TRUE;
}

#endif // __RAINBOW_RACE

#if __VER >= 13 // __EXT_ENCHANT
BOOL TextCmd_ChangeAttribute( CScanner& s )
{
#ifdef __CLIENT
	if(g_WndMng.m_pWndChangeAttribute)
		SAFE_DELETE(g_WndMng.m_pWndChangeAttribute);

	g_WndMng.m_pWndChangeAttribute = new CWndChangeAttribute;

	if(g_WndMng.m_pWndChangeAttribute)
		g_WndMng.m_pWndChangeAttribute->Initialize(NULL);
#endif // __CLIENT
	return TRUE;
}
#endif //__EXT_ENCHANT

#if __VER >= 13 // __HOUSING
BOOL TextCmd_HousingVisitRoom( CScanner& s )
{
#ifdef __CLIENT
	s.GetToken();
	if( s.Token == "" )	// 아무 값 없으면 내방으로...
		g_DPlay.SendHousingVisitRoom( g_pPlayer->m_idPlayer );
	else	// 캐릭터명이 있으면 해당 캐릭터의 방으로...
	{
		DWORD dwPlayerId = CPlayerDataCenter::GetInstance()->GetPlayerId( s.token );
		if( dwPlayerId )
			g_DPlay.SendHousingVisitRoom( dwPlayerId );
	}
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_HousingGMRemoveAll( CScanner& s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	CHousingMng::GetInstance()->ReqGMFunrnitureListAll( pUser );
#endif // __WORLDSERVER
	return TRUE;
}
#endif // __HOUSING
/*
#if __VER >= 14 // __SMELT_SAFETY
BOOL TextCmd_SmeltSafetyNormal( CScanner& s )
{
#ifdef __CLIENT
	if(g_WndMng.m_pWndSmeltSafety)
		SAFE_DELETE(g_WndMng.m_pWndSmeltSafety);

	g_WndMng.m_pWndSmeltSafety = new CWndSmeltSafety(CWndSmeltSafety::WND_NORMAL);
	if(g_WndMng.m_pWndSmeltSafety)
	{
		g_WndMng.m_pWndSmeltSafety->Initialize(NULL);
	}
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_SmeltSafetyAccessary( CScanner& s )
{
#ifdef __CLIENT
	if(g_WndMng.m_pWndSmeltSafety)
		SAFE_DELETE(g_WndMng.m_pWndSmeltSafety);

	g_WndMng.m_pWndSmeltSafety = new CWndSmeltSafety(CWndSmeltSafety::WND_ACCESSARY);
	if(g_WndMng.m_pWndSmeltSafety)
	{
		g_WndMng.m_pWndSmeltSafety->Initialize(NULL);
	}
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_SmeltSafetyPiercing( CScanner& s )
{
#ifdef __CLIENT
	if(g_WndMng.m_pWndSmeltSafety)
		SAFE_DELETE(g_WndMng.m_pWndSmeltSafety);

	g_WndMng.m_pWndSmeltSafety = new CWndSmeltSafety(CWndSmeltSafety::WND_PIERCING);
	if(g_WndMng.m_pWndSmeltSafety)
	{
		g_WndMng.m_pWndSmeltSafety->Initialize(NULL);
	}
#endif // __CLIENT
	return TRUE;
}

#endif //__SMELT_SAFETY
*/
#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
BOOL TextCmd_SmeltSafetyElement( CScanner& s )
{
#ifdef __CLIENT
	if( g_WndMng.m_pWndSmeltSafety )
		SAFE_DELETE( g_WndMng.m_pWndSmeltSafety );

	g_WndMng.m_pWndSmeltSafety = new CWndSmeltSafety( CWndSmeltSafety::WND_ELEMENT );
	if( g_WndMng.m_pWndSmeltSafety )
		g_WndMng.m_pWndSmeltSafety->Initialize( NULL );
#endif // __CLIENT
	return TRUE;
}
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY

#ifdef __QUIZ
BOOL TextCmd_QuizEventOpen( CScanner& s )
{
#ifdef __WORLDSERVER
	if( !CQuiz::GetInstance()->IsRun() )
		g_dpDBClient.SendQuizEventOpen( CQuiz::GetInstance()->GetType() );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_QuizEventEnterance( CScanner& s )
{
#ifdef __CLIENT
	g_DPlay.SendQuizEventEntrance();
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_QuizStateNext( CScanner& s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	if( CQuiz::GetInstance()->IsRun() )
	{
		if( IsValidObj( pUser ) && pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_QUIZ )
		{
			CQuiz::GetInstance()->SetNextTime( 0 );
			CQuiz::GetInstance()->Process();
		}
	}
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_QuizEventClose( CScanner& s )
{
#ifdef __WORLDSERVER
	if( CQuiz::GetInstance()->IsRun() )
		CQuiz::GetInstance()->CloseQuizEvent();
#endif // __WORLDSERVER
	return TRUE;
}
#endif // __QUIZ

#if __VER >= 15 // __GUILD_HOUSE
BOOL TextCmd_BuyGuildHouse( CScanner& s )
{
#ifdef __CLIENT
	g_DPlay.SendBuyGuildHouse();
#endif // __CLIENT
	return TRUE;
}

#ifdef __CLIENT
#include "WndGuildHouse.h"
#endif // __CLIENT
BOOL TextCmd_GuildHouseUpkeep( CScanner & s )
{
#ifdef __CLIENT
	if( !g_WndMng.m_pWndUpkeep )
	{
		g_WndMng.m_pWndUpkeep = new CWndGHUpkeep;
		g_WndMng.m_pWndUpkeep->Initialize( &g_WndMng, APP_CONFIRM_BUY_ );
	}
	else
	{
		g_WndMng.m_pWndUpkeep->Destroy( );
		g_WndMng.m_pWndUpkeep = NULL;
	}
#endif // __CLIENT
	return TRUE;
}
#endif // __GUILD_HOUSE

#if __VER >= 15 // __CAMPUS
BOOL TextCmd_CampusInvite( CScanner& s )
{
#ifdef __WORLDSERVER
	CUser* pRequest = (CUser*)s.dwValue;
	if( !IsValidObj( pRequest ) )
		return FALSE;

	s.GetToken();
	u_long idTarget	= CPlayerDataCenter::GetInstance()->GetPlayerId( s.token );

	if( 0 < idTarget )
	{
		CUser* pTarget	= g_UserMng.GetUserByPlayerID( idTarget );	
		if( IsValidObj( pTarget ) )
		{
			CCampusHelper::GetInstance()->OnInviteCampusMember( pRequest, pTarget );
			PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->GetPlayerData( idTarget );
			if( pPlayerData )
				pRequest->AddQueryPlayerData( idTarget, pPlayerData );
		}

		else
			pRequest->AddDefinedText( TID_DIAG_0061, "\"%s\"", s.Token );
	}
	else
		pRequest->AddDefinedText( TID_DIAG_0060, "\"%s\"", s.Token );
#endif // __WORLDSERVER

	return TRUE;
}

BOOL TextCmd_RemoveCampusMember( CScanner& s )
{
#ifdef __WORLDSERVER
	CUser* pRequest = (CUser*)s.dwValue;
	if( !IsValidObj( pRequest ) )
		return FALSE;

	s.GetToken();
	u_long idTarget	= CPlayerDataCenter::GetInstance()->GetPlayerId( s.token );

	if( idTarget > 0 )
		CCampusHelper::GetInstance()->OnRemoveCampusMember( pRequest, idTarget );
		
	else
		pRequest->AddDefinedText( TID_DIAG_0060, "\"%s\"", s.Token );

#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_UpdateCampusPoint( CScanner& s )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)s.dwValue;
	if( !IsValidObj( pUser ) )
		return FALSE;

	int nCampusPoint = 0;
	nCampusPoint = s.GetNumber();

	if( IsValidObj( pUser ) )
		g_dpDBClient.SendUpdateCampusPoint( pUser->m_idPlayer, nCampusPoint, TRUE, 'G' );
#endif // __WORLDSERVER
	return TRUE;
}
#endif // __CAMPUS

BOOL TextCmd_InvenRemove( CScanner& scanner )       
{ 
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	if( IsValidObj( pUser ) )
	{
		int nSize = pUser->m_Inventory.GetMax();
		for( int i = 0 ; i < nSize; ++i )
		{
			CItemElem* pItemElem = pUser->m_Inventory.GetAtId( i );
			if( IsUsableItem( pItemElem ) && !pUser->m_Inventory.IsEquip( pItemElem->m_dwObjId ) && !pItemElem->IsPet() && !pItemElem->IsEatPet() )
				pUser->UpdateItem( i, UI_NUM, 0 );
		}
	}
#endif	// __WORLDSERVER
	return TRUE;
}


BEGINE_TEXTCMDFUNC_MAP
////////////////////////////////////////////////// AUTH_GENERAL begin/////////////////////////////////////////////////////
	ON_TEXTCMDFUNC( TextCmd_whisper,               "whisper",           "w",              "귓속말",         "귓",      TCM_SERVER, AUTH_GENERAL      , "귓속말 [/명령 아이디 내용]" )
	ON_TEXTCMDFUNC( TextCmd_say,                   "say",               "say",            "말",             "말",      TCM_SERVER, AUTH_GENERAL      , "속삭임 [/명령 아이디 내용]" )
	ON_TEXTCMDFUNC( TextCmd_Position,              "position",          "pos",            "좌표",           "좌표",    TCM_CLIENT, AUTH_GENERAL      , "현재 좌표를 출력해준다." )
	ON_TEXTCMDFUNC( TextCmd_shout,                 "shout",             "s",              "외치기",         "외",      TCM_BOTH  , AUTH_GENERAL      , "외치기 [/명령 아이디 내용]" )
	ON_TEXTCMDFUNC( TextCmd_PartyChat,             "partychat",         "p",              "극단말",         "극",      TCM_BOTH  , AUTH_GENERAL      , "파티 채팅 [/명령 내용]" )
	ON_TEXTCMDFUNC( TextCmd_Time,                  "Time",              "ti",             "시간",           "시",      TCM_CLIENT, AUTH_GENERAL      , "시간 보기 [/시간]" )
//	ON_TEXTCMDFUNC( TextCmd_ChangeFace,            "ChangeFace",        "cf",             "얼굴변경",       "얼변",    TCM_BOTH  , AUTH_GENERAL      , "얼굴 변경" )
	ON_TEXTCMDFUNC( TextCmd_GuildChat,             "GuildChat",         "g",              "길드말",         "길말",    TCM_BOTH, AUTH_GENERAL      , "길드말" )
	ON_TEXTCMDFUNC( TextCmd_PartyInvite,           "PartyInvite",       "partyinvite",    "극단초청",       "극초",    TCM_SERVER, AUTH_GENERAL      , "극단 초청" )
	ON_TEXTCMDFUNC( TextCmd_GuildInvite,           "GuildInvite",       "guildinvite",    "길드초청",       "길초",    TCM_SERVER, AUTH_GENERAL      , "길드 초청" )
#if __VER >= 15 // __CAMPUS
	ON_TEXTCMDFUNC( TextCmd_CampusInvite,          "CampusInvite",		"campusinvite",   "사제초청",		"사초",    TCM_SERVER, AUTH_GENERAL      , "사제 초청" )
#endif // __CAMPUS
#ifdef __CLIENT
	ON_TEXTCMDFUNC( TextCmd_tradeagree,            "tradeagree",        "ta",             "거래승인",       "거승",    TCM_CLIENT, AUTH_GENERAL      , "거래 승인 [/명령] " )
	ON_TEXTCMDFUNC( TextCmd_traderefuse,           "traderefuse",       "tr",             "거래거절",       "거절",    TCM_CLIENT, AUTH_GENERAL      , "거래 거절 [/명령] " )
	ON_TEXTCMDFUNC( TextCmd_whisperagree,          "whisperagree",      "wa",             "귓속말승인",     "귓승",    TCM_CLIENT, AUTH_GENERAL      , "귓속말 승인 [/명령] " )
	ON_TEXTCMDFUNC( TextCmd_whisperrefuse,         "whisperrefuse",     "wr",             "귓속말거절",     "귓절",    TCM_CLIENT, AUTH_GENERAL      , "귓속말 거절 [/명령] " )
	ON_TEXTCMDFUNC( TextCmd_messengeragree,        "messengeragree",    "ma",             "메신저승인",     "메승",    TCM_CLIENT, AUTH_GENERAL      , "메신저 승인 [/명령] " )
	ON_TEXTCMDFUNC( TextCmd_messengerrefuse,       "messengerrefuse",   "mr",             "메신저거절",     "메절",    TCM_CLIENT, AUTH_GENERAL      , "메신저 거절 [/명령] " )
	ON_TEXTCMDFUNC( TextCmd_stageagree,            "stageagree",        "ga",             "극단승인",       "극승",    TCM_CLIENT, AUTH_GENERAL      , "극단 승인 [/명령] " )
	ON_TEXTCMDFUNC( TextCmd_stagerefuse,           "stagerefuse",       "gr",             "극단거절",       "극절",    TCM_CLIENT, AUTH_GENERAL      , "극단 거절 [/명령] " )
	ON_TEXTCMDFUNC( TextCmd_connectagree,          "connectagree",      "ca",             "접속알림",       "접알",    TCM_CLIENT, AUTH_GENERAL      , "접속알림 [/명령] " )
	ON_TEXTCMDFUNC( TextCmd_connectrefuse,         "connectrefuse",     "cr",             "접속알림해제",   "접해",    TCM_CLIENT, AUTH_GENERAL      , "접속알림 해제 [/명령] " )
	ON_TEXTCMDFUNC( TextCmd_shoutagree,            "shoutagree",        "ha",             "외치기승인",     "외승",    TCM_CLIENT, AUTH_GENERAL      , "외치기 승인 [/명령] " )
	ON_TEXTCMDFUNC( TextCmd_shoutrefuse,           "shoutrefuse",       "hr",             "외치기해제",     "외해",    TCM_CLIENT, AUTH_GENERAL      , "외치기 거절 [/명령] " )
#ifdef __YS_CHATTING_BLOCKING_SYSTEM
	ON_TEXTCMDFUNC( TextCmd_BlockUser,             "ignore",             "ig",             "채팅차단",       "채차",    TCM_CLIENT, AUTH_GENERAL      , "채팅차단 [/명령 아이디]" )
	ON_TEXTCMDFUNC( TextCmd_CancelBlockedUser,     "unignore",           "uig",            "채팅차단해제",   "채차해",  TCM_CLIENT, AUTH_GENERAL      , "채팅차단해제 [/명령 아이디]" )
	ON_TEXTCMDFUNC( TextCmd_IgnoreList,            "ignorelist",         "igl",            "채팅차단목록",   "채차목",  TCM_CLIENT, AUTH_GENERAL      , "채팅 차단 목록" )
#endif // __YS_CHATTING_BLOCKING_SYSTEM
#endif //__CLIENT
////////////////////////////////////////////////// AUTH_GENERAL end/////////////////////////////////////////////////////
	// GM_LEVEL_1
	ON_TEXTCMDFUNC( TextCmd_Teleport,              "teleport",          "te",             "텔레포트",       "텔레",    TCM_SERVER, AUTH_GAMEMASTER   , "텔레포트" )
	ON_TEXTCMDFUNC( TextCmd_Invisible,             "invisible",         "inv",            "투명",           "투명",    TCM_SERVER, AUTH_GAMEMASTER   , "투명화" )
	ON_TEXTCMDFUNC( TextCmd_NoInvisible,           "noinvisible",       "noinv",          "투명해제",       "투해",    TCM_SERVER, AUTH_GAMEMASTER   , "투명화 해제" )
	ON_TEXTCMDFUNC( TextCmd_Summon,                "summon",            "su",             "소환",           "소환",    TCM_SERVER, AUTH_GAMEMASTER   , "유저소환" )
	ON_TEXTCMDFUNC( TextCmd_count,                 "count",             "cnt",            "접속자수",       "접속자수",TCM_SERVER, AUTH_GENERAL   , "접속자 카운트" )
	
	// GM_LEVEL_2
	ON_TEXTCMDFUNC( TextCmd_Out,                   "out",               "out",            "퇴출",           "퇴출",    TCM_SERVER, AUTH_GAMEMASTER2   , "퇴출" )
	ON_TEXTCMDFUNC( TextCmd_Talk,                  "talk",              "nota",           "말해제",         "말해",    TCM_SERVER, AUTH_GAMEMASTER2   , "말하지 못하게 하기 해제" )
	ON_TEXTCMDFUNC( TextCmd_NoTalk,                "notalk",            "ta",             "말정지",         "말정",    TCM_SERVER, AUTH_GAMEMASTER2   , "말하지 못하게 하기" )
	ON_TEXTCMDFUNC( TextCmd_ip,                    "ip",                "ip",             "아이피",         "아이피",  TCM_BOTH  , AUTH_GAMEMASTER2     , "상대 IP알기" )
#ifdef __JEFF_9_20
	ON_TEXTCMDFUNC( TextCmd_Mute,					"Mute",				"mute",          "조용히",     "조용히",    TCM_SERVER,  AUTH_GAMEMASTER2, "" )	
#endif	// __JEFF_9_20
	ON_TEXTCMDFUNC( TextCmd_GuildRanking,          "GuildRanking",       "ranking",        "길랭",           "길랭",    TCM_SERVER, AUTH_GAMEMASTER2   , "" )
	ON_TEXTCMDFUNC( TextCmd_FallSnow,              "FallSnow",           "fs",             "눈와라",         "눈와",    TCM_SERVER, AUTH_GAMEMASTER2   , "눈 내리기 토글" )
	ON_TEXTCMDFUNC( TextCmd_StopSnow,              "StopSnow",           "ss",             "눈그만",         "눈끝",    TCM_SERVER, AUTH_GAMEMASTER2   , "눈 내리기 못하게 토글" )
	ON_TEXTCMDFUNC( TextCmd_FallRain,              "FallRain",           "frain",          "비와라",         "비와",    TCM_SERVER, AUTH_GAMEMASTER2   , "비 내리기 토글" )
	ON_TEXTCMDFUNC( TextCmd_StopRain,              "StopRain",           "sr",             "비그만",         "비끝",    TCM_SERVER, AUTH_GAMEMASTER2   , "비 내리기 못하게 토글" )
	ON_TEXTCMDFUNC( TextCmd_System,                "system",             "sys",            "알림",           "알",      TCM_SERVER, AUTH_GAMEMASTER2   , "시스템 메시지" )

	ON_TEXTCMDFUNC( TextCmd_SystemName,			   "systemname",		 "sysn",			"sysn",			"sysn",		TCM_SERVER, AUTH_GAMEMASTER2   , "시스템 메시지" )
	// GM_LEVEL_3
	ON_TEXTCMDFUNC( TextCmd_PvpParam,              "PvpParam",           "p_Param",        "PVP설정",        "피설",    TCM_SERVER, AUTH_GAMEMASTER3, "PVP(카오)설정" )
#if __VER >= 8 // __S8_PK
	ON_TEXTCMDFUNC( TextCmd_PKParam,			   "PKParam",			 "pkparam",		   "PK설정",		 "pk설정",  TCM_SERVER, AUTH_GAMEMASTER3, "카오설정" )
#endif // __VER >= 8 // __S8_PK
	ON_TEXTCMDFUNC( TextCmd_Undying,               "undying",            "ud",             "무적",           "무",      TCM_BOTH  , AUTH_GAMEMASTER3   , "무적" )
	ON_TEXTCMDFUNC( TextCmd_Undying2,              "undying2",           "ud2",            "반무적",         "반무",    TCM_BOTH  , AUTH_GAMEMASTER3   , "반무적" )
	ON_TEXTCMDFUNC( TextCmd_NoUndying,             "noundying",          "noud",           "무적해제",       "무해",    TCM_BOTH  , AUTH_GAMEMASTER3   , "무적 해제" )
	ON_TEXTCMDFUNC( TextCmd_Onekill,               "onekill",            "ok",             "초필",           "초필",    TCM_BOTH  , AUTH_GAMEMASTER3   , "적을 한방에 죽이기" )
	ON_TEXTCMDFUNC( TextCmd_NoOnekill,             "noonekill",          "nook",           "초필해제",       "초해",    TCM_BOTH  , AUTH_GAMEMASTER3   , "적을 한방에 죽이기 해제" )
	ON_TEXTCMDFUNC( TextCmd_AroundKill,            "aroundkill",         "ak",             "원샷",           "원",      TCM_SERVER, AUTH_GAMEMASTER3   , "어라운드에 있는 몬스터 죽이기" )
	ON_TEXTCMDFUNC( TextCmd_stat,                  "stat",               "stat",           "스탯",           "스탯",    TCM_SERVER, AUTH_GAMEMASTER3   , "스탯 설정 하기" )
	ON_TEXTCMDFUNC( TextCmd_Level,                 "level",              "lv",             "레벨",           "렙",      TCM_SERVER, AUTH_GAMEMASTER3   , "레벨 설정 하기" )
	ON_TEXTCMDFUNC( TextCmd_InitSkillExp,          "InitSkillExp",       "InitSE",         "스킬초기화",     "스초",    TCM_SERVER, AUTH_GAMEMASTER3, "스킬초기화" )
	ON_TEXTCMDFUNC( TextCmd_SkillLevel,            "skilllevel",         "slv",            "스킬레벨",       "스렙",    TCM_BOTH  , AUTH_GAMEMASTER3   , "스킬레벨 설정 하기" )
	ON_TEXTCMDFUNC( TextCmd_SkillLevelAll,         "skilllevelAll",      "slvAll",         "스킬레벨올",     "스렙올",  TCM_BOTH  , AUTH_GAMEMASTER3   , "스킬레벨 설정 하기" )
	ON_TEXTCMDFUNC( TextCmd_BeginQuest,            "BeginQuest",         "bq",             "퀘스트시작",     "퀘시",    TCM_SERVER, AUTH_GAMEMASTER3, "퀘스트 시작 [ID]" )
	ON_TEXTCMDFUNC( TextCmd_EndQuest,              "EndQuest",           "eq",             "퀘스트종료",     "퀘종",    TCM_SERVER, AUTH_GAMEMASTER3, "퀘스트 종료 [ID]" )
	ON_TEXTCMDFUNC( TextCmd_RemoveQuest,           "RemoveQuest",        "rq",             "퀘스트제거",     "퀘제",    TCM_SERVER, AUTH_GAMEMASTER3, "퀘스트 제거 [ID]" )
	ON_TEXTCMDFUNC( TextCmd_RemoveAllQuest,        "RemoveAllQuest",     "raq",            "퀘스트전체제거", "퀘전제",  TCM_SERVER, AUTH_GAMEMASTER3, "퀘스트 전체 제거" )
	ON_TEXTCMDFUNC( TextCmd_RemoveCompleteQuest,   "RemoveCompleteQuest","rcq",            "퀘스트완료제거", "퀘완제",  TCM_SERVER, AUTH_GAMEMASTER3, "퀘스트 완료 제거" )
	ON_TEXTCMDFUNC( TextCmd_ChangeJob,             "changejob",			 "cjob",           "전직",           "전직",    TCM_SERVER, AUTH_GAMEMASTER3   , "전직 하기" )
	ON_TEXTCMDFUNC( TextCmd_Freeze,                "freeze",             "fr",             "정지",           "정지",    TCM_SERVER, AUTH_GAMEMASTER3   , "움직이지 못하게 하기" )
	ON_TEXTCMDFUNC( TextCmd_NoFreeze,              "nofreeze",           "nofr",           "정지해제",       "정해",    TCM_SERVER, AUTH_GAMEMASTER3   , "움직이지 못하게 하기 해제" )
	ON_TEXTCMDFUNC( TextCmd_PartyLevel,            "PartyLevel",         "plv",            "극단레벨",       "극레",    TCM_SERVER, AUTH_GAMEMASTER3   , "극단레벨 설정 하기" )
	ON_TEXTCMDFUNC( TextCmd_GuildStat,             "GuildStat",          "gstat",          "길드스탯",       "길스탯",  TCM_SERVER, AUTH_GAMEMASTER3   , "길드 스탯변경" )
	ON_TEXTCMDFUNC( TextCmd_CreateGuild,           "createguild",        "cg",             "길드생성",       "길생",    TCM_SERVER, AUTH_GAMEMASTER3   , "길드 생성" )
	ON_TEXTCMDFUNC( TextCmd_DestroyGuild,          "destroyguild",       "dg",             "길드해체",       "길해",    TCM_CLIENT, AUTH_GAMEMASTER3   , "길드 해체" )
	ON_TEXTCMDFUNC( TextCmd_GuildCombatIn,         "GCIn",               "gcin",           "길드워입장",     "길워입",  TCM_BOTH  , AUTH_GAMEMASTER3, "길드대전 입장" )
	ON_TEXTCMDFUNC( TextCmd_GuildCombatOpen,       "GCOpen",             "gcopen",         "길드워오픈",     "길워오",  TCM_BOTH  , AUTH_GAMEMASTER3, "길드대전 오픈" )
	ON_TEXTCMDFUNC( TextCmd_GuildCombatClose,      "GCClose",            "gcclose",        "길드워닫기",     "길워닫",  TCM_BOTH  , AUTH_GAMEMASTER3, "길드대전 닫기" )
	ON_TEXTCMDFUNC( TextCmd_GuildCombatNext,       "GCNext",             "gcNext",         "길드워다음",     "길워다",  TCM_BOTH  , AUTH_GAMEMASTER3, "길드대전 다음" )	
	ON_TEXTCMDFUNC( TextCmd_indirect,              "indirect",           "id",             "간접",           "간접",    TCM_BOTH  , AUTH_GAMEMASTER3   , "상대에게 간접으로 말하게 하기" )
	ON_TEXTCMDFUNC( TextCmd_CreateNPC,             "createnpc",          "cn",             "엔피씨생성",     "엔생",    TCM_SERVER, AUTH_GAMEMASTER3   , "npc생성" )
#if __VER >= 9 // __EVENTLUA
	ON_TEXTCMDFUNC( TextCmd_LuaEventList,     "EVENTLIST",         "eventlist",          "이벤트목록",     "이벤트목록",    TCM_SERVER,  AUTH_GAMEMASTER3, "" )
	ON_TEXTCMDFUNC( TextCmd_LuaEventInfo,     "EVENTINFO",         "eventinfo",          "이벤트정보",     "이벤트정보",    TCM_SERVER,  AUTH_GAMEMASTER3, "" )	
#endif	// __EVENTLUA
	ON_TEXTCMDFUNC( TextCmd_GameSetting,           "gamesetting",        "gs",             "게임설정",       "게설",    TCM_SERVER, AUTH_GAMEMASTER3   , "게임 설정 보기" )
	ON_TEXTCMDFUNC( TextCmd_RemoveNpc,             "rmvnpc",             "rn",             "삭제",           "삭",      TCM_SERVER, AUTH_GAMEMASTER3, "NPC삭제" )

	// GM_LEVEL_4
	ON_TEXTCMDFUNC( TextCmd_outall,                "outall",            "outall",         "접속자수",       "접속자수",TCM_BOTH,   AUTH_ADMINISTRATOR    , "접속자 카운트" )  
	ON_TEXTCMDFUNC( TextCmd_Disguise,				"disguise",           "dis",            "변신",           "변",      TCM_SERVER, AUTH_ADMINISTRATOR   , "변신" )
	ON_TEXTCMDFUNC( TextCmd_NoDisguise,				"noDisguise",         "nodis",          "변신해제",       "변해",    TCM_SERVER, AUTH_ADMINISTRATOR   , "변신 해제" )
	ON_TEXTCMDFUNC( TextCmd_ResistItem,				"ResistItem",         "ritem",          "속성아이템",     "속아",    TCM_BOTH  , AUTH_ADMINISTRATOR, "속성아이템" )
	ON_TEXTCMDFUNC( TextCmd_JobName,				"jobname",            "jn",             "직업이름",       "직이",    TCM_CLIENT, AUTH_ADMINISTRATOR   , "직업이름 보기" )
	ON_TEXTCMDFUNC( TextCmd_GetGold,				"getgold",            "gg",             "돈줘",           "돈",      TCM_SERVER, AUTH_ADMINISTRATOR, "돈 얻기" )
	ON_TEXTCMDFUNC( TextCmd_CreateItem,				"createitem",         "ci",             "아이템생성",     "아생",    TCM_BOTH  , AUTH_ADMINISTRATOR, "아이템생성" )
	ON_TEXTCMDFUNC( TextCmd_CreateItem2,			"createitem2",        "ci2",            "아이템생성2",    "아생2",   TCM_SERVER, AUTH_ADMINISTRATOR, "아이템생성2" )
	ON_TEXTCMDFUNC( TextCmd_QuestState,				"QuestState",         "qs",             "퀘스트상태",     "퀘상",    TCM_SERVER, AUTH_ADMINISTRATOR, "퀘스트 설정 [ID] [State]" )
	ON_TEXTCMDFUNC( TextCmd_LoadScript,				"loadscript",         "loscr",          "로드스크립트",   "로스",    TCM_BOTH  , AUTH_ADMINISTRATOR   , "스크립트 다시 읽기" )
	ON_TEXTCMDFUNC( TextCmd_ReloadConstant,			"ReloadConstant",     "rec",            "리로드콘스탄트", "리콘",    TCM_SERVER, AUTH_ADMINISTRATOR, "리로드 콘스탄트파일" )
	ON_TEXTCMDFUNC( TextCmd_CTD,					"ctd",				 "ctd",            "이벤트듀얼존",   "이듀",    TCM_BOTH  , AUTH_ADMINISTRATOR   , "이벤트 듀얼존 설정" )
	ON_TEXTCMDFUNC( TextCmd_Piercing,				"Piercing",           "pier",           "피어싱",         "피싱",    TCM_BOTH  , AUTH_ADMINISTRATOR, "피어싱(소켓)" )
#if __VER >= 9	// __PET_0410
	ON_TEXTCMDFUNC( TextCmd_PetLevel,				"petlevel",         "pl",          "펫레벨",     "펫레",    TCM_BOTH,  AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_PetExp,					"petexp",         "pe",          "펫경험치",     "펫경",    TCM_BOTH,  AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_MakePetFeed,			"makepetfeed",         "mpf",          "먹이만들기",     "먹이",    TCM_BOTH,  AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_Pet,					"Pet",         "pet",          "펫",     "펫",    TCM_BOTH,  AUTH_ADMINISTRATOR, "" )
#endif	// __PET_0410
#if __VER >= 9 // __EVENTLUA
	ON_TEXTCMDFUNC( TextCmd_Lua,					"Lua",         "lua",          "루아",     "루아",    TCM_SERVER,  AUTH_ADMINISTRATOR, "" )
#endif	// __EVENTLUA
#if __VER >= 11 // __GUILD_COMBAT_1TO1
	ON_TEXTCMDFUNC( TextCmd_GC1to1Open,				"GC1TO1OPEN",		"gc1to1open",			"일대일대전오픈", "일오",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_GC1to1Close,			"GC1TO1CLOSE",		"gc1to1close",			"일대일대전닫기", "일닫",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_GC1to1Next,				"GC1TO1NEXT",		"gc1to1next",			"일대일대전다음", "일다",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_RefineAccessory,		"RefineAccessory",	"ra",	"액세서리제련", "액제",	TCM_BOTH,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_RefineCollector,		"RefineCollector",	"rc",	"채집기재련", "채제",	TCM_BOTH,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_GenRandomOption,		"GenRandomOption",	"gro",	"각성축복", "각축",	TCM_BOTH,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_InitializeRandomOption,	"InitializeRandomOption",	"iro",	"각성축복제거", "각축제거",	TCM_BOTH,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_SetRandomOption,		"SetRandomOption",	"sro",	"각성축복지정", "각지",	TCM_BOTH,	AUTH_ADMINISTRATOR, "" )
#endif // __GUILD_COMBAT_1TO1
#ifdef __PET_1024
	ON_TEXTCMDFUNC( TextCmd_SetPetName,             "SetPetName",           "setpetname",             "펫작명",       "펫작",    TCM_SERVER, AUTH_ADMINISTRATOR , "펫작명" )
	ON_TEXTCMDFUNC( TextCmd_ClearPetName,           "ClearPetName",           "cpn",             "펫작명취소",       "펫작취",    TCM_CLIENT, AUTH_ADMINISTRATOR , "펫작명취소" )
#endif	// __PET_1024
#if __VER >= 13 // __COUPLE_1117
	ON_TEXTCMDFUNC( TextCmd_Propose,				"Propose",           "propose",             "프러포즈",       "프러포즈",    TCM_SERVER, AUTH_ADMINISTRATOR , "프러포즈" )
	ON_TEXTCMDFUNC( TextCmd_Refuse,					"Refuse",           "refuse",             "프러포즈거절",       "프거",    TCM_SERVER, AUTH_ADMINISTRATOR , "프러포즈거절" )
	ON_TEXTCMDFUNC( TextCmd_Couple,					"Couple",           "couple",             "커플",       "커플",    TCM_SERVER, AUTH_ADMINISTRATOR , "커플" )
	ON_TEXTCMDFUNC( TextCmd_Decouple,				"Decouple",           "decouple",             "커플해지",       "커해",    TCM_SERVER, AUTH_ADMINISTRATOR , "커플해지" )
	ON_TEXTCMDFUNC( TextCmd_ClearPropose,           "ClearPropose",           "clearpropose",             "프러포즈초기화",       "프초",    TCM_SERVER, AUTH_ADMINISTRATOR , "프러포즈초기화" )
//	ON_TEXTCMDFUNC( TextCmd_CoupleState,            "CoupleState",           "couplestate",             "커플상태",       "커상",    TCM_CLIENT, AUTH_ADMINISTRATOR , "커플상태" )
#if __VER >= 13 // __COUPLE_1202
	ON_TEXTCMDFUNC( TextCmd_NextCoupleLevel,        "NextCoupleLevel",           "ncl",             "커플레벨업",       "커레",    TCM_SERVER, AUTH_ADMINISTRATOR , "커플레벨업" )
#endif	// __COUPLE_1202
#endif	// __COUPLE_1117
#ifdef __NPC_BUFF
	ON_TEXTCMDFUNC( TextCmd_RemoveAllBuff,			"RemoveBuff",		"rb",			"버프해제", "버해",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
#endif // __NPC_BUFF
	#if __VER >= 13 // __HONORABLE_TITLE			// 달인
	ON_TEXTCMDFUNC( TextCmd_HonorTitleSet,			"HonorTitleSet", "hts", "달인세팅", "달세", TCM_BOTH, AUTH_ADMINISTRATOR, "" )
#endif	// __HONORABLE_TITLE			// 달인




// 여기부터 국내만 
	ON_TEXTCMDFUNC( TextCmd_Open,                  "open",               "open",           "열기",           "열기",    TCM_CLIENT, AUTH_ADMINISTRATOR   , "" )
	ON_TEXTCMDFUNC( TextCmd_Close,                 "close",              "close",          "닫기",           "닫기",    TCM_CLIENT, AUTH_ADMINISTRATOR   , "" )
	ON_TEXTCMDFUNC( TextCmd_Music,                 "music",              "mu",             "음악",           "음악",    TCM_SERVER, AUTH_ADMINISTRATOR   , "배경음악" )
	ON_TEXTCMDFUNC( TextCmd_Sound,                 "sound",              "so",             "소리",           "소리",    TCM_SERVER, AUTH_ADMINISTRATOR   , "사운드 효과" )
	ON_TEXTCMDFUNC( TextCmd_LocalEvent,            "localevent",         "le",             "지역이벤트",     "지이",    TCM_SERVER, AUTH_ADMINISTRATOR   , "지역이벤트" )
	ON_TEXTCMDFUNC( TextCmd_CommercialElem,        "CommercialElem",     "CommercialElem", "속성강화창",     "속강",    TCM_CLIENT, AUTH_ADMINISTRATOR   , "속성강화창 띄우기" )
	ON_TEXTCMDFUNC( TextCmd_QuerySetPlayerName,    "SetPlayerName",      "spn",            "플레이어이름",   "플이",    TCM_SERVER, AUTH_ADMINISTRATOR   , "플레이어 이름 변경" )
	ON_TEXTCMDFUNC( TextCmd_QuerySetGuildName,     "SetGuildName",       "sgn",            "길드이름",       "길이",    TCM_SERVER, AUTH_ADMINISTRATOR   , "길드 이름 변경" )
	ON_TEXTCMDFUNC( TextCmd_DeclWar,               "DeclWar",            "declwar",        "길드전신청",     "길신",    TCM_CLIENT, AUTH_ADMINISTRATOR   , "길드전 신청" )
	ON_TEXTCMDFUNC( TextCmd_RemoveGuildMember,     "rgm",                "rgm",            "길드추방",       "길추",    TCM_CLIENT, AUTH_ADMINISTRATOR   , "길드 추방" )
	ON_TEXTCMDFUNC( TextCmd_GuildRankingDBUpdate,  "GuildRankingUpdate", "rankingupdate",  "길업",           "길업",    TCM_SERVER, AUTH_ADMINISTRATOR   , "" )
	ON_TEXTCMDFUNC( TextCmd_ItemMode,              "gmitem",             "gmitem",         "아이템모드",     "아모",    TCM_SERVER, AUTH_ADMINISTRATOR   , "아이템 못집고 못떨어트리게 하기" )
	ON_TEXTCMDFUNC( TextCmd_ItemNotMode,           "gmnotitem",          "gmnotitem",      "아이템해제",     "아모해",  TCM_SERVER, AUTH_ADMINISTRATOR   , "아이템 모드 해제" )
	ON_TEXTCMDFUNC( TextCmd_AttackMode,            "gmattck",            "gmattck",        "공격모드",       "공모",    TCM_SERVER, AUTH_ADMINISTRATOR   , "공격 못하게 하기" )
	ON_TEXTCMDFUNC( TextCmd_AttackNotMode,         "gmnotattck",         "gmnotattck",     "공격해제",       "공모해",  TCM_SERVER, AUTH_ADMINISTRATOR   , "공격 모드 해제" )
	ON_TEXTCMDFUNC( TextCmd_CommunityMode,         "gmcommunity",        "gmcommunity",    "커뮤니티모드",   "커모",    TCM_SERVER, AUTH_ADMINISTRATOR	, "길드, 파티, 친구, 거래, 상점 못하게 하기" )
	ON_TEXTCMDFUNC( TextCmd_CommunityNotMode,      "gmnotcommunity",     "gmnotcommunity", "커뮤니티해제",   "커모해",  TCM_SERVER, AUTH_ADMINISTRATOR   , "커뮤니티 모드 해체" )
	ON_TEXTCMDFUNC( TextCmd_ObserveMode,           "gmobserve",          "gmobserve",      "관전모드",       "관모",    TCM_SERVER, AUTH_ADMINISTRATOR   , "아이템, 커뮤니티, 말못하게, 어텍 모드 합한것" )
	ON_TEXTCMDFUNC( TextCmd_ObserveNotMode,        "gmnotobserve",       "gmnotobserve",   "관전해제",       "관모해",  TCM_SERVER, AUTH_ADMINISTRATOR   , "관전 모드 해제" )
	ON_TEXTCMDFUNC( TextCmd_EscapeReset,           "EscapeReset",        "EscapeReset",    "탈출초기화",     "탈초",    TCM_BOTH  , AUTH_ADMINISTRATOR   , "탈출(귀환석) 시간 초기화" )
	ON_TEXTCMDFUNC( TextCmd_userlist,              "userlist",           "ul",             "사용자리스트",   "사용자리",TCM_SERVER, AUTH_ADMINISTRATOR, "사용자 리스트" )
	//ON_TEXTCMDFUNC( TextCmd_sbready,               "sbready",            "sbready",        "sbready",        "sbready", TCM_SERVER, AUTH_ADMINISTRATOR   , "sbready" )
	//ON_TEXTCMDFUNC( TextCmd_sbstart,               "sbstart",            "sbstart",        "sbstart",        "sbstart", TCM_SERVER, AUTH_ADMINISTRATOR   , "sbstart" )
	//ON_TEXTCMDFUNC( TextCmd_sbstart2,              "sbstart2",           "sbstart2",       "sbstart2",       "sbstart2",TCM_SERVER, AUTH_ADMINISTRATOR   , "sbstart2" )
	//ON_TEXTCMDFUNC( TextCmd_sbend,                 "sbend",              "sbend",          "sbend",          "sbend",   TCM_SERVER, AUTH_ADMINISTRATOR   , "sbend" )
	//ON_TEXTCMDFUNC( TextCmd_sbreport,              "sbreport",           "sbreport",       "sbreport",       "sbreport",TCM_SERVER, AUTH_ADMINISTRATOR   , "sbreport" )
	//ON_TEXTCMDFUNC( TextCmd_OpenBattleServer,      "bsopen",             "bsopen",         "bsopen",         "bsopen",  TCM_SERVER, AUTH_ADMINISTRATOR   , "bsopen" )
	//ON_TEXTCMDFUNC( TextCmd_CloseBattleServer,     "bsclose",            "bsclose",        "bsclose",        "bsclose", TCM_SERVER, AUTH_ADMINISTRATOR   , "bsclose" )
	ON_TEXTCMDFUNC( TextCmd_SetGuildQuest,         "SetGuildQuest",      "sgq",            "길드퀘스트",     "길퀘",    TCM_SERVER, AUTH_ADMINISTRATOR   , "길드 퀘스트 상태 변경" )
	ON_TEXTCMDFUNC( TextCmd_SetSnoop,              "Snoop",              "snoop",          "감청",           "감청",    TCM_SERVER, AUTH_ADMINISTRATOR   , "감청" )
	ON_TEXTCMDFUNC( TextCmd_SetSnoopGuild,         "SnoopGuild",         "sg",             "길드대화저장",   "길저",    TCM_SERVER, AUTH_ADMINISTRATOR   , "길드 대화 저장" )

	ON_TEXTCMDFUNC( TextCmd_GuildCombatRequest,    "GCRequest",          "gcrquest",       "길드워신청",     "길워신",  TCM_BOTH  , AUTH_ADMINISTRATOR, "길드대전 신청" )
	ON_TEXTCMDFUNC( TextCmd_GuildCombatCancel,     "GCCancel",           "gccancel",       "길드워탈퇴",     "길워탈",  TCM_BOTH  , AUTH_ADMINISTRATOR, "길드대전 탈퇴" )
	ON_TEXTCMDFUNC( TextCmd_PostMail,              "PostMail",           "pm",             "편지발송",       "발송",    TCM_CLIENT, AUTH_ADMINISTRATOR   , "편지 발송" )
	ON_TEXTCMDFUNC( TextCmd_RemoveMail,            "RemoveMail",         "rm",             "편지삭제",       "편삭",    TCM_CLIENT, AUTH_ADMINISTRATOR   , "편지 삭제" )
	ON_TEXTCMDFUNC( TextCmd_GetMailItem,           "GetMailItem",        "gm",             "소포받기",       "소포",    TCM_CLIENT, AUTH_ADMINISTRATOR   , "소포 받기" )
	ON_TEXTCMDFUNC( TextCmd_GetMailGold,           "GetMailGolg",        "gmg",            "수금",           "수금",    TCM_CLIENT, AUTH_ADMINISTRATOR   , "수금" )
	ON_TEXTCMDFUNC( TextCmd_InvenClear,            "InvenClear",         "icr",            "인벤청소",       "인청",    TCM_SERVER, AUTH_ADMINISTRATOR, "인벤토리의 내용을 모두 삭제" )
	ON_TEXTCMDFUNC( TextCmd_ExpUpStop,             "ExpUpStop",          "es",             "경험치금지",     "경금",    TCM_SERVER, AUTH_ADMINISTRATOR, "사냥으로 오르는 경험치 상승을 금지" )

#ifdef _DEBUG
	ON_TEXTCMDFUNC( TextCmd_CreateChar,            "createchar",         "cc",             "캐릭터생성",     "캐생",    TCM_SERVER, AUTH_ADMINISTRATOR   , "캐릭터생성" )
	ON_TEXTCMDFUNC( TextCmd_CreateCtrl,            "createctrl",         "ct",             "컨트롤생성",     "컨생",    TCM_SERVER, AUTH_ADMINISTRATOR   , "ctrl생성" )
	ON_TEXTCMDFUNC( TextCmd_SetMonsterRespawn,     "setmonsterrespawn",  "smr",            "리스폰영역설정", "리영설",  TCM_SERVER, AUTH_ADMINISTRATOR, "리스폰 영역 설정" )
	ON_TEXTCMDFUNC( TextCmd_TransyItemList,        "TransyItemList",     "til",            "트랜지리스트",   "트아리",  TCM_CLIENT, AUTH_ADMINISTRATOR, "트랜지아이템리스트" )
	ON_TEXTCMDFUNC( TextCmd_LoadToolTipColor,      "LoadToolTip",        "ltt",            "로드툴팁",       "로툴팁",  TCM_CLIENT, AUTH_ADMINISTRATOR, "로드 툴팁 컬러" )
#endif

#ifdef __S1108_BACK_END_SYSTEM
	ON_TEXTCMDFUNC( TextCmd_PropMonster,           "monstersetting",     "ms",             "몬스터설정",     "몬설",    TCM_CLIENT, AUTH_ADMINISTRATOR   , "몬스터 설정 보기" )
#else
	ON_TEXTCMDFUNC( TextCmd_ChangeShopCost,	       "changeshopcost",	 "csc",	           "상점가격조정",   "상가조",  TCM_SERVER, AUTH_ADMINISTRATOR, "상점가격조정 Min(0.5) ~ Max(2.0)"  )
#endif // __S1108_BACK_END_SYSTEM

#ifdef __EVENT_1101
	ON_TEXTCMDFUNC( TextCmd_CallTheRoll,			"CallTheRoll",        "ctr",            "출석설정",       "출석",  TCM_BOTH,	AUTH_ADMINISTRATOR, "출석 조작 명령어" )
#endif	// __EVENT_1101
	
#if __VER >= 8 //__CSC_VER8_5
	ON_TEXTCMDFUNC( TextCmd_AngelExp,				"AExp",		"aexp",			"엔젤경험치", "엔경",	TCM_SERVER, AUTH_ADMINISTRATOR, "" )
#endif // __CSC_VER8_5
	
#if __VER >= 10 // __REMOVE_ATTRIBUTE
	ON_TEXTCMDFUNC( TextCmd_RemoveAttribute,		"RemAttr",		"remattr",			"속성제거", "속제",	TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
#endif // __REMOVE_ATTRIBUTE

#if __VER >= 11 // __SYS_COLLECTING
	ON_TEXTCMDFUNC( TextCmd_StartCollecting,		"StartCollecting",	"col1",	"채집시작", "채시",	TCM_BOTH,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_StopCollecting,			"StopCollecting",	"col2",	"채집끝", "채끝",	TCM_BOTH,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_DoUseItemBattery,		"Battery",	"battery",	"채집기충전", "채충",	TCM_BOTH,	AUTH_ADMINISTRATOR, "" )
#endif	// __SYS_COLLECTING

#if __VER >= 11 // __SYS_POCKET
	ON_TEXTCMDFUNC( TextCmd_AvailPocket,			"AvailPocket",	"ap",	"주머니사용", "주사",	TCM_BOTH,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_PocketView,				"PocketView",	"pv",	"주머니보기", "주보",	TCM_BOTH,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_MoveItem_Pocket,		"MoveItemPocket",	"mip",	"아이템이동", "아이",	TCM_BOTH,	AUTH_ADMINISTRATOR, "" )
#endif	// __SYS_POCKET

#if __VER >= 11 // __SYS_IDENTIFY
	ON_TEXTCMDFUNC( TextCmd_ItemLevel,				"ItemLevel",	"il",	"하락", "하락",	TCM_BOTH,	AUTH_ADMINISTRATOR, "" )
#endif	// __SYS_IDENTIFY

#ifdef __EVENTLUA_COUPON
	ON_TEXTCMDFUNC( TextCmd_Coupon,					"COUPON",		"coupon",			"쿠폰설정", "쿠폰",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
#endif // __EVENTLUA_COUPON

//#ifdef __PERF_0226
	ON_TEXTCMDFUNC( TextCmd_CreatePc,				"CreatePc",		"cp",			"cp", "cp",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
//#endif	// __PERF_0226
#ifdef __SFX_OPT
	ON_TEXTCMDFUNC( TextCmd_SfxLv,					"SfxLevel",		"sl",			"sl", "sl",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
#endif	

#if __VER >= 12 // __SECRET_ROOM
	ON_TEXTCMDFUNC( TextCmd_SecretRoomOpen,			"SROPEN",		"sropen",			"비밀의방오픈", "비오",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_SecretRoomNext,			"SRNEXT",		"srnext",			"비밀의방다음", "비다",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_SecretRoomEntrance,		"SRENTRANCE",		"srentrance",			"비밀의방입장", "비입장",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_SecretRoomTender,		"SRTENDER",		"srtender",			"비밀의방입찰", "비입",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_SecretRoomLineUp,		"SRLINEUP",		"srlineup",			"비밀의방구성", "비구",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_SecretRoomClose,		"SRCLOSE",		"srclose",			"비밀의방닫기", "비닫",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_SecretRoomTenderView,	"SRVIEW",		"srview",			"비밀의방입찰현황", "비현",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_SecretRoomTenderCancelReturn, "SRCANCEL",		"srcancel",		"비밀의방입찰취소", "비취",	TCM_BOTH, AUTH_ADMINISTRATOR, "" )
#endif // __SECRET_ROOM

#if __VER >= 12 // __LORD
	ON_TEXTCMDFUNC( TextCmd_ElectionRequirement,	"ElectionRequirement", "er", "군주투표현황", "군투현", TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
//#ifdef __INTERNALSERVER
	ON_TEXTCMDFUNC( TextCmd_ElectionAddDeposit,		"ElectionAddDeposit", "ead", "군주입찰", "군입", TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_ElectionSetPledge,		"ElectionSetPledge", "esp", "군주공약설정", "군공", TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_ElectionIncVote,		"ElectionIncVote", "eiv", "군주투표", "군투", TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_ElectionProcess,		"ElectionProcess", "ep", "군주프로세스", "군프", TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_ElectionBeginCandidacy,	"ElectionBeginCandidacy", "ebc", "군주입후보시작", "군입시", TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_ElectionBeginVote,		"ElectionBeginVote", "ebv", "군주투표시작", "군투시", TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_ElectionEndVote,		"ElectionEndVote", "eev", "군주투표종료", "군투종", TCM_BOTH, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_ElectionState,			"ElectionState", "estate", "군주투표상태", "군투상", TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_LEventCreate,			"LEventCreate", "lecreate", "군주이벤트시작", "군이시", TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_LEventInitialize,		"LEventInitialize", "leinitialize", "군주이벤트초기화", "군이초", TCM_SERVER, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_LSkill,					"LSkill", "lskill", "군주스킬", "군스", TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_RemoveTotalGold,		"RemoveTotalGold", "rtg", "돈삭제", "돈삭", TCM_SERVER, AUTH_ADMINISTRATOR, "" )
//#endif	// __INTERNALSERVER
#endif	// __LORD

#if __VER >= 12 // __MOD_TUTORIAL
	ON_TEXTCMDFUNC( TextCmd_SetTutorialState,		"SetTutorialState", "sts", "튜토리얼레벨", "튜레", TCM_SERVER, AUTH_ADMINISTRATOR, "" )
#endif	// __MOD_TUTORIAL

#if __VER >= 12 // __TAX
	ON_TEXTCMDFUNC( TextCmd_TaxApplyNow,			"TaxApplyNow", "tan", "세율적용", "세적", TCM_BOTH, AUTH_ADMINISTRATOR, "" )
#endif // __TAX

#if __VER >= 12 // __HEAVEN_TOWER
	ON_TEXTCMDFUNC( TextCmd_HeavenTower,			"HeavenTower", "HTower", "심연의탑", "심탑", TCM_BOTH, AUTH_ADMINISTRATOR, "" )
#endif //__HEAVEN_TOWER

#if __VER >= 12 // __CSC_VER12_4
	ON_TEXTCMDFUNC( TextCmd_RemoveJewel,			"RemoveJewel", "RJewel", "보석제거", "보제", TCM_BOTH, AUTH_ADMINISTRATOR, "" )
#endif //__CSC_VER12_4

#if __VER >= 12 // __CSC_VER12_5
	ON_TEXTCMDFUNC( TextCmd_TransEggs,				"TransEggs", "TEggs", "알변환", "알변", TCM_BOTH, AUTH_ADMINISTRATOR, "" )
#endif //__CSC_VER12_5
	ON_TEXTCMDFUNC( TextCmd_PickupPetAwakeningCancel,	"PickupPetAwakeningCancel",	"ppac",	"픽업펫각성취소", "픽소",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )

#ifdef __LAYER_1020
	ON_TEXTCMDFUNC( TextCmd_CreateLayer,            "CreateLayer",           "cl",             "레이어생성",       "레생",    TCM_SERVER, AUTH_ADMINISTRATOR , "레이어생성" )
	ON_TEXTCMDFUNC( TextCmd_DeleteLayer,            "DeleteLayer",           "dl",             "레이어파괴",       "레파",    TCM_SERVER, AUTH_ADMINISTRATOR , "레이어파괴" )
	ON_TEXTCMDFUNC( TextCmd_Layer,					"Layer",           "lay",             "레이어이동",       "레이",    TCM_SERVER, AUTH_ADMINISTRATOR , "레이어이동" )
#endif	// __LAYER_1020

#if __VER >= 13 // __RAINBOW_RACE
	ON_TEXTCMDFUNC( TextCmd_RainbowRaceApp,			"RRApp",	"rrapp",	"레인보우신청", "레신",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_RainbowRaceOpen,		"RROpen",	"rropen",	"레인보우오픈", "레오",	TCM_SERVER,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_RainbowRaceNext,		"RRNext",	"rrnext",	"레인보우다음", "레다",	TCM_SERVER,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_RainbowRacePass,		"RRPass",	"rrpass",	"레인보우패스", "레패",	TCM_SERVER,	AUTH_ADMINISTRATOR, "" )
	
//	ON_TEXTCMDFUNC( TextCmd_RainbowRaceInfo,		"RRinfo",	"rrinfo",	"레인보우정보", "레정",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )
//	ON_TEXTCMDFUNC( TextCmd_RainbowRaceRule,		"RRRule",	"rrrule",	"레인보우규칙", "레규",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )
//	ON_TEXTCMDFUNC( TextCmd_RainbowRaceRanking,		"RRRanking","rrranking","레인보우랭킹", "레랭",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )
//	ON_TEXTCMDFUNC( TextCmd_RainbowRacePrize,		"RRPrize",	"rrprize",	"레인보우상품", "레상",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )

	ON_TEXTCMDFUNC( TextCmd_RainbowRaceKawiBawiBo,	"RRKawiBawiBo",	"rrkawibawibo",	"레인보우가위바위보", "레가",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_RainbowRaceDice,		"RRDice",	"rrdice",	"레인보우주사위", "레주",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_RainbowRaceArithmetic,	"RRArithmetic",	"rrarithmetic",	"레인보우수학", "레수",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )	
	ON_TEXTCMDFUNC( TextCmd_RainbowRaceStopWatch,	"RRStopWatch",	"rrstopwatch",	"레인보우스톱워치", "레스",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_RainbowRaceTyping,		"RRTyping",	"rrtyping",	"레인보우타자치기", "레타",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_RainbowRaceCard,		"RRCard",	"rrcard",	"레인보우카드", "레카",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_RainbowRaceLadder,		"RRLadder",	"rrladder",	"레인보우사다리", "레사",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_RainbowRaceReqFininsh,	"RRFINISH",	"rrfinish",	"레인보우완주", "레완",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )
#endif // __RAINBOW_RACE

//#ifdef __EXT_ENCHANT
//	ON_TEXTCMDFUNC( TextCmd_ChangeAttribute,		"CHATTRIBUTE",	"chattribute",	"속성변경", "속변",	TCM_CLIENT,	AUTH_ADMINISTRATOR, "" )
//#endif //__EXT_ENCHANT
#if __VER >= 13 // __HOUSING
	ON_TEXTCMDFUNC( TextCmd_HousingVisitRoom,		"HousingVisit",	"hv",	"방문", "방문",	TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_HousingGMRemoveAll,		"HousingGMRemoveAll",	"hgmra",	"가구삭제", "가삭",	TCM_SERVER, AUTH_ADMINISTRATOR, "" )
#endif // __HOUSING
/*
#if __VER >= 14 // __SMELT_SAFETY
	ON_TEXTCMDFUNC( TextCmd_SmeltSafetyNormal,		"SmeltSafetyNormal",	"ssn",	"안전제련일반", "안제일",	TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_SmeltSafetyAccessary,	"SmeltSafetyAccessary",	"ssa",	"안전제련악세", "안제악",	TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_SmeltSafetyPiercing,	"SmeltSafetyPiercing",	"ssp",	"안전제련피어싱", "안제피",	TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
#endif //__SMELT_SAFETY
*/
#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
	ON_TEXTCMDFUNC( TextCmd_SmeltSafetyElement,		"SmeltSafetyElement",	"sse",	"안전제련속성", "안제속",	TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY
#ifdef __QUIZ
	ON_TEXTCMDFUNC( TextCmd_QuizEventOpen,			"QuizEventOpen",		"qeo",		"퀴즈오픈", "퀴오", TCM_SERVER, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_QuizEventEnterance,		"QuizEventEnterance",	"qee",		"퀴즈입장", "퀴입", TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_QuizStateNext,			"QuizStateNext",		"qsn",		"퀴즈다음", "퀴다", TCM_SERVER, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_QuizEventClose,			"QuizEventClose",		"qec",		"퀴즈종료", "퀴종", TCM_SERVER, AUTH_ADMINISTRATOR, "" )
#endif // __QUIZ

#if __VER >= 15 // __GUILD_HOUSE
	ON_TEXTCMDFUNC( TextCmd_BuyGuildHouse,			"BuyGuildHouse",		"bgh",		"길드하우스구입",	"길하구", TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_GuildHouseUpkeep,		"GuildHouseUpkeep",		"ghu",		"길드하우스유지비",	"길하유", TCM_CLIENT, AUTH_ADMINISTRATOR, "" )
#endif // __GUILD_HOUSE

#if __VER >= 15 // __CAMPUS
	ON_TEXTCMDFUNC( TextCmd_RemoveCampusMember,		"RemoveCampusMember",	"rcm",		"사제해지",		"사해",		TCM_SERVER, AUTH_ADMINISTRATOR, "" )
	ON_TEXTCMDFUNC( TextCmd_UpdateCampusPoint,		"UpdateCampusPoint",	"ucp",		"사제포인트업",	"사포업",	TCM_SERVER, AUTH_ADMINISTRATOR, "" )
#endif // __CAMPUS
	ON_TEXTCMDFUNC( TextCmd_InvenRemove,            "InvenRemove",         "irm",       "인벤삭제",       "인삭",    TCM_SERVER, AUTH_ADMINISTRATOR, "" )
END_TEXTCMDFUNC_MAP


int ParsingCommand( LPCTSTR lpszString, CMover* pMover, BOOL bItem )
{
	CScanner scanner;
	scanner.SetProg( (LPTSTR)lpszString );
	scanner.dwValue	= (DWORD)pMover;
	scanner.GetToken(); // skip /
	scanner.GetToken(); // get command

	int nCount = 0;
	while( m_textCmdFunc[ nCount ].m_pFunc )
	{
		TextCmdFunc* pTextCmdFunc = &m_textCmdFunc[nCount];			// 해외 명령어 제한 
		if( ::GetLanguage() != LANG_KOR )
		{
			if( memcmp( pTextCmdFunc->m_pCommand, "open", 4 ) == 0 )
				break;
		}

		if( scanner.Token == pTextCmdFunc->m_pCommand || scanner.Token == pTextCmdFunc->m_pAbbreviation ||
			scanner.Token == pTextCmdFunc->m_pKrCommand || scanner.Token == pTextCmdFunc->m_pKrAbbreviation )

		{
		#ifdef __CLIENT
			/*if( scanner.Token == "disguise" || scanner.Token == "dis" || scanner.Token == "변신" || scanner.Token == "변" ||
				scanner.Token == "noDisguise" || scanner.Token == "nodis" || scanner.Token == "변신해제" || scanner.Token == "변해" )
			{
				g_WndMng.PutString( "Not Command!" );
				return TRUE;
			}*/				
		#endif // __CLIENT

			if( bItem == FALSE && pTextCmdFunc->m_dwAuthorization > pMover->m_dwAuthorization )
				break;

		#ifdef __CLIENT
			if( pTextCmdFunc->m_nServer == TCM_CLIENT || pTextCmdFunc->m_nServer == TCM_BOTH )
			{
				if( ( *pTextCmdFunc->m_pFunc )( scanner ) )
					if( pTextCmdFunc->m_nServer == TCM_BOTH )
					{
						char szSendChat[MAX_PATH];
						sprintf( szSendChat, "%s", scanner.m_pBuf );
						g_DPlay.SendChat( (LPCSTR)szSendChat );
					}
			}
			else
				g_DPlay.SendChat( (LPCSTR)lpszString );
		#else	// __CLIENT
			if( pTextCmdFunc->m_nServer == TCM_SERVER ||  pTextCmdFunc->m_nServer == TCM_BOTH )
				( *pTextCmdFunc->m_pFunc )( scanner );
		#endif	// __CLIENT
			return TRUE;
		}
		nCount++;
	}

#ifdef __CLIENT
	BOOL bSkip = FALSE;
	CString strTemp = lpszString;
	
	if( strTemp.Find( "#", 0 ) >= 0 )
		bSkip = TRUE;

	int nstrlen = strlen(lpszString);

	if( !bSkip )
	{
		TCHAR	szText[MAX_EMOTICON_STR];

		if(nstrlen < MAX_EMOTICON_STR)
			strcpy( szText, lpszString );
		else
		{
			strncpy(szText, lpszString, MAX_EMOTICON_STR);
			szText[MAX_EMOTICON_STR] = NULL;
		}

		// 이모티콘 명령
		for( int j=0; j < MAX_EMOTICON_NUM; j++ )
		{
			if( stricmp( &(szText[1]), g_DialogMsg.m_EmiticonCmd[ j ].m_szCommand ) == 0 )			
	//		if( _tcsicmp( szText, g_DialogMsg.m_EmiticonCmd[ j ].m_szCommand ) == 0 )
			{
				g_DPlay.SendChat( (LPCSTR)lpszString );
				return TRUE;
			}
		}
	}
#endif	//__CLIENT

	return FALSE;
}

void RemoveCRLF( char* szString )
{
	CString str		= szString;
	str.Replace( "\\n", " " );
	lstrcpy( szString, (LPCSTR)str );
}

void ParsingEffect( TCHAR* pChar, int nLen )
{
	CString strTemp;
	
	for( int i = 0; i < nLen; i++ )
	{
		if( pChar[ i ] == '#' ) // 인식 코드
		{
			if( ++i >= nLen )
				break;
			switch( pChar[ i ] )
			{
			case 'c':
				{
					if( ++i >= nLen )
						break;
					
					i += 7;
				}
				break;
			case 'u':
				break;
			case 'b':
				break;
			case 's':
				break;
				
			case 'l':
				{
					if(++i >= nLen)
						break;
					
					i += 3;
				}				
				break;
			case 'n':
				if( ++i >= nLen )
					break;
				
				{					
					switch( pChar[ i ] )
					{
					case 'c':
						break;
					case 'b':
						break;
					case 'u':
						break;
					case 's':
						break;
					case 'l':
						break;
					}
				}
				break;
			default: // 명령코드를 발견 못했을 경우 
				{
					// #코드를 넣어준다
					strTemp += pChar[ i - 1 ];
					strTemp += pChar[ i ];
				}
				break;
			}
		}
		else
		{
			if( pChar[ i ] == '\\' && pChar[ i+1 ] == 'n' )
			{
				strTemp += '\n';
				i+=1;
			}
			else
			{
				strTemp += pChar[ i ];
				int nlength = strTemp.GetLength();
			}
		}
	}
	
//	memcpy( pChar, strTemp, sizeof(TCHAR)*nLen );
	strcpy( pChar, strTemp );
}
