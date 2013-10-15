#include "WndMonsterInformation.h"
#include "stdafx.h"
#include "resData.h"
#include "DPClient.h"
#include "WndManager.h"
#include "defineText.h"

extern CDPClient g_DPlay;
#if __VER >= 19
CWndMonsterInformation::CWndMonsterInformation( CMover* pMover )
{
	m_pMover = pMover;

	m_nHPWidth = -1;
	m_nMPWidth = -1;

	m_pVBHPGauge = NULL;
	m_pVBMPGauge = NULL;

	m_bVBHPGauge = TRUE;
	m_bVBMPGauge = TRUE;

	m_nDigit1 = 0;
	m_nDigit2 = 0;
	m_nDigit3 = 0;	
	m_nType = 0;

	bIsAMonster = FALSE;
}
CWndMonsterInformation::~CWndMonsterInformation()
{
	DeleteDeviceObjects();
}
void CWndMonsterInformation::SetTextures()
{
	if( m_pMover == NULL || m_pMover->IsDie() || m_pMover->IsDelete() )
		return;

	if( m_pMover->IsPlayer() )//Spieler
	{
		m_nType = 1;
		//m_texBack.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "PlayerBG.tga" ), 0xffff00ff, TRUE );
		switch( m_pMover->GetJob() )
		{
			case JOB_VAGRANT:
				m_texFront.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "JobVagrant.tga" ), 0xffff00ff, TRUE );
				break;
			case JOB_MERCENARY:
				m_texFront.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "JobMercenary.tga" ), 0xffff00ff, TRUE );
				break;
			case JOB_ASSIST:
				m_texFront.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "JobAssist.tga" ), 0xffff00ff, TRUE );
				break;
			case JOB_MAGICIAN:
				m_texFront.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "JobMagician.tga" ), 0xffff00ff, TRUE );
				break;
			case JOB_ACROBAT:
				m_texFront.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "JobAcrobat.tga" ), 0xffff00ff, TRUE );
				break;
			case JOB_BLADE:
				m_texFront.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "JobBlade.tga" ), 0xffff00ff, TRUE );
				break;
			case JOB_KNIGHT:
				m_texFront.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "JobKnight.tga" ), 0xffff00ff, TRUE );
				break;
			case JOB_RINGMASTER:
				m_texFront.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "JobRingmaster.tga" ), 0xffff00ff, TRUE );
				break;
			case JOB_BILLPOSTER:
				m_texFront.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "JobBillposter.tga" ), 0xffff00ff, TRUE );
				break;
			case JOB_ELEMENTOR:
				m_texFront.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "JobElementor.tga" ), 0xffff00ff, TRUE );
				break;
			case JOB_PSYCHIKEEPER:
				m_texFront.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "JobPsychikeeper.tga" ), 0xffff00ff, TRUE );
				break;
			case JOB_RANGER:
				m_texFront.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "JobRanger.tga" ), 0xffff00ff, TRUE );
				break;
			case JOB_JESTER:
				m_texFront.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "JobJester.tga" ), 0xffff00ff, TRUE );
				break;
			case JOB_BLADE_MASTER:
				m_texFront.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "JobBladeMaster.tga" ), 0xffff00ff, TRUE );
				break;
			case JOB_KNIGHT_MASTER:
				m_texFront.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "JobKnightMaster.tga" ), 0xffff00ff, TRUE );
				break;
			case JOB_RINGMASTER_MASTER:
				m_texFront.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "JobRingmasterMaster.tga" ), 0xffff00ff, TRUE );
				break;
			case JOB_BILLPOSTER_MASTER:
				m_texFront.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "JobBillposterMaster.tga" ), 0xffff00ff, TRUE );
				break;
			case JOB_ELEMENTOR_MASTER:
				m_texFront.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "JobElementorMaster.tga" ), 0xffff00ff, TRUE );
				break;
			case JOB_PSYCHIKEEPER_MASTER:
				m_texFront.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "JobPsychikeeperMaster.tga" ), 0xffff00ff, TRUE );
				break;
			case JOB_RANGER_MASTER:
				m_texFront.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "JobRangerMaster.tga" ), 0xffff00ff, TRUE );
				break;
			case JOB_JESTER_MASTER:
				m_texFront.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "JobJesterMaster.tga" ), 0xffff00ff, TRUE );
				break;
			case JOB_BLADE_HERO:
				m_texFront.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "JobBladeHero.tga" ), 0xffff00ff, TRUE );
				break;
			case JOB_KNIGHT_HERO:
				m_texFront.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "JobKnightHero.tga" ), 0xffff00ff, TRUE );
				break;
			case JOB_RINGMASTER_HERO:
				m_texFront.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "JobRingmasterHero.tga" ), 0xffff00ff, TRUE );
				break;
			case JOB_BILLPOSTER_HERO:
				m_texFront.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "JobBillposterHero.tga" ), 0xffff00ff, TRUE );
				break;
			case JOB_ELEMENTOR_HERO:
				m_texFront.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "JobElementorHero.tga" ), 0xffff00ff, TRUE );
				break;
			case JOB_PSYCHIKEEPER_HERO:
				m_texFront.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "JobPsychikeeperHero.tga" ), 0xffff00ff, TRUE );
				break;
			case JOB_RANGER_HERO:
				m_texFront.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "JobRangerHero.tga" ), 0xffff00ff, TRUE );
				break;
			case JOB_JESTER_HERO:
				m_texFront.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "JobJesterHero.tga" ), 0xffff00ff, TRUE );
				break;
			case JOB_STORMBLADE_HERO:
				m_texFront.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "JobStormbladeHero.tga" ), 0xffff00ff, TRUE );
				break;
			case JOB_LORDTEMPLER_HERO:
				m_texFront.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "JobLordTemplerHero.tga" ), 0xffff00ff, TRUE );
				break;
			case JOB_FORCEMASTER_HERO:
				m_texFront.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "JobForcemasterHero.tga" ), 0xffff00ff, TRUE );
				break;
			case JOB_FLORIST_HERO:
				m_texFront.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "JobFloristHero.tga" ), 0xffff00ff, TRUE );
				break;
			case JOB_ELEMENTORLORD_HERO:
				m_texFront.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "JobElementorlordHero.tga" ), 0xffff00ff, TRUE );
				break;
			case JOB_MENTALIST_HERO:
				m_texFront.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "JobMentalistHero.tga" ), 0xffff00ff, TRUE );
				break;
			case JOB_WINDLURKER_HERO:
				m_texFront.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "JobWindlurkerHero.tga" ), 0xffff00ff, TRUE );
				break;
			case JOB_CRACKSHOOTER_HERO:
				m_texFront.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "JobCrackshooterHero.tga" ), 0xffff00ff, TRUE );
				break;
		}
	}
	else if( m_pMover->IsNPC() == m_pMover->IsPeaceful() || m_pMover->GetProp()->dwClass == RANK_GUARD )//NPC
	{
		m_nType = 2;
		//m_texBack.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "PlayerBG.tga" ), 0xffff00ff, TRUE );
		m_texFront.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "JobNPC.tga" ), 0xffff00ff, TRUE );
	}
	else//Monster
	{
		m_nType = 3;
		switch( m_pMover->GetProp()->eElementType )
		{
			case SAI79::FIRE:
				m_texBack.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "eElementTypeRed01.tga" ), 0xffff00ff, TRUE );
				break;
			case SAI79::WATER:
				m_texBack.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "eElementTypeBlue01.tga" ), 0xffff00ff, TRUE );
				break;
			case SAI79::ELECTRICITY:
				m_texBack.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "eElementTypeYellow01.tga" ), 0xffff00ff, TRUE );
				break;
			case SAI79::WIND:
				m_texBack.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "eElementTypeGreen01.tga" ), 0xffff00ff, TRUE );
				break;
			case SAI79::EARTH:
				m_texBack.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "eElementTypePurple01.tga" ), 0xffff00ff, TRUE );
				break;
			default:
				m_texBack.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "eElementTypeWhite01.tga" ), 0xffff00ff, TRUE );
				break;
		}

		int nLevel = m_pMover->GetProp()->dwLevel;
		if( nLevel > 99 )
		{
			m_nDigit1 = nLevel / 100;
			nLevel %= 100;
		}
		if( nLevel > 9 )
		{
			m_nDigit2 = nLevel / 10;
			nLevel %= 10;
		}
		m_nDigit3 = nLevel;

		CString str;
		if( m_nDigit1 != 0 )
		{
			str.Format("TargetNum%d.tga", m_nDigit1 );
			m_texDigit1.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, str.GetString() ), 0xffff00ff, TRUE );
		}
		if( m_nDigit2 != 0 || m_nDigit1 != 0 )
		{
			str.Format("TargetNum%d.tga", m_nDigit2 );
			m_texDigit2.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, str.GetString() ), 0xffff00ff, TRUE );
		}

		str.Format("TargetNum%d.tga", m_nDigit3 );
		m_texDigit3.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, str.GetString() ), 0xffff00ff, TRUE );

		CString szFileName;
		switch( m_pMover->GetProp()->dwAreaColor )
		{
			case AREA_NORMAL:
				szFileName = "RanBlue";
				break;
			case AREA_DUNGEON:
				szFileName = "RanRed";
				break;
			case AREA_CASH:
				szFileName = "RanPurple";
				break;
			case AREA_INSTANCE:
				szFileName = "RanRed";
				break;
			default:
				szFileName = "RanBlue";
				break;
		}
		CString szFile;
		szFile.Format( "%s01.tga", szFileName );
		m_texDanger[0].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, szFile.GetString() ), 0xffff00ff, TRUE );
		szFile.Format( "%s02.tga", szFileName );
		m_texDanger[1].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, szFile.GetString() ), 0xffff00ff, TRUE );
		szFile.Format( "%s03.tga", szFileName );
		m_texDanger[2].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, szFile.GetString() ), 0xffff00ff, TRUE );
		m_nDangerNum = ( m_pMover->GetProp()->dwClass > 6 || m_pMover->GetProp()->dwClass < 1 ? 1 : m_pMover->GetProp()->dwClass );
	}
	CWndStatic* pWndStatic1 = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
	CWndStatic* pWndStatic2 = (CWndStatic*)GetDlgItem(WIDC_STATIC2);
	pWndStatic1->SetVisible(FALSE);
	pWndStatic2->SetVisible(FALSE);
}
void CWndMonsterInformation::OnInitialUpdate()
{
	if( m_pMover == NULL || m_pMover->IsDie() || m_pMover->IsDelete() )
		return;

	CWndNeuz::OnInitialUpdate();
	
	RestoreDeviceObjects();
	m_texGauHp.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "Targetgauge01.tga" ), 0xffff00ff, TRUE );
	m_texGauMp.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "Targetgauge02.tga" ), 0xffff00ff, TRUE );
	SetTextures();

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	int left = rectRoot.right / 2 - GetClientRect().right / 2;
	CRect rect = CRect( left, 30, left + GetClientRect().right / 2, 30 + GetClientRect().bottom );
	CPoint point( rect.left, rect.top );

	Move( point );
}
void CWndMonsterInformation::OnDraw( C2DRender* p2DRender )
{
	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetRenderState( D3DRS_AMBIENT,  D3DCOLOR_ARGB( 255, 255,255,255) );

	if( m_pMover == NULL || m_pMover->IsDie() || m_pMover->IsDelete())
	{
		Destroy();
		return;
	}

	CRect rectBack;
	if( m_nType == 3 )
		rectBack = GetWndCtrl( WIDC_CUSTOM3 )->rect;
	else
	{
		rectBack = GetWndCtrl( WIDC_STATIC2 )->rect;
		rectBack.left += 3;
		rectBack.top += 3;
	}
	m_texBack.Render( p2DRender, CPoint( rectBack.left, rectBack.top ), 255 );
	//m_texBack2.Render( p2DRender, CPoint( 0,0 ), 255 );

    CRect rectM;
	if( m_nType == 3 )
		rectM = GetWndCtrl( WIDC_CUSTOM4 )->rect;
	else
		rectM = GetWndCtrl( WIDC_STATIC2 )->rect;

	if( m_nType == 3 )
	{
		int nRect[6] = { WIDC_CUSTOM10, WIDC_CUSTOM9, WIDC_CUSTOM8, WIDC_CUSTOM6, WIDC_CUSTOM5, WIDC_CUSTOM7 };
		for( int i=0;i<6;i++ )
		{
			if( i >= m_nDangerNum )
				break;
			CRect rectRan = GetWndCtrl( nRect[i] )->rect;
			if( i == 0 || i == 5 )
				m_texDanger[0].Render( p2DRender, rectRan.TopLeft(), 255 );
			else if( i == 1 || i == 4 )
				m_texDanger[1].Render( p2DRender, rectRan.TopLeft(), 255 );
			else
				m_texDanger[2].Render( p2DRender, rectRan.TopLeft(), 255 );
		}

		int countDigits = 0;
		if( m_nDigit2 != 0 || m_nDigit1 != 0 )
			countDigits++;
		if( m_nDigit1 != 0 )
			countDigits++;

		CPoint tempPoint = CPoint( rectM.left + (rectM.Width() / 2), rectM.top + (rectM.Height() / 2) - (16/2) );

		if( countDigits == 2 )
		{
			m_texDigit1.Render( p2DRender, CPoint( tempPoint.x - ( 10/2 ) - 10, tempPoint.y ) );
			m_texDigit2.Render( p2DRender, CPoint( tempPoint.x - ( 10/2 ), tempPoint.y ) );
			m_texDigit3.Render( p2DRender, CPoint( tempPoint.x - ( 10/2 ) + 10, tempPoint.y ) );
		}
		else if( countDigits == 1 )
		{
			m_texDigit2.Render( p2DRender, CPoint( tempPoint.x - ( 10 ), tempPoint.y ) );
			m_texDigit3.Render( p2DRender, CPoint( tempPoint.x, tempPoint.y ) );
		}
		else
			m_texDigit3.Render( p2DRender, CPoint( tempPoint.x - ( 10/2 ), tempPoint.y ) );
	}
	else
		m_texFront.Render( p2DRender, CPoint( rectM.left, rectM.top ), 255 );

	CRect lpHP   = GetWndCtrl( WIDC_CUSTOM1 )->rect;
	CRect lpMP   = GetWndCtrl( WIDC_CUSTOM2 )->rect;
	if( m_pMover )
	{
		MakeGaugeVertex();

		if( m_bVBHPGauge )
			m_pTheme->RenderGauge( p2DRender->m_pd3dDevice, m_pVBHPGauge, &m_texGauHp );
		if( m_bVBMPGauge )
			m_pTheme->RenderGauge( p2DRender->m_pd3dDevice, m_pVBMPGauge, &m_texGauMp );
	}	

	CString szName;
	if( m_pMover->IsPlayer() )
		szName.Format( "%s", m_pMover->GetName() );
	else
		szName.Format( "%s", m_pMover->GetProp()->szName );

	CPoint newpoint;
	CSize sizeTitle = p2DRender->m_pFont->GetTextExtent( szName );
	CRect rectNAME	= GetWindowRect();
	newpoint = (rectNAME.Width() - sizeTitle.cx) / 2;

	CD3DFont* pOldFont = p2DRender->GetFont();
	p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndNewTitle );
    p2DRender->TextOut( newpoint.x + 25 , 6, szName, 0xffffffff );
    p2DRender->SetFont( pOldFont );
}
HRESULT CWndMonsterInformation::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if( m_pVBHPGauge == NULL )
	{
		g_Neuz.m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBHPGauge, NULL );
		g_Neuz.m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBMPGauge, NULL );

		m_nHPWidth = -1;
		m_nMPWidth = -1;
	}

	m_texGauHp.SetInvalidate(g_Neuz.m_pd3dDevice); 
	m_texGauMp.SetInvalidate(g_Neuz.m_pd3dDevice);  
	
	return S_OK;
}
HRESULT CWndMonsterInformation::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
    SAFE_RELEASE( m_pVBHPGauge );
    SAFE_RELEASE( m_pVBMPGauge );

	m_texGauHp.Invalidate(); 
	m_texGauMp.Invalidate(); 
	m_texBack.Invalidate();
	//m_texBack2.Invalidate();
	m_texFront.Invalidate();
	m_texDifficult.Invalidate();
	m_texDigit1.Invalidate();
	m_texDigit2.Invalidate();
	m_texDigit3.Invalidate();

	for( int i=0;i<3;i++ )
		m_texDanger[i].Invalidate();
	
	return S_OK;
}
HRESULT CWndMonsterInformation::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	return InvalidateDeviceObjects();
}
void CWndMonsterInformation::MakeGaugeVertex()
{
	LPWNDCTRL lpHP   = GetWndCtrl( WIDC_CUSTOM1 );
	LPWNDCTRL lpMP   = GetWndCtrl( WIDC_CUSTOM2 );

	if( m_pMover )
	{
		CRect rectG = GetClientRect();
		CRect rectTemp;
		
		int nWidthClient = lpHP->rect.Width();
		int nWidth;

		// HP //m_pMover->GetHitPoint()
		nWidth = (int)( (__int64)nWidthClient * (__int64)m_pMover->GetHitPoint() / (__int64)m_pMover->GetMaxHitPoint() );
		if( m_nHPWidth != nWidth ) 
		{
			m_nHPWidth = nWidth;
			rectG = lpHP->rect;
			rectTemp = rectG; 
			rectTemp.right = rectTemp.left + nWidth;
			ClientToScreen( rectTemp );
			m_bVBHPGauge = m_pTheme->MakeGaugeVertex( g_Neuz.m_pd3dDevice, &rectTemp, 0xffffffff, m_pVBHPGauge, &m_texGauHp );
		}
		nWidth	= nWidthClient * m_pMover->GetManaPoint() / m_pMover->GetMaxManaPoint();
		if( m_nMPWidth != nWidth ) 
		{
			m_nMPWidth = nWidth;
			rectG = lpMP->rect;
			rectTemp = rectG; 
			rectTemp.right = rectTemp.left + nWidth;
			ClientToScreen( rectTemp );
			m_bVBMPGauge = m_pTheme->MakeGaugeVertex( g_Neuz.m_pd3dDevice, &rectTemp, 0xffffffff, m_pVBMPGauge, &m_texGauMp );
		}
	}
}
BOOL CWndMonsterInformation::Initialize(CWndBase* pWndParent,DWORD dwWndId)
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_TARGETINFORNATION, WBS_TOPMOST, CPoint( 0, 0 ), pWndParent );
}
void CWndMonsterInformation::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndMonsterInformation::SetWndRect( CRect rectWnd, BOOL bOnSize  )
{
	m_nHPWidth = -1;
	m_nMPWidth = -1;
	
	CWndNeuz::SetWndRect( rectWnd, bOnSize );
}
#endif