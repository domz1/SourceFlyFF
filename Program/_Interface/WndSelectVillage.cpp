#include "stdafx.h"
#include "resData.h"
#include "WndSelectVillage.h"
#include "dpclient.h"
#include "definetext.h"

extern		CDPClient		g_DPlay;




///////////////////////////////////////////////////////////////////////////
// CReturnScrollMsgBox
//////////////////////////////////////////////////////////////////////////////


BOOL CReturnScrollMsgBox::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	// 아이템 사용 후 4시간 안에 저장된 위치로 돌아오지 않으면 아이템의 효력은 
	//사라집니다. 아이템을 사용하시겠습니까?
	return CWndMessageBox::Initialize( prj.GetText(TID_GAME_RETURN_USEITEM), 
	                                   pWndParent, 
									   MB_OKCANCEL );

}

BOOL CReturnScrollMsgBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
 	switch( nID )
	{
	case IDOK:
		{
			SAFE_DELETE( g_WndMng.m_pWndSelectVillage );
			g_WndMng.m_pWndSelectVillage		= new CWndSelectVillage;
			g_WndMng.m_pWndSelectVillage->m_dwItemId = GetItemID();
			g_WndMng.m_pWndSelectVillage->Initialize( &g_WndMng, 0 );
			Destroy();
		}
		break;
	case IDCANCEL:
		Destroy();
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );  //??
} 


/*
  WndId : APP_COMM_SELECTVIL
  CtrlId : WIDC_STATIC1
  CtrlId : WIDC_OK 
  CtrlId : WIDC_CANCEL 
  CtrlId : WIDC_EDIT_CHANGENAME 
*/

CWndSelectVillage::CWndSelectVillage() 
{ 
	m_dwItemId = 0;
} 

CWndSelectVillage::~CWndSelectVillage() 
{ 

} 

void CWndSelectVillage::OnDraw( C2DRender* p2DRender ) 
{ 

} 

void CWndSelectVillage::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndButton* pWndButton[ 3 ];
	
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_RADIO_FLARIS );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_RADIO_SAINTM );
	pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_RADIO_DARKON );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ 0 ]->SetCheck( TRUE );

	MoveParentCenter();
} 

BOOL CWndSelectVillage::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_COMM_SELECTVIL, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndSelectVillage::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndSelectVillage::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndSelectVillage::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 

} 

void CWndSelectVillage::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 

} 


BOOL CWndSelectVillage::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BUTTON_OK )
	{
		UINT nRadios[3] = { WIDC_RADIO_FLARIS, WIDC_RADIO_SAINTM, WIDC_RADIO_DARKON };
		for( int i=0; i<3; ++i )
		{
			CWndButton* pButton = (CWndButton* )GetDlgItem( nRadios[i] );
			if( pButton->GetCheck() )
				break;
		}
		g_DPlay.SendReturnScroll( i );
		Destroy();
	}
	else if( nID == WIDC_BUTTON_CANCEL )
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

