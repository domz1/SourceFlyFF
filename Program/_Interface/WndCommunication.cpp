#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndCommand.h"
#include "party.h"

#include "WndManager.h"

#include "WndChatFilter.h"

#include "DPClient.h"
extern	CDPClient	g_DPlay;


#define		MAX_HISTORY_STRING		20

CWndEditChat::CWndEditChat()
{
}
CWndEditChat::~CWndEditChat()
{
}
void CWndEditChat::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWndChat* pWndChat = (CWndChat*) m_pParentWnd;
	if( pWndChat->m_pWndCommand )
		pWndChat->m_pWndCommand->Destroy();
	CWndEdit::OnLButtonDown( nFlags, point );
}
void CWndEditChat::OnSetFocus(CWndBase* pOldWnd)
{
	if( m_string.IsEmpty() )
	{
		CWndChat* pWndChat = (CWndChat*) m_pParentWnd;	

		if( pWndChat->m_pWndCommand == NULL )
		{
			if( pWndChat->m_nChatChannel == CHATSTY_GENERAL )
			{
			}
			else
			if( pWndChat->m_nChatChannel == CHATSTY_WHISPER )
			{
				CString string;
				if(  pWndChat->m_strCharName.IsEmpty() )
					string.Format( "/w " );
				else
					string.Format( "/w %s ", pWndChat->m_strCharName );
				SetString( string );
			}
			else
			if( pWndChat->m_nChatChannel == CHATSTY_SHOUT )
			{
				SetString( "/s " );
			}
			else
			if( pWndChat->m_nChatChannel == CHATSTY_PARTY )
			{
				SetString( "/p " );
			}
			else
			if( pWndChat->m_nChatChannel == CHATSTY_GUILD )
			{
				SetString( "/g " );
			}
		}
	}


	CWndEdit::OnSetFocus( pOldWnd );
}
void CWndEditChat::OnKillFocus(CWndBase* pNewWnd)
{
	CWndChat* pWndChat = (CWndChat*) m_pParentWnd;
	if( pWndChat->m_pWndCommand && this!= pNewWnd )
		pWndChat->m_pWndCommand->Destroy();
	CWndEdit::OnKillFocus( pNewWnd );
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 채팅 윈도 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndMacroChat::CWndMacroChat()
{
}
CWndMacroChat::~CWndMacroChat()
{
}
void CWndMacroChat::OnDraw( C2DRender* p2DRender )
{
	CWndChat* pWndChat = (CWndChat*)g_WndMng.GetApplet( APP_COMMUNICATION_CHAT );
	CWndEditChat* pWndEdit = &pWndChat->m_wndEdit;//GetDlgItem( WIDC_EDIT );
	if( pWndEdit->m_string.IsEmpty() == FALSE )
		p2DRender->RenderTexture( CPoint( 0, 0 ), &m_texMacroChat );
}
void CWndMacroChat::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWndChat* pWndChat = (CWndChat*)g_WndMng.GetApplet( APP_COMMUNICATION_CHAT );
	CWndEditChat* pWndEdit = &pWndChat->m_wndEdit;//GetDlgItem( WIDC_EDIT );
	if( pWndEdit->m_string.IsEmpty() == FALSE )
	{
		CWndChat* pWndChat = (CWndChat*)g_WndMng.GetApplet( APP_COMMUNICATION_CHAT );
		m_GlobalShortcut.m_dwShortcut = SHORTCUT_CHAT;
		m_GlobalShortcut.m_pFromWnd = this;
		m_GlobalShortcut.m_pTexture = m_pTexMacro; //.m_pFromWnd   = this;
		m_GlobalShortcut.m_dwData = 0;
		//CWndEdit* pWndEdit = (CWndEdit*)pWndChat->GetDlgItem( WIDC_EDIT );
		//strcpy( m_GlobalShortcut.m_szString, pWndEdit->m_string );

		CString strFormat;
		strFormat = pWndEdit->m_string;

		pWndEdit->m_string.GetTextFormat(strFormat);
		
		if( pWndEdit->m_string.GetLength() >= MAX_SHORTCUT_STRING-1 )
		{
			memcpy( m_GlobalShortcut.m_szString, strFormat, MAX_SHORTCUT_STRING-1 );
		}
		else
		{
			strcpy( m_GlobalShortcut.m_szString, strFormat );
		}
	}
}

void CWndMacroChat::OnMouseMove(UINT nFlags, CPoint point)
{
}
void CWndMacroChat::OnInitialUpdate()
{
	//m_ptexMacroe.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_ICON, "icon_MacroChat.dds" ), 0xffff00ff );
	m_pTexMacro = m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_ICON, "icon_MacroChat.dds" ), 0xffff00ff );//, bMyLoader );
	m_texMacroChat.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "wndButtMacroChat.bmp" ), 0xffff00ff );
}
CWndTextChat::CWndTextChat()
{
}
CWndTextChat::~CWndTextChat()
{
}
BOOL CWndTextChat::IsPickupSpace( CPoint point )
{
	return FALSE;
}
int CWndChat::m_nChatChannel = CHATSTY_GENERAL;

CWndChat::CWndChat()
{
	m_pWndCommand = NULL;
//	m_timerInsMsg.Set( MIN( 1 ) );
	m_timerInsMsg.Set( MIN( 2 ) );  // 도움말 표시 간격 증가
#ifdef __CSC_GAME_GRADE
	m_timerAlertGGrade.Set( MIN(60) );
#endif //__CSC_GAME_GRADE
	m_nInsMsgCnt = 0;
	m_bChatLock = FALSE;
	m_bMoveLock = FALSE;
	m_pWndChatFilter = NULL;
	m_nHistoryIndex = 0;
#if __VER >= 8 //__CSC_VER8_1
	m_bChatLog = FALSE;
#endif //__CSC_VER8_1

}
CWndChat::~CWndChat()
{
	SAFE_DELETE( m_pWndCommand );
	SAFE_DELETE( m_pWndChatFilter );
}
void CWndChat::OnDraw(C2DRender* p2DRender)
{
	if( g_pPlayer == NULL )
		return;
	//if( timer.IsTimeOut() )
	//{
	//	timer.Set( SEC( 10 ) );
	//	if( szMessage[ nMsgCnt ][0] )
	//		PutString( szMessage[ nMsgCnt ++ ] );
	//}
	if( g_Option.m_nInstantHelp )
	{
		CString string;
		if( m_strArray.GetSize() && m_timerInsMsg.TimeOut() )
		{
			m_timerInsMsg.Reset();
			//string = "[도움말] ";
			string  = prj.GetText(TID_SYS_HELP);
			string += " ";
			string += m_strArray.GetAt( m_nInsMsgCnt );
			g_WndMng.PutString( string, NULL, 0xffffff00, CHATSTY_HELP );
			m_nInsMsgCnt++;
			if( m_nInsMsgCnt == m_strArray.GetSize() )
				m_nInsMsgCnt = 0;
		}
	}

#ifdef __CSC_GAME_GRADE
	if(m_timerAlertGGrade.TimeOut())
	{
		m_timerAlertGGrade.Reset();
		//PK서버 구분  12세 or 15세
		if(g_eLocal.GetState( EVE_PK ))
			g_WndMng.PutString( prj.GetText(TID_SYS_ALERTGGRADE_PK), NULL, 0xffffff00, CHATSTY_HELP );
		else
			g_WndMng.PutString( prj.GetText(TID_SYS_ALERTGGRADE_NONPK), NULL, 0xffffff00, CHATSTY_HELP );		
	}
#endif //__CSC_GAME_GRADE

	CWndButton* pWndRadio4 = (CWndButton*)GetDlgItem( WIDC_RADIO4 ); // party
	CWndButton* pWndRadio5 = (CWndButton*)GetDlgItem( WIDC_RADIO5 ); // guild

	pWndRadio4->EnableWindow( g_Party.IsMember( g_pPlayer->m_idPlayer ) );
	pWndRadio5->EnableWindow( g_pPlayer->GetGuild() == NULL ? FALSE : TRUE );

	if( ::GetLanguage() == LANG_KOR || ::GetLanguage() == LANG_TWN || ::GetLanguage() == LANG_HK )
	{
		static TCHAR text[12] = { 0 };
		static wchar_t* wText = g_imeMgr.GetStringIndicator();

		if( wText )
		{
			CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC1 ); 

			if( pWndStatic )
			{
				WideCharToMultiByteEx(g_imeMgr.m_codePage, 0, wText, wcslen(wText), text, 12, NULL, NULL);		
				
				pWndStatic->m_dwColor = D3DCOLOR_ARGB( 255, 50, 50, 220 );

				if( PRIMARYLANGID(g_imeMgr.m_hkl)==LANG_KOREAN )
				{
					if( g_imeMgr.GetImeState() == 0 )
					{
						_tcscpy( text, "En" );
					}
				}
				else
				if( PRIMARYLANGID(g_imeMgr.m_hkl)==LANG_CHINESE )
				{
					static wchar_t wTextCh[3][3] = 
					{ 
						L"\x6CE8", 
						L"\x65B0", 
						L"\x5176", 
					};
					
					if( g_imeMgr.IsNewPhonetic() )
					{
						if( g_imeMgr.GetImeState() == 2 )
							_tcscpy( text, "En" );
						else
							WideCharToMultiByteEx(g_imeMgr.m_codePage, 0, wTextCh[1], wcslen(wTextCh[1]), text, 12, NULL, NULL);		
					}
					else
					if( g_imeMgr.IsPhonetic() )
					{
						if( g_imeMgr.GetImeState() == 0 )
							_tcscpy( text, "En" );
						else	
							WideCharToMultiByteEx(g_imeMgr.m_codePage, 0, wTextCh[0], wcslen(wTextCh[0]), text, 12, NULL, NULL);		
					}
					else
					{
						if( g_imeMgr.GetImeState() == 0 )
							_tcscpy( text, "En" );
						else	
							WideCharToMultiByteEx(g_imeMgr.m_codePage, 0, wTextCh[2], wcslen(wTextCh[2]), text, 12, NULL, NULL);		
					}
				}
				pWndStatic->SetTitle( text );
			}
		}
	}
}
void CWndChat::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

#ifdef __CHAT_V19
	CWndButton* pWndLock = (CWndButton*)GetDlgItem( WIDC_CHECK1 );
	CWndButton* pWndLog = (CWndButton*)GetDlgItem( WIDC_CHECK3 );
	CWndButton* pWndMoveLock = (CWndButton*)GetDlgItem( WIDC_MOVELOCK );
	CWndButton* pWndHelp = (CWndButton*)GetDlgItem( WIDC_HELP );
	pWndLock->SetVisible( false );
	pWndLog->SetVisible( TRUE );
	pWndMoveLock->SetVisible( false );
	pWndHelp->SetVisible( false );
#endif //__CHAT_V19

	//AddWndStyle( WBS_THICKFRAME );

	m_timerDobe.Set( SEC( 5 ) );

	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_MACRO_CHAT );
	LPWNDCTRL lpWndText = GetWndCtrl( WIDC_CUSTOM1 );
	m_wndMacroChat.Create( "g", WBS_NODRAWFRAME, lpWndCtrl->rect, this, WIDC_MACRO_CHAT );
	m_wndMacroChat.AddWndStyle( WBS_NODRAWFRAME );

	lpWndCtrl = GetWndCtrl( WIDC_EDIT );
	m_wndEdit.Create( g_Neuz.GetSafeHwnd(), WBS_NODRAWFRAME, lpWndCtrl->rect, this, WIDC_EDIT );
	m_wndText.Create( WBS_NOFRAME | WBS_NODRAWFRAME | WBS_VSCROLL, lpWndText->rect, this, WIDC_TEXT );
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );
	CWndEdit* pWndHead = (CWndEdit*)GetDlgItem( WIDC_HEAD );
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT );

	if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
	{
		pWndText->m_bEnableClipboard = TRUE;
		pWndHead->m_bEnableClipboard = TRUE;
		pWndEdit->m_bEnableClipboard = TRUE;
	}
	pWndEdit->SetNativeMode();

#if __VER >= 11 // __CSC_VER11_1
	CRect rectEdit = m_wndEdit.GetWndRect();
	rectEdit.left = rectEdit.left - 90;
	m_wndEdit.SetWndRect( rectEdit );

	CRect customRect = lpWndCtrl->rect;
	CWndCustom* pCustom = (CWndCustom*)GetDlgItem(WIDC_MACRO_CHAT);
	if(pCustom)
#ifdef __CHAT_V19
		pCustom->Move(customRect.TopLeft().x - 112, customRect.TopLeft().y - 2);
#else
		pCustom->Move(customRect.TopLeft().x - 119, customRect.TopLeft().y);
		pWndEdit->Move(customRect.Left().x + 105, customRect.bottom().y + 30);
#endif //__CHAT_V19
	if(pWndHead)
	{
		pWndHead->EnableWindow(FALSE);
		pWndHead->SetVisible(FALSE);
	}
#endif //__CSC_VER11_1
	if( g_Option.m_nInstantHelp )
	{
		CScript	s;
		s.Load( MakePath( DIR_CLIENT, "InstantHelp.inc" ) );
		s.GetToken();
		while( s.tok != FINISHED )
		{
			m_strArray.Add( s.Token );
			s.GetToken();
		}
	}
	//m_strArray.

/*
	CRect rect = GetClientRect();
	rect.top = rect.bottom - 20;
	rect.right = 70;
	rect.left += 5;
	rect.DeflateRect( 1, 1 );
	m_wndBeginning.Create( g_Neuz.GetSafeHwnd(), WBS_NODRAWFRAME, rect, this, 1000);

	rect = GetClientRect();
	rect.top = rect.bottom - 20;
	rect.left = 75;
	rect.right -= 134;
	rect.DeflateRect( 1, 1 );
	//rect.left = rect.Width() * 20 / 100;
	//m_wndEdit.AddWndStyle( WBS_NODRAWFRAME );
	m_wndEdit.Create( g_Neuz.GetSafeHwnd(), WBS_NODRAWFRAME, rect, this, 1001 );
*/
//	m_texEdit.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "WndChat2.bmp" ), 0xffff00ff, TRUE );
/*
	rect = GetClientRect();
	rect.top = rect.bottom - 20;
	rect.left = rect.right - 134;
	rect.right = r mnnnnft + 15;
	rect.DeflateRect( 1, 1 );
	m_wndShout.Create( " ", 0, rect, this, 1010 );

	rect = GetClientRect();
	rect.top = rect.bottom - 20;
	rect.left = rect.right - 117;
	rect.right = rect.left + 15;
	rect.DeflateRect( 1, 1 );
	m_wndParty.Create( " ", 0, rect, this, 1011 );
*/

/*
	m_wndText.AddWndStyle( WBS_VSCROLL );
	m_wndText.AddWndStyle( WBS_NOFRAME );
	m_wndText.AddWndStyle( WBS_NODRAWFRAME );
	m_wndText.Create( 0, rect, this, 1003 ); 
	m_wndText.SetFont( g_Neuz.m_Theme.m_pFontChat );
	m_wndText.m_string.Reset( m_pFont, &rect);
*/
/*
	rect = GetClientRect();
	rect.left = rect.right - 100;
	//rect.bottom -= 80;
	rect.DeflateRect( 1, 1 );
	m_wndReceiver.AddWndStyle( WBS_VSCROLL );
	m_wndReceiver.AddWndStyle( WBS_NOFRAME );
	m_wndReceiver.Create( EBS_AUTOVSCROLL, rect, this, 1002 );
	m_wndReceiver.AddString( "Vampyre\n" );
	m_wndReceiver.AddString( "Xuzhu\n" );
	m_wndReceiver.AddString( "Zodiacus\n" );
	m_wndReceiver.AddString( "Jeff\n" );
	m_wndReceiver.m_string.Reset( m_pFont, &rect);
*/

	//CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );
	SetChannel();
	CWndButton* pWndRadio1 = (CWndButton*)GetDlgItem( WIDC_RADIO1 );
	CWndButton* pWndRadio2 = (CWndButton*)GetDlgItem( WIDC_RADIO2 );
	CWndButton* pWndRadio3 = (CWndButton*)GetDlgItem( WIDC_RADIO3 );
	CWndButton* pWndRadio4 = (CWndButton*)GetDlgItem( WIDC_RADIO4 );
	CWndButton* pWndRadio5 = (CWndButton*)GetDlgItem( WIDC_RADIO5 );
	if( m_nChatChannel == CHATSTY_GENERAL ) pWndRadio1->SetCheck( TRUE );	else
	if( m_nChatChannel == CHATSTY_WHISPER ) pWndRadio2->SetCheck( TRUE );	else
	if( m_nChatChannel == CHATSTY_SHOUT   ) pWndRadio3->SetCheck( TRUE );	else
	if( m_nChatChannel == CHATSTY_PARTY   ) pWndRadio4->SetCheck( TRUE );	else
	if( m_nChatChannel == CHATSTY_GUILD   ) pWndRadio5->SetCheck( TRUE );
	//pWndText->m_string.Init(
	//pWndText->m_string = g_WndMng.m_ChatString;
	pWndText->m_nLineSpace = 2;
	//pWndText->m_string.Reset( m_pFont, &pWndText->GetClientRect() );
	
	//pWndText->
	/*
	CRect rect = GetClientRect();
	rect.bottom -= 20;
	rect.right -= 0;//100;
	rect.DeflateRect( 1, 1);
	m_wndText.SetWndRect( rect );
*/
	//LPBYTE m_wndChatEdit[3];
	//CSize sizeWndChatEdit[3];
#if __VER >= 11 // __CSC_VER11_1
	LoadTGA( MakePath( DIR_THEME, "WndChatEdit00_1.tga" ), &m_wndChatEdit[0] );
#else //__CSC_VER11_1
	LoadTGA( MakePath( DIR_THEME, "WndChatEdit00.tga" ), &m_wndChatEdit[0] );
#endif //__CSC_VER11_1
	LoadTGA( MakePath( DIR_THEME, "WndChatEdit01.tga" ), &m_wndChatEdit[1] );
#ifdef __LANG_IME_0327
	LoadTGA( MakePath( DIR_THEME,  ::GetLanguage(), "WndChatEdit02.tga" ), &m_wndChatEdit[2] );
#else	// __LANG_IME_0327
	LoadTGA( MakePath( DIR_THEME, "WndChatEdit02.tga" ), &m_wndChatEdit[2] );
#endif	// __LANG_IME_0327

	if( m_wndChatEdit[2].lpData == NULL )
	{
		Error( "CWndChat::OnInitialUpdate() m_wndChatEdit[2].lpData = NULL" );
		return;
	}
/*
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( 154, rectRoot.bottom - rectWindow.Height() );
	Move( point );
*/
#ifndef __CHAT_V19
	CWndButton* pWndLock = (CWndButton*)GetDlgItem( WIDC_CHECK1 );
	pWndLock->SetCheck( m_bChatLock );
	CWndButton* pWndMoveLock = (CWndButton*)GetDlgItem( WIDC_MOVELOCK );
	pWndMoveLock->SetCheck( m_bMoveLock );
#endif //__CHAT_V19
	/*CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rect = GetWindowRect();
	int nWidth  = rect.Width(); 
	int nHeight = rect.Height(); 
	int x = 0;//rectRoot.left + (rectRoot.Width()  / 2) - (nWidth  / 2);
	int y = rectRoot.Height() - rect.Height() - 80;//rectRoot.bottom - nHeight;	
	CPoint point( x, y );
	Move( point );*/

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CRect rect = g_Neuz.GetDeviceRect();
	CPoint point( 0, rect.Height() - rectWindow.Height() - 80  );
	Move( point );

	m_timerInputTimeOut.Set( SEC( 300 ) );

#if __VER >= 8 //__Y_CHAT_SYSTEM_8
	if( g_WndMng.m_pWndChatLog == NULL )
	{
		g_WndMng.m_pWndChatLog = new CWndChatLog;
		
		if( g_WndMng.m_pWndChatLog )
		{
			g_WndMng.m_pWndChatLog->Initialize( NULL, APP_CHAT_LOG );
			
			CRect rectRoot = m_pWndRoot->GetLayoutRect();
			CRect rect = GetWindowRect(TRUE);
			CRect rect2 = g_WndMng.m_pWndChatLog->GetClientRect(TRUE);
			
			int x = 0;
			int y = rect.top;
			
			if( (rect.right+rect2.Width()) < rectRoot.right )
				x = rect.right;
			else
				x = (rect.left - rect2.Width() );
			
			CPoint point( x, y );
			g_WndMng.m_pWndChatLog->Move( point );
           // pWndCheck3->SetCheck(false);
			//g_WndMng.m_pWndChatLog->SetVisible(FALSE);
		}
	}

	if( g_WndMng.m_pWndChatLog)
	{
		CWndButton* pWndCheck3  = (CWndButton*)GetDlgItem( WIDC_CHECK3 );
		if( pWndCheck3 )
			//pWndCheck3->SetCheck(g_WndMng.m_pWndChatLog->m_bc);
		    g_WndMng.m_pWndChatLog->SetVisible(false);
		//SetVisible
	}

#endif //__Y_CHAT_SYSTEM_8
}
BOOL CWndChat::Process ()
{
	if( m_timerInputTimeOut.IsTimeOut() )
	{
		CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );

		pWndText->m_string.SetAlpha( 0 );
	}
	
	if( 0 < strlen( g_Neuz.m_szWhisperName ) )
	{
		LPCTSTR lpstr = m_wndEdit.GetString();
		BOOL bCommand = FALSE;
		if( strcmp( lpstr, "/r " ) == 0 || strcmp( lpstr, "/R " ) == 0 || ( GetLanguage() == LANG_KOR && strcmp( lpstr, "/ㄱ " ) == 0 ) )
		{
			CString strWhisper;
			strWhisper.Format( "/whisper %s ", g_Neuz.m_szWhisperName );
			m_wndEdit.SetString( strWhisper );
			m_wndEdit.SetFocus();
		}		
	}
	
	return TRUE;
}

void CWndChat::SetChannel()
{
	m_wndText.SetString( "" );

	int nChannel = 0;
	switch( CWndChat::m_nChatChannel )
	{
		case CHATSTY_GENERAL: nChannel = 0; break;
		case CHATSTY_WHISPER: nChannel = 1; break;
		case CHATSTY_SHOUT  : nChannel = 2; break;
		case CHATSTY_PARTY  : nChannel = 3; break;
		case CHATSTY_GUILD  : nChannel = 4; break;
	}
	DWORD dwChatFilter = g_Option.m_dwChatFilterEtc | g_Option.m_dwChatFilter[ nChannel ];

	for( int i = 0; i < g_WndMng.m_aChatString.GetSize(); i++ )
	{
		if( g_WndMng.m_aChatStyle.GetAt( i ) & dwChatFilter ) 
		{
			CString string = g_WndMng.m_aChatString.GetAt( i );
			m_wndText.m_string.AddParsingString( string, g_WndMng.m_aChatColor.GetAt( i ), 0x00000000, 0, PS_NOT_MACRO );
			m_wndText.m_string.AddString( "\n" );
		}
	}
	m_wndText.ResetString();
	m_wndText.m_wndScrollBar.SetMaxScrollPos();
}
void CWndChat::OnSetFocus(CWndBase* pOldWnd)
{
}
void CWndChat::OnKillFocus(CWndBase* pNewWnd)
{
	//if( m_pWndCommand && pNewWnd != m_pWndCommand )
	//	m_pWndCommand->Destroy();
}
BOOL CWndChat::Initialize(CWndBase* pWndParent,DWORD dwWndId)
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	//CRect rect( 154, rectWindow.Height() - 100, 154 + rectWindow.Width() - 300, rectWindow.Height() );
	SetTitle( GETTEXT( TID_APP_COMMUNICATION_CHAT ) );
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, WBS_KEY| WBS_THICKFRAME, CPoint( 792, 130 ), pWndParent );
	//return CWndNeuz::Create( 0|WBS_MOVE|/*WBS_DOCKABLE|*/WBS_SOUND|WBS_CAPTION|WBS_THICKFRAME|WBS_MAXIMIZEBOX, rect, &g_WndMng, dwWndId);
}

/*
	"applet" TY_CLIENT 0
	"to"     TY_SERVER 0 
	"to"     TY_SERVER 0
	"to"     TY_SERVER 0
*/

extern	CDPClient	g_DPlay;

void CWndChat::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}
void CWndChat::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}
void CWndChat::OnDestroyChildWnd( CWndBase* pWndChild )
{
	if( m_pWndCommand == pWndChild )
	{
		SAFE_DELETE( m_pWndCommand );
	}
	if( m_pWndChatFilter == pWndChild )
	{
		SAFE_DELETE( m_pWndChatFilter );
		//ChatFilter Wnd Destroy후 Focus가 World로 되어있지 않아 이동키가 먹지 않기 때문에 SetFocus해줌.
		CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetApplet( APP_WORLD );
		if( pWndWorld ) 
			pWndWorld->SetFocus();
	}
}
void CWndChat::SerializeRegInfo( CAr& ar, DWORD& dwVersion )
{
	CWndNeuz::SerializeRegInfo( ar, dwVersion );
	if( ar.IsLoading() )
	{
		if( dwVersion == 0 )
		{
		}
		else
		{
			ar >> m_bChatLock;
		}
#ifndef __CHAT_V19
		CWndButton* pWndLock = (CWndButton*)GetDlgItem( WIDC_CHECK1 );
		pWndLock->SetCheck( m_bChatLock );
#endif //__CHAT_V19
	}
	else
	{
		dwVersion = 1;
		ar << m_bChatLock;
	}
}

BOOL CWndChat::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	m_timerInputTimeOut.Reset();

	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );	
	if( pWndText )
		pWndText->m_string.SetAlpha( 255 );	
	
	switch(nID)
	{
		case WIDC_CLOSE:
			nID = WTBID_CLOSE;
			break;
		case WIDC_HELP:
			nID = WTBID_HELP;
			break;
		case WIDC_BUTTON1:
			{
				if( m_pWndChatFilter == NULL )
				{
					m_pWndChatFilter = new CWndChatFilter;
					m_pWndChatFilter->Initialize( this );
				}
			}
			break;
#ifndef __CHAT_V19
		case WIDC_CHECK1: // 채팅 입력창 잠구기 
			{
				CWndButton* pWndCheck = (CWndButton*)GetDlgItem( WIDC_CHECK1 );
				m_bChatLock = pWndCheck->GetCheck();
			}
			break;
#endif //__CHAT_V19
#if __VER >= 8 //__Y_CHAT_SYSTEM_8
		case WIDC_CHECK3: // 시스탐창
			{
				CWndButton* pWndCheck = (CWndButton*)GetDlgItem( WIDC_CHECK3 );
				BOOL bChatLog = pWndCheck->GetCheck();
#if __VER >= 8 //__CSC_VER8_1 모든 윈도우를 지울때 ChatLog의 유무 구별을 위해 추가.
				m_bChatLog = bChatLog;
#endif //_CSC_VER8_1
				if( g_WndMng.m_pWndChatLog )
				{
					if( bChatLog )
					{
						CRect rectRoot = m_pWndRoot->GetLayoutRect();
						CRect rect = GetWindowRect(TRUE);
						CRect rect2 = g_WndMng.m_pWndChatLog->GetClientRect(TRUE);
						
						int x = 0;
						int y = rect.top;
						
						if( (rect.right+rect2.Width()) < rectRoot.right )
							x = rect.right;
						else
							x = (rect.left - rect2.Width() );
						
						CPoint point( x, y );
						g_WndMng.m_pWndChatLog->Move( point );
					}

					g_WndMng.m_pWndChatLog->SetVisible( bChatLog );
				}
			}
			break;
#endif //__Y_CHAT_SYSTEM_8
#ifndef __CHAT_V19
		case WIDC_MOVELOCK: // 이동 잠그기
			{
				CWndButton* pWndCheck = (CWndButton*)GetDlgItem( WIDC_MOVELOCK );
				m_bMoveLock = pWndCheck->GetCheck();
				DWORD dwStyle = GetWndStyle();
				if( m_bMoveLock )
					dwStyle &= (~WBS_MOVE);
				else
					dwStyle |= WBS_MOVE;
				SetWndStyle( dwStyle );
			}
			break;
#endif //__CHAT_V19
		case WIDC_RADIO1:
			m_nChatChannel = CHATSTY_GENERAL;
			m_wndEdit.SetString( "" );
			m_wndEdit.SetFocus();
			SetChannel();
			break;
		case WIDC_RADIO2:
			m_nChatChannel = CHATSTY_WHISPER;
			m_wndEdit.SetString( "/w " );
			m_wndEdit.SetFocus();
			SetChannel();
			break;
		case WIDC_RADIO3:
			m_nChatChannel = CHATSTY_SHOUT;
			m_wndEdit.SetString( "/s " );
			m_wndEdit.SetFocus();
			SetChannel();
			break;
		case WIDC_RADIO4:
			m_nChatChannel = CHATSTY_PARTY;
			m_wndEdit.SetString( "/p " );
			m_wndEdit.SetFocus();
			SetChannel();
			break;
		case WIDC_RADIO5:
			m_nChatChannel = CHATSTY_GUILD;
			m_wndEdit.SetString( "/g " );
			m_wndEdit.SetFocus();
			SetChannel();
			break;
		case APP_COMMAND:
			{
				if( message == WNM_DBLCLK )
				{
					CString string;
					m_pWndCommand->GetText( m_pWndCommand->GetCurSel(), string );
					string += " ";
					m_wndEdit.SetString( "/" );
					m_wndEdit.AddString( string );
					m_wndEdit.ResetString();
					m_pWndCommand->Destroy();
				}
				SetFocus();
			}
		case WIDC_HEAD: // 말머리
			if( message == EN_RETURN )
			{
				g_WndMng.SetFocus();
			}
			break;
		case WIDC_EDIT: // 본문
			if( message == EN_UP )		
			{
				if( m_strHistory.size() == 0 )
					return FALSE;

				if( m_nHistoryIndex >= (int)( m_strHistory.size() ) )
				{
					m_strHistory.front();
					m_nHistoryIndex = 0;
				}

				m_nHistoryIndex++;

				m_wndEdit.m_string = m_strHistory[m_strHistory.size()-m_nHistoryIndex];

				return TRUE;
			}
			
			if( message == EN_RETURN )
			{
				BOOL bFinished = TRUE;
				if( m_pWndCommand )
				{
					CString str1, str2;
					str1 = m_wndEdit.m_string.Right( m_wndEdit.m_string.GetLength() - 1 );
					if( m_pWndCommand->GetCurSel() != -1 )
					{
						m_pWndCommand->GetText( m_pWndCommand->GetCurSel(), str2 );
						if( str1 != str2 )
						{
							str2 += " ";
							m_wndEdit.SetString( "/" );
							m_wndEdit.AddString( str2 );
							m_wndEdit.ResetString();
							bFinished = FALSE;
						}
					}
					m_pWndCommand->Destroy();
				}
				if( bFinished == TRUE )
				{
					CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );
					CWndEdit* pWndHead = (CWndEdit*)GetDlgItem( WIDC_HEAD );
					CString string; 
					if( pWndHead->m_string.IsEmpty() == FALSE && 
						( pWndHead->m_string[ 0 ] == '/' || 
						( m_wndEdit.m_string.IsEmpty() == FALSE && pWndHead->m_string[ 0 ] != '/' && m_wndEdit.m_string[ 0 ] != '/' ) ) 
					)
						string = pWndHead->m_string + " ";

					WORD wCodePage = g_codePage;
					const char* begin = m_wndEdit.m_string;
					const char* end = begin + m_wndEdit.m_string.GetLength();
					const char* iter = begin;
					while(*iter && iter < end)
					{
						if(*((BYTE*)iter) >= 0x80) 
						{
							if(wCodePage != m_wndEdit.m_string.m_awCodePage[iter-begin])
							{
								wCodePage = m_wndEdit.m_string.m_awCodePage[iter-begin];
								
								char temp[16];
								
								if(wCodePage == g_codePage)
								{
									wsprintf(temp, "#nl");
								}
								else
								{
									wsprintf(temp, "#l%04d", wCodePage);
								}
								string += temp;
							}
						}
						const char* next = CharNextEx(iter, wCodePage);
						char temp[16];
						memcpy(temp, iter, next-iter);
						temp[next-iter] = 0;
						string += temp;
						iter = next;
					}
					if( m_strHistory.size() >= MAX_HISTORY_STRING )
					{
						m_nHistoryIndex = 0;
						m_strHistory.erase( m_strHistory.begin() );
					}

					m_strHistory.push_back(string);
					int a = string.GetLength();
					if( !m_wndEdit.m_string.IsEmpty() )
						m_wndEdit.m_string.GetTextFormat(string);

					g_WndMng.ParsingChat( string );

					CScanner scanner;
					scanner.SetProg( m_wndEdit.m_string.LockBuffer() );
					scanner.GetToken(); // 
					if( scanner.Token == "/" )
					{
						scanner.GetToken();
						if( scanner.Token == "w" || scanner.Token == "whisper" )
						{
							scanner.GetToken();
							m_strCharName = scanner.Token;
						}
					}
					m_wndEdit.m_string.UnlockBuffer();
					m_wndEdit.Empty();
					if( m_bChatLock == FALSE ) 
					{
						if( IsWndStyle( WBS_MODAL ) == TRUE )
						{
							DelWndStyle( WBS_MODAL );
							if( g_Neuz.pOldModalWindow )
							{
								g_Neuz.pOldModalWindow->SetFocus();
							}
						}
						else
						{
							CWndWorld* pWndWorld = ( CWndWorld* )g_WndMng.GetApplet( APP_WORLD );
							if( pWndWorld )
							{
								pWndWorld->SetFocus();
							}
							else
							{
								g_WndMng.SetFocus();
							}
						}
					}
					else
						m_wndEdit.SetFocus();
				}
			}
			else
			if( message == EN_CHANGE )
			{
				if( m_wndEdit.m_string.IsEmpty() == FALSE )
				{
					if( m_wndEdit.m_string[ 0 ] == '/' && m_wndEdit.GetOffset() == 1 && m_pWndCommand == NULL )
					{
						m_pWndCommand = new CWndCommand;
						m_pWndCommand->m_pWndEdit = this;
						m_pWndCommand->Initialize( this );
						CRect rect = m_wndEdit.GetScreenRect();
						CPoint point = rect.TopLeft();
						point.y -= m_pWndCommand->GetWndRect().Height();
						m_pWndCommand->Move( point );
					}
					else
					if( m_pWndCommand )
					{
						CString string; 
						string = m_wndEdit.m_string.Right( m_wndEdit.m_string.GetLength() - 1 );
						// 중간에 스페이스가 껴 있으면 닫아버린다.
						if( string.Find( ' ' ) == -1 )
							m_pWndCommand->SelectString( 0, string );
						else
							m_pWndCommand->Destroy();
						
						if( ::GetLanguage() == LANG_KOR )
						{
							if( m_wndEdit.m_string == "/ㅎ " )
							{
								m_wndEdit.SetString( "/g " );
							}
							else if( m_wndEdit.m_string == "/ㅔ " )
							{
								m_wndEdit.SetString( "/p " );
							}
							else if( m_wndEdit.m_string == "/ㄴ " )
							{
								m_wndEdit.SetString( "/s " );
							}
#if __VER >= 11 // __CSC_VER11_1
							else if( m_wndEdit.m_string == "/ㅈ " )
							{
								m_wndEdit.SetString( "/w " );
							}
#endif //__CSC_VER11_1
						}
					}
				}
				else
				if( m_pWndCommand )
					m_pWndCommand->Destroy();
			}
			break;
		case 10000: // 최소화 
			break;
		case 10001: // 최대화 
			break;
		case 10002: // 종료
			Destroy();
			break;
	}
	
	//아래 특정 버튼 누를 경우 Focus가 World로 되어있지 않아 이동키가 먹지 않기 때문에 SetFocus해줌.
	if(nID == WIDC_CHECK1 || nID == WIDC_CHECK3 || nID == WIDC_MOVELOCK)
	{
		CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetApplet( APP_WORLD );
		if( pWndWorld ) 
			pWndWorld->SetFocus();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

BOOL CWndChat::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand(nID,dwMessage,pWndBase);
}
void CWndChat::Parsing( CString string )
{
	/*
	MotionProp* pProp;
	for( int i = 0; i < prj.m_aMotionProp.GetSize(); i++ )
	{
		pProp = prj.GetMotionProp( i );
		if( pProp && pProp->dwRequireLv )
		{
			if( string.Find( pProp->szLink ) != -1 )
			{
			//	g_pPlayer->SendActMsg( OBJMSG_MOTION, pProp->dwID );
			}
		}
	}
	*/
}
void CWndChat::PutString( LPCTSTR lpszString, DWORD dwColor, DWORD dwPStyle )
{
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );
	
	if( dwColor != 0xffffff00 )
	{
		m_timerInputTimeOut.Reset();
		
		CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );	
		pWndText->m_string.SetAlpha( 255 );	
	}

	BOOL bLineEnd = FALSE;
	if( pWndText->m_wndScrollBar.GetScrollPos() == ( pWndText->m_wndScrollBar.GetMaxScrollPos() - pWndText->m_wndScrollBar.GetScrollPage() ) )
		bLineEnd = TRUE;

	pWndText->AddString( lpszString, dwColor, dwPStyle );
	pWndText->AddString( "\n", dwColor );// += '\n';
	// 글 내용이 1000줄을 넘어가면 500라인을 잘라낸다.
	if( pWndText->m_string.GetLineCount() >= 1000 )
	{
		pWndText->m_string.DeleteLine( 0, 500 );
		pWndText->UpdateScrollBar();
	}
	if( bLineEnd )
		pWndText->m_wndScrollBar.SetMaxScrollPos();
}
void CWndChat::OnDestroy()
{
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );
	g_WndMng.m_ChatString = pWndText->m_string;
}
void CWndChat::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
#if __VER >= 8 //__Y_CHAT_SYSTEM_8
	AdjustMinRect( &rectWnd, 16 * 29, 16 * 3 );
#else // __Y_CHAT_SYSTEM_8
	AdjustMinRect( &rectWnd, 16 * 26, 16 * 3 );
#endif //__Y_CHAT_SYSTEM_8
	CRect rectOld = m_rectClient;
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect( 4, 4, 6, 6 );
	m_rectLayout = m_rectClient;
	if( bOnSize && ( rectOld.Width() != m_rectClient.Width() || rectOld.Height() != m_rectClient.Height() ) )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
	MakeVertexBuffer();
}
void CWndChat::OnSize(UINT nType, int cx, int cy)
{
//#ifdef __NEWINTERFACE
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );
	CWndEdit* pWndHead = (CWndEdit*)GetDlgItem( WIDC_HEAD );
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT );

	CRect rectEdit;
	CRect rect = GetClientRect();
	//rect.left = 60;
#ifdef __CHAT_V19
	rect.top = 8;
	rect.left = 8;
#else
	rect.top = 0;
	rect.left = 0;
#endif //__CHAT_V19
	rect.right -= 0;
	rect.bottom -= 39;
	pWndText->SetWndRect( rect );
	pWndText->m_wndScrollBar.SetMaxScrollPos();
	
	rect = GetClientRect();
	rectEdit = pWndHead->GetWndRect();
	rectEdit.top = rect.bottom - 18;
	rectEdit.bottom = rect.bottom;
	rect.DeflateRect( 1, 1 );
	pWndHead->SetWndRect( rectEdit );

	rect = GetClientRect();
	rectEdit = m_wndEdit.GetWndRect();
	rectEdit.top = rect.bottom - 20;
	rectEdit.bottom = rect.bottom - 4;
	rectEdit.right = rect.right - 20;
	rect.DeflateRect( 1, 1 );
	m_wndEdit.SetWndRect( rectEdit );

	rect = GetClientRect();
	rectEdit = m_wndMacroChat.GetWndRect();
	rectEdit.top = rect.bottom - 19;
	rectEdit.bottom = rect.bottom;
	m_wndMacroChat.SetWndRect( rectEdit );
	CWndButton* pWndRadio1 = (CWndButton*)GetDlgItem( WIDC_RADIO1 );
	CWndButton* pWndRadio2 = (CWndButton*)GetDlgItem( WIDC_RADIO2 );
	CWndButton* pWndRadio3 = (CWndButton*)GetDlgItem( WIDC_RADIO3 );
	CWndButton* pWndRadio4 = (CWndButton*)GetDlgItem( WIDC_RADIO4 );
	CWndButton* pWndRadio5 = (CWndButton*)GetDlgItem( WIDC_RADIO5 );
#ifdef __CHAT_V19
	CWndButton* pWndRadio6 = (CWndButton*)GetDlgItem( WIDC_RADIO6 );
#else
	CWndButton* pWndCheck  = (CWndButton*)GetDlgItem( WIDC_CHECK1 );
	CWndButton* pWndCheck2  = (CWndButton*)GetDlgItem( WIDC_MOVELOCK );
#endif //__CHAT_V19
	CWndButton* pWndButton1 = (CWndButton*)GetDlgItem( WIDC_BUTTON1 );
	CWndButton* pWndButton2 = (CWndButton*)GetDlgItem( WIDC_HELP   );
	CWndButton* pWndButton3 = (CWndButton*)GetDlgItem( WIDC_CLOSE  );

#if __VER >= 8 //__Y_CHAT_SYSTEM_8
	CWndButton* pWndCheck3  = (CWndButton*)GetDlgItem( WIDC_CHECK3 );
#else //__Y_CHAT_SYSTEM_8
	CWndButton* pWndCheck3  = (CWndButton*)GetDlgItem( WIDC_CHECK3 );
	pWndCheck3->SetVisible(FALSE);
#endif //__Y_CHAT_SYSTEM_8

	rect = GetClientRect();
	rectEdit = pWndRadio1->GetWndRect();
#ifdef __CHAT_V19
	rectEdit.top = rect.bottom - 42;
	rectEdit.left = 5;
	pWndRadio6->Move( rectEdit.TopLeft() ); 
	rectEdit.left = 36;
	pWndRadio1->Move( rectEdit.TopLeft() ); 
	rectEdit.left = 67;
	pWndRadio2->Move( rectEdit.TopLeft()  ); 
	rectEdit.left = 98;
	pWndRadio3->Move( rectEdit.TopLeft()  ); 
	rectEdit.left = 129;
	pWndRadio4->Move( rectEdit.TopLeft()  ); 
	rectEdit.left = 160;
	pWndRadio5->Move( rectEdit.TopLeft()  ); 
#else
	rectEdit.top = rect.bottom - 38;
	rectEdit.left = 0;
	pWndRadio1->Move( rectEdit.TopLeft() ); 
	rectEdit.left = 50;
	pWndRadio2->Move( rectEdit.TopLeft()  ); 
	rectEdit.left = 100;
	pWndRadio3->Move( rectEdit.TopLeft()  ); 
	rectEdit.left = 150;
	pWndRadio4->Move( rectEdit.TopLeft()  ); 
	rectEdit.left = 200;
	pWndRadio5->Move( rectEdit.TopLeft()  ); 
#endif //__CHAT_V19
#if __VER >= 8 //__Y_CHAT_SYSTEM_8
	rectEdit.left = rect.right - 200;
#ifndef __CHAT_V19
	pWndCheck->Move( rectEdit.TopLeft()  ); 
#endif //__CHAT_V19

#ifdef __CHAT_V19
	rectEdit.top -= 2;
	rectEdit.left = rect.right - 35;
#else
	rectEdit.left = rect.right - 150;
#endif //__CHAT_V19
	pWndButton1->Move( rectEdit.TopLeft()  ); 
#ifdef __CHAT_V19
	rectEdit.top += 2;
#endif //__CHAT_V19

	rectEdit.left = rect.right - 100;
	pWndCheck3->Move( rectEdit.TopLeft()  ); 	
#else //__Y_CHAT_SYSTEM_8
	rectEdit.left = rect.right - 150;
	pWndCheck->Move( rectEdit.TopLeft()  ); 
#ifdef __CHAT_V19
	rectEdit.left = rect.right - 200;
	rectEdit.top += 6;
#else
	rectEdit.left = rect.right - 100;
#endif //__CHAT_V19
	pWndButton1->Move( rectEdit.TopLeft() ); 
#ifdef __CHAT_V19
	rectEdit.top += 6;
#endif //__CHAT_V19
#endif //__Y_CHAT_SYSTEM_8
	rectEdit.left = rect.right - 49;

	CRect rectEdit2 = rectEdit;

	rectEdit2.top  += 1;
#ifndef __CHAT_V19
	pWndCheck2->Move( rectEdit2.TopLeft()  ); 
#endif //__CHAT_V19
	rectEdit.left = rect.right - 44;
	rectEdit.top -= 2;

#ifndef __CHAT_V19
	rectEdit.left = rect.right - 32;
	rectEdit.top -= 2;

	pWndButton2->Move( rectEdit.TopLeft()  );
	rectEdit.left = rect.right - 15;
#else//__CHAT_V19
	rectEdit.top -= 2;

	rectEdit.left = rect.right - 67;
	pWndCheck3->Move( rectEdit.TopLeft()  ); 	

	rectEdit.left = rect.right - 46;
	pWndButton1->Move( rectEdit.TopLeft()  );

	rectEdit.left = rect.right - 25;
#endif //__CHAT_V19

	pWndButton3->Move( rectEdit.TopLeft()  ); 

	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC1 ); 
	
	if( pWndStatic )
	{
		rectEdit.top  += 20;
		rectEdit.left -= 5;
		pWndStatic->Move( rectEdit.TopLeft()  ); 
	}

	CWndNeuz::OnSize(nType,cx,cy);
}
void CWndChat::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(IsWndRoot())
		return;

	//if(IsWndStyle(WBS_CAPTION) && m_bPickup)
	{//
	//	m_wndTitleBar.m_wndMinimize.SetVisible(TRUE);
		//m_wndTitleBar.m_wndMaximize.SetVisible(TRUE);
	}
	if( g_WndMng.m_pWndWorld )
	{
		CRect rect = GetWindowRect( TRUE );
		CPoint p;
		p.x = point.x + rect.left;
		p.y = point.y + rect.top;
//		g_WndMng.m_pWndWorld->m_bLButtonDown = TRUE;
		g_WndMng.m_pWndWorld->OnLButtonUp( nFlags, p );
	}
}

void CWndChat::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( m_pWndCommand )
		m_pWndCommand->Destroy();

	if( g_WndMng.m_pWndWorld )
	{
		CRect rect = GetWindowRect( TRUE );
		CPoint p;
		p.x = point.x + rect.left;
		p.y = point.y + rect.top;
		g_WndMng.m_pWndWorld->m_bLButtonDown = TRUE;
		g_WndMng.m_pWndWorld->OnLButtonDown( nFlags, p );
	}
}

void CWndChat::OnRButtonDown(UINT nFlags, CPoint point)
{
	if( g_WndMng.m_pWndWorld )
	{
		CRect rect = GetWindowRect( TRUE );
		CPoint p;
		p.x = point.x + rect.left;
		p.y = point.y + rect.top;
		g_WndMng.m_pWndWorld->m_bRButtonDown = TRUE;
		g_WndMng.m_pWndWorld->OnRButtonDown( nFlags, p );
	}

	int nLineCount = m_wndText.m_string.GetLineCount();
	int nPos = m_wndText.IsWndStyle( WBS_VSCROLL ) ? m_wndText.m_wndScrollBar.GetScrollPos() : 0;
	DWORD dwMaxHeight = m_wndText.GetFontHeight();
	for( int i = nPos ; i < nLineCount ; ++i )
	{
		if( (int)( ((i-nPos) * dwMaxHeight) ) <= point.y && point.y < (int)( (dwMaxHeight * (i-nPos + 1)) ) )
		{
			CString str = m_wndText.m_string.GetLine( i );

			char szFind[MAX_NAME] = {0,};
			
			BOOL bChat = FALSE;
			
			// 일반 채팅 검색
			int nFindName = str.Find( " :" );
			if( 0 < nFindName && nFindName < MAX_NAME )
			{
				int nFindSpace = str.Find( ' ' );
				if(  nFindSpace == -1 || nFindName <= nFindSpace )
				{
					strncpy( szFind, str, nFindName );
					if( strcmp( szFind, g_pPlayer->GetName() ) != 0 )
						bChat = TRUE;
				}
			}
			
			// 귓속말 검색
			if( bChat == FALSE )
			{
				int nFindName1 = str.Find( "<- [" );
				int nFindName2 = str.Find( "]" );
				if( 0 == nFindName1 )
				{
					str.Replace( "<- [", "" );
				}					
				else
				{
					nFindName1 = str.Find( "-> [" );
					str.Replace( "-> [", "" );
				}
				
				if( 0 == nFindName1 && 0 < nFindName2 && nFindName2 - nFindName1 < MAX_NAME + 4 )
				{
					nFindName2 = str.Find( "]" );
					strncpy( szFind, str, nFindName2 );
					int nFindSpace = strspn( " ", szFind );

					if(  nFindSpace < 1 && strcmp( szFind, g_pPlayer->GetName() ) != 0 )
						bChat = TRUE;

				}
			}
			
			// 외치기 검색( 알림은 빠짐 )
			if( bChat == FALSE )
			{
				int nFindName1 = str.Find( "[" );
				int nFindName2 = str.Find( "]" );
				if( 0 == nFindName1 && 0 < nFindName2 && nFindName2 - nFindName1 < MAX_NAME + 1 )
				{
					str.Replace( "[", "" );
					nFindName2 = str.Find( "]" );
					strncpy( szFind, str, nFindName2 );
					int nFindSpace = strspn( " ", szFind );

					if(  nFindSpace < 1 && strcmp( szFind, g_pPlayer->GetName() ) != 0 )
						bChat = TRUE;
					
					CString strSystem = prj.GetText(TID_ADMIN_ANNOUNCE);
					strSystem.Replace( "[", "" );
					int nFindSystem = strSystem.Find("]");
					if( 0 < nFindSystem )
					{
						char szFindSystem[MAX_NAME] = {0,};
						strncpy( szFindSystem, strSystem, nFindSystem );
						if( strcmp( szFind, szFindSystem ) == 0 )
							bChat = FALSE;
					}
				}
			}	
			if( bChat )
			{
				CString strWhisper;
				strWhisper.Format( "/whisper %s ", szFind );
				m_wndEdit.SetString( strWhisper );
				m_wndEdit.SetFocus();
			}
			break;
		}
	}
}	
void CWndChat::OnRButtonUp(UINT nFlags, CPoint point)
{
	if( g_WndMng.m_pWndWorld )
	{
		CRect rect = GetWindowRect( TRUE );
		CPoint p;
		p.x = point.x + rect.left;
		p.y = point.y + rect.top;
//		g_WndMng.m_pWndWorld->m_bRButtonDown = TRUE;
		g_WndMng.m_pWndWorld->OnRButtonUp( nFlags, p );
	}
}

void CWndChat::OnMouseMove(UINT nFlags, CPoint point)
{
	if( g_WndMng.m_pWndWorld )
	{
		CRect rect = GetWindowRect( TRUE );
		CPoint p;
		p.x = point.x + rect.left;
		p.y = point.y + rect.top;
//		g_WndMng.m_pWndWorld->m_bRButtonDown = TRUE;
		g_WndMng.m_pWndWorld->OnMouseMove( nFlags, p );
	}
}



BOOL CWndChat::OnEraseBkgnd(C2DRender* p2DRender)
{
	return TRUE;
}
//void CWndNeuz::PaintTexture( LPBYTE pDest, LPBYTE pSrc, CPoint pt,
// CSize sizeTexture, CSize sizeSurface )

void CWndChat::AdditionalSkinTexture( LPWORD pDest, CSize size, D3DFORMAT d3dFormat )
{
	CWndNeuz::AdditionalSkinTexture( pDest, size, d3dFormat );

	CRect rect = GetWindowRect();
	CPoint point =  CPoint( 0, rect.Height() - m_wndChatEdit[0].size.cy );
	PaintTexture( pDest, &m_wndChatEdit[0], point, size );
	int nWidth = ( rect.Width() - m_wndChatEdit[0].size.cx ) / m_wndChatEdit[1].size.cx;
	point.x += m_wndChatEdit[0].size.cx;
	for( int i = 0; i < nWidth; i++ )
	{
		if( i == nWidth - 2 )
			PaintTexture( pDest, &m_wndChatEdit[2], point, size );
		else
		if( i < nWidth - 2 )
			PaintTexture( pDest, &m_wndChatEdit[1], point, size );
		point.x += m_wndChatEdit[1].size.cx;
	}
}

#if __VER >= 8 //__Y_CHAT_SYSTEM_8

CWndChatLog::CWndChatLog()
{
}
CWndChatLog::~CWndChatLog()
{
}
/*
void CWndChatLog::OnDraw(C2DRender* p2DRender)
{
}
*/
void CWndChatLog::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	LPWNDCTRL lpWndText = GetWndCtrl( WIDC_CUSTOM1 );
	m_wndText.Create( WBS_NOFRAME | WBS_NODRAWFRAME | WBS_VSCROLL, lpWndText->rect, this, WIDC_TEXT );
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );

	if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
	{
		pWndText->m_bEnableClipboard = TRUE;
	}

	pWndText->m_nLineSpace = 2;

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rect = GetWindowRect();
	int nWidth  = rect.Width(); 
	int nHeight = rect.Height(); 
	int x = 0;//rectRoot.left + (rectRoot.Width()  / 2) - (nWidth  / 2);
	int y = rectRoot.bottom - nHeight;	
	CPoint point( x, y );
	Move( point );
	
	m_wndMenuPlace.CreateMenu( this );	
	m_wndMenuPlace.AppendMenu( 0, 0 , prj.GetText(TID_GAME_GUIDE_HIDE) );
}


BOOL CWndChatLog::Process ()
{	
	return TRUE;
}
void CWndChatLog::OnSetFocus(CWndBase* pOldWnd)
{
}
void CWndChatLog::OnKillFocus(CWndBase* pNewWnd)
{
}
BOOL CWndChatLog::Initialize(CWndBase* pWndParent,DWORD dwWndId)
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 154, rectWindow.Height() - 100, 154 + rectWindow.Width() - 300, rectWindow.Height() );
	SetTitle( GETTEXT( TID_APP_COMMUNICATION_CHAT ) );


	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, WBS_THICKFRAME, CPoint( 792, 130 ), pWndParent );
//	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint( 792, 130 ), pWndParent );
}

void CWndChatLog::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}
void CWndChatLog::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}
void CWndChatLog::OnDestroyChildWnd( CWndBase* pWndChild )
{
}
BOOL CWndChatLog::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	CWndBase* pWndBase = (CWndBase*) pLResult;
	
	if( pWndBase->m_pParentWnd == &m_wndMenuPlace )
	{
		SetFocus();
		switch( nID )
		{
		case 0:
			{
				CWndChat* pWndChat = (CWndChat*)g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );

				if( pWndChat )
				{
					CWndButton* pWndCheck = (CWndButton*)pWndChat->GetDlgItem( WIDC_CHECK3 );

					if( pWndCheck )
						pWndCheck->SetCheck(FALSE);
				}

				SetVisible(FALSE);
			}
			break;
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

BOOL CWndChatLog::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand(nID,dwMessage,pWndBase);
}
void CWndChatLog::PutString( LPCTSTR lpszString, DWORD dwColor, DWORD dwPStyle )
{
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );
	
	BOOL bLineEnd = FALSE;
	if( pWndText->m_wndScrollBar.GetScrollPos() == ( pWndText->m_wndScrollBar.GetMaxScrollPos() - pWndText->m_wndScrollBar.GetScrollPage() ) )
		bLineEnd = TRUE;

	pWndText->AddString( lpszString, dwColor, dwPStyle );
	pWndText->AddString( "\n", dwColor );// += '\n';
	// 글 내용이 1000줄을 넘어가면 500라인을 잘라낸다.
	if( pWndText->m_string.GetLineCount() >= 1000 )
	{
		pWndText->m_string.DeleteLine( 0, 500 );
		pWndText->UpdateScrollBar();
	}
	if( bLineEnd )
		pWndText->m_wndScrollBar.SetMaxScrollPos();
}
void CWndChatLog::OnDestroy()
{
}
void CWndChatLog::OnSize(UINT nType, int cx, int cy)
{
	CRect rectEdit;
	CRect rect = GetClientRect();
	//rect.left = 60;
#ifdef __CHAT_V19
	rect.top = 8;
	rect.left = 8;
#else
	rect.top = 0;
	rect.left = 0;
#endif //__CHAT_V19
	rect.right -= 0;
	rect.bottom -= 0;

	m_wndText.SetWndRect( rect );
	m_wndText.m_wndScrollBar.SetMaxScrollPos();
	
	CWndNeuz::OnSize(nType,cx,cy);
}
void CWndChatLog::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndChatLog::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( g_WndMng.m_pWndWorld )
	{
		CRect rect = GetWindowRect( TRUE );
		CPoint p;
		p.x = point.x + rect.left;
		p.y = point.y + rect.top;
		g_WndMng.m_pWndWorld->m_bLButtonDown = TRUE;
		g_WndMng.m_pWndWorld->OnLButtonDown( nFlags, p );
	}
}

void CWndChatLog::OnRButtonDown(UINT nFlags, CPoint point)
{
	CRect rect = GetWindowRect( TRUE );
	CRect menurect = m_wndMenuPlace.GetWindowRect();

	rect.top  += point.y;
	rect.left += point.x;
	
	m_wndMenuPlace.Move( CPoint( rect.left, rect.top ) );
	
	m_wndMenuPlace.SetVisible( !m_wndMenuPlace.IsVisible() );
	m_wndMenuPlace.SetFocus();	
}	
void CWndChatLog::OnRButtonUp(UINT nFlags, CPoint point)
{
}
void CWndChatLog::OnMouseMove(UINT nFlags, CPoint point)
{
}
/*
BOOL CWndChatLog::OnEraseBkgnd(C2DRender* p2DRender)
{
	return TRUE;
}
*/

void CWndChatLog::AdditionalSkinTexture( LPWORD pDest, CSize size, D3DFORMAT d3dFormat )
{
	CWndNeuz::AdditionalSkinTexture( pDest, size, d3dFormat );
}

void CWndChatLog::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	AdjustMinRect( &rectWnd, 16 * 17, 16 * 3 );
	CRect rectOld = m_rectClient;
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect( 4, 4, 6, 6 );
	m_rectLayout = m_rectClient;
	if( bOnSize && ( rectOld.Width() != m_rectClient.Width() || rectOld.Height() != m_rectClient.Height() ) )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
	MakeVertexBuffer();
}

#endif //__Y_CHAT_SYSTEM_8














