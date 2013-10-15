#include "stdafx.h"
#include "resData.h"
#include "WndChangeName.h"
#include "dpclient.h"
#include "definetext.h"
extern		CDPClient		g_DPlay;


/*
  WndId : APP_CHANGENAME
  CtrlId : WIDC_STATIC1
  CtrlId : WIDC_OK 
  CtrlId : WIDC_CANCEL 
  CtrlId : WIDC_EDIT_CHANGENAME 
*/

CWndChangeName::CWndChangeName() 
{ 
	
} 

CWndChangeName::~CWndChangeName() 
{ 

} 

void CWndChangeName::OnDraw( C2DRender* p2DRender ) 
{ 
#ifdef __S_SERVER_UNIFY
	if( g_WndMng.m_bAllAction == FALSE )
	{
		CRect rectRoot = m_pWndRoot->GetLayoutRect();
		CRect rectWindow = GetWindowRect();
		CPoint point( rectRoot.right - rectWindow.Width(), 110 );
		Move( point );
		MoveParentCenter();		
	}
#endif // __S_SERVER_UNIFY
} 

void CWndChangeName::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	m_dwData	= 0;

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 

BOOL CWndChangeName::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_CHANGENAME, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndChangeName::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndChangeName::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndChangeName::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 

} 

void CWndChangeName::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 

} 

void CWndChangeName::SetData( WORD wId, WORD wReset )
{
	m_dwData	= MAKELONG( wId, wReset );
}

extern DWORD IsValidPlayerName( CString& strName );

BOOL CWndChangeName::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK )
	{
		CWndEdit* pEdit	= (CWndEdit*)GetDlgItem( WIDC_EDIT_CHANGENAME );
		CString string	= pEdit->GetString();

		DWORD dwError = ::IsValidPlayerName( string );
		if( dwError > 0 )
		{
			g_WndMng.OpenMessageBox( prj.GetText(dwError) );
			pEdit->SetFocus();
			return TRUE;
		}

		if( prj.IsInvalidName( string )
#ifdef __RULE_0615
			|| prj.IsAllowedLetter( string ) == FALSE
#endif	// __RULE_0615
			)
		{
			g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0020) ) );
			return TRUE;
		}
		g_DPlay.SendQuerySetPlayerName( m_dwData, (LPSTR)(LPCSTR)string );
		Destroy();
	}
	else if( nID == WIDC_CANCEL || nID == WTBID_CLOSE )
	{
#ifdef __S_SERVER_UNIFY
		if( g_WndMng.m_bAllAction == FALSE )
			return TRUE;
#endif // __S_SERVER_UNIFY
		Destroy();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

#ifdef __PET_1024

CWndChangePetName::CWndChangePetName() 
{ 
	m_dwId = 0;
} 

CWndChangePetName::~CWndChangePetName() 
{ 

} 

void CWndChangePetName::OnDraw( C2DRender* p2DRender ) 
{ 
#ifdef __S_SERVER_UNIFY
	if( g_WndMng.m_bAllAction == FALSE )
	{
		CRect rectRoot = m_pWndRoot->GetLayoutRect();
		CRect rectWindow = GetWindowRect();
		CPoint point( rectRoot.right - rectWindow.Width(), 110 );
		Move( point );
		MoveParentCenter();		
	}
#endif // __S_SERVER_UNIFY
} 

void CWndChangePetName::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	m_dwData	= 0;

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 

BOOL CWndChangePetName::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_CHANGENAME, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndChangePetName::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndChangePetName::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndChangePetName::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 

} 

void CWndChangePetName::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 

} 

void CWndChangePetName::SetData( WORD wId, WORD wReset )
{
	m_dwData	= MAKELONG( wId, wReset );
}

//extern DWORD IsValidPlayerName( CString& strName );

BOOL CWndChangePetName::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK )
	{
		CWndEdit* pEdit	= (CWndEdit*)GetDlgItem( WIDC_EDIT_CHANGENAME );
		CString string	= pEdit->GetString();

		DWORD dwError = ::IsValidPlayerName( string );
		if( dwError > 0 )
		{
			g_WndMng.OpenMessageBox( prj.GetText(dwError) );
			pEdit->SetFocus();
			return TRUE;
		}

		if( prj.IsInvalidName( string )
#ifdef __RULE_0615
			|| prj.IsAllowedLetter( string ) == FALSE
#endif	// __RULE_0615
			)
		{
			g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0020) ) );
			return TRUE;
		}
		// 펫 이름을 바꾸도록 요청하는 함수를 호출한다
		 g_DPlay.SendDoUseItemInput(m_dwId, (LPSTR)(LPCSTR)string);
		Destroy();
	}
	else if( nID == WIDC_CANCEL || nID == WTBID_CLOSE )
	{
#ifdef __S_SERVER_UNIFY
		if( g_WndMng.m_bAllAction == FALSE )
			return TRUE;
#endif // __S_SERVER_UNIFY
		Destroy();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
#endif
