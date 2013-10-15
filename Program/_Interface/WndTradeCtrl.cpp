// WndBase.cpp: implementation of the CWndBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AppDefine.h"
#include "DPClient.h"
extern	CDPClient	g_DPlay;

#include "WndManager.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndTradeCtrl
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define MAX_TRADE 25
CTextureMng CWndTradeCtrl::m_textureMng;

CWndTradeCtrl::CWndTradeCtrl() 
{
	m_nWndColor    = D3DCOLOR_TEMP( 255,  0x5 << 3,  0x5 << 3,  0x5 << 3 );
	m_nFontColor   = D3DCOLOR_TEMP( 255, 255, 255, 255 );
	m_nSelectColor = D3DCOLOR_TEMP( 255, 255, 255, 0   );
	m_nCurSelect = -1;
	m_pFocusItem = NULL;
	m_pMover = NULL;
	m_bDrag = FALSE;
}
CWndTradeCtrl::~CWndTradeCtrl()
{
}
void CWndTradeCtrl::Create( DWORD dwListCtrlStyle, RECT& rect, CWndBase* pParentWnd, UINT nID )
{
	m_dwListCtrlStyle = dwListCtrlStyle;
	CWndBase::Create( m_dwListCtrlStyle|WBS_CHILD, rect, pParentWnd, nID );
}
void CWndTradeCtrl::InitItem( CMover* pMover )
{
	m_pMover = pMover;
	m_pFocusItem	= NULL;
}
void CWndTradeCtrl::LoadListBoxScript(LPCTSTR lpFileName) 
{
}	
void CWndTradeCtrl::OnInitialUpdate()
{
}
void CWndTradeCtrl::OnDraw(C2DRender* p2DRender) 
{
	CPoint pt( 3, 3 );
	CRect rect = GetClientRect();

	int nWidth = rect.Width() / 32;
	int nHeight = rect.Height() / 35;

	for( int i = 0; i < MAX_TRADE; i++ )
	{
		int x = i % 5;
		int y = i / 5;
		CItemBase* pItemBase = m_pMover->m_vtInfo.GetItem( i );
		if( pItemBase )
		{
			if( ((CItemElem*)pItemBase)->IsFlag( CItemElem::expired ) )
				pItemBase->GetTexture()->Render2( p2DRender, CPoint( x * 32, y * 32 ), D3DCOLOR_ARGB( 255, 255, 100, 100 ) );
			else
				pItemBase->GetTexture()->Render( p2DRender, CPoint( x * 32, y * 32 ) );
			
			CRect rectHittest = CRect( x * 32, y * 32 + 3, x * 32 + 32, y * 32 + 32 + 3);
			CPoint point = GetMousePoint();
			if( rectHittest.PtInRect( point ) )
			{
				CPoint point2 = point;
				ClientToScreen( &point2 );
				ClientToScreen( &rectHittest );

				g_WndMng.PutToolTip_Item( pItemBase, point2, &rectHittest );
			}
			if( i == m_nCurSelect )
				p2DRender->RenderRect( CRect( x * 32, y * 32, x * 32 + 32 - 2, y * 32 + 32 - 2 ), 0xffffffff );

			CItemElem* pItemElem = (CItemElem*)pItemBase;
			if( pItemElem->GetProp()->dwPackMax > 1 )
			{
				short n	= pItemElem->m_nItemNum;
				if( IsUsingItem( pItemBase ) )
					n = pItemBase->GetExtra();																		

				TCHAR szTemp[ 32 ];
				_stprintf( szTemp, "%d", n );
				CSize size = p2DRender->m_pFont->GetTextExtent( szTemp );
				p2DRender->TextOut( x * 32 + 32 - size.cx, y * 32 + 32 - size.cy, szTemp, 0xff1010ff );
			}
		}
	}
}
void CWndTradeCtrl::OnLButtonUp( UINT nFlags, CPoint point )
{
	ReleaseCapture();
	m_bDrag = FALSE;
}
void CWndTradeCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bDrag == FALSE )
		return;
	m_bDrag = FALSE;
}

BOOL CWndTradeCtrl::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CWndTradeGold* pWndTradeGold	= (CWndTradeGold*)g_WndMng.GetWndBase( APP_TRADE_GOLD );
	if( pWndTradeGold != NULL )
		return TRUE;

	int nDstId = -1;
	CRect rect = GetClientRect();
	int nWidth = rect.Width() / 32;
	int nHeight = rect.Height() / 35;
	CPoint pt( 3, 3 );

	if( pShortcut->m_dwType == ITYPE_ITEM && pShortcut->m_dwData == 0 )
	{
		CWndBase* pParent = (CWndBase*)GetParentWnd();
		pParent->OnChildNotify( WIN_ITEMDROP, m_nIdWnd, (LRESULT*)pShortcut ); 
		return TRUE;
	}
	else
	{
		CItemBase* pItemBase = g_pPlayer->GetItemId( pShortcut->m_dwId );
		if( pItemBase ) 
		{
#if __VER >= 9 // __CSC_VER9_1
//			ItemProp* pItemProp = pItemBase->GetProp();
			CItemElem* pItemElem = (CItemElem*)pItemBase;
			if( pItemElem->IsFlag( CItemElem::expired ) )
				return FALSE;
/*
			if(pItemProp->dwItemKind3 == IK3_EGG && pItemElem->m_pPet) //사망한 펫은 거래 불가
			{
				if(pItemElem->m_pPet->GetLife() <= 0)
					return FALSE;
			}
*/
#endif //__CSC_VER9_1
			if( ( (CItemElem*)pItemBase )->m_nItemNum > 1 )
			{
				for( int i = 0; i < MAX_TRADE; i++ )
				{
					int x = i % 5;
					int y = i / 5;
					rect.SetRect( x * 32, y * 32, x * 32 + 32, y * 32 + 32 );
					if( rect.PtInRect( point ) && m_pMover->m_vtInfo.GetItem( i ) == NULL )
					{
						pShortcut->m_dwData = i + 100;
						CWndBase* pParent = (CWndBase*)GetParentWnd();
						pParent->OnChildNotify( WIN_ITEMDROP, m_nIdWnd, (LRESULT*)pShortcut ); 
					}
				}
				return TRUE;
			}
		}
	}

	for( int i = 0; i < MAX_TRADE; i++ )
	{
		int x = i % 5;
		int y = i / 5;
		rect.SetRect( x * 32, y * 32, x * 32 + 32, y * 32 + 32 );
		if( rect.PtInRect( point ) && m_pMover->m_vtInfo.GetItem( i ) == NULL )
		{
			g_DPlay.SendTradePut( i, (BYTE)( pShortcut->m_dwType ), (BYTE)( pShortcut->m_dwId ) );
			return TRUE;
		}
	}
	return TRUE;
}

void CWndTradeCtrl::OnLButtonDown( UINT nFlags, CPoint point )
{
}
void CWndTradeCtrl::OnRButtonUp( UINT nFlags, CPoint point )
{
}
void CWndTradeCtrl::OnRButtonDown( UINT nFlags, CPoint point )
{
}
void CWndTradeCtrl::OnLButtonDblClk( UINT nFlags, CPoint point )
{
}
void CWndTradeCtrl::OnRButtonDblClk( UINT nFlags, CPoint point)
{
}
void CWndTradeCtrl::OnSize( UINT nType, int cx, int cy )
{
	CRect rect = GetWindowRect();
	rect.left = rect.right - 15;

	CWndBase::OnSize( nType, cx, cy);
}
void CWndTradeCtrl::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect( 3, 3 );

	if( bOnSize )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
}
void CWndTradeCtrl::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();

	DWORD dwColor1 = D3DCOLOR_ARGB( 100, 0, 0,  0 );//D3DCOLOR_TEMP( 255,   0,   0,  50 );//
	DWORD dwColor2 = D3DCOLOR_ARGB( 255, 240, 240,  240 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//
	DWORD dwColor3 = D3DCOLOR_ARGB( 100, 200, 200,  200 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//

	p2DRender->RenderFillRect ( rect, dwColor1 );
	p2DRender->RenderRoundRect( rect, dwColor2 );
	rect.DeflateRect( 1 , 1 );
	p2DRender->RenderRect( rect, dwColor2 );
	rect.DeflateRect( 1 , 1 );
	p2DRender->RenderRect( rect, dwColor3 );
}

BOOL CWndTradeCtrl::OnEraseBkgnd( C2DRender* p2DRender )
{
	return TRUE;
}

BOOL CWndTradeCtrl::SetItem( const LVITEM* pItem )
{
	if( pItem->iItem < m_aItems.GetSize() && m_aItems.GetAt( pItem->iItem ) == NULL ) 
		return FALSE; // 존재하지 않는다.
	LVITEM* pItems = (LVITEM*)m_aItems.GetAt( pItem->iItem );
	memcpy( &pItems[ pItem->iSubItem ], pItem, sizeof( LVITEM ) );
	pItems[ pItem->iSubItem ].pszText = new _TCHAR[ _tcslen( pItem->pszText ) + sizeof( _TCHAR ) ];
	_tcscpy( pItems[ pItem->iSubItem ].pszText, pItem->pszText );
	return TRUE;
}
int CWndTradeCtrl::InsertItem( const LVITEM* pItem )
{
	if( pItem->iItem < m_aItems.GetSize() && m_aItems.GetAt( pItem->iItem ) ) 
		return -1; // 이미 존재한다.
	LVITEM* pNewItems = new LVITEM[ 3 ]; // m_aColumns.GetSize()
	memcpy( &pNewItems[ pItem->iSubItem ], pItem, sizeof( LVITEM ) );
	pNewItems[ pItem->iSubItem ].pszText = new _TCHAR[ _tcslen( pItem->pszText ) + sizeof( _TCHAR ) ];
	_tcscpy( pNewItems[ pItem->iSubItem ].pszText, pItem->pszText );
	m_aItems.SetAtGrow( pItem->iItem, (void*)pNewItems );
	return pItem->iItem;
}
int CWndTradeCtrl::GetItemCount() const
{
	return m_aItems.GetSize();
}
int CWndTradeCtrl::InsertColumn( int nCol, const LVCOLUMN* pColumn )
{
	LVCOLUMN* pNewColumn = new LVCOLUMN;
	memcpy( pNewColumn, pColumn, sizeof( LVCOLUMN ) );
	pNewColumn->pszText = new _TCHAR[ _tcslen( pColumn->pszText ) ];
	_tcscpy( pNewColumn->pszText, pColumn->pszText );
	if( nCol < m_aColumns.GetSize() && m_aColumns.GetAt( nCol ) ) 
		return -1; // 이미 존재한다.
	m_aColumns.SetAtGrow( nCol, (void*)pNewColumn );
	return nCol;
}

