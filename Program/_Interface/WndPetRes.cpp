

#include "stdafx.h"
#include "resData.h"
#include "WndPetRes.h"
#include "DPClient.h"
#include "Item.h"
#include "DefineText.h"


#ifdef __JEFF_11

extern CDPClient g_DPlay;

/****************************************************
  WndId : APP_PET_RES - 교환창
  CtrlId : WIDC_OK - 
  CtrlId : WIDC_CANCEL - 
  CtrlId : WIDC_DESC - 
  CtrlId : WIDC_CHANGE - 
****************************************************/

CWndPetRes::CWndPetRes() 
{ 
	m_pItemElem  = NULL;
	m_pEItemProp = NULL;
	m_pTexture	 = NULL;
} 
CWndPetRes::~CWndPetRes() 
{ 
} 
void CWndPetRes::OnDraw( C2DRender* p2DRender ) 
{ 
	ItemProp* pItemProp;
	
	if(m_pItemElem != NULL)
	{
		pItemProp = m_pItemElem->GetProp();
		LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_CHANGE );
		if(pItemProp != NULL)
		{
			if(m_pTexture != NULL)
			m_pTexture->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
		}
	} 
} 
void CWndPetRes::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	CWndText* pDesc = (CWndText*)GetDlgItem( WIDC_DESC );
	pDesc->m_string.AddParsingString(prj.GetText(TID_GAME_PETTRADE));
	pDesc->ResetString();	
	
	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndPetRes::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_PET_RES, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndPetRes::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/

void CWndPetRes::OnDestroy()
{
	if(m_pItemElem != NULL)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pItemElem ) )
			m_pItemElem->SetExtra(0);
	}
}

BOOL CWndPetRes::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndPetRes::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndPetRes::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndPetRes::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndPetRes::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 

	if( nID == WIDC_OK )
	{
		if(m_pItemElem != NULL)
		{
			CWndButton* pButton;
			pButton = (CWndButton*)GetDlgItem( WIDC_OK );
			pButton->EnableWindow(FALSE);

			// 서버에 처리 요청하는 함수 호출
			if(m_pItemElem)
			{
				g_DPlay.SendQuePetResurrection(m_pItemElem->m_dwObjId);
				Destroy();
			}
		}
	}
	else if(nID == WIDC_CANCEL)
	{
		Destroy();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndPetRes::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	if(!m_pItemElem) return;
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_CHANGE );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ))
	{
		m_pItemElem->SetExtra(0);
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_OK);
		pButton->EnableWindow(FALSE);
		m_pItemElem = NULL;
		m_pEItemProp = NULL;
		m_pTexture	= NULL;
	}
}

BOOL CWndPetRes::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CItemElem* pTempElem;
	pTempElem  = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );

	if(!pTempElem) return FALSE;
	// 올릴 수 있는 아이템 거르기
	CPet* pPet	= pTempElem->m_pPet;
	
	if(!pPet)
	{
		g_WndMng.PutString( prj.GetText(TID_GAME_PETTRADE_ERROR), NULL, 0xffff0000 );
		return FALSE;
	}

	BYTE nLevel		= pPet->GetLevel();
	if((g_pPlayer->GetPetId() == pTempElem->m_dwObjId) || (!IsUsableItem( pTempElem ) )
		|| nLevel < PL_B || nLevel > PL_S )
	{
		g_WndMng.PutString( prj.GetText(TID_GAME_PETTRADE_ERROR), NULL, 0xffff0000 );
		return FALSE;
	}

	if( pTempElem->IsFlag( CItemElem::expired ) )
	{
		g_WndMng.PutString( prj.GetText(TID_GAME_PETTRADE_ERROR), NULL, 0xffff0000 );
		return FALSE;
	}
	
	if(m_pItemElem) m_pItemElem->SetExtra(0);
	m_pItemElem = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
	m_pEItemProp = m_pItemElem->GetProp();
	m_pItemElem->SetExtra(m_pItemElem->GetExtra()+1);
	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_OK);
	pButton->EnableWindow(TRUE);
	
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_CHANGE );
	if(m_pEItemProp != NULL)
	{
		m_pTexture = m_pItemElem->m_pTexture;
		//m_pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, m_pEItemProp->szIcon), 0xffff00ff );
	}	

	return TRUE;

}

#endif
