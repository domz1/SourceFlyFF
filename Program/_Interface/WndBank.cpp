#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndBank.h"

#include "WndManager.h"
#include "DPClient.h"
extern	CDPClient	g_DPlay;


/****************************************************
  WndId : APP_BANK - BANK
  CtrlId : WIDC_INVENTORY - 
  CtrlId : WIDC_COST - 0
****************************************************/

CWndBank::CWndBank() 
{ 
} 
CWndBank::~CWndBank() 
{
	g_DPlay.SendCloseBankWnd();
	SAFE_DELETE( g_WndMng.m_pWndTradeGold );
} 

void CWndBank::OnDraw( C2DRender* p2DRender ) 
{ 
	int     nIndex[3] = { WIDC_CUSTOM4, WIDC_CUSTOM5, WIDC_CUSTOM6 };
	CString str;
	
	for( int i = 0 ; i < 3 ; ++i )
	{
		str.Format( _T( "%d" ),g_pPlayer->m_dwGoldBank[i] );
		pCost[i]->SetTitle( str );	
		
		if( bUse[i] == FALSE )
		{
			LPWNDCTRL lpWndCtrl = GetWndCtrl( nIndex[i] );
			m_pTexture->Render( p2DRender, lpWndCtrl->rect.TopLeft(), 50 );
		}
	}
	
	
	
} 

void CWndBank::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
#if __VER >= 11 // __SYS_POCKET
	if(GetWndBase( APP_BAG_EX )) GetWndBase( APP_BAG_EX )->Destroy();
#endif
	if( g_WndMng.m_pWndTrade || g_WndMng.m_pWndShop || g_WndMng.m_pWndGuildBank || g_WndMng.GetWndVendorBase() )
	{
		Destroy();
		return;
	}

	CWndTabCtrl* pTabCtrl = (CWndTabCtrl*) GetDlgItem( WIDC_BANK_0 );
	
	m_wndItemCtrl[0].Create( WLVS_ICON, CRect( 0, 0, 250, 250 ), pTabCtrl, 11 );
	m_wndItemCtrl[0].InitItem( &g_pPlayer->m_Bank[0], APP_COMMON_BANK );

	CWndTabCtrl* pTabCtrl1 = (CWndTabCtrl*) GetDlgItem( WIDC_BANK_1 );

	m_wndItemCtrl[1].Create( WLVS_ICON, CRect( 0, 0, 250, 250 ), pTabCtrl1, 11 );
	m_wndItemCtrl[1].InitItem( &g_pPlayer->m_Bank[1], APP_COMMON_BANK );

	CWndTabCtrl* pTabCtrl2 = (CWndTabCtrl*) GetDlgItem( WIDC_BANK_3 );
	
	m_wndItemCtrl[2].Create( WLVS_ICON, CRect( 0, 0, 250, 250 ), pTabCtrl2, 11 );
	m_wndItemCtrl[2].InitItem( &g_pPlayer->m_Bank[2], APP_COMMON_BANK );

	WTCITEM tabTabItem;
	
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_GAME_ITEM);//"아이템";
	tabTabItem.pWndBase = &m_wndItemCtrl[0];
	pTabCtrl->InsertItem( 0, &tabTabItem );

	tabTabItem.pWndBase = &m_wndItemCtrl[1];
	pTabCtrl1->InsertItem( 0, &tabTabItem );

	
	tabTabItem.pWndBase = &m_wndItemCtrl[2];
	pTabCtrl2->InsertItem( 0, &tabTabItem );
	

	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_BANKGO_0 );
	m_wndGold[0].Create( "g", WBS_NODRAWFRAME, lpWndCtrl->rect, this, WIDC_BANKGO_0 );
	m_wndGold[0].AddWndStyle( WBS_NODRAWFRAME );

	lpWndCtrl = GetWndCtrl( WIDC_BANKGO_1 );
	m_wndGold[1].Create( "g", WBS_NODRAWFRAME, lpWndCtrl->rect, this, WIDC_BANKGO_1 );
	m_wndGold[1].AddWndStyle( WBS_NODRAWFRAME );

	lpWndCtrl = GetWndCtrl( WIDC_BANKGO_2 );
	m_wndGold[2].Create( "g", WBS_NODRAWFRAME, lpWndCtrl->rect, this, WIDC_BANKGO_2 );
	m_wndGold[2].AddWndStyle( WBS_NODRAWFRAME );

	pCost[0] = (CWndStatic*) GetDlgItem( WIDC_BANKGOLD_0 );
	pCost[1] = (CWndStatic*) GetDlgItem( WIDC_BANKGOLD_1 );
	pCost[2] = (CWndStatic*) GetDlgItem( WIDC_BANKGOLD_2 );

	ReSetBank();
	
	RectItemCtrl[0] = pTabCtrl->m_rectClient + CRect( -3, 2, -18, -20 );
	RectItemCtrl[1] = pTabCtrl1->m_rectClient + CRect( -3, 2, -19, -20 );
	RectItemCtrl[2] = pTabCtrl2->m_rectClient + CRect( -3, 2, -20, -20 );

	CWndInventory* pWndInventory = (CWndInventory*)GetWndBase( APP_INVENTORY );
	CRect rectInventory = pWndInventory->GetWindowRect( TRUE );
	CPoint ptInventory = rectInventory.TopLeft();
	CPoint ptMove;

	if( ptInventory.x > m_pWndRoot->GetWndRect().Width() / 2 )
		ptMove = ptInventory - CPoint( (int)( rectInventory.Width() * 2.525 ), 0 );
	else
		ptMove = ptInventory + CPoint( rectInventory.Width(), 0 );
	Move( ptMove );

	m_pTexture = m_textureMng.AddTexture( m_pApp->m_pd3dDevice,  MakePath( DIR_THEME, "WndCommonBankNotUse.tga" ), 0xffff00ff );
} 

void CWndBank::ReSetBank( )
{
	for( int i = 0 ; i < 3 ; ++i )
	{
		if( g_pPlayer->m_nSlot == i )
		{
			bUse[i] = TRUE;
		}
		else
		if( g_pPlayer->m_idPlayerBank[i] == 0 || !g_pPlayer->IsCommBank() )
		{
			m_wndItemCtrl[i].SetScrollBarVisible( FALSE );
			m_wndItemCtrl[i].EnableWindow( FALSE );
			m_wndGold[i].EnableWindow( FALSE );
			bUse[i] = FALSE;
		}
		else
		{
			m_wndItemCtrl[i].SetScrollBarVisible( TRUE );
			m_wndItemCtrl[i].EnableWindow( TRUE );
			m_wndGold[i].EnableWindow( TRUE );
			bUse[i] = TRUE;
		}
		RectGold[i] = pCost[i]->m_rectClient;
	}	
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndBank::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	if( g_pPlayer == NULL )
		return FALSE;
#if __VER >= 12 // __MOD_TUTORIAL
	CWndGuideSystem* pWndGuide = (CWndGuideSystem*)GetWndBase( APP_GUIDE );
	if(pWndGuide && pWndGuide->IsVisible()) 
	{
		if(pWndGuide->m_CurrentGuide.m_nVicCondition == OPEN_WINDOW && pWndGuide->m_CurrentGuide.m_nInput == APP_COMMON_BANK)
			pWndGuide->m_Condition.nOpenedWindowID = 1;
	}
#endif
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_COMMON_BANK, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndBank::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndBank::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndBank::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndBank::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndBank::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	// 인벤토리에서 Bank로 아이템이 Drop되었다. 어디서 왔는지 확인한후 적절히 대응하라.
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
		}
		else
		{
			nSlot = 2;
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
						if( itemElem->m_nItemNum > 1 )
						{ 
							g_WndMng.m_pWndTradeGold = new CWndTradeGold;
							memcpy( &g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT) );
							g_WndMng.m_pWndTradeGold->m_dwGold = itemElem->m_nItemNum;
							g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_BANK;
							g_WndMng.m_pWndTradeGold->m_pWndBase = this;
							g_WndMng.m_pWndTradeGold->m_nSlot = nSlot;
							
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
							g_DPlay.SendPutItemBank( nSlot, (BYTE)( lpShortcut->m_dwId ), 1 );
						}
						
					}
				}
				else
				{
					// 페냐 (돈)
					g_WndMng.m_pWndTradeGold = new CWndTradeGold;
					memcpy( &g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT) );
					g_WndMng.m_pWndTradeGold->m_dwGold = g_pPlayer->GetGold();
					g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_BANK;
					g_WndMng.m_pWndTradeGold->m_pWndBase = this;
					g_WndMng.m_pWndTradeGold->m_nSlot = nSlot;
					
					g_WndMng.m_pWndTradeGold->Initialize( NULL, APP_TRADE_GOLD );
					g_WndMng.m_pWndTradeGold->MoveParentCenter();
					CWndStatic* pStatic	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC );
					CWndStatic* pStaticCount	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_CONTROL1 );
					//CString strMain = "얼마를 이동하시겠습니까?";
					//CString strCount = " SEED : ";
					CString strMain = prj.GetText(TID_GAME_MOVEPENYA);//"얼마를 이동하시겠습니까?";
					CString strCount = prj.GetText(TID_GAME_PENYACOUNT );// " pennya : ";
					pStatic->m_strTitle = strMain;
					pStaticCount->m_strTitle = strCount;
				}
			}
			else if( pWndFrame->GetWndId() == APP_COMMON_BANK )
			{
				BYTE nPutSolt;
			
				SAFE_DELETE( g_WndMng.m_pWndTradeGold );
				if( lpShortcut->m_dwData != 0 )
				{
					if( pWndPut == &(m_wndItemCtrl[0]) )
					{
						nPutSolt = 0;
					}
					else if( pWndPut == &(m_wndItemCtrl[1]) )
					{
						nPutSolt = 1;
					}
					else
					{
						nPutSolt = 2;
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
							g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_COMMON_BANK;
							g_WndMng.m_pWndTradeGold->m_pWndBase = this;
							g_WndMng.m_pWndTradeGold->m_nSlot = nSlot;
							g_WndMng.m_pWndTradeGold->m_nPutSlot = nPutSolt;
							
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
							g_DPlay.SendPutItemBankToBank( nPutSolt, nSlot, (BYTE)( lpShortcut->m_dwId ), 1 );
						}
						
					}
				}
				else
				{

					if( pWndPut == &(m_wndItemCtrl[0]) )
					{
						nPutSolt = 0;
					}
					else if( pWndPut == &(m_wndItemCtrl[1]) )
					{
						nPutSolt = 1;
					}
					else
					{
						nPutSolt = 2;
					}


					if( pWndPut == &(m_wndGold[0]) )
					{
						nPutSolt = 0;
					}
					else if( pWndPut == &(m_wndGold[1]) )
					{
						nPutSolt = 1;
					}
					else
					{
						nPutSolt = 2;
					}
					
					// 페냐 (돈)
					g_WndMng.m_pWndTradeGold = new CWndTradeGold;
					memcpy( &g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT) );
					g_WndMng.m_pWndTradeGold->m_dwGold = g_pPlayer->m_dwGoldBank[nPutSolt];
					g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_COMMON_BANK;
					g_WndMng.m_pWndTradeGold->m_pWndBase = this;
					g_WndMng.m_pWndTradeGold->m_nSlot = nSlot;
					g_WndMng.m_pWndTradeGold->m_nPutSlot = nPutSolt;
					
					g_WndMng.m_pWndTradeGold->Initialize( NULL, APP_TRADE_GOLD );
					g_WndMng.m_pWndTradeGold->MoveParentCenter();
					CWndStatic* pStatic	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC );
					CWndStatic* pStaticCount	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_CONTROL1 );
					//CString strMain = "얼마를 이동하시겠습니까?";
					//CString strCount = " SEED : ";
					CString strMain = prj.GetText(TID_GAME_MOVEPENYA);//"얼마를 이동하시겠습니까?";
					CString strCount = prj.GetText(TID_GAME_PENYACOUNT );// " pennya : ";
					pStatic->m_strTitle = strMain;
					pStaticCount->m_strTitle = strCount;
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
				{
//					g_DPlay.SendCloseBankWnd();
				}
				break;
		}
	}	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

//---------------------------------------------------------------------------------------------------------------------------
// 패스워드 확인창
//---------------------------------------------------------------------------------------------------------------------------
CWndConfirmBank::CWndConfirmBank() 
{ 
	m_dwId	= NULL_ID;
	m_dwItemId	= 0;
} 
CWndConfirmBank::~CWndConfirmBank() 
{ 
} 
void CWndConfirmBank::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndConfirmBank::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	CWndEdit *m_pPass   = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	if( m_pPass )
	{
		m_pPass->AddWndStyle( EBS_PASSWORD );
		m_pPass->SetFocus();
	}
	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndConfirmBank::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_CONFIRM_BANK, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndConfirmBank::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndConfirmBank::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndConfirmBank::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndConfirmBank::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndConfirmBank::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndConfirmBank::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK || message == EN_RETURN )
	{
		CWndEdit *m_pPass   = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
		if( m_pPass )
		{
			// 여기서 공백이 있는지 혹은 숫자이외의 것이 오는지 확인
			char szPass[10] ={0,};
			strncpy( szPass, m_pPass->GetString(), 10 );
			if( strlen( szPass ) == 4 )
			{
				BOOL bDigit = TRUE;
				for( int i = 0 ; i < 4 ; i++ )
				{
					if( isdigit2( szPass[i] ) == FALSE )
					{
						bDigit = FALSE;	
						break;
					}
				}
				if( bDigit )
				{ 
					g_DPlay.SendConfirmBank( szPass, m_dwId, m_dwItemId );
				} 
				else
				{
					m_pPass->SetString( "" );
					g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0025) ) );
//					g_WndMng.OpenMessageBox( "숫자 4자리로만 넣어야 합니다. 다시 입력해주세요" );
				}
			}
			else
			{
				m_pPass->SetString( "" );
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0025) ) );
//				g_WndMng.OpenMessageBox( "숫자 4자리로만 넣어야 합니다. 다시 입력해주세요" );
			}
		}
	}
	else if( nID == WIDC_CHANGE_PASS )
	{
		g_WndMng.m_pWndBankPassword = new CWndBankPassword;
		g_WndMng.m_pWndBankPassword->SetBankPassword( 1 );
		g_WndMng.m_pWndBankPassword->Initialize( NULL, APP_BANK_PASSWORD );	

		Destroy();
	}
	else if( nID == WIDC_CANCEL )
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 



//---------------------------------------------------------------------------------------------------------------------------
// 패스워드 변경창
//---------------------------------------------------------------------------------------------------------------------------
CWndBankPassword::CWndBankPassword() 
{ 
	m_nFlags = 0;
	m_dwId	= NULL_ID;
	m_dwItemId	= 0;
} 
CWndBankPassword::~CWndBankPassword() 
{ 
} 
void CWndBankPassword::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndBankPassword::SetBankPassword( int nFlags )
{
	m_nFlags = nFlags;
}
void CWndBankPassword::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	CWndEdit *m_pLastPass   = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	CWndEdit *m_pNewPass = (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
	CWndEdit *m_pConfirmPass = (CWndEdit*)GetDlgItem( WIDC_EDIT3 );

	if( m_pLastPass && m_pNewPass && m_pConfirmPass )
	{	
		m_pLastPass->AddWndStyle( EBS_PASSWORD );
		m_pNewPass->AddWndStyle( EBS_PASSWORD );
		m_pConfirmPass->AddWndStyle( EBS_PASSWORD );

		if( m_nFlags == 0 )
		{
			m_pLastPass->SetString( "0000" );
			m_pNewPass->SetFocus();
		}
		else
		{
			m_pLastPass->SetFocus();
		}
	}

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndBankPassword::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_BANK_PASSWORD, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndBankPassword::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndBankPassword::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndBankPassword::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndBankPassword::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndBankPassword::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndBankPassword::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK || message == EN_RETURN )
	{
		CWndEdit *m_pLastPass   = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
		CWndEdit *m_pNewPass = (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
		CWndEdit *m_pConfirmPass = (CWndEdit*)GetDlgItem( WIDC_EDIT3 );
		if( m_pLastPass && m_pNewPass && m_pConfirmPass )
		{
			// 여기서 공백이 있는지 혹은 숫자이외의 것이 오는지 확인
			char szLastPass[10] ={0,};
			char szNewPass[10] ={0,};
			char szConfirmPass[10] ={0,};
//			strncpy( szLastPass, m_pLastPass->GetString(), 10 );
//			strncpy( szNewPass, m_pNewPass->GetString(), 10 );
//			strncpy( szConfirmPass, m_pConfirmPass->GetString(), 10 );
			strcpy( szLastPass, m_pLastPass->GetString() );
			strcpy( szNewPass, m_pNewPass->GetString() );
			strcpy( szConfirmPass, m_pConfirmPass->GetString() );
			if( strlen( szLastPass ) == 4 && strlen( szNewPass ) == 4 && strlen( szConfirmPass ) == 4)
			{
				if( 0 == strcmp( szNewPass, szConfirmPass ) )
				{
					BOOL bDigit = TRUE;
					for( int i = 0 ; i < 4 ; i++ )
					{
						if( (isdigit2( szLastPass[i] ) == FALSE) || (isdigit2( szNewPass[i] ) == FALSE) || (isdigit2( szConfirmPass[i] ) == FALSE) )
						{
							bDigit = FALSE;
							break;
						}
					}
					if( bDigit )
					{
						g_DPlay.SendChangeBankPass( szLastPass, szNewPass, m_dwId, m_dwItemId );
					}
					else
					{
						if( m_nFlags == 0 )
						{
							m_pLastPass->SetString( "0000" );
						}
						else
						{
							m_pLastPass->SetString( "" );
						}

						m_pNewPass->SetString( "" );
						m_pConfirmPass->SetString( "" );
						g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0025) ) );
//						g_WndMng.OpenMessageBox( "숫자 4자리로만 넣어야 합니다. 다시 입력해주세요" );
					}					
				}
				else
				{
					m_pNewPass->SetString( "" );
					m_pConfirmPass->SetString( "" );
					g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0022) ) );
//					g_WndMng.OpenMessageBox( "새암호와 암호확인이 다름니다. 다시 입력해주세요" );					
				}
			}
			else
			{
				if( m_nFlags == 0 )
				{
					m_pLastPass->SetString( "0000" );
				}
				else
				{
					m_pLastPass->SetString( "" );
				}
				m_pNewPass->SetString( "" );
				m_pConfirmPass->SetString( "" );
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0025) ) );
//				g_WndMng.OpenMessageBox( "숫자 4자리로만 넣어야 합니다. 다시 입력해주세요" );
			}
		}
	}
	else if( nID == WIDC_CANCEL )
	{
		Destroy();
	}
	else if( m_nFlags == 0 && message == 0 && nID == WIDC_EDIT1 )
	{
		CWndEdit *m_pNewPass = (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
		m_pNewPass->SetFocus();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}