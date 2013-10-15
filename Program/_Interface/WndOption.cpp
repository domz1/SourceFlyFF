// WndArcane.cpp: implementation of the CWndNeuz class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AppDefine.h"
#include "Resdata.h"
#include "defineSound.h"
#include "definetext.h"
#include "DPClient.h"

#define		MAX_SLIDER		10

extern	CDPClient	g_DPlay;

CWndTotalOption::CWndTotalOption() 
{ 
} 
CWndTotalOption::~CWndTotalOption() 
{ 
} 

void CWndTotalOption::UpdateDataAll()
{
	//m_OptTabVideoSnd
}

void CWndTotalOption::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndTotalOption::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	//	¿©±â¿¡ ÄÚµùÇÏ¸é µË´Ï´Ù
	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	CRect rect = GetClientRect();
	rect.left = 5;
	rect.top = 0;

	
	
#if __VER >= 12 // __UPDATE_OPT
	m_OptTabVideoSnd.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_OPTEX_AV12);
	m_OptTabGame.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_OPTEX_GAME12);
#else
	m_OptTabVideoSnd.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_OPTEX_AV );
	m_OptTabGame.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_OPTEX_GAME );
	m_OptTabEtc.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_OPTEX_ETC );
#ifdef __SFX_OPT
	m_OptTabSound.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_OPTEX_SOUND);
#endif
#endif
	WTCITEM tabTabItem;
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;

#if __VER >= 12 // __UPDATE_OPT
	tabTabItem.pszText = prj.GetText(TID_TOOLTIP_OPT_VIDEOAUDIO);
	//tabTabItem.pszText = prj.GetText(TID_APP_OPTION_VIDEO);
	tabTabItem.pWndBase = &m_OptTabVideoSnd;
	pWndTabCtrl->InsertItem( 0, &tabTabItem );

	tabTabItem.pszText = prj.GetText(TID_TOOLTIP_OPT_GAME);
	//tabTabItem.pszText = prj.GetText(TID_APP_OPTION_GAME);
	tabTabItem.pWndBase = &m_OptTabGame;
	pWndTabCtrl->InsertItem( 1, &tabTabItem );
#else
	tabTabItem.pszText = prj.GetText(TID_TOOLTIP_OPT_VIDEOAUDIO);
	//tabTabItem.pszText = prj.GetText(TID_APP_OPTION_VIDEO);
	tabTabItem.pWndBase = &m_OptTabVideoSnd;
	pWndTabCtrl->InsertItem( 0, &tabTabItem );

	tabTabItem.pszText = prj.GetText(TID_TOOLTIP_OPT_ETC);
	//tabTabItem.pszText = prj.GetText(TID_APP_OPTION_ETC);
	tabTabItem.pWndBase = &m_OptTabEtc;
	pWndTabCtrl->InsertItem( 1, &tabTabItem );
	
	tabTabItem.pszText = prj.GetText(TID_TOOLTIP_OPT_GAME);
	//tabTabItem.pszText = prj.GetText(TID_APP_OPTION_GAME);
	tabTabItem.pWndBase = &m_OptTabGame;
	pWndTabCtrl->InsertItem( 2, &tabTabItem );
#ifdef __SFX_OPT
	tabTabItem.pszText = prj.GetText(TID_TOOLTIP_OPT_SOUND);
	tabTabItem.pWndBase = &m_OptTabSound;
	pWndTabCtrl->InsertItem( 3, &tabTabItem );
#endif
#endif
	MoveParentCenter();	
} 
// Ã³À½ ÀÌ ÇÔ¼ö¸¦ ºÎ¸£¸é À©µµ°¡ ¿­¸°´Ù.
BOOL CWndTotalOption::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndTotalOption::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndTotalOption::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndTotalOption::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndTotalOption::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndTotalOption::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
#ifdef __OPTION_EX
	    case WIDC_BUT_OK:
		{
/*m_OptTabGame*/
			if( m_OptTabGame.pWndButton1[0]->GetCheck())
			{
					g_Option.m_bTrade = 1;
			}else
			if( m_OptTabGame.pWndButton1[1]->GetCheck())
			{
					g_Option.m_bTrade = 0;
			}
			if( m_OptTabGame.pWndButton2[0]->GetCheck())
			{
					g_Option.m_bSay = 1;
			}else
			if( m_OptTabGame.pWndButton2[1]->GetCheck())
			{
					g_Option.m_bSay = 0;
			}
			if( m_OptTabGame.pWndButton3[0]->GetCheck())
			{
					g_Option.m_bParty = 1;
			}else
			if( m_OptTabGame.pWndButton3[1]->GetCheck())
			{
				    g_Option.m_bParty = 0;
			}
			if( m_OptTabGame.pWndButton4[0]->GetCheck())
			{
					g_Option.m_bMessengerJoin = 1;
			}else
			if( m_OptTabGame.pWndButton4[1]->GetCheck())
			{
					g_Option.m_bMessengerJoin = 0;
			}
			if( m_OptTabGame.pWndButton5[0]->GetCheck())
			{
					g_Option.m_bMessenger = 1;
			}else
			if( m_OptTabGame.pWndButton5[1]->GetCheck())
			{
					g_Option.m_bMessenger = 0;
			}
			if( m_OptTabGame.pWndButton6[0]->GetCheck())
			{
					g_DPlay.SendChangePKPVPMode( FREEPK_MODE, 0 );
			}else
			if( m_OptTabGame.pWndButton6[1]->GetCheck())
			{
					g_DPlay.SendChangePKPVPMode( FREEPK_MODE, 1 );
			}
			if( m_OptTabGame.pWndButton7[0]->GetCheck())
			{
					g_DPlay.SendChangePKPVPMode( PVPCONFIRM_MODE, 3 );
			}else
			if( m_OptTabGame.pWndButton7[1]->GetCheck())
			{
					g_DPlay.SendChangePKPVPMode( PVPCONFIRM_MODE, 2 );
			}
			if( m_OptTabGame.pWndButton8[0]->GetCheck())
			{
					g_DPlay.SendQueryEquipSetting(TRUE);
			}else
			if( m_OptTabGame.pWndButton8[1]->IsPush())
			{
					g_DPlay.SendQueryEquipSetting(FALSE);	
			}
			if( m_OptTabGame.pWndButton9[0]->GetCheck())
			{
					g_Option.m_bAutoAttack = 1;
			}else
			if( m_OptTabGame.pWndButton9[1]->GetCheck())
			{
					g_Option.m_bAutoAttack = 0;
			}
			if( m_OptTabGame.pWndHelp->GetCheck())
			    g_Option.m_nInstantHelp = TRUE;
		    else
			    g_Option.m_nInstantHelp = FALSE;

			if( m_OptTabGame.pWndCheck->GetCheck())
			{
			    *g_Option.m_pGuide = 0;
			}
			else
			{
			    CWndGuideSystem* pWndGuide = NULL;
			    pWndGuide = (CWndGuideSystem*)GetWndBase( APP_GUIDE );
			    pWndGuide->m_wndGuideText->SetVisible(FALSE);
			    *g_Option.m_pGuide = 2;
			}

			if( m_OptTabGame.pWndAlpha->GetCheck())
			    CWndBase::m_nAlpha = g_Option.m_nWindowAlpha = 128;
			else
			    CWndBase::m_nAlpha = g_Option.m_nWindowAlpha = 255;

			if( m_OptTabGame.pWndCheckBattleBGM->GetCheck())
			{
			    g_Option.m_bBattleBGM = ( m_OptTabGame.pWndCheckBattleBGM->GetCheck() == TRUE ) ? TRUE: FALSE;
			}
#ifdef __GAME_GRADE_SYSTEM
	       if( m_OptTabGame.pWndCheckGameGrade->GetCheck())
		   {
			    BOOL bGameGradeChecked = m_OptTabGame.pWndCheckGameGrade->GetCheck();
			    g_Option.m_bGameGradeRendering = ( bGameGradeChecked == TRUE ) ? TRUE: FALSE;
			    if( bGameGradeChecked == TRUE )
			    {
				     g_Neuz.m_dwTimeGameGradeMarkRendering = g_tmCurrent + SEC( CNeuzApp::GAME_GRADE_MARK_RENDERING_INTERVAL_SECOND );
			    }
		  }
#endif // __GAME_GRADE_SYSTEM
		    Destroy();//Fecha depois de salvar.
		}
		break;

	    case WIDC_BUT_CANCEL:
		{
		    Destroy();
		}
		break;

	    case WIDC_BUT_RESET:
			{
		      CWndButton *pWndReset = (CWndButton*)GetDlgItem( WIDC_BUT_RESET );

			   if(pWndReset->IsPush())
			   {
		 	       g_Option.Defalt( "neuzDefault.ini" );//carrega o neuzdefault.ini e depois que soutar o botão ele para.

		 	       m_OptTabGame.pWndButton1[0]->SetCheck(TRUE);
		 	       m_OptTabGame.pWndButton2[0]->SetCheck(TRUE);
		 	       m_OptTabGame.pWndButton3[0]->SetCheck(TRUE);
		 	       m_OptTabGame.pWndButton4[0]->SetCheck(TRUE);
		 	       m_OptTabGame.pWndButton5[0]->SetCheck(TRUE);
		 	       m_OptTabGame.pWndButton9[0]->SetCheck(TRUE);
//		 	       m_OptTabGame.pWndCheckGameGrade->SetCheck(TRUE);
		 	       m_OptTabGame.pWndCheckBattleBGM->SetCheck(TRUE);
		 	       m_OptTabGame.pWndHelp->SetCheck(TRUE);

		 	       m_OptTabGame.pWndButton1[1]->SetCheck(FALSE);
		 	       m_OptTabGame.pWndButton2[1]->SetCheck(FALSE);
		 	       m_OptTabGame.pWndButton3[1]->SetCheck(FALSE);
		 	       m_OptTabGame.pWndButton4[1]->SetCheck(FALSE);
		 	       m_OptTabGame.pWndButton5[1]->SetCheck(FALSE);
		 	       m_OptTabGame.pWndButton9[1]->SetCheck(FALSE);
		 	       m_OptTabGame.pWndCheck->SetCheck(FALSE);
		 	       m_OptTabGame.pWndAlpha->SetCheck(FALSE);

			   }
		    }
		break;
#endif//__OPTION_EX
		case 10000:
			{
				CWndGuideSystem* pWndGuide = (CWndGuideSystem*)GetWndBase( APP_GUIDE );
				
				if( pWndGuide )
				{
					pWndGuide->m_dwGuideLevel = *(g_Option.m_pGuide);
					
					if( pWndGuide->m_dwGuideLevel == 0 )
					{
						if( g_pPlayer )
						{
							if(pWndGuide->m_bVisible != false)
							{
								pWndGuide->m_bVisible = true;
								pWndGuide->ChangeModel( g_pPlayer->GetJob() );
								pWndGuide->SetAni( g_pPlayer->GetJob(), CWndGuideSystem::ANI_IDLE );
							}
						}
					}
					else
					{
						if( g_pPlayer )
						{
							pWndGuide->SetAni( g_pPlayer->GetJob(), CWndGuideSystem::ANI_BYTE );
						}
					}
				}
			}
			break;
	}
		
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

CWndOption::CWndOption() 
{ 
} 
CWndOption::~CWndOption() 
{ 
} 
void CWndOption::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndOption::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndButton* pWndAlpha = (CWndButton*)GetDlgItem( WIDC_CHECK1 );
	pWndAlpha->SetCheck( g_Option.m_nWindowAlpha == 255 ? FALSE : TRUE);
	CWndButton* pWndHelp = (CWndButton*)GetDlgItem( WIDC_CHECK2 );
	pWndHelp->SetCheck( g_Option.m_nInstantHelp );
	
	CWndButton* pWndButton[ 4 ];

	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_RADIO1 );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_RADIO2 );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ g_Option.m_nSlangWord ]->SetCheck( TRUE );

	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_RADIO3 );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_RADIO4 );
	pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_RADIO5 );
	pWndButton[ 3 ] = (CWndButton*)GetDlgItem( WIDC_RADIO6 );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ g_Option.m_nChatCommand ]->SetCheck( TRUE );

	// ÀÎÅÍÆäÀÌ½º ¹æ½Ä(±¸¹öÀüÀÌ µðÆúÆ®)
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_RADIO7 );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_RADIO8 );
#ifdef __Y_INTERFACE_VER3
	pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_RADIO12 );
#endif //__Y_INTERFACE_VER3
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ g_Option.m_nInterface ]->SetCheck( TRUE );

	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_RADIO9 );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_RADIO10 );
	pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_RADIO11 );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ *g_Option.m_pGuide ]->SetCheck( TRUE );

	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_CHECK4 );
	if(pWndButton[ 0 ])
	pWndButton[ 0 ]->SetCheck( g_Option.m_bCameraLock );

	// ºñÇà½Ã ·Ñ¸µ È¿°ú.
	CWndButton* pWndRoll = (CWndButton*)GetDlgItem( WIDC_CHECK3 );		
	pWndRoll->SetCheck( g_Option.m_bRollEffect );

	#if __VER >= 11 // __ADD_ZOOMOPT
	CWndButton* pWndZoom = (CWndButton*)GetDlgItem( WIDC_CHECK5 );		
	pWndZoom->SetCheck(!g_Option.m_bZoomLimit);
	#endif
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// Ã³À½ ÀÌ ÇÔ¼ö¸¦ ºÎ¸£¸é À©µµ°¡ ¿­¸°´Ù.
BOOL CWndOption::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy¿¡¼­ ¼³Á¤ÇÑ ¸®¼Ò½º·Î À©µµ¸¦ ¿¬´Ù.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_OPTION_ETC, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  Á÷Á¢ À©µµ¸¦ ¿­¶§ »ç¿ë 
BOOL CWndOption::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndOption::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndOption::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndOption::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndOption::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndOption::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	CWndButton* pWndAlpha = (CWndButton*)GetDlgItem( WIDC_CHECK1 );
	CWndButton* pWndHelp = (CWndButton*)GetDlgItem( WIDC_CHECK2 );
	CWndButton* pWndRoll = (CWndButton*)GetDlgItem( WIDC_CHECK3 );
	CWndButton* pWndCamearaLock = (CWndButton*)GetDlgItem( WIDC_CHECK4 );
#if __VER >= 11 // __ADD_ZOOMOPT
	CWndButton* pWndZoomLimit   = (CWndButton*)GetDlgItem( WIDC_CHECK5 );
#endif

	switch( nID )
	{
	case WIDC_CHECK1: // À©µµ ¹ÝÅõ¸í ¿É¼Ç 
		if( pWndAlpha->GetCheck() )
			CWndBase::m_nAlpha = g_Option.m_nWindowAlpha = 128;
		else
			CWndBase::m_nAlpha = g_Option.m_nWindowAlpha = 255;
		break;
	case WIDC_CHECK2: // ÃÊº¸ÀÚ µµ¿ò¸» ¿É¼Ç 
		if( pWndHelp->GetCheck() )
			g_Option.m_nInstantHelp = TRUE;
		else
			g_Option.m_nInstantHelp = FALSE;
		break;
	case WIDC_CHECK3:	// ºñÇà½Ã ·Ñ¸µ È¿°ú.
		if( pWndRoll->GetCheck() )
			g_Option.m_bRollEffect = TRUE;
		else
			g_Option.m_bRollEffect = FALSE;
		break;
	case WIDC_CHECK4:
		{
			if( pWndCamearaLock->GetCheck() )
				g_Option.m_bCameraLock = TRUE;
			else
				g_Option.m_bCameraLock = FALSE;			
		}
		break;
#if __VER >= 11 // __ADD_ZOOMOPT
	case WIDC_CHECK5:
		{
			if( pWndZoomLimit->GetCheck() )
				g_Option.m_bZoomLimit = FALSE;
			else
				g_Option.m_bZoomLimit = TRUE;			
		}	
		break;
#endif

	case WIDC_RADIO1:
		g_Option.m_nSlangWord = 0;
		break;
	case WIDC_RADIO2:
		g_Option.m_nSlangWord = 1;
		break;
	case WIDC_RADIO3:
		g_Option.m_nChatCommand = 0;
		break;
	case WIDC_RADIO4:
		g_Option.m_nChatCommand = 1;
		break;
	case WIDC_RADIO5:
		g_Option.m_nChatCommand = 2;
		break;
	case WIDC_RADIO6:
		g_Option.m_nChatCommand = 3;
		break;
	case WIDC_RADIO7:	// ÈÙ·Î Ä«¸Þ¶ó / ¿ìÅ¬¸¯ ½ºÅ³ÀÇ °íÀü¹æ½Ä
		g_Option.m_nInterface = 0;
		break;
	case WIDC_RADIO8:	// ¿ìÅ¬¸¯ Ä«¸Þ¶ó / ÁÂÅ¬¸¯+X ½ºÅ³ÀÇ ½Å¹æ½Ä.
		g_Option.m_nInterface = 1;
		break;
#ifdef __Y_INTERFACE_VER3
	case WIDC_RADIO12:
		g_Option.m_nInterface = 2;
		break;
#endif //__Y_INTERFACE_VER3
	case WIDC_RADIO9:
		*g_Option.m_pGuide = 0;
		break;
	case WIDC_RADIO10:
		*g_Option.m_pGuide = 1;
		break;
	case WIDC_RADIO11:
		*g_Option.m_pGuide = 2;
		break;
	case 10000:
		{
			CWndGuideSystem* pWndGuide = (CWndGuideSystem*)GetWndBase( APP_GUIDE );
			
			if( pWndGuide )
			{
				pWndGuide->m_dwGuideLevel = *(g_Option.m_pGuide);
				
				if( pWndGuide->m_dwGuideLevel == 0 )
				{
					if( g_pPlayer )
					{
						pWndGuide->m_bVisible = TRUE;
						pWndGuide->ChangeModel( g_pPlayer->GetJob() );
						pWndGuide->SetAni( g_pPlayer->GetJob(), CWndGuideSystem::ANI_IDLE );
					}
				}
				else
				{
					if(g_pPlayer)
					{
						pWndGuide->SetAni( g_pPlayer->GetJob(), CWndGuideSystem::ANI_BYTE );
					}
				}
			}
		}
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

CWndOptSound::CWndOptSound() 
{ 
	m_bLButtonClick = FALSE;
	m_bLButtonClick2 = FALSE;
	
	m_nStep[0] = 0;
	m_nStep[1] = 0;
} 
CWndOptSound::~CWndOptSound() 
{ 
} 
void CWndOptSound::OnDraw( C2DRender* p2DRender ) 
{ 
	CPoint pt;

	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
	CPoint pt1 = lpWndCtrl->rect.TopLeft();
	CPoint pt2 = lpWndCtrl->rect.BottomRight() - lpWndCtrl->rect.TopLeft();
	p2DRender->RenderFillRect( lpWndCtrl->rect, 0);	
	m_Texture.Render( p2DRender, pt1, pt2 );
	pt = pt1;
	pt.x += GetStepPos( m_nStep[0], lpWndCtrl->rect.right - lpWndCtrl->rect.left, 10 ).x;
	m_TexturePt.Render( p2DRender, pt, CPoint( 16, 16 ) );
	
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );
	pt1 = lpWndCtrl->rect.TopLeft();
	pt2 = lpWndCtrl->rect.BottomRight() - lpWndCtrl->rect.TopLeft();
	m_Texture.Render( p2DRender, pt1, pt2 );
	pt = pt1;
	pt.x += GetStepPos( m_nStep[1], lpWndCtrl->rect.right - lpWndCtrl->rect.left, 10 ).x;
	m_TexturePt.Render( p2DRender, pt, CPoint( 16, 16 ) );

} 
void CWndOptSound::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();

	CWndButton* pWndButton[ 3 ];

	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_VOICEON );
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_VOICEOFF );
	if( ::GetLanguage() == LANG_JAP )
	{
		
		pWndButton[ 1 ]->SetGroup( TRUE );
		pWndButton[ g_Option.m_bVoice ]->SetCheck( TRUE );
	}
	else
	{
		CWndStatic* pWndVoice = (CWndStatic*)GetDlgItem(WIDC_STATIC2);
		pWndVoice->SetVisible(FALSE);
		pWndButton[ 1 ]->SetVisible(FALSE);
		pWndButton[ 0 ]->SetVisible(FALSE);
	}
	m_Texture.LoadTexture( g_Neuz.GetDevice(), MakePath( DIR_THEME, "WndVolumeBar.tga" ), 0xffff00ff, TRUE );
	m_TexturePt.LoadTexture( g_Neuz.GetDevice(), MakePath( DIR_THEME, "ButtSpin.tga" ), 0xffffffff, TRUE );		
#if __VER < 12 // __UPDATE_OPT
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_MSHIGH );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_MSMID );
	pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_MSLOW );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ g_Option.m_MouseSpeed ]->SetCheck( TRUE );
#endif
	m_nStep[0] = (int)( g_Option.m_fEffectVolume * 10 );
	m_nStep[1] = (int)( g_Option.m_fBGMVolume * 10 );

} 
// Ã³À½ ÀÌ ÇÔ¼ö¸¦ ºÎ¸£¸é À©µµ°¡ ¿­¸°´Ù.
BOOL CWndOptSound::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy¿¡¼­ ¼³Á¤ÇÑ ¸®¼Ò½º·Î À©µµ¸¦ ¿¬´Ù.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_OPTEX_SOUND, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  Á÷Á¢ À©µµ¸¦ ¿­¶§ »ç¿ë 
BOOL CWndOptSound::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndOptSound::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndOptSound::OnSize( UINT nType, int cx, int cy )
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndOptSound::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
	PLAYSND(SND_NONE);
	ReleaseCapture();
	m_bLButtonClick = FALSE;
	m_bLButtonClick2 = FALSE;
} 
void CWndOptSound::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
	SetCapture();

	int nResult = 0;
	nResult = GetSliderStep( WIDC_CUSTOM1, m_nStep[0], point );	
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
	nResult = GetSliderStep( WIDC_CUSTOM2, m_nStep[1], point );	
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

} 

void CWndOptSound::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bLButtonClick )
	{
		GetRangeSlider( WIDC_CUSTOM1, m_nStep[0], point );

		g_Option.m_fEffectVolume	= m_nStep[0]*0.1f;
		g_SoundMng.m_nSoundVolume	= (int)( -(1.0f-g_Option.m_fEffectVolume)*5000 );
	}

	if( m_bLButtonClick2 )
	{
		GetRangeSlider( WIDC_CUSTOM2, m_nStep[1], point );

		g_Option.m_fBGMVolume = m_nStep[1]*0.1f;
		SetVolume(g_Option.m_fBGMVolume);
	}
}
BOOL CWndOptSound::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch(nID)
	{
		case WIDC_VOICEON:
			g_Option.m_bVoice = 1;
			break;
		case WIDC_VOICEOFF:
			g_Option.m_bVoice = 0;
			break;
#if __VER < 12 // __UPDATE_OPT
		case WIDC_MSLOW:
			g_Option.m_MouseSpeed = 2;
			break;
		case WIDC_MSMID:
			g_Option.m_MouseSpeed = 1;
			break;
		case WIDC_MSHIGH:
			g_Option.m_MouseSpeed = 0;
			break;
#endif
	};
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

CPoint CWndOptSound::GetStepPos(int nStep, int nWidth, int nDivision)
{
	return nStep * (nWidth / (nDivision+1));
}

int CWndOptSound::GetSliderStep(DWORD dwWndId, int &nStep, CPoint point )
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

void CWndOptSound::GetRangeSlider(DWORD dwWndId, int &nStep, CPoint point)
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

HRESULT CWndOptSound::InvalidateDeviceObjects()
{
#ifdef __YDEBUG
	m_Texture.Invalidate();
	m_TexturePt.Invalidate();
#endif //__YDEBUG
	return CWndNeuz::InvalidateDeviceObjects();
}
HRESULT CWndOptSound::RestoreDeviceObjects()
{
#ifdef __YDEBUG
	m_Texture.SetInvalidate(m_pApp->m_pd3dDevice);
	m_TexturePt.SetInvalidate(m_pApp->m_pd3dDevice);
#endif //__YDEBUG
	return CWndNeuz::RestoreDeviceObjects();
}


CWndOptVideo::CWndOptVideo() 
{ 
	m_bLButtonClick = FALSE;
	m_bLButtonClick2 = FALSE;
	
	m_nStep[0] = 0;
	m_nStep[1] = 0;

#if __VER >= 8 //__Y_GAMMA_CONTROL_8
	m_nStep[2] = 0;
	m_nStep[3] = 0;
	m_nStep[4] = 0;

	m_bLButtonClick3 = FALSE;
	m_bLButtonClick4 = FALSE;
	m_bLButtonClick5 = FALSE;

	memset( m_nBrightTable, 0, sizeof(int) * 11 );
#endif //__Y_GAMMA_CONTROL_8
} 
CWndOptVideo::~CWndOptVideo() 
{ 
	g_Option.Save( "neuz.ini" );	
} 
HRESULT CWndOptVideo::InvalidateDeviceObjects()
{
#ifdef __YDEBUG
	m_Texture.Invalidate();
	m_TexturePt.Invalidate();
#endif //__YDEBUG
	return CWndNeuz::InvalidateDeviceObjects();
}
HRESULT CWndOptVideo::RestoreDeviceObjects()
{
#ifdef __YDEBUG
	m_Texture.SetInvalidate(m_pApp->m_pd3dDevice);
	m_TexturePt.SetInvalidate(m_pApp->m_pd3dDevice);
#endif //__YDEBUG
	return CWndNeuz::RestoreDeviceObjects();
}

CPoint CWndOptVideo::GetStepPos(int nStep, int nWidth, int nDivision)
{
	if(nDivision == 1 && nStep)
		return nStep * nWidth - 16;
	else
		return nStep * (nWidth / (nDivision+1));
}

void CWndOptVideo::OnDraw( C2DRender* p2DRender ) 
{ 
	CPoint pt;
#ifdef __SFX_OPT
	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );
	CPoint pt1 = lpWndCtrl->rect.TopLeft();
	CPoint pt2 = lpWndCtrl->rect.BottomRight() - lpWndCtrl->rect.TopLeft();
	p2DRender->RenderFillRect( lpWndCtrl->rect, 0);	
	m_Texture.Render( p2DRender, pt1, pt2 );
	pt = pt1;
	pt.x += GetStepPos( m_nStep[0], lpWndCtrl->rect.right - lpWndCtrl->rect.left, 5 ).x;
	m_TexturePt.Render( p2DRender, pt, CPoint( 16, 16 ) );
	
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM3 );
	pt1 = lpWndCtrl->rect.TopLeft();
	pt2 = lpWndCtrl->rect.BottomRight() - lpWndCtrl->rect.TopLeft();
	m_Texture.Render( p2DRender, pt1, pt2 );
	pt = pt1;
	pt.x += GetStepPos( m_nStep[1], lpWndCtrl->rect.right - lpWndCtrl->rect.left, 1 ).x;
	m_TexturePt.Render( p2DRender, pt, CPoint( 16, 16 ) );
#else
	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );
	CPoint pt1 = lpWndCtrl->rect.TopLeft();
	CPoint pt2 = lpWndCtrl->rect.BottomRight() - lpWndCtrl->rect.TopLeft();
	p2DRender->RenderFillRect( lpWndCtrl->rect, 0);	
	m_Texture.Render( p2DRender, pt1, pt2 );
	pt = pt1;
	pt.x += GetStepPos( m_nStep[0], lpWndCtrl->rect.right - lpWndCtrl->rect.left, 10 ).x;
	m_TexturePt.Render( p2DRender, pt, CPoint( 16, 16 ) );
	
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM3 );
	pt1 = lpWndCtrl->rect.TopLeft();
	pt2 = lpWndCtrl->rect.BottomRight() - lpWndCtrl->rect.TopLeft();
	m_Texture.Render( p2DRender, pt1, pt2 );
	pt = pt1;
	pt.x += GetStepPos( m_nStep[1], lpWndCtrl->rect.right - lpWndCtrl->rect.left, 10 ).x;
	m_TexturePt.Render( p2DRender, pt, CPoint( 16, 16 ) );
#endif
#if __VER >= 8 //__Y_GAMMA_CONTROL_8
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
	pt1 = lpWndCtrl->rect.TopLeft();
	pt2 = lpWndCtrl->rect.BottomRight() - lpWndCtrl->rect.TopLeft();
	m_Texture.Render( p2DRender, pt1, pt2 );
	pt = pt1;
	pt.x += GetStepPos( m_nStep[2], lpWndCtrl->rect.right - lpWndCtrl->rect.left, 10 ).x;
	//m_TexturePt.Render( p2DRender, pt, CPoint( 16, 16 ) );

	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM4 );
	pt1 = lpWndCtrl->rect.TopLeft();
	pt2 = lpWndCtrl->rect.BottomRight() - lpWndCtrl->rect.TopLeft();
	m_Texture.Render( p2DRender, pt1, pt2 );
	pt = pt1;
	pt.x += GetStepPos( m_nStep[3], lpWndCtrl->rect.right - lpWndCtrl->rect.left, 10 ).x;
	//m_TexturePt.Render( p2DRender, pt, CPoint( 16, 16 ) );

	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM5 );
	pt1 = lpWndCtrl->rect.TopLeft();
	pt2 = lpWndCtrl->rect.BottomRight() - lpWndCtrl->rect.TopLeft();
	m_Texture.Render( p2DRender, pt1, pt2 );
	pt = pt1;
	pt.x += GetStepPos( m_nStep[4], lpWndCtrl->rect.right - lpWndCtrl->rect.left, 10 ).x;
	//m_TexturePt.Render( p2DRender, pt, CPoint( 16, 16 ) );
#endif //__Y_GAMMA_CONTROL_8


} 
void CWndOptVideo::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();

	CWndButton* pWndButton[ 3 ];

	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_OBJECT_LOD_HIGH );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_OBJECT_LOD_MID );
	pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_OBJECT_LOD_LOW );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ g_Option.m_nObjectDetail ]->SetCheck( TRUE );

	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_OBJECT_DISTANT_HIGH );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_OBJECT_DISTANT_MID );
	pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_OBJECT_DISTANT_LOW );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ g_Option.m_nObjectDistant ]->SetCheck( TRUE );

	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_TERRAIN_LOD_HIGH );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_TERRAIN_LOD_MID );
	pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_TERRAIN_LOD_LOW );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ g_Option.m_nViewArea ]->SetCheck( TRUE );

	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_SHADOW_HIGH );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_SHADOW_MID );
	pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_SHADOW_LOW );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ g_Option.m_nShadow ]->SetCheck( TRUE );
	if( g_Neuz.m_d3dCaps.MaxSimultaneousTextures <= 2 ) 
	{
		pWndButton[ 0 ]->EnableWindow( FALSE );
		pWndButton[ 1 ]->EnableWindow( FALSE );
		pWndButton[ 2 ]->EnableWindow( FALSE );
	}
#ifdef __SFX_OPT
	if( ::GetLanguage() == LANG_KOR || ::GetLanguage() == LANG_JAP)
	{
		pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_CHECK4 );
		if(pWndButton[ 0 ])
			pWndButton[ 0 ]->SetVisible(FALSE);
	}
#else
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_VOICEON );
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_VOICEOFF );
	pWndButton[ 1 ]->SetGroup( TRUE );
	pWndButton[ g_Option.m_bVoice ]->SetCheck( TRUE );

	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_GLOWON );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_GLOWOFF );
	pWndButton[ 0 ]->SetGroup( TRUE );
	if( g_Neuz.m_d3dCaps.PixelShaderVersion >= D3DPS_VERSION(1,1) )
	{
		pWndButton[ g_Option.m_nBloom^1 ]->SetCheck( TRUE );
	} else
	{
		pWndButton[0]->EnableWindow( FALSE );
		pWndButton[1]->EnableWindow( FALSE );
	}

	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_CHECK5 );
	pWndButton[ 0 ]->SetCheck(!g_Option.m_bSFXRenderOff);

#endif
#ifdef __YENV
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_SEPCBUMPCHECK );

	if( g_bUsableVS && g_dwUsablePS_Level == 2 )
	{
		pWndButton[0]->EnableWindow( TRUE );
	}
	else
	{
		pWndButton[0]->EnableWindow( FALSE );
	}

	if( g_Option.m_bSpecBump )
	{
		pWndButton[0]->SetCheck( TRUE );
	}
	else
	{
		pWndButton[0]->SetCheck( FALSE );
	}
	if( g_Neuz.m_bFakeFullMode )
	{
		pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_CHECK4 );
		if(pWndButton[ 0 ])
			pWndButton[ 0 ]->SetCheck(!g_Option.m_bStartFullScreen);
	}
#endif //__YENV	
	
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_CHECK6 );
	pWndButton[ 0 ]->SetCheck(g_Option.m_nPlayerName);
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_CHECK7 );
	pWndButton[ 0 ]->SetCheck(g_Option.m_nOtherPlayerName);
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_CHECK8 );
	pWndButton[ 0 ]->SetCheck(g_Option.m_nMonName);	
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_CHECK2 );
	pWndButton[ 0 ]->SetCheck(g_Option.m_bDamageRender);
#if __VER >= 12 // __UPDATE_OPT
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_CHECK1);
	pWndButton[ 0 ]->SetCheck(!g_Option.m_bZoomLimit);
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_VIEWMASK );
	pWndButton[ 0 ]->SetCheck(g_Option.m_bViewMask);
	if(g_pPlayer && g_pPlayer->IsOptionRenderMask() != g_Option.m_bViewMask)
		g_DPlay.SendOptionEnableRenderMask( g_Option.m_bViewMask );
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_CHECK5 );
	if(pWndButton[ 0 ])
	pWndButton[ 0 ]->SetCheck( g_Option.m_bCameraLock );
#else
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_VISIBLEKEEPTIME );
	pWndButton[ 0 ]->SetCheck(g_Option.m_bVisibleBuffTimeRender);	
#endif
	m_Texture.LoadTexture( g_Neuz.GetDevice(), MakePath( DIR_THEME, "WndVolumeBar.tga" ), 0xffff00ff, TRUE );
	m_TexturePt.LoadTexture( g_Neuz.GetDevice(), MakePath( DIR_THEME, "ButtSpin.tga" ), 0xffffffff, TRUE );		
#ifdef __SFX_OPT
	m_nStep[0] = g_Option.m_nSfxLevel;
	m_nStep[1] = !g_Option.m_bSFXRenderOff;
#else
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_MOUSE_HIGH );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_MOUSE_MID );
	pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_MOUSE_LOW );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ g_Option.m_MouseSpeed ]->SetCheck( TRUE );

	m_nStep[0] = g_Option.m_fEffectVolume * 10;
	m_nStep[1] = g_Option.m_fBGMVolume * 10;
#endif

	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_CHECK3 );
	if(pWndButton[ 0 ]) pWndButton[ 0 ]->SetCheck(g_Option.m_nWeatherEffect);

#if __VER <= 8 //__Y_GAMMA_CONTROL_8
	m_nBrightTable[0] = -40;
	m_nBrightTable[1] = -30;
	m_nBrightTable[2] = -20;
	m_nBrightTable[3] = -10;
	m_nBrightTable[4] = 0;
	m_nBrightTable[5] = 10;
	m_nBrightTable[6] = 20;
	m_nBrightTable[7] = 30;
	m_nBrightTable[8] = 40;
	m_nBrightTable[9] = 50;
	m_nBrightTable[10] = 60;

	for( int i=0; i<11; i++ )
	{
		if( m_nBrightTable[i] == g_Option.m_nOverBright )
		{
			m_nStep[3] = i;
			break;
		}
	}

	m_nStep[2] = (int)( g_Option.m_fGamma * 5 );
	m_nStep[4] = (int)( g_Option.m_fContrast * 5 );
#endif //__Y_GAMMA_CONTROL_8
} 
// Ã³À½ ÀÌ ÇÔ¼ö¸¦ ºÎ¸£¸é À©µµ°¡ ¿­¸°´Ù.
BOOL CWndOptVideo::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy¿¡¼­ ¼³Á¤ÇÑ ¸®¼Ò½º·Î À©µµ¸¦ ¿¬´Ù.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_OPTION_VIDEO, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  Á÷Á¢ À©µµ¸¦ ¿­¶§ »ç¿ë 
BOOL CWndOptVideo::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndOptVideo::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndOptVideo::OnSize( UINT nType, int cx, int cy )
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndOptVideo::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
	PLAYSND(SND_NONE);
	ReleaseCapture();
	m_bLButtonClick = FALSE;
	m_bLButtonClick2 = FALSE;
#if __VER >= 8 //__Y_GAMMA_CONTROL_8
	m_bLButtonClick3 = FALSE;
	m_bLButtonClick4 = FALSE;
	m_bLButtonClick5 = FALSE;
#endif //__Y_GAMMA_CONTROL_8
} 
void CWndOptVideo::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 

	SetCapture();

	int nResult = 0;
#ifdef __SFX_OPT
	nResult = GetSliderStep( WIDC_CUSTOM2, m_nStep[0], point, 5);	
	if( nResult == -1 )
	{
		m_bLButtonClick = TRUE;
		return;
	}
	else
	if( nResult == 1 )
	{
		g_Option.m_nSfxLevel = m_nStep[0];
	}
	/////////////////////////////////////////
	nResult = GetSliderStep( WIDC_CUSTOM3, m_nStep[1], point, 1);	
	if( nResult == -1 )
	{
		m_bLButtonClick2 = TRUE;
		return;
	}
	else
	if( nResult == 1 )
	{
		g_Option.m_bSFXRenderOff = !m_nStep[1];
	}
#else
	nResult = GetSliderStep( WIDC_CUSTOM2, m_nStep[0], point );	
	if( nResult == -1 )
	{
		m_bLButtonClick = TRUE;
		return;
	}
	else
	if( nResult == 1 )
	{
		g_Option.m_fEffectVolume = m_nStep[0]*0.1f;
		g_SoundMng.m_nSoundVolume = -(1.0f-g_Option.m_fEffectVolume)*5000;		
	}
	/////////////////////////////////////////
	nResult = GetSliderStep( WIDC_CUSTOM3, m_nStep[1], point );	
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
#if __VER <= 8 //__Y_GAMMA_CONTROL_8
	/////////////////////////////////////////
	nResult = GetSliderStep( WIDC_CUSTOM1, m_nStep[2], point );	
#ifdef __DISABLE_GAMMA_WND
	if( g_Neuz.m_bStartFullscreen )
	{
#endif
		if( nResult == -1 )
		{
			m_bLButtonClick3 = TRUE;
			return;
		}
		else
		if( nResult == 1 )
		{
			g_Option.m_fGamma = (FLOAT)m_nStep[2] / (FLOAT)5.0f;
			g_Neuz.Drv_SetGamma( g_Neuz.GetSafeHwnd(), g_Option.m_fGamma, g_Option.m_nOverBright, g_Option.m_fContrast );
		}
		/////////////////////////////////////////
		nResult = GetSliderStep( WIDC_CUSTOM4, m_nStep[3], point );	
		if( nResult == -1 )
		{
			m_bLButtonClick4 = TRUE;
			return;
		}
		else
		if( nResult == 1 )
		{
			g_Option.m_nOverBright = m_nBrightTable[m_nStep[3]];
			g_Neuz.Drv_SetGamma( g_Neuz.GetSafeHwnd(), g_Option.m_fGamma, g_Option.m_nOverBright, g_Option.m_fContrast );
		}
		/////////////////////////////////////////
		nResult = GetSliderStep( WIDC_CUSTOM5, m_nStep[4], point );	
		if( nResult == -1 )
		{
			m_bLButtonClick5 = TRUE;
			return;
		}
		else
		if( nResult == 1 )
		{
			g_Option.m_fContrast = (FLOAT)m_nStep[4] / (FLOAT)5.0f;
			g_Neuz.Drv_SetGamma( g_Neuz.GetSafeHwnd(), g_Option.m_fGamma, g_Option.m_nOverBright, g_Option.m_fContrast );
		}
#ifdef __DISABLE_GAMMA_WND
	}
#endif
#endif //__Y_GAMMA_CONTROL_8
} 

void CWndOptVideo::OnMouseMove(UINT nFlags, CPoint point)
{
#ifdef __SFX_OPT
	if( m_bLButtonClick )
	{
		GetRangeSlider( WIDC_CUSTOM2, m_nStep[0], point, 5);
		g_Option.m_nSfxLevel = m_nStep[0];
	}

	if( m_bLButtonClick2 )
	{
		GetRangeSlider( WIDC_CUSTOM3, m_nStep[1], point,1);
		g_Option.m_bSFXRenderOff = !m_nStep[1];
	}
#else
	if( m_bLButtonClick )
	{
		GetRangeSlider( WIDC_CUSTOM2, m_nStep[0], point );

		g_Option.m_fEffectVolume = m_nStep[0]*0.1f;
		g_SoundMng.m_nSoundVolume = -(1.0f-g_Option.m_fEffectVolume)*5000;
	}

	if( m_bLButtonClick2 )
	{
		GetRangeSlider( WIDC_CUSTOM3, m_nStep[1], point );

		g_Option.m_fBGMVolume = m_nStep[1]*0.1f;
		SetVolume(g_Option.m_fBGMVolume);
	}
#endif

#if __VER <= 8 //__Y_GAMMA_CONTROL_8
#ifdef __DISABLE_GAMMA_WND
	if(g_Neuz.m_bStartFullscreen)
	{
#endif
		if( m_bLButtonClick3 )
		{
			GetRangeSlider( WIDC_CUSTOM1, m_nStep[2], point );
			g_Option.m_fGamma = (FLOAT)m_nStep[2] / (FLOAT)5.0f;
			g_Neuz.Drv_SetGamma( g_Neuz.GetSafeHwnd(), g_Option.m_fGamma, g_Option.m_nOverBright, g_Option.m_fContrast );
		}
		if( m_bLButtonClick4 )
		{
			GetRangeSlider( WIDC_CUSTOM4, m_nStep[3], point );
			g_Option.m_nOverBright = m_nBrightTable[m_nStep[3]];
			g_Neuz.Drv_SetGamma( g_Neuz.GetSafeHwnd(), g_Option.m_fGamma, g_Option.m_nOverBright, g_Option.m_fContrast );
		}
		if( m_bLButtonClick5 )
		{
			GetRangeSlider( WIDC_CUSTOM5, m_nStep[4], point );
			g_Option.m_fContrast = (FLOAT)m_nStep[4] / (FLOAT)5.0f;
			g_Neuz.Drv_SetGamma( g_Neuz.GetSafeHwnd(), g_Option.m_fGamma, g_Option.m_nOverBright, g_Option.m_fContrast );
		}
#ifdef __DISABLE_GAMMA_WND
	}
#endif
#endif //__Y_GAMMA_CONTROL_8
}

void CWndOptVideo::OnMouseWndSurface( CPoint point )
{
}
BOOL CWndOptVideo::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	CWndButton* pWndButton		= (CWndButton*)*pLResult;
	CWndButton* pWndZoomLimit   = (CWndButton*)GetDlgItem( WIDC_CHECK1 );
#if __VER >= 12 // __UPDATE_OPT
	CWndButton* pWndCamearaLock = (CWndButton*)GetDlgItem( WIDC_CHECK5 );
#endif
	switch( nID )
	{
	case WIDC_OBJECT_LOD_HIGH:
		g_Option.m_nObjectDetail = 0;
		break;
	case WIDC_OBJECT_LOD_MID:
		g_Option.m_nObjectDetail = 1;
		break;
	case WIDC_OBJECT_LOD_LOW:
		g_Option.m_nObjectDetail = 2;
		break;

	case WIDC_OBJECT_DISTANT_HIGH:
		g_Option.m_nObjectDistant = 0;
		break;
	case WIDC_OBJECT_DISTANT_MID:
		g_Option.m_nObjectDistant = 1;
		break;
	case WIDC_OBJECT_DISTANT_LOW:
		g_Option.m_nObjectDistant = 2;
		break;

	case WIDC_TERRAIN_LOD_HIGH:
		g_Option.m_nViewArea = 0;
		SetLODDetail( g_Option.m_nViewArea );
		break;
	case WIDC_TERRAIN_LOD_MID:
		g_Option.m_nViewArea = 1;
		SetLODDetail( g_Option.m_nViewArea );
		break;
	case WIDC_TERRAIN_LOD_LOW:
		g_Option.m_nViewArea = 2;
		SetLODDetail( g_Option.m_nViewArea );
		break;

	case WIDC_SHADOW_HIGH:
		g_Option.m_nShadow = 0;
		CreateShadowMap( m_pApp->m_pd3dDevice, g_Neuz.m_d3dpp.BackBufferFormat );
		break;
	case WIDC_SHADOW_MID:
		g_Option.m_nShadow = 1;
		CreateShadowMap( m_pApp->m_pd3dDevice, g_Neuz.m_d3dpp.BackBufferFormat );
		break;
	case WIDC_SHADOW_LOW:
		g_Option.m_nShadow = 2;
		break;
#ifdef __SFX_OPT
	case WIDC_CHECK4:
		if( g_Neuz.m_bFakeFullMode && ::GetLanguage() != LANG_KOR)
		{
			pWndButton = (CWndButton*)GetDlgItem( WIDC_CHECK4 );
			if(pWndButton)
			{
				g_Option.m_bStartFullScreen = !(pWndButton->GetCheck());
				g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GAME_RESETTING), MB_OK, this );
			}
		}
		break;
#else
	case WIDC_GLOWON:
		if( g_Neuz.m_d3dCaps.PixelShaderVersion >= D3DPS_VERSION(1,1) )
		{
			g_Option.m_nBloom = 1; 
			g_Glare.Create( D3DDEVICE, D3DFMT_R5G6B5, g_Option.m_nResWidth, g_Option.m_nResHeight - 48 );
		}
		break;
	case WIDC_GLOWOFF:
		g_Option.m_nBloom = 0;
		break;
	case WIDC_VOICEON:
		g_Option.m_bVoice = 1;
		break;
	case WIDC_VOICEOFF:
		g_Option.m_bVoice = 0;
		break;
	case WIDC_CHECK5:
		pWndButton = (CWndButton*)GetDlgItem( WIDC_CHECK5 );
		g_Option.m_bSFXRenderOff = !pWndButton->GetCheck();
		break;
	case WIDC_MOUSE_HIGH:
		g_Option.m_MouseSpeed = 0;
		break;
	case WIDC_MOUSE_MID:
		g_Option.m_MouseSpeed = 1;
		break;
	case WIDC_MOUSE_LOW:
		g_Option.m_MouseSpeed = 2;
		break;
	case WIDC_CHECK4:
		if( g_Neuz.m_bFakeFullMode )
		{
			pWndButton = (CWndButton*)GetDlgItem( WIDC_CHECK4 );
			g_Option.m_bStartFullScreen = !(pWndButton->GetCheck());
			g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GAME_RESETTING), MB_OK, this );
		}
		break;
#endif
#ifdef __YENV
	case WIDC_SEPCBUMPCHECK:
		// ¼³Á¤°ªÀ» ¹Ù·Î Àû¿ëÇÏ´Â°ÍÀÌ ¾Æ´Ï¶ó Á¾·áÇÒ¶§ Àû¿ëÇÑ´Ù
		// ¿É¼ÇÁß ´Ù½Ã ½ÃÀÛÇØ¾ß Àû¿ëµÇ´Â°ÍµéÀ» À§ÇÑ º¯¼ö..( ¹üÇÁ¸Ê... )
		pWndButton = (CWndButton*)GetDlgItem( WIDC_SEPCBUMPCHECK );
		if( pWndButton->GetCheck() != g_Option.m_bSpecBump )
		{
			g_Option.m_bExitChange = TRUE;
			g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GAME_RESETTING), MB_OK, this );
		}
		else
			g_Option.m_bExitChange = FALSE;
		break;
#endif //__YENV
	case WIDC_CHECK2:
		pWndButton = (CWndButton*)GetDlgItem( WIDC_CHECK2 );
		g_Option.m_bDamageRender = pWndButton->GetCheck();
		break;

	case WIDC_CHECK6:
		pWndButton = (CWndButton*)GetDlgItem( WIDC_CHECK6 );
		g_Option.m_nPlayerName = pWndButton->GetCheck();
		break;
	case WIDC_CHECK7:
		pWndButton = (CWndButton*)GetDlgItem( WIDC_CHECK7 );
		g_Option.m_nOtherPlayerName = pWndButton->GetCheck();
		break;
	case WIDC_CHECK8:
		pWndButton = (CWndButton*)GetDlgItem( WIDC_CHECK8 );
		g_Option.m_nMonName = pWndButton->GetCheck();
		break;
#if __VER >= 12 // __UPDATE_OPT
	case WIDC_CHECK1:
	{
		if( pWndZoomLimit->GetCheck() )
			g_Option.m_bZoomLimit = FALSE;
		else
			g_Option.m_bZoomLimit = TRUE;			
	}	
	break;
	case WIDC_VIEWMASK:
		pWndButton = (CWndButton*)GetDlgItem( WIDC_VIEWMASK );
		g_Option.m_bViewMask = pWndButton->GetCheck();
		g_DPlay.SendOptionEnableRenderMask( pWndButton->GetCheck() );
		break;
	case WIDC_CHECK5:
		{
			if( pWndCamearaLock->GetCheck() )
				g_Option.m_bCameraLock = TRUE;
			else
				g_Option.m_bCameraLock = FALSE;			
		}
		break;
#else
	case WIDC_VISIBLEKEEPTIME:
		pWndButton = (CWndButton*)GetDlgItem( WIDC_VISIBLEKEEPTIME );
		g_Option.m_bVisibleBuffTimeRender = pWndButton->GetCheck();
		break;
#endif
	case WIDC_CHECK3:
		pWndButton = (CWndButton*)GetDlgItem( WIDC_CHECK3 );
		if(pWndButton) g_Option.m_nWeatherEffect = pWndButton->GetCheck();
		break;
	/*
	case WIDC_EFFECTONOFF:
		{
			CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_EFFECTONOFF);
			g_Option.m_bEffect = pButton->GetCheck();
			pButton->SetCheck( g_Option.m_bEffect );
		}
		break;
	case WIDC_BMGONOFF:
		{
			CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BMGONOFF);
			g_Option.m_bMusic = pButton->GetCheck();
			pButton->SetCheck( g_Option.m_bMusic );

			// À½¾Ç ²ô°í ÄÑ±â
			SetOn( g_Option.m_bMusic );
		}
		break;
	*/
		
/*		
	case WIDC_SOUND_ON:
		g_Option.m_nSoundEffect = g_SoundMng.m_nSoundVolume = 1;
		break;
	case WIDC_SOUND_OFF:
		g_Option.m_nSoundEffect = g_SoundMng.m_nSoundVolume = 0;
		break;

	case WIDC_MUSIC_ON:
		g_Option.m_fMusicVolume = 1.0f;
		SetVolume( 1.0f );
		break;
	case WIDC_MUSIC_OFF:
		g_Option.m_fMusicVolume = 0.0f;
		SetVolume( 0.0f );
		break;
*/
	case WIDC_OK:
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


CWndOptWindow::CWndOptWindow() 
{ 
} 
CWndOptWindow::~CWndOptWindow() 
{ 
} 
void CWndOptWindow::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndOptWindow::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndButton* pWndEffect = (CWndButton*)GetDlgItem( WIDC_CHECK );
	pWndEffect->SetCheck( g_Option.m_nWindowAlpha == 255 ? FALSE : TRUE);

	CWndButton* pWndButton[ 3 ];

	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_RADIO1 );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_RADIO2 );
	pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_RADIO3 );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ g_Option.m_nWindowEffect ]->SetCheck( TRUE );
/*
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_OBJECT_DISTANT_HIGH );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_OBJECT_DISTANT_MID );
	pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_OBJECT_DISTANT_LOW );
	pWndButton[ 0 ]->SetGroup( TRUE );
*/

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// Ã³À½ ÀÌ ÇÔ¼ö¸¦ ºÎ¸£¸é À©µµ°¡ ¿­¸°´Ù.
BOOL CWndOptWindow::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy¿¡¼­ ¼³Á¤ÇÑ ¸®¼Ò½º·Î À©µµ¸¦ ¿¬´Ù.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_OPTION_WINDOW, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  Á÷Á¢ À©µµ¸¦ ¿­¶§ »ç¿ë 
BOOL CWndOptWindow::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndOptWindow::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndOptWindow::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndOptWindow::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndOptWindow::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndOptWindow::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	CWndButton* pWndAlpha = (CWndButton*)GetDlgItem( WIDC_CHECK );
	CWndButton* pWndEffect = (CWndButton*)GetDlgItem( WIDC_RADIO1 );
	switch( nID )
	{
	case WIDC_CHECK:
		if( pWndAlpha->GetCheck() )
			CWndBase::m_nAlpha = g_Option.m_nWindowAlpha = 128;
		else
			CWndBase::m_nAlpha = g_Option.m_nWindowAlpha = 255;
	case WIDC_RADIO1:
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

CWndOptMyInfo::CWndOptMyInfo() 
{ 
} 
CWndOptMyInfo::~CWndOptMyInfo() 
{ 
} 
void CWndOptMyInfo::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndOptMyInfo::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// Ã³À½ ÀÌ ÇÔ¼ö¸¦ ºÎ¸£¸é À©µµ°¡ ¿­¸°´Ù.
BOOL CWndOptMyInfo::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy¿¡¼­ ¼³Á¤ÇÑ ¸®¼Ò½º·Î À©µµ¸¦ ¿¬´Ù.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_OPTION_MYINFO, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  Á÷Á¢ À©µµ¸¦ ¿­¶§ »ç¿ë 
BOOL CWndOptMyInfo::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndOptMyInfo::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndOptMyInfo::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndOptMyInfo::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndOptMyInfo::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndOptMyInfo::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 



/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Å×¸¶
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//EMPTY_WNDCLASSMEMBER( CWndOptVideo, _T( "ºñµð¿À ¿É¼Ç" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Å×¸¶
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//EMPTY_WNDCLASSMEMBER( CWndOptSound, _T( "»ç¿îµå ¿É¼Ç" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Å×¸¶
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//EMPTY_WNDCLASSMEMBER( CWndOptMusic, _T( "¹è°æÀ½¾Ç ¿É¼Ç" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Å×¸¶
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//EMPTY_WNDCLASSMEMBER( CWndOptMouse, _T( "¸¶¿ì½º °¨µµ Á¶Àý" ) )



int CWndOptVideo::GetSliderStep(DWORD dwWndId, int &nStep, CPoint point )
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

int CWndOptVideo::GetSliderStep(DWORD dwWndId, int &nStep, CPoint point, int nDivision)
{
	LPWNDCTRL lpWndCtrl = GetWndCtrl( dwWndId );	
	CRect crect = lpWndCtrl->rect;
	
	crect.left += GetStepPos( nStep, lpWndCtrl->rect.right - lpWndCtrl->rect.left, nDivision ).x;
	crect.right = crect.left+16;
	
	if( PtInRect( crect, point ) )
	{
		return -1;
	}
	
	crect = lpWndCtrl->rect;
	if( PtInRect( crect, point ) )
	{
		int nWidth = lpWndCtrl->rect.right - lpWndCtrl->rect.left;
		
		if( point.x > lpWndCtrl->rect.left+GetStepPos( nStep, nWidth, nDivision ).x )
			nStep += 1;//nWidth / 10;
		else
			nStep -= 1;//nWidth / 10;

		return 1;
	}

	return 0;
}

void CWndOptVideo::GetRangeSlider(DWORD dwWndId, int &nStep, CPoint point, int nDivision)
{
	LPWNDCTRL lpWndCtrl = GetWndCtrl( dwWndId );	
	int nWidth = lpWndCtrl->rect.right - lpWndCtrl->rect.left;
	
	if( point.x > lpWndCtrl->rect.left+GetStepPos( nStep, nWidth, nDivision ).x )
		nStep += 1;//nWidth / 10;
	if( point.x < lpWndCtrl->rect.left+GetStepPos( nStep, nWidth, nDivision ).x )
		nStep -= 1;//nWidth / 10;
	
	if( nStep < 0 )
		nStep = 0;
	
	if( nStep > nDivision )
		nStep = nDivision;
}

void CWndOptVideo::GetRangeSlider(DWORD dwWndId, int &nStep, CPoint point)
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
