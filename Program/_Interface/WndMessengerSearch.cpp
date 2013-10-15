#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndMessengerSearch.h"

#include "dpCertified.h"
extern	CDPCertified	g_dpCertified;

/****************************************************
  WndId : APP_MESSENGER_SEARCH - 사용자검색
  CtrlId : WIDC_STATIC_ID - 아이디검색
  CtrlId : WIDC_EDIT_ID - 
  CtrlId : WIDC_STATIC_SERVER - 서버별검색
  CtrlId : WIDC_COMBOBOX_SERVER - 
  CtrlId : WIDC_STATIC_SEX - 성별검색
  CtrlId : WIDC_RADIO_MALE - 남
  CtrlId : WIDC_RADIO_FEMALE - 여
  CtrlId : WIDC_BUTTON_OK - Button
  CtrlId : WIDC_BUTTON_CANCLE - Button
  CtrlId : WIDC_LIST - 
****************************************************/

CWndMessengerSearch::CWndMessengerSearch() 
{ 
} 
CWndMessengerSearch::~CWndMessengerSearch() 
{ 
} 
void CWndMessengerSearch::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndMessengerSearch::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	CWndComboBox* pWndComboBox = (CWndComboBox*)GetDlgItem( WIDC_COMBOBOX_SERVER );
	
	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_LIST );
	WTCITEM tabTabItem;
	
	m_wndFriend.Create( WLVS_ICON, CRect( 0, 0, 250, 250 ), pWndTabCtrl, 11 );
	m_wndFriend.AddWndStyle( WBS_NODRAWFRAME );
//	m_wndFriend.InitItem( g_pPlayer->m_nJob, g_pPlayer->m_nJob, g_pPlayer->m_aJobSkill, APP_MESSENGER_SEARCH );

	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_APP_COMMUNITY_FRIEND);//"친구";
	tabTabItem.pWndBase = &m_wndFriend;
	pWndTabCtrl->InsertItem( 0, &tabTabItem );

	// 서버별 리스트 넣기
	pWndComboBox->AddString( "All Server" );
	for( int i = 0 ; i < (int)( g_dpCertified.m_dwSizeofServerset ) ; ++i )
	{
		if( g_Neuz.m_dwSys == g_dpCertified.m_aServerset[i].dwParent )
		{
			pWndComboBox->AddString( g_dpCertified.m_aServerset[i].lpName );
		}
	}
	pWndComboBox->SetCurSel( 0 );


	// 남 / 여 / 모두
	CWndButton* pWndButton[ 3 ];
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_RADIO_MALE );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_RADIO_FEMALE );
	pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_RADIO_ALL );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ 2 ]->SetCheck( TRUE );
	nSexSearch = 3;
	
	

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndMessengerSearch::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_MESSENGER_SEARCH, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndMessengerSearch::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndMessengerSearch::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndMessengerSearch::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndMessengerSearch::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndMessengerSearch::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndMessengerSearch::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch(nID)
	{
		case WIDC_BUTTON_OK:
			{
				CWndComboBox* pWndComboBox = (CWndComboBox*)GetDlgItem( WIDC_COMBOBOX_SERVER );
				CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT_ID );
				int nSelect = pWndComboBox->GetCurSel();
				CString strId = pWndEdit->GetString();
				if( strId.GetLength() < MAX_PATH )
					m_wndFriend.SetSearch( nSelect, nSexSearch, strId );
			}
			break;
		case WIDC_BUTTON_CANCLE:
			{
				Destroy();
			}
			break;
		case WIDC_RADIO_MALE:
			{
				nSexSearch = 0;
			}
			break;
		case WIDC_RADIO_FEMALE:
			{
				nSexSearch = 1;
			}
			break;
		case WIDC_RADIO_ALL:
			{
				nSexSearch = 3;
			}
			break;
		default:
			break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

