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

#ifdef __CONV_SKILL_11_MONTH_JOB1
void CDbManager::ConvSkill_11_MONTH_POINT( void )
{
	int nStarted	= 0;
	
	CQuery* pQueryChar	= new CQuery;
	CQuery* pQuery	= new CQuery;
	
	int nCount	= 0;
	
	if( FALSE == pQueryChar->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
	{
		SAFE_DELETE( pQueryChar );
		char szMessageBox[256];
		sprintf( szMessageBox, "1 Not Connect SQL_DB ID : %s, Pass : %s, DNS : %s", DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01, DSN_NAME_CHARACTER01 );
		AfxMessageBox( szMessageBox , MB_OK );
		return;
	}
	if( FALSE == pQuery->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
	{
		SAFE_DELETE( pQueryChar );
		SAFE_DELETE( pQuery );
		char szMessageBox[256];
		sprintf( szMessageBox, "2 Not Connect SQL_DB ID : %s, Pass : %s, DNS : %s", DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01, DSN_NAME_CHARACTER01 );
		AfxMessageBox( szMessageBox , MB_OK );
		return;
	}
	char* szSQL	= new char[40960];
	sprintf( szSQL, "SELECT m_idPlayer, serverindex, m_nJob, m_aJobSkill from CHARACTER_TBL WHERE serverindex = '%02d'", g_appInfo.dwSys );

	if( FALSE == pQueryChar->Exec( szSQL ) )
	{
		SAFE_DELETE( pQueryChar );
		SAFE_DELETE( pQuery );
		char szMessageBox[256];
		sprintf( szMessageBox, "Not Qry %s", szSQL );
		AfxMessageBox( szMessageBox , MB_OK );
		return;
	}

	while( pQueryChar->Fetch() )
	{
		if( (++nStarted % 100) == 0 )
		{
			TRACE( "%d converted\n", nStarted );
		}

		int nJob	= pQueryChar->GetInt( "m_nJob" );
		if( nJob == JOB_VAGRANT )
			continue;

		CMover* pMover	= new CMover;

		pMover->m_idPlayer	= pQueryChar->GetInt( "m_idPlayer" );
		u_long si	= pQueryChar->GetInt( "serverindex" );
		pMover->m_nJob	= nJob;

		// 1. 직업 스킬 로드
		int CountStr	= 0;
		int IndexJobSkill	= 0;
		char JobSkill[1024]	= {0,};
		pQueryChar->GetStr( "m_aJobSkill", JobSkill );

		while( '$' != JobSkill[CountStr] )
		{
			SKILL BufSkill;
			GetOneSkill( &BufSkill, JobSkill, &CountStr );
			pMover->m_aJobSkill[IndexJobSkill] = BufSkill;

			IndexJobSkill++;
		}

		int nExpPoint = pMover->InitSkillExp();
		
		sprintf( szSQL, 
			"UPDATE CHARACTER_TBL SET m_dwSkillPoint = %d where m_idPlayer = '%07d' AND serverindex = '%02d'",
			nExpPoint, pMover->m_idPlayer, si );
		if( FALSE == pQuery->Exec( szSQL ) )
		{
			char lpString[260]	= { 0, };
			sprintf( lpString, "Update Faild, m_idPlayer = %d", pMover->m_idPlayer );
			AfxMessageBox( lpString );
		}
		safe_delete( pMover );
	}
	SAFE_DELETE_ARRAY( szSQL );
	SAFE_DELETE( pQueryChar );	
}
#endif // __CONV_SKILL_11_MONTH_JOB1

#ifdef __CONV_SKILL_11_MONTH_JOB2
void CDbManager::ConvSkill_11_MONTH( void )
{
	int nStarted	= 0;

	CQuery* pQueryChar	= new CQuery;
	CQuery* pQuery	= new CQuery;

	int nCount	= 0;
	
	if( FALSE == pQueryChar->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
	{
		SAFE_DELETE( pQueryChar );
		char szMessageBox[256];
		sprintf( szMessageBox, "1 Not Connect SQL_DB ID : %s, Pass : %s, DNS : %s", DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01, DSN_NAME_CHARACTER01 );
		AfxMessageBox( szMessageBox , MB_OK );
		return;
	}
	if( FALSE == pQuery->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
	{
		SAFE_DELETE( pQueryChar );
		SAFE_DELETE( pQuery );
		char szMessageBox[256];
		sprintf( szMessageBox, "2 Not Connect SQL_DB ID : %s, Pass : %s, DNS : %s", DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01, DSN_NAME_CHARACTER01 );
		AfxMessageBox( szMessageBox , MB_OK );
		return;
	}

	char* szSQL	= new char[40960];
	sprintf( szSQL, "SELECT m_idPlayer, serverindex, m_nJob, m_aJobSkill from CHARACTER_TBL WHERE serverindex = '%02d'", g_appInfo.dwSys );

	if( FALSE == pQueryChar->Exec( szSQL ) )
	{
		SAFE_DELETE( pQueryChar );
		SAFE_DELETE( pQuery );
		char szMessageBox[256];
		sprintf( szMessageBox, "Not Qry %s", szSQL );
		AfxMessageBox( szMessageBox , MB_OK );
		return;
	}

	while( pQueryChar->Fetch() )
	{
		if( (++nStarted % 100) == 0 )
		{
			TRACE( "%d converted\n", nStarted );
		}

		int nJob	= pQueryChar->GetInt( "m_nJob" );
		if( nJob == JOB_VAGRANT )
			continue;

		CMover* pMover	= new CMover;

		// 0. 스킬 초기화
		for( int i = MAX_JOB_SKILL; i < MAX_SKILL_JOB; i++ )
		{
			pMover->m_aJobSkill[i].dwSkill	= NULL_ID;
			pMover->m_aJobSkill[i].dwExp	= 0;
			pMover->m_aJobSkill[i].dwLevel	= 1;
			pMover->m_aJobSkill[i].bStop = TRUE;
		}

		pMover->m_idPlayer	= pQueryChar->GetInt( "m_idPlayer" );
		u_long si	= pQueryChar->GetInt( "serverindex" );
		pMover->m_nJob	= nJob;

		// 1. 직업 스킬 로드
		int CountStr	= 0;
		int IndexJobSkill	= 0;
		char JobSkill[512]	= { 0, };
		pQueryChar->GetStr( "m_aJobSkill", JobSkill );
		
		int len		= strlen( JobSkill );
		if( len < 1 || JobSkill[len-1] != '$' )
		{
			TRACE( "%d\n", pMover->m_idPlayer );
			safe_delete( pMover );
			continue;
		}

		DWORD dwExp, dwLevel, dwSkill;
		BOOL bStop;
		SKILL	m_aJobSkill[MAX_SKILL_JOB];
		memset( m_aJobSkill, 0, sizeof(SKILL) * ( MAX_SKILL_JOB ) );
		while( '$' != JobSkill[CountStr] )
		{
			dwExp	= (DWORD)GetIntFromStr( JobSkill, &CountStr );
			dwLevel	= (DWORD)GetIntFromStr( JobSkill, &CountStr );
			dwSkill	= (DWORD)GetIntFromStr( JobSkill, &CountStr );
			bStop	= (BOOL)GetIntFromStr( JobSkill, &CountStr );
			ItemProp* pItemProp	= prj.GetSkillProp( dwSkill );
			if( pItemProp )
			{
				m_aJobSkill[IndexJobSkill].dwExp	= dwExp;
				m_aJobSkill[IndexJobSkill].dwLevel	= dwLevel;
				m_aJobSkill[IndexJobSkill].dwSkill	= dwSkill;
				m_aJobSkill[IndexJobSkill].bStop	= bStop;
				++IndexJobSkill;
				if( 3 < IndexJobSkill ) // 방랑자 까지 뺌
				{
					break;
				}
			}
		}

		memcpy( &pMover->m_aJobSkill[0], &m_aJobSkill[0], sizeof( SKILL ) * 3 ); // 방랑자 스킬 복사

		ItemProp** apSkillProp	= prj.m_aJobSkill[pMover->m_nJob];
		int nJobNum	= prj.m_aJobSkillNum[pMover->m_nJob];
		int k	= 3;
		for( i = 0; i < nJobNum; i++ )
		{
			ItemProp* pSkillProp	= apSkillProp[i];
			pMover->m_aJobSkill[k].dwSkill	= pSkillProp->dwID;
			pMover->m_aJobSkill[k].dwLevel	= 1;
			pMover->m_aJobSkill[k].dwExp	= 0;
			pMover->m_aJobSkill[k].bStop	= TRUE;
			k++;
		}

		// 3. 저장한다.
		*JobSkill	= '\0';
		for( i = 0; i < ( MAX_SKILL_JOB ); i++ )
		{
			char OneJobSkill[64]	= {0,};
			sprintf( OneJobSkill, "%d,%d,%d,%d/", pMover->m_aJobSkill[i].dwExp, pMover->m_aJobSkill[i].dwLevel, pMover->m_aJobSkill[i].dwSkill, pMover->m_aJobSkill[i].bStop );
			strcat( JobSkill, OneJobSkill );
		}
		strcat( JobSkill, "$" );

		sprintf( szSQL, "UPDATE CHARACTER_TBL SET m_aJobSkill = '%s' where m_idPlayer = '%07d' AND serverindex = '%02d'",
			JobSkill, pMover->m_idPlayer, si );
		if( FALSE == pQuery->Exec( szSQL ) )
		{
			char lpString[260]	= { 0, };
			sprintf( lpString, "Update Faild, m_idPlayer = %d", pMover->m_idPlayer );
			AfxMessageBox( lpString );
		}
		safe_delete( pMover );
	}
	SAFE_DELETE_ARRAY( szSQL );
	SAFE_DELETE( pQueryChar );
}
#endif // __CONV_SKILL_11_MONTH_JOB2

 
#ifdef __INITITEMHITPOINT
BOOL CDbManager::InitItemHitPoint( void )
{
	char* szSql		= new char[MAX_QUERY_SIZE];
	CQuery* pQuery1		= new CQuery;
	CQuery* pQuery2		= new CQuery;

	HWND hDlg	= CreateDialog( hInst, MAKEINTRESOURCE( IDD_VERIFYPLAYER ),
		hMainWnd, VerifyPlayerDlgProc );
	ShowWindow( hDlg, SW_SHOW );
	HWND hwndPB;

	int nInitialized	= 0;
	char lpString[32]	= { 0, };

	if( FALSE == pQuery1->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 )
		||FALSE == pQuery2->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
		goto lbl_exit;
	//________________________________________________________________________________

	SetDlgItemText( hDlg, IDC_STATIC2, "" );
	
	sprintf( szSql, "SELECT m_idPlayer, m_Inventory FROM INVENTORY_TBL" );
	if( FALSE == pQuery1->Exec( szSql ) )
		goto lbl_exit;

	hwndPB	= GetDlgItem( hDlg, IDC_PROGRESS1 );
	SendMessage( hwndPB, PBM_SETRANGE, 0, MAKELPARAM( 0, pQuery1->AffectCount ) ); 
	SendMessage( hwndPB, PBM_SETSTEP, (WPARAM) 1, 0 ); 
	
	while( pQuery1->Fetch() )
	{
		++nInitialized;
		sprintf( lpString, "%d/%d", nInitialized, pQuery1->AffectCount );
		SetDlgItemText( hDlg, IDC_STATIC, lpString );
		SendMessage( hwndPB, PBM_STEPIT, 0, 0 ); 
		//________________________________________________________________________________
		CMover* pMover	= new CMover;
		pMover->m_idPlayer	= (u_long)pQuery1->GetInt( "m_idPlayer" );
		//________________________________________________________________________________
		int nOffset		= 0;
		DWORD dwId	= 0;
		char sItem[8192]	= { 0, };
		pQuery1->GetStr( "m_Inventory", sItem );
		int len	= strlen( sItem );
		if( len <= 0 || sItem[len-1] != '$' )
		{
			WriteLog( "INITITEMHITPOINT/01/%d", pMover->m_idPlayer );
			SAFE_DELETE( pMover );
			continue;
		}

		BOOL bEquip;
		int nTrade;
		while( '$' != sItem[nOffset] )
		{
			dwId	= (DWORD)GetIntFromStr( sItem, &nOffset );
			pMover->m_Inventory.m_apItem[dwId].m_dwObjId	= dwId;
			pMover->m_Inventory.m_apItem[dwId].m_dwItemId	= (DWORD)GetIntFromStr( sItem, &nOffset );
			bEquip	= GetIntFromStr( sItem, &nOffset );
			nTrade	= GetIntFromStr( sItem, &nOffset );
			GetStrFromStr( sItem, pMover->m_Inventory.m_apItem[dwId].m_szItemText, &nOffset );
			pMover->m_Inventory.m_apItem[dwId].m_nItemNum	= GetIntFromStr( sItem, &nOffset );
			pMover->m_Inventory.m_apItem[dwId].m_nRepairNumber	= (BYTE)GetIntFromStr( sItem, &nOffset );
			pMover->m_Inventory.m_apItem[dwId].m_nHitPoint	= GetIntFromStr( sItem, &nOffset );
			int nMaxHitPoint	= GetIntFromStr( sItem, &nOffset );
			int nMaterial	= GetIntFromStr( sItem, &nOffset );
			pMover->m_Inventory.m_apItem[dwId].m_byFlag		= (BYTE)GetIntFromStr( sItem, &nOffset );
			pMover->m_Inventory.m_apItem[dwId].SetSerialNumber( GetSerialNumberFromStr( sItem, &nOffset ) );
			pMover->m_Inventory.m_apItem[dwId].SetOption( GetIntFromStr( sItem, &nOffset ) );

			// initialize
			pMover->m_Inventory.m_apItem[dwId].m_nRepairNumber	= 0;
			pMover->m_Inventory.m_apItem[dwId].m_byFlag		= 0;
			ItemProp* pItemProp		= prj.GetItemProp( pMover->m_Inventory.m_apItem[dwId].m_dwItemId );
			if( pItemProp )
				pMover->m_Inventory.m_apItem[dwId].m_nHitPoint	= pItemProp->dwEndurance;
		}

		*sItem	= '\0';
		for( int dwId = 0; dwId < pMover->m_Inventory.m_dwItemMax; dwId++ )
		{
			char s[512]	= { 0, };
			if( pMover->m_Inventory.m_apItem[dwId].IsEmpty() == FALSE )
			{
				sprintf( s, "%d,%d,%d,%d,%s,%d,%d,%d,%d,%d,%d,%d,%d/",
					pMover->m_Inventory.m_apItem[dwId].m_dwObjId, pMover->m_Inventory.m_apItem[dwId].m_dwItemId,
					0, 0, pMover->m_Inventory.m_apItem[dwId].m_szItemText,
					pMover->m_Inventory.m_apItem[dwId].m_nItemNum, pMover->m_Inventory.m_apItem[dwId].m_nRepairNumber,
					pMover->m_Inventory.m_apItem[dwId].m_nHitPoint, 0,
					0, pMover->m_Inventory.m_apItem[dwId].m_byFlag,
					pMover->m_Inventory.m_apItem[dwId].GetSerialNumber(), pMover->m_Inventory.m_apItem[dwId].GetOption() );
				strcat( sItem, s );
			}
		}
		strcat( sItem, "$" );
		//________________________________________________________________________________
		sprintf( szSql, "UPDATE INVENTORY_TBL SET m_Inventory = '%s' WHERE m_idPlayer = '%07d'",
			sItem, pMover->m_idPlayer );

		WriteLog( "%s", szSql );
#if 0
		if( FALSE == pQuery2->Exec( szSql ) )
		{
			WriteLog( "INITITEMHITPOINT/SQL_ERROR/%d", pMover->m_idPlayer );
			SAFE_DELETE( pMover );
			continue;
		}
#endif	// 0
		//________________________________________________________________________________
		MSG msg;
		if( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
		{
			if( !GetMessage( &msg, NULL, 0, 0 ) )
			{
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		SAFE_DELETE( pMover );
	}

lbl_exit:
	SAFE_DELETE( pQuery1 );
	SAFE_DELETE( pQuery2 );
	SAFE_DELETE( szSql );

	EndDialog( hDlg, 0 );
	return TRUE;
}
#endif	// __INITITEMHITPOIN


#ifdef __VERIFY_PLAYER
BOOL CDbManager::VerifyPlayer( void )
{
	char* szSql		= new char[MAX_QUERY_SIZE];
	CQuery* pQuery1		= new CQuery;
	CQuery* pQuery2		= new CQuery;

	HWND hDlg	= CreateDialog( hInst, MAKEINTRESOURCE( IDD_VERIFYPLAYER ),
		hMainWnd, VerifyPlayerDlgProc );
	ShowWindow( hDlg, SW_SHOW );
	HWND hwndPB;

	int nVerified	= 0, nFixed		= 0;
	char lpString[32]	= { 0, };

	if( FALSE == pQuery1->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 )
		||FALSE == pQuery2->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
		goto lbl_exit;
	//________________________________________________________________________________

	SetDlgItemText( hDlg, IDC_STATIC2, "" );
	
	sprintf( szSql, "SELECT* FROM INVENTORY_TBL" );
	if( FALSE == pQuery1->Exec( szSql ) )
		goto lbl_exit;

	hwndPB	= GetDlgItem( hDlg, IDC_PROGRESS1 );
	sprintf( lpString, "%d/%d", nFixed, pQuery1->AffectCount );
	SetDlgItemText( hDlg, IDC_STATIC1, lpString );
	SendMessage( hwndPB, PBM_SETRANGE, 0, MAKELPARAM( 0, pQuery1->AffectCount ) ); 
	SendMessage( hwndPB, PBM_SETSTEP, (WPARAM) 1, 0 ); 
	
	while( pQuery1->Fetch() )
	{
		++nVerified;
		sprintf( lpString, "%d/%d", nVerified, pQuery1->AffectCount );
		SetDlgItemText( hDlg, IDC_STATIC, lpString );
		SendMessage( hwndPB, PBM_STEPIT, 0, 0 ); 
		//________________________________________________________________________________
		CMover* pMover	= new CMover;
		pMover->m_idPlayer	= (u_long)pQuery1->GetInt( "m_idPlayer" );
		//________________________________________________________________________________
		int nOffset		= 0;
		DWORD dwId	= 0;
		char sItem[8192]	= { 0, };
		pQuery1->GetStr( "m_Inventory", sItem );
		int len	= strlen( sItem );
		if( len <= 0 || sItem[len-1] != '$' )
		{
			WriteLog( "VP/01/%d", pMover->m_idPlayer );
			SAFE_DELETE( pMover );
			continue;
		}
		BOOL bEquip;
		int nTrade;
		while( '$' != sItem[nOffset] )
		{
			dwId	= (DWORD)GetIntFromStr( sItem, &nOffset );
			pMover->m_Inventory.m_apItem[dwId].m_dwObjId	= dwId;
			pMover->m_Inventory.m_apItem[dwId].m_dwItemId	= (DWORD)GetIntFromStr( sItem, &nOffset );
			bEquip		= GetIntFromStr( sItem, &nOffset );
			nTrade		= GetIntFromStr( sItem, &nOffset );
			GetStrFromStr( sItem, pMover->m_Inventory.m_apItem[dwId].m_szItemText, &nOffset );
			pMover->m_Inventory.m_apItem[dwId].m_nItemNum	= GetIntFromStr( sItem, &nOffset );
			pMover->m_Inventory.m_apItem[dwId].m_nRepairNumber	= (BYTE)GetIntFromStr( sItem, &nOffset );
			pMover->m_Inventory.m_apItem[dwId].m_nHitPoint	= GetIntFromStr( sItem, &nOffset );
			int nMaxHitPoint	= GetIntFromStr( sItem, &nOffset );
			int nMaterial	= GetIntFromStr( sItem, &nOffset );
			pMover->m_Inventory.m_apItem[dwId].m_byFlag		= (BYTE)GetIntFromStr( sItem, &nOffset );
			pMover->m_Inventory.m_apItem[dwId].SetSerialNumber( GetSerialNumberFromStr( sItem, &nOffset ) );
			pMover->m_Inventory.m_apItem[dwId].SetOption( GetIntFromStr( sItem, &nOffset ) );
		}

		int nIndex	= 0;
		nOffset	= 0;
		char sIndex[512]	= { 0, };
		pQuery1->GetStr( "m_apIndex", sIndex );
		len		= strlen( sIndex );
		if( len <= 0 || sIndex[len-1] != '$' )
		{
			WriteLog( "VP/02/%d", pMover->m_idPlayer );
			SAFE_DELETE( pMover );
			continue;
		}
		while( '$' != sIndex[nOffset] )
		{
			pMover->m_Inventory.m_apIndex[nIndex]	= (DWORD)GetIntFromStr( sIndex, &nOffset );
			nIndex++;
		}

		nOffset		= 0;
		dwId	= 0;
		char sObjIndex[512]		= { 0, };
		pQuery1->GetStr( "m_dwObjIndex", sObjIndex );
		len		= strlen( sObjIndex );
		if( len <= 0 || sObjIndex[len-1] != '$' )
		{
			WriteLog( "VP/03/%d", pMover->m_idPlayer );
			SAFE_DELETE( pMover );
			continue;
		}
		while( '$' != sObjIndex[nOffset] )
		{
			pMover->m_Inventory.m_apItem[dwId].m_dwObjIndex	= (DWORD)GetIntFromStr( sObjIndex, &nOffset );
			dwId++;
		}

		nOffset		= 0;
		nIndex	= 0;
		char sEquipment[512]	= { 0, };
		pQuery1->GetStr( "m_adwEquipment", sEquipment );
		len		= strlen( sEquipment );
		if( len <= 0 || sEquipment[len-1] != '$' )
		{
			WriteLog( "VP/04/%d", pMover->m_idPlayer );
			SAFE_DELETE( pMover );
			continue;
		}
		while( '$' != sEquipment[nOffset] )
		{
//			if( nIndex < MAX_HUMAN_PARTS )
			pMover->m_adwEquipment[nIndex]	= (DWORD)GetIntFromStr( sEquipment, &nOffset );
			nIndex++;
		}
		if( IsAbnormalPlayerData( pMover ) )
		{
			NormalizePlayerData( pMover );
			if( IsAbnormalPlayerData( pMover ) )
			{
				WriteLog( "VP/VP_ERROR/%d", pMover->m_idPlayer );
				SAFE_DELETE( pMover );
				continue;
			}
			//________________________________________________________________________________
			*sItem	= '\0';
			*sIndex		= '\0';
			*sObjIndex	= '\0';
			for( int dwId = 0; dwId < pMover->m_Inventory.m_dwItemMax; dwId++ )
			{
				char s[512]	= { 0, };
				if( pMover->m_Inventory.m_apItem[dwId].IsEmpty() == FALSE )
				{
					sprintf( s, "%d,%d,%d,%d,%s,%d,%d,%d,%d,%d,%d,%d,%d/",
						pMover->m_Inventory.m_apItem[dwId].m_dwObjId, pMover->m_Inventory.m_apItem[dwId].m_dwItemId,
						0, 0, pMover->m_Inventory.m_apItem[dwId].m_szItemText,
						pMover->m_Inventory.m_apItem[dwId].m_nItemNum, pMover->m_Inventory.m_apItem[dwId].m_nRepairNumber,
						pMover->m_Inventory.m_apItem[dwId].m_nHitPoint, 0,
						0, pMover->m_Inventory.m_apItem[dwId].m_byFlag,
						pMover->m_Inventory.m_apItem[dwId].GetSerialNumber(), pMover->m_Inventory.m_apItem[dwId].GetOption() );
					strcat( sItem, s );
				}
				*s	= '\0';
				sprintf( s,  "%d/", pMover->m_Inventory.m_apIndex[dwId] );
				strcat( sIndex, s );
				*s	= '\0';
				sprintf( s, "%d/", pMover->m_Inventory.m_apItem[dwId].m_dwObjIndex );
				strcat( sObjIndex, s );
			}
			strcat( sItem, "$" );
			strcat( sIndex, "$" );
			strcat( sObjIndex, "$" );

			*sEquipment		= '\0';
			for( nIndex = 0; nIndex < MAX_HUMAN_PARTS; nIndex++ )
			{
				char s[32]	= { 0, };
				sprintf( s, "%d/", pMover->m_adwEquipment[nIndex] );
				strcat( sEquipment, s );
			}
			strcat( sEquipment, "$" );
			//________________________________________________________________________________
			sprintf( szSql, "UPDATE INVENTORY_TBL SET m_Inventory = '%s', m_apIndex = '%s', m_dwObjIndex = '%s', m_adwEquipment = '%s' WHERE m_idPlayer = '%07d'",
				sItem, sIndex, sObjIndex, sEquipment, pMover->m_idPlayer );

#ifdef __TEST_VERIFY
			WriteLog( "%s", szSql );
#else	// __TEST_VERIFY
			if( FALSE == pQuery2->Exec( szSql ) )
			{
				WriteLog( "VP/SQL_ERROR/%d", pMover->m_idPlayer );
				SAFE_DELETE( pMover );
				continue;
			}
#endif	// __TEST_VERIFY
			
			WriteLog( "VP/FIXED/%d", pMover->m_idPlayer );
			++nFixed;
			sprintf( lpString, "%d/%d", nFixed, pQuery1->AffectCount );
			SetDlgItemText( hDlg, IDC_STATIC1, lpString );
			//________________________________________________________________________________
		}
		MSG msg;
		if( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
		{
			if( !GetMessage( &msg, NULL, 0, 0 ) )
			{
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		SAFE_DELETE( pMover );
	}
lbl_exit:
	SAFE_DELETE( pQuery1 );
	SAFE_DELETE( pQuery2 );
	SAFE_DELETE( szSql );

	EndDialog( hDlg, 0 );
	return TRUE;
}
#endif	// __VERIFY_PLAYER

#if defined( __VERIFY_PLAYER ) || defined( __PROVIDE ) || defined( __S0707_ITEM_CONV ) || defined(__RECOVER0816) || defined(__ITEM_REMOVE_LIST)
BOOL CALLBACK VerifyPlayerDlgProc( HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam )
{
	switch( iMessage )
	{
		case WM_LBUTTONDOWN:
			SendMessage( hDlg, WM_NCLBUTTONDOWN, HTCAPTION, lParam ); 
			break;
		case WM_LBUTTONDBLCLK:
			EndDialog( hDlg, 0 );
			break;
	}
	return FALSE;
}
#endif	// __VERIFY_PLAYER OR __PROVIDE


#ifdef __S0707_ITEM_CONV
// 아이템 프로퍼티가 바뀌었을시 사용
BOOL CDbManager::ConvItem( void )
{
	CQuery* pQueryChar	= new CQuery;
	CQuery* pQuerySave	= new CQuery;

	int nCount	= 0;
	int nStarted	= 0;
	DWORD dwMaxConv	= 0;
	
	AfxMessageBox( "Start", MB_OK );
	
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

	char* szSQL	= new char[40960];

	HWND hDlg	= CreateDialog( hInst, MAKEINTRESOURCE( IDD_VERIFYPLAYER ),
		hMainWnd, VerifyPlayerDlgProc );
	ShowWindow( hDlg, SW_SHOW );
	HWND hwndPB;

	char lpString[300];
	hwndPB	= GetDlgItem( hDlg, IDC_PROGRESS1 );
	SetDlgItemText( hDlg, IDC_STATIC1, "0 / 0" );
	SetDlgItemText( hDlg, IDC_STATIC, "Init" );
	SendMessage( hwndPB, PBM_SETRANGE, 0, MAKELPARAM( 0, 100 ) ); 

//////////////////////////////////////// Inventory Start ////////////////////////////////////////
//	pQueryChar->Clear();

	sprintf( szSQL, "select count = count(m_idPlayer) from INVENTORY_TBL where serverindex = '%02d'", g_appInfo.dwSys );

	TRACE( szSQL );
	TRACE( "\n" );
	if( FALSE == pQueryChar->Exec( szSQL ) )
	{
		SAFE_DELETE( pQueryChar );
		SAFE_DELETE( pQuerySave );
		AfxMessageBox( szSQL, MB_OK );
		return FALSE;
	}

	if( pQueryChar->Fetch() )
	{
		dwMaxConv = (DWORD)pQueryChar->GetInt( "count" );
		TRACE( "Inven MAX = %d Data Convert\n", dwMaxConv );
	}
	else
	{
		TRACE( "Inven No Data Convert\n" );
		AfxMessageBox( "Inven No Data Convert", MB_OK );
	}

	SetDlgItemText( hDlg, IDC_STATIC2, "Inventory" );
	sprintf( lpString, "%d / %d", 0, dwMaxConv );
	SetDlgItemText( hDlg, IDC_STATIC1, lpString );
	SetDlgItemText( hDlg, IDC_STATIC, "0 %%" );
	SendMessage( hwndPB, PBM_SETPOS, (WPARAM)0, 0 );

	sprintf( szSQL, "select m_idPlayer, m_Inventory from INVENTORY_TBL where serverindex = '%02d'", g_appInfo.dwSys );

	if( FALSE == pQueryChar->Exec( szSQL ) )
	{
		SAFE_DELETE( pQueryChar );
		SAFE_DELETE( pQuerySave );
		AfxMessageBox( szSQL, MB_OK );
		return FALSE;
	}
	
	while( pQueryChar->Fetch() )
	{
		++nStarted;
		TRACE( "Inven converted %f\n", float( float( (float)nStarted / (float)dwMaxConv ) * 100.0f ) );

		sprintf( lpString, "%d / %d", nStarted, dwMaxConv );
		SetDlgItemText( hDlg, IDC_STATIC1, lpString );
		sprintf( lpString, "%d %", int( float( (float)nStarted / (float)dwMaxConv ) * 100.0f ) );
		SetDlgItemText( hDlg, IDC_STATIC, lpString );
		SendMessage( hwndPB, PBM_SETPOS, (WPARAM)(int( float( (float)nStarted / (float)dwMaxConv ) * 100.0f )), 0 );

		CMover* pMover	= new CMover;

		pMover->m_idPlayer	= pQueryChar->GetInt( "m_idPlayer" );

//		if( pMover->m_idPlayer == 668 )
//			continue;

		int nOffset	= 0;
		int nId	= 0;
		char sInventory[6000]	= { 0, };
		pQueryChar->GetStr( "m_Inventory", sInventory );

		BOOL f	= FALSE;

		while( '$' != sInventory[nOffset] )
		{
			CItemElem itemElem;
			nId	= GetOneItem( &itemElem, sInventory, &nOffset );
			if( nId == -1 )
			{
				Error( "1 : %d", pMover->m_idPlayer );
				f	= TRUE;
			}
			else
			{
				if( nId >= MAX_INVENTORY + MAX_HUMAN_PARTS)
				{
					Error( "2 : %d", pMover->m_idPlayer );
					f	= TRUE;
				}
				if( itemElem.m_nItemNum > 0 )
				{
					pMover->m_Inventory.m_apItem[nId]	= itemElem;
				}
				else
				{
					Error( "3 : %d", pMover->m_idPlayer );
					f	= TRUE;
				}
			}
		}
		
		if( f )
		{
			char InvenSave[6000]	= { 0, };
			for( int ch = 0; ch < pMover->m_Inventory.m_dwItemMax; ch++ )	// 0-504
			{	
				char OneItem[512]	= { 0, };
				if( pMover->m_Inventory.m_apItem[ch].IsEmpty() == FALSE )
				{
					ItemProp *pProp = prj.GetItemProp( pMover->m_Inventory.m_apItem[ch].m_dwItemId );
					if( pProp )
					{
						if( pProp->dwItemKind3 == IK3_CLOAK )	// 망토일경우
						{
							sprintf(OneItem, "%d,%d,%d,%d,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d/",
								pMover->m_Inventory.m_apItem[ch].m_dwObjId, pMover->m_Inventory.m_apItem[ch].m_dwItemId,
								0, 0, pMover->m_Inventory.m_apItem[ch].m_szItemText,
								pMover->m_Inventory.m_apItem[ch].m_nItemNum, pMover->m_Inventory.m_apItem[ch].m_nRepairNumber,
								pMover->m_Inventory.m_apItem[ch].m_nHitPoint, 0,
								0, pMover->m_Inventory.m_apItem[ch].m_byFlag,
								pMover->m_Inventory.m_apItem[ch].GetSerialNumber(), pMover->m_Inventory.m_apItem[ch].GetOption(), 
								pMover->m_Inventory.m_apItem[ch].m_bItemResist, pMover->m_Inventory.m_apItem[ch].m_nResistAbilityOption,
								pMover->m_Inventory.m_apItem[ch].m_idGuild );
						}
						else
						{
							sprintf(OneItem, "%d,%d,%d,%d,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d/",
								pMover->m_Inventory.m_apItem[ch].m_dwObjId, pMover->m_Inventory.m_apItem[ch].m_dwItemId,
								0, 0, pMover->m_Inventory.m_apItem[ch].m_szItemText,
								pMover->m_Inventory.m_apItem[ch].m_nItemNum, pMover->m_Inventory.m_apItem[ch].m_nRepairNumber,
								pMover->m_Inventory.m_apItem[ch].m_nHitPoint, 0,
								0, pMover->m_Inventory.m_apItem[ch].m_byFlag,
								pMover->m_Inventory.m_apItem[ch].GetSerialNumber(), pMover->m_Inventory.m_apItem[ch].GetOption()
								,pMover->m_Inventory.m_apItem[ch].m_bItemResist, pMover->m_Inventory.m_apItem[ch].m_nResistAbilityOption
								);
						}
						strncat(InvenSave, OneItem, sizeof(OneItem));
					}
					else
					{
						Error( "ConvItem::InventorySave: << 프로퍼티 없음. %s -> %d", pMover->m_szName, pMover->m_Inventory.m_apItem[ch].m_dwItemId );
						SAFE_DELETE( pQueryChar );
						SAFE_DELETE( pQuerySave );
						AfxMessageBox( "프로퍼티", MB_OK );
						return FALSE;
					}
				}
			}
			strcat( InvenSave, "$" );
			sprintf( szSQL, "UPDATE INVENTORY_TBL SET m_Inventory = '%s' where m_idPlayer = '%07d' and serverindex = '%02d'", InvenSave, pMover->m_idPlayer, g_appInfo.dwSys );

			if( FALSE == pQuerySave->Exec( szSQL ) )
			{
				SAFE_DELETE( pQueryChar );
				SAFE_DELETE( pQuerySave );
				AfxMessageBox( "UPDATE", MB_OK );
				return FALSE;
			}
		}
		safe_delete( pMover );
	}

	// bank

	sprintf( szSQL, "select count = count(m_idPlayer) from BANK_TBL where serverindex = '%02d'", g_appInfo.dwSys );
	if( FALSE == pQueryChar->Exec( szSQL ) )
	{
		SAFE_DELETE( pQueryChar );
		SAFE_DELETE( pQuerySave );
		AfxMessageBox( szSQL, MB_OK );
		return FALSE;
	}

	if( pQueryChar->Fetch() )
	{
		nStarted = 0;
		dwMaxConv = (DWORD)pQueryChar->GetInt( "count" );
		TRACE( "Bank MAX = %d Data Convert\n", dwMaxConv );
	}
	else
	{
		TRACE( "Bank No Data Convert\n" );
		AfxMessageBox( "Bank No Data Convert", MB_OK );
	}

	SetDlgItemText( hDlg, IDC_STATIC2, "Bank" );
	sprintf( lpString, "%d / %d", 0, dwMaxConv );
	SetDlgItemText( hDlg, IDC_STATIC1, lpString );
	SetDlgItemText( hDlg, IDC_STATIC, "0 %%" );
	SendMessage( hwndPB, PBM_SETPOS, (WPARAM)0, 0 );
	
	sprintf( szSQL, "select m_idPlayer, m_Bank from BANK_TBL where serverindex = '%02d'", g_appInfo.dwSys );
	if( FALSE == pQueryChar->Exec( szSQL ) )
	{
		SAFE_DELETE( pQueryChar );
		SAFE_DELETE( pQuerySave );
		AfxMessageBox( szSQL, MB_OK );
		return FALSE;
	}
	
	while( pQueryChar->Fetch() )
	{
		++nStarted;
		TRACE( "Bank converted %f\n", float( float( (float)nStarted / (float)dwMaxConv ) * 100.0f ) );

		sprintf( lpString, "%d / %d", nStarted, dwMaxConv );
		SetDlgItemText( hDlg, IDC_STATIC1, lpString );
		sprintf( lpString, "%d %", int( float( (float)nStarted / (float)dwMaxConv ) * 100.0f ) );
		SetDlgItemText( hDlg, IDC_STATIC, lpString );
		SendMessage( hwndPB, PBM_SETPOS, (WPARAM)(int( float( (float)nStarted / (float)dwMaxConv ) * 100.0f )), 0 );

		CMover* pMover	= new CMover;

		pMover->m_idPlayer	= pQueryChar->GetInt( "m_idPlayer" );
		
		int nOffset	= 0;
		int nId	= 0;
		char Bank[7500]	= { 0, };
		pQueryChar->GetStr( "m_Bank", Bank );

		BOOL f	= FALSE;

		while('$' != Bank[nOffset])
		{
			CItemElem itemElem;
			nId	= GetOneItem( &itemElem, Bank, &nOffset );
			if( nId == -1 )
			{
				Error( "4 : %d", pMover->m_idPlayer );
				f	= TRUE;
			}
			else
			{
				if( nId >= MAX_BANK )
				{
					Error( "5 : %d", pMover->m_idPlayer );
					f	= TRUE;
				}
				if( itemElem.m_nItemNum > 0 )
				{
					pMover->m_Bank.m_apItem[nId]	= itemElem;
				}
				else
				{
					Error( "6 : %d", pMover->m_idPlayer );
					f	= TRUE;
				}
			}
		}
		if( f )
		{
			char BankSave[6000]	= { 0, };
			for( int ch = 0; ch < pMover->m_Bank.m_dwItemMax; ch++ )	// 0-504
			{	
				char OneItem[512]	= { 0, };
				if( pMover->m_Bank.m_apItem[ch].IsEmpty() == FALSE )
				{
					ItemProp *pProp = prj.GetItemProp( pMover->m_Bank.m_apItem[ch].m_dwItemId );
					if( pProp )
					{
						if( pProp->dwItemKind3 == IK3_CLOAK )	// 망토일경우
						{
							sprintf(OneItem, "%d,%d,%d,%d,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d/",
								pMover->m_Bank.m_apItem[ch].m_dwObjId, pMover->m_Bank.m_apItem[ch].m_dwItemId,
								0, 0, pMover->m_Bank.m_apItem[ch].m_szItemText,
								pMover->m_Bank.m_apItem[ch].m_nItemNum, pMover->m_Bank.m_apItem[ch].m_nRepairNumber,
								pMover->m_Bank.m_apItem[ch].m_nHitPoint, 0,
								0, pMover->m_Bank.m_apItem[ch].m_byFlag,
								pMover->m_Bank.m_apItem[ch].GetSerialNumber(), pMover->m_Bank.m_apItem[ch].GetOption(),
								pMover->m_Bank.m_apItem[ch].m_bItemResist, pMover->m_Bank.m_apItem[ch].m_nResistAbilityOption,
								pMover->m_Bank.m_apItem[ch].m_idGuild );
						}
						else
						{
							sprintf(OneItem, "%d,%d,%d,%d,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d/",
								pMover->m_Bank.m_apItem[ch].m_dwObjId, pMover->m_Bank.m_apItem[ch].m_dwItemId,
								0, 0, pMover->m_Bank.m_apItem[ch].m_szItemText,
								pMover->m_Bank.m_apItem[ch].m_nItemNum, pMover->m_Bank.m_apItem[ch].m_nRepairNumber,
								pMover->m_Bank.m_apItem[ch].m_nHitPoint, 0,
								0, pMover->m_Bank.m_apItem[ch].m_byFlag,
								pMover->m_Bank.m_apItem[ch].GetSerialNumber(), pMover->m_Bank.m_apItem[ch].GetOption()
								, pMover->m_Bank.m_apItem[ch].m_bItemResist, pMover->m_Bank.m_apItem[ch].m_nResistAbilityOption
								);
						}
						strncat(BankSave, OneItem, sizeof(OneItem));
					}
					else
					{
						Error( "ConvItem::BankSave: << 프로퍼티 없음. %s -> %d", pMover->m_szName, pMover->m_Bank.m_apItem[ch].m_dwItemId );
						SAFE_DELETE( pQueryChar );
						SAFE_DELETE( pQuerySave );
						AfxMessageBox( "프로퍼티", MB_OK );
						return FALSE;
					}
				}
			}
			strcat(BankSave, "$" );

			sprintf( szSQL, "UPDATE BANK_TBL SET m_Bank = '%s' where m_idPlayer = '%07d' and serverindex = '%02d'", BankSave, pMover->m_idPlayer, g_appInfo.dwSys );
		
			if( FALSE == pQuerySave->Exec( szSQL ) )
			{
				SAFE_DELETE( pQueryChar );
				SAFE_DELETE( pQuerySave );
				AfxMessageBox( "UPDATE", MB_OK );
				return FALSE;
			}
		}
		safe_delete( pMover );
	}

	// guild bank

	sprintf( szSQL, "select count = count(m_idGuild) from GUILD_BANK_TBL where serverindex = '%02d'", g_appInfo.dwSys );

	TRACE( szSQL );
	TRACE( "\n" );
	if( FALSE == pQueryChar->Exec( szSQL ) )
	{
		SAFE_DELETE( pQueryChar );
		SAFE_DELETE( pQuerySave );
		AfxMessageBox( szSQL, MB_OK );
		return FALSE;
	}

	if( pQueryChar->Fetch() )
	{
		nStarted = 0;
		dwMaxConv = (DWORD)pQueryChar->GetInt( "count" );
		TRACE( "GuildBank MAX = %d Data Convert\n", dwMaxConv );
	}
	else
	{
		TRACE( "GuildBank No Data Convert\n" );
		AfxMessageBox( "GuildBank No Data Convert", MB_OK );
	}

	SetDlgItemText( hDlg, IDC_STATIC2, "GuildBank" );
	sprintf( lpString, "%d / %d", 0, dwMaxConv );
	SetDlgItemText( hDlg, IDC_STATIC1, lpString );
	SetDlgItemText( hDlg, IDC_STATIC, "0 %%" );
	SendMessage( hwndPB, PBM_SETPOS, (WPARAM)0, 0 );

	sprintf( szSQL, "select m_idGuild, m_GuildBank from GUILD_BANK_TBL where serverindex = '%02d'", g_appInfo.dwSys );

	TRACE( szSQL );
	TRACE( "\n" );
	if( FALSE == pQueryChar->Exec( szSQL ) )
	{
		SAFE_DELETE( pQueryChar );
		SAFE_DELETE( pQuerySave );
		AfxMessageBox( szSQL, MB_OK );
		return FALSE;
	}

	for ( ; pQueryChar->Fetch() == TRUE ; ++nCount )
	{
		++nStarted;
		TRACE( "GuildBank converted %f\n", float( float( (float)nStarted / (float)dwMaxConv ) * 100.0f ) );

		sprintf( lpString, "%d / %d", nStarted, dwMaxConv );
		SetDlgItemText( hDlg, IDC_STATIC1, lpString );
		sprintf( lpString, "%d %", int( float( (float)nStarted / (float)dwMaxConv ) * 100.0f ) );
		SetDlgItemText( hDlg, IDC_STATIC, lpString );
		SendMessage( hwndPB, PBM_SETPOS, (WPARAM)(int( float( (float)nStarted / (float)dwMaxConv ) * 100.0f )), 0 );
		

		CItemContainer<CItemElem>	GuildBank;	// 길드 창고
		int							nGuildId	= 0;
		
		GuildBank.SetItemContainer( ITYPE_ITEM, MAX_GUILDBANK );
		
		nGuildId		= pQueryChar->GetInt( "m_idGuild" );
		
		int nOffset	= 0;
		int	nId	= 0;
		char Bank[7500]	= { 0, };
		pQueryChar->GetStr( "m_GuildBank", Bank );
		
		BOOL f	= FALSE;

		while( '$' != Bank[nOffset] )
		{
			CItemElem itemElem;
			nId	= GetOneItem( &itemElem, Bank, &nOffset );
			if( nId == -1 )
			{
				Error( "7 : %d", nGuildId );
				f	= TRUE;
			}
			else
			{
				if( nId >= MAX_GUILDBANK )
				{
					Error( "8 : %d", nGuildId );
					f	= TRUE;
				}
				else
				{
					if( itemElem.m_nItemNum > 0 )
					{
						GuildBank.m_apItem[nId] = itemElem;
					}
					else
					{
						Error( "9 : %d", nGuildId );
						f	= TRUE;
					}
				}
			}
		}

		if( f )
		{
			char BankSave[6000]	= { 0, };
			for( int ch = 0; ch < GuildBank.m_dwItemMax; ch++ )	// 0-504
			{	
				char OneItem[512]	= { 0, };
				if( GuildBank.m_apItem[ch].IsEmpty() == FALSE )
				{
					ItemProp *pProp = prj.GetItemProp( GuildBank.m_apItem[ch].m_dwItemId );
					if( pProp )
					{
						if( pProp->dwItemKind3 == IK3_CLOAK )	// 망토일경우
						{
							sprintf(OneItem, "%d,%d,%d,%d,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d/",
								GuildBank.m_apItem[ch].m_dwObjId, GuildBank.m_apItem[ch].m_dwItemId,
								0, 0, GuildBank.m_apItem[ch].m_szItemText,
								GuildBank.m_apItem[ch].m_nItemNum, GuildBank.m_apItem[ch].m_nRepairNumber,
								GuildBank.m_apItem[ch].m_nHitPoint, 0,
								0, GuildBank.m_apItem[ch].m_byFlag,
								GuildBank.m_apItem[ch].GetSerialNumber(), GuildBank.m_apItem[ch].GetOption(),
								GuildBank.m_apItem[ch].m_bItemResist, GuildBank.m_apItem[ch].m_nResistAbilityOption,
								GuildBank.m_apItem[ch].m_idGuild );
						}
						else
						{
							sprintf(OneItem, "%d,%d,%d,%d,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d/",
								GuildBank.m_apItem[ch].m_dwObjId, GuildBank.m_apItem[ch].m_dwItemId,
								0, 0, GuildBank.m_apItem[ch].m_szItemText,
								GuildBank.m_apItem[ch].m_nItemNum, GuildBank.m_apItem[ch].m_nRepairNumber,
								GuildBank.m_apItem[ch].m_nHitPoint, 0,
								0, GuildBank.m_apItem[ch].m_byFlag,
								GuildBank.m_apItem[ch].GetSerialNumber(), GuildBank.m_apItem[ch].GetOption()
								, GuildBank.m_apItem[ch].m_bItemResist, GuildBank.m_apItem[ch].m_nResistAbilityOption
								);
						}
						strncat(BankSave, OneItem, sizeof(OneItem));
					}
					else
					{
						Error( "GuileSave GuildBank: << 프로퍼티 없음. GuildBank -> %d", GuildBank.m_apItem[ch].m_dwItemId );
						SAFE_DELETE( pQueryChar );
						SAFE_DELETE( pQuerySave );
						AfxMessageBox( "GuildBank 프로퍼티", MB_OK );
						return FALSE;
					}
					
				}
			}
			strcat(BankSave, "$" );

			sprintf( szSQL, "UPDATE GUILD_BANK_TBL SET m_GuildBank = '%s' where m_idGuild = '%06d' and serverindex = '%02d'", BankSave, nGuildId, g_appInfo.dwSys );

			if( FALSE == pQuerySave->Exec( szSQL ) )
			{
				SAFE_DELETE( pQueryChar );
				SAFE_DELETE( pQuerySave );
				AfxMessageBox( "GuildBank UPDATE", MB_OK );
				return FALSE;
			}
		}
	}

	SAFE_DELETE( pQueryChar );
	SAFE_DELETE( pQuerySave );

	return TRUE;
}
#endif	// __S0707_ITEM_CONV

#ifdef __RECOVER0816
BOOL CDbManager::RecoverCharacter( void )
{
	CQuery* pQueryChar	= new CQuery;
	CQuery* pQuerySave	= new CQuery;

	int nCount	= 0;
	int nStarted	= 0;
	int	nMax	= 0;
	
	AfxMessageBox( "call RecoverCharacter()", MB_OK );
	
	if( FALSE == pQueryChar->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
	{
		SAFE_DELETE( pQueryChar );
		AfxMessageBox( "1" );
		return FALSE;
	}

	if( FALSE == pQuerySave->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
	{
		SAFE_DELETE( pQuerySave );
		AfxMessageBox( "1" );
		return FALSE;
	}

	char* szSQL	= new char[40960];

	HWND hDlg	= CreateDialog( hInst, MAKEINTRESOURCE( IDD_VERIFYPLAYER ),
		hMainWnd, VerifyPlayerDlgProc );
	ShowWindow( hDlg, SW_SHOW );
	HWND hwndPB;

	char lpString[300];
	hwndPB	= GetDlgItem( hDlg, IDC_PROGRESS1 );
	SetDlgItemText( hDlg, IDC_STATIC1, "0 / 0" );
	SetDlgItemText( hDlg, IDC_STATIC, "Init" );
	SendMessage( hwndPB, PBM_SETRANGE, 0, MAKELPARAM( 0, 100 ) ); 

//	pQueryChar->Clear();

	sprintf( szSQL, "SELECT COUNT = COUNT(m_idPlayer) from CHARACTER_TBL" );
	TRACE( "%s\n", szSQL );

	if( FALSE == pQueryChar->Exec( szSQL ) )
	{
		SAFE_DELETE( pQueryChar );
		SAFE_DELETE( pQuerySave );
		AfxMessageBox( szSQL, MB_OK );
		safe_delete_array( szSQL );
		return FALSE;
	}

	if( pQueryChar->Fetch() )
	{
		nMax	= pQueryChar->GetInt( "count" );
		TRACE( "MAX = %d\n", nMax );
	}
	
	sprintf( lpString, "%d / %d", 0, nMax );
	SetDlgItemText( hDlg, IDC_STATIC1, lpString );
	SetDlgItemText( hDlg, IDC_STATIC, "0 %%" );
	SendMessage( hwndPB, PBM_SETPOS, (WPARAM)0, 0 );

//	pQueryChar->Clear();

	sprintf( szSQL, "SELECT m_idPlayer, m_nLevel FROM CHARACTER_TBL" );

	if( FALSE == pQueryChar->Exec( szSQL ) )
	{
		SAFE_DELETE( pQueryChar );
		SAFE_DELETE( pQuerySave );
		AfxMessageBox( szSQL, MB_OK );
		safe_delete_array( szSQL );
		return FALSE;
	}
	
	while( pQueryChar->Fetch() )
	{
		++nStarted;
		TRACE( "%f\n", float( float( (float)nStarted / (float)nMax ) * 100.0f ) );

		sprintf( lpString, "%d / %d", nStarted, nMax );
		SetDlgItemText( hDlg, IDC_STATIC1, lpString );
		sprintf( lpString, "%d %", int( float( (float)nStarted / (float)nMax ) * 100.0f ) );
		SetDlgItemText( hDlg, IDC_STATIC, lpString );
		SendMessage( hwndPB, PBM_SETPOS, (WPARAM)(int( float( (float)nStarted / (float)nMax ) * 100.0f ) ), 0 );

		u_long idPlayer	= (u_long)pQueryChar->GetInt( "m_idPlayer" );
		LONG nLevel	= (LONG)pQueryChar->GetInt( "m_nLevel" );
		long nRemainGP	= 0;

		for( int i = 1; i < nLevel; i++ )
			nRemainGP	+= prj.m_aExpCharacter[i + 1].dwLPPoint;

		int nHitPoint	= ( nLevel * 20 ) + ( 15 * 10 ) + ( 15 * 4 ); 
		int nManaPoint	= ( nLevel * 2 ) + ( 15 * 8 ) + 22;
		int nFatiguePoint	= ( nLevel * 2 ) + ( 15 * 7 ) + ( 15 *2 ) + ( 15 * 4 );

		sprintf( szSQL, "UPDATE CHARACTER_TBL SET m_nRemainGP = %d, m_nRemainLP = 0, m_nHitPoint = %d, m_nManaPoint = %d, m_nFatiguePoint = %d, m_nStr = 15, m_nSta = 15, m_nDex = 15, m_nInt = 15 WHERE m_idPlayer = '%07d'"
			, nRemainGP, nHitPoint, nManaPoint, nFatiguePoint, idPlayer );

		TRACE( "%s\n", szSQL );

		if( FALSE == pQuerySave->Exec( szSQL ) )
		{
			SAFE_DELETE( pQueryChar );
			SAFE_DELETE( pQuerySave );
			AfxMessageBox( "2", MB_OK );
			safe_delete_array( szSQL );
			return FALSE;
		}
	}
//	pQueryChar->Clear();
//	pQuerySave->Clear();
	SAFE_DELETE( pQueryChar );
	SAFE_DELETE( pQuerySave );
	return TRUE;
}
#endif	// __RECOVER0816

#ifdef __CONV_SKILL
void CDbManager::ConvSkill( void )
{
	int nStarted	= 0;

	CQuery* pQueryChar	= new CQuery;
	CQuery* pQuery	= new CQuery;

	int nCount	= 0;
	
	if( FALSE == pQueryChar->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
	{
		SAFE_DELETE( pQueryChar );
		return;
	}
	if( FALSE == pQuery->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
	{
		SAFE_DELETE( pQueryChar );
		SAFE_DELETE( pQuery );
		return;
	}

	char* szSQL	= new char[40960];
	sprintf( szSQL, "SELECT m_idPlayer, serverindex, m_nJob, m_aJobSkill from CHARACTER_TBL" );

	if( FALSE == pQueryChar->Exec( szSQL ) )
	{
		SAFE_DELETE( pQueryChar );
		SAFE_DELETE( pQuery );
		return;
	}

	while( pQueryChar->Fetch() )
	{
		if( (++nStarted % 100) == 0 )
		{
			TRACE( "%d converted\n", nStarted );
		}

		int nJob	= pQueryChar->GetInt( "m_nJob" );
		if( nJob == JOB_VAGRANT )
			continue;

		CMover* pMover	= new CMover;

		// 0. 스킬 초기화
		for( int i = 0; i < MAX_JOB_SKILL + MAX_EXPERT_SKILL; i++ )
		{
			pMover->m_aJobSkill[i].dwSkill	= NULL_ID;
			pMover->m_aJobSkill[i].dwExp	= 0;
			pMover->m_aJobSkill[i].dwLevel	= 1;
			pMover->m_aJobSkill[i].bStop = TRUE;
		}

		pMover->m_idPlayer	= pQueryChar->GetInt( "m_idPlayer" );
		u_long si	= pQueryChar->GetInt( "serverindex" );
		pMover->m_nJob	= nJob;

		// 1. 직업 스킬 로드
		int CountStr	= 0;
		int IndexJobSkill	= 0;
		char JobSkill[512]	= { 0, };
		pQueryChar->GetStr( "m_aJobSkill", JobSkill );
		
		int len		= strlen( JobSkill );
		if( len < 1 || JobSkill[len-1] != '$' )
		{
			TRACE( "%d\n", pMover->m_idPlayer );
			safe_delete( pMover );
			continue;
		}

		DWORD dwExp, dwLevel, dwSkill;
		BOOL bStop;
		SKILL	m_aJobSkill[MAX_JOB_SKILL + MAX_EXPERT_SKILL];
		memset( m_aJobSkill, 0, sizeof(SKILL) * ( MAX_JOB_SKILL + MAX_EXPERT_SKILL ) );
		while( '$' != JobSkill[CountStr] )
		{
			dwExp	= (DWORD)GetIntFromStr( JobSkill, &CountStr );
			dwLevel	= (DWORD)GetIntFromStr( JobSkill, &CountStr );
			dwSkill	= (DWORD)GetIntFromStr( JobSkill, &CountStr );
			bStop	= (BOOL)GetIntFromStr( JobSkill, &CountStr );
			ItemProp* pItemProp	= prj.GetSkillProp( dwSkill );
			if( pItemProp )
			{
				{
					m_aJobSkill[IndexJobSkill].dwExp	= dwExp;
					m_aJobSkill[IndexJobSkill].dwLevel	= dwLevel;
					m_aJobSkill[IndexJobSkill].dwSkill	= dwSkill;
					m_aJobSkill[IndexJobSkill].bStop	= bStop;
					IndexJobSkill++;
				}
			}
		}

		memcpy( &pMover->m_aJobSkill[0], &m_aJobSkill[0], sizeof( SKILL ) * 3 ); // 방랑자 스킬 복사

		ItemProp** apSkillProp	= prj.m_aJobSkill[pMover->m_nJob];
		int nJobNum	= prj.m_aJobSkillNum[pMover->m_nJob];
		int k	= 3;
		for( i = 0; i < nJobNum; i++ )
		{
			BOOL bIs = FALSE;
			ItemProp* pSkillProp	= apSkillProp[i];
			for( int j = 3 ; j < IndexJobSkill && bIs == FALSE; ++j )
			{
				if( pSkillProp->dwID == m_aJobSkill[j].dwSkill )
				{
					// 이미 있는 스킬
					bIs = TRUE;
					memcpy( &pMover->m_aJobSkill[k], &m_aJobSkill[j], sizeof(SKILL) );
					break;
				}
			}

			if( bIs == FALSE )
			{
				pMover->m_aJobSkill[k].dwSkill	= pSkillProp->dwID;
				pMover->m_aJobSkill[k].dwLevel	= 1;
				pMover->m_aJobSkill[k].dwExp	= 0;
				pMover->m_aJobSkill[k].bStop	= TRUE;
			}

			k++;
		}

		// 3. 저장한다.
		*JobSkill	= '\0';
		for( i = 0; i < ( MAX_JOB_SKILL + MAX_EXPERT_SKILL ); i++ )
		{
			char OneJobSkill[64]	= {0,};
			sprintf( OneJobSkill, "%d,%d,%d,%d/", pMover->m_aJobSkill[i].dwExp, pMover->m_aJobSkill[i].dwLevel, pMover->m_aJobSkill[i].dwSkill, pMover->m_aJobSkill[i].bStop );
			strcat( JobSkill, OneJobSkill );
		}
		strcat( JobSkill, "$" );

		sprintf( szSQL, 
			"UPDATE CHARACTER_TBL SET m_aJobSkill = '%s' where m_idPlayer = '%07d' AND serverindex = '%02d'",
			JobSkill, pMover->m_idPlayer, si );
		if( FALSE == pQuery->Exec( szSQL ) )
		{
			char lpString[260]	= { 0, };
			sprintf( lpString, "Update Faild, m_idPlayer = %d", pMover->m_idPlayer );
			AfxMessageBox( lpString );
		}
		safe_delete( pMover );
	}
	SAFE_DELETE_ARRAY( szSQL );
	SAFE_DELETE( pQueryChar );
}
#endif	// __CONV_SKILL

#ifdef __CONV_SKILL_STOP
void CDbManager::ConvSkillStop( void )
{
	int nStarted	= 0;

	CQuery* pQueryChar	= new CQuery;
	CQuery* pQuery	= new CQuery;

	int nCount	= 0;
	
	if( FALSE == pQueryChar->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
	{
		SAFE_DELETE( pQueryChar );
		return;
	}
	if( FALSE == pQuery->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
	{
		SAFE_DELETE( pQueryChar );
		SAFE_DELETE( pQuery );
		return;
	}

	char* szSQL	= new char[40960];
	sprintf( szSQL, "SELECT m_idPlayer, serverindex, m_aJobSkill from CHARACTER_TBL );

	if( FALSE == pQueryChar->Exec( szSQL ) )
	{
		SAFE_DELETE( pQueryChar );
		SAFE_DELETE( pQuery );
		return;
	}

	while( pQueryChar->Fetch() )
	{
		if( (++nStarted % 100) == 0 )
		{
			TRACE( "%d converted\n", nStarted );
		}

		CMover* pMover	= new CMover;

		pMover->m_idPlayer	= pQueryChar->GetInt( "m_idPlayer" );
		u_long si	= pQueryChar->GetInt( "serverindex" );

		// 1. 직업 스킬 로드
		int CountStr	= 0;
		int IndexJobSkill	= 0;
		char JobSkill[512]	= { 0, };
		pQueryChar->GetStr( "m_aJobSkill", JobSkill );
		
		int len		= strlen( JobSkill );
		if( len < 1 || JobSkill[len-1] != '$' )
		{
			TRACE( "%d\n", pMover->m_idPlayer );
			safe_delete_array( pMover );
			continue;
		}

		DWORD dwExp, dwLevel, dwSkill;
		while( '$' != JobSkill[CountStr] )
		{
			pMover->m_aJobSkill[IndexJobSkill].dwExp	= (DWORD)GetIntFromStr( JobSkill, &CountStr );
			pMover->m_aJobSkill[IndexJobSkill].dwLevel	= (DWORD)GetIntFromStr( JobSkill, &CountStr );
			pMover->m_aJobSkill[IndexJobSkill].dwSkill	= (DWORD)GetIntFromStr( JobSkill, &CountStr );
			if( JobSkill[CountStr + 1 ] == '/' )
			{
				int aaa = (DWORD)GetIntFromStr( JobSkill, &CountStr );
			}
			IndexJobSkill++;
			pMover->m_aJobSkill[IndexJobSkill].bStop = TRUE;
		}

		// 3. 저장한다.
		*JobSkill	= '\0';
		for( int i = 0; i < ( MAX_JOB_SKILL + MAX_EXPERT_SKILL ); i++ )
		{
			char OneJobSkill[64]	= {0,};
			sprintf( OneJobSkill, "%d,%d,%d,%d/", pMover->m_aJobSkill[i].dwExp, pMover->m_aJobSkill[i].dwLevel, pMover->m_aJobSkill[i].dwSkill, pMover->m_aJobSkill[i].bStop );
			strcat( JobSkill, OneJobSkill );
		}
		strcat( JobSkill, "$" );

		sprintf( szSQL, 
			"UPDATE CHARACTER_TBL SET m_aJobSkill = '%s' where m_idPlayer = '%07d' AND serverindex = '%02d'",
			JobSkill, pMover->m_idPlayer, si );
		if( FALSE == pQuery->Exec( szSQL ) )
		{
			char lpString[260]	= { 0, };
			sprintf( lpString, "업데이트 실패, m_idPlayer = %d", pMover->m_idPlayer );
			AfxMessageBox( lpString );
		}

		safe_delete( pMover );
	}

	SAFE_DELETE_ARRAY( szSQL );
	SAFE_DELETE( pQueryChar );
}
#endif // __CONV_SKILL_STOP

#ifdef __CONV_SERIAL_NUMBER
BOOL CDbManager::SN( void )
{
	CQuery* pQueryChar	= new CQuery;
	CQuery* pQuery	= new CQuery;

	int nCount	= 0;
	
	if( FALSE == pQueryChar->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
	{
		SAFE_DELETE( pQueryChar );
		return FALSE;
	}
	if( FALSE == pQuery->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
	{
		SAFE_DELETE( pQueryChar );
		SAFE_DELETE( pQuery );
		return FALSE;
	}

	char* szSQL	= new char[40960];
	sprintf( szSQL, "SELECT * from INVENTORY_TBL" );

	if( FALSE == pQueryChar->Exec( szSQL ) )
	{
		SAFE_DELETE( pQueryChar );
		SAFE_DELETE( pQuery );
		return FALSE;
	}

	while( pQueryChar->Fetch() )
	{
		CMover* pMover	= new CMover;
		int serverindex	= pQueryChar->GetInt( "serverindex" );
		int m_idPlayer	= pQueryChar->GetInt( "m_idPlayer" );

		// m_Inventory
		int CountStr	= 0;
		DWORD IndexItem	= 0;
		char Inven[6000]	= { 0, };

		pQueryChar->GetStr("m_Inventory", Inven);
		int len		= strlen( Inven );
		if( len < 1 || Inven[len-1] != '$' )
		{
			TRACE( "%d\n", pMover->m_idPlayer );
			safe_delete( pMover );
			continue;
		}

		BOOL bEquip;
		int nTrade;
		while('$' != Inven[CountStr])
		{
			IndexItem = (DWORD)GetIntFromStr(Inven, &CountStr);
			pMover->m_Inventory.m_apItem[IndexItem].m_dwObjId = IndexItem;
			pMover->m_Inventory.m_apItem[IndexItem].m_dwItemId = (DWORD)GetIntFromStr(Inven, &CountStr);
			bEquip = GetIntFromStr(Inven, &CountStr);
			nTrade = GetIntFromStr(Inven, &CountStr);
			GetStrFromStr(Inven, pMover->m_Inventory.m_apItem[IndexItem].m_szItemText, &CountStr);
			pMover->m_Inventory.m_apItem[IndexItem].m_nItemNum = GetIntFromStr(Inven, &CountStr);
			pMover->m_Inventory.m_apItem[IndexItem].m_nRepairNumber		= (BYTE)GetIntFromStr(Inven, &CountStr);
			pMover->m_Inventory.m_apItem[IndexItem].m_nHitPoint = GetIntFromStr(Inven, &CountStr);
			int nMaxHitPoint = GetIntFromStr(Inven, &CountStr);
			int nMaterial = GetIntFromStr(Inven, &CountStr);
			pMover->m_Inventory.m_apItem[IndexItem].m_byFlag = (BYTE)GetIntFromStr(Inven, &CountStr);
			pMover->m_Inventory.m_apItem[IndexItem].SetSerialNumber( GetSerialNumberFromStr(Inven, &CountStr) );
			pMover->m_Inventory.m_apItem[IndexItem].SetOption( GetIntFromStr( Inven, &CountStr ) );
		}

		// m_apIndex
		CountStr	= 0;
		int itemCount	= 0;
		char m_apIndex[512]		= { 0, };
		pQueryChar->GetStr( "m_apIndex", m_apIndex );
		while( '$' != m_apIndex[CountStr] )
		{
			pMover->m_Inventory.m_apIndex[itemCount]	= (DWORD)GetIntFromStr( m_apIndex, &CountStr );
			itemCount++;
		}

		// m_dwObjIndex
		CountStr	= 0;
		int IndexObjIndex	= 0;
		char ObjIndex[512]	= {0,};
		pQueryChar->GetStr( "m_dwObjIndex", ObjIndex );
		while( '$' != ObjIndex[CountStr] )
		{
			pMover->m_Inventory.m_apItem[IndexObjIndex].m_dwObjIndex	= (DWORD)GetIntFromStr( ObjIndex, &CountStr );
			IndexObjIndex++;
		}

		// m_adwEquipment
		CountStr	= 0;
		int IndexEquipment	= 0;
		char Equipment[500]		= {0,};
		pQueryChar->GetStr( "m_adwEquipment", Equipment );
		while( '$' != Equipment[CountStr] )
		{
			if( IndexEquipment < MAX_HUMAN_PARTS )
				pMover->m_adwEquipment[IndexEquipment]	= (DWORD)GetIntFromStr( Equipment, &CountStr );
			IndexEquipment++;
		}

		/*
		// 2. 이제부터 장착 한 아이템을 다 내려 놓고,
		for( DWORD dwParts	= 0; dwParts < MAX_HUMAN_PARTS; dwParts++ )
		{
			if( pMover->m_Inventory.GetEquip( dwParts ) )
			{
				if( pMover->m_Inventory.UnEquip( dwParts ) == FALSE )
				{
					AfxMessageBox( "2" );
					continue;
				}
				else
				{
					pMover->m_adwEquipment[dwParts]	= NULL_ID;
				}
			}
		}
		*/
		// 2. 이제부터 의미있는 시리얼 넘버를 부여하고,
		for( int i = 0; i < pMover->m_Inventory.m_dwItemMax; i++ )
		{
			CItemElem* pItemElem	= pMover->m_Inventory.GetAtId( i );
			if( pItemElem )
			{
				if( pItemElem->GetSerialNumber() == 0 )
					pItemElem->SetSerialNumber();
			}
		}

		// 3. 저장한다.
		char NullStr[2] = "$";
		*Inven	= '\0';
		*m_apIndex	= '\0';
		*ObjIndex	= '\0';
		*Equipment	= '\0';
		for( int ch = 0; ch < pMover->m_Inventory.m_dwItemMax; ch++ )
		{	
			char OneItem[512]	= { 0, };
			if( pMover->m_Inventory.m_apItem[ch].IsEmpty() == FALSE )
			{
				sprintf(OneItem, "%d,%d,%d,%d,%s,%d,%d,%d,%d,%d,%d,%d,%d/",
					pMover->m_Inventory.m_apItem[ch].m_dwObjId, pMover->m_Inventory.m_apItem[ch].m_dwItemId,
					0, 0, pMover->m_Inventory.m_apItem[ch].m_szItemText,
					pMover->m_Inventory.m_apItem[ch].m_nItemNum, pMover->m_Inventory.m_apItem[ch].m_nRepairNumber,
					pMover->m_Inventory.m_apItem[ch].m_nHitPoint, 0,
					0, pMover->m_Inventory.m_apItem[ch].m_byFlag,
					pMover->m_Inventory.m_apItem[ch].GetSerialNumber(), pMover->m_Inventory.m_apItem[ch].GetOption() );
				strncat(Inven, OneItem, sizeof(OneItem));
			}
			char Onem_apIndex[10] = {0,};
			sprintf(Onem_apIndex, "%d/", pMover->m_Inventory.m_apIndex[ch]);
			strncat(m_apIndex, Onem_apIndex, sizeof(Onem_apIndex));

			char sPerObjIndex[16]	= { 0, };
			sprintf( sPerObjIndex, "%d/", pMover->m_Inventory.m_apItem[ch].m_dwObjIndex );
			strcat( ObjIndex, sPerObjIndex );
		}
		strncat( Inven, NullStr, sizeof(NullStr));
		strncat(m_apIndex, NullStr, sizeof(NullStr));
		strcat( ObjIndex, NullStr );

		for( i = 0 ; i < MAX_HUMAN_PARTS ; i++)
		{
			char OneEquipment[20] = {0,};
			sprintf(OneEquipment, "%d/", pMover->m_adwEquipment[i]);
			strncat(Equipment, OneEquipment, sizeof(OneEquipment));
		}
		strncat(Equipment, NullStr, sizeof(NullStr));

		sprintf( szSQL, "UPDATE INVENTORY_TBL SET m_Inventory = '%s', m_apIndex = '%s', m_dwObjIndex = '%s', m_adwEquipment = '%s' WHERE serverindex = '%02d' AND m_idPlayer = '%07d'",
			Inven, m_apIndex, ObjIndex, Equipment, serverindex, m_idPlayer );
		if( FALSE == pQuery->Exec( szSQL ) )
		{
			AfxMessageBox( "3" );
		}

//		pQuery->Clear();

		if( ( ++nCount % 100 ) == 0 )
		{
			TRACE( "%d completed\n", nCount );
//			g_MyTrace.Add( CMyTrace::Key( "prog" ), TRUE, "%d completed", nCount );
		}

		safe_delete( pMover );
	}


//	pQueryChar->Clear();

//	pQueryChar->Clear();
//	pQueryChar->DisConnect();

	SAFE_DELETE_ARRAY( szSQL );
	SAFE_DELETE( pQueryChar );

	AfxMessageBox( "Serial Number Converted" );
	return TRUE;
}
#endif	// __CONV_SERIAL_NUMBER


BOOL CDbManager::ConvItem050615( void )
{
	return TRUE;
}

void CDbManager::ConvChangeItem050615( CMover* pMover )
{
	// Inventory
	for( DWORD ch = 0; ch < pMover->m_Inventory.m_dwItemMax; ch++ )	// 0-504
	{	
		if( pMover->m_Inventory.m_apItem[ch].IsEmpty() == FALSE )
		{
			BOOL bCharged = FALSE;
			DWORD dwItemId = pMover->m_Inventory.m_apItem[ch].m_dwItemId;

			if( (10206 <= dwItemId && dwItemId <= 10224) || (10270 <= dwItemId && dwItemId <= 10286) || (10299 <= dwItemId && dwItemId <= 10347))
			{
				pMover->m_Inventory.m_apItem[ch].m_bCharged = TRUE;
			}				
		}
	}
	// Bank
	for( DWORD ch = 0; ch < pMover->m_Bank[pMover->m_nSlot].m_dwItemMax; ch++ )	// 0-504
	{			
		if( pMover->m_Bank[pMover->m_nSlot].m_apItem[ch].IsEmpty() == FALSE )
		{
			BOOL bCharged = FALSE;
			DWORD dwItemId = pMover->m_Bank[pMover->m_nSlot].m_apItem[ch].m_dwItemId;
			
			if( (10206 <= dwItemId && dwItemId <= 10224) || (10270 <= dwItemId && dwItemId <= 10286) || (10299 <= dwItemId && dwItemId <= 10347))
			{
				pMover->m_Bank[pMover->m_nSlot].m_apItem[ch].m_bCharged = TRUE;
			}				
		}
	}
}

void CDbManager::ConvChangeGuild050615( CItemContainer< CItemElem  >*  GuildBank )
{
	for( DWORD ch = 0; ch < GuildBank->m_dwItemMax; ch++ )	// 0-504
	{			
		if( GuildBank->m_apItem[ch].IsEmpty() == FALSE )
		{
			BOOL bCharged = FALSE;
			DWORD dwItemId = GuildBank->m_apItem[ch].m_dwItemId;
			
			if( (10206 <= dwItemId && dwItemId <= 10224) || (10270 <= dwItemId && dwItemId <= 10286) || (10299 <= dwItemId && dwItemId <= 10347))
			{
				GuildBank->m_apItem[ch].m_bCharged = TRUE;
			}				
		}
	}
}