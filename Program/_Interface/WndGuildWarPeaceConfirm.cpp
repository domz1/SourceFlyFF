#include "stdafx.h"
#include "ResData.h"
#include "WndGuildWarPeaceConfirm.h"
#include "dpclient.h"

extern	CDPClient	g_DPlay;


/****************************************************
  WndId : APP_GUILD_WARPEACECONFIRM - 휴전승인창
  CtrlId : WIDC_YES - Yes
  CtrlId : WIDC_NO - No
  CtrlId : WIDC_STATIC1 - 휴전요청이 들어왔습니다.
  CtrlId : WIDC_STATIC2 - 승인하시겠습니까?
****************************************************/

CWndGuildWarPeaceConfirm::CWndGuildWarPeaceConfirm() 
{ 
} 
CWndGuildWarPeaceConfirm::~CWndGuildWarPeaceConfirm() 
{ 
} 
void CWndGuildWarPeaceConfirm::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndGuildWarPeaceConfirm::OnInitialUpdate() 
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
BOOL CWndGuildWarPeaceConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILD_WARPEACECONFIRM, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndGuildWarPeaceConfirm::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndGuildWarPeaceConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndGuildWarPeaceConfirm::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndGuildWarPeaceConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndGuildWarPeaceConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndGuildWarPeaceConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case WIDC_YES:
		// 휴전을 승락 한다는것을 서버로 보냄.
		if( g_pPlayer )
			g_DPlay.SendAcptTruce( g_pPlayer->m_idPlayer );
		Destroy();
		break;
	case WIDC_NO:
		Destroy();
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

