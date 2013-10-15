#include "StdAfx.h"
#ifdef __NEW_WEB_BOX
#ifdef __CLIENT
#include "HelperWebBox.h"
#include "WebCtrl.h"

//-----------------------------------------------------------------------------
BOOL CALLBACK HelperWebDialogProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );
//-----------------------------------------------------------------------------
CHelperWebBox::CHelperWebBox( void ) : 
m_strHelperWebAddress( _T( "" ) ), 
m_hInstance( NULL ), 
m_hParent( NULL ), 
m_hDialog( NULL ), 
m_hDialogBox( NULL ), 
m_rectHelperWeb( 0, 0, 0, 0 )
{
}
//-----------------------------------------------------------------------------
CHelperWebBox::~CHelperWebBox( void )
{
	Close();
}
//-----------------------------------------------------------------------------
CHelperWebBox* CHelperWebBox::GetInstance( void )
{
	static CHelperWebBox sHelperWebBox;
	return &sHelperWebBox;
}
//-----------------------------------------------------------------------------
void CHelperWebBox::Initialize( void )
{
	InitializeWebInformation();

	D3DDEVICE->SetDialogBoxMode( TRUE );

	m_hInstance = g_Neuz.m_hInstance;
	m_hParent = g_Neuz.GetSafeHwnd();

	if( g_Option.m_nResWidth >= 1024 && g_Option.m_nResHeight >= 768 )
	{
		m_rectHelperWeb.SetRect( -1, 0, HELPER_WEB_SIZE_LARGE_X + 5, HELPER_WEB_SIZE_LARGE_Y );
	}
	else
	{
		m_rectHelperWeb.SetRect( -1, 0, HELPER_WEB_SIZE_NORMAL_X + 5, HELPER_WEB_SIZE_NORMAL_Y );
	}

	Close();

	ShowWindow( m_hDialogBox, SW_HIDE );
	SetFocus( m_hParent );

	HINSTANCE hInstance = GetModuleHandle( NULL );
	m_hDialog = CreateDialog( hInstance, MAKEINTRESOURCE( IDD_WEBDLG ), m_hParent, HelperWebDialogProc );
	m_hDialogBox = CreateWindow( 
		WC_WEBCTRL, 
		m_strHelperWebAddress, 
		WS_EX_TOPMOST | WS_CHILDWINDOW, 
		m_rectHelperWeb.left, 
		m_rectHelperWeb.top, 
		m_rectHelperWeb.right, 
		m_rectHelperWeb.bottom, 
		m_hDialog, 
		NULL, 
		m_hInstance, 
		NULL );

	GoFirstPage();

	ShowWindow( m_hDialogBox, SW_SHOW );
	SetFocus( m_hDialogBox );
}
//-----------------------------------------------------------------------------
void CHelperWebBox::Process( void )
{
	if( m_hDialogBox )
	{
		static BOOL bBackKeySwitch = FALSE;
		if( GetAsyncKeyState( VK_BACK ) & 0x8000 )
		{
			if( bBackKeySwitch == FALSE )
			{
				SendMessage( m_hDialogBox, WEB_CTRL_GO_BACK, 0, 0 );
				bBackKeySwitch = TRUE;
			}
		}
		else
		{
			if( bBackKeySwitch == TRUE )
			{
				bBackKeySwitch = FALSE;
			}
		}

		static BOOL bF5KeySwitch = FALSE;
		if( GetAsyncKeyState( VK_F5 ) & 0x8000 )
		{
			if( bF5KeySwitch == FALSE )
			{
				SendMessage( m_hDialogBox, WEB_CTRL_REFRESH, 0, 0 );
				bF5KeySwitch = TRUE;
			}
		}
		else
		{
			if( bF5KeySwitch == TRUE )
			{
				bF5KeySwitch = FALSE;
			}
		}
	}
}
//-----------------------------------------------------------------------------
void CHelperWebBox::Release( void )
{
	ShowWindow( m_hDialogBox, SW_HIDE );
	SetFocus( m_hParent );
	Close();
	UninitWebCtrl();
}
//-----------------------------------------------------------------------------
void CHelperWebBox::InitializeWebInformation( void )
{
	switch( ::GetLanguage() )
	{
#ifdef __INTERNALSERVER
	case LANG_KOR:
		{
			m_strHelperWebAddress = _T( "http://flyff-wiki.gpotato.com/index.php/Main_Page" );
			break;
		}
#endif // __INTERNALSERVER
	case LANG_USA:
		{
			switch( ::GetSubLanguage() )
			{
			case LANG_SUB_USA:
				{
					m_strHelperWebAddress = _T( "http://flyff-wiki.gpotato.com/index.php/Main_Page" );
					break;
				}
			}

			break;
		}
	}
}
//-----------------------------------------------------------------------------
void CHelperWebBox::Close( void )
{
	if( m_hDialogBox )
	{	
		DestroyWindow( m_hDialogBox );
		m_hDialogBox = NULL;
	}

	if( m_hDialog )
	{
		DestroyWindow( m_hDialog );
		m_hDialog = NULL;
	}
}
//-----------------------------------------------------------------------------
void CHelperWebBox::GoFirstPage( void )
{
	if( m_strHelperWebAddress == _T( "" ) )
	{
		return;
	}

	TCHAR szHelperWebAddress[ 512 ] = { 0, };
	TCHAR header[WEB_STR_LEN] = { 0, };
	strcpy( szHelperWebAddress, m_strHelperWebAddress );
	SendMessage( m_hDialogBox, WEB_CTRL_ADDRESS, 0, ( LPARAM )szHelperWebAddress );
	SendMessage( m_hDialogBox, WEB_CTRL_NAVIGATE, 0, 0 );
}
//-----------------------------------------------------------------------------
const CString& CHelperWebBox::GetHelperWebAddress( void ) const
{
	return m_strHelperWebAddress;
}
//-----------------------------------------------------------------------------
const HWND& CHelperWebBox::GetParentHandle( void ) const
{
	return m_hParent;
}
//-----------------------------------------------------------------------------
const HWND& CHelperWebBox::GetDialogHandle( void ) const
{
	return m_hDialog;
}
//-----------------------------------------------------------------------------
const HWND& CHelperWebBox::GetDialogBoxHandle( void ) const
{
	return m_hDialogBox;
}
//-----------------------------------------------------------------------------
BOOL CALLBACK HelperWebDialogProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	CHelperWebBox* pHelperWebBox = CHelperWebBox::GetInstance();

	int nHelperWebSizeX = CHelperWebBox::HELPER_WEB_SIZE_NORMAL_X;
	int nHelperWebSizeY = CHelperWebBox::HELPER_WEB_SIZE_NORMAL_Y;
	if( g_Option.m_nResWidth >= 1024 && g_Option.m_nResHeight >= 768 )
	{
		nHelperWebSizeX = CHelperWebBox::HELPER_WEB_SIZE_LARGE_X;
		nHelperWebSizeY = CHelperWebBox::HELPER_WEB_SIZE_LARGE_Y;
	}

	switch( Msg )
	{
	case WM_INITDIALOG:
		{
			CRect rcCenter( 0, 0, 0, 0 );
			GetWindowRect( pHelperWebBox->GetParentHandle(), &rcCenter );

			int xLeft = ( rcCenter.left + rcCenter.right ) / 2 - nHelperWebSizeX / 2;
			int yTop = ( rcCenter.top + rcCenter.bottom ) / 2 - nHelperWebSizeY / 2;

			DWORD CapH = 0;

			if( g_Neuz.IsWindow() == true )
				CapH = GetSystemMetrics( SM_CYCAPTION );
			else
				CapH = 9;

			if( g_Neuz.m_bFakeFullMode )
			{
				if( g_Option.m_bStartFullScreen )
					CapH = 5;
				else
					CapH = 20;				
			}

			SetWindowPos( hWnd, NULL, xLeft, yTop+CapH, nHelperWebSizeX, nHelperWebSizeY, SWP_NOZORDER | SWP_NOACTIVATE );
			break;
		}
	case WM_COMMAND:
		{
			switch( wParam )
			{
			case IDCANCEL:
				{
					ShowWindow( pHelperWebBox->GetDialogHandle(), SW_HIDE );
					break;
				}
			}
			break;
		}
	}
	return FALSE;
}
//-----------------------------------------------------------------------------

#endif // __CLIENT
#endif // __NEW_WEB_BOX