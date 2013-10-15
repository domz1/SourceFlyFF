#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndManager.h"
#include "WndRainbowRace.h"
#include "defineSound.h"

#include "DPClient.h"
extern	CDPClient	g_DPlay;

#if __VER >= 13 // __RAINBOW_RACE

//////////////////////////////////////////////////////////////////////////
// RainbowRace Offer Window
//////////////////////////////////////////////////////////////////////////

CWndRainbowRaceOffer::CWndRainbowRaceOffer()
{
	m_pText = NULL;
	m_nOfferCount = 0;
}

CWndRainbowRaceOffer::~CWndRainbowRaceOffer()
{
}

void CWndRainbowRaceOffer::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	m_pText = (CWndText *)GetDlgItem( WIDC_TEXT1 );
	
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "RainbowRaceOffer.inc" ) ));
	CHAR* szChar = scanner.m_pProg;

	if(m_pText != NULL && checkflag)
	{
		CString tempstr;
		tempstr.Format(prj.GetText(TID_GAME_RAINBOWRACE_OFFERNUM), m_nOfferCount);
		m_pText->m_string.AddString(tempstr, D3DCOLOR_XRGB( 0, 0, 0 ), ESSTY_BOLD);
		m_pText->m_string.AddString("\n");
		m_pText->m_string.AddParsingString(szChar);
		m_pText->ResetString();
	}

	MoveParentCenter();
} 

BOOL CWndRainbowRaceOffer::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_RAINBOWRACE_OFFER, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndRainbowRaceOffer::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if(nID == WIDC_OK)
	{
		g_DPlay.SendRainbowRaceApplication();
		Destroy();
	}
	else if(nID == WIDC_CANCEL)
		Destroy();
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

//////////////////////////////////////////////////////////////////////////
// RainbowRace Information Window
//////////////////////////////////////////////////////////////////////////

CWndRainbowRaceInfo::CWndRainbowRaceInfo()
{
	m_pText = NULL;
}

CWndRainbowRaceInfo::~CWndRainbowRaceInfo()
{
}

void CWndRainbowRaceInfo::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	m_pText = (CWndText *)GetDlgItem( WIDC_TEXT1 );
	
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "RainbowRaceInfo.inc" ) ));
	CHAR* szChar = scanner.m_pProg;

	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString(szChar);
		m_pText->ResetString();	
	}

	MoveParentCenter();
} 

BOOL CWndRainbowRaceInfo::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_RAINBOWRACE_INFO, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndRainbowRaceInfo::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	if( nID == WIDC_OK )
		Destroy();

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

//////////////////////////////////////////////////////////////////////////
// RainbowRace Rule Window
//////////////////////////////////////////////////////////////////////////

CWndRainbowRaceRule::CWndRainbowRaceRule()
{
	m_pText = NULL;
}

CWndRainbowRaceRule::~CWndRainbowRaceRule()
{
}

void CWndRainbowRaceRule::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	m_pText = (CWndText *)GetDlgItem( WIDC_TEXT1 );
	
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "RainbowRaceRule.inc" ) ));
	CHAR* szChar = scanner.m_pProg;

	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString(szChar);
		m_pText->ResetString();	
	}

	MoveParentCenter();
} 

BOOL CWndRainbowRaceRule::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_RAINBOWRACE_RULE, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndRainbowRaceRule::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK )
		Destroy();

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

//////////////////////////////////////////////////////////////////////////
// RainbowRace Ranking Window
//////////////////////////////////////////////////////////////////////////

CWndRainbowRaceRanking::CWndRainbowRaceRanking()
{
	m_dwPlayerId[0] = 0;
	m_dwPlayerId[1] = 0;
	m_dwPlayerId[2] = 0;
	m_dwPlayerId[3] = 0;
	m_dwPlayerId[4] = 0;
}

CWndRainbowRaceRanking::~CWndRainbowRaceRanking()
{
}

void CWndRainbowRaceRanking::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	MoveParentCenter();
} 

BOOL CWndRainbowRaceRanking::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_RAINBOWRACE_RANKING, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndRainbowRaceRanking::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK )
		Destroy();

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

void CWndRainbowRaceRanking::OnDraw( C2DRender* p2DRender ) 
{
	TEXTUREVERTEX2* pVertex = new TEXTUREVERTEX2[60];
	TEXTUREVERTEX2* pVertices = pVertex;

	for(int i=0; i<5; i++)
	{
		int nTextGap = i*24;
		// Draw Text Rank.
		CString strTemp;
		strTemp.Format("%d.", i+1);
		m_p2DRender->TextOut( 30, 55 + nTextGap, strTemp, 0xffffffff );//ADEILSON 0xff000000

		PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->GetPlayerData(m_dwPlayerId[i]);
		if(m_dwPlayerId[i] != 0 && pPlayerData && strlen(pPlayerData->szPlayer) > 0)
		{
			int nJob = pPlayerData->data.nJob;
			// Draw Text Name.		
			m_p2DRender->TextOut( 80, 55 + nTextGap, pPlayerData->szPlayer, 0xffffffff );//ADEILSON 0xff000000

			// Draw Job Icon
			CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );

			if(pWndWorld)
			{
				if( prj.m_aJob[ nJob ].dwJobType == JTYPE_PRO )			
					pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( 218, 55 + nTextGap ),  ( 19 + nJob - 6 ), &pVertices, 0xffffffff );
				else if( prj.m_aJob[ nJob ].dwJobType == JTYPE_MASTER )
				{
					int nMasterIndex = 27;
					if(/*m_nLevel >= 60 && */pPlayerData->data.nLevel < 70) //Level Down될 경우를 생각해서 주석처리.
						nMasterIndex = 27;
					else if(pPlayerData->data.nLevel >= 70 && pPlayerData->data.nLevel < 80)
						nMasterIndex = 28;
					else if(pPlayerData->data.nLevel >= 80 && pPlayerData->data.nLevel < 90)
						nMasterIndex = 29;
					else if(pPlayerData->data.nLevel >= 90 && pPlayerData->data.nLevel < 100)
						nMasterIndex = 30;
					else if(pPlayerData->data.nLevel >= 100 && pPlayerData->data.nLevel < 110)
						nMasterIndex = 31;
					else if(pPlayerData->data.nLevel >= 110 && pPlayerData->data.nLevel <= 120)
						nMasterIndex = 32;

					pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( 196, 55 + nTextGap ),  nMasterIndex, &pVertices, 0xffffffff );
					pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( 218, 55 + nTextGap ),  ( 19 + nJob - 16 ), &pVertices, 0xffffffff );
				}
				else if( prj.m_aJob[ nJob ].dwJobType == JTYPE_HERO )
				{
					pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( 196, 55 + nTextGap ),  33, &pVertices, 0xffffffff );
					pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( 218, 55 + nTextGap ),  ( 19 + nJob - 24 ), &pVertices, 0xffffffff );
				}
				else
					pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( 218, 55 + nTextGap ),  14 + nJob, &pVertices, 0xffffffff );

				pWndWorld->m_texPlayerDataIcon.Render( m_pApp->m_pd3dDevice, pVertex, ( (int) pVertices - (int) pVertex ) / sizeof( TEXTUREVERTEX2 ) );
			}
		}
	}

	SAFE_DELETE_ARRAY( pVertex );
}

void CWndRainbowRaceRanking::SetRankingPlayer(u_long* dwPlayerId)
{
	for(int i=0; i<5; i++)
		m_dwPlayerId[i] = dwPlayerId[i];
}

//////////////////////////////////////////////////////////////////////////
// RainbowRace Prize Window
//////////////////////////////////////////////////////////////////////////

CWndRainbowRacePrize::CWndRainbowRacePrize()
{
	m_pText = NULL;
}

CWndRainbowRacePrize::~CWndRainbowRacePrize()
{
}

void CWndRainbowRacePrize::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	m_pText = (CWndText *)GetDlgItem( WIDC_TEXT1 );
	
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "RainbowRacePrize.inc" ) ));
	CHAR* szChar = scanner.m_pProg;

	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString(szChar);
		m_pText->ResetString();	
	}

	MoveParentCenter();
} 

BOOL CWndRainbowRacePrize::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_RAINBOWRACE_PRIZE, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndRainbowRacePrize::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	if( nID == WIDC_OK )
		Destroy();

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

//////////////////////////////////////////////////////////////////////////
// RainbowRace Minigame Window
//////////////////////////////////////////////////////////////////////////

CWndRainbowRaceMiniGame::CWndRainbowRaceMiniGame()
{
	m_pWndMiniGameInfo = NULL;
	m_bReverse = FALSE;
	m_fLerp = 0.0f;
	m_nCurrentGame = -1;
	
	for(int i=0; i<7; i++)
		m_bIsCompletedGame[i] = FALSE;

	m_bGameInfoRefresh = TRUE;
}

CWndRainbowRaceMiniGame::~CWndRainbowRaceMiniGame()
{
	if(m_pWndMiniGameInfo)
		SAFE_DELETE(m_pWndMiniGameInfo);
}

void CWndRainbowRaceMiniGame::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	m_strMiniGameInfo[0] = "RRMinigameKawiBawiBo.inc";
	m_strMiniGameInfo[1] = "RRMinigameDice.inc";
	m_strMiniGameInfo[2] = "RRMinigameArithmetic.inc";
	m_strMiniGameInfo[3] = "RRMinigameStopwatch.inc";
	m_strMiniGameInfo[4] = "RRMinigameTyping.inc";
	m_strMiniGameInfo[5] = "RRMinigameCard.inc";
	m_strMiniGameInfo[6] = "RRMinigameLadder.inc";

	m_strPathCheck[0] = "CheckRed.bmp";
	m_strPathCheck[1] = "CheckOrange.bmp";
	m_strPathCheck[2] = "CheckYellow.bmp";
	m_strPathCheck[3] = "CheckGreen.bmp";
	m_strPathCheck[4] = "CheckBlue.bmp";
	m_strPathCheck[5] = "CheckIndigo.bmp";
	m_strPathCheck[6] = "CheckViolet.bmp";
	
	m_TitleStaticID[0] = WIDC_STATIC1;
	m_TitleStaticID[1] = WIDC_STATIC2;
	m_TitleStaticID[2] = WIDC_STATIC3;
	m_TitleStaticID[3] = WIDC_STATIC4;
	m_TitleStaticID[4] = WIDC_STATIC5;
	m_TitleStaticID[5] = WIDC_STATIC6;
	m_TitleStaticID[6] = WIDC_STATIC7;

	m_CheckStaticID[0] = WIDC_STATIC8;
	m_CheckStaticID[1] = WIDC_STATIC9;
	m_CheckStaticID[2] = WIDC_STATIC10;
	m_CheckStaticID[3] = WIDC_STATIC11;
	m_CheckStaticID[4] = WIDC_STATIC12;
	m_CheckStaticID[5] = WIDC_STATIC13;
	m_CheckStaticID[6] = WIDC_STATIC14;

	m_dwColor[0] = D3DCOLOR_XRGB(255, 0, 0);
	m_dwColor[1] = D3DCOLOR_XRGB(250, 128, 114);
	m_dwColor[2] = D3DCOLOR_XRGB(255, 255, 0);
	m_dwColor[3] = D3DCOLOR_XRGB(0, 128, 0);
	m_dwColor[4] = D3DCOLOR_XRGB(0, 0, 255);
	m_dwColor[5] = D3DCOLOR_XRGB(75, 0, 100);
	m_dwColor[6] = D3DCOLOR_XRGB(128, 0, 128);

	m_nTextID[0] = TID_MMI_LORD_RAINBOW_KAWIBAWIBO;
	m_nTextID[1] = TID_MMI_LORD_RAINBOW_DICE;
	m_nTextID[2] = TID_MMI_LORD_RAINBOW_ARITHMETIC;
	m_nTextID[3] = TID_MMI_LORD_RAINBOW_STOPWATCH;
	m_nTextID[4] = TID_MMI_LORD_RAINBOW_TYPING;
	m_nTextID[5] = TID_MMI_LORD_RAINBOW_CARD;
	m_nTextID[6] = TID_MMI_LORD_RAINBOW_LADDER;
/*
	CWndRainbowRaceMiniGameButton* pWndButton = (CWndRainbowRaceMiniGameButton*)g_WndMng.GetWndBase( APP_RAINBOWRACE_BUTTON );
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point;

	if(pWndButton)
	{
		point.x = rectRoot.right - rectWindow.Width();
		point.y = pWndButton->GetWndRect().bottom + 5;
	}
	else
	{
		point.x = rectRoot.right - rectWindow.Width();
		point.y = 250;
	}

	Move( point );

*/
	CWndRainbowRaceMiniGameButton* pWndButton = (CWndRainbowRaceMiniGameButton*)g_WndMng.GetWndBase( APP_RAINBOWRACE_BUTTON );
	if(pWndButton != NULL)
	{
		CRect rectRoot = pWndButton->m_pWndRoot->GetLayoutRect();
		CRect rectButton = pWndButton->GetWindowRect( TRUE );

		CPoint ptMove;
		CPoint ptGame;

		CRect rect = GetWindowRect( TRUE );
		int wndWidth = rect.Width();
		int wndHeight = rect.Height();

		if(rectRoot.right - rectButton.right < wndWidth)
		{
			ptGame = rectButton.BottomRight();
			ptMove = ptGame - CPoint(wndWidth, 0);
		}
		else
		{
			ptGame = rectButton.TopLeft();
			ptMove = ptGame + CPoint(0, rectButton.Height());
		}

		if(rectRoot.bottom - rectButton.bottom < wndHeight)
			ptMove = ptMove - CPoint(0, rectButton.Height() + wndHeight + 10);

		Move( ptMove );
	}
} 

BOOL CWndRainbowRaceMiniGame::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_RAINBOWRACE_MINIGAME, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndRainbowRaceMiniGame::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	if(nID == WIDC_OK)
		Destroy();
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

void CWndRainbowRaceMiniGame::OnDraw(C2DRender* p2DRender)
{
	for(int i=0; i<7; i++)
	{
		DWORD dwColor1, dwColor2;

		if(i == m_nCurrentGame)
		{
			dwColor1 = m_dwColor[i];
			int nR, nG, nB;
			nR = (0x00ff0000 & m_dwColor[i])  >> 16;
			nG = (0x0000ff00 & m_dwColor[i])  >> 8;
			nB = 0x000000ff & m_dwColor[i];
			dwColor2 = D3DCOLOR_ARGB(0, nR, nG, nB);

			FillRect( p2DRender, GetWndCtrl( m_TitleStaticID[i] )->rect, dwColor1, dwColor2, TRUE );
		}
		else if(m_bIsCompletedGame[i])
		{
			CTexture* pTexture;
			pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, m_strPathCheck[i]), 0xffff00ff );
			if(pTexture != NULL)
				pTexture->Render( p2DRender, CPoint( GetWndCtrl( m_CheckStaticID[i] )->rect.left, GetWndCtrl( m_CheckStaticID[i] )->rect.top  ) );
		}

		if(m_bIsCompletedGame[i])
			p2DRender->TextOut(GetWndCtrl( m_TitleStaticID[i] )->rect.left + 10, GetWndCtrl( m_TitleStaticID[i] )->rect.top + 5, prj.GetText(m_nTextID[i]), 0xff858585);
		else
			p2DRender->TextOut(GetWndCtrl( m_TitleStaticID[i] )->rect.left + 10, GetWndCtrl( m_TitleStaticID[i] )->rect.top + 5, prj.GetText(m_nTextID[i]), 0xff7878c8);
	}
}

void CWndRainbowRaceMiniGame::OnLButtonUp( UINT nFlags, CPoint point ) 
{
	CRect rect;
	LPWNDCTRL lpWndCtrl;

	for(int i=0; i<7; i++) 
	{
		lpWndCtrl = GetWndCtrl( m_TitleStaticID[i] );
		rect = lpWndCtrl->rect;
		if( rect.PtInRect( point ) )
		{
			if(m_pWndMiniGameInfo)
				SAFE_DELETE(m_pWndMiniGameInfo);
			m_pWndMiniGameInfo = new CWndRainbowRaceMiniGameInfo;
			if(m_pWndMiniGameInfo)
			{
				m_pWndMiniGameInfo->SetFileName(m_strMiniGameInfo[i]);
				m_pWndMiniGameInfo->Initialize(NULL);
			}
		}
	}
}

BOOL CWndRainbowRaceMiniGame::OnSetCursor( CWndBase* pWndBase, UINT nHitTest, UINT message )
{
	CRect rect;
	LPWNDCTRL lpWndCtrl;
	BOOL bOnTitle = FALSE;
	CPoint point = GetMousePoint();

	for(int i=0; i<7; i++) 
	{
		lpWndCtrl = GetWndCtrl( m_TitleStaticID[i] );
		rect = lpWndCtrl->rect;
		if( rect.PtInRect( point ) )
			bOnTitle = TRUE;
	}

	if(bOnTitle)
		SetMouseCursor( CUR_SELECT );
	else
	{
		SetMouseCursor( CUR_BASE );
		CWndBase::OnSetCursor( pWndBase, nHitTest, message );
	}

	return TRUE;
}

void CWndRainbowRaceMiniGame::FillRect(C2DRender *p2DRender, CRect rectBg, DWORD dwColorstart, DWORD dwColorend, BOOL bLamp)
{
	CPoint Point = CPoint( rectBg.TopLeft().x, rectBg.TopLeft().y );
	float fVar = 0.04f;

	D3DXCOLOR dwColorDest2 = dwColorstart;
	D3DXCOLOR dwColor = dwColorend;
	D3DXCOLOR dwColor1 = D3DCOLOR_ARGB( 0, 255, 255, 255 );
	D3DXCOLOR dwColor2 = D3DCOLOR_ARGB( 0, 255, 255, 255 );

	float fLerp = 1.0f;

	if(bLamp)
	{
		if( m_bReverse )
			m_fLerp -= fVar;
		else
			m_fLerp += fVar;

		if( m_fLerp > 1.0f )
		{
			m_bReverse = TRUE;
			m_fLerp = 1.0f;
		}
		else if( m_fLerp < 0.0f )
		{
			m_bReverse = FALSE;
			m_fLerp = 0.0f;
		}
		fLerp = m_fLerp;

		D3DXColorLerp( &dwColor2, &dwColor, &dwColorDest2, fLerp );
		int nthick = 8;	
		CRect Rect = CRect( 0, 0, rectBg.right - rectBg.left, rectBg.bottom - rectBg.top );
		
		p2DRender->RenderFillRect( CRect( Point.x+Rect.left, Point.y+Rect.top, Point.x+Rect.left+nthick, Point.y+Rect.bottom ),
			dwColor2, dwColor1, dwColor2, dwColor1 );
		
		p2DRender->RenderFillRect( CRect( Point.x+Rect.left, Point.y+Rect.top+nthick, Point.x+Rect.right, Point.y+Rect.top ),
			dwColor1, dwColor1, dwColor2, dwColor2 );
		
		p2DRender->RenderFillRect( CRect( Point.x+Rect.right, Point.y+Rect.top, Point.x+Rect.right-nthick, Point.y+Rect.bottom ),
			dwColor2, dwColor1, dwColor2, dwColor1 );
		
		p2DRender->RenderFillRect( CRect( Point.x+Rect.left, Point.y+Rect.bottom, Point.x+Rect.right, Point.y+Rect.bottom-nthick ),
			dwColor2, dwColor2, dwColor1, dwColor1 );
	}
	else
	{
		D3DXColorLerp( &dwColor2, &dwColor, &dwColorDest2, fLerp );
		p2DRender->RenderFillRect( rectBg, dwColor1, dwColor2, dwColor1, dwColor2 );
	}
}

BOOL CWndRainbowRaceMiniGame::Process()
{
	if( m_bGameInfoRefresh )
		SetCurrentGameInfo();

	return TRUE;
}

void CWndRainbowRaceMiniGame::SetCurrentGameInfo()
{
	m_nCurrentGame = CRainbowRace::GetInstance()->GetGameKey();

	for(int i=0; i<7; i++)
		m_bIsCompletedGame[i] = CRainbowRace::GetInstance()->IsCompletedGame(i);
}

//////////////////////////////////////////////////////////////////////////
// RainbowRace Minigame Information Window
//////////////////////////////////////////////////////////////////////////

CWndRainbowRaceMiniGameInfo::CWndRainbowRaceMiniGameInfo()
{
}

CWndRainbowRaceMiniGameInfo::~CWndRainbowRaceMiniGameInfo()
{
}

void CWndRainbowRaceMiniGameInfo::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate();
	// 여기에 코딩하세요
	m_pText = (CWndText *)GetDlgItem( WIDC_TEXT1 );
	
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load( MakePath( DIR_CLIENT,  m_strFileName ));
	CHAR* szChar = scanner.m_pProg;

	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString(szChar);
		m_pText->ResetString();	
	}

	CWndRainbowRaceMiniGame* pWndGame = (CWndRainbowRaceMiniGame*)GetWndBase( APP_RAINBOWRACE_MINIGAME );
	if(pWndGame != NULL)
	{
		CRect rectRoot = pWndGame->m_pWndRoot->GetLayoutRect();
		CRect rectGame = pWndGame->GetWindowRect( TRUE );

		CPoint ptMove;
		CPoint ptGame;

		CRect rect = GetWindowRect( TRUE );
		int wndWidth = rect.Width();

		if(rectRoot.right - rectGame.right < wndWidth)
		{
			ptGame = rectGame.TopLeft();
			ptMove = ptGame + CPoint(-(10+wndWidth), 0);
		}
		else
		{
			ptGame = rectGame.BottomRight();
			ptMove = ptGame + CPoint(10, -rectGame.Height());
		}

		Move( ptMove );
	}
} 

BOOL CWndRainbowRaceMiniGameInfo::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_RAINBOWRACE_MINIGAME_INFO, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndRainbowRaceMiniGameInfo::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	if(nID == WIDC_OK)
		Destroy();

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

//////////////////////////////////////////////////////////////////////////
// RainbowRace Minigame Button Window
//////////////////////////////////////////////////////////////////////////

CWndRainbowRaceMiniGameButton::CWndRainbowRaceMiniGameButton()
{
	m_bLoadTexMap = FALSE;
	m_bFocus = FALSE;
	m_bPush = FALSE;
//	m_bMove = FALSE;
	m_ptPush.x = -1;
	m_ptPush.y = -1;
	m_nAlpha = 255;
	m_bReverse = TRUE;
}

CWndRainbowRaceMiniGameButton::~CWndRainbowRaceMiniGameButton()
{
}

HRESULT CWndRainbowRaceMiniGameButton::DeleteDeviceObjects()
{
	m_BtnTexture.DeleteDeviceObjects();
	return CWndNeuz::DeleteDeviceObjects();
}

HRESULT CWndRainbowRaceMiniGameButton::RestoreDeviceObjects()
{
	m_BtnTexture.RestoreDeviceObjects(g_Neuz.m_pd3dDevice);
	return CWndNeuz::RestoreDeviceObjects();
}

HRESULT CWndRainbowRaceMiniGameButton::InvalidateDeviceObjects()
{
	m_BtnTexture.InvalidateDeviceObjects();
	return CWndNeuz::InvalidateDeviceObjects();
}

void CWndRainbowRaceMiniGameButton::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate();
	// 여기에 코딩하세요
	this->DelWndStyle(WBS_CAPTION);
	m_wndTitleBar.SetVisible( FALSE );

	CWndNavigator* pWndNavigator = (CWndNavigator*)g_WndMng.GetWndBase( APP_NAVIGATOR );
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point;

	if(pWndNavigator)
	{
		point.x = rectRoot.right - rectWindow.Width() - 5;
		point.y = pWndNavigator->GetWndRect().bottom + 5;
	}
	else
	{
		point.x = rectRoot.right - rectWindow.Width() - 5;
		point.y = rectRoot.top + rectWindow.Height() + 120;
	}

	Move( point );

	m_bLoadTexMap = m_BtnTexture.LoadScript( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, _T( "texMapRainbowButton.inc" ) ) );
} 

void CWndRainbowRaceMiniGameButton::PaintFrame( C2DRender* p2DRender )
{
}

void CWndRainbowRaceMiniGameButton::OnLButtonDown( UINT nFlags, CPoint point )
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

void CWndRainbowRaceMiniGameButton::OnLButtonUp( UINT nFlags, CPoint point )
{
	CPoint ptScreen = point;
	ClientToScreen( &ptScreen );
	if(m_ptPush == ptScreen)
	{
		CRect rectWindow = GetWindowRect();
		if(rectWindow.PtInRect(point))
		{
			if(g_WndMng.m_pWndRainbowRaceMiniGame)
			{
				SAFE_DELETE(g_WndMng.m_pWndRainbowRaceMiniGame);
			}
			else
			{
				g_WndMng.m_pWndRainbowRaceMiniGame = new CWndRainbowRaceMiniGame;
				if(g_WndMng.m_pWndRainbowRaceMiniGame)
					g_WndMng.m_pWndRainbowRaceMiniGame->Initialize(NULL);
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
/*
void CWndRainbowRaceMiniGameButton::OnMouseWndSurface( CPoint point )
{
	CRect rectWindow = GetWindowRect();
	if(rectWindow.PtInRect(point))
		m_bFocus = TRUE;
	else
		m_bFocus = FALSE;
}

void CWndRainbowRaceMiniGameButton::OnMouseMove(UINT nFlags, CPoint point)
{
	m_bMove = TRUE;
}
*/
BOOL CWndRainbowRaceMiniGameButton::Process()
{
	CPoint ptMouse = GetMousePoint();
//	ClientToScreen( &ptMouse );

	CRect rectWindow = GetWindowRect();
	if(rectWindow.PtInRect(ptMouse))
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

BOOL CWndRainbowRaceMiniGameButton::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_RAINBOWRACE_BUTTON, WBS_NOFOCUS, CPoint( 0, 0 ), pWndParent );
}

void CWndRainbowRaceMiniGameButton::OnDraw(C2DRender* p2DRender)
{
	if(m_bLoadTexMap)
	{
		int nTexNum = -1;
		BOOL bNormal = FALSE;
		CTexture* pTexture = NULL;
		if(!m_bFocus && !m_bPush)
		{
			nTexNum = 0;
			bNormal = TRUE;
		}
		else if(m_bFocus && !m_bPush)
			nTexNum = 0;
		else if(m_bFocus && m_bPush)
			nTexNum = 2;
		
		if(nTexNum != -1)
			pTexture = m_BtnTexture.GetAt(nTexNum);

		if(pTexture)
		{
			if(bNormal)
				pTexture->Render( &g_Neuz.m_2DRender, CPoint(0, 0), m_nAlpha );
			else if(nTexNum == 0 || nTexNum == 2)
				pTexture->Render( &g_Neuz.m_2DRender, CPoint(0, 0) );
		}
	}
}
/*
BOOL CWndRainbowRaceMiniGameButton::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	if(nID == WIDC_BUTTON1)
	{
		if(g_WndMng.m_pWndRainbowRaceMiniGame)
		{
			SAFE_DELETE(g_WndMng.m_pWndRainbowRaceMiniGame);
		}
		else
		{
			g_WndMng.m_pWndRainbowRaceMiniGame = new CWndRainbowRaceMiniGame;
			if(g_WndMng.m_pWndRainbowRaceMiniGame)
				g_WndMng.m_pWndRainbowRaceMiniGame->Initialize(NULL);
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
*/
//////////////////////////////////////////////////////////////////////////
// RainbowRace Minigame End Window
//////////////////////////////////////////////////////////////////////////
void CWndRainbowRaceMiniGameEnd::OnDestroy()
{
	if(g_WndMng.m_pWndRainbowRaceMiniGame)
		g_WndMng.m_pWndRainbowRaceMiniGame->m_bGameInfoRefresh = TRUE;

	CRainbowRace::GetInstance()->m_bCheckEnd = TRUE;
}

BOOL CWndRainbowRaceMiniGameEnd::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CString strTemp;
	int nTextId;

	switch( m_nGameID )
	{
		case RMG_GAWIBAWIBO:	nTextId = TID_MMI_LORD_RAINBOW_KAWIBAWIBO;	break;
		case RMG_DICEPLAY:		nTextId = TID_MMI_LORD_RAINBOW_DICE;		break;
		case RMG_ARITHMATIC:	nTextId = TID_MMI_LORD_RAINBOW_ARITHMETIC;	break;
		case RMG_STOPWATCH:		nTextId = TID_MMI_LORD_RAINBOW_STOPWATCH;	break;
		case RMG_TYPING:		nTextId = TID_MMI_LORD_RAINBOW_TYPING;		break;
		case RMG_PAIRGAME:		nTextId = TID_MMI_LORD_RAINBOW_CARD;		break;
		case RMG_LADDER:		nTextId = TID_MMI_LORD_RAINBOW_LADDER;		break;
	}
	strTemp.Format(prj.GetText(TID_GAME_RAINBOWRACE_END), prj.GetText(nTextId));

	return CWndMessageBox::Initialize( strTemp, pWndParent, MB_OK );
}

BOOL CWndRainbowRaceMiniGameEnd::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
		case IDOK:
			{
				CWndRainbowRaceMiniGame* pWndRainbowRaceMiniGameInfo = (CWndRainbowRaceMiniGame*)g_WndMng.GetWndBase( APP_RAINBOWRACE_MINIGAME );

				if( pWndRainbowRaceMiniGameInfo )
					pWndRainbowRaceMiniGameInfo->SetCurrentGameInfo();

				Destroy();
			}
			break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

//////////////////////////////////////////////////////////////////////////
// RainbowRace Minigame - KawiBawiBo Window
//////////////////////////////////////////////////////////////////////////

CWndRRMiniGameKawiBawiBo::CWndRRMiniGameKawiBawiBo() 
{
	m_pText = NULL;
	m_nWinningCount = 0;
	m_nCount = 0;
	m_nDelay = 1;
	m_nMyChoice = KAWI;
	m_nComChoice = KAWI;
	m_strChoice[0].Format("        %s", prj.GetText( TID_GAME_KAWIBAWIBO_KAWI ));
	m_strChoice[1].Format("        %s", prj.GetText( TID_GAME_KAWIBAWIBO_BAWI ));
	m_strChoice[2].Format("        %s", prj.GetText( TID_GAME_KAWIBAWIBO_BO ));
	m_nStatus = 0;
	m_nResult = -1;
	m_dwRItemId = -1;
	m_dwRNextItemId = -1;
	m_nItemCount = -1;
	m_nNextItemCount = -1;
}
 
CWndRRMiniGameKawiBawiBo::~CWndRRMiniGameKawiBawiBo() 
{ 
} 

int CWndRRMiniGameKawiBawiBo::GetKawiBawiBoComResult(int nMyChoice, int nResult)
{
	int nRtn = 0;
	
	if(nResult == MP_TRUE || nResult == MP_FINISH)
	{
		switch(nMyChoice)
		{
			case KAWI:
				nRtn = BO;
				break;
			case BAWI:
				nRtn = KAWI;
				break;
			case BO:
				nRtn = BAWI;
				break;
		}
	}
	else if(nResult == MP_FAIL)
	{
		switch(nMyChoice)
		{
			case KAWI:
				nRtn = BAWI;
				break;
			case BAWI:
				nRtn = BO;
				break;
			case BO:
				nRtn = KAWI;
				break;
		}
	}
	else if(nResult == MP_FALSE)
		nRtn = nMyChoice;

	return nRtn;
}

BOOL CWndRRMiniGameKawiBawiBo::Process()
{
	int nCom = -1;
	
	if(m_nStatus == 1) //Start버튼 누를 경우 컴퓨터의 선택이 회전하도록 함.
	{
		if(m_nCount > m_nDelay)
		{
			if(m_nResult > -1)
			{
				if(m_nDelay < 20)
				{
					PLAYSND( "InfOpen.wav" );
					m_nDelay += 1;
				}
				else
				{
					PLAYSND( "InfOpen.wav" );
					nCom = GetKawiBawiBoComResult(m_nMyChoice, m_nResult);
				}
			}
			
			m_nComChoice++;
			( m_nComChoice > 2 ) ? m_nComChoice = 0 : m_nComChoice;
			m_pStComChoice->SetTitle( m_strChoice[m_nComChoice] );
			
			if(m_nComChoice == nCom)
			{
				m_nStatus = 2;
				m_nDelay = 40;
			}
			
			m_nCount = 0;
		}
		m_nCount++;
	}
	else if(m_nStatus == 2) //Com의 결과가 보여지고 바로 사라지거나 하기 때문에 약간의 Delay를 줌
	{
		if(m_nCount > m_nDelay)
		{
			if(m_nResult == MP_TRUE)
			{
				m_nWinningCount++;
				RefreshInfo();
				PlayMusic( BGM_IN_FITUP );
			}
			else if(m_nResult == MP_FINISH)
			{
				m_nWinningCount++;
				RefreshInfo();
				PlayMusic( BGM_IN_FITUP );
				// 성공창 보여주고 닫기
				if(g_WndMng.m_pWndRainbowRaceMiniGameEnd)
					SAFE_DELETE(g_WndMng.m_pWndRainbowRaceMiniGameEnd);

				g_WndMng.m_pWndRainbowRaceMiniGameEnd = new CWndRainbowRaceMiniGameEnd;

				if(g_WndMng.m_pWndRainbowRaceMiniGameEnd)
				{
					g_WndMng.m_pWndRainbowRaceMiniGameEnd->SetGameID(RMG_GAWIBAWIBO);
					g_WndMng.m_pWndRainbowRaceMiniGameEnd->Initialize(NULL);
				}

				Destroy();
			}
			else if(m_nResult == MP_FAIL)
			{
				g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_KAWIBAWIBO_DEFEAT ) );
				m_nWinningCount = 0;
				RefreshInfo();
			}			
			EnableAllBtn();
			
			m_nStatus = 0;
			m_nDelay = 1;
			m_nResult = -1;	
		}
		m_nCount++;
	}
	
	return TRUE;
}

void CWndRRMiniGameKawiBawiBo::OnDraw( C2DRender* p2DRender ) 
{
	//Draw Image
	CTexture* pTexture;
	CString strMyPath, strComPath;

	for(int i=0; i<2; i++)
	{
		int nChoice;
		CString* pstr;
		if(i == 0)
		{
			nChoice = m_nMyChoice;
			pstr = &strMyPath;
			switch(nChoice) 
			{
			case KAWI:
				*pstr = MakePath( DIR_ICON, "Icon_Kawi1.dds");
				break;
			case BAWI:
				*pstr = MakePath( DIR_ICON, "Icon_Bawi1.dds");
				break;
			case BO:
				*pstr = MakePath( DIR_ICON, "Icon_Bo1.dds");
				break;
			}
		}
		else if(i == 1)
		{
			nChoice = m_nComChoice;
			pstr = &strComPath;
			switch(nChoice) 
			{
			case KAWI:
				*pstr = MakePath( DIR_ICON, "Icon_Kawi2.dds");
				break;
			case BAWI:
				*pstr = MakePath( DIR_ICON, "Icon_Bawi2.dds");
				break;
			case BO:
				*pstr = MakePath( DIR_ICON, "Icon_Bo2.dds");
				break;
			}
		}
	}
	
	if(strMyPath.GetLength() > 0)
	{
		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, strMyPath, 0xffff00ff );
		if(pTexture != NULL)
			pTexture->Render( p2DRender, CPoint( GetWndCtrl( WIDC_STATIC_MY1 )->rect.left + 3, GetWndCtrl( WIDC_STATIC_MY1 )->rect.top + 8 ) );		
	}

	if(strComPath.GetLength() > 0)
	{
		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, strComPath, 0xffff00ff );
		if(pTexture != NULL)
			pTexture->Render( p2DRender, CPoint( GetWndCtrl( WIDC_STATIC_COM1 )->rect.left + 3, GetWndCtrl( WIDC_STATIC_COM1 )->rect.top + 8 ) );		
	}
}

void CWndRRMiniGameKawiBawiBo::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	m_pText = (CWndText *)GetDlgItem( WIDC_TEXT1 );
	
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "RRMinigameKawiBawiBo.inc" ) ));
	CHAR* szChar = scanner.m_pProg;

	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString(szChar);
		m_pText->ResetString();	
	}

	m_pStWinningCount = (CWndStatic*)GetDlgItem( WIDC_WINNING_COUNT );
	m_pStMyChoice = (CWndStatic*)GetDlgItem( WIDC_MY_CHOICE1 );
	m_pStComChoice = (CWndStatic*)GetDlgItem( WIDC_COM_CHOICE1 );

	RefreshInfo();

	MoveParentCenter();
} 

BOOL CWndRRMiniGameKawiBawiBo::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_RR_MINIGAME_KAWIBAWIBO, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndRRMiniGameKawiBawiBo::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if(message == WNM_CLICKED)
	{
		switch(nID) 
		{
			case WIDC_LBUTTON:
				m_nMyChoice--;
				( m_nMyChoice < 0 ) ? m_nMyChoice = 2 : m_nMyChoice;
				RefreshInfo();
				break;
			case WIDC_RBUTTON:
				m_nMyChoice++;
				( m_nMyChoice > 2 ) ? m_nMyChoice = 0 : m_nMyChoice;
				RefreshInfo();
				break;
			case WIDC_BTN_START:
				DisableAllBtn();				
				m_nStatus = 1;											//컴퓨터의 선택을 화면상에 돌리기 위함.
				CRainbowRace::GetInstance()->SendMinigamePacket();		//DpClient에 정보를 넘겨 가위바위보 결과를 기다림.
				break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndRRMiniGameKawiBawiBo::RefreshInfo()
{
	CString temp;
	temp.Format(prj.GetText(TID_GAME_KWAIBAWIBO_WINCOUNT), m_nWinningCount);
	m_pStWinningCount->SetTitle( temp );
	m_pStMyChoice->SetTitle( m_strChoice[m_nMyChoice] );
	m_pStComChoice->SetTitle( m_strChoice[m_nComChoice] );
}

void CWndRRMiniGameKawiBawiBo::DisableAllBtn()
{
	CWndButton* pButton;
	pButton = (CWndButton*)GetDlgItem( WIDC_LBUTTON );
	pButton->EnableWindow(FALSE);
	pButton = (CWndButton*)GetDlgItem( WIDC_RBUTTON );
	pButton->EnableWindow(FALSE);
	pButton = (CWndButton*)GetDlgItem( WIDC_BTN_START );
	pButton->EnableWindow(FALSE);
}

void CWndRRMiniGameKawiBawiBo::EnableAllBtn()
{
	CWndButton* pButton;
	pButton = (CWndButton*)GetDlgItem( WIDC_LBUTTON );
	pButton->EnableWindow(TRUE);
	pButton = (CWndButton*)GetDlgItem( WIDC_RBUTTON );
	pButton->EnableWindow(TRUE);
	pButton = (CWndButton*)GetDlgItem( WIDC_BTN_START );
	pButton->EnableWindow(TRUE);
}

//////////////////////////////////////////////////////////////////////////
// RainbowRace Minigame - Dice Window
//////////////////////////////////////////////////////////////////////////

CWndRRMiniGameDice::CWndRRMiniGameDice()
{
	m_pText = NULL;
	m_nTargetNum = 0;
	m_nTargetNumBackup = 0;
	m_nDiceNum1 = 0;
	m_nDiceNum2 = 5;
	m_nResultDiceNum1 = 0;
	m_nResultDiceNum2 = 0;
	m_nCount1 = 0;
	m_nCount2 = 0;
	m_nDelay1 = 1;
	m_nDelay2 = 1;
	m_nStatus1 = -1;
	m_nStatus2 = -1;
	m_bFinishStatus1 = FALSE;
}

CWndRRMiniGameDice::~CWndRRMiniGameDice()
{
}

void CWndRRMiniGameDice::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	m_pText = (CWndText *)GetDlgItem( WIDC_TEXT1 );
	
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "RRMinigameDice.inc" ) ));
	CHAR* szChar = scanner.m_pProg;

	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString(szChar);
		m_pText->ResetString();	
	}

	m_strPathDice[0] = MakePath( DIR_ICON, "Icon_1Dice90X90.dds");
	m_strPathDice[1] = MakePath( DIR_ICON, "Icon_2Dice90X90.dds");
	m_strPathDice[2] = MakePath( DIR_ICON, "Icon_3Dice90X90.dds");
	m_strPathDice[3] = MakePath( DIR_ICON, "Icon_4Dice90X90.dds");
	m_strPathDice[4] = MakePath( DIR_ICON, "Icon_5Dice90X90.dds");
	m_strPathDice[5] = MakePath( DIR_ICON, "Icon_6Dice90X90.dds");

	if(!m_texTargetNum.LoadScript( D3DDEVICE, MakePath( DIR_THEME, "RainbowDiceNumber.inc" ) ))
		Destroy();

	MoveParentCenter();
} 

HRESULT CWndRRMiniGameDice::DeleteDeviceObjects()
{
	m_texTargetNum.DeleteDeviceObjects();
	return CWndNeuz::DeleteDeviceObjects();
}
HRESULT CWndRRMiniGameDice::InvalidateDeviceObjects()
{
	m_texTargetNum.InvalidateDeviceObjects();
	return CWndNeuz::InvalidateDeviceObjects();
}
HRESULT CWndRRMiniGameDice::RestoreDeviceObjects()
{
	m_texTargetNum.RestoreDeviceObjects(m_pApp->m_pd3dDevice);
	return CWndNeuz::RestoreDeviceObjects();
}

BOOL CWndRRMiniGameDice::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_RR_MINIGAME_DICE, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndRRMiniGameDice::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	if(nID == WIDC_START)
	{
		CWndButton* pButton;
		pButton = (CWndButton*)GetDlgItem( WIDC_START );
		pButton->EnableWindow(FALSE);
		m_nStatus1 = 0;
		m_bFinishStatus1 = FALSE;

		CRainbowRace::GetInstance()->SendMinigamePacket();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

void CWndRRMiniGameDice::OnDraw(C2DRender* p2DRender )
{
	// Draw Text Target Number
	int nCount[2] = {0, 0};
	nCount[0] = m_nTargetNum / 10;
	nCount[1] = m_nTargetNum % 10;

	CTexture* pTexture = m_texTargetNum.GetAt(nCount[0]);
	if(pTexture)
		pTexture->Render( &g_Neuz.m_2DRender, CPoint(GetWndCtrl( WIDC_STATIC1 )->rect.left + 5, GetWndCtrl( WIDC_STATIC1 )->rect.top) );
	pTexture = m_texTargetNum.GetAt(nCount[1]);
	if(pTexture)
		pTexture->Render( &g_Neuz.m_2DRender, CPoint(GetWndCtrl( WIDC_STATIC1 )->rect.left + 29, GetWndCtrl( WIDC_STATIC1 )->rect.top) );

	// Draw twe Dice
	CString strPath;
	
	strPath = m_strPathDice[m_nDiceNum1];

	if(strPath.GetLength() > 0)
	{
		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, strPath, 0xffff00ff );
		if(pTexture != NULL)
			pTexture->Render( p2DRender, CPoint( GetWndCtrl( WIDC_STATIC2 )->rect.left + 3, GetWndCtrl( WIDC_STATIC2 )->rect.top + 4 ) );	
	}

	strPath = m_strPathDice[m_nDiceNum2];

	if(strPath.GetLength() > 0)
	{
		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, strPath, 0xffff00ff );
		if(pTexture != NULL)
			pTexture->Render( p2DRender, CPoint( GetWndCtrl( WIDC_STATIC3 )->rect.left + 3, GetWndCtrl( WIDC_STATIC3 )->rect.top + 4 ) );	
	}
}

BOOL CWndRRMiniGameDice::Process()
{
	//Start버튼 누를 경우 컴퓨터의 선택이 회전하도록 함.
	if(m_nStatus1 == 0)
	{
		if(m_nCount1%4 == 0)
		{
			if(m_nStatus2 == -1 && m_nCount1 > 30)
				m_nStatus2 = 0;

			if(m_nCount1 > 70)
			{
				m_nCount1 = 0;
				m_nStatus1 = 1;
			}
			else
			{
				PLAYSND( "InfOpen.wav" );
				int randnum = rand() % 6;
				if(randnum == m_nDiceNum1)
				{
					randnum++;
					( randnum > 5 ) ? randnum = 0 : randnum;
					
				}
				m_nDiceNum1 = randnum;
			}
		}
		m_nCount1++;
	}
	else if(m_nStatus1 == 1)
	{
		if(m_nCount1 > m_nDelay1)
		{
			PLAYSND( "InfOpen.wav" );

			if(m_nDelay1 < 22)
			{
				m_nDelay1 += 1;
				int randnum = rand() % 6;
				if(randnum == m_nDiceNum1)
				{
					randnum++;
					( randnum > 5 ) ? randnum = 0 : randnum;
					
				}
				m_nDiceNum1 = randnum;
				
				m_nCount1 = 0;
			}
			else
			{
				m_nDiceNum1 = m_nResultDiceNum1;
				m_bFinishStatus1 = TRUE;
				m_nStatus1 = -1;
			}
		}
		m_nCount1++;
	}

	if(m_nStatus2 == 0)
	{
		if(m_nCount2%4 == 0)
		{
			if(m_nCount2 > 100)
			{
				m_nCount2 = 0;
				m_nStatus2 = 1;
			}
			else
			{
				if(m_bFinishStatus1)
					PLAYSND( "InfOpen.wav" );

				int randnum = rand() % 6;
				if(randnum == m_nDiceNum2)
				{
					randnum++;
					( randnum > 5 ) ? randnum = 0 : randnum;
					
				}
				m_nDiceNum2 = randnum;			
			}
		}
		m_nCount2++;
	}
	else if(m_nStatus2 == 1)
	{
		if(m_nCount2 > m_nDelay2)
		{
			if(m_bFinishStatus1)
				PLAYSND( "InfOpen.wav" );

			if(m_nDelay2 < 24)
			{
				m_nDelay2 += 1;
			
				int randnum = rand() % 6;
				if(randnum == m_nDiceNum2)
				{
					randnum++;
					( randnum > 5 ) ? randnum = 0 : randnum;
					
				}
				m_nDiceNum2 = randnum;
			}
			else
			{
				m_nDiceNum2 = m_nResultDiceNum2;
				m_nStatus2 = 2;
				m_nDelay2 = 40;
			}			
			m_nCount2 = 0;
		}
		m_nCount2++;
	}
	else if(m_nStatus2 == 2) //Com의 결과가 보여지고 바로 사라지거나 하기 때문에 약간의 Delay를 줌
	{
		if(m_nCount2 > m_nDelay2)
		{
			if(m_bFinishStatus1 && m_nResult == MP_FINISH)
			{
				PlayMusic( BGM_IN_FITUP );
				// 성공창 보여주고 닫기
				if(g_WndMng.m_pWndRainbowRaceMiniGameEnd)
					SAFE_DELETE(g_WndMng.m_pWndRainbowRaceMiniGameEnd);

				g_WndMng.m_pWndRainbowRaceMiniGameEnd = new CWndRainbowRaceMiniGameEnd;

				if(g_WndMng.m_pWndRainbowRaceMiniGameEnd)
				{
					g_WndMng.m_pWndRainbowRaceMiniGameEnd->SetGameID(RMG_DICEPLAY);
					g_WndMng.m_pWndRainbowRaceMiniGameEnd->Initialize(NULL);
				}

				Destroy();
			}
			else
			{
				CWndButton* pButton;
				pButton = (CWndButton*)GetDlgItem( WIDC_START );
				pButton->EnableWindow(TRUE);

				m_nCount1 = 0;
				m_nCount2 = 0;
				m_nDelay1 = 1;
				m_nDelay2 = 1;
				m_nStatus1 = -1;
				m_nStatus2 = -1;
				m_bFinishStatus1 = FALSE;

				SetTargetNumber(m_nTargetNumBackup);
			}
		}
		m_nCount2++;
	}

	return TRUE;
}

void CWndRRMiniGameDice::ReceiveResult(int nResult, int nDice1, int nDice2)
{
	if(nResult == MP_FAIL && nDice2 == -1)
	{
		m_nTargetNumBackup = nDice1;
	}
	else
	{
		m_nResult = nResult;
		m_nResultDiceNum1 = nDice1 - 1;
		m_nResultDiceNum2 = nDice2 - 1;
	}
}

//////////////////////////////////////////////////////////////////////////
// RainbowRace Minigame - Arithmetic Window
//////////////////////////////////////////////////////////////////////////

CWndRRMiniGameArithmetic::CWndRRMiniGameArithmetic()
{
	m_pText = NULL;
	m_pWndRRMiniGameArithmeticTimeOver = NULL;
	m_dwTime = 0;
	m_nCorrectCount = 0;
}

CWndRRMiniGameArithmetic::~CWndRRMiniGameArithmetic()
{
	if(m_pWndRRMiniGameArithmeticTimeOver)
		SAFE_DELETE(m_pWndRRMiniGameArithmeticTimeOver);
}

void CWndRRMiniGameArithmetic::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate();
	// 여기에 코딩하세요
	m_pText = (CWndText *)GetDlgItem( WIDC_TEXT2 );
	
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "RRMinigameArithmetic.inc" ) ));
	CHAR* szChar = scanner.m_pProg;

	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString(szChar);
		m_pText->ResetString();	
	}

	MoveParentCenter();
} 

BOOL CWndRRMiniGameArithmetic::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_RR_MINIGAME_ARITHMETIC, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndRRMiniGameArithmetic::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	if(nID == WIDC_BUTTON1)
	{
		CWndEdit* pEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
		CString strValue = pEdit->GetString();
		int nValue = atoi(strValue);
		CRainbowRace::GetInstance()->SendMinigamePacket( RMG_MAX, MP_NONE, nValue );
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

BOOL CWndRRMiniGameArithmetic::Process()
{
	// 1문제당 10초씩 타이머
/*	if( m_dwTime > 0 && timeGetTime() - m_dwTime > SEC(10) )
	{
		CWndButton* pButton;
		pButton = (CWndButton*)GetDlgItem( WIDC_BUTTON1 );
		pButton->EnableWindow(FALSE);

		if(m_pWndRRMiniGameArithmeticTimeOver)
			SAFE_DELETE(m_pWndRRMiniGameArithmeticTimeOver);

		m_pWndRRMiniGameArithmeticTimeOver = new CWndRRMiniGameArithmeticTimeOver;
		if(m_pWndRRMiniGameArithmeticTimeOver)
			m_pWndRRMiniGameArithmeticTimeOver->Initialize();

		m_dwTime = 0;
	}
*/
	return TRUE;
}

void CWndRRMiniGameArithmetic::SetNextQuestion(CString strQuestion, int nCorrectCount)
{
	m_strQuestion = strQuestion;

	CWndText* pText = (CWndText *)GetDlgItem( WIDC_TEXT1 );

	if(pText != NULL)
		pText->SetString(m_strQuestion);

	CWndButton* pButton = (CWndButton*)GetDlgItem( WIDC_BUTTON1 );
	if(pButton)
		pButton->EnableWindow(TRUE);

	CWndEdit* pEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	if(pEdit)
		pEdit->SetString("");

	m_dwTime = timeGetTime();
	m_nCorrectCount = nCorrectCount;

	CWndStatic* pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC3 );
	if(pStatic)
	{
		CString strTemp;
		strTemp.Format("%d", m_nCorrectCount);
		pStatic->SetTitle(strTemp);
	}
}

//////////////////////////////////////////////////////////////////////////
// MiniGameArithmeticTimeOver Message
//////////////////////////////////////////////////////////////////////////

void CWndRRMiniGameArithmeticTimeOver::OnDestroy()
{
	CRainbowRace::GetInstance()->SendMinigamePacket( RMG_MAX, MP_FAIL, 0 );
}

BOOL CWndRRMiniGameArithmeticTimeOver::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	return CWndMessageBox::Initialize( prj.GetText(TID_GAME_REMOVE_ARITHMETIC_TIMEOVER), pWndParent, MB_OK );
}

BOOL CWndRRMiniGameArithmeticTimeOver::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
		case IDOK:
			Destroy();
			break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

//////////////////////////////////////////////////////////////////////////
// RainbowRace Minigame - StopWatch Window
//////////////////////////////////////////////////////////////////////////

CWndRRMiniGameStopWatch::CWndRRMiniGameStopWatch()
{
	m_pText = NULL;
	m_dwTargetTime = 0;
	m_dwStartTime = 0;
	m_dwTime = 0;
	m_bStart = FALSE;
	m_timeSec = 0;
	m_nUSec = 0;
}

CWndRRMiniGameStopWatch::~CWndRRMiniGameStopWatch()
{
}

void CWndRRMiniGameStopWatch::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	m_pText = (CWndText *)GetDlgItem( WIDC_TEXT1 );
	
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "RRMinigameStopwatch.inc" ) ));
	CHAR* szChar = scanner.m_pProg;

	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString(szChar);
		m_pText->ResetString();	
	}

	if(!m_texStopWatchNum.LoadScript( D3DDEVICE, MakePath( DIR_THEME, "RainbowStopWatchNumber.inc" ) ))
		Destroy();

	MoveParentCenter();
} 

HRESULT CWndRRMiniGameStopWatch::DeleteDeviceObjects()
{
	m_texStopWatchNum.DeleteDeviceObjects();
	return CWndNeuz::DeleteDeviceObjects();
}
HRESULT CWndRRMiniGameStopWatch::InvalidateDeviceObjects()
{
	m_texStopWatchNum.InvalidateDeviceObjects();
	return CWndNeuz::InvalidateDeviceObjects();
}
HRESULT CWndRRMiniGameStopWatch::RestoreDeviceObjects()
{
	m_texStopWatchNum.RestoreDeviceObjects(m_pApp->m_pd3dDevice);
	return CWndNeuz::RestoreDeviceObjects();
}

BOOL CWndRRMiniGameStopWatch::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_RR_MINIGAME_STOPWATCH, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndRRMiniGameStopWatch::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	if(nID == WIDC_START && message == WNM_CLICKED)
	{
		if(!m_bStart)
		{
			m_dwStartTime = GetTickCount();

			CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_START);
			if(pWndButton)
				pWndButton->SetTexture( m_pParentWnd->m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "Buttstop.bmp" ) ), TRUE );

			m_bStart = TRUE;
		}
		else
		{
			CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_START);
			if(pWndButton)
				pWndButton->SetTexture( m_pParentWnd->m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtStart.bmp" ) ), TRUE );

			m_bStart = FALSE;
			m_dwTime = GetTickCount() - m_dwStartTime;
			m_timeSec = m_dwTime / 1000;
			m_nUSec = m_dwTime - (m_dwTime / 1000) * 1000;
			DWORD dwTime = m_timeSec*1000 + m_nUSec;

			CRainbowRace::GetInstance()->SendMinigamePacket( RMG_MAX, MP_NONE, dwTime );
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

void CWndRRMiniGameStopWatch::OnDraw(C2DRender* p2DRender)
{
	int nSec = m_timeSec - (m_timeSec/60) * 60;
	int nUSec = m_nUSec/10;
	int nSec1 = nSec / 10;
	int nSec2 = nSec % 10;
	int nUSec1 = nUSec / 10;
	int nUSec2 = nUSec % 10;

	CTexture* pTexture = m_texStopWatchNum.GetAt(nSec1);
	if(pTexture)
		pTexture->Render( &g_Neuz.m_2DRender, CPoint(GetWndCtrl( WIDC_STATIC2 )->rect.left + 40, GetWndCtrl( WIDC_STATIC2 )->rect.top) );
	pTexture = m_texStopWatchNum.GetAt(nSec2);
	if(pTexture)
		pTexture->Render( &g_Neuz.m_2DRender, CPoint(GetWndCtrl( WIDC_STATIC2 )->rect.left + 70, GetWndCtrl( WIDC_STATIC2 )->rect.top) );
	pTexture = m_texStopWatchNum.GetAt(nUSec1);
	if(pTexture)
		pTexture->Render( &g_Neuz.m_2DRender, CPoint(GetWndCtrl( WIDC_STATIC2 )->rect.left + 135, GetWndCtrl( WIDC_STATIC2 )->rect.top) );
	pTexture = m_texStopWatchNum.GetAt(nUSec2);
	if(pTexture)
		pTexture->Render( &g_Neuz.m_2DRender, CPoint(GetWndCtrl( WIDC_STATIC2 )->rect.left + 165, GetWndCtrl( WIDC_STATIC2 )->rect.top) );
}

BOOL CWndRRMiniGameStopWatch::Process()
{
	if(m_bStart)
	{
		m_dwTime = GetTickCount() - m_dwStartTime;
		m_timeSec = m_dwTime / 1000;
		m_nUSec = (int)(m_dwTime - (m_dwTime / 1000) * 1000);
	}
	else
		m_dwTime = 0;

	if(m_dwTime > SEC(60))
	{
		//m_dwStartTime = GetTickCount();
		CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_START);
		if(pWndButton)
			pWndButton->SetTexture( m_pParentWnd->m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtStart.bmp" ) ), TRUE );

		m_bStart = FALSE;
		m_dwTime = SEC(60);
		m_timeSec = m_dwTime / 1000;
		m_nUSec = m_dwTime - (m_dwTime / 1000) * 1000;
		DWORD dwTime = m_timeSec*1000 + m_nUSec;
		CRainbowRace::GetInstance()->SendMinigamePacket( RMG_MAX, MP_NONE, dwTime );
	}
	
	return TRUE;
}

void CWndRRMiniGameStopWatch::SetTargetTime(DWORD dwTargetTime)
{
	m_dwTargetTime = dwTargetTime;

	CString strText, strSec, strUSec;
	time_t time = m_dwTargetTime / 1000;
	int nSec = time - (time/60) * 60;
	int nUSec = (m_dwTargetTime - (m_dwTargetTime / 1000) * 1000) / 10;

	if(nSec/10 > 0)
		strSec.Format("%d", nSec);
	else
		strSec.Format("0%d", nSec);

	if(nUSec/10 > 0)
		strUSec.Format("%d", nUSec);
	else
		strUSec.Format("0%d", nUSec);


	strText.Format("%s : %s", strSec, strUSec);

	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC3);
	if(pWndStatic)
		pWndStatic->SetTitle(strText);
}

//////////////////////////////////////////////////////////////////////////
// RainbowRace Minigame - Typing Window
//////////////////////////////////////////////////////////////////////////

CWndRRMiniGameTyping::CWndRRMiniGameTyping()
{
	m_pText = NULL;
}

CWndRRMiniGameTyping::~CWndRRMiniGameTyping()
{
}

void CWndRRMiniGameTyping::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	m_pText = (CWndText *)GetDlgItem( WIDC_TEXT4 );
	
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "RRMinigameTyping.inc" ) ));
	CHAR* szChar = scanner.m_pProg;

	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString(szChar);
		m_pText->ResetString();	
	}

	m_QuestionStaticID[0] = WIDC_STATIC1;
	m_QuestionStaticID[1] = WIDC_STATIC2;
	m_QuestionStaticID[2] = WIDC_STATIC3;

	m_TypingStaticID[0] = WIDC_EDIT1;
	m_TypingStaticID[1] = WIDC_EDIT2;
	m_TypingStaticID[2] = WIDC_EDIT3;

	MoveParentCenter();
} 

BOOL CWndRRMiniGameTyping::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_RR_MINIGAME_TYPING, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndRRMiniGameTyping::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	if(nID == WIDC_OK)
	{
		vector<string> vecStrValue;

		for(int i=0; i<3; i++)
		{
			CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(m_TypingStaticID[i]);

			if(pWndEdit)
				vecStrValue.push_back(pWndEdit->GetString());
		}
		
		CRainbowRace::GetInstance()->SendMinigameExtPacket( vecStrValue, RMG_MAX, MP_NONE, 0, 0 );
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

void CWndRRMiniGameTyping::SetQuestion(int nQuestion, CString strQuestion)
{
	if(nQuestion >= 0 && nQuestion < 3)
	{
		CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(m_QuestionStaticID[nQuestion]);

		if(pWndStatic)
			pWndStatic->SetTitle(strQuestion);
	}
}

void CWndRRMiniGameTyping::ResetQuestion()
{
	for(int i=0; i<3; i++)
	{
		CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(m_TypingStaticID[i]);

		if(pWndEdit)
			pWndEdit->SetString("");
	}
}

//////////////////////////////////////////////////////////////////////////
// RainbowRace Minigame - Card Window
//////////////////////////////////////////////////////////////////////////

CWndRRMiniGameCard::CWndRRMiniGameCard()
{
	m_pText = NULL;
	m_texCardBack = NULL;
	m_nPair[0] = -1;
	m_nPair[1] = -1;
	m_dwPenaltyTime = 0;
	m_nPenaltySec = -1;
}

CWndRRMiniGameCard::~CWndRRMiniGameCard()
{
}

void CWndRRMiniGameCard::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	m_pText = (CWndText *)GetDlgItem( WIDC_TEXT1 );
	
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "RRMinigameCard.inc" ) ));
	CHAR* szChar = scanner.m_pProg;

	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString(szChar);
		m_pText->ResetString();	
	}

	m_strCard[0] = "RainbowRaceCard1.tga";
	m_strCard[1] = "RainbowRaceCard2.tga";
	m_strCard[2] = "RainbowRaceCard3.tga";
	m_strCard[3] = "RainbowRaceCard4.tga";
	m_strCard[4] = "RainbowRaceCard5.tga";
	m_strCard[5] = "RainbowRaceCard6.tga";
	m_strCard[6] = "RainbowRaceCard7.tga";
	m_strCard[7] = "RainbowRaceCard8.tga";
	m_strCard[8] = "RainbowRaceCard9.tga";

	m_texCardBack = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "RainbowRaceCardBack.tga"), 0xffff00ff );

	m_nCustomID[0] = WIDC_CUSTOM1;
	m_nCustomID[1] = WIDC_CUSTOM2;
	m_nCustomID[2] = WIDC_CUSTOM3;
	m_nCustomID[3] = WIDC_CUSTOM4;
	m_nCustomID[4] = WIDC_CUSTOM5;
	m_nCustomID[5] = WIDC_CUSTOM6;
	m_nCustomID[6] = WIDC_CUSTOM7;
	m_nCustomID[7] = WIDC_CUSTOM8;
	m_nCustomID[8] = WIDC_CUSTOM9;
	m_nCustomID[9] = WIDC_CUSTOM10;
	m_nCustomID[10] = WIDC_CUSTOM11;
	m_nCustomID[11] = WIDC_CUSTOM12;
	m_nCustomID[12] = WIDC_CUSTOM13;
	m_nCustomID[13] = WIDC_CUSTOM14;
	m_nCustomID[14] = WIDC_CUSTOM15;
	m_nCustomID[15] = WIDC_CUSTOM16;
	m_nCustomID[16] = WIDC_CUSTOM17;
	m_nCustomID[17] = WIDC_CUSTOM18;

	if(!m_texPenaltyNum.LoadScript( D3DDEVICE, MakePath( DIR_THEME, "RainbowCardPenaltyNumber.inc" ) ))
		Destroy();

	MoveParentCenter();
} 

HRESULT CWndRRMiniGameCard::DeleteDeviceObjects()
{
	m_texPenaltyNum.DeleteDeviceObjects();
	return CWndNeuz::DeleteDeviceObjects();
}
HRESULT CWndRRMiniGameCard::InvalidateDeviceObjects()
{
	m_texPenaltyNum.InvalidateDeviceObjects();
	return CWndNeuz::InvalidateDeviceObjects();
}
HRESULT CWndRRMiniGameCard::RestoreDeviceObjects()
{
	m_texPenaltyNum.RestoreDeviceObjects(m_pApp->m_pd3dDevice);
	return CWndNeuz::RestoreDeviceObjects();
}

BOOL CWndRRMiniGameCard::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_RR_MINIGAME_CARD, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndRRMiniGameCard::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

void CWndRRMiniGameCard::OnDraw(C2DRender* p2DRender)
{
	for(int i=0; i<18; i++)
	{
		if((m_stCard[i].m_bCheck && m_stCard[i].m_pTexture != NULL) || i == m_nPair[0] || i == m_nPair[1])
		{
			m_stCard[i].m_pTexture->Render(p2DRender, CPoint( GetWndCtrl( m_nCustomID[i] )->rect.left, GetWndCtrl( m_nCustomID[i] )->rect.top ));	
		}
		else if(m_texCardBack != NULL)
			m_texCardBack->Render(p2DRender, CPoint( GetWndCtrl( m_nCustomID[i] )->rect.left, GetWndCtrl( m_nCustomID[i] )->rect.top ));
	}

	if(m_dwPenaltyTime > 0 && m_nPenaltySec >= 0 && m_nPenaltySec < 5)
	{
		CTexture* pTexture = m_texPenaltyNum.GetAt(4 - m_nPenaltySec);
		if(pTexture)
			pTexture->Render( &g_Neuz.m_2DRender, CPoint(180, 50) );
	}
}

BOOL CWndRRMiniGameCard::Process()
{
	if(m_dwPenaltyTime > 0)
	{
		time_t time = (GetTickCount() - m_dwPenaltyTime) / 1000;
		m_nPenaltySec = time - (time/60) * 60;

		if(GetTickCount() - m_dwPenaltyTime > SEC(5))
		{
			m_dwPenaltyTime = 0;
			m_nPenaltySec = -1;
		}
	}

	return TRUE;
}

void CWndRRMiniGameCard::SetQuestion(CString strQuestion)
{
	for(int i=0; i<strQuestion.GetLength(); i++)
	{
		m_stCard[i].m_nCardNum = strQuestion.GetAt(i) - 1;
		
		if(m_stCard[i].m_nCardNum >= 0 && m_stCard[i].m_nCardNum < 9)
			m_stCard[i].m_pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, m_strCard[m_stCard[i].m_nCardNum]), 0xffff00ff );
		
		m_stCard[i].m_bCheck = FALSE;
	}
}

void CWndRRMiniGameCard::OnLButtonUp( UINT nFlags, CPoint point ) 
{
	CRect rect;
	LPWNDCTRL lpWndCtrl;

	if(m_dwPenaltyTime == 0)
	{
		for(int i=0; i<18; i++) 
		{
			lpWndCtrl = GetWndCtrl( m_nCustomID[i] );
			rect = lpWndCtrl->rect;
			if( rect.PtInRect( point ) )
			{
				if(m_nPair[0] == -1) //첫번째 그림을 선택했을 때는 기억만
					m_nPair[0] = i;
				else if(m_nPair[0] != -1 && m_nPair[0] != i && m_nPair[1] == -1) //두번째 그림 선택 시 서버로 결과를 전송
				{
					m_nPair[1] = i;

					CRainbowRace::GetInstance()->SendMinigamePacket( RMG_MAX, MP_NONE, m_nPair[0], m_nPair[1] );
				}
			}
		}
	}
}

BOOL CWndRRMiniGameCard::OnSetCursor( CWndBase* pWndBase, UINT nHitTest, UINT message )
{
	CRect rect;
	LPWNDCTRL lpWndCtrl;
	BOOL bOnTitle = FALSE;
	CPoint point = GetMousePoint();

	for(int i=0; i<18; i++) 
	{
		lpWndCtrl = GetWndCtrl( m_nCustomID[i] );
		rect = lpWndCtrl->rect;
		if( rect.PtInRect( point ) && !m_stCard[i].m_bCheck )
			bOnTitle = TRUE;
	}

	if(m_dwPenaltyTime == 0 && bOnTitle)
		SetMouseCursor( CUR_SELECT );
	else
	{
		SetMouseCursor( CUR_BASE );
		CWndBase::OnSetCursor( pWndBase, nHitTest, message );
	}

	return TRUE;
}

void CWndRRMiniGameCard::ReceiveResult(int nResult)
{
	if(nResult == MP_TRUE)
	{
		m_stCard[m_nPair[0]].m_bCheck = TRUE;
		m_stCard[m_nPair[1]].m_bCheck = TRUE;
	}
	else if(nResult == MP_FALSE)
		m_dwPenaltyTime = GetTickCount();

	m_nPair[0] = -1;
	m_nPair[1] = -1;
}


//////////////////////////////////////////////////////////////////////////
// RainbowRace Minigame - Ladder Fail MessageBox
//////////////////////////////////////////////////////////////////////////
void CWndRRMiniGameLadderFail::OnDestroy()
{
	if(g_WndMng.m_pWndRRMiniGameLadder)
		g_WndMng.m_pWndRRMiniGameLadder->Reset();
}

BOOL CWndRRMiniGameLadderFail::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CString strTemp;
	strTemp.Format(prj.GetText(TID_GAME_RRMINIGAME_LADDER_FAIL));

	return CWndMessageBox::Initialize( strTemp, pWndParent, MB_OK );
}

BOOL CWndRRMiniGameLadderFail::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
		case IDOK:
			{
				if(g_WndMng.m_pWndRRMiniGameLadder)
					g_WndMng.m_pWndRRMiniGameLadder->Reset();

				Destroy();
			}
			break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

//////////////////////////////////////////////////////////////////////////
// RainbowRace Minigame - Ladder Window
//////////////////////////////////////////////////////////////////////////

CWndRRMiniGameLadder::CWndRRMiniGameLadder()
{
	m_pText = NULL;
	m_nResult = -1;
	m_nStart = 0;
	m_nTarget = 0;
	m_nGoTo = GO_DOWN;
	m_nLevel = -1;
	m_bStart = FALSE;
	m_bMakeLadder = FALSE;
	m_bFailMsg = FALSE;
	m_nNextPosY = 0;

	m_pTexBg1 = NULL;
	m_pTexBg2 = NULL;
	m_pTexHorizon = NULL;
	m_pTexChoice = NULL;
	m_pTexPoint = NULL;
	m_pTexGoal = NULL;
	m_pTexFail = NULL;

	m_nLineCount = 0;
	m_bReverse = FALSE;
	m_fLerp = 0.0f;
	m_pWndRRMiniGameLadderFail = NULL;
}

CWndRRMiniGameLadder::~CWndRRMiniGameLadder()
{
	if(m_pWndRRMiniGameLadderFail)
		SAFE_DELETE(m_pWndRRMiniGameLadderFail);
}

void CWndRRMiniGameLadder::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	m_pText = (CWndText *)GetDlgItem( WIDC_TEXT1 );
	
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "RRMinigameLadder.inc" ) ));
	CHAR* szChar = scanner.m_pProg;

	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString(szChar);
		m_pText->ResetString();	
	}

	m_nStartCustomID[0] = WIDC_CUSTOM1;
	m_nStartCustomID[1] = WIDC_CUSTOM2;
	m_nStartCustomID[2] = WIDC_CUSTOM3;
	m_nStartCustomID[3] = WIDC_CUSTOM4;
	m_nStartCustomID[4] = WIDC_CUSTOM5;
	m_nStartCustomID[5] = WIDC_CUSTOM6;
	m_nStartCustomID[6] = WIDC_CUSTOM7;
	m_nStartCustomID[7] = WIDC_CUSTOM8;
	m_nStartCustomID[8] = WIDC_CUSTOM9;
	m_nStartCustomID[9] = WIDC_CUSTOM10;
	m_nStartCustomID[10] = WIDC_CUSTOM11;
	m_nStartCustomID[11] = WIDC_CUSTOM12;
	m_nStartCustomID[12] = WIDC_CUSTOM13;
	m_nStartCustomID[13] = WIDC_CUSTOM14;
	m_nStartCustomID[14] = WIDC_CUSTOM15;

	m_pTexBg1 = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "WndRainbowRaceLadderBg1.bmp"), 0xffff00ff );
	m_pTexBg2 = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "WndRainbowRaceLadderBg2.bmp"), 0xffff00ff );
	m_pTexHorizon = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "RainbowRaceLadderHorizon.bmp"), 0xffff00ff );
	m_pTexChoice = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "WndRainbowRaceLadderChoice.bmp"), 0xffff00ff );
	m_pTexPoint = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "WndRainbowRaceLadderPoint.bmp"), 0xffff00ff );
	m_pTexGoal = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "WndRainbowRaceLadderGoal.bmp"), 0xffff00ff );
	m_pTexFail = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "WndRainbowRaceLadderFail.bmp"), 0xffff00ff );

	m_ptPoint.x = GetWndCtrl(m_nStartCustomID[m_nStart])->rect.left;
	m_ptPoint.y = GetWndCtrl(m_nStartCustomID[m_nStart])->rect.top;

	m_rectPointLine.left = m_ptPoint.x + 9;
	m_rectPointLine.right = m_rectPointLine.left + 5;
	m_rectPointLine.top = m_ptPoint.y + 24;
	m_rectPointLine.bottom = m_rectPointLine.top;
	
	MoveParentCenter();
	Reset();
} 

BOOL CWndRRMiniGameLadder::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_RR_MINIGAME_LADDER, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndRRMiniGameLadder::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	if(nID == WIDC_OK)
	{
		m_bStart = TRUE;
		CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_OK);
		if(pWndButton)
			pWndButton->EnableWindow(FALSE);

		CRainbowRace::GetInstance()->SendMinigamePacket();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

void CWndRRMiniGameLadder::OnLButtonUp(UINT nFlags, CPoint point )
{
	CRect rect;
	LPWNDCTRL lpWndCtrl;

	if(!m_bStart)
	{
		for(int i=0; i<15; i++) 
		{
			lpWndCtrl = GetWndCtrl(m_nStartCustomID[i]);
			rect = lpWndCtrl->rect;
			if(rect.PtInRect(point))
			{
				m_nStart = i;
				m_ptPoint.x = lpWndCtrl->rect.left;
				m_ptPoint.y = lpWndCtrl->rect.top;

				m_rectPointLine.left = m_ptPoint.x + 9;
				m_rectPointLine.right = m_rectPointLine.left + 5;
				m_rectPointLine.top = m_ptPoint.y + 24;
				m_rectPointLine.bottom = m_rectPointLine.top;
			}
		}
	}
}

BOOL CWndRRMiniGameLadder::OnSetCursor(CWndBase* pWndBase, UINT nHitTest, UINT message )
{
	CRect rect;
	LPWNDCTRL lpWndCtrl;
	BOOL bOnTitle = FALSE;
	CPoint point = GetMousePoint();

	for(int i=0; i<15; i++) 
	{
		lpWndCtrl = GetWndCtrl( m_nStartCustomID[i] );
		rect = lpWndCtrl->rect;
		if( rect.PtInRect( point ) )
			bOnTitle = TRUE;
	}

	if(!m_bStart && bOnTitle)
		SetMouseCursor( CUR_SELECT );
	else
	{
		SetMouseCursor( CUR_BASE );
		CWndBase::OnSetCursor( pWndBase, nHitTest, message );
	}

	return TRUE;
}

void CWndRRMiniGameLadder::OnDraw(C2DRender* p2DRender)
{
	// Draw Choice Box
	for(int i=0; i<30; i++)
	{
		if(m_pTexChoice)
		{
			if(i<15)
				m_pTexChoice->Render( p2DRender, CPoint( GetWndCtrl( m_nStartCustomID[i] )->rect.left, GetWndCtrl( m_nStartCustomID[i] )->rect.top ) );
			else
				m_pTexChoice->Render( p2DRender, CPoint( GetWndCtrl( m_nStartCustomID[i-15] )->rect.left, GetWndCtrl( m_nStartCustomID[i-15] )->rect.top + 327 ) );
		}
	}

	if(m_pTexBg1 && m_pTexBg2 && m_pTexHorizon)
	{
		if(m_bStart && m_bMakeLadder)
		{
			// Draw Background
			m_pTexBg2->Render( p2DRender, CPoint( GetWndCtrl( WIDC_CUSTOM_BG )->rect.left, GetWndCtrl( WIDC_CUSTOM_BG )->rect.top ) );
			int nStartGap = GetWndCtrl( WIDC_CUSTOM_BG )->rect.top + 32;
			
			// Draw Random Horizon Line
			for(int i=0; i<15; i++)
			{
				for(int j=0; j<10; j++)
				{
					if(m_nLadder[i][j] > i)
						m_pTexHorizon->Render( p2DRender, CPoint(18 + (34*i), nStartGap + (25*j) - 2) );
				}
			}

			// Draw Goal Box
			for(i=0; i<15; i++)
			{
				if(m_nTarget == i)
				{
					if(m_pTexGoal)
						m_pTexGoal->Render( p2DRender, CPoint( GetWndCtrl( m_nStartCustomID[i] )->rect.left, GetWndCtrl( m_nStartCustomID[i] )->rect.top + 327 ) );
				}
				else
				{
					if(m_pTexFail)
						m_pTexFail->Render( p2DRender, CPoint( GetWndCtrl( m_nStartCustomID[i] )->rect.left, GetWndCtrl( m_nStartCustomID[i] )->rect.top + 327 ) );
				}
			}
		}
		else
			m_pTexBg1->Render( p2DRender, CPoint( GetWndCtrl( WIDC_CUSTOM_BG )->rect.left, GetWndCtrl( WIDC_CUSTOM_BG )->rect.top ) );
	}

	// Draw Line
	SetFillRectValue();
	for(i=0; i<m_nLineCount; i++)
		FillRect( p2DRender, m_rectLine[i], 0x00ff0000, 0xbbff0000 );

	FillRect( p2DRender, m_rectPointLine, 0x00ff0000, 0xbbff0000 );

	// Draw Point
	if(m_pTexPoint)
		m_pTexPoint->Render( p2DRender, CPoint(m_ptPoint.x, m_ptPoint.y) );
}

void CWndRRMiniGameLadder::SetFillRectValue()
{
	float fVar = 0.1f;
	if( m_bReverse )
		m_fLerp -= fVar;
	else
		m_fLerp += fVar;

	if( m_fLerp > 1.0f )
	{
		m_bReverse = TRUE;
		m_fLerp = 1.0f;
	}
	else if( m_fLerp < 0.0f )
	{
		m_bReverse = FALSE;
		m_fLerp = 0.0f;
	}
}

void CWndRRMiniGameLadder::FillRect(C2DRender *p2DRender, CRect rectBg, DWORD dwColorstart, DWORD dwColorend)
{
	CPoint Point = CPoint( rectBg.TopLeft().x, rectBg.TopLeft().y );
	
	D3DXCOLOR dwColorDest2 = dwColorstart;
	D3DXCOLOR dwColor = dwColorend;
	D3DXCOLOR dwColor1 = D3DCOLOR_ARGB( 0, 255, 255, 255 );
	D3DXCOLOR dwColor2 = D3DCOLOR_ARGB( 0, 255, 255, 255 );

	float fLerp = 1.0f;
	fLerp = m_fLerp;

	D3DXColorLerp( &dwColor2, &dwColor, &dwColorDest2, fLerp );
	int nthick = 8;	
	CRect Rect = CRect( 0, 0, rectBg.right - rectBg.left, rectBg.bottom - rectBg.top );
	
	p2DRender->RenderFillRect( CRect( Point.x+Rect.left, Point.y+Rect.top, Point.x+Rect.left+nthick, Point.y+Rect.bottom ),
		dwColor2, dwColor1, dwColor2, dwColor1 );
	
	p2DRender->RenderFillRect( CRect( Point.x+Rect.left, Point.y+Rect.top+nthick, Point.x+Rect.right, Point.y+Rect.top ),
		dwColor1, dwColor1, dwColor2, dwColor2 );
	
	p2DRender->RenderFillRect( CRect( Point.x+Rect.right, Point.y+Rect.top, Point.x+Rect.right-nthick, Point.y+Rect.bottom ),
		dwColor2, dwColor1, dwColor2, dwColor1 );
	
	p2DRender->RenderFillRect( CRect( Point.x+Rect.left, Point.y+Rect.bottom, Point.x+Rect.right, Point.y+Rect.bottom-nthick ),
		dwColor2, dwColor2, dwColor1, dwColor1 );
}

BOOL CWndRRMiniGameLadder::Process()
{
	BOOL bEnd = FALSE;

	int nDownSpeed = 3;
	int nTurnSpeed = 2;

	// 분기 레벨은 첫 분기 도달까지를 -1로 본다.
	if(m_bMakeLadder && m_nLevel >= -1 && m_nLevel < 10) // 사다리가 완성되고 분기점이 -1 ~ 9 일 경우 시작
	{
		int nStartLeft;
		int nFirstTop = GetWndCtrl(WIDC_CUSTOM_BG)->rect.top + 20; // 첫번째 분기까지의 거리

		if(m_nLevel == -1) // 첫 분기 전일 경우는 포인트는 시작점 (포인트 = 막대기번호)
			m_nPoint = m_nStart;
			
		if(m_nPoint >=0 && m_nPoint < 15)
			nStartLeft = GetWndCtrl(m_nStartCustomID[m_nPoint])->rect.left;

		switch(m_nGoTo)
		{
			case GO_DOWN:
				{
					if(m_nLevel == -1)
						m_nNextPosY = nFirstTop;
					else
						m_nNextPosY = nFirstTop + (25*(m_nLevel+1));

					if(m_ptPoint.y < m_nNextPosY) // 분기레벨 Y좌표 도달할때 까지 Y좌표 증가
					{
						m_ptPoint.y += nDownSpeed;
						m_rectPointLine.bottom = m_ptPoint.y;
					}
					else
					{
						// 분기 끝나면 라인 추가
						m_rectPointLine.bottom += 10;
						m_rectLine[m_nLineCount] = m_rectPointLine;
						m_nLineCount++;

						m_nLevel++; // 다운완료 후 새 분기점 도달 시 분기 레벨 증가 이동라인 시작점 갱신

						if(m_nLevel < 10)
						{
							if(m_nLadder[m_nPoint][m_nLevel] == -1)
							{
								m_nGoTo = GO_DOWN;
								m_rectPointLine.top = m_rectPointLine.bottom;
							}
							else if(m_nLadder[m_nPoint][m_nLevel] < m_nPoint)
							{
								m_nPoint = m_nLadder[m_nPoint][m_nLevel];
								m_nGoTo = GO_LEFT;
								
								m_rectPointLine.top = m_rectPointLine.bottom - 2;
								m_rectPointLine.bottom += 3;
								m_rectPointLine.left = m_rectPointLine.right;
							}
							else if(m_nLadder[m_nPoint][m_nLevel] > m_nPoint)
							{
								m_nPoint = m_nLadder[m_nPoint][m_nLevel];
								m_nGoTo = GO_RIGHT;

								m_rectPointLine.top = m_rectPointLine.bottom - 2;
								m_rectPointLine.bottom += 3;
								m_rectPointLine.right = m_rectPointLine.left;
							}
						}
					}
				}
				break;
			case GO_LEFT:
				{
					if(m_ptPoint.x > nStartLeft) // 분기레벨 X좌표 도달할때 까지 Y좌표 증가
					{
						m_ptPoint.x -= nTurnSpeed;
						m_rectPointLine.left = m_ptPoint.x;
					}
					else
					{
						m_nGoTo = GO_DOWN;
						// 분기 끝나면 라인 추가
						m_rectPointLine.left += 9;
						m_rectLine[m_nLineCount] = m_rectPointLine;
						m_nLineCount++;
						m_rectPointLine.right = m_rectPointLine.left + 5;
					}
				}
				break;
			case GO_RIGHT:
				{
					if(m_ptPoint.x < nStartLeft) // 분기레벨 X좌표 도달할때 까지 Y좌표 증가
					{
						m_ptPoint.x += nTurnSpeed;
						m_rectPointLine.right = m_ptPoint.x;
					}
					else
					{
						m_nGoTo = GO_DOWN;
						// 분기 끝나면 라인 추가
						m_rectPointLine.right += 14;
						m_rectLine[m_nLineCount] = m_rectPointLine;
						m_nLineCount++;
						m_rectPointLine.left = m_rectPointLine.right - 5;
					}
				}
				break;
		}
	}
	else if(!bEnd && m_bMakeLadder && m_nLevel == 10)
	{
		if(m_ptPoint.y < GetWndCtrl(WIDC_CUSTOM_BG)->rect.bottom)
		{
			m_ptPoint.y += nDownSpeed;
			m_rectPointLine.bottom = m_ptPoint.y;
		}
		else
			bEnd = TRUE;
	}

	if(m_nLevel == 10 && bEnd)
	{
		if(m_nResult == MP_FINISH)
		{
			// 성공 메세지 후 종료
			g_WndMng.m_pWndRainbowRaceMiniGameEnd = new CWndRainbowRaceMiniGameEnd;

			if(g_WndMng.m_pWndRainbowRaceMiniGameEnd)
			{
				g_WndMng.m_pWndRainbowRaceMiniGameEnd->SetGameID(RMG_LADDER);
				g_WndMng.m_pWndRainbowRaceMiniGameEnd->Initialize(NULL);
			}
			Destroy();
		}
		else if(m_nResult == MP_FAIL && !m_bFailMsg)
		{
			//실패 메세지 후 리셋
			m_bFailMsg = TRUE;

			if(m_pWndRRMiniGameLadderFail)
				SAFE_DELETE(m_pWndRRMiniGameLadderFail);

			m_pWndRRMiniGameLadderFail = new CWndRRMiniGameLadderFail;
			if(m_pWndRRMiniGameLadderFail)
				m_pWndRRMiniGameLadderFail->Initialize(NULL);
		}
	}
	
	return TRUE;
}

void CWndRRMiniGameLadder::ReceiveResult(int nResult)
{
	m_nResult = nResult;
	m_bMakeLadder = MakeLadder();
}

BOOL CWndRRMiniGameLadder::MakeLadder()
{
	for(int i=0; i<15; i++)
	{
//		CString strtemp;
//		for(int k=0; k<10; k++)
//		{
//			CString str;
//			str.Format("%d ", m_nLadder[14][k]);
//			strtemp += str;
//		}
//		strtemp += "\n";
//		TRACE(strtemp);

		for(int j=0; j<10; j++)
		{
			int nNext;
			if(i>0 && i<14) // 맨 처음과 끝을 뺀 가운데 막대기
			{
				nNext = rand() % 3;
				if(m_nLadder[i-1][j] != i) // 이전 막대기가 나를 향할 경우 분기 제외
				{
					if(i>1 && m_nLadder[i-2][j] == i-1 && nNext == GO_LEFT) // 세번째 막대기 부터는 전전 막대기가 전막대기를 향하고 나온 방향이 왼쪽일 경우 분기 제외
						nNext = GO_DOWN;
					
					switch(nNext)
					{
						case GO_DOWN:
							m_nLadder[i][j] = -1;
							break;
						case GO_LEFT:
							{
								m_nLadder[i][j] = i-1;
								m_nLadder[i-1][j] = i;
							}
							break;
						case GO_RIGHT:
							{
								m_nLadder[i][j] = i+1;
								m_nLadder[i+1][j] = i;
							}
							break;
					}
				}
			}
			else if(i == 0)// 맨 처음 막대기
			{
				nNext = rand() % 2;
				if(nNext == 0)
				{
					if(i==0)
					{
						m_nLadder[i][j] = i+1;
						m_nLadder[i+1][j] = i;
					}
				}
				else
					m_nLadder[i][j] = -1;
			}
			else // 맨 끝 막대기
			{
				if(m_nLadder[i-1][j] == i) // 전 막대기가 나를 볼 경우 나도 전 막대기를 향한다.
					m_nLadder[i][j] = i-1;
				else if(m_nLadder[i-2][j] == i-1) // 전전 막대기가 전 막대기를 볼 경우 중복을 피해 분기 제외
					m_nLadder[i][j] = -1;
				else // 걸리는게 없을 경우 랜덤으로 분기를 결정
				{
					nNext = rand() % 2;
					if(nNext == 0)
					{
						if(i==0)
						{
							m_nLadder[i][j] = i-1;
							m_nLadder[i-1][j] = i;
						}
					}
					else
						m_nLadder[i][j] = -1;
				}
			}
		}
	}

	// 시뮬레이팅 후 서버에서 온 성공 실패에 따라 결과값 셋 팅
	int nGoal = m_nStart;
	for(i=0; i<10; i++)
	{
		if(m_nLadder[nGoal][i] != -1)
			nGoal = m_nLadder[nGoal][i];
	}

	int nTarget = nGoal;
	while(nTarget == nGoal)
		nTarget = rand() % 15;
	
	if(m_nResult == MP_FINISH)
		m_nTarget = nGoal;
	else if(m_nResult == MP_FAIL)
		m_nTarget = nTarget;

	return TRUE;
}

void CWndRRMiniGameLadder::Reset()
{
	m_nResult = -1;
	m_nStart = 0;
	m_nTarget = 0;
	m_nGoTo = GO_DOWN;
	m_nLevel = -1;
	m_bStart = FALSE;
	m_bMakeLadder = FALSE;
	m_bFailMsg = FALSE;
	m_ptPoint.x = GetWndCtrl(m_nStartCustomID[m_nStart])->rect.left;
	m_ptPoint.y = GetWndCtrl(m_nStartCustomID[m_nStart])->rect.top;
	m_nLineCount = 0;

	m_rectPointLine.left = m_ptPoint.x + 9;
	m_rectPointLine.right = m_rectPointLine.left + 5;
	m_rectPointLine.top = m_ptPoint.y + 24;
	m_rectPointLine.bottom = m_rectPointLine.top;

	CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_OK);
	if(pWndButton)
		pWndButton->EnableWindow(TRUE);

	for(int i=0; i<15; i++)
	{
		for(int j=0; j<10; j++)
			m_nLadder[i][j] = -1;
	}
}

#endif //__RAINBOW_RACE
