#include "stdafx.h"
#include "AppDefine.h"
#include "wndvendorctrl.h"
#include "wndvendor.h"
#include "DPClient.h"
#include "defineText.h"
extern	CDPClient	g_DPlay;

#include "WndManager.h"



CTextureMng	CWndVendorCtrl::m_textureMng;

CWndVendorCtrl::CWndVendorCtrl()
{
	m_nWndColor		= D3DCOLOR_TEMP( 255,  0x5 << 3,  0x5 << 3,  0x5 << 3 );
	m_nFontColor	= D3DCOLOR_TEMP( 255, 255, 255, 255 );
	m_nSelectColor	= D3DCOLOR_TEMP( 255, 255, 255, 0   );
	m_nCurSel	= -1;
	m_pFocusItem	= NULL;
	m_pMover    = NULL;
	m_bDrag		= FALSE;
}

void CWndVendorCtrl::Create( DWORD dwListCtrlStyle, RECT & rect, CWndBase* pParentWnd, UINT nID )
{
	m_dwListCtrlStyle	= dwListCtrlStyle;
	CWndBase::Create( m_dwListCtrlStyle | WBS_CHILD, rect, pParentWnd, nID );
}

void CWndVendorCtrl::InitVendor( CMover* pMover )
{
	m_pMover	 = pMover;
	m_pFocusItem = NULL;
}

CItemBase* CWndVendorCtrl::GetItem( BYTE i )
{
	ASSERT( m_pMover );
	return m_pMover->m_vtInfo.GetItem( i );
}

void CWndVendorCtrl::OnInitialUpdate( void )
{
	CRect rect	= GetWindowRect();
	rect.left	= rect.right - 15;
	
	m_pTex = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "WndPosMark.tga" ), 0xffff00ff );
}

#if __VER >= 8 // __S8_VENDOR_REVISION
CString CWndVendorCtrl::GetNumberFormatSelling( LPCTSTR szNumber )
{
	ASSERT( szNumber );
	
	CString str;
	int nCommaCount = 0;
	
	int nLength = lstrlen( szNumber );	
	str.Insert(0, prj.GetText( TID_GAME_SELLING_COST_0 ) );
	for( int i=nLength-1; i>=0; --i)
	{
		str.Insert(0, szNumber[i]);
		if( ((nLength-i) % 4) == 0 && i != 0 )
		{
			DWORD dwText = TID_GAME_SELLING_COST_1;
			if( nCommaCount != 0 )
				dwText = TID_GAME_SELLING_COST_2;
			str.Insert(0, prj.GetText( dwText ) );
			++nCommaCount;
		}
	}
	return str;
}

#define	MAX_VENDOR_ITEM_TEXT_EXTENT		108

void CWndVendorCtrl::OnDrawItemInfo( C2DRender* p2DRender, CItemElem* pItemElem, int nX, int nY )
{
	// 아이템 이름 및 가격
	CString strItem;
	if( pItemElem->GetAbilityOption() )
		strItem.Format( "%s +%d", pItemElem->GetName(), pItemElem->GetAbilityOption());
	else
	{
		strItem.Format( "%s", pItemElem->GetName() );
	}
	
	int nLen	= strItem.GetLength();
	CSize size	= p2DRender->m_pFont->GetTextExtent( strItem );
	
	if( size.cx > MAX_VENDOR_ITEM_TEXT_EXTENT )
	{
		CString str, strOld;
		WORD wCodePage = g_codePage;
		const char* begin = strItem;
		const char* end = begin + strItem.GetLength();
		const char* iter = begin;
		while( *iter && iter < end )
		{
			const char* next	= CharNextEx( iter, wCodePage );
			char temp[16];
			memcpy( temp, iter, next - iter );
			temp[next - iter]	= 0;
			str		+= temp;
			size	= p2DRender->m_pFont->GetTextExtent( str );
			if( size.cx > MAX_VENDOR_ITEM_TEXT_EXTENT )
				break;
			iter	= next;
			strOld	= str;
		}
		strItem		= strOld + "...";
	}
/*
	// 아이템 명이 너무 길 경우 ...으로.
	if( strItem.GetLength() > 18 ) 
	{
		int	nReduceCount = 0;

		for( nReduceCount=0; nReduceCount<18; )
		{
			if( IsDBCSLeadByte( strItem[ nReduceCount ] ) )
				nReduceCount+=2;
			else
				nReduceCount++;
		}
		strItem = strItem.Left( nReduceCount );
		strItem += "...";
	}
*/
	p2DRender->TextOut( nX + 50, nY + 3, strItem, 0xffffffff );//ADEILSON 0xff000000
	
	char szCost[1024] = {0,};
	DWORD dwCostTem = pItemElem->m_nCost;
	_itoa( dwCostTem, szCost, 10 );
	if( ::GetLanguage() == LANG_KOR )
		strItem = GetNumberFormatSelling(szCost);
	else
		strItem = GetNumberFormatEx(szCost);
	
	DWORD dwCostColor = 0xffffffff;//ADEILSON 0xff000000
	if( dwCostTem >= 1000000000 ) // 10억
		dwCostColor = 0xff990099;
	else if( dwCostTem >= 100000000 ) // 1억
		dwCostColor = 0xffCC0303;
	else if( dwCostTem >= 10000000 ) // 1000만
		dwCostColor = 0xffFF6600;
	else if( dwCostTem >= 1000000 ) // 100만
		dwCostColor = 0xff3333FF;
	else if( dwCostTem >= 100000 ) // 10만
		dwCostColor = 0xff009900;
	
	p2DRender->TextOut( nX + 50, nY + 17, strItem, dwCostColor );
}

void CWndVendorCtrl::OnDraw( C2DRender* p2DRender )
{
	if( m_pMover == NULL )
		return;
	
	CRect rect	= GetClientRect();
	int nWidth	= rect.Width() / 32;
	int nHeight		= rect.Height() / 35;

#if __VER >= 8 //__S8_VENDOR_REVISION
	for( int i = 0; i < MAX_VENDOR_REVISION; i++ )
#else //__S8_VENDOR_REVISION
	for( int i = 0; i < MAX_VENDITEM; i++ )
#endif //__S8_VENDOR_REVISION
	{
		int x	= i % 2;
		int y	= i / 2;
		
		int nX;
		int nY = y * 32 + 8 + y * 4;
		if( x == 0 )
			nX = x * 32 + 15;
		else
			nX = 235;

		CRect rectHittest = CRect( nX, nY, nX + 180, nY + 32 );
		CPoint point	= GetMousePoint();
		if( rectHittest.PtInRect( point ) )
		{
			if( CWndBase::m_GlobalShortcut.m_dwData )
			{
				m_nCurSel = -1;
				{
					CPoint ptx = CPoint(nX, nY);
					m_pTex->Render( p2DRender, ptx );		// 아이템 테두리 그리기
				}
			}
		}			
		CItemBase* pItemBase = GetItem( i );
		if( pItemBase )
		{

			// 툴팁
			float fScal = 1.0f;
			CPoint point	= GetMousePoint();
			if( rectHittest.PtInRect( point ) )
			{
				fScal = 1.15f;
				CPoint point2 = point;
				ClientToScreen( &point2 );
				ClientToScreen( &rectHittest );
				g_WndMng.PutToolTip_Item( pItemBase, point2, &rectHittest, APP_VENDOR );
			}

			// 아이템 아이콘 
			if( ((CItemElem*)pItemBase)->IsFlag( CItemElem::expired ) )
				pItemBase->GetTexture()->Render2( p2DRender, CPoint( nX, nY ), D3DCOLOR_XRGB( 255, 100, 100 ) );					
			else
				pItemBase->GetTexture()->Render2( p2DRender, CPoint( nX, nY ), D3DCOLOR_XRGB( 255, 255, 255 ), fScal, fScal );

			// 아이템 이름, 판매가격
			OnDrawItemInfo( p2DRender, ((CItemElem*)pItemBase), nX, nY );

			if( i == m_nCurSel )
				p2DRender->RenderRect( CRect( nX, nY, nX + 32, nY + 32 ), 0xff00ffff );
			CItemElem* pItemElem	= (CItemElem*)pItemBase;
			if( pItemElem->GetProp()->dwPackMax > 1 )
			{
				short nItemNum	= pItemBase->GetExtra();
				TCHAR szTemp[32];
				_stprintf( szTemp, "%d", nItemNum );
				CSize size	= p2DRender->m_pFont->GetTextExtent( szTemp );
				p2DRender->TextOut( nX - 7 + 32 - size.cx+5, nY - 11 + 32 - size.cy+12, szTemp, 0xff1010ff );
			}
		}
	}
}
#else // __VER >= 8 // __S8_VENDOR_REVISION
void CWndVendorCtrl::OnDraw( C2DRender* p2DRender )
{
	if( m_pMover == NULL )
		return;

	CRect rect	= GetClientRect();
	int nWidth	= rect.Width() / 32;
	int nHeight		= rect.Height() / 35;

	for( int i = 0; i < MAX_VENDITEM; i++ )
	{
		int x	= i % 6;
		int y	= i / 6;
		//p2DRender->RenderRect( CRect( x * 32 + 7, y * 32 + 11, x * 32 + 32 + 5, y * 32 + 32 + 9 ), 0xff00ffff );
		CRect rectHittest	= CRect( x * 32 + 7, y * 32 + 11, x * 32 + 32 + 5, y * 32 + 32 + 9 );

		CPoint point	= GetMousePoint();

		if( rectHittest.PtInRect( point ) )
		{
			if( CWndBase::m_GlobalShortcut.m_dwData )
			{
				m_nCurSel = -1;
				CPoint ptx = CPoint(x * 32 + 7, y * 32 + 11);
				{
					m_pTex->Render( p2DRender, ptx );
				}
			}
		}			
		CItemBase* pItemBase = GetItem( i );
		if( pItemBase )
		{
			if( ((CItemElem*)pItemBase)->IsFlag( CItemElem::expired ) )
			{
				pItemBase->GetTexture()->Render2( p2DRender, CPoint( x * 32 + 6, y * 32 + 10 ), D3DCOLOR_XRGB( 255, 100, 100 ) );
			}
			else
			{
				pItemBase->GetTexture()->Render( p2DRender, CPoint( x * 32 + 6, y * 32 + 10 ) );
			}

			CPoint point	= GetMousePoint();
			if( rectHittest.PtInRect( point ) )
			{
				CPoint point2 = point;
				ClientToScreen( &point2 );
				ClientToScreen( &rectHittest );
				g_WndMng.PutToolTip_Item( pItemBase, point2, &rectHittest, APP_VENDOR );
			}
			if( i == m_nCurSel )
				p2DRender->RenderRect( CRect( x * 32 + 7, y * 32 + 11, x * 32 + 32 + 5, y * 32 + 32 + 9 ), 0xff00ffff );
			CItemElem* pItemElem	= (CItemElem*)pItemBase;
			if( pItemElem->GetProp()->dwPackMax > 1 )
			{
				short nItemNum	= pItemBase->GetExtra();
				TCHAR szTemp[32];
				_stprintf( szTemp, "%d", nItemNum );
				CSize size	= p2DRender->m_pFont->GetTextExtent( szTemp );
				p2DRender->TextOut( x * 32 + 32 - size.cx+5, y * 32 + 32 - size.cy+12, szTemp, 0xff1010ff );
			}
		}
	}
}
#endif // __VER >= 8 // __S8_VENDOR_REVISION

BOOL CWndVendorCtrl::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	int nDstId	= -1;
	CRect rect	= GetClientRect();
	int nWidth	= rect.Width() / 32;
	int nHeight		= rect.Height() / 35;
	CPoint pt( 3, 3 );
	if( pShortcut->m_dwType == ITYPE_ITEM && pShortcut->m_dwData == 0 )
	{
		//		CWndBase* pParent	= (CWndBase*)GetParentWnd();
		//		pParent->OnChildNotify( WIN_ITEMDROP, m_nIdWnd, (LRESULT*)pShortcut );
		return TRUE;
	}
	else
	{
		CItemBase* pItemBase = g_pPlayer->GetItemId( pShortcut->m_dwId );
		if( pItemBase )
		{
#if __VER >= 8 //__S8_VENDOR_REVISION
			for( int i = 0; i < MAX_VENDOR_REVISION; i++ )
#else //__S8_VENDOR_REVISION
			for( int i = 0; i < MAX_VENDITEM; i++ )
#endif //__S8_VENDOR_REVISION
			{
#if __VER >= 8 // __S8_VENDOR_REVISION
				int x	= i % 2;
				int y	= i / 2;
				
				int nX;
				int nY = y * 32 + 8 + y * 4;
				if( x == 0 )
					nX = x * 32 + 15;			
				else
					nX = 235;
				
				rect.SetRect( nX, nY, nX + 180, nY + 32 );
#else // __VER >= 8 // __S8_VENDOR_REVISION
				int x	= i % 6;
				int y	= i / 6;
				rect.SetRect( x * 32 + 7, y * 32 + 11, x * 32 + 32 + 5, y * 32 + 32 + 9 );
#endif // __VER >= 8 // __S8_VENDOR_REVISION
				
				if( rect.PtInRect( point ) && GetItem( i ) == NULL )
				{
					pShortcut->m_dwData		= i + 100;
					CWndBase* pParent	= (CWndBase*)GetParentWnd();
					pParent->OnChildNotify( 100, m_nIdWnd, (LRESULT*)pShortcut );
				}
			}
			return TRUE;
		}
	}
	return TRUE;
}

void CWndVendorCtrl::OnMouseMove( UINT nFlags, CPoint point )
{
	if( m_bDrag == FALSE )
		return;
	m_bDrag		= FALSE;
	CPoint pt( 3, 3 );
	CRect rect;

	CItemBase* pItemBase	= m_pFocusItem;
	if( IsUsingItem( pItemBase ) )
	{
		m_GlobalShortcut.m_pFromWnd		= this;
		m_GlobalShortcut.m_dwShortcut	= SHORTCUT_ITEM;
		m_GlobalShortcut.m_dwIndex	= m_nCurSel;
		m_GlobalShortcut.m_dwType	= 0;
		m_GlobalShortcut.m_dwId		= pItemBase->m_dwObjId;
		m_GlobalShortcut.m_pTexture		= pItemBase->GetTexture();
		m_GlobalShortcut.m_dwData	= (DWORD)pItemBase;
		_tcscpy( m_GlobalShortcut.m_szString, pItemBase->GetProp()->szName );
	}
}

void CWndVendorCtrl::OnLButtonDown( UINT nFlags, CPoint point )
{
#if __VER < 8 // __S8_VENDOR_REVISION
	if( m_pMover == g_pPlayer )
		return;
#endif // __VER < 8 // __S8_VENDOR_REVISION

	if( GetAsyncKeyState( VK_LCONTROL ) & 0x8000 )
		return;

	if( CWndBase::m_GlobalShortcut.IsEmpty() == FALSE )
		return;
	int nItem	= HitTest( point );
	if( nItem == -1 )
		return;

	CItemBase* pItemBase	= GetItem( nItem );
	if( pItemBase )
	{
		m_pFocusItem	= pItemBase;
		m_nCurSel	= nItem;
		m_bDrag		= TRUE;
	}
}

void CWndVendorCtrl::OnLButtonUp( UINT nFlags, CPoint point )
{
	m_bDrag		= FALSE;
	int nItem	= HitTest( point );
	if( nItem == -1 )
		return;
	CItemBase* pItemBase	= GetItem( nItem );
	if( pItemBase )
	{
		if( ( GetAsyncKeyState( VK_LCONTROL ) & 0x8000 ) )
		{
			m_nCurSel	= nItem;
			m_pFocusItem	= pItemBase;
		}
	}
}

void CWndVendorCtrl::OnRButtonUp( UINT nFlags, CPoint point )
{
	int nItem	= HitTest( point );
	if( nItem == -1 )
		return;

	CItemBase* pItemBase	= GetItem( nItem );
	if( pItemBase )
	{
		{
			m_nCurSel	= nItem;
			m_pFocusItem	= pItemBase;
		}
	}

	g_DPlay.SendUnregisterPVendorItem( nItem );
}

void CWndVendorCtrl::OnRButtonDown( UINT nFlags, CPoint point )
{

}

void CWndVendorCtrl::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	if( m_pMover == g_pPlayer )
		return;

	int nItem	= HitTest( point );
	if( nItem == -1 )
		return;

	CItemBase* pItemBase	= GetItem( nItem );
	if( pItemBase )
	{
		m_pFocusItem	= pItemBase;
		m_nCurSel	= nItem;
		if( m_pFocusItem )
		{
			CWndVendor* pWndVendor	= (CWndVendor*)GetParentWnd();
			SAFE_DELETE( pWndVendor->m_pWndVendorBuy );
			pWndVendor->m_pWndVendorBuy		= new CWndVendorBuy( pItemBase, nItem );
			pWndVendor->m_pWndVendorBuy->Initialize( pWndVendor, APP_VENDOR_BUY );
//			pParent->OnChildNotify( WIN_DBLCLK, m_nIdWnd, (LRESULT*)m_pFocusItem );
		}
	}
}

void CWndVendorCtrl::OnRButtonDblClk( UINT nFlags, CPoint point )
{

}

void CWndVendorCtrl::OnSize( UINT nType, int cx, int cy )
{
	CWndBase::OnSize( nType, cx, cy );
}

void CWndVendorCtrl::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow	= rectWnd;
	m_rectClient	= m_rectWindow;
//	m_rectClient.DeflateRect( 3, 3 );

	if( bOnSize )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
}

void CWndVendorCtrl::PaintFrame( C2DRender* p2DRender )
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

BOOL CWndVendorCtrl::OnEraseBkgnd( C2DRender* p2DRender )
{
	return TRUE;
}

int CWndVendorCtrl::HitTest( CPoint point )
{
	CRect rect	= GetClientRect();
	int nWidth	= rect.Width() / 32;
	int nHeight		= rect.Height() / 35;

#if __VER >= 8 //__S8_VENDOR_REVISION
	for( int i = 0; i < MAX_VENDOR_REVISION; i++ )
#else //__S8_VENDOR_REVISION
	for( int i = 0; i < MAX_VENDITEM; i++ )
#endif //__S8_VENDOR_REVISION
	{
#if __VER >= 8 // __S8_VENDOR_REVISION
		int x	= i % 2;
		int y	= i / 2;
		
		int nX;
		int nY = y * 32 + 8 + y * 4;
		if( x == 0 )
			nX = x * 32 + 15;
		else
			nX = 235;
		rect.SetRect( nX, nY, nX + 32, nY + 32 );
#else // __VER >= 8 // __S8_VENDOR_REVISION
		int x	= i % 6;
		int y	= i / 6;
		rect.SetRect( x * 32 + 7, y * 32 + 11, x * 32 + 32 + 5, y * 32 + 32 + 9 );
#endif // __VER >= 8 // __S8_VENDOR_REVISION
		
		if( rect.PtInRect( point ) )
			return i;
	}
	return -1;
}

