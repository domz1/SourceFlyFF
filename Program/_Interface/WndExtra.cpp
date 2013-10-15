#include "stdafx.h"
#include "AppDefine.h"
#include "WndExtra.h"

#if 0
CWndExtraCapture::CWndExtraCapture() 
{ 
} 
CWndExtraCapture::~CWndExtraCapture() 
{ 
} 
void CWndExtraCapture::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndExtraCapture::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndExtraCapture::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_EXTRA_CAPTURE, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndExtraCapture::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndExtraCapture::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndExtraCapture::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndExtraCapture::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndExtraCapture::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndExtraCapture::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

#endif // if 0