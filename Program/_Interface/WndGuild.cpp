#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndGuildTabInfo.h"
#include "WndGuildTabApp.h"
#include "WndGuildTabWar.h"
#include "WndGuild.h"
#include "DPClient.h"
extern	CDPClient	g_DPlay;


#include "guild.h"
extern	CGuildMng	g_GuildMng;

/****************************************************
  WndId : APP_GUILD - 길드창
  CtrlId : WIDC_TABCTRL1 - TabCtrl
****************************************************/
CWndGuild::CWndGuild() 
{ 
	Error( "CWndGuild::CWndGuild()" );
} 
CWndGuild::~CWndGuild() 
{ 
} 
void CWndGuild::OnDraw( C2DRender* p2DRender ) 
{ 
} 

void CWndGuild::UpdateDataAll()
{
	m_WndGuildTabInfo.UpdateData();
	m_WndGuildTabApp.UpdateData();
	m_WndGuildTabMember.UpdateData();
	m_WndGuildTabWar.UpdateData();

#if __VER >= 15 // __GUILD_HOUSE
	m_WndGuildTabPower.UpdateData( );
#endif
}

void CWndGuild::OnInitialUpdate() 
{ 
	Error( "CWndGuild::OnInitialUpdate() - IN" );
	Error( "CWndGuild::OnInitialUpdate(CWndNeuz::OnInitialUpdate()) - IN" );
	CWndNeuz::OnInitialUpdate(); 
	Error( "CWndGuild::OnInitialUpdate(CWndNeuz::OnInitialUpdate()) - OUT" );
	// 여기에 코딩하세요
	
	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	CRect rect = GetClientRect();
	rect.left = 5;
	rect.top = 0;
	//pWndTabCtrl->Create( WBS_NOFRAME, rect, this, 12 );
	
	m_WndGuildTabInfo.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILD_TABINFO );
	m_WndGuildTabApp.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILD_TABAPPELLATION );
#if __VER >= 12 // __CSC_VER12_2
	m_WndGuildTabMember.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILD_TABMEMBER_EX );
#else //__CSC_VER12_2
	m_WndGuildTabMember.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILD_TABMEMBER );
#endif //__CSC_VER12_2
	m_WndGuildTabWar.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILD_TABGUILDWAR );

#if __VER >= 15 // __GUILD_HOUSE
	m_WndGuildTabPower.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILD_TAPPOWER );
#endif
	
	WTCITEM tabTabItem;
	
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_GAME_TOOLTIP_GUILDINFO);
	tabTabItem.pWndBase = &m_WndGuildTabInfo;
	pWndTabCtrl->InsertItem( 0, &tabTabItem );


	tabTabItem.pszText = prj.GetText(TID_GAME_TOOLTIP_GUILDMEMBER);
	tabTabItem.pWndBase = &m_WndGuildTabMember;
	pWndTabCtrl->InsertItem( 1, &tabTabItem );
	
	tabTabItem.pszText = prj.GetText(TID_GAME_TOOLTIP_APPELLATION);
	tabTabItem.pWndBase = &m_WndGuildTabApp;
	pWndTabCtrl->InsertItem( 2, &tabTabItem );

	tabTabItem.pszText = prj.GetText(TID_GAME_TOOLTIP_GUILDWAR);
	tabTabItem.pWndBase = &m_WndGuildTabWar;
	pWndTabCtrl->InsertItem( 3, &tabTabItem );

#if __VER >= 15 // __GUILD_HOUSE
	tabTabItem.pszText = prj.GetText(TID_GAME_TOOLTIP_AGIT);
	tabTabItem.pWndBase = &m_WndGuildTabPower;
	pWndTabCtrl->InsertItem( 4, &tabTabItem );
#endif


#ifdef __S_SERVER_UNIFY
	if( g_WndMng.m_bAllAction == FALSE )
	{
		Destroy();
		return;
	}
#endif // __S_SERVER_UNIFY
	
	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();

	{
		CRect r = GetWindowRect( TRUE );
		m_WndGuildTabMember.m_rectParent = r;
	}	

	Error( "CWndGuild::OnInitialUpdate() - OUT" );
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndGuild::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	if( g_eLocal.GetState( EVE_SCHOOL ) )
		return FALSE;

	// Daisy에서 설정한 리소스로 윈도를 연다.
	Error( "CWndGuild::Initialize() - IN" );
	BOOL bFlag = CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILD, 0, CPoint( 0, 0 ), pWndParent );
	Error( "CWndGuild::Initialize() - OUT" );

	return bFlag;
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndGuild::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndGuild::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndGuild::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndGuild::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndGuild::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndGuild::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
void CWndGuild::OnMouseMove(UINT nFlags, CPoint point )
{
#ifdef _DEBUG
	m_WndGuildTabInfo.m_nMx = point.x;
	m_WndGuildTabInfo.m_nMy = point.y;
#endif
}

#if __VER >= 15 // __GUILD_HOUSE
void CWndGuild::SetCurTab( int index )
{
	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	assert( pWndTabCtrl );

	pWndTabCtrl->SetCurSel( index );
}
#endif







//---------------------------------------------------------------------------------------------------------------------------
// 길드 초청 확인창
//---------------------------------------------------------------------------------------------------------------------------
CWndGuildConfirm::CWndGuildConfirm() 
{ 
	m_idMaster = 0;
//	m_idPlayer = 0;
} 
CWndGuildConfirm::~CWndGuildConfirm() 
{ 
} 
void CWndGuildConfirm::SetGuildName( char* pGuildName )
{
	strGuildName.Format( "%s", pGuildName );
}
void CWndGuildConfirm::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndGuildConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	CWndEdit* pEdit = (CWndEdit*)GetDlgItem( WIDC_STATIC3 );
	pEdit->SetTitle( strGuildName );
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( ( rectRoot.right - rectWindow.Width() ) / 2, 70 );
	Move( point );		
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndGuildConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILD_INVATE, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndGuildConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndGuildConfirm::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndGuildConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndGuildConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndGuildConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK || message == EN_RETURN )
	{
		if( g_pPlayer )
		{
//			g_DPlay.SendAddGuildMember( m_idMaster, m_idPlayer );
			GUILD_MEMBER_INFO	info;
			info.idPlayer	= g_pPlayer->m_idPlayer;
#if __VER < 11 // __SYS_PLAYER_DATA
			info.nLevel		= g_pPlayer->GetLevel();
			info.nJob	= g_pPlayer->GetJob();
			info.dwSex	= g_pPlayer->GetSex();
#endif	// __SYS_PLAYER_DATA
			g_DPlay.SendAddGuildMember( m_idMaster, info, g_pPlayer->IsAuthorization( AUTH_GAMEMASTER ) );
		}
		Destroy();
	}
	else if( nID == WIDC_NO )
	{
		Destroy();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

