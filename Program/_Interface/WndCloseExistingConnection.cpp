#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndCloseExistingConnection.h"
#include "WndManager.h"

#include "dpcertified.h"
extern	CDPCertified	g_dpCertified;


/****************************************************
  WndId : APP_CLOSE_EXISTING_CONNECTION - !
  CtrlId : WIDC_STATIC1 - Static
  CtrlId : WIDC_YES - Button
  CtrlId : WIDC_NO - Button
****************************************************/
CWndCloseExistingConnection::CWndCloseExistingConnection() 
{

}
 
CWndCloseExistingConnection::~CWndCloseExistingConnection() 
{

}
 
void CWndCloseExistingConnection::OnDraw( C2DRender* p2DRender ) 
{
}
 
void CWndCloseExistingConnection::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CRect rect	= GetClientRect();
	rect.left	+= 8;
	rect.right	-= 4;
	rect.top	+= 8;
	rect.bottom	/= 2;

	m_wndText.Create( WBS_NODRAWFRAME, rect, this, 0 );
	m_wndText.SetString( _T( prj.GetText(TID_DIAG_0065) ), 0xffffffff );//ADEILSON 0xff000000 //prj.GetTextColor(TID_DIAG_0065) );
	//m_wndText.SetString( _T( "접속중인 계정입니다. 기존 접속을 해제하시겠습니까?" ), 0xff000000 );
	m_wndText.ResetString();

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 

BOOL CWndCloseExistingConnection::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_CLOSE_EXISTING_CONNECTION, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndCloseExistingConnection::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndCloseExistingConnection::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndCloseExistingConnection::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndCloseExistingConnection::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndCloseExistingConnection::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES )
	{
		g_dpCertified.SendCloseExistingConnection( g_Neuz.m_szAccount, g_Neuz.m_szPassword );
		Sleep( 1000 );
		CWndLogin* pWndLogin	= (CWndLogin*)g_WndMng.GetWndBase( APP_LOGIN );
		if( pWndLogin )
			pWndLogin->GetDlgItem( WIDC_OK )->EnableWindow( TRUE );
		Destroy( TRUE );
	}
	else if( nID == WIDC_NO )
	{
//		g_dpCertified.Destroy( TRUE );
		CWndLogin* pWndLogin	= (CWndLogin*)g_WndMng.GetWndBase( APP_LOGIN );
		if( pWndLogin )
			pWndLogin->GetDlgItem( WIDC_OK )->EnableWindow( TRUE );
		Destroy( TRUE );
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

