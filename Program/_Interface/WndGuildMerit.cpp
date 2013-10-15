#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndManager.h"

#include "Guild.h"

#include "WndGuildMerit.h"

#ifdef __CLIENT
#include "DPClient.h"
extern	CDPClient	g_DPlay;
#endif // CLIENT

/****************************************************
  WndId : APP_GUILD_MERIT - 길드공헌창
  CtrlId : WIDC_EDIT1 - 
  CtrlId : WIDC_STATIC1 - PXP지원 :
  CtrlId : WIDC_CHECK1 - 
  CtrlId : WIDC_STATIC2 - 지 원 금   :
  CtrlId : WIDC_BUTTON1 - Button
  CtrlId : WIDC_BUTTON2 - Button
****************************************************/

CWndGuildMerit::CWndGuildMerit() 
{ 
	m_nPenya = 0;
	m_nItem = 0;
} 
CWndGuildMerit::~CWndGuildMerit() 
{ 
	m_nPenya = 0;
	m_nItem = 0;
} 
void CWndGuildMerit::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndGuildMerit::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_LISTBOX1 );
	
	m_wndctrlMerit.Create( WLVS_ICON | WBS_NODRAWFRAME, lpWndCtrl->rect, this, 100 );
	
	
	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();

	TCHAR szNumber[ 64 ];
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	
	_itot( 0, szNumber, 10 );
	pWndEdit->SetString( szNumber );
	pWndEdit->SetFocus();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndGuildMerit::Initialize( CWndBase* pWndParent, DWORD ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILD_MERIT1, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndGuildMerit::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndGuildMerit::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndGuildMerit::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndGuildMerit::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

/*
TID_GAME_GUILDNEEDGOLD 길드창고에 페냐가 부족하여 망토를 생성할 수 없습니다. 
TID_GAME_GUILDMERITSTUFF 길드에 페냐와 경험치를 공헌했습니다. 
TID_GAME_GUILDMERITMAXLEVEL 길드의 레벨이 상승하지 않는 단계이므로 공헌이 되지 않습니다. 
TID_GAME_GUILDMERITLOWPXP PXP가 100% 되지 않아 공헌도가 올라지 않았습니다. 
TID_GAME_GUILDMERITMAXGOLD 공헌할 수 있는 최대를 넘었습니다. 
TID_GAME_GUILDMAXBANKGOLD 길드창고에 42억페냐가 넘는 금액이 있습니다. 
TID_GAME_GUILDNOTENGGOLD 인벤토리에 가진 페냐가 부족하여 창고에 페냐를 넣지 못했습니다. 
TID_GAME_GUILDMERITSYSERROR 시스템 영향으로 공헌이 되지않았습니다.  
*/
// 에러 메시지들.
void CGuild::MeritResultMsg( CONTRIBUTION_RESULT cbResult )
{
	switch( cbResult )
	{
	case CONTRIBUTION_OK:	// 공헌 성공 - 이건 서버에서 검사해보고 됐을때 보내주자.
		g_WndMng.PutString( prj.GetText( TID_GAME_GUILDMERITSTUFF ), NULL, prj.GetTextColor( TID_GAME_GUILDMERITSTUFF ) );
		break;
	case CONTRIBUTION_FAIL_MAXLEVEL:	// 길드가 이미 최고레벨에 올랐을때.
		g_WndMng.PutString( prj.GetText( TID_GAME_GUILDMERITMAXLEVEL ), NULL, prj.GetTextColor( TID_GAME_GUILDMERITMAXLEVEL ) );
		break;
//	case CONTRIBUTION_FAIL_GUILD_OVERFLOW_PXP:	// 길드PXP가 한계치(42억)를 넘었을때.
//		g_WndMng.PutString( prj.GetText( TID_GAME_GUILDMERITMAXLEVEL ), NULL, prj.GetTextColor( TID_GAME_GUILDMERITMAXLEVEL ) );
//		break;
	case CONTRIBUTION_FAIL_GUILD_OVERFLOW_PENYA:// 길드페냐가 한계치(42억)를 넘었을때.
		g_WndMng.PutString( prj.GetText( TID_GAME_GUILDMAXBANKGOLD ), NULL, prj.GetTextColor( TID_GAME_GUILDMAXBANKGOLD ) );
		break;
	case CONTRIBUTION_FAIL_INVALID_CONDITION:	// 서버이상으로 공헌이 실패.
		g_WndMng.PutString( prj.GetText( TID_GAME_GUILDMERITSYSERROR ), NULL, prj.GetTextColor( TID_GAME_GUILDMERITSYSERROR ) );
		break;
//	case CONTRIBUTION_FAIL_OVERFLOW_PXP:	// 개인PXP가 한계치(42억)를 넘었을때.
//		g_WndMng.PutString( prj.GetText( TID_GAME_GUILDMERITMAXLEVEL ), NULL, prj.GetTextColor( TID_GAME_GUILDMERITMAXLEVEL ) );
//		break;
	case CONTRIBUTION_FAIL_OVERFLOW_PENYA:	// 개인페냐가 한계치(42억)를 넘었을때.
		g_WndMng.PutString( prj.GetText( TID_GAME_GUILDMERITMAXGOLD ), NULL, prj.GetTextColor( TID_GAME_GUILDMERITMAXGOLD ) );
		break;
	}
}


BOOL CWndGuildMerit::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if ( message == EN_RETURN )
	{
		nID = WIDC_BUTTON1;
	}

	switch( nID )
	{
	case WIDC_CHECK2:	
		{
			CWndButton* pChk = (CWndButton*)GetDlgItem( WIDC_CHECK2 );
			
			m_nItem = pChk->GetCheck();

			if( m_nItem )
			{
				if( g_pPlayer )
				{
					CItemElem* pItemElem = NULL;
					
					for( int i=0; i<g_pPlayer->m_Inventory.GetMax() ; i++ )
					{
						pItemElem = g_pPlayer->m_Inventory.GetAtId(i);

						if( pItemElem == NULL )
							continue;
						
						if( pItemElem->GetProp()->dwItemKind3 == IK3_GEM )
						{
							pItemElem->SetExtra( pItemElem->m_nItemNum );
							m_wndctrlMerit.AddItem(pItemElem);
						}
					}
				}
			}
			else
			{
				m_wndctrlMerit.RestoreItem();
				m_wndctrlMerit.ClearItem();
			}
		}
		break;

	case WIDC_BUTTON1:		// OK
		{
			CWndEdit *pWndEdit = (CWndEdit *)GetDlgItem( WIDC_EDIT1 );
			m_nPenya = atoi( pWndEdit->GetString() );		// 문자열을 숫자로 변환.
			if( m_nPenya < 0 )
				m_nPenya = 0;
			CGuild* pGuild = g_pPlayer->GetGuild();
			if( pGuild )
			{
				// 페냐가 공헌가능한지 검사.
				if( m_nPenya )
				{
					CONTRIBUTION_RESULT cbResult = pGuild->CanContribute( 0, (DWORD)m_nPenya, g_pPlayer->m_idPlayer );
					if( cbResult == CONTRIBUTION_OK )	// 페냐 공헌가능.
						g_DPlay.SendGuildContribution( 0, m_nPenya );
					pGuild->MeritResultMsg( cbResult );	// 안내메시지.
				}
				
				//*
				// 공헌아이템
				if( m_nItem )
				{
					CONTRIBUTION_RESULT cbResult = pGuild->CanContribute( m_wndctrlMerit.GetTotalCount(), 0, g_pPlayer->m_idPlayer );
					if( cbResult == CONTRIBUTION_OK )	// 아이템 공헌가능
						g_DPlay.SendGuildContribution( 0, 0,  1);

					pGuild->MeritResultMsg( cbResult );	// 안내메시지.
				}
				
				/**/


			} // pGuild
			Destroy();
		}
		break;
	case 10000:
	case WIDC_BUTTON2:		// NO
		m_wndctrlMerit.RestoreItem();
		Destroy();			// 걍 창 닫고 나감.
		break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


CTextureMng	CWndGuildMeritCtrl::m_textureMng;
CWndGuildMeritCtrl::CWndGuildMeritCtrl()
{
	m_nWndColor		= D3DCOLOR_TEMP( 255,  0x5 << 3,  0x5 << 3,  0x5 << 3 );
	m_nFontColor	= D3DCOLOR_TEMP( 255, 255, 255, 255 );
	m_nSelectColor	= D3DCOLOR_TEMP( 255, 255, 255, 0   );
	m_nCurSel	= -1;
	m_pFocusItem	= NULL;
	m_pItemContainer.clear();
	m_bDrag		= FALSE;
}

CWndGuildMeritCtrl::~CWndGuildMeritCtrl()
{
	RestoreItem();
}

void CWndGuildMeritCtrl::Create( DWORD dwListCtrlStyle, RECT & rect, CWndBase* pParentWnd, UINT nID )
{
	m_dwListCtrlStyle	= dwListCtrlStyle;
	CWndBase::Create( m_dwListCtrlStyle | WBS_CHILD, rect, pParentWnd, nID );
}
void CWndGuildMeritCtrl::OnInitialUpdate( void )
{
	CRect rect	= GetWindowRect();
	rect.left	= rect.right - 15;

	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );//,m_pSprPack,-1);
}
	
void CWndGuildMeritCtrl::OnDraw( C2DRender* p2DRender )
{
	if( m_pItemContainer.size() <= 0 )
		return;

	CRect rect	= GetClientRect();
	int nWidth	= rect.Width() / 32;
	int nHeight		= rect.Height() / 35;

	// 스크롤바 관련   
	CPoint pt( 0, 0 );

	nWidth = 6;
	int nPage = nHeight;
	int nRange = m_pItemContainer.size() / nWidth;// - nPage;
	if( m_pItemContainer.size() % nWidth )
		nRange++;

	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );
	
	pt.y = 0;
	pt.y += m_wndScrollBar.GetScrollPos() * nWidth;

	for( int i = pt.y; i < (int)( m_pItemContainer.size() ); i++ )
	{
		int x	= ( i - pt.x ) % 6;
		int y	= ( i - pt.y ) / 6;
		CItemBase* pItemBase	= m_pItemContainer[i];
		if( pItemBase )
		{
			pItemBase->GetTexture()->Render( p2DRender, CPoint( x * 32 + 13, y * 32 + 7 ) );
			CRect rectHittest	= CRect( x * 32 + 13, y * 32  + 7, x * 32  + 13 + 32, y * 32 + 32 + 7 );
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
				p2DRender->TextOut( x * 32 + 32 - size.cx + 17, y * 32 + 32 - size.cy + 10, szTemp, 0xff1010ff );
			}
		}
	}
}

void CWndGuildMeritCtrl::OnMouseMove( UINT nFlags, CPoint point )
{
	if( m_bDrag == FALSE )
		return;
	m_bDrag		= FALSE;
	CPoint pt( 3, 3 );
	CRect rect;

	CItemBase* pItemBase	= m_pFocusItem;
	if( pItemBase && pItemBase->GetExtra() > 0 )
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

BOOL CWndGuildMeritCtrl::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	return TRUE;
}

void CWndGuildMeritCtrl::OnRButtonUp( UINT nFlags, CPoint point )
{

}

void CWndGuildMeritCtrl::OnRButtonDown( UINT nFlags, CPoint point )
{

}

void CWndGuildMeritCtrl::OnLButtonDblClk( UINT nFlags, CPoint point )
{
}

void CWndGuildMeritCtrl::OnRButtonDblClk( UINT nFlags, CPoint point )
{

}

void CWndGuildMeritCtrl::OnSize( UINT nType, int cx, int cy )
{
	CWndBase::OnSize( nType, cx, cy );
}

void CWndGuildMeritCtrl::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow	= rectWnd;
	m_rectClient	= m_rectWindow;

	if( bOnSize )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
}

void CWndGuildMeritCtrl::PaintFrame( C2DRender* p2DRender )
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

BOOL CWndGuildMeritCtrl::OnEraseBkgnd( C2DRender* p2DRender )
{
	return TRUE;
}

int CWndGuildMeritCtrl::HitTest( CPoint point )
{
	CRect rect	= GetClientRect();
	int nWidth	= rect.Width() / 32;
	int nHeight		= rect.Height() / 35;
	
	CPoint pt( 0, 0 );
	
	pt.y = 0;
	pt.y += m_wndScrollBar.GetScrollPos() * nWidth;
	
	for( int i = pt.y; i < 50; i++ )
	{
		int x	= ( i - pt.y ) % 6;
		int y	= ( i - pt.y ) / 6;
		rect.SetRect( x * 32, y * 32, x * 32 + 32, y * 32 + 32 );
		if( rect.PtInRect( point ) )
			return i;
	}
	return -1;
}

void CWndGuildMeritCtrl::AddItem(CItemBase *pItemContainer)
{
	m_pItemContainer.push_back(pItemContainer);
}

void CWndGuildMeritCtrl::ClearItem()
{
	m_pItemContainer.clear();
}

int CWndGuildMeritCtrl::GetItemSize()
{
	return m_pItemContainer.size();
}

void CWndGuildMeritCtrl::RestoreItem()
{
	CItemElem* pItemElem = NULL;

	for( int i=0; i<(int)( m_pItemContainer.size() ); i++ )
	{
		pItemElem = (CItemElem*)m_pItemContainer[i];

		if( pItemElem )
			pItemElem->SetExtra( 0 );
	}
}

int CWndGuildMeritCtrl::GetTotalCount()
{
	int nCount = 0;
	CItemElem* pItemElem = NULL;
	
	for( int i=0; i<(int)( m_pItemContainer.size() ); i++ )
	{
		pItemElem = (CItemElem*)m_pItemContainer[i];		
		if( pItemElem )
			nCount += pItemElem->GetExtra();
	}

	return nCount;
}


