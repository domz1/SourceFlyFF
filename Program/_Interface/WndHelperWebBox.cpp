#include "StdAfx.h"
#ifdef __NEW_WEB_BOX
#ifdef __CLIENT
#include "WndHelperWebBox.h"
#include "HelperWebBox.h"
#include "resdata.h"
#include "defineText.h"
#include "WebCtrl.h"

//-----------------------------------------------------------------------------
CWndHelperWebBox::CWndHelperWebBox( void )
{
	SetPutRegInfo( FALSE );
}
//-----------------------------------------------------------------------------
CWndHelperWebBox::~CWndHelperWebBox( void )
{
	Release();
}
//-----------------------------------------------------------------------------
BOOL CWndHelperWebBox::Initialize( CWndBase* pWndParent, DWORD nType )
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_WEBBOX2, WBS_MODAL, CPoint( 0, 0 ), pWndParent );
}
//-----------------------------------------------------------------------------
void CWndHelperWebBox::OnInitialUpdate( void )
{
	CWndNeuz::OnInitialUpdate();

	SetTitle( prj.GetText( TID_GAME_HELPER_WEB_BOX_ICON_TITLE ) );

	if( g_Option.m_nResWidth >= 1024 && g_Option.m_nResHeight >= 768 )
	{
		CRect rectHelperWebBox = GetWndRect();
		rectHelperWebBox.right = 1024;
		rectHelperWebBox.bottom = 768;
		SetWndRect( rectHelperWebBox );
	}

	DelWndStyle( WBS_MOVE );
	AddWndStyle( WBS_TOPMOST );

	CHelperWebBox::GetInstance()->Initialize();

	MoveParentCenter();
}
//-----------------------------------------------------------------------------
BOOL CWndHelperWebBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case WTBID_CLOSE:
		{
			Release();
			Destroy();
			return TRUE;
		}
	//case WIDC_BUTTON_GO_BACK:
	//	{
	//		CHelperWebBox* pHelperWebBox = CHelperWebBox::GetInstance();
	//		if( pHelperWebBox && pHelperWebBox->GetDialogBoxHandle() )
	//		{
	//			::SendMessage( pHelperWebBox->GetDialogBoxHandle(), WEB_CTRL_GO_BACK, 0, 0 );
	//		}
	//		break;
	//	}
	//case WIDC_BUTTON_GO_FORWARD:
	//	{
	//		CHelperWebBox* pHelperWebBox = CHelperWebBox::GetInstance();
	//		if( pHelperWebBox && pHelperWebBox->GetDialogBoxHandle() )
	//		{
	//			::SendMessage( pHelperWebBox->GetDialogBoxHandle(), WEB_CTRL_GO_FORWARD, 0, 0 );
	//		}
	//		break;
	//	}
	//case WIDC_BUTTON_GO_FIRST_PAGE:
	//	{
	//		CHelperWebBox* pHelperWebBox = CHelperWebBox::GetInstance();
	//		if( pHelperWebBox && pHelperWebBox->GetDialogBoxHandle() )
	//		{
	//			pHelperWebBox->GoFirstPage();
	//		}
	//		break;
	//	}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
//-----------------------------------------------------------------------------
BOOL CWndHelperWebBox::Process( void )
{
	CHelperWebBox::GetInstance()->Process();

	return TRUE;
}
//-----------------------------------------------------------------------------
HRESULT CWndHelperWebBox::RestoreDeviceObjects( void )
{
	CHelperWebBox* pHelperWebBox = CHelperWebBox::GetInstance();

	if( pHelperWebBox && pHelperWebBox->GetDialogHandle() )
		::SendMessage( pHelperWebBox->GetDialogHandle(), WM_INITDIALOG, 0, 0 );

	return CWndNeuz::RestoreDeviceObjects();
}
//-----------------------------------------------------------------------------
void CWndHelperWebBox::Release( void )
{
	ShowWindow( CHelperWebBox::GetInstance()->GetDialogHandle(), SW_HIDE );
	CHelperWebBox::GetInstance()->Close();
}
//-----------------------------------------------------------------------------

#endif // __CLIENT
#endif // __NEW_WEB_BOX