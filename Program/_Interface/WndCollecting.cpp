#include "stdafx.h"
#include "resData.h"
#include "WndCollecting.h"
#include "Collecting.h"
#include "DPClient.h"
#include "DefineObj.h"
#include "defineText.h"

extern	CDPClient	g_DPlay;


/****************************************************
  WndId : APP_COLLECTING - Applet
  CtrlId : WIDC_BUTTON1 - Start
  CtrlId : WIDC_STATIC1 - Bettery
  CtrlId : WIDC_STATIC2 - Level
  CtrlId : WIDC_CUSTOM1 - Custom
  CtrlId : WIDC_STATIC3 - 0
  CtrlId : WIDC_STATIC4 - /  5
  CtrlId : WIDC_STATIC5 - 0
  CtrlId : WIDC_STATIC6 - / 1600
****************************************************/

CCollectingWnd::CCollectingWnd() 
{ 

	m_bIsCollecting = false;
	D3DXMatrixIdentity(&m_matModel);
	//D3DXMatrixScaling(&m_matModel, 3.0f, 3.0f, 3.0f);
	m_pSfxBase	= NULL;
	m_pSfx		= NULL;
	m_fAngle	= 0.0f;
	m_pVBGauge	= NULL;
	m_pTexGauEmptyNormal = NULL;
	m_pTexGauFillNormal = NULL;
	m_pTexBatt = m_pTexLevel = NULL;

} 


CCollectingWnd::~CCollectingWnd() 
{ 
} 


void CCollectingWnd::OnDraw( C2DRender* p2DRender ) 
{ 
#if __VER >= 11 // __SYS_COLLECTING
	//if( m_pElem == NULL)
	//return;
    if(!g_pPlayer) return;
	LPWNDCTRL wndCtrlPic1 = GetWndCtrl( WIDC_PIC1 );
	LPWNDCTRL wndCtrlPic2 = GetWndCtrl( WIDC_PIC2 );

	if(m_pTexBatt)
	{
		m_pTexBatt->Render(p2DRender, wndCtrlPic2->rect.TopLeft());
	}
	if(m_pTexLevel)
	{
		m_pTexLevel->Render(p2DRender, wndCtrlPic1->rect.TopLeft());
	}

	CItemElem* pCollector = g_pPlayer->GetCollector();

	if(pCollector)
	{
		LPWNDCTRL	lpGauge   = GetWndCtrl( WIDC_GAUGE );
		CRect 		rect = GetClientRect();
		CRect 		rectTemp;
		int 		nWidthClient = lpGauge->rect.Width();
		int 		nWidth;
		int			nMax = CCollectingProperty::GetInstance()->GetMaxBattery();
		float		fRatio = (float)pCollector->m_nHitPoint / (float)nMax; 
		int			nBtryRemain = pCollector->m_nHitPoint;

		if(nBtryRemain < 0)  nBtryRemain = 0;

		nWidth = (int)( nWidthClient * fRatio );
		wsprintf(m_pbufText, "%d / %d (%d%%)", nBtryRemain, nMax, (int)(fRatio * 100));

		m_nGWidth = nWidth;
		if(m_nGWidth >= 2)
		{
			rect = lpGauge->rect;
			rectTemp = rect; 
			rectTemp.right = rectTemp.left + nWidth;
			ClientToScreen( rectTemp );
			//m_pTheme->MakeGaugeVertex( m_pApp->m_pd3dDevice, &rect, D3DCOLOR_ARGB( 200, 255, 255, 255 ), m_pVBGauge, m_pTexGauFillNormal );
			//m_pTheme->RenderGauge( p2DRender->m_pd3dDevice, m_pVBGauge, m_pTexGauEmptyNormal );	
			m_pTheme->MakeGaugeVertex( m_pApp->m_pd3dDevice, &rectTemp, D3DCOLOR_ARGB( 128, 0, 0, 255 ), m_pVBGauge, m_pTexGauFillNormal );
			m_pTheme->RenderGauge( p2DRender->m_pd3dDevice, m_pVBGauge, m_pTexGauEmptyNormal );	
		}
	}
	
	

	LPWNDCTRL lpGauge   = GetWndCtrl( WIDC_GAUGE );
	DWORD dwColor = D3DCOLOR_ARGB(255, 0, 0, 255 );
	
	p2DRender->TextOut( lpGauge->rect.left + 5, lpGauge->rect.top, m_pbufText, dwColor, 0x00000000 );
/*
	D3DXMATRIX  matView;
	D3DXMATRIX  matWorld;
	D3DXMATRIX  matScale;
	D3DXMATRIX  matRot;
	D3DXMATRIX  matTrans;
	D3DXVECTOR3 vecPos;
	D3DXVECTOR3 vecLookAt;
	float		fScale	= 1.5f;
	D3DXVECTOR3 vSrc, vLocal	= D3DXVECTOR3( 0, 0, 0 );
				

	CPoint point	= CPoint( GetWndCtrl( WIDC_CUSTOM1 )->rect.left-12, GetWndCtrl( WIDC_CUSTOM1 )->rect.top-6 );
	CRect rect		= GetClientRect();
	int nWidthClient = GetClientRect().Width() - 110;
	CRect rectTemp;
	LPWNDCTRL lpFace = GetWndCtrl( WIDC_CUSTOM1 );
	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;
	// 뷰포트 세팅 
	D3DVIEWPORT9 viewport;

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

	FLOAT fov = D3DX_PI/4.0f;
	FLOAT h = cos(fov / 2) / sin(fov / 2);
	FLOAT w = h * fAspect;
	D3DXMatrixOrthoLH( &matProj, w, h, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixIdentity(&matRot);
	D3DXMatrixIdentity(&matTrans);
	
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	vecPos.x = 0.0f;
	vecPos.y = 1.5f;
	vecPos.z = -1.5f;

	vecLookAt.x = 0.0f;
	vecLookAt.y = 0.0f;
	vecLookAt.z = 1.0f;
	
	D3DXMatrixLookAtLH( &matView, &vecPos, &vecLookAt, &D3DXVECTOR3(0.0f,1.0f,0.0f) );
	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
	
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	// 랜더링 
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	// CWorld::m_dwBgColor
	pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_COLORVALUE(50,50,50,0), 1.0f, 0 ) ;
	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_COLORVALUE(30,30,30,0), 1.0f, 0 ) ;
		
	::SetTransformView( matView );
	::SetTransformProj( matProj );
	SetDiffuse( 0.0f, 0.0f, 0.0f );
	SetAmbient( 1.0f, 1.0f, 1.0f );

	D3DXMatrixRotationY(&matRot, m_fAngle);
	m_matModel = matRot;
	D3DXMatrixScaling(&matScale, fScale, fScale, fScale);
	D3DXMatrixMultiply(&matWorld,&matWorld,&matScale);
	D3DXMatrixMultiply(&matWorld, &matWorld,&matRot);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans );
*/
/* 이펙트.. 문제가 좀 있어서 요구하기 전까지는 일단 보류
	if(m_pSfx) 
	{
		CMover* pPlayer	= CMover::GetActiveMover();
		D3DXVECTOR3 vAdd, vLeft;
		((CModelObject *)pPlayer->m_pModel)->GetForcePos( &m_pSfx->m_vPos, 0, PARTS_RWEAPON, matWorld);
		D3DXVec3Scale(&vLeft, &D3DXVECTOR3(m_matModel._11, m_matModel._21, m_matModel._31), 1.0f);
		D3DXVec3Normalize(&vLeft, &vLeft);
		D3DXVec3Scale(&vAdd, &vLeft, 0.9f);
		D3DXVec3Add(&m_pSfx->m_vPos, &m_pSfx->m_vPos, &vAdd);
		m_pSfx->m_vPos.y -= 0.2f;
		m_pSfx->m_matScale = matScale;
		m_pSfx->m_vScale  = D3DXVECTOR3(fScale, fScale, fScale);
		m_pSfx->m_vRotate = D3DXVECTOR3(D3DXToDegree(m_fAngle), D3DXToDegree(m_fAngle), D3DXToDegree(m_fAngle));
		if( m_pSfx->Process() )				m_pSfx->m_nCurFrame = 0;
		if( m_pSfx->m_nCurFrame >= 145 )	m_pSfx->m_nCurFrame = 30; 

		m_pSfx->Render(pd3dDevice, &matWorld);
	}
*//*
	matWorld._42 = 0.4f;
	if(m_pModel) m_pModel->Render(pd3dDevice, &matWorld);
	matWorld._42 = 0.0f;

	

	pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	
	m_fAngle += 0.01f;
	if(m_fAngle > 360.0f)
	m_fAngle -= 360.0f;
*/
#endif
}
 
void CCollectingWnd::OnDestroy()
{
	int a = 0;
}


void CCollectingWnd::AddSfx()
{
	/*
	if( !m_pSfx )
	{
		m_pSfx = new CSfxModel;

		if(!m_pSfxBase)
		{
			m_pSfxBase = new CSfxBase;
			m_pSfxBase->m_strName = "sfx_collect";
			m_pSfxBase->Load();
		}
		//m_pSfx->m_pSfxBase = m_pSfxBase;
		m_pSfx->SetSfx(m_pSfxBase);
		m_pSfx->m_vPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}*/
}

void CCollectingWnd::DeleteSfx()
{
	/*
	if(m_pSfxBase)
	{
		SAFE_DELETE(m_pSfxBase);
		m_pSfxBase = NULL;
	}
	if(m_pSfx)
	{
		SAFE_DELETE(m_pSfx);
		m_pSfx = NULL;
	}*/
}


HRESULT CCollectingWnd::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if(!m_pVBGauge)
	m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBGauge, NULL );
	
	return S_OK;
}

HRESULT CCollectingWnd::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	SAFE_RELEASE( m_pVBGauge );
	return S_OK;
}

HRESULT CCollectingWnd::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	return InvalidateDeviceObjects();
}

void CCollectingWnd::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
#if __VER >= 11 // __SYS_COLLECTING
	// 여기에 코딩하세요
	if(!g_pPlayer) return;
	CItemElem* pCollector = g_pPlayer->GetCollector();

	if(pCollector)
	{
		char		szTemp[64];
		//CWndStatic* pBettery = (CWndStatic*)GetDlgItem( WIDC_STATIC5 );
		CWndStatic* pLevel   = (CWndStatic*)GetDlgItem( WIDC_STATIC3 );
		CWndStatic* pPic1    = (CWndStatic*)GetDlgItem( WIDC_PIC1 );
		CWndStatic* pPic2    = (CWndStatic*)GetDlgItem( WIDC_PIC2 );
		LPWNDCTRL lpWndCtrl1 	= GetWndCtrl( WIDC_STATIC1 );
		LPWNDCTRL lpWndCtrl2 	= GetWndCtrl( WIDC_STATIC2 );
		LPWNDCTRL	lpGauge		= GetWndCtrl( WIDC_GAUGE );
		CRect 		rect		= GetClientRect();
		CRect 		rectTemp;
		int 		nWidthClient= lpGauge->rect.Width();
		int			nMax		= CCollectingProperty::GetInstance()->GetMaxBattery();
		int			nRatio		= pCollector->m_nHitPoint / nMax; 
		int			nWidth		= nWidthClient * nRatio;
		int			nBtryRemain = pCollector->m_nHitPoint;

		if(nBtryRemain < 0)  nBtryRemain = 0;
		m_BetteryRect	= lpWndCtrl1->rect;
		m_LevelRect		= lpWndCtrl2->rect;
		
		if(!m_pVBGauge)
			m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBGauge, NULL );

		m_pTexGauEmptyNormal = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "GauEmptyNormal.bmp" ), 0xffff00ff );
		m_pTexGauFillNormal  = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "GauEmptyNormal.bmp" ), 0xffff00ff );
		/*m_texGauEmptyNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptyNormal.bmp" ), 0xffff00ff, TRUE );
		m_texGauEmptySmall.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptySmall.bmp" ), 0xffff00ff, TRUE );
		m_texGauFillNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptyNormal.bmp" ), 0xffff00ff, TRUE );
		m_texGauFillSmall.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptySmall.bmp" ), 0xffff00ff, TRUE );
		*/
		wsprintf(m_pbufText, "%d / %d (%d%%)", nBtryRemain, nMax, nRatio * 100);
		
		m_nGWidth = nWidth;
		rect = lpGauge->rect;
		rectTemp = rect; 
		rectTemp.right = rectTemp.left + nWidth;
		ClientToScreen( rectTemp );
		//m_bVBGauge = m_pTheme->MakeGaugeVertex( m_pApp->m_pd3dDevice, &rect, 0x640010ff, m_pVBGauge, m_pTexGauFillNormal );

		memset(szTemp, 0, 64);
		sprintf(szTemp, "%d  /  5", pCollector->GetAbilityOption());
		pLevel->SetTitle(szTemp);
		m_pTexBatt  = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, _T( "Icon_ColBattery.tga" )), 0xffff00ff );
		m_pTexLevel = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, _T( "Icon_ColLevel.tga" )), 0xffff00ff );
		
		/*
		if(!m_pModel) m_pModel = new CModelObject;
		m_pModel = (CModelObject*)prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_ITEM, pCollector->m_dwItemId );
		m_pModel->InitDeviceObjects( g_Neuz.GetDevice() );*/
	}

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	
#endif
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CCollectingWnd::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	#if __VER >= 11 // __SYS_COLLECTING
		// Daisy에서 설정한 리소스로 윈도를 연다.
		return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_COLLECTING, 0, CPoint( 0, 0 ), pWndParent );
	#else
		return FALSE;
	#endif
} 
/*
  직접 윈도를 열때 사용 
BOOL CCollectingWnd::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/


void  CCollectingWnd::Update()
{
#if __VER >= 11 // __SYS_COLLECTING
	if(!g_pPlayer) return;
	CItemElem* pCollector = g_pPlayer->GetCollector();

	if(pCollector)
	{
		char		szTemp[64];
		CWndStatic* pLevel   = (CWndStatic*)GetDlgItem( WIDC_STATIC3 );

		memset(szTemp, 0, 64);
		sprintf(szTemp, "%d  /  5", pCollector->GetAbilityOption());
		pLevel->SetTitle(szTemp);
	}
#endif
}

BOOL CCollectingWnd::Process()
{
	return TRUE;
}


BOOL CCollectingWnd::SetButtonCaption(bool bIsStart)
{
#if __VER >= 11 // __SYS_COLLECTING
	CWndButton* pWndButton = (CWndButton*)GetDlgItem( WIDC_BUTTON1 );
	
	if(bIsStart)
		pWndButton->SetTexture(m_pParentWnd->m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtNormal00.tga" )), TRUE );
	else
	{
		if(::GetLanguage() == LANG_FRE)
			pWndButton->SetTexture(g_Neuz.m_pd3dDevice, MakePath( "Theme\\", ::GetLanguage(), _T( "ButtNormal00.tga" ) ), TRUE);
		else
			pWndButton->SetTexture(m_pParentWnd->m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtNormal00.tga" )), TRUE );
	}
#endif
	return true;

}

BOOL CCollectingWnd::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CCollectingWnd::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CCollectingWnd::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CCollectingWnd::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CCollectingWnd::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	#if __VER >= 11 // __SYS_COLLECTING
	if(!g_pPlayer) return FALSE;
	switch(nID)
	{
		case WIDC_BUTTON1 :
			m_bIsCollecting = !m_bIsCollecting;
			if(m_bIsCollecting)
			{
				if(!g_pPlayer->m_pActMover->IsFly())	
					g_DPlay.SendQueryStartCollecting();
				else
					g_WndMng.PutString( prj.GetText(TID_GAME_COLLECT_FLY), NULL, 0xffff0000 );
			}
			else				
				g_DPlay.SendQueryStopCollecting();
			break;
	};
	#endif
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

BOOL CCollectingWnd::OnEraseBkgnd(C2DRender* p2DRender)
{
	//CRect rect = GetClientRect();
	//p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	//p2DRender->RenderFillRect( rect, D3DCOLOR_ARGB( 100, 0, 0, 0 ) );

	return CWndBase::OnEraseBkgnd( p2DRender );
	
}


void CCollectingWnd::OnMouseWndSurface( CPoint point )
{
#if __VER >= 11 // __SYS_COLLECTING
	CRect DrawRect 		= m_BetteryRect;


	if( DrawRect.PtInRect( point ) )
	{
		CPoint point2 = point;
		CString strText;

		ClientToScreen( &point2 );
		ClientToScreen( &DrawRect );

#if __VER >= 11
		strText = prj.GetText(TID_GAME_COLLECTOR_BETTERY_INFO);
#endif	// 
		g_toolTip.PutToolTip( 100000, strText, DrawRect, point2 );

	}

	DrawRect 		= m_LevelRect;

	if( DrawRect.PtInRect( point ) )
	{
		CPoint point2 = point;
		CString strText;

		ClientToScreen( &point2 );
		ClientToScreen( &DrawRect );
#if __VER >= 11		
		strText = prj.GetText(TID_GAME_COLLECTOR_LEVEL_INFO);
#endif	// 
		g_toolTip.PutToolTip( 100000, strText, DrawRect, point2 );

	}
#endif
}
