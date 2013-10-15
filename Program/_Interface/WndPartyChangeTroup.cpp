#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndPartyChangeTroup.h"
#include "WndManager.h"


#include "DPClient.h"
extern	CDPClient	g_DPlay;

#include "party.h"
extern	CParty g_Party;




/****************************************************
  WndId : APP_CHANGETROUP - 극단 변경
  CtrlId : WIDC_STATIC1 - 순회극단으로 변경하시겠습니까?
  CtrlId : WIDC_STATIC2 - 극단명 : 
  CtrlId : WIDC_EDIT1 - 
  CtrlId : WIDC_OK - Button
  CtrlId : WIDC_CANCEL - Button
****************************************************/

CWndPartyChangeName::CWndPartyChangeName() 
{ 
} 
CWndPartyChangeName::~CWndPartyChangeName() 
{ 
} 
void CWndPartyChangeName::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndPartyChangeName::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	if( 0 == strlen( g_pPlayer->m_szPartyName ) )
	{
		strcpy( m_sParty, prj.GetText(TID_GAME_PARTY_NAME) );
	}
	else
	{
		strcpy( m_sParty, g_pPlayer->m_szPartyName );
	}

	//sprintf( m_sParty, "순회극단" );
	
	CWndEdit *pEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	pEdit->SetString( m_sParty );
	
	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndPartyChangeName::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_PARTYCHANGENAME, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndPartyChangeName::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndPartyChangeName::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndPartyChangeName::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndPartyChangeName::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndPartyChangeName::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK )
	{
		CWndEdit *pEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
		CString PartyName = pEdit->GetString();

		PartyName.TrimLeft();
		PartyName.TrimRight();
		LPCTSTR lpszString = PartyName;		
		if( PartyName.IsEmpty() == FALSE )
		{
			CHAR c = PartyName[ 0 ];
#ifdef __RULE_0615
			if( PartyName.GetLength() < 4 || PartyName.GetLength() > 16 )
#else	// __RULE_0615
			if( PartyName.GetLength() < 3 || PartyName.GetLength() > 16 )
#endif	// __RULE_0615
			{
#ifdef __RULE_0615
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_RULE_0) ) );
#else	// __RULE_0615
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0011) ) );
#endif	// __RULE_0615
//				g_WndMng.OpenMessageBox( _T( "명칭에 3글자 이상, 16글자 이하로 입력 입력하십시오." ) );
				return TRUE;
			}
			else
			if( IsDBCSLeadByte( c ) == FALSE && isdigit2( c ) ) 
			{
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0012) ) );
//				g_WndMng.OpenMessageBox( _T( "명칭에 첫글자를 숫자로 사용할 수 없습니다." ) );
				return TRUE;
			}
			else
			{

				for( int i = 0; i < PartyName.GetLength(); i++ )
				{
					c = PartyName[ i ];
					// 숫자나 알파벳이 아닐 경우는 의심하자.
					if( IsDBCSLeadByte( c ) == TRUE ) 
					{
						CHAR c2 = PartyName[ ++i ];
						WORD word = ( ( c << 8 ) & 0xff00 ) | ( c2 & 0x00ff );
						if( ::GetLanguage() == LANG_KOR )
						{
							if( IsHangul( word ) == FALSE ) 
							{
								g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0014) ) );
//								g_WndMng.OpenMessageBox( _T( "명칭에 한자나 특수 문자를 사용할 수 없습니다." ) );
								return TRUE;
							}
						}
					}
					else
					if( ::GetLanguage() != LANG_THA )
					{
						if( !IsCyrillic( c ) && ( isalnum( c ) == FALSE || iscntrl( c ) )  )
						{
							// 특수 문자도 아니다 (즉 콘트롤 또는 !@#$%^&**()... 문자임)
							g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0013) ) );
//							g_WndMng.OpenMessageBox( _T( "명칭에 콘트롤이나 스페이스, 특수 문자를 사용할 수 없습니다." ) );
							return TRUE;
						}
					}
				}
			}

			if( prj.IsInvalidName( PartyName ) 
#ifdef __RULE_0615
				|| prj.IsAllowedLetter( PartyName ) == FALSE
#endif	// __RULE_0615
				)
			{
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0020) ) );
				return TRUE;
			}

			strcpy( m_sParty, PartyName );
			if( 0 == strcmp( m_sParty, prj.GetText(TID_GAME_PARTY_NAME ) ) )
			{
			}
			else
			{
				g_DPlay.SendChangeTroup( TRUE, m_sParty );
				Destroy();			
			}
			
		}
		else
		{
			// "명칭에 3글자 이상, 16글자 이하로 입력 입력하십시오."
#ifdef __RULE_0615
			g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_RULE_0) ) );
#else	// __RULE_0615
			g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0011) ) );
#endif	// __RULE_0615
			return TRUE;
		}
	}
	else if( nID == WIDC_CANCEL )
	{
		Destroy();
//		g_WndMng.OpenMessageBox( _T( "순회극단 명칭을 정해야 합니다." ) );
	}
/*	else if( nID == WTBID_CLOSE )
	{
		g_WndMng.OpenMessageBox( _T( "순회극단 명칭을 정해야 합니다." ) );
//		SAFE_DELETE( g_WndMng.m_pWndPartyChangeName );
//		g_WndMng.m_pWndPartyChangeName = new CWndPartyChangeName;
//		g_WndMng.m_pWndPartyChangeName->Initialize( );	
		return FALSE;
	}
*/
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

//--------------------------------------------------------------------------------------------------------------------

CWndPartyChangeTroup::CWndPartyChangeTroup() 
{ 
	m_WndPartyChangeName = NULL;
} 
CWndPartyChangeTroup::~CWndPartyChangeTroup() 
{ 
#ifdef __FIX_WND_1109
	SAFE_DELETE( m_WndPartyChangeName );
#endif	// __FIX_WND_1109
} 

void CWndPartyChangeTroup::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndPartyChangeTroup::OnInitialUpdate() 
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
BOOL CWndPartyChangeTroup::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_CHANGETROUP, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndPartyChangeTroup::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndPartyChangeTroup::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndPartyChangeTroup::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndPartyChangeTroup::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndPartyChangeTroup::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK )
	{
		SAFE_DELETE( m_WndPartyChangeName );
		m_WndPartyChangeName = new CWndPartyChangeName;
#ifdef __FIX_WND_1109
		m_WndPartyChangeName->Initialize( );
#else	// __FIX_WND_1109
		m_WndPartyChangeName->Initialize( );
#endif	// __FIX_WND_1109
		Destroy();		
	}
	else if( nID == WIDC_CANCEL )
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

//------------------------------------------------------------------------------------------------
// 극단 탈퇴시 다시 확인 창
//------------------------------------------------------------------------------------------------
/****************************************************
WndId : APP_PARTYLEAVE_CONFIRM - 파티탈퇴확인
CtrlId : WIDC_EDIT1 - 
CtrlId : WIDC_YES - Yes
CtrlId : WIDC_NO - Button
****************************************************/

CWndPartyLeaveConfirm::CWndPartyLeaveConfirm() 
{ 
} 
CWndPartyLeaveConfirm::~CWndPartyLeaveConfirm() 
{ 
} 
void CWndPartyLeaveConfirm::SetLeaveId( u_long uidPlayer )
{
	uLeaveId = uidPlayer;

	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	if( g_pPlayer->m_idPlayer == uLeaveId )
	{
		// 내 자신이 탈퇴
		pWndEdit->SetString( prj.GetText( TID_DIAG_0084 ) );
	}
	else
	{
		// 단장이 선택하여 탈퇴
		pWndEdit->SetString( prj.GetText( TID_DIAG_0085 ) );
	}
	pWndEdit->EnableWindow( FALSE );
}
void CWndPartyLeaveConfirm::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndPartyLeaveConfirm::OnInitialUpdate() 
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
BOOL CWndPartyLeaveConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_PARTYLEAVE_CONFIRM, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
직접 윈도를 열때 사용 
BOOL CWndPartyLeaveConfirm::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
CRect rectWindow = m_pWndRoot->GetWindowRect(); 
CRect rect( 50 ,50, 300, 300 ); 
SetTitle( _T( "title" ) ); 
return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndPartyLeaveConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndPartyLeaveConfirm::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndPartyLeaveConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndPartyLeaveConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndPartyLeaveConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_NO || nID == WTBID_CLOSE ) // 취소 
	{
		Destroy( TRUE );
	}
	else if ( nID == WIDC_YES )
	{
		g_DPlay.SendRemovePartyMember( g_pPlayer->m_idPlayer, uLeaveId );
		Destroy( TRUE );
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
