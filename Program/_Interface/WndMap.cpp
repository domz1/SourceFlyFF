#include "stdafx.h"
#include "AppDefine.h"
#include "WndMap.h"
#include "WorldMap.h"

/****************************************************
  WndId : APP_MAP - Map
****************************************************/

CWndMap::CWndMap() 
{ 
} 
CWndMap::~CWndMap() 
{ 
} 
void CWndMap::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndMap::OnInitialUpdate() 
{ 

#if __VER >= 9  // __INSERT_MAP

#else
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_ITEM, m_szMapFile), TRUE );
	FitTextureSize();

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
#endif

} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndMap::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 

#if __VER >= 9  // __INSERT_MAP
	CWorldMap* pWorldMap = CWorldMap::GetInstance();


	// 렌더링중이면 끄고 아니면 켠다
	if(pWorldMap->IsRender())	pWorldMap->DeleteWorldMap();
	else						pWorldMap->LoadWorldMap();

	return FALSE;
#else
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_MAP, 0, CPoint( 0, 0 ), pWndParent );
#endif
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndMap::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndMap::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndMap::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndMap::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndMap::OnRButtonUp( UINT nFlags, CPoint point ) 
{ 
	Destroy();
} 
void CWndMap::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndMap::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

