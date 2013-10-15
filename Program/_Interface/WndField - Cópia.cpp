// WndArcane.cpp: implementation of the CWndNeuz class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "defineObj.h"
#include "DefineItem.h"

#if __VER >= 19
#include "defineItemGrade.h"
#include "defineItemType.h"
#endif//__VER >= 19

#include "defineText.h"
#include "defineskill.h"
#include "defineSound.h"
#include "AppDefine.h"
#include "DialogMsg.h"
#include "WndField.h"
#include "Party.h"
#include "WndManager.h"
#include <afxdisp.h>
#include "defineNeuz.h"


#include "DPClient.h"
#include "MsgHdr.h"
extern	CDPClient	g_DPlay;

#include "wndvendor.h"
#include "wndwebbox.h"
#if __VER >= 11 // __SYS_POCKET
#include "WndBagEx.h"
#endif

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

#if __VER >= 15 // __GUILD_HOUSE
#include "GuildHouse.h"
#endif

#ifdef __TELEPORT_SCROLL
#include "WndMapEx.h"
#endif // __TELEPORT_SCROLL

extern int g_nSkillCurSelect;
extern float g_fHairLight;

#include "eveschool.h"
extern CGuildCombat g_GuildCombatMng;

#include "dpcertified.h"
extern	CDPCertified	g_dpCertified;

#ifdef __NEW_WEB_BOX
#include "WndHelperWebBox.h"
#endif // __NEW_WEB_BOX

#if __VER >= 11 // __SYS_IDENTIFY
#include "randomoption.h"
#endif	// __SYS_IDENTIFY
//extern TCHAR g_szExpertName[ MAX_EXPERT ][ 32 ];
//extern TCHAR g_szJobName[ MAX_JOB ][ 32 ];
//extern const JOB prj.m_aJob[ MAX_JOB ];

#define WND_WIDTH 210
#define TASKBAR_HEIGHT 44

extern DWORD FULLSCREEN_WIDTH;
extern DWORD FULLSCREEN_HEIGHT;


#define MAX_GUILDCOMBAT_LIST_PER_PAGE 11
#define MAX_MAIL_LIST_PER_PAGE 6
#define MAX_GUILDCOMBAT_LIST		  100

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 애플렛 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 드롭아이템
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
CWndDropItem::CWndDropItem() 
{ 
	m_pItemElem = NULL;
	m_pEdit = NULL;
} 
CWndDropItem::~CWndDropItem() 
{ 
} 
void CWndDropItem::OnDraw( C2DRender* p2DRender ) 
{ 
	LPCTSTR szNumber;
	szNumber = m_pEdit->GetString();
	int nNumber = atoi( szNumber );

	if( m_pItemElem->m_nItemNum == 1 )
	{
		m_pEdit->SetString( "1" );
	}
	else
	{
		if( m_pItemElem->m_nItemNum < nNumber )
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
void CWndDropItem::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	m_pEdit   = (CWndEdit  *)GetDlgItem( WIDC_EDIT1 );
	CWndButton* pWndOk = (CWndButton *)GetDlgItem( WIDC_OK );	
	pWndOk->SetDefault( TRUE );
	m_pEdit->SetFocus();

	if( m_pItemElem->m_nItemNum == 1 )
	{
		m_pEdit->SetString( "1" );
		//		m_pEdit->SetVisible( FALSE );
	}
	else
	{
		TCHAR szNumber[ 64 ];
		_itot( m_pItemElem->m_nItemNum, szNumber, 10 );
		m_pEdit->SetString( szNumber );
	}	

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndDropItem::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_DROP_ITEM, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndDropItem::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndDropItem::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndDropItem::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndDropItem::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndDropItem::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

#if __VER >= 14 // __DROP_CONFIRM_BUG
BOOL CWndDropItem::Process( void )
{
	if(m_pItemElem->GetExtra() > 0)
	{
		Destroy();
	}
	return TRUE;
}
#endif // __DROP_CONFIRM_BUG

BOOL CWndDropItem::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK || message == EN_RETURN )
	{
		short DropNum = 0;

		if( m_pItemElem->m_nItemNum >= 1 )
		{
			DropNum = atoi( m_pEdit->GetString() );
		}
		if( DropNum != 0 )
		{
			g_DPlay.SendDropItem( 0, m_pItemElem->m_dwObjId, DropNum, m_vPos );
		}

		Destroy();
	}
	else if( nID == WIDC_CANCEL )
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 드롭확인
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
CWndDropConfirm::CWndDropConfirm() 
{ 
	m_pItemElem = NULL;
	m_vPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
} 
CWndDropConfirm::~CWndDropConfirm() 
{ 
} 
void CWndDropConfirm::OnDraw( C2DRender* p2DRender ) 
{ 

} 
void CWndDropConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	CWndButton* pWndOk = (CWndButton *)GetDlgItem( WIDC_YES );	
	pWndOk->SetDefault( TRUE );

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndDropConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_DROP_CONFIRM, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndDropConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndDropConfirm::OnSize( UINT nType, int cx, int cy )
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndDropConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndDropConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

#if __VER >= 14 // __DROP_CONFIRM_BUG
BOOL CWndDropConfirm::Process( void )
{
	if(m_pItemElem->GetExtra() > 0)
	{
		Destroy();
	}
	return TRUE;
}
#endif // __DROP_CONFIRM_BUG

BOOL CWndDropConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES || message == EN_RETURN )
	{
		g_DPlay.SendDropItem( 0, m_pItemElem->m_dwObjId, 1, m_vPos );
		Destroy();
	}
	else if( nID == WIDC_NO )
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

/****************************************************
  WndId : APP_RANDOMSCROLL_CONFIRM - 랜덤 옵션
  CtrlId : WIDC_TEXT1 - 
  CtrlId : WIDC_YES - Button
  CtrlId : WIDC_NO - Button
****************************************************/

CWndRandomScrollConfirm::CWndRandomScrollConfirm() 
{ 
	bFlag = FALSE;
	objid = objid1 = NULL_ID;
} 
CWndRandomScrollConfirm::~CWndRandomScrollConfirm() 
{ 
} 
void CWndRandomScrollConfirm::SetItem( OBJID objidBuf, OBJID objid1Buf, BOOL bFlagBuf )
{
	objid = objidBuf;
	objid1 = objid1Buf;
	bFlag = bFlagBuf;
	
}
void CWndRandomScrollConfirm::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndRandomScrollConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	CItemElem* pItemElem0	= g_pPlayer->m_Inventory.GetAtId( objid );
	CItemElem* pItemElem1	= g_pPlayer->m_Inventory.GetAtId( objid1 );

	if( pItemElem0 && pItemElem1 )
	{
		CString strMessage;
		if( bFlag )
			strMessage.Format( prj.GetText( TID_GAME_RANDOMSCROLL_CONFIRM1 ), pItemElem0->GetProp()->szName );
		else
			strMessage.Format( prj.GetText( TID_GAME_RANDOMSCROLL_CONFIRM ), pItemElem0->GetProp()->szName );

		CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
		pWndText->SetString( strMessage );
		pWndText->EnableWindow( FALSE );
	}
	else
	{
		Destroy();
	}

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndRandomScrollConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_RANDOMSCROLL_CONFIRM, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndRandomScrollConfirm::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndRandomScrollConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndRandomScrollConfirm::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndRandomScrollConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndRandomScrollConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndRandomScrollConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES )
	{
		g_DPlay.SendRandomScroll( objid, objid1 );
	}
	Destroy();

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

/****************************************************
WndId : APP_QUEITMWARNING - 아이템 경고
CtrlId : WIDC_CONTEXT - 
CtrlId : WIDC_BTN_YES - Button
CtrlId : WIDC_BTN_NO - Button
CtrlId : WIDC_WARNING - Picture
****************************************************/

CWndQuestItemWarning::CWndQuestItemWarning() 
{ 
	m_pItemElem = NULL;
	m_vPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
} 
CWndQuestItemWarning::~CWndQuestItemWarning() 
{ 
} 
void CWndQuestItemWarning::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndQuestItemWarning::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	CWndEdit* pWndEdit	= (CWndEdit*)GetDlgItem( WIDC_CONTEXT );
	
	if( pWndEdit )
	{
		pWndEdit->SetString( _T( prj.GetText( TID_GAME_QUEITMWARNING ) ) );
		pWndEdit->EnableWindow( FALSE );	
	}
	
	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndQuestItemWarning::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_QUEITMWARNING, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
직접 윈도를 열때 사용 
BOOL CWndQuestItemWarning::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
CRect rectWindow = m_pWndRoot->GetWindowRect(); 
CRect rect( 50 ,50, 300, 300 ); 
SetTitle( _T( "title" ) ); 
return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndQuestItemWarning::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndQuestItemWarning::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndQuestItemWarning::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndQuestItemWarning::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndQuestItemWarning::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BTN_YES || message == EN_RETURN )
	{
		if( m_pItemElem->m_nItemNum == 1 )	// 갯수가 하나라면 그냥 떨군다.
		{
			g_WndMng.m_pWndDropConfirm = new CWndDropConfirm;
			g_WndMng.m_pWndDropConfirm->m_pItemElem = m_pItemElem;
			g_WndMng.m_pWndDropConfirm->m_vPos = m_vPos;
			g_WndMng.m_pWndDropConfirm->Initialize( NULL, APP_DROP_CONFIRM );
		}
		else
		{
			//SAFE_DELETE( g_WndMng.m_pWndDropItem );
			g_WndMng.m_pWndDropItem = new CWndDropItem;
			g_WndMng.m_pWndDropItem->m_pItemElem = m_pItemElem;
			g_WndMng.m_pWndDropItem->m_vPos = m_vPos;
			g_WndMng.m_pWndDropItem->Initialize( NULL, APP_DROP_ITEM );
		}
		Destroy();
	}
	else if( nID == WIDC_BTN_NO )
	{
		Destroy();
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 인벤토리 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "WndShop.h"

CWndGold::CWndGold()
{
}
CWndGold::~CWndGold()
{
}
void CWndGold::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_GlobalShortcut.m_dwShortcut = SHORTCUT_ITEM;
	m_GlobalShortcut.m_pFromWnd = this;
	m_GlobalShortcut.m_pTexture = &m_texture; //.m_pFromWnd   = this;
	m_GlobalShortcut.m_dwData = 0;
}
void CWndGold::OnMouseMove(UINT nFlags, CPoint point)
{
}
void CWndGold::OnInitialUpdate()
{
	m_texture.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_ITEM, "itm_GolGolSeed.dds" ), 0xffff00ff );
}

//////////////////////////////////////////////
CWndQueryEquip::CWndQueryEquip()
{
	m_ObjID = NULL_ID;
	memset( m_InvenRect, 0, sizeof(CRect) * MAX_HUMAN_PARTS );

	m_pModel = NULL;
	m_OldPos = CPoint(0,0);
}
CWndQueryEquip::~CWndQueryEquip()
{
	SAFE_DELETE( m_pModel );
}

BOOL CWndQueryEquip::Process()
{
	CMover* pMover = GetMover();

	if( IsInvalidObj(pMover) )
	{
		Destroy();
		return FALSE;
	}

#if __VER < 12 // __CSC_VER12_1
	D3DXVECTOR3	vDist = pMover->GetPos() - g_pPlayer->GetPos();	// 타겟과의 거리.
	FLOAT		fDistSq = D3DXVec3LengthSq( &vDist );
	
	int nHalfRange = 20 / 2;
	if( fDistSq > (nHalfRange * nHalfRange) )
	{
		Destroy();
		return FALSE;		
	}
#endif //__CSC_VER12_1

	if( m_pModel )
		m_pModel->FrameMove();
		
	return TRUE;
}	
void CWndQueryEquip::OnMouseWndSurface( CPoint point )
{
	CMover* pMover = GetMover();

	if( IsInvalidObj(pMover) )
		return ;
	
	for( int i=2; i<MAX_HUMAN_PARTS; i++ )
	{
		if( pMover->m_aEquipInfo[i].dwId == NULL_ID )
			continue;
		
		CRect DrawRect = m_InvenRect[i];
		
		CPoint point = GetMousePoint();
		// 툴팁
		if( DrawRect.PtInRect( point ) )
		{
			CPoint point2 = point;
			ClientToScreen( &point2 );
			ClientToScreen( &DrawRect );
			
			CItemElem itemElem;
			itemElem.m_dwItemId	= pMover->m_aEquipInfo[i].dwId;
			itemElem.m_byFlag	= pMover->m_aEquipInfo[i].byFlag;
			itemElem.SetAbilityOption( pMover->m_aEquipInfo[i].nOption & 0xFF );
			itemElem.m_nResistAbilityOption = m_aEquipInfoAdd[i].nResistAbilityOption;
			itemElem.m_bItemResist	= m_aEquipInfoAdd[i].bItemResist;
#if __VER >= 11 // __SYS_IDENTIFY
			itemElem.SetRandomOptItemId( m_aEquipInfoAdd[i].iRandomOptItemId );
#else	// __SYS_IDENTIFY
			itemElem.SetRandomOptItemId( m_aEquipInfoAdd[i].nRandomOptItemId );
#endif	// __SYS_IDENTIFY
			itemElem.CopyPiercing( m_aEquipInfoAdd[i].piercing );
			
			// 장비창에 있는것 툴팁
			g_WndMng.PutToolTip_Item( (CItemBase*)&itemElem, point2, &DrawRect, APP_QUERYEQUIP );
		}
	}
}
void CWndQueryEquip::OnDraw(C2DRender* p2DRender)
{
	CMover* pMover = GetMover();
	
	if( IsInvalidObj(pMover) )
		return ;
	
	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

	// 뷰포트 세팅 
	D3DVIEWPORT9 viewport;

	viewport.X      = p2DRender->m_ptOrigin.x + 5;//100;// + 5;
	viewport.Y      = p2DRender->m_ptOrigin.y + 5;// + 5;
	viewport.Width  = p2DRender->m_clipRect.Width() - 100;//105;
	viewport.Height = 182;//p2DRender->m_clipRect.Height() - 200;
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;

	DWORD dwColor1 = D3DCOLOR_ARGB( 100, 0, 0,  0 );//D3DCOLOR_TEMP( 255,   0,   0,  50 );//
	DWORD dwColor2 = D3DCOLOR_ARGB( 255, 240, 240,  240 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//
	DWORD dwColor3 = D3DCOLOR_ARGB( 100, 200, 200,  200 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//

	for( int i=2; i<MAX_HUMAN_PARTS; i++ )
	{
		FLOAT sx = 1.0f, sy = 1.0f;

		DWORD dwAlpha = 255;

		if( pMover->m_aEquipInfo[i].dwId == NULL_ID )
			continue;

		ItemProp* pItemProp	= prj.GetItemProp( pMover->m_aEquipInfo[i].dwId );
		if( !pItemProp )
			continue;

		if( m_aEquipInfoAdd[i].pTexture == NULL )
			continue;

		if( i == PARTS_LWEAPON )		// 왼손무기 그릴타이밍일때
		{
			if( pItemProp->dwHanded == HD_TWO )	// 투핸드 무기면
			{
				dwAlpha   = 100;
			}
		}
		
		CRect DrawRect = m_InvenRect[i];
		CPoint cpAdd = CPoint(6,6);
		
		if( i >= PARTS_NECKLACE1 && i <= PARTS_EARRING2 )
		{
			cpAdd = CPoint(0,0);
			
			sx = 0.8f;
			sy = 0.8f;
		}
		else
		if( i >= PARTS_HAT && i <= PARTS_BOOTS )
		{
			cpAdd = CPoint(0,0);
			
			sx = 0.9f;
			sy = 0.9f;
		}

		if( pMover->m_aEquipInfo[i].byFlag & CItemElem::expired )
		{
			p2DRender->RenderTexture2( DrawRect.TopLeft()+cpAdd, m_aEquipInfoAdd[i].pTexture, sx, sy, D3DCOLOR_XRGB( 255, 100, 100 ) );
		}
		else
		{
			p2DRender->RenderTexture( DrawRect.TopLeft()+cpAdd, m_aEquipInfoAdd[i].pTexture, dwAlpha, sx, sy );
		}
	}

	if( m_pModel == NULL )
		return;

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
	
	#ifdef __YENV
	D3DXVECTOR3 vDir( 0.0f, 0.0f, 1.0f );
	SetLightVec( vDir );
	#endif //__YENV
	
	// 왼쪽 원본 모델 랜더링

	{
	


		LPWNDCTRL lpFace = GetWndCtrl( WIDC_CUSTOM20 );

		viewport.X      = p2DRender->m_ptOrigin.x + lpFace->rect.left;
		viewport.Y      = p2DRender->m_ptOrigin.y + lpFace->rect.top;
		viewport.Width  = lpFace->rect.Width();
		viewport.Height = lpFace->rect.Height();

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
		D3DXMatrixIdentity(&matRot);
		D3DXMatrixIdentity(&matTrans);
		D3DXMatrixIdentity(&matWorld);
		
		D3DXMatrixRotationY( &matRot, D3DXToRadian( m_fRot ) );
		
		D3DXMatrixScaling(&matScale,1.2f,1.2f,1.2f);
		D3DXMatrixTranslation(&matTrans,0.0f,-0.6f,0.0f);
		
		D3DXMatrixMultiply(&matWorld,&matWorld,&matScale);
		D3DXMatrixMultiply(&matWorld, &matWorld, &matRot );
		D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans );
		pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

		// 랜더링 
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );//m_bViewLight );
		
		::SetLight( FALSE );
		::SetFog( FALSE );
		SetDiffuse( 1.0f, 1.0f, 1.0f );
		SetAmbient( 1.0f, 1.0f, 1.0f );

		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[0] = g_pPlayer->m_fHairColorR;
		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[1] = g_pPlayer->m_fHairColorG;
		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[2] = g_pPlayer->m_fHairColorB;

		D3DXVECTOR4 vConst( 1.0f, 1.0f, 1.0f, 1.0f );
	#ifdef __YENV
		g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvFog, &vConst );
	#else //__YENV						
		pd3dDevice->SetVertexShaderConstantF( 95, (float*)&vConst, 1 );
	#endif //__YENV
		::SetTransformView( matView );
		::SetTransformProj( matProj );
	
	pMover->OverCoatItemRenderCheck(m_pModel);
		
	// 헬멧이 머리카락 날려야하는것이냐?  // 인벤이 없는경우
			DWORD dwId	= pMover->m_aEquipInfo[PARTS_CAP].dwId;
			O3D_ELEMENT*	pElement = NULL;
			ItemProp*		pItemProp = NULL;
			
			if( dwId != NULL_ID )
			{
				pItemProp	= prj.GetItemProp( dwId );
				
				if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
				{
					pElement = m_pModel->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE );
				}
				
				// 외투의상을 입었을경우 머리날릴것인가의 기준을 외투 모자를 기준으로 바꾼다
				dwId	= pMover->m_aEquipInfo[PARTS_HAT].dwId;
				if( dwId != NULL_ID )
				{
					if( !(pMover->m_aEquipInfo[PARTS_HAT].byFlag & CItemElem::expired) )
					{
						pItemProp	= prj.GetItemProp( dwId );
						if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
						{
							if( pItemProp->dwBasePartsIgnore == PARTS_HEAD )
								((CModelObject*)m_pModel)->SetEffect(PARTS_HAIR, XE_HIDE );
							
							((CModelObject*)m_pModel)->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE );
						}
						else
						{
							if( pElement )
								pElement->m_nEffect &= ~XE_HIDE;
						}
					}
				}
									
			}
			else
			{
				// 외투의상을 입었을경우 머리날릴것인가의 기준을 외투 모자를 기준으로 바꾼다
				dwId	= pMover->m_aEquipInfo[PARTS_HAT].dwId;
				if( dwId != NULL_ID )
				{
					if( !(pMover->m_aEquipInfo[PARTS_HAT].byFlag & CItemElem::expired) )
					{						
						pItemProp	= prj.GetItemProp( dwId );
						if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
						{
							if( pItemProp->dwBasePartsIgnore == PARTS_HEAD )
								((CModelObject*)m_pModel)->SetEffect(PARTS_HAIR, XE_HIDE );

							((CModelObject*)m_pModel)->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE );
						}
					}
				}							
			}
			
		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[0] = pMover->m_fHairColorR;
		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[1] = pMover->m_fHairColorG;
		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[2] = pMover->m_fHairColorB;
			
		m_pModel->SetGroup( 0 );	
		m_pModel->Render( p2DRender->m_pd3dDevice, &matWorld );
	}

	return;
}
void CWndQueryEquip::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bLButtonDownRot )
	{
		m_fRot += (m_OldPos.x - point.x) * 0.5f;
	}
	
	m_OldPos = point;
}
void CWndQueryEquip::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bLButtonDownRot = FALSE;
	ReleaseCapture();
	
}
void CWndQueryEquip::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM20 );
	if( lpWndCtrl->rect.PtInRect( point ) )
	{
		m_OldPos          = point;
		m_bLButtonDownRot = TRUE;
	}
}	
	
void CWndQueryEquip::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_bLButtonDownRot = FALSE;
	m_fRot = 0.0f;
	
	LPWNDCTRL lpWndCtrl1 = GetWndCtrl( WIDC_CUSTOM1 );
	LPWNDCTRL lpWndCtrl2 = GetWndCtrl( WIDC_CUSTOM2 );
	LPWNDCTRL lpWndCtrl3 = GetWndCtrl( WIDC_CUSTOM3 );
	LPWNDCTRL lpWndCtrl4 = GetWndCtrl( WIDC_CUSTOM4 );
	LPWNDCTRL lpWndCtrl5 = GetWndCtrl( WIDC_CUSTOM5 );
	LPWNDCTRL lpWndCtrl6 = GetWndCtrl( WIDC_CUSTOM6 );
	LPWNDCTRL lpWndCtrl7 = GetWndCtrl( WIDC_CUSTOM7 );
	LPWNDCTRL lpWndCtrl8 = GetWndCtrl( WIDC_CUSTOM8 );
	LPWNDCTRL lpWndCtrl9 = GetWndCtrl( WIDC_CUSTOM9 );
	LPWNDCTRL lpWndCtrl10 = GetWndCtrl( WIDC_CUSTOM10 );
	
	m_InvenRect[6] = m_InvenRect[14] = lpWndCtrl1->rect;
	m_InvenRect[2] = m_InvenRect[15] = lpWndCtrl2->rect;
	m_InvenRect[4] = m_InvenRect[17] = lpWndCtrl3->rect;
	m_InvenRect[5] = m_InvenRect[18] = lpWndCtrl4->rect;
	
	m_InvenRect[10] = lpWndCtrl5->rect;
	m_InvenRect[9] = m_InvenRect[11] = lpWndCtrl6->rect;
	m_InvenRect[25] = lpWndCtrl7->rect;
	m_InvenRect[8] = lpWndCtrl8->rect;
	m_InvenRect[12] = lpWndCtrl9->rect;
	m_InvenRect[13] = lpWndCtrl10->rect;
	
	lpWndCtrl1 = GetWndCtrl( WIDC_CUSTOM11 );
	lpWndCtrl2 = GetWndCtrl( WIDC_CUSTOM12 );
	lpWndCtrl3 = GetWndCtrl( WIDC_CUSTOM13 );
	lpWndCtrl4 = GetWndCtrl( WIDC_CUSTOM14 );
	lpWndCtrl5 = GetWndCtrl( WIDC_CUSTOM15 );
	lpWndCtrl6 = GetWndCtrl( WIDC_CUSTOM16 );
	lpWndCtrl7 = GetWndCtrl( WIDC_CUSTOM17 );
	lpWndCtrl8 = GetWndCtrl( WIDC_CUSTOM18 );
	lpWndCtrl9 = GetWndCtrl( WIDC_CUSTOM19 );
	
	m_InvenRect[20] = lpWndCtrl1->rect;
	m_InvenRect[22] = lpWndCtrl2->rect;
	m_InvenRect[19] = lpWndCtrl3->rect;
	m_InvenRect[23] = lpWndCtrl4->rect;
	m_InvenRect[21] = lpWndCtrl5->rect;
	
	m_InvenRect[26] = lpWndCtrl6->rect;
	m_InvenRect[27] = lpWndCtrl7->rect;
	m_InvenRect[28] = lpWndCtrl8->rect;
	m_InvenRect[29] = lpWndCtrl9->rect;
	
	MoveParentCenter();
}

void CWndQueryEquip::SetMover( DWORD ObjID )
{
	m_ObjID = ObjID;

	SAFE_DELETE( m_pModel );
	
	if( GetMover() )
	{
		int nMover = (GetMover()->GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);
		m_pModel = (CModelObject*)prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE );
		prj.m_modelMng.LoadMotion( m_pModel,  OT_MOVER, nMover, MTI_STAND );
		CMover::UpdateParts( GetMover()->GetSex(), GetMover()->m_dwSkinSet, GetMover()->m_dwFace, GetMover()->m_dwHairMesh, GetMover()->m_dwHeadMesh,GetMover()->m_aEquipInfo, m_pModel, NULL );
		m_pModel->InitDeviceObjects( g_Neuz.GetDevice() );
	}
}

void CWndQueryEquip::SetEquipInfoAdd( EQUIP_INFO_ADD* aEquipInfoAdd )
{
//	memcpy( m_aEquipInfoAdd, aEquipInfoAdd, sizeof(EQUIP_INFO_ADD) * MAX_HUMAN_PARTS );
	for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
		m_aEquipInfoAdd[i]	= aEquipInfoAdd[i];

	CMover* pMover = GetMover();
	
	if( IsInvalidObj(pMover) )
		return ;
	
#ifdef __CLIENT
	for( i = 0; i < MAX_HUMAN_PARTS; i++ )
	{
		if( pMover->m_aEquipInfo[i].dwId != NULL_ID )
		{
			ItemProp* pItemProp	= prj.GetItemProp( pMover->m_aEquipInfo[i].dwId );
			if( pItemProp )
				m_aEquipInfoAdd[i].pTexture	= CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
		}
	}
#endif	// __CLIENT
}

BOOL CWndQueryEquip::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();

	CRect rect( 792, 130, 792 + 232, 130 + 405 + 20 ); // 1024 768

	// 인벤토리 출력 위치 지정
	memset( m_InvenRect, 0, sizeof(CRect) * MAX_HUMAN_PARTS );
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint( 792, 130 ), pWndParent );
}






//////////////////////////////////////////////
CWndInventory::CWndInventory()
{
	m_bReport = FALSE;
	m_pSelectItem = NULL;
	m_pWndConfirmBuy = NULL;

	m_bIsUpgradeMode = FALSE;
	m_pUpgradeItem = NULL;
	m_pUpgradeMaterialItem = NULL;
	m_pSfxUpgrade = NULL;
	m_dwEnchantWaitTime = 0xffffffff;
	m_TexRemoveItem = NULL;

	m_TabTexture = NULL;

	m_pModel = NULL;
	m_OldPos = CPoint(0,0);
	
	memset( m_InvenRect, 0, sizeof(CRect) * MAX_HUMAN_PARTS );
#if __VER >= 9 // __CSC_VER9_1
	m_pWndRemoveJewelConfirm = NULL;
	m_bRemoveJewel = FALSE;
#endif //__CSC_VER9_1
}
CWndInventory::~CWndInventory()
{
	SAFE_DELETE( m_pModel );	
	SAFE_DELETE( m_pWndConfirmBuy );

	if( m_pSfxUpgrade )
	{
		m_pSfxUpgrade->Delete();
		m_pSfxUpgrade = NULL;
	}
#if __VER >= 9 // __CSC_VER9_1
	SAFE_DELETE(m_pWndRemoveJewelConfirm);
#endif //__CSC_VER9_1
}

void CWndInventory::OnDestroy( void )
{
	SAFE_DELETE( m_pModel );
	SAFE_DELETE( m_pWndConfirmBuy );	
#if __VER >= 8 // __CSC_VER8_5
	CWndSummonAngel* pWndAngel = (CWndSummonAngel*)GetWndBase( APP_SUMMON_ANGEL );
	if(pWndAngel != NULL)
		pWndAngel->Destroy();
#endif // __CSC_VER8_5

#ifdef __EVE_MINIGAME
	CWndFindWordGame* pWndWordGame = (CWndFindWordGame*)GetWndBase( APP_MINIGAME_WORD );
	if(pWndWordGame != NULL)
		pWndWordGame->Destroy();
	
	CWndPuzzleGame* pWndPuzzleGame = (CWndPuzzleGame*)GetWndBase( APP_MINIGAME_PUZZLE );
	if(pWndPuzzleGame != NULL)
		pWndPuzzleGame->Destroy();
#endif //__EVE_MINIGAME
#if __VER >= 9 // __CSC_VER9_1
	CWndMixJewel* pWndMixJewel = (CWndMixJewel*)GetWndBase( APP_SMELT_MIXJEWEL );
	if(pWndMixJewel != NULL)
		pWndMixJewel->Destroy();

	CWndExtraction* pWndExtraction = (CWndExtraction*)GetWndBase( APP_SMELT_EXTRACTION );
	if(pWndExtraction != NULL)
		pWndExtraction->Destroy();
#endif //__CSC_VER9_1

#ifdef __WINDOW_INTERFACE_BUG
	CWndPiercing* pWndPiercing = (CWndPiercing*)GetWndBase( APP_PIERCING );
	if(pWndPiercing != NULL)
		pWndPiercing->Destroy();
	CWndRemoveAttribute* pWndRemoveAttribute = (CWndRemoveAttribute*)GetWndBase( APP_REMOVE_ATTRIBUTE );
	if(pWndRemoveAttribute != NULL)
		pWndRemoveAttribute->Destroy();
	CWndRemovePiercing* pWndRemovePiercing = (CWndRemovePiercing*)GetWndBase( APP_SMELT_REMOVE_PIERCING_EX );
	if(pWndRemovePiercing != NULL)
		pWndRemovePiercing->Destroy();
	CWndRemoveJewel* pWndRemoveJewel = (CWndRemoveJewel*)GetWndBase( APP_SMELT_REMOVE_JEWEL );
	if(pWndRemoveJewel != NULL)
		pWndRemoveJewel->Destroy();
	CWndLvReqDown* pWndLvReqDown = (CWndLvReqDown*)GetWndBase( APP_LVREQDOWN );
	if(pWndLvReqDown != NULL)
		pWndLvReqDown->Destroy();
	CWndBlessingCancel* pWndBlessingCancel = (CWndBlessingCancel*)GetWndBase( APP_CANCEL_BLESSING );
	if(pWndBlessingCancel != NULL)
		pWndBlessingCancel->Destroy();
	CWndUpgradeBase* pWndUpgradeBase = (CWndUpgradeBase*)GetWndBase( APP_TEST );
	if(pWndUpgradeBase != NULL)
		pWndUpgradeBase->Destroy();
#endif // __WINDOW_INTERFACE_BUG

#if __VER >= 14 // __SMELT_SAFETY
	CWndSmeltSafety* pWndSmeltSafety = (CWndSmeltSafety*)GetWndBase( APP_SMELT_SAFETY );
	if(pWndSmeltSafety != NULL)
		pWndSmeltSafety->Destroy();

	CWndSmeltSafetyConfirm* pWndSmeltSafetyConfirm = (CWndSmeltSafetyConfirm*)GetWndBase( APP_SMELT_SAFETY_CONFIRM );
	if(pWndSmeltSafetyConfirm != NULL)
		pWndSmeltSafetyConfirm->Destroy();
#endif //__SMELT_SAFETY

#if __VER >= 14 // __EQUIP_BIND
	CWndEquipBindConfirm* pWndEquipBindConfirm = (CWndEquipBindConfirm*)GetWndBase(APP_EQUIP_BIND_CONFIRM);
	if(pWndEquipBindConfirm != NULL)
		pWndEquipBindConfirm->Destroy();
#endif // __EQUIP_BIND

#if __VER >= 14 // __RESTATE_CONFIRM
	CWndRestateConfirm* pWndRestateConfirm = (CWndRestateConfirm*)GetWndBase(APP_RESTATE_CONFIRM);
	if(pWndRestateConfirm != NULL)
		pWndRestateConfirm->Destroy();
#endif // __RESTATE_CONFIRM

#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
	CWndPetFoodMill* pWndPetFoodMill = ( CWndPetFoodMill* )GetWndBase( APP_PET_FOODMILL );
	if(pWndPetFoodMill != NULL)
		pWndPetFoodMill->Destroy();
#endif // __IMPROVE_SYSTEM_VER15

	CWndShop* pWndShop = ( CWndShop* )GetWndBase( APP_SHOP_ );
	if( pWndShop != NULL )
		pWndShop->Destroy();

	CWndBank* pWndBank = ( CWndBank* )GetWndBase( APP_COMMON_BANK );
	if( pWndBank != NULL )
		pWndBank->Destroy();
}
void CWndInventory::OnMouseWndSurface( CPoint point )
{
	int nTemp = 0;
	for( int i=2; i<MAX_HUMAN_PARTS; i++ )
	{
		CItemBase* pItemBase = NULL;
		
		pItemBase = g_pPlayer->GetEquipItem( i );
		
		CRect DrawRect = m_InvenRect[i];
		
		CPoint point = GetMousePoint();
		// 툴팁
		if( DrawRect.PtInRect( point ) )
		{
			nTemp = i;
			
			if( i == PARTS_LWEAPON )
			{
				pItemBase = g_pPlayer->GetEquipItem( PARTS_SHIELD );
				
				if( pItemBase )
				{
					nTemp = PARTS_SHIELD;
				}
				else
				{
					pItemBase = g_pPlayer->GetEquipItem( PARTS_LWEAPON );
					
					if( pItemBase == NULL )
						pItemBase = g_pPlayer->GetEquipItem( PARTS_RWEAPON );
				}
			}			

			if( pItemBase )
			{
				CPoint point2 = point;
				ClientToScreen( &point2 );
				ClientToScreen( &DrawRect );
				
				// 장비창에 있는것 툴팁
#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
				g_toolTip.SetSubToolTipNumber( 0 );
#endif // __IMPROVE_SYSTEM_VER15
				g_WndMng.PutToolTip_Item( pItemBase, point2, &DrawRect, APP_INVENTORY );
				break;
			}
			else
			{
				// 장비창에 없는것 툴팁
				
				CPoint point2 = point;
				ClientToScreen( &point2 );
				ClientToScreen( &DrawRect );
				
				CString strText;

				switch( nTemp )
				{
					case PARTS_UPPER_BODY:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_SUIT);
						break;
					case PARTS_HAND:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_GAUNTLET);
						break;
					case PARTS_FOOT:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_BOOTS );
						break;
					case PARTS_CAP:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_HELMET );
						break;
					case PARTS_CLOAK:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_CLO);
						break;
					case PARTS_LWEAPON:
					case PARTS_RWEAPON:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_WEAPON);
						break;
					case PARTS_SHIELD:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_GUARD  );
						break;
					case PARTS_MASK:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_MASK );
						break;
					case PARTS_RIDE:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_RID );
						break;
					case PARTS_NECKLACE1:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_NECKLACE  );
						break;
					case PARTS_RING1:
					case PARTS_RING2:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_RING   );
						break;
					case PARTS_EARRING1:
					case PARTS_EARRING2:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_EARRING   );
						break;
					case PARTS_BULLET:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_BULLET    );
						break;							
					case PARTS_HAT:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_CLOTHES_01     );
						break;	
					case PARTS_CLOTH:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_CLOTHES_02     );
						break;	
					case PARTS_GLOVE:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_CLOTHES_03     );
						break;	
					case PARTS_BOOTS:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_CLOTHES_04     );
						break;							
				}

				g_toolTip.PutToolTip( 100000, strText, DrawRect, point2 );
				break;
			}
//			break;
		}
	}
}
void CWndInventory::OnDraw(C2DRender* p2DRender)
{
	CMover* pMover = g_pPlayer;
	if( !pMover )
		return;

	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

	// 뷰포트 세팅 
	D3DVIEWPORT9 viewport;

	viewport.X      = p2DRender->m_ptOrigin.x + 5;
	viewport.Y      = p2DRender->m_ptOrigin.y + 5;
	viewport.Width  = p2DRender->m_clipRect.Width() - 100;
	viewport.Height = 182;
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;

	CString strGoldNum;
	CWndStatic* pGoldNum = (CWndStatic*) GetDlgItem( WIDC_GOLD_NUM );
	CWndTrade * pWndTrade = (CWndTrade *)g_WndMng.GetWndBase( APP_TRADE );
	strGoldNum.Format( _T( "%d" ), g_pPlayer->GetGold() );

	LPWNDCTRL lpWndCtrl1 = GetWndCtrl( WIDC_CUSTOM21 );
	if( lpWndCtrl1 && m_TexRemoveItem )
	{
		CPoint point = GetMousePoint();
		float fScal = 1.0f;
		if( lpWndCtrl1->rect.PtInRect( point ) )
		{
			fScal = 1.2f;
			CPoint point2 = point;
			CRect DrawRect = lpWndCtrl1->rect;
			ClientToScreen( &point2 );
			ClientToScreen( &DrawRect );
			g_toolTip.PutToolTip( 100, prj.GetText(TID_GAME_INVEONTORY_REMOVE_TOOL), DrawRect, point2, 0 );
		}
		if(m_TexRemoveItem)
			m_TexRemoveItem->Render( p2DRender, lpWndCtrl1->rect.TopLeft(), CPoint( 27, 27 ), 255, fScal, fScal );
	}

	CWndTabCtrl* pTabCtrl = (CWndTabCtrl*) GetDlgItem( WIDC_INVENTORY );
	//m_TabTexture->Render( p2DRender, CPoint( pTabCtrl->GetWndRect().left ,pTabCtrl->GetWndRect().bottom - 20 ), 255 );

	pGoldNum->SetTitle( strGoldNum );

	DWORD dwColor1 = D3DCOLOR_ARGB( 100, 0, 0,  0 );//D3DCOLOR_TEMP( 255,   0,   0,  50 );//
	DWORD dwColor2 = D3DCOLOR_ARGB( 255, 240, 240,  240 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//
	DWORD dwColor3 = D3DCOLOR_ARGB( 100, 200, 200,  200 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//

	for( int i=2; i<MAX_HUMAN_PARTS; i++ )
	{
		DWORD dwAlpha = 255;
		CItemBase* pItemBase = g_pPlayer->GetEquipItem( i );
		if( i == PARTS_LWEAPON )		// 왼손무기 그릴타이밍일때
		{
			CItemBase* pRWeapon = g_pPlayer->GetEquipItem( PARTS_RWEAPON );		// 오른손 무기를 꺼내보고
			if( pRWeapon && pRWeapon->GetProp()->dwHanded == HD_TWO )	// 투핸드 무기면
			{
				pItemBase = pRWeapon;	// 오른손무기랑 같은걸 그리자.
				dwAlpha   = 100;
			}
		}

		FLOAT sx = 1.0f;
		FLOAT sy = 1.0f;

		CPoint cpAdd = CPoint(6,6);
		CRect DrawRect = m_InvenRect[i];

		if( i >= PARTS_NECKLACE1 && i <= PARTS_EARRING2 )
		{
			cpAdd = CPoint(0,0);

			sx = 0.8f;
			sy = 0.8f;
		}
		else
		if( i >= PARTS_HAT && i <= PARTS_BOOTS )
		{
			cpAdd = CPoint(0,0);
			
			sx = 0.9f;
			sy = 0.9f;
		}
		
		
		if( pItemBase && pItemBase->GetTexture() )
		{
			if( ((CItemElem*)pItemBase)->IsFlag( CItemElem::expired ) )
			{
				pItemBase->GetTexture()->Render2(p2DRender, DrawRect.TopLeft()+cpAdd, D3DCOLOR_XRGB( 255, 100, 100 ), sx, sy );
			}
			else
			{
				pItemBase->GetTexture()->Render2(p2DRender, DrawRect.TopLeft()+cpAdd, D3DCOLOR_ARGB( dwAlpha, 255, 255, 255 ), sx, sy );
			}

			CItemElem *pItemElem = (CItemElem *)pItemBase;
			if( pItemElem->GetProp()->dwPackMax > 1 )		// 묶음 아이템이냐?
			{
				short nItemNum	= pItemElem->m_nItemNum;

				TCHAR szTemp[ 32 ];
				_stprintf( szTemp, "%d", nItemNum );
				CSize size = m_p2DRender->m_pFont->GetTextExtent( szTemp );
				int x = DrawRect.left;	
				int y = DrawRect.top;
				m_p2DRender->TextOut( x + 42 - size.cx, y + 42 - size.cy, szTemp, 0xff00220b );
				m_p2DRender->TextOut( x + 41 - size.cx, y + 41 - size.cy, szTemp, 0xff00ffff );
			}
		} 
	}

	if( g_pPlayer == NULL || m_pModel == NULL )
		return;

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	//pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	//pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

	pd3dDevice->SetRenderState( D3DRS_AMBIENT,  D3DCOLOR_ARGB( 255, 255,255,255) );
	
	CRect rect = GetClientRect();

	// 뷰포트 세팅 
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
	
	#ifdef __YENV
	D3DXVECTOR3 vDir( 0.0f, 0.0f, 1.0f );
	SetLightVec( vDir );
	#endif //__YENV
	
	{
		LPWNDCTRL lpFace = GetWndCtrl( WIDC_CUSTOM20 );

		viewport.X      = p2DRender->m_ptOrigin.x + lpFace->rect.left;
		viewport.Y      = p2DRender->m_ptOrigin.y + lpFace->rect.top;
		viewport.Width  = lpFace->rect.Width();
		viewport.Height = lpFace->rect.Height();

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
		D3DXMatrixIdentity(&matRot);
		D3DXMatrixIdentity(&matTrans);
		D3DXMatrixIdentity(&matWorld);
		
		D3DXMatrixRotationY( &matRot, D3DXToRadian( m_fRot ) );
		
		D3DXMatrixScaling(&matScale,1.2f,1.2f,1.2f);
		D3DXMatrixTranslation(&matTrans,0.0f,-0.6f,0.0f);
		
		D3DXMatrixMultiply(&matWorld,&matWorld,&matScale);
		D3DXMatrixMultiply(&matWorld, &matWorld, &matRot );
		D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans );
		pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

		// 랜더링 
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );//m_bViewLight );
		
		::SetLight( FALSE );
		::SetFog( FALSE );
		SetDiffuse( 1.0f, 1.0f, 1.0f );
		SetAmbient( 1.0f, 1.0f, 1.0f );

		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[0] = g_pPlayer->m_fHairColorR;
		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[1] = g_pPlayer->m_fHairColorG;
		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[2] = g_pPlayer->m_fHairColorB;

		D3DXVECTOR4 vConst( 1.0f, 1.0f, 1.0f, 1.0f );
	#ifdef __YENV
		g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvFog, &vConst );
	#else //__YENV						
		pd3dDevice->SetVertexShaderConstantF( 95, (float*)&vConst, 1 );
	#endif //__YENV
		::SetTransformView( matView );
		::SetTransformProj( matProj );
	
	if( g_pPlayer )
		g_pPlayer->OverCoatItemRenderCheck(m_pModel);
		
		// 헬멧이 머리카락 날려야하는것이냐?  // 인벤이 있는 경우 
		CItemElem* pItemElem	= g_pPlayer->GetEquipItem( PARTS_CAP );
		if( pItemElem )
		{
			O3D_ELEMENT* pElement = NULL;
			ItemProp* pItemProp = pItemElem->GetProp();
			if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
			{
				pElement = m_pModel->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE);
			}

			// 외투의상을 입었을경우 머리날릴것인가의 기준을 외투 모자를 기준으로 바꾼다
			CItemElem* pItemElemOvercoat	= g_pPlayer->GetEquipItem( PARTS_HAT );
			
			if( pItemElemOvercoat )
			{
				if( !(pItemElemOvercoat->IsFlag( CItemElem::expired )) )
				{
					ItemProp* pItemPropOC = pItemElemOvercoat->GetProp();
					if( pItemPropOC && pItemPropOC->dwBasePartsIgnore != -1 )
					{
						if( pItemPropOC->dwBasePartsIgnore == PARTS_HEAD )
							m_pModel->SetEffect(PARTS_HAIR, XE_HIDE );
						
						m_pModel->SetEffect(pItemPropOC->dwBasePartsIgnore, XE_HIDE);
					}
					else
					{
						if( pElement )
							pElement->m_nEffect &= ~XE_HIDE;
					}
				}
				else
				{
					//m_pModel->SetEffectOff(PARTS_HAIR, XE_HIDE );
					if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
					{
						m_pModel->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE);
					}
					
					m_pModel->SetEffectOff(PARTS_HEAD, XE_HIDE );
				}
			}						
		}
		else
		{
			// 외투의상을 입었을경우 머리날릴것인가의 기준을 외투 모자를 기준으로 바꾼다
			CItemElem* pItemElemOvercoat	= g_pPlayer->GetEquipItem( PARTS_HAT );
			
			if( pItemElemOvercoat )
			{
				if( !(pItemElemOvercoat->IsFlag( CItemElem::expired )) )
				{
					ItemProp* pItemPropOC = pItemElemOvercoat->GetProp();
					if( pItemPropOC && pItemPropOC->dwBasePartsIgnore != -1 )
					{
						if( pItemPropOC->dwBasePartsIgnore == PARTS_HEAD )
							m_pModel->SetEffect(PARTS_HAIR, XE_HIDE );
						
						m_pModel->SetEffect(pItemPropOC->dwBasePartsIgnore, XE_HIDE);
					}
				}
			}
		}
		
		m_pModel->Render( p2DRender->m_pd3dDevice, &matWorld );
	}

	return;
}

void CWndInventory::UpDateModel()
{
	SAFE_DELETE( m_pModel );
	
	int nMover = (g_pPlayer->GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);
	m_pModel = (CModelObject*)prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE );
	prj.m_modelMng.LoadMotion( m_pModel,  OT_MOVER, nMover, MTI_STAND );
	UpdateParts();
	m_pModel->InitDeviceObjects( g_Neuz.GetDevice() );	
}


void CWndInventory::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_bLButtonDownRot = FALSE;
	m_fRot = 0.0f;

	LPWNDCTRL lpWndCtrl1 = GetWndCtrl( WIDC_CUSTOM1 );
	LPWNDCTRL lpWndCtrl2 = GetWndCtrl( WIDC_CUSTOM2 );
	LPWNDCTRL lpWndCtrl3 = GetWndCtrl( WIDC_CUSTOM3 );
	LPWNDCTRL lpWndCtrl4 = GetWndCtrl( WIDC_CUSTOM4 );
	LPWNDCTRL lpWndCtrl5 = GetWndCtrl( WIDC_CUSTOM5 );
	LPWNDCTRL lpWndCtrl6 = GetWndCtrl( WIDC_CUSTOM6 );
	LPWNDCTRL lpWndCtrl7 = GetWndCtrl( WIDC_CUSTOM7 );
	LPWNDCTRL lpWndCtrl8 = GetWndCtrl( WIDC_CUSTOM8 );
	LPWNDCTRL lpWndCtrl9 = GetWndCtrl( WIDC_CUSTOM9 );
	LPWNDCTRL lpWndCtrl10 = GetWndCtrl( WIDC_CUSTOM10 );
	
	m_InvenRect[6] = m_InvenRect[14] = lpWndCtrl1->rect;
	m_InvenRect[2] = m_InvenRect[15] = lpWndCtrl2->rect;
	m_InvenRect[4] = m_InvenRect[17] = lpWndCtrl3->rect;
	m_InvenRect[5] = m_InvenRect[18] = lpWndCtrl4->rect;

	m_InvenRect[10] = lpWndCtrl5->rect;
	m_InvenRect[9] = m_InvenRect[11] = lpWndCtrl6->rect;
	m_InvenRect[25] = lpWndCtrl7->rect;
	m_InvenRect[8] = lpWndCtrl8->rect;
	m_InvenRect[12] = lpWndCtrl9->rect;
	m_InvenRect[13] = lpWndCtrl10->rect;

	lpWndCtrl1 = GetWndCtrl( WIDC_CUSTOM11 );
	lpWndCtrl2 = GetWndCtrl( WIDC_CUSTOM12 );
	lpWndCtrl3 = GetWndCtrl( WIDC_CUSTOM13 );
	lpWndCtrl4 = GetWndCtrl( WIDC_CUSTOM14 );
	lpWndCtrl5 = GetWndCtrl( WIDC_CUSTOM15 );
	lpWndCtrl6 = GetWndCtrl( WIDC_CUSTOM16 );
	lpWndCtrl7 = GetWndCtrl( WIDC_CUSTOM17 );
	lpWndCtrl8 = GetWndCtrl( WIDC_CUSTOM18 );
	lpWndCtrl9 = GetWndCtrl( WIDC_CUSTOM19 );
	
	m_InvenRect[20] = lpWndCtrl1->rect;
	m_InvenRect[22] = lpWndCtrl2->rect;
	m_InvenRect[19] = lpWndCtrl3->rect;
	m_InvenRect[23] = lpWndCtrl4->rect;
	m_InvenRect[21] = lpWndCtrl5->rect;

	m_InvenRect[26] = lpWndCtrl6->rect;
	m_InvenRect[27] = lpWndCtrl7->rect;
	m_InvenRect[28] = lpWndCtrl8->rect;
	m_InvenRect[29] = lpWndCtrl9->rect;

	SAFE_DELETE( m_pModel );

	int nMover = (g_pPlayer->GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);
	m_pModel = (CModelObject*)prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE );
	prj.m_modelMng.LoadMotion( m_pModel,  OT_MOVER, nMover, MTI_STAND );

	UpdateParts();
	m_pModel->InitDeviceObjects( g_Neuz.GetDevice() );

#ifdef __TABBED_INV
    CWndTabCtrl* pTabCtrl = (CWndTabCtrl*) GetDlgItem( WIDC_INVENTORY );
	m_wndInvCtrl[0].Create( WLVS_ICON, CRect( 0, 0, 250, 250 ), pTabCtrl, 21 );
	m_wndInvCtrl[1].Create( WLVS_ICON, CRect( 0, 0, 250, 250 ), pTabCtrl, 22 );
	m_wndInvCtrl[2].Create( WLVS_ICON, CRect( 0, 0, 250, 250 ), pTabCtrl, 23 );
	m_wndInvCtrl[3].Create( WLVS_ICON, CRect( 0, 0, 250, 250 ), pTabCtrl, 24 );


	m_wndInvCtrl[0].InitItem( &g_pPlayer->m_Inventory, APP_INVENTORY );
	m_wndInvCtrl[1].InitItem( &g_pPlayer->m_Inventory, APP_INVENTORY );
	m_wndInvCtrl[2].InitItem( &g_pPlayer->m_Inventory, APP_INVENTORY );
	m_wndInvCtrl[3].InitItem( &g_pPlayer->m_Inventory, APP_INVENTORY );

	WTCITEM tabTabItem;
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;

	tabTabItem.pszText = GETTEXT( TID_GAME_ITEM );
	tabTabItem.pWndBase = &m_wndInvCtrl[0];
	pTabCtrl->InsertItem( 0, &tabTabItem);

	tabTabItem.pszText = GETTEXT( TID_INVEN_PET );
	tabTabItem.pWndBase = &m_wndInvCtrl[1];
	pTabCtrl->InsertItem( 1, &tabTabItem);

	tabTabItem.pszText = GETTEXT( TID_INVEN_COSTUME );
	tabTabItem.pWndBase = &m_wndInvCtrl[2];
	pTabCtrl->InsertItem( 2, &tabTabItem);

	tabTabItem.pszText = GETTEXT( TID_INVEN_QUEST );
	tabTabItem.pWndBase = &m_wndInvCtrl[3];
	pTabCtrl->InsertItem( 3, &tabTabItem);

#else//__TABBED_INV

	CWndTabCtrl* pTabCtrl = (CWndTabCtrl*) GetDlgItem( WIDC_INVENTORY );
	m_wndItemCtrl.Create( WLVS_ICON, CRect( 0, 0, 250, 250 ), pTabCtrl, 11 );
	m_wndItemCtrl.InitItem( &g_pPlayer->m_Inventory, APP_INVENTORY );

	WTCITEM tabTabItem;
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = GETTEXT( TID_GAME_ITEM );//"아이템";
	tabTabItem.pWndBase = &m_wndItemCtrl;
	pTabCtrl->InsertItem( 0, &tabTabItem );
#endif//__TABBED_INV
	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_GOLD );
	m_wndGold.Create( "g", WBS_NODRAWFRAME, lpWndCtrl->rect, this, WIDC_GOLD );
	m_wndGold.AddWndStyle( WBS_NODRAWFRAME );

	CWndStatic* pGoldNum = (CWndStatic*) GetDlgItem( WIDC_GOLD_NUM );
	pGoldNum->m_dwColor = D3DCOLOR_ARGB( 255, 238, 221, 130 );
	pGoldNum->AddWndStyle(WSS_MONEY);

	m_pUpgradeItem = NULL;
	m_pUpgradeMaterialItem = NULL;
	m_bIsUpgradeMode = FALSE;
	m_dwEnchantWaitTime = 0xffffffff;

	m_TexRemoveItem = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "WndInventoryGarbage.dds" ), 0xffff00ff );
	m_TabTexture = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "WndInventab02.tga" ), 0xffff00ff );

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 112 + 48 );
	Move( point );
}
BOOL CWndInventory::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 792, 130, 792 + 232, 130 + 405 + 20 ); // 1024 768

	// 인벤토리 출력 위치 지정
	memset( m_InvenRect, 0, sizeof(CRect) * MAX_HUMAN_PARTS );
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint( 792, 130 ), pWndParent );
//	return CWndNeuz::Create( WBS_VIEW | WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_THICKFRAME, rect, pWndParent, dwWndId );
}

BOOL CWndInventory::Process()
{
	if( m_pModel )
		m_pModel->FrameMove();


/*	ItemProp* pItemProp = NULL;
	CItemElem* pTempElem = NULL;
	pItemProp = pTempElem->GetProp();

	int nIndex = pItemProp->GetInvenIndex();

	g_pPlayer->m_InventoryTAB[nIndex];*/

	if( m_dwEnchantWaitTime < g_tmCurrent )
	{
		m_dwEnchantWaitTime = 0xffffffff;
		
		if( m_pSfxUpgrade )
		{
			m_pSfxUpgrade->Delete();
			m_pSfxUpgrade = NULL;
		}
		
		CItemElem* pItemElem = (CItemElem*)m_pUpgradeItem;
		if( pItemElem && m_pUpgradeMaterialItem )
		{
			CItemElem* pItemMaterialElem = (CItemElem*)m_pUpgradeMaterialItem;
			// 패킷 전송
			
			if( pItemMaterialElem->GetProp() )
			{
				// 인첸트에 관한 아이템이냐?
#if __VER >= 8 //__Y_NEW_ENCHANT
				if( pItemMaterialElem->GetProp()->dwItemKind3 == IK3_ELECARD 
					|| pItemMaterialElem->GetProp()->dwItemKind3 == IK3_ENCHANT
#if __VER >= 9 // __ULTIMATE
					|| pItemMaterialElem->GetProp()->dwItemKind3 == IK3_PIERDICE
#endif // __ULTIMATE			
					)
#else //__Y_NEW_ENCHANT
				if( pItemMaterialElem->GetProp()->dwItemKind3 == IK3_ELECARD || pItemMaterialElem->GetProp()->dwItemKind3 == IK3_DICE )
#endif //__Y_NEW_ENCHANT
				{
#if __VER >= 9 // __CSC_VER9_1
					if(pItemMaterialElem->m_dwItemId == II_GEN_MAT_ORICHALCUM02)
						g_DPlay.SendUltimateEnchantWeapon( pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId );
					else if(pItemMaterialElem->m_dwItemId == II_GEN_MAT_MOONSTONE || pItemMaterialElem->m_dwItemId == II_GEN_MAT_MOONSTONE_1)
					{
#if __VER >= 11 // __SYS_COLLECTING
						if( pItemElem->IsCollector( TRUE ) || pItemElem->GetProp()->dwItemKind2 == IK2_JEWELRY )
							g_DPlay.SendEnchant( pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId );
#if __VER < 12 // __CSC_VER12_4
						else
							g_DPlay.SendUltimateRemoveGem( pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId );
#endif //__CSC_VER12_4
#else	// __SYS_COLLECTING
						g_DPlay.SendUltimateRemoveGem( pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId );
#endif	// __SYS_COLLECTING
					}
					else
						g_DPlay.SendEnchant( pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId );
#else
					g_DPlay.SendEnchant( pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId );
#endif //__CSC_VER9_1
				}
				else
				// 피어싱에 관한 아이템이냐?
				if( pItemMaterialElem->GetProp()->dwItemKind3 == IK3_SOCKETCARD
#if __VER >= 12 // __EXT_PIERCING
					|| pItemMaterialElem->GetProp()->dwItemKind3 == IK3_SOCKETCARD2
#endif // __EXT_PIERCING
					)
				{
					g_DPlay.SendPiercing( pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId );
				}
#if __VER >= 11
				else if( IsNeedTarget( pItemMaterialElem->GetProp() ) )
				{
					g_DPlay.SendDoUseItemTarget( m_pUpgradeMaterialItem->m_dwObjId, pItemElem->m_dwObjId );
				}
#endif	// __VER
				else
				if( pItemMaterialElem->GetProp()->dwItemKind3 == IK3_RANDOM_SCROLL )
				{
					SAFE_DELETE( g_WndMng.m_pWndRandomScrollConfirm );
					g_WndMng.m_pWndRandomScrollConfirm = new CWndRandomScrollConfirm;
					if( 0 < pItemElem->GetRandomOpt() )
						g_WndMng.m_pWndRandomScrollConfirm->SetItem( pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId, TRUE );
					else
						g_WndMng.m_pWndRandomScrollConfirm->SetItem( pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId );
					g_WndMng.m_pWndRandomScrollConfirm->Initialize();
				}
			}
		}
	}
	return TRUE;
}

BOOL CWndInventory::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	// 인벤토리로 아이템이 Drop되었다. 어디서 왔는지 확인한후 적절히 대응하라.
	if( message == WIN_ITEMDROP )
	{
		LPSHORTCUT lpShortcut = (LPSHORTCUT)pLResult;
		if( lpShortcut->m_pFromWnd == NULL )
		{
			ADDERRORMSG( "CWndInventory::OnChildNotify : m_pFromWnd 가 NULL " );
			return CWndNeuz::OnChildNotify( message, nID, pLResult );
		}
		CWndBase* pWndFrame = lpShortcut->m_pFromWnd->GetFrameWnd();

		if( pWndFrame == NULL )
		{
			LPCTSTR szErr = Error( "CWndInventory::OnChildNotify : pWndFrame==NULL" );
			ADDERRORMSG( szErr );
		}
		BOOL bForbid = TRUE;
		if( pWndFrame && nID == 11 ) // item
		{
			if( pWndFrame->GetWndId() == GetWndId() && lpShortcut->m_dwData == 0 )
			{
				SetForbid( TRUE );
				return FALSE;
			}
			
			// 장착과 탈착이 이루어질 것이다.
			if( pWndFrame->GetWndId() == GetWndId() && lpShortcut->m_dwIndex != II_GOLD_SEED1 )
			{

				int nPart = -1;
				if( m_pSelectItem )
					nPart = m_pSelectItem->m_dwObjIndex - MAX_INVENTORY;

				g_DPlay.SendDoUseItem( MAKELONG( ITYPE_ITEM, lpShortcut->m_dwId ), 0, nPart );
				bForbid = FALSE;
			}
			else
			// 숍에서 온거라면 구입 
			
			if( pWndFrame->GetWndId() == APP_SHOP_ )
			{
				CWndItemCtrl* pWndItemCtrl = (CWndItemCtrl*)lpShortcut->m_pFromWnd;
				for( int i = 0; i < (int)( pWndItemCtrl->GetSelectedCount() ); i++ )
				{
					int nItem = pWndItemCtrl->GetSelectedItem( i );
					pWndItemCtrl->GetItem( nItem );
				}
#if __VER >= 11 // __CSC_VER11_3
				CWndShop* pwndShop = (CWndShop*)g_WndMng.GetWndBase(APP_SHOP_);
				if(pwndShop)
				{
					LPCHARACTER lpCharacter = pwndShop->m_pMover->GetCharacter();
					if(lpCharacter->m_nVenderType == 1)
					{
						if(g_pPlayer->m_Inventory.GetAtItemNum( II_CHP_RED ) - (int)((CItemElem*)lpShortcut->m_dwData)->GetChipCost() >= 0) //구매하려는 품목의 칩개수 이상을 가지고 있는지 확인.
						{
							SAFE_DELETE( m_pWndConfirmBuy );
							m_pWndConfirmBuy = new CWndConfirmBuy;
							m_pWndConfirmBuy->m_pItemElem = (CItemElem*)lpShortcut->m_dwData;
							m_pWndConfirmBuy->m_nBuyType = 1;
							m_pWndConfirmBuy->Initialize( this, APP_CONFIRM_BUY_ );
							bForbid = FALSE;
						}
						else
							g_WndMng.OpenMessageBox( _T( prj.GetText(TID_GAME_CANNT_BY_REDCHIP) ) );
					}
					else if(lpCharacter->m_nVenderType == 0)
					{
#ifdef __SHOP_COST_RATE
						if( g_pPlayer->GetGold() - ( ( ( CItemElem* )lpShortcut->m_dwData )->GetCost() * prj.m_fShopBuyRate ) >= 0 )
#else // __SHOP_COST_RATE
						if( g_pPlayer->GetGold() - ((CItemElem*)lpShortcut->m_dwData)->GetCost() >= 0 )
#endif // __SHOP_COST_RATE
#else //__CSC_VER11_3
						if( g_pPlayer->GetGold() - ((CItemElem*)lpShortcut->m_dwData)->GetCost() >= 0)
#endif //__CSC_VER11_3
						{
							SAFE_DELETE( m_pWndConfirmBuy );
							m_pWndConfirmBuy = new CWndConfirmBuy;
							m_pWndConfirmBuy->m_pItemElem = (CItemElem*)lpShortcut->m_dwData;
							m_pWndConfirmBuy->Initialize( this, APP_CONFIRM_BUY_ );
							bForbid = FALSE;
						}
						else
							g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0048) ) );
#if __VER >= 11 // __CSC_VER11_3
					}
				}
#endif //__CSC_VER11_3
			}
#if __VER >= 8 // __S8_VENDOR_REVISION
			else if( pWndFrame->GetWndId() == APP_VENDOR_REVISION )
#else // __VER >= 8 // __S8_VENDOR_REVISION
			else if( pWndFrame->GetWndId() == APP_VENDOREX )
#endif // __VER >= 8 // __S8_VENDOR_REVISION
			{
				CWndVendor* pWndVendor	= (CWndVendor*)pWndFrame;
#if __VER >= 8 // __S8_VENDOR_REVISION
				if( pWndVendor->m_pVendor == g_pPlayer )
				{
					g_DPlay.SendUnregisterPVendorItem( (BYTE)( lpShortcut->m_dwIndex ) );
				}
				else
#endif // __VER >= 8 // __S8_VENDOR_REVISION
				{
					SAFE_DELETE( pWndVendor->m_pWndVendorBuy );
					pWndVendor->m_pWndVendorBuy		= new CWndVendorBuy( (CItemBase*)lpShortcut->m_dwData, lpShortcut->m_dwIndex/*nItem*/ );
					pWndVendor->m_pWndVendorBuy->Initialize( pWndVendor, APP_VENDOR_BUY );
				}
				bForbid		= FALSE;
			}
			else
			// 뱅크에서 온거라면 빼옴
			if( pWndFrame->GetWndId() == APP_COMMON_BANK )
			{
				CWndBase* pWndTaget = pWndFrame->m_pCurFocus;
				BYTE nSlot;

				SAFE_DELETE( g_WndMng.m_pWndTradeGold );
				if( lpShortcut->m_dwData != 0 )
				{
					if( pWndTaget == &(g_WndMng.m_pWndBank->m_wndItemCtrl[0]) )
					{					
						nSlot = 0;
					}
					else if( pWndTaget == &(g_WndMng.m_pWndBank->m_wndItemCtrl[1]) )
					{
						nSlot = 1;
					}
					else
					{
						nSlot = 2;
					}
					
					CWndItemCtrl* pWndItemCtrl = (CWndItemCtrl*)lpShortcut->m_pFromWnd;
					
					for( int i = 0; i < (int)( pWndItemCtrl->GetSelectedCount() ); i++ )
					{
						int nItem = pWndItemCtrl->GetSelectedItem( i );
						pWndItemCtrl->GetItem( nItem );
					}

					CItemElem* itemElem = (CItemElem*)lpShortcut->m_dwData;
					if( itemElem->m_nItemNum > 1 )
					{ 
						g_WndMng.m_pWndTradeGold = new CWndTradeGold;
						memcpy( &g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT) );
						g_WndMng.m_pWndTradeGold->m_dwGold = itemElem->m_nItemNum;
						g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_GOLD; // 인벤토리 인데 쓰고 있어서 Gold로 씀.
						g_WndMng.m_pWndTradeGold->m_pWndBase = this;
						g_WndMng.m_pWndTradeGold->m_nSlot = nSlot;
						
						g_WndMng.m_pWndTradeGold->Initialize( &g_WndMng, APP_TRADE_GOLD );
						g_WndMng.m_pWndTradeGold->MoveParentCenter();
						CWndStatic* pStatic	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC );
						CWndStatic* pStaticCount	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_CONTROL1 );
						CString strMain = prj.GetText( TID_GAME_MOVECOUNT );//"몇개를 이동하시겠습니까?";
						CString strCount = prj.GetText(TID_GAME_NUMCOUNT);//" 갯수 : ";
						pStatic->m_strTitle = strMain;
						pStaticCount->m_strTitle = strCount;
					}
					else
					{
						g_DPlay.SendGetItemBank( nSlot, (BYTE)( lpShortcut->m_dwId ), 1 );
					}
				}
				else
				{
					if( pWndTaget == &(g_WndMng.m_pWndBank->m_wndGold[0]) )
					{					
						nSlot = 0;
					}
					else if( pWndTaget == &(g_WndMng.m_pWndBank->m_wndGold[1]) )
					{
						nSlot = 1;
					}
					else
					{
						nSlot = 2;
					}

					// 페냐 (돈)
					g_WndMng.m_pWndTradeGold = new CWndTradeGold;
					memcpy( &g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT) );
					g_WndMng.m_pWndTradeGold->m_dwGold = g_pPlayer->m_dwGoldBank[nSlot];
					g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_GOLD;	// 인벤토리 인데 쓰고 있어서 Gold로 씀.
					g_WndMng.m_pWndTradeGold->m_pWndBase = this;
					g_WndMng.m_pWndTradeGold->m_nSlot = nSlot;
					
					g_WndMng.m_pWndTradeGold->Initialize( &g_WndMng, APP_TRADE_GOLD );
					g_WndMng.m_pWndTradeGold->MoveParentCenter();
					CWndStatic* pStatic	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC );
					CWndStatic* pStaticCount	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_CONTROL1 );
					CString strMain = prj.GetText( TID_GAME_MOVEPENYA );//"얼마를 이동하시겠습니까?";
					CString strCount = prj.GetText(TID_GAME_PENYACOUNT);//" SEED : ";
					pStatic->m_strTitle = strMain;
					pStaticCount->m_strTitle = strCount;
				}
				bForbid = FALSE;
			}
#if __VER >= 11 // __SYS_POCKET
			// 휴대가방에서 온 경우
			if( pWndFrame->GetWndId() == APP_BAG_EX )
			{
				
				CWndBase* pWndFrom = pWndFrame->m_pCurFocus;
				BYTE nSlot;

				SAFE_DELETE( g_WndMng.m_pWndTradeGold );
				if( lpShortcut->m_dwData != 0 )
				{
					if( pWndFrom->m_pParentWnd->GetWndId() == WIDC_BASIC )
					{					
						nSlot = 0;
					}
					else if( pWndFrom->m_pParentWnd->GetWndId() == WIDC_EXBAG1 )
					{
						nSlot = 1;
						if(!g_pPlayer->m_Pocket.IsAvailable(1)) return FALSE;
					}
					else
					{
						nSlot = 2;
						if(!g_pPlayer->m_Pocket.IsAvailable(2)) return FALSE;
					}
					
					CWndItemCtrl* pWndItemCtrl = (CWndItemCtrl*)lpShortcut->m_pFromWnd;
					
					for( int i = 0; i < (int)( pWndItemCtrl->GetSelectedCount() ); i++ )
					{
						int nItem = pWndItemCtrl->GetSelectedItem( i );
						pWndItemCtrl->GetItem( nItem );
					}

					CItemElem* itemElem = (CItemElem*)lpShortcut->m_dwData;
					if( itemElem->m_nItemNum > 1 )
					{ 
						g_WndMng.m_pWndTradeGold = new CWndTradeGold;
						memcpy( &g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT) );
						g_WndMng.m_pWndTradeGold->m_dwGold = itemElem->m_nItemNum;
						g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_BAG_EX; // 인벤토리 인데 쓰고 있어서 Gold로 씀.
						g_WndMng.m_pWndTradeGold->m_pWndBase = this;
						g_WndMng.m_pWndTradeGold->m_nSlot = -1;
						g_WndMng.m_pWndTradeGold->m_nPutSlot = nSlot;
						
						g_WndMng.m_pWndTradeGold->Initialize( &g_WndMng, APP_TRADE_GOLD );
						g_WndMng.m_pWndTradeGold->MoveParentCenter();
						CWndStatic* pStatic	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC );
						CWndStatic* pStaticCount	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_CONTROL1 );
						CString strMain = prj.GetText( TID_GAME_MOVECOUNT );//"몇개를 이동하시겠습니까?";
						CString strCount = prj.GetText(TID_GAME_NUMCOUNT);//" 갯수 : ";
						pStatic->m_strTitle = strMain;
						pStaticCount->m_strTitle = strCount;
					}
					else
					{
						g_DPlay.SendMoveItem_Pocket( nSlot, lpShortcut->m_dwId, 1 , -1);
					}
				}
				
				bForbid = FALSE;
			}
#endif
			else
			if( pWndFrame->GetWndId() == APP_POST_READ )
			{
				CWndPostRead* pWndPostRead = (CWndPostRead*)pWndFrame;
				
				if( pWndPostRead )
				{
					if( lpShortcut->m_dwData != 0 )
					{
						pWndPostRead->MailReceiveItem();
					}
					else
					{
						pWndPostRead->MailReceiveGold();
					}
					
					bForbid = FALSE;
				}
			}
			else	
			if( pWndFrame->GetWndId() == APP_GUILD_BANK )
			{
				SAFE_DELETE( g_WndMng.m_pWndTradeGold );
				if( lpShortcut->m_dwData != 0 )
				{
					CGuild* pGuild = g_pPlayer->GetGuild();
					if( pGuild && pGuild->IsGetItem( g_pPlayer->m_idPlayer ))
					{
						CWndItemCtrl* pWndItemCtrl = (CWndItemCtrl*)lpShortcut->m_pFromWnd;
						
						for( int i = 0; i < (int)( pWndItemCtrl->GetSelectedCount() ); i++ )
						{
							int nItem = pWndItemCtrl->GetSelectedItem( i );
							pWndItemCtrl->GetItem( nItem );
						}
						CItemElem* itemElem = (CItemElem*)lpShortcut->m_dwData;
						if( itemElem->m_nItemNum > 1 )
						{ 
							g_WndMng.m_pWndTradeGold = new CWndTradeGold;
							memcpy( &g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT) );
							g_WndMng.m_pWndTradeGold->m_dwGold = itemElem->m_nItemNum;
							g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_MESSENGER;	// ****중요****
							g_WndMng.m_pWndTradeGold->m_pWndBase = this;
							
							g_WndMng.m_pWndTradeGold->Initialize( &g_WndMng, APP_TRADE_GOLD );
							g_WndMng.m_pWndTradeGold->MoveParentCenter();
							CWndStatic* pStatic	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC );
							CWndStatic* pStaticCount	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_CONTROL1 );
							CString strMain = prj.GetText( TID_GAME_MOVECOUNT );//"몇개를 이동하시겠습니까?";
							CString strCount = prj.GetText(TID_GAME_NUMCOUNT);//" 갯수 : ";
							pStatic->m_strTitle = strMain;
							pStaticCount->m_strTitle = strCount;
						}
						else
						{
							g_DPlay.SendGetItemGuildBank( (BYTE)( lpShortcut->m_dwId ), 1, 1 );
						}
					}
				}
				else
				{
					CGuild* pGuild = g_pPlayer->GetGuild();
					if( pGuild && pGuild->IsGetPenya( g_pPlayer->m_idPlayer ))
					{
						// 페냐 (돈)
						g_WndMng.m_pWndTradeGold = new CWndTradeGold;
						memcpy( &g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT) );
						g_WndMng.m_pWndTradeGold->m_dwGold = pGuild->m_nGoldGuild;
						g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_MESSENGER;	// 인벤토리 인데 쓰고 있어서 Gold로 씀.
						g_WndMng.m_pWndTradeGold->m_pWndBase = this;
						
						g_WndMng.m_pWndTradeGold->Initialize( &g_WndMng, APP_TRADE_GOLD );
						g_WndMng.m_pWndTradeGold->MoveParentCenter();
						CWndStatic* pStatic	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC );
						CWndStatic* pStaticCount	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_CONTROL1 );
						CString strMain = prj.GetText( TID_GAME_MOVEPENYA );//"얼마를 이동하시겠습니까?";
						CString strCount = prj.GetText(TID_GAME_PENYACOUNT);//" SEED : ";
						pStatic->m_strTitle = strMain;
						pStaticCount->m_strTitle = strCount;
					}
				}
				bForbid = FALSE;
			}
		}
		SetForbid( bForbid );
	}
	else
	// 차일드로부터 더블클릭 알림 메시지가 왔다. 아이템 콘트롤에서 아이템을 사용하겠다는 메시지다.
	if( message == WIN_DBLCLK )
	{
		if( nID == 11 ) // item
		{
#if __VER >= 14 // __SMELT_SAFETY
			if( m_dwEnchantWaitTime != 0xffffffff || GetWndBase(APP_SMELT_SAFETY_CONFIRM) != NULL )
#else //__SMELT_SAFETY
			if( m_dwEnchantWaitTime != 0xffffffff )
#endif //__SMELT_SAFETY
			{
				g_WndMng.PutString( prj.GetText(TID_MMI_NOTUPGRADE), NULL, prj.GetTextColor(TID_MMI_NOTUPGRADE) );
				return 0;
			}

#if __VER >= 14 // __EQUIP_BIND
			if( GetWndBase(APP_EQUIP_BIND_CONFIRM) != NULL )
			{
				g_WndMng.PutString( prj.GetText(TID_TOOLTIP_EQUIPBIND_ERROR01), NULL, prj.GetTextColor(TID_TOOLTIP_EQUIPBIND_ERROR01) );
				return 0;
			}
#endif // __EQUIP_BIND

#if __VER >= 14 // __DROP_CONFIRM_BUG
			if( GetWndBase(APP_COMMITEM_DIALOG) != NULL )
			{
				g_WndMng.PutString( prj.GetText(TID_TOOLTIP_ITEM_USING_ERROR), NULL, prj.GetTextColor(TID_TOOLTIP_ITEM_USING_ERROR) );
				return 0;
			}
#endif // __DROP_CONFIRM_BUG
			
			CCtrl* pCtrl = (CCtrl*)g_WorldMng()->GetObjFocus();
			DWORD dwObjId = NULL_ID;
			if( pCtrl && pCtrl->GetType() != OT_OBJ )
				dwObjId = pCtrl->GetId();
			CItemBase* pFocusItem = (CItemBase*) pLResult;

			BOOL	bAble = TRUE;
			if( pFocusItem )
			{
				ItemProp *pProp = pFocusItem->GetProp();
				// 소비아이템이 1개 남았을때 더블클릭하고 서버로 갔다가 
				// 삭제되기 전에 또 더블클릭하면 pFocusItem->m_dwItemId가 0이 되면서
				// 프로퍼티가 널되서 죽는다.
				if( pProp )
				{					
#if __VER >= 8 // __CSC_VER8_5
					if(g_WndMng.GetWndBase( APP_SUMMON_ANGEL ))
					{
						if(pProp->dwID == II_GEN_MAT_ORICHALCUM01 || pProp->dwID == II_GEN_MAT_MOONSTONE ||
							pProp->dwID == II_GEN_MAT_ORICHALCUM01_1 || pProp->dwID == II_GEN_MAT_MOONSTONE_1)
						{
							if(pFocusItem->GetExtra() < ((CItemElem*)pFocusItem)->m_nItemNum)
							{
								CWndSummonAngel* pWndSummonAngel = (CWndSummonAngel*)g_WndMng.GetWndBase( APP_SUMMON_ANGEL );
								pWndSummonAngel->SetDie((CItemElem*)pFocusItem);
								return TRUE;
							}
						}
					}
#endif // __CSC_VER8_5
#ifdef __EVE_MINIGAME
					if(g_WndMng.GetWndBase( APP_MINIGAME_WORD ))
					{
						if(pProp->dwID >= II_SYS_SYS_EVE_A_CARD && pProp->dwID <= II_SYS_SYS_EVE_Z_CARD)
						{
							if(pFocusItem->GetExtra() < ((CItemElem*)pFocusItem)->m_nItemNum)
							{								
								CWndFindWordGame* pWndFindWordGame = (CWndFindWordGame*)g_WndMng.GetWndBase( APP_MINIGAME_WORD );
								pWndFindWordGame->SetWord((CItemElem*)pFocusItem);
								return TRUE;
							}
						}
					}
					if(g_WndMng.GetWndBase( APP_MINIGAME_PUZZLE ))
					{
						if(pFocusItem->GetExtra() < ((CItemElem*)pFocusItem)->m_nItemNum)
						{			
							CWndPuzzleGame* pWndPuzzleGame = (CWndPuzzleGame*)g_WndMng.GetWndBase( APP_MINIGAME_PUZZLE );
							pWndPuzzleGame->SetPicture((CItemElem*)pFocusItem);
							return TRUE;
						}
					}
#endif //__EVE_MINIGAME
#if __VER >= 10 // __REMOVE_ATTRIBUTE
					if(g_WndMng.GetWndBase( APP_REMOVE_ATTRIBUTE ))
					{
						if( CItemElem::IsEleRefineryAble(pProp) )
						{
							if(pFocusItem->GetExtra() < ((CItemElem*)pFocusItem)->m_nItemNum)
							{			
								CWndRemoveAttribute* pWndRemoveAttribute = (CWndRemoveAttribute*)GetWndBase( APP_REMOVE_ATTRIBUTE );
								pWndRemoveAttribute->SetWeapon((CItemElem*)pFocusItem);
								return TRUE;
							}
						}
					}
#endif //__REMOVE_ATTRIBUTE
#if __VER >= 11 // __PIERCING_REMOVE
#if __VER >= 12 // __CSC_VER12_4
					if(g_WndMng.GetWndBase( APP_SMELT_REMOVE_PIERCING_EX ))
#else //__CSC_VER12_4
					if(g_WndMng.GetWndBase( APP_SMELT_REMOVE_PIERCING ))
#endif //__CSC_VER12_4
					{
						if( CItemElem::IsEleRefineryAble(pProp) )
						{
//							if(pFocusItem->GetExtra() < ((CItemElem*)pFocusItem)->m_nItemNum)
							if( IsUsableItem( pFocusItem ) )
							{			
#if __VER >= 12 // __CSC_VER12_4
								CWndRemovePiercing* pWndRemovePiercing = (CWndRemovePiercing*)GetWndBase( APP_SMELT_REMOVE_PIERCING_EX );
								pWndRemovePiercing->SetItem((CItemElem*)pFocusItem);
#else //__CSC_VER12_4
								CWndRemovePiercing* pWndRemovePiercing = (CWndRemovePiercing*)GetWndBase( APP_SMELT_REMOVE_PIERCING );
								pWndRemovePiercing->SetSuit((CItemElem*)pFocusItem);
#endif //__CSC_VER12_4
								return TRUE;
							}
						}
					}
#endif //__PIERCING_REMOVE
#if __VER >= 12 // __CSC_VER12_4
					if(g_WndMng.GetWndBase( APP_SMELT_REMOVE_JEWEL ))
					{
						if( pProp->dwReferStat1 == WEAPON_ULTIMATE || pProp->dwItemKind2 == IK2_MATERIAL )
						{
							if( IsUsableItem( pFocusItem ) )
							{			
								CWndRemoveJewel* pWndRemoveJewel = (CWndRemoveJewel*)GetWndBase( APP_SMELT_REMOVE_JEWEL );
								pWndRemoveJewel->SetItem((CItemElem*)pFocusItem);
								return TRUE;
							}
						}
					}
#endif //__CSC_VER12_4
#if __VER >= 12 // __CSC_VER12_5
					if(g_WndMng.GetWndBase( APP_PET_TRANS_EGGS ))
					{	
						if(g_pPlayer->IsUsing((CItemElem*)pFocusItem))
						{
							g_WndMng.PutString( prj.GetText( TID_GAME_TRANS_EGGS_ERROR2 ), NULL, prj.GetTextColor( TID_GAME_TRANS_EGGS_ERROR2 ) );
							return TRUE;
						}
							
						if( (pProp->dwItemKind3 == IK3_EGG && ((CItemElem*)pFocusItem)->m_pPet == NULL) ||
							(pProp->dwItemKind3 == IK3_EGG && ((CItemElem*)pFocusItem)->m_pPet && ((CItemElem*)pFocusItem)->m_pPet->GetLevel() == PL_EGG) )
						{
							if( IsUsableItem( pFocusItem ) )
							{			
								CWndPetTransEggs* pWndPetTransEggs = (CWndPetTransEggs*)GetWndBase( APP_PET_TRANS_EGGS );
								pWndPetTransEggs->SetItem((CItemElem*)pFocusItem);
								return TRUE;
							}
						}
						else
						{
							g_WndMng.PutString( prj.GetText( TID_GAME_TRANS_EGGS_ERROR1 ), NULL, prj.GetTextColor( TID_GAME_TRANS_EGGS_ERROR1 ) );
							return TRUE;
						}
					}
#endif //__CSC_VER12_5
#if __VER >= 14 // __SMELT_SAFETY
					if(g_WndMng.GetWndBase( APP_SMELT_SAFETY ))
					{
						if(pFocusItem->GetExtra() < ((CItemElem*)pFocusItem)->m_nItemNum)
						{			
							CWndSmeltSafety* pWndSmeltSafety = (CWndSmeltSafety*)GetWndBase( APP_SMELT_SAFETY );
							pWndSmeltSafety->SetItem((CItemElem*)pFocusItem);
							return TRUE;
						}
					}
#endif //__SMELT_SAFETY
#if __VER >= 9 // __CSC_VER9_1
					if(g_WndMng.GetWndBase( APP_SMELT_MIXJEWEL ))
					{
						if(pProp->dwID == II_GEN_MAT_ORICHALCUM01 || pProp->dwID == II_GEN_MAT_MOONSTONE ||
							pProp->dwID == II_GEN_MAT_ORICHALCUM01_1 || pProp->dwID == II_GEN_MAT_MOONSTONE_1)
						{
							if(pFocusItem->GetExtra() < ((CItemElem*)pFocusItem)->m_nItemNum)
							{			
								CWndMixJewel* pWndMixJewel = (CWndMixJewel*)GetWndBase( APP_SMELT_MIXJEWEL );
								pWndMixJewel->SetJewel((CItemElem*)pFocusItem);
								return TRUE;
							}
						}
					}

					if(g_WndMng.GetWndBase( APP_SMELT_JEWEL ))
					{
						if( pProp->dwID == II_GEN_MAT_DIAMOND ||
							pProp->dwID == II_GEN_MAT_EMERALD ||
							pProp->dwID == II_GEN_MAT_SAPPHIRE ||
							pProp->dwID == II_GEN_MAT_RUBY ||
							pProp->dwID == II_GEN_MAT_TOPAZ )
						{
							if(pFocusItem->GetExtra() < ((CItemElem*)pFocusItem)->m_nItemNum)
							{			
								CWndSmeltJewel* pWndSmeltJewel = (CWndSmeltJewel*)GetWndBase( APP_SMELT_JEWEL );
								pWndSmeltJewel->SetJewel((CItemElem*)pFocusItem);
								return TRUE;
							}
						}
					}

					if(g_WndMng.GetWndBase( APP_SMELT_CHANGEWEAPON ))
					{
						if( pProp->dwItemKind1 == IK1_WEAPON ||
							pProp->dwID == II_GEN_MAT_ORICHALCUM02 ||
							pProp->dwID == II_GEN_MAT_DIAMOND ||
							pProp->dwID == II_GEN_MAT_EMERALD ||
							pProp->dwID == II_GEN_MAT_SAPPHIRE ||
							pProp->dwID == II_GEN_MAT_RUBY ||
							pProp->dwID == II_GEN_MAT_TOPAZ )
						{
							if(pFocusItem->GetExtra() < ((CItemElem*)pFocusItem)->m_nItemNum)
							{			
								CWndChangeWeapon* pWndChangeWeapon = (CWndChangeWeapon*)GetWndBase( APP_SMELT_CHANGEWEAPON );
								pWndChangeWeapon->SetItem((CItemElem*)pFocusItem);
								return TRUE;
							}
						}
					}
#if __VER >= 10 // __CSC_VER9_1 -> __LEGEND
					if(g_WndMng.GetWndBase( APP_HERO_SKILLUP ))
					{
						if( pProp->dwID == II_GEN_MAT_DIAMOND ||
							pProp->dwID == II_GEN_MAT_EMERALD ||
							pProp->dwID == II_GEN_MAT_SAPPHIRE ||
							pProp->dwID == II_GEN_MAT_RUBY ||
							pProp->dwID == II_GEN_MAT_TOPAZ )
						{
							if(pFocusItem->GetExtra() < ((CItemElem*)pFocusItem)->m_nItemNum)
							{			
								CWndHeroSkillUp* pWndHeroSkillUp = (CWndHeroSkillUp*)GetWndBase( APP_HERO_SKILLUP );
								pWndHeroSkillUp->SetJewel((CItemElem*)pFocusItem);
								return TRUE;
							}
						}
					}
#endif // __CSC_VER9_1 -> __LEGEND
					if(g_WndMng.GetWndBase( APP_SMELT_EXTRACTION ))
					{
						if( pProp->dwItemKind1 == IK1_WEAPON )
						{
							if(pFocusItem->GetExtra() < ((CItemElem*)pFocusItem)->m_nItemNum)
							{			
								CWndExtraction* pWndExtraction = (CWndExtraction*)GetWndBase( APP_SMELT_EXTRACTION );
								pWndExtraction->SetWeapon((CItemElem*)pFocusItem);
								return TRUE;
							}
						}
					}
					// 속성카드나 주나위를 더블클릭했을경우 인첸트모드로 변경한다.
					if( (pFocusItem->GetExtra() < ((CItemElem*)pFocusItem)->m_nItemNum) 
						&& ( 
							pProp->dwItemKind3 == IK3_ELECARD
							|| pProp->dwItemKind3 == IK3_ENCHANT 
							|| pProp->dwItemKind3 == IK3_SOCKETCARD
							|| pProp->dwItemKind3 == IK3_RANDOM_SCROLL
							|| pProp->dwItemKind3 == IK3_PIERDICE 
#if __VER >= 11
							|| IsNeedTarget( pProp )
#endif	// __VER 
#if __VER >= 12 // __EXT_PIERCING
							|| pProp->dwItemKind3 == IK3_SOCKETCARD2
#endif // __EXT_PIERCING
							) 
						)
#else //__CSC_VER9_1
					// 속성카드나 주나위를 더블클릭했을경우 인첸트모드로 변경한다.
				#if __VER >= 8 //__Y_NEW_ENCHANT
					if(  pProp->dwItemKind3 == IK3_ELECARD || pProp->dwItemKind3 == IK3_ENCHANT ||
						pProp->dwItemKind3 == IK3_SOCKETCARD
						|| pProp->dwItemKind3 == IK3_RANDOM_SCROLL
					#if __VER >= 9 // __ULTIMATE
						|| pProp->dwItemKind3 == IK3_PIERDICE
					#endif // __ULTIMATE
						)
				#else //__Y_NEW_ENCHANT
					if(  pProp->dwItemKind3 == IK3_ELECARD || pProp->dwItemKind3 == IK3_DICE ||
					     pProp->dwItemKind3 == IK3_SOCKETCARD || pProp->dwItemKind3 == IK3_RANDOM_SCROLL )
				#endif //__Y_NEW_ENCHANT
#endif //__CSC_VER9_1
					{
#ifdef __QUIZ
						if( g_pPlayer && g_pPlayer->GetWorld() && g_pPlayer->GetWorld()->GetID() == WI_WORLD_QUIZ )
						{
							g_WndMng.PutString( prj.GetText( TID_SBEVE_NOTUSEITEM ), NULL, prj.GetTextColor( TID_SBEVE_NOTUSEITEM ) );
							return FALSE;
						}
#endif // __QUIZ
						bAble = FALSE;
						m_bIsUpgradeMode = TRUE;
						m_pUpgradeMaterialItem = pFocusItem;	

						if((g_pPlayer->IsMode( TRANSPARENT_MODE ) ) == 0)
							m_pSfxUpgrade = CreateSfx( g_Neuz.m_pd3dDevice, XI_INT_INCHANT, g_pPlayer->GetPos(), g_pPlayer->GetId(), g_pPlayer->GetPos(), g_pPlayer->GetId(), -1 );
					}
					
					if( pProp->dwItemKind1 == IK1_RIDE )	// 사용한 아이템이 탈것인가.
					{
						if( g_pPlayer->m_pActMover->IsFly() == FALSE )		// Ground상태인가?
							if( g_pPlayer->m_pActMover->m_bGround == 0 )		// 땅에 발을 안붙이고 있다면 못탄다.
								bAble = FALSE;	// 아이템사용 실패.
					}

					if( pFocusItem->m_dwItemId == II_SYS_SYS_MAP_FLARIS )
					{
						g_WndMng.OpenMap( "map_flaris.tga" );	// 파일이 없어서 잠시 막아둠
					}
					else
					if( pFocusItem->m_dwItemId == II_SYS_SYS_MAP_SAINTMORNING )
					{
						g_WndMng.OpenMap( "map_saintmorning.tga" );
					}
					
#if __VER >= 13 // __EXT_ENCHANT
					if( pFocusItem->m_dwItemId == II_SYS_SYS_SCR_SOKCHANG )
					{
						if(g_WndMng.m_pWndChangeAttribute)
							SAFE_DELETE(g_WndMng.m_pWndChangeAttribute);

						g_WndMng.m_pWndChangeAttribute = new CWndChangeAttribute;
						g_WndMng.m_pWndChangeAttribute->SetChangeItem((CItemElem*)pFocusItem);
						g_WndMng.m_pWndChangeAttribute->Initialize(&g_WndMng, APP_COMMITEM_DIALOG);
						bAble = FALSE;
					}
#endif //__EXT_ENCHANT
#ifdef __SCROLL_OF_UNBIND
					if( pFocusItem->m_dwItemId == II_SYS_SYS_SCR_CANCELLATION )
					{
						bAble = FALSE;
					}
#endif // __SCROLL_OF_UNBIND
#ifdef __TELEPORT_SCROLL
					if( pProp->dwItemKind2 == IK2_TELEPORTMAP )
					{
						CWndMapEx* pWndMapEx = ( CWndMapEx* )g_WndMng.CreateApplet( APP_MAP_EX );
						if( pWndMapEx == NULL )
						{
							return FALSE;
						}
						pWndMapEx->SetConstructionMode( CWndMapEx::MAPTELEPORT );
						pWndMapEx->InitializeMapTeleportInformation( pProp->dwItemKind3 );
					}
#endif // __TELEPORT_SCROLL
					if(pProp && pProp->dwID == II_SYS_SYS_SCR_RECCURENCE || pProp->dwID == II_SYS_SYS_SCR_RECCURENCE_LINK)
					{	// 리스킬 아이템은 모두 확인 창 띄운다.
						if(g_pPlayer->m_nSkillPoint < g_pPlayer->GetCurrentMaxSkillPoint())
						{
							SAFE_DELETE( g_WndMng.m_pWndCommItemDlg );
							g_WndMng.m_pWndCommItemDlg = new CWndCommItemDlg;
							g_WndMng.m_pWndCommItemDlg->Initialize( &g_WndMng, APP_COMMITEM_DIALOG );
							g_WndMng.m_pWndCommItemDlg->SetItem( TID_GAME_SKILLINIT, pFocusItem->m_dwObjId, dwObjId );
						}
						else
							g_WndMng.PutString( prj.GetText(TID_GAME_ERROR_SKILLRECCURENCE), NULL, prj.GetTextColor(TID_GAME_ERROR_SKILLRECCURENCE) );

						bAble = FALSE;
					}

#if __VER >= 14 // __RESTATE_CONFIRM
					if( pFocusItem->m_dwItemId == II_CHR_SYS_SCR_RESTATE || 
						pFocusItem->m_dwItemId == II_CHR_SYS_SCR_RESTATE_STR || 
						pFocusItem->m_dwItemId == II_CHR_SYS_SCR_RESTATE_STA || 
						pFocusItem->m_dwItemId == II_CHR_SYS_SCR_RESTATE_DEX || 
						pFocusItem->m_dwItemId == II_CHR_SYS_SCR_RESTATE_INT
#ifdef __ADD_RESTATE_LOW
						|| pFocusItem->m_dwItemId == II_CHR_SYS_SCR_RESTATE_STR_LOW
						|| pFocusItem->m_dwItemId == II_CHR_SYS_SCR_RESTATE_STA_LOW
						|| pFocusItem->m_dwItemId == II_CHR_SYS_SCR_RESTATE_DEX_LOW
						|| pFocusItem->m_dwItemId == II_CHR_SYS_SCR_RESTATE_INT_LOW
#endif // __ADD_RESTATE_LOW
						)
					{
						SAFE_DELETE( g_WndMng.m_pWndRestateConfirm );
						g_WndMng.m_pWndRestateConfirm = new CWndRestateConfirm(pFocusItem->m_dwItemId);
						g_WndMng.m_pWndRestateConfirm->SetInformation(MAKELONG( ITYPE_ITEM, pFocusItem->m_dwObjId ), dwObjId, pFocusItem->GetProp()->dwParts);
						g_WndMng.m_pWndRestateConfirm->Initialize(NULL);
						bAble = FALSE;
					}
#endif // __RESTATE_CONFIRM

					if( ((CItemElem*)pFocusItem)->m_bCharged != 1 )
					{
						switch( pProp->dwID )
						{
						case II_SYS_SYS_SCR_BLESSING:
							{
								if( g_pPlayer->IsSMMode( SM_REVIVAL ) )
								{
									g_WndMng.PutString( prj.GetText( TID_GAME_LIMITED_USE ), NULL, prj.GetTextColor(TID_GAME_LIMITED_USE) );
								}
								else
								{
									SAFE_DELETE( g_WndMng.m_pWndCommItemDlg );
									g_WndMng.m_pWndCommItemDlg = new CWndCommItemDlg;
									g_WndMng.m_pWndCommItemDlg->Initialize( &g_WndMng, APP_COMMITEM_DIALOG );
									g_WndMng.m_pWndCommItemDlg->SetItem( TID_GAME_DEPEN_USE, pFocusItem->m_dwObjId, dwObjId );
								}
								bAble = FALSE;
							}
							break;
//						case II_SYS_SYS_SCR_RECCURENCE:
//						case II_SYS_SYS_SCR_RECCURENCE_LINK:
//							{
//								SAFE_DELETE( g_WndMng.m_pWndCommItemDlg );
//								g_WndMng.m_pWndCommItemDlg = new CWndCommItemDlg;
//								g_WndMng.m_pWndCommItemDlg->Initialize( &g_WndMng, APP_COMMITEM_DIALOG );
//								g_WndMng.m_pWndCommItemDlg->SetItem( TID_GAME_SKILLINIT, pFocusItem->m_dwObjId, dwObjId );
//								bAble = FALSE;
//							}
//							break;
						case II_SYS_SYS_SCR_CHACLA:
							{
								if( g_pPlayer->IsBaseJob() )
								{
									g_WndMng.PutString( prj.GetText( TID_GAME_NOTUSEVAG ), NULL, prj.GetTextColor( TID_GAME_NOTUSEVAG ) );
									bAble = FALSE;
								}
								else
								{
									for( DWORD dwParts = 0; dwParts < MAX_HUMAN_PARTS; dwParts++ )
									{
										if( dwParts == PARTS_HEAD || dwParts == PARTS_HAIR || dwParts == PARTS_RIDE )
											continue;
										CItemElem* pArmor	= g_pPlayer->m_Inventory.GetEquip( dwParts );
										if( pArmor )
										{
											g_WndMng.PutString( prj.GetText( TID_GAME_CHECK_EQUIP ), NULL, prj.GetTextColor( TID_GAME_CHECK_EQUIP ) );
											bAble = FALSE;
										}
									}
								}

								if( bAble )
								{
									SAFE_DELETE( g_WndMng.m_pWndCommItemDlg );
									g_WndMng.m_pWndCommItemDlg = new CWndCommItemDlg;
									g_WndMng.m_pWndCommItemDlg->Initialize( &g_WndMng, APP_COMMITEM_DIALOG );
									g_WndMng.m_pWndCommItemDlg->SetItem( TID_GAME_WARNINGCCLS, pProp->dwID, pProp->dwID );
									bAble = FALSE;
								}
							}
							break;
						}
					}
				}
#ifdef _DEBUG
				else
				{
					int a = 0;
				}
#endif
			}
			if( bAble )	// 성공판정 났을때만 보냄.
			{
				if( g_WndMng.GetWndBase( APP_SHOP_ )|| g_WndMng.GetWndBase( APP_BANK )|| g_WndMng.GetWndBase( APP_TRADE ))
				{
					//g_WndMng.PutString( "거래중에 아이템 사용은 불가능해요.", NULL, 0xffff0000 );
					g_WndMng.PutString( prj.GetText(TID_GAME_TRADELIMITUSING), NULL, prj.GetTextColor(TID_GAME_TRADELIMITUSING) );
				}
#if __VER >= 8 // __CSC_VER8_5
				else if(g_WndMng.GetWndBase( APP_SUMMON_ANGEL ))
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_TRADELIMITUSING), NULL, prj.GetTextColor(TID_GAME_TRADELIMITUSING) );
				}
#endif // __CSC_VER8_5
#ifdef __EVE_MINIGAME
				else if(g_WndMng.GetWndBase( APP_MINIGAME_DICE ))
				{
					g_WndMng.PutString( prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM) );
				}
				else if(g_WndMng.GetWndBase( APP_MINIGAME_KAWIBAWIBO ))
				{
					g_WndMng.PutString( prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM) );
				}
				else if(g_WndMng.GetWndBase( APP_MINIGAME_KAWIBAWIBO_WIN ))
				{
					g_WndMng.PutString( prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM) );
				}
				else if(g_WndMng.GetWndBase( APP_MINIGAME_PUZZLE ))
				{
					g_WndMng.PutString( prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM) );
				}
				else if(g_WndMng.GetWndBase( APP_MINIGAME_WORD ))
				{
					g_WndMng.PutString( prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM) );
				}
#endif //__EVE_MINIGAME
#if __VER >= 9 // __CSC_VER9_1
				else if(g_WndMng.GetWndBase( APP_SMELT_EXTRACTION ))
				{
					g_WndMng.PutString( prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM) );
				}
				else if(g_WndMng.GetWndBase( APP_SMELT_JEWEL ))
				{
					g_WndMng.PutString( prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM) );
				}
				else if(g_WndMng.GetWndBase( APP_SMELT_MIXJEWEL ))
				{
					g_WndMng.PutString( prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM) );
				}
				else if(g_WndMng.GetWndBase( APP_PET_FOODMILL ))
				{
					g_WndMng.PutString( prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM) );
				}
#if __VER >= 14 // __SMELT_SAFETY
				else if(g_WndMng.GetWndBase( APP_SMELT_SAFETY ))
				{
					g_WndMng.PutString( prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM) );
				}
#endif
#ifdef __QUIZ
				else if( g_pPlayer && g_pPlayer->GetWorld() && g_pPlayer->GetWorld()->GetID() == WI_WORLD_QUIZ )
				{
					g_WndMng.PutString( prj.GetText( TID_SBEVE_NOTUSEITEM ), NULL, prj.GetTextColor( TID_SBEVE_NOTUSEITEM ) );
				}
#endif // __QUIZ
#endif //__CSC_VER9_1
				else if( g_WndMng.GetWndBase( APP_REPAIR ) )
				{
					//g_WndMng.PutString( "수리중에 아이템 사용은 불가능해요.", NULL, 0xffff0000 );
					g_WndMng.PutString( prj.GetText(TID_GAME_REPAIR_NOTUSE), NULL, prj.GetTextColor(TID_GAME_TRADELIMITUSING) );
				}
				else
				{
#if __VER >= 14 // __EQUIP_BIND
					CItemElem* pItemElem = (CItemElem*)pFocusItem;
					if( pItemElem->GetProp() && ( pItemElem->GetProp()->dwFlag & IP_FLAG_EQUIP_BIND ) && !pItemElem->IsFlag( CItemElem::binds ) )
					{
						SAFE_DELETE(g_WndMng.m_pWndEquipBindConfirm)
						g_WndMng.m_pWndEquipBindConfirm = new CWndEquipBindConfirm(CWndEquipBindConfirm::EQUIP_DOUBLE_CLICK);
						g_WndMng.m_pWndEquipBindConfirm->SetInformation(pFocusItem, dwObjId);
						g_WndMng.m_pWndEquipBindConfirm->Initialize(NULL);
					}
					else
					{
#endif // __EQUIP_BIND
						// 탈착 명령일 경우, nPart는 실제 장착되어 있는 부분과 일치해야 하므로 프로퍼티에서 꺼내지 않는다.
						ItemProp* pItemProp = pFocusItem->GetProp();
						
						if( pItemProp )
						{
							int nPart	= pItemProp->dwParts;
							
							BOOL bEquiped	= g_pPlayer->m_Inventory.IsEquip( pFocusItem->m_dwObjId );
							if( bEquiped )
								nPart	= pFocusItem->m_dwObjIndex - g_pPlayer->m_Inventory.m_dwIndexNum;
#if __VER >= 15 // __PETVIS
							if( !g_WndMng.CheckConfirm( pFocusItem ) )		//gmpbigsun: 패킷보내기전 확인등의 처리담당 
							{
								g_DPlay.SendDoUseItem( MAKELONG( ITYPE_ITEM, pFocusItem->m_dwObjId ), dwObjId, nPart );
							}
#else
							g_DPlay.SendDoUseItem( MAKELONG( ITYPE_ITEM, pFocusItem->m_dwObjId ), dwObjId, nPart );
#endif
						}
#if __VER >= 14 // __EQUIP_BIND
					}
#endif // __EQUIP_BIND
				}
			}
		}
	}
	if( message == WNM_CLICKED )
	{
		switch( nID )
		{
			case 100: // icon
			//	m_wndItemCtrl.m_dwListCtrlStyle = WLVS_ICON;
				m_wndItemCtrl.SetWndRect( m_wndItemCtrl.GetWindowRect( TRUE ) );
				break;
			//case 101: // report
			//	m_wndItemCtrl.m_dwListCtrlStyle = WLVS_REPORT;
			//	m_wndItemCtrl.SetWndRect( m_wndItemCtrl.GetWindowRect( TRUE ) );
			//	break;
			case WTBID_REPORT:
				/*
				if( m_bReport )
				{
					m_wndItemCtrl.m_dwListCtrlStyle = WLVS_ICON;
				}
				else
				{
					m_wndItemCtrl.m_dwListCtrlStyle = WLVS_REPORT;
				}
				m_bReport = !m_bReport;
				m_wndItemCtrl.SetWndRect( m_wndItemCtrl.GetWindowRect( TRUE ) );
*/
				break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
BOOL CWndInventory::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	/*
	switch(nID)
	{
		case 0: // wnd1 
			g_WndMng.OpenWnd1();
			break;
		case 1: // wnd2
			g_WndMng.OpenCharacter();
			break;
		case 2: // 챗 
			g_WndMng.OpenChat();
			break;
		case 3:
			g_clientMsg.PutMessage(&g_Neuz.m_2DRender,0xffffffff,"fuck");
			break;
		case 9:
			g_WndMng.OpenCustomBox("종료하시겠습니까?",new CWndExitBox);
			break;
	}
	*/
	return CWndNeuz::OnCommand(nID,dwMessage,pWndBase);
}
void CWndInventory::OnSize(UINT nType, int cx, int cy)
{
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 50;
	CSize size = CSize(70,25);//m_pSprPack->GetAt(9)->GetSize();

	CRect rect1_1( x - ( size.cx / 2), y, ( x - ( size.cx / 2 ) ) + size.cx, y + size.cy );

	CRect rect2_1( x - size.cx - 10, y, ( x - size.cx - 10 ) + size.cx, y + size.cy );
	CRect rect2_2( x + 10          , y, ( x + 10           ) + size.cx, y + size.cy );

	CRect rect3_1( x - ( size.cx / 2) - size.cx - 10, y, (x - ( size.cx / 2) - size.cx - 10) + size.cx, y + size.cy );
	CRect rect3_2( x - ( size.cx / 2)               , y, (x - ( size.cx / 2)               ) + size.cx, y + size.cy );
	CRect rect3_3( x + ( size.cx / 2) + 10          , y, (x + ( size.cx / 2) + 10          ) + size.cx, y + size.cy );

	//m_wndButton1.SetWndRect( rect3_1 );
	//m_wndButton2.SetWndRect( rect3_2 );
	//m_wndButton3.SetWndRect( rect3_3 );

	CRect rect = GetClientRect();
	rect.DeflateRect( 5, 5 );
	rect.top    += 187;//203;
	rect.bottom -= 20;
	//rect.bottom -= 2;//40;
	//m_wndListCtrl.SetWndRect( rect );
//	CWndTabCtrl* pTabCtrl = (CWndTabCtrl*) GetDlgItem( WIDC_INVENTORY );

	//rect.bottom += 20;
//	pTabCtrl->SetWndRect( rect );

	CWndNeuz::OnSize(nType,cx,cy);
}
void CWndInventory::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bLButtonDownRot = FALSE;
	ReleaseCapture();
	
}
void CWndInventory::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM20 );
	if( lpWndCtrl->rect.PtInRect( point ) )
	{
		m_OldPos          = point;
		m_bLButtonDownRot = TRUE;
	}
		
	if( m_bLButtonDown )
	{
		for( int i=2; i<MAX_HUMAN_PARTS; i++ )
		{
			CRect DrawRect = m_InvenRect[i];
			
			if( DrawRect.PtInRect( point ) )
			{
				CItemBase* pItemBase = NULL;

				pItemBase = g_pPlayer->GetEquipItem( i );

				if( pItemBase )
				{
					m_pSelectItem  = (CItemElem*)pItemBase;
					return;
				}
			} 
		}
	}
	m_pSelectItem = NULL;
}

void CWndInventory::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	for( int i=2; i<MAX_HUMAN_PARTS; i++ )
	{
		CRect DrawRect = m_InvenRect[i];

		if( DrawRect.PtInRect( point ) )
		{
			CItemElem* pItemElem = NULL;

			pItemElem = (CItemElem*)g_pPlayer->GetEquipItem( i );
			
			if( pItemElem )
			{
				g_DPlay.SendDoEquip( pItemElem, i );		// 파츠번호를 같이 보낸다. 쌍칼땜시.
				// 장착인벤 영역을 더블클릭해서 벗기거나
				// 드래그&드롭으로 장착하려 하거나 벗기려고 할때 파츠번호를 보낸다.
			}
		}
	}
}

void CWndInventory::OnRButtonDown(UINT nFlags, CPoint point)
{
	BaseMouseCursor();
}

BOOL CWndInventory::OnSetCursor ( CWndBase* pWndBase, UINT nHitTest, UINT message )
{
	SetEnchantCursor();

	return TRUE;
}

void CWndInventory::SetEnchantCursor()
{
	// 인벤창이 열려있고 인첸트 모드이면 커서모양 변경
	if( m_bIsUpgradeMode )
	{
		SetMouseCursor( CUR_HAMMER );
	}
	else
	{
		SetMouseCursor( CUR_BASE );		
	}
}


void CWndInventory::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bLButtonDownRot )
	{
		m_fRot += (m_OldPos.x - point.x) * 0.5f;
	}
	
	m_OldPos = point;
	
	if( m_bLButtonDown && IsPush() )
	{
		if( m_bPickup == TRUE )
		{
			m_pSelectItem = NULL;
			return;
		}

		if( m_pSelectItem && m_pSelectItem->GetProp() )
		{
			m_GlobalShortcut.m_pFromWnd   = this;
			m_GlobalShortcut.m_dwShortcut = SHORTCUT_ITEM;
			m_GlobalShortcut.m_dwIndex    = 0xffffffff;
			m_GlobalShortcut.m_dwType     = ITYPE_ITEM;
			m_GlobalShortcut.m_dwId       = m_pSelectItem->m_dwObjId;//(DWORD)pItemElem;
			m_GlobalShortcut.m_pTexture   = m_pSelectItem->GetTexture();
			m_GlobalShortcut.m_dwData     = (DWORD) m_pSelectItem;
			_tcscpy( m_GlobalShortcut.m_szString, m_pSelectItem->GetProp()->szName);
		}
	}
}
BOOL CWndInventory::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CWndBase* pWndFrame = pShortcut->m_pFromWnd->GetFrameWnd();

	if(pWndFrame == NULL)
		return FALSE;
	
	if( g_WndMng.GetWndBase( APP_SHOP_ ) ||
		g_WndMng.GetWndBase( APP_BANK ) ||
		g_WndMng.GetWndBase( APP_TRADE ) )
	{
		if( pWndFrame->GetWndId() == APP_INVENTORY )
		{
			SetForbid( TRUE );
			//g_WndMng.PutString( "거래중에 아이템 사용은 불가능해요.", NULL, 0xffff0000 );
			g_WndMng.PutString( prj.GetText(TID_GAME_TRADELIMITUSING), NULL, prj.GetTextColor(TID_GAME_TRADELIMITUSING) );
			
			return FALSE;
		}
	}
	else if( g_WndMng.GetWndBase( APP_REPAIR ) )
	{
		if( pWndFrame->GetWndId() == APP_INVENTORY )
		{
			SetForbid( TRUE );
			//g_WndMng.PutString( "수리중에 아이템 사용은 불가능해요.", NULL, 0xffff0000 );
			g_WndMng.PutString( prj.GetText(TID_GAME_REPAIR_NOTUSE), NULL, prj.GetTextColor(TID_GAME_TRADELIMITUSING) );
			
			return FALSE;
		}
	}
	else if( g_WndMng.GetWndBase( APP_INVENTORY ) )
	{
		if( pWndFrame->GetWndId() == APP_INVENTORY )
		{
			LPWNDCTRL lpWndCtrl1 = GetWndCtrl( WIDC_CUSTOM21 );
			if( lpWndCtrl1->rect.PtInRect( point ) )
			{
				if( pShortcut->m_dwType == ITYPE_ITEM && pShortcut->m_dwData ) // dwData가 0이면 골드
				{
					CItemElem* pItemElem = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
					if( !pItemElem )
						return FALSE;
					if(pItemElem->GetExtra() > 0)
						return FALSE;
					if( g_pPlayer->m_Inventory.IsEquip( pShortcut->m_dwId ) ) 
						return FALSE;
					if( g_pPlayer->IsUsing( pItemElem ) )
						return FALSE;
					if( pItemElem->IsUndestructable() == TRUE )
					{
						g_WndMng.PutString( prj.GetText( TID_GAME_ERROR_UNDESTRUCTABLE_ITEM ), NULL, prj.GetTextColor( TID_GAME_ERROR_UNDESTRUCTABLE_ITEM ) );
						return FALSE;
					}

#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
					if( pItemElem && pItemElem->m_dwItemId == II_SYS_SYS_SCR_SEALCHARACTER  )
						return FALSE;
#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz

					SAFE_DELETE( g_WndMng.m_pWndInvenRemoveItem );
					g_WndMng.m_pWndInvenRemoveItem = new CWndInvenRemoveItem;
					g_WndMng.m_pWndInvenRemoveItem->Initialize( &g_WndMng, APP_INVEN_REMOVE_ITEM );
					g_WndMng.m_pWndInvenRemoveItem->InitItem( pItemElem );
					return TRUE;
				}				
			}
		}
	}
	
	if( pShortcut->m_dwShortcut == SHORTCUT_ITEM && pShortcut->m_pFromWnd != this && pWndFrame->GetWndId() == APP_INVENTORY )
	{
		if( pShortcut->m_dwType == ITYPE_ITEM && pShortcut->m_dwData ) // dwData가 0이면 골드 
		{
			CItemElem* pItemElem = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
			if( pItemElem && pItemElem->GetProp()->dwParts != NULL_ID ) //&& pItemElem->GetProp()->dwParts == i * 3 + j )
			{
				//인벤토리 테두리에 올려지는 아이템은 장착이 되지 않도록 수정.
				CRect rect;
				LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM5 );
				rect = lpWndCtrl->rect;
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM10 );
				rect.bottom = lpWndCtrl->rect.bottom;
				rect.right = lpWndCtrl->rect.right;

				if(rect.PtInRect( point ))
				{
#if __VER >= 14 // __EQUIP_BIND
					if( pItemElem->GetProp() && ( pItemElem->GetProp()->dwFlag & IP_FLAG_EQUIP_BIND ) && !pItemElem->IsFlag( CItemElem::binds ) )
					{
						SAFE_DELETE(g_WndMng.m_pWndEquipBindConfirm)
						g_WndMng.m_pWndEquipBindConfirm = new CWndEquipBindConfirm(CWndEquipBindConfirm::EQUIP_DRAG_AND_DROP);
						g_WndMng.m_pWndEquipBindConfirm->SetInformation(pItemElem);
						g_WndMng.m_pWndEquipBindConfirm->Initialize(NULL);
					}
					else
					{
						g_DPlay.SendDoEquip( pItemElem );
					}
					return TRUE;
#else // __EQUIP_BIND
					g_DPlay.SendDoEquip( pItemElem );
					return TRUE;
#endif // __EQUIP_BIND
				}
			}
		}
	}
	return CWndBase::OnDropIcon( pShortcut, point );
}
void CWndInventory::OnDestroyChildWnd( CWndBase* pWndChild )
{
	if( pWndChild == m_pWndConfirmBuy )
		SAFE_DELETE( m_pWndConfirmBuy );
}


#if __VER >= 13 // __RENEW_CHARINFO
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 캐릭터 통합 정보
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
CWndCharInfo::CWndCharInfo()
{
	m_nStrCount = 0;
	m_nStaCount = 0;
	m_nDexCount = 0;
	m_nIntCount = 0;
	m_nGpPoint = 0;

	m_nATK = 0;
	m_nDEF = 0;
	m_nCritical = 0;
	m_nATKSpeed = 0;

	m_bExpert = FALSE;
	m_pWndChangeJob = NULL;
	m_fWaitingConfirm	= FALSE;
	m_nDisplay = 1;

}


CWndCharInfo::~CWndCharInfo()
{
	SAFE_DELETE(m_pWndChangeJob);
}

void CWndCharInfo::OnDraw(C2DRender* p2DRender)
{

	CRect rect = GetClientRect();
	rect.bottom -= 30;
	int			y = 0, nNext = 15, nyAdd3 = 21;
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
	DWORD		dwColor = D3DCOLOR_ARGB(255,255,255,255);//agora
	//DWORD		dwColor = D3DCOLOR_ARGB(255,0,0,0);//antes
/***********************************************************************************/
#if __VER >= 10 // __CSC_VER9_1
	BYTE		checkhero = g_pPlayer->GetLegendChar();
	int			xpos = 0;
	int			ypos = 0;
	CTexture*	pTexture;
	CString		strPath;
	CPoint		point;
#endif //__CSC_VER9_1

	y = 13;
	p2DRender->TextOut( 60, y, prj.GetText((TID_APP_CHARACTER_BASE)), dwColor );
	y = 10 + nyAdd3;
	if( TRUE ) //::GetLanguage() == LANG_JAP )
	{
		p2DRender->TextOut( 80, y, g_pPlayer->GetName()       , dwColor); y += nNext;
		p2DRender->TextOut( 80, y, g_pPlayer->GetJobString()  , dwColor ); y += nNext;
#if __VER >= 10 // __CSC_VER9_1
		ypos = y;
		if(checkhero == LEGEND_CLASS_MASTER)
		{
			if(g_pPlayer->GetLevel() < 100)
				xpos = 97;
			else
				xpos = 103;
		}
#if __VER >= 15 // __HERO129_VER15				// 15차 히어로 레벨확장
		if(checkhero == LEGEND_CLASS_HERO || checkhero == LEGEND_CLASS_LEGENDHERO)
			xpos = 103;
	#else
		if(checkhero == LEGEND_CLASS_HERO || checkhero == LEGEND_CLASS_LEGENDHERO)
			xpos = 80;
		else
	#endif	// 15차 히어로 레벨확장	
			p2DRender->TextOut( 80, y, g_pPlayer->GetLevel()      , dwColor ); y += nNext;
#else
		p2DRender->TextOut( 80, y, g_pPlayer->GetLevel()      , dwColor ); y += nNext;
#endif //__CSC_VER9_1

		y = 81+ nyAdd3;
	}
	else
	{
		p2DRender->TextOut( 50, y, g_pPlayer->GetName()       , dwColor); y += nNext;
		p2DRender->TextOut( 50, y, g_pPlayer->GetJobString()  , dwColor ); y += nNext;
#if __VER >= 10 // __CSC_VER9_1
		ypos = y;
		if(checkhero == LEGEND_CLASS_MASTER)
		{
			if(g_pPlayer->GetLevel() < 100)
				xpos = 67;
			else
				xpos = 73;
		}
#if __VER >= 15 // __HERO129_VER15				// 15차 히어로 레벨확장
		if(checkhero == LEGEND_CLASS_HERO || checkhero == LEGEND_CLASS_LEGENDHERO )
			xpos = 73;
	#else
		if(checkhero == LEGEND_CLASS_HERO || checkhero == LEGEND_CLASS_LEGENDHERO )
			xpos = 50;
		else
	#endif	// 15차 히어로 레벨확장	
			p2DRender->TextOut( 50, y, g_pPlayer->GetLevel()      , dwColor ); y += nNext;
	
#else
		p2DRender->TextOut( 50, y, g_pPlayer->GetLevel()      , dwColor ); y += nNext;
#endif //__CSC_VER9_1
		y = 81 + nyAdd3;
	}

#if __VER >= 10 // __CSC_VER9_1
	point.x = xpos;
	point.y = ypos - 2;
	if(checkhero == LEGEND_CLASS_MASTER) //전승을 했을 경우.
	{
		if(/*g_pPlayer->m_nLevel >= 60 && */g_pPlayer->m_nLevel < 70) //Level Down될 경우를 생각해서 주석처리
			strPath = MakePath( DIR_ICON, "Icon_MasterMark1.dds");
		else if(g_pPlayer->m_nLevel >= 70 && g_pPlayer->m_nLevel < 80)
			strPath = MakePath( DIR_ICON, "Icon_MasterMark2.dds");
		else if(g_pPlayer->m_nLevel >= 80 && g_pPlayer->m_nLevel < 90)
			strPath = MakePath( DIR_ICON, "Icon_MasterMark3.dds");
		else if(g_pPlayer->m_nLevel >= 90 && g_pPlayer->m_nLevel < 100)
			strPath = MakePath( DIR_ICON, "Icon_MasterMark4.dds");
		else if(g_pPlayer->m_nLevel >= 100 && g_pPlayer->m_nLevel < 110)
			strPath = MakePath( DIR_ICON, "Icon_MasterMark5.dds");
		else if(g_pPlayer->m_nLevel >= 110 && g_pPlayer->m_nLevel <= 120)
			strPath = MakePath( DIR_ICON, "Icon_MasterMark6.dds");

		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, strPath, 0xffff00ff );
		if(pTexture != NULL)
			pTexture->Render( p2DRender, point );			
		
	}
	else if(checkhero == LEGEND_CLASS_HERO || checkhero == LEGEND_CLASS_LEGENDHERO ) //영웅일 경우.
	{
		strPath = MakePath( DIR_ICON, "Icon_HeroMark.dds");
		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, strPath, 0xffff00ff );
		if(pTexture != NULL)
			pTexture->Render( p2DRender, point );
	}

#endif //__CSC_VER9_1
	//서버 정보
	y = 55+ nyAdd3;
	CString strServerName;
	strServerName.Format( "%s", g_dpCertified.GetServerName(g_Option.m_nSer) );
	if( TRUE ) //::GetLanguage() == LANG_JAP )
	{
		p2DRender->TextOut( 80, y, strServerName, dwColor );
		y += nNext;
	}
	else
	{
		p2DRender->TextOut( 50, y, strServerName, dwColor );
		y += nNext;
	}

	//채널 정보
	LPSERVER_DESC pServerDesc = NULL;
	int nCount = 0;
	for( int j = 0; j < (int)( g_dpCertified.m_dwSizeofServerset ); j++ )
	{
		if(g_dpCertified.m_aServerset[j].dwParent == NULL_ID)
		{
			if(nCount++ == g_Option.m_nSer)
				pServerDesc = g_dpCertified.m_aServerset + j;
		}
		if(g_dpCertified.m_aServerset[j].dwParent != NULL_ID && g_dpCertified.m_aServerset[j].lEnable != 0L)
		{
			if(pServerDesc != NULL && g_dpCertified.m_aServerset[j].dwParent == pServerDesc->dwID)
			{
				strServerName.Format( "%s",  g_dpCertified.m_aServerset[j+g_Option.m_nMSer].lpName );
				if( TRUE ) //::GetLanguage() == LANG_JAP )
					p2DRender->TextOut( 80, y, strServerName, dwColor );
				else
					p2DRender->TextOut( 50, y, strServerName, dwColor );

				j = g_dpCertified.m_dwSizeofServerset;
			}
		}
	}
	/*
	y = 96;
	
	p2DRender->TextOut( 96, y, g_pPlayer->GetFlightLv(), dwColor ); y += nNext;
	{
		char szBuff[64];
		int nMaxFxp = prj.m_aFxpCharacter[ g_pPlayer->GetFlightLv() + 1 ].dwFxp;
		sprintf( szBuff, "%5.2f%%", (float)g_pPlayer->GetFxp() * 100.0f / (float)nMaxFxp );
		p2DRender->TextOut( 90, y, szBuff, dwColor ); y += nNext;
	}
	*/
	y = 10+ nyAdd3;
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
	dwColor = D3DCOLOR_ARGB( 255, 238, 221, 130 );//AGORA
	//dwColor = D3DCOLOR_ARGB(255, 0, 0, 180);//ANTES
/***********************************************************************************/
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_01), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_APP_CHARACTER_JOB), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_02), dwColor ); y += nNext;
	//p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_03), dwColor ); y += nNext;
	//직업레벨 -> 서버/채널 정보
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHAR_SERVER), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHAR_SERVERNAME), dwColor ); y += nNext;
	/*
	y += 10;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_04), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_05), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_06), dwColor );
	*/

	p2DRender->TextOut( 60, 113, prj.GetText((TID_APP_CHARACTER_DETAIL)), D3DCOLOR_ARGB(255,0,0,0));
	/////////////////////////// detail begin //////////////////////////////////
	int nyAdd = 121;
	int x = 5, nNextX = 100;
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
	dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );//AGORA
	//dwColor = D3DCOLOR_ARGB(255,0,0,0);//ANTES
/***********************************************************************************/
	x = 50; y = 10 + nyAdd;
	nNext = 15;
	// 공격력
	RenderATK( p2DRender, x, y );
	y += nNext;

	//방어력
	if(	m_nStaCount != 0 && GetVirtualDEF() != g_pPlayer->GetShowDefense( FALSE ) ) //임의 스탯이 변경되고 현재 능력치랑 다를 경우
	{
		if( (g_nRenderCnt / 8) & 1 )	
		{
			dwColor = D3DCOLOR_ARGB(255,255,0,0);
		}
		p2DRender->TextOut( x , y, GetVirtualDEF(), dwColor ); y += nNext;
	}
	else
		p2DRender->TextOut( x , y, g_pPlayer->GetShowDefense( FALSE ), dwColor ); y += nNext;

	x = 140; y = 10 + nyAdd;

	//크리티컬
	CString strMsg;
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
    dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );//AGORA
	//dwColor = D3DCOLOR_ARGB(255,0,0,0);//ANTES
/**********************************************************************************/
	if(	m_nDexCount != 0 && GetVirtualCritical() != g_pPlayer->GetCriticalProb() ) //임의 스탯이 변경되고 현재 능력치랑 다를 경우
	{
		if( (g_nRenderCnt / 8) & 1 )	
		{
			dwColor = D3DCOLOR_ARGB(255,255,0,0);
		}
		strMsg.Format( "%d%%", GetVirtualCritical() );
	}
	else
		strMsg.Format( "%d%%", g_pPlayer->GetCriticalProb() );
	p2DRender->TextOut( x , y, strMsg, dwColor ); y += nNext;

	//공격속도	
	float fAttackSpeed;
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
    dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );//AGORA
	//dwColor = D3DCOLOR_ARGB(255,0,0,0);//ANTES
/**********************************************************************************/
	if(	m_nDexCount != 0 && GetVirtualATKSpeed() != g_pPlayer->GetAttackSpeed() ) //임의 스탯이 변경되고 현재 능력치랑 다를 경우
	{
		if( (g_nRenderCnt / 8) & 1 )	
		{
			dwColor = D3DCOLOR_ARGB(255,255,0,0);
		}
		fAttackSpeed = GetVirtualATKSpeed();
	}
	else
		fAttackSpeed = g_pPlayer->GetAttackSpeed();

	strMsg.Format( "%d%%", int( fAttackSpeed*100.0f ) /2 );
	p2DRender->TextOut( x , y, strMsg, dwColor ); y += nNext;

	x =15; nNextX = 60;
	// 아래부터 능력치 관련 
	y = 52 + nyAdd;
	int StatYPos = 50;

	if( g_pPlayer->m_nStr == g_pPlayer->GetStr() )
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
    dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );//AGORA
	//dwColor = D3DCOLOR_ARGB(255,0,0,0);//ANTES
/**********************************************************************************/
	else
	if( g_pPlayer->m_nStr < g_pPlayer->GetStr() )
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
    dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );//AGORA
	//dwColor = D3DCOLOR_ARGB(255,0,0,255);//ANTES
/**********************************************************************************/
	else
		dwColor = D3DCOLOR_ARGB(255,255,0,0);

#if __VER >= 19
	p2DRender->TextOut( StatYPos, y, g_pPlayer->GetStr(), dwColor );
	p2DRender->TextOut( StatYPos + 50, y, g_pPlayer->m_nStr, dwColor );y += nNext;
#else
	p2DRender->TextOut( StatYPos, y, g_pPlayer->GetStr(), dwColor );y += nNext;
#endif//__VER >= 19

	if( g_pPlayer->m_nSta == g_pPlayer->GetSta() )
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
    dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );//AGORA
	//dwColor = D3DCOLOR_ARGB(255,0,0,0);//ANTES
/**********************************************************************************/
	else
	if( g_pPlayer->m_nSta < g_pPlayer->GetSta() )
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
    dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );//AGORA
	//dwColor = D3DCOLOR_ARGB(255,0,0,255);//ANTES
/**********************************************************************************/
	else
		dwColor = D3DCOLOR_ARGB(255,255,0,0);

#if __VER >= 19
	p2DRender->TextOut( StatYPos, y, g_pPlayer->GetSta(), dwColor );
	p2DRender->TextOut( StatYPos + 50, y, g_pPlayer->m_nSta, dwColor );y += nNext;
#else
	p2DRender->TextOut( StatYPos, y, g_pPlayer->GetSta(), dwColor );y += nNext;
#endif//__VER >= 19

	if( g_pPlayer->m_nDex == g_pPlayer->GetDex() )
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
    dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );//AGORA
	//dwColor = D3DCOLOR_ARGB(255,0,0,0);//ANTES
/**********************************************************************************/
	else
	if( g_pPlayer->m_nDex < g_pPlayer->GetDex() )
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
    dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );//AGORA
	//dwColor = D3DCOLOR_ARGB(255,0,0,255);//ANTES
/**********************************************************************************/
	else
		dwColor = D3DCOLOR_ARGB(255,255,0,0);

#if __VER >= 19
	p2DRender->TextOut( StatYPos, y, g_pPlayer->GetDex(), dwColor );
	p2DRender->TextOut( StatYPos + 50, y, g_pPlayer->m_nDex, dwColor );y += nNext;
#else
	p2DRender->TextOut( StatYPos, y, g_pPlayer->GetDex(), dwColor );y += nNext;
#endif//__VER >= 19

	if( g_pPlayer->m_nInt == g_pPlayer->GetInt() )
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
    dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );//AGORA
	//dwColor = D3DCOLOR_ARGB(255,0,0,0);//ANTES
/**********************************************************************************/
	else
	if( g_pPlayer->m_nInt < g_pPlayer->GetInt() )
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
    dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );//AGORA
	//dwColor = D3DCOLOR_ARGB(255,0,0,255);//ANTES
/**********************************************************************************/
	else
		dwColor = D3DCOLOR_ARGB(255,255,0,0);

#if __VER >= 19
	p2DRender->TextOut( StatYPos, y, g_pPlayer->GetInt(), dwColor );
	p2DRender->TextOut( StatYPos + 50, y, g_pPlayer->m_nInt, dwColor );y += nNext;
#else
	p2DRender->TextOut( StatYPos, y, g_pPlayer->GetInt(), dwColor );y += nNext;
#endif//__VER >= 19

	if(m_nGpPoint)
	{
		if( (g_nRenderCnt / 8) & 1 )	
		{
			dwColor = D3DCOLOR_ARGB(255,255,0,0);
		}
	}

	p2DRender->TextOut( 105, y, m_nGpPoint, dwColor ); y += nNext;

	CRect rectHittest[5];

	rectHittest[0].SetRect( 10, 52+ nyAdd, 80, 65 + nyAdd);
	rectHittest[1].SetRect( 10, 67+ nyAdd, 80, 80 + nyAdd);
	rectHittest[2].SetRect( 10, 82+ nyAdd, 80, 95 + nyAdd);
	rectHittest[3].SetRect( 10, 97+ nyAdd, 80, 110 + nyAdd);
	rectHittest[4].SetRect( 10, 112+ nyAdd, 160, 125 + nyAdd);

	CRect rectTemp;
	CPoint ptTemp;
	// 툴팁 나오게 하기( Str, Sta, Dex, Int, GP )
	CPoint ptMouse = GetMousePoint();
	for( int iC = 0 ; iC < 5 ; ++iC )
	{
		if( rectHittest[iC].PtInRect( ptMouse ) )
		{
			ClientToScreen( &ptMouse );
			ClientToScreen( &rectHittest[iC] );
			CEditString strEdit;
			CString szString;
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
             DWORD dwColorName = D3DCOLOR_ARGB( 255, 255, 255, 255 );
			 DWORD dwColorTitle = D3DCOLOR_ARGB( 255,  50, 169, 169 );
	         DWORD dwColorCommand = D3DCOLOR_ARGB( 255, 255, 0, 0 );
			//DWORD dwColorName = D3DCOLOR_XRGB( 0, 93, 0 );//ANTES
			//DWORD dwColorCommand = D3DCOLOR_XRGB( 180, 0, 0 );//ANTES
/**********************************************************************************/
			if( iC == 0 )	// STR
			{
				szString.Format( "%s", prj.GetText( TID_TOOLTIP_STR ) );
				strEdit.AddString( szString, dwColorTitle , ESSTY_BOLD );
				szString.Format( "  %d\n ", g_pPlayer->GetStr() );
				strEdit.AddString( szString, dwColorTitle , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_STR0 ), dwColorName );
				strEdit.AddString( "(" );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_STR1 ), dwColorCommand );
				strEdit.AddString( ")" );
			}
			else if( iC == 1 ) // STA
			{
				szString.Format( "%s", prj.GetText( TID_TOOLTIP_STA ) );
				strEdit.AddString( szString, dwColorTitle , ESSTY_BOLD );
				szString.Format( "  %d\n ", g_pPlayer->GetSta() );
				strEdit.AddString( szString, dwColorTitle , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_STA0 ), dwColorName );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_STA1 ), dwColorName );
			}
			else if( iC == 2 ) // DEX
			{
				szString.Format( "%s", prj.GetText( TID_TOOLTIP_DEX ) );
				strEdit.AddString( szString, dwColorTitle , ESSTY_BOLD );
				szString.Format( "  %d\n ", g_pPlayer->GetDex() );
				strEdit.AddString( szString, dwColorTitle , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_DEX0 ), dwColorName);
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_DEX1 ), dwColorName );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_DEX2 ), dwColorName );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_DEX3 ), dwColorCommand );
			}
			else if( iC == 3 ) // INT
			{
				szString.Format( "%s", prj.GetText( TID_TOOLTIP_INT ) );
				strEdit.AddString( szString, dwColorTitle , ESSTY_BOLD );
				szString.Format( "  %d\n ", g_pPlayer->GetInt() );
				strEdit.AddString( szString, dwColorTitle , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_INT0 ), dwColorName );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_INT1 ), dwColorName );
			}
			else // GP
				strEdit.AddString( prj.GetText( TID_TOOLTIP_CHARSTATUS_GPPOINT ), dwColorName );
			
			g_toolTip.PutToolTip( 100, strEdit, rectHittest[iC], ptMouse, 3 );
			break;
		}
	}

	y = 10 + nyAdd;
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
	dwColor = D3DCOLOR_ARGB( 255, 238, 221, 130 );
	//dwColor = D3DCOLOR_ARGB(255, 0, 0, 180);//ANTES
/***********************************************************************************/
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_13), dwColor ); 
	p2DRender->TextOut( 85, y, prj.GetText(TID_GAME_CHARACTER_14), dwColor ); y += nNext;

	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_DEFENCE), dwColor ); 
	p2DRender->TextOut( 85, y, prj.GetText(TID_GAME_CHARACTER_15), dwColor ); y += nNext;
	y += 12;
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_STR), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_STA), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_DEX), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_INT), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_07), dwColor ); y += nNext;
	
	//능력치 상태 Tooltip
	rect.SetRect( 7, 10+ nyAdd, 160, 38 + nyAdd);
	if( rect.PtInRect( ptMouse ) )
	{
		ClientToScreen( &ptMouse );
		ClientToScreen( &rect );
		CEditString strEdit;
		if(m_nStrCount != 0 || m_nStaCount != 0 || m_nDexCount != 0 || m_nIntCount != 0)
			strEdit.AddString( prj.GetText( TID_TOOLTIP_CHARSTATUS_STATUS1 ) );
		else
			strEdit.AddString( prj.GetText( TID_TOOLTIP_CHARSTATUS_STATUS2 ) );
		g_toolTip.PutToolTip( 100, strEdit, rect, ptMouse, 3 );
	}
	//Edit Tooltip
	rect.SetRect(90, 52+ nyAdd, 160, 110+ nyAdd);
	if( rect.PtInRect( ptMouse ) )
	{
		ClientToScreen( &ptMouse );
		ClientToScreen( &rect );
		CEditString strEdit;
		strEdit.AddString( prj.GetText( TID_TOOLTIP_CHARSTATUS_EDIT ) );
		g_toolTip.PutToolTip( 100, strEdit, rect, ptMouse, 3 );
	}	
	//Button Tooltip
	rect = m_wndApply.m_rectWindow;
	if( rect.PtInRect( ptMouse ) )
	{
		ClientToScreen( &ptMouse );
		ClientToScreen( &rect );
		CEditString strEdit;
		strEdit.AddString( prj.GetText( TID_TOOLTIP_CHARSTATUS_APPLY ) );
		g_toolTip.PutToolTip( 100, strEdit, rect, ptMouse, 3 );
	}	
	rect = m_wndReset.m_rectWindow;
	if( rect.PtInRect( ptMouse ) )
	{
		ClientToScreen( &ptMouse );
		ClientToScreen( &rect );
		CEditString strEdit;
		strEdit.AddString( prj.GetText( TID_TOOLTIP_CHARSTATUS_RESET ) );
		g_toolTip.PutToolTip( 100, strEdit, rect, ptMouse, 3 );
	}

	//////////////// pvp /////////////////////////
	
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

	//CRect rect = GetClientRect();
	//rect.bottom -= 30;
	int nyAdd2 = 284;
	y = 15 + nyAdd2, nNext = 15;
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
	dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );
	//dwColor = D3DCOLOR_ARGB( 255, 0, 0, 0 )/*adeilson*/;//antes
/***********************************************************************************/
	char szBuff[32];
	int gap1 = 0;
	int gap2 = 0;
	gap1 -= 10;
	gap2 -= 10;
	
	p2DRender->TextOut( 60, 281, prj.GetText((TID_GAME_CHARACTTER_PVP0)), dwColor );

	strcpy( szBuff, g_pPlayer->GetFameName() );
	if( IsEmpty(szBuff) ) {	szBuff[0] = '-'; szBuff[1] = NULL; }
	p2DRender->TextOut( 100+gap1, y, szBuff  , dwColor ); y += nNext;
	y += 4;
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
	
	y = 13 + nyAdd2;
	nNext = 19;
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
	//dwColor = D3DCOLOR_ARGB(255, 0, 0, 180);//antes
	dwColor = D3DCOLOR_ARGB( 255, 238, 221, 130 );
/**********************************************************************************/
#if __VER >= 8 // __S8_PK
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTTER_PVP1), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTTER_PVP2), dwColor ); y += nNext;
	y += 20;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTTER_PVP3), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTTER_PVP4), dwColor ); y += nNext;
#else // __VER >= 8 // __S8_PK
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_08), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_09), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_10), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_11), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_12), dwColor ); y += nNext;
#endif // __VER >= 8 // __S8_PK
}


void CWndCharInfo::OnInitialUpdate()
{
	
	CWndBase::OnInitialUpdate();
	SetTexture( m_pApp->m_pd3dDevice, MakePath( "Theme\\", ::GetLanguage(), _T( "WndNewCharacter01.tga" ) ), TRUE );
	//FitTextureSize();
/*
	//SetTexture( m_pApp->m_pd3dDevice, MakePath( "Theme\\", ::GetLanguage(), _T( "WndCharacter2_2.tga" ) ), TRUE );
	//FitTextureSize();
	
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize( 70, 25);
	
	CRect rect1_1( x - ( size.cx / 2), y, ( x - ( size.cx / 2 ) ) + size.cx, y + size.cy );
	
	CRect rect2_1( x - size.cx - 10, y, ( x - size.cx - 10 ) + size.cx, y + size.cy );
	CRect rect2_2( x + 10          , y, ( x + 10           ) + size.cx, y + size.cy );
	
	CRect rect3_1( x - ( size.cx / 2) - size.cx - 10, y, (x - ( size.cx / 2) - size.cx - 10) + size.cx, y + size.cy );
	CRect rect3_2( x - ( size.cx / 2)               , y, (x - ( size.cx / 2)               ) + size.cx, y + size.cy );
	CRect rect3_3( x + ( size.cx / 2) + 10          , y, (x + ( size.cx / 2) + 10          ) + size.cx, y + size.cy );
*/
	// 아래부터 능력치 관련 
//	int nyAdd = 121;
//	int posY = 49 + nyAdd;
//	int posX = 128;


#if __VER >= 19
	int nyAdd = 121;
	int posY = 48 + nyAdd;
	int posX = 128;

//14 de tamanho.
	m_editStrCount.Create  ( g_Neuz.GetSafeHwnd(), 0, CRect( posX + 4, posY, posX + 38, posY + 16 ), this, 100 );
	m_wndStrPlus.Create  ( "", 0, CRect( posX + 36, posY + 2, posX + 50, posY + 18 ), this, 101 );
	m_wndStrMinus.Create  ( "", 0, CRect( posX + 52, posY + 2, posX + 66, posY + 18 ), this, 102 ); posY += 15;

	m_editStaCount.Create  ( g_Neuz.GetSafeHwnd(), 0, CRect( posX + 4, posY, posX + 38, posY + 16 ), this, 103 );
	m_wndStaPlus.Create  ( "", 0, CRect( posX + 36, posY + 2, posX + 50, posY + 18 ), this, 104 );
	m_wndStaMinus.Create  ( "", 0, CRect( posX + 52, posY + 2, posX + 66, posY + 18 ), this, 105 ); posY += 15;

	m_editDexCount.Create  ( g_Neuz.GetSafeHwnd(), 0, CRect( posX + 4, posY, posX + 38, posY + 16 ), this, 106 );
	m_wndDexPlus.Create  ( "", 0, CRect( posX + 36, posY + 2, posX + 50, posY + 18 ), this, 107 );
	m_wndDexMinus.Create  ( "", 0, CRect( posX + 52, posY + 2, posX + 66, posY + 18 ), this, 108 ); posY += 15;

	m_editIntCount.Create  ( g_Neuz.GetSafeHwnd(), 0, CRect( posX + 4, posY, posX + 38, posY + 16 ), this, 109 );
	m_wndIntPlus.Create  ( "", 0, CRect( posX + 36, posY + 2, posX + 50, posY + 18 ), this, 110 );
	m_wndIntMinus.Create  ( "", 0, CRect( posX + 52, posY + 2, posX + 66, posY + 18 ), this, 111 ); posY += 15;

#else//__VER >= 19
	int nyAdd = 121;
	int posY = 49 + nyAdd;
	int posX = 128;

	m_editStrCount.Create  ( g_Neuz.GetSafeHwnd(), 0, CRect( posX - 38, posY, posX - 4, posY + 16 ), this, 100 );
	m_wndStrPlus.Create  ( "", 0, CRect( posX, posY + 2, posX + 14, posY + 18 ), this, 101 );
	m_wndStrMinus.Create  ( "", 0, CRect( posX + 16, posY + 2, posX + 30, posY + 18 ), this, 102 ); posY += 15;

	m_editStaCount.Create  ( g_Neuz.GetSafeHwnd(), 0, CRect( posX - 38, posY, posX - 4, posY + 16 ), this, 103 );
	m_wndStaPlus.Create  ( "", 0, CRect( posX, posY + 2, posX + 14, posY + 18 ), this, 104 );
	m_wndStaMinus.Create  ( "", 0, CRect( posX + 16, posY + 2, posX + 30, posY + 18 ), this, 105 ); posY += 15;

	m_editDexCount.Create  ( g_Neuz.GetSafeHwnd(), 0, CRect( posX - 38, posY, posX - 4, posY + 16 ), this, 106 );
	m_wndDexPlus.Create  ( "", 0, CRect( posX, posY + 2, posX + 14, posY + 18 ), this, 107 );
	m_wndDexMinus.Create  ( "", 0, CRect( posX + 16, posY + 2, posX + 30, posY + 18 ), this, 108 ); posY += 15;

	m_editIntCount.Create  ( g_Neuz.GetSafeHwnd(), 0, CRect( posX - 38, posY, posX - 4, posY + 16 ), this, 109 );
	m_wndIntPlus.Create  ( "", 0, CRect( posX, posY + 2, posX + 14, posY + 18 ), this, 110 );
	m_wndIntMinus.Create  ( "", 0, CRect( posX + 16, posY + 2, posX + 30, posY + 18 ), this, 111 );
#endif//__VER >= 19


	m_wndStrPlus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndStrMinus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharMinus.bmp" ) ), TRUE );
	m_wndStaPlus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndStaMinus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharMinus.bmp" ) ), TRUE );
	m_wndDexPlus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndDexMinus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharMinus.bmp" ) ), TRUE );
	m_wndIntPlus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndIntMinus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharMinus.bmp" ) ), TRUE );

#if __VER >= 19
	posY += 22;
#else
	posY += 38;
#endif//__VER >= 19

	m_wndApply.Create  ( "Apply", 0, CRect( posX - 108, posY, posX - 58, posY + 22 ), this, 112 );
	m_wndReset.Create  ( "Reset", 0, CRect( posX - 30, posY, posX + 20, posY + 22 ), this, 113 );

	if(::GetLanguage() == LANG_USA)
	{
		m_wndApply.SetTexture( m_pApp->m_pd3dDevice, MakePath( "Theme\\", ::GetLanguage(), _T( "ButtNormal05.tga" ) ), TRUE );
		m_wndReset.SetTexture( m_pApp->m_pd3dDevice, MakePath( "Theme\\", ::GetLanguage(), _T( "ButtNormal05.tga" ) ), TRUE );
	}
	else
	{
		m_wndApply.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtNormal05.tga" ) ), TRUE );
		m_wndReset.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtNormal05.tga" ) ), TRUE );
	}

	//모든 스탯의 증가량 1로 고정이므로 아래 삭제
	/*
	LPJOBABILITY pJobAbillity;
	if( g_pPlayer->IsExpert() )
		pJobAbillity = &prj.m_aJobAbillity[ g_pPlayer->GetJob() ];
	else
		pJobAbillity = &prj.m_aJobAbillity[ JOB_VAGRANT ];

	m_wndStr.EnableWindow( ( g_pPlayer->GetRemainGP() >= pJobAbillity->dwStr ) ? TRUE : FALSE );
	m_wndSta.EnableWindow( ( g_pPlayer->GetRemainGP() >= pJobAbillity->dwSta ) ? TRUE : FALSE );
	m_wndDex.EnableWindow( ( g_pPlayer->GetRemainGP() >= pJobAbillity->dwDex ) ? TRUE : FALSE );
	m_wndInt.EnableWindow( ( g_pPlayer->GetRemainGP() >= pJobAbillity->dwInt ) ? TRUE : FALSE );
	*/

	m_nGpPoint = g_pPlayer->GetRemainGP();
	//스탯 증가량 한적 없으므로 Minus Button Default는 False
	m_wndStrMinus.EnableWindow(FALSE);
	m_wndStaMinus.EnableWindow(FALSE);
	m_wndDexMinus.EnableWindow(FALSE);
	m_wndIntMinus.EnableWindow(FALSE);

	if(g_pPlayer->GetRemainGP() <= 0)
	{
		m_wndStrPlus.EnableWindow(FALSE);
		m_wndStaPlus.EnableWindow(FALSE);
		m_wndDexPlus.EnableWindow(FALSE);
		m_wndIntPlus.EnableWindow(FALSE);
	}
	int nyAdd2 = 280;
	int y = 105 + nyAdd2;
	if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
		m_wndChangeJob.Create( ">", 0, CRect( 130, y, 135+40, y + 13 ), this, 10  ); 

	//SetTexture(m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "WndTile00.tga")), TRUE);

	RefreshStatPoint();

	//FitTextureSize();

	MakeVertexBuffer();
}


BOOL CWndCharInfo::Initialize(CWndBase* pWndParent,DWORD dwWndId)
{
	CRect rect = m_pWndRoot->GetWindowRect();
	return CWndBase::Create(WBS_THICKFRAME|WBS_MOVE|WBS_SOUND|WBS_CAPTION|WBS_EXTENSION,rect,pWndParent,dwWndId);

}


BOOL CWndCharInfo::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndBase::OnCommand(nID,dwMessage,pWndBase);
}


BOOL CWndCharInfo::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	int editnum = 0;
	
	if( nID == 100 || nID == 103 || nID == 106 || nID == 109 )
	{
		CString tempnum = m_editStrCount.GetString();
		for( int i = 0 ; i < tempnum.GetLength() ; i++ )
		{
			if( isdigit2( tempnum.GetAt(i) ) == FALSE )
				return FALSE;
		}
	}
	switch(nID) 
	{
		case 100:
			editnum = atoi( m_editStrCount.GetString() );
			if(editnum > g_pPlayer->GetRemainGP() - (m_nStaCount + m_nDexCount + m_nIntCount))
				editnum = g_pPlayer->GetRemainGP() - (m_nStaCount + m_nDexCount + m_nIntCount);
			m_nStrCount = editnum;
			break;
		case 103:
			editnum = atoi( m_editStaCount.GetString() );
			if(editnum > g_pPlayer->GetRemainGP() - (m_nStrCount + m_nDexCount + m_nIntCount))
				editnum = g_pPlayer->GetRemainGP() - (m_nStrCount + m_nDexCount + m_nIntCount);
			m_nStaCount = editnum;
			break;
		case 106:
			editnum = atoi( m_editDexCount.GetString() );
			if(editnum > g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nIntCount))
				editnum = g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nIntCount);
			m_nDexCount = editnum;
			break;
		case 109:
			editnum = atoi( m_editIntCount.GetString() );
			if(editnum > g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nDexCount))
				editnum = g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nDexCount);
			m_nIntCount = editnum;
			break;
	}

	if( message == WNM_CLICKED )		
	{
		switch(nID) 
		{
			case 101: //Str Plus
				m_nStrCount++;
				break;
			case 102: //Str Minus
				m_nStrCount--;
				break;
			case 104: //Sta Plus
				m_nStaCount++;
				break;
			case 105: //Sta Minus
				m_nStaCount--;
				break;
			case 107: //Dex Plus
				m_nDexCount++;
				break;
			case 108: //Dex Minus
				m_nDexCount--;
				break;
			case 110: //Int Plus
				m_nIntCount++;
				break;
			case 111: //Int Minus
				m_nIntCount--;
				break;
			case 112: //Apply
				if(m_nStrCount != 0 || m_nStaCount != 0 || m_nDexCount != 0 || m_nIntCount != 0)
				{
					SAFE_DELETE( g_WndMng.m_pWndStateConfirm );
					g_WndMng.m_pWndStateConfirm = new CWndStateConfirm;
					g_WndMng.m_pWndStateConfirm->Initialize();
				}
				break;
			case 113: //Reset
				m_nStrCount = 0;
				m_nStaCount = 0;
				m_nDexCount = 0;
				m_nIntCount = 0;
				RefreshStatPoint();
				break;

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

	if(nID >= 100 && nID <= 111) //If use Full GpPoint
	{
		if(g_pPlayer->GetRemainGP() == (m_nStrCount + m_nStaCount + m_nDexCount + m_nIntCount))
		{
			m_wndStrPlus.EnableWindow(FALSE);
			m_wndStaPlus.EnableWindow(FALSE);
			m_wndDexPlus.EnableWindow(FALSE);
			m_wndIntPlus.EnableWindow(FALSE);
		}
		else if(g_pPlayer->GetRemainGP() > (m_nStrCount + m_nStaCount + m_nDexCount + m_nIntCount))
		{
			m_wndStrPlus.EnableWindow(TRUE);
			m_wndStaPlus.EnableWindow(TRUE);
			m_wndDexPlus.EnableWindow(TRUE);
			m_wndIntPlus.EnableWindow(TRUE);
		}
	}

	return CWndBase::OnChildNotify( message, nID, pLResult );
}


void CWndCharInfo::OnSize(UINT nType, int cx, int cy)
{

}


void CWndCharInfo::OnLButtonUp(UINT nFlags, CPoint point)
{

}


void CWndCharInfo::OnLButtonDown(UINT nFlags, CPoint point)
{

}


void CWndCharInfo::OnMouseWndSurface( CPoint point )
{

}


BOOL CWndCharInfo::Process()
{
	//Control Button Plus or Minus
	if(m_nStrCount > 0)
		m_wndStrMinus.EnableWindow(TRUE);
	else
		m_wndStrMinus.EnableWindow(FALSE);

	if(m_nStaCount > 0)
		m_wndStaMinus.EnableWindow(TRUE);
	else
		m_wndStaMinus.EnableWindow(FALSE);

	if(m_nDexCount > 0)
		m_wndDexMinus.EnableWindow(TRUE);
	else
		m_wndDexMinus.EnableWindow(FALSE);

	if(m_nIntCount > 0)
		m_wndIntMinus.EnableWindow(TRUE);
	else
		m_wndIntMinus.EnableWindow(FALSE);

	m_nGpPoint = g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nDexCount + m_nIntCount);

	if(m_nGpPoint > 0)
	{
		m_wndStrPlus.EnableWindow(TRUE);
		m_wndStaPlus.EnableWindow(TRUE);
		m_wndDexPlus.EnableWindow(TRUE);
		m_wndIntPlus.EnableWindow(TRUE);
		
		m_editStrCount.EnableWindow(TRUE);
		m_editStaCount.EnableWindow(TRUE);
		m_editDexCount.EnableWindow(TRUE);
		m_editIntCount.EnableWindow(TRUE);
	}
	else
	{
		m_wndStrPlus.EnableWindow(FALSE);
		m_wndStaPlus.EnableWindow(FALSE);
		m_wndDexPlus.EnableWindow(FALSE);
		m_wndIntPlus.EnableWindow(FALSE);
		
		m_editStrCount.EnableWindow(FALSE);
		m_editStaCount.EnableWindow(FALSE);
		m_editDexCount.EnableWindow(FALSE);
		m_editIntCount.EnableWindow(FALSE);
	}

	if(m_nStrCount > 0 || m_nStaCount > 0 || m_nDexCount > 0 || m_nIntCount > 0)
	{
		m_wndApply.EnableWindow(TRUE);
		m_wndReset.EnableWindow(TRUE);
	}
	else
	{
		m_wndApply.EnableWindow(FALSE);
		m_wndReset.EnableWindow(FALSE);
	}

	RefreshStatPoint();
	return TRUE;
}


void CWndCharInfo::RefreshStatPoint()
{
	CString tempstr;
	tempstr.Format("%d", m_nStrCount);
	m_editStrCount.SetString(tempstr);
	tempstr.Format("%d", m_nStaCount);
	m_editStaCount.SetString(tempstr);
	tempstr.Format("%d", m_nDexCount);
	m_editDexCount.SetString(tempstr);
	tempstr.Format("%d", m_nIntCount);
	m_editIntCount.SetString(tempstr);
}


void CWndCharInfo::GetVirtualATK(int* pnMin, int* pnMax)
{
	int nParts = PARTS_RWEAPON;
	
	*pnMin = 0;
	*pnMax = 0;
	
	if( g_pPlayer )
	{
		ItemProp* pItemProp = g_pPlayer->GetActiveHandItemProp( nParts );
		if( pItemProp == NULL )
			return;
		
		*pnMin = pItemProp->dwAbilityMin * 2;
		*pnMax = pItemProp->dwAbilityMax * 2;
		
		*pnMin = g_pPlayer->GetParam( DST_ABILITY_MIN, *pnMin );
		*pnMax = g_pPlayer->GetParam( DST_ABILITY_MAX, *pnMax );
		
		int nATK = 0;
		switch( pItemProp->dwWeaponType )
		{
			case WT_MELEE_SWD:
				nATK = (int)( float(( g_pPlayer->GetStr() + m_nStrCount - 12 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_SWD)) + (float(g_pPlayer->GetLevel() * 1.1f)) );
				break;
			case WT_MELEE_AXE:
				nATK = (int)( float(( g_pPlayer->GetStr() + m_nStrCount - 12 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_AXE)) + (float(g_pPlayer->GetLevel() * 1.2f)) );
				break;
			case WT_MELEE_STAFF:
				nATK = (int)( float(( g_pPlayer->GetStr() + m_nStrCount - 10 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_STAFF)) + (float(g_pPlayer->GetLevel() * 1.1f)) );
				break;
			case WT_MELEE_STICK:
				nATK = (int)( float(( g_pPlayer->GetStr() + m_nStrCount - 10 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_STICK)) + (float(g_pPlayer->GetLevel() * 1.3f)) );
				break;
			case WT_MELEE_KNUCKLE:
				nATK = (int)( float( (g_pPlayer->GetStr() + m_nStrCount - 10 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_KNUCKLE)) + (float(g_pPlayer->GetLevel() * 1.2f)) );
				break;
			case WT_MAGIC_WAND:
				nATK = (int)( ( g_pPlayer->GetInt() + m_nIntCount - 10 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_WAND) + g_pPlayer->GetLevel() * 1.2f );
				break;
			case WT_MELEE_YOYO:
				nATK = (int)( float(( g_pPlayer->GetStr() + m_nStrCount - 12 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_YOYO)) + (float(g_pPlayer->GetLevel() * 1.1f)) );
				break;
			case WT_RANGE_BOW:
				nATK = (int)( (((g_pPlayer->GetDex() + m_nDexCount -14)*4.0f + (g_pPlayer->GetLevel()*1.3f) + ((g_pPlayer->GetStr()+m_nStrCount)*0.2f)) * 0.7f) );
				break;
		}
		
		nATK += g_pPlayer->GetPlusWeaponATK( pItemProp->dwWeaponType );	// 무기의 추가 공격력를 구한다.

		int nPlus = nATK + g_pPlayer->GetParam( DST_CHR_DMG, 0 );

		*pnMin += nPlus;
		*pnMax += nPlus;
		
		CItemElem *pWeapon = g_pPlayer->GetWeaponItem( nParts );
		if( pWeapon && pWeapon->GetProp() )
		{
			float f = g_pPlayer->GetItemMultiplier( pWeapon );
			*pnMin = (int)( *pnMin * f );
			*pnMax = (int)( *pnMax * f );
		}
		
		if( pWeapon )
		{
			int nOption = pWeapon->GetAbilityOption();
			if( nOption > 0 )
			{
				int nValue = (int)pow( (float)( nOption ), 1.5f );

				*pnMin += nValue;
				*pnMax += nValue;
			}
		}
	}
}


int CWndCharInfo::GetVirtualDEF()
{
	int nDefense = 0;

	ATTACK_INFO info;
	memset( &info, 0x00, sizeof(info) );
	info.dwAtkFlags = AF_GENERIC;
	
	float fFactor = 1.0f;
	if( g_pPlayer )
	{
		JobProp* pProperty = prj.GetJobProp( g_pPlayer->GetJob() ); 
		assert( pProperty );
		fFactor = pProperty->fFactorDef;
	}
	nDefense	= (int)( ((((g_pPlayer->GetLevel()*2) + ((g_pPlayer->GetSta()+m_nStaCount)/2)) / 2.8f ) - 4) + ((g_pPlayer->GetSta() + m_nStaCount - 14) * fFactor) );
	nDefense	= nDefense + (g_pPlayer->GetDefenseByItem( FALSE ) / 4);
	nDefense	= nDefense + ( g_pPlayer->GetParam( DST_ADJDEF, 0 ) );

	nDefense	= (int)( nDefense * g_pPlayer->GetDEFMultiplier( &info ) );

#ifdef __JEFF_11
	if( nDefense < 0 )
		nDefense	= 0;
#endif	// __JEFF_11
	return nDefense;
}


int CWndCharInfo::GetVirtualCritical()
{
	int nCritical;
	nCritical	= ((g_pPlayer->GetDex() + m_nDexCount) / 10);
	nCritical	= (int)( nCritical * g_pPlayer->GetJobPropFactor( JOB_PROP_CRITICAL ) );
	nCritical	= g_pPlayer->GetParam( DST_CHR_CHANCECRITICAL, nCritical );	// 크리티컬 확률을 높여주는 스킬관련 
#ifdef __JEFF_11
	if( nCritical < 0 )
		nCritical	= 0;
#endif	// __JEFF_11
	
	if( g_pPlayer )
	{
		if( g_pPlayer->m_idparty && (g_pPlayer->m_dwFlag & MVRF_CRITICAL) )	// 파티 && 1회 크리티컬 발동?
		{
			if( g_Party.IsMember( g_pPlayer->m_idPlayer ) )
				nCritical += (g_Party.m_nSizeofMember / 2);	
			g_pPlayer->m_dwFlag &= (~MVRF_CRITICAL);
		}

#if __VER < 9 // __S_9_ADD
		if( g_pPlayer->IsAfterDeath() )							// 죽음 이후 상태라면?
			nCritical += CRITICAL_AFTER_DEATH;
		
		int nHitPercent = g_pPlayer->GetHitPointPercent( 100 );
		if( nHitPercent < CRITICAL_BERSERK_HP )			// HP가 MAX대비 30% 미만? 
		{
			// CRITICAL_BERSERK_HP : nHitPercent = CRITICAL_BERSERK_PROB : x
			// 30 : 15 = 20 : x
			nCritical += CRITICAL_BERSERK_PROB - ( nHitPercent * CRITICAL_BERSERK_PROB / CRITICAL_BERSERK_HP );
		}
#endif // __S_9_ADD
	}
	return nCritical;
}


float CWndCharInfo::GetVirtualATKSpeed()
{
	float fSpeed = 1.0f;
	float fItem = 1.0f;
	
	ItemProp* pWeaponProp = g_pPlayer->GetActiveHandItemProp();
	if( pWeaponProp )
		fItem = pWeaponProp->fAttackSpeed;
	
	JobProp* pProperty = prj.GetJobProp( g_pPlayer->GetJob() ); 
	ASSERT( pProperty );
	
	// A = int( 캐릭터의 공속 + ( 무기의 공속 * ( 4 * 덱스 + ( 레벨 / 8 ) ) ) - 3 )
	// 공격속도 = ( ( 50 / 200 - A ) / 2 ) + 가중치 
	int A = int( pProperty->fAttackSpeed + ( fItem * ( 4.0f * (g_pPlayer->GetDex() + m_nDexCount) + g_pPlayer->GetLevel() / 8.0f ) ) - 3.0f );

	if( 187.5f <= A )
		A	= (int)( 187.5f );

	const MAX_ATTACK_SPEED_PLUSVALUE = 18;
	
	int nIndex = A / 10;
	nIndex = max( nIndex, 0 );
	nIndex = min( nIndex, (MAX_ATTACK_SPEED_PLUSVALUE-1) );
	
	float fPlusValue[MAX_ATTACK_SPEED_PLUSVALUE] = {
		0.08f, 0.16f, 0.24f, 0.32f,	0.40f,
		0.48f, 0.56f, 0.64f, 0.72f,	0.80f,
		0.88f, 0.96f, 1.04f, 1.12f,	1.20f,
		1.30f, 1.38f, 1.50f
	};
	
	fSpeed = ( ( 50.0f / (200.f - A) ) / 2.0f ) + fPlusValue[nIndex];
	
	float fDstParam = g_pPlayer->GetParam( DST_ATTACKSPEED, 0 ) / 1000.0f;
	fSpeed += fDstParam;
	
	if( 0 < g_pPlayer->GetParam( DST_ATTACKSPEED_RATE, 0 ) )
		fSpeed = fSpeed + ( fSpeed * g_pPlayer->GetParam( DST_ATTACKSPEED_RATE, 0 ) / 100 );
	
	if( fSpeed < 0.0f )
		fSpeed = 0.1f;
	if( fSpeed > 2.0f )
		fSpeed = 2.0f;
	
	return fSpeed;
}


void CWndCharInfo::RenderATK( C2DRender* p2DRender, int x, int y )
{
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
	//DWORD dwColor = D3DCOLOR_ARGB(255,0,0,0);//antes
	DWORD dwColor = D3DCOLOR_ARGB(255,255,255,255);
/***********************************************************************************/
	char szText[32];
	int nMin, nMax;
	int nTemp1, nTemp2;

	GetVirtualATK(&nTemp1, &nTemp2);
	g_pPlayer->GetHitMinMax( &nMin, &nMax );

	if(	(m_nStrCount != 0 || m_nDexCount != 0 || m_nIntCount != 0) && (nTemp1 != nMin || nTemp2 != nMax) ) //임의 스탯이 변경되고 현재 능력치랑 다를 경우
	{
		if( (g_nRenderCnt / 8) & 1 )	
		{
			dwColor = D3DCOLOR_ARGB(255,255,0,0);
		}

		GetVirtualATK(&nMin, &nMax);
	}
	else
		g_pPlayer->GetHitMinMax( &nMin, &nMax );
	
	int nATK = ( nMin + nMax ) / 2;

	if( g_pPlayer->IsSMMode( SM_ATTACK_UP1 ) || g_pPlayer->IsSMMode( SM_ATTACK_UP ) )
		nATK	= (int)( nATK * 1.2f );
	
	if( 0 < g_pPlayer->GetParam( DST_ATKPOWER_RATE, 0 ) )
		nATK = nATK + ( nATK * g_pPlayer->GetParam( DST_ATKPOWER_RATE, 0 ) / 100 );

	nATK += g_pPlayer->GetWeaponPlusDamage( 1, FALSE );
	nATK += g_pPlayer->GetParam( DST_ATKPOWER, 0 );

#ifdef __JEFF_11
	if( nATK < 0 )
		nATK	= 0;
#endif	// __JEFF_11

	wsprintf( szText, "%d", nATK );

	p2DRender->TextOut( x, y, szText, dwColor );
}



CWndHonor::CWndHonor()
{
	m_nSelectedId = 0;
	m_vecTitle.clear();	
}

CWndHonor::~CWndHonor()
{

}

void CWndHonor::OnDraw( C2DRender* p2DRender )
{
#if __VER >= 13 // __HONORABLE_TITLE
	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	LPWNDCTRL	pCustom = NULL;
	DWORD		dwNormal;
	int			nIndex = 0;
	pCustom = GetWndCtrl( WIDC_LISTBOX1 );
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
	//dwNormal = D3DCOLOR_ARGB( 255, 0, 0, 0 )/*adeilson*/;//antes
	dwNormal = D3DCOLOR_ARGB( 255, 255, 255, 255 );
/***********************************************************************************/

	if(m_vecTitle.empty() != false)
		p2DRender->TextOut( pCustom->rect.left + 5, pCustom->rect.top + 8 + (nIndex)*16,prj.GetText(TID_GAME_NO_TITLE), dwNormal);
#endif
}


void CWndHonor::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	// 타이틀을 검색해서 리스트에 추가하고, 없으면 없다는 메시지 출력
	RefreshList();
	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	pWndListBox->m_nSelectColor = D3DCOLOR_ARGB(255, 255, 0, 0);
}

void CWndHonor::RefreshList()
{
#if __VER >= 13 // __HONORABLE_TITLE
	CWndButton* pWndButton1 = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	pWndListBox->ResetContent();
	pWndButton1->EnableWindow(FALSE);

	m_vecTitle.clear();
	m_vecTitle	= CTitleManager::Instance()->m_vecEarned;

	if(g_pPlayer)
		m_nSelectedId = g_pPlayer->m_nHonor;

	if(m_vecTitle.size() > 0)
	{
		vector<EarnedTitle>::iterator	iter;
		pWndListBox->AddString(prj.GetText(TID_GAME_NOT_SELECTED_TITLE));
		for(iter = m_vecTitle.begin(); iter != m_vecTitle.end(); ++iter)
		{
			pWndListBox->AddString(iter->strTitle.GetBuffer(0));
		}
	}
#endif	// __HONORABLE_TITLE
}


BOOL CWndHonor::Initialize( CWndBase* pWndParent,DWORD dwWndId )
{

	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_HONOR, 0, CPoint( 0, 0 ), pWndParent );
}


BOOL CWndHonor::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{

	return CWndNeuz::OnCommand(nID,dwMessage,pWndBase);
}


BOOL CWndHonor::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
#if __VER >= 13 // __HONORABLE_TITLE
	CWndButton* pWndButton1 = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	switch(nID)
	{
	case WIDC_LISTBOX1: // view ctrl
		{
			CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
			if(pWndListBox->GetCurSel() == 0)
			{
				m_nSelectedId = -1;
				pWndButton1->EnableWindow(TRUE);
			}
			else
			{
				m_nSelectedId = m_vecTitle[pWndListBox->GetCurSel() - 1].nId;
				pWndButton1->EnableWindow(TRUE);
			}
			break;
		}
	case WIDC_BUTTON1:// 선택된 타이틀을 사용하게 한다
		if(g_pPlayer)
		{
			pWndButton1->EnableWindow(FALSE);
			g_DPlay.SendReqHonorTitleChange(m_nSelectedId);
			break;
		}
	};
#endif	// __HONORABLE_TITLE
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}


void CWndHonor::OnSize(UINT nType, int cx, int cy)
{

	CWndNeuz::OnSize(nType,cx,cy);
}


void CWndHonor::OnLButtonUp( UINT nFlags, CPoint point )
{


}


void CWndHonor::OnLButtonDown( UINT nFlags, CPoint point )
{


}
#endif // __RENEW_CHARINFO


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 캐릭터 정보  
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndCharacter::CWndCharacter()
{
}
CWndCharacter::~CWndCharacter()
{
}
void CWndCharacter::OnDraw(C2DRender* p2DRender)
{
}
void CWndCharacter::SerializeRegInfo( CAr& ar, DWORD& dwVersion )
{
	CWndNeuz::SerializeRegInfo( ar, dwVersion );
	CWndTabCtrl* lpTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	if( ar.IsLoading() )
	{
		int nCurSel;
		ar >> nCurSel;
#if __VER >= 13 // __RENEW_CHARINFO
		if(nCurSel > 1) nCurSel = 0;
#endif // __RENEW_CHARINFO
		lpTabCtrl->SetCurSel( nCurSel );
	}
	else
	{
		ar << lpTabCtrl->GetCurSel();
	}
}
void CWndCharacter::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	//CRect rectRoot = m_pWndRoot->GetLayoutRect();
	//CPoint point( rectRoot.left, rectRoot.top + 96 + 16 );
	//Move( point );
	
	CRect rect = GetClientRect();
	rect.left = 5;
	rect.top = 15;

	CWndTabCtrl* lpTapCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	//m_wndTabCtrl.Create( WBS_NOFRAME, rect, this, 12 );
#if __VER >= 13 // __RENEW_CHARINFO
	m_wndCharInfo.Create(WBS_CHILD | WBS_NOFRAME , rect, lpTapCtrl, 100000);
	m_wndHonor.Create(WBS_CHILD | WBS_NOFRAME , rect, lpTapCtrl, APP_HONOR);
	m_wndHonor.AddWndStyle( WBS_NOFRAME );
	m_wndHonor.AddWndStyle( WBS_NODRAWFRAME );
	m_wndCharInfo.AddWndStyle( WBS_NOFRAME );
	//m_wndCharInfo.AddWndStyle( WBS_NODRAWFRAME );
	WTCITEM tabTabItem;

	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_GAME_TITLE_CHAR_INFO);//"기본";
	tabTabItem.pWndBase = &m_wndCharInfo;
	lpTapCtrl->InsertItem( 0, &tabTabItem );
	
	tabTabItem.pszText = prj.GetText(TID_GAME_TITLE_HONOR);//"달인";
	tabTabItem.pWndBase = &m_wndHonor;
	lpTapCtrl->InsertItem( 1, &tabTabItem );
	lpTapCtrl->SetCurSel( 0 );

	//lpTapCtrl->SetButtonLength( 60 );

#else // __RENEW_CHARINFO
	m_wndStateBase.Create( WBS_CHILD | WBS_NOFRAME , rect, lpTapCtrl, 100000 );
	m_wndStateDetail.Create( WBS_CHILD | WBS_NOFRAME , rect, lpTapCtrl, 1000001 );
	m_wndPvp.Create( WBS_CHILD | WBS_NOFRAME , rect, lpTapCtrl, 1000002 );
	
	WTCITEM tabTabItem;

	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_APP_CHARACTER_BASE);//"기본";
	tabTabItem.pWndBase = &m_wndStateBase;
	lpTapCtrl->InsertItem( 0, &tabTabItem );
	//lpTapCtrl->InsertTexture( 0 ,MakePath( DIR_THEME, "TabCharacter1.bmp" ) );

	tabTabItem.pszText = prj.GetText(TID_APP_CHARACTER_DETAIL);//"자세히";
	tabTabItem.pWndBase = &m_wndStateDetail;
	lpTapCtrl->InsertItem( 1, &tabTabItem );
	//lpTapCtrl->InsertTexture( 1 ,MakePath( DIR_THEME, "TabCharacter2.bmp" ) );

#if __VER >= 8 // __S8_PK
	tabTabItem.pszText = prj.GetText(TID_GAME_CHARACTTER_PVP0);//"PVP";
#else // __VER >= 8 // __S8_PK
	tabTabItem.pszText = prj.GetText(TID_APP_PVP);//"PVP";
#endif // __VER >= 8 // __S8_PK
	tabTabItem.pWndBase = &m_wndPvp;

	lpTapCtrl->InsertItem( 2, &tabTabItem );
	//lpTapCtrl->InsertTexture( 2 ,MakePath( DIR_THEME, "TabCharacter3.bmp" ) );

	lpTapCtrl->SetCurSel( 1 );

	lpTapCtrl->SetButtonLength( 55 );
#endif // __RENEW_CHARINFO
}
BOOL CWndCharacter::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	//CRect rect( 240, 0, 240 + 260, 255 - 105 + 20 ); 
	CRect rect( 0, 90, WND_WIDTH, 260 );//255 - 105 + 20 ); 
	//CRect rect( 50, 50, 50+210, 50+300 );
	//SetTitle( GETTEXT( TID_APP_CHARACTER )  );
	//return CWndNeuz::Create(WBS_MOVE|WBS_SOUND|WBS_CAPTION|WBS_EXTENSION,rect,pWndParent,dwWndId);
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint(0, 0), pWndParent );
}
BOOL CWndCharacter::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{ 
	if( message == WNM_CLICKED )
	{
	/*f( nID == WTBID_EXTENSION )
		{
			CRect rect = GetWindowRect( TRUE ); 
			if( m_wndTitleBar.GetTitleBarButton( WTBID_EXTENSION )->GetCheck() )
				rect.bottom += 135ww
			else
				rect.bottom -= 135;
			SetWndRect( rect );
		}
		*/
	}	
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
BOOL CWndCharacter::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand(nID,dwMessage,pWndBase);
}
void CWndCharacter::OnSize(UINT nType, int cx, int cy)
{
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize( 70, 25 );

	CRect rect1_1( x - ( size.cx / 2), y, ( x - ( size.cx / 2 ) ) + size.cx, y + size.cy );

	CRect rect2_1( x - size.cx - 10, y, ( x - size.cx - 10 ) + size.cx, y + size.cy );
	CRect rect2_2( x + 10          , y, ( x + 10           ) + size.cx, y + size.cy );

	CRect rect3_1( x - ( size.cx / 2) - size.cx - 10, y, (x - ( size.cx / 2) - size.cx - 10) + size.cx, y + size.cy );
	CRect rect3_2( x - ( size.cx / 2)               , y, (x - ( size.cx / 2)               ) + size.cx, y + size.cy );
	CRect rect3_3( x + ( size.cx / 2) + 10          , y, (x + ( size.cx / 2) + 10          ) + size.cx, y + size.cy );

	CWndNeuz::OnSize(nType,cx,cy);
}
void CWndCharacter::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndCharacter::OnLButtonDown(UINT nFlags, CPoint point)
{
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

CWndStateConfirm::CWndStateConfirm() 
{ 
	m_nId = 0;
} 
CWndStateConfirm::~CWndStateConfirm() 
{ 
} 
void CWndStateConfirm::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndStateConfirm::OnSetState( UINT nId )
{
	m_nId = nId;
}
void CWndStateConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
//	strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_SHELLCOST)
	CString strMessage;

#if __VER >= 9 // __CSC_VER9_2
	strMessage.Format( prj.GetText( TID_GAME_CHARSTATUS_APPLY_Q ) );
#else //__CSC_VER9_2
	switch( m_nId )
	{
	case 100:	// str
		strMessage.Format( prj.GetText( TID_DIAG_0076 ) );
		break;
	case 101:	// sta
		strMessage.Format( prj.GetText( TID_DIAG_0077 ) );
		break;
	case 102:	// dex
		strMessage.Format( prj.GetText( TID_DIAG_0078 ) );
		break;
	case 103:	// int
		strMessage.Format( prj.GetText( TID_DIAG_0079 ) );
		break;
	default:
		break;
	}
#endif //__CSC_VER9_2

	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );

	// 1차 전직인지 확인
	if( g_pPlayer->IsBaseJob() == TRUE )
	{
		// 방랑자
		CString strAddMessage;
		strAddMessage.Format( prj.GetText( TID_DIAG_0082 ) );
		strMessage += '\n';
		strMessage += strAddMessage;
	}
	pWndText->SetString( strMessage );
	pWndText->EnableWindow( FALSE );

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndStateConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_STATE_CONFIRM, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndStateConfirm::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndStateConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndStateConfirm::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndStateConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndStateConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndStateConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
#if __VER >= 13 // __RENEW_CHARINFO
	if( nID == WIDC_YES )
	{
		CWndCharacter* pWndBase = (CWndCharacter*)g_WndMng.GetWndBase( APP_CHARACTER3 );
		if( pWndBase )
		{
			CWndCharInfo* pInfo = &pWndBase->m_wndCharInfo;
			if( pInfo )
			{
				g_DPlay.SendModifyStatus(pInfo->m_nStrCount, pInfo->m_nStaCount, pInfo->m_nDexCount, pInfo->m_nIntCount);
				pInfo->m_nStrCount = pInfo->m_nStaCount = pInfo->m_nDexCount = pInfo->m_nIntCount = 0;
				pInfo->RefreshStatPoint();
				Destroy();
			}
		}
	}
	else if( nID == WIDC_NO || nID == WTBID_CLOSE )
	{
		CWndBase* pWndBase	= g_WndMng.GetWndBase( APP_CHARACTER3 );
		if( pWndBase ) 
		{
			( (CWndCharacter*)pWndBase )->m_wndCharInfo.m_fWaitingConfirm	= FALSE;
		}
		Destroy();
	}
#else // __RENEW_CHARINFO
#if __VER >= 9 // __CSC_VER9_2 
	if( nID == WIDC_YES )
	{
		CWndCharacter* pWndBase = (CWndCharacter*)g_WndMng.GetWndBase( APP_CHARACTER2 );
		if( pWndBase )
		{
			CWndCharacterDetail2* pDetail = &pWndBase->m_wndStateDetail;
			if( pDetail )
			{
				g_DPlay.SendModifyStatus(pDetail->m_nStrCount, pDetail->m_nStaCount, pDetail->m_nDexCount, pDetail->m_nIntCount);
				pDetail->m_nStrCount = pDetail->m_nStaCount = pDetail->m_nDexCount = pDetail->m_nIntCount = 0;
				pDetail->RefreshStatPoint();
			}
		}
	}

	Destroy();
#else //__CSC_VER9_2
	if( nID == WIDC_YES )
	{
		SendYes();
	}
	else if( nID == WIDC_NO || nID == WTBID_CLOSE )
	{
#if __VER >= 9 // __CSC_VER9_2
		CWndBase* pWndBase	= g_WndMng.GetWndBase( APP_CHARACTER2 );
#else //__CSC_VER9_2
		CWndBase* pWndBase	= g_WndMng.GetWndBase( APP_CHARACTER );
#endif //__CSC_VER9_2
		if( pWndBase ) {
			( (CWndCharacter*)pWndBase )->m_wndStateDetail.m_fWaitingConfirm	= FALSE;
		}
		Destroy();
	}
#endif //__CSC_VER9_2
#endif // __RENEW_CHARINFO
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndStateConfirm::SendYes( void )
{
	g_DPlay.SendIncStatLevel( (CHAR)m_nId );
	Destroy();
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


CWndCharacterBase::CWndCharacterBase()
{
}
CWndCharacterBase::~CWndCharacterBase()
{
}
void CWndCharacterBase::OnDraw(C2DRender* p2DRender)
{
	CRect rect = GetClientRect();
	rect.bottom -= 30;
	int y = 0, nNext = 15;

/************************ADEILSONV19**************************/
	DWORD dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255);//agora
	//DWORD dwColor = D3DCOLOR_ARGB(255,0,0,0);//antes
/*************************************************************/

#if __VER >= 10 // __CSC_VER9_1
	BYTE checkhero = g_pPlayer->GetLegendChar();
	int xpos = 0;
	int ypos = 0;
	CTexture* pTexture;
	CString strPath;
	CPoint point;
#endif //__CSC_VER9_1

//	TCHAR szString[ 32 ];
//	p2DRender->TextOut( 5, y, _T( "Name"  ) ); y += nNext;
	//p2DRender->TextOut( 5, y, _T( "Style" ) ); y += nNext;
//	p2DRender->TextOut( 5, y, _T( "Job"   ) ); y += nNext;
	//p2DRender->TextOut( 5, y, _T( "Guild" ) ); y += nNext;
//	p2DRender->TextOut( 5, y, _T( "Level" ) ); y += nNext;
	//p2DRender->TextOut( 5, y, _T( "Exp1." ) ); y += nNext;
	//p2DRender->TextOut( 5, y, _T( "Exp2." ) ); y += nNext;
	//p2DRender->TextOut( 5, y, _T( "Gold"  ) ); y += nNext;

	y = 10;

	if( ::GetLanguage() == LANG_JAP )
	{
		p2DRender->TextOut( 80, y, g_pPlayer->GetName()       , dwColor); y += nNext;
		p2DRender->TextOut( 80, y, g_pPlayer->GetJobString()  , dwColor ); y += nNext;
#if __VER >= 10 // __CSC_VER9_1
		ypos = y;
		if(checkhero == LEGEND_CLASS_MASTER)
		{
			if(g_pPlayer->GetLevel() < 100)
				xpos = 97;
			else
				xpos = 103;
		}
#if __VER < 15 // __HERO129_VER15				// 15차 히어로 레벨확장
		if(checkhero == LEGEND_CLASS_HERO)
			xpos = 80;
		else
	#endif	// 15차 히어로 레벨확장		
			p2DRender->TextOut( 80, y, g_pPlayer->GetLevel()      , dwColor ); y += nNext;
#else
		p2DRender->TextOut( 80, y, g_pPlayer->GetLevel()      , dwColor ); y += nNext;
#endif //__CSC_VER9_1

		y = 81;
	}
	else
	{
		p2DRender->TextOut( 50, y, g_pPlayer->GetName()       , dwColor); y += nNext;
		p2DRender->TextOut( 50, y, g_pPlayer->GetJobString()  , dwColor ); y += nNext;
#if __VER >= 10 // __CSC_VER9_1
		ypos = y;
		if(checkhero == LEGEND_CLASS_MASTER)
		{
			if(g_pPlayer->GetLevel() < 100)
				xpos = 67;
			else
				xpos = 73;
		}
#if __VER < 15 // __HERO129_VER15				// 15차 히어로 레벨확장
		if(checkhero == LEGEND_CLASS_HERO)
			xpos = 50;
		else
	#endif	// 15차 히어로 레벨확장	
			p2DRender->TextOut( 50, y, g_pPlayer->GetLevel()      , dwColor ); y += nNext;
#else
		p2DRender->TextOut( 50, y, g_pPlayer->GetLevel()      , dwColor ); y += nNext;
#endif //__CSC_VER9_1
		y = 81;
	}

#if __VER >= 10 // __CSC_VER9_1
	point.x = xpos;
	point.y = ypos - 2;
	if(checkhero == LEGEND_CLASS_MASTER) //전승을 했을 경우.
	{
		if(/*g_pPlayer->m_nLevel >= 60 && */g_pPlayer->m_nLevel < 70) //Level Down될 경우를 생각해서 주석처리
			strPath = MakePath( DIR_ICON, "Icon_MasterMark1.dds");
		else if(g_pPlayer->m_nLevel >= 70 && g_pPlayer->m_nLevel < 80)
			strPath = MakePath( DIR_ICON, "Icon_MasterMark2.dds");
		else if(g_pPlayer->m_nLevel >= 80 && g_pPlayer->m_nLevel < 90)
			strPath = MakePath( DIR_ICON, "Icon_MasterMark3.dds");
		else if(g_pPlayer->m_nLevel >= 90 && g_pPlayer->m_nLevel < 100)
			strPath = MakePath( DIR_ICON, "Icon_MasterMark4.dds");
		else if(g_pPlayer->m_nLevel >= 100 && g_pPlayer->m_nLevel < 110)
			strPath = MakePath( DIR_ICON, "Icon_MasterMark5.dds");
		else if(g_pPlayer->m_nLevel >= 110 && g_pPlayer->m_nLevel <= 120)
			strPath = MakePath( DIR_ICON, "Icon_MasterMark6.dds");

		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, strPath, 0xffff00ff );
		if(pTexture != NULL)
			pTexture->Render( p2DRender, point );			
		
	}
	else if(checkhero == LEGEND_CLASS_HERO) //영웅일 경우.
	{
#if __VER >= 15 // __HERO129_VER15				// 15차 히어로 레벨확장
		strPath = MakePath( DIR_ICON, "Icon_Hero.dds");
	#else	// 15차 히어로 레벨확장
		strPath = MakePath( DIR_ICON, "Icon_HeroMark.dds");
	#endif	// 15차 히어로 레벨확장
		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, strPath, 0xffff00ff );
		if(pTexture != NULL)
			pTexture->Render( p2DRender, point );
	}
#endif //__CSC_VER9_1
	//서버 정보
	y = 55;
	CString strServerName;
	strServerName.Format( "%s", g_dpCertified.GetServerName(g_Option.m_nSer) );
	if( ::GetLanguage() == LANG_JAP )
	{
		p2DRender->TextOut( 80, y, strServerName, dwColor );
		y += nNext;
	}
	else
	{
		p2DRender->TextOut( 50, y, strServerName, dwColor );
		y += nNext;
	}

	//채널 정보
	LPSERVER_DESC pServerDesc = NULL;
	int nCount = 0;
	for( int j = 0; j < (int)( g_dpCertified.m_dwSizeofServerset ); j++ )
	{
		if(g_dpCertified.m_aServerset[j].dwParent == NULL_ID)
		{
			if(nCount++ == g_Option.m_nSer)
				pServerDesc = g_dpCertified.m_aServerset + j;
		}
		if(g_dpCertified.m_aServerset[j].dwParent != NULL_ID && g_dpCertified.m_aServerset[j].lEnable != 0L)
		{
			if(pServerDesc != NULL && g_dpCertified.m_aServerset[j].dwParent == pServerDesc->dwID)
			{
				strServerName.Format( "%s",  g_dpCertified.m_aServerset[j+g_Option.m_nMSer].lpName );
				if( ::GetLanguage() == LANG_JAP )
					p2DRender->TextOut( 80, y, strServerName, dwColor );
				else
					p2DRender->TextOut( 50, y, strServerName, dwColor );

				j = g_dpCertified.m_dwSizeofServerset;
			}
		}
	}

	y = 96;
	
	p2DRender->TextOut( 96, y, g_pPlayer->GetFlightLv(), dwColor ); y += nNext;
	{
		char szBuff[64];
		int nMaxFxp = prj.m_aFxpCharacter[ g_pPlayer->GetFlightLv() + 1 ].dwFxp;
		sprintf( szBuff, "%5.2f%%", (float)g_pPlayer->GetFxp() * 100.0f / (float)nMaxFxp );
		p2DRender->TextOut( 90, y, szBuff, dwColor ); y += nNext;
	}
	
	y = 10;
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
	dwColor = D3DCOLOR_ARGB( 255, 238, 221, 130 );//agora
	//dwColor = D3DCOLOR_ARGB(255, 0, 0, 230);//antes
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_01), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_APP_CHARACTER_JOB), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_02), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHAR_SERVER), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHAR_SERVERNAME), dwColor ); y += nNext;
	
	y += 10;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_04), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_05), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_06), dwColor );	
/***********************************************************************************/
}
void CWndCharacterBase::OnInitialUpdate()
{
	CWndBase::OnInitialUpdate();

#if __VER >= 9 // __CSC_VER9_2
	SetTexture( m_pApp->m_pd3dDevice, MakePath( "Theme\\", ::GetLanguage(), _T( "WndCharacter2_1.tga" ) ), TRUE );
#else //__CSC_VER9_2
	SetTexture( m_pApp->m_pd3dDevice, MakePath( "Theme\\", ::GetLanguage(), _T( "wndCharacter1.tga" ) ), TRUE );
#endif //__CSC_VER9_2
	FitTextureSize();
	
	MakeVertexBuffer();
}

BOOL CWndCharacterBase::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 240, 0, 240 + 330, 255 - 135 );
	//SetTitle( GETTEXT( TID_APP_CHARACTER )  );
	return CWndBase::Create(WBS_THICKFRAME|WBS_MOVE|WBS_SOUND|WBS_CAPTION|WBS_EXTENSION,rect,pWndParent,dwWndId);
}
BOOL CWndCharacterBase::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{ 
	return CWndBase::OnChildNotify( message, nID, pLResult );
}
BOOL CWndCharacterBase::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndBase::OnCommand(nID,dwMessage,pWndBase);
}
void CWndCharacterBase::OnSize(UINT nType, int cx, int cy)
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
void CWndCharacterBase::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndCharacterBase::OnLButtonDown(UINT nFlags, CPoint point)
{
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

CWndCharacterDetail::CWndCharacterDetail()
{
	if( !g_WndMng.m_pWndStateConfirm )
		m_fWaitingConfirm	= FALSE;

	m_nCnt = 0;
}
CWndCharacterDetail::~CWndCharacterDetail()
{
}

void CWndCharacterDetail::RenderATK( C2DRender* p2DRender, int x, int y )
{
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
	DWORD dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );//agora
	//DWORD dwColor = D3DCOLOR_ARGB( 255, 0, 0, 0 )/*adeilson*/;//antes
/***********************************************************************************/
	char szText[32];
	int nMin, nMax;
	g_pPlayer->GetHitMinMax( &nMin, &nMax );
	int nATK = ( nMin + nMax ) / 2;

	if( g_pPlayer->IsSMMode( SM_ATTACK_UP1 ) || g_pPlayer->IsSMMode( SM_ATTACK_UP ) )
		nATK	= (int)( nATK * 1.2f );
	
	if( 0 < g_pPlayer->GetParam( DST_ATKPOWER_RATE, 0 ) )
		nATK = nATK + ( nATK * g_pPlayer->GetParam( DST_ATKPOWER_RATE, 0 ) / 100 );

	nATK += g_pPlayer->GetWeaponPlusDamage( 1, FALSE );

//#ifdef __PET_0410
	nATK	+= g_pPlayer->GetParam( DST_ATKPOWER, 0 );
//#endif	// __PET_0410

#ifdef __JEFF_11
	if( nATK < 0 )
		nATK	= 0;
#endif	// __JEFF_11

	wsprintf( szText, "%d", nATK );

	p2DRender->TextOut( x, y, szText, dwColor ); 
}

void CWndCharacterDetail::OnDraw(C2DRender* p2DRender)
{
	int y = 5, nNext = 15;
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
	//DWORD dwColor = D3DCOLOR_ARGB( 255, 0, 0, 0 )/*adeilson*/;//antes
	DWORD dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );//agora
/***********************************************************************************/
	y = 15;
	int x = 5, nNextX = 100;

	x = 50; y = 10;
	RenderATK( p2DRender, x, y );		// 공격력
	y += nNext;

	p2DRender->TextOut( x , y, g_pPlayer->GetShowDefense( FALSE ), dwColor ); y += nNext; //방어력
	x = 100; y = 5;

	x = 130; y = 10;
	CString strMsg;
	strMsg.Format( "%d%%", g_pPlayer->GetCriticalProb() );
	p2DRender->TextOut( x , y, strMsg, dwColor ); y += nNext;		// 크리티컬
	
	float fAttackSpeed = g_pPlayer->GetAttackSpeed();
	strMsg.Format( "%d%%", int( fAttackSpeed*100.0f ) /2 );
	p2DRender->TextOut( x , y, strMsg, dwColor ); y += nNext;

#ifdef _KWCSC_UPDATE
	x = 50;
	int fHitRate = g_pPlayer->GetAdjHitRate();
	strMsg.Format( "%d%%", fHitRate);
	p2DRender->TextOut( x , y, strMsg, dwColor );
#endif
	x =15; y = 40; nNextX = 60;
	// 아래부터 능력치 관련 
#ifdef _KWCSC_UPDATE
	y = 67;
#else
	y = 52;
#endif
	if( g_pPlayer->m_nStr == g_pPlayer->GetStr() )
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
	//dwColor = D3DCOLOR_ARGB( 255, 0, 0, 0 )/*adeilson*/;antes
	    dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );//agora
/***********************************************************************************/
	else
	if( g_pPlayer->m_nStr < g_pPlayer->GetStr() )
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
	//	dwColor = D3DCOLOR_ARGB( 255, 0, 0, 255 );antes
	    dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );//agora
/***********************************************************************************/
	else
		dwColor = D3DCOLOR_ARGB( 255, 255, 0, 0 );

	p2DRender->TextOut( 65, y, g_pPlayer->GetStr(), dwColor ); y += nNext;

	if( g_pPlayer->m_nSta == g_pPlayer->GetSta() )
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
	//	dwColor = D3DCOLOR_ARGB( 255, 0, 0, 0 )/*adeilson*/;antes
	    dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );//agora
/***********************************************************************************/
	else
	if( g_pPlayer->m_nSta < g_pPlayer->GetSta() )
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
	//	dwColor = D3DCOLOR_ARGB( 255, 0, 0, 255 );antes
	    dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );//agora
/***********************************************************************************/

	else
		dwColor = D3DCOLOR_ARGB( 255, 255, 0, 0 );

	p2DRender->TextOut( 65, y, g_pPlayer->GetSta(), dwColor ); y += nNext;

	if( g_pPlayer->m_nDex == g_pPlayer->GetDex() )
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
	//	dwColor = D3DCOLOR_ARGB( 255, 0, 0, 0 )/*adeilson*/;antes
	    dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );//agora
/***********************************************************************************/
    else
	if( g_pPlayer->m_nDex < g_pPlayer->GetDex() )
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
	//	dwColor = D3DCOLOR_ARGB( 255, 0, 0, 255 );antes
	    dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );//agora
/***********************************************************************************/
	else
		dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	p2DRender->TextOut( 65, y, g_pPlayer->GetDex(), dwColor ); y += nNext;

	if( g_pPlayer->m_nInt == g_pPlayer->GetInt() )
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
	//	dwColor = D3DCOLOR_ARGB( 255, 0, 0, 0 )/*adeilson*/;antes
	    dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );//agora
/***********************************************************************************/

	else
	if( g_pPlayer->m_nInt < g_pPlayer->GetInt() )
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
	//	dwColor = D3DCOLOR_ARGB( 255, 0, 0, 255 );antes
	    dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );//agora
/***********************************************************************************/

	else
		dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	p2DRender->TextOut( 65, y, g_pPlayer->GetInt(), dwColor ); y += nNext;

#ifdef __S_ADD_RESTATE
	if( m_fWaitingConfirm  == FALSE )
	{
		m_wndStr.EnableWindow( ( (int)g_pPlayer->GetRemainGP() > 0 ) ? TRUE : FALSE );
		m_wndSta.EnableWindow( ( (int)g_pPlayer->GetRemainGP() > 0 ) ? TRUE : FALSE );
		m_wndDex.EnableWindow( ( (int)g_pPlayer->GetRemainGP() > 0 ) ? TRUE : FALSE );
		m_wndInt.EnableWindow( ( (int)g_pPlayer->GetRemainGP() > 0 ) ? TRUE : FALSE );
	}
#else // __S_ADD_RESTATE
	LPJOBABILITY pJobAbillity;
	if( g_pPlayer->IsExpert() )
		pJobAbillity = &prj.m_aJobAbillity[ g_pPlayer->GetJob() ];
	else
		pJobAbillity = &prj.m_aJobAbillity[ JOB_VAGRANT ];

	if( m_fWaitingConfirm  == FALSE )
	{
		m_wndStr.EnableWindow( ( (int)g_pPlayer->GetRemainGP() >= (int)pJobAbillity->dwStr ) ? TRUE : FALSE );
		m_wndSta.EnableWindow( ( (int)g_pPlayer->GetRemainGP() >= (int)pJobAbillity->dwSta ) ? TRUE : FALSE );
		m_wndDex.EnableWindow( ( (int)g_pPlayer->GetRemainGP() >= (int)pJobAbillity->dwDex ) ? TRUE : FALSE );
		m_wndInt.EnableWindow( ( (int)g_pPlayer->GetRemainGP() >= (int)pJobAbillity->dwInt ) ? TRUE : FALSE );
	}
#endif // __S_ADD_RESTATE
	else
	{
		m_wndStr.EnableWindow( FALSE );
		m_wndSta.EnableWindow( FALSE );
		m_wndDex.EnableWindow( FALSE );
		m_wndInt.EnableWindow( FALSE );
	}
#ifdef _KWCSC_UPDATE
	y = 67;
#else
	y = 52;
#endif
#ifdef __S_ADD_RESTATE
	p2DRender->TextOut( 135, y, 1, dwColor ); y += nNext;
	p2DRender->TextOut( 135, y, 1, dwColor ); y += nNext;
	p2DRender->TextOut( 135, y, 1, dwColor ); y += nNext;
	p2DRender->TextOut( 135, y, 1, dwColor ); y += nNext;
#else // __S_ADD_RESTATE
	p2DRender->TextOut( 135, y, pJobAbillity->dwStr, dwColor ); y += nNext;
	p2DRender->TextOut( 135, y, pJobAbillity->dwSta, dwColor ); y += nNext;
	p2DRender->TextOut( 135, y, pJobAbillity->dwDex, dwColor ); y += nNext;
	p2DRender->TextOut( 135, y, pJobAbillity->dwInt, dwColor ); y += nNext;
#endif // __S_ADD_RESTATE

	//
	// m_nCnt같은 글로벌성 카운터는 g_nRenderCnt나 g_nProcessCnt를 쓰면 됩니다.
	// g_nProcessCnt는 매 프로세스(1/60)마다 한번씩 올라가는 카운트고
	// g_nRenderCnt는 매 렌더화면마다 한번씩 카운팅 되는 값입니다.  -xuzhu-
	// 
	if( g_pPlayer->GetRemainGP() )
	{
		if( (g_nRenderCnt / 8) & 1 )	
		{
			dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
			m_nCnt = 0;
		}
	}

	p2DRender->TextOut( 105, y, g_pPlayer->GetRemainGP(), dwColor ); y += nNext;

	CRect rectHittest[5];
#ifdef _KWCSC_UPDATE
	rectHittest[0].SetRect( 10, 67, 160, 65 );
	rectHittest[1].SetRect( 10, 82, 160, 80 );
	rectHittest[2].SetRect( 10, 97, 160, 95 );
	rectHittest[3].SetRect( 10, 112, 160, 110 );
	rectHittest[4].SetRect( 10, 127, 160, 125 );
#else
	rectHittest[0].SetRect( 10, 52, 160, 65 );
	rectHittest[1].SetRect( 10, 67, 160, 80 );
	rectHittest[2].SetRect( 10, 82, 160, 95 );
	rectHittest[3].SetRect( 10, 97, 160, 110 );
	rectHittest[4].SetRect( 10, 112, 160, 125 );
#endif
	CRect rectTemp;
	CPoint ptTemp;
	// 툴팁 나오게 하기( Str, Sta, Dex, Int, GP )
	CPoint ptMouse = GetMousePoint();
	for( int iC = 0 ; iC < 5 ; ++iC )
	{
		if( rectHittest[iC].PtInRect( ptMouse ) )
		{
			ClientToScreen( &ptMouse );
			ClientToScreen( &rectHittest[iC] );
			CEditString strEdit;
			CString szString;
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
			//DWORD dwColorName = D3DCOLOR_ARGB( 255, 0, 0, 0 )/*adeilson*/;antes
			DWORD dwColorName = D3DCOLOR_ARGB( 255, 238, 221, 130 );//agora
			DWORD dwColorCommand = D3DCOLOR_ARGB( 255, 180, 0, 0 );
/***********************************************************************************/

			if( iC == 0 )	// STR
			{
				szString.Format( "%s", prj.GetText( TID_TOOLTIP_STR ) );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				szString.Format( "  %d\n ", g_pPlayer->GetStr() );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_STR0 ) );
				strEdit.AddString( "(" );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_STR1 ), dwColorCommand );
				strEdit.AddString( ")" );
			}
			else if( iC == 1 ) // STA
			{
				szString.Format( "%s", prj.GetText( TID_TOOLTIP_STA ) );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				szString.Format( "  %d\n ", g_pPlayer->GetSta() );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_STA0 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_STA1 ) );
			}
			else if( iC == 2 ) // DEX
			{
				szString.Format( "%s", prj.GetText( TID_TOOLTIP_DEX ) );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				szString.Format( "  %d\n ", g_pPlayer->GetDex() );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_DEX0 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_DEX1 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_DEX2 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_DEX3 ), dwColorCommand );
			}
			else if( iC == 3 ) // INT
			{
				szString.Format( "%s", prj.GetText( TID_TOOLTIP_INT ) );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				szString.Format( "  %d\n ", g_pPlayer->GetInt() );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_INT0 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_INT1 ) );
			}
			else	// GP
			{
				szString.Format( "GP  %d\n ", g_pPlayer->GetRemainGP() );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_GP0 ) );
			}
			
			g_toolTip.PutToolTip( 100, strEdit, rectHittest[iC], ptMouse, 3 );
			break;
		}
	}

	y = 10;
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
	dwColor = D3DCOLOR_ARGB( 255, 238, 221, 130 );//agora
	//	dwColor = D3DCOLOR_ARGB( 255, 0, 0, 180 );//antes
/***********************************************************************************/

	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_13), dwColor ); 
	p2DRender->TextOut( 75, y, prj.GetText(TID_GAME_CHARACTER_14), dwColor ); y += nNext;

	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_DEFENCE), dwColor ); 
	p2DRender->TextOut( 75, y, prj.GetText(TID_GAME_CHARACTER_15), dwColor ); y += nNext;
#ifdef _KWCSC_UPDATE
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_HITRATE), dwColor ); y += nNext;
#endif
	y += 12;
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_STR), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_STA), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_DEX), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_INT), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_07), dwColor ); y += nNext;	
}
void CWndCharacterDetail::OnInitialUpdate()
{
	CWndBase::OnInitialUpdate();

	SetTexture( m_pApp->m_pd3dDevice, MakePath( "Theme\\", ::GetLanguage(), _T( "wndCharacter2.tga" ) ), TRUE );
	FitTextureSize();

	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize( 70, 25);

	CRect rect1_1( x - ( size.cx / 2), y, ( x - ( size.cx / 2 ) ) + size.cx, y + size.cy );

	CRect rect2_1( x - size.cx - 10, y, ( x - size.cx - 10 ) + size.cx, y + size.cy );
	CRect rect2_2( x + 10          , y, ( x + 10           ) + size.cx, y + size.cy );

	CRect rect3_1( x - ( size.cx / 2) - size.cx - 10, y, (x - ( size.cx / 2) - size.cx - 10) + size.cx, y + size.cy );
	CRect rect3_2( x - ( size.cx / 2)               , y, (x - ( size.cx / 2)               ) + size.cx, y + size.cy );
	CRect rect3_3( x + ( size.cx / 2) + 10          , y, (x + ( size.cx / 2) + 10          ) + size.cx, y + size.cy );


	// 아래부터 능력치 관련 
#ifdef _KWCSC_UPDATE
	y = 67;
#else
	y = 52;
#endif
	m_wndStr.Create  ( "", 0, CRect( 100,   y, 145 + 20, y + 16 ), this, 100 ); y += 14;
	m_wndSta.Create  ( "", 0, CRect( 100,   y, 145 + 20, y + 16 ), this, 101 ); y += 14;
	m_wndDex.Create  ( "", 0, CRect( 100,   y, 145 + 20, y + 16 ), this, 102 ); y += 14;
	m_wndInt.Create  ( "", 0, CRect( 100,   y, 145 + 20, y + 16 ), this, 103 ); y += 14;

	m_wndStr.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndSta.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndDex.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndInt.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharPlus.bmp" ) ), TRUE );

#ifdef __S_ADD_RESTATE
	m_wndStr.EnableWindow( ( g_pPlayer->GetRemainGP() > 0 ) ? TRUE : FALSE );
	m_wndSta.EnableWindow( ( g_pPlayer->GetRemainGP() > 0 ) ? TRUE : FALSE );
	m_wndDex.EnableWindow( ( g_pPlayer->GetRemainGP() > 0 ) ? TRUE : FALSE );
	m_wndInt.EnableWindow( ( g_pPlayer->GetRemainGP() > 0 ) ? TRUE : FALSE );
#else // __S_ADD_RESTATE
	LPJOBABILITY pJobAbillity;
	if( g_pPlayer->IsExpert() )
		pJobAbillity = &prj.m_aJobAbillity[ g_pPlayer->GetJob() ];
	else
		pJobAbillity = &prj.m_aJobAbillity[ JOB_VAGRANT ];
	m_wndStr.EnableWindow( ( g_pPlayer->GetRemainGP() >= pJobAbillity->dwStr ) ? TRUE : FALSE );
	m_wndSta.EnableWindow( ( g_pPlayer->GetRemainGP() >= pJobAbillity->dwSta ) ? TRUE : FALSE );
	m_wndDex.EnableWindow( ( g_pPlayer->GetRemainGP() >= pJobAbillity->dwDex ) ? TRUE : FALSE );
	m_wndInt.EnableWindow( ( g_pPlayer->GetRemainGP() >= pJobAbillity->dwInt ) ? TRUE : FALSE );
#endif // __S_ADD_RESTATE
}
BOOL CWndCharacterDetail::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 240, 0, 240 + 330, 255 - 135 ); 
	//SetTitle( GETTEXT( TID_APP_CHARACTER )  );
	return CWndBase::Create(WBS_THICKFRAME|WBS_MOVE|WBS_SOUND|WBS_CAPTION|WBS_EXTENSION,rect,pWndParent,dwWndId);
}
BOOL CWndCharacterDetail::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{ 
	if( message == WNM_CLICKED )		
	{
		if( m_fWaitingConfirm == FALSE ) {
			m_fWaitingConfirm	= TRUE;
			SAFE_DELETE( g_WndMng.m_pWndStateConfirm );
			g_WndMng.m_pWndStateConfirm = new CWndStateConfirm;
			g_WndMng.m_pWndStateConfirm->OnSetState( nID );
			g_WndMng.m_pWndStateConfirm->Initialize();
		}
	}
	return CWndBase::OnChildNotify( message, nID, pLResult );
}
BOOL CWndCharacterDetail::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndBase::OnCommand(nID,dwMessage,pWndBase);
}
void CWndCharacterDetail::OnSize(UINT nType, int cx, int cy)
{
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize( 70, 25 );

	CRect rect1_1( x - ( size.cx / 2), y, ( x - ( size.cx / 2 ) ) + size.cx, y + size.cy );

	CRect rect2_1( x - size.cx - 10, y, ( x - size.cx - 10 ) + size.cx, y + size.cy );
	CRect rect2_2( x + 10          , y, ( x + 10           ) + size.cx, y + size.cy );

	CRect rect3_1( x - ( size.cx / 2) - size.cx - 10, y, (x - ( size.cx / 2) - size.cx - 10) + size.cx, y + size.cy );
	CRect rect3_2( x - ( size.cx / 2)               , y, (x - ( size.cx / 2)               ) + size.cx, y + size.cy );
	CRect rect3_3( x + ( size.cx / 2) + 10          , y, (x + ( size.cx / 2) + 10          ) + size.cx, y + size.cy );

	CWndBase::OnSize(nType,cx,cy);
}
void CWndCharacterDetail::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(IsWndRoot())
		return;
}
void CWndCharacterDetail::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(IsWndRoot())
		return;
}
void CWndCharacterDetail::OnMouseWndSurface( CPoint point )
{
	// 직업 아이콘 위치에 커서를 가져가면 접속한 서버의 정보(서버이름과 멀티서버 번호)를 보여준다.
/*	CRect rectHittest( 10, 52, 100, 64 );	
	if( rectHittest.PtInRect( point ) )
	{
		CPoint point2 = point;
		ClientToScreen( &point2 );
		ClientToScreen( &rectHittest );
		CString szEditString;
		szEditString.Format( "%d, %d", point.x, point.y );
		g_toolTip.PutToolTip( 100, szEditString, rectHittest, point2 );
		g_WndMng.PutString( szEditString );
//		g_toolTip.PutToolTip( (DWORD)this, m_strToolTip, rectHittest, point2, 0 );
	}
*/}
	
//////////////////////////////////////////////////////////////////////////
// CWndCharacterDetail2 Class
//////////////////////////////////////////////////////////////////////////
	
#if __VER >= 9 // __CSC_VER9_2
	
CWndCharacterDetail2::CWndCharacterDetail2()
{
	m_nStrCount = 0;
	m_nStaCount = 0;
	m_nDexCount = 0;
	m_nIntCount = 0;
	m_nGpPoint = 0;

	m_nATK = 0;
	m_nDEF = 0;
	m_nCritical = 0;
	m_nATKSpeed = 0;
}

CWndCharacterDetail2::~CWndCharacterDetail2()
{
}

void CWndCharacterDetail2::RenderATK( C2DRender* p2DRender, int x, int y )
{
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
	DWORD dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255);//agora
	//DWORD dwColor = D3DCOLOR_ARGB(255,0,0,0);//antes
/***********************************************************************************/

	char szText[32];
	int nMin, nMax;
	int nTemp1, nTemp2;

	GetVirtualATK(&nTemp1, &nTemp2);
	g_pPlayer->GetHitMinMax( &nMin, &nMax );

	if(	(m_nStrCount != 0 || m_nDexCount != 0 || m_nIntCount != 0) && (nTemp1 != nMin || nTemp2 != nMax) ) //임의 스탯이 변경되고 현재 능력치랑 다를 경우
	{
		if( (g_nRenderCnt / 8) & 1 )	
		{
			dwColor = D3DCOLOR_ARGB(255,255,0,0);
		}

		GetVirtualATK(&nMin, &nMax);
	}
	else
		g_pPlayer->GetHitMinMax( &nMin, &nMax );
	
	int nATK = ( nMin + nMax ) / 2;

	if( g_pPlayer->IsSMMode( SM_ATTACK_UP1 ) || g_pPlayer->IsSMMode( SM_ATTACK_UP ) )
		nATK	= (int)( nATK * 1.2f );
	
	if( 0 < g_pPlayer->GetParam( DST_ATKPOWER_RATE, 0 ) )
		nATK = nATK + ( nATK * g_pPlayer->GetParam( DST_ATKPOWER_RATE, 0 ) / 100 );

	nATK += g_pPlayer->GetWeaponPlusDamage( 1, FALSE );
	nATK += g_pPlayer->GetParam( DST_ATKPOWER, 0 );

#ifdef __JEFF_11
	if( nATK < 0 )
		nATK	= 0;
#endif	// __JEFF_11

	wsprintf( szText, "%d", nATK );

	p2DRender->TextOut( x, y, szText, dwColor ); 
}

void CWndCharacterDetail2::OnDraw(C2DRender* p2DRender)
{
	if(g_pPlayer == NULL)
		return;

	int y = 5, nNext = 15;
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
	DWORD dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255);//agora
	//DWORD dwColor = D3DCOLOR_ARGB(255,0,0,0);//antes
/***********************************************************************************/

	y = 15;
	int x = 5, nNextX = 100;

	x = 50; y = 10;
	// 공격력
	RenderATK( p2DRender, x, y );
	y += nNext;

	//방어력
	if(	m_nStaCount != 0 && GetVirtualDEF() != g_pPlayer->GetShowDefense( FALSE ) ) //임의 스탯이 변경되고 현재 능력치랑 다를 경우
	{
		if( (g_nRenderCnt / 8) & 1 )	
		{
			dwColor = D3DCOLOR_ARGB(255,255,0,0);
		}
		p2DRender->TextOut( x , y, GetVirtualDEF(), dwColor ); y += nNext;
	}
	else
		p2DRender->TextOut( x , y, g_pPlayer->GetShowDefense( FALSE ), dwColor ); y += nNext;

	x = 130; y = 10;

	//크리티컬
	CString strMsg;
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
	dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255);//agora
	//dwColor = D3DCOLOR_ARGB(255,0,0,0);//antes
/***********************************************************************************/
	if(	m_nDexCount != 0 && GetVirtualCritical() != g_pPlayer->GetCriticalProb() ) //임의 스탯이 변경되고 현재 능력치랑 다를 경우
	{
		if( (g_nRenderCnt / 8) & 1 )	
		{
			dwColor = D3DCOLOR_ARGB(255,255,0,0);
		}
		strMsg.Format( "%d%%", GetVirtualCritical() );
	}
	else
		strMsg.Format( "%d%%", g_pPlayer->GetCriticalProb() );
	p2DRender->TextOut( x , y, strMsg, dwColor ); y += nNext;

	//공격속도	
	float fAttackSpeed;
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
	dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255);//agora
	//dwColor = D3DCOLOR_ARGB(255,0,0,0);//antes
/***********************************************************************************/
	if(	m_nDexCount != 0 && GetVirtualATKSpeed() != g_pPlayer->GetAttackSpeed() ) //임의 스탯이 변경되고 현재 능력치랑 다를 경우
	{
		if( (g_nRenderCnt / 8) & 1 )	
		{
			dwColor = D3DCOLOR_ARGB(255,255,0,0);
		}
		fAttackSpeed = GetVirtualATKSpeed();
	}
	else
		fAttackSpeed = g_pPlayer->GetAttackSpeed();

	strMsg.Format( "%d%%", int( fAttackSpeed*100.0f ) /2 );
	p2DRender->TextOut( x , y, strMsg, dwColor ); y += nNext;

	x =15; y = 40; nNextX = 60;
	// 아래부터 능력치 관련 
	y = 52;
	int StatYPos = 50;

	if( g_pPlayer->m_nStr == g_pPlayer->GetStr() )
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
	dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255);//agora
	//dwColor = D3DCOLOR_ARGB(255,0,0,0);//antes
/***********************************************************************************/
	else
	if( g_pPlayer->m_nStr < g_pPlayer->GetStr() )
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
	dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255);//agora
	//dwColor = D3DCOLOR_ARGB(255,0,0,255);//antes
/***********************************************************************************/
	else
		dwColor = D3DCOLOR_ARGB(255,255,0,0);

	p2DRender->TextOut( StatYPos, y, g_pPlayer->GetStr(), dwColor ); y += nNext;

	if( g_pPlayer->m_nSta == g_pPlayer->GetSta() )
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
	dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255);//agora
	//dwColor = D3DCOLOR_ARGB(255,0,0,0);//antes
/***********************************************************************************/
	else
	if( g_pPlayer->m_nSta < g_pPlayer->GetSta() )
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
	dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255);//agora
	//dwColor = D3DCOLOR_ARGB(255,0,0,255);//antes
/***********************************************************************************/
	else
		dwColor = D3DCOLOR_ARGB(255,255,0,0);

	p2DRender->TextOut( StatYPos, y, g_pPlayer->GetSta(), dwColor ); y += nNext;

	if( g_pPlayer->m_nDex == g_pPlayer->GetDex() )
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
	dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255);//agora
	//dwColor = D3DCOLOR_ARGB(255,0,0,0);//antes
/***********************************************************************************/
	else
	if( g_pPlayer->m_nDex < g_pPlayer->GetDex() )
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
	dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255);//agora
	//dwColor = D3DCOLOR_ARGB(255,0,0,255);//antes
/***********************************************************************************/
	else
		dwColor = D3DCOLOR_ARGB(255,255,0,0);

	p2DRender->TextOut( StatYPos, y, g_pPlayer->GetDex(), dwColor ); y += nNext;

	if( g_pPlayer->m_nInt == g_pPlayer->GetInt() )
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
	dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255);//agora
	//dwColor = D3DCOLOR_ARGB(255,0,0,0);//antes
/***********************************************************************************/
	else
	if( g_pPlayer->m_nInt < g_pPlayer->GetInt() )
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
	dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255);//agora
	//dwColor = D3DCOLOR_ARGB(255,0,0,255);//antes
/***********************************************************************************/
	else
		dwColor = D3DCOLOR_ARGB(255,255,0,0);

	p2DRender->TextOut( StatYPos, y, g_pPlayer->GetInt(), dwColor ); y += nNext;
/*
	LPJOBABILITY pJobAbillity;
	if( g_pPlayer->IsExpert() )
		pJobAbillity = &prj.m_aJobAbillity[ g_pPlayer->GetJob() ];
	else
		pJobAbillity = &prj.m_aJobAbillity[ JOB_VAGRANT ];

	y = 52;

	p2DRender->TextOut( 135, y, pJobAbillity->dwStr, dwColor ); y += nNext;
	p2DRender->TextOut( 135, y, pJobAbillity->dwSta, dwColor ); y += nNext;
	p2DRender->TextOut( 135, y, pJobAbillity->dwDex, dwColor ); y += nNext;
	p2DRender->TextOut( 135, y, pJobAbillity->dwInt, dwColor ); y += nNext;
*/
	if(m_nGpPoint)
	{
		if( (g_nRenderCnt / 8) & 1 )	
		{
			dwColor = D3DCOLOR_ARGB(255,255,0,0);
		}
	}

	p2DRender->TextOut( 105, y, m_nGpPoint, dwColor ); y += nNext;

	CRect rectHittest[5];

	rectHittest[0].SetRect( 10, 52, 80, 65 );
	rectHittest[1].SetRect( 10, 67, 80, 80 );
	rectHittest[2].SetRect( 10, 82, 80, 95 );
	rectHittest[3].SetRect( 10, 97, 80, 110 );
	rectHittest[4].SetRect( 10, 112, 160, 125 );

	CRect rectTemp;
	CPoint ptTemp;
	// 툴팁 나오게 하기( Str, Sta, Dex, Int, GP )
	CPoint ptMouse = GetMousePoint();
	for( int iC = 0 ; iC < 5 ; ++iC )
	{
		if( rectHittest[iC].PtInRect( ptMouse ) )
		{
			ClientToScreen( &ptMouse );
			ClientToScreen( &rectHittest[iC] );
			CEditString strEdit;
			CString szString;
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
			DWORD dwColorName = D3DCOLOR_ARGB( 255, 238, 221, 130 );//agora
			DWORD dwColorCommand = D3DCOLOR_ARGB( 255, 180, 0, 0 );//agoraa
			//DWORD dwColorName = D3DCOLOR_XRGB( 0, 93, 0 );//antes
			//DWORD dwColorCommand = D3DCOLOR_XRGB( 180, 0, 0 );//antes
/***********************************************************************************/

			if( iC == 0 )	// STR
			{
				szString.Format( "%s", prj.GetText( TID_TOOLTIP_STR ) );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				szString.Format( "  %d\n ", g_pPlayer->GetStr() );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_STR0 ) );
				strEdit.AddString( "(" );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_STR1 ), dwColorCommand );
				strEdit.AddString( ")" );
			}
			else if( iC == 1 ) // STA
			{
				szString.Format( "%s", prj.GetText( TID_TOOLTIP_STA ) );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				szString.Format( "  %d\n ", g_pPlayer->GetSta() );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_STA0 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_STA1 ) );
			}
			else if( iC == 2 ) // DEX
			{
				szString.Format( "%s", prj.GetText( TID_TOOLTIP_DEX ) );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				szString.Format( "  %d\n ", g_pPlayer->GetDex() );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_DEX0 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_DEX1 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_DEX2 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_DEX3 ), dwColorCommand );
			}
			else if( iC == 3 ) // INT
			{
				szString.Format( "%s", prj.GetText( TID_TOOLTIP_INT ) );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				szString.Format( "  %d\n ", g_pPlayer->GetInt() );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_INT0 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_INT1 ) );
			}
			else // GP
				strEdit.AddString( prj.GetText( TID_TOOLTIP_CHARSTATUS_GPPOINT ) );
			
			g_toolTip.PutToolTip( 100, strEdit, rectHittest[iC], ptMouse, 3 );
			break;
		}
	}

	y = 10;
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
	//dwColor = D3DCOLOR_ARGB(255, 0, 0, 180);//ANTES
	dwColor = D3DCOLOR_ARGB( 255, 238, 221, 130 );
/***********************************************************************************/
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_13), dwColor ); 
	p2DRender->TextOut( 75, y, prj.GetText(TID_GAME_CHARACTER_14), dwColor ); y += nNext;

	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_DEFENCE), dwColor ); 
	p2DRender->TextOut( 75, y, prj.GetText(TID_GAME_CHARACTER_15), dwColor ); y += nNext;
	y += 12;
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_STR), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_STA), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_DEX), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_INT), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_07), dwColor ); y += nNext;
	
	//능력치 상태 Tooltip
	CRect rect;
	rect.SetRect( 7, 10, 160, 38 );
	if( rect.PtInRect( ptMouse ) )
	{
		ClientToScreen( &ptMouse );
		ClientToScreen( &rect );
		CEditString strEdit;
		if(m_nStrCount != 0 || m_nStaCount != 0 || m_nDexCount != 0 || m_nIntCount != 0)
			strEdit.AddString( prj.GetText( TID_TOOLTIP_CHARSTATUS_STATUS1 ) );
		else
			strEdit.AddString( prj.GetText( TID_TOOLTIP_CHARSTATUS_STATUS2 ) );
		g_toolTip.PutToolTip( 100, strEdit, rect, ptMouse, 3 );
	}
	//Edit Tooltip
	rect.SetRect(90, 52, 160, 110);
	if( rect.PtInRect( ptMouse ) )
	{
		ClientToScreen( &ptMouse );
		ClientToScreen( &rect );
		CEditString strEdit;
		strEdit.AddString( prj.GetText( TID_TOOLTIP_CHARSTATUS_EDIT ) );
		g_toolTip.PutToolTip( 100, strEdit, rect, ptMouse, 3 );
	}	
	//Button Tooltip
	rect = m_wndApply.m_rectWindow;
	if( rect.PtInRect( ptMouse ) )
	{
		ClientToScreen( &ptMouse );
		ClientToScreen( &rect );
		CEditString strEdit;
		strEdit.AddString( prj.GetText( TID_TOOLTIP_CHARSTATUS_APPLY ) );
		g_toolTip.PutToolTip( 100, strEdit, rect, ptMouse, 3 );
	}	
	rect = m_wndReset.m_rectWindow;
	if( rect.PtInRect( ptMouse ) )
	{
		ClientToScreen( &ptMouse );
		ClientToScreen( &rect );
		CEditString strEdit;
		strEdit.AddString( prj.GetText( TID_TOOLTIP_CHARSTATUS_RESET ) );
		g_toolTip.PutToolTip( 100, strEdit, rect, ptMouse, 3 );
	}
}

void CWndCharacterDetail2::OnInitialUpdate()
{
	CWndBase::OnInitialUpdate();

	SetTexture( m_pApp->m_pd3dDevice, MakePath( "Theme\\", ::GetLanguage(), _T( "WndCharacter2_2.tga" ) ), TRUE );
	FitTextureSize();
	
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize( 70, 25);
	
	CRect rect1_1( x - ( size.cx / 2), y, ( x - ( size.cx / 2 ) ) + size.cx, y + size.cy );
	
	CRect rect2_1( x - size.cx - 10, y, ( x - size.cx - 10 ) + size.cx, y + size.cy );
	CRect rect2_2( x + 10          , y, ( x + 10           ) + size.cx, y + size.cy );
	
	CRect rect3_1( x - ( size.cx / 2) - size.cx - 10, y, (x - ( size.cx / 2) - size.cx - 10) + size.cx, y + size.cy );
	CRect rect3_2( x - ( size.cx / 2)               , y, (x - ( size.cx / 2)               ) + size.cx, y + size.cy );
	CRect rect3_3( x + ( size.cx / 2) + 10          , y, (x + ( size.cx / 2) + 10          ) + size.cx, y + size.cy );

	// 아래부터 능력치 관련 
	int posY = 49;
	int posX = 128;

	m_editStrCount.Create  ( g_Neuz.GetSafeHwnd(), 0, CRect( posX - 38, posY, posX - 4, posY + 16 ), this, 100 );
//	m_wndStrPlus.Create  ( "", 0, CRect( posX, posY + 2, posX + 14, posY + 18 ), this, 101 );
//	m_wndStrMinus.Create  ( "", 0, CRect( posX + 16, posY + 2, posX + 30, posY + 18 ), this, 102 ); posY += 15;

	m_editStaCount.Create  ( g_Neuz.GetSafeHwnd(), 0, CRect( posX - 38, posY, posX - 4, posY + 16 ), this, 103 );
//	m_wndStaPlus.Create  ( "", 0, CRect( posX, posY + 2, posX + 14, posY + 18 ), this, 104 );
//	m_wndStaMinus.Create  ( "", 0, CRect( posX + 16, posY + 2, posX + 30, posY + 18 ), this, 105 ); posY += 15;

	m_editDexCount.Create  ( g_Neuz.GetSafeHwnd(), 0, CRect( posX - 38, posY, posX - 4, posY + 16 ), this, 106 );
//	m_wndDexPlus.Create  ( "<", 0, CRect( posX, posY + 2, posX + 14, posY + 18 ), this, 107 );
//	m_wndDexMinus.Create  ( "<", 0, CRect( posX + 16, posY + 2, posX + 30, posY + 18 ), this, 108 ); posY += 15;

	m_editIntCount.Create  ( g_Neuz.GetSafeHwnd(), 0, CRect( posX - 38, posY, posX - 4, posY + 16 ), this, 109 );
//	m_wndIntPlus.Create  ( "<", 0, CRect( posX, posY + 2, posX + 14, posY + 18 ), this, 110 );
//	m_wndIntMinus.Create  ( "<", 0, CRect( posX + 16, posY + 2, posX + 30, posY + 18 ), this, 111 );*/

	m_wndStrPlus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndStrMinus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharMinus.bmp" ) ), TRUE );
	m_wndStaPlus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndStaMinus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharMinus.bmp" ) ), TRUE );
	m_wndDexPlus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndDexMinus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharMinus.bmp" ) ), TRUE );
	m_wndIntPlus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndIntMinus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharMinus.bmp" ) ), TRUE );

	posY += 36;
	m_wndApply.Create  ( "Apply", 0, CRect( posX - 108, posY, posX - 58, posY + 22 ), this, 112 );
	m_wndReset.Create  ( "Reset", 0, CRect( posX - 30, posY, posX + 20, posY + 22 ), this, 113 );

	if(::GetLanguage() == LANG_USA)
	{
		m_wndApply.SetTexture( m_pApp->m_pd3dDevice, MakePath( "Theme\\", ::GetLanguage(), _T( "ButtNormal05.tga" ) ), TRUE );
		m_wndReset.SetTexture( m_pApp->m_pd3dDevice, MakePath( "Theme\\", ::GetLanguage(), _T( "ButtNormal05.tga" ) ), TRUE );
	}
	else
	{
		m_wndApply.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtNormal05.tga" ) ), TRUE );
		m_wndReset.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtNormal05.tga" ) ), TRUE );
	}

	//모든 스탯의 증가량 1로 고정이므로 아래 삭제
	/*
	LPJOBABILITY pJobAbillity;
	if( g_pPlayer->IsExpert() )
		pJobAbillity = &prj.m_aJobAbillity[ g_pPlayer->GetJob() ];
	else
		pJobAbillity = &prj.m_aJobAbillity[ JOB_VAGRANT ];

	m_wndStr.EnableWindow( ( g_pPlayer->GetRemainGP() >= pJobAbillity->dwStr ) ? TRUE : FALSE );
	m_wndSta.EnableWindow( ( g_pPlayer->GetRemainGP() >= pJobAbillity->dwSta ) ? TRUE : FALSE );
	m_wndDex.EnableWindow( ( g_pPlayer->GetRemainGP() >= pJobAbillity->dwDex ) ? TRUE : FALSE );
	m_wndInt.EnableWindow( ( g_pPlayer->GetRemainGP() >= pJobAbillity->dwInt ) ? TRUE : FALSE );
	*/

	m_nGpPoint = g_pPlayer->GetRemainGP();
	//스탯 증가량 한적 없으므로 Minus Button Default는 False
	m_wndStrMinus.EnableWindow(FALSE);
	m_wndStaMinus.EnableWindow(FALSE);
	m_wndDexMinus.EnableWindow(FALSE);
	m_wndIntMinus.EnableWindow(FALSE);

	if(g_pPlayer->GetRemainGP() <= 0)
	{
		m_wndStrPlus.EnableWindow(FALSE);
		m_wndStaPlus.EnableWindow(FALSE);
		m_wndDexPlus.EnableWindow(FALSE);
		m_wndIntPlus.EnableWindow(FALSE);
	}

	RefreshStatPoint();
}

BOOL CWndCharacterDetail2::Process()
{
	//Control Button Plus or Minus
	if(m_nStrCount > 0)
		m_wndStrMinus.EnableWindow(TRUE);
	else
		m_wndStrMinus.EnableWindow(FALSE);

	if(m_nStaCount > 0)
		m_wndStaMinus.EnableWindow(TRUE);
	else
		m_wndStaMinus.EnableWindow(FALSE);

	if(m_nDexCount > 0)
		m_wndDexMinus.EnableWindow(TRUE);
	else
		m_wndDexMinus.EnableWindow(FALSE);

	if(m_nIntCount > 0)
		m_wndIntMinus.EnableWindow(TRUE);
	else
		m_wndIntMinus.EnableWindow(FALSE);

	m_nGpPoint = g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nDexCount + m_nIntCount);

	if(m_nGpPoint > 0)
	{
		m_wndStrPlus.EnableWindow(TRUE);
		m_wndStaPlus.EnableWindow(TRUE);
		m_wndDexPlus.EnableWindow(TRUE);
		m_wndIntPlus.EnableWindow(TRUE);
		
		m_editStrCount.EnableWindow(TRUE);
		m_editStaCount.EnableWindow(TRUE);
		m_editDexCount.EnableWindow(TRUE);
		m_editIntCount.EnableWindow(TRUE);
	}
	else
	{
		m_wndStrPlus.EnableWindow(FALSE);
		m_wndStaPlus.EnableWindow(FALSE);
		m_wndDexPlus.EnableWindow(FALSE);
		m_wndIntPlus.EnableWindow(FALSE);
		
		m_editStrCount.EnableWindow(FALSE);
		m_editStaCount.EnableWindow(FALSE);
		m_editDexCount.EnableWindow(FALSE);
		m_editIntCount.EnableWindow(FALSE);
	}

	if(m_nStrCount > 0 || m_nStaCount > 0 || m_nDexCount > 0 || m_nIntCount > 0)
	{
		m_wndApply.EnableWindow(TRUE);
		m_wndReset.EnableWindow(TRUE);
	}
	else
	{
		m_wndApply.EnableWindow(FALSE);
		m_wndReset.EnableWindow(FALSE);
	}

	RefreshStatPoint();
	return TRUE;
}

void CWndCharacterDetail2::RefreshStatPoint()
{
	CString tempstr;
	tempstr.Format("%d", m_nStrCount);
	m_editStrCount.SetString(tempstr);
	tempstr.Format("%d", m_nStaCount);
	m_editStaCount.SetString(tempstr);
	tempstr.Format("%d", m_nDexCount);
	m_editDexCount.SetString(tempstr);
	tempstr.Format("%d", m_nIntCount);
	m_editIntCount.SetString(tempstr);
}

void CWndCharacterDetail2::GetVirtualATK(int* pnMin, int* pnMax)
{
	int nParts = PARTS_RWEAPON;
	
	*pnMin = 0;
	*pnMax = 0;
	
	if( g_pPlayer )
	{
		ItemProp* pItemProp = g_pPlayer->GetActiveHandItemProp( nParts );
		if( pItemProp == NULL )
			return;
		
		*pnMin = pItemProp->dwAbilityMin * 2;
		*pnMax = pItemProp->dwAbilityMax * 2;
		
		*pnMin = g_pPlayer->GetParam( DST_ABILITY_MIN, *pnMin );
		*pnMax = g_pPlayer->GetParam( DST_ABILITY_MAX, *pnMax );
		
		int nATK = 0;
		switch( pItemProp->dwWeaponType )
		{
			case WT_MELEE_SWD:
				nATK = (int)( float(( g_pPlayer->GetStr() + m_nStrCount - 12 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_SWD)) + (float(g_pPlayer->GetLevel() * 1.1f)) );
				break;
			case WT_MELEE_AXE:
				nATK = (int)( float(( g_pPlayer->GetStr() + m_nStrCount - 12 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_AXE)) + (float(g_pPlayer->GetLevel() * 1.2f)) );
				break;
			case WT_MELEE_STAFF:
				nATK = (int)( float(( g_pPlayer->GetStr() + m_nStrCount - 10 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_STAFF)) + (float(g_pPlayer->GetLevel() * 1.1f)) );
				break;
			case WT_MELEE_STICK:
				nATK = (int)( float(( g_pPlayer->GetStr() + m_nStrCount - 10 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_STICK)) + (float(g_pPlayer->GetLevel() * 1.3f)) );
				break;
			case WT_MELEE_KNUCKLE:
				nATK = (int)( float( (g_pPlayer->GetStr() + m_nStrCount - 10 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_KNUCKLE)) + (float(g_pPlayer->GetLevel() * 1.2f)) );
				break;
			case WT_MAGIC_WAND:
				nATK = (int)( ( g_pPlayer->GetInt() + m_nIntCount - 10 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_WAND) + g_pPlayer->GetLevel() * 1.2f );
				break;
			case WT_MELEE_YOYO:
				nATK = (int)( float(( g_pPlayer->GetStr() + m_nStrCount - 12 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_YOYO)) + (float(g_pPlayer->GetLevel() * 1.1f)) );
				break;
			case WT_RANGE_BOW:
				nATK = (int)( (((g_pPlayer->GetDex() + m_nDexCount -14)*4.0f + (g_pPlayer->GetLevel()*1.3f) + ((g_pPlayer->GetStr()+m_nStrCount)*0.2f)) * 0.7f) );
				break;
		}
		
		nATK += g_pPlayer->GetPlusWeaponATK( pItemProp->dwWeaponType );	// 무기의 추가 공격력를 구한다.

		int nPlus = nATK + g_pPlayer->GetParam( DST_CHR_DMG, 0 );

		*pnMin += nPlus;
		*pnMax += nPlus;
		
		CItemElem *pWeapon = g_pPlayer->GetWeaponItem( nParts );
		if( pWeapon && pWeapon->GetProp() )
		{
			float f = g_pPlayer->GetItemMultiplier( pWeapon );
			*pnMin = (int)( *pnMin * f );
			*pnMax = (int)( *pnMax * f );
		}
		
		if( pWeapon )
		{
			int nOption = pWeapon->GetAbilityOption();
			if( nOption > 0 )
			{
				int nValue = (int)pow( (float)( nOption ), 1.5f );

				*pnMin += nValue;
				*pnMax += nValue;
			}
		}
	}
}

int CWndCharacterDetail2::GetVirtualDEF()
{
	int nDefense = 0;

	ATTACK_INFO info;
	memset( &info, 0x00, sizeof(info) );
	info.dwAtkFlags = AF_GENERIC;
	
	float fFactor = 1.0f;
	if( g_pPlayer )
	{
		JobProp* pProperty = prj.GetJobProp( g_pPlayer->GetJob() ); 
		assert( pProperty );
		fFactor = pProperty->fFactorDef;
	}
	nDefense	= (int)( ((((g_pPlayer->GetLevel()*2) + ((g_pPlayer->GetSta()+m_nStaCount)/2)) / 2.8f ) - 4) + ((g_pPlayer->GetSta() + m_nStaCount - 14) * fFactor) );
	nDefense	+= (g_pPlayer->GetDefenseByItem( FALSE ) / 4);
	nDefense	+= g_pPlayer->GetParam( DST_ADJDEF, 0 );
	nDefense	= (int)( nDefense * g_pPlayer->GetDEFMultiplier( &info ) );

#ifdef __JEFF_11
	if( nDefense < 0 )
		nDefense	= 0;
#endif	// __JEFF_11
	return nDefense;
}

int CWndCharacterDetail2::GetVirtualCritical()
{
	int nCritical;
	nCritical	= ((g_pPlayer->GetDex() + m_nDexCount) / 10);
	nCritical	= (int)( nCritical * g_pPlayer->GetJobPropFactor( JOB_PROP_CRITICAL ) );
	nCritical	= g_pPlayer->GetParam( DST_CHR_CHANCECRITICAL, nCritical );	// 크리티컬 확률을 높여주는 스킬관련 
#ifdef __JEFF_11
	if( nCritical < 0 )
		nCritical	= 0;
#endif	// __JEFF_11
	
	if( g_pPlayer )
	{
		if( g_pPlayer->m_idparty && (g_pPlayer->m_dwFlag & MVRF_CRITICAL) )	// 파티 && 1회 크리티컬 발동?
		{
			if( g_Party.IsMember( g_pPlayer->m_idPlayer ) )
				nCritical += (g_Party.m_nSizeofMember / 2);	
			g_pPlayer->m_dwFlag &= (~MVRF_CRITICAL);
		}

#if __VER < 9 // __S_9_ADD
		if( g_pPlayer->IsAfterDeath() )							// 죽음 이후 상태라면?
			nCritical += CRITICAL_AFTER_DEATH;
		
		int nHitPercent = g_pPlayer->GetHitPointPercent( 100 );
		if( nHitPercent < CRITICAL_BERSERK_HP )			// HP가 MAX대비 30% 미만? 
		{
			// CRITICAL_BERSERK_HP : nHitPercent = CRITICAL_BERSERK_PROB : x
			// 30 : 15 = 20 : x
			nCritical += CRITICAL_BERSERK_PROB - ( nHitPercent * CRITICAL_BERSERK_PROB / CRITICAL_BERSERK_HP );
		}
#endif // __S_9_ADD
	}
	return nCritical;
}

float CWndCharacterDetail2::GetVirtualATKSpeed()
{
	float fSpeed = 1.0f;
	float fItem = 1.0f;
	
	ItemProp* pWeaponProp = g_pPlayer->GetActiveHandItemProp();
	if( pWeaponProp )
		fItem = pWeaponProp->fAttackSpeed;
	
	JobProp* pProperty = prj.GetJobProp( g_pPlayer->GetJob() ); 
	ASSERT( pProperty );
	
	// A = int( 캐릭터의 공속 + ( 무기의 공속 * ( 4 * 덱스 + ( 레벨 / 8 ) ) ) - 3 )
	// 공격속도 = ( ( 50 / 200 - A ) / 2 ) + 가중치 
	int A = int( pProperty->fAttackSpeed + ( fItem * ( 4.0f * (g_pPlayer->GetDex() + m_nDexCount) + g_pPlayer->GetLevel() / 8.0f ) ) - 3.0f );

	if( 187.5f <= A )
		A	= (int)( 187.5f );

	const MAX_ATTACK_SPEED_PLUSVALUE = 18;
	
	int nIndex = A / 10;
	nIndex = max( nIndex, 0 );
	nIndex = min( nIndex, (MAX_ATTACK_SPEED_PLUSVALUE-1) );
	
	float fPlusValue[MAX_ATTACK_SPEED_PLUSVALUE] = {
		0.08f, 0.16f, 0.24f, 0.32f,	0.40f,
		0.48f, 0.56f, 0.64f, 0.72f,	0.80f,
		0.88f, 0.96f, 1.04f, 1.12f,	1.20f,
		1.30f, 1.38f, 1.50f
	};
	
	fSpeed = ( ( 50.0f / (200.f - A) ) / 2.0f ) + fPlusValue[nIndex];
	
	float fDstParam = g_pPlayer->GetParam( DST_ATTACKSPEED, 0 ) / 1000.0f;
	fSpeed += fDstParam;
	
	if( 0 < g_pPlayer->GetParam( DST_ATTACKSPEED_RATE, 0 ) )
		fSpeed = fSpeed + ( fSpeed * g_pPlayer->GetParam( DST_ATTACKSPEED_RATE, 0 ) / 100 );
	
	if( fSpeed < 0.0f )
		fSpeed = 0.1f;
	if( fSpeed > 2.0f )
		fSpeed = 2.0f;
	
	return fSpeed;
}

BOOL CWndCharacterDetail2::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 240, 0, 240 + 330, 255 - 135 ); 
	//SetTitle( GETTEXT( TID_APP_CHARACTER )  );
	return CWndBase::Create(WBS_THICKFRAME|WBS_MOVE|WBS_SOUND|WBS_CAPTION|WBS_EXTENSION,rect,pWndParent,dwWndId);
}

BOOL CWndCharacterDetail2::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	int editnum = 0;
	
	if( nID == 100 || nID == 103 || nID == 106 || nID == 109 )
	{
		CString tempnum = m_editStrCount.GetString();
		for( int i = 0 ; i < tempnum.GetLength() ; i++ )
		{
			if( isdigit2( tempnum.GetAt(i) ) == FALSE )
				return FALSE;
		}
	}
	switch(nID) 
	{
		case 100:
			editnum = atoi( m_editStrCount.GetString() );
			if(editnum > g_pPlayer->GetRemainGP() - (m_nStaCount + m_nDexCount + m_nIntCount))
				editnum = g_pPlayer->GetRemainGP() - (m_nStaCount + m_nDexCount + m_nIntCount);
			m_nStrCount = editnum;
			break;
		case 103:
			editnum = atoi( m_editStaCount.GetString() );
			if(editnum > g_pPlayer->GetRemainGP() - (m_nStrCount + m_nDexCount + m_nIntCount))
				editnum = g_pPlayer->GetRemainGP() - (m_nStrCount + m_nDexCount + m_nIntCount);
			m_nStaCount = editnum;
			break;
		case 106:
			editnum = atoi( m_editDexCount.GetString() );
			if(editnum > g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nIntCount))
				editnum = g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nIntCount);
			m_nDexCount = editnum;
			break;
		case 109:
			editnum = atoi( m_editIntCount.GetString() );
			if(editnum > g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nDexCount))
				editnum = g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nDexCount);
			m_nIntCount = editnum;
			break;
	}

	if( message == WNM_CLICKED )		
	{
		switch(nID) 
		{
			case 101: //Str Plus
				m_nStrCount++;
				break;
			case 102: //Str Minus
				m_nStrCount--;
				break;
			case 104: //Sta Plus
				m_nStaCount++;
				break;
			case 105: //Sta Minus
				m_nStaCount--;
				break;
			case 107: //Dex Plus
				m_nDexCount++;
				break;
			case 108: //Dex Minus
				m_nDexCount--;
				break;
			case 110: //Int Plus
				m_nIntCount++;
				break;
			case 111: //Int Minus
				m_nIntCount--;
				break;
			case 112: //Apply
				if(m_nStrCount != 0 || m_nStaCount != 0 || m_nDexCount != 0 || m_nIntCount != 0)
				{
					SAFE_DELETE( g_WndMng.m_pWndStateConfirm );
					g_WndMng.m_pWndStateConfirm = new CWndStateConfirm;
					g_WndMng.m_pWndStateConfirm->Initialize();
				}
				break;
			case 113: //Reset
				m_nStrCount = 0;
				m_nStaCount = 0;
				m_nDexCount = 0;
				m_nIntCount = 0;
				RefreshStatPoint();
				break;
		}
	}

	if(nID >= 100 && nID <= 111) //If use Full GpPoint
	{
		if(g_pPlayer->GetRemainGP() == (m_nStrCount + m_nStaCount + m_nDexCount + m_nIntCount))
		{
			m_wndStrPlus.EnableWindow(FALSE);
			m_wndStaPlus.EnableWindow(FALSE);
			m_wndDexPlus.EnableWindow(FALSE);
			m_wndIntPlus.EnableWindow(FALSE);
		}
		else if(g_pPlayer->GetRemainGP() > (m_nStrCount + m_nStaCount + m_nDexCount + m_nIntCount))
		{
			m_wndStrPlus.EnableWindow(TRUE);
			m_wndStaPlus.EnableWindow(TRUE);
			m_wndDexPlus.EnableWindow(TRUE);
			m_wndIntPlus.EnableWindow(TRUE);
		}
	}
	return CWndBase::OnChildNotify( message, nID, pLResult );
}

BOOL CWndCharacterDetail2::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndBase::OnCommand(nID,dwMessage,pWndBase);
}

void CWndCharacterDetail2::OnSize(UINT nType, int cx, int cy)
{
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize( 70, 25 );

	CRect rect1_1( x - ( size.cx / 2), y, ( x - ( size.cx / 2 ) ) + size.cx, y + size.cy );

	CRect rect2_1( x - size.cx - 10, y, ( x - size.cx - 10 ) + size.cx, y + size.cy );
	CRect rect2_2( x + 10          , y, ( x + 10           ) + size.cx, y + size.cy );

	CRect rect3_1( x - ( size.cx / 2) - size.cx - 10, y, (x - ( size.cx / 2) - size.cx - 10) + size.cx, y + size.cy );
	CRect rect3_2( x - ( size.cx / 2)               , y, (x - ( size.cx / 2)               ) + size.cx, y + size.cy );
	CRect rect3_3( x + ( size.cx / 2) + 10          , y, (x + ( size.cx / 2) + 10          ) + size.cx, y + size.cy );

	CWndBase::OnSize(nType,cx,cy);
}

void CWndCharacterDetail2::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(IsWndRoot())
		return;
}

void CWndCharacterDetail2::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(IsWndRoot())
		return;
}

void CWndCharacterDetail2::OnMouseWndSurface( CPoint point )
{
}

#endif //__CSC_VER9_2
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 직업  
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
CWndChangeJob::CWndChangeJob( int nJob ) 
{ 
	m_nJob = nJob;
} 
CWndChangeJob::~CWndChangeJob() 
{ 
} 
void CWndChangeJob::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndChangeJob::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	for( int i  = 0 ; i < 6 ; ++i )
	{
		m_wndExpert[ i ].SetFontColor( 0xffffffff );
	}
	m_wndOk.SetFontColor( 0xffffffff );
	m_wndCancel.SetFontColor( 0xffffffff );

	if( m_nJob == 0 )	{
		m_wndExpert[ 0 ].Create( prj.m_aJob[ MAX_JOBBASE + 0 ].szName, 0, CRect( 10, 10, 230, 35 ), this, 10 );
		m_wndExpert[ 1 ].Create( prj.m_aJob[ MAX_JOBBASE + 1 ].szName, 0, CRect( 10, 40, 230, 65 ), this, 11 );
		m_wndExpert[ 2 ].Create( prj.m_aJob[ MAX_JOBBASE + 2 ].szName, 0, CRect( 10, 70, 230, 95 ), this, 12 );
		m_wndExpert[ 3 ].Create( prj.m_aJob[ MAX_JOBBASE + 3 ].szName, 0, CRect( 10, 100, 230, 125 ), this, 13 );
		m_wndExpert[ 4 ].Create( prj.m_aJob[ MAX_JOBBASE + 4 ].szName, 0, CRect( 10, 130, 230, 155 ), this, 14 );
	}
	else	{
		m_wndExpert[ 0 ].Create( prj.m_aJob[ m_nJob * 2 + 4 ].szName, 0, CRect( 10, 10, 230, 35  ), this, 10 ); 
		m_wndExpert[ 1 ].Create( prj.m_aJob[ m_nJob * 2 + 5 ].szName, 0, CRect( 10, 40, 230, 65  ), this, 11 ); 
	}
	m_wndOk.Create( _T( "OK" )  , 0, CRect( 60, 180, 110, 200), this, IDOK ); 
	m_wndCancel.Create( _T( "Cancel" ) , 0, CRect( 130, 180, 180, 200), this, IDCANCEL ); 

} 
BOOL CWndChangeJob::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	//SetTitle( _T( "전직" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_MODAL, rect, pWndParent, dwWndId ); 
} 
BOOL CWndChangeJob::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndChangeJob::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndChangeJob::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndChangeJob::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndChangeJob::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( message == WNM_CLICKED )
	{
		if( 10 <= nID && nID <= 14 )
		{
			if( g_pPlayer->IsBaseJob() )
			{
				if( 10 == nID || 11 == nID || 12 == nID || 13 == nID )
				{
					g_DPlay.SendChangeJob( nID - 9 );
				}
				else
				{
					g_WndMng.PutString( "Not Change Job" );
				}
			}
			else
			if( g_pPlayer->IsExpert() )
			{
				if( nID == 10 )
					g_DPlay.SendChangeJob( m_nJob * 2 + 4 );
				else
				if( nID == 11 )
					g_DPlay.SendChangeJob( m_nJob * 2 + 5 );
			}
			Destroy();
		}
		else
		{
			Destroy();
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 




/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 인벤토리 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
MERCENARY    
KNIGHT       
SPEARMAN     
TUMBLER      
PIERROT      
BEASTMASTER  
HEALER       
FORTUNETELLER
RINGMASTER   
JACKMETA     
JACKFLAM     
JACKEMITER   
PUPPETEER    
STRONGMAN    
STRINGMASTER 
*/

#ifdef __NEW_SKILL_TREE

CWndSkillTreeEx::CWndSkillTreeEx()
{
	m_pFocusItem = NULL;
	m_bDrag = FALSE;
	m_apSkill = NULL;
	m_cOldJob = JOB_VAGRANT;
	m_cActiveSlot = 0;
	m_cActiveJobType = JTYPE_BASE;

	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
		m_atexSkill[ i ] = NULL;

	m_atexTopDown[0] = NULL;
	m_atexTopDown[1] = NULL;

	m_texSelectSkill = NULL;

	m_aTexSkillTrees[ 0 ] = NULL;
	m_aTexSkillTrees[ 1 ] = NULL;
	m_aTexSkillTrees[ 2 ] = NULL;
	m_aTexSkillTrees[ 3 ] = NULL;

	m_nCurrSkillPoint = 0;
}

CWndSkillTreeEx::~CWndSkillTreeEx()
{
	DeleteDeviceObjects();
}

void CWndSkillTreeEx::SerializeRegInfo( CAr& ar, DWORD& dwVersion )
{
	CWndNeuz::SerializeRegInfo( ar, dwVersion );
	if( ar.IsLoading() )
	{
		ar >> m_cActiveSlot;
		SetSlot(m_cActiveSlot);
	}
	else
	{
		ar << m_cActiveSlot;
	}
}

BOOL CWndSkillTreeEx::IsDownPoint(DWORD skill)
{
	LPSKILL pSkill = GetdwSkill( skill );
	if( pSkill == NULL )
		return FALSE;

	LPSKILL pSkillMy = g_pPlayer->GetSkill( pSkill->dwSkill );
	if( pSkillMy == NULL )
		return FALSE;

	if(  pSkill->dwLevel <= pSkillMy->dwLevel )
		return FALSE;

	return TRUE;
}

void CWndSkillTreeEx::SubSkillPointDown( LPSKILL lpSkill ) // check skillpoint req for skill
{
	for( int i = 0; i < MAX_SKILL_JOB; i++ )	
	{
		if( m_apSkill[i].dwSkill == NULL_ID )
			continue;
		
		LPSKILL pSkill2 = &m_apSkill[i];
		
		if( pSkill2 == NULL )
			continue;
		
		if( pSkill2->GetProp()->dwReSkill1 == lpSkill->dwSkill )
		{
			if( pSkill2->GetProp()->dwReSkillLevel1 )
			{
				if( lpSkill->dwLevel < pSkill2->GetProp()->dwReSkillLevel1 )
				{
					if( pSkill2->dwLevel > 0 )
					{
						m_nCurrSkillPoint += ( prj.GetSkillPoint( pSkill2->GetProp() ) * pSkill2->dwLevel );
						pSkill2->dwLevel = 0;
						SubSkillPointDown( pSkill2 );
					}
				}
			}
		}
		
		if( pSkill2->GetProp()->dwReSkill2 == lpSkill->dwSkill )
		{
			if( pSkill2->GetProp()->dwReSkillLevel2 )
			{
				if( lpSkill->dwLevel < pSkill2->GetProp()->dwReSkillLevel2 )
				{
					if( pSkill2->dwLevel > 0 )
					{
						m_nCurrSkillPoint += ( prj.GetSkillPoint( pSkill2->GetProp() ) * pSkill2->dwLevel );
						pSkill2->dwLevel = 0;
						SubSkillPointDown( pSkill2 );
					}
				}
			}
		}
	}
}

LPSKILL CWndSkillTreeEx::GetdwSkill( DWORD dwSkill )
{
	for( int i = 0; i < MAX_SKILL_JOB; i++ )	
	{
		if( m_apSkill[i].dwSkill == dwSkill )
			return &m_apSkill[i];
	}
	return NULL;
}

LPSKILL CWndSkillTreeEx::GetSkill( int i )
{
	LPSKILL pSkill = &m_apSkill[ i ];
	
	if( pSkill->dwSkill == NULL_ID ) 
		return NULL;
	if( pSkill->GetProp() == NULL )
	{
		Error( "GetSkill Error Not Prop : %d", i );
		return NULL;
	}
	
	DWORD dwReqDislLV = pSkill->GetProp()->dwReqDisLV;
	return pSkill;
}
// 
void CWndSkillTreeEx::OnSize(UINT nType, int cx, int cy)
{
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 50;
	CSize size = CSize(70,25);
	
	CRect rect1_1( x - ( size.cx / 2), y, ( x - ( size.cx / 2 ) ) + size.cx, y + size.cy );
	
	CRect rect2_1( x - size.cx - 10, y, ( x - size.cx - 10 ) + size.cx, y + size.cy );
	CRect rect2_2( x + 10          , y, ( x + 10           ) + size.cx, y + size.cy );
	
	CRect rect3_1( x - ( size.cx / 2) - size.cx - 10, y, (x - ( size.cx / 2) - size.cx - 10) + size.cx, y + size.cy );
	CRect rect3_2( x - ( size.cx / 2)               , y, (x - ( size.cx / 2)               ) + size.cx, y + size.cy );
	CRect rect3_3( x + ( size.cx / 2) + 10          , y, (x + ( size.cx / 2) + 10          ) + size.cx, y + size.cy );
	
	CRect rect = GetClientRect();
	rect.DeflateRect( 4, 4 );

	CWndNeuz::OnSize(nType,cx,cy);
}

BOOL CWndSkillTreeEx::CheckSkill( int i )
{
	LPSKILL pSkill = &m_apSkill[ i ];

	if( pSkill->dwSkill == NULL_ID || g_pPlayer == NULL ) 
		return FALSE;
	
	DWORD dwSkill = pSkill->dwSkill;
	
	ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );

	if( pSkillProp == NULL || pSkillProp->nLog == 1 )
		return FALSE;
	if( g_pPlayer->GetLevel() < (int)( pSkillProp->dwReqDisLV ) )
		return FALSE;
				
	if( pSkillProp->dwReSkill1 != 0xffffffff )
	{
		LPSKILL pSkillBuf = GetdwSkill( pSkillProp->dwReSkill1 );

		if( pSkillBuf )
		{
			if( pSkillBuf->dwLevel < pSkillProp->dwReSkillLevel1 )
			{
				return FALSE;
			}
		}
		else
		{
		}
	}
	
	if( pSkillProp->dwReSkill2 != 0xffffffff )
	{
		LPSKILL pSkillBuf = GetdwSkill( pSkillProp->dwReSkill2 );
		if( pSkillBuf )
		{
			if( pSkillBuf->dwLevel < pSkillProp->dwReSkillLevel2 )
			{
				return FALSE;
			}				
		}
		else
		{
		}
	}	
	
	return TRUE;
}

HRESULT CWndSkillTreeEx::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	return S_OK;
}
HRESULT CWndSkillTreeEx::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	return S_OK;
}
HRESULT CWndSkillTreeEx::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	
	if( g_pPlayer->m_nSkillPoint > 0 )
	{
		safe_delete_array( m_apSkill );
	}
	else
		m_apSkill = NULL;
	
	return S_OK;
}

void CWndSkillTreeEx::InitItem(int nJob, LPSKILL apSkill, BOOL bReset )
{
	if( g_pPlayer == NULL )
		return;

	if( g_pPlayer->m_nSkillPoint > 0 )
	{
		m_nCurrSkillPoint = g_pPlayer->m_nSkillPoint;
		m_apSkill = new SKILL[MAX_SKILL_JOB];
		memcpy( m_apSkill, apSkill, sizeof(SKILL)*MAX_SKILL_JOB );
	}
	else
	{
		m_nCurrSkillPoint = 0;
		m_apSkill = apSkill;
	}

	m_pFocusItem = NULL;

	if( m_apSkill == NULL )
		return;

	CString strTex[4];
	CWndStatic* pWndStaticVagrant = (CWndStatic*) GetDlgItem( WIDC_STATIC_C1 );
	CWndStatic* pWndStatic1stJob = (CWndStatic*) GetDlgItem( WIDC_STATIC_C2 );
	CWndStatic* pWndStatic2ndJob = (CWndStatic*) GetDlgItem( WIDC_STATIC_C3 );
	CWndStatic* pWndStatic3rdJob = (CWndStatic*) GetDlgItem( WIDC_STATIC_C4 );
	CWndButton* pWndBtnVagrant = (CWndButton*) GetDlgItem( WIDC_BUTTON_C1 );
	CWndButton* pWndBtn1stJob = (CWndButton*) GetDlgItem( WIDC_BUTTON_C2 );
	CWndButton* pWndBtn2ndJob = (CWndButton*) GetDlgItem( WIDC_BUTTON_C3 );
	CWndButton* pWndBtn3rdJob = (CWndButton*) GetDlgItem( WIDC_BUTTON_C4 );
	CWndStatic* pWndStaticMaster = (CWndStatic*) GetDlgItem( WIDC_STATIC_MASTER );
	//CWndStatic* pWndStaticMasterIcon = (CWndStatic*) GetDlgItem( WIDC_CUSTOM2 );
	CWndStatic* pWndStaticHero = (CWndStatic*) GetDlgItem( WIDC_STATIC_HERO );
	//CWndStatic* pWndStaticHeroIcon = (CWndStatic*) GetDlgItem( WIDC_CUSTOM3 );

	LPWNDCTRL wndCtrlMaster = GetWndCtrl( WIDC_CUSTOM2 );
	LPWNDCTRL wndCtrlHero = GetWndCtrl( WIDC_CUSTOM3 );

	CString firstJob	= "LOCKED";
	CString secondJob	= "LOCKED";
	CString thirdJob	= "LOCKED";
/***********************************************************************************
                                  ADEILSON V19
/***********************************************************************************/
	pWndStaticVagrant->m_dwColor = D3DCOLOR_ARGB( 255, 238, 221, 130 );
	pWndStatic1stJob->m_dwColor = D3DCOLOR_ARGB( 255, 238, 221, 130 );
	pWndStatic2ndJob->m_dwColor = D3DCOLOR_ARGB( 255, 238, 221, 130 );
	pWndStatic3rdJob->m_dwColor = D3DCOLOR_ARGB( 255, 238, 221, 130 );
/***********************************************************************************/
	strTex[0] = "ImgSkillVagrant.tga";
	pWndStaticVagrant->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
	pWndBtnVagrant->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotVan.bmp" ), TRUE );

	
	//pWndStaticMasterIcon->EnableWindow(FALSE);
	//pWndStaticHeroIcon->EnableWindow(FALSE);

	//has first job?
	if(nJob >= MAX_JOBBASE)
	{
		pWndBtn1stJob->EnableWindow(TRUE);
	}
	else
	{
		pWndBtn1stJob->EnableWindow(FALSE);
		pWndBtn1stJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotVan.bmp" ), TRUE );
		pWndStatic1stJob->SetTitle(firstJob);
	}

	//has 2nd job?
	if(nJob >= MAX_EXPERT)
	{
		pWndBtn2ndJob->EnableWindow(TRUE);
	}
	else
	{
		pWndBtn2ndJob->EnableWindow(FALSE);
		pWndBtn2ndJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotVan.bmp" ), TRUE );
		pWndStatic2ndJob->SetTitle(secondJob);
	}
	// master
	if( nJob >= MAX_PROFESSIONAL )
	{
		pWndStaticMaster->SetVisible(TRUE);
	}
	else
	{
		pWndStaticMaster->SetVisible(FALSE);
	}
	//hero
	if( nJob >= MAX_MASTER )
	{
		pWndStaticHero->SetVisible(TRUE);

		if( nJob == JOB_KNIGHT_HERO || nJob == JOB_LORDTEMPLER_HERO )
			m_strHeroSkillIcon = "Back_Hero_KntDrawing.tga";
		else
		if( nJob == JOB_BLADE_HERO || nJob == JOB_STORMBLADE_HERO )
			m_strHeroSkillIcon = "Back_Hero_BldDefence.tga";
		else
		if( nJob == JOB_JESTER_HERO || nJob == JOB_WINDLURKER_HERO )
			m_strHeroSkillIcon = "Back_Hero_JstSilence.tga";
		else
		if( nJob == JOB_RANGER_HERO || nJob == JOB_CRACKSHOOTER_HERO )
			m_strHeroSkillIcon = "Back_Hero_RagHawkeye.tga";
		else
		if( nJob == JOB_RINGMASTER_HERO || nJob == JOB_FLORIST_HERO )
			m_strHeroSkillIcon = "Back_Hero_RigReturn.tga";
		else
		if( nJob == JOB_BILLPOSTER_HERO || nJob == JOB_FORCEMASTER_HERO )
			m_strHeroSkillIcon = "Back_Hero_BilDisenchant.tga";
		else
		if( nJob == JOB_PSYCHIKEEPER_HERO || nJob == JOB_MENTALIST_HERO )
			m_strHeroSkillIcon = "Back_Hero_PsyStone.tga";
		else
		if( nJob == JOB_ELEMENTOR_HERO || nJob == JOB_ELEMENTORLORD_HERO )
			m_strHeroSkillIcon = "Back_Hero_EleCursemind.tga";

	}
	else
	{
		pWndStaticHero->SetVisible(FALSE);
	}
	//has 3rd job?
	if(nJob >= MAX_HERO)
	{
		pWndBtn3rdJob->EnableWindow(TRUE);
	}
	else
	{
		pWndBtn3rdJob->EnableWindow(FALSE);
		pWndBtn3rdJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotVan.bmp" ), TRUE );
		pWndStatic3rdJob->SetTitle(thirdJob);
	}



	switch( nJob )
	{	
	case JOB_KNIGHT_HERO:
	case JOB_KNIGHT_MASTER:
	case JOB_KNIGHT:
		strTex[1] = "Back_Me.TGA";
		strTex[2] = "Back_Night.TGA";
		pWndStatic1stJob->SetTitle( prj.m_aJob[JOB_MERCENARY].szName );
		pWndStatic2ndJob->SetTitle( prj.m_aJob[JOB_KNIGHT].szName );
		pWndBtn1stJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotMer.bmp" ), TRUE );
		pWndBtn2ndJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotKnight.bmp" ), TRUE );
		break;	
	case JOB_LORDTEMPLER_HERO:
		strTex[1] = "Back_Me.TGA";
		strTex[2] = "Back_Night.TGA";
		strTex[3] = "SkillTreeLord.TGA";
		pWndStatic1stJob->SetTitle( prj.m_aJob[JOB_MERCENARY].szName );
		pWndStatic2ndJob->SetTitle( prj.m_aJob[JOB_KNIGHT].szName );
		pWndStatic3rdJob->SetTitle( prj.m_aJob[JOB_LORDTEMPLER_HERO].szName );
		pWndBtn1stJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotMer.bmp" ), TRUE );
		pWndBtn2ndJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotKnight.bmp" ), TRUE );
		pWndBtn3rdJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotLord.bmp" ), TRUE );
		break;
	case JOB_BLADE:
	case JOB_BLADE_MASTER:
	case JOB_BLADE_HERO:
		strTex[1] = "Back_Me.TGA";
		strTex[2] = "Back_Blade.TGA";
		pWndStatic1stJob->SetTitle( prj.m_aJob[JOB_MERCENARY].szName );
		pWndStatic2ndJob->SetTitle( prj.m_aJob[JOB_BLADE].szName );
		pWndBtn1stJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotMer.bmp" ), TRUE );
		pWndBtn2ndJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotBlad.bmp" ), TRUE );
		break;	
	case JOB_STORMBLADE_HERO:
		strTex[1] = "Back_Me.TGA";
		strTex[2] = "Back_Blade.TGA";
		strTex[3] = "SkillTreeStormb.TGA";
		pWndStatic1stJob->SetTitle( prj.m_aJob[JOB_MERCENARY].szName );
		pWndStatic2ndJob->SetTitle( prj.m_aJob[JOB_BLADE].szName );
		pWndStatic3rdJob->SetTitle( prj.m_aJob[JOB_STORMBLADE_HERO].szName );
		pWndBtn1stJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotMer.bmp" ), TRUE );
		pWndBtn2ndJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotBlad.bmp" ), TRUE );
		pWndBtn3rdJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotStormb.bmp" ), TRUE );
		break;	
	case JOB_MERCENARY:
		strTex[1] = "Back_Me.TGA";
		pWndStatic1stJob->SetTitle( prj.m_aJob[JOB_MERCENARY].szName );
		pWndBtn1stJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotMer.bmp" ), TRUE );
		break;			
	case JOB_BILLPOSTER:
	case JOB_BILLPOSTER_MASTER:
	case JOB_BILLPOSTER_HERO:
		strTex[1] = "Back_As.TGA";
		strTex[2] = "Back_Bill.TGA";
		pWndStatic1stJob->SetTitle( prj.m_aJob[JOB_ASSIST].szName );
		pWndStatic2ndJob->SetTitle( prj.m_aJob[JOB_BILLPOSTER].szName );
		pWndBtn1stJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotAs.bmp" ), TRUE );
		pWndBtn2ndJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotBillfor.bmp" ), TRUE );
		break;
	case JOB_FORCEMASTER_HERO:
		strTex[1] = "Back_As.TGA";
		strTex[2] = "Back_Bill.TGA";
		strTex[3] = "SkillTreeForcm.TGA";
		pWndStatic1stJob->SetTitle( prj.m_aJob[JOB_ASSIST].szName );
		pWndStatic2ndJob->SetTitle( prj.m_aJob[JOB_BILLPOSTER].szName );
		pWndStatic3rdJob->SetTitle( prj.m_aJob[JOB_FORCEMASTER_HERO].szName );
		pWndBtn1stJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotAs.bmp" ), TRUE );
		pWndBtn2ndJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotBillfor.bmp" ), TRUE );
		pWndBtn3rdJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotForcem.bmp" ), TRUE );
		break;	
	case JOB_RINGMASTER:
	case JOB_RINGMASTER_MASTER:
	case JOB_RINGMASTER_HERO:
		strTex[1] = "Back_As.TGA";
		strTex[2] = "Back_Ring.TGA";
		pWndStatic1stJob->SetTitle( prj.m_aJob[JOB_ASSIST].szName );
		pWndStatic2ndJob->SetTitle( prj.m_aJob[JOB_RINGMASTER].szName );
		pWndBtn1stJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotAs.bmp" ), TRUE );
		pWndBtn2ndJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotRingm.bmp" ), TRUE );
		break;
	case JOB_FLORIST_HERO:
		strTex[1] = "Back_As.TGA";
		strTex[2] = "Back_Ring.TGA";
		strTex[3] = "SkillTreeFlor.TGA";
		pWndStatic1stJob->SetTitle( prj.m_aJob[JOB_ASSIST].szName );
		pWndStatic2ndJob->SetTitle( prj.m_aJob[JOB_RINGMASTER].szName );
		pWndStatic3rdJob->SetTitle( prj.m_aJob[JOB_FLORIST_HERO].szName );
		pWndBtn1stJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotAs.bmp" ), TRUE );
		pWndBtn2ndJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotRingm.bmp" ), TRUE );
		pWndBtn3rdJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotFlor.bmp" ), TRUE );
		break;	
	case JOB_ASSIST:
		strTex[1] = "Back_As.TGA";
		pWndStatic1stJob->SetTitle( prj.m_aJob[JOB_ASSIST].szName );
		pWndBtn1stJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotAs.bmp" ), TRUE );
		break;			
	case JOB_ELEMENTOR:
	case JOB_ELEMENTOR_MASTER:
	case JOB_ELEMENTOR_HERO:
		strTex[1] = "Back_Ma.TGA";
		strTex[2] = "Back_Ele.TGA";
		pWndStatic1stJob->SetTitle( prj.m_aJob[JOB_MAGICIAN].szName );
		pWndStatic2ndJob->SetTitle( prj.m_aJob[JOB_ELEMENTOR].szName );
		pWndBtn1stJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotMag.bmp" ), TRUE );
		pWndBtn2ndJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotElem.bmp" ), TRUE );
		break;
	case JOB_ELEMENTORLORD_HERO:
		strTex[1] = "Back_Ma.TGA";
		strTex[2] = "Back_Ele.TGA";
		strTex[3] = "SkillTreeElel.TGA";
		pWndStatic1stJob->SetTitle( prj.m_aJob[JOB_MAGICIAN].szName );
		pWndStatic2ndJob->SetTitle( prj.m_aJob[JOB_ELEMENTOR].szName );
		pWndStatic3rdJob->SetTitle( prj.m_aJob[JOB_ELEMENTORLORD_HERO].szName );
		pWndBtn1stJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotMag.bmp" ), TRUE );
		pWndBtn2ndJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotElem.bmp" ), TRUE );
		pWndBtn3rdJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotElel.bmp" ), TRUE );
		break;	
	case JOB_PSYCHIKEEPER:
	case JOB_PSYCHIKEEPER_MASTER:
	case JOB_PSYCHIKEEPER_HERO:
		strTex[1] = "Back_Ma.TGA";
		strTex[2] = "Back_Psy.TGA";
		pWndStatic1stJob->SetTitle( prj.m_aJob[JOB_MAGICIAN].szName );
		pWndStatic2ndJob->SetTitle( prj.m_aJob[JOB_PSYCHIKEEPER].szName );
		pWndBtn1stJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotMag.bmp" ), TRUE );
		pWndBtn2ndJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotPsy.bmp" ), TRUE );
		break;
	case JOB_MENTALIST_HERO:
		strTex[1] = "Back_Ma.TGA";
		strTex[2] = "Back_Psy.TGA";
		strTex[3] = "SkillTreeMent.TGA";
		pWndStatic1stJob->SetTitle( prj.m_aJob[JOB_MAGICIAN].szName );
		pWndStatic2ndJob->SetTitle( prj.m_aJob[JOB_PSYCHIKEEPER].szName );
		pWndStatic3rdJob->SetTitle( prj.m_aJob[JOB_MENTALIST_HERO].szName );
		pWndBtn1stJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotMag.bmp" ), TRUE );
		pWndBtn2ndJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotPsy.bmp" ), TRUE );
		pWndBtn3rdJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotMent.bmp" ), TRUE );
		break;
	case JOB_MAGICIAN:
		strTex[1] = "Back_Ma.TGA";
		pWndStatic1stJob->SetTitle( prj.m_aJob[JOB_MAGICIAN].szName );
		pWndBtn1stJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotMag.bmp" ), TRUE );
		break;	
	case JOB_ACROBAT:
		strTex[1] = "Back_Acr.TGA";
		pWndStatic1stJob->SetTitle( prj.m_aJob[JOB_ACROBAT].szName );
		pWndBtn1stJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotArc.bmp" ), TRUE );
		break;	
	case JOB_JESTER:
	case JOB_JESTER_MASTER:
	case JOB_JESTER_HERO:
		strTex[1] = "Back_Acr.TGA";
		strTex[2] = "Back_Jst.TGA";
		pWndStatic1stJob->SetTitle( prj.m_aJob[JOB_ACROBAT].szName );
		pWndStatic2ndJob->SetTitle( prj.m_aJob[JOB_JESTER].szName );
		pWndBtn1stJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotArc.bmp" ), TRUE );
		pWndBtn2ndJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotJes.bmp" ), TRUE );
		break;
	case JOB_WINDLURKER_HERO:
		strTex[1] = "Back_Acr.TGA";
		strTex[2] = "Back_Jst.TGA";
		strTex[3] = "SkillTreeWindI.TGA";
		pWndStatic1stJob->SetTitle( prj.m_aJob[JOB_ACROBAT].szName );
		pWndStatic2ndJob->SetTitle( prj.m_aJob[JOB_JESTER].szName );
		pWndStatic3rdJob->SetTitle( prj.m_aJob[JOB_WINDLURKER_HERO].szName );
		pWndBtn1stJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotArc.bmp" ), TRUE );
		pWndBtn2ndJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotJes.bmp" ), TRUE );
		pWndBtn3rdJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotWindI.bmp" ), TRUE );
		break;
	case JOB_RANGER:
	case JOB_RANGER_MASTER:
	case JOB_RANGER_HERO:
		strTex[1] = "Back_Acr.TGA";
		strTex[2] = "Back_Rag.TGA";
		pWndStatic1stJob->SetTitle( prj.m_aJob[JOB_ACROBAT].szName );
		pWndStatic2ndJob->SetTitle( prj.m_aJob[JOB_RANGER].szName );
		pWndBtn1stJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotArc.bmp" ), TRUE );
		pWndBtn2ndJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotRan.bmp" ), TRUE );
		break;
	case JOB_CRACKSHOOTER_HERO:
		strTex[1] = "Back_Acr.TGA";
		strTex[2] = "Back_Rag.TGA";
		strTex[3] = "SkillTreeCracks.TGA";
		pWndStatic1stJob->SetTitle( prj.m_aJob[JOB_ACROBAT].szName );
		pWndStatic2ndJob->SetTitle( prj.m_aJob[JOB_RANGER].szName );
		pWndStatic3rdJob->SetTitle( prj.m_aJob[JOB_CRACKSHOOTER_HERO].szName );
		pWndBtn1stJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotArc.bmp" ), TRUE );
		pWndBtn2ndJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotRan.bmp" ), TRUE );
		pWndBtn3rdJob->SetTexture( D3DDEVICE, MakePath( DIR_THEME,  "SlotCracks.bmp" ), TRUE );
		break;
	}
	

	if( !strTex[0].IsEmpty() ) // vagrant bg
	{
		m_aTexSkillTrees[0] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME,  strTex[0] ), 0xffff00ff );
		if(!m_aTexSkillTrees[0])
			Error( "CWndSkillTreeEx::InitItem failed to load %s.", strTex[0] );
	}

	if( !strTex[1].IsEmpty() ) // 1st job bg
	{
		m_aTexSkillTrees[1] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME,  strTex[1] ), 0xffff00ff );
		if(!m_aTexSkillTrees[1])
			Error( "CWndSkillTreeEx::InitItem failed to load %s.", strTex[1] );
	}

	if( !strTex[2].IsEmpty() ) // 2nd job bg
	{
		m_aTexSkillTrees[2] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME,  strTex[2] ), 0xffff00ff );
		if(!m_aTexSkillTrees[2])
			Error("CWndSkillTreeEx::InitItem failed to load %s.",strTex[2]);
	}

	if( !strTex[3].IsEmpty() ) // 3rd job bg
	{
		m_aTexSkillTrees[3] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME,  strTex[3] ), 0xffff00ff );
		if(!m_aTexSkillTrees[3])
			Error("CWndSkillTreeEx::InitItem failed to load %s.",strTex[3]);
	}

	AdjustWndBase();
		
	LoadTextureSkillicon();

	g_nSkillCurSelect = -1;

	m_dwMouseSkill = NULL_ID;

	if(m_cOldJob != nJob)
	{
		SetSlot(0);
		m_cOldJob = nJob;
	}

	if( g_nSkillCurSelect >= 0 )
		m_pFocusItem = &m_apSkill[ g_nSkillCurSelect ];
}
BOOL CWndSkillTreeEx::GetSkillPoint(DWORD dwSkillID, CRect& rect )
{
	ItemProp* pPropSkill = prj.GetSkillProp( dwSkillID );

	if( pPropSkill )
	{
		int nRectX, nRectY;
		nRectX = nRectY = 0;

		switch(dwSkillID)
		{
	// Vagrant Skill Position
			case SI_VAG_ONE_CLEANHIT:				nRectX =  50, nRectY = 55; break;
			case SI_VAG_ONE_BRANDISH:				nRectX = 100, nRectY = 55; break;
			case SI_VAG_ONE_OVERCUTTER:				nRectX = 150, nRectY = 55; break;
	// Mercenary Skill Position
			case SI_MER_ONE_SPLMASH:				nRectX =  25, nRectY = 16; break;
			case SI_MER_SHIELD_PROTECTION:			nRectX =  25, nRectY = 68; break;
			case SI_MER_ONE_KEENWHEEL:				nRectX =  75, nRectY =  3; break;
			case SI_MER_ONE_BLOODYSTRIKE:			nRectX =  75, nRectY = 29; break;
			case SI_MER_SHIELD_PANBARRIER:			nRectX =  75, nRectY = 55; break;
			case SI_MER_SUP_IMPOWERWEAPON:			nRectX =  75, nRectY = 81; break;
			case SI_MER_ONE_BLINDSIDE:				nRectX = 125, nRectY =  3; break;
			case SI_MER_ONE_REFLEXHIT:				nRectX = 125, nRectY = 29; break;
			case SI_MER_ONE_SNEAKER:				nRectX = 125, nRectY = 55; break;
			case SI_MER_SUP_SMITEAXE:				nRectX = 125, nRectY = 81; break;
			case SI_MER_SUP_BLAZINGSWORD:			nRectX = 125, nRectY =107; break;
			case SI_MER_ONE_SPECIALHIT:				nRectX = 175, nRectY =  3; break;
			case SI_MER_ONE_GUILOTINE:				nRectX = 175, nRectY = 29; break;
			case SI_MER_SUP_AXEMASTER:				nRectX = 175, nRectY = 81; break;
			case SI_MER_SUP_SWORDMASTER:			nRectX = 175, nRectY =107; break;
	// Knight Skill Position
			case SI_KNT_TWOSW_CHARGE:				nRectX =  25, nRectY = 29; break;
			case SI_KNT_TWOAX_PAINDEALER:			nRectX =  25, nRectY = 55; break;
			case SI_KNT_SUP_GUARD:					nRectX =  25, nRectY = 81; break;
			case SI_KNT_TWOSW_EARTHDIVIDER:			nRectX =  75, nRectY = 29; break;
			case SI_KNT_TWOAX_POWERSTUMP:			nRectX =  75, nRectY = 55; break;
			case SI_KNT_SUP_RAGE:					nRectX =  75, nRectY = 81; break;
			case SI_KNT_TWO_POWERSWING:				nRectX = 125, nRectY = 42; break;
			case SI_KNT_SUP_PAINREFLECTION:			nRectX = 125, nRectY = 81; break;
	// Blade Skill Position
			case SI_BLD_DOUBLESW_SILENTSTRIKE:		nRectX =  25, nRectY = 29; break;
			case SI_BLD_DOUBLEAX_SPRINGATTACK:		nRectX =  25, nRectY = 55; break;
			case SI_BLD_DOUBLE_ARMORPENETRATE:		nRectX =  25, nRectY = 81; break;
			case SI_BLD_DOUBLESW_BLADEDANCE:		nRectX =  75, nRectY = 29; break;
			case SI_BLD_DOUBLEAX_HAWKATTACK:		nRectX =  75, nRectY = 55; break;
			case SI_BLD_SUP_BERSERK:				nRectX =  75, nRectY = 81; break;
			case SI_BLD_DOUBLE_CROSSSTRIKE:			nRectX = 125, nRectY = 42; break;
			case SI_BLD_DOUBLE_SONICBLADE:			nRectX = 125, nRectY = 81; break;
	// Assist Skill Position
			case SI_ASS_HEAL_HEALING:				nRectX =   8, nRectY = 42; break;
			case SI_ASS_HEAL_PATIENCE:				nRectX =  54, nRectY = 16; break;
			case SI_ASS_CHEER_QUICKSTEP:			nRectX =  54, nRectY = 42; break;
			case SI_ASS_CHEER_MENTALSIGN:			nRectX =  54, nRectY = 68; break;
			case SI_ASS_KNU_TAMPINGHOLE:			nRectX =  54, nRectY = 94; break;
			case SI_ASS_HEAL_RESURRECTION:			nRectX = 100, nRectY = 16; break;
			case SI_ASS_CHEER_HASTE:				nRectX = 100, nRectY = 42; break;
			case SI_ASS_CHEER_HEAPUP:				nRectX = 100, nRectY = 68; break;
			case SI_ASS_CHEER_STONEHAND:			nRectX = 100, nRectY = 94; break;
			case SI_ASS_CHEER_CIRCLEHEALING:		nRectX = 146, nRectY = 16; break;
			case SI_ASS_CHEER_CATSREFLEX:			nRectX = 146, nRectY = 42; break;
			case SI_ASS_CHEER_BEEFUP:				nRectX = 146, nRectY = 68; break;
			case SI_ASS_KNU_BURSTCRACK:				nRectX = 146, nRectY = 94; break;
			case SI_ASS_HEAL_PREVENTION:			nRectX = 192, nRectY = 16; break;
			case SI_ASS_CHEER_CANNONBALL:			nRectX = 192, nRectY = 42; break;
			case SI_ASS_CHEER_ACCURACY:				nRectX = 192, nRectY = 68; break;
			case SI_ASS_KNU_POWERFIST:				nRectX = 192, nRectY = 94; break;
	// Ringmaster Skill Position
			case SI_RIN_SUP_PROTECT:				nRectX =  25, nRectY = 29; break;
			case SI_RIN_SUP_HOLYCROSS:				nRectX =  25, nRectY = 55; break;
			case SI_RIN_HEAL_GVURTIALLA:			nRectX =  25, nRectY = 81; break;
			case SI_RIN_SUP_HOLYGUARD:				nRectX =  75, nRectY = 29; break;
			case SI_RIN_SUP_SPIRITUREFORTUNE:		nRectX =  75, nRectY = 55; break;
			case SI_RIN_HEAL_HEALRAIN:				nRectX =  75, nRectY = 81; break;
			case SI_RIN_SQU_GEBURAHTIPHRETH:		nRectX = 125, nRectY = 42; break;
			case SI_RIN_SUP_MERKABAHANZELRUSHA:		nRectX = 125, nRectY = 81; break;
	// Billposter Skill Position
			case SI_BIL_KNU_BELIALSMESHING:			nRectX =  25, nRectY = 29; break;
			case SI_BIL_PST_ASMODEUS:				nRectX =  25, nRectY = 55; break;
			case SI_BIL_KNU_BLOODFIST:				nRectX =  75, nRectY = 29; break;
			case SI_BIL_PST_BARAQIJALESNA:			nRectX =  75, nRectY = 55; break;
			case SI_BIL_KNU_PIERCINGSERPENT:		nRectX = 125, nRectY = 29; break;
			case SI_BIL_PST_BGVURTIALBOLD:			nRectX = 125, nRectY = 55; break;
			case SI_BIL_KNU_SONICHAND:				nRectX = 175, nRectY = 42; break;
			case SI_BIL_PST_ASALRAALAIKUM:			nRectX = 175, nRectY = 81; break;
	// Mage Skill Position
			case SI_MAG_MAG_MENTALSTRIKE:			nRectX =  25, nRectY =  3; break;
			case SI_MAG_MAG_BLINKPOOL: 				nRectX =  25, nRectY = 55; break;
			case SI_MAG_FIRE_BOOMERANG:				nRectX =  75, nRectY =  3; break;
			case SI_MAG_WIND_SWORDWIND:				nRectX =  75, nRectY = 29; break;
			case SI_MAG_WATER_ICEMISSILE:			nRectX =  75, nRectY = 55; break;
			case SI_MAG_ELECTRICITY_LIGHTINGBALL:	nRectX =  75, nRectY = 81; break;
			case SI_MAG_EARTH_SPIKESTONE:			nRectX =  75, nRectY =107; break;
			case SI_MAG_FIRE_HOTAIR:				nRectX = 125, nRectY =  3; break;
			case SI_MAG_WIND_STRONGWIND:			nRectX = 125, nRectY = 29; break;
			case SI_MAG_WATER_WATERBALL:			nRectX = 125, nRectY = 55; break;
			case SI_MAG_ELECTRICITY_LIGHTINGPARM:	nRectX = 125, nRectY = 81; break;
			case SI_MAG_EARTH_ROCKCRASH:			nRectX = 125, nRectY =107; break;
			case SI_MAG_FIRE_FIRESTRIKE:			nRectX = 175, nRectY =  3; break;
			case SI_MAG_WIND_WINDCUTTER:			nRectX = 175, nRectY = 29; break;
			case SI_MAG_WATER_SPRINGWATER:			nRectX = 175, nRectY = 55; break;
			case SI_MAG_ELECTRICITY_LIGHTINGSHOCK:	nRectX = 175, nRectY = 81; break;
			case SI_MAG_EARTH_LOOTING:				nRectX = 175, nRectY =107; break;
	// Elementor Skill Position
			case SI_ELE_FIRE_FIREBIRD:				nRectX =   25, nRectY =   3; break;
			case SI_ELE_EARTH_STONESPEAR:			nRectX =   25, nRectY =  29; break;
			case SI_ELE_WIND_VOID:					nRectX =   25, nRectY =  55; break;
			case SI_ELE_ELECTRICITY_THUNDERSTRIKE:	nRectX =   25, nRectY =  81; break;
			case SI_ELE_WATER_ICESHARK:				nRectX =   25, nRectY = 107; break;
			case SI_ELE_FIRE_BURINGFIELD:			nRectX =  75, nRectY =   3; break;
			case SI_ELE_EARTH_EARTHQUAKE:			nRectX =  75, nRectY =  29; break;
			case SI_ELE_WIND_WINDFIELD:				nRectX =  75, nRectY =  55; break;
			case SI_ELE_ELECTRICITY_ELETRICSHOCK:	nRectX =  75, nRectY =  81; break;
			case SI_ELE_WATER_POISONCLOUD:			nRectX =  75, nRectY = 107; break;
			case SI_ELE_MULTY_METEOSHOWER:			nRectX = 125, nRectY =  16; break;
			case SI_ELE_MULTY_SANDSTORM:			nRectX = 125, nRectY =  42; break;
			case SI_ELE_MULTY_LIGHTINGSTORM:		nRectX = 125, nRectY =  68; break;
			case SI_ELE_MULTY_AVALANCHE:			nRectX = 125, nRectY = 107; break;
			case SI_ELE_FIRE_FIREMASTER:			nRectX = 175, nRectY =   3; break;
			case SI_ELE_EARTH_EARTHMASTER:			nRectX = 175, nRectY =  29; break;
			case SI_ELE_WIND_WINDMASTER:			nRectX = 175, nRectY =  55; break;
			case SI_ELE_ELECTRICITY_LIGHTINGMASTER:	nRectX = 175, nRectY =  81; break;
			case SI_ELE_WATER_WATERMASTER:			nRectX = 175, nRectY = 107; break;
	// Psykeeper Skill Position
			case SI_PSY_NLG_DEMONOLGY:				nRectX =  25, nRectY =  29; break;
			case SI_PSY_PSY_PSYCHICBOMB:			nRectX =  25, nRectY =  55; break;
			case SI_PSY_NLG_CRUCIOSPELL:			nRectX =  25, nRectY =  81; break;
			case SI_PSY_NLG_SATANOLGY:				nRectX =  75, nRectY =  29; break;
			case SI_PSY_PSY_SPRITBOMB:				nRectX =  75, nRectY =  55; break;
			case SI_PSY_PSY_MAXIMUMCRISIS:			nRectX =  75, nRectY =  81; break;
			case SI_PSY_PSY_PSYCHICWALL:			nRectX = 125, nRectY =  42; break;
			case SI_PSY_PSY_PSYCHICSQUARE:			nRectX = 125, nRectY =  81; break;
	// Acrobat
			case SI_ACR_YOYO_PULLING:				nRectX =  25, nRectY =   4; break;
			case SI_ACR_SUP_SLOWSTEP:				nRectX =  25, nRectY =  30; break;
			case SI_ACR_BOW_JUNKBOW:				nRectX =  25, nRectY =  81; break;
			case SI_ACR_SUP_FASTWALKER:				nRectX =  75, nRectY =   4; break;
			case SI_ACR_SUP_YOYOMASTER:				nRectX =  75, nRectY =  30; break;
			case SI_ACR_SUP_BOWMASTER: 				nRectX =  75, nRectY =  81; break;
			case SI_ACR_SUP_DARKILLUSION:			nRectX = 125, nRectY =   4; break;
			case SI_ACR_YOYO_SNITCH:				nRectX = 125, nRectY =  30; break;
			case SI_ACR_YOYO_CROSSLINE:				nRectX = 125, nRectY =  56; break;		
			case SI_ACR_BOW_SILENTSHOT:				nRectX = 125, nRectY =  81; break;		
			case SI_ACR_BOW_AIMEDSHOT:				nRectX = 125, nRectY = 107; break;
			case SI_ACR_YOYO_ABSOLUTEBLOCK:			nRectX = 175, nRectY =   4; break;
			case SI_ACR_YOYO_DEADLYSWING:			nRectX = 175, nRectY =  30; break;		
			case SI_ACR_YOYO_COUNTER:				nRectX = 175, nRectY =  56; break;		
			case SI_ACR_BOW_AUTOSHOT:				nRectX = 175, nRectY =  81; break;		
			case SI_ACR_BOW_ARROWRAIN:				nRectX = 175, nRectY = 107; break;		
	//Jester
			case SI_JST_SUP_POISON:					nRectX =  25, nRectY = 29; break;
			case SI_JST_SUP_BLEEDING:				nRectX =  25, nRectY = 55; break;
			case SI_JST_YOYO_ESCAPE:				nRectX =  25, nRectY = 81; break;
			case SI_JST_YOYO_CRITICALSWING:			nRectX =  75, nRectY = 29; break;
			case SI_JST_YOYO_BACKSTAB:				nRectX =  75, nRectY = 55; break;
			case SI_JST_SUP_ABSORB:					nRectX =  75, nRectY = 81; break;
			case SI_JST_YOYO_VATALSTAB:				nRectX = 125, nRectY = 42; break;
			case SI_JST_YOYO_HITOFPENYA:			nRectX = 125, nRectY = 81; break;
	//Ranger
			case SI_RAG_BOW_ICEARROW:				nRectX =  25, nRectY = 29; break;
			case SI_RAG_BOW_FLAMEARROW:				nRectX =  25, nRectY = 55; break;
			case SI_RAG_BOW_POISONARROW:			nRectX =  25, nRectY = 81; break;
			case SI_RAG_SUP_FASTATTACK:				nRectX =  75, nRectY = 29; break;
			case SI_RAG_BOW_PIRCINGARROW:			nRectX =  75, nRectY = 55; break;
			case SI_RAG_SUP_NATURE:					nRectX =  75, nRectY = 81; break;
			case SI_RAG_BOW_TRIPLESHOT:				nRectX = 125, nRectY = 42; break;
			case SI_RAG_BOW_SILENTARROW:			nRectX = 125, nRectY = 81; break;
	// v16 Skills
	// Lord Templar
			case SI_LOD_SUP_PULLING:				nRectX =  25, nRectY = 29; break;
			case SI_LOD_ONE_GRANDRAGE:				nRectX =  25, nRectY = 55; break;
			case SI_LOD_SUP_HOLYARMOR:				nRectX =  25, nRectY = 81; break;
			case SI_LOD_SUP_SCOPESTRIKE:			nRectX =  75, nRectY = 55; break;
			case SI_LOD_SUP_ANGER:					nRectX =  75, nRectY = 81; break;
			case SI_LOD_ONE_SHILDSTRIKE:			nRectX = 125, nRectY = 55; break;
	// Storm Blade		
			case SI_STORM_DOUBLE_CROSSBLOOD:		nRectX =  25, nRectY = 29; break;
			case SI_STORM_SUP_POWERINCREASE:		nRectX =  25, nRectY = 55; break;
			case SI_STORM_DOUBLE_STORMBLAST:		nRectX =  75, nRectY = 29; break;
			case SI_STORM_DOUBLE_HOLDINGSTORM:		nRectX =  75, nRectY = 55; break;
	// Wind Lurker
			case SI_WIN_YOYO_MADHURRICANE:				nRectX =  25, nRectY = 55; break;
			case SI_WIN_SUP_EVASIONINCREASE:			nRectX =  25, nRectY = 29; break;
			case SI_WIN_SUP_CONTROLINCREASE:			nRectX =  75, nRectY = 29; break;
			case SI_WIN_YOYO_BACKSTEP:					nRectX =  75, nRectY = 55; break;
	// Crack Shooter
			case SI_CRA_SUP_POWERINCREASE:			nRectX =  25, nRectY = 29; break;
			case SI_CRA_CRBOW_RANGESTRIKE:			nRectX =  25, nRectY = 55; break;
			case SI_CRA_SUP_HAWKEYE:				nRectX =  125, nRectY = 29; break;
			case SI_CRA_SUP_CONTROLINCREASE:		nRectX =  75, nRectY = 29; break;
	// Florist
			case SI_FLO_HEAL_DOT:					nRectX =  25, nRectY = 42; break;
			case SI_FLO_SQU_BLESSSTEP:				nRectX =  71, nRectY = 16; break;
			case SI_FLO_SQU_BLESSBODY:				nRectX =  71, nRectY = 42; break;
			case SI_FLO_SQU_BLESSARMOR:				nRectX =  71, nRectY = 68; break;
			case SI_FLO_SUP_ABSOLUTE:				nRectX = 117, nRectY = 94; break;
			case SI_FLO_CHEER_PATTERS:				nRectX =  71, nRectY = 94; break;
	// Force Master		
			case SI_FOR_SQU_FORCETENACITY:		nRectX =  25, nRectY = 29; break;	
			case SI_FOR_SQU_FORCEANGER:			nRectX =  75, nRectY = 29; break;
			case SI_FOR_SQU_FORCESPEED:			nRectX =  25, nRectY = 55; break;
			case SI_FOR_SQU_FORCEMAD:			nRectX =  75, nRectY = 55; break;
	//Mentalist	
			case SI_MEN_WAN_DARKNESSLAKE:			nRectX =  71, nRectY = 94; break;
			case SI_MEN_WAN_ATKDECREASE:			nRectX =  71, nRectY = 16; break;
			case SI_MEN_WAN_FEARSCREAM:				nRectX =  25, nRectY = 42; break;
			case SI_MEN_WAN_DEFDECREASE:			nRectX =  71, nRectY = 42; break;	
			case SI_MEN_WAN_SPEDECREASE:			nRectX =  71, nRectY = 68; break;	
	// Elemental Lord
			case SI_ELE_STF_FINALSPEAR:				nRectX =  25, nRectY = 29; break;
			case SI_ELE_STF_COSMICELEMENT:			nRectX =  25, nRectY = 55; break;
			case SI_ELE_STF_SLIPPING:				nRectX =  75, nRectY = 81; break;
			case SI_ELE_STF_THUNDERBOLT:			nRectX =  75, nRectY = 42; break;
	// Default
			default:
				if( pPropSkill->dwItemKind1 == JTYPE_MASTER )
				{
					rect = CRect( 0,  0, 24, 24 );
					LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );
					if(lpWndCtrl)
						rect += lpWndCtrl->rect.TopLeft();
					return TRUE;
				}
				else
				if( pPropSkill->dwItemKind1 == JTYPE_HERO )
				{
					rect = CRect( 0,  0, 24, 24 );
					LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM3 );
					if(lpWndCtrl)
						rect += lpWndCtrl->rect.TopLeft();
					return TRUE;
				}
				else
					return FALSE;
		}

		nRectY += 4;
		rect = CRect( nRectX,  nRectY, nRectX + 24, nRectY + 24 );

		LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_STATIC_PANNEL );
		if(lpWndCtrl)
		{
			rect += lpWndCtrl->rect.TopLeft();
		}
	}
	return TRUE;
}
// Icon/Level Graphic Load ( ready )
void CWndSkillTreeEx::LoadTextureSkillicon()
{
	if( m_apSkill == NULL )
		return;

	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
	{
		LPSKILL lpSkill;
		lpSkill = &m_apSkill[ i ];
		if( lpSkill->dwSkill != NULL_ID )
		{
			ItemProp* pSkillProp = prj.m_aPropSkill.GetAt( lpSkill->dwSkill );
			if( pSkillProp )
				m_atexSkill[ i ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, pSkillProp->szIcon ), 0xffff00ff );
		}
	}
	m_textPackNum.LoadScript( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, _T( "icon_IconSkillLevel.inc" ) ) );
}
// On Move Skill to F-Bar ( ready )
void CWndSkillTreeEx::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_apSkill == NULL )
		return;

	if( m_bDrag == FALSE )
		return;
	
	CPoint pt( 3, 3 );
	CRect rect;
	if( CheckSkill( g_nSkillCurSelect ) )
	{
		m_bDrag = FALSE;
		DWORD dwSkill = m_apSkill[ g_nSkillCurSelect ].dwSkill;
		ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
		
		m_GlobalShortcut.m_pFromWnd    = this;
		m_GlobalShortcut.m_dwShortcut  = SHORTCUT_SKILL;
		m_GlobalShortcut.m_dwType  = 0;
		m_GlobalShortcut.m_dwIndex = dwSkill;
		m_GlobalShortcut.m_dwData = 0;
		m_GlobalShortcut.m_dwId       = g_nSkillCurSelect; // 컬런트 셀렉트가 곧 ID나 마찬가지임.
		m_GlobalShortcut.m_pTexture = m_atexSkill[ g_nSkillCurSelect ];//L;//pItemElem->m_pTexture;
		_tcscpy( m_GlobalShortcut.m_szString, pSkillProp->szName);
	}
}
// 
BOOL CWndSkillTreeEx::Process()
{
	if( m_apSkill == NULL || g_pPlayer == NULL)
		return FALSE;
	CWndButton* pWndBtn;
	switch(m_cActiveSlot)
	{
	case 0: pWndBtn = (CWndButton*)GetDlgItem(WIDC_BUTTON_C1);break;
	case 1: pWndBtn = (CWndButton*)GetDlgItem(WIDC_BUTTON_C2);break;
	case 2: pWndBtn = (CWndButton*)GetDlgItem(WIDC_BUTTON_C3);break;
	case 3: pWndBtn = (CWndButton*)GetDlgItem(WIDC_BUTTON_C4);break;
	}
	
	if(pWndBtn)
		pWndBtn->m_bPush = true;

	m_pWndButton[0]->EnableWindow( FALSE );
	m_pWndButton[1]->EnableWindow( FALSE );
	m_pWndButton[2]->EnableWindow( FALSE );
	m_pWndButton[3]->EnableWindow( FALSE );

	if( m_pFocusItem && 0 < g_pPlayer->m_nSkillPoint )
	{
		LPSKILL lpSkillUser = g_pPlayer->GetSkill( m_pFocusItem->dwSkill );
		ItemProp* pSkillProp = m_pFocusItem->GetProp();
		if( pSkillProp == NULL || lpSkillUser == NULL )
			return TRUE;
		if( pSkillProp->dwItemKind1 == JTYPE_MASTER || pSkillProp->dwItemKind1 == JTYPE_HERO )
		{	
			CWndStatic* lpWndStaticNeedSP = (CWndStatic*)GetDlgItem( WIDC_STATIC_NEEDSP );
			lpWndStaticNeedSP->SetTitle( "0" );
			return TRUE;
		}
		int nPoint = prj.GetSkillPoint( pSkillProp );
		if( m_pFocusItem->dwLevel < pSkillProp->dwExpertMax && nPoint <= m_nCurrSkillPoint )
		{
			m_pWndButton[0]->EnableWindow( TRUE );	
		}
		if( m_pFocusItem->dwLevel != lpSkillUser->dwLevel )
			m_pWndButton[1]->EnableWindow( TRUE );	

		if( m_nCurrSkillPoint != g_pPlayer->m_nSkillPoint )
		{
			m_pWndButton[2]->EnableWindow( TRUE );
			m_pWndButton[3]->EnableWindow( TRUE );
		}
	}

	if( m_pFocusItem )  // render the amount of points it takes to level this skill
	{
		CWndStatic* lpWndStaticNeedSP = (CWndStatic*)GetDlgItem( WIDC_STATIC_NEEDSP );
		ItemProp* pSkillProp = m_pFocusItem->GetProp();
		if ( pSkillProp )
		{
			int nPoint = prj.GetSkillPoint ( pSkillProp );
			CString strSPN;
			strSPN.Format( "%d", nPoint );
			lpWndStaticNeedSP->SetTitle( strSPN );
		}
	}
	else
	{
		CWndStatic* lpWndStaticNeedSP = (CWndStatic*)GetDlgItem( WIDC_STATIC_NEEDSP );
		lpWndStaticNeedSP->SetTitle( "0" );
	}

	return TRUE;
}
// 
void CWndSkillTreeEx::OnMouseWndSurface( CPoint point )
{
	DWORD dwMouseSkill = NULL_ID;
	for( int i = 0; i < MAX_SKILL_JOB; i++ )
	{
		LPSKILL pSkill = GetSkill( i );
		if( pSkill == NULL )
			continue;
		DWORD dwSkill = pSkill->dwSkill;

		ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );

		if( dwSkill != NULL_ID && ( pSkillProp->dwItemKind1 == m_cActiveJobType || ( pSkillProp->dwItemKind1 == JTYPE_MASTER || pSkillProp->dwItemKind1 == JTYPE_HERO ) ))
		{
			CRect rect;
			if( GetSkillPoint( pSkillProp->dwID, rect ) )
			{
				rect.top  -= 2;
				rect.right  = rect.left+27;
				rect.bottom = rect.top+27;

				if( rect.PtInRect(point) )
				{
					dwMouseSkill = dwSkill;

					ClientToScreen( &point );
					ClientToScreen( &rect );
					g_WndMng.PutToolTip_Skill( dwSkill, pSkill->dwLevel, point, &rect, CheckSkill( i ) );
					break;
				}
			}
		}
	}
	m_dwMouseSkill = dwMouseSkill;
}
// 
void CWndSkillTreeEx::OnDraw(C2DRender* p2DRender)
{
	m_aTexSkillTrees[m_cActiveSlot]->Render(p2DRender,m_pointTreePos);

	if( m_apSkill == NULL )
		return;

	if( !m_strHeroSkillIcon.IsEmpty() )
	{
		CTexture* pTexture;
		CRect rect;
		CPoint point;
		LPWNDCTRL lpWndCtrl;
		
		lpWndCtrl = GetWndCtrl( WIDC_CUSTOM3 );
		rect = lpWndCtrl->rect;
		point = rect.TopLeft();
		point.y -= 2;
		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, m_strHeroSkillIcon), 0xffff00ff );
		if(pTexture != NULL)
			pTexture->Render( p2DRender, point, CPoint( 27, 27 ) );				
	}

	m_nAlphaCount = m_nAlpha;
	CPoint pt;
	LPWNDCTRL lpWndCtrl;

	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
	{
		LPSKILL pSkill = GetSkill( i );
		if( pSkill == NULL) 
			continue;

		DWORD dwSkill = pSkill->dwSkill;

		ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
		
		if( pSkillProp && pSkillProp->nLog != 1 && dwSkill != NULL_ID )
		{
			if( m_atexSkill[ i ] && CheckSkill( i ) && 0 < pSkill->dwLevel  && ( pSkillProp->dwItemKind1 == m_cActiveJobType || ( pSkillProp->dwItemKind1 == JTYPE_MASTER || pSkillProp->dwItemKind1 == JTYPE_HERO) ))
			{
				CRect rect;
				if( GetSkillPoint( pSkillProp->dwID, rect ) )
				{
					rect.top -= 2;
					m_atexSkill[ i ]->Render( p2DRender, rect.TopLeft(), CPoint( 27, 27 ) );
					int nAddNum = 0;
					LPSKILL pSkillUser = g_pPlayer->GetSkill( pSkill->dwSkill );
					if( pSkillUser && pSkill->dwLevel != pSkillUser->dwLevel )
						nAddNum = 20;
					if( pSkill->dwLevel < pSkillProp->dwExpertMax )
						m_textPackNum.Render( p2DRender, rect.TopLeft()-CPoint(2,2), pSkill->dwLevel-1 + nAddNum );
					else
						m_textPackNum.Render( p2DRender, rect.TopLeft()-CPoint(2,2), 19 + nAddNum );
				}
			}	
			else if( m_dwMouseSkill == dwSkill && m_atexSkill[ i ] && CheckSkill( i ) )
			{
				CRect rect;
				if( GetSkillPoint( pSkillProp->dwID, rect ) )
				{
					rect.top -= 2;
					m_atexSkill[ i ]->Render( p2DRender, rect.TopLeft(), CPoint( 27, 27 ) );				
				}
			}
			if( m_pFocusItem && m_pFocusItem->dwSkill == dwSkill )
			{
				CRect rect;
				if( GetSkillPoint( pSkillProp->dwID, rect ) )
				{
					rect.top -= 3;
					rect.left -= 3;
					if( pSkillProp->dwItemKind1 == m_cActiveJobType || ( pSkillProp->dwItemKind1 == JTYPE_MASTER || pSkillProp->dwItemKind1 == JTYPE_HERO) )
						m_texSelectSkill->Render( p2DRender, rect.TopLeft() , CPoint( 27, 27 ), 255UL, 1.1f, 1.1f );
				}
			} 
		}
	}

	CWndStatic* lpWndStatic9 = (CWndStatic*)GetDlgItem( WIDC_STATIC_SP ); // current skill points
	CString strSP;
	strSP.Format( "%d", m_nCurrSkillPoint );
	lpWndStatic9->SetTitle( strSP );

	if( m_pFocusItem && g_nSkillCurSelect > -1 )
	{
		if( m_pFocusItem->dwSkill == -1 )
		{
			CString str;
			str.Format( "Impossible Skill!!! Player Job %d, Focused Skill %d , Level %d, CurSelect %d", g_pPlayer->GetJob(), m_pFocusItem->dwSkill, m_pFocusItem->dwLevel, g_nSkillCurSelect );
			ADDERRORMSG( str );
		}

		lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 ); // Current selected Skill
		m_atexSkill[ g_nSkillCurSelect ]->Render( p2DRender, lpWndCtrl->rect.TopLeft(), CPoint( 30, 30 ), 255, 1.2f, 1.2f );


		ItemProp* pSkillProp = prj.GetSkillProp( m_pFocusItem->dwSkill );

		if( pSkillProp && 0 < m_pFocusItem->dwLevel )
		{
			int nAddNum = 0;
			LPSKILL pSkillUser = g_pPlayer->GetSkill( m_pFocusItem->dwSkill );
			if( pSkillUser && m_pFocusItem->dwLevel != pSkillUser->dwLevel )
				nAddNum = 20;

			CPoint pt = lpWndCtrl->rect.TopLeft();
			pt.x += 5;
			pt.y += 5;

			if( m_pFocusItem->dwLevel < pSkillProp->dwExpertMax )
				m_textPackNum.Render( p2DRender, pt, m_pFocusItem->dwLevel-1 + nAddNum );
			else
				m_textPackNum.Render( p2DRender, pt, 19 + nAddNum );
		}
	}
}
void CWndSkillTreeEx::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	
	RestoreDeviceObjects();
	
	//m_texGauEmptyNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME,  "GauEmptyNormal.bmp" ), 0xffff00ff, TRUE );
	//m_texGauFillNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME,  "GauFillNormal.bmp" ), 0xffff00ff, TRUE );

	//m_aSkillLevel[ 0 ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME,  "ButtSkillLevelHold01.tga" ), 0xffff00ff );
	//m_aSkillLevel[ 1 ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME,  "ButtSkillLevelup01.tga" ), 0xffff00ff );
	//m_aSkillLevel[ 2 ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME,  "ButtSkillLevelHold02.tga" ), 0xffff00ff );
	
	InitItem( g_pPlayer->GetJob(), g_pPlayer->m_aJobSkill );

	m_atexTopDown[ 0 ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME,  "LvUp.bmp" ), 0xffff00ff );
	m_atexTopDown[ 1 ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME,  "LvDn.bmp" ), 0xffff00ff );
	m_texSelectSkill = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "WndPosMark.tga" ), 0xffff00ff );

	if( g_nSkillCurSelect >= 0 )
		m_pFocusItem = &m_apSkill[ g_nSkillCurSelect ];

	m_pWndButton[0] = (CWndButton*) GetDlgItem( WIDC_BUTTON_PLUS );	// + Skill Level
	m_pWndButton[1] = (CWndButton*) GetDlgItem( WIDC_BUTTON_MINUS );	// - Skill Level
	m_pWndButton[2] = (CWndButton*) GetDlgItem( WIDC_BUTTON_BACK );	// Reset Distribution
	m_pWndButton[3] = (CWndButton*) GetDlgItem( WIDC_BUTTON_OK );	// Finish Distribution

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rect = GetWindowRect();
	int x = rectRoot.right - rect.Width();
	int y = rectRoot.bottom - rect.Height();

	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_STATIC_PANNEL );
	if(lpWndCtrl)
	{
		m_pointTreePos = lpWndCtrl->rect.TopLeft();
		m_pointTreePos.y += 4;
	}

	CPoint point( x, y );
	Move( point );
}
BOOL CWndSkillTreeEx::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint( 792, 130 ), pWndParent );
}
BOOL CWndSkillTreeEx::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	if( m_pFocusItem && 0 < g_pPlayer->m_nSkillPoint )
	{
		ItemProp* pSkillProp = prj.GetSkillProp( m_pFocusItem->dwSkill );
		if( pSkillProp )
		{
			int nPoint = prj.GetSkillPoint( pSkillProp );
			switch( nID )
			{
			case WIDC_BUTTON_PLUS:	// + skill level
				{
					if( nPoint <= m_nCurrSkillPoint )
					{
						if( m_pFocusItem->dwLevel < pSkillProp->dwExpertMax )
						{
							m_nCurrSkillPoint -= nPoint;
							++m_pFocusItem->dwLevel;
						}
						else
						{
							break;							
						}
					}					
				}
				break;
			case WIDC_BUTTON_MINUS:	// - skill level
				{
					if( IsDownPoint(m_pFocusItem->dwSkill) )
					{
						m_nCurrSkillPoint += nPoint;
						--m_pFocusItem->dwLevel;
						SubSkillPointDown( m_pFocusItem );
					}
				}
				break;
			case WIDC_BUTTON_BACK:	// Reset
				{
					if( m_nCurrSkillPoint != g_pPlayer->m_nSkillPoint )
						InitItem(g_pPlayer->GetJob(), g_pPlayer->m_aJobSkill, TRUE );
					m_pFocusItem = NULL;
				}
				break;
			case WIDC_BUTTON_OK:	// Finish
				{
					if( m_nCurrSkillPoint != g_pPlayer->m_nSkillPoint )
					{
						SAFE_DELETE(g_WndMng.m_pWndReSkillWarning);
						g_WndMng.m_pWndReSkillWarning = new CWndReSkillWarning;
						g_WndMng.m_pWndReSkillWarning->Initialize(NULL);
					}
				}
				break;
			}
		}
	}

	switch(nID)
	{
		case WIDC_BUTTON_C1:SetSlot(0);break; // reset selection;break;
		case WIDC_BUTTON_C2:SetSlot(1);break;
		case WIDC_BUTTON_C3:SetSlot(2);break;
		case WIDC_BUTTON_C4:SetSlot(3);break;
	}

	if( nID == 10000 )
	{
		if( m_nCurrSkillPoint != g_pPlayer->m_nSkillPoint )
		{
			CWndBase* pWndBase = (CWndBase*)g_WndMng.GetWndBase( APP_QUEITMWARNING );	
			if( pWndBase == NULL )
			{
				SAFE_DELETE(g_WndMng.m_pWndReSkillWarning);
				g_WndMng.m_pWndReSkillWarning = new CWndReSkillWarning;
				g_WndMng.m_pWndReSkillWarning->Initialize(NULL);
				g_WndMng.m_pWndReSkillWarning->InitItem( TRUE );
			}
			return TRUE;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
BOOL CWndSkillTreeEx::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndSkillTreeEx::SetSlot( char cSlot )
{
	if(cSlot > 3)
		cSlot = 0;

	CWndButton* pWndBtn;
	switch(m_cActiveSlot)
	{
	case 0: pWndBtn = (CWndButton*)GetDlgItem(WIDC_BUTTON_C1);break;
	case 1: pWndBtn = (CWndButton*)GetDlgItem(WIDC_BUTTON_C2);break;
	case 2: pWndBtn = (CWndButton*)GetDlgItem(WIDC_BUTTON_C3);break;
	case 3: pWndBtn = (CWndButton*)GetDlgItem(WIDC_BUTTON_C4);break;
	}
	
	if(pWndBtn)
		pWndBtn->m_bPush = false;

	m_cActiveSlot = cSlot;
	char cSelJob = JOB_VAGRANT;
	switch(cSlot)
	{
		case 0:
			m_cActiveJobType = JTYPE_BASE;
			break;
		case 1:
			m_cActiveJobType = JTYPE_EXPERT;
			switch(g_pPlayer->GetJob())
			{
				case JOB_MERCENARY:
				case JOB_KNIGHT:
				case JOB_KNIGHT_MASTER:
				case JOB_KNIGHT_HERO:
				case JOB_BLADE:
				case JOB_BLADE_MASTER:
				case JOB_BLADE_HERO:
				case JOB_LORDTEMPLER_HERO:
				case JOB_STORMBLADE_HERO:
					cSelJob = JOB_MERCENARY;
					break;
				case JOB_ASSIST:
				case JOB_RINGMASTER:
				case JOB_RINGMASTER_MASTER:
				case JOB_RINGMASTER_HERO:
				case JOB_BILLPOSTER:
				case JOB_BILLPOSTER_MASTER:
				case JOB_BILLPOSTER_HERO:
				case JOB_FLORIST_HERO:
				case JOB_FORCEMASTER_HERO:
					cSelJob = JOB_ASSIST;
					break;
				case JOB_MAGICIAN:
				case JOB_ELEMENTOR:
				case JOB_ELEMENTOR_MASTER:
				case JOB_ELEMENTOR_HERO:
				case JOB_PSYCHIKEEPER:
				case JOB_PSYCHIKEEPER_MASTER:
				case JOB_PSYCHIKEEPER_HERO:
				case JOB_MENTALIST_HERO:
				case JOB_ELEMENTORLORD_HERO:
					cSelJob = JOB_MAGICIAN;
					break;
				case JOB_ACROBAT:
				case JOB_RANGER:
				case JOB_RANGER_MASTER:
				case JOB_RANGER_HERO:
				case JOB_JESTER:
				case JOB_JESTER_MASTER:
				case JOB_JESTER_HERO:
				case JOB_WINDLURKER_HERO:
				case JOB_CRACKSHOOTER_HERO:
					cSelJob = JOB_ACROBAT;
					break;
			}
			break;
		case 2:
			m_cActiveJobType = JTYPE_PRO;
			switch(g_pPlayer->GetJob())
			{
				case JOB_KNIGHT:
				case JOB_KNIGHT_MASTER:
				case JOB_KNIGHT_HERO:
				case JOB_LORDTEMPLER_HERO:
					cSelJob = JOB_KNIGHT;
					break;
				case JOB_BLADE:
				case JOB_BLADE_MASTER:
				case JOB_BLADE_HERO:
				case JOB_STORMBLADE_HERO:
					cSelJob = JOB_BLADE;
					break;
				case JOB_RINGMASTER:
				case JOB_RINGMASTER_MASTER:
				case JOB_RINGMASTER_HERO:
				case JOB_FLORIST_HERO:
					cSelJob = JOB_RINGMASTER;
					break;
				case JOB_BILLPOSTER:
				case JOB_BILLPOSTER_MASTER:
				case JOB_BILLPOSTER_HERO:
				case JOB_FORCEMASTER_HERO:
					cSelJob = JOB_BILLPOSTER;
					break;
				case JOB_ELEMENTOR:
				case JOB_ELEMENTOR_MASTER:
				case JOB_ELEMENTOR_HERO:
				case JOB_ELEMENTORLORD_HERO:
					cSelJob = JOB_ELEMENTOR;
					break;
				case JOB_PSYCHIKEEPER:
				case JOB_PSYCHIKEEPER_MASTER:
				case JOB_PSYCHIKEEPER_HERO:
				case JOB_MENTALIST_HERO:
					cSelJob = JOB_PSYCHIKEEPER;
					break;
				case JOB_RANGER:
				case JOB_RANGER_MASTER:
				case JOB_RANGER_HERO:
				case JOB_CRACKSHOOTER_HERO:
					cSelJob = JOB_RANGER;
					break;
				case JOB_JESTER:
				case JOB_JESTER_MASTER:
				case JOB_JESTER_HERO:
				case JOB_WINDLURKER_HERO:
					cSelJob = JOB_JESTER;
					break;
			}
			break;
		case 3:
			m_cActiveJobType = JTYPE_LEGEND_HERO;
			switch(g_pPlayer->GetJob())
			{
				case JOB_LORDTEMPLER_HERO:
					cSelJob = JOB_LORDTEMPLER_HERO;
					break;
				case JOB_STORMBLADE_HERO:
					cSelJob = JOB_STORMBLADE_HERO;
					break;
				case JOB_WINDLURKER_HERO:
					cSelJob = JOB_WINDLURKER_HERO;
					break;
				case JOB_CRACKSHOOTER_HERO:
					cSelJob = JOB_CRACKSHOOTER_HERO;
					break;
				case JOB_FLORIST_HERO:
					cSelJob = JOB_FLORIST_HERO;
					break;
				case JOB_FORCEMASTER_HERO:
					cSelJob = JOB_FORCEMASTER_HERO;
					break;
				case JOB_MENTALIST_HERO:
					cSelJob = JOB_MENTALIST_HERO;
					break;
				case JOB_ELEMENTORLORD_HERO:
					cSelJob = JOB_ELEMENTORLORD_HERO;
					break;
			}
			break;
	}

	CWndStatic* pStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC_CLASSNAME);
	if(pStatic)
		pStatic->SetTitle(prj.m_aJob[cSelJob].szName);
}
void CWndSkillTreeEx::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bDrag = FALSE;
}
void CWndSkillTreeEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( m_apSkill == NULL )
		return;

	CPoint ptMouse = GetMousePoint();
				
	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
	{
		LPSKILL pSkill = GetSkill( i );
		if( pSkill == NULL ) 
			continue;
		DWORD dwSkill = pSkill->dwSkill;

		ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );

		if( pSkillProp && pSkillProp->nLog != 1 && dwSkill != NULL_ID &&  ( pSkillProp->dwItemKind1 == m_cActiveJobType || ( pSkillProp->dwItemKind1 == JTYPE_MASTER || pSkillProp->dwItemKind1 == JTYPE_HERO ) ))
		{
			CRect rect;
			if( GetSkillPoint( pSkillProp->dwID, rect ) && CheckSkill( i ) )
			{
				rect.top  -= 2;
				
				rect.right  = rect.left+27;
				rect.bottom = rect.top+27;
				
				if( rect.PtInRect(ptMouse) )
				{
					g_nSkillCurSelect = i;
					m_pFocusItem = &m_apSkill[ i ];
					m_bDrag = TRUE;
					break;
				}			
			}	
		}
	}
}
void CWndSkillTreeEx::OnLButtonDblClk( UINT nFlags, CPoint point)
{	
	if( m_apSkill == NULL )
		return;

	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
	{
		LPSKILL pSkill = GetSkill( i );
		if( pSkill == NULL ) 
			continue;
		DWORD dwSkill = pSkill->dwSkill;
		
		ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );

		if( pSkillProp && pSkillProp->nLog != 1 && dwSkill != NULL_ID &&  ( pSkillProp->dwItemKind1 == m_cActiveJobType || ( pSkillProp->dwItemKind1 == JTYPE_MASTER || pSkillProp->dwItemKind1 == JTYPE_HERO ) ))
		{
			CRect rect;
			if( GetSkillPoint( pSkillProp->dwID, rect ) && CheckSkill( i ) )
			{
				rect.top  -= 2;
				
				rect.right  = rect.left+27;
				rect.bottom = rect.top+27;
				
				CPoint ptMouse = GetMousePoint();

				
				if( rect.PtInRect(ptMouse) )
				{
					g_nSkillCurSelect = i;
					m_pFocusItem = &m_apSkill[ i ];

					if( g_pPlayer->CheckSkill( m_pFocusItem->dwSkill ) == FALSE )
						return;
					CWndTaskBar* pTaskBar = g_WndMng.m_pWndTaskBar;
					if( pTaskBar->m_nExecute == 0 )		// 스킬큐가 사용되지 않을때만 등록됨.
						pTaskBar->SetSkillQueue( pTaskBar->m_nCurQueueNum, 0, i, m_atexSkill[i] );
					break;
				}		
			}	
		}
	}
}
#else // __NEW_SKILL_TREE
CWndSkillTreeEx::CWndSkillTreeEx()
{
	m_nCount     = 0;
//	m_nCurSelect = -1;
	m_pFocusItem = NULL;
	m_bDrag = FALSE;
	m_apSkill = NULL;
	m_nJob = -1;
	m_pVBGauge = NULL;

	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
		m_atexSkill[ i ] = NULL;

	m_aSkillLevel[ 0 ] = NULL;
	m_aSkillLevel[ 1 ] = NULL;
	m_aSkillLevel[ 2 ] = NULL;

	m_atexJobPannel[ 0 ] = NULL;
	m_atexJobPannel[ 1 ] = NULL;

	m_atexTopDown[0] = NULL;
	m_atexTopDown[1] = NULL;
	m_nTopDownGap = 0;
	m_bSlot[0] = TRUE;
	m_bSlot[1] = TRUE;
	m_bSlot[2] = TRUE;
#if __VER >= 10 //__CSC_VER9_1
	m_bSlot[3] = TRUE;
	m_bLegend = FALSE;
#endif //__CSC_VER9_1

	m_nCurrSkillPoint = 0;
}
CWndSkillTreeEx::~CWndSkillTreeEx()
{
	DeleteDeviceObjects();
}

void CWndSkillTreeEx::SerializeRegInfo( CAr& ar, DWORD& dwVersion )
{
	CWndNeuz::SerializeRegInfo( ar, dwVersion );
	if( ar.IsLoading() )
	{
#if __VER >= 10 // __CSC_VER9_1
		ar >> m_bSlot[0] >> m_bSlot[1] >> m_bSlot[2] >> m_bSlot[3];
		BOOL bFlag[4];

		bFlag[0] = m_bSlot[0];
		bFlag[1] = m_bSlot[1];
		bFlag[2] = m_bSlot[2];
		bFlag[3] = m_bSlot[3];
		
		m_bSlot[0] = m_bSlot[1] = m_bSlot[2] = m_bSlot[3] = TRUE;
		
		SetActiveSlot( 0, bFlag[0] );
		SetActiveSlot( 1, bFlag[1] );
		SetActiveSlot( 2, bFlag[2] );
		if(m_bLegend)
		{
			if(FULLSCREEN_HEIGHT == 600)
			{
				if(bFlag[3])
					SetActiveSlot( 2, FALSE );
			}
			else
				SetActiveSlot( 3, bFlag[3] );
		}
#else //__CSC_VER9_1
		ar >> m_bSlot[0] >> m_bSlot[1] >> m_bSlot[2];
		BOOL bFlag[3];

		bFlag[0] = m_bSlot[0];
		bFlag[1] = m_bSlot[1];
		bFlag[2] = m_bSlot[2];
		
		m_bSlot[0] = m_bSlot[1] = m_bSlot[2] = TRUE;

		SetActiveSlot( 0, bFlag[0] );
		SetActiveSlot( 1, bFlag[1] );
		SetActiveSlot( 2, bFlag[2] );
#endif //__CSC_VER9_1
	}
	else
	{
		dwVersion = 0;
#if __VER >= 10 // __CSC_VER9_1
		ar << m_bSlot[0] << m_bSlot[1] << m_bSlot[2] << m_bSlot[3];
#else
		ar << m_bSlot[0] << m_bSlot[1] << m_bSlot[2] ;
#endif //__CSC_VER9_1
	}
}

void CWndSkillTreeEx::SetActiveSlot(int nSlot, BOOL bFlag)
{
	CRect rect = GetWindowRect(TRUE);

	if( nSlot == 0 )
	{
		if( m_bSlot[0] == bFlag )
			return;

		CWndStatic* lpWndStatic;
		lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC3 );

		DWORD dwValue = 48;

		if( bFlag )
		{
			CWndStatic* lpWndStatic;
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC3 );
			CRect crect = lpWndStatic->GetWindowRect(TRUE);
			crect.OffsetRect( 0, dwValue );
			lpWndStatic->SetWndRect( crect, TRUE );

			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC4 );
			lpWndStatic->SetVisible(TRUE);
			
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC5 );
			crect = lpWndStatic->GetWindowRect(TRUE);
			lpWndStatic->Move( crect.left, crect.top+dwValue );
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC7 );
			crect = lpWndStatic->GetWindowRect(TRUE);
			lpWndStatic->Move( crect.left, crect.top+dwValue );
			
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC8 );
			lpWndStatic->SetVisible(TRUE);
			
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC6 );
			crect = lpWndStatic->GetWindowRect(TRUE);
			lpWndStatic->Move( crect.left, crect.top+dwValue );
#if __VER >= 10 // __CSC_VER9_1
			if(m_bLegend)
			{
				crect = m_pWndHeroStatic[0]->GetWindowRect(TRUE);
				m_pWndHeroStatic[0]->Move( crect.left, crect.top+dwValue );	
				
				crect = m_pWndHeroStatic[1]->GetWindowRect(TRUE);
				m_pWndHeroStatic[1]->Move( crect.left, crect.top+dwValue );	
			}
#endif //__CSC_VER9_1			
			int n = rect.Height();
			rect.bottom += dwValue;
		}
		else
		{
			CWndStatic* lpWndStatic;
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC3 );
			CRect crect = lpWndStatic->GetWindowRect(TRUE);
			crect.OffsetRect( 0, ((-1) * (int)( dwValue )) );
			lpWndStatic->SetWndRect( crect, TRUE );
			
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC4 );
			lpWndStatic->EnableWindow(FALSE);
			lpWndStatic->SetVisible(FALSE);

			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC5 );
			crect = lpWndStatic->GetWindowRect(TRUE);
			lpWndStatic->Move( crect.left, crect.top-dwValue );
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC7 );
			crect = lpWndStatic->GetWindowRect(TRUE);
			lpWndStatic->Move( crect.left, crect.top-dwValue );
			
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC8 );
			lpWndStatic->EnableWindow(FALSE);
			lpWndStatic->SetVisible(FALSE);

			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC6 );
			crect = lpWndStatic->GetWindowRect(TRUE);
			lpWndStatic->Move( crect.left, crect.top-dwValue );

#if __VER >= 10 // __CSC_VER9_1
			if(m_bLegend)
			{
				crect = m_pWndHeroStatic[0]->GetWindowRect(TRUE);
				m_pWndHeroStatic[0]->Move( crect.left, crect.top-dwValue );
				
				crect = m_pWndHeroStatic[1]->GetWindowRect(TRUE);
				m_pWndHeroStatic[1]->Move( crect.left, crect.top-dwValue );
			}
#endif //__CSC_VER9_1	

			int n = rect.Height();
			rect.bottom -= dwValue;

		}

		SetWndRect(rect);

		m_bSlot[0] = bFlag;
	}
	else
	if( nSlot == 1 )
	{
		if( m_bSlot[1] == bFlag )
			return;

		DWORD dwValue = 144;
		
		if( bFlag )
		{
			CWndStatic* lpWndStatic;
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC5 );
			lpWndStatic->SetVisible(TRUE);
			
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC6 );
			CRect crect = lpWndStatic->GetWindowRect(TRUE);
			lpWndStatic->Move( crect.left, crect.top+dwValue );
			
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC7 );
			crect = lpWndStatic->GetWindowRect(TRUE);
			lpWndStatic->Move( crect.left, crect.top+dwValue );

#if __VER >= 10 // __CSC_VER9_1
			if(m_bLegend)
			{
				crect = m_pWndHeroStatic[0]->GetWindowRect(TRUE);
				m_pWndHeroStatic[0]->Move( crect.left, crect.top+dwValue );
				
				crect = m_pWndHeroStatic[1]->GetWindowRect(TRUE);
				m_pWndHeroStatic[1]->Move( crect.left, crect.top+dwValue );
			}
#endif //__CSC_VER9_1	

			rect.bottom += dwValue;
		}
		else
		{
			CWndStatic* lpWndStatic;
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC5 );
			lpWndStatic->SetVisible(FALSE);
			
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC6 );
			CRect crect = lpWndStatic->GetWindowRect(TRUE);
			lpWndStatic->Move( crect.left, crect.top-dwValue );

			
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC7 );
			crect = lpWndStatic->GetWindowRect(TRUE);
			lpWndStatic->Move( crect.left, crect.top-dwValue );

#if __VER >= 10 // __CSC_VER9_1
			if(m_bLegend)
			{
				crect = m_pWndHeroStatic[0]->GetWindowRect(TRUE);
				m_pWndHeroStatic[0]->Move( crect.left, crect.top-dwValue );
				
				crect = m_pWndHeroStatic[1]->GetWindowRect(TRUE);
				m_pWndHeroStatic[1]->Move( crect.left, crect.top-dwValue );
			}
#endif //__CSC_VER9_1
			
			rect.bottom -= dwValue;
		}
		
		SetWndRect(rect);
		
		m_bSlot[1] = bFlag;
	}
	else
	if( nSlot == 2 )
	{
		if( m_bSlot[2] == bFlag )
			return;

		DWORD dwValue = 144;
		
		if( bFlag )
		{
			CWndStatic* lpWndStatic;
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC7 );
			lpWndStatic->SetVisible(TRUE);

#if __VER >= 10 // __CSC_VER9_1
			if(m_bLegend)
			{
				CRect crect = m_pWndHeroStatic[0]->GetWindowRect(TRUE);
				m_pWndHeroStatic[0]->Move( crect.left, crect.top+dwValue );
				
				crect = m_pWndHeroStatic[1]->GetWindowRect(TRUE);
				m_pWndHeroStatic[1]->Move( crect.left, crect.top+dwValue );
			}
#endif //__CSC_VER9_1
			
			rect.bottom += dwValue;
		}
		else
		{
			CWndStatic* lpWndStatic;
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC7 );
			lpWndStatic->SetVisible(FALSE);

#if __VER >= 10 // __CSC_VER9_1
			if(m_bLegend)
			{
				CRect crect = m_pWndHeroStatic[0]->GetWindowRect(TRUE);
				m_pWndHeroStatic[0]->Move( crect.left, crect.top-dwValue );
				
				crect = m_pWndHeroStatic[1]->GetWindowRect(TRUE);
				m_pWndHeroStatic[1]->Move( crect.left, crect.top-dwValue );
			}
#endif //__CSC_VER9_1
			
			rect.bottom -= dwValue;
		}
		
		SetWndRect(rect);
		
		m_bSlot[2] = bFlag;
	}
#if __VER >= 10 // __CSC_VER9_1
	else if(nSlot == 3)
	{
		if( m_bSlot[3] == bFlag )
			return;
		
		DWORD dwValue = 48;
		
		if( bFlag )
		{
			m_pWndHeroStatic[1]->SetVisible(TRUE);
			rect.bottom += dwValue;
		}
		else
		{
			m_pWndHeroStatic[1]->SetVisible(FALSE);
			rect.bottom -= dwValue;
		}
		
		SetWndRect(rect);
		
		m_bSlot[3] = bFlag;
	}
#endif //__CSC_VER9_1

	if( rect.bottom > (int)( FULLSCREEN_HEIGHT-48 ) )
	{
		rect.OffsetRect( 0, (int)(FULLSCREEN_HEIGHT-48)-rect.bottom );
		SetWndRect(rect);
	}
		
	AdjustWndBase();
}


BOOL CWndSkillTreeEx::IsDownPoint(DWORD skill)
{
	LPSKILL pSkill = GetdwSkill( skill );
	if( pSkill == NULL )
		return FALSE;

	LPSKILL pSkillMy = g_pPlayer->GetSkill( pSkill->dwSkill );
	if( pSkillMy == NULL )
		return FALSE;

	if(  pSkill->dwLevel <= pSkillMy->dwLevel )
		return FALSE;

	return TRUE;
}

void CWndSkillTreeEx::SubSkillPointDown( LPSKILL lpSkill )
{
	for( int i = 0; i < MAX_SKILL_JOB; i++ )	
	{
		if( m_apSkill[i].dwSkill == NULL_ID )
			continue;
		
		LPSKILL pSkill2 = &m_apSkill[i];
		
		if( pSkill2 == NULL )
			continue;
		
		if( pSkill2->GetProp()->dwReSkill1 == lpSkill->dwSkill )
		{
			if( pSkill2->GetProp()->dwReSkillLevel1 )
			{
				if( lpSkill->dwLevel < pSkill2->GetProp()->dwReSkillLevel1 )
				{
					if( pSkill2->dwLevel > 0 )
					{
						m_nCurrSkillPoint += ( prj.GetSkillPoint( pSkill2->GetProp() ) * pSkill2->dwLevel );
						pSkill2->dwLevel = 0;
						SubSkillPointDown( pSkill2 );
					}
				}
			}
		}
		
		if( pSkill2->GetProp()->dwReSkill2 == lpSkill->dwSkill )
		{
			if( pSkill2->GetProp()->dwReSkillLevel2 )
			{
				if( lpSkill->dwLevel < pSkill2->GetProp()->dwReSkillLevel2 )
				{
					if( pSkill2->dwLevel > 0 )
					{
						m_nCurrSkillPoint += ( prj.GetSkillPoint( pSkill2->GetProp() ) * pSkill2->dwLevel );
						pSkill2->dwLevel = 0;
						SubSkillPointDown( pSkill2 );
					}
				}
			}
		}
	}
}

LPSKILL CWndSkillTreeEx::GetdwSkill( DWORD dwSkill )
{
	for( int i = 0; i < MAX_SKILL_JOB; i++ )	
	{
		if( m_apSkill[i].dwSkill == dwSkill )
			return &m_apSkill[i];
	}
	return NULL;
}

LPSKILL CWndSkillTreeEx::GetSkill( int i )
{
	LPSKILL pSkill = &m_apSkill[ i ];
	
	if( pSkill->dwSkill == NULL_ID ) 
		return NULL;
	if( pSkill->GetProp() == NULL )
	{
		Error( "GetSkill Error Not Prop : %d", i );
		return NULL;
	}
	
	DWORD dwReqDislLV = pSkill->GetProp()->dwReqDisLV;
	return pSkill;
}
void CWndSkillTreeEx::OnSize(UINT nType, int cx, int cy)
{
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 50;
	CSize size = CSize(70,25);//m_pSprPack->GetAt(9)->GetSize();
	
	CRect rect1_1( x - ( size.cx / 2), y, ( x - ( size.cx / 2 ) ) + size.cx, y + size.cy );
	
	CRect rect2_1( x - size.cx - 10, y, ( x - size.cx - 10 ) + size.cx, y + size.cy );
	CRect rect2_2( x + 10          , y, ( x + 10           ) + size.cx, y + size.cy );
	
	CRect rect3_1( x - ( size.cx / 2) - size.cx - 10, y, (x - ( size.cx / 2) - size.cx - 10) + size.cx, y + size.cy );
	CRect rect3_2( x - ( size.cx / 2)               , y, (x - ( size.cx / 2)               ) + size.cx, y + size.cy );
	CRect rect3_3( x + ( size.cx / 2) + 10          , y, (x + ( size.cx / 2) + 10          ) + size.cx, y + size.cy );
	
	CRect rect = GetClientRect();
	rect.DeflateRect( 4, 4 );
	//rect.top += 40;
	//m_wndListCtrl.SetWndRect( rect );
	//	CWndTabCtrl* lpTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	
	//rect.bottom += 20;
	//	lpTabCtrl->SetWndRect( rect );
	
	//for( int i = 0; i < 2; i++ )
	//{
	//	m_wndSkillCtrl[ i ].SetWndRect( rect );
	//}
	CWndNeuz::OnSize(nType,cx,cy);
}

BOOL CWndSkillTreeEx::CheckSkill( int i )
{
	LPSKILL pSkill = &m_apSkill[ i ];

	if( pSkill->dwSkill == NULL_ID || g_pPlayer == NULL ) 
		return FALSE;
	
	DWORD dwSkill = pSkill->dwSkill;
	
	ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );

	if( pSkillProp == NULL || pSkillProp->nLog == 1 )
		return FALSE;

#if __VER >= 10 // __LEGEND	//	9차 전승시스템	Neuz, World, Trans
#ifdef __3RD_LEGEND16
		if( g_pPlayer->GetLevel() < (int)( pSkillProp->dwReqDisLV ) && !g_pPlayer->IsMaster() && !g_pPlayer->IsHero() && !g_pPlayer->IsLegendHero() )
		return FALSE;
#else // __3RD_LEGEND16
		if( g_pPlayer->GetLevel() < (int)( pSkillProp->dwReqDisLV ) && !g_pPlayer->IsMaster() && !g_pPlayer->IsHero() )
		return FALSE;
#endif // __3RD_LEGEND16
#else //__LEGEND	//	9차 전승시스템	Neuz, World, Trans
	if( g_pPlayer->GetLevel() < pSkillProp->dwReqDisLV )
		return FALSE;
#endif	//__LEGEND	//	9차 전승시스템	Neuz, World, Trans
				
	if( pSkillProp->dwReSkill1 != 0xffffffff )
	{
		LPSKILL pSkillBuf = GetdwSkill( pSkillProp->dwReSkill1 );

		if( pSkillBuf )
		{
			if( pSkillBuf->dwLevel < pSkillProp->dwReSkillLevel1 )
			{
				return FALSE;
			}
		}
		else
		{
		}
	}
	
	if( pSkillProp->dwReSkill2 != 0xffffffff )
	{
		LPSKILL pSkillBuf = GetdwSkill( pSkillProp->dwReSkill2 );
		if( pSkillBuf )
		{
			if( pSkillBuf->dwLevel < pSkillProp->dwReSkillLevel2 )
			{
				return FALSE;
			}				
		}
		else
		{
		}
	}	
	
	return TRUE;
}

HRESULT CWndSkillTreeEx::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if( m_pVBGauge == NULL )
		return m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBGauge, NULL );
	return S_OK;
}
HRESULT CWndSkillTreeEx::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
    SAFE_RELEASE( m_pVBGauge );
	return S_OK;
}
HRESULT CWndSkillTreeEx::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	SAFE_RELEASE( m_pVBGauge );

	SAFE_DELETE( m_atexJobPannel[0] );
	SAFE_DELETE( m_atexJobPannel[1] );
	
	if( g_pPlayer->m_nSkillPoint > 0 )
	{
		safe_delete_array( m_apSkill );
	}
	else
		m_apSkill = NULL;
	
	return S_OK;
}

void CWndSkillTreeEx::InitItem(int nJob, LPSKILL apSkill, BOOL bReset )
{
	if( g_pPlayer == NULL )
		return;

	if( g_pPlayer->m_nSkillPoint > 0 )
	{
		m_nCurrSkillPoint = g_pPlayer->m_nSkillPoint;
		m_apSkill = new SKILL[MAX_SKILL_JOB];
		memcpy( m_apSkill, apSkill, sizeof(SKILL)*MAX_SKILL_JOB );
	}
	else
	{
		m_nCurrSkillPoint = 0;
		m_apSkill = apSkill;
	}

	m_pFocusItem = NULL;
	m_nJob = nJob;

	if( m_apSkill == NULL )
		return;


	m_nCount = 0;
	CString strTex[2];
	
	CWndStatic* lpWndStatic1 = (CWndStatic*) GetDlgItem( WIDC_STATIC2 );
	CWndStatic* lpWndStatic2 = (CWndStatic*) GetDlgItem( WIDC_STATIC3 );
	CWndStatic* lpWndStatic3 = (CWndStatic*) GetDlgItem( WIDC_STATIC6 );

	switch( m_nJob )
	{
	case JOB_VAGRANT:
		m_nCount  = 3;
		strTex[0] = "";//"Back_Upper.TGA";
		strTex[1] = "";//"Back_Lower.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( "" );
		lpWndStatic3->SetTitle( "" );
		break;			
	case JOB_KNIGHT:
#if __VER >= 10 // __CSC_VER9_1
	case JOB_KNIGHT_MASTER:
	case JOB_KNIGHT_HERO:
#endif //__CSC_VER9_1
		m_nCount  = 26;
		strTex[0] = "Back_Me.TGA";
		strTex[1] = "Back_Night.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_MERCENARY].szName );
		lpWndStatic3->SetTitle( prj.m_aJob[JOB_KNIGHT].szName );
		break;			
	case JOB_BLADE:
#if __VER >= 10 // __CSC_VER9_1
	case JOB_BLADE_MASTER:
	case JOB_BLADE_HERO:
#endif //__CSC_VER9_1
		m_nCount  = 26;
		strTex[0] = "Back_Me.TGA";
		strTex[1] = "Back_Blade.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_MERCENARY].szName );
		lpWndStatic3->SetTitle( prj.m_aJob[JOB_BLADE].szName );
		break;			
	case JOB_MERCENARY:
		m_nCount  = 18;
		strTex[0] = "Back_Me.TGA";
		strTex[1] = "";//"Back_Lower.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_MERCENARY].szName );
		lpWndStatic3->SetTitle( "" );
		break;			
	case JOB_BILLPOSTER:
#if __VER >= 10 // __CSC_VER9_1
	case JOB_BILLPOSTER_MASTER:
	case JOB_BILLPOSTER_HERO:
#endif //__CSC_VER9_1
		m_nCount  = 28;
		strTex[0] = "Back_As.TGA";
		strTex[1] = "Back_Bill.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_ASSIST].szName );
		lpWndStatic3->SetTitle( prj.m_aJob[JOB_BILLPOSTER].szName );
		break;			
	case JOB_RINGMASTER:
#if __VER >= 10 // __CSC_VER9_1
	case JOB_RINGMASTER_MASTER:
	case JOB_RINGMASTER_HERO:
#endif //__CSC_VER9_1
		m_nCount  = 28;
		strTex[0] = "Back_As.TGA";
		strTex[1] = "Back_Ring.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_ASSIST].szName );
		lpWndStatic3->SetTitle( prj.m_aJob[JOB_RINGMASTER].szName );
		break;			
	case JOB_ASSIST:
		m_nCount  = 20;
		strTex[0] = "Back_As.TGA";
		strTex[1] = "";//"Back_Lower.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_ASSIST].szName );
		lpWndStatic3->SetTitle( "" );
		break;			
	case JOB_ELEMENTOR:
#if __VER >= 10 // __CSC_VER9_1
	case JOB_ELEMENTOR_MASTER:
	case JOB_ELEMENTOR_HERO:
#endif //__CSC_VER9_1
		m_nCount  = 39;
		strTex[0] = "Back_Ma.TGA";
		strTex[1] = "Back_Ele.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_MAGICIAN].szName );
		lpWndStatic3->SetTitle( prj.m_aJob[JOB_ELEMENTOR].szName );
		break;			
	case JOB_PSYCHIKEEPER:
#if __VER >= 10 // __CSC_VER9_1
	case JOB_PSYCHIKEEPER_MASTER:
	case JOB_PSYCHIKEEPER_HERO:
#endif //__CSC_VER9_1
		m_nCount  = 28;
		strTex[0] = "Back_Ma.TGA";
		strTex[1] = "Back_Psy.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_MAGICIAN].szName );
		lpWndStatic3->SetTitle( prj.m_aJob[JOB_PSYCHIKEEPER].szName );
		break;			
	case JOB_MAGICIAN:
		m_nCount  = 20;
		strTex[0] = "Back_Ma.TGA";
		strTex[1] = "";//"Back_Lower.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_MAGICIAN].szName );
		lpWndStatic3->SetTitle( "" );
		break;	
	case JOB_ACROBAT:
		m_nCount  = 20;
		strTex[0] = "Back_Acr.tga";
		strTex[1] = "";//"Back_Lower.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_ACROBAT].szName );
		lpWndStatic3->SetTitle( "" );
		break;	
	case JOB_JESTER:
#if __VER >= 10 // __CSC_VER9_1
	case JOB_JESTER_MASTER:
	case JOB_JESTER_HERO:
#endif //__CSC_VER9_1
		m_nCount  = 20;
		strTex[0] = "Back_Acr.tga";
		strTex[1] = "Back_Jst.TGA";//"Back_Lower.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_ACROBAT].szName );
		lpWndStatic3->SetTitle( prj.m_aJob[JOB_JESTER].szName );
		break;	
	case JOB_RANGER:
#if __VER >= 10 // __CSC_VER9_1
	case JOB_RANGER_MASTER:
	case JOB_RANGER_HERO:
#endif //__CSC_VER9_1
		m_nCount  = 20;
		strTex[0] = "Back_Acr.tga";
		strTex[1] = "Back_Rag.TGA";//"Back_Lower.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_ACROBAT].szName );
		lpWndStatic3->SetTitle( prj.m_aJob[JOB_RANGER].szName );
		break;
	default:
		Error( "CWndSkillTreeEx::InitItem 알수없는 직업 : %d", m_nJob );
		return;
	}
	
#if __VER >= 10 // __CSC_VER9_1
	//Master Skill은 시작부터 1Lv이므로 배경 이미지 제외.
	switch( m_nJob )
	{
		case JOB_KNIGHT_HERO:
			m_strHeroSkilBg = "Back_Hero_KntDrawing.tga";
			break;
		case JOB_BLADE_HERO:
			m_strHeroSkilBg = "Back_Hero_BldDefence.tga";
			break;
		case JOB_BILLPOSTER_HERO:
			m_strHeroSkilBg = "Back_Hero_BilDisEnchant.tga";
			break;
		case JOB_RINGMASTER_HERO:
			m_strHeroSkilBg = "Back_Hero_RigReturn.tga";
			break;
		case JOB_ELEMENTOR_HERO:
			m_strHeroSkilBg = "Back_Hero_EleCursemind.tga";
			break;
		case JOB_PSYCHIKEEPER_HERO:
			m_strHeroSkilBg = "Back_Hero_PsyStone.tga";
			break;
		case JOB_JESTER_HERO:
			m_strHeroSkilBg = "Back_Hero_JstSilence.tga";
			break;
		case JOB_RANGER_HERO:
			m_strHeroSkilBg = "Back_Hero_RagHawkeye.tga";
			break;
	}
#endif //__CSC_VER9_1

	SAFE_DELETE( m_atexJobPannel[0] );
	SAFE_DELETE( m_atexJobPannel[1] );

	if( !strTex[0].IsEmpty() )
	{
		m_atexJobPannel[0] = new IMAGE;
		if( LoadImage( MakePath( DIR_THEME, strTex[0] ), m_atexJobPannel[0] ) == FALSE )
			Error( "CWndSkillTreeEx::InitItem에서 %s Open1 실패", strTex[0] );

	}

	if( !strTex[1].IsEmpty() )
	{
		m_atexJobPannel[1] = new IMAGE;
		if( LoadImage( MakePath( DIR_THEME, strTex[1] ), m_atexJobPannel[1] ) == FALSE )
			Error( "CWndSkillTreeEx::InitItem에서 %s Open1 실패", strTex[1] );

	}
	AdjustWndBase();
	
	// 소스 아이템을 입력
	LoadTextureSkillicon();

#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
	g_nSkillCurSelect = -1;
#else // __IMPROVE_SYSTEM_VER15
	if( CheckSkill( 0 ))
		g_nSkillCurSelect = 0;
	else
		g_nSkillCurSelect = -1;
#endif // __IMPROVE_SYSTEM_VER15

	m_dwMouseSkill = NULL_ID;

	if( g_nSkillCurSelect >= 0 )
		m_pFocusItem = &m_apSkill[ g_nSkillCurSelect ];
}
BOOL CWndSkillTreeEx::GetSkillPoint(DWORD dwSkillID, CRect& rect )
{
	/*
	LPWNDCTRL lpWndCtrl;

	CPoint ptUpperAdd, ptLowerAdd;

	ptUpperAdd.x = 0;
	ptUpperAdd.y = 0;

	ptLowerAdd.x = 0;
	ptLowerAdd.y = 139;
	
	switch(dwSkillID)
	{
		case SI_VAG_ONE_CLEANHIT:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );
				rect = lpWndCtrl->rect;
			}
			break;
		case SI_VAG_ONE_BRANDISH:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM3 );
				rect = lpWndCtrl->rect;
			}
			break;
		case SI_VAG_ONE_OVERCUTTER:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM4 );
				rect = lpWndCtrl->rect;
			}
			break;

//머서너리
		case SI_MER_ONE_SPLMASH:		rect = CRect( 34, 109, 58, 133 ); break;
		case SI_MER_SHIELD_PROTECTION:	rect = CRect( 34, 161, 58, 185 ); break;
		case SI_MER_ONE_KEENWHEEL:		rect = CRect( 84, 97, 108, 121 ); break;
		case SI_MER_ONE_BLOODYSTRIKE:	rect = CRect( 84, 123, 108, 147 ); break;
		case SI_MER_SHIELD_PANBARRIER:	rect = CRect( 84, 149, 108, 173 ); break;
		case SI_MER_SUP_IMPOWERWEAPON:	rect = CRect( 84, 175, 108, 199 ); break;
		case SI_MER_ONE_BLINDSIDE:		rect = CRect( 134, 97, 158, 121 ); break;
		case SI_MER_ONE_REFLEXHIT:		rect = CRect( 134, 123, 158, 147 ); break;
		case SI_MER_ONE_SNEAKER:		rect = CRect( 134, 149, 158, 173 ); break;
		case SI_MER_SUP_SMITEAXE:		rect = CRect( 134, 175, 158, 199 ); break;
		case SI_MER_SUP_BLAZINGSWORD:	rect = CRect( 134, 201, 158, 225 ); break;
		case SI_MER_ONE_SPECIALHIT:		rect = CRect( 184, 97, 208, 121 ); break;
		case SI_MER_ONE_GUILOTINE:		rect = CRect( 184, 123, 208, 147 ); break;
		case SI_MER_SUP_AXEMASTER:		rect = CRect( 184, 175, 208, 199 ); break;
		case SI_MER_SUP_SWORDMASTER:	rect = CRect( 184, 201, 208, 225 ); break;
// 나이트
		case SI_KNT_TWOSW_CHARGE:		rect = CRect( 34, 260, 58, 284 ); break;
		case SI_KNT_TWOAX_PAINDEALER:	rect = CRect( 34, 286, 58, 310 ); break;
		case SI_KNT_SUP_GUARD:			rect = CRect( 34, 312, 58, 336 ); break;
		case SI_KNT_TWOSW_EARTHDIVIDER:	rect = CRect( 84, 260, 108, 284 ); break;
		case SI_KNT_TWOAX_POWERSTUMP:	rect = CRect( 84, 286, 108, 310 ); break;
		case SI_KNT_SUP_RAGE:			rect = CRect( 84, 312, 108, 336 ); break;
		case SI_KNT_TWO_POWERSWING:		rect = CRect( 134, 272, 158, 296 ); break;
		case SI_KNT_SUP_PAINREFLECTION:	rect = CRect( 134, 312, 158, 336 ); break;
// 블레이드
		case SI_BLD_DOUBLESW_SILENTSTRIKE:	rect = CRect( 34, 260, 58, 284 ); break;
		case SI_BLD_DOUBLEAX_SPRINGATTACK:	rect = CRect( 34, 286, 58, 310 ); break;
		case SI_BLD_DOUBLE_ARMORPENETRATE:	rect = CRect( 34, 312, 58, 336 ); break;
		case SI_BLD_DOUBLESW_BLADEDANCE:	rect = CRect( 84, 260, 108, 284 ); break;
		case SI_BLD_DOUBLEAX_HAWKATTACK:	rect = CRect( 84, 286, 108, 310 ); break;
		case SI_BLD_SUP_BERSERK:			rect = CRect( 84, 312, 108, 336 ); break;
		case SI_BLD_DOUBLE_CROSSSTRIKE:		rect = CRect( 134, 272, 158, 296 ); break;
		case SI_BLD_DOUBLE_SONICBLADE:		rect = CRect( 134, 312, 158, 336 ); break;
		
			
// 어시스트	
		case SI_ASS_HEAL_HEALING:			rect = CRect( 18, 136, 42, 160 ); break;
		case SI_ASS_HEAL_PATIENCE:			rect = CRect( 64, 110, 88, 134 ); break;
		case SI_ASS_CHEER_QUICKSTEP:		rect = CRect( 64, 136, 88, 160 ); break;
		case SI_ASS_CHEER_MENTALSIGN:		rect = CRect( 64, 162, 88, 186 ); break;
		case SI_ASS_KNU_TAMPINGHOLE:		rect = CRect( 64, 188, 88, 212 ); break;
		case SI_ASS_HEAL_RESURRECTION:		rect = CRect( 108, 110, 132, 134 ); break;
		case SI_ASS_CHEER_HASTE:			rect = CRect( 110, 136, 134, 160 ); break;
		case SI_ASS_CHEER_HEAPUP:			rect = CRect( 110, 162, 134, 186 ); break;
		case SI_ASS_CHEER_STONEHAND:		rect = CRect( 110, 188, 134, 212 ); break;		
		case SI_ASS_CHEER_CIRCLEHEALING:	rect = CRect( 156, 110, 180, 134 ); break;
		case SI_ASS_CHEER_CATSREFLEX:		rect = CRect( 156, 136, 180, 160 ); break;
		case SI_ASS_CHEER_BEEFUP:			rect = CRect( 156, 162, 180, 186 ); break;
		case SI_ASS_KNU_BURSTCRACK:			rect = CRect( 156, 188, 180, 212 ); break;		
		case SI_ASS_HEAL_PREVENTION:		rect = CRect( 202, 110, 226, 134 ); break;
		case SI_ASS_CHEER_CANNONBALL:		rect = CRect( 202, 136, 226, 160 ); break;
		case SI_ASS_CHEER_ACCURACY:			rect = CRect( 202, 162, 226, 186 ); break;
		case SI_ASS_KNU_POWERFIST:			rect = CRect( 202, 188, 226, 212 ); break;		
//링마스터
		case SI_RIN_SUP_PROTECT:            rect = CRect( 34, 260, 58, 284 ); break;
		case SI_RIN_SUP_HOLYCROSS:			rect = CRect( 34, 286, 58, 310 ); break;
		case SI_RIN_HEAL_GVURTIALLA:		rect = CRect( 34, 312, 58, 336 ); break;
		case SI_RIN_SUP_HOLYGUARD:			rect = CRect( 84, 260, 108, 284 ); break;
		case SI_RIN_SUP_SPIRITUREFORTUNE:	rect = CRect( 84, 286, 108, 310 ); break;
		case SI_RIN_HEAL_HEALRAIN:			rect = CRect( 84, 312, 108, 336 ); break;
		case SI_RIN_SQU_GEBURAHTIPHRETH:	rect = CRect( 134, 272, 158, 296 ); break;
		case SI_RIN_SUP_MERKABAHANZELRUSHA:	rect = CRect( 134, 312, 158, 336 ); break;
//빌포스터
		case SI_BIL_KNU_BELIALSMESHING:		rect = CRect( 34, 260, 58, 284 ); break;
		case SI_BIL_PST_ASMODEUS:			rect = CRect( 34, 286, 58, 310 ); break;
		case SI_BIL_KNU_BLOODFIST:			rect = CRect( 84, 260, 108, 284 ); break;
		case SI_BIL_PST_BARAQIJALESNA:		rect = CRect( 84, 286, 108, 310 ); break;
		case SI_BIL_KNU_PIERCINGSERPENT:	rect = CRect( 134, 260, 158, 284 ); break;
		case SI_BIL_PST_BGVURTIALBOLD:		rect = CRect( 134, 286, 158, 310 ); break;
		case SI_BIL_KNU_SONICHAND:			rect = CRect( 184, 272, 208, 296 ); break;
		case SI_BIL_PST_ASALRAALAIKUM:		rect = CRect( 184, 312, 208, 336 ); break;
			
			
// 매지션
		case SI_MAG_MAG_MENTALSTRIKE:			rect = CRect( 34, 96, 58, 120 );	break;
		case SI_MAG_MAG_BLINKPOOL: 				rect = CRect( 34, 148, 58, 172 );	break;
		case SI_MAG_FIRE_BOOMERANG:				rect = CRect( 84, 96, 108, 120 );   break;
		case SI_MAG_WIND_SWORDWIND:				rect = CRect( 84, 122, 108, 146 );	break;
		case SI_MAG_WATER_ICEMISSILE:			rect = CRect( 84, 148, 108, 172 );	break;			
		case SI_MAG_ELECTRICITY_LIGHTINGBALL:	rect = CRect( 84, 174, 108, 198 );	break;
		case SI_MAG_EARTH_SPIKESTONE:			rect = CRect( 84, 200, 108, 224 );	break;	
		case SI_MAG_FIRE_HOTAIR:				rect = CRect( 134, 96, 158, 120 );		break;		
		case SI_MAG_WIND_STRONGWIND:			rect = CRect( 134, 122, 158, 146 );		break;		
		case SI_MAG_WATER_WATERBALL:			rect = CRect( 134, 148, 158, 172 );		break;		
		case SI_MAG_ELECTRICITY_LIGHTINGPARM:	rect = CRect( 134, 174, 158, 198 );		break;		
		case SI_MAG_EARTH_ROCKCRASH:			rect = CRect( 134, 200, 158, 224 );		break;		
		case SI_MAG_FIRE_FIRESTRIKE:			rect = CRect( 184, 96, 208, 120 );		break;		
		case SI_MAG_WIND_WINDCUTTER:			rect = CRect( 184, 122, 208, 146 );		break;		
		case SI_MAG_WATER_SPRINGWATER:			rect = CRect( 184, 148, 208, 172 );		break;		
		case SI_MAG_ELECTRICITY_LIGHTINGSHOCK:	rect = CRect( 184, 174, 208, 198 );		break;		
		case SI_MAG_EARTH_LOOTING:				rect = CRect( 184, 200, 208, 224 );		break;
// 엘리멘터
		case SI_ELE_FIRE_FIREBIRD:				rect = CRect( 34, 234, 58, 258 ); break;
		case SI_ELE_EARTH_STONESPEAR:			rect = CRect( 34, 260, 58, 284 ); break;
		case SI_ELE_WIND_VOID:					rect = CRect( 34, 286, 58, 310 ); break;
		case SI_ELE_ELECTRICITY_THUNDERSTRIKE:	rect = CRect( 34, 312, 58, 336 ); break;
		case SI_ELE_WATER_ICESHARK:				rect = CRect( 34, 338, 58, 362 ); break;
		case SI_ELE_FIRE_BURINGFIELD:			rect = CRect( 84, 234, 108, 258 ); break;
		case SI_ELE_EARTH_EARTHQUAKE:			rect = CRect( 84, 260, 108, 284 ); break;
		case SI_ELE_WIND_WINDFIELD:				rect = CRect( 84, 286, 108, 310 ); break;
		case SI_ELE_ELECTRICITY_ELETRICSHOCK:	rect = CRect( 84, 312, 108, 336 ); break;
		case SI_ELE_WATER_POISONCLOUD:			rect = CRect( 84, 338, 108, 362 ); break;
		case SI_ELE_MULTY_METEOSHOWER:			rect = CRect( 134, 246, 158, 270 ); break;
		case SI_ELE_MULTY_SANDSTORM:			rect = CRect( 134, 272, 158, 296 ); break;
		case SI_ELE_MULTY_LIGHTINGSTORM:		rect = CRect( 134, 298, 158, 322 ); break;
		case SI_ELE_MULTY_AVALANCHE:			rect = CRect( 134, 338, 158, 362 ); break;
		case SI_ELE_FIRE_FIREMASTER:			rect = CRect( 184, 234, 208, 258 ); break;
		case SI_ELE_EARTH_EARTHMASTER:			rect = CRect( 184, 260, 208, 284 ); break;
		case SI_ELE_WIND_WINDMASTER:			rect = CRect( 184, 286, 208, 310 ); break;
		case SI_ELE_ELECTRICITY_LIGHTINGMASTER:	rect = CRect( 184, 312, 208, 336 ); break;
		case SI_ELE_WATER_WATERMASTER:			rect = CRect( 184, 338, 208, 362 ); break;
// 사이키퍼
		case SI_PSY_NLG_DEMONOLGY:				rect = CRect( 34, 260, 58, 284 ); break;
		case SI_PSY_PSY_PSYCHICBOMB:			rect = CRect( 34, 286, 58, 310 ); break;
		case SI_PSY_NLG_CRUCIOSPELL:			rect = CRect( 34, 312, 58, 336 ); break;
		case SI_PSY_NLG_SATANOLGY:				rect = CRect( 84, 260, 108, 284 ); break;
		case SI_PSY_PSY_SPRITBOMB:				rect = CRect( 84, 286, 108, 310 ); break;
		case SI_PSY_PSY_MAXIMUMCRISIS:			rect = CRect( 84, 312, 108, 336 ); break;
		case SI_PSY_PSY_PSYCHICWALL:			rect = CRect( 134, 272, 158, 296 ); break;
		case SI_PSY_PSY_PSYCHICSQUARE:			rect = CRect( 134, 312, 158, 336 ); break;
////////////////////////////////////////////////////////////////////////////////////
			
		default:
			return FALSE;
	}

*/
	int nExpertGapX = 9;
	int nExpertGapY = 90;
	
	int nProGapX = 8;
	int nProGapY = 228;

	ItemProp* pPropSkill = prj.GetSkillProp( dwSkillID );

	if( pPropSkill )
	{
		CPoint pt;

		LPWNDCTRL lpWndCtrl;
		LPWNDCTRL lpWndCtrlUpper = GetWndCtrl( WIDC_STATIC5 );
		LPWNDCTRL lpWndCtrlLower = GetWndCtrl( WIDC_STATIC7 );

		switch( pPropSkill->dwItemKind1 )
		{
			case JTYPE_BASE  : pt = 0; break;
			case JTYPE_EXPERT: pt = lpWndCtrlUpper->rect.TopLeft(); break;
			case JTYPE_PRO   : pt = lpWndCtrlLower->rect.TopLeft(); break;
#if __VER >= 10 // __CSC_VER9_1
			case JTYPE_MASTER : pt = 0; break;
			case JTYPE_HERO  : pt = 0; break;
#endif //__CSC_VER9_1
		}
		int nRectX, nRectY, nJobKind;
		nRectX = nRectY = 0;
		nJobKind = MAX_JOBBASE;
#if __VER >= 10 // __CSC_VER9_1
		int nLegendSkill = 0;
#endif //__CSC_VER9_1
		switch(dwSkillID)
		{
			case SI_VAG_ONE_CLEANHIT:
				{
					lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );
					rect = lpWndCtrl->rect;
					rect.top += 1;
				}
				break;
			case SI_VAG_ONE_BRANDISH:
				{
					lpWndCtrl = GetWndCtrl( WIDC_CUSTOM3 );
					rect = lpWndCtrl->rect;
					rect.top += 1;
				}
				break;
			case SI_VAG_ONE_OVERCUTTER:
				{
					lpWndCtrl = GetWndCtrl( WIDC_CUSTOM4 );
					rect = lpWndCtrl->rect;
					rect.top += 1;
				}
				break;
	//머서너리
			case SI_MER_ONE_SPLMASH:				nRectX =  34, nRectY = 109, nJobKind = MAX_EXPERT; break;
			case SI_MER_SHIELD_PROTECTION:			nRectX =  34, nRectY = 161, nJobKind = MAX_EXPERT; break;
			case SI_MER_ONE_KEENWHEEL:				nRectX =  84, nRectY =  97, nJobKind = MAX_EXPERT; break;
			case SI_MER_ONE_BLOODYSTRIKE:			nRectX =  84, nRectY = 123, nJobKind = MAX_EXPERT; break;
			case SI_MER_SHIELD_PANBARRIER:			nRectX =  84, nRectY = 149, nJobKind = MAX_EXPERT; break;
			case SI_MER_SUP_IMPOWERWEAPON:			nRectX =  84, nRectY = 175, nJobKind = MAX_EXPERT; break;
			case SI_MER_ONE_BLINDSIDE:				nRectX = 134, nRectY =  97, nJobKind = MAX_EXPERT; break;
			case SI_MER_ONE_REFLEXHIT:				nRectX = 134, nRectY = 123, nJobKind = MAX_EXPERT; break;
			case SI_MER_ONE_SNEAKER:				nRectX = 134, nRectY = 149, nJobKind = MAX_EXPERT; break;
			case SI_MER_SUP_SMITEAXE:				nRectX = 134, nRectY = 175, nJobKind = MAX_EXPERT; break;
			case SI_MER_SUP_BLAZINGSWORD:			nRectX = 134, nRectY = 201, nJobKind = MAX_EXPERT; break;
			case SI_MER_ONE_SPECIALHIT:				nRectX = 184, nRectY =  97, nJobKind = MAX_EXPERT; break;
			case SI_MER_ONE_GUILOTINE:				nRectX = 184, nRectY = 123, nJobKind = MAX_EXPERT; break;
			case SI_MER_SUP_AXEMASTER:				nRectX = 184, nRectY = 175, nJobKind = MAX_EXPERT; break;
			case SI_MER_SUP_SWORDMASTER:			nRectX = 184, nRectY = 201, nJobKind = MAX_EXPERT; break;
	// 나이트
			case SI_KNT_TWOSW_CHARGE:				nRectX =  34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_KNT_TWOAX_PAINDEALER:			nRectX =  34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_KNT_SUP_GUARD:					nRectX =  34, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_KNT_TWOSW_EARTHDIVIDER:			nRectX =  84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_KNT_TWOAX_POWERSTUMP:			nRectX =  84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_KNT_SUP_RAGE:					nRectX =  84, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_KNT_TWO_POWERSWING:				nRectX = 134, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SI_KNT_SUP_PAINREFLECTION:			nRectX = 134, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
	// 블레이드
			case SI_BLD_DOUBLESW_SILENTSTRIKE:		nRectX =  34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BLD_DOUBLEAX_SPRINGATTACK:		nRectX =  34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BLD_DOUBLE_ARMORPENETRATE:		nRectX =  34, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BLD_DOUBLESW_BLADEDANCE:		nRectX =  84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BLD_DOUBLEAX_HAWKATTACK:		nRectX =  84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BLD_SUP_BERSERK:				nRectX =  84, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BLD_DOUBLE_CROSSSTRIKE:			nRectX = 134, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BLD_DOUBLE_SONICBLADE:			nRectX = 134, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
				
	// 어시스트	
			case SI_ASS_HEAL_HEALING:				nRectX =  18, nRectY = 135, nJobKind = MAX_EXPERT; break;
			case SI_ASS_HEAL_PATIENCE:				nRectX =  64, nRectY = 109, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_QUICKSTEP:			nRectX =  64, nRectY = 135, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_MENTALSIGN:			nRectX =  64, nRectY = 161, nJobKind = MAX_EXPERT; break;
			case SI_ASS_KNU_TAMPINGHOLE:			nRectX =  64, nRectY = 187, nJobKind = MAX_EXPERT; break;
			case SI_ASS_HEAL_RESURRECTION:			nRectX = 110, nRectY = 109, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_HASTE:				nRectX = 110, nRectY = 135, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_HEAPUP:				nRectX = 110, nRectY = 161, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_STONEHAND:			nRectX = 110, nRectY = 187, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_CIRCLEHEALING:		nRectX = 156, nRectY = 109, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_CATSREFLEX:			nRectX = 156, nRectY = 135, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_BEEFUP:				nRectX = 156, nRectY = 161, nJobKind = MAX_EXPERT; break;
			case SI_ASS_KNU_BURSTCRACK:				nRectX = 156, nRectY = 187, nJobKind = MAX_EXPERT; break;
			case SI_ASS_HEAL_PREVENTION:			nRectX = 202, nRectY = 109, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_CANNONBALL:			nRectX = 202, nRectY = 135, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_ACCURACY:				nRectX = 202, nRectY = 161, nJobKind = MAX_EXPERT; break;
			case SI_ASS_KNU_POWERFIST:				nRectX = 202, nRectY = 187, nJobKind = MAX_EXPERT; break;
	//링마스터
			case SI_RIN_SUP_PROTECT:				nRectX =  34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RIN_SUP_HOLYCROSS:				nRectX =  34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RIN_HEAL_GVURTIALLA:			nRectX =  34, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RIN_SUP_HOLYGUARD:				nRectX =  84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RIN_SUP_SPIRITUREFORTUNE:		nRectX =  84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RIN_HEAL_HEALRAIN:				nRectX =  84, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RIN_SQU_GEBURAHTIPHRETH:		nRectX = 134, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RIN_SUP_MERKABAHANZELRUSHA:		nRectX = 134, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
	//빌포스터
			case SI_BIL_KNU_BELIALSMESHING:			nRectX =  34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BIL_PST_ASMODEUS:				nRectX =  34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BIL_KNU_BLOODFIST:				nRectX =  84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BIL_PST_BARAQIJALESNA:			nRectX =  84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BIL_KNU_PIERCINGSERPENT:		nRectX = 134, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BIL_PST_BGVURTIALBOLD:			nRectX = 134, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BIL_KNU_SONICHAND:				nRectX = 184, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BIL_PST_ASALRAALAIKUM:			nRectX = 184, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
								
	// 매지션
			case SI_MAG_MAG_MENTALSTRIKE:			nRectX =  34, nRectY =  96, nJobKind = MAX_EXPERT; break;
			case SI_MAG_MAG_BLINKPOOL: 				nRectX =  34, nRectY = 148, nJobKind = MAX_EXPERT; break;
			case SI_MAG_FIRE_BOOMERANG:				nRectX =  84, nRectY =  96, nJobKind = MAX_EXPERT; break;
			case SI_MAG_WIND_SWORDWIND:				nRectX =  84, nRectY = 122, nJobKind = MAX_EXPERT; break;
			case SI_MAG_WATER_ICEMISSILE:			nRectX =  84, nRectY = 148, nJobKind = MAX_EXPERT; break;
			case SI_MAG_ELECTRICITY_LIGHTINGBALL:	nRectX =  84, nRectY = 174, nJobKind = MAX_EXPERT; break;
			case SI_MAG_EARTH_SPIKESTONE:			nRectX =  84, nRectY = 200, nJobKind = MAX_EXPERT; break;
			case SI_MAG_FIRE_HOTAIR:				nRectX = 134, nRectY =  96, nJobKind = MAX_EXPERT; break;
			case SI_MAG_WIND_STRONGWIND:			nRectX = 134, nRectY = 122, nJobKind = MAX_EXPERT; break;
			case SI_MAG_WATER_WATERBALL:			nRectX = 134, nRectY = 148, nJobKind = MAX_EXPERT; break;
			case SI_MAG_ELECTRICITY_LIGHTINGPARM:	nRectX = 134, nRectY = 174, nJobKind = MAX_EXPERT; break;
			case SI_MAG_EARTH_ROCKCRASH:			nRectX = 134, nRectY = 200, nJobKind = MAX_EXPERT; break;
			case SI_MAG_FIRE_FIRESTRIKE:			nRectX = 184, nRectY =  96, nJobKind = MAX_EXPERT; break;
			case SI_MAG_WIND_WINDCUTTER:			nRectX = 184, nRectY = 122, nJobKind = MAX_EXPERT; break;
			case SI_MAG_WATER_SPRINGWATER:			nRectX = 184, nRectY = 148, nJobKind = MAX_EXPERT; break;
			case SI_MAG_ELECTRICITY_LIGHTINGSHOCK:	nRectX = 184, nRectY = 174, nJobKind = MAX_EXPERT; break;
			case SI_MAG_EARTH_LOOTING:				nRectX = 184, nRectY = 200, nJobKind = MAX_EXPERT; break;
	// 엘리멘터
			case SI_ELE_FIRE_FIREBIRD:				nRectX =  34, nRectY = 234, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_EARTH_STONESPEAR:			nRectX =  34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_WIND_VOID:					nRectX =  34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_ELECTRICITY_THUNDERSTRIKE:	nRectX =  34, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_WATER_ICESHARK:				nRectX =  34, nRectY = 338, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_FIRE_BURINGFIELD:			nRectX =  84, nRectY = 234, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_EARTH_EARTHQUAKE:			nRectX =  84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_WIND_WINDFIELD:				nRectX =  84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_ELECTRICITY_ELETRICSHOCK:	nRectX =  84, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_WATER_POISONCLOUD:			nRectX =  84, nRectY = 338, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_MULTY_METEOSHOWER:			nRectX = 134, nRectY = 246, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_MULTY_SANDSTORM:			nRectX = 134, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_MULTY_LIGHTINGSTORM:		nRectX = 134, nRectY = 298, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_MULTY_AVALANCHE:			nRectX = 134, nRectY = 338, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_FIRE_FIREMASTER:			nRectX = 184, nRectY = 234, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_EARTH_EARTHMASTER:			nRectX = 184, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_WIND_WINDMASTER:			nRectX = 184, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_ELECTRICITY_LIGHTINGMASTER:	nRectX = 184, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_WATER_WATERMASTER:			nRectX = 184, nRectY = 338, nJobKind = MAX_PROFESSIONAL; break;
	// 사이키퍼
			case SI_PSY_NLG_DEMONOLGY:				nRectX =  34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_PSY_PSY_PSYCHICBOMB:			nRectX =  34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_PSY_NLG_CRUCIOSPELL:			nRectX =  34, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_PSY_NLG_SATANOLGY:				nRectX =  84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_PSY_PSY_SPRITBOMB:				nRectX =  84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_PSY_PSY_MAXIMUMCRISIS:			nRectX =  84, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_PSY_PSY_PSYCHICWALL:			nRectX = 134, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SI_PSY_PSY_PSYCHICSQUARE:			nRectX = 134, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
	////////////////////////////////////////////////////////////////////////////////////
	// 아크로벳
			case SI_ACR_YOYO_PULLING:				nRectX =  34, nRectY =  97, nJobKind = MAX_EXPERT; break;
			case SI_ACR_SUP_SLOWSTEP:				nRectX =  34, nRectY = 123, nJobKind = MAX_EXPERT; break;
			case SI_ACR_BOW_JUNKBOW:				nRectX =  34, nRectY = 175, nJobKind = MAX_EXPERT; break;
			case SI_ACR_SUP_FASTWALKER:				nRectX =  84, nRectY =  96, nJobKind = MAX_EXPERT; break;
			case SI_ACR_SUP_YOYOMASTER:				nRectX =  84, nRectY = 122, nJobKind = MAX_EXPERT; break;
			case SI_ACR_SUP_BOWMASTER: 				nRectX =  84, nRectY = 174, nJobKind = MAX_EXPERT; break;
			case SI_ACR_SUP_DARKILLUSION:			nRectX = 134, nRectY =  96, nJobKind = MAX_EXPERT; break;
			case SI_ACR_YOYO_SNITCH:				nRectX = 134, nRectY = 122, nJobKind = MAX_EXPERT; break;
			case SI_ACR_YOYO_CROSSLINE:				nRectX = 134, nRectY = 149, nJobKind = MAX_EXPERT; break;		
			case SI_ACR_BOW_SILENTSHOT:				nRectX = 134, nRectY = 174, nJobKind = MAX_EXPERT; break;		
			case SI_ACR_BOW_AIMEDSHOT:				nRectX = 134, nRectY = 200, nJobKind = MAX_EXPERT; break;
			case SI_ACR_YOYO_ABSOLUTEBLOCK:			nRectX = 184, nRectY =  96, nJobKind = MAX_EXPERT; break;
			case SI_ACR_YOYO_DEADLYSWING:			nRectX = 184, nRectY = 122, nJobKind = MAX_EXPERT; break;		
			case SI_ACR_YOYO_COUNTER:				nRectX = 184, nRectY = 149, nJobKind = MAX_EXPERT; break;		
			case SI_ACR_BOW_AUTOSHOT:				nRectX = 184, nRectY = 174, nJobKind = MAX_EXPERT; break;		
			case SI_ACR_BOW_ARROWRAIN:				nRectX = 184, nRectY = 200, nJobKind = MAX_EXPERT; break;		
	//제스터
			case SI_JST_SUP_POISON:					nRectX =  34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_JST_SUP_BLEEDING:				nRectX =  34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_JST_YOYO_ESCAPE:				nRectX =  34, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_JST_YOYO_CRITICALSWING:			nRectX =  84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_JST_YOYO_BACKSTAB:				nRectX =  84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_JST_SUP_ABSORB:					nRectX =  84, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_JST_YOYO_VATALSTAB:				nRectX = 134, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SI_JST_YOYO_HITOFPENYA:			nRectX = 134, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
	//레인저
			case SI_RAG_BOW_ICEARROW:				nRectX =  34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RAG_BOW_FLAMEARROW:				nRectX =  34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RAG_BOW_POISONARROW:			nRectX =  34, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RAG_SUP_FASTATTACK:				nRectX =  84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RAG_BOW_PIRCINGARROW:			nRectX =  84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RAG_SUP_NATURE:					nRectX =  84, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RAG_BOW_TRIPLESHOT:				nRectX = 134, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RAG_BOW_SILENTARROW:			nRectX = 134, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
				
#if __VER >= 10 // __CSC_VER9_1 //9차 전승 관련 영웅 스킬 추가.
			//마스터 스킬
			case SI_BLD_MASTER_ONEHANDMASTER:		nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			case SI_KNT_MASTER_TWOHANDMASTER:		nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			case SI_JST_MASTER_YOYOMASTER:			nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			case SI_RAG_MASTER_BOWMASTER:			nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			case SI_ELE_MASTER_INTMASTER:			nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			case SI_PSY_MASTER_INTMASTER:			nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			case SI_BIL_MASTER_KNUCKLEMASTER:		nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			case SI_RIG_MASTER_BLESSING:			nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			//영웅 스킬			
			case SI_BLD_HERO_DEFFENCE:				nLegendSkill = 1, nJobKind = MAX_HERO; break;
			case SI_KNT_HERO_DRAWING:				nLegendSkill = 1, nJobKind = MAX_HERO; break;
			case SI_JST_HERO_SILENCE:				nLegendSkill = 1, nJobKind = MAX_HERO; break;
			case SI_RAG_HERO_HAWKEYE:				nLegendSkill = 1, nJobKind = MAX_HERO; break;
			case SI_ELE_HERO_CURSEMIND:				nLegendSkill = 1, nJobKind = MAX_HERO; break;
			case SI_PSY_HERO_STONE:					nLegendSkill = 1, nJobKind = MAX_HERO; break;
			case SI_BIL_HERO_DISENCHANT:			nLegendSkill = 1, nJobKind = MAX_HERO; break;
			case SI_RIG_HERO_RETURN:				nLegendSkill = 1, nJobKind = MAX_HERO; break;
#endif //__CSC_VER9_1
			default:
				return FALSE;
		}

		if( nJobKind == MAX_EXPERT )
			rect = CRect(  nRectX-nExpertGapX ,  nRectY-nExpertGapY, nRectX+24-nExpertGapX, nRectY+24-nExpertGapY );
		else if( nJobKind == MAX_PROFESSIONAL )
			rect = CRect(  nRectX-nProGapX,  nRectY-nProGapY, nRectX+24-nProGapX, nRectY+24-nProGapY );
#if __VER >= 10 // __CSC_VER9_1
		else if( nJobKind == MAX_MASTER || nJobKind == MAX_HERO )
		{
			LPWNDCTRL lpWndCtrl;
			if(nLegendSkill == 0)
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM5 );
			else if(nLegendSkill == 1)
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM6 );

			rect = lpWndCtrl->rect;
		}
#endif //__CSC_VER9_1
		
		rect += pt;
	}
/*
	switch( m_nJob )
	{
	case JOB_KNIGHT:
		break;			
	case JOB_BLADE:
		break;			
	case JOB_MERCENARY:
		break;			
	case JOB_BILLPOSTER:
		break;			
	case JOB_RINGMASTER:
		break;			
	case JOB_ASSIST:
		break;			
	case JOB_ELEMENTOR:
		break;			
	case JOB_PSYCHIKEEPER:
		break;			
	case JOB_MAGICIAN:
		break;	
	}
*/
	return TRUE;
}

void CWndSkillTreeEx::LoadTextureSkillicon()
{
	if( m_apSkill == NULL )
		return;

	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
	{
		LPSKILL lpSkill;
		lpSkill = &m_apSkill[ i ];
		if( lpSkill->dwSkill != NULL_ID )
		{
			ItemProp* pSkillProp = prj.m_aPropSkill.GetAt( lpSkill->dwSkill );
			if( pSkillProp )
				m_atexSkill[ i ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, pSkillProp->szIcon ), 0xffff00ff );
		}
	}
	m_textPackNum.LoadScript( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, _T( "icon_IconSkillLevel.inc" ) ) );
}
void CWndSkillTreeEx::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_apSkill == NULL )
		return;

	if( m_bDrag == FALSE )
		return;
	
	CPoint pt( 3, 3 );
	CRect rect;
	if( CheckSkill( g_nSkillCurSelect ) )
	{
		m_bDrag = FALSE;
		DWORD dwSkill = m_apSkill[ g_nSkillCurSelect ].dwSkill;
		ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
		
		m_GlobalShortcut.m_pFromWnd    = this;
		m_GlobalShortcut.m_dwShortcut  = SHORTCUT_SKILL;
		m_GlobalShortcut.m_dwType  = 0;
		m_GlobalShortcut.m_dwIndex = dwSkill;
		m_GlobalShortcut.m_dwData = 0;
		m_GlobalShortcut.m_dwId       = g_nSkillCurSelect; // 컬런트 셀렉트가 곧 ID나 마찬가지임.
		m_GlobalShortcut.m_pTexture = m_atexSkill[ g_nSkillCurSelect ];//L;//pItemElem->m_pTexture;
		_tcscpy( m_GlobalShortcut.m_szString, pSkillProp->szName);
	}
}

BOOL CWndSkillTreeEx::Process()
{
	if( m_apSkill == NULL || g_pPlayer == NULL)
		return FALSE;
	
#if __VER >= 10 // __CSC_VER9_1
	if(!m_bLegend)
	{
		if(g_pPlayer->GetLegendChar() >= LEGEND_CLASS_MASTER)
		{
			m_bLegend = TRUE;
			//Hero SkillTree
			m_pWndHeroStatic[0] = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
			m_pWndHeroStatic[0]->EnableWindow(TRUE);
			m_pWndHeroStatic[0]->SetVisible(TRUE);
			
			m_pWndHeroStatic[1] = (CWndStatic*)GetDlgItem( WIDC_STATIC11 );
			m_pWndHeroStatic[1]->EnableWindow(TRUE);
			m_pWndHeroStatic[1]->SetVisible(TRUE);

			CRect rect = GetWindowRect();
			rect.bottom += 90;
			m_bSlot[3] = TRUE;
			
			SetWndRect(rect);

			CRect rectRoot = m_pWndRoot->GetLayoutRect();
			int x = rectRoot.right - rect.Width();
			int y = rectRoot.bottom - rect.Height();
			
			CPoint point( x, y );
			Move( point );
			return TRUE;
		}
	}
	else
	{
		if(g_pPlayer->GetLegendChar() < LEGEND_CLASS_MASTER)
		{
			m_bLegend = FALSE;
			//Normal SkillTree
			m_pWndHeroStatic[0] = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
			m_pWndHeroStatic[0]->EnableWindow(FALSE);
			m_pWndHeroStatic[0]->SetVisible(FALSE);
			
			m_pWndHeroStatic[1] = (CWndStatic*)GetDlgItem( WIDC_STATIC11 );
			m_pWndHeroStatic[1]->EnableWindow(FALSE);
			m_pWndHeroStatic[1]->SetVisible(FALSE);
			
			CRect rect = GetWindowRect();
			rect.bottom -= 100;
			m_bSlot[3] = FALSE;
			
			SetWndRect(rect);

			CRect rectRoot = m_pWndRoot->GetLayoutRect();
			int x = rectRoot.right - rect.Width();
			int y = rectRoot.bottom - rect.Height();
			
			CPoint point( x, y );
			Move( point );
			return TRUE;
		}
	}
#endif //__CSC_VER9_1

	m_pWndButton[0]->EnableWindow( FALSE );
	m_pWndButton[1]->EnableWindow( FALSE );
	m_pWndButton[2]->EnableWindow( FALSE );
	m_pWndButton[3]->EnableWindow( FALSE );
	
	if( m_pFocusItem && 0 < g_pPlayer->m_nSkillPoint )
	{
		LPSKILL lpSkillUser = g_pPlayer->GetSkill( m_pFocusItem->dwSkill );
		ItemProp* pSkillProp = m_pFocusItem->GetProp();
		if( pSkillProp == NULL || lpSkillUser == NULL )
			return TRUE;
		
		int nPoint = prj.GetSkillPoint( pSkillProp );
		if( m_pFocusItem->dwLevel < pSkillProp->dwExpertMax && nPoint <= m_nCurrSkillPoint )
			m_pWndButton[0]->EnableWindow( TRUE );	

		if( m_pFocusItem->dwLevel != lpSkillUser->dwLevel )
			m_pWndButton[1]->EnableWindow( TRUE );	

		if( m_nCurrSkillPoint != g_pPlayer->m_nSkillPoint )
		{
			m_pWndButton[2]->EnableWindow( TRUE );
			m_pWndButton[3]->EnableWindow( TRUE );
		}

#if __VER >= 10 // __CSC_VER9_1 //전승 스킬의 레벨업은 NPC를 통해서 하기 때문에 막자.
		pSkillProp = prj.GetSkillProp( m_pFocusItem->dwSkill );
		if(pSkillProp->dwItemKind1 == JTYPE_MASTER || pSkillProp->dwItemKind1 == JTYPE_HERO)
		{
			m_pWndButton[0]->EnableWindow( FALSE );
			m_pWndButton[1]->EnableWindow( FALSE );
			m_pWndButton[2]->EnableWindow( FALSE );
			m_pWndButton[3]->EnableWindow( FALSE );
		}
#endif //__CSC_VER9_1
	}
	return TRUE;
}

void CWndSkillTreeEx::OnMouseWndSurface( CPoint point )
{
	DWORD dwMouseSkill = NULL_ID;
	for( int i = 0; i < MAX_SKILL_JOB; i++ )
	{
		LPSKILL pSkill = GetSkill( i );
		if( pSkill == NULL )
			continue;
		DWORD dwSkill = pSkill->dwSkill;

		ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );

		if( pSkillProp )
		{
			if( !m_bSlot[0] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_BASE )
					continue;
			}

			if( !m_bSlot[1] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_EXPERT )
					continue;
			}

			if( !m_bSlot[2] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_PRO )
					continue;
			}
#if __VER >= 10 // __CSC_VER9_1
			if( !m_bSlot[3] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_HERO )
					continue;
			}
#endif //__CSC_VER9_1
			GetCalcImagePos( pSkillProp->dwItemKind1 );
		}

		if( dwSkill != NULL_ID )
		{
			CRect rect;
			if( GetSkillPoint( pSkillProp->dwID, rect ) )
			{
				rect.top  -= 2;
				rect.right  = rect.left+27;
				rect.bottom = rect.top+27;
				rect.OffsetRect( 0, m_nTopDownGap );

				if( rect.PtInRect(point) )
				{
					dwMouseSkill = dwSkill;

					ClientToScreen( &point );
					ClientToScreen( &rect );
					g_WndMng.PutToolTip_Skill( dwSkill, pSkill->dwLevel, point, &rect, CheckSkill( i ) );
					break;
				}
			}
		}
	}
	m_dwMouseSkill = dwMouseSkill;
}

int CWndSkillTreeEx::GetCalcImagePos(int nIndex)
{
	m_nTopDownGap = 0;

	if( m_bSlot[0] )
	{
		if( nIndex == JTYPE_BASE )
			m_nTopDownGap = 0;
	}
	
	if( m_bSlot[1] )
	{
		if( nIndex == JTYPE_EXPERT )
		{
			if( m_bSlot[0] )
				m_nTopDownGap = 0;
			else
				m_nTopDownGap -= 48;
		}
	}

	if( m_bSlot[2] )
	{
		if( nIndex == JTYPE_PRO )
		{
			if( m_bSlot[0] )
			{
				if( m_bSlot[1] )
					m_nTopDownGap = 0;
				else
					m_nTopDownGap -= 144;
			}
			else
			{
				if( m_bSlot[1] )
					m_nTopDownGap -= 48;
				else
					m_nTopDownGap -= (144+48);
			}
		}
	}
#if __VER >= 10 // __CSC_VER9_1
	if( m_bSlot[3] )
	{
		if( nIndex == JTYPE_MASTER || nIndex == JTYPE_HERO )
		{
			if( m_bSlot[0] )
			{
				if( m_bSlot[1] )
				{
					if( m_bSlot[2] )
						m_nTopDownGap = 0;
					else
						m_nTopDownGap -= 144;
				}
				else
				{
					if( m_bSlot[2] )
						m_nTopDownGap -= 144;
					else
						m_nTopDownGap -= (144+144);
				}
			}
			else
			{
				if( m_bSlot[1] )
				{
					if( m_bSlot[2] )
						m_nTopDownGap -= 48;
					else
						m_nTopDownGap -= (144+48);
				}
				else
				{
					if( m_bSlot[2] )
						m_nTopDownGap -= (144+48);
					else
						m_nTopDownGap -= (144+144+48);
				}
			}
		}
	}
#endif //__CSC_VER9_1

	return 0;
}

void CWndSkillTreeEx::OnDraw(C2DRender* p2DRender)
{
	if( m_apSkill == NULL )
		return;

	m_nAlphaCount = m_nAlpha;
	CPoint pt;
	LPWNDCTRL lpWndCtrl;

#if __VER >= 10 // __CSC_VER9_1 //Hero Skill Background Image.
	if(m_bLegend && m_bSlot[3])
	{
		//Master Skill은 시작부터 1Lv이므로 배경 이미지 제외.
/*		if( !m_strHeroSkilBg[0].IsEmpty() )
		{
			CTexture* pTexture;
			CRect rect;
			CPoint point;
			LPWNDCTRL lpWndCtrl;
			
			lpWndCtrl = GetWndCtrl( WIDC_CUSTOM5 );
			rect = lpWndCtrl->rect;
			GetCalcImagePos(JTYPE_MASTER);
			rect.OffsetRect( 0, m_nTopDownGap );
			point = rect.TopLeft();
			point.y -= 2;
			pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, m_strHeroSkilBg[0]), 0xffff00ff );
			if(pTexture != NULL)
				pTexture->Render( p2DRender, point, CPoint( 27, 27 ) );				
		}
*/		
		if( !m_strHeroSkilBg.IsEmpty() )
		{
			CTexture* pTexture;
			CRect rect;
			CPoint point;
			LPWNDCTRL lpWndCtrl;
			
			lpWndCtrl = GetWndCtrl( WIDC_CUSTOM6 );
			rect = lpWndCtrl->rect;
			GetCalcImagePos(JTYPE_HERO);
			rect.OffsetRect( 0, m_nTopDownGap );
			point = rect.TopLeft();
			point.y -= 2;
			pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, m_strHeroSkilBg), 0xffff00ff );
			if(pTexture != NULL)
				pTexture->Render( p2DRender, point, CPoint( 27, 27 ) );				
		}
	}
#endif //__CSC_VER9_1

	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
	{
		LPSKILL pSkill = GetSkill( i );
		if( pSkill == NULL ) 
			continue;

		DWORD dwSkill = pSkill->dwSkill;

		ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );

		if( pSkillProp )
		{
			if( !m_bSlot[0] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_BASE )
					continue;
			}
			
			if( !m_bSlot[1] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_EXPERT )
					continue;
			}
			
			if( !m_bSlot[2] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_PRO )
					continue;
			}		
#if __VER >= 10 // __CSC_VER9_1
			if( !m_bSlot[3] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_MASTER || pSkillProp->dwItemKind1 == JTYPE_HERO )
					continue;
			}
#endif //__CSC_VER9_1
			GetCalcImagePos( pSkillProp->dwItemKind1 );
		}
		
		if( pSkillProp && pSkillProp->nLog != 1 && dwSkill != NULL_ID )
		{
			// 스킬 아이콘 출력 
			if( m_atexSkill[ i ] && CheckSkill( i ) && 0 < pSkill->dwLevel )
			{
				CRect rect;
				if( GetSkillPoint( pSkillProp->dwID, rect ) )
				{
					rect.top -= 2;
					rect.OffsetRect( 0, m_nTopDownGap );
					m_atexSkill[ i ]->Render( p2DRender, rect.TopLeft(), CPoint( 27, 27 ) );
					int nAddNum = 0;
					LPSKILL pSkillUser = g_pPlayer->GetSkill( pSkill->dwSkill );
					if( pSkillUser && pSkill->dwLevel != pSkillUser->dwLevel )
						nAddNum = 20;
					if( pSkill->dwLevel < pSkillProp->dwExpertMax )
						m_textPackNum.Render( p2DRender, rect.TopLeft()-CPoint(2,2), pSkill->dwLevel-1 + nAddNum );
					else
						m_textPackNum.Render( p2DRender, rect.TopLeft()-CPoint(2,2), 19 + nAddNum );
				}
			}	
			else if( m_dwMouseSkill == dwSkill && m_atexSkill[ i ] && CheckSkill( i ) )
			{
				CRect rect;
				if( GetSkillPoint( pSkillProp->dwID, rect ) )
				{
					rect.top -= 2;
					rect.OffsetRect( 0, m_nTopDownGap );
					m_atexSkill[ i ]->Render( p2DRender, rect.TopLeft(), CPoint( 27, 27 ) );				
				}
			}
		}
	}

	CWndStatic* lpWndStatic9 = (CWndStatic*)GetDlgItem( WIDC_STATIC9 );
	CString strSP;
	strSP.Format( "%d", m_nCurrSkillPoint );
	lpWndStatic9->SetTitle( strSP );

	// 선택된 스킬이 있을시 위 창에 출력
#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
	if( m_pFocusItem && g_nSkillCurSelect > -1 )
#else // __IMPROVE_SYSTEM_VER15
	if( m_pFocusItem && g_nSkillCurSelect >= 0 )
#endif // __IMPROVE_SYSTEM_VER15
	{
		if( m_pFocusItem->dwSkill == -1 )
		{
			CString str;
			str.Format( "스킬쓴넘 직업 %d\n스킬 인덱스가 %d인값이 나옴 : 레벨 %d, CurSelect %d", g_pPlayer->GetJob(), m_pFocusItem->dwSkill, m_pFocusItem->dwLevel, g_nSkillCurSelect );
			ADDERRORMSG( str );
		}

		lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
		m_atexSkill[ g_nSkillCurSelect ]->Render( p2DRender, lpWndCtrl->rect.TopLeft() );

		ItemProp* pSkillProp = prj.GetSkillProp( m_pFocusItem->dwSkill );
		if( pSkillProp && 0 < m_pFocusItem->dwLevel )
		{
			int nAddNum = 0;
			LPSKILL pSkillUser = g_pPlayer->GetSkill( m_pFocusItem->dwSkill );
			if( pSkillUser && m_pFocusItem->dwLevel != pSkillUser->dwLevel )
				nAddNum = 20;

			if( m_pFocusItem->dwLevel < pSkillProp->dwExpertMax )
				m_textPackNum.Render( p2DRender, lpWndCtrl->rect.TopLeft(), m_pFocusItem->dwLevel-1 + nAddNum );
			else
				m_textPackNum.Render( p2DRender, lpWndCtrl->rect.TopLeft(), 19 + nAddNum );
		}
	}
	CRect rect;
	CWndStatic* pStatic;
	pStatic = (CWndStatic *)GetDlgItem( WIDC_STATIC2 );
	rect = pStatic->GetWindowRect(TRUE);

	rect.left = rect.right - 30;
	
	if( m_bSlot[0] )
		m_atexTopDown[ 0 ]->Render( p2DRender, rect.TopLeft(), CPoint( 20, 20 ) );
	else
		m_atexTopDown[ 1 ]->Render( p2DRender, rect.TopLeft(), CPoint( 20, 20 ) );
	///////////////////////////////////////////////////////////////////////////////////////

	pStatic = (CWndStatic *)GetDlgItem( WIDC_STATIC3 );
	rect = pStatic->GetWindowRect(TRUE);
	
	rect.left = rect.right - 30;
	
	if( m_bSlot[1] )
		m_atexTopDown[ 0 ]->Render( p2DRender, rect.TopLeft(), CPoint( 20, 20 ) );
	else
		m_atexTopDown[ 1 ]->Render( p2DRender, rect.TopLeft(), CPoint( 20, 20 ) );
	///////////////////////////////////////////////////////////////////////////////////////

	pStatic = (CWndStatic *)GetDlgItem( WIDC_STATIC6 );
	rect = pStatic->GetWindowRect(TRUE);
	
	rect.left = rect.right - 30;
	
	if( m_bSlot[2] )
		m_atexTopDown[ 0 ]->Render( p2DRender, rect.TopLeft(), CPoint( 20, 20 ) );
	else
		m_atexTopDown[ 1 ]->Render( p2DRender, rect.TopLeft(), CPoint( 20, 20 ) );
	
#if __VER >= 10 // __CSC_VER9_1
	///////////////////////////////////////////////////////////////////////////////////////
	
	if(m_bLegend)
	{
		pStatic = (CWndStatic *)GetDlgItem( WIDC_STATIC1 );
		rect = pStatic->GetWindowRect(TRUE);
		
		rect.left = rect.right - 30;
		
		if( m_bSlot[3] )
			m_atexTopDown[ 0 ]->Render( p2DRender, rect.TopLeft(), CPoint( 20, 20 ) );
		else
			m_atexTopDown[ 1 ]->Render( p2DRender, rect.TopLeft(), CPoint( 20, 20 ) );
	}
#endif //__CSC_VER9_1


/*
	{
	CString str;
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM7 );
	str.Format( "rect = CRect( %d, %d, %d, %d ); break;\n", lpWndCtrl->rect.left, lpWndCtrl->rect.top, lpWndCtrl->rect.right, lpWndCtrl->rect.bottom );
	OutputDebugString(str);
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM8 );
	str.Format( "rect = CRect( %d, %d, %d, %d ); break;\n", lpWndCtrl->rect.left, lpWndCtrl->rect.top, lpWndCtrl->rect.right, lpWndCtrl->rect.bottom );
	OutputDebugString(str);
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM9 );
	str.Format( "rect = CRect( %d, %d, %d, %d ); break;\n", lpWndCtrl->rect.left, lpWndCtrl->rect.top, lpWndCtrl->rect.right, lpWndCtrl->rect.bottom );
	OutputDebugString(str);
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM10 );
	str.Format( "rect = CRect( %d, %d, %d, %d ); break;\n", lpWndCtrl->rect.left, lpWndCtrl->rect.top, lpWndCtrl->rect.right, lpWndCtrl->rect.bottom );
	OutputDebugString(str);
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM11 );
	str.Format( "rect = CRect( %d, %d, %d, %d ); break;\n", lpWndCtrl->rect.left, lpWndCtrl->rect.top, lpWndCtrl->rect.right, lpWndCtrl->rect.bottom );
	OutputDebugString(str);
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM12 );
	str.Format( "rect = CRect( %d, %d, %d, %d ); break;\n", lpWndCtrl->rect.left, lpWndCtrl->rect.top, lpWndCtrl->rect.right, lpWndCtrl->rect.bottom );
	OutputDebugString(str);
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM13 );
	str.Format( "rect = CRect( %d, %d, %d, %d ); break;\n", lpWndCtrl->rect.left, lpWndCtrl->rect.top, lpWndCtrl->rect.right, lpWndCtrl->rect.bottom );
	OutputDebugString(str);
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM14 );
	str.Format( "rect = CRect( %d, %d, %d, %d ); break;\n", lpWndCtrl->rect.left, lpWndCtrl->rect.top, lpWndCtrl->rect.right, lpWndCtrl->rect.bottom );
	OutputDebugString(str);
	}
	/**/	
}
void CWndSkillTreeEx::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	
	RestoreDeviceObjects();
	
	m_texGauEmptyNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptyNormal.bmp" ), 0xffff00ff, TRUE );
	m_texGauFillNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauFillNormal.bmp" ), 0xffff00ff, TRUE );

	m_aSkillLevel[ 0 ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "ButtSkillLevelHold01.tga" ), 0xffff00ff );
	m_aSkillLevel[ 1 ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "ButtSkillLevelup01.tga" ), 0xffff00ff );
	m_aSkillLevel[ 2 ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "ButtSkillLevelHold02.tga" ), 0xffff00ff );
	
	InitItem( g_pPlayer->GetJob(), g_pPlayer->m_aJobSkill );


	m_atexTopDown[ 0 ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "LvUp.bmp" ), 0xffff00ff );
	m_atexTopDown[ 1 ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "LvDn.bmp" ), 0xffff00ff );

	m_nTopDownGap = 0;
	m_bSlot[0] = TRUE;
	m_bSlot[1] = TRUE;
	m_bSlot[2] = TRUE;
#if __VER >= 10 // __CSC_VER9_1
	m_bSlot[3] = TRUE;
#endif //__CSC_VER9_1
	
	if( g_nSkillCurSelect >= 0 )
		m_pFocusItem = &m_apSkill[ g_nSkillCurSelect ];

	m_pWndButton[0] = (CWndButton*) GetDlgItem( WIDC_BUTTON1 );	// + 버튼
	m_pWndButton[1] = (CWndButton*) GetDlgItem( WIDC_BUTTON2 );	// - 버튼
	m_pWndButton[2] = (CWndButton*) GetDlgItem( WIDC_BUTTON3 );	// Reset 버튼
	m_pWndButton[3] = (CWndButton*) GetDlgItem( WIDC_BUTTON4 );	// Finish 버튼
	
#if __VER >= 10 // __CSC_VER9_1 //전승 스킬 추가.
	if(g_pPlayer->GetLegendChar() >= LEGEND_CLASS_MASTER)
		m_bLegend = TRUE;
	else
		m_bLegend = FALSE;

	if(m_bLegend)
	{
		m_pWndHeroStatic[0] = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
		m_pWndHeroStatic[0]->EnableWindow(TRUE);
		m_pWndHeroStatic[0]->SetVisible(TRUE);

		m_pWndHeroStatic[1] = (CWndStatic*)GetDlgItem( WIDC_STATIC11 );
		m_pWndHeroStatic[1]->EnableWindow(TRUE);
		m_pWndHeroStatic[1]->SetVisible(TRUE);
	}
	else
	{
		m_pWndHeroStatic[0] = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
		m_pWndHeroStatic[0]->EnableWindow(FALSE);
		m_pWndHeroStatic[0]->SetVisible(FALSE);
		
		m_pWndHeroStatic[1] = (CWndStatic*)GetDlgItem( WIDC_STATIC11 );
		m_pWndHeroStatic[1]->EnableWindow(FALSE);
		m_pWndHeroStatic[1]->SetVisible(FALSE);
		
		CRect rect = GetWindowRect();
		rect.bottom -= 100;
		m_bSlot[3] = FALSE;
		
		SetWndRect(rect);
	}
#endif //__CSC_VER9_1

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rect = GetWindowRect();
	int x = rectRoot.right - rect.Width();
	int y = rectRoot.bottom - rect.Height();

	CPoint point( x, y );
	Move( point );
}
BOOL CWndSkillTreeEx::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint( 792, 130 ), pWndParent );
}
BOOL CWndSkillTreeEx::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	if( m_pFocusItem && 0 < g_pPlayer->m_nSkillPoint )
	{
		ItemProp* pSkillProp = prj.GetSkillProp( m_pFocusItem->dwSkill );
		if( pSkillProp )
		{
			int nPoint = prj.GetSkillPoint( pSkillProp );
			switch( nID )
			{
			case WIDC_BUTTON1:	// + 버튼
				{
					if( nPoint <= m_nCurrSkillPoint )
					{
						if( m_pFocusItem->dwLevel < pSkillProp->dwExpertMax )
						{
							m_nCurrSkillPoint -= nPoint;
							++m_pFocusItem->dwLevel;
						}
						else
						{
							break;							
						}
					}					
				}
				break;
			case WIDC_BUTTON2:	// - 버튼
				{
					if( IsDownPoint(m_pFocusItem->dwSkill) )
					{
						m_nCurrSkillPoint += nPoint;
						--m_pFocusItem->dwLevel;
						SubSkillPointDown( m_pFocusItem );
					}
				}
				break;
			case WIDC_BUTTON3:	// Reset 버튼
				{
					if( m_nCurrSkillPoint != g_pPlayer->m_nSkillPoint )
						InitItem(g_pPlayer->GetJob(), g_pPlayer->m_aJobSkill, TRUE );
					m_pFocusItem = NULL;
				}
				break;
			case WIDC_BUTTON4:	// Finish 버튼
				{
					if( m_nCurrSkillPoint != g_pPlayer->m_nSkillPoint )
					{
						SAFE_DELETE(g_WndMng.m_pWndReSkillWarning);
						g_WndMng.m_pWndReSkillWarning = new CWndReSkillWarning;
						g_WndMng.m_pWndReSkillWarning->Initialize(NULL);
					}
				}
				break;
			}
		}
	}

	if( nID == 10000 )
	{
		if( m_nCurrSkillPoint != g_pPlayer->m_nSkillPoint )
		{
			CWndBase* pWndBase = (CWndBase*)g_WndMng.GetWndBase( APP_QUEITMWARNING );	
			if( pWndBase == NULL )
			{
				SAFE_DELETE(g_WndMng.m_pWndReSkillWarning);
				g_WndMng.m_pWndReSkillWarning = new CWndReSkillWarning;
				g_WndMng.m_pWndReSkillWarning->Initialize(NULL);
				g_WndMng.m_pWndReSkillWarning->InitItem( TRUE );
			}
			return TRUE;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
BOOL CWndSkillTreeEx::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndSkillTreeEx::SetJob( int nJob )
{
	m_nJob = nJob;
}
void CWndSkillTreeEx::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bDrag = FALSE;
}
void CWndSkillTreeEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( m_apSkill == NULL )
		return;

	CPoint ptMouse = GetMousePoint();


	CWndStatic* pStatic	= (CWndStatic *)GetDlgItem( WIDC_STATIC2 );
	CRect rect = pStatic->GetWindowRect(TRUE);
	if( rect.PtInRect( point ) ) 
	{
		if( m_bSlot[0] )
		{
			SetActiveSlot(0, FALSE );
		}
		else
		{
			SetActiveSlot(0, TRUE );
		}
	}

	pStatic	= (CWndStatic *)GetDlgItem( WIDC_STATIC3 );
	rect = pStatic->GetWindowRect(TRUE);
	if( rect.PtInRect( point ) ) 
	{
		if( m_bSlot[1] )
		{
			SetActiveSlot(1, FALSE );
#if __VER >= 10 // __CSC_VER9_1
			if(FULLSCREEN_HEIGHT == 600 && m_bLegend)
			{
				SetActiveSlot(2, TRUE );
				CRect rectRoot = m_pWndRoot->GetLayoutRect();
				CRect rect = GetWindowRect();
				int x = this->m_rectWindow.left;
				int y = rectRoot.bottom - rect.Height();
				
				CPoint point( x, y );
				Move( point );
			}
#endif //__CSC_VER9_1
		}
		else
		{
			SetActiveSlot(1, TRUE );
#if __VER >= 10 // __CSC_VER9_1
			if(FULLSCREEN_HEIGHT == 600 && m_bLegend)
			{
				SetActiveSlot(2, FALSE );
				CRect rectRoot = m_pWndRoot->GetLayoutRect();
				CRect rect = GetWindowRect();
				int x = this->m_rectWindow.left;
				int y = rectRoot.bottom - rect.Height();
				
				CPoint point( x, y );
				Move( point );
			}
#endif //__CSC_VER9_1
		}
	}

	pStatic	= (CWndStatic *)GetDlgItem( WIDC_STATIC6 );
	rect = pStatic->GetWindowRect(TRUE);
	if( rect.PtInRect( point ) ) 
	{
		if( m_bSlot[2] )
		{
			SetActiveSlot(2, FALSE );
#if __VER >= 10 // __CSC_VER9_1
			if(FULLSCREEN_HEIGHT == 600 && m_bLegend)
			{
				SetActiveSlot(1, TRUE );
				CRect rectRoot = m_pWndRoot->GetLayoutRect();
				CRect rect = GetWindowRect();
				int x = this->m_rectWindow.left;
				int y = rectRoot.bottom - rect.Height();
				
				CPoint point( x, y );
				Move( point );
			}
#endif //__CSC_VER9_1
		}
		else
		{
			SetActiveSlot(2, TRUE );
#if __VER >= 10 // __CSC_VER9_1
			if(FULLSCREEN_HEIGHT == 600 && m_bLegend)
			{
				SetActiveSlot(1, FALSE );
				CRect rectRoot = m_pWndRoot->GetLayoutRect();
				CRect rect = GetWindowRect();
				int x = this->m_rectWindow.left;
				int y = rectRoot.bottom - rect.Height();
				
				CPoint point( x, y );
				Move( point );
			}
#endif //__CSC_VER9_1			
		}
	}

#if __VER >= 10 // __CSC_VER9_1
	if(m_bLegend)
	{
		pStatic	= (CWndStatic *)GetDlgItem( WIDC_STATIC1 );
		rect = pStatic->GetWindowRect(TRUE);
		if( rect.PtInRect( point ) ) 
		{
			if( m_bSlot[3] )
				SetActiveSlot(3, FALSE );
			else
				SetActiveSlot(3, TRUE );
		}
	}
#endif
	
				
	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
	{
		LPSKILL pSkill = GetSkill( i );
		if( pSkill == NULL ) 
			continue;
		DWORD dwSkill = pSkill->dwSkill;

		ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
		
		if( pSkillProp )
		{
			if( !m_bSlot[0] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_BASE )
					continue;
			}
			
			if( !m_bSlot[1] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_EXPERT )
					continue;
			}
			
			if( !m_bSlot[2] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_PRO )
					continue;
			}		
			
#if __VER >= 10 // __CSC_VER9_1
			if( !m_bSlot[3] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_MASTER || pSkillProp->dwItemKind1 == JTYPE_HERO )
					continue;
			}
#endif //__CSC_VER9_1
			GetCalcImagePos( pSkillProp->dwItemKind1 );
		}

		if( pSkillProp && pSkillProp->nLog != 1 && dwSkill != NULL_ID )
		{
			CRect rect;
			if( GetSkillPoint( pSkillProp->dwID, rect ) && CheckSkill( i ) )
			{
				rect.top  -= 2;
				
				rect.right  = rect.left+27;
				rect.bottom = rect.top+27;

				rect.OffsetRect( 0, m_nTopDownGap );
				
				if( rect.PtInRect(ptMouse) )
				{
					g_nSkillCurSelect = i;
					m_pFocusItem = &m_apSkill[ i ];
					m_bDrag = TRUE;
					break;
				}			
			}	
		}
	}
}
void CWndSkillTreeEx::OnRButtonDblClk( UINT nFlags, CPoint point)
{

}	

void CWndSkillTreeEx::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	// 스킬바 등록 및 스킬레벨 업
	if( m_apSkill == NULL )
		return;

	// 스킬창에서 더블클릭하면 자동으로 스킬바에 등록된다.
	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
	{
		LPSKILL pSkill = GetSkill( i );
		if( pSkill == NULL ) 
			continue;
		DWORD dwSkill = pSkill->dwSkill;
		
		ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
		
		if( pSkillProp )
		{
			if( !m_bSlot[0] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_BASE )
					continue;
			}
			
			if( !m_bSlot[1] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_EXPERT )
					continue;
			}
			
			if( !m_bSlot[2] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_PRO )
					continue;
			}		
#if __VER >= 10 // __CSC_VER9_1
			if( !m_bSlot[3] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_MASTER || pSkillProp->dwItemKind1 == JTYPE_HERO )
					continue;
			}
#endif //__CSC_VER9_1
			
			GetCalcImagePos( pSkillProp->dwItemKind1 );
		}

		if( pSkillProp && pSkillProp->nLog != 1 && dwSkill != NULL_ID )
		{
			CRect rect;
			if( GetSkillPoint( pSkillProp->dwID, rect ) && CheckSkill( i ) )
			{
				rect.top  -= 2;
				
				rect.right  = rect.left+27;
				rect.bottom = rect.top+27;
				
				CPoint ptMouse = GetMousePoint();

				rect.OffsetRect( 0, m_nTopDownGap );
				
				if( rect.PtInRect(ptMouse) )
				{
					g_nSkillCurSelect = i;
					m_pFocusItem = &m_apSkill[ i ];

					if( g_pPlayer->CheckSkill( m_pFocusItem->dwSkill ) == FALSE )
						return;
					CWndTaskBar* pTaskBar = g_WndMng.m_pWndTaskBar;
					if( pTaskBar->m_nExecute == 0 )		// 스킬큐가 사용되지 않을때만 등록됨.
						pTaskBar->SetSkillQueue( pTaskBar->m_nCurQueueNum, 0, i, m_atexSkill[i] );
					break;
				}			
			}	
		}
	}
}

void CWndSkillTreeEx::AfterSkinTexture( LPWORD pDest, CSize size, D3DFORMAT d3dFormat )
{
	CPoint pt;
	LPWNDCTRL lpWndCtrl;
 
	CPoint pt2 = m_rectClient.TopLeft() - m_rectWindow.TopLeft();
	
	lpWndCtrl = GetWndCtrl( WIDC_STATIC5 );
	pt = lpWndCtrl->rect.TopLeft() + pt2;

	pt.y += 2;
	
	GetCalcImagePos(JTYPE_EXPERT);
	pt.y += m_nTopDownGap;

	if( m_atexJobPannel[ 0 ] && m_bSlot[1] ) PaintTexture( pDest, m_atexJobPannel[ 0 ], pt, size );

	lpWndCtrl = GetWndCtrl( WIDC_STATIC7 );
	pt = lpWndCtrl->rect.TopLeft() + pt2;
	GetCalcImagePos(JTYPE_PRO);
	pt.y += 2;
	pt.y += m_nTopDownGap;
	
	if( m_atexJobPannel[ 1 ] && m_bSlot[2] ) PaintTexture( pDest, m_atexJobPannel[ 1 ], pt, size );


}

#endif // __NEW_SKILL_TREE
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 돈 거래 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndTradeGold::CWndTradeGold() 
{ 
	m_dwGold = 0;
	m_nIdWndTo = APP_INVENTORY;
	m_pWndBase = NULL;
	m_nSlot = 0;
	m_nPutSlot = 100;
} 
CWndTradeGold::~CWndTradeGold() 
{ 
} 
BOOL CWndTradeGold::Process( void )
{
	CItemBase* pItemBase = g_pPlayer->GetItemId( m_Shortcut.m_dwId );
	if( pItemBase == NULL )
		return TRUE;

	if( ( ( CItemElem* )pItemBase)->GetExtra() > 0 )
	{
		Destroy();
	}
	return TRUE;
}
void CWndTradeGold::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndTradeGold::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT );
	CWndButton* pWndOk = (CWndButton *)GetDlgItem( WIDC_OK );	
	pWndOk->SetDefault( TRUE );
	pWndEdit->SetFocus();

	if( m_dwGold > INT_MAX )
		m_dwGold = INT_MAX;
	
	TCHAR szNumber[ 64 ];
	_itot( m_dwGold, szNumber, 10 );
	pWndEdit->SetString( szNumber );
}

BOOL CWndTradeGold::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	return InitDialog( g_Neuz.GetSafeHwnd(), APP_TRADE_GOLD, WBS_MODAL, 0, pWndParent );
}
BOOL CWndTradeGold::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndTradeGold::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndTradeGold::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndTradeGold::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndTradeGold::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK || message == EN_RETURN )
	{
		CRect rect = GetClientRect();
		CWorld* pWorld	= g_WorldMng.Get();
		CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT );

		CString string = pWndEdit->GetString();
		int nCost = _ttoi( string );
		if( nCost < 1 )
			nCost = 1;
		if( m_nIdWndTo == APP_INVENTORY )
		{
			if( nCost > g_pPlayer->GetGold() )
				nCost = g_pPlayer->GetGold();

			g_DPlay.SendDropGold( nCost, g_pPlayer->GetPos(), m_vPos );
		}
		else
		if( m_nIdWndTo == APP_TRADE )
		{
			if( m_Shortcut.m_dwData == 0 ) // 돈
			{
				if( nCost > g_pPlayer->GetGold() )
					nCost = g_pPlayer->GetGold();

				if( nCost > 0 )
				{
					g_DPlay.SendTradePutGold( nCost );
				}
				else
				{
					// error message
				}
			}
			else // 아이템
			{
				CItemBase* pItemBase = g_pPlayer->GetItemId( m_Shortcut.m_dwId );
				if( pItemBase )
				{
					if( nCost > ( (CItemElem*)pItemBase )->m_nItemNum )
					{
						nCost = ( (CItemElem*)pItemBase )->m_nItemNum;
					}

					m_Shortcut.m_dwData -= 100;
					g_DPlay.SendTradePut( (BYTE)( m_Shortcut.m_dwData ), (BYTE)( m_Shortcut.m_dwType ), (BYTE)( m_Shortcut.m_dwId ), nCost );
				}
			}
		}
		else
		if( m_nIdWndTo == APP_BANK )
		{
			if( m_Shortcut.m_dwData == 0 ) // 돈
			{
				if( nCost > g_pPlayer->GetGold() )
					nCost = g_pPlayer->GetGold();

				g_DPlay.SendPutGoldBank( m_nSlot, nCost );
			}
			else // 아이템
			{
				CItemBase* pItemBase = g_pPlayer->GetItemId( m_Shortcut.m_dwId );
				if( pItemBase )
				{
					if( nCost > ( (CItemElem*)pItemBase )->m_nItemNum )
					{
						nCost = ( (CItemElem*)pItemBase )->m_nItemNum;
					}
					m_Shortcut.m_dwData -= 100;
					g_DPlay.SendPutItemBank( m_nSlot, (BYTE)( m_Shortcut.m_dwId ), nCost );
				}
			}
		}
#if __VER >= 11 // __SYS_POCKET
		else
		if( m_nIdWndTo == APP_BAG_EX )
		{
			if( m_Shortcut.m_dwData != 0 )
			{
				
				if( nCost > (int)( ( m_dwGold ) ) )
				{
					nCost = m_dwGold;
				}
				m_Shortcut.m_dwData -= 100;
				//g_DPlay.SendPutItemBank( m_nSlot, m_Shortcut.m_dwId, nCost );
				g_DPlay.SendMoveItem_Pocket( m_nPutSlot, m_Shortcut.m_dwId, nCost , m_nSlot);
				
			}
		}
#endif
		// 자신의 인벤토리에서 길드창고로 아이템을 이동
		else
		if (m_nIdWndTo == APP_GUILD_BANK)
		{
			if( m_Shortcut.m_dwData != 0 ) // 아이템
			{
				CItemBase* pItemBase = g_pPlayer->GetItemId( m_Shortcut.m_dwId );
				if( pItemBase )
				{
					if( nCost > ( (CItemElem*)pItemBase )->m_nItemNum )
					{
						nCost = ( (CItemElem*)pItemBase )->m_nItemNum;
					}
					
					m_Shortcut.m_dwData -= 100;

					g_DPlay.SendPutItemGuildBank( (BYTE)( m_Shortcut.m_dwId ), nCost, 1 );

				}
			}
		}
		//	길드창고에서 자신의 인벤으로 가져올때
		//	APP_MESSENGER로 그냥 설정
		else
		if (m_nIdWndTo == APP_MESSENGER)
		{
			if (g_pPlayer->GetGuild())
			{
				if( m_Shortcut.m_dwData == 0 ) // 돈
				{
					if( (DWORD)nCost > ( g_pPlayer->GetGuild()->m_nGoldGuild ) )
						nCost = g_pPlayer->GetGuild()->m_nGoldGuild;
					
					if( nCost > 0 )
					{
						g_DPlay.SendGetItemGuildBank( (BYTE)( m_Shortcut.m_dwId ), nCost, 0); // 0은 페냐가 변경됐다는 것을 의미한다.
					}
				}
				else // 아이템
				{
					CItemBase* pItemBase = NULL;
					if( m_Shortcut.m_dwType == ITYPE_ITEM )
					{
						pItemBase = g_pPlayer->GetGuild()->m_GuildBank.GetAtId( m_Shortcut.m_dwId );
					}
					else
					{
						//					assert(0);
					}

					if( pItemBase )
					{
						if( nCost > ( (CItemElem*)pItemBase )->m_nItemNum )
						{
							nCost = ( (CItemElem*)pItemBase )->m_nItemNum;
						}
						
						m_Shortcut.m_dwData -= 100;
						g_DPlay.SendGetItemGuildBank( (BYTE)( m_Shortcut.m_dwId ), nCost, 1 ); // 1은 아이템이 변경됐다는 것을 의미한다.
					}
				}
			}
		}

		else
		// 인벤토리 인데 쓰고 있어서 Gold로 씀.
		if( m_nIdWndTo == APP_GOLD ) // 뱅크에서 인벤토리로 빼옴
		{
			if( m_Shortcut.m_dwData == 0 ) // 돈
			{
				if( nCost > (int)( g_pPlayer->m_dwGoldBank[m_nSlot] ) )
					nCost = g_pPlayer->m_dwGoldBank[m_nSlot];
				
				if( nCost > 0 )
				{
					g_DPlay.SendGetGoldBank( m_nSlot, nCost );
				}
				else
				{
					// error message
				}
			}
			else // 아이템
			{
				CItemBase* pItemBase = NULL;
				if( m_Shortcut.m_dwType == ITYPE_ITEM )
				{
					pItemBase = g_pPlayer->GetItemBankId( m_nSlot, m_Shortcut.m_dwId );

					if( pItemBase )
					{
						if( nCost > ( (CItemElem*)pItemBase )->m_nItemNum )
						{
							nCost = ( (CItemElem*)pItemBase )->m_nItemNum;
						}
						
						m_Shortcut.m_dwData -= 100;
						g_DPlay.SendGetItemBank( m_nSlot, (BYTE)( m_Shortcut.m_dwId ), nCost );
					}
				}
				else
				{
					//					assert(0);
				}
			}
		}
		else
		if( m_nIdWndTo == APP_COMMON_BANK )
		{
			if( m_Shortcut.m_dwData == 0 ) // 돈
			{
				if( nCost > (int)( g_pPlayer->m_dwGoldBank[m_nPutSlot] ) )
					nCost = g_pPlayer->m_dwGoldBank[m_nPutSlot];
				
				if( nCost > 0 ) 
				{
					g_DPlay.SendPutGoldBankToBank( m_nPutSlot, m_nSlot, nCost );
				}
				else
				{
					// error message
				}
			}
			else // 아이템
			{
				CItemBase* pItemBase = NULL;
				if( m_Shortcut.m_dwType == ITYPE_ITEM )
				{
					pItemBase = g_pPlayer->GetItemBankId( m_nPutSlot, m_Shortcut.m_dwId );
					if( pItemBase )
					{				
						if( nCost > ( (CItemElem*)pItemBase )->m_nItemNum )
						{
							nCost = ( (CItemElem*)pItemBase )->m_nItemNum;
						}
						
						m_Shortcut.m_dwData -= 100;
						g_DPlay.SendPutItemBankToBank( m_nPutSlot, m_nSlot, (BYTE)( m_Shortcut.m_dwId ), nCost );
					}
				}
				else
				{
					//					assert(0);
				}
			}
		}
		else
		if( m_nIdWndTo == APP_POST_SEND )
		{
			if( m_Shortcut.m_dwData == 0 ) // 돈
			{
				if( nCost > g_pPlayer->GetGold()  )
					nCost = g_pPlayer->GetGold();
				
				if( nCost > 0 ) 
				{
					// 보내기 창에 넣는다.
					if( g_WndMng.m_pWndPost )
					{
						g_WndMng.m_pWndPost->m_PostTabSend.SetCost(nCost);
					}
				}
				else
				{
					// error message
				}
			}
			else // 아이템
			{
				CItemBase* pItemBase = NULL;
				if( m_Shortcut.m_dwType == ITYPE_ITEM )
				{
					pItemBase = g_pPlayer->GetItemId( m_Shortcut.m_dwId );
					if( pItemBase )
					{				
						if( nCost > ( (CItemElem*)pItemBase )->m_nItemNum )
						{
							nCost = ( (CItemElem*)pItemBase )->m_nItemNum;
						}
						
						pItemBase->SetExtra( nCost );		

						m_Shortcut.m_dwData -= 100;

						if( g_WndMng.m_pWndPost )
						{
							g_WndMng.m_pWndPost->m_PostTabSend.SetItemId( (BYTE)( m_Shortcut.m_dwId ) );
							g_WndMng.m_pWndPost->m_PostTabSend.SetCount(nCost);
						}						
					}
				}
				else
				{
					//					assert(0);
				}
			}
		}			
		Destroy();
	}
	if( nID == WIDC_CANCEL )
	{
		if( m_nIdWndTo == APP_POST_SEND )
		{
			g_WndMng.m_pWndPost->m_PostTabSend.SetItemId(0xff);
			g_WndMng.m_pWndPost->m_PostTabSend.SetCount(0);
		}
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 거래 확인
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
CWndConfirmTrade::CWndConfirmTrade() 
{ 
	m_objid = 0;
} 
CWndConfirmTrade::~CWndConfirmTrade() 
{ 
} 
void CWndConfirmTrade::OnSetName( const char* szName, OBJID objid )
{
	m_objid = objid;
	CWndStatic *m_pName   = (CWndStatic*)GetDlgItem( WIDC_STATIC2 );
	CString sName;
	sName = szName;
	sName.Format( prj.GetText(TID_GAME_FROM),  szName ); // " 님에게"
	m_pName->SetTitle( sName );
}
void CWndConfirmTrade::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndConfirmTrade::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( ( rectRoot.right - rectWindow.Width() ) / 2, 70 );
	Move( point );	
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndConfirmTrade::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	m_objid = 0;
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_CONFIRM_TRADE, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndConfirmTrade::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndConfirmTrade::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndConfirmTrade::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndConfirmTrade::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndConfirmTrade::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK )
	{
		CMover* pTrader	= prj.GetMover( m_objid );
		if( pTrader != NULL )
		{
			g_DPlay.SendTrade( pTrader );
		}
		Destroy();		// 수동파괴로 바꿈.
	}
	else 
	if( nID == WIDC_CANCEL || nID == WTBID_CLOSE )
	{
		g_DPlay.SendConfirmTradeCancel( m_objid );
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 거래 최종 확인
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
CWndTradeConfirm::CWndTradeConfirm() 
{ 
} 
CWndTradeConfirm::~CWndTradeConfirm() 
{ 
} 
void CWndTradeConfirm::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndTradeConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	CWndButton * pWndButtonOk = (CWndButton*)GetDlgItem( WIDC_YES );
	pWndButtonOk->SetVisible( TRUE );
	CWndButton * pWndButtonNO = (CWndButton*)GetDlgItem( WIDC_NO );
	pWndButtonNO->SetVisible( TRUE );

	bMsg = FALSE;

	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
	pWndStatic->SetTitle( prj.GetText( TID_DIAG_0083 ) );
					
	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndTradeConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_TRADE_CONFIRM, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndTradeConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndTradeConfirm::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndTradeConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndTradeConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndTradeConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( bMsg )
	{
		return( TRUE );
	}

	if( nID == WIDC_NO || nID == WTBID_CLOSE ) // 취소 
	{
		g_DPlay.SendTradeCancel();
	}
	else if ( nID == WIDC_YES )
	{
		g_DPlay.SendTradeConfirm();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 거래 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndTrade::CWndTrade()
{
	SetPutRegInfo( FALSE );
}
CWndTrade::~CWndTrade()
{
	g_DPlay.SendTradeCancel();
	SAFE_DELETE( g_WndMng.m_pWndTradeGold );
}

void CWndTrade::OnDraw(C2DRender* p2DRender)
{
	CWorld* pWorld = g_WorldMng.Get();
	CMover* pMover	= g_pPlayer->m_vtInfo.GetOther();
	if( pMover && pMover->GetType() == OT_MOVER )
	{
		CWndStatic* pWndNameYou = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
		CWndStatic* pWndNameI = (CWndStatic*)GetDlgItem( WIDC_STATIC2 );
		CWndStatic* pWndGoldYou = (CWndStatic*)GetDlgItem( WIDC_STATIC3 );
		CWndStatic* pWndGoldI = (CWndStatic*)GetDlgItem( WIDC_STATIC4 );

		pWndNameYou->SetTitle( pMover->GetName( TRUE ) );
		pWndNameI->SetTitle( g_pPlayer->GetName() );

		CString string;
		int nLength;
		string.Format( "%d", m_nGoldYou );
		
		nLength = string.GetLength();
		while(nLength - 3 > 0)
		{
			nLength -= 3;
			string.Insert(nLength, ',');
		}
		pWndGoldYou->SetTitle( string );

		string.Format( "%d", m_nGoldI );
		nLength = string.GetLength();
		while(nLength - 3 > 0)
		{
			nLength -= 3;
			string.Insert(nLength, ',');
		}
		pWndGoldI->SetTitle( string );
		
	}
	else
	{
		g_pPlayer->m_vtInfo.TradeClear();
	}
}
void CWndTrade::OnInitialUpdate()
{
	m_wndItemCtrlYou.Create( WLVS_ICON|WBS_NODRAWFRAME, CRect(   5, 25 + 15,   5 + 32 * 5 + 5, 25 + 32 * 5 + 5 + 15 ), this, 10001 );
	m_wndItemCtrlI.  Create( WLVS_ICON|WBS_NODRAWFRAME, CRect( 175, 25 + 15, 175 + 32 * 5 + 5, 25 + 32 * 5 + 5 + 15 ), this, 10002 );

	m_nGoldI = 0;
	m_nGoldYou = 0;

	CWorld* pWorld = g_WorldMng.Get();
	CMover* pMover	= g_pPlayer->m_vtInfo.GetOther();

	if( pMover )
	{
		pMover->m_vtInfo.TradeClear();
		pMover->m_vtInfo.SetOther( g_pPlayer );
		m_wndItemCtrlYou.InitItem( pMover );
		m_wndItemCtrlI.InitItem( g_pPlayer );
	}

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
	
	CWndNeuz::OnInitialUpdate();

#if __VER >= 11 // __CSC_VER11_1
	CWndButton* pClearButton = (CWndButton*)GetDlgItem(WIDC_CLEAR);
	if(pClearButton)
	{
		pClearButton->EnableWindow(FALSE);
		pClearButton->SetVisible(FALSE);
	}
#endif //__CSC_VER11_1
#if __VER >= 11 // __SYS_POCKET
	if(GetWndBase( APP_BAG_EX )) GetWndBase( APP_BAG_EX )->Destroy();
#endif
	if( GetWndBase(APP_WEBBOX) || g_WndMng.m_pWndShop || g_WndMng.m_pWndBank || g_WndMng.m_pWndGuildBank || g_WndMng.GetWndVendorBase() )
	{
		Destroy();
		return;
	}
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndTrade::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	CWorld* pWorld = g_WorldMng.Get();
	CMover* pMover	= g_pPlayer->m_vtInfo.GetOther();
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_TRADE, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndTrade::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	CWndTradeConfirm* pWndTradeConfirm = (CWndTradeConfirm*)g_WndMng.GetWndBase( APP_TRADE_CONFIRM );
	if( pWndTradeConfirm )
	{
		return( TRUE );
	}
	if( message == WIN_ITEMDROP && ( nID == 10001 || nID == 10002 ) )
	{
		BOOL TradeGold = FALSE;
		SAFE_DELETE( g_WndMng.m_pWndTradeGold );
		g_WndMng.m_pWndTradeGold = new CWndTradeGold;
		memcpy( &g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT) );
		if( g_WndMng.m_pWndTradeGold->m_Shortcut.m_dwData == 0 ) // 돈
		{
			TradeGold = TRUE;
 			g_WndMng.m_pWndTradeGold->m_dwGold = g_pPlayer->GetGold();
		}
		else // 아이템
		{
			CItemBase* pItemBase = g_pPlayer->GetItemId( g_WndMng.m_pWndTradeGold->m_Shortcut.m_dwId );
			g_WndMng.m_pWndTradeGold->m_dwGold = ( (CItemElem*)pItemBase )->m_nItemNum;
		}
		g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_TRADE;
		g_WndMng.m_pWndTradeGold->m_pWndBase = this;
		
		if( TradeGold )
		{
			if( m_nGoldI == 0 )
			{
				g_WndMng.m_pWndTradeGold->Initialize( &g_WndMng, APP_TRADE_GOLD );
				g_WndMng.m_pWndTradeGold->MoveParentCenter();
				CWndStatic* pStatic	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC );
				CWndStatic* pStaticCount	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_CONTROL1 );
				g_WndMng.m_pWndTradeGold->SetTitle( "Penya" );
				CString strMain = prj.GetText( TID_GAME_MOVEPENYA );//"얼마를 이동하시겠습니까?";
				CString strCount = prj.GetText(TID_GAME_PENYACOUNT);//" SEED : ";
				pStatic->m_strTitle = strMain;
				pStaticCount->m_strTitle = strCount;
			}
			else
			{
				SAFE_DELETE( g_WndMng.m_pWndTradeGold );
			}
		}
		else
		{
			g_WndMng.m_pWndTradeGold->Initialize( &g_WndMng, APP_TRADE_GOLD );
			g_WndMng.m_pWndTradeGold->MoveParentCenter();
			CWndStatic* pStatic	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC );
			CWndStatic* pStaticCount	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_CONTROL1 );
			g_WndMng.m_pWndTradeGold->SetTitle( "Item" );
			CString strMain = prj.GetText( TID_GAME_MOVECOUNT );//"몇개를 이동하시겠습니까?";
			CString strCount = prj.GetText(TID_GAME_NUMCOUNT);//" 갯수 : ";
			pStatic->m_strTitle = strMain;
			pStaticCount->m_strTitle = strCount;
		}
	}
	else
	{
		if( nID == WIDC_CLEAR )	// clear
		{
		}
		else
		{
			if( nID == WIDC_OK_I )	// 확인
			{
				g_DPlay.SendTradeOk();
			}
			else if( nID == WIDC_CANCEL || nID == WTBID_CLOSE ) // 취소 
			{
				g_DPlay.SendTradeCancel();
				return( TRUE );
			}
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
void CWndTrade::DoCancel()
{
	CMover* pTrader	= g_pPlayer->m_vtInfo.GetOther();
	if( pTrader )
		pTrader->m_vtInfo.TradeClear();
	g_pPlayer->m_vtInfo.TradeClear();
	
	Destroy();
}

BOOL CWndTrade::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand(nID,dwMessage,pWndBase);
}

void CWndTrade::OnSize(UINT nType, int cx, int cy)
{
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize(70,25);

	CRect rect1_1( x - ( size.cx / 2), y, ( x - ( size.cx / 2 ) ) + size.cx, y + size.cy );

	CRect rect2_1( x - size.cx - 10, y, ( x - size.cx - 10 ) + size.cx, y + size.cy );
	CRect rect2_2( x + 10          , y, ( x + 10           ) + size.cx, y + size.cy );

	CRect rect3_1( x - ( size.cx / 2) - size.cx - 10, y, (x - ( size.cx / 2) - size.cx - 10) + size.cx, y + size.cy );
	CRect rect3_2( x - ( size.cx / 2)               , y, (x - ( size.cx / 2)               ) + size.cx, y + size.cy );
	CRect rect3_3( x + ( size.cx / 2) + 10          , y, (x + ( size.cx / 2) + 10          ) + size.cx, y + size.cy );

	CWndNeuz::OnSize(nType,cx,cy);
}
void CWndTrade::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(IsWndRoot())
		return;
}
void CWndTrade::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(IsWndRoot())
		return;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 네비게이터 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#define OBJFILTER_PLAYER   0
#define OBJFILTER_PARTY    1 
#define OBJFILTER_NPC      2
#define OBJFILTER_MONSTER  3 

CWndNavigator::CWndNavigator()
{
	m_bObjFilterPlayer  = TRUE;
	m_bObjFilterParty   = TRUE;
	m_bObjFilterNPC     = TRUE;
	m_bObjFilterMonster = TRUE;
	m_szName[ 0 ] = 0;
	m_iFrame = 0;
	m_iPastTime = 0;
	m_size.cx = 0;
	m_size.cy = 0;
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	m_pDestinationPositionTexture = NULL;
#endif // __IMPROVE_QUEST_INTERFACE

}
CWndNavigator::~CWndNavigator()
{
	//m_billTest.DeleteDeviceObjects();
}

void CWndNavigator::SetRegionName( TCHAR *tszName )
{
	CString strTitle = tszName;
    CWndStatic STitle;

	if( strTitle.IsEmpty() )
		strTitle = prj.GetText( TID_APP_NAVIGATOR );

	STitle.Create( strTitle,0,CRect(0,8,256,8),this,110);
	SetTitle( strTitle );
}

BOOL CWndNavigator::OnEraseBkgnd(C2DRender* p2DRender)
{

	if( g_pPlayer == NULL ) return TRUE;
	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

	CWorld* pWorld	= g_WorldMng();
	CRect rect = GetClientRect();

	// 포커스 오브젝트 출력 
	CObj* pObjFocus = pWorld->GetObjFocus();
		
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	CLandscape* pLand = pWorld->GetLandscape( pWorld->m_pCamera->m_vPos );

	if( pLand == NULL ) return 1;
	int nImageBlock = pLand->m_texMiniMap.m_size.cx;


    CRect rectN = GetClientRect();

    m_ALPHACIRCLE.m_size = CSize( rectN.Width(), rectN.Height());
	p2DRender->RenderTexture( CPoint( 0, 0 ), &m_ALPHACIRCLE, 200 );

	//pLand->m_ALPHACIRCLE.m_size = CSize( rectN.Width(), rectN.Height());
	//p2DRender->RenderTexture( CPoint( 0, 0 ), &pLand->m_ALPHACIRCLE, 200 );
	
	CRectClip clipRect( 0, 0, p2DRender->m_clipRect.Width(), p2DRender->m_clipRect.Height() );
	CPoint point;
	CRect rectCur;

	FLOAT fx = (FLOAT)m_size.cx / ( MAP_SIZE * MPU );
	FLOAT fy = (FLOAT)m_size.cy / ( MAP_SIZE * MPU );

	D3DXVECTOR3 vPos, vCenter = ( g_pPlayer != NULL ? g_pPlayer->GetPos() : D3DXVECTOR3( 0, 0, 0 ) );
	vCenter.x *= fx;
	vCenter.z *= fy;
	int xCenter = (int)( vCenter.x );
	int yCenter = (int)( - vCenter.z );
	int i;

	BOOL  bDrawGuildWarNum = FALSE;
	DWORD dwWorldID = pWorld->GetID();

	if( dwWorldID == WI_WORLD_GUILDWAR )
		bDrawGuildWarNum = TRUE;

	for( int z = 0; z < pWorld->m_nLandHeight; z++ )
	{
		for( int x = 0; x < pWorld->m_nLandWidth; x++ )
		{
			point = CPoint( x * nImageBlock, z * nImageBlock );
			point.y -= ( nImageBlock * pWorld->m_nLandWidth );
			point.x += rect.Width() / 2;
			point.y += rect.Height() / 2;
			point.x -= xCenter;
			point.y -= yCenter;

			rectCur.SetRect( point.x, point.y, point.x + nImageBlock, point.y + nImageBlock );
			CLandscape* pLand = pWorld->GetLandscape( x, pWorld->m_nLandHeight - z - 1);
			if( pLand )
			{
				pLand->m_texMiniMap.m_size = m_size;

				if( clipRect.RectLapRect( rectCur ) )
				{
					if( pLand->m_texMiniMap.m_pTexture )
					{
						pLand->m_texMiniMap.m_size = m_size;
						pLand->m_texMiniMap.Render( p2DRender, point, 50 );//CWndBase::m_nAlpha );//ademior
   
						if( bDrawGuildWarNum )
						{
							m_GuildCombatTextureMask.m_size = pLand->m_texMiniMap.m_size;
							m_GuildCombatTextureMask.Render( p2DRender, point, 200 );
						}
					}
				} 
			}
		}
	}

	AccuFrame();	//	간단한 프레임 계산을 해준다.

	if( pWorld->m_bIsIndoor == FALSE 
#if __VER >= 15 // __15TH_INSTANCE_DUNGEON
		&& !pWorld->IsWorldInstanceDungeon()
#endif // __15TH_INSTANCE_DUNGEON
		)
	{
		// 객체 출력을 위한 설정 
		TEXTUREVERTEX vertex[ 1000 * 6 ];
		TEXTUREVERTEX* pVertices = vertex; 

		int nCount = 0;
		int xu = 0, yv = 0;
		CRect aRcArray[ 100 ];
		int nRcCount = 0;
		
		////////////////////////////////////////////////////
		// 오브젝트 출력 
		////////////////////////////////////////////////////
		CObj* pObj;
		int nIndex = 0;
		int nPartyMap[ MAX_PTMEMBER_SIZE ];
		CMover* apQuest[ 100 ];
		int nQuestNum = 0;
		ZeroMemory( nPartyMap, sizeof(nPartyMap) );
		int nPartyMapCount = 0;
		float fDistMap = rect.Width() / 2 / fx;

		FOR_LAND( pWorld, pLand, pWorld->m_nVisibilityLand, FALSE )
		{

			FOR_OBJ( pLand, pObj, OT_MOVER )
			{
//				BOOL bPartyMap = FALSE;
				if( pObj != g_pPlayer )
				{
					CMover* pMover = (CMover*)pObj;						

					if( g_pPlayer->m_idMurderer != pMover->m_idPlayer )	// 찍을 타겟이 날 죽인넘이면 조건없이 무조건 찍는다.
					{
						// 플레이어필터가 아닌데, 플레이어라면 
						if( m_bObjFilterPlayer == FALSE && pMover->IsPlayer() == TRUE )
						{
							// 게다가 파티필터도 아니라면 스킵 
							if( m_bObjFilterParty == FALSE )
								continue;
							if( g_Party.IsMember( pMover->m_idPlayer ) == FALSE ) 
								continue;
						}
						if( m_bObjFilterMonster == FALSE && ( pMover->IsPlayer() == FALSE && pMover->IsPeaceful() == FALSE ) )
							continue;
						if( m_bObjFilterNPC == FALSE && ( pMover->IsPlayer() == FALSE && pMover->IsPeaceful() == TRUE ) )
						{
							// 상점이 아니라면 컨티뉴.
							LPCHARACTER lpCharacter = pMover->GetCharacter();
							if( lpCharacter == NULL || lpCharacter->m_nStructure == -1 ) 
								continue;
								
						}
						if( pMover->IsMode( TRANSPARENT_MODE ) || pMover->IsAuthHigher( AUTH_GAMEMASTER ) )	// 대상이 투명모드이거나 GM일경우 타겟 안됨.
							continue;
						
					}
					vPos = pObj->GetPos();
					vPos.x *= fx;
					vPos.z *= fy;
					point.x = (LONG)( ( rect.Width() / 2 ) + vPos.x );
					point.y = (LONG)( ( rect.Height() / 2 ) - vPos.z );
					point.x -= xCenter;
					point.y -= yCenter;
					
					CRect rectHittest( point.x, point.y, point.x + 5, point.y + 5);
					//rectHittest.InflateRect( 4, 4 );
					CPoint ptMouse = GetMousePoint();
					if( rectHittest.PtInRect( ptMouse ) )
					{
						ClientToScreen( &ptMouse );
						ClientToScreen( &rectHittest );
						g_toolTip.PutToolTip( 10000 + ((CMover*)pObj)->GetId(), ((CMover*)pObj)->GetName( TRUE ), rectHittest, ptMouse, 0 );
					}
					// 퀘스트 이모티콘 
					if( pMover->IsNPC() && pMover->m_nQuestEmoticonIndex != -1 )
					{
						apQuest[ nQuestNum++ ] = pMover;
					}
					else
					if( g_pPlayer->m_idMurderer && g_pPlayer->m_idMurderer == pMover->m_idPlayer )		// 찍을 타겟이 날 죽인넘이면 빨갛게 표시
						nIndex = 5;
					else
					if( pMover->IsNPC() && pMover->IsPeaceful() )
					{
						nIndex = 1;
					}
					else
					if( g_pPlayer->m_idGuild && g_pPlayer->m_idGuild == pMover->m_idGuild )
						nIndex = 3;
					else
					if( pMover->IsNPC() && pMover->IsPeaceful() == FALSE )
						nIndex = 2;
						//xu = 10, yv = 0;
					else
					if( pMover->IsPlayer() )
					{
						nPartyMap[nPartyMapCount] = g_Party.FindMember( pMover->m_idPlayer );
						if( nPartyMap[nPartyMapCount] != -1 )
						{
							g_Party.m_aMember[ nPartyMap[nPartyMapCount] ].m_vPos = pMover->GetPos();
							++nPartyMapCount;
							nIndex = 4;
						}
						else
							nIndex = 0;
					}
					if( nCount < 1000 )
					{
						nCount++;
						
						LPCHARACTER lpCharacter = pMover->GetCharacter();
						if( lpCharacter && lpCharacter->m_nStructure != -1 ) 
							m_texNavObjs.MakeVertex( p2DRender, CPoint( point.x - 8, point.y - 8 ), 6 + lpCharacter->m_nStructure, &pVertices );
						else
						{
							// 퀘스트가 없는 것만 출력 
							if( pMover->m_nQuestEmoticonIndex == -1 )
								m_texNavObjs.MakeVertex( p2DRender, CPoint( point.x - 2, point.y - 2 ), nIndex, &pVertices );
						}
					}
				}
			}
			END_OBJ
		}
		END_LAND
		////////////////////////////////////////////////////////
		// 리젼 오브젝트로 설정된 스트럭쳐 이모티콘 출력 
		////////////////////////////////////////////////////////
		int nSize = pWorld->m_aStructure.GetSize();
		for( i = 0; i < nSize; i++ )
		{
			LPREGIONELEM lpRegionElem = pWorld->m_aStructure.GetAt( i );
			vPos = lpRegionElem->m_vPos;
			vPos.x *= fx;
			vPos.z *= fy;
			point.x = (LONG)( ( rect.Width() / 2 ) + vPos.x );
			point.y = (LONG)( ( rect.Height() / 2 ) - vPos.z );
			point.x -= xCenter;
			point.y -= yCenter;
			m_texNavObjs.MakeVertex( p2DRender, CPoint( point.x - 8, point.y - 8 ), 6 + lpRegionElem->m_dwStructure, &pVertices );
		}
		////////////////////////////////////////////////////////
		// 퀘스트 이모티콘 출력 
		// 모든 아이콘 보다 맨 위에 찍혀야 되므로 여가서 몰아서 찍는다.
		////////////////////////////////////////////////////////
		for( i = 0; i < nQuestNum; i++ )
		{
			CMover* pMover = apQuest[ i ];
			vPos = pMover->GetPos();
			vPos.x *= fx;
			vPos.z *= fy;
			point.x = (LONG)( ( rect.Width() / 2 ) + vPos.x );
			point.y = (LONG)( ( rect.Height() / 2 ) - vPos.z );
			point.x -= xCenter;
			point.y -= yCenter;

			nIndex = pMover->m_nQuestEmoticonIndex + 20;
			m_texNavObjs.MakeVertex( p2DRender, CPoint( point.x - 5, point.y - 5 ), nIndex, &pVertices );		
/*			
			
			LPCHARACTER lpCharacter = pMover->GetCharacter();
			if( pMover->m_nQuestEmoticonIndex == 0 || pMover->m_nQuestEmoticonIndex == 2 ) 
			{
				if( lpCharacter && lpCharacter->m_nStructure == -1 )
				{
					nIndex = pMover->m_nQuestEmoticonIndex + 20;
					m_texNavObjs.MakeVertex( p2DRender, CPoint( point.x - 5, point.y - 5 ), nIndex, &pVertices );
				}
			}
			else
			if( pMover->m_nQuestEmoticonIndex == 1 || pMover->m_nQuestEmoticonIndex == 3 ) 
			{
				//if( lpCharacter && lpCharacter->m_nStructure != -1 )
				{
					nIndex = pMover->m_nQuestEmoticonIndex + 20;
					m_texNavObjs.MakeVertex( p2DRender, CPoint( point.x - 5, point.y - 5 ), nIndex, &pVertices );
				}
			}
*/
		}
/*	#ifdef __S1005_PARTY_MAP
		nPartyMap[nPartyMapCount] = g_Party.FindMember( g_pPlayer->m_idPlayer );
		if( nPartyMap[nPartyMapCount] != -1 )
			++nPartyMapCount;
		for( i = 0 ; i < g_Party.GetSizeofMember() && g_Party.GetSizeofMember() != nPartyMapCount ; ++i )
		{
			BOOL bDisplay = TRUE;
			for( int j = 0 ; j < nPartyMapCount ; ++j )
			{
				if( nPartyMap[j] == i )
				{
					bDisplay = FALSE;
					break;
				}
			}
					
			if( bDisplay )
			{
				D3DXVECTOR3 vPosBuf = g_pPlayer->GetPos() - g_Party.GetPos( i );
				float fDist = D3DXVec3Length( &vPosBuf );

				
				if( fDist > fDistMap )
				{
					RenderPartyMember( p2DRender, &pVertices, rect, g_Party.GetPos( i ), g_Party.m_aMember[i].m_uPlayerId, g_Party.m_aMember[i].m_szName );
				}
				else
				{
					vPos = g_Party.GetPos( i );
					vPos.x *= fx;
					vPos.z *= fy;
					point.x = ( rect.Width() / 2 ) + vPos.x;
					point.y = ( rect.Height() / 2 ) - vPos.z;
					point.x -= xCenter;
					point.y -= yCenter;
					m_texNavObjs.MakeVertex( p2DRender, CPoint( point.x - 2, point.y - 2 ), 3, &pVertices );

					CRect rectHittest( point.x, point.y, point.x + 5, point.y + 5);
					CPoint ptMouse = GetMousePoint();
					if( rectHittest.PtInRect( ptMouse ) )
					{
						ClientToScreen( &ptMouse );
						ClientToScreen( &rectHittest );
						g_toolTip.PutToolTip( 10000 + g_Party.m_aMember[i].m_uPlayerId, g_Party.m_aMember[i].m_szName, rectHittest, ptMouse, 0 );
					}
				}
			}
		}
	#endif // __S1005_PARTY_MAP
*/
		////////////////////////////////////////////////////
		// 포커스 오브젝트 출력 (좀더 분멸하게 보이도록)
		////////////////////////////////////////////////////
		if( pObjFocus )
		{
			vPos = pObjFocus->GetPos();
			vPos.x *= fx;
			vPos.z *= fy;
			point.x = (LONG)( ( rect.Width() / 2 ) + vPos.x );
			point.y = (LONG)( ( rect.Height() / 2 ) - vPos.z );
			point.x -= xCenter;
			point.y -= yCenter;

			xu = 9, yv = 9;

			point.x -= 3;
			point.y -= 3;

			if( nCount < 1000 )
			{
				nCount++;
				m_texNavObjs.MakeVertex( p2DRender, point, 5, &pVertices );
			}
		}
		m_texNavObjs.Render( m_pApp->m_pd3dDevice, vertex, ( (int) pVertices - (int) vertex ) / sizeof( TEXTUREVERTEX ) );

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
		D3DXVECTOR3& rDestinationArrow = g_WndMng.m_pWndWorld->m_vDestinationArrow;
		if( rDestinationArrow != D3DXVECTOR3( -1.0F, 0.0F, -1.0F ) )
		{
			vPos = rDestinationArrow;
			vPos.x *= fx;
			vPos.z *= fy;
			point.x = (LONG)( ( rect.Width() / 2 ) + vPos.x );
			point.y = (LONG)( ( rect.Height() / 2 ) - vPos.z );
			point.x -= xCenter;
			point.y -= yCenter;

			xu = 9, yv = 9;

			point.x -= 3;
			point.y -= 3;

			m_pDestinationPositionTexture->RenderScal( p2DRender, point, 255, 0.4F, 0.4F );
		}
#endif // __IMPROVE_QUEST_INTERFACE
	}
	else
	{
		m_texDunFog.m_size = CSize( rect.Width(), rect.Height() );
		p2DRender->RenderTexture( CPoint( 0, 0 ), &m_texDunFog, 255 );
	}


	m_wndPlace.EnableWindow( !pWorld->m_bIsIndoor );

	RenderMarkAll(p2DRender, g_pPlayer);

	//m_TEST.m_size = CSize( rect.Width(), rect.Height());
	//p2DRender->RenderTexture( CPoint( 0, 0 ), &m_TEST, 255 );

	// 화면 비율 때문에 임의로 정사각형 뷰포트를 지정해 놓는다. 안그러면 화살표 모양이 찌그러짐.
	D3DVIEWPORT9 viewport;
	viewport.X      = p2DRender->m_clipRect.left + ( p2DRender->m_clipRect.Width()  / 2 );
	viewport.Y      = p2DRender->m_clipRect.top  + ( p2DRender->m_clipRect.Height() / 2 );
	viewport.Width  = 32;
	viewport.Height = 32;
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;
	viewport.X      -= 16;
	viewport.Y      -= 16;

	pd3dDevice->SetViewport( &viewport );

	// 프로젝션 
	D3DXMATRIX matProj;
	D3DXMatrixIdentity( &matProj );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
	// 카메라 
	D3DXMATRIX  matView;
	D3DXMatrixIdentity( &matView );
	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	D3DXMATRIX mat, matWorld;
	D3DXMatrixIdentity( &matWorld );

	D3DXMatrixScaling( &mat, 1.2f, 1.2f, 1.2f );
	D3DXMatrixMultiply( &matWorld, &matWorld, &mat );
/*
	FLOAT fAngle	= ( g_pPlayer != NULL ? g_pPlayer->GetAngle() - 180 : 0 );
	if( fAngle < 0 ) fAngle = 360 + fAngle;
	FLOAT fTheta = D3DXToRadian( fAngle );// (2*D3DX_PI*g_pPlayer->m_nAngle)/(360);//m_nAngle
	D3DXMatrixRotationZ( &mat, fTheta );
*/

	D3DXVECTOR3 vDir      = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
	D3DXVECTOR3 vDestNor  = g_pPlayer->GetPos() - g_Neuz.m_camera.m_vPos;
	D3DXVECTOR3 vAxis;
	D3DXQUATERNION   qRot;
	
	FLOAT       fTheta;
	D3DXVec3Normalize( &vDestNor, &vDestNor );
	D3DXVec3Cross( &vAxis, &vDir, &vDestNor );
	fTheta = D3DXVec3Dot( &vDir, &vDestNor );
	D3DXQuaternionRotationAxis( &qRot, &vAxis, acosf( fTheta ) );
	
	D3DXVECTOR3 vYPW;
	
	QuaternionRotationToYPW( qRot, vYPW );
	D3DXMatrixRotationZ( &mat, -(vYPW.y) );
	
	D3DXMatrixMultiply( &matWorld, &matWorld, &mat );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );		
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );		
	
	// 화살표 출력 
	m_billArrow.Render( pd3dDevice );

	return TRUE;
}

void CWndNavigator::RenderMark( C2DRender* p2DRender, CMover* Player )
{

    CRect rect;

	FLOAT fx = (FLOAT)m_size.cx / ( MAP_SIZE * MPU );
	FLOAT fy = (FLOAT)m_size.cy / ( MAP_SIZE * MPU );
	if ( Player->m_nvPoint.Time != 0 )
	{	
		D3DXVECTOR3 Pos = g_pPlayer->GetPos() - Player->m_nvPoint.Pos;
		Pos.x *= fx ;
		Pos.z *= -fy;
		Player->m_nvPoint.Time--;

		CPoint point;
		CRect rect = GetClientRect();
		point.x = (LONG)( ( rect.Width() / 2 ) - ( Pos.x + 2 ) );
		point.y = (LONG)( ( rect.Height() / 2 ) - ( Pos.z + 2 ) );
		
		//m_texNavPos.MakeVertex(p2DRender, point, 1, &pVertices);
		m_texNavPos.Render( p2DRender, point, m_iFrame, 255, 0.5f, 0.5f);

		CRect rectHit( point.x - 8, point.y - 8, point.x + 8, point.y + 8);
		CPoint ptMouse = GetMousePoint();
		if( rectHit.PtInRect( ptMouse ) )
		{
//			CString toolTip("Mark : ");
			CString toolTip		= prj.GetText( TID_GAME_NAV_MARK );
			toolTip += Player->GetName( TRUE );
			ClientToScreen( &ptMouse );
			ClientToScreen( &rectHit );
			g_toolTip.PutToolTip( 10000 + Player->GetId(), toolTip, rectHit, ptMouse, 0 );
		}
	}
}

//	이걸 넣어 준다음에 정리를 위해서 NaviPoint형을 참조나 포인터로 받는 형태로 다시 만들어 주어야 할것이다.
void CWndNavigator::RenderMarkAll( C2DRender* p2DRender , CMover * Player )
{
	RenderMark( p2DRender, Player );

    CRect rect;

	FLOAT fx = (FLOAT)m_size.cx / ( MAP_SIZE * MPU );
	FLOAT fy = (FLOAT)m_size.cy / ( MAP_SIZE * MPU );

	V_NaviPoint::iterator nvi = g_pPlayer->m_vOtherPoint.begin();
	for ( int i = 0 ; i < (int)( g_pPlayer->m_vOtherPoint.size() ) ; )
	{
		if ( g_pPlayer->m_vOtherPoint[i].Time != 0 )
		{
			D3DXVECTOR3 Pos = g_pPlayer->GetPos() - g_pPlayer->m_vOtherPoint[i].Pos;
			Pos.x *= fx ;
			Pos.z *= -fy;
			g_pPlayer->m_vOtherPoint[i].Time--;

			CPoint point;
			CRect rect = GetClientRect();
			point.x = (LONG)( ( rect.Width() / 2 ) - ( Pos.x + 2 ) );
			point.y = (LONG)( ( rect.Height() / 2 ) - ( Pos.z + 2 ) );
			
			//m_texNavPos.MakeVertex(p2DRender, point, 1, &pVertices);
			m_texNavPos.Render( p2DRender, point, m_iFrame, 255, 0.5f, 0.5f);

			CRect rectHit( point.x - 8, point.y - 8, point.x + 8, point.y + 8);
			CPoint ptMouse = GetMousePoint();
			if( rectHit.PtInRect( ptMouse ) )
			{
//				CString toolTip("Mark : ");
				CString toolTip		= prj.GetText( TID_GAME_NAV_MARK );
				toolTip += g_pPlayer->m_vOtherPoint[i].Name.c_str();
				ClientToScreen( &ptMouse );
				ClientToScreen( &rectHit );
				g_toolTip.PutToolTip( 10000 + g_pPlayer->m_vOtherPoint[i].objid, toolTip, rectHit, ptMouse, 0 );
			}
			nvi++;
			i++;
		}
		else
		{
			g_pPlayer->m_vOtherPoint.erase(nvi);
		}
		
	}
}

void CWndNavigator::RenderPartyMember( C2DRender* p2DRender, TEXTUREVERTEX** pVertices, CRect rect, D3DXVECTOR3 vPos, u_long uIdPlayer, LPCTSTR lpStr )
{
	float fDistx = g_pPlayer->GetPos().x - vPos.x;
	float fDistz = g_pPlayer->GetPos().z - vPos.z;
	float fRadian = atan2( fDistx, fDistz );
	float fDegree = D3DXToDegree( fRadian );

	CPoint PointBuf;
	
	if( fDegree < 0 )
	{
		if( fDegree > -45 )
			PointBuf = CPoint( (int)( rect.Width() / 2 + -fDegree ), rect.Height() - 10 );
		else if( fDegree > -135 )
			PointBuf = CPoint( rect.Width() - 10, (int)( rect.Height() / 2 + 90 + fDegree ) );
		else
	        PointBuf = CPoint( (int)( rect.Width() / 2 + (180 + fDegree) ), 5 );
	}
	else
	{
		if( fDegree < 45 )
			PointBuf = CPoint( (int)( rect.Width() / 2 + -fDegree ), rect.Height() - 10 );
		else if( fDegree < 135 )
			PointBuf = CPoint( 8, (int)( rect.Height() / 2 + 90 -fDegree ) );
		else
			PointBuf = CPoint( (int)( rect.Width() / 2 + -(180 - fDegree) ), 5 );
	}

	m_texNavObjs.MakeVertex( p2DRender, PointBuf, 3, pVertices );

	CRect rectHittest( PointBuf.x, PointBuf.y, PointBuf.x + 5, PointBuf.y + 5);
	CPoint ptMouse = GetMousePoint();
	if( rectHittest.PtInRect( ptMouse ) )
	{
		ClientToScreen( &ptMouse );
		ClientToScreen( &rectHittest );
		g_toolTip.PutToolTip( 10000 + uIdPlayer, lpStr, rectHittest, ptMouse, 0 );
	}
}
HRESULT CWndNavigator::DeleteDeviceObjects()
{
	//CWndNeuz::DeleteDeviceObjects();
	m_texArrow.DeleteDeviceObjects();
	m_texNavObjs.DeleteDeviceObjects();
	m_billArrow.DeleteDeviceObjects();
	m_texNavPos.DeleteDeviceObjects();
	m_GuildCombatTextureMask.DeleteDeviceObjects();
#ifdef __YDEBUG
	m_texDunFog.DeleteDeviceObjects();
	m_ALPHACIRCLE.DeleteDeviceObjects();
	//m_NEWNAV.DeleteDeviceObjects();
#endif //__YDEBUG
	
	return CWndNeuz::DeleteDeviceObjects();
}
HRESULT CWndNavigator::InvalidateDeviceObjects()
{
	SAFE_RELEASE( m_pVBMEUTEST );

	m_texNavPos.InvalidateDeviceObjects();
	m_GuildCombatTextureMask.Invalidate();
#ifdef __YDEBUG
	m_texArrow.Invalidate();
	m_texDunFog.Invalidate();
	m_ALPHACIRCLE.Invalidate();
	//m_NEWNAV.Invalidate();
	m_texNavObjs.InvalidateDeviceObjects();
#endif //__YDEBUG
	
	m_billArrow.InvalidateDeviceObjects();
	return CWndNeuz::InvalidateDeviceObjects();
}
HRESULT CWndNavigator::RestoreDeviceObjects()
{
	m_texNavPos.RestoreDeviceObjects( m_pApp->m_pd3dDevice );
	m_GuildCombatTextureMask.SetInvalidate(m_pApp->m_pd3dDevice);
#ifdef __YDEBUG
	m_texArrow.SetInvalidate(m_pApp->m_pd3dDevice);
	m_texDunFog.SetInvalidate(m_pApp->m_pd3dDevice);
	m_ALPHACIRCLE.SetInvalidate(m_pApp->m_pd3dDevice);
	//m_NEWNAV.SetInvalidate(m_pApp->m_pd3dDevice);
	m_texNavObjs.RestoreDeviceObjects(m_pApp->m_pd3dDevice);
#endif //__YDEBUG

	ResizeMiniMap();
	m_billArrow.RestoreDeviceObjects();
	return CWndNeuz::RestoreDeviceObjects();
}
void CWndNavigator::OnDraw(C2DRender* p2DRender)
{
#if __VER >= 13 // __RAINBOW_RACE
	// Rainbow Race Time 출력
	DWORD dwRainbowRaceTime = CRainbowRace::GetInstance()->m_dwRemainTime;
	if(dwRainbowRaceTime > 0)
	{
		char szMsg[256] = { 0 };
		CTimeSpan ct( (dwRainbowRaceTime -  GetTickCount()) / 1000 );
		sprintf( szMsg, "%.2d:%.2d:%.2d", ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );
		CRect rectWindow = GetClientRect();
		p2DRender->TextOut(rectWindow.right - 50, rectWindow.bottom - 16, szMsg, 0xffffff00);
	}
#endif //__RAINBOW_RACE

/*      CRect rectN = GetClientRect();

    m_ALPHACIRCLE.m_size = CSize( rectN.Width(), rectN.Height());
	p2DRender->RenderTexture( CPoint( 0, 0 ), &m_ALPHACIRCLE, 250 );*/
}
void CWndNavigator::SerializeRegInfo( CAr& ar, DWORD& dwVersion )
{
	CWndNeuz::SerializeRegInfo( ar, dwVersion );
	if( ar.IsLoading() )
	{
		if( dwVersion == 0 )
		{
			ar >> m_bObjFilterPlayer >> m_bObjFilterParty >> m_bObjFilterNPC >> m_bObjFilterMonster;
		}
		else
		{
			ar >> m_bObjFilterPlayer >> m_bObjFilterParty >> m_bObjFilterNPC >> m_bObjFilterMonster;
			ar >> m_size.cx >> m_size.cy;
		}
		m_wndMenuPlace.CheckMenuItem( 0, m_bObjFilterPlayer );
		m_wndMenuPlace.CheckMenuItem( 1, m_bObjFilterParty  );
		m_wndMenuPlace.CheckMenuItem( 2, m_bObjFilterNPC    );
		m_wndMenuPlace.CheckMenuItem( 3, m_bObjFilterMonster );
	}
	else
	{
		dwVersion = 1;
		ar << m_bObjFilterPlayer << m_bObjFilterParty << m_bObjFilterNPC << m_bObjFilterMonster;
		ar << m_size.cx << m_size.cy;
	}
}
void CWndNavigator::OnInitialUpdate()
{
	CWndBase::OnInitialUpdate();

	SetTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "map.tga" ), TRUE );
	MakeVertexBuffer();

	DelWndStyle( WBS_THICKFRAME );

    CRect rectN = GetClientRect();
	m_pTexture->m_size = CSize( rectN.Width(), rectN.Height());

	CRect rectClient = GetClientRect();
	rectClient.right = 13;
	m_wndPlace.Create  ( "", 0, CRect( rectClient.left,   1, rectClient.left + 18,   1 + 18 ), this, 100000 );//p
//	m_wndMover.Create  ( "U", 0, CRect( rectClient.left,  17, rectClient.left + 16,  17 + 16 ), this, 100001 );
//	m_wndMover.Create  ( "M", 0, CRect( rectClient.left,  36, rectClient.left + 16,  36 + 16 ), this, 102 );
	m_wndZoomIn.Create ( "", 0, CRect( rectClient.left,  54, rectClient.left + 18,  54 + 18 ), this, 100005 );//+
	m_wndZoomOut.Create( "", 0, CRect( rectClient.left,  70, rectClient.left + 18,  70 + 18 ), this, 100006 );//-

	//////////////ADEILSON
	m_wndClose.Create( "", 0, CRect( rectClient.left,   1, rectClient.left + 18,   1 + 18 ), this, WTBID_CLOSE );//x
	m_wndHelp.Create( "", 0, CRect( rectClient.left,   17, rectClient.left + 18,   17 + 18 ), this, WTBID_HELP );//?

	m_wndClose.SetTexture( D3DDEVICE, MakePath( DIR_THEME, "ButtMap05.tga" ), TRUE );//x
	m_wndClose.FitTextureSize();
	m_wndHelp.SetTexture( D3DDEVICE, MakePath( DIR_THEME, "ButtMap06.tga" ), TRUE );//?
	m_wndHelp.FitTextureSize();
	//////////////end ADEILSON

	m_wndPlace.SetTexture( D3DDEVICE, MakePath( DIR_THEME, "ButtMap04.tga" ), TRUE );
	m_wndPlace.FitTextureSize();
	m_wndZoomIn.SetTexture( D3DDEVICE, MakePath( DIR_THEME, "ButtMap01.tga" ), TRUE );
	m_wndZoomIn.FitTextureSize();
	m_wndZoomOut.SetTexture( D3DDEVICE, MakePath( DIR_THEME, "ButtMap02.tga" ), TRUE );
	m_wndZoomOut.FitTextureSize();

    m_texDunFog.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "map3_2.tga" ), 0 , 1 );
	m_ALPHACIRCLE.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "alphacircle.tga" ), 0 , 1 );
	//m_ALPHACIRCLE.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "TEST.tga" ), 0 , 1 );
	m_NEWNAV.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "map.tga" ), 0 , 1 );
	m_NEWNAV.m_size = CSize( rectN.Width(), rectN.Height());

	m_TEST.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "TEST.tga" ), 0 , 1 );
	//m_texDunFog.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "NavDunFog.tga" ), 0 , 1 );

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	m_pDestinationPositionTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "ButtDestination.bmp"), 0xffff00ff );
#endif // __IMPROVE_QUEST_INTERFACE

	m_wndMenuPlace.CreateMenu( this );	
	/*
	m_wndMenuPlace.AppendMenu( 0, 0 ,_T("플레이어" ) );
	m_wndMenuPlace.AppendMenu( 0, 1 ,_T("극단"     ) ); 
	m_wndMenuPlace.AppendMenu( 0, 2 ,_T("NPC"      ) );
	m_wndMenuPlace.AppendMenu( 0, 3 ,_T("몬스터"   ) ); 
	*/
	m_wndMenuPlace.AppendMenu( 0, 0 , prj.GetText(TID_GAME_PLAYER) );
	m_wndMenuPlace.AppendMenu( 0, 1 , prj.GetText(TID_GAME_PARTYTEXT) ); 
	m_wndMenuPlace.AppendMenu( 0, 2 , prj.GetText(TID_GAME_NPC) );
	m_wndMenuPlace.AppendMenu( 0, 3 , prj.GetText(TID_GAME_MONSTER) ); 
	

	m_wndMenuPlace.CheckMenuItem( 0, m_bObjFilterPlayer );
	m_wndMenuPlace.CheckMenuItem( 1, m_bObjFilterParty  );
	m_wndMenuPlace.CheckMenuItem( 2, m_bObjFilterNPC    );
	m_wndMenuPlace.CheckMenuItem( 3, m_bObjFilterMonster );

/*
	m_wndMenuPlace.AppendMenu( 0, 0 ,_T("무기상점" ) );
	m_wndMenuPlace.AppendMenu( 0, 1 ,_T("잡화점"   ) );
	m_wndMenuPlace.AppendMenu( 0, 2 ,_T("방어구점" ) ); 
	m_wndMenuPlace.AppendMenu( 0, 3 ,_T("술집"     ) );
	m_wndMenuPlace.AppendMenu( 0, 4 ,_T("도시 "    ) );
	m_wndMenuPlace.AppendMenu( 0, 5 ,_T("무기상점" ) );
	m_wndMenuPlace.AppendMenu( 0, 6 ,_T("잡화점"   ) );
	m_wndMenuPlace.AppendMenu( 0, 7 ,_T("방어구점" ) ); 
	m_wndMenuPlace.AppendMenu( 0, 8 ,_T("술집"     ) );
	m_wndMenuPlace.AppendMenu( 0, 9 ,_T("-- 장소추가 --" ));
*/
	m_wndMenuMover.CreateMenu( this );	
	/*
	CRect rect = GetClientRect();
	D3DXVECTOR3 vPos = ( g_pPlayer != NULL ? g_pPlayer->GetPos() : D3DXVECTOR3( 0, 0 , 0 ) );
	// 128 : m_texture.m_size.cx = 1 : x

	FLOAT fx = (FLOAT)m_texture.m_size.cx / 256.0f * 2;
	FLOAT fy = (FLOAT)m_texture.m_size.cy / 256.0f * 2;

	vPos.x *= fx;
	vPos.z *= fy;
	CObj* pObj;
	D3DXVECTOR3 vCenter	= ( g_pPlayer != NULL ? g_pPlayer->GetPos() : D3DXVECTOR3( 0, 0, 0 ) );
	*/

	//m_size = CSize( MINIMAP_SIZE, MINIMAP_SIZE );
	m_size = CSize( 256, 256) ;//MINIMAP_SIZE, MINIMAP_SIZE );
	m_nSizeCnt = 0;
	//m_wndMinimize.Create("_",0,CRect(00,0,14,14),this,10000);

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), rectRoot.top );
	Move( point );

	ResizeMiniMap();
}
BOOL CWndNavigator::Initialize(CWndBase* pWndParent,DWORD dwWndId)
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 0, 0, 200, 200);//115, 110 ); // 1024 768

	m_texNavObjs.LoadScript( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME,"Navigator.inc") );
	m_texArrow.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME,"ImgNavArrow.bmp"), 0xffff00ff );
	m_texNavPos.LoadScript( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "NavPosition.inc") );
	ZeroMemory( &m_billboard, sizeof( m_billboard ) );
	m_billboard.rect.SetRect( 0, 0, m_texArrow.m_size.cx, m_texArrow.m_size.cy );
	m_billboard.ptCenter = CPoint( m_texArrow.m_size.cx / 2, m_texArrow.m_size.cy / 2 );
	m_billArrow.InitDeviceObjects( g_Neuz.m_pd3dDevice, &m_billboard, &m_texArrow );
	m_billArrow.RestoreDeviceObjects();

	m_GuildCombatTextureMask.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_WORLD_GUILDCOMBAT, "WdGuildWar_Mask.dds" ), 0  );

	SetTitle( GETTEXT( TID_APP_NAVIGATOR ) );
	//return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, WBS_NODRAWFRAME|WBS_MOVE , CPoint( 792, 130 ), pWndParent );

	CRect rectM = m_pWndRoot->GetWindowRect();
	return CWndBase::Create( WBS_MOVE|WBS_SOUND|WBS_CAPTION|WBS_EXTENSION,rectM,pWndParent,dwWndId);
}
void CWndNavigator::ResizeMiniMap()
{
	CWorld* pWorld	= g_WorldMng();
	if( pWorld == NULL )
		return;
	for( int z = 0; z < pWorld->m_nLandHeight; z++ )
	{
		for( int x = 0; x < pWorld->m_nLandWidth; x++ )
		{
			CLandscape* pLand = pWorld->GetLandscape( x, pWorld->m_nLandHeight - z - 1);
			if( pLand )
			{
				pLand->m_texMiniMap.m_size = m_size;
			}
		}
	}
}
BOOL CWndNavigator::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	CRect rect = GetWindowRect( TRUE );
	//int nImageBlock = pLand->m_texMiniMap.m_size.cx;

	CWndBase* pWndBase = (CWndBase*) pLResult;

	if( pWndBase->m_pParentWnd == &m_wndMenuPlace )
	{
	//	g_WndMng.m_nObjectFilter = nID;
		//m_wndMenuPlace.CheckMenuRadioItem( 0, 2, nID, TRUE );
		int nState = !m_wndMenuPlace.GetMenuState( nID, 0 );
		m_wndMenuPlace.CheckMenuItem( nID, nState );
		SetFocus();
		switch( nID )
		{
		case OBJFILTER_PLAYER:
			m_bObjFilterPlayer = nState;
			break;
		case OBJFILTER_PARTY:
			m_bObjFilterParty = nState;
			break;
		case OBJFILTER_NPC:
			m_bObjFilterNPC = nState;
			break;
		case OBJFILTER_MONSTER:
			m_bObjFilterMonster = nState;
			break;
		}
	}
	else
	if( message == WNM_CLICKED )
	{
		switch(nID)
		{
			case 100000: // 장소 찾기
#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
				{
					CRect rectRootLayout = m_pWndRoot->GetLayoutRect();
					int nMenuPlaceLeft = rect.left - m_wndMenuPlace.GetWindowRect().Width();
					if( nMenuPlaceLeft < rectRootLayout.left )
						m_wndMenuPlace.Move( CPoint( rect.right, rect.top ) );
					else
						m_wndMenuPlace.Move( CPoint( nMenuPlaceLeft, rect.top ) );
					m_wndMenuPlace.SetVisible( !m_wndMenuPlace.IsVisible() );
					m_wndMenuPlace.SetFocus();
				}
#else // __IMPROVE_SYSTEM_VER15
				m_wndMenuPlace.Move( CPoint( rect.left - m_wndMenuPlace.GetWindowRect().Width(), rect.top ) );
				m_wndMenuPlace.SetVisible( !m_wndMenuPlace.IsVisible() );
				m_wndMenuPlace.SetFocus();
#endif // __IMPROVE_SYSTEM_VER15
				break;
			case 100001: // 장소 찾기
				{
					m_wndMenuMover.DeleteAllMenu();
					CWorld* pWorld	= g_WorldMng();
					CLandscape* pLand;
					CObj* pObj;
					CWndButton* pWndButton;
					int i = 0;

					FOR_LAND( pWorld, pLand, pWorld->m_nVisibilityLand, FALSE )
					{
						FOR_OBJ( pLand, pObj, OT_MOVER )
						{
							pWndButton = m_wndMenuMover.AppendMenu( i++, ((CMover*)pObj)->GetId() , ((CMover*)pObj)->GetName( TRUE ) );
							pWndButton->m_shortcut.m_dwShortcut = SHORTCUT_OBJECT;
						}
						END_OBJ
					}
					END_LAND
					m_wndMenuMover.Move( CPoint( rect.left - m_wndMenuMover.GetWindowRect().Width(), rect.top ) );
					m_wndMenuMover.SetVisible( !m_wndMenuMover.IsVisible() );
					m_wndMenuMover.SetFocus();
				}
				break;
			case 100005: // zoom in
				m_nSizeCnt = 1;
				m_size.cx += 32;
				m_size.cy += 32;
				if(m_size.cx > 352)
					m_size.cx = 352;
				if(m_size.cy > 352 )
					m_size.cy = 352;
				/*
				if(m_size.cx > 1024)
					m_size.cx = 1024;
				if(m_size.cy > 1024 )
					m_size.cy = 1024;
					*/
				ResizeMiniMap();
				break;
			case 100006: // zoom out 
				m_nSizeCnt = -1;
				m_size.cx -= 32;
				m_size.cy -= 32;
				if( m_size.cx < 128 )
					m_size.cx = 128;
				if( m_size.cy < 128 )
					m_size.cy = 128;
				ResizeMiniMap();
				break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
void CWndNavigator::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	if( g_pPlayer == NULL ) 
		return;
	if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
	{
		CRect rect = GetClientRect();
		CPoint pt = point;
		
		pt.x -= ( rect.Width() / 2 );
		pt.y -= ( rect.Height() / 2 );
		
		FLOAT fx = (FLOAT)m_size.cx / ( MAP_SIZE * MPU );//(FLOAT)m_size.cx / (FLOAT)MINIMAP_SIZE;// * 2;
		FLOAT fy = (FLOAT)m_size.cy / ( MAP_SIZE * MPU );//(FLOAT)m_size.cy / (FLOAT)MINIMAP_SIZE;// * 2;
		
		D3DXVECTOR3 vPos = g_pPlayer->GetPos();
		vPos.x += ( pt.x / fx );
		vPos.z -= ( pt.y / fy );

		CString string;
		string.Format( "/teleport %d %f %f", g_WorldMng()->m_dwWorldID, vPos.x, vPos.z );
		g_DPlay.SendChat( string );
	}
}

BOOL CWndNavigator::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{
	if( pWndBase == &m_wndMenuMover )
	{
		if( nID >= 0 && nID < 100000 )
		{
			CMover* pMover = prj.GetMover( nID );
			g_WorldMng()->SetObjFocus( pMover );
			SetFocus();
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
			g_WndMng.m_pWndWorld->m_vDestinationArrow = pMover->m_vPos;
#endif // __IMPROVE_QUEST_INTERFACE
		}
	}
	/*
	CRect rect = GetWindowRect( TRUE );
	
	switch(nID)
	{
		case 100: // wnd1 
			m_wndMenu.Move( CPoint( rect.right, rect.top ) );
			m_wndMenu.SetVisible( !m_wndMenu.IsVisible() );
			m_wndMenu.SetFocus();
			break;
	}
	*/
	return CWndNeuz::OnCommand(nID,dwMessage,pWndBase);
}
void CWndNavigator::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	AdjustMinRect( &rectWnd, 16 * 6, 16 * 7 );
	AdjustMaxRect( &rectWnd, 16 * 12, 16 * 13 );
	CWndNeuz::SetWndRect( rectWnd, bOnSize );
}
void CWndNavigator::OnSize(UINT nType, int cx, int cy)
{
	m_wndPlace.Move( CPoint( 20, 20 ) );//<
	m_wndZoomIn.Move( CPoint(5, cy - 61 ) );//+
	m_wndZoomOut.Move( CPoint(15, cy - 45 ) );//-

	m_wndClose.Move( CPoint(145, 20 ) );//x
	m_wndHelp.Move( CPoint(155, 37) );//?

	CWndNeuz::OnSize(nType,cx,cy);
}
void CWndNavigator::OnRButtonDown(UINT nFlags, CPoint point)
{				
	CWorld* pWorld = g_WorldMng();
	pWorld->SetObjFocus(  NULL );
	g_pPlayer->ClearDest();

#if __VER >= 15 // __FIND_OBJ_INSIGHT
	m_wndMenuMover.DeleteAllMenu();
	CLandscape* pLand;
	CObj* pObj;
	CWndButton* pWndButton;
	int i = 0;

	CMover* pMover = NULL;
	int nTarget = 0;
	FOR_LAND( pWorld, pLand, pWorld->m_nVisibilityLand, FALSE )
	{
		FOR_OBJ( pLand, pObj, OT_MOVER )
		{
			pMover = ( CMover* )pObj;
			if( !pMover->IsPlayer( ) && pMover->GetCharacter( ) )		//NPC인경우만 
			{
				pWndButton = m_wndMenuMover.AppendMenu( i++, ((CMover*)pObj)->GetId() , ((CMover*)pObj)->GetName( TRUE ) );
				pWndButton->m_shortcut.m_dwShortcut = SHORTCUT_OBJECT;
				++nTarget;
			}
		}
		END_OBJ
	}
	END_LAND

	if( nTarget > 0 )
	{
		CRect rect = GetWindowRect( TRUE );
#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
		CRect rectRootLayout = m_pWndRoot->GetLayoutRect();
		int nMenuMoverLeft = rect.left - m_wndMenuMover.GetWindowRect().Width();
		if( nMenuMoverLeft < rectRootLayout.left )
			m_wndMenuMover.Move( CPoint( rect.right, rect.top ) );
		else
			m_wndMenuMover.Move( CPoint( nMenuMoverLeft, rect.top ) );
#else // __IMPROVE_SYSTEM_VER15
		m_wndMenuMover.Move( CPoint( rect.left - m_wndMenuMover.GetWindowRect().Width(), rect.top ) );
#endif // __IMPROVE_SYSTEM_VER15
		m_wndMenuMover.SetVisible( !m_wndMenuMover.IsVisible() );
		m_wndMenuMover.SetFocus();
	}
#endif
}

void CWndNavigator::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndNavigator::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rect = GetClientRect();
	D3DXVECTOR3 vPos, vCenter = ( g_pPlayer != NULL ? g_pPlayer->GetPos() : D3DXVECTOR3( 0, 0 , 0 ) );
	// 128 : m_texture.m_size.cx = 1 : x

	FLOAT fx = (FLOAT)m_size.cx / ( MAP_SIZE * MPU );
	FLOAT fy = (FLOAT)m_size.cy / ( MAP_SIZE * MPU );

	CWorld* pWorld	= g_WorldMng();

	g_pPlayer->m_nvPoint.Pos.x = vCenter.x + ( (float)( point.x - (rect.right / 2) ) / fx );
	g_pPlayer->m_nvPoint.Pos.z = vCenter.z - ( (float)( point.y - (rect.bottom / 2) ) / fy );
	//	이 시간은 네비게이터에 남아 있는 시간으로 디파인이나 파일로 뺄수도 있을듯하지만 우선 하드 코딩이닷
	g_pPlayer->m_nvPoint.Time = 200;	
	if( pWorld )
	{
		CObj* pObj	= pWorld->GetObjFocus();
		if( IsValidObj( pObj ) && pObj->GetType() == OT_MOVER && ( (CMover*)pObj )->IsPlayer() )
		{
				g_DPlay.SendSetNaviPoint( g_pPlayer->m_nvPoint.Pos, ( (CCtrl*)pObj )->GetId() );
		}
		else
		{
			if( g_Party.IsMember( g_pPlayer->m_idPlayer ) )
				g_DPlay.SendSetNaviPoint( g_pPlayer->m_nvPoint.Pos, NULL_ID );
		}
	}

	vCenter.x *= fx;
	vCenter.z *= fy;
	int xCenter = (int)( vCenter.x );
	int yCenter = (int)( - vCenter.z );
	
	CObj* pObj;
	CLandscape* pLand;
//	CWorld* pWorld	= g_WorldMng();
	CMover *pMover;
	FOR_LAND( pWorld, pLand, pWorld->m_nVisibilityLand, FALSE )
	{
		FOR_OBJ( pLand, pObj, OT_MOVER )
		{
			if( pObj->GetType() == OT_MOVER )
			{
				pMover = (CMover *)pObj;
				if( pMover->IsMode( TRANSPARENT_MODE ) == FALSE )
				{
					vPos = pObj->GetPos();
					vPos.x *= fx;
					vPos.z *= fy;
					int x = (int)( ( rect.Width() / 2 ) + vPos.x );
					int y = (int)( ( rect.Height() / 2 ) - vPos.z );
					x -= xCenter + 2;
					y -= yCenter + 2;
					CRect rectHittest( x, y, x + 5, y + 5);
					if( rectHittest.PtInRect( point ) )
					{
						if( CMover::GetActiveMover() != pObj )
							pWorld->SetObjFocus( pObj );
						return;
					}
				}
			}
		}
		END_OBJ
	}
	END_LAND
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 게이지 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndStatus::CWndStatus()
{
	m_nDisplay = 2;
	m_nHPWidth = -1;
	m_nMPWidth = -1;
	m_nFPWidth = -1;
	m_nEXPWidth = -1;
	m_nPXPWidth = -1;

	m_pVBHPGauge = NULL;
	m_pVBMPGauge = NULL;
	m_pVBFPGauge = NULL;
	m_pVBEXPGauge = NULL;

	m_bVBHPGauge = TRUE;
	m_bVBMPGauge = TRUE;
	m_bVBFPGauge = TRUE;
	
#if __VER >= 15 // __GUILD_HOUSE
	m_bVBAEXPGauge = TRUE;
	m_pVBAEXPGauge = NULL;
#endif

	m_bHPVisible = TRUE;
	m_bExpVisible = TRUE;
}

CWndStatus::~CWndStatus()
{
	DeleteDeviceObjects();
}
void CWndStatus::MakeGaugeVertex()
{
	LPWNDCTRL lpHP   = GetWndCtrl( WIDC_CUSTOM1 );
	LPWNDCTRL lpMP   = GetWndCtrl( WIDC_CUSTOM2 );
	LPWNDCTRL lpFP   = GetWndCtrl( WIDC_CUSTOM3 );
	LPWNDCTRL lpExp  = GetWndCtrl( WIDC_CUSTOM4 );
	LPWNDCTRL lpPxp  = GetWndCtrl( WIDC_CUSTOM5 );
	LPWNDCTRL lpFace = GetWndCtrl( WIDC_CUSTOM6 );
	CMover* pMover = CMover::GetActiveMover();

	if( pMover )
	{
		CRect rect = GetClientRect();
		CRect rectTemp;
		
		//	pMover->m_nHitPoint = 50;
		int nWidthClient = lpHP->rect.Width();
		//100 : rect = hp : x
		int nWidth;

		// HP
		nWidth = nWidthClient * pMover->GetHitPoint() / pMover->GetMaxHitPoint();
		if( m_nHPWidth != nWidth ) 
		{
			m_nHPWidth = nWidth;
			rect = lpHP->rect;//.SetRect( 105, 6, 105 + nWidthClient, 6 + 12 );
			rectTemp = rect; 
			rectTemp.right = rectTemp.left + nWidth;
			ClientToScreen( rectTemp );
#ifdef __STATUS_V19
			m_bVBHPGauge = m_pTheme->MakeGaugeVertex( m_pApp->m_pd3dDevice, &rectTemp, 0xffffffff, m_pVBHPGauge, &m_HPGaugeTexture );
#else
			m_bVBHPGauge = m_pTheme->MakeGaugeVertex( m_pApp->m_pd3dDevice, &rectTemp, 0x64ff0000, m_pVBHPGauge, &m_texGauFillNormal );
#endif//__STATUS_V19
		}
		//else m_bVBHPGauge = TRUE;
		// MP
		nWidth	= nWidthClient * pMover->GetManaPoint() / pMover->GetMaxManaPoint();
		if( m_nMPWidth != nWidth ) 
		{
			m_nMPWidth = nWidth;
			rect = lpMP->rect;//rect.SetRect( 105, 20, 105 + nWidthClient, 20 + 12 );
			rectTemp = rect; 
			rectTemp.top = rect.top + 1;
			rectTemp.right = rectTemp.left + nWidth;
			ClientToScreen( rectTemp );
#ifdef __STATUS_V19
			m_bVBMPGauge = m_pTheme->MakeGaugeVertex( m_pApp->m_pd3dDevice, &rectTemp, 0xffffffff, m_pVBMPGauge, &m_MPGaugeTexture );
#else
			m_bVBMPGauge = m_pTheme->MakeGaugeVertex( m_pApp->m_pd3dDevice, &rectTemp, 0x640000ff, m_pVBMPGauge, &m_texGauFillNormal );
#endif//__STATUS_V19
		}
		//else m_bVBMPGauge = TRUE;
		// FP
		nWidth	= nWidthClient * pMover->GetFatiguePoint() / pMover->GetMaxFatiguePoint();
		if( m_nFPWidth != nWidth ) 
		{
			m_nFPWidth = nWidth;
			rect = lpFP->rect;//rect.SetRect( 105, 32, 105 + nWidthClient, 32 + 12 );
			rectTemp = rect; 
			rectTemp.right = rectTemp.left + nWidth;
			ClientToScreen( rectTemp );
#ifdef __STATUS_V19
			m_bVBFPGauge = m_pTheme->MakeGaugeVertex( m_pApp->m_pd3dDevice, &rectTemp, 0xffffffff, m_pVBFPGauge, &m_FPGaugeTexture );
#else
			m_bVBFPGauge = m_pTheme->MakeGaugeVertex( m_pApp->m_pd3dDevice, &rectTemp, 0x6400ff00, m_pVBFPGauge, &m_texGauFillNormal );
#endif//__STATUS_V19
		}
	//lse m_bVBFPGauge = TRUE;
		//nWidth	= ( ( hyper ) nWidthClient * pMover->GetExp1() ) / pMover->GetMaxExp1();
		//nWidth	= ( ( hyper ) nWidthClient * pMover->GetExp1() ) / pMover->GetMaxExp1();
		nWidth	= (int)( ( (float) nWidthClient / pMover->GetMaxExp1() ) * pMover->GetExp1() );

		if( m_nEXPWidth != nWidth ) 
		{
			m_nEXPWidth = nWidth;
			rect = lpExp->rect;//rect.SetRect( 105, 48, 105 + nWidthClient, 48 + 8 );
			rectTemp = rect; 
			rectTemp.right = rectTemp.left + nWidth;
			ClientToScreen( rectTemp );
#ifdef __STATUS_V19
			m_bVBEXPGauge = m_pTheme->MakeGaugeVertex( m_pApp->m_pd3dDevice, &rectTemp, 0xffffffff, m_pVBEXPGauge, &m_EXPGaugeTexture );
#else
			m_bVBEXPGauge = m_pTheme->MakeGaugeVertex( m_pApp->m_pd3dDevice, &rectTemp, 0x847070ff, m_pVBEXPGauge, &m_texGauFillSmall );
#endif//__STATUS_V19
		}

#if __VER >= 15 // __GUILD_HOUSE
		if( m_nPXPWidth != GuildHouse->m_nExtraExp )
		{
			m_nPXPWidth = GuildHouse->m_nExtraExp;
			rect = lpPxp->rect;
			rectTemp = rect;
			
			if( 0 < m_nPXPWidth && m_nPXPWidth < 9 )
				m_nPXPWidth = 9;
			rectTemp.right = rectTemp.left + m_nPXPWidth;
			ClientToScreen( rectTemp );
#ifdef __STATUS_V19
			m_bVBAEXPGauge = m_pTheme->MakeGaugeVertex( m_pApp->m_pd3dDevice, &rectTemp, 0x84e6ce19, m_pVBAEXPGauge, &m_AEXPGaugeTexture );
#else
			m_bVBAEXPGauge = m_pTheme->MakeGaugeVertex( m_pApp->m_pd3dDevice, &rectTemp, 0x84e6ce19, m_pVBAEXPGauge, &m_texGauFillSmall );
#endif//__STATUS_V19
		}
#endif
	}
}
void CWndStatus::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();
	if( m_pTexture && g_pPlayer )
	{
		RenderWnd();
		// 여기는 타이틀 바의 텍스트를 출력하는 곳 
		if( IsWndStyle( WBS_CAPTION ) )	
		{
			int y = 4;
#ifdef __WINDOWS_V19
			CPoint newpoint;
			CSize sizeTitle = p2DRender->m_pFont->GetTextExtent( m_strTitle );

			CD3DFont* pOldFont = p2DRender->GetFont();
			p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndNewTitle );
            p2DRender->TextOut( 10 , 8, m_strTitle, 0xffffffff );
			p2DRender->SetFont( pOldFont );
#else
			CD3DFont* pOldFont = p2DRender->GetFont();
			p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle );
			p2DRender->TextOut( 10, y, m_strTitle, m_dwColor );
#endif//__WINDOWS_V19

//			CHAR szNum[32];
//			itoa( g_pPlayer->GetLevel(), szNum, 10 );
			
//			CHAR szLevel[32];
//			strcpy( szLevel, " Lv. " );
//			strcat( szLevel, szNum );
//			p2DRender->TextOut( 57, y, szLevel, 0xffffffd0 );

			char szNameLevel[128] = {0,};
#if __VER >= 10 // __CSC_VER9_1
#if __VER < 15 // __HERO129_VER15				// 15차 히어로 레벨확장
			if(g_pPlayer->GetLegendChar() == LEGEND_CLASS_HERO)
			{
				sprintf( szNameLevel, "%s", g_pPlayer->GetName() );
				p2DRender->TextOut( 130, 4, prj.GetText(TID_GAME_STATUS_HERO), 0xffc0c0ff, 0x00000000 );
			}
			else
	#endif	// 15차 히어로 레벨확장
				sprintf( szNameLevel, prj.GetText( TID_GAME_WND_STATUS_PLAYER_INFORMATION ), g_pPlayer->GetName(), g_pPlayer->GetLevel() );
#else //__CSC_VER9_1
			sprintf( szNameLevel, "%s Lv %d", g_pPlayer->GetName(), g_pPlayer->GetLevel() );
#endif //__CSC_VER9_1
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

void CWndStatus::OnDraw(C2DRender* p2DRender)
{
	CMover* pMover = CMover::GetActiveMover();
	if( pMover == NULL )
		return;

	CRect rect = GetClientRect();
	int nWidthClient = GetClientRect().Width() - 110;// - 30;

	CRect rectTemp;
	//SetTitle( "" );
	/////////////////////////////////////

	LPWNDCTRL lpHP   = GetWndCtrl( WIDC_CUSTOM1 );
	LPWNDCTRL lpMP   = GetWndCtrl( WIDC_CUSTOM2 );
	LPWNDCTRL lpFP   = GetWndCtrl( WIDC_CUSTOM3 );
	LPWNDCTRL lpExp  = GetWndCtrl( WIDC_CUSTOM4 );
	LPWNDCTRL lpFace = GetWndCtrl( WIDC_CUSTOM6 );

#if __VER >= 15 // __GUILD_HOUSE
	LPWNDCTRL lpAExp = GetWndCtrl( WIDC_CUSTOM5 );
#endif
	
	if( pMover )
	{
		MakeGaugeVertex();

		if( m_bVBHPGauge )
		{
//#ifdef __VCRITICAL
#if __VER < 9 // __S_9_ADD
			int nHitPercent = g_pPlayer->GetHitPointPercent( 100 );
			if( nHitPercent < CRITICAL_BERSERK_HP )
				m_bHPVisible = !m_bHPVisible;
			else
#endif // __S_9_ADD
				m_bHPVisible = TRUE;
//#else
//			m_bHPVisible = TRUE;
//#endif

#ifdef __STATUS_V19
		if( m_bHPVisible )
			m_pTheme->RenderGauge( p2DRender->m_pd3dDevice, m_pVBHPGauge, &m_HPGaugeTexture );
		}
		if( m_bVBMPGauge )
			m_pTheme->RenderGauge( p2DRender->m_pd3dDevice, m_pVBMPGauge, &m_MPGaugeTexture );
		if( m_bVBFPGauge )
			m_pTheme->RenderGauge( p2DRender->m_pd3dDevice, m_pVBFPGauge, &m_FPGaugeTexture );
		if( g_pPlayer->IsAfterDeath() )
			m_bExpVisible = !m_bExpVisible;
		else
			m_bExpVisible = TRUE;
		if( m_bVBEXPGauge )
		{
			m_pTheme->RenderGauge( p2DRender->m_pd3dDevice, m_pVBEXPGauge, &m_EXPGaugeTexture );
		}
		if( m_bVBAEXPGauge )
			m_pTheme->RenderGauge( p2DRender->m_pd3dDevice, m_pVBAEXPGauge, &m_AEXPGaugeTexture );
#else//__STATUS_V19
		if( m_bHPVisible )
			m_pTheme->RenderGauge( p2DRender->m_pd3dDevice, m_pVBHPGauge, &m_texGauFillNormal );
		}
		if( m_bVBMPGauge )
			m_pTheme->RenderGauge( p2DRender->m_pd3dDevice, m_pVBMPGauge, &m_texGauFillNormal );
		if( m_bVBFPGauge )
			m_pTheme->RenderGauge( p2DRender->m_pd3dDevice, m_pVBFPGauge, &m_texGauFillNormal );
		if( g_pPlayer->IsAfterDeath() )
			m_bExpVisible = !m_bExpVisible;
		else
			m_bExpVisible = TRUE;
#if __VER >= 8     // 8차 스킬경험치다운변경
		if( m_bVBEXPGauge )
		{
			m_pTheme->RenderGauge( p2DRender->m_pd3dDevice, m_pVBEXPGauge, &m_texGauFillSmall );
		}
		if( m_bExpVisible )
		{
/*
#if __VER < 8 // #ifndef __SKILL_WITHOUT_EXP
			if( m_bVBPXPGauge )
			{
				m_pTheme->RenderGauge( p2DRender->m_pd3dDevice, m_pVBPXPGauge, &m_texGauFillSmall );

			}
#endif // __VER < 8
*/
		}
#else	// __VER >= 8  
		if( m_bExpVisible )
		{
			if( m_bVBEXPGauge )
			{
				m_pTheme->RenderGauge( p2DRender->m_pd3dDevice, m_pVBEXPGauge, &m_texGauFillSmall );
			}
/*
			if( m_bVBPXPGauge )
			{
				m_pTheme->RenderGauge( p2DRender->m_pd3dDevice, m_pVBPXPGauge, &m_texGauFillSmall );

			}
*/
		}
#endif	// __VER >= 8  

#if __VER >= 15 // __GUILD_HOUSE
		if( m_bVBAEXPGauge )
			m_pTheme->RenderGauge( p2DRender->m_pd3dDevice, m_pVBAEXPGauge, &m_texGauFillSmall );
		
#endif

#endif//__STATUS_V19
/*
		//p2DRender->TextOut( 10, 65, 1, 1, "Lv. : ", 0xff101010 );
		CHAR szNum[32];
		itoa( g_pPlayer->GetLevel(), szNum, 10 );
		//p2DRender->TextOut( 35, 65, 1, 1, szNum, 0xff606060 );

		CHAR szLevel[32];
		strcpy( szLevel, "Status - Lv. " );
		strcat( szLevel, szNum );

		m_strTitle = szLevel;
*/
	}		
	if( m_nDisplay != 0 )
	{
		DWORD dwColor = D3DCOLOR_ARGB(255, 255, 255, 255 );
		//p2DRender->SetFont( m_pTheme->m_pFontWndTitle );

		char cbufHp[16] = {0,};
		char cbufMp[16] = {0,};
		char cbufFp[16] = {0,};
		char cbufExp[16] = {0,};
		char cbufPxp[16] = {0,};
		
		int nCharHP, nCharMP, nCharFP, nCharEXP; //, nCharPXP;
		CSize size = p2DRender->m_pFont->GetTextExtent("8");
		int nMaxHeight = p2DRender->m_pFont->GetMaxHeight();
		int nTopGap = 0;

		if( ::GetLanguage() == LANG_FRE || ::GetLanguage() == LANG_GER )
			nTopGap = 2;
		else
		{
			if(nMaxHeight > 14)
				nTopGap = nMaxHeight - 14;
		}

		if(m_nDisplay == 1)
		{
			wsprintf(cbufHp, "%d%%", pMover->GetHitPointPercent());
			wsprintf(cbufMp, "%d%%", pMover->GetManaPointPercent());
			wsprintf(cbufFp, "%d%%", pMover->GetFatiguePointPercent());

			p2DRender->TextOut( lpHP->rect.right - 90, lpHP->rect.top - nTopGap, cbufHp, dwColor, 0xff000000 );
			p2DRender->TextOut( lpMP->rect.right - 90, lpMP->rect.top - nTopGap, cbufMp, dwColor, 0xff000000 );
			p2DRender->TextOut( lpFP->rect.right - 90, lpFP->rect.top - nTopGap, cbufFp, dwColor, 0xff000000 );

			nCharHP = wsprintf(cbufHp, prj.GetText( TID_GAME_WND_STATUS_PERCENT_POINT ), pMover->GetHitPoint());
			nCharMP = wsprintf(cbufMp, prj.GetText( TID_GAME_WND_STATUS_PERCENT_POINT ), pMover->GetManaPoint());
			nCharFP = wsprintf(cbufFp, prj.GetText( TID_GAME_WND_STATUS_PERCENT_POINT ), pMover->GetFatiguePoint());

			int x = lpHP->rect.right-17; // -40
			p2DRender->TextOut( x - (int)(nCharHP*5.8f) , lpHP->rect.top - nTopGap, cbufHp, dwColor, 0xff000000 );
			p2DRender->TextOut( x - (int)(nCharMP*5.8f), lpMP->rect.top - nTopGap, cbufMp, dwColor, 0xff000000 );
			p2DRender->TextOut( x - (int)(nCharFP*5.8f), lpFP->rect.top - nTopGap, cbufFp, dwColor, 0xff000000 );
		}
		else
		{
			nCharHP = wsprintf(cbufHp, "%d", pMover->GetHitPoint());
			nCharMP = wsprintf(cbufMp, "%d", pMover->GetManaPoint());
			nCharFP = wsprintf(cbufFp, "%d", pMover->GetFatiguePoint());

			int x = lpHP->rect.right - 82;
			p2DRender->TextOut( x - (int)(((float)nCharHP / 2.0f) * size.cx), lpHP->rect.top - nTopGap, cbufHp, dwColor, 0xff000000 );
			p2DRender->TextOut( x - (int)(((float)nCharMP / 2.0f) * size.cx), lpMP->rect.top - nTopGap, cbufMp, dwColor, 0xff000000 );
			p2DRender->TextOut( x - (int)(((float)nCharFP / 2.0f) * size.cx), lpFP->rect.top - nTopGap, cbufFp, dwColor, 0xff000000 );
			
			nCharHP = wsprintf(cbufHp, "%d", pMover->GetMaxHitPoint());
			nCharMP = wsprintf(cbufMp, "%d", pMover->GetMaxManaPoint());
			nCharFP = wsprintf(cbufFp, "%d", pMover->GetMaxFatiguePoint());
			
			x = lpHP->rect.right - 30;
			p2DRender->TextOut( x - (int)(((float)nCharHP / 2.0f) * size.cx), lpHP->rect.top - nTopGap, cbufHp, dwColor, 0xff000000 );
			p2DRender->TextOut( x - (int)(((float)nCharMP / 2.0f) * size.cx), lpMP->rect.top - nTopGap, cbufMp, dwColor, 0xff000000 );
			p2DRender->TextOut( x - (int)(((float)nCharFP / 2.0f) * size.cx), lpFP->rect.top - nTopGap, cbufFp, dwColor, 0xff000000 );
				
			LPWNDCTRL lpHP = GetWndCtrl( WIDC_CUSTOM1 );
			int nWidthCustom = lpHP->rect.Width();
			int nGap = (int)( ((nWidthCustom / 2.0f) + (size.cx / 2.0f)) );

			p2DRender->TextOut( lpHP->rect.right - nGap, lpHP->rect.top - nTopGap, "/", dwColor, 0xff000000 );
			p2DRender->TextOut( lpMP->rect.right - nGap, lpMP->rect.top - nTopGap, "/", dwColor, 0xff000000 );
			p2DRender->TextOut( lpFP->rect.right - nGap, lpFP->rect.top - nTopGap, "/", dwColor, 0xff000000 );
		}
		EXPINTEGER	nExpResult = pMover->GetExp1() * (EXPINTEGER)10000 / pMover->GetMaxExp1();
		float fExp = (float)nExpResult / 100.0f;
		if( fExp >= 99.99f )
			nCharEXP = sprintf( cbufExp, "99.99%%" );		// sprintf함수 내부에서 반올림되어 100.00으로 표시되는 것을 막기 위해서 
		else
			nCharEXP = sprintf( cbufExp, "%.2f%%", fExp );

		int x = lpHP->rect.right-7; // -40
		p2DRender->TextOut( x - (int)(nCharEXP*5.8f), lpExp->rect.top - 0, cbufExp, dwColor, 0xff000000 );

#if __VER >= 15 // __GUILD_HOUSE
		nCharEXP = sprintf( cbufExp, "%d%%", GuildHouse->m_nExtraExp );
		p2DRender->TextOut( x - (int)(nCharEXP*5.8f), lpAExp->rect.top - 0, cbufExp, dwColor, 0xff000000 );
#endif
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
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );		
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );		


//	pd3dDevice->SetRenderState( D3DRS_AMBIENT,  D3DCOLOR_ARGB( 255,255,255,255) );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

	// 뷰포트 세팅 
	D3DVIEWPORT9 viewport;

	viewport.X      = p2DRender->m_ptOrigin.x + lpFace->rect.left;//2;
	viewport.Y      = p2DRender->m_ptOrigin.y + lpFace->rect.top;//5;
	viewport.Width  = lpFace->rect.Width();//p2DRender->m_clipRect.Width();
	viewport.Height = lpFace->rect.Height();// - 10;//p2DRender->m_clipRect.Height();
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;

	pd3dDevice->SetViewport(&viewport);

	// 프로젝션 
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

//#ifdef __VCRITICAL
#if __VER < 9 // __S_9_ADD
	int nHitPercent = g_pPlayer->GetHitPointPercent( 100 );
	if( nHitPercent < CRITICAL_BERSERK_HP )
	{
		D3DXVECTOR3 vecLookAt( 0.0f, -0.0f, 3.0f );
		D3DXVECTOR3 vecPos(  0.0f, 0.7f, -3.5f );
		D3DXMatrixLookAtLH( &matView, &vecPos, &vecLookAt, &D3DXVECTOR3(0.0f,1.0f,0.0f) );
		pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
		
		D3DXMatrixScaling(&matScale, 6.0f, 6.0f, 6.0f);
		D3DXMatrixTranslation(&matTrans,0.0f,-7.75f,0.0f);
		
		D3DXMatrixRotationY( &matRot1, D3DXToRadian( -20 ) );
		D3DXMatrixRotationX( &matRot2, D3DXToRadian( -5 ) );
		matRot1 *= matRot2;
	}
	else
#endif // __S_9_ADD
	{	
		D3DXVECTOR3 vecLookAt( 0.0f, -0.0f, 3.0f );
		D3DXVECTOR3 vecPos(  0.0f, 0.7f, -3.5f );
		D3DXMatrixLookAtLH( &matView, &vecPos, &vecLookAt, &D3DXVECTOR3(0.0f,1.0f,0.0f) );
		pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

		D3DXMatrixScaling(&matScale, 6.0f, 6.0f, 6.0f);
		D3DXMatrixTranslation(&matTrans,0.0f,-7.8f,0.0f);

		D3DXMatrixRotationY( &matRot1, D3DXToRadian( -20 ) );
	}
//#else
//	D3DXVECTOR3 vecLookAt( 0.0f, -0.0f, 3.0f );
//	D3DXVECTOR3 vecPos(  0.0f, 0.7f, -3.5f );
//	D3DXMatrixLookAtLH( &matView, &vecPos, &vecLookAt, &D3DXVECTOR3(0.0f,1.0f,0.0f) );
//	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
//
//	D3DXMatrixScaling(&matScale, 6.0f, 6.0f, 6.0f);
//	D3DXMatrixTranslation(&matTrans,0.0f,-7.8f,0.0f);
//	
//	D3DXMatrixRotationY( &matRot1, D3DXToRadian( -20 ) );
//#endif
	
	D3DXMatrixMultiply(&matWorld,&matWorld,&matScale);
	D3DXMatrixMultiply(&matWorld, &matWorld,&matRot1);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	/*
	// 프로젝션 
	D3DXMATRIX matProj;
	D3DXMatrixIdentity( &matProj );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	// 카메라 
	D3DXMATRIX  matView;
	D3DXMatrixIdentity( &matView );
	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	// 월드 
	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 matScale;
	D3DXMATRIXA16 matRot;
	D3DXMATRIXA16 matTrans;

	// 초기화 
	D3DXMatrixIdentity(&matScale);
	D3DXMatrixIdentity(&matRot);
	D3DXMatrixIdentity(&matTrans);
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixScaling(&matScale,6.5f,6.5f,6.5f);
	D3DXMatrixTranslation(&matTrans,0.2f,-9.7f,0.9f);
	D3DXMatrixRotationY( &matRot, D3DXToRadian( -30 ) );

	D3DXMatrixMultiply(&matWorld,&matWorld,&matScale);
	D3DXMatrixMultiply(&matWorld, &matWorld,&matRot);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
*/
	// 랜더링 
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, CWorld::m_dwBgColor, 1.0f, 0 ) ;
	if( g_pBipedMesh )
	{
		//m_pModel->FrameMove();
		g_pBipedMesh->SetGroup( 0 );
		::SetTransformView( matView );
		::SetTransformProj( matProj );

		O3D_ELEMENT* pElem = g_pBipedMesh->GetParts( PARTS_HAIR );
		
		if( pElem && pElem->m_pObject3D && g_pPlayer )
		{
#if __VER >= 14 // __BS_FIX_HAIR_AMBIENT
			pElem->m_pObject3D->m_fAmbient[0] = g_pPlayer->m_fHairColorR; // / (CWorld::m_light.Diffuse.r*g_fHairLight);
			pElem->m_pObject3D->m_fAmbient[1] = g_pPlayer->m_fHairColorG; // / (CWorld::m_light.Diffuse.g*g_fHairLight);
			pElem->m_pObject3D->m_fAmbient[2] = g_pPlayer->m_fHairColorB; // / (CWorld::m_light.Diffuse.b*g_fHairLight);
#else // __BS_FIX_HAIR_AMBIENT
			pElem->m_pObject3D->m_fAmbient[0] = g_pPlayer->m_fHairColorR / (CWorld::m_light.Diffuse.r*g_fHairLight);
			pElem->m_pObject3D->m_fAmbient[1] = g_pPlayer->m_fHairColorG / (CWorld::m_light.Diffuse.g*g_fHairLight);
			pElem->m_pObject3D->m_fAmbient[2] = g_pPlayer->m_fHairColorB / (CWorld::m_light.Diffuse.b*g_fHairLight);
#endif // __BS_FIX_HAIR_AMBIENT
		}
//#ifdef __VCRITICAL
		if( g_pPlayer->IsDie() )
		{
			D3DXVECTOR3 vDir( 0.0f, 1.0f, 0.0f );
			::SetLight( TRUE );
			SetDiffuse( 1.0f, 0.3f, 0.3f );
#ifdef __YENV
			SetAmbient( 0.9f, 0.9f, 0.9f );
#else //__YENV
			SetAmbient( 0.3f, 0.3f, 0.3f );
#endif //__YENV
			SetLightVec( vDir );		}
		else
		{	
#if __VER < 9 // __S_9_ADD
			int nHitPercent = g_pPlayer->GetHitPointPercent( 100 );
			if( nHitPercent < CRITICAL_BERSERK_HP )
			{
				::SetLight( TRUE );
				SetDiffuse( 1.0f, 0.3f, 0.3f );
#ifdef __YENV
				D3DXVECTOR3 vDir( 0.0f, 1.0f, 0.0f );
				SetAmbient( 1.0f, 1.0f, 1.0f );
#else //__YENV
				D3DXVECTOR3 vDir( 0.0f, 1.0f, 0.0f );
				SetAmbient( 0.55f, 0.55f, 0.55f );
#endif //__YENV
				SetLightVec( vDir );
			}
			else
#endif // __S_9_ADD
			{	
#ifdef __YENV
				D3DXVECTOR3 vDir( 0.0f, 0.0f, 1.0f );
				SetLightVec( vDir );
				SetDiffuse( 1.0f, 1.0f, 1.0f );
#else //__YENV
				SetDiffuse( 0.0f, 0.0f, 0.0f );
#endif //__YENV
				
				SetAmbient( 1.0f, 1.0f, 1.0f );
			}			
		}
//#else
//		SetDiffuse( 0.0f, 0.0f, 0.0f );
//		SetAmbient( 1.0f, 1.0f, 1.0f );
//#endif
#ifdef __YENV
		if( g_pPlayer->m_pModel )
		{
			O3D_ELEMENT *pElem = ((CModelObject*)g_pPlayer->m_pModel)->GetParts( PARTS_HAIR );
			
			if( pElem )
			{
				if( pElem->m_pObject3D )
				{
					pElem->m_pObject3D->m_fAmbient[0] = g_pPlayer->m_fHairColorR;
					pElem->m_pObject3D->m_fAmbient[1] = g_pPlayer->m_fHairColorG;
					pElem->m_pObject3D->m_fAmbient[2] = g_pPlayer->m_fHairColorB;
				}
			}
		}	
#endif //__YENV

	if( g_pPlayer )
		g_pPlayer->OverCoatItemRenderCheck(g_pBipedMesh);
		
		// 헬멧이 머리카락 날려야하는것이냐?  // 인벤이 있는 경우 
		O3D_ELEMENT* pElement   = NULL;
		CItemElem* pItemElem	= g_pPlayer->GetEquipItem( PARTS_CAP );

		if( pItemElem )
		{
			ItemProp* pItemProp = pItemElem->GetProp();
			if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
			{
				pElement = g_pBipedMesh->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE);
			}

			// 외투의상을 입었을경우 머리날릴것인가의 기준을 외투 모자를 기준으로 바꾼다
			CItemElem* pItemElemOvercoat	= g_pPlayer->GetEquipItem( PARTS_HAT );
			
			if( pItemElemOvercoat )
			{
				if( !pItemElemOvercoat->IsFlag( CItemElem::expired ) )
				{
					ItemProp* pItemPropOC = pItemElemOvercoat->GetProp();
					if( pItemPropOC && pItemPropOC->dwBasePartsIgnore != -1 )
					{
						if( pItemPropOC->dwBasePartsIgnore == PARTS_HEAD )
							g_pBipedMesh->SetEffect(PARTS_HAIR, XE_HIDE );
						
						g_pBipedMesh->SetEffect(pItemPropOC->dwBasePartsIgnore, XE_HIDE );
					}
					else
					{
						if( pElement )
							pElement->m_nEffect &= ~XE_HIDE;
					}
				}
				else
				{
					//g_pBipedMesh->SetEffectOff(PARTS_HAIR, XE_HIDE );
					if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
					{
						g_pBipedMesh->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE);
					}
					
					g_pBipedMesh->SetEffectOff(PARTS_HEAD, XE_HIDE );					
				}
			}
						
			
		}
		else
		{
			// 외투의상을 입었을경우 머리날릴것인가의 기준을 외투 모자를 기준으로 바꾼다
			CItemElem* pItemElemOvercoat	= g_pPlayer->GetEquipItem( PARTS_HAT );
			
			if( pItemElemOvercoat )
			{
				if( !pItemElemOvercoat->IsFlag( CItemElem::expired ) )
				{					
					ItemProp* pItemPropOC = pItemElemOvercoat->GetProp();
					if( pItemPropOC && pItemPropOC->dwBasePartsIgnore != -1 )
					{
						if( pItemPropOC->dwBasePartsIgnore == PARTS_HEAD )
							g_pBipedMesh->SetEffect(PARTS_HAIR, XE_HIDE );
						
						g_pBipedMesh->SetEffect(pItemPropOC->dwBasePartsIgnore, XE_HIDE );
					}
				}
			}
							
		}

//gmpbigsun : 윈도 페이스 고정라이트
	::SetLight( FALSE );
 	::SetFog( FALSE );
 	SetDiffuse( 1.0f, 1.0f, 1.0f );
 	SetAmbient( 1.0f, 1.0f, 1.0f );

		g_pBipedMesh->Render(pd3dDevice,&matWorld);

		SetDiffuse( 0.0f, 0.0f, 0.0f );
		SetAmbient( 1.0f, 1.0f, 1.0f );
	}

	pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
}
HRESULT CWndStatus::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if( m_pVBHPGauge == NULL )
	{
		m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBHPGauge, NULL );
		m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBMPGauge, NULL );
		m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBFPGauge, NULL );
		m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBEXPGauge, NULL );
#if __VER >= 15 // __GUILD_HOUSE
		m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBAEXPGauge, NULL );
#endif
		m_nHPWidth = -1;
		m_nMPWidth = -1;
		m_nFPWidth = -1;
		m_nEXPWidth = -1;
		m_nPXPWidth = -1;
	}

#ifdef __YDEBUG
	m_texGauEmptyNormal.SetInvalidate(m_pApp->m_pd3dDevice); 
	m_texGauEmptySmall.SetInvalidate(m_pApp->m_pd3dDevice);  
	m_texGauFillNormal.SetInvalidate(m_pApp->m_pd3dDevice);  
	m_texGauFillSmall.SetInvalidate(m_pApp->m_pd3dDevice);   
#endif //__YDEBUG
	
	return S_OK;
}
HRESULT CWndStatus::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
    SAFE_RELEASE( m_pVBHPGauge );
    SAFE_RELEASE( m_pVBMPGauge );
    SAFE_RELEASE( m_pVBFPGauge );
    SAFE_RELEASE( m_pVBEXPGauge );
#if __VER >= 15 // __GUILD_HOUSE
	SAFE_RELEASE( m_pVBAEXPGauge );
#endif

#if __VER >= 19
	m_HPGaugeTexture.Invalidate();
	m_MPGaugeTexture.Invalidate();
	m_FPGaugeTexture.Invalidate();
	m_EXPGaugeTexture.Invalidate();
	m_AEXPGaugeTexture.Invalidate();
#endif// __VER >= 19

#ifdef __YDEBUG
	m_texGauEmptyNormal.Invalidate(); 
	m_texGauEmptySmall.Invalidate(); 
	m_texGauFillNormal.Invalidate(); 
	m_texGauFillSmall.Invalidate(); 
#endif //__YDEBUG
	
	return S_OK;//return S_OK;
}
HRESULT CWndStatus::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	return InvalidateDeviceObjects();
}

/*
void CWndStatus::RenderGauge( C2DRender* p2DRender, CRect* pRect, DWORD dwColor, LPDIRECT3DVERTEXBUFFER9 pVBGauge, CTexture* pTexture )
{
//	pTexture = &m_texGauEmptyNormal;
    LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice; 
	CPoint pt = pRect->TopLeft();
	pt += p2DRender->m_ptOrigin;
	CPoint ptCenter = pTexture->m_ptCenter;
	pt -= ptCenter;

	///////////////////////////////////////////////////////////////
	int nTexWidth = ( pTexture->m_size.cx ) / 3;
	int nTexHeight = pTexture->m_size.cy;

	FLOAT left   = pt.x;
	FLOAT top    = pt.y;
	FLOAT right  = pt.x + nTexWidth;//( pTexture->m_size.cx );
	FLOAT bottom = pt.y + nTexHeight;//( pTexture->m_size.cy );

	int nWidth = ( pRect->Width() / nTexWidth );// - 2;

	// 기본 패턴으로 완성될 수 있는건 2이다. 2보다 작으면 이미지가 깨질 수 있으니 리턴.
	if( nWidth < 2 )
		return;
	
	int nTileNum = 3;
	int nVertexNum = 3 * 6;
	
	DRAWVERTEX* pVertices,* pVertices_;
	HRESULT hr = m_pVBGauge->Lock( 0, sizeof(DRAWVERTEX) * nVertexNum, (void**) &pVertices_, 0 );
	if(hr != D3D_OK) return;
	{
		SIZE size = m_texGauEmptyNormal.m_size;//
		SIZE sizePitch = m_texGauEmptyNormal.m_sizePitch;//
		size.cx /= 3;
		
		pVertices = pVertices_;
		for( int i = 0; i < nVertexNum; i++ )
		{
			pVertices->z = 0;
			pVertices->rhw = 1.0f;
			pVertices->color = dwColor;
			pVertices++;
		}
		pVertices = pVertices_;

		left   = pt.x;
		right  = pt.x + nTexWidth;
			
		pVertices->x = (FLOAT)left - 0.5f;
		pVertices->y = (FLOAT)top - 0.5f;
		pVertices->u = 0.0f;
		pVertices->v = 0.0f;
		pVertices++;

		pVertices->x = (FLOAT)right - 0.5f;
		pVertices->y = (FLOAT)top - 0.5f;
		pVertices->u = (FLOAT)size.cx / sizePitch.cx;
		pVertices->v = 0.0f;
		pVertices++;

		pVertices->x = (FLOAT)left - 0.5f;
		pVertices->y = (FLOAT)bottom - 0.5f;
		pVertices->u = 0.0f;
		pVertices->v = (FLOAT)size.cy / sizePitch.cy;
		pVertices++;

		pVertices->x = (FLOAT)right - 0.5f;
		pVertices->y = (FLOAT)top - 0.5f;
		pVertices->u = (FLOAT)size.cx / sizePitch.cx;
		pVertices->v = 0.0f;
		pVertices++;
		
		pVertices->x = (FLOAT)left - 0.5f;
		pVertices->y = (FLOAT)bottom - 0.5f;
		pVertices->u = 0.0f;
		pVertices->v = (FLOAT)size.cy / sizePitch.cy;
		pVertices++;
		
		pVertices->x = (FLOAT)right - 0.5f;
		pVertices->y = (FLOAT)bottom - 0.5f;
		pVertices->u = (FLOAT)size.cx / sizePitch.cx;
		pVertices->v = (FLOAT)size.cy / sizePitch.cy;
		pVertices++;

		//////////////////////////////////
		
		left   = pt.x + nTexWidth;
		right  = pt.x + nTexWidth + ( ( nWidth - 2 ) * nTexWidth );
		
		pVertices->x = (FLOAT)left - 0.5f;
		pVertices->y = (FLOAT)top - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 1.0f;
		pVertices->v = 0.0f;
		pVertices++;
		
		pVertices->x = (FLOAT)right - 0.5f;
		pVertices->y = (FLOAT)top - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 2.0f;
		pVertices->v = 0.0f;
		pVertices++;
		
		pVertices->x = (FLOAT)left - 0.5f;
		pVertices->y = (FLOAT)bottom - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 1.0f;
		pVertices->v = ( (FLOAT)size.cy / sizePitch.cy ) * 1.0f;
		pVertices++;
		
		pVertices->x = (FLOAT)right - 0.5f;
		pVertices->y = (FLOAT)top - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 2.0f;
		pVertices->v = 0.0f;
		pVertices++;
		
		pVertices->x = (FLOAT)left - 0.5f;
		pVertices->y = (FLOAT)bottom - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 1.0f;
		pVertices->v = ( (FLOAT)size.cy / sizePitch.cy ) * 1.0f;
		pVertices++;
		
		pVertices->x = (FLOAT)right - 0.5f;
		pVertices->y = (FLOAT)bottom - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 2.0f;
		pVertices->v = ( (FLOAT)size.cy / sizePitch.cy ) * 1.0f;
		pVertices++;

		//////////////////////////////////
		left   = pt.x + ( ( nWidth - 1 ) * nTexWidth );
		right  = pt.x + ( ( nWidth ) * nTexWidth );

		pVertices->x = (FLOAT)left - 0.5f;
		pVertices->y = (FLOAT)top - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 2.0f;
		pVertices->v = 0.0f;//(FLOAT)size.cy / sizePitch.cy;
		pVertices++;
		
		pVertices->x = (FLOAT)right - 0.5f;
		pVertices->y = (FLOAT)top - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 3.0f;
		pVertices->v = 0.0f;
		pVertices++;
		
		pVertices->x = (FLOAT)left - 0.5f;
		pVertices->y = (FLOAT)bottom - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 2.0f;
		pVertices->v = ( (FLOAT)size.cy / sizePitch.cy ) * 1.0f;
		pVertices++;
		
		pVertices->x = (FLOAT)right - 0.5f;
		pVertices->y = (FLOAT)top - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 3.0f;
		pVertices->v = 0.0f;
		pVertices++;
		
		pVertices->x = (FLOAT)left - 0.5f;
		pVertices->y = (FLOAT)bottom - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 2.0f;
		pVertices->v = ( (FLOAT)size.cy / sizePitch.cy ) * 1.0f;
		pVertices++;
		
		pVertices->x = (FLOAT)right - 0.5f;
		pVertices->y = (FLOAT)bottom - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 3.0f;
		pVertices->v = ( (FLOAT)size.cy / sizePitch.cy ) * 1.0f;
		pVertices++;

	}
	m_pVBGauge->Unlock(); 

	///////////////////////////////////////////////////////////////

	pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, 1 );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, 1 );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );		
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );		
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );		
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );		

	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );	
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( 255, 0, 0, 0 ));
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR  );

	//pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
	//pd3dDevice->SetRenderState( D3DRS_ALPHAREF,         0x08 );
	//pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC,  D3DCMP_GREATEREQUAL );
	//pd3dDevice->SetRenderState( D3DRS_FILLMODE,   D3DFILL_SOLID );
	//pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
	//pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
	//pd3dDevice->SetRenderState( D3DRS_CLIPPING,         TRUE );
	//pd3dDevice->SetRenderState( D3DRS_CLIPPLANEENABLE,  FALSE );
	//pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND,      D3DVBF_DISABLE );
	//pd3dDevice->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE );
	//pd3dDevice->SetRenderState( D3DRS_FOGENABLE,        FALSE );
	//pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE,
	//	D3DCOLORWRITEENABLE_RED  | D3DCOLORWRITEENABLE_GREEN |
	//	D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
	
	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetTexture( 0, pTexture->m_pTexture );
	pd3dDevice->SetFVF( D3DFVF_DRAWVERTEX );
	pd3dDevice->SetStreamSource( 0, m_pVBGauge, 0,sizeof( DRAWVERTEX ) );
	pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, nVertexNum / 3 );
}				
*/
void CWndStatus::OnInitialUpdate()
{
//	m_pModel = prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice,g_pPlayer->m_dwType,g_pPlayer->m_dwIndex);
//	prj.m_modelMng.LoadMotion(m_pModel,g_pPlayer->m_dwType,g_pPlayer->m_dwIndex,0);
	CWndNeuz::OnInitialUpdate();
	
	RestoreDeviceObjects();
	//m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( DRAWVERTEX ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_DRAWVERTEX, D3DPOOL_DEFAULT, &m_pVBGauge, NULL );
#if __VER >= 19
	m_HPGaugeTexture.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "Targetgauge01.tga" ), 0xffff00ff, TRUE );
	m_MPGaugeTexture.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "Targetgauge02.tga" ), 0xffff00ff, TRUE );
	m_FPGaugeTexture.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "Targetgauge03.tga" ), 0xffff00ff, TRUE );			
	m_EXPGaugeTexture.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "BarSky.tga" ), 0xffff00ff, TRUE );
	m_AEXPGaugeTexture.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "BarSky.tga" ), 0xffff00ff, TRUE );
#else
	m_texGauEmptyNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptyNormal.bmp" ), 0xffff00ff, TRUE );
	m_texGauEmptySmall.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptySmall.bmp" ), 0xffff00ff, TRUE );
	m_texGauFillNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptyNormal.bmp" ), 0xffff00ff, TRUE );
	m_texGauFillSmall.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptySmall.bmp" ), 0xffff00ff, TRUE );
#endif// __VER >= 19

	//m_texGauEmptySmall  
	//m_texGauFillNormal  
	//m_texGauFillSmall   
	
	// 장착, 게이지에 나올 캐릭터 오브젝트 설정 
	if( g_pBipedMesh == NULL )
	{
		int nMover = (g_pPlayer->GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);
		g_pBipedMesh = (CModelObject*)prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE );
		prj.m_modelMng.LoadMotion( g_pBipedMesh,  OT_MOVER, nMover, 0 );
		CMover::UpdateParts( g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh, g_pPlayer->m_aEquipInfo, g_pBipedMesh, &g_pPlayer->m_Inventory );
	}
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CPoint point( rectRoot.left, rectRoot.top );
	Move( point );

}

BOOL CWndStatus::Initialize(CWndBase* pWndParent,DWORD dwWndId)
{
	return InitDialog( g_Neuz.GetSafeHwnd(), APP_STATUS1 );
}

BOOL CWndStatus::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	switch(nID)
	{
		case 100: // wnd2
			/*
			g_Music.m_nMusicVolume = !g_Music.m_nMusicVolume;
			if ( g_Music.m_nMusicVolume == 0 )
				g_Music.Stop();
			else
				g_Music.Play();
				*/
			break;
		case 101: // wnd1 
			g_SoundMng.m_nSoundVolume = !g_SoundMng.m_nSoundVolume;
			break;
	}
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndStatus::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndStatus::SetWndRect( CRect rectWnd, BOOL bOnSize  )
{
	m_nHPWidth = -1;
	m_nMPWidth = -1;
	m_nFPWidth = -1;
	m_nEXPWidth = -1;
	m_nPXPWidth = -1;
	
	CWndNeuz::SetWndRect( rectWnd, bOnSize );
}
void CWndStatus::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndStatus::OnLButtonDown(UINT nFlags, CPoint point)
{	
	if(++m_nDisplay > 2)
		m_nDisplay = 0;
}
BOOL CWndStatus::OnEraseBkgnd(C2DRender* p2DRender)
{
	return CWndBase::OnEraseBkgnd( p2DRender );
	//oint pt = m_rectClient.TopLeft() - m_rectWindow.TopLeft();
	CRect rect = GetClientRect();
	p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );


	//p2DRender->RenderFillRect( rect, D3DCOLOR_ARGB( 255, 70, 70, 170 ) );
	p2DRender->RenderFillRect( rect, D3DCOLOR_ARGB( 100, 0, 0, 0 ) );

	return TRUE;
}

#if __VER >= 15 // __GUILD_HOUSE
void CWndStatus::OnMouseWndSurface( CPoint point )
{
	static LPWNDCTRL lpPxp  = GetWndCtrl( WIDC_CUSTOM5 );
	BOOL bInPoint = FALSE;
	if( lpPxp )
		bInPoint = PtInRect( &lpPxp->rect, point );
	else return;

	if( bInPoint )	
	{
		CRect DrawRect = lpPxp->rect;
		CPoint point2 = point;
		ClientToScreen( &point2 );
		ClientToScreen( &DrawRect );
		g_toolTip.PutToolTip( 100000, GETTEXT( TID_TOOLTIP_RESTPOINT_GAGE ), DrawRect, point2 );
	}
}
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 퀘스트 아이템 정보
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
CWndQuestItemInfo::CWndQuestItemInfo() : m_pItemBase(NULL)
{
}

CWndQuestItemInfo::~CWndQuestItemInfo()
{
}

BOOL CWndQuestItemInfo::Initialize( CWndBase* pWndParent, CItemBase* pItemBase )
{
	return Create( pItemBase, APP_QUESTITEM_INFO, pWndParent );//RUE;
}
BOOL CWndQuestItemInfo::Create( CItemBase* pItemBase, UINT nID, CWndBase* pWndParent )
{
	m_pItemBase = pItemBase;
	CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), nID, 0, m_Position, pWndParent );
	((CWndEdit*)GetDlgItem( WIDC_EDIT1 ))->SetVisible( FALSE );

	MoveParentCenter();
	return 1;
}

BOOL CWndQuestItemInfo::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndQuestItemInfo::OnDraw(C2DRender* p2DRender)
{
	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_EDIT1 );
	CRect rectCtrl = lpWndCtrl->rect;
	rectCtrl.top += 10;
	rectCtrl.left += 10;
	rectCtrl.right -= 10;
	rectCtrl.bottom -= 10;

	m_pItemBase->GetTexture()->Render(p2DRender, rectCtrl.TopLeft(), 200);
	p2DRender->TextOut(rectCtrl.left + 34, rectCtrl.top + 16 - 5, 1, 1, m_pItemBase->GetProp()->szName, 0xff00ffff);

	CEditString	string = "";
	string.Init( m_pFont, &rectCtrl );
	string = m_pItemBase->GetProp()->szCommand;
	p2DRender->TextOut_EditString( rectCtrl.left , rectCtrl.top + 40/* '+40'의 의미는 아이콘있는 곳에서 한줄 아래에 찍겠다는 의미 */, string);
}

void CWndQuestItemInfo::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndEdit* pWndEdit	= (CWndEdit*)GetDlgItem( WIDC_EDIT1 );

	if( pWndEdit )
		pWndEdit->EnableWindow( FALSE );
}

BOOL CWndQuestItemInfo::OnCommand(UINT nID, DWORD dwMessage, CWndBase *pWndBase)
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

void CWndQuestItemInfo::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}

void CWndQuestItemInfo::OnLButtonUp( UINT nFlags, CPoint point )
{
}

void CWndQuestItemInfo::OnLButtonDown( UINT nFlags, CPoint point )
{
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 매크로
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
CWndMacro::CWndMacro()
{
}
CWndMacro::~CWndMacro()
{
}
void CWndMacro::OnDraw( C2DRender* p2DRender )
{

}
void CWndMacro::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
}
BOOL CWndMacro::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 50 ,50, 300, 300 );
	//SetTitle( GETTEXT( TID_APP_MACRO )  );
	return CWndNeuz::Create( 0 | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId );
}
BOOL CWndMacro::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndMacro::OnSize( UINT nType, int cx, int cy )
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndMacro::OnLButtonUp( UINT nFlags, CPoint point )
{
}
void CWndMacro::OnLButtonDown( UINT nFlags, CPoint point )
{
}
BOOL CWndMacro::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 스타일
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
CWndStyle::CWndStyle()
{
}
CWndStyle::~CWndStyle()
{
}
void CWndStyle::OnDraw( C2DRender* p2DRender )
{

}
void CWndStyle::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
}
BOOL CWndStyle::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 50 ,50, 300, 300 );
	//SetTitle( GETTEXT( TID_APP_STYLE )  );
	return CWndNeuz::Create( 0 | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId );
}
BOOL CWndStyle::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndStyle::OnSize( UINT nType, int cx, int cy )
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndStyle::OnLButtonUp( UINT nFlags, CPoint point )
{
}
void CWndStyle::OnLButtonDown( UINT nFlags, CPoint point )
{
}
BOOL CWndStyle::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 내정보
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
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
}
BOOL CWndOptMyInfo::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 50 ,50, 300, 300 );
	SetTitle( GETTEXT( TID_APP_OPTION_MYINFO )  );
	return CWndNeuz::Create( 0 | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId );
}
BOOL CWndOptMyInfo::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndOptMyInfo::OnSize( UINT nType, int cx, int cy )
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
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 단축키
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndEmotion::CWndEmotion() 
{ 
	m_lpSelectTreeElem = NULL;
} 
CWndEmotion::~CWndEmotion() 
{ 
} 
void CWndEmotion::OnDraw( C2DRender* p2DRender ) 
{ 
	CRect rect = GetClientRect();
/*
	CD3DFont* pFont = p2DRender->m_pFont;//GetFont();
	p2DRender->m_pFont = m_pTheme->m_pFontGameTitle;
	p2DRender->TextOut( 5, 10, _T( "Lv.0 기본 제스춰" ) );
	p2DRender->TextOut( 5, 40, _T( "Lv.1 생활 표현" ) );
	p2DRender->TextOut( 5, 70, _T( "Lv.2 도발" ) );
	p2DRender->TextOut( 5,100, _T( "Lv.3 춤" ) );
	p2DRender->TextOut( 5,130, _T( "Lv.4 곡예" ) );
	p2DRender->TextOut( 5,160, _T( "Lv.5 무술" ) );

	p2DRender->m_pFont = pFont;
	*/
	p2DRender->TextOut( 5, rect.Height() - 50, _T( "연결 대사" ) );
} 
void CWndEmotion::OnInitialUpdate() 
{ 
/*
	m_wndViewCtrl.Create( WBS_CHILD, CRect( 5, 5, m_rectClient.Width() - 5,m_rectClient.Height() - 60 ), this, 1005 );//,m_pSprPack,-1);//m_pSprPack,16);

	PropMotion* pProp;
	for( int i = 0; i < prj.m_aPropMotion.GetSize(); i++ )
	{
		pProp = prj.GetPropMotion( i );
		if( pProp && pProp->dwRequireLv )
		{
			LPTREEELEM lpTreeElem = m_wndViewCtrl.FindTreeElem( pProp->szRoot );
			if( lpTreeElem == NULL )
			{
				lpTreeElem = m_wndViewCtrl.InsertItem( NULL, pProp->szRoot, 0 ); // root
				m_wndViewCtrl.InsertItem( lpTreeElem, pProp->szName, pProp->dwID ) ;
			}
			else
				m_wndViewCtrl.InsertItem( lpTreeElem, pProp->szName, pProp->dwID );
		}
	}
	CRect rect = GetClientRect();
	rect.top = rect.bottom - 55;
	rect.bottom = rect.top + 20;
	rect.left = 70;
	rect.right -= 5;
	m_wndEdit.Create( g_Neuz.GetSafeHwnd(), 0, rect, this, 1001 );

	rect = GetClientRect();
	rect.top = rect.bottom - 30;
	rect.bottom = rect.top + 25;
	rect.left = rect.right - 80;
	rect.right -= 5;
	m_wndDefault.Create( _T( "Default" ), 0, rect, this, 1002 );
*/
	CWndNeuz::OnInitialUpdate(); 
} 
BOOL CWndEmotion::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	//SetTitle( _T( "감정표현" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
BOOL CWndEmotion::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndEmotion::OnSize( UINT nType, int cx, int cy ) 
{ 
	m_wndViewCtrl.SetWndRect( CRect( 5, 5, m_rectClient.Width() - 5, m_rectClient.Height() - 60 ) );

	CRect rect = GetClientRect();
	rect.top = rect.bottom - 55;
	rect.bottom = rect.top + 20;
	rect.left = 70;
	rect.right -= 5;
	m_wndEdit.SetWndRect( rect );

	rect = GetClientRect();
	rect.top = rect.bottom - 30;
	rect.bottom = rect.top + 25;
	rect.left = rect.right - 80;
	rect.right -= 5;
	m_wndDefault.SetWndRect( rect );

	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndEmotion::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndEmotion::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndEmotion::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	/*
	switch( nID )
	{
	case 1001:
		if( message == EN_CHANGE && m_lpSelectTreeElem )
		{
			_tcscpy( prj.GetPropMotion( m_lpSelectTreeElem->m_dwData )->szLink, m_wndEdit.m_string );
		}
	case 1005: // view ctrl
		{
			if( message == WNM_DBLCLK )
			{
				LPTREEELEM lpTreeElem = (LPTREEELEM)pLResult;
				if( lpTreeElem )
				{
					if( lpTreeElem->m_dwData )
					{
						g_pPlayer->SendActMsg( OBJMSG_MOTION, lpTreeElem->m_dwData );
						m_wndEdit.SetString( prj.GetPropMotion( lpTreeElem->m_dwData )->szLink );
						m_lpSelectTreeElem = lpTreeElem;
					}
					else
						;//g_pPlayer->SetMotion( lpTreeElem->m_dwData );
				}
			}
		}
	}
	*/
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//EMPTY_WNDCLASSMEMBER( CWndFriend, _T( "친구" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//EMPTY_WNDCLASSMEMBER( CWndGuild, _T( "길드" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//EMPTY_WNDCLASSMEMBER( CWndClub, _T( "클럽" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//EMPTY_WNDCLASSMEMBER( CWndMessage, _T( "매신저" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//EMPTY_WNDCLASSMEMBER( CWndEMail, _T( "이메일" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//EMPTY_WNDCLASSMEMBER( CWndBBS, _T( "게시판" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//EMPTY_WNDCLASSMEMBER( CWndSearchPeople, _T( "사람찾기" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//EMPTY_WNDCLASSMEMBER( CWndSearchGuild, _T( "길드찾기" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//EMPTY_WNDCLASSMEMBER( CWndSearchClub, _T( "클럽찾기" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//EMPTY_WNDCLASSMEMBER( CWndSearchShop, _T( "상점찾기" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////


//EMPTY_WNDCLASSMEMBER( CWndInfoEvent, _T( "이벤트" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//EMPTY_WNDCLASSMEMBER( CWndInfoVersion, _T( "버전" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//EMPTY_WNDCLASSMEMBER( CWndInfoSchedule, _T( "개발 스케쥴" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//EMPTY_WNDCLASSMEMBER( CWndJoinVote, _T( "전자투표" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//EMPTY_WNDCLASSMEMBER( CWndJoinEvent, _T( "이벤트 참여 신청" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//EMPTY_WNDCLASSMEMBER( CWndSequence, _T( "시퀀스" ) )


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 카드 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// CWndLogOut : 계정 화면으로, 캐릭터 선택 화면으로 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CWndLogOut::Initialize(CWndBase* pWndParent,DWORD dwWndId)
{
	CRect rect = m_pWndRoot->MakeCenterRect( 250, 130 );
/*
	Create( _T( "매시지 박스" ), MB_OKCANCEL, rect, APP_MESSAGEBOX );//dwWndId );
	m_wndText.SetString( _T( "접속을 종료하시겠습니까?" ) );
*/
	Create( _T( prj.GetText(TID_DIAG_0068) ), MB_OKCANCEL, rect, APP_MESSAGEBOX );//dwWndId );
	m_wndText.SetString( _T( prj.GetText(TID_DIAG_0069) ) );
	m_wndText.ResetString();
	if( g_WndMng.m_pWndWorld && g_WndMng.m_pWndWorld->GetMouseMode() == 1 )	// FPS모드일때
	{
		g_WndMng.m_pWndWorld->SetMouseMode( 0 );
	}

	// 게이지 위쪽으로 나오게 하기
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( ( rectRoot.right - rectWindow.Width() ) / 2, 70 );
	Move( point );

	return CWndMessageBox::Initialize( pWndParent, dwWndId );
}
BOOL CWndLogOut::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( message == WNM_CLICKED  )
	{
		switch(nID)
		{
			case IDCANCEL:   
				Destroy(); 
				break;
			case IDOK:    
				if( ::GetLanguage() == LANG_JAP )
				{
					if( g_pPlayer->GetSex() == SEX_MALE )
						PLAYSND( "VocMale-logout.wav" );
					else
						PLAYSND( "VocFemale-logout.wav" );
				}

#if __VER >= 15
				g_Neuz.ResetStaticValues( );
#endif

				if( g_pPlayer->EndMotion() )
					g_Neuz.m_timerQuit.Set( SEC( 3 ) );		

				if( g_pPlayer->IsRegionAttr( RA_SAFETY ) || g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
				{
					::PostMessage( g_Neuz.GetSafeHwnd(), WM_LOGOUT, 0, 0 );
				}
				else
				{
					if( g_WndMng.m_pLogOutWaitting == NULL )
					{
						g_WndMng.m_pLogOutWaitting = new CWndLogOutWaitting;
#ifdef __FIX_WND_1109
						g_WndMng.m_pLogOutWaitting->Initialize( NULL );	// 康
#else	// __FIX_WND_1109
						g_WndMng.m_pLogOutWaitting->Initialize( this );	// 康
#endif	// __FIX_WND_1109
						g_WndMng.m_pLogOutWaitting->SetIsLogOut(TRUE);
						SetVisible(FALSE);
					}
				}

				break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}


CWndLogOutWaitting::CWndLogOutWaitting() 
{ 
} 
CWndLogOutWaitting::~CWndLogOutWaitting() 
{ 
} 

BOOL CWndLogOutWaitting::Initialize( CWndBase* pWndParent ) 
{ 
	LPWNDAPPLET lpWndApplet = m_resMng.GetAt ( APP_MESSAGE_TEXT );
	CRect rect = CRect( 0, 0, lpWndApplet->size.cx, lpWndApplet->size.cy );
	return CWndNeuz::Create( WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_MODAL, rect, pWndParent, APP_MESSAGE_TEXT ); 
} 


BOOL CWndLogOutWaitting::Process()
{
	DWORD dwCurrentTime = g_tmCurrent;
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	DWORD dwEndWaitTime = dwTime - dwCurrentTime;
	dwEndWaitTime /= 1000;

	CString str;
	str.Format( prj.GetText( TID_PK_TIME_END ), dwEndWaitTime );
	
	if( dwEndWaitTime > 0 )
	{
		pWndText->SetString( str );
	}
	
	if( dwCurrentTime > dwTime )
	{
		if( m_bIsLogOut )
		{
			::PostMessage( g_Neuz.GetSafeHwnd(), WM_LOGOUT, 0, 0 );
		}
		else
		{
			::PostMessage( g_Neuz.GetSafeHwnd(), WM_CLOSE, 0, 0 );
		}

		Destroy(); 
	}
	return TRUE;
}

BOOL CWndLogOutWaitting::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
void CWndLogOutWaitting::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate(); 

	m_wndTitleBar.SetVisible( FALSE );
	
	m_bIsLogOut = FALSE;
	
	dwTime = g_tmCurrent+SEC(TIMEWAIT_CLOSE);

	// 게이지 위쪽으로 나오게 하기
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( ( rectRoot.right - rectWindow.Width() ) / 2, 70 );
	Move( point );

	// 종료 패킷 서버 전송
	g_DPlay.SendReqLeave();	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// CWndQuit  : 프로그램 종료
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CWndQuit::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rect = m_pWndRoot->MakeCenterRect( 250, 130 );
	/*
	Create( _T( "매시지 박스" ), MB_OKCANCEL, rect, APP_MESSAGEBOX );//dwWndId );
	m_wndText.SetString( _T( "프로그램을 종료하시겠습니까?" ) );
	*/
	Create( _T( prj.GetText(TID_DIAG_0068) ), MB_OKCANCEL, rect, APP_MESSAGEBOX );//dwWndId );
	m_wndText.SetString( _T( prj.GetText(TID_DIAG_0070) ) );
	m_wndText.ResetString();
	if( g_WndMng.m_pWndWorld && g_WndMng.m_pWndWorld->GetMouseMode() == 1 )	// FPS모드일때
	{
		g_WndMng.m_pWndWorld->SetMouseMode( 0 );
	}

	// 게이지 위쪽으로 나오게 하기
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( ( rectRoot.right - rectWindow.Width() ) / 2, 70 );
	Move( point );

	m_bFlag = FALSE;
	
	return CWndMessageBox::Initialize( pWndParent, dwWndId );
}
BOOL CWndQuit::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( message == WNM_CLICKED  )
	{
		switch(nID)
		{
			case IDCANCEL:   
				Destroy(); 
				break;
			case IDOK:       
				if( m_bFlag )
					return TRUE;

				m_bFlag = TRUE;

				if( ::GetLanguage() == LANG_JAP )
				{
					if( g_pPlayer->GetSex() == SEX_MALE )
						PLAYSND( "VocMale-logout.wav" );
					else
						PLAYSND( "VocFemale-logout.wav" );
				}

				if( g_pPlayer->EndMotion() )
					g_Neuz.m_timerQuit.Set( SEC( 3 ) );		
				
				if( g_pPlayer->IsRegionAttr( RA_SAFETY ))
				{
					::PostMessage( g_Neuz.GetSafeHwnd(), WM_CLOSE, 0, 0 );
				}
				else
				{
					if( g_WndMng.m_pLogOutWaitting == NULL )
					{
						g_WndMng.m_pLogOutWaitting = new CWndLogOutWaitting;
#ifdef __FIX_WND_1109
						g_WndMng.m_pLogOutWaitting->Initialize( NULL );	// 康
#else	// __FIX_WND_1109
						g_WndMng.m_pLogOutWaitting->Initialize( this );	// 康
#endif	// __FIX_WND_1109
						g_WndMng.m_pLogOutWaitting->SetIsLogOut(FALSE);
						SetVisible(FALSE);
					}
				}

				break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// CWndRevival : 부활 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndRevival::CWndRevival() 
{ 
	m_pLodeLight = NULL;
	m_pLodeStar = NULL;
	m_pRevival = NULL;
#if __VER >= 9 // __S_9_ADD
	m_pShop = NULL;
#endif // __S_9_ADD
}
CWndRevival::~CWndRevival() 
{ 
} 
void CWndRevival::OnDraw( C2DRender* p2DRender ) 
{ 
#ifdef __JEFF_11_4
	BOOL	bArena	= g_pPlayer->GetWorld() && g_pPlayer->GetWorld()->IsArena(); 
#endif	// __JEFF_11_4
	if( NULL == g_pPlayer->m_Inventory.GetAtItemId( II_SYS_SYS_SCR_RESURRECTION )
#ifdef __JEFF_11_4
		|| bArena
#endif	// __JEFF_11_4
		)
		m_pRevival->EnableWindow( FALSE );
	else
		m_pRevival->EnableWindow( TRUE );
} 
void CWndRevival::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	m_pLodeLight = (CWndButton*)GetDlgItem( WIDC_REVIVAL_TO_LODELIGHT );
	m_pLodeStar = (CWndButton*)GetDlgItem( WIDC_REVIVAL_TO_LODESTAR );
	m_pRevival = (CWndButton*)GetDlgItem( WIDC_REVIVAL_STAND );
#if __VER >= 9 // __S_9_ADD
	m_pShop = (CWndButton*)GetDlgItem( WIDC_REVIVAL_SHOP );
#endif // __S_9_ADD
	m_pLodeLight->m_strToolTip = _T( prj.GetText(TID_GAME_TOOLTIP_LODELIGHT) );//"로드라이트는 여행자가 직접 지정한 부활 위치입니다." );
	m_pLodeStar->m_strToolTip = _T( prj.GetText(TID_GAME_TOOLTIP_LODESTAR) );//"로드스타는 인근 마을의 부활 위치입니다." );
	m_pRevival->m_strToolTip = _T( prj.GetText(TID_GAME_TOOLTIP_OGNPOINT) );
#if __VER >= 9 // __S_9_ADD
	m_pShop->m_strToolTip = _T( prj.GetText(TID_GAME_TOOLTIP_SHOP) );
#endif // __S_9_ADD

#ifdef __JEFF_11_4
	BOOL	bArena	= g_pPlayer->GetWorld() && g_pPlayer->GetWorld()->IsArena(); 
#endif	// __JEFF_11_4
	if( NULL == g_pPlayer->m_Inventory.GetAtItemId( II_SYS_SYS_SCR_RESURRECTION )
#ifdef __JEFF_11_4
		|| bArena
#endif	// __JEFF_11_4
		)
		m_pRevival->EnableWindow( FALSE );
	else
		m_pRevival->EnableWindow( TRUE );
	
	CWndWebBox* pWndWebBox = (CWndWebBox*)g_WndMng.GetApplet( APP_WEBBOX );

	if(pWndWebBox)
		pWndWebBox->Destroy();

#ifdef __NEW_WEB_BOX
	CWndHelperWebBox* pWndHelperWebBox = ( CWndHelperWebBox* )g_WndMng.GetApplet( APP_WEBBOX2 );

	if( pWndHelperWebBox )
		pWndHelperWebBox->Destroy();
#else // __NEW_WEB_BOX
	CWndWebBox2* pWndWebBox2 = (CWndWebBox2*)g_WndMng.GetApplet( APP_WEBBOX2 );

	if(pWndWebBox2)
		pWndWebBox2->Destroy();
#endif // __NEW_WEB_BOX

	m_wndTitleBar.SetVisible( FALSE );

#if __VER >= 9 // __S_9_ADD
	if( m_pLodeLight )
	{
		CRect RevivalRect = m_pRevival->GetWindowRect(TRUE);
		CRect LodeLightRect = m_pLodeLight->GetWindowRect(TRUE);
		m_pRevival->SetWndRect( LodeLightRect, TRUE );
		m_pShop->SetWndRect( RevivalRect, TRUE );

		m_pLodeLight->EnableWindow( FALSE );
		m_pLodeLight->SetVisible( FALSE );		

//		crect = m_pLodeStar->GetWindowRect(TRUE);
//		crect.top += 2;
//		crect.bottom += 2;
//		m_pLodeStar->SetWndRect( crect, TRUE );
//		crect = m_pRevival->GetWindowRect(TRUE);
//		crect.top += 8;
//		crect.bottom += 8;
//		m_pRevival->SetWndRect( crect, TRUE );
		
//		crect = GetWndRect();
//		crect.bottom -= 31;
//		SetWndRect( crect, TRUE );
	}
#endif // __S_9_ADD
	
	// 게이지 위쪽으로 나오게 하기
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( ( rectRoot.right - rectWindow.Width() ) / 2, 70 );
	Move( point );
} 
BOOL CWndRevival::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	InitDialog( g_Neuz.GetSafeHwnd(), APP_REVIVAL, WBS_MODAL );

	if( g_WndMng.m_pWndWorld && g_WndMng.m_pWndWorld->GetMouseMode() == 1 )	// FPS모드일때
	{
		g_WndMng.m_pWndWorld->SetMouseMode( 0 );
	}
	
	return TRUE;
} 
BOOL CWndRevival::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndRevival::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndRevival::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndRevival::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
void CWndRevival::EnableButton( int nButton, BOOL bEnable )
{
	switch( nButton )
	{
	case WIDC_REVIVAL_TO_LODELIGHT:
		{
			m_pLodeLight->EnableWindow(bEnable);
			break;
		}
	case WIDC_REVIVAL_TO_LODESTAR:
		{
			m_pLodeStar->EnableWindow(bEnable);
			break;
		}
	}
}

BOOL CWndRevival::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	BOOL bClose = TRUE;
	switch( nID )
	{
		case WIDC_REVIVAL_TO_LODELIGHT:
			{
				g_DPlay.SendHdr( PACKETTYPE_REVIVAL_TO_LODELIGHT );
				
				CWndBase* pWndBase = g_WndMng.GetWndBase( APP_RESURRECTION );

				if( pWndBase )
					g_DPlay.SendResurrectionCancel();

				break;
			}
		case WIDC_REVIVAL_TO_LODESTAR:
			{
				g_DPlay.SendHdr( PACKETTYPE_REVIVAL_TO_LODESTAR );

				CWndBase* pWndBase = g_WndMng.GetWndBase( APP_RESURRECTION );
				
				if( pWndBase )
					g_DPlay.SendResurrectionCancel();

				break;
			}
		case WIDC_REVIVAL_STAND:
			{
				g_DPlay.SendHdr( PACKETTYPE_REVIVAL );

				CWndBase* pWndBase = g_WndMng.GetWndBase( APP_RESURRECTION );
				
				if( pWndBase )
					g_DPlay.SendResurrectionCancel();

				break;
			}
#if __VER >= 9 // __S_9_ADD
		case WIDC_REVIVAL_SHOP:
			{
				bClose = FALSE;
				g_WndMng.ObjectExecutor( SHORTCUT_APPLET, APP_WEBBOX );
				break;
			}
#endif // __S_9_ADD
	}	
	if( bClose )
		Destroy();
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}



///////////////////////////////////////////////////////////////////////////
// CWantedMessageBox
//////////////////////////////////////////////////////////////////////////////
class CWantedMessageBox : public CWndMessageBox
{ 
public: 
	int m_nGold;
	CString m_strMsg;

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 

BOOL CWantedMessageBox::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	return CWndMessageBox::Initialize( prj.GetText(TID_PK_INPUT_TIMEWARN), 
	                                   pWndParent, 
									   MB_OKCANCEL );

}

BOOL CWantedMessageBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
 	switch( nID )
	{
	case IDOK:
		{
			g_DPlay.SendWantedGold( m_nGold, m_strMsg );
			Destroy();
		}
		break;
	case IDCANCEL:
		Destroy();
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


///////////////////////////////////////////////////////////////////////////
// CWndReWanted
//////////////////////////////////////////////////////////////////////////////
CWndReWanted::CWndReWanted() 
{ 
} 

CWndReWanted::~CWndReWanted() 
{ 
} 
void CWndReWanted::OnDraw( C2DRender* p2DRender ) 
{ 
} 

void CWndReWanted::SetWantedName( LPCTSTR szWanted )
{
	m_strWanted = szWanted;
}

void CWndReWanted::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	pWndEdit->SetFocus();	

	GetDlgItem( WIDC_STATIC_TARGETNAME )->SetTitle( m_strWanted );

	MoveParentCenter();	// 윈도를 중앙으로 옮기는 부분.
} 

BOOL CWndReWanted::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	InitDialog( g_Neuz.GetSafeHwnd(), APP_REWARD_INPUT );
	return TRUE;
} 

void CWndReWanted::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndReWanted::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndReWanted::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndReWanted::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
}

BOOL CWndReWanted::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BUTTON1 || message == EN_RETURN )
	{
		CWndEdit* pWndGold = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
		CWndEdit* pWndMsg  = (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
		if( pWndGold && pWndMsg && g_pPlayer )
		{
			if( strlen(pWndGold->GetString()) <= 0 )
				return FALSE;
			CString strMsg = pWndMsg->GetString();
			int nGold = atoi( pWndGold->GetString() ); 
			
			g_WndMng.WordChange( strMsg );

			if( CheckWantedInfo( nGold, strMsg ) == TRUE )
			{
				CWantedMessageBox* pBox = new CWantedMessageBox;
				pBox->m_nGold = nGold;
				pBox->m_strMsg = strMsg;
				g_WndMng.OpenCustomBox( "", pBox );
				Destroy();
			}
			else
				return FALSE;
		}

		Destroy();
	}
	else if( nID == WIDC_BUTTON2 )
	{
		Destroy();
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

BOOL CWndReWanted::CheckWantedInfo( int nGold, LPCTSTR szMsg )
{
	if( szMsg[0] == '\0' )
	{
		g_WndMng.OpenMessageBoxUpper( prj.GetText(TID_PK_REPU_INPUT) );
		return FALSE;
	}

	if( strlen(szMsg) > WANTED_MSG_MAX )
	{
		g_WndMng.OpenMessageBoxUpper( prj.GetText(TID_GUILD_NOTICE_ERROR) );
		return FALSE;
	}

	if( nGold < MIN_INPUT_REWARD || nGold > MAX_INPUT_REWARD )			// 현상금은 최소 1000패냐에서 최대 2억 패냐까지 걸 수 있다. 
	{
		char szWarning[256];
//		wsprintf( szWarning, "입력한 금액이 범위(%d - %d)를 벗어났습니다.", MIN_INPUT_REWARD, MAX_INPUT_REWARD );		
		wsprintf( szWarning, prj.GetText(TID_PK_MONEY_RANGE), MIN_INPUT_REWARD, MAX_INPUT_REWARD );		
		g_WndMng.OpenMessageBoxUpper( szWarning );
		return FALSE;
	}

	int nTax = MulDiv( nGold, 10, 100 );					// 건 현상금의 10%는 수수료로 지급된다. 
	if( g_pPlayer->GetGold() < (nGold + nTax) ) 
	{
		g_WndMng.OpenMessageBoxUpper( prj.GetText(TID_GAME_LACKMONEY) );	// 인벤에 돈이부족
		return FALSE;
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////
// CWndWantedConfirm
////////////////////////////////////////////////////////////////////////////////////////
CWndWantedConfirm::CWndWantedConfirm() 
{ 
	m_nGold = 0;
	memset( m_szName, 0, sizeof(m_szName) );
} 
CWndWantedConfirm::~CWndWantedConfirm() 
{ 
} 
void CWndWantedConfirm::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndWantedConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 
BOOL CWndWantedConfirm::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	InitDialog( g_Neuz.GetSafeHwnd(), APP_WANTED_CONFIRM, WBS_MODAL );
	
	return TRUE;
} 
BOOL CWndWantedConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndWantedConfirm::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndWantedConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndWantedConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
void CWndWantedConfirm::SetInfo(const char szName[], const int nGold)
{
	m_nGold  = nGold;
	_tcscpy( m_szName, szName );
	
	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
	pWndStatic->SetTitle(szName);

	pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC2 );

	CString strMsg1, strMsg2;

	strMsg1 = prj.GetText( TID_PK_POINT_SHOW );
	strMsg2.Format( prj.GetText(TID_PK_WASTE_SHOW), REQ_WANTED_GOLD );

	pWndStatic->SetTitle( strMsg1 + strMsg2 );	

	 
}
BOOL CWndWantedConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
		case  WIDC_BUTTON1:
			{
				if( strlen(m_szName) > 0 )
				{
					g_DPlay.SendWantedInfo(m_szName);
				}
				Destroy();
			}
			break;
		case  WIDC_BUTTON2:
			{
				Destroy();
			}
			break;
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

////////////////////////////////////////////////////////////////////////////////////////
// CWndWanted
////////////////////////////////////////////////////////////////////////////////////////

const int MAX_WANTED_PER_PAGE = 20;

CWndWanted::CWndWanted() 
{ 
	Init( 0 );
} 

CWndWanted::~CWndWanted() 
{ 
	SAFE_DELETE(m_pWantedConfirm);
} 

void CWndWanted::Init( time_t lTime )
{
	memset( m_aList, 0, sizeof(m_aList) );
	m_recvTime = lTime;
	m_pWantedConfirm  = NULL;
	m_nMax = 0;
	m_nSelect = -1;
}

void CWndWanted::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CRect rect = GetWindowRect();
	rect.top    += 53;
	rect.bottom -= 10;
	rect.left    = rect.right - 30;
	rect.right   -= 10;

	if( m_nMax > 0 )
	{
		m_wndScrollBar.SetScrollRange( 0, m_nMax );
		if( m_nMax < MAX_WANTED_PER_PAGE )
			m_wndScrollBar.SetScrollPage( m_nMax );
		else
			m_wndScrollBar.SetScrollPage( MAX_WANTED_PER_PAGE );
	}
	else
	{
		m_wndScrollBar.SetScrollRange( 0, 1 );
		m_wndScrollBar.SetScrollPage( 1 );
	}

	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );

	CString strTitle = GetTitle();
	CTime tm( m_recvTime );
//	strTitle += tm.Format( " - %H시 %M분 %S초의 데이타입니다." );
	strTitle += tm.Format( prj.GetText(TID_PK_DATA_SHOWTIME) );
	SetTitle( strTitle );

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 

BOOL CWndWanted::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_WANTED, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndWanted::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndWanted::OnSize( UINT nType, int cx, int cy )
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndWanted::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

// 선택된 인덱스를 얻는다.
int CWndWanted::GetSelectIndex( const CPoint& point )
{
	int nBase = m_wndScrollBar.GetScrollPos();
	int nIndex = (point.y - 32) / 18;

	if( 0 <= nIndex && nIndex < MAX_WANTED_PER_PAGE ) // 0 - 19사이 
	{
		int nSelect = nBase + nIndex;
		if( 0 <= nSelect && nSelect < m_nMax )
			return nSelect;
	}
	return -1;
}

void CWndWanted::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
	if( m_nMax <= 0 )
		return;

	int nSelect = GetSelectIndex( point );
	if( nSelect != -1 )
		m_nSelect = nSelect;
} 

void CWndWanted::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	// 현재 더블클릭된 좌표가 선택된 인덱스인가?
	if( m_nSelect >= 0 && GetSelectIndex( point ) == m_nSelect )	
	{
		SAFE_DELETE(m_pWantedConfirm);
		m_pWantedConfirm = new CWndWantedConfirm;
		m_pWantedConfirm->Initialize( this, 0 );

		// 표시할 이름, 현상금정보 전달
		m_pWantedConfirm->SetInfo( m_aList[m_nSelect].szName, (int)( m_aList[m_nSelect].nGold ) );
	}
}

void CWndWanted::OnRButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

void CWndWanted::OnRButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndWanted::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndWanted::OnMouseMove(UINT nFlags, CPoint point )
{
}

void CWndWanted::OnMouseWndSurface( CPoint point )
{
}	

BOOL CWndWanted::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	return TRUE;
}

void CWndWanted::InsertWanted( const char szName[], __int64 nGold, int nTime, const char szMsg[] )
{
	if( m_nMax >= MAX_WANTED_LIST )
	{
		Error( "CWndWanted::InsertWanted - range over" );
		return;
	}

	_tcscpy( m_aList[m_nMax].szName, szName );
	m_aList[m_nMax].nGold = nGold;

	CTime cTime( (time_t)nTime );		
	SYSTEMTIME st;
	st.wYear= cTime.GetYear(); 
	st.wMonth= cTime.GetMonth(); 
	st.wDay= cTime.GetDay();  

	if( GetDateFormat( LOCALE_USER_DEFAULT, 0, &st, NULL, m_aList[m_nMax].szDate, sizeof(m_aList[m_nMax].szDate) ) == 0 )
		m_aList[m_nMax].szDate[0] = '\0';

	_tcscpy( m_aList[m_nMax].szMsg, szMsg );
	m_nMax++;
}

void CWndWanted::OnDraw( C2DRender* p2DRender ) 
{ 
	const DWORD dwColor = D3DCOLOR_XRGB(0, 0, 0);
	int	sx, sy;
	char szNum[8], szGold[16];
	CString strGold;

	sx = 8;
	sy = 35;	

	CRect rc( sx, 5, sx+570, 7 ); 	
	p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 128, 0 , 0 , 0 ) );

	rc += CPoint( 0, 20 );
	p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 128, 0 , 0 , 0 ) );

	int nBase = m_wndScrollBar.GetScrollPos();
	if( nBase < 0 )
	{
		nBase = 0;
		TRACE("aa\n");
	}

	for( int i=nBase; i<nBase + MAX_WANTED_PER_PAGE; ++i )
	{
		if( i >= m_nMax )	
			break;
		
		sprintf( szNum, "%3d", i+1 );
		sprintf( szGold, "%I64d", m_aList[i].nGold );
		strGold = GetNumberFormatEx( szGold );

		if( m_nSelect >= 0 && i == m_nSelect )
		{
			rc.SetRect( sx, sy - 4, sx+570, sy + 16 ); 	
			p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 64, 0 , 0 , 0 ) );
		}

		p2DRender->TextOut( sx + 4,    sy, szNum,             dwColor );
		p2DRender->TextOut( sx + 33,   sy, m_aList[i].szName, dwColor );
		p2DRender->TextOut( sx + 190,  sy, strGold,           dwColor );
		p2DRender->TextOut( sx + 268,  sy, m_aList[i].szDate, dwColor );
		p2DRender->TextOut( sx + 350,  sy, m_aList[i].szMsg,  dwColor );

		sy += 18;
	}
} 

///////////////////////////////////////////////////////////////////////////////////////////
// CWndResurrectionConfirm
///////////////////////////////////////////////////////////////////////////////////////////
CWndResurrectionConfirm::CWndResurrectionConfirm() 
{ 
} 
CWndResurrectionConfirm::~CWndResurrectionConfirm() 
{ 
} 
void CWndResurrectionConfirm::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndResurrectionConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	m_wndTitleBar.SetVisible( FALSE );

	// 게이지 위쪽으로 나오게 하기
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( ( rectRoot.right - rectWindow.Width() ) / 2, 70 );
	Move( point );
} 
BOOL CWndResurrectionConfirm::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	InitDialog( g_Neuz.GetSafeHwnd(), APP_RESURRECTION, WBS_MODAL );
	return TRUE;
} 
BOOL CWndResurrectionConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndResurrectionConfirm::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndResurrectionConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndResurrectionConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndResurrectionConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case  WIDC_BUTTON1:
		{
			g_DPlay.SendResurrectionOK();
			Destroy();
		}
		break;
	case  10000:
	case  WIDC_BUTTON2:
		{
			g_DPlay.SendResurrectionCancel();
			Destroy();
		}
		break;
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}


/****************************************************
  WndId : APP_COMMITEM_DIALOG - 아이템 사용
  CtrlId : WIDC_EDIT_COMMUSE - 
  CtrlId : WIDC_BUTTON_OK - Button
****************************************************/

CWndCommItemDlg::CWndCommItemDlg() 
{ 
	m_pWndEdit = NULL;
	m_dwObjId = NULL_ID;
	m_dwCtrlId = NULL_ID;
} 
CWndCommItemDlg::~CWndCommItemDlg() 
{ 
} 
void CWndCommItemDlg::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndCommItemDlg::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	m_pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT_COMMUSE );
	if( m_pWndEdit )
		m_pWndEdit->EnableWindow( FALSE );

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
}
void CWndCommItemDlg::SetItem( DWORD dwDefindText, DWORD dwObjId, DWORD dwCtrlId )
{
	m_pWndEdit->AddString( prj.GetText( dwDefindText ) );
	m_dwObjId = dwObjId;
	m_dwCtrlId = dwCtrlId;
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndCommItemDlg::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_COMMITEM_DIALOG, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndCommItemDlg::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndCommItemDlg::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndCommItemDlg::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndCommItemDlg::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndCommItemDlg::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BUTTON_OK || message == EN_RETURN )
	{
		if( g_WndMng.GetWndBase( APP_SHOP_ )  ||
			g_WndMng.GetWndBase( APP_BANK )  ||
			g_WndMng.GetWndBase( APP_TRADE ) )
		{
			g_WndMng.PutString( prj.GetText(TID_GAME_TRADELIMITUSING), NULL, prj.GetTextColor(TID_GAME_TRADELIMITUSING) );
		}
		else
		if( g_WndMng.GetWndBase( APP_REPAIR ) )
		{
			g_WndMng.PutString( prj.GetText(TID_GAME_REPAIR_NOTUSE), NULL, prj.GetTextColor(TID_GAME_TRADELIMITUSING) );
		}
		else
		{
			if( m_dwObjId == II_SYS_SYS_SCR_CHACLA )
			{
				if( g_pPlayer->IsExpert() )
				{
					SAFE_DELETE( g_WndMng.m_pWndChangeClass1 );
					g_WndMng.m_pWndChangeClass1 = new CWndChangeClass1;
					g_WndMng.m_pWndChangeClass1->Initialize( &g_WndMng, APP_CHANGECLASS_1 );
				}
				else
				{
					SAFE_DELETE( g_WndMng.m_pWndChangeClass2 );
					g_WndMng.m_pWndChangeClass2 = new CWndChangeClass2;
					g_WndMng.m_pWndChangeClass2->Initialize( &g_WndMng, APP_CHANGECLASS_2 );
				}
			}
#if __VER >= 9 // __CSC_VER9_1
			else if( m_dwCtrlId == II_SYS_SYS_SCR_PET_TAMER_MISTAKE )
			{
				g_DPlay.SendPetTamerMistake(m_dwObjId);
			}
#endif //__CSC_VER9_1
			else
			{
#ifdef __AZRIA_1023
				CItemElem* pItem	= static_cast<CItemElem*>( g_pPlayer->GetItemId( m_dwObjId ) );
				if( pItem )
				{
					if( pItem->GetProp()->dwItemKind3 == IK3_TICKET )	// +
						g_DPlay.SendDoUseItemInput( MAKELONG( ITYPE_ITEM, m_dwObjId ), "0" );
					else	
						g_DPlay.SendDoUseItem( MAKELONG( ITYPE_ITEM, m_dwObjId ), m_dwCtrlId, -1, FALSE );
				}
#else	// __AZRIA_1023
				g_DPlay.SendDoUseItem( MAKELONG( ITYPE_ITEM, m_dwObjId ), m_dwCtrlId, -1, FALSE );
#endif	// __AZRIA_1023
			}
		}
		Destroy();
	}
	else if( nID == WIDC_BUTTON_CANCEL || nID == WTBID_CLOSE )
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

/****************************************************
WndId : APP_CHANGECLASS_1 - 직업변경(1차)
CtrlId : WIDC_RADIO_MER - 머서너리
CtrlId : WIDC_RADIO_ACR - 아크로뱃
CtrlId : WIDC_RADIO_MAG - 매지션
CtrlId : WIDC_RADIO_ASS - 어시스트
CtrlId : WIDC_STATIC1 - 변경을 원하는 직업을 선택하십시요
CtrlId : WIDC_STATIC2 - 직업 목록
CtrlId : WIDC_BUTTON_OK - Button
CtrlId : WIDC_BUTTON_CANCEL - 
****************************************************/

CWndChangeClass1::CWndChangeClass1() 
{ 
} 
CWndChangeClass1::~CWndChangeClass1() 
{ 
} 
void CWndChangeClass1::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndChangeClass1::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	nJob = g_pPlayer->GetJob();
	SetJob();

	CWndButton* pWndButton;
	int nJobBuf = nJob - 1;
	if( nJobBuf == 0 )
		pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_MER );
	else if( nJobBuf == 1 )
		pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_ACR );
	else if( nJobBuf == 2 )
		pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_ASS );
	else if( nJobBuf == 3 )
		pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_MAG );

	pWndButton->EnableWindow( FALSE );
	
	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 

void CWndChangeClass1::SetJob()
{
	CWndButton* pWndButton;

	BOOL bJob[MAX_EXPERT - MAX_JOBBASE];
	memset( bJob, 0, sizeof( bJob ) );
	bJob[nJob - 1] = TRUE;
 
	pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_MER );
	pWndButton->SetCheck( bJob[0] );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_ACR );
	pWndButton->SetCheck( bJob[1] );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_ASS );
	pWndButton->SetCheck( bJob[2] );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_MAG );
	pWndButton->SetCheck( bJob[3] );
}
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndChangeClass1::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_CHANGECLASS_1, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndChangeClass1::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndChangeClass1::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndChangeClass1::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndChangeClass1::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndChangeClass1::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BUTTON_OK || message == EN_RETURN )
	{
		if( g_pPlayer->GetJob() == nJob )
		{
			g_WndMng.PutString( prj.GetText(TID_GAME_EQUALJOB), NULL, prj.GetTextColor(TID_GAME_EQUALJOB) );
		}
		else
		{
			g_DPlay.SendChangeJob( nJob, FALSE );
			Destroy();
		}
	}
	else if( nID == WIDC_BUTTON_CANCEL || nID == WTBID_CLOSE )
	{
		Destroy();
	}
	else if( nID == WIDC_RADIO_MER )
	{
		nJob = JOB_MERCENARY;
		SetJob();
	}
	else if( nID == WIDC_RADIO_ACR )
	{
		nJob = JOB_ACROBAT;
		SetJob();
	}
	else if( nID == WIDC_RADIO_ASS )
	{
		nJob = JOB_ASSIST;
		SetJob();
	}
	else if( nID == WIDC_RADIO_MAG )
	{
		nJob = JOB_MAGICIAN;
		SetJob();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

/****************************************************
WndId : APP_CHANGECLASS_2 - 직업변경(2차)
CtrlId : WIDC_STATIC1 - 변경을 원하는 직업을 선택하십시요
CtrlId : WIDC_STATIC2 - 직업 목록
CtrlId : WIDC_RADIO_KNI - 나이트
CtrlId : WIDC_RADIO_BLA - 블레이드
CtrlId : WIDC_RADIO_RIN - 링마스터
CtrlId : WIDC_RADIO_PSY - 사이키퍼
CtrlId : WIDC_RADIO_ELE - 엘리멘터
CtrlId : WIDC_RADIO_JES - 제스터
CtrlId : WIDC_RADIO_RAN - 레인저
CtrlId : WIDC_RADIO_BIL - 빌포스터
CtrlId : WIDC_BUTTON_OK2 - Button
CtrlId : WIDC_BUTTON_CANCEL2 - Button
****************************************************/

CWndChangeClass2::CWndChangeClass2() 
{ 
} 
CWndChangeClass2::~CWndChangeClass2() 
{ 
} 
void CWndChangeClass2::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndChangeClass2::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	nJob = g_pPlayer->GetJob();
	SetJob();

	CWndButton* pWndButton;
	int nJobBuf = nJob - 6;
	if( ( nJobBuf ) == 0 )
		pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_KNI );
	else if( nJobBuf == 1 )
		pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_BLA );
	else if( nJobBuf == 2 )
		pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_JES );
	else if( nJobBuf == 3 )
		pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_RAN );
	else if( nJobBuf == 4 )
		pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_RIN );
	else if( nJobBuf == 5 )
		pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_BIL );
	else if( nJobBuf == 6 )
		pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_PSY );
	else if( nJobBuf == 7 )
		pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_ELE );	
	
	pWndButton->EnableWindow( FALSE );
	
	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 
void CWndChangeClass2::SetJob()
{
	CWndButton* pWndButton;

	BOOL bJob[MAX_PROFESSIONAL - MAX_EXPERT];
	memset( bJob, 0, sizeof( bJob ) );
	bJob[nJob - 6] = TRUE;

	pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_KNI );
	pWndButton->SetCheck( bJob[0] );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_BLA );
	pWndButton->SetCheck( bJob[1] );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_JES );
	pWndButton->SetCheck( bJob[2] );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_RAN );
	pWndButton->SetCheck( bJob[3] );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_RIN );
	pWndButton->SetCheck( bJob[4] );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_BIL );
	pWndButton->SetCheck( bJob[5] );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_PSY );
	pWndButton->SetCheck( bJob[6] );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_ELE );
	pWndButton->SetCheck( bJob[7] );	
}
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndChangeClass2::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_CHANGECLASS_2, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndChangeClass2::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndChangeClass2::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndChangeClass2::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndChangeClass2::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndChangeClass2::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BUTTON_OK2 || message == EN_RETURN )
	{
		if( g_pPlayer->GetJob() == nJob )
		{
			g_WndMng.PutString( prj.GetText(TID_GAME_EQUALJOB), NULL, prj.GetTextColor(TID_GAME_EQUALJOB) );
		}
		else
		{
			g_DPlay.SendChangeJob( nJob, FALSE );
			Destroy();
		}
	}
	else if( nID == WIDC_BUTTON_CANCEL2 || nID == WTBID_CLOSE )
	{
		Destroy();
	}
	else if( nID == WIDC_RADIO_KNI )
	{
		nJob = JOB_KNIGHT;
		SetJob();
	}
	else if( nID == WIDC_RADIO_BLA )
	{
		nJob = JOB_BLADE;
		SetJob();
	}
	else if( nID == WIDC_RADIO_JES )
	{
		nJob = JOB_JESTER;
		SetJob();
	}
	else if( nID == WIDC_RADIO_RAN )
	{
		nJob = JOB_RANGER;
		SetJob();
	}
	else if( nID == WIDC_RADIO_RIN )
	{
		nJob = JOB_RINGMASTER;
		SetJob();
	}
	else if( nID == WIDC_RADIO_BIL )
	{
		nJob = JOB_BILLPOSTER;
		SetJob();
	}
	else if( nID == WIDC_RADIO_PSY )
	{
		nJob = JOB_PSYCHIKEEPER;
		SetJob();
	}
	else if( nID == WIDC_RADIO_ELE )
	{
		nJob = JOB_ELEMENTOR;
		SetJob();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


void CWndInventory::RunUpgrade( CItemBase* pItem )
{
	if( m_bIsUpgradeMode )
	{
		m_bIsUpgradeMode = FALSE;

		if( pItem == NULL || m_pUpgradeMaterialItem == NULL )
		{
			g_WndMng.PutString( prj.GetText( TID_UPGRADE_CANCLE ), NULL, prj.GetTextColor( TID_UPGRADE_CANCLE ) );
			BaseMouseCursor();
			return;
		}
		
		ItemProp* pItemProp = m_pUpgradeMaterialItem->GetProp();

		if( !pItemProp )
			return;

#if __VER >= 12 // __EXT_PIERCING
		if( pItemProp->dwItemKind3 == IK3_SOCKETCARD || pItemProp->dwItemKind3 == IK3_SOCKETCARD2 )
		{
			if( !((CItemElem*)pItem)->IsPierceAble( pItemProp->dwItemKind3 ) )
#else // __EXT_PIERCING
		if( pItemProp->dwItemKind3 == IK3_SOCKETCARD )
		{
			if( pItem->GetProp()->dwItemKind3 != IK3_SUIT )
#endif // __EXT_PIERCING
			{
				g_WndMng.PutString( prj.GetText( TID_PIERCING_POSSIBLE_ITEM ) );
				BaseMouseCursor();
				return;
			}
			
			int nCount = 0;
			for( int j = 0; j < ( (CItemElem*)pItem )->GetPiercingSize(); j++ )
			{
				if( ( (CItemElem*)pItem )->GetPiercingItem( j ) != 0 )
					nCount++;
			}

			// 빈곳이 없으면 중단
			if( nCount == ( (CItemElem*)pItem )->GetPiercingSize() )
			{
				g_WndMng.PutString( prj.GetText(TID_PIERCING_ERROR_NOPIERCING), NULL, prj.GetTextColor(TID_PIERCING_ERROR_NOPIERCING));
				BaseMouseCursor();
				return;
			}
		}
		else
		if( pItemProp->dwItemKind3 == IK3_ELECARD )
		{
			if( !CItemElem::IsEleRefineryAble( pItem->GetProp()) )
			{
				g_WndMng.PutString( prj.GetText( TID_GAME_NOTEQUALITEM ), NULL, prj.GetTextColor( TID_GAME_NOTEQUALITEM ) );
				BaseMouseCursor();
				return;
			}

			CItemElem* pItemElem = (CItemElem*)pItem;

			if( pItemElem->m_bItemResist != SAI79::NO_PROP && pItemElem->m_bItemResist != pItemProp->eItemType )
			{
				g_WndMng.PutString( prj.GetText( TID_UPGRADE_ERROR_TWOELEMENT ), NULL, prj.GetTextColor( TID_UPGRADE_ERROR_TWOELEMENT ) );
				BaseMouseCursor();
				return;
			}

			DWORD dwWantedCard	= 0;
			DWORD dwItemAO	= pItemElem->m_nResistAbilityOption;

#if __VER >= 12 // __J12_0
			switch( pItemProp->eItemType )
			{
			case SAI79::FIRE:
				dwWantedCard	= II_GEN_MAT_ELE_FLAME;	break;
			case SAI79::WATER:
				dwWantedCard	= II_GEN_MAT_ELE_RIVER;	break;
			case SAI79::ELECTRICITY:
				dwWantedCard	= II_GEN_MAT_ELE_GENERATOR;	break;
			case SAI79::EARTH:
				dwWantedCard	= II_GEN_MAT_ELE_DESERT;	break;
			case SAI79::WIND:
				dwWantedCard	= II_GEN_MAT_ELE_CYCLON; break;
			default:
				dwWantedCard	= 0;	break;
			}
#else	// __J12_0
			switch( pItemProp->eItemType )
			{
			case SAI79::FIRE:
				dwWantedCard = ( dwItemAO <= 4 ) ? II_GEN_MAT_ELE_TOUCH: II_GEN_MAT_ELE_FLAME; break;
			case SAI79::WATER:
				dwWantedCard = ( dwItemAO <= 4 ) ? II_GEN_MAT_ELE_LAKE: II_GEN_MAT_ELE_RIVER; break;
			case SAI79::ELECTRICITY:
				dwWantedCard = ( dwItemAO <= 4 ) ? II_GEN_MAT_ELE_VOLTAGE: II_GEN_MAT_ELE_GENERATOR; break;
			case SAI79::EARTH:
				dwWantedCard = ( dwItemAO <= 4 ) ? II_GEN_MAT_ELE_STONE: II_GEN_MAT_ELE_DESERT; break;
			case SAI79::WIND:
				dwWantedCard = ( dwItemAO <= 4 ) ? II_GEN_MAT_ELE_GALE: II_GEN_MAT_ELE_CYCLON; break;
			default:
				dwWantedCard = 0; break;
			}
#endif	// __J12_0

			if( pItemProp->dwID != dwWantedCard )
			{
				g_WndMng.PutString( prj.GetText( TID_UPGRADE_ERROR_WRONGUPLEVEL ), NULL, prj.GetTextColor( TID_UPGRADE_ERROR_WRONGUPLEVEL ) );
				BaseMouseCursor();
				return;					
			}
		}
		else
#if __VER >= 8 //__Y_NEW_ENCHANT
		if( pItemProp->dwItemKind3 == IK3_ENCHANT )
#else //__Y_NEW_ENCHANT
		if( pItemProp->dwItemKind3 == IK3_DICE )
#endif //__Y_NEW_ENCHANT
		{
#if __VER >= 9 // __CSC_VER9_1 //오리칼쿰, 문스톤 얼터멋 웨폰 제련 관련 변경.
			if(m_pWndRemoveJewelConfirm != NULL)
			{
				if(m_pWndRemoveJewelConfirm->m_pUpgradeItem->m_dwObjId == pItem->m_dwObjId)
				{
					g_WndMng.PutString( prj.GetText( TID_GAME_NOTEQUALITEM ), NULL, prj.GetTextColor( TID_GAME_NOTEQUALITEM ) );
					BaseMouseCursor();
					return;
				}
			}
			
			if(m_pUpgradeMaterialItem->m_dwItemId == II_GEN_MAT_ORICHALCUM02)
			{
				CWndSmeltJewel* pWndSmeltJewel = (CWndSmeltJewel*)g_WndMng.GetWndBase( APP_SMELT_JEWEL );
				if(pWndSmeltJewel != NULL)
				{
					if(pWndSmeltJewel->m_pItemElem->m_dwObjId == pItem->m_dwObjId)
					{
						g_WndMng.PutString( prj.GetText( TID_GAME_NOTEQUALITEM ), NULL, prj.GetTextColor( TID_GAME_NOTEQUALITEM ) );
						BaseMouseCursor();
						return;
					}
				}
			
				if(pItem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE)
				{
					g_WndMng.PutString( prj.GetText( TID_GAME_NOTEQUALITEM ), NULL, prj.GetTextColor( TID_GAME_NOTEQUALITEM ) );
					BaseMouseCursor();
					return;
				}
			}
			else if( !CItemElem::IsDiceRefineryAble( pItem->GetProp()) )				
#else
			if( !CItemElem::IsDiceRefineryAble( pItem->GetProp()) )
#endif //__CSC_VER9_1
			{
				g_WndMng.PutString( prj.GetText( TID_GAME_NOTEQUALITEM ), NULL, prj.GetTextColor( TID_GAME_NOTEQUALITEM ) );
				BaseMouseCursor();
				return;
			}

			CItemElem* pItemElem = (CItemElem*)pItem;

		#if __VER >= 8 //__Y_NEW_ENCHANT
		#else //__Y_NEW_ENCHANT
			// 주사위일 경우 조건검사
			if( pItemProp->dwID != II_GEN_MAT_DIE_TWELVE )
			{
				if( pItemElem->GetAbilityOption() <= 4 )
				{
					if( pItemProp->dwID != II_GEN_MAT_DIE_FOUR )
					{
						BaseMouseCursor();
						g_WndMng.PutString( prj.GetText( TID_UPGRADE_ERROR_WRONGUPLEVEL ), NULL, prj.GetTextColor( TID_UPGRADE_ERROR_WRONGUPLEVEL ) );			
						return;					
					}
				}
				else
				{
					if( pItemProp->dwID != II_GEN_MAT_DIE_SIX )
					{
						BaseMouseCursor();
						g_WndMng.PutString( prj.GetText( TID_UPGRADE_ERROR_WRONGUPLEVEL ), NULL, prj.GetTextColor( TID_UPGRADE_ERROR_WRONGUPLEVEL ) );			
						return;					
					}
				}
			}
		#endif //__Y_NEW_ENCHANT
		}
		else
		if( pItemProp->dwItemKind3 == IK3_RANDOM_SCROLL )
		{
			if( !( pItem->GetProp()->dwItemKind1 == IK1_WEAPON || pItem->GetProp()->dwItemKind2 == IK2_ARMOR || pItem->GetProp()->dwItemKind2 == IK2_ARMORETC ) )
			{
				BaseMouseCursor();
				g_WndMng.PutString( prj.GetText( TID_GAME_RANDOMSCROLL_ERROR ), NULL, prj.GetTextColor( TID_GAME_RANDOMSCROLL_ERROR ) );			
				return;	
			}
		}
#if __VER >= 11
		else if( IsNeedTarget( pItemProp ) )
		{
			m_pUpgradeItem	= pItem;
			m_dwEnchantWaitTime		= g_tmCurrent + SEC(4);
			return;
		}
#endif	// __VER
#if __VER >= 9 // __CSC_VER9_1 //오리칼쿰, 문스톤 얼터멋 웨폰 제련 관련 변경.
		else
		if( pItemProp->dwItemKind3 == IK3_PIERDICE )
		{
			if(m_pUpgradeMaterialItem->m_dwItemId == II_GEN_MAT_MOONSTONE || m_pUpgradeMaterialItem->m_dwItemId == II_GEN_MAT_MOONSTONE_1)
			{
#if __VER >= 11 // __SYS_COLLECTING
				if( ( static_cast<CItemElem*>( pItem ) )->IsCollector( TRUE ) || pItem->GetProp()->dwItemKind2 == IK2_JEWELRY )
				{
					m_pUpgradeItem = pItem;
					m_dwEnchantWaitTime = g_tmCurrent + SEC(4);
					return;
				}
#endif	// __SYS_COLLECTING

#if __VER >= 12 // __CSC_VER12_4 //창으로 변경
				g_WndMng.PutString( prj.GetText( TID_GAME_NOTEQUALITEM ), NULL, prj.GetTextColor( TID_GAME_NOTEQUALITEM ) );
				BaseMouseCursor();
				return;			
#endif //__CSC_VER12_4

				BOOL checkJewel = FALSE;
					
				for(int i=0; i<5; i++)
				{
#if __VER >= 12 // __EXT_PIERCING
					if(((CItemElem*)pItem)->GetUltimatePiercingItem( i ) != 0)
#else // __EXT_PIERCING
					if(((CItemElem*)pItem)->GetPiercingItem( i ) != 0)
#endif // __EXT_PIERCING
						checkJewel = TRUE;
				}
				CWndSmeltJewel* pWndSmeltJewel = (CWndSmeltJewel*)g_WndMng.GetWndBase( APP_SMELT_JEWEL );
				if(pWndSmeltJewel != NULL)
				{
					if(pWndSmeltJewel->m_pItemElem && pWndSmeltJewel->m_pItemElem->m_dwObjId == pItem->m_dwObjId)
					{
						g_WndMng.PutString( prj.GetText( TID_GAME_NOTEQUALITEM ), NULL, prj.GetTextColor( TID_GAME_NOTEQUALITEM ) );
						BaseMouseCursor();
						return;
					}
				}
				if(m_pWndRemoveJewelConfirm != NULL)
				{
					if(m_pWndRemoveJewelConfirm->m_pUpgradeItem->m_dwObjId == pItem->m_dwObjId)
					{
						g_WndMng.PutString( prj.GetText( TID_GAME_NOTEQUALITEM ), NULL, prj.GetTextColor( TID_GAME_NOTEQUALITEM ) );
						BaseMouseCursor();
						return;
					}
				}
				if( pItem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE )
				{
					g_WndMng.PutString( prj.GetText( TID_GAME_NOTEQUALITEM ), NULL, prj.GetTextColor( TID_GAME_NOTEQUALITEM ) );
					BaseMouseCursor();
					return;
				}
				else
				{
					if(!checkJewel)
					{
						g_WndMng.PutString( prj.GetText( TID_GAME_NOTEQUALITEM ), NULL, prj.GetTextColor( TID_GAME_NOTEQUALITEM ) );
						BaseMouseCursor();
						return;
					}
				}
				
				if(m_pWndRemoveJewelConfirm == NULL)
				{
					m_pWndRemoveJewelConfirm = new CWndRemoveJewelConfirm;
					m_pWndRemoveJewelConfirm->Initialize(this);
					m_pWndRemoveJewelConfirm->SetItem(pItem);

					m_bRemoveJewel = TRUE;
					return;
				}
			}
		}
#endif // __CSC_VER9_1 
		// 인첸트가 되는 아이템 - 방어구 등등
		m_pUpgradeItem = pItem;
		m_dwEnchantWaitTime = g_tmCurrent + SEC(4);
	}
}

void CWndInventory::BaseMouseCursor()
{
	m_bIsUpgradeMode = FALSE;

	if( m_pSfxUpgrade )
	{
		m_pSfxUpgrade->Delete();
		m_pSfxUpgrade = NULL;
	}
}

void CWndInventory::UpdateParts()
{
	CMover::UpdateParts( g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh,g_pPlayer->m_aEquipInfo, m_pModel, &g_pPlayer->m_Inventory );
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 스킬 재분배 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndReSkillControl::CWndReSkillControl() 
{ 
} 
CWndReSkillControl::~CWndReSkillControl() 
{ 
} 
void CWndReSkillControl::OnDraw( C2DRender* p2DRender ) 
{ 
	
} 
void CWndReSkillControl::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	
	pWndText->m_string.AddParsingString( prj.GetText( TID_RESKILLPOINT_CONTROL1 )  );
	m_wndTitleBar.SetVisible( FALSE );	
	pWndText->ResetString();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndReSkillControl::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_RESKILL_CONTROL1, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndReSkillControl::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndReSkillControl::OnSize( UINT nType, int cx, int cy )
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndReSkillControl::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndReSkillControl::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndReSkillControl::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BUTTON1 )
	{
#ifdef __NEW_SKILL_TREE
		CWndSkillTreeEx* pSkillTree = (CWndSkillTreeEx*)g_WndMng.GetWndBase( APP_SKILL4 );
#else // __NEW_SKILL_TREE
#if __VER >= 10 // __CSC_VER9_1
		CWndSkillTreeEx* pSkillTree = (CWndSkillTreeEx*)g_WndMng.GetWndBase( APP_SKILL3 );
#else
		CWndSkillTreeEx* pSkillTree = (CWndSkillTreeEx*)g_WndMng.GetWndBase( APP_SKILL1 );
#endif //__CSC_VER9_1
#endif // __NEW_SKILL_TREE	
		if( pSkillTree )
			pSkillTree->InitItem(g_pPlayer->GetJob(), g_pPlayer->m_aJobSkill, TRUE );
	}
	else if( nID == WIDC_BUTTON2 )
	{
		SAFE_DELETE(g_WndMng.m_pWndReSkillWarning);
		g_WndMng.m_pWndReSkillWarning = new CWndReSkillWarning;
		g_WndMng.m_pWndReSkillWarning->Initialize(NULL);
	}
	else if( nID == 10000 )
	{
		return TRUE;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 



CWndReSkillWarning::CWndReSkillWarning() 
{ 
} 
CWndReSkillWarning::~CWndReSkillWarning() 
{ 
	Destroy();	
} 
void CWndReSkillWarning::InitItem( BOOL bParentDestroy )
{
	m_bParentDestroy = bParentDestroy;
}
void CWndReSkillWarning::OnDestroy()
{
	if( m_bParentDestroy )
	{
#ifdef __NEW_SKILL_TREE
		CWndBase* pWndBase1 = (CWndBase*)g_WndMng.GetWndBase( APP_SKILL4 );
#else // __NEW_SKILL_TREE
#if __VER >= 10 // __CSC_VER9_1
		CWndBase* pWndBase1 = (CWndBase*)g_WndMng.GetWndBase( APP_SKILL3 );
#else
		CWndBase* pWndBase1 = (CWndBase*)g_WndMng.GetWndBase( APP_SKILL1 );
#endif //__CSC_VER9_1
#endif // __NEW_SKILL_TREE
		pWndBase1->Destroy();
	}
}
void CWndReSkillWarning::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndReSkillWarning::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	CWndEdit* pWndEdit	= (CWndEdit*)GetDlgItem( WIDC_CONTEXT );
	
	if( pWndEdit )
	{
		pWndEdit->SetString( _T( prj.GetText( TID_GAME_SKILLLEVEL_CHANGE ) ) );
		pWndEdit->EnableWindow( FALSE );	
	}

	m_bParentDestroy = FALSE;

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 
BOOL CWndReSkillWarning::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_QUEITMWARNING, WBS_MODAL, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndReSkillWarning::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndReSkillWarning::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndReSkillWarning::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndReSkillWarning::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndReSkillWarning::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BTN_YES || message == EN_RETURN )
	{
#ifdef __NEW_SKILL_TREE
		CWndSkillTreeEx* pSkillTree = (CWndSkillTreeEx*)g_WndMng.GetWndBase( APP_SKILL4 );
#else // __NEW_SKILL_TREE
#if __VER >= 10 // __CSC_VER9_1
		CWndSkillTreeEx* pSkillTree = (CWndSkillTreeEx*)g_WndMng.GetWndBase( APP_SKILL3 );
#else
		CWndSkillTreeEx* pSkillTree = (CWndSkillTreeEx*)g_WndMng.GetWndBase( APP_SKILL1 );
#endif //__CSC_VER9_1
#endif // __NEW_SKILL_TREE	
		if( pSkillTree )
			g_DPlay.SendDoUseSkillPoint( pSkillTree->GetSkill() );

		Destroy();
	}
	else if( nID == WIDC_BTN_NO )
	{
		Destroy();
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


BOOL CWndSkillTreeEx::IsReSkillPointDone()
{
	if( m_apSkill == NULL )
		return FALSE;
	
	if( m_nCurrSkillPoint < g_pPlayer->m_nSkillPoint )
		return TRUE;

	return FALSE;	
}

CWndPostItemWarning::CWndPostItemWarning() 
{ 
	m_nMailIndex = -1;
} 
CWndPostItemWarning::~CWndPostItemWarning() 
{ 
} 
void CWndPostItemWarning::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndPostItemWarning::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 

void CWndPostItemWarning::SetString( char* string )
{
	CWndEdit* pWndEdit	= (CWndEdit*)GetDlgItem( WIDC_CONTEXT );
	
	if( pWndEdit )
	{
		pWndEdit->SetString( string );
		pWndEdit->EnableWindow( FALSE );	
	}
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndPostItemWarning::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	LPWNDAPPLET lpWndApplet = m_resMng.GetAt ( APP_QUEITMWARNING );
	CRect rect = CRect( 0, 0, lpWndApplet->size.cx, lpWndApplet->size.cy );
	
	return CWndNeuz::Create( WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_MODAL, rect, pWndParent, APP_QUEITMWARNING ); 
} 
BOOL CWndPostItemWarning::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndPostItemWarning::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndPostItemWarning::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndPostItemWarning::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndPostItemWarning::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BTN_YES || message == EN_RETURN )
	{
		g_DPlay.SendQueryGetMailItem( m_nMailIndex );					
		Destroy();
	}
	else if( nID == WIDC_BTN_NO )
	{
		Destroy();
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 






CWndPost::CWndPost() 
{ 
#ifdef __MAIL_REQUESTING_BOX
	m_pWndMailRequestingBox = NULL;
#endif // __MAIL_REQUESTING_BOX
} 
CWndPost::~CWndPost() 
{ 
#ifdef __MAIL_REQUESTING_BOX
	//SaveLastMailBox();
	CloseMailRequestingBox();
#endif // __MAIL_REQUESTING_BOX
} 
void CWndPost::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndPost::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	EnableWindow( FALSE );

	//	여기에 코딩하면 됩니다
	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	CRect rect = GetClientRect();
	rect.left = 5;
	rect.top = 0;

#if __VER >= 11 // __SYS_POCKET
	if(GetWndBase( APP_BAG_EX )) GetWndBase( APP_BAG_EX )->Destroy();
#endif

	m_PostTabSend.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_POST_SEND );
	m_PostTabReceive.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_POST_RECEIVE );

	WTCITEM tabTabItem;
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;

	tabTabItem.pszText = prj.GetText(TID_APP_POST_SEND);
	tabTabItem.pWndBase = &m_PostTabSend;
	pWndTabCtrl->InsertItem( 0, &tabTabItem );

	tabTabItem.pszText = prj.GetText(TID_APP_POST_RECEIVE);
	tabTabItem.pWndBase = &m_PostTabReceive;
	pWndTabCtrl->InsertItem( 1, &tabTabItem );

	pWndTabCtrl->SetCurSel(1);

	MoveParentCenter();	

	// 도착한 편지 목록 요청
	g_DPlay.SendQueryMailBox();	

#ifdef __MAIL_REQUESTING_BOX
	//CMailBox* pMailBox = CMailBox::GetInstance();
	//if( pMailBox )
	//{
	//	pMailBox->Clear();
	//}
	//LoadLastMailBox();
	//m_PostTabReceive.UpdateScroll();

	CloseMailRequestingBox();
	m_pWndMailRequestingBox = new CWndMailRequestingBox;
	m_pWndMailRequestingBox->Initialize();
#endif // __MAIL_REQUESTING_BOX
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndPost::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndPost::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndPost::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndPost::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndPost::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndPost::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( message == WNM_SELCHANGE )
	{
		CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );

		if( pWndTabCtrl->GetCurSel() == 0 )
		{
		}
		else
		{
#ifndef __MAIL_REQUESTING_BOX
			m_PostTabSend.ClearData();
#endif // __MAIL_REQUESTING_BOX
			g_DPlay.SendQueryMailBox();
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

#ifdef __MAIL_REQUESTING_BOX
void CWndPost::LoadLastMailBox( void )
{
	CString strFileName = _T( "" );
	if( g_pPlayer == NULL )
	{
		return;
	}
	strFileName.Format( "%s_MailData.Temp", g_pPlayer->GetName() );
	FILE* fp = fopen( strFileName, "rb" );
	if( fp == NULL )
	{
		Error( "LoadLastMailBox : fp == NULL" );
		return;
	}

	while( feof( fp ) == 0 )
	{
		CMail* pMail = new CMail;
		fread( &( pMail->m_nGold ), sizeof( DWORD ), 1, fp );
		fread( &( pMail->m_byRead ), sizeof( BYTE ), 1, fp );
		fread( &( pMail->m_szTitle ), sizeof( char ), MAX_MAILTITLE, fp );
		CMailBox::GetInstance()->AddMail( pMail );
	}

	fclose( fp );
}

void CWndPost::SaveLastMailBox( void )
{
	CMailBox* pMailBox = CMailBox::GetInstance();
	if( pMailBox == NULL )
	{
		return;
	}

	CMailBox& MailBox = *pMailBox;
	if( MailBox.empty() == true )
	{
		CString strFileName = _T( "" );
		if( g_pPlayer == NULL )
		{
			return;
		}
		strFileName.Format( "%s_MailData.Temp", g_pPlayer->GetName() );
		DeleteFile( strFileName );

		return;
	}

	CString strFileName = _T( "" );
	if( g_pPlayer == NULL )
	{
		return;
	}
	strFileName.Format( "%s_MailData.Temp", g_pPlayer->GetName() );
	FILE* fp = fopen( strFileName.GetBuffer(0), "wb" );
	if( fp == NULL )
	{
		Error( "SaveLastMailBox : fp == NULL" );
		return;
	}

	for( size_t i = 0; i < MailBox.size(); ++i )
	{
		fwrite( &( MailBox[ i ]->m_nGold ), sizeof( DWORD ), 1, fp );
		fwrite( &( MailBox[ i ]->m_byRead ), sizeof( BYTE ), 1, fp );
		fwrite( &( MailBox[ i ]->m_szTitle ), sizeof( char ), MAX_MAILTITLE, fp );
	}

	fclose( fp );
}
void CWndPost::CloseMailRequestingBox( void )
{
	if( m_pWndMailRequestingBox )
	{
		SAFE_DELETE( m_pWndMailRequestingBox );
	}
}
#endif // __MAIL_REQUESTING_BOX


///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
// CWndPostSendMessageBox
//////////////////////////////////////////////////////////////////////////////
class CWndPostSendMessageBox : public CWndMessageBox
{ 
public: 
	CWndText	m_wndText;
	
	BYTE		m_nItem;
	short		m_nItemNum;
	int			m_nGold;
	char		m_szReceiver[MAX_NAME];
	char		m_szTitle[MAX_MAILTITLE];	
	char		m_szText[MAX_MAILTEXT];		
	void		SetValue( BYTE nItem, short nItemNum, char* lpszReceiver, int nGold, char* lpszTitle, char* lpszText );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 

BOOL CWndPostSendMessageBox::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CString str;
	str.Format( prj.GetText(TID_MAIL_SEND_CONFIRM), MAX_KEEP_MAX_DAY );
	
	return CWndMessageBox::Initialize( str, 
		pWndParent, 
		MB_OKCANCEL );	
}

void CWndPostSendMessageBox::SetValue( BYTE nItem, short nItemNum, char* lpszReceiver, int nGold, char* lpszTitle, char* lpszText )
{
	m_nItem		= nItem;
	m_nItemNum	= nItemNum;
	strcpy( m_szReceiver, lpszReceiver );
	strcpy( m_szTitle, lpszTitle );
	strcpy( m_szText, lpszText );
	m_nGold		= nGold;
}

BOOL CWndPostSendMessageBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( nID == 1 )
	{
		if( g_pPlayer )
		{
			g_DPlay.SendQueryPostMail( m_nItem, m_nItemNum, m_szReceiver, m_nGold, m_szTitle, m_szText );			
			
			CWndPost * pWndPost = (CWndPost *)g_WndMng.GetWndBase( APP_POST );
			
			if( pWndPost )
			{
				pWndPost->m_PostTabSend.ClearData();
			}
		}
		
		Destroy();
	}
	else if( nID == 2 )
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 



CWndPostSend::CWndPostSend():m_nItem(0xff), m_nCost(0), m_nCount(0)
{ 
//	m_pWndPostSendConfirm = NULL;
} 
CWndPostSend::~CWndPostSend() 
{ 
	ClearData();
//	SAFE_DELETE(m_pWndPostSendConfirm);	
} 

void CWndPostSend::ClearData()
{
	CItemElem* pItemElem;
	pItemElem  = (CItemElem*)g_pPlayer->m_Inventory.GetAtId( m_nItem );
	
	if(pItemElem)
	{
		// 트레이드에 걸린 아이템은 무시
		if( !g_pPlayer->m_vtInfo.IsTrading( pItemElem ) )
			pItemElem->SetExtra( 0 );
	}
	
	m_nItem		= 0xff;		
	m_nCost		= 0;
	m_nCount	= 0;
	/*
	CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
	CWndEdit* pWndEdit3	= (CWndEdit*)GetDlgItem( WIDC_EDIT3 );
	CWndEdit* pWndEdit4	= (CWndEdit*)GetDlgItem( WIDC_EDIT4 );
	
	pWndEdit2->SetString("");
	pWndEdit3->SetString("");
	pWndEdit4->SetString("");
	*/
}

void CWndPostSend::SetReceive( char* pchar )
{
	CWndComboBox* pWndCombo = (CWndComboBox*)GetDlgItem( WIDC_COMBOBOX1 );	
	pWndCombo->SetString( pchar );
}
void CWndPostSend::SetTitle( char* pchar )
{
	CWndEdit* pWndEdit1	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
	
	pWndEdit1->SetString( pchar );
}
void CWndPostSend::SetText( char* pchar )
{
	CWndEdit* pWndEdit1	= (CWndEdit*)GetDlgItem( WIDC_EDIT3 );
	
	pWndEdit1->SetString( pchar );
}
void CWndPostSend::OnDraw( C2DRender* p2DRender ) 
{ 
	if( m_nItem != 0xff )
	{
		CItemElem* pItemElem;
		pItemElem  = (CItemElem*)g_pPlayer->m_Inventory.GetAtId( m_nItem );
		
		if( pItemElem )
		{
			LPWNDCTRL pCustom = NULL;
			pCustom = GetWndCtrl( WIDC_STATIC3 );
			
			pItemElem->GetTexture()->Render( p2DRender, pCustom->rect.TopLeft(), 255 );

			if( pItemElem->GetProp()->dwPackMax > 1 && pItemElem->GetExtra() > 0 )
			{
				CD3DFont* pOldFont = p2DRender->GetFont(); 
				p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndNewTitle ); 
				
				TCHAR szTemp[32];
				_stprintf( szTemp, "%d", pItemElem->GetExtra() );
				p2DRender->TextOut( pCustom->rect.right-13,  pCustom->rect.bottom-13 , szTemp, 0xff00220b );
				p2DRender->TextOut( pCustom->rect.right-14,  pCustom->rect.bottom-14 , szTemp, 0xff00ffff );

				p2DRender->SetFont( pOldFont );				
			}	
			
			//*
			CRect hitrect = pCustom->rect;
			CPoint point = GetMousePoint();
			if( hitrect.PtInRect( point ) )
			{
				CPoint point2 = point;
				ClientToScreen( &point2 );
				ClientToScreen( &hitrect );
				
				g_WndMng.PutToolTip_Item( (CItemBase*)pItemElem, point2, &hitrect );
			}
			/**/
		}
	}
} 
void CWndPostSend::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndEdit* pNotice = (CWndEdit*) GetDlgItem( WIDC_EDIT3 );
	pNotice->AddWndStyle( EBS_WANTRETURN );
	pNotice->AddWndStyle( EBS_AUTOVSCROLL );

	// 아래는 가라로 막은것이다. CWndEdit의 SetWndRect에서 스크롤바의 visible상태에 따라 셋팅하고 나서
	// 부모클래스인 CWndText의 OnInitialUpdate 호출되어 스크롤바의 visible상태를 설정한다. 그래서 SetWndRect를 재호출....ㅡㅡ;;;; 길드공지도 동일하게 막았다.
	// 정상동작하는 메신저 메세지 창의 경우 OnSize에서 SetWndRect를 호출해서 정상적으로 동작했던 것.
	LPWNDCTRL pWndCtrl = GetWndCtrl( WIDC_EDIT3 );
	if(pWndCtrl)
		pNotice->SetWndRect(pWndCtrl->rect);

	CWndComboBox* pWndCombo = (CWndComboBox*)GetDlgItem( WIDC_COMBOBOX1 );
	CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
	CWndEdit* pWndEdit3	= (CWndEdit*)GetDlgItem( WIDC_EDIT3 );
	
	pWndCombo->SetTabStop( TRUE );
	pWndEdit2->SetTabStop( TRUE );
	pWndEdit3->SetTabStop( TRUE );

#ifdef __RT_1025
	for( map<u_long, Friend>::iterator i	= g_WndMng.m_RTMessenger.begin(); i != g_WndMng.m_RTMessenger.end(); ++i )
	{
		u_long idPlayer	= i->first;
		Friend* pFriend	= &i->second;
//		if( pFriend )
		{
#else	// __RT_1025
	C2FriendPtr::iterator iter = g_WndMng.m_Messenger.m_aFriend.begin();
	for( ; iter != g_WndMng.m_Messenger.m_aFriend.end() ; ++iter )
	{
		LPFRIEND lpFriend = (LPFRIEND)iter->second;
		if( lpFriend )
		{
			u_long idPlayer	= lpFriend->dwUserId;
#endif	// __RT_1025

#if __VER >= 11 // __SYS_PLAYER_DATA
			pWndCombo->AddString( CPlayerDataCenter::GetInstance()->GetPlayerString( idPlayer ) );
#else	// __SYS_PLAYER_DATA
			pWndCombo->AddString( lpFriend->szName );
#endif	// __SYS_PLAYER_DATA
		}
	}

	if( ::GetLanguage() != LANG_KOR )
	{
		CWndStatic* pStatic2	= (CWndStatic *)GetDlgItem( WIDC_STATIC2 );
		CRect rect	= pStatic2->GetWndRect();
		rect.left	-= 14;
		rect.right	-= 14;
		pStatic2->SetWndRect( rect, TRUE );

		CWndEdit* pEdit4	= (CWndEdit*)GetDlgItem( WIDC_EDIT4 );
		rect	= pEdit4->GetWndRect();
		rect.left	-= 14;
		rect.right	+= 10;
		pEdit4->SetWndRect( rect, TRUE );
	}

	pWndCombo->SetFocus();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndPostSend::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_POST_SEND, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndPostSend::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndPostSend::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndPostSend::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndPostSend::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

void CWndPostSend::SetItemId( BYTE nId )
{ 
	// 기존 아이템이 있었으면 확장 데이타를 초기화 한다.
	if( nId != m_nItem && m_nItem != 0xff )
	{
		CItemElem* pItemElem  = (CItemElem*)g_pPlayer->m_Inventory.GetAtId( m_nItem );
		if( pItemElem )
		{
			pItemElem->SetExtra( 0 );				
		}
	}
	
	m_nItem = nId; 
}

BOOL CWndPostSend::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CWndBase* pWndFrame =  pShortcut->m_pFromWnd->GetFrameWnd();
	
	// 아이템이 인벤토리에서 왔는가?
	if( !(pShortcut->m_dwShortcut == SHORTCUT_ITEM) && !(pWndFrame->GetWndId() == APP_INVENTORY) )
		return FALSE;

	if( pShortcut->m_dwData == 0 )
	{
		SetForbid( TRUE );		
		return FALSE;
	}
	
	if( g_pPlayer->m_Inventory.IsEquip( pShortcut->m_dwId ) )
	{
		g_WndMng.PutString( prj.GetText(TID_GAME_EQUIPPUT), NULL, prj.GetTextColor(TID_GAME_EQUIPPUT) );
		SetForbid( TRUE );
		return FALSE;
	}
	
	CItemElem* pItemElem;
	pItemElem  = (CItemElem*)g_pPlayer->m_Inventory.GetAt( pShortcut->m_dwIndex );
	
	if(pItemElem == NULL)
		return FALSE;

	LPWNDCTRL pCustom = NULL;
	pCustom = GetWndCtrl( WIDC_STATIC3 );
	
	// 아이템( 방어구, 무기구 )
	if( PtInRect(&(pCustom->rect), point) )
	{
		if( pItemElem->IsCharged() )//&& pItemElem->GetProp()->dwItemRare == 200 || pItemElem->GetProp()->dwItemRare == 300 )
		{
			g_WndMng.PutString( prj.GetText(TID_GAME_CHARGED_NOTUSE), NULL, prj.GetTextColor(TID_GAME_CHARGED_NOTUSE) );
			pItemElem = NULL;
			return FALSE;
		}
		
		if( pItemElem->m_nItemNum > 1 )
		{ 
			SetItemId( (BYTE)( pItemElem->m_dwObjId ) );

			g_WndMng.m_pWndTradeGold = new CWndTradeGold;
			memcpy( &g_WndMng.m_pWndTradeGold->m_Shortcut, pShortcut, sizeof(SHORTCUT) );
			g_WndMng.m_pWndTradeGold->m_dwGold = pItemElem->m_nItemNum;
			g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_POST_SEND; // 인벤토리 인데 쓰고 있어서 Gold로 씀.
			g_WndMng.m_pWndTradeGold->m_pWndBase = this;
			g_WndMng.m_pWndTradeGold->m_nSlot = 0;
			
			g_WndMng.m_pWndTradeGold->Initialize( &g_WndMng, APP_TRADE_GOLD );
			g_WndMng.m_pWndTradeGold->AddWndStyle( WBS_MODAL );
			g_WndMng.m_pWndTradeGold->MoveParentCenter();

			CWndStatic* pStatic	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC );
			CWndStatic* pStaticCount	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_CONTROL1 );
			CString strMain = prj.GetText( TID_GAME_MOVECOUNT );//"몇개를 이동하시겠습니까?";
			CString strCount = prj.GetText(TID_GAME_NUMCOUNT);//" 갯수 : ";
			pStatic->m_strTitle = strMain;
			pStaticCount->m_strTitle = strCount;
			g_WndMng.m_pWndTradeGold->SetTitle("");

			if( pItemElem->GetProp() )
			{
				CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );

				if( stricmp( pWndEdit2->GetString(), pItemElem->GetProp()->szName ) != 0 )
					pWndEdit2->SetString(pItemElem->GetProp()->szName);
			}
		}
		else
		{
			SetItemId( (BYTE)( pItemElem->m_dwObjId ) );
			pItemElem->SetExtra( 1 );				

			if( pItemElem->GetProp() )
			{
				CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );

				if( stricmp( pWndEdit2->GetString(), pItemElem->GetProp()->szName ) != 0 )
					pWndEdit2->SetString(pItemElem->GetProp()->szName);
			}			
		}
	}		
			
	return TRUE;
}

void CWndPostSend::OnRButtonUp( UINT nFlags, CPoint point )
{
	LPWNDCTRL pCustom = NULL;
	pCustom = GetWndCtrl( WIDC_STATIC3 );
	
	// 아이템( 방어구, 무기구 )
	if( PtInRect(&(pCustom->rect), point) )
	{
		CItemElem* pItemElem;
		pItemElem  = (CItemElem*)g_pPlayer->m_Inventory.GetAtId( m_nItem );
		
		if(pItemElem)
		{
			pItemElem->SetExtra( 0 );				
		}

		m_nItem = 0xff;	
	}		
}

void CWndPostSend::AdditionalSkinTexture( LPWORD pDest, CSize sizeSurface, D3DFORMAT d3dFormat )
{
	CWndNeuz::AdditionalSkinTexture( pDest, sizeSurface, d3dFormat );
}


#define MAX_BYTE_POST		128

BOOL CWndPostSend::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case WIDC_SEND:	
		{
			CWndComboBox* pWndCombo = (CWndComboBox*)GetDlgItem( WIDC_COMBOBOX1 );
			CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
			CWndEdit* pWndEdit3	= (CWndEdit*)GetDlgItem( WIDC_EDIT3 );
			CWndEdit* pWndEdit4	= (CWndEdit*)GetDlgItem( WIDC_EDIT4 );
			
			LPCTSTR szstr1 = pWndCombo->GetString();
			LPCTSTR szstr2 = pWndEdit2->GetString();
			LPCTSTR szstr3 = pWndEdit3->GetString();
			LPCTSTR szstr4 = pWndEdit4->GetString();
			
			// 제목과 받는사람이 없으면 못보냄
			if( strlen( szstr1 ) <=0 || strlen( szstr2 ) <=0 )
			{
				return FALSE;
			}
			
			// 아이템과 페냐 두개중 하나가 없어도 못보냄
			if( m_nItem == NULL_ID && strlen( szstr4 ) <=0 )
			{
				return FALSE;
			}

			// 금액란에 숫자가 아니면 못보냄
			int nlen = strlen(szstr4);
			BOOL bisdigit = TRUE;

			if( nlen > 0 )
			{
				for( int i = 0 ; i < nlen ; i++ )
				{
					if( (isdigit2( szstr4[i] ) == FALSE) )
					{
						bisdigit = FALSE;
						break;
					}
				}
			}
			
			if( bisdigit == FALSE )
			{
				//g_WndMng.OpenMessageBoxUpper( prj.GetText(TID_GAME_GUILDONLYNUMBER) );
				return FALSE;
			}

			__int64 liGold	= _atoi64(szstr4);
			if( liGold > 1000000000 )
			{
				g_WndMng.OpenMessageBoxUpper( prj.GetText(TID_GAME_LIMITPENYA) );
				return FALSE;				
			}
			
			if( strlen(szstr1) < 3 || strlen(szstr1) > 16 )
			{
				//이름이 너무 깁니다. 다시 입력해주십시오.
				g_WndMng.OpenMessageBoxUpper( prj.GetText(TID_DIAG_0057) );
				return FALSE;				
			}

			if( strlen(szstr3) > ( MAX_MAILTEXT - 1 ) )		//gmpbigsun: 허용범위 오류수정 
			{
				CString str;
				str.Format( prj.GetText(TID_GAME_MAILBOX_TEXT_MAX), MAX_MAILTEXT );
				g_WndMng.OpenMessageBoxUpper( str );
				return FALSE;				
			}

			if( strlen(szstr2) > ( MAX_MAILTITLE - 1 ) )	//gmpbigsun: 허용범위 오류수정
			{
				CString str;
				str.Format( prj.GetText(TID_GAME_MAILBOX_TITLE_MAX), MAX_MAILTITLE );
				g_WndMng.OpenMessageBoxUpper( str );
				return FALSE;				
			}

			CWndPostSendMessageBox* pBox = new CWndPostSendMessageBox;

			if( pBox )
			{
				g_WndMng.OpenCustomBox( "", pBox );
				
				CItemElem* pItemElem;
				pItemElem  = (CItemElem*)g_pPlayer->m_Inventory.GetAtId( m_nItem );
				
				if( pItemElem )
				{
					pBox->SetValue( m_nItem, pItemElem->GetExtra(), (char*)szstr1, atoi(szstr4), (char*)szstr2, (char*)szstr3 );
				}
				else
				{
					pBox->SetValue( -1, 0, (char*)szstr1, atoi(szstr4), (char*)szstr2, (char*)szstr3 );
				}
			}
				
			break;
		}
	}
		
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


///////////////////////////////////////////////////////////////////////////////////////////////////////
CWndPostRead::CWndPostRead():m_nMailIndex(-1), m_bDrag(0), m_pWndPostItemWarning(0)
{ 
	m_pDeleteConfirm = NULL;
} 
CWndPostRead::~CWndPostRead() 
{ 
	ClearData();
//	SAFE_DELETE(m_pWndPostItemWarning);
} 

void CWndPostRead::ClearData()
{
	m_nMailIndex = -1;
	m_bDrag = FALSE;
}

void CWndPostRead::OnDraw( C2DRender* p2DRender ) 
{ 
	if( m_nMailIndex == -1 )
		return;

	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;
	
	if( !mailbox[m_nMailIndex] )
		return;
	
	CItemElem* m_pItemElem = mailbox[m_nMailIndex]->m_pItemElem;

	if( m_pItemElem && m_pItemElem->GetTexture() )
	{
		LPWNDCTRL pCustom = NULL;
		pCustom = GetWndCtrl( WIDC_STATIC3 );

		CRect hitrect = pCustom->rect;
		CPoint point = GetMousePoint();
		if( hitrect.PtInRect( point ) )
		{
			CPoint point2 = point;
			ClientToScreen( &point2 );
			ClientToScreen( &hitrect );
			
			p2DRender->RenderRoundRect( pCustom->rect, D3DCOLOR_XRGB( 150, 0, 0 ) );
			g_WndMng.PutToolTip_Item( (CItemBase*)m_pItemElem, point2, &hitrect );
		}

		m_pItemElem->GetTexture()->Render( p2DRender, pCustom->rect.TopLeft(), 255 );

		if( m_pItemElem->GetProp()->dwPackMax > 1 )
		{
			CD3DFont* pOldFont = p2DRender->GetFont(); 
			p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndNewTitle ); 
			
			TCHAR szTemp[32];
			_stprintf( szTemp, "%d", m_pItemElem->m_nItemNum  );
			CSize size	= p2DRender->m_pFont->GetTextExtent( szTemp );
			p2DRender->TextOut( pCustom->rect.right-13,  pCustom->rect.bottom-13 , szTemp, 0xff00220b );
			p2DRender->TextOut( pCustom->rect.right-14,  pCustom->rect.bottom-14 , szTemp, 0xff00ffff );

			p2DRender->SetFont( pOldFont );				
		}	
	}

	{
		LPWNDCTRL pCustom = NULL;
		pCustom = GetWndCtrl( WIDC_STATIC4 );
		CRect hitrect = pCustom->rect;
		CPoint point = GetMousePoint();
		if( hitrect.PtInRect( point ) && mailbox[m_nMailIndex]->m_nGold > 0 )
		{
			CPoint point2 = point;
			ClientToScreen( &point2 );
			ClientToScreen( &hitrect );
			
			p2DRender->RenderRoundRect( pCustom->rect, D3DCOLOR_XRGB( 150, 0, 0 ) );
		}
	}
} 
void CWndPostRead::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndEdit* pWndEdit1	= (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
	CWndEdit* pWndEdit3	= (CWndEdit*)GetDlgItem( WIDC_EDIT3 );
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );

	CRect rect	= pWndEdit3->GetWndRect();
	rect.right	+= 32;
	pWndEdit3->SetWndRect( rect, TRUE );

	CWndButton* pWndButton = (CWndButton*)GetDlgItem( WIDC_BUTTON3 );
	

	pWndEdit1->EnableWindow(FALSE);
	pWndEdit2->EnableWindow(FALSE);
	pWndEdit3->EnableWindow(FALSE);
//	pWndText->EnableWindow(FALSE);

	pWndButton->SetFocus();
	MoveParentCenter();
	
	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_STATIC4 );
	m_wndGold.Create( "g", WBS_NODRAWFRAME, lpWndCtrl->rect, this, WIDC_STATIC4 );
	m_wndGold.AddWndStyle( WBS_NODRAWFRAME );
	
} 

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndPostRead::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	LPWNDAPPLET lpWndApplet = m_resMng.GetAt ( APP_POST_READ );
	CRect rect = CRect( 0, 0, lpWndApplet->size.cx, lpWndApplet->size.cy );
	
	return CWndNeuz::Create( WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_MODAL, rect, pWndParent, APP_POST_READ ); 
} 

BOOL CWndPostRead::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndPostRead::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndPostRead::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
	m_bDrag = FALSE;
} 
void CWndPostRead::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
	if( m_nMailIndex == -1 )
		return;
	
	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;
	
	CMail* pMail = mailbox[m_nMailIndex];
	
	if( !pMail )
		return;	
	
	LPWNDCTRL pCustom = NULL;
	pCustom = GetWndCtrl( WIDC_STATIC3 );
	
	// 아이템( 방어구, 무기구 )
	if( PtInRect(&(pCustom->rect), point) )
	{
		m_bDrag = TRUE;
	}
} 
void CWndPostRead::SetValue( int nMailIndex )
{
	m_nMailIndex	= nMailIndex;

	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;

	CMail* pMail = mailbox[m_nMailIndex];
				
	if( pMail == NULL )
		return;

	CWndEdit* pWndEdit1	= (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
	CWndEdit* pWndEdit3	= (CWndEdit*)GetDlgItem( WIDC_EDIT3 );
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );

	TCHAR szCutTitle[128];

#if __VER >= 11 // __SYS_PLAYER_DATA
	LPCSTR lpszPlayerString		= CPlayerDataCenter::GetInstance()->GetPlayerString( pMail->m_idSender );
	char lpszPlayer[MAX_PLAYER]		= { 0,};
	if( pMail->m_idSender == 0 )
		lstrcpy( lpszPlayer, "FLYFF" );
	else
		lstrcpy( lpszPlayer, lpszPlayerString );
#else	// __SYS_PLAYER_DATA
	LPCSTR lpszPlayerString	= prj.GetPlayerString( pMail->m_idSender );
	char	lpszPlayer[MAX_PLAYER]	= { 0, };
	if( pMail->m_idSender == 0 )
		lstrcpy( lpszPlayer, "FLYFF" );
	else if( lpszPlayerString )
		lstrcpy( lpszPlayer, lpszPlayerString );
#endif	// __SYS_PLAYER_DATA

	memset( szCutTitle, 0, sizeof(szCutTitle) );
	GetStrCut( lpszPlayer, szCutTitle, 16 );
	
	if( GetStrLen( lpszPlayer ) > 16 )
		_tcscat( szCutTitle, "..." );
	
	if( lstrlen( lpszPlayer ) > 0 )
		pWndEdit1->SetString(szCutTitle);
	else
		pWndEdit1->SetString("Unknown");
	
	memset( szCutTitle, 0, sizeof(szCutTitle) );
	GetStrCut( pMail->m_szTitle, szCutTitle, 13 );
	
	if( GetStrLen( pMail->m_szTitle ) >= 13 )
	{
		_tcscat( szCutTitle, "..." );
	}
	
	if( lstrlen( lpszPlayer ) > 0 )
	{
		pWndEdit2->SetString(szCutTitle);
		pWndText->SetString(pMail->m_szText);
	}
	else
	{
		pWndEdit2->SetString( "" );
		pWndText->SetString( "" );
	}

	g_DPlay.SendQueryReadMail( pMail->m_nMail );

	char szbuffer[128] = {0};
	sprintf( szbuffer, "%d", pMail->m_nGold );
	pWndEdit3->SetString(GetNumberFormatEx(szbuffer));
}
BOOL CWndPostRead::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	return TRUE;
}

void CWndPostRead::OnMouseMove(UINT nFlags, CPoint point )
{
	if( m_bDrag )
	{
		if( m_nMailIndex == -1 )
			return;
		
		CMailBox* pMailBox	= CMailBox::GetInstance();	
		CMailBox& mailbox = *pMailBox;
		
		CMail* pMail = mailbox[m_nMailIndex];
		
		if( pMail && pMail->m_pItemElem && pMail->m_pItemElem->GetProp() )
		{
			m_GlobalShortcut.m_pFromWnd   = this;
			m_GlobalShortcut.m_dwShortcut = SHORTCUT_ITEM;
			m_GlobalShortcut.m_dwIndex    = 0xffffffff;
			m_GlobalShortcut.m_dwType     = ITYPE_ITEM;
			m_GlobalShortcut.m_dwId       = pMail->m_pItemElem->m_dwObjId;
			m_GlobalShortcut.m_pTexture   = pMail->m_pItemElem->GetTexture();
			m_GlobalShortcut.m_dwData     = (DWORD) pMail->m_pItemElem;
			_tcscpy( m_GlobalShortcut.m_szString, pMail->m_pItemElem->GetProp()->szName);
		}
	}	
}

void CWndPostRead::OnRButtonUp( UINT nFlags, CPoint point )
{
	if( m_nMailIndex == -1 )
		return;

	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;
	
	CMail* pMail = mailbox[m_nMailIndex];
	
	if( !pMail )
		return;	

	LPWNDCTRL pCustom = NULL;
	pCustom = GetWndCtrl( WIDC_STATIC3 );
	
	// 아이템( 방어구, 무기구 )
	if( PtInRect(&(pCustom->rect), point) )
	{
		MailReceiveItem();
	}
	
	pCustom = GetWndCtrl( WIDC_STATIC4 );

	if( PtInRect(&(pCustom->rect), point) )
	{
		MailReceiveGold();
	}
}
void CWndPostRead::MailReceiveItem()
{
	// 해당 아이템을 메일에서 갖고온다.
	if( m_nMailIndex == -1 )
		return;
	
	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;
	
	CMail* pMail = mailbox[m_nMailIndex];
				
	if( pMail && pMail->m_pItemElem )
	{
		// 기본 보관일수 지났는지를 검사하여 보관료 부과한다.
		int nDay = 0;
		DWORD dwTime = 0;
		pMail->GetMailInfo( &nDay, &dwTime );

		// 기본 보관일수가 지났다!!!
		if( (MAX_KEEP_MAX_DAY*24) - dwTime > (MAX_KEEP_BASIC_DAY*24) )
		{
			FLOAT fCustody = 0.0f;
			FLOAT fPay = 0.0f;
			fCustody = (FLOAT)( (FLOAT)( MAX_KEEP_MAX_DAY - MAX_KEEP_BASIC_DAY - nDay ) / (FLOAT)( MAX_KEEP_MAX_DAY - MAX_KEEP_BASIC_DAY ) );

			m_bDrag = FALSE;
//			SAFE_DELETE(m_pWndPostItemWarning);
			m_pWndPostItemWarning = new CWndPostItemWarning;
			m_pWndPostItemWarning->Initialize( this, 0 );
			m_pWndPostItemWarning->SetIndex( pMail->m_nMail );

			fPay = pMail->m_pItemElem->GetCost() * fCustody;
			if( fPay < 0.0f )
				fPay = 0.0f;

			TCHAR szChar[256] = { 0 };
			sprintf( szChar, prj.GetText(TID_GAME_MAILBOX_KEEP_PAY), MAX_KEEP_BASIC_DAY, (int)fPay );

			m_pWndPostItemWarning->SetString( szChar );
		}
		else
		{
			g_DPlay.SendQueryGetMailItem( pMail->m_nMail );								
		}
	}
}
void CWndPostRead::MailReceiveGold()
{
	// 해당 페냐를 메일에서 갖고온다.
	if( m_nMailIndex == -1 )
		return;
	
	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;
	
	CMail* pMail = mailbox[m_nMailIndex];
	
	if( pMail && pMail->m_nMail > 0 && pMail->m_nGold > 0 )
	{
		g_DPlay.SendQueryGetMailGold( pMail->m_nMail );		
		
		CWndEdit* pWndEdit	= (CWndEdit*)GetDlgItem( WIDC_EDIT3 );
		pWndEdit->SetString("0");
	}	
}
BOOL CWndPostRead::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;
	
	CMail* pMail = mailbox[m_nMailIndex];

	if( !pMail )
		return FALSE;
	
	CWndPost* pWndPost = (CWndPost*)g_WndMng.GetWndBase(APP_POST);

	if( pWndPost == NULL )
		return FALSE;

	switch( nID )
	{
	case WIDC_BUTTON1:	
		{
//			SAFE_DELETE(m_pDeleteConfirm);

#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
			if(pMail->m_pItemElem && pMail->m_pItemElem->m_dwItemId == II_SYS_SYS_SCR_SEALCHARACTER )
				return FALSE;
#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz

			m_pDeleteConfirm = new CWndPostDeleteConfirm;

			if( m_pDeleteConfirm )
			{
				m_pDeleteConfirm->Initialize(this);
				m_pDeleteConfirm->SetValue(m_nMailIndex);			
			}
		}
		break;
	case WIDC_BUTTON2:	
		{
			CWndEdit* pWndEdit1	= (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
			CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
			
			if( pWndEdit1 && pWndEdit2 )
			{
				if( stricmp( pWndEdit1->GetString(), "Unknow" ) == 0 )
				{
					g_WndMng.OpenMessageBoxUpper( prj.GetText(TID_MAIL_UNKNOW) );
					return FALSE;
				}

				pWndPost->m_PostTabSend.SetItemId(0xff);
				pWndPost->m_PostTabSend.SetReceive("");
				pWndPost->m_PostTabSend.SetText("");
				pWndPost->m_PostTabSend.SetTitle("");
				
				CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)pWndPost->GetDlgItem( WIDC_TABCTRL1 );
				pWndTabCtrl->SetCurSel(0);	

#if __VER >= 11 // __SYS_PLAYER_DATA
				LPCSTR lpszPlayerString		= CPlayerDataCenter::GetInstance()->GetPlayerString( pMail->m_idSender );
				char	lpszPlayer[MAX_PLAYER]	= { 0, };
				if( pMail->m_idSender == 0 )
					lstrcpy( lpszPlayer, "FLYFF" );
				else
					lstrcpy( lpszPlayer, lpszPlayerString );
#else	// __SYS_PLAYER_DATA
				LPCSTR lpszPlayerString	= prj.GetPlayerString( pMail->m_idSender );
				char	lpszPlayer[MAX_PLAYER]	= { 0, };
				if( pMail->m_idSender == 0 )
					lstrcpy( lpszPlayer, "FLYFF" );
				else if( lpszPlayerString )
					lstrcpy( lpszPlayer, lpszPlayerString );
#endif	// __SYS_PLAYER_DATA

				pWndPost->m_PostTabSend.SetReceive( (char*)lpszPlayer );

				CString str = "Re:";
				str += pWndEdit2->GetString();
				pWndPost->m_PostTabSend.SetTitle(str.GetBuffer(0));
				Destroy();
			}
		}
		break;
	case WIDC_BUTTON3:	
		{
			Destroy();
		}
		break;
	}
		
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

CWndPostDeleteConfirm::CWndPostDeleteConfirm():m_nIndex(0)
{
}

CWndPostDeleteConfirm::~CWndPostDeleteConfirm() 
{
	
}

void CWndPostDeleteConfirm::OnDraw( C2DRender* p2DRender ) 
{
}
void CWndPostDeleteConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	CRect rect	= GetClientRect();
	rect.left	+= 8;
	rect.right	-= 4;
	rect.top	+= 8;
	rect.bottom	/= 2;

	CWndText* pWndText = (CWndText*)(GetDlgItem( WIDC_TEXT1 ));
	pWndText->SetString(_T( prj.GetText(TID_MAIL_DELETE_CONFIRM) ));	
	
	MoveParentCenter();
} 

BOOL CWndPostDeleteConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	LPWNDAPPLET lpWndApplet = m_resMng.GetAt ( APP_POST_DELETE_CONFIRM );
	CRect rect = CRect( 0, 0, lpWndApplet->size.cx, lpWndApplet->size.cy );
	
	return CWndNeuz::Create( WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_MODAL, rect, pWndParent, APP_POST_DELETE_CONFIRM ); 
} 

BOOL CWndPostDeleteConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndPostDeleteConfirm::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndPostDeleteConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndPostDeleteConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndPostDeleteConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES )
	{
		CMailBox* pMailBox	= CMailBox::GetInstance();	
		CMailBox& mailbox = *pMailBox;
		
		CMail* pMail = mailbox[m_nIndex];
		
		if( pMail )
		{
			g_DPlay.SendQueryRemoveMail( pMail->m_nMail );						
			Destroy(TRUE);

			CWndPostRead* pWndPost = (CWndPostRead*)g_WndMng.GetWndBase(APP_POST_READ);
			if(pWndPost)
				pWndPost->Destroy();
			
		}
	}
	else if( nID == WIDC_NO )
	{
		Destroy( TRUE );
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}
///////////////////////////////////////////////////////////////////////////////////////////////////////

CWndPostReceive::CWndPostReceive():m_wndPostRead(NULL)
{ 
	m_nMax = 0;
	m_nSelect = -1;
} 
CWndPostReceive::~CWndPostReceive() 
{ 
#ifdef __FIX_WND_1109
	DeleteDeviceObjects();
#endif	// __FIX_WND_1109
} 
HRESULT CWndPostReceive::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();

	m_Texture[0].DeleteDeviceObjects();
	m_Texture[1].DeleteDeviceObjects();
	m_Texture[2].DeleteDeviceObjects();

#ifdef __FIX_WND_1109
	SAFE_DELETE( m_wndPostRead );
#endif	// __FIX_WND_1109

	return TRUE;
}
int CWndPostReceive::GetSelectIndex( const CPoint& point )
{
	int nBase = m_wndScrollBar.GetScrollPos();
	int nIndex = (point.y-23) / 40;
	
	if( 0 <= nIndex && nIndex < MAX_MAIL_LIST_PER_PAGE ) 
	{
		int nSelect = nBase + nIndex;
		if( 0 <= nSelect && nSelect < m_nMax )
			return nSelect;
	}
	return -1;
}
void CWndPostReceive::UpdateScroll()
{
	CMailBox* pMailBox	= CMailBox::GetInstance();
	m_nMax = pMailBox->size();
	
	if( m_nMax > 0 )
	{
		m_wndScrollBar.SetScrollRange( 0, m_nMax );
		if( m_nMax < MAX_MAIL_LIST_PER_PAGE )
			m_wndScrollBar.SetScrollPage( m_nMax );
		else
			m_wndScrollBar.SetScrollPage( MAX_MAIL_LIST_PER_PAGE );
	}
	else
	{
		m_wndScrollBar.SetScrollRange( 0, 1 );
		m_wndScrollBar.SetScrollPage( 1 );
	}	
}
void CWndPostReceive::OnDraw( C2DRender* p2DRender ) 
{ 
	const DWORD dwColor = D3DCOLOR_XRGB(0, 0, 0);
	int	sx, sy;
	
	sx = 9;
	sy = 30;
	
	CRect rc( sx, 5, sx+310, 7 ); 	
	rc += CPoint( 0, 20 );
	
	int nBase = m_wndScrollBar.GetScrollPos();
	if( nBase < 0 )
	{
		nBase = 0;
		TRACE("aa\n");
	}
	
	TCHAR szTemp[32];

	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;

	p2DRender->RenderTexture( CPoint(sx+4, sy - 4), &m_Texture[0], 150  );

	// 기간 색상 변화
	D3DXVECTOR2 v2_1 = D3DXVECTOR2( 255, 0 );
	D3DXVECTOR2 v2_2 = D3DXVECTOR2( 0, 150 );
	D3DXVECTOR2 v2Result;

	TCHAR szCutTitle[128];
	

	COleDateTime dt;
	CString strDateTime;

	for( int i=nBase; i<nBase + MAX_MAIL_LIST_PER_PAGE; ++i )
	{
		if( i >= m_nMax )	
			break;

		if( (int)( mailbox.size() ) <= i )
			continue;
			
		if( mailbox[i] == NULL ) 
			continue;

		if( m_nSelect >= 0 && i == m_nSelect )
		{
			rc.SetRect( sx+4, sy - 4, sx+240, sy + 36 ); 	
			p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 32, 190, 0 , 0 ) );
		}
		
		// 아이템이 있으면 출력
		if( mailbox[i]->m_pItemElem && mailbox[i]->m_pItemElem->m_pTexture )
		{
			p2DRender->RenderTexture( CPoint( sx + 10,  sy+2 ), mailbox[i]->m_pItemElem->m_pTexture );

			if( mailbox[i]->m_pItemElem->GetProp()->dwPackMax > 1 )
			{
				CD3DFont* pOldFont = p2DRender->GetFont(); 
				p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndNewTitle ); 
				
				_stprintf( szTemp, "%d", mailbox[i]->m_pItemElem->m_nItemNum );
				CSize size	= p2DRender->m_pFont->GetTextExtent( szTemp );
				p2DRender->TextOut( sx+27,  sy+20 , szTemp, 0xff00220b );
				p2DRender->TextOut( sx+27,  sy+20 , szTemp, 0xff00ffff );
				
				p2DRender->SetFont( pOldFont );			
			}
		}
		else
		// 아이템은 없고 돈만 있으면 돈모양 출력
		if( mailbox[i]->m_pItemElem == NULL && mailbox[i]->m_nGold > 0 )
		{
			p2DRender->RenderTexture( CPoint( sx + 10,  sy+2 ), &m_Texture[2] );
		}

		// 편지를 읽었다면 읽은 표시
		if( mailbox[i]->m_byRead )
		{
			p2DRender->RenderTexture( CPoint( sx+10, sy+1 ), &m_Texture[1], 110 );
		}		

#if __VER >= 11 // __SYS_PLAYER_DATA
		const char* lpszPlayerString	= CPlayerDataCenter::GetInstance()->GetPlayerString( mailbox[i]->m_idSender );
		char lpszPlayer[MAX_PLAYER]	= { 0, };
		if( mailbox[i]->m_idSender == 0 )
			lstrcpy( lpszPlayer, "FLYFF" );
		else
			lstrcpy( lpszPlayer, lpszPlayerString );
#else	// __SYS_PLAYER_DATA
		LPCSTR lpszPlayerString	= prj.GetPlayerString( mailbox[i]->m_idSender );
		char	lpszPlayer[MAX_PLAYER]	= { 0, };
		if( mailbox[i]->m_idSender == 0 )
			lstrcpy( lpszPlayer, "FLYFF" );
		else if( lpszPlayerString )
			lstrcpy( lpszPlayer, lpszPlayerString );
#endif	// __SYS_PLAYER_DATA
		
		memset( szCutTitle, 0, sizeof(szCutTitle) );
		GetStrCut( lpszPlayer, szCutTitle, 16 );
		
		if( GetStrLen( lpszPlayer ) > 16 )
			_tcscat( szCutTitle, "..." );

		if( lstrlen( lpszPlayer ) > 0 )
		{
			p2DRender->TextOut( sx + 60,  sy+3, szCutTitle ,  D3DCOLOR_XRGB( 255, 255, 255 ) );//adeilson D3DCOLOR_XRGB( 0, 0, 0 ) );
		}
		else
		{
			p2DRender->TextOut( sx + 60,  sy+3, "Unknown",  D3DCOLOR_XRGB( 255, 255, 255 ) );//adeilson D3DCOLOR_XRGB( 0, 0, 0 ) );
		}

		int nDay = 0;
		DWORD dwTime = 0;
		mailbox[i]->GetMailInfo( &nDay, &dwTime );  // 보관일자 검사

		CString szDay;

		// 하루 이하 남으면 시간으로 표시해준다.
		if( nDay <= 1 )
		{
			szDay.Format( prj.GetText(TID_PK_LIMIT_HOUR), dwTime );
		}
		else
		{
			szDay.Format( prj.GetText(TID_PK_LIMIT_DAY), nDay );
		}
		
		FLOAT f = (FLOAT)( (FLOAT)nDay / (FLOAT)MAX_KEEP_BASIC_DAY );
		D3DXVec2Lerp( &v2Result, &v2_1, &v2_2, f );

		if( nDay <= 0 )
			p2DRender->TextOut( sx + 190,  sy+3, szDay,  D3DCOLOR_XRGB( (int)v2Result.x, (int)v2Result.y, 0 ) );			
		else
			p2DRender->TextOut( sx + 193,  sy+3, szDay,  D3DCOLOR_XRGB( (int)v2Result.x, (int)v2Result.y, 0 ) );			
		
		memset( szCutTitle, 0, sizeof(szCutTitle) );
		GetStrCut( mailbox[i]->m_szTitle, szCutTitle, 13 );
		
		if( GetStrLen( mailbox[i]->m_szTitle ) >= 13 )
		{
			_tcscat( szCutTitle, "..." );
		}
		
//		if( GetFocusWnd() == this )
		{
			CRect  rect;
			rect.SetRect( sx+4, sy - 4, sx+240, sy + 36 ); 	
			CPoint point	= GetMousePoint();
			if( PtInRect( rect, point ) )
			{
				ClientToScreen( &point );
				ClientToScreen( &rect );
				dt = mailbox[i]->m_tmCreate;
				strDateTime.Format( prj.GetText( TID_GAME_MAIL_RECEIVE_DATE ), dt.Format() );
				g_toolTip.PutToolTip( (DWORD)this, strDateTime, rect, point );
			}		
		}
		
		if( lstrlen( lpszPlayer ) > 0 )
			p2DRender->TextOut( sx + 60,  sy+18, szCutTitle,  D3DCOLOR_XRGB( 0, 255, 255 ) );
		else
			p2DRender->TextOut( sx + 60,  sy+18, "",  D3DCOLOR_XRGB( 0, 255, 255 ) );

		sy += 40;
	}	
} 

void CWndPostReceive::OnMouseWndSurface( CPoint point )
{
	int	sx, sy;
	
	sx = 15;
	sy = 30;
	
	CRect rc( sx, 5, sx+310, 7 ); 	
	rc += CPoint( 0, 20 );
	
	int nBase = m_wndScrollBar.GetScrollPos();
	if( nBase < 0 )
	{
		nBase = 0;
		TRACE("aa\n");
	}
	
	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;

	COleDateTime dt;
	CString strDateTime;

	CPoint cpoint	= point;

	for( int i=nBase; i<nBase + MAX_MAIL_LIST_PER_PAGE; ++i )
	{
		if( i >= m_nMax )	
			break;

		if( (int)( mailbox.size() ) <= i )
			continue;
			
		if( mailbox[i] == NULL ) 
			continue;

//		if( GetFocusWnd() == this )
		{			
			rc.SetRect( sx+4, sy - 4, sx+240, sy + 36 ); 	
			if( PtInRect( rc, cpoint ) )
			{
				ClientToScreen( &cpoint );
				ClientToScreen( &rc );
				dt = mailbox[i]->m_tmCreate;
				strDateTime.Format( prj.GetText( TID_GAME_MAIL_RECEIVE_DATE ), dt.Format() );
				g_toolTip.PutToolTip( (DWORD)this, strDateTime, rc, cpoint );
				break;
			}		
		}
		
		sy += 40;
	}	
}

void CWndPostReceive::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	if( m_nMax <= 0 )
		return;

	int nSelect = GetSelectIndex( point );
	if( nSelect == -1 )
		return;
	
	m_nSelect = nSelect;	

	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;
	
	if( mailbox[m_nSelect] == NULL )
		return;

#ifdef __FIX_WND_1109
	SAFE_DELETE( m_wndPostRead );
#endif	// __FIX_WND_1109

	m_wndPostRead = new CWndPostRead;

	if( m_wndPostRead )
	{
#ifdef __FIX_WND_1109
		m_wndPostRead->Initialize( this );
#else	// __FIX_WND_1109
		m_wndPostRead->Initialize();
#endif	// __FIX_WND_1109
		m_wndPostRead->SetValue( m_nSelect );
	}
}

void CWndPostReceive::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CRect rect = GetWindowRect();
	rect.top    += 15;
	rect.bottom -= 50;
	rect.left    = rect.right - 40;
	rect.right   -= 30;
	
	if( m_nMax > 0 )
	{
		m_wndScrollBar.SetScrollRange( 0, m_nMax );
		if( m_nMax < MAX_MAIL_LIST_PER_PAGE )
			m_wndScrollBar.SetScrollPage( m_nMax );
		else
			m_wndScrollBar.SetScrollPage( MAX_MAIL_LIST_PER_PAGE );
	}
	else
	{
		m_wndScrollBar.SetScrollRange( 0, 1 );
		m_wndScrollBar.SetScrollPage( 1 );
	}
	
	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );
	
	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
	
	m_Texture[0].LoadTexture(g_Neuz.m_pd3dDevice, MakePath( "Theme\\Default\\", "WndPostTable.tga" ), 0xffff00ff );	
	m_Texture[1].LoadTexture(g_Neuz.m_pd3dDevice, MakePath( "Theme\\Default\\", "WndNotUse.tga" ), 0xffff00ff );	
	m_Texture[2].LoadTexture(g_Neuz.m_pd3dDevice, MakePath( "item\\", "itm_GolGolSeed.dds" ), 0xffff00ff );	
	
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndPostReceive::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_POST_RECEIVE, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndPostReceive::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndPostReceive::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndPostReceive::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndPostReceive::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
	if( m_nMax <= 0 )
		return;
	
	int nSelect = GetSelectIndex( point );
	if( nSelect != -1 )
		m_nSelect = nSelect;	
} 
BOOL CWndPostReceive::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

BOOL CWndPostReceive::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
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

///////////////////////////////////////////////////////////////////////////
// CWndGuildCombatOfferMessageBox
//////////////////////////////////////////////////////////////////////////////
class CWndGuildCombatOfferMessageBox : public CWndMessageBox
{ 
public: 
	DWORD m_nCost;
	void	SetValue( CString str, DWORD nCost );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 

BOOL CWndGuildCombatOfferMessageBox::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	return CWndMessageBox::Initialize( "", 
		pWndParent, 
		MB_OKCANCEL );	
}

void CWndGuildCombatOfferMessageBox::SetValue( CString str, DWORD nCost )
{
	m_wndText.SetString( str );
	m_nCost = nCost;	
}

BOOL CWndGuildCombatOfferMessageBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case IDOK:
		{
			g_DPlay.SendGuildCombatApp( m_nCost );
			SAFE_DELETE( g_WndMng.m_pWndGuildCombatOffer);
			Destroy();
		}
		break;
	case IDCANCEL:
		Destroy();
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 



// 길드 컴뱃 신청하기
CWndGuildCombatOffer::CWndGuildCombatOffer() 
{
	m_dwMinGold    = 0;
	m_dwBackupGold = 0;
	m_dwReqGold    = 0;
}

CWndGuildCombatOffer::~CWndGuildCombatOffer() 
{
	
}

void CWndGuildCombatOffer::OnDraw( C2DRender* p2DRender ) 
{	
}

void CWndGuildCombatOffer::EnableAccept( BOOL bFlag )
{
	CWndButton* pWndButton = (CWndButton*)GetDlgItem( WIDC_ACCEPT );

	if( pWndButton )
	{
		pWndButton->EnableWindow( bFlag );	
	}
}

void CWndGuildCombatOffer::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	CRect rect	= GetClientRect();
	rect.left	+= 8;
	rect.right	-= 4;
	rect.top	+= 8;
	rect.bottom	/= 2;

	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	pWndText->SetString( prj.GetText(TID_GAME_GUILDCOMBAT_PRESENT_TEXT) );
	pWndText->EnableWindow( TRUE );
	
	MoveParentCenter();
} 

BOOL CWndGuildCombatOffer::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_OFFER, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndGuildCombatOffer::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndGuildCombatOffer::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndGuildCombatOffer::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndGuildCombatOffer::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

void CWndGuildCombatOffer::SetGold( DWORD nCost )
{
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );

	if( pWndEdit )
	{
		CString str;
		str.Format( "%d", nCost );
		pWndEdit->SetString(str);
	}				
}

void CWndGuildCombatOffer::SetTotalGold( __int64 nCost )
{
	CWndStatic* pStatic	= (CWndStatic *)GetDlgItem( WIDC_STATIC6 );
	pStatic->AddWndStyle(WSS_MONEY);
	
	if( pStatic )
	{
		CString str;
		str.Format( "%I64d", nCost );
		pStatic->SetTitle(str);
	}
}

BOOL CWndGuildCombatOffer::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_ACCEPT )
	{
		if( g_pPlayer )
		{
			CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
			
			DWORD nCost;
			CString str = pWndEdit->GetString();
			nCost = atoi( str );

			if( m_dwReqGold != 0 )
			{
				if( nCost <= m_dwBackupGold )
				{
					g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT_MORE_CURRENT_REQUEST) ); //기존 페냐보다 더 많은 금액으로 신청을 하시기 바랍니다.
					return FALSE;
				}
				
			}
			else
			{
				if( nCost < m_dwMinGold )
				{
					g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT_LIMIT_MIN) ); //최소금액보다 더 많은 페냐로 신청하시기 바랍니다.
					return FALSE;
				}
			}


			CWndGuildCombatOfferMessageBox* pMsg = new CWndGuildCombatOfferMessageBox;
			if( pMsg )
			{
				g_WndMng.OpenCustomBox( "", pMsg, this );
				CString str;

				if( m_dwReqGold == 0 )
				{
					str.Format( prj.GetText(TID_GAME_GUILDCOMBAT_MORE_REQUEST), 0, nCost ); //기존에 신청된 %d페냐에서 추가로 %d페냐를 신청하겠습니까?
				}
				else
				{
					str.Format( prj.GetText(TID_GAME_GUILDCOMBAT_MORE_REQUEST), m_dwBackupGold, nCost-m_dwBackupGold ); //기존에 신청된 %d페냐에서 추가로 %d페냐를 신청하겠습니까?
				}

				pMsg->SetValue( str, nCost );
			}
		}
		
//		Destroy();
	}
	else if( nID == WIDC_CLOSE )
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

// 길드 정보출력
#if __VER >= 11 // __GUILD_COMBAT_1TO1
CWndGuildCombatBoard::CWndGuildCombatBoard(int nCombatType)
{
	m_nCombatType = nCombatType;
}
#else //__GUILD_COMBAT_1TO1
CWndGuildCombatBoard::CWndGuildCombatBoard() 
{
}
#endif //__GUILD_COMBAT_1TO1
CWndGuildCombatBoard::~CWndGuildCombatBoard() 
{
}

#if __VER >= 11 // __GUILD_COMBAT_1TO1
void CWndGuildCombatBoard::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();
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

		if(m_nCombatType == 0)
			sprintf( szNameLevel, "%s", prj.GetText(TID_GAME_GUILDCOMBAT_BOARD) );
		else if(m_nCombatType == 1)
			sprintf( szNameLevel, "%s", prj.GetText(TID_GAME_GUILDCOMBAT_1TO1_BOARD) );

		SetTitle( szNameLevel );
		
		p2DRender->SetFont( pOldFont );
	}
}
#endif //__GUILD_COMBAT_1TO1

void CWndGuildCombatBoard::OnDraw( C2DRender* p2DRender ) 
{
}

void CWndGuildCombatBoard::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	CRect rect	= GetClientRect();
	rect.left	+= 8;
	rect.right	-= 4;
	rect.top	+= 8;
	rect.bottom	/= 2;

	MoveParentCenter();
} 

BOOL CWndGuildCombatBoard::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_BOARD, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndGuildCombatBoard::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndGuildCombatBoard::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndGuildCombatBoard::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndGuildCombatBoard::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndGuildCombatBoard::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BUTTON1 )
	{
		Destroy();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndGuildCombatBoard::SetString( CHAR* szChar )
{
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	
	pWndText->m_string.AddParsingString( szChar );
	pWndText->ResetString();	
}

// 길드컴뱃 관련 정보창...시간별 뜨는 메세지창...
#if __VER >= 11 // __GUILD_COMBAT_1TO1
CGuildCombatInfoMessageBox::CGuildCombatInfoMessageBox(int nCombatType)
{
	m_nCombatType = nCombatType;
}
#else //__GUILD_COMBAT_1TO1
CGuildCombatInfoMessageBox::CGuildCombatInfoMessageBox()
{
}
#endif //__GUILD_COMBAT_1TO1

CGuildCombatInfoMessageBox::~CGuildCombatInfoMessageBox()
{
}

#if __VER >= 11 // __GUILD_COMBAT_1TO1
void CGuildCombatInfoMessageBox::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();
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

		if(m_nCombatType == 0)
			sprintf( szNameLevel, "%s", prj.GetText(TID_GAME_GUILDCOMBAT_OFFERSTATE) );
		else if(m_nCombatType == 1)
			sprintf( szNameLevel, "%s", prj.GetText(TID_GAME_GUILDCOMBAT_1TO1_OFFERSTATE) );

		SetTitle( szNameLevel );
		
		p2DRender->SetFont( pOldFont );
	}
}
#endif //__GUILD_COMBAT_1TO1

BOOL CGuildCombatInfoMessageBox::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_MSG, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CGuildCombatInfoMessageBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case WIDC_YES:
		{
#if __VER >= 11 // __GUILD_COMBAT_1TO1
			if(m_nCombatType == 0)
				g_DPlay.SendGCTele();
			else if(m_nCombatType == 1)
				g_DPlay.SendGC1to1TeleportToNPC();
#else //__GUILD_COMBAT_1TO1
			g_DPlay.SendGCTele();
#endif //__GUILD_COMBAT_1TO1
			Destroy();
		}
		break;
	case WIDC_NO:
		{
			Destroy();
		}
	case 10000:
		{
			Destroy();
		}
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CGuildCombatInfoMessageBox::SetString( CHAR* szChar )
{
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	
	pWndText->m_string.AddParsingString( szChar  );
	pWndText->ResetString();	
}

#if __VER >= 11 // __GUILD_COMBAT_1TO1
void CGuildCombatInfoMessageBox::SetString( CString strMsg )
{
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	
	pWndText->m_string.AddParsingString( strMsg  );
	pWndText->ResetString();	
}
#endif //__GUILD_COMBAT_1TO1

void CGuildCombatInfoMessageBox::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	MoveParentCenter();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// 길드컴뱃 관련 정보창...시간별 뜨는 메세지창...
CGuildCombatInfoMessageBox2::CGuildCombatInfoMessageBox2()
{
}
CGuildCombatInfoMessageBox2::~CGuildCombatInfoMessageBox2()
{
}
void CGuildCombatInfoMessageBox2::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	MoveParentCenter();	
}

BOOL CGuildCombatInfoMessageBox2::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_MSG2, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CGuildCombatInfoMessageBox2::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case WIDC_OK:
		{
			Destroy();
		}
		break;
	case 10000:
		{
			Destroy();
		}
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CGuildCombatInfoMessageBox2::SetString( CHAR* szChar  )
{
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	
	pWndText->m_string.AddParsingString( szChar  );
	pWndText->ResetString();	
}
////////////////////////////////////////////////////////////////////////////////////////////////////////




// 길드 참가자 구성 리셋 컨펌
BOOL CGuildCombatSelectionClearMessageBox::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	return CWndMessageBox::Initialize( prj.GetText(TID_GAME_GUILDCOMBAT_REMAKE_MAKEUP), //명단작성을 다시 하시겠습니까?
		pWndParent, 
		MB_OKCANCEL );
}

BOOL CGuildCombatSelectionClearMessageBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case IDOK:
		{
			CWndGuildCombatSelection *pGuildCombatSelection = (CWndGuildCombatSelection*)g_WndMng.GetWndBase( APP_GUILDCOMBAT_SELECTION );

			if( pGuildCombatSelection )
				pGuildCombatSelection->Reset();

			Destroy();
		}
		break;
	case IDCANCEL:
		Destroy();
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
////////////////////////////////////////////////////////////////////////////////////////////////////




// 길드 컴뱃 참가자 구성
CWndGuildCombatSelection::CWndGuildCombatSelection() 
{
	m_vecGuildList.clear();
	m_mapSelectPlayer.clear();
	m_vecSelectPlayer.clear();
	m_uidDefender = -1;	

	nMaxJoinMember = 0;
	nMaxWarMember  = 0;
}

CWndGuildCombatSelection::~CWndGuildCombatSelection() 
{
	
}

void CWndGuildCombatSelection::SetDefender( u_long uiPlayer ) 
{
	m_uidDefender = uiPlayer;
}

void CWndGuildCombatSelection::AddCombatPlayer( u_long uiPlayer ) 
{
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem( WIDC_LISTBOX2 );
	m_vecSelectPlayer.push_back( uiPlayer );

	CGuild* pGuild = g_pPlayer->GetGuild();
	
	map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.find( uiPlayer );
	CGuildMember* pMember = i->second;
				
	CString str;
#if __VER >= 11 // __SYS_PLAYER_DATA
	PlayerData* pPlayerData		= CPlayerDataCenter::GetInstance()->GetPlayerData( pMember->m_idPlayer );
	str.Format( "Lv%.2d	%.16s %.10s", pPlayerData->data.nLevel, pPlayerData->szPlayer, prj.m_aJob[ pPlayerData->data.nJob ].szName );
#else	// __SYS_PLAYER_DATA
	str.Format( "Lv%.2d	%.16s %.10s", pMember->m_nLevel, prj.GetPlayerString( pMember->m_idPlayer ), prj.m_aJob[ pMember->m_nJob ].szName );
#endif	// __SYS_PLAYER_DATA
	pWndList->AddString( str );			
} 

void CWndGuildCombatSelection::SetMemberSize( int nMaxJoin,  int nMaxWar ) 
{ 
	nMaxJoinMember = nMaxJoin; 
	nMaxWarMember  = nMaxWar; 

	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC3 );
	
	CString str;
	str.Format( prj.GetText(TID_GAME_GUILDCOMBAT_OFFER_INFO), nMaxWarMember, nMaxJoinMember );
	pWndStatic->SetTitle( str );	
}


void CWndGuildCombatSelection::AddGuildPlayer( u_long uiPlayer ) 
{
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	m_vecGuildList.push_back( uiPlayer );
	
	CGuild* pGuild = g_pPlayer->GetGuild();
	
	map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.find( uiPlayer );
	CGuildMember* pMember = i->second;
				
	CString str;
#if __VER >= 11 // __SYS_PLAYER_DATA
	PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->GetPlayerData( pMember->m_idPlayer );
	str.Format( "Lv%.2d	%.16s %.10s", pPlayerData->data.nLevel, pPlayerData->szPlayer, prj.m_aJob[ pPlayerData->data.nJob ].szName );
#else	// __SYS_PLAYER_DATA
	str.Format( "Lv%.2d	%.16s %.10s", pMember->m_nLevel, prj.GetPlayerString( pMember->m_idPlayer ), prj.m_aJob[ pMember->m_nJob ].szName );
#endif	// __SYS_PLAYER_DATA
	pWndList->AddString( str );			
} 

void CWndGuildCombatSelection::RemoveGuildPlayer( int nIndex ) 
{
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	
	pWndList->DeleteString( nIndex );
	vector<u_long>::iterator where = &(m_vecGuildList[nIndex]);	
	m_vecGuildList.erase( where );	
} 

void CWndGuildCombatSelection::RemoveCombatPlayer( int nIndex ) 
{
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem( WIDC_LISTBOX2 );
	
	pWndList->DeleteString( nIndex );
	vector<u_long>::iterator where = &(m_vecSelectPlayer[nIndex]);	
	if( m_vecSelectPlayer[nIndex] == m_uidDefender )
		SetDefender( -1 );

	m_vecSelectPlayer.erase( where );	
} 


void CWndGuildCombatSelection::UpDateGuildListBox() 
{
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );

	if( pWndList )
	{
		pWndList->ResetContent();
		
		m_vecGuildList.clear();
		m_mapSelectPlayer.clear();

		CGuild* pGuild = g_pPlayer->GetGuild();
		if( pGuild )
		{
			// 레벨별로 소팅
			CGuildMember* pMember;
			for( map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
			{
				pMember		= i->second;
#if __VER >= 11 // __SYS_PLAYER_DATA
				PlayerData* pPlayerData		= CPlayerDataCenter::GetInstance()->GetPlayerData( pMember->m_idPlayer );
				if( pPlayerData->data.uLogin > 0 )
					m_mapSelectPlayer.insert( make_pair( pPlayerData->data.nLevel, pMember) );
#else	// __SYS_PLAYER_DATA
				if( pMember->m_nLogin )
					m_mapSelectPlayer.insert( make_pair(pMember->m_nLevel, pMember) );
#endif	// __SYS_PLAYER_DATA
			}

			// 리스트에 추가			
			CString str;
			for( multimap<int, CGuildMember*>::iterator j = m_mapSelectPlayer.begin(); j != m_mapSelectPlayer.end(); ++j )
			{
				pMember		= j->second;		
#if __VER >= 11 // __SYS_PLAYER_DATA
				PlayerData* pPlayerData		= CPlayerDataCenter::GetInstance()->GetPlayerData( pMember->m_idPlayer );
				if( pPlayerData->data.uLogin > 0 )
#else	// __SYS_PLAYER_DATA
				if( pMember->m_nLogin )
#endif	// __SYS_PLAYER_DATA
				{
#if __VER >= 11 // __SYS_PLAYER_DATA
					str.Format( "Lv%.2d	%.16s %.10s", pPlayerData->data.nLevel, pPlayerData->szPlayer, prj.m_aJob[ pPlayerData->data.nJob ].szName );
#else	// __SYS_PLAYER_DATA
					str.Format( "Lv%.2d	%.16s %.10s", pMember->m_nLevel, prj.GetPlayerString( pMember->m_idPlayer ), prj.m_aJob[ pMember->m_nJob ].szName );
#endif	// __SYS_PLAYER_DATA
					pWndList->AddString( str );	
					m_vecGuildList.push_back( pMember->m_idPlayer );
				}
			}
		}
	}
}

u_long CWndGuildCombatSelection::FindCombatPlayer(u_long uiPlayer)
{
	for( int i = 0; i < (int)( m_vecSelectPlayer.size() ); i++ )
	{
		if( m_vecSelectPlayer[i] == uiPlayer )
			return m_vecSelectPlayer[i];
	}
	
	return -1;
}

u_long CWndGuildCombatSelection::FindGuildPlayer(u_long uiPlayer)
{
	for( int i = 0; i < (int)( m_vecGuildList.size() ); i++ )
	{
		if( m_vecGuildList[i] == uiPlayer )
			return m_vecGuildList[i];
	}
	
	return -1;
}

void CWndGuildCombatSelection::OnDraw( C2DRender* p2DRender ) 
{
	CWndButton* pWndStatic = (CWndButton*)GetDlgItem( WIDC_BUTTON8 );

	if( pWndStatic )
	{
		pWndStatic->EnableWindow( FALSE );

		if( m_uidDefender <= 0 )
		{
			pWndStatic->SetVisible( FALSE );
		}
		else
		{
			pWndStatic->SetVisible( TRUE );

			for( int i=0; i<(int)( m_vecSelectPlayer.size() ); i++ )
			{
				if( m_uidDefender == m_vecSelectPlayer[i] )
				{
					break;
				}
			}

			pWndStatic->Move( 330, ( i * 17 ) + 75 );			
		}
	}

	CRect crect;
	crect.left = 349;
	crect.top  = 74;
	crect.right = 587;
	crect.bottom = crect.top+(nMaxWarMember*17);
	p2DRender->RenderFillRect( crect, D3DCOLOR_ARGB( 40, 0, 0, 220 ) );

	crect.left = 349;
	crect.top  = crect.bottom;
	crect.right = 587;
	crect.bottom = 377;
	p2DRender->RenderFillRect( crect, D3DCOLOR_ARGB( 40, 220, 0, 0 ) );
	
}

void CWndGuildCombatSelection::EnableFinish( BOOL bFlag )
{
	CWndButton* pWndButton = (CWndButton*)GetDlgItem( WIDC_FINISH );

	if( pWndButton )
	{
		pWndButton->EnableWindow( bFlag );	
	}
}

void CWndGuildCombatSelection::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	// 시간 지났는지를 판단
	if( g_GuildCombatMng.m_nGCState != CGuildCombat::NOTENTER_COUNT_STATE )
	{
		g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_MAKEUP) ); //지금은 명단작성을 할 수 없습니다.
		Destroy();
		return;
	}
	
	MoveParentCenter();

	UpDateGuildListBox();
} 

BOOL CWndGuildCombatSelection::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_SELECTION, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndGuildCombatSelection::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndGuildCombatSelection::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndGuildCombatSelection::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndGuildCombatSelection::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 


void CWndGuildCombatSelection::Reset()
{
	m_uidDefender = 0;
	UpDateGuildListBox();
	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX2 );
	pWndListBox->ResetContent();
	m_vecSelectPlayer.clear();	
}

BOOL CWndGuildCombatSelection::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	// 출전자 등록
	if( nID == WIDC_BUTTON1 )
	{
		CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );

		int nCurSel = pWndListBox->GetCurSel();
		if( nCurSel == -1 )
			return FALSE;

		if( nMaxJoinMember < (int)( m_vecSelectPlayer.size() ) )
		{
			CString str;
			str.Format( prj.GetText(TID_GAME_GUILDCOMBAT_SELECTION_MAX), nMaxJoinMember );
			g_WndMng.OpenMessageBox( str );
			return FALSE;
		}

		CGuild *pGuild = g_pPlayer->GetGuild();

		if( pGuild )
		{
			CGuildMember* pGuildMember = pGuild->GetMember( m_vecGuildList[nCurSel] );

			if( pGuildMember )
			{
#if __VER >= 11 // __SYS_PLAYER_DATA
				if( CPlayerDataCenter::GetInstance()->GetPlayerData( pGuildMember->m_idPlayer )->data.nLevel < 30 )
#else	// __SYS_PLAYER_DATA
				if( pGuildMember->m_nLevel < 30 )
#endif	// __SYS_PLAYER_DATA
				{
					g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT_LIMIT_LEVEL_NOTICE) ); //출전자 등록은 레벨 30이상이 되어야 합니다.
					return FALSE;
				}
			}	
			else
			{
				g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT_NOT_GUILD_MEMBER) );	//길드대전에 참가하는 길드의 맴버가 아닙니다.			
				return FALSE;
			}
		}

		u_long uiPlayer;
		uiPlayer = FindCombatPlayer( m_vecGuildList[nCurSel] );

		if( uiPlayer != -1 )
		{
			g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT_ALREADY_ENTRY) ); //이미 등록되어 있습니다. 다시 등록해주세요.
			return FALSE;
		}
 
		AddCombatPlayer( m_vecGuildList[nCurSel] );		
		RemoveGuildPlayer( nCurSel );		
	}
	else
	// 출전자 취소
	if( nID == WIDC_BUTTON2 )
	{
		CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX2 );
		
		int nCurSel = pWndListBox->GetCurSel();
		if( nCurSel == -1 )
			return FALSE;

		u_long uiPlayer;
		uiPlayer = FindGuildPlayer( m_vecSelectPlayer[nCurSel] );
		
		if( uiPlayer == -1 )
		{
			// 길드리스트에 없다면 추가 
			AddGuildPlayer( m_vecSelectPlayer[nCurSel] );		
			RemoveCombatPlayer( nCurSel );		
		}
		else
		{
			RemoveCombatPlayer( nCurSel );		
		}		
	}
	else
	// 출전자 순서 위로
	if( nID == WIDC_BUTTON3 )
	{
		CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX2 );
		
		int nCurSel = pWndListBox->GetCurSel();
		if( nCurSel == -1 || nCurSel == 0 )
			return FALSE;
		
		CString temp1, temp2;
		pWndListBox->GetText( nCurSel-1, temp1 );
		pWndListBox->GetText( nCurSel,   temp2 );

		pWndListBox->SetString( nCurSel-1, temp2 );
		pWndListBox->SetString( nCurSel, temp1 );
		
		u_long uiTemp = 0;

		uiTemp						 = m_vecSelectPlayer[nCurSel];
		m_vecSelectPlayer[nCurSel]   = m_vecSelectPlayer[nCurSel-1];
		m_vecSelectPlayer[nCurSel-1] = uiTemp;

		m_nDefenderIndex = nCurSel-1;
	}
	else
	// 출전자 순서 아래
	if( nID == WIDC_BUTTON4 )
	{
		CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX2 );
		
		int nCurSel = pWndListBox->GetCurSel();
		if( nCurSel == -1 || nCurSel == pWndListBox->GetCount()-1 )
			return FALSE;
		
		CString temp1, temp2;
		pWndListBox->GetText( nCurSel, temp1 );
		pWndListBox->GetText( nCurSel+1,   temp2 );
		
		pWndListBox->SetString( nCurSel, temp2 );
		pWndListBox->SetString( nCurSel+1, temp1 );
		
		u_long uiTemp = 0;
		
		uiTemp						 = m_vecSelectPlayer[nCurSel];
		m_vecSelectPlayer[nCurSel]   = m_vecSelectPlayer[nCurSel+1];
		m_vecSelectPlayer[nCurSel+1] = uiTemp;

		m_nDefenderIndex = nCurSel+1;
	}
	else
	if( nID == WIDC_RESET )
	{
		CGuildCombatSelectionClearMessageBox* pBox = new CGuildCombatSelectionClearMessageBox;
		g_WndMng.OpenCustomBox( "", pBox );
	}
	else
	if( nID == WIDC_FINISH )
	{
#ifndef _DEBUG
		if( m_uidDefender == -1 || m_uidDefender == 0 )
		{
			g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT_NOT_ASSIGN_DEFENDER) ); //디펜더가 지정되지 않았습니다. 길드마스터를 제외한 1명의 디펜더를 지정해 주세요.
			return FALSE;
		}
#endif //_DEBUG

		// 시간 지났는지를 판단
		if( g_GuildCombatMng.m_nGCState != CGuildCombat::NOTENTER_COUNT_STATE )
		{
			g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_MAKEUP) ); //지금은 명단작성을 할 수 없습니다.
			Destroy();
			return FALSE;
		}
		
		if( m_vecSelectPlayer.size() == 0 )
		{
			g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT_HAVENOT_PLAYER) ); //출전자가 없습니다. 출전자를 선택해주세요.
			return FALSE;
		}
		
		CGuild *pGuild = g_pPlayer->GetGuild();
		CGuildMember* pGuildMemberl;
		
		if( pGuild )
		{
			BOOL bSkip = FALSE;

			// 출전자 맴버중에 마스터가 킹핀이 있는지 검사를한다.
			// 둘중 하나라도 없으면 출전 불가능...
			for( int i=0; i<(int)( m_vecSelectPlayer.size() ); i++ )
			{
				pGuildMemberl = pGuild->GetMember( m_vecSelectPlayer[i] );

				if( pGuildMemberl )
				{
					if( pGuildMemberl->m_nMemberLv == GUD_MASTER || pGuildMemberl->m_nMemberLv == GUD_KINGPIN )
					{
						bSkip = TRUE;
						break;
					}
				}
			}

			if( bSkip )
			{
				g_DPlay.SendGCSelectPlayer( m_vecSelectPlayer, m_uidDefender );
				Destroy();
			}
			else
			{
				g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT_HAVENOT_MASTER) ); //출전자 명단에 길드마스터나 킹핀이 존재하지 않습니다.
				return FALSE;
			}
		}
	}
	else
	// 디펜더 지정
	if( nID == WIDC_BUTTON7 )
	{
		CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX2 );
		
		int nCurSel = pWndListBox->GetCurSel();
		if( nCurSel == -1 )
			return FALSE;

		CGuild *pGuild = g_pPlayer->GetGuild();

		if( pGuild )
		{
			if( m_vecSelectPlayer.size() > 1 &&  pGuild->IsMaster( m_vecSelectPlayer[nCurSel] ) )
			{
				g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT_MASTER_NOT_ASSIGN_DEFENDER) ); //길드마스터는 디펜더가 될 수 없습니다.
				return FALSE;
			}
		}
		
		m_nDefenderIndex = nCurSel;
		SetDefender( m_vecSelectPlayer[nCurSel] );
	}
	else
	if( nID == WIDC_CLOSE )
	{
		Destroy();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}



//길드대전 참가 길드 명단 출력
#if __VER >= 11 // __GUILD_COMBAT_1TO1
CWndGuildCombatState::CWndGuildCombatState(int nCombatType)
{ 
	Init( 0 );

	m_tEndTime    = 0;
	m_tCurrentTime = 0;
	m_nCombatType = nCombatType;
}
#else //__GUILD_COMBAT_1TO1
CWndGuildCombatState::CWndGuildCombatState() 
{ 
	Init( 0 );

	m_tEndTime    = 0;
	m_tCurrentTime = 0;
} 
#endif //__GUILD_COMBAT_1TO1

CWndGuildCombatState::~CWndGuildCombatState() 
{ 
} 

#if __VER >= 11 // __GUILD_COMBAT_1TO1
void CWndGuildCombatState::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();
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

		if(m_nCombatType == 0)
			sprintf( szNameLevel, "%s", prj.GetText(TID_GAME_GUILDCOMBAT_OFFERSTATE) );
		else if(m_nCombatType == 1)
			sprintf( szNameLevel, "%s", prj.GetText(TID_GAME_GUILDCOMBAT_1TO1_OFFERSTATE) );

		SetTitle( szNameLevel );
		
		p2DRender->SetFont( pOldFont );
	}
}
#endif //__GUILD_COMBAT_1TO1

void CWndGuildCombatState::Init( time_t lTime )
{
}

void CWndGuildCombatState::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 

void CWndGuildCombatState::InsertTitle( const char szTitle[] )
{
	CString strTitle;
	strTitle.Format( "%s - %s", GetTitle(), szTitle );
	SetTitle( strTitle );
}

BOOL CWndGuildCombatState::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{
#if __VER >= 11 // __GUILD_COMBAT_1TO1
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_1TO1_OFFERSTATE, 0, CPoint( 0, 0 ), pWndParent );
#else //__GUILD_COMBAT_1TO1
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_OFFERSTATE, 0, CPoint( 0, 0 ), pWndParent );
#endif //__GUILD_COMBAT_1TO1
} 

BOOL CWndGuildCombatState::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndGuildCombatState::OnSize( UINT nType, int cx, int cy )
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndGuildCombatState::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

// 선택된 인덱스를 얻는다.
int CWndGuildCombatState::GetSelectIndex( const CPoint& point )
{
	return -1;
}

void CWndGuildCombatState::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

void CWndGuildCombatState::OnLButtonDblClk( UINT nFlags, CPoint point)
{
}

void CWndGuildCombatState::OnRButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

void CWndGuildCombatState::OnRButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndGuildCombatState::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_CLOSE )
	{
		Destroy();
	}
	else
	if( nID == WIDC_BUTTON1 )
	{
		Destroy();
#if __VER >= 11 // __GUILD_COMBAT_1TO1
		if(m_nCombatType == 0)
			g_DPlay.SendGuildCombatWindow();
		else if(m_nCombatType == 1)
			g_DPlay.SendGC1to1TenderOpenWnd();
#else //__GUILD_COMBAT_1TO1
		g_DPlay.SendGuildCombatWindow();
#endif //__GUILD_COMBAT_1TO1
	}
	else
	if( nID == WIDC_BUTTON2 )
	{
#if __VER >= 11 // __GUILD_COMBAT_1TO1
		if(m_nCombatType == 0)
			g_DPlay.SendGCRequestStatusWindow();
		else if(m_nCombatType == 1)
			g_DPlay.SendGC1to1TenderView();
#else //__GUILD_COMBAT_1TO1
		g_DPlay.SendGCRequestStatusWindow();
#endif //__GUILD_COMBAT_1TO1
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndGuildCombatState::InsertGuild( const char szGuild[], const char szName[], int nNum )
{
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	
	CString str;
	str.Format( "No.%.2d		%s", nNum, szGuild );
	pWndList->AddString(str);
}

void CWndGuildCombatState::OnDraw( C2DRender* p2DRender ) 
{ 
	if( m_tCurrentTime > 0 )
	{
		CWndStatic* pStatic	= (CWndStatic *)GetDlgItem( WIDC_STATIC2 );

		CString str1;
		CString str2;
		CString str3;
		CString str4;
		CString str;
		
		str1.Format(prj.GetText(TID_PK_LIMIT_DAY), static_cast<int>(m_ct.GetDays()) );
		str2.Format(prj.GetText(TID_PK_LIMIT_HOUR), m_ct.GetHours() );
		str3.Format(prj.GetText(TID_PK_LIMIT_MINUTE), m_ct.GetMinutes() );
		str4.Format(prj.GetText(TID_PK_LIMIT_SECOND), m_ct.GetSeconds() );

		str.Format( "%s %s %s %s", str1, str2, str3, str4 );
		pStatic->SetTitle(str);
	}
} 

void CWndGuildCombatState::SetRate( int nRate )
{
	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC5 );
	
	CString str;
	str.Format( "%d", nRate );
	
	if( nRate == -1 )
		str = " ";

	pWndStatic->SetTitle( str );
	if( 8 < nRate )
		pWndStatic->m_dwColor = 0xFFFF0000;	
}

BOOL CWndGuildCombatState::Process() 
{
	m_tCurrentTime = ( m_tEndTime - time_null() );
	
	if( m_tEndTime && m_tCurrentTime > 0 )
	{
		m_ct = m_tCurrentTime;//long)(m_dwCurrentTime / 1000.0f) );
	}
	
	return TRUE;
}

#if __VER < 11 // __GUILD_COMBAT_1TO1
void CWndGuildCombatState::SetTotalGold( __int64 nGold )
{
	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC8 );
	pWndStatic->AddWndStyle(WSS_MONEY);	

	CString str;
	str.Format( "%I64d", nGold );
	pWndStatic->SetTitle( str );
	
}
#endif //__GUILD_COMBAT_1TO1

void CWndGuildCombatState::SetGold( int nGold )
{
	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC6 );
	pWndStatic->AddWndStyle(WSS_MONEY);	

	CString str;
	str.Format( "%d", nGold );
	pWndStatic->SetTitle( str );	
}



// 길드전장 입장지역 선택 창
CWndGuildCombatJoinSelection::CWndGuildCombatJoinSelection() 
{
	m_nMapNum = 99;
	m_dwOldTime = 0xffffffff;
}

CWndGuildCombatJoinSelection::~CWndGuildCombatJoinSelection() 
{
	
}

void CWndGuildCombatJoinSelection::OnDraw( C2DRender* p2DRender ) 
{
	DWORD dwLeftTime = m_dwOldTime - g_tmCurrent;

	CWndStatic* pStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC2 );
	
	CString str;
	CString strstr;
	
	if( m_nMapNum == 99 )
	{
		strstr = "Random";
	}
	else
	{
		strstr.Format( "%d", m_nMapNum+1 );
	}

	str.Format( prj.GetText(TID_GAME_GUILDCOMBAT_JOIN_MESSAGE), (dwLeftTime/1000)+1, strstr );
	
	pStatic->SetTitle( str );

	if( m_nMapNum != 99 )
	{
		CWndButton* pWndButton = NULL;

		if( m_nMapNum == 0 )
		{
			pWndButton = (CWndButton*) GetDlgItem( WIDC_BUTTON1 );
		}
		else
		if( m_nMapNum == 1 )
		{
			pWndButton = (CWndButton*) GetDlgItem( WIDC_BUTTON2 );
		}
		else
		if( m_nMapNum == 2 )
		{
			pWndButton = (CWndButton*) GetDlgItem( WIDC_BUTTON3 );
		}
		else
		if( m_nMapNum == 3 )
		{
			pWndButton = (CWndButton*) GetDlgItem( WIDC_BUTTON4 );
		}
		
		if( pWndButton )
		{
			CRect rect = pWndButton->GetWndRect();
			rect.DeflateRect( 1, 1, 1, 1 );
			p2DRender->RenderRect( rect, 0xFF0000FF );
		}
	}
}

void CWndGuildCombatJoinSelection::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	MoveParentCenter();

	m_wndTitleBar.SetVisible( FALSE );	
} 

BOOL CWndGuildCombatJoinSelection::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_SELSTART, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndGuildCombatJoinSelection::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndGuildCombatJoinSelection::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndGuildCombatJoinSelection::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndGuildCombatJoinSelection::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndGuildCombatJoinSelection::Process ()
{
	if( m_dwOldTime <= g_tmCurrent )
	{
		g_DPlay.SendGCSelectMap(m_nMapNum);		
		Destroy();
	}

	return 1;
}

BOOL CWndGuildCombatJoinSelection::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BUTTON1 )
	{
		g_DPlay.SendGCSelectMap(0);	
		m_nMapNum = 0;
	}
	else if( nID == WIDC_BUTTON2 )
	{
		g_DPlay.SendGCSelectMap(1);		
		m_nMapNum = 1;
	}
	else if( nID == WIDC_BUTTON3 )
	{
		g_DPlay.SendGCSelectMap(2);		
		m_nMapNum = 2;
	}
	else if( nID == WIDC_BUTTON4 )
	{
		g_DPlay.SendGCSelectMap(3);		
		m_nMapNum = 3;
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 길드대전 신청
CWndGuildWarAppConfirm::CWndGuildWarAppConfirm() 
{
}

CWndGuildWarAppConfirm::~CWndGuildWarAppConfirm() 
{
}

void CWndGuildWarAppConfirm::OnDraw( C2DRender* p2DRender ) 
{
}

void CWndGuildWarAppConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	CRect rect	= GetClientRect();
	rect.left	+= 8;
	rect.right	-= 4;
	rect.top	+= 8;
	rect.bottom	/= 2;
	
	m_wndText.Create( WBS_NODRAWFRAME, rect, this, 0 );
	m_wndText.SetString( _T( prj.GetText(TID_GAME_GUILDWAR_APP) ), 0xffffffff );//ADEILSON 0xff000000//prj.GetTextColor(TID_DIAG_0065) );
	m_wndText.ResetString();
	
	MoveParentCenter();
} 

BOOL CWndGuildWarAppConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_CLOSE_EXISTING_CONNECTION, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndGuildWarAppConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndGuildWarAppConfirm::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndGuildWarAppConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndGuildWarAppConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndGuildWarAppConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES )
	{
		if( g_pPlayer )
			g_DPlay.SendGuildCombatApp( 80000 );

		Destroy( TRUE );
	}
	else if( nID == WIDC_NO )
	{
		Destroy( TRUE );
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

// 길드대전 취소
#if __VER >= 11 // __GUILD_COMBAT_1TO1
CWndGuildWarCancelConfirm::CWndGuildWarCancelConfirm(int nCombatType)
{
	m_nCombatType = nCombatType;
}
#else //__GUILD_COMBAT_1TO1
CWndGuildWarCancelConfirm::CWndGuildWarCancelConfirm() 
{
}
#endif //__GUILD_COMBAT_1TO1

CWndGuildWarCancelConfirm::~CWndGuildWarCancelConfirm() 
{
}

#if __VER >= 11 // __GUILD_COMBAT_1TO1
void CWndGuildWarCancelConfirm::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();
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

		if(m_nCombatType == 0)
			sprintf( szNameLevel, "%s", prj.GetText(TID_GAME_GUILDCOMBAT_CANCEL_CONFIRM) );
		else if(m_nCombatType == 1)
			sprintf( szNameLevel, "%s", prj.GetText(TID_GAME_GUILDCOMBAT_1TO1_CANCEL_CONFIRM) );

		SetTitle( szNameLevel );
		
		p2DRender->SetFont( pOldFont );
	}
}
#endif //__GUILD_COMBAT_1TO1

void CWndGuildWarCancelConfirm::OnDraw( C2DRender* p2DRender ) 
{
}

void CWndGuildWarCancelConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	CRect rect	= GetClientRect();
	rect.left	+= 8;
	rect.right	-= 4;
	rect.top	+= 8;
	rect.bottom	/= 2;
	
	m_wndText.Create( WBS_NODRAWFRAME, rect, this, 0 );
	m_wndText.SetString( _T( prj.GetText(TID_GAME_GUILDWAR_CANCEL) ), 0xffffffff );//ADEILSON 0xff000000//prj.GetTextColor(TID_DIAG_0065) );
	m_wndText.ResetString();
	
	MoveParentCenter();
} 

BOOL CWndGuildWarCancelConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_CANCEL_CONFIRM, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndGuildWarCancelConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndGuildWarCancelConfirm::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndGuildWarCancelConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndGuildWarCancelConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndGuildWarCancelConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES )
	{
#if __VER >= 11 // __GUILD_COMBAT_1TO1
		if(m_nCombatType == 0)
		{
			if( g_pPlayer )
				g_DPlay.SendGuildCombatCancel();
		}
		else if(m_nCombatType == 1)
		{
			if( g_pPlayer )
				g_DPlay.SendGC1to1TenderCancel();
		}
#else //__GUILD_COMBAT_1TO1
		if( g_pPlayer )
			g_DPlay.SendGuildCombatCancel();
#endif //__GUILD_COMBAT_1TO1
		
		Destroy( TRUE );
	}
	else if( nID == WIDC_NO )
	{
		Destroy( TRUE );
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

// 길드대전 입장
#if __VER >= 11 // __GUILD_COMBAT_1TO1
CWndGuildWarJoinConfirm::CWndGuildWarJoinConfirm(int nCombatType) 
{
	m_nCombatType = nCombatType;
}
#else //__GUILD_COMBAT_1TO1
CWndGuildWarJoinConfirm::CWndGuildWarJoinConfirm() 
{
}
#endif //__GUILD_COMBAT_1TO1

CWndGuildWarJoinConfirm::~CWndGuildWarJoinConfirm() 
{	
}

#if __VER >= 11 // __GUILD_COMBAT_1TO1
void CWndGuildWarJoinConfirm::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();
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

		if(m_nCombatType == 0)
			sprintf( szNameLevel, "%s", prj.GetText(TID_GAME_GUILDCOMBAT_ENTRANCE) );
		else if(m_nCombatType == 1)
			sprintf( szNameLevel, "%s", prj.GetText(TID_GAME_GUILDCOMBAT_1TO1_ENTRANCE) );

		SetTitle( szNameLevel );
		
		p2DRender->SetFont( pOldFont );
	}
}
#endif //__GUILD_COMBAT_1TO1

void CWndGuildWarJoinConfirm::OnDraw( C2DRender* p2DRender ) 
{
}

void CWndGuildWarJoinConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	CRect rect	= GetClientRect();
	rect.left	+= 8;
	rect.right	-= 4;
	rect.top	+= 8;
	rect.bottom	/= 2;
	
	m_wndText.Create( WBS_NODRAWFRAME, rect, this, 0 );
#if __VER >= 11 // __GUILD_COMBAT_1TO1
	if(m_nCombatType == 0)
		m_wndText.SetString( _T( prj.GetText(TID_GAME_GUILDWAR_JOIN) ), prj.GetTextColor(TID_GAME_GUILDWAR_JOIN) );
	else if(m_nCombatType == 1)
		m_wndText.SetString( _T( prj.GetText(TID_GAME_GUILDCOMBAT_1TO1_ENTRANCE_MSG) ), prj.GetTextColor(TID_GAME_GUILDCOMBAT_1TO1_ENTRANCE_MSG) );
#else //__GUILD_COMBAT_1TO1
	m_wndText.SetString( _T( prj.GetText(TID_GAME_GUILDWAR_JOIN) ), prj.GetTextColor(TID_GAME_GUILDWAR_JOIN) );
#endif //__GUILD_COMBAT_1TO1
	m_wndText.ResetString();
	
	MoveParentCenter();
} 

BOOL CWndGuildWarJoinConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_CLOSE_EXISTING_CONNECTION, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndGuildWarJoinConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndGuildWarJoinConfirm::OnSize( UINT nType, int cx, int cy )
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndGuildWarJoinConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndGuildWarJoinConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndGuildWarJoinConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES )
	{
		if( g_pPlayer )
		{
			if( g_pPlayer->IsBaseJob() )
			{
				g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT_NOTENTER_VAGRANT) ); //방랑자는 입장할수 없습니다. 1차 전직 후에 입장을 해주시기 바랍니다.
				Destroy( TRUE );
				return FALSE;
			}
#if __VER >= 11 // __GUILD_COMBAT_1TO1
			if(m_nCombatType == 0)
				g_DPlay.SendGCJoin();
			else if(m_nCombatType == 1)
			{
				if(g_WndMng.m_pWndGuildCombatInfoMessageBox)
					SAFE_DELETE( g_WndMng.m_pWndGuildCombatInfoMessageBox );
				
				g_DPlay.SendGC1to1TeleportToStage();
			}
#else //__GUILD_COMBAT_1TO1
			g_DPlay.SendGCJoin();
#endif //__GUILD_COMBAT_1TO1
		}
		
		Destroy( TRUE );
	}
	else if( nID == WIDC_NO )
	{
		Destroy( TRUE );
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}
	

//길드대전 참가 길드 명단 출력

CWndGuildWarState::CWndGuildWarState() 
{ 
	Init( 0 );
} 

CWndGuildWarState::~CWndGuildWarState() 
{ 
} 

void CWndGuildWarState::Init( time_t lTime )
{
	memset( m_aList, 0, sizeof(m_aList) );
	m_nMax = 0;
	m_nSelect = -1;
}

void CWndGuildWarState::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CRect rect = GetWindowRect();
	rect.top    += 53;
	rect.bottom -= 10;
	rect.left    = rect.right - 30;
	rect.right   -= 10;

	if( m_nMax > 0 )
	{
		m_wndScrollBar.SetScrollRange( 0, m_nMax );
		if( m_nMax < MAX_GUILDCOMBAT_LIST_PER_PAGE )
			m_wndScrollBar.SetScrollPage( m_nMax );
		else
			m_wndScrollBar.SetScrollPage( MAX_GUILDCOMBAT_LIST_PER_PAGE );
	}
	else
	{
		m_wndScrollBar.SetScrollRange( 0, 1 );
		m_wndScrollBar.SetScrollPage( 1 );
	}

	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 

void CWndGuildWarState::InsertTitle( const char szTitle[] )
{
	CString strTitle;
	strTitle.Format( "%s - %s", GetTitle(), szTitle );
	SetTitle( strTitle );
}

BOOL CWndGuildWarState::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILD_WAR_STATE, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndGuildWarState::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndGuildWarState::OnSize( UINT nType, int cx, int cy )
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndGuildWarState::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

// 선택된 인덱스를 얻는다.
int CWndGuildWarState::GetSelectIndex( const CPoint& point )
{
	int nBase = m_wndScrollBar.GetScrollPos();
	int nIndex = (point.y - 32) / 18;

	if( 0 <= nIndex && nIndex < MAX_GUILDCOMBAT_LIST_PER_PAGE ) // 0 - 19사이 
	{
		int nSelect = nBase + nIndex;
		if( 0 <= nSelect && nSelect < m_nMax )
			return nSelect;
	}
	return -1;
}

void CWndGuildWarState::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
	if( m_nMax <= 0 )
		return;

	int nSelect = GetSelectIndex( point );
	if( nSelect != -1 )
		m_nSelect = nSelect;
} 

void CWndGuildWarState::OnLButtonDblClk( UINT nFlags, CPoint point)
{
}

void CWndGuildWarState::OnRButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

void CWndGuildWarState::OnRButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndGuildWarState::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndGuildWarState::OnMouseMove(UINT nFlags, CPoint point )
{
}

void CWndGuildWarState::OnMouseWndSurface( CPoint point )
{
}	
void CWndGuildWarState::InsertGuild( const char szGuild[], const char szName[], int nNum )
{
	if( m_nMax >= MAX_GUILDCOMBAT_LIST )
	{
		Error( "CWndGuildWarState::InsertGuild - range over" );
		return;
	}
	
	if( nNum == 0 )
		return;

	GetStrCut( szGuild, m_aList[m_nMax].szGuild, 8 );

	if( GetStrLen( szGuild ) >= 8 )
	{
		_tcscat( m_aList[m_nMax].szGuild, "..." );
	}
	else
	{
		_tcscpy( m_aList[m_nMax].szGuild, szGuild );
	}

	_tcscpy( m_aList[m_nMax].szName, szName );
	m_aList[m_nMax].nNum = nNum;

	m_nMax++;
}

void CWndGuildWarState::OnDraw( C2DRender* p2DRender ) 
{ 
	const DWORD dwColor = D3DCOLOR_XRGB(0, 0, 0);
	int	sx, sy;
	char szNum[8], szCount[8];

	sx = 8;
	sy = 35;	

	CRect rc( sx, 5, sx+310, 7 ); 	
	p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 128, 0 , 0 , 0 ) );

	rc += CPoint( 0, 20 );
	p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 128, 0 , 0 , 0 ) );

	int nBase = m_wndScrollBar.GetScrollPos();
	if( nBase < 0 )
	{
		nBase = 0;
		TRACE("aa\n");
	}

	for( int i=nBase; i<nBase + MAX_GUILDCOMBAT_LIST_PER_PAGE; ++i )
	{
		if( i >= m_nMax )	
			break;
		
		sprintf( szNum, "%3d", i+1 );
		sprintf( szCount, "%2d", m_aList[i].nNum );

		if( m_nSelect >= 0 && i == m_nSelect )
		{
			rc.SetRect( sx, sy - 4, sx+295, sy + 16 ); 	
			p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 64, 0 , 0 , 0 ) );
		}

		p2DRender->TextOut( sx + 4,    sy, szNum,             dwColor );
		p2DRender->TextOut( sx + 30,   sy, m_aList[i].szGuild, dwColor );
		p2DRender->TextOut( sx + 160,  sy, m_aList[i].szName,  dwColor );
		p2DRender->TextOut( sx + 260,  sy, szCount,    dwColor );

		sy += 18;
	}
} 
BOOL CWndGuildWarState::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
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




//길드컴뱃 랭킹

CWndGuildCombatRanking::CWndGuildCombatRanking() 
{ 
	Init( 0 );
} 

CWndGuildCombatRanking::~CWndGuildCombatRanking() 
{ 
} 

void CWndGuildCombatRanking::Init( time_t lTime )
{
	m_multimapRanking.clear();
	m_nMax = 0;
	m_nSelect = -1;
}

void CWndGuildCombatRanking::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CRect rect = GetWindowRect();
	rect.top    += 53;
	rect.bottom -= 10;
	rect.left    = rect.right - 30;
	rect.right   -= 10;

	if( m_nMax > 0 )
	{
		m_wndScrollBar.SetScrollRange( 0, m_nMax );
		if( m_nMax < MAX_GUILDCOMBAT_LIST_PER_PAGE )
			m_wndScrollBar.SetScrollPage( m_nMax );
		else
			m_wndScrollBar.SetScrollPage( MAX_GUILDCOMBAT_LIST_PER_PAGE );
	}
	else
	{
		m_wndScrollBar.SetScrollRange( 0, 1 );
		m_wndScrollBar.SetScrollPage( 1 );
	}

	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 

BOOL CWndGuildCombatRanking::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_RANKING, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndGuildCombatRanking::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndGuildCombatRanking::OnSize( UINT nType, int cx, int cy )
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndGuildCombatRanking::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

// 선택된 인덱스를 얻는다.
int CWndGuildCombatRanking::GetSelectIndex( const CPoint& point )
{
	int nBase = m_wndScrollBar.GetScrollPos();
	int nIndex = (point.y - 32) / 18;

	if( 0 <= nIndex && nIndex < MAX_GUILDCOMBAT_LIST_PER_PAGE ) // 0 - 19사이 
	{
		int nSelect = nBase + nIndex;
		if( 0 <= nSelect && nSelect < m_nMax )
			return nSelect;
	}
	return -1;
}

void CWndGuildCombatRanking::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
	if( m_nMax <= 0 )
		return;

	int nSelect = GetSelectIndex( point );
	if( nSelect != -1 )
		m_nSelect = nSelect;
} 

void CWndGuildCombatRanking::OnLButtonDblClk( UINT nFlags, CPoint point)
{
}

void CWndGuildCombatRanking::OnRButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

void CWndGuildCombatRanking::OnRButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndGuildCombatRanking::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndGuildCombatRanking::OnMouseMove(UINT nFlags, CPoint point )
{
}

void CWndGuildCombatRanking::OnMouseWndSurface( CPoint point )
{
}	
void CWndGuildCombatRanking::InsertGuild( const char szGuild[], int nWinCount )
{
	if( m_nMax >= MAX_GUILDCOMBAT_LIST )
	{
		Error( "CWndGuildWarState::InsertGuild - range over" );
		return;
	}

	GUILDNAME p;
	memset( &p, 0, sizeof(GUILDNAME) );
	GetStrCut( szGuild, p.szGuild, 8 );
	
	if( GetStrLen( szGuild ) >= 8 )
	{
		_tcscat( p.szGuild, "..." );
	}
	else
	{
		_tcscpy( p.szGuild, szGuild );
	}
	
	m_multimapRanking.insert( make_pair(nWinCount, p) );

	m_nMax++;
}

void CWndGuildCombatRanking::OnDraw( C2DRender* p2DRender ) 
{ 
	DWORD dwColor = D3DCOLOR_XRGB(0, 0, 0);
	int	sx, sy;
	char szNum[8], szCount[8];

	sx = 8;
	sy = 35;	

	CRect rc( sx, 5, sx+230, 7 ); 	
	p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 128, 0 , 0 , 0 ) );

	rc += CPoint( 0, 20 );
	p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 128, 0 , 0 , 0 ) );

	int nBase = m_wndScrollBar.GetScrollPos();
	if( nBase < 0 )
	{
		nBase = 0;
		TRACE("aa\n");
	}

	int nRanking = 0;
	int nOldRanking = -1;
	multimap< int, GUILDNAME >::reverse_iterator iterRobotBegin = m_multimapRanking.rbegin();
	
	for( int i=nBase; i<nBase + MAX_GUILDCOMBAT_LIST_PER_PAGE; ++i )
	{
		if( i >= m_nMax )	
			break;

		multimap< int, GUILDNAME >::value_type& refValue = *iterRobotBegin;
		
		if( nOldRanking != refValue.first )
			nRanking++;

		sprintf( szNum, "%3d", nRanking );
		sprintf( szCount, "%2d", refValue.first );

		if( m_nSelect >= 0 && i == m_nSelect )
		{
			rc.SetRect( sx, sy - 4, sx+215, sy + 16 ); 	
			p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 64, 0 , 0 , 0 ) );
		}

		if( i == 0 )
			dwColor = D3DCOLOR_XRGB( 200 , 0 , 0 );
		else
			dwColor = D3DCOLOR_XRGB( 0 , 0 , 0 );		

		if( nOldRanking != refValue.first )
		{
			p2DRender->TextOut( sx + 4,    sy, szNum,  dwColor );
		}
		else
		{
			p2DRender->TextOut( sx + 5,    sy, "   ",  dwColor );
		}

		if( i == 0 )
		{
			p2DRender->TextOut( sx + 40,  sy, refValue.second.szGuild,  D3DCOLOR_XRGB( 200 , 0 , 0 ) );
			p2DRender->TextOut( sx + 180,  sy, szCount,    D3DCOLOR_XRGB( 200 , 0 , 0 ) );
		}
		else
		{
			p2DRender->TextOut( sx + 40,  sy, refValue.second.szGuild,  dwColor );
			p2DRender->TextOut( sx + 180,  sy, szCount,    dwColor );
		}
		

		sy += 18;

		iterRobotBegin++;
		nOldRanking = refValue.first;			
	}
} 
BOOL CWndGuildCombatRanking::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
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

void CWndGuildCombatRanking::SortRanking()
{

}


/****************************************************
  WndId : APP_GUILDCOMBAT_RESULT_POINT - 길드대전 결과창
  CtrlId : WIDC_TABCTRL1 - TabCtrl
****************************************************/
CWndGuildCombatResult::CWndGuildCombatResult() 
{ 
} 
CWndGuildCombatResult::~CWndGuildCombatResult() 
{ 
} 
void CWndGuildCombatResult::OnDraw( C2DRender* p2DRender ) 
{ 
} 

void CWndGuildCombatResult::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	CRect rect = GetClientRect();
	rect.left = 0;
	rect.top = 0;
	//pWndTabCtrl->Create( WBS_NOFRAME, rect, this, 12 );
	
	pWndTabCtrl->SetButtonLength( 100 );

	m_WndGuildCombatTabResultRate.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILDCOMBAT_TAB_RESULT );
	m_WndGuildCombatTabResultLog.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILDCOMBAT_TAB_RESULT_LOG );
	
	WTCITEM tabTabItem;
	
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_GAME_TOOLTIP_LOG1);
	tabTabItem.pWndBase = &m_WndGuildCombatTabResultRate;
	pWndTabCtrl->InsertItem( 0, &tabTabItem );


	tabTabItem.pszText = prj.GetText(TID_GAME_TOOLTIP_LOG2);
	tabTabItem.pWndBase = &m_WndGuildCombatTabResultLog;
	pWndTabCtrl->InsertItem( 1, &tabTabItem );
	
	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();

	{
//		CRect r = GetWindowRect( TRUE );
//		m_WndGuildTabMember.m_rectParent = r;
	}
	
	CWndListBox* pWndList1 = (CWndListBox*)(m_WndGuildCombatTabResultRate.GetDlgItem( WIDC_LISTBOX1 ));
	CWndListBox* pWndList2 = (CWndListBox*)(m_WndGuildCombatTabResultRate.GetDlgItem( WIDC_LISTBOX2 ));
	CWndText* pWndText = (CWndText*)(m_WndGuildCombatTabResultLog.GetDlgItem( WIDC_TEXT1 ));
	
	pWndList1->ResetContent();
	pWndList2->ResetContent();	
	pWndText->ResetString();
} 
BOOL CWndGuildCombatResult::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_RESULT_POINT, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndGuildCombatResult::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndGuildCombatResult::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndGuildCombatResult::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndGuildCombatResult::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndGuildCombatResult::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
void CWndGuildCombatResult::OnMouseMove(UINT nFlags, CPoint point )
{
}

void CWndGuildCombatResult::InsertGuildRate(CString str)
{
	CWndListBox* pWndList = (CWndListBox*)(m_WndGuildCombatTabResultRate.GetDlgItem( WIDC_LISTBOX1 ));
	pWndList->AddString( str );	
}

void CWndGuildCombatResult::InsertPersonRate(CString str)
{
	CWndListBox* pWndList = (CWndListBox*)(m_WndGuildCombatTabResultRate.GetDlgItem( WIDC_LISTBOX2 ));
	pWndList->AddString( str );		
}

void CWndGuildCombatResult::InsertLog(CString str)
{
//	CWndListBox* pWndList = (CWndListBox*)(m_WndGuildCombatTabResultLog.GetDlgItem( WIDC_LISTBOX1 ));
//	pWndList->AddString( str );	
	
	CWndText* pWndText = (CWndText*)(m_WndGuildCombatTabResultLog.GetDlgItem( WIDC_TEXT1 ));
	pWndText->AddString( str );			
//	pWndList->AddString( str );			
	
}


/****************************************************
  WndId : APP_GUILDCOMBAT_TAB_RESULT
****************************************************/
CWndGuildCombatTabResultRate::CWndGuildCombatTabResultRate() 
{ 
} 
CWndGuildCombatTabResultRate::~CWndGuildCombatTabResultRate() 
{ 
} 
void CWndGuildCombatTabResultRate::OnDraw( C2DRender* p2DRender ) 
{ 
} 

void CWndGuildCombatTabResultRate::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 
BOOL CWndGuildCombatTabResultRate::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_TAB_RESULT, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndGuildCombatTabResultRate::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
BOOL CWndGuildCombatTabResultRate::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
	
/****************************************************
  WndId : APP_GUILDCOMBAT_TAB_RESULT_LOG
****************************************************/
CWndGuildCombatTabResultLog::CWndGuildCombatTabResultLog() 
{ 
} 
CWndGuildCombatTabResultLog::~CWndGuildCombatTabResultLog() 
{ 
} 
void CWndGuildCombatTabResultLog::OnDraw( C2DRender* p2DRender ) 
{ 
} 

void CWndGuildCombatTabResultLog::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 
BOOL CWndGuildCombatTabResultLog::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_TAB_RESULT_LOG, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndGuildCombatTabResultLog::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
BOOL CWndGuildCombatTabResultLog::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BUTTON1 )
	{
		CTime time = CTime::GetCurrentTime();
		CString str;

		str.Format( "GuildLog_%d%.2d%.2d.txt", time.GetYear(), time.GetMonth(), time.GetDay() );

		CFileIO file;

		if( file.Open( str, _T( "wt" ) ) == FALSE )
			return FALSE;

		CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
				
		file.PutString( _T( pWndText->m_string ) ); 

		CString strTemp;
		strTemp.Format( prj.GetText(TID_GAME_GUILDCOMBAT_LOG_SAVE), str );

		g_WndMng.OpenMessageBox( strTemp );
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


/****************************************************
  WndId : APP_GUILDCOMBAT_RANK_P - 길드대전 랭킹창
  CtrlId : WIDC_TABCTRL1 - TabCtrl
****************************************************/
CWndGuildCombatRank_Person::CWndGuildCombatRank_Person() 
{ 
	m_mTotalRanking.clear();
} 
CWndGuildCombatRank_Person::~CWndGuildCombatRank_Person() 
{ 
} 
void CWndGuildCombatRank_Person::OnDraw( C2DRender* p2DRender ) 
{ 
} 

void CWndGuildCombatRank_Person::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	CRect rect = GetClientRect();
	rect.left = 0;
	rect.top = 0;
	//pWndTabCtrl->Create( WBS_NOFRAME, rect, this, 12 );
	
	pWndTabCtrl->SetButtonLength( 100 );

	m_WndGuildCombatTabClass_Tot.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILDCOMBAT_RANKINGCLASS );
	m_WndGuildCombatTabClass_Mer.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILDCOMBAT_RANKINGCLASS );
	m_WndGuildCombatTabClass_Mag.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILDCOMBAT_RANKINGCLASS );
	m_WndGuildCombatTabClass_Acr.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILDCOMBAT_RANKINGCLASS );
	m_WndGuildCombatTabClass_Ass.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILDCOMBAT_RANKINGCLASS );
	
	WTCITEM tabTabItem;
	
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_GAME_TOOLTIP_LOG1);
	tabTabItem.pWndBase = &m_WndGuildCombatTabClass_Tot;
	pWndTabCtrl->InsertItem( 0, &tabTabItem );

	tabTabItem.pszText = prj.m_aJob[ 1 ].szName;
	tabTabItem.pWndBase = &m_WndGuildCombatTabClass_Mer;
	pWndTabCtrl->InsertItem( 1, &tabTabItem );

	tabTabItem.pszText = prj.m_aJob[ 4 ].szName;
	tabTabItem.pWndBase = &m_WndGuildCombatTabClass_Mag;
	pWndTabCtrl->InsertItem( 2, &tabTabItem );
	
	tabTabItem.pszText = prj.m_aJob[ 2 ].szName;
	tabTabItem.pWndBase = &m_WndGuildCombatTabClass_Acr;
	pWndTabCtrl->InsertItem( 3, &tabTabItem );

	tabTabItem.pszText = prj.m_aJob[ 3 ].szName;
	tabTabItem.pWndBase = &m_WndGuildCombatTabClass_Ass;
	pWndTabCtrl->InsertItem( 4, &tabTabItem );


	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 
BOOL CWndGuildCombatRank_Person::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_RANK_P, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndGuildCombatRank_Person::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndGuildCombatRank_Person::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndGuildCombatRank_Person::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndGuildCombatRank_Person::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndGuildCombatRank_Person::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_CLOSE )
	{
		Destroy();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
void CWndGuildCombatRank_Person::OnMouseMove(UINT nFlags, CPoint point )
{
}

void CWndGuildCombatRank_Person::InsertRank( int nJob, u_long	uidPlayer, int nPoint )
{
	__GUILDCOMBAT_RANK_INFO GcRankInfo;
	GcRankInfo.nJob      = nJob;
	GcRankInfo.uidPlayer = uidPlayer;
	m_mTotalRanking.insert( make_pair( nPoint, GcRankInfo ) );
}

void CWndGuildCombatRank_Person::DivisionList()
{
	if( m_mTotalRanking.size() <= 0 )
		return;
	
	int nPoint;

	__GUILDCOMBAT_RANK_INFO GcRankInfo;
	CWndGuildCombatRank_Class * pRankTot;
	CWndGuildCombatRank_Class * pRank;

	// 전체목록에 등록
	pRankTot = &(m_WndGuildCombatTabClass_Tot);
	multimap< int, __GUILDCOMBAT_RANK_INFO >::reverse_iterator i;

	// 각지업별 등록
	pRank = NULL;
	for( i = m_mTotalRanking.rbegin(); i != m_mTotalRanking.rend(); ++i )
	{ 
		nPoint			= i->first;
		GcRankInfo		= i->second;
		
		pRank = __GetJobKindWnd( GcRankInfo.nJob );
		
		if( pRank == NULL )
			continue;

		pRank->InsertRank( GcRankInfo.nJob, GcRankInfo.uidPlayer, nPoint );
		pRankTot->InsertRank( GcRankInfo.nJob, GcRankInfo.uidPlayer, nPoint );
	}
}

void CWndGuildCombatRank_Person::UpdateList()
{

}

void CWndGuildCombatRank_Person::UpdatePlayer(int nJob, u_long idPlayer)
{

}

CWndGuildCombatRank_Class* CWndGuildCombatRank_Person::__GetJobKindWnd(int nJob)
{
	switch(nJob) 
	{
	case JOB_MERCENARY:
	case JOB_KNIGHT:
	case JOB_BLADE:
 	case JOB_KNIGHT_MASTER:
	case JOB_BLADE_MASTER:
	case JOB_KNIGHT_HERO:
	case JOB_BLADE_HERO:
		return &m_WndGuildCombatTabClass_Mer;
		break;
		
	case JOB_ACROBAT:
	case JOB_JESTER:
	case JOB_RANGER:
	case JOB_JESTER_MASTER:
	case JOB_RANGER_MASTER:
	case JOB_JESTER_HERO:
	case JOB_RANGER_HERO:
		return &m_WndGuildCombatTabClass_Acr;
		break;
		
	case JOB_ASSIST:
	case JOB_RINGMASTER:
	case JOB_BILLPOSTER:
	case JOB_RINGMASTER_MASTER:
	case JOB_BILLPOSTER_MASTER:
	case JOB_RINGMASTER_HERO:
	case JOB_BILLPOSTER_HERO:
		return &m_WndGuildCombatTabClass_Ass;
		break;
		
	case JOB_MAGICIAN:
	case JOB_PSYCHIKEEPER:
	case JOB_ELEMENTOR:
	case JOB_PSYCHIKEEPER_MASTER:
	case JOB_ELEMENTOR_MASTER:
	case JOB_PSYCHIKEEPER_HERO:
	case JOB_ELEMENTOR_HERO:
		return &m_WndGuildCombatTabClass_Mag;
		break;
		
	default:
		break;
	}	

	return NULL;
}

/****************************************************
  WndId : APP_GUILDCOMBAT_RANKINGCLASS
****************************************************/
CWndGuildCombatRank_Class::CWndGuildCombatRank_Class() 
{ 
	m_nRate = 0;
	m_nOldRate = -1;
	m_nMax = 0;
	m_nSelect = -1;	
} 
CWndGuildCombatRank_Class::~CWndGuildCombatRank_Class() 
{ 
} 
void CWndGuildCombatRank_Class::OnDraw( C2DRender* p2DRender ) 
{ 
	DWORD dwColor = D3DCOLOR_XRGB(0, 0, 0);
	int	sx, sy;
	char szNum[8], szCount[8];
	
	sx = 8;
	sy = 35;	
	
	CRect rc( sx, 5, sx+330, 7 ); 	
	p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 128, 0 , 0 , 0 ) );
	
	rc += CPoint( 0, 20 );
	p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 128, 0 , 0 , 0 ) );
	
	int nBase = m_wndScrollBar.GetScrollPos();
	if( nBase < 0 )
	{
		nBase = 0;
		TRACE("aa\n");
	}
	
	int nRanking = 0;
	int nOldRanking = -1;
	
	__GUILDCOMBAT_RANK_INFO2 GCRankInfo;
	__GUILDCOMBAT_RANK_INFO2 GCRankInfoMy;
	int	nMyRanking = 0;
	
	CString szName;
	CString szJob;

	BOOL	bMyRanking = FALSE;
	
	p2DRender->TextOut( sx + 4,      10, prj.GetText(TID_GAME_RATE),  dwColor );
	p2DRender->TextOut( sx + 40,     10, prj.GetText(TID_GAME_NAME),  dwColor );
	p2DRender->TextOut( sx + 180,    10, prj.GetText(TID_GAME_JOB),  dwColor );
	p2DRender->TextOut( sx + 275,    10, prj.GetText(TID_GAME_POINT),  dwColor );
	
	for( int k = 0; k < nBase; k++ )
	{
		GCRankInfo = m_listRank[k];

		if( nOldRanking != GCRankInfo.nPoint )
			nRanking++;

		if( GCRankInfo.uidPlayer == g_pPlayer->m_idPlayer ) 
		{
			bMyRanking   = TRUE;
			GCRankInfoMy = GCRankInfo;
			nMyRanking   = nRanking;
		}		

		nOldRanking = GCRankInfo.nPoint;					
	}
	
	for( int i=nBase; i<nBase + MAX_GUILDCOMBAT_RANK_PER_PAGE; ++i )
	{
		if( i >= m_nMax )	
			break;
		
		GCRankInfo = m_listRank[i];

		if( nOldRanking != GCRankInfo.nPoint )
			nRanking++;

		if( GCRankInfo.uidPlayer == g_pPlayer->m_idPlayer ) 
		{
			bMyRanking   = TRUE;
			GCRankInfoMy = GCRankInfo;
			nMyRanking   = nRanking;
		}

		sprintf( szNum, "%3d", nRanking );
		sprintf( szCount, "%2d", GCRankInfo.nPoint );
		
		if( m_nSelect >= 0 && i == m_nSelect )
		{
			rc.SetRect( sx, sy - 4, sx+320, sy + 16 ); 	
			p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 64, 0, 0, 0 ) );
		}
		
		if( i == 0 )
			dwColor = D3DCOLOR_XRGB( 200, 0, 0 );
		else
			dwColor = D3DCOLOR_XRGB( 0, 0, 0 );		
		
		if( nOldRanking != GCRankInfo.nPoint )
		{
			p2DRender->TextOut( sx + 4, sy, szNum,  dwColor );
		}
		else
		{
			p2DRender->TextOut( sx + 5, sy, "   ",  dwColor );
		}


#if __VER >= 11 // __SYS_PLAYER_DATA
		CString strName;
		const char* lpName	= CPlayerDataCenter::GetInstance()->GetPlayerString( GCRankInfo.uidPlayer );
		if( lpName )
			strName	= lpName;
#endif // __SYS_PLAYER_DATA

		if( i == 0 )
		{
#if __VER >= 11 // __SYS_PLAYER_DATA			
			p2DRender->TextOut( sx + 40,  sy, strName,  D3DCOLOR_XRGB( 200, 0, 0 ) );
#else // __SYS_PLAYER_DATA
			p2DRender->TextOut( sx + 40,  sy, GCRankInfo.strName,  D3DCOLOR_XRGB( 200, 0, 0 ) );
#endif // __SYS_PLAYER_DATA
			p2DRender->TextOut( sx + 180, sy, GCRankInfo.strJob,  D3DCOLOR_XRGB( 200, 0, 0 ) );
		}
		else
		{
#if __VER >= 11 // __SYS_PLAYER_DATA
			p2DRender->TextOut( sx + 40,  sy, strName,  dwColor );
#else // __SYS_PLAYER_DATA
			p2DRender->TextOut( sx + 40,  sy, GCRankInfo.strName,  dwColor );
#endif // __SYS_PLAYER_DATA
			p2DRender->TextOut( sx + 180, sy, GCRankInfo.strJob,    dwColor );
		}
		
		p2DRender->TextOut( sx + 275,  sy, szCount,    dwColor );
		
		sy += 18;
		
		nOldRanking = GCRankInfo.nPoint;					
	}	

	// 나의 랭킹 표시...
	if( bMyRanking )
	{
		sprintf( szNum, "%3d", nMyRanking );
		sprintf( szCount, "%2d", GCRankInfoMy.nPoint );
		
		dwColor = D3DCOLOR_XRGB( 0 , 255 , 255 );		
		
		p2DRender->TextOut( sx + 4,    250, szNum,  dwColor );

		p2DRender->TextOut( sx + 40,  250, GCRankInfoMy.strName,  dwColor );
		p2DRender->TextOut( sx + 180, 250, GCRankInfoMy.strJob,   dwColor );		
		p2DRender->TextOut( sx + 275, 250, szCount, dwColor );
	}
} 
// 선택된 인덱스를 얻는다.
int CWndGuildCombatRank_Class::GetSelectIndex( const CPoint& point )
{
	int nBase = m_wndScrollBar.GetScrollPos();
	int nIndex = (point.y - 32) / 18;
	
	if( 0 <= nIndex && nIndex < MAX_GUILDCOMBAT_RANK_PER_PAGE ) // 0 - 19사이 
	{
		int nSelect = nBase + nIndex;
		if( 0 <= nSelect && nSelect < m_nMax )
			return nSelect;
	}
	return -1;
}
BOOL CWndGuildCombatRank_Class::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
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

void CWndGuildCombatRank_Class::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
	if( m_nMax <= 0 )
		return;
	
	int nSelect = GetSelectIndex( point );
	if( nSelect != -1 )
		m_nSelect = nSelect;
} 

void CWndGuildCombatRank_Class::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	CRect rect = GetWindowRect();
	rect.top    += 30;
	rect.bottom -= 80;
	rect.left    = rect.right - 30;
	rect.right  -= 30;
	
	if( m_nMax > 0 )
	{
		m_wndScrollBar.SetScrollRange( 0, m_nMax );
		if( m_nMax < MAX_GUILDCOMBAT_RANK_PER_PAGE )
			m_wndScrollBar.SetScrollPage( m_nMax );
		else
			m_wndScrollBar.SetScrollPage( MAX_GUILDCOMBAT_RANK_PER_PAGE );
	}
	else
	{
		m_wndScrollBar.SetScrollRange( 0, 1 );
		m_wndScrollBar.SetScrollPage( 1 );
	}
	
	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );
	
	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 
BOOL CWndGuildCombatRank_Class::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_RANKINGCLASS, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndGuildCombatRank_Class::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
BOOL CWndGuildCombatRank_Class::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
void CWndGuildCombatRank_Class::InsertRank( int nJob, u_long uidPlayer, int nPoint )
{
	if( m_nMax >= MAX_GUILDCOMBAT_RANK )
	{
		Error( "CWndGuildCombatRank_Class::InsertRank - range over" );
		return;
	}

#if __VER >= 11 // __SYS_PLAYER_DATA
	m_listRank[m_nMax].strName    = CPlayerDataCenter::GetInstance()->GetPlayerString( uidPlayer );
#else	// __SYS_PLAYER_DATA
	m_listRank[m_nMax].strName    = prj.GetPlayerString( uidPlayer );
#endif	// __SYS_PLAYER_DATA
	m_listRank[m_nMax].strJob     = prj.m_aJob[ nJob ].szName;	
	m_listRank[m_nMax].uidPlayer  = uidPlayer;
	m_listRank[m_nMax].nPoint     = nPoint;
	
	m_nMax++;	

	if( m_nMax > 0 )
	{
		m_wndScrollBar.SetScrollRange( 0, m_nMax );
		if( m_nMax < MAX_GUILDCOMBAT_RANK_PER_PAGE )
			m_wndScrollBar.SetScrollPage( m_nMax );
		else
			m_wndScrollBar.SetScrollPage( MAX_GUILDCOMBAT_RANK_PER_PAGE );
	}
	else
	{
		m_wndScrollBar.SetScrollRange( 0, 1 );
		m_wndScrollBar.SetScrollPage( 1 );
	}		
}


/****************************************************
  WndId : APP_FONTEDIT
****************************************************/
CWndFontEdit::CWndFontEdit() 
{ 
	m_pTexture = NULL;
	m_pWndText = NULL;
	memset( m_ColorRect, 0, sizeof(CRect)*3 );	
} 
CWndFontEdit::~CWndFontEdit() 
{ 
} 
void CWndFontEdit::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	m_pTexture = m_textureMng.AddTexture( m_pApp->m_pd3dDevice,  MakePath( DIR_THEME, "yellowbuttten.tga" ), 0xffff00ff, TRUE );

	m_ColorRect[0].left   = 38;
	m_ColorRect[0].top    = 46;
	m_ColorRect[0].right  = 156;
	m_ColorRect[0].bottom = 63;
	
	m_ColorRect[1].left   = 38;
	m_ColorRect[1].top    = 68;
	m_ColorRect[1].right  = 156;
	m_ColorRect[1].bottom = 83;
	
	m_ColorRect[2].left   = 38;
	m_ColorRect[2].top    = 89;
	m_ColorRect[2].right  = 156;
	m_ColorRect[2].bottom = 103;

	m_ColorScrollBar[0] = 0;
	m_ColorScrollBar[1] = 0;
	m_ColorScrollBar[2] = 0;

	m_bLButtonClick = FALSE;

	m_fColor[0] = m_fColor[1] = m_fColor[2] = 0.0f;

	ReSetBar( m_fColor[0], m_fColor[1], m_fColor[2] );	
	
	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 
BOOL CWndFontEdit::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_FONTEDIT, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndFontEdit::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
BOOL CWndFontEdit::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( m_pWndText == NULL )
		return FALSE;

	CWndButton* pWndCheck = NULL;

	switch( nID )
	{
		case WIDC_CHECK1:
			pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK1);

			if( pWndCheck->GetCheck() )
			{
				m_pWndText->BlockSetStyle(ESSTY_BOLD);
//				m_pWndText->BlockInsert( "#b", "#nb" );
			}
			else
			{
				m_pWndText->BlockClearStyle(ESSTY_BOLD);
//				m_pWndText->BlockDelete( "#b", "#nb" );
			}
			break;
		case WIDC_CHECK2:
			pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK2);			

			if( pWndCheck->GetCheck() )
			{
				m_pWndText->BlockSetStyle(ESSTY_UNDERLINE);
//				m_pWndText->BlockInsert( "#u", "#nu" );				
			}
			else
			{
				m_pWndText->BlockClearStyle(ESSTY_UNDERLINE);
//				m_pWndText->BlockDelete( "#u", "#nu" );				
			}
			break;
		case WIDC_CHECK3:			
			pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK3);	
			
			if( pWndCheck->GetCheck() )
			{
				m_pWndText->BlockSetStyle(ESSTY_STRIKETHROUGH);
//				m_pWndText->BlockInsert( "#s", "#ns" );				
			}
			else
			{
				m_pWndText->BlockClearStyle(ESSTY_STRIKETHROUGH);
//				m_pWndText->BlockDelete( "#s", "#ns" );				
			}
			break;
		case WIDC_CHECK4:			
			pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK4);	

			if( pWndCheck->GetCheck() )
			{
				DWORD dwR, dwG, dwB, dwColor;
				dwR = (DWORD)( m_fColor[0] * 255 );
				dwG = (DWORD)( m_fColor[1] * 255 );
				dwB = (DWORD)( m_fColor[2] * 255 );
				
				dwColor = D3DCOLOR_XRGB(dwR, dwG, dwB);
				
				CString str;
				str.Format( "#c%x", dwColor );
						
				m_pWndText->BlockSetColor( dwColor );
//				m_pWndText->BlockInsert( str, "#nc" );				
			}
			else
			{
				m_pWndText->BlockSetColor( 0xffffffff );//ADEILSON 0xff000000
//				m_pWndText->BlockDelete( "#c", "#nc" );				
			}

			break;
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndFontEdit::OnLButtonUp( UINT nFlags, CPoint point )
{
	m_bLButtonClick = FALSE;
}
void CWndFontEdit::OnLButtonDown( UINT nFlags, CPoint point )
{
	m_bLButtonClick = TRUE;
}
void CWndFontEdit::OnMouseWndSurface( CPoint point )
{
	if( g_pPlayer == NULL )
		return;
	
	CRect rect = CRect( 17, 17, 186, 148 );
	
	if( !rect.PtInRect( point ) )
		m_bLButtonClick = FALSE;
	
	for( int i=0; i<3; i++ )
	{
		CRect DrawRect = m_ColorRect[i];
		
		DrawRect.top    -= 22;
		DrawRect.bottom -= 22;
		
		if( DrawRect.PtInRect( point ) && m_bLButtonClick )
		{
			point.x = (point.x > DrawRect.right) ? DrawRect.right : point.x;
			
			LONG Width = DrawRect.right - DrawRect.left;
			LONG Pos   = point.x - DrawRect.left;
			
			FLOAT p = ((FLOAT)((FLOAT)Pos / (FLOAT)Width));
			
			D3DXVECTOR2 vec1= D3DXVECTOR2( 0.0f, 1.0f );
			D3DXVECTOR2 vec2= D3DXVECTOR2( 1.0f, 1.0f );
			D3DXVECTOR2 vec3;
			
			D3DXVec2Lerp( &vec3, &vec1, &vec2, p );
			
			m_fColor[i] = vec3.x;
			
			m_ColorScrollBar[i].x = point.x;
		}
	}
}

void CWndFontEdit::OnDraw( C2DRender* p2DRender )
{	
	for( int i=0; i<3; i++ )
	{
		CPoint pt = CPoint( m_ColorScrollBar[i].x - ( m_pTexture->m_size.cx / 2 ), m_ColorScrollBar[i].y );
		
		m_pTexture->Render( p2DRender, pt );
	}
/*	
	for( int j=0; j<3; j++ )
	{
		if( m_ColorScrollBar[j].x != m_OriginalColorScrollBar[j].x )
			m_pTexture->Render( p2DRender, CPoint( m_OriginalColorScrollBar[j].x - ( m_pTexture->m_size.cx / 2 ), m_OriginalColorScrollBar[j].y ), 160 );
	}
*/

	LPWNDCTRL lpFace = GetWndCtrl( WIDC_CUSTOM1 );

	DWORD dwR, dwG, dwB;
	dwR = (DWORD)( m_fColor[0] * 255 );
	dwG = (DWORD)( m_fColor[1] * 255 );
	dwB = (DWORD)( m_fColor[2] * 255 );

	p2DRender->RenderFillRect( lpFace->rect, D3DCOLOR_XRGB( dwR, dwG, dwB ) );
}	

void CWndFontEdit::ReSetBar( FLOAT r, FLOAT g, FLOAT b )
{
	FLOAT fR = ((r-0.0f)/(1.0f - 0.0f)) * 100.0f;
	FLOAT fG = ((g-0.0f)/(1.0f - 0.0f)) * 100.0f;
	FLOAT fB = ((b-0.0f)/(1.0f - 0.0f)) * 100.0f;
	
	m_ColorScrollBar[0].x = (LONG)( (((m_ColorRect[0].right-m_ColorRect[0].left) * fR) / 100.0f) + m_ColorRect[0].left );
	m_ColorScrollBar[0].y = m_ColorRect[0].top - 20;
	m_ColorScrollBar[1].x = (LONG)( (((m_ColorRect[1].right-m_ColorRect[1].left) * fG) / 100.0f) + m_ColorRect[1].left );
	m_ColorScrollBar[1].y = m_ColorRect[1].top - 20;
	m_ColorScrollBar[2].x = (LONG)( (((m_ColorRect[2].right-m_ColorRect[2].left) * fB) / 100.0f) + m_ColorRect[2].left );
	m_ColorScrollBar[2].y = m_ColorRect[2].top - 20;
}

#if __VER >= 8 // __CSC_VER8_3

CWndBuffStatus::CWndBuffStatus() 
{ 
	m_BuffIconViewOpt = g_Option.m_BuffStatusMode;
} 

CWndBuffStatus::~CWndBuffStatus() 
{ 
} 

void CWndBuffStatus::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	this->DelWndStyle(WBS_CAPTION);
	
	m_wndTitleBar.SetVisible( FALSE );

	SetBuffIconInfo();

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CPoint point( (rectRoot.right - rectRoot.left) / 3, (rectRoot.bottom - rectRoot.top) / 3);
	Move( point );
} 
BOOL CWndBuffStatus::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_BUFF_STATUS, WBS_NOFOCUS, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndBuffStatus::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

BOOL CWndBuffStatus::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndBuffStatus::OnLButtonUp( UINT nFlags, CPoint point )
{
	this->m_pWndFocus = this;
}

void CWndBuffStatus::OnLButtonDown( UINT nFlags, CPoint point )
{
	if(!GetHitTestResult())
	{
		if( g_WndMng.m_pWndWorld )
		{
			CRect rect = GetWindowRect( TRUE );
			CPoint p;
			p.x = point.x + rect.left;
			p.y = point.y + rect.top;
			g_WndMng.m_pWndWorld->m_bLButtonDown = TRUE;
			g_WndMng.m_pWndWorld->OnLButtonDown( nFlags, p );
			this->m_pWndFocus = g_WndMng.m_pWndWorld;
		}
	}
}

void CWndBuffStatus::OnRButtonUp( UINT nFlags, CPoint point )
{
}

void CWndBuffStatus::OnRButtonDown( UINT nFlags, CPoint point )
{
	if(!GetHitTestResult())
	{
		if( g_WndMng.m_pWndWorld )
		{
			CRect rect = GetWindowRect( TRUE );
			CPoint p;
			p.x = point.x + rect.left;
			p.y = point.y + rect.top;
			g_WndMng.m_pWndWorld->m_bRButtonDown = TRUE;
			g_WndMng.m_pWndWorld->OnRButtonDown( nFlags, p );
		}
	}
}

void CWndBuffStatus::OnMouseWndSurface( CPoint point )
{
}

void CWndBuffStatus::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	if(GetHitTestResult())
	{
		if(m_BuffIconViewOpt == 0)
			m_BuffIconViewOpt = 1;
		else if(m_BuffIconViewOpt == 1)
			m_BuffIconViewOpt = 0;
		g_Option.m_BuffStatusMode = m_BuffIconViewOpt;
		SetBuffIconInfo();
	}
	else
	{
		if( g_WndMng.m_pWndWorld )
		{
			CRect rect = GetWindowRect( TRUE );
			CPoint p;
			p.x = point.x + rect.left;
			p.y = point.y + rect.top;
			g_WndMng.m_pWndWorld->m_bLButtonDown = TRUE;
			g_WndMng.m_pWndWorld->OnLButtonDblClk( nFlags, p );
		}
	}
}

void CWndBuffStatus::OnRButtonDblClk( UINT nFlags, CPoint point )
{
	if(GetHitTestResult())
	{
		if(m_BuffIconViewOpt == 0)
			m_BuffIconViewOpt = 1;
		else if(m_BuffIconViewOpt == 1)
			m_BuffIconViewOpt = 0;
		g_Option.m_BuffStatusMode = m_BuffIconViewOpt;
		SetBuffIconInfo();
	}
	else
	{
		if( g_WndMng.m_pWndWorld )
		{
			CRect rect = GetWindowRect( TRUE );
			CPoint p;
			p.x = point.x + rect.left;
			p.y = point.y + rect.top;
			g_WndMng.m_pWndWorld->m_bRButtonDown = TRUE;
			g_WndMng.m_pWndWorld->OnRButtonDblClk( nFlags, p );
		}
	}
}

BOOL CWndBuffStatus::GetHitTestResult()
{
#ifdef __BUFF_1107
	BOOL rtn_val = FALSE;
	RECT rectHittest;
	CPoint ptMouse = GetMousePoint();
	ClientToScreen( &ptMouse );
	BUFFICON_INFO buffinfo;
	
	std::list<BUFFICON_INFO>::iterator it = m_pBuffIconInfo.begin();
	for( MAPBUFF::iterator i = g_pPlayer->m_buffs.m_mapBuffs.begin(); i!= g_pPlayer->m_buffs.m_mapBuffs.end(); ++i )
	{
		IBuff* pBuff	= i->second;
		if( pBuff->GetType() == BUFF_SKILL )
		{
			buffinfo  = *it;
			SetRect( &rectHittest, buffinfo.pt.x, buffinfo.pt.y, buffinfo.pt.x+34, buffinfo.pt.y+34 );
			ClientToScreen( &rectHittest );
			if( PtInRect(&rectHittest, ptMouse ) )
				rtn_val = TRUE;
			it++;
		}
	}
	return rtn_val;
#else	// __BUFF_1107
//{{AFX
	BOOL rtn_val = FALSE;
	RECT rectHittest;
	int i;
	CPoint ptMouse = GetMousePoint();
	ClientToScreen( &ptMouse );
	BUFFICON_INFO buffinfo;
	
	std::list<BUFFICON_INFO>::iterator it = m_pBuffIconInfo.begin();
	for(i=0; i<MAX_SKILLINFLUENCE; i++)
	{
		SKILLINFLUENCE* pSkill = g_pPlayer->m_SkillState.Get(i);
		if(pSkill->wID)	
		{
			if(pSkill->wType == BUFF_SKILL)
			{
				buffinfo  = *it;
				SetRect( &rectHittest, buffinfo.pt.x, buffinfo.pt.y, buffinfo.pt.x+34, buffinfo.pt.y+34 );
				ClientToScreen( &rectHittest );
				if( PtInRect(&rectHittest, ptMouse ) )
				{
					rtn_val = TRUE;
				}
				it++;
			}
		}
	}
	return rtn_val;
//}}AFX
#endif	// __BUFF_1107
}

void CWndBuffStatus::SetBuffIconInfo()
{
	BUFFICON_INFO buffinfo;
	int x = 0;
	int y = 0;
	int i;
	CRect rect;
	rect = GetWindowRect(TRUE);
	
	if(!m_pBuffIconInfo.empty())
		m_pBuffIconInfo.clear();

	if(m_BuffIconViewOpt == 0)
	{
		for(i=0; i<MAX_SKILLBUFF_COUNT; i++)
		{
			buffinfo.pt = CPoint( x, y );
			m_pBuffIconInfo.push_back(buffinfo);
			x += 34;
			if(((i+1) % 7) == 0)
			{
				x  = 0;
				y += 34;
			}
		}
		//widht 238, heigth = 68
		rect.bottom = 136 + rect.top;
		rect.right = 238 + rect.left;
	}
	else if(m_BuffIconViewOpt == 1)
	{
		for(i=0; i<MAX_SKILLBUFF_COUNT; i++)
		{
			buffinfo.pt = CPoint( x, y );
			m_pBuffIconInfo.push_back(buffinfo);
			y += 34;
			if(((i+1) % 7) == 0)
			{
				y  = 0;
				x += 34;
			}
		}
		//widht 54, heigth = 238
		rect.bottom = 238 + rect.top;
		rect.right = 136 + rect.left;
	}
	SetWndRect(rect);
	AdjustWndBase();
}

void CWndBuffStatus::PaintFrame( C2DRender* p2DRender )
{
}

void CWndBuffStatus::OnDraw( C2DRender* p2DRender )
{	
	if( g_pPlayer == NULL )
		return;
	CPoint ptMouse = GetMousePoint();
	ClientToScreen( &ptMouse );
	BUFFICON_INFO buffinfo;

	std::list<BUFFICON_INFO>::iterator it = m_pBuffIconInfo.begin();
#ifdef __BUFF_1107
	for( MAPBUFF::iterator i = g_pPlayer->m_buffs.m_mapBuffs.begin(); i != g_pPlayer->m_buffs.m_mapBuffs.end(); ++i )
	{
		IBuff* pBuff	= i->second;
		if( pBuff->GetType() == BUFF_SKILL )
		{
			buffinfo  = *it;
			RenderBuffIcon( p2DRender, pBuff, TRUE, &buffinfo, ptMouse );
			it++;
		}
	}
#else	// __BUFF_1107
//{{AFX
	int i;
	for(i=0; i<MAX_SKILLINFLUENCE; i++)
	{
		SKILLINFLUENCE* pSkill = g_pPlayer->m_SkillState.Get(i);
		if(pSkill->wID)	
		{
			if(pSkill->wType == BUFF_SKILL)
			{
				buffinfo  = *it;
				RenderBuffIcon( p2DRender, pSkill, TRUE, &buffinfo, ptMouse );
				it++;
			}
		}
	}
//}}AFX
#endif	// __BUFF_1107
}

#ifdef __BUFF_1107
void CWndBuffStatus::RenderBuffIcon( C2DRender *p2DRender, IBuff* pBuff, BOOL bPlayer, BUFFICON_INFO* pInfo, CPoint ptMouse )
#else	// __BUFF_1107
void CWndBuffStatus::RenderBuffIcon( C2DRender *p2DRender, SKILLINFLUENCE* pSkill, BOOL bPlayer, BUFFICON_INFO* pInfo, CPoint ptMouse )
#endif	// __BUFF_1107
{
	int nTexture;
	RECT rectHittest;	
	multimap< DWORD, BUFFSKILL >::iterator iter;
	
	if( bPlayer )
		nTexture = 0;
	else
		nTexture = 1;
	
	ItemProp* pItem = NULL;

#ifdef __BUFF_1107
	WORD wID = pBuff->GetId();
	if( pBuff->GetType() == BUFF_SKILL )
	{
		iter = ((CWndWorld*)g_WndMng.m_pWndWorld)->m_pBuffTexture[0].find( pBuff->GetId() );

		if( iter == ((CWndWorld*)g_WndMng.m_pWndWorld)->m_pBuffTexture[0].end() )
			return;

		if( bPlayer )
			pItem = prj.GetSkillProp( pBuff->GetId() );
		else
			pItem = prj.GetPartySkill( pBuff->GetId() );
	}
	else
	{
		iter = ((CWndWorld*)g_WndMng.m_pWndWorld)->m_pBuffTexture[2].find( pBuff->GetId() );

		if( iter == ((CWndWorld*)g_WndMng.m_pWndWorld)->m_pBuffTexture[2].end() )
			return;

		pItem = prj.GetItemProp( wID );
	}
	
	multimap< DWORD, BUFFSKILL >::value_type* pp;
	pp = &(*iter);

	ASSERT( pItem );
	if( pp->second.m_pTexture == NULL )
		return;
	
	BOOL bFlash = FALSE;
	DWORD dwOddTime = 0;
	
	if( pBuff->GetTotal() > 0 )
	{	
		dwOddTime = pBuff->GetTotal() - ( g_tmCurrent - pBuff->GetInst() );
		bFlash = ( dwOddTime < 20 * 1000 );		// 20초 이하 남았으면 깜빡거림
		
		if( pBuff->GetTotal() < ( g_tmCurrent - pBuff->GetInst() ) ) // - 가 되면 0으로 처리
			dwOddTime = 0;
	}
	
	D3DXCOLOR color;
	
	if( bFlash )		
	{		
		if( pp->second.m_bFlsh == TRUE )
		{
			pp->second.m_nAlpha+=6;
			
			if( pp->second.m_nAlpha > 192 )
			{
				pp->second.m_nAlpha = 192;
				pp->second.m_bFlsh = FALSE;
			}
		}
		else
		{
			pp->second.m_nAlpha-=6;
			
			if( pp->second.m_nAlpha < 64 )
			{
				pp->second.m_nAlpha = 64;
				pp->second.m_bFlsh = TRUE;
			}
		}
		
		if( pItem->nEvildoing < 0 )							// 나쁜마법은
			color =  D3DCOLOR_ARGB( pp->second.m_nAlpha, 255, 120, 255 );		// 빨간 색 
		else
			color =  D3DCOLOR_ARGB( pp->second.m_nAlpha, 255, 255, 255 );
		
		p2DRender->RenderTexture2( pInfo->pt, pp->second.m_pTexture, 1.2f, 1.2f, color );		
	}
	else
	{
		if( pItem->nEvildoing < 0 )							// 나쁜마법은
			color =  D3DCOLOR_ARGB( 192, 255, 120, 255 );		// 빨간 색 
		else
			color =  D3DCOLOR_ARGB( 192, 255, 255, 255 );
		
		p2DRender->RenderTexture2( pInfo->pt, pp->second.m_pTexture, 1.2f, 1.2f, color );
	}

	SetRect( &rectHittest, pInfo->pt.x, pInfo->pt.y, pInfo->pt.x+28, pInfo->pt.y+28 );
	ClientToScreen( &rectHittest );
	
	CEditString strEdit;
	CString strLevel;
	strLevel.Format("   Lvl %d", pBuff->GetLevel() );

	if( pItem->dwItemRare == 102 )
	{
		strEdit.AddString( pItem->szName, D3DCOLOR_XRGB( 0, 93, 0 ), ESSTY_BOLD );
		strEdit.AddString( strLevel, D3DCOLOR_XRGB( 0, 93, 0 ), ESSTY_BOLD );
	}
	else if( pItem->dwItemRare == 103 )
	{
		strEdit.AddString( pItem->szName, D3DCOLOR_XRGB( 182, 0, 255 ), ESSTY_BOLD );
		strEdit.AddString( strLevel, D3DCOLOR_XRGB( 182, 0, 255 ), ESSTY_BOLD );
	}
	else
	{
		strEdit.AddString( pItem->szName, 0xff2fbe6d, ESSTY_BOLD );
		strEdit.AddString( strLevel, 0xff2fbe6d, ESSTY_BOLD );
	}

	CString str;

	if( pBuff->GetTotal() > 0 )
	{
		CTimeSpan ct( (long)(dwOddTime / 1000.0f) );		// 남은시간을 초단위로 변환해서 넘겨줌
				
		if( ct.GetDays() != 0 )
		{
			str.Format( "\n%.2d:%.2d:%.2d:%.2d", static_cast<int>(ct.GetDays()), ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );	//시분초 
		}
		else
		{
			if( ct.GetHours() >= 1 )
				str.Format( "\n%.2d:%.2d:%.2d", ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );	//시분초 
			else
				str.Format( "\n%.2d:%.2d", ct.GetMinutes(), ct.GetSeconds() );						// 분초
		}
		RenderOptBuffTime( p2DRender, pInfo->pt, ct, D3DCOLOR_XRGB( 0, 255, 255 ) );//ADEILSON
	}

	CString strTemp;
	strTemp.Format( "\n%s", pItem->szCommand );

	strEdit.AddString( strTemp );

	if( PtInRect(&rectHittest, ptMouse ) )
	{
		g_WndMng.PutDestParam( pItem->dwDestParam[0], pItem->dwDestParam[1],
			pItem->nAdjParamVal[0], pItem->nAdjParamVal[1], strEdit );

#if __VER >= 14 // __PREVENTION_TOOLTIP_BUG
		if( pBuff->GetType() == BUFF_SKILL && 
			pBuff->GetId() != SI_RIG_MASTER_BLESSING && 
			pBuff->GetId() != SI_ASS_CHEER_STONEHAND && 
			pBuff->GetId() != SI_MAG_EARTH_LOOTING && 
			pBuff->GetId() != SI_ASS_HEAL_PREVENTION ) //현자의 축복, 스톤 핸드, 루팅, 프리벤션 제외
#else // __PREVENTION_TOOLTIP_BUG
		if( pBuff->GetType() == BUFF_SKILL && pBuff->GetId() != SI_RIG_MASTER_BLESSING && 
			pBuff->GetId() != SI_ASS_CHEER_STONEHAND && pBuff->GetId() != SI_MAG_EARTH_LOOTING ) //현자의 축복, 스톤 핸드, 루팅 제외
#endif // __PREVENTION_TOOLTIP_BUG
		{
			AddSkillProp* pAddSkillProp = prj.GetAddSkillProp( pItem->dwSubDefine, pBuff->GetLevel() );

			if( pAddSkillProp )
			{
				g_WndMng.PutDestParam( pAddSkillProp->dwDestParam[0], pAddSkillProp->dwDestParam[1],
					pAddSkillProp->nAdjParamVal[0], pAddSkillProp->nAdjParamVal[1], strEdit );
			}
		}
	}
#else	// __BUFF_1107
//{{AFX
	WORD wID = pSkill->wID;
	if( pSkill->wType == BUFF_SKILL )
	{
		iter = ((CWndWorld*)g_WndMng.m_pWndWorld)->m_pBuffTexture[0].find(pSkill->wID);

		if( iter == ((CWndWorld*)g_WndMng.m_pWndWorld)->m_pBuffTexture[0].end() )
			return;

		if( bPlayer )
			pItem = prj.GetSkillProp( pSkill->wID );
		else
			pItem = prj.GetPartySkill( pSkill->wID );
	}
	else
	{
		iter = ((CWndWorld*)g_WndMng.m_pWndWorld)->m_pBuffTexture[2].find(pSkill->wID);

		if( iter == ((CWndWorld*)g_WndMng.m_pWndWorld)->m_pBuffTexture[2].end() )
			return;

		pItem = prj.GetItemProp( wID );
	}
	
	multimap< DWORD, BUFFSKILL >::value_type* pp;
	pp = &(*iter);

	ASSERT( pItem );
	if( pp->second.m_pTexture == NULL )
		return;
	
	BOOL bFlash = FALSE;
	DWORD dwOddTime = 0;
	
#if __VER >= 8 //__Y_FLAG_SKILL_BUFF
	if( pSkill->tmCount > 0 )
	{	
#endif //__Y_FLAG_SKILL_BUFF
		dwOddTime = pSkill->tmCount - (g_tmCurrent - pSkill->tmTime);
		bFlash = ( dwOddTime < 20 * 1000 );		// 20초 이하 남았으면 깜빡거림
		
		if(pSkill->tmCount < (g_tmCurrent - pSkill->tmTime)) // - 가 되면 0으로 처리
			dwOddTime = 0;
#if __VER >= 8 //__Y_FLAG_SKILL_BUFF
	}
#endif //__Y_FLAG_SKILL_BUFF
	
	D3DXCOLOR color;
	
	if( bFlash )		
	{		
		if( pp->second.m_bFlsh == TRUE )
		{
			pp->second.m_nAlpha+=6;
			
			if( pp->second.m_nAlpha > 192 )
			{
				pp->second.m_nAlpha = 192;
				pp->second.m_bFlsh = FALSE;
			}
		}
		else
		{
			pp->second.m_nAlpha-=6;
			
			if( pp->second.m_nAlpha < 64 )
			{
				pp->second.m_nAlpha = 64;
				pp->second.m_bFlsh = TRUE;
			}
		}
		
		if( pItem->nEvildoing < 0 )							// 나쁜마법은
			color =  D3DCOLOR_ARGB( pp->second.m_nAlpha, 255, 120, 255 );		// 빨간 색 
		else
			color =  D3DCOLOR_ARGB( pp->second.m_nAlpha, 255, 255, 255 );
		
		p2DRender->RenderTexture2( pInfo->pt, pp->second.m_pTexture, 1.2f, 1.2f, color );		
	}
	else
	{
		if( pItem->nEvildoing < 0 )							// 나쁜마법은
			color =  D3DCOLOR_ARGB( 192, 255, 120, 255 );		// 빨간 색 
		else
			color =  D3DCOLOR_ARGB( 192, 255, 255, 255 );
		
		p2DRender->RenderTexture2( pInfo->pt, pp->second.m_pTexture, 1.2f, 1.2f, color );
	}

	SetRect( &rectHittest, pInfo->pt.x, pInfo->pt.y, pInfo->pt.x+28, pInfo->pt.y+28 );
	ClientToScreen( &rectHittest );
	
	CEditString strEdit;
	CString strLevel;
	strLevel.Format("   Lv %d", pSkill->dwLevel);

	if( pItem->dwItemRare == 102 )
	{
		strEdit.AddString( pItem->szName, D3DCOLOR_XRGB( 0, 93, 0 ), ESSTY_BOLD );
		strEdit.AddString( strLevel, D3DCOLOR_XRGB( 0, 93, 0 ), ESSTY_BOLD );
	}
	else if( pItem->dwItemRare == 103 )
	{
		strEdit.AddString( pItem->szName, D3DCOLOR_XRGB( 182, 0, 255 ), ESSTY_BOLD );
		strEdit.AddString( strLevel, D3DCOLOR_XRGB( 182, 0, 255 ), ESSTY_BOLD );
	}
	else
	{
		strEdit.AddString( pItem->szName, 0xff2fbe6d, ESSTY_BOLD );
		strEdit.AddString( strLevel, 0xff2fbe6d, ESSTY_BOLD );
	}

	CString str;

#if __VER >= 8 //__Y_FLAG_SKILL_BUFF
	if( pSkill->tmCount > 0 )
#endif //__Y_FLAG_SKILL_BUFF
	{
		CTimeSpan ct( (long)(dwOddTime / 1000.0f) );		// 남은시간을 초단위로 변환해서 넘겨줌
				
		if( ct.GetDays() != 0 )
		{
			str.Format( "\n%.2d:%.2d:%.2d:%.2d", static_cast<int>(ct.GetDays()), ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );	//시분초 
		}
		else
		{
			if( ct.GetHours() >= 1 )
				str.Format( "\n%.2d:%.2d:%.2d", ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );	//시분초 
			else
				str.Format( "\n%.2d:%.2d", ct.GetMinutes(), ct.GetSeconds() );						// 분초
		}
		RenderOptBuffTime( p2DRender, pInfo->pt, ct, D3DCOLOR_XRGB( 255, 255, 255 ) );//adeilson D3DCOLOR_XRGB( 0, 0, 0 ) );
	}

	CString strTemp;
	strTemp.Format( "\n%s", pItem->szCommand );

	strEdit.AddString( strTemp );

	if( PtInRect(&rectHittest, ptMouse ) )
	{
		g_WndMng.PutDestParam( pItem->dwDestParam[0], pItem->dwDestParam[1],
			pItem->nAdjParamVal[0], pItem->nAdjParamVal[1], strEdit );
		
	#if __VER >= 8 //__Y_FLAG_SKILL_BUFF
		if( pSkill->wType == BUFF_SKILL && pSkill->wID != SI_RIG_MASTER_BLESSING && 
			pSkill->wID != SI_ASS_CHEER_STONEHAND && pSkill->wID != SI_MAG_EARTH_LOOTING ) //현자의 축복, 스톤 핸드, 루팅 제외
		{
			AddSkillProp* pAddSkillProp = prj.GetAddSkillProp( pItem->dwSubDefine, pSkill->dwLevel );

			if( pAddSkillProp )
			{
				g_WndMng.PutDestParam( pAddSkillProp->dwDestParam[0], pAddSkillProp->dwDestParam[1],
					pAddSkillProp->nAdjParamVal[0], pAddSkillProp->nAdjParamVal[1], strEdit );
			}
		}
	#endif //__Y_FLAG_SKILL_BUFF		
	}
//}}AFX
#endif	// __BUFF_1107

	strEdit.AddString( str );

	g_toolTip.PutToolTip( wID, strEdit, rectHittest, ptMouse, 1);

	++pInfo->nCount;
}

void CWndBuffStatus::RenderOptBuffTime(C2DRender *p2DRender, CPoint& point, CTimeSpan &ct, DWORD dwColor )
{
	if(g_Option.m_bVisibleBuffTimeRender)
	{
		CString str;
		int seconds = (int)(ct.GetTotalSeconds());
		str.Format( "%d" , seconds );
		p2DRender->TextOut(point.x+2, point.y+22, str, dwColor, 0xFF000000);
	}
}

#endif //__CSC_VER8_3

#if __VER >= 9 // __CSC_VER9_1
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndMixJewel
////////////////////////////////////////////////////////////////////////////////////////////////////////////
CWndMixJewel::CWndMixJewel() 
{
	m_nSelectCtrl = -1;
	m_nOrichalcum = 0;
	m_nMoonstone = 0;
	m_pText = NULL;
	m_nitemcount = 0;
	m_pWndInventory = NULL;
	m_pConfirm = NULL;
	m_bStart = FALSE;

	// cr : uw : initialize
	memset( m_MatJewel, 0, sizeof(GENMATDIEINFO) * MAX_JEWEL );
	memset( m_ItemInfo, 0, sizeof(ItemCountSet) * MAX_JEWEL );
}

CWndMixJewel::~CWndMixJewel() 
{ 
	DeleteDeviceObjects();
	SAFE_DELETE(m_pConfirm);	// if(m_pConfirm != NULL)
} 

void CWndMixJewel::OnDestroy()
{
	for(int i=0; i<MAX_JEWEL; i++)
	{
		if(m_MatJewel[i].pItemElem != NULL)
		{
			if( !g_pPlayer->m_vtInfo.IsTrading( m_MatJewel[i].pItemElem ) )
				m_MatJewel[i].pItemElem->SetExtra(0);
		}
	}
	if(m_pWndInventory != NULL)
		m_pWndInventory->m_wndItemCtrl.SetDieFlag(FALSE);
}

void CWndMixJewel::SerializeRegInfo( CAr& ar, DWORD& dwVersion )
{
	CWndNeuz::SerializeRegInfo( ar, dwVersion );
}

void CWndMixJewel::OnDestroyChildWnd( CWndBase* pWndChild )
{
	if( m_pConfirm == pWndChild )
		SAFE_DELETE( m_pConfirm );
}

void CWndMixJewel::OnDraw( C2DRender* p2DRender ) 
{ 
	//Render Icon
	ItemProp* pItemProp;
	CTexture* pTexture;
	BOOL bCheckSlot = TRUE;
	
	CPoint point = GetMousePoint();
	int testnum = HitTest( point );
	if( testnum != -1)
	{
		if( CWndBase::m_GlobalShortcut.m_dwData )
		{
			m_nSelectCtrl = testnum;
			CRect rect;
			rect = m_MatJewel[m_nSelectCtrl].wndCtrl->rect;
			p2DRender->RenderFillRect( rect, 0x60ffff00 );
		}
	}
	else
		m_nSelectCtrl = -1;
	
	for(int i=0; i<MAX_JEWEL; i++)
	{
		if(m_MatJewel[i].isUse)
		{
			if(m_MatJewel[i].pItemElem)
			{
				pItemProp = prj.GetItemProp( m_MatJewel[i].pItemElem->GetProp()->dwID );
				if(pItemProp != NULL)
				{
					pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
					if(pTexture != NULL)
						pTexture->Render( p2DRender, CPoint( m_MatJewel[i].wndCtrl->rect.left, m_MatJewel[i].wndCtrl->rect.top ) );
				}
			}
		}
		else
		{
			if( i < 5 )
				pItemProp = prj.GetItemProp( II_GEN_MAT_ORICHALCUM01 );
			else 
				pItemProp = prj.GetItemProp( II_GEN_MAT_MOONSTONE );
			if(pItemProp != NULL)
			{
				pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
				if(pTexture != NULL)
					pTexture->Render( p2DRender, CPoint( m_MatJewel[i].wndCtrl->rect.left, m_MatJewel[i].wndCtrl->rect.top ), 50 );
				bCheckSlot = FALSE;
			}
		}
	}

	if(!m_bStart)
		SetStartBtn(bCheckSlot);
	else
		SetStartBtn(FALSE);
}

void CWndMixJewel::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	//Ctrl Initialize.
	m_pText = (CWndText *)GetDlgItem( WIDC_TEXT1 );

	int StaticID[20] = {WIDC_PIC_SLOT1, WIDC_PIC_SLOT2, WIDC_PIC_SLOT3, WIDC_PIC_SLOT4, WIDC_PIC_SLOT5, 
						WIDC_PIC_SLOT6,	WIDC_PIC_SLOT7, WIDC_PIC_SLOT8, WIDC_PIC_SLOT9, WIDC_PIC_SLOT10};

	for(int i=0; i<MAX_JEWEL; i++)
	{
		m_MatJewel[i].wndCtrl = GetWndCtrl( StaticID[i] );
		m_MatJewel[i].staticNum = StaticID[i];
		m_MatJewel[i].isUse = FALSE;
		m_MatJewel[i].pItemElem = NULL;
		m_ItemInfo[i].extracount = 0;
		m_ItemInfo[i].itemid = -1;
	}

	SetDescription(NULL);

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);

	if(::GetLanguage() == LANG_FRE)
		pButton->SetTexture(g_Neuz.m_pd3dDevice, MakePath( "Theme\\", ::GetLanguage(), _T( "ButtNormal00.tga" ) ), TRUE);

	pButton->EnableWindow(FALSE);

	m_pWndInventory = (CWndInventory*)GetWndBase( APP_INVENTORY );
	if(m_pWndInventory != NULL)
	{
		m_pWndInventory->m_wndItemCtrl.SetDieFlag(TRUE);
	}
	MoveParentCenter();
}

void CWndMixJewel::SetDescription( CHAR* szChar )
{
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "SmeltMixJewel.inc" ) ));
	szChar = scanner.m_pProg;
	
	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString( szChar );
		m_pText->ResetString();	
	}
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndMixJewel::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_SMELT_MIXJEWEL, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndMixJewel::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndMixJewel::OnSize( UINT nType, int cx, int cy )
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndMixJewel::OnMouseMove( UINT nFlags, CPoint point )
{
}

void CWndMixJewel::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndMixJewel::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
}

void CWndMixJewel::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	int choicenum = HitTest( point );
	if(choicenum > -1 && m_MatJewel[choicenum].isUse)
	{
		m_MatJewel[choicenum].isUse = FALSE;

		if(choicenum >= 0 && choicenum < 10)
			m_nOrichalcum--;
		else if(choicenum > 9 && choicenum < 20)
			m_nMoonstone--;

		m_MatJewel[choicenum].pItemElem->SetExtra(m_MatJewel[choicenum].pItemElem->GetExtra()-1);
	}
}

void CWndMixJewel::OnRButtonDblClk( UINT nFlags, CPoint point )
{
}

BOOL CWndMixJewel::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	int isValid = ITEM_INVALID;
	CItemElem* pItemElem  = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
	if( m_nSelectCtrl > -1 && !m_MatJewel[m_nSelectCtrl].isUse)
	{
		if( m_nSelectCtrl < 5 && (pItemElem->GetProp()->dwID == II_GEN_MAT_ORICHALCUM01 || pItemElem->GetProp()->dwID == II_GEN_MAT_ORICHALCUM01_1))
		{
			if( pItemElem->m_nItemNum > pItemElem->GetExtra() )
				isValid = ITEM_VALID;
			else
				isValid = ITEM_MAX_OVERFLOW;
		}
		else if( m_nSelectCtrl > 4 && m_nSelectCtrl < 10 && (pItemElem->GetProp()->dwID == II_GEN_MAT_MOONSTONE || pItemElem->GetProp()->dwID == II_GEN_MAT_MOONSTONE_1))
		{
			if( pItemElem->m_nItemNum > pItemElem->GetExtra() )
				isValid = ITEM_VALID;
			else
				isValid = ITEM_MAX_OVERFLOW;
		}

	}	
	switch (isValid)
	{
		case ITEM_VALID:
			SetJewel(pItemElem);
			break;					
		case ITEM_MAX_OVERFLOW:
			// Max overflow Error Msg.
			g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_SUMMONANGEL_ERROR1 ) );
			break;
		case ITEM_INVALID:
			break;
	}
	return TRUE;
}

void CWndMixJewel::SetJewel(CItemElem* pItemElem)
{
	int ptcount, extslotcount, usableitemnum, insertnum, i;
	int slotnum[10];
	ptcount = 0;
	extslotcount = 0;
	usableitemnum = 0;
	insertnum = 0;

	if(pItemElem != NULL)
	{
		if(m_nSelectCtrl > -1)
		{
			if(m_nSelectCtrl >=0 && m_nSelectCtrl < 5)
				ptcount = 0;
			else if(m_nSelectCtrl >= 5 && m_nSelectCtrl < 10)
				ptcount = 5;

			if(g_WndMng.m_pWndWorld->m_bShiftPushed)
			{
				int maxcount = ptcount + 5;
				for(i=ptcount; i<maxcount; i++)
				{
					if(!m_MatJewel[i].isUse)
					{
						slotnum[extslotcount] = i;
						extslotcount++;
					}
				}
				usableitemnum = pItemElem->m_nItemNum - pItemElem->GetExtra();
				
				if(extslotcount > 0 && usableitemnum > 0)
				{
					if(extslotcount > usableitemnum) //남은 Slot과 사용가능한 Item갯 수를 파악하여 한번에 넣을 갯 수를 지정.
						insertnum = usableitemnum;
					else
						insertnum = extslotcount;

					for(i=0; i<insertnum; i++)
					{
						m_MatJewel[slotnum[i]].isUse = TRUE;
						m_MatJewel[slotnum[i]].pItemElem = pItemElem;
					}
					pItemElem->SetExtra(pItemElem->GetExtra()+insertnum);
				}
			}
			else
			{
				insertnum++;
				m_MatJewel[m_nSelectCtrl].isUse = TRUE;
				m_MatJewel[m_nSelectCtrl].pItemElem = pItemElem;
				pItemElem->SetExtra(pItemElem->GetExtra()+insertnum);	
			}

			switch(ptcount)
			{
			case 0:
				m_nOrichalcum += insertnum;
				break;
			case 5:
				m_nMoonstone += insertnum;
				break;
			}
		}
		else //Inventory Dbl Clk...
		{
			int nSelect = -1;
			int count = 0;
			BOOL stopcheck = FALSE;
			if(pItemElem->GetProp()->dwID == II_GEN_MAT_ORICHALCUM01 || pItemElem->GetProp()->dwID == II_GEN_MAT_ORICHALCUM01_1)
				ptcount = 0;
			else if(pItemElem->GetProp()->dwID == II_GEN_MAT_MOONSTONE || pItemElem->GetProp()->dwID == II_GEN_MAT_MOONSTONE_1)
				ptcount = 5;

			count += ptcount;
			while(!stopcheck && count < ptcount + 5)
			{
				if(!m_MatJewel[count].isUse)
				{
					stopcheck = TRUE;
					nSelect = count;
				}
				count++;					
			}
			
			if(nSelect > -1)
			{
				m_MatJewel[nSelect].isUse = TRUE;
				m_MatJewel[nSelect].pItemElem = pItemElem;
				pItemElem->SetExtra(pItemElem->GetExtra()+1);
				if(ptcount == 0)
					m_nOrichalcum++;
				else if(ptcount == 5)
					m_nMoonstone++;
			}
		}
	}
}

BOOL CWndMixJewel::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_START )
	{
		if(	m_nOrichalcum + m_nMoonstone < 10 )
		{
			g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_SUMMONANGEL_ERROR2 ) );
		}
		else
		{	
			m_bStart = TRUE;
			
			m_nitemcount = 0;
			for(int i=0; i<MAX_JEWEL; i++)
			{
				if(m_MatJewel[i].isUse)
				{
					if(m_MatJewel[i].pItemElem != NULL && m_MatJewel[i].pItemElem->GetExtra() > 0)
					{
						BOOL equalflag = FALSE;
						for(int j=0; j<m_nitemcount; j++)
						{
							if(m_ItemInfo[j].itemid == m_MatJewel[i].pItemElem->m_dwObjId)
							{
								m_ItemInfo[j].extracount++;
								equalflag = TRUE;
							}
						}
						if(!equalflag)
						{
							m_ItemInfo[m_nitemcount].itemid = m_MatJewel[i].pItemElem->m_dwObjId;
							m_ItemInfo[m_nitemcount].extracount++;
							m_nitemcount++;
						}
					}
				}		
			}
			
			CString sendstr;
			OBJID itemobjid[MAX_JEWEL];
			int index = 0;

			for(i=0; i<m_nitemcount; i++)
			{
				for(int j=0; j<m_ItemInfo[i].extracount; j++)
				{
					itemobjid[index] = m_ItemInfo[i].itemid;
					index++;
				}
			}

			g_DPlay.SendUltimateMakeItem(itemobjid);
		}
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

int CWndMixJewel::HitTest( CPoint point )
{
	int rtn_val = -1;
	CRect rect;
	for(int i=0; i<MAX_JEWEL; i++)
	{
		rect = m_MatJewel[i].wndCtrl->rect;
		if( rect.PtInRect( point ) )
		{
			rtn_val = i;
			i = MAX_JEWEL;
		}
	}
	return rtn_val;
}

void CWndMixJewel::ReceiveResult(int nResult)
{
	//Server에서 결과를 받으면 초기화를 진행하고 창을 띄운다.
	//초기화
	for(int i=0; i<MAX_JEWEL; i++)
	{
		m_MatJewel[i].isUse = FALSE;
		m_MatJewel[i].pItemElem->SetExtra(0);
		m_MatJewel[i].pItemElem = NULL;
		m_ItemInfo[i].extracount = 0;
		m_ItemInfo[i].itemid = -1;
	}
	m_bStart = FALSE;
	
	switch(nResult) 
	{
		case CUltimateWeapon::ULTIMATE_SUCCESS:
			{
				SetStartBtn(FALSE);
				SAFE_DELETE( m_pConfirm );	// cr : uw : leak
				m_pConfirm = new CWndMixJewelConfirm();
				m_pConfirm->Initialize( this );		// cr : uw : 
			}
			break;
		case CUltimateWeapon::ULTIMATE_FAILED:
		case CUltimateWeapon::ULTIMATE_CANCEL:
			Destroy();
			break;
		case CUltimateWeapon::ULTIMATE_INVENTORY:
			g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_EXTRACTION_ERROR ) );
			break;			
	}
}

void CWndMixJewel::SetStartBtn(BOOL buse)
{
	CWndButton* pbutton = (CWndButton*)GetDlgItem(WIDC_START);
	pbutton->EnableWindow(buse);		
}

void CWndMixJewel::SetConfirmInit()
{
	m_pConfirm = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndMixJewelConfirm Class
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndMixJewelConfirm::CWndMixJewelConfirm() 
{ 
}
 
CWndMixJewelConfirm::~CWndMixJewelConfirm() 
{ 
} 

void CWndMixJewelConfirm::OnDestroy()
{
	CWndMixJewel* pWndMixJewel = (CWndMixJewel*)GetWndBase( APP_SMELT_MIXJEWEL );
	if(pWndMixJewel != NULL)
		pWndMixJewel->SetConfirmInit();
}

void CWndMixJewelConfirm::OnDraw( C2DRender* p2DRender ) 
{
	int itemID = II_GEN_MAT_ORICHALCUM02;
	ItemProp* pItemProp;
	CTexture* pTexture;

	pItemProp = prj.GetItemProp( itemID );
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_PIC_SLOT );
	if(pItemProp != NULL)
	{
		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
		if(pTexture != NULL)
			pTexture->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
	}
} 

void CWndMixJewelConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	CWndText* pText = (CWndText*)GetDlgItem(WIDC_TEXT1);
	pText->SetString(prj.GetText( TID_GAME_MIXJEWEL_SUCCESS ));
	
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndMixJewelConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_SMELT_MIXJEWEL_CONFIRM, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndMixJewelConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndMixJewelConfirm::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndMixJewelConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndMixJewelConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndMixJewelConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK )
		Destroy();

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndExtraction Class
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndExtraction::CWndExtraction() 
{
	m_pItemElem = NULL;
	m_pEItemProp = NULL;
}

CWndExtraction::~CWndExtraction() 
{ 
} 

void CWndExtraction::OnDestroy()
{
	if(m_pItemElem != NULL)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pItemElem ) )
			m_pItemElem->SetExtra(0);
	}
}

void CWndExtraction::OnDraw( C2DRender* p2DRender ) 
{
	ItemProp* pItemProp;
	CTexture* pTexture;
	
	if(m_pItemElem != NULL)
	{
		pItemProp = m_pItemElem->GetProp();
		LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_PIC_SLOT );
		if(pItemProp != NULL)
		{
			pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
			if(pTexture != NULL)
				pTexture->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
		}
	} 
}

void CWndExtraction::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);

	if(::GetLanguage() == LANG_FRE)
		pButton->SetTexture(g_Neuz.m_pd3dDevice, MakePath( "Theme\\", ::GetLanguage(), _T( "ButtNormal00.tga" ) ), TRUE);

	pButton->EnableWindow(FALSE);
	m_pText = (CWndText*)GetDlgItem( WIDC_TEXT1 );

	SetDescription(NULL);

	//Window Position
/*	CWndInventory* pWndInventory;
	pWndInventory = (CWndInventory*)GetWndBase( APP_INVENTORY );
	CRect rectInventory;
	if(pWndInventory != NULL)
	{
		pWndInventory->m_wndItemCtrl.SetDieFlag(TRUE);
		rectInventory = pWndInventory->GetWindowRect( TRUE );
	}
	CPoint ptInventory = rectInventory.TopLeft();
	CPoint ptMove;
	
	CRect rect = GetWindowRect( TRUE );
	
	if( ptInventory.x > rect.Width() / 2 )
		ptMove = ptInventory - CPoint( rect.Width(), 0 );
	else
		ptMove = ptInventory + CPoint( rectInventory.Width(), 0 );
	
	Move( ptMove );
*/
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndExtraction::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_SMELT_EXTRACTION, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndExtraction::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndExtraction::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndExtraction::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndExtraction::OnLButtonDown( UINT nFlags, CPoint point ) 
{
}

void CWndExtraction::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_PIC_SLOT );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{
		if(m_pItemElem)
		{
			m_pItemElem->SetExtra(0);
			CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
			pButton->EnableWindow(FALSE);
			m_pItemElem = NULL;
			m_pEItemProp = NULL;
		}
	}
}

BOOL CWndExtraction::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_PIC_SLOT );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{		
		//무기 아이템만 올려놓을 수 있다.
		ItemProp* pItemProp;
		CItemElem* pTempElem;
		pTempElem  = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
		
		if(m_pItemElem == NULL && pTempElem != NULL)
		{
			pItemProp = pTempElem->GetProp();
			if( pItemProp->dwItemKind1 == IK1_WEAPON )
			{
				if( (pItemProp->dwReferStat1 == WEAPON_GENERAL && pItemProp->dwLimitLevel1 >= 60) || pItemProp->dwReferStat1 == WEAPON_UNIQUE )
				{
					m_pItemElem = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
					m_pEItemProp = m_pItemElem->GetProp();
					m_pItemElem->SetExtra(m_pItemElem->GetExtra()+1);
					CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
					pButton->EnableWindow(TRUE);
				}
			}
		}
	}
	return TRUE;
}

void CWndExtraction::SetWeapon(CItemElem* pItemElem)
{
	if(m_pItemElem == NULL && pItemElem != NULL)
	{
		ItemProp* pProp = pItemElem->GetProp();
		
		if( pProp->dwItemKind1 == IK1_WEAPON )
		{
			if( (pProp->dwReferStat1 == WEAPON_GENERAL && pProp->dwLimitLevel1 >= 60) || pProp->dwReferStat1 == WEAPON_UNIQUE )
			{
				m_pItemElem = pItemElem;
				m_pEItemProp = m_pItemElem->GetProp();
				m_pItemElem->SetExtra(m_pItemElem->GetExtra()+1);
				CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
				pButton->EnableWindow(TRUE);
			}
		}
	}
}

BOOL CWndExtraction::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_START )
	{
		//서버로 시작을 알린다.
		if(m_pItemElem != NULL)
		{
			CWndButton* pButton;
			pButton = (CWndButton*)GetDlgItem( WIDC_START );
			pButton->EnableWindow(FALSE);

			g_DPlay.SendUltimateMakeGem(m_pItemElem->m_dwObjId);
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndExtraction::SetDescription( CHAR* szChar )
{
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "SmeltExtraction.inc" ) ));
	szChar = scanner.m_pProg;
	
	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString( szChar );
		m_pText->ResetString();	
	}
}

void CWndExtraction::ReceiveResult(int result, int nCount)
{
	//결과 값에 따른 처리
	//1. 성공 : Destroy Window
	//2. 실패 : Destroy Window
	//3. 취소 : 인벤토리 여유 공간 부족 및 기타 에러 사항.
	
	// ULTIMATE_SUCCESS : 성공
	// ULTIMATE_FAILED : 실패
	// ULTIMATE_CANCEL : 예외 처리
	// ULTIMATE_ISULTIMATE : 무기가 아니거나 일반, 유니크가 아닐 때
	// ULTIMATE_INVENTORY : 인벤토리가 부족 할 때
	// ULTIMATE_ISNOTULTIMATE : 얼터멋 웨폰이 아닐 때

	
	switch(result) 
	{
		case CUltimateWeapon::ULTIMATE_SUCCESS:
			{
				CString message;
				ItemProp* pItemProp = prj.GetItemProp( prj.m_UltimateWeapon.GetGemKind(m_pEItemProp->dwLimitLevel1) );
				message.Format(prj.GetText( TID_GAME_EXTRACTION_SUCCESS ), pItemProp->szName, nCount);
				g_WndMng.OpenMessageBox( message );
				m_pItemElem = NULL;
			}
			break;
		case CUltimateWeapon::ULTIMATE_FAILED:
		case CUltimateWeapon::ULTIMATE_CANCEL:
			g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_EXTRACTION_FAILED ) );
			Destroy();
			break;
		case CUltimateWeapon::ULTIMATE_INVENTORY:
			g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_EXTRACTION_ERROR ) );

			CWndButton* pButton;
			pButton = (CWndButton*)GetDlgItem( WIDC_START );
			pButton->EnableWindow(TRUE);
			break;			
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndSmeltJewel Class
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndSmeltJewel::CWndSmeltJewel()
{
	m_pMainItem = NULL;
	m_pJewelElem = NULL;
	m_pItemElem = NULL;
	m_pText = NULL;
	m_nJewelCount = 0;
	m_nUsableSlot = -1;
	m_objJewelId = -1;
	
	for(int i=0; i<5; i++)
	{
		m_nJewelSlot[i] = -1;
		m_dwJewel[i] = -1;
	}
	m_fRotate = 0.0f;
	m_fAddRot = 2.0f;
	
	m_nStatus = 0;
	m_nCount = 0;
	m_nDelay = 25;
	m_nAlpha = 0;
	m_nEyeYPos = 0.0f;
	m_bFlash = FALSE;
}

CWndSmeltJewel::~CWndSmeltJewel()
{
// Todo::Model 초기화 필요
//	if(m_pMainItem != NULL)
//		SAFE_DELETE(m_pMainItem);
}

void CWndSmeltJewel::OnDestroy()
{
	if(m_pJewelElem)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pJewelElem ) )
			m_pJewelElem->SetExtra(0);
	}
	if(m_pItemElem != NULL)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pItemElem ) )
			m_pItemElem->SetExtra(0);		
	}
}

void CWndSmeltJewel::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	// 윈도를 중앙으로 옮기는 부분.
	m_nJewelSlot[0] = WIDC_STATIC1;
	m_nJewelSlot[1] = WIDC_STATIC2;
	m_nJewelSlot[2] = WIDC_STATIC3;
	m_nJewelSlot[3] = WIDC_STATIC4;
	m_nJewelSlot[4] = WIDC_STATIC5;

	for(int i=0; i<5; i++)
	{
		CWndStatic* pStatic = (CWndStatic*)GetDlgItem(m_nJewelSlot[i]);
		pStatic->EnableWindow(FALSE);
	}

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
	if(::GetLanguage() == LANG_FRE)
		pButton->SetTexture(g_Neuz.m_pd3dDevice, MakePath( "Theme\\", ::GetLanguage(), _T( "ButtNormal00.tga" ) ), TRUE);

	pButton->EnableWindow(FALSE);

	m_pText = (CWndText *)GetDlgItem( WIDC_TEXT1 );

	SetDescription(NULL);

	MoveParentCenter();
} 

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndSmeltJewel::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_SMELT_JEWEL, 0, CPoint( 0, 0 ), pWndParent );
} 


BOOL CWndSmeltJewel::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
}

void CWndSmeltJewel::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
}

void CWndSmeltJewel::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
}

void CWndSmeltJewel::OnLButtonDown( UINT nFlags, CPoint point ) 
{
}

BOOL CWndSmeltJewel::Process()
{
	if(m_pItemElem != NULL)
	{
		CWndButton* pButton;
		pButton = (CWndButton*)GetDlgItem(WIDC_START);
		
		if(m_nStatus != 1)
		{
			if( (m_nUsableSlot >= 0 && m_nUsableSlot < 5) && m_dwJewel[m_nUsableSlot] != -1)
				pButton->EnableWindow(TRUE);
			else
				pButton->EnableWindow(FALSE);
		}
		else
			pButton->EnableWindow(FALSE);
	}

	if(m_nStatus == 1) //Start!
	{
		if(m_nStatus == 1) //Start버튼 누를 경우 컴퓨터의 선택이 회전하도록 함.
		{
			if(m_nCount > m_nDelay)
			{
				m_fAddRot += 4.0f;
				//( m_fRotate < 1 ) ? m_fRotate = 1 : m_fRotate;
				
				if(m_nDelay <= 25 && m_nDelay > 16) //회전이 끊기는 부분에서 플래쉬를 주자.
					m_bFlash = TRUE;
				
				if(m_nDelay < 10)
				{
					m_nEyeYPos -= 0.4f;
					m_nAlpha += 18;
					(m_nAlpha > 230 ) ? m_nAlpha = 230 : m_nAlpha;
				}

				m_nDelay -= 1;				
				if(m_nDelay < 0)
				{
					m_nStatus = 0;
					m_nDelay = 25;
					m_fRotate = 0.0f;
					m_fAddRot = 2.0f;
					m_nAlpha = 0;
					m_nEyeYPos = 0.0f;
					
					if(m_pItemElem != NULL && m_objJewelId != -1)
						g_DPlay.SendUltimateSetGem(m_pItemElem->m_dwObjId, m_objJewelId);
				}
				
				m_nCount = 0;
			}
			m_nCount++;
		}
	}

	return TRUE;
}

void CWndSmeltJewel::OnDraw( C2DRender* p2DRender ) 
{ 
	if( m_pItemElem == NULL )
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

	pd3dDevice->SetRenderState( D3DRS_AMBIENT,  D3DCOLOR_ARGB( 255,255,255,255 ) );
	
	CRect rect = GetClientRect();

	// 뷰포트 세팅 
	D3DVIEWPORT9 viewport;

	// 월드 
	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 matScale;
	D3DXMATRIXA16 matRot;
	D3DXMATRIXA16 matTrans;

	// Color
	D3DXCOLOR color;
	
	// 초기화
	D3DXMatrixIdentity(&matScale);
	D3DXMatrixIdentity(&matTrans);
	D3DXMatrixIdentity(&matWorld);
	
	D3DXVECTOR3 vEyePt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);

	// 무기에 따른 Camera Angle조정
	if(m_pItemElem->GetProp()->dwItemKind3 == IK3_AXE ||
		m_pItemElem->GetProp()->dwItemKind3 == IK3_SWD)
	{
		if(m_pItemElem->GetProp()->dwHanded == HD_ONE)
		{
			vEyePt.x = 0.0f;
			vEyePt.y = 3.0f;
			vEyePt.z = 0.0f;
			
			vLookatPt.x = 0.6f;
			vLookatPt.y = -0.2f;
			vLookatPt.z = 0.0f;
		}
		else if(m_pItemElem->GetProp()->dwHanded == HD_TWO)
		{
			vEyePt.x = 1.0f;
			vEyePt.y = 5.0f;
			vEyePt.z = 0.0f;
			
			vLookatPt.x = 1.2f;
			vLookatPt.y = -0.2f;
			vLookatPt.z = 0.0f;
		}
	}
	else if(m_pItemElem->GetProp()->dwItemKind3 == IK3_YOYO ||
			m_pItemElem->GetProp()->dwItemKind3 == IK3_KNUCKLEHAMMER ||
#ifdef __CROSSBOW
			m_pItemElem->GetProp()->dwItemKind3 == IK3_CROSSBOW ||
#endif // __CROSSBOW
			m_pItemElem->GetProp()->dwItemKind3 == IK3_BOW)
	{
		vEyePt.x = 0.0f;
		vEyePt.y = 3.0f;
		vEyePt.z = 0.0f;
		
		vLookatPt.x = 0.01f;
		vLookatPt.y = -0.2f;
		vLookatPt.z = 0.0f;
	}
	else if(m_pItemElem->GetProp()->dwItemKind3 == IK3_WAND)
	{
		vEyePt.x = 0.0f;
		vEyePt.y = 3.0f;
		vEyePt.z = 0.0f;
		
		vLookatPt.x = 0.4f;
		vLookatPt.y = -0.2f;
		vLookatPt.z = 0.0f;
	}
	else if(m_pItemElem->GetProp()->dwItemKind3 == IK3_CHEERSTICK ||
			m_pItemElem->GetProp()->dwItemKind3 == IK3_STAFF)
	{
		vEyePt.x = 0.0f;
		vEyePt.y = 4.0f;
		vEyePt.z = 0.0f;
		
		vLookatPt.x = 0.01f;
		vLookatPt.y = -0.2f;
		vLookatPt.z = 0.0f;
	}

	if(m_nStatus == 1) //Start후 이미지 회전 시 시점을 당기기 위함.
	{
		vEyePt.y += m_nEyeYPos;
	}

	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);
	pd3dDevice->SetTransform(D3DTS_VIEW, &matView);
	
    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 100.0f );
    pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	// Rotation
	m_fRotate += m_fAddRot;
	D3DXMatrixRotationX(&matRot, D3DXToRadian( m_fRotate ) );
	
	// Scaling
	D3DXMatrixScaling(&matScale, 1.5f, 1.5f, 1.5f);	

	LPWNDCTRL lpFace = GetWndCtrl( WIDC_CUSTOM1 );

	// 렌더링 위치 설정
	viewport.X      = p2DRender->m_ptOrigin.x + lpFace->rect.left;
	viewport.Y      = p2DRender->m_ptOrigin.y + lpFace->rect.top;
	viewport.Width  = lpFace->rect.Width();
	viewport.Height = lpFace->rect.Height();

	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;
	pd3dDevice->SetViewport(&viewport);
	pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xffa08080, 1.0f, 0 );

	// Matrix Multiply
	D3DXMatrixMultiply(&matWorld,&matWorld,&matScale);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matRot );
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	
	// 랜더링 
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		
	::SetLight( FALSE );
	::SetFog( FALSE );
	SetDiffuse( 1.0f, 1.0f, 1.0f );
	SetAmbient( 1.0f, 1.0f, 1.0f );

	D3DXVECTOR4 vConst( 1.0f, 1.0f, 1.0f, 1.0f );
	pd3dDevice->SetVertexShaderConstantF( 95, (float*)&vConst, 1 );
	::SetTransformView( matView );
	::SetTransformProj( matProj );
	
/*	if(m_nStatus == 1) //깜빡임.
	{
		if(m_bFlash)
		{
			color =  D3DCOLOR_ARGB( 255, 255, 255, 255 );
			p2DRender->RenderFillRect( rect, color );
			m_bFlash = FALSE;
		}
	}
*/
	m_pMainItem->Render( p2DRender->m_pd3dDevice, &matWorld );

	viewport.X      = p2DRender->m_ptOrigin.x;
	viewport.Y      = p2DRender->m_ptOrigin.y;
	viewport.Width  = p2DRender->m_clipRect.Width();
	viewport.Height = p2DRender->m_clipRect.Height();
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;
	pd3dDevice->SetViewport(&viewport);

	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );	

	//Jewel Rendering
	CTexture* pTexture;
	ItemProp* pItemProp;
	if(m_pItemElem != NULL)
	{
		for(int i=0; i<5; i++)
		{
			LPWNDCTRL pWndCtrl = GetWndCtrl( m_nJewelSlot[i] );
#if __VER >= 12 // __EXT_PIERCING
			if( i < m_pItemElem->GetUltimatePiercingSize() ) //뚫린 소켓
#else // __EXT_PIERCING
			if( i < m_pItemElem->GetPiercingSize() ) //뚫린 소켓
#endif // __EXT_PIERCING
			{
				if(m_dwJewel[i] != -1) //박힌 보석
				{
					if(i != m_nUsableSlot) //새로 박은 보석위에는 색칠하지 않음.
					{
						pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "WndDisableBlue.bmp"), 0xffff00ff );
						if(pTexture != NULL)
							pTexture->Render( p2DRender, CPoint( pWndCtrl->rect.left, pWndCtrl->rect.top ) );
						//p2DRender->RenderFillRect( pWndCtrl->rect, 0x609370db );
					}

					pItemProp = prj.GetItemProp( m_dwJewel[i] );
					if(pItemProp != NULL)
					{
						pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
						if(pTexture != NULL)
							pTexture->Render( p2DRender, CPoint( pWndCtrl->rect.left, pWndCtrl->rect.top ) );
					}
				}
				else
				{
					if(i != m_nUsableSlot) //뚫린 소켓 중 보석을 박을 수 있는 첫번째 슬롯만 빼고 나머지는 회색으로.
					{
						pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "WndDisableBlack.bmp"), 0xffff00ff );
						if(pTexture != NULL)
							pTexture->Render( p2DRender, CPoint( pWndCtrl->rect.left, pWndCtrl->rect.top ) );
						//p2DRender->RenderFillRect( pWndCtrl->rect, 0xa0a8a8a8 );
					}
				}
			}
			else //안 뚫린 소켓
			{
				pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "WndDisableRed.bmp"), 0xffff00ff );
				if(pTexture != NULL)
					pTexture->Render( p2DRender, CPoint( pWndCtrl->rect.left, pWndCtrl->rect.top ) );
				//p2DRender->RenderFillRect( pWndCtrl->rect, 0xa0ff0000 );
			}
		}
	}

	if(m_nStatus == 1)
	{
		color =  D3DCOLOR_ARGB( m_nAlpha, 240, 255, 255 );
				
		CRect rect;
		LPWNDCTRL lpWndCtrl;
					
		lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
		rect = lpWndCtrl->rect;
		p2DRender->RenderFillRect( rect, color );
	}
}

BOOL CWndSmeltJewel::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	if(m_nStatus != 0)
		return FALSE;

	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_CUSTOM1 );		

	//Set Weapon
	CItemElem* pItemElem = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );

	if(pItemElem && (pItemElem->GetProp()->dwItemKind2 == IK2_WEAPON_DIRECT || pItemElem->GetProp()->dwItemKind2 == IK2_WEAPON_MAGIC) &&
		pItemElem->GetProp()->dwReferStat1 == WEAPON_ULTIMATE)
	{	
		if( wndCtrl->rect.PtInRect( point ) )
		{
			//초기화.
			if(m_pItemElem != NULL)
				m_pItemElem->SetExtra(0);

			InitializeJewel(pItemElem);
			
			if(m_pItemElem != NULL)
				m_pItemElem->SetExtra(pItemElem->GetExtra()+1);
			
			m_pMainItem = (CModelObject*)prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_ITEM, m_pItemElem->m_dwItemId );
			m_pMainItem->InitDeviceObjects( g_Neuz.GetDevice() );
		}
	} 
	
	//SetJewel
#if __VER >= 12 // __EXT_PIERCING
	if(m_pItemElem != NULL && m_pItemElem->GetUltimatePiercingSize() > 0)
#else // __EXT_PIERCING
	if(m_pItemElem != NULL && m_pItemElem->GetPiercingSize() > 0)
#endif // __EXT_PIERCING
	{
		if(pItemElem && (pItemElem->m_dwItemId == II_GEN_MAT_RUBY || 
			pItemElem->m_dwItemId == II_GEN_MAT_DIAMOND ||
			pItemElem->m_dwItemId == II_GEN_MAT_EMERALD ||
			pItemElem->m_dwItemId == II_GEN_MAT_SAPPHIRE ||
			pItemElem->m_dwItemId == II_GEN_MAT_TOPAZ))
		{
			int checkslot = -1;
			for(int i=0; i<5; i++)
			{
				LPWNDCTRL wndCtrl = GetWndCtrl( m_nJewelSlot[i] );
				if(wndCtrl->rect.PtInRect( point ))
				{
					checkslot = i;
					i = 5;
				}
			}
			if(m_nUsableSlot > -1 && m_dwJewel[m_nUsableSlot] == -1 && checkslot == m_nUsableSlot)
			{
				m_dwJewel[checkslot] = pItemElem->m_dwItemId;
				m_objJewelId = pItemElem->m_dwObjId;
				pItemElem->SetExtra(pItemElem->GetExtra()+1);
				m_pJewelElem = pItemElem;
			}
		}
	}
	
	return TRUE;
}

void CWndSmeltJewel::SetJewel(CItemElem* pItemElem)
{
	//SetJewel
#if __VER >= 12 // __EXT_PIERCING
	if( m_nStatus == 0 && m_pItemElem != NULL && m_pItemElem->GetUltimatePiercingSize() > 0 )
#else // __EXT_PIERCING
	if( m_nStatus == 0 && m_pItemElem != NULL && m_pItemElem->GetPiercingSize() > 0 )
#endif // __EXT_PIERCING
	{
		if(pItemElem && (pItemElem->m_dwItemId == II_GEN_MAT_RUBY || 
			pItemElem->m_dwItemId == II_GEN_MAT_DIAMOND ||
			pItemElem->m_dwItemId == II_GEN_MAT_EMERALD ||
			pItemElem->m_dwItemId == II_GEN_MAT_SAPPHIRE ||
			pItemElem->m_dwItemId == II_GEN_MAT_TOPAZ))
		{
			if(m_nUsableSlot > -1 && m_dwJewel[m_nUsableSlot] == -1)
			{
				m_dwJewel[m_nUsableSlot] = pItemElem->m_dwItemId;
				m_objJewelId = pItemElem->m_dwObjId;
				pItemElem->SetExtra(pItemElem->GetExtra()+1);
				m_pJewelElem = pItemElem;
			}
		}
	}
}

void CWndSmeltJewel::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	LPWNDCTRL wndCtrl = GetWndCtrl( m_nJewelSlot[m_nUsableSlot] );		
	if( m_nStatus == 0 && wndCtrl != NULL && wndCtrl->rect.PtInRect( point ) )
	{
		if(m_dwJewel[m_nUsableSlot] != -1)
		{
			if(m_pJewelElem)
				m_pJewelElem->SetExtra(0);
			m_dwJewel[m_nUsableSlot] = -1;
		}
	}
}

void CWndSmeltJewel::SetDescription( CHAR* szChar )
{
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "SmeltJewel.inc" ) ));
	szChar = scanner.m_pProg;
	
	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString( szChar );
		m_pText->ResetString();	
	}
}

BOOL CWndSmeltJewel::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	if(message == WNM_CLICKED)
	{
		if(nID == WIDC_START) 
		{
			CWndButton* pButton;
			pButton = (CWndButton*)GetDlgItem( WIDC_START );
			pButton->EnableWindow(FALSE);
			
			m_nStatus = 1;
			PLAYSND( "PcSkillD-Counter01.wav" );
		}
	}
			
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

void CWndSmeltJewel::ReceiveResult(int result)
{
	//결과 값에 따른 처리
	//1. 성공 : 초기화
	//2. 실패 : 초기화
	//3. 취소 : 기타.
	
	switch(result) 
	{
	case CUltimateWeapon::ULTIMATE_SUCCESS:
		g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_SMELTJEWEL_SUCCESS ) );
		PLAYSND( "InfUpgradeSuccess.wav" );
		//초기화.
		InitializeJewel(m_pItemElem);
		Destroy();
		break;
	case CUltimateWeapon::ULTIMATE_FAILED:
		g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_SMELTJEWEL_FAIL ) );
		//초기화.
		InitializeJewel(m_pItemElem);		
		break;
	case CUltimateWeapon::ULTIMATE_CANCEL:
		Destroy();
		break;			
	}
}

void CWndSmeltJewel::InitializeJewel(CItemElem* pItemElem)
{
	//기존 데이타 초기화.
	m_nJewelCount = 0;
	m_nUsableSlot = -1;
	m_dwJewel[m_nUsableSlot] = -1;
	m_objJewelId = -1;

	m_pItemElem = pItemElem;
	
	for(int i=0; i<5; i++)
	{
		m_dwJewel[i] = -1;
#if __VER >= 12 // __EXT_PIERCING
		if(i < m_pItemElem->GetUltimatePiercingSize() )
		{
			if(m_pItemElem->GetUltimatePiercingItem( i ) != 0)
			{
				m_dwJewel[i] = m_pItemElem->GetUltimatePiercingItem( i );
				m_nJewelCount++;
			}
		}
	}

	//빈 슬롯이 남았는지 확인하여 사용가능 슬롯 번호 저장.
	int m_nSlot = pItemElem->GetUltimatePiercingSize();
#else // __EXT_PIERCING
		if(i < m_pItemElem->GetPiercingSize() )
		{
			if(m_pItemElem->GetPiercingItem( i ) != 0)
			{
				m_dwJewel[i] = m_pItemElem->GetPiercingItem( i );
				m_nJewelCount++;
			}
		}
	}

	//빈 슬롯이 남았는지 확인하여 사용가능 슬롯 번호 저장.
	int m_nSlot = pItemElem->GetPiercingSize();
#endif // __EXT_PIERCING
	if(m_nJewelCount < m_nSlot)
		m_nUsableSlot = m_nJewelCount;
	else
		m_nUsableSlot = -1;
	
	if(m_pJewelElem)
		m_pJewelElem->SetExtra(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndChangeWeapon Class
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndChangeWeapon::CWndChangeWeapon(int nType) 
{
	m_pWItemElem = NULL;
	m_pJItemElem[0] = NULL;
	m_pJItemElem[1] = NULL;
	m_nWeaponType = nType;
	m_bIsSendChange = FALSE;
}

CWndChangeWeapon::~CWndChangeWeapon() 
{ 
} 

void CWndChangeWeapon::OnDestroy()
{
	if(m_pWItemElem != NULL)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pWItemElem ) )
			m_pWItemElem->SetExtra(0);
	}
	if(m_pJItemElem[0] != NULL)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pJItemElem[0] ) )
			m_pJItemElem[0]->SetExtra(0);
	}
	if(m_pJItemElem[1] != NULL)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pJItemElem[1] ) )
			m_pJItemElem[1]->SetExtra(0);
	}
}

void CWndChangeWeapon::OnDraw( C2DRender* p2DRender ) 
{
	ItemProp* pItemProp;
	CTexture* pTexture;
	
	if(m_pWItemElem != NULL)
	{
		pItemProp = m_pWItemElem->GetProp();
		LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC1 );
		if(pItemProp != NULL)
		{
			pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
			if(pTexture != NULL)
				pTexture->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
		}
	} 
	
	if(m_pJItemElem[0] != NULL)
	{
		pItemProp = m_pJItemElem[0]->GetProp();
		LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC2 );
		if(pItemProp != NULL)
		{
			pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
			if(pTexture != NULL)
				pTexture->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
		}
	} 
	
	if(m_pJItemElem[1] != NULL)
	{
		pItemProp = m_pJItemElem[1]->GetProp();
		LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC3 );
		if(pItemProp != NULL)
		{
			pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
			if(pTexture != NULL)
				pTexture->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
		}
	} 
}

void CWndChangeWeapon::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);

	if(::GetLanguage() == LANG_FRE)
		pButton->SetTexture(g_Neuz.m_pd3dDevice, MakePath( "Theme\\", ::GetLanguage(), _T( "ButtNormal00.tga" ) ), TRUE);

	pButton->EnableWindow(FALSE);
	m_pText1 = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	m_pText2 = (CWndText*)GetDlgItem( WIDC_TEXT2 );

	SetDescription(NULL);

	//Set Caption
	if(m_nWeaponType  == WEAPON_GENERAL)
		SetTitle(prj.GetText(TID_GAME_CHANGEWEAPON_UNIQUE));
	else if(m_nWeaponType  == WEAPON_UNIQUE)
		SetTitle(prj.GetText(TID_GAME_CHANGEWEAPON_ULTIMATE));
	
	//Window Position
/*	CWndInventory* pWndInventory;
	pWndInventory = (CWndInventory*)GetWndBase( APP_INVENTORY );
	CRect rectInventory;
	if(pWndInventory != NULL)
		rectInventory = pWndInventory->GetWindowRect( TRUE );

	CPoint ptInventory = rectInventory.TopLeft();
	CPoint ptMove;
	
	CRect rect = GetWindowRect( TRUE );
	
	if( ptInventory.x > rect.Width() / 2 )
		ptMove = ptInventory - CPoint( rect.Width(), 0 );
	else
		ptMove = ptInventory + CPoint( rectInventory.Width(), 0 );
	
	Move( ptMove );
*/
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndChangeWeapon::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_SMELT_CHANGEWEAPON, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndChangeWeapon::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndChangeWeapon::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndChangeWeapon::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndChangeWeapon::OnLButtonDown( UINT nFlags, CPoint point ) 
{
}

void CWndChangeWeapon::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC1 );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{
		if(m_pWItemElem)m_pWItemElem->SetExtra(0);
		m_pWItemElem = NULL;
	}

	wndCtrl = GetWndCtrl( WIDC_STATIC2 );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{
		if(m_pJItemElem[0]) m_pJItemElem[0]->SetExtra(0);
		m_pJItemElem[0] = NULL;
	}

	wndCtrl = GetWndCtrl( WIDC_STATIC3 );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{
		if(m_pJItemElem[1])m_pJItemElem[1]->SetExtra(0);
		m_pJItemElem[1] = NULL;
	}
}

void CWndChangeWeapon::OnMouseWndSurface(CPoint point)
{
	CRect rect1, rect2, rect3;
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC1 );
	rect1 = wndCtrl->rect;
	wndCtrl = GetWndCtrl( WIDC_STATIC2 );
	rect2 = wndCtrl->rect;
	wndCtrl = GetWndCtrl( WIDC_STATIC3 );
	rect3 = wndCtrl->rect;

	if( rect1.PtInRect( point ) )
	{
		ClientToScreen( &point );
		ClientToScreen( &rect1 );
		CString tooltip;
		if(m_nWeaponType  == WEAPON_GENERAL)
			tooltip.Format( "%s", prj.GetText( TID_TOOLTIP_CHANGEW_GENERAL ) );
		else if(m_nWeaponType  == WEAPON_UNIQUE)
			tooltip.Format( "%s", prj.GetText( TID_TOOLTIP_CHANGEW_UNIQUE ) );		
		g_toolTip.PutToolTip( (DWORD)this, tooltip, rect1, point );
	}
	else if(rect2.PtInRect( point ))
	{
		ClientToScreen( &point );
		ClientToScreen( &rect2 );
		CString tooltip;
		tooltip.Format( "%s", prj.GetText( TID_TOOLTIP_CHANGEW_ORICALCUM ) );
		g_toolTip.PutToolTip( (DWORD)this, tooltip, rect2, point );
	}
	else if(rect3.PtInRect( point ))
	{
		ClientToScreen( &point );
		ClientToScreen( &rect3 );
		CString tooltip;
		tooltip.Format( "%s", prj.GetText( TID_TOOLTIP_CHANGEW_JEWEL ) );
		g_toolTip.PutToolTip( (DWORD)this, tooltip, rect3, point );
	}
}

BOOL CWndChangeWeapon::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	//Set Weapon
	CRect rect;
	ItemProp* pItemProp;
	CItemElem* pTempElem;
	pTempElem  = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );

	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC1 );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{
		if(pTempElem != NULL && m_pWItemElem == NULL)
		{
			pItemProp = pTempElem->GetProp();

			if( pItemProp->dwItemKind1 == IK1_WEAPON && pItemProp->dwLimitLevel1 >= 60 ) //60레벨 제한 이상 무기만 적용
			{
				if(m_nWeaponType == pItemProp->dwReferStat1) //창에 따라 유니크인지 일반인지를 확인한다.
				{
					if(m_nWeaponType == WEAPON_UNIQUE)
					{
						if(pTempElem->GetAbilityOption() >= 10) //유니크 무기의 경우 +10 이상인지를 확인 한다.
						{
							m_pWItemElem = pTempElem;
							m_pWItemElem->SetExtra(m_pWItemElem->GetExtra()+1);
						}
					}
					else if(m_nWeaponType == WEAPON_GENERAL)
					{
						m_pWItemElem = pTempElem;
						m_pWItemElem->SetExtra(m_pWItemElem->GetExtra()+1);
					}
				}
			}
		}
	}
	
	//Set Ligthing Oricalcum
	wndCtrl = GetWndCtrl( WIDC_STATIC2 );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{
		if(pTempElem && (pTempElem->m_dwItemId == II_GEN_MAT_ORICHALCUM02))
		{
			m_pJItemElem[0] = pTempElem;
			m_pJItemElem[0]->SetExtra(m_pJItemElem[0]->GetExtra()+1);
		}
	}
	
	//Set Jewel
	wndCtrl = GetWndCtrl( WIDC_STATIC3 );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{	
		if((pTempElem != NULL && m_pWItemElem != NULL) &&
			(pTempElem->m_dwItemId == prj.m_UltimateWeapon.GetGemKind(m_pWItemElem->GetProp()->dwLimitLevel1)))
		{
			m_pJItemElem[1] = pTempElem;
			m_pJItemElem[1]->SetExtra(m_pJItemElem[1]->GetExtra()+1);
		}
	}

	return TRUE;
}

void CWndChangeWeapon::SetItem(CItemElem* pItemElem)
{
	if(pItemElem != NULL)
	{
		ItemProp* pProp = pItemElem->GetProp();
		
		if(m_pWItemElem == NULL && pProp->dwItemKind1 == IK1_WEAPON)
		{
			if(pProp->dwLimitLevel1 >= 60) //60레벨 제한 이상 무기만 적용
			{
				if(m_nWeaponType == pProp->dwReferStat1) //창에 따라 유니크인지 일반인지를 확인한다.
				{
					if(m_nWeaponType == WEAPON_UNIQUE)
					{
						if(pItemElem->GetAbilityOption() >= 10) //유니크 무기의 경우 +10 이상인지를 확인 한다.
						{
							m_pWItemElem = pItemElem;
							m_pWItemElem->SetExtra(m_pWItemElem->GetExtra()+1);
						}
					}
					else if(m_nWeaponType == WEAPON_GENERAL)
					{
						m_pWItemElem = pItemElem;
						m_pWItemElem->SetExtra(m_pWItemElem->GetExtra()+1);
					}
				}
			}
		}
		else if(m_pJItemElem[0] == NULL && pProp->dwID == II_GEN_MAT_ORICHALCUM02)
		{
			m_pJItemElem[0] = pItemElem;
			m_pJItemElem[0]->SetExtra(m_pJItemElem[0]->GetExtra()+1);
		}
		else if( m_pJItemElem[1] == NULL )
		{
			if(m_pWItemElem != NULL)
			{
				if(pItemElem->m_dwItemId == prj.m_UltimateWeapon.GetGemKind(m_pWItemElem->GetProp()->dwLimitLevel1) )
				{
					m_pJItemElem[1] = pItemElem;
					m_pJItemElem[1]->SetExtra(m_pJItemElem[1]->GetExtra()+1);
				}
			}
		}
	}
}

BOOL CWndChangeWeapon::Process()
{
	if(!m_bIsSendChange && m_pWItemElem != NULL && m_pJItemElem[0] != NULL && m_pJItemElem[1] != NULL)
	{
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
		pButton->EnableWindow(TRUE);
	}
	else
	{
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
		pButton->EnableWindow(FALSE);
	}

	return TRUE;
}

BOOL CWndChangeWeapon::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_START )
	{
		//서버로 시작을 알린다.
		if(!m_bIsSendChange && m_pWItemElem != NULL && m_pJItemElem[0] != NULL && m_pJItemElem[1] != NULL)
		{
			CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
			pButton->EnableWindow(FALSE);
			m_bIsSendChange = TRUE;
			
			g_DPlay.SendUltimateTransWeapon(m_pWItemElem->m_dwObjId, m_pJItemElem[1]->m_dwObjId, m_pJItemElem[0]->m_dwObjId);
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndChangeWeapon::SetDescription( CHAR* szChar )
{
	CScript scanner1, scanner2;
	BOOL checkflag;
	CHAR* szInfo;

	if(m_nWeaponType == WEAPON_GENERAL)
	{
		checkflag = scanner1.Load( MakePath( DIR_CLIENT,  _T( "SmeltChangeUniqueWeapon.inc" ) ));
		checkflag = scanner2.Load( MakePath( DIR_CLIENT,  _T( "ChangeUniqueWeaponInfo.inc" ) ));
	}
	else if(m_nWeaponType == WEAPON_UNIQUE)
	{
		checkflag = scanner1.Load( MakePath( DIR_CLIENT,  _T( "SmeltChangeUltimateWeapon.inc" ) ));
		checkflag = scanner2.Load( MakePath( DIR_CLIENT,  _T( "ChangeUltimateWeaponInfo.inc" ) ));
	}

	szChar = scanner1.m_pProg;
	szInfo = scanner2.m_pProg;

	if(m_pText1 != NULL && checkflag)
	{
		m_pText1->m_string.AddParsingString( szChar );
		m_pText1->ResetString();	
	}

	if(m_pText2 != NULL && checkflag)
	{
		m_pText2->m_string.AddParsingString( szInfo );
		m_pText2->ResetString();	
	}
}

void CWndChangeWeapon::ReceiveResult(int result)
{
	//결과 값에 따른 처리
	//1. 성공 : Destroy Window
	//2. 실패 : Destroy Window
	//3. 취소 : 인벤토리 여유 공간 부족 및 기타 에러 사항.
	
	switch(result) 
	{
		case CUltimateWeapon::ULTIMATE_SUCCESS:
			if(m_nWeaponType == WEAPON_UNIQUE)
				g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_CUWEAPON_SUCCESS ) );
			else if(m_nWeaponType == WEAPON_GENERAL)
				g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_CGWEAPON_SUCCESS ) );
			break;
		case CUltimateWeapon::ULTIMATE_FAILED:
			if(m_nWeaponType == WEAPON_UNIQUE)
				g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_CUWEAPON_FAILED ) );
			else if(m_nWeaponType == WEAPON_GENERAL)
				g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_CGWEAPON_FAILED ) );
			break;
		case CUltimateWeapon::ULTIMATE_CANCEL:
			Destroy();
			break;
		case CUltimateWeapon::ULTIMATE_INVENTORY:
			g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_EXTRACTION_ERROR ) );
			break;			
	}

	if(m_pJItemElem[0] != NULL)
		m_pJItemElem[0]->SetExtra(0);
	if(m_pJItemElem[1] != NULL)
		m_pJItemElem[1]->SetExtra(0);
	if(m_pWItemElem != NULL)
		m_pWItemElem->SetExtra(0);
	
	m_pJItemElem[0] = NULL;
	m_pJItemElem[1] = NULL;
	m_pWItemElem = NULL;
	m_bIsSendChange = FALSE;
}

//////////////////////////////////////////////////////////////////////////
//	CWndRemoveJewelConfirm
//////////////////////////////////////////////////////////////////////////

CWndRemoveJewelConfirm::CWndRemoveJewelConfirm() 
{
	m_pInventory = NULL;
	m_pUpgradeItem = NULL;
}
 
CWndRemoveJewelConfirm::~CWndRemoveJewelConfirm() 
{
} 

void CWndRemoveJewelConfirm::OnDestroy()
{
	if(m_pInventory != NULL)
	{
		m_pInventory->m_pWndRemoveJewelConfirm = NULL;
		m_pInventory->m_bRemoveJewel = FALSE;
	}
}

void CWndRemoveJewelConfirm::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndRemoveJewelConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	CWndText* pText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	pText->m_string.AddParsingString( prj.GetText( TID_GAME_REMOVEJEWEL ) );
	pText->ResetString();
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndRemoveJewelConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{
	m_pInventory = (CWndInventory*)pWndParent;
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_SMELT_REMOVE_CONFIRM, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndRemoveJewelConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndRemoveJewelConfirm::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndRemoveJewelConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndRemoveJewelConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndRemoveJewelConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES )
	{
		if(m_pInventory != NULL)
		{
			// 인첸트가 되는 아이템 - 방어구 등등
			m_pInventory->m_pUpgradeItem = m_pUpgradeItem;
			m_pInventory->m_dwEnchantWaitTime = g_tmCurrent + SEC(4);
		}
		Destroy();
	}
	else if( nID == WIDC_NO || nID == WTBID_CLOSE )
	{
		if(m_pInventory != NULL)
			m_pInventory->BaseMouseCursor();
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndRemoveJewelConfirm::SetItem(CItemBase*	m_pItem)
{
	m_pUpgradeItem = m_pItem;
}

#endif //__CSC_VER9_1


#if __VER >= 10 // __CSC_VER9_1 -> __LEGEND

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndHeroSkillUp Class
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndHeroSkillUp::CWndHeroSkillUp() 
{
	for(int i=0; i<5; i++)
	{
		m_pItemElem[i] = NULL;
		m_JewelID[i] = -1;
		m_SlotID[i] = -1;
		m_PicJewel[i] = -1;
	}

	m_bSendHeroSkillup = FALSE;
}

CWndHeroSkillUp::~CWndHeroSkillUp() 
{ 
} 

void CWndHeroSkillUp::OnDestroy()
{
	for(int i=0; i<5; i++)
	{
		if(m_pItemElem[i] != NULL)
		{
			if( !g_pPlayer->m_vtInfo.IsTrading( m_pItemElem[i] ) )
				m_pItemElem[i]->SetExtra(0);
		}
	}
}

void CWndHeroSkillUp::OnDraw( C2DRender* p2DRender )
{
	//Render Icon
	ItemProp* pItemProp;
	CTexture* pTexture;
	BOOL bCheckSlot = TRUE;
	
	CPoint point = GetMousePoint();
	int testnum = HitTest( point );
	if( testnum != -1)
	{
		if( CWndBase::m_GlobalShortcut.m_dwData )
		{
			CRect rect;
			LPWNDCTRL wndCtrl = GetWndCtrl( m_SlotID[testnum] );
			rect = wndCtrl->rect;
			p2DRender->RenderFillRect( rect, 0x60ffff00 );
		}
	}
	
	for(int i=0; i<5; i++)
	{
		//Default Jewel
		pItemProp = prj.GetItemProp( m_JewelID[i] );
		LPWNDCTRL wndCtrl = GetWndCtrl( m_PicJewel[i] );
		if(pItemProp != NULL)
		{
			pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
			if(pTexture != NULL)
				pTexture->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
		}
		
		//Slot Jewel
		if(m_pItemElem[i] != NULL)
		{
			pItemProp = m_pItemElem[i]->GetProp();
			wndCtrl = GetWndCtrl( m_SlotID[i] );
			if(pItemProp != NULL)
			{
				pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
				if(pTexture != NULL)
					pTexture->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
			}
		}
		else
			bCheckSlot = FALSE;	
	}

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_OK);
	pButton->EnableWindow(bCheckSlot);
}

void CWndHeroSkillUp::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_OK);
	pButton->EnableWindow(FALSE);

	m_pText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	SetDescription(NULL);

	m_SlotID[0] = WIDC_PIC_SLOT1;
	m_SlotID[1] = WIDC_PIC_SLOT2;
	m_SlotID[2] = WIDC_PIC_SLOT3;
	m_SlotID[3] = WIDC_PIC_SLOT4;
	m_SlotID[4] = WIDC_PIC_SLOT5;

	m_PicJewel[0] = WIDC_CUSTOM1;
	m_PicJewel[1] = WIDC_CUSTOM2;
	m_PicJewel[2] = WIDC_CUSTOM3;
	m_PicJewel[3] = WIDC_CUSTOM4;
	m_PicJewel[4] = WIDC_CUSTOM5;

	m_JewelID[0] = II_GEN_MAT_DIAMOND;
	m_JewelID[1] = II_GEN_MAT_EMERALD;
	m_JewelID[2] = II_GEN_MAT_SAPPHIRE;
	m_JewelID[3] = II_GEN_MAT_RUBY;
	m_JewelID[4] = II_GEN_MAT_TOPAZ;
	
	//Window Position
/*	CWndInventory* pWndInventory;
	pWndInventory = (CWndInventory*)GetWndBase( APP_INVENTORY );
	CRect rectInventory;
	if(pWndInventory != NULL)
		rectInventory = pWndInventory->GetWindowRect( TRUE );

	CPoint ptInventory = rectInventory.TopLeft();
	CPoint ptMove;
	
	CRect rect = GetWindowRect( TRUE );
	
	if( ptInventory.x > rect.Width() / 2 )
		ptMove = ptInventory - CPoint( rect.Width(), 0 );
	else
		ptMove = ptInventory + CPoint( rectInventory.Width(), 0 );
	
	Move( ptMove );
*/
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndHeroSkillUp::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_HERO_SKILLUP, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndHeroSkillUp::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndHeroSkillUp::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndHeroSkillUp::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndHeroSkillUp::OnLButtonDown( UINT nFlags, CPoint point ) 
{
}

void CWndHeroSkillUp::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	int choicenum = HitTest( point );
	if(choicenum > -1 && m_pItemElem[choicenum] != NULL)
	{
		m_pItemElem[choicenum]->SetExtra(0);
		m_pItemElem[choicenum] = NULL;
	}
}

BOOL CWndHeroSkillUp::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	int nSelect = HitTest(point);
	if(nSelect > -1)
	{
		CItemElem* pItemElem = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );

		if(m_pItemElem[nSelect] == NULL)
		{
			if(pItemElem->GetProp()->dwID == m_JewelID[nSelect])
			{
				m_pItemElem[nSelect] = pItemElem;
				m_pItemElem[nSelect]->SetExtra(m_pItemElem[nSelect]->GetExtra()+1);
			}
		}
	}
	
	return TRUE;
}

BOOL CWndHeroSkillUp::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK )
	{
		if(!m_bSendHeroSkillup)
		{
			//서버로 시작을 알린다.
			BOOL checkall = TRUE;
			OBJID itemobjId[5];
			for(int i=0; i<5; i++)
			{
				if(m_pItemElem[i] == NULL)
					checkall = FALSE;
				else
					itemobjId[i] = m_pItemElem[i]->m_dwObjId;				
			}

			if(checkall)
			{
				m_bSendHeroSkillup = TRUE;
				g_DPlay.SendLegendSkillUp(itemobjId, 5);
			}
		}
	}
	else if( nID == WIDC_CANCEL )
		Destroy();

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

int CWndHeroSkillUp::HitTest( CPoint point )
{
	int rtn_val = -1;
	CRect rect;
	for(int i=0; i<5; i++)
	{
		LPWNDCTRL wndCtrl = GetWndCtrl( m_SlotID[i] );		
		rect = wndCtrl->rect;
		if( rect.PtInRect( point ) )
		{
			rtn_val = i;
			i = MAX_JEWEL;
		}
	}
	return rtn_val;
}

void CWndHeroSkillUp::SetDescription( CHAR* szChar )
{
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "HeroSkillUp.inc" ) ));
	szChar = scanner.m_pProg;
	
	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString( szChar );
		m_pText->ResetString();	
	}
}

void CWndHeroSkillUp::SetJewel(CItemElem* pItemElem)
{
	ItemProp* pProp = pItemElem->GetProp();
	int nSelect = -1;

	switch(pProp->dwID) 
	{
		case II_GEN_MAT_DIAMOND:
			nSelect = 0;
			break;
		case II_GEN_MAT_EMERALD:
			nSelect = 1;
			break;
		case II_GEN_MAT_SAPPHIRE:
			nSelect = 2;
			break;
		case II_GEN_MAT_RUBY:
			nSelect = 3;
			break;
		case II_GEN_MAT_TOPAZ:
			nSelect = 4;
			break;
	}

	if(nSelect > -1)
	{
		if(m_pItemElem[nSelect] == NULL)
		{
			if(pItemElem->GetProp()->dwID == m_JewelID[nSelect])
			{
				m_pItemElem[nSelect] = pItemElem;
				m_pItemElem[nSelect]->SetExtra(m_pItemElem[nSelect]->GetExtra()+1);
			}
		}
	}
}

void CWndHeroSkillUp::ReceiveResult(int nresult)
{
	switch(nresult) 
	{
		case -1:
			g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_HEROSKILLUP_MAX ) );
			break;
		case 0:
			g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_HEROSKILLUP_FAIL ) );
			break;
		case 1:
			g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_HEROSKILLUP_SUCCESS ) );
			break;
	}

	Destroy();
}
#endif //__CSC_VER9_1 -> __LEGEND




#ifdef __TRADESYS

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndDialogEvent Class
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndDialogEvent::CWndDialogEvent() 
{
	m_nChoiceNum = -1;
	m_nListCount = 0;
	m_nDescCount = 0;
	m_nCurrentPage = 0;
	m_nMMI = -1;
	m_nListOffset = 0;

	CSize size = g_Neuz.m_2DRender.m_pFont->GetTextExtent( "123" );

	m_nGap = size.cy + 2;

	if(m_nGap >= 16)
		m_nListOffset = m_nGap - 16;
	else
		m_nListOffset = 0;
}

CWndDialogEvent::~CWndDialogEvent() 
{ 
} 

void CWndDialogEvent::OnDestroy()
{
}

void CWndDialogEvent::OnDraw( C2DRender* p2DRender ) 
{
	if(m_nChoiceNum > -1)
	{
		CRect rect;
		LPWNDCTRL lpWndCtrl;
		D3DXCOLOR color =  D3DCOLOR_ARGB( 60, 240, 0, 0 );
		
		lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );
		rect = lpWndCtrl->rect;
		rect.top += m_nChoiceNum * m_nGap + m_nListOffset;
		rect.bottom += m_nChoiceNum * m_nGap + m_nListOffset;
		//영어 때문에 창 크기 늘임
		rect.right += 80;
		p2DRender->RenderFillRect( rect, color );
	}
}

void CWndDialogEvent::SetMMI(int mmi)
{
	m_nMMI = mmi;
}

void CWndDialogEvent::OnInitialUpdate()
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	if(m_nMMI != -1)
	{
		m_nVecList = prj.m_Exchange.GetListTextId(m_nMMI);
		m_nListCount = m_nVecList.size();
		if( !m_nListCount ) // 리스트가 없는 경우
		{	
			Destroy();
			return;
		}
		SetDescription(NULL);
	}
	else
		Destroy();

	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndDialogEvent::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_DIALOG_EVENT, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndDialogEvent::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch(nID) 
	{
		case WIDC_BUTTON1:
			//Text Page Down
			if(m_nCurrentPage < m_nDescCount - 1)
			{
				m_nCurrentPage++;
				CWndText* pText = (CWndText*)GetDlgItem(WIDC_TEXT1);
				pText->m_string.SetString(prj.GetText(m_nDescList[m_nCurrentPage]));
				
				if(m_nCurrentPage == m_nDescCount - 1)
				{
					CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
					pButton->SetVisible(FALSE);
					pButton->EnableWindow(FALSE);
				}
			}
			break;
		case WIDC_OK:
			if(m_nChoiceNum > -1)
			{
				CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_OK);
				pButton->EnableWindow(FALSE);		
				//Send to Server....
				g_DPlay.SendExchange(m_nMMI, m_nChoiceNum);

			}
			else
				Destroy();
			break;
		case WIDC_CANCEL:
			Destroy();
			break;
		case WIDC_LISTBOX1:
			CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
			int nCurSel = pWndListBox->GetCurSel();
			if( nCurSel != -1 )
				m_nChoiceNum = nCurSel;
			break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndDialogEvent::SetDescription( CHAR* szChar )
{
	//Set List
	CRect rect;
	LPWNDCTRL lpWndCtrl;
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	lpWndCtrl = GetWndCtrl( WIDC_LISTBOX1 );
	rect = lpWndCtrl->rect;

	for(int i=0; i<m_nListCount; i++)
	{
		pWndList->AddString( prj.GetText(m_nVecList[i]));
	}

	//Set Description
	m_nDescList = prj.m_Exchange.GetDescId(m_nMMI);
	m_nDescCount = m_nDescList.size();
	if( !m_nDescCount )
		return;
	
	if(m_nDescCount == 1)
	{
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
		pButton->SetVisible(FALSE);
		pButton->EnableWindow(FALSE);
	}

	CWndText* pText = (CWndText*)GetDlgItem(WIDC_TEXT1);
	m_nCurrentPage = 0;
	pText->m_string.AddParsingString( prj.GetText(m_nDescList[m_nCurrentPage]) );
	pText->ResetString();	

	//Size Control
	if(m_nListCount <= MAX_LIST_COUNT)
	{
		int nListBoxBottom = 0;
		int nListBoxHeight = 0;
		int nButtonHeight = 0;
		//ListBox
		rect.bottom = rect.top + m_nListCount * m_nGap + 10;
		nListBoxBottom = rect.bottom;
		nListBoxHeight = rect.bottom - rect.top;
		//영어 때문에 창 크기 늘임
		rect.right += 80;
		pWndList->SetWndRect(rect);
		//Window
		rect = GetWindowRect(TRUE);
		lpWndCtrl = GetWndCtrl(WIDC_TEXT1);
		CRect rectText = lpWndCtrl->rect;
		rect.bottom = rect.top + nListBoxHeight + (rectText.bottom - rectText.top) + 80;
		SetWndRect(rect);
		//Button
		CWndButton* pWndButton = (CWndButton*)GetDlgItem( WIDC_OK );		
		lpWndCtrl = GetWndCtrl( WIDC_OK );
		rect = lpWndCtrl->rect;
		nButtonHeight = rect.bottom - rect.top;
		rect.top = nListBoxBottom + 8;
		rect.bottom = rect.top + nButtonHeight;
		//영어 때문에 창 크기 늘임
		rect.left += 40;
		rect.right += 40;
		pWndButton->SetWndRect(rect);

		pWndButton = (CWndButton*)GetDlgItem( WIDC_CANCEL );		
		lpWndCtrl = GetWndCtrl( WIDC_CANCEL );
		rect = lpWndCtrl->rect;
		nButtonHeight = rect.bottom - rect.top;
		rect.top = nListBoxBottom + 8;
		rect.bottom = rect.top + nButtonHeight;
		//영어 때문에 창 크기 늘임
		rect.left += 40;
		rect.right += 40;
		pWndButton->SetWndRect(rect);
	}

	//영어 때문에 창 크기 늘임
	//Window
	rect = GetWindowRect(TRUE);
	rect.right += 80;
	SetWndRect(rect);

	//Button
	CWndButton* pWndButton = (CWndButton*)GetDlgItem( WIDC_BUTTON1 );		
	lpWndCtrl = GetWndCtrl( WIDC_BUTTON1 );
	rect = lpWndCtrl->rect;
	rect.left += 80;
	rect.right += 80;
	pWndButton->SetWndRect(rect);

	//Text
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	lpWndCtrl = GetWndCtrl( WIDC_TEXT1 );
	rect = lpWndCtrl->rect;
	rect.right += 80;
	pWndText->SetWndRect(rect);
}

void CWndDialogEvent::ReceiveResult(int result)
{
	vector<int> vResult = prj.m_Exchange.GetResultMsg(m_nMMI, m_nChoiceNum);
	
	switch(result) 
	{
		case CExchange::EXCHANGE_SUCCESS:
			// 서버에서 시스템 메세지 출력으로 변경
			if( vResult.size() == 2 )
				g_WndMng.OpenMessageBox( prj.GetText(vResult[0]) );
			break;
		case CExchange::EXCHANGE_INVENTORY_FAILED:
			g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_LACKSPACE) );
			break;
		case CExchange::EXCHANGE_CONDITION_FAILED:
			// 서버에서 시스템 메세지 출력으로 변경
			if( vResult.size() == 2 )
				g_WndMng.OpenMessageBox( prj.GetText(vResult[1]) );
			break;
		case CExchange::EXCHANGE_FAILED:
			g_WndMng.OpenMessageBox( "FAILED" );
			break;			
	}

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_OK);
	pButton->EnableWindow(TRUE);
}

#endif //__TRADESYS

#if __VER >= 12 // __HEAVEN_TOWER

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndHeavenTower Class
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndHeavenTower::CWndHeavenTower() 
{
	m_nChoiceNum = -1;
	m_nListCount = 0;
	m_nDescCount = 0;
	m_nCurrentPage = 0;
	m_nListOffset = 0;

	CSize size = g_Neuz.m_2DRender.m_pFont->GetTextExtent( "123" );

	m_nGap = size.cy + 2;

	if(m_nGap >= 16)
		m_nListOffset = m_nGap - 16;
	else
		m_nListOffset = 0;
}

CWndHeavenTower::~CWndHeavenTower() 
{ 
} 

void CWndHeavenTower::OnDestroy()
{
}

void CWndHeavenTower::OnDraw( C2DRender* p2DRender ) 
{
	if(m_nChoiceNum > -1)
	{
		CRect rect;
		LPWNDCTRL lpWndCtrl;
		D3DXCOLOR color =  D3DCOLOR_ARGB( 60, 240, 0, 0 );
		
		lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );
		rect = lpWndCtrl->rect;
		rect.top += m_nChoiceNum * m_nGap + m_nListOffset;
		rect.bottom += m_nChoiceNum * m_nGap + m_nListOffset;
		//영어 때문에 창 크기 늘임
		rect.right += 80;
		p2DRender->RenderFillRect( rect, color );
	}
}

void CWndHeavenTower::InitText()
{
	m_nDescList.push_back(TID_TOOLTIP_HEAVENTOWER);

	m_nVecList.push_back(TID_GAME_HEAVETOWER_FLOOR01);
	m_nVecList.push_back(TID_GAME_HEAVETOWER_FLOOR02);
	m_nVecList.push_back(TID_GAME_HEAVETOWER_FLOOR03);
	m_nVecList.push_back(TID_GAME_HEAVETOWER_FLOOR04);
	m_nVecList.push_back(TID_GAME_HEAVETOWER_FLOOR05);
	m_nVecList.push_back(TID_GAME_HEAVETOWER_FLOOR06);
	m_nVecList.push_back(TID_GAME_HEAVETOWER_FLOOR07);

	m_nMsgList.push_back(TID_GAME_HEAVETOWER_FLOOR01_MSG);
	m_nMsgList.push_back(TID_GAME_HEAVETOWER_FLOOR02_MSG);
	m_nMsgList.push_back(TID_GAME_HEAVETOWER_FLOOR03_MSG);
	m_nMsgList.push_back(TID_GAME_HEAVETOWER_FLOOR04_MSG);
	m_nMsgList.push_back(TID_GAME_HEAVETOWER_FLOOR05_MSG);
	m_nMsgList.push_back(TID_GAME_HEAVETOWER_FLOOR06_MSG);

	m_nCost.push_back(10000);
	m_nCost.push_back(30000);
	m_nCost.push_back(50000);
	m_nCost.push_back(70000);
	m_nCost.push_back(100000);

	m_nListCount = m_nVecList.size();

	//Set List
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );

	for(int i=0; i<m_nListCount; i++)
	{
		CString strText;
		strText.Format("%d. %s", i+1, prj.GetText(m_nVecList[i]));
		pWndList->AddString(strText);
	}

	//Set Description
	m_nDescCount = m_nDescList.size();
	if( m_nDescCount == 0 )
		return;
	
	if(m_nDescCount == 1)
	{
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
		pButton->SetVisible(FALSE);
		pButton->EnableWindow(FALSE);
	}

	CWndText* pText = (CWndText*)GetDlgItem(WIDC_TEXT1);
	m_nCurrentPage = 0;
	pText->m_string.AddParsingString( prj.GetText(m_nDescList[m_nCurrentPage]) );
	pText->ResetString();
}

void CWndHeavenTower::OnInitialUpdate()
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	InitText();

	if( m_nListCount == 0 )
	{	
		Destroy();
		return;
	}
	InitWnd();

	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndHeavenTower::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_HEAVEN_TOWER, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndHeavenTower::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch(nID) 
	{
		case WIDC_BUTTON1:
			//Text Page Down
			if(m_nCurrentPage < m_nDescCount - 1)
			{
				m_nCurrentPage++;
				CWndText* pText = (CWndText*)GetDlgItem(WIDC_TEXT1);
				pText->m_string.SetString(prj.GetText(m_nDescList[m_nCurrentPage]));
				
				if(m_nCurrentPage == m_nDescCount - 1)
				{
					CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
					pButton->SetVisible(FALSE);
					pButton->EnableWindow(FALSE);
				}
			}
			break;
		case WIDC_LISTBOX1:
			CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
			int nCurSel = pWndListBox->GetCurSel();
			if( nCurSel != -1 )
			{
				m_nChoiceNum = nCurSel;

				if(m_nChoiceNum == 6)
				{
					Destroy();
					break;
				}

				CString strMsg;
				CWndHeavenTowerEntranceConfirm* pWndEntranceConfirm = new CWndHeavenTowerEntranceConfirm;
				if( pWndEntranceConfirm )
				{
					g_WndMng.OpenCustomBox( "", pWndEntranceConfirm, this );
					
					if(m_nChoiceNum != 5)
					{
						strMsg.Format( prj.GetText(m_nMsgList[m_nChoiceNum]), m_nCost[m_nChoiceNum] );
						pWndEntranceConfirm->SetValue( strMsg, m_nChoiceNum+1 );
					}
					else
					{
						strMsg.Format( prj.GetText(m_nMsgList[m_nChoiceNum]) );
						pWndEntranceConfirm->SetValue( strMsg, m_nChoiceNum+1 );
					}

				}
			}
			break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndHeavenTower::InitWnd()
{
	CRect rect;
	LPWNDCTRL lpWndCtrl;

	//Size Control
	if(m_nListCount <= MAX_FLOOR_COUNT)
	{
		CWndListBox* pWndList = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
		lpWndCtrl = GetWndCtrl( WIDC_LISTBOX1 );
		rect = lpWndCtrl->rect;

		int nListBoxBottom = 0;
		int nListBoxHeight = 0;
		int nButtonHeight = 0;
		//ListBox
		rect.bottom = rect.top + m_nListCount * m_nGap + 10;
		nListBoxBottom = rect.bottom;
		nListBoxHeight = rect.bottom - rect.top;
		//영어 때문에 창 크기 늘임
		rect.right += 80;

		pWndList->SetWndRect(rect);
		//Window
		rect = GetWindowRect(TRUE);
		lpWndCtrl = GetWndCtrl(WIDC_TEXT1);
		CRect rectText = lpWndCtrl->rect;
		rect.bottom = rect.top + nListBoxHeight + (rectText.bottom - rectText.top) + 60;
		SetWndRect(rect);
	}

	//영어 때문에 창 크기 늘임
	//Window
	rect = GetWindowRect(TRUE);
	rect.right += 80;
	SetWndRect(rect);

	//Button
	CWndButton* pWndButton = (CWndButton*)GetDlgItem( WIDC_BUTTON1 );		
	lpWndCtrl = GetWndCtrl( WIDC_BUTTON1 );
	rect = lpWndCtrl->rect;
	rect.left += 80;
	rect.right += 80;
	pWndButton->SetWndRect(rect);

	//Text
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	lpWndCtrl = GetWndCtrl( WIDC_TEXT1 );
	rect = lpWndCtrl->rect;
	rect.right += 80;
	pWndText->SetWndRect(rect);
}

//////////////////////////////////////////////////////////////////////////
// CWndHeavenTowerEntranceConfirm Class
//////////////////////////////////////////////////////////////////////////

BOOL CWndHeavenTowerEntranceConfirm::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	m_nFloor = 0;
	return CWndMessageBox::Initialize( "", pWndParent, MB_OKCANCEL );	
}

void CWndHeavenTowerEntranceConfirm::SetValue( CString str, DWORD nFloor )
{
	m_wndText.SetString( str );
	m_nFloor = nFloor;	
}

BOOL CWndHeavenTowerEntranceConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
		case IDOK:
			if(m_nFloor > 0 && m_nFloor != 6)
			{
				g_DPlay.SendTeleportToHeavenTower(m_nFloor);
				SAFE_DELETE(g_WndMng.m_pWndHeavenTower);
			}
			Destroy();
			break;
		case IDCANCEL:
			Destroy();
			break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}
#endif //__HEAVEN_TOWER

#if __VER >= 10 // __REMOVE_ATTRIBUTE
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndRemoveAttribute Class
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndRemoveAttribute::CWndRemoveAttribute() 
{
	m_pItemElem = NULL;
	m_pEItemProp = NULL;
	m_pWndConfirm = NULL;
	m_pTexture = NULL;
}

CWndRemoveAttribute::~CWndRemoveAttribute() 
{
	if(m_pItemElem != NULL)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pItemElem ) )
			m_pItemElem->SetExtra(0);
	}

	if(m_pWndConfirm != NULL)
		m_pWndConfirm->Destroy();

	SAFE_DELETE(m_pWndConfirm);
}

void CWndRemoveAttribute::OnDestroy()
{
}

void CWndRemoveAttribute::OnDraw( C2DRender* p2DRender ) 
{
	ItemProp* pItemProp;
	
	if(m_pItemElem != NULL)
	{
		pItemProp = m_pItemElem->GetProp();
		LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_PIC_SLOT );
		if(pItemProp != NULL)
		{
			if(m_pTexture != NULL)
				m_pTexture->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
		}
	} 
}

void CWndRemoveAttribute::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);

	if(::GetLanguage() == LANG_FRE)
		pButton->SetTexture(g_Neuz.m_pd3dDevice, MakePath( "Theme\\", ::GetLanguage(), _T( "ButtNormal00.tga" ) ), TRUE);

	pButton->EnableWindow(FALSE);
	m_pText = (CWndText*)GetDlgItem( WIDC_TEXT1 );

	SetDescription(NULL);

	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndRemoveAttribute::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_REMOVE_ATTRIBUTE, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndRemoveAttribute::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndRemoveAttribute::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndRemoveAttribute::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndRemoveAttribute::OnLButtonDown( UINT nFlags, CPoint point ) 
{
}

void CWndRemoveAttribute::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	if(!m_pItemElem) return;
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_PIC_SLOT );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{
		m_pItemElem->SetExtra(0);
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
		pButton->EnableWindow(FALSE);
		m_pItemElem = NULL;
		m_pEItemProp = NULL;
		m_pTexture = NULL;
	}
}

BOOL CWndRemoveAttribute::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_PIC_SLOT );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{		
		//속성제련이 된 무기만 올려놓을 수 있다.
		ItemProp* pItemProp;
		CItemElem* pTempElem;
		pTempElem  = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
		
		if( pTempElem != NULL && pTempElem->m_nResistAbilityOption > 0 )
		{
			pItemProp = pTempElem->GetProp();
			if( CItemElem::IsEleRefineryAble(pItemProp) )
			{
//				if( pItemProp->dwReferStat1 == WEAPON_GENERAL )
//				{
					if(m_pItemElem != NULL)
						m_pItemElem->SetExtra(0);

					m_pItemElem = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
					m_pEItemProp = m_pItemElem->GetProp();
					m_pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, m_pEItemProp->szIcon), 0xffff00ff );
					m_pItemElem->SetExtra(m_pItemElem->GetExtra()+1);
					CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
					pButton->EnableWindow(TRUE);
//				}
			}
		}
		else
			g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_REMOVE_ERROR ) ); //Error Message
	}
	return TRUE;
}

void CWndRemoveAttribute::SetWeapon(CItemElem* pItemElem)
{
	if( pItemElem != NULL && pItemElem->m_nResistAbilityOption > 0 )
	{
		ItemProp* pProp = pItemElem->GetProp();

//		if( pProp->dwItemKind1 == IK1_WEAPON )
		if( CItemElem::IsEleRefineryAble(pProp) )
		{
//			if( pProp->dwReferStat1 == WEAPON_GENERAL )
//			{
				if(m_pItemElem != NULL)
					m_pItemElem->SetExtra(0);

				m_pItemElem = pItemElem;
				m_pEItemProp = m_pItemElem->GetProp();
				m_pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, m_pEItemProp->szIcon), 0xffff00ff );
				m_pItemElem->SetExtra(m_pItemElem->GetExtra()+1);
				CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
				pButton->EnableWindow(TRUE);
//			}
		}
	}
	else
		g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_REMOVE_ERROR ) ); //Error Message
}

BOOL CWndRemoveAttribute::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_START )
	{
		if(m_pWndConfirm == NULL)
		{
			m_pWndConfirm = new CWndRemoveAttributeConfirm;
			m_pWndConfirm->Initialize(this);
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndRemoveAttribute::SetDescription( CHAR* szChar )
{
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "RemoveAttribute.inc" ) ));
	szChar = scanner.m_pProg;
	
	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString( szChar );
		m_pText->ResetString();	
	}
}

void CWndRemoveAttribute::ReceiveResult(BOOL result)
{
	if(result)
		g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_REMOVE_ATTRIBUTE_CONFIRM ) );

	Destroy();
}

//////////////////////////////////////////////////////////////////////////
//	CWndRemoveAttributeConfirm
//////////////////////////////////////////////////////////////////////////

CWndRemoveAttributeConfirm::CWndRemoveAttributeConfirm() 
{
}
 
CWndRemoveAttributeConfirm::~CWndRemoveAttributeConfirm() 
{
} 

void CWndRemoveAttributeConfirm::OnDestroy()
{
}

void CWndRemoveAttributeConfirm::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndRemoveAttributeConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	CWndText* pText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	pText->m_string.AddParsingString( prj.GetText( TID_GAME_REMOVE_ATTRIBUTE ) );
	pText->ResetString();	
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndRemoveAttributeConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_REMOVE_ATTRIBUTE_CONFIRM, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndRemoveAttributeConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndRemoveAttributeConfirm::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndRemoveAttributeConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndRemoveAttributeConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndRemoveAttributeConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	 CWndRemoveAttribute* pParentwnd = (CWndRemoveAttribute*)this->m_pParentWnd;

	if( nID == WIDC_YES )
	{
		//SEND to Server
		if(pParentwnd && pParentwnd->m_pItemElem)
			g_DPlay.SendRemoveAttribute(pParentwnd->m_pItemElem->m_dwObjId);
	}
	else if( nID == WIDC_NO || nID == WTBID_CLOSE )
	{
		pParentwnd->m_pWndConfirm = NULL;
		Destroy();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

#endif //__REMOVE_ATTRIBUTE


#if __VER >= 11 // __PIERCING_REMOVE

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndRemovePiercing Class
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndRemovePiercing::CWndRemovePiercing() 
{
	m_pItemElem = NULL;
	m_pEItemProp = NULL;
	m_pTexture = NULL;
}

CWndRemovePiercing::~CWndRemovePiercing() 
{ 
} 

void CWndRemovePiercing::OnDestroy()
{
	if(m_pItemElem != NULL)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pItemElem ) )
			m_pItemElem->SetExtra(0);
	}
}

void CWndRemovePiercing::OnDraw( C2DRender* p2DRender ) 
{
	ItemProp* pItemProp;
	
	if(m_pItemElem != NULL)
	{
		pItemProp = m_pItemElem->GetProp();
		LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_PIC_SLOT );
		if(pItemProp != NULL)
		{	
			if(m_pTexture != NULL)
				m_pTexture->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
#if __VER >= 12 // __CSC_VER12_4
			int nMaxPiercing;
			CRect rect;
			LPWNDCTRL lpWndCtrl;

			// Suit - 4 Slot Use
			if(pItemProp->dwItemKind3 == IK3_SUIT)
			{
				nMaxPiercing = 4;
				D3DXCOLOR color =  D3DCOLOR_ARGB( 60, 240, 0, 0 );

				for(int i=4; i<10; i++)
				{
					lpWndCtrl = GetWndCtrl( m_nInfoSlot[i] );
					rect = lpWndCtrl->rect;
					p2DRender->RenderFillRect( rect, color );
				}
			}
			else
				nMaxPiercing = 10;

			// Render Piercing Options
			CString textOpt;
			CString strTemp;

			int nPiercingSize = m_pItemElem->GetPiercingSize();

			for(int i=0; i<nPiercingSize; i++)
			{
				if(nPiercingSize > nMaxPiercing)
					break;

				PPIERCINGAVAIL ptr = NULL;
				ptr = CPiercingAvail::GetInstance()->GetPiercingAvail( m_pItemElem->GetPiercingItem( i ) );

				if(ptr != NULL)
				{
					for(int j=0; j<ptr->nSize; j++)
					{
						int nDst = (int)ptr->anDstParam[j];
						int nAdj = (int)ptr->anAdjParam[j];
						
						if( g_WndMng.IsDstRate(nDst) )
						{
							if( nDst == DST_ATTACKSPEED )
								strTemp.Format( "%s%+d%%  ", prj.GetText(g_WndMng.GetDSTStringId( nDst )), nAdj / 2 / 10 );
							else
								strTemp.Format( "%s%+d%%  ", prj.GetText(g_WndMng.GetDSTStringId( nDst )), nAdj );
						}
						else
							strTemp.Format( "%s+%d  ", prj.GetText(g_WndMng.GetDSTStringId( nDst )), nAdj );

						textOpt += strTemp;
					}
					
					lpWndCtrl = GetWndCtrl( m_nInfoSlot[i] );
					p2DRender->TextOut( lpWndCtrl->rect.left + 10, lpWndCtrl->rect.top + 8, textOpt, D3DCOLOR_ARGB(255,0,0,0) );
					
					textOpt = "";
					strTemp = "";
				}
			}
#endif //__CSC_VER12_4
		}
	} 
}

void CWndRemovePiercing::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);

	if(::GetLanguage() == LANG_FRE)
		pButton->SetTexture(g_Neuz.m_pd3dDevice, MakePath( "Theme\\", ::GetLanguage(), _T( "ButtNormal00.tga" ) ), TRUE);

	pButton->EnableWindow(FALSE);
	m_pText = (CWndText*)GetDlgItem( WIDC_TEXT1 );

	SetDescription(NULL);

#if __VER >= 12 // __CSC_VER12_4
	m_nInfoSlot[0] = WIDC_STATIC_PIERCING1;
	m_nInfoSlot[1] = WIDC_STATIC_PIERCING2;
	m_nInfoSlot[2] = WIDC_STATIC_PIERCING3;
	m_nInfoSlot[3] = WIDC_STATIC_PIERCING4;
	m_nInfoSlot[4] = WIDC_STATIC_PIERCING5;
	m_nInfoSlot[5] = WIDC_STATIC_PIERCING6;
	m_nInfoSlot[6] = WIDC_STATIC_PIERCING7;
	m_nInfoSlot[7] = WIDC_STATIC_PIERCING8;
	m_nInfoSlot[8] = WIDC_STATIC_PIERCING9;
	m_nInfoSlot[9] = WIDC_STATIC_PIERCING10;
#endif //__CSC_VER12_4

	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndRemovePiercing::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
#if __VER >= 12 // __CSC_VER12_4
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_SMELT_REMOVE_PIERCING_EX, 0, CPoint( 0, 0 ), pWndParent );
#else //__CSC_VER12_4
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_SMELT_REMOVE_PIERCING, 0, CPoint( 0, 0 ), pWndParent );
#endif //__CSC_VER12_4
} 
BOOL CWndRemovePiercing::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndRemovePiercing::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndRemovePiercing::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndRemovePiercing::OnLButtonDown( UINT nFlags, CPoint point ) 
{
}

void CWndRemovePiercing::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	if(!m_pItemElem) return;
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_PIC_SLOT );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{
		if(m_pItemElem)
		{
			m_pItemElem->SetExtra(0);
			CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
			pButton->EnableWindow(FALSE);
			m_pItemElem = NULL;
			m_pEItemProp = NULL;
			m_pTexture = NULL;
		}
	}
}

#if __VER >= 12 // __CSC_VER12_4
void CWndRemovePiercing::OnMouseWndSurface(CPoint point)
{
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_PIC_SLOT );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{
		ClientToScreen( &point );
		ClientToScreen( &rect );
		g_toolTip.PutToolTip( (DWORD)this, prj.GetText(TID_GAME_TOOLTIP_PIERCINGITEM), rect, point );
	}
}
#endif //__CSC_VER12_4

BOOL CWndRemovePiercing::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_PIC_SLOT );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{		
		ItemProp* pItemProp;
		CItemElem* pTempElem;
		pTempElem  = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
		
		if(m_pItemElem == NULL && pTempElem != NULL)
		{
			pItemProp = pTempElem->GetProp();
#if __VER >= 12 // __EXT_PIERCING
			if( pTempElem->IsPierceAble() && pTempElem->GetPiercingItem( 0 ) != 0 )
#else // __EXT_PIERCING
			if( pItemProp->dwItemKind3 == IK3_SUIT && pTempElem->GetPiercingItem( 0 ) != 0 )
#endif // __EXT_PIERCING
			{
				m_pItemElem = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
				m_pEItemProp = m_pItemElem->GetProp();
				m_pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, m_pEItemProp->szIcon), 0xffff00ff );
				m_pItemElem->SetExtra(m_pItemElem->GetExtra()+1);
				CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
				pButton->EnableWindow(TRUE);
			}
			else
#if __VER >= 12 // __CSC_VER12_4
				g_WndMng.PutString( prj.GetText( TID_GAME_REMOVE_PIERCING_ERROR_EX ), NULL, prj.GetTextColor( TID_GAME_REMOVE_PIERCING_ERROR_EX ) );
#else //__CSC_VER12_4
				g_WndMng.PutString( prj.GetText( TID_GAME_REMOVE_PIERCING_ERROR ), NULL, prj.GetTextColor( TID_GAME_REMOVE_PIERCING_ERROR ) );
#endif //__CSC_VER12_4
		}
	}
	return TRUE;
}

#if __VER >= 12 // __CSC_VER12_4
void CWndRemovePiercing::SetItem(CItemElem* pItemElem)
#else //__CSC_VER12_4
void CWndRemovePiercing::SetSuit(CItemElem* pItemElem)
#endif //__CSC_VER12_4
{
	if(m_pItemElem == NULL && pItemElem != NULL)
	{
		ItemProp* pProp = pItemElem->GetProp();
#if __VER >= 12 // __EXT_PIERCING
		if( pItemElem->IsPierceAble() && pItemElem->GetPiercingItem( 0 ) != 0 )
#else // __EXT_PIERCING
		if( pProp->dwItemKind3 == IK3_SUIT && pItemElem->GetPiercingItem( 0 ) != 0 )
#endif // __EXT_PIERCING
		{
			m_pItemElem = pItemElem;
			m_pEItemProp = m_pItemElem->GetProp();
			m_pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, m_pEItemProp->szIcon), 0xffff00ff );
			m_pItemElem->SetExtra(m_pItemElem->GetExtra()+1);
			CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
			pButton->EnableWindow(TRUE);
		}
		else
#if __VER >= 12 // __CSC_VER12_4
			g_WndMng.PutString( prj.GetText( TID_GAME_REMOVE_PIERCING_ERROR_EX ), NULL, prj.GetTextColor( TID_GAME_REMOVE_PIERCING_ERROR_EX ) );			
#else //__CSC_VER12_4
			g_WndMng.PutString( prj.GetText( TID_GAME_REMOVE_PIERCING_ERROR ), NULL, prj.GetTextColor( TID_GAME_REMOVE_PIERCING_ERROR ) );			
#endif //__CSC_VER12_4
	}
}

BOOL CWndRemovePiercing::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_START )
	{
		//서버로 시작을 알린다.
		if(m_pItemElem != NULL)
		{
			CWndButton* pButton;
			pButton = (CWndButton*)GetDlgItem( WIDC_START );
			pButton->EnableWindow(FALSE);

			g_DPlay.SendPiercingRemove(m_pItemElem->m_dwObjId);
			Destroy();
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndRemovePiercing::SetDescription( CHAR* szChar )
{
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "SmeltRemovePiercing.inc" ) ));
	szChar = scanner.m_pProg;
	
	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString( szChar );
		m_pText->ResetString();	
	}
}

#if __VER >= 12 // __CSC_VER12_4
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndRemoveJewel Class
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndRemoveJewel::CWndRemoveJewel() 
{
	m_pItemElem = NULL;
	m_pMoonstone = NULL;
	m_pEItemProp = NULL;
	m_pTexture = NULL;
	m_pMoonstoneTex = NULL;

	ResetJewel();
}

CWndRemoveJewel::~CWndRemoveJewel() 
{ 
} 

void CWndRemoveJewel::OnDestroy()
{
	if(m_pItemElem != NULL)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pItemElem ) )
			m_pItemElem->SetExtra(0);
	}

	if(m_pMoonstone != NULL)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pMoonstone ) )
			m_pMoonstone->SetExtra(0);
	}
}

void CWndRemoveJewel::OnDraw( C2DRender* p2DRender ) 
{
	ItemProp* pItemProp;
	LPWNDCTRL wndCtrl;
	
	// Render Moonstone Bg
	if(m_pMoonstone != NULL)
	{
		wndCtrl = GetWndCtrl( WIDC_PIC_SLOT1 );
		m_pMoonstoneTex->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
	}
	else
	{
		wndCtrl = GetWndCtrl( WIDC_PIC_SLOT1 );
		m_pMoonstoneTex->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ), 50 );
	}
	
	// Render Item
	if(m_pItemElem != NULL)
	{
		pItemProp = m_pItemElem->GetProp();
		wndCtrl = GetWndCtrl( WIDC_PIC_SLOT );
		if(pItemProp != NULL)
		{	
			if(m_pTexture != NULL)
				m_pTexture->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
		}

		// Render Jewel Opt
		for(int i=0; i<m_nJewelCount; i++)
		{
			if(m_nJewelCount > 5)
				break;
			
			//Jewel Render
			if(m_nJewelID[i] != 0)
			{
				wndCtrl = GetWndCtrl( m_nJewelSlot[i] );

				if(m_pJewelTex[i])
					m_pJewelTex[i]->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );

				//Text Render
				CString DstText;
				DWORD dwColorbuf;
				
				if(m_nJewelID[i] >= II_GEN_MAT_DIAMOND01 && m_nJewelID[i] <= II_GEN_MAT_TOPAZ01) //HP 상승
				{
					dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt1;
					DstText.Format( "%s", prj.GetText(TID_TOOLTIP_DST_HP_MAX) );
				}
				else if(m_nJewelID[i] >= II_GEN_MAT_DIAMOND02 && m_nJewelID[i] <= II_GEN_MAT_TOPAZ02) //공격력 상승
				{
					dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt2;
					DstText.Format( "%s", prj.GetText(TID_TOOLTIP_DST_ATKPOWER) );
				}
				else if(m_nJewelID[i] >= II_GEN_MAT_DIAMOND03 && m_nJewelID[i] <= II_GEN_MAT_TOPAZ03) //방어력 상승
				{
					dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt3;
					DstText.Format( "%s", prj.GetText(TID_TOOLTIP_DST_ADJDEF) );
				}
				else if(m_nJewelID[i] >= II_GEN_MAT_DIAMOND04 && m_nJewelID[i] <= II_GEN_MAT_TOPAZ04) //흡혈
				{
					dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt4;
					DstText.Format( "%s", prj.GetText(TID_TOOLTIP_DST_MELEE_STEALHP) );
				}
				else if(m_nJewelID[i] >= II_GEN_MAT_DIAMOND05 && m_nJewelID[i] <= II_GEN_MAT_TOPAZ05) //PvP시 대미지 증가
				{
					dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt5;
					DstText.Format( "%s", prj.GetText(TID_TOOLTIP_DST_PVP_DMG) );
				}
				else if(m_nJewelID[i] >= II_GEN_MAT_DIAMOND06 && m_nJewelID[i] <= II_GEN_MAT_TOPAZ06) //힘증가
				{
					dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt6;
					DstText.Format( "%s", prj.GetText(TID_TOOLTIP_STR) );
				}
				else if(m_nJewelID[i] >= II_GEN_MAT_DIAMOND07 && m_nJewelID[i] <= II_GEN_MAT_TOPAZ07) //체력증가
				{
					dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt7;
					DstText.Format( "%s", prj.GetText(TID_TOOLTIP_STA) );
				}
				else if(m_nJewelID[i] >= II_GEN_MAT_DIAMOND08 && m_nJewelID[i] <= II_GEN_MAT_TOPAZ08) //민첩증가
				{
					dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt8;
					DstText.Format( "%s", prj.GetText(TID_TOOLTIP_DEX) );
				}
				else if(m_nJewelID[i] >= II_GEN_MAT_DIAMOND09 && m_nJewelID[i] <= II_GEN_MAT_TOPAZ09) //지능증가
				{
					dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt9;
					DstText.Format( "%s", prj.GetText(TID_TOOLTIP_INT) );
				}

				wndCtrl = GetWndCtrl( m_nInfoSlot[i] );
				p2DRender->TextOut( wndCtrl->rect.left + 10, wndCtrl->rect.top + 10, DstText, dwColorbuf );					
			}
		}
	}
}

void CWndRemoveJewel::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	ItemProp* pItemProp = prj.GetItemProp( II_GEN_MAT_MOONSTONE );
	if(pItemProp)
		m_pMoonstoneTex = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);

	if(::GetLanguage() == LANG_FRE)
		pButton->SetTexture(g_Neuz.m_pd3dDevice, MakePath( "Theme\\", ::GetLanguage(), _T( "ButtNormal00.tga" ) ), TRUE);

	pButton->EnableWindow(FALSE);
	m_pText = (CWndText*)GetDlgItem( WIDC_TEXT1 );

	m_nJewelSlot[0] = WIDC_JEWEL_SLOT1;
	m_nJewelSlot[1] = WIDC_JEWEL_SLOT2;
	m_nJewelSlot[2] = WIDC_JEWEL_SLOT3;
	m_nJewelSlot[3] = WIDC_JEWEL_SLOT4;
	m_nJewelSlot[4] = WIDC_JEWEL_SLOT5;

	m_nInfoSlot[0] = WIDC_JEWEL_SLOT_INFO1;
	m_nInfoSlot[1] = WIDC_JEWEL_SLOT_INFO2;
	m_nInfoSlot[2] = WIDC_JEWEL_SLOT_INFO3;
	m_nInfoSlot[3] = WIDC_JEWEL_SLOT_INFO4;
	m_nInfoSlot[4] = WIDC_JEWEL_SLOT_INFO5;

	SetDescription(NULL);

	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndRemoveJewel::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_SMELT_REMOVE_JEWEL, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndRemoveJewel::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndRemoveJewel::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndRemoveJewel::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndRemoveJewel::OnLButtonDown( UINT nFlags, CPoint point ) 
{
}

void CWndRemoveJewel::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_PIC_SLOT );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{
		if(m_pItemElem)
		{
			m_pItemElem->SetExtra(0);
			m_pItemElem = NULL;
			m_pEItemProp = NULL;
			m_pTexture = NULL;

			ResetJewel();
		}
	}

	wndCtrl = GetWndCtrl( WIDC_PIC_SLOT1 );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{
		if(m_pMoonstone)
		{
			m_pMoonstone->SetExtra(0);
			m_pMoonstone = NULL;
		}
	}

	if(m_pItemElem == NULL || m_pMoonstone == NULL)
	{
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
		pButton->EnableWindow(FALSE);
	}
}

void CWndRemoveJewel::OnMouseWndSurface(CPoint point)
{
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_PIC_SLOT );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{
		ClientToScreen( &point );
		ClientToScreen( &rect );
		g_toolTip.PutToolTip( (DWORD)this, prj.GetText(TID_GAME_TOOLTIP_REMOVEJEWEL1), rect, point );
	}

	wndCtrl = GetWndCtrl( WIDC_PIC_SLOT1 );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{
		ClientToScreen( &point );
		ClientToScreen( &rect );
		g_toolTip.PutToolTip( (DWORD)this, prj.GetText(TID_GAME_TOOLTIP_REMOVEJEWEL2), rect, point );
	}

	wndCtrl = GetWndCtrl( WIDC_JEWEL_SLOT1 );
	rect.left = wndCtrl->rect.left;
	rect.top = wndCtrl->rect.top;
	wndCtrl = GetWndCtrl( WIDC_JEWEL_SLOT_INFO5 );
	rect.right = wndCtrl->rect.right;
	rect.bottom = wndCtrl->rect.bottom;
	if( rect.PtInRect( point ) )
	{		
		ClientToScreen( &point );
		ClientToScreen( &rect );
		g_toolTip.PutToolTip( (DWORD)this, prj.GetText(TID_GAME_TOOLTIP_REMOVEJEWEL3), rect, point );
	}
}

BOOL CWndRemoveJewel::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CRect rect;
	ItemProp* pItemProp;
	CItemElem* pTempElem;

	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_PIC_SLOT );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{		
		pTempElem  = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
		
		if(m_pItemElem == NULL && pTempElem != NULL)
		{
			pItemProp = pTempElem->GetProp();
			
			if(pTempElem && (pItemProp->dwItemKind2 == IK2_WEAPON_DIRECT || pItemProp->dwItemKind2 == IK2_WEAPON_MAGIC) &&
				pItemProp->dwReferStat1 == WEAPON_ULTIMATE && pTempElem->GetUltimatePiercingSize() > 0)
			{
				m_pItemElem = pTempElem;
				m_pEItemProp = pItemProp;
				m_pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, m_pEItemProp->szIcon), 0xffff00ff );
				m_pItemElem->SetExtra(m_pItemElem->GetExtra()+1);

				m_nJewelCount = pTempElem->GetUltimatePiercingSize();
				for(int i=0; i<m_nJewelCount; i++)
				{
					m_nJewelID[i] = m_pItemElem->GetUltimatePiercingItem(i);
					pItemProp = prj.GetItemProp( m_nJewelID[i] );
					if(pItemProp != NULL)
						m_pJewelTex[i] = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
				}
			}
			else
				g_WndMng.PutString( prj.GetText( TID_GAME_REMOVE_JEWEL_ERROR2 ), NULL, prj.GetTextColor( TID_GAME_REMOVE_JEWEL_ERROR2 ) );
		}
	}

	wndCtrl = GetWndCtrl( WIDC_PIC_SLOT1 );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{
		pTempElem  = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
		
		if(m_pMoonstone == NULL && pTempElem != NULL)
		{
			ItemProp* pItemProp = pTempElem->GetProp();
			if(pItemProp->dwID == II_GEN_MAT_MOONSTONE || pItemProp->dwID ==II_GEN_MAT_MOONSTONE_1)
			{
				m_pMoonstone = pTempElem;
				m_pMoonstone->SetExtra(m_pMoonstone->GetExtra()+1);
			}
			else
				g_WndMng.PutString( prj.GetText( TID_GAME_REMOVE_JEWEL_ERROR1 ), NULL, prj.GetTextColor( TID_GAME_REMOVE_JEWEL_ERROR1 ) );
		}
	}

	if(m_pItemElem && m_pMoonstone)
	{
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
		pButton->EnableWindow(TRUE);
	}

	return TRUE;
}

void CWndRemoveJewel::SetItem(CItemElem* pItemElem)
{
	if(pItemElem != NULL)
	{
		ItemProp* pItemProp = pItemElem->GetProp();

		if(m_pMoonstone == NULL && pItemProp->dwItemKind2 == IK2_MATERIAL)
		{
			if(pItemProp->dwID == II_GEN_MAT_MOONSTONE || pItemProp->dwID ==II_GEN_MAT_MOONSTONE_1)
			{
				m_pMoonstone = pItemElem;
				m_pMoonstone->SetExtra(m_pMoonstone->GetExtra()+1);
			}
			else
				g_WndMng.PutString( prj.GetText( TID_GAME_REMOVE_JEWEL_ERROR1 ), NULL, prj.GetTextColor( TID_GAME_REMOVE_JEWEL_ERROR1 ) );
		}
		else if(m_pItemElem == NULL)
		{
			if(pItemElem && (pItemProp->dwItemKind2 == IK2_WEAPON_DIRECT || pItemProp->dwItemKind2 == IK2_WEAPON_MAGIC) &&
				pItemProp->dwReferStat1 == WEAPON_ULTIMATE && pItemElem->GetUltimatePiercingSize() > 0)
			{
				m_pItemElem = pItemElem;
				m_pEItemProp = pItemProp;
				m_pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, m_pEItemProp->szIcon), 0xffff00ff );
				m_pItemElem->SetExtra(m_pItemElem->GetExtra()+1);

				m_nJewelCount = pItemElem->GetUltimatePiercingSize();
				for(int i=0; i<m_nJewelCount; i++)
				{
					m_nJewelID[i] = m_pItemElem->GetUltimatePiercingItem(i);
					pItemProp = prj.GetItemProp( m_nJewelID[i] );
					if(pItemProp != NULL)
						m_pJewelTex[i] = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
				}
			}
			else
				g_WndMng.PutString( prj.GetText( TID_GAME_REMOVE_JEWEL_ERROR2 ), NULL, prj.GetTextColor( TID_GAME_REMOVE_JEWEL_ERROR2 ) );			
		}
	}
	
	if(m_pItemElem && m_pMoonstone)
	{
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
		pButton->EnableWindow(TRUE);
	}
}

BOOL CWndRemoveJewel::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_START )
	{
		//서버로 시작을 알린다.
		if(m_pItemElem != NULL && m_pMoonstone != NULL)
		{
			CWndButton* pButton;
			pButton = (CWndButton*)GetDlgItem( WIDC_START );
			pButton->EnableWindow(FALSE);

			g_DPlay.SendUltimateRemoveGem( m_pItemElem->m_dwObjId, m_pMoonstone->m_dwObjId );
			Destroy();
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndRemoveJewel::SetDescription( CHAR* szChar )
{
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "SmeltRemoveJewel.inc" ) ));
	szChar = scanner.m_pProg;
	
	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString( szChar );
		m_pText->ResetString();	
	}
}

void CWndRemoveJewel::ResetJewel()
{
	m_nJewelCount = 0;

	for(int i=0; i<5; i++)
	{
		m_nJewelID[i] = 0;
		m_pJewelTex[i] = NULL;
	}
}

#endif //__CSC_VER12_4


#if __VER >= 13 // __EXT_ENCHANT

//////////////////////////////////////////////////////////////////////////
// Change Attribute Window
//////////////////////////////////////////////////////////////////////////

CWndChangeAttribute::CWndChangeAttribute()
{
	m_pText = NULL;
	m_nAttributeNum = -1;
	m_pItemElem = NULL;
	m_pChangeItem = NULL;
	m_pTexture = NULL;
}

CWndChangeAttribute::~CWndChangeAttribute()
{
	if( m_pItemElem )
		m_pItemElem->SetExtra( 0 );
	if( m_pChangeItem )
		m_pChangeItem->SetExtra( 0 );
}

void CWndChangeAttribute::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	m_pText = (CWndText *)GetDlgItem( WIDC_TEXT1 );
	
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "ChangeAttribute.inc" ) ));
	CHAR* szChar = scanner.m_pProg;

	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString(szChar);
		m_pText->ResetString();	
	}

	m_nAttributeStaticID[0] = WIDC_CUSTOM1;
	m_nAttributeStaticID[1] = WIDC_CUSTOM2;
	m_nAttributeStaticID[2] = WIDC_CUSTOM3;
	m_nAttributeStaticID[3] = WIDC_CUSTOM4;
	m_nAttributeStaticID[4] = WIDC_CUSTOM5;

	m_nTooltipTextIndx[0] = TID_TOOLTIP_CA_INSLOT;
	m_nTooltipTextIndx[1] = TID_TOOLTIP_CA_FIRE;
	m_nTooltipTextIndx[2] = TID_TOOLTIP_CA_WATER;
	m_nTooltipTextIndx[3] = TID_TOOLTIP_CA_ELEC;
	m_nTooltipTextIndx[4] = TID_TOOLTIP_CA_WIND;
	m_nTooltipTextIndx[5] = TID_TOOLTIP_CA_EARTH;

	MoveParentCenter();
} 

BOOL CWndChangeAttribute::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_CHANGE_ATTRIBUTE, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndChangeAttribute::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if(nID == WIDC_OK)
	{
		if(m_pItemElem != NULL && m_pChangeItem != NULL && m_nAttributeNum > -1 && m_nAttributeNum < 5)
			g_DPlay.SendChangeAttribute(m_pItemElem->m_dwObjId, m_pChangeItem->m_dwObjId, m_nAttributeNum + 1);

		if(m_pItemElem)
			m_pItemElem->SetExtra(0);

		Destroy();
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndChangeAttribute::OnDraw(C2DRender* p2DRender)
{
	// Draw Item
	if(m_pItemElem != NULL)
	{
		if(m_pTexture != NULL)
			m_pTexture->Render( p2DRender, CPoint( GetWndCtrl( WIDC_STATIC1 )->rect.left, GetWndCtrl( WIDC_STATIC1 )->rect.top ) );
	}

	// Draw Attribute & Draw Rect Round Lamp
	for(int i=0; i<5; i++)
	{
		CRect rect = GetWndCtrl( m_nAttributeStaticID[i] )->rect;
		g_WndMng.m_pWndWorld->m_texAttrIcon.Render( p2DRender, CPoint(rect.left, rect.top), i, 255, 1.5f, 1.5f );
		
		if(i == m_nAttributeNum)
			FillRect( p2DRender, GetWndCtrl( m_nAttributeStaticID[i] )->rect, 0xffbb00ff, 0x00bb00ff );
	}
}

BOOL CWndChangeAttribute::Process()
{
	CRect rect;
	LPWNDCTRL lpWndCtrl;
	CPoint ptMouse = GetMousePoint();

	lpWndCtrl = GetWndCtrl(WIDC_STATIC1);
	rect = lpWndCtrl->rect;
	if(rect.PtInRect(ptMouse))
	{
		ClientToScreen( &ptMouse );
		ClientToScreen( &rect );
		g_toolTip.PutToolTip( 10000, prj.GetText(m_nTooltipTextIndx[0]), rect, ptMouse, 1 );
	}

	for(int i=0; i<5; i++) 
	{
		lpWndCtrl = GetWndCtrl(m_nAttributeStaticID[i]);
		rect = lpWndCtrl->rect;
		if(rect.PtInRect(ptMouse))
		{
			ClientToScreen( &ptMouse );
			ClientToScreen( &rect );
			g_toolTip.PutToolTip( 10000, prj.GetText(m_nTooltipTextIndx[i+1]), rect, ptMouse, 1 );
		}
	}
	
	return TRUE;
}

void CWndChangeAttribute::OnLButtonUp( UINT nFlags, CPoint point )
{
	CRect rect;
	LPWNDCTRL lpWndCtrl;

	for(int i=0; i<5; i++) 
	{
		lpWndCtrl = GetWndCtrl(m_nAttributeStaticID[i]);
		rect = lpWndCtrl->rect;
		if(rect.PtInRect(point))
			m_nAttributeNum = i;
	}
}

BOOL CWndChangeAttribute::OnSetCursor( CWndBase* pWndBase, UINT nHitTest, UINT message )
{
	CRect rect;
	LPWNDCTRL lpWndCtrl;
	BOOL bOnTitle = FALSE;
	CPoint point = GetMousePoint();

	for(int i=0; i<5; i++) 
	{
		lpWndCtrl = GetWndCtrl( m_nAttributeStaticID[i] );
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

void CWndChangeAttribute::SetChangeItem( CItemElem* pItemElem )
{
	pItemElem->SetExtra( pItemElem->GetExtra() + 1 );
	m_pChangeItem = pItemElem;
}

void CWndChangeAttribute::FillRect(C2DRender *p2DRender, CRect rectBg, DWORD dwColorstart, DWORD dwColorend)
{
	CPoint Point = CPoint( rectBg.TopLeft().x, rectBg.TopLeft().y );
	float fVar = 0.04f;

	D3DXCOLOR dwColorDest2 = dwColorstart;
	D3DXCOLOR dwColor = dwColorend;
	D3DXCOLOR dwColor1 = D3DCOLOR_ARGB( 0, 255, 255, 255 );
	D3DXCOLOR dwColor2 = D3DCOLOR_ARGB( 0, 255, 255, 255 );

	float fLerp = 1.0f;

	D3DXColorLerp( &dwColor2, &dwColor, &dwColorDest2, fLerp );
	int nthick = 8;	
	CRect Rect = CRect( 0, 0, rectBg.right - rectBg.left, rectBg.bottom - rectBg.top );
	
	p2DRender->RenderFillRect( CRect( Point.x+Rect.left-2, Point.y+Rect.top-2, Point.x+Rect.left+nthick-2, Point.y+Rect.bottom+2 ),
		dwColor2, dwColor1, dwColor2, dwColor1 );
	
	p2DRender->RenderFillRect( CRect( Point.x+Rect.left-2, Point.y+Rect.top+nthick-2, Point.x+Rect.right+2, Point.y+Rect.top-2 ),
		dwColor1, dwColor1, dwColor2, dwColor2 );
	
	p2DRender->RenderFillRect( CRect( Point.x+Rect.right+2, Point.y+Rect.top-2, Point.x+Rect.right-nthick+2, Point.y+Rect.bottom+2 ),
		dwColor2, dwColor1, dwColor2, dwColor1 );
	
	p2DRender->RenderFillRect( CRect( Point.x+Rect.left-2, Point.y+Rect.bottom+2, Point.x+Rect.right+2, Point.y+Rect.bottom-nthick+2 ),
		dwColor2, dwColor2, dwColor1, dwColor1 );
}

BOOL CWndChangeAttribute::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CWndBase* pWndFrame = pShortcut->m_pFromWnd->GetFrameWnd();
	if( pWndFrame == NULL )
		return FALSE;

	if( pWndFrame->GetWndId() != APP_INVENTORY )
	{
		SetForbid( TRUE );
		return FALSE;
	}

	CRect rect;
	ItemProp* pItemProp;
	CItemElem* pTempElem;

	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC1 );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{		
		pTempElem  = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
		
		if(m_pItemElem == NULL && pTempElem != NULL)
		{
			pItemProp = pTempElem->GetProp();
			
			if(pTempElem && CItemElem::IsEleRefineryAble(pItemProp) && pTempElem->m_nResistAbilityOption > 0)
			{
				m_pItemElem = pTempElem;
				m_pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
				m_pItemElem->SetExtra(m_pItemElem->GetExtra()+1);
			}
			else
				g_WndMng.PutString( prj.GetText( TID_GAME_NOTELEMENT  ), NULL, prj.GetTextColor( TID_GAME_NOTELEMENT  ) );
		}
	}

	return TRUE;
}

void CWndChangeAttribute::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC1 );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{
		if(m_pItemElem)
		{
			m_pItemElem->SetExtra(0);
			m_pItemElem = NULL;
			m_pTexture = NULL;
		}
	}
}

#endif //__EXT_ENCHANT

#if __VER >= 13 // __CSC_VER13_2

//////////////////////////////////////////////////////////////////////////
// Couple Message Window
//////////////////////////////////////////////////////////////////////////
BOOL CWndCoupleMessage::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	return CWndMessageBox::Initialize( m_strText, pWndParent, MB_OKCANCEL );
}

BOOL CWndCoupleMessage::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( m_nMode )
	{
		case CM_SENDPROPOSE:
			{
				if(nID == IDOK)
				{
					if(m_pTargetObj)
						g_DPlay.SendPropose(((CMover*)m_pTargetObj)->GetName());
					Destroy();
				}
				else if(nID == IDCANCEL)
					Destroy();					
			}
			break;
		case CM_RECEIVEPROPOSE:
			{
				if(nID == IDOK)
				{
					g_DPlay.SendCouple();
					Destroy();
				}
				else if(nID == IDCANCEL)
				{
					g_DPlay.SendRefuse();
					Destroy();
				}
			}
			break;
		case CM_CANCELCOUPLE:
			{
				if(nID == IDOK)
				{
					g_DPlay.SendDecouple();
					Destroy();
				}
				else if(nID == IDCANCEL)
					Destroy();
			}
			break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndCoupleMessage::SetMessageMod(CString strText, int nMode, CObj* pTargetObj)
{
	m_strText = strText;
	m_nMode = nMode;
	m_pTargetObj = pTargetObj;
}

//////////////////////////////////////////////////////////////////////////
// Couple Manager Tab Information Window
//////////////////////////////////////////////////////////////////////////
CWndCoupleTabInfo::CWndCoupleTabInfo()
{
	m_pCouple = NULL;
	m_pVBGauge = NULL;
}

CWndCoupleTabInfo::~CWndCoupleTabInfo()
{
}

HRESULT CWndCoupleTabInfo::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if( m_pVBGauge == NULL )
		return m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBGauge, NULL );
	return S_OK;
}
HRESULT CWndCoupleTabInfo::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
    SAFE_RELEASE( m_pVBGauge );
	return S_OK;
}
HRESULT CWndCoupleTabInfo::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	SAFE_RELEASE( m_pVBGauge );
	return S_OK;
}

void CWndCoupleTabInfo::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
#ifdef __GAUGE_V19
	m_texGauEmptyNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "BarBg.tga" ), 0xffff00ff, TRUE );
	m_texGauFillNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "BarSky.tga" ), 0xffff00ff, TRUE );
#else
	m_texGauEmptyNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptyNormal.bmp" ), 0xffff00ff, TRUE );
	m_texGauFillNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauFillNormal.bmp" ), 0xffff00ff, TRUE );
#endif//__GAUGE_V19
	CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	if(pWndButton)
	{
#ifdef __NO_SUB_LANG
		if(::GetLanguage() == LANG_USA || ::GetLanguage() == LANG_VTN)
#else // __NO_SUB_LANG
		if(::GetLanguage() == LANG_USA || ::GetLanguage() == LANG_VTN)
#endif // __NO_SUB_LANG
			pWndButton->SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtNormal02.tga" ), 0xffff00ff );
		else
			pWndButton->SetTexture( m_pApp->m_pd3dDevice, MakePath( "Theme\\", ::GetLanguage(), "ButtNormal02.tga" ), 0xffff00ff );
	}

	MoveParentCenter();
} 

BOOL CWndCoupleTabInfo::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_COUPLE_TAB_INFO, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndCoupleTabInfo::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	if(nID == WIDC_BUTTON1)
	{
		if(m_pCouple)
		{
			// 프러포즈 해제 메세지 창 출력
			if(g_WndMng.m_pWndCoupleMessage)
				SAFE_DELETE(g_WndMng.m_pWndCoupleMessage);

			g_WndMng.m_pWndCoupleMessage = new CWndCoupleMessage;
			if(g_WndMng.m_pWndCoupleMessage)
			{
				u_long idPartner = m_pCouple->GetPartner(g_pPlayer->m_idPlayer);
				PlayerData* pData = CPlayerDataCenter::GetInstance()->GetPlayerData(idPartner);

				if(pData)
				{
					CString strText;
					strText.Format(prj.GetText(TID_GAME_COUPLECANCEL), pData->szPlayer);
					g_WndMng.m_pWndCoupleMessage->SetMessageMod(strText, CWndCoupleMessage::CM_CANCELCOUPLE);
					g_WndMng.m_pWndCoupleMessage->Initialize(NULL);
				}
			}
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndCoupleTabInfo::OnDraw(C2DRender* p2DRender)
{
	TEXTUREVERTEX2* pVertex = new TEXTUREVERTEX2[6];
	TEXTUREVERTEX2* pVertices = pVertex;

	//Couple Info & Exp Gauge Draw
	CRect rect, rectTemp;
	LPWNDCTRL lpStatic = GetWndCtrl( WIDC_STATIC4 );
	rect.TopLeft().y = lpStatic->rect.top + 36;
	rect.TopLeft().x = lpStatic->rect.left + 70;
	rect.BottomRight().y = lpStatic->rect.top + 48;
	rect.BottomRight().x = lpStatic->rect.right - 10;

	m_pCouple = CCoupleHelper::Instance()->GetCouple();
	if(m_pCouple)
	{
		u_long idPartner = m_pCouple->GetPartner(g_pPlayer->m_idPlayer);
		PlayerData* pData = CPlayerDataCenter::GetInstance()->GetPlayerData(idPartner);

		if(pData)
		{
			CString strTemp;
			CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_NAME);
			pWndStatic->m_dwColor = 0xff3f3f3f;
			pWndStatic->SetTitle(pData->szPlayer);
			pWndStatic = (CWndStatic*)GetDlgItem(WIDC_JOB);
			pWndStatic->m_dwColor = 0xff3f3f3f;
			pWndStatic->SetTitle(prj.m_aJob[ pData->data.nJob ].szName);
			pWndStatic = (CWndStatic*)GetDlgItem(WIDC_LEVEL);
			pWndStatic->m_dwColor = 0xff3f3f3f;
			strTemp.Format("%d", pData->data.nLevel);
			pWndStatic->SetTitle(strTemp);
			pWndStatic = (CWndStatic*)GetDlgItem(WIDC_COUPLE_LEVEL);
			pWndStatic->m_dwColor = 0xff3f3f3f;
			strTemp.Format("%d", m_pCouple->GetLevel());
			pWndStatic->SetTitle(strTemp);
			pWndStatic = (CWndStatic*)GetDlgItem(WIDC_COUPLE_LEVEL);
			pWndStatic->m_dwColor = 0xff3f3f3f;
			strTemp.Format("%d", m_pCouple->GetLevel());
			pWndStatic->SetTitle(strTemp);

			// Draw Master/Hero Icon
			CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );

			if(pWndWorld)
			{
				LPWNDCTRL lpCtrl = GetWndCtrl(WIDC_LEVEL);
				CPoint ptJobType;
				ptJobType.x = lpCtrl->rect.right - 60;
				ptJobType.y = lpCtrl->rect.top - 1;

				if( prj.m_aJob[ pData->data.nJob ].dwJobType == JTYPE_MASTER )
				{
					int nMasterIndex = 27;
					if(/*m_nLevel >= 60 && */pData->data.nLevel < 70) //Level Down될 경우를 생각해서 주석처리.
						nMasterIndex = 27;
					else if(pData->data.nLevel >= 70 && pData->data.nLevel < 80)
						nMasterIndex = 28;
					else if(pData->data.nLevel >= 80 && pData->data.nLevel < 90)
						nMasterIndex = 29;
					else if(pData->data.nLevel >= 90 && pData->data.nLevel < 100)
						nMasterIndex = 30;
					else if(pData->data.nLevel >= 100 && pData->data.nLevel < 110)
						nMasterIndex = 31;
					else if(pData->data.nLevel >= 110 && pData->data.nLevel <= 120)
						nMasterIndex = 32;

					pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, ptJobType,  nMasterIndex, &pVertices, 0xffffffff );
				}
				else if( prj.m_aJob[ pData->data.nJob ].dwJobType == JTYPE_HERO )
					pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, ptJobType,  33, &pVertices, 0xffffffff );

				pWndWorld->m_texPlayerDataIcon.Render( m_pApp->m_pd3dDevice, pVertex, ( (int) pVertices - (int) pVertex ) / sizeof( TEXTUREVERTEX2 ) );
			}
		}

		float fRate = CCoupleProperty::Instance()->GetExperienceRate( m_pCouple->GetLevel(), m_pCouple->GetExperience() );

		//처음에 게이지 안차서 보정
		if(fRate > 0.00f && fRate < 0.04f)
			fRate = 0.05f;
		else if(fRate >= 0.04f && fRate <= 0.075f)
			fRate = 0.075f;

		int nWidth	= (int)( rect.Width() * fRate );
		rectTemp = rect; 
		rectTemp.right = rectTemp.left + nWidth;
		if( rect.right < rectTemp.right )
			rectTemp.right = rect.right;

//		CString stTrace;
//		stTrace.Format("fRate = %f nWidth = %d \n", fRate, nWidth);
//		TRACE(stTrace);
	}
	else
	{
		CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_NAME);
		pWndStatic->SetTitle("");
		pWndStatic = (CWndStatic*)GetDlgItem(WIDC_JOB);
		pWndStatic->SetTitle("");
		pWndStatic = (CWndStatic*)GetDlgItem(WIDC_LEVEL);
		pWndStatic->SetTitle("");
		pWndStatic = (CWndStatic*)GetDlgItem(WIDC_COUPLE_LEVEL);
		pWndStatic->SetTitle("");
		pWndStatic = (CWndStatic*)GetDlgItem(WIDC_COUPLE_LEVEL);
		pWndStatic->SetTitle("");
	}

	if(m_pVBGauge)
	{
		m_pTheme->RenderGauge( p2DRender, &rect, 0xffffffff, m_pVBGauge, &m_texGauEmptyNormal );
		m_pTheme->RenderGauge( p2DRender, &rectTemp, 0xffffffff, m_pVBGauge, &m_texGauFillNormal );
	}

	SAFE_DELETE_ARRAY( pVertex );
}

//////////////////////////////////////////////////////////////////////////
// Couple Manager Tab Skill Window
//////////////////////////////////////////////////////////////////////////
CWndCoupleTabSkill::CWndCoupleTabSkill()
{
	m_pText = NULL;
	m_pSkillBgTexture = NULL;
}

CWndCoupleTabSkill::~CWndCoupleTabSkill()
{
}

void CWndCoupleTabSkill::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	m_pText = (CWndText *)GetDlgItem( WIDC_TEXT1 );
	
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "CoupleSkillInfo.inc" ) ));
	CHAR* szChar = scanner.m_pProg;

	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString(szChar);
		m_pText->ResetString();	
	}

	m_pSkillBgTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "Bg_Couple_Skill.tga"), 0xffff00ff );

	MoveParentCenter();
} 

BOOL CWndCoupleTabSkill::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_COUPLE_TAB_SKILL, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndCoupleTabSkill::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndCoupleTabSkill::OnDraw(C2DRender* p2DRender)
{
	CPoint point;

	LPWNDCTRL lpWndStatic = GetWndCtrl(WIDC_STATIC1);
	
	if(lpWndStatic)
	{
		point.x = lpWndStatic->rect.left + 4;
		point.y = lpWndStatic->rect.top + 2;
	}

	// Draw Background Image
	if(m_pSkillBgTexture)
		p2DRender->RenderTexture( point, m_pSkillBgTexture );

	// Draw Active SKill
	int nLevel = 0;
	CCouple* pCouple = CCoupleHelper::Instance()->GetCouple();
	if(pCouple)
		nLevel = pCouple->GetLevel();

	VS& vSkills	= CCoupleProperty::Instance()->GetSkill( nLevel );
	VSK& vSkillKinds = CCoupleProperty::Instance()->GetSKillKinds();

	if(vSkills.size() == vSkillKinds.size())
	{
		for(int i=0; i<(int)( vSkills.size() ); i++)
		{
			if(vSkills[i] > 0)
			{
				ItemProp* pItemProp = prj.GetItemProp( vSkillKinds[i] );
				if(pItemProp)
				{
					CTexture* pTex = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, pItemProp->szIcon), 0xffff00ff );

					if(pTex)
						p2DRender->RenderTexture( point, pTex );
				}
			}
			point.x += 32;
		}
	}

	// Make Tooltip
	CPoint ptMouse = GetMousePoint();
	CRect rect = lpWndStatic->rect;

	rect.left += 4;
	rect.top += 2;
	rect.right = rect.left + 32;
	rect.bottom = rect.top + 32;

	for(int i=0; i<(int)( vSkillKinds.size() ); i++)
	{
		if(rect.PtInRect(ptMouse))
		{
			// Tooltip
			CString strSkillName;
			ItemProp* pItemProp = prj.GetItemProp( vSkillKinds[i] );

			if(pItemProp)
				strSkillName.Format("%s", pItemProp->szName);

			ClientToScreen( &ptMouse );
			ClientToScreen( &rect );
			g_toolTip.PutToolTip( 10000, strSkillName, rect, ptMouse, 1 );
		}

		rect.left += 32;
		rect.right += 32;
	}
}

//////////////////////////////////////////////////////////////////////////
// Couple Manager Window
//////////////////////////////////////////////////////////////////////////
CWndCoupleManager::CWndCoupleManager()
{
}

CWndCoupleManager::~CWndCoupleManager()
{
}

void CWndCoupleManager::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	CRect rect = GetClientRect();
	rect.left = 5;
	rect.top = 0;
	
	m_wndCoupleTabInfo.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_COUPLE_TAB_INFO );
	m_wndCoupleTabSkill.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_COUPLE_TAB_SKILL );
	
	WTCITEM tabTabItem;
	
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_GAME_COUPLE_INFO);
	tabTabItem.pWndBase = &m_wndCoupleTabInfo;
	pWndTabCtrl->InsertItem( 0, &tabTabItem );

	tabTabItem.pszText = prj.GetText(TID_GAME_COUPLE_SKILL);
	tabTabItem.pWndBase = &m_wndCoupleTabSkill;
	pWndTabCtrl->InsertItem( 1, &tabTabItem );

	MoveParentCenter();
} 

BOOL CWndCoupleManager::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_COUPLE_MAIN, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndCoupleManager::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

#endif //__CSC_VER13_2

#endif //__PIERCING_REMOVE

#ifdef __FUNNY_COIN

CWndFunnyCoinConfirm::CWndFunnyCoinConfirm() 
{
	m_dwItemId = 0;
	m_pItemElem = NULL;
}
 
CWndFunnyCoinConfirm::~CWndFunnyCoinConfirm() 
{ 
} 

void CWndFunnyCoinConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	if(m_pItemElem != NULL)
	{
		CWndText* pText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
		if(pText)
		{
			CString strText;
			ItemProp* pItemProp = prj.GetItemProp( m_pItemElem->m_dwItemId );
			if(pItemProp)
				strText.Format(prj.GetText( TID_GAME_FUNNYCOIN_ASKUSE ), pItemProp->szName);

			pText->AddString(strText);
		}
	}
	else
		Destroy();

	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndFunnyCoinConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_FUNNYCOIN_CONFIRM, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndFunnyCoinConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES )
	{
		if(static_cast<int>(m_dwItemId) > -1)
			g_DPlay.SendDoUseItem(MAKELONG( ITYPE_ITEM, m_dwItemId ), NULL_ID, -1, FALSE);
	}

	Destroy();

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndFunnyCoinConfirm::SetInfo(DWORD dwItemId, CItemElem* pItemElem)
{
	m_dwItemId = dwItemId;
	m_pItemElem = pItemElem;
}

#endif //__FUNNY_COIN

#if __VER >= 14 // __SMELT_SAFETY
CWndSmeltSafety::CWndSmeltSafety(CWndSmeltSafety::WndMode eWndMode) : 
m_eWndMode(eWndMode), 
m_pItemElem(NULL), 
m_pItemTexture(NULL), 
m_pNowGaugeTexture(NULL), 
m_pSuccessTexture(NULL), 
m_pFailureTexture(NULL), 
m_nMaterialCount(0), 
m_nScroll1Count(0), 
m_nScroll2Count(0), 
m_nResultCount(0), 
m_bStart(FALSE), 
m_bResultSwitch(false), 
m_dwEnchantTimeStart(0xffffffff), 
m_dwEnchantTimeEnd(0xffffffff), 
m_fGaugeRate(0), 
m_nValidSmeltCounter(0), 
m_nCurrentSmeltNumber(0), 
m_pVertexBufferGauge(NULL), 
m_pVertexBufferSuccessImage(NULL), 
m_pVertexBufferFailureImage(NULL)
#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
, 
m_pSelectedElementalCardItemProp( NULL )
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY
{
}

CWndSmeltSafety::~CWndSmeltSafety()
{
	for(int i = 0; i < SMELT_MAX; ++i)
	{
		if(m_Material[i].pItemElem != NULL)
		{
			if( !g_pPlayer->m_vtInfo.IsTrading( m_Material[i].pItemElem ) )
				m_Material[i].pItemElem->SetExtra(0);
		}

		if(m_Scroll1[i].pItemElem != NULL)
		{
			if( !g_pPlayer->m_vtInfo.IsTrading( m_Scroll1[i].pItemElem ) )
				m_Scroll1[i].pItemElem->SetExtra(0);
		}

#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
		if( ( m_eWndMode == WND_NORMAL || m_eWndMode == WND_ELEMENT ) && m_Scroll2[i].pItemElem != NULL )
#else // __15_5TH_ELEMENTAL_SMELT_SAFETY
		if(m_eWndMode == WND_NORMAL && m_Scroll2[i].pItemElem != NULL)
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY
		{
			if( !g_pPlayer->m_vtInfo.IsTrading( m_Scroll2[i].pItemElem ) )
				m_Scroll2[i].pItemElem->SetExtra(0);
		}
	}

	if(m_pItemElem != NULL)
	{
		m_pItemElem->SetExtra(0);
		m_pItemElem = NULL;
	}

	CWndInventory* pWndInventory = (CWndInventory*)GetWndBase( APP_INVENTORY );
	if(pWndInventory != NULL)
	{
		pWndInventory->m_wndItemCtrl.SetDieFlag(FALSE);
	}
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndSmeltSafety::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ )
{
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_SMELT_SAFETY, 0, CPoint( 0, 0 ), pWndParent );
}

void CWndSmeltSafety::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	// 여기에 코딩하세요

	int StaticMaterialID[SMELT_MAX] = {WIDC_STATIC11, WIDC_STATIC12, WIDC_STATIC13, WIDC_STATIC14, WIDC_STATIC15, WIDC_STATIC16,
								WIDC_STATIC17, WIDC_STATIC18, WIDC_STATIC19, WIDC_STATIC20};
	int StaticScrollID1[SMELT_MAX] = {WIDC_STATIC41, WIDC_STATIC42, WIDC_STATIC43, WIDC_STATIC44, WIDC_STATIC45, WIDC_STATIC46,
								WIDC_STATIC47, WIDC_STATIC48, WIDC_STATIC49, WIDC_STATIC50};
	int StaticScrollID2[SMELT_MAX] = {WIDC_STATIC61, WIDC_STATIC62, WIDC_STATIC63, WIDC_STATIC64, WIDC_STATIC65, WIDC_STATIC66,
								WIDC_STATIC67, WIDC_STATIC68, WIDC_STATIC69, WIDC_STATIC70};

	for(int i = 0; i < SMELT_MAX; ++i)
	{
		m_Material[i].wndCtrl = GetWndCtrl( StaticMaterialID[i] );
		m_Material[i].staticNum = StaticMaterialID[i];
		m_Material[i].isUse = FALSE;
		m_Material[i].pItemElem = NULL;
		m_Scroll1[i].wndCtrl = GetWndCtrl( StaticScrollID1[i] );
		m_Scroll1[i].staticNum = StaticScrollID1[i];
		m_Scroll1[i].isUse = FALSE;
		m_Scroll1[i].pItemElem = NULL;
		m_Scroll2[i].wndCtrl = GetWndCtrl( StaticScrollID2[i] );
		m_Scroll2[i].staticNum = StaticScrollID2[i];
		m_Scroll2[i].isUse = FALSE;
		m_Scroll2[i].pItemElem = NULL;
	}

	m_nResultStaticID[0] = WIDC_STATIC31;
	m_nResultStaticID[1] = WIDC_STATIC32;
	m_nResultStaticID[2] = WIDC_STATIC33;
	m_nResultStaticID[3] = WIDC_STATIC34;
	m_nResultStaticID[4] = WIDC_STATIC35;
	m_nResultStaticID[5] = WIDC_STATIC36;
	m_nResultStaticID[6] = WIDC_STATIC37;
	m_nResultStaticID[7] = WIDC_STATIC38;
	m_nResultStaticID[8] = WIDC_STATIC39;
	m_nResultStaticID[9] = WIDC_STATIC40;

	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_TITLE_NOW_GRADE);
	assert(pWndStatic != NULL);
#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
	if( m_eWndMode == WND_NORMAL || m_eWndMode == WND_ACCESSARY || m_eWndMode == WND_ELEMENT )
#else // __15_5TH_ELEMENTAL_SMELT_SAFETY
	if(m_eWndMode == WND_NORMAL || m_eWndMode == WND_ACCESSARY)
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY
	{
		pWndStatic->SetTitle(prj.GetText(TID_GAME_SMELT_SAFETY_NOW_GRADE));
	}
	else if(m_eWndMode == WND_PIERCING)
	{
		pWndStatic->SetTitle(prj.GetText(TID_GAME_SMELT_SAFETY_NOW_PIERCING));
	}

	pWndStatic = (CWndStatic*)GetDlgItem(WIDC_TITLE_MAX_GRADE);
	assert(pWndStatic != NULL);
#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
	if( m_eWndMode == WND_NORMAL || m_eWndMode == WND_ACCESSARY || m_eWndMode == WND_ELEMENT )
#else // __15_5TH_ELEMENTAL_SMELT_SAFETY
	if(m_eWndMode == WND_NORMAL || m_eWndMode == WND_ACCESSARY)
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY
	{
		pWndStatic->SetTitle(prj.GetText(TID_GAME_SMELT_SAFETY_MAX_GRADE));
	}
	else
	{
		pWndStatic->SetTitle(prj.GetText(TID_GAME_SMELT_SAFETY_MAX_PIERCING));
	}

	RefreshInformation();

	switch(m_eWndMode)
	{
		case WND_NORMAL:
			SetTitle(prj.GetText(TID_GAME_SMELTSAFETY_NORMAL));
			break;
		case WND_ACCESSARY:
			SetTitle(prj.GetText(TID_GAME_SMELTSAFETY_ACCESSARY));
			break;
		case WND_PIERCING:
			SetTitle(prj.GetText(TID_GAME_SMELTSAFETY_PIERCING));
			break;
#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
		case WND_ELEMENT:
			SetTitle( prj.GetText( TID_GAME_SMELTSAFETY_ELEMENT ) );
			break;
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY
	}

#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
	if( m_eWndMode == WND_NORMAL || m_eWndMode == WND_ELEMENT )
#else // __15_5TH_ELEMENTAL_SMELT_SAFETY
	if(m_eWndMode == WND_NORMAL)
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY
	{
		CRect rect;
		for(int i = 0; i < SMELT_MAX; ++i)
		{
			CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(m_nResultStaticID[i]);
			assert(pWndStatic != NULL);
			rect = pWndStatic->GetWndRect();
			pWndStatic->Move(rect.left + EXTENSION_PIXEL, rect.top);
		}

		CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
		assert(pWndStatic != NULL);
		rect = pWndStatic->GetWndRect();
		pWndStatic->Move(rect.left + HALF_EXTENSION_PIXEL, rect.top);

		pWndStatic = (CWndStatic*)GetDlgItem(WIDC_TITLE_NOW_GRADE);
		assert(pWndStatic != NULL);
		rect = pWndStatic->GetWndRect();
		pWndStatic->Move(rect.left + HALF_EXTENSION_PIXEL, rect.top);

		pWndStatic = (CWndStatic*)GetDlgItem(WIDC_TITLE_MAX_GRADE);
		assert(pWndStatic != NULL);
		rect = pWndStatic->GetWndRect();
		pWndStatic->Move(rect.left + HALF_EXTENSION_PIXEL, rect.top);

		pWndStatic = (CWndStatic*)GetDlgItem(WIDC_NOW_GRADE);
		assert(pWndStatic != NULL);
		rect = pWndStatic->GetWndRect();
		pWndStatic->Move(rect.left + HALF_EXTENSION_PIXEL, rect.top);

		CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
		assert(pWndEdit != NULL);
		rect = pWndEdit->GetWndRect();
		pWndEdit->Move(rect.left + HALF_EXTENSION_PIXEL, rect.top);

		CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_BUTTON_PLUS);
		assert(pWndButton != NULL);
		rect = pWndButton->GetWndRect();
		pWndButton->Move(rect.left + HALF_EXTENSION_PIXEL, rect.top);

		pWndButton = (CWndButton*)GetDlgItem(WIDC_BUTTON_MINUS);
		assert(pWndButton != NULL);
		rect = pWndButton->GetWndRect();
		pWndButton->Move(rect.left + HALF_EXTENSION_PIXEL, rect.top);

		pWndButton = (CWndButton*)GetDlgItem(WIDC_BUTTON_START);
		assert(pWndButton != NULL);
		rect = pWndButton->GetWndRect();
		pWndButton->Move(rect.left + HALF_EXTENSION_PIXEL, rect.top);

		pWndButton = (CWndButton*)GetDlgItem(WIDC_BUTTON_STOP);
		assert(pWndButton != NULL);
		rect = pWndButton->GetWndRect();
		pWndButton->Move(rect.left + HALF_EXTENSION_PIXEL, rect.top);

		pWndButton = (CWndButton*)GetDlgItem(WIDC_BUTTON_RESET);
		assert(pWndButton != NULL);
		rect = pWndButton->GetWndRect();
		pWndButton->Move(rect.left + HALF_EXTENSION_PIXEL, rect.top);

		CRect wndrect = GetWndRect();
		wndrect.right = wndrect.right + EXTENSION_PIXEL;
		SetWndRect(wndrect);
	}
	else
	{
		for(int i = 0; i < SMELT_MAX; ++i)
		{
			CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(m_Scroll2[i].staticNum);
			assert(pWndStatic != NULL);
			pWndStatic->SetVisible(FALSE);
			pWndStatic->EnableWindow(FALSE);
		}
	}

	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
	assert(pWndEdit != NULL);
	pWndEdit->AddWndStyle(EBS_NUMBER);
	pWndEdit->EnableWindow(FALSE);

	CWndInventory* pWndInventory = (CWndInventory*)g_WndMng.CreateApplet(APP_INVENTORY);
	assert(pWndInventory != NULL);
	pWndInventory->m_wndItemCtrl.SetDieFlag(TRUE);

	m_pNowGaugeTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "SafetyGauge.bmp"), 0xffff00ff);
	m_pSuccessTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "SafetySuccess.bmp"), 0xffff00ff);
	m_pFailureTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "SafetyFailure.bmp"), 0xffff00ff);

	MoveParentCenter();
}

BOOL CWndSmeltSafety::Process()
{
	if( CMover::GetActiveMover()->m_vtInfo.GetOther() || CMover::GetActiveMover()->m_vtInfo.VendorIsVendor() )
	{
		Destroy();
	}

	if(m_bStart != FALSE && m_pItemElem != NULL)
	{
		if(m_bResultSwitch != false)
		{
			m_fGaugeRate = static_cast<float>(g_tmCurrent - m_dwEnchantTimeStart) / static_cast<float>(m_dwEnchantTimeEnd - m_dwEnchantTimeStart);
		}
		else
		{
			m_fGaugeRate = 0.0f;
		}

		if(m_nValidSmeltCounter == 0 || GetNowSmeltValue() == GetDefaultMaxSmeltValue())
		{
			StopSmelting();
		}

		if(m_eWndMode == WND_PIERCING)
		{
			CWndInventory* pWndInventory = (CWndInventory*)GetWndBase(APP_INVENTORY);
			assert(pWndInventory != NULL);
			CWndStatic* pGoldNumberStatic = (CWndStatic*)pWndInventory->GetDlgItem(WIDC_GOLD_NUM);
			assert(pGoldNumberStatic != NULL);
			int nGoldNumber(atoi(pGoldNumberStatic->GetTitle()));
			if(nGoldNumber < 100000)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_LACKMONEY), NULL, prj.GetTextColor(TID_GAME_LACKMONEY));
				StopSmelting();
			}
		}
		if(m_dwEnchantTimeEnd < g_tmCurrent)
		{
			m_bResultSwitch = false;
			m_dwEnchantTimeStart = 0xffffffff;
			m_dwEnchantTimeEnd = 0xffffffff;

			// Send to Server...
			GENMATDIEINFO* pTargetMaterial = &m_Material[m_nCurrentSmeltNumber];
			GENMATDIEINFO* pTargetScroll1 = &m_Scroll1[m_nCurrentSmeltNumber];
			GENMATDIEINFO* pTargetScroll2 = &m_Scroll2[m_nCurrentSmeltNumber];
			if(pTargetMaterial->isUse != FALSE && m_Scroll1[m_nCurrentSmeltNumber].isUse != FALSE)
			{
				g_DPlay.SendSmeltSafety(
					m_pItemElem->m_dwObjId, 
					pTargetMaterial->pItemElem->m_dwObjId, 
					pTargetScroll1->pItemElem->m_dwObjId, 
					pTargetScroll2->isUse != FALSE ? pTargetScroll2->pItemElem->m_dwObjId : NULL_ID);
				
				pTargetMaterial->isUse = FALSE;
				if(pTargetMaterial->pItemElem != NULL)
				{
					pTargetMaterial->pItemElem->SetExtra(pTargetMaterial->pItemElem->GetExtra() - 1);
					pTargetMaterial->pItemElem = NULL;
				}
				pTargetScroll1->isUse = FALSE;
				if(pTargetScroll1->pItemElem != NULL)
				{
					pTargetScroll1->pItemElem->SetExtra(pTargetScroll1->pItemElem->GetExtra() - 1);
					pTargetScroll1->pItemElem = NULL;
				}
				if(pTargetScroll2->isUse != FALSE)
				{
					pTargetScroll2->pItemElem->SetExtra(pTargetScroll2->pItemElem->GetExtra() - 1);
					pTargetScroll2->isUse = FALSE;
					if(pTargetScroll2->pItemElem != NULL)
					{
						pTargetScroll2->pItemElem = NULL;
					}
				}
			}
			m_dwEnchantTimeStart = g_tmCurrent;
			m_dwEnchantTimeEnd = g_tmCurrent + SEC(ENCHANT_TIME);
		}
	}
	else
	{
		m_dwEnchantTimeStart = 0xffffffff;
		m_dwEnchantTimeEnd = 0xffffffff;
	}

	return TRUE;
}

void CWndSmeltSafety::OnDraw(C2DRender* p2DRender)
{
	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
	assert(pWndStatic != NULL);
	CRect rectSmeltItem = pWndStatic->GetWndRect();

	if(m_pItemElem != NULL && m_pItemTexture != NULL)
	{
		// Draw Item
		m_pItemTexture->Render( p2DRender, CPoint( rectSmeltItem.left, rectSmeltItem.top ) );

		// Draw Enchant Effect
		if( m_dwEnchantTimeStart != 0xffffffff && m_dwEnchantTimeEnd != 0xffffffff )
		{
			RenderEnchant( p2DRender, CPoint(rectSmeltItem.left, rectSmeltItem.top) );
		}
	}

	// Draw Tooltip
	CPoint pointMouse = GetMousePoint();
	if(rectSmeltItem.PtInRect(pointMouse) != FALSE)
	{
		if(m_pItemElem != NULL)
		{
			ClientToScreen( &pointMouse );
			ClientToScreen( &rectSmeltItem );
			g_WndMng.PutToolTip_Item((CItemBase*)m_pItemElem, pointMouse, &rectSmeltItem);
		}
		else
		{
			ClientToScreen( &pointMouse );
			ClientToScreen( &rectSmeltItem );
			CString strEmptyTooltip;
			strEmptyTooltip.Format("%s", prj.GetText(TID_GAME_TOOLTIP_SMELT_SAFETY_ITEM));
			g_toolTip.PutToolTip(reinterpret_cast<DWORD>(this), strEmptyTooltip, rectSmeltItem, pointMouse);
		}
	}

	DrawListItem(p2DRender);

	for(int i = 0; i < m_nCurrentSmeltNumber; ++i)
	{
#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
		const int nExtensionPixel( ( m_eWndMode == WND_NORMAL || m_eWndMode == WND_ELEMENT ) ? EXTENSION_PIXEL : 0 );
#else // __15_5TH_ELEMENTAL_SMELT_SAFETY
		const int nExtensionPixel(m_eWndMode == WND_NORMAL ? EXTENSION_PIXEL : 0);
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY
		static CRect rectStaticTemp;
		LPWNDCTRL lpStatic = GetWndCtrl(m_nResultStaticID[i]);
		rectStaticTemp.TopLeft().y = lpStatic->rect.top;
		rectStaticTemp.TopLeft().x = lpStatic->rect.left + nExtensionPixel;
		rectStaticTemp.BottomRight().y = lpStatic->rect.bottom;
		rectStaticTemp.BottomRight().x = lpStatic->rect.right + nExtensionPixel;
		if(m_bResultStatic[i] != false)
		{
			m_pTheme->RenderGauge(p2DRender, &rectStaticTemp, 0xffffffff, m_pVertexBufferSuccessImage, m_pSuccessTexture);
		}
		else
		{
			m_pTheme->RenderGauge(p2DRender, &rectStaticTemp, 0xffffffff, m_pVertexBufferSuccessImage, m_pFailureTexture);
		}
	}

	if(m_bStart != NULL && m_bResultSwitch != false)
	{
#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
		const int nExtensionPixel( ( m_eWndMode == WND_NORMAL || m_eWndMode == WND_ELEMENT ) ? EXTENSION_PIXEL : 0 );
#else // __15_5TH_ELEMENTAL_SMELT_SAFETY
		const int nExtensionPixel(m_eWndMode == WND_NORMAL ? EXTENSION_PIXEL : 0);
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY
		static CRect rectStaticTemp;
		LPWNDCTRL lpStatic = GetWndCtrl(m_nResultStaticID[m_nCurrentSmeltNumber]);
		rectStaticTemp.TopLeft().y = lpStatic->rect.top;
		rectStaticTemp.TopLeft().x = lpStatic->rect.left + nExtensionPixel;
		rectStaticTemp.BottomRight().y = lpStatic->rect.bottom;
		int nGaugeWidth(lpStatic->rect.left + static_cast<int>(static_cast<float>(lpStatic->rect.right - lpStatic->rect.left) * m_fGaugeRate));
		nGaugeWidth = nGaugeWidth < lpStatic->rect.right ? nGaugeWidth : lpStatic->rect.right;
		rectStaticTemp.BottomRight().x = nGaugeWidth + nExtensionPixel;
		assert(m_pVertexBufferGauge != NULL);
		m_pTheme->RenderGauge(p2DRender, &rectStaticTemp, 0xffffffff, m_pVertexBufferGauge, m_pNowGaugeTexture);
	}
}

BOOL CWndSmeltSafety::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch(nID)
	{
	case WIDC_BUTTON_START:
		{
			if(m_bStart != FALSE)
				break;

			if(m_pItemElem == NULL)
			{
				// 먼저 안전 제련을 시도할 아이템을 등록해야 합니다.
				g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR07), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR07));
				break;
			}

			if(m_nValidSmeltCounter > 0)
			{
				if(m_eWndMode == WND_PIERCING)
				{
					CWndInventory* pWndInventory = (CWndInventory*)GetWndBase(APP_INVENTORY);
					assert(pWndInventory != NULL);
					CWndStatic* pGoldNumberStatic = (CWndStatic*)pWndInventory->GetDlgItem(WIDC_GOLD_NUM);
					assert(pGoldNumberStatic != NULL);
					int nGoldNumber(atoi(pGoldNumberStatic->GetTitle()));
					if(nGoldNumber < 100000)
					{
						g_WndMng.PutString(prj.GetText(TID_GAME_LACKMONEY), NULL, prj.GetTextColor(TID_GAME_LACKMONEY));
						break;
					}
				}
				m_bStart = TRUE;
				m_bResultSwitch = true;
				m_dwEnchantTimeStart = g_tmCurrent;
				m_dwEnchantTimeEnd = g_tmCurrent + SEC(ENCHANT_TIME);
				CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
				assert(pWndEdit != NULL);
				pWndEdit->EnableWindow(FALSE);
			}
			else
			{
				CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
				assert(pWndEdit != NULL);

				if(GetNowSmeltValue() == GetDefaultMaxSmeltValue())
				{
					// 이미 최대치까지 제련되어 더 이상 제련할 수 없습니다.
					g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR13), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR13));
				}
				else if(m_nCurrentSmeltNumber >= SMELT_MAX)
				{
					// 제련 공간이 부족합니다. 계속 제련하기를 원하시면 Reset 버튼을 눌러 초기화해야 합니다.
					g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR15), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR15));
				}
				else if(m_nMaterialCount <= m_nCurrentSmeltNumber || m_nScroll1Count <= m_nCurrentSmeltNumber)
				{
					switch(m_eWndMode)
					{
					case WND_NORMAL:
						{
							if(m_pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE)
							{
								// 오리칼쿰과 일반 보호의 두루마리가 하나라도 등록된 상태가 아니면 수행할 수 없습니다.
								g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR08), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR08));
							}
							else
							{
								// 빛나는 오리칼쿰과 최상급 보호의 두루마리가 하나라도 등록된 상태가 아니면 수행할 수 없습니다.
								g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR09), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR09));
							}
							break;
						}
					case WND_ACCESSARY:
						{
							// 문스톤과 액세서리 보호의 두루마리가 하나라도 등록된 상태가 아니면 수행할 수 없습니다.
							g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR10), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR10));
							break;
						}
					case WND_PIERCING:
						{
							// 문스톤과 피어싱 보호의 두루마리가 하나라도 등록된 상태가 아니면 수행할 수 없습니다.
							g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR11), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR11));
							break;
						}
#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
					case WND_ELEMENT:
						{
							// 속성 카드와 일반 보호의 두루마리가 하나라도 등록된 상태가 아니면 수행할 수 없습니다.
							g_WndMng.PutString( prj.GetText( TID_GAME_SMELT_SAFETY_ERROR20 ), NULL, prj.GetTextColor( TID_GAME_SMELT_SAFETY_ERROR20 ) );
							break;
						}
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY
					}
				}
				else if(GetNowSmeltValue() >= atoi(pWndEdit->GetString()))
				{
					// 제련을 수행할 수 없는 조건입니다.
					g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR14), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR14));
				}
			}
			break;
		}
	case WIDC_BUTTON_STOP:
		{
			if(m_bStart != FALSE)
			{
				StopSmelting();
			}
			break;
		}
	case WIDC_BUTTON_RESET:
		{
			if(m_bStart == FALSE)
			{
				ResetData();
				RefreshInformation();
			}
			break;
		}
	case WIDC_EDIT_MAX_GRADE:
		{
			if(m_bStart == FALSE && m_pItemElem != NULL)
			{
				CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
				assert(pWndEdit != NULL);
				int nMaxSmeltNumber(atoi(pWndEdit->GetString()));
				nMaxSmeltNumber = min(nMaxSmeltNumber, GetDefaultMaxSmeltValue());
				CString strMaxSmeltNumber;
				strMaxSmeltNumber.Format("%d", nMaxSmeltNumber);
				pWndEdit->SetString(strMaxSmeltNumber);
				RefreshValidSmeltCounter();
			}
			break;
		}
	case WIDC_BUTTON_PLUS:
		{
			CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
			assert(pWndEdit != NULL);
			if(pWndEdit->IsWindowEnabled() != FALSE && m_bStart == FALSE && m_pItemElem != NULL)
			{
				int nMaxSmeltNumber(atoi(pWndEdit->GetString()));
				nMaxSmeltNumber = (++nMaxSmeltNumber < GetDefaultMaxSmeltValue()) ? nMaxSmeltNumber : GetDefaultMaxSmeltValue();
				CString strMaxSmeltNumber;
				strMaxSmeltNumber.Format("%d", nMaxSmeltNumber);
				pWndEdit->SetString(strMaxSmeltNumber);
				RefreshValidSmeltCounter();
			}
			break;
		}
	case WIDC_BUTTON_MINUS:
		{
			CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
			assert(pWndEdit != NULL);
			if(pWndEdit->IsWindowEnabled() != FALSE && m_bStart == FALSE && m_pItemElem != NULL)
			{
				int nMaxSmeltNumber(atoi(pWndEdit->GetString()));
				nMaxSmeltNumber = (--nMaxSmeltNumber > 0) ? nMaxSmeltNumber : 0;
				CString strMaxSmeltNumber;
				strMaxSmeltNumber.Format("%d", nMaxSmeltNumber);
				pWndEdit->SetString(strMaxSmeltNumber);
				RefreshValidSmeltCounter();
			}
			break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

BOOL CWndSmeltSafety::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CWndBase* pWndFrame = pShortcut->m_pFromWnd->GetFrameWnd();
	if( pWndFrame == NULL )
		return FALSE;

	if( pWndFrame->GetWndId() != APP_INVENTORY )
	{
		SetForbid( TRUE );
		return FALSE;
	}

	CRect WndRect = GetClientRect();
	if(WndRect.PtInRect(point))
	{
		CItemElem* pTempElem = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
		if(pTempElem == NULL)
			return TRUE;

		ItemProp* pItemProp = pTempElem->GetProp();
		if(pItemProp == NULL)
			return TRUE;

		SetItem(pTempElem);
	}

	return TRUE;
}

void CWndSmeltSafety::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	if(m_bStart != FALSE)
		return;

	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
	if(pWndStatic == NULL)
		return;

	CRect rect = pWndStatic->GetWndRect();
	if(rect.PtInRect(point))
	{
		// 제련 아이템을 더블클릭하면 Reset 버튼으로 포커스를 강제로 옮긴다
		CWndBase* pWndResetButtonBase = GetDlgItem(WIDC_BUTTON_RESET);
		assert(pWndResetButtonBase != NULL);
		pWndResetButtonBase->SetFocus();

		if(m_pItemElem != NULL)
		{
			m_pItemElem->SetExtra(0);
			m_pItemElem = NULL;
		}
		m_pItemTexture = NULL;

		CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
		assert(pWndEdit != NULL);
		pWndEdit->SetString(TEXT(""));
		pWndEdit->EnableWindow(FALSE);

		ResetData();
		RefreshInformation();
	}
	else
	{
		if(IsDropMaterialZone(point) != FALSE && m_nMaterialCount > m_nCurrentSmeltNumber)
		{
			if(g_WndMng.m_pWndWorld->m_bShiftPushed != FALSE)
			{
				while(m_nMaterialCount > m_nCurrentSmeltNumber)
				{
					SubtractListItem(&m_Material[m_nMaterialCount - 1]);
					--m_nMaterialCount;

					while(m_nScroll1Count > m_nMaterialCount)
					{
						SubtractListItem(&m_Scroll1[m_nScroll1Count - 1]);
						--m_nScroll1Count;
					}

#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
					if( m_eWndMode == WND_NORMAL || m_eWndMode == WND_ELEMENT )
#else // __15_5TH_ELEMENTAL_SMELT_SAFETY
					if(m_eWndMode == WND_NORMAL)
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY
					{
						while(m_nScroll2Count > m_nMaterialCount)
						{
							SubtractListItem(&m_Scroll2[m_nScroll2Count - 1]);
							--m_nScroll2Count;
						}
					}
				}
			}
			else
			{
				SubtractListItem(&m_Material[m_nMaterialCount - 1]);
				--m_nMaterialCount;

				if(m_nScroll1Count > m_nMaterialCount)
				{
					SubtractListItem(&m_Scroll1[m_nScroll1Count - 1]);
					--m_nScroll1Count;
				}

#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
				if( ( m_eWndMode == WND_NORMAL || m_eWndMode == WND_ELEMENT ) && m_nScroll2Count > m_nMaterialCount )
#else // __15_5TH_ELEMENTAL_SMELT_SAFETY
				if(m_eWndMode == WND_NORMAL && m_nScroll2Count > m_nMaterialCount)
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY
				{
					SubtractListItem(&m_Scroll2[m_nScroll2Count - 1]);
					--m_nScroll2Count;
				}
			}
			RefreshValidSmeltCounter();
#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
			if( m_nMaterialCount == m_nCurrentSmeltNumber )
				m_pSelectedElementalCardItemProp = NULL;
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY
		}
		else if(IsDropScroll1Zone(point) != FALSE && m_nScroll1Count > m_nCurrentSmeltNumber)
		{
			if(g_WndMng.m_pWndWorld->m_bShiftPushed != FALSE)
			{
				while(m_nScroll1Count > m_nCurrentSmeltNumber)
				{
					SubtractListItem(&m_Scroll1[m_nScroll1Count - 1]);
					--m_nScroll1Count;
				}
			}
			else
			{
				SubtractListItem(&m_Scroll1[m_nScroll1Count - 1]);
				--m_nScroll1Count;
			}
			RefreshValidSmeltCounter();
		}
#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
		else if( ( m_eWndMode == WND_NORMAL || m_eWndMode == WND_ELEMENT ) && IsDropScroll2Zone(point) != FALSE && m_nScroll2Count > m_nCurrentSmeltNumber )
#else // __15_5TH_ELEMENTAL_SMELT_SAFETY
		else if(m_eWndMode == WND_NORMAL && IsDropScroll2Zone(point) != FALSE && m_nScroll2Count > m_nCurrentSmeltNumber)
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY
		{
			if(g_WndMng.m_pWndWorld->m_bShiftPushed != FALSE)
			{
				while(m_nScroll2Count > m_nCurrentSmeltNumber)
				{
					SubtractListItem(&m_Scroll2[m_nScroll2Count - 1]);
					--m_nScroll2Count;
				}
			}
			else
			{
				SubtractListItem(&m_Scroll2[m_nScroll2Count - 1]);
				--m_nScroll2Count;
			}
			RefreshValidSmeltCounter();
		}
	}
}

HRESULT CWndSmeltSafety::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if( m_pVertexBufferGauge == NULL )
		m_pApp->m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, 
												 D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 
												 D3DFVF_TEXTUREVERTEX2, 
												 D3DPOOL_DEFAULT, 
												 &m_pVertexBufferGauge, 
												 NULL);
	assert(m_pVertexBufferGauge != NULL);

	if( m_pVertexBufferSuccessImage == NULL )
		m_pApp->m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, 
												 D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 
												 D3DFVF_TEXTUREVERTEX2, 
												 D3DPOOL_DEFAULT, 
												 &m_pVertexBufferSuccessImage, 
												 NULL);
	assert(m_pVertexBufferSuccessImage != NULL);

	if( m_pVertexBufferFailureImage == NULL )
		m_pApp->m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, 
												 D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 
												 D3DFVF_TEXTUREVERTEX2, 
												 D3DPOOL_DEFAULT, 
												 &m_pVertexBufferFailureImage, 
												 NULL);
	assert(m_pVertexBufferFailureImage != NULL);
	return S_OK;
}
HRESULT CWndSmeltSafety::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
    SAFE_RELEASE( m_pVertexBufferGauge );
	SAFE_RELEASE( m_pVertexBufferSuccessImage );
	SAFE_RELEASE( m_pVertexBufferFailureImage );
	return S_OK;
}
HRESULT CWndSmeltSafety::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	SAFE_RELEASE( m_pVertexBufferGauge );
	SAFE_RELEASE( m_pVertexBufferSuccessImage );
	SAFE_RELEASE( m_pVertexBufferFailureImage );
	return S_OK;
}

void CWndSmeltSafety::SetItem(CItemElem* pItemElem)
{
	assert(pItemElem != NULL);

	if(m_bStart != FALSE)
	{
		// 안전 제련 도중에는 아이템을 등록할 수 없습니다.
		g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR12), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR12));
		return;
	}

	ItemProp* pItemProp = pItemElem->GetProp();
	if(pItemProp == NULL)
		return;

	if(m_pItemElem == NULL) // 제련 아이템이 올려져 있지 않으면
	{
		BOOL bAcceptableItem = FALSE;
		switch(m_eWndMode)
		{
		case WND_NORMAL:
			{
				if(CItemElem::IsDiceRefineryAble(pItemProp))
				{
					bAcceptableItem = TRUE;
				}
				else
				{
					// 제련할 수 없는 아이템입니다.
					g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR01), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR01));
				}
				break;
			}
		case WND_ACCESSARY:
			{
				if(pItemElem->IsCollector(TRUE) || pItemProp->dwItemKind2 == IK2_JEWELRY)
				{
					bAcceptableItem = TRUE;
				}
				else
				{
					// 제련할 수 없는 아이템입니다.
					g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR01), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR01));
				}
				break;
			}
		case WND_PIERCING:
			{
				if(pItemElem->IsPierceAble())
				{
					bAcceptableItem = TRUE;
				}
				else
				{
					// 피어싱 할 수 없는 아이템입니다.
					g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR02), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR02));
				}
				break;
			}
#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
		case WND_ELEMENT:
			{
				if( CItemElem::IsEleRefineryAble( pItemProp ) )
					bAcceptableItem = TRUE;
				else
				{
					// 제련할 수 없는 아이템입니다.
					g_WndMng.PutString( prj.GetText( TID_GAME_SMELT_SAFETY_ERROR01 ), NULL, prj.GetTextColor( TID_GAME_SMELT_SAFETY_ERROR01 ) );
				}
				break;
			}
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY
		}

		if(bAcceptableItem && m_pItemElem == NULL && pItemElem != NULL)
		{
			m_pItemElem = pItemElem;
			m_pItemElem->SetExtra(m_pItemElem->GetExtra() + 1);

			assert(pItemProp != NULL);
			m_pItemTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );

			CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
			assert(pWndEdit != NULL);
			CString strDefaultSmeltMaxValue;
			strDefaultSmeltMaxValue.Format("%d", GetDefaultMaxSmeltValue());
			pWndEdit->EnableWindow(TRUE);
			pWndEdit->SetString(strDefaultSmeltMaxValue);

			RefreshInformation();
		}
	}
	else
	{
		if(IsAcceptableMaterial(pItemProp) != FALSE)
		{
			if(m_nMaterialCount < SMELT_MAX)
			{
				if(g_WndMng.m_pWndWorld->m_bShiftPushed != FALSE)
				{
					while(m_nMaterialCount < SMELT_MAX && pItemElem->GetExtra() < pItemElem->m_nItemNum)
					{
						AddListItem(&m_Material[m_nMaterialCount], pItemElem);
						++m_nMaterialCount;
					}
				}
				else
				{
					if(pItemElem->GetExtra() < pItemElem->m_nItemNum)
					{
						AddListItem(&m_Material[m_nMaterialCount], pItemElem);
						++m_nMaterialCount;
					}
				}
			}
		}
		else if(IsAcceptableScroll1(pItemProp) != FALSE)
		{
			if(m_nScroll1Count < m_nMaterialCount)
			{
				if(g_WndMng.m_pWndWorld->m_bShiftPushed != FALSE)
				{
					while(m_nScroll1Count < m_nMaterialCount && pItemElem->GetExtra() < pItemElem->m_nItemNum)
					{
						AddListItem(&m_Scroll1[m_nScroll1Count], pItemElem);
						++m_nScroll1Count;
					}
				}
				else
				{
					if(pItemElem->GetExtra() < pItemElem->m_nItemNum)
					{
						AddListItem(&m_Scroll1[m_nScroll1Count], pItemElem);
						++m_nScroll1Count;
					}
				}
			}
			else if(m_nScroll1Count != SMELT_MAX)
			{
				switch(m_eWndMode)
				{
				case WND_NORMAL:
					{
						if(m_pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE)
						{
							// 먼저 오리칼쿰을 등록해야 합니다.
							g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR03), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR03));
						}
						else
						{
							// 먼저 빛나는 오리칼쿰을 등록해야 합니다.
							g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR04), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR04));
						}
						break;
					}
				case WND_ACCESSARY:
				case WND_PIERCING:
					{
						// 먼저 문스톤을 등록해야 합니다.
						g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR05), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR05));
						break;
					}
#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
				case WND_ELEMENT:
					{
						// 먼저 속성 카드를 등록해야 합니다.
						g_WndMng.PutString( prj.GetText( TID_GAME_SMELT_SAFETY_ERROR17 ), NULL, prj.GetTextColor( TID_GAME_SMELT_SAFETY_ERROR17 ) );
						break;
					}
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY
				}
			}
		}
#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
		else if( ( m_eWndMode == WND_NORMAL || m_eWndMode == WND_ELEMENT ) && IsAcceptableScroll2( pItemProp ) != FALSE )
#else // __15_5TH_ELEMENTAL_SMELT_SAFETY
		else if(m_eWndMode == WND_NORMAL && IsAcceptableScroll2(pItemProp) != FALSE)
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY
		{
			if(m_nScroll2Count < m_nMaterialCount)
			{
				if(g_WndMng.m_pWndWorld->m_bShiftPushed != FALSE)
				{
					m_nScroll2Count = (m_nCurrentSmeltNumber > m_nScroll2Count) ? m_nCurrentSmeltNumber : m_nScroll2Count;
					while(m_nScroll2Count < m_nMaterialCount && pItemElem->GetExtra() < pItemElem->m_nItemNum)
					{
						AddListItem(&m_Scroll2[m_nScroll2Count], pItemElem);
						++m_nScroll2Count;
					}
				}
				else
				{
					m_nScroll2Count = (m_nCurrentSmeltNumber > m_nScroll2Count) ? m_nCurrentSmeltNumber : m_nScroll2Count;
					if(pItemElem->GetExtra() < pItemElem->m_nItemNum)
					{
						AddListItem(&m_Scroll2[m_nScroll2Count], pItemElem);
						++m_nScroll2Count;
					}
				}
			}
			else if(m_nScroll2Count != SMELT_MAX)
			{
				switch(m_eWndMode)
				{
				case WND_NORMAL:
					{
						if(m_pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE)
						{
							// 먼저 오리칼쿰을 등록해야 합니다.
							g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR03), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR03));
						}
						else
						{
							// 먼저 빛나는 오리칼쿰을 등록해야 합니다.
							g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR04), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR04));
						}
						break;
					}
				case WND_ACCESSARY:
				case WND_PIERCING:
					{
						// 먼저 문스톤을 등록해야 합니다.
						g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR05), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR05));
						break;
					}
#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
				case WND_ELEMENT:
					{
						// 먼저 속성 카드를 등록해야 합니다.
						g_WndMng.PutString( prj.GetText( TID_GAME_SMELT_SAFETY_ERROR17 ), NULL, prj.GetTextColor( TID_GAME_SMELT_SAFETY_ERROR17 ) );
						break;
					}
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY
				}
			}
		}
		else
		{
#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
			if( m_eWndMode == WND_ELEMENT && CItemElem::IsElementalCard( pItemProp->dwID ) == TRUE )
			{
				if( m_pItemElem->GetItemResist() != SAI79::NO_PROP )
				{
					// 이미 아이템에 속성 제련이 되어 있습니다. 한 아이템에 두 속성이 들어갈 수 없습니다.
					g_WndMng.PutString( prj.GetText( TID_GAME_SMELT_SAFETY_ERROR18 ), NULL, prj.GetTextColor( TID_GAME_SMELT_SAFETY_ERROR18 ) );
				}
				else if( pItemProp != m_pSelectedElementalCardItemProp )
				{
					// 이미 다른 종류의 속성 카드가 등록되어 있습니다.
					g_WndMng.PutString( prj.GetText( TID_GAME_SMELT_SAFETY_ERROR19 ), NULL, prj.GetTextColor( TID_GAME_SMELT_SAFETY_ERROR19 ) );
				}
			}
			else
			{
				// 제련 아이템에 맞는 재료나 두루마리가 아닙니다.
				g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR06), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR06));
			}
#else // __15_5TH_ELEMENTAL_SMELT_SAFETY
			// 제련 아이템에 맞는 재료나 두루마리가 아닙니다.
			g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR06), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR06));
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY
		}
		RefreshValidSmeltCounter();
	}
}

void CWndSmeltSafety::RefreshInformation(void)
{
	RefreshText();
	RefreshValidSmeltCounter();
}

void CWndSmeltSafety::RefreshText(void)
{
	CString strItemAbility;
	if(m_pItemElem != NULL)
	{
		strItemAbility.Format("%d", GetNowSmeltValue());
	}
	else
	{
		strItemAbility.Format(TEXT("--"));
	}
	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_NOW_GRADE);
	assert(pWndStatic != NULL);
	pWndStatic->SetTitle(strItemAbility);
}

void CWndSmeltSafety::RefreshValidSmeltCounter(void)
{
	// 유효한 제련 카운터를 (다시) 계산하여 갱신하는 코드
	m_nValidSmeltCounter = m_nMaterialCount;
	m_nValidSmeltCounter = (m_nScroll1Count < m_nValidSmeltCounter) ? m_nScroll1Count : m_nValidSmeltCounter;
	m_nValidSmeltCounter -= m_nCurrentSmeltNumber;
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
	assert(pWndEdit != NULL);
	int nMaxSmeltNumber(atoi(pWndEdit->GetString()));
	int nNowSmeltNumber(GetNowSmeltValue());
	int nSmeltNumber(nMaxSmeltNumber - nNowSmeltNumber);
	nSmeltNumber = (nSmeltNumber > 0) ? nSmeltNumber : 0;
	m_nValidSmeltCounter = (nSmeltNumber < m_nValidSmeltCounter) ? nSmeltNumber : m_nValidSmeltCounter;
}

void CWndSmeltSafety::StopSmelting(void)
{
	m_bStart = FALSE;
	m_bResultSwitch = false;
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
	assert(pWndEdit != NULL);
	pWndEdit->EnableWindow(TRUE);
}

void CWndSmeltSafety::DisableScroll2(void)
{
	assert(m_pItemElem != NULL);
#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
	if( ( m_eWndMode == WND_NORMAL && ( m_pItemElem->GetAbilityOption() >= GENERAL_NON_USING_SCROLL2_LEVEL || m_pItemElem->GetProp()->dwReferStat1 == WEAPON_ULTIMATE ) ) || 
		( m_eWndMode == WND_ELEMENT && m_pItemElem->GetResistAbilityOption() >= ELEMENTAL_NON_USING_SCROLL2_LEVEL ) )
#else // __15_5TH_ELEMENTAL_SMELT_SAFETY
	if(!(m_pItemElem->GetAbilityOption() < 7 && m_pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE))
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY
	{
		m_nScroll2Count = 0;
		for(int i = 0; i < SMELT_MAX; ++i)
		{
			GENMATDIEINFO* pTargetScroll2 = &m_Scroll2[i];
			if(pTargetScroll2->pItemElem != NULL)
			{
				pTargetScroll2->isUse = FALSE;
				pTargetScroll2->pItemElem->SetExtra(pTargetScroll2->pItemElem->GetExtra() - 1);
				pTargetScroll2->pItemElem = NULL;
			}
		}
	}
}

void CWndSmeltSafety::ResetData(void)
{
	m_nMaterialCount = 0;
	m_nScroll1Count = 0;
	m_nScroll2Count = 0;
	m_nCurrentSmeltNumber = 0;
	for(int i = 0; i < SMELT_MAX; ++i)
	{
		m_Material[i].isUse = FALSE;
		if(m_Material[i].pItemElem != NULL)
		{
			m_Material[i].pItemElem->SetExtra(0);
			m_Material[i].pItemElem = NULL;
		}
		m_Scroll1[i].isUse = FALSE;
		if(m_Scroll1[i].pItemElem != NULL)
		{
			m_Scroll1[i].pItemElem->SetExtra(0);
			m_Scroll1[i].pItemElem = NULL;
		}
		m_Scroll2[i].isUse = FALSE;
		if(m_Scroll2[i].pItemElem != NULL)
		{
			m_Scroll2[i].pItemElem->SetExtra(0);
			m_Scroll2[i].pItemElem = NULL;
		}

		CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(m_nResultStaticID[i]);
		assert(pWndStatic != NULL);
		pWndStatic->SetTitle("");
	}
#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
	m_pSelectedElementalCardItemProp = NULL;
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY
}

void CWndSmeltSafety::AddListItem(GENMATDIEINFO* pListItem, CItemElem* pItemElem)
{
	assert(pListItem->isUse == FALSE && pListItem->pItemElem == NULL);
	pListItem->isUse = TRUE;
	pListItem->pItemElem = pItemElem;
	pListItem->pItemElem->SetExtra(pItemElem->GetExtra() + 1);
}
void CWndSmeltSafety::SubtractListItem(GENMATDIEINFO* pListItem)
{
	assert(pListItem->isUse != FALSE && pListItem->pItemElem != NULL);
	pListItem->isUse = FALSE;
	pListItem->pItemElem->SetExtra(pListItem->pItemElem->GetExtra() - 1);
	pListItem->pItemElem = NULL;
}

void CWndSmeltSafety::DrawListItem(C2DRender* p2DRender)
{
	if(m_eWndMode == WND_NORMAL && (m_pItemElem == NULL || m_pItemTexture == NULL))
		return;

	static const int NORMAL_ALPHA(255);
	static const int TRANSLUCENT_ALPHA(75);
	static int nAlphaBlend(NORMAL_ALPHA);
	ItemProp* pItemProp = NULL;
	CTexture* pTexture = NULL;

	for(int i = m_nCurrentSmeltNumber; i < SMELT_MAX; ++i)
	{
		assert(m_Material[i].wndCtrl != NULL);
		switch(m_eWndMode)
		{
		case WND_NORMAL:
			{
				pItemProp = (m_pItemElem->GetProp()->dwReferStat1 == WEAPON_ULTIMATE) ? prj.GetItemProp(II_GEN_MAT_ORICHALCUM02) : prj.GetItemProp(II_GEN_MAT_ORICHALCUM01);
				break;
			}
		case WND_ACCESSARY:
			{
				pItemProp = prj.GetItemProp( II_GEN_MAT_MOONSTONE );
				break;
			}
		case WND_PIERCING:
			{
				pItemProp = prj.GetItemProp( II_GEN_MAT_MOONSTONE );
				break;
			}
#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
		case WND_ELEMENT:
			{
				pItemProp = m_pSelectedElementalCardItemProp;
				if( m_pItemElem )
				{
					switch( m_pItemElem->GetItemResist() )
					{
					case SAI79::FIRE:
						{
							pItemProp = prj.GetItemProp( II_GEN_MAT_ELE_FLAME );
							break;
						}
					case SAI79::WATER:
						{
							pItemProp = prj.GetItemProp( II_GEN_MAT_ELE_RIVER );
							break;
						}
					case SAI79::ELECTRICITY:
						{
							pItemProp = prj.GetItemProp( II_GEN_MAT_ELE_GENERATOR );
							break;
						}
					case SAI79::WIND:
						{
							pItemProp = prj.GetItemProp( II_GEN_MAT_ELE_CYCLON );
							break;
						}
					case SAI79::EARTH:
						{
							pItemProp = prj.GetItemProp( II_GEN_MAT_ELE_DESERT );
							break;
						}
					}
				}
				break;
			}
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY
		}
#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
		if( m_eWndMode != WND_ELEMENT || pItemProp )
		{
			assert(pItemProp != NULL);
			pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pItemProp->szIcon), 0xffff00ff);
			assert(pTexture != NULL);
			nAlphaBlend = (m_Material[i].isUse != FALSE) ? NORMAL_ALPHA : TRANSLUCENT_ALPHA;
			pTexture->Render( p2DRender, CPoint( m_Material[i].wndCtrl->rect.left, m_Material[i].wndCtrl->rect.top ), nAlphaBlend );
		}
#else // __15_5TH_ELEMENTAL_SMELT_SAFETY
		assert(pItemProp != NULL);
		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pItemProp->szIcon), 0xffff00ff);
		assert(pTexture != NULL);
		nAlphaBlend = (m_Material[i].isUse != FALSE) ? NORMAL_ALPHA : TRANSLUCENT_ALPHA;
		pTexture->Render( p2DRender, CPoint( m_Material[i].wndCtrl->rect.left, m_Material[i].wndCtrl->rect.top ), nAlphaBlend );
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY

		assert(m_Scroll1[i].wndCtrl != NULL);
		switch(m_eWndMode)
		{
		case WND_NORMAL:
			{
				pItemProp = (m_pItemElem->GetProp()->dwReferStat1 == WEAPON_ULTIMATE) ? prj.GetItemProp(II_SYS_SYS_SCR_SMELPROT3) : prj.GetItemProp(II_SYS_SYS_SCR_SMELPROT);
				break;
			}
		case WND_ACCESSARY:
			{
				pItemProp = prj.GetItemProp( II_SYS_SYS_SCR_SMELPROT4 );
				break;
			}
		case WND_PIERCING:
			{
				pItemProp = prj.GetItemProp( II_SYS_SYS_SCR_PIEPROT );
				break;
			}
#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
		case WND_ELEMENT:
			{
				pItemProp = prj.GetItemProp( II_SYS_SYS_SCR_SMELPROT );
				break;
			}
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY
		}
		assert(pItemProp != NULL);
		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pItemProp->szIcon), 0xffff00ff);
		assert(pTexture != NULL);
		nAlphaBlend = (m_Scroll1[i].isUse != FALSE) ? NORMAL_ALPHA : TRANSLUCENT_ALPHA;
		pTexture->Render( p2DRender, CPoint( m_Scroll1[i].wndCtrl->rect.left, m_Scroll1[i].wndCtrl->rect.top ), nAlphaBlend );

#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
		if( m_eWndMode == WND_NORMAL && m_pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE && m_pItemElem->GetAbilityOption() < GENERAL_NON_USING_SCROLL2_LEVEL )
#else // __15_5TH_ELEMENTAL_SMELT_SAFETY
		if(m_eWndMode == WND_NORMAL && m_pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE && m_pItemElem->GetAbilityOption() < 7)
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY
		{
			assert(m_Scroll2[i].wndCtrl != NULL);
			pItemProp = prj.GetItemProp(II_SYS_SYS_SCR_SMELTING);
			assert(pItemProp != NULL);
			pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pItemProp->szIcon), 0xffff00ff);
			assert(pTexture != NULL);
			nAlphaBlend = (m_Scroll2[i].isUse != FALSE) ? NORMAL_ALPHA : TRANSLUCENT_ALPHA;
			pTexture->Render( p2DRender, CPoint( m_Scroll2[i].wndCtrl->rect.left, m_Scroll2[i].wndCtrl->rect.top ), nAlphaBlend );
		}
#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
		if( m_eWndMode == WND_ELEMENT && m_pItemElem && m_pItemElem->GetResistAbilityOption() < ELEMENTAL_NON_USING_SCROLL2_LEVEL )
		{
			assert( m_Scroll2[ i ].wndCtrl != NULL );
			pItemProp = prj.GetItemProp( II_SYS_SYS_SCR_SMELTING2 );
			assert( pItemProp != NULL );
			pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon ), 0xffff00ff );
			assert( pTexture != NULL );
			nAlphaBlend = ( m_Scroll2[ i ].isUse != FALSE ) ? NORMAL_ALPHA : TRANSLUCENT_ALPHA;
			pTexture->Render( p2DRender, CPoint( m_Scroll2[ i ].wndCtrl->rect.left, m_Scroll2[ i ].wndCtrl->rect.top ), nAlphaBlend );
		}
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY
	}
}

BOOL CWndSmeltSafety::IsDropMaterialZone(CPoint point)
{
	BOOL rtnval = FALSE;
	for(int i=0; i<SMELT_MAX; i++)
	{
		if(m_Material[i].wndCtrl->rect.PtInRect(point))
		{
			rtnval = TRUE;
			i = SMELT_MAX;
		}
	}
	return rtnval;
}

BOOL CWndSmeltSafety::IsDropScroll1Zone(CPoint point)
{
	BOOL rtnval = FALSE;
	for(int i=0; i<SMELT_MAX; i++)
	{
		if(m_Scroll1[i].wndCtrl->rect.PtInRect(point))
		{
			rtnval = TRUE;
			i = SMELT_MAX;
		}
	}
	return rtnval;
}

BOOL CWndSmeltSafety::IsDropScroll2Zone(CPoint point)
{
	BOOL rtnval = FALSE;
	for(int i=0; i<SMELT_MAX; i++)
	{
		if(m_Scroll2[i].wndCtrl->rect.PtInRect(point))
		{
			rtnval = TRUE;
			i = SMELT_MAX;
		}
	}
	return rtnval;
}

BOOL CWndSmeltSafety::IsAcceptableMaterial(ItemProp* pItemProp)
{
	assert(m_pItemElem != NULL);
	BOOL bAcceptableItem(FALSE);
	switch(m_eWndMode)
	{
	case WND_NORMAL:
		{
			if(m_pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE)
			{
				if(pItemProp->dwID == II_GEN_MAT_ORICHALCUM01 || pItemProp->dwID == II_GEN_MAT_ORICHALCUM01_1)
					bAcceptableItem = TRUE;
			}
			else
			{
				if(pItemProp->dwID == II_GEN_MAT_ORICHALCUM02)
					bAcceptableItem = TRUE;
			}
			break;
		}
	case WND_ACCESSARY:
	case WND_PIERCING:
		{
			if(pItemProp->dwID == II_GEN_MAT_MOONSTONE || pItemProp->dwID == II_GEN_MAT_MOONSTONE_1)
				bAcceptableItem = TRUE;
			break;
		}
#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
	case WND_ELEMENT:
		{
			switch( m_pItemElem->GetItemResist() )
			{
			case SAI79::FIRE:
				{
					if( pItemProp->dwID == II_GEN_MAT_ELE_FLAME )
						bAcceptableItem = TRUE;
					break;
				}
			case SAI79::WATER:
				{
					if( pItemProp->dwID == II_GEN_MAT_ELE_RIVER )
						bAcceptableItem = TRUE;
					break;
				}
			case SAI79::ELECTRICITY:
				{
					if( pItemProp->dwID == II_GEN_MAT_ELE_GENERATOR )
						bAcceptableItem = TRUE;
					break;
				}
			case SAI79::WIND:
				{
					if( pItemProp->dwID == II_GEN_MAT_ELE_CYCLON )
						bAcceptableItem = TRUE;
					break;
				}
			case SAI79::EARTH:
				{
					if( pItemProp->dwID == II_GEN_MAT_ELE_DESERT )
						bAcceptableItem = TRUE;
					break;
				}
			default:
				{
					if( m_nMaterialCount == m_nCurrentSmeltNumber )
					{
						if( CItemElem::IsElementalCard( pItemProp->dwID ) == TRUE )
						{
							m_pSelectedElementalCardItemProp = pItemProp;
							bAcceptableItem = TRUE;
						}
					}
					else
					{
						if( pItemProp == m_pSelectedElementalCardItemProp )
							bAcceptableItem = TRUE;
					}
				}
			}
			break;
		}
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY
	}
	return bAcceptableItem;
}

BOOL CWndSmeltSafety::IsAcceptableScroll1(ItemProp* pItemProp)
{
	assert(m_pItemElem != NULL);
	BOOL bAcceptableItem(FALSE);
	switch(m_eWndMode)
	{
	case WND_NORMAL:
		{
			if(m_pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE)
			{
				if(pItemProp->dwID == II_SYS_SYS_SCR_SMELPROT)
					bAcceptableItem = TRUE;
			}
			else
			{
				if(pItemProp->dwID == II_SYS_SYS_SCR_SMELPROT3)
					bAcceptableItem = TRUE;
			}
			break;
		}
	case WND_ACCESSARY:
		{
			if(pItemProp->dwID == II_SYS_SYS_SCR_SMELPROT4)
				bAcceptableItem = TRUE;
			break;
		}
	case WND_PIERCING:
		{
			if(pItemProp->dwID == II_SYS_SYS_SCR_PIEPROT)
				bAcceptableItem = TRUE;
			break;
		}
#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
	case WND_ELEMENT:
		{
			if( pItemProp->dwID == II_SYS_SYS_SCR_SMELPROT )
				bAcceptableItem = TRUE;
			break;
		}
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY
	}
	return bAcceptableItem;
}

BOOL CWndSmeltSafety::IsAcceptableScroll2(ItemProp* pItemProp)
{
#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
	assert(m_pItemElem != NULL);
	BOOL bAcceptableItem(FALSE);
	switch( m_eWndMode )
	{
	case WND_NORMAL:
		{
			if( m_pItemElem->GetAbilityOption() < GENERAL_NON_USING_SCROLL2_LEVEL && m_pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE )
			{
				if( pItemProp->dwID == II_SYS_SYS_SCR_SMELTING )
					bAcceptableItem = TRUE;
			}
			break;
		}
	case WND_ELEMENT:
		{
			if( m_pItemElem->GetResistAbilityOption() < ELEMENTAL_NON_USING_SCROLL2_LEVEL )
			{
				if( pItemProp->dwID == II_SYS_SYS_SCR_SMELTING2 )
					bAcceptableItem = TRUE;
			}
			break;
		}
	}
#else // __15_5TH_ELEMENTAL_SMELT_SAFETY
	BOOL bAcceptableItem(FALSE);
	assert(m_pItemElem != NULL);
	if(m_eWndMode == WND_NORMAL)
	{
		if(m_pItemElem->GetAbilityOption() < 7 && m_pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE)
		{
			if(pItemProp->dwID == II_SYS_SYS_SCR_SMELTING)
				bAcceptableItem = TRUE;
		}
	}
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY
	return bAcceptableItem;
}

int CWndSmeltSafety::GetNowSmeltValue(void)
{
	int nNowSmeltValue(0);
	if(m_pItemElem != NULL)
	{
		if(m_eWndMode == WND_NORMAL || m_eWndMode == WND_ACCESSARY)
		{
			nNowSmeltValue = m_pItemElem->GetAbilityOption();
		}
		else if(m_eWndMode == WND_PIERCING)
		{
			nNowSmeltValue = m_pItemElem->GetPiercingSize();
		}
#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
		else if( m_eWndMode == WND_ELEMENT )
			nNowSmeltValue = m_pItemElem->GetResistAbilityOption();
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY
	}
	return nNowSmeltValue;
}

int CWndSmeltSafety::GetDefaultMaxSmeltValue(void)
{
	assert(m_pItemElem != NULL);
	int nDefaultMaxSmeltValue(0);
	switch(m_eWndMode)
	{
	case WND_NORMAL:
		{
			nDefaultMaxSmeltValue = 10;
			break;
		}
	case WND_ACCESSARY:
		{
			nDefaultMaxSmeltValue = 20;
			break;
		}
	case WND_PIERCING:
		{
			if(m_pItemElem->GetProp()->dwItemKind3 == IK3_SUIT)
			{
				nDefaultMaxSmeltValue = 4;
			}
			else
			{
				nDefaultMaxSmeltValue = 10;
			}
			break;
		}
#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
	case WND_ELEMENT:
		{
			nDefaultMaxSmeltValue = 20;
			break;
		}
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY
	}
	return nDefaultMaxSmeltValue;
}

CWndSmeltSafetyConfirm::CWndSmeltSafetyConfirm(ErrorMode eErrorMode) : 
m_eErrorMode(eErrorMode), 
m_pItemElem(NULL)
{
}

CWndSmeltSafetyConfirm::~CWndSmeltSafetyConfirm()
{
}

BOOL CWndSmeltSafetyConfirm::Initialize( CWndBase* pWndParent, DWORD nType )
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_SMELT_SAFETY_CONFIRM, 0, CPoint( 0, 0 ), pWndParent );
}

void CWndSmeltSafetyConfirm::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_CONFIRM_TEXT);
	assert(pWndText != NULL);

	switch(m_eErrorMode)
	{
	case WND_ERROR1:
		{
			pWndText->AddString(prj.GetText(TID_GAME_SMELT_SAFETY_CONFIRM_NORMAL));
			break;
		}
	case WND_ERROR2:
		{
			pWndText->AddString(prj.GetText(TID_GAME_SMELT_SAFETY_CONFIRM_HIGHEST));
			break;
		}
	case WND_ERROR3:
		{
			pWndText->AddString(prj.GetText(TID_GAME_SMELT_SAFETY_CONFIRM_ACCESSARY));
			break;
		}
	}

	MoveParentCenter();
}

BOOL CWndSmeltSafetyConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	CWndInventory* pWndInventory = (CWndInventory*)GetWndBase(APP_INVENTORY);

	if(pWndInventory != NULL)
	{
		if(nID == WIDC_SMELT_YES)
		{
			if(m_pItemElem != NULL)
				pWndInventory->RunUpgrade(m_pItemElem);

			Destroy();
		}
		else if(nID == WIDC_SMELT_NO)
		{
			pWndInventory->BaseMouseCursor();
			Destroy();
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

void CWndSmeltSafetyConfirm::OnDestroy()
{
	CWndInventory* pWndInventory = (CWndInventory*)GetWndBase( APP_INVENTORY );
	if(pWndInventory != NULL)
		pWndInventory->BaseMouseCursor();
}

void CWndSmeltSafetyConfirm::SetWndMode(CItemElem* pItemElem)
{
	m_pItemElem = pItemElem;	
}
#endif //__SMELT_SAFETY

#if __VER >= 14 // __EQUIP_BIND
CWndEquipBindConfirm::CWndEquipBindConfirm(EquipAction eEquipAction) : 
m_eEquipAction(eEquipAction), 
m_pItemBase(NULL), 
m_dwObjId(0), 
m_pItemElem( NULL )
{
}

CWndEquipBindConfirm::~CWndEquipBindConfirm(void)
{
}

BOOL CWndEquipBindConfirm::Initialize( CWndBase* pWndParent, DWORD nType )
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_EQUIP_BIND_CONFIRM, WBS_MODAL | WBS_KEY, CPoint( 0, 0 ), pWndParent );
}

void CWndEquipBindConfirm::OnInitialUpdate( void )
{
	CWndNeuz::OnInitialUpdate();

	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_EQUIP_BIND_TEXT);
	assert(pWndText != NULL);
	pWndText->AddString(prj.GetText(TID_TOOLTIP_EQUIPBIND_CONFIRM));

	CWndEdit* pWndEdit = ( CWndEdit* )GetDlgItem( WIDC_EDIT_FOCUS );
	assert( pWndEdit );
	pWndEdit->Move( -100, -100 );
	pWndEdit->SetFocus();

	CWndButton* pWndButton = ( CWndButton* )GetDlgItem( WIDC_EQUIP_BIND_YES );
	assert( pWndButton );
	pWndButton->SetDefault( TRUE );

	MoveParentCenter();
}

BOOL CWndEquipBindConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( message == EN_RETURN && nID == WIDC_EDIT_FOCUS )
	{
		EquipItem();
		Destroy();
		return CWndNeuz::OnChildNotify( message, nID, pLResult );
	}

	switch( nID )
	{
	case WIDC_EQUIP_BIND_YES:
		{
			switch( m_eEquipAction )
			{
			case EQUIP_DOUBLE_CLICK:
				{
					EquipItem();
					Destroy();
					break;
				}
			case EQUIP_DRAG_AND_DROP:
				{
					g_DPlay.SendDoEquip( m_pItemElem );
					Destroy();
					break;
				}
			default:
				{
					Destroy();
				}
			}
			break;
		}
	case WIDC_EQUIP_BIND_NO:
		{
			Destroy();
			break;
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

void CWndEquipBindConfirm::SetInformation(CItemBase* pItemBase, DWORD dwObjId)
{
	m_eEquipAction = EQUIP_DOUBLE_CLICK;
	m_pItemBase = pItemBase;
	assert(m_pItemBase != NULL);
	m_dwObjId = dwObjId;
}

void CWndEquipBindConfirm::SetInformation(CItemElem* pItemElem)
{
	m_eEquipAction = EQUIP_DRAG_AND_DROP;
	m_pItemElem = pItemElem;
	assert(m_pItemElem != NULL);
}

void CWndEquipBindConfirm::EquipItem( void )
{
	ItemProp* pItemProp = m_pItemBase->GetProp();
	if( pItemProp != NULL)
	{
		int nPart = pItemProp->dwParts;
		BOOL bEquiped = g_pPlayer->m_Inventory.IsEquip( m_pItemBase->m_dwObjId );
		if( bEquiped != FALSE )
			nPart = m_pItemBase->m_dwObjIndex - g_pPlayer->m_Inventory.m_dwIndexNum;
		g_DPlay.SendDoUseItem( MAKELONG( ITYPE_ITEM, m_pItemBase->m_dwObjId ), m_dwObjId, nPart );
	}
}

#endif // __EQUIP_BIND

#if __VER >= 14 // __RESTATE_CONFIRM
CWndRestateConfirm::CWndRestateConfirm(DWORD dwItemID) : 
m_dwItemID(dwItemID), 
m_ObjID(0), 
m_nPart(-1)
{
}

CWndRestateConfirm::~CWndRestateConfirm(void)
{
}

BOOL CWndRestateConfirm::Initialize( CWndBase* pWndParent, DWORD nType )
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_RESTATE_CONFIRM, 0, CPoint( 0, 0 ), pWndParent );
}

void CWndRestateConfirm::OnInitialUpdate( void )
{
	CWndNeuz::OnInitialUpdate();

	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_RESTATE_CONFIRM_TEXT);
	assert(pWndText != NULL);
	switch(m_dwItemID)
	{
	case II_CHR_SYS_SCR_RESTATE:
		{
			pWndText->AddString(prj.GetText(TID_TOOLTIP_RESTATE_ALL_CONFIRM));
			break;
		}
	case II_CHR_SYS_SCR_RESTATE_STR:
		{
			pWndText->AddString(prj.GetText(TID_TOOLTIP_RESTATE_STR_CONFIRM));
			break;
		}
	case II_CHR_SYS_SCR_RESTATE_STA:
		{
			pWndText->AddString(prj.GetText(TID_TOOLTIP_RESTATE_STA_CONFIRM));
			break;
		}
	case II_CHR_SYS_SCR_RESTATE_DEX:
		{
			pWndText->AddString(prj.GetText(TID_TOOLTIP_RESTATE_DEX_CONFIRM));
			break;
		}
	case II_CHR_SYS_SCR_RESTATE_INT:
		{
			pWndText->AddString(prj.GetText(TID_TOOLTIP_RESTATE_INT_CONFIRM));
			break;
		}
#ifdef __ADD_RESTATE_LOW
	case II_CHR_SYS_SCR_RESTATE_STR_LOW:
		{
			pWndText->AddString( prj.GetText( TID_GAME_RESTATE_STR_LOW_CONFIRM ) );
			break;
		}
	case II_CHR_SYS_SCR_RESTATE_STA_LOW:
		{
			pWndText->AddString( prj.GetText( TID_GAME_RESTATE_STA_LOW_CONFIRM ) );
			break;
		}
	case II_CHR_SYS_SCR_RESTATE_DEX_LOW:
		{
			pWndText->AddString( prj.GetText( TID_GAME_RESTATE_DEX_LOW_CONFIRM ) );
			break;
		}
	case II_CHR_SYS_SCR_RESTATE_INT_LOW:
		{
			pWndText->AddString( prj.GetText( TID_GAME_RESTATE_INT_LOW_CONFIRM ) );
			break;
		}
#endif // __ADD_RESTATE_LOW
	}

	MoveParentCenter();
}

BOOL CWndRestateConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	CWndInventory* pWndInventory = (CWndInventory*)GetWndBase(APP_INVENTORY);
	if(pWndInventory != NULL)
	{
		switch(nID)
		{
		case WIDC_RESTATE_CONFIRM_YES:
			{
				g_DPlay.SendDoUseItem(m_dwItemObjID, m_ObjID, m_nPart);
				Destroy();
				break;
			}
		case WIDC_RESTATE_CONFIRM_NO:
			{
				Destroy();
				break;
			}
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

void CWndRestateConfirm::SetInformation(DWORD dwItemObjID, OBJID m_ObjID, int nPart)
{
	m_dwItemObjID = dwItemObjID;
	m_ObjID = m_ObjID;
	m_nPart = nPart;
}
#endif //__RESTATE_CONFIRM

#if __VER >= 15 // __CAMPUS
//-----------------------------------------------------------------------------
CWndCampusInvitationConfirm::CWndCampusInvitationConfirm( u_long idSender, const CString& rstrSenderName ) : 
m_idSender( idSender ), 
m_strSenderName( rstrSenderName )
{
}
//-----------------------------------------------------------------------------
CWndCampusInvitationConfirm::~CWndCampusInvitationConfirm( void )
{
}
//-----------------------------------------------------------------------------
BOOL CWndCampusInvitationConfirm::Initialize( CWndBase* pWndParent, DWORD nType )
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_CONFIRM_ENTER, WBS_KEY, 0, pWndParent );
}
//-----------------------------------------------------------------------------
void CWndCampusInvitationConfirm::OnInitialUpdate( void )
{
	CWndNeuz::OnInitialUpdate();

	CWndText* pWndText = ( CWndText* )GetDlgItem( WIDC_TEXT1 );
	if( pWndText )
	{
		CString strMessage = _T( "" );
		strMessage.Format( prj.GetText( TID_GAME_CAMPUS_INVITATION_CONFIRM ), m_strSenderName ); // %s 님이 사제 신청을 하셨습니다. 수락하시겠습니까?
		pWndText->AddString( strMessage );
	}
	CWndButton* pOk = ( CWndButton* )GetDlgItem( WIDC_BUTTON1 );
	CWndButton* pCancel = ( CWndButton* )GetDlgItem( WIDC_BUTTON2 );
	CWndEdit* pEdit = ( CWndEdit* )GetDlgItem( WIDC_EDIT1 );
	AddWndStyle( WBS_MODAL );
	pOk->SetDefault( TRUE );

	//에디트창을 안 보이는 곳으로 보내 버리고 ENTER 받을 준비
	pEdit->Move( -100, -100 );
	pEdit->SetFocus( );

	MoveParentCenter();
}
//-----------------------------------------------------------------------------
BOOL CWndCampusInvitationConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( message == EN_RETURN && nID == WIDC_EDIT1 )
	{
		g_DPlay.SendAcceptCampusMember( m_idSender );
		Destroy();
		return CWndNeuz::OnChildNotify( message, nID, pLResult );
	}

	switch( nID )
	{
	case WIDC_BUTTON1:
		{
			g_DPlay.SendAcceptCampusMember( m_idSender );
			Destroy();
			break;
		}
	case WIDC_BUTTON2:
		{
			g_DPlay.SendRefuseCampusMember( m_idSender );
			Destroy();
			break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
//-----------------------------------------------------------------------------
CWndCampusSeveranceConfirm::CWndCampusSeveranceConfirm( u_long idTarget, const CString& rstrTargetName ) : 
m_idTarget( idTarget ), 
m_strTargetName( rstrTargetName )
{
}
//-----------------------------------------------------------------------------
CWndCampusSeveranceConfirm::~CWndCampusSeveranceConfirm( void )
{
}
//-----------------------------------------------------------------------------
BOOL CWndCampusSeveranceConfirm::Initialize( CWndBase* pWndParent, DWORD nType )
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_CONFIRM_ENTER, WBS_KEY, 0, pWndParent );
}
//-----------------------------------------------------------------------------
void CWndCampusSeveranceConfirm::OnInitialUpdate( void )
{
	CWndNeuz::OnInitialUpdate();

	CWndText* pWndText = ( CWndText* )GetDlgItem( WIDC_TEXT1 );
	if( pWndText )
	{
		CString strMessage = _T( "" );
		strMessage.Format( prj.GetText( TID_GAME_CAMPUS_SEVERANCE_CONFIRM ), m_strTargetName ); // %s 님과 사제 관계를 끊겠습니까?
		pWndText->AddString( strMessage );
	}
	CWndButton* pOk = ( CWndButton* )GetDlgItem( WIDC_BUTTON1 );
	CWndButton* pCancel = ( CWndButton* )GetDlgItem( WIDC_BUTTON2 );
	CWndEdit* pEdit = ( CWndEdit* )GetDlgItem( WIDC_EDIT1 );
	AddWndStyle( WBS_MODAL );
	pOk->SetDefault( TRUE );

	//에디트창을 안 보이는 곳으로 보내 버리고 ENTER 받을 준비
	pEdit->Move( -100, -100 );
	pEdit->SetFocus( );

	MoveParentCenter();
}
//-----------------------------------------------------------------------------
BOOL CWndCampusSeveranceConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( message == EN_RETURN && nID == WIDC_EDIT1 )
	{
		g_DPlay.SendRemoveCampusMember( m_idTarget );
		Destroy();
		return CWndNeuz::OnChildNotify( message, nID, pLResult );
	}
	
	switch( nID )
	{
	case WIDC_BUTTON1:
		{
			g_DPlay.SendRemoveCampusMember( m_idTarget );
			Destroy();
			break;
		}
	case WIDC_BUTTON2:
		{
			Destroy();
			break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
//-----------------------------------------------------------------------------
#endif // __CAMPUS

#ifdef __NEW_ITEM_VARUNA


////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndBarunaNPC
////////////////////////////////////////////////////////////////////////////////////////////////////
CWndBarunaNPC::CWndBarunaNPC( )
{
	m_pPetModel = NULL;
	m_fRadius = 0.0f;

	m_nEXPWidth = -1;
	m_pVBEXPGauge = NULL;
	m_bVBEXPGauge = TRUE;

#if __VER >= 19
	m_nLevel = 1;
#else
	m_nLevel = 0;
#endif // __VER >= 19

	m_nFailCounter = 0;

	m_nFailLevelTable[0] = 8;
	m_nFailLevelTable[1] = 25;
	m_nFailLevelTable[2] = 80;
	m_nFailLevelTable[3] = 200;
	m_nFailLevelTable[4] = 500;
//	{ 100 , 250, 400, 800, 2000 };
}
CWndBarunaNPC::~CWndBarunaNPC( )
{
	DeleteDeviceObjects();
}
void CWndBarunaNPC::MakeGaugeVertex()
{
	LPWNDCTRL lpExp  = GetWndCtrl( WIDC_CUSTOM1 );

	CMover* pMover = CMover::GetActiveMover();
	if( pMover )
	{
		CRect rect = GetClientRect();
		CRect rectTemp;
		//	pMover->m_nHitPoint = 50;
		int nWidthClient = lpExp->rect.Width();
		//100 : rect = hp : x
		int nWidth;
		nWidth	= (int)( ( (float) nWidthClient / m_nFailLevelTable[m_nLevel] ) * m_nFailCounter );
//		if( m_nEXPWidth != nWidth ) 
		{
			m_nEXPWidth = nWidth;
			rect = lpExp->rect;//rect.SetRect( 105, 48, 105 + nWidthClient, 48 + 8 );
			rect.top -= 1;
			rectTemp = rect; 
			rectTemp.right = rectTemp.left + nWidth;
			ClientToScreen( rectTemp );
#if __VER >= 19
			m_bVBEXPGauge = m_pTheme->MakeGaugeVertex( m_pApp->m_pd3dDevice, &rectTemp, 0xffffffff, m_pVBEXPGauge, &m_texGauFillSmall );
#else
			m_bVBEXPGauge = m_pTheme->MakeGaugeVertex( m_pApp->m_pd3dDevice, &rectTemp, 0x8437dc37, m_pVBEXPGauge, &m_texGauFillSmall );
#endif// __VER >= 19
		}

	}
}
HRESULT CWndBarunaNPC::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if( m_pVBEXPGauge == NULL )
	{
		m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBEXPGauge, NULL );
		m_nEXPWidth = -1;
	}
	return S_OK;
}
HRESULT CWndBarunaNPC::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
    SAFE_RELEASE( m_pVBEXPGauge );
	return S_OK;
}
HRESULT CWndBarunaNPC::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	return InvalidateDeviceObjects();
}
void CWndBarunaNPC::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	RestoreDeviceObjects();

#if __VER >= 19
	m_texGauEmptyNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "BarSky.tga" ), 0xffff00ff, TRUE );
	m_texGauEmptySmall.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "BarSky.tga" ), 0xffff00ff, TRUE );
	m_texGauFillNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "BarSky.tga" ), 0xffff00ff, TRUE );
	m_texGauFillSmall.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "BarSky.tga" ), 0xffff00ff, TRUE );
#else
	m_texGauEmptyNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptyNormal.bmp" ), 0xffff00ff, TRUE );
	m_texGauEmptySmall.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptyNormal.bmp" ), 0xffff00ff, TRUE );
	m_texGauFillNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptyNormal.bmp" ), 0xffff00ff, TRUE );
	m_texGauFillSmall.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptyNormal.bmp" ), 0xffff00ff, TRUE );
#endif//__VER >= 19

	m_pLevelStatic = ( CWndStatic* )GetDlgItem( WIDC_STATIC3 );

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
}
BOOL CWndBarunaNPC::Initialize(CWndBase* pWndParent,DWORD dwWndId)
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_BARUNA_NPC, 0, CPoint( 0, 0 ), pWndParent );
}
void CWndBarunaNPC::OnDraw(C2DRender* p2DRender)
{
	if( g_pPlayer == NULL ) 
		return;
	
	CMover* pBarunaPet = NULL;
	pBarunaPet = prj.GetMover( g_pPlayer->GetEatPetId( ) );
 	if( pBarunaPet )
	{
		m_pPetModel = (CModelObject*)pBarunaPet->GetModel( );
		SetTitle( pBarunaPet->GetName() );
	}
	else
	{
		Destroy();
		return;
	}



	CRect rect = GetClientRect();
	int nWidthClient = GetClientRect().Width() - 110;// - 30;

	CRect rectTemp;
	LPWNDCTRL lpExp  = GetWndCtrl( WIDC_CUSTOM1 );
	LPWNDCTRL lpFace = GetWndCtrl( WIDC_CUSTOM2 );

	MakeGaugeVertex();

	if( m_bVBEXPGauge )
	{
		CRect rectTemp;
		rectTemp = lpExp->rect;;
		m_pTheme->RenderGauge( p2DRender->m_pd3dDevice, m_pVBEXPGauge, &m_texGauFillSmall );
//		m_pTheme->RenderGauge( p2DRender, &rectTemp, 0x8437dc37 , m_pVBEXPGauge, &m_texGauFillSmall );
	}

	{
		DWORD dwColor = D3DCOLOR_ARGB(255, 132, 86, 135 );
		char cbufExp[16] = {0,};
		int nCharEXP;

		int	nExpResult = m_nFailCounter * 100 / m_nFailLevelTable[m_nLevel];
		nCharEXP = sprintf( cbufExp, "%d%%", nExpResult );

		int x = lpExp->rect.left; // -40
		p2DRender->TextOut( x + 20, lpExp->rect.top - 0, cbufExp, dwColor, 0xffffffff );//ADEILSON 0xff000000
	}	

	CPoint point;
	point = CPoint( lpFace->rect.left-12, lpFace->rect.top-12 );

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
	D3DVIEWPORT9 viewport;
	viewport.X      = p2DRender->m_ptOrigin.x + lpFace->rect.left;
	viewport.Y      = p2DRender->m_ptOrigin.y + lpFace->rect.top;
	viewport.Width  = lpFace->rect.Width();
	viewport.Height = lpFace->rect.Height();
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;

	pd3dDevice->SetViewport(&viewport);

	D3DXMATRIX matProj;
	D3DXMatrixIdentity( &matProj );
	FLOAT fAspect = ((FLOAT)viewport.Width) / (FLOAT)viewport.Height;
	FLOAT fov = D3DX_PI/4.0f;//796.0f;
	FLOAT h = cos(fov/2) / sin(fov/2);
	FLOAT w = h * fAspect;
	D3DXMatrixOrthoLH( &matProj, w, h, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	D3DXMATRIX  matView;
	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 matScale;
	D3DXMATRIXA16 matRot1, matRot2;
	D3DXMATRIXA16 matTrans;

	D3DXMatrixIdentity(&matScale);
	D3DXMatrixIdentity(&matRot1);
	D3DXMatrixIdentity(&matRot2);
	D3DXMatrixIdentity(&matTrans);
	D3DXMatrixIdentity(&matWorld);

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

	vecPos = pObj3D->m_vEvent[ 0 ];

	float fModelScale = pModel->m_pModelElem->m_fScale;
	if( fModelScale < 1.0f && fModelScale > 0.001f )
		vecPos *= ( fModelScale - fModelScale * (0.5f + ( 1.0f - fModelScale ) * 0.01f ) );	//스케일 변동치가 클수록 
	else if ( fModelScale > 1.0f )
		vecPos *= ( fModelScale  - fModelScale * (0.9f + fModelScale * 0.01f) );
	m_fRadius = pModel->GetRadius( );
	vecPos.x += 0.5f;
	vecPos.y += 2.8f; //1.8f;
	vecPos.z -= ( 3.0f ); //* fRadius );
	vecLookAt.x = -0.24f;
	vecLookAt.y = 1.5f;
	vecLookAt.z = 1.0f;
	fScale = (1 / m_fRadius ) * 1.5f; // ( 2.0f * fRadius );
	D3DXMatrixScaling(&matScale, fScale, fScale, fScale);
	D3DXMatrixLookAtLH( &matView, &vecPos, &vecLookAt, &D3DXVECTOR3(0.0f,1.0f,0.0f) );
	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
	D3DXMatrixMultiply(&matWorld,&matWorld,&matScale);
	D3DXMatrixMultiply(&matWorld, &matWorld,&matRot1);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, CWorld::m_dwBgColor, 1.0f, 0 ) ;
	::SetTransformView( matView );
	::SetTransformProj( matProj );
 	::SetLight( FALSE );
 	::SetFog( FALSE );
 	SetDiffuse( 1.0f, 1.0f, 1.0f );
 	SetAmbient( 1.0f, 1.0f, 1.0f );
//	m_pPetModel->SetTextureEx( m_pPetModel->m_pModelElem->m_nTextureEx );
	m_pPetModel->Render(pd3dDevice, &matWorld);
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE ); 
}
BOOL CWndBarunaNPC::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
}
BOOL CWndBarunaNPC::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}
void CWndBarunaNPC::OnDestroy()
{
	if( NULL !=  g_WndMng.GetWndBase( APP_BARUNA_ENCHANT ))
			g_WndMng.m_pWndBarunaEnchant->Destroy();
	if( NULL !=  g_WndMng.GetWndBase( APP_BARUNA_EXTRACT_OFFER ))
			g_WndMng.m_pWndBarunaExtractOffer->Destroy();
	if( NULL !=  g_WndMng.GetWndBase( APP_BARUNA_OFFER ))
			g_WndMng.m_pWndBarunaOffer->Destroy();
	if( NULL !=  g_WndMng.GetWndBase( APP_BARUNA_SEED ))
			g_WndMng.m_pWndBarunaSeed->Destroy();
	if( NULL !=  g_WndMng.GetWndBase( APP_BARUNA_OFFERSEED ))
			g_WndMng.m_pWndBarunaOfferSeed->Destroy();
#ifdef __BARUNA_PIERCING
	if( NULL !=  g_WndMng.GetWndBase( APP_BARUNA_PIERCE ))
			g_WndMng.m_pWndBarunaPiercing->Destroy();
#endif // __BARUNA_PIERCING

}
BOOL CWndBarunaNPC::Process()
{
	if(!IsValidObj(g_pPlayer))
		return FALSE;
	
	CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAtId( m_dwItemId );

	char buffer[10] = { 0 };
	_itoa( pItemElem->m_nRepairNumber, buffer, 10 );

	m_pLevelStatic->SetTitle( buffer );

	m_nLevel = pItemElem->m_nRepairNumber;
	m_nFailCounter = pItemElem->m_nRepair;

	return TRUE;
}
void CWndBarunaNPC::SetInfo( DWORD dwItemId )
{
	m_dwItemId = dwItemId;
	CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAtId( m_dwItemId );
	m_nLevel = pItemElem->m_nRepairNumber;
	m_nFailCounter = pItemElem->m_nRepair;

	CMover* pBarunaPet = NULL;
	pBarunaPet = prj.GetMover( g_pPlayer->GetEatPetId( ) );
 	if( pBarunaPet )
	{
		m_pPetModel = (CModelObject*)pBarunaPet->GetModel( );
	}
}

CWndBarunaExtractOffer::CWndBarunaExtractOffer() 
{
	m_pItemElem = NULL;
	m_pEItemProp = NULL;
	m_pTexture = NULL;
}
CWndBarunaExtractOffer::~CWndBarunaExtractOffer() 
{
	if(m_pItemElem != NULL)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pItemElem ) )
			m_pItemElem->SetExtra(0);
	}
}
void CWndBarunaExtractOffer::OnDraw( C2DRender* p2DRender ) 
{
	ItemProp* pItemProp;
	
	if(m_pItemElem != NULL)
	{
		pItemProp = m_pItemElem->GetProp();
		LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC1 );
		if(pItemProp != NULL)
		{
			if(m_pTexture != NULL)
				m_pTexture->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
		}
	} 
}
void CWndBarunaExtractOffer::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	pButton->EnableWindow(FALSE);

	MoveParentCenter();
} 
BOOL CWndBarunaExtractOffer::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_BARUNA_EXTRACT_OFFER, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndBarunaExtractOffer::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndBarunaExtractOffer::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
BOOL CWndBarunaExtractOffer::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC1 );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{		
		ItemProp* pItemProp;
		CItemElem* pTempElem;
		pTempElem  = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
		
		if( pTempElem != NULL )
		{
			pItemProp = pTempElem->GetProp();
			if( pItemProp->dwItemKind1 == IK1_WEAPON || pItemProp->dwItemKind1 == IK1_ARMOR )
			{
				if(m_pItemElem != NULL)
					m_pItemElem->SetExtra(0);

				m_pItemElem = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
				m_pEItemProp = m_pItemElem->GetProp();
				m_pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, m_pEItemProp->szIcon), 0xffff00ff );
				m_pItemElem->SetExtra(m_pItemElem->GetExtra()+1);
				CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
				pButton->EnableWindow(TRUE);
			}
			else
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_NEWSMELT_OPEREXTRACT01 ) );
		}
	}
	return TRUE;
}
BOOL CWndBarunaExtractOffer::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
		case WIDC_BUTTON1:
		{
			g_DPlay.SendBaruna( 0x03, m_pItemElem->m_dwObjId );
			Destroy();
			break;
		}
		case WIDC_BUTTON2:
		{
			Destroy();
			break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

CWndBarunaOffer::CWndBarunaOffer() 
{
	m_pItemElem = NULL;
	m_pItemElem2 = NULL;
	m_pEItemProp = NULL;
	m_pEItemProp2 = NULL;
	m_pTexture = NULL;
	m_pTexture2 = NULL;
}
CWndBarunaOffer::~CWndBarunaOffer() 
{
	if(m_pItemElem != NULL)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pItemElem ) )
			m_pItemElem->SetExtra(0);
	}
	if(m_pItemElem2 != NULL)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pItemElem2 ) )
			m_pItemElem2->SetExtra(0);
	}
}
void CWndBarunaOffer::OnDraw( C2DRender* p2DRender ) 
{
	ItemProp* pItemProp;
	
	if(m_pItemElem != NULL)
	{
		pItemProp = m_pItemElem->GetProp();
		LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC1 );
		if(pItemProp != NULL)
		{
			if(m_pTexture != NULL)
				m_pTexture->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
		}
	} 
	if(m_pItemElem2 != NULL)
	{
		pItemProp = m_pItemElem2->GetProp();
		LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC2 );
		if(pItemProp != NULL)
		{
			if(m_pTexture2 != NULL)
				m_pTexture2->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
		}
	} 
}
void CWndBarunaOffer::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	pButton->EnableWindow(FALSE);

	MoveParentCenter();
} 
BOOL CWndBarunaOffer::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_BARUNA_OFFER, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndBarunaOffer::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndBarunaOffer::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
BOOL CWndBarunaOffer::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC1 );
	rect = wndCtrl->rect;
	CRect rect2;
	LPWNDCTRL wndCtrl2 = GetWndCtrl( WIDC_STATIC2 );
	rect2 = wndCtrl2->rect;

	if( rect.PtInRect( point ) )
	{		
		ItemProp* pItemProp;
		CItemElem* pTempElem;
		pTempElem  = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
		
		if( pTempElem != NULL )
		{
			pItemProp = pTempElem->GetProp();
			if( pItemProp->dwID == II_GEN_MAT_OPERPIECE )
			{
				if(m_pItemElem != NULL)
					m_pItemElem->SetExtra(0);

				m_pItemElem = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
				m_pEItemProp = m_pItemElem->GetProp();
				m_pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, m_pEItemProp->szIcon), 0xffff00ff );
				m_pItemElem->SetExtra(m_pItemElem->GetExtra()+1);
			}
			else
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_GHOUSE_OPERPIECE ) );
		}
	}else
	if( rect2.PtInRect( point ) )
	{		
		ItemProp* pItemProp;
		CItemElem* pTempElem;
		pTempElem  = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
		
		if( pTempElem != NULL )
		{
			pItemProp = pTempElem->GetProp();
			if( pItemProp->dwID == II_GEN_MAT_OPERMIX )
			{
				if(m_pItemElem2 != NULL)
					m_pItemElem2->SetExtra(0);

				m_pItemElem2 = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
				m_pEItemProp2 = m_pItemElem2->GetProp();
				m_pTexture2 = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, m_pEItemProp2->szIcon), 0xffff00ff );
				m_pItemElem2->SetExtra(m_pItemElem2->GetExtra()+1);
			}
			else
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_GHOUSE_OPERMIX ) );
		}
	}
	if( m_pItemElem != NULL && m_pItemElem2 != NULL )
	{
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
		pButton->EnableWindow(TRUE);
	}
	return TRUE;
}
BOOL CWndBarunaOffer::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
		case WIDC_BUTTON1:
		{
			g_DPlay.SendBaruna( 0x05, m_pItemElem->m_dwObjId, m_pItemElem2->m_dwObjId );
			Destroy();
			break;
		}
		case WIDC_BUTTON2:
		{
			Destroy();
			break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

CWndBarunaEnchant::CWndBarunaEnchant() 
{
	m_pItemElem = NULL;
	m_pItemElem2 = NULL;
	m_pItemElem3 = NULL;
	m_pEItemProp = NULL;
	m_pEItemProp2 = NULL;
	m_pEItemProp3 = NULL;
	m_pTexture = NULL;
	m_pTexture2 = NULL;
	m_pTexture3 = NULL;
}
CWndBarunaEnchant::~CWndBarunaEnchant() 
{
	if(m_pItemElem != NULL)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pItemElem ) )
			m_pItemElem->SetExtra(0);
	}
	if(m_pItemElem2 != NULL)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pItemElem2 ) )
			m_pItemElem2->SetExtra(0);
	}
	if(m_pItemElem3 != NULL)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pItemElem3 ) )
			m_pItemElem3->SetExtra(0);
	}
}
void CWndBarunaEnchant::OnDraw( C2DRender* p2DRender ) 
{
	ItemProp* pItemProp;
	
	if(m_pItemElem != NULL)
	{
		pItemProp = m_pItemElem->GetProp();
		LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC2 );
		if(pItemProp != NULL)
		{
			if(m_pTexture != NULL)
				m_pTexture->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
		}
	} 
	if(m_pItemElem2 != NULL)
	{
		pItemProp = m_pItemElem2->GetProp();
		LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC3 );
		if(pItemProp != NULL)
		{
			if(m_pTexture2 != NULL)
				m_pTexture2->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
		}
	} 
	if(m_pItemElem3 != NULL)
	{
		pItemProp = m_pItemElem3->GetProp();
		LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC4 );
		if(pItemProp != NULL)
		{
			if(m_pTexture3 != NULL)
				m_pTexture3->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
		}
	} 
}
void CWndBarunaEnchant::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	pButton->EnableWindow(FALSE);

	MoveParentCenter();
} 
BOOL CWndBarunaEnchant::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_BARUNA_ENCHANT, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndBarunaEnchant::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndBarunaEnchant::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
BOOL CWndBarunaEnchant::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC2 );
	rect = wndCtrl->rect;
	CRect rect2;
	LPWNDCTRL wndCtrl2 = GetWndCtrl( WIDC_STATIC3 );
	rect2 = wndCtrl2->rect;
	CRect rect3;
	LPWNDCTRL wndCtrl3 = GetWndCtrl( WIDC_STATIC4 );
	rect3 = wndCtrl3->rect;

	if( rect.PtInRect( point ) )
	{		
		ItemProp* pItemProp;
		CItemElem* pTempElem;
		pTempElem  = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
		
		if( pTempElem != NULL )
		{
			pItemProp = pTempElem->GetProp();
			if( pItemProp->dwItemKind3 == IK3_BARUNA_PROTECTION || pItemProp->dwItemKind3 == IK3_BARUNA_PROTECTION_KEEP )
			{
				if(m_pItemElem != NULL)
					m_pItemElem->SetExtra(0);

				m_pItemElem = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
				m_pEItemProp = m_pItemElem->GetProp();
				m_pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, m_pEItemProp->szIcon), 0xffff00ff );
				m_pItemElem->SetExtra(m_pItemElem->GetExtra()+1);
			}
			else
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_GHOUSE_BARUNASMELPROT ) );
		}
	}else
	if( rect2.PtInRect( point ) )
	{		
		ItemProp* pItemProp;
		CItemElem* pTempElem;
		pTempElem  = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
		
		if( pTempElem != NULL )
		{
			pItemProp = pTempElem->GetProp();
			if( pItemProp->dwReferStat1 >= BARUNA_D &&  pItemProp->dwReferStat1 <= BARUNA_S )
			{
				if(m_pItemElem2 != NULL)
					m_pItemElem2->SetExtra(0);

				m_pItemElem2 = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
				m_pEItemProp2 = m_pItemElem2->GetProp();
				m_pTexture2 = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, m_pEItemProp2->szIcon), 0xffff00ff );
				m_pItemElem2->SetExtra(m_pItemElem2->GetExtra()+1);
			}
			else
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_NEWSMELT_NEWSMELT01 ) );
		}
	}else
	if( rect3.PtInRect( point ) )
	{		
		ItemProp* pItemProp;
		CItemElem* pTempElem;
		pTempElem  = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
		
		if( pTempElem != NULL )
		{
			pItemProp = pTempElem->GetProp();
			if( pItemProp->dwID == II_GEN_MAT_OPERCID )
			{
				if(m_pItemElem3 != NULL)
					m_pItemElem3->SetExtra(0);

				m_pItemElem3 = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
				m_pEItemProp3 = m_pItemElem3->GetProp();
				m_pTexture3 = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, m_pEItemProp3->szIcon), 0xffff00ff );
				m_pItemElem3->SetExtra(m_pItemElem3->GetExtra()+1);
			}
			else
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_NEWSMELT_NEWSMELT02 ) );
		}
	}
	if( m_pItemElem3 != NULL && m_pItemElem2 != NULL )
	{
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
		pButton->EnableWindow(TRUE);
	}
	return TRUE;
}
BOOL CWndBarunaEnchant::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
		case WIDC_BUTTON1:
		{
			if( m_pItemElem != NULL )
				g_DPlay.SendBaruna( 0x01, m_pItemElem2->m_dwObjId, m_pItemElem3->m_dwObjId, 0, m_pItemElem->m_dwObjId );
			else				
				g_DPlay.SendBaruna( 0x01, m_pItemElem2->m_dwObjId, m_pItemElem3->m_dwObjId );
			Destroy();
			break;
		}
		case WIDC_BUTTON2:
		{
			Destroy();
			break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

CWndBarunaSeed::CWndBarunaSeed() 
{
	m_pItemElem = NULL;
	m_pItemElem2 = NULL;
	m_pEItemProp = NULL;
	m_pEItemProp2 = NULL;
	m_pTexture = NULL;
	m_pTexture2 = NULL;
}
CWndBarunaSeed::~CWndBarunaSeed() 
{
	if(m_pItemElem != NULL)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pItemElem ) )
			m_pItemElem->SetExtra(0);
	}
	if(m_pItemElem2 != NULL)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pItemElem2 ) )
			m_pItemElem2->SetExtra(0);
	}
}
void CWndBarunaSeed::OnDraw( C2DRender* p2DRender ) 
{
	ItemProp* pItemProp;
	
	if(m_pItemElem != NULL)
	{
		pItemProp = m_pItemElem->GetProp();
		LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC1 );
		if(pItemProp != NULL)
		{
			if(m_pTexture != NULL)
				m_pTexture->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
		}
	} 
	if(m_pItemElem2 != NULL)
	{
		pItemProp = m_pItemElem2->GetProp();
		LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC2 );
		if(pItemProp != NULL)
		{
			if(m_pTexture2 != NULL)
				m_pTexture2->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
		}
	} 
}
void CWndBarunaSeed::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	pButton->EnableWindow(FALSE);

	MoveParentCenter();
} 
BOOL CWndBarunaSeed::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_BARUNA_SEED, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndBarunaSeed::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndBarunaSeed::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
BOOL CWndBarunaSeed::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC1 );
	rect = wndCtrl->rect;
	CRect rect2;
	LPWNDCTRL wndCtrl2 = GetWndCtrl( WIDC_STATIC2 );
	rect2 = wndCtrl2->rect;

	if( rect.PtInRect( point ) )
	{		
		ItemProp* pItemProp;
		CItemElem* pTempElem;
		pTempElem  = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
		
		if( pTempElem != NULL )
		{
			pItemProp = pTempElem->GetProp();
			if( pItemProp->dwItemKind3 == IK3_CIDCRYSTAL && ( pTempElem->m_nItemNum >= 10 || pTempElem->m_dwItemId == II_GEN_MAT_CIDPIEC01 ))
			{
				if(m_pItemElem != NULL)
					m_pItemElem->SetExtra(0);

				m_pItemElem = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
				m_pEItemProp = m_pItemElem->GetProp();
				m_pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, m_pEItemProp->szIcon), 0xffff00ff );
				m_pItemElem->SetExtra(m_pItemElem->GetExtra()+10);
			}
			else
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_NEWSMELT_CID05 ) );
		}
	}else
	if( rect2.PtInRect( point ) )
	{		
		ItemProp* pItemProp;
		CItemElem* pTempElem;
		pTempElem  = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
		
		if( pTempElem != NULL )
		{
			pItemProp = pTempElem->GetProp();
			if( pItemProp->dwItemKind3 == IK3_CIDMIX )
			{
				if(m_pItemElem2 != NULL)
					m_pItemElem2->SetExtra(0);

				m_pItemElem2 = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
				m_pEItemProp2 = m_pItemElem2->GetProp();
				m_pTexture2 = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, m_pEItemProp2->szIcon), 0xffff00ff );
				m_pItemElem2->SetExtra(m_pItemElem2->GetExtra()+1);
			}
			else
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_NEWSMELT_OPER01 ) );
		}
	}
	if( m_pItemElem != NULL && m_pItemElem2 != NULL )
	{
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
		pButton->EnableWindow(TRUE);
	}
	return TRUE;
}
BOOL CWndBarunaSeed::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
		case WIDC_BUTTON1:
		{
			g_DPlay.SendBaruna( 0x02, m_pItemElem->m_dwObjId, m_pItemElem2->m_dwObjId );
			Destroy();
			break;
		}
		case WIDC_BUTTON2:
		{
			Destroy();
			break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

CWndBarunaOfferSeed::CWndBarunaOfferSeed() 
{
	m_pItemElem = NULL;
	m_pItemElem2 = NULL;
	m_pItemElem3 = NULL;
	m_pEItemProp = NULL;
	m_pEItemProp2 = NULL;
	m_pEItemProp3 = NULL;
	m_pTexture = NULL;
	m_pTexture2 = NULL;
	m_pTexture3 = NULL;
}
CWndBarunaOfferSeed::~CWndBarunaOfferSeed() 
{
	if(m_pItemElem != NULL)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pItemElem ) )
			m_pItemElem->SetExtra(0);
	}
	if(m_pItemElem2 != NULL)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pItemElem2 ) )
			m_pItemElem2->SetExtra(0);
	}
	if(m_pItemElem3 != NULL)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pItemElem3 ) )
			m_pItemElem3->SetExtra(0);
	}
}
void CWndBarunaOfferSeed::OnDraw( C2DRender* p2DRender ) 
{
	ItemProp* pItemProp;
	
	if(m_pItemElem != NULL)
	{
		pItemProp = m_pItemElem->GetProp();
		LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC1 );
		if(pItemProp != NULL)
		{
			if(m_pTexture != NULL)
				m_pTexture->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
		}
	} 
	if(m_pItemElem2 != NULL)
	{
		pItemProp = m_pItemElem2->GetProp();
		LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC2 );
		if(pItemProp != NULL)
		{
			if(m_pTexture2 != NULL)
				m_pTexture2->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
		}
	} 
	if(m_pItemElem3 != NULL)
	{
		pItemProp = m_pItemElem3->GetProp();
		LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC3 );
		if(pItemProp != NULL)
		{
			if(m_pTexture3 != NULL)
				m_pTexture3->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
		}
	} 
}
void CWndBarunaOfferSeed::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	pButton->EnableWindow(FALSE);

	MoveParentCenter();
} 
BOOL CWndBarunaOfferSeed::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_BARUNA_OFFERSEED, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndBarunaOfferSeed::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndBarunaOfferSeed::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
BOOL CWndBarunaOfferSeed::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC1 );
	rect = wndCtrl->rect;
	CRect rect2;
	LPWNDCTRL wndCtrl2 = GetWndCtrl( WIDC_STATIC2 );
	rect2 = wndCtrl2->rect;
	CRect rect3;
	LPWNDCTRL wndCtrl3 = GetWndCtrl( WIDC_STATIC3 );
	rect3 = wndCtrl3->rect;

	if( rect.PtInRect( point ) )
	{		
		ItemProp* pItemProp;
		CItemElem* pTempElem;
		pTempElem  = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
		
		if( pTempElem != NULL )
		{
			pItemProp = pTempElem->GetProp();
			if( pItemProp->dwID == II_GEN_MAT_OPER )
			{
				if(m_pItemElem != NULL)
					m_pItemElem->SetExtra(0);

				m_pItemElem = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
				m_pEItemProp = m_pItemElem->GetProp();
				m_pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, m_pEItemProp->szIcon), 0xffff00ff );
				m_pItemElem->SetExtra(m_pItemElem->GetExtra()+1);
			}
			else
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_NEWSMELT_OPERCID01 ) );
		}
	}else
	if( rect2.PtInRect( point ) )
	{		
		ItemProp* pItemProp;
		CItemElem* pTempElem;
		pTempElem  = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
		
		if( pTempElem != NULL )
		{
			pItemProp = pTempElem->GetProp();
			if( pItemProp->dwID == II_GEN_MAT_CID )
			{
				if(m_pItemElem2 != NULL)
					m_pItemElem2->SetExtra(0);

				m_pItemElem2 = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
				m_pEItemProp2 = m_pItemElem2->GetProp();
				m_pTexture2 = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, m_pEItemProp2->szIcon), 0xffff00ff );
				m_pItemElem2->SetExtra(m_pItemElem2->GetExtra()+1);
			}
			else
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_NEWSMELT_OPERCID02 ) );
		}
	}else
	if( rect3.PtInRect( point ) )
	{		
		ItemProp* pItemProp;
		CItemElem* pTempElem;
		pTempElem  = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
		
		if( pTempElem != NULL )
		{
			pItemProp = pTempElem->GetProp();
			if( pItemProp->dwID == II_GEN_MAT_BARYUMMEAL )
			{
				if(m_pItemElem3 != NULL)
					m_pItemElem3->SetExtra(0);

				m_pItemElem3 = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
				m_pEItemProp3 = m_pItemElem3->GetProp();
				m_pTexture3 = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, m_pEItemProp3->szIcon), 0xffff00ff );
				m_pItemElem3->SetExtra(m_pItemElem3->GetExtra()+1);
			}
			else
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_NEWSMELT_OPERCID03 ) );
		}
	}
	if( m_pItemElem != NULL && m_pItemElem3 != NULL && m_pItemElem2 != NULL )
	{
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
		pButton->EnableWindow(TRUE);
	}
	return TRUE;
}
BOOL CWndBarunaOfferSeed::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
		case WIDC_BUTTON1:
		{
			g_DPlay.SendBaruna( 0x04, m_pItemElem2->m_dwObjId, m_pItemElem->m_dwObjId,  m_pItemElem3->m_dwObjId );
			Destroy();
			break;
		}
		case WIDC_BUTTON2:
		{
			Destroy();
			break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
#ifdef __BARUNA_PIERCING
CWndBarunaPiercing::CWndBarunaPiercing() 
{
	for( int i = 0; i < 4; i++ )
	{
		m_dwItemId[i] = NULL_ID;
		m_pTexture[i] = NULL;
		m_pItemElem[i] = NULL;
	}
}
CWndBarunaPiercing::~CWndBarunaPiercing() 
{
	for( int i = 0; i < 4; i++ )
	{
		if( m_pItemElem[i] != NULL )
			if( !g_pPlayer->m_vtInfo.IsTrading( m_pItemElem[i] ) )
				m_pItemElem[i]->SetExtra(0);

	}
}
void CWndBarunaPiercing::OnDraw( C2DRender* p2DRender ) 
{
	for( int i = 0; i < 4; i++ )
	{	
		if( m_pTexture[i] != NULL )
		{
			m_pTexture[i]->Render( p2DRender, CPoint( m_wndCtrl[i]->rect.left, m_wndCtrl[i]->rect.top ) );
		}
	}
}
void CWndBarunaPiercing::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	m_wndCtrl[0] = GetWndCtrl( WIDC_PC_BARUNAPIERCE01 ); 
	m_wndCtrl[1] = GetWndCtrl( WIDC_PC_BARUNAPIERCE02 ); 
	m_wndCtrl[2] = GetWndCtrl( WIDC_PC_BARUNAPIERCE03 ); 
	m_wndCtrl[3] = GetWndCtrl( WIDC_PC_BARUNAPIERCE04 ); 

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BT_START);
	pButton->EnableWindow(FALSE);

	MoveParentCenter();
} 
BOOL CWndBarunaPiercing::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_BARUNA_PIERCE, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndBarunaPiercing::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndBarunaPiercing::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
BOOL CWndBarunaPiercing::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	ItemProp* pItemProp = NULL;
	CItemElem* pTempElem = NULL;
	CRect rect;

	rect = m_wndCtrl[0]->rect;
	if( rect.PtInRect( point ) )
	{		
		pTempElem  = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
		if( pTempElem != NULL && pTempElem->GetPiercingSize() <= 0 )
		{
			pItemProp = pTempElem->GetProp();
			if( pItemProp->dwItemKind1 == IK1_WEAPON && pItemProp->IsBaruna() )
			{
				if(m_pItemElem[0] != NULL)
					m_pItemElem[0]->SetExtra(0);

				m_pItemElem[0] = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
				m_pTexture[0] = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
				m_pItemElem[0]->SetExtra(m_pItemElem[0]->GetExtra()+1);
				m_dwItemId[0] = m_pItemElem[0]->m_dwObjId;
			}
			else
				g_WndMng.OpenMessageBox( prj.GetText( TID_BW_PEARCING_ERROR_MSG_2 ) );
		}
		else
			g_WndMng.OpenMessageBox( prj.GetText( TID_BW_PEARCING_ERROR_MSG_3 ) );
	}

	rect = m_wndCtrl[1]->rect;
	if( rect.PtInRect( point ) )
	{		
		pTempElem  = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
		if( pTempElem != NULL )
		{
			pItemProp = pTempElem->GetProp();
			if( pItemProp->dwItemKind3 == IK3_BARUNAWEA_PIERCE_RUNE )
			{
				if(m_pItemElem[1] != NULL)
					m_pItemElem[1]->SetExtra(0);

				m_pItemElem[1] = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
				m_pTexture[1] = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
				m_pItemElem[1]->SetExtra(m_pItemElem[1]->GetExtra()+1);
				m_dwItemId[1] = m_pItemElem[1]->m_dwObjId;
			}
			else
				g_WndMng.OpenMessageBox( prj.GetText( TID_BW_PEARCING_ERROR_MSG_6 ) );
		}
	}

	rect = m_wndCtrl[2]->rect;
	if( rect.PtInRect( point ) )
	{		
		pTempElem  = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
		if( pTempElem != NULL )
		{
			pItemProp = pTempElem->GetProp();
			if( pItemProp->dwItemKind3 == IK3_BARUNA_PROTECTION_RUNE )
			{
				if(m_pItemElem[2] != NULL)
					m_pItemElem[2]->SetExtra(0);

				m_pItemElem[2] = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
				m_pTexture[2] = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
				m_pItemElem[2]->SetExtra(m_pItemElem[2]->GetExtra()+1);
				m_dwItemId[2] = m_pItemElem[2]->m_dwObjId;
			}
			else
				g_WndMng.OpenMessageBox( prj.GetText( TID_BW_PEARCING_ERROR_MSG_7 ) );
		}
	}

	rect = m_wndCtrl[3]->rect;
	if( rect.PtInRect( point ) )
	{		
		pTempElem  = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
		if( pTempElem != NULL )
		{
			pItemProp = pTempElem->GetProp();
			if( pItemProp->dwItemKind3 == IK3_BARUNA_PIERCE_RATE )
			{
				if(m_pItemElem[3] != NULL)
					m_pItemElem[3]->SetExtra(0);

				m_pItemElem[3] = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
				m_pTexture[3] = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
				m_pItemElem[3]->SetExtra(m_pItemElem[3]->GetExtra()+1);
				m_dwItemId[3] = m_pItemElem[3]->m_dwObjId;
			}
			else
				g_WndMng.OpenMessageBox( prj.GetText( TID_BW_PEARCING_ERROR_MSG_8 ) );
		}
	}
	if( m_pItemElem[0] != NULL && m_pItemElem[1] != NULL )
	{
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
		pButton->EnableWindow(TRUE);
	}
	return TRUE;
}
BOOL CWndBarunaPiercing::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
		case WIDC_BT_START:
		{
			g_DPlay.SendBaruna( 0x06, m_dwItemId[0], m_dwItemId[1], m_dwItemId[2], m_dwItemId[3] );
			Destroy();
			break;
		}
		case WIDC_BT_CANCEL:
		{
			Destroy();
			break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
#endif // __BARUNA_PIERCING

#endif // __NEW_ITEM_VARUNA
#ifdef __FASHION_COMBINE
CWndFashionCombine::CWndFashionCombine() 
{
	for( int i = 0; i < 12; i++ )
	{
		m_pItemElem[i] = NULL;
		m_pTexture[i] = NULL;
		//ZeroMemory( m_pItemElem[i], sizeof( m_pItemElem ));
		//ZeroMemory( m_pTexture[i], sizeof( m_pTexture ));
	}	
}
CWndFashionCombine::~CWndFashionCombine() 
{
	for( int i = 0; i < 8; i++ )
	{
		if( m_pItemElem[i] != NULL )
		{
			if( !g_pPlayer->m_vtInfo.IsTrading( m_pItemElem[i] ))
				m_pItemElem[i]->SetExtra(0);
		}
	}
}
void CWndFashionCombine::OnDraw( C2DRender* p2DRender ) 
{
	LPWNDCTRL wndCtrl[12];

	wndCtrl[0] = GetWndCtrl( WIDC_ST_HAT_A );
	wndCtrl[1] = GetWndCtrl( WIDC_ST_CLOTH_A );
	wndCtrl[2] = GetWndCtrl( WIDC_ST_GLOVE_A );
	wndCtrl[3] = GetWndCtrl( WIDC_ST_SHOES_A );
	wndCtrl[4] = GetWndCtrl( WIDC_ST_HAT_B );
	wndCtrl[5] = GetWndCtrl( WIDC_ST_CLOTH_B );
	wndCtrl[6] = GetWndCtrl( WIDC_ST_GLOVE_B );
	wndCtrl[7] = GetWndCtrl( WIDC_ST_SHOES_B );
	wndCtrl[8] = GetWndCtrl( WIDC_ST_RESULT_HAT );
	wndCtrl[9] = GetWndCtrl( WIDC_ST_RESULT_CLOTH );
	wndCtrl[10]= GetWndCtrl( WIDC_ST_RESULT_GLOVE );
	wndCtrl[11]= GetWndCtrl( WIDC_ST_RESULT_SHOES );

	for( int i = 0; i < 12; i++ )
	{
		if( m_pItemElem[i] != NULL && m_pTexture[i] != NULL )
		{
			m_pTexture[i]->Render( p2DRender, CPoint( wndCtrl[i]->rect.left, wndCtrl[i]->rect.top));
		}
	}
}
void CWndFashionCombine::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BT_START);
	pButton->EnableWindow(FALSE);

	MoveParentCenter();
} 
BOOL CWndFashionCombine::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_COSTUME_MIXMENU, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndFashionCombine::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndFashionCombine::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
BOOL CWndFashionCombine::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CRect rect[8];
	LPWNDCTRL wndCtrl[8];

	wndCtrl[0] = GetWndCtrl( WIDC_ST_HAT_A );
	wndCtrl[1] = GetWndCtrl( WIDC_ST_CLOTH_A );
	wndCtrl[2] = GetWndCtrl( WIDC_ST_GLOVE_A );
	wndCtrl[3] = GetWndCtrl( WIDC_ST_SHOES_A );
	wndCtrl[4] = GetWndCtrl( WIDC_ST_HAT_B );
	wndCtrl[5] = GetWndCtrl( WIDC_ST_CLOTH_B );
	wndCtrl[6] = GetWndCtrl( WIDC_ST_GLOVE_B );
	wndCtrl[7] = GetWndCtrl( WIDC_ST_SHOES_B );

	BOOL bUpdate = FALSE;

	for( int i = 0; i < 8; i++ )
	{
		rect[i] = wndCtrl[i]->rect;
		
		if( rect[i].PtInRect( point ))
		{
			CItemElem* pTempElem;
			pTempElem = (CItemElem*) g_pPlayer->GetItemId( pShortcut->m_dwId );
			if( pTempElem != NULL )
			{
				if( 0 < g_xRandomOptionProperty->GetRandomOptionSize( pTempElem->GetRandomOptItemId() ))
				{
					ItemProp* pItemProp = pTempElem->GetProp();

					if(( i == 0 || i == 4 ) && ( pItemProp->dwItemKind3 == IK3_HAT ))
					{	
						if( i == 0 )
						{
							if( m_pItemElem[4] != NULL )
							{
								int nSize = 0;
								nSize += g_xRandomOptionProperty->GetRandomOptionSize( pTempElem->GetRandomOptItemId() );
								nSize += g_xRandomOptionProperty->GetRandomOptionSize( m_pItemElem[4]->GetRandomOptItemId() );
								if( nSize > 3 )
								{
									g_WndMng.OpenMessageBox( "oversize" );
									return TRUE;
								}else
								{
									bUpdate = TRUE;
								}
							}
							if(m_pItemElem[i] != NULL)
								m_pItemElem[i]->SetExtra(0);

							m_pItemElem[i] = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
							m_pTexture[i] = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
							m_pItemElem[i]->SetExtra(m_pItemElem[i]->GetExtra()+1);

							if( bUpdate == TRUE )
							{
								m_pItemElem[8] = m_pItemElem[0];
								m_pTexture[8] = m_pTexture[0];
							//	g_xRandomOptionProperty->MergeRandomOption( m_pItemElem[8], m_pItemElem[4] );
							}
						}
						if( i == 4 )
						{
							if( m_pItemElem[0] != NULL )
							{
								int nSize = 0;
								nSize += g_xRandomOptionProperty->GetRandomOptionSize( pTempElem->GetRandomOptItemId() );
								nSize += g_xRandomOptionProperty->GetRandomOptionSize( m_pItemElem[0]->GetRandomOptItemId() );
								if( nSize > 3 )
								{
									g_WndMng.OpenMessageBox( "oversize" );
									return TRUE;
								}else
								{
									bUpdate = TRUE;
								}
							}
							if(m_pItemElem[i] != NULL)
								m_pItemElem[i]->SetExtra(0);

							m_pItemElem[i] = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
							m_pTexture[i] = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
							m_pItemElem[i]->SetExtra(m_pItemElem[i]->GetExtra()+1);

							if( bUpdate == TRUE )
							{
								m_pItemElem[8] = m_pItemElem[0];
								m_pTexture[8] = m_pTexture[0];
							//	g_xRandomOptionProperty->MergeRandomOption( m_pItemElem[8], m_pItemElem[4] );
							}
						}
					}else
					if(( i == 1 || i == 5 ) && ( pItemProp->dwItemKind3 == IK3_CLOTH ))
					{
						if( i == 1 )
						{
							if( m_pItemElem[5] != NULL )
							{
								int nSize = 0;
								nSize += g_xRandomOptionProperty->GetRandomOptionSize( pTempElem->GetRandomOptItemId() );
								nSize += g_xRandomOptionProperty->GetRandomOptionSize( m_pItemElem[5]->GetRandomOptItemId() );
								if( nSize > 3 )
								{
									g_WndMng.OpenMessageBox( "oversize" );
									return TRUE;
								}else
								{
									bUpdate = TRUE;
								}
							}
							if(m_pItemElem[i] != NULL)
								m_pItemElem[i]->SetExtra(0);

							m_pItemElem[i] = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
							m_pTexture[i] = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
							m_pItemElem[i]->SetExtra(m_pItemElem[i]->GetExtra()+1);

							if( bUpdate == TRUE )
							{
								m_pItemElem[9] = m_pItemElem[1];
								m_pTexture[9] = m_pTexture[1];

//								g_xRandomOptionProperty->MergeRandomOption( m_pItemElem[9], m_pItemElem[5] );
							}
						}
						if( i == 5 )
						{
							if( m_pItemElem[1] != NULL )
							{
								int nSize = 0;
								nSize += g_xRandomOptionProperty->GetRandomOptionSize( pTempElem->GetRandomOptItemId() );
								nSize += g_xRandomOptionProperty->GetRandomOptionSize( m_pItemElem[1]->GetRandomOptItemId() );
								if( nSize > 3 )
								{
									g_WndMng.OpenMessageBox( "oversize" );
									return TRUE;
								}else
								{
									bUpdate = TRUE;
								}
							}
							if(m_pItemElem[i] != NULL)
								m_pItemElem[i]->SetExtra(0);

							m_pItemElem[i] = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
							m_pTexture[i] = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
							m_pItemElem[i]->SetExtra(m_pItemElem[i]->GetExtra()+1);

							if( bUpdate == TRUE )
							{
								m_pItemElem[9] = m_pItemElem[1];
								m_pTexture[9] = m_pTexture[1];
								//g_xRandomOptionProperty->MergeRandomOption( m_pItemElem[9], m_pItemElem[5] );
							}
						}
					}else
					if(( i == 2 || i == 6 ) && ( pItemProp->dwItemKind3 == IK3_GLOVE ))
					{
						if( i == 2 )
						{
							if( m_pItemElem[6] != NULL )
							{
								int nSize = 0;
								nSize += g_xRandomOptionProperty->GetRandomOptionSize( pTempElem->GetRandomOptItemId() );
								nSize += g_xRandomOptionProperty->GetRandomOptionSize( m_pItemElem[6]->GetRandomOptItemId() );
								if( nSize > 3 )
								{
									g_WndMng.OpenMessageBox( "oversize" );
									return TRUE;
								}else
								{
									bUpdate = TRUE;
								}
							}
							if(m_pItemElem[i] != NULL)
								m_pItemElem[i]->SetExtra(0);

							m_pItemElem[i] = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
							m_pTexture[i] = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
							m_pItemElem[i]->SetExtra(m_pItemElem[i]->GetExtra()+1);

							if( bUpdate == TRUE )
							{
								m_pItemElem[10] = m_pItemElem[2];
								m_pTexture[10] = m_pTexture[2];
							//	g_xRandomOptionProperty->MergeRandomOption( m_pItemElem[10], m_pItemElem[6] );
							}
						}
						if( i == 6 )
						{
							if( m_pItemElem[2] != NULL )
							{
								int nSize = 0;
								nSize += g_xRandomOptionProperty->GetRandomOptionSize( pTempElem->GetRandomOptItemId() );
								nSize += g_xRandomOptionProperty->GetRandomOptionSize( m_pItemElem[2]->GetRandomOptItemId() );
								if( nSize > 3 )
								{
									g_WndMng.OpenMessageBox( "oversize" );
									return TRUE;
								}else
								{
									bUpdate = TRUE;
								}
							}
							if(m_pItemElem[i] != NULL)
								m_pItemElem[i]->SetExtra(0);

							m_pItemElem[i] = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
							m_pTexture[i] = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
							m_pItemElem[i]->SetExtra(m_pItemElem[i]->GetExtra()+1);

							if( bUpdate == TRUE )
							{
								m_pItemElem[10] = m_pItemElem[2];
								m_pTexture[10] = m_pTexture[2];
							//	g_xRandomOptionProperty->MergeRandomOption( m_pItemElem[10], m_pItemElem[6] );
							}
						}
					}else
					if(( i == 3 || i == 7 ) && ( pItemProp->dwItemKind3 == IK3_SHOES ))
					{
						if( i == 3 )
						{
							if( m_pItemElem[7] != NULL )
							{
								int nSize = 0;
								nSize += g_xRandomOptionProperty->GetRandomOptionSize( pTempElem->GetRandomOptItemId() );
								nSize += g_xRandomOptionProperty->GetRandomOptionSize( m_pItemElem[7]->GetRandomOptItemId() );
								if( nSize > 3 )
								{
									g_WndMng.OpenMessageBox( "oversize" );
									return TRUE;
								}else
								{
									bUpdate = TRUE;
								}
							}
							if(m_pItemElem[i] != NULL)
								m_pItemElem[i]->SetExtra(0);

							m_pItemElem[i] = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
							m_pTexture[i] = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
							m_pItemElem[i]->SetExtra(m_pItemElem[i]->GetExtra()+1);

							if( bUpdate == TRUE )
							{
								m_pItemElem[11] = m_pItemElem[3];
								m_pTexture[11] = m_pTexture[3];
							//	g_xRandomOptionProperty->MergeRandomOption( m_pItemElem[11], m_pItemElem[7] );
							}
						}
						if( i == 7 )
						{
							if( m_pItemElem[3] != NULL )
							{
								int nSize = 0;
								nSize += g_xRandomOptionProperty->GetRandomOptionSize( pTempElem->GetRandomOptItemId() );
								nSize += g_xRandomOptionProperty->GetRandomOptionSize( m_pItemElem[3]->GetRandomOptItemId() );
								if( nSize > 3 )
								{
									g_WndMng.OpenMessageBox( "oversize" );
									return TRUE;
								}else
								{
									bUpdate = TRUE;
								}
							}
							if(m_pItemElem[i] != NULL)
								m_pItemElem[i]->SetExtra(0);

							m_pItemElem[i] = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
							m_pTexture[i] = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
							m_pItemElem[i]->SetExtra(m_pItemElem[i]->GetExtra()+1);

							if( bUpdate == TRUE )
							{
								m_pItemElem[11] = m_pItemElem[3];
								m_pTexture[11] = m_pTexture[3];
							//	g_xRandomOptionProperty->MergeRandomOption( m_pItemElem[11], m_pItemElem[7] );
							}
						}
					}else
					{
						g_WndMng.OpenMessageBox( prj.GetText( TID_COSTUME_REGISTERNOTSLOT ) );
						// wrong tab
					}
				}else
				{
					g_WndMng.OpenMessageBox( prj.GetText( TID_COSTUME_REGISTERNOTSLOT ) );
					// not blessed
				}
			}
		}
	}

	int nCheck = 0;
	for( int i = 0; i < 8; i++ )
	{
		if( m_pItemElem[i] != NULL )
			nCheck++;
	}
	if( 8 <= nCheck )
	{
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BT_START);
		pButton->EnableWindow(TRUE);			
	}
	return TRUE;
}
BOOL CWndFashionCombine::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
		case WIDC_BUTTON1:
		{
			g_DPlay.SendCombineFashion( 
				m_pItemElem[0]->m_dwObjId, m_pItemElem[4]->m_dwObjId,
				m_pItemElem[1]->m_dwObjId, m_pItemElem[5]->m_dwObjId,
				m_pItemElem[2]->m_dwObjId, m_pItemElem[6]->m_dwObjId,
				m_pItemElem[3]->m_dwObjId, m_pItemElem[7]->m_dwObjId				
				);
			Destroy();
			break;
		}
		case WIDC_BUTTON2:
		{
			Destroy();
			break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
#endif // __FASHION_COMBINE
#ifdef __COLOSSEUM
#endif // __COLOSSEUM

#ifdef __TREASUREBOX
/***************************************
*
*	APP_TREASURECHEST_OPEN
*
****************************************/

CWndTreasureBox::CWndTreasureBox( DWORD dwObjID )
{
	m_nDelay = 2; //Sekunden die der Balken braucht
	m_fGaugeRate = 0;

	m_pItemElem = NULL;
	m_pBoxItem = NULL;

	m_nSmeltWidth = -1;
	m_pVBSmeltGauge = NULL;
	m_bVBSmeltGauge = FALSE;
	m_bStart = FALSE;
	m_dwStartTime = m_dwEndTime = 0xffffffff;

	m_pBoxItem = g_pPlayer->m_Inventory.GetAtId( dwObjID );
	if( m_pBoxItem == NULL )
		Destroy();
	m_pBoxItem->SetExtra( 1 );
}

CWndTreasureBox::~CWndTreasureBox()
{
	DeleteDeviceObjects();
}

HRESULT CWndTreasureBox::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();

	m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBSmeltGauge, NULL );
	m_nSmeltWidth = -1;
	m_texGauEmptyNormal.SetInvalidate(m_pApp->m_pd3dDevice); 

	return S_OK;
}

HRESULT CWndTreasureBox::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	SAFE_RELEASE( m_pVBSmeltGauge );
	m_texGauEmptyNormal.Invalidate(); 
	return S_OK;
}

HRESULT CWndTreasureBox::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	return InvalidateDeviceObjects();
}

BOOL CWndTreasureBox::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CRect rect;
	CItemElem* pItemElem;
	pItemElem = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );

	if(g_pPlayer->IsUsing(pItemElem))
	{
		g_WndMng.PutString( "Item wird bereits verwendet" );
	}
	else
	{
		if( pItemElem->GetProp()->dwItemKind3 == IK3_BOXOPEN && IsOpenKey( pItemElem ) )	
		{
			if( /*m_nCtrlId[0].PtInRect( point ) &&*/ m_pItemElem == NULL )
			{
				m_pItemElem = pItemElem;
				m_pItemElem->SetExtra( 1 );
			}
		}
	}
	if( m_pItemElem != NULL )
	{
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BT_OK);
		pButton->EnableWindow(TRUE);
	}
	return TRUE;
}

BOOL CWndTreasureBox::Initialize( CWndBase* pWndParent, DWORD nType )
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_TREASURECHEST_OPEN, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndTreasureBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( nID == WIDC_BT_OK )
	{
		if( m_bStart )
			return TRUE;

		m_bStart = TRUE;
		m_dwStartTime = g_tmCurrent;
		m_dwEndTime = g_tmCurrent + SEC(m_nDelay);
	}
	else if( nID == WIDC_BUTTON2 )
	{
		if( m_bStart )
		{
			m_bStart = FALSE;
		}
		else
			Destroy();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

BOOL CWndTreasureBox::Process()
{
	if(m_bStart && m_pItemElem != NULL)
	{
		m_fGaugeRate = static_cast<float>(g_tmCurrent - m_dwStartTime) / static_cast<float>(m_dwEndTime - m_dwStartTime);
		
		if(m_dwEndTime < g_tmCurrent)
		{
			m_bStart = FALSE;
			m_dwStartTime = 0xffffffff;
			m_dwEndTime = 0xffffffff;

			// Send to Server...
			m_pBoxItem->SetExtra( 0 );
			m_pItemElem->SetExtra( 0 );
			g_DPlay.SendTreasureBoxOpen( m_pBoxItem, m_pItemElem );
			Destroy();
		}
	}
	else
	{
		m_dwStartTime = 0xffffffff;
		m_dwEndTime = 0xffffffff;
	}

	return TRUE;
}

void CWndTreasureBox::OnDraw( C2DRender* p2DRender )
{

////////////////////////////////
	if( m_bStart )
	{
		static CRect rectStaticTemp;
		LPWNDCTRL lpStatic = GetWndCtrl( WIDC_PC_DELAY );
		rectStaticTemp.TopLeft().y = lpStatic->rect.top+2;
		rectStaticTemp.TopLeft().x = lpStatic->rect.left+2;
		rectStaticTemp.BottomRight().y = lpStatic->rect.bottom;
		int nGaugeWidth(lpStatic->rect.left + static_cast<int>(static_cast<float>(lpStatic->rect.right - lpStatic->rect.left) * m_fGaugeRate));
		nGaugeWidth = nGaugeWidth < lpStatic->rect.right ? nGaugeWidth : lpStatic->rect.right;
		rectStaticTemp.BottomRight().x = nGaugeWidth;
		assert(m_pVBSmeltGauge != NULL);
		m_pTheme->RenderGauge(p2DRender, &rectStaticTemp, 0xffffffff, m_pVBSmeltGauge, &m_texGauEmptyNormal);
	}
/////////////////////////////////////

	if( m_pBoxItem != NULL )
	{
		m_pBoxItem->GetTexture()->Render( p2DRender, m_nBoxRect.TopLeft(), 255 );

		CRect hitrect2 = m_nBoxRect;
		CPoint point3 = GetMousePoint();
		if( m_nBoxRect.PtInRect( point3 ) )
		{
			CPoint point4 = point3;
			ClientToScreen( &point4 );
			ClientToScreen( &hitrect2 );
			
			g_WndMng.PutToolTip_Item( (CItemBase*)m_pBoxItem, point4, &hitrect2 );
		}
	}

	if( m_pItemElem != NULL )
	{
		m_pItemElem->GetTexture()->Render( p2DRender, m_nCtrlId.TopLeft(), 255 );

		if( m_pItemElem->m_nItemNum > 1 )
		{
			TCHAR szTemp[32];
			_stprintf( szTemp, "%d", m_pItemElem->GetExtra() );
			CSize size	= p2DRender->m_pFont->GetTextExtent( szTemp );
			p2DRender->TextOut( m_nCtrlId.right-11,  m_nCtrlId.bottom-11 , szTemp, 0xff1010ff );
		}

		CRect hitrect = m_nCtrlId;
		CPoint point = GetMousePoint();
		if( m_nCtrlId.PtInRect( point ) )
		{
			CPoint point2 = point;
			ClientToScreen( &point2 );
			ClientToScreen( &hitrect );
			
			g_WndMng.PutToolTip_Item( (CItemBase*)m_pItemElem, point2, &hitrect );
		}
	}

}


void CWndTreasureBox::OnInitialUpdate()
{ 
	CWndNeuz::OnInitialUpdate();
	
	RestoreDeviceObjects();

	m_texGauEmptyNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ColoGauge01.bmp" ), 0xffff00ff, TRUE );

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BT_OK);
	pButton->EnableWindow(FALSE);

	CWndCustom* pWndCustom = (CWndCustom*)GetDlgItem(WIDC_PC_DELAY);
	pWndCustom->SetTitle( "" );

	m_nCtrlId = GetWndCtrl( WIDC_CB_OPEN )->rect;
	m_nBoxRect = GetWndCtrl( WIDC_LB_NOTICE )->rect;

	MoveParentCenter();
} 

void CWndTreasureBox::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	if(m_nCtrlId.PtInRect( point ))
	{
		if(m_pItemElem)
		{
			m_pItemElem->SetExtra(0);
			m_pItemElem = NULL;
			CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BT_OK);
			pButton->EnableWindow(FALSE);
		}
	}
}

void CWndTreasureBox::OnDestroy( void )
{
	if(m_pItemElem != NULL)
		m_pItemElem->SetExtra(0);
	if( m_pBoxItem != NULL )
		m_pBoxItem->SetExtra( 0 );
}

BOOL CWndTreasureBox::IsOpenKey( CItemElem* pKey )
{
	for( int i=0;i<(int)prj.m_vTreasureBox.size();i++ )
	{
		if( m_pBoxItem->GetProp()->dwID == prj.m_vTreasureBox.at(i).dwBoxID )
		{
			for( int j=0;j<3;j++ )
			{
				if( prj.m_vTreasureBox.at(i).dwKeyID[j] == pKey->GetProp()->dwID )
					return TRUE;
			}
			return FALSE;
		}
	}
	return FALSE;
}

/***************************************
*
*	APP_TREASURECHEST_RESULT
*	WIDC_CB_OPEN03	WIDC_CB_OPEN02	WIDC_CB_OPEN	WIDC_BT_CANCEL
****************************************/
CWndTreasureRecieve::CWndTreasureRecieve()
{
}

CWndTreasureRecieve::~CWndTreasureRecieve()
{
}
void CWndTreasureRecieve::InsertItem( DWORD dwID, int idx, int nCount )
{
	if( idx > 2 || idx < 0 )
		return;
	m_dwTreasures[idx] = dwID;
	if( dwID != NULL )
		m_nCount[idx] = nCount;
	else
		m_nCount[idx] = NULL;
}

BOOL CWndTreasureRecieve::Initialize( CWndBase* pWndParent, DWORD nType )
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_TREASURECHEST_RESULT, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndTreasureRecieve::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( nID == WIDC_BT_CANCEL )
	{
		Destroy();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndTreasureRecieve::OnDraw( C2DRender* p2DRender )
{
	for( int i=0;i<3;i++ )
	{
		if(pTexture[i] != NULL)
			pTexture[i]->Render( p2DRender, m_nCtrlId[i].TopLeft(), 255 );

		if( m_nCount[i] > 1 )
		{
			TCHAR szTemp[32];
			_stprintf( szTemp, "%d", m_nCount[i] );
			CSize size	= p2DRender->m_pFont->GetTextExtent( szTemp );
			p2DRender->TextOut( m_nCtrlId[i].right-11,  m_nCtrlId[i].bottom-11 , szTemp, 0xff1010ff );
		}

		CRect hitrect = m_nCtrlId[i];
		CPoint point = GetMousePoint();
		if( m_nCtrlId[i].PtInRect( point ) )
		{
			CPoint point2 = point;
			ClientToScreen( &point2 );
			ClientToScreen( &hitrect );
			
			CItemElem pElem;
			pElem.m_dwItemId = m_dwTreasures[i];
			pElem.m_nHitPoint	= 0;

			g_WndMng.PutToolTip_Item( &pElem, point2, &hitrect );
		}
	}
}


void CWndTreasureRecieve::OnInitialUpdate()
{ 
	CWndNeuz::OnInitialUpdate(); 

	for( int i=0;i<3;i++ )
	{
		if( m_dwTreasures[i] == NULL )
		{
			CString strPath = MakePath( DIR_ITEM, "Itm_TreasureBox01.dds");
			pTexture[i] = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, strPath, 0xffff00ff );
		}
		else
		{
			CString strPath = MakePath( DIR_ITEM, prj.GetItemProp( m_dwTreasures[i] )->szIcon );
			pTexture[i] = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, strPath, 0xffff00ff );
		}
	}

	m_nCtrlId[0] = GetWndCtrl( WIDC_CB_OPEN03 )->rect;
	m_nCtrlId[1] = GetWndCtrl( WIDC_CB_OPEN02 )->rect;
	m_nCtrlId[2] = GetWndCtrl( WIDC_CB_OPEN )->rect;

	MoveParentCenter();
} 

#endif//__TREASUREBOX
