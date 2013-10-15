#include "stdafx.h"
#include "WndText.h"
#include "resdata.h"
#ifdef __CLIENT
 #include "DPClient.h"
 extern	CDPClient	g_DPlay;
#endif

/****************************************************
  WndId : APP_TEXT_BOOK - 서적
  CtrlId : WIDC_BUTTON1 - Button
  CtrlId : WIDC_BUTTON2 - Button
  CtrlId : WIDC_TEXT1 - Text
  CtrlId : WIDC_TEXT2 - Text
  CtrlId : WIDC_ACCEPT - Button
  CtrlId : WIDC_CLOSE - Button
****************************************************/

CWndTextBook::CWndTextBook() 
{ 
} 
CWndTextBook::~CWndTextBook() 
{ 
} 
void CWndTextBook::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndTextBook::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	CWndButton* pWndAccept = (CWndButton*)GetDlgItem( WIDC_ACCEPT );
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	ItemProp* pItemProp = m_pItemBase->GetProp();

	CItemElem* pItemElem = (CItemElem*)m_pItemBase;
	if( pItemProp->dwQuestId == 0 || pItemElem->m_bItemResist == TRUE )
		pWndAccept->EnableWindow( FALSE );

	CFileIO file;
	if( file.Open( MakePath( DIR_TEXT, pItemProp->szTextFileName ), "rb" ) )
	{
		CHAR* pText = new CHAR[ file.GetLength() + 1 ];
		file.Read( pText, file.GetLength() );
		pText[ file.GetLength() ] = 0;
		pWndText->SetString( pText );
		safe_delete( pText );
	}
	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndTextBook::Initialize( CWndBase* pWndParent, CItemBase* pItemBase )
{ 
	m_pItemBase = pItemBase;
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_TEXT_BOOK, 0, CPoint( 0, 0 ), pWndParent );
} 

/*
  직접 윈도를 열때 사용 
BOOL CWndTextBook::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
void CWndTextBook::SetItemBase( CItemBase* pItemBase ) 
{
	m_pItemBase = pItemBase;
}
BOOL CWndTextBook::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndTextBook::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndTextBook::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndTextBook::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndTextBook::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case WIDC_ACCEPT:
		{
			CWndTextQuest* pWndTextQuest = new CWndTextQuest;
			pWndTextQuest->m_pItemBase = m_pItemBase;
			g_WndMng.OpenCustomBox( "", pWndTextQuest );
		}
		break;
	case WIDC_CLOSE:
		Destroy();
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

/****************************************************
  WndId : APP_TEXT_SCROLL - 스크롤
  CtrlId : WIDC_TEXT1 - Text
  CtrlId : WIDC_ACCEPT - Button
  CtrlId : WIDC_CLOSE - Button
****************************************************/

CWndTextScroll::CWndTextScroll() 
{ 
} 
CWndTextScroll::~CWndTextScroll() 
{ 
} 
void CWndTextScroll::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndTextScroll::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	CWndButton* pWndAccept = (CWndButton*)GetDlgItem( WIDC_ACCEPT );
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	ItemProp* pItemProp = m_pItemBase->GetProp();

	CItemElem* pItemElem = (CItemElem*)m_pItemBase;
	if( pItemProp->dwQuestId == 0 || pItemElem->m_bItemResist == TRUE )
		pWndAccept->EnableWindow( FALSE );

	CFileIO file;
	if( file.Open( MakePath( DIR_TEXT, pItemProp->szTextFileName ), "rb" ) )
	{
		CHAR* pText = new CHAR[ file.GetLength() + 1 ];
		file.Read( pText, file.GetLength() );
		pText[ file.GetLength() ] = 0;
		pWndText->SetString( pText );
		safe_delete( pText );
	}
	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndTextScroll::Initialize( CWndBase* pWndParent, CItemBase* pItemBase )
{ 
	m_pItemBase = pItemBase;
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_TEXT_SCROLL, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndTextScroll::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
void CWndTextScroll::SetItemBase( CItemBase* pItemBase ) 
{
	m_pItemBase = pItemBase;
}
BOOL CWndTextScroll::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndTextScroll::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndTextScroll::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndTextScroll::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndTextScroll::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case WIDC_ACCEPT:
		{
			CWndTextQuest* pWndTextQuest = new CWndTextQuest;
			pWndTextQuest->m_pItemBase = m_pItemBase;
			g_WndMng.OpenCustomBox( "", pWndTextQuest );
		}
		break;
	case WIDC_CLOSE:
		Destroy();
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

/****************************************************
  WndId : APP_TEXT_LETTER - 편지
  CtrlId : WIDC_TEXT1 - Text
  CtrlId : WIDC_ACCEPT - Button
  CtrlId : WIDC_CLOSE - Button
****************************************************/

CWndTextLetter::CWndTextLetter() 
{ 
} 
CWndTextLetter::~CWndTextLetter() 
{ 
} 
void CWndTextLetter::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndTextLetter::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	CWndButton* pWndAccept = (CWndButton*)GetDlgItem( WIDC_ACCEPT );
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );

	ItemProp* pItemProp = m_pItemBase->GetProp();

	CItemElem* pItemElem = (CItemElem*)m_pItemBase;
	if( pItemProp->dwQuestId == 0 || pItemElem->m_bItemResist == TRUE )
		pWndAccept->EnableWindow( FALSE );

	CFileIO file;
	if( file.Open( MakePath( DIR_TEXT, pItemProp->szTextFileName ), "rb" ) )
	{
		CHAR* pText = new CHAR[ file.GetLength() + 1 ];
		file.Read( pText, file.GetLength() );
		pText[ file.GetLength() ] = 0;
		pWndText->SetString( pText );
		safe_delete( pText );
	}
	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndTextLetter::Initialize( CWndBase* pWndParent, CItemBase* pItemBase )
{ 
	m_pItemBase = pItemBase;
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_TEXT_LETTER, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndTextLetter::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
void CWndTextLetter::SetItemBase( CItemBase* pItemBase ) 
{
	m_pItemBase = pItemBase;
}
BOOL CWndTextLetter::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndTextLetter::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndTextLetter::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndTextLetter::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndTextLetter::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case WIDC_ACCEPT:
		{
			CWndTextQuest* pWndTextQuest = new CWndTextQuest;
			pWndTextQuest->m_pItemBase = m_pItemBase;
			g_WndMng.OpenCustomBox( "", pWndTextQuest );
		}
		break;
	case WIDC_CLOSE:
		Destroy();
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


/****************************************************
  WndId : APP_TEXT_QUEST - 퀘스트 확인
  CtrlId : WIDC_STATIC1 - 퀘스트를 진행하시겠습니까?
  CtrlId : WIDC_BUTTON1 - Button
  CtrlId : WIDC_BUTTON2 - Button
  CtrlId : WIDC_STATIC2 - 
****************************************************/

BOOL CWndTextQuest::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	return CWndMessageBox::Initialize( "퀘스트를 진행하시겠습니까?", pWndParent, MB_OKCANCEL );
}
BOOL CWndTextQuest::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
 	switch( nID )
	{
	case IDOK:
		{
			ItemProp* pItemProp = m_pItemBase->GetProp();
			g_DPlay.SendDoUseItem( MAKELONG(0, m_pItemBase->m_dwObjId ), NULL_ID );
			Destroy();
		}

		break;
	case IDCANCEL:
		Destroy();
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

