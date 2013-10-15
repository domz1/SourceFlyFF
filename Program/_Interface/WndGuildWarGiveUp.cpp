#include "stdafx.h"
#include "ResData.h"
#include "WndGuildWarGiveUp.h"
#include "dpclient.h"

extern	CDPClient	g_DPlay;


/****************************************************
  WndId : APP_GUILD_WARGIVEUP - 항복창
  CtrlId : WIDC_YES - Button
  CtrlId : WIDC_NO - Button
  CtrlId : WIDC_STATIC1 - 길드전에서 항복을 하겠습니까?
****************************************************/

CWndGuildWarGiveUp::CWndGuildWarGiveUp() 
{ 
} 
CWndGuildWarGiveUp::~CWndGuildWarGiveUp() 
{ 
} 
void CWndGuildWarGiveUp::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndGuildWarGiveUp::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndGuildWarGiveUp::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILD_WARGIVEUP, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndGuildWarGiveUp::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndGuildWarGiveUp::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndGuildWarGiveUp::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndGuildWarGiveUp::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndGuildWarGiveUp::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndGuildWarGiveUp::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case WIDC_YES:
		// 항복했다는걸 서버로 보냄.
		if( g_pPlayer )
			g_DPlay.SendSurrender( g_pPlayer->m_idPlayer );
		Destroy();
		break;
	case WIDC_NO:
		Destroy();
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
