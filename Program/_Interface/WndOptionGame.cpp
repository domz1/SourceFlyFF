#include "stdafx.h"
#include "AppDefine.h"
#include "WndOptionGame.h"
#include "DPClient.h"
#include "defineSound.h"

extern	CDPClient	g_DPlay;

#define		MAX_SLIDER		10


/****************************************************
  WndId : APP_OPTION_GAME - Game Option
****************************************************/

CWndOptionGame::CWndOptionGame() 
{ 
#if __VER >= 12 // __UPDATE_OPT
	m_bLButtonClick = FALSE;
	m_bLButtonClick2 = FALSE;
	
	m_nStep[0] = 0;
	m_nStep[1] = 0;
#endif
} 
CWndOptionGame::~CWndOptionGame() 
{ 
} 
void CWndOptionGame::OnDraw( C2DRender* p2DRender ) 
{
#if __VER >= 12 // __UPDATE_OPT
	CPoint pt;

	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );
	CPoint pt1 = lpWndCtrl->rect.TopLeft();
	CPoint pt2 = lpWndCtrl->rect.BottomRight() - lpWndCtrl->rect.TopLeft();
	p2DRender->RenderFillRect( lpWndCtrl->rect, 0);	
	m_Texture.Render( p2DRender, pt1, pt2 );
	pt = pt1;
	pt.x += GetStepPos( m_nStep[0], lpWndCtrl->rect.right - lpWndCtrl->rect.left, 10 ).x;
	m_TexturePt.Render( p2DRender, pt, CPoint( 16, 16 ) );
	
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
	pt1 = lpWndCtrl->rect.TopLeft();
	pt2 = lpWndCtrl->rect.BottomRight() - lpWndCtrl->rect.TopLeft();
	m_Texture.Render( p2DRender, pt1, pt2 );
	pt = pt1;
	pt.x += GetStepPos( m_nStep[1], lpWndCtrl->rect.right - lpWndCtrl->rect.left, 10 ).x;
	m_TexturePt.Render( p2DRender, pt, CPoint( 16, 16 ) );
#endif
} 

void CWndOptionGame::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요


	pWndButton1[ 0 ] = (CWndButton*)GetDlgItem( WIDC_RADIO1 );
	pWndButton1[ 1 ] = (CWndButton*)GetDlgItem( WIDC_RADIO2 );
	pWndButton1[ 0 ]->SetGroup( TRUE );
	pWndButton1[ !(g_Option.m_bTrade) ]->SetCheck( TRUE );
/*Fim*/

	pWndButton2[ 0 ] = (CWndButton*)GetDlgItem( WIDC_RADIO3 );
	pWndButton2[ 1 ] = (CWndButton*)GetDlgItem( WIDC_RADIO4 );
	pWndButton2[ 0 ]->SetGroup( TRUE );
	pWndButton2[ !(g_Option.m_bSay) ]->SetCheck( TRUE );
/*Fim*/

	pWndButton3[ 0 ] = (CWndButton*)GetDlgItem( WIDC_RADIO5 );
	pWndButton3[ 1 ] = (CWndButton*)GetDlgItem( WIDC_RADIO6 );
	pWndButton3[ 0 ]->SetGroup( TRUE );
	pWndButton3[ !(g_Option.m_bParty) ]->SetCheck( TRUE );

#if __VER >= 12 // __UPDATE_OPT
	m_Texture.LoadTexture( g_Neuz.GetDevice(), MakePath( DIR_THEME, "WndVolumeBar.tga" ), 0xffff00ff, TRUE );
	m_TexturePt.LoadTexture( g_Neuz.GetDevice(), MakePath( DIR_THEME, "ButtSpin.tga" ), 0xffffffff, TRUE );	
	m_nStep[0] = (int)( g_Option.m_fEffectVolume * 10 );
	m_nStep[1] = (int)( g_Option.m_fBGMVolume * 10 );

	pWndHelp = (CWndButton*)GetDlgItem( WIDC_CHECK1 );
	pWndHelp->SetCheck( g_Option.m_nInstantHelp );

	pWndCheck = (CWndButton*)GetDlgItem( WIDC_CHECK2 );
	if(*g_Option.m_pGuide > 0)
	pWndCheck->SetCheck( FALSE );
	else
	pWndCheck->SetCheck( TRUE );

	pWndAlpha = (CWndButton*)GetDlgItem( WIDC_CHECK3 );
	pWndAlpha->SetCheck( g_Option.m_nWindowAlpha == 255 ? FALSE : TRUE);

#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
	pWndCheckBattleBGM = (CWndButton*)GetDlgItem( WIDC_CHECK_BATTLE_BGM );
	pWndCheckBattleBGM->SetCheck( g_Option.m_bBattleBGM ? TRUE : FALSE );
#endif // __IMPROVE_SYSTEM_VER15
#ifdef __GAME_GRADE_SYSTEM
	pWndCheckGameGrade = (CWndButton*)GetDlgItem( WIDC_CHECK_GAME_GRADE );
	if( pWndCheckGameGrade )
	{
		if( ::GetLanguage() == LANG_KOR )
		{
			pWndCheckGameGrade->SetCheck( g_Option.m_bGameGradeRendering ? TRUE : FALSE );
		}
		else
		{
			pWndCheckGameGrade->SetVisible( FALSE );
		}
	}
#endif // __GAME_GRADE_SYSTEM

	pWndButton4[ 0 ] = (CWndButton*)GetDlgItem( WIDC_RADIO7 );
	pWndButton4[ 1 ] = (CWndButton*)GetDlgItem( WIDC_RADIO8 );
	pWndButton4[ 0 ]->SetGroup( TRUE );
	pWndButton4[ !(g_Option.m_bMessengerJoin) ]->SetCheck( TRUE );
/*Fim*/

#else
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_SHOUT_ON );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_SHOUT_OFF );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ !(g_Option.m_bShout) ]->SetCheck( TRUE );
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_MESSENGERJOIN_ON );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_MESSENGERJOIN_OFF );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ !(g_Option.m_bMessengerJoin) ]->SetCheck( TRUE );
#endif
	

	pWndButton5[ 0 ] = (CWndButton*)GetDlgItem( WIDC_MESSENGER_ON );
	pWndButton5[ 1 ] = (CWndButton*)GetDlgItem( WIDC_MESSENGER_OFF );
	pWndButton5[ 0 ]->SetGroup( TRUE );
	pWndButton5[ !(g_Option.m_bMessenger) ]->SetCheck( TRUE );

#if __VER >= 9 // __CSC_VER9_REMOVE_PKSETTING
	CRect rect;
	LPWNDCTRL lpWndCtrl;
	// 자동 PK 모드 옵션 제거
	pWndButton6[ 0 ] = (CWndButton*)GetDlgItem( WIDC_AUTOPK_ON );
	pWndButton6[ 1 ] = (CWndButton*)GetDlgItem( WIDC_AUTOPK_OFF );
	pWndButton6[ 0 ]->EnableWindow(FALSE);
	pWndButton6[ 0 ]->SetVisible(FALSE);
	pWndButton6[ 1 ]->EnableWindow(FALSE);
	pWndButton6[ 1 ]->SetVisible(FALSE);
	CWndStatic* pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC8 );
	pStatic->EnableWindow(FALSE);
	pStatic->SetVisible(FALSE);

	// 옵션 위치 제조정
	lpWndCtrl = GetWndCtrl( WIDC_STATIC8 );
	rect = lpWndCtrl->rect;


	pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC9 );
	pStatic->Move(rect.TopLeft().x, rect.TopLeft().y);


	pWndButton7[ 0 ] = (CWndButton*)GetDlgItem( WIDC_ACCEPTPVP_ON );
	lpWndCtrl = GetWndCtrl( WIDC_AUTOPK_ON );
	rect = lpWndCtrl->rect;
	pWndButton7[ 0 ]->Move(rect.TopLeft().x, rect.TopLeft().y);
/*Fim*/

	pWndButton7[ 1 ] = (CWndButton*)GetDlgItem( WIDC_ACCEPTPVP_OFF );
	lpWndCtrl = GetWndCtrl( WIDC_AUTOPK_OFF );
	rect = lpWndCtrl->rect;
	pWndButton7[ 1 ]->Move(rect.TopLeft().x, rect.TopLeft().y);
/*Fim*/

#else //__CSC_VER9_REMOVE_PKSETTING
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_AUTOPK_ON );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_AUTOPK_OFF );
	pWndButton[ 0 ]->SetGroup( TRUE );
	if( g_pPlayer )
		pWndButton[ !(g_pPlayer->IsMode( FREEPK_MODE ) ) ]->SetCheck( TRUE );
	else
		pWndButton[ 1 ]->SetCheck( TRUE );
#endif //__CSC_VER9_REMOVE_PKSETTING
	
	pWndButton7[ 0 ] = (CWndButton*)GetDlgItem( WIDC_ACCEPTPVP_ON );
	pWndButton7[ 1 ] = (CWndButton*)GetDlgItem( WIDC_ACCEPTPVP_OFF );
	pWndButton7[ 0 ]->SetGroup( TRUE );
	if( g_pPlayer )
		pWndButton7[ g_pPlayer->IsMode( PVPCONFIRM_MODE ) ]->SetCheck( TRUE );
	else
		pWndButton7[ 0 ]->SetCheck( TRUE );
	
/*Fim*/


	pWndButton8[ 0 ] = (CWndButton*)GetDlgItem( WIDC_RADIO12 );
	pWndButton8[ 1 ] = (CWndButton*)GetDlgItem( WIDC_RADIO13 );
	pWndButton8[ 0 ]->SetGroup( TRUE );
	if( g_pPlayer->IsMode( EQUIP_DENIAL_MODE ) )
		pWndButton8[ 1 ]->SetCheck( TRUE );
	else
		pWndButton8[ 0 ]->SetCheck( TRUE );

/*Fim*/

	pWndButton9[ 0 ] = (CWndButton*)GetDlgItem( WIDC_RADIO14 );
	pWndButton9[ 1 ] = (CWndButton*)GetDlgItem( WIDC_RADIO15 );
	pWndButton9[ 0 ]->SetGroup( TRUE );

	if( g_Option.m_bAutoAttack )
		pWndButton9[ 0 ]->SetCheck( TRUE );	
	else
		pWndButton9[ 1 ]->SetCheck( TRUE );		
	
	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndOptionGame::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_OPTION_GAME, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndOptionGame::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndOptionGame::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndOptionGame::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndOptionGame::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
#if __VER >= 12 // __UPDATE_OPT
	PLAYSND(SND_NONE);
	ReleaseCapture();
	m_bLButtonClick = FALSE;
	m_bLButtonClick2 = FALSE;
#endif
} 
void CWndOptionGame::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
#if __VER >= 12 // __UPDATE_OPT
	SetCapture();

	int nResult = 0;
	nResult = GetSliderStep( WIDC_CUSTOM2, m_nStep[0], point );	
	if( nResult == -1 )
	{
		m_bLButtonClick = TRUE;
		return;
	}
	else
	if( nResult == 1 )
	{
		g_Option.m_fEffectVolume	= m_nStep[0]*0.1f;
		g_SoundMng.m_nSoundVolume	= (int)( -(1.0f-g_Option.m_fEffectVolume)*5000 );
	}
	/////////////////////////////////////////
	nResult = GetSliderStep( WIDC_CUSTOM1, m_nStep[1], point );	
	if( nResult == -1 )
	{
		m_bLButtonClick2 = TRUE;
		return;
	}
	else
	if( nResult == 1 )
	{
		g_Option.m_fBGMVolume = m_nStep[1]*0.1f;
		SetVolume(g_Option.m_fBGMVolume);		
	}
#endif
} 
BOOL CWndOptionGame::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
#ifndef __OPTION_EX
	pWndHelp = (CWndButton*)GetDlgItem( WIDC_CHECK1 );
	pWndCheck = (CWndButton*)GetDlgItem( WIDC_CHECK2 );
	pWndAlpha = (CWndButton*)GetDlgItem( WIDC_CHECK3 );
#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
	pWndCheckBattleBGM = (CWndButton*)GetDlgItem( WIDC_CHECK_BATTLE_BGM );
#endif // __IMPROVE_SYSTEM_VER15
#ifdef __GAME_GRADE_SYSTEM
	pWndCheckGameGrade = (CWndButton*)GetDlgItem( WIDC_CHECK_GAME_GRADE );
#endif // __GAME_GRADE_SYSTEM

	switch( nID )
	{
	case WIDC_RADIO1:
		g_Option.m_bTrade = 1;
		break;
	case WIDC_RADIO2:
		g_Option.m_bTrade = 0;
		break;
	case WIDC_RADIO3:
		g_Option.m_bSay = 1;
		break;
	case WIDC_RADIO4:
		g_Option.m_bSay = 0;
		break;
	case WIDC_RADIO5:
		g_Option.m_bParty = 1;
		break;
	case WIDC_RADIO6:
		g_Option.m_bParty = 0;
		break;
#if __VER >= 12 // __UPDATE_OPT
	case WIDC_CHECK1: // 초보자 도움말 옵션 
		if( pWndHelp->GetCheck() )
			g_Option.m_nInstantHelp = TRUE;
		else
			g_Option.m_nInstantHelp = FALSE;
		break;
	case WIDC_CHECK2:
		if(pWndCheck->GetCheck()) 
			*g_Option.m_pGuide = 0;
		else
		{
			CWndGuideSystem* pWndGuide = NULL;
			pWndGuide = (CWndGuideSystem*)GetWndBase( APP_GUIDE );
			//pWndGuide->SetVisible(FALSE);
			pWndGuide->m_wndGuideText->SetVisible(FALSE);
			*g_Option.m_pGuide = 2;
		}
		break;
	case WIDC_CHECK3: // 윈도 반투명 옵션 
		if( pWndAlpha->GetCheck() )
			CWndBase::m_nAlpha = g_Option.m_nWindowAlpha = 128;
		else
			CWndBase::m_nAlpha = g_Option.m_nWindowAlpha = 255;
		break;
#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
	case WIDC_CHECK_BATTLE_BGM: // 전투 음악
		{
			g_Option.m_bBattleBGM = ( pWndCheckBattleBGM->GetCheck() == TRUE ) ? TRUE: FALSE;
			break;
		}
#endif // __IMPROVE_SYSTEM_VER15
#ifdef __GAME_GRADE_SYSTEM
	case WIDC_CHECK_GAME_GRADE: // 게임물 등급 표시
		{
			BOOL bGameGradeChecked = pWndCheckGameGrade->GetCheck();
			g_Option.m_bGameGradeRendering = ( bGameGradeChecked == TRUE ) ? TRUE: FALSE;
			if( bGameGradeChecked == TRUE )
			{
				g_Neuz.m_dwTimeGameGradeMarkRendering = g_tmCurrent + SEC( CNeuzApp::GAME_GRADE_MARK_RENDERING_INTERVAL_SECOND );
			}
			break;
		}
#endif // __GAME_GRADE_SYSTEM
	case WIDC_RADIO7:
		g_Option.m_bMessengerJoin = 1;
		break;
	case WIDC_RADIO8:
		g_Option.m_bMessengerJoin = 0;
#else
	case WIDC_SHOUT_ON:
		g_Option.m_bShout = 1;
		break;
	case WIDC_SHOUT_OFF:
		g_Option.m_bShout = 0;
		break;
	case WIDC_MESSENGERJOIN_ON:
		g_Option.m_bMessengerJoin = 1;
		break;
	case WIDC_MESSENGERJOIN_OFF:
		g_Option.m_bMessengerJoin = 0;
		break;	
#endif


	case WIDC_MESSENGER_ON:
		g_Option.m_bMessenger = 1;
		break;
	case WIDC_MESSENGER_OFF:
		g_Option.m_bMessenger = 0;
		break;
#if __VER < 9 // __CSC_VER9_REMOVE_PKSETTING
	case WIDC_AUTOPK_ON:
		g_DPlay.SendChangePKPVPMode( FREEPK_MODE, 0 );
		break;
	case WIDC_AUTOPK_OFF:
		g_DPlay.SendChangePKPVPMode( FREEPK_MODE, 1 );
		break;
#endif //__CSC_VER9_REMOVE_PKSETTING
	case WIDC_ACCEPTPVP_ON:
		g_DPlay.SendChangePKPVPMode( PVPCONFIRM_MODE, 3 );
		break;
	case WIDC_ACCEPTPVP_OFF:
		g_DPlay.SendChangePKPVPMode( PVPCONFIRM_MODE, 2 );
		break;
	case WIDC_RADIO12:		
		g_DPlay.SendQueryEquipSetting(TRUE);			
		break;
	case WIDC_RADIO13:
		g_DPlay.SendQueryEquipSetting(FALSE);			
		break;
	case WIDC_RADIO14:
		g_Option.m_bAutoAttack = 1;
		break;		
	case WIDC_RADIO15:
		g_Option.m_bAutoAttack = 0;
		break;		
	}
#endif//__OPTION_EX
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

#if __VER >= 12 // __UPDATE_OPT

void CWndOptionGame::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bLButtonClick )
	{
		GetRangeSlider( WIDC_CUSTOM2, m_nStep[0], point );

		g_Option.m_fEffectVolume	= m_nStep[0]*0.1f;
		g_SoundMng.m_nSoundVolume	= (int)( -(1.0f-g_Option.m_fEffectVolume)*5000 );
	}

	if( m_bLButtonClick2 )
	{
		GetRangeSlider( WIDC_CUSTOM1, m_nStep[1], point );

		g_Option.m_fBGMVolume = m_nStep[1]*0.1f;
		SetVolume(g_Option.m_fBGMVolume);
	}
}

CPoint CWndOptionGame::GetStepPos(int nStep, int nWidth, int nDivision)
{
	return nStep * (nWidth / (nDivision+1));
}

int CWndOptionGame::GetSliderStep(DWORD dwWndId, int &nStep, CPoint point )
{
	LPWNDCTRL lpWndCtrl = GetWndCtrl( dwWndId );	
	CRect crect = lpWndCtrl->rect;
	
	crect.left += GetStepPos( nStep, lpWndCtrl->rect.right - lpWndCtrl->rect.left, MAX_SLIDER ).x;
	crect.right = crect.left+16;
	
	if( PtInRect( crect, point ) )
	{
		return -1;
	}
	
	crect = lpWndCtrl->rect;
	if( PtInRect( crect, point ) )
	{
		int nWidth = lpWndCtrl->rect.right - lpWndCtrl->rect.left;
		
		if( point.x > lpWndCtrl->rect.left+GetStepPos( nStep, nWidth, MAX_SLIDER ).x )
			nStep += 1;//nWidth / 10;
		else
			nStep -= 1;//nWidth / 10;

		return 1;
	}

	return 0;
}

void CWndOptionGame::GetRangeSlider(DWORD dwWndId, int &nStep, CPoint point)
{
	LPWNDCTRL lpWndCtrl = GetWndCtrl( dwWndId );	
	int nWidth = lpWndCtrl->rect.right - lpWndCtrl->rect.left;
	
	if( point.x > lpWndCtrl->rect.left+GetStepPos( nStep, nWidth, MAX_SLIDER ).x )
		nStep += 1;//nWidth / 10;
	if( point.x < lpWndCtrl->rect.left+GetStepPos( nStep, nWidth, MAX_SLIDER ).x )
		nStep -= 1;//nWidth / 10;
	
	if( nStep < 0 )
		nStep = 0;
	
	if( nStep > MAX_SLIDER )
		nStep = MAX_SLIDER;
}

HRESULT CWndOptionGame::InvalidateDeviceObjects()
{
#ifdef __YDEBUG
	m_Texture.Invalidate();
	m_TexturePt.Invalidate();
#endif //__YDEBUG
	return CWndNeuz::InvalidateDeviceObjects();
}
HRESULT CWndOptionGame::RestoreDeviceObjects()
{
#ifdef __YDEBUG
	m_Texture.SetInvalidate(m_pApp->m_pd3dDevice);
	m_TexturePt.SetInvalidate(m_pApp->m_pd3dDevice);
#endif //__YDEBUG
	return CWndNeuz::RestoreDeviceObjects();
}

#endif

