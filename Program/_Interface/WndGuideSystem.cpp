// GuideSystem.cpp: implementation of the CGuideSystem class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "define.h"
#include "definetext.h"
#include "defineSound.h"
#include "resdata.h"
#include "WndGuideSystem.h"
#include "dpclient.h"

extern	CDPClient	g_DPlay;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWndGuideSystem::CWndGuideSystem()
{
	m_bIsLoad = FALSE;
	m_pModel = NULL;

	m_wndGuideText = NULL;
	m_wndGuideSelection = NULL;
#if __VER >= 12 // __MOD_TUTORIAL
	m_pWndTutorialView = NULL;
	m_CurrentGuide.init();
	m_vecEventGuide.clear();
	m_mapGuide.clear();
	m_bIsViewVisible = false;
#endif
}

CWndGuideSystem::~CWndGuideSystem()
{
	m_pModel->DeleteDeviceObjects();

	SAFE_DELETE(m_pModel);
	m_wndGuideText->Destroy();
	SAFE_DELETE(m_wndGuideText);
	m_wndGuideSelection->Destroy();
	SAFE_DELETE(m_wndGuideSelection);
#if __VER >= 12 // __MOD_TUTORIAL
	m_pWndTutorialView->Destroy();
	SAFE_DELETE(m_pWndTutorialView);
#endif
}

BOOL CWndGuideSystem::Initialize( CWndBase* pWndParent, DWORD nType )
{	
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUIDE, WBS_TOPMOST, CPoint( 0, 0 ), pWndParent );
}

void CWndGuideSystem::OnDraw( C2DRender* p2DRender )
{
	if( m_pModel == NULL )
		return;

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
	
	CRect rect = GetClientRect();

	// 뷰포트 세팅 
	D3DVIEWPORT9 viewport;

	// 월드 
	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 matScale;
	D3DXMATRIXA16 matRot;
	D3DXMATRIXA16 matTrans;

	// 카메라 
	D3DXMATRIX  matView;
	D3DXVECTOR3 vecLookAt( 0.0f, 0.0f, 3.0f );
	D3DXVECTOR3 vecPos(  0.0f, 0.7f, -5.5f );
	
	D3DXMatrixLookAtLH( &matView, &vecPos, &vecLookAt, &D3DXVECTOR3(0.0f,1.0f,0.0f) );
	
	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
	
	{
		D3DXMatrixIdentity(&matScale);
		D3DXMatrixIdentity(&matTrans);
		D3DXMatrixIdentity(&matWorld);

		CRect rect = GetClientRect();

		viewport.X      = p2DRender->m_ptOrigin.x + rect.left;//2;
		viewport.Y      = p2DRender->m_ptOrigin.y + rect.top;//5;
		viewport.Width  = rect.Width();
		viewport.Height = rect.Height();

		viewport.MinZ   = 0.0f;
		viewport.MaxZ   = 1.0f;
		pd3dDevice->SetViewport(&viewport);
		pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xffa08080, 1.0f, 0 ) ;

		D3DXMATRIX matProj;
		D3DXMatrixIdentity( &matProj );
		FLOAT fAspect = ((FLOAT)viewport.Width) / (FLOAT)viewport.Height;
/*		
		D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4.0f, fAspect, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
		pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
*/		
		FLOAT fov = D3DX_PI/4.0f;
		FLOAT h = cos(fov/2) / sin(fov/2);
		FLOAT w = h * fAspect;
		D3DXMatrixOrthoLH( &matProj, w, h, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
		pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
		
		D3DXMatrixScaling(&matScale, 2.8f, 2.8f, 2.8f);
		
		D3DXMatrixTranslation(&matTrans,0.0f,-0.8f,0.0f);

		D3DXMatrixMultiply(&matWorld,&matWorld,&matScale);
		D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans );
		pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

		// 랜더링 
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );//m_bViewLight );
		
		::SetLight( FALSE );
		::SetFog( FALSE );
		SetDiffuse( 1.0f, 1.0f, 1.0f );
		SetAmbient( 1.0f, 1.0f, 1.0f );

		D3DXVECTOR4 vConst( 1.0f, 1.0f, 1.0f, 1.0f );
		
#ifdef __YENV	
		D3DXVECTOR3 vDir( 0.0f, 0.0f, 1.0f );
		SetLightVec( vDir );
	
		g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvFog, &vConst );
#else //__YENV						
		pd3dDevice->SetVertexShaderConstantF( 95, (float*)&vConst, 1 );
#endif //__YENV
		
		::SetTransformView( matView );		
		::SetTransformProj( matProj );
		m_pModel->Render( p2DRender->m_pd3dDevice, &matWorld );
	}
	
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
}					
void CWndGuideSystem::PaintFrame( C2DRender* p2DRender )
{
}
void CWndGuideSystem::ChangeModel( int nJob )
{
	SAFE_DELETE(m_pModel);
	m_pModel = new CModelObject;
	m_pModel->InitDeviceObjects(g_Neuz.GetDevice());

	if( m_pModel )
	{
		switch( nJob )
		{
			default:
			case JOB_VAGRANT:
				{
					m_pModel->LoadBone( "Mvr_Guidepang.chr" );
					m_pModel->LoadElement("Mvr_Guidepang.o3d", 0 );
				}
				break;
			case JOB_KNIGHT:
			case JOB_BLADE:
			case JOB_MERCENARY:
				{
					m_pModel->LoadBone( "Mvr_McGuidepang.chr" );
					m_pModel->LoadElement("Mvr_McGuidepang.o3d", 0 );
				}
				break;
			case JOB_RINGMASTER:
			case JOB_BILLPOSTER:
			case JOB_ASSIST:
				{
					m_pModel->LoadBone( "Mvr_AsGuidepang.chr" );
					m_pModel->LoadElement("Mvr_AsGuidepang.o3d", 0 );
				}
				break;
			case JOB_PSYCHIKEEPER:
			case JOB_ELEMENTOR:
			case JOB_MAGICIAN:
				{
					m_pModel->LoadBone( "Mvr_MgGuidepang.chr" );
					m_pModel->LoadElement("Mvr_MgGuidepang.o3d", 0 );
				}
				break;
			case JOB_ACROBAT:
			case JOB_RANGER:
			case JOB_JESTER:
				{
					m_pModel->LoadBone( "Mvr_AcrGuidepang.chr" );
					m_pModel->LoadElement("Mvr_AcrGuidepang.o3d", 0 );
				}
				break;				
		}
	}
}	

void CWndGuideSystem::SetAni(int nJob, int nAniKind)
{
	if( m_pModel == NULL )
		return;
		
	CString filename;
	char* VagArry[3] = { "Mvr_Guidepang_Appear.ani", "Mvr_Guidepang_Default.ani", "Mvr_Guidepang_Disappear.ani"	};
	char* MerArry[3] = { "Mvr_McGuidepang_appear.ani", "Mvr_McGuidepang_default.ani", "Mvr_McGuidepang_Disappear.ani"	};
	char* AssArry[3] = { "Mvr_AsGuidepang_Appear.ani", "Mvr_AsGuidepang_Default.ani", "Mvr_AsGuidepang_Disappear.ani"	};
	char* MagArry[3] = { "Mvr_MgGuidepang_Appear.ani", "Mvr_MgGuidepang_Dafault.ani", "Mvr_MgGuidepang_DisAppear.ani"	};
	char* AcrArry[3] = { "Mvr_AcrGuidepang_Appear.ani", "Mvr_AcrGuidepang_Default.ani", "Mvr_AcrGuidepang_DisAppear.ani"	};
	
	switch( nJob )
	{
	default:
	case JOB_VAGRANT:
		{
			filename = VagArry[nAniKind];
		}
		break;
	case JOB_KNIGHT:
	case JOB_BLADE:
	case JOB_MERCENARY:
		{
			filename = MerArry[nAniKind];
		}
		break;
	case JOB_RINGMASTER:
	case JOB_BILLPOSTER:
	case JOB_ASSIST:
		{
			filename = AssArry[nAniKind];
		}
		break;
	case JOB_PSYCHIKEEPER:
	case JOB_ELEMENTOR:
	case JOB_MAGICIAN:
		{
			filename = MagArry[nAniKind];
		}
		break;
	case JOB_ACROBAT:
	case JOB_RANGER:
	case JOB_JESTER:
		{
			filename = AcrArry[nAniKind];
		}
		break;
		
	}
	
	m_pModel->LoadMotion( filename );
	
	switch(nAniKind)
	{
	case ANI_INTRO:
		{
			m_pModel->SetMotionBlending( TRUE );
			m_pModel->SetLoop( ANILOOP_CONT );
			m_bAniState = ANI_INTRO;					
		}
		break;
	case ANI_IDLE:
		{
			m_pModel->SetMotionBlending( TRUE );
			m_pModel->SetLoop( ANILOOP_LOOP );
			m_bAniState = ANI_IDLE;	
		}
		break;
	case ANI_BYTE:
		{
			m_pModel->SetMotionBlending( TRUE );
			m_pModel->SetLoop( ANILOOP_CONT );
			m_bAniState = ANI_BYTE;	
		}
		break;
	}
}
#if __VER >= 12 // __MOD_TUTORIAL

//================================================================================
void CWndInfoPang::OnInitialUpdate()
{
	CWndGuideSystem* pWndGuide = (CWndGuideSystem*)GetWndBase( APP_GUIDE );
	if(pWndGuide->m_dwGuideLevel == 0)
	{
		pWndGuide->m_bVisible = true;
		pWndGuide->ChangeModel( g_pPlayer->GetJob() );
		pWndGuide->SetAni( g_pPlayer->GetJob(), 0 );
	}
}


CWndInfoPang::~CWndInfoPang() 
{ 
} 
void CWndInfoPang::OnDraw( C2DRender* p2DRender ) 
{ 
} 

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndInfoPang::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_INFOPANG, 0, CPoint( 0, 0 ), pWndParent );
	SetVisible(FALSE);
	return TRUE;
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndMap::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndInfoPang::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndInfoPang::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndInfoPang::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndInfoPang::OnRButtonUp( UINT nFlags, CPoint point ) 
{ 
	Destroy();
} 
void CWndInfoPang::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndInfoPang::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

//================================================================================

BOOL CWndGuideSystem::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	CWndBase* pWndBase = (CWndBase*) pLResult;
	
	if( pWndBase->m_pParentWnd == &m_wndMenuPlace )
	{
		int nState = !m_wndMenuPlace.GetMenuState( nID, 0 );
		m_wndMenuPlace.CheckMenuItem( nID, nState );
		m_wndMenuPlace.CheckMenuItem( 0, FALSE );
		m_wndMenuPlace.CheckMenuItem( 1, FALSE );
		m_wndMenuPlace.CheckMenuItem( 2, FALSE );
		SetFocus();
		switch( nID )
		{
		case 0: // 숨기기 
			{
				if(g_pPlayer)
					SetAni( g_pPlayer->GetJob(), ANI_BYTE );
				//Destroy();
				
				if( m_wndGuideText->m_VecGuideText.size() )
					m_wndGuideText->m_bVisible = FALSE;

				m_pWndTutorialView->SetVisible(FALSE);
				
			}
			break;
		case 1: // 보기
			if( m_wndGuideText->m_VecGuideText.size() )
				m_wndGuideText->m_bVisible = !m_wndGuideText->m_bVisible;

			if(m_wndGuideText->m_bVisible) 
			{
				if(m_CurrentGuide.m_nEventMsg != 0) m_wndGuideText->AddGuideText(m_CurrentGuide);
			}
			break;
		case 2:
			{
				SAFE_DELETE(m_pWndTutorialView);
				m_pWndTutorialView = new CWndTutorial;
				m_pWndTutorialView->Initialize(this);
				//m_bIsViewVisible = !m_bIsViewVisible;
				//m_pWndTutorialView->m_bVisible = m_bIsViewVisible;
			}
			break;
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

void CWndGuideSystem::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_bIsLoad = FALSE;

	this->DelWndStyle(WBS_CAPTION);
	this->AddWndStyle(WBS_TOPMOST);

	SAFE_DELETE(m_pModel);

	if( g_pPlayer )
	{
		ChangeModel( g_pPlayer->GetJob() );
		SetAni( g_pPlayer->GetJob(), ANI_INTRO );
	}
	else
		Error( "CWndGuideSystem::OnInitialUpdate() -> g_pPlayer == NULL " );

	m_wndTitleBar.SetVisible( FALSE );

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rect = GetWindowRect();
	int nWidth  = rect.Width(); 
	int nHeight = rect.Height(); 
	int x = rectRoot.right - rect.Width();
	int y = rectRoot.bottom - nHeight;	

	CPoint point( x, y );
	Move( point );
	
	LoadGuide( MakePath( DIR_CLIENT, "Guide.inc" ) );
	//*
	//텍스트 창 출력
	SAFE_DELETE(m_wndGuideText);
	m_wndGuideText = new CWndGuideTextMgr;
#ifdef __FIX_WND_1109
	m_wndGuideText->Initialize( this );
#else	// __FIX_WND_1109
	m_wndGuideText->Initialize();
#endif	// __FIX_WND_1109
	
	SAFE_DELETE(m_pWndTutorialView);
	m_pWndTutorialView = new CWndTutorial;
	m_pWndTutorialView->Initialize(this);
	m_pWndTutorialView->SetVisible(FALSE);

	m_bIsGuideChart[0] = FALSE;
	m_bIsGuideChart[1] = FALSE;
	
	m_dwTime = g_tmCurrent;

	m_wndMenuPlace.CreateMenu( this );	
	m_wndMenuPlace.AppendMenu( 0, 0 , prj.GetText(TID_GAME_GUIDE_HIDE) );
	m_wndMenuPlace.AppendMenu( 0, 1 , prj.GetText(TID_GAME_GUIDE_OPEN) );
	m_wndMenuPlace.AppendMenu( 0, 2 , prj.GetText(TID_GAME_VIEW_TUTORIAL) );
	m_wndMenuPlace.CheckMenuItem( 0, FALSE );
	m_wndMenuPlace.CheckMenuItem( 1, m_wndGuideText->m_bVisible );
	m_wndMenuPlace.CheckMenuItem( 2, FALSE );
	
	m_dwGuideLevel = 1;
}

void CWndGuideSystem::PushBack(GUIDE_STRUCT guide)
{
}

void CWndGuideSystem::SendGuideMessage( int nMsg )
{
}

BOOL CWndGuideSystem::Process()
{

	if( !(g_tmCurrent > m_dwTime+SEC(2.5)) )
		return TRUE;

	if( m_pModel == NULL )
		return FALSE;
	
	// 여기부턴 인포팡 모델 처리
	m_pModel->FrameMove();
	
	if( m_pModel->IsEndFrame() )
	{
		if( m_bAniState == ANI_INTRO )
		{
			if( g_pPlayer )
				SetAni( g_pPlayer->GetJob(), ANI_IDLE );
		}
		else
		if( m_bAniState == ANI_BYTE )
		{
			m_bVisible     = FALSE;
			m_bAniState    = ANI_IDLE;
		}
	}

	// 여기부터 가이드 스크립트 처리
	if(m_CurrentGuide.m_nVicCondition)
	{
		// 대상 가이드가 있으면 가이드의 완료 조건을 체크한다
		if(CheckCompletion(m_CurrentGuide))
		{
			if(m_CurrentIter == m_mapGuide.end())
			{
				// 마지막가이드이면 종료처리한다
				m_wndGuideText->SetVisible(FALSE);
				g_Option.m_nTutorialLv = m_CurrentGuide.m_nLevel;
				g_DPlay.SendTutorialState(g_Option.m_nTutorialLv);
				if(g_Option.m_nTutorialLv > 0)
					m_pWndTutorialView->AddToList(g_Option.m_nTutorialLv - 1);
				m_CurrentGuide.init();
				m_CurrentIter = NULL;
			}
			else if(m_CurrentIter == NULL)
			{
				// 이벤트성(일회성) 이벤트일 경우는 다음이 없다
				m_wndGuideText->SetVisible(FALSE);
				if(m_CurrentGuide.m_nLevel > g_Option.m_nTutorialLv)
				{
					g_Option.m_nTutorialLv = m_CurrentGuide.m_nLevel;
					//if(g_Option.m_nTutorialLv > 0)
					//	m_pWndTutorialView->AddToList(g_Option.m_nTutorialLv - 1);
				}
				m_CurrentGuide.init();
			}
			else
			{
				// 마지막이 아니면 다음 가이드로
				++m_CurrentIter;
				if(m_CurrentIter != m_mapGuide.end()) 
				{
					m_CurrentGuide = m_CurrentIter->second;
					m_wndGuideText->AddGuideText(m_CurrentGuide);
					if(m_CurrentGuide.m_nLevel > g_Option.m_nTutorialLv + 1)
					{
						g_Option.m_nTutorialLv = m_CurrentGuide.m_nLevel - 1;
						g_DPlay.SendTutorialState(g_Option.m_nTutorialLv);
						if(g_Option.m_nTutorialLv > 0)
							m_pWndTutorialView->AddToList(g_Option.m_nTutorialLv - 1);
					}
				}
				else
				{
					
					m_wndGuideText->SetVisible(FALSE);
					g_Option.m_nTutorialLv = m_CurrentGuide.m_nLevel;
					g_DPlay.SendTutorialState(g_Option.m_nTutorialLv);
					if(g_Option.m_nTutorialLv > 0)
						m_pWndTutorialView->AddToList(g_Option.m_nTutorialLv - 1);
					m_CurrentIter = NULL;
					m_CurrentGuide.init();
				}
			}
		}
	}

	m_Condition.Init();

	return TRUE;

}

BOOL CWndGuideSystem::PassToNext()
{
	if(!m_CurrentGuide.m_nVicCondition)
	{
		if(m_CurrentIter == m_mapGuide.end())
		{
			// 마지막가이드이면 종료처리한다
			
			m_wndGuideText->SetVisible(FALSE);
			g_Option.m_nTutorialLv = m_CurrentGuide.m_nLevel;
			g_DPlay.SendTutorialState(g_Option.m_nTutorialLv);
			if(g_Option.m_nTutorialLv > 0)
				m_pWndTutorialView->AddToList(g_Option.m_nTutorialLv - 1);
			m_CurrentGuide.init();
			m_CurrentIter = NULL;
			return FALSE;
		}
		else if(m_CurrentIter == NULL)
		{
			// 이벤트성(일회성) 이벤트일 경우는 다음이 없다
			m_wndGuideText->SetVisible(FALSE);
			if(m_CurrentGuide.m_nLevel > g_Option.m_nTutorialLv)
			{
				g_Option.m_nTutorialLv = m_CurrentGuide.m_nLevel;
				//if(g_Option.m_nTutorialLv > 0)
				//	m_pWndTutorialView->AddToList(g_Option.m_nTutorialLv - 1);
			}
			m_CurrentGuide.init();
		}
		else
		{
			// 마지막이 아니면 다음 가이드로
			++m_CurrentIter;
			if(m_CurrentIter != m_mapGuide.end()) 
			{
				m_CurrentGuide = m_CurrentIter->second;
				m_wndGuideText->AddGuideText(m_CurrentGuide);
				if(m_CurrentGuide.m_nLevel > g_Option.m_nTutorialLv + 1)
				{
					g_Option.m_nTutorialLv = m_CurrentGuide.m_nLevel - 1;
					g_DPlay.SendTutorialState(g_Option.m_nTutorialLv);
					if(g_Option.m_nTutorialLv > 0)
						m_pWndTutorialView->AddToList(g_Option.m_nTutorialLv - 1);
				}
			}
			else
			{
				m_wndGuideText->SetVisible(FALSE);
				g_Option.m_nTutorialLv = m_CurrentGuide.m_nLevel;
				g_DPlay.SendTutorialState(g_Option.m_nTutorialLv);
				if(g_Option.m_nTutorialLv > 0)
					m_pWndTutorialView->AddToList(g_Option.m_nTutorialLv - 1);
				m_CurrentIter = NULL;
				m_CurrentGuide.init();
			}
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CWndGuideSystem::CheckCompletion(GUIDE_STRUCT Guide)
{
	switch(Guide.m_nVicCondition)
	{
		case CAMERA_ROTATION:
			if(m_Condition.bIsCamMove) return TRUE;
			break;

		case CAMERA_ZOOMED:
			if(m_Condition.bIsCamZoomed) return TRUE;
			break;

		case INPUT_KEY:
			if(Guide.m_nInput)
			{
				if(m_Condition.nInputKey == Guide.m_nInput) return TRUE;
			}
			break;

		case INPUT_STRING:
			if(!Guide.m_strInput.IsEmpty())
			{
				if(m_Condition.strInput.Find(Guide.m_strInput) >= 0) return TRUE;
				else if(m_Condition.strInput.Compare(Guide.m_strInput) == 0) return TRUE;
			}
			break;
		case MOVE_ON_MOUSE:
			if(m_Condition.bIsClickOnLand) return TRUE;
			break;

		case MOVE_ON_KEY: 
			if(m_Condition.bIsKeyMove) return TRUE;
			break;
		case OPEN_WINDOW:
			if(m_Condition.nOpenedWindowID == 1) return TRUE;
			break;
	};

	return FALSE;
}

BOOL CWndGuideSystem::LoadGuide( LPCTSTR lpszFileName )
{
	CScript script;
	if( script.Load( lpszFileName ) == FALSE )
		return FALSE;

	if(m_mapGuide.size() || m_vecEventGuide.size()) return FALSE;

	GUIDE_STRUCT guidestruct;

	script.tok = 0;
	
	while( script.tok != FINISHED )
	{
		guidestruct.init();
		guidestruct.m_nEventMsg = script.GetNumber();

		script.GetToken(); // {
		if( *script.token  == '{' )
		{
			script.GetToken(); // BEGINNER
			guidestruct.m_bBeginner = script.GetNumber();
			script.GetToken(); // SHOWLEVEL
			guidestruct.m_nShowLevel = script.GetNumber();
			script.GetToken(); // FLAG
			guidestruct.m_bFlag      = script.GetNumber();
			script.GetToken(); // KEY
			guidestruct.m_nkey = script.GetNumber();
			guidestruct.m_str = "";
			script.GetToken();
			guidestruct.m_str	= script.Token;
			script.GetToken();
			if( strcmpi( script.token, "SEQUENCE" ) == 0 )
			{
				guidestruct.m_nSequence = script.GetNumber();
				script.GetToken();
			}
			if( strcmpi( script.token, "LEVEL" ) == 0 )
			{
				guidestruct.m_nLevel = script.GetNumber();
				script.GetToken();
			}
			if( strcmpi( script.token, "CONDITION" ) == 0 )
			{
				guidestruct.m_nVicCondition = script.GetNumber();
				script.GetToken();
			}
			if( strcmpi( script.token, "INPUTCHAR" ) == 0 )
			{
				guidestruct.m_nInput = script.GetNumber();
				script.GetToken();
			}
			if( strcmpi( script.token, "INPUTSTR" ) == 0 )
			{
				script.GetToken();
				guidestruct.m_strInput = script.Token;
				script.GetToken();	// }
				while(*script.token  != '}')
				{
					guidestruct.m_strInput.Format("%s%s", guidestruct.m_strInput, script.token);
					script.GetToken();	// }
				}
			}
			if(guidestruct.m_nSequence != 0)
				m_mapGuide.insert(mgValType(guidestruct.m_nSequence, guidestruct));
			else
				m_vecEventGuide.push_back(guidestruct);
		}
		else
			script.GetToken();
	}
	
	m_bIsLoad = TRUE;
	
	return TRUE;
}

void CWndGuideSystem::GuideStart(BOOL ischart)
{

	if(m_dwGuideLevel) return;

	// 이벤트성 가이드일 경우
	if(ischart)
	{
		if(m_vecEventGuide.size())
		{
			for( vector<GUIDE_STRUCT>::iterator iter = m_vecEventGuide.begin(); iter != m_vecEventGuide.end(); ++iter )
			{
				GUIDE_STRUCT guidestruct;
				guidestruct = (*iter);
				if(guidestruct.m_nEventMsg == ischart)
				{
					m_CurrentGuide = guidestruct;
					m_wndGuideText->AddGuideText(m_CurrentGuide);
					m_CurrentIter = NULL;
				}
			}
		}
	}
	else
	{	
		// 순차적으로 실행되는 기본가이드를 시작한다
		if(m_mapGuide.size())
		{
			mgMapItor Iter = m_mapGuide.begin();

			while(Iter != m_mapGuide.end())
			{
				m_CurrentIter = Iter;
				m_CurrentGuide = (Iter->second);
				if(g_Option.m_nTutorialLv + 1 <= m_CurrentGuide.m_nLevel) 
				{
					m_wndGuideText->AddGuideText(m_CurrentGuide);
					return;
				}
				++Iter;
			}
			// 튜토리얼을 다 완료했으면 튜토리얼창이 보이지 않는다.
			SetVisible(FALSE);
		}
	}
	
}
#else
BOOL CWndGuideSystem::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	CWndBase* pWndBase = (CWndBase*) pLResult;
	
	if( pWndBase->m_pParentWnd == &m_wndMenuPlace )
	{
		int nState = !m_wndMenuPlace.GetMenuState( nID, 0 );
		m_wndMenuPlace.CheckMenuItem( nID, nState );
		m_wndMenuPlace.CheckMenuItem( 0, FALSE );
		m_wndMenuPlace.CheckMenuItem( 1, FALSE );
		
		SetFocus();
		switch( nID )
		{
		case 0:
			{
				if(g_pPlayer)
					SetAni( g_pPlayer->GetJob(), ANI_BYTE );

				if( m_wndGuideText->m_VecGuideText.size() )
					m_wndGuideText->m_bVisible = FALSE;
				
			}
			break;
		case 1:
			if( m_wndGuideText->m_VecGuideText.size() )
				m_wndGuideText->m_bVisible = !m_wndGuideText->m_bVisible;
			break;
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

void CWndGuideSystem::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_bIsLoad = FALSE;

	this->DelWndStyle(WBS_CAPTION);
	this->AddWndStyle(WBS_TOPMOST);

	SAFE_DELETE(m_pModel);

	if( g_pPlayer )
	{
		ChangeModel( g_pPlayer->GetJob() );
		SetAni( g_pPlayer->GetJob(), ANI_INTRO );
	}
	else
		Error( "CWndGuideSystem::OnInitialUpdate() -> g_pPlayer == NULL " );

	m_wndTitleBar.SetVisible( FALSE );

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rect = GetWindowRect();
	int nWidth  = rect.Width(); 
	int nHeight = rect.Height(); 
	int x = rectRoot.right - rect.Width();
	int y = rectRoot.bottom - nHeight;	
	CPoint point( x, y );
	Move( point );

	m_listGuide.clear();
	m_listGuideMsg.clear();
	m_listGuideChart.clear();

	LoadGuide( MakePath( DIR_CLIENT, "Guide.inc" ) );
	//*
	//텍스트 창 출력
	SAFE_DELETE(m_wndGuideText);
	m_wndGuideText = new CWndGuideTextMgr;
#ifdef __FIX_WND_1109
	m_wndGuideText->Initialize( this );
#else	// __FIX_WND_1109
	m_wndGuideText->Initialize();
#endif	// __FIX_WND_1109
	/**/

	m_bIsGuideChart[0] = FALSE;
	m_bIsGuideChart[1] = FALSE;
	
	m_dwTime = g_tmCurrent;

	m_wndMenuPlace.CreateMenu( this );	
	m_wndMenuPlace.AppendMenu( 0, 0 , prj.GetText(TID_GAME_GUIDE_HIDE) );
	m_wndMenuPlace.AppendMenu( 0, 1 , prj.GetText(TID_GAME_GUIDE_OPEN) );
	m_wndMenuPlace.CheckMenuItem( 0, FALSE );
	m_wndMenuPlace.CheckMenuItem( 1, m_wndGuideText->m_bVisible );
	
	m_dwGuideLevel = 1;
}

void CWndGuideSystem::PushBack(GUIDE_STRUCT guide)
{
	m_listGuide.push_back( guide );

	GUIDE_STRUCT* pguide = &(m_listGuide.back());
	
	m_listGuideChart.push_back(pguide);
}

void CWndGuideSystem::SendGuideMessage( int nMsg )
{
	if( m_bAniState != ANI_IDLE )
		return;

	if( m_dwGuideLevel != 2 )
		m_listGuideMsg.push_back( nMsg );
}

BOOL CWndGuideSystem::Process()
{
//	return 1;
	if( !(g_tmCurrent > m_dwTime+SEC(2.5)) )
		return TRUE;

	if( m_pModel == NULL )
		return FALSE;
	
	m_pModel->FrameMove();
	
	if( m_pModel->IsEndFrame() )
	{
		if( m_bAniState == ANI_INTRO )
		{
			if( g_pPlayer )
				SetAni( g_pPlayer->GetJob(), ANI_IDLE );
		}
		else
		if( m_bAniState == ANI_BYTE )
		{
			m_bVisible     = FALSE;
			m_bAniState    = ANI_IDLE;
		}
	}
	
	int nMsg;

	if( m_bIsGuideChart[0] )
	{	
		GUIDE_STRUCT* guidestruct;

		if( m_listGuideMsg.size() )
		{
			nMsg = m_listGuideMsg.front();

			if( m_listGuideChart.size() )
			{
				guidestruct = m_listGuideChart.front();

				if( guidestruct->m_nEventMsg == nMsg )
				{
					m_listGuideChart.pop_front();
					m_listGuideMsg.pop_front();

					// 새로운 가이드 추가
					if( m_listGuideChart.size() )
					{
						guidestruct = m_listGuideChart.front();
						m_wndGuideText->AddGuideText( *guidestruct );
					}
					else
					{
						m_wndGuideText->m_bVisible = FALSE;
						m_bIsGuideChart[0] = FALSE;
						m_bIsGuideChart[1] = TRUE;
					}
				}
				else
					m_listGuideMsg.pop_front();
			}
		}
	}
	else
	if( m_bIsGuideChart[1] )
	{
		if( m_listGuideMsg.size() )
		{
			nMsg = m_listGuideMsg.front();

			for( list<GUIDE_STRUCT>::iterator i = m_listGuide.begin(); i != m_listGuide.end(); ++i )
			{
				GUIDE_STRUCT guidestruct;

				guidestruct = (*i);

				if( (*i).m_nEventMsg == nMsg && (*i).m_bBeginner == FALSE  )
				{
					if( g_pPlayer && g_pPlayer->GetLevel() == (*i).m_nShowLevel )
					{
						(*i).m_bFlag = !(*i).m_bFlag;

						m_listGuideMsg.pop_front();
						m_wndGuideText->AddGuideText( (*i) );
					}
				}
			}

			if( m_listGuideMsg.size() )
				m_listGuideMsg.pop_front();
		}
		
	}

	return TRUE;
}

BOOL CWndGuideSystem::LoadGuide( LPCTSTR lpszFileName )
{
	CScript script;
	if( script.Load( lpszFileName ) == FALSE )
		return FALSE;

	GUIDE_STRUCT guidestruct;
	
	m_listGuide.clear();
	m_listGuideChart.clear();
	
	script.tok = 0;
	
	while( script.tok != FINISHED )
	{
		guidestruct.m_nEventMsg = script.GetNumber();

		script.GetToken(); // {
		if( *script.token  == '{' )
		{
			script.GetToken(); // BEGINNER
			guidestruct.m_bBeginner = script.GetNumber();
			script.GetToken(); // SHOWLEVEL
			guidestruct.m_nShowLevel = script.GetNumber();
			script.GetToken(); // FLAG
			guidestruct.m_bFlag      = script.GetNumber();
			script.GetToken(); // KEY
			guidestruct.m_nkey = script.GetNumber();
			guidestruct.m_str = "";
			script.GetToken();
			guidestruct.m_str	= script.Token;
			script.GetToken();	// }
			PushBack(guidestruct);
		}
		else
			script.GetToken();
	}
	
	m_bIsLoad = TRUE;
	
	return TRUE;
}

void CWndGuideSystem::GuideStart(BOOL ischart)
{
	if( ischart )
	{
		m_bIsGuideChart[0] = TRUE;
		m_bIsGuideChart[1] = FALSE;
		GUIDE_STRUCT* guidestruct = m_listGuideChart.front();
		m_wndGuideText->AddGuideText( *guidestruct );
	}
	else
	{
		m_bIsGuideChart[1] = TRUE;
		m_bIsGuideChart[0] = FALSE;
	}
}
#endif


BOOL CWndGuideSystem::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndGuideSystem::OnSize( UINT nType, int cx, int cy )
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndGuideSystem::OnLButtonUp( UINT nFlags, CPoint point )
{
//	m_wndGuideText->UpDate();
//	m_wndGuideText->m_bVisible = FALSE;
}
void CWndGuideSystem::OnLButtonDown( UINT nFlags, CPoint point )
{
	m_wndMenuPlace.SetVisible( FALSE );
	m_wndGuideText->m_bVisible = FALSE;
	
	if( IsPickupSpace(point) )
	{
		m_bPickup = TRUE;
	}
}
void CWndGuideSystem::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	m_wndGuideText->UpDate();
}
void CWndGuideSystem::OnRButtonDown( UINT nFlags, CPoint point )
{
	if(m_bAniState == ANI_BYTE)
		return;

	CRect rect = GetWindowRect( TRUE );
	CRect menurect = m_wndMenuPlace.GetWindowRect();

	if( rect.left - m_wndMenuPlace.GetWindowRect().Width() < m_wndMenuPlace.GetWindowRect().Width() )
		m_wndMenuPlace.Move( CPoint( rect.left + m_wndMenuPlace.GetWindowRect().Width(), rect.top ) );
	else
		m_wndMenuPlace.Move( CPoint( rect.left - m_wndMenuPlace.GetWindowRect().Width(), rect.top ) );

	m_wndMenuPlace.SetVisible( !m_wndMenuPlace.IsVisible() );
	m_wndMenuPlace.SetFocus();	
}




void CWndGuideSystem::CreateGuideSelection()
{
	SAFE_DELETE(m_wndGuideSelection);
	m_wndGuideSelection = new CWndGuideSelection;
#ifdef __FIX_WND_1109
	m_wndGuideSelection->Initialize( this );
#else	// __FIX_WND_1109
	m_wndGuideSelection->Initialize();
#endif	// __FIX_WND_1109
}





////////////////////////////////////////////////////////////////////////////////////
CWndGuideTextMgr::CWndGuideTextMgr() 
{ 
	m_pTextureBG = NULL;
	SetPutRegInfo( FALSE );
} 
CWndGuideTextMgr::~CWndGuideTextMgr() 
{ 
} 
void CWndGuideTextMgr::OnDraw( C2DRender* p2DRender ) 
{ 
	CWndGuideSystem* pWndGuide = (CWndGuideSystem*)GetWndBase( APP_GUIDE );
	
	if( !pWndGuide )
		return;
} 
void CWndGuideTextMgr::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	DelWndStyle(WBS_MOVE);
	AddWndStyle(WBS_TOPMOST);
	m_wndTitleBar.SetVisible( FALSE );

	m_bVisible = FALSE;

	CWndText* pWndText;
	CWndButton* pWndButton;
    pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	m_Rect[0] = pWndText->GetWndRect();
	pWndButton = (CWndButton*)GetDlgItem( WIDC_BACK );
	m_Rect[1] = pWndButton->GetWndRect();
#if __VER >= 12 // __MOD_TUTORIAL
	pWndButton->SetVisible(FALSE);
#endif
	pWndButton  = (CWndButton*)GetDlgItem( WIDC_NEXT );	
	m_Rect[2] = pWndButton->GetWndRect();
	m_Rect[3] = GetWndRect();

	m_nCurrentVector = 0;
	m_VecGuideText.clear();	

	m_pTextureBG = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "GuideBG.tga" ), 0, TRUE );
} 

BOOL CWndGuideTextMgr::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUIDE_TEXT, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndGuideTextMgr::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndGuideTextMgr::OnSize( UINT nType, int cx, int cy )
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndGuideTextMgr::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndGuideTextMgr::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
#if __VER >= 12 // __MOD_TUTORIAL
BOOL CWndGuideTextMgr::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	GUIDE_STRUCT guide;
	switch( nID )
	{
		case WIDC_NEXT:
			{
				/*
				if( m_VecGuideText.size() )
				{
					m_nCurrentVector++;

					if( m_nCurrentVector > m_VecGuideText.size()-1 )
						m_nCurrentVector = m_VecGuideText.size()-1;

					guide = m_VecGuideText[m_nCurrentVector];
					_SetGuideText(guide);
					
					CWndWorld* pWndWorld = (CWndWorld*)GetWndBase( APP_WORLD );
				}*/
				CWndGuideSystem* pWndGuide = (CWndGuideSystem*)GetWndBase( APP_GUIDE );
				pWndGuide->PassToNext();
			}
			break;
		default:
			{
				m_bVisible = FALSE;
				CWndWorld* pWndWorld = (CWndWorld*)GetWndBase( APP_WORLD );
			}
			break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}
#else
BOOL CWndGuideTextMgr::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	GUIDE_STRUCT guide;
	switch( nID )
	{
		case WIDC_BACK:
			{
				if( m_VecGuideText.size() )
				{
					m_nCurrentVector--;

					if( m_nCurrentVector < 0 )
						m_nCurrentVector = 0;

					guide = m_VecGuideText[m_nCurrentVector];
					_SetGuideText(guide);
				}
			}
			break;
		case WIDC_NEXT:
			{
				if( m_VecGuideText.size() )
				{
					m_nCurrentVector++;

					if( m_nCurrentVector > m_VecGuideText.size()-1 )
						m_nCurrentVector = m_VecGuideText.size()-1;

					guide = m_VecGuideText[m_nCurrentVector];
					_SetGuideText(guide);
					
					CWndWorld* pWndWorld = (CWndWorld*)GetWndBase( APP_WORLD );
								
					if( pWndWorld ) 
					{
						pWndWorld->m_pWndGuideSystem->SendGuideMessage(GUIDE_EVENT_INTRO);
						pWndWorld->m_pWndGuideSystem->SendGuideMessage(GUIDE_EVENT_END);
					}
				}
			}
			break;
		default:
			{
				m_bVisible = FALSE;
				CWndWorld* pWndWorld = (CWndWorld*)GetWndBase( APP_WORLD );
			
				if( pWndWorld ) 
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(GUIDE_EVENT_INTRO);	
			}
			break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
#endif

void CWndGuideTextMgr::PaintFrame( C2DRender* p2DRender )
{
}

BOOL CWndGuideTextMgr::OnEraseBkgnd( C2DRender* p2DRender )
{
	CWndGuideSystem* pWndGuide = (CWndGuideSystem*)GetWndBase( APP_GUIDE );
	
	if( !pWndGuide )
		return FALSE;

	CRect rect = GetLayoutRect();

	rect.bottom = rect.bottom+5;
	p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	if(m_pTextureBG)
		m_pTextureBG->Render( p2DRender, CPoint(8,0), CPoint( rect.right, rect.bottom ), 200 );
	//p2DRender->RenderFillRect( CRect( 8, -8, rect.right, rect.bottom ), D3DXCOLOR( 1.0f, 1.0f, 1.0f, 0.8f ) );
	//p2DRender->RenderFillRect( CRect( rect.right, 8, rect.right+3, rect.bottom+3 ), D3DXCOLOR( 0.0f, 0.0f, 0.0f, 1.0f ) );
	//p2DRender->RenderFillRect( CRect( 8+3, rect.bottom, rect.right+3, rect.bottom+3 ), D3DXCOLOR( 0.0f, 0.0f, 0.0f, 1.0f ) );
	
	return TRUE;
}

void CWndGuideTextMgr::AddGuideText(GUIDE_STRUCT guide)
{

	m_VecGuideText.push_back( guide );

	m_nCurrentVector = m_VecGuideText.size()-1;

	PLAYSND( SND_INF_MESSENGERRING );
#if __VER >= 12 // __MOD_TUTORIAL
	if(guide.m_nVicCondition == 0)	_SetGuideText( guide, true);
	else							_SetGuideText( guide, false);
#else
	_SetGuideText( guide );
#endif
	
}

#if __VER >= 12 // __MOD_TUTORIAL
void CWndGuideTextMgr::_SetGuideText(GUIDE_STRUCT guide, bool bIsNext)
#else
void CWndGuideTextMgr::_SetGuideText(GUIDE_STRUCT guide)
#endif
{
	CWndText* pWndText;
	CWndButton* pWndButton;

	pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	pWndText->SetWndRect( m_Rect[0] );
	pWndButton= (CWndButton*)GetDlgItem( WIDC_BACK );
	pWndButton->SetWndRect( m_Rect[1] );
	pWndButton= (CWndButton*)GetDlgItem( WIDC_NEXT );
	pWndButton->SetWndRect( m_Rect[2] );
	SetWndRect(	m_Rect[3] );
#if __VER >= 12 // __MOD_TUTORIAL
	if(bIsNext) pWndButton->SetVisible(TRUE);
	else		pWndButton->SetVisible(FALSE);
#endif
	
	m_bVisible = TRUE;

	m_strHelpKey = guide.m_str;
	pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
#if __VER >= 12 // __MOD_TUTORIAL
	pWndText->SetString("");
	pWndText->m_string.AddParsingString(LPCTSTR(guide.m_str));
	pWndText->ResetString();
#else
	pWndText->SetString( (guide.m_nkey == CWndGuideSystem::KEY) ? prj.GetHelp( m_strHelpKey ) : guide.m_str );
#endif
	CRect rect = pWndText->GetWndRect();

	pWndText->m_string.Align( m_pFont );
	
	int nLine = pWndText->m_string.GetLineCount();
	
	if( nLine < 10 )
	{
		if( nLine < 8 )
			nLine = 8;

		CRect clientrect = GetWndRect();
		
		int ngap = rect.Height() - (nLine * 22);
		clientrect.top += ngap;
		SetWndRect(clientrect);

		rect.bottom -= ngap;
		pWndText->SetWndRect( rect );

		CRect ptRect;
#ifndef __MOD_TOTURIAL
		pWndButton= (CWndButton*)GetDlgItem( WIDC_BACK );
		ptRect = pWndButton->GetWndRect();
		ptRect.OffsetRect( 0, -ngap );
		pWndButton->SetWndRect(ptRect);
#endif
		pWndButton= (CWndButton*)GetDlgItem( WIDC_NEXT );
		ptRect = pWndButton->GetWndRect();
		ptRect.OffsetRect( 0, -ngap );
		pWndButton->SetWndRect(ptRect);
		
		pWndText->m_wndScrollBar.m_bVisible = FALSE;
	}
	else
	{
		/*
		pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
		pWndText->SetWndRect( m_Rect[0] );
		pWndButton= (CWndButton*)GetDlgItem( WIDC_BACK );
		pWndButton->SetWndRect( m_Rect[1] );
		pWndButton= (CWndButton*)GetDlgItem( WIDC_NEXT );
		pWndButton->SetWndRect( m_Rect[2] );
		SetWndRect(	m_Rect[3] );
		*/

		pWndText->m_wndScrollBar.m_bVisible = TRUE;
		pWndText->SetWndRect( rect );
	}

	CWndGuideSystem* pWndGuide = (CWndGuideSystem*)GetWndBase( APP_GUIDE );
	CRect rectGuide = pWndGuide->GetWindowRect( TRUE );
	CPoint ptGuide = rectGuide.TopLeft();
	CPoint ptMove;

	pWndGuide->m_bVisible = TRUE;
	
	CRect windowrect = GetWindowRect( TRUE );
	
	if( ptGuide.x > windowrect.Width() )
		ptMove = ptGuide - CPoint( windowrect.Width(), 0 );
	else
		ptMove = ptGuide + CPoint( rectGuide.Width(), 0 );

	ptMove.y = rectGuide.bottom;
	ptMove.y -= windowrect.Height();
	
	if( ptMove.y < 0 )
		ptMove.y = rectGuide.top;
	
	Move( ptMove );
}

void CWndGuideTextMgr::UpDate()
{

	if( m_VecGuideText.size() )
	{
		m_bVisible = TRUE;
		
		GUIDE_STRUCT guide;
		guide = m_VecGuideText[m_nCurrentVector];
		//_SetGuideText(guide);
	}
	else
		m_bVisible = FALSE;

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndGuideSelection::CWndGuideSelection()
{
}

CWndGuideSelection::~CWndGuideSelection()
{
}

BOOL CWndGuideSelection::Initialize( CWndBase* pWndParent, DWORD nType )
{	
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUIDE_SELECTION, 0, CPoint( 0, 0 ), pWndParent );
}
BOOL CWndGuideSelection::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
		case WIDC_RADIO1:
			{
				*g_Option.m_pGuide = 0;
			}
			break;
		case WIDC_RADIO2:
			{
				*g_Option.m_pGuide = 1;
			}
			break;
		case WIDC_RADIO3:
			{
				*g_Option.m_pGuide = 2;
			}
			break;
		case WIDC_BUTTON1:
			{
				CWndGuideSystem* pWndGuide = (CWndGuideSystem*)GetWndBase( APP_GUIDE );

				if( pWndGuide && pWndGuide->m_bAniState == CWndGuideSystem::ANI_IDLE && pWndGuide->m_bIsLoad )
				{
					pWndGuide->m_dwGuideLevel = *g_Option.m_pGuide;
				#if __VER >= 12 // __MOD_TUTORIAL
					if( pWndGuide->m_dwGuideLevel == 0 )
						pWndGuide->GuideStart();
					else
					{
						if(g_pPlayer)
							pWndGuide->SetAni( g_pPlayer->GetJob(), CWndGuideSystem::ANI_BYTE );
					}
				#else
					if( pWndGuide->m_dwGuideLevel == 0 )
						pWndGuide->GuideStart(TRUE);
					else
					{
						pWndGuide->GuideStart(FALSE);
						if(g_pPlayer)
							pWndGuide->SetAni( g_pPlayer->GetJob(), CWndGuideSystem::ANI_BYTE );
					}
				#endif
					
					Destroy();
				}
			}
			break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
void CWndGuideSelection::OnDraw( C2DRender* p2DRender )
{

}
void CWndGuideSelection::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	
	m_wndTitleBar.SetVisible( FALSE );

	CWndButton* pWndButton[3];
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_RADIO1 );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_RADIO2 );
	pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_RADIO3 );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ 0 ]->SetCheck( TRUE );

	*g_Option.m_pGuide = 0;
	
	MoveParentCenter();
}
BOOL CWndGuideSelection::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndGuideSelection::OnSize( UINT nType, int cx, int cy )
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndGuideSelection::OnLButtonUp( UINT nFlags, CPoint point )
{
}
void CWndGuideSelection::OnLButtonDown( UINT nFlags, CPoint point )
{
}
void CWndGuideSelection::OnRButtonDown( UINT nFlags, CPoint point )
{
}

////////////////////////////////////////////////////////////////////////////////////



