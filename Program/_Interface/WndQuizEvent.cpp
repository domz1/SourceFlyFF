// WndQuizEvent.cpp: implementation of the CWndQuizEvent class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WndQuizEvent.h"
#ifdef __QUIZ
#include "Quiz.h"
#include "ResData.h"
#include "defineText.h"
#include "DPClient.h"
extern	CDPClient	g_DPlay;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWndQuizEventConfirm::CWndQuizEventConfirm( BOOL bEnter )
:m_bEnter( bEnter )
{
}

CWndQuizEventConfirm::~CWndQuizEventConfirm()
{
}

void CWndQuizEventConfirm::OnDraw( C2DRender* p2DRender )
{
}

BOOL CWndQuizEventConfirm::Initialize( CWndBase* pWndParent, DWORD nType )
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_QUIZ_CONFIRM, 0, CPoint( 0, 0 ), pWndParent );
}

void CWndQuizEventConfirm::OnInitialUpdate( void )
{
	CWndNeuz::OnInitialUpdate();
	MoveParentCenter();
}

BOOL CWndQuizEventConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case WIDC_YES:
		{
			if( m_bEnter )
				g_DPlay.SendQuizEventEntrance();

			else
				g_DPlay.SendQuizEventTeleport( CQuiz::ZONE_EXIT );

			Destroy();
			break;
		}
	case WIDC_NO:
		{
			Destroy();
			break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

void CWndQuizEventConfirm::SetString( const char* lpszMessage )
{
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	
	pWndText->m_string.AddParsingString( lpszMessage  );
	pWndText->ResetString();
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWndQuizEventQuestionOX::CWndQuizEventQuestionOX()
{
}

CWndQuizEventQuestionOX::~CWndQuizEventQuestionOX()
{
}

BOOL CWndQuizEventQuestionOX::Initialize( CWndBase* pWndParent, DWORD nType )
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_QUIZ_QUESTION_OX, 0, CPoint( 0, 0 ), pWndParent );
}

void CWndQuizEventQuestionOX::OnInitialUpdate( void )
{
	CWndNeuz::OnInitialUpdate();
	CWndBase* pWndBase = (CWndBase*)g_WndMng.GetApplet( APP_INVENTORY );

	if( pWndBase )
		pWndBase->Destroy();
	
	m_texChar.DeleteDeviceObjects();
	m_texChar.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( "char\\", "char_Juria_JAP.tga" ), 0xffff00ff, TRUE );
	
	MoveParentCenter();
}

BOOL CWndQuizEventQuestionOX::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case WIDC_O:
		{
			if( CQuiz::GetInstance()->GetState() != CQuiz::QE_CORRECT_ANSWER
				&& CQuiz::GetInstance()->GetState() != CQuiz::QE_DROP_OUT 
				&& CQuiz::GetInstance()->GetZoneType( g_pPlayer ) == CQuiz::ZONE_QUIZ )
				g_DPlay.SendQuizEventTeleport( CQuiz::ZONE_1 );
			else
				g_WndMng.PutString( prj.GetText( TID_GAME_QUIZ_DO_NOT_TELEPORT ), NULL, prj.GetTextColor( TID_GAME_QUIZ_DO_NOT_TELEPORT ) );
			Destroy();
			break;
		}
	case WIDC_X:
		{
			if( CQuiz::GetInstance()->GetState() != CQuiz::QE_CORRECT_ANSWER
				&& CQuiz::GetInstance()->GetState() != CQuiz::QE_DROP_OUT 
				&& CQuiz::GetInstance()->GetZoneType( g_pPlayer ) == CQuiz::ZONE_QUIZ )
				g_DPlay.SendQuizEventTeleport( CQuiz::ZONE_2 );
			else
				g_WndMng.PutString( prj.GetText( TID_GAME_QUIZ_DO_NOT_TELEPORT ), NULL, prj.GetTextColor( TID_GAME_QUIZ_DO_NOT_TELEPORT ) );
			Destroy();
			break;
		}
	case WIDC_CLOSE:
		{
			Destroy();
			break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

void CWndQuizEventQuestionOX::OnDraw( C2DRender* p2DRender )
{
	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
	m_texChar.Render( p2DRender, lpWndCtrl->rect.TopLeft(), lpWndCtrl->rect.BottomRight() );
}

void CWndQuizEventQuestionOX::UpdateQuestion( const char* lpszQuestion )
{
	CWndText* pText   = (CWndText*)GetDlgItem( WIDC_TEXT1 );

	if( lpszQuestion != NULL )
		pText->SetString( lpszQuestion, 0xffffffff );//ADEILSON 0xff000000
	else
		pText->SetString( CQuiz::GetInstance()->m_strQuestion.c_str(), 0xffffffff );//ADEILSON 0xff000000
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWndQuizEventQuestion4C::CWndQuizEventQuestion4C()
{
}

CWndQuizEventQuestion4C::~CWndQuizEventQuestion4C()
{
}

BOOL CWndQuizEventQuestion4C::Initialize( CWndBase* pWndParent, DWORD nType )
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_QUIZ_QUESTION_4C, 0, CPoint( 0, 0 ), pWndParent );
}

void CWndQuizEventQuestion4C::OnInitialUpdate( void )
{
	CWndNeuz::OnInitialUpdate();
	CWndBase* pWndBase = (CWndBase*)g_WndMng.GetApplet( APP_INVENTORY );

	if( pWndBase )
		pWndBase->Destroy();
	
	m_texChar.DeleteDeviceObjects();
	m_texChar.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( "char\\", "char_Juria_JAP.tga" ), 0xffff00ff, TRUE );
	
	MoveParentCenter();
}

BOOL CWndQuizEventQuestion4C::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case WIDC_NO1:
		{
			if( CQuiz::GetInstance()->GetState() != CQuiz::QE_CORRECT_ANSWER
				&& CQuiz::GetInstance()->GetState() != CQuiz::QE_DROP_OUT 
				&& CQuiz::GetInstance()->GetZoneType( g_pPlayer ) == CQuiz::ZONE_QUIZ )
				g_DPlay.SendQuizEventTeleport( CQuiz::ZONE_1 );
			else
				g_WndMng.PutString( prj.GetText( TID_GAME_QUIZ_DO_NOT_TELEPORT ), NULL, prj.GetTextColor( TID_GAME_QUIZ_DO_NOT_TELEPORT ) );
			Destroy();
			break;
		}
	case WIDC_NO2:
		{
			if( CQuiz::GetInstance()->GetState() != CQuiz::QE_CORRECT_ANSWER
				&& CQuiz::GetInstance()->GetState() != CQuiz::QE_DROP_OUT 
				&& CQuiz::GetInstance()->GetZoneType( g_pPlayer ) == CQuiz::ZONE_QUIZ )
				g_DPlay.SendQuizEventTeleport( CQuiz::ZONE_2 );
			else
				g_WndMng.PutString( prj.GetText( TID_GAME_QUIZ_DO_NOT_TELEPORT ), NULL, prj.GetTextColor( TID_GAME_QUIZ_DO_NOT_TELEPORT ) );
			Destroy();
			break;
		}
	case WIDC_NO3:
		{
			if( CQuiz::GetInstance()->GetState() != CQuiz::QE_CORRECT_ANSWER
				&& CQuiz::GetInstance()->GetState() != CQuiz::QE_DROP_OUT 
				&& CQuiz::GetInstance()->GetZoneType( g_pPlayer ) == CQuiz::ZONE_QUIZ )
				g_DPlay.SendQuizEventTeleport( CQuiz::ZONE_3 );
			else
				g_WndMng.PutString( prj.GetText( TID_GAME_QUIZ_DO_NOT_TELEPORT ), NULL, prj.GetTextColor( TID_GAME_QUIZ_DO_NOT_TELEPORT ) );
			Destroy();
			break;
		}
	case WIDC_NO4:
		{
			if( CQuiz::GetInstance()->GetState() != CQuiz::QE_CORRECT_ANSWER
				&& CQuiz::GetInstance()->GetState() != CQuiz::QE_DROP_OUT 
				&& CQuiz::GetInstance()->GetZoneType( g_pPlayer ) == CQuiz::ZONE_QUIZ )
				g_DPlay.SendQuizEventTeleport( CQuiz::ZONE_4 );
			else
				g_WndMng.PutString( prj.GetText( TID_GAME_QUIZ_DO_NOT_TELEPORT ), NULL, prj.GetTextColor( TID_GAME_QUIZ_DO_NOT_TELEPORT ) );
			Destroy();
			break;
		}
	case WIDC_CLOSE:
		{
			Destroy();
			break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

void CWndQuizEventQuestion4C::OnDraw( C2DRender* p2DRender )
{
	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
	m_texChar.Render( p2DRender, lpWndCtrl->rect.TopLeft(), lpWndCtrl->rect.BottomRight() );
}

void CWndQuizEventQuestion4C::UpdateQuestion( const char* lpszQuestion )
{
	CWndText* pText   = (CWndText*)GetDlgItem( WIDC_TEXT1 );

	if( lpszQuestion != NULL )
		pText->SetString( lpszQuestion, 0xffffffff );//ADEILSON 0xff000000
	else
		pText->SetString( CQuiz::GetInstance()->m_strQuestion.c_str(), 0xffffffff );//ADEILSON 0xff000000
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWndQuizEventButton::CWndQuizEventButton()
{
	m_bLoadTexMap = FALSE;
	m_bFocus = FALSE;
	m_bPush = FALSE;
	m_ptPush.x = -1;
	m_ptPush.y = -1;
	m_nAlpha = 255;
	m_bReverse = TRUE;
}

CWndQuizEventButton::~CWndQuizEventButton()
{
}

HRESULT CWndQuizEventButton::DeleteDeviceObjects()
{
	m_BtnTexture.DeleteDeviceObjects();
	return CWndNeuz::DeleteDeviceObjects();
}

HRESULT CWndQuizEventButton::RestoreDeviceObjects()
{
	m_BtnTexture.RestoreDeviceObjects(g_Neuz.m_pd3dDevice);
	return CWndNeuz::RestoreDeviceObjects();
}

HRESULT CWndQuizEventButton::InvalidateDeviceObjects()
{
	m_BtnTexture.InvalidateDeviceObjects();
	return CWndNeuz::InvalidateDeviceObjects();
}

void CWndQuizEventButton::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate();
	// 여기에 코딩하세요
	this->DelWndStyle(WBS_CAPTION);
	m_wndTitleBar.SetVisible( FALSE );

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point;
	
	//point.x = rectRoot.right - rectWindow.Width() - 5;
	point.x = rectRoot.left + 10;
	point.y = rectRoot.top + rectWindow.Height() + 100;

	Move( point );

	m_bLoadTexMap = m_BtnTexture.LoadScript( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, _T( "texMapQuizEventButton.inc" ) ) );
} 

void CWndQuizEventButton::PaintFrame( C2DRender* p2DRender )
{
}

void CWndQuizEventButton::OnLButtonDown( UINT nFlags, CPoint point )
{
	CRect rectWindow = GetWindowRect();
	if(rectWindow.PtInRect(point))
	{
		m_bPush = TRUE;
		ClientToScreen( &point );
		m_ptPush = point;
	}
	else
		m_bPush = FALSE;
}

void CWndQuizEventButton::OnLButtonUp( UINT nFlags, CPoint point )
{
	CPoint ptScreen = point;
	ClientToScreen( &ptScreen );
	if( m_ptPush == ptScreen )
	{
		CRect rectWindow = GetWindowRect();
		if( rectWindow.PtInRect( point ) )
		{
			if( CQuiz::GetInstance()->GetType() == TYPE_OX )
			{
				if( g_WndMng.m_pWndQuizEventQuestionOX )
				{
					SAFE_DELETE( g_WndMng.m_pWndQuizEventQuestionOX );
				}
				else
				{
					g_WndMng.m_pWndQuizEventQuestionOX = new CWndQuizEventQuestionOX;
					if( g_WndMng.m_pWndQuizEventQuestionOX )
					{
						g_WndMng.m_pWndQuizEventQuestionOX->Initialize();
						g_WndMng.m_pWndQuizEventQuestionOX->UpdateQuestion();
					}
				}
			}
			else
			{
				if( g_WndMng.m_pWndQuizEventQuestion4C )
				{
					SAFE_DELETE( g_WndMng.m_pWndQuizEventQuestion4C );
				}
				else
				{
					g_WndMng.m_pWndQuizEventQuestion4C = new CWndQuizEventQuestion4C;
					if( g_WndMng.m_pWndQuizEventQuestion4C )
					{
						g_WndMng.m_pWndQuizEventQuestion4C->Initialize();
						g_WndMng.m_pWndQuizEventQuestion4C->UpdateQuestion();
					}
				}
			}
		}
	}
	else
	{
		m_ptPush.x = -1;
		m_ptPush.y = -1;
	}

	m_bPush = FALSE;
}

BOOL CWndQuizEventButton::Process()
{
	CPoint ptMouse = GetMousePoint();

	CRect rectWindow = GetWindowRect();
	if( rectWindow.PtInRect( ptMouse ) )
		m_bFocus = TRUE;
	else
		m_bFocus = FALSE;

	if( m_bReverse )
		m_nAlpha -= 5;
	else
		m_nAlpha += 5;

	if( m_nAlpha < 100 )
	{
		m_bReverse = FALSE;
		m_nAlpha = 100;
	}
	else if( m_nAlpha > 255 )
	{
		m_bReverse = TRUE;
		m_nAlpha = 255;
	}

	return TRUE;
}

BOOL CWndQuizEventButton::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_QUIZ_BUTTON, WBS_NOFOCUS, CPoint( 0, 0 ), pWndParent );
}

void CWndQuizEventButton::OnDraw( C2DRender* p2DRender )
{
	if( m_bLoadTexMap )
	{
		int nTexNum = -1;
		BOOL bNormal = FALSE;
		CTexture* pTexture = NULL;
		
		if( CQuiz::GetInstance()->GetState() == CQuiz::QE_QUESTION )
		{
			if( !m_bFocus && !m_bPush )
			{
				nTexNum = 0;
				bNormal = TRUE;
			}
			else if( m_bFocus && !m_bPush )
				nTexNum = 0;
			else if( m_bFocus && m_bPush )
				nTexNum = 2;
		}
		else
			nTexNum = 2;
		
		if( nTexNum != -1 )
			pTexture = m_BtnTexture.GetAt( nTexNum );

		if( pTexture )
		{
			if( bNormal )
				pTexture->Render( &g_Neuz.m_2DRender, CPoint(0, 0), m_nAlpha );
			else if( nTexNum == 0 || nTexNum == 2 )
				pTexture->Render( &g_Neuz.m_2DRender, CPoint(0, 0) );
		}
	}
}

#endif // __QUIZ