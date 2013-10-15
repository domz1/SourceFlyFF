// WndBase.cpp: implementation of the CWndBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DPClient.h"
extern	CDPClient	g_DPlay;

#include "WndManager.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndSkillCtrl
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndSkillCtrl::CWndSkillCtrl() 
{
	m_nWndColor    = D3DCOLOR_TEMP( 255,  0x5 << 3,  0x5 << 3,  0x5 << 3 );
	m_nFontColor   = D3DCOLOR_TEMP( 255, 255, 255, 255 );
	m_nSelectColor = D3DCOLOR_TEMP( 255, 255, 255, 0   );
	m_nCurSelect = -1;
	m_pFocusItem = NULL;
	m_bDrag = FALSE;
	//m_apJobSkill = NULL;
	m_apSkill = NULL;
	m_nJob = -1;
	m_pVBGauge = NULL;
	//nExpert = -1;
}
CWndSkillCtrl::~CWndSkillCtrl()
{
//	ResetContent();
	DeleteDeviceObjects();
}
int CWndSkillCtrl::GetMaxSkill()
{
	int nNum = 0,	i;
	for( i = 0; i < MAX_SKILL_JOB; i ++ )
	{
		if( g_pPlayer->m_aJobSkill[i].dwSkill != NULL_ID )
			nNum ++;
	}

	return nNum;
}
void CWndSkillCtrl::Create( DWORD dwListCtrlStyle, RECT& rect, CWndBase* pParentWnd, UINT nID )
{
	m_dwListCtrlStyle = dwListCtrlStyle;
	CWndBase::Create( WBS_CHILD, rect, pParentWnd, nID );
}
void CWndSkillCtrl::SetExpert( int nJob )
{
	//m_nExpert = nExpert;
	m_nJob = nJob;
	for( int i = MAX_JOB_SKILL; i < MAX_JOB_SKILL + MAX_EXPERT_SKILL; i++ ) 
	{
		LPSKILL lpSkill = &m_apSkill[ i ];
		if( lpSkill->dwSkill == NULL_ID ) continue;
		ItemProp* pSkillProp = prj.m_aPropSkill.GetAt( lpSkill->dwSkill );
		m_atexSkill[ i ] = NULL;
		if( pSkillProp )
			m_atexSkill[ i ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, pSkillProp->szIcon ), 0xffff00ff );
	}
	int nPage, nRange;
	m_nFontHeight = 35;//GetFontHeight( &g_2DRender );
	nPage = GetClientRect().Height() / ( m_nFontHeight + 3 );
	nRange = GetMaxSkill();//m_pItemContainer->m_dwIndexNum;// - nPage;
	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );
	SetWndRect( GetWindowRect( TRUE ) );
}

void CWndSkillCtrl::SetPro( int nJob )
{
	m_nJob = nJob;
	for( int i = MAX_JOB_SKILL + MAX_EXPERT_SKILL; i < MAX_JOB_SKILL + MAX_EXPERT_SKILL + MAX_PRO_SKILL; i++ ) 
	{
		LPSKILL lpSkill = &m_apSkill[ i ];
		if( lpSkill->dwSkill == NULL_ID ) continue;
		ItemProp* pSkillProp = prj.m_aPropSkill.GetAt( lpSkill->dwSkill );
		m_atexSkill[ i ] = NULL;
		if( pSkillProp )
			m_atexSkill[ i ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, pSkillProp->szIcon ), 0xffff00ff );
	}
	int nPage, nRange;
	m_nFontHeight = 35;//GetFontHeight( &g_2DRender );
	nPage = GetClientRect().Height() / ( m_nFontHeight + 3 );
	nRange = GetMaxSkill();//m_pItemContainer->m_dwIndexNum;// - nPage;
	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );
	SetWndRect( GetWindowRect( TRUE ) );
}

void CWndSkillCtrl::InitItem( int nJob, int nJob_, LPSKILL apSkill )
{
	m_apSkill = apSkill;
	m_pFocusItem = NULL;
	m_nJob = nJob;
	
	// 소스 아이템을 입력
	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
	{
		LPSKILL lpSkill;
		lpSkill = &apSkill[ i ];
		m_atexSkill[ i ] = NULL;
		if( lpSkill->dwSkill != NULL_ID )
		{
			ItemProp* pSkillProp = prj.m_aPropSkill.GetAt( lpSkill->dwSkill );
			if( pSkillProp )
				m_atexSkill[ i ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, pSkillProp->szIcon ), 0xffff00ff );
		}
	}

	m_aSkillLevel[ 0 ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "ButtSkillLevelHold01.tga" ), 0xffff00ff );
	m_aSkillLevel[ 1 ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "ButtSkillLevelup01.tga" ), 0xffff00ff );
	m_aSkillLevel[ 2 ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "ButtSkillLevelHold02.tga" ), 0xffff00ff );
}
void CWndSkillCtrl::LoadListBoxScript(LPCTSTR lpFileName) 
{
}	
void CWndSkillCtrl::InterpriteScript(CScanner& scanner,CPtrArray& ptrArray) 
{
}
HRESULT CWndSkillCtrl::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if( m_pVBGauge == NULL )
		return m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBGauge, NULL );
	return S_OK;
}
HRESULT CWndSkillCtrl::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
    SAFE_RELEASE( m_pVBGauge );
	return S_OK;
}
HRESULT CWndSkillCtrl::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	SAFE_RELEASE( m_pVBGauge );
	return S_OK;
}
void CWndSkillCtrl::OnInitialUpdate()
{
	CRect rect = GetWindowRect();
	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );//,m_pSprPack,-1);
	int nPage, nRange;
	m_nFontHeight = 35;//GetFontHeight( &g_2DRender );
	nPage = GetClientRect().Height() / ( m_nFontHeight + 3 );
	nRange = GetMaxSkill();//m_pItemContainer->m_dwIndexNum;// - nPage;
	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );

	RestoreDeviceObjects();
	m_texGauEmptyNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptyNormal.bmp" ), 0xffff00ff, TRUE );
	m_texGauFillNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauFillNormal.bmp" ), 0xffff00ff, TRUE );
}
BOOL CWndSkillCtrl::CheckSkill( int i )
{
	LPSKILL pSkill = &m_apSkill[ i ];//m_apSkill[ i ];
	if( pSkill->dwSkill == NULL_ID ) 
		return NULL;
	int nJobLevel = g_pPlayer->m_aJobLv[g_pPlayer->GetJob()];
	DWORD dwReqDislLV = pSkill->GetProp()->dwReqDisLV;
	// 내가 익스퍼트일 경우 그 이전 직업( JTYPE_JOB )의 스킬은 익스퍼트 스킬 레벨 제한에
	// 걸려 일부만 출력된다 그것을 막기 위해 직업 타입을 구해 그 이전 스킬은 강제로
	// 출력하도록 했다.
	if( g_pPlayer->IsJobType( JTYPE_BASE ) )
	{
		if( dwReqDislLV > nJobLevel )
			return FALSE;
	}
	else
	if( g_pPlayer->IsJobType( JTYPE_EXPERT ) )
	{
		int a = pSkill->GetProp()->dwItemKind1;
		if( pSkill->GetProp()->dwItemKind1 != JTYPE_BASE && dwReqDislLV > nJobLevel )
			return FALSE;
	}
	return TRUE;
}
LPSKILL CWndSkillCtrl::GetSkill( int i )
{
	LPSKILL pSkill = &m_apSkill[ i ];//m_apSkill[ i ];
	if( pSkill->dwSkill == NULL_ID ) 
		return NULL;
	int nJobLevel = g_pPlayer->m_aJobLv[g_pPlayer->GetJob()];
	if( pSkill->GetProp() == NULL )
	{
		Error( "GetSkill Error Not Prop : %d", i );
		return NULL;
	}
	
	DWORD dwReqDislLV = pSkill->GetProp()->dwReqDisLV;
	return pSkill;
}
void CWndSkillCtrl::OnMouseWndSurface( CPoint point )
{
	CPoint pt( 3, 3 );
	pt.y -= ( m_nFontHeight + 3 ) * m_wndScrollBar.GetScrollPos();

	int nMaxSkill = GetMaxSkill();
	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
	{
		LPSKILL pSkill = GetSkill( i );
		if( pSkill == NULL ) 
			continue;
		DWORD dwSkill = pSkill->dwSkill;
		ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );

		int x = 0, nWidth = m_rectClient.Width();// - 1;

		CRect rectHittest = CRect( x + 3, pt.y + 6, x + 3 + nWidth, pt.y + 6 + 32 );// pt.x, pt.y+ 12, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight+ 12 );

		if( rectHittest.PtInRect( point ) )
		{
			CPoint point2 = point;
			ClientToScreen( &point2 );
			ClientToScreen( &rectHittest );

			g_WndMng.PutToolTip_Skill( dwSkill, pSkill->dwLevel, point2, &rectHittest );
		}
		pt.y += m_nFontHeight + 3;
	}
}

void CWndSkillCtrl::OnDraw( C2DRender* p2DRender ) 
{
	if( NULL == g_pPlayer )
		return;

	CPoint pt( 3, 3 );

	// 리포트 출력 
	if( 1 ) //m_dwListCtrlStyle == WLVS_REPORT )
	{
		pt.y -= ( m_nFontHeight + 3 ) * m_wndScrollBar.GetScrollPos();
		int nMaxSkill = GetMaxSkill();
		for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
		{
			DWORD dwColor = 0xff000000;
			if( i == m_nCurSelect )
				dwColor = 0xff6060ff; 
			LPSKILL pSkill = GetSkill( i );
			if( pSkill == NULL ) 
				continue;
			DWORD dwSkill = pSkill->dwSkill;
			
			ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
			AddSkillProp* pAddSkillProp = prj.GetAddSkillProp( pSkillProp->dwSubDefine, pSkill->dwLevel );
			int x = 0, nWidth = m_rectClient.Width();// - 1;

			CRect rect( x, pt.y, x + nWidth, pt.y + m_nFontHeight );

			if( dwSkill != NULL_ID )
			{
				// 게이지 출력 
				int ax = 10;
				int ay = 5;
				rect.SetRect( x + 50 + ax, pt.y + 20 + ay, x + nWidth - 10, pt.y + 30 + ay ); 
				CRect rectTemp = rect; 
				rectTemp.right = rectTemp.left + nWidth;
				if( rect.right < rectTemp.right )
					rectTemp.right = rect.right;

				if( pSkill->dwLevel < pSkillProp->dwExpertMax )
				{
					m_pTheme->RenderGauge( p2DRender, &rect, 0xffffffff, m_pVBGauge, &m_texGauEmptyNormal );
					m_pTheme->RenderGauge( p2DRender, &rectTemp, 0x64ff0000, m_pVBGauge, &m_texGauFillNormal );
				}
				
				rect.SetRect( x + 3, pt.y + 6, x + 3 + 32, pt.y + 6 + 32 ); 
				// 스킬 아이콘 출력 
				if( m_atexSkill[ i ] )
				{
					if( CheckSkill( i ) )
						m_atexSkill[ i ]->Render(  p2DRender, rect.TopLeft() );
					else
						m_atexSkill[ i ]->Render(  p2DRender, rect.TopLeft(), 64 );
				}	
				// 스킬 레벨 출력 
				TCHAR szNum[ 32 ]; 
#ifdef __SKILL0517
				sprintf( szNum, "Lv%2d", g_pPlayer->GetSkillLevel( pSkill ) );
#else	// __SKILL0517
				sprintf( szNum, "Lv%2d", pSkill->dwLevel );
#endif	// __SKILL0517
				SIZE size = p2DRender->m_pFont->GetTextExtent( szNum ); // 레벨 폰트의 사이즈 읽기 
				p2DRender->TextOut( rect.left + 50 - size.cx + ax / 2, rect.top + 30 - size.cy, szNum, dwColor );

				if( pSkill->dwLevel >= pSkillProp->dwExpertMax )
				{ 
					p2DRender->TextOut( rect.left + 83 - size.cx + ax / 2, rect.top + 30 - size.cy, "Master", 0xffff0000 );
				}
				
				// 스킬명 출력 
				p2DRender->TextOut( x + 60 + ax / 2, pt.y + 5 + ay, pSkillProp->szName, dwColor ); 
			}
			pt.y += m_nFontHeight + 3;
		}
	}
}

BOOL CWndSkillCtrl::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
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
	
	return TRUE;
}
	
void CWndSkillCtrl::OnLButtonUp( UINT nFlags, CPoint point )
{
	m_bDrag = FALSE;

	if(	CWndBase::m_GlobalShortcut.m_dwShortcut != SHORTCUT_NONE )
	{
	}
	CPoint pt( 3, 3 );
	CRect rect;
	CRect rectstop;

	// 리포트  
	if( 1 ) //m_dwListCtrlStyle == WLVS_REPORT )
	{
		pt.y -= (m_nFontHeight + 3) * m_wndScrollBar.GetScrollPos();
		int nMaxSkill = GetMaxSkill();
		for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
		{
			DWORD dwSkill = m_apSkill[ i ].dwSkill;
			LPSKILL pSkill = GetSkill( i );
			if( pSkill && CheckSkill( i ) )
			{
				ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
				rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );

				rectstop.SetRect( pt.x + 33, pt.y, pt.x + 57, pt.y + 12 );
				if( rect.PtInRect( point ) )
				{
					if( rectstop.PtInRect( point ) )
					{
						int aaa = 0;
						return;
					}
					if( m_pFocusItem == &m_apSkill[ i ] )
					{
						// 부모가 차일드 윈도가 아니어야 OnCommand 메시지를 받는다.
						CWndBase* pWnd = m_pParentWnd;
						pWnd->OnChildNotify( WNM_SELCHANGE, m_nIdWnd, (LRESULT*)m_pFocusItem ); 
						return;
					}
				}
			}
			pt.y += m_nFontHeight + 3;
		}
	}
}
void CWndSkillCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bDrag == FALSE )
		return;

	CPoint pt( 3, 3 );
	CRect rect;
	if( CheckSkill( m_nCurSelect ) )
	{
		m_bDrag = FALSE;
		DWORD dwSkill = m_apSkill[ m_nCurSelect ].dwSkill;
			ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );

		m_GlobalShortcut.m_dwShortcut  = SHORTCUT_SKILL;
		m_GlobalShortcut.m_dwType  = 0;//m_nJob;//(DWORD)pItemElem;//->m_dwItemId;
		m_GlobalShortcut.m_dwIndex = dwSkill;//m_nCurSelect;//(DWORD)pItemElem;//->m_dwItemId;
		m_GlobalShortcut.m_dwData = 0;//pItemElem->m_dwObjId;//(DWORD)pItemElem;
		m_GlobalShortcut.m_dwId       = m_nCurSelect; // 컬런트 셀렉트가 곧 ID나 마찬가지임.
		m_GlobalShortcut.m_pTexture = m_atexSkill[ m_nCurSelect ];//L;//pItemElem->m_pTexture;
		_tcscpy( m_GlobalShortcut.m_szString, pSkillProp->szName);
	}
}
BOOL CWndSkillCtrl::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	return TRUE;
	if( pShortcut->m_dwShortcut == SHORTCUT_ITEM )
	{
		int nDstId = -1;
		if( m_dwListCtrlStyle == WLVS_ICON )
		{
			CRect rect = GetClientRect();
			int nWidth = rect.Width() / 32;
			int nHeight = rect.Height() / 32;
			CPoint pt( 3, 3 );

			pt.y = 0;
			pt.y += m_wndScrollBar.GetScrollPos() * nWidth;
			for( int i = pt.y; i < GetMaxSkill(); i++ ) 
			{
				int x = ( i - pt.y ) % nWidth;
				int y = ( i - pt.y ) / nWidth;
				rect.SetRect( x * 32, y * 32, x * 32 + 32, y * 32 + 32 );
				if( rect.PtInRect( point ) )
				{
					nDstId = i;
					break;
				}
			}
		}
		return TRUE;
	}
	return FALSE;
}
void CWndSkillCtrl::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	// 클로즈 베타용 임시 코드
	// 스킬창에서 더블클릭하면 자동으로 스킬바에 등록된다.
	CPoint pt( 3, 3 );
	CRect rect;

	// 리포트 출력 
	if( 1 ) //m_dwListCtrlStyle == WLVS_REPORT )
	{
		pt.y -= (m_nFontHeight + 3) * m_wndScrollBar.GetScrollPos();
		int nMaxSkill = GetMaxSkill();
		for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
		{
			LPSKILL pSkill = GetSkill( i );
			if( pSkill && CheckSkill( i ) )
			{
				DWORD dwSkill = m_apSkill[ i ].dwSkill;
				ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
				rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
				if( rect.PtInRect( point ) )
				{
					m_nCurSelect = i;
					m_pFocusItem = &m_apSkill[ i ];//pSkillProp;
					m_rect = rect;
					CWndTaskBar* pTaskBar = g_WndMng.m_pWndTaskBar;
					if( pTaskBar->m_nExecute == 0 )		// 스킬큐가 사용되지 않을때만 등록됨.
						pTaskBar->SetSkillQueue( pTaskBar->m_nCurQueueNum, pSkillProp->dwItemJob, i, m_atexSkill[i] );
				
					break;
				}
				pt.y += m_nFontHeight + 3;
			}
		}
	}
	
	return;
}

void CWndSkillCtrl::OnLButtonDown( UINT nFlags, CPoint point )
{
	CPoint pt( 3, 3 );
	CRect rect;
	CRect rectstop;

	// 리포트 출력 
	if( 1 ) //m_dwListCtrlStyle == WLVS_REPORT )
	{
		pt.y -= (m_nFontHeight + 3) * m_wndScrollBar.GetScrollPos();
		for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
		{
			LPSKILL pSkill = GetSkill( i );
			if( pSkill && CheckSkill( i ) )
			{
				DWORD dwSkill = m_apSkill[ i ].dwSkill;
				ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
				rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
				rectstop.SetRect( pt.x + 33, pt.y + 13, pt.x + 56, pt.y + 24 );
				if( rect.PtInRect( point ) )
				{
					m_nCurSelect = i;
					m_pFocusItem = &m_apSkill[ i ];//pSkillProp;
					m_bDrag = TRUE;
					m_rect = rect;
					//SetCapture();
					break;
				}
				pt.y += m_nFontHeight + 3;
			}
		}
	}
}
void CWndSkillCtrl::OnRButtonUp( UINT nFlags, CPoint point )
{
}
void CWndSkillCtrl::OnRButtonDown( UINT nFlags, CPoint point )
{
}
void CWndSkillCtrl::OnRButtonDblClk( UINT nFlags, CPoint point)
{
}
void CWndSkillCtrl::OnSize( UINT nType, int cx, int cy )
{
	CRect rect = GetWindowRect();
	rect.left = rect.right - 15;
	m_wndScrollBar.SetWndRect( rect );

	CWndBase::OnSize( nType, cx, cy);
}
void CWndSkillCtrl::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect( 3, 3 );

	if( m_apSkill )
	{
		if( 1 ) //m_dwListCtrlStyle == WLVS_REPORT )
		{
			int nPage = GetClientRect().Height() / (m_nFontHeight + 3);
			int nRange = GetMaxSkill();//m_pItemContainer->m_dwIndexNum;// - nPage;
			m_rectClient.right -= 15; // 스크롤 바가 보이면 
		}
	}
	if( bOnSize )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
}
void CWndSkillCtrl::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();

	DWORD dwColor1 = D3DCOLOR_ARGB( 100, 0, 0,  0 );//D3DCOLOR_TEMP( 255,   0,   0,  50 );//
	DWORD dwColor2 = D3DCOLOR_ARGB( 255, 240, 240,  240 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//
	DWORD dwColor3 = D3DCOLOR_ARGB( 100, 200, 200,  200 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//

	p2DRender->RenderFillRect ( rect, dwColor1 );
	p2DRender->RenderRoundRect( rect, dwColor2 );
	rect.DeflateRect( 1 , 1 );
	p2DRender->RenderRect( rect, dwColor2 );
	rect.DeflateRect( 1 , 1 );
	p2DRender->RenderRect( rect, dwColor3 );

	int nPage, nRange;
	if( 1 ) //m_dwListCtrlStyle == WLVS_REPORT )
	{
		m_nFontHeight = 35;//GetFontHeight( &g_2DRender );
		nPage = GetClientRect().Height() / ( m_nFontHeight + 3 );
		nRange = GetMaxSkill();//m_pItemContainer->m_dwIndexNum;// - nPage;
	}

	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );
}

BOOL CWndSkillCtrl::OnEraseBkgnd( C2DRender* p2DRender )
{
	return TRUE;
}
BOOL CWndSkillCtrl::SetItem( const LVITEM* pItem )
{
	return TRUE;
}
int CWndSkillCtrl::InsertItem( const LVITEM* pItem )
{
	return TRUE;
}
int CWndSkillCtrl::GetItemCount() const
{
	return 1;//m_aItems.GetSize();
}
int CWndSkillCtrl::InsertColumn( int nCol, const LVCOLUMN* pColumn )
{
	return nCol;
}