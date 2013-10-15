
#include "StdAfx.h"

//Author : gmpbigsun
//Date : 2009_11_16

#if __VER >= 15 // __GUILD_HOUSE
#include "WndGuildTabPower.h"
#include "AppDefine.h"
#include "DPClient.h"

extern	CDPClient	g_DPlay;

CWndGuildTabPower::CWndGuildTabPower() 
{ 
	m_bChanedCheckBox = FALSE;
} 

CWndGuildTabPower::~CWndGuildTabPower() 
{ 
} 

void CWndGuildTabPower::OnDraw( C2DRender* p2DRender ) 
{ 
} 

void CWndGuildTabPower::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	UpdateData();

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();

	//최초엔 버튼 disable 
	CWndButton* pWndBtnOK = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	pWndBtnOK->EnableWindow(m_bChanedCheckBox);
} 

BOOL CWndGuildTabPower::Initialize( CWndBase* pWndParent, DWORD ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILD_TAPPOWER, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndGuildTabPower::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndGuildTabPower::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndGuildTabPower::UpdateData()
{
	CGuild* pGuild = g_pPlayer->GetGuild();
	if( pGuild )
	{
		SetData( pGuild->m_adwPower );

		if( pGuild->IsMaster( g_pPlayer->m_idPlayer ) )
			EnableButton( TRUE );
		else EnableButton( FALSE );
	}
	else
	{
		DWORD adwPower [MAX_GM_LEVEL] = { 0 };
		SetData( adwPower );

		EnableButton( FALSE );
	}
}

void CWndGuildTabPower::EnableButton(BOOL bEnable)
{
	CWndButton* pWndCheck = NULL;
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK1);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK9);
	pWndCheck->EnableWindow(bEnable);
	
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK2);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK10);
	pWndCheck->EnableWindow(bEnable);
	
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK3);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK11);
	pWndCheck->EnableWindow(bEnable);
		
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK4);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK12);
	pWndCheck->EnableWindow(bEnable);
	
	// 버튼 부분
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	pWndCheck->EnableWindow(bEnable);
	
	//취소버튼 일단 잠금 
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_BUTTON11);
	pWndCheck->EnableWindow(FALSE);
}

void CWndGuildTabPower::SetData(DWORD dwPower[])
{
	CWndButton* pWndCheck = NULL;

	memcpy( m_adwPower, dwPower, sizeof(DWORD)*MAX_GM_LEVEL );

	// Master
	// GUD_KINGPIN
	if( m_adwPower[GUD_KINGPIN] & PF_GUILDHOUSE_FURNITURE )
	{
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK1);
		pWndCheck->SetCheck(1);
	}

	if( m_adwPower[GUD_KINGPIN] & PF_GUILDHOUSE_UPKEEP )
	{
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK9);
		pWndCheck->SetCheck(1);
	}


	// GUD_CAPTAIN
	if( m_adwPower[GUD_CAPTAIN] & PF_GUILDHOUSE_FURNITURE )
	{
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK2);
		pWndCheck->SetCheck(1);
	}

	if( m_adwPower[GUD_CAPTAIN] & PF_GUILDHOUSE_UPKEEP )
	{
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK10);
		pWndCheck->SetCheck(1);
	}

	// GUD_SUPPORTER
	if( m_adwPower[GUD_SUPPORTER] & PF_GUILDHOUSE_FURNITURE )
	{
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK3);
		if(pWndCheck) pWndCheck->SetCheck(1);
	}

	if( m_adwPower[GUD_SUPPORTER] & PF_GUILDHOUSE_UPKEEP )
	{
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK11);
		pWndCheck->SetCheck(1);
	}

	// GUD_ROOKIE
	if( m_adwPower[GUD_ROOKIE] & PF_GUILDHOUSE_FURNITURE )
	{
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK4);
		pWndCheck->SetCheck(1);
	}

	if( m_adwPower[GUD_ROOKIE] & PF_GUILDHOUSE_UPKEEP )
	{
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK12);
		pWndCheck->SetCheck(1);
	}
}

BOOL CWndGuildTabPower::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	CGuild* pGuild = g_pPlayer->GetGuild();
	
	if( !pGuild )
		return FALSE;
	
	CGuildMember* pGuildMember = pGuild->GetMember(g_pPlayer->m_idPlayer);
	
	if( !pGuildMember )
		return FALSE;
	
	if(	pGuildMember->m_nMemberLv != GUD_MASTER )
		return FALSE;

	static BOOL bChanged = FALSE;		//변화가 됐을때만 요청 
	
	//Kingpin
	if( nID == WIDC_CHECK1 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK1);
		pWndCheck->GetCheck() ? m_adwPower[GUD_KINGPIN] |= PF_GUILDHOUSE_FURNITURE : m_adwPower[GUD_KINGPIN] &= (~PF_GUILDHOUSE_FURNITURE);
		bChanged = TRUE;
	}
	
	if( nID == WIDC_CHECK9 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK9);
		pWndCheck->GetCheck() ? m_adwPower[GUD_KINGPIN] |= PF_GUILDHOUSE_UPKEEP : m_adwPower[GUD_KINGPIN] &= (~PF_GUILDHOUSE_UPKEEP);
		bChanged = TRUE;
	}
	
	
	//GA_LEADER
	if( nID == WIDC_CHECK2 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK2);
		pWndCheck->GetCheck() ? m_adwPower[GUD_CAPTAIN] |= PF_GUILDHOUSE_FURNITURE : m_adwPower[GUD_CAPTAIN] &= (~PF_GUILDHOUSE_FURNITURE);
		bChanged = TRUE;
	}
	
	if( nID == WIDC_CHECK10 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK10);
		pWndCheck->GetCheck() ? m_adwPower[GUD_CAPTAIN] |= PF_GUILDHOUSE_UPKEEP : m_adwPower[GUD_CAPTAIN] &= (~PF_GUILDHOUSE_UPKEEP);
		bChanged = TRUE;
	}
	
	//GA_SUPPORTER
	if( nID == WIDC_CHECK3 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK3);
		pWndCheck->GetCheck() ? m_adwPower[GUD_SUPPORTER] |= PF_GUILDHOUSE_FURNITURE : m_adwPower[GUD_SUPPORTER] &= (~PF_GUILDHOUSE_FURNITURE);
		bChanged = TRUE;
	}
	
	if( nID == WIDC_CHECK11 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK11);
		pWndCheck->GetCheck() ? m_adwPower[GUD_SUPPORTER] |= PF_GUILDHOUSE_UPKEEP : m_adwPower[GUD_SUPPORTER] &= (~PF_GUILDHOUSE_UPKEEP);
		bChanged = TRUE;
	}
	
	
	//GA_ROOKIE
	if( nID == WIDC_CHECK4 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK4);
		pWndCheck->GetCheck() ? m_adwPower[GUD_ROOKIE] |= PF_GUILDHOUSE_FURNITURE : m_adwPower[GUD_ROOKIE] &= (~PF_GUILDHOUSE_FURNITURE);
		bChanged = TRUE;
	}
	
	if( nID == WIDC_CHECK12 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK12);
		pWndCheck->GetCheck() ? m_adwPower[GUD_ROOKIE] |= PF_GUILDHOUSE_UPKEEP : m_adwPower[GUD_ROOKIE] &= (~PF_GUILDHOUSE_UPKEEP);
		bChanged = TRUE;
	}
	

	 if( WIDC_BUTTON1 == nID && bChanged )	// 보내기
	{
		g_DPlay.SendGuildAuthority( pGuild->GetGuildId(), m_adwPower );
		bChanged = FALSE;
	}

	CWndButton* pWndBtnOK = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	if( pWndBtnOK )
	{
		pWndBtnOK->EnableWindow( bChanged );
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

#endif //__GUILD_HOUSE