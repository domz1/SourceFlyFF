#include "stdafx.h"
#include "resData.h"

#ifdef __AZRIA_1023
#include "defineText.h"
#include "AppDefine.h"

#include "WndSelectCh.h"
#include "DPClient.h"
extern	CDPClient	g_DPlay;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/****************************************************
  WndId : APP_SELECT_CHANNEL - 채널 선택
  CtrlId : WIDC_LISTBOX1 - Listbox
  CtrlId : WIDC_STATIC1 - GroupBox
****************************************************/
/*
#ifdef __PVPLAYER
			if( g_WorldMng.GetWorld( pTicketProp->dwWorldId )->IsArena() && nExpand > 0 )
			{
				if( GetLevel() >= 130 )
					nLayer = 5;
				else if( GetLevel() >= 100 && !IsPro() )
					nLayer = 4;
				else if( GetLevel() >= 60 )
				{
					if( IsPro() )
						nLayer = 2;
					else
						nLayer = 3;
				}
				else
					nLayer = 1;
				nLayer *= -1;
			}
			else
#endif
*/
#ifdef __CASH_AREA
CWndSelectCh::CWndSelectCh(int nItemId, int nChCount, CString str) 
#else
CWndSelectCh::CWndSelectCh(int nItemId, int nChCount) 
#endif
{ 
	m_nItemId  = nItemId;
	m_nChCount = nChCount + 1;	// 0일때 1개이므로 실제갯수대로 맞춰준다
#ifdef __CASH_AREA
	m_str = str;
#endif
} 

CWndSelectCh::~CWndSelectCh() 
{ 
}

void CWndSelectCh::OnDraw( C2DRender* p2DRender ) 
{ 
}

void CWndSelectCh::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	// 여기에 코딩하세요
#ifdef __CASH_AREA
	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LB_CHANNEL );
#else
	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
#endif
	pWndListBox->ResetContent();
	CString strTitle;
	for(int i=0; i < m_nChCount; ++i)
	{
		strTitle.Format( "%s	%d", prj.GetText(TID_GAME_CHAR_SERVERNAME), i+1);
		pWndListBox->AddString(strTitle);
	}

#ifdef __CASH_AREA
	pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LB_WORLD );
	pWndListBox->ResetContent();
	pWndListBox->AddString( m_str );
#endif

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndSelectCh::Initialize( CWndBase* pWndParent, DWORD  ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
#ifdef __CASH_AREA
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_WORLD_FREETICKET, 0, CPoint( 0, 0 ), pWndParent );
#else
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_SELECT_CHANNEL, 0, CPoint( 0, 0 ), pWndParent );
#endif
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndSelectCh::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndSelectCh::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndSelectCh::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndSelectCh::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndSelectCh::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndSelectCh::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	int nSelect = 0;

	switch(nID)
	{
#ifdef __CASH_AREA
		case WIDC_BT_MOVE: //Ok Button
			{
				CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LB_CHANNEL );
				char strTemp[8];
				nSelect = pWndListBox->GetCurSel() * -1;
				_itoa(nSelect, strTemp, 10);
				g_DPlay.SendDoUseItemInput(m_nItemId, strTemp);
				Destroy();
			}
			break;
		case WIDC_BT_CANCEL: //Cancel Button
			{
				Destroy();
			}
			break;
#else
		case WIDC_LISTBOX1: // view ctrl
			{
				CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
				char strTemp[8];
				nSelect = pWndListBox->GetCurSel() * -1;
				_itoa(nSelect, strTemp, 10);
				g_DPlay.SendDoUseItemInput(m_nItemId, strTemp);
				Destroy();
			}
			break;
#endif
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

#endif