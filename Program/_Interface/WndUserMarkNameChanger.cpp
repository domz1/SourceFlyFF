#include "StdAfx.h"
#ifdef __IMPROVE_MAP_SYSTEM
#ifdef __CLIENT
#include "WndUserMarkNameChanger.h"
#include "ResData.h"
//-----------------------------------------------------------------------------
CWndUserMarkNameChanger::CWndUserMarkNameChanger( void ) : 
m_dwUserMarkPositionInfoID( 0 )
{
}
//-----------------------------------------------------------------------------
CWndUserMarkNameChanger::~CWndUserMarkNameChanger( void )
{
}
//-----------------------------------------------------------------------------
BOOL CWndUserMarkNameChanger::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_USER_MARK_NAME_CHANGER, WBS_MODAL | WBS_KEY, CPoint( 0, 0 ), pWndParent );
}
//-----------------------------------------------------------------------------
void CWndUserMarkNameChanger::OnInitialUpdate( void )
{
	CWndNeuz::OnInitialUpdate();

	CWndEdit* pWndEditNameChanger = ( CWndEdit* )GetDlgItem( WIDC_EDIT_NAME_CHANGER );
	if( pWndEditNameChanger )
	{
		pWndEditNameChanger->SetMaxStringNumber( USER_MARK_NAME_MAX_LENGTH );
		pWndEditNameChanger->SetFocus();
	}

	CWndButton* pWndButtonOK = ( CWndButton* )GetDlgItem( WIDC_BUTTON_OK );
	if( pWndButtonOK )
	{
		pWndButtonOK->SetDefault( TRUE );
	}

	MoveParentCenter();
}
//-----------------------------------------------------------------------------
BOOL CWndUserMarkNameChanger::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case WIDC_BUTTON_OK:
		{
			CWndEdit* pWndEditNameChanger = ( CWndEdit* )GetDlgItem( WIDC_EDIT_NAME_CHANGER );
			if( pWndEditNameChanger == NULL )
			{
				break;
			}

			CUserMarkPositionInfo* pUserMarkPositionInfo = prj.m_MapInformationManager.FindUserMarkPositionInfo( m_dwUserMarkPositionInfoID );
			if( pUserMarkPositionInfo == NULL )
			{
				break;
			}

			CString strChangedName = pWndEditNameChanger->GetString();
			if( strChangedName == _T( "" ) )
			{
				strChangedName = _T( "À§Ä¡" );
			}
			pUserMarkPositionInfo->SetName( strChangedName );

			Destroy();

			break;
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
//-----------------------------------------------------------------------------
void CWndUserMarkNameChanger::SetInfo( DWORD dwUserMarkPositionInfoID, const CString& strName )
{
	m_dwUserMarkPositionInfoID = dwUserMarkPositionInfoID;

	CWndEdit* pWndEditNameChanger = ( CWndEdit* )GetDlgItem( WIDC_EDIT_NAME_CHANGER );
	if( pWndEditNameChanger )
	{
		pWndEditNameChanger->SetString( strName );
	}
}
//-----------------------------------------------------------------------------
#endif // __CLIENT
#endif // __IMPROVE_MAP_SYSTEM