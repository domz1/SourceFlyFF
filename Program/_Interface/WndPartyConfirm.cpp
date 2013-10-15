#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndPartyConfirm.h"

#include "DPClient.h"
extern	CDPClient	g_DPlay;


/****************************************************
  WndId : APP_PARTY_CONFIRM - party confirm
  CtrlId : WIDC_YES - yes
  CtrlId : WIDC_NO - No
  CtrlId : WIDC_STATIC1 - Static
****************************************************/

CWndPartyConfirm::CWndPartyConfirm() 
{ 
	m_uLeader = m_uMember = 0;
	m_szLeaderName[ 0 ] = '\0';
	m_szMemberName[ 0 ] = '\0';
	bTroup = FALSE;
} 
CWndPartyConfirm::~CWndPartyConfirm() 
{ 
} 

void CWndPartyConfirm::SetMember( u_long uLeader, LONG nLLevel, LONG nLJob, DWORD dwLSex, u_long uMember, LONG nMLevel, LONG nMJob, DWORD dwMSex, char * szLName, BOOL bTroupbuf )
{
	m_uLeader = uLeader;
	m_nLeaderLevel = nLLevel;
	m_nLeaderJob = nLJob;
	m_dwLeaderSex = dwLSex;
	m_uMember = uMember;
	m_nMemberLevel = nMLevel;
	m_nMemberJob = nMJob;
	m_dwMemberSex = dwMSex;
	strcpy( m_szLeaderName, szLName );
	bTroup = bTroupbuf;
}

void CWndPartyConfirm::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndPartyConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
//	CWndStatic* pWndStatic;
//	pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
	CString strTemp;
	strTemp.Format(_T(prj.GetText(TID_DIAG_0072)),m_szLeaderName); // 메시지 바꾸려면 이걸 바꾸시오
//	strTemp.Format(_T("%s 님이 극단에 초청하고 있습니다. 참여하시겠습니까?"),m_szLeaderName); // 메시지 바꾸려면 이걸 바꾸시오
	pWndText->SetString( strTemp );
//	pWndStatic->SetTitle(strTemp); 
	
	// 윈도를 중앙으로 옮기는 부분.
	/*
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
	*/
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( ( rectRoot.right - rectWindow.Width() ) / 2, 70 );
	Move( point );	
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndPartyConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_PARTY_CONFIRM, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndPartyConfirm::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndPartyConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndPartyConfirm::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndPartyConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndPartyConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndPartyConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES ) 
	{
		// 여기다가 승락하는 처리 추가하시오
		if( bTroup == FALSE ) // 단막극단
		{
			g_DPlay.SendAddPartyMember( m_uLeader, m_nLeaderLevel, m_nLeaderJob, m_dwLeaderSex, m_uMember, m_nMemberLevel, m_nMemberJob, m_dwMemberSex );		
		}
		Destroy();	// 수동파괴로 바꿈 -XUZHU- 09/16
	}
	else if(nID==WIDC_NO || nID==WTBID_CLOSE)
	{
		// 여기다가 거부하는 처리 추가하시오
		g_DPlay.SendPartyMemberCancle( m_uLeader, m_uMember );
		Destroy();	// 수동파괴로 바꿈 -XUZHU- 09/16
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

