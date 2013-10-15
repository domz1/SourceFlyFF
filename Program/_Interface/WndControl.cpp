// WndBase.cpp: implementation of the CWndBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "defineSound.h" 
#include "defineText.h" 
#include "AppDefine.h" // 여기서 이걸 왜 include하지? 
#include "clipboard.h"
// 경고!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// 이 클레스는 기본 윈도 베이스 콘토롤이다. 이 콘트롤을 개조하기 위해 매인프레임의 펑션을 부른다거나
// 애플랫 단위에 접근을 시도하는 행위는 엄격히 금지함!!! 이미 몇부분 그러한 행각이 발생했으므로 
// 작성자는 수정하기 바람!!!! 기본 윈도 코딩에서 윈도의 콘트롤 박스나 리스트 박스가 맘에 안든다고
// API 자체를 수정하는 행위는 불가능하다. 윈도에서는 콜백, 훅. 오버라이드 같은 방식으로 기능에 변형을 가한다.
// 여기서는 클레스를 계승받아 오버라이드하여 재작성하는 방법이 적당하다.

#if __VER >= 15 // __GUILD_HOUSE
#include "GuildHouse.h"
#include "WndGuildHouse.h"
#endif

#define EDIT_HEIGHT 8
#define VSCROLL_WIDTH 16

#define GETRANGE ( ( GetClientRect().Height() + m_nLineSpace ) / GetFontHeight() )

CWndStatic::CWndStatic() 
{ 
	m_byWndType = WTYPE_STATIC; 
	m_dwColor = 0xffffffff;
	m_strTexture = DEF_CTRL_GROUP;
	m_bTile = TRUE;
	
}
void CWndStatic::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	//rectWnd.DeflateRect( 2, 2 );
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	
	if( bOnSize )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
}
BOOL CWndStatic::Create( LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID )	
{
	if( lpszCaption && strlen( lpszCaption ) ) m_strTitle = lpszCaption;
	if( !(dwStyle & WSS_GROUPBOX) )
	{
		m_strTexture = "";//WndEditTile200.tga";
		m_dwColor = D3DCOLOR_ARGB( 255, 246, 204, 77 );//Adeilson 0xff2e70a9;
	}
	return CWndBase::Create( dwStyle | WBS_CHILD | WBS_NOFRAME , rect,  pParentWnd, nID );
}
void CWndStatic::OnDraw( C2DRender* p2DRender )
{
	CPoint point( 4, 4 );
	
	if( IsWndStyle( WSS_GROUPBOX ) )
	{
		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont( CWndBase::m_Theme.m_pFontText );

		int nFontMaxHeight = CWndBase::m_Theme.m_pFontText->GetMaxHeight();

		if( ::GetLanguage() == LANG_FRE || ::GetLanguage() == LANG_GER )
			point.y -= 2;
		else
		{
			if(nFontMaxHeight > 14)
				point.y -= nFontMaxHeight - 14;
		}

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
		if( IsWndStyle( WSS_ALIGNHRIGHT ) )
		{
			CSize sizeTitle = p2DRender->m_pFont->GetTextExtent( m_strTitle );
			CRect rect	= GetWindowRect();
			point.x	= ( rect.Width() - sizeTitle.cx - 4 );
			if( point.x < 2 )
				point.x = 2;
		}
		if( IsWndStyle( WSS_ALIGNHCENTER ) )
		{
			CSize sizeTitle = p2DRender->m_pFont->GetTextExtent( m_strTitle );
			CRect rect	= GetWindowRect();
			point.x	= (rect.Width() - sizeTitle.cx) / 2;
			if( point.x < 2 )
				point.x = 2;
		}
		if( IsWndStyle( WSS_ALIGNVBOTTOM ) )
		{
			CSize sizeTitle = p2DRender->m_pFont->GetTextExtent( m_strTitle );
			CRect rect	= GetWindowRect();
			point.y	= rect.Height() - sizeTitle.cy;
			if( point.y < 2 )
				point.y = 2;
		}
		if( IsWndStyle( WSS_ALIGNVCENTER ) )
		{
			CSize sizeTitle = p2DRender->m_pFont->GetTextExtent( m_strTitle );
			CRect rect	= GetWindowRect();
			point.y	= (rect.Height() - sizeTitle.cy + 4) / 2;
			if( point.y < 2 )
				point.y = 2;
		}
#endif // __IMPROVE_QUEST_INTERFACE
		//p2DRender->TextOut( 6, 0, m_strTitle, D3DCOLOR_ARGB( 255, 217, 91, 51) );
		//p2DRender->TextOut( 7, 0, m_strTitle, D3DCOLOR_ARGB( 255, 217, 91, 51) );
		//p2DRender->TextOut( 6, 2, m_strTitle, D3DCOLOR_ARGB( 255, 217, 91, 51) );
		//p2DRender->TextOut( 8, 0, m_strTitle, D3DCOLOR_ARGB( 255, 217, 91, 51) );
		//p2DRender->TextOut( 7, 2, m_strTitle, D3DCOLOR_ARGB( 255, 217, 91, 51) );
		//p2DRender->TextOut( 8, 2, m_strTitle, D3DCOLOR_ARGB( 255, 217, 91, 51) );
		p2DRender->TextOut( 0 + point.x, point.y, m_strTitle, m_dwColor );//m_dwColor );
		p2DRender->TextOut( 1 + point.x, point.y, m_strTitle, m_dwColor );
		p2DRender->SetFont( pOldFont );
			/*
		CRect rect = GetWindowRect();
		rect.DeflateRect( 0, 6, 1, 1 );
//fdef __CLIENT
		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle );
		//p2DRender->TextOut( 9, 0, m_strTitle, 0xff6464ff );
		//p2DRender->TextOut( 9, 2, m_strTitle, 0xff6464ff );
		//p2DRender->TextOut( 11, 0, m_strTitle, 0xff6464ff );
		p2DRender->TextOut( 11, 2, m_strTitle, 0xffffffff );
		p2DRender->TextOut( 10, 1, m_strTitle, 0xffffffff );//m_dwColor );
		p2DRender->SetFont( pOldFont );
//#else		
//		p2DRender->TextOut( 10, 0, m_strTitle, 0xffffffff );
//#endif
		CSize size = p2DRender->m_pFont->GetTextExtent( m_strTitle );
		p2DRender->RenderLine( CPoint( rect.left,  rect.top    ), CPoint( rect.left , rect.bottom ), 0xffffffff );
		p2DRender->RenderLine( CPoint( rect.left,  rect.bottom ), CPoint( rect.right, rect.bottom ), 0xffffffff );
		p2DRender->RenderLine( CPoint( rect.right, rect.top    ), CPoint( rect.right, rect.bottom ), 0xffffffff );
		p2DRender->RenderLine( CPoint( rect.left + size.cx + 15,  rect.top    ), CPoint( rect.right, rect.top    ), 0xffffffff );
		p2DRender->RenderLine( CPoint( rect.left,  rect.top ), CPoint( rect.left + 5, rect.top    ), 0xffffffff );
		//p2DRender->RenderRoundRect( rect, 0xfff0f0f0 );
		*/
	}
	else
	{
		CPoint point( 2, 2 );

#if __VER >= 14 // __STATIC_ALIGN
		if( IsWndStyle( WSS_ALIGNHRIGHT ) )
		{
			CSize sizeTitle = p2DRender->m_pFont->GetTextExtent( m_strTitle );
			CRect rect	= GetWindowRect();
			point.x	= ( rect.Width() - sizeTitle.cx - 4 );
			if( point.x < 2 )
				point.x = 2;
		}
		if( IsWndStyle( WSS_ALIGNHCENTER ) )
		{
			CSize sizeTitle = p2DRender->m_pFont->GetTextExtent( m_strTitle );
			CRect rect	= GetWindowRect();
			point.x	= (rect.Width() - sizeTitle.cx) / 2;
			if( point.x < 2 )
				point.x = 2;
		}
		if( IsWndStyle( WSS_ALIGNVBOTTOM ) )
		{
			CSize sizeTitle = p2DRender->m_pFont->GetTextExtent( m_strTitle );
			CRect rect	= GetWindowRect();
			point.y	= rect.Height() - sizeTitle.cy;
			if( point.y < 2 )
				point.y = 2;
		}
		if( IsWndStyle( WSS_ALIGNVCENTER ) )
		{
			CSize sizeTitle = p2DRender->m_pFont->GetTextExtent( m_strTitle );
			CRect rect	= GetWindowRect();
			point.y	= (rect.Height() - sizeTitle.cy + 4) / 2;
			if( point.y < 2 )
				point.y = 2;
		}
#endif // __STATIC_ALIGN

		if( IsWndStyle( WSS_MONEY ) )
		{
			CString str;
			str = GetNumberFormatEx(m_strTitle);
			CD3DFont* pOldFont = p2DRender->GetFont();
			p2DRender->SetFont( CWndBase::m_Theme.m_pFontText );
			p2DRender->TextOut( 0 + point.x, point.y, str, m_dwColor );
			p2DRender->TextOut( 1 + point.x, point.y, str, m_dwColor );
			p2DRender->SetFont( pOldFont );
		}
		else
		{
			CD3DFont* pOldFont = p2DRender->GetFont();
			p2DRender->SetFont( CWndBase::m_Theme.m_pFontText );
			//p2DRender->TextOut( 6, 0, m_strTitle, D3DCOLOR_ARGB( 255, 217, 91, 51) );
			//p2DRender->TextOut( 7, 0, m_strTitle, D3DCOLOR_ARGB( 255, 217, 91, 51) );
			//p2DRender->TextOut( 6, 2, m_strTitle, D3DCOLOR_ARGB( 255, 217, 91, 51) );
			//p2DRender->TextOut( 8, 0, m_strTitle, D3DCOLOR_ARGB( 255, 217, 91, 51) );
			//p2DRender->TextOut( 7, 2, m_strTitle, D3DCOLOR_ARGB( 255, 217, 91, 51) );
			//p2DRender->TextOut( 8, 2, m_strTitle, D3DCOLOR_ARGB( 255, 217, 91, 51) );
			//p2DRender->TextOut( 1, 0, m_strTitle, 0xff10106f );//m_dwColor );
			//p2DRender->TextOut( 0, 0, m_strTitle, 0xff7070cf );//m_dwColor );

			p2DRender->TextOut( 0 + point.x, point.y,  m_strTitle, m_dwColor );//m_dwColor );
			p2DRender->TextOut( 1 + point.x, point.y,  m_strTitle, m_dwColor );//m_dwColor );

			p2DRender->SetFont( pOldFont );
		}
	}
}
BOOL CWndStatic::IsPickupSpace( CPoint point )
{
	if( IsWndStyle( WSS_GROUPBOX ) || IsWndStyle( WSS_PICTURE ) )
	{
		return FALSE;
	}
	CRect rect = GetWindowRect();
	if( rect.PtInRect( point ) )
	{
		if( m_bLButtonDown )
		{
			int a = 0;
		}
		return TRUE;
	}
	return FALSE;
}
void CWndStatic::OnLButtonUp(UINT nFlags, CPoint point)
{
#ifdef __CLIENT
	if(m_pParentWnd)
		m_pParentWnd->SetFocus();
#endif
}
void CWndStatic::OnLButtonDown(UINT nFlags, CPoint point)
{
#ifdef __CLIENT
	if(m_pParentWnd)
		m_pParentWnd->SetFocus();
#endif
}
void CWndStatic::PaintFrame( C2DRender* p2DRender )
{
	if( m_strTexture.IsEmpty() == FALSE || IsWndStyle( WSS_PICTURE ) == FALSE )
		return;
	CRect rect = GetWindowRect();
	p2DRender->RenderRect( rect, 0xffffffff );//ADEILSON 0xff000000
}


void CWndCustom::OnDraw( C2DRender* p2DRender )
{
	p2DRender->RenderFillRect( GetWindowRect(), 0xff808080 );
	//return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndButton
////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
WBS_TEXT
WBS_SPRITE
WBS_RADIO
WBS_CHECK

ex)
	WBS_TEXT
	WBS_TEXT | WBS_RADIO
	WBS_TEXT | WBS_CHECK
	WBS_SPRITE
	WBS_SPRITE | WBS_RADIO
	WBS_SPRITE | WBS_CHECK
*/

CWndButton::CWndButton()
{
#ifdef __BUTTONS_V19
	m_nFontColor      = D3DCOLOR_ARGB( 255, 28, 28, 28 );
	m_nPushColor      = D3DCOLOR_ARGB( 255, 28, 28, 28 );
	m_nDisableColor   = D3DCOLOR_ARGB( 255, 28, 28, 28 );
	m_nHighlightColor = D3DCOLOR_ARGB( 255, 116, 107, 85 );
#else
	m_nFontColor      = D3DCOLOR_ARGB( 255,  64,  64,  64 );
	m_nPushColor      = D3DCOLOR_ARGB( 255, 255, 255, 100 );
	m_nDisableColor   = D3DCOLOR_ARGB( 255, 140, 140, 140 );
	m_nHighlightColor = D3DCOLOR_ARGB( 255, 64 , 64 , 255 );
#endif// __BUTTONS_V19
	//m_strSndEffect    = "sound\\sndsyspickupitems.wav";
	m_bCheck          = FALSE;
	m_nTimePush       = 0; 
	m_bHighLight      = FALSE;
	m_bGroup          = FALSE;
	m_ptPush          = CPoint(1,1);
	m_pWndExecute     = NULL;
	m_pWndMenu        = NULL;
	m_bTopDown        = FALSE;
	m_byWndType       = WTYPE_BUTTON;
	m_cHotkey         = 0;
	ZeroMemory( &m_shortcut, sizeof( m_shortcut ) );
		/*
	DWORD     m_dwShortCutType    ;
	DWORD     m_dwShortCutIndex   ;
	DWORD     m_dwShortCutUserId  ;
	DWORD     m_dwShortCutData    ;
	CString   m_strShortCutString ;

	m_dwShortCutType    = SHORTCUT_NONE;
	m_dwShortCutType    = 0;
	m_dwShortCutIndex   = 0;
	m_dwShortCutUserId  = 0;
	m_dwShortCutData    = 0;
	*/
}
CWndButton::~CWndButton()
{
	//SAFE_DELETE( m_pWndMenu );
}
void CWndButton::FitTextureSize() 
{
	if( m_pTexture )
	{
		CRect rect = GetWindowRect( TRUE );
		if( IsWndStyle( WBS_RADIO ) | IsWndStyle( WBS_CHECK ) )
			rect.right = rect.left + ( m_pTexture->m_size.cx / 6 );
		else
			rect.right = rect.left + ( m_pTexture->m_size.cx / 4 );
		SetWndRect( rect );
	}
}
void CWndButton::SetMenu(CWndMenu* pWndMenu)
{
	m_pWndMenu = pWndMenu;
}
void CWndButton::SetWndExecute(CWndBase* pWndBase) 
{ 
	m_pWndExecute = pWndBase; 
	m_pWndExecute->Initialize(this);
	m_pWndExecute->SetVisible(FALSE);
}

BOOL CWndButton::Create(LPCTSTR lpszCaption,DWORD dwStyle,const RECT& rect,CWndBase* pParentWnd,UINT nID)
{
	//m_dwStyle = dwButtonStyle;// | WBS_NODRAW;
	//if(pSprPack)
//		dwStyle |= WBS_SPRITE;
	dwStyle |= WBS_HIGHLIGHT;
	if(lpszCaption && strlen(lpszCaption))
	{
		//dwButtonStyle |= WBS_TEXT;
		m_strTitle = lpszCaption;
	}
	BOOL b = CWndBase::Create(dwStyle|WBS_CHILD|/*WBS_NODRAWFRAME|*/WBS_NOFRAME,rect,pParentWnd,nID);//,pSprPack,nSprIdx,nColorTable);;
	//m_strTexture = "ButtRadio.bmp";
	if( IsWndStyle( WBS_RADIO ) )
		SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, DEF_CTRL_RADIO ), 1 );
	if( IsWndStyle( WBS_CHECK ) )
		SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, DEF_CTRL_CHECK ), 1 );
	
#ifdef __CLIENT
	//AddWndStyle( WBS_NODRAWFRAME );
	//DelWndStyle( WBS_NODRAWFRAME );
#else
	//DelWndStyle( WBS_NODRAWFRAME );
#endif
	return b;
}
void CWndButton::OnDraw( C2DRender* p2DRender )
{
}
void CWndButton::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();
	if(m_pWndExecute)
		if(m_pWndExecute->OnDrawIcon(this,p2DRender))
			return;
	GET_CLIENT_POINT( m_pApp->GetSafeHwnd(),  point );
	CString string;
#ifdef __CLIENT
/*	
	if(	CWndBase::m_GlobalShortcut.m_dwShortcut == SHORTCUT_NONE )	  
	{
		// 툴립 출력 
		switch( m_shortcut.m_dwShortcut )
		{
		case SHORTCUT_NONE   : 
			if( m_strToolTip.IsEmpty() == FALSE )
				g_toolTip.PutToolTip( m_nIdWnd, m_strToolTip, m_rectCurrentWindow, point, m_nToolTipPos );
			break;
		case SHORTCUT_APPLET :
			{
				AppletFunc* pAppletFunc = g_WndMng.GetAppletFunc( m_nIdWnd ); 
				if( pAppletFunc )
				{
					CString string;
					if( pAppletFunc->m_cHotkey == 0 )
						string.Format( "애플렛\n%s", pAppletFunc->m_pAppletDesc );
					else
						string.Format( "애플렛\n%s\n[단축키 %c]", pAppletFunc->m_pAppletDesc, pAppletFunc->m_cHotkey );
					g_toolTip.PutToolTip( m_nIdWnd, string, m_rectCurrentWindow, point, m_nToolTipPos );
				}
			}
			break;
		case SHORTCUT_MOTION :
		case SHORTCUT_SCRIPT :
		case SHORTCUT_ITEM   :
		case SHORTCUT_SKILL  :
		case SHORTCUT_OBJECT :
			{
				string.Format( "오브젝트\n%s", GetTitle() );
				g_toolTip.PutToolTip( m_nIdWnd, string, m_rectCurrentWindow, point, m_nToolTipPos );
			}
			break;
		}
	}
*/	
#endif
	DWORD dwColor = D3DCOLOR_TEMP( 200, 240, 240, 240 );
	m_bHighLight = FALSE;
	int nFontHeight = GetFontHeight();
	int nFontColor = p2DRender->GetTextColor();

	//m_strTitle = "Button";


	if( ( m_dwStyle & WBS_HIGHLIGHT ) && m_GlobalShortcut.m_dwShortcut == SHORTCUT_NONE )
	{
		if( GetClientRect().PtInRect( m_ptMouse ) && m_bEnable == TRUE )
		{
			m_bHighLight = TRUE;
		}
	}
	if(m_dwStyle & WBS_RADIO)
	{
#ifdef __BUTTONS_V19
		m_nFontColor      = D3DCOLOR_ARGB( 255, 255, 249, 198 );
        m_nPushColor      = D3DCOLOR_ARGB( 255,  64,  64,  64 );
        m_nDisableColor   = D3DCOLOR_ARGB( 255, 100, 100, 100 );
        m_nHighlightColor = D3DCOLOR_ARGB( 255, 116, 107, 85 );
#endif// __BUTTONS_V19
		m_pTheme->RenderWndButtonRadio( p2DRender, this );
	}
	else
	if(m_dwStyle & WBS_CHECK)
	{
#ifdef __BUTTONS_V19
		m_nFontColor      = D3DCOLOR_ARGB( 255, 255, 249, 198 );
        m_nPushColor      = D3DCOLOR_ARGB( 255,  64,  64,  64 );
        m_nDisableColor   = D3DCOLOR_ARGB( 255, 100, 100, 100 );
        m_nHighlightColor = D3DCOLOR_ARGB( 255, 116, 107, 85 );
#endif// __BUTTONS_V19
		m_pTheme->RenderWndButtonCheck( p2DRender,  this );
	}
	else
	if(m_dwStyle & WBS_TEXT)
	{
		m_pTheme->RenderWndButtonText( p2DRender, this );
	}
	else
	if(m_dwStyle & WBS_MENUITEM)
	{
		CPoint pt(0);

		DWORD dwColor1 = ( m_bHighLight == TRUE ?	D3DCOLOR_ARGB( 155, 200, 200, 200 ) : D3DCOLOR_ARGB( 0, 0, 0, 0 ) );
		DWORD dwColor2 = ( m_bHighLight == TRUE ?	D3DCOLOR_ARGB( 155, 200, 200, 200 ) : D3DCOLOR_ARGB( 0, 0, 0, 0 ) );
		DWORD dwColor3 = ( m_bHighLight == TRUE ?	D3DCOLOR_ARGB( 155, 200, 200, 200 ) : D3DCOLOR_ARGB( 0, 0, 0, 0 ) );
		CRect rect = GetClientRect();

		if( m_pWndMenu )
		{
			if( m_bTopDown )
			{
				p2DRender->RenderFillRect( rect, dwColor3, dwColor3, dwColor3, dwColor3 );
				p2DRender->RenderRoundRect( rect, dwColor );

				if( m_bTopDown == 1 )
				{
					p2DRender->RenderLine( 
						CPoint( rect.right - 14, rect.top + 6 ), 
						CPoint( rect.right - 4 , rect.top + 6 ), 
						dwColor
						);
					p2DRender->RenderFillTriangle( 
						CPoint( rect.right - 14, rect.top + 8 ), 
						CPoint( rect.right - 4 , rect.top + 8 ), 
						CPoint( rect.right - 9 , rect.top + 14 ), 
						dwColor
						);
				}
				else
				{
					p2DRender->RenderLine( 
						CPoint( rect.right - 14, rect.top + 13 ), 
						CPoint( rect.right - 4 , rect.top + 13 ), 
						dwColor
						);
					p2DRender->RenderFillTriangle( 
						CPoint( rect.right - 14, rect.top + 12 ), 
						CPoint( rect.right - 3 , rect.top + 12 ), 
						CPoint( rect.right - 9 , rect.top + 6 ), 
						dwColor
						);
				}
			}
			else
			{
				//DRender->RenderFillRect( rect, dwColor1, dwColor1, dwColor1, dwColor1 );
				//DRender->RenderRoundRect( rect, dwColor );

				if( m_pWndMenu->IsVisible() )
				{
					dwColor1 = 	D3DCOLOR_TEMP( 155, 200, 200, 200 );
					p2DRender->RenderFillRect( rect, dwColor1, dwColor1, dwColor1, dwColor1 );
					dwColor = m_nPushColor;
				}
				p2DRender->RenderFillTriangle( 
					CPoint( rect.right - 10, rect.top    + 4 ), 
					CPoint( rect.right - 10, rect.bottom - 4 ), 
					CPoint( rect.right - 5, rect.top + rect.Height() / 2 ), 
					0xffa0a0ff //dwColor
					);
				// 매뉴 아이템의 아이콘 
				if( m_pTexture )
				{
					CSize size = m_pTexture->m_size;
					m_pTexture->m_size.cx = 20;
					m_pTexture->m_size.cy = 20;
					m_pTexture->Render( p2DRender, CPoint( pt.x, pt.y     ), m_nAlphaCount );
					m_pTexture->m_size = size;
				}
			}
			p2DRender->TextOut( pt.x + 4 + 20, pt.y + 4, m_strTitle, 0xffffffff  );//ADEILSON 0xff000000
		}
		else
		{
			if( m_bHighLight )
			{
				p2DRender->RenderFillRect( rect, dwColor2, dwColor2, dwColor2, dwColor2 );
				p2DRender->RenderRoundRect( rect, dwColor );
			}
			// 매뉴 아이템의 아이콘 
			if( m_pTexture )
			{
				CSize size = m_pTexture->m_size;
				m_pTexture->m_size.cx = 20;
				m_pTexture->m_size.cy = 20;
				m_pTexture->Render( p2DRender, CPoint( pt.x, pt.y ), m_nAlphaCount );
				m_pTexture->m_size = size;
			}
			// 체크 
			if( GetCheck() ) 
			{
				p2DRender->RenderLine( CPoint( 2,  2), CPoint( 5, 8), 0xffffffff );//ADEILSON 0xff000000
				p2DRender->RenderLine( CPoint( 2,  2), CPoint( 6, 8), 0xffffffff );//ADEILSON 0xff000000
				p2DRender->RenderLine( CPoint( 5,  8), CPoint( 8, 3), 0xffffffff );//ADEILSON 0xff000000
			}
			p2DRender->TextOut( pt.x + 4 + 20, pt.y + 4, m_strTitle, 0xffffffff );//ADEILSON 0xff000000
			if( m_cHotkey )
			{
				CString strHotkey = m_cHotkey;
				p2DRender->TextOut( m_rectClient.right - 32, pt.y + 4, strHotkey, 0xffffffff );//ADEILSON 0xff000000
			}
		}
		//if( IsButtonStyle( WBS_NOMENUICON ) )
		//	p2DRender->TextOut( pt.x + 4, pt.y + 4, m_strTitle, dwColor );
		//else

	}
	else
	// 표준 버튼 
	{
		/*
		if( m_pTexture )
		{
			m_pTexture->m_size.cx / 3
			p2DRender->RenderTexture( CPoint( 0, 0), m_pTexture );
		}
		else
		*/
			m_pTheme->RenderWndButton( p2DRender, this );
	}
	/*
	p2DRender->SetTextColor( D3DCOLOR_TEMP( 255, 255, 255, 255 ) );
	rect = GetClientRect();
	if( m_shortcut.m_dwShortcut && !( m_dwStyle & WBS_MENUITEM ) )
	{
		switch( m_shortcut.m_dwShortcut )
		{
		case SHORTCUT_NONE   : break; 
		case SHORTCUT_APPLET : p2DRender->RenderRect( rect, D3DCOLOR_ARGB( 255,   0, 255, 255)  ); break;
		case SHORTCUT_MOTION : p2DRender->RenderRect( rect, D3DCOLOR_ARGB( 255, 255, 255, 0  )  ); break;
		case SHORTCUT_SCRIPT : p2DRender->RenderRect( rect, D3DCOLOR_ARGB( 255,   0, 255, 0  )  ); break;
		case SHORTCUT_ITEM   : p2DRender->RenderRect( rect, D3DCOLOR_ARGB( 255, 255,   0, 255)  ); break;
		case SHORTCUT_SKILL  : p2DRender->RenderRect( rect, D3DCOLOR_ARGB( 255,   0,   0,  0 )  ); break;
		case SHORTCUT_OBJECT : p2DRender->RenderRect( rect, D3DCOLOR_ARGB( 255, 255, 255,  0 )  ); break;
		}
	}
	*/
}
BOOL CWndButton::Process()
{
	//if(m_nTimePush && m_timerPush.Over() && IsPush())
		//m_pParentWnd->OnCommand(m_nIdWnd,0);
	return CWndBase::Process();
}
void CWndButton::SetPushTime(int nTime) 
{ 
	m_nTimePush = nTime; 
}
UINT CWndButton::GetState() const // Retrieves the check state, highlight state, and focus state of a button control.
{
	return 1;
}
void CWndButton::SetState(BOOL bHighlight) // Sets the highlighting state of a button control
{
}
int CWndButton::GetCheck() const // Retrieves the check state of a button control.
{
	return m_bCheck;
}
void CWndButton::SetCheck(int nCheck) // Sets the check state of a button control.
{
	m_bCheck = nCheck;
}
/*
UINT CWndButton::GetButtonStyle() const // Retrieves information about the button control style.
{
	return m_dwStyle;
}
void CWndButton::SetButtonStyle(UINT nStyle, BOOL bRedraw) // Changes the style of a button
{
}
*/
void CWndButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	if( IsPush() )
	{
///		PLAYSND( m_strSndEffect, NULL );

		if( m_pWndMenu && m_bTopDown )
		{
			CWndMenu* pWndMenu = (CWndMenu*)m_pParentWnd;
			// 공간 넓히기 
			for( int i = 0; i < pWndMenu->m_awndMenuItem.GetSize(); i++ )
			{
				if( pWndMenu->GetMenuItem( i ) == this )
				{
					for(int i2 = 0; i2 < m_pWndMenu->m_awndMenuItem.GetSize(); i2++ )
					{
						if(m_bTopDown == 1)
						{
							CWndButton* pMenuIns = m_pWndMenu->GetMenuItem( i2 );
							pWndMenu->InsertMenu( ++i, 0, pMenuIns->GetWndId(), pMenuIns->GetTitle() );
						}
						else
							pWndMenu->DeleteMenu( i + 1, 0 );
					}
					break;
				}
			}
			if(m_bTopDown == 1)
				m_bTopDown = 2;
			else
				m_bTopDown = 1;
			m_bHighLight = FALSE;
			m_bPush = FALSE;
			return;
		}
		if( m_pWndExecute )
			m_pWndExecute->OnSystemNotify( WM_LBUTTONUP, 0, NULL );

		if( m_dwStyle & WBS_CHECK )
			m_bCheck = !m_bCheck;

		CWndBase* pWnd = m_pParentWnd;
		//if( ! (pWnd->GetStyle() & WBS_CHILD ) )
			pWnd->OnChildNotify( WNM_CLICKED, m_nIdWnd, (LRESULT*)this); 

		if( (m_dwStyle & WBS_RADIO) && m_pParentWnd)
		{
			CPtrArray* pWndArray = m_pParentWnd->GetWndArray();
			// 내 버튼의 인덱스를 페어런트로부터 찾는다.
			for( int i = 0; i < pWndArray->GetSize(); i++)
			{
				CWndButton* pWndBase = (CWndButton*)pWndArray->GetAt(i);
				if( pWndBase == this )
				{
					// 앞쪽으로 그룹 선언을 찾는다.
					for( int i2 = i; i2 >= 0; i2-- )
					{
						pWndBase = (CWndButton*)pWndArray->GetAt(i2);
						if(pWndBase->IsGroup() && pWndBase->IsWndStyle(WBS_RADIO))
						{
							// 라디오 버튼이 아니거나 새로운 그룹이 나타날 때까지 검색 
							for(int i3 = i2; i3 < pWndArray->GetSize(); i3++)
							{
								pWndBase = (CWndButton*)pWndArray->GetAt(i3);
								if(i3 > i2 && pWndBase->IsGroup())
									break;
								if(pWndBase->IsWndStyle(WBS_RADIO))
									((CWndButton*)pWndBase)->SetCheck(0);
								else
									break;
							}
							SetCheck(TRUE);
							return;
						}
					}
				}
			}
		}
	}
}
void CWndButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	PLAYSND( SND_INF_CLICK );
	//g_Sound.Play("sound\\SndSysEquippingWeapon.wav");
//	m_timerPush.Set(m_nTimePush); // 1초의 딜레이 
	/*
	if( ( m_dwStyle & WBS_MENUITEM ) && m_pWndMenu == NULL )
	{
		m_GlobalShortcut.m_dwShortcut  = SHORTCUT_APPLET;
		m_GlobalShortcut.m_dwId = m_nIdWnd;
		m_GlobalShortcut.m_pTexture = m_pTexture;
		_tcscpy( m_GlobalShortcut.m_szString, GetTitle() );
	}
	*/
}
void CWndButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if( ( m_dwStyle & WBS_MENUITEM ) && m_pWndMenu == NULL && IsPush() )
	{
		m_GlobalShortcut.m_dwShortcut  = m_shortcut.m_dwShortcut;//SHORTCUT_APPLET;
		m_GlobalShortcut.m_dwId = m_nIdWnd;
		m_GlobalShortcut.m_pTexture = m_pTexture;
		/*
		if( m_shortcut.m_dwShortcut == SHORTCUT_OBJECT )
		{
		//m_GlobalShortcut.m_dwShortcut = SHORTCUT_OBJECT;
		//m_GlobalShortcut.m_dwIndex    = m_nIdWnd;
		//m_GlobalShortcut.m_dwId       = ((CCtrl*)m_pSelectObj)->GetId();
		//m_GlobalShortcut.m_dwData     = (DWORD) m_pSelectObj;
		//strcpy( m_GlobalShortcut.m_szString, m_pSelectObj->GetName() );
		*/
		_tcscpy( m_GlobalShortcut.m_szString, GetTitle() );
	}
}

void CWndButton::OnRButtonUp(UINT nFlags, CPoint point)
{
}

void CWndButton::OnRButtonDown(UINT nFlags, CPoint point)
{
}
void CWndButton::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	//PLAYSND( m_strSndEffect, NULL );
	//OnLButtonUp(nFlags,point);
	//CWndBase* pWnd = m_pParentWnd;
	//if( ! (pWnd->GetStyle() & WBS_CHILD ) )
		//pWnd->OnChildNotify( WNM_DBLCLK, m_nIdWnd, (LRESULT*)this ); 
}
void CWndButton::OnRButtonDblClk( UINT nFlags, CPoint point)
{
}
void CWndButton::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
//	if(m_dwStyle & WBS_CHILD && m_bEnable == TRUE && nChar == VK_SPACE)
//	{
	//	PlayEffect(m_strSndEffect);
		//m_pParentWnd->OnCommand(m_nIdWnd,WM_LBUTTONDOWN);
	//}
}
BOOL CWndButton::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CWndBase* pParent = (CWndBase*)GetParentWnd();
	return pParent->OnChildNotify( WIN_ITEMDROP, m_nIdWnd, (LRESULT*)pShortcut ); 
}
LRESULT CWndButton::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return CWndBase::DefWindowProc(message,wParam,lParam);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndTreeCtrl
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndTreeCtrl::CWndTreeCtrl()
{
	//m_nWndColor    = D3DCOLOR_TEMP( 255,  0x5 << 3,  0x5 << 3,  0x5 << 3 );
	m_byWndType    = WTYPE_TREECTRL;

	m_strTexture = DEF_CTRL_TEXT;
	m_bTile = TRUE;
	m_nLineSpace = 3;

#if __VER >= 19 
	m_nFontColor   = D3DCOLOR_ARGB( 255, 255, 255, 255 );
	m_nSelectColor   = D3DCOLOR_ARGB( 255, 0, 255,  50 );
#else
	m_nFontColor   = D3DCOLOR_ARGB( 255, 64, 64, 64 );
	m_nSelectColor   = D3DCOLOR_ARGB( 255, 64, 64,  255 );
#endif//__VER >= 19

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	m_pFocusElem = NULL;
	m_nFontHeight = 0;
	m_pTexButtOpen = NULL;
	m_pTexButtClose = NULL;
	m_bMemberCheckingMode = FALSE;
	m_nMaxCheckNumber = 0;
	m_nTreeTabWidth = 16;
	m_nCategoryTextSpace = 16;
	m_nTreeItemsMaxWidth = 0;
#endif // __IMPROVE_QUEST_INTERFACE
} 

CWndTreeCtrl::~CWndTreeCtrl()
{
	DeleteAllItems();
}
BOOL CWndTreeCtrl::Create(DWORD dwTextStyle,const RECT& rect,CWndBase* pParentWnd,UINT nID)
{
	return CWndBase::Create( WBS_CHILD | dwTextStyle, rect, pParentWnd, nID );
}
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
void CWndTreeCtrl::DeleteItemArray( void )
{
	for(int i = 0; i < m_treeItemArray.GetSize(); i++)
		safe_delete( (LPTREEITEM)m_treeItemArray.GetAt(i) );
	m_treeItemArray.RemoveAll();
}
#endif // __IMPROVE_QUEST_INTERFACE
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
BOOL CWndTreeCtrl::DeleteAllItems()
{
	FreeTree(m_treeElem.m_ptrArray);
	DeleteItemArray();
	m_pFocusElem = NULL;
	return TRUE;
}
#else // __IMPROVE_QUEST_INTERFACE
BOOL CWndTreeCtrl::DeleteAllItems()
{
	FreeTree(m_treeElem.m_ptrArray);
	for(int i = 0; i < m_treeItemArray.GetSize(); i++)
		safe_delete( (LPTREEITEM)m_treeItemArray.GetAt(i) );
	m_pFocusElem = NULL;
	m_treeItemArray.RemoveAll();
	return TRUE;
}
#endif // __IMPROVE_QUEST_INTERFACE
LPTREEELEM CWndTreeCtrl::GetCurSel()
{
	return m_pFocusElem;
}
LPTREEELEM CWndTreeCtrl::GetRootElem() 
{ 
	return &m_treeElem; 
}
LPTREEELEM CWndTreeCtrl::GetNextElem( LPTREEELEM pElem, int& nPos )
{
	LPTREEELEM pGetElem = NULL;
	if( nPos < pElem->m_ptrArray.GetSize() ) 
	{
		pGetElem = (LPTREEELEM)pElem->m_ptrArray.GetAt( nPos );
		nPos++;
	}
	return pGetElem;
}
LPTREEELEM CWndTreeCtrl::FindTreeElem( DWORD dwData )
{
	return FindTreeElem( m_treeElem.m_ptrArray, dwData );
}
LPTREEELEM CWndTreeCtrl::FindTreeElem( CPtrArray& ptrArray, DWORD dwData )
{
	for(int i = 0; i < ptrArray.GetSize(); i++)
	{
		LPTREEELEM lpTreeElem = (LPTREEELEM)ptrArray.GetAt(i);
		if( lpTreeElem->m_dwData == dwData )
			return lpTreeElem;
		if( lpTreeElem->m_ptrArray.GetSize() )
		{
			lpTreeElem = FindTreeElem( lpTreeElem->m_ptrArray, dwData );
			if( lpTreeElem ) return lpTreeElem;
		}
	}
	return NULL;
}
LPTREEELEM CWndTreeCtrl::SetCurSel( DWORD dwData )
{
	m_pFocusElem = FindTreeElem( m_treeElem.m_ptrArray, dwData );
	return m_pFocusElem;
}
LPTREEELEM CWndTreeCtrl::SetCurSel( LPCTSTR lpszKeyword )
{
	m_pFocusElem = FindTreeElem( m_treeElem.m_ptrArray, lpszKeyword );
	return m_pFocusElem;
}

void CWndTreeCtrl::FreeTree( CPtrArray& ptrArray ) 
{
	for(int i = 0; i < ptrArray.GetSize(); i++)
	{
		LPTREEELEM lpTreeElem = (LPTREEELEM)ptrArray.GetAt(i);
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
		if( m_bMemberCheckingMode == TRUE )
			SAFE_DELETE( lpTreeElem->m_pWndCheckBox );
#endif // __IMPROVE_QUEST_INTERFACE
		if(lpTreeElem->m_ptrArray.GetSize())
			FreeTree(lpTreeElem->m_ptrArray);
		safe_delete( lpTreeElem );
	}
	ptrArray.RemoveAll();
}
LPTREEELEM CWndTreeCtrl::FindTreeElem( LPCTSTR lpszKeyword )
{
	return FindTreeElem( m_treeElem.m_ptrArray, lpszKeyword );
}
LPTREEELEM CWndTreeCtrl::FindTreeElem( CPtrArray& ptrArray, LPCTSTR lpszKeyword )
{
	for(int i = 0; i < ptrArray.GetSize(); i++)
	{
		LPTREEELEM lpTreeElem = (LPTREEELEM)ptrArray.GetAt(i);
		if( lpTreeElem->m_strKeyword == lpszKeyword )
			return lpTreeElem;
		if( lpTreeElem->m_ptrArray.GetSize() )
		{
			lpTreeElem = FindTreeElem( lpTreeElem->m_ptrArray, lpszKeyword );
			if( lpTreeElem ) return lpTreeElem;
		}
	}
	return NULL;
}

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
LPTREEELEM CWndTreeCtrl::InsertItem( LPTREEELEM lpParent, LPCTSTR lpString, DWORD dwData, BOOL bForbidChecking, BOOL bCheck, DWORD dwFontColor, DWORD dwSelectColor )
{
	LPTREEELEM lpTreeElem = new TREEELEM( &GetClientRect() );
	lpTreeElem->m_lpParent = lpParent;
	lpTreeElem->m_dwColor = dwFontColor;
	lpTreeElem->m_dwSelectColor = dwSelectColor;
	lpTreeElem->m_strKeyword.Init( m_pFont, &GetClientRect() );
	lpTreeElem->m_strKeyword.SetParsingString( lpString, dwFontColor, 0x00000000, 0, 0x00000001, TRUE );
	lpTreeElem->m_dwData = dwData;
	lpTreeElem->m_bOpen = FALSE;

	if( m_bMemberCheckingMode == TRUE && bForbidChecking == FALSE )
	{
		lpTreeElem->m_pWndCheckBox = new CWndButton;
		CWndButton* pWndCheckBox = lpTreeElem->m_pWndCheckBox;
		CRect rectCheckBox( 0, 0, CHECK_BOX_SIZE_XY, CHECK_BOX_SIZE_XY );
		pWndCheckBox->Create( "", WBS_CHECK, rectCheckBox, this, WIDC_CHECK );
		pWndCheckBox->SetTexture( D3DDEVICE, MakePath( DIR_THEME, _T( DEF_CTRL_CHECK ) ), 1 );
		pWndCheckBox->FitTextureSize();
		pWndCheckBox->SetCheck( bCheck );
		pWndCheckBox->EnableWindow( FALSE );
		pWndCheckBox->SetVisible( FALSE );
	}
	if( lpParent )
		lpParent->m_ptrArray.Add( lpTreeElem );
	else
		m_treeElem.m_ptrArray.Add( lpTreeElem );
	return lpTreeElem;
}
#else // __IMPROVE_QUEST_INTERFACE
LPTREEELEM CWndTreeCtrl::InsertItem( LPTREEELEM lpParent, LPCTSTR lpString, DWORD dwData )
{
	LPTREEELEM lpTreeElem = new TREEELEM;

	lpTreeElem->m_lpParent = lpParent;
	lpTreeElem->m_dwColor = m_nFontColor;
	lpTreeElem->m_strKeyword = lpString;
	lpTreeElem->m_dwData = dwData;
	lpTreeElem->m_bOpen = FALSE;
	if( lpParent )
		lpParent->m_ptrArray.Add( lpTreeElem );
	else
		m_treeElem.m_ptrArray.Add( lpTreeElem );
	return lpTreeElem;
}
#endif // __IMPROVE_QUEST_INTERFACE
void CWndTreeCtrl::LoadTreeScript(LPCTSTR lpFileName) 
{
	CScript script;
 	if( script.Load( lpFileName ) == FALSE )
		return;
	//script.GetToken(); // keyword
	do {
		InterpriteScript( script, m_treeElem.m_ptrArray );
		script.GetToken(); // keyword
	} while( script.tok != FINISHED );

}
void CWndTreeCtrl::InterpriteScript( CScript& script, CPtrArray& ptrArray ) 
{
	LPTREEELEM lpTreeElem	= NULL;
	script.GetToken();	// keyword
	while( *script.token != '}' && script.tok != FINISHED )
	{
		lpTreeElem	= new TREEELEM;
		lpTreeElem->m_strKeyword	= script.Token;
		lpTreeElem->m_bOpen		= FALSE;
		lpTreeElem->m_dwColor	= m_nFontColor;
		ptrArray.Add(lpTreeElem);
		lpTreeElem->m_dwData	= 0;
		script.GetToken();	// keyword
		if( *script.token == '{' )
		{
			if( lpTreeElem )
				InterpriteScript( script, lpTreeElem->m_ptrArray );
		}
	}
	if( script.tok == FINISHED )
		return;
	script.GetToken();
}
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
BOOL CWndTreeCtrl::CheckParentTreeBeOpened( LPTREEELEM lpTreeElem )
{
	if( lpTreeElem->m_bOpen == FALSE )
		return lpTreeElem->m_bOpen;
	else
	{
		if( lpTreeElem->m_lpParent )
			return CheckParentTreeBeOpened( lpTreeElem->m_lpParent );
		else
			return lpTreeElem->m_bOpen;
	}
}
CPtrArray* CWndTreeCtrl::GetTreeItemArray( void )
{
	return &m_treeItemArray;
}
void CWndTreeCtrl::SetFocusElem( const LPTREEELEM pFocusElem )
{
	m_pFocusElem = pFocusElem;
}
const LPTREEELEM CWndTreeCtrl::GetFocusElem( void ) const
{
	return m_pFocusElem;
}
void CWndTreeCtrl::SetLineSpace( int nLineSpace )
{
	m_nLineSpace = nLineSpace;
}
int CWndTreeCtrl::GetLineSpace( void ) const
{
	return m_nLineSpace;
}
void CWndTreeCtrl::SetMemberCheckingMode( BOOL bMemberCheckingMode )
{
	m_bMemberCheckingMode = bMemberCheckingMode;
}
BOOL CWndTreeCtrl::GetMemberCheckingMode( void ) const
{
	return m_bMemberCheckingMode;
}
void CWndTreeCtrl::SetMaxCheckNumber( int nMaxCheckNumber )
{
	m_nMaxCheckNumber = nMaxCheckNumber;
}
int CWndTreeCtrl::GetMaxCheckNumber( void ) const
{
	return m_nMaxCheckNumber;
}
void CWndTreeCtrl::SetTreeTabWidth( int nTreeTabWidth )
{
	m_nTreeTabWidth = nTreeTabWidth;
}
int CWndTreeCtrl::GetTreeTabWidth( void ) const
{
	return m_nTreeTabWidth;
}
void CWndTreeCtrl::SetCategoryTextSpace( int nCategoryTextSpace )
{
	m_nCategoryTextSpace = nCategoryTextSpace;
}
int CWndTreeCtrl::GetCategoryTextSpace( void ) const
{
	return m_nCategoryTextSpace;
}
int CWndTreeCtrl::GetTreeItemsNumber( void ) const
{
	int nTreeItemsNumber = 0;
	CalculateTreeItemsNumber( nTreeItemsNumber, m_treeElem.m_ptrArray );
	return nTreeItemsNumber;
}
void CWndTreeCtrl::SetTreeItemsMaxWidth( int nTreeItemsMaxWidth )
{
	m_nTreeItemsMaxWidth = nTreeItemsMaxWidth;
}
int CWndTreeCtrl::GetTreeItemsMaxWidth( void ) const
{
	return m_nTreeItemsMaxWidth;
}
int CWndTreeCtrl::GetTreeItemsMaxHeight( void )
{
	int nTreeItemsMaxHeight = 6;
	CalculateTreeItemsMaxHeight( nTreeItemsMaxHeight, m_treeElem.m_ptrArray );
	return nTreeItemsMaxHeight;
}
void CWndTreeCtrl::SetTextColor( DWORD dwCategoryTextColor, DWORD dwNormalTextColor, DWORD dwSelectedCategoryTextColor, DWORD dwSelectedNormalTextColor )
{
	CalculateTextColor( dwCategoryTextColor, dwNormalTextColor, dwSelectedCategoryTextColor, dwSelectedNormalTextColor, m_treeElem.m_ptrArray );
}
void CWndTreeCtrl::CalculateTreeItemsNumber( int& nSumTreeItemsNumber, const CPtrArray& rPtrArray ) const
{
	for( int i = 0; i < rPtrArray.GetSize(); ++i )
	{
		LPTREEELEM lpTreeElem = ( LPTREEELEM )rPtrArray.GetAt( i );
 		if( lpTreeElem )
		{
			++nSumTreeItemsNumber;
			if( lpTreeElem->m_ptrArray.GetSize() > 0 )
				CalculateTreeItemsNumber( nSumTreeItemsNumber, lpTreeElem->m_ptrArray );
		}
	}
}
void CWndTreeCtrl::CalculateTreeItemsMaxHeight( int& nSumHeight, const CPtrArray& rPtrArray )
{
	for( int i = 0; i < rPtrArray.GetSize(); ++i )
	{
		LPTREEELEM lpTreeElem = ( LPTREEELEM )rPtrArray.GetAt( i );
		if( lpTreeElem )
		{
			nSumHeight += GetFontHeight();
			if( lpTreeElem->m_ptrArray.GetSize() > 0 && lpTreeElem->m_bOpen == TRUE )
				CalculateTreeItemsMaxHeight( nSumHeight, lpTreeElem->m_ptrArray );
		}
	}
}
void CWndTreeCtrl::CalculateTextColor( DWORD dwCategoryTextColor, DWORD dwNormalTextColor, DWORD dwSelectedCategoryTextColor, DWORD dwSelectedNormalTextColor, const CPtrArray& rPtrArray )
{
	for( int i = 0; i < rPtrArray.GetSize(); ++i )
	{
		LPTREEELEM lpTreeElem = ( LPTREEELEM )rPtrArray.GetAt( i );
 		if( lpTreeElem )
		{
			if( lpTreeElem->m_ptrArray.GetSize() > 0 || lpTreeElem->m_lpParent == NULL )
			{
				lpTreeElem->m_dwColor = dwCategoryTextColor;
				lpTreeElem->m_dwSelectColor = dwSelectedCategoryTextColor;
				CalculateTextColor( dwCategoryTextColor, dwNormalTextColor, dwSelectedCategoryTextColor, dwSelectedNormalTextColor, lpTreeElem->m_ptrArray );
			}
			else
			{
				lpTreeElem->m_dwColor = dwNormalTextColor;
				lpTreeElem->m_dwSelectColor = dwSelectedNormalTextColor;
			}
		}
	}
}
#endif // __IMPROVE_QUEST_INTERFACE
void CWndTreeCtrl::OnInitialUpdate()
{
	//CSize size = m_pSprPack->GetAt(13)->GetSize();
	CRect rect = GetWindowRect();
	m_pTexButtOpen  = m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtTreeOpen.tga"   ), 0xffff00ff );
	m_pTexButtClose = m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtTreeClose.tga"   ), 0xffff00ff );
	//AddWndStyle(WBS_CAPTION);
	//m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_DOCKING | WBS_VERT, rect, this, 1000 );//,m_pSprPack,-1);
	m_wndScrollBar.SetVisible( IsWndStyle( WBS_VSCROLL ) );
}
void CWndTreeCtrl::SetWndRect(CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect( 4, 4 );
	if( IsWndStyle( WBS_VSCROLL ) ) 
		m_rectClient.right -= 15;
	m_wndScrollBar.SetVisible( IsWndStyle( WBS_VSCROLL ) );

	if( bOnSize )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
}
void CWndTreeCtrl::PaintFrame(C2DRender* p2DRender)
{
	CRect rect = GetWindowRect();
//	m_pTheme->RenderWndTextFrame( p2DRender, &rect );
/*
	DWORD dwColor1 = D3DCOLOR_ARGB( 158, 0, 0,  0 );//D3DCOLOR_TEMP( 255,   0,   0,  50 );//
	DWORD dwColor2 = D3DCOLOR_ARGB( 180, 240, 240,  30 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//
	DWORD dwColor3 = D3DCOLOR_ARGB( 100, 200, 200,  30 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//

	p2DRender->RenderFillRect ( rect, dwColor1 );
	p2DRender->RenderRoundRect( rect, dwColor2 );
	rect.DeflateRect( 1 , 1 );
	p2DRender->RenderRect( rect, dwColor3 );
*/
	//p2DRender->RenderFillRect ( rect, D3DCOLOR_TEMP( 128,  50,  50,  50 ) );
	//p2DRender->RenderRoundRect( rect, D3DCOLOR_TEMP( 128, 200, 200, 200 ) );
}
void CWndTreeCtrl::OnDraw(C2DRender* p2DRender) 
{
	CPoint pt( 3, 3);
	m_nFontHeight = GetFontHeight();
	pt.y -= (m_nFontHeight + 3) * m_wndScrollBar.GetScrollPos();
	for(int i = 0; i < m_treeItemArray.GetSize(); i++)
		safe_delete( (LPTREEITEM)m_treeItemArray.GetAt(i) );
	m_treeItemArray.RemoveAll();
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	m_nTreeItemsMaxWidth = 0;
#endif // __IMPROVE_QUEST_INTERFACE
	PaintTree(p2DRender,pt,m_treeElem.m_ptrArray) ;

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	int nPage = GetClientRect().Height() / m_nFontHeight;
#else // __IMPROVE_QUEST_INTERFACE
	int nPage = GETRANGE; //GetClientRect().Height() / (m_nFontHeight + 3);
#endif // __IMPROVE_QUEST_INTERFACE
	int nRange = m_treeItemArray.GetSize();// - nPage;

	if(	IsWndStyle( WBS_VSCROLL ) )  
	{
		m_wndScrollBar.SetVisible( TRUE );
		m_wndScrollBar.SetScrollRange( 0, nRange );
		m_wndScrollBar.SetScrollPage( nPage );

		if(nPage >= m_wndScrollBar.GetMaxScrollPos()) // 스크롤바가 활성화되지 않을 영역에서의 포지션은 0로 설정
			m_wndScrollBar.SetScrollPos(0);
	}
	else
		m_wndScrollBar.SetVisible( FALSE );
}

void CWndTreeCtrl::PaintTree(C2DRender* p2DRender,CPoint& pt,CPtrArray& ptrArray) 
{
	LPTREEELEM pTreeElem;
	LPTREEITEM pTreeItem;
	for(int i = 0; i < ptrArray.GetSize(); i++)
	{
		pTreeElem = (LPTREEELEM)ptrArray.GetAt( i );
		pTreeItem = new TREEITEM;
		CSize sizeStr;
		p2DRender->m_pFont->GetTextExtent( pTreeElem->m_strKeyword, &sizeStr);
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
		int nRectLeft = pt.x + m_nCategoryTextSpace;
		int nRectTop = pt.y;
		int nRectRight = pt.x + m_nCategoryTextSpace + sizeStr.cx;
		int nRectBottom = pt.y + sizeStr.cy;
		pTreeItem->m_rect.SetRect( nRectLeft, nRectTop, nRectRight, nRectBottom );
		m_nTreeItemsMaxWidth = ( nRectRight > m_nTreeItemsMaxWidth ) ? nRectRight : m_nTreeItemsMaxWidth;
		if( m_bMemberCheckingMode == TRUE && pTreeElem->m_pWndCheckBox )
		{
			CWndButton* pWndCheckBox = pTreeElem->m_pWndCheckBox;
			if( pTreeElem->m_ptrArray.GetSize() == 0 )
			{
				pWndCheckBox->EnableWindow( TRUE );
				pWndCheckBox->SetVisible( TRUE );
				pWndCheckBox->SetWndRect( CRect( pt.x, pt.y - 1, pt.x + CHECK_BOX_SIZE_XY, pt.y + CHECK_BOX_SIZE_XY - 1 ) );
			}
		}
#else // __IMPROVE_QUEST_INTERFACE
		pTreeItem->m_rect.SetRect( pt.x, pt.y, pt.x + m_nFontHeight + 5 + sizeStr.cx, pt.y + sizeStr.cy );
#endif // __IMPROVE_QUEST_INTERFACE
		pTreeItem->m_lpTreeElem = pTreeElem;
		m_treeItemArray.Add( pTreeItem );
		if( pTreeElem->m_ptrArray.GetSize() )
		{

			if( pTreeElem->m_bOpen )
				p2DRender->RenderTexture( pt, m_pTexButtOpen );
			else
				p2DRender->RenderTexture( pt, m_pTexButtClose );

		}
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
		if( m_pFocusElem == pTreeElem )
		{
			pTreeElem->m_strKeyword.SetColor( pTreeElem->m_dwSelectColor );
			p2DRender->TextOut_EditString( nRectLeft, nRectTop, pTreeElem->m_strKeyword );
		}
		else
		{
			pTreeElem->m_strKeyword.SetColor( pTreeElem->m_dwColor );
			p2DRender->TextOut_EditString( nRectLeft, nRectTop, pTreeElem->m_strKeyword );
		}
#else // __IMPROVE_QUEST_INTERFACE
		if( m_pFocusElem == pTreeElem )
		{
			p2DRender->TextOut( pt.x + m_nFontHeight + 2, pt.y, pTreeElem->m_strKeyword, m_nSelectColor );
		}
		else
		{
			p2DRender->TextOut( pt.x + m_nFontHeight + 2, pt.y, pTreeElem->m_strKeyword, pTreeElem->m_dwColor );
		}
#endif // __IMPROVE_QUEST_INTERFACE

		pt.y += GetFontHeight();
		if( pTreeElem->m_ptrArray.GetSize() && pTreeElem->m_bOpen )
		{
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
			pt.x += m_nTreeTabWidth;
			PaintTree( p2DRender, pt, pTreeElem->m_ptrArray );
			pt.x -= m_nTreeTabWidth;
#else // __IMPROVE_QUEST_INTERFACE
			pt.x += GetFontHeight();
			PaintTree(p2DRender,pt,pTreeElem->m_ptrArray) ;
			pt.x -= GetFontHeight();
#endif // __IMPROVE_QUEST_INTERFACE
		}
	}
}
void CWndTreeCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	LPTREEITEM pTreeItem;
	for(int i = 0; i < m_treeItemArray.GetSize(); i++)
	{
		pTreeItem = (LPTREEITEM)m_treeItemArray.GetAt(i);
		if(pTreeItem->m_rect.PtInRect(point))
		{
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
			m_pFocusElem = pTreeItem->m_lpTreeElem;
			CWndBase* pWnd = m_pParentWnd;
			while(pWnd->GetStyle() & WBS_CHILD)
				pWnd = pWnd->GetParentWnd();
			pWnd->OnChildNotify( WNM_CLICKED, m_nIdWnd, (LRESULT*)m_pFocusElem );
			return;
#else // __IMPROVE_QUEST_INTERFACE
			if(m_pFocusElem == pTreeItem->m_lpTreeElem)
			{
				// 부모가 차일드 윈도가 아니어야 OnCommand 메시지를 받는다.
				CWndBase* pWnd = m_pParentWnd;
				while(pWnd->GetStyle() & WBS_CHILD)
					pWnd = pWnd->GetParentWnd();
				pWnd->OnChildNotify( WNM_CLICKED, m_nIdWnd, (LRESULT*)m_pFocusElem );
				return;
			}
#endif // __IMPROVE_QUEST_INTERFACE
		}
	}
}
void CWndTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	LPTREEITEM pTreeItem;
	for(int i = 0; i < m_treeItemArray.GetSize(); i++)
	{
		pTreeItem = (LPTREEITEM)m_treeItemArray.GetAt(i);
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
		if( m_bMemberCheckingMode == TRUE )
		{
			CWndButton* pWndCheckBox = pTreeItem->m_lpTreeElem->m_pWndCheckBox;
			LPTREEELEM pParentTreeElem = pTreeItem->m_lpTreeElem->m_lpParent;
			if( pWndCheckBox && pParentTreeElem && CheckParentTreeBeOpened( pParentTreeElem ) == FALSE )
			{
				if( pWndCheckBox->IsWindowEnabled() == TRUE )
					pWndCheckBox->EnableWindow( FALSE );
				if( pWndCheckBox->IsVisible() == TRUE )
					pWndCheckBox->SetVisible( FALSE );
			}
		}
		CRect rect = pTreeItem->m_rect;
		SIZE size = m_pTexButtOpen->m_size;
		rect.left -= m_nCategoryTextSpace;
		rect.SetRect( rect.left, rect.top, rect.left + size.cx, rect.top + size.cy );
		if( rect.PtInRect( point ) )
		{
			pTreeItem->m_lpTreeElem->m_bOpen = !pTreeItem->m_lpTreeElem->m_bOpen;
			m_pFocusElem = pTreeItem->m_lpTreeElem;
		}
#else // __IMPROVE_QUEST_INTERFACE
		if(pTreeItem->m_rect.PtInRect(point))
		{
			CRect rect = pTreeItem->m_rect;
			rect.SetRect(rect.left,rect.top,rect.left+m_nFontHeight,rect.top+m_nFontHeight);
			if(rect.PtInRect(point))
				pTreeItem->m_lpTreeElem->m_bOpen = !pTreeItem->m_lpTreeElem->m_bOpen;
			else
				m_pFocusElem = pTreeItem->m_lpTreeElem;
		}
#endif // __IMPROVE_QUEST_INTERFACE
	}
}

void CWndTreeCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
}

void CWndTreeCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
}
void CWndTreeCtrl::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	LPTREEITEM pTreeItem;
	for(int i = 0; i < m_treeItemArray.GetSize(); i++)
	{
		pTreeItem = (LPTREEITEM)m_treeItemArray.GetAt(i);
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
		if( m_bMemberCheckingMode == TRUE )
		{
			CWndButton* pWndCheckBox = pTreeItem->m_lpTreeElem->m_pWndCheckBox;
			LPTREEELEM pParentTreeElem = pTreeItem->m_lpTreeElem->m_lpParent;
			if( pWndCheckBox && pParentTreeElem && CheckParentTreeBeOpened( pParentTreeElem ) == FALSE )
			{
				if( pWndCheckBox->IsWindowEnabled() == TRUE )
					pWndCheckBox->EnableWindow( FALSE );
				if( pWndCheckBox->IsVisible() == TRUE )
					pWndCheckBox->SetVisible( FALSE );
			}
		}
#endif // __IMPROVE_QUEST_INTERFACE
		if(pTreeItem->m_rect.PtInRect(point))
		{
			m_pFocusElem = pTreeItem->m_lpTreeElem;
			m_pFocusElem->m_bOpen = !m_pFocusElem->m_bOpen;
			CWndBase* pWnd = m_pParentWnd;
			while( pWnd->GetStyle() & WBS_CHILD )
				pWnd = pWnd->GetParentWnd();
			pWnd->OnChildNotify( WNM_DBLCLK, m_nIdWnd, (LRESULT*)m_pFocusElem );
		}
	}
}
void CWndTreeCtrl::OnRButtonDblClk( UINT nFlags, CPoint point)
{
}
void CWndTreeCtrl::OnSize(UINT nType, int cx, int cy)
{
	CRect rect = GetWindowRect();
	if( IsWndStyle( WBS_VSCROLL ) ) 
		rect.left = rect.right - VSCROLL_WIDTH;
	m_wndScrollBar.SetVisible( IsWndStyle( WBS_VSCROLL ) );
	m_wndScrollBar.SetWndRect( rect );
	CWndBase::OnSize( nType, cx, cy);
}
BOOL CWndTreeCtrl::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	if(m_wndScrollBar.GetScrollPage() < m_wndScrollBar.GetMaxScrollPos())	//스크롤이 활성화 되었을 때에만 동작하자.
	{
		if( zDelta < 0 )
		{
			if( m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() > m_wndScrollBar.GetScrollPos() )
				m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetScrollPos()+1 );
			else
				m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() );
		}
		else
		{
			if( m_wndScrollBar.GetMinScrollPos() < m_wndScrollBar.GetScrollPos() )
				m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetScrollPos()-1 );
			else
				m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetMinScrollPos() );
		}
	}
	
	return TRUE;
}
BOOL CWndTreeCtrl::OnEraseBkgnd( C2DRender* p2DRender )
{
	return TRUE;
}
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
tagScriptElem::tagScriptElem( void ) : 
m_lpParent( NULL ),
#if __VER >= 19
m_dwColor( D3DCOLOR_ARGB( 255, 255, 255, 255 ) ),
m_dwSelectColor( D3DCOLOR_ARGB( 255, 0, 255,  50 ) ),
#else
m_dwColor( D3DCOLOR_ARGB( 255, 64, 64, 64 ) ), 
m_dwSelectColor( D3DCOLOR_ARGB( 255, 0, 0, 255 ) ), 
#endif// __VER >= 19
m_strKeyword( _T( "" ) ), 
m_dwData( 0 ), 
m_bOpen( FALSE ), 
m_pWndCheckBox( NULL )
{}

tagScriptElem::tagScriptElem( CRect* pRect ) : 
m_lpParent( NULL ), 
#if __VER >= 19
m_dwColor( D3DCOLOR_ARGB( 255, 255, 255, 255 ) ),
m_dwSelectColor( D3DCOLOR_ARGB( 255, 0, 255,  50 ) ),
#else
m_dwColor( D3DCOLOR_ARGB( 255, 64, 64, 64 ) ), 
m_dwSelectColor( D3DCOLOR_ARGB( 255, 0, 0, 255 ) ), 
#endif// __VER >= 19
m_strKeyword( _T( "" ) ), 
m_dwData( 0 ), 
m_bOpen( FALSE ), 
m_pWndCheckBox( NULL )
{
	m_strKeyword.Init( CWndBase::m_Theme.m_pFontText, pRect );
}
#endif // __IMPROVE_QUEST_INTERFACE

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndScrollBar
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndScrollBar::CWndScrollBar()
{
	m_nScrollPos = 0;
	m_nScrollMinPos = 0;
	m_nScrollMaxPos = 0;
	m_nScrollPage = 0;
	//m_dwScrollStyle = 0;
	m_byWndType = WTYPE_SCROLLBAR;
}
CWndScrollBar::~CWndScrollBar()
{
}
BOOL CWndScrollBar::Create(DWORD dwStyle,const RECT& rect,CWndBase* pParentWnd,UINT nID)
{
	CRect rc = rect;
	rc.left = rc.right - 15;//VSCROLL_WIDTH;
	return CWndBase::Create( dwStyle | WBS_CHILD | WBS_DOCKING, rc, pParentWnd, nID );
}
int CWndScrollBar::GetScrollPos() const // Retrieves the current position of a scroll box. 
{
	return m_nScrollPos;
}
int CWndScrollBar::SetScrollPos(int nPos, BOOL bRedraw) // Sets the current position of a scroll box. 
{
	m_nScrollPos = nPos;
	return 1;
}
void CWndScrollBar::SetMinScrollPos() 
{
	m_nScrollPos = m_nScrollMinPos;
}
void CWndScrollBar::SetMaxScrollPos() 
{
	m_nScrollPos = m_nScrollMaxPos - m_nScrollPage;
	if( m_nScrollPos < 0 ) m_nScrollPos = 0;
}
int CWndScrollBar::GetMinScrollPos() 
{
	return m_nScrollMinPos;
}
int CWndScrollBar::GetMaxScrollPos() 
{
	return m_nScrollMaxPos;
}
int CWndScrollBar::GetScrollRange() const
{
	return m_nScrollMaxPos - m_nScrollMinPos;
}
int CWndScrollBar::GetScrollAbiliableRange() 
{
	CRect rect = GetClientRect();
	rect.top += m_wndArrow1.GetClientRect().Height(); 
	rect.bottom -= m_wndArrow2.GetClientRect().Height();
	int nHeight = rect.Height();
	int nPadHeight = ( m_nScrollPage * nHeight / GetScrollRange() );
	if( nPadHeight < 6 ) nPadHeight = 6;
	rect.bottom = rect.top + nPadHeight;
	// 역산해서 길이를 조정하자.
	return nHeight * m_nScrollPage / nPadHeight;
}
int CWndScrollBar::GetScrollAbiliablePos() 
{
	// Range : pos = AbiliableRange : AbiliablePos
	return m_nScrollPos * GetScrollAbiliableRange() / GetScrollRange();
}
int CWndScrollBar::GetScrollAbiliablePage() 
{
	// Range : page = AbiliableRange : AbiliablePage
	return m_nScrollPage * GetScrollAbiliableRange() / GetScrollRange();
}
void CWndScrollBar::GetScrollRange(LPINT lpMinPos, LPINT lpMaxPos) const // Retrieves the current minimum and maximum scroll-bar positions for the given scroll bar. 
{
	*lpMinPos = m_nScrollMinPos;
	*lpMinPos = m_nScrollMaxPos;
}
void CWndScrollBar::SetScrollRange(int nMinPos, int nMaxPos, BOOL bRedraw) // Sets minimum and maximum position values for the given scroll bar. 
{
	m_nScrollMinPos = nMinPos;
	m_nScrollMaxPos = nMaxPos;
	/*
	// auto hide
	if( GetScrollRange() - m_nScrollPage <= 0 )
		SetVisible( FALSE );
	else
		SetVisible( TRUE );
	*/
}
void CWndScrollBar::ShowScrollBar(BOOL bShow) // Shows or hides a scroll bar. 
{
}
BOOL CWndScrollBar::EnableScrollBar(UINT nArrowFlags) // Enables or disables one or both arrows of a scroll bar. 
{
	return TRUE;
}
BOOL CWndScrollBar::SetScrollInfo(LPSCROLLINFO lpScrollInfo, BOOL bRedraw) // Sets information about the scroll bar. 
{
	return TRUE;
}
BOOL CWndScrollBar::GetScrollInfo(LPSCROLLINFO lpScrollInfo, UINT nMask) // Retrieves information about the scroll bar. 
{
	return TRUE;
}
int CWndScrollBar::GetScrollLimit() // Retrieves the limit of the scroll bar
{
	return 1;
}
int CWndScrollBar::GetScrollPage() const
{
	return m_nScrollPage;
}	
void CWndScrollBar::SetScrollPage(int nPage)
{
	m_nScrollPage = nPage;
	/*
	// auto hide
	if( GetScrollRange() - m_nScrollPage <= 0 )
		SetVisible( FALSE );
	else
		SetVisible( TRUE );
	*/
}	
/*
CSprite* CWndScrollBar::GetSprVertArrow1()
{
	return m_pSprPack->GetAt(m_nScrollBarIdx+0);
}
CSprite* CWndScrollBar::GetSprVertArrow2()
{
	return m_pSprPack->GetAt(m_nScrollBarIdx+2);
}
CSprite* CWndScrollBar::GetSprVertPad()
{
	return m_pSprPack->GetAt(m_nScrollBarIdx+4);
}
CSprite* CWndScrollBar::GetSprVertBar()
{
	return m_pSprPack->GetAt(m_nScrollBarIdx+6);
}
*/
void CWndScrollBar::SetWndRect(CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	//m_rectClient.DeflateRect( 1, 1 );
	if( bOnSize )
		OnSize(0,m_rectClient.Width(),m_rectClient.Height());
}
void CWndScrollBar::PaintFrame(C2DRender* p2DRender)
{
	static const int TEXTURE_V_SCROLL_ARROW_SIZE_Y = 16;
	static const int TEXTURE_V_SCROLL_BAR_SIZE_Y = m_pTexButtVScrBar->m_size.cy;

	CRect rect = GetClientRect();
	CPoint point = rect.TopLeft();
	FLOAT fHeightSize = static_cast<FLOAT>( ( rect.Height() - ( TEXTURE_V_SCROLL_ARROW_SIZE_Y * 2 ) ) / TEXTURE_V_SCROLL_BAR_SIZE_Y );

	p2DRender->RenderTexture( CPoint( point.x, point.y + TEXTURE_V_SCROLL_ARROW_SIZE_Y ), m_pTexButtVScrBar, CWndBase::m_nAlpha, 1.0F, fHeightSize );
}
void CWndScrollBar::OnDraw(C2DRender* p2DRender)
{
	if( GetScrollRange() && GetScrollRange() - m_nScrollPage > 0 )
	{
		CRect rect = GetClientRect();
		rect.top += m_wndArrow1.GetClientRect().Height();
		rect.bottom -= m_wndArrow2.GetClientRect().Height();

		int nHeight = rect.Height();
		int nRange = GetScrollRange();
		int nPos = GetScrollPos();
		int nPage = GetScrollPage();

		int nPadHeight = ( nPage * nHeight / nRange );
		if( nPadHeight < 6 )
		{
			nHeight -= 6 - nPadHeight;
			nPadHeight = 6;
		}
		rect.bottom = rect.top + nPadHeight;
		int nPadPos =  ( nPos * nHeight / nRange );

		rect.OffsetRect( 0, nPadPos );

		static const int TEXTURE_SCROLL_P_UP_SIZE_Y = m_pTexButtVScrPUp->m_size.cy;
		static const int TEXTURE_SCROLL_P_BAR_SIZE_Y = m_pTexButtVScrPBar->m_size.cy;
		static const int TEXTURE_SCROLL_P_DOWN_SIZE_Y = m_pTexButtVScrPDown->m_size.cy;

		CPoint point = rect.TopLeft();
		FLOAT fHeightSize = static_cast<FLOAT>( ( rect.Height() / TEXTURE_SCROLL_P_BAR_SIZE_Y ) - 1 );

		if( nHeight > 0 )
		{
			p2DRender->RenderTexture( CPoint( point.x, point.y ), m_pTexButtVScrPUp );
			p2DRender->RenderTexture( CPoint( point.x, point.y + TEXTURE_SCROLL_P_UP_SIZE_Y ), m_pTexButtVScrPBar, CWndBase::m_nAlpha, 1.0F, fHeightSize );
			p2DRender->RenderTexture( CPoint( point.x, point.y + ( rect.Height() - TEXTURE_SCROLL_P_DOWN_SIZE_Y ) ), m_pTexButtVScrPDown );
		}
	}
}
//CTexturePack CWndScrollBar::m_texturePack;

void CWndScrollBar::OnInitialUpdate()
{
	//m_pTexButtVScrUp    = m_texturePack.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtVScrUp.bmp"    ), 0xffff00ff );
	//m_pTexButtVScrDown  = m_texturePack.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtVScrDown.bmp"  ), 0xffff00ff );
	m_pTexButtVScrBar   = m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtVScrBar.bmp"   ), 0xffff00ff );
	m_pTexButtVScrPUp   = m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtVScrPUp.bmp"   ), 0xffff00ff );
	m_pTexButtVScrPDown = m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtVScrPDown.bmp" ), 0xffff00ff );
	m_pTexButtVScrPBar  = m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtVScrPBar.bmp"  ), 0xffff00ff );

	CRect rect = GetClientRect();
	rect.bottom = rect.bottom / 4 * 4;
	//rect.DeflateRect( 1, 1 );
	//CSize size;// = m_pSprPack->GetAt(13+2)->GetSize();
	m_wndArrow1.Create( "",0,CRect( rect.left, rect.top, rect.right, rect.top + rect.Width()),this,1000);//,m_pSprPack,13+0);
	m_wndArrow1.SetTexture( m_pApp->m_pd3dDevice,MakePath( DIR_THEME, "ButtVScrUp.tga" ), TRUE );
	m_wndArrow1.FitTextureSize();
	//rect.top = rect.bottom - size.cy;
	rect.top = 0;
	m_wndArrow2.Create( "",0,CRect( rect.left, rect.bottom - rect.Width(), rect.right, rect.bottom),this,1001);//,m_pSprPack,13+2);
	m_wndArrow2.SetTexture( m_pApp->m_pd3dDevice,MakePath( DIR_THEME, "ButtVScrDown.tga" ), TRUE );
	m_wndArrow2.FitTextureSize();

	m_wndArrow1.SetPushTime(500);
	m_wndArrow2.SetPushTime(500);

	m_wndArrow1.DelWndStyle( WBS_NODRAWFRAME );
	m_wndArrow2.DelWndStyle( WBS_NODRAWFRAME );
	m_bPushPad = FALSE;
}
void CWndScrollBar::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	m_bPushPad = FALSE;
}
void CWndScrollBar::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	OnLButtonDown( nFlags, point );
}
void CWndScrollBar::OnLButtonDown(UINT nFlags, CPoint point)
{
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	if( GetScrollPage() >= GetMaxScrollPos() )
		return;
#endif // __IMPROVE_QUEST_INTERFACE

	SetCapture();
	CRect rect = GetClientRect();
	rect.top += m_wndArrow1.GetClientRect().Height();
	rect.bottom -= m_wndArrow2.GetClientRect().Height();
	if(rect.PtInRect(point) && GetScrollRange())
	{
		rect = GetClientRect();
		rect.top += m_wndArrow1.GetClientRect().Height(); 
		rect.bottom -= m_wndArrow2.GetClientRect().Height();
		int nHeight = rect.Height();
		int nRange = GetScrollRange();
		int nPos = GetScrollPos();
		int nPage = GetScrollPage();
		int nPadHeight = ( nPage * nHeight / nRange );
		if( nPadHeight < 6 )
		{
			nHeight -= 6 - nPadHeight;
			nPadHeight = 6;
		}
		rect.bottom = rect.top + nPadHeight;
		int nPadPos =  ( nPos * nHeight / nRange );
		rect.OffsetRect( 0, nPadPos );

		if( rect.PtInRect( point ) )
		{
			SetCapture();
			m_nPos = point.y - rect.top;
			m_bPushPad = TRUE;
		}
		else
		if( point.y < rect.top )
		{
			m_nScrollPos -= m_nScrollPage;
			if(m_nScrollPos < m_nScrollMinPos)
				m_nScrollPos = m_nScrollMinPos;
		}
		else
		if( point.y > rect.bottom )
		{
			m_nScrollPos += m_nScrollPage;
			if(m_nScrollPos > m_nScrollMaxPos - m_nScrollPage)
				m_nScrollPos = m_nScrollMaxPos - m_nScrollPage;
		}
	}
}
void CWndScrollBar::OnRButtonUp(UINT nFlags, CPoint point)
{
}
void CWndScrollBar::OnRButtonDown(UINT nFlags, CPoint point)
{
}
void CWndScrollBar::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_bPushPad)
	{
		point.y -= ( m_wndArrow1.GetClientRect().Height() + m_nPos );
  		CRect rect = GetClientRect();
		rect.top += m_wndArrow1.GetClientRect().Height(); 
		rect.bottom -= m_wndArrow2.GetClientRect().Height();
		int nHeight = rect.Height();
		// rect.Height() : pt.y = GetScrollRange() : x
		int nPadHeight = ( m_nScrollPage * nHeight / GetScrollRange() );
		if( nPadHeight < 6 ) 		
			nHeight -= 6 - nPadHeight;
		m_nScrollPos = point.y * GetScrollRange() / nHeight;
		if( m_nScrollPos < m_nScrollMinPos ) m_nScrollPos = m_nScrollMinPos;
		if( m_nScrollPos > m_nScrollMaxPos - m_nScrollPage ) m_nScrollPos = m_nScrollMaxPos - m_nScrollPage;
	}
}
BOOL CWndScrollBar::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	switch(nID)
	{
		case 1000: 
			if(--m_nScrollPos < m_nScrollMinPos)
				m_nScrollPos = m_nScrollMinPos;
			//TRACE("%d\n",m_nScrollPos);
			break;
		case 1001:
			if(++m_nScrollPos > m_nScrollMaxPos - m_nScrollPage)
				m_nScrollPos--;// = m_nScrollMaxPos - m_nScrollPage;
			//TRACE("%d\n",m_nScrollPos);
			break;
	}
	return TRUE;
}
void CWndScrollBar::SetScrollDown()
{
	if(++m_nScrollPos > m_nScrollMaxPos - m_nScrollPage)
		m_nScrollPos--;// = m_nScrollMaxPos - m_nScrollPage;
}
void CWndScrollBar::OnSize(UINT nType, int cx, int cy)
{
	CRect rect = GetClientRect();
	rect.bottom = rect.bottom / 4 * 4;

	m_wndArrow1.SetWndRect( CRect( rect.left, rect.top, rect.right, rect.top + rect.Width() ) );
	m_wndArrow2.SetWndRect( CRect( rect.left, rect.bottom - rect.Width(), rect.right, rect.bottom) );

//	m_string.Reset( m_pFont, &GetClientRect() );
	CWndBase::OnSize( nType, cx, cy);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndListBox
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndListBox::CWndListBox()
{
	m_nWndColor    = D3DCOLOR_ARGB( 255,  0x5 << 3,  0x5 << 3,  0x5 << 3 );
	//m_nFontColor   = D3DCOLOR_ARGB( 255, 255, 255, 255 );
	//m_nSelectColor = D3DCOLOR_ARGB( 255, 64, 64, 0 );

	CWndSelectServer* pWndSelecServ = (CWndSelectServer*)g_WndMng.GetWndBase(APP_SELECT_SERVER);

#if __VER >= 19 
	m_nFontColor   = D3DCOLOR_ARGB( 255, 255, 255, 255 );

    if( pWndSelecServ )
	{
	m_nSelectColor   = D3DCOLOR_ARGB( 255, 0, 255, 255 );
	m_dwOnMouseColor = D3DCOLOR_ARGB( 255, 255, 0, 255 );
	m_dwInvalidColor = D3DCOLOR_ARGB( 255, 170, 170, 170 );
	}
	else
	{
	m_nSelectColor   = D3DCOLOR_ARGB( 255, 0, 0, 250 );
	m_dwOnMouseColor = D3DCOLOR_ARGB( 255, 255, 157,   0 );
	m_dwInvalidColor = D3DCOLOR_ARGB( 255, 170, 170, 170 );
	}
#else

	m_nFontColor     = D3DCOLOR_ARGB( 255, 0, 0, 0 );
	m_nSelectColor   = D3DCOLOR_ARGB( 255, 64, 64,  255 );
	m_dwOnMouseColor = D3DCOLOR_ARGB( 255, 255, 128, 0 );
	m_dwInvalidColor = D3DCOLOR_ARGB( 255, 170, 170, 170 )

#endif// __VER >= 19

	m_nLeftMargin = 0;
	m_nCurSelect = -1;
	m_pFocusItem = NULL;
	m_byWndType = WTYPE_LISTBOX;
	m_nLineSpace = 1;
	m_strTexture = DEF_CTRL_TEXT;
	m_bTile = TRUE;
}
CWndListBox::~CWndListBox()
{
	ResetContent();
}
void CWndListBox::Create( DWORD dwListBoxStyle, RECT& rect, CWndBase* pParentWnd, UINT nID )
{
	//m_dwListBoxStyle = dwListBoxStyle;
	CWndBase::Create( dwListBoxStyle | WBS_CHILD, rect, pParentWnd, nID );
}
void CWndListBox::LoadListBoxScript(LPCTSTR lpFileName) 
{
	CScanner scanner;
 	if(scanner.Load(lpFileName) == FALSE)
		return;
	scanner.GetToken(); // keyword
	do {
		InterpriteScript(scanner,m_listItemArray);
		scanner.GetToken(); // keyword
	} while(scanner.tok != FINISHED);

}
void CWndListBox::InterpriteScript(CScanner& scanner,CPtrArray& ptrArray) 
{
	do {
		LPLISTITEM lpListItem = new LISTITEM;
		lpListItem->m_strWord = scanner.Token;
		ptrArray.Add(lpListItem);
		scanner.GetToken(); 
	} while(*scanner.token != '}' && scanner.tok != FINISHED);
	if(scanner.tok == FINISHED)
		return;
	scanner.GetToken(); 
}
void CWndListBox::OnInitialUpdate()
{
	CRect rect = GetWindowRect();

	m_wndScrollBar.Create( WBS_DOCKING | WBS_VERT, rect, this, 1000 );
	m_wndScrollBar.SetVisible( IsWndStyle( WBS_VSCROLL ) );
}
void CWndListBox::OnDraw(C2DRender* p2DRender)
{
	CWndBase* pWnd = GetParentWnd();
	m_nFontHeight = GetFontHeight() + m_nLineSpace;
	{
		CPoint pt(3,3);
		pt.y -= m_nFontHeight * m_wndScrollBar.GetScrollPos();
		PaintListBox(p2DRender,pt,m_listItemArray);

		int nPage = GetClientRect().Height() / m_nFontHeight;
		int nRange = m_listItemArray.GetSize();
		if(	IsWndStyle( WBS_VSCROLL ) )
		{
			m_wndScrollBar.SetVisible( TRUE );
			m_wndScrollBar.SetScrollRange( 0, nRange );
			m_wndScrollBar.SetScrollPage( nPage );
		}
		else
			m_wndScrollBar.SetVisible( FALSE );
	}
}

void CWndListBox::PaintListBox(C2DRender* p2DRender,CPoint& pt,CPtrArray& ptrArray) 
{
	LPLISTITEM pListItem;
	for(int i = 0; i < ptrArray.GetSize(); i++)
	{
		pListItem = (LPLISTITEM)ptrArray.GetAt(i);
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
		if( pListItem->m_bIsVisible == FALSE )
			continue;

		int nScrollBarWidth = IsWndStyle( WBS_VSCROLL ) ? m_wndScrollBar.GetClientRect().Width() : 0;
		CSize size = p2DRender->m_pFont->GetTextExtent_EditString( pListItem->m_strWord );
		pListItem->m_rect.left = pt.x;
		pListItem->m_rect.top = pt.y;
		pListItem->m_rect.right = pt.x + m_rectWindow.Width() - nScrollBarWidth;
		pListItem->m_rect.bottom = pt.y + m_nFontHeight;
		if( pListItem->m_bIsValid == TRUE )
		{
			if( i == m_nCurSelect )
				pListItem->m_strWord.SetColor( m_nSelectColor );
			else
			{
				CPoint point = GetMousePoint();
				if( pListItem->m_rect.PtInRect( point ) == TRUE )
					pListItem->m_strWord.SetColor( m_dwOnMouseColor );
				else
					pListItem->m_strWord.SetColor( m_nFontColor );
			}
		}
		else
			pListItem->m_strWord.SetColor( m_dwInvalidColor );
		p2DRender->TextOut_EditString( m_nLeftMargin + pt.x, pt.y, pListItem->m_strWord );
		pt.y += m_nFontHeight;
#else // __IMPROVE_QUEST_INTERFACE
		if( i == m_nCurSelect ) //m_pFocusItem == pListItem)
		{
			p2DRender->SetTextColor(m_nSelectColor);
			p2DRender->TextOut(pt.x,pt.y,pListItem->m_strWord,m_nSelectColor);
		}
		else
		{
			p2DRender->SetTextColor(m_nFontColor);
			p2DRender->TextOut(pt.x,pt.y,pListItem->m_strWord,m_nFontColor);
		}
		pt.y += m_nFontHeight;
#endif // __IMPROVE_QUEST_INTERFACE
	}
}
void CWndListBox::OnLButtonUp(UINT nFlags, CPoint point)
{
	CWndBase* pWnd = GetParentWnd();
	{
		CPoint pt(3,3);
		pt.y -= m_nFontHeight * m_wndScrollBar.GetScrollPos();
		CRect rect;
		LPLISTITEM pListItem;
		for(int i = 0; i < m_listItemArray.GetSize(); i++)
		{
			pListItem = (LPLISTITEM)m_listItemArray.GetAt(i);
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
			if( pListItem->m_bIsValid == FALSE )
			{
				pt.y += m_nFontHeight;
				continue;
			}
#endif // __IMPROVE_QUEST_INTERFACE
			int nScrollBarWidth = IsWndStyle( WBS_VSCROLL ) ? m_wndScrollBar.GetClientRect().Width() : 0;
			rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - nScrollBarWidth, pt.y + m_nFontHeight );
			if(rect.PtInRect(point))
			{
				if(m_pFocusItem == pListItem)
				{
					// 부모가 차일드 윈도가 아니어야 OnCommand 메시지를 받는다.
					CWndBase* pWnd = m_pParentWnd;
					pWnd->OnChildNotify(WNM_SELCHANGE,m_nIdWnd,(LRESULT*)pListItem);
					return;
				}
			}
			pt.y += m_nFontHeight;
		}
	}
}
void CWndListBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	CPoint pt(3,3);
	pt.y -= m_nFontHeight * m_wndScrollBar.GetScrollPos();
	CRect rect;
	LPLISTITEM pListItem;
	for(int i = 0; i < m_listItemArray.GetSize(); i++)
	{
		pListItem = (LPLISTITEM)m_listItemArray.GetAt(i);
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
		if( pListItem->m_bIsValid == FALSE )
		{
			pt.y += m_nFontHeight;
			continue;
		}
#endif // __IMPROVE_QUEST_INTERFACE
		int nScrollBarWidth = IsWndStyle( WBS_VSCROLL ) ? m_wndScrollBar.GetClientRect().Width() : 0;
		rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - nScrollBarWidth, pt.y + m_nFontHeight );
		if(rect.PtInRect(point))
		{
			m_nCurSelect = i;
			m_pFocusItem = pListItem;
		}
		pt.y += m_nFontHeight;
	}
}
void CWndListBox::OnRButtonUp(UINT nFlags, CPoint point)
{
	CPoint pt(3,3);
	pt.y -= m_nFontHeight * m_wndScrollBar.GetScrollPos();
	CRect rect;
	LPLISTITEM pListItem;
	for(int i = 0; i < m_listItemArray.GetSize(); i++)
	{
		pListItem = (LPLISTITEM)m_listItemArray.GetAt(i);
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
		if( pListItem->m_bIsValid == FALSE )
		{
			pt.y += m_nFontHeight;
			continue;
		}
#endif // __IMPROVE_QUEST_INTERFACE
		int nScrollBarWidth = IsWndStyle( WBS_VSCROLL ) ? m_wndScrollBar.GetClientRect().Width() : 0;
		rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - nScrollBarWidth, pt.y + m_nFontHeight );
		if(rect.PtInRect(point))
		{
			if(m_pFocusItem == pListItem)
			{
				// 부모가 차일드 윈도가 아니어야 OnCommand 메시지를 받는다.
				CWndBase* pWnd = m_pParentWnd;
				pListItem->m_rect.left = point.x;
				pListItem->m_rect.top = point.y;				
				//while(pWnd->GetStyle() & WBS_CHILD)
				//	pWnd = pWnd->GetParentWnd();
				pWnd->OnChildNotify(WNM_SELCANCEL,m_nIdWnd,(LRESULT*)pListItem);//m_pFocusItem); 
				return;
			}
		}
		pt.y += m_nFontHeight;
	}
}
void CWndListBox::OnRButtonDown(UINT nFlags, CPoint point)
{
}
void CWndListBox::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	CPoint pt(3,3);
	pt.y -= m_nFontHeight * m_wndScrollBar.GetScrollPos();
	CRect rect;
	LPLISTITEM pListItem;
	for(int i = 0; i < m_listItemArray.GetSize(); i++)
	{
		pListItem = (LPLISTITEM)m_listItemArray.GetAt(i);
		int nScrollBarWidth = IsWndStyle( WBS_VSCROLL ) ? m_wndScrollBar.GetClientRect().Width() : 0;
		rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - nScrollBarWidth, pt.y + m_nFontHeight );
		if(rect.PtInRect(point))
		{
			if(m_pFocusItem == pListItem)
			{
				// 부모가 차일드 윈도가 아니어야 OnCommand 메시지를 받는다.
				CWndBase* pWnd = m_pParentWnd;
				while(pWnd->GetStyle() & WBS_CHILD)
					pWnd = pWnd->GetParentWnd();
				pWnd->OnChildNotify(WNM_DBLCLK,m_nIdWnd,(LRESULT*)m_pFocusItem); 
				return;
			}
		}
		pt.y += m_nFontHeight;
	}
}
void CWndListBox::OnRButtonDblClk( UINT nFlags, CPoint point)
{
}
BOOL CWndListBox::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	if( m_wndScrollBar.GetScrollPage() >= m_wndScrollBar.GetMaxScrollPos() )
		return TRUE;
#endif // __IMPROVE_QUEST_INTERFACE
	if( zDelta < 0 )
	{
		if( m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() > m_wndScrollBar.GetScrollPos() )
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetScrollPos()+1 );
		else
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() );
	}
	else
	{
		if( m_wndScrollBar.GetMinScrollPos() < m_wndScrollBar.GetScrollPos() )
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetScrollPos()-1 );
		else
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetMinScrollPos() );
	}
	
	return TRUE;
}

int CWndListBox::GetCount() const
{
	return m_listItemArray.GetSize();
}
DWORD CWndListBox::GetItemData(int nIndex) const
{
	LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
	return lpListItem->m_dwData;
}

void* CWndListBox::GetItemDataPtr(int nIndex) const
{
	LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
	return (void*)lpListItem->m_dwData;
}

int CWndListBox::SetItemData(int nIndex,DWORD dwItemData)
{
	if(nIndex < 0 || nIndex >= m_listItemArray.GetSize())
		return LB_ERR;
	LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
	lpListItem->m_dwData = dwItemData;
	return 0;
}

int CWndListBox::SetItemDataPtr(int nIndex,void* pData)
{
	if(nIndex < 0 || nIndex >= m_listItemArray.GetSize())
		return LB_ERR;
	LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
	lpListItem->m_dwData = (DWORD) pData;
	return 0;
}

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
DWORD CWndListBox::GetItemData2( int nIndex ) const
{
	LPLISTITEM lpListItem = ( LPLISTITEM )m_listItemArray.GetAt( nIndex );
	return lpListItem->m_dwData2;
}

void* CWndListBox::GetItemData2Ptr( int nIndex ) const
{
	LPLISTITEM lpListItem = ( LPLISTITEM )m_listItemArray.GetAt( nIndex );
	return ( void* )lpListItem->m_dwData2;
}

BOOL CWndListBox::GetItemValidity( int nIndex )
{
	LPLISTITEM lpListItem = ( LPLISTITEM )m_listItemArray.GetAt( nIndex );
	return lpListItem->m_bIsValid;
}

BOOL CWndListBox::GetItemVisibility( int nIndex )
{
	LPLISTITEM lpListItem = ( LPLISTITEM )m_listItemArray.GetAt( nIndex );
	return lpListItem->m_bIsVisible;
}

int CWndListBox::SetItemData2( int nIndex,DWORD dwItemData )
{
	if( nIndex < 0 || nIndex >= m_listItemArray.GetSize() )
		return LB_ERR;
	LPLISTITEM lpListItem = ( LPLISTITEM )m_listItemArray.GetAt( nIndex );
	lpListItem->m_dwData2 = dwItemData;
	return 0;
}

int CWndListBox::SetItemData2Ptr( int nIndex,void* pData )
{
	if( nIndex < 0 || nIndex >= m_listItemArray.GetSize() )
		return LB_ERR;
	LPLISTITEM lpListItem = ( LPLISTITEM )m_listItemArray.GetAt( nIndex );
	lpListItem->m_dwData2 = ( DWORD )pData;
	return 0;
}

int CWndListBox::SetItemValidity( int nIndex, BOOL bValidity )
{
	if( nIndex < 0 || nIndex >= m_listItemArray.GetSize() )
		return LB_ERR;
	LPLISTITEM lpListItem = ( LPLISTITEM )m_listItemArray.GetAt( nIndex );
	lpListItem->m_bIsValid = bValidity;
	return 0;
}

int CWndListBox::SetItemVisibility( int nIndex, BOOL bIsVisible )
{
	if( nIndex < 0 || nIndex >= m_listItemArray.GetSize() )
		return LB_ERR;
	LPLISTITEM lpListItem = ( LPLISTITEM )m_listItemArray.GetAt( nIndex );
	lpListItem->m_bIsVisible = bIsVisible;
	return 0;
}
#endif // __IMPROVE_QUEST_INTERFACE

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
const CRect& CWndListBox::GetItemRect( int nIndex ) const
{
	LPLISTITEM lpListItem = ( LPLISTITEM )m_listItemArray.GetAt( nIndex );
	return lpListItem->m_rect;
}
#else // __IMPROVE_QUEST_INTERFACE
int CWndListBox::GetItemRect(int nIndex,LPRECT lpRect) const
{
		return 1;
}
#endif // __IMPROVE_QUEST_INTERFACE

int CWndListBox::GetSel(int nIndex) const
{
		return 1;
}

int CWndListBox::GetText(int nIndex,LPSTR lpszBuffer) const
{
		return 1;
}

void CWndListBox::GetText(int nIndex,CString& rString) const
{
	if(nIndex >= 0 && nIndex < m_listItemArray.GetSize())
	{
		LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
		rString = lpListItem->m_strWord;
	}
}

#ifdef __IMPROVE_MAP_SYSTEM
int CWndListBox::GetTextLen(int nIndex) const
{
	if(nIndex >= 0 && nIndex < m_listItemArray.GetSize())
	{
		LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
		return lpListItem->m_strWord.GetLength();
	}
	return 0;
}
#else // __IMPROVE_MAP_SYSTEM
int CWndListBox::GetTextLen(int nIndex) const
{
		return 1;
}
#endif // __IMPROVE_MAP_SYSTEM


int CWndListBox::GetCurSel() const
{
	return m_nCurSelect;
}

int CWndListBox::SetCurSel(int nSelect)
{
	if(nSelect >= 0 && nSelect < m_listItemArray.GetSize())
	{	
		m_pFocusItem = (LPLISTITEM)m_listItemArray.GetAt(nSelect);
		m_nCurSelect = nSelect;
		return m_nCurSelect;
	}

	Error( "CWndListBox::SetCurSel(int nSelect) 범위 넘어섬 %d", nSelect );

	return 0;
}

int CWndListBox::SetSel(int nIndex,BOOL bSelect)
{
		return 1;
}

int CWndListBox::GetSelCount() const
{
		return 1;
}

int CWndListBox::GetSelItems(int nMaxItems,LPINT rgIndex) const
{
		return 1;
}


int CWndListBox::AddString(LPCTSTR lpszItem)
{
	LPLISTITEM lpListItem = new LISTITEM;
	lpListItem->m_strWord = lpszItem;
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	lpListItem->m_strWord.Init( m_pFont, &GetClientRect() );
	lpListItem->m_strWord.SetParsingString( lpszItem, m_nFontColor, 0x00000000, 0, 0x00000001, TRUE );
#endif // __IMPROVE_QUEST_INTERFACE
	m_listItemArray.Add(lpListItem);
	return m_listItemArray.GetSize()-1;
}

int CWndListBox::DeleteString(UINT nIndex)
{
	safe_delete( (LPLISTITEM)m_listItemArray.GetAt(nIndex) );
	m_listItemArray.RemoveAt(nIndex);
	if(nIndex == m_nCurSelect)
	{
		m_nCurSelect = -1;
		m_pFocusItem = NULL;
	}
	return 1;
}

void CWndListBox::SetString( int nIndex, LPCTSTR lpszItem )
{
	LPLISTITEM lpListItem	= (LPLISTITEM)m_listItemArray.GetAt( nIndex );
	lpListItem->m_strWord	= lpszItem;
}

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
const CString& CWndListBox::GetString( int nIndex ) const
{
	LPLISTITEM lpListItem	= ( LPLISTITEM )m_listItemArray.GetAt( nIndex );
	return lpListItem->m_strWord;
}

void CWndListBox::SetListStringAlpha( int nIndex, BYTE byAlpha )
{
	LPLISTITEM lpListItem	= ( LPLISTITEM )m_listItemArray.GetAt( nIndex );
	lpListItem->m_strWord.SetAlpha( byAlpha );
}

void CWndListBox::SetKeyString( int nIndex, LPCTSTR lpszItem )
{
	LPLISTITEM lpListItem	= ( LPLISTITEM )m_listItemArray.GetAt( nIndex );
	lpListItem->m_strKey	= lpszItem;
}

const CString& CWndListBox::GetKeyString( int nIndex ) const
{
	LPLISTITEM lpListItem	= ( LPLISTITEM )m_listItemArray.GetAt( nIndex );
	return lpListItem->m_strKey;
}

void CWndListBox::SetOnMouseColor( DWORD dwOnMouseColor )
{
	m_dwOnMouseColor = dwOnMouseColor;
}

DWORD CWndListBox::GetOnMouseColor( void ) const
{
	return m_dwOnMouseColor;
}

void CWndListBox::SetInvalidColor( DWORD dwInvalidColor )
{
	m_dwInvalidColor = dwInvalidColor;
}

DWORD CWndListBox::GetInvalidColor( void ) const
{
	return m_dwInvalidColor;
}

void CWndListBox::SetLeftMargin( int nLeftMargin )
{
	m_nLeftMargin = nLeftMargin;
}

int CWndListBox::GetLeftMargin( void ) const
{
	return m_nLeftMargin;
}
#endif // __IMPROVE_QUEST_INTERFACE

#ifdef __IMPROVE_MAP_SYSTEM
int CWndListBox::GetItemIndex( const CString& strItem ) const
{
	for( int nIndex = 0; nIndex < m_listItemArray.GetSize(); ++nIndex )
	{
		LPLISTITEM lpListItem = ( LPLISTITEM )m_listItemArray.GetAt( nIndex );
		if( lpListItem == NULL )
		{
			continue;
		}

		if( strcmp( strItem, lpListItem->m_strWord ) == 0 )
		{
			return nIndex;
		}
	}

	return -1;
}

int CWndListBox::GetItemIndex( DWORD dwItem ) const
{
	for( int nIndex = 0; nIndex < m_listItemArray.GetSize(); ++nIndex )
	{
		LPLISTITEM lpListItem = ( LPLISTITEM )m_listItemArray.GetAt( nIndex );
		if( lpListItem == NULL )
		{
			continue;
		}

		if( dwItem == lpListItem->m_dwData )
		{
			return nIndex;
		}
	}

	return -1;
}
#endif // __IMPROVE_MAP_SYSTEM

int CWndListBox::InsertString(int nIndex,LPCTSTR lpszItem)
{
		return 1;
} 

void CWndListBox::ResetContent()
{
	for(int i = 0; i < m_listItemArray.GetSize(); i++)
		safe_delete( (LPLISTITEM)m_listItemArray.GetAt(i) );
	m_nCurSelect = -1;
	m_pFocusItem = NULL;
	m_listItemArray.RemoveAll();
	m_wndScrollBar.SetScrollPos( 0, FALSE );
}

int CWndListBox::FindString( int nStartAfter, LPCTSTR lpszItem ) const
{
	if( m_listItemArray.GetSize() == 0 )
		return -1;
	LPLISTITEM lpListItem;
	int nCmp = 0;
	int nBegin = 0, nEnd = m_listItemArray.GetSize() - 1;
	int nDiv = nBegin + ( ( nEnd - nBegin ) / 2 );
	do
	{
		if( nCmp ==  1 ) nEnd = nDiv;
		if( nCmp == -1 ) nBegin = nDiv;
		nDiv = nBegin + ( ( nEnd - nBegin ) / 2 );
		lpListItem = (LPLISTITEM)m_listItemArray.GetAt( nDiv );
		nCmp = strcmp( lpListItem->m_strWord, lpszItem );
		if( nCmp ==  0 ) return nDiv;
	}
	while( ( nEnd - nBegin ) > 1 );
	int nResult;
	lpListItem = (LPLISTITEM)m_listItemArray.GetAt( nBegin );
	nCmp = strcmp( lpListItem->m_strWord, lpszItem );
	if( nCmp >= 0 ) 
		nResult = nBegin;
	else
	{
		lpListItem = (LPLISTITEM)m_listItemArray.GetAt( nEnd );
		nResult = nEnd;
	}
	if( lpszItem[ 0 ] != lpListItem->m_strWord[ 0 ] )
		nResult = -1;
	return nResult;
}
/*

input d 

0 5 = 2
2 5 = 3
2 3 = 2

0 a
1 b
2 c
3 f
4 f
5 f

0 5 = 2
2 5 = 3
3 5 = 4
3 4 = 3

0 a
1 b
2 c
3 c
4 f
5 f
*/
int QSortListBox( const VOID* arg1, const VOID* arg2 )
{
	//return _stricmp( * ( char** ) arg1, * ( char** ) arg2 );
	LPCTSTR pSrc = (*(CWndListBox::LISTITEM**)arg1)->m_strWord;
    LPCTSTR pDst = (*(CWndListBox::LISTITEM**)arg2)->m_strWord;

	return _stricmp( pSrc, pDst );
}
void CWndListBox::SortListBox() 
{
    qsort( m_listItemArray.GetData(), m_listItemArray.GetSize(), sizeof(LPLISTITEM), QSortListBox ); 
}
int CWndListBox::FindStringExact(int nIndexStart,LPCTSTR lpszItem) const
{
	return 1;
}

int CWndListBox::SelectString(int nStartAfter,LPCTSTR lpszItem)
{
	m_nCurSelect = FindString( nStartAfter, lpszItem );
	if( m_nCurSelect == -1 )
		return -1;
	int nPos = m_wndScrollBar.GetScrollPos();
	int nRange = m_wndScrollBar.GetScrollRange();
	int nPage = m_wndScrollBar.GetScrollPage();
	if( ( m_nCurSelect < nPos || m_nCurSelect > nPos + nPage ) )
	{
		if( m_nCurSelect < nRange - nPage )
			m_wndScrollBar.SetScrollPos( m_nCurSelect < 0 ? 0 : m_nCurSelect );
		else
			m_wndScrollBar.SetScrollPos( nRange - nPage  );
	}
	return m_nCurSelect;
}
void CWndListBox::OnSize(UINT nType, int cx, int cy)
{
//	m_string.Reset( m_pFont, &GetClientRect() );

	CRect rect = GetWindowRect();
	if( IsWndStyle( WBS_VSCROLL ) ) 
		rect.left = rect.right - VSCROLL_WIDTH;
	m_wndScrollBar.SetVisible( IsWndStyle( WBS_VSCROLL ) );
	m_wndScrollBar.SetWndRect( rect );
	CWndBase::OnSize( nType, cx, cy);
}
void CWndListBox::SetWndRect(CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect( 4, 4 );
	if( IsWndStyle( WBS_VSCROLL ) ) 
		m_rectClient.right -= 15;
	m_wndScrollBar.SetVisible( IsWndStyle( WBS_VSCROLL ) );

	if( bOnSize )
		OnSize(0,m_rectClient.Width(),m_rectClient.Height());
}
void CWndListBox::PaintFrame( C2DRender* p2DRender )
{
	CWndBase::PaintFrame( p2DRender );
}
BOOL CWndListBox::OnEraseBkgnd( C2DRender* p2DRender )
{
	return TRUE;
}
void CWndListBox::OnSetFocus( CWndBase* pOldWnd )
{
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndScrollBar
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndText::CWndText()
{
	m_dwOffset = 0;
	m_dwBlockBegin = 0;
	m_dwBlockEnd = 0;
	m_bLButtonDown = FALSE;
	m_timerCaret.Set(1000);
	m_bCaretVisible = FALSE;
	m_ptCaret = 0;
	m_nLineSpace	= 5;
	if( GetLanguage() == LANG_FRE )
		m_nLineSpace	= 0;
	m_byWndType = WTYPE_TEXT;
	m_bEnableClipboard = FALSE;
	memset(m_szCaret, 0, sizeof(char) * 3);
	
	m_szCaret[0] = 0;
	m_strTexture = DEF_CTRL_TEXT;
	m_bTile = TRUE;
	m_ptDeflateRect = CPoint( 6, 6 );
}
CWndText::~CWndText()
{
}
BOOL CWndText::Create(DWORD dwStyle,const RECT& rect,CWndBase* pParentWnd,UINT nID)
{
	return CWndBase::Create( dwStyle | WBS_CHILD, rect, pParentWnd, nID );
}
void CWndText::OnInitialUpdate()
{
	m_wndMenu.CreateMenu( this );	

	if( GetLanguage() == LANG_FRE )
	{
		CRect rect	= GetWndRect();
		rect.bottom	+= 2;
		SetWndRect( rect, TRUE );
	}

	m_string.Init( m_pFont, &GetClientRect() );
	CRect rect = GetClientRect();
	m_wndScrollBar.Create( WBS_DOCKING | WBS_VERT, rect, this, 1000 );
	UpdateScrollBar();
	m_wndScrollBar.SetVisible( IsWndStyle( WBS_VSCROLL ) );
	
	m_bScr = TRUE;
	m_nLineRefresh = 0;
}
void CWndText::OnSize( UINT nType, int cx, int cy )
{
	CRect rect = GetClientRect();
	m_string.Init( m_pFont, &rect );
	rect = GetWindowRect();
	if( IsWndStyle( WBS_VSCROLL ) ) 
		rect.left = rect.right - VSCROLL_WIDTH;
	m_wndScrollBar.SetVisible( IsWndStyle( WBS_VSCROLL ) );
	m_wndScrollBar.SetWndRect( rect );
	UpdateScrollBar();
	CWndBase::OnSize( nType, cx, cy);
}
void CWndText::SetWndRect(CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	if( !IsWndStyle( WBS_NOFRAME ) )
		m_rectClient.DeflateRect( m_ptDeflateRect.x, m_ptDeflateRect.y );

	if( IsWndStyle( WBS_VSCROLL ) ) 
		m_rectClient.right -= 15;
	m_wndScrollBar.SetVisible( IsWndStyle( WBS_VSCROLL ) );
	if( bOnSize )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
	CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret );
	SetCaretPos( ptCaret );
	
}


void CWndText::PaintFrame( C2DRender* p2DRender )
{
}
void CWndText::OnDraw( C2DRender* p2DRender )
{
	CString string;
	DWORD dwMaxHeight = GetFontHeight();
	DWORD dwOffset = 0;
	TCHAR strHan[3];
	DWORD dwBegin = 0;
	DWORD dwCurOffset = 0;
	DWORD dwOffsetLine = m_string.OffsetToLine( m_dwOffset );
	CPoint ptCaret = 0;
	DWORD dwLineCount = m_string.GetLineCount();
	CD3DFont* pOldFont = p2DRender->GetFont();
	p2DRender->SetFont( m_pFont );
	
	int nPos = IsWndStyle( WBS_VSCROLL ) ? m_wndScrollBar.GetScrollPos() : 0;
	int nLines = IsWndStyle( WBS_VSCROLL ) ? m_wndScrollBar.GetScrollPage() : GETRANGE;// GetClientRect().Height() + m_nLineSpace ) / GetFontHeight();

	int nPage = GETRANGE;//( GetClientRect().Height() + m_nLineSpace ) / GetFontHeight();
	int nRange = m_string.GetLineCount();

	if(	IsWndStyle( WBS_VSCROLL ) ) 
	{
		m_wndScrollBar.SetVisible( TRUE );
		m_wndScrollBar.SetScrollRange( 0, nRange );
		m_wndScrollBar.SetScrollPage( nPage );

		if( GetParentWnd()->m_nIdWnd == APP_MESSAGE_NOTE )
		{
			SetWndRect( GetWindowRect( TRUE ), FALSE);
			if( m_nLineRefresh < (int)( m_string.GetLineCount() ) )
			{
				m_wndScrollBar.SetScrollDown();
				m_nLineRefresh = m_string.GetLineCount();
			}
		}
	}
	else
		m_wndScrollBar.SetVisible( FALSE );

	if( IsWndStyle( EBS_PASSWORD ) )
	{
		for(int i = nPos, y = 0; i < (int)( dwLineCount ); i++, y++)
		{
			dwOffset = m_string.GetLineOffset( i );
			string = m_string.GetLine(i);
			dwBegin = 0;
			int nLength = string.GetLength();
			const char* begin = string;
			const char* end = begin + string.GetLength();
			const char* iter = begin;

			while(*iter) {

				WORD wCodePage = m_string.m_awCodePage[dwCurOffset];

				dwCurOffset = dwOffset + (iter - begin);

				const char* next = CharNextEx( iter, wCodePage );

				strHan[0] = '*';
				strHan[1] = 0;

				CSize size;
				p2DRender->m_pFont->GetTextExtent( "*", &size );
				size.cx *= next - iter;

				DWORD dwColor = m_string.m_adwColor[dwCurOffset];
				DWORD dwStyle = m_string.m_abyStyle[dwCurOffset];

				if( m_bEnableClipboard )
				{
					DWORD dwBlockBegin;
					DWORD dwBlockEnd;
					if(m_dwBlockBegin > m_dwBlockEnd)
						dwBlockBegin = m_dwBlockEnd, dwBlockEnd = m_dwBlockBegin;
					else
						dwBlockBegin = m_dwBlockBegin, dwBlockEnd = m_dwBlockEnd;


					if(dwCurOffset >= dwBlockBegin && dwCurOffset < dwBlockEnd)
					{
						DWORD dwBkgr = dwColor;
						dwColor = ~dwColor | 0xff000000;//0xffffffff;//ADEILSON 
						p2DRender->RenderFillRect(CRect( dwBegin, 0 + y * dwMaxHeight, dwBegin + size.cx, 0 + y * dwMaxHeight + size.cy), dwBkgr );
					}
				}
				p2DRender->TextOut( dwBegin, 0 + y * dwMaxHeight, strHan, dwColor ); 
				if( dwStyle & ESSTY_BOLD )
					p2DRender->TextOut( dwBegin + 1, 0 + y * dwMaxHeight, strHan, dwColor ); 
				if( dwStyle & ESSTY_UNDERLINE )
					p2DRender->RenderLine( CPoint( dwBegin, y * dwMaxHeight + size.cy ), CPoint( dwBegin + size.cx, y * dwMaxHeight + size.cy ), dwColor );
				

				// 문장 내에서 캐럿과 위치가 같다면 출력 위치다.
				if(dwCurOffset == m_dwOffset)
				{
					strcpy( m_szCaret, strHan );
					ptCaret = CPoint( dwBegin, 0 + y * dwMaxHeight );
				}
				dwBegin += size.cx;

				iter = next;
			}
		}
		 //m_ptCaret = ptCaret; 
		 //m_timerCaret.Reset(); 
	}
	else
	{
		{
			if( nLines == 6 )
			{
				int a = 0;
			}
			BlockSetStyle(ESSTY_BLOCK);
			//ptCaret = OffsetToPoint( m_dwOffset, m_szCaret );
			p2DRender->TextOut_EditString( 0, 0, m_string, nPos, nLines, m_nLineSpace );
		}
	}
	//SetCaretPos( ptCaret );
	DrawCaret( p2DRender );
	p2DRender->SetFont( pOldFont );
}

void CWndText::BlockSetStyle(DWORD dwStyle)
{
	if( m_bEnableClipboard )
	{
		DWORD dwBlockBegin;
		DWORD dwBlockEnd;
		if(m_dwBlockBegin > m_dwBlockEnd) 
			dwBlockBegin = m_dwBlockEnd, dwBlockEnd = m_dwBlockBegin;
		else
			dwBlockBegin = m_dwBlockBegin, dwBlockEnd = m_dwBlockEnd;
		if( dwBlockEnd - dwBlockBegin )
		{
			m_string.SetStyle( dwBlockBegin, dwBlockEnd - dwBlockBegin, dwStyle );
		}
	}	
}

void CWndText::BlockSetColor( DWORD dwColor )
{
	if( m_bEnableClipboard )
	{
		DWORD dwBlockBegin;
		DWORD dwBlockEnd;
		if(m_dwBlockBegin > m_dwBlockEnd) 
			dwBlockBegin = m_dwBlockEnd, dwBlockEnd = m_dwBlockBegin;
		else
			dwBlockBegin = m_dwBlockBegin, dwBlockEnd = m_dwBlockEnd;
		if( dwBlockEnd - dwBlockBegin )
		{
			m_string.SetColor( dwBlockBegin, dwBlockEnd - dwBlockBegin, dwColor );
		}
	}	
}


void CWndText::BlockClearStyle(DWORD dwStyle)
{
	if( m_bEnableClipboard )
	{
		DWORD dwBlockBegin;
		DWORD dwBlockEnd;
		if(m_dwBlockBegin > m_dwBlockEnd) 
			dwBlockBegin = m_dwBlockEnd, dwBlockEnd = m_dwBlockBegin;
		else
			dwBlockBegin = m_dwBlockBegin, dwBlockEnd = m_dwBlockEnd;
		if( dwBlockEnd - dwBlockBegin )
		{
			m_string.ClearStyle( dwBlockBegin, dwBlockEnd - dwBlockBegin, dwStyle );
		}
	}	
}

void CWndText::UpdateScrollBar()
{
	if(	IsWndStyle( WBS_VSCROLL ) )
	{

		int nPage = GETRANGE;//( GetClientRect().Height() + m_nLineSpace ) / GetFontHeight();
		int nRange = m_string.GetLineCount();// - nPage;

		if(	IsWndStyle( WBS_VSCROLL ) )  
		{
			m_wndScrollBar.SetVisible( TRUE );
			m_wndScrollBar.SetScrollRange( 0, nRange );
			m_wndScrollBar.SetScrollPage( nPage );
		}
		else
			m_wndScrollBar.SetVisible( FALSE );

		if( m_wndScrollBar.GetScrollPos() > ( m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() ) )
			m_wndScrollBar.SetMaxScrollPos();
	}

}
CPoint CWndText::OffsetToPoint( DWORD dwSetOffset, TCHAR* pszStr )
{
	CString string;
	DWORD dwMaxHeight = GetFontHeight();
	DWORD dwOffset = 0;
	TCHAR strHan[3];
	DWORD dwBegin = 0;
	DWORD dwCurOffset = 0;
	DWORD dwOffsetLine = m_string.OffsetToLine( m_dwOffset );
	CPoint ptCaret( 0, 0);
	for(int i = m_wndScrollBar.GetScrollPos(); i < (int)( m_string.GetLineCount() ); i++)
	{
		dwOffset = m_string.GetLineOffset( i );
		string = m_string.GetLine(i);
		dwBegin = 0;
		strHan[0] = 0;
		const char* begin = string;
		const char* end = begin + string.GetLength();
		const char* iter = begin;

		while( *iter && iter < end ) {

			WORD wCodePage = m_string.m_awCodePage[dwOffset + (iter-begin)];

			const char* next = CharNextEx(iter, wCodePage );

			SIZE size;

			if( IsWndStyle( EBS_PASSWORD ) ) {
				m_pFont->GetTextExtent("*", &size);
				size.cx *= next-iter;
			} else {
				char temp[16];
				memcpy(temp, iter, next-iter);
				temp[next-iter] = 0;
				m_pFont->GetTextExtent(temp, &size, wCodePage);
			}

			dwCurOffset = dwOffset + (iter-begin);

			// 문장 내에서 캐럿과 위치가 같다면 출력 위치다.
			if( dwCurOffset == dwSetOffset )
			{
				if( pszStr )
					strcpy( pszStr, strHan );
				return CPoint( dwBegin, 0 + i * dwMaxHeight );// + GetScreenRect().TopLeft();
			}
			dwBegin += size.cx;
			iter = next;
		}

		// 캐럿이 문장 내에 있지 않고 문장의 끝에 새로운 입력을 기다려야하는 위치이다.
		if( dwSetOffset == m_string.GetLength() )
		{
			if( !m_dwOffset || m_string.GetAt( m_dwOffset - 1 ) != '\n' )
			{
				if( pszStr )
					pszStr[0] = 0;
				ptCaret = CPoint( dwBegin, i * dwMaxHeight );// + GetScreenRect().TopLeft();
			}
			else
			{
				if( pszStr )
					pszStr[0] = 0;
				ptCaret = CPoint( 0, ( i + 1 ) * dwMaxHeight );// + GetScreenRect().TopLeft();
			}
		}
	}
	return ptCaret;
}

//dwCurOffset = dwOffset + i2;
//if( dwCurOffset == m_string.GetLength() && dwCurOffset == m_dwOffset )
void CWndText::DrawCaret(C2DRender* p2DRender)
{
}
LONG CWndText::GetOffset(CPoint point)
{
	CString string;
	int dwMaxHeight = GetFontHeight();
	CPoint pt = point;
	pt.y /= dwMaxHeight;
	pt.y += m_wndScrollBar.GetScrollPos();
	DWORD dwOffset1 = m_string.GetLineOffset( pt.y );
	DWORD dwOffset2 = m_string.GetLineOffset( pt.y + 1);
	DWORD dwBegin = 0;
	const char* begin = m_string;
	const char* end = begin + dwOffset2;
	const char* iter = begin + dwOffset1;

	while(*iter && iter < end) {

		if( *iter == '\r' || *iter == '\n')
		{
			return iter-begin;
		}

		WORD wCodePage = m_string.m_awCodePage[iter-begin];

		const char* next = CharNextEx( iter, wCodePage );

		SIZE size;

		if( IsWndStyle( EBS_PASSWORD ) )
		{
			m_pFont->GetTextExtent("*", &size);
			size.cx *= next-iter;
		}
		else
		{
			char temp[16];
			memcpy(temp, iter, next-iter);
			temp[next-iter] = 0;
			m_pFont->GetTextExtent(temp, &size, wCodePage);
		}

		if( (int)( dwBegin+size.cx ) > pt.x)
		{

			if(pt.x-dwBegin < (dwBegin+size.cx)-pt.x)
				return iter-begin;
			else 
				return next-begin;
		}

		dwBegin += size.cx;
		iter = next;
	}
	return iter-begin;
}
void CWndText::OnChar( UINT nChar  )
{
	if( nChar == 3 ) // CTRL-C : copy
	{
		if( m_dwBlockBegin != m_dwBlockEnd )
		{
			DWORD dwBlockBegin;
			DWORD dwBlockEnd;
			if(m_dwBlockBegin > m_dwBlockEnd)
				dwBlockBegin = m_dwBlockEnd, dwBlockEnd = m_dwBlockBegin;
			else
				dwBlockBegin = m_dwBlockBegin, dwBlockEnd = m_dwBlockEnd;
			CString strClipboard;
			strClipboard = m_string.Mid( dwBlockBegin, dwBlockEnd - dwBlockBegin );
			if( m_bEnableClipboard ) 
				CClipboard::SetText( strClipboard );
		}
	}
}
void CWndText::OnLButtonDown( UINT nFlags, CPoint point )
{
	m_string.ClearStyle( ESSTY_BLOCK );
	LONG lOffset = GetOffset(point);
	if(lOffset != -1)
		m_dwBlockBegin = m_dwBlockEnd = m_dwOffset = lOffset; 
	m_bLButtonDown = TRUE;
	SetCapture();
	m_TextComp[ 0 ] = 0;
	CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret );
	SetCaretPos( ptCaret );
}
void CWndText::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_string.ClearStyle( ESSTY_BLOCK );
	m_bLButtonDown = FALSE;
	ReleaseCapture();
	CWndBase* pWnd = m_pParentWnd;
	pWnd->OnChildNotify( STN_CLICKED, m_nIdWnd, (LRESULT*)this); 
}
void CWndText::OnRButtonUp(UINT nFlags, CPoint point)
{
	if( g_pPlayer && g_pPlayer->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_FONTEDIT ) )
	{
		m_wndMenu.DeleteAllMenu();
		m_wndMenu.AppendMenu( 0, MGI_APPELL_UP,		prj.GetText( TID_GAME_FONT_EDIT ) );
		m_wndMenu.Move( CPoint( m_rectCurrentWindow.left, m_rectCurrentWindow.top ) + point );
		m_wndMenu.SetVisible( TRUE );//!m_wndMenuMover.IsVisible() );
		m_wndMenu.SetFocus();
	}
}

BOOL CWndText::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
//	m_wndMenu.SetVisible( FALSE );
	return CWndBase::OnChildNotify( message, nID, pLResult ); 
} 

BOOL CWndText::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	if( pWndBase == &m_wndMenu )
	{
		switch( nID )
		{
		case MGI_APPELL_UP:
			SAFE_DELETE(g_WndMng.m_pWndFontEdit);
			g_WndMng.m_pWndFontEdit = new CWndFontEdit;
			g_WndMng.m_pWndFontEdit->Initialize();
			g_WndMng.m_pWndFontEdit->m_pWndText = this;
			break;
		} 
	}
	
	m_wndMenu.SetVisible(FALSE);

	return CWndBase::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndText::OnMouseMove(UINT nFlags, CPoint point)
{
	//return;
	if(m_bLButtonDown == TRUE)
	{
		m_string.ClearStyle( ESSTY_BLOCK );
		LONG lOffset = GetOffset(point);
		if(lOffset != -1)
			m_dwBlockEnd = m_dwOffset = lOffset; 
	}
	else
	{
		int dwOffset;
		int dwBlockBegin;
		int dwBlockEnd;
		LONG lOffset = GetOffset( point );
		BYTE chr1, chr2;

		if(lOffset >= 0 && lOffset < m_string.GetLength() ) 
		{
			if( !IsWhite( m_string.GetAt( lOffset ) ) ) 
			{
				dwBlockBegin = lOffset;
				dwBlockEnd = lOffset;

				while( dwBlockBegin > 1 )
				{
					chr1 = m_string.GetAt ( dwBlockBegin - 1 );
					chr2 = m_string.GetAt ( dwBlockBegin - 2 ); 

					if( IsDBCSLeadByte( chr1 ) && IsDBCSLeadByte( chr2 ) )
						dwBlockBegin -= 2;
					else
					if( !IsWhite( chr1 ) )
						dwBlockBegin -= 1;
					else
						break;
				}
				/*
				while(dwBlockEnd < m_string.GetLength() )
				{
					chr1 = m_string.GetAt ( dwBlockEnd );

					if( IsDBCSLeadByte( chr1 ) )
						dwBlockEnd += 2;
					else
					if( !IsWhite( chr1 ) )
						dwBlockEnd += 1;
					else
						break;
				}
				*/
				dwOffset = dwBlockEnd; 

				CString strFind = m_string.Mid( dwBlockBegin, dwBlockEnd - dwBlockBegin );
				/*
				CStringArray* pStrArray = prj.GetHelp( strFind );
				if(pStrArray)
				{
					GET_CLIENT_POINT( m_pApp->GetSafeHwnd(), point);
					CPoint pt1 = OffsetToPoint(dwBlockBegin);
					CPoint pt2 = OffsetToPoint(dwBlockEnd);
					g_toolTip.PutToolTip( 100, pStrArray->GetAt(0), CRect
						(	pt1.x, pt1.y, pt2.x, pt2.y + m_pFont->GetMaxHeight() )
						, point);
				}
				*/
			}
		}
	}
}
void CWndText::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	LONG lOffset = GetOffset(point);
	BYTE chr1;

	if(lOffset >= 0 && lOffset < m_string.GetLength() ) 
	{
		if( !IsWhite( m_string.GetAt( lOffset ) ) ) 
		{
			chr1 = 0;
			const char* begin = m_string;
			const char* end = begin + m_string.GetLength();
			const char* iter = begin;

			m_dwBlockBegin = 0;

			// Get m_dwBlockBegin
			while(*iter && iter < end) {

				if( IsWhite( *iter ) )
					m_dwBlockBegin = iter - begin;

				WORD wCodePage = m_string.m_awCodePage[iter-begin];

				iter = CharNextEx( iter, wCodePage );
				if(iter - begin > lOffset) break;
			}

			m_dwBlockEnd = m_dwBlockBegin;

			while(*iter && iter < end) {

				if( IsWhite( *iter ) ) {
					m_dwBlockEnd = iter - begin;
					break;
				}

				WORD wCodePage = m_string.m_awCodePage[iter-begin];

				iter = CharNextEx( iter, wCodePage );
				if(iter - begin > lOffset) break;
			}

			m_dwOffset = m_dwBlockEnd;
		}
	}
}
BOOL CWndText::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	if( m_wndScrollBar.GetScrollPage() >= m_wndScrollBar.GetMaxScrollPos() )
		return TRUE;
#endif // __IMPROVE_QUEST_INTERFACE
	if( zDelta < 0 )
	{
		if( m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() > m_wndScrollBar.GetScrollPos() )
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetScrollPos()+1 );
		else
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() );
	}
	else
	{
		if( m_wndScrollBar.GetMinScrollPos() < m_wndScrollBar.GetScrollPos() )
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetScrollPos()-1 );
		else
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetMinScrollPos() );
	}
	
	return TRUE;
}

void CWndText::OnSetFocus(CWndBase* pOldWnd)
{
	CWndBase::OnSetFocus(pOldWnd);

	if( g_WndMng.m_pWndFontEdit )
	{
		g_WndMng.m_pWndFontEdit->m_pWndText = this;
	}
}
void CWndText::OnKillFocus(CWndBase* pNewWnd)
{
	CWndBase::OnKillFocus( pNewWnd );
}
void CWndText::SetString( LPCTSTR pszString, DWORD dwColor )
{
	// Init의 목적은 Align이 아니라 m_pfont와 Rect세팅이다.
	m_string.Init( m_pFont, &GetClientRect() );
	// 이 내부에서 Align을 한다.
	m_string.SetParsingString( pszString, dwColor );
	if( IsWndStyle( WBS_VSCROLL ) )
		UpdateScrollBar();
	m_dwOffset = m_string.GetLength();
	CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret );
	SetCaretPos( ptCaret );
	
	m_dwBlockBegin = m_dwBlockEnd = 0;
	//m_dwBlockBegin = m_dwBlockEnd = m_dwOffset = 0;
	//SetCaretPos( CPoint(0,0) );
	//m_string.Reset( m_pFont, &GetClientRect() );
}
void CWndText::AddString(LPCTSTR pszString, DWORD dwColor, DWORD dwPStyle )
{
	int nLine = m_string.GetLineCount() - 1;
	m_string.AddParsingString( pszString, dwColor, 0x00000000, 0, dwPStyle );//+= pszString;
	//m_string.Align( m_pFont, nLine );
	UpdateScrollBar();
	m_dwOffset = m_string.GetLength();
	CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret );
	SetCaretPos( ptCaret );
}

void CWndText::Insert(int nIndex, LPCTSTR pstr)
{
	int nLine = m_string.GetLineCount() - 1;
	m_string.Insert( nIndex, pstr );
	UpdateScrollBar();
	m_dwOffset = m_string.GetLength();
	CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret );
	SetCaretPos( ptCaret );	
}

void CWndText::BlockInsert( LPCTSTR pstr, LPCTSTR pstr2 )
{
	DWORD dwBlockTemp;

	if(m_dwBlockBegin > m_dwBlockEnd) 
	{
		dwBlockTemp = m_dwBlockBegin;
		m_dwBlockBegin = m_dwBlockEnd;
		m_dwBlockEnd   = dwBlockTemp;
	}
		
	if( m_dwBlockBegin >= 0 )
	{
		m_string.Insert( m_dwBlockBegin, pstr );
		
		m_dwBlockBegin += strlen(pstr);

		if( (int)( m_dwBlockEnd + strlen(pstr) ) < m_string.GetLength() )
			m_dwBlockEnd += strlen(pstr);
		else
			m_dwBlockEnd = m_string.GetLength();

		m_string.Insert( m_dwBlockEnd, pstr2 );

		UpdateScrollBar();
		m_dwOffset = m_string.GetLength();
		CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret );
		SetCaretPos( ptCaret );	
	}
}

void CWndText::BlockDelete( LPCTSTR pstr, LPCTSTR pstr2 )
{
	DWORD dwBlockTemp;
	
	if(m_dwBlockBegin > m_dwBlockEnd) 
	{
		dwBlockTemp = m_dwBlockBegin;
		m_dwBlockBegin = m_dwBlockEnd;
		m_dwBlockEnd   = dwBlockTemp;
	}
	
	if( m_dwBlockBegin >= 0 )
	{
		int nColor = stricmp( pstr, "#c" );

		TCHAR ch;
		int nCount = m_dwBlockEnd;
		int nIndex = 0;

		nIndex = m_string.Find( pstr2, nCount );

		if( nIndex != -1 )
			Delete( nIndex, strlen(pstr2) );
		///////////////////////////////////////////

		nCount = m_dwBlockBegin;

		while( nCount-- )
		{
			ch = m_string.GetAt( nCount );

			if( ch == '#' )
			{
				nIndex = m_string.Find( pstr, nCount );

				if( nIndex != -1 )
				{
					Delete( nIndex, strlen(pstr) );	

					if( nColor != -1 )
					{
						Delete( nIndex, 8 );							
					}					
					
					break;
				}
			}
		}

		m_dwBlockBegin -= strlen(pstr);
		m_dwBlockEnd   -= strlen(pstr);

		if( nColor != -1 )
		{
			m_dwBlockBegin -= 8;
			m_dwBlockEnd   -= 8;
		}					
		

		if( (int)m_dwBlockBegin < 0 )
			m_dwBlockBegin = 0;

		if( (int)m_dwBlockEnd < 0 )
			m_dwBlockEnd = 0;
								
		UpdateScrollBar();
		m_dwOffset = m_string.GetLength();
		CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret );
		SetCaretPos( ptCaret );	
	}
}

void CWndText::Delete(int nIndex, int nLen)
{
	int nLine = m_string.GetLineCount() - 1;
	m_string.Delete( nIndex, nLen );
	UpdateScrollBar();
	m_dwOffset = m_string.GetLength();
	CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret );
	SetCaretPos( ptCaret );		
}

void CWndText::ResetString()
{
	//m_string.Reset( m_pFont, &GetClientRect() );
	CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret );
	SetCaretPos( ptCaret );
	UpdateScrollBar();
}

////////////////////////////

// 매뉴는 생성시 m_wndArray에 들어가지만 화면 좌표는 전역 좌표다. 
// 따라서 m_rectWindow, m_rectClient는 화면 상에서의 좌표이므로 바로 사용해두 된다.
CWndMenu::CWndMenu()
{
	m_nLargeWidth = 0;
	m_byWndType = WTYPE_MENU;
}
CWndMenu::~CWndMenu()
{
	for( int i = 0; i < m_awndMenuItem.GetSize(); i++)
		safe_delete( GetMenuItem( i ) );
}
BOOL CWndMenu::CreateMenu( CWndBase* pWndParent )
{
	BOOL bResult = CWndBase::Create( WBS_POPUP/*|WBS_NOFRAME*/, CRect(0,0,0,0), pWndParent, 10);
	SetVisible( FALSE );
	return bResult;
}
void CWndMenu::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	CRect rectOld = m_rectClient;
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect( 3, 3 );
	m_rectLayout = m_rectClient;
	if( bOnSize && ( rectOld.Width() != m_rectClient.Width() || rectOld.Height() != m_rectClient.Height() ) )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
	MakeVertexBuffer();
}
BOOL CWndMenu::CreatePopupMenu()
{
	return TRUE;
}
BOOL CWndMenu::LoadMenu(LPCTSTR lpszResourceName)
{
	return TRUE;
}
BOOL CWndMenu::LoadMenu(UINT nIDResource)
{
	return TRUE;
}
BOOL CWndMenu::LoadMenuIndirect(const void* lpMenuTemplate)
{
	return TRUE;
}
BOOL CWndMenu::DestroyMenu()
{
	return TRUE;
}
void CWndMenu::DeleteAllMenu()
{	
	for( int i = 0; i < m_awndMenuItem.GetSize(); i++)
		safe_delete( GetMenuItem( i ) );
	m_awndMenuItem.RemoveAll();
}

BOOL CWndMenu::DeleteMenu(UINT nPosition, UINT nFlags)
{	
	safe_delete( GetMenuItem( nPosition ) );
	m_awndMenuItem.RemoveAt( nPosition );
	for( int i = nPosition; i < m_awndMenuItem.GetSize(); i++)
	{
		CWndButton* pWndMenuItem = GetMenuItem( i );
		CRect rect = pWndMenuItem->GetWindowRect(TRUE);
		rect.OffsetRect( 0, -22);
		pWndMenuItem->SetWndRect( rect );  
	}
	CRect rect = GetWindowRect(TRUE);
	rect.OffsetRect( 0, 22);
	rect.bottom -= 22;
	SetWndRect( rect );  
	return TRUE;
}
CWndButton* CWndMenu::AppendMenu(UINT nFlags, UINT nIDNewItem,	LPCTSTR lpszNewItem)
{
	CWndButton* pWndButton = new CWndButton;
	int nCount = m_awndMenuItem.GetSize();
	CSize size = m_pFont->GetTextExtent( lpszNewItem );
	if( size.cx + 60 > m_nLargeWidth )
		m_nLargeWidth = size.cx + 60;
	pWndButton->Create(lpszNewItem, WBS_MENUITEM | WBS_HIGHLIGHT/* | WBS_NOMENUICON*/, CRect( 2, 2 + ( nCount * 22 ), m_nLargeWidth, 2 + ( nCount * 22 ) + 20 ), this, nIDNewItem );
	pWndButton->DelWndStyle( WBS_NODRAWFRAME );
	m_awndMenuItem.Add( pWndButton );
	for( int i = 0; i < m_awndMenuItem.GetSize(); i++ )
	{
		pWndButton = (CWndButton*)m_awndMenuItem.GetAt( i );
		CRect rect = pWndButton->GetWindowRect( TRUE );
		rect.right = rect.left + m_nLargeWidth; 
		pWndButton->SetWndRect( rect );
	}
	SetWndRect( CRect( 0, 0, m_nLargeWidth, 5 + 5 + ( ( nCount + 1 ) * 22 ) ) );
	return pWndButton;
}
UINT CWndMenu::CheckMenuItem(UINT nIDCheckItem, UINT nCheck)
{
	CWndButton* pWndButton = (CWndButton*)m_awndMenuItem.GetAt( nIDCheckItem );
	pWndButton->SetCheck( nCheck );
	return TRUE;
}
UINT CWndMenu::EnableMenuItem(UINT nIDEnableItem, UINT nEnable)
{
	return TRUE;
}
UINT CWndMenu::GetMenuItemCount() const
{
	return TRUE;
}
UINT CWndMenu::GetMenuItemID(int nPos) const
{
	return TRUE;
}
UINT CWndMenu::GetMenuState(UINT nID, UINT nFlags) const
{
	CWndButton* pWndButton = (CWndButton*)m_awndMenuItem.GetAt( nID );
	return pWndButton->GetCheck();//( nCheck );
	//return TRUE;
}
int  CWndMenu::GetMenuString(UINT nIDItem, LPTSTR lpString, int nMaxCount,	UINT nFlags) const
{
	return TRUE;
}
int  CWndMenu::GetMenuString(UINT nIDItem, CString& rString, UINT nFlags) const
{
	return NULL;
}
CWndMenu* CWndMenu::GetSubMenu(int nPos) const
{
	return NULL;
}
BOOL CWndMenu::InsertMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem,	LPCTSTR lpszNewItem)
{
	CWndButton* pWndButton = new CWndButton;
	pWndButton->Create(lpszNewItem, WBS_MENUITEM | WBS_HIGHLIGHT, CRect( 5, 50 + 5 + ( nPosition * 22 ), 175, 50 + 5 + ( nPosition * 22 ) + 20 ), this, nIDNewItem );
	pWndButton->DelWndStyle( WBS_NODRAWFRAME );
	m_awndMenuItem.InsertAt( nPosition, pWndButton );
	for( int i = nPosition + 1; i < m_awndMenuItem.GetSize(); i++)
	{
		CWndButton* pWndMenuItem = GetMenuItem( i );
		CRect rect = pWndMenuItem->GetWindowRect(TRUE);
		rect.OffsetRect( 0, 22);
		pWndMenuItem->SetWndRect( rect );  
	}
	CRect rect = GetWindowRect(TRUE);
	rect.OffsetRect( 0, -22);
	rect.bottom += 22;
	SetWndRect( rect );  
	return TRUE;
}
BOOL CWndMenu::ModifyMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem,	LPCTSTR lpszNewItem)
{
	return TRUE;
}
BOOL CWndMenu::RemoveMenu(UINT nPosition, UINT nFlags)
{
	return TRUE;
}
BOOL CWndMenu::CheckMenuRadioItem(UINT nIDFirst, UINT nIDLast, UINT nIDItem, UINT nFlags)
{
	return TRUE;
}
BOOL CWndMenu::SetDefaultItem(UINT uItem, BOOL fByPos)
{
	return TRUE;
}
UINT CWndMenu::GetDefaultItem(UINT gmdiFlags, BOOL fByPos)
{
	return TRUE;
}
CWndButton* CWndMenu::GetMenuItem( int nPos )
{
	return (CWndButton*) m_awndMenuItem.GetAt( nPos );
}
void CWndMenu::SetVisibleSub( BOOL bVisible )
{
	CWndBase::SetVisible( bVisible );
	for( int i = 0; i < m_awndMenuItem.GetSize(); i++)
	{
		if( GetMenuItem( i )->m_pWndMenu )
			GetMenuItem( i )->m_pWndMenu->SetVisibleSub( bVisible );
	}
}
void CWndMenu::SetVisibleAllMenu( BOOL bVisible )
{
	CWndMenu* pMenu = this;
	CWndMenu* pMenu2 = this;
	while( pMenu2 && pMenu2->IsWndStyle( WBS_POPUP ) )
	{
		pMenu = pMenu2;
		pMenu2 = (CWndMenu*) pMenu->GetParentWnd();
			//pMenu = dynamic_cast<CWndMenu*>(pMenu->GetParentWnd());
	}
	if( pMenu )
	{
		pMenu->CWndBase::SetVisible( bVisible );
		for( int i = 0; i < pMenu->m_awndMenuItem.GetSize(); i++)
		{
			if( pMenu->GetMenuItem( i )->m_pWndMenu )
				pMenu->GetMenuItem( i )->m_pWndMenu->SetVisibleSub( bVisible );
		}
	}
}
void CWndMenu::OnKillFocus(CWndBase* pNewWnd)
{
	if( pNewWnd == NULL )
		SetVisibleAllMenu( FALSE );
	else
	if( pNewWnd->IsWndStyle( WBS_POPUP ) == FALSE )
		SetVisibleAllMenu( FALSE );
}

void CWndMenu::OnDraw(C2DRender* p2DRender)
{

/*
//	if( m_bHighLight && ( m_pWndMenu == FALSE || m_pWndMenu->IsVisible( ) == FALSE ) )
	{
		//if( m_pParentWnd->IsWndStyle(WBS_POPUP) )
		{
			// 매뉴의 버튼을 훑어서 켜진거 다 끈다.
		//	CWndMenu* pMenu = (CWndMenu*)m_pParentWnd;
			for( int i = 0; i < m_awndMenuItem.GetSize(); i++)
			{
				if( ((CWndButton*)m_awndMenuItem.GetAt( i ) )->m_pWndMenu )
					((CWndButton*)m_awndMenuItem.GetAt( i ) )->m_pWndMenu->SetVisibleSub( FALSE );
			}
		}
	}
*/
}
BOOL CWndMenu::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	BOOL bResult = FALSE;
	if( m_pParentWnd )
	{
		bResult = m_pParentWnd->OnChildNotify( message, nID, pLResult );
		m_pParentWnd->SetFocus();
	}

	return bResult;
}
BOOL CWndMenu::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
#if __VER >= 15 // __GUILD_HOUSE
	
	//gmpbigsun: 길드하우징 콘트롤 오브젝트에 대해 메세지전파를 차단, 새로운방식으로 작동해야 하기때문에...
	if( MMI_GHOUSE_INFO == nID )
	{
		// sgook : 길드가구창을 열어주면 댄다고 함. 2009_11_18
		CWndGuildHousing* pWnd = (CWndGuildHousing*)g_WndMng.GetApplet( APP_GH_FURNITURE_STORAGE );
		if( !pWnd )
		{
			g_WndMng.CreateApplet( APP_GH_FURNITURE_STORAGE ); 
			pWnd = (CWndGuildHousing*)g_WndMng.GetApplet( APP_GH_FURNITURE_STORAGE );
			assert( pWnd );

			pWnd->InitBySection( CWndGuildHousing::GS_FURNITURE );
		}

	}
	else if( MMI_GHOUSE_REINSTALL == nID )		//재설치
	{
		GuildHouse->Reset( );
		return TRUE;
	}
	else if( MMI_GHOUSE_RECALL == nID )		//회수 
	{
		GuildHouse->Remove( );
		return TRUE;
	}
#endif

	BOOL bResult = FALSE;
	if( m_pParentWnd )
	{
		bResult = m_pParentWnd->OnCommand( nID, dwMessage, this );
		if(bResult != -1)
			m_pParentWnd->SetFocus();
	}
	return bResult;
}
void CWndMenu::PaintFrame( C2DRender* p2DRender )
{
	//p2DRender->RenderFillRect( GetWindowRect(), D3DCOLOR_ARGB( 255, 226,198,181 ) );
	//p2DRender->RenderRoundRect( GetWindowRect(), D3DCOLOR_ARGB( 255, 238,163,128 ) );
	//CRect rect = GetWindowRect();
	//m_pTheme->RenderWndMenu( p2DRender, &rect );
//	CWndBase::PaintFrame( p2DRender );
}
BOOL CWndMenu::OnEraseBkgnd( C2DRender* p2DRender )
{
	p2DRender->RenderFillRect( GetClientRect(), D3DCOLOR_ARGB( 200, 0,0,0 ) );
	//CRect rect = GetClientRect();
//	p2DRender->RenderFillRect(rect, D3DCOLOR_ARGB( 255, 255,255,255 ) );
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndListCtrl
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndListCtrl::CWndListCtrl() 
{
	m_nWndColor    = D3DCOLOR_TEMP( 255,  0x5 << 3,  0x5 << 3,  0x5 << 3 );
	m_nFontColor   = D3DCOLOR_ARGB( 255, 64, 64, 64 );
	m_nSelectColor = D3DCOLOR_ARGB( 255, 255, 0, 0   );
	m_nCurSelect = -1;
	m_pFocusItem = NULL;
	m_byWndType  = WTYPE_LISTCTRL;
	m_nLineSpace = 3;
}
CWndListCtrl::~CWndListCtrl()
{
	LVCOLUMN* pColumn;
	LVITEM* pItems;
	for( int i = 0; i < m_aColumns.GetSize(); i++ )
	{
		pColumn = (LVCOLUMN*) m_aColumns.GetAt( i );
		safe_delete( pColumn->pszText );
		safe_delete( pColumn );
	}
	for( i = 0; i < m_aItems.GetSize(); i++ )
	{
		pItems = (LVITEM*)m_aItems.GetAt ( i );
		for( int j = 0; j < m_aColumns.GetSize(); j++ )
		{
			if( pItems[ j ].pszText )
				safe_delete( pItems[ j ].pszText );
		}
		safe_delete_array( pItems );
	}
//	ResetContent();
}
void CWndListCtrl::Create( DWORD dwListCtrlStyle, RECT& rect, CWndBase* pParentWnd, UINT nID )
{
	//m_dwListCtrlStyle = dwListCtrlStyle;
	CWndBase::Create( dwListCtrlStyle | WBS_CHILD, rect, pParentWnd, nID );
}
void CWndListCtrl::LoadListBoxScript(LPCTSTR lpFileName) 
{
/*Scanner scanner;
 	if(scanner.Load(lpFileName) == FALSE)
		return;
	scanner.GetToken(); // keyword
	do {
		InterpriteScript(scanner,m_listItemArray);
		scanner.GetToken(); // keyword
	} while(scanner.tok != FINISHED);
	*/
}	
void CWndListCtrl::InterpriteScript(CScanner& scanner,CPtrArray& ptrArray) 
{
	/*
	do {
		LPLISTITEM lpListItem = new LISTITEM;
		lpListItem->m_strWord = scanner.Token;
		ptrArray.Add(lpListItem);
		scanner.GetToken(); 
	} while(*scanner.token != '}' && scanner.tok != FINISHED);
	if(scanner.tok == FINISHED)
		return;
	scanner.GetToken(); 
	*/
}
void CWndListCtrl::OnInitialUpdate()
{
	//CSize size = m_pSprPack->GetAt(13)->GetSize();
	//CRect rect = GetClientRect();
	//rect.left = rect.right - size.cx;
	CRect rect = GetWindowRect();
	//rect.right += 5;
	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );//,m_pSprPack,-1);
}


void CWndListCtrl::OnDraw(C2DRender* p2DRender) 
{
	CPoint pt( 3, 3 );
	m_nFontHeight = GetFontHeight();
	//
	// 리포트 출력 
	//
//	if( IsWndStyle( WLVS_REPORT ) )
	{
		pt.y -= (m_nFontHeight + 3) * m_wndScrollBar.GetScrollPos();
		for( int i = 0; i < m_aItems.GetSize(); i++ ) 
		{
			LVITEM* pItems = (LVITEM*)m_aItems.GetAt( i );
			for( int i2 = 0, x = 0; i2 < m_aColumns.GetSize(); i2++ ) 
			{
				if( i == m_nCurSelect )
					p2DRender->TextOut( x + 2, pt.y, pItems[ i2 ].pszText, m_nSelectColor ); 
				else
					p2DRender->TextOut( x + 2, pt.y, pItems[ i2 ].pszText, m_nFontColor ); 
				LVCOLUMN* pColumn = (LVCOLUMN*)m_aColumns.GetAt( i2 );
				x += pColumn->cx + 7;
			}
			pt.y += m_nFontHeight + 3;
		}
		// 스크롤바 관련 
		int nPage = GETRANGE;//GetClientRect().Height() / (m_nFontHeight + 3);
		int nRange = m_aItems.GetSize();// - nPage;

		if(	IsWndStyle( WBS_VSCROLL ) )  
		{
			m_wndScrollBar.SetVisible( TRUE );
			m_wndScrollBar.SetScrollRange( 0, nRange );
			m_wndScrollBar.SetScrollPage( nPage );
		}
		else
			m_wndScrollBar.SetVisible( FALSE );
	}
	//
	// 아이콘 출력
	// CCtrllist
	if( IsWndStyle( WLVS_ICON ) )
	{
		CRect rect = GetClientRect();
		int nWidth = rect.Width() / 32;
		int nHeight = rect.Height() / 35;
		pt.y = 0;
		pt.y += m_wndScrollBar.GetScrollPos() * nWidth;
		for( int i = pt.y; i < m_aItems.GetSize(); i++ ) 
		{
			int x = ( i - pt.y ) % nWidth;
			int y = ( i - pt.y ) / nWidth;
			LVITEM* pItems = (LVITEM*)m_aItems.GetAt( i );
			if( i == m_nCurSelect )
				p2DRender->RenderFillRect( CRect( x * 32, y * 32, x * 32 + 32 - 2, y * 32 + 32 - 2 ), 0xff707070 );
			else
				p2DRender->RenderFillRect( CRect( x * 32, y * 32, x * 32 + 32 - 2, y * 32 + 32 - 2 ), 0xffffffff );
			//p2DRender->TextOut( x * 32, y * 32, pItems->pszText, 0xff000000 );
		}
		// 스크롤바 관련 
		int nPage = nHeight;
		int nRange = m_aItems.GetSize() / nWidth;// - nPage;
		if(nRange - nPage <= 0)
			m_wndScrollBar.SetVisible(FALSE);
		else
		{
			m_wndScrollBar.SetVisible( TRUE );
			m_wndScrollBar.SetScrollRange( 0, nRange );
			m_wndScrollBar.SetScrollPage( nPage );
		}
	}
}
int CWndListCtrl::GetCurSel() const
{
	return m_nCurSelect;
}
int CWndListCtrl::SetCurSel( int nSelect )
{
	m_pFocusItem = (LVITEM*)m_aItems.GetAt( nSelect );
	m_nCurSelect = nSelect;
	return m_nCurSelect;
}
CString CWndListCtrl::GetItemText(int nItem, int nSubItem) const
{
	LVITEM* pItem = (LVITEM*)m_aItems.GetAt( nItem );
	return pItem[ nSubItem ].pszText;
}
void CWndListCtrl::OnLButtonUp( UINT nFlags, CPoint point )
{
	CPoint pt( 3, 3 );
	CRect rect;
	//
	// 리포트 출력 
	//
	//if( IsWndStyle( WLVS_REPORT ) )
	{
		pt.y -= (m_nFontHeight + 3) * m_wndScrollBar.GetScrollPos();
		for( int i = 0; i < m_aItems.GetSize(); i++ ) 
		{
			LVITEM* pItems = (LVITEM*)m_aItems.GetAt( i );
			rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
			if( rect.PtInRect( point ) )
			{
				if(m_pFocusItem == pItems)
				{
					// 부모가 차일드 윈도가 아니어야 OnCommand 메시지를 받는다.
					CWndBase* pWnd = m_pParentWnd;
					pWnd->OnChildNotify(WNM_SELCHANGE,m_nIdWnd,(LRESULT*)m_pFocusItem); 
					return;
				}
			}
			pt.y += m_nFontHeight + 3;
		}
	}
	//
	// 아이콘 출력
	// CCtrllist
	if( IsWndStyle( WLVS_ICON ) )
	{
		CRect rect = GetClientRect();
		int nWidth = rect.Width() / 32;
		int nHeight = rect.Height() / 35;
		pt.y = 0;
		pt.y += m_wndScrollBar.GetScrollPos() * nWidth;
		for( int i = pt.y; i < m_aItems.GetSize(); i++ ) 
		{
			int x = ( i - pt.y ) % nWidth;
			int y = ( i - pt.y ) / nWidth;
			LVITEM* pItems = (LVITEM*)m_aItems.GetAt( i );
			rect.SetRect( x * 32, y * 32, x * 32 + 32, y * 32 + 32 );
			if( rect.PtInRect( point ) )
			{
				if(m_pFocusItem == pItems)
				{
					// 부모가 차일드 윈도가 아니어야 OnCommand 메시지를 받는다.
					CWndBase* pWnd = m_pParentWnd;
					pWnd->OnChildNotify(WNM_SELCHANGE,m_nIdWnd,(LRESULT*)m_pFocusItem); 
					return;
				}
			}
		}
	}
}
void CWndListCtrl::OnLButtonDown( UINT nFlags, CPoint point )
{
	CPoint pt( 3, 3 );
	CRect rect;
	//
	// 리포트 출력 
	//
	//if( IsWndStyle( WLVS_REPORT ) )
	{
		pt.y -= (m_nFontHeight + 3) * m_wndScrollBar.GetScrollPos();
		for( int i = 0; i < m_aItems.GetSize(); i++ ) 
		{
			LVITEM* pItems = (LVITEM*)m_aItems.GetAt( i );
			rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
			if( rect.PtInRect( point ) )
			{
				m_nCurSelect = i;
				m_pFocusItem = pItems;
				break;
			}
			pt.y += m_nFontHeight + 3;
		}
	}
	//
	// 아이콘 출력
	// CCtrllist
	if( IsWndStyle( WLVS_ICON ) )
	{
		CRect rect = GetClientRect();
		int nWidth = rect.Width() / 32;
		int nHeight = rect.Height() / 35;
		pt.y = 0;
		pt.y += m_wndScrollBar.GetScrollPos() * nWidth;
		for( int i = pt.y; i < m_aItems.GetSize(); i++ ) 
		{
			int x = ( i - pt.y ) % nWidth;
			int y = ( i - pt.y ) / nWidth;
			LVITEM* pItems = (LVITEM*)m_aItems.GetAt( i );
			rect.SetRect( x * 32, y * 32, x * 32 + 32, y * 32 + 32 );
			if( rect.PtInRect( point ) )
			{
				m_nCurSelect = i;
				m_pFocusItem = pItems;
				break;
			}
		}
	}
} 
void CWndListCtrl::OnRButtonUp( UINT nFlags, CPoint point )
{
}
void CWndListCtrl::OnRButtonDown( UINT nFlags, CPoint point )
{
}
void CWndListCtrl::OnLButtonDblClk( UINT nFlags, CPoint point )
{
}
void CWndListCtrl::OnRButtonDblClk( UINT nFlags, CPoint point)
{
}
void CWndListCtrl::OnSize( UINT nType, int cx, int cy )
{
	CRect rect = GetWindowRect();

	if( IsWndStyle( WBS_VSCROLL ) ) 
		rect.left = rect.right - VSCROLL_WIDTH;
	m_wndScrollBar.SetVisible( IsWndStyle( WBS_VSCROLL ) );
	m_wndScrollBar.SetWndRect( rect );

	CWndBase::OnSize( nType, cx, cy);
}
void CWndListCtrl::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect( 4, 4 );

	//if( IsWndStyle( WLVS_REPORT ) )
	{
		int nPage = GETRANGE;//GetClientRect().Height() / (m_nFontHeight + 3);
		int nRange = m_aItems.GetSize();// - nPage;
		if( !( nRange - nPage <= 0 ) )
			m_rectClient.right -= 15; // 스크롤 바가 보이면 
		m_rectClient.top += 15; // 리포트 칼럼 
	}
	if( IsWndStyle( WLVS_ICON ) )
	{
		CRect rect = GetClientRect();
		int nWidth = rect.Width() / 32;
		int nHeight = rect.Height() / 35;
		int nPage = nHeight;
		int nRange = m_aItems.GetSize() / nWidth;// - nPage;
		if( !( nRange - nPage <= 0 ) )
			m_rectClient.right -= 15; // 스크롤 바가 보이면 
	}
	if( bOnSize )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
}
void CWndListCtrl::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();
	m_pTheme->RenderWndTextFrame( p2DRender, &rect );

	//if( IsWndStyle( WLVS_REPORT ) )
	{
		// 컬럼 출력 
		for( int i = 0, x = 0; i < m_aColumns.GetSize(); i++ )
		{
			LVCOLUMN* pColumn = (LVCOLUMN*)m_aColumns.GetAt( i );
			p2DRender->TextOut( x + 4, 4, pColumn->pszText ); 
			m_pTheme->GradationRect( p2DRender, &CRect( x, 0, x + pColumn->cx + 5, 20 ), 
				D3DCOLOR_ARGB( 50, 100, 100, 100 ),
				D3DCOLOR_ARGB( 90, 255, 255, 255 ),
				D3DCOLOR_ARGB( 50, 000, 000, 000 ), 20 );
			x += pColumn->cx + 6;
		}
		m_pTheme->GradationRect( p2DRender, &CRect( x, 0, rect.Width(), 20 ), 
			D3DCOLOR_ARGB( 50, 100, 100, 100 ),
			D3DCOLOR_ARGB( 90, 255, 255, 255 ),
			D3DCOLOR_ARGB( 50, 000, 000, 000 ), 20 );
	}
}
/*
typedef struct tagLVCOLUMNA
{
    UINT mask;
    int fmt;
    int cx;
    LPSTR pszText;
    int cchTextMax;
    int iSubItem;
#if (_WIN32_IE >= 0x0300)
    int iImage;
    int iOrder;
#endif
} LVCOLUMNA, FAR* LPLVCOLUMNA;
typedef struct tagLVITEMA
{
    UINT mask;
    int iItem;
    int iSubItem;
    UINT state;
    UINT stateMask;
    LPSTR pszText;
    int cchTextMax;
    int iImage;
    LPARAM lParam;
#if (_WIN32_IE >= 0x0300)
    int iIndent;
#endif
} LVITEMA, FAR* LPLVITEMA;
*/
BOOL CWndListCtrl::OnEraseBkgnd( C2DRender* p2DRender )
{
	return TRUE;
}
BOOL CWndListCtrl::SetItem( const LVITEM* pItem )
{
	if( pItem->iItem < m_aItems.GetSize() && m_aItems.GetAt( pItem->iItem ) == NULL ) 
		return FALSE; // 존재하지 않는다.
	LVITEM* pItems = (LVITEM*)m_aItems.GetAt( pItem->iItem );
	if( pItems )
	{
		if( pItems[ pItem->iSubItem ].pszText )
			safe_delete( pItems[ pItem->iSubItem ].pszText );
		memcpy( &pItems[ pItem->iSubItem ], pItem, sizeof( LVITEM ) );
		pItems[ pItem->iSubItem ].pszText = new _TCHAR[ _tcslen( pItem->pszText ) + sizeof( _TCHAR ) ];
		_tcscpy( pItems[ pItem->iSubItem ].pszText, pItem->pszText );
	}
	return TRUE;
}
int CWndListCtrl::InsertItem( const LVITEM* pItem )
{
//	if( pItem->iItem < m_aItems.GetSize() && m_aItems.GetAt( pItem->iItem ) ) 
	if( pItem->iItem > m_aItems.GetSize() ) 
			return -1; // 이미 존재한다.
	LVITEM* pNewItems = new LVITEM[ m_aColumns.GetSize() ];
	ZeroMemory( pNewItems, sizeof( LVITEM ) * m_aColumns.GetSize() );
	memcpy( &pNewItems[ pItem->iSubItem ], pItem, sizeof( LVITEM ) );
	pNewItems[ pItem->iSubItem ].pszText = new _TCHAR[ _tcslen( pItem->pszText ) + sizeof( _TCHAR ) ];
	_tcscpy( pNewItems[ pItem->iSubItem ].pszText, pItem->pszText );
	m_aItems.SetAtGrow( pItem->iItem, (void*)pNewItems );
	return pItem->iItem;
}
int CWndListCtrl::GetItemCount() const
{
	return m_aItems.GetSize();
}
int CWndListCtrl::InsertColumn( int nCol, const LVCOLUMN* pColumn )
{
	LVCOLUMN* pNewColumn = new LVCOLUMN;
	memcpy( pNewColumn, pColumn, sizeof( LVCOLUMN ) );
	pNewColumn->pszText = new _TCHAR[ _tcslen( pColumn->pszText ) + sizeof( _TCHAR ) ];
	_tcscpy( pNewColumn->pszText, pColumn->pszText );
	if( nCol < m_aColumns.GetSize() && m_aColumns.GetAt( nCol ) ) 
		return -1; // 이미 존재한다.
	m_aColumns.SetAtGrow( nCol, (void*)pNewColumn );
	// 컬럼 버튼 생성 
	/*
	CPtrArray m_aWndColumns;
	CRect rect;// = GetWindowRect();
	CWndButton* pWndButton = new CWndButton;
	pWndButton->AddWndStyle( WBS_DOCKING );
	rect = CRect( 0, 0, pColumn->cx, 20 );
	if( nCol )
	{
		LVCOLUMN* pNewColumn = (LVCOLUMN*)m_aColumns.GetAt( nCol - 1 );
		if( 
		i
	}
	pWndButton->Create( pColumn->pszText, 0, rect , this, nCol );
	m_aWndColumns.SetAtGrow( nCol, pWndButton );
	*/
	return nCol;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
tagWTCITEM::tagWTCITEM( void ) : 
mask( 0 ), 
dwState( 0 ), 
dwStateMask( 0 ), 
pszText( _T( "" ) ), 
cchTextMax( 0 ), 
iImage( 0 ), 
lParam( 0 ), 
pWndBase( NULL )
{
}
#endif // __IMPROVE_QUEST_INTERFACE
CWndTabCtrl::CWndTabCtrl()
{
	m_nTabButtonLength = 0;
	m_nCurSelect = 0;
	m_aTab.reserve( 10 );
#if __VER >= 19
	m_eTabTitleAlign = ALIGN_CENTER;
#else
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	m_eTabTitleAlign = ALIGN_LEFT;
#endif // __IMPROVE_QUEST_INTERFACE
#endif // __VER >= 19

//#ifndef __NEWTAB
	//ZeroMemory( m_apTexture, sizeof( m_apTexture ) );
//#endif
}
CWndTabCtrl::~CWndTabCtrl()
{
	vector< LPWTCITEM >::iterator itor;
	for( itor = m_aTab.begin(); itor != m_aTab.end(); itor++ )
		SAFE_DELETE( *itor );
//#ifndef __NEWTAB
//	for( int i = 0; i < 10; i++ )
//		SAFE_DELETE( m_apTexture[ i ] );
//#endif
}
BOOL CWndTabCtrl::Create(DWORD dwStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID)
{
	return CWndBase::Create( dwStyle | WBS_CHILD | WBS_NOFRAME /*| WBS_NODRAWFRAME*/, rect, pParentWnd, nID );//,pSprPack,nSprIdx,nColorTable);
}
HRESULT CWndTabCtrl::InitDeviceObjects()
{
	return CWndBase::InitDeviceObjects();
}
HRESULT CWndTabCtrl::RestoreDeviceObjects()
{
	return CWndBase::RestoreDeviceObjects();
}
HRESULT CWndTabCtrl::InvalidateDeviceObjects()
{
	return CWndBase::InvalidateDeviceObjects();
}
HRESULT CWndTabCtrl::DeleteDeviceObjects()
{
	return CWndBase::DeleteDeviceObjects();
}
void CWndTabCtrl::PaintFrame(C2DRender* p2DRender)
{
	return;
	CRect rectClient = GetClientRect();
	CRect rect;
	DWORD dwColor1 = D3DCOLOR_TEMP( 250, 255, 255, 255 );//D3DCOLOR_TEMP( 255, 130, 130, 230 );//
	DWORD dwColor2 = D3DCOLOR_TEMP( 50,    0,   0,  00 );//D3DCOLOR_TEMP( 255,  50,  50, 100 );//
	DWORD dwColor3 = D3DCOLOR_TEMP( 200, 150, 150, 150 );//D3DCOLOR_TEMP( 255, 180, 180, 220 );//

	CRect rect2 = rectClient;
	rect2.bottom -= 18;
	p2DRender->RenderRoundRect( rect2, D3DCOLOR_ARGB( 200, 200, 200, 200 ) );
	int y = rectClient.bottom - 18;
	if( m_aTab.size() )
	{
		int nLength = rectClient.Width() / m_aTab.size();
		for( int i = 0; i < (int)( m_aTab.size() ); i++ )
		{
			LPWTCITEM pItem = m_aTab[ i ];
			if( pItem )
			{
				CSize size = p2DRender->m_pFont->GetTextExtent( pItem->pszText );
				rect.SetRect( i * nLength, y, nLength + i * nLength, rectClient.bottom );
				p2DRender->TextOut( i * nLength + 2, y + 4, pItem->pszText, 0xffffffff);//ADEILSON 0xff000000
				p2DRender->TextOut( i * nLength + 2, y + 4, pItem->pszText, 0xffffffff);

				if( i != m_nCurSelect )
				{
					GradationRect( p2DRender, &rect, dwColor1 ,dwColor2, dwColor3 );
					p2DRender->RenderRoundRect( rect, D3DCOLOR_ARGB( 128, 200, 200, 200 ) );
					//p2DRender->RenderLine( CPoint(  i * nLength, y ), CPoint( nLength + i * nLength, y ), 0xff000000 );
					//p2DRender->RenderLine( CPoint(  i * nLength, y + 1), CPoint( nLength + i * nLength, y + 1), 0xff000000 );
					p2DRender->TextOut( i * nLength + 2, y + 4, pItem->pszText, 0xffffffff);//ADEILSON 0xff000000
					p2DRender->TextOut( i * nLength + 2, y + 4, pItem->pszText, 0xffffffff);

				}
				else
				{
					DWORD dwColor1_ = D3DCOLOR_ARGB( 255, 255, 255, 255 );//D3DCOLOR_ARGB( 255, 130, 130, 230 );//
					DWORD dwColor2_ = D3DCOLOR_ARGB( 255,  150, 150, 150 );//D3DCOLOR_ARGB( 255,  50,  50, 100 );//
					DWORD dwColor3_ = D3DCOLOR_ARGB( 255, 230, 230, 230 );//D3DCOLOR_ARGB( 255, 180, 180, 220 );//

					GradationRect( p2DRender, &rect, dwColor1_ ,dwColor2_, dwColor3_ );
					//p2DRender->RenderFillRect( rect, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
					p2DRender->TextOut( i * nLength + 2, y + 4, pItem->pszText, 0xffffffff);//ADEILSON 0xff000000

				}
			}
		}
	}
	rect = GetClientRect();
	rect.right -= 100;
//2DRender->RenderFillRect( rect, 0xffffffff );
}
void CWndTabCtrl::OnDraw( C2DRender* p2DRender )
{
	int nTabSize = m_aTab.size();

	if( m_nCurSelect >= 0 )
	{
		CRect rect = GetWindowRect();
		rect.right -= 15;
		int nHeight = ( rect.Height() / EDIT_HEIGHT ) - 2;
		rect.top += ( nHeight * EDIT_HEIGHT );

		if( nTabSize == 0 )
			nTabSize = 1;
		int nWidth = rect.Width() / 8;
		int nSize = ( nWidth / nTabSize );
		int nCurPos = nSize * m_nCurSelect;

		static const int TEXTURE_TAB_SIZE_X = m_aTexture[ 1 ].m_size.cx;
		static const int TEXTURE_TAB_SIZE_Y = m_aTexture[ 1 ].m_size.cy;

		CPoint pointStart = rect.TopLeft();
		pointStart.x += ( TEXTURE_TAB_SIZE_X * nCurPos );

		int nFirstTabTextureIndex = ( m_nCurSelect == 0 ) ? 0 : 1;
		m_aTexture[ nFirstTabTextureIndex ].Render( p2DRender, pointStart, CWndBase::m_nAlpha );
		m_aTexture[ 1 ].Render( p2DRender, 
								CPoint( pointStart.x + TEXTURE_TAB_SIZE_X, pointStart.y ), 
								CPoint( TEXTURE_TAB_SIZE_X, TEXTURE_TAB_SIZE_Y ), 
								CWndBase::m_nAlpha, 
								static_cast<FLOAT>( nSize - 3 ));
		m_aTexture[ 2 ].Render( p2DRender, CPoint( pointStart.x + ( TEXTURE_TAB_SIZE_X * ( nSize - 2 ) ), pointStart.y ), CWndBase::m_nAlpha );
		if( m_nCurSelect == nTabSize - 1 )
			m_aTexture[ 5 ].Render( p2DRender, CPoint( pointStart.x + ( TEXTURE_TAB_SIZE_X * ( nSize - 1 ) ), pointStart.y ), CWndBase::m_nAlpha );
		else
		{
			m_aTexture[ 3 ].Render( p2DRender, CPoint( pointStart.x + ( TEXTURE_TAB_SIZE_X * ( nSize - 1 ) ), pointStart.y ), CWndBase::m_nAlpha );
			m_aTexture[ 4 ].Render( p2DRender, CPoint( pointStart.x + ( TEXTURE_TAB_SIZE_X * ( nSize ) ), pointStart.y ), CWndBase::m_nAlpha );
		}
	}

	CRect rectClient = GetWindowRect();
	rectClient.right -= 15;
	int y = rectClient.bottom - 17;

	if( nTabSize > 0 )
	{
		int nLength = rectClient.Width() / nTabSize;
		for( int i = 0; i < nTabSize; ++i )
		{
			LPWTCITEM pItem = m_aTab[ i ];
			if( pItem )
			{
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
				switch( m_eTabTitleAlign )
				{
				case ALIGN_LEFT:
					{
						DWORD dwTextColor = ( i != m_nCurSelect ) ? 0xff6e6e6e : 0xffffffff;//ADEILSON 0xff000000
						p2DRender->TextOut( ( i * nLength ) + 2, y, pItem->pszText, dwTextColor );
						break;
					}
				case ALIGN_RIGHT:
					{
						DWORD dwTextColor = ( i != m_nCurSelect ) ? 0xff6e6e6e : 0xffffffff;//ADEILSON 0xff000000
						CSize size = p2DRender->m_pFont->GetTextExtent( pItem->pszText );
						if( size.cx < nLength )
						{
							int nRenderingPositionX = nLength - size.cx;
							p2DRender->TextOut( ( i * nLength ) + nRenderingPositionX, y, pItem->pszText, dwTextColor );
						}
						else
							p2DRender->TextOut( ( i * nLength ) + 2, y, pItem->pszText, dwTextColor );
						break;
					}
				case ALIGN_CENTER:
					{
						DWORD dwTextColor = ( i != m_nCurSelect ) ? 0xff6e6e6e : 0xffffffff;//ADEILSON 0xff000000
						CSize size = p2DRender->m_pFont->GetTextExtent( pItem->pszText );
						if( size.cx < nLength )
						{
							int nRenderingPositionX = ( nLength - size.cx ) / 2;
							p2DRender->TextOut( ( i * nLength ) + nRenderingPositionX, y, pItem->pszText, dwTextColor );
						}
						else
							p2DRender->TextOut( ( i * nLength ) + 2, y, pItem->pszText, dwTextColor );
						break;
					}
				default:
					{
						DWORD dwTextColor = ( i != m_nCurSelect ) ? 0xff6e6e6e : 0xffffffff;//ADEILSON 0xff000000
						p2DRender->TextOut( ( i * nLength ) + 2, y, pItem->pszText, dwTextColor );
					}
				}
#else // __IMPROVE_QUEST_INTERFACE
				if( i != m_nCurSelect )
					p2DRender->TextOut( i * nLength + 2, y, pItem->pszText, 0xff6e6e6e );
				else
					p2DRender->TextOut( i * nLength + 2, y, pItem->pszText, 0xffffffff );//ADEILSON 0xff000000
#endif // __IMPROVE_QUEST_INTERFACE
			}
		}
	}
}
void CWndTabCtrl::OnInitialUpdate()
{
	m_aTexture[ 0 ].LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "WndTabTile10.bmp" ), 0xffff00ff, TRUE );
	m_aTexture[ 1 ].LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "WndTabTile11.bmp" ), 0xffff00ff, TRUE );
	m_aTexture[ 2 ].LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "WndTabTile12.bmp" ), 0xffff00ff, TRUE );
	m_aTexture[ 3 ].LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "WndTabTile13.bmp" ), 0xffff00ff, TRUE );
	m_aTexture[ 4 ].LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "WndTabTile14.bmp" ), 0xffff00ff, TRUE );
	m_aTexture[ 5 ].LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "WndTabTile15.bmp" ), 0xffff00ff, TRUE );
}

void CWndTabCtrl::AdditionalSkinTexture( LPWORD pDest, CSize sizeSurface, D3DFORMAT d3dFormat )
{
	LPIMAGE lpImage[20];
	CString m_strTile = "WndEditTile06.tga";
	CString strFileName;
	
	int nImgBit = IMGBIT_32;
	for( int i = 0; i < 9; i++ )
	{
		CString strTemp1 = m_strTile.Left( m_strTile.GetLength() - 6 );
		CString strTemp2 = m_strTile.Right( 4 );
		strFileName.Format( "%s%02d%s", strTemp1, i, strTemp2 );
		if( m_strWndTileMap.Lookup( strFileName, (void*&)lpImage[i] ) == FALSE )
		{
			lpImage[i] = new IMAGE;
			
			if( LoadImage( MakePath( DIR_THEME, strFileName ), lpImage[i] ) == FALSE )
				Error( "CWndTabCtrl::AdditionalSkinTexture에서 %s Open1 실패", strFileName );

			m_strWndTileMap.SetAt( strFileName, lpImage[i] );
		}
	}
	///////////////////////////////////////////////////////
	CRect rect = GetWindowRect( TRUE );
	rect += ( m_pParentWnd->GetClientRect( TRUE ).TopLeft() - m_pParentWnd->GetWindowRect( TRUE ).TopLeft() );
	CRect rect2 = rect;
	//ect = GetClientRect();
	rect.right -= 15;
	int nWidth = rect.Width() / 8;
	int nHeight = ( rect.Height() / EDIT_HEIGHT );// - 2;
	CPoint point;
	for( i = 0; i < nHeight; i++ )
	{
		for( int j = 0; j < nWidth; j++ )
		{
			point = CPoint( j * 8, i * EDIT_HEIGHT ) + rect.TopLeft();
			if( i == 0 )
			{
				if( j == 0 )
					PaintTexture( pDest, lpImage[ 0 ], point, sizeSurface, d3dFormat );
				else
					if( j == nWidth - 1 )
						PaintTexture( pDest, lpImage[ 2 ], point, sizeSurface, d3dFormat );
					else
						PaintTexture( pDest, lpImage[ 1 ], point, sizeSurface, d3dFormat );
			}
			else
			if( i == nHeight - 1 )
			{
				if( j == 0 )
					PaintTexture( pDest, lpImage[ 6 ], point, sizeSurface, d3dFormat );
				else
					if( j == nWidth - 1 )
						PaintTexture( pDest, lpImage[ 8 ], point, sizeSurface, d3dFormat );
					else
						PaintTexture( pDest, lpImage[ 7 ], point, sizeSurface, d3dFormat );					
			}
			else
			{
				if( j == 0 )
					PaintTexture( pDest, lpImage[ 3 ], point, sizeSurface, d3dFormat );
				else
					if( j == nWidth - 1 )
						PaintTexture( pDest, lpImage[ 5 ], point, sizeSurface, d3dFormat );
					else
						PaintTexture( pDest, lpImage[ 4 ], point, sizeSurface, d3dFormat );				
			}
		}
	}
	m_strTile = "WndTabTile00.bmp";
	nImgBit = IMGBIT_32;
	for( i = 0; i < 6; i++ )
	{
		CString strTemp1 = m_strTile.Left( m_strTile.GetLength() - 6 );
		CString strTemp2 = m_strTile.Right( 4 );
		strFileName.Format( "%s%02d%s", strTemp1, i, strTemp2 );
		if( m_strWndTileMap.Lookup( strFileName, (void*&)lpImage[i] ) == FALSE )
		{
			lpImage[i] = new IMAGE;
			if( LoadImage( MakePath( DIR_THEME, strFileName ), lpImage[i] ) == FALSE )
				Error( "CWndTabCtrl::AdditionalSkinTexture에서 %s Open1 실패", strFileName );

			m_strWndTileMap.SetAt( strFileName, lpImage[i] );
		}
	}
	for( i = 10; i < 16; i++ )
	{
		CString strTemp1 = m_strTile.Left( m_strTile.GetLength() - 6 );
		CString strTemp2 = m_strTile.Right( 4 );
		strFileName.Format( "%s%02d%s", strTemp1, i, strTemp2 );
		if( m_strWndTileMap.Lookup( strFileName, (void*&)lpImage[i] ) == FALSE )
		{
			lpImage[i] = new IMAGE;
			if( LoadImage( MakePath( DIR_THEME, strFileName ), lpImage[i] ) == FALSE )
				Error( "CWndTabCtrl::AdditionalSkinTexture에서 %s Open1 실패", strFileName );

			m_strWndTileMap.SetAt( strFileName, lpImage[i] );
		}
	}
	///////////////////////////////////////////////////////
	nHeight -= 2;
	CRect rectTab = GetWindowRect( TRUE );
	rectTab += ( m_pParentWnd->GetClientRect( TRUE ).TopLeft() - m_pParentWnd->GetWindowRect( TRUE ).TopLeft() );
	rectTab.top = rectTab.top + nHeight * EDIT_HEIGHT;//rect.Height() / 8 rect.bottom - 20;
	//ect = GetClientRect();
	rectTab.right -= 15;
	nWidth = rectTab.Width() / 8;
	//nHeight = rect.Height() / 10;// ) - 2;
	int nTapSize = m_aTab.size();
	if( nTapSize == 0 )
		nTapSize = 1;
	int nSize = ( nWidth ) / nTapSize;//m_aTab.size() ;
	int nCurSize = 0;//e;
	for( i = 0; i < nWidth; i++ )
	{
		point = CPoint( i * 8, 0 ) + rectTab.TopLeft();
		if( i == nCurSize - 2 )
		{
			PaintTexture( pDest, lpImage[ 2 ], point, sizeSurface, d3dFormat );
			i++;
			point = CPoint( i * 8, 0 ) + rectTab.TopLeft();
			if( i == nWidth - 1 )
				PaintTexture( pDest, lpImage[ 5 ], point, sizeSurface, d3dFormat );
			else
				PaintTexture( pDest, lpImage[ 3 ], point, sizeSurface, d3dFormat );				
		}
		else
		if( i == nCurSize )
		{
			if( i == 0 )
				PaintTexture( pDest, lpImage[ 0 ], point, sizeSurface, d3dFormat );
			else
			{
				PaintTexture( pDest, lpImage[ 4 ], point, sizeSurface, d3dFormat );
			}
			nCurSize += nSize;
		}
		else
			PaintTexture( pDest, lpImage[ 1 ], point, sizeSurface, d3dFormat );
	}
 	for( i = 0; i < m_wndArray.GetSize(); i++ )
	{
		CWndBase* pWndBase = (CWndBase*)m_wndArray.GetAt( i );
		CRect rectOld = m_rectWindow;
		m_rectWindow.OffsetRect( rect.TopLeft() );
		if( pWndBase->IsDestroy() == FALSE && pWndBase->IsVisible() )
		{
 			pWndBase->AdditionalSkinTexture( pDest, sizeSurface, d3dFormat );
			for( int i = 0; i < pWndBase->m_wndArray.GetSize(); i++ )
			{
				CWndBase* pWndChild = (CWndBase*)pWndBase->m_wndArray.GetAt( i );
				CRect rectOldChild = pWndChild->m_rectWindow;
				pWndChild->m_rectWindow.OffsetRect( rect.TopLeft() );
				if( pWndChild->IsDestroy() == FALSE && pWndChild->IsVisible() )
					pWndChild->AdditionalSkinTexture( pDest, sizeSurface, d3dFormat );
				pWndChild->m_rectWindow = rectOldChild;
			}
			pWndBase->AfterSkinTexture( pDest, sizeSurface );
		}
		m_rectWindow = rectOld;
	}
}


void CWndTabCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( m_aTab.size() == 0 ) return;

	CRect rectClient = GetClientRect();
	CRect rect;
	CRect rect2 = rectClient;
	rect2.bottom -= 18;
	int y = rectClient.bottom - 18;

	LPWTCITEM pItem;
	int nSelect = -1;
	if( m_aTab.size() )
	{
		for( int i = 0; i < (int)( m_aTab.size() ); i++ )
		{
			pItem = m_aTab[ i ];
			if( pItem )
			{
				rect.SetRect( i * m_nTabButtonLength, y, m_nTabButtonLength + i * m_nTabButtonLength, rectClient.bottom );
				if( rect.PtInRect( point ) )
					nSelect = i;
			}
		}
	}
	if( nSelect != -1 )
	{
		SetCurSel( nSelect );
		CWndBase* pParent = (CWndBase*)GetParentWnd();
		pParent->OnChildNotify( WNM_SELCHANGE, GetWndId(), (LRESULT*) this ); 
	}
}
BOOL CWndTabCtrl::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	CWndBase* pParent = (CWndBase*)GetParentWnd();
	return pParent->OnChildNotify( message,nID,pLResult); 
}

int CWndTabCtrl::GetCurSel() const
{
	return m_nCurSelect;
}
int CWndTabCtrl::SetCurSel( int nItem )
{
	int nOldSelect = m_nCurSelect;
	LPWTCITEM pItem;
	CRect rect;
	m_nCurSelect = nItem;
	if( m_aTab[ m_nCurSelect ]->pWndBase )
	{
		pItem = m_aTab[ nOldSelect ];
		pItem->pWndBase->SetVisible( FALSE );
		
		if( m_nCurSelect >= (int)( m_aTab.size() ) )
		{
			CString string;
			string.Format( "CWndTabCtrl::SetCurSel에서 nItem이 범위를 넘어섬 : nItem(%d) m_aTabSize(%d)", m_nCurSelect, m_aTab.size() );
			ADDERRORMSG( string );
		}

		pItem = m_aTab[ m_nCurSelect ];
		pItem->pWndBase->SetVisible( TRUE );
		rect = GetWindowRect( TRUE );
		if( IsWndStyle( WBS_VSCROLL ) )
			rect.right -= 15;
		rect.bottom -= 18;
		for( int i = 0; i < (int)( m_aTab.size() ); i++ )
		{
			LPWTCITEM pItem = m_aTab[ i ];
			if( pItem && pItem->pWndBase )
			{
				rect.OffsetRect( -rect.TopLeft() );
				pItem->pWndBase->SetWndRect( rect );
			}
		}
		GetFrameWnd()->AdjustWndBase();
		m_aTab[ m_nCurSelect ]->pWndBase->SetFocus();
	}
	else
		m_nCurSelect = nOldSelect;
	return nItem;
}
BOOL CWndTabCtrl::GetItem(int nItem, WTCITEM* pTabCtrlItem) const
{
	if( nItem >= (int)( m_aTab.size() ) )
		return FALSE;
	memcpy( pTabCtrlItem, m_aTab[ nItem ], sizeof( WTCITEM ) );
	return TRUE;
}
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
LPWTCITEM CWndTabCtrl::GetTabItem( int nItemNumber ) const
{
	if( nItemNumber < 0 || nItemNumber >= (int)( m_aTab.size() ) )
		return NULL;
	return m_aTab[ nItemNumber ];
}

LPWTCITEM CWndTabCtrl::GetSelectedTab( void ) const
{
	return m_aTab[ m_nCurSelect ];
}

void CWndTabCtrl::SetTabTitleAlign( const CWndTabCtrl::TabTitleAlign eTabTitleAlign )
{
	m_eTabTitleAlign = eTabTitleAlign;
}

const CWndTabCtrl::TabTitleAlign CWndTabCtrl::GetTabTitleAlign( void ) const
{
	return m_eTabTitleAlign;
}
#endif // __IMPROVE_QUEST_INTERFACE
BOOL CWndTabCtrl::InsertItem( int nItem, WTCITEM* pTabCtrlItem )
{
	if( nItem >= (int)( m_aTab.size() ) )
		m_aTab.resize( nItem + 1 );//serve( nItem + 1 );
	LPWTCITEM pItem = new WTCITEM;
	memcpy( pItem, pTabCtrlItem, sizeof( WTCITEM ) );
	m_aTab[ nItem ] = pItem;

	if( pTabCtrlItem->pWndBase )
	{
		if( nItem == m_nCurSelect )
			pTabCtrlItem->pWndBase->SetVisible( TRUE );
		else
			pTabCtrlItem->pWndBase->SetVisible( FALSE );
		CRect rect = GetClientRect( TRUE );
		rect.bottom -= 18;
		rect.OffsetRect( -rect.TopLeft() );
		if( IsWndStyle( WBS_VSCROLL ) )
			rect.right -= 15;

		pItem->pWndBase->SetWndRect( rect );
		pItem->pWndBase->AddWndStyle( WBS_CHILDFRAME );
		// 언젠 처음 것에 포커스가 가게 한다.
		if( m_aTab[ m_nCurSelect ] )
			m_aTab[ m_nCurSelect ]->pWndBase->SetFocus();
	}
	m_nTabButtonLength = GetClientRect().Width() / m_aTab.size();// - 10;
	
/*
	// 탭 클릭할 부분의 영역을 설정.
	rect = GetClientRect();
	int nLength = rect.Width() / m_aTab.size() - 10;
	rect.SetRect( nItem * nLength, y, nLength + nItem * nLength, rect.bottom );
	m_rect[ nItem ] = rect;
*/
	return TRUE;
}
BOOL CWndTabCtrl::InsertTexture( int nItem, LPCTSTR lpszFileName )
{
	//m_apTexture[ nItem ] = new CTexture;
	//m_apTexture[ nItem ]->LoadTexture( m_pApp->m_pd3dDevice, lpszFileName, 0xffff00ff, TRUE );
	return TRUE;
}
void CWndTabCtrl::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	CWndBase::SetWndRect( rectWnd, bOnSize );
}
BOOL CWndTabCtrl::InsertItem( int nItem, LPCTSTR lpszItem )
{
	return TRUE;
}

void CWndTabCtrl::OnSize(UINT nType, int cx, int cy)
{
	CRect rect = GetClientRect( TRUE );
//	rect.DeflateRect( 4, 6 );
	//rect.top    += 15;
	rect.bottom -= 18;
	//m_wndListCtrl.SetWndRect( rect );
//	if( IsWndStyle( WBS_VSCROLL ) )
//		rect.right -= 15;
	
	for( int i = 0; i < (int)( m_aTab.size() ); i++ )
	{
		LPWTCITEM pItem = m_aTab[ i ];
		if( pItem && pItem->pWndBase )
		{
			rect.OffsetRect( -rect.TopLeft() );
			pItem->pWndBase->SetWndRect( rect );
		}
	}
	if(  m_aTab.size() )
		m_nTabButtonLength = GetClientRect().Width() / m_aTab.size();// - 10;
	
	CWndBase::OnSize(nType,cx,cy);
}
//////////////////////////////////////////////////////////////////////////////
// CWndComboBox
//////////////////////////////////////////////////////////////////////////////

#ifdef __IMPROVE_MAP_SYSTEM
void CWndComboListBox::PaintFrame( C2DRender* p2DRender )
{
	m_pTheme->RenderWndEditFrame( p2DRender, &GetWindowRect() );
}
#endif // __IMPROVE_MAP_SYSTEM

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
void CWndComboListBox::OnKillFocus( CWndBase* pNewWnd )
{
	m_pParentWnd->OnChildNotify( WNM_KILLFOCUS, m_nIdWnd, ( LRESULT* )this );

	CWndBase::OnKillFocus( pNewWnd );
}
#endif // __IMPROVE_QUEST_INTERFACE

CWndComboBox::CWndComboBox()
{
	CWndListBox::CWndListBox();
    m_strTexture = DEF_CTRL_TEXT;
	m_bTile = TRUE;
//	m_dwComboBoxStyle = 0; 
	m_byWndType = WTYPE_COMBOBOX;
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	m_bOpen = FALSE;
#endif // __IMPROVE_QUEST_INTERFACE
}
CWndComboBox::~CWndComboBox()
{
	ResetContent();
}
void CWndComboBox::OnInitialUpdate()
{
	CRect rect = GetWindowRect();
	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );//,m_pSprPack,-1);

	//CRect rect = GetWindowRect();
	rect.left = rect.right - 20;
	m_wndButton.AddWndStyle( WBS_DOCKING );
	m_wndButton.Create( _T( "" ), WBS_CHILD, rect, this, 0 );
	m_wndButton.m_pTexture = m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtQuickListDn.tga" ), 0xffff00ff, TRUE );
	rect = GetWindowRect();
	ClientToScreen( &rect );
	rect.top = rect.bottom;
	rect.bottom += 100;

	//m_wndListBox.Create( 0, rect, m_pWndRoot, 1 );
	m_wndListBox.CWndBase::Create( WBS_POPUP | WBS_VSCROLL, rect, this, 1 );
	m_wndListBox.m_strTitle = _T( "Combo ListBox" );
	m_wndListBox.m_strTexture = m_strTexture;
	m_wndListBox.m_bTile = m_bTile;
	m_wndListBox.AdjustWndBase();
	m_wndListBox.RestoreDeviceObjects();
	m_wndListBox.SetVisible( FALSE );	

}	

void CWndComboBox::Create( DWORD dwComboBoxStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID )
{
	CWndEdit::Create( pParentWnd->m_pApp->GetSafeHwnd(), dwComboBoxStyle | WBS_CHILD, rect, pParentWnd, nID );
}
void CWndComboBox::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	CWndEdit::SetWndRect( rectWnd, bOnSize );
}

void CWndComboBox::PaintFrame( C2DRender* p2DRender )
{
#ifdef __IMPROVE_MAP_SYSTEM
	m_pTheme->RenderWndEditFrame( p2DRender, &GetWindowRect() );
#else // __IMPROVE_MAP_SYSTEM
	CWndEdit::PaintFrame( p2DRender);
#endif // __IMPROVE_MAP_SYSTEM
}

void CWndComboBox::OnDraw( C2DRender* p2DRender )
{
	CWndEdit::OnDraw( p2DRender);
	//CWndListBox::OnDraw(p2DRender);
}
void CWndComboBox::OnLButtonDown( UINT nFlags, CPoint point )
{
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	if( m_bOpen == TRUE )
		m_bOpen = FALSE;
	else
	{
		m_bOpen = TRUE;
		OpenListBox();
	}
#endif // __IMPROVE_QUEST_INTERFACE
}

void CWndComboBox::OnLButtonUp( UINT nFlags, CPoint point )
{
}
BOOL CWndComboBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( nID == 0 && message == WNM_CLICKED) // list
	{
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
		if( m_bOpen == TRUE )
			m_bOpen = FALSE;
		else
		{
			m_bOpen = TRUE;
			OpenListBox();
		}
#else // __IMPROVE_QUEST_INTERFACE
		CRect rect = GetScreenRect();//GetWindowRect( );
		rect.top = rect.bottom;
		rect.bottom += 200;

		m_wndListBox.Move( rect.TopLeft() );
		m_wndListBox.SetVisible( TRUE );
		m_wndListBox.m_bTile = m_bTile;
		m_wndListBox.AdjustWndBase();
		m_wndListBox.SetFocus();
#endif // __IMPROVE_QUEST_INTERFACE
	}
	if( nID == 1 && message == WNM_SELCHANGE) // list
	{
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
		if( m_bOpen == TRUE )
		{
#ifdef __IMPROVE_MAP_SYSTEM
			SetSelectedItemInformation();
#else // __IMPROVE_MAP_SYSTEM
			CString string;
			m_wndListBox.GetText( m_wndListBox.GetCurSel(), string );
			SetString( string );
			CWndBase* pWnd = m_pParentWnd;
			pWnd->OnChildNotify( WNM_SELCHANGE, m_nIdWnd, ( LRESULT* )this );
#endif // __IMPROVE_MAP_SYSTEM
			SetFocus();
			m_wndListBox.SetVisible( FALSE );
			m_bOpen = FALSE;
		}
#else // __IMPROVE_QUEST_INTERFACE
		CString string;
		m_wndListBox.GetText( m_wndListBox.GetCurSel(), string );
		SetString( string );
		m_wndListBox.SetVisible( FALSE );
		CWndBase* pWnd = m_pParentWnd;
		pWnd->OnChildNotify( WNM_SELCHANGE, m_nIdWnd, (LRESULT*)this); 
#endif // __IMPROVE_QUEST_INTERFACE
	}
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	if( nID == 1 && message == WNM_KILLFOCUS )
	{
		CPoint point = GetMousePoint();
		CRect rtComboBox = GetClientRect();
		CRect rtButton = m_wndButton.GetClientRect();
		if( rtComboBox.PtInRect( point ) == TRUE || rtButton.PtInRect( point ) == TRUE )
			m_bOpen = TRUE;
		else
		{
			if( m_bOpen == TRUE )
				m_bOpen = FALSE;
		}
	}
#endif // __IMPROVE_QUEST_INTERFACE
	return TRUE;
}
// manipulating listbox items
int CWndComboBox::AddString( LPCTSTR lpszString )
{
	int nNum = m_wndListBox.AddString( lpszString );
	CRect rect = m_wndListBox.GetWindowRect( TRUE );

#ifdef __19_INTERFACE_UPDATE

	if( nNum >= 6 )
	{
		nNum = 7;
	}

	rect.bottom = rect.top + ( ( nNum + 1 ) * ( m_pFont->GetMaxHeight() + 3 ) ) + 8;

#else//__19_INTERFACE_UPDATE
	rect.bottom = rect.top + ( ( nNum + 1 ) * ( m_pFont->GetMaxHeight() + 3 ) ) + 8;
#endif//__19_INTERFACE_UPDATE

	m_wndListBox.SetWndRect( rect );
	return nNum;
}
int CWndComboBox::DeleteString( UINT nIndex )
{
	return m_wndListBox.DeleteString( nIndex );
}
int CWndComboBox::InsertString( int nIndex, LPCTSTR lpszString )
{
	return 1;
}
void CWndComboBox::ResetContent()
{
#ifdef __IMPROVE_MAP_SYSTEM
	SetString( _T( "" ) );
#endif // __IMPROVE_MAP_SYSTEM
	m_wndListBox.ResetContent();
}
#ifdef __IMPROVE_MAP_SYSTEM
void CWndComboBox::SelectItem( const CString& strItem )
{
	int nIndex = m_wndListBox.GetItemIndex( strItem );
	if( nIndex == -1 )
	{
		return;
	}

	m_wndListBox.SetCurSel( nIndex );
	SetSelectedItemInformation();
}
void CWndComboBox::SelectItem( DWORD dwItem )
{
	int nIndex = m_wndListBox.GetItemIndex( dwItem );
	if( nIndex == -1 )
	{
		return;
	}

	m_wndListBox.SetCurSel( nIndex );
	SetSelectedItemInformation();
}
void CWndComboBox::SetSelectedItemInformation( void )
{
	CString strSelectedItem = _T( "" );
	m_wndListBox.GetText( m_wndListBox.GetCurSel(), strSelectedItem );
	SetString( strSelectedItem );
	CWndBase* pWnd = m_pParentWnd;
	if( pWnd )
	{
		pWnd->OnChildNotify( WNM_SELCHANGE, m_nIdWnd, ( LRESULT* )this );
	}
}
#endif // __IMPROVE_MAP_SYSTEM
DWORD CWndComboBox::GetItemData( int nIndex ) const
{
	return m_wndListBox.GetItemData( nIndex );
}
int CWndComboBox::SetItemData( int nIndex, DWORD dwItemData )
{
	return m_wndListBox.SetItemData( nIndex, dwItemData );
}
int CWndComboBox::GetCurSel() const
{
	return m_wndListBox.GetCurSel();
}
int CWndComboBox::SetCurSel( int nSelect )
{
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	assert( nSelect >= 0 && nSelect < m_wndListBox.GetCount() );
#endif // __IMPROVE_QUEST_INTERFACE
	CString string;
	m_wndListBox.GetText( nSelect, string );
	SetString( string );
	return m_wndListBox.SetCurSel( nSelect );
}
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
void CWndComboBox::OpenListBox( void )
{
	CRect rect = GetScreenRect();
	rect.top = rect.bottom;
	rect.bottom += 200;

	m_wndListBox.Move( rect.TopLeft() );
	m_wndListBox.SetVisible( TRUE );
	m_wndListBox.m_bTile = m_bTile;
	m_wndListBox.AdjustWndBase();
	m_wndListBox.SetFocus();
}
#endif // __IMPROVE_QUEST_INTERFACE
#ifdef __IMPROVE_MAP_SYSTEM
DWORD CWndComboBox::GetSelectedItemData( void ) const
{
	int nSelectedListNumber = m_wndListBox.GetCurSel();
	if( nSelectedListNumber == -1 )
	{
		return 0;
	}
	return m_wndListBox.GetItemData( nSelectedListNumber );
}
void CWndComboBox::GetListBoxText( int nIndex, CString& strString ) const
{
	m_wndListBox.GetText( nIndex, strString );
}
int CWndComboBox::GetListBoxTextLength( int nIndex ) const
{
	return m_wndListBox.GetTextLen( nIndex );
}
int CWndComboBox::GetListBoxSize( void ) const
{
	return m_wndListBox.GetCount();
}
#endif // __IMPROVE_MAP_SYSTEM