
#include "StdAfx.h"

#if __VER >= 15 // __GUILD_HOUSE

#include "AppDefine.h"
#include "defineText.h"
#include "WndGuildHouse.h"
#include "GuildHouse.h"
#include "dpclient.h"
extern	CDPClient	g_DPlay;


static const int MAX_LIST_VIEW_CAPACITY = 10;

CWndGHMainMenu::CWndGHMainMenu( )
{
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	m_bNoCloseButton = TRUE;
#endif
}

CWndGHMainMenu::~CWndGHMainMenu( )
{
	// 길드하우스를 빠져나감.
	CWndBase* pWnd = NULL;
	pWnd = (CWndGuildHousing*)g_WndMng.GetApplet( APP_GH_FURNITURE_STORAGE );
	if( pWnd )	
		pWnd->Destroy( ); 

	pWnd = (CWndGuild*)g_WndMng.GetApplet( APP_GUILD );
	if( pWnd )	
		pWnd->Destroy( ); 

	if( g_WndMng.m_pWndGuildBank )	
		SAFE_DELETE( g_WndMng.m_pWndGuildBank ); 

	if( GuildDeploy()->IsReady( ) )
		GuildDeploy()->EndDeploy( );
}

BOOL CWndGHMainMenu::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GH_MAIN, 0, CPoint( 0, 0 ), NULL );
} 

void CWndGHMainMenu::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_pSetupPower = (CWndButton*)GetDlgItem( WIDC_BTN_SETUPPOWER );
	m_pFurniture = (CWndButton*)GetDlgItem( WIDC_BTN_FURNITURE);
	m_pStorage = (CWndButton*)GetDlgItem( WIDC_BTN_STORAGE );
	m_pTeleporter = (CWndButton*)GetDlgItem( WIDC_BTN_TELEPORTER );
	m_pUpkeep = (CWndButton*)GetDlgItem( WIDC_BTN_UPKEEP );
}


BOOL CWndGHMainMenu::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case WIDC_BTN_SETUPPOWER:	//권한
		{
			CWndGuild* pWnd = (CWndGuild*)g_WndMng.GetApplet( APP_GUILD );
			if( pWnd )	
				pWnd->Destroy( ); 
			else		
			{
				g_WndMng.CreateApplet( APP_GUILD ); 
				pWnd = (CWndGuild*)g_WndMng.GetApplet( APP_GUILD );
				assert( pWnd );

				pWnd->SetCurTab( 4 ); //권한 tab으로 !
			}
		}
		
		break;	

	case WIDC_BTN_FURNITURE:	//가구창 		
		{
			CWndGuildHousing* pWnd = (CWndGuildHousing*)g_WndMng.GetApplet( APP_GH_FURNITURE_STORAGE );
			if( pWnd )	
			{
				if( pWnd->IsSection( CWndGuildHousing::GS_FURNITURE ) )
					pWnd->Destroy( ); 
				else 
				if( pWnd->IsSection( CWndGuildHousing::GS_TELEPORTER ) )			//텔레포터랑 같은 윈도를 쓰기때문에 텔레포터창 죽이고 가구창 열게함.
					pWnd->InitBySection( CWndGuildHousing::GS_FURNITURE );
				
			}
			else		
			{ 
				g_WndMng.CreateApplet( APP_GH_FURNITURE_STORAGE ); 
				CWndGuildHousing* pWnd = (CWndGuildHousing*)g_WndMng.GetApplet( APP_GH_FURNITURE_STORAGE );
				assert( pWnd );

				pWnd->InitBySection( CWndGuildHousing::GS_FURNITURE );

			}
		}
		break;

	case WIDC_BTN_STORAGE:		//창고		
		{
 			if( g_WndMng.m_pWndGuildBank )	
			{
				g_DPlay.SendCloseGuildBankWnd(); 
				g_WndMng.m_pWndGuildBank->Destroy( );
				g_WndMng.m_pWndGuildBank = NULL;
			}
			else
				g_DPlay.SendOpenGuildBankWnd();

			break;
		}

		break;	

	case WIDC_BTN_TELEPORTER:	//텔레포터 
		{
			CWndGuildHousing* pWnd = (CWndGuildHousing*)g_WndMng.GetApplet( APP_GH_FURNITURE_STORAGE );
			if( pWnd ) 
			{
				if( pWnd->IsSection( CWndGuildHousing::GS_TELEPORTER ) )
					pWnd->Destroy( ); 
				else
				if( pWnd->IsSection( CWndGuildHousing::GS_FURNITURE ) )
					pWnd->InitBySection( CWndGuildHousing::GS_TELEPORTER );
			}
			else				
			{
				g_WndMng.CreateApplet( APP_GH_FURNITURE_STORAGE ); 
				pWnd = (CWndGuildHousing*)g_WndMng.GetApplet( APP_GH_FURNITURE_STORAGE );
				assert( pWnd );

				pWnd->InitBySection( CWndGuildHousing::GS_TELEPORTER );
			}
		}
		break;

	case WIDC_BTN_UPKEEP:		//유지비 
		{
			BOOL bAuthority = FALSE;
			CGuild* pGuild = g_pPlayer->GetGuild( );
			if( pGuild )
				bAuthority = pGuild->IsAuthority( g_pPlayer->m_idPlayer, PF_GUILDHOUSE_UPKEEP );

			if( !bAuthority )		//유지비 권한이 없으면 
			{
				g_WndMng.PutString( GETTEXT( TID_GAME_GUILDHOUSE_TAX_LEVEL ) );
				break;
			}
	

			if( !g_WndMng.m_pWndUpkeep )
			{
				g_WndMng.m_pWndUpkeep = new CWndGHUpkeep;
				g_WndMng.m_pWndUpkeep->Initialize(&g_WndMng, APP_CONFIRM_BUY_);
				
			}else
			{
				g_WndMng.m_pWndUpkeep->Destroy( );
				g_WndMng.m_pWndUpkeep = NULL;
			}
		
		}
		break;

	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

BOOL CWndGHMainMenu::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return TRUE;
}

BOOL CWndGHMainMenu::Process()
{
	CWndStatic* pEndTime   = (CWndStatic*)GetDlgItem( WIDC_STATIC2 );
	if( !pEndTime )
		return FALSE;

	time_t	t = (time_t)GuildHouse->GetUpkeepTime() - time_null();
	if(t < 0)	
	{
		pEndTime->SetTitle( GETTEXT( TID_GAME_GUILDHOUSE_EXPIRATION ) );
		return TRUE;
	}
		
	CTimeSpan	ts( t );
	static CString	strDays, strHours, strMins, strSecs;
	CString strBind;
	const CString strSpace = " ";

	if( ts.GetDays( ) )
	{
		strDays.Format( prj.GetText( TID_PK_LIMIT_DAY ), ts.GetDays() );
		strBind += ( strSpace + strDays );
	}
	if( ts.GetHours( ) )
	{
		strHours.Format( prj.GetText(TID_PK_LIMIT_HOUR ), ts.GetHours() );
		strBind += ( strSpace + strHours );
	}
	if( ts.GetMinutes( ) )
	{
		strMins.Format( prj.GetText(TID_PK_LIMIT_MINUTE ), ts.GetMinutes() );
		strBind += ( strSpace + strMins );
	}
	if( ts.GetSeconds( ) )
	{
		strSecs.Format( prj.GetText(TID_PK_LIMIT_SECOND ), ts.GetSeconds() );
		strBind += ( strSpace + strSecs );
	}

	pEndTime->SetTitle( strBind );

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndGHTeleporter
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const int EMPTY_SLOT = 999;

CWndGHTeleporter::CWndGHTeleporter( )
{
	m_pOK = NULL;
	m_pFire = NULL;
	m_nSelectedSlot = -1;
}

CWndGHTeleporter::~CWndGHTeleporter( )
{
	DeleteDeviceObjects();
}

HRESULT CWndGHTeleporter::RestoreDeviceObjects()
{
	CWndNeuz::RestoreDeviceObjects();
	return S_OK;
}

HRESULT CWndGHTeleporter::InvalidateDeviceObjects()
{
	CWndNeuz::InvalidateDeviceObjects();
	return S_OK;
}

HRESULT CWndGHTeleporter::DeleteDeviceObjects()
{
	CWndNeuz::DeleteDeviceObjects();
	return InvalidateDeviceObjects();
}


BOOL CWndGHTeleporter::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GH_TELEPORTER, 0, CPoint( 0, 0 ), pWndParent );
} 

void CWndGHTeleporter::OnInitialUpdate()	
{
	CWndNeuz::OnInitialUpdate();

	RestoreDeviceObjects();
	
	m_pOK = (CWndButton*)GetDlgItem( WIDC_BUTTON_OK );
	m_pFire = (CWndButton*)GetDlgItem( WIDC_BUTTON_FIRE );

	m_nCtrlId[0] = WIDC_SLOT1;
	m_nCtrlId[1] = WIDC_SLOT2;
	m_nCtrlId[2] = WIDC_SLOT3;
	m_nCtrlId[3] = WIDC_SLOT4;
	m_nCtrlId[4] = WIDC_SLOT5;

	HOUSING_ITEM kItem;
	kItem.m_nType = EMPTY_SLOT;
	m_cWndItems.resize( MAX_TELEPORTER_SLOT, kItem );

	RefreshItems( );

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
}

BOOL CWndGHTeleporter::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case WIDC_BUTTON_OK:			
		{
			HOUSING_ITEM& kItem = m_cWndItems[ m_nSelectedSlot ];
			if( EMPTY_SLOT == kItem.m_nType )
				break;

			GH_Fntr_Info* pItem = GuildHouse->GetFurnitureInfoPtr( kItem.m_nSlotIndex );
			assert( pItem );
			
			GuildHouse->SendClientToWorld( GUILDHOUSE_PCKTTYPE_SETUP, *pItem, kItem.m_nSlotIndex );

			m_nSelectedSlot = -1;
		}
		break;

	case WIDC_BUTTON_FIRE:			
		{
			HOUSING_ITEM& kItem = m_cWndItems[ m_nSelectedSlot ];
			if( EMPTY_SLOT == kItem.m_nType )
				break;

			GH_Fntr_Info* pItem = GuildHouse->GetFurnitureInfoPtr( kItem.m_nSlotIndex );
			assert( pItem );

			GuildHouse->SendClientToWorld( GUILDHOUSE_PCKTTYPE_LISTDROP, *pItem, kItem.m_nSlotIndex );

			m_nSelectedSlot = -1;
		}

		break;
	case WTBID_CLOSE:				
		Destroy( );
		break;
	}

	return CWndBase::OnChildNotify( message, nID, pLResult );
}

void CWndGHTeleporter::OnLButtonDown( UINT nFlags, CPoint point )
{
	//해당 슬롯을 판별해서 정보를 shortcut에 넣어준다.
	int selectedSlot = GetSlotIndexByPoint( point );
 	if( selectedSlot < 0 || selectedSlot >= MAX_TELEPORTER_SLOT )
 		return;

	m_nSelectedSlot = selectedSlot;
// 
// 	size_t size = m_cWndItems.size();
// 	if( size == 0 )
// 		return;
// 
// 	HOUSING_ITEM& kFirst =  m_cWndItems[ 0 ];
// 	if( !kFirst.m_bDeploy )
// 		--selectedSlot;
// 
// 	if( selectedSlot > size -1 )
// 		return;
// 
// 	HOUSING_ITEM& kItem = m_cWndItems[ selectedSlot ];
// 	
// 	m_GlobalShortcut.m_pFromWnd = this;
// 	m_GlobalShortcut.m_dwShortcut = SHORTCUT_ITEM;
// 	m_GlobalShortcut.m_dwType     = ITYPE_ITEM;
// 	m_GlobalShortcut.m_dwId       = 0;
// 	m_GlobalShortcut.m_dwIndex    = kItem.m_nIndex;
// 	m_GlobalShortcut.m_pTexture   = m_pTexture[ selectedSlot ];
// 	m_GlobalShortcut.m_dwData     = selectedSlot;
}

void CWndGHTeleporter::OnDraw(C2DRender* p2DRender)
{
	DrawSlotItems( p2DRender );

	if( m_nSelectedSlot > -1 && m_nSelectedSlot < MAX_TELEPORTER_SLOT )
	{
		LPWNDCTRL wndCtrl = NULL;
		wndCtrl = GetWndCtrl( m_nCtrlId[ m_nSelectedSlot ] );
		CRect rect = wndCtrl->rect;

		p2DRender->RenderFillRect( rect, 0x55ff2277 );
	}
}

int CWndGHTeleporter::GetSlotIndexByPoint( const CPoint& point )
{
	LPWNDCTRL wndCtrl = NULL;
	CRect rect;
	for( int i = 0; i < MAX_TELEPORTER_SLOT; ++i )
	{
		wndCtrl = GetWndCtrl( m_nCtrlId[i] );		// 슬롯으로 만들어진 윈도우에 대하여 
		rect  = wndCtrl->rect;

		if( rect.PtInRect( point ) )
			return i;		
	}

	return -1;
}

int CWndGHTeleporter::GetCountListUp( )
{
	int count = 0;
	for( int i = 0; i < MAX_TELEPORTER_SLOT; ++i )
	{
		HOUSING_ITEM& kItem = m_cWndItems[ i ];
		if( EMPTY_SLOT != kItem.m_nType && !kItem.m_bDeploy )		//On Slot되어 있고 설치되지 않았다면 ( 리스트 업만 된 상태 )
			++count;
	}

	return count;
}

extern	CDPClient		g_DPlay;
BOOL CWndGHTeleporter::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CWndBase* pWndFrame	= pShortcut->m_pFromWnd->GetFrameWnd();
	if( !pWndFrame )
	{
		assert( 0 );
		return FALSE;
	}

	if( APP_INVENTORY == pWndFrame->GetWndId( ) )			// 인벤에서 온 아이템은 조건검사 
	{
		CItemElem* pItem = g_pPlayer->m_Inventory.GetAtId( pShortcut->m_dwId );
		if( !IsUsableItem( pItem ) )
			return FALSE;

		// 텔레포터 아이템일때만 
		if( IK3_TELEPORTER != pItem->GetProp( )->dwItemKind3  )
			return FALSE;

		g_DPlay.SendDoUseItem( MAKELONG( ITYPE_ITEM, pItem->m_dwObjId ), g_pPlayer->GetId( ) );
	}

	return TRUE;
}

void CWndGHTeleporter::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	//파괴 
	int selectedSlot = GetSlotIndexByPoint( point );
	if( selectedSlot < 0 || selectedSlot >= MAX_TELEPORTER_SLOT )
		return;

	HOUSING_ITEM& kWndItem = m_cWndItems[ selectedSlot ];
	if( EMPTY_SLOT == kWndItem.m_nType )
		return;
	GH_Fntr_Info* pItem = GuildHouse->GetFurnitureInfoPtr( kWndItem.m_nSlotIndex );
	assert( pItem );
	
	GuildHouse->SendClientToWorld( GUILDHOUSE_PCKTTYPE_LISTDROP, *pItem, kWndItem.m_nSlotIndex );
}

void CWndGHTeleporter::OnMouseWndSurface( CPoint point )
{
	CRect rect;
	LPWNDCTRL wndCtrl = NULL;
	if( m_cWndItems.empty( ) )
		return;
	
// 	for(int i=0; i<MAX_VIS; i++)
// 	{
// 		wndCtrl = GetWndCtrl( m_nCtrlId[i] );
// 		rect = wndCtrl->rect;
// 		if( rect.PtInRect( point ) )
// 		{
// 			ClientToScreen( &point );
// 			ClientToScreen( &rect );
// 
// 			DWORD dwIndex = pItem->GetPiercingItem( i );
// 			if(  0 != dwIndex )	
// 			{
// 				if( NULL == m_pItemElem[ i ] )
// 					m_pItemElem[ i ] = new CItemElem;
// 			
// 				m_pItemElem[ i ]->m_dwItemId = dwIndex;
// 
// 				time_t endTime = pItem->GetVisKeepTime( i );
// 				//DWORD remainTime = endTime - timeGetTime( );
// 				m_pItemElem[ i ]->m_dwKeepTime = endTime;
// 			
// 			}else
// 			{
// 				SAFE_DELETE( m_pItemElem[ i ] );
// 				continue;
// 			}
// 
// 			g_WndMng.PutToolTip_Item( (CItemBase*)m_pItemElem[i], point, &rect );
// 		}
// 
// 	}
}

GH_Fntr_Info* CWndGHTeleporter::GetInstalledItem( int& serverSlot )
{
	for( int i = 0; i < GuildHouse->GetFurnitureListSize( ); ++i )
	{
		GH_Fntr_Info* pItem = GuildHouse->GetFurnitureInfoPtr( i );
		assert( pItem );

		ItemProp* pProp = prj.GetItemProp( pItem->dwItemId );
		assert( pProp );

		if( IK3_TELEPORTER != pProp->dwItemKind3 )
			continue;

		if( pItem->bSetup )
		{
			serverSlot = i;
			return pItem;
		}
	}

	return NULL;
}
void CopySGHtoCGH( GH_Fntr_Info* pSeverData, HOUSING_ITEM& kWndItem )
{
	kWndItem.dwItemId = pSeverData->objId;
	kWndItem.m_bDeploy = pSeverData->bSetup;
	kWndItem.m_dwTime = pSeverData->tKeepTime;
	kWndItem.m_fAngle = pSeverData->fAngle;
	kWndItem.m_vPos = pSeverData->vPos;
	kWndItem.m_nIndex = pSeverData->dwItemId;

	kWndItem.m_nType = 0;
}

void CWndGHTeleporter::RefreshItems( )
{
	for( int set = 0; set < MAX_TELEPORTER_SLOT; ++set )
		m_cWndItems[ set ].m_nType = EMPTY_SLOT;

	
	ItemProp* pProp = NULL;
	
	// 0번째 슬롯 채우기 ( 텔레포터이고 설치된 경우 )
	int serverSlot = -1;
	GH_Fntr_Info* pItem = GetInstalledItem( serverSlot );
	if( pItem )
	{
		HOUSING_ITEM& kItem = m_cWndItems[ 0 ];
		CopySGHtoCGH( pItem, kItem );
		kItem.m_nSlotIndex = serverSlot;
		kItem.m_nTeleIndex = 0;
	
		pProp = prj.GetItemProp( pItem->dwItemId );
		assert( pProp );
		kItem.m_strName = pProp->szName;
	}
	
	// 1~ 4번 슬롯 채우기 
	int telSlot = 1;
	for( int i = 0; i < GuildHouse->GetFurnitureListSize( ); ++i )
	{
		 GH_Fntr_Info* pItem = GuildHouse->GetFurnitureInfoPtr( i );
		 assert( pItem );

		 ItemProp* pProp = prj.GetItemProp( pItem->dwItemId );
		 assert( pProp );

		 if( IK3_TELEPORTER != pProp->dwItemKind3 )
			 continue;

		 if( pItem->bSetup )		// 텔레포터 아이템인데 설치된 경우 이전에 이미 데이터를 집어넣었으므로 패스 
			 continue;	
		 
		 HOUSING_ITEM& kItem = m_cWndItems[ telSlot ];

		 if( telSlot > 4 )
		 {
			 assert( 0 );
			 continue;
		 }

		 CopySGHtoCGH( pItem, kItem );
		 kItem.m_nSlotIndex = i;
		 kItem.m_nTeleIndex = telSlot;
		 ++telSlot;
		 kItem.m_strName = pProp->szName;
	}
}

void CWndGHTeleporter::DrawSlotItems( C2DRender* p2DRender )
{
	if( !g_pPlayer )
		return;

	LPWNDCTRL wndCtrl = NULL;
	for( int i=0; i<MAX_TELEPORTER_SLOT; ++i )
	{
		HOUSING_ITEM& kItem = m_cWndItems[ i ];

		if( EMPTY_SLOT == kItem.m_nType )			//빈슬롯 
			continue;
	
		ItemProp* pProp = prj.GetItemProp( kItem.m_nIndex );
		if( !pProp )
			continue;

		m_pTexture[ i ] = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pProp->szIcon), 0xffff00ff );
			
		if(m_pTexture[ i ] != NULL)
		{
			wndCtrl = GetWndCtrl( m_nCtrlId[i] );
			if( wndCtrl )
			{
				CPoint pt2 = CPoint( wndCtrl->rect.left, wndCtrl->rect.top );
				if( 0 == i )
					m_pTexture[ i ]->Render2( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ), 0xffffff00 );
				else
					m_pTexture[ i ]->Render2( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ), 0xffffffff );
			}
		}

	}	
}

void CWndGHTeleporter::UpdateButtons( )
{
	if( m_nSelectedSlot > -1 && m_nSelectedSlot < MAX_TELEPORTER_SLOT )
	{
		if( 0 == m_nSelectedSlot )	//설치된 슬롯을 선택했다면
			m_pOK->EnableWindow( FALSE );
		else m_pOK->EnableWindow( TRUE );
	
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndUpkeep
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const int MAX_UPKEEP_DAY_CLIENT = MAX_UPKEEP_DAY - 1;


CWndGHUpkeep::CWndGHUpkeep() 
{ 
	m_pEdit = NULL;
	m_pStatic = NULL;

	assert( MAX_UPKEEP_DAY_CLIENT > 0 );
} 

CWndGHUpkeep::~CWndGHUpkeep() 
{ 

} 

void CWndGHUpkeep::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
} 


void CWndGHUpkeep::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndGHUpkeep::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndGHUpkeep::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

// 살려는 수량이 바뀌면, 가격표시도 변경시킨다.
void CWndGHUpkeep::OnChangeBuyCount( DWORD dwBuy )
{
	if( dwBuy > MAX_UPKEEP_DAY_CLIENT )
		return;

	CString szString;

	if( dwBuy != atoi(m_pEdit->GetString()) )
	{
		szString.Format("%u", dwBuy);
		m_pEdit->SetString( szString );
	}

	DWORD dwCost = 0;

	// 지금 소유한 길드하우스 레벨에 따라서 유지비 산출
	switch( GuildHouse->GetType() )
	{
	case WI_GUILDHOUSE_SMALL:
		dwCost = UPKEEP_DAY_PENYA_SMALL;
		break;

	case WI_GUILDHOUSE_MIDDLE:
		dwCost = UPKEEP_DAY_PENYA_MIDDLE;
		break;

	case WI_GUILDHOUSE_LARGE:
		assert( 0 );
		break;

	default:
		assert( 0 );
		break;
	}
		 
	DWORD dwTotalBuy = dwBuy * dwCost;
	
	szString.Format("%u", dwTotalBuy);
	m_pStaticGold->SetTitle( szString );
}

BOOL CWndGHUpkeep::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	InitDialog( g_Neuz.GetSafeHwnd(), APP_CONFIRM_BUY_, 0, 0, pWndParent );
	MoveParentCenter();

	SetTitle( GETTEXT( TID_GAME_GUILDHOUSE_TAXUI01 ) );					//유지비 

	CWndStatic* pLabel  = (CWndStatic  *)GetDlgItem( WIDC_STATIC );
	pLabel->SetTitle( "" );

	pLabel  = (CWndStatic  *)GetDlgItem( WIDC_STATIC1 );
	pLabel->SetTitle( GETTEXT( TID_GAME_GUILDHOUSE_TAXUI03 ) );			//납부금액

	m_pEdit   = (CWndEdit  *)GetDlgItem( WIDC_CONTROL2 );
	m_pStatic	= (CWndStatic *)GetDlgItem( WIDC_CONTROL1 );
	m_pStatic->SetTitle( GETTEXT( TID_GAME_GUILDHOUSE_TAXUI02 ) );		//유지기간 
	m_pStaticGold = (CWndStatic *)GetDlgItem( WIDC_STATIC2 );
	m_pStaticGold->AddWndStyle(WSS_MONEY);
	
	OnChangeBuyCount( 1 );
	return TRUE;
}

void CWndGHUpkeep::OnDraw( C2DRender* p2DRender ) 
{ 
	return;
} 

const int MAX_BUY_UPKEEP = 200;

BOOL CWndGHUpkeep::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	int nBuyNum = 0;

	switch( nID )
	{
	case WIDC_PLUS:
		nBuyNum = atoi(m_pEdit->GetString());
		++nBuyNum;

		if ( nBuyNum > MAX_UPKEEP_DAY_CLIENT )
			nBuyNum = MAX_UPKEEP_DAY_CLIENT;
			
		OnChangeBuyCount(nBuyNum);
		break;

	case WIDC_MINUS:
		nBuyNum = atoi(m_pEdit->GetString());
		if ( --nBuyNum < 1 )
			nBuyNum = 1;

		OnChangeBuyCount(nBuyNum);
		break;

	case WIDC_MAX:
		OnChangeBuyCount( MAX_UPKEEP_DAY_CLIENT );
		break;

	case WIDC_CONTROL2:
		if( EN_CHANGE == message )
		{
			if( m_pEdit == NULL )
			{
				ADDERRORMSG( "CWndConfirmBuy::OnChildNotify : m_pEdit == NULL" );
				char szMsg[256];
				sprintf( szMsg, "CWndConfirmBuy::OnChildNotify : more info(%d, %d)", pLResult, GetDlgItem( WIDC_CONTROL2 ) );
				ADDERRORMSG( szMsg );

				nBuyNum = 1;
			}
			else
				nBuyNum = atoi(m_pEdit->GetString());

			nBuyNum = max( nBuyNum, 0 );
			DWORD dwMAXCount = MAX_BUY_UPKEEP;

			nBuyNum = min( nBuyNum, (int)( dwMAXCount ) );

			OnChangeBuyCount(nBuyNum);
		}
		break;

	case WIDC_CANCEL:
		Destroy();
		break;

	case WIDC_OK:
		OnOK();
		Destroy();
		break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndGHUpkeep::OnOK()
{
	int nBuy = atoi( m_pEdit->GetString() );

	assert( nBuy > 0 && nBuy <= MAX_UPKEEP_DAY_CLIENT );
	GH_Fntr_Info data;
	data.dwItemId = nBuy; //기간 (일)
	GuildHouse->SendClientToWorld( GUILDHOUSE_PCKTTYPE_UPKEEP, data, NULL_ID );

//	PLAYSND( SND_INF_TRADE );
}


#endif //__GUILD_HOUSE