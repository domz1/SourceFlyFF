#include "StdAfx.h"
#ifdef __IMPROVE_MAP_SYSTEM
#ifdef __CLIENT
#include "WndMapEx.h"
#include "AppDefine.h"
#include "MapComboBoxData.h"
#include "party.h"
#include "playerdata.h"
#include "Continent.h"
#include "DPClient.h"
#include "defineText.h"
#include "defineMapComboBoxData.h"
#include "defineSound.h"
extern CDPClient g_DPlay;
//-----------------------------------------------------------------------------
const FLOAT CWndMapEx::SOURCE_MAP_SIZE_X( 1280.0F );
const FLOAT CWndMapEx::SOURCE_MAP_SIZE_Y( 960.0F );
const FLOAT CWndMapEx::ANIMATION_SPEED( 800.0f );
const FLOAT CWndMapEx::TELEPORTATION_POSITION_TEXTURE_SIZE_RATIO( 1.4f );
const FLOAT CWndMapEx::EXTEND_RATIO( 0.4f );
const FLOAT CWndMapEx::EXTEND_TELEPORTATION_POSITION_TEXTURE_SIZE_RATIO( TELEPORTATION_POSITION_TEXTURE_SIZE_RATIO + EXTEND_RATIO );
const FLOAT CWndMapEx::DESTINATION_POSITION_TEXTURE_SIZE_RATIO( 0.6f );
const FLOAT CWndMapEx::NPC_POSITION_TEXTURE_SIZE_RATIO( 1.6F );
const FLOAT CWndMapEx::USER_MARK_POSITION_TEXTURE_SIZE_RATIO( 1.0f );
const int CWndMapEx::WINDOW_TILE_TEXTURE_SIZE_XY( 16 );
const int CWndMapEx::MINIMUM_WINDOW_TILE_NUMBER_X( 34 );
const int CWndMapEx::MINIMUM_WINDOW_TILE_NUMBER_Y( 27 );
//-----------------------------------------------------------------------------
CWndMapEx::CWndMapEx( void ) : 
m_eConstructionMode( NORMAL ), 
m_pPCArrowTexture( NULL ), 
m_pPartyPCArrowTexture( NULL ), 
m_pMapTexture( NULL ), 
m_pTeleportationPositionTexture( NULL ), 
m_pDestinationPositionTexture( NULL ), 
m_pNPCPositionTexture( NULL ), 
m_pUserMarkPositionTexture( NULL ), 
m_fRevisedMapSizeRatio( 1.0F ), 
m_rectRevisedMapPosition( 0, 0, 0, 0 ), 
m_dwSelectedMapID( MCD_WORLD ), 
m_bySelectedMapLocationID( CONT_NODATA ), 
m_bMonsterInformationToolTip( FALSE ), 
m_nSelectedMonsterIconIndex( -1 ), 
m_dwSelectedUserMarkID( 0 ), 
m_nIconTextureAlpha( MINIMUM_ICON_TEXTURE_ALPHA ), 
m_tmOld( g_tmCurrent ), 
m_bAlphaSwitch( FALSE ), 
m_idTeleporter( NULL_ID ), 
m_fDestinationPositionX( -1.0F ), 
m_fDestinationPositionY( -1.0F ), 
m_fNPCPositionX( -1.0F ), 
m_fNPCPositionY( -1.0F ), 
m_bMapComboBoxInitialization( FALSE ), 
m_byTransparentStateAlpha( NORMAL_STATE_ALPHA ), 
m_pWndUserMarkNameChanger( NULL ), 
m_nMinimumWindowTileWidth( WINDOW_TILE_TEXTURE_SIZE_XY * MINIMUM_WINDOW_TILE_NUMBER_X ), 
m_nMinimumWindowTileHeight( WINDOW_TILE_TEXTURE_SIZE_XY * MINIMUM_WINDOW_TILE_NUMBER_Y ), 
m_nMaximumWindowTileWidth( WINDOW_TILE_TEXTURE_SIZE_XY * ( MINIMUM_WINDOW_TILE_NUMBER_X + 1 ) ), 
m_nMaximumWindowTileHeight( WINDOW_TILE_TEXTURE_SIZE_XY * ( MINIMUM_WINDOW_TILE_NUMBER_Y + 1 ) )
{
}
//-----------------------------------------------------------------------------
CWndMapEx::~CWndMapEx( void )
{
	SAFE_DELETE( m_pWndUserMarkNameChanger );

	m_WndMenuUserMark.DeleteAllMenu();

	for( vector< TeleportationIconInfo* >::iterator Iterator = m_vecTeleportationPositionRect.begin(); Iterator != m_vecTeleportationPositionRect.end(); ++Iterator )
	{
		SAFE_DELETE( *Iterator );
	}
	m_vecTeleportationPositionRect.clear();
}
//-----------------------------------------------------------------------------
BOOL CWndMapEx::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_MAP_EX, 0, CPoint( 0, 0 ), pWndParent );
}
//-----------------------------------------------------------------------------
void CWndMapEx::OnInitialUpdate( void )
{
	CWndNeuz::OnInitialUpdate();

	m_pPCArrowTexture = prj.m_MapInformationManager.GetPCArrowTexture();
	m_pPartyPCArrowTexture = prj.m_MapInformationManager.GetPartyPCArrowTexture();
	m_pTeleportationPositionTexture = prj.m_MapInformationManager.GetTeleportationPositionTexture();
	m_pDestinationPositionTexture = prj.m_MapInformationManager.GetDestinationPositionTexture();
	m_pNPCPositionTexture = prj.m_MapInformationManager.GetNPCPositionTexture();
	m_pUserMarkPositionTexture = prj.m_MapInformationManager.GetUserMarkPositionTexture();

	CWndComboBox* pWndComboBoxMapCategory = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_MAP_CATEGORY );
	if( pWndComboBoxMapCategory )
	{
		pWndComboBoxMapCategory->AddWndStyle( EBS_READONLY );

		int nIndex = 0;
		MapComboBoxDataVector* pMapCategoryVector = prj.m_MapInformationManager.GetMapCategoryVector();
		if( pMapCategoryVector )
		{
			for( MapComboBoxDataVector::iterator Iterator = pMapCategoryVector->begin(); Iterator != pMapCategoryVector->end(); ++Iterator )
			{
				CMapComboBoxData* pMapComboBoxData = ( CMapComboBoxData* )( *Iterator );
				if( pMapComboBoxData == NULL )
				{
					continue;
				}
				pWndComboBoxMapCategory->AddString( pMapComboBoxData->GetTitle() );
				pWndComboBoxMapCategory->SetItemData( nIndex, pMapComboBoxData->GetID() );
				++nIndex;
			}
		}
	}

	CWndComboBox* pWndComboBoxMapName = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_MAP_NAME );
	if( pWndComboBoxMapName )
	{
		pWndComboBoxMapName->AddWndStyle( EBS_READONLY );
	}

	CWndComboBox* pWndComboBoxNpcName = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_NPC_NAME );
	if( pWndComboBoxNpcName )
	{
		pWndComboBoxNpcName->AddWndStyle( EBS_READONLY );
	}

	m_WndMenuUserMark.CreateMenu( this );
	m_WndMenuUserMark.AppendMenu( 0, ID_USER_MARK_MENU_DELETE , prj.GetText( TID_GAME_MAP_EX_MARK_DELETE ) );
	m_WndMenuUserMark.AppendMenu( 0, ID_USER_MARK_MENU_DELETE_ALL , prj.GetText( TID_GAME_MAP_EX_MARK_DELETE_ALL ) );
	m_WndMenuUserMark.AppendMenu( 0, ID_USER_MARK_MENU_INSERT_CHATTING_WINDOW , prj.GetText( TID_GAME_MAP_EX_MARK_INSERT_CHATTING_WINDOW ) );
	m_WndMenuUserMark.AppendMenu( 0, ID_USER_MARK_MENU_CHANGE_NAME , prj.GetText( TID_GAME_MAP_EX_MARK_CHANGE_NAME ) );

	CalculateMaximumWindowTileLength();
}
//-----------------------------------------------------------------------------
BOOL CWndMapEx::Process( void )
{
	InitializeMapComboBoxSelecting();
	ProcessMapSizeInformation();
	ProcessMonsterInformationToolTip();
	ProcessUserMarkToolTip();
	ProcessIconTextureAlpha();
	UpdateDestinationPosition();

#ifdef __MAP_EX_ALPHA
	if( prj.m_bMapTransparent == FALSE )
	{
		m_byTransparentStateAlpha = NORMAL_STATE_ALPHA;
#endif
		CWndComboBox* pWndComboBoxMapCategory = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_MAP_CATEGORY );
		if( pWndComboBoxMapCategory && pWndComboBoxMapCategory->IsVisible() == FALSE )
		{
			pWndComboBoxMapCategory->SetVisible( TRUE );
		}

		CWndComboBox* pWndComboBoxMapName = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_MAP_NAME );
		if( pWndComboBoxMapName && pWndComboBoxMapName->IsVisible() == FALSE )
		{
			pWndComboBoxMapName->SetVisible( TRUE );
		}

		CWndComboBox* pWndComboBoxNpcName = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_NPC_NAME );
		if( pWndComboBoxNpcName && pWndComboBoxNpcName->IsVisible() == FALSE )
		{
			pWndComboBoxNpcName->SetVisible( TRUE );
		}

		CWndButton* pWndMonsterInfoCheckBox = ( CWndButton* )GetDlgItem( WIDC_BUTTON_MONSTER_INFO );
		if( pWndMonsterInfoCheckBox && pWndMonsterInfoCheckBox->IsVisible() == FALSE )
		{
			pWndMonsterInfoCheckBox->SetVisible( TRUE );
		}
#ifdef __MAP_EX_ALPHA
	}
	else
	{
		m_byTransparentStateAlpha = TRANSPARENT_STATE_ALPHA;

		CWndComboBox* pWndComboBoxMapCategory = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_MAP_CATEGORY );
		if( pWndComboBoxMapCategory && pWndComboBoxMapCategory->IsVisible() == TRUE )
		{
			pWndComboBoxMapCategory->SetVisible( FALSE );
		}

		CWndComboBox* pWndComboBoxMapName = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_MAP_NAME );
		if( pWndComboBoxMapName && pWndComboBoxMapName->IsVisible() == TRUE )
		{
			pWndComboBoxMapName->SetVisible( FALSE );
		}

		CWndComboBox* pWndComboBoxNpcName = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_NPC_NAME );
		if( pWndComboBoxNpcName && pWndComboBoxNpcName->IsVisible() == TRUE )
		{
			pWndComboBoxNpcName->SetVisible( FALSE );
		}

		CWndButton* pWndMonsterInfoCheckBox = ( CWndButton* )GetDlgItem( WIDC_BUTTON_MONSTER_INFO );
		if( pWndMonsterInfoCheckBox && pWndMonsterInfoCheckBox->IsVisible() == TRUE )
		{
			pWndMonsterInfoCheckBox->SetVisible( FALSE );
		}
	}
#endif
	return TRUE;
}
//-----------------------------------------------------------------------------
void CWndMapEx::OnDraw( C2DRender* p2DRender )
{
	p2DRender->RenderFillRect( GetClientRect(), D3DCOLOR_ARGB( m_byTransparentStateAlpha, 0, 0, 0 ) );

	if( m_pMapTexture == NULL )
	{
		return;
	}

	p2DRender->RenderTexture( 
		CPoint( m_rectRevisedMapPosition.left, m_rectRevisedMapPosition.top ), 
		m_pMapTexture, 
		m_byTransparentStateAlpha, 
		m_fRevisedMapSizeRatio, 
		m_fRevisedMapSizeRatio );

	RenderTeleportationPosition( p2DRender );

	RenderDestinationPosition( p2DRender );

	RenderNPCPosition( p2DRender );

	RenderUserMarkPosition( p2DRender );

	for( int i = 0; i < g_Party.GetSizeofMember(); ++i )
	{
		if( g_pPlayer->m_idPlayer != g_Party.GetPlayerId( i ) )
		{
			if( g_Party.m_aMember[ i ].m_bRemove == FALSE )
			{
				if( m_pPartyPCArrowTexture )
				{
					RenderPlayerPosition( 
						p2DRender, 
						m_pPartyPCArrowTexture, 
						g_Party.GetPos( i ), 
						g_Party.GetPos( i ) + D3DXVECTOR3( 0.0F, 0.0F, -1.0F ), 
						CPlayerDataCenter::GetInstance()->GetPlayerString( g_Party.m_aMember[ i ].m_uPlayerId ), 
						0xffffff7f );
				}
			}
		}
	}

	RenderPlayerPosition( p2DRender, m_pPCArrowTexture, g_pPlayer->GetPos(), g_Neuz.m_camera.m_vPos, g_pPlayer->GetName(), 0xff7fff7f );

	RenderMapMonsterInformation( p2DRender );

	RenderRainbowNPCInformation( p2DRender );
}
//-----------------------------------------------------------------------------
void CWndMapEx::PaintFrame( C2DRender* p2DRender )
{
#ifdef __MAP_EX_ALPHA
	if( prj.m_bMapTransparent == TRUE )
	{
		return;
	}
#endif
	if( m_pTexture == NULL )
	{
		return;
	}

	RenderWnd();
	if( IsWndStyle( WBS_CAPTION ) )
	{
		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle );
		p2DRender->TextOut( 10, 4, m_strTitle, ( ( m_dwColor & 0x00ffffff ) | ( m_nAlphaCount << 24 ) ) );
		p2DRender->SetFont( pOldFont );
	}
}
//-----------------------------------------------------------------------------
BOOL CWndMapEx::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	if( &m_WndMenuUserMark == pWndBase )
	{
		switch( nID )
		{
		case ID_USER_MARK_MENU_DELETE:
			{
				prj.m_MapInformationManager.DeleteUserMarkPositionInfo( m_dwSelectedUserMarkID );
				break;
			}
		case ID_USER_MARK_MENU_DELETE_ALL:
			{
				prj.m_MapInformationManager.DeleteAllUserMarkPositionInfo();
				break;
			}
		case ID_USER_MARK_MENU_INSERT_CHATTING_WINDOW:
			{
				int a = 0;
				break;
			}
		case ID_USER_MARK_MENU_CHANGE_NAME:
			{
				CUserMarkPositionInfo* pUserMarkPositionInfo = prj.m_MapInformationManager.FindUserMarkPositionInfo( m_dwSelectedUserMarkID );
				if( pUserMarkPositionInfo )
				{
					SAFE_DELETE( m_pWndUserMarkNameChanger );
					m_pWndUserMarkNameChanger = new CWndUserMarkNameChanger;
					m_pWndUserMarkNameChanger->Initialize();
					m_pWndUserMarkNameChanger->SetInfo( pUserMarkPositionInfo->GetID(), pUserMarkPositionInfo->GetName() );
				}
				break;
			}
		}

		m_dwSelectedUserMarkID = 0;
		m_WndMenuUserMark.SetVisible( FALSE );
	}

	return TRUE;
}
//-----------------------------------------------------------------------------
BOOL CWndMapEx::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( message )
	{
	case WNM_SELCHANGE:
		{
			switch( nID )
			{
			case WIDC_COMBOBOX_MAP_CATEGORY:
				{
					CWndComboBox* pWndComboBoxMapCategory = ( CWndComboBox* )pLResult;
					if( pWndComboBoxMapCategory == NULL )
					{
						break;
					}
					DWORD dwSelectedMapCategoryItemData = pWndComboBoxMapCategory->GetSelectedItemData();
					RearrangeComboBoxData( dwSelectedMapCategoryItemData, WIDC_COMBOBOX_MAP_NAME, prj.m_MapInformationManager.GetMapNameVector() );

					CWndComboBox* pWndComboBoxMapName = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_MAP_NAME );
					if( pWndComboBoxMapName == NULL )
					{
						break;
					}
					DWORD dwSelectedMapNameItemData = pWndComboBoxMapName->GetSelectedItemData();
					RearrangeComboBoxData( dwSelectedMapNameItemData, WIDC_COMBOBOX_NPC_NAME, prj.m_MapInformationManager.GetNPCNameVector() );

					break;
				}
			case WIDC_COMBOBOX_MAP_NAME:
				{
					CWndComboBox* pWndComboBoxMapName = ( CWndComboBox* )pLResult;
					if( pWndComboBoxMapName == NULL )
					{
						break;
					}
					DWORD dwSelectedMapNameItemData = pWndComboBoxMapName->GetSelectedItemData();
					RearrangeComboBoxData( dwSelectedMapNameItemData, WIDC_COMBOBOX_NPC_NAME, prj.m_MapInformationManager.GetNPCNameVector() );

					break;
				}
			}

			ResetMapInformation();
			ResetNPCPosition();

			break;
		}
	}

	if( nID == WIDC_BUTTON_MONSTER_INFO )
	{
		CWndButton* pWndButton = ( CWndButton* )GetDlgItem( WIDC_BUTTON_MONSTER_INFO );
		pWndButton->SetCheck( !pWndButton->GetCheck() );
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
//-----------------------------------------------------------------------------
void CWndMapEx::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	AdjustMinRect( &rectWnd, m_nMinimumWindowTileWidth, m_nMinimumWindowTileHeight );
	AdjustMaxRect( &rectWnd, m_nMaximumWindowTileWidth, m_nMaximumWindowTileHeight );

	CWndNeuz::SetWndRect( rectWnd, bOnSize );
}
//-----------------------------------------------------------------------------
void CWndMapEx::OnLButtonDown( UINT nFlags, CPoint point )
{
#ifdef __MAP_EX_ALPHA
	if( prj.m_bMapTransparent == TRUE )
	{
		if( g_WndMng.m_pWndWorld )
		{
			CRect rect = GetClientRect( TRUE );
			g_WndMng.m_pWndWorld->m_bLButtonDown = TRUE;
			g_WndMng.m_pWndWorld->OnLButtonDown( nFlags, CPoint( point.x + rect.left, point.y + rect.top ) );
		}
	}
	else
	{
#endif
		m_dwSelectedUserMarkID = 0;
		m_WndMenuUserMark.SetVisible( FALSE );

		if( g_WndMng.m_pWndWorld->m_bCtrlPushed == TRUE )
		{
			D3DXVECTOR3 vReconvertedPosition( 0.0F, 0.0F, 0.0F );
			ReconvertPosition( 
				vReconvertedPosition, 
				D3DXVECTOR3( static_cast< FLOAT >( point.x ), 0.0F, static_cast< FLOAT >( point.y ) ), 
				m_bySelectedMapLocationID );
			prj.m_MapInformationManager.InsertUserMarkPositionInfo( prj.GetText( TID_GAME_MAP_EX_MARK_INITIAL_TITLE_NAME ), vReconvertedPosition.x, vReconvertedPosition.z );
			return;
		}

		if( m_eConstructionMode == TELEPORTATION )
		{
			for( vector< TeleportationIconInfo* >::iterator Iterator = m_vecTeleportationPositionRect.begin(); Iterator != m_vecTeleportationPositionRect.end(); ++Iterator )
			{
				TeleportationIconInfo* pTeleportationIconInfo = ( TeleportationIconInfo* )( *Iterator );
				if( pTeleportationIconInfo == NULL )
				{
					continue;
				}

				const D3DXVECTOR3 vTeleportationIconPosition( 
					static_cast< FLOAT >( pTeleportationIconInfo->GetIconPoint().x ), 
					0.0F, 
					static_cast< FLOAT >( pTeleportationIconInfo->GetIconPoint().y ) );
				BYTE byPlayerLocationID = GetMapArea( vTeleportationIconPosition );
				if( m_bySelectedMapLocationID != CONT_NODATA && byPlayerLocationID != m_bySelectedMapLocationID )
				{
					continue;
				}

				D3DXVECTOR3 vConvertedPosition( 0.0F, 0.0F, 0.0F );
				ConvertPosition( vConvertedPosition, vTeleportationIconPosition, m_bySelectedMapLocationID );

				CRect rectTeleportationPositionIcon( 0, 0, 0, 0 );
				CalculateMapIconRectFromPoint( 
					rectTeleportationPositionIcon, 
					vConvertedPosition.x, 
					vConvertedPosition.z, 
					m_pTeleportationPositionTexture, 
					TELEPORTATION_POSITION_TEXTURE_SIZE_RATIO );
				if( rectTeleportationPositionIcon.PtInRect( point ) == TRUE )
				{
					CMover* pMover = prj.GetMover( m_idTeleporter );
					if( pMover == NULL )
					{
						return;
					}

					PLAYSND( SND_INF_CLICK );
					g_DPlay.SendTeleporterReq( pMover->m_szCharacterKey, pTeleportationIconInfo->GetIndex() );
					Destroy();
					return;
				}
			}
		}
#ifdef __MAP_EX_ALPHA
	}
#endif
}
//-----------------------------------------------------------------------------
void CWndMapEx::OnRButtonDown( UINT nFlags, CPoint point )
{
#ifdef __MAP_EX_ALPHA
	if( prj.m_bMapTransparent == TRUE )
	{
		if( g_WndMng.m_pWndWorld )
		{
			CRect rect = GetClientRect( TRUE );
			g_WndMng.m_pWndWorld->m_bRButtonDown = TRUE;
			g_WndMng.m_pWndWorld->OnRButtonDown( nFlags, CPoint( point.x + rect.left, point.y + rect.top ) );
		}
	}
	else
	{
#endif
		UserMarkPositionInfoList& rUserMarkPositionInfoList = prj.m_MapInformationManager.GetUserMarkPositionInfoList();
		for( UserMarkPositionInfoList::iterator Iterator = rUserMarkPositionInfoList.begin(); Iterator != rUserMarkPositionInfoList.end(); ++Iterator )
		{
			CUserMarkPositionInfo* pUserMarkPositionInfo = ( CUserMarkPositionInfo* )( *Iterator );
			if( pUserMarkPositionInfo == NULL )
			{
				continue;
			}

			D3DXVECTOR3 vConvertedPosition( 0.0F, 0.0F, 0.0F );
			ConvertPosition( 
				vConvertedPosition, 
				D3DXVECTOR3( pUserMarkPositionInfo->GetPositionX(), 0.0F, pUserMarkPositionInfo->GetPositionY() ), 
				m_bySelectedMapLocationID );

			CRect rectUserMarkIcon( 0, 0, 0, 0 );
			CalculateMapIconRectFromPoint( 
				rectUserMarkIcon, 
				vConvertedPosition.x, 
				vConvertedPosition.z, 
				m_pUserMarkPositionTexture, 
				USER_MARK_POSITION_TEXTURE_SIZE_RATIO );
			if( rectUserMarkIcon.PtInRect( point ) == TRUE )
			{
				m_dwSelectedUserMarkID = pUserMarkPositionInfo->GetID();

				ClientToScreen( &point );
				m_WndMenuUserMark.Move( point );
				m_WndMenuUserMark.SetVisible( TRUE );
				m_WndMenuUserMark.SetFocus();

				break;
			}
		}
#ifdef __MAP_EX_ALPHA
	}
#endif
}
//-----------------------------------------------------------------------------
void CWndMapEx::SetConstructionMode( CWndMapEx::ConstructionMode eConstructionMode )
{
	m_eConstructionMode = eConstructionMode;
}
//-----------------------------------------------------------------------------
void CWndMapEx::InitializeTeleportationInformation( CMover* const pFocusMover )
{
	switch( m_eConstructionMode )
	{
	case TELEPORTATION:
		{
			CHARACTER* pCharacter = pFocusMover->GetCharacter();
			if( pCharacter == NULL )
			{
				return;
			}

			if( pCharacter->m_vecTeleportPos.empty() == true )
			{
				return;
			}

			if( m_idTeleporter != pFocusMover->GetId() )
			{
				m_idTeleporter = pFocusMover->GetId();

				int nIndex = 0;
				for( vector< D3DXVECTOR3 >::iterator Iterator = pCharacter->m_vecTeleportPos.begin(); Iterator != pCharacter->m_vecTeleportPos.end(); ++Iterator )
				{
					TeleportationIconInfo* pTeleportationIconInfo = new TeleportationIconInfo;
					pTeleportationIconInfo->SetIndex( nIndex );
					++nIndex;
					pTeleportationIconInfo->SetIconPoint( CPoint( static_cast< LONG >( Iterator->x ), static_cast< LONG >( Iterator->z ) ) );
					m_vecTeleportationPositionRect.push_back( pTeleportationIconInfo );
				}
			}

			break;
		}
	}
}
//-----------------------------------------------------------------------------
void CWndMapEx::UpdateDestinationPosition( void )
{
	switch( m_eConstructionMode )
	{
	case DESTINATION:
		{
			CWndWorld* pWndWorld = ( CWndWorld* )g_WndMng.GetApplet( APP_WORLD );
			if( pWndWorld == NULL )
			{
				break;
			}

			const D3DXVECTOR3& vDestinationArrowPosition( pWndWorld->m_vDestinationArrow );
			BYTE byPlayerLocationID = GetMapArea( vDestinationArrowPosition );
			if( m_bySelectedMapLocationID != CONT_NODATA && byPlayerLocationID != m_bySelectedMapLocationID )
			{
				break;
			}

			D3DXVECTOR3 vDestination( 0.0F, 0.0F, 0.0F );
			ConvertPosition( vDestination, vDestinationArrowPosition, m_bySelectedMapLocationID );
			m_fDestinationPositionX = vDestination.x;
			m_fDestinationPositionY = vDestination.z;

			break;
		}
	}
}
//-----------------------------------------------------------------------------
void CWndMapEx::InitializeMapComboBoxSelecting( void )
{
	if( m_bMapComboBoxInitialization == TRUE )
	{
		return;
	}

	m_bMapComboBoxInitialization = TRUE;

	BYTE byLocationID = CONT_NODATA;
	switch( m_eConstructionMode )
	{
	case NORMAL:
		{
			if( g_pPlayer )
			{
				byLocationID = GetMapArea( g_pPlayer->GetPos() );
			}
			break;
		}
	}

	CWndComboBox* pWndComboBoxMapCategory = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_MAP_CATEGORY );
	CWndComboBox* pWndComboBoxMapName = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_MAP_NAME );

	MapComboBoxDataVector* pMapComboBoxDataVector = prj.m_MapInformationManager.GetMapNameVector();
	for( MapComboBoxDataVector::iterator Iterator = pMapComboBoxDataVector->begin(); Iterator != pMapComboBoxDataVector->end(); ++Iterator )
	{
		CMapComboBoxData* pMapComboBoxData = ( CMapComboBoxData* )( *Iterator );
		if( pMapComboBoxData == NULL )
		{
			continue;
		}

		if( byLocationID == pMapComboBoxData->GetLocationID() )
		{
			DWORD dwMapCategoryID = pMapComboBoxData->GetParentID();

			if( pWndComboBoxMapCategory )
			{
				pWndComboBoxMapCategory->SelectItem( dwMapCategoryID );
			}

			if( pWndComboBoxMapName )
			{
				pWndComboBoxMapName->SelectItem( pMapComboBoxData->GetID() );
			}

			break;
		}
	}
}
//-----------------------------------------------------------------------------
void CWndMapEx::ResetMapInformation( void )
{
	CWorld* pWorld	= g_WorldMng();
	CWndComboBox* pWndComboBoxMapName = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_MAP_NAME );
	if( pWndComboBoxMapName == NULL )
	{
		return;
	}
	DWORD dwSelectedMapNameItemData = pWndComboBoxMapName->GetSelectedItemData();
	if( dwSelectedMapNameItemData == MCD_NONE )
	{
		return;
	}

	MapComboBoxDataVector* pMapComboBoxDataVector = prj.m_MapInformationManager.GetMapNameVector();
	for( MapComboBoxDataVector::iterator Iterator = pMapComboBoxDataVector->begin(); Iterator != pMapComboBoxDataVector->end(); ++Iterator )
	{
		CMapComboBoxData* pMapComboBoxData = ( CMapComboBoxData* )( *Iterator );
		if( pMapComboBoxData == NULL )
		{
			continue;
		}

		if( dwSelectedMapNameItemData == pMapComboBoxData->GetID() )
		{
			CString strMapPictureFileName = pMapComboBoxData->GetPictureFileName();
			m_pMapTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( _T( "Theme\\" ), ::GetLanguage(), strMapPictureFileName ), 0xff000000 );
			//m_pMapTexture = pMapComboBoxData->GetMapTexture();
			m_dwSelectedMapID = pMapComboBoxData->GetID();
			m_bySelectedMapLocationID = pMapComboBoxData->GetLocationID();
			break;
		}
		else if( /*!g_pPlayer->IsMapOpen() && */( pWorld->m_bIsIndoor || pWorld->IsWorldInstanceDungeon() ))
		{
		    m_pMapTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( _T( "Theme\\" ), ::GetLanguage(),"WndMapBlind.dds" ), 0xff000000 );
		}
		else
		{
			m_pMapTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( _T( "Theme\\" ), ::GetLanguage(),"WndMapBlind.dds" ), 0xff000000 );
		}
	}
}
//-----------------------------------------------------------------------------
void CWndMapEx::ResetNPCPosition( void )
{
	CWndComboBox* pWndComboBoxNPCName = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX_NPC_NAME );
	if( pWndComboBoxNPCName == NULL )
	{
		return;
	}
	DWORD dwSelectedNPCNameItemData = pWndComboBoxNPCName->GetSelectedItemData();
	if( dwSelectedNPCNameItemData == MCD_NONE )
	{
		m_fNPCPositionX = -1.0F;
		m_fNPCPositionY = -1.0F;
		return;
	}

	MapComboBoxDataVector* pMapComboBoxDataVector = prj.m_MapInformationManager.GetNPCNameVector();
	for( MapComboBoxDataVector::iterator Iterator = pMapComboBoxDataVector->begin(); Iterator != pMapComboBoxDataVector->end(); ++Iterator )
	{
		CMapComboBoxData* pMapComboBoxData = ( CMapComboBoxData* )( *Iterator );
		if( pMapComboBoxData == NULL )
		{
			continue;
		}

		if( dwSelectedNPCNameItemData == pMapComboBoxData->GetID() )
		{
			CPoint pointNPCPosition = pMapComboBoxData->GetNPCPosition();
			m_fNPCPositionX = static_cast< FLOAT >( pointNPCPosition.x );
			m_fNPCPositionY = static_cast< FLOAT >( pointNPCPosition.y );
			break;
		}
	}
}
//-----------------------------------------------------------------------------
void CWndMapEx::RearrangeComboBoxData( DWORD dwParentSelectedListItemData, DWORD dwComboBoxWIDC , MapComboBoxDataVector* pMapComboBoxDataVector )
{
	CWndComboBox* pWndComboBox = ( CWndComboBox* )GetDlgItem( dwComboBoxWIDC );
	if( pWndComboBox == NULL )
	{
		return;
	}
	pWndComboBox->ResetContent();

	if( pMapComboBoxDataVector == NULL )
	{
		return;
	}

	if( dwParentSelectedListItemData == MCD_NONE )
	{
		pWndComboBox->EnableWindow( FALSE );
	}
	else
	{
		pWndComboBox->EnableWindow( TRUE );
	}

	int nIndex = 0;
	for( MapComboBoxDataVector::iterator Iterator = pMapComboBoxDataVector->begin(); Iterator != pMapComboBoxDataVector->end(); ++Iterator )
	{
		CMapComboBoxData* pMapComboBoxData = ( CMapComboBoxData* )( *Iterator );
		if( pMapComboBoxData == NULL )
		{
			continue;
		}

		if( pMapComboBoxData->GetParentID() == dwParentSelectedListItemData )
		{
			pWndComboBox->AddString( pMapComboBoxData->GetTitle() );
			pWndComboBox->SetItemData( nIndex, pMapComboBoxData->GetID() );
			++nIndex;
		}
	}

	if( static_cast< int >( pWndComboBox->GetListBoxSize() ) == 1 )
	{
		pWndComboBox->SetCurSel( 0 );
	}
}
//-----------------------------------------------------------------------------
void CWndMapEx::ProcessMapSizeInformation( void )
{
	if( m_pMapTexture == NULL )
	{
		return;
	}

	CSize sizeClient = GetClientRect().Size();
	CSize sizeTexture = m_pMapTexture->m_size;
	float fWindowXYRatio = static_cast< float >( sizeClient.cx ) / static_cast< float >( sizeClient.cy );
	float fTextureXYRatio = static_cast< float >( sizeTexture.cx ) / static_cast< float >( sizeTexture.cy );
	float fTextureYXRatio = static_cast< float >( sizeTexture.cy ) / static_cast< float >( sizeTexture.cx );
	FLOAT fRevisedMapSizeRatio = 1.0F;

	CSize sizeRevisedTexture( sizeClient );
	int nStartPositionX = 0;
	int nStartPositionY = 0;
	if( fWindowXYRatio > fTextureXYRatio )
	{
		sizeRevisedTexture.cx = static_cast< int >( static_cast< float >( sizeClient.cy ) * fTextureXYRatio );
		sizeRevisedTexture.cy = sizeClient.cy;
		nStartPositionX = ( sizeClient.cx - sizeRevisedTexture.cx ) / 2;
		m_fRevisedMapSizeRatio = fRevisedMapSizeRatio * ( ( static_cast< FLOAT >( sizeRevisedTexture.cy ) / static_cast< FLOAT >( sizeTexture.cy ) ) );
	}
	else if( fWindowXYRatio < fTextureXYRatio )
	{
		sizeRevisedTexture.cx = sizeClient.cx;
		sizeRevisedTexture.cy = static_cast< int >( static_cast< float >( sizeClient.cx ) * fTextureYXRatio );
		nStartPositionY = ( sizeClient.cy - sizeRevisedTexture.cy ) / 2;
		m_fRevisedMapSizeRatio = fRevisedMapSizeRatio * ( ( static_cast< FLOAT >( sizeRevisedTexture.cx ) / static_cast< FLOAT >( sizeTexture.cx ) ) );
	}

	m_rectRevisedMapPosition.SetRect( nStartPositionX, nStartPositionY, nStartPositionX + sizeRevisedTexture.cx, nStartPositionY + sizeRevisedTexture.cy );
}
//-----------------------------------------------------------------------------
void CWndMapEx::ProcessMonsterInformationToolTip( void )
{
	if( m_pMapTexture == NULL )
	{
		return;
	}
 
	CWndButton* pWndButton = ( CWndButton* )GetDlgItem( WIDC_BUTTON_MONSTER_INFO );
	if( pWndButton == NULL )
	{
		return;
	}
	if( pWndButton->GetCheck() == FALSE )
	{
		return;
	}

	CMapMonsterInformationPack* pMapMonsterInformationPack = prj.m_MapInformationManager.FindMapMonsterInformationPack( m_dwSelectedMapID );
	if( pMapMonsterInformationPack == NULL )
	{
		return;
	}

	CPoint pointMouse = GetMousePoint();

	for( int i = 0; i < static_cast< int >( pMapMonsterInformationPack->GetNumber() ); ++i )
	{
		CTexture* pMonsterIconTexture = pMapMonsterInformationPack->GetAt( i );
		if( pMonsterIconTexture == NULL )
		{
			continue;
		}

		const CMapMonsterInformation* pMapMonsterInformation = pMapMonsterInformationPack->GetMapMonsterInformation( i );
		if( pMapMonsterInformation == NULL )
		{
			continue;
		}

		CRect rectRevisedMonsterIconPosition( 0, 0, 0, 0 );
		ReviseScriptRectInformation( rectRevisedMonsterIconPosition, pMapMonsterInformation->GetIconPositionRect() );

		m_bMonsterInformationToolTip = FALSE;

		if( rectRevisedMonsterIconPosition.PtInRect( pointMouse ) == TRUE )
		{
			CEditString strMonsterInformationToolTip = _T( "" );
			CString strTemp = _T( "" );

			CPoint pointMouseScreen = pointMouse;
			CRect rectRevisedMonsterIconPositionScreen = rectRevisedMonsterIconPosition;
			ClientToScreen( &pointMouseScreen );
			ClientToScreen( &rectRevisedMonsterIconPositionScreen );

			for( int j = 0; j < pMapMonsterInformation->GetMonsterIDNumber(); ++j )
			{
				MoverProp* pMoverProp = prj.GetMoverProp( pMapMonsterInformation->GetMonsterID( j ) );
				if( pMoverProp == NULL )
				{
					continue;
				}

				strMonsterInformationToolTip.AddString( prj.GetText( TID_GAME_MONSTER_INFORMATION_LEVEL ), D3DCOLOR_XRGB( 0, 0, 255 ) );
				strTemp.Format( _T( "%d " ), pMoverProp->dwLevel );
				strMonsterInformationToolTip.AddString( strTemp, D3DCOLOR_XRGB( 130, 130, 200 ) );
				strTemp.Format( _T( "%s" ), pMoverProp->szName );
				if( j + 1 == pMapMonsterInformation->GetMonsterIDNumber() )
				{
					strMonsterInformationToolTip.AddString( strTemp, D3DCOLOR_XRGB( 255, 0, 0 ) );
				}
				else
				{
					strMonsterInformationToolTip.AddString( strTemp );
				}
				strMonsterInformationToolTip.AddString( _T( "\n" ) );
			}

			strMonsterInformationToolTip.AddString( _T( "\n" ) );

			strTemp.Format( _T( "%s : " ), prj.GetText( TID_GAME_DROP_ITEM ) );
			strMonsterInformationToolTip.AddString( strTemp );
			ItemProp* pItemProp = prj.GetItemProp( pMapMonsterInformation->GetDropItemID() );
			if( pItemProp )
			{
				strTemp.Format( _T( "%s" ), pItemProp->szName );
				strMonsterInformationToolTip.AddString( strTemp, D3DCOLOR_XRGB( 46, 112, 169 ) );
			}
			else
			{
				strMonsterInformationToolTip.AddString( prj.GetText( TID_GAME_DROP_NONE ) );
			}

			g_toolTip.PutToolTip( 10000, strMonsterInformationToolTip, rectRevisedMonsterIconPositionScreen, pointMouseScreen, 0 );
			g_toolTip.ResizeMapMonsterToolTip();
			for( int k = 0; k < pMapMonsterInformation->GetMonsterIDNumber(); ++k )
			{
				g_toolTip.InsertMonsterID( pMapMonsterInformation->GetMonsterID( k ) );
			}

			m_nSelectedMonsterIconIndex = i;
			m_bMonsterInformationToolTip = TRUE;

			break;
		}
	}

	if( m_bMonsterInformationToolTip == FALSE )
	{
		m_nSelectedMonsterIconIndex = -1;
	}
}
//-----------------------------------------------------------------------------
void CWndMapEx::ProcessUserMarkToolTip( void )
{
	if( m_pMapTexture == NULL )
	{
		return;
	}

	UserMarkPositionInfoList& rUserMarkPositionInfoList = prj.m_MapInformationManager.GetUserMarkPositionInfoList();
	for( UserMarkPositionInfoList::iterator Iterator = rUserMarkPositionInfoList.begin(); Iterator != rUserMarkPositionInfoList.end(); ++Iterator )
	{
		CUserMarkPositionInfo* pUserMarkPositionInfo = ( CUserMarkPositionInfo* )( *Iterator );
		if( pUserMarkPositionInfo == NULL )
		{
			continue;
		}

		D3DXVECTOR3 vConvertedPosition( 0.0F, 0.0F, 0.0F );
		ConvertPosition( 
			vConvertedPosition, 
			D3DXVECTOR3( pUserMarkPositionInfo->GetPositionX(), 0.0F, pUserMarkPositionInfo->GetPositionY() ), 
			m_bySelectedMapLocationID );

		CRect rectUserMarkIcon( 0, 0, 0, 0 );
		CalculateMapIconRectFromPoint( 
			rectUserMarkIcon, 
			vConvertedPosition.x, 
			vConvertedPosition.z, 
			m_pUserMarkPositionTexture, 
			USER_MARK_POSITION_TEXTURE_SIZE_RATIO );
		CPoint pointMouse = GetMousePoint();
		if( rectUserMarkIcon.PtInRect( pointMouse ) == TRUE )
		{
			CEditString strUserMarkToolTip = pUserMarkPositionInfo->GetName();
			ClientToScreen( &pointMouse );
			ClientToScreen( &rectUserMarkIcon );
			g_toolTip.PutToolTip( 10000, strUserMarkToolTip, rectUserMarkIcon, pointMouse, 0 );

			break;
		}
	}
}
//-----------------------------------------------------------------------------
void CWndMapEx::ProcessIconTextureAlpha( void )
{
	if( m_eConstructionMode != DESTINATION )
	{
		return;
	}

	float fDelta = ( static_cast< float >( g_tmCurrent - m_tmOld ) / 1000.0f );
	m_tmOld = g_tmCurrent;

	if( m_bAlphaSwitch == FALSE )
	{
		m_nIconTextureAlpha += static_cast< int >( ANIMATION_SPEED * fDelta );
		if( m_nIconTextureAlpha > MAXIMUM_ICON_TEXTURE_ALPHA )
		{
			m_nIconTextureAlpha = MAXIMUM_ICON_TEXTURE_ALPHA;
			m_bAlphaSwitch = TRUE;
		}
	}
	else
	{
		m_nIconTextureAlpha -= static_cast< int >( ANIMATION_SPEED * fDelta );
		if( m_nIconTextureAlpha < MINIMUM_ICON_TEXTURE_ALPHA )
		{
			m_nIconTextureAlpha = MINIMUM_ICON_TEXTURE_ALPHA;
			m_bAlphaSwitch = FALSE;
		}
	}
}
//-----------------------------------------------------------------------------
void CWndMapEx::RenderPlayerPosition( C2DRender* p2DRender, CTexture* pArrowTexture, const D3DXVECTOR3& vPlayerPosition, const D3DXVECTOR3& vCameraPosition, const CString& strName, DWORD dwNameColor )
{
	if( pArrowTexture == NULL )
	{
		return;
	}

	BYTE byPlayerLocationID = GetMapArea( vPlayerPosition );
	if( m_bySelectedMapLocationID != CONT_NODATA && byPlayerLocationID != m_bySelectedMapLocationID )
	{
		return;
	}

	D3DXVECTOR3 vConvertedPosition( 0.0F, 0.0F, 0.0F );
	ConvertPosition( vConvertedPosition, vPlayerPosition, m_bySelectedMapLocationID );
	CPoint pointConvertedPosition( static_cast< int >( vConvertedPosition.x ), static_cast< int >( vConvertedPosition.z ) );

	// 위치 출력
	D3DXVECTOR3 vCamera = g_Neuz.m_camera.m_vPos;
	D3DXVECTOR3 vView = vPlayerPosition - vCameraPosition;
	vView.y = 0.0F;
	D3DXVec3Normalize( &vView, &vView );
	D3DXVECTOR3 vDatumLine = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
	FLOAT fTheta = D3DXVec3Dot( &vDatumLine, &vView );
	D3DXVECTOR3 vAxis( 0.0F, 0.0F, 0.0F );
	D3DXVec3Cross( &vAxis, &vDatumLine, &vView );
	FLOAT fResultRadian = ( vAxis.y >= 0.0F ) ? acosf( fTheta ) : -acosf( fTheta );
	FLOAT fRevisedArrowStartPositionX = static_cast< FLOAT >( m_pPCArrowTexture->m_size.cx ) * m_fRevisedMapSizeRatio * 0.5F;
	FLOAT fRevisedArrowStartPositionY = static_cast< FLOAT >( m_pPCArrowTexture->m_size.cy ) * m_fRevisedMapSizeRatio * 0.5F;
	int nRevisedPCArrowStartPositionX = static_cast< int >( static_cast< FLOAT >( pointConvertedPosition.x ) - fRevisedArrowStartPositionX );
	int nRevisedPCArrowStartPositionY = static_cast< int >( static_cast< FLOAT >( pointConvertedPosition.y ) - fRevisedArrowStartPositionY );
	pArrowTexture->RenderRotate( p2DRender, CPoint( nRevisedPCArrowStartPositionX, nRevisedPCArrowStartPositionY ), fResultRadian, TRUE, 255, m_fRevisedMapSizeRatio, m_fRevisedMapSizeRatio );

	// 이름 출력
	CD3DFont* pOldFont = p2DRender->GetFont();
	p2DRender->SetFont( CWndBase::m_Theme.m_pFontWorld );
	static const int FONT_REVISING_VALUE = 2;

	CSize sizeNameText = p2DRender->GetFont()->GetTextExtent( strName );
	// 폰트가 변경되어 원래 크기보다 FONT_REVISING_VALUE 만큼 더 늘어남 (가로 세로 모두 )
	sizeNameText.cx += ( FONT_REVISING_VALUE * 2 );
	sizeNameText.cy += ( FONT_REVISING_VALUE * 2 );

	int nTextPositionX = pointConvertedPosition.x;
	int nTextPositionY = pointConvertedPosition.y;
	nTextPositionX -= sizeNameText.cx / 2;
	nTextPositionY -= static_cast< int >( ( static_cast< FLOAT >( sizeNameText.cy ) * m_fRevisedMapSizeRatio ) + static_cast< FLOAT >( sizeNameText.cy / 2 ) );
	p2DRender->TextOut( nTextPositionX, nTextPositionY, strName, dwNameColor );

	p2DRender->SetFont( pOldFont );
}
//-----------------------------------------------------------------------------
void CWndMapEx::RenderTeleportationPosition( C2DRender* p2DRender )
{
	if( m_eConstructionMode != TELEPORTATION )
	{
		return;
	}

	if( m_pTeleportationPositionTexture == NULL )
	{
		return;
	}

	for( vector< TeleportationIconInfo* >::iterator Iterator = m_vecTeleportationPositionRect.begin(); Iterator != m_vecTeleportationPositionRect.end(); ++Iterator )
	{
		TeleportationIconInfo* pTeleportationIconInfo = ( TeleportationIconInfo* )( *Iterator );
		if( pTeleportationIconInfo == NULL )
		{
			continue;
		}

		const D3DXVECTOR3 vTeleportationIconPosition( 
			static_cast< FLOAT >( pTeleportationIconInfo->GetIconPoint().x ), 
			0.0F, 
			static_cast< FLOAT >( pTeleportationIconInfo->GetIconPoint().y ) );
		BYTE byPlayerLocationID = GetMapArea( vTeleportationIconPosition );
		if( m_bySelectedMapLocationID != CONT_NODATA && byPlayerLocationID != m_bySelectedMapLocationID )
		{
			continue;
		}

		D3DXVECTOR3 vConvertedPosition( 0.0F, 0.0F, 0.0F );
		ConvertPosition( vConvertedPosition, vTeleportationIconPosition, m_bySelectedMapLocationID );

		CRect rectTeleportationPositionIcon( 0, 0, 0, 0 );
		FLOAT fRevisedTeleportationPositionTextureSizeRatio = CalculateMapIconRectFromPoint( 
			rectTeleportationPositionIcon, 
			vConvertedPosition.x, 
			vConvertedPosition.z, 
			m_pTeleportationPositionTexture, 
			TELEPORTATION_POSITION_TEXTURE_SIZE_RATIO );
		CPoint pointMouse = GetMousePoint();
		if( rectTeleportationPositionIcon.PtInRect( pointMouse ) == TRUE )
		{
			CPoint pointExtendTeleportationPosition( 0, 0 );
			FLOAT fExtendTeleportationPositionTextureSizeRatio = CalculateMapIconStartPosition( 
				pointExtendTeleportationPosition, 
				vConvertedPosition.x, 
				vConvertedPosition.z, 
				m_pTeleportationPositionTexture, 
				EXTEND_TELEPORTATION_POSITION_TEXTURE_SIZE_RATIO );
			m_pTeleportationPositionTexture->RenderScal( 
				p2DRender, 
				pointExtendTeleportationPosition, 
				255, 
				fExtendTeleportationPositionTextureSizeRatio, 
				fExtendTeleportationPositionTextureSizeRatio );
		}
		else
		{
			m_pTeleportationPositionTexture->RenderScal( 
				p2DRender, 
				CPoint( rectTeleportationPositionIcon.left, rectTeleportationPositionIcon.top ), 
				255, 
				fRevisedTeleportationPositionTextureSizeRatio, 
				fRevisedTeleportationPositionTextureSizeRatio );
		}
	}
}
//-----------------------------------------------------------------------------
void CWndMapEx::RenderMapMonsterInformation( C2DRender* p2DRender )
{
	CWndButton* pWndButton = ( CWndButton* )GetDlgItem( WIDC_BUTTON_MONSTER_INFO );
	if( pWndButton == NULL )
	{
		return;
	}
	if( pWndButton->GetCheck() == FALSE )
	{
		return;
	}

	if( m_dwSelectedMapID == MCD_NONE )
	{
		return;
	}
	CMapMonsterInformationPack* pMapMonsterInformationPack = prj.m_MapInformationManager.FindMapMonsterInformationPack( m_dwSelectedMapID );
	if( pMapMonsterInformationPack == NULL )
	{
		return;
	}

	for( int i = 0; i < static_cast< int >( pMapMonsterInformationPack->GetNumber() ); ++i )
	{
		CTexture* pMonsterIconTexture = pMapMonsterInformationPack->GetAt( i );
		if( pMonsterIconTexture == NULL )
		{
			continue;
		}

		const CMapMonsterInformation* pMapMonsterInformation = pMapMonsterInformationPack->GetMapMonsterInformation( i );
		if( pMapMonsterInformation == NULL )
		{
			continue;
		}

		CRect rectRevisedMonsterIconPosition( 0, 0, 0, 0 );
		ReviseScriptRectInformation( rectRevisedMonsterIconPosition, pMapMonsterInformation->GetIconPositionRect() );

		if( i == m_nSelectedMonsterIconIndex )
		{
			pMonsterIconTexture->RenderScal( p2DRender, CPoint( rectRevisedMonsterIconPosition.left, rectRevisedMonsterIconPosition.top ), 255, m_fRevisedMapSizeRatio, m_fRevisedMapSizeRatio );
		}
		else
		{
			pMonsterIconTexture->RenderScal( p2DRender, CPoint( rectRevisedMonsterIconPosition.left, rectRevisedMonsterIconPosition.top ), 150, m_fRevisedMapSizeRatio, m_fRevisedMapSizeRatio );
		}
	}
}
//-----------------------------------------------------------------------------
void CWndMapEx::RenderRainbowNPCInformation( C2DRender* p2DRender )
{
	if( CRainbowRace::GetInstance()->m_dwRemainTime == 0 )
	{
		return;
	}

	CRainbowNPCInformationPack* pRainbowNPCInformationPack = prj.m_MapInformationManager.GetRainbowNPCInformationPack();
	if( pRainbowNPCInformationPack == NULL )
	{
		return;
	}

	for( int i = 0; i < static_cast< int >( pRainbowNPCInformationPack->GetNumber() ); ++i )
	{
		CTexture* pRainbowNPCTexture = pRainbowNPCInformationPack->GetAt( i );
		if( pRainbowNPCTexture == NULL )
		{
			continue;
		}

		const CRainbowNPCInformation* pRainbowNPCInformation = pRainbowNPCInformationPack->GetRainbowNPCInformation( i );
		if( pRainbowNPCInformation == NULL )
		{
			continue;
		}

		CRect rectRevisedRainbowNPCPosition( 0, 0, 0, 0 );
		if( m_dwSelectedMapID == MCD_MADRIGAL )
		{
			ReviseScriptRectInformation( rectRevisedRainbowNPCPosition, pRainbowNPCInformation->GetWorldPositionRect() );
			pRainbowNPCTexture->RenderScal( p2DRender, CPoint( rectRevisedRainbowNPCPosition.left, rectRevisedRainbowNPCPosition.top ), 255, m_fRevisedMapSizeRatio, m_fRevisedMapSizeRatio );
		}
		else if( m_dwSelectedMapID == pRainbowNPCInformation->GetMapID() )
		{
			ReviseScriptRectInformation( rectRevisedRainbowNPCPosition, pRainbowNPCInformation->GetRegionPositionRect() );
			pRainbowNPCTexture->RenderScal( p2DRender, CPoint( rectRevisedRainbowNPCPosition.left, rectRevisedRainbowNPCPosition.top ), 150, m_fRevisedMapSizeRatio, m_fRevisedMapSizeRatio );
		}
	}
}
//-----------------------------------------------------------------------------
void CWndMapEx::RenderDestinationPosition( C2DRender* p2DRender )
{
	if( m_eConstructionMode != DESTINATION )
	{
		return;
	}

	if( m_pDestinationPositionTexture == NULL )
	{
		return;
	}

	CWndWorld* pWndWorld = ( CWndWorld* )g_WndMng.GetApplet( APP_WORLD );
	if( pWndWorld == NULL )
	{
		return;
	}

	const D3DXVECTOR3& vDestinationArrowPosition( pWndWorld->m_vDestinationArrow );
	BYTE byPlayerLocationID = GetMapArea( vDestinationArrowPosition );
	if( m_bySelectedMapLocationID != CONT_NODATA && byPlayerLocationID != m_bySelectedMapLocationID )
	{
		return;
	}

	if( m_fDestinationPositionX == -1.0F && m_fDestinationPositionY == -1.0F )
	{
		return;
	}

	CPoint pointRevisedDestinationPosition( 0, 0 );
	FLOAT fRevisedUserMarkPositionTextureSizeRatio = CalculateMapIconStartPosition( 
		pointRevisedDestinationPosition, 
		m_fDestinationPositionX, 
		m_fDestinationPositionY, 
		m_pDestinationPositionTexture, 
		DESTINATION_POSITION_TEXTURE_SIZE_RATIO );
	m_pDestinationPositionTexture->RenderScal( 
		p2DRender, 
		pointRevisedDestinationPosition, 
		m_nIconTextureAlpha, 
		fRevisedUserMarkPositionTextureSizeRatio, 
		fRevisedUserMarkPositionTextureSizeRatio );
}
//-----------------------------------------------------------------------------
void CWndMapEx::RenderNPCPosition( C2DRender* p2DRender )
{
	if( m_pNPCPositionTexture == NULL )
	{
		return;
	}

	CWndWorld* pWndWorld = ( CWndWorld* )g_WndMng.GetApplet( APP_WORLD );
	if( pWndWorld == NULL )
	{
		return;
	}

	if( m_fNPCPositionX == -1.0F && m_fNPCPositionY == -1.0F )
	{
		return;
	}

	const D3DXVECTOR3 vNPCPosition( m_fNPCPositionX, 0.0F, m_fNPCPositionY );
	BYTE byPlayerLocationID = GetMapArea( vNPCPosition );
	if( m_bySelectedMapLocationID != CONT_NODATA && byPlayerLocationID != m_bySelectedMapLocationID )
	{
		return;
	}

	D3DXVECTOR3 vConvertedPosition( 0.0F, 0.0F, 0.0F );
	ConvertPosition( vConvertedPosition, vNPCPosition, m_bySelectedMapLocationID );

	CPoint pointRevisedNPCPosition( 0, 0 );
	FLOAT fRevisedNPCPositionTextureSizeRatio = CalculateMapIconStartPosition( 
		pointRevisedNPCPosition, 
		vConvertedPosition.x, 
		vConvertedPosition.z, 
		m_pNPCPositionTexture, 
		NPC_POSITION_TEXTURE_SIZE_RATIO );
	m_pNPCPositionTexture->RenderScal( 
		p2DRender, 
		pointRevisedNPCPosition, 
		255, 
		fRevisedNPCPositionTextureSizeRatio, 
		fRevisedNPCPositionTextureSizeRatio );
}
//-----------------------------------------------------------------------------
void CWndMapEx::RenderUserMarkPosition( C2DRender* p2DRender )
{
	if( m_pUserMarkPositionTexture == NULL )
	{
		return;
	}

	UserMarkPositionInfoList& rUserMarkPositionInfoList = prj.m_MapInformationManager.GetUserMarkPositionInfoList();
	for( UserMarkPositionInfoList::iterator Iterator = rUserMarkPositionInfoList.begin(); Iterator != rUserMarkPositionInfoList.end(); ++Iterator )
	{
		CUserMarkPositionInfo* pUserMarkPositionInfo = ( CUserMarkPositionInfo* )( *Iterator );
		if( pUserMarkPositionInfo == NULL )
		{
			continue;
		}

		const D3DXVECTOR3 vUserMarkPosition( pUserMarkPositionInfo->GetPositionX(), 0.0F, pUserMarkPositionInfo->GetPositionY() );
		BYTE byPlayerLocationID = GetMapArea( vUserMarkPosition );
		if( m_bySelectedMapLocationID != CONT_NODATA && byPlayerLocationID != m_bySelectedMapLocationID )
		{
			continue;
		}

		D3DXVECTOR3 vConvertedPosition( 0.0F, 0.0F, 0.0F );
		ConvertPosition( vConvertedPosition, vUserMarkPosition, m_bySelectedMapLocationID );

		CPoint pointRevisedUserMarkPosition( 0, 0 );
		FLOAT fRevisedUserMarkPositionTextureSizeRatio = CalculateMapIconStartPosition( 
			pointRevisedUserMarkPosition, 
			vConvertedPosition.x, 
			vConvertedPosition.z, 
			m_pUserMarkPositionTexture, 
			USER_MARK_POSITION_TEXTURE_SIZE_RATIO );
		m_pUserMarkPositionTexture->RenderScal( 
			p2DRender, 
			pointRevisedUserMarkPosition, 
			255, 
			fRevisedUserMarkPositionTextureSizeRatio, 
			fRevisedUserMarkPositionTextureSizeRatio );
	}
}
//-----------------------------------------------------------------------------
FLOAT CWndMapEx::CalculateMapIconRectFromPoint( CRect& rectDestinationIcon, FLOAT fIconPositionX, FLOAT fIconPositionY, const CTexture* const pIconTexture, FLOAT fIconSizeRatio )
{
	if( pIconTexture == NULL )
	{
		return 0.0F;
	}

	FLOAT fRevisedIconSizeRatio = fIconSizeRatio * m_fRevisedMapSizeRatio;
	FLOAT fRevisedIconSizeX = static_cast< FLOAT >( pIconTexture->m_size.cx ) * fRevisedIconSizeRatio;
	FLOAT fRevisedIconSizeY = static_cast< FLOAT >( pIconTexture->m_size.cy ) * fRevisedIconSizeRatio;
	FLOAT fRevisedIconStartPositionX = fIconPositionX - ( fRevisedIconSizeX * 0.5f );
	FLOAT fRevisedIconStartPositionY = fIconPositionY - ( fRevisedIconSizeY * 0.5f );
	rectDestinationIcon.SetRect( 
		static_cast< int >( fRevisedIconStartPositionX ), 
		static_cast< int >( fRevisedIconStartPositionY ), 
		static_cast< int >( fRevisedIconStartPositionX + fRevisedIconSizeX ), 
		static_cast< int >( fRevisedIconStartPositionY + fRevisedIconSizeY ) );
	return fRevisedIconSizeRatio;
}
//-----------------------------------------------------------------------------
FLOAT CWndMapEx::CalculateMapIconStartPosition( CPoint& pointDestinationPosition, FLOAT fIconPositionX, FLOAT fIconPositionY, const CTexture* const pIconTexture, FLOAT fIconSizeRatio )
{
	FLOAT fRevisedIconSizeRatio = fIconSizeRatio * m_fRevisedMapSizeRatio;
	FLOAT fRevisedIconSizeX = static_cast< FLOAT >( pIconTexture->m_size.cx ) * fRevisedIconSizeRatio;
	FLOAT fRevisedIconSizeY = static_cast< FLOAT >( pIconTexture->m_size.cy ) * fRevisedIconSizeRatio;
	pointDestinationPosition.SetPoint( 
		static_cast< int >( fIconPositionX - ( fRevisedIconSizeX * 0.5f ) ), 
		static_cast< int >( fIconPositionY - ( fRevisedIconSizeY * 0.5f ) ) );
	return fRevisedIconSizeRatio;
}
//-----------------------------------------------------------------------------
const D3DXVECTOR3& CWndMapEx::ConvertPosition( D3DXVECTOR3& vDestination, const D3DXVECTOR3& vSource, BYTE byLocationID )
{
	CRect rectLocationRealPosition = prj.m_MapInformationManager.FindRealPosition( byLocationID );
	FLOAT fLocationRealPositionX1 = static_cast< FLOAT >( rectLocationRealPosition.left );
	FLOAT fLocationRealPositionY1 = static_cast< FLOAT >( rectLocationRealPosition.top );
	FLOAT fLocationRealPositionX2 = static_cast< FLOAT >( rectLocationRealPosition.right );
	FLOAT fLocationRealPositionY2 = static_cast< FLOAT >( rectLocationRealPosition.bottom );
	FLOAT fRealMapWidth = fLocationRealPositionX2 - fLocationRealPositionX1;
	FLOAT fRealMapHeight = fLocationRealPositionY2 - fLocationRealPositionY1;
	FLOAT fRealPositionRatioX = ( vSource.x - fLocationRealPositionX1 ) / fRealMapWidth;
	FLOAT fRealPositionRatioY = ( fLocationRealPositionY2 - vSource.z ) / fRealMapHeight;
	vDestination.x = static_cast< FLOAT >( m_rectRevisedMapPosition.left ) + ( static_cast< FLOAT >( m_rectRevisedMapPosition.Width() ) * fRealPositionRatioX );
	vDestination.y = vSource.y;
	vDestination.z = static_cast< FLOAT >( m_rectRevisedMapPosition.top ) + ( static_cast< FLOAT >( m_rectRevisedMapPosition.Height() ) * fRealPositionRatioY );
	return vDestination;
}
//-----------------------------------------------------------------------------
const D3DXVECTOR3& CWndMapEx::ReconvertPosition( D3DXVECTOR3& vDestination, const D3DXVECTOR3& vSource, BYTE byLocationID )
{
	CRect rectLocationRealPosition = prj.m_MapInformationManager.FindRealPosition( byLocationID );
	FLOAT fLocationRealPositionX1 = static_cast< FLOAT >( rectLocationRealPosition.left );
	FLOAT fLocationRealPositionY1 = static_cast< FLOAT >( rectLocationRealPosition.top );
	FLOAT fLocationRealPositionX2 = static_cast< FLOAT >( rectLocationRealPosition.right );
	FLOAT fLocationRealPositionY2 = static_cast< FLOAT >( rectLocationRealPosition.bottom );
	FLOAT fRealMapWidth = fLocationRealPositionX2 - fLocationRealPositionX1;
	FLOAT fRealMapHeight = fLocationRealPositionY2 - fLocationRealPositionY1;
	vDestination.x = fLocationRealPositionX1 + ( ( fRealMapWidth * ( vSource.x - static_cast< FLOAT >( m_rectRevisedMapPosition.left ) ) ) / static_cast< FLOAT >( m_rectRevisedMapPosition.Width() ) );
	vDestination.y = vSource.y;
	vDestination.z = fLocationRealPositionY2 - ( ( fRealMapHeight * ( vSource.z - static_cast< FLOAT >( m_rectRevisedMapPosition.top ) ) ) / static_cast< FLOAT >( m_rectRevisedMapPosition.Height() ) );
	return vDestination;
}
//-----------------------------------------------------------------------------
const CRect& CWndMapEx::ReviseScriptRectInformation( CRect& rectDestination, const CRect& rectSource )
{
	// 스크립트의 원본 위치 데이터는 모두 1280 * 960 해상도를 기준으로 작성되어 있음
	// 새로운 맵 시스템에서는 맵 크기를 마음대로 변경할 수 있으므로, 그에 맞게끔 보정 계산을 해서 보정된 위치 데이터를 사용해야 함
	FLOAT fRevisedPositionXRatio = static_cast< FLOAT >( rectSource.left ) / SOURCE_MAP_SIZE_X * static_cast< FLOAT >( m_rectRevisedMapPosition.Width() );
	FLOAT fRevisedPositionYRatio = static_cast< FLOAT >( rectSource.top ) / SOURCE_MAP_SIZE_Y * static_cast< FLOAT >( m_rectRevisedMapPosition.Height() );
	FLOAT fRevisedWidthRatio = static_cast< FLOAT >( rectSource.Width() ) * m_fRevisedMapSizeRatio;
	FLOAT fRevisedHeightRatio = static_cast< FLOAT >( rectSource.Height() ) * m_fRevisedMapSizeRatio;
	rectDestination.left = static_cast< int >( static_cast< FLOAT >( m_rectRevisedMapPosition.left ) + fRevisedPositionXRatio );
	rectDestination.top = static_cast< int >( static_cast< FLOAT >( m_rectRevisedMapPosition.top ) + fRevisedPositionYRatio );
	rectDestination.right = rectDestination.left + static_cast< int >( fRevisedWidthRatio );
	rectDestination.bottom = rectDestination.top + static_cast< int >( fRevisedHeightRatio );
	return rectDestination;
}
//-----------------------------------------------------------------------------
BYTE CWndMapEx::GetMapArea( const D3DXVECTOR3& vPlayerPosition )
{
	BYTE byPlayerLocationID = CContinent::GetInstance()->GetTown( vPlayerPosition );
	if( byPlayerLocationID == CONT_NODATA )
	{
		byPlayerLocationID = CContinent::GetInstance()->GetContinent( vPlayerPosition );
	}

	if( byPlayerLocationID == TOWN_SAINCITY )
	{
		byPlayerLocationID = CContinent::GetInstance()->GetTown( vPlayerPosition );
	}
	if( byPlayerLocationID == TOWN_DARKEN )
	{
		byPlayerLocationID = CContinent::GetInstance()->GetTown( vPlayerPosition );
	}
	if( byPlayerLocationID == TOWN_FLARINENOSPLE )
	{
		byPlayerLocationID = CContinent::GetInstance()->GetTown( vPlayerPosition );
	}
	if( byPlayerLocationID == TOWN_ELIUN )
	{
		byPlayerLocationID = CContinent::GetInstance()->GetTown( vPlayerPosition );
	}

	return byPlayerLocationID;
}
//-----------------------------------------------------------------------------
void CWndMapEx::CalculateMaximumWindowTileLength( void )
{
	assert( ( m_nMinimumWindowTileWidth < g_Option.m_nResWidth ) && ( m_nMinimumWindowTileHeight < g_Option.m_nResHeight ) );
	int nWndTaskBarHeight = 0;
	if( g_WndMng.m_pWndTaskBar )
	{
		nWndTaskBarHeight = g_WndMng.m_pWndTaskBar->GetWindowRect().Height();
	}

	int nMaximumWindowTileNumberX = MINIMUM_WINDOW_TILE_NUMBER_X;
	while( ( WINDOW_TILE_TEXTURE_SIZE_XY * ( nMaximumWindowTileNumberX + 1 ) ) < g_Option.m_nResWidth )
	{
		++nMaximumWindowTileNumberX;
	}
	m_nMaximumWindowTileWidth = WINDOW_TILE_TEXTURE_SIZE_XY * nMaximumWindowTileNumberX;

	int nMaximumWindowTileNumberY = MINIMUM_WINDOW_TILE_NUMBER_Y;
	while( ( WINDOW_TILE_TEXTURE_SIZE_XY * ( nMaximumWindowTileNumberY + 1 ) ) < ( g_Option.m_nResHeight - nWndTaskBarHeight ) )
	{
		++nMaximumWindowTileNumberY;
	}
	m_nMaximumWindowTileHeight = WINDOW_TILE_TEXTURE_SIZE_XY * nMaximumWindowTileNumberY;
}
//-----------------------------------------------------------------------------
#endif // __CLIENT
#endif // __IMPROVE_MAP_SYSTEM