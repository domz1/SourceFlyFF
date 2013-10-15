#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndGuildTabApp.h"
#include "WndManager.h"

#include "DPClient.h"
extern	CDPClient	g_DPlay;

#include "guild.h"
extern	CGuildMng	g_GuildMng;




/****************************************************
  WndId : APP_GUILD_TABAPP 
****************************************************/
CWndGuildTabApp::CWndGuildTabApp() 
{ 
	for( int i=0; i<MAX_GM_LEVEL; i++ )
		m_adwPower[i] = 0xffffffff;

	m_pWndGuildPayConfirm = NULL;
} 
CWndGuildTabApp::~CWndGuildTabApp() 
{ 
	SAFE_DELETE(m_pWndGuildPayConfirm);
} 
void CWndGuildTabApp::OnDraw( C2DRender* p2DRender ) 
{ 
	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
		
	TEXTUREVERTEX2* pVertex = new TEXTUREVERTEX2[ 6 * MAX_GM_LEVEL ];
	TEXTUREVERTEX2* pVertices = pVertex;
	
	LPWNDCTRL pCustom = NULL;
	
	pCustom = GetWndCtrl( WIDC_CUSTOM1 );
	pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, pCustom->rect.TopLeft(), 43 + 0 + ( 6 * 1 ), &pVertices, 0xffffffff );
	pCustom = GetWndCtrl( WIDC_CUSTOM2 );
	pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, pCustom->rect.TopLeft(), 43 + 1 + ( 6 * 1 ), &pVertices, 0xffffffff );
	pCustom = GetWndCtrl( WIDC_CUSTOM3 );
	pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, pCustom->rect.TopLeft(), 43 + 2 + ( 6 * 1 ), &pVertices, 0xffffffff );
	pCustom = GetWndCtrl( WIDC_CUSTOM4 );
	pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, pCustom->rect.TopLeft(), 43 + 3 + ( 6 * 1 ), &pVertices, 0xffffffff );
	pCustom = GetWndCtrl( WIDC_CUSTOM5 );
	pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, pCustom->rect.TopLeft(), 43 + 4 + ( 6 * 1 ), &pVertices, 0xffffffff );
	
	pWndWorld->m_texMsgIcon.Render( m_pApp->m_pd3dDevice, pVertex, ( (int) pVertices - (int) pVertex ) / sizeof( TEXTUREVERTEX2 ) );

	SAFE_DELETE_ARRAY( pVertex );
} 
void CWndGuildTabApp::OnInitialUpdate() 
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
} 
BOOL CWndGuildTabApp::Initialize( CWndBase* pWndParent, DWORD ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILD_TABAPPELLATION, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndGuildTabApp::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndGuildTabApp::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndGuildTabApp::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndGuildTabApp::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndGuildTabApp::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	CGuild* pGuild = g_pPlayer->GetGuild();
	
	if( pGuild == NULL )
		return FALSE;
	
	CGuildMember* pGuildMember = pGuild->GetMember(g_pPlayer->m_idPlayer);
	
	if( pGuildMember == NULL )
		return FALSE;
	
	if(	pGuildMember->m_nMemberLv != GUD_MASTER )
		return FALSE;
	
	/*
	// Master
	if( nID == WIDC_CHECK1 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK1);
		pWndCheck->GetCheck() ? m_adwPower[GUD_MASTER] |= PF_MEMBERLEVEL : m_adwPower[GUD_MASTER] &= (~PF_MEMBERLEVEL);
	}
	if( nID == WIDC_CHECK6 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK6);
		pWndCheck->GetCheck() ? m_adwPower[GUD_MASTER] |= PF_LEVEL : m_adwPower[GUD_MASTER] &= (~PF_LEVEL);
	}
	if( nID == WIDC_CHECK11 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK11);
		pWndCheck->GetCheck() ? m_adwPower[GUD_MASTER] |= PF_INVITATION : m_adwPower[GUD_MASTER] &= (~PF_INVITATION);
	}
	if( nID == WIDC_CHECK16 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK16);
		pWndCheck->GetCheck() ? m_adwPower[GUD_MASTER] |= PF_PENYA : m_adwPower[GUD_MASTER] &= (~PF_PENYA);
	}
	if( nID == WIDC_CHECK21 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK21);
		pWndCheck->GetCheck() ? m_adwPower[GUD_MASTER] |= PF_ITEM : m_adwPower[GUD_MASTER] &= (~PF_ITEM);
	}
	*/
	
	//Kingpin
	if( nID == WIDC_CHECK2 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK2);
		pWndCheck->GetCheck() ? m_adwPower[GUD_KINGPIN] |= PF_MEMBERLEVEL : m_adwPower[GUD_KINGPIN] &= (~PF_MEMBERLEVEL);
	}
	if( nID == WIDC_CHECK7 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK7);
		pWndCheck->GetCheck() ? m_adwPower[GUD_KINGPIN] |= PF_LEVEL : m_adwPower[GUD_KINGPIN] &= (~PF_LEVEL);
	}
	if( nID == WIDC_CHECK12 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK12);
		pWndCheck->GetCheck() ? m_adwPower[GUD_KINGPIN] |= PF_INVITATION : m_adwPower[GUD_KINGPIN] &= (~PF_INVITATION);
	}
	if( nID == WIDC_CHECK17 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK17);
		pWndCheck->GetCheck() ? m_adwPower[GUD_KINGPIN] |= PF_PENYA : m_adwPower[GUD_KINGPIN] &= (~PF_PENYA);
	}
	if( nID == WIDC_CHECK22 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK22);
		pWndCheck->GetCheck() ? m_adwPower[GUD_KINGPIN] |= PF_ITEM : m_adwPower[GUD_KINGPIN] &= (~PF_ITEM);
	}
	
	//GA_LEADER
	if( nID == WIDC_CHECK3 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK3);
		pWndCheck->GetCheck() ? m_adwPower[GUD_CAPTAIN] |= PF_MEMBERLEVEL : m_adwPower[GUD_CAPTAIN] &= (~PF_MEMBERLEVEL);
	}
	if( nID == WIDC_CHECK8 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK8);
		pWndCheck->GetCheck() ? m_adwPower[GUD_CAPTAIN] |= PF_LEVEL : m_adwPower[GUD_CAPTAIN] &= (~PF_LEVEL);
	}
	if( nID == WIDC_CHECK13 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK13);
		pWndCheck->GetCheck() ? m_adwPower[GUD_CAPTAIN] |= PF_INVITATION : m_adwPower[GUD_CAPTAIN] &= (~PF_INVITATION);
	}
	if( nID == WIDC_CHECK18 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK18);
		pWndCheck->GetCheck() ? m_adwPower[GUD_CAPTAIN] |= PF_PENYA : m_adwPower[GUD_CAPTAIN] &= (~PF_PENYA);
	}
	if( nID == WIDC_CHECK23 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK23);
		pWndCheck->GetCheck() ? m_adwPower[GUD_CAPTAIN] |= PF_ITEM : m_adwPower[GUD_CAPTAIN] &= (~PF_ITEM);
	}
	
	//GA_SUPPORTER
	if( nID == WIDC_CHECK4 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK4);
		pWndCheck->GetCheck() ? m_adwPower[GUD_SUPPORTER] |= PF_MEMBERLEVEL : m_adwPower[GUD_SUPPORTER] &= (~PF_MEMBERLEVEL);
	}
	if( nID == WIDC_CHECK9 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK9);
		pWndCheck->GetCheck() ? m_adwPower[GUD_SUPPORTER] |= PF_LEVEL : m_adwPower[GUD_SUPPORTER] &= (~PF_LEVEL);
	}
	if( nID == WIDC_CHECK14 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK14);
		pWndCheck->GetCheck() ? m_adwPower[GUD_SUPPORTER] |= PF_INVITATION : m_adwPower[GUD_SUPPORTER] &= (~PF_INVITATION);
	}
	if( nID == WIDC_CHECK19 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK19);
		pWndCheck->GetCheck() ? m_adwPower[GUD_SUPPORTER] |= PF_PENYA : m_adwPower[GUD_SUPPORTER] &= (~PF_PENYA);
	}
	if( nID == WIDC_CHECK24 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK24);
		pWndCheck->GetCheck() ? m_adwPower[GUD_SUPPORTER] |= PF_ITEM : m_adwPower[GUD_SUPPORTER] &= (~PF_ITEM);
	}

	
	//GA_ROOKIE
	if( nID == WIDC_CHECK5 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK5);
		pWndCheck->GetCheck() ? m_adwPower[GUD_ROOKIE] |= PF_MEMBERLEVEL : m_adwPower[GUD_ROOKIE] &= (~PF_MEMBERLEVEL);
	}
	if( nID == WIDC_CHECK10 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK10);
		pWndCheck->GetCheck() ? m_adwPower[GUD_ROOKIE] |= PF_LEVEL : m_adwPower[GUD_ROOKIE] &= (~PF_LEVEL);
	}
	if( nID == WIDC_CHECK15 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK15);
		pWndCheck->GetCheck() ? m_adwPower[GUD_ROOKIE] |= PF_INVITATION : m_adwPower[GUD_ROOKIE] &= (~PF_INVITATION);
	}
	if( nID == WIDC_CHECK20 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK20);
		pWndCheck->GetCheck() ? m_adwPower[GUD_ROOKIE] |= PF_PENYA : m_adwPower[GUD_ROOKIE] &= (~PF_PENYA);
	}
	if( nID == WIDC_CHECK25 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK25);
		pWndCheck->GetCheck() ? m_adwPower[GUD_ROOKIE] |= PF_ITEM : m_adwPower[GUD_ROOKIE] &= (~PF_ITEM);
	}

	// 설절버튼 눌렀을때 실행 클라에서 먼저 가지고 있는권한이 다르면 서버로 전송
	if( nID == WIDC_BUTTON1 )	// 마스트
	{
		SAFE_DELETE(m_pWndGuildPayConfirm);
		m_pWndGuildPayConfirm = new CWndGuildPayConfirm;
		m_pWndGuildPayConfirm->Initialize( this );
		m_pWndGuildPayConfirm->m_dwAppellation = GUD_MASTER;
	}
	else if( nID == WIDC_BUTTON2 )  // 킹 핀
	{
		SAFE_DELETE(m_pWndGuildPayConfirm);
		m_pWndGuildPayConfirm = new CWndGuildPayConfirm;
		m_pWndGuildPayConfirm->Initialize( this );
		m_pWndGuildPayConfirm->m_dwAppellation = GUD_KINGPIN;
	}
	else if( nID == WIDC_BUTTON3 )  // 캡 틴
	{
		SAFE_DELETE(m_pWndGuildPayConfirm);
		m_pWndGuildPayConfirm = new CWndGuildPayConfirm;
		m_pWndGuildPayConfirm->Initialize( this );
		m_pWndGuildPayConfirm->m_dwAppellation = GUD_CAPTAIN;
	}
	else if( nID == WIDC_BUTTON4 )  // 서포터
	{
		SAFE_DELETE(m_pWndGuildPayConfirm);
		m_pWndGuildPayConfirm = new CWndGuildPayConfirm;
		m_pWndGuildPayConfirm->Initialize( this );
		m_pWndGuildPayConfirm->m_dwAppellation = GUD_SUPPORTER;
	}
	else if( nID == WIDC_BUTTON5 )  // 루 키
	{
		SAFE_DELETE(m_pWndGuildPayConfirm);
		m_pWndGuildPayConfirm = new CWndGuildPayConfirm;
		m_pWndGuildPayConfirm->Initialize( this );
		m_pWndGuildPayConfirm->m_dwAppellation = GUD_ROOKIE;
	}
	else if( nID == WIDC_OK )	// 보내기
	{
		g_DPlay.SendGuildAuthority( pGuild->GetGuildId(), m_adwPower );
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


void CWndGuildTabApp::SetPenya( void )
{
	CGuild * pGuild = g_pPlayer->GetGuild();
	if( pGuild )
	{
		CString strMessege;
		for( int i = 0 ; i < MAX_GM_LEVEL ; ++i )
		{
			strMessege.Format( "%d", pGuild->m_adwPenya[ i ] );
			m_pWndPenya[i]->SetTitle( strMessege );
		}
	}
}


void CWndGuildTabApp::SetData(DWORD dwPower[])
{
	CWndButton* pWndCheck = NULL;

	memcpy( m_adwPower, dwPower, sizeof(DWORD)*MAX_GM_LEVEL );

	// Master
	if( m_adwPower[GUD_MASTER] & PF_MEMBERLEVEL )
	{
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK1);
		pWndCheck->SetCheck(1);
	}

	if( m_adwPower[GUD_MASTER] & PF_LEVEL )
	{
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK6);
		pWndCheck->SetCheck(1);
	}

	if( m_adwPower[GUD_MASTER] & PF_INVITATION )
	{
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK11);
		pWndCheck->SetCheck(1);
	}

	if( m_adwPower[GUD_MASTER] & PF_PENYA )
	{
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK16);
		pWndCheck->SetCheck(1);
	}

	if( m_adwPower[GUD_MASTER] & PF_ITEM )
	{
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK21);
		pWndCheck->SetCheck(1);
	}
	
	
	// GUD_KINGPIN
	if( m_adwPower[GUD_KINGPIN] & PF_MEMBERLEVEL )
	{
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK2);
		pWndCheck->SetCheck(1);
	}

	if( m_adwPower[GUD_KINGPIN] & PF_LEVEL )
	{
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK7);
		pWndCheck->SetCheck(1);
	}

	if( m_adwPower[GUD_KINGPIN] & PF_INVITATION )
	{
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK12);
		pWndCheck->SetCheck(1);
	}

	if( m_adwPower[GUD_KINGPIN] & PF_PENYA )
	{
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK17);
		pWndCheck->SetCheck(1);
	}

	if( m_adwPower[GUD_KINGPIN] & PF_ITEM )
	{
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK22);
		pWndCheck->SetCheck(1);
	}
	

	// GUD_CAPTAIN
	if( m_adwPower[GUD_CAPTAIN] & PF_MEMBERLEVEL )
	{
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK3);
		pWndCheck->SetCheck(1);
	}

	if( m_adwPower[GUD_CAPTAIN] & PF_LEVEL )
	{
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK8);
		pWndCheck->SetCheck(1);
	}

	if( m_adwPower[GUD_CAPTAIN] & PF_INVITATION )
	{
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK13);
		pWndCheck->SetCheck(1);
	}

	if( m_adwPower[GUD_CAPTAIN] & PF_PENYA )
	{
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK18);
		pWndCheck->SetCheck(1);
	}

	if( m_adwPower[GUD_CAPTAIN] & PF_ITEM )
	{
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK23);
		pWndCheck->SetCheck(1);
	}


	// GUD_SUPPORTER
	if( m_adwPower[GUD_SUPPORTER] & PF_MEMBERLEVEL )
	{
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK4);
		if(pWndCheck) pWndCheck->SetCheck(1);
	}

	if( m_adwPower[GUD_SUPPORTER] & PF_LEVEL )
	{
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK9);
		pWndCheck->SetCheck(1);
	}

	if( m_adwPower[GUD_SUPPORTER] & PF_INVITATION )
	{
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK14);
		pWndCheck->SetCheck(1);
	}

	if( m_adwPower[GUD_SUPPORTER] & PF_PENYA )
	{
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK19);
		pWndCheck->SetCheck(1);
	}

	if( m_adwPower[GUD_SUPPORTER] & PF_ITEM )
	{
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK24);
		pWndCheck->SetCheck(1);
	}

	// GUD_ROOKIE
	if( m_adwPower[GUD_ROOKIE] & PF_MEMBERLEVEL )
	{
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK5);
		pWndCheck->SetCheck(1);
	}

	if( m_adwPower[GUD_ROOKIE] & PF_LEVEL )
	{
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK10);
		pWndCheck->SetCheck(1);
	}

	if( m_adwPower[GUD_ROOKIE] & PF_INVITATION )
	{
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK15);
		pWndCheck->SetCheck(1);
	}

	if( m_adwPower[GUD_ROOKIE] & PF_PENYA )
	{
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK20);
		pWndCheck->SetCheck(1);
	}

	if( m_adwPower[GUD_ROOKIE] & PF_ITEM )
	{
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK25);
		pWndCheck->SetCheck(1);
	}
}

/****************************************************
  WndId : APP_GUILD_PAYCONFIRM
****************************************************/
CWndGuildPayConfirm::CWndGuildPayConfirm() 
{ 
	m_dwAppellation = -1;
} 
CWndGuildPayConfirm::~CWndGuildPayConfirm() 
{ 
} 
void CWndGuildPayConfirm::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndGuildPayConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndGuildPayConfirm::Initialize( CWndBase* pWndParent ) 
{ 
	LPWNDAPPLET lpWndApplet = m_resMng.GetAt ( APP_GUILD_PAYCONFIRM );
	CRect rect = CRect( 0, 0, lpWndApplet->size.cx, lpWndApplet->size.cy );

	return CWndNeuz::Create( /*WBS_THICKFRAME |*/ WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_MODAL, rect, pWndParent, APP_GUILD_PAYCONFIRM ); 
} 

BOOL CWndGuildPayConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndGuildPayConfirm::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndGuildPayConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndGuildPayConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndGuildPayConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES )
	{
		CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );

		CString strcost = pWndEdit->GetString();

		BOOL bisdigit = TRUE;
		for( int i = 0 ; i < strcost.GetLength() ; i++ )
		{
			if( isdigit2( strcost.GetAt(i) ) == FALSE )
			{
				bisdigit = FALSE;	
				break;
			}
		}

		
		if( bisdigit )
		{ 
			DWORD dwSendPenya = atoi( pWndEdit->GetString() );

			if( 0 <= dwSendPenya && dwSendPenya <= 999999 )
			{	
				CGuild * pGuild = g_pPlayer->GetGuild();

				if( pGuild )
				{
					// Send보냄
					g_DPlay.SendGuilPenya( pGuild->GetGuildId(), m_dwAppellation, dwSendPenya );
				}
				Destroy();
			}
			else
			{
				pWndEdit->SetString( "" );
				g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDONLYNUMBER), MB_OK, this );
			}
		}
		else
		{
			pWndEdit->SetString( "" );
			g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDONLYNUMBER), MB_OK, this );
		}
	}
	else
	if( nID == WIDC_NO )
	{
		Destroy();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndGuildTabApp::EnableButton(BOOL bEnable)
{
	CWndButton* pWndCheck = NULL;
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK1);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK6);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK11);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK16);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK21);
	pWndCheck->EnableWindow(bEnable);

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK2);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK7);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK12);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK17);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK22);
	pWndCheck->EnableWindow(bEnable);

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK3);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK8);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK13);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK18);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK23);
	pWndCheck->EnableWindow(bEnable);
	
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK4);
	if(pWndCheck) pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK9);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK14);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK19);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK24);
	pWndCheck->EnableWindow(bEnable);

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK5);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK10);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK15);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK20);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK25);
	pWndCheck->EnableWindow(bEnable);
	
	// 버튼 부분
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_BUTTON2);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_BUTTON3);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_BUTTON4);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_BUTTON5);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_OK);
	pWndCheck->EnableWindow(bEnable);
	
}


void CWndGuildTabApp::UpdateData()
{
	m_pWndPenya[GUD_MASTER] = (CWndStatic*)GetDlgItem( WIDC_GUILD_PAY_MASTER );
	m_pWndPenya[GUD_KINGPIN] = (CWndStatic*)GetDlgItem( WIDC_GUILD_PAY_KINGPIN );
	m_pWndPenya[GUD_CAPTAIN] = (CWndStatic*)GetDlgItem( WIDC_GUILD_PAY_CAPTAIN );
	m_pWndPenya[GUD_SUPPORTER] = (CWndStatic*)GetDlgItem( WIDC_GUILD_PAY_SUPPORTER );
	m_pWndPenya[GUD_ROOKIE] = (CWndStatic*)GetDlgItem( WIDC_GUILD_PAY_ROOKIE );
	
	CGuild* pGuild = g_pPlayer->GetGuild();
	
	if( pGuild  )
	{
		CString str;
		CWndStatic* pStatic;
		
		pStatic = (CWndStatic*)GetDlgItem( WIDC_GUILD_NUM_MASTER );
		str.Format( "%.2d/%.2d", pGuild->GetMemberLvSize(GUD_MASTER), pGuild->GetMaxMemberLvSize( GUD_MASTER ) );
		pStatic->SetTitle( str );
		
		pStatic = (CWndStatic*)GetDlgItem( WIDC_GUILD_NUM_KINGPIN );
		str.Format( "%.2d/%.2d", pGuild->GetMemberLvSize(GUD_KINGPIN), pGuild->GetMaxMemberLvSize( GUD_KINGPIN ) );
		pStatic->SetTitle( str );
		
		pStatic = (CWndStatic*)GetDlgItem( WIDC_GUILD_NUM_CAPTAIN  );
		str.Format( "%.2d/%.2d", pGuild->GetMemberLvSize(GUD_CAPTAIN), pGuild->GetMaxMemberLvSize( GUD_CAPTAIN ) );
		pStatic->SetTitle( str );
		
		pStatic = (CWndStatic*)GetDlgItem( WIDC_GUILD_NUM_SUPPORTER );
		str.Format( "%.2d/%.2d", pGuild->GetMemberLvSize(GUD_SUPPORTER), pGuild->GetMaxMemberLvSize( GUD_SUPPORTER ) );
		pStatic->SetTitle( str );

		pStatic = (CWndStatic*)GetDlgItem( WIDC_GUILD_NUM_ROOKIE );
		str.Format( "%.2d/%.2d", pGuild->GetMemberLvSize(GUD_ROOKIE), pGuild->GetMaxMemberSize() );
//		str.Format( "%.2d/%.2d", pGuild->GetMaxMemberSize(), pGuild->GetMaxMemberSize() );
		pStatic->SetTitle( str );
		
		SetData( pGuild->m_adwPower );
		SetPenya();
		
		if(	pGuild->IsMaster( g_pPlayer->m_idPlayer ) )
			EnableButton( TRUE );
		else
			EnableButton( FALSE );
	}
	else
	{
		DWORD adwPower [MAX_GM_LEVEL] = { 0 };
		SetData( adwPower );

		EnableButton( FALSE );
		
		for( int i = 0 ; i < MAX_GM_LEVEL ; ++i )
		{
			m_pWndPenya[i]->SetTitle( "0" );
		}
	}
	
	// 마스터 부분의 버튼은 누룰수 없다
	CWndButton* pWndCheck = NULL;
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK1);
	pWndCheck->EnableWindow(FALSE);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK6);
	pWndCheck->EnableWindow(FALSE);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK11);
	pWndCheck->EnableWindow(FALSE);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK16);
	pWndCheck->EnableWindow(FALSE);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK21);
	pWndCheck->EnableWindow(FALSE);	
}
