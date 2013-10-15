#include "StdAfx.h"
#include "defineObj.h"
#include "dbmanager.h"
#include "dploginsrvr.h"
#include "dpcoresrvr.h"
#include "dptrans.h"
#include "..\_Network\Objects\Obj.h"
#include "misc.h"
#include "mytrace.h"
#include "lang.h"

extern  CDPCoreSrvr			g_dpCoreSrvr;
extern	CDPLoginSrvr		g_dpLoginSrvr;
extern	CProject			prj;
extern	APP_INFO			g_appInfo;
extern	CMyTrace			g_MyTrace;


void CDbManager::ItemUpdateThread( void )
{
	if( prj.m_bItemUpdate == FALSE )
		return;
	
	CQuery* pQuery	= new CQuery;
	if( FALSE == pQuery->Connect( 3, DSN_NAME_ITEMUPDATE, DB_ADMIN_ID_ITEMUPDATE, DB_ADMIN_PASS_ITEMUPDATE ) )
	{
		char s[128];
		sprintf( s, "%s, %d ItemUpdate Connect Failed", __FILE__, __LINE__ );
		AfxMessageBox( s );
		SAFE_DELETE( pQuery );
		return;
	}

	HANDLE hHandle	= m_hItemUpdateCloseWorker;
	int nItemYes = 0;

	while( WaitForSingleObject( hHandle, 5000 ) == WAIT_TIMEOUT )	// 10초	= 1000 * 10 //1000 * 60 * 5
	{
		if( nItemYes < m_nItemUpdate )
		{
			nItemYes = m_nItemUpdate;
			ChangeItemUpdate( pQuery );	
			ChangeSkillUpdate( pQuery );
			ChangeMonsterUpdate( pQuery );
			ChangeQuestUpdate( pQuery );	
//			ChangeJobNameUpdate( pQuery );
//			ChangeSexUpdate( pQuery );			
			TRACE( "Item Update Complete GOOD !!!\n");
		}
		TRACE( "Worker ITEM_UPDATE\n");
	}
	SAFE_DELETE( pQuery );
}

void CDbManager::ChangeItemUpdate( CQuery* pQuery )
{
	char szSQL[4096] = {0,};
	
	sprintf( szSQL, "ITEM_STR 'D1'" );
	
	if( FALSE == pQuery->Exec( szSQL ) )
	{
		WriteLog( szSQL );
		return;		
	}

	for( int i = 0; i < prj.m_aPropItem.GetSize(); i++ )
	{
		ItemProp* pItemProp =  prj.GetItemProp( i );
		if( pItemProp && pItemProp->dwItemKind3 != IK3_VIRTUAL && pItemProp->dwID != 0 )
		{
			CString	strItem	= pItemProp->szName;
			strItem.Replace( "'", " " );
			sprintf( szSQL, "ITEM_STR 'I1', %d,'%s','%s'," // 0
				"%d,%d,%d,%d,%d,"	// 1
				"%d,%d,%d,%d,%d,"	// 2
				"%d,%d,%d,%d,%d,"	// 3
				"%d,%d,%d,%d,%d,"	// 4
				"%d,%d,%d,%d,%d,"	// 5
				"%d,%d,%d,%d,%d,"	// 6
				"%d,%d,%d,%d,%d,"	// 7
				"%d,%d,%f,%d,%d,"	// 8
				"%d,%d,%d,%d,%d,"	// 9
				"%d,%d,%d,%d,%d,"	// 10
				"%d,%d,%d,%d,%d,"	// 11
				"%d,%d,%d,%d,%d,"	// 12
				"%d,%d,%d,%d,%d,"	// 13
				"%d,%d,%d,%d,%d,"	// 14
				"%d,%d,%d,%d,%d,"	// 15
				"%d,%d,%d,%d,%d,"	// 16
				"%d,%d,%d,%d,%d,"	// 17
				"%d,  %f,  %d,%d,%d,%d,%d,  %d,  '%s'",

				pItemProp->dwID, strItem, strItem, // 0
				pItemProp->dwNum, pItemProp->dwPackMax, pItemProp->dwItemKind1, pItemProp->dwItemKind2, pItemProp->dwItemKind3, // 1
				pItemProp->dwItemJob, pItemProp->bPermanence, pItemProp->dwUseable, pItemProp->dwItemSex, 0,	// 2
				pItemProp->dwCost, 0, pItemProp->dwEndurance, 0, 0,		// 3
				pItemProp->nAbrasion, pItemProp->nMaxRepair,
				pItemProp->dwParts, pItemProp->dwItemLV, 0,		// 4
				pItemProp->dwItemRare, pItemProp->nLog, 0, 0, pItemProp->dwAbilityMin, // 5
				pItemProp->dwAbilityMax, pItemProp->nAddSkillMin, pItemProp->nAddSkillMax, pItemProp->dwWeaponType, pItemProp->tmContinuousPain, // 6
				//pItemProp->dwShellQuantity, pItemProp->dwRecoil, pItemProp->dwCaliber, pItemProp->dwBarrageRate, pItemProp->dwLoadingTime, 
				pItemProp->dwRecoil, pItemProp->dwRecoil, 0, 0, pItemProp->dwLoadingTime, // 7
				pItemProp->nAdjHitRate, 0, pItemProp->fAttackSpeed, pItemProp->dwDmgShift, pItemProp->dwAttackRange, // 8
				0, pItemProp->nProbability, pItemProp->dwDestParam1, pItemProp->dwDestParam2, pItemProp->nAdjParamVal1, // 9
				pItemProp->nAdjParamVal2, pItemProp->dwChgParamVal1, pItemProp->dwChgParamVal2, 0, 0, // 10
				0, 0, 0, pItemProp->dwReqMp, pItemProp->dwReqFp, // 11
				0, pItemProp->dwReqDisLV, pItemProp->dwSkillReadyType, pItemProp->dwSkillReady, pItemProp->_dwSkillRange, // 12
				pItemProp->dwCircleTime, pItemProp->dwSkillTime, 0, pItemProp->dwExeTarget, pItemProp->dwUseChance, // 13
				pItemProp->dwSpellRegion, pItemProp->dwSpellType, pItemProp->dwSkillType, 0, pItemProp->nItemResistElecricity, // 14
				pItemProp->nItemResistDark, pItemProp->nItemResistFire, pItemProp->nItemResistWind, pItemProp->nItemResistWater, pItemProp->nItemResistEarth, // 15
				0, 0, pItemProp->nEvildoing, 0, 0, // 16
				pItemProp->dwExpertLV, pItemProp->dwExpertMax, pItemProp->dwSubDefine, pItemProp->dwExp, 0, // 17
				0, 
				pItemProp->fFlightSpeed, 
				pItemProp->dwFlightLimit, 0, pItemProp->dwLimitLevel1, 0, 0, 
				pItemProp->nReflect, 
				""
				);
			if( FALSE == pQuery->Exec( szSQL ) )
			{
				WriteLog( szSQL );
				return;
			}
		}	// if
	}	// for
	TRACE( "ChangeItemUpdate Complete\n" );
}

void CDbManager::ChangeSkillUpdate( CQuery* pQuery )
{
	char szSQL[1024] = {0,};
	
	sprintf( szSQL, "ITEM_STR 'D2'" );
	
	if( FALSE == pQuery->Exec( szSQL ) )
	{
		WriteLog( szSQL );
		return;		
	}
	
	for( int i = 0; i < prj.m_aPropSkill.GetSize(); i++ )
	{
		ItemProp* pSkillProp =  prj.GetSkillProp( i );
		if( pSkillProp && pSkillProp->dwID != 0 )
		{
			CString strSkill	= pSkillProp->szName;
			strSkill.Replace( "'", " " );
			sprintf( szSQL, "ITEM_STR 'I2', %d,'%s','',%d",
				pSkillProp->dwID, strSkill, pSkillProp->dwItemJob );
			if( FALSE == pQuery->Exec( szSQL ) )
			{
				WriteLog( szSQL );
				return;
			}
		}
	}

	TRACE( "ChangeSkillUpdate Complete\n" );
}

void CDbManager::ChangeMonsterUpdate( CQuery* pQuery )
{
	char szSQL[1024]	= {0,};
	
	sprintf( szSQL, "ITEM_STR 'D3'" );
	
	if( FALSE == pQuery->Exec( szSQL ) )
	{
		WriteLog( szSQL );
		return;		
	}

	for( int i = 0; i < prj.m_nMoverPropSize; i++ )
	{
		MoverProp* pMoverProp	= prj.m_pPropMover + i;
		if( pMoverProp->dwID != 0 && pMoverProp->dwAI != AII_NONE && pMoverProp->dwAI != AII_MOVER	)
		{
			//	mulcom	BEGIN100406	ITEM_STR I3 MoverName quotation mark 제거
			//sprintf( szSQL, "ITEM_STR 'I3', %d,'%s','',%d", pMoverProp->dwID, pMoverProp->szName, pMoverProp->dwLevel );
			CString	strMoverName	= pMoverProp->szName;
			strMoverName.Replace( "'", " " );

			sprintf( szSQL, "ITEM_STR 'I3', %d,'%s','',%d",
				pMoverProp->dwID, strMoverName, pMoverProp->dwLevel );
			//	mulcom	END100406	ITEM_STR I3 MoverName quotation mark 제거

			
			if( FALSE == pQuery->Exec( szSQL ) )
			{
				WriteLog( szSQL );
				return ;
			}
		}
	}
	
	TRACE( "ChangeMonsterUpdate Complete\n" );
}


void CDbManager::ChangeQuestUpdate( CQuery* pQuery )
{
	char szSQL[1024] = {0,};
	
	sprintf( szSQL, "ITEM_STR 'D4'" );
	
	if( FALSE == pQuery->Exec( szSQL ) )
	{
		WriteLog( szSQL );
		return ;		
	}

	for( INT2STRING::iterator i = m_int2StrItemUpdate.begin(); i != m_int2StrItemUpdate.end(); ++i )
	{
		CString	strQuest	= (*i).second.data();
		strQuest.Replace( "'", " " );
		sprintf( szSQL, "ITEM_STR 'I4', %d,'%s'",
			(*i).first, strQuest );
		
		if( FALSE == pQuery->Exec( szSQL ) )
		{
			WriteLog( szSQL );
			return ;
		}
	}

	TRACE( "ChangeQuestUpdate Complete\n" );
}

void CDbManager::ChangeJobNameUpdate( CQuery* pQuery )
{
	TRACE( "ChangeJobNameUpdate Complete\n" );
}

void CDbManager::ChangeSexUpdate( CQuery* pQuery )
{
//	SEX_MALE, SEX_FEMALE, SEX_SEXLESS
	TRACE( "ChangeSexUpdate Complete\n" );
}

