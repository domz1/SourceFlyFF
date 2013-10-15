#include "stdafx.h"
#include "WndColosseum.h"
#ifdef	__COLOSSEUM

#include "resData.h"
#include "defineText.h"
#include "DPClient.h"

#include "party.h"
extern	CParty		g_Party;
extern	CPartyMng	g_PartyMng;

extern	CDPClient			g_DPlay;

CWndColosseumComplete::CWndColosseumComplete() 
{  
} 
CWndColosseumComplete::~CWndColosseumComplete() 
{ 
} 
void CWndColosseumComplete::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	m_ct = m_nTime / 1000;
	CWndStatic* pStatic	= (CWndStatic *)GetDlgItem( WIDC_STATIC3 );
	pStatic->m_dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );
	CString str;
	str.Format( "%02d:%02d:%02d", m_ct.GetHours(), m_ct.GetMinutes(), m_ct.GetSeconds() );


	pStatic->SetTitle(str);

	MoveParentCenter();
}
BOOL CWndColosseumComplete::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_NEWCOLOCOMPLETE, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndColosseumComplete::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	Destroy();
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}
BOOL CWndColosseumComplete::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 	
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 



CWndColosseumEnter::CWndColosseumEnter() 
{  	
} 
CWndColosseumEnter::~CWndColosseumEnter() 
{ 
} 
void CWndColosseumEnter::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	byDifficult = NULL;
	m_pWndButton[0] = (CWndButton*)GetDlgItem( WIDC_BUTTON1 );
	m_pWndButton[1] = (CWndButton*)GetDlgItem( WIDC_BUTTON2 );
	m_pWndButton[2] = (CWndButton*)GetDlgItem( WIDC_BUTTON3 );
	m_pWndButton[3] = (CWndButton*)GetDlgItem( WIDC_BUTTON4 );
	m_pWndButton[0]->EnableWindow( true );
	m_pWndButton[1]->EnableWindow( true );
	m_pWndButton[2]->EnableWindow( false );
	m_pWndButton[3]->EnableWindow( false );

	MoveParentCenter();
}
BOOL CWndColosseumEnter::Process()
{
	if( byDifficult == 1 )
		m_pWndButton[0]->m_bPush = true;
	else if( byDifficult == 2 )
		m_pWndButton[1]->m_bPush = true;

	return TRUE;
}
BOOL CWndColosseumEnter::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_NEWCOLOENTER, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndColosseumEnter::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case WIDC_BUTTON1:
		{
			byDifficult = 1;
			m_pWndButton[2]->EnableWindow( TRUE );
			m_pWndButton[3]->EnableWindow( TRUE );
			break;
		}
	case WIDC_BUTTON2:
		{
			byDifficult = 2;
			m_pWndButton[2]->EnableWindow( TRUE );
			m_pWndButton[3]->EnableWindow( TRUE );
			break;
		}
	case WIDC_BUTTON3:
		{
			g_DPlay.SendColosseumStart(1, byDifficult, 1);
			Destroy();
			break;
		}
	case WIDC_BUTTON4:
		{
			g_DPlay.SendColosseumStart(1, byDifficult, 2);
			Destroy();
			break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}
BOOL CWndColosseumEnter::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 	
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 



CWndColosseumMassage::CWndColosseumMassage() 
{  
} 
CWndColosseumMassage::~CWndColosseumMassage() 
{ 
} 
void CWndColosseumMassage::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate();
	CMover* pMover = g_Party.GetLeader();
	

	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	CString strTemp;
	strTemp.Format( "Your party leader, %s, has invited %s to join the Colosseum.  Will you accept the invitation?",g_Party.GetLeader()->GetName,g_pPlayer->GetName()); 
	pWndText->SetString( strTemp );


	MoveParentCenter();
}
BOOL CWndColosseumMassage::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_NEWCOLOMASSAGE, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndColosseumMassage::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
		case WIDC_BUTTON1:
			{
				g_DPlay.SendColosseumEnter( 2 );
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
BOOL CWndColosseumMassage::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 	
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 



CWndColosseumRankGuild::CWndColosseumRankGuild() 
{  
} 
CWndColosseumRankGuild::~CWndColosseumRankGuild() 
{ 
} 
void CWndColosseumRankGuild::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	MoveParentCenter();
}
BOOL CWndColosseumRankGuild::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_NEWCOLORANKGUILD, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndColosseumRankGuild::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}
BOOL CWndColosseumRankGuild::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 	
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 



CWndColosseumRankNomal::CWndColosseumRankNomal() 
{  
} 
CWndColosseumRankNomal::~CWndColosseumRankNomal() 
{ 
} 
void CWndColosseumRankNomal::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	MoveParentCenter();
}
BOOL CWndColosseumRankNomal::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_NEWCOLORANKNOMAL, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndColosseumRankNomal::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}
BOOL CWndColosseumRankNomal::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 	
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 



CWndColosseumRetry::CWndColosseumRetry() 
{  
} 
CWndColosseumRetry::~CWndColosseumRetry() 
{ 
} 
void CWndColosseumRetry::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	MoveParentCenter();
}
BOOL CWndColosseumRetry::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_NEWCOLORETRY, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndColosseumRetry::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	Destroy();
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}
BOOL CWndColosseumRetry::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 	
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 



CWndColosseumStart::CWndColosseumStart() 
{  
} 
CWndColosseumStart::~CWndColosseumStart() 
{ 
} 
void CWndColosseumStart::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	if( !g_Party.IsLeader( g_pPlayer->m_idPlayer ) )
	{
		CWndButton* pButton = (CWndButton*)GetDlgItem( WIDC_BUTTON1 );
		pButton->EnableWindow( FALSE );
	}

	MoveParentCenter();


}
BOOL CWndColosseumStart::Process()
{

	m_ct = ( m_nTime - g_tmCurrent ) / 1000;
	if( (int)m_ct.GetTotalSeconds() > 0 )
	{
		CWndStatic* pStatic	= (CWndStatic *)GetDlgItem( WIDC_STATIC3 );
		pStatic->m_dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );
		CString str;
		str.Format( "%02d:%02d:%02d", m_ct.GetHours(), m_ct.GetMinutes(), m_ct.GetSeconds() );

		if( m_ct.GetTotalSeconds() <= 5 )
			pStatic->m_dwColor = D3DCOLOR_ARGB( 255, 229, 0, 0 );

		pStatic->SetTitle(str);
	}
	else
	{
		Destroy();
	}
	return TRUE;
}
BOOL CWndColosseumStart::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_NEWCOLOSTART, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndColosseumStart::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BUTTON1 )
	{
		g_DPlay.SendColosseumEndWait( 3 );
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}
BOOL CWndColosseumStart::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 	
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 



CWndColosseumStartReady1::CWndColosseumStartReady1() 
{  
} 
CWndColosseumStartReady1::~CWndColosseumStartReady1() 
{ 
} 
BOOL CWndColosseumStartReady1::Process()
{
CWorld* pWorld = g_pPlayer->GetWorld();
if( pWorld->GetID() == WI_WORLD_COLOSSEUM )
{
	m_ct = ( m_nTime - g_tmCurrent ) / 1000;
	if( (int)m_ct.GetTotalSeconds() > 0 )
	{
		CWndStatic* pStatic	= (CWndStatic *)GetDlgItem( WIDC_STATIC1 );
		pStatic->m_dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );
		CString str;
		str.Format( "%02d:%02d:%02d", m_ct.GetHours(), m_ct.GetMinutes(), m_ct.GetSeconds() );

		if( m_ct.GetTotalSeconds() <= 5 )
			pStatic->m_dwColor = D3DCOLOR_ARGB( 255, 229, 0, 0 );

		pStatic->SetTitle(str);

	}
}
else
	{
		Destroy();
	}
	return TRUE;
}
void CWndColosseumStartReady1::OnInitialUpdate() 
{ 
CWorld* pWorld = g_pPlayer->GetWorld();
if( pWorld->GetID() == WI_WORLD_COLOSSEUM )
{
	CWndNeuz::OnInitialUpdate(); 
	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC2 );
    pWndStatic->m_dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );
	CString strTemp;
	strTemp.Format(_T( prj.GetText(TID_COLOSSEUM_WATINGSTAGESTART01) ), m_nStage ); 
	pWndStatic->SetTitle( strTemp );

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( ( rectRoot.right - rectWindow.Width() ) / 2, 65 );
	Move( point );


	DWORD dwStyle = GetWndStyle();
	dwStyle &= (~WBS_MOVE);
	SetWndStyle( dwStyle );
}
else
    {
	Destroy();
    }

}
BOOL CWndColosseumStartReady1::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_NEWCOLOSTARTREADY1, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndColosseumStartReady1::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}
BOOL CWndColosseumStartReady1::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 	
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 


CWndColosseumStartReady2::CWndColosseumStartReady2() 
{  
} 
CWndColosseumStartReady2::~CWndColosseumStartReady2() 
{ 
} 
BOOL CWndColosseumStartReady2::Process()
{
CWorld* pWorld = g_pPlayer->GetWorld();
if( pWorld->GetID() == WI_WORLD_COLOSSEUM )
{

	m_ct = ( m_nTime - g_tmCurrent ) / 1000;
	if( (int)m_ct.GetTotalSeconds() > 0 )
	{
		CWndStatic* pStatic	 = (CWndStatic *)GetDlgItem( WIDC_STATIC1 );
		pStatic->m_dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );

		CString str;
		str.Format( "%02d:%02d:%02d", m_ct.GetHours(), m_ct.GetMinutes(), m_ct.GetSeconds() );

		if( m_ct.GetTotalSeconds() <= 5 )
			pStatic->m_dwColor = D3DCOLOR_ARGB( 255, 229, 0, 0 );

		pStatic->SetTitle(str);
	}
}
else
{
    Destroy();
}
	return TRUE;
}
void CWndColosseumStartReady2::OnInitialUpdate() 
{ 
CWorld* pWorld = g_pPlayer->GetWorld();
if( pWorld->GetID() == WI_WORLD_COLOSSEUM )
{
	CWndNeuz::OnInitialUpdate(); 

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( ( rectRoot.right - rectWindow.Width() ) / 2, 65 );
	Move( point );

	DWORD dwStyle = GetWndStyle();
	dwStyle &= (~WBS_MOVE);
	SetWndStyle( dwStyle );

	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
	pWndWorld->dwColosseumStartSplashTick = g_tmCurrent + 10000;

}
else
   {
	Destroy();
   }
}
BOOL CWndColosseumStartReady2::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_NEWCOLOSTARTREADY2, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndColosseumStartReady2::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}
BOOL CWndColosseumStartReady2::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 	
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
#endif //__COLOSSEUM