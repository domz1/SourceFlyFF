#include "StdAfx.h"

#ifdef __BS_CONSOLE

static char szBuf[512] ;

INT_PTR CALLBACK DlgConsoleProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch (uMsg )
	{
	case WM_INITDIALOG: 
		{
			::SetWindowPos( hwndDlg, 0, 1200, 10, 0, 0, SW_SHOW );
//			assert( DlgConsole::GetSingletonPtr() ) ;
			//Intialize controls here			
//			DlgConsole::GetSingleton().ClearOutWindow() ;
//			DlgConsole::GetSingleton().ClearInWindow() ;
//			utilx::Console::GetSingleton().SetRenderTarget( (utilx::ConsoleRenderTarget*)DlgConsole::GetSingletonPtr()) ; //Hook Console render target
			
		}
		return TRUE;  

	case WM_COMMAND:
		{
			switch( LOWORD( wParam ) )
			{ 				
			case IDC_EDIT1:
				{
					//switch( HIWORD( wParam ) )
					//{
					//case EN_CHANGE : 
										
					//	GetWindowText( (HWND)lParam, str, 128 );
					//	break;

					if( HIWORD(wParam) == EN_MAXTEXT )
					{
						::GetDlgItemText( hwndDlg, IDC_EDIT1, szBuf, 512 ) ;			
						gConsole( )->Parsing( szBuf );
						//::SetDlgItemText( hwndDlg, IDC_CON_IN, "") ;

						//if( strlen(szBuf) == 0 )
						//{
						//	DlgConsole::GetSingleton().Hide() ;
						//	return TRUE ;
						//}
						// 
						//utilx::Console::GetSingleton().PassLine( szBuf ) ;		
						//utilx::Console::GetSingleton().Render() ;
					}


					
				}
				return TRUE ;
			default:
				break ;
			} //switch( LOWORD( wParam ) )
		}
		break ;
		
	}
	return FALSE ;
}

DlgConsole::DlgConsole( )
{
	_bShow = false;
	_hWnd = 0;
	_hWndParent = 0;

	_fSpeedMul = 1.0f;

	_dwHitter = 0;
	_bRandomMoving = FALSE;
	_bTestToggle = FALSE;
}

DlgConsole::~DlgConsole( )
{
//	con::AssDeleter< ConDataConationer > ( ) ( _cConDatas );

	if( _hWnd )
	{
		DestroyWindow( _hWnd );
	}
}

WNDPROC procEdit;

//sub classing for editbox
LRESULT CALLBACK EditSubProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{ 
	TCHAR str[256];
	switch (iMessage)
	{
	case WM_KEYDOWN:
		if (wParam==VK_RETURN)
		{   
			GetWindowText(hWnd,str,256);//에디터 박스에서 문자열을 가져옴
			if( strlen( str ) == 0 )
				gConsole()->Hide( );

			gConsole()->AddString( str );
			SetWindowText(hWnd,"");//에디터 박스 문자열을 지움
			gConsole()->Parsing( str );
		}

		if( wParam == VK_OEM_3 )
		{
			SetWindowText( hWnd, "" );
			//gConsole()->ToggleShow( );
			if( GetFocus( ) == hWnd )
				SetFocus( gConsole()->GetParentHWnd( ) );
			else gConsole()->Show( );
		}

	 break;

	}

	return CallWindowProc(procEdit,hWnd,iMessage,wParam,lParam);
}


void DlgConsole::Init( HWND hWndParent )
{
	_hWndParent = hWndParent;
	
	HINSTANCE hIns = ( HINSTANCE )::GetWindowLong( hWndParent, GWL_HINSTANCE );
	assert( hIns );

	_hWnd = CreateDialog( hIns, MAKEINTRESOURCE(IDD_CONSOLE_DLG ), hWndParent, DlgConsoleProc );
	assert( _hWnd );

	//sub classing
	HWND hEdit = ::GetDlgItem( _hWnd, IDC_CONSOLE_EDIT );
	procEdit = (WNDPROC)SetWindowLongPtr( hEdit, GWLP_WNDPROC, ( LONG_PTR )EditSubProc );

	AddString( "----Welcome to Console!!----" );
	AddString( "Note:Input command <help>, you can see all command!" );

	ConsoleRegister( );

#ifdef __CLIENT
	_auto_start( );
#endif

	SetFocus( _hWndParent );
}


void DlgConsole::Show()
{
	_bShow = true ;
	::ShowWindow( _hWnd, SW_SHOW) ;
	HWND hEdit = ::GetDlgItem( _hWnd, IDC_CONSOLE_EDIT );
	SetWindowText( hEdit, "" );
	SetFocus( hEdit );
	
//	::SendDlgItemMessage( _hWnd, IDC_CON_OUT, WM_VSCROLL, (WPARAM)SB_BOTTOM, 0 );
}

void DlgConsole::Hide()
{
	_bShow = false ;
	::ShowWindow( _hWnd, SW_HIDE) ;
}

void DlgConsole::ToggleShow( )
{
	_bShow ? Hide( ) : Show( );
}


void DlgConsole::AddString( const char* str )
{
	HWND hList = ::GetDlgItem( _hWnd, IDC_CONSOLE_LIST );
	::SendMessage( hList, LB_ADDSTRING, 0, (LPARAM)str );

	int nIndex;

	nIndex=(int)SendMessage(hList, LB_GETCOUNT, 0, 0);
	SendMessage(hList, LB_SETTOPINDEX, nIndex-1, 0);
}

void DlgConsole::Reset( )
{
	HWND hList = ::GetDlgItem( _hWnd, IDC_CONSOLE_LIST );
	::SendMessage( hList, LB_RESETCONTENT, 0, 0 );
}

int DlgConsole::Log( char* fmt, ... )
{
//#ifdef _DEBUG
	char	buff[1024];
//	char	date[128];
//	char	time[128];

//	_strdate( date );
//	_strtime( time );
	vsprintf( buff, fmt, (char *)(&fmt+1) );
/*
	// Console에 출력할 경우 
	if( m_nTarget & ZF_LOG_TARGET_CONSOLE )
	{
		printf( "(date[%s] time[%s]) : %s\n", date, time, buff );
	}

	// Log File에 출력할 경우
	if( m_nTarget & ZF_LOG_TARGET_FILE )
	{
		FILE*	fp = NULL;
		fp = fopen( m_szFilename, "a+" );
		if( fp )
		{
			fprintf( fp, "(date[%s] time[%s]) : %s\n", date, time, buff );
			fclose( fp );
		}
	} */

	// Log Window에 출력할 경우 
//	if( m_nTarget & ZF_LOG_TARGET_WINDOW )
//	{
		HWND hList = ::GetDlgItem( _hWnd, IDC_CONSOLE_LIST );
		SendMessage( hList, LB_ADDSTRING, 0, (LPARAM) buff );
	//	UINT32 n = SendMessage( m_hwndList, LB_GETCOUNT, 0, 0L ) - 1;
	//	SendMessage( m_hwndList, LB_SETCURSEL, (WPARAM)n, 0L );
//	}

	int nIndex;
	nIndex=(int)SendMessage(hList, LB_GETCOUNT, 0, 0);
	SendMessage(hList, LB_SETTOPINDEX, nIndex-1, 0);
//#endif
	
	return 1;
}


void DlgConsole::Parsing( const char* str )
{
	//TODO : 명령 문자열 파싱해서 해당 함수 불러주기 
	char key[ 64 ] = "";
	char val[ 64 ] = "";
	sscanf( str, "%s%s", key, val );

	CON_DATA* pFindData = FindData( key );
	if( pFindData )
	{
		vector< string > args;
		args.push_back( string( key ) );
		args.push_back( string( val ) );
		pFindData->CallFunction( args );

		AddString( "=>OK..." );
	}else
	{
		AddString( "=>NO function..." );
	}
}

CON_DATA* DlgConsole::FindData( const string& key )
{
	ConDataIter iter = _cConDatas.find( key );
	if( iter != _cConDatas.end() )
		return &(iter->second);

	return NULL; 
}

void DlgConsole::AddData( const string& key, CON_DATA::VALUE_TYPE eType, void* func, const string& exp )
{
	ConDataIter iter =  _cConDatas.find( key );
	if( iter == _cConDatas.end() )
	{
		CON_DATA kNewData( key, eType, func, exp );
		_cConDatas.insert( ConDataConationer::value_type( key, kNewData ) );
	}
}

void DlgConsole::ShowAllCommand( )
{
	string temp;

	ConDataIter iter = _cConDatas.begin();
	++iter; //'?'

	for( iter ; iter != _cConDatas.end(); ++iter )
	{
		CON_DATA& kData = iter->second;
		if( kData._strKey == string( "help" ) )
			continue;

		temp = "[" + kData._strKey + "]" + " : " + kData._strExplain;
		AddString( temp.c_str() );
	}
}

void DlgConsole::Update( )
{
	_update_console( );
}

// helper
void CONSOLE_REGISTER( const string& key, CON_DATA::VALUE_TYPE eType, void* func, const string& explain )
{
	gConsole()->AddData( key, eType, func, explain );
}

#endif