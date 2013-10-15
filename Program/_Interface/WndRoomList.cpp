

#include "stdafx.h"
#include "resData.h"
#include "WndRoomList.h"
#include "defineText.h"
#include "playerdata.h"

#include "DPClient.h"
extern	CDPClient	g_DPlay;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if __VER >= 13 // __HOUSING
/****************************************************
  WndId : APP_MINIROOM_LIST - 미니룸 목록
  CtrlId : WIDC_BUTTON1 - Button
  CtrlId : WIDC_BUTTON2 - Button
  CtrlId : WIDC_BUTTON3 - Button
  CtrlId : WIDC_LISTBOX1 - Listbox
  CtrlId : WIDC_TEXT1 - 현재 입장이 가능한 미니룸의 캐릭터 목록입니다.
****************************************************/

CWndRoomList::CWndRoomList() 
{ 
} 
CWndRoomList::~CWndRoomList() 
{ 
	
} 
void CWndRoomList::OnDraw( C2DRender* p2DRender ) 
{ 
} 

void CWndRoomList::Refresh()
{
	// 리스트를 새로 갱신
	CWndListBox*				pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	vector<DWORD>::iterator		iter		= CHousing::GetInstance()->m_vecVisitable.begin();
	int							nIndex		= 0;
	
	pWndListBox->ResetContent();
	
	for(;iter != CHousing::GetInstance()->m_vecVisitable.end(); ++iter)
	{
		PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->GetPlayerData(*iter);
		if(pPlayerData)
		{
			CString	strName;
			strName.Format("%d. %s", nIndex + 1, pPlayerData->szPlayer);
			pWndListBox->AddString(strName);
			pWndListBox->SetItemData(nIndex, *iter);
			++nIndex;
		}
	}
}

void CWndRoomList::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	CHousing::GetInstance()->m_vecVisitable.clear();
	g_DPlay.SendHousingReqVisitableList();

	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	pWndListBox->m_nSelectColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndRoomList::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_MINIROOM_LIST, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndRoomList::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndRoomList::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndRoomList::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndRoomList::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndRoomList::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndRoomList::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	switch(nID)
	{
		case WIDC_BUTTON1:// ok 버튼
			if(pWndListBox->GetCurSel() >= 0)
			{
				
				DWORD dwID = pWndListBox->GetItemData(pWndListBox->GetCurSel());
				if(dwID >= 0)
					g_DPlay.SendHousingVisitRoom(dwID);
				Destroy();
			}
			break;

		case WIDC_BUTTON2:// refresh 버튼
			g_DPlay.SendHousingReqVisitableList();
			break;

		case WIDC_BUTTON3:// cancel 버튼
			Destroy();
			break;
	};

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

#endif // __HOUSING