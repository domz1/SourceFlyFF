#include "StdAfx.h"
#include "WndQuestQuickInfo.h"
#if __VER >= 15 /* __IMPROVE_QUEST_INTERFACE */ && defined( __CLIENT )
#include "ResData.h"
#include "defineText.h"
#include "party.h"
#include "WndQuest.h"
#include "WorldMap.h"
#endif // defined( __IMPROVE_QUEST_INTERFACE ) && defined( __CLIENT )

#if __VER >= 15 /* __IMPROVE_QUEST_INTERFACE */ && defined( __CLIENT )
//-----------------------------------------------------------------------------
CWndQITreeCtrl::CWndQITreeCtrl( void )
{
}
//-----------------------------------------------------------------------------
CWndQITreeCtrl::~CWndQITreeCtrl( void )
{
}
//-----------------------------------------------------------------------------
void CWndQITreeCtrl::OnLButtonDown( UINT nFlags, CPoint point )
{
	LPTREEITEM pTreeItem = NULL;
	for( int i = 0; i < GetTreeItemArray()->GetSize(); ++i )
	{
		pTreeItem = ( LPTREEITEM )GetTreeItemArray()->GetAt( i );
		if( GetMemberCheckingMode() == TRUE )
		{
			CWndButton* pWndCheckBox = pTreeItem->m_lpTreeElem->m_pWndCheckBox;
			LPTREEELEM pParentTreeElem = pTreeItem->m_lpTreeElem->m_lpParent;
			if( pWndCheckBox && pParentTreeElem && pParentTreeElem->m_bOpen == FALSE )
			{
				if( pWndCheckBox->IsWindowEnabled() == TRUE )
					pWndCheckBox->EnableWindow( FALSE );
				if( pWndCheckBox->IsVisible() == TRUE )
					pWndCheckBox->SetVisible( FALSE );
			}
		}
		LPTREEELEM pTreeElem = pTreeItem->m_lpTreeElem;
		CRect rect = pTreeItem->m_rect;
		SIZE size = m_pTexButtOpen->m_size;
		rect.left -= GetCategoryTextSpace();
		rect.SetRect( rect.left, rect.top, rect.left + size.cx, rect.top + size.cy );
		if( rect.PtInRect( point ) )
		{
			pTreeElem->m_bOpen = !pTreeElem->m_bOpen;
			CTreeInformation* pTreeInformation = g_QuestTreeInfoManager.GetTreeInformation( pTreeElem->m_dwData );
			if( pTreeInformation )
			{
				pTreeInformation->SetOpen( pTreeElem->m_bOpen );
				SetFocusElem( pTreeItem->m_lpTreeElem );
			}
			if( pTreeElem->m_bOpen == TRUE )
				( ( CWndQuestQuickInfo* )m_pParentWnd )->ExtendAppletHeight();
		}

		if( pTreeItem->m_rect.PtInRect( point ) )
		{
			if( pTreeElem->m_ptrArray.GetSize() > 0 || pTreeElem->m_lpParent == NULL )
			{
				DWORD dwQuestID = pTreeElem->m_dwData;
				if( g_WndMng.m_pWndQuestDetail && dwQuestID == g_WndMng.m_pWndQuestDetail->GetQuestID() )
					SAFE_DELETE( g_WndMng.m_pWndQuestDetail )
				else
				{
					SAFE_DELETE( g_WndMng.m_pWndQuestDetail )
					g_WndMng.m_pWndQuestDetail = new CWndQuestDetail( dwQuestID );
					g_WndMng.m_pWndQuestDetail->Initialize( NULL );
					LPQUEST lpQuest = g_pPlayer->FindQuest( dwQuestID );
					BOOL bComplete = dwQuestID != -1 && g_pPlayer->IsCompleteQuest( dwQuestID );
					g_WndMng.m_pWndQuestDetail->UpdateQuestDetailText( dwQuestID, lpQuest, bComplete );
				}
				SetQuestDestinationInformation( dwQuestID, 0 );
			}
			else
			{
				DWORD dwGoalTextID = SetQuestDestinationInformation( pTreeElem->m_lpParent->m_dwData, pTreeElem->m_dwData );
				ProcessQuestDestinationWorldMap( dwGoalTextID );
			}
			SetFocusElem( pTreeItem->m_lpTreeElem );
		}
	}

	if( ( ( CWndQuestQuickInfo* )m_pParentWnd )->GetActiveGroundClick() == TRUE )
	{
		if( g_WndMng.m_pWndWorld )
		{
			CRect rect = GetWindowRect( TRUE );
			ClientToScreen( &rect );
			g_WndMng.m_pWndWorld->m_bLButtonDown = TRUE;
			g_WndMng.m_pWndWorld->OnLButtonDown( nFlags, CPoint( point.x + rect.left, point.y + rect.top ) );
		}
	}
}
//-----------------------------------------------------------------------------
void CWndQITreeCtrl::OnRButtonDown( UINT nFlags, CPoint point )
{
	if( ( ( CWndQuestQuickInfo* )m_pParentWnd )->GetActiveGroundClick() == TRUE )
	{
		if( g_WndMng.m_pWndWorld )
		{
			CRect rect = GetWindowRect( TRUE );
			ClientToScreen( &rect );
			g_WndMng.m_pWndWorld->m_bRButtonDown = TRUE;
			g_WndMng.m_pWndWorld->OnRButtonDown( nFlags, CPoint( point.x + rect.left, point.y + rect.top ) );
		}
	}
}
//-----------------------------------------------------------------------------
void CWndQITreeCtrl::PaintTree( C2DRender* p2DRender, CPoint& pt, CPtrArray& ptrArray )
{
	LPTREEELEM pTreeElem = NULL;
	LPTREEITEM pTreeItem = NULL;
	for(int i = 0; i < ptrArray.GetSize(); ++i )
	{
		pTreeElem = ( LPTREEELEM )ptrArray.GetAt( i );
		pTreeItem = new TREEITEM;
		CSize sizeStr( 0, 0 );
		p2DRender->m_pFont->GetTextExtent( pTreeElem->m_strKeyword, &sizeStr );
		int nRectLeft = pt.x + GetCategoryTextSpace();
		int nRectTop = pt.y;
		int nRectRight = pt.x + GetCategoryTextSpace() + sizeStr.cx;
		int nRectBottom = pt.y + sizeStr.cy;
		pTreeItem->m_rect.SetRect( nRectLeft, nRectTop, nRectRight, nRectBottom );
		SetTreeItemsMaxWidth( ( nRectRight > GetTreeItemsMaxWidth() ) ? nRectRight : GetTreeItemsMaxWidth() );
		if( GetMemberCheckingMode() == TRUE && pTreeElem->m_pWndCheckBox )
		{
			CWndButton* pWndCheckBox = pTreeElem->m_pWndCheckBox;
			if( pTreeElem->m_ptrArray.GetSize() == 0 )
			{
				pWndCheckBox->EnableWindow( TRUE );
				pWndCheckBox->SetVisible( TRUE );
				pWndCheckBox->SetWndRect( CRect( pt.x, pt.y - 1, pt.x + CWndTreeCtrl::CHECK_BOX_SIZE_XY, pt.y + CWndTreeCtrl::CHECK_BOX_SIZE_XY - 1 ) );
			}
		}
		pTreeItem->m_lpTreeElem = pTreeElem;
		GetTreeItemArray()->Add( pTreeItem );
		if( pTreeElem->m_ptrArray.GetSize() )
		{
			if( pTreeElem->m_bOpen )
				p2DRender->RenderTexture( pt, m_pTexButtOpen );
			else
				p2DRender->RenderTexture( pt, m_pTexButtClose );
		}

		CD3DFont* pOldFont = p2DRender->GetFont();
		if( ( ( CWndQuestQuickInfo* )m_pParentWnd )->m_nAlphaCount == 0 )
		{
			static const int REVISING_VALUE = 2;
			nRectLeft -= REVISING_VALUE;
			nRectTop -= REVISING_VALUE;
			p2DRender->SetFont( CWndBase::m_Theme.m_pFontWorld );
		}
		else
			p2DRender->SetFont( CWndBase::m_Theme.m_pFontText );
		if( GetFocusElem() == pTreeElem )
		{
			pTreeElem->m_strKeyword.SetColor( pTreeElem->m_dwSelectColor );
			p2DRender->TextOut_EditString( nRectLeft, nRectTop, pTreeElem->m_strKeyword );
		}
		else
		{
			pTreeElem->m_strKeyword.SetColor( pTreeElem->m_dwColor );
			p2DRender->TextOut_EditString( nRectLeft, nRectTop, pTreeElem->m_strKeyword );
		}
		p2DRender->SetFont( pOldFont );

		pt.y += GetFontHeight();
		if( pTreeElem->m_ptrArray.GetSize() && pTreeElem->m_bOpen == TRUE )
		{
			pt.x += GetTreeTabWidth();
			PaintTree( p2DRender, pt, pTreeElem->m_ptrArray );
			pt.x -= GetTreeTabWidth();
		}
	}
}
//-----------------------------------------------------------------------------
CWndQuestQuickInfo::CWndQuestQuickInfo( void ) : 
m_nTreeInformationListSize( 0 ), 
m_bOnSurface( FALSE ), 
m_bActiveGroundClick( FALSE ), 
m_tmOld( g_tmCurrent ), 
m_bUpdateText( FALSE )
{
	m_nAlphaCount = 0;
	m_bNoCloseButton = TRUE;
}
//-----------------------------------------------------------------------------
CWndQuestQuickInfo::~CWndQuestQuickInfo( void )
{
	DeleteTree();
}
//-----------------------------------------------------------------------------
BOOL CWndQuestQuickInfo::Initialize( CWndBase* pWndParent, DWORD dwType )
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_QUEST_QUICK_INFO, WBS_MANAGER, CPoint( 0, 0 ), pWndParent );
}
//-----------------------------------------------------------------------------
void CWndQuestQuickInfo::OnInitialUpdate( void )
{
	CWndNeuz::OnInitialUpdate();

	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_TREECTRL_QUICK_INFO );
	m_CWndQITreeCtrl.Create( lpWndCtrl->dwWndStyle, lpWndCtrl->rect, this, lpWndCtrl->dwWndId );
	m_CWndQITreeCtrl.SetTreeTabWidth( 6 );
	m_CWndQITreeCtrl.SetCategoryTextSpace( 12 );

	static const int TOP_MARGIN = 220;
	CRect rectRoot = m_pWndRoot->m_rectWindow;
	Move( rectRoot.right - m_rectWindow.Width(), TOP_MARGIN );
}
//-----------------------------------------------------------------------------
BOOL CWndQuestQuickInfo::Process( void )
{
	CRect rtQuickInfoApplet = GetWindowRect();
	static const int REVISION_RECT_LEFT = 4; // Rect가 실제 윈도우보다 4픽셀 정도 오른쪽으로 치우쳐져 있는 것을 보정
	static const int REVISION_RECT_UP = 22; // Rect가 실제 윈도우보다 22픽셀 정도 내려가 있는 것을 보정
	rtQuickInfoApplet.left -= REVISION_RECT_LEFT;
	rtQuickInfoApplet.right -= REVISION_RECT_LEFT;
	rtQuickInfoApplet.top -= REVISION_RECT_UP;
	rtQuickInfoApplet.bottom -= REVISION_RECT_UP;
	CPoint ptMouse = GetMousePoint();
	m_bOnSurface = ( rtQuickInfoApplet.PtInRect( ptMouse ) == TRUE || m_nResizeDir > 0 || m_bPickup == TRUE ) ? TRUE : FALSE;

	float fDelta = ( static_cast< float >( g_tmCurrent - m_tmOld ) / 1000.0f );
	m_tmOld = g_tmCurrent;

	static const float ANIMATION_SPEED = 500.0f;
	if( m_bOnSurface == TRUE )
	{
		m_nAlphaCount += static_cast< int >( ANIMATION_SPEED * fDelta );
		if( m_nAlphaCount > m_nAlpha )
			m_nAlphaCount = m_nAlpha;
	}
	else
	{
		m_nAlphaCount -= static_cast< int >( ANIMATION_SPEED * fDelta );
		if( m_nAlphaCount < 0 )
			m_nAlphaCount = 0;
	}

	static const DWORD NO_SURFACE_CATEGORY_TEXT_COLOR = D3DCOLOR_ARGB( 255, 255, 255, 0 );
	static const DWORD SURFACE_CATEGORY_TEXT_COLOR = D3DCOLOR_ARGB( 255, 161, 28, 70 );
	static const DWORD NO_SURFACE_NORMAL_TEXT_COLOR = D3DCOLOR_ARGB( 255, 255, 255, 255 );
	static const DWORD SURFACE_NORMAL_TEXT_COLOR = D3DCOLOR_ARGB( 255, 121, 80, 255 );
	static const DWORD NO_SURFACE_SELECTED_CATEGORY_TEXT_COLOR = D3DCOLOR_ARGB( 255, 255, 0, 0 );
	static const DWORD SURFACE_SELECTED_CATEGORY_TEXT_COLOR = D3DCOLOR_ARGB( 255, 255, 0, 0 );
	static const DWORD NO_SURFACE_SELECTED_TEXT_COLOR = D3DCOLOR_ARGB( 255, 255, 0, 0 );
	static const DWORD SURFACE_SELECTED_TEXT_COLOR = D3DCOLOR_ARGB( 255, 255, 0, 0 );

	CWndTreeCtrl* pWndTreeCtrl = ( CWndTreeCtrl* )GetDlgItem( WIDC_TREECTRL_QUICK_INFO );
	assert( pWndTreeCtrl );
	pWndTreeCtrl->SetTextColor( 
		MakeTextColor( NO_SURFACE_CATEGORY_TEXT_COLOR, SURFACE_CATEGORY_TEXT_COLOR, m_nAlphaCount, m_nAlpha ), 
		MakeTextColor( NO_SURFACE_NORMAL_TEXT_COLOR, SURFACE_NORMAL_TEXT_COLOR, m_nAlphaCount, m_nAlpha ), 
		MakeTextColor( NO_SURFACE_SELECTED_CATEGORY_TEXT_COLOR, SURFACE_SELECTED_CATEGORY_TEXT_COLOR, m_nAlphaCount, m_nAlpha ), 
		MakeTextColor( NO_SURFACE_SELECTED_TEXT_COLOR, SURFACE_SELECTED_TEXT_COLOR, m_nAlphaCount, m_nAlpha ) );

	const int nCheckedQuestSize = g_pPlayer->m_nCheckedQuestSize;
	if( nCheckedQuestSize != m_nTreeInformationListSize || m_bUpdateText == TRUE )
	{
		m_bUpdateText = FALSE;
		pWndTreeCtrl->DeleteAllItems();

		if( nCheckedQuestSize > 0 )
		{
			for( int i = 0; i < nCheckedQuestSize; ++i )
			{
				if( IsVisible() != TRUE )
					SetVisible( TRUE );

				DWORD dwQuestID = g_pPlayer->m_aCheckedQuest[ i ];
				QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( dwQuestID );
				if( pQuestProp == NULL )
					continue;
				LPTREEELEM lpTreeElem = pWndTreeCtrl->InsertItem( NULL, 
																  pQuestProp->m_szTitle, 
																  dwQuestID, 
																  TRUE, 
																  FALSE, 
																  MakeTextColor( NO_SURFACE_CATEGORY_TEXT_COLOR, SURFACE_CATEGORY_TEXT_COLOR, m_nAlphaCount, m_nAlpha ), 
																  MakeTextColor( NO_SURFACE_SELECTED_CATEGORY_TEXT_COLOR, SURFACE_SELECTED_CATEGORY_TEXT_COLOR, m_nAlphaCount, m_nAlpha ) );
				MakeQuestConditionItems( dwQuestID, 
										 pWndTreeCtrl, 
										 FALSE, 
										 lpTreeElem, 
										 MakeTextColor( NO_SURFACE_NORMAL_TEXT_COLOR, SURFACE_NORMAL_TEXT_COLOR, m_nAlphaCount, m_nAlpha ), 
										 MakeTextColor( NO_SURFACE_NORMAL_TEXT_COLOR, SURFACE_NORMAL_TEXT_COLOR, m_nAlphaCount, m_nAlpha ), 
										 MakeTextColor( NO_SURFACE_SELECTED_TEXT_COLOR, SURFACE_SELECTED_TEXT_COLOR, m_nAlphaCount, m_nAlpha ) );

				CTreeInformation* pTreeInformation = g_QuestTreeInfoManager.GetTreeInformation( dwQuestID );
				lpTreeElem->m_bOpen = pTreeInformation ? pTreeInformation->GetOpen() : TRUE;
			}

			if( nCheckedQuestSize > m_nTreeInformationListSize )
				ExtendAppletHeight();
		}
		else
		{
			if( this->IsVisible() != FALSE )
				this->SetVisible( FALSE );
		}
	}
	m_nTreeInformationListSize = nCheckedQuestSize;

	return TRUE;
}
//-----------------------------------------------------------------------------
void CWndQuestQuickInfo::PaintFrame( C2DRender* p2DRender )
{
	if( m_pTexture == NULL )
		return;

	RenderWnd();
	if( IsWndStyle( WBS_CAPTION ) )
	{
#ifdef __WINDOWS_V19
			CPoint newpoint;
			CSize sizeTitle = p2DRender->m_pFont->GetTextExtent( m_strTitle );
			CRect rect	= GetWindowRect();
			newpoint = (rect.Width() - sizeTitle.cx) / 2;

			CD3DFont* pOldFont = p2DRender->GetFont();
			p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndNewTitle );
            p2DRender->TextOut( newpoint.x , 8, m_strTitle, 0xffffffff );
			p2DRender->SetFont( pOldFont );
#else
		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle );
		p2DRender->TextOut( 10, 4, m_strTitle, ( ( m_dwColor & 0x00ffffff ) | ( m_nAlphaCount << 24 ) ) );
		p2DRender->SetFont( pOldFont );
#endif//__WINDOWS_V19
	}
}
//-----------------------------------------------------------------------------
void CWndQuestQuickInfo::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	static const int TILE_TEXTURE_SIZE_XY = 16; // 타일 텍스처 사이즈
	static const int MINIMUM_TILE_NUMBER_X = 8; // 가장 작은 윈도우일 때, 타일 텍스처의 가로 크기
	static const int MINIMUM_TILE_NUMBER_Y = 6; // 가장 작은 윈도우일 때, 타일 텍스처의 세로 크기
	static const int MAXIMUM_TILE_NUMBER_X = 48; // 가장 큰 윈도우일 때, 타일 텍스처의 가로 크기
	static const int MAXIMUM_TILE_NUMBER_Y = 34; // 가장 큰 윈도우일 때, 타일 텍스처의 세로 크기
	AdjustMinRect( &rectWnd, TILE_TEXTURE_SIZE_XY * MINIMUM_TILE_NUMBER_X, TILE_TEXTURE_SIZE_XY * MINIMUM_TILE_NUMBER_Y );
	AdjustMaxRect( &rectWnd, TILE_TEXTURE_SIZE_XY * MAXIMUM_TILE_NUMBER_X, TILE_TEXTURE_SIZE_XY * MAXIMUM_TILE_NUMBER_Y );

	CWndNeuz::SetWndRect( rectWnd, bOnSize );
}
//-----------------------------------------------------------------------------
void CWndQuestQuickInfo::OnSize( UINT nType, int cx, int cy )
{
	CRect rtQuickInfoApplet = GetClientRect();
	CWndTreeCtrl* pWndTreeCtrl = ( CWndTreeCtrl* )GetDlgItem( WIDC_TREECTRL_QUICK_INFO );
	pWndTreeCtrl->SetWndRect( rtQuickInfoApplet );

	CWndNeuz::OnSize( nType, cx, cy );
}
//-----------------------------------------------------------------------------
void CWndQuestQuickInfo::SetUpdateTextSwitch( BOOL bUpdateText )
{
	m_bUpdateText = bUpdateText;
}
//-----------------------------------------------------------------------------
void CWndQuestQuickInfo::ExtendAppletHeight( void )
{
	CWndTreeCtrl* pWndTreeCtrl = ( CWndTreeCtrl* )GetDlgItem( WIDC_TREECTRL_QUICK_INFO );
	if( pWndTreeCtrl == NULL )
		return;

	int nTreeItemsHeight = pWndTreeCtrl->GetTreeItemsMaxHeight();
	CRect rtQuickInfoApplet = GetWndRect();
	CRect rtQuickInfoTree = pWndTreeCtrl->GetWndRect();
	int nMarginHeight = rtQuickInfoApplet.Height() - rtQuickInfoTree.Height();
	if( nTreeItemsHeight > rtQuickInfoTree.Height() )
		SetWndSize( rtQuickInfoApplet.Width(), nTreeItemsHeight + nMarginHeight + 4 );
}
//-----------------------------------------------------------------------------
BOOL CWndQuestQuickInfo::GetActiveGroundClick( void ) const
{
	return m_bActiveGroundClick;
}
//-----------------------------------------------------------------------------
void CWndQuestQuickInfo::DeleteTree( void )
{
	CWndTreeCtrl* pWndTreeCtrl = ( CWndTreeCtrl* )GetDlgItem( WIDC_TREECTRL_QUICK_INFO );
	if( pWndTreeCtrl )
		pWndTreeCtrl->DeleteAllItems();
}
//-----------------------------------------------------------------------------
#endif // defined( __IMPROVE_QUEST_INTERFACE ) && defined( __CLIENT )