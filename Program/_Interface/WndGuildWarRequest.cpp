#include "stdafx.h"
#include "ResData.h"
#include "DefineText.h"
#include "WndGuildWarRequest.h"

#include "dpclient.h"

extern	CDPClient	g_DPlay;

/****************************************************
  WndId : APP_GUILD_WARREQUEST - 길드전요청창
  CtrlId : WIDC_EDIT1 - 
  CtrlId : WIDC_YES - Button
  CtrlId : WIDC_NO - No
****************************************************/

CWndGuildWarRequest::CWndGuildWarRequest() 
{ 
	m_idEnemyGuild = 0;
} 
CWndGuildWarRequest::~CWndGuildWarRequest() 
{ 
	m_idEnemyGuild = 0;
} 
void CWndGuildWarRequest::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndGuildWarRequest::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	CGuild *pEnemyGuild = g_GuildMng.GetGuild( m_idEnemyGuild );
	if( pEnemyGuild )
	{
		CWndEdit *pWndEdit = (CWndEdit *)GetDlgItem( WIDC_EDIT1 );

		CString strText;
		strText.Format( prj.GetText( TID_GAME_GUILDWARREQUEST ), pEnemyGuild->m_szGuild, m_szMaster );
		pWndEdit->SetString( strText );
		pWndEdit->EnableWindow( FALSE );
	}

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndGuildWarRequest::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILD_WARREQUEST, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndGuildWarRequest::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndGuildWarRequest::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndGuildWarRequest::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndGuildWarRequest::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndGuildWarRequest::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndGuildWarRequest::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case WIDC_YES:
		// 전쟁을 수락함.
		g_DPlay.SendAcptWar( g_pPlayer->m_idPlayer, m_idEnemyGuild );
		Destroy();
		break;
	case WIDC_NO:
		Destroy();
		break;
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

