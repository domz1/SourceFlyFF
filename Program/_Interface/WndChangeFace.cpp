#include "stdafx.h"
#include "defineObj.h"
#include "AppDefine.h"
#include "WndChangeFace.h"
#include "defineText.h"
#ifdef __CLIENT
#include "dpclient.h"
extern CDPClient	g_DPlay;
#endif


/****************************************************
  WndId : APP_CHANGEFACE - 얼굴변경
  CtrlId : WIDC_STATIC1 - 얼굴을 선택하세요. 총 2회 변경이 가능합니다.
  CtrlId : WIDC_OK - Button
  CtrlId : WIDC_CANCEL - Button
****************************************************/

CWndChangeFace::CWndChangeFace() 
{ 
	m_nSelectFace = 0;
} 
CWndChangeFace::~CWndChangeFace() 
{ 
} 
void CWndChangeFace::OnDraw( C2DRender* p2DRender ) 
{ 
	if( m_nSelectFace >=0 && m_nSelectFace < 5 )
	{
		CRect rect( 0, 0, 86, 170 );
		rect.OffsetRect( 10, 5 );

		rect.OffsetRect( m_nSelectFace * 86, 0 );

		//point.x -= 10;
		//point.y -= 25;
		//point.x /= 86;

		p2DRender->RenderRect( rect, 0xff00ffff );	

		rect.InflateRect( 1, 1 );
		p2DRender->RenderRect( rect, 0xff00ffff );
	}
} 
void CWndChangeFace::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndChangeFace::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_CHANGEFACE, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndChangeFace::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndChangeFace::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndChangeFace::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndChangeFace::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
	// 10,25 
	point.x -= 10;
	
	point.x /= 86;

	if( point.x >=0 && point.x < 5 && point.y >= 0 && point.y < 180 )
	{
		m_nSelectFace = point.x;
	}
} 
void CWndChangeFace::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndChangeFace::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK )
	{
		CString strCommand;
		strCommand.Format( "/ChangeFace %d", m_nSelectFace );
		g_DPlay.SendChat( strCommand );
		Destroy( TRUE );
	}
	else
	if( nID == WIDC_CANCEL || nID == WTBID_CLOSE )
	{
		nID = WTBID_CLOSE;
		Destroy( TRUE );
		return TRUE;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


#define MAX_FACE 5

CWndChangeSex::CWndChangeSex() 
{ 
	m_nSelectFace = 0;
	m_pModel          = NULL;
//	m_dwItemId	= 0;
	m_dwItemId	= NULL_ID;
	m_dwObjId	= NULL_ID;
} 
CWndChangeSex::~CWndChangeSex() 
{ 
	SAFE_DELETE( m_pModel );
} 
void CWndChangeSex::OnDraw( C2DRender* p2DRender ) 
{ 
	if( g_pPlayer == NULL  )
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
	D3DXVECTOR3 vecPos(  0.0f, 0.7f, -3.5f );
	
	D3DXMatrixLookAtLH( &matView, &vecPos, &vecLookAt, &D3DXVECTOR3(0.0f,1.0f,0.0f) );
	
	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
	
	// 왼쪽 원본 모델 랜더링
	{
		LPWNDCTRL lpFace = GetWndCtrl( WIDC_STATIC1 );

		viewport.X      = p2DRender->m_ptOrigin.x + lpFace->rect.left;//2;
		viewport.X     -= 6;
		viewport.Y      = p2DRender->m_ptOrigin.y + lpFace->rect.top;//5;
		viewport.Width  = lpFace->rect.Width();//p2DRender->m_clipRect.Width();
		viewport.Height = lpFace->rect.Height();// - 10;//p2DRender->m_clipRect.Height();

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
		FLOAT fov = D3DX_PI/4.0f;//796.0f;
		FLOAT h = cos(fov/2) / sin(fov/2);
		FLOAT w = h * fAspect;
		D3DXMatrixOrthoLH( &matProj, w, h, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
		pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
		
	    D3DXMatrixIdentity(&matScale);
		D3DXMatrixIdentity(&matTrans);
		D3DXMatrixIdentity(&matWorld);
		
		D3DXMatrixScaling(&matScale, 4.5f, 4.5f, 4.5f);
		
		if( g_pPlayer->GetSex() == SEX_MALE )
			D3DXMatrixTranslation(&matTrans,0.0f,-5.6f,0.0f);
		else
			D3DXMatrixTranslation(&matTrans,0.0f,-5.8f,0.0f);

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
/*
		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[0] = 0.5f;
		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[1] = 0.5f;
		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[2] = 0.5f;
*/		
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
} 
void CWndChangeSex::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	SAFE_DELETE( m_pModel );
	
	// 성전환이니깐 반대로 하자~
	int nMover = (g_pPlayer->GetSex() == SEX_MALE ? MI_FEMALE:MI_MALE );
	m_pModel = (CModelObject*)prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE );
	prj.m_modelMng.LoadMotion( m_pModel,  OT_MOVER, nMover, MTI_STAND2 );

	CMover::UpdateParts( !g_pPlayer->GetSex(), 0, 0, g_pPlayer->m_dwHairMesh , m_nSelectFace,g_pPlayer->m_aEquipInfo, m_pModel, &g_pPlayer->m_Inventory );
	
	m_pModel->InitDeviceObjects( g_Neuz.GetDevice() );
	
	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndChangeSex::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_CHANGESEX, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndChangeSex::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndChangeSex::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndChangeSex::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndChangeSex::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK )
	{
		if( m_dwItemId == NULL_ID )
		{
			CString strCommand;
			strCommand.Format( "/ChangeFace %d", m_nSelectFace );
			g_DPlay.SendChat( strCommand );
		}
		else
		{
			g_DPlay.SendDoUseItem( m_dwItemId, m_dwObjId, m_nSelectFace );
		}
		Destroy( TRUE );
	}
	else
	if( nID == WIDC_CANCEL || nID == WTBID_CLOSE )
	{
		nID = WTBID_CLOSE;
		Destroy( TRUE );
		return TRUE;
	}
	else
	if( nID == WIDC_LEFT )
	{
		m_nSelectFace--;

		if( m_nSelectFace < 0 )
		{
			m_nSelectFace = MAX_FACE-1;
		}
		CMover::UpdateParts( !g_pPlayer->GetSex(), 0, 0, g_pPlayer->m_dwHairMesh, m_nSelectFace,g_pPlayer->m_aEquipInfo, m_pModel, &g_pPlayer->m_Inventory );		
	}
	else
	if( nID == WIDC_RIGHT )
	{
		m_nSelectFace++;
		
		if( m_nSelectFace >= MAX_FACE )
		{
			m_nSelectFace = 0;
		}
		CMover::UpdateParts( !g_pPlayer->GetSex(), 0, 0, g_pPlayer->m_dwHairMesh, m_nSelectFace,g_pPlayer->m_aEquipInfo, m_pModel, &g_pPlayer->m_Inventory );		
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

/****************************************************
  WndId : APP_ITEM_TRANSY - 아이템 트랜지(ITM)
  CtrlId : WIDC_STATIC1 - 
  CtrlId : WIDC_STATIC2 - 아이템 트랜지(ITM)
  CtrlId : WIDC_STATIC3 - 아이템 : 
  CtrlId : WIDC_OK - Button
  CtrlId : WIDC_CANCEL - Button
****************************************************/

CWndItemTransy::CWndItemTransy() 
{ 
} 
CWndItemTransy::~CWndItemTransy() 
{ 
}

#ifdef __SYS_ITEMTRANSY
void CWndItemTransy::Init( CItemElem* pItemElem, BOOL bMenu )
#else //__SYS_ITEMTRANSY
void CWndItemTransy::Init( CItemElem* pItemElem )
#endif //__SYS_ITEMTRANSY
{
#ifdef __SYS_ITEMTRANSY
	m_bMenu = bMenu;
#endif // __SYS_ITEMTRANSY
	for( int i = 0 ; i < 2 ; ++i )
		m_pItemElem[i] = NULL;
	 	
	LPWNDCTRL pWndCtrl = NULL;
	pWndCtrl = GetWndCtrl( WIDC_STATIC1 );
	m_Rect[0] = pWndCtrl->rect;

	pWndCtrl = GetWndCtrl( WIDC_STATIC4 );
	m_Rect[1] = pWndCtrl->rect;

	if( pItemElem )
	{
		m_pItemElem[1] = pItemElem;
		m_pItemElem[1]->SetExtra( 1 );
	}
}
void CWndItemTransy::OnDestroy( void )
{
	if( m_pItemElem[0] )
		m_pItemElem[0]->SetExtra( 0 );	
	if( m_pItemElem[1] )
		m_pItemElem[1]->SetExtra( 0 );	
}
void CWndItemTransy::OnDraw( C2DRender* p2DRender ) 
{ 
	if( m_pItemElem[0] )
	{
		if( m_pItemElem[0]->GetTexture() )
			m_pItemElem[0]->GetTexture()->Render( p2DRender, m_Rect[0].TopLeft(), 255 );

		CTexture			Texture1;
		Texture1.LoadTexture(g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, m_pItemElemChange.GetProp()->szIcon ), 0xffff00ff );
		Texture1.Render( p2DRender, m_Rect[1].TopLeft(), 255 );
	}

	CPoint	Point = GetMousePoint();
	CRect HitRect = m_Rect[0];
	CPoint Point2 = Point;
	if( m_Rect[0].PtInRect( Point ) )
	{
		ClientToScreen( &Point2 );
		ClientToScreen( &HitRect );
		if( m_pItemElem[0] )
			g_WndMng.PutToolTip_Item( (CItemBase*)m_pItemElem[0], Point2, &HitRect );
		else
			g_toolTip.PutToolTip( 100, prj.GetText( TID_GAME_TRANSITEM_PUT ), *HitRect, Point2, 0 );
	}
	
	HitRect = m_Rect[1];
	if( m_Rect[1].PtInRect( Point ) )
	{
		ClientToScreen( &Point2 );
		ClientToScreen( &HitRect );
		if( m_pItemElem[0] )
			g_WndMng.PutToolTip_Item( (CItemBase*)&m_pItemElemChange, Point2, &HitRect );
		else
			g_toolTip.PutToolTip( 100, prj.GetText( TID_GAME_TRANSITEM_PUT1 ), *HitRect, Point2, 0 );
	}
} 
void CWndItemTransy::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndItemTransy::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_ITEM_TRANSY, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndItemTransy::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndItemTransy::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndItemTransy::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndItemTransy::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
void CWndItemTransy::OnRButtonUp( UINT nFlags, CPoint point )
{
	if( m_pItemElem[0] && PtInRect( &m_Rect[0], point ) )
	{
		m_pItemElem[0]->SetExtra( 0 );
		m_pItemElem[0] = NULL;
	}
}
BOOL CWndItemTransy::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case WIDC_OK:
		{
#ifdef __SYS_ITEMTRANSY
			if(m_bMenu)
			{
				if(m_pItemElem[0])
					g_DPlay.SendItemTransy( m_pItemElem[0]->m_dwObjId, NULL_ID, m_pItemElemChange.m_dwItemId, FALSE );
			}
			else
			{
				if(m_pItemElem[0] && m_pItemElem[1])
					g_DPlay.SendItemTransy( m_pItemElem[0]->m_dwObjId, m_pItemElem[1]->m_dwObjId, m_pItemElemChange.m_dwItemId, TRUE );
			}

			Destroy();
#else //__SYS_ITEMTRANSY
			if( m_pItemElem[0] && m_pItemElem[1] )
			{
				g_DPlay.SendItemTransy( m_pItemElem[0]->m_dwObjId, m_pItemElem[1]->m_dwObjId );
				Destroy();
			}
#endif //__SYS_ITEMTRANSY
		}
		break;
	case WIDC_CANCEL:
		{
			Destroy();
		}
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

BOOL CWndItemTransy::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CWndBase* pWndFrame = pShortcut->m_pFromWnd->GetFrameWnd();
	
	// 인벤토리에서 온것인지 검사
	if( !(pShortcut->m_dwShortcut == SHORTCUT_ITEM) && !(pWndFrame->GetWndId() == APP_INVENTORY) )
		return FALSE;
	
	// 장착되어있는지 검사
	if( g_pPlayer->m_Inventory.IsEquip( pShortcut->m_dwId ) )
	{
		g_WndMng.PutString( prj.GetText(TID_GAME_EQUIPPUT), NULL, prj.GetTextColor(TID_GAME_EQUIPPUT) );
		SetForbid( TRUE );
		return FALSE;
	}
	
	CItemElem* pItemElem = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
	if( pItemElem == NULL )
	{
		SetForbid( TRUE );
		return FALSE;
	}
	
	if( PtInRect( &m_Rect[0], point ) )
	{
		ItemProp* pItemPropChange = NULL;
		// 성별이 있는 아이템 인지 검사
		ItemProp* pItemProp = pItemElem->GetProp();
		
		pItemPropChange = g_pPlayer->GetTransyItem( pItemProp );
		if( pItemPropChange == NULL )
		{
			g_WndMng.PutString( prj.GetText(TID_GAME_ITEM_TRANSY), NULL, prj.GetTextColor(TID_GAME_ITEM_TRANSY) );
			SetForbid( TRUE );
			return FALSE;
		}

#ifdef __SYS_ITEMTRANSY
		if(!m_bMenu)
		{
#endif //__SYS_ITEMTRANSY
			if( m_pItemElem[1]->GetProp()->dwID == II_CHR_SYS_SCR_ITEMTRANSY_A )
			{
				if( 61 <= pItemElem->GetProp()->dwLimitLevel1 )
				{
					CString strMsg;
					strMsg.Format( prj.GetText( TID_GAME_ITEM_TRANSY_NOT_LEVEL_0 ), m_pItemElem[1]->GetProp()->szName );
					g_WndMng.PutString( strMsg, NULL, prj.GetTextColor(TID_GAME_ITEM_TRANSY_NOT_LEVEL_0) );
					SetForbid( TRUE );
					return FALSE;
				}
			}
			else
			{
				if( pItemElem->GetProp()->dwLimitLevel1 < 61 )
				{
					CString strMsg;
					strMsg.Format( prj.GetText( TID_GAME_ITEM_TRANSY_NOT_LEVEL_1 ), m_pItemElem[1]->GetProp()->szName );
					g_WndMng.PutString( strMsg, NULL, prj.GetTextColor(TID_GAME_ITEM_TRANSY_NOT_LEVEL_1) );
					SetForbid( TRUE );
					return FALSE;
				}
			}
#ifdef __SYS_ITEMTRANSY
		}
#endif //__SYS_ITEMTRANSY
		if( m_pItemElem[0] )
		{
			m_pItemElem[0]->SetExtra( 0 );	
		}
		
		pItemElem->SetExtra( 1 );
		m_pItemElem[0] = pItemElem;

		m_pItemElemChange = *m_pItemElem[0];
		m_pItemElemChange.m_dwItemId = pItemPropChange->dwID;
		m_pItemElemChange.m_nHitPoint	= pItemPropChange->dwEndurance;
	}
	
	return TRUE;
}
