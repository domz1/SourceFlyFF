#include "StdAfx.h"
#ifdef __MAIL_REQUESTING_BOX
#ifdef __CLIENT
#include "WndMailRequestingBox.h"
#include "ResData.h"
#include "DPClient.h"
extern CDPClient g_DPlay;
//-----------------------------------------------------------------------------
CWndMailRequestingBox::CWndMailRequestingBox( void ) : 
m_timeWaitingRequest( g_tmCurrent + SEC( REQUEST_BUTTON_WAITING_TIME ) ), 
m_nDestroyCounter( 0 )
{
	m_bNoCloseButton = TRUE;
}
//-----------------------------------------------------------------------------
CWndMailRequestingBox::~CWndMailRequestingBox( void )
{
}
//-----------------------------------------------------------------------------
BOOL CWndMailRequestingBox::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_REQUESTING_POST, WBS_MODAL, CPoint( 0, 0 ), pWndParent );
}
//-----------------------------------------------------------------------------
void CWndMailRequestingBox::OnInitialUpdate( void )
{
	CWndNeuz::OnInitialUpdate();

	DelWndStyle( WBS_MOVE );

	CWndButton* pWndButton = ( CWndButton* )GetDlgItem( WIDC_BUTTON_REQUEST );
	if( pWndButton )
	{
		pWndButton->EnableWindow( FALSE );
	}

	MoveParentCenter();
}
//-----------------------------------------------------------------------------
BOOL CWndMailRequestingBox::Process( void )
{
	if( m_timeWaitingRequest < g_tmCurrent )
	{
		if( m_nDestroyCounter >= DESTRUCTION_TIME )
		{
			Destroy();
		}

		CWndButton* pWndButton = ( CWndButton* )GetDlgItem( WIDC_BUTTON_REQUEST );
		if( pWndButton )
		{
			pWndButton->EnableWindow( TRUE );
		}
		m_timeWaitingRequest = 0xffffffff;
	}

	return TRUE;
}
//-----------------------------------------------------------------------------
BOOL CWndMailRequestingBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case WIDC_BUTTON_REQUEST:
		{
			++m_nDestroyCounter;
			CWndButton* pWndButton = ( CWndButton* )GetDlgItem( WIDC_BUTTON_REQUEST );
			if( pWndButton == NULL )
			{
				break;
			}

			if( pWndButton->IsWindowEnabled() == FALSE )
			{
				break;
			}

			g_DPlay.SendQueryMailBox();
			m_timeWaitingRequest = g_tmCurrent + SEC( REQUEST_BUTTON_WAITING_TIME );
			pWndButton->EnableWindow( FALSE );

			break;
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
//-----------------------------------------------------------------------------
#endif // __CLIENT
#endif // __MAIL_REQUESTING_BOX