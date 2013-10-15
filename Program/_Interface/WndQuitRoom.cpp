
#include "stdafx.h"
#include "resData.h"
#include "WndQuitRoom.h"

#include "DPClient.h"
extern	CDPClient	g_DPlay;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if __VER >= 13 // __HOUSING
/****************************************************
  WndId : APP_QUIT_ROOM - 미니룸
  CtrlId : WIDC_STATIC1 - 미니룸에서 퇴장 하시겠습니까?
  CtrlId : WIDC_BUTTON1 - Button
  CtrlId : WIDC_BUTTON2 - Button
****************************************************/

CWndQuitRoom::CWndQuitRoom() 
{ 
} 
CWndQuitRoom::~CWndQuitRoom() 
{ 
} 
void CWndQuitRoom::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndQuitRoom::OnInitialUpdate() 
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
BOOL CWndQuitRoom::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_QUIT_ROOM, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndQuitRoom::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndQuitRoom::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndQuitRoom::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndQuitRoom::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndQuitRoom::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndQuitRoom::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch(nID)
	{
		case WIDC_BUTTON1:// ok 버튼
			g_DPlay.SendHousingGoOut();
			Destroy();
			break;

		case WIDC_BUTTON2:// cancel 버튼
			Destroy();
			break;
	};
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

#endif // __HOUSING