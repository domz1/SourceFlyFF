#include "stdafx.h"
#include "definetext.h"
#include "defineSound.h"
#include "AppDefine.h"
#include "WndDialog.h"
#include "WndQuest.h"

#ifdef __CLIENT
#include "DPClient.h"
extern	CDPClient	g_DPlay;
#endif




/****************************************************
  WndId : APP_DIALOG - Dialog
****************************************************/

CWndDialog::CWndDialog() 
{ 
	m_nWordButtonNum = 0;
	m_nKeyButtonNum = 0;
	m_nContextButtonNum = 0;
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	m_nNewQuestListNumber = 0;
	m_nCurrentQuestListNumber = 0;
	m_nSelectKeyButton = -1;
	m_pNewQuestListIconTexture = NULL;
	m_pExpectedQuestListIconTexture = NULL;
	m_pCurrentQuestListIconTexture = NULL;
	m_pCompleteQuestListIconTexture = NULL;
#endif // __IMPROVE_QUEST_INTERFACE
	m_bWordButtonEnable = FALSE;
	m_dwQuest = 0;
	ZeroMemory( m_apWndAnswer, sizeof( m_apWndAnswer ) );
} 
CWndDialog::~CWndDialog() 
{ 
	for( int i = 0; i < m_strArray.GetSize(); i++ )
	{
		CEditString* pEditString = (CEditString*) m_strArray.GetAt( i );
		safe_delete( pEditString );
	}
	for( i = 0; i < 6; i++ )
		SAFE_DELETE( m_apWndAnswer[ i ] )
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	CWndQuest* pWndQuest = (CWndQuest*)g_WndMng.GetWndBase( APP_QUEST_EX_LIST );
#else // __IMPROVE_QUEST_INTERFACE
	CWndQuest* pWndQuest = (CWndQuest*)g_WndMng.GetWndBase( APP_QUEST );
#endif // __IMPROVE_QUEST_INTERFACE
	if( pWndQuest ) pWndQuest->Update();
} 
BOOL CWndDialog::OnSetCursor( CWndBase* pWndBase, UINT nHitTest, UINT message )
{
	return TRUE;
}
BOOL CWndDialog::Process() 
{
	CMover* pMover = prj.GetMover( m_idMover );
	if( pMover == NULL ) Destroy();
	return TRUE;
}
void CWndDialog::OnDraw( C2DRender* p2DRender ) 
{ 
	CWndButton* pEnter = (CWndButton*)GetDlgItem( WIDC_BUTTON1 );
	DWORD dwMaxHeight = m_pFont->GetMaxHeight() + 6;
	
	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
	POINT pt = lpWndCtrl->rect.TopLeft();

	int i;
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	if( m_bSay == TRUE )
		p2DRender->TextOut_EditString( pt.x, pt.y, m_string, 0, 0, 6 );
	else if( strcmp( m_aKeyButton[ m_nSelectKeyButton ].szWord, prj.GetText( TID_GAME_NEW_QUEST ) ) == 0 && m_nNewQuestListNumber == 0 )
		p2DRender->TextOut( pt.x + 10, pt.y + 10, prj.GetText( TID_GAME_EMPTY_NEW_QUEST ), D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
	else if( strcmp( m_aKeyButton[ m_nSelectKeyButton ].szWord, prj.GetText( TID_GAME_CURRENT_QUEST ) ) == 0 && m_nCurrentQuestListNumber == 0 )
		p2DRender->TextOut( pt.x + 10, pt.y + 10, prj.GetText( TID_GAME_EMPTY_CURRENT_QUEST ), D3DCOLOR_ARGB( 255, 255, 255, 255 ) );//adeilson
#else // __IMPROVE_QUEST_INTERFACE
	p2DRender->TextOut_EditString( pt.x, pt.y, m_string, 0, 0, 6 );//f000000 );
#endif // __IMPROVE_QUEST_INTERFACE
	if( m_bWordButtonEnable )
	{
		for( i = 0; i < m_nWordButtonNum; i++ )
		{
			DWORD dwColor = 0xffffffff;
			WORDBUTTON* pKeyButton = &m_aWordButton[ i ];
			if( strcmp( pKeyButton->szWord, "__YES__" ) == 0 )
				continue;
			if( strcmp( pKeyButton->szWord, "__NO__" ) == 0 )
				continue;
			if( strcmp( pKeyButton->szWord, "__CANCEL__" ) == 0 )
				continue;
			if( strcmp( pKeyButton->szWord, "__OK__" ) == 0 )
				continue;
			if( pKeyButton->bStatus )
			{
				dwColor = 0xffffffff;
				CSize size = p2DRender->m_pFont->GetTextExtent( pKeyButton->szWord );
				p2DRender->RenderLine( CPoint( pKeyButton->rect.left, pKeyButton->rect.top + size.cy ),
					CPoint( pKeyButton->rect.left + size.cx, pKeyButton->rect.top + size.cy ), dwColor );
				
			}
			p2DRender->TextOut( pKeyButton->rect.left, pKeyButton->rect.top,  pKeyButton->szWord, dwColor );
			p2DRender->TextOut( pKeyButton->rect.left + 1, pKeyButton->rect.top,  pKeyButton->szWord, dwColor );
		}
	}
	for( i = 0; i < m_nKeyButtonNum; i++ )
	{
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
		DWORD dwColor = ( i == m_nSelectKeyButton ) ? D3DCOLOR_ARGB( 255, 0, 255, 255 ) : 0xffffffff;
#else // __IMPROVE_QUEST_INTERFACE
		DWORD dwColor = 0xffffffff;
#endif // __IMPROVE_QUEST_INTERFACE
		WORDBUTTON* pKeyButton = &m_aKeyButton[ i ];
		if( pKeyButton->bStatus )
		{
			dwColor = 0xffffffff;
			CSize size = p2DRender->m_pFont->GetTextExtent( pKeyButton->szWord );
			p2DRender->RenderLine( CPoint( pKeyButton->rect.left, pKeyButton->rect.top + size.cy ),
				CPoint( pKeyButton->rect.left + size.cx, pKeyButton->rect.top + size.cy ), dwColor );
	
		}
		p2DRender->TextOut( pKeyButton->rect.left, pKeyButton->rect.top,  pKeyButton->szWord, dwColor );
		p2DRender->TextOut( pKeyButton->rect.left + 1, pKeyButton->rect.top,  pKeyButton->szWord, dwColor );
	}
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	if( m_WndNewQuestListBox.IsVisible() == TRUE )
		RenderNewQuestListIcon( p2DRender );
	else if( m_WndCurrentQuestListBox.IsVisible() == TRUE )
		RenderCurrentQuestListIcon( p2DRender );
#endif // __IMPROVE_QUEST_INTERFACE
	for( i = 0; i < m_nContextButtonNum; i++ )
	{
		DWORD dwColor = 0xffffffff;//adeilson 0xff101010
		WORDBUTTON* pKeyButton = &m_aContextButton[ i ];

		if( pKeyButton->bStatus )
		{
			dwColor = 0xff1010ff;
			CSize size = p2DRender->m_pFont->GetTextExtent( pKeyButton->szWord );
			p2DRender->RenderLine( CPoint( pKeyButton->rect.left, pKeyButton->rect.top + size.cy ),
				CPoint( pKeyButton->rect.left + size.cx, pKeyButton->rect.top + size.cy ), dwColor );
			p2DRender->TextOut( pKeyButton->rect.left, pKeyButton->rect.top,  pKeyButton->szWord, dwColor );
			p2DRender->TextOut( pKeyButton->rect.left + 1, pKeyButton->rect.top,  pKeyButton->szWord, dwColor );
			if( pKeyButton->nLinkIndex >= 0 )
			{
				WORDBUTTON* pKeyButton2 = &m_aContextButton[ pKeyButton->nLinkIndex ];
				CSize size = p2DRender->m_pFont->GetTextExtent( pKeyButton2->szWord );
				p2DRender->RenderLine( CPoint( pKeyButton2->rect.left, pKeyButton2->rect.top + size.cy ),
					CPoint( pKeyButton2->rect.left + size.cx, pKeyButton2->rect.top + size.cy ), dwColor );
				p2DRender->TextOut( pKeyButton2->rect.left, pKeyButton2->rect.top,  pKeyButton2->szWord, dwColor );
				p2DRender->TextOut( pKeyButton2->rect.left + 1, pKeyButton2->rect.top,  pKeyButton2->szWord, dwColor );
			}
		}
	}
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM3 );
	m_texChar.Render( p2DRender, lpWndCtrl->rect.TopLeft() );
} 
void CWndDialog::OnMouseWndSurface( CPoint point )
{
	for( int i = 0; i < m_nWordButtonNum; i++ )
		m_aWordButton[ i ].bStatus = FALSE;
	for( i = 0; i < m_nKeyButtonNum; i++ )
		m_aKeyButton[ i ].bStatus = FALSE;
	for( i = 0; i < m_nContextButtonNum; i++ )
		m_aContextButton[ i ].bStatus = FALSE;
	if( m_bWordButtonEnable )
	{
		for( i = 0; i < m_nWordButtonNum; i++ )
		{
			if( m_aWordButton[ i ].rect.PtInRect( point ) )
			{
				SetMouseCursor( CUR_SELECT );
			 	m_aWordButton[ i ].bStatus = TRUE;
				return;
			}
		}
	}
	for( i = 0; i < m_nKeyButtonNum; i++ )
	{
		WORDBUTTON* pKeyButton = &m_aKeyButton[ i ];
		if( pKeyButton->rect.PtInRect( point ) )
		{
			SetMouseCursor( CUR_SELECT );
			pKeyButton->bStatus = TRUE;
			CString string;// = prj.GetWordToolTip( pKeyButton->text );
			string.Format( prj.GetText(TID_GAME_ABOUTQUESTION), pKeyButton->szWord );
			if( string.IsEmpty() == FALSE )
			{
				CRect rect = pKeyButton->rect;
				ClientToScreen( &rect );

				CPoint point2 = point;
				ClientToScreen( &point2 );
				rect.InflateRect( 2, 2);
				int nLang	= GetLanguage();
				switch( nLang )
				{
					case LANG_GER:
					case LANG_FRE:
						break;
					default:
						g_toolTip.PutToolTip( (DWORD)pKeyButton, string, &rect, point2 );
						break;
				}
			}
			return;
		}
	}
	CString strWord, strOriginal;
	for( i = 0; i < m_nContextButtonNum; i++ )
	{
		DWORD dwColor = 0xffffffff;//adeilson 0xff101010
		WORDBUTTON* pKeyButton = &m_aContextButton[ i ];
		if( pKeyButton->rect.PtInRect( point ) )
		{
			SetMouseCursor( CUR_HELP );
			pKeyButton->bStatus = TRUE;
			CRect rect = pKeyButton->rect;
			ClientToScreen( &rect );

			CPoint point2 = point;
			ClientToScreen( &point2 );
			rect.InflateRect( 2, 2);

			return;
		}
	}
	SetMouseCursor( CUR_BASE );
}

void CWndDialog::RunScript( const char* szKey, DWORD dwParam, DWORD dwQuest )
{
	CMover* pMover = prj.GetMover( m_idMover );
	if( pMover )
		g_DPlay.SendScriptDialogReq( pMover->GetId(), szKey, dwParam, dwQuest, pMover->GetId(), g_pPlayer->GetId() );
	m_timer.Set( SEC( 1 ) );
}

void CWndDialog::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	CWndGroupBox* pWndGroupBox = ( CWndGroupBox* )GetDlgItem( WIDC_GROUP_BOX_TITLE );
	if( pWndGroupBox )
	{
		pWndGroupBox->m_dwColor = D3DCOLOR_ARGB( 255, 246, 204, 77 );
		pWndGroupBox->SetTitle( prj.GetText( TID_GAME_DIALOG ) );
	}

	m_pNewQuestListIconTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, _T( "QuestUiPaperGreen.tga" )), 0xffffffff );
	m_pExpectedQuestListIconTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, _T( "QuestUiPaperRed.tga" )), 0xffffffff );
	m_pCurrentQuestListIconTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, _T( "QuestUiPaperGray.tga" )), 0xffffffff );
	m_pCompleteQuestListIconTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, _T( "QuestUiPaperYellow.tga" )), 0xffffffff );

	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
	lpWndCtrl->dwWndStyle |= WBS_VSCROLL;

	static const int QUEST_LIST_SELECT_COLOR = 0xffff0000;
	static const int QUEST_LIST_LINE_SPACE = 3;

	m_WndNewQuestListBox.Create( lpWndCtrl->dwWndStyle, lpWndCtrl->rect, this, lpWndCtrl->dwWndId );
	m_WndNewQuestListBox.m_nSelectColor = QUEST_LIST_SELECT_COLOR;
	m_WndNewQuestListBox.m_nLineSpace = QUEST_LIST_LINE_SPACE;
	if( m_pNewQuestListIconTexture )
		m_WndNewQuestListBox.SetLeftMargin( m_pNewQuestListIconTexture->m_size.cx );
	m_WndNewQuestListBox.SetVisible( FALSE );

	m_WndCurrentQuestListBox.Create( lpWndCtrl->dwWndStyle, lpWndCtrl->rect, this, lpWndCtrl->dwWndId );
	m_WndCurrentQuestListBox.m_nSelectColor = QUEST_LIST_SELECT_COLOR;
	m_WndCurrentQuestListBox.m_nLineSpace = QUEST_LIST_LINE_SPACE;
	if( m_pCurrentQuestListIconTexture )
		m_WndCurrentQuestListBox.SetLeftMargin( m_pCurrentQuestListIconTexture->m_size.cx );
	m_WndCurrentQuestListBox.SetVisible( FALSE );
#endif // __IMPROVE_QUEST_INTERFACEf
	
	CWndBase* pWndBase = (CWndBase*)g_WndMng.GetApplet( APP_INVENTORY );

	if( pWndBase )
	{
		pWndBase->Destroy();
	}
	
	CMover* pMover = prj.GetMover( m_idMover );
	if( pMover == NULL ) return;
	LPCHARACTER lpCharacter = pMover->GetCharacter();

	if( lpCharacter )
	{
		m_texChar.DeleteDeviceObjects();
		m_texChar.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( "char\\",lpCharacter->m_szChar ), 0xffff00ff, TRUE );
		if( lpCharacter->m_dwMusicId )
			PlayMusic( lpCharacter->m_dwMusicId, 1 );
	}
	m_nCurArray = 0;
	for( int i = 0; i < m_strArray.GetSize(); i++ )
	{
		CEditString* pEditString = (CEditString*) m_strArray.GetAt( i );
		safe_delete( pEditString );
	}
	m_strArray.RemoveAll();


	MakeKeyButton();
	
	m_bSay = FALSE;

	UpdateButtonEnable();
	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rect = GetWindowRect();
	int nWidth  = rect.Width(); 
	int nHeight = rect.Height(); 
	int x = rectRoot.left + (rectRoot.Width()  / 2) - (nWidth  / 2);
	int y = rectRoot.top  + (rectRoot.Height() / 2) - (nHeight / 2);	
	CPoint point( x, 10 );
	Move( point );

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	CWndQuest* pWndQuest = (CWndQuest*)g_WndMng.GetWndBase( APP_QUEST_EX_LIST );
#else // __IMPROVE_QUEST_INTERFACE
	CWndQuest* pWndQuest = (CWndQuest*)g_WndMng.GetWndBase( APP_QUEST );
#endif // __IMPROVE_QUEST_INTERFACE
	if( pWndQuest ) pWndQuest->Update();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndDialog::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_DIALOG_EX, WBS_MODAL, CPoint( 0, 0 ), pWndParent );
#else // __IMPROVE_QUEST_INTERFACE
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_DIALOG, WBS_MODAL, CPoint( 0, 0 ), pWndParent );
#endif // __IMPROVE_QUEST_INTERFACE
} 

BOOL CWndDialog::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndDialog::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndDialog::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
	if( m_timer.IsTimeOut() == FALSE )
		return;

	PLAYSND( SND_INF_CLICK );
	if( m_bWordButtonEnable )
	{
		for( int i = 0; i < m_nWordButtonNum; i++ )
		{
			if( m_aWordButton[ i ].rect.PtInRect( point ) )
			{			
				BeginText();
				RunScript( m_aWordButton[i].szKey, m_aWordButton[i].dwParam, m_aWordButton[i].dwParam2 );
				MakeKeyButton();
				UpdateButtonEnable();
			}
		}
	}
	for( int i = 0; i < m_nKeyButtonNum; i++ )
	{
		if( m_aKeyButton[ i ].rect.PtInRect( point ) )
		{
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
			m_nSelectKeyButton = i;

			CWndGroupBox* pWndGroupBox = ( CWndGroupBox* )GetDlgItem( WIDC_GROUP_BOX_TITLE );
			if( pWndGroupBox )
				pWndGroupBox->SetTitle( m_aKeyButton[ m_nSelectKeyButton ].szWord );

			m_bSay = FALSE;
			BeginText();
 			LPWNDCTRL lpWndCustom1 = GetWndCtrl( WIDC_CUSTOM1 );
			if( strcmp( m_aKeyButton[ i ].szWord, prj.GetText( TID_GAME_NEW_QUEST ) ) == 0 && m_nNewQuestListNumber > 0 )
			{
				m_WndNewQuestListBox.SetVisible( TRUE );
				m_WndNewQuestListBox.SetFocus();
				m_WndCurrentQuestListBox.SetVisible( FALSE );
			}
			else if( strcmp( m_aKeyButton[ i ].szWord, prj.GetText( TID_GAME_CURRENT_QUEST ) ) == 0 && m_nCurrentQuestListNumber > 0 )
			{
				m_WndCurrentQuestListBox.SetVisible( TRUE );
				m_WndCurrentQuestListBox.SetFocus();
				m_WndNewQuestListBox.SetVisible( FALSE );
			}
			else
			{
				m_WndNewQuestListBox.SetVisible( FALSE );
				m_WndCurrentQuestListBox.SetVisible( FALSE );
				RunScript( m_aKeyButton[i].szKey, m_aKeyButton[i].dwParam, m_aKeyButton[i].dwParam2 );
			}
			MakeKeyButton();
			UpdateButtonEnable();
#else // __IMPROVE_QUEST_INTERFACE
			BeginText();
			RunScript( m_aKeyButton[i].szKey, m_aKeyButton[i].dwParam, m_aKeyButton[i].dwParam2 );
			MakeKeyButton();
			UpdateButtonEnable();
#endif // __IMPROVE_QUEST_INTERFACE
		}
	}
	for( i = 0; i < m_nContextButtonNum; i++ )
	{
		WORDBUTTON* pContextButton = &m_aContextButton[ i ];
		if( pContextButton->rect.PtInRect( point ) )
		{
			CString strKey = pContextButton->szKey;
			BeginText();
			RunScript( strKey, 0, 0 );
		}
	}
} 
void CWndDialog::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
void CWndDialog::Say( LPCTSTR lpszString, DWORD dwQuest )
{
 	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( dwQuest );
 	CString string = "  ";
	if( dwQuest )
	{
		string = "#b#cff5050f0";
		string += pQuestProp->m_szTitle;
		string += "#nb#nc\n";
		string += "  ";
		string += lpszString;
	}
	else
		string += lpszString;

	int nLength = string.GetLength();//_tcslen( szString );
	for( int i = 0, j = 0; i < nLength; i++ )
	{
		if( string[ i ] == '\\' && string[ i + 1 ] == 'n' )
		{
			// 중간의 두개의 코드를 \n와 ' '로 대체.
			string.SetAt( i, '\n' );
			string.SetAt( i + 1, ' ' );
			// 그리고 스페이스 한개 삽입.
			string.Insert( i + 1, " " );
		}
	}
	m_dwQuest = dwQuest;
	ParsingString( string );
	EndSay();
}

void CWndDialog::ParsingString( LPCTSTR lpszString )
{
	CString strWord, strOriginal;
	int nMarkBraceIdx;
	BOOL bKey = FALSE;
	BOOL bBrace = FALSE;
	int nTemp  =0;
	BOOL bWirdChar = FALSE;

	CMover* pMover = prj.GetMover( m_idMover );
	if( pMover == NULL ) return;

	int nPos =  m_strArray.GetSize();
	m_aContextMark[ nPos ].RemoveAll();

	CEditString editString;
	editString.AddParsingString( lpszString );
	CEditString* pEditString = new CEditString;

	int nLength = editString.GetLength();
	
	// 왼쪽이 strWord, 오른쪽이 strOrigital
	for( int i = 0; i < nLength; i++ )
	{
		if( editString[ i ] == '['  )
		{
			nMarkBraceIdx = i;
			bBrace = TRUE;
			if( bKey == FALSE )
			{
				strOriginal.Empty();
				strWord.Empty();
				nTemp = 1;
			}
		}
		else
		if( editString[ i ] == ']' )
		{
			if( bBrace == TRUE )
			{
				bBrace = FALSE;
				if( bKey == FALSE )
				{
					bKey = TRUE;
					int nLen = strWord.GetLength();
					int nMark = 3;
					if( nLen == 1 ) 
						nMark = 4;
					if( nLen == 2 && bWirdChar )
						nMark = 4;
					m_aContextMark[ nPos ].SetAt( m_aContextMark[ nPos ].GetSize() - 1, nMark );
					if( bWirdChar )
						m_aContextMark[ nPos ].SetAt( m_aContextMark[ nPos ].GetSize() - 2, nMark );
				}
				else
				{
					bKey = FALSE;
					if( strOriginal.IsEmpty() )
						strOriginal = strWord;
					m_mapWordToOriginal.SetAt( strWord, strOriginal );
				}
			}
		}
		else
		{
			if( bKey == TRUE )
			{
				if( bBrace == FALSE )
				{
					LPCHARACTER lpCharacter = pMover->GetCharacter();
					CString string;
					string.Format( "%s에서 [%s]와 [] 사이에 잡문자가 들어있거나 연결이 안된다.", lpCharacter->m_szDialog, strWord );
					AfxMessageBox( string );
				}
				strOriginal += editString[ i ];
				if( IsDBCSLeadByte( editString[ i ] ) )
					strOriginal += editString[ ++i ];
			}
			else
			{
				int nMarkTemp = 0;
				DWORD dwStyle = editString.m_abyStyle.GetAt( i );
				DWORD dwColor = editString.m_adwColor.GetAt( i );
				if( bBrace )
				{
					nMarkTemp = nTemp;
					if( nTemp == 1 ) nTemp++;
					dwStyle = ESSTY_BOLD;
				}
				m_aContextMark[ nPos ].Add( nMarkTemp );
				pEditString->AddString( editString[ i ], dwColor, dwStyle );

				strWord += editString[ i ];
				bWirdChar = FALSE;
				if( IsDBCSLeadByte( editString[ i ] ) )
				{
					m_aContextMark[ nPos ].Add( nMarkTemp );
					pEditString->AddString( editString[ ++i ], 0xffffffff, dwStyle );//adeilson 0xff000000

					strWord += editString[ i ];
					bWirdChar = TRUE;
				}
			}
		}
	}
	pEditString->Align( pEditString->m_pFont, 0 );
	m_strArray.Add( pEditString );
}
void CWndDialog::EndSay()
{
	if( m_strArray.GetSize() )
	{
		LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
		m_string.Init( m_pFont, &lpWndCtrl->rect );
		m_string.SetEditString( *(CEditString*)m_strArray.GetAt( 0 ) );//, 0xff000000 );
		m_nCurArray = 0;
	}
	MakeKeyButton();
	MakeContextButton();
	MakeAnswerButton();
	UpdateButtonEnable();

}
void CWndDialog::BeginText()
{
	for( int i = 0; i < m_strArray.GetSize(); i++ )
	{
		CEditString* pEditString = (CEditString*) m_strArray.GetAt( i );
		safe_delete( pEditString );
	}
	m_strArray.RemoveAll();
	m_mapWordToOriginal.RemoveAll();
	m_nWordButtonNum = 0;
	for( i = 0; i < 6; i++ )
	{
		if( m_apWndAnswer[ i ] )
			m_apWndAnswer[ i ]->Destroy( TRUE );
		m_apWndAnswer[ i ] = NULL;
	}
}
void CWndDialog::MakeContextButton()
{
	DWORD dwLineCount = m_string.GetLineCount();
	DWORD dwMaxHeight = m_pFont->GetMaxHeight() + 6;
	m_nContextButtonNum = 0;
	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
	int x, y = lpWndCtrl->rect.top;
	DWORD dwCurOffset;
	TCHAR strHan[ 3 ];
	CRect rectWord(0, 0, 0, 0);
	DWORD dwMark = 0;
	CString strKey, strWord;
	BOOL bLinkWord = FALSE;
	for( int i = 0; i < (int)( dwLineCount ); i++)
	{
		CString string = m_string.GetLine( i );
		DWORD dwOffset = m_string.GetLineOffset( i );
		LPCTSTR lpszStr = string;
		int nLength = string.GetLength();
		x = lpWndCtrl->rect.left;
		strWord.Empty();
		
		for( int j = 0; j < nLength; j++ )
		{
			if( IsDBCSLeadByte( string[ j ] ) )
			{
				strHan[ 0 ] = string[ j++ ];
				strHan[ 1 ] = string[ j ];
				strHan[ 2 ] = '\0';
				dwCurOffset = dwOffset + ( j - 1 );
			}
			else
			{
				strHan[ 0 ] = string[ j ];
				strHan[ 1 ] = '\0';
				dwCurOffset = dwOffset + j;
			}
			CSize size = m_pFont->GetTextExtent( strHan );
			dwMark = m_aContextMark[ m_nCurArray ].GetAt( dwCurOffset );

			if( dwMark )
			{
				strWord += strHan;
				rectWord.right += size.cx;
			}
			if( dwMark == 1 || dwMark == 4 ) 
			{
				rectWord.SetRect( x, y, x + size.cx, y + dwMaxHeight );
				if( dwMark == 4 ) dwMark = 3;
			}
			if( dwMark == 3 ) 
			{
				m_aContextButton[ m_nContextButtonNum ].rect = rectWord;
				m_aContextButton[ m_nContextButtonNum ].bStatus = FALSE;
				m_aContextButton[ m_nContextButtonNum ].nLinkIndex = -1;
				m_aContextButton[ m_nContextButtonNum ].dwParam2 = m_dwQuest;
				CString strTemp = strWord;
				if( bLinkWord )
				{
					m_aContextButton[ m_nContextButtonNum ].nLinkIndex = m_nContextButtonNum - 1;
					strWord = m_aContextButton[ m_nContextButtonNum - 1 ].szWord;
					strWord += strTemp;
				}
				CString strKey;
				m_mapWordToOriginal.Lookup( strWord, strKey );
				strcpy( m_aContextButton[ m_nContextButtonNum ].szWord, strTemp );
				strcpy( m_aContextButton[ m_nContextButtonNum ].szKey, strKey );
				if( bLinkWord )
					strcpy( m_aContextButton[ m_nContextButtonNum - 1 ].szKey, strKey );
				m_nContextButtonNum++;
				bLinkWord = FALSE;
				strWord.Empty();
			}
			x += size.cx;
		}	
		y += dwMaxHeight;
		
		// 중간에 끊어진 경우 
		if( dwMark == 1 || dwMark == 2 ) //bKeyButton == TRUE )
		{
			//bKeyButton = FALSE;
			bLinkWord = TRUE;
			m_aContextButton[ m_nContextButtonNum ].rect = rectWord;
			m_aContextButton[ m_nContextButtonNum ].bStatus = FALSE;
			m_aContextButton[ m_nContextButtonNum ].nLinkIndex = m_nContextButtonNum + 1;
			m_aContextButton[ m_nContextButtonNum ].dwParam2 = m_dwQuest;
			m_mapWordToOriginal.Lookup( strWord, strKey );
			strcpy( m_aContextButton[ m_nContextButtonNum ].szWord, strWord );
			strcpy( m_aContextButton[ m_nContextButtonNum ].szKey, strKey );
			rectWord.SetRect( lpWndCtrl->rect.left, y, lpWndCtrl->rect.left, y + dwMaxHeight );
			m_nContextButtonNum++;
		}
	}
}
void CWndDialog::AddKeyButton( LPCTSTR lpszWord, LPCTSTR lpszKey, DWORD dwParam, DWORD dwQuest )
{
	WORDBUTTON* lpKeyButton;// = &m_aKeyButton[ m_nKeyButtonNum ];
	// 같은 워드가 발견되면 무시한다. 키는 같아도 되지만 워드는 같으면 하나는 무시함.
	for( int i = 0; i < m_nKeyButtonNum; i++ )
	{
		lpKeyButton = &m_aKeyButton[ i ];
		if( strcmp( lpKeyButton->szWord, lpszWord ) == 0 )
			return; 
		
	}
	lpKeyButton = &m_aKeyButton[ m_nKeyButtonNum ];
	strcpy( lpKeyButton->szWord, lpszWord );
	strcpy( lpKeyButton->szKey, lpszKey );
	lpKeyButton->bStatus = FALSE;
	//lpWordButton->rect
	lpKeyButton->dwParam = dwParam;
	lpKeyButton->dwParam2 = dwQuest;
	m_nKeyButtonNum++;
	EndSay();
}
void CWndDialog::RemoveAllKeyButton()
{
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	m_WndNewQuestListBox.ResetContent();
	m_nNewQuestListNumber = 0;
	m_WndCurrentQuestListBox.ResetContent();
	m_nCurrentQuestListNumber = 0;
#endif // __IMPROVE_QUEST_INTERFACE
	m_nKeyButtonNum = 0;
	EndSay();
}
void CWndDialog::RemoveKeyButton( LPCTSTR lpszKey )
{
	WORDBUTTON* lpKeyButton;// = &m_aKeyButton[ m_nKeyButtonNum ];
	for( int i = 0; i < m_nKeyButtonNum; i++ )
	{
		lpKeyButton = &m_aKeyButton[ i ];
		//if( strcmp( lpKeyButton->szWord, lpszWord ) == 0 )
		if( strcmp( lpKeyButton->szKey, lpszKey ) == 0 )
		{
			for( int j = i; j < m_nKeyButtonNum - 1; j++ )
			{
				memcpy( &m_aKeyButton[ j ], &m_aKeyButton[ j + 1 ], sizeof( WORDBUTTON ) ) ;
			}
			m_nKeyButtonNum--;
			EndSay();
			return;
		}
	}
	EndSay();
}
// 하얀색 대화영역에 나오는 버튼 
void CWndDialog::AddAnswerButton( LPCTSTR lpszWord, LPCTSTR lpszKey, DWORD dwParam, DWORD dwQuest )
{
	WORDBUTTON* lpWordButton = &m_aWordButton[ m_nWordButtonNum ];
	strcpy( lpWordButton->szWord, lpszWord );
	strcpy( lpWordButton->szKey, lpszKey );
	lpWordButton->bStatus = FALSE;
	//lpWordButton->rect
	lpWordButton->dwParam  = dwParam;
	lpWordButton->dwParam2 = dwQuest;
	m_nWordButtonNum++;
	EndSay();
}

void CWndDialog::MakeKeyButton()
{
	DWORD dwMaxHeight = m_pFont->GetMaxHeight() + 6;
	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );
	CRect rectClient = GetClientRect();
	int x = 0, y = 0;
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	BOOL bQuestButtonLinefeedSwitch = FALSE;
#endif // __IMPROVE_QUEST_INTERFACE
	for( int i = 0; i < m_nKeyButtonNum; i++ )
	{
		WORDBUTTON* pWndButton = &m_aKeyButton[ i ];

		CSize size = m_pFont->GetTextExtent( pWndButton->szWord );
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
		size.cx += 16;
#else // __IMPROVE_QUEST_INTERFACE
		size.cx += 20;//10 * 6;
#endif // __IMPROVE_QUEST_INTERFACE
		//size.cy += 4;
		pWndButton->bStatus = FALSE;
		
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
		pWndButton->rect = CRect( lpWndCtrl->rect.left + x, lpWndCtrl->rect.top + y * dwMaxHeight, 
			lpWndCtrl->rect.left + x + size.cx, lpWndCtrl->rect.top + y * dwMaxHeight + size.cy );
#else // __IMPROVE_QUEST_INTERFACE
		pWndButton->rect = CRect( lpWndCtrl->rect.left + x, lpWndCtrl->rect.top + y * dwMaxHeight, 
			lpWndCtrl->rect.left + x + size.cx, lpWndCtrl->rect.top + y * size.cy + dwMaxHeight );
#endif // __IMPROVE_QUEST_INTERFACE
		if( pWndButton->rect.right > lpWndCtrl->rect.right )
		{
			x = 0, y++;
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
			pWndButton->rect = CRect( lpWndCtrl->rect.left + x * size.cx, lpWndCtrl->rect.top + y * dwMaxHeight, 
				lpWndCtrl->rect.left + ( x * size.cx ) + size.cx, lpWndCtrl->rect.top + y * dwMaxHeight + size.cy );
		}
		else if( ( strcmp( pWndButton->szWord, prj.GetText( TID_GAME_NEW_QUEST ) ) == 0 || strcmp( pWndButton->szWord, prj.GetText( TID_GAME_CURRENT_QUEST ) ) == 0 ) && 
			bQuestButtonLinefeedSwitch == FALSE )
		{
			x = 0, ++y;
			pWndButton->rect = CRect( lpWndCtrl->rect.left + x * size.cx, lpWndCtrl->rect.top + y * dwMaxHeight, 
				lpWndCtrl->rect.left + ( x * size.cx ) + size.cx, lpWndCtrl->rect.top + y * dwMaxHeight + size.cy );
			bQuestButtonLinefeedSwitch = TRUE;
#else // __IMPROVE_QUEST_INTERFACE
			pWndButton->rect = CRect( lpWndCtrl->rect.left + x * size.cx, lpWndCtrl->rect.top + y * dwMaxHeight, 
				lpWndCtrl->rect.left + x * size.cx + size.cx, lpWndCtrl->rect.top + y * size.cy + dwMaxHeight );
#endif // __IMPROVE_QUEST_INTERFACE
		}				
		x += size.cx;
	}
}
// 하얀색 대화영역에 나오는 버튼 
// AddAnswer로 추가 
void CWndDialog::MakeAnswerButton()
{
	m_bWordButtonEnable = FALSE;
	if( m_nCurArray == m_strArray.GetSize() - 1 )
	{
		DWORD dwMaxHeight = m_pFont->GetMaxHeight() + 6;
		LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
		
		int yBegin = lpWndCtrl->rect.top + m_string.GetLineCount() * dwMaxHeight + dwMaxHeight;
		int x = 0, y = 0;//
		int nButtonNum = 0;
		for( int i = 0; i < m_nWordButtonNum; i++ )
		{
			WORDBUTTON* pWordButton = &m_aWordButton[ i ];
			if( strcmp( pWordButton->szWord, "__YES__" ) == 0 )
			{ nButtonNum++; continue; }
			if( strcmp( pWordButton->szWord, "__NO__" ) == 0 )
			{ nButtonNum++; continue; }
			if( strcmp( pWordButton->szWord, "__CANCEL__" ) == 0 )
			{ nButtonNum++; continue; }
			if( strcmp( pWordButton->szWord, "__OK__" ) == 0 )
			{ nButtonNum++; continue; }

			CSize size = m_pFont->GetTextExtent( pWordButton->szWord );
			size.cx += 10;

			pWordButton->rect = CRect( lpWndCtrl->rect.left + x, yBegin + y * dwMaxHeight, 
				lpWndCtrl->rect.left + x + size.cx, yBegin + y * size.cy + dwMaxHeight );
			if( pWordButton->rect.right > lpWndCtrl->rect.right )
			{
				x = 0, y++;
				//pWordButton->rect = CRect( lpWndCtrl->rect.left + x * size.cx, lpWndCtrl->rect.top + y * dwMaxHeight, 
				//	lpWndCtrl->rect.left + x * size.cx + size.cx, lpWndCtrl->rect.top + y * size.cy + dwMaxHeight );

				pWordButton->rect = CRect( lpWndCtrl->rect.left + x, yBegin + y * dwMaxHeight, 
					lpWndCtrl->rect.left + x + size.cx, yBegin + y * size.cy + dwMaxHeight );

			}				
			x += size.cx;
		}
		x = lpWndCtrl->rect.left + ( lpWndCtrl->rect.Width() / 2 ) - ( ( 90 * nButtonNum ) / 2 );
		CRect rect;
		CString strTexture;
		int nWndId = 0;
		int j      = 0;
		for( i = 0; i < 6; i++ )
			SAFE_DELETE( m_apWndAnswer[ i ] )
		for( i = 0; i < m_nWordButtonNum; i++ )
		{
			WORDBUTTON* pWordButton = &m_aWordButton[ i ];
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
			rect = CRect( x, lpWndCtrl->rect.bottom - 20, x + 70, lpWndCtrl->rect.bottom + 10 );
#else // __IMPROVE_QUEST_INTERFACE
			rect = CRect( x, lpWndCtrl->rect.bottom - 10, x + 70, lpWndCtrl->rect.bottom + 10 );
#endif // __IMPROVE_QUEST_INTERFACE

            CRect Trect;
			Trect = CRect( x, lpWndCtrl->rect.bottom - 25, x + 70, lpWndCtrl->rect.bottom - 1 );
            LPCSTR lpszFileName;
			if( strcmp( pWordButton->szWord, "__YES__" ) == 0 )
			{  			
				nWndId =  WIDC_YES;
				lpszFileName = "Yes";
				strTexture = "ButtNormal00.tga";
				m_apWndAnswer[ j ] = new CWndAnswer;
				m_apWndAnswer[ j ]->Create( lpszFileName , WBS_CHILD, Trect, this, nWndId );
				m_apWndAnswer[ j ]->SetTexture( D3DDEVICE, MakePath( DIR_THEME, strTexture ), 1 );
				m_apWndAnswer[ j ]->FitTextureSize();
				m_apWndAnswer[ j ]->m_pWordButton = (WORDBUTTON*)	pWordButton;
				j++;
			}
			else
			if( strcmp( pWordButton->szWord, "__NO__" ) == 0 )
			{
				nWndId =  WIDC_NO;
				lpszFileName = "No";
				strTexture = "ButtNormal00.tga";
				m_apWndAnswer[ j ] = new CWndAnswer;
				m_apWndAnswer[ j ]->Create( lpszFileName, WBS_CHILD, Trect, this, nWndId );
				m_apWndAnswer[ j ]->SetTexture( D3DDEVICE, MakePath( DIR_THEME, strTexture ), 1 );
				m_apWndAnswer[ j ]->FitTextureSize();
				m_apWndAnswer[ j ]->m_pWordButton = (WORDBUTTON*)	pWordButton;
				j++;
			}
			else
			if( strcmp( pWordButton->szWord, "__CANCEL__" ) == 0 )
			{
				nWndId =  WIDC_CANCEL;
				lpszFileName = "Cancel";
				strTexture = "ButtNormal00.tga";
				m_apWndAnswer[ j ] = new CWndAnswer;
				m_apWndAnswer[ j ]->Create( lpszFileName, WBS_CHILD, Trect, this, nWndId );
				m_apWndAnswer[ j ]->SetTexture( D3DDEVICE, MakePath( DIR_THEME, strTexture ), 1 );
				m_apWndAnswer[ j ]->FitTextureSize();
				m_apWndAnswer[ j ]->m_pWordButton = (WORDBUTTON*)	pWordButton;
				j++;
			}
			else
			if( strcmp( pWordButton->szWord, "__OK__" ) == 0 )
			{
				nWndId =  WIDC_OK; 
				lpszFileName = "OK";
				strTexture = "ButtNormal00.tga";
				m_apWndAnswer[ j ] = new CWndAnswer;
				m_apWndAnswer[ j ]->Create( lpszFileName, WBS_CHILD, Trect, this, nWndId );
				m_apWndAnswer[ j ]->SetTexture( D3DDEVICE, MakePath( DIR_THEME, strTexture ), 1 );
				m_apWndAnswer[ j ]->FitTextureSize();
				m_apWndAnswer[ j ]->m_pWordButton = (WORDBUTTON*)	pWordButton;
				j++;
			}
			if(	nWndId )
			{
/*				m_apWndAnswer[ j ] = new CWndAnswer;
				m_apWndAnswer[ j ]->Create( "", WBS_CHILD, rect, this, nWndId );
				m_apWndAnswer[ j ]->SetTexture( D3DDEVICE, MakePath( DIR_THEME, strTexture ), 1 );
				m_apWndAnswer[ j ]->FitTextureSize();
				m_apWndAnswer[ j ]->m_pWordButton = (WORDBUTTON*)	pWordButton;
				j++;*/
			}
			x += 90;
		}

		m_bWordButtonEnable = TRUE;
	}
}

void CWndDialog::UpdateButtonEnable()
{
	CWndButton* pEnter = (CWndButton*)GetDlgItem( WIDC_BUTTON1 );
	if( m_strArray.GetSize() == 0 || m_strArray.GetSize() - 1 == m_nCurArray )
	{
		pEnter->SetVisible( FALSE );
	}
	else
	{
		pEnter->SetVisible( TRUE );
	}		
}


BOOL CWndDialog::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case WIDC_BUTTON1:
		if( m_nCurArray < m_strArray.GetSize() - 1 )
		{
			m_nCurArray++;
			LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
			m_string.Init( m_pFont, &lpWndCtrl->rect );
			m_string.SetEditString( *(CEditString*)m_strArray.GetAt( m_nCurArray ) );
			MakeContextButton();
			MakeAnswerButton();
		}
		UpdateButtonEnable();
		break;
	case WIDC_OK:
	case WIDC_YES:
	case WIDC_NO:
	case WIDC_CANCEL:
		{
			CWndAnswer* pWndAnswer = (CWndAnswer*)pLResult;
			WORDBUTTON* pWordButton = (WORDBUTTON*)pWndAnswer->m_pWordButton;
			BeginText();
			RunScript( pWordButton->szKey,  pWordButton->dwParam,  pWordButton->dwParam2 );
			MakeKeyButton();
			UpdateButtonEnable();
		}
		break;
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	case WIDC_CUSTOM1:
		{
			if( m_WndNewQuestListBox.IsVisible() == TRUE && m_WndCurrentQuestListBox.IsVisible() == FALSE )
			{
				for( int i = 0; i < m_nNewQuestListNumber; ++i )
				{
					CRect rect = m_WndNewQuestListBox.GetItemRect( i );
					CPoint point = m_WndNewQuestListBox.GetMousePoint();
					if( rect.PtInRect( point ) )
					{
						if( strcmp( m_WndNewQuestListBox.GetKeyString( i ), "QUEST_NEXT_LEVEL" ) != 0 )
						{
							m_bSay = FALSE;
							m_WndNewQuestListBox.SetVisible( FALSE );
							BeginText();
							RunScript( m_WndNewQuestListBox.GetKeyString( i ), m_WndNewQuestListBox.GetItemData( i ), m_WndNewQuestListBox.GetItemData2( i ) );
							MakeKeyButton();
							UpdateButtonEnable();
						}
					}
				}
			}
			else if( m_WndNewQuestListBox.IsVisible() == FALSE && m_WndCurrentQuestListBox.IsVisible() == TRUE )
			{
				for( int i = 0; i < m_nCurrentQuestListNumber; ++i )
				{
					CRect rect = m_WndCurrentQuestListBox.GetItemRect( i );
					CPoint point = m_WndCurrentQuestListBox.GetMousePoint();
					if( rect.PtInRect( point ) )
					{
						m_bSay = FALSE;
						m_WndCurrentQuestListBox.SetVisible( FALSE );
						BeginText();
						RunScript( m_WndCurrentQuestListBox.GetKeyString( i ), m_WndCurrentQuestListBox.GetItemData( i ), m_WndCurrentQuestListBox.GetItemData2( i ) );
						MakeKeyButton();
						UpdateButtonEnable();
					}
				}
			}
			break;
		}
#endif // __IMPROVE_QUEST_INTERFACE
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
void CWndDialog::AddNewQuestList( const LPCTSTR lpszWord, const LPCTSTR lpszKey, const DWORD dwParam, const DWORD dwQuest )
{
	MakeQuestKeyButton( prj.GetText( TID_GAME_NEW_QUEST ) );
	AddQuestList( m_WndNewQuestListBox, m_nNewQuestListNumber, lpszWord, lpszKey, dwParam, dwQuest );
}
void CWndDialog::AddCurrentQuestList( const LPCTSTR lpszWord, const LPCTSTR lpszKey, const DWORD dwParam, const DWORD dwQuest )
{
	MakeQuestKeyButton( prj.GetText( TID_GAME_CURRENT_QUEST ) );
	AddQuestList( m_WndCurrentQuestListBox, m_nCurrentQuestListNumber, lpszWord, lpszKey, dwParam, dwQuest );
}
void CWndDialog::MakeQuestKeyButton( const CString& rstrKeyButton )
{
	WORDBUTTON* lpKeyButton = NULL;
	for( int i = 0; i < m_nKeyButtonNum; i++ )
	{
		lpKeyButton = &m_aKeyButton[ i ];
		if( strcmp( lpKeyButton->szWord, rstrKeyButton ) == 0 )
			return;
	}
	lpKeyButton = &m_aKeyButton[ m_nKeyButtonNum ];
	strcpy( lpKeyButton->szWord, rstrKeyButton );
	strcpy( lpKeyButton->szKey, rstrKeyButton );
	lpKeyButton->bStatus = FALSE;
	lpKeyButton->dwParam = 0;
	lpKeyButton->dwParam2 = 0;
	++m_nKeyButtonNum;
	EndSay();
}
void CWndDialog::RenderNewQuestListIcon( C2DRender* p2DRender )
{
	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
	if( lpWndCtrl == NULL || m_pNewQuestListIconTexture == NULL || m_pExpectedQuestListIconTexture == NULL )
		return;
	POINT pt = lpWndCtrl->rect.TopLeft();
	for( int i = 0; i < m_WndNewQuestListBox.GetCount(); ++i )
	{
		CRect rectItem = m_WndNewQuestListBox.GetItemRect( i );
		int nRenderPositionX = pt.x + rectItem.left;
		int nRenderPositionY = pt.y + rectItem.top;
		if( ( nRenderPositionY >= lpWndCtrl->rect.top ) && ( nRenderPositionY + rectItem.Height() <= lpWndCtrl->rect.bottom ) )
		{
			if( m_WndNewQuestListBox.GetItemValidity( i ) == FALSE )
				p2DRender->RenderTexture( CPoint( nRenderPositionX, nRenderPositionY ), m_pExpectedQuestListIconTexture );
			else
				p2DRender->RenderTexture( CPoint( nRenderPositionX, nRenderPositionY ), m_pNewQuestListIconTexture );
		}
	}
}
void CWndDialog::RenderCurrentQuestListIcon( C2DRender* p2DRender )
{
	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
	if( lpWndCtrl == NULL || m_pCurrentQuestListIconTexture == NULL || m_pCompleteQuestListIconTexture == NULL )
		return;
	POINT pt = lpWndCtrl->rect.TopLeft();
	for( int i = 0; i < m_WndCurrentQuestListBox.GetCount(); ++i )
	{
		CRect rectItem = m_WndCurrentQuestListBox.GetItemRect( i );
		int nRenderPositionX = pt.x + rectItem.left;
		int nRenderPositionY = pt.y + rectItem.top;
		if( ( nRenderPositionY >= lpWndCtrl->rect.top ) && ( nRenderPositionY + rectItem.Height() <= lpWndCtrl->rect.bottom ) )
		{
			if( __IsEndQuestCondition( g_pPlayer->GetActiveMover(), m_WndCurrentQuestListBox.GetItemData2( i ) ) )
				p2DRender->RenderTexture( CPoint( nRenderPositionX, nRenderPositionY ), m_pCompleteQuestListIconTexture );
			else
				p2DRender->RenderTexture( CPoint( nRenderPositionX, nRenderPositionY ), m_pCurrentQuestListIconTexture );
		}
	}
}
void CWndDialog::AddQuestList( CWndListBox& pWndListBox, int& nQuestListNumber, const LPCTSTR lpszWord, const LPCTSTR lpszKey, const DWORD dwParam, const DWORD dwQuest )
{
	for( int i = 0; i < nQuestListNumber; ++i )
	{
		CString strQuestList = pWndListBox.GetString( i );
		if( strcmp( strQuestList, lpszWord ) == 0 )
			return;
	}
	CEditString strTitleWord = _T( "" );
	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( dwQuest );
	if( pQuestProp )
		strTitleWord.Format( _T( "[#b%d#nb~#b%d#nb ] #b%s#nb" ), pQuestProp->m_nBeginCondLevelMin, pQuestProp->m_nBeginCondLevelMax, lpszWord );
	pWndListBox.AddString( strTitleWord );
	if( strcmp( lpszKey, _T( "QUEST_NEXT_LEVEL" ) ) == 0 )
		pWndListBox.SetItemValidity( nQuestListNumber, FALSE );
	pWndListBox.SetKeyString( nQuestListNumber, lpszKey );
	pWndListBox.SetItemData( nQuestListNumber, dwParam );
	pWndListBox.SetItemData2( nQuestListNumber, dwQuest );
	++nQuestListNumber;
}
#endif // __IMPROVE_QUEST_INTERFACE