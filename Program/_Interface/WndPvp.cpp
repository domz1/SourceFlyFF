#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndPvp.h"
#include "WndManager.h"

#include "DPClient.h"
extern	CDPClient	g_DPlay;

/****************************************************
  WndId : APP_PVP - PVP
  CtrlId : WIDC_TABCTRL1 - 
****************************************************/

CWndPvp::CWndPvp() 
{ 
} 
CWndPvp::~CWndPvp() 
{ 
} 
void CWndPvp::OnDraw( C2DRender* p2DRender ) 
{ 
	CRect rect = GetClientRect();
	rect.bottom -= 30;
	//	p2DRender->RenderFillRect( rect, 0xff00ffff );
	int y = 0, nNext = 16;
	DWORD dwColor = D3DCOLOR_ARGB(255,0,0,0);
	//	TCHAR szString[ 32 ];
	//	p2DRender->TextOut( 5, y, _T( "Name"  ) ); y += nNext;
	//p2DRender->TextOut( 5, y, _T( "Style" ) ); y += nNext;
	//	p2DRender->TextOut( 5, y, _T( "Job"   ) ); y += nNext;
	//p2DRender->TextOut( 5, y, _T( "Guild" ) ); y += nNext;
	//	p2DRender->TextOut( 5, y, _T( "Level" ) ); y += nNext;
	//p2DRender->TextOut( 5, y, _T( "Exp1." ) ); y += nNext;
	//p2DRender->TextOut( 5, y, _T( "Exp2." ) ); y += nNext;
	//p2DRender->TextOut( 5, y, _T( "Gold"  ) ); y += nNext;

	char szBuff[32];
	y = 15;

	strcpy( szBuff, g_pPlayer->GetFameName() );
	if( IsEmpty(szBuff) ) {	szBuff[0] = '-'; szBuff[1] = NULL; }
	p2DRender->TextOut( 90, y, szBuff, dwColor); y += nNext;
#if __VER < 8 // __S8_PK
	strcpy( szBuff, g_pPlayer->GetSlaughterName() );
	if( IsEmpty(szBuff) ) {	szBuff[0] = '-'; szBuff[1] = NULL; }
	p2DRender->TextOut( 90, y, szBuff  , dwColor ); y += nNext;
#endif // __VER < 8 // __S8_PK

//	p2DRender->TextOut( 90, y, g_pPlayer->GetFameName()       , dwColor); y += nNext;
//	p2DRender->TextOut( 90, y, g_pPlayer->GetSlaughterName()  , dwColor ); y += nNext;
	y = 58;
	p2DRender->TextOut( 100, y, g_pPlayer->m_nFame	, dwColor ); y += nNext;
#if __VER < 8 // __S8_PK
	p2DRender->TextOut( 100, y, g_pPlayer->m_nSlaughter	, dwColor ); y += nNext;
	p2DRender->TextOut( 100, y, g_pPlayer->m_nNumKill	, dwColor ); y += nNext;
#endif // __VER < 8 // __S8_PK
	
} 
void CWndPvp::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 


	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CPoint point( rectRoot.left, rectRoot.top + 96 + 16 );
	Move( point );
	
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndPvp::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_PVP, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndPvp::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndPvp::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndPvp::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndPvp::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndPvp::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// PVP정보
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
CWndPvpBase::CWndPvpBase()
{
	m_bExpert = FALSE;
	m_pWndChangeJob = NULL;
	m_fWaitingConfirm	= FALSE;
	m_nDisplay = 1;

}
CWndPvpBase::~CWndPvpBase()
{
	SAFE_DELETE(m_pWndChangeJob);
}
void CWndPvpBase::OnDraw(C2DRender* p2DRender)
{
	if( g_pPlayer->IsBaseJob() )
	{
		if( g_pPlayer->GetLevel() >= MAX_JOB_LEVEL )
			m_wndChangeJob.EnableWindow( TRUE );
		else
			m_wndChangeJob.EnableWindow( FALSE );
	}
	else if( g_pPlayer->IsExpert() )
	{
		if( g_pPlayer->GetLevel() >= MAX_JOB_LEVEL + MAX_EXP_LEVEL )
			m_wndChangeJob.EnableWindow( TRUE );
		else
			m_wndChangeJob.EnableWindow( FALSE );
	}

	CRect rect = GetClientRect();
	rect.bottom -= 30;
	int y = 0, nNext = 15;
	DWORD dwColor = D3DCOLOR_ARGB(255,0,0,0);
	char szBuff[32];
	y = 22;

	int gap1 = 0;
	int gap2 = 0;
	gap1 -= 10;
	gap2 -= 10;
	y = 15;
	
	strcpy( szBuff, g_pPlayer->GetFameName() );
	if( IsEmpty(szBuff) ) {	szBuff[0] = '-'; szBuff[1] = NULL; }
	p2DRender->TextOut( 100+gap1, y, szBuff  , dwColor ); y += nNext;
	y = 33;
	p2DRender->TextOut( 100+gap2, y, g_pPlayer->m_nFame	, dwColor ); y += nNext;
#if __VER >= 8 // __S8_PK
	y += 20;
	p2DRender->TextOut( 100+gap2, y, g_pPlayer->GetPKValue()	, dwColor ); y += nNext;
	y += 4;
	p2DRender->TextOut( 100+gap2, y, g_pPlayer->GetPKPropensity()	, dwColor ); y += nNext;
#else // __VER >= 8 // __S8_PK
	y += 4;
	p2DRender->TextOut( 100+gap2, y, g_pPlayer->m_nNumKill	, dwColor ); y += nNext;
	y += 4;
	p2DRender->TextOut( 100+gap2, y, g_pPlayer->GetSlaughterName(), dwColor ); y += nNext;
	y += 4;
	p2DRender->TextOut( 100+gap2, y, g_pPlayer->m_nSlaughter	, dwColor ); y += nNext;
#endif // __VER >= 8 // __S8_PK
	
	y = 13;
	nNext = 19;
	dwColor = D3DCOLOR_ARGB(255, 0, 0, 180);
#if __VER >= 8 // __S8_PK
	p2DRender->TextOut( 15, y, prj.GetText(TID_GAME_CHARACTTER_PVP1), dwColor ); y += nNext;
	p2DRender->TextOut( 15, y, prj.GetText(TID_GAME_CHARACTTER_PVP2), dwColor ); y += nNext;
	y += 20;
	p2DRender->TextOut( 15, y, prj.GetText(TID_GAME_CHARACTTER_PVP3), dwColor ); y += nNext;
	p2DRender->TextOut( 15, y, prj.GetText(TID_GAME_CHARACTTER_PVP4), dwColor ); y += nNext;
#else // __VER >= 8 // __S8_PK
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_08), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_09), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_10), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_11), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_12), dwColor ); y += nNext;
#endif // __VER >= 8 // __S8_PK
	
}

void CWndPvpBase::OnInitialUpdate()
{
	CWndBase::OnInitialUpdate();
	
	int y = 105;
	if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
		m_wndChangeJob.Create( ">", 0, CRect( 135, y, 135+40, y + 13 ), this, 10  ); 

#if __VER >= 9 // __CSC_VER9_2
	SetTexture( m_pApp->m_pd3dDevice, MakePath( "Theme\\", ::GetLanguage(), _T( "WndPvP2.tga" ) ), TRUE );
#else //__CSC_VER9_2
	SetTexture( m_pApp->m_pd3dDevice, MakePath( "Theme\\", ::GetLanguage(), _T( "wndPvP.tga" ) ), TRUE );
#endif //__CSC_VER9_2
	
	FitTextureSize();
}

BOOL CWndPvpBase::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 240, 0, 240 + 330, 255 - 135 ); 
	//SetTitle( GETTEXT( TID_APP_CHARACTER )  );
	return CWndBase::Create(WBS_THICKFRAME|WBS_MOVE|WBS_SOUND|WBS_CAPTION|WBS_EXTENSION,rect,pWndParent,dwWndId);
}
BOOL CWndPvpBase::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{ 
	if( message == WNM_CLICKED )
	{
		switch( nID )
		{
		case JOB_MERCENARY: 
		case JOB_ACROBAT: 
		case JOB_ASSIST: 
		case JOB_MAGICIAN: 
		case JOB_PUPPETEER:
			{
				if( nID != g_pPlayer->GetJob() )
				{
					//"자신의 직업만 올릴수 있습니다"
					g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0037) ) );
				}
				if( m_fWaitingConfirm == FALSE ) 
				{
					g_DPlay.SendIncJobLevel( nID );
					m_fWaitingConfirm = TRUE;
				}
				break;
			}
		case 10: // 전직 
			// 방랑자의 레벨이 15이상인것을 찾음
			if( g_pPlayer->GetLevel() >= MAX_JOB_LEVEL )
			{
				SAFE_DELETE(m_pWndChangeJob);
				m_pWndChangeJob = new CWndChangeJob( g_pPlayer->GetJob() ); 
				m_pWndChangeJob->Initialize( this, 1106 );
			}
			break;
		}
	}

	return CWndBase::OnChildNotify( message, nID, pLResult );
}
BOOL CWndPvpBase::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndBase::OnCommand(nID,dwMessage,pWndBase);
}
void CWndPvpBase::OnSize(UINT nType, int cx, int cy)
{
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize( 70, 25);

	CRect rect1_1( x - ( size.cx / 2), y, ( x - ( size.cx / 2 ) ) + size.cx, y + size.cy );

	CRect rect2_1( x - size.cx - 10, y, ( x - size.cx - 10 ) + size.cx, y + size.cy );
	CRect rect2_2( x + 10          , y, ( x + 10           ) + size.cx, y + size.cy );

	CRect rect3_1( x - ( size.cx / 2) - size.cx - 10, y, (x - ( size.cx / 2) - size.cx - 10) + size.cx, y + size.cy );
	CRect rect3_2( x - ( size.cx / 2)               , y, (x - ( size.cx / 2)               ) + size.cx, y + size.cy );
	CRect rect3_3( x + ( size.cx / 2) + 10          , y, (x + ( size.cx / 2) + 10          ) + size.cx, y + size.cy );

	CWndBase::OnSize(nType,cx,cy);
}
void CWndPvpBase::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndPvpBase::OnLButtonDown(UINT nFlags, CPoint point)
{
}

