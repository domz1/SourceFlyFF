// WndBase.cpp: implementation of the CWndBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "DPClient.h"
#include "WndGuildCtrl.h"
#include "WndMessenger.h"
extern	CDPClient	g_DPlay;

#include "WndManager.h"

#include "guild.h"

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndPartyCtrl
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndGuildCtrl::CWndGuildCtrl() 
{
	m_nCurSelect = -1;
	m_bDrag = FALSE;
	m_pVBGauge = NULL;
}
CWndGuildCtrl::~CWndGuildCtrl()
{
//	ResetContent();
	DeleteDeviceObjects();
}
void CWndGuildCtrl::Create( DWORD dwListCtrlStyle, RECT& rect, CWndBase* pParentWnd, UINT nID )
{
	m_dwListCtrlStyle = dwListCtrlStyle;
	CWndBase::Create( WBS_CHILD, rect, pParentWnd, nID );
}
void CWndGuildCtrl::LoadListBoxScript(LPCTSTR lpFileName) 
{
}	
void CWndGuildCtrl::InterpriteScript(CScanner& scanner,CPtrArray& ptrArray) 
{
}
HRESULT CWndGuildCtrl::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if( m_pVBGauge == NULL )
		return m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBGauge, NULL );
	return S_OK;
}
HRESULT CWndGuildCtrl::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
    SAFE_RELEASE( m_pVBGauge );
	return S_OK;
}
HRESULT CWndGuildCtrl::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	SAFE_RELEASE( m_pVBGauge );
	return S_OK;
}
void CWndGuildCtrl::OnInitialUpdate()
{
	CRect rect = GetWindowRect();
	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );//,m_pSprPack,-1);
	m_nFontHeight = 20;

	RestoreDeviceObjects();
	m_texGauEmptyNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptySmall.bmp" ), 0xffff00ff, TRUE );
	m_texGauFillNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauFillSmall.bmp" ), 0xffff00ff, TRUE );
}

void CWndGuildCtrl::OnDraw( C2DRender* p2DRender ) 
{
	if( NULL == g_pPlayer )
		return;

	CPoint pt( 3, 3 );

	CWndMessenger* pWndMessenger = (CWndMessenger*)GetWndBase( APP_MESSENGER_ );
	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
	
	CGuild * pGuild = g_pPlayer->GetGuild();
	if( pGuild )
	{
		int nMax = pGuild->GetSize();

		if( nMax - m_wndScrollBar.GetScrollPos() > m_wndScrollBar.GetScrollPage() )
			nMax = m_wndScrollBar.GetScrollPage() + m_wndScrollBar.GetScrollPos();
		if( nMax < m_wndScrollBar.GetScrollPos() )
			nMax = 0;

		TEXTUREVERTEX2* pVertex = new TEXTUREVERTEX2[ 6 * 2 * nMax ];
		TEXTUREVERTEX2* pVertices = pVertex;

		map<u_long, CGuildMember*>::iterator iter = pGuild->m_mapPMember.begin();

		m_nDrawCount = 0;
		for( int i = 0; i < m_wndScrollBar.GetScrollPos() && iter != pGuild->m_mapPMember.end(); i++, iter++, m_nDrawCount++ );

		BOOL bReturn = FALSE;
		memset( m_nServerCount, 0, sizeof( m_nServerCount ) );

		// 먼저 출력해야 될것을 추려냄
		for( ; iter != pGuild->m_mapPMember.end() ; ++iter )
		{
			CGuildMember * pGuildMember = (CGuildMember*)iter->second;
#if __VER >= 11 // __SYS_PLAYER_DATA
			PlayerData* pPlayerData		= CPlayerDataCenter::GetInstance()->GetPlayerData( pGuildMember->m_idPlayer );
			if( pPlayerData->data.uLogin == 0 )
#else	// __SYS_PLAYER_DATA
			if( pGuildMember->m_nMultiNo == 100 || pGuildMember->m_nLogin == 0 ) // 로그오프인것은 나중에 출력해야 되므로 모아둠
#endif	// __SYS_PLAYER_DATA
			{
				m_uServerPlayerId[10][m_nServerCount[10]] = pGuildMember->m_idPlayer ;
				++m_nServerCount[10];
			}
			else	// 먼저 출력해야 될것 모아둠 // 멀티서버 별로 모아둠
			{
#if __VER >= 11 // __SYS_PLAYER_DATA
				m_uServerPlayerId[pPlayerData->data.uLogin-1][m_nServerCount[pPlayerData->data.uLogin-1]] = pGuildMember->m_idPlayer;
				++m_nServerCount[pPlayerData->data.uLogin-1];
#else	// __SYS_PLAYER_DATA
				m_uServerPlayerId[pGuildMember->m_nMultiNo-1][m_nServerCount[pGuildMember->m_nMultiNo-1]] = pGuildMember->m_idPlayer;
				++m_nServerCount[pGuildMember->m_nMultiNo-1];
#endif	// __SYS_PLAYER_DATA
			}
		}

		// 나의 멀티서버 먼저 출력
		for( int j = 0 ; j < m_nServerCount[g_Neuz.m_uIdofMulti-1] && i < nMax; ++j, ++i )
		{
			u_long uGuildMemberId = m_uServerPlayerId[g_Neuz.m_uIdofMulti-1][j];
			CGuildMember * pGuildMember = pGuild->GetMember( uGuildMemberId );

#if __VER >= 11 // __SYS_PLAYER_DATA
			PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->GetPlayerData( pGuildMember->m_idPlayer );
			LPCSTR szMember		= pPlayerData->szPlayer;
			int nJob	= pPlayerData->data.nJob;
			DWORD dwSex		= pPlayerData->data.nSex;
#else	// __SYS_PLAYER_DATA
			LPCSTR szMember		= prj.GetPlayerString( pGuildMember->m_idPlayer );
			int nJob	= pGuildMember->m_nJob;
			DWORD dwSex		= pGuildMember->m_dwSex;
#endif	// __SYS_PLAYER_DATA

			if( szMember )
			{
				// 상태에 따라 색 변경
				DWORD dwColor = 0xffffffff;//adeilson 0xff000000
				if( i == m_nCurSelect )
					dwColor = 0xff6060ff; 
				
				int x = 0, nWidth = m_rectClient.Width();// - 1;
				
				CRect rect( x, pt.y, x + nWidth, pt.y + m_nFontHeight );
				rect.SetRect( x + 3, pt.y + 6, x + 3 + 32, pt.y + 6 + 32 ); 
				
				p2DRender->TextOut( x + 40, pt.y + 3, szMember, dwColor );	// 이름 출력

				// 직업과 성별 아이템 생성
				if( MAX_EXPERT <= nJob )
				{
#if __VER >= 10 // __LEGEND
					if( MAX_PROFESSIONAL <= nJob && nJob < MAX_MASTER )
						pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 16 ) + ( 8 * dwSex ), &pVertices, 0xffffffff );
					else if( MAX_MASTER <= nJob )
						pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 24 ) + ( 8 * dwSex ), &pVertices, 0xffffffff );
					else
#ifdef __3RD_LEGEND16
					if ( MAX_HERO <= nJob )
						pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 32 ) + ( 8 * dwSex ), &pVertices, 0xffffffff );
				else
#endif //__3RD_LEGEND16
#endif //__LEGEND
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 6 ) + ( 8 * dwSex ), &pVertices, 0xffffffff );
				}
				else
				{
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ), 12 + nJob + ( 6 * dwSex ), &pVertices, 0xffffffff );
				}
				// 멀티 서버 아이템 생성
#if __VER >= 11 // __SYS_PLAYER_DATA
				pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 21, pt.y ), 38 + pPlayerData->data.uLogin, &pVertices, 0xffffffff );
#else	// __SYS_PLAYER_DATA
				pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 21, pt.y ), 38 + pGuildMember->m_nMultiNo, &pVertices, 0xffffffff );
#endif	// __SYS_PLAYER_DATA
			}
			pt.y += m_nFontHeight;// + 3;

			if( bReturn )
				break;
		}


		// 다른 서버 출력
		for( j = 0 ; j < 11 ; ++j )
		{
			if( j == g_Neuz.m_uIdofMulti -1 )
			{
				continue;
			}
			for( int k = 0 ; k < m_nServerCount[j] && i < nMax ; ++k , ++i )
			{
				u_long uGuildMemberId = m_uServerPlayerId[j][k];
				CGuildMember * pGuildMember = pGuild->GetMember( uGuildMemberId );

#if __VER >= 11 // __SYS_PLAYER_DATA
				PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->GetPlayerData( uGuildMemberId );
				LPCSTR szMember	= pPlayerData->szPlayer;
				int nJob	= pPlayerData->data.nJob;
				DWORD dwSex		= pPlayerData->data.nSex;
#else	// __SYS_PLAYER_DATA
				LPCSTR szMember = prj.GetPlayerString( uGuildMemberId );
				int nJob	= pGuildMember->m_nJob;
				DWORD dwSex		= pGuildMember->m_dwSex;
#endif	// __SYS_PLAYER_DATA

				if( szMember )
				{
					// 상태에 따라 색 변경
					DWORD dwColor = 0xffffffff;//adeilson 0xff000000
					if( j == m_nCurSelect )
						dwColor = 0xff6060ff; 
					
					int x = 0, nWidth = m_rectClient.Width();// - 1;
					
					CRect rect( x, pt.y, x + nWidth, pt.y + m_nFontHeight );
					rect.SetRect( x + 3, pt.y + 6, x + 3 + 32, pt.y + 6 + 32 ); 
					
					p2DRender->TextOut( x + 40, pt.y + 3, szMember, dwColor ); 	
					
					if( j == 10 )	// OFFLINE
					{
						// 직업과 성별 아이템 생성
						if( MAX_EXPERT <= nJob )
						{
#if __VER >= 10 // __LEGEND
							if( MAX_PROFESSIONAL <= nJob && nJob < MAX_MASTER )
								pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 16 ) + ( 8 * dwSex ), &pVertices, 0xffff6464 );
							else if( MAX_MASTER <= nJob )
								pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 24 ) + ( 8 * dwSex ), &pVertices, 0xffff6464 );
							else
#ifdef __3RD_LEGEND16
					if ( MAX_HERO <= nJob )
						pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 32 ) + ( 8 * dwSex ), &pVertices, 0xffff6464 );
				else
#endif //__3RD_LEGEND16
#endif //__LEGEND
							pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 6 ) + ( 8 * dwSex ), &pVertices, 0xffff6464 );
						}
						else
						{
							pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ), 12 + nJob + ( 6 * dwSex ), &pVertices, 0xffff6464 );
						}
					}
					else
					{
						// 직업과 성별 아이템 생성
						if( MAX_EXPERT <= nJob )
						{
#if __VER >= 10 // __LEGEND
							if( MAX_PROFESSIONAL <= nJob && nJob < MAX_MASTER )
								pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 16 ) + ( 8 * dwSex ), &pVertices, 0xffffffff );
							else if( MAX_MASTER <= nJob )
								pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 24 ) + ( 8 * dwSex ), &pVertices, 0xffffffff );
							else
#ifdef __3RD_LEGEND16
					if ( MAX_HERO <= nJob )
						pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 32 ) + ( 8 * dwSex ), &pVertices, 0xffffffff );
				else
#endif //__3RD_LEGEND16
#endif //__LEGEND
							pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 6 ) + ( 8 * dwSex ), &pVertices, 0xffffffff );
						}
						else
						{
							pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ), 12 + nJob + ( 6 * dwSex ), &pVertices, 0xffffffff );
						}
						// 멀티 서버 아이템 생성
#if __VER >= 11 // __SYS_PLAYER_DATA
						pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 21, pt.y ), 38 + pPlayerData->data.uLogin, &pVertices, 0xffffffff );
#else	// __SYS_PLAYER_DATA
						pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 21, pt.y ), 38 + pGuildMember->m_nMultiNo, &pVertices, 0xffffffff );
#endif	// __SYS_PLAYER_DATA
					}
				}
				pt.y += m_nFontHeight;// + 3;

				if( bReturn )
					break;
			}
		}

		pWndWorld->m_texMsgIcon.Render( m_pApp->m_pd3dDevice, pVertex, ( (int) pVertices - (int) pVertex ) / sizeof( TEXTUREVERTEX2 ) );
		safe_delete_array( pVertex );
	}
}
void CWndGuildCtrl::OnLButtonUp( UINT nFlags, CPoint point )
{
	m_bDrag = FALSE;
	
	CPoint pt( 3, 3 );
	CRect rect;
	
	pt.y -= (m_nFontHeight) * m_wndScrollBar.GetScrollPos();
	int count = 0;	
	for( int j = 0 ; j < m_nServerCount[g_Neuz.m_uIdofMulti-1] ; ++j, ++count )
	{
		rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
		if( rect.PtInRect( point ) )
		{
			m_nCurSelect = count;
		}
		pt.y += m_nFontHeight;
	}
	
	for( j = 0 ; j < 11 ; ++j )
	{
		if( j == g_Neuz.m_uIdofMulti -1 )
		{
			continue;
		}
		for( int k = 0 ; k < m_nServerCount[j] ; ++k, ++count )
		{
			rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
			if( rect.PtInRect( point ) )
			{
				m_nCurSelect = count;
			}
			pt.y += m_nFontHeight;
		}
	}
}
void CWndGuildCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
}
BOOL CWndGuildCtrl::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	return TRUE;
}
void CWndGuildCtrl::OnLButtonDown( UINT nFlags, CPoint point )
{
}
void CWndGuildCtrl::OnRButtonUp( UINT nFlags, CPoint point )
{
}

void CWndGuildCtrl::OnRButtonDown( UINT nFlags, CPoint point )
{
}
void CWndGuildCtrl::OnRButtonDblClk( UINT nFlags, CPoint point)
{
}
void CWndGuildCtrl::OnSize( UINT nType, int cx, int cy )
{
	CRect rect = GetWindowRect();
	rect.left = rect.right - 15;
	m_wndScrollBar.SetWndRect( rect );

	int nPage, nRange;
	nPage = GetClientRect().Height() / m_nFontHeight;
	CGuild * pGuild = g_pPlayer->GetGuild();
	if( pGuild )
		nRange = pGuild->GetSize();
	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );

	CWndBase::OnSize( nType, cx, cy);
}
void CWndGuildCtrl::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect( 3, 3 );

	if( bOnSize )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
}
void CWndGuildCtrl::PaintFrame( C2DRender* p2DRender )
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
	if( 1 ) //m_dwListCtrlStyle == WLVS_REPORT )
	{
		m_nFontHeight = 20;//GetFontHeight( &g_2DRender );
		nPage = GetClientRect().Height() / m_nFontHeight;
		CGuild * pGuild = g_pPlayer->GetGuild();
		if( pGuild )
			nRange = pGuild->GetSize();
	}
	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );
}
BOOL CWndGuildCtrl::OnEraseBkgnd( C2DRender* p2DRender )
{
	return TRUE;
}
BOOL CWndGuildCtrl::SetItem( const LVITEM* pItem )
{
	return TRUE;
}
int CWndGuildCtrl::InsertItem( const LVITEM* pItem )
{
	return TRUE;
}
int CWndGuildCtrl::GetItemCount() const
{
	return 1;//m_aItems.GetSize();
}
int CWndGuildCtrl::InsertColumn( int nCol, const LVCOLUMN* pColumn )
{
	return nCol;
}

void CWndGuildCtrl::ScrollBarPos( int nPos )
{
	m_wndScrollBar.SetScrollPos( nPos );
}

void CWndGuildCtrl::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	CGuildMember * pGuildMember = NULL;
	int nSelect = GetSelect( point, &pGuildMember );
	if( nSelect != -1 )
	{
#if __VER >= 11 // __SYS_PLAYER_DATA
		PlayerData* pPlayerData		= CPlayerDataCenter::GetInstance()->GetPlayerData( pGuildMember->m_idPlayer );
		if( pPlayerData->data.uLogin > 0 )
#else	// __SYS_PLAYER_DATA
		if( pGuildMember->m_nLogin == 1 )
#endif	// __SYS_PLAYER_DATA
		{
			m_nCurSelect = nSelect;
			if( pGuildMember->m_idPlayer != g_pPlayer->m_idPlayer )
			{
#if __VER >= 11 // __SYS_PLAYER_DATA
				const char* szMember	= pPlayerData->szPlayer;
#else	// __SYS_PLAYER_DATA
				LPCSTR szMember = prj.GetPlayerString( pGuildMember->m_idPlayer );
#endif	// __SYS_PLAYER_DATA
				if( szMember )
				{
					CWndMessage* pWndMessage = g_WndMng.OpenMessage( szMember );
				}
			}
			else
			{
				CString szMessage;
				szMessage = prj.GetText(TID_GAME_MSGSELFSENDERROR);                               //szMessage += "자신에게는 메세지를 보낼수 없음";
				g_WndMng.PutString( szMessage, NULL, prj.GetTextColor(TID_GAME_MSGSELFSENDERROR) );
			}
		}
	}	
}


int CWndGuildCtrl::GetSelect( CPoint point, CGuildMember ** lppGuildMember )
{
	CPoint pt( 3, 3 );
	CRect rect;
	pt.y -= (m_nFontHeight) * m_wndScrollBar.GetScrollPos();

	pt.y += ( m_nFontHeight * m_nDrawCount );
	int count = 0;	
	CGuild * pGuild = g_pPlayer->GetGuild();
	if( pGuild )
	{
		for( int j = 0 ; j < m_nServerCount[g_Neuz.m_uIdofMulti-1] ; ++j, ++count )
		{
			rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
			if( rect.PtInRect( point ) )
			{
				u_long uGuildMemberId = m_uServerPlayerId[g_Neuz.m_uIdofMulti-1][j];
				CGuildMember * lpGuildMember = pGuild->GetMember( uGuildMemberId );
				if( lpGuildMember )
					*lppGuildMember = lpGuildMember;
				return count;
			}
			pt.y += m_nFontHeight;
		}

		for( j = 0 ; j < 11 ; ++j )
		{
			if( j == g_Neuz.m_uIdofMulti -1 )
			{
				continue;
			}
			for( int k = 0 ; k < m_nServerCount[j] ; ++k, ++count )
			{
				rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
				if( rect.PtInRect( point ) )
				{
					u_long uGuildMemberId = m_uServerPlayerId[j][k];
					CGuildMember * lpGuildMember = pGuild->GetMember( uGuildMemberId );
					if( lpGuildMember )
						*lppGuildMember = lpGuildMember;
					return count;
				}
				pt.y += m_nFontHeight;
			}
		}
	}
	if( lppGuildMember )
		*lppGuildMember = NULL;
	return -1;
}

void CWndGuildCtrl::GetSelectFriend( int SelectCount, CGuildMember ** lppFriend )
{
}

u_long CWndGuildCtrl::GetSelectId( int SelectCount )
{
	return -1;
}
