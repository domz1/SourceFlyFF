#include "StdAfx.h"
#include "defineObj.h"
#include "dbmanager.h"
#include "dploginsrvr.h"
#include "dpcoresrvr.h"
#include "dptrans.h"
#include "..\_Network\Objects\Obj.h"
#include "misc.h"
#include "mytrace.h"
#ifdef IDC_STATIC
#undef IDC_STATIC
#endif
#include "Resource.h"

extern  CDPCoreSrvr			g_dpCoreSrvr;
extern	CDPLoginSrvr		g_dpLoginSrvr;
extern	CProject			prj;
extern	APP_INFO			g_appInfo;
extern	CMyTrace			g_MyTrace;

void FILEOUT2(LPCTSTR lpszFileName,LPCTSTR lpszFormat, ...)
{
	va_list args;
	va_start(args, lpszFormat);
	int nBuf;
	static TCHAR szBuffer[40960];
	nBuf = _vsntprintf(szBuffer, 40960, lpszFormat, args);
	FILE* fp;
	if(!(fp = fopen(lpszFileName,"a")))
		if(!(fp = fopen(lpszFileName,"wt")))
			return;
		fprintf(fp,szBuffer);
		fprintf(fp,"\r\n");
		va_end(args);
		fclose(fp);
}

HWND hItemConvDlg, hItemConvWndPB;
#if defined(__ITEM_REMOVE_LIST)
#define	MAX_QUERY_SIZE	1024 * 64
#include "resource.h"
BOOL	CALLBACK	VerifyPlayerDlgProc( HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam );
#endif	// __VERIFY_PLAYER OR __PROVIDE


#ifdef __ITEM_REMOVE_LIST
void CDbManager::InitConvItemDialog( )
{
	HWND hItemConvDlg	= CreateDialog( hInst, MAKEINTRESOURCE( IDD_VERIFYPLAYER ),
		hMainWnd, VerifyPlayerDlgProc );
	ShowWindow( hItemConvDlg, SW_SHOW );
	HWND hItemConvWndPB;
	hItemConvWndPB	= GetDlgItem( hItemConvDlg, IDC_PROGRESS1 );
	SetDlgItemText( hItemConvDlg, IDC_STATIC1, "0 / 0" );
	SetDlgItemText( hItemConvDlg, IDC_STATIC, "Init" );
	SendMessage( hItemConvWndPB, PBM_SETRANGE, 0, MAKELPARAM( 0, 100 ) ); 
}

void CDbManager::UpdateConvItemDialog( char* pTitle, char* pString, char* pPosStirng, int nPos )
{
	SetDlgItemText( hItemConvDlg, IDC_STATIC2, pTitle );
	SetDlgItemText( hItemConvDlg, IDC_STATIC1, pString );
	SetDlgItemText( hItemConvDlg, IDC_STATIC, pPosStirng );
	SendMessage( hItemConvWndPB, PBM_SETPOS, (WPARAM)nPos, 0 );	
}



BOOL CDbManager::ConvItemConnectDB( CQuery* pQueryChar, CQuery* pQuerySave )
{
	if( FALSE == pQueryChar->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
	{
		SAFE_DELETE( pQueryChar );
		AfxMessageBox( "Connect Not Item", MB_OK );
		return FALSE;
	}
	
	if( FALSE == pQuerySave->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
	{
		SAFE_DELETE( pQuerySave );
		AfxMessageBox( "Connect Not Item", MB_OK );
		return FALSE;
	}	
	
	return TRUE;
}

DWORD CDbManager::GetMaxInventory( char* pszSQL, CQuery* pQueryChar, CQuery* pQuerySave )
{
	DWORD dwMaxConv = 0;
	sprintf( pszSQL, "select count = count(m_idPlayer) from INVENTORY_TBL where serverindex = '%02d'", g_appInfo.dwSys );
	TRACE( pszSQL ); TRACE( "\n" );
	if( FALSE == pQueryChar->Exec( pszSQL ) )
	{
		SAFE_DELETE( pQueryChar );
		SAFE_DELETE( pQuerySave );
		AfxMessageBox( pszSQL, MB_OK );
		return dwMaxConv;
	}
	if( pQueryChar->Fetch() )
		dwMaxConv = (DWORD)pQueryChar->GetInt( "count" );
	else
		AfxMessageBox( "Inven No Data Convert", MB_OK );
	
	return dwMaxConv;
}

DWORD CDbManager::GetMaxGuildBank( char* pszSQL, CQuery* pQueryChar, CQuery* pQuerySave )
{
	DWORD dwMaxConv = 0;
	sprintf( pszSQL, "select count = count(m_idGuild) from GUILD_BANK_TBL where serverindex = '%02d'", g_appInfo.dwSys );
	TRACE( pszSQL ); 	TRACE( "\n" );
	if( FALSE == pQueryChar->Exec( pszSQL ) )
	{
		SAFE_DELETE( pQueryChar );
		SAFE_DELETE( pQuerySave );
		AfxMessageBox( pszSQL, MB_OK );
		return dwMaxConv;
	}
	if( pQueryChar->Fetch() )
		dwMaxConv = (DWORD)pQueryChar->GetInt( "count" );
	else
		AfxMessageBox( "GuildBank No Data Convert", MB_OK );
	
	return dwMaxConv;
}
BOOL CDbManager::CreateInvenBankView( char* pszSQL, CQuery* pQueryChar, CQuery* pQuerySave )
{
	// Inven, Bank view 생성
	sprintf( pszSQL, " if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[seghope]') and OBJECTPROPERTY(id, N'IsView') = 1)"
		" drop view [dbo].[seghope]");
	
	if( FALSE == pQueryChar->Exec( pszSQL ) )
	{
		SAFE_DELETE( pQueryChar );
		SAFE_DELETE( pQuerySave );
		AfxMessageBox( pszSQL, MB_OK );
		return FALSE;
	}
	
	sprintf( pszSQL, "CREATE view seghope "
		" as "
		" select a.m_idPlayer, a.m_Inventory, a.m_apIndex, a.m_dwObjIndex,"
		" b.m_extInventory, b.m_InventoryPiercing,"
		" c.m_Bank, c.m_apIndex_Bank, c.m_dwObjIndex_Bank, "
		" d.m_extBank, d.m_BankPiercing,"
		" e.playerslot"
		" from "
		" INVENTORY_TBL a(nolock), INVENTORY_EXT_TBL b(nolock), BANK_TBL c(nolock), BANK_EXT_TBL d(nolock), CHARACTER_TBL e(nolock)"
		" where a.serverindex = b.serverindex "
		" and b.serverindex = c.serverindex "
		" and c.serverindex = d.serverindex "
		" and d.serverindex = e.serverindex"
		" and e.serverindex = '%02d' "
		" and a.m_idPlayer = b.m_idPlayer "
		" and b.m_idPlayer = c.m_idPlayer "
		" and c.m_idPlayer = d.m_idPlayer "
		" and d.m_idPlayer = e.m_idPlayer"
		,g_appInfo.dwSys );
	
	if( FALSE == pQueryChar->Exec( pszSQL ) )
	{
		SAFE_DELETE( pQueryChar );
		SAFE_DELETE( pQuerySave );
		AfxMessageBox( pszSQL, MB_OK );
		return FALSE;
	}
	return TRUE;
}

BOOL CDbManager::CreateGuildBankView( char* pszSQL, CQuery* pQueryChar, CQuery* pQuerySave )
{
	// Inven, Bank view 생성
	sprintf( pszSQL, " if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[seghope_guild]') and OBJECTPROPERTY(id, N'IsView') = 1)"
		" drop view [dbo].[seghope_guild]");
	
	if( FALSE == pQueryChar->Exec( pszSQL ) )
	{
		SAFE_DELETE( pQueryChar );
		SAFE_DELETE( pQuerySave );
		AfxMessageBox( pszSQL, MB_OK );
		return FALSE;
	}
	
	sprintf( pszSQL, "CREATE view seghope_guild "
		" as "
		" select a.m_idGuild, a.m_GuildBank,"
		" b.m_extGuildBank , b.m_GuildBankPiercing"
		" from "
		" GUILD_BANK_TBL a(nolock), GUILD_BANK_EXT_TBL b(nolock)"
		" where a.serverindex = b.serverindex"
		" and b.serverindex = '%02d'"
		" and a.m_idGuild = b.m_idGuild"
		, g_appInfo.dwSys );
	
	if( FALSE == pQueryChar->Exec( pszSQL ) )
	{
		SAFE_DELETE( pQueryChar );
		SAFE_DELETE( pQuerySave );
		AfxMessageBox( pszSQL, MB_OK );
		return FALSE;
	}
	return TRUE;
}

BOOL CDbManager::DeleteInvenBankView( char* pszSQL, CQuery* pQueryChar, CQuery* pQuerySave )
{
	// Inven, Bank view 삭제
	sprintf( pszSQL, "drop view seghope" );
	if( FALSE == pQueryChar->Exec( pszSQL ) )
	{
		SAFE_DELETE( pQueryChar );
		SAFE_DELETE( pQuerySave );
		AfxMessageBox( pszSQL, MB_OK );
		return FALSE;
	}
	return TRUE;
}

BOOL CDbManager::DeleteGuildBankView( char* pszSQL, CQuery* pQueryChar, CQuery* pQuerySave )
{
	sprintf( pszSQL, "drop view seghope_guild" );
	if( FALSE == pQueryChar->Exec( pszSQL ) )
	{
		SAFE_DELETE( pQueryChar );
		SAFE_DELETE( pQuerySave );
		AfxMessageBox( pszSQL, MB_OK );
		return FALSE;
	}
	return TRUE;
}

BOOL CDbManager::GetInventoryBankView( char* pszSQL, CQuery* pQueryChar, CQuery* pQuerySave )
{
	sprintf( pszSQL, "select * from seghope" );
	if( FALSE == pQueryChar->Exec( pszSQL ) )
	{
		SAFE_DELETE( pQueryChar ); SAFE_DELETE( pQuerySave ); AfxMessageBox( pszSQL, MB_OK );
		return FALSE;
	}
	return TRUE;
}

BOOL CDbManager::GetGuildBankView( char* pszSQL, CQuery* pQueryChar, CQuery* pQuerySave )
{
	sprintf( pszSQL, "select * from seghope_guild" );
	if( FALSE == pQueryChar->Exec( pszSQL ) )
	{
		SAFE_DELETE( pQueryChar ); SAFE_DELETE( pQuerySave ); AfxMessageBox( pszSQL, MB_OK );
		return FALSE;
	}
	return TRUE;
}

BOOL CDbManager::GetUserInventoryBank( CMover* pMover, CQuery* pQueryChar )
{
	pMover->m_idPlayer	= pQueryChar->GetInt( "m_idPlayer" );
	pMover->m_nSlot		= pQueryChar->GetInt( "playerslot" );
	
	// GetItem
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus	= NULL;
	//GetInventory( pMover, pQueryChar, lpDbOverlappedPlus );
	//GetBankMover( pMover, pQueryChar, 0 );
	// mirchang_100416 VERIFYSTRING use return value
	if( GetInventory( pMover, pQueryChar, lpDbOverlappedPlus ) == FALSE || GetBankMover( pMover, pQueryChar, 0 ) == FALSE )
	{
		return FALSE;
	}
	return TRUE;
	// mirchang_100416
}

BOOL CDbManager::SaveUserInventoryBank( char* pszSQL, CMover* pMover, CQuery* pQueryChar, CQuery* pQuerySave )
{
	ItemContainerStruct icsInventory, icsBank;
	SaveInventory( pMover, &icsInventory );
	SaveBank( pMover, &pMover->m_Bank[0], &icsBank );

	sprintf( pszSQL, "UPDATE INVENTORY_TBL"
		" SET m_Inventory = '%s', m_apIndex = '%s', m_dwObjIndex = '%s'"
		" where m_idPlayer = '%07d' and serverindex = '%02d'",
		icsInventory.szItem, icsInventory.szIndex, icsInventory.szObjIndex
		, pMover->m_idPlayer, g_appInfo.dwSys );

	if( prj.IsConvMode( SAVE_TEXT ) )
	{
		FILEOUT2( "..\\ConvertQuery.txt", pszSQL );
	}
	else
	{
		if( FALSE == pQuerySave->Exec( pszSQL ) )
		{
			SAFE_DELETE( pQueryChar );
			SAFE_DELETE( pQuerySave );
			AfxMessageBox( "UPDATE", MB_OK );
			return FALSE;
		}
	}
	
	sprintf( pszSQL, "UPDATE INVENTORY_EXT_TBL"
		" SET m_extInventory = '%s', m_InventoryPiercing = '%s'"
		" where m_idPlayer = '%07d' and serverindex = '%02d'",
		icsInventory.szExt, icsInventory.szPiercing, pMover->m_idPlayer, g_appInfo.dwSys );
	
	if( prj.IsConvMode( SAVE_TEXT ) )
	{
		FILEOUT2( "..\\ConvertQuery.txt", pszSQL );
	}
	else
	{
		if( FALSE == pQuerySave->Exec( pszSQL ) )
		{
			SAFE_DELETE( pQueryChar );
			SAFE_DELETE( pQuerySave );
			AfxMessageBox( "UPDATE INVENTORY_EXT_TBL", MB_OK );
			return FALSE;
		}
	}
	
	sprintf( pszSQL, "UPDATE BANK_TBL"
		" SET m_Bank = '%s', m_apIndex_Bank = '%s', m_dwObjIndex_Bank = '%s'"
		" where m_idPlayer = '%07d' and serverindex = '%02d'",
		icsBank.szItem, icsBank.szIndex, icsBank.szObjIndex, pMover->m_idPlayer, g_appInfo.dwSys );

		if( prj.IsConvMode( SAVE_TEXT ) )
	{
		FILEOUT2( "..\\ConvertQuery.txt", pszSQL );
	}
	else
	{
		if( FALSE == pQuerySave->Exec( pszSQL ) )
		{
			SAFE_DELETE( pQueryChar );
			SAFE_DELETE( pQuerySave );
			AfxMessageBox( "UPDATE BANK_TBL", MB_OK );
			return FALSE;
		}		
	}
	
	sprintf( pszSQL, "UPDATE BANK_EXT_TBL"
		" SET m_extBank = '%s', m_BankPiercing = '%s'"
		" where m_idPlayer = '%07d' and serverindex = '%02d'",
		icsBank.szExt, icsBank.szPiercing, pMover->m_idPlayer, g_appInfo.dwSys );
		
	if( prj.IsConvMode( SAVE_TEXT ) )
	{
		FILEOUT2( "..\\ConvertQuery.txt", pszSQL );
	}
	else
	{
		if( FALSE == pQuerySave->Exec( pszSQL ) )
		{
			SAFE_DELETE( pQueryChar );
			SAFE_DELETE( pQuerySave );
			AfxMessageBox( "UPDATE BANK_EXT_TBL", MB_OK );
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CDbManager::SaveConvGuildBank( char* pszSQL, int nGuildId, CItemContainer< CItemElem >* GuildBank, CQuery* pQueryChar, CQuery* pQuerySave )
{
	// Guild Save
	ItemContainerStruct	icsGuildBank;
	SaveGuildBank( GuildBank, &icsGuildBank );
	
	sprintf( pszSQL, "UPDATE GUILD_BANK_TBL"
		" SET m_GuildBank = '%s'"
		" where m_idGuild = '%06d' and serverindex = '%02d'",
		icsGuildBank.szItem, g_appInfo.dwSys );

	if( prj.IsConvMode( SAVE_TEXT ) )
	{
		FILEOUT2( "..\\ConvertQuery.txt", pszSQL );
	}
	else
	{	
		if( FALSE == pQuerySave->Exec( pszSQL ) )
		{
			SAFE_DELETE( pQueryChar );
			SAFE_DELETE( pQuerySave );
			AfxMessageBox( pszSQL, MB_OK );
			return FALSE;
		}
	}
	
	sprintf( pszSQL, "UPDATE GUILD_BANK_EXT_TBL"
		" SET m_extGuildBank = '%s', m_GuildBankPiercing = '%s'"
		" where m_idGuild = '%06d' and serverindex = '%02d'",
		icsGuildBank.szExt, icsGuildBank.szPiercing, nGuildId, g_appInfo.dwSys );

	if( prj.IsConvMode( SAVE_TEXT ) )
	{
		FILEOUT2( "..\\ConvertQuery.txt", pszSQL );
	}
	else
	{
		if( FALSE == pQuerySave->Exec( pszSQL ) )
		{
			SAFE_DELETE( pQueryChar );
			SAFE_DELETE( pQuerySave );
			AfxMessageBox( pszSQL, MB_OK );
			return FALSE;
		}
	}
	return TRUE;	
}

BOOL CDbManager::RemoveGuildBankList( char* pszSQL, int nGuildId, CItemContainer< CItemElem  >*  GuildBank, int* nCountItem0, int* nCountItem1 )
{
	BOOL bUpdate = FALSE;
	if( prj.IsConvMode( REMOVE_ITEM_ALL ) )
		bUpdate = RemoveItemGuildBank( nGuildId, GuildBank, nCountItem0, nCountItem1 );
	
	return bUpdate;
}

BOOL CDbManager::RemoveGuildBankListPanya( char* pszSQL, int nGuildId, CQuery* pQueryChar, CQuery* pQuerySave, int nCountItem0, int nCountItem1 )
{
	if( prj.IsConvMode( REMOVE_ITEM_ALL_PAY ) )
	{
		DWORD dwAddGold = GetRemoveItemPanya( nCountItem0, nCountItem1 );
		sprintf( pszSQL, "UPDATE GUILD_TBL"
			" SET m_nGuildGold = m_nGuildGold + %d"
			" where m_idGuild = '%06d' and serverindex = '%02d'",
			dwAddGold, nGuildId, g_appInfo.dwSys );
		FILEOUT( "..\\RemoveItem.log", "GuildId = %d, Item0 = %d, Item1 = %d, AddGold = %d", nGuildId,  nCountItem0, nCountItem1, dwAddGold );
		
		if( FALSE == pQuerySave->Exec( pszSQL ) )
		{
			SAFE_DELETE( pQueryChar );
			SAFE_DELETE( pQuerySave );
			AfxMessageBox( pszSQL, MB_OK );
			return FALSE;
		}
	}
	return TRUE;
}

DWORD CDbManager::GetRemoveItemPanya( int nItem0, int nItem1 )
{
	DWORD dwAddGold = 0;
	if( nItem0 )	//"천운의 스태프"
	{
		dwAddGold += 36000 * nItem0;
	}
	if( nItem1 )	//"파황의 스태프"
	{
		dwAddGold += 147500 * nItem1;
	}
	return dwAddGold;
}

BOOL CDbManager::RemoveItemInvenBank( CMover* pMover, int* nCountItem0, int* nCountItem1 )
{
	BOOL bResult = FALSE;
	SET_STRING::iterator it;
	for( it = m_RemoveItem_List.begin() ; it != m_RemoveItem_List.end() ; ++it )
	{
		// Inven
		ItemProp* pItemProp = NULL;
		pItemProp = prj.GetItemProp( it->data() );
		if( pItemProp )
		{
			int	nSize	= pMover->m_Inventory.m_dwItemMax;
			CItemElem* pItemElem;
			for( int i = 0 ; i < nSize; i++ )
			{
				pItemElem	= pMover->m_Inventory.GetAtId( i );
				if( pItemElem )
				{
					ItemProp* pGetItemProp		= prj.GetItemProp( pItemElem->m_dwItemId );	
					if( pGetItemProp && pGetItemProp->dwID == pItemProp->dwID )
					{
						if( pItemElem->m_bItemResist || pItemElem->GetOption() || pItemElem->m_nResistAbilityOption )
						{
							FILEOUT( "..\\RemoveItem.log", "IdPlayer = %07d, ItemName : %s %d/%d/%d", pMover->m_idPlayer,  it->data(), pItemElem->GetOption(), pItemElem->m_bItemResist, pItemElem->m_nResistAbilityOption );
						}
						pMover->m_Inventory.RemoveAtId( pItemElem->m_dwObjId );
						if( prj.IsConvMode( REMOVE_ITEM_ALL_PAY ) )
						{
							if( lstrcmp( it->data(), "천운의 스태프" ) == 0 )
								(*nCountItem0)++;
							if( lstrcmp( it->data(), "파황의 스태프" ) == 0 )
								(*nCountItem1)++;
						}
//						FILEOUT( "..\\RemoveItem.log", "PlayerName = %s, ItemName : %s", pMover->m_szName,  it->data() );
						bResult = TRUE;
					}
				}
			}

			// Bank
			nSize	= pMover->m_Bank[0].m_dwItemMax;
			for( i = 0 ; i < nSize; i++ )
			{
				pItemElem	= pMover->m_Bank[0].GetAtId( i );
				if( pItemElem )
				{
					ItemProp* pGetItemProp		= prj.GetItemProp( pItemElem->m_dwItemId );	
					if( pGetItemProp && pGetItemProp->dwID == pItemProp->dwID )
					{
						if( pItemElem->m_bItemResist || pItemElem->GetOption() || pItemElem->m_nResistAbilityOption )
						{
							FILEOUT( "..\\RemoveItem.log", "IdPlayer = %07d, ItemName : %s %d/%d/%d", pMover->m_idPlayer,  it->data(), pItemElem->GetOption(), pItemElem->m_bItemResist, pItemElem->m_nResistAbilityOption );
						}

						pMover->m_Bank[0].RemoveAtId( pItemElem->m_dwObjId );
						if( prj.IsConvMode( REMOVE_ITEM_ALL_PAY ) )
						{
							if( lstrcmp( it->data(), "천운의 스태프" ) == 0 )
								(*nCountItem0)++;
							if( lstrcmp( it->data(), "파황의 스태프" ) == 0 )
								(*nCountItem1)++;
						}
//						FILEOUT( "..\\RemoveItem.log", "PlayerName = %s, ItemName : %s", pMover->m_szName,  it->data() );
						bResult = TRUE;
					}
				}
			}
		}
	}
	return bResult;
}

BOOL CDbManager::RemoveItemGuildBank( int nGuildId, CItemContainer< CItemElem  >*  GuildBank, int* nCountItem0, int* nCountItem1 )
{
	BOOL bResult = FALSE;
	SET_STRING::iterator it;
	for( it = m_RemoveItem_List.begin() ; it != m_RemoveItem_List.end() ; ++it )
	{
		ItemProp* pItemProp = NULL;
		pItemProp = prj.GetItemProp( it->data() );
		if( pItemProp )
		{
			int	nSize	= GuildBank->m_dwItemMax;
			CItemElem* pItemElem;
			for( int i = 0 ; i < nSize; i++ )
			{
				pItemElem	= GuildBank->GetAtId( i );
				if( pItemElem )
				{
					ItemProp* pGetItemProp		= prj.GetItemProp( pItemElem->m_dwItemId );	
					if( pGetItemProp && pGetItemProp->dwID == pItemProp->dwID )
					{
						if( pItemElem->m_bItemResist || pItemElem->GetOption() || pItemElem->m_nResistAbilityOption )
						{
							FILEOUT( "..\\RemoveItem.log", "GuildId = %d, ItemName : %s %d/%d/%d", nGuildId,  it->data(), pItemElem->GetOption(), pItemElem->m_bItemResist, pItemElem->m_nResistAbilityOption );
						}
						
						GuildBank->RemoveAtId( pItemElem->m_dwObjId );
						if( prj.IsConvMode( REMOVE_ITEM_ALL_PAY ) )
						{
							if( lstrcmp( it->data(), "천운의 스태프" ) == 0 )
								(*nCountItem0)++;
							if( lstrcmp( it->data(), "파황의 스태프" ) == 0 )
								(*nCountItem1)++;
						}
//						FILEOUT( "..\\RemoveItem.log", "GuildId = %s, ItemName : %s", nGuildId,  it->data() );
						bResult = TRUE;
					}
				}
			}		
		}
	}
	return bResult;
}

BOOL CDbManager::RemoveInventoryBankList( char* pszSQL, CMover* pMover, int* nCountItem0, int* nCountItem1 )
{
	// RemoveItem
	BOOL bUpdate = FALSE;
	if( prj.IsConvMode( REMOVE_ITEM_ALL ) )
		bUpdate = RemoveItemInvenBank( pMover, nCountItem0, nCountItem1 );

	return bUpdate;
}

BOOL CDbManager::RemoveInventoryBankListPanya( char* pszSQL, CMover* pMover, CQuery* pQueryChar, CQuery* pQuerySave, int nCountItem0, int nCountItem1 )
{
	if( prj.IsConvMode( REMOVE_ITEM_ALL_PAY ) )
	{
		DWORD dwAddGold = GetRemoveItemPanya( nCountItem0, nCountItem1 );
		sprintf( pszSQL, "UPDATE CHARACTER_TBL"
			" SET m_dwGold = m_dwGold + %d"
			" where m_idPlayer = '%07d' and serverindex = '%02d'",
			dwAddGold, pMover->m_idPlayer, g_appInfo.dwSys );
		FILEOUT( "..\\RemoveItem.log", "IdPlayer = %07d, Item0 = %d, Item1 = %d, AddGold = %d", pMover->m_idPlayer,  nCountItem0, nCountItem1, dwAddGold );
		
		if( FALSE == pQuerySave->Exec( pszSQL ) )
		{
			SAFE_DELETE( pQueryChar );
			SAFE_DELETE( pQuerySave );
			AfxMessageBox( pszSQL, MB_OK );
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CDbManager::PiercingConfirmInventoryBank( CMover* pMover )
{
	BOOL bUpdate = FALSE;
	if( prj.IsConvMode( PIERCING_CONFIRM ) )	
	{
		// Inventory
		int	nSize	= pMover->m_Inventory.m_dwItemMax;
		CItemElem* pItemElem;
		for( int i = 0 ; i < nSize; i++ )
		{
			pItemElem	= pMover->m_Inventory.GetAtId( i );
			if( pItemElem )
			{
				ItemProp* pGetItemProp		= prj.GetItemProp( pItemElem->m_dwItemId );	
				if( pGetItemProp && pGetItemProp->dwItemKind3 != IK3_SUIT && pItemElem->GetPiercingSize() != 0 )
				{
					FILEOUT( "..\\RemoveItemPiercingConfirm.log", "IdPlayer = %07d, ItemName : %s %d/%d/%d %d/%d/%d/%d/%d", 
						pMover->m_idPlayer,  pGetItemProp->szName, pItemElem->GetOption(), pItemElem->m_bItemResist, pItemElem->m_nResistAbilityOption, pItemElem->GetPiercingSize(), pItemElem->GetPiercingItem( 0 ), pItemElem->GetPiercingItem( 1 ), pItemElem->GetPiercingItem( 2 ), pItemElem->GetPiercingItem( 3 ) );
					pMover->m_Inventory.RemoveAtId( pItemElem->m_dwObjId );
					bUpdate = TRUE;
				}
			}
		}
		// Bank
		nSize	= pMover->m_Bank[0].m_dwItemMax;
		for( i = 0 ; i < nSize; i++ )
		{
			pItemElem	= pMover->m_Bank[0].GetAtId( i );
			if( pItemElem )
			{
				ItemProp* pGetItemProp		= prj.GetItemProp( pItemElem->m_dwItemId );	
				if( pGetItemProp && pGetItemProp->dwItemKind3 != IK3_SUIT && pItemElem->GetPiercingSize() != 0 )
				{
					FILEOUT( "..\\RemoveItemPiercingConfirm.log", "IdPlayer = %07d, ItemName : %s %d/%d/%d %d/%d/%d/%d/%d", 
						pMover->m_idPlayer,  pGetItemProp->szName, pItemElem->GetOption(), pItemElem->m_bItemResist, pItemElem->m_nResistAbilityOption, pItemElem->GetPiercingSize(), pItemElem->GetPiercingItem( 0 ), pItemElem->GetPiercingItem( 1 ), pItemElem->GetPiercingItem( 2 ), pItemElem->GetPiercingItem( 3 ) );
					pMover->m_Bank[0].RemoveAtId( pItemElem->m_dwObjId );
					bUpdate = TRUE;
				}
			}
		}
	}
	return bUpdate;
}

BOOL CDbManager::PiercingConfirmGuildBank( int nGuildId, CItemContainer< CItemElem >* GuildBank )
{
	BOOL bUpdate = FALSE;
	if( prj.IsConvMode( PIERCING_CONFIRM ) )	
	{
		int	nSize	= GuildBank->m_dwItemMax;
		CItemElem* pItemElem;
		for( int i = 0 ; i < nSize; i++ )
		{
			pItemElem	= GuildBank->GetAtId( i );
			if( pItemElem )
			{
				ItemProp* pGetItemProp		= prj.GetItemProp( pItemElem->m_dwItemId );	
				if( pGetItemProp && pGetItemProp->dwItemKind3 != IK3_SUIT && pItemElem->GetPiercingSize() != 0 )
				{
					FILEOUT( "..\\RemoveItemPiercingConfirm.log", "nGuildId = %06d, ItemName : %s %d/%d/%d %d/%d/%d/%d/%d", 
						nGuildId,  pGetItemProp->szName, pItemElem->GetOption(), pItemElem->m_bItemResist, pItemElem->m_nResistAbilityOption, pItemElem->GetPiercingSize(), pItemElem->GetPiercingItem( 0 ), pItemElem->GetPiercingItem( 1 ), pItemElem->GetPiercingItem( 2 ), pItemElem->GetPiercingItem( 3 ) );
					GuildBank->RemoveAtId( pItemElem->m_dwObjId );
					bUpdate = TRUE;
				}
			}
		}
	}
	return bUpdate;
}

BOOL CDbManager::InventoryBankConv( char* pszSQL, CMover* pMover, CQuery* pQueryChar, CQuery* pQuerySave )
{
	int nCountItem0 = 0;
	int nCountItem1 = 0;
	BOOL bUpdateRemove = FALSE;
	BOOL bUpdatePiercingConfirm = FALSE;

	bUpdateRemove = RemoveInventoryBankList( pszSQL, pMover, &nCountItem0, &nCountItem1 );
	if( bUpdateRemove )
		RemoveInventoryBankListPanya( pszSQL, pMover, pQueryChar, pQuerySave, nCountItem0, nCountItem1 );
	bUpdatePiercingConfirm = PiercingConfirmInventoryBank( pMover );

	if( bUpdateRemove || bUpdatePiercingConfirm )
		return TRUE;
	else
		return FALSE;
}

BOOL CDbManager::GuildBankConv( char* pszSQL, int nGuildId, CItemContainer< CItemElem >* GuildBank, CQuery* pQueryChar, CQuery* pQuerySave )
{
	int nCountItem0 = 0;
	int nCountItem1 = 0;
	BOOL bUpdateRemove = FALSE;
	BOOL bUpdatePiercingConfirm = FALSE;
	
	bUpdateRemove = RemoveGuildBankList( pszSQL, nGuildId, GuildBank, &nCountItem0, &nCountItem1 );
	if( bUpdateRemove )
		RemoveGuildBankListPanya( pszSQL, nGuildId, pQueryChar, pQuerySave, nCountItem0, nCountItem1 );

	bUpdatePiercingConfirm = PiercingConfirmGuildBank( nGuildId, GuildBank );

	if( bUpdateRemove || bUpdatePiercingConfirm )
		return TRUE;
	else
		return FALSE;
	
}

BOOL CDbManager::ConvItemStart( void )
{
	int nCount	= 0;
	int nStarted	= 0;
	DWORD dwMaxConv	= 0;
	char* szSQL	= new char[40960];
	char lpString[300];
	char lpString1[300];

	CQuery* pQueryChar	= new CQuery;
	CQuery* pQuerySave	= new CQuery;
	if( ConvItemConnectDB( pQueryChar, pQuerySave ) == FALSE )
		return FALSE;

	InitConvItemDialog();
	
	AfxMessageBox( "ConvItemStart", MB_OK );
//////////////////////////////////////// Inventory Start ////////////////////////////////////////
	if( ( dwMaxConv = GetMaxInventory( szSQL, pQueryChar, pQuerySave ) ) == 0 )
		return FALSE;

	sprintf( lpString, "%d / %d", 0, dwMaxConv );
	UpdateConvItemDialog( "Inventory, Bank", lpString, "0 %%", 0 );
	if( CreateInvenBankView( szSQL, pQueryChar, pQuerySave ) == FALSE )
		return FALSE;
	// Inven, Bank Item 가지고 오기
	if( GetInventoryBankView( szSQL, pQueryChar, pQuerySave ) == FALSE )
		return FALSE;
	while( pQueryChar->Fetch() )
	{
		++nStarted;
		sprintf( lpString, "%d / %d", nStarted, dwMaxConv );
		sprintf( lpString1, "%d %", int( float( (float)nStarted / (float)dwMaxConv ) * 100.0f ) );
		UpdateConvItemDialog( "Inventory, Bank", lpString, lpString1, (int( float( (float)nStarted / (float)dwMaxConv ) * 100.0f )) );
		
		CMover* pMover	= new CMover;
		//GetUserInventoryBank( pMover, pQueryChar );
		// mirchang_100416 VERIFYSTRING use return value
		if( GetUserInventoryBank( pMover, pQueryChar ) == FALSE )
		{
			safe_delete( pMover );
			return FALSE;
		}
		// mirchang_100416

		if( InventoryBankConv( szSQL, pMover, pQueryChar, pQuerySave ) ) // SaveItem
		{
			if( SaveUserInventoryBank( szSQL, pMover, pQueryChar, pQuerySave ) == FALSE )
			{
				safe_delete( pMover );
				return FALSE;
			}
		}
		safe_delete( pMover );
	}
	if( DeleteInvenBankView( szSQL, pQueryChar, pQuerySave ) == FALSE )
		return FALSE;

//////////////////////////////////////// GuildBank Start ////////////////////////////////////////
	nStarted = 0;
	if( ( dwMaxConv = GetMaxGuildBank( szSQL, pQueryChar, pQuerySave ) ) == 0 )
		return FALSE;
	
	sprintf( lpString, "%d / %d", 0, dwMaxConv );
	UpdateConvItemDialog( "GuildBank", lpString, "0 %%", 0 );
	if( CreateGuildBankView( szSQL, pQueryChar, pQuerySave ) == FALSE )
		return FALSE;
	// Inven, Bank Item 가지고 오기
	if( GetGuildBankView( szSQL, pQueryChar, pQuerySave ) == FALSE )
		return FALSE;
	
	while( pQueryChar->Fetch() )
	{
		++nStarted;

		sprintf( lpString, "%d / %d", nStarted, dwMaxConv );
		sprintf( lpString1, "%d %", int( float( (float)nStarted / (float)dwMaxConv ) * 100.0f ) );
		UpdateConvItemDialog( "GuildBank", lpString, lpString1, (int( float( (float)nStarted / (float)dwMaxConv ) * 100.0f )) );
			
		CItemContainer<CItemElem>	GuildBank;	// 길드 창고
		int							nGuildId	= 0;
	
		GuildBank.SetItemContainer( ITYPE_ITEM, MAX_GUILDBANK );
		nGuildId		= pQueryChar->GetInt( "m_idGuild" );
		GetGuildBank( &GuildBank, pQueryChar );
		
		if( GuildBankConv( szSQL, nGuildId, &GuildBank, pQueryChar, pQuerySave ) )
		{
			if( SaveConvGuildBank( szSQL, nGuildId, &GuildBank, pQueryChar, pQuerySave ) == FALSE )
				return FALSE;
		}
	}
	if( DeleteGuildBankView( szSQL, pQueryChar, pQuerySave ) == FALSE )
		return FALSE;
	
	SAFE_DELETE( pQueryChar ); SAFE_DELETE( pQuerySave );
	return TRUE;
}

#endif // __ITEM_REMOVE_LIST

