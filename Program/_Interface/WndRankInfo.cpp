#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndRankInfo.h"

/****************************************************
  WndId : APP_RANK_INFOTABLEVEL - 최대레벨합
  CtrlId : WIDC_STATIC1 - 순위
  CtrlId : WIDC_STATIC2 - 길드명칭
  CtrlId : WIDC_STATIC3 - 길드장명칭
  CtrlId : WIDC_STATIC4 - 평균레벨
****************************************************/

CWndRankInfoTabLevel::CWndRankInfoTabLevel() 
{ 
} 
CWndRankInfoTabLevel::~CWndRankInfoTabLevel() 
{ 
} 
void CWndRankInfoTabLevel::OnDraw( C2DRender* p2DRender ) 
{ 
	int		i, sx, sy;
	DWORD	dwColor;
	int		nFlag = CGuildRank::R7;
	
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
		
		// 레벨
		p2DRender->TextOut( sx + 350, sy, (int)( pGuildRank->m_Ranking[nFlag][i].m_AvgLevel ), dwColor );
		
		sy += 25;
	}
	
	pWndWorld->m_texMsgIcon.Render( m_pApp->m_pd3dDevice, pVertex, ( (int) pVertices - (int) pVertex ) / sizeof( TEXTUREVERTEX2 ) );
	SAFE_DELETE_ARRAY( pVertex );
} 
void CWndRankInfoTabLevel::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
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
BOOL CWndRankInfoTabLevel::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_RANK_INFOTABLEVEL, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndRankInfoTabLevel::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndRankInfoTabLevel::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndRankInfoTabLevel::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndRankInfoTabLevel::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndRankInfoTabLevel::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndRankInfoTabLevel::OnRButtonDown( UINT nFlags, CPoint point ) 
{ 
	OnLButtonDown( nFlags, point );
} 

void CWndRankInfoTabLevel::OnMouseMove(UINT nFlags, CPoint point )
{
	if( nFlags & MK_LBUTTON )
	{
		int		nDistY = (m_nMyOld - point.y) / 5;		// 과거 좌표와의 차이.
		int		nFlag = CGuildRank::R7;
		
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

BOOL CWndRankInfoTabLevel::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	int		nZoom = 64;
	int		nFlag = CGuildRank::R7;
	
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/****************************************************
  WndId : APP_RANK_INFOTABPLAYTIME - 누적플레이최대
  CtrlId : WIDC_STATIC1 - 순위
  CtrlId : WIDC_STATIC2 - 길드명칭
  CtrlId : WIDC_STATIC3 - 길드장명칭
****************************************************/

CWndRankInfoTabPlayTime::CWndRankInfoTabPlayTime() 
{ 
} 
CWndRankInfoTabPlayTime::~CWndRankInfoTabPlayTime() 
{ 
} 
void CWndRankInfoTabPlayTime::OnDraw( C2DRender* p2DRender ) 
{ 
	int		i, sx, sy;
	DWORD	dwColor;
	int		nFlag = CGuildRank::R8;
	
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
		
		sy += 25;
	}
	
	pWndWorld->m_texMsgIcon.Render( m_pApp->m_pd3dDevice, pVertex, ( (int) pVertices - (int) pVertex ) / sizeof( TEXTUREVERTEX2 ) );
	//	delete pVertex;
	SAFE_DELETE_ARRAY( pVertex );
} 
void CWndRankInfoTabPlayTime::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

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
BOOL CWndRankInfoTabPlayTime::Initialize( CWndBase* pWndParent, DWORD  ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_RANK_INFOTABPLAYTIME, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndRankInfoTabPlayTime::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndRankInfoTabPlayTime::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndRankInfoTabPlayTime::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndRankInfoTabPlayTime::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndRankInfoTabPlayTime::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndRankInfoTabPlayTime::OnRButtonDown( UINT nFlags, CPoint point ) 
{ 
	OnLButtonDown( nFlags, point );
} 

void CWndRankInfoTabPlayTime::OnMouseMove(UINT nFlags, CPoint point )
{
	if( nFlags & MK_LBUTTON )
	{
		int		nDistY = (m_nMyOld - point.y) / 5;		// 과거 좌표와의 차이.
		int		nFlag = CGuildRank::R8;
		
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

BOOL CWndRankInfoTabPlayTime::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	int		nZoom = 64;
	int		nFlag = CGuildRank::R8;
	
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/****************************************************
  WndId : APP_RANK_INFO - 길드기타 랭킹창
  CtrlId : WIDC_TABCTRL1 - 
****************************************************/

CWndRankInfo::CWndRankInfo() 
{ 
} 
CWndRankInfo::~CWndRankInfo() 
{ 
} 
void CWndRankInfo::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndRankInfo::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	CRect rect = GetClientRect();
	rect.left = 5;
	rect.top = 0;
	
	m_WndRankInfoTabLevel.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_RANK_INFOTABLEVEL );
	m_WndRankInfoTabPlayTime.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_RANK_INFOTABPLAYTIME );
	
	WTCITEM tabTabItem;
	
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText( TID_GAME_TOOLTIP_RANKLEVEL );
	tabTabItem.pWndBase = &m_WndRankInfoTabLevel;
	pWndTabCtrl->InsertItem( 0, &tabTabItem );
	
	
	tabTabItem.pszText = prj.GetText( TID_GAME_TOOLTIP_RANKPLAYTIME );
	tabTabItem.pWndBase = &m_WndRankInfoTabPlayTime;
	pWndTabCtrl->InsertItem( 1, &tabTabItem );
	
	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndRankInfo::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_RANK_INFO, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndRankInfo::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndRankInfo::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndRankInfo::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndRankInfo::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndRankInfo::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

