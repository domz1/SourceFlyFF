#include "stdafx.h"
#include "ResData.h"
#include "WndGuildWarDecl.h"
#include "dpclient.h"

extern	CDPClient	g_DPlay;

/****************************************************
  WndId : APP_GUILD_WAR - 길드전선언창
  CtrlId : WIDC_OK - Button
  CtrlId : WIDC_CANCEL - Button
  CtrlId : WIDC_STATIC1 - 상대길드
  CtrlId : WIDC_STATIC2 - 대전페냐
  CtrlId : WIDC_EDIT1 - 
  CtrlId : WIDC_EDIT2 - 
****************************************************/


CWndGuildWarDecl::CWndGuildWarDecl() 
{ 
} 
CWndGuildWarDecl::~CWndGuildWarDecl() 
{ 
} 
void CWndGuildWarDecl::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndGuildWarDecl::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	CWndEdit *pWndName = (CWndEdit *)GetDlgItem( WIDC_EDIT1 );	// 상대길드명.
	CWndEdit *pWndPenya = (CWndEdit *)GetDlgItem( WIDC_EDIT2 );	// 전쟁자금.

	pWndName->SetTabStop( TRUE );
	pWndPenya->SetTabStop( TRUE );
	pWndName->SetFocus();
	
	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndGuildWarDecl::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILD_WAR, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndGuildWarDecl::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndGuildWarDecl::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndGuildWarDecl::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndGuildWarDecl::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndGuildWarDecl::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndGuildWarDecl::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case WIDC_OK:
		{
			CWndEdit *pWndEdit = (CWndEdit *)GetDlgItem( WIDC_EDIT1 );
			if( g_pPlayer )
			{
				CString strGuild	= pWndEdit->GetString();
				if( strGuild.GetLength() >=3 && strGuild.GetLength() < MAX_G_NAME )
				{
					strGuild.TrimLeft();	strGuild.TrimRight();
					g_DPlay.SendDeclWar( g_pPlayer->m_idPlayer, (LPSTR)(LPCSTR)strGuild );
					Destroy();
				}
			}
		}
		break;
	case WIDC_CANCEL:
		Destroy();
		break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


