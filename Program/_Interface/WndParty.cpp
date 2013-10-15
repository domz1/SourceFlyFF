#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndParty.h"
#include "WndManager.h"
#include "Party.h"
#include "dpclient.h"
extern CDPClient	g_DPlay;

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA


/****************************************************
  WndId : APP_PARTY - 극단(Party)
  CtrlId : WIDC_TABCTRL1 - TabCtrl
  CtrlId : WIDC_LEAVE - 탈퇴
  CtrlId : WIDC_CHANGE - 극단전환
****************************************************/

CWndParty::CWndParty() 
{ 
	m_WndPartyChangeTroup = NULL;
#if __VER >= 8 //__CSC_VER8_2
	m_pWndPartyQuick = NULL;
#endif //__CSC_VER8_2
	Error( "CWndParty::CWndParty()" );
} 
CWndParty::~CWndParty() 
{ 
#ifdef __FIX_WND_1109
	SAFE_DELETE( m_WndPartyChangeTroup );
#endif	// __FIX_WND_1109
	DeleteDeviceObjects();
} 
void CWndParty::SerializeRegInfo( CAr& ar, DWORD& dwVersion )
{
	Error( "CWndParty::SerializeRegInfo() - IN" );

	CWndNeuz::SerializeRegInfo( ar, dwVersion );
	CWndTabCtrl* lpTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	if( ar.IsLoading() )
	{
		if( dwVersion == 0 )
		{
		}
		else
		if( dwVersion == 1 )
		{
			int nCurSel;
			ar >> nCurSel;
			lpTabCtrl->SetCurSel( nCurSel );
		}
	}
	else
	{
		dwVersion = 1;
		ar << lpTabCtrl->GetCurSel();
	}

	Error( "CWndParty::SerializeRegInfo() - OUT" );
}
void CWndParty::OnDraw( C2DRender* p2DRender ) 
{ 
	if( g_Party.GetSizeofMember() < 2 )
	{
		m_pWndLeave->EnableWindow( FALSE );
		m_pWndTransfer->EnableWindow( FALSE );
#if __VER >= 8 //__CSC_VER8_2
		m_pBtnPartyQuick->EnableWindow( FALSE );
#endif //__CSC_VER8_2
	}
	else
	{
		m_pWndLeave->EnableWindow( TRUE );
		m_pWndTransfer->EnableWindow( TRUE );
#if __VER >= 8 //__CSC_VER8_2
		m_pBtnPartyQuick->EnableWindow( TRUE );
#endif //__CSC_VER8_2
	}

#ifdef __PARTY_DEBUG_0129		// 극단장 튕기는 현상 디버깅 neuz
	if( g_Party.GetSizeofMember() < 2 && g_Party.m_nLevel > 1 )
	{
		m_pWndLeave->EnableWindow( TRUE );
	}
#endif	// __PARTY_DEBUG_0129		// 극단장 튕기는 현상 디버깅 neuz

	if( g_Party.GetLevel() >= 10 && g_Party.IsLeader( g_pPlayer->m_idPlayer ) )
		m_pWndChange->EnableWindow( TRUE );
	else
		m_pWndChange->EnableWindow( FALSE );

	// 파티 정보 출력
	CWndStatic* pWndStatic;
	CString strTemp;
	if(g_Party.m_nKindTroup==0)
	{
		strTemp.Format("%s",prj.GetText(TID_GAME_PARTY1));
	}
	else 
	{
		if( 0 == strlen( g_Party.m_sParty ) )
			strTemp.Format("%s",prj.GetText(TID_GAME_PARTY2));
		else
			strTemp.Format("%s",g_Party.m_sParty);
	}
	pWndStatic = (CWndStatic*)GetDlgItem( WIDC_NAME );
	pWndStatic->SetTitle(strTemp);
	strTemp.Format("%d",g_Party.m_nLevel);
	pWndStatic = (CWndStatic*)GetDlgItem( WIDC_LEVEL );
	pWndStatic->SetTitle(strTemp);
	
	if( g_Party.m_nLevel >= 10 )
	{
		strTemp.Format("%.2f %%", (float)g_Party.m_nExp * 100 / (float)( ( ( 50 + g_Party.GetLevel() ) * g_Party.GetLevel() / 13 ) * 10 ));
	}
	else
	{
		strTemp.Format("%.2f %%", (float)g_Party.m_nExp * 100 / (float)prj.m_aExpParty[g_Party.m_nLevel].Exp );
	}
	pWndStatic = (CWndStatic*)GetDlgItem( WIDC_EXP );
	pWndStatic->SetTitle(strTemp);
	strTemp.Format("%d",g_Party.m_nPoint);
	pWndStatic = (CWndStatic*)GetDlgItem( WIDC_POINT );
	pWndStatic->SetTitle(strTemp);
	// 아이템 경험치 분배방식 상태 갱신
	CWndButton* pWndButton;
	switch( g_Party.m_nTroupsShareExp) 
	{
	case 0:
		{
			pWndButton = (CWndButton*)GetDlgItem( WIDC_EXP_SHARE );
			pWndButton->SetCheck(TRUE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO6 );
			pWndButton->SetCheck(FALSE);
//			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO7 );
//			pWndButton->SetCheck(FALSE);
		}
		break;
	case 1:
		{
			pWndButton = (CWndButton*)GetDlgItem( WIDC_EXP_SHARE );
			pWndButton->SetCheck(FALSE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO6 );
			pWndButton->SetCheck(TRUE);
//			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO7 );
//			pWndButton->SetCheck(FALSE);
		}
		break;
	case 2:
		{
			pWndButton = (CWndButton*)GetDlgItem( WIDC_EXP_SHARE );
			pWndButton->SetCheck(FALSE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO6 );
			pWndButton->SetCheck(FALSE);
//			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO7 );
//			pWndButton->SetCheck(TRUE);
		}
		break;
	}
	switch(g_Party.m_nTroupeShareItem) 
	{
	case 0:
		{
			pWndButton = (CWndButton*)GetDlgItem( WIDC_ITEM_SHARE );
			pWndButton->SetCheck(TRUE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO2 );
			pWndButton->SetCheck(FALSE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO3 );
			pWndButton->SetCheck(FALSE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO4 );
			pWndButton->SetCheck(FALSE);
		}
		break;
	case 1:
		{
			pWndButton = (CWndButton*)GetDlgItem( WIDC_ITEM_SHARE );
			pWndButton->SetCheck(FALSE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO2 );
			pWndButton->SetCheck(TRUE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO3 );
			pWndButton->SetCheck(FALSE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO4 );
			pWndButton->SetCheck(FALSE);
		}
		break;
	case 2:
		{
			pWndButton = (CWndButton*)GetDlgItem( WIDC_ITEM_SHARE );
			pWndButton->SetCheck(FALSE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO2 );
			pWndButton->SetCheck(FALSE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO3 );
			pWndButton->SetCheck(TRUE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO4 );
			pWndButton->SetCheck(FALSE);
		}
		break;
	case 3:
		{
			pWndButton = (CWndButton*)GetDlgItem( WIDC_ITEM_SHARE );
			pWndButton->SetCheck(FALSE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO2 );
			pWndButton->SetCheck(FALSE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO3 );
			pWndButton->SetCheck(FALSE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO4 );
			pWndButton->SetCheck(TRUE);
		}
		break;
	}
} 
void CWndParty::OnInitialUpdate() 
{ 
	Error( "CWndParty::OnInitialUpdate() - IN" );

	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	m_pWndLeave = (CWndButton*)GetDlgItem( WIDC_LEAVE );
	m_pWndChange = (CWndButton*)GetDlgItem( WIDC_CHANGE );
	m_pWndTransfer = (CWndButton*)GetDlgItem( WIDC_BUTTON1 );
#if __VER >= 8 //__CSC_VER8_2
	m_pBtnPartyQuick = (CWndButton*)GetDlgItem( WIDC_BUTTON2 );
#else
	m_pBtnPartyQuick = (CWndButton*)GetDlgItem( WIDC_BUTTON2 );
	m_pBtnPartyQuick->EnableWindow(FALSE);
	m_pBtnPartyQuick->SetVisible(FALSE);
#endif //__CSC_VER8_2
	if( g_Party.GetSizeofMember() < 2 )
	{
		m_pWndTransfer->EnableWindow( FALSE );
		m_pWndLeave->EnableWindow( FALSE );
#if __VER >= 8 //__CSC_VER8_2
		m_pBtnPartyQuick->EnableWindow( FALSE );
#endif	//__CSC_VER8_2
	}
	m_pWndChange->EnableWindow( FALSE );
	
	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	CRect rect = GetClientRect();
	rect.left = 5;
	rect.top = 15;

	//gmpbigsun( 100416 ) : Error 제거 
	//Error( "CWndParty::OnInitialUpdate() - m_wndPartyInfo.Create - IN" );
	m_wndPartyInfo.Create( WBS_CHILD | WBS_NOFRAME | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_PARTY_INFO );
	//Error( "CWndParty::OnInitialUpdate() - m_wndPartyInfo.Create - OUT" );

	//Error( "CWndParty::OnInitialUpdate() - m_wndPartySkill.Create - IN" );
	m_wndPartySkill.Create( WBS_CHILD | WBS_NOFRAME | WBS_NODRAWFRAME , rect, pWndTabCtrl, APP_PARTY_SKILL );
	//Error( "CWndParty::OnInitialUpdate() - m_wndPartySkill.Create - OUT" );

	WTCITEM tabTabItem;
	
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_APP_INFOMATION);//"정보";
	tabTabItem.pWndBase = &m_wndPartyInfo;
	pWndTabCtrl->InsertItem( 0, &tabTabItem );
	
	tabTabItem.pszText = prj.GetText(TID_APP_SKILL);//"스킬";
	tabTabItem.pWndBase = &m_wndPartySkill;
	pWndTabCtrl->InsertItem( 1, &tabTabItem );

	MoveParentCenter();

	Error( "CWndParty::OnInitialUpdate() - OUT" );
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndParty::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	Error( "CWndParty::Initialize() - IN" );
	BOOL bFlag = CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_PARTY, 0, CPoint( 0, 0 ), pWndParent );
	Error( "CWndParty::Initialize() - OUT" );

	// Daisy에서 설정한 리소스로 윈도를 연다.
	return bFlag;
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndParty::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndParty::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndParty::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndParty::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndParty::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndParty::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_LEAVE ) 
	{ // 탈퇴 눌렸다...
		if( g_pPlayer->m_nDuel == 2 )		// 극단듀얼중이면 탈퇴 안됨.
		{
			DWORD dwText = TID_GAME_PPVP_LEAVE;
			g_WndMng.PutString( prj.GetText( dwText ), NULL, prj.GetTextColor( dwText ) );
		} 
		else
		{
			if(m_wndPartyInfo.m_nSelected!=-1) 
			{
				u_long  nLeaveMember = g_Party.m_aMember[m_wndPartyInfo.m_nSelected].m_uPlayerId; //탈퇴시킬놈 ID
				// 여기다가 탈퇴
				if( g_Party.m_nSizeofMember != 0 )
				{
					CWndPartyLeaveConfirm* pWndPartyLeaveConfirm = (CWndPartyLeaveConfirm*)g_WndMng.GetWndBase( APP_PARTYLEAVE_CONFIRM );
					if( pWndPartyLeaveConfirm == NULL )
					{
						g_WndMng.m_pWndPartyLeaveConfirm = new CWndPartyLeaveConfirm;
						g_WndMng.m_pWndPartyLeaveConfirm->Initialize( NULL, APP_PARTYLEAVE_CONFIRM );
						if( g_Party.m_aMember[0].m_uPlayerId == g_pPlayer->m_idPlayer )
							g_WndMng.m_pWndPartyLeaveConfirm->SetLeaveId( nLeaveMember );
						else
							g_WndMng.m_pWndPartyLeaveConfirm->SetLeaveId( g_pPlayer->m_idPlayer );
					}
				}
				else
				{
					// 극단이 구성이 안되어있음
				}
			}
			else
			{
				CWndPartyLeaveConfirm* pWndPartyLeaveConfirm = (CWndPartyLeaveConfirm*)g_WndMng.GetWndBase( APP_PARTYLEAVE_CONFIRM );
				if( pWndPartyLeaveConfirm == NULL )
				{
					g_WndMng.m_pWndPartyLeaveConfirm = new CWndPartyLeaveConfirm;
					g_WndMng.m_pWndPartyLeaveConfirm->Initialize( NULL, APP_PARTYLEAVE_CONFIRM );
					if( g_Party.m_nSizeofMember != 0 )
						g_WndMng.m_pWndPartyLeaveConfirm->SetLeaveId( g_pPlayer->m_idPlayer );
				}
				// 내 자신을 극단에서 탈퇴
			}
			m_wndPartyInfo.m_nSelected=-1;
		}
	}
	// 극단장 인계.
	else if( nID==WIDC_BUTTON1 )
	{
		if( g_Party.m_nSizeofMember != 0 )
		{
			if( g_Party.IsLeader( g_pPlayer->m_idPlayer ) && m_wndPartyInfo.m_nSelected != -1 )
			{
				if( g_Party.m_aMember[m_wndPartyInfo.m_nSelected].m_bRemove == FALSE )
				{
					u_long  nLeadMember = g_Party.m_aMember[m_wndPartyInfo.m_nSelected].m_uPlayerId;
					if( g_Party.IsLeader( nLeadMember ) == FALSE )
					{
						if( g_pPlayer->m_nDuel == 2 )	// 극단 듀얼중일때는 극단장을 바꿀수 없음
							g_WndMng.PutString( prj.GetText( TID_PK_NOCHANGE_PARTYLEADER ), NULL, prj.GetTextColor( TID_PK_NOCHANGE_PARTYLEADER ) );
						else
							g_DPlay.SendPartyChangeLeader( g_pPlayer->m_idPlayer, nLeadMember );						
					}
					else
					{
						g_WndMng.PutString( prj.GetText( TID_GAME_PARTY_ALREADYMASTER ), NULL, prj.GetTextColor( TID_GAME_PARTY_ALREADYMASTER ) ); // "이미 단장을 하고 입니다"
					}
				}
				else
				{
					g_WndMng.PutString( prj.GetText( TID_GAME_MASTER_AWAY ), NULL, prj.GetTextColor( TID_GAME_MASTER_AWAY ) ); // "단장이 아니거나 극단창에서 단원을 선택을 해야 합니다"					
				}
			}
			else
			{
				g_WndMng.PutString( prj.GetText( TID_GAME_PARTY_ISNOTMASTER ), NULL, prj.GetTextColor( TID_GAME_PARTY_ISNOTMASTER ) ); // "단장이 아니거나 극단창에서 단원을 선택을 해야 합니다"
			}
		}
	}
	else if( nID==WIDC_CHANGE )
	{
		if( g_Party.m_nKindTroup == 0 /* && g_Party.m_nLevel == 10 */ )
		{
			// 순회극단으로 바꾸기~
			SAFE_DELETE( m_WndPartyChangeTroup );
			m_WndPartyChangeTroup = new CWndPartyChangeTroup;
#ifdef __FIX_WND_1109
			m_WndPartyChangeTroup->Initialize( this );
#else	// __FIX_WND_1109
			m_WndPartyChangeTroup->Initialize();
#endif	// __FIX_WND_1109
		}
	}
#if __VER >= 8 //__CSC_VER8_2
	else if(nID == WIDC_BUTTON2)
	{
		if(g_Party.GetSizeofMember() >= 2)
		{
			m_pWndPartyQuick = (CWndPartyQuick*)g_WndMng.GetApplet( APP_PARTY_QUICK );
			if(m_pWndPartyQuick == NULL)
			{
				m_pWndPartyQuick = (CWndPartyQuick*)g_WndMng.CreateApplet( APP_PARTY_QUICK );
				m_pWndPartyQuick->SetActiveMember(g_Party.GetSizeofMember());
			}
			else
			{
				m_pWndPartyQuick->Destroy();
				m_pWndPartyQuick = NULL;
			}
		}
	}
#endif //__CSC_VER8_2
	// 순회극단일때만 아이템 경험치 분배방식 갱신
	else
	if( g_Party.IsLeader(g_pPlayer->m_idPlayer) )
	{
		if( g_Party.m_nKindTroup != 0 )
		{
			// 경험치 분배 방식은 순회극단 임
			int nTroupsShareExp = g_Party.m_nTroupsShareExp;
			
			if(nID==WIDC_EXP_SHARE) nTroupsShareExp   = 0;
			else if(nID==WIDC_RADIO6) nTroupsShareExp = 1;
	//		else if(nID==WIDC_RADIO7) nTroupsShareExp = 2;
			
			if( nTroupsShareExp == 2 )
			{
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0004) ) );
				//			g_WndMng.OpenMessageBox( _T("경험치 분배의 동일분배는 아직 지원이 안됩니다.") );
			}
			else
			{
				if( nTroupsShareExp != g_Party.m_nTroupsShareExp )
				{
					// Send
					g_DPlay.SendChangeShareExp( nTroupsShareExp );
				}
			}
		}
		// 아이템 분배 방식은 단막극단 임
		int nTroupeShareItem = g_Party.m_nTroupeShareItem;

		if(nID==WIDC_ITEM_SHARE) nTroupeShareItem  = 0;
		else if(nID==WIDC_RADIO2) nTroupeShareItem = 1;
		else if(nID==WIDC_RADIO3) nTroupeShareItem = 2;
		else if(nID==WIDC_RADIO4) nTroupeShareItem = 3;		

		if( nTroupeShareItem != g_Party.m_nTroupeShareItem )
		{
			// Send
			g_DPlay.SendChangeShareItem( nTroupeShareItem );
			
		}
	}
	else
	{
		if( nID != WTBID_CLOSE && nID != WTBID_HELP )
			g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0008) ) );
//			g_WndMng.OpenMessageBox( "단장이 아니거나 순회극단이 아니므로 변경할수 없습니다." );
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

/****************************************************
  WndId : APP_PARTY_INFO - 개인분배
  CtrlId : WIDC_GROUPBOX1 - 극단정보
  CtrlId : WIDC_GROUPBOX3 - 경험치분배
  CtrlId : WIDC_GROUPBOX4 - 아이템분배
  CtrlId : WIDC_ITEM_SHARE - 개인분배
  CtrlId : WIDC_RADIO2 - 순서분배
  CtrlId : WIDC_RADIO3 - 수동분배
  CtrlId : WIDC_RADIO4 - 랜던분배
  CtrlId : WIDC_EXP_SHARE - 동일분배
  CtrlId : WIDC_RADIO6 - 기여도분배
  CtrlId : WIDC_RADIO7 - 피해분배
  CtrlId : WIDC_STATIC1 - 극단명칭 :
  CtrlId : WIDC_STATIC2 - 극단레벨 :
  CtrlId : WIDC_STATIC3 - 극단경험치 :
  CtrlId : WIDC_STATIC4 - 극단포인트 :
  CtrlId : WIDC_NAME - 마스커레이드
  CtrlId : WIDC_LEVEL - 1
  CtrlId : WIDC_EXP - 90
  CtrlId : WIDC_POINT - 0
****************************************************/

CWndPartyInfo::CWndPartyInfo() 
{ 
	m_nSelected=-1;
	m_pVBGauge = NULL;
} 
CWndPartyInfo::~CWndPartyInfo() 
{ 
	DeleteDeviceObjects();
} 
void CWndPartyInfo::OnDraw( C2DRender* p2DRender ) 
{
	CString strTemp;
	
	// 현재 선택된 놈 표시
	if( m_nSelected != -1 ) 
	{
		CRect rect( 5, 8 + m_nSelected * 15, 410, 22 + m_nSelected * 15 );
		p2DRender->RenderFillRect( rect, 0x60ffff00 );
	}
	// 파티원 정보 출력
	if( g_Party.GetSizeofMember() < m_nSelected + 1 ) 
	{
		m_nSelected = -1;
	}
	int y = 10;

#if __VER >= 8 //__CSC_VER8_2
	DWORD dwColor;
	for( int i = 0; i < g_Party.m_nSizeofMember; i++ ) 
	{		
		dwColor = 0xffffffff;//adeilson 0xff000000
#else 
	DWORD dwColor = 0xffffffff;//adeilson 0xff000000
	for( int i = 0; i < g_Party.m_nSizeofMember; i++ ) 
	//for( int i = 0; i < 8; i++ )//g_Party.m_nSizeofMember; i++ ) 
	{
#endif //__CSC_VER8_2
		CMover* pObjMember	= prj.GetUserByID( g_Party.m_aMember[ i ].m_uPlayerId );

		CRect rectTemp,rect;
		CString strClass(prj.GetText(TID_GAME_MEMBER));
		CString strName;
		if( i == 0 ) strClass = prj.GetText(TID_GAME_LEADER); // 맨 위에놈이 단장
		rectTemp = CRect( 10, y, 20, y + 10 );
		p2DRender->RenderFillRect( rectTemp, dwColor );
		rectTemp = CRect( 11, y + 1, 19, y + 9 );

		// 상태에 따라 색 변경
		DWORD colorStatus = 0xff0000ff; // 디폴트는 이상없음
		if( IsValidObj(pObjMember) )
		{
			if( pObjMember->GetHitPoint() == 0 ) 
				colorStatus = 0xffff0000; // 죽은놈
			else 
			{
				if( pObjMember->GetMaxHitPoint() > 0 && ( (FLOAT)pObjMember->GetHitPoint() ) / ( (FLOAT)pObjMember->GetMaxHitPoint() ) <.1f ) 
					colorStatus = 0xffffff00; // HP 10% 이하인놈
			}
		}
		else
		{
			colorStatus = 0xff878787; // 디폴트는 주위에 없는놈
			if( g_Party.m_aMember[ i ].m_bRemove ) 
				colorStatus = 0xffffffff;//ADEILSON 0xff000000 // 서버에 없는놈
		}
		p2DRender->RenderFillRect( rectTemp, colorStatus );

#if __VER >= 8 //__CSC_VER8_2
		u_long  nLeadMember = g_Party.m_aMember[i].m_uPlayerId;
		if(g_Party.IsLeader(nLeadMember)) //Leader Color Set
			dwColor = 0xff1fb72d;
#endif	//__CSC_VER8_2	
		BOOL bSummary	= FALSE;
		int nLang	= ::GetLanguage();
		switch( nLang )
		{
			case LANG_GER:
			case LANG_FRE:
				bSummary	= TRUE;
				break;
		}

		// 레벨 계급 직업
		if( IsValidObj(pObjMember) )
		{
#if __VER >= 10 // __LEGEND
			if( MAX_PROFESSIONAL <= pObjMember->GetJob() && pObjMember->GetJob() < MAX_MASTER )
				strTemp.Format( "%d%s", pObjMember->GetLevel(), prj.GetText( TID_GAME_TOOLTIP_MARK_MASTER ) );
			else if( MAX_MASTER <= pObjMember->GetJob() )
#if __VER >= 15 // __HERO129_VER15 // 15차 히어로 레벨확장
				strTemp.Format( "%d%s", pObjMember->GetLevel(), prj.GetText( TID_GAME_TOOLTIP_MARK_HERO ) );
#else // 15차 히어로 레벨확장
				strTemp = prj.GetText( TID_GAME_TOOLTIP_MARK_HERO_BEFORE );
#endif // 15차 히어로 레벨확장
			else 
				strTemp.Format( "%d", pObjMember->GetLevel() );
			p2DRender->TextOut( 30, y, 1.0f, 1.0f, strTemp, dwColor );
			if( !bSummary )
			{
				strTemp.Format( "%s", strClass );
				p2DRender->TextOut( 80, y, 1.0f, 1.0f, strTemp, dwColor );
				strTemp.Format( "%s", pObjMember->GetJobString() );
				p2DRender->TextOut( 120, y, 1.0f, 1.0f, strTemp, dwColor );
			}
			else
			{
				strTemp.Format( "%s", pObjMember->GetJobString() );
				p2DRender->TextOut( 80, y, 1.0f, 1.0f, strTemp, dwColor );
			}
			strName		= pObjMember->GetName();
#else //__LEGEND
			strTemp.Format( "%d", pObjMember->GetLevel() );
			p2DRender->TextOut( 30, y, 1.0f, 1.0f, strTemp, dwColor );
			if( !bSummary )
			{
				strTemp.Format( "%s", strClass );
				p2DRender->TextOut( 50, y, 1.0f, 1.0f, strTemp, dwColor );
				strTemp.Format( "%s", pObjMember->GetJobString() );
				p2DRender->TextOut( 80, y, 1.0f, 1.0f, strTemp, dwColor );
			}
			else
			{
				strTemp.Format( "%s", pObjMember->GetJobString() );
				p2DRender->TextOut( 50, y, 1.0f, 1.0f, strTemp, dwColor );
			}
			strName = pObjMember->GetName();
#endif //__LEGEND
		}
		else
		{
#if __VER >= 10 // __LEGEND
#if __VER >= 11 // __SYS_PLAYER_DATA
			PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->GetPlayerData( g_Party.m_aMember[i].m_uPlayerId );
			int nLevel	= pPlayerData->data.nLevel;
			int nJob	= pPlayerData->data.nJob;
#else	// __SYS_PLAYER_DATA
			int nLevel	= g_Party.m_aMember[i].m_nLevel;
			int nJob	= g_Party.m_aMember[i].m_nJob;
#endif	// __SYS_PLAYER_DATA
			if( MAX_PROFESSIONAL <= nJob && nJob < MAX_MASTER )
				strTemp.Format( "%d%s", nLevel, prj.GetText( TID_GAME_TOOLTIP_MARK_MASTER ) );
			else if( MAX_MASTER <= nJob )
#if __VER >= 15 // __HERO129_VER15 // 15차 히어로 레벨확장
				strTemp.Format( "%d%s", nLevel, prj.GetText( TID_GAME_TOOLTIP_MARK_HERO ) );
#else // 15차 히어로 레벨확장
				strTemp = prj.GetText( TID_GAME_TOOLTIP_MARK_HERO_BEFORE );
#endif // 15차 히어로 레벨확장
			else 
				strTemp.Format( "%d", nLevel );
			p2DRender->TextOut(30,y,1.0f,1.0f,strTemp,dwColor);
			if( !bSummary )
			{
				strTemp.Format("%s",strClass);
				p2DRender->TextOut(80,y,1.0f,1.0f,strTemp,dwColor);
				strTemp.Format("%s",prj.m_aJob[nJob].szName);
				p2DRender->TextOut(120,y,1.0f,1.0f,strTemp,dwColor);
			}
			else
			{
				strTemp.Format( "%s", prj.m_aJob[nJob].szName );
				p2DRender->TextOut( 80, y, 1.0f, 1.0f, strTemp, dwColor );
			}
#if __VER >= 11 // __SYS_PLAYER_DATA
			strName		= pPlayerData->szPlayer;
#else	// __SYS_PLAYER_DATA
			strName		= g_Party.m_aMember[i].m_szName;
#endif	// __SYS_PLAYER_DATA
#else //__LEGEND
			strTemp.Format("%d",g_Party.m_aMember[i].m_nLevel);
			p2DRender->TextOut(30,y,1.0f,1.0f,strTemp,dwColor);
			if( !bSummary )
			{
				strTemp.Format("%s",strClass);
				p2DRender->TextOut(50,y,1.0f,1.0f,strTemp,dwColor);
				strTemp.Format("%s",prj.m_aJob[g_Party.m_aMember[i].m_nJob].szName);
				p2DRender->TextOut(80,y,1.0f,1.0f,strTemp,dwColor);
			}
			else
			{
				strTemp.Format( "%s", prj.m_aJob[g_Party.m_aMember[i].m_nJob].szName );
				p2DRender->TextOut( 50,y,1.0f,1.0f,strTemp,dwColor );
			}
			strName		= g_Party.m_aMember[i].m_szName;
#endif //__LEGEND
		}
		// 이름은 10바이트째에서 짜른다
		if( strName.GetLength() > 8 ) 
		{
			int	nReduceCount = 0;

			for( nReduceCount= 0; nReduceCount < 8; )
			{
				if( IsDBCSLeadByte( strName[ nReduceCount ] ) )
					nReduceCount+=2;
				else
					nReduceCount++;
			}

			strName = strName.Left( nReduceCount ); // 10바이트째에서 한글이 두동강날거같으면 9바이트에서 짜르기
			strName+="..."; // 이름 짤린놈은 이게 끝이 아니라고 표시하자... 기획에는 없지만 내멋대로 붙였음
		}
		strTemp.Format("%s",strName);
#if __VER >= 10		// __LEGEND
		p2DRender->TextOut( 180, y, 1.0f, 1.0f, strTemp, dwColor );
#else //__LEGEND
		p2DRender->TextOut( 135, y, 1.0f, 1.0f, strTemp, dwColor );	// 50
#endif //__LEGEND
		
		// HP 게이지
		int nWidth	= ( IsValidObj( pObjMember ) ? 110 * pObjMember->GetHitPoint() / pObjMember->GetMaxHitPoint() : 0 );
		if( !bSummary )
			rect.SetRect( 280, y, 280 + 110, y + 12 );
		else
			rect.SetRect( 310, y, 310 + 110, y + 12 );	// 오른쪽으로 30 이동

		rectTemp = rect; 
		rectTemp.right = rectTemp.left + nWidth;
		//rectTemp.DeflateRect( 1, 1 );
		//if( rectTemp.Width() > 5 )
		//	m_pTheme->RenderWndGauge( p2DRender, &rectTemp, D3DCOLOR_ARGB( 255, 255, 0, 0 ) );
		//m_pTheme->RenderWndGauge( p2DRender, &rect,  D3DCOLOR_ARGB( 100, 255, 255, 255 )  );
		m_pTheme->RenderGauge( p2DRender, &rect, 0xffffffff, m_pVBGauge, &m_texGauEmptyNormal );
		if( IsValidObj(pObjMember) )
			m_pTheme->RenderGauge( p2DRender, &rectTemp, 0x64ff0000, m_pVBGauge, &m_texGauFillNormal );
	
		y+=15; // 다음줄
	}
} 
HRESULT CWndPartyInfo::RestoreDeviceObjects()
{
	CWndNeuz::RestoreDeviceObjects();
	if( m_pVBGauge == NULL )
		return m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBGauge, NULL );
	return S_OK;
}
HRESULT CWndPartyInfo::InvalidateDeviceObjects()
{
	CWndNeuz::InvalidateDeviceObjects();
    SAFE_RELEASE( m_pVBGauge );
	return S_OK;
}
HRESULT CWndPartyInfo::DeleteDeviceObjects()
{
	CWndNeuz::DeleteDeviceObjects();
	InvalidateDeviceObjects();
	return S_OK;
}

void CWndPartyInfo::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	RestoreDeviceObjects();
	m_texGauEmptyNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptyNormal.bmp" ), 0xffff00ff, TRUE );
	m_texGauFillNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptyNormal.bmp" ), 0xffff00ff, TRUE );
	
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndPartyInfo::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_PARTY_INFO, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndPartyInfo::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndPartyInfo::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndPartyInfo::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndPartyInfo::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndPartyInfo::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
#if __VER >= 11 // __CSC_VER11_2
	CWndTaskBar* pTaskBar = g_WndMng.m_pWndTaskBar;
	if(((CWndWorld*)g_WndMng.m_pWndWorld)->m_bAutoAttack || pTaskBar->m_nExecute != 0)
		return;
#endif //__CSC_VER11_2
	
	// 파티창에서 어떤놈 눌렀는지 색출
	CRect rect( 10, 10, 365, 10 + g_Party.GetSizeofMember() * 15 );
	if( rect.PtInRect( point ) ) 
		m_nSelected = ( point.y - 10 ) / 15;
	else 
		m_nSelected = -1;
	if( m_nSelected != -1 ) 
	{ // 선택된놈 있으면
		// 그놈을 타겟으로
#if __VER >= 8 //__CSC_VER8_2
		((CWndWorld*)g_WndMng.m_pWndWorld)->m_pSelectRenderObj = NULL; //마우스에 걸려 그려진놈을 지우고 타겟을 잡아야 안겹친다..
#endif //__CSC_VER8_2
		CMover* pObjMember = prj.GetUserByID( g_Party.m_aMember[m_nSelected].m_uPlayerId );
		if(g_pPlayer!=pObjMember) 
		{
			if(IsValidObj(pObjMember)) 
			{ // 화면에 없는놈은 패스...
				g_WorldMng()->SetObjFocus(pObjMember);
				CWndWorld* pWndWorld = g_WndMng.m_pWndWorld;
				if(pWndWorld)
					pWndWorld->m_pRenderTargetObj = NULL;
			}
		}
#if __VER >= 8 //__CSC_VER8_2
		else
			g_WorldMng()->SetObjFocus( NULL );
#endif //__CSC_VER8_2
	}
	else 
	{
		// 없으면 타겟 지운다
		g_WorldMng()->SetObjFocus(NULL);
	}
} 
BOOL CWndPartyInfo::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

/****************************************************
  WndId : APP_PARTY_SKILL - 극단스킬
****************************************************/

CWndPartySkill::CWndPartySkill() 
{ 
	ZeroMemory( m_atexSkill, sizeof( m_atexSkill ) );
	m_nSkillSelect = -1;
} 
CWndPartySkill::~CWndPartySkill() 
{ 
} 
void CWndPartySkill::OnDraw( C2DRender* p2DRender ) 
{ 
//	if( g_Party.m_nKindTroup )
	{
		CRect rect = GetClientRect();
		int nWidth = rect.Width() / 3;
		int nHeight = rect.Height() / 3;
		int nCount = 0;
		for( int i = 0; i < 3; i++ )
		{
			for( int j = 0; j < 3; j++, nCount++ )
			{
				ItemProp* pItemProp =  prj.GetPartySkill( nCount + 1 );
				if( pItemProp ) //&& g_Party.m_nSizeofMember >= 2 )
				{
					m_atexSkill[ i * 3 + j ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, pItemProp->szIcon/*pItemBase->GetProp()->szIcon*/), 0xffff00ff, FALSE );
					CPoint point1 = CPoint( j * nWidth + 35 , i * nHeight + 6 );
					CPoint point2 = CPoint( j * nWidth + 3, i * nHeight + 3 );
					
					if( g_Party.m_nKindTroup && int(g_Party.GetLevel() - pItemProp->dwReqDisLV) >= 0 )
					{
						if( (g_Party.GetPoint() - pItemProp->dwExp) >= 0 )
						{
							// 사용가능한 기술
							p2DRender->TextOut( point1.x, point1.y, pItemProp->szName, 0xffffffff );//ADEILSON 0xff000000
							//m_aSlotQueue[nIndex].m_pTexture = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, lpSkill->GetProp()->szIcon/*pItemBase->GetProp()->szIcon*/), 0xffff00ff, FALSE );
							p2DRender->RenderTexture( point2, m_atexSkill[ i * 3 + j ] );
							
						}
						else
						{
							// 기술은 익혔으나 포인트가 모자라 사용을 못함
							p2DRender->TextOut( point1.x, point1.y, pItemProp->szName, 0xff0000ff );
							p2DRender->RenderTexture( point2, m_atexSkill[ i * 3 + j ] );
							
						}
					}
					else
					{
						// 기술을 못익혔음
						p2DRender->TextOut( point1.x, point1.y, pItemProp->szName, 0xffff0000 );
						p2DRender->RenderTexture( point2, m_atexSkill[ i * 3 + j ], 54 );		
					}
				}
			}
		}
	}
} 
void CWndPartySkill::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
	CRect rect = GetClientRect();
	int nWidth = rect.Width() / 3;
	int nHeight = rect.Height() / 3;
	point.x /= nWidth; 
	point.y /= nHeight;
	int nSkill = point.y * 3 + point.x;
	
	ItemProp* pItemProp =  prj.GetPartySkill( nSkill + 1 );
	if( pItemProp )
	{
		m_nSkillSelect = nSkill;
		return;
	}
	m_nSkillSelect = -1;
} 
void CWndPartySkill::OnMouseWndSurface( CPoint point )
{
	CRect rect = GetClientRect();
	int nWidth = rect.Width() / 3;
	int nHeight = rect.Height() / 3;
	int nCount = 0;
	for( int i = 0; i < 3; i++ )
	{
		for( int j = 0; j < 3; j++, nCount++ )
		{
			ItemProp* pItemProp =  prj.GetPartySkill( nCount + 1 );
			if( pItemProp ) //&& g_Party.m_nSizeofMember >= 2 )
			{
				CRect rect( j * nWidth + 3, i * nHeight + 3, j * nWidth + 3 + nWidth, i * nHeight + 3 + nHeight);
				if( rect.PtInRect( point ) )
				{
					CPoint point2 = point;
					ClientToScreen( &point2 );
					ClientToScreen( &rect );
					
					g_WndMng.PutToolTip_Troupe( i * 3 + j + 1, point2, &rect );
				}
			}
		}
	}
}	
void CWndPartySkill::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
	m_nSkillSelect = -1;
}

void CWndPartySkill::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_nSkillSelect == -1 || IsPush() == FALSE )
		return;
	//	TRACE( " hello \n" );
	CPoint pt( 3, 3 );
	CRect rect;
	//if( m_rect.PtInRect( point ) )
	//{
	//DWORD dwSkill = m_apJobSkill[ m_nCurSelect ].dwSkill;
	ItemProp* pItemProp =  prj.GetPartySkill( m_nSkillSelect + 1 );
	if( pItemProp ) //&& g_Party.m_nSizeofMember >= 2 )
	{
		if( int(g_Party.GetLevel() - pItemProp->dwReqDisLV) >= 0 )
		{
			//CItemElem* pItemElem = &m_pItemContainer->m_apItem[ m_pItemContainer->m_apIndex[ m_nCurSelect ] ] ;
			m_GlobalShortcut.m_dwShortcut  = SHORTCUT_SKILL;
			m_GlobalShortcut.m_dwType  = 2;//m_nJob;//(DWORD)pItemElem;//->m_dwItemId;
			m_GlobalShortcut.m_dwIndex = m_nSkillSelect + 1;//m_nSkillSelect;//m_nCurSelect;//(DWORD)pItemElem;//->m_dwItemId;
			m_GlobalShortcut.m_dwData  = 0;//pItemElem->m_dwObjId;//(DWORD)pItemElem;
			m_GlobalShortcut.m_dwId     = m_nSkillSelect + 1; // 컬런트 셀렉트가 곧 ID나 마찬가지임.
			m_GlobalShortcut.m_pTexture = m_atexSkill[ m_nSkillSelect ];//L;//pItemElem->m_pTexture;
			_tcscpy( m_GlobalShortcut.m_szString, pItemProp->szName);
			//}
		}
	}
}

void CWndPartySkill::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndPartySkill::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_PARTY_SKILL, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndPartySkill::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndPartySkill::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndPartySkill::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndPartySkill::OnLButtonDblClk( UINT nFlags, CPoint point)
{
//	if( g_Party.m_nSizeofMember >= 2 && g_Party.m_nKindTroup == 1 )
	{
		CRect rect = GetClientRect();
		int nWidth = rect.Width() / 3;
		int nHeight = rect.Height() / 3;
		point.x /= nWidth; 
		point.y /= nHeight;
		int nSkill = point.y * 3 + point.x;
		
		ItemProp* pItemProp =  prj.GetPartySkill( nSkill + 1 );
		if( pItemProp )
		{
			g_WndMng.ObjectExecutor( SHORTCUT_SKILL, nSkill + 1, 2 );
		}
	}
}
BOOL CWndPartySkill::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
