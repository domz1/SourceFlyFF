// WndMessageBox.cpp: implementation of the CWndNeuz class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"

#include "dploginclient.h"
extern	CDPLoginClient	g_dpLoginClient;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 메시지 박스 기본 클레스 
//
//
// IDABORT    The Abort CHILD was selected.
// IDCANCEL   The Cancel CHILD was selected.
// IDIGNORE   The Ignore CHILD was selected.
// IDNO       The No CHILD was selected.
// IDOK       The OK CHILD was selected.
// IDRETRY    The Retry CHILD was selected.
// IDYES      The Yes CHILD was selected. 
//
// MB_ABORTRETRYIGNORE   The message box contains three pushCHILDs: Abort, Retry, and Ignore.
// MB_OK   The message box contains one pushCHILD: OK.
// MB_OKCANCEL   The message box contains two pushCHILDs: OK and Cancel.
// MB_RETRYCANCEL   The message box contains two pushCHILDs: Retry and Cancel.
// MB_YESNO   The message box contains two pushCHILDs: Yes and No.
// MB_YESNOCANCEL   The message box contains three pushCHILDs: Yes, No, and Cancel. 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndMessageBox::CWndMessageBox()
{
}
CWndMessageBox::~CWndMessageBox()
{
}
void CWndMessageBox::OnDraw( C2DRender* p2DRender )
{
	/*
	p2DRender->SetTextColor( D3DCOLOR_TEMP( 255, 30>>3, 30>>3, 30>>3 ));
	if(m_strArray.GetSize() == 0)
	{
		CPoint pt = GetStrCenter(p2DRender,m_strMessage);
		p2DRender->TextOut(pt.x,35,m_strMessage,D3DCOLOR_TEMP( 255, 100, 100, 250));
	}
	else
	{
		CRect rect = GetClientRect();
		rect.top += 20; rect.left += 40; rect.right -= 70; rect.bottom -= 50;
		int y = (rect.top + rect.Height() / 2)  - ((m_strArray.GetSize() * 18) / 2);
		CString string;
		for(int i = 0; i < m_strArray.GetSize(); i++)
		{
			string = m_strArray.GetAt(i);
			p2DRender->TextOut(50,y+i*18,string);
		}
	}
	*/
}
void CWndMessageBox::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CRect rect = GetClientRect();
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize(60,25);//m_pSprPack->GetAt(9)->GetSize();

	CRect rect1_1( x - ( size.cx / 2), y, ( x - ( size.cx / 2 ) ) + size.cx, y + size.cy );

	CRect rect2_1( x - size.cx - 10, y, ( x - size.cx - 10 ) + size.cx, y + size.cy );
	CRect rect2_2( x + 10          , y, ( x + 10           ) + size.cx, y + size.cy );

	CRect rect3_1( x - ( size.cx / 2) - size.cx - 10, y, (x - ( size.cx / 2) - size.cx - 10) + size.cx, y + size.cy );
	CRect rect3_2( x - ( size.cx / 2)               , y, (x - ( size.cx / 2)               ) + size.cx, y + size.cy );
	CRect rect3_3( x + ( size.cx / 2) + 10          , y, (x + ( size.cx / 2) + 10          ) + size.cx, y + size.cy );

	rect.DeflateRect( 10, 10, 10, 35 );
	m_wndText.AddWndStyle( WBS_VSCROLL );
	//m_wndText.AddWndStyle( WBS_NOFRAME );
	m_wndText.Create( WBS_NODRAWFRAME, rect, this, 0 );

	switch(m_nType)
	{
		case MB_ABORTRETRYIGNORE:
			m_wndButton1.Create( "Abort"  , 0, rect3_1, this, IDABORT );
			m_wndButton2.Create( "Retry", 0, rect3_1, this, IDRETRY );
			m_wndButton3.Create( "Ignore"  , 0, rect3_1, this, IDIGNORE);
			break;
		case MB_OK:
			m_wndButton1.Create("Ok", 0, rect1_1, this, IDOK);
			m_wndButton1.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtNormal00.tga" ) );
			m_wndButton1.FitTextureSize();
			break;
		case MB_CANCEL:
			m_wndButton1.Create("Cancel", 0, rect1_1, this, IDCANCEL);
			m_wndButton1.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtNormal00.tga" ) );
			m_wndButton1.FitTextureSize();
			break;
		case MB_OKCANCEL:
			m_wndButton1.Create("OK"    , 0, rect2_1, this, IDOK    );
			m_wndButton2.Create("Cancel", 0, rect2_2, this, IDCANCEL);
			m_wndButton1.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtNormal00.tga" ) );
			m_wndButton2.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtNormal00.tga" ) );
			m_wndButton1.FitTextureSize();
			m_wndButton2.FitTextureSize();
			break;
		case MB_RETRYCANCEL:
			m_wndButton1.Create("Retry", 0, rect2_1, this, IDRETRY );
			m_wndButton2.Create("Cancel", 0, rect2_2, this, IDCANCEL);
			break;
		case MB_YESNO:
			m_wndButton1.Create("예"    ,0, rect2_1,this,IDYES);
			m_wndButton2.Create("아니오",0, rect2_2,this,IDNO );
			break;
		case MB_YESNOCANCEL:
			m_wndButton1.Create("예"    ,0, rect3_1,this,IDYES);//   ,m_pSprPack,9);
			m_wndButton2.Create("아니오",0, rect3_2,this,IDNO);//    ,m_pSprPack,9);
			m_wndButton3.Create("CANCEL",0, rect3_3,this,IDCANCEL);//,m_pSprPack,9);
			break;
	}
	m_wndButton1.SetDefault( TRUE );
	MoveParentCenter();
}
void CWndMessageBox::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( nChar == VK_RETURN )
	{
		CWndLogin *pWndLogin = (CWndLogin *)g_WndMng.GetWndBase( APP_LOGIN );
		if( pWndLogin )
		{
			CWndEdit* pPassword = (CWndEdit*) pWndLogin->GetDlgItem( WIDC_PASSWORD );
			if( pPassword )
			{
				pPassword->SetString("");//>m_string = "";
				pPassword->PostMessage( WM_SETFOCUS );//SetFocus();
			}
			Destroy(); 
		}
	}
}

BOOL CWndMessageBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch(nID)
	{
		case IDABORT: 
		case IDCANCEL:   
		case IDIGNORE:   
		case IDNO:       
		case IDOK:       
		case IDRETRY:    
		case IDYES:
			if( _tcscmp( m_wndText.m_string, _T( prj.GetText(TID_DIAG_0035) ) ) == 0 )
				::PostMessage( g_Neuz.GetSafeHwnd(), WM_CLOSE, TRUE, 0 );
			Destroy(); 
			break;
	}
	if( GetParentWnd() != m_pWndRoot )
		GetParentWnd()->OnCommand( nID, message, this );
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
/*
BOOL CWndMessageBox::OnCommand( UINT nID, DWORD dwMessage )
{
	switch(nID)
	{
		case IDABORT: 
		case IDCANCEL:   
		case IDIGNORE:   
		case IDNO:       
		case IDOK:       
		case IDRETRY:    
		case IDYES:   
			Destroy(); 
			break;
	}
	return CWndNeuz::OnCommand( nID, dwMessage );
}
*/
BOOL CWndMessageBox::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	return TRUE;
}
BOOL CWndMessageBox::Initialize( LPCTSTR lpszMessage, CWndBase* pWndParent, DWORD nType )
{
	if( g_WndMng.m_pWndWorld && g_WndMng.m_pWndWorld->GetMouseMode() == 1 )	// FPS모드일때
	{
		g_WndMng.m_pWndWorld->SetMouseMode( 0 );
	}
	return Create( lpszMessage, nType, CRect( 0, 0, 0, 0 ), APP_MESSAGEBOX, pWndParent );//RUE;
}
BOOL CWndMessageBox::Create( LPCTSTR lpszMessage, UINT nType, const RECT& rect, UINT nID, CWndBase* pWndParent )
{
	m_nType = nType;
	SetTitle( lpszMessage );
	CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), nID, WBS_MODAL, CPoint( -1, -1 ), pWndParent );
	m_wndText.SetString( lpszMessage, 0xffffffff );//ADEILSON 0xff000000 
	m_wndText.ResetString();
	return 1;
}

void CWndMessageBox::OnEnter( UINT nChar )
{
	if( IsDisable() )
		Destroy(); 
}

BOOL CWndMessageBox::IsDisable( )
{
	CWndButton * pWndButton = (CWndButton*)GetDlgItem(IDOK);
	if( pWndButton && pWndButton->IsWindowEnabled( ) )
		return TRUE;

	return FALSE;
}


CWndMessageBoxUpper::CWndMessageBoxUpper()
{
	m_bPostLogoutMsg = FALSE;
}
CWndMessageBoxUpper::~CWndMessageBoxUpper()
{
}
void CWndMessageBoxUpper::OnDraw( C2DRender* p2DRender )
{
	/*
	p2DRender->SetTextColor( D3DCOLOR_TEMP( 255, 30>>3, 30>>3, 30>>3 ));
	if(m_strArray.GetSize() == 0)
	{
		CPoint pt = GetStrCenter(p2DRender,m_strMessage);
		p2DRender->TextOut(pt.x,35,m_strMessage,D3DCOLOR_TEMP( 255, 100, 100, 250));
	}
	else
	{
		CRect rect = GetClientRect();
		rect.top += 20; rect.left += 40; rect.right -= 70; rect.bottom -= 50;
		int y = (rect.top + rect.Height() / 2)  - ((m_strArray.GetSize() * 18) / 2);
		CString string;
		for(int i = 0; i < m_strArray.GetSize(); i++)
		{
			string = m_strArray.GetAt(i);
			p2DRender->TextOut(50,y+i*18,string);
		}
	}
	*/
}
void CWndMessageBoxUpper::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CRect rect = GetClientRect();
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize(60,25);//m_pSprPack->GetAt(9)->GetSize();

	CRect rect1_1( x - ( size.cx / 2), y, ( x - ( size.cx / 2 ) ) + size.cx, y + size.cy );

	CRect rect2_1( x - size.cx - 10, y, ( x - size.cx - 10 ) + size.cx, y + size.cy );
	CRect rect2_2( x + 10          , y, ( x + 10           ) + size.cx, y + size.cy );

	CRect rect3_1( x - ( size.cx / 2) - size.cx - 10, y, (x - ( size.cx / 2) - size.cx - 10) + size.cx, y + size.cy );
	CRect rect3_2( x - ( size.cx / 2)               , y, (x - ( size.cx / 2)               ) + size.cx, y + size.cy );
	CRect rect3_3( x + ( size.cx / 2) + 10          , y, (x + ( size.cx / 2) + 10          ) + size.cx, y + size.cy );

	rect.DeflateRect( 10, 10, 10, 35 );
	m_wndText.AddWndStyle( WBS_VSCROLL );
	//m_wndText.AddWndStyle( WBS_NOFRAME );
	m_wndText.Create( WBS_NODRAWFRAME, rect, this, 0 );

	switch(m_nType)
	{
		case MB_ABORTRETRYIGNORE:
			m_wndButton1.Create( "중단"  , 0, rect3_1, this, IDABORT );
			m_wndButton2.Create( "재시도", 0, rect3_1, this, IDRETRY );
			m_wndButton3.Create( "무시"  , 0, rect3_1, this, IDIGNORE);
			break;
		case MB_OK:
			m_wndButton1.Create("Ok", 0, rect1_1, this, IDOK);
			m_wndButton1.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtNormal00.tga" ) );
			m_wndButton1.FitTextureSize();
			break;
		case MB_CANCEL:
			m_wndButton1.Create("Cancel", 0, rect1_1, this, IDCANCEL);
			m_wndButton1.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtNormal00.tga" ) );
			m_wndButton1.FitTextureSize();
			break;
		case MB_OKCANCEL:
			m_wndButton1.Create("Ok"    , 0, rect2_1, this, IDOK    );
			m_wndButton2.Create("Cancel", 0, rect2_2, this, IDCANCEL);
			m_wndButton1.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtNormal00.tga" ) );
			m_wndButton2.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtNormal00.tga" ) );
			m_wndButton1.FitTextureSize();
			m_wndButton2.FitTextureSize();
			break;
		case MB_RETRYCANCEL:
			m_wndButton1.Create("재시도", 0, rect2_1, this, IDRETRY );
			m_wndButton2.Create("CANCEL", 0, rect2_2, this, IDCANCEL);
			break;
		case MB_YESNO:
			m_wndButton1.Create("예"    ,0, rect2_1,this,IDYES);
			m_wndButton2.Create("아니오",0, rect2_2,this,IDNO );
			break;
		case MB_YESNOCANCEL:
			m_wndButton1.Create("예"    ,0, rect3_1,this,IDYES);//   ,m_pSprPack,9);
			m_wndButton2.Create("아니오",0, rect3_2,this,IDNO);//    ,m_pSprPack,9);
			m_wndButton3.Create("CANCEL",0, rect3_3,this,IDCANCEL);//,m_pSprPack,9);
			break;
	}
	m_wndButton1.SetDefault( TRUE );

	// 게이지 위쪽으로 나오게 하기
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( ( rectRoot.right - rectWindow.Width() ) / 2, 70 );
	Move( point );
}

BOOL CWndMessageBoxUpper::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch(nID)
	{
		case IDABORT: 
		case IDCANCEL:   
		case IDIGNORE:   
		case IDNO:       
		case IDOK:       
		case IDRETRY:    
		case IDYES:
#ifndef _DEBUG			
			if( m_bPostLogoutMsg )
				::PostMessage( g_Neuz.GetSafeHwnd(), WM_LOGOUT, TRUE, 0 );
#endif
			Destroy(); 
			break;
	}
	
	if( nID == 10000 )
		return TRUE;

	if( GetParentWnd() != m_pWndRoot )
		GetParentWnd()->OnCommand( nID, message, this );

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
/*
BOOL CWndMessageBox::OnCommand( UINT nID, DWORD dwMessage )
{
	switch(nID)
	{
		case IDABORT: 
		case IDCANCEL:   
		case IDIGNORE:   
		case IDNO:       
		case IDOK:       
		case IDRETRY:    
		case IDYES:   
			Destroy(); 
			break;
	}
	return CWndNeuz::OnCommand( nID, dwMessage );
}
*/
BOOL CWndMessageBoxUpper::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	return TRUE;
}
BOOL CWndMessageBoxUpper::Initialize( LPCTSTR lpszMessage, CWndBase* pWndParent, DWORD nType, BOOL bPostLogoutMsg )
{
	m_bPostLogoutMsg = bPostLogoutMsg;
	CRect rect = m_pWndRoot->MakeCenterRect( 250, 130 );
	//Create( _T( "매시지 박스" ), nType, rect, APP_MESSAGEBOX );
	if( g_WndMng.m_pWndWorld && g_WndMng.m_pWndWorld->GetMouseMode() == 1 )	// FPS모드일때
	{
		g_WndMng.m_pWndWorld->SetMouseMode( 0 );
	}
	return Create( lpszMessage, nType, rect, APP_MESSAGEBOX, pWndParent );//RUE;
}
BOOL CWndMessageBoxUpper::Create( LPCTSTR lpszMessage, UINT nType, const RECT& rect, UINT nID, CWndBase* pWndParent )
{
	m_nType = nType;
	SetTitle( lpszMessage );
	CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), nID, WBS_MODAL, CPoint( -1, -1 ), pWndParent );
	m_wndText.SetString( lpszMessage, 0xffffffff );//ADEILSON 0xff000000
	m_wndText.ResetString();
	return 1;
}
