#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndRepairItem.h"
#include "dpclient.h"
extern	CDPClient	g_DPlay;
#include "WndManager.h"


/****************************************************
  WndId : APP_REPAIR - 수리창
  CtrlId : WIDC_EDIT1 - 
  CtrlId : WIDC_STATIC1 - 총수리가격
  CtrlId : WIDC_EDIT2 - 
  CtrlId : WIDC_OK - 수리확인
  CtrlId : WIDC_CANCEL - 수리취소
  CtrlId : WIDC_STATIC2 - 수리목록
****************************************************/

CWndRepairItem::CWndRepairItem() 
{
	SetPutRegInfo( FALSE );
}

CWndRepairItem::~CWndRepairItem() 
{
	for( int i = 0; i < MAX_REPAIRINGITEM; i++ )
	{
		if( m_adwIdRepair[i] != NULL_ID )
		{
			CItemElem* pItemElem	= g_pPlayer->m_Inventory.GetAtId( m_adwIdRepair[i] );
			if( pItemElem )
			{
				pItemElem->m_bRepair	= FALSE;
			}
		}
	}
}

void CWndRepairItem::OnDraw( C2DRender* p2DRender ) 
{ 
}
 
void CWndRepairItem::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	memset( m_adwIdRepair, 0xff, sizeof(m_adwIdRepair) );
	m_dwCost = 0;

//	CWndTabCtrl* pTabCtrl = (CWndTabCtrl*) GetDlgItem( WIDC_INVENTORY );

	CWndEdit* pWndEdit = (CWndEdit*) GetDlgItem( WIDC_EDIT1 );
	pWndEdit->EnableWindow( FALSE );

	pWndStaticCost = (CWndStatic*) GetDlgItem( WIDC_STATIC3 );

	m_wndItemCtrl.Create( WLVS_ICON | WBS_NODRAWFRAME, CRect( 6, 24, 174, 192 ), this, 11 );
	m_wndItemCtrl.InitItem( m_adwIdRepair );

//	pRepairItem.SetItemContainer( ITYPE_ITEM, MAX_REPAIR );
//	m_wndItemCtrl.InitItem( &pRepairItem, APP_REPAIR );
//	WTCITEM tabTabItem;
//	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
//	tabTabItem.pszText = "아이템";
//	tabTabItem.pWndBase = &m_wndItemCtrl;
//	pTabCtrl->InsertItem( 0, &tabTabItem );

	CWndInventory* pWndInventory = (CWndInventory*)g_WndMng.CreateApplet( APP_INVENTORY );
	CRect rectInventory = pWndInventory->GetWindowRect( TRUE );
	CPoint ptInventory = rectInventory.TopLeft();
	CPoint ptMove;
	
	if( ptInventory.x > m_pWndRoot->GetWndRect().Width() / 2 )
		ptMove = ptInventory - CPoint( rectInventory.Width(), 0 );
	else
		ptMove = ptInventory + CPoint( rectInventory.Width(), 0 );
	Move( ptMove );
/*		CRect rectRoot = m_pWndRoot->GetLayoutRect();
		CRect rectWindow = GetWindowRect();
		CPoint point( rectRoot.right - rectWindow.Width(), 110 );
		Move( point );
		MoveParentCenter();
*/
} 

BOOL CWndRepairItem::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_REPAIR, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndRepairItem::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndRepairItem::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
}

void CWndRepairItem::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
}
 
void CWndRepairItem::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
}

BOOL CWndRepairItem::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( message == WIN_ITEMDROP )
	{
		LPSHORTCUT lpShortcut = (LPSHORTCUT)pLResult;
		CWndBase* pWndFrame = lpShortcut->m_pFromWnd->GetFrameWnd();
		if( nID == 11 ) // item
		{
			if( pWndFrame->GetWndId() == APP_INVENTORY )
			{
				CItemBase* pItemBase	= g_pPlayer->GetItemId( lpShortcut->m_dwId );
				if( pItemBase )
				{
					if( g_pPlayer->m_Inventory.IsEquip( pItemBase->m_dwObjId ) == FALSE )
					{
						// 아이템인가?
						// 방어구 혹은 무기인가?
						// 수리할 필요가 있는가?
						ItemProp* pItemProp	= pItemBase->GetProp();
						if( pItemProp )
						{
							if( pItemProp->dwItemKind2 >= IK2_WEAPON_HAND && pItemProp->dwItemKind2 <= IK2_ARMORETC && ((CItemElem*)pItemBase)->m_nRepair < pItemProp->nMaxRepair )
							{
								CItemElem* pItemElem	= (CItemElem*)pItemBase;
								if( pItemElem->m_nHitPoint < 0 )
									pItemElem->m_nHitPoint = 0;

								if( pItemElem->m_nHitPoint < (int)( pItemProp->dwEndurance ) )
								{
									int nRepair	= 100 - ( ( pItemElem->m_nHitPoint * 100 ) / pItemProp->dwEndurance );
									DWORD dwSumCost = m_dwCost + nRepair * ( pItemProp->dwCost / 1000 + 1 );
									if( g_pPlayer->GetGold() >= (int)( dwSumCost ) )
									{
										m_dwCost = dwSumCost;
										char szCost[MAX_PATH] = {0,};
										sprintf( szCost, "%-d", m_dwCost );
										pWndStaticCost->SetTitle( szCost );
										int iIndex	= lpShortcut->m_dwData - 100;
										m_adwIdRepair[iIndex]	= pItemElem->m_dwObjId;
										pItemElem->m_bRepair	= TRUE;
									}
									else
									{
										// 페냐가 부족하여 수리를 할수 없습니다.
										g_WndMng.PutString( prj.GetText(TID_GAME_REPAIR_NOTENOUGHPENYA), NULL, prj.GetTextColor( TID_GAME_REPAIR_NOTENOUGHPENYA ) );
									}									
								}
								else
								{
									// 수리할 필요가 없는 아이템
									g_WndMng.PutString( prj.GetText(TID_GAME_REPAIR_FULLENDURANCE), NULL, prj.GetTextColor( TID_GAME_REPAIR_FULLENDURANCE ) );
								}
							}
							else
							{
								// 수리할수 없는 아이템
								g_WndMng.PutString( prj.GetText(TID_GAME_REPAIR_DONOT), NULL, prj.GetTextColor( TID_GAME_REPAIR_DONOT ) );
							}
						}
					}
					else
					{
						// 장착되어 있음.
						g_WndMng.PutString( prj.GetText(TID_GAME_REPAIR_EQUIP), NULL, prj.GetTextColor( TID_GAME_REPAIR_EQUIP ) );
					}
				}
//				if( itemElem )
//				{
//					pRepairItem.Add( itemElem->m_dwItemId, itemElem->m_nItemNum, itemElem->m_nAbilityOption, itemElem->m_dwSerialNumber );
//				}

				/*
				CWndItemCtrl* pWndItemCtrl = (CWndItemCtrl*)lpShortcut->m_pFromWnd;
				UINT SelectCount = pWndItemCtrl->GetSelectedCount();
				if( SelectCount != 1)
				{
					//g_WndMng.PutString( "장착 되어 있는것은 넣을수 없습니다", NULL, 0xffffff00 );
					g_WndMng.PutString( prj.GetText(TID_GAME_EQUIPPUT), NULL, prj.GetTextColor( TID_GAME_EQUIPPUT ) );
					
				}
				else
				{
					for( int i = 0; i < SelectCount; i++ )
					{
						int nItem = pWndItemCtrl->GetSelectedItem( i );
						pWndItemCtrl->GetItem( nItem );
					}
					CItemElem* itemElem = (CItemElem*)lpShortcut->m_dwData;
					if( itemElem )
					{
						pRepairItem.Add( itemElem->m_dwItemId, itemElem->m_nItemNum, itemElem->m_nAbilityOption, itemElem->m_dwSerialNumber );
					}
				}
				int aaa = 0;				
				*/
			}
		}
	}
	else
	if( message == WNM_CLICKED )
	{
		if( nID == WIDC_OK )
		{
			g_DPlay.SendRepairItem( m_adwIdRepair );
		}
		else if( nID == WIDC_CANCEL || nID == WTBID_CLOSE )
		{
			OnInit();
			nID = WTBID_CLOSE;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndRepairItem::OnDestroy( void )
{
	OnInit();
}

void CWndRepairItem::OnInit( void )
{
	for( int i = 0; i < MAX_REPAIRINGITEM; i++ )
	{
		if( m_adwIdRepair[i] != NULL_ID )
		{
			CItemElem* pItemElem	= g_pPlayer->m_Inventory.GetAtId( m_adwIdRepair[i] );
			if( pItemElem )
			{
				pItemElem->m_bRepair	= FALSE;
			}
		}
	}
}

