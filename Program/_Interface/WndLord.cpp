
#include "stdafx.h"
#include "resData.h"
#include "WndLord.h"
#include "DPClient.h"
#include "defineText.h"
#include "playerdata.h"

#if __VER >= 12 // __LORD

#include "definelordskill.h"
#include "lord.h"

extern	CDPClient			g_DPlay;

/****************************************************
  WndId : APP_LORD_STATE - 후보 등록 현황
  CtrlId : WIDC_STATIC1 - Static
  CtrlId : WIDC_LISTBOX1 - Listbox
  CtrlId : WIDC_STATIC2 - Level
  CtrlId : WIDC_STATIC3 - Name
  CtrlId : WIDC_STATIC4 - Class
  CtrlId : WIDC_STATIC5 - Pledge
  CtrlId : WIDC_BUTTON1 - Button
  CtrlId : WIDC_STATIC6 - Static
****************************************************/

CWndLordState::CWndLordState() 
{ 
	m_tmRefresh =  g_tmCurrent;
	m_pWndPledge = NULL;
	m_nSelect = 0;
} 
CWndLordState::~CWndLordState() 
{ 
	SAFE_DELETE(m_pWndPledge);
} 
void CWndLordState::OnDraw( C2DRender* p2DRender ) 
{ 
	CCElection* pElection	= static_cast<CCElection*>( CCLord::Instance()->GetElection() );
	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	//CString		strClass, strName;
	LPWNDCTRL	pCustom = NULL;
	DWORD	dwColor;

	pCustom = GetWndCtrl( WIDC_LISTBOX1 );
	dwColor = D3DCOLOR_ARGB( 255, 0, 0, 0 )/*adeilson*/;

	for(int i = 1; i < 11; ++i)
	{
		SPC pRanker = pElection->GetRanker(i);
		if(pRanker)
		{
			if(pRanker->GetIdPlayer() > 0)
			{	
				if(!strName[i-1].IsEmpty())  p2DRender->TextOut( pCustom->rect.left + 15, pCustom->rect.top + 8 + (i-1)*16, strName[i-1], dwColor);
				if(!strClass[i-1].IsEmpty()) p2DRender->TextOut( pCustom->rect.left + 220, pCustom->rect.top + 8 + (i-1)*16, strClass[i-1], dwColor);
				p2DRender->TextOut( pCustom->rect.left + 313, pCustom->rect.top + 8 + (i-1)*16, "open", dwColor);

				if(pWndListBox && i == m_nSelect)
				{
					CRect rect;
					rect.left = pCustom->rect.left;
					rect.right = pCustom->rect.right - 4;
					rect.top = pCustom->rect.top + 6 + (i-1)*16;
					rect.bottom = rect.top + 16;
					D3DXCOLOR color =  D3DCOLOR_ARGB( 60, 240, 0, 0 );
					p2DRender->RenderFillRect( rect, color );		
				}
			}
		}
	}
}

BOOL CWndLordState::Process()
{
	// 2초에 한번씩만 갱신한다
	if(m_tmRefresh > g_tmCurrent) return FALSE;

	m_tmRefresh = g_tmCurrent + 1000;

	CCElection* pElection	= static_cast<CCElection*>( CCLord::Instance()->GetElection() );
	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );

	pWndListBox->ResetContent();
	if(pElection->IsValid())	((CWndStatic*)GetDlgItem( WIDC_LIVE ))->SetTitle(prj.GetText(TID_GAME_LORD_MINREQ_OK));
	else						((CWndStatic*)GetDlgItem( WIDC_LIVE ))->SetTitle(prj.GetText(TID_GAME_LORD_MINREQ_NO));
	
	
	for(int i = 1; i < 11; ++i)
	{
		SPC pRanker = pElection->GetRanker(i);
		if(pRanker)
		{
			if(pRanker->GetIdPlayer() > 0)
			{
				PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->GetPlayerData(pRanker->GetIdPlayer());
				// 순위
				if(i == 10)		strName[i-1].Format("  %d       %s", i, pPlayerData->szPlayer);
				else			strName[i-1].Format("  %d        %s", i, pPlayerData->szPlayer);
				strClass[i-1].Format("%s", prj.m_aJob[pPlayerData->data.nJob].szName);
				pWndListBox->AddString("                                                                             ");
			}
		}
		else
		{
			strName[i-1].Empty();
			strClass[i-1].Empty();
		}
	}

	return TRUE;
}

void CWndLordState::OnInitialUpdate() 
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
BOOL CWndLordState::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_LORD_STATE, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndLordState::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndLordState::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndLordState::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndLordState::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndLordState::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndLordState::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch(nID)
	{
		case WIDC_LISTBOX1: // view ctrl
			{
				CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
				SAFE_DELETE(m_pWndPledge);
				m_pWndPledge = new CWndLordPledge(pWndListBox->GetCurSel() + 1);
				m_pWndPledge->Initialize(this);
				m_nSelect = pWndListBox->GetCurSel() + 1;
				//m_pWndPledge->SetRanker(pWndListBox->GetCurSel() + 1);
			}
			break;

		case WIDC_BUTTON1:// ok버튼
			Destroy();
			break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 




// CWndLordPledge
CWndLordPledge::CWndLordPledge() 
{ 
	m_bIsFirst		= true;
	m_pWndConfirm	= NULL; 
} 
CWndLordPledge::CWndLordPledge(int nRank)
{
	m_bIsFirst		= true;
	m_pWndConfirm	= NULL; 
	SetRanker(nRank);
}
CWndLordPledge::~CWndLordPledge() 
{ 
} 
void CWndLordPledge::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndLordPledge::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	pWndEdit->SetWndRect( pWndEdit->GetWindowRect( TRUE ), FALSE);
	pWndEdit->AddWndStyle( EBS_AUTOVSCROLL );
	pWndEdit->AddWndStyle( EBS_WANTRETURN );
	if(m_pRanker->GetPledge()) pWndText->SetString(m_pRanker->GetPledge());

	// 에디터는 일단 숨긴다
	pWndEdit->SetVisible(FALSE);

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndLordPledge::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_LORD_PLEDGE, 0, CPoint( 0, 0 ), pWndParent );
} 

void CWndLordPledge::SetPledge(LPCTSTR strPledge)
{
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	pWndText->SetString(strPledge);
}
/*
  직접 윈도를 열때 사용 
BOOL CWndLordState::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndLordPledge::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	// 자기자신만 더블클릭으로 공약을 수정할 수 있다
	if(dwMessage == WM_LBUTTONDBLCLK && nID == WIDC_TEXT1 && g_pPlayer->m_idPlayer == m_pRanker->GetIdPlayer())
	{
		CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
		CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
		if(pWndText->m_string.IsEmpty()) m_bIsFirst = true;
		else							 m_bIsFirst = false;
		pWndEdit->SetString(pWndText->m_string);
		pWndEdit->SetVisible(TRUE);
		pWndEdit->SetFocus();
		pWndText->SetVisible(FALSE);
	}
	
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndLordPledge::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndLordPledge::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndLordPledge::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndLordPledge::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch(nID)
	{
		// 공약을 전송한다
		case WIDC_BUTTON2:
			CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
			CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
			CString	  strTemp  = pWndEdit->GetString();
			if(strTemp.GetLength() > 255) 
			{
				g_WndMng.OpenMessageBox(prj.GetText(TID_GUILD_NOTICE_ERROR));
				return FALSE;
			}
			if(!m_bIsFirst) 
			{
				//if(g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_MOD_PLEGDE), MB_OKCANCEL, this) == MB_OK)
				//g_DPlay.SendElectionSetPledge(pWndEdit->GetString());
				SAFE_DELETE(m_pWndConfirm);
				m_pWndConfirm = new CWndLordConfirm(pWndEdit->GetString());
				m_pWndConfirm->Initialize();
			}
			else
			{
				if(!((CEditString)pWndEdit->GetString()).IsEmpty())
				{
					g_DPlay.SendElectionSetPledge(pWndEdit->GetString());
				}
				Destroy();
			}
			break;
	};
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

BOOL CWndLordPledge::SetRanker(int nRank)
{
	CCElection* pElection	= static_cast<CCElection*>( CCLord::Instance()->GetElection() );
	m_pRanker = pElection->GetRanker(nRank);

	if(m_pRanker) return TRUE;
	else		return FALSE;

}



CWndLordTender::CWndLordTender() 
{ 
} 
CWndLordTender::~CWndLordTender() 
{ 
} 
void CWndLordTender::OnDraw( C2DRender* p2DRender ) 
{ 
	CCElection* pElection	= static_cast<CCElection*>( CCLord::Instance()->GetElection() );
	CWndText*	pWndText = (CWndText*)GetDlgItem( WIDC_TEXT2 );
	CString		strTemp, strResult;

	pWndText->ResetString();
	
	for(int i = 1; i < (int)( pElection->GetCandidatesSize() + 1 ); ++i)
	{
		SPC pRanker = pElection->GetRanker(i);
		if(pRanker)
		{
			if(pRanker->GetIdPlayer() > 0)
			{
				PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->GetPlayerData(pRanker->GetIdPlayer());
				if(pPlayerData->szPlayer)
				{
					if(i  >= 10) strTemp.Format("  %d           %s \n", i, pPlayerData->szPlayer);
					else         strTemp.Format("  %d            %s \n", i, pPlayerData->szPlayer);
				}
				else
					strTemp.Format("  %d            %s \n", i, "....");
				strResult.Insert(INT_MAX, strTemp);
			}
		}
	}
	pWndText->SetString(strResult);
} 

void CWndLordTender::RefreshDeposit()
{

	CCElection* pElection	= static_cast<CCElection*>( CCLord::Instance()->GetElection() );
	CWndEdit*	pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );

	for(int i = 0; i <= (int)( pElection->GetCandidatesSize() ); ++i)
	{
		SPC pRanker = pElection->GetRanker(i);
		if(pRanker)
		{
			if(pRanker->GetIdPlayer() == g_pPlayer->m_idPlayer)
			{
				CString		strDeposit;
				strDeposit.Format("%I64d", pRanker->GetDeposit());
				pWndEdit->SetString(strDeposit);
			}
		}
	}

}

void CWndLordTender::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	CWndText*	pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );		
	CScript		scanner;

	if(scanner.Load( MakePath( DIR_CLIENT,  _T( "lordcandidate.inc" ) )))
		pWndText->SetString(scanner.m_pProg);

	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	pWndEdit->SetString("0");
	RefreshDeposit();
	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndLordTender::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_LORD_TENDER, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndLordState::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndLordTender::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndLordTender::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndLordTender::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndLordTender::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndLordTender::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BUTTON1 )
	{
		if( g_pPlayer )
		{
			CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
			
			DWORD nCost;
			CString str = pWndEdit->GetString();
			nCost = atol( str );
			
			if( nCost >= 100000000)
			{
				g_DPlay.SendElectionAddDeposit(nCost);
			}
			else
			{
				g_WndMng.OpenMessageBox(prj.GetText(TID_UPGRADE_ERROR_NOMONEY));
			}
		}
	}
	else if( nID == WIDC_BUTTON2 )
	{
		Destroy();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


CWndLordVote::CWndLordVote() 
{ 
	m_tmRefresh = g_tmCurrent;
} 
CWndLordVote::~CWndLordVote() 
{ 
} 

void CWndLordVote::OnDraw( C2DRender* p2DRender ) 
{ 
	CCElection* pElection	= static_cast<CCElection*>( CCLord::Instance()->GetElection() );
	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	CString		strLevel, strName, strClass;
	LPWNDCTRL	pCustom = NULL;
	DWORD		dwColor;

	pCustom = GetWndCtrl( WIDC_LISTBOX1 );
	dwColor = D3DCOLOR_ARGB( 255, 0, 0, 0 )/*adeilson*/;

	for(int i = 1; i < 11; ++i)
	{
		SPC pRanker = pElection->GetRanker(i);
		if(pRanker)
		{
			if(pRanker->GetIdPlayer() > 0)
			{	
				PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->GetPlayerData(pRanker->GetIdPlayer());
				// 레벨
				strLevel.Format("%d", pPlayerData->data.nLevel);
				// 이름
				strName.Format("%s", pPlayerData->szPlayer);
				// 클래스
				strClass.Format("%s", prj.m_aJob[pPlayerData->data.nJob].szName);
				p2DRender->TextOut( pCustom->rect.left + 15, pCustom->rect.top + 8 + (i-1)*16, strLevel, dwColor);
				p2DRender->TextOut( pCustom->rect.left + 60, pCustom->rect.top + 8 + (i-1)*16, strName, dwColor);
				p2DRender->TextOut( pCustom->rect.left + 220, pCustom->rect.top + 8 + (i-1)*16, strClass, dwColor);
			}
		}
	}
} 

BOOL CWndLordVote::Process()
{/*
	// 2초에 한번씩만 갱신한다
	if(m_tmRefresh > g_tmCurrent) return FALSE;

	m_tmRefresh = g_tmCurrent + 1000;
*/
	return TRUE;
}

void CWndLordVote::OnInitialUpdate() 
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
BOOL CWndLordVote::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_LORD_VOTE, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndLordState::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndLordVote::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndLordVote::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndLordVote::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndLordVote::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndLordVote::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	CCElection*		pElection	= static_cast<CCElection*>( CCLord::Instance()->GetElection() );
	CWndListBox*	pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	CWndEdit*		pWndEdit	= (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	SPC				pRanker		= pElection->GetRanker(pWndListBox->GetCurSel() + 1);
	
	switch(nID)
	{
		case WIDC_LISTBOX1:
			/*// 리스트를 클릭하면 에디트박스에 이름이 올라간다
			
			if(pRanker)
			{
				if(pRanker->GetIdPlayer() > 0)
				{
					PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->GetPlayerData(pRanker->GetIdPlayer());
					CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
					pWndEdit->SetString(pPlayerData->szPlayer);
				}
			}*/
			break;

		case WIDC_BUTTON1:
			// 투표를 함
			if(((CEditString)pWndEdit->GetString()).IsEmpty()) return FALSE;
			for(int i = 1; i < 11; ++i)
			{
				SPC pRanker = pElection->GetRanker(i);
				if(pRanker)
				{
					if(pRanker->GetIdPlayer() > 0)
					{
						PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->GetPlayerData(pRanker->GetIdPlayer());
						CString strRanker(pPlayerData->szPlayer);
						if(((CEditString)pWndEdit->GetString()).Compare(strRanker) == 0)
							g_DPlay.SendElectionIncVote(pRanker->GetIdPlayer());
					}
				}
			}
			// 이름이 일치하는 후보가 없음
			// g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_NO_CANDIDATE));
			break;
	};
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

// 이벤트 창
CWndLordEvent::CWndLordEvent()
{
	m_nEEvent = 0;
	m_nDEvent = 0;
}

CWndLordEvent::~CWndLordEvent()
{

}

BOOL CWndLordEvent::Initialize( CWndBase* pWndParent, DWORD nType)
{
	if(!CCLord::Instance()->IsLord(g_pPlayer->m_idPlayer))
	{
		g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_L_EVENT_CREATE_E001));
		Destroy();
		return FALSE;
	}
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_LORD_EVENT, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndLordEvent::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch(nID)
	{
		case WIDC_BUTTON1:
			
			if( --m_nEEvent < 0)
				m_nEEvent += CCLord::Instance()->GetEvent()->GetEFactorSize();
			break;

		case WIDC_BUTTON2:
				m_nEEvent = ( m_nEEvent + 1 ) % CCLord::Instance()->GetEvent()->GetEFactorSize();
			break;

		case WIDC_BUTTON3:
			if( --m_nDEvent < 0)
				m_nDEvent += CCLord::Instance()->GetEvent()->GetIFactorSize();
			break;

		case WIDC_BUTTON4:
				m_nDEvent = ( m_nDEvent + 1 ) % CCLord::Instance()->GetEvent()->GetEFactorSize();
			break;

		case WIDC_BUTTON5:
			if( !CCLord::Instance()->GetEvent()->GetComponent( g_pPlayer->m_idPlayer ) )
				g_DPlay.SendLEventCreate(m_nEEvent, m_nDEvent);
			break;
	};
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndLordEvent::OnDraw( C2DRender* p2DRender )
{

	CString strTemp;
	int nTemp = (int)(100 *  CCLord::Instance()->GetEvent()->GetEFactor( m_nEEvent ));
	if((nTemp % 5) != 0) nTemp += 1;
	strTemp.Format("  %d %c", nTemp, '%');
	CWndText*	pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );	
	pWndText->SetString(strTemp);

	nTemp = (int)(100 *  CCLord::Instance()->GetEvent()->GetIFactor( m_nDEvent ));
	if((nTemp % 5) != 0) nTemp += 1;
	strTemp.Format("  %d %c", nTemp, '%');
	pWndText = (CWndText*)GetDlgItem( WIDC_TEXT2 );	
	pWndText->SetString(strTemp);
	
	__int64 iCost = CCLord::Instance()->GetEvent()->GetCost( m_nEEvent, m_nDEvent );
	strTemp.Format("%I64d", iCost);
	pWndText = (CWndText*)GetDlgItem( WIDC_TEXT3 );	
	int nLength = strTemp.GetLength();
	while(nLength - 3 > 0)
	{
		nLength -= 3;
		strTemp.Insert(nLength, ',');
	}
	pWndText->SetString(strTemp);

}

void CWndLordEvent::OnInitialUpdate()
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

BOOL CWndLordEvent::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{

	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
}

void CWndLordEvent::OnSize( UINT nType, int cx, int cy )
{

	CWndNeuz::OnSize( nType, cx, cy ); 

}

void CWndLordEvent::OnLButtonUp( UINT nFlags, CPoint point )
{

}

void CWndLordEvent::OnLButtonDown( UINT nFlags, CPoint point )
{

}



// CWndLordSkill
CWndLordSkill::CWndLordSkill()
{
	m_bDrag			= FALSE;
	m_nCurSelect	= 0;
	for(int i = 0; i < 8; ++i) 
	{
		m_aTexSkill[i] = NULL;
		m_aWndCtrl[i]  = NULL;
	}
}

CWndLordSkill::~CWndLordSkill()
{

}

BOOL CWndLordSkill::Initialize( CWndBase* pWndParent, DWORD nType)
{
	if(!CCLord::Instance()->IsLord(g_pPlayer->m_idPlayer))
	{
		//g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_L_EVENT_CREATE_E001));
		g_WndMng.PutString( prj.GetText( TID_GAME_L_EVENT_CREATE_E001 ), NULL, prj.GetTextColor( TID_GAME_L_EVENT_CREATE_E001 ) );
		Destroy();
		return FALSE;
	}
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_LORD_SKILL, 0, CPoint( 0, 0 ), pWndParent );

}

BOOL CWndLordSkill::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndLordSkill::OnDraw( C2DRender* p2DRender )
{
	// 아이콘 렌더링 해야함
	for(int i = 0; i < 8; ++i)
	{
		m_aTexSkill[ i ]->Render( p2DRender, m_aWndCtrl[i]->rect.TopLeft(), CPoint( 32, 32 ) );
	}
}

void CWndLordSkill::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	pWndText->SetString(prj.GetText(TID_GAME_LORD_SKILL_INFO));
	
	// 아이콘 텍스쳐 로딩
	CCLord* pLord						= CCLord::Instance();
	for(int i = 0; i < 8; ++i)
	{
		CLordSkillComponentExecutable* pComponent		= pLord->GetSkills()->GetSkill(i);
		m_aTexSkill[i] = pComponent->GetTexture();
	}

	m_aWndCtrl[0] = GetWndCtrl( WIDC_CUSTOM1 );
	m_aWndCtrl[1] = GetWndCtrl( WIDC_CUSTOM2 );
	m_aWndCtrl[2] = GetWndCtrl( WIDC_CUSTOM3 );
	m_aWndCtrl[3] = GetWndCtrl( WIDC_CUSTOM4 );
	m_aWndCtrl[4] = GetWndCtrl( WIDC_CUSTOM5 );
	m_aWndCtrl[5] = GetWndCtrl( WIDC_CUSTOM6 );
	m_aWndCtrl[6] = GetWndCtrl( WIDC_CUSTOM7 );
	m_aWndCtrl[7] = GetWndCtrl( WIDC_CUSTOM8 );
	m_aWndCtrl[8] = GetWndCtrl( WIDC_CUSTOM9 );

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
}

BOOL CWndLordSkill::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{

	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
}

void CWndLordSkill::OnSize( UINT nType, int cx, int cy )
{

	CWndNeuz::OnSize( nType, cx, cy ); 

}

void CWndLordSkill::OnLButtonUp( UINT nFlags, CPoint point )
{
	m_bDrag		 = FALSE;
	m_nCurSelect = 0;
}

void CWndLordSkill::OnLButtonDown( UINT nFlags, CPoint point )
{
	for(int i = 0; i < 8; ++i)
	{
		if(m_aWndCtrl[i]->rect.PtInRect( point ) )
		{
			m_nCurSelect = i + 1;
			m_bDrag = TRUE;
		}
	}
}

void CWndLordSkill::OnMouseMove(UINT nFlags, CPoint point)
{

	if( m_bDrag == FALSE )
		return;
	
	if(m_nCurSelect)
	{
		CCLord* pLord						= CCLord::Instance();
		CLordSkillComponentExecutable* pComponent		= pLord->GetSkills()->GetSkill(m_nCurSelect - 1);
		m_bDrag = FALSE;
		m_GlobalShortcut.m_pFromWnd    = this;
		m_GlobalShortcut.m_dwShortcut  = SHORTCUT_LORDSKILL;
		m_GlobalShortcut.m_dwType	   = 0;
		m_GlobalShortcut.m_dwIndex	   = 0;//dwSkill;
		m_GlobalShortcut.m_dwData	   = 0;
		m_GlobalShortcut.m_dwId        = m_nCurSelect; // 컬런트 셀렉트가 곧 ID나 마찬가지임.
		m_GlobalShortcut.m_pTexture	   = pComponent->GetTexture(); 
		_tcscpy( m_GlobalShortcut.m_szString, pComponent->GetName());
	}

}

void CWndLordSkill::OnMouseWndSurface( CPoint point )
{
	
	for(int i = 0; i < 8; ++i)
	{
		if(m_aWndCtrl[i]->rect.PtInRect( point ) )
		{
			CCLord* pLord								= CCLord::Instance();
			CLordSkillComponentExecutable* pComponent	= pLord->GetSkills()->GetSkill(i);
			CRect rect									= m_aWndCtrl[i]->rect;
			CPoint point2								= point;

			ClientToScreen( &point2 );
			ClientToScreen( &rect );
			CString		string;
			CEditString strEdit;
			string.Format( "#b#cff2fbe6d%s#nb#nc \n%s", pComponent->GetName(), pComponent->GetDesc());
			strEdit.SetParsingString( string );
			g_toolTip.PutToolTip(i, strEdit, &rect, point2, 0 );
		}
	}

}

// CWndLordConfirm
BOOL CWndLordConfirm::Initialize( CWndBase* pWndParent, DWORD nType)
{

	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_LORD_CONFIRM, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndLordConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch(nID)
	{
		case WIDC_BUTTON1:
			if(!m_pPledge.IsEmpty())
			{
				g_DPlay.SendElectionSetPledge(m_pPledge);
				SetVisible(FALSE);
				if(g_WndMng.m_pWndLordState)
				{
					if(g_WndMng.m_pWndLordState->m_pWndPledge)	g_WndMng.m_pWndLordState->m_pWndPledge->Destroy();
				}
			}
			break;
		case WIDC_BUTTON2:
			SetVisible(FALSE);
			break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndLordConfirm::OnDraw( C2DRender* p2DRender )
{

}

void CWndLordConfirm::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	pWndText->SetString(prj.GetText(TID_GAME_MOD_PLEGDE));
	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
}

BOOL CWndLordConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
}

void CWndLordConfirm::OnSize( UINT nType, int cx, int cy )
{
	CWndNeuz::OnSize( nType, cx, cy ); 
}

void CWndLordConfirm::OnLButtonUp( UINT nFlags, CPoint point )
{

}

void CWndLordConfirm::OnLButtonDown( UINT nFlags, CPoint point )
{

}


// CWndLordSkillConfirm
BOOL CWndLordSkillConfirm::Initialize( CWndBase* pWndParent, DWORD nType)
{

	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_LORD_SKILL_CONFIRM, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndLordSkillConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );

	switch(nID)
	{
		
		case WIDC_BUTTON1:
				if(pWndEdit->GetString() && m_nType != 0)
				{
					g_DPlay.SendLordSkillUse(m_nType, pWndEdit->GetString());
					Destroy();
				}
			break;
	};
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndLordSkillConfirm::OnDraw( C2DRender* p2DRender )
{

}

void CWndLordSkillConfirm::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	
	switch(m_nType)
	{
		case LI_SUMMON:
			pWndText->SetString(prj.GetText(TID_GAME_LORD_SKILL_CONFIRM1));
			pWndEdit->SetToolTip(prj.GetText(TID_TIP_LORD_SKILL_CONFIRM1));
			break;
		case LI_TELEPORT:
			pWndText->SetString(prj.GetText(TID_GAME_LORD_SKILL_CONFIRM2));
			pWndEdit->SetToolTip(prj.GetText(TID_TIP_LORD_SKILL_CONFIRM2));
			break;
	};

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
}

BOOL CWndLordSkillConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
}

void CWndLordSkillConfirm::OnSize( UINT nType, int cx, int cy )
{
	CWndNeuz::OnSize( nType, cx, cy ); 
}


// CWndLordInfo
BOOL CWndLordInfo::Initialize( CWndBase* pWndParent, DWORD nType)
{

	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_LORD_INFO, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndLordInfo::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch(nID)
	{
		
		case WIDC_BUTTON1:
			Destroy();
			break;
	};
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndLordInfo::OnDraw( C2DRender* p2DRender )
{
	CCElection* pElection		= static_cast<CCElection*>( CCLord::Instance()->GetElection() );
	CCLord* pLord				= CCLord::Instance();
	int nLordId					= pLord->Get();
	CWndStatic* pWndStatus		= (CWndStatic*)GetDlgItem( WIDC_STATIC4 ); 
	CWndStatic* pWndName		= (CWndStatic*)GetDlgItem( WIDC_STATIC5 ); 
	CWndStatic* pWndremainTime	= (CWndStatic*)GetDlgItem( WIDC_STATIC6 ); 
	
	if(nLordId != NULL_ID)
	{
		// 군주가 있을때
		PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->GetPlayerData(nLordId);

		pWndName->SetTitle(pPlayerData->szPlayer);
		if(pElection->GetRestTimeEndVote() >= 0)
		{
			CTimeSpan	ts(pElection->GetRestTimeEndVote());
			CString strTime;
			strTime.Format( prj.GetText( TID_TOOLTIP_DATE ), static_cast<int>(ts.GetDays()), ts.GetHours(), ts.GetMinutes(), ts.GetSeconds());
			pWndremainTime->SetTitle(strTime);
		}
	}
	else
	{
		pWndName->SetTitle("");
		pWndremainTime->SetTitle("");
	}
	// 현 상태를 표시해준다.
	if(pElection->GetState() == CCElection::eReady) 
	{
		if(nLordId != NULL_ID)
		{
			// 군주가 있을때
			pWndStatus->SetTitle(prj.GetText(TID_GAME_LORD_STATUS_IS));
		}
		else
		{
			pWndStatus->SetTitle(prj.GetText(TID_GAME_LORD_STATUS_VA));
		}
	}
	else if(pElection->GetState() == CCElection::eCandidacy)
	{
		// 입후보 기간
		pWndStatus->SetTitle(prj.GetText(TID_GAME_LORD_STATUS_L1));
		
	}
	else if(pElection->GetState() == CCElection::eVote)
	{
		// 투표 기간
		pWndStatus->SetTitle(prj.GetText(TID_GAME_LORD_STATUS_L2));
	}
	else if(pElection->GetState() == CCElection::eExpired)
	{
		// 군주가 없을때
		pWndStatus->SetTitle(prj.GetText(TID_GAME_LORD_STATUS_VA));
	}
	// TID_GAME_LORD_STATUS_L1 - 입후보 기간
	// TID_GAME_LORD_STATUS_L2
	// TID_GAME_LORD_STATUS_IS
	// TID_GAME_LORD_STATUS_VA
}

void CWndLordInfo::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	CWndText*	pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );		
	CScript		scanner;

	if(scanner.Load( MakePath( DIR_CLIENT,  _T( "lordInfo.inc" ) )))
		pWndText->SetString(scanner.m_pProg);

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
}

BOOL CWndLordInfo::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
}

void CWndLordInfo::OnSize( UINT nType, int cx, int cy )
{
	CWndNeuz::OnSize( nType, cx, cy ); 
}

// CWndLordRPInfo
BOOL CWndLordRPInfo::Initialize( CWndBase* pWndParent, DWORD nType)
{

	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_LORD_INFO2, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndLordRPInfo::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch(nID)
	{
		case WIDC_BUTTON1:
			Destroy();
			break;
	};
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndLordRPInfo::OnDraw( C2DRender* p2DRender )
{

}

void CWndLordRPInfo::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	CWndText*	pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );		
	CScript		scanner;

	if(scanner.Load( MakePath( DIR_CLIENT,  _T( "lordrpInfo.inc" ) )))
		pWndText->SetString(scanner.m_pProg);

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
}

BOOL CWndLordRPInfo::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
}

void CWndLordRPInfo::OnSize( UINT nType, int cx, int cy )
{
	CWndNeuz::OnSize( nType, cx, cy ); 
}
#endif