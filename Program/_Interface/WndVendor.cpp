#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndVendor.h"

#include "WndManager.h"

#include "dpclient.h"
extern	CDPClient	g_DPlay;

#include "Chatting.h"
extern	CChatting g_Chatting;




/*
	WndId : APP_VENDOR - 개인상점
	CtrlId : WIDC_EDIT1 - 
	CtrlId : WIDC_STATIC1 - 상점이름
	CtrlId : WIDC_EDIT2 - 
	CtrlId : WIDC_STATIC2 - 판매목록
	CtrlId : WIDC_OK - 확인
	CtrlId : WIDC_CANCEL - 취소
*/



CWndVendor::CWndVendor()
{
	SetPutRegInfo( FALSE );
	m_pVendor	= NULL;
	m_pWndRegVend	= NULL;
	m_pWndVendorBuy		= NULL;

	m_pwndVenderMessage = NULL;
		
}

CWndVendor::~CWndVendor()
{
	SAFE_DELETE( m_pWndRegVend );
	SAFE_DELETE( m_pWndVendorBuy );

	SAFE_DELETE( m_pwndVenderMessage );
	SAFE_DELETE( g_WndMng.m_pWndTradeGold );
}

void CWndVendor::SetVendor( CMover* pVendor )
{
	m_pVendor	= pVendor;
	m_wndctrlVendor.InitVendor( m_pVendor );
	if( !m_pVendor->IsActiveMover() )
	{
		CWndEdit* pWndEdit	= (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
		pWndEdit->SetString( pVendor->m_vtInfo.GetTitle() );
		pWndEdit->EnableWindow( FALSE );
		CWndButton* pButton	= (CWndButton*)GetDlgItem( WIDC_OK );
		pButton->SetVisible( FALSE );
		pButton	= (CWndButton*)GetDlgItem( WIDC_CANCEL );
		pButton->SetVisible( FALSE );
	}
	CWndWorld* pWndWorld	= (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
	if( pWndWorld )
		pWndWorld->m_objidTracking	= NULL_ID;

	if( g_WndMng.m_pWndDialog )
		Destroy();
}

void CWndVendor::OnDraw( C2DRender* p2DRender )
{

}

void CWndVendor::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate(); 

#if __VER >= 8 // __S8_VENDOR_REVISION
	SetTitle( prj.GetText( TID_GAME_VENDOR_TITLE ) );
	m_wndctrlVendor.Create( WLVS_ICON | WBS_NODRAWFRAME, CRect( 5, 60, 440, 460 ), this, 100 );
#else // __VER >= 8 // __S8_VENDOR_REVISION
	m_wndctrlVendor.Create( WLVS_ICON | WBS_NODRAWFRAME, CRect( 18, 80, 220, 250 ), this, 100 );
#endif // __VER >= 8 // __S8_VENDOR_REVISION
	
	SetVendor( g_pPlayer );
	if( g_pPlayer )
	{
		if( g_pPlayer->IsMode( COMMUNITY_MODE ) )
		{
			Destroy();
		}
	}
#if __VER >= 11 // __SYS_POCKET
	if(GetWndBase( APP_BAG_EX )) GetWndBase( APP_BAG_EX )->Destroy();
#endif
	if( g_WndMng.m_pWndTrade || g_WndMng.m_pWndShop || g_WndMng.m_pWndBank || g_WndMng.m_pWndGuildBank)
	{
		Destroy();
		return;
	}
	
#ifdef __S_SERVER_UNIFY
	if( g_WndMng.m_bAllAction == FALSE )
	{
		Destroy();
		return;
	}
#endif // __S_SERVER_UNIFY
	
#if __VER >= 8 // __S8_PK
	if( g_pPlayer->IsChaotic() )
	{
		CHAO_PROPENSITY Propensity = prj.GetPropensityPenalty( g_pPlayer->GetPKPropensity() );
		if( !Propensity.nVendor )
		{
			g_WndMng.PutString( prj.GetText( TID_GAME_CHAOTIC_NOT_VENDOR ) );
			Destroy();
			return;
		}
	}
#endif // __VER >= 8 // __S8_PK

	if( !m_pVendor->IsActiveMover() )
	{
		CWndButton* pButton	= (CWndButton*)GetDlgItem( WIDC_OK );
		pButton->SetVisible( FALSE );
		pButton	= (CWndButton*)GetDlgItem( WIDC_CANCEL );
		pButton->SetVisible( FALSE );
	}

	ReloadItemList();

	CWndInventory* pWndInventory	= (CWndInventory*)g_WndMng.CreateApplet( APP_INVENTORY );
	CRect rcInventory	= pWndInventory->GetWindowRect( TRUE );
	CRect rcVendor = GetWindowRect( TRUE );
	CPoint ptInventory	= rcInventory.TopLeft();
	CPoint point;
	if( ptInventory.x > m_pWndRoot->GetWndRect().Width() / 2 )
		point	= ptInventory - CPoint( rcVendor.Width(), 0 );
	else
		point	= ptInventory + CPoint( rcInventory.Width(), 0 );
	Move( point ); 
} 

void CWndVendor::ReloadItemList()
{
#if __VER >= 11 // __MOD_VENDOR
	// 백업해 둔 리스트를 로드한다 	
	if( !g_pPlayer->m_vtInfo.IsVendorOpen() )
	{
		for( int i = 0; i < MAX_VENDITEM; i++ )
		{
			for(int nIndex = 0;nIndex < g_pPlayer->m_Inventory.GetMax(); nIndex++)
			{
				CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAtId(nIndex);
				if(pItemElem)
				{
					if( pItemElem->m_dwObjId == 0 ) continue;
					// 100304_mirchang vendor item check
					if( g_Neuz.m_aSavedInven[i].m_dwObjId == pItemElem->m_dwObjId 
					&& g_Neuz.m_aSavedInven[i].m_dwItemId == pItemElem->m_dwItemId )
					{
						// 갯수가 다르면 갯수를 조정한다 	
						if(g_Neuz.m_aSavedInven[i].m_nExtra > pItemElem->m_nItemNum)
						{
							//g_Neuz.m_aSavedInven[i].m_nExtra = pItemElem->m_nItemNum;
							// 100304_mirchang vendor item check
							g_Neuz.m_aSavedInven[i].m_dwObjId = 0;
							g_Neuz.m_aSavedInven[i].m_nExtra = 0;
							g_Neuz.m_aSavedInven[i].m_nCost = 0;
							g_Neuz.m_aSavedInven[i].m_dwItemId = 0;
						}
						else
							g_DPlay.SendRegisterPVendorItem(i, 0, (BYTE)( g_Neuz.m_aSavedInven[i].m_dwObjId ),
															g_Neuz.m_aSavedInven[i].m_nExtra, g_Neuz.m_aSavedInven[i].m_nCost);
					}
				}
			}
		}
	}
#endif
}

BOOL CWndVendor::Process()
{
#if __VER >= 11 // __MOD_VENDOR
	// 장사를 시작하면 리셋버튼은 보이지 않게 함 
	CWndButton* pButton	= (CWndButton*)GetDlgItem( WIDC_RESET );
	if(g_pPlayer->m_vtInfo.IsVendorOpen())
		pButton->SetVisible( FALSE );
	else
		pButton->SetVisible( TRUE );
#endif
	return TRUE;
}

BOOL CWndVendor::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{
	if( g_eLocal.GetState( EVE_SCHOOL ) )
		return FALSE;

	if( g_WndMng.GetWndBase( APP_TRADE ) || g_WndMng.GetWndBase( APP_SHOP_ ) || g_WndMng.GetWndBase( APP_BANK ) || g_WndMng.GetWndBase( APP_GUILD_BANK ) )
	{
		return FALSE;
	}
#if __VER >= 8 // 8차 듀얼 061226 ma
	if( g_pPlayer->m_nDuel )
	{
		return FALSE;
	}
#endif // __VER >= 8 // 8차 듀얼 061226 ma

#if __VER >= 8 // __S8_VENDOR_REVISION
	return InitDialog( g_Neuz.GetSafeHwnd(), APP_VENDOR_REVISION, 0, 0, pWndParent );
#else // __VER >= 8 // __S8_VENDOR_REVISION
	return InitDialog( g_Neuz.GetSafeHwnd(), APP_VENDOREX, 0, 0, pWndParent );
#endif // __VER >= 8 // __S8_VENDOR_REVISION
}

BOOL CWndVendor::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

void CWndVendor::OnSize( UINT nType, int cx, int cy )
{
	CWndNeuz::OnSize( nType, cx, cy );
}

void CWndVendor::OnLButtonUp( UINT nFlags, CPoint point ) 
{

}

void CWndVendor::OnLButtonDown( UINT nFlags, CPoint point ) 
{

}

BOOL CWndVendor::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( message == 100 && nID == 100 )
	{
		LPSHORTCUT pShortcut	= (LPSHORTCUT)pLResult;
		if( pShortcut )
		{
			CWndBase* pWndFrame		= pShortcut->m_pFromWnd->GetFrameWnd();
			if( pWndFrame->GetWndId() == APP_INVENTORY )
			{
				CItemBase* pItemBase	= m_pVendor->GetItemId( pShortcut->m_dwId );
				if( pItemBase )
				{
					if( pItemBase->IsQuest() )
						return FALSE;

					if( pItemBase->GetProp()->dwItemKind3 == IK3_CLOAK && ( (CItemElem*)pItemBase )->m_idGuild != 0 )
						return FALSE;

//#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz	
//					if( pItemBase->m_dwItemId == II_SYS_SYS_SCR_SEALCHARACTER )
//						return FALSE;	// 개인상점판매허용
//#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz

#if __VER >= 9 // __CSC_VER9_1
//					ItemProp* pItemProp = pItemBase->GetProp();
					CItemElem* pItemElem = (CItemElem*)pItemBase;
					if( pItemElem->IsFlag( CItemElem::expired ) )
						return TRUE;
					/*
					if(pItemProp->dwItemKind3 == IK3_EGG && pItemElem->m_pPet) //사망한 펫은 거래 불가
					{
						if(pItemElem->m_pPet->GetLife() <= 0)
							return FALSE;
					}
					*/
#endif //__CSC_VER9_1

					/*
					if( ((CItemElem*)pItemBase)->m_bCharged == 1 )
					{
						g_WndMng.PutString( prj.GetText( TID_GAME_NOTTRADE ), NULL, prj.GetTextColor( TID_GAME_NOTTRADE ) );
						return FALSE;
					}
					*/

					int iIndex	= pShortcut->m_dwData - 100;
					if( 0 <= iIndex && iIndex < MAX_VENDITEM )						
					{
						if( m_pVendor->m_vtInfo.GetItem( iIndex ) != NULL )
							return FALSE;
						
						if( IsUsingItem( pItemBase ) == FALSE && m_pVendor->IsActiveMover() && !m_pVendor->m_vtInfo.IsVendorOpen() )
						{
							SAFE_DELETE( m_pWndRegVend );
							m_pWndRegVend	= new CWndRegVend;
							m_pWndRegVend->Initialize( this );
							m_pWndRegVend->SetItem( iIndex, pItemBase );
						}
					}
				}
			}	// APP_INVENTORY
		}
	}
	else
	{
		if( nID == WIDC_OK )
		{
			if( g_pPlayer->m_pActMover->IsFly() )
				return TRUE;
			if( g_pPlayer->m_vtInfo.VendorIsVendor() == FALSE )
				return TRUE;
			CWndEdit* pWndEdit	= (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
			CString strVendor	= pWndEdit->GetString();
			strVendor.TrimLeft();
			strVendor.TrimRight();

		#if __VER >= 11 // __MOD_VENDOR
			// 매물 액수 총합 + 현재 소지금이 2억1천이 넘으면 경고띄우고 리턴시킴.
			int nGold = g_pPlayer->GetGold();

			for( int iv = 0 ; iv < MAX_VENDITEM ; ++iv )
			{
				CItemBase *pItemBase = g_pPlayer->m_vtInfo.GetItem( iv );
				if( pItemBase == NULL )
					continue;

				nGold += ((CItemElem*)pItemBase)->m_nCost * pItemBase->GetExtra();
			}					
			
			if( nGold <= 0 || 2100000000 <= nGold )
			{
				g_WndMng.OpenMessageBox( _T(prj.GetText(TID_GAME_VENDOR_MAX_ALL_GOLD)), MB_OK, this );
				return TRUE;
			}
		#endif

			if( prj.IsInvalidName( strVendor )
#ifdef __RULE_0615
				|| prj.IsAllowedLetter( strVendor, TRUE ) == FALSE
#endif	// __RULE_0615
				)
			{
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0020) ) );
				return TRUE;
			}

			g_WndMng.WordChange( strVendor );
			// name
#ifdef __RULE_0615
			if( strVendor.GetLength() < 6 || strVendor.GetLength() > 32 )
#else	// __RULE_0615
			if( strVendor.GetLength() < 6 || strVendor.GetLength() >= 38 )
#endif	// __RULE_0615
			{
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0011) ) );
				return TRUE;
			}

			CString strFormat = strVendor;

			if( !strVendor.IsEmpty()  )
			{
				pWndEdit->m_string	= strFormat;
				pWndEdit->m_string.GetTextFormat(strFormat);
			}
			
#if __VER >= 8 // __S8_VENDOR_REVISION
			CWndVendorConfirm* pBox = new CWndVendorConfirm;
			g_WndMng.OpenCustomBox( "", pBox );			
			pBox->SetVendorName( strFormat );
			pBox->SetValue( prj.GetText( TID_GAME_VENDOR_START ) );
#else // __VER >= 8 // __S8_VENDOR_REVISION
			g_DPlay.SendPVendorOpen( (const char*)strFormat );
#endif // __VER >= 8 // __S8_VENDOR_REVISION
			CWndButton* pWndButton	= (CWndButton*)GetDlgItem( WIDC_OK );
			pWndButton->SetVisible( FALSE );
		}
		else if( nID == WIDC_CANCEL || nID == WTBID_CLOSE )
		{
#if __VER >= 8 // __S8_VENDOR_REVISION
			if( g_pPlayer->m_vtInfo.IsVendorOpen() )
			{
				CWndVendorConfirm* pBox = new CWndVendorConfirm;
				g_WndMng.OpenCustomBox( "", pBox );			
				pBox->SetValue( prj.GetText( TID_GAME_VENDOR_STOP ) );
				return FALSE;
			}
#endif // __VER >= 8 // __S8_VENDOR_REVISION
			Destroy( FALSE );
		}
		else if( nID == WIDC_CHAT )
		{
			BOOL bChattingRoom = FALSE;
			if( g_pPlayer->m_vtInfo.IsVendorOpen() )
			{
				g_DPlay.SendOpenChattingRoom();
				bChattingRoom = TRUE;
			}
			else if( g_pPlayer->m_vtInfo.GetOther() && g_Chatting.m_bState )
			{
				bChattingRoom = TRUE;
			}

			if( bChattingRoom )
			{
			#if __VER >= 11 // __MOD_VENDOR
				if(!m_pwndVenderMessage) 
				{
					m_pwndVenderMessage = new CWndVendorMessage;

					if(m_pVendor->IsActiveMover())
						m_pwndVenderMessage->m_nIsOwner = TRUE;
				#ifdef __FIX_WND_1109
					m_pwndVenderMessage->Initialize( this );
				#else	// __FIX_WND_1109
					m_pwndVenderMessage->Initialize( );
				#endif	// __FIX_WND_1109
				}
				else
				{
					if(m_pwndVenderMessage->IsVisible()) m_pwndVenderMessage->SetVisible(FALSE);
					else								 m_pwndVenderMessage->SetVisible(TRUE);
				}
			#else
				SAFE_DELETE( m_pwndVenderMessage );
				m_pwndVenderMessage = new CWndVendorMessage;
			#ifdef __FIX_WND_1109
				m_pwndVenderMessage->Initialize( this );
			#else	// __FIX_WND_1109
				m_pwndVenderMessage->Initialize( );
			#endif	// __FIX_WND_1109
			#endif

			}
		}
#if __VER >= 11 // __MOD_VENDOR
		else if(nID == WIDC_RESET)
		{
			
			CItemBase	apItem_VT[MAX_VENDITEM];		/// vendor and trader share pointer array
			ZeroMemory( apItem_VT, sizeof( apItem_VT ) );

			//if( g_pPlayer->m_vtInfo.IsVendorOpen() )
			//{
				for( int i = 0; i < MAX_VENDITEM; i++ )
				{
					// 리스트를 클리어 한다. 
					CItemBase* pItemBase = g_pPlayer->m_vtInfo.GetItem(i);
					if( pItemBase != NULL )
					{
						if( pItemBase->m_dwObjId != 0 )
							g_DPlay.SendUnregisterPVendorItem( i );
					}
				}

				// 저장버퍼도 클리어
				memset(g_Neuz.m_aSavedInven, 0, sizeof(g_Neuz.m_aSavedInven));
			//}
		}
#endif
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndVendor::OnDestroyChildWnd( CWndBase* pWndChild )
{
	if( pWndChild == m_pWndVendorBuy )
	{
		SAFE_DELETE( m_pWndVendorBuy );
	}
	else if( pWndChild == m_pWndRegVend )
	{
		SAFE_DELETE( m_pWndRegVend );
	}
	else if( pWndChild == m_pwndVenderMessage )
	{
		SAFE_DELETE( m_pwndVenderMessage );
	}
}

void CWndVendor::OnDestroy( void )
{
#if __VER >= 11 // __MOD_VENDOR
	// 리스트를 백업해둔다
	for( int i = 0; i < MAX_VENDITEM; i++ )
	{
		CItemBase* pItemBase = g_pPlayer->m_vtInfo.GetItem(i);
		if(pItemBase)
		{
			g_Neuz.m_aSavedInven[i].m_dwObjId = pItemBase->m_dwObjId;
			g_Neuz.m_aSavedInven[i].m_nExtra = pItemBase->m_nExtra;
			g_Neuz.m_aSavedInven[i].m_nCost = pItemBase->m_nCost;
			// 100304_mirchang vendor item check
			g_Neuz.m_aSavedInven[i].m_dwItemId = pItemBase->m_dwItemId;
		}
	}
#endif
	g_DPlay.SendPVendorClose( m_pVendor->GetId() );

	SAFE_DELETE( m_pWndVendorBuy );
	SAFE_DELETE( m_pWndRegVend );
	SAFE_DELETE( m_pwndVenderMessage );
}


CWndVendorMessage::CWndVendorMessage() 
{
#if __VER >= 11 // __MOD_VENDOR	
	m_nIsOwner = FALSE;
#endif
} 

CWndVendorMessage::~CWndVendorMessage() 
{ 
#if __VER >= 11 // __MOD_VENDOR
	m_wndChat.Destroy();
	if( m_nIsOwner )
	{
		m_wndInfo.Destroy();
	}
#endif
//	g_Chatting.m_nSizeofMember = 0;
} 
void CWndVendorMessage::OnDraw( C2DRender* p2DRender ) 
{ 
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 ); 
	
	pWndList->ResetContent();

	for( int i = 0 ; i < g_Chatting.GetChattingMember(); ++i )
	{
		pWndList->AddString( g_Chatting.m_szName[i] );
	}
} 
void CWndVendorMessage::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	
	pWndEdit->AddWndStyle( EBS_AUTOVSCROLL );
	pWndEdit->SetNativeMode();

	pWndEdit->SetFocus();

#if __VER >= 11 // __MOD_VENDOR
	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	WTCITEM tabTabItem;

	m_wndChat.Create(WBS_NODRAWFRAME, CRect( 0, 0, 300, 220 ), pWndTabCtrl, 10 );
	m_wndChat.AddWndStyle(WBS_VSCROLL);
	
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_APP_DIALOG); //"대화"
	tabTabItem.pWndBase = &m_wndChat;
	pWndTabCtrl->InsertItem( 0, &tabTabItem );

	if( m_nIsOwner )
	{
		m_wndInfo.Create(WBS_NODRAWFRAME, CRect( 0, 0, 300, 220 ), pWndTabCtrl, 11 );
		m_wndInfo.AddWndStyle(WBS_VSCROLL);
		tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
		tabTabItem.pszText = prj.GetText(TID_APP_INFOMATION); //"정보"
		tabTabItem.pWndBase = &m_wndInfo;
		pWndTabCtrl->InsertItem( 1, &tabTabItem );
	}
		
#endif

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
void CWndVendorMessage::InitSize( void )
{
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
}
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndVendorMessage::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_VENDOREX_CHAT, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndVendorMessage::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndVendorMessage::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	AdjustMinRect( &rectWnd, 16 * 10, 16 * 10 );
	CWndNeuz::SetWndRect( rectWnd, bOnSize );
}
void CWndVendorMessage::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndVendorMessage::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndVendorMessage::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndVendorMessage::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	CWndEdit* pWndText = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	switch(nID)
	{
		case WIDC_EDIT1: // 본문 
			if( message == EN_RETURN)
			{
				if( pWndText->m_string.IsEmpty() == FALSE )
				{
					char szChat[1024];
					if( strlen( pWndText->m_string ) < 1020 )
					{
						CString strFormat;
						pWndText->m_string.GetTextFormat(strFormat);
						if( strlen( strFormat ) < 1020 )
						{
							strcpy( szChat, strFormat );
							g_DPlay.SendChatting( szChat );
							pWndText->Empty();
						}
					}
				}
			}
			break;
		case WIDC_BUTTON1: // 본문 
			{
				if( pWndText->m_string.IsEmpty() == FALSE )
				{
					char szChat[1024];
					if( strlen( pWndText->m_string ) < 1020 )
					{
						CString strFormat;
						pWndText->m_string.GetTextFormat(strFormat);
						if( strlen( strFormat ) < 1020 )
						{
							strcpy( szChat, strFormat );
							g_DPlay.SendChatting( szChat );
							pWndText->Empty();
						}
					}
				}
			}
			break;
		case WTBID_CLOSE:
			{
				if( g_pPlayer->m_vtInfo.IsVendorOpen() )
					g_DPlay.SendCloseChattingRoom();
			}
			break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
void CWndVendorMessage::AddMessage( LPCTSTR lpszFrom, LPCTSTR lpszMessage )
{


#if __VER >= 11 // __MOD_VENDOR
	CString strMessage;

	if( !strcmp( lpszFrom, g_pPlayer->GetName() ) )
		strMessage.Format( "#cffff0000%s%s :#nc\n  %s\n", lpszFrom, prj.GetText(TID_GAME_FROM3), lpszMessage );
	else
#if __VER >= 19
		strMessage.Format( "#cff62cbe9%s%s :#nc\n  %s\n", lpszFrom, prj.GetText(TID_GAME_FROM3), lpszMessage );
#else
		strMessage.Format( "#cff0000ff%s%s :#nc\n  %s\n", lpszFrom, prj.GetText(TID_GAME_FROM3), lpszMessage );
#endif//__VER >= 19
	//  	strMessage.Format( "#cff0000ff%s님의 말 :#nc\n  %s\n", lpszFrom, lpszMessage );
	
	m_wndChat.AddString( strMessage );
	m_wndChat.m_wndScrollBar.SetMaxScrollPos();

#else
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	CString strMessage;

	if( !strcmp( lpszFrom, g_pPlayer->GetName() ) )
		strMessage.Format( "#cffff0000%s%s :#nc\n  %s\n", lpszFrom, prj.GetText(TID_GAME_FROM3), lpszMessage );
	else
#if __VER >= 19
		strMessage.Format( "#cff62cbe9%s%s :#nc\n  %s\n", lpszFrom, prj.GetText(TID_GAME_FROM3), lpszMessage );
#else
		strMessage.Format( "#cff0000ff%s%s :#nc\n  %s\n", lpszFrom, prj.GetText(TID_GAME_FROM3), lpszMessage );
#endif//__VER >= 19
	//  	strMessage.Format( "#cff0000ff%s님의 말 :#nc\n  %s\n", lpszFrom, lpszMessage );
	
	pWndText->AddString( strMessage );
	pWndText->m_wndScrollBar.SetMaxScrollPos();
#endif
	
}

void CWndVendorMessage::AddChattingMemver(LPCTSTR lpszName)
{
#if __VER >= 11 // __MOD_VENDOR
	if( m_nIsOwner )
	{
		CString str;
		str.Format( prj.GetText(TID_GAME_VENCHATIN), lpszName );

		CString strMessage;
		strMessage.Format( "#cffff0000%s\n", str );
		
		m_wndInfo.AddString( strMessage );
		m_wndInfo.m_wndScrollBar.SetMaxScrollPos();	
	}
#else
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	CString str;
	str.Format( prj.GetText(TID_GAME_VENCHATIN), lpszName );

	CString strMessage;
	strMessage.Format( "#cffff0000%s\n", str );
	
	pWndText->AddString( strMessage );
	pWndText->m_wndScrollBar.SetMaxScrollPos();	
#endif
}

#if __VER >= 11 // __MOD_VENDOR
void CWndVendorMessage::WriteBuyInfo(char *pBuyerName, CString strItem, int nItemNum, int nTotalCost)
{
	if( m_nIsOwner )
	{
		CString str;
		str.Format( prj.GetText(TID_GAME_BUY_INFO), pBuyerName, (LPSTR)(LPCSTR)strItem, nItemNum, nTotalCost);

		CString strMessage;
#if __VER >= 19
		strMessage.Format( "#cff62cbe9%s\n", str );
#else
		strMessage.Format( "#cff0000ff%s\n", str );
#endif//__VER >= 19

		m_wndInfo.AddString( strMessage );
		m_wndInfo.m_wndScrollBar.SetMaxScrollPos();
	}
}
#endif

void CWndVendorMessage::RemoveChattingMemver(LPCTSTR lpszName)
{
#if __VER >= 11 // __MOD_VENDOR
	if( m_nIsOwner )
	{
		CString str;
		str.Format( prj.GetText(TID_GAME_VENCHATOUT), lpszName );

		CString strMessage;
		strMessage.Format( "#cffff0000%s\n", str );
		
		m_wndInfo.AddString( strMessage );
		m_wndInfo.m_wndScrollBar.SetMaxScrollPos();
	}
#else
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	CString str;
	str.Format( prj.GetText(TID_GAME_VENCHATOUT), lpszName );

	CString strMessage;
	strMessage.Format( "#cffff0000%s\n", str );
	
	pWndText->AddString( strMessage );
	pWndText->m_wndScrollBar.SetMaxScrollPos();	
#endif	
}

#if __VER >= 8 // __S8_VENDOR_REVISION
void CWndVendorConfirm::SetVendorName( CString str )
{
	m_strVendorName = str;
}
void CWndVendorConfirm::SetValue( CString str )
{
	m_wndText.SetString( str );
}
BOOL CWndVendorConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return  CWndMessageBox::Initialize( "", pWndParent, MB_OKCANCEL );	
} 
BOOL CWndVendorConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case IDOK:
		{
			CWndVendor* pWndVendor	= (CWndVendor*)g_WndMng.GetWndBase( APP_VENDOR_REVISION );
			if( pWndVendor )
			{
				if( g_pPlayer->m_vtInfo.IsVendorOpen() )
					pWndVendor->Destroy();
				else
					g_DPlay.SendPVendorOpen( (const char*)m_strVendorName );
			}
			Destroy();
		}
		break;
	case IDCANCEL:
		CWndVendor* pWndVendor	= (CWndVendor*)g_WndMng.GetWndBase( APP_VENDOR_REVISION );
		if( pWndVendor && g_pPlayer->m_vtInfo.IsVendorOpen() == FALSE )
		{
			CWndButton* pWndButton	= (CWndButton*)pWndVendor->GetDlgItem( WIDC_OK );
			pWndButton->SetVisible( TRUE );
		}
		Destroy();
		break;
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
#endif // __VER >= 8 // __S8_VENDOR_REVISION