#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndManager.h"
#include "WndPetSys.h"

#include "DPClient.h"
extern	CDPClient	g_DPlay;

#if __VER >= 15 // __PETVIS
#include "definesound.h"
#endif

#if __VER >= 12 // __PET_0519
CWndPetAwakCancel::CWndPetAwakCancel() 
{ 

	m_pItemElem  = NULL;
	m_pEItemProp = NULL;
	m_pTexture   = NULL;
	
} 
CWndPetAwakCancel::~CWndPetAwakCancel() 
{ 
} 
void CWndPetAwakCancel::OnDraw( C2DRender* p2DRender ) 
{ 
	ItemProp* pItemProp;
	
	if(m_pItemElem != NULL)
	{
		pItemProp = m_pItemElem->GetProp();
		LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC1 );
		
		if(m_pTexture != NULL)
			m_pTexture->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
	} 
} 
void CWndPetAwakCancel::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);

	if(::GetLanguage() == LANG_FRE)
		pButton->SetTexture(g_Neuz.m_pd3dDevice, MakePath( "Theme\\", ::GetLanguage(), _T( "ButtNormal00.tga" ) ), TRUE);

	pButton->EnableWindow(FALSE);
	m_pText = (CWndText*)GetDlgItem( WIDC_TEXT1 );

	SetDescription();
	
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndPetAwakCancel::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_PET_AWAK_CANCEL, 0, CPoint( 0, 0 ), pWndParent );
} 


void CWndPetAwakCancel::OnDestroy()
{
	if(m_pItemElem != NULL)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pItemElem ) )
			m_pItemElem->SetExtra(0);
	}
}

BOOL CWndPetAwakCancel::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndPetAwakCancel::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndPetAwakCancel::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndPetAwakCancel::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndPetAwakCancel::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BUTTON1 )
	{
		if(m_pItemElem != NULL)
		{
			CWndButton* pButton;
			pButton = (CWndButton*)GetDlgItem( WIDC_BUTTON1 );
			pButton->EnableWindow(FALSE);
			g_DPlay.SendPickupPetAwakeningCancel( m_pItemElem->m_dwObjId );
			Destroy();
		}
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndPetAwakCancel::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	if(!m_pItemElem) return;
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC1 );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{
		m_pItemElem->SetExtra(0);
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
		pButton->EnableWindow(FALSE);
		m_pItemElem = NULL;
		m_pEItemProp = NULL;
		m_pTexture	= NULL;
	}
}

BOOL CWndPetAwakCancel::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	
	CItemElem* pTempElem= NULL;


	if(g_pPlayer != NULL) pTempElem = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
	if( pTempElem != NULL)
	{
		// 픽업펫 확인	// 각성 여부는 서버 검사
		if( !pTempElem->IsEatPet() )	
			return FALSE;
		// 확인 버튼 활성
		if(m_pItemElem) m_pItemElem->SetExtra(0);
		m_pItemElem = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
		m_pEItemProp = m_pItemElem->GetProp();
		m_pItemElem->SetExtra(m_pItemElem->GetExtra()+1);
		if(m_pEItemProp != NULL)
		{
			m_pTexture = m_pItemElem->m_pTexture;
			//m_pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, m_pEItemProp->szIcon), 0xffff00ff );
		}
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
		pButton->EnableWindow(TRUE);
	}

	return TRUE;
}

void CWndPetAwakCancel::SetDescription()
{

	CScript scanner;
	BOOL checkflag;
	CHAR* szChar;


	checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "PetAwakCancel.inc" ) ));
	szChar = scanner.m_pProg;
	
	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString( szChar );
		m_pText->ResetString();	
	}
}

#endif

#if __VER >= 9 // __CSC_VER9_1
//////////////////////////////////////////////////////////////////////////
// CWndPetStatus
//////////////////////////////////////////////////////////////////////////

CWndPetStatus::CWndPetStatus()
{
	m_nDisplay = 2;
	m_nHPWidth = -1;
	m_nEXPWidth = -1;

	m_pVBHPGauge = NULL;
	m_pVBEXPGauge = NULL;
	
	m_bVBHPGauge = TRUE;
	m_bVBEXPGauge = TRUE;

	m_bHPVisible = TRUE;
	m_bExpVisible = TRUE;

	m_pPetElem = NULL;

	m_bExtension = FALSE;
	m_nLockLevel[0] = -1;
	m_nLockLevel[1] = -1;
	m_nBackUpLevel[0] = -1;
	m_nBackUpLevel[1] = -1;
	
	m_strPathLvImage[0] = MakePath( DIR_ICON, "Icon_PetLevel1_S.dds");
	m_strPathLvImage[1] = MakePath( DIR_ICON, "Icon_PetLevel2_S.dds");
	m_strPathLvImage[2] = MakePath( DIR_ICON, "Icon_PetLevel3_S.dds");
	m_strPathLvImage[3] = MakePath( DIR_ICON, "Icon_PetLevel4_S.dds");
	m_strPathLvImage[4] = MakePath( DIR_ICON, "Icon_PetLevel5_S.dds");
	m_strPathLvImage[5] = MakePath( DIR_ICON, "Icon_PetLevel6_S.dds");
	m_strPathLvImage[6] = MakePath( DIR_ICON, "Icon_PetLevel7_S.dds");
	m_strPathLvImage[7] = MakePath( DIR_ICON, "Icon_PetLevel8_S.dds");
	m_strPathLvImage[8] = MakePath( DIR_ICON, "Icon_PetLevel9_S.dds");
	
	m_nChoiceLevel = -1;
	m_pPetModel = NULL;

	for(int i=0; i<6; i++)
		m_pTexPetLv[i] = NULL;
	
	m_pTexPetLvBg = NULL;
	m_pTexPetStatusBg = NULL;
	memset(m_aPetLv, 0, sizeof(int) * 6);
	
	SetPetCamTable();
}

CWndPetStatus::~CWndPetStatus()
{
	DeleteDeviceObjects();
}

void CWndPetStatus::SetPetCamTable()
{
	//백호
	m_PetCameTable[0].CamPos.x = 1.2f;
	m_PetCameTable[0].CamPos.y = 4.6f;
	m_PetCameTable[0].CamPos.z = -7.0f;
	m_PetCameTable[0].CamLook.x = -2.4f;
	m_PetCameTable[0].CamLook.y = 3.0f;
	m_PetCameTable[0].CamLook.z = 0.0f;
	m_PetCameTable[0].Scale = 6.0f;
	//바바리 사자
	m_PetCameTable[1].CamPos.x = 0.7f;
	m_PetCameTable[1].CamPos.y = 3.0f;
	m_PetCameTable[1].CamPos.z = -5.2f;
	m_PetCameTable[1].CamLook.x = -2.2f;
	m_PetCameTable[1].CamLook.y = 2.5f;
	m_PetCameTable[1].CamLook.z = 1.0f;
	m_PetCameTable[1].Scale = 6.0f;
	//토끼
	m_PetCameTable[2].CamPos.x = 2.0f;
	m_PetCameTable[2].CamPos.y = 2.2f;
	m_PetCameTable[2].CamPos.z = -4.0f;
	m_PetCameTable[2].CamLook.x = -2.2f;
	m_PetCameTable[2].CamLook.y = 1.0f;
	m_PetCameTable[2].CamLook.z = 3.0f;
	m_PetCameTable[2].Scale = 7.0f;
	//구미호
	m_PetCameTable[3].CamPos.x = 1.4f;
	m_PetCameTable[3].CamPos.y = 4.2f;
	m_PetCameTable[3].CamPos.z = -8.0f;
	m_PetCameTable[3].CamLook.x = -2.2f;
	m_PetCameTable[3].CamLook.y = 2.0f;
	m_PetCameTable[3].CamLook.z = 3.0f;
	m_PetCameTable[3].Scale = 6.0f;
	//새끼 드래곤
	m_PetCameTable[4].CamPos.x = 1.4f;
	m_PetCameTable[4].CamPos.y = 6.8f;
	m_PetCameTable[4].CamPos.z = -6.0f;
	m_PetCameTable[4].CamLook.x = -2.0f;
	m_PetCameTable[4].CamLook.y = 6.0f;
	m_PetCameTable[4].CamLook.z = 3.0f;
	m_PetCameTable[4].Scale = 5.5f;
	//새끼 그리핀
	m_PetCameTable[5].CamPos.x = 3.0f;
	m_PetCameTable[5].CamPos.y = 6.0f;
	m_PetCameTable[5].CamPos.z = -12.0f;
	m_PetCameTable[5].CamLook.x = -2.0f;
	m_PetCameTable[5].CamLook.y = 3.0f;
	m_PetCameTable[5].CamLook.z = 3.0f;
	m_PetCameTable[5].Scale = 4.5f;
	//유니콘
	m_PetCameTable[6].CamPos.x = 4.0f;
	m_PetCameTable[6].CamPos.y = 3.0f;
	m_PetCameTable[6].CamPos.z = -10.0f;
	m_PetCameTable[6].CamLook.x = -6.0f;
	m_PetCameTable[6].CamLook.y = 6.0f;
	m_PetCameTable[6].CamLook.z = 3.0f;
	m_PetCameTable[6].Scale = 5.0f;
}

void CWndPetStatus::MakeGaugeVertex()
{
	LPWNDCTRL lpHP   = GetWndCtrl( WIDC_CUSTOM2 );
	LPWNDCTRL lpExp  = GetWndCtrl( WIDC_CUSTOM3 );

	if( m_pPetElem != NULL && m_pPetElem->m_pPet != NULL )
	{
		CRect rect = GetClientRect();
		CRect rectTemp;
		
		int nWidthClient = lpHP->rect.Width();
		int nWidth;

		// HP
		nWidth = nWidthClient * m_pPetElem->m_pPet->GetEnergy() / m_pPetElem->m_pPet->GetMaxEnergy();
		if( m_nHPWidth != nWidth ) 
		{
			m_nHPWidth = nWidth;
			rect = lpHP->rect;
			rectTemp = rect; 
			rectTemp.right = rectTemp.left + nWidth;
			ClientToScreen( rectTemp );
#ifdef __STATUS_V19
			m_bVBHPGauge = m_pTheme->MakeGaugeVertex( m_pApp->m_pd3dDevice, &rectTemp, 0xffffffff, m_pVBHPGauge, &m_HPGaugeTexture );
#else
			m_bVBHPGauge = m_pTheme->MakeGaugeVertex( m_pApp->m_pd3dDevice, &rectTemp, 0x64ff0000, m_pVBHPGauge, &m_texGauFillNormal );
#endif// __STATUS_V19
		}

		// Exp
		nWidth	= (int)( ( (float) nWidthClient / m_pPetElem->m_pPet->GetMaxExp() ) * m_pPetElem->m_pPet->GetExp() );
		if( m_nEXPWidth != nWidth ) 
		{
			m_nEXPWidth = nWidth;
			rect = lpExp->rect;
			rectTemp = rect; 
			rectTemp.right = rectTemp.left + nWidth;
			ClientToScreen( rectTemp );
#ifdef __STATUS_V19
			m_bVBEXPGauge = m_pTheme->MakeGaugeVertex( m_pApp->m_pd3dDevice, &rectTemp, 0xffffffff, m_pVBEXPGauge, &m_EXPGaugeTexture );
#else
			m_bVBEXPGauge = m_pTheme->MakeGaugeVertex( m_pApp->m_pd3dDevice, &rectTemp, 0x847070ff, m_pVBEXPGauge, &m_texGauFillNormal );
#endif//__STATUS_V19
		}
	}
}

void CWndPetStatus::PaintFrame( C2DRender* p2DRender )
{
	if(!IsValidObj(g_pPlayer))
		return;

	CRect rect = GetWindowRect();
	
	if( g_pPlayer->HasActivatedSystemPet() )
	{
		m_pPetElem = g_pPlayer->GetPetItem();
		
		if( m_pTexture && m_pPetElem != NULL && m_pPetElem->m_pPet != NULL)
		{
			RenderWnd();
			// 여기는 타이틀 바의 텍스트를 출력하는 곳 
			if( IsWndStyle( WBS_CAPTION ) )	
			{
				int y = 4;
#ifdef __WINDOWS_V19
			CPoint newpoint;
			CSize sizeTitle = p2DRender->m_pFont->GetTextExtent( m_strTitle );
			CRect rect	= GetWindowRect();
			newpoint = (rect.Width() - sizeTitle.cx) / 2;

			CD3DFont* pOldFont = p2DRender->GetFont();
			p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndNewTitle );
            p2DRender->TextOut( newpoint.x , 8, m_strTitle, 0xffffffff );
			p2DRender->SetFont( pOldFont );
#else
				CD3DFont* pOldFont = p2DRender->GetFont();
				p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle );
				p2DRender->TextOut( 10, y, m_strTitle, m_dwColor );
#endif//__WINDOWS_V19
				char szNameLevel[128] = {0,};

				ItemProp* pProp	= m_pPetElem->GetProp();
				CString strName		= pProp->szName;
				if( m_pPetElem->m_pPet->GetLevel() > PL_EGG )
				{
					MoverProp* pMoverProp	= prj.GetMoverProp( m_pPetElem->m_pPet->GetIndex() );
					if( pMoverProp )
						strName		= pMoverProp->szName;
				}
#ifdef __PET_1024
				if( m_pPetElem->m_pPet->HasName() )
					strName		= m_pPetElem->m_pPet->GetName();
#endif	// __PET_1024
				sprintf( szNameLevel, "%s", strName );
				SetTitle( szNameLevel );
				
				p2DRender->SetFont( pOldFont );
			}				
		}
		else
		{
			m_pTheme->RenderWndBaseFrame( p2DRender, &rect );
			if( IsWndStyle( WBS_CAPTION ) )
			{
				// 타이틀 바 
				rect.bottom = 21;
				{
					m_pTheme->RenderWndBaseTitleBar( p2DRender, &rect, m_strTitle, m_dwColor );
				}
			}
		}
	}
}

void CWndPetStatus::DrawPetInformation(C2DRender* p2DRender)
{
	// Draw Bg Text
	p2DRender->TextOut( 74, 4, prj.GetText(TID_GAME_PET_STATUS_LEVEL), D3DCOLOR_ARGB(255, 74, 74, 173), D3DCOLOR_ARGB(255, 160, 160, 240));
	p2DRender->TextOut( 74, 18, prj.GetText(TID_GAME_PET_STATUS_ABILITY), D3DCOLOR_ARGB(255, 74, 74, 173), D3DCOLOR_ARGB(255, 160, 160, 240));
	p2DRender->TextOut( 74, 46, prj.GetText(TID_GAME_PET_STATUS_HP), D3DCOLOR_ARGB(255, 74, 74, 173), D3DCOLOR_ARGB(255, 160, 160, 240));
	p2DRender->TextOut( 74, 61, prj.GetText(TID_GAME_PET_STATUS_EXP), D3DCOLOR_ARGB(255, 74, 74, 173), D3DCOLOR_ARGB(255, 160, 160, 240));

	DWORD dwColor = D3DCOLOR_ARGB(255, 255, 28, 174);
	
	if(m_pPetElem->m_pPet)
	{
		int nLevel = m_pPetElem->m_pPet->GetLevel();
		CString strTemp;
		DWORD dwLevelText;
		switch(nLevel) 
		{
			case PL_EGG:
				dwLevelText = TID_GAME_PETGRADE_E;
				break;
			case PL_D:
				dwLevelText = TID_GAME_PETGRADE_D;
				break;
			case PL_C:
				dwLevelText = TID_GAME_PETGRADE_C;
				break;
			case PL_B:
				dwLevelText = TID_GAME_PETGRADE_B;
				break;
			case PL_A:
				dwLevelText = TID_GAME_PETGRADE_A;
				break;
			case PL_S:
				dwLevelText = TID_GAME_PETGRADE_S;
				break;
		}
		strTemp.Format( "%s", prj.GetText(dwLevelText) );
		p2DRender->TextOut( 112, 4, strTemp, dwColor);
		
		DWORD dwDstParam;
		int nParam;
		DWORD dwTooltip;
		m_pPetElem->m_pPet->GetAvailDestParam(dwDstParam, nParam);
		
		switch(dwDstParam) 
		{
			case DST_STR:
				dwTooltip = TID_TOOLTIP_STR;
				break;
			case DST_DEX:
				dwTooltip = TID_TOOLTIP_DEX;
				break;
			case DST_INT:
				dwTooltip = TID_TOOLTIP_INT;
				break;
			case DST_STA:
				dwTooltip = TID_TOOLTIP_STA;
				break;
			case DST_ATKPOWER:
				dwTooltip = TID_TOOLTIP_ATKPOWER_VALUE;
				break;
			case DST_ADJDEF:
				dwTooltip = TID_TOOLTIP_DEFENCE;
				break;
			case DST_HP_MAX:
				dwTooltip = TID_TOOLTIP_HP;
				break;
		}
		
		CSize size = g_Neuz.m_2DRender.m_pFont->GetTextExtent( prj.GetText(TID_GAME_PET_STATUS_ABILITY) );

		if(dwDstParam != 0 && nParam != 0)
			strTemp.Format("%s +%d", prj.GetText(dwTooltip), nParam);
		else
			strTemp.Format("%s", prj.GetText(TID_GAME_PET_EGG_ABILITY));

		p2DRender->TextOut( 76 + size.cx, 18, strTemp, dwColor); //'Ability'자간이 넓어 겹치는 국가 발생하여 위치 조정
		
		int nLife = m_pPetElem->m_pPet->GetLife();
		strTemp.Format("%d", nLife);
		p2DRender->TextOut( 100, 32, strTemp, dwColor);
		
		if(m_bExtension)
		{
			//Draw Level Background Image
			CPoint point;
			
			point = CPoint( GetWndCtrl( WIDC_CUSTOM4 )->rect.left-2, GetWndCtrl( WIDC_CUSTOM4 )->rect.top-2 );
			
			if(m_pTexPetLvBg != NULL)
				m_pTexPetLvBg->Render( p2DRender,  point );

			//Draw Level Image
			dwColor = D3DCOLOR_ARGB(255, 0, 0, 0);

			for(int i=PL_D; i<=nLevel; i++)
			{
				BYTE bLevel = m_pPetElem->m_pPet->GetAvailLevel(i);
				if(bLevel != 0)
				{
					if(m_aPetLv[i] != bLevel)
					{
						CString strPath;
						if(m_nLockLevel[0] == i)
							strPath = m_strPathLvImage[m_nBackUpLevel[0]-1];
						else if(m_nLockLevel[1] == i)
							strPath = m_strPathLvImage[m_nBackUpLevel[1]-1];
						else
							strPath = m_strPathLvImage[bLevel-1];
						
						m_pTexPetLv[i] = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, strPath, 0xffff00ff );
						m_aPetLv[i] = bLevel;
					}
					if(m_pTexPetLv[i] != NULL)
					{
						if(m_nChoiceLevel == i)
							m_pTexPetLv[i]->Render( p2DRender,  point );
						else
							m_pTexPetLv[i]->Render( p2DRender,  point, 120 );
					}

					point.x += 35;					
				}
			}
		}
	}
}

void CWndPetStatus::OnDraw(C2DRender* p2DRender)
{
	if( g_pPlayer == NULL || m_pPetElem == NULL || m_pPetElem->m_pPet == NULL )
		return;

	//Draw Status Background Image
	CPoint point;
				
	point = CPoint( GetWndCtrl( WIDC_CUSTOM1 )->rect.left-12, GetWndCtrl( WIDC_CUSTOM1 )->rect.top-6 );
	
	if(m_pTexPetStatusBg != NULL)
		m_pTexPetStatusBg->Render( p2DRender,  point );
	
	CRect rect = GetClientRect();
	int nWidthClient = GetClientRect().Width() - 110;

	CRect rectTemp;

	LPWNDCTRL lpFace = GetWndCtrl( WIDC_CUSTOM1 );
	LPWNDCTRL lpHP   = GetWndCtrl( WIDC_CUSTOM2 );
	LPWNDCTRL lpExp  = GetWndCtrl( WIDC_CUSTOM3 );
	
	MakeGaugeVertex();

	if( m_bVBHPGauge )
	{
		int nMax = m_pPetElem->m_pPet->GetMaxEnergy();
		int nHitPercent = MulDiv( m_pPetElem->m_pPet->GetEnergy(), 100, nMax );

#if __VER < 9 // __S_9_ADD
		if( nHitPercent < CRITICAL_BERSERK_HP )
			m_bHPVisible = !m_bHPVisible;
		else
#endif // __S_9_ADD
			m_bHPVisible = TRUE;

		if( m_bHPVisible )
		{
#ifdef __STATUS_V19
			m_pTheme->RenderGauge( p2DRender->m_pd3dDevice, m_pVBHPGauge, &m_HPGaugeTexture );
#else
			m_pTheme->RenderGauge( p2DRender->m_pd3dDevice, m_pVBHPGauge, &m_texGauFillNormal );
#endif//__STATUS_V19
		}
	}
	if( m_bVBEXPGauge )
	{
#ifdef __STATUS_V19
		m_pTheme->RenderGauge( p2DRender->m_pd3dDevice, m_pVBEXPGauge, &m_EXPGaugeTexture );
#else
		m_pTheme->RenderGauge( p2DRender->m_pd3dDevice, m_pVBEXPGauge, &m_texGauFillNormal );
#endif//__STATUS_V19
	}
	
	//Pet Information
	DrawPetInformation(p2DRender);
	
	if( m_nDisplay != 0 )
	{
		DWORD dwColor = D3DCOLOR_ARGB(255, 255, 255, 255);

		char cbufHp[16] = {0,};
		char cbufExp[16] = {0,};
		
		int nCharHP, nCharEXP;

		if(m_nDisplay == 1)
		{
			int nMax = m_pPetElem->m_pPet->GetMaxEnergy();
			int nHitPercent = MulDiv( m_pPetElem->m_pPet->GetEnergy(), 100, nMax );

			wsprintf(cbufHp, "%d%%", nHitPercent);
			p2DRender->TextOut( lpHP->rect.right - 70, lpHP->rect.top - 0, cbufHp, dwColor, 0xff000000 );

			nCharHP = wsprintf(cbufHp, "%dp", m_pPetElem->m_pPet->GetEnergy());

			int x = lpHP->rect.right-7;
			p2DRender->TextOut( x - (int)(nCharHP*5.8f) , lpHP->rect.top - 0, cbufHp, dwColor, 0xff000000 );
		}
		else
		{
			nCharHP = wsprintf(cbufHp, "%d", m_pPetElem->m_pPet->GetEnergy());

			int x = lpHP->rect.right - 76 + 15;
			p2DRender->TextOut( x - (int)(nCharHP*2.6f), lpHP->rect.top - 0, cbufHp, dwColor, 0xff000000 );
			
			nCharHP = wsprintf(cbufHp, "%d", m_pPetElem->m_pPet->GetMaxEnergy());
			
			x = lpHP->rect.right - 38 + 15;
			p2DRender->TextOut( x - nCharHP*2, lpHP->rect.top - 0, cbufHp, dwColor, 0xff000000 );
			
			p2DRender->TextOut( lpHP->rect.right - 42, lpHP->rect.top - 0, "/", dwColor, 0xff000000 );
		}

		EXPINTEGER	nExpResult = m_pPetElem->m_pPet->GetExp() * (EXPINTEGER)10000 / m_pPetElem->m_pPet->GetMaxExp();
		float fExp = (float)nExpResult / 100.0f;

		if( fExp >= 99.99f )
			nCharEXP = sprintf( cbufExp, "99.99%%" );		// sprintf함수 내부에서 반올림되어 100.00으로 표시되는 것을 막기 위해서 
		else
			nCharEXP = sprintf( cbufExp, "%.2f%%", fExp );

		int x = lpHP->rect.right-7; // -40
		p2DRender->TextOut( x - (int)(nCharEXP*5.8f), lpExp->rect.top - 0, cbufExp, dwColor, 0xff000000 );
	}


	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;
	pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xffa08080, 1.0f, 0 ) ;

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );		
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );		

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

	// 뷰포트 세팅 
	D3DVIEWPORT9 viewport;

	viewport.X      = p2DRender->m_ptOrigin.x + lpFace->rect.left;
	viewport.Y      = p2DRender->m_ptOrigin.y + lpFace->rect.top;
	viewport.Width  = lpFace->rect.Width();
	viewport.Height = lpFace->rect.Height();
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;

	pd3dDevice->SetViewport(&viewport);

	// 프로젝션 
	D3DXMATRIX matProj;
	D3DXMatrixIdentity( &matProj );
	FLOAT fAspect = ((FLOAT)viewport.Width) / (FLOAT)viewport.Height;

	FLOAT fov = D3DX_PI/4.0f;//796.0f;
	FLOAT h = cos(fov/2) / sin(fov/2);
	FLOAT w = h * fAspect;
	D3DXMatrixOrthoLH( &matProj, w, h, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	D3DXMATRIX  matView;

	// 월드 
	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 matScale;
	D3DXMATRIXA16 matRot1, matRot2;
	D3DXMATRIXA16 matTrans;

	// 초기화 
	D3DXMatrixIdentity(&matScale);
	D3DXMatrixIdentity(&matRot1);
	D3DXMatrixIdentity(&matRot2);
	D3DXMatrixIdentity(&matTrans);
	D3DXMatrixIdentity(&matWorld);

	//펫 종류에 따라 설정.
	D3DXVECTOR3 vecPos;
	D3DXVECTOR3 vecLookAt;
	float fScale = 1.0f;
		
	if( m_pPetElem->m_pPet->GetLevel() == PL_EGG )
	{
		vecPos.x = 0.0f;
		vecPos.y = 5.0f;
		vecPos.z = -4.0f;

		vecLookAt.x = 0.0f;
		vecLookAt.y = 0.0f;
		vecLookAt.z = 3.0f;
		fScale = 5.0f;
	}
	else
	{
		int petkind = m_pPetElem->m_pPet->GetKind();
		vecPos = m_PetCameTable[petkind].CamPos;
		vecLookAt = m_PetCameTable[petkind].CamLook;
		fScale = m_PetCameTable[petkind].Scale;
	}

	D3DXMatrixScaling(&matScale, fScale, fScale, fScale);
	
	D3DXMatrixLookAtLH( &matView, &vecPos, &vecLookAt, &D3DXVECTOR3(0.0f,1.0f,0.0f) );
	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
	
	D3DXMatrixMultiply(&matWorld,&matWorld,&matScale);
	D3DXMatrixMultiply(&matWorld, &matWorld,&matRot1);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	// 랜더링 
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, CWorld::m_dwBgColor, 1.0f, 0 ) ;


	//CModelObject* pPetModel	=  g_pPlayer->GetPetModel();
	//if( pPetModel )
	if(m_pPetModel != NULL)
	{
		// Pet LOD가 들어갔기 때문에 LodGroup setting.
		if(g_pPlayer && g_pPlayer->m_pet.GetObj() )
		{
			float	fDist = 50.0f;
			switch( g_Option.m_nObjectDetail )
			{
				case 0 :	fDist = 20.0f;	break;
				case 1 :	fDist = 10.0f;	break;
				case 2 :	fDist = 5.0f;	break;
			}
			int nLevel = (int)( g_pPlayer->m_pet.GetObj()->m_fDistCamera / fDist );
			if( nLevel >= 2 )	nLevel = 2;
			if( nLevel < 0 )	
			{
				//Error( "CMover::Render : %s, lod lv=%d %f", m_szName, nLevel, m_fDistCamera );
				nLevel = 0;
			}
			m_pPetModel->SetGroup( nLevel );
		}
		
		::SetTransformView( matView );
		::SetTransformProj( matProj );

		int nMax = m_pPetElem->m_pPet->GetMaxEnergy();
		int nHitPercent = MulDiv( m_pPetElem->m_pPet->GetEnergy(), 100, nMax );

#if __VER < 9 // __S_9_ADD
		if( nHitPercent < CRITICAL_BERSERK_HP )
		{
			::SetLight( TRUE );
			SetDiffuse( 1.0f, 0.3f, 0.3f );
			D3DXVECTOR3 vDir( 0.0f, 1.0f, 0.0f );
			SetAmbient( 0.55f, 0.55f, 0.55f );
			SetLightVec( vDir );
		}
		else
#endif // __S_9_ADD
		{	
			SetDiffuse( 0.0f, 0.0f, 0.0f );
			SetAmbient( 1.0f, 1.0f, 1.0f );
		}			

		m_pPetModel->SetTextureEx( m_pPetModel->m_pModelElem->m_nTextureEx );
		m_pPetModel->Render(pd3dDevice, &matWorld);

		SetDiffuse( 0.0f, 0.0f, 0.0f );
		SetAmbient( 1.0f, 1.0f, 1.0f );
	}

	
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
}

HRESULT CWndPetStatus::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if( m_pVBHPGauge == NULL )
	{
		m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBHPGauge, NULL );
		m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBEXPGauge, NULL );

		m_nHPWidth = -1;
		m_nEXPWidth = -1;
	}
	return S_OK;
}

HRESULT CWndPetStatus::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
    SAFE_RELEASE( m_pVBHPGauge );
    SAFE_RELEASE( m_pVBEXPGauge );
	SAFE_DELETE( m_pPetModel );
	return S_OK;
}

HRESULT CWndPetStatus::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	return InvalidateDeviceObjects();
}

void CWndPetStatus::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	
	RestoreDeviceObjects();
#ifdef __STATUS_V19
	m_HPGaugeTexture.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "Targetgauge01.tga" ), 0xffff00ff, TRUE );
	m_EXPGaugeTexture.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "BarSky.tga" ), 0xffff00ff, TRUE );
	m_texGauEmptyNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "TargetgaugeBg.tga" ), 0xffff00ff, TRUE );
#endif//__STATUS_V19
	m_texGauFillNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptyNormal.bmp" ), 0xffff00ff, TRUE );

	m_pTexPetLvBg = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "PetLevelBg.tga"), 0xffff00ff );
	m_pTexPetStatusBg = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "PetStatusBg.tga"), 0xffff00ff, TRUE );

	// 장착, 게이지에 나올 캐릭터 오브젝트 설정
	if( g_pPlayer->HasActivatedSystemPet() )
		m_pPetElem	= g_pPlayer->GetPetItem();

	//Size Control
	SetExtension(m_bExtension);

	//Position Control
	CWndStatus* pWndStatus = (CWndStatus*)GetWndBase( APP_STATUS1 );
	if(pWndStatus != NULL)
	{
		CRect rectRoot = pWndStatus->GetWndRect();
		CRect rect = GetWindowRect();
		int x = rectRoot.right;
		int y = rectRoot.top;	
		Move( CPoint(x, y));
	}
	else
	{
		CRect rectRoot = m_pWndRoot->GetLayoutRect();
		CPoint point( rectRoot.left+192, rectRoot.top );
		Move( point );
	}
}

void CWndPetStatus::OnMouseWndSurface(CPoint point)
{
	if(!m_bExtension || m_pPetElem->m_pPet == NULL)
		return;
	
	CRect baseRect = GetWndCtrl(WIDC_CUSTOM4)->rect;
	CRect testRect;
	BOOL bCheckChoice = FALSE;

	testRect = baseRect;
	testRect.right = testRect.left + 34;
	
	int nLevel = m_pPetElem->m_pPet->GetLevel();

	for(int i=PL_D; i<nLevel+1; i++)
	{
		if( testRect.PtInRect(point) )
		{
			CPoint point2 = point;
			ClientToScreen( &point2 );
			ClientToScreen( &testRect );

			DWORD dwDstParam;
			int nParam;
			DWORD dwTooltip;
			CString strTemp;
			CEditString strEdit;
			
			PPETAVAILPARAM pAvailParam = CPetProperty::GetInstance()->GetAvailParam( m_pPetElem->m_pPet->GetKind() );
			dwDstParam	= pAvailParam->dwDstParam;
			nParam = pAvailParam->m_anParam[m_pPetElem->m_pPet->GetAvailLevel(i) - 1];
			
			switch(dwDstParam) 
			{
				case DST_STR:
					dwTooltip = TID_TOOLTIP_STR;
					break;
				case DST_DEX:
					dwTooltip = TID_TOOLTIP_DEX;
					break;
				case DST_INT:
					dwTooltip = TID_TOOLTIP_INT;
					break;
				case DST_STA:
					dwTooltip = TID_TOOLTIP_STA;
					break;
				case DST_ATKPOWER:
					dwTooltip = TID_TOOLTIP_ATKPOWER_VALUE;
					break;
				case DST_ADJDEF:
					dwTooltip = TID_TOOLTIP_DEFENCE;
					break;
				case DST_HP_MAX:
					dwTooltip = TID_TOOLTIP_DST_HP_MAX;
					break;
			}
			strTemp.Format( "%s +%d", prj.GetText(dwTooltip), nParam );
			strEdit.AddString( strTemp, D3DCOLOR_XRGB(255, 0, 0) );

			g_toolTip.PutToolTip(m_pPetElem->m_dwItemId, strEdit, &testRect, point2, 0 );
			bCheckChoice = TRUE;
			m_nChoiceLevel = i;
		}
		testRect.left = testRect.right;
		testRect.right = testRect.left + 34;
	}

	if(!bCheckChoice)
		m_nChoiceLevel = -1;
}

void CWndPetStatus::SetExtension(BOOL eflag)
{
	CRect rect = GetWindowRect(TRUE);

	if(eflag)
		rect.bottom += 48;
	else
		rect.bottom -= 48;
	
	SetWndRect(rect);
}

BOOL CWndPetStatus::Initialize(CWndBase* pWndParent,DWORD dwWndId)
{
	return InitDialog( g_Neuz.GetSafeHwnd(), APP_PET_STATUS );
}

BOOL CWndPetStatus::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

void CWndPetStatus::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize( nType, cx, cy );
}

void CWndPetStatus::SetWndRect( CRect rectWnd, BOOL bOnSize  )
{
	m_nHPWidth = -1;
	m_nEXPWidth = -1;
	
	CWndNeuz::SetWndRect( rectWnd, bOnSize );
}

void CWndPetStatus::OnLButtonUp(UINT nFlags, CPoint point)
{
}

void CWndPetStatus::OnLButtonDown(UINT nFlags, CPoint point)
{	
	if(++m_nDisplay > 2)
		m_nDisplay = 0;
}

BOOL CWndPetStatus::OnEraseBkgnd(C2DRender* p2DRender)
{
	return CWndBase::OnEraseBkgnd( p2DRender );
	CRect rect = GetClientRect();
	p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	p2DRender->RenderFillRect( rect, D3DCOLOR_ARGB( 100, 0, 0, 0 ) );

	return TRUE;
}

BOOL CWndPetStatus::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BUTTON1 )
	{
		m_bExtension = !m_bExtension;
		SetExtension(m_bExtension);
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

BOOL CWndPetStatus::Process()
{
	if(!IsValidObj(g_pPlayer))
		return FALSE;
	
	// 장착, 게이지에 나올 캐릭터 오브젝트 설정
	if( g_pPlayer->HasActivatedSystemPet() )
	{
		m_pPetElem = g_pPlayer->GetPetItem();

		if( m_pPetElem != NULL )
		{
			if( g_pPlayer->GetPetModel() == NULL && m_pPetModel != NULL )
			{
				SAFE_DELETE(m_pPetModel);
			}
			else if( m_pPetElem->m_pPet != NULL && g_pPlayer->GetPetModel() != NULL && m_pPetModel == NULL )
			{
				m_pPetModel = new CModelObject;
				m_pPetModel->InitDeviceObjects( g_Neuz.m_pd3dDevice );
				
				CString textFile;
				textFile.Format("Mvr_%s.chr", g_pPlayer->GetPetModel()->m_pModelElem->m_szName);
				m_pPetModel->LoadBone( textFile );
				textFile.Format("Mvr_%s.o3d", g_pPlayer->GetPetModel()->m_pModelElem->m_szName);
				m_pPetModel->LoadElement( textFile, 0 );

				m_pPetModel->m_pModelElem = g_pPlayer->GetPetModel()->m_pModelElem;

				if( m_pPetElem->m_pPet->GetLevel() == PL_EGG )
					textFile.Format("Mvr_%s_idle.ani", g_pPlayer->GetPetModel()->m_pModelElem->m_szName);
				else
					textFile.Format("Mvr_%s_stand.ani", g_pPlayer->GetPetModel()->m_pModelElem->m_szName);

				m_pPetModel->LoadMotion( textFile );
			}
		}
	}
	
	if(m_pPetModel != NULL)
		m_pPetModel->FrameMove();

	return TRUE;
}

void CWndPetStatus::LockShowLevel(BOOL lFlag, int nLevel, int nPos)
{
	if(lFlag)
	{
		m_nLockLevel[nPos] = nLevel;
		if(m_pPetElem->m_pPet)
			m_nBackUpLevel[nPos] = m_pPetElem->m_pPet->GetAvailLevel(nLevel);
	}
	else
	{
		m_nLockLevel[nPos] = -1;
		m_nBackUpLevel[nPos] = -1;

		CString strPath;
		BYTE bLevel = m_pPetElem->m_pPet->GetAvailLevel(nLevel);
		if(bLevel != 0)
		{
			strPath = m_strPathLvImage[bLevel-1];
			m_pTexPetLv[nLevel] = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, strPath, 0xffff00ff );
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//	CWndFoodConfirm Class
//////////////////////////////////////////////////////////////////////////

CWndFoodConfirm::CWndFoodConfirm()
{ 
	m_pItemElem = NULL;
	m_pEdit = NULL;
	m_nParent = 0;
}

CWndFoodConfirm::CWndFoodConfirm(int nParent)
{ 
	m_pItemElem = NULL;
	m_pEdit = NULL;
	m_nParent = nParent;
} 

CWndFoodConfirm::~CWndFoodConfirm() 
{ 
} 

void CWndFoodConfirm::SetItem(DWORD dwObjId)
{
	m_pItemElem = (CItemElem*)g_pPlayer->GetItemId( dwObjId );
}

void CWndFoodConfirm::OnDraw( C2DRender* p2DRender ) 
{ 
	LPCTSTR szNumber;
	szNumber = m_pEdit->GetString();
	int nNumber = atoi( szNumber );

	if(m_pItemElem == NULL)
		return;

	if( m_pItemElem->m_nItemNum == 1 )
	{
		m_pEdit->SetString( "1" );
	}
	else
	{
		if( nNumber > 1000 )	// 0723
		{
			char szNumberbuf[16] = {0, };
			nNumber = 1000;
			_itoa( 1000, szNumberbuf, 10 );
			m_pEdit->SetString( szNumberbuf );
		}	
		else if( m_pItemElem->m_nItemNum < nNumber )
		{
			char szNumberbuf[16] = {0, };
			nNumber = m_pItemElem->m_nItemNum;
			_itoa( m_pItemElem->m_nItemNum, szNumberbuf, 10 );
			m_pEdit->SetString( szNumberbuf );
		}
		
		for( int i = 0 ; i < 8 ; i++ )
		{
			char szNumberbuf[8] = {0, };
			strncpy( szNumberbuf, szNumber, 8 );
			
			// 0 : 공백, 48 : 숫자 0, 57 : 숫자 9
			if( 47 >= szNumberbuf[i] || szNumberbuf[i] >= 58 )
			{
				if( szNumberbuf[i] != 0 )
				{
					nNumber = m_pItemElem->m_nItemNum;
					_itoa( m_pItemElem->m_nItemNum, szNumberbuf, 10 );
					m_pEdit->SetString( szNumberbuf );
					break;										
				}
			}
		}
	}
} 
void CWndFoodConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	m_pEdit   = (CWndEdit  *)GetDlgItem( WIDC_EDIT1 );
	CWndButton* pWndOk = (CWndButton *)GetDlgItem( WIDC_OK );	
	pWndOk->SetDefault( TRUE );
	m_pEdit->SetFocus();

	if(m_pItemElem == NULL)
	{
		Destroy();
		return;
	}

	if( m_pItemElem->m_nItemNum == 1 )
	{
		m_pEdit->SetString( "1" );
	}
	else
	{
		TCHAR szNumber[ 64 ];
		if( m_pItemElem->m_nItemNum > 1000 )	// 0723
			_itot( 1000, szNumber, 10 );
		else
			_itot( m_pItemElem->m_nItemNum, szNumber, 10 );
		m_pEdit->SetString( szNumber );
	}

	CWndStatic* pStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
	if(m_nParent == 1)
		pStatic->SetTitle(prj.GetText(TID_GAME_PETFOOD_COUNTCONFIRM));
	else if(m_nParent == 2)
		pStatic->SetTitle(prj.GetText(TID_GAME_ITEMCOUNT_QUESTION));

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndFoodConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_PET_ITEM, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndFoodConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndFoodConfirm::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndFoodConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndFoodConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndFoodConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK || message == EN_RETURN )
	{
		short DropNum = 0;

		if(m_pItemElem != NULL)
		{
			if( m_pItemElem->m_nItemNum >= 1 )
			{
				DropNum = atoi( m_pEdit->GetString() );
			}
			if( DropNum != 0 && DropNum <= 1000 )	// 0723
			{
				if(m_nParent == 1) //Pet Food
				{
					//Send to Server...
					#if __VER < 12 // __PET_0519
					g_DPlay.SendUsePetFeed(m_pItemElem->m_dwObjId, DropNum);
					#endif	// __PET_0519
					//PLAYSND( "Feed.wav" );
				}
				else if(m_nParent == 2) //Food Mill
				{
					CWndPetFoodMill* pWndPetFoodMill = (CWndPetFoodMill*)g_WndMng.GetWndBase(APP_PET_FOODMILL);
					if(pWndPetFoodMill != NULL)
						pWndPetFoodMill->SetItemForFeed(m_pItemElem, DropNum);
				}
			}
		}

		Destroy();
	}
	else if( nID == WIDC_CANCEL )
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndFoodConfirm::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();
	if( m_pTexture )
	{
		RenderWnd();
		// 여기는 타이틀 바의 텍스트를 출력하는 곳 
		if( IsWndStyle( WBS_CAPTION ) )	
		{
			int y = 4;
#ifdef __WINDOWS_V19
			CPoint newpoint;
			CSize sizeTitle = p2DRender->m_pFont->GetTextExtent( m_strTitle );
			CRect rect	= GetWindowRect();
			newpoint = (rect.Width() - sizeTitle.cx) / 2;

			CD3DFont* pOldFont = p2DRender->GetFont();
			p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndNewTitle );
            p2DRender->TextOut( newpoint.x , 8, m_strTitle, 0xffffffff );
			p2DRender->SetFont( pOldFont );
#else
			CD3DFont* pOldFont = p2DRender->GetFont();
			p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle );
			p2DRender->TextOut( 10, y, m_strTitle, m_dwColor );
#endif//__WINDOWS_V19	
			char szNameLevel[128] = {0,};
			if(m_nParent == 1)
				sprintf( szNameLevel, "%s", prj.GetText(TID_GAME_PETFOOD_CONFIRM) );
			else if(m_nParent == 2)
				sprintf( szNameLevel, "%s", prj.GetText(TID_GAME_ITEMCOUNT_CONFIRM) );

			SetTitle( szNameLevel );
			
			p2DRender->SetFont( pOldFont );
		}				
	}
	else
	{
		m_pTheme->RenderWndBaseFrame( p2DRender, &rect );
		if( IsWndStyle( WBS_CAPTION ) )
		{
			// 타이틀 바 
			rect.bottom = 21;
			{
				m_pTheme->RenderWndBaseTitleBar( p2DRender, &rect, m_strTitle, m_dwColor );
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//	CWndPetMiracle Class
//////////////////////////////////////////////////////////////////////////
CWndPetMiracle::CWndPetMiracle() 
{ 
	m_nCount[0] = 0;
	m_nDelay[0] = 1;
	m_nStatus[0] = -1;
	m_nCount[1] = 0;
	m_nDelay[1] = 1;
	m_nStatus[1] = -1;

	m_nPreLvCount = -1;
	m_nCurLvCount = -1;
	m_nResPreLevel = -1;
	m_nResCurLevel = -1;
	m_nPetLevel = -1;
	m_dwObjId = -1;

	m_bReciveResult[0] = FALSE;
	m_bReciveResult[1] = FALSE;

	m_bEnd = FALSE;
	m_pText = NULL;
	m_bLocked[0] = FALSE;
	m_bLocked[1] = FALSE;

	m_strPathLvImage[0] = MakePath( DIR_ICON, "Icon_PetLevel1_L.dds");
	m_strPathLvImage[1] = MakePath( DIR_ICON, "Icon_PetLevel2_L.dds");
	m_strPathLvImage[2] = MakePath( DIR_ICON, "Icon_PetLevel3_L.dds");
	m_strPathLvImage[3] = MakePath( DIR_ICON, "Icon_PetLevel4_L.dds");
	m_strPathLvImage[4] = MakePath( DIR_ICON, "Icon_PetLevel5_L.dds");
	m_strPathLvImage[5] = MakePath( DIR_ICON, "Icon_PetLevel6_L.dds");
	m_strPathLvImage[6] = MakePath( DIR_ICON, "Icon_PetLevel7_L.dds");
	m_strPathLvImage[7] = MakePath( DIR_ICON, "Icon_PetLevel8_L.dds");
	m_strPathLvImage[8] = MakePath( DIR_ICON, "Icon_PetLevel9_L.dds");
}
 
CWndPetMiracle::~CWndPetMiracle() 
{	
} 

void CWndPetMiracle::OnDestroy()
{
	CWndPetStatus* pWndStatus = (CWndPetStatus*)g_WndMng.GetWndBase( APP_PET_STATUS );
	if(pWndStatus != NULL && m_bLocked[0])
	{
		pWndStatus->LockShowLevel(FALSE, m_nPetLevel-1, 0);
		m_bLocked[0] = FALSE;
	}
	if(pWndStatus != NULL && m_bLocked[1])
	{
		pWndStatus->LockShowLevel(FALSE, m_nPetLevel, 1);
		m_bLocked[1] = FALSE;
	}	
}

void CWndPetMiracle::OnDraw( C2DRender* p2DRender ) 
{ 
	CTexture* pTexture;
	CString strPath;
	CEditString strEdit;
	
	//레벨 텍스트 그리기.
	strEdit.SetString(m_strPetLevel[0], D3DCOLOR_XRGB( 255, 255, 255 ), ESSTY_BOLD);
	p2DRender->TextOut_EditString(GetWndCtrl( WIDC_STATIC1 )->rect.left + 8, GetWndCtrl( WIDC_STATIC1 )->rect.top + 3, strEdit);
	strEdit.SetString(m_strPetLevel[1], D3DCOLOR_XRGB( 255, 255, 255 ), ESSTY_BOLD);
	p2DRender->TextOut_EditString(GetWndCtrl( WIDC_STATIC2 )->rect.left + 8, GetWndCtrl( WIDC_STATIC2 )->rect.top + 3, strEdit);

	//이전 레벨의 특성치 레벨 이미지 그리기.
	if(m_bReciveResult[0])
		strPath = m_strPathLvImage[m_nPreLvCount];
	else
		strPath = m_strPathLvImage[m_nMiracleLv[0] - 1];
	if(strPath.GetLength() > 0)
	{
		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, strPath, 0xffff00ff );
		if(pTexture != NULL)
			pTexture->Render( p2DRender, CPoint( GetWndCtrl( WIDC_STATIC3 )->rect.left, GetWndCtrl( WIDC_STATIC3 )->rect.top + 4 ) );	
	}
	//현재 레벨의 특성치 레벨 이미지 그리기.
	if(m_bReciveResult[1])
		strPath = m_strPathLvImage[m_nCurLvCount];
	else
		strPath = m_strPathLvImage[m_nMiracleLv[1] - 1];
	
	if(strPath.GetLength() > 0)
	{
		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, strPath, 0xffff00ff );
		if(pTexture != NULL)
			pTexture->Render( p2DRender, CPoint( GetWndCtrl( WIDC_STATIC4 )->rect.left, GetWndCtrl( WIDC_STATIC4 )->rect.top + 4 ) );
	}
}

void CWndPetMiracle::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	// 미국 버튼 이미지 변경
	CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_BUTTON2);
	if(pWndButton)
	{
#ifdef __NO_SUB_LANG
		if(::GetLanguage() == LANG_USA || ::GetLanguage() == LANG_VTN)
#else // __NO_SUB_LANG
		if(::GetLanguage() == LANG_USA || ::GetLanguage() == LANG_VTN)
#endif // __NO_SUB_LANG
			pWndButton->SetTexture( m_pApp->m_pd3dDevice, MakePath( "Theme\\", ::GetLanguage(), "ButChance.bmp" ), 0xffff00ff );
	}

	//B/A/S 급 펫만 해당 기능을 이용할 수 있다.
	if( g_pPlayer->HasActivatedSystemPet() )
	{
		CItemElem* m_pPetElem	= g_pPlayer->GetPetItem();
		m_nPetLevel = m_pPetElem->m_pPet->GetLevel();
		if(m_pPetElem->m_pPet != NULL && (m_nPetLevel == PL_B || m_nPetLevel == PL_A || m_nPetLevel == PL_S))
		{
			switch(m_nPetLevel) 
			{
				case PL_B:
					m_strPetLevel[0] = prj.GetText(TID_GAME_PET_GRADE_C); //"Lv. C"
					m_strPetLevel[1] = prj.GetText(TID_GAME_PET_GRADE_B); //"Lv. B"
					break;
				case PL_A:
					m_strPetLevel[0] = prj.GetText(TID_GAME_PET_GRADE_B); //"Lv. B"
					m_strPetLevel[1] = prj.GetText(TID_GAME_PET_GRADE_A); //"Lv. A"
					break;
				case PL_S:
					m_strPetLevel[0] = prj.GetText(TID_GAME_PET_GRADE_A); //"Lv. A"
					m_strPetLevel[1] = prj.GetText(TID_GAME_PET_GRADE_S); //"Lv. S"
					break;
			}

			m_nMiracleLv[0] = m_pPetElem->m_pPet->GetAvailLevel(m_nPetLevel-1);
			m_nMiracleLv[1] = m_pPetElem->m_pPet->GetAvailLevel(m_nPetLevel);
			m_nPreLvCount = m_nMiracleLv[0] - 1;
			m_nCurLvCount = m_nMiracleLv[1] - 1;

			m_pText = (CWndText *)GetDlgItem( WIDC_TEXT_DESC );
			SetDescription(NULL);

			CWndButton* pButton;
			pButton = (CWndButton*)GetDlgItem( WIDC_OK );
			pButton->EnableWindow(FALSE);
			pButton->SetVisible(FALSE);

			//만약 Status창이 활성화된 상태라면 레벨값이 변하지 않도록 고정을 요청하자.
			CWndPetStatus* pWndStatus = (CWndPetStatus*)g_WndMng.GetWndBase( APP_PET_STATUS );
			if(pWndStatus != NULL)
			{
				pWndStatus->LockShowLevel(TRUE, m_nPetLevel-1, 0);
				pWndStatus->LockShowLevel(TRUE, m_nPetLevel, 1);
				m_bLocked[0] = m_bLocked[1] = TRUE;
			}
		
			MoveParentCenter();
		}
		else
			Destroy();
	}
	else
		Destroy();
} 

BOOL CWndPetMiracle::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_PET_MIRACLE, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndPetMiracle::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if(message == WNM_CLICKED)
	{
		if(nID == WIDC_BUTTON2) 
		{
			CWndButton* pButton;
			pButton = (CWndButton*)GetDlgItem( WIDC_BUTTON2 );
			pButton->EnableWindow(FALSE);

			//Send to Server
			if(m_dwObjId != -1)
				g_DPlay.SendPetTamerMiracle(m_dwObjId);
		}
		else if(nID == WIDC_OK || nID == WIDC_CLOSE)
			Destroy();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndPetMiracle::ReceiveResult(int nPreLevel, int nCurLevel)
{
	m_nResPreLevel = nPreLevel;
	m_nResCurLevel = nCurLevel;
	m_nMiracleLv[0] = m_nResPreLevel;
	m_nMiracleLv[1] = m_nResCurLevel;
	
	m_nStatus[0] = 0;
	m_nStatus[1] = 0;
	m_bReciveResult[0] = TRUE;
	m_bReciveResult[1] = TRUE;
}

void CWndPetMiracle::SetItem(DWORD dwObjId)
{
	m_dwObjId = dwObjId;
}

BOOL CWndPetMiracle::Process()
{
	//Start버튼 누를 경우 컴퓨터의 선택이 회전하도록 함.
	PreLevelImgProcess();
	CurLevelImgProcess();
	return TRUE;
}

void CWndPetMiracle::PreLevelImgProcess()
{
	if(m_nStatus[0] == 0)
	{
		if(m_nCount[0]%4 == 0)
		{
			if(m_nCount[0] > 60)
			{
				m_nCount[0] = 0;
				m_nStatus[0] = 1;
			}
			else
			{
				PLAYSND( "InfOpen.wav" );
				int randnum = rand() % 9;
				if(randnum == m_nPreLvCount)
				{
					randnum++;
					( randnum > 8 ) ? randnum = 0 : randnum;
					
				}
				m_nPreLvCount = randnum;			
			}
		}
		m_nCount[0]++;
	}
	else if(m_nStatus[0] == 1)
	{
		if(m_nCount[0] > m_nDelay[0])
		{
			if(m_nDelay[0] < 10)
			{
				PLAYSND( "InfOpen.wav" );
				m_nDelay[0] += 1;
			}
			else
			{
				PLAYSND( "InfOpen.wav" );					
				//초기화 및 상태창이 열려있을 경우 상태창의 Level Lock을 푼다.
				m_nPreLvCount = m_nResPreLevel;
				m_nStatus[0] = -1;
				m_nDelay[0] = 1;
				m_nCount[0] = 0;
				m_bReciveResult[0] = FALSE;
				m_bEnd = TRUE;
				
				CWndPetStatus* pWndStatus = (CWndPetStatus*)g_WndMng.GetWndBase( APP_PET_STATUS );
				if(pWndStatus != NULL && m_bLocked[0])
				{
					pWndStatus->LockShowLevel(FALSE, m_nPetLevel-1, 0);
					m_bLocked[0] = FALSE;
				}
			}

			int randnum = rand() % 9;
			if(randnum == m_nPreLvCount)
			{
				randnum++;
				( randnum > 8 ) ? randnum = 0 : randnum;
			}
			m_nPreLvCount = randnum;
			m_nCount[0] = 0;
		}
		m_nCount[0]++;
	}
}

void CWndPetMiracle::CurLevelImgProcess()
{
	if(m_nStatus[1] == 0)
	{
		if(m_nCount[1]%4 == 0)
		{
			if(m_nCount[1] > 80)
			{
				m_nCount[1] = 0;
				m_nStatus[1] = 1;
			}
			else
			{
				PLAYSND( "InfOpen.wav" );
				int randnum = rand() % 9;
				if(randnum == m_nCurLvCount)
				{
					randnum++;
					( randnum > 8 ) ? randnum = 0 : randnum;
					
				}
				m_nCurLvCount = randnum;			
			}
		}
		m_nCount[1]++;
	}
	else if(m_nStatus[1] == 1)
	{
		if(m_nCount[1] > m_nDelay[1])
		{
			if(m_nDelay[1] < 20)
			{
				PLAYSND( "InfOpen.wav" );
				m_nDelay[1] += 1;
			}
			else
			{
				PLAYSND( "InfOpen.wav" );					
				//초기화 및 상태창이 열려있을 경우 상태창의 Level Lock을 푼다.
				m_nCurLvCount = m_nResCurLevel;
				m_nStatus[1] = -1;
				m_nDelay[1] = 1;
				m_nCount[1] = 0;
				m_bReciveResult[1] = FALSE;
				m_bEnd = TRUE;
				
				CWndPetStatus* pWndStatus = (CWndPetStatus*)g_WndMng.GetWndBase( APP_PET_STATUS );
				if(pWndStatus != NULL && m_bLocked[1])
				{
					pWndStatus->LockShowLevel(FALSE, m_nPetLevel, 1);
					m_bLocked[1] = FALSE;
				}
				CWndButton* pButton;
				pButton = (CWndButton*)GetDlgItem( WIDC_BUTTON2 );
				pButton->SetVisible(FALSE);

				pButton = (CWndButton*)GetDlgItem( WIDC_OK );
				pButton->SetVisible(TRUE);
				pButton->EnableWindow(TRUE);
			}
			
			int randnum = rand() % 9;
			if(randnum == m_nCurLvCount)
			{
				randnum++;
				( randnum > 8 ) ? randnum = 0 : randnum;
			}
			m_nCurLvCount = randnum;
			m_nCount[1] = 0;
		}
		m_nCount[1]++;
	}
}

void CWndPetMiracle::SetDescription( CHAR* szChar )
{
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "PetMiracle.inc" ) ));
	szChar = scanner.m_pProg;

	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString( szChar );
		m_pText->ResetString();	
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndPetFoodMill Class
////////////////////////////////////////////////////////////////////////////////////////////////////////////
CWndPetFoodMill::CWndPetFoodMill()
{
	m_dwObjId = NULL_ID;
	m_pItemElem = NULL;
	m_pWndFoodConfrim = NULL;
	m_pTexture = NULL;
}

CWndPetFoodMill::~CWndPetFoodMill()
{
}

void CWndPetFoodMill::OnDestroy()
{
	if(m_pItemElem != NULL)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pItemElem ) )
			m_pItemElem->SetExtra(0);
		m_pItemElem = NULL;
	}

	if(m_pWndFoodConfrim != NULL)
		m_pWndFoodConfrim->Destroy();
}

void CWndPetFoodMill::OnDraw(C2DRender* p2DRender)
{
	//Draw Food
	ItemProp* pItemProp;

	if(m_pItemElem != NULL)
	{
		pItemProp = m_pItemElem->GetProp();
		if(pItemProp != NULL)
		{
			CPoint point;
			point.x = GetWndCtrl( WIDC_STATIC2 )->rect.left;
			point.y = GetWndCtrl( WIDC_STATIC2 )->rect.top;
			
			if(m_pTexture != NULL)
				m_pTexture->Render( p2DRender, point );
			
			TCHAR szTemp[ 32 ];
			_stprintf( szTemp, prj.GetText(TID_GAME_KWAIBAWIBO_PRESENT_NUM), m_nItemCount );
			CSize size = m_p2DRender->m_pFont->GetTextExtent( szTemp );
			m_p2DRender->TextOut( point.x + 36 - size.cx, point.y + 48 - size.cy, szTemp, 0xff0000ff );
		}
	}	
}

void CWndPetFoodMill::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	
	CWndStatic* pStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
	pStatic->SetTitle(prj.GetText(TID_GAME_PETFOODMILL_DESC));

#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
	CWndInventory* pWndInventory = (CWndInventory*)g_WndMng.CreateApplet(APP_INVENTORY);
#endif // __IMPROVE_SYSTEM_VER15
	
	MoveParentCenter();
}

BOOL CWndPetFoodMill::Initialize(CWndBase* pWndParent,DWORD dwWndId)
{
	return InitDialog( g_Neuz.GetSafeHwnd(), APP_PET_FOODMILL );
}

BOOL CWndPetFoodMill::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if(nID == WIDC_OK)
	{
		if(m_pItemElem != NULL)
		{
			ItemProp* pItemProp = m_pItemElem->GetProp();
//			if( ( pItemProp->dwParts != NULL_ID || pItemProp->dwItemKind3 == IK3_GEM ) && m_pItemElem->IsCharged() == FALSE && pItemProp->dwCost > 0 )
//			if( pItemProp && (pItemProp->dwItemKind1 == IK1_WEAPON || pItemProp->dwReferStat1 == IK1_ARMOR ||
//				pItemProp->dwItemKind3 == IK3_GEM) && pItemProp->dwCost > 0)
			if( pItemProp->dwItemKind3 == IK3_GEM && m_pItemElem->IsCharged() == FALSE && pItemProp->dwCost > 0 )
			{
				//Send to Server...
				g_DPlay.SendMakePetFeed(m_pItemElem->m_dwObjId, (short)m_nItemCount, m_dwObjId);
			}
			else
				g_WndMng.PutString( prj.GetText(TID_GAME_NOTFOOD), NULL, prj.GetTextColor(TID_GAME_NOTFOOD) );
		}
	}
	else if(nID == WIDC_CANCEL || nID == WTBID_CLOSE)
		Destroy();
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndPetFoodMill::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC2 );
	rect = wndCtrl->rect;
	if( m_pItemElem != NULL && rect.PtInRect( point ) )
	{
		m_pItemElem->SetExtra(0);
		m_pItemElem = NULL;
	}
}

BOOL CWndPetFoodMill::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
#if __VER >= 11
	CWndBase* pWndFrame = pShortcut->m_pFromWnd->GetFrameWnd();
	if( pWndFrame && pWndFrame->GetWndId() == APP_BAG_EX )
	{
		g_WndMng.OpenMessageBox( _T( prj.GetText(TID_GAME_ERROR_FOOD_MILL_POCKET) ) );
		return FALSE;
	}
#endif //__VER >= 11

	CItemElem* pTempElem;
	pTempElem = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
	
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC2 );
	if( wndCtrl->rect.PtInRect( point ) )
	{
		if(pTempElem != NULL)
		{
			if(m_pItemElem != NULL)
			{
				if( !g_pPlayer->m_vtInfo.IsTrading( m_pItemElem ) )
					m_pItemElem->SetExtra(0);
				m_pItemElem = NULL;
			}

			SAFE_DELETE( m_pWndFoodConfrim );
			
			m_pWndFoodConfrim = new CWndFoodConfirm(2);
			m_pWndFoodConfrim->m_pItemElem = pTempElem;
			m_pWndFoodConfrim->Initialize(NULL);
		}
	}

	return TRUE;
}

void CWndPetFoodMill::SetItem(DWORD dwObjId)
{
	m_dwObjId = dwObjId;
}

void CWndPetFoodMill::SetItemForFeed(CItemElem* pItemElem, int nCount)
{
	if(pItemElem != NULL)
	{
		m_pItemElem = pItemElem;
		m_nItemCount = nCount;
		m_pItemElem->SetExtra( nCount );
			
		ItemProp* pItemProp = m_pItemElem->GetProp();
		
		if(pItemProp != NULL)
			m_pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
	}
}

void CWndPetFoodMill::ReceiveResult(int nResult, int nCount)
{
	switch(nResult) 
	{
		case 0:
			g_WndMng.PutString( prj.GetText(TID_GAME_LACKSPACE), NULL, prj.GetTextColor(TID_GAME_LACKSPACE) );
			break;
		case 1:
			CString strtmp;
			strtmp.Format(prj.GetText(TID_GAME_PETFEED_MAKE), nCount);
			g_WndMng.PutString( strtmp, NULL, prj.GetTextColor(TID_GAME_PETFEED_MAKE) );
			if(m_pItemElem != NULL)
			{
				if( !g_pPlayer->m_vtInfo.IsTrading( m_pItemElem ) )
					m_pItemElem->SetExtra(0);
				m_pItemElem = NULL;
			}
			if(m_pTexture != NULL)
				m_pTexture = NULL;
			break;
	}
}

//////////////////////////////////////////////////////////////////////////
//	CWndPetLifeConfirm
//////////////////////////////////////////////////////////////////////////

CWndPetLifeConfirm::CWndPetLifeConfirm() 
{
	m_nId	= -1;
}

CWndPetLifeConfirm::~CWndPetLifeConfirm() 
{ 
} 

void CWndPetLifeConfirm::OnDestroy()
{
}

void CWndPetLifeConfirm::SetItem(int nId)
{
	m_nId	= nId;
}

void CWndPetLifeConfirm::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndPetLifeConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate();
	// 여기에 코딩하세요
	CRect rect = GetClientRect();
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize(60,25);
	
	CRect rect2_1( x - size.cx - 10, y, ( x - size.cx - 10 ) + size.cx, y + size.cy );
	CRect rect2_2( x + 10          , y, ( x + 10           ) + size.cx, y + size.cy );
	
	rect.DeflateRect( 10, 10, 10, 35 );
	m_wndText.AddWndStyle( WBS_VSCROLL );
	m_wndText.Create( WBS_NODRAWFRAME, rect, this, 0 );
	m_strText = prj.GetText( TID_GAME_PET_USELIFE );
	m_wndText.SetString( m_strText, 0xffffffff );//ADEILSON 0xff000000
	m_wndText.ResetString();
	
	m_wndButton1.Create("YES"    , 0, rect2_1, this, IDYES);
	m_wndButton2.Create("NO", 0, rect2_2, this, IDNO);
	m_wndButton1.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtYes.tga" ) );
	m_wndButton2.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtNo.tga" ) );
	m_wndButton1.FitTextureSize();
	m_wndButton2.FitTextureSize();

	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndPetLifeConfirm::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_MESSAGEBOX, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndPetLifeConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndPetLifeConfirm::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndPetLifeConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndPetLifeConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndPetLifeConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == IDYES )
	{
		//Send to Server..
		if(	m_nId != -1 )
			g_DPlay.SendDoUseItem( MAKELONG( 0, m_nId ), NULL_ID, -1, FALSE);
	}
	else if( nID == IDNO )
	{
		//그냥 종료
	}
	
	Destroy();
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

#if __VER >= 12 // __CSC_VER12_5

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndPetTransEggs Class
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndPetTransEggs::CWndPetTransEggs() 
{
	ResetEgg();
}

CWndPetTransEggs::~CWndPetTransEggs() 
{ 
} 

void CWndPetTransEggs::OnDestroy()
{
	for(int i=0; i<MAX_TRANS_EGG; i++)
	{
		if(m_pItemElem[i] != NULL)
			m_pItemElem[i]->SetExtra(0);
	}
}

void CWndPetTransEggs::OnDraw( C2DRender* p2DRender ) 
{
	LPWNDCTRL wndCtrl;
	
	for(int i=0; i<MAX_TRANS_EGG; i++)
	{
		if(m_pItemElem[i] != NULL)
		{
			wndCtrl = GetWndCtrl( m_nCtrlId[i] );
			m_pEggTexture->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
		}
	}
}

void CWndPetTransEggs::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_OK);
	pButton->EnableWindow(FALSE);

	m_pText = (CWndText*)GetDlgItem( WIDC_TEXT1 );

	m_nCtrlId[0] = WIDC_STATIC1;
	m_nCtrlId[1] = WIDC_STATIC2;
	m_nCtrlId[2] = WIDC_STATIC3;
	m_nCtrlId[3] = WIDC_STATIC4;
	m_nCtrlId[4] = WIDC_STATIC5;
	m_nCtrlId[5] = WIDC_STATIC6;
	m_nCtrlId[6] = WIDC_STATIC7;
	m_nCtrlId[7] = WIDC_STATIC8;
	m_nCtrlId[8] = WIDC_STATIC9;
	m_nCtrlId[9] = WIDC_STATIC10;

	ItemProp* pItemProp = prj.GetItemProp( II_PET_EGG );
	if(pItemProp)
		m_pEggTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );

	SetDescription(NULL);

	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndPetTransEggs::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_PET_TRANS_EGGS, 0, CPoint( 0, 0 ), pWndParent );
}

void CWndPetTransEggs::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	CRect rect;

	for(int i=0; i<MAX_TRANS_EGG; i++)
	{
		LPWNDCTRL wndCtrl = GetWndCtrl( m_nCtrlId[i] );

		if(wndCtrl)
		{
			rect = wndCtrl->rect;
			if(rect.PtInRect( point ))
			{
				if(m_pItemElem[i])
				{
					m_pItemElem[i]->SetExtra(0);
					m_pItemElem[i] = NULL;
					CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_OK);
					pButton->EnableWindow(FALSE);
				}
			}
		}
	}
}

void CWndPetTransEggs::OnMouseWndSurface(CPoint point)
{
	CRect rect;
	LPWNDCTRL wndCtrl;

	for(int i=0; i<MAX_TRANS_EGG; i++)
	{
		if(m_pItemElem[i] != NULL)
		{
			wndCtrl = GetWndCtrl( m_nCtrlId[i] );
			rect = wndCtrl->rect;
			if( rect.PtInRect( point ) )
			{
				ClientToScreen( &point );
				ClientToScreen( &rect );
				g_WndMng.PutToolTip_Item( (CItemBase*)m_pItemElem[i], point, &rect );
			}
		}
	}
}

BOOL CWndPetTransEggs::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CRect rect;
	CItemElem* pItemElem;
	pItemElem = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );

	if(g_pPlayer->IsUsing(pItemElem))
	{
		g_WndMng.PutString( prj.GetText( TID_GAME_TRANS_EGGS_ERROR2 ), NULL, prj.GetTextColor( TID_GAME_TRANS_EGGS_ERROR2 ) );
	}
	else
	{

		if( (pItemElem->GetProp()->dwItemKind3 == IK3_EGG && pItemElem->m_pPet == NULL) ||
			(pItemElem->GetProp()->dwItemKind3 == IK3_EGG && pItemElem->m_pPet && pItemElem->m_pPet->GetLevel() == PL_EGG) )
		{
			if( IsUsableItem( pItemElem ) )
			{
				for(int i=0; i<MAX_TRANS_EGG; i++)
				{
					LPWNDCTRL wndCtrl = GetWndCtrl( m_nCtrlId[i] );

					if(wndCtrl)
					{
						rect = wndCtrl->rect;
						if(rect.PtInRect( point ))
						{
							if(m_pItemElem[i] == NULL)
							{
								m_pItemElem[i] = pItemElem;
								m_pItemElem[i]->SetExtra(m_pItemElem[i]->GetExtra()+1);
							}
						}
					}
				}

				CheckFull();
			}
		}
		else
			g_WndMng.PutString( prj.GetText( TID_GAME_TRANS_EGGS_ERROR1 ), NULL, prj.GetTextColor( TID_GAME_TRANS_EGGS_ERROR1 ) );
	}

	return TRUE;
}

void CWndPetTransEggs::SetItem(CItemElem* pItemElem)
{
	int nEmpty = -1;

	for(int i=0; i<MAX_TRANS_EGG; i++)
	{
		if(m_pItemElem[i] == NULL)
		{
			nEmpty = i;
			i = MAX_TRANS_EGG;
		}
	}

	if(nEmpty != -1)
	{
		m_pItemElem[nEmpty] = pItemElem;
		m_pItemElem[nEmpty]->SetExtra(m_pItemElem[nEmpty]->GetExtra()+1);
	}

	CheckFull();
}

BOOL CWndPetTransEggs::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK )
	{
		CTransformStuff stuff( 0 );
		
		for(int i=0; i<MAX_TRANS_EGG; i++)
			stuff.AddComponent( m_pItemElem[i]->m_dwObjId, 1 );

		g_DPlay.SendTransformItem( stuff );
		Destroy();
	}
	else if( nID == WIDC_CANCEL )
	{
		Destroy();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndPetTransEggs::SetDescription( CHAR* szChar )
{
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "PetTransEggs.inc" ) ));
	szChar = scanner.m_pProg;
	
	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString( szChar );
		m_pText->ResetString();
	}
}

void CWndPetTransEggs::ResetEgg()
{
	for(int i=0; i<MAX_TRANS_EGG; i++)
		m_pItemElem[i] = NULL;
}

void CWndPetTransEggs::CheckFull()
{
	BOOL bFull = TRUE;

	for(int i=0; i<MAX_TRANS_EGG; i++)
	{
		if(m_pItemElem[i] == NULL)
		{
			bFull = FALSE;
			i = MAX_TRANS_EGG;
		}
	}

	if(bFull)
	{
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_OK);
		pButton->EnableWindow(TRUE);
	}
}

#endif //__CSC_VER12_5

#endif //__CSC_VER9_1


#if __VER >= 15 // __PETVIS

////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndBuffPetStatus
////////////////////////////////////////////////////////////////////////////////////////////////////
CWndBuffPetStatus::CWndBuffPetStatus( )
{
	m_pPetModel = NULL;
	m_pWndConfirmVis = NULL;

	for( int i = 0; i < MAX_VIS; ++i )
		m_pItemElem[ i ] = NULL;

	m_pTexPetStatusBg = NULL;
	m_fRadius = 0.0f;
}

CWndBuffPetStatus::~CWndBuffPetStatus( )
{
	DeleteDeviceObjects();
}

HRESULT CWndBuffPetStatus::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	return S_OK;
}

HRESULT CWndBuffPetStatus::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	SAFE_DELETE( m_pWndConfirmVis );
	for( int i = 0; i < MAX_VIS; ++i )
	{
		SAFE_DELETE( m_pItemElem[ i ] );
	}
	return S_OK;
}

HRESULT CWndBuffPetStatus::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	return InvalidateDeviceObjects();
}

void CWndBuffPetStatus::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	
	RestoreDeviceObjects();
	
	// 장착, 게이지에 나올 캐릭터 오브젝트 설정
	
	//Position Control
	CWndStatus* pWndStatus = (CWndStatus*)GetWndBase( APP_STATUS1 );
	if(pWndStatus != NULL)
	{
		CRect rectRoot = pWndStatus->GetWndRect();
		CRect rect = GetWindowRect();
		int x = rectRoot.right;
		int y = rectRoot.top;	

		CRect kWRect;
		CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
		if( pWndWorld )
		{
			kWRect = pWndWorld->GetClientRect( );
			if( ( x + rect.Width() ) > kWRect.right )
				x = rectRoot.left - rect.Width();
		}

		Move( CPoint(x, y));
	}
	else
	{
		CRect rectRoot = m_pWndRoot->GetLayoutRect();
		CPoint point( rectRoot.left+192, rectRoot.top );
		Move( point );
	}

	m_nCtrlId[0] = WIDC_BUFFPET_SLOT1;
	m_nCtrlId[1] = WIDC_BUFFPET_SLOT2;
	m_nCtrlId[2] = WIDC_BUFFPET_SLOT3;
	m_nCtrlId[3] = WIDC_BUFFPET_SLOT4;
	m_nCtrlId[4] = WIDC_BUFFPET_SLOT5;
	m_nCtrlId[5] = WIDC_BUFFPET_SLOT6;
	m_nCtrlId[6] = WIDC_BUFFPET_SLOT7;
	m_nCtrlId[7] = WIDC_BUFFPET_SLOT8;
	m_nCtrlId[8] = WIDC_BUFFPET_SLOT9;

	m_pTexPetStatusBg = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "BuffpetStatusBg .tga"), 0xffff00ff, TRUE );
}

BOOL CWndBuffPetStatus::Initialize(CWndBase* pWndParent,DWORD dwWndId)
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_BUFFPET_STATUS, 0, CPoint( 0, 0 ), pWndParent );
}

/*void CWndBuffPetStatus::PaintFrame( C2DRender* p2DRender )
{
	if(!IsValidObj(g_pPlayer))
		return;

	CRect rect = GetWindowRect();
	
	if( g_pPlayer->HasActivatedVisPet() )
	{
		RenderWnd();
		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle );
		p2DRender->TextOut( 10, 4, m_strTitle, m_dwColor );
		p2DRender->SetFont( pOldFont );
	}
}*/

void CWndBuffPetStatus::OnDraw(C2DRender* p2DRender)
{
	if( g_pPlayer == NULL ) 
		return;

	//Draw slots
	DrawSlotItems( p2DRender );

	if( !m_pPetModel )
		return;
	
	CRect rect = GetClientRect();
	int nWidthClient = GetClientRect().Width() - 110;

	CRect rectTemp;

	LPWNDCTRL lpFace = GetWndCtrl( WIDC_CUSTOM1 );

	CPoint point;
	point = CPoint( lpFace->rect.left-12, lpFace->rect.top-22 );
	
	if(m_pTexPetStatusBg != NULL)
		m_pTexPetStatusBg->Render( p2DRender,  point );
		

	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;
	pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xffa08080, 1.0f, 0 ) ;

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );		
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );		

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

//	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
//	pd3dDevice->SetRenderState( D3DRS_AMBIENT,  D3DCOLOR_ARGB( 255, 255,255,255) );

	// 뷰포트 세팅 
	D3DVIEWPORT9 viewport;

	viewport.X      = p2DRender->m_ptOrigin.x + lpFace->rect.left;
	viewport.Y      = p2DRender->m_ptOrigin.y + lpFace->rect.top;
	viewport.Width  = lpFace->rect.Width();
	viewport.Height = lpFace->rect.Height();
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;

	pd3dDevice->SetViewport(&viewport);

	// 프로젝션 
	D3DXMATRIX matProj;
	D3DXMatrixIdentity( &matProj );
	FLOAT fAspect = ((FLOAT)viewport.Width) / (FLOAT)viewport.Height;

	FLOAT fov = D3DX_PI/4.0f;//796.0f;
	FLOAT h = cos(fov/2) / sin(fov/2);
	FLOAT w = h * fAspect;
	D3DXMatrixOrthoLH( &matProj, w, h, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	D3DXMATRIX  matView;

	// 월드 
	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 matScale;
	D3DXMATRIXA16 matRot1, matRot2;
	D3DXMATRIXA16 matTrans;

	// 초기화 
	D3DXMatrixIdentity(&matScale);
	D3DXMatrixIdentity(&matRot1);
	D3DXMatrixIdentity(&matRot2);
	D3DXMatrixIdentity(&matTrans);
	D3DXMatrixIdentity(&matWorld);

	//펫 종류에 따라 설정.
	D3DXVECTOR3 vecPos;
	D3DXVECTOR3 vecLookAt;
	float fScale = 1.0f;

	CMover* pMoverPet = prj.GetMover( g_pPlayer->GetEatPetId( ) );
	if( !pMoverPet )
		return;
	
	CModelObject* pModel = (CModelObject *)pMoverPet->m_pModel;
	CObject3D* pObj3D = pModel->GetObject3D( );
	if( !pObj3D )
		return;

	//CModelObject에는 이벤트 좌표가 없는데 CObject3D는 있고?
	vecPos = pObj3D->m_vEvent[ 0 ];

	// mdldyna.inc에서 스케일을 조정한경우 그에 맞게 보정을 해주는데, 어떤 원리인지 내가 했지만 이상함. 나중에 다시 정확히 잡아볼까?
	float fModelScale = pModel->m_pModelElem->m_fScale;
	if( fModelScale < 1.0f && fModelScale > 0.001f )
		vecPos *= ( fModelScale - fModelScale * (0.5f + ( 1.0f - fModelScale ) * 0.01f ) );	//스케일 변동치가 클수록 
	else if ( fModelScale > 1.0f )
		vecPos *= ( fModelScale  - fModelScale * (0.9f + fModelScale * 0.01f) );

	m_fRadius = pModel->GetRadius( );

	vecPos.x += 0.5f;
	vecPos.y += 1.8f;
	vecPos.z -= ( 3.0f ); //* fRadius );

	vecLookAt.x = -0.24f;
	vecLookAt.y = 0.28f;
	vecLookAt.z = 1.0f;
	fScale = (1 / m_fRadius ) * 1.5f; // ( 2.0f * fRadius );

	D3DXMatrixScaling(&matScale, fScale, fScale, fScale);
	
	D3DXMatrixLookAtLH( &matView, &vecPos, &vecLookAt, &D3DXVECTOR3(0.0f,1.0f,0.0f) );
	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
	
	D3DXMatrixMultiply(&matWorld,&matWorld,&matScale);
	D3DXMatrixMultiply(&matWorld, &matWorld,&matRot1);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	// 랜더링 
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, CWorld::m_dwBgColor, 1.0f, 0 ) ;

	::SetTransformView( matView );
	::SetTransformProj( matProj );

	//gmpbigsun : 윈도 페이스 고정라이트
 	::SetLight( FALSE );
 	::SetFog( FALSE );
 	SetDiffuse( 1.0f, 1.0f, 1.0f );
 	SetAmbient( 1.0f, 1.0f, 1.0f );

	m_pPetModel->SetTextureEx( m_pPetModel->m_pModelElem->m_nTextureEx );
	m_pPetModel->Render(pd3dDevice, &matWorld);

//	SetDiffuse( 0.0f, 0.0f, 0.0f );
//	SetAmbient( 1.0f, 1.0f, 1.0f );
	
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE ); 
}

BOOL CWndBuffPetStatus::Process()
{
	if(!IsValidObj(g_pPlayer))
		return FALSE;

		
	// 장착, 게이지에 나올 캐릭터 오브젝트 설정
	if( g_pPlayer->HasActivatedVisPet( ) )
	{
		CMover* pMyBuffPet = NULL;

 		if( !m_pPetModel )
 		{
 			pMyBuffPet = prj.GetMover( g_pPlayer->GetEatPetId( ) );
 			if( pMyBuffPet )
			{
				lstrcpy( pMyBuffPet->m_szCharacterKey, "MaFl_BuffPet" );
				m_pPetModel = (CModelObject*)pMyBuffPet->GetModel( );		// 버프펫이 생성됐다면 모델공유 

				//버프펫 모델이 준비되면 타이틀 (펫이름) 세팅!
				m_strTitle = pMyBuffPet->GetName( );
			}
			else
 				return TRUE;
 		}
	}

	return TRUE;
}

void CWndBuffPetStatus::UpdateVisState( )
{
	if( g_pPlayer )
		m_cVisStates = g_pPlayer->GetValidVisTable( g_pPlayer->GetVisPetItem( ) );
}


BOOL CWndBuffPetStatus::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CWndBase* pWndFrame	= pShortcut->m_pFromWnd->GetFrameWnd();
	if( !pWndFrame )
	{
		assert( 0 );
		return FALSE;
	}

	int selectedSlot = GetSlotIndexByPoint( point );
	if( selectedSlot < 0  )
		return FALSE;

	if( !IsFreeSlot( selectedSlot ) )
		return FALSE;

	if( APP_INVENTORY == pWndFrame->GetWndId( ) )			// 인벤에서 온 아이템은 조건검사 
	{
		CItemElem* pItem = g_pPlayer->m_Inventory.GetAtId( pShortcut->m_dwId );
		if( !IsUsableItem( pItem ) )
			return FALSE;

		if( !pItem->GetProp( )->IsVis( ) )
		return FALSE;
	}

	if( pWndFrame->GetWndId( ) != APP_INVENTORY )				
	{
		if( APP_BUFFPET_STATUS == pWndFrame->GetWndId( ) )			//같은 창 내에서 이동일 경우 
		{
			int selectedSlot = GetSlotIndexByPoint( point );
			if( selectedSlot < 0  )
				return FALSE;
			
			if( pShortcut->m_dwData == selectedSlot )			//	아이콘을 같은자리에 놓았다.
				return FALSE;

			//스왑요청 
			g_DPlay.SendSwapVis( pShortcut->m_dwData, selectedSlot );
		}
		
		return TRUE;
	}


	//확인창 띄워주고 확인창에서 OK시 SendDoUseItem
	if( APP_INVENTORY == pWndFrame->GetWndId( ) )
		DoModal_ConfirmQuestion( pShortcut->m_dwId, g_pPlayer->GetId(), ((CItemElem*)pShortcut->m_dwData)->m_dwItemId );
	else
		DoModal_ConfirmQuestion( pShortcut->m_dwId, g_pPlayer->GetId(), pShortcut->m_dwIndex );

	return TRUE;
}

BOOL CWndBuffPetStatus::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
}

BOOL CWndBuffPetStatus::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndBuffPetStatus::OnDestroy()
{
	for(int i=0; i<MAX_VIS; i++)
	{
// 		if(m_pItemElem[i] != NULL)
// 			m_pItemElem[i]->SetExtra(0);

		SAFE_DELETE( m_pItemElem[ i ] );
	}
}

//비스는 필요비스의 활성화 여부에 따라 자기의 활성화 여부가 결정되므로
//재귀검사로 Leaf까지 탐색한다. 
//주석화 되있는 부분은 단순히 필요비스가 있다면 모든 필요비스를 찾아서 장착되어있는지 검사한다.
//현재는 서버와 같은 함수를 사용함.

/*BOOL IsEquipedVis( DWORD visIndex )
{
	CItemElem* pPetItem = g_pPlayer->GetVisPetItem( );
	if( !pPetItem )
		return FALSE;

	// gmpbigsun: 비스를 착용하고 있는가?
	for( int i = 0; i < MAX_VIS; ++i )
	{
		DWORD dwIndex = pPetItem->GetPiercingItem( i );

		if( dwIndex == visIndex )
			return TRUE;
	}

	return FALSE;
}

BOOL IsValidIndex( DWORD dwIndex )
{
	return ( 0 != dwIndex && NULL_ID != dwIndex );
}

BOOL GetRequireVis( vector< DWORD >& cNeedVises, DWORD visIndex )
{
	if( !IsValidIndex( visIndex ) )
		return FALSE;

	ItemProp* pProp = prj.GetItemProp( visIndex );
	if( !pProp )
		return FALSE;

	DWORD dwNeeds[2] = { pProp->dwReferTarget1, pProp->dwReferTarget2 };

	BOOL bRequire1 = TRUE;
	BOOL bRequire2 = TRUE;

	cNeedVises.push_back( visIndex );

	if( !IsValidIndex( dwNeeds[ 0 ] ) && !IsValidIndex( dwNeeds[ 1 ] ) )			// 필요비스가 없다면
		return TRUE;

	if( IsValidIndex( dwNeeds[ 0 ] ) )						//필요비스가 있다!
	{
		cNeedVises.push_back( dwNeeds[ 0 ] );
		bRequire1 = GetRequireVis( cNeedVises, dwNeeds[ 0 ] );	//필요비스 저장 
	}
		
	if( !bRequire1 )
		return FALSE;

	if( IsValidIndex( dwNeeds[ 1 ] ) )
	{
		cNeedVises.push_back( dwNeeds[ 1 ] );
		bRequire2 = GetRequireVis( cNeedVises, dwNeeds[ 1 ] );
	}

	if( !bRequire2 )
		return FALSE;

	if( bRequire1 && bRequire2 )
		return TRUE;

	return TRUE;

}

BOOL IsEquipedRequireVis( CItemElem* pPetItem, DWORD visIndex, BOOL bSelfCheck )
{
	//gmpbigsun: 해당 비스의 필요 비스를 모두 착용하고 있고 활성화 되어 있는가?
	
	//algorithm :  필요 비스에 대한 정보를 모은후 모두 장착되어있는지 검사 

	if( !IsValidIndex( visIndex ) )
		return FALSE;

	ItemProp* pProp = prj.GetItemProp( visIndex );
	if( !pProp )
		return FALSE;

	vector< DWORD > cNeedVis;

	if( bSelfCheck )
		cNeedVis.push_back( visIndex );
	
	GetRequireVis( cNeedVis, pProp->dwReferTarget1 );
	GetRequireVis( cNeedVis, pProp->dwReferTarget2 );

	for( vector< DWORD >::iterator iter = cNeedVis.begin(); iter != cNeedVis.end(); ++iter )
	{
		if( !IsEquipedVis( *iter ) )
			return FALSE;
	}	
	

	return TRUE;
}*/

void CWndBuffPetStatus::DrawSlotItems( C2DRender* p2DRender )
{
	if( !g_pPlayer )
		return;

	CItemElem* pItem = g_pPlayer->GetVisPetItem();
	if( !pItem )
		return;

	int nAvailableSlot = pItem->GetPiercingSize( );
	if( nAvailableSlot > MAX_VIS )
		return;

	UpdateVisState( );

	LPWNDCTRL wndCtrl = NULL;
	for( int i=0; i<MAX_VIS; ++i )
	{
		if( i < nAvailableSlot )
		{
			DWORD dwItemIndex = pItem->GetPiercingItem( i );
	
			ItemProp* pProp = prj.GetItemProp( dwItemIndex );
			if( !pProp )
				continue;

			m_pTexture[ i ] = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pProp->szIcon), 0xffff00ff );
			
			if(m_pTexture[ i ] != NULL)
			{
				
				DWORD color = ( m_cVisStates[i] == SUCCSESS_NEEDVIS ? 0xffffffff : 0x4fffffff );

				wndCtrl = GetWndCtrl( m_nCtrlId[i] );
				if( wndCtrl )
				{
					CPoint pt2 = CPoint( wndCtrl->rect.left, wndCtrl->rect.top );
					m_pTexture[ i ]->Render2( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ), color );
				}
			}
		}
		else
		{
			//사용불가능 슬롯 
			wndCtrl = GetWndCtrl( m_nCtrlId[i] );
			CTexture* pTexClosed = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, "Icon_Lock.dds" ), 0xffff00ff );
			if( pTexClosed )
				pTexClosed->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );

		}
	}
}


BOOL CWndBuffPetStatus::DoModal_ConfirmQuestion( DWORD dwItemId, OBJID dwObjid, DWORD dwIndex, int nSlot, CWndConfirmVis::ConfirmVisSection eSection )
{
	SAFE_DELETE( m_pWndConfirmVis );
	m_pWndConfirmVis = new CWndConfirmVis;
	m_pWndConfirmVis->m_dwItemId = dwItemId;
	m_pWndConfirmVis->m_objid = dwObjid;
	m_pWndConfirmVis->m_nSlot = nSlot;
	m_pWndConfirmVis->m_eSection = eSection;
	m_pWndConfirmVis->m_dwItemIndex = dwIndex;
	
	m_pWndConfirmVis->Initialize( this, APP_CONFIRM_ENTER );
		
	return TRUE;
}

int CWndBuffPetStatus::GetSlotIndexByPoint( const CPoint& point )
{
	LPWNDCTRL wndCtrl = NULL;
	CRect rect;
	for( int i = 0; i < MAX_VIS; ++i )
	{
		wndCtrl = GetWndCtrl( m_nCtrlId[i] );		// 슬롯으로 만들어진 윈도우에 대하여 
		rect  = wndCtrl->rect;

		if( rect.PtInRect( point ) )
			return i;		
	}

	return -1;
}

void CWndBuffPetStatus::OnLButtonDown( UINT nFlags, CPoint point )
{
	//해당 슬롯을 판별해서 정보를 shortcut에 넣어준다.

	LPWNDCTRL wndCtrl = NULL;
	CRect rect;

	int selectedSlot = GetSlotIndexByPoint( point );
	if( selectedSlot < 0 )
		return;

	CItemElem* pItem = g_pPlayer->GetVisPetItem();
	if( !pItem )
		return;

	DWORD dwItemIndex = pItem->GetPiercingItem( selectedSlot );		// 클릭을 했으나 비스가 없는 슬롯은 패스 
	if( 0 == dwItemIndex )
		return;
	
	assert( selectedSlot >= 0 && selectedSlot < MAX_VIS );
	assert( m_pTexture[ selectedSlot ] );
	
	m_GlobalShortcut.m_pFromWnd = this;
	m_GlobalShortcut.m_dwShortcut = SHORTCUT_ITEM;
	m_GlobalShortcut.m_dwType     = ITYPE_ITEM;
	m_GlobalShortcut.m_dwId       = 0;
	m_GlobalShortcut.m_dwIndex    = dwItemIndex;
	m_GlobalShortcut.m_pTexture   = m_pTexture[ selectedSlot ];
	m_GlobalShortcut.m_dwData     = selectedSlot;
}

void CWndBuffPetStatus::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	//파괴 
	int selectedSlot = GetSlotIndexByPoint( point );
	if( selectedSlot < 0 )
		return;

	CItemElem* pItem = g_pPlayer->GetVisPetItem();
	if( !pItem )
		return;

	DWORD dwItemIndex = pItem->GetPiercingItem( selectedSlot );		// 클릭을 했으나 비스가 없는 슬롯은 패스 
	if( 0 == dwItemIndex )
		return;

	DoModal_ConfirmQuestion( 0, 0, dwItemIndex, selectedSlot, CWndConfirmVis::CVS_UNEQUIP_VIS );
}

BOOL CWndBuffPetStatus::IsVisItem( DWORD index )
{
	ItemProp* pProp = prj.GetItemProp( index );
	if( !pProp )
		return FALSE;

	return pProp->IsVis();
}

BOOL CWndBuffPetStatus::IsFreeSlot( const int index )
{
	CItemElem* pItem = g_pPlayer->GetVisPetItem();
	if( !pItem )
		return FALSE;

	int nAvailableSlot = pItem->GetPiercingSize( );
	return ( index < nAvailableSlot );
}

void CWndBuffPetStatus::OnMouseWndSurface( CPoint point )
{
	CRect rect;
	LPWNDCTRL wndCtrl = NULL;
	CItemElem* pItem = g_pPlayer->GetVisPetItem( );
	assert( pItem );
	if( pItem->GetPiercingSize( ) == 0 )
		return;

	for(int i=0; i<MAX_VIS; i++)
	{
		wndCtrl = GetWndCtrl( m_nCtrlId[i] );
		rect = wndCtrl->rect;
		if( rect.PtInRect( point ) )
		{
			ClientToScreen( &point );
			ClientToScreen( &rect );

			DWORD dwIndex = pItem->GetPiercingItem( i );
			if(  0 != dwIndex )	
			{
				if( NULL == m_pItemElem[ i ] )
					m_pItemElem[ i ] = new CItemElem;
			
				m_pItemElem[ i ]->m_dwItemId = dwIndex;

				time_t endTime = pItem->GetVisKeepTime( i );
				//DWORD remainTime = endTime - timeGetTime( );
				m_pItemElem[ i ]->m_dwKeepTime = endTime;
			
			}else
			{
				SAFE_DELETE( m_pItemElem[ i ] );
				continue;
			}

			g_WndMng.PutToolTip_Item( (CItemBase*)m_pItemElem[i], point, &rect );
		}

	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//CWndConfirmVis : 비스 꼽고 뺄때 확인창 
//////////////////////////////////////////////////////////////////////////////////////////////////
CWndConfirmVis::CWndConfirmVis() 
{ 
 	m_dwItemId = NULL_ID;
 	m_objid = NULL_ID;
 	m_nSlot = 0;
	m_dwItemIndex = 0;
	m_eSection = CVS_EQUIP_VIS;
} 

CWndConfirmVis::~CWndConfirmVis() 
{ 
	
} 


void CWndConfirmVis::OnDraw( C2DRender* p2DRender ) 
{ 
	
} 

void CWndConfirmVis::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndButton* pOk   = (CWndButton*)GetDlgItem( WIDC_BUTTON1 );
	CWndButton* pCancel   = (CWndButton*)GetDlgItem( WIDC_BUTTON2 );
	CWndEdit* pEdit   = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
#if __VER >= 19
	pOk->SetTitle("Ok");
    pCancel->SetTitle("Cancel");
#endif//__VER >= 19
	AddWndStyle( WBS_MODAL );
	
	pOk->SetDefault( TRUE );

	ItemProp* pProp = prj.GetItemProp( m_dwItemIndex );
	assert( pProp );

	if( !pProp )
		return;

	CWndText* pText = ( CWndText* )GetDlgItem( WIDC_TEXT1 );
	CString strTitle;
	switch( m_eSection )
	{
	case CVS_EQUIP_VIS : strTitle.Format( GETTEXT( TID_GAME_BUFFPET_EQUIP ), pProp->szName ); break;
	case CVS_UNEQUIP_VIS : strTitle.Format( GETTEXT( TID_GAME_BUFFPET_CANCEL ), pProp->szName ); break;
	case CVS_EQUIP_VISKEY : // 비스 슬롯 확장 키 쓸려고 할때 
		{
			MoverProp* pProp = NULL;
			if( g_pPlayer )
			{
				CMover* pPet = prj.GetMover( g_pPlayer->GetEatPetId( ) );
				if( pPet )
					pProp = pPet->GetProp( );
			}

			if( pProp )
				strTitle.Format( GETTEXT( TID_GAME_BUFFPET_KET01 ), pProp->szName );
		}break;

	case CVS_PICKUP_TO_BUFF:
		strTitle = GETTEXT( TID_GAME_PET_TRAN );	// 소환되어 있는 픽업펫 버프펫으로 변환?
		break;

#ifdef __PROTECT_AWAKE
	case ETC_PROTECT_AWAKE:
		strTitle = GETTEXT( TID_GAME_REGARDLESS_USE01 ); //"진짜러 각성보호 쓸래염? 의 아이디 필요";
		break;
#endif //AWAKE_PROTECT
	}

	pText->SetString( strTitle );

	//에디트창을 안보이는곳으로 보내버리고 ENTER받을준비 
	pEdit->Move( -100, -100 );
	pEdit->SetFocus( );
} 

BOOL CWndConfirmVis::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	InitDialog( g_Neuz.GetSafeHwnd(), APP_CONFIRM_ENTER, WBS_KEY, 0, pWndParent );
	MoveParentCenter();
	return TRUE;
}
BOOL CWndConfirmVis::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndConfirmVis::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndConfirmVis::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

void CWndConfirmVis::SendEquipPacket( )
{
	switch( m_eSection )
	{
	case CVS_EQUIP_VIS:			g_DPlay.SendDoUseItem( MAKELONG( ITYPE_ITEM, m_dwItemId ), m_objid ); break;
	case CVS_UNEQUIP_VIS :		g_DPlay.SendRemoveVis( m_nSlot );	break;
	case CVS_EQUIP_VISKEY :		g_DPlay.SendDoUseItem( MAKELONG( ITYPE_ITEM, m_dwItemId ), m_objid ); break;
	case CVS_PICKUP_TO_BUFF :	g_DPlay.SendDoUseItem( MAKELONG( ITYPE_ITEM, m_dwItemId ), m_objid ); break;

#ifdef __PROTECT_AWAKE
	case ETC_PROTECT_AWAKE: g_DPlay.SendDoUseItem( MAKELONG( ITYPE_ITEM, m_dwItemId ), m_objid ); break;
#endif //__PROTECT_AWAKE

	default : 
		assert( 0 ); break;
	}
}

BOOL CWndConfirmVis::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	// 엔터 입력 처리 
	if( WIDC_EDIT1 == nID && EN_RETURN == message )
	{
		SendEquipPacket( );	
		Destroy();
		return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
	}

	if( message == WNM_CLICKED  )
	{
		switch(nID)
		{
			case WIDC_BUTTON2:   
				Destroy(); 
				break;
			case WIDC_BUTTON1:       
				SendEquipPacket( );
				Destroy();
				break;
		}
	}
		
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
#endif //#ifdef __PETVIS