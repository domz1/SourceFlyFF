// WndBase.cpp: implementation of the CWndBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AppDefine.h"
#include "DPClient.h"
extern	CDPClient	g_DPlay;

#include "WndManager.h"

#if __VER >= 12 // __TAX
#include "Tax.h"
#endif // __TAX
#ifdef __16_SHOP
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndShopCtrl
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CTextureMng CWndShopCtrl::m_textureMng;

CWndShopCtrl::CWndShopCtrl() 
{
	m_nWndColor    = D3DCOLOR_TEMP( 255,  0x5 << 3,  0x5 << 3,  0x5 << 3 );
	m_nFontColor   = D3DCOLOR_TEMP( 255, 255, 255, 255 );
	m_nSelectColor = D3DCOLOR_TEMP( 255, 255, 255, 0   );
	m_nCurSelect = -1;
	m_bDrag = FALSE;
	m_nLastPoint = 0;

	Clear();

	m_nTabType = 0;
}
CWndShopCtrl::~CWndShopCtrl()
{
}
void CWndShopCtrl::Create( DWORD dwListCtrlStyle, RECT& rect, CWndBase* pParentWnd, UINT nID )
{
	m_dwListCtrlStyle = dwListCtrlStyle;
	CWndBase::Create( m_dwListCtrlStyle|WBS_CHILD, rect, pParentWnd, nID );
}
void CWndShopCtrl::OnInitialUpdate()
{
}
void CWndShopCtrl::OnDraw(C2DRender* p2DRender) 
{
	CPoint pt( 3, 3 );
	CRect rect = GetClientRect();

	for( int i = 0; i < MAX_CART; i++ )
	{
		int x = i % 6;
		int y = i / 6;		
		CItemBase* pItemBase = m_pItemElem[ i ];
		if( pItemBase )
		{
			if( ((CItemElem*)pItemBase)->IsFlag( CItemElem::expired ) )
				pItemBase->GetTexture()->Render2( p2DRender, CPoint( x * 35, y * 35 ), D3DCOLOR_ARGB( 255, 255, 100, 100 ) );
			else
				pItemBase->GetTexture()->Render( p2DRender, CPoint( x * 35, y * 35 ) );
			
			CRect rectHittest = CRect( x * 35, y * 35 + 3, x * 35 + 35, y * 35 + 35 + 3);
			CPoint point = GetMousePoint();
			if( rectHittest.PtInRect( point ) )
			{
				CPoint point2 = point;
				ClientToScreen( &point2 );
				ClientToScreen( &rectHittest );

				g_WndMng.PutToolTip_Item( pItemBase, point2, &rectHittest );
			}

			CItemElem* pItemElem = (CItemElem*)pItemBase;
			if( pItemElem->GetProp()->dwPackMax > 1 )
			{
				//short n	= m_nBuy[i];
				//if( IsUsingItem( pItemBase ) )
				//	n = pItemBase->GetExtra();																		
				short m_nItemNum	= pItemElem->m_nItemNum;
				if( pItemElem->GetExtra() > 0 )
				    m_nItemNum	-= (short)pItemElem->GetExtra();

				TCHAR szTemp[ 35 ];
				_stprintf( szTemp, "%d", m_nItemNum );
				//_stprintf( szTemp, "%d", n );
			    CD3DFont* pOldFont = p2DRender->GetFont();//adeilson
		        p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndNewTitle );
				CSize size = m_p2DRender->m_pFont->GetTextExtent( szTemp );
				m_p2DRender->TextOut( x * 35 + 35 - size.cx, y * 35 + 35 - size.cy, szTemp, 0xFF000000 );
				m_p2DRender->TextOut( x * 35 + 34 - size.cx, y * 35 + 34 - size.cy, szTemp, 0xFF85FF8A );
				p2DRender->SetFont( pOldFont );

				//_stprintf( szTemp, "%d", n );
				//CSize size = p2DRender->m_pFont->GetTextExtent( szTemp );
				//p2DRender->TextOut( x * 35 + 35 - size.cx, y * 35 + 35 - size.cy, szTemp, 0xff1010ff );
			}
		}
	}
}
void CWndShopCtrl::OnLButtonUp( UINT nFlags, CPoint point )
{
	ReleaseCapture();
	m_bDrag = FALSE;
}
void CWndShopCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bDrag == FALSE )
		return;
	m_bDrag = FALSE;
}

BOOL CWndShopCtrl::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CWndBase* pWndFrame =  pShortcut->m_pFromWnd->GetFrameWnd();
	if( pWndFrame->GetWndId() != APP_SHOP_ )
		return FALSE;

	if( pShortcut->m_dwShortcut == SHORTCUT_ITEM )
	{
		CItemElem* pItemElem = (CItemElem*)pShortcut->m_dwData;
		DWORD nTempCost = 0;
		if( m_nTabType == 0)
		{
			nTempCost += pItemElem->GetCost();
			nTempCost = static_cast< int >( static_cast< float >( nTempCost ) * prj.m_fShopBuyRate );

			if(CTax::GetInstance()->IsApplyTaxRate( g_pPlayer, pItemElem ))
				nTempCost += ( static_cast<DWORD>(nTempCost * CTax::GetInstance()->GetPurchaseTaxRate( g_pPlayer )) );
		}
		else if( m_nTabType == 1 || m_nTabType == 2 )
			nTempCost += pItemElem->GetChipCost();

		nTempCost = (int)((float)nTempCost * prj.m_fShopCost );
		
		if( pItemElem && pItemElem->m_dwItemId == II_SYS_SYS_SCR_PERIN )
			nTempCost = PERIN_VALUE;

		if( nTempCost > INT_MAX )
			return FALSE;

		for( int i = 0; i < MAX_CART; i++ )
		{
			int x = i % 6;
			int y = i / 6;			
			CRect rectHittest = CRect( x * 35, y * 35 + 3, x * 35 + 35, y * 35 + 35 + 3);
			if( rectHittest.PtInRect( point ) )
			{
				m_nLastPoint = i;
				break;
			}
		}
		CWndBase* pParent = (CWndBase*)GetParentWnd();
		pParent->OnChildNotify( WIN_ITEMDROP, m_nIdWnd, (LRESULT*)pShortcut ); 
		return TRUE;
	}

	return FALSE;
}

void CWndShopCtrl::OnSize( UINT nType, int cx, int cy )
{
	CRect rect = GetWindowRect();
	rect.left = rect.right - 15;

	CWndBase::OnSize( nType, cx, cy);
}
void CWndShopCtrl::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect( 3, 3 );

	if( bOnSize )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
}
void CWndShopCtrl::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();

	DWORD dwColor1 = D3DCOLOR_ARGB( 100, 0, 0,  0 );
	DWORD dwColor2 = D3DCOLOR_ARGB( 255, 240, 240,  240 );
	DWORD dwColor3 = D3DCOLOR_ARGB( 100, 200, 200,  200 );

	p2DRender->RenderFillRect ( rect, dwColor1 );
	p2DRender->RenderRoundRect( rect, dwColor2 );
	rect.DeflateRect( 1 , 1 );
	p2DRender->RenderRect( rect, dwColor2 );
	rect.DeflateRect( 1 , 1 );
	p2DRender->RenderRect( rect, dwColor3 );
}

void CWndShopCtrl::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	for( int i = 0; i < MAX_CART; i++ )
	{
		int x = i % 6;
		int y = i / 6;		
		CRect rectHittest = CRect( x * 35, y * 35 + 3, x * 35 + 35, y * 35 + 35 + 3);
		if( rectHittest.PtInRect( point ) )
		{
			m_nCurSelect = i;
			break;
		}
	}
	if( m_nCurSelect != -1 )
	{
		Clear( m_nCurSelect );
		m_nCurSelect = -1;
	}
}
void CWndShopCtrl::InsertItem( CItemElem* pItemElem, char cTab, int nBuy )
{
	if( pItemElem->GetProp()->dwItemKind1 == IK1_GOLD )
		return;

	if( m_pItemElem[m_nLastPoint] == NULL )//normal an der stelle einfügen, die zuletzt anvisiert wurde
	{
		m_pItemElem[ m_nLastPoint ] = pItemElem;
		m_cTab[ m_nLastPoint ] = cTab;
		m_nBuy[ m_nLastPoint ] = nBuy;
	}
	else
	{
		int did = 0;
		for( int i=0;i<MAX_CART;i++ )
		{
			if( m_pItemElem[i] == NULL )//am ersten null element einfügen
			{
				m_pItemElem[i] = pItemElem;
				m_cTab[i] = cTab;
				m_nBuy[i] = nBuy;
				did = 1;
				break;
			}
		}
		if( did == 0 )//andernfalls ersetzen
		{
			m_pItemElem[ m_nLastPoint ] = pItemElem;
			m_cTab[ m_nLastPoint ] = cTab;
			m_nBuy[ m_nLastPoint ] = nBuy;
		}
	}
	m_nLastPoint = NULL;

}
int CWndShopCtrl::GetCost( int nType )
{
	int nCost = 0;
	for( int i=0;i<MAX_CART;i++ )
	{
		if( m_pItemElem[i] == NULL )
			continue;
		int nTempCost = 0;
		if( nType == 0)
		{
			nTempCost += m_pItemElem[i]->GetCost();
			nTempCost = static_cast< int >( static_cast< float >( nTempCost ) * prj.m_fShopBuyRate );

			if(CTax::GetInstance()->IsApplyTaxRate( g_pPlayer, m_pItemElem[i] ))
				nTempCost += ( static_cast<DWORD>(nTempCost * CTax::GetInstance()->GetPurchaseTaxRate( g_pPlayer )) );
		}
		else if( nType == 1 || nType == 2 )
			nTempCost += m_pItemElem[i]->GetChipCost();

		nTempCost = (int)((float)nTempCost * prj.m_fShopCost );
		
		if( m_pItemElem[i] && m_pItemElem[i]->m_dwItemId == II_SYS_SYS_SCR_PERIN )
			nTempCost = PERIN_VALUE;
		nCost += nTempCost*m_nBuy[i];
	}
	return nCost;
}
int CWndShopCtrl::GetCount( )
{
	int nCount = 0;
	for( int i=0;i<MAX_CART;i++ )
	{
		if( m_pItemElem[i] != NULL )
		{
			nCount++;
		}
	}
	return nCount;
}
void CWndShopCtrl::Clear( int nIndex )
{
	if( nIndex >= MAX_CART || nIndex < -1 )
		return;
	if( nIndex != -1 )
	{
		m_pItemElem[nIndex] = NULL;
		m_cTab[nIndex] = NULL;
		m_nBuy[nIndex] = NULL;
	}
	else
	{
		for( int i=0;i<MAX_CART;i++ )
		{
			m_pItemElem[i] = NULL;
			m_cTab[i] = NULL;
			m_nBuy[i] = NULL;	
		}
	}
}
#endif