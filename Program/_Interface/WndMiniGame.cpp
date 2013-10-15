#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndManager.h"
#include "WndMiniGame.h"
#include "MiniGame.h"
#include "defineSound.h"

#include "DPClient.h"
extern	CDPClient	g_DPlay;

#ifdef __EVE_MINIGAME

/*************************
	CWndKawiBawiBoGame
*************************/
CWndKawiBawiBoGame::CWndKawiBawiBoGame() 
{ 
	m_nWinningCount = 0;
	m_nWinningMaxCount = 0;
	m_nCount = 0;
	m_nDelay = 1;
	m_nMyChoice = KAWI;
	m_nComChoice = KAWI;
	m_strChoice[0].Format("        %s", prj.GetText( TID_GAME_KAWIBAWIBO_KAWI ));
	m_strChoice[1].Format("        %s", prj.GetText( TID_GAME_KAWIBAWIBO_BAWI ));
	m_strChoice[2].Format("        %s", prj.GetText( TID_GAME_KAWIBAWIBO_BO ));

	m_pWndGameWin = NULL;
	m_nStatus = 0;
	m_nResult = -1;
	m_nPrevResult = -1;
	m_bCheckCoupon = TRUE;

	m_dwRItemId = -1;
	m_dwRNextItemId = -1;
	m_nItemCount = -1;
	m_nNextItemCount = -1;
}
 
CWndKawiBawiBoGame::~CWndKawiBawiBoGame() 
{ 
	if(m_pWndGameWin != NULL)
		SAFE_DELETE(m_pWndGameWin);
} 

void CWndKawiBawiBoGame::OnDestroy()
{
}

BOOL CWndKawiBawiBoGame::Process()
{
	int nCom = -1;
	
	if(m_nStatus == 1) //Start버튼 누를 경우 컴퓨터의 선택이 회전하도록 함.
	{
		if(m_nCount > m_nDelay)
		{
			if(m_nResult > -1)
			{
				if(m_nDelay < 20)
				{
					PLAYSND( "InfOpen.wav" );
					m_nDelay += 1;
				}
				else
				{
					PLAYSND( "InfOpen.wav" );
					nCom = prj.m_MiniGame.GetKawibawiboYou(m_nMyChoice, m_nResult);
				}
			}
			
			m_nComChoice++;
			( m_nComChoice > 2 ) ? m_nComChoice = 0 : m_nComChoice;
			m_pStComChoice->SetTitle( m_strChoice[m_nComChoice] );
			
			if(m_nComChoice == nCom)
			{
				m_nStatus = 2;
				m_nDelay = 40;
			}
			
			m_nCount = 0;
		}
		m_nCount++;
	}
	else if(m_nStatus == 2) //Com의 결과가 보여지고 바로 사라지거나 하기 때문에 약간의 Delay를 줌
	{
		if(m_nCount > m_nDelay)
		{
			if(m_nResult == CMiniGame::KAWIBAWIBO_WIN)
			{
				PlayMusic( BGM_IN_FITUP );
				CreateWinningWnd();
			}
			else if(m_nResult == CMiniGame::KAWIBAWIBO_DRAW)
			{
				EnableAllBtn();
			}
			else if(m_nResult == CMiniGame::KAWIBAWIBO_LOST)
			{
				g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_KAWIBAWIBO_DEFEAT ) );
				m_nWinningCount = 0;
				EnableAllBtn();
				RefreshInfo();
			}			
			
			m_nStatus = 0;
			m_nDelay = 1;
			m_nResult = -1;	
		}
		m_nCount++;
	}
	
	return TRUE;
}

void CWndKawiBawiBoGame::OnDraw( C2DRender* p2DRender ) 
{ 
	CTexture* pTexture;
	CString strMyPath, strComPath;

	for(int i=0; i<2; i++)
	{
		int nChoice;
		CString* pstr;
		if(i == 0)
		{
			nChoice = m_nMyChoice;
			pstr = &strMyPath;
			switch(nChoice) 
			{
			case KAWI:
				*pstr = MakePath( DIR_ICON, "Icon_Kawi1.dds");
				break;
			case BAWI:
				*pstr = MakePath( DIR_ICON, "Icon_Bawi1.dds");
				break;
			case BO:
				*pstr = MakePath( DIR_ICON, "Icon_Bo1.dds");
				break;
			}
		}
		else if(i == 1)
		{
			nChoice = m_nComChoice;
			pstr = &strComPath;
			switch(nChoice) 
			{
			case KAWI:
				*pstr = MakePath( DIR_ICON, "Icon_Kawi2.dds");
				break;
			case BAWI:
				*pstr = MakePath( DIR_ICON, "Icon_Bawi2.dds");
				break;
			case BO:
				*pstr = MakePath( DIR_ICON, "Icon_Bo2.dds");
				break;
			}
		}
	}
	
	if(strMyPath.GetLength() > 0)
	{
		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, strMyPath, 0xffff00ff );
		if(pTexture != NULL)
			pTexture->Render( p2DRender, CPoint( GetWndCtrl( WIDC_STATIC_MY )->rect.left + 3, GetWndCtrl( WIDC_STATIC_MY )->rect.top + 8 ) );		
	}

	if(strComPath.GetLength() > 0)
	{
		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, strComPath, 0xffff00ff );
		if(pTexture != NULL)
			pTexture->Render( p2DRender, CPoint( GetWndCtrl( WIDC_STATIC_COM )->rect.left + 3, GetWndCtrl( WIDC_STATIC_COM )->rect.top + 8 ) );		
	}
}

void CWndKawiBawiBoGame::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	m_pStWinningCount = (CWndStatic*)GetDlgItem( WIDC_WINNING_COUNT );
	m_pStMyChoice = (CWndStatic*)GetDlgItem( WIDC_MY_CHOICE );
	m_pStComChoice = (CWndStatic*)GetDlgItem( WIDC_COM_CHOICE );

	RefreshInfo();

	MoveParentCenter();
} 

BOOL CWndKawiBawiBoGame::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_MINIGAME_KAWIBAWIBO, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndKawiBawiBoGame::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndKawiBawiBoGame::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndKawiBawiBoGame::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndKawiBawiBoGame::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndKawiBawiBoGame::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if(message == WNM_CLICKED)
	{
		switch(nID) 
		{
			case WIDC_BTN_LEFT:
				m_nMyChoice--;
				( m_nMyChoice < 0 ) ? m_nMyChoice = 2 : m_nMyChoice;
				RefreshInfo();
				break;
			case WIDC_BTN_RIGHT:
				m_nMyChoice++;
				( m_nMyChoice > 2 ) ? m_nMyChoice = 0 : m_nMyChoice;
				RefreshInfo();
				break;
			case WIDC_BTN_START:
				if(m_bCheckCoupon && g_pPlayer->IsPlayer() && 
					g_pPlayer->m_Inventory.GetAtItemId( II_SYS_SYS_EVE_KAWIBAWIBO ) == NULL)
				{
					g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_KAWIBAWIBO_STARTGUID1 ) );
					Destroy();
				}
				
				DisableAllBtn();				
				m_nStatus = 1;						//컴퓨터의 선택을 화면상에 돌리기 위함.
				g_DPlay.SendKawibawiboStart();		//DpClient에 정보를 넘겨 가위바위보 결과를 기다림.
				break;
			case WTBID_CLOSE:
				if( m_nWinningCount > 0 || m_nStatus != 0 || m_nPrevResult == CMiniGame::KAWIBAWIBO_DRAW )
					return FALSE;
				break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndKawiBawiBoGame::ReceiveResult(int nResult, int nItemCount, int nNextItemCount, DWORD dwItemId, DWORD dwNextItemId)
{
	if(nResult == CMiniGame::KAWIBAWIBO_FAILED)
	{
		g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_KAWIBAWIBO_STARTGUID1 ) );
		Destroy();
	}
	else
	{
		if(nResult == CMiniGame::KAWIBAWIBO_DRAW)
			m_bCheckCoupon = FALSE;
		else if(nResult == CMiniGame::KAWIBAWIBO_LOST || nResult == CMiniGame::KAWIBAWIBO_WIN)
			m_bCheckCoupon = TRUE;

		m_nPrevResult = m_nResult = nResult;
		m_nItemCount = nItemCount;
		m_nNextItemCount = nNextItemCount;
		m_dwRItemId = dwItemId;
		m_dwRNextItemId = dwNextItemId;
	}
}

void CWndKawiBawiBoGame::CreateWinningWnd()
{
	m_nWinningCount++;
	RefreshInfo();
	if(m_pWndGameWin != NULL)
		SAFE_DELETE(m_pWndGameWin);

	m_pWndGameWin = new CWndKawiBawiBoGameWin;
	m_pWndGameWin->Initialize(this);
	m_pWndGameWin->SetInfo(m_nItemCount, m_nNextItemCount, m_dwRItemId, m_dwRNextItemId, m_nWinningCount, m_nWinningMaxCount);
}

void CWndKawiBawiBoGame::RefreshInfo()
{
	CString temp;
	temp.Format(prj.GetText(TID_GAME_KWAIBAWIBO_WINCOUNT), m_nWinningCount);
	m_pStWinningCount->SetTitle( temp );
	m_pStMyChoice->SetTitle( m_strChoice[m_nMyChoice] );
	m_pStComChoice->SetTitle( m_strChoice[m_nComChoice] );
}

void CWndKawiBawiBoGame::DisableAllBtn()
{
	CWndButton* pButton;
	pButton = (CWndButton*)GetDlgItem( WIDC_BTN_LEFT );
	pButton->EnableWindow(FALSE);
	pButton = (CWndButton*)GetDlgItem( WIDC_BTN_RIGHT );
	pButton->EnableWindow(FALSE);
	pButton = (CWndButton*)GetDlgItem( WIDC_BTN_START );
	pButton->EnableWindow(FALSE);
}

void CWndKawiBawiBoGame::EnableAllBtn()
{
	CWndButton* pButton;
	pButton = (CWndButton*)GetDlgItem( WIDC_BTN_LEFT );
	pButton->EnableWindow(TRUE);
	pButton = (CWndButton*)GetDlgItem( WIDC_BTN_RIGHT );
	pButton->EnableWindow(TRUE);
	pButton = (CWndButton*)GetDlgItem( WIDC_BTN_START );
	pButton->EnableWindow(TRUE);
}

/*************************
   CWndKawiBawiBoGameWin
*************************/
CWndKawiBawiBoGameWin::CWndKawiBawiBoGameWin() 
{ 
	m_pWndGame = NULL;
	m_dwItemID = -1;
	m_dwNextItemID = -1;
	m_nItemCount = -1;
	m_nNextItemCount = -1;
}
 
CWndKawiBawiBoGameWin::~CWndKawiBawiBoGameWin() 
{ 
} 

void CWndKawiBawiBoGameWin::OnDestroy()
{
	if(m_pWndGame != NULL)
	{
		m_pWndGame->RefreshInfo();
		m_pWndGame->EnableAllBtn();
	}
}

void CWndKawiBawiBoGameWin::OnDraw( C2DRender* p2DRender ) 
{ 
	if(m_dwItemID != -1 && m_dwNextItemID != -1)
	{
		ItemProp* pItemProp;
		CTexture* pTexture;
		CPoint point;

		pItemProp = prj.GetItemProp( m_dwItemID );
		if(pItemProp != NULL)
		{
			point = CPoint( GetWndCtrl( WIDC_CURRENT_PRESENT )->rect.left + 16, GetWndCtrl( WIDC_CURRENT_PRESENT )->rect.top + 16 );
			pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
			if(pTexture != NULL)
				pTexture->Render( p2DRender,  point);
			if(m_nItemCount > 1)
			{
				TCHAR szTemp[ 32 ];
				_stprintf( szTemp, prj.GetText(TID_GAME_KWAIBAWIBO_PRESENT_NUM), m_nItemCount );
				CSize size = m_p2DRender->m_pFont->GetTextExtent( szTemp );
				m_p2DRender->TextOut( point.x + 36 - size.cx, point.y + 48 - size.cy, szTemp, 0xff0000ff );
			}				
		}

		pItemProp = prj.GetItemProp( m_dwNextItemID );
		if(pItemProp != NULL)
		{
			point = CPoint( GetWndCtrl( WIDC_NEXT_PRESENT )->rect.left + 16, GetWndCtrl( WIDC_NEXT_PRESENT )->rect.top + 16 );
			pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
			if(pTexture != NULL)
				pTexture->Render( p2DRender,  point);
			if(m_nNextItemCount > 1)
			{
				TCHAR szTemp[ 32 ];
				_stprintf( szTemp, prj.GetText(TID_GAME_KWAIBAWIBO_PRESENT_NUM), m_nNextItemCount );
				CSize size = m_p2DRender->m_pFont->GetTextExtent( szTemp );
				m_p2DRender->TextOut( point.x + 36 - size.cx, point.y + 48 - size.cy, szTemp, 0xff0000ff );
			}
		}
	}
} 

void CWndKawiBawiBoGameWin::OnMouseWndSurface(CPoint point)
{
	CItemElem itemElem;	
	CRect rectHittest = GetWndCtrl(WIDC_CURRENT_PRESENT)->rect;
	
	if( rectHittest.PtInRect(point) )
	{
		itemElem.m_dwItemId = m_dwItemID;
		itemElem.m_nItemNum	= 1;
		CPoint point2 = point;
		ClientToScreen( &point2 );
		ClientToScreen( &rectHittest );
	
		g_WndMng.PutToolTip_Item( &itemElem, point2, &rectHittest );
	}
	
	rectHittest = GetWndCtrl(WIDC_NEXT_PRESENT)->rect;
	if( rectHittest.PtInRect(point) )
	{
		itemElem.m_dwItemId = m_dwNextItemID;
		itemElem.m_nItemNum	= 1;

		CPoint point2 = point;
		ClientToScreen( &point2 );
		ClientToScreen( &rectHittest );
			
		g_WndMng.PutToolTip_Item( &itemElem, point2, &rectHittest );
	}
}

void CWndKawiBawiBoGameWin::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	CWndKawiBawiBoGame* pWndGame = (CWndKawiBawiBoGame*)GetWndBase( APP_MINIGAME_KAWIBAWIBO );
	if(pWndGame != NULL)
	{
		CRect rectRoot = pWndGame->m_pWndRoot->GetLayoutRect();
		CRect rectGame = pWndGame->GetWindowRect( TRUE );

		CPoint ptMove;
		CPoint ptGame;

		CRect rect = GetWindowRect( TRUE );
		int wndWidth = rect.Width();

		if(rectRoot.right - rectGame.right < wndWidth)
		{
			ptGame = rectGame.TopLeft();
			ptMove = ptGame + CPoint(-(10+wndWidth), 0);
		}
		else
		{
			ptGame = rectGame.BottomRight();
			ptMove = ptGame + CPoint(10, -rectGame.Height());
		}

		Move( ptMove );
	}

	CWndStatic* pStatic;
	CWndText* pText;
	pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC3 );
	pStatic->SetTitle(prj.GetText(TID_GMAE_KAWIBAWIBO_CUR_PRESENT));
	pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC4 );
	pStatic->SetTitle(prj.GetText(TID_GAME_KWAIBAWIBO_NXT_PRESENT));
	pText = (CWndEdit*)GetDlgItem( WIDC_TEXT_GUID );
	pText->SetString(prj.GetText( TID_GAME_KAWIBAWIBO_GUID1 ));
	pText->AddString(prj.GetText( TID_GAME_KAWIBAWIBO_GUID2 ), 0xffff0000);
} 

BOOL CWndKawiBawiBoGameWin::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	m_pWndGame  = (CWndKawiBawiBoGame*)pWndParent;	
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_MINIGAME_KAWIBAWIBO_WIN, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndKawiBawiBoGameWin::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndKawiBawiBoGameWin::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndKawiBawiBoGameWin::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndKawiBawiBoGameWin::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndKawiBawiBoGameWin::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if(message == WNM_CLICKED)
	{
		switch(nID) 
		{
			case WIDC_BTN_END: //End일 경우 상품을 받는다.
				g_DPlay.SendKawibawiboGetItem();
				if(m_pWndGame != NULL)
				{
					//m_pWndGame->m_nWinningCount = 0;
					m_pWndGame->m_bCheckCoupon = TRUE;
				}
				Destroy();
				break;
			case WIDC_BTN_NEXT: //Next일 경우 승수를 증가시키고 다시 게임을 진행한다.
				if(m_pWndGame != NULL)
				{
					//m_pWndGame->m_nWinningCount++;
					m_pWndGame->m_bCheckCoupon = FALSE;
				}
				Destroy();
				break;
			case WTBID_CLOSE:
				return FALSE;
				break;
		}
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndKawiBawiBoGameWin::SetInfo(int nItemCount, int nNextItemCount, DWORD dwItemId, DWORD dwNextItemId, int nWinningCount, int nWinningMaxCount)
{
	m_nItemCount = nItemCount;
	m_nNextItemCount = nNextItemCount;
	m_dwItemID = dwItemId;
	m_dwNextItemID = dwNextItemId;

	if(nWinningCount >= nWinningMaxCount)
	{
		CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_BTN_NEXT);
		assert(pWndButton != NULL);
		pWndButton->EnableWindow(FALSE);
	}
}

/******************************
	CWndKawiBawiBoGameConfirm
*******************************/
CWndKawiBawiBoGameConfirm::CWndKawiBawiBoGameConfirm() 
{ 
}

CWndKawiBawiBoGameConfirm::~CWndKawiBawiBoGameConfirm() 
{ 
} 

void CWndKawiBawiBoGameConfirm::OnDestroy()
{
}

void CWndKawiBawiBoGameConfirm::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndKawiBawiBoGameConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate();
	// 여기에 코딩하세요
	CRect rect = GetClientRect();
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize(60,25);
	
	CRect rect2_1( x - size.cx - 10, y, ( x - size.cx - 10 ) + size.cx, y + size.cy );
	CRect rect2_2( x + 10          , y, ( x + 10           ) + size.cx, y + size.cy );
	
	rect.DeflateRect( 10, 10, 10, 35 );
	m_wndText.AddWndStyle( WBS_VSCROLL );
	m_wndText.Create( WBS_NODRAWFRAME, rect, this, 0 );
	m_strText = prj.GetText( TID_GAME_KAWIBAWIBO_STARTGUID2 );
	m_wndText.SetString( m_strText, 0xffffffff );//ADEILSON 0xff000000
	m_wndText.ResetString();
	
	m_wndButton1.Create("OK"    , 0, rect2_1, this, IDOK    );
	m_wndButton2.Create("CANCEL", 0, rect2_2, this, IDCANCEL);
	m_wndButton1.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtNormal05.tga" ) );
	m_wndButton2.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtNormal05.tga" ) );
	m_wndButton1.FitTextureSize();
	m_wndButton2.FitTextureSize();

	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndKawiBawiBoGameConfirm::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_MESSAGEBOX, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndKawiBawiBoGameConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndKawiBawiBoGameConfirm::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndKawiBawiBoGameConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndKawiBawiBoGameConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndKawiBawiBoGameConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == IDOK )
	{
		SAFE_DELETE( g_WndMng.m_pWndKawiBawiBoGame );
		g_WndMng.m_pWndKawiBawiBoGame = new CWndKawiBawiBoGame;
		g_WndMng.m_pWndKawiBawiBoGame->Initialize();
	}
	else if( nID == IDCANCEL )
	{
		//그냥 종료
	}
	Destroy();
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

/*************************
	CWndFindWordGame
*************************/
CWndFindWordGame::CWndFindWordGame()
{
	for(int i=0; i<5; i++)
	{
		m_itemID[i] = -1;
		m_pItemElem[i] = NULL;
	}
	m_pStaticID[0] = WIDC_WORD_NUM1;
	m_pStaticID[1] = WIDC_WORD_NUM2;
	m_pStaticID[2] = WIDC_WORD_NUM3;
	m_pStaticID[3] = WIDC_WORD_NUM4;
	m_pStaticID[4] = WIDC_WORD_NUM5;
	
	m_nSelectCtrl = -1;
	m_pText = NULL;
	m_nPublic = 0;
	m_bGetFirst = FALSE;
	m_bStart = TRUE;
	m_pWndInventory = NULL;
}

CWndFindWordGame::~CWndFindWordGame() 
{ 
} 

void CWndFindWordGame::OnDestroy()
{
	for(int i=0; i<5; i++)
	{
		if(m_pItemElem[i] != NULL)
		{
			if( !g_pPlayer->m_vtInfo.IsTrading( m_pItemElem[i] ) )
				m_pItemElem[i]->SetExtra(0);

			m_pItemElem[i] = NULL;
		}
	}
	if(m_pWndInventory != NULL)
		m_pWndInventory->m_wndItemCtrl.SetDieFlag(FALSE);
}

void CWndFindWordGame::SetFirstWord(int nPublic, char nChar)
{
	m_nPublic = nPublic;
	m_firstwordID = nChar - 'A' + II_SYS_SYS_EVE_A_CARD;
	m_bGetFirst = TRUE;				
}

BOOL CWndFindWordGame::Process()
{
	if(m_bStart)
	{	
		BOOL checkfull = TRUE;
		CWndButton* pButton;
		for(int i=0; i<5; i++)
		{
			if(m_pItemElem[i] == NULL)
				checkfull = FALSE;
		}
		if(checkfull)
		{
			pButton = (CWndButton*)GetDlgItem( WIDC_BTN_START );
			pButton->EnableWindow(TRUE);
			m_bStart = FALSE;
		}
		else
		{
			pButton = (CWndButton*)GetDlgItem( WIDC_BTN_START );
			pButton->EnableWindow(FALSE);
			m_bStart = FALSE;			
		}
	}

	return TRUE;
}

void CWndFindWordGame::OnDraw( C2DRender* p2DRender ) 
{
	ItemProp* pItemProp;
	CTexture* pTexture;
	
	//현재 선택된 Ctrl 기억하기.
	CPoint point = GetMousePoint();
	m_nSelectCtrl = HitTest( point );
			
	//Server로 부터 받은 힌트 단어는 흐리게 그리기.
	if(m_bGetFirst)
	{
		pItemProp = prj.GetItemProp( m_firstwordID );
		if(pItemProp != NULL)
		{
			pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
			if(pTexture != NULL)
				pTexture->Render( p2DRender, CPoint( GetWndCtrl( m_pStaticID[m_nPublic] )->rect.left, GetWndCtrl( m_pStaticID[m_nPublic] )->rect.top ), 100 );
		}
	}
	
	//나머지 올려진 단어 그리기.
	for(int i=0; i<5; i++)
	{
		if(m_pItemElem[i] != NULL)
		{
			pItemProp = m_pItemElem[i]->GetProp();
			if(pItemProp != NULL)
			{
				pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
				if(pTexture != NULL)
					pTexture->Render( p2DRender, CPoint( GetWndCtrl( m_pStaticID[i] )->rect.left, GetWndCtrl( m_pStaticID[i] )->rect.top ) );
			}
		}		
	}
} 

BOOL CWndFindWordGame::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CItemElem* pItemElem  = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
	
	if(pItemElem != NULL && (pItemElem->GetProp()->dwID >= II_SYS_SYS_EVE_A_CARD && pItemElem->GetProp()->dwID <= II_SYS_SYS_EVE_Z_CARD))
	{
		if( m_nSelectCtrl > -1 && m_itemID[m_nSelectCtrl] == -1)
		{
			m_pItemElem[m_nSelectCtrl] = pItemElem;
			pItemElem->SetExtra(pItemElem->GetExtra()+1);
			m_itemID[m_nSelectCtrl] = pItemElem->GetProp()->dwID;
			m_bStart = TRUE;
		}
	}

	return TRUE;
}
	
void CWndFindWordGame::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	if(m_nSelectCtrl > -1 && m_itemID[m_nSelectCtrl] != -1)
	{
		m_itemID[m_nSelectCtrl] = -1;
		if(m_pItemElem[m_nSelectCtrl] != NULL)
		{
			m_pItemElem[m_nSelectCtrl]->SetExtra(m_pItemElem[m_nSelectCtrl]->GetExtra()-1);		
			m_pItemElem[m_nSelectCtrl] = NULL;
			m_bStart = TRUE;
		}
	}
}
	
void CWndFindWordGame::SetWord(CItemElem* pItemElem)
{
	int nNum = -1;
	for(int i=0; i<5; i++)
	{
		if(m_itemID[i] == -1)
		{
			nNum = i;
			i = 5;
		}
	}

	if(nNum != -1)
	{
		m_itemID[nNum] = pItemElem->GetProp()->dwID;
		m_pItemElem[nNum] = pItemElem;
		pItemElem->SetExtra(pItemElem->GetExtra()+1);
		m_bStart = TRUE;
	}
}

void CWndFindWordGame::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	m_pText = (CWndText *)GetDlgItem( WIDC_TEXT_DESC );

	m_nQuestionID = 0;

	CWndButton* pButton;
	pButton = (CWndButton*)GetDlgItem( WIDC_BTN_START );
	pButton->EnableWindow(FALSE);

	SetDescription(NULL);

	g_DPlay.SendAlphabetOpenWnd();
	
	m_pWndInventory = (CWndInventory*)GetWndBase( APP_INVENTORY );
	if(m_pWndInventory != NULL)
		m_pWndInventory->m_wndItemCtrl.SetDieFlag(TRUE);

	MoveParentCenter();
} 

BOOL CWndFindWordGame::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_MINIGAME_WORD, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndFindWordGame::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndFindWordGame::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndFindWordGame::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndFindWordGame::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndFindWordGame::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if(message == WNM_CLICKED)
	{
		if(nID == WIDC_BTN_START)
		{
			BOOL checkall = TRUE;
			for(int i=0; i<5; i++)
			{
				if(m_pItemElem[i] == NULL)
					checkall = FALSE;
				else
					m_itemID[i] = m_pItemElem[i]->m_dwObjId;				
			}
			if(checkall)
			{
				CWndButton* pButton;
				pButton = (CWndButton*)GetDlgItem( WIDC_BTN_START );
				pButton->EnableWindow(FALSE);
				
				g_DPlay.SendAlphabetStart(m_itemID , 5, m_nQuestionID); //서버로 해당 글자들의 ID를 묶어서 보낸다.
			}
			else	
				g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_MINIGAME_EMPTY_ERROR ) ); //글자가 빠져있다면 Error Message를 띄운다.
		}
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

int CWndFindWordGame::HitTest( CPoint point )
{
	int rtn_val = -1;
	for(int i=0; i<5; i++)
	{
		if( GetWndCtrl( m_pStaticID[i] )->rect.PtInRect( point ) )
		{
			rtn_val = i;
			i = 5;
		}
	}
	return rtn_val;
}

void CWndFindWordGame::SetDescription( CHAR* szChar )
{
	CScript scanner;
	BOOL checkflag;
	if( szChar == NULL )
	{
		checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "MiniGameFindWordDesc.inc" ) ));
		szChar = scanner.m_pProg;
	}
	else
		checkflag = TRUE;
	
	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString( szChar );
		m_pText->ResetString();	
	}
}

void CWndFindWordGame::ReceiveResult(int nResult, DWORD dwItemId, int nItemCount)
{
	if(nResult == CMiniGame::ALPHABET_FAILED)
		g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_FINDWORD_FAIL ) );
	else if(nResult == CMiniGame::ALPHABET_NOTENOUGH_MONEY)
		g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_LACKMONEY ) );
	else
	{
		CString message;
		message.Format(prj.GetText( TID_GAME_PUZZLE_GIVEPRESENT ));
		g_WndMng.OpenMessageBox( message );						
	}
	
	Destroy();
}

/*************************
	CWndDiceGame
*************************/
CWndDiceGame::CWndDiceGame() 
{ 
	m_nSelectCtrl = -1;
	m_pWndDiceTender = NULL;
	m_pWndMessageBox = NULL;
	m_nCount = 0;
	m_nDelay = 1;
	m_nStatus = -1;
	m_nDiceNum = 0;
	m_nPrizePenya = 0;
	m_nAlpha = 0;
	m_nDiceChoiceNum = -1;
	m_bReciveResult = FALSE;
	m_bSendStart = FALSE;
	m_bEnd = FALSE;
	m_bFlsh = TRUE;
	m_pText = NULL;

	for(int i=0; i<6; i++)
	{
		m_nPenya[i] = 0;
		m_pCost[i] = NULL;
		m_pRCost[i] = NULL;
	}

	m_pBtnID[0] = WIDC_BTN_NUM1;
	m_pBtnID[1] = WIDC_BTN_NUM2;
	m_pBtnID[2] = WIDC_BTN_NUM3;
	m_pBtnID[3] = WIDC_BTN_NUM4;
	m_pBtnID[4] = WIDC_BTN_NUM5;
	m_pBtnID[5] = WIDC_BTN_NUM6;

	m_pStaticID[0] = WIDC_STATIC2;
	m_pStaticID[1] = WIDC_STATIC3;
	m_pStaticID[2] = WIDC_STATIC4;
	m_pStaticID[3] = WIDC_STATIC5;
	m_pStaticID[4] = WIDC_STATIC6;
	m_pStaticID[5] = WIDC_STATIC7;

	m_pStaticRID[0] = WIDC_STATIC8;
	m_pStaticRID[1] = WIDC_STATIC9;
	m_pStaticRID[2] = WIDC_STATIC10;
	m_pStaticRID[3] = WIDC_STATIC11;
	m_pStaticRID[4] = WIDC_STATIC12;
	m_pStaticRID[5] = WIDC_STATIC13;

	m_pStaticNum[0] = WIDC_STATIC14;
	m_pStaticNum[1] = WIDC_STATIC15;
	m_pStaticNum[2] = WIDC_STATIC16;
	m_pStaticNum[3] = WIDC_STATIC17;
	m_pStaticNum[4] = WIDC_STATIC18;
	m_pStaticNum[5] = WIDC_STATIC19;

	m_strPathDice[0] = MakePath( DIR_ICON, "Icon_1Dice90X90.dds");
	m_strPathDice[1] = MakePath( DIR_ICON, "Icon_2Dice90X90.dds");
	m_strPathDice[2] = MakePath( DIR_ICON, "Icon_3Dice90X90.dds");
	m_strPathDice[3] = MakePath( DIR_ICON, "Icon_4Dice90X90.dds");
	m_strPathDice[4] = MakePath( DIR_ICON, "Icon_5Dice90X90.dds");
	m_strPathDice[5] = MakePath( DIR_ICON, "Icon_6Dice90X90.dds");

	m_strPathDiceNum[0] = MakePath( DIR_ICON, "Icon_1Dice1.dds");
	m_strPathDiceNum[1] = MakePath( DIR_ICON, "Icon_2Dice2.dds");
	m_strPathDiceNum[2] = MakePath( DIR_ICON, "Icon_3Dice3.dds");
	m_strPathDiceNum[3] = MakePath( DIR_ICON, "Icon_4Dice4.dds");
	m_strPathDiceNum[4] = MakePath( DIR_ICON, "Icon_5Dice5.dds");
	m_strPathDiceNum[5] = MakePath( DIR_ICON, "Icon_6Dice6.dds");

	//Default
	m_nMinPenya = 1000000;
	m_nMaxPenya = 100000000;
	m_nMultiple = 5;
}
 
CWndDiceGame::~CWndDiceGame() 
{	
	if(m_pWndDiceTender != NULL)
		SAFE_DELETE(m_pWndDiceTender);
	if(m_pWndMessageBox != NULL)
		SAFE_DELETE(m_pWndMessageBox);
} 

void CWndDiceGame::OnDestroy()
{
	g_DPlay.SendFiveSystemDestroyWnd();
}

void CWndDiceGame::SetMinMaxPenya(int nMinPenya, int nMaxPenya, int nMultiple)
{
	m_nMinPenya = nMinPenya;
	m_nMaxPenya = nMaxPenya;
	m_nMultiple = nMultiple;

	CWndButton* pButton;
	pButton = (CWndButton*)GetDlgItem( WIDC_BTN_START );
	pButton->EnableWindow(TRUE);
}

void CWndDiceGame::OnDraw( C2DRender* p2DRender ) 
{ 
	CTexture* pTexture;
	CString strPath;
	
	strPath = m_strPathDice[m_nDiceNum];

	if(strPath.GetLength() > 0)
	{
		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, strPath, 0xffff00ff );
		if(pTexture != NULL)
			pTexture->Render( p2DRender, CPoint( GetWndCtrl( WIDC_STATIC_DICE )->rect.left + 3, GetWndCtrl( WIDC_STATIC_DICE )->rect.top + 4 ) );	
	}

	for(int i=0; i<6; i++)
	{
		strPath = m_strPathDiceNum[i];
		if(strPath.GetLength() > 0)
		{
			pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, strPath, 0xffff00ff );
			if(pTexture != NULL)
				pTexture->Render( p2DRender, CPoint( GetWndCtrl( m_pStaticNum[i] )->rect.left, GetWndCtrl( m_pStaticNum[i] )->rect.top) );			
		}
	}

	//선택된 숫자에 대한 표시.
	if(m_bEnd)
	{
		if(m_nDiceChoiceNum > -1)
		{
			D3DXCOLOR color;
			if( m_bFlsh == TRUE )
			{
				m_nAlpha += 15;
					
				if( m_nAlpha > 230 )
				{
					m_nAlpha = 230;
					m_bFlsh = FALSE;
				}
			}
			else
			{
				m_nAlpha -= 15;
				
				if( m_nAlpha < 70 )
				{
					m_nAlpha = 70;
					m_bFlsh = TRUE;
				}
			}
			
			color =  D3DCOLOR_ARGB( m_nAlpha, 240, 255, 255 );
				
			CRect rect;
			LPWNDCTRL lpWndCtrl;
			
			lpWndCtrl = GetWndCtrl( m_pStaticID[m_nDiceChoiceNum] );
			rect = lpWndCtrl->rect;
			p2DRender->RenderFillRect( rect, color );

			lpWndCtrl = GetWndCtrl( m_pStaticRID[m_nDiceChoiceNum] );
			rect = lpWndCtrl->rect;
			p2DRender->RenderFillRect( rect, color );

			lpWndCtrl = GetWndCtrl( m_pStaticNum[m_nDiceChoiceNum] );
			rect = lpWndCtrl->rect;
			rect.bottom -= 3; //실제 이미지가 rect보다 공간이 남으므로 조정
			rect.right -= 3;
			p2DRender->RenderFillRect( rect, color );
		}
	}
}

void CWndDiceGame::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	m_pCost[0] = (CWndText*)GetDlgItem( WIDC_COST_NUM1 );
	m_pCost[1] = (CWndText*)GetDlgItem( WIDC_COST_NUM2 );
	m_pCost[2] = (CWndText*)GetDlgItem( WIDC_COST_NUM3 );
	m_pCost[3] = (CWndText*)GetDlgItem( WIDC_COST_NUM4 );
	m_pCost[4] = (CWndText*)GetDlgItem( WIDC_COST_NUM5 );
	m_pCost[5] = (CWndText*)GetDlgItem( WIDC_COST_NUM6 );

	m_pRCost[0] = (CWndText*)GetDlgItem( WIDC_RCOST_NUM1 );
	m_pRCost[1] = (CWndText*)GetDlgItem( WIDC_RCOST_NUM2 );
	m_pRCost[2] = (CWndText*)GetDlgItem( WIDC_RCOST_NUM3 );
	m_pRCost[3] = (CWndText*)GetDlgItem( WIDC_RCOST_NUM4 );
	m_pRCost[4] = (CWndText*)GetDlgItem( WIDC_RCOST_NUM5 );
	m_pRCost[5] = (CWndText*)GetDlgItem( WIDC_RCOST_NUM6 );

	for(int i=0; i<6; i++)
	{
		m_pCost[i]->EnableWindow(FALSE);
		m_pRCost[i]->EnableWindow(FALSE);
	}

	m_pText = (CWndText *)GetDlgItem( WIDC_TEXT_DESC );
	SetDescription(NULL);

	CWndButton* pButton;
	pButton = (CWndButton*)GetDlgItem( WIDC_BTN_START );
	pButton->EnableWindow(FALSE);

	RefreshInfo();

	//서버로 창이 열렸음을 보낸다.
	g_DPlay.SendFiveSystemOpenWnd();
	
	MoveParentCenter();
} 

BOOL CWndDiceGame::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_MINIGAME_DICE, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndDiceGame::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndDiceGame::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndDiceGame::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndDiceGame::OnLButtonDown( UINT nFlags, CPoint point ) 
{
} 

BOOL CWndDiceGame::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if(message == WNM_CLICKED)
	{
		m_nSelectCtrl = -1;
		switch(nID) 
		{
			case WIDC_BTN_NUM1:
				m_nSelectCtrl = 0;
				break;
			case WIDC_BTN_NUM2:
				m_nSelectCtrl = 1;
				break;
			case WIDC_BTN_NUM3:
				m_nSelectCtrl = 2;
				break;
			case WIDC_BTN_NUM4:
				m_nSelectCtrl = 3;
				break;
			case WIDC_BTN_NUM5:
				m_nSelectCtrl = 4;
				break;
			case WIDC_BTN_NUM6:
				m_nSelectCtrl = 5;
				break;
			case WIDC_BTN_START:
				BOOL checkusable = FALSE;
				int i;
					
				for(i=0; i<6; i++)
				{
					if(m_nPenya[i] > 0)
						checkusable = TRUE;
				}
				
				if(checkusable)
				{
					CWndButton* pButton;
					pButton = (CWndButton*)GetDlgItem( WIDC_BTN_START );
					pButton->EnableWindow(FALSE);
					
					for(int i=0; i<6; i++)
					{
						pButton = (CWndButton*)GetDlgItem( m_pBtnID[i] );
						pButton->EnableWindow(FALSE);		
					}
					
					m_nStatus = 0;		//컴퓨터의 선택을 화면상에 돌리기 위함.
				}
				else
					g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_DICEGAME_ERROR ) );
				break;
		}
		
		if(m_nSelectCtrl >= 0 && m_nSelectCtrl < 6)
		{
			//금액 임력 창을 띄운다.
			if(m_pWndDiceTender != NULL)
				SAFE_DELETE(m_pWndDiceTender);
				
			m_pWndDiceTender = new CWndDiceGameTender;
			m_pWndDiceTender->Initialize(this);
			m_pWndDiceTender->SetSelectNum(m_nSelectCtrl);
			m_pWndDiceTender->SetMinMaxPenya(m_nMinPenya, m_nMaxPenya);
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndDiceGame::RefreshInfo()
{
	CString strtemp;
	for(int i=0; i<6; i++)
	{
		strtemp.Format("%d", m_nPenya[i]);
		strtemp = GetNumberFormatEx(strtemp);
		m_pCost[i]->SetString(strtemp, 0xff4d4dff);
		
		strtemp.Format("%d", m_nPenya[i] * m_nMultiple);
		strtemp = GetNumberFormatEx(strtemp);
		m_pRCost[i]->SetString(strtemp, 0xff9932cd);
	}
}

void CWndDiceGame::RefreshCtrl()
{
	CWndButton* pButton;
	pButton = (CWndButton*)GetDlgItem( WIDC_BTN_START );
	pButton->EnableWindow(TRUE);
	
	for(int i=0; i<6; i++)
	{
		pButton = (CWndButton*)GetDlgItem( m_pBtnID[i] );
		pButton->EnableWindow(TRUE);		
	}
	m_pWndMessageBox = NULL;
	m_bEnd = FALSE;
	m_bSendStart = FALSE;
}

void CWndDiceGame::CheckPenyaUsuable(int nPenya, int nSelect)
{
	//서버로 해당 금액을 전송하여 유효성 여부를 검사 받는다.	
	g_DPlay.SendFiveSystemBet(nSelect, nPenya);
}

void CWndDiceGame::SetTenderPenya(int nSelect, int nPenya)
{
	if(nSelect >= 0 && nSelect < 6)
	{
		m_nPenya[nSelect] = nPenya;
		RefreshInfo();	
	}
	//나머지 값이 올경우 Error값임.
}

void CWndDiceGame::ReceiveResult(int nDiceNum, int nPenya)
{
	if(nDiceNum >= 0 && nDiceNum < 6)
	{
		m_nDiceNum = nDiceNum;
		m_nDiceChoiceNum = nDiceNum;
		m_nPrizePenya = nPenya;
		m_bReciveResult = TRUE;
	}
}

BOOL CWndDiceGame::Process()
{
	//Start버튼 누를 경우 컴퓨터의 선택이 회전하도록 함.
	if(m_nStatus == 0)
	{
		if(m_nCount%4 == 0)
		{
			if(m_nCount > 170)
			{
				m_nCount = 0;
				m_nStatus = 1;
			}
			else
			{
				PLAYSND( "InfOpen.wav" );
				int randnum = rand() % 6;
				if(randnum == m_nDiceNum)
				{
					randnum++;
					( randnum > 5 ) ? randnum = 0 : randnum;
					
				}
				m_nDiceNum = randnum;			
			}
		}
		m_nCount++;
	}
	else if(m_nStatus == 1)
	{
		if(m_nCount > m_nDelay)
		{
			if(m_nDelay < 30)
			{
				PLAYSND( "InfOpen.wav" );
				m_nDelay += 1;
			}
			else
			{
				PLAYSND( "InfOpen.wav" );					
				m_nStatus = 2;
				m_nDelay = 30;
			}
			
			int randnum = rand() % 6;
			if(randnum == m_nDiceNum)
			{
				randnum++;
				( randnum > 5 ) ? randnum = 0 : randnum;
				
			}
			m_nDiceNum = randnum;
			
			m_nCount = 0;
		}
		m_nCount++;
	}
	else if(m_nStatus == 2) //Com의 결과가 보여지고 바로 사라지거나 하기 때문에 약간의 Delay를 줌
	{
		if(m_nCount > m_nDelay)
		{
			if(m_bReciveResult)
			{
				CString message;
				if(m_nPenya[m_nDiceChoiceNum] > 0)
				{
					CString strPenya;
					strPenya.Format("%d", m_nPrizePenya);
					strPenya = GetNumberFormatEx(strPenya);
					message.Format(prj.GetText( TID_GAME_DICEGAME_SUCCESS ), m_nDiceChoiceNum + 1, strPenya );				
					PlayMusic( BGM_IN_FITUP );
				}
				else
					message.Format(prj.GetText( TID_GAME_DICEGAME_FAIL ));

				if(m_pWndMessageBox != NULL)
					SAFE_DELETE( m_pWndMessageBox );					
				m_pWndMessageBox = new CWndDiceGameMessage;
				m_pWndMessageBox->Initialize( message, this, MB_OK );
				
				m_nStatus = -1;
				m_nDelay = 1;
				m_bReciveResult = FALSE;
				m_bEnd = TRUE;
			}
			else
			{
				if(!m_bSendStart) //서버로 주사위 게임의 시작을 알린다.
				{
					g_DPlay.SendFiveSystemStart();
					m_bSendStart = TRUE;
				}
			}
		}
		m_nCount++;
	}
	
	return TRUE;
}

void CWndDiceGame::SetDescription( CHAR* szChar )
{
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "MiniGameDiceDesc.inc" ) ));
	szChar = scanner.m_pProg;

	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString( szChar );
		m_pText->ResetString();	
	}
}

void CWndDiceGame::ResetPenya()
{
	for(int i=0; i<6; i++)
		m_nPenya[i] = 0;
}

/*************************
	CWndDiceGameTender
*************************/
CWndDiceGameTender::CWndDiceGameTender() 
{ 
	m_pDiceGame = NULL;
	m_nTenderPenya = 0;
	m_nSelect = -1;
}
 
CWndDiceGameTender::~CWndDiceGameTender() 
{ 
} 

void CWndDiceGameTender::OnDestroy()
{
}

void CWndDiceGameTender::OnDraw( C2DRender* p2DRender ) 
{ 
} 

void CWndDiceGameTender::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT );
	pWndEdit->SetFocus();
	MoveParentCenter();
} 

void CWndDiceGameTender::SetMinMaxPenya(int nMinPenya, int nMaxPenya)
{
	m_nMinPenya = nMinPenya;
	m_nMaxPenya = nMaxPenya;
}

BOOL CWndDiceGameTender::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	m_pDiceGame = (CWndDiceGame*)pWndParent;
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_TRADE_GOLD, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndDiceGameTender::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndDiceGameTender::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndDiceGameTender::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndDiceGameTender::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndDiceGameTender::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK || message == EN_RETURN )
	{
		CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT );
		
		CString string = pWndEdit->GetString();
		m_nTenderPenya = _ttoi( string );

		if( m_nTenderPenya < m_nMinPenya && m_nTenderPenya != 0)
		{
			string.Format( _T( prj.GetText(TID_GAME_DICETENDER_ERROR) ), m_nMinPenya );
			g_WndMng.OpenMessageBox( string );
		}
		else
		{
			if(m_nSelect > -1 && m_pDiceGame != NULL)
			{
				CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT );
				
				CString string = pWndEdit->GetString();
				m_nTenderPenya = _ttoi( string );
			
				m_pDiceGame->CheckPenyaUsuable(m_nTenderPenya, m_nSelect);
			}
			Destroy();
		}
	}
	else if( nID == WIDC_EDIT )
	{
		CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT );
		
		CString string = pWndEdit->GetString();
		int m_nTenderPenya = _ttoi( string );

		if(m_nTenderPenya > m_nMaxPenya) //입찰 금액이 최대 입찰 금액을 초과할 경우
			m_nTenderPenya = m_nMaxPenya;			

		string.Format("%d", m_nTenderPenya);
		pWndEdit->SetString(string);
	}
	else if( nID == WIDC_CANCEL )
		Destroy();
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndDiceGameTender::SetSelectNum(int nSelect)
{
	CString string;
	
	m_nSelect = nSelect;

	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT );
	string.Format("%d", m_pDiceGame->m_nPenya[m_nSelect]);
	pWndEdit->SetString(string);
}

/*************************
	CWndDiceGameMessage
*************************/
CWndDiceGameMessage::CWndDiceGameMessage() 
{ 
}
 
CWndDiceGameMessage::~CWndDiceGameMessage() 
{ 
} 

void CWndDiceGameMessage::OnDestroy()
{
}

void CWndDiceGameMessage::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndDiceGameMessage::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate();
	// 여기에 코딩하세요
	CRect rect = GetClientRect();
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize(60,25);
	
	CRect rect1( x - ( size.cx / 2), y, ( x - ( size.cx / 2 ) ) + size.cx, y + size.cy );
	
	rect.DeflateRect( 10, 10, 10, 35 );
	m_wndText.AddWndStyle( WBS_VSCROLL );
	m_wndText.Create( WBS_NODRAWFRAME, rect, this, 0 );
	m_wndText.SetString( m_strText, 0xffffffff );//ADEILSON 0xff000000
	m_wndText.ResetString();
	
	m_wndButton.Create("OK", 0, rect1, this, IDOK);
	m_wndButton.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtNormal05.tga" ) );
	m_wndButton.FitTextureSize();

	CWndDiceGame* pWndGame = (CWndDiceGame*)GetWndBase( APP_MINIGAME_DICE );
	if(pWndGame != NULL)
	{
		CRect rectGame = pWndGame->GetWindowRect( TRUE );
		CPoint ptGame = rectGame.BottomRight();
		CPoint ptMove;
		ptMove = ptGame - CPoint(290, 150);
		Move( ptMove );
	}
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndDiceGameMessage::Initialize( LPCTSTR lpszMessage, CWndBase* pWndParent, DWORD nType ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	m_strText = lpszMessage;
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_MESSAGEBOX, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndDiceGameMessage::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndDiceGameMessage::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndDiceGameMessage::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndDiceGameMessage::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndDiceGameMessage::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == IDOK )
	{
		CWndDiceGame* pWndGame = (CWndDiceGame*)GetWndBase( APP_MINIGAME_DICE );
		if(pWndGame != NULL)
		{
			pWndGame->ResetPenya();
			pWndGame->RefreshCtrl();
			pWndGame->RefreshInfo();
		}		
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

/*************************
	CWndPuzzleGame
*************************/
CWndPuzzleGame::CWndPuzzleGame() 
{ 
	for(int i=0; i<9; i++)
		m_pItemElem[i] = NULL;

	m_pStaticID[0] = WIDC_PIC_NUM1;
	m_pStaticID[1] = WIDC_PIC_NUM2;
	m_pStaticID[2] = WIDC_PIC_NUM3;
	m_pStaticID[3] = WIDC_PIC_NUM4;
	m_pStaticID[4] = WIDC_PIC_NUM5;
	m_pStaticID[5] = WIDC_PIC_NUM6;
	m_pStaticID[6] = WIDC_PIC_NUM7;
	m_pStaticID[7] = WIDC_PIC_NUM8;
	m_pStaticID[8] = WIDC_PIC_NUM9;
	
	m_nSelectCtrl = -1;
	m_pText = NULL;
	m_bStart = TRUE;
	m_pWndInventory = NULL;
}
 
CWndPuzzleGame::~CWndPuzzleGame() 
{ 
} 

void CWndPuzzleGame::OnDestroy()
{
	for(int i=0; i<9; i++)
	{
		if(m_pItemElem[i] != NULL)
		{
			if( !g_pPlayer->m_vtInfo.IsTrading( m_pItemElem[i] ) )
				m_pItemElem[i]->SetExtra(0);

			m_pItemElem[i] = NULL;
		}
	}
	if(m_pWndInventory != NULL)
		m_pWndInventory->m_wndItemCtrl.SetDieFlag(FALSE);
}

void CWndPuzzleGame::SetPuzzle(vector<DWORD> vPuzzle)
{
	int i=0;

	std::vector<DWORD>::iterator iter = vPuzzle.begin();
	while(iter != vPuzzle.end())
	{
		m_itemID[i] = *iter;
		i++;
		iter++;
	}
}

BOOL CWndPuzzleGame::Process()
{
	if(m_bStart)
	{	
		BOOL checkfull = TRUE;
		CWndButton* pButton;
		for(int i=0; i<9; i++)
		{
			if(m_pItemElem[i] == NULL)
				checkfull = FALSE;
		}
		if(checkfull)
		{
			pButton = (CWndButton*)GetDlgItem( WIDC_BTN_START );
			pButton->EnableWindow(TRUE);
			m_bStart = FALSE;
		}
		else
		{
			pButton = (CWndButton*)GetDlgItem( WIDC_BTN_START );
			pButton->EnableWindow(FALSE);
			m_bStart = FALSE;			
		}
	}

	return TRUE;
}

void CWndPuzzleGame::OnDraw( C2DRender* p2DRender ) 
{ 
	//현재 선택된 Ctrl 기억하기.
	CPoint point = GetMousePoint();
	m_nSelectCtrl = HitTest( point );
	
	//나머지 올려진 퍼즐 그리기.
	ItemProp* pItemProp;
	CTexture* pTexture;
	
	for(int i=0; i<9; i++)
	{
		if(m_pItemElem[i] != NULL)
		{
			pItemProp = prj.GetItemProp( m_itemID[i] );
			if(pItemProp != NULL)
			{
				//아이템 아이콘이 아닌 큰 크기의 그림이 따로 들어갈 경우 여기를 수정.
				pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
				if(pTexture != NULL)
					pTexture->Render( p2DRender, CPoint( GetWndCtrl( m_pStaticID[i] )->rect.left, GetWndCtrl( m_pStaticID[i] )->rect.top ) );
			}
		}		
	}
} 

BOOL CWndPuzzleGame::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CItemElem* pItemElem  = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );

	//if( ::GetLanguage() == LANG_KOR )
	{
		int nNum = 0;
		for(int i=0; i<9; i++)
		{
			if(m_pItemElem[i] == NULL && m_itemID[i] == pItemElem->m_dwItemId)
			{
				if(pItemElem->m_nItemNum > nNum)
				{
					m_itemID[i] = pItemElem->GetProp()->dwID;
					m_pItemElem[i] = pItemElem;
					nNum++;
				}
				else
					i = 9;
			}
		}

		if(nNum > 0)
		{
			pItemElem->SetExtra(nNum);
			m_bStart = TRUE;
		}
	}
	/*
	else
	{
		int nNum = -1;
		for(int i=0; i<9; i++)
		{
			if(m_pItemElem[i] == NULL && m_itemID[i] == pItemElem->m_dwItemId)
			{
				nNum = i;
				i = 9;
			}
		}
		
		if(nNum != -1)
		{
			m_itemID[nNum] = pItemElem->GetProp()->dwID;
			m_pItemElem[nNum] = pItemElem;
			pItemElem->SetExtra(1);
			m_bStart = TRUE;
		}
	}
	*/
	
	return TRUE;
}

void CWndPuzzleGame::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	if(m_nSelectCtrl > -1 && m_itemID[m_nSelectCtrl] != -1)
	{
		if(m_pItemElem[m_nSelectCtrl] != NULL)
		{
			//if( ::GetLanguage() == LANG_KOR )
			{
				DWORD dwItemId = m_pItemElem[m_nSelectCtrl]->m_dwItemId;
				for(int i=0; i<9; i++)
				{
					if(m_itemID[i] == dwItemId)
					{
						m_pItemElem[i]->SetExtra(0);
						m_pItemElem[i] = NULL;
					}
				}
			}
			/*
			else
			{
				m_pItemElem[m_nSelectCtrl]->SetExtra(0);			
				m_pItemElem[m_nSelectCtrl] = NULL;
				m_bStart = TRUE;
			}
			*/
		}
	}
}

void CWndPuzzleGame::SetPicture(CItemElem* pItemElem)
{
	//if( ::GetLanguage() == LANG_KOR )
	{
		int nNum = 0;
		for(int i=0; i<9; i++)
		{
			if(m_pItemElem[i] == NULL && m_itemID[i] == pItemElem->m_dwItemId)
			{
				if(pItemElem->m_nItemNum > nNum)
				{
					m_itemID[i] = pItemElem->GetProp()->dwID;
					m_pItemElem[i] = pItemElem;
					nNum++;
				}
				else
					i = 9;
			}
		}

		if(nNum > 0)
		{
			pItemElem->SetExtra(nNum);
			m_bStart = TRUE;
		}
	}
	/*
	else
	{
		int nNum = -1;
		for(int i=0; i<9; i++)
		{
			if(m_pItemElem[i] == NULL && m_itemID[i] == pItemElem->m_dwItemId)
			{
				nNum = i;
				i = 9;
			}
		}
		
		if(nNum != -1)
		{
			m_itemID[nNum] = pItemElem->GetProp()->dwID;
			m_pItemElem[nNum] = pItemElem;
			pItemElem->SetExtra(pItemElem->GetExtra()+1);
			m_bStart = TRUE;
		}
	}
	*/
}

void CWndPuzzleGame::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	m_pText = (CWndText *)GetDlgItem( WIDC_TEXT_DESC );
	
	CWndButton* pButton;
	pButton = (CWndButton*)GetDlgItem( WIDC_BTN_START );
	pButton->EnableWindow(FALSE);

	SetDescription(NULL);
	g_DPlay.SendReassembleOpenWnd();
	
	MoveParentCenter();
} 

BOOL CWndPuzzleGame::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_MINIGAME_PUZZLE, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndPuzzleGame::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndPuzzleGame::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndPuzzleGame::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndPuzzleGame::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndPuzzleGame::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if(message == WNM_CLICKED)
	{
		if(nID == WIDC_BTN_START)
		{
			BOOL checkall = TRUE;
			for(int i=0; i<9; i++)
			{
				if(m_pItemElem[i] == NULL)
					checkall = FALSE;
			}
			if(checkall)
			{
				CWndButton* pButton;
				pButton = (CWndButton*)GetDlgItem( WIDC_BTN_START );
				pButton->EnableWindow(FALSE);
				
				//서버로 해당 글자들의 ID를 묶어서 보낸다.
				for(int i=0; i<9; i++)
				{
					m_objItemID[i] = m_pItemElem[i]->m_dwObjId;
				}
				g_DPlay.SendReassembleStart(m_objItemID, 9);
			}
			else
			{
				//글자가 빠져있다면 Error Message를 띄운다.
				g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_MINIGAME_EMPTY_ERROR ) );
			}
		}
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

int CWndPuzzleGame::HitTest( CPoint point )
{
	int rtn_val = -1;
	for(int i=0; i<9; i++)
	{
		if( GetWndCtrl( m_pStaticID[i] )->rect.PtInRect( point ) )
		{
			rtn_val = i;
			i = 9;
		}
	}
	return rtn_val;
}

void CWndPuzzleGame::ReceiveResult()
{
	g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_PUZZLE_GIVEPRESENT ) );
	PlayMusic( BGM_IN_FITUP );
	
	Destroy();
}

void CWndPuzzleGame::SetDescription( CHAR* szChar )
{
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "MiniGamePuzzleDesc.inc" ) ));
	szChar = scanner.m_pProg;

	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString( szChar );
		m_pText->ResetString();	
	}
}

#endif //__EVE_MINIGAME
