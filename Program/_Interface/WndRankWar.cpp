#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndRankWar.h"

/****************************************************
  WndId : APP_RANK_WARTABGIVEUP - 항복패
  CtrlId : WIDC_STATIC1 - 순위
  CtrlId : WIDC_STATIC2 - 길드명칭
  CtrlId : WIDC_STATIC3 - 길드장명칭
  CtrlId : WIDC_STATIC4 - 항복패
****************************************************/

CWndRankWarTabGiveUp::CWndRankWarTabGiveUp() 
{ 
} 
CWndRankWarTabGiveUp::~CWndRankWarTabGiveUp() 
{ 
} 
void CWndRankWarTabGiveUp::OnDraw( C2DRender* p2DRender ) 
{ 
	int		i, sx, sy;
	DWORD	dwColor;
	int		nFlag = CGuildRank::R4;
	
	sx = 8;
	sy = 32;	
	
	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
	CGuildRank* pGuildRank = CGuildRank::Instance();
	
	TEXTUREVERTEX2* pVertex = new TEXTUREVERTEX2[ pGuildRank->m_Total[nFlag] * 6 ];
	TEXTUREVERTEX2* pVertices = pVertex;
	
	for( i = m_nCurrentList; i < m_nCurrentList + 10; ++i )
	{
		if( i >= pGuildRank->m_Total[nFlag] )
			break;
		
		dwColor = D3DCOLOR_ARGB( 255, 0, 0, 0 )/*adeilson*/;
		
		// 길드 순위 로그
		int nNo = i + 1;
		if( nNo < 2 )
			nNo = 0;
		else if( nNo < 4 )
			nNo = 1;
		else if( nNo < 8 )
			nNo = 2;
		else if( nNo < 13 )
			nNo = 3;
		else
			nNo = 4;
		pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( sx +  0, sy-3 ), 49 + nNo, &pVertices, 0xffffffff );	
		
		// 길드 순위
		p2DRender->TextOut( sx +   20, sy, i+1, dwColor  );
		
		// 길드 로그
		
		if( pGuildRank->m_Ranking[nFlag][i].m_dwLogo != 0 )
		{
			
			CPoint point = CPoint( sx + 42, sy - 5 );
			
			p2DRender->m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
			p2DRender->m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
			pWndWorld->m_pTextureLogo[pGuildRank->m_Ranking[nFlag][i].m_dwLogo-1].Render( &g_Neuz.m_2DRender, point, 255 );
			p2DRender->m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			p2DRender->m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		}
		
		// 길드명
		CString guildname = pGuildRank->m_Ranking[nFlag][i].m_szGuild;
		p2DRender->TextOut( sx + 72, sy, guildname, dwColor );
				
		// 마스터명 (길 경우 ... 으로)
		CString strMasterName;
		strMasterName.Format("%s", pGuildRank->m_Ranking[nFlag][i].m_szName);
		strMasterName.TrimRight();
		
		if( strMasterName.GetLength() > 12 ) 
		{
			int	nReduceCount = 0;

			for( nReduceCount=0; nReduceCount <12; )
			{
				if( IsDBCSLeadByte( strMasterName[ nReduceCount ] ) )
					nReduceCount+=2;
				else
					nReduceCount++;
			}

			strMasterName = strMasterName.Left( nReduceCount );
			strMasterName+="...";
		}
		p2DRender->TextOut( sx + 210, sy, strMasterName, dwColor );
		
		p2DRender->TextOut( sx + 350, sy, pGuildRank->m_Ranking[nFlag][i].m_nSurrender, dwColor );
		
		sy += 25;
	}
	
	pWndWorld->m_texMsgIcon.Render( m_pApp->m_pd3dDevice, pVertex, ( (int) pVertices - (int) pVertex ) / sizeof( TEXTUREVERTEX2 ) );
	//	delete pVertex;
	SAFE_DELETE_ARRAY( pVertex );
} 
void CWndRankWarTabGiveUp::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	//	m_nSelect = 0;
	m_nCurrentList = 0;
	m_nMxOld = m_nMyOld = 0;

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndRankWarTabGiveUp::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_RANK_WARTABGIVEUP, 0, CPoint( 0, 0 ), pWndParent );
} 


BOOL CWndRankWarTabGiveUp::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndRankWarTabGiveUp::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndRankWarTabGiveUp::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndRankWarTabGiveUp::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndRankWarTabGiveUp::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndRankWarTabGiveUp::OnRButtonDown( UINT nFlags, CPoint point ) 
{ 
	OnLButtonDown( nFlags, point );
} 

void CWndRankWarTabGiveUp::OnMouseMove(UINT nFlags, CPoint point )
{
	if( nFlags & MK_LBUTTON )
	{
		int		nDistY = (m_nMyOld - point.y) / 5;		// 과거 좌표와의 차이.
		int		nFlag = CGuildRank::R4;
		
		m_nCurrentList += nDistY;
		if( m_nCurrentList < 0 )
			m_nCurrentList = 0;
		if( (m_nCurrentList + 10 - 1) >= CGuildRank::Instance()->m_Total[nFlag] )
		{
			m_nCurrentList = CGuildRank::Instance()->m_Total[nFlag] - 10;
			if( m_nCurrentList < 0 )
				m_nCurrentList = 0;
		}
		
	}
	
	m_nMxOld = point.x;
	m_nMyOld = point.y;
}

BOOL CWndRankWarTabGiveUp::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	int		nZoom = 64;
	int		nFlag = CGuildRank::R4;
	
	if( zDelta > 0 )
		m_nCurrentList -= 3;
	else
		m_nCurrentList += 3;
	
	if( m_nCurrentList < 0 )
		m_nCurrentList = 0;
	if( (m_nCurrentList + 10 - 1) >= CGuildRank::Instance()->m_Total[nFlag] )
	{
		m_nCurrentList = CGuildRank::Instance()->m_Total[nFlag] - 10;
		if( m_nCurrentList < 0 )
			m_nCurrentList = 0;
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/****************************************************
  WndId : APP_RANK_WARTABLOSE - Applet
  CtrlId : WIDC_STATIC2 - 길드명칭
  CtrlId : WIDC_STATIC3 - 길드장명칭
  CtrlId : WIDC_STATIC4 - 승
  CtrlId : WIDC_STATIC5 - /
  CtrlId : WIDC_STATIC6 - 패
  CtrlId : WIDC_STATIC1 - 순위
****************************************************/

CWndRankWarTabLose::CWndRankWarTabLose() 
{ 
} 
CWndRankWarTabLose::~CWndRankWarTabLose() 
{ 
} 
void CWndRankWarTabLose::OnDraw( C2DRender* p2DRender ) 
{ 
	int		i, sx, sy;
	DWORD	dwColor;
	int		nFlag = CGuildRank::R3;
	
	sx = 8;
	sy = 32;	
	
	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
	CGuildRank* pGuildRank = CGuildRank::Instance();
	
	TEXTUREVERTEX2* pVertex = new TEXTUREVERTEX2[ pGuildRank->m_Total[nFlag] * 6 ];
	TEXTUREVERTEX2* pVertices = pVertex;
	
	for( i = m_nCurrentList; i < m_nCurrentList + 10; ++i )
	{
		if( i >= pGuildRank->m_Total[nFlag] )
			break;
		
		dwColor = D3DCOLOR_ARGB( 255, 0, 0, 0 )/*adeilson*/;
		
		// 길드 순위 로그
		int nNo = i + 1;
		if( nNo < 2 )
			nNo = 0;
		else if( nNo < 4 )
			nNo = 1;
		else if( nNo < 8 )
			nNo = 2;
		else if( nNo < 13 )
			nNo = 3;
		else
			nNo = 4;
		pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( sx +  0, sy-3 ), 49 + nNo, &pVertices, 0xffffffff );	
		
		// 길드 순위
		p2DRender->TextOut( sx +   20, sy, i+1, dwColor  );
		
		// 길드 로그
		
		if( pGuildRank->m_Ranking[nFlag][i].m_dwLogo != 0 )
		{
			
			CPoint point = CPoint( sx + 42, sy - 5 );
			
			p2DRender->m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
			p2DRender->m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
			pWndWorld->m_pTextureLogo[pGuildRank->m_Ranking[nFlag][i].m_dwLogo-1].Render( &g_Neuz.m_2DRender, point, 255 );
			p2DRender->m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			p2DRender->m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		}
		
		// 길드명
		CString guildname = pGuildRank->m_Ranking[nFlag][i].m_szGuild;
		p2DRender->TextOut( sx + 72, sy, guildname, dwColor );
				
		// 마스터명 (길 경우 ... 으로)
		CString strMasterName;
		strMasterName.Format("%s", pGuildRank->m_Ranking[nFlag][i].m_szName);
		strMasterName.TrimRight();
		
		if( strMasterName.GetLength() > 12 ) 
		{
			int	nReduceCount = 0;

			for( nReduceCount=0; nReduceCount <12; )
			{
				if( IsDBCSLeadByte( strMasterName[ nReduceCount ] ) )
					nReduceCount+=2;
				else
					nReduceCount++;
			}

			strMasterName = strMasterName.Left( nReduceCount );
			strMasterName+="...";
		}
		p2DRender->TextOut( sx + 210, sy, strMasterName, dwColor );
		
		// 승 / 패
		CString strWin;
		strWin.Format( "%5d  /  %5d", pGuildRank->m_Ranking[nFlag][i].m_nWin, pGuildRank->m_Ranking[nFlag][i].m_nLose );
		p2DRender->TextOut( sx + 330, sy, strWin, dwColor );
		
		sy += 25;
	}
	
	pWndWorld->m_texMsgIcon.Render( m_pApp->m_pd3dDevice, pVertex, ( (int) pVertices - (int) pVertex ) / sizeof( TEXTUREVERTEX2 ) );
	//	delete pVertex;
	SAFE_DELETE_ARRAY( pVertex );
} 
void CWndRankWarTabLose::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	//	m_nSelect = 0;
	m_nCurrentList = 0;
	m_nMxOld = m_nMyOld = 0;

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndRankWarTabLose::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_RANK_WARTABLOSE, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndRankWarTabLose::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndRankWarTabLose::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndRankWarTabLose::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndRankWarTabLose::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndRankWarTabLose::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndRankWarTabLose::OnRButtonDown( UINT nFlags, CPoint point ) 
{ 
	OnLButtonDown( nFlags, point );
} 

void CWndRankWarTabLose::OnMouseMove(UINT nFlags, CPoint point )
{
	if( nFlags & MK_LBUTTON )
	{
		int		nDistY = (m_nMyOld - point.y) / 5;		// 과거 좌표와의 차이.
		int		nFlag = CGuildRank::R3;
		
		m_nCurrentList += nDistY;
		if( m_nCurrentList < 0 )
			m_nCurrentList = 0;
		if( (m_nCurrentList + 10 - 1) >= CGuildRank::Instance()->m_Total[nFlag] )
		{
			m_nCurrentList = CGuildRank::Instance()->m_Total[nFlag] - 10;
			if( m_nCurrentList < 0 )
				m_nCurrentList = 0;
		}
		
	}
	
	m_nMxOld = point.x;
	m_nMyOld = point.y;
}

BOOL CWndRankWarTabLose::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	int		nZoom = 64;
	int		nFlag = CGuildRank::R3;
	
	if( zDelta > 0 )
		m_nCurrentList -= 3;
	else
		m_nCurrentList += 3;
	
	if( m_nCurrentList < 0 )
		m_nCurrentList = 0;
	if( (m_nCurrentList + 10 - 1) >= CGuildRank::Instance()->m_Total[nFlag] )
	{
		m_nCurrentList = CGuildRank::Instance()->m_Total[nFlag] - 10;
		if( m_nCurrentList < 0 )
			m_nCurrentList = 0;
	}
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/****************************************************
  WndId : APP_RANK_WARTABWIN - 길드전승리
  CtrlId : WIDC_STATIC1 - 순위
  CtrlId : WIDC_STATIC2 - 길드명칭
  CtrlId : WIDC_STATIC3 - 길드장명칭
  CtrlId : WIDC_STATIC4 - 승
  CtrlId : WIDC_STATIC5 - /
  CtrlId : WIDC_STATIC6 - 패
****************************************************/

CWndRankWarTabWin::CWndRankWarTabWin() 
{ 
} 
CWndRankWarTabWin::~CWndRankWarTabWin() 
{ 
} 
void CWndRankWarTabWin::OnDraw( C2DRender* p2DRender ) 
{ 
	int		i, sx, sy;
	DWORD	dwColor;
	int		nFlag = CGuildRank::R2;
	
	sx = 8;
	sy = 32;	
	
	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
	CGuildRank* pGuildRank = CGuildRank::Instance();
	
	TEXTUREVERTEX2* pVertex = new TEXTUREVERTEX2[ pGuildRank->m_Total[nFlag] * 6 ];
	TEXTUREVERTEX2* pVertices = pVertex;
	
	for( i = m_nCurrentList; i < m_nCurrentList + 10; ++i )
	{
		if( i >= pGuildRank->m_Total[nFlag] )
			break;

		dwColor = D3DCOLOR_ARGB( 255, 0, 0, 0 )/*adeilson*/;
		
		// 길드 순위 로그
		int nNo = i + 1;
		if( nNo < 2 )
			nNo = 0;
		else if( nNo < 4 )
			nNo = 1;
		else if( nNo < 8 )
			nNo = 2;
		else if( nNo < 13 )
			nNo = 3;
		else
			nNo = 4;
		pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( sx +  0, sy-3 ), 49 + nNo, &pVertices, 0xffffffff );	
		
		// 길드 순위
		p2DRender->TextOut( sx +   20, sy, i+1, dwColor  );
		
		// 길드 로그
		
		if( pGuildRank->m_Ranking[nFlag][i].m_dwLogo != 0 )
		{
			
			CPoint point = CPoint( sx + 42, sy - 5 );
			
			p2DRender->m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
			p2DRender->m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
			pWndWorld->m_pTextureLogo[pGuildRank->m_Ranking[nFlag][i].m_dwLogo-1].Render( &g_Neuz.m_2DRender, point, 255 );
			p2DRender->m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			p2DRender->m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		}
		
		// 길드명
		CString guildname = pGuildRank->m_Ranking[nFlag][i].m_szGuild;
		p2DRender->TextOut( sx + 72, sy, guildname, dwColor );
				
		// 마스터명 (길 경우 ... 으로)
		CString strMasterName;
		strMasterName.Format("%s", pGuildRank->m_Ranking[nFlag][i].m_szName);
		strMasterName.TrimRight();
		
		if( strMasterName.GetLength() > 12 ) 
		{
			int	nReduceCount = 0;

			for( nReduceCount=0; nReduceCount <12; )
			{
				if( IsDBCSLeadByte( strMasterName[ nReduceCount ] ) )
					nReduceCount+=2;
				else
					nReduceCount++;
			}

			strMasterName = strMasterName.Left( nReduceCount );
			strMasterName+="...";
		}
		p2DRender->TextOut( sx + 210, sy, strMasterName, dwColor );
		
		// 승 / 패
		CString strWin;
		strWin.Format( "%5d  /  %5d", pGuildRank->m_Ranking[nFlag][i].m_nWin, pGuildRank->m_Ranking[nFlag][i].m_nLose );
		p2DRender->TextOut( sx + 330, sy, strWin, dwColor );
		
		sy += 25;
	}
	
	pWndWorld->m_texMsgIcon.Render( m_pApp->m_pd3dDevice, pVertex, ( (int) pVertices - (int) pVertex ) / sizeof( TEXTUREVERTEX2 ) );
	//	delete pVertex;
	SAFE_DELETE_ARRAY( pVertex );
} 
void CWndRankWarTabWin::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	//	m_nSelect = 0;
	m_nCurrentList = 0;
	m_nMxOld = m_nMyOld = 0;

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndRankWarTabWin::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_RANK_WARTABWIN, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndRankWarTabWin::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndRankWarTabWin::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndRankWarTabWin::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndRankWarTabWin::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndRankWarTabWin::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndRankWarTabWin::OnRButtonDown( UINT nFlags, CPoint point ) 
{ 
	OnLButtonDown( nFlags, point );
} 

void CWndRankWarTabWin::OnMouseMove(UINT nFlags, CPoint point )
{
	if( nFlags & MK_LBUTTON )
	{
		int		nDistY = (m_nMyOld - point.y) / 5;		// 과거 좌표와의 차이.
		int		nFlag = CGuildRank::R2;
		
		m_nCurrentList += nDistY;
		if( m_nCurrentList < 0 )
			m_nCurrentList = 0;
		if( (m_nCurrentList + 10 - 1) >= CGuildRank::Instance()->m_Total[nFlag] )
		{
			m_nCurrentList = CGuildRank::Instance()->m_Total[nFlag] - 10;
			if( m_nCurrentList < 0 )
				m_nCurrentList = 0;
		}
		
	}
	
	m_nMxOld = point.x;
	m_nMyOld = point.y;
}

BOOL CWndRankWarTabWin::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	int		nZoom = 64;
	int		nFlag = CGuildRank::R2;
	
	if( zDelta > 0 )
		m_nCurrentList -= 3;
	else
		m_nCurrentList += 3;
	
	if( m_nCurrentList < 0 )
		m_nCurrentList = 0;
	if( (m_nCurrentList + 10 - 1) >= CGuildRank::Instance()->m_Total[nFlag] )
	{
		m_nCurrentList = CGuildRank::Instance()->m_Total[nFlag] - 10;
		if( m_nCurrentList < 0 )
			m_nCurrentList = 0;
	}
	
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/****************************************************
  WndId : APP_RANK_WAR - 길드전승패 랭킹창
  CtrlId : WIDC_TABCTRL1 - 
****************************************************/

CWndRankWar::CWndRankWar() 
{ 
} 
CWndRankWar::~CWndRankWar() 
{ 
} 
void CWndRankWar::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndRankWar::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	

	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	CRect rect = GetClientRect();
	rect.left = 5;
	rect.top = 0;

	m_WndRankWarTabWin.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_RANK_WARTABWIN );
	m_WndRankWarTabLose.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_RANK_WARTABLOSE );
	m_WndRankWarTabGiveUp.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_RANK_WARTABGIVEUP );
	
	WTCITEM tabTabItem;
	
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText( TID_GAME_TOOLTIP_RANKWIN );
	tabTabItem.pWndBase = &m_WndRankWarTabWin;
	pWndTabCtrl->InsertItem( 0, &tabTabItem );
	
	
	tabTabItem.pszText = prj.GetText( TID_GAME_TOOLTIP_RANKLOSE );
	tabTabItem.pWndBase = &m_WndRankWarTabLose;
	pWndTabCtrl->InsertItem( 1, &tabTabItem );
	
	tabTabItem.pszText = prj.GetText( TID_GAME_TOOLTIP_RANKGIVEUP );
	tabTabItem.pWndBase = &m_WndRankWarTabGiveUp;
	pWndTabCtrl->InsertItem( 2, &tabTabItem );	

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndRankWar::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_RANK_WAR, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndRankWar::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndRankWar::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndRankWar::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndRankWar::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndRankWar::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

