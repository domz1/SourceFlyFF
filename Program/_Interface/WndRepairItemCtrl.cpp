#include "stdafx.h"
#include "WndRepairItemCtrl.h"
#include "WndRepairItem.h"
#include "WndManager.h"


CTextureMng	CWndRepairItemCtrl::m_textureMng;

CWndRepairItemCtrl::CWndRepairItemCtrl()
{
	m_nWndColor		= D3DCOLOR_TEMP( 255,  0x5 << 3,  0x5 << 3,  0x5 << 3 );
	m_nFontColor	= D3DCOLOR_TEMP( 255, 255, 255, 255 );
	m_nSelectColor	= D3DCOLOR_TEMP( 255, 255, 255, 0   );
	m_nCurSel	= -1;
	m_pFocusItem	= NULL;

	m_pdwIdRepair	= NULL;
	m_bDrag		= FALSE;
}

void CWndRepairItemCtrl::Create( DWORD dwListCtrlStyle, RECT & rect, CWndBase* pParentWnd, UINT nID )
{
	m_dwListCtrlStyle	= dwListCtrlStyle;
	CWndBase::Create( m_dwListCtrlStyle | WBS_CHILD, rect, pParentWnd, nID );
}


void CWndRepairItemCtrl::InitItem( DWORD* pdwIdRepair )
{

	m_pdwIdRepair	= pdwIdRepair;
	m_pFocusItem	= NULL;
}

void CWndRepairItemCtrl::OnInitialUpdate( void )
{

}
	
void CWndRepairItemCtrl::OnDraw( C2DRender* p2DRender )
{
	if( NULL == m_pdwIdRepair )
		return;

	CMover* pMover	= CMover::GetActiveMover();
	if( !pMover )
		return;

	CRect rect	= GetClientRect();
	int nWidth	= rect.Width() / 32;
	int nHeight		= rect.Height() / 35;

	for( int i = 0; i < MAX_REPAIRINGITEM; i++ )
	{
		int x	= i % 5;
		int y	= i / 5;

		if( m_pdwIdRepair[i] != NULL_ID )
		{

			CItemElem* pItemElem	= pMover->m_Inventory.GetAtId( m_pdwIdRepair[i] );
			if( pItemElem )
			{
				pItemElem->GetTexture()->Render( p2DRender, CPoint( x * 32 + 6, y * 32 + 10 ) );
				CRect rectHittest	= CRect( x * 32, y * 32 + 3, x * 32 + 32, y * 32 + 32 + 3 );
				CPoint point	= GetMousePoint();
				if( rectHittest.PtInRect( point ) )
				{
					CPoint point2 = point;
					ClientToScreen( &point2 );
					ClientToScreen( &rectHittest );
					g_WndMng.PutToolTip_Item( pItemElem, point2, &rectHittest, 276 ); // APP_REPAIR
				}
				if( i == m_nCurSel )
					p2DRender->RenderRect( CRect( x * 32 + 7, y * 32 + 11, x * 32 + 32 + 5, y * 32 + 32 + 9 ), 0xff00ffff );
			}
		}
	}
}

void CWndRepairItemCtrl::OnMouseMove( UINT nFlags, CPoint point )
{
}

BOOL CWndRepairItemCtrl::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	int nDstId	= -1;
	CRect rect	= GetClientRect();
	int nWidth	= rect.Width() / 32;
	int nHeight		= rect.Height() / 35;
	CPoint pt( 3, 3 );
	if( pShortcut->m_dwType == ITYPE_ITEM && pShortcut->m_dwData == 0 )
	{
		return TRUE;
	}
	else
	{
		CItemBase* pItemBase = g_pPlayer->GetItemId( pShortcut->m_dwId );
		if( pItemBase )
		{
			for( int i = 0; i < MAX_REPAIRINGITEM; i++ )
			{
				int x	= i % 5;
				int y	= i / 5;
				rect.SetRect( x * 32, y * 32, x * 32 + 32, y * 32 + 32 );

				if( rect.PtInRect( point ) && m_pdwIdRepair[i] == NULL_ID )
				{
					pShortcut->m_dwData		= i + 100;
					CWndBase* pParent	= (CWndBase*)GetParentWnd();
					pParent->OnChildNotify( WIN_ITEMDROP, m_nIdWnd, (LRESULT*)pShortcut );
				}
			}
			return TRUE;
		}
	}
	return TRUE;
}

void CWndRepairItemCtrl::OnLButtonDown( UINT nFlags, CPoint point )
{
	if( GetAsyncKeyState( VK_LCONTROL ) & 0x8000 )
		return;

	if( CWndBase::m_GlobalShortcut.IsEmpty() == FALSE )
		return;

	int nItem	= HitTest( point );
	if( nItem == -1 )
		return;

	CItemElem*	pItemElem	= g_pPlayer->m_Inventory.GetAtId( m_pdwIdRepair[nItem] );
	if( pItemElem )
	{
		m_pFocusItem	= pItemElem;
		m_nCurSel	= nItem;
	}
}

void CWndRepairItemCtrl::OnLButtonUp( UINT nFlags, CPoint point )
{

}

void CWndRepairItemCtrl::OnRButtonUp( UINT nFlags, CPoint point )
{

}

void CWndRepairItemCtrl::OnRButtonDown( UINT nFlags, CPoint point )
{

}

void CWndRepairItemCtrl::OnLButtonDblClk( UINT nFlags, CPoint point )
{

}

void CWndRepairItemCtrl::OnRButtonDblClk( UINT nFlags, CPoint point )
{

}

void CWndRepairItemCtrl::OnSize( UINT nType, int cx, int cy )
{
	CWndBase::OnSize( nType, cx, cy );
}

void CWndRepairItemCtrl::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow	= rectWnd;
	m_rectClient	= m_rectWindow;

	if( bOnSize )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
}

void CWndRepairItemCtrl::PaintFrame( C2DRender* p2DRender )
{
	CRect rect	= GetWindowRect();

	DWORD dwColor1	= D3DCOLOR_ARGB( 100, 0, 0,  0 );
	DWORD dwColor2	= D3DCOLOR_ARGB( 255, 240, 240,  240 );
	DWORD dwColor3	= D3DCOLOR_ARGB( 100, 200, 200,  200 );

	p2DRender->RenderFillRect ( rect, dwColor1 );
	p2DRender->RenderRoundRect( rect, dwColor2 );
	rect.DeflateRect( 1 , 1 );
	p2DRender->RenderRect( rect, dwColor2 );
	rect.DeflateRect( 1 , 1 );
	p2DRender->RenderRect( rect, dwColor3 );
}

BOOL CWndRepairItemCtrl::OnEraseBkgnd( C2DRender* p2DRender )
{
	return TRUE;
}

int CWndRepairItemCtrl::HitTest( CPoint point )
{
	CRect rect	= GetClientRect();
	int nWidth	= rect.Width() / 32;
	int nHeight		= rect.Height() / 35;

	for( int i = 0; i < MAX_VENDITEM; i++ )
	{
		int x	= i % 5;
		int y	= i / 5;
		rect.SetRect( x * 32, y * 32, x * 32 + 32, y * 32 + 32 );
		if( rect.PtInRect( point ) )
			return i;
	}
	return -1;
}
