#include "stdafx.h"
#include "AppDefine.h"
#include "WndIndirectTalk.h"

/****************************************************
  WndId : APP_ADMIN_INDIRECT_TALK - 간접 대화
  CtrlId : WIDC_EDIT1 - Edit
  CtrlId : WIDC_STATIC1 - Mover ID
  CtrlId : WIDC_EDIT2 - Edit
  CtrlId : WIDC_BUTTON1 - Send
****************************************************/

CWndIndirectTalk::CWndIndirectTalk() 
{ 
} 
CWndIndirectTalk::~CWndIndirectTalk() 
{ 
} 
void CWndIndirectTalk::OnDraw( C2DRender* p2DRender ) 
{ 
	CWorld* pWorld = g_WorldMng();
	CMover* pMover = (CMover*)pWorld->GetObjFocus();
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	TCHAR szNum[ 32 ];
	if( pMover && pMover->GetType() == OT_MOVER )
		itoa( pMover->GetId(), szNum, 10 );
	else
		itoa( 0, szNum, 10 );
	pWndEdit->SetString( szNum );
} 
void CWndIndirectTalk::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT2);
	pWndEdit->AddWndStyle( EBS_AUTOVSCROLL );

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndIndirectTalk::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_ADMIN_INDIRECT_TALK, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndIndirectTalk::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndIndirectTalk::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndIndirectTalk::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndIndirectTalk::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndIndirectTalk::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndIndirectTalk::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	CWorld* pWorld = g_WorldMng();
	CObj* pObj = pWorld->GetObjFocus();
	if( pObj && pObj->GetType() == OT_MOVER )
	{
		switch( nID )
		{
		case WIDC_EDIT2: // 본문 
			if( message != EN_RETURN )
				break;
		case WIDC_BUTTON1:
			{
				CWndEdit* pWndEdit1 = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
				CWndEdit* pWndEdit2 = (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
				LPCTSTR lpId = pWndEdit1->m_string;
				LPCTSTR lpText = pWndEdit2->m_string;
				CString string;
				string.Format( "/id %s %s", lpId, lpText );
				ParsingCommand( string.LockBuffer(), g_pPlayer );
				string.UnlockBuffer();
				pWndEdit2->Empty();
			}
			break;
		}
	}
	if( nID == WTBID_CLOSE )
	{
		Destroy( TRUE );
		return TRUE;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

