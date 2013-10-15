#include "StdAfx.h"
#include "WndChattingBlockingList.h"
#include "playerdata.h"
#include "ResData.h"
#include "defineText.h"

#ifdef __YS_CHATTING_BLOCKING_SYSTEM
#ifdef __CLIENT
//-----------------------------------------------------------------------------
CWndChattingBlockingList::CWndChattingBlockingList( void )
{
}
//-----------------------------------------------------------------------------
CWndChattingBlockingList::~CWndChattingBlockingList( void )
{
}
//-----------------------------------------------------------------------------
BOOL CWndChattingBlockingList::Initialize( CWndBase* pWndParent, DWORD nType )
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_CHATTING_BLOCKING_LIST, 0, CPoint( 0, 0 ), pWndParent );
}
//-----------------------------------------------------------------------------
void CWndChattingBlockingList::OnInitialUpdate( void )
{
	CWndNeuz::OnInitialUpdate();

	if( g_pPlayer )
	{
		CString strTitle = _T( "" );
		// [ %s ] 님의 차단 목록
		strTitle.Format( prj.GetText( TID_GAME_CHATTING_BLOCKING_LIST_TITLE ), g_pPlayer->GetName() );
		SetTitle( strTitle );
	}

	UpdateInformation();

	MoveParentCenter();
}
//-----------------------------------------------------------------------------
void CWndChattingBlockingList::UpdateInformation( void )
{
	CWndGroupBox* pWndGroupBox = ( CWndGroupBox* )GetDlgItem( WIDC_GROUP_BOX_TOTAL_NUMBER );
	if( pWndGroupBox )
	{
		CString strBlockedUsersTotalNumber = _T( "" );
		// 차단된 총 인원 %d명
		strBlockedUsersTotalNumber.Format( prj.GetText( TID_GAME_CHATTING_BLOCKING_USERS_TOTAL_NUMBER ), static_cast< int >( prj.m_setBlockedUserID.size() ) );
		pWndGroupBox->SetTitle( strBlockedUsersTotalNumber );
	}

	CWndListBox* pWndListBox = ( CWndListBox* )GetDlgItem( WIDC_LISTBOX_BLOCKING_USERS );
	if( pWndListBox )
	{
		pWndListBox->ResetContent();
		for( set< CString >::iterator Iterator = prj.m_setBlockedUserID.begin(); Iterator != prj.m_setBlockedUserID.end(); ++Iterator )
		{
			pWndListBox->AddString( *Iterator );
		}
	}
}
//-----------------------------------------------------------------------------
#endif // __CLIENT
#endif // __YS_CHATTING_BLOCKING_SYSTEM