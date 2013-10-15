#include "stdafx.h"
#include "resData.h"
#include "WndBagEx.h"

#include "WndManager.h"
#include "DPClient.h"
#include "defineText.h"
#include "AppDefine.h"




extern	CDPClient	g_DPlay;

/****************************************************
  WndId : APP_BAG_EX - 휴대 가방
  CtrlId : WIDC_EXBAG1 - 
  CtrlId : WIDC_EXBAG2 - 
  CtrlId : WIDC_TABCTRL3 - TabCtrl
  CtrlId : WIDC_BASIC - 
  CtrlId : WIDC_TABCTRL2 - TabCtrl
  CtrlId : WIDC_ICO_BAG1 - 
  CtrlId : WIDC_ICO_BAG2 - 
****************************************************/
#if __VER >= 11 // __SYS_POCKET

CWndBagEx::CWndBagEx() 
{ 
	m_bUse[0] = FALSE;
	m_bUse[1] = FALSE;
	m_pTexNouse		= NULL;
	m_pTexIco		= NULL;;
	m_pTexIco_empty = NULL;
} 

CWndBagEx::~CWndBagEx() 
{ 
} 

void CWndBagEx::InitItem( void )
{
	if(g_pPlayer->m_Pocket.IsAvailable(0)) 
	{
		m_wndItemCtrl[0].InitItem( g_pPlayer->m_Pocket.GetPocket(0), APP_CANCEL_BLESSING );
	}
	// 가방이 활성화 돼있는지 확인
	if(g_pPlayer->m_Pocket.IsAvailable(1)) 
	{
		m_bUse[0] = TRUE;
		m_wndItemCtrl[1].InitItem( g_pPlayer->m_Pocket.GetPocket(1), APP_CANCEL_BLESSING );
	}
	else								   
		m_bUse[0] = FALSE;
	if(g_pPlayer->m_Pocket.IsAvailable(2)) 
	{
		m_bUse[1] = TRUE;
		m_wndItemCtrl[2].InitItem( g_pPlayer->m_Pocket.GetPocket(2), APP_CANCEL_BLESSING );
	}
	else								   
		m_bUse[1] = FALSE;
}

void CWndBagEx::OnDraw( C2DRender* p2DRender ) 
{ 
	
	int     nBag[2] = { WIDC_CUSTOM4, WIDC_CUSTOM5};
	int     nIco[2] = { WIDC_ICO_BAG1, WIDC_ICO_BAG2};
	if(g_pPlayer)
	{
		/*
		if(g_pPlayer->m_Pocket.IsAvailable(1)) 
		{
			m_bUse[0] = TRUE;
		}
		else								   
			m_bUse[0] = FALSE;
		if(g_pPlayer->m_Pocket.IsAvailable(2)) 
		{
			m_bUse[1] = TRUE;
		}
		else								   
			m_bUse[1] = FALSE;
		if(g_pPlayer->m_Pocket.IsAvailable(0)) 
		{
			m_wndItemCtrl[0].InitItem( g_pPlayer->m_Pocket.GetPocket(0), APP_CANCEL_BLESSING );
		}
		// 가방이 활성화 돼있는지 확인
		if(g_pPlayer->m_Pocket.IsAvailable(1)) 
		{
			m_bUse[0] = TRUE;
			m_wndItemCtrl[1].InitItem( g_pPlayer->m_Pocket.GetPocket(1), APP_CANCEL_BLESSING );
		}
		else								   
			m_bUse[0] = FALSE;
		if(g_pPlayer->m_Pocket.IsAvailable(2)) 
		{
			m_bUse[1] = TRUE;
			m_wndItemCtrl[2].InitItem( g_pPlayer->m_Pocket.GetPocket(2), APP_CANCEL_BLESSING );
		}
		else								   
			m_bUse[1] = FALSE;
		*/
	}
	else 
		return;

	for( int i = 1 ; i < 3 ; ++i )
	{
		if( m_bUse[i - 1] == FALSE )
		{
			LPWNDCTRL lpWndCtrl = GetWndCtrl( nBag[i - 1] );
			if(m_pTexNouse) m_pTexNouse->Render( p2DRender, lpWndCtrl->rect.TopLeft(), 150 );
			lpWndCtrl = GetWndCtrl( nIco[i - 1] );
			if(m_pTexIco_empty) m_pTexIco_empty->Render( p2DRender, lpWndCtrl->rect.TopLeft(), 200 );
		}
		else
		{
			LPWNDCTRL lpWndCtrl = GetWndCtrl( nIco[i - 1] );
			if(m_pTexIco) m_pTexIco->Render( p2DRender, lpWndCtrl->rect.TopLeft(), 200 );
		}
	}
} 
void CWndBagEx::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	CWndTabCtrl* pTab0 = (CWndTabCtrl*) GetDlgItem( WIDC_BASIC );
	CWndTabCtrl* pTab1 = (CWndTabCtrl*) GetDlgItem( WIDC_EXBAG1 );
	CWndTabCtrl* pTab2 = (CWndTabCtrl*) GetDlgItem( WIDC_EXBAG2 );
	
	//pTab0->SetVisible(false);
	//pTab1->SetVisible(false);
	//pTab2->SetVisible(false);

	if( g_WndMng.GetApplet( APP_TRADE ) || g_WndMng.GetWndBase( APP_POST_READ ) || g_WndMng.m_pWndShop || g_WndMng.m_pWndGuildBank || g_WndMng.GetWndVendorBase() )
	{
		Destroy();
		return;
	}

	// 가방이 활성화 돼있는지 확인
	if(g_pPlayer->m_Pocket.IsAvailable(1)) m_bUse[0] = TRUE;
	else								   m_bUse[0] = FALSE;
	if(g_pPlayer->m_Pocket.IsAvailable(2)) m_bUse[1] = TRUE;
	else								   m_bUse[1] = FALSE;

	m_wndItemCtrl[0].Create( WLVS_ICON, CRect( 0, 0, 250, 250 ), pTab0, 11 );
	m_wndItemCtrl[0].InitItem( g_pPlayer->m_Pocket.GetPocket(0), APP_CANCEL_BLESSING );

	m_wndItemCtrl[1].Create( WLVS_ICON, CRect( 0, 0, 250, 250 ), pTab1, 11 );
	m_wndItemCtrl[1].InitItem( g_pPlayer->m_Pocket.GetPocket(1), APP_CANCEL_BLESSING );
	
	m_wndItemCtrl[2].Create( WLVS_ICON, CRect( 0, 0, 250, 250 ), pTab2, 11 );
	m_wndItemCtrl[2].InitItem( g_pPlayer->m_Pocket.GetPocket(2), APP_CANCEL_BLESSING );

	WTCITEM tabTabItem;
	
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_GAME_ITEM);//"아이템";
	tabTabItem.pWndBase = &m_wndItemCtrl[0];
	pTab0->InsertItem( 0, &tabTabItem );

	tabTabItem.pWndBase = &m_wndItemCtrl[1];
	pTab1->InsertItem( 0, &tabTabItem );

	tabTabItem.pWndBase = &m_wndItemCtrl[2];
	pTab2->InsertItem( 0, &tabTabItem );

	m_RectItemCtrl[0] = pTab0->m_rectClient;
	m_RectItemCtrl[1] = pTab1->m_rectClient;
	m_RectItemCtrl[2] = pTab2->m_rectClient;

	m_pTexNouse = m_textureMng.AddTexture( m_pApp->m_pd3dDevice,  MakePath( DIR_THEME, "InventoryBagEff.tga" ), 0xffff00ff );
	m_pTexIco	= m_textureMng.AddTexture( m_pApp->m_pd3dDevice,  MakePath( DIR_THEME, "Itm_SysSysScrBagBag01.bmp" ), 0xffff00ff );;
	m_pTexIco_empty = m_textureMng.AddTexture( m_pApp->m_pd3dDevice,  MakePath( DIR_THEME, "Itm_SysSysScrBagBag02.bmp" ), 0xffff00ff );;

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndBagEx::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_BAG_EX, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndBagEx::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndBagEx::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndBagEx::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndBagEx::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndBagEx::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndBagEx::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	LPWNDCTRL	lpWndCtrl 	= GetWndCtrl( WIDC_ICO_BAG1 );
	CRect		DrawRect	= lpWndCtrl->rect;

	CItemElem* pTempElem;
	pTempElem	= (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
	if(!pTempElem)
		return FALSE;
	if(pShortcut->m_pFromWnd->m_pParentWnd->m_pParentWnd->GetWndId() != APP_INVENTORY)
		return FALSE;
	ItemProp* pItemProp	= pTempElem->GetProp();
	if( !pItemProp )
		return FALSE;

	// 가방1 
	if( DrawRect.PtInRect( point ) )
	{
		if( pItemProp->dwItemKind3 == IK3_POCKET )
		{
			if(m_bUse[0]) 
			{
				// 가방이 이미 사용중이면 안내메시지 처리
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_GAME_ALREADY_USE) ) );
				return FALSE;
			}
			else
			{
				// 휴대가방 사용권이면 
				g_DPlay.SendAvailPocket(1, pTempElem->m_dwObjId);
			}	return TRUE;
		}
		
	}
	
	lpWndCtrl = GetWndCtrl( WIDC_ICO_BAG2 );
	DrawRect  = lpWndCtrl->rect;
	
	// 가방2
	if( DrawRect.PtInRect( point ) )
	{
		if( pItemProp->dwItemKind3 == IK3_POCKET )
		{
			if(m_bUse[1]) 
			{
				// 가방이 이미 사용중이면 안내메시지 처리
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_GAME_ALREADY_USE) ) );
				return FALSE;
			}
			else
			{
				// 휴대가방 사용권이면 
				g_DPlay.SendAvailPocket(2, pTempElem->m_dwObjId);
				return TRUE;
			}
		}
	}

	return FALSE;
}


void CWndBagEx::OnMouseWndSurface( CPoint point )
{

	LPWNDCTRL	lpWndCtrl 	= GetWndCtrl( WIDC_ICO_BAG1 );		//가방아이콘 1
	CRect		DrawRect	= lpWndCtrl->rect;

	// 가방1 
	if( DrawRect.PtInRect( point ) )
	{
		CPoint point2 = point;
		CString strText;

		ClientToScreen( &point2 );
		ClientToScreen( &DrawRect );
		
		if(m_bUse[0])
		{
			// 사용하면 시간을 보여준다
			time_t nRemain = (g_pPlayer->m_Pocket.GetPocket(1))->GetExpirationDate() - time(NULL);
			/*
			int nHour = (int)(nRemain / 3600);
			int nMin  = (int)((nRemain % 3600) / 60);
			if(nHour < 0) nHour = 0;
			if(nMin < 0) nMin = 0;
			strText.Format(prj.GetText(TID_TOOLTIP_BAG_TIME), nHour, nMin);
			*/
			CTimeSpan time(nRemain);
			strText.Format(prj.GetText(TID_TOOLTIP_BAG_TIME), static_cast<int>(time.GetDays()), time.GetHours(), time.GetMinutes());
		}
		else
		{
			// 사용하지 않으면 안내문구로
			strText = prj.GetText(TID_TOOLTIP_BAG_INFO);
		}
		g_toolTip.PutToolTip( 100000, strText, DrawRect, point2 );

	}

	lpWndCtrl = GetWndCtrl( WIDC_ICO_BAG2 );
	DrawRect  = lpWndCtrl->rect;

	// 가방2
	if( DrawRect.PtInRect( point ) )
	{
		CPoint point2 = point;
		CString strText;

		ClientToScreen( &point2 );
		ClientToScreen( &DrawRect );
		if(m_bUse[1])
		{
			// 사용하면 시간을 보여준다
			time_t nRemain = (g_pPlayer->m_Pocket.GetPocket(2))->GetExpirationDate() - time(NULL);
			/*
			int nHour = (int)(nRemain / 3600);
			int nMin  = (int)((nRemain % 3600) / 60);
			if(nHour < 0) nHour = 0;
			if(nMin < 0) nMin = 0;
			strText.Format(prj.GetText(TID_TOOLTIP_BAG_TIME), nHour, nMin);
			*/
			CTimeSpan time(nRemain);
			strText.Format(prj.GetText(TID_TOOLTIP_BAG_TIME), static_cast<int>(time.GetDays()), time.GetHours(), time.GetMinutes());
		}
		else
		{
			// 사용하지 않으면 안내문구로
			strText = prj.GetText(TID_TOOLTIP_BAG_INFO);
		}
		g_toolTip.PutToolTip( 100000, strText, DrawRect, point2 );

	}
}

BOOL CWndBagEx::process()
{

	return TRUE;
}


BOOL CWndBagEx::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( message == WIN_ITEMDROP )
	{
		LPSHORTCUT lpShortcut = (LPSHORTCUT)pLResult;
		CWndBase* pWndFrame = lpShortcut->m_pFromWnd->GetFrameWnd();
		CWndBase* pWndTarget = pWndFrame->m_pWndOnMouseMove;
		CWndBase* pWndPut = pWndFrame->m_pCurFocus;
		BYTE nSlot;
		if( pWndTarget == &(m_wndItemCtrl[0]) )
		{
			nSlot = 0;
		}
		else if( pWndTarget == &(m_wndItemCtrl[1]) )
		{
			nSlot = 1;
			if(!m_bUse[0]) return FALSE;
		}
		else
		{
			nSlot = 2;
			if(!m_bUse[1]) return FALSE;
		}
		
		BOOL bForbid = TRUE;
		if( nID == 11 ) // item
		{
			if( pWndFrame->GetWndId() == APP_INVENTORY )
			{
				SAFE_DELETE( g_WndMng.m_pWndTradeGold );
				if( lpShortcut->m_dwData != 0 )
				{
					CWndItemCtrl* pWndItemCtrl = (CWndItemCtrl*)lpShortcut->m_pFromWnd;
					UINT SelectCount = pWndItemCtrl->GetSelectedCount();
					if( SelectCount != 1)
					{
						//g_WndMng.PutString( "장착 되어 있는것은 넣을수 없습니다", NULL, 0xffffff00 );
						g_WndMng.PutString( prj.GetText(TID_GAME_EQUIPPUT), NULL, prj.GetTextColor( TID_GAME_EQUIPPUT ) );
						
					}
					else
					{
						for( int i = 0; i < (int)( SelectCount ); i++ )
						{
							int nItem = pWndItemCtrl->GetSelectedItem( i );
							pWndItemCtrl->GetItem( nItem );
						}
						CItemElem* itemElem = (CItemElem*)lpShortcut->m_dwData;
						if( itemElem->GetProp()->dwItemKind3 == IK3_QUEST )
						{
							// 퀘스트 아이템은 휴대 가방으로 옮길 수 없습니다.
							g_WndMng.PutString( prj.GetText( TID_GAME_ERROR_DONT_MOVE_QUEST_ITEM_TO_BAG_EX ), NULL, prj.GetTextColor( TID_GAME_ERROR_DONT_MOVE_QUEST_ITEM_TO_BAG_EX ) );
						}
						else
						{
							if( itemElem->m_nItemNum > 1 )
							{
								if( itemElem->GetExtra() == 0 )
								{
									g_WndMng.m_pWndTradeGold = new CWndTradeGold;
									memcpy( &g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT) );
									g_WndMng.m_pWndTradeGold->m_dwGold = itemElem->m_nItemNum;
									g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_BAG_EX;
									g_WndMng.m_pWndTradeGold->m_pWndBase = this;
									g_WndMng.m_pWndTradeGold->m_nSlot = nSlot;
									g_WndMng.m_pWndTradeGold->m_nPutSlot = -1;
									g_WndMng.m_pWndTradeGold->Initialize( NULL, APP_TRADE_GOLD );
									g_WndMng.m_pWndTradeGold->MoveParentCenter();
									CWndStatic* pStatic	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC );
									CWndStatic* pStaticCount	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_CONTROL1 );
									CString strMain = prj.GetText(TID_GAME_MOVECOUNT);//"몇개를 이동하시겠습니까?";
									CString strCount = prj.GetText(TID_GAME_NUMCOUNT);// " 갯수 : ";
									pStatic->m_strTitle = strMain;
									pStaticCount->m_strTitle = strCount;
								}
							}
							else
							{
								g_DPlay.SendMoveItem_Pocket( -1, lpShortcut->m_dwId, 1 , nSlot);
							}
						}
					}
				}
			}
			else if( pWndFrame->GetWndId() == APP_BAG_EX )
			{
				BYTE nPutSlot;
			
				SAFE_DELETE( g_WndMng.m_pWndTradeGold );
				if( lpShortcut->m_dwData != 0 )
				{
					if( pWndPut == &(m_wndItemCtrl[0]) )
					{
						nPutSlot = 0;
					}
					else if( pWndPut == &(m_wndItemCtrl[1]) )
					{
						nPutSlot = 1;
						if(!m_bUse[0]) return FALSE;
					}
					else
					{
						nPutSlot = 2;
						if(!m_bUse[1]) return FALSE;
					}

					CWndItemCtrl* pWndItemCtrl = (CWndItemCtrl*)lpShortcut->m_pFromWnd;
					UINT SelectCount = pWndItemCtrl->GetSelectedCount();
					if( SelectCount != 1)
					{
						//g_WndMng.PutString( "장착 되어 있는것은 넣을수 없습니다", NULL, 0xffffff00 );
						g_WndMng.PutString( prj.GetText(TID_GAME_EQUIPPUT), NULL, prj.GetTextColor( TID_GAME_EQUIPPUT ) );
						
					}
					else
					{
						for( int i = 0; i < (int)( SelectCount ); i++ )
						{
							int nItem = pWndItemCtrl->GetSelectedItem( i );
							pWndItemCtrl->GetItem( nItem );
						}
						CItemElem* itemElem = (CItemElem*)lpShortcut->m_dwData;
						if( itemElem->m_nItemNum > 1 )
						{ 
							g_WndMng.m_pWndTradeGold = new CWndTradeGold;
							memcpy( &g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT) );
							g_WndMng.m_pWndTradeGold->m_dwGold = itemElem->m_nItemNum;
							g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_BAG_EX;
							g_WndMng.m_pWndTradeGold->m_pWndBase = this;
							g_WndMng.m_pWndTradeGold->m_nSlot = nSlot;
							g_WndMng.m_pWndTradeGold->m_nPutSlot = nPutSlot;
							
							g_WndMng.m_pWndTradeGold->Initialize( NULL, APP_TRADE_GOLD );
							g_WndMng.m_pWndTradeGold->MoveParentCenter();
							CWndStatic* pStatic	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC );
							CWndStatic* pStaticCount	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_CONTROL1 );
							CString strMain = prj.GetText(TID_GAME_MOVECOUNT);//"몇개를 이동하시겠습니까?";
							CString strCount = prj.GetText(TID_GAME_NUMCOUNT);// " 갯수 : ";
							pStatic->m_strTitle = strMain;
							pStaticCount->m_strTitle = strCount;
						}
						else
						{
							g_DPlay.SendMoveItem_Pocket( nPutSlot, lpShortcut->m_dwId, 1 , nSlot);
						}
						
					}
				}
			}
		}
	}
	if( message == WNM_CLICKED )
	{
		switch( nID )
		{
			case 100: // icon
//				m_wndItemCtrl.SetWndRect( m_wndItemCtrl.GetWindowRect( TRUE ) );
				break;
			case WTBID_REPORT:
				break;
			case WTBID_CLOSE: // 종료
				break;
		}
	}	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

#endif