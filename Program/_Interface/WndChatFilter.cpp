#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndChatFilter.h"

#include "WndManager.h"

#include "DPClient.h"
extern	CDPClient	g_DPlay;


/****************************************************
  WndId : APP_CHAT_FILTER - 채팅 필터
  CtrlId : WIDC_CHECK1 - 일반 채팅
  CtrlId : WIDC_CHECK2 - 귓속말
  CtrlId : WIDC_CHECK3 - 외치기
  CtrlId : WIDC_CHECK4 - 파티
  CtrlId : WIDC_CHECK5 - 길드
  CtrlId : WIDC_CHECK6 - 도움말, 화면
  CtrlId : WIDC_CHECK7 - 도움말, 챗 

  CtrlId : WIDC_RADIO1 - 게임, 화면 
  CtrlId : WIDC_RADIO2 - 게임, 챗 
  CtrlId : WIDC_RADIO3 - 알림, 화면 
  CtrlId : WIDC_RADIO4 - 알림, 챗 

  CtrlId : WIDC_STATIC1 - 채팅 채널
  CtrlId : WIDC_STATIC2 - 매시지 필터
****************************************************/
CWndChatFilter::CWndChatFilter() 
{ 
} 
CWndChatFilter::~CWndChatFilter() 
{ 
} 
void CWndChatFilter::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndChatFilter::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	SetButtonStatus();

#if __VER >= 8 //__Y_CHAT_SYSTEM_8
	CRect rect = GetClientRect();
	SetWndRect( CRect(0, 0, rect.right, rect.bottom - 130));

	CWndGroupBox* pWndGroup = (CWndGroupBox*)GetDlgItem( WIDC_GROUPBOX2 );
	if( pWndGroup )
		pWndGroup->SetVisible(FALSE);	
#endif //__Y_CHAT_SYSTEM_8

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
void CWndChatFilter::SetButtonStatus()
{
	CWndButton* pWndCheck1 = (CWndButton*)GetDlgItem( WIDC_CHECK1 );
	CWndButton* pWndCheck2 = (CWndButton*)GetDlgItem( WIDC_CHECK2 );
	CWndButton* pWndCheck3 = (CWndButton*)GetDlgItem( WIDC_CHECK3 );
	CWndButton* pWndCheck4 = (CWndButton*)GetDlgItem( WIDC_CHECK4 );
	CWndButton* pWndCheck5 = (CWndButton*)GetDlgItem( WIDC_CHECK5 );
#if __VER >= 8 //__Y_CHAT_SYSTEM_8	

#else //#if __VER >= 8
	CWndButton* pWndCheck6 = (CWndButton*)GetDlgItem( WIDC_CHECK6 );
	CWndButton* pWndCheck7 = (CWndButton*)GetDlgItem( WIDC_CHECK7 );

	CWndButton* pWndRadio1 = (CWndButton*)GetDlgItem( WIDC_RADIO1 );
	CWndButton* pWndRadio2 = (CWndButton*)GetDlgItem( WIDC_RADIO2 );
	CWndButton* pWndRadio3 = (CWndButton*)GetDlgItem( WIDC_RADIO3 );
	CWndButton* pWndRadio4 = (CWndButton*)GetDlgItem( WIDC_RADIO4 );
#endif //__Y_CHAT_SYSTEM_8

	int nChannel = 0;
	switch( CWndChat::m_nChatChannel )
	{
		case CHATSTY_GENERAL: nChannel = 0; break;
		case CHATSTY_WHISPER: nChannel = 1; break;
		case CHATSTY_SHOUT  : nChannel = 2; break;
		case CHATSTY_PARTY  : nChannel = 3; break;
		case CHATSTY_GUILD  : nChannel = 4; break;
	}
	DWORD dwChatFilter = g_Option.m_dwChatFilter[ nChannel ];
	if( dwChatFilter & CHATSTY_GENERAL       ) pWndCheck1->SetCheck( TRUE );
	if( dwChatFilter & CHATSTY_WHISPER       ) pWndCheck2->SetCheck( TRUE );
	if( dwChatFilter & CHATSTY_SHOUT         ) pWndCheck3->SetCheck( TRUE );
	if( dwChatFilter & CHATSTY_PARTY         ) pWndCheck4->SetCheck( TRUE );
	if( dwChatFilter & CHATSTY_GUILD         ) pWndCheck5->SetCheck( TRUE );

#if __VER >= 8 //__Y_CHAT_SYSTEM_8

#else //#if __VER >= 8
	DWORD dwChatFilterEtc = g_Option.m_dwChatFilterEtc;

	if( dwChatFilterEtc & CHATSTY_HELP_CLIENT   ) pWndCheck6->SetCheck( TRUE );
	if( dwChatFilterEtc & CHATSTY_HELP_CHAT     ) pWndCheck7->SetCheck( TRUE );
	if( dwChatFilterEtc & CHATSTY_GAME_CLIENT   ) pWndRadio1->SetCheck( TRUE );
	if( dwChatFilterEtc & CHATSTY_GAME_CHAT     ) pWndRadio2->SetCheck( TRUE );
	if( dwChatFilterEtc & CHATSTY_SYSTEM_CLIENT ) pWndRadio3->SetCheck( TRUE );
	if( dwChatFilterEtc & CHATSTY_SYSTEM_CHAT   ) pWndRadio4->SetCheck( TRUE );
#endif//__Y_CHAT_SYSTEM_8

	if( CWndChat::m_nChatChannel == CHATSTY_GENERAL ) pWndCheck1->EnableWindow( FALSE );
	if( CWndChat::m_nChatChannel == CHATSTY_WHISPER ) pWndCheck2->EnableWindow( FALSE );
	if( CWndChat::m_nChatChannel == CHATSTY_SHOUT   ) pWndCheck3->EnableWindow( FALSE );
	if( CWndChat::m_nChatChannel == CHATSTY_PARTY   ) pWndCheck4->EnableWindow( FALSE );
	if( CWndChat::m_nChatChannel == CHATSTY_GUILD   ) pWndCheck5->EnableWindow( FALSE );

}
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndChatFilter::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_CHAT_FILTER, WBS_MODAL, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndChatFilter::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndChatFilter::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndChatFilter::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndChatFilter::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndChatFilter::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndChatFilter::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	CWndButton* pWndCheck1 = (CWndButton*)GetDlgItem( WIDC_CHECK1 );
	CWndButton* pWndCheck2 = (CWndButton*)GetDlgItem( WIDC_CHECK2 );
	CWndButton* pWndCheck3 = (CWndButton*)GetDlgItem( WIDC_CHECK3 );
	CWndButton* pWndCheck4 = (CWndButton*)GetDlgItem( WIDC_CHECK4 );
	CWndButton* pWndCheck5 = (CWndButton*)GetDlgItem( WIDC_CHECK5 );
#if __VER >= 8 //__Y_CHAT_SYSTEM_8

#else //#if __VER >= 8 //
	CWndButton* pWndCheck6 = (CWndButton*)GetDlgItem( WIDC_CHECK6 );
	CWndButton* pWndCheck7 = (CWndButton*)GetDlgItem( WIDC_CHECK7 );

	CWndButton* pWndRadio1 = (CWndButton*)GetDlgItem( WIDC_RADIO1 );
	CWndButton* pWndRadio2 = (CWndButton*)GetDlgItem( WIDC_RADIO2 );
	CWndButton* pWndRadio3 = (CWndButton*)GetDlgItem( WIDC_RADIO3 );
	CWndButton* pWndRadio4 = (CWndButton*)GetDlgItem( WIDC_RADIO4 );
#endif //__Y_CHAT_SYSTEM_8

	CWndChat* pWndChat = (CWndChat*)g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
	int nChannel = 0;
	switch( pWndChat->m_nChatChannel )
	{
		case CHATSTY_GENERAL: nChannel = 0; break;
		case CHATSTY_WHISPER: nChannel = 1; break;
		case CHATSTY_SHOUT  : nChannel = 2; break;
		case CHATSTY_PARTY  : nChannel = 3; break;
		case CHATSTY_GUILD  : nChannel = 4; break;
	}
	switch( nID )
	{
	case WIDC_CHECK1 :
		if( pWndCheck1->GetCheck() )
			g_Option.m_dwChatFilter[nChannel] |= CHATSTY_GENERAL;
		else
			g_Option.m_dwChatFilter[nChannel] &= ~CHATSTY_GENERAL;
		SetButtonStatus();
		break;
	case WIDC_CHECK2 :
		if( pWndCheck2->GetCheck() )
			g_Option.m_dwChatFilter[nChannel] |= CHATSTY_WHISPER;
		else
			g_Option.m_dwChatFilter[nChannel] &= ~CHATSTY_WHISPER;
		SetButtonStatus();
		break;
	case WIDC_CHECK3 :
		if( pWndCheck3->GetCheck() )
			g_Option.m_dwChatFilter[nChannel] |= CHATSTY_SHOUT;
		else
			g_Option.m_dwChatFilter[nChannel] &= ~CHATSTY_SHOUT;
		SetButtonStatus();
		break;
	case WIDC_CHECK4 :
		if( pWndCheck4->GetCheck() )
			g_Option.m_dwChatFilter[nChannel] |= CHATSTY_PARTY;
		else
			g_Option.m_dwChatFilter[nChannel] &= ~CHATSTY_PARTY;
		SetButtonStatus();
		break;
	case WIDC_CHECK5 :
		if( pWndCheck5->GetCheck() )
			g_Option.m_dwChatFilter[nChannel] |= CHATSTY_GUILD;
		else
			g_Option.m_dwChatFilter[nChannel] &= ~CHATSTY_GUILD;
		SetButtonStatus();
		break;
#if __VER >= 8 //__Y_CHAT_SYSTEM_8
#else //#if __VER >= 8 //
	case WIDC_CHECK6 :
		if( pWndCheck6->GetCheck() )
			g_Option.m_dwChatFilterEtc |= CHATSTY_HELP_CLIENT;
		else
			g_Option.m_dwChatFilterEtc &= ~CHATSTY_HELP_CLIENT;
		SetButtonStatus();
		break;
	case WIDC_CHECK7 :
		if( pWndCheck7->GetCheck() )
			g_Option.m_dwChatFilterEtc |= CHATSTY_HELP_CHAT;
		else
			g_Option.m_dwChatFilterEtc &= ~CHATSTY_HELP_CHAT;
		SetButtonStatus();
		break;
	case WIDC_RADIO1 :
		g_Option.m_dwChatFilterEtc |= CHATSTY_GAME_CLIENT;
		g_Option.m_dwChatFilterEtc &= ~CHATSTY_GAME_CHAT;
		break;
	case WIDC_RADIO2 :
		g_Option.m_dwChatFilterEtc |= CHATSTY_GAME_CHAT;
		g_Option.m_dwChatFilterEtc &= ~CHATSTY_GAME_CLIENT;
		break;
	case WIDC_RADIO3 :
		g_Option.m_dwChatFilterEtc |= CHATSTY_SYSTEM_CLIENT;
		g_Option.m_dwChatFilterEtc &= ~CHATSTY_SYSTEM_CHAT;
		break;
	case WIDC_RADIO4 :
		g_Option.m_dwChatFilterEtc |= CHATSTY_SYSTEM_CHAT;
		g_Option.m_dwChatFilterEtc &= ~CHATSTY_SYSTEM_CLIENT;
		break;
#endif //__Y_CHAT_SYSTEM_8
	}
	pWndChat->SetChannel();

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


