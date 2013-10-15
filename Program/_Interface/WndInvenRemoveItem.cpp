#include "stdafx.h"
#include "resData.h"
#include "WndInvenRemoveItem.h"
#include "defineText.h"
#include "DPClient.h"
extern	CDPClient	g_DPlay;


/****************************************************
  WndId : APP_INVEN_REMOVE_ITEM - 아이템 삭제 확인
  CtrlId : WIDC_STATIC1 - 아이템을 삭제하시겠습니까?
  CtrlId : WIDC_CUSTOM1 - Custom
  CtrlId : WIDC_STATIC2 - 개 수
  CtrlId : WIDC_EDIT1 - Edit
  CtrlId : WIDC_OK - Button
  CtrlId : WIDC_CANCEL - Button
****************************************************/

CWndInvenRemoveItem::CWndInvenRemoveItem() 
{ 
	m_pWndItemCtrl	= NULL;
	m_pWndEditNum		= NULL;
	m_pItemElem		= NULL;
	m_nRemoveNum = 0;
} 
CWndInvenRemoveItem::~CWndInvenRemoveItem() 
{ 
} 
void CWndInvenRemoveItem::InitItem( CItemElem* pItemElem )
{
	m_pItemElem = pItemElem;
	int nItemNum = pItemElem->m_nItemNum;

	char szItemNum[10] = {0,};
	_itoa( pItemElem->m_nItemNum , szItemNum, 10 );
	m_pWndEditNum->SetString( szItemNum );
	
	if( nItemNum <= 1 )
		m_pWndEditNum->EnableWindow( FALSE );
	else
		m_pWndEditNum->SetFocus();

	m_nRemoveNum = 0;	
}
void CWndInvenRemoveItem::OnDraw( C2DRender* p2DRender ) 
{ 
	if( m_pItemElem )
		m_pItemElem->GetTexture()->Render( p2DRender, m_pWndItemCtrl->rect.TopLeft(), 255 );
} 
void CWndInvenRemoveItem::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	m_pWndItemCtrl = GetWndCtrl( WIDC_CUSTOM1 );
	m_pWndEditNum = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	
	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndInvenRemoveItem::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_INVEN_REMOVE_ITEM, WBS_MODAL, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndInvenRemoveItem::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndInvenRemoveItem::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndInvenRemoveItem::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndInvenRemoveItem::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndInvenRemoveItem::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
void CWndInvenRemoveItem::OnMouseWndSurface( CPoint point )
{
	if( m_pItemElem && m_pWndItemCtrl->rect.PtInRect( point ) )
	{
		CPoint point2 = point;
		CRect  DrawRect = m_pWndItemCtrl->rect;
		ClientToScreen( &point2 );
		ClientToScreen( &DrawRect );
		g_WndMng.PutToolTip_Item( m_pItemElem, point2, &DrawRect, APP_INVEN_REMOVE_ITEM );
	}
}

BOOL CWndInvenRemoveItem::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case WIDC_CANCEL:
		Destroy();
		break;
		
	case WIDC_OK:
		{
			if( OnButtonOK() )
			{
				// 100304_mirchang vendor item check
				for( int i = 0; i < MAX_VENDITEM; i++ )
				{
					if( g_Neuz.m_aSavedInven[i].m_dwObjId == m_pItemElem->m_dwObjId && g_Neuz.m_aSavedInven[i].m_dwItemId == m_pItemElem->m_dwItemId )
					{
						g_Neuz.m_aSavedInven[i].m_dwObjId = 0;
						g_Neuz.m_aSavedInven[i].m_nCost = 0;
						g_Neuz.m_aSavedInven[i].m_nExtra = 0;
						g_Neuz.m_aSavedInven[i].m_dwItemId = 0;
						break;
					}
				}
				g_DPlay.SendRemoveItem( m_pItemElem, m_nRemoveNum );
				Destroy();
			}
			else
			{
				g_WndMng.PutString( prj.GetText(TID_ERROR_NOT_ITEM_NUM), NULL, prj.GetTextColor(TID_ERROR_NOT_ITEM_NUM) );
			}
		}		
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
BOOL CWndInvenRemoveItem::OnButtonOK( void )
{
	LPCTSTR szNumber = m_pWndEditNum->GetString();
	BOOL bOK = TRUE;

	// 숫자 검사
	int nlen = strlen(szNumber);
	for( int i = 0 ; i < nlen ; i++ )
	{
		if( (isdigit2( szNumber[i] ) == FALSE) )
		{
			bOK = FALSE;
			break;
		}
	}
	
	// 삭제할수 개수 인지 검사
	if( bOK )
	{
		int nNum = atoi( m_pWndEditNum->GetString() );
		if( 0 < nNum && nNum <= m_pItemElem->m_nItemNum )
			m_nRemoveNum = nNum;
		else
			bOK = FALSE;
	}

	if( bOK == FALSE )
	{
		char szItemNum[10] = {0,};
		_itoa( m_pItemElem->m_nItemNum , szItemNum, 10 );
		m_pWndEditNum->SetString( szItemNum );
	}
	
	return bOK;
}

