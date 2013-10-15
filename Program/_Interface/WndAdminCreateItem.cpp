#include "stdafx.h"
#include "AppDefine.h"
#include "WndAdminCreateItem.h"


CWndAdminCreateItem::CWndAdminCreateItem() 
{ 
} 
CWndAdminCreateItem::~CWndAdminCreateItem() 
{ 
} 
void CWndAdminCreateItem::OnDraw( C2DRender* p2DRender ) 
{ 
} 
CString CWndAdminCreateItem::MakeName( ItemProp *pProp )
{
	CString szName;
	szName.Format( "%s (Lv%d)", pProp->szName, pProp->dwLimitLevel1 );
	return szName;
}
void CWndAdminCreateItem::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndListBox* pListBox = (CWndListBox*)GetDlgItem( WIDC_CONTROL1 );
	for( int i = 0; i < prj.m_aPropItem.GetSize(); i++ )
	{
		ItemProp* pItemProp =  prj.GetItemProp( i );
		if( pItemProp )
		{
			if( GetLanguage() != LANG_KOR && pItemProp->nVer >= 7 && pItemProp->bCharged == TRUE )
				continue;
			int nIndex = pListBox->AddString( MakeName( pItemProp ) );
			pListBox->SetItemDataPtr( nIndex, pItemProp );
		}
	}
	CWndComboBox* pWndItemKind = (CWndComboBox*)GetDlgItem( WIDC_ITEM_KIND );
	CWndComboBox* pWndItemSex = (CWndComboBox*)GetDlgItem( WIDC_ITEM_SEX );
	CWndComboBox* pWndItemJob = (CWndComboBox*)GetDlgItem( WIDC_ITEM_JOB );
	CWndEdit* pWndLevel = (CWndEdit*)GetDlgItem( WIDC_LEVEL );
	CStringArray strArray;
	CScript::GetFindIdToArray( "IK2_", &strArray );
	int nIndex = pWndItemKind->AddString( "ALL" );
	pWndItemKind->SetItemData( nIndex, NULL_ID );
	for( i = 0; i < strArray.GetSize(); i++ )
	{
		nIndex = pWndItemKind->AddString( strArray.GetAt( i ) );
		DWORD dwNum = CScript::GetDefineNum( strArray.GetAt( i ) );
		pWndItemKind->SetItemData( nIndex, dwNum );
	}
	pWndItemKind->m_wndListBox.SortListBox();
	nIndex = pWndItemKind->m_wndListBox.FindString( 0, "ALL" );
	pWndItemKind->SetCurSel( nIndex );
	strArray.RemoveAll();
	CScript::GetFindIdToArray( "SEX_", &strArray );
	for( i = 0; i < strArray.GetSize(); i++ )
	{
		nIndex = pWndItemSex->AddString( strArray.GetAt( i ) );
		DWORD dwNum = CScript::GetDefineNum( strArray.GetAt( i ) );
		pWndItemSex->SetItemData( nIndex, dwNum );
	}
	pWndItemSex->SetCurSel( 2 );

	strArray.RemoveAll();
	CScript::GetFindIdToArray( "JOB_", &strArray );
	nIndex = pWndItemJob->AddString( "ALL" );
	pWndItemJob->SetItemData( nIndex, -1 );
	for( i = 0; i < strArray.GetSize(); i++ )
	{
		nIndex = pWndItemJob->AddString( strArray.GetAt( i ) );
		DWORD dwNum = CScript::GetDefineNum( strArray.GetAt( i ) );
		pWndItemJob->SetItemData( nIndex, dwNum );
	}
	pWndItemJob->m_wndListBox.SortListBox();
	nIndex = pWndItemJob->m_wndListBox.FindString( 0, "ALL" );
	pWndItemJob->SetCurSel( nIndex );
	// 좌표 이동 

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndAdminCreateItem::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_ADMIN_CREATEITEM, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndAdminCreateItem::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndAdminCreateItem::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 

	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndAdminCreateItem::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndAdminCreateItem::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndAdminCreateItem::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndAdminCreateItem::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	ItemProp* pItemProp;
	CWndListBox* pListBox = (CWndListBox*)GetDlgItem( WIDC_CONTROL1 );
	if( nID == WIDC_ITEM_KIND || nID == WIDC_ITEM_SEX || nID == WIDC_ITEM_JOB || nID == WIDC_LEVEL )
	{
		if( message == WNM_SELCHANGE || message == EN_CHANGE )
		{
			DWORD dwLevel = 0;
			CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_LEVEL );
			CString string = pWndEdit->GetString();
			dwLevel = atoi( string );
			CWndComboBox* pWndItemKind = (CWndComboBox*)GetDlgItem( WIDC_ITEM_KIND );
			CWndComboBox* pWndItemSex = (CWndComboBox*)GetDlgItem( WIDC_ITEM_SEX );
			CWndComboBox* pWndItemJob = (CWndComboBox*)GetDlgItem( WIDC_ITEM_JOB );
			pListBox->ResetContent();
			for( int i = 0; i < prj.m_aPropItem.GetSize(); i++ )
			{
				ItemProp* pItemProp =  prj.GetItemProp( i );
				DWORD dwKind = pWndItemKind->GetItemData( pWndItemKind->GetCurSel() );
				DWORD dwSex = pWndItemSex->GetItemData( pWndItemSex->GetCurSel() );
				DWORD dwJob = pWndItemJob->GetItemData( pWndItemJob->GetCurSel() );
				if( pItemProp && ( pItemProp->dwItemKind2 == dwKind || dwKind == NULL_ID ) &&
					 ( pItemProp->dwItemSex == dwSex || dwSex == SEX_SEXLESS ) )
				{
					if( pItemProp->dwLimitLevel1 >= dwLevel && ( pItemProp->dwItemJob == dwJob || dwJob == -1 ) )
					{
						if( GetLanguage() != LANG_KOR && pItemProp->nVer >= 7 && pItemProp->bCharged == TRUE )
							continue;
						int nIndex = pListBox->AddString( MakeName( pItemProp ) );
						pListBox->SetItemDataPtr( nIndex, pItemProp );
					}
				}
			}
		}
	}
	else
	if( nID == WIDC_OK || ( nID == WIDC_CONTROL1 && message == WNM_DBLCLK ) )
	{
		CString string;
		CWndText* pWndItemName = (CWndText*)GetDlgItem( WIDC_ITEM_NAME );
		CWndText* pWndItemNum = (CWndText*)GetDlgItem( WIDC_ITEM_NUM );
		DWORD dwNum;
		if( pWndItemNum->m_string.GetLength() ) 
		{
			dwNum = _ttoi(pWndItemNum->m_string);
		}
		else 
		{
			dwNum = 1;
		}
		
		dwNum = ( dwNum == 0? 1:dwNum );
		if( pWndItemName->m_string.GetLength() ) 
		{
			CString str2;
			str2 = pWndItemName->m_string;
			string.Format( "/ci %s %d",str2, dwNum);

			ParsingCommand( string.LockBuffer(), g_pPlayer );
			string.UnlockBuffer();
		}
		else 
		{
			int nIndex = pListBox->GetCurSel();
			if( nIndex != -1 ) 	
			{
				pItemProp = (ItemProp*)pListBox->GetItemDataPtr( nIndex );
				if( pItemProp )	
				{
					string.Format( "/ci \"%s\" %d", pItemProp->szName, dwNum);
					ParsingCommand( string.LockBuffer(), g_pPlayer );
					string.UnlockBuffer();
				}
			}
		}
	//	nID = WTBID_CLOSE;
	}
	else
	if( nID == WIDC_CANCEL || nID == WTBID_CLOSE )
	{
		nID = WTBID_CLOSE;
		Destroy( TRUE );
		return TRUE;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

