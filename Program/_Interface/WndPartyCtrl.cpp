// WndBase.cpp: implementation of the CWndBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AppDefine.h"
#include "DPClient.h"
#include "Party.h"
#include "WndPartyCtrl.h"
#include "WndMessenger.h"
extern	CDPClient	g_DPlay;
extern CParty g_Party;

#include "WndManager.h"

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA


////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndPartyCtrl
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndPartyCtrl::CWndPartyCtrl() 
{
	m_nCurSelect = -1;
	m_bDrag = FALSE;
	m_pVBGauge = NULL;
}
CWndPartyCtrl::~CWndPartyCtrl()
{
//	ResetContent();
	DeleteDeviceObjects();
}
void CWndPartyCtrl::Create( DWORD dwListCtrlStyle, RECT& rect, CWndBase* pParentWnd, UINT nID )
{
	m_dwListCtrlStyle = dwListCtrlStyle;
	CWndBase::Create( WBS_CHILD, rect, pParentWnd, nID );
}

void CWndPartyCtrl::LoadListBoxScript(LPCTSTR lpFileName) 
{
}	
void CWndPartyCtrl::InterpriteScript(CScanner& scanner,CPtrArray& ptrArray) 
{
}
HRESULT CWndPartyCtrl::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();

	if( m_pVBGauge == NULL )
		return m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBGauge, NULL );
	
	return S_OK;
}
HRESULT CWndPartyCtrl::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();

	m_texGauEmptyNormal.Invalidate();
	m_texGauFillNormal.Invalidate();
	
    SAFE_RELEASE( m_pVBGauge );
	return S_OK;
}
HRESULT CWndPartyCtrl::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();

	m_texGauEmptyNormal.DeleteDeviceObjects();
	m_texGauFillNormal.DeleteDeviceObjects();
	
	SAFE_RELEASE( m_pVBGauge );
	return S_OK;
}
void CWndPartyCtrl::OnInitialUpdate()
{
	CRect rect = GetWindowRect();
	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );//,m_pSprPack,-1);
	m_nFontHeight = 20;//GetFontHeight( &g_2DRender );

	RestoreDeviceObjects();
	m_texGauEmptyNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptySmall.bmp" ), 0xffff00ff, TRUE );
	m_texGauFillNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauFillSmall.bmp" ), 0xffff00ff, TRUE );
}
void CWndPartyCtrl::OnDraw( C2DRender* p2DRender ) 
{
	if( NULL == g_pPlayer )
		return;

	CPoint pt( 3, 3 );

//	pt.y -= ( m_nFontHeight + 3 ) * m_wndScrollBar.GetScrollPos();

#if __VER < 11 // __CSC_VER11_4
	CWndMessenger* pWndMessenger = (CWndMessenger*)GetWndBase( APP_MESSENGER_ );
#endif //__CSC_VER11_4
	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
	
	int nMax = g_Party.m_nSizeofMember;
	// 눈에 보이는 갯수가 페이지라인수 보다 크면 보이는 갯수를 페이지라인수로 조정 
	if( nMax - m_wndScrollBar.GetScrollPos() > m_wndScrollBar.GetScrollPage() )
		nMax = m_wndScrollBar.GetScrollPage() + m_wndScrollBar.GetScrollPos();
	if( nMax < m_wndScrollBar.GetScrollPos() )
		nMax = 0;
	
	TEXTUREVERTEX2* pVertex = new TEXTUREVERTEX2[ 6 * 2 * nMax ];
	TEXTUREVERTEX2* pVertices = pVertex;
	
	for( int i = m_wndScrollBar.GetScrollPos(); i < nMax; i++ ) 
	{
		CMover* pObjMember = prj.GetUserByID( g_Party.m_aMember[i].m_uPlayerId );
		CString strMember;
#if __VER >= 11 // __SYS_PLAYER_DATA
		PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->GetPlayerData( g_Party.m_aMember[i].m_uPlayerId );
		int nJob	= pPlayerData->data.nJob;
		int nSex	= pPlayerData->data.nSex;
#else	// __SYS_PLAYER_DATA
		int nJob	= g_Party.m_aMember[ i ].m_nJob;
		int nSex	= g_Party.m_aMember[ i ].m_nSex;
#endif	// __SYS_PLAYER_DATA
		
		// 상태에 따라 색 변경
		DWORD dwColor = 0xffffffff;//adeilson 0xff000000
		if( IsValidObj(pObjMember) )
		{
			if( pObjMember->GetHitPoint() == 0 ) 
				dwColor = 0xffff0000; // 죽은놈
			else 
				if( ((FLOAT)pObjMember->GetHitPoint()) / ((FLOAT)pObjMember->GetMaxHitPoint()) < 0.1f ) 
					dwColor = 0xffffff00; // HP 10% 이하인 놈
			strMember.Format( "%d %s", pObjMember->GetLevel(), pObjMember->GetName() );
		}
		else
		{
			dwColor = 0xff878787; // 디폴트는 주위에 없는 놈
			if( g_Party.m_aMember[ i ].m_bRemove ) 
				dwColor = 0xffffffff;//adeilson 0xff000000 // 서버에 없는 놈
#if __VER >= 11 // __SYS_PLAYER_DATA
			strMember.Format( "?? %s", pPlayerData->szPlayer );
#else	// __SYS_PLAYER_DATA
			strMember.Format( "?? %s", g_Party.m_aMember[ i ].m_szName );
#endif	// __SYS_PLAYER_DATA
		}
		if( i == m_nCurSelect )
			dwColor = 0xff6060ff; 

		int x = 0, nWidth = m_rectClient.Width() - 10;// - 1;

		CRect rect( x, pt.y, x + nWidth, pt.y + m_nFontHeight );

		rect.SetRect( x + 20, pt.y + 18, x + nWidth - 10, pt.y + 30 ); 
		nWidth	= pObjMember ? pObjMember->GetHitPointPercent( rect.Width() ) : 0;
		CRect rectTemp = rect; 
		rectTemp.right = rectTemp.left + nWidth;
		if( rect.right < rectTemp.right )
			rectTemp.right = rect.right;

		m_pTheme->RenderGauge( p2DRender, &rect, 0xffffffff, m_pVBGauge, &m_texGauEmptyNormal );
		m_pTheme->RenderGauge( p2DRender, &rectTemp, 0x64ff0000, m_pVBGauge, &m_texGauFillNormal );

		rect.SetRect( x + 3, pt.y, x + 3 + 32, pt.y + 6 + 32 ); 

		p2DRender->TextOut( x + 20, pt.y + 3, strMember, dwColor ); 

		if( MAX_EXPERT <= nJob )
		{
#if __VER >= 10 // __LEGEND
			if( MAX_PROFESSIONAL <= nJob && nJob < MAX_MASTER )
				pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 16 ) + ( 8 * nSex ), &pVertices, 0xffffffff );
			else if( MAX_MASTER <= nJob )
				pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 24 ) + ( 8 * nSex ), &pVertices, 0xffffffff );
			else
#ifdef __3RD_LEGEND16
				if ( MAX_HERO <= nJob )
						pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 32 ) + ( 8 * nSex ), &pVertices, 0xffffffff );
				else
#endif //__3RD_LEGEND16
#endif //__LEGEND
			pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 6 ) + ( 8 * nSex ), &pVertices, 0xffffffff );
		}
		else
		{
			pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  12 + nJob + ( 6 * nSex ), &pVertices, 0xffffffff );
		}

		pt.y += m_nFontHeight + 3;
	}
	pWndWorld->m_texMsgIcon.Render( m_pApp->m_pd3dDevice, pVertex, ( (int) pVertices - (int) pVertex ) / sizeof( TEXTUREVERTEX2 ) );
	safe_delete_array( pVertex );
}
void CWndPartyCtrl::OnLButtonUp( UINT nFlags, CPoint point )
{
	m_bDrag = FALSE;

	CPoint pt( 3, 3 );
	CRect rect;

	pt.y -= (m_nFontHeight + 3) * m_wndScrollBar.GetScrollPos();
	for( int i = 0; i < g_Party.m_nSizeofMember; i++ ) 
	{
		CMover* pObjMember = prj.GetUserByID( g_Party.m_aMember[i].m_uPlayerId );
		rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
		if( rect.PtInRect( point ) )
		{
			CMover* pObjMember = prj.GetUserByID( g_Party.m_aMember[ i ].m_uPlayerId );
			m_nCurSelect = i;
			if( g_pPlayer != pObjMember ) 
			{
				if( IsValidObj(pObjMember ) ) 
				{ // 화면에 없는놈은 패스...
					g_WorldMng()->SetObjFocus( pObjMember );
					return;
				}
			}
		}
		pt.y += m_nFontHeight + 3;
	}
	g_WorldMng()->SetObjFocus(NULL);
}
void CWndPartyCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
}
void CWndPartyCtrl::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	CPoint pt( 3, 3 );
	CRect rect;
	
	int nMax = g_Party.m_nSizeofMember;
	// 눈에 보이는 갯수가 페이지라인수 보다 크면 보이는 갯수를 페이지라인수로 조정 
	if( nMax - m_wndScrollBar.GetScrollPos() > m_wndScrollBar.GetScrollPage() )
		nMax = m_wndScrollBar.GetScrollPage();
	
	for( int i = m_wndScrollBar.GetScrollPos(); i < nMax; i++ ) 
	{
		CMover* pObjMember = prj.GetUserByID( g_Party.m_aMember[i].m_uPlayerId );
		rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
		if( rect.PtInRect( point ) )
		{
			CMover* pObjMember = prj.GetUserByID( g_Party.m_aMember[ i ].m_uPlayerId );
			m_nCurSelect = i;
			if( g_pPlayer != pObjMember ) 
			{
#if __VER >= 11 // __SYS_PLAYER_DATA
				CWndMessage* pWndMessage	= g_WndMng.OpenMessage( CPlayerDataCenter::GetInstance()->GetPlayerString( g_Party.m_aMember[ i ].m_uPlayerId ) );
#else	// __SYS_PLAYER_DATA
				CWndMessage* pWndMessage = g_WndMng.OpenMessage( g_Party.m_aMember[ i ].m_szName );
#endif	// __SYS_PLAYER_DATA
			}
		}
		pt.y += m_nFontHeight + 3;
	}
}

void CWndPartyCtrl::OnLButtonDown( UINT nFlags, CPoint point )
{

}
void CWndPartyCtrl::OnRButtonUp( UINT nFlags, CPoint point )
{
}
void CWndPartyCtrl::OnRButtonDown( UINT nFlags, CPoint point )
{
}
void CWndPartyCtrl::OnRButtonDblClk( UINT nFlags, CPoint point)
{
}
void CWndPartyCtrl::OnSize( UINT nType, int cx, int cy )
{
//	m_string.Reset( g_2DRender.m_pFont, &GetClientRect() );

	CRect rect = GetWindowRect();
	rect.left = rect.right - 15;
	m_wndScrollBar.SetWndRect( rect );

	int nPage, nRange;
	nPage = GetClientRect().Height() / m_nFontHeight;
	nRange = g_Party.m_nSizeofMember;
	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );

	CWndBase::OnSize( nType, cx, cy);
}
void CWndPartyCtrl::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect( 3, 3 );

	if( bOnSize )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
}
void CWndPartyCtrl::PaintFrame( C2DRender* p2DRender )
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

	int nPage, nRange;
	if( 1 ) 
	{
		m_nFontHeight = 20;
		nPage = GetClientRect().Height() / m_nFontHeight;
		nRange = g_Party.m_nSizeofMember;
	}
	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );
}
BOOL CWndPartyCtrl::OnEraseBkgnd( C2DRender* p2DRender )
{
	return TRUE;
}
BOOL CWndPartyCtrl::SetItem( const LVITEM* pItem )
{
	return TRUE;
}
int CWndPartyCtrl::InsertItem( const LVITEM* pItem )
{
	return TRUE;
}
int CWndPartyCtrl::GetItemCount() const
{
	return 1;//m_aItems.GetSize();
}
int CWndPartyCtrl::InsertColumn( int nCol, const LVCOLUMN* pColumn )
{
	return nCol;
}

void CWndPartyCtrl::ScrollBarPos( int nPos )
{
	m_wndScrollBar.SetScrollPos( nPos );
}
