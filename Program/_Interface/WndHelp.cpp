#include "stdafx.h"
#include "AppDefine.h"
#include "WndHelp.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 도움말
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndHelp::CWndHelp()
{
}
CWndHelp::~CWndHelp()
{
}
void CWndHelp::OnDraw(C2DRender* p2DRender)
{
	DWORD dwLeft = m_rectClient.Width() * 50 / 100;
	CRect rect;

	rect.SetRect( dwLeft + 5, 5, m_rectClient.Width() - 5, 23 );

//	p2DRender->RenderFillRect ( rect, D3DCOLOR_ARGB(255,150,150,200), D3DCOLOR_ARGB(255,100,100,150), D3DCOLOR_ARGB(255,150,150,200), D3DCOLOR_ARGB(255,100,100,150));
	//p2DRender->RenderRoundRect( rect, D3DCOLOR_ARGB(255,200,200,250) );

	p2DRender->TextOut( rect.left + 10, rect.top + 8, m_strKeyword, 0xffffffff );//ADEILSON 0xff000000
	/*

	rect.SetRect( dwLeft + 5, 25, m_rectClient.Width() - 5, m_rectClient.Height() - 5 );

	p2DRender->RenderFillRect ( rect, D3DCOLOR_ARGB(255,170,170,230) );
	p2DRender->RenderRoundRect( rect, D3DCOLOR_ARGB(255,200,200,250) );

	CStringArray* pStrArray = prj.GetHelp(m_strKeyword);
	if(pStrArray)
	{
		m_strArray.RemoveAll();
		ClipStrArray(&g_Neuz.m_2DRender,rect,0,pStrArray,&m_strArray);
		CString string;
		for(int i = 0; i < m_strArray.GetSize(); i++)
		{
			string = m_strArray.GetAt(i);
			p2DRender->TextOut(rect.left + 5, rect.top + 5 + i * 16, string);
		}
	}
	*/
}
void CWndHelp::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	DWORD dwRight = m_rectClient.Width() * 50 / 100;
	//m_wndViewCtrl.Create( WBS_CHILD, CRect( 5, 5, dwRight,m_rectClient.Height() - 5 ), this, 1005 );//,m_pSprPack,-1);//m_pSprPack,16);

	CWndTreeCtrl* pWndTreeCtrl = (CWndTreeCtrl*)GetDlgItem( WIDC_TREE1 );
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	pWndText->AddWndStyle( WBS_VSCROLL );
	pWndTreeCtrl->LoadTreeScript( MakePath( DIR_CLIENT,  _T( "treeHelp.inc" ) ) );
	CRect rect( 210, 0, 300, 100 );
	//m_wndText.AddWndStyle( WBS_VSCROLL );
	//m_wndText.Create( 0, rect, this, 10 );
	//rect = m_wndText.GetClientRect();

	//m_wndText.m_string.SetString( _T("나는\n 자랑스러운\n 태극기 앞에 조국과 민족의 무궁한 영광을 위하여 몸과 마음을 바쳐 충성을 다할 것을 굳게 다짐합니다.\n 할랠루야 ") );
	//m_wndText.m_string.Reset( m_pFont, &rect );
	MoveParentCenter();

	//SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "WndHelp.bmp" ), TRUE );
	//FitTextureSize();
}
BOOL CWndHelp::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rect = m_pWndRoot->MakeCenterRect(400,350);
//etTitle( GETTEXT( TID_APP_HELPER_HELP ) );
	//return CWndNeuz::Create(0|WBS_MOVE|WBS_SOUND|WBS_CAPTION|WBS_THICKFRAME,rect,pWndParent,dwWndId);
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_HELPER_HELP, 0, CPoint( 0, 0 ), pWndParent );

}

BOOL CWndHelp::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	/*
	switch(nID)
	{
		case 0: // wnd1 
			g_WndMng.OpenWnd1();
			break;
		case 1: // wnd2
			g_WndMng.OpenCharacter();
			break;
		case 2: // 챗 
			g_WndMng.OpenChat();
			break;
		case 3:
			g_clientMsg.PutMessage(&g_Neuz.m_2DRender,0xffffffff,"fuck");
			break;
		case 9:
			g_WndMng.OpenCustomBox("종료하시겠습니까?",new CWndExitBox);
			break;
	}
	*/
	return CWndNeuz::OnCommand(nID,dwMessage,pWndBase);
}
void CWndHelp::OnSize(UINT nType, int cx, int cy)
{
	DWORD dwRight = m_rectClient.Width() * 50 / 100;
	//m_wndViewCtrl.SetWndRect( CRect( 5, 5, dwRight,m_rectClient.Height() - 5 ) );

	//DWORD dwLeft = m_rectClient.Width() * 50 / 100;
	CRect rect;
	//rect.SetRect( dwRight + 5, 25, m_rectClient.Width() - 5, m_rectClient.Height() - 5 );
	//m_wndText.SetWndRect( rect );

	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndHelp::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(IsWndRoot())
		return;

	//if(IsWndStyle(WBS_CAPTION) && m_bPickup)
	{//
	//	m_wndTitleBar.m_wndMinimize.SetVisible(TRUE);
		//m_wndTitleBar.m_wndMaximize.SetVisible(TRUE);
	}
}
void CWndHelp::OnLButtonDown(UINT nFlags, CPoint point)
{
//	CWndBase::OnLButtonDown(nFlags,point
	if(IsWndRoot())
		return;
//	return;
}
BOOL CWndHelp::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	switch(nID)
	{
		case WIDC_TREE1: // view ctrl
			{
				LPTREEELEM lpTreeElem = (LPTREEELEM)pLResult;
				if(lpTreeElem)
				{
					//m_strArray.RemoveAll();
					m_strKeyword = lpTreeElem->m_strKeyword;

					CString string = prj.GetHelp( m_strKeyword );

					CWndTreeCtrl* pWndTreeCtrl = (CWndTreeCtrl*)GetDlgItem( WIDC_TREE1 );
					CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
					pWndText->SetString( string );
					//pWndText->m_string.SetString( string );
					//pWndText->m_string.Reset( m_pFont, &pWndText->GetClientRect() );
					pWndText->UpdateScrollBar();
					//m_wndText.m_string.SetString( string );
					//m_wndText.m_string.Reset( m_pFont, &m_wndText.GetClientRect() );
					//m_wndText.UpdateScrollBar();

					/*
					CStringArray* pStrArray = prj.GetHelp(lpTreeElem->m_strKeyword);
					if(pStrArray)
					{
						CRect rect = GetClientRect();
						rect.left = 220;
						rect.top = 60;
						rect.right -= 50;
						rect.bottom -= 100;
						//CFont* pOldFont =
							//m_pMemDC->SelectObject(&g_pMainFrame->m_fontStatus);
						ClipStrArray(&g_Neuz.m_2DRender,rect,0,pStrArray,&m_strArray);
						//m_pMemDC->SelectObject(pOldFont);
						//ClipStrArray(m_pMemDC,rect,0,pStrArray,&m_strArray);
					}
					*/
				}
			}
			break;
	}
	return CWndNeuz::OnChildNotify(message,nID,pLResult);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 도움말
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndHelpFAQ::CWndHelpFAQ()
{
}
CWndHelpFAQ::~CWndHelpFAQ()
{
}
void CWndHelpFAQ::OnDraw(C2DRender* p2DRender)
{
	/*
	DWORD dwLeft = m_rectClient.Width() * 50 / 100;
	CRect rect;

	rect.SetRect( dwLeft + 5, 5, m_rectClient.Width() - 5, 23 );

//	p2DRender->RenderFillRect ( rect, D3DCOLOR_ARGB(255,150,150,200), D3DCOLOR_ARGB(255,100,100,150), D3DCOLOR_ARGB(255,150,150,200), D3DCOLOR_ARGB(255,100,100,150));
	//p2DRender->RenderRoundRect( rect, D3DCOLOR_ARGB(255,200,200,250) );

	p2DRender->TextOut( rect.left + 10, rect.top + 10, m_strKeyword, 0xff000000 );
	*/
	/*

	rect.SetRect( dwLeft + 5, 25, m_rectClient.Width() - 5, m_rectClient.Height() - 5 );

	p2DRender->RenderFillRect ( rect, D3DCOLOR_ARGB(255,170,170,230) );
	p2DRender->RenderRoundRect( rect, D3DCOLOR_ARGB(255,200,200,250) );

	CStringArray* pStrArray = prj.GetHelp(m_strKeyword);
	if(pStrArray)
	{
		m_strArray.RemoveAll();
		ClipStrArray(&g_Neuz.m_2DRender,rect,0,pStrArray,&m_strArray);
		CString string;
		for(int i = 0; i < m_strArray.GetSize(); i++)
		{
			string = m_strArray.GetAt(i);
			p2DRender->TextOut(rect.left + 5, rect.top + 5 + i * 16, string);
		}
	}
	*/
}
void CWndHelpFAQ::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	DWORD dwRight = m_rectClient.Width() * 50 / 100;
	//m_wndViewCtrl.Create( WBS_CHILD, CRect( 5, 5, dwRight,m_rectClient.Height() - 5 ), this, 1005 );//,m_pSprPack,-1);//m_pSprPack,16);

	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LIST1 );
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
//	pWndText->AddWndStyle( WBS_VSCROLL );
	//pWndTreeCtrl->LoadTreeScript( MakePath( DIR_CLIENT,  _T( "treeFaq.inc" ) ) );
	CRect rect( 210, 0, 300, 100 );
//	pWndListBox->AddWndStyle( WBS_NODRAWFRAME );
	//pWndListBox->m_nFontColor = 0xff000000;
	//pWndListBox->m_nSelectColor = 0xff0000ff;

	LoadFAQ( MakePath( DIR_CLIENT,  _T( "Faq.inc" ) ) );
	//m_wndText.AddWndStyle( WBS_VSCROLL );
	//m_wndText.Create( 0, rect, this, 10 );
	//rect = m_wndText.GetClientRect();

	//m_wndText.m_string.SetString( _T("나는\n 자랑스러운\n 태극기 앞에 조국과 민족의 무궁한 영광을 위하여 몸과 마음을 바쳐 충성을 다할 것을 굳게 다짐합니다.\n 할랠루야 ") );
	//m_wndText.m_string.Reset( m_pFont, &rect );

	//pWndTreeCtrl->m_nFontColor = 0xff000000;
	//pWndTreeCtrl->m_nSelectColor = 0xff8080f0;

	//SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "WndHelp.bmp" ), TRUE );
	//FitTextureSize();
	MoveParentCenter();
}
BOOL CWndHelpFAQ::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rect = m_pWndRoot->MakeCenterRect(400,350);
//etTitle( GETTEXT( TID_APP_HELPER_HELP ) );
	//return CWndNeuz::Create(0|WBS_MOVE|WBS_SOUND|WBS_CAPTION|WBS_THICKFRAME,rect,pWndParent,dwWndId);
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_HELPER_FAQ, 0, CPoint( 0, 0 ), pWndParent );

}

BOOL CWndHelpFAQ::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	/*
	switch(nID)
	{
		case 0: // wnd1 
			g_WndMng.OpenWnd1();
			break;
		case 1: // wnd2
			g_WndMng.OpenCharacter();
			break;
		case 2: // 챗 
			g_WndMng.OpenChat();
			break;
		case 3:
			g_clientMsg.PutMessage(&g_Neuz.m_2DRender,0xffffffff,"fuck");
			break;
		case 9:
			g_WndMng.OpenCustomBox("종료하시겠습니까?",new CWndExitBox);
			break;
	}
	*/
	return CWndNeuz::OnCommand(nID,dwMessage,pWndBase);
}
void CWndHelpFAQ::OnSize(UINT nType, int cx, int cy)
{
	DWORD dwRight = m_rectClient.Width() * 50 / 100;
	//m_wndViewCtrl.SetWndRect( CRect( 5, 5, dwRight,m_rectClient.Height() - 5 ) );

	//DWORD dwLeft = m_rectClient.Width() * 50 / 100;
	CRect rect;
	//rect.SetRect( dwRight + 5, 25, m_rectClient.Width() - 5, m_rectClient.Height() - 5 );
	//m_wndText.SetWndRect( rect );

	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndHelpFAQ::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(IsWndRoot())
		return;

	//if(IsWndStyle(WBS_CAPTION) && m_bPickup)
	{//
	//	m_wndTitleBar.m_wndMinimize.SetVisible(TRUE);
		//m_wndTitleBar.m_wndMaximize.SetVisible(TRUE);
	}
}
void CWndHelpFAQ::OnLButtonDown(UINT nFlags, CPoint point)
{
//	CWndBase::OnLButtonDown(nFlags,point
	if(IsWndRoot())
		return;
//	return;
}
BOOL CWndHelpFAQ::LoadFAQ( LPCTSTR lpszFileName )
{
	CScript s;
	if( s.Load( lpszFileName ) == FALSE )
		return FALSE;
	CString strKeyword;
	s.GetToken();	// keyword
	while( s.tok != FINISHED )
	{
		strKeyword	= s.token;
		s.GetToken();	// {
		s.GetToken();	//
		CString string = "    " + s.Token;
		m_mapFAQ.SetAt( strKeyword, string );
		CWndListBox* pWndListBox	= (CWndListBox*)GetDlgItem( WIDC_LIST1 );
		CString strFAQ;
		strFAQ = "Q.";
		strFAQ	+= strKeyword;
		pWndListBox->AddString( strFAQ );
		s.GetToken();	// }
		s.GetToken();
	}
	return TRUE;
}

BOOL CWndHelpFAQ::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	switch(nID)
	{
		case WIDC_LIST1: // view ctrl
			{
				CString strKey, string;
				CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LIST1 );
				pWndListBox->GetText( pWndListBox->GetCurSel(), strKey );
				{
					strKey = strKey.Right( strKey.GetLength() - 2 );
					m_mapFAQ.Lookup( strKey, string );
					m_strKeyword = strKey;

					CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
					pWndText->m_string.Init( m_pFont, &pWndText->GetClientRect() );
					pWndText->m_string.SetString("");
					pWndText->m_string.AddString(m_strKeyword, 0xff8080ff, ESSTY_BOLD);
					pWndText->m_string.AddString("\n\n");
					pWndText->m_string.AddString( string );
					pWndText->UpdateScrollBar();
				}
			}
			break;
	}
	return CWndNeuz::OnChildNotify(message,nID,pLResult);
}

/*
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// CWndHelp
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndHelp::CWndHelp()
{
	if(g_pPlayer)
		g_pPlayer->SetNewbieStatus(NEWBIE_HELP);
}
CWndHelp::~CWndHelp()
{
}

LRESULT CWndHelp::WindowProc(UINT message,WPARAM wParam,LPARAM lParam)
{
	return CWndBase::WindowProc(message,wParam,lParam);
}
void CWndHelp::OnDraw(CMemDC* pMemDC)
{
	pMemDC->PaintRect(m_wndViewCtrl.GetWndRect(),MKHIGHRGB(0x4,0x3,0x3));
	pMemDC->SetTextColor(MKHIGHRGB(30,30,30));
	CPoint pt = GetStrCenter(pMemDC,m_strTitle);
	pt.y = 30;
	pMemDC->TextOutPut(pt.x,pt.y,m_strTitle);
	CFont* pOldFont =
		pMemDC->SelectObject(&g_pMainFrame->m_fontStatus);
	CString string;
	for(int i = 0; i < m_strArray.GetSize(); i++)
	{
		string = m_strArray.GetAt(i);
		pMemDC->TextOutPut(220,60+i*16,string);
	}
	pMemDC->SelectObject(pOldFont);
}
BOOL CWndHelp::Initialize(CWndBase* pWndParent)
{
	static int tileTable[13*9] = 
	{ 
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 
		3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5,
		3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5,
		3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5,
		3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5,
		3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5,
		3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5,
		3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5,
		6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8
	};
	if(IsOpenWnd())
		return FALSE;
	m_strTitle = "도움말";
	Create(WBS_MOVE|WBS_SOUND,CRect(100,100,230,230),g_pWndMgr,WID_HELP,g_pWndMgr->m_pSprWndDialog1,CSize(13,9),tileTable);
	if(g_pFieldWnd) MoveRectCenter(RECT_FIELD);
	else MoveRectCenter(RECT_CLIENT);
	return TRUE;
}
void CWndHelp::OnInitialUpdate()
{
	m_wndButton.Create("확인",WBS_SPRITE,CRect(0,0,0,0),this,1002,m_pSprPack,9);
	// 마지막 nSprIdx를 -1로 넣는 것은 m_pSprPack의 세팅은 필요하지만 출력은 하지 말아야 하기 때문이다.
	CRect rect = GetClientRect();
	m_wndViewCtrl.Create(WBS_CHILD,CRect(40,60,210,rect.bottom-55),this,1005,m_pSprPack,-1);//m_pSprPack,16);
	m_wndViewCtrl.SetFont(&g_pMainFrame->m_fontStatus);
	m_wndViewCtrl.LoadTreeScript("Client\\treeHelp.inc");
	rect = GetClientRect();
	rect.top = rect.bottom - 63;
	m_wndButton.MoveRectCenter(rect);
//#ifdef _NEWTITLEMENU
//	m_wndEditCtrl.Create(g_pMainFrame->m_hWnd,g_pMemDC->m_hDC,WBS_CHILD,CRect(0,0,210,rect.bottom-55),this,1005);//m_pSprPack,16);Init(g_pMainFrame->m_hWnd,0,0,g_pMemDC->m_hDC);
//#endif
}
BOOL CWndHelp::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	switch(nID)
	{
		case 1000:       
			break;
		case 1002:   
			Destroy();
			break;
		case 1005: // view ctrl 
			break;
	}
	return CWndBase::OnCommand(nID,dwMessage,pWndBase);
}
BOOL CWndHelp::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	switch(nID)
	{
		case 1005: // view ctrl
			{
				LPTREEELEM lpTreeElem = (LPTREEELEM)pLResult;
				if(lpTreeElem)
				{
					m_strArray.RemoveAll();
					CStringArray* pStrArray = prj.GetHelp(lpTreeElem->m_strKeyword);
					if(pStrArray)
					{
						CRect rect = GetClientRect();
						rect.left = 220;
						rect.top = 60;
						rect.right -= 50;
						rect.bottom -= 100;
						CFont* pOldFont =
							m_pMemDC->SelectObject(&g_pMainFrame->m_fontStatus);
						ClipStrArray(m_pMemDC,rect,0,pStrArray,&m_strArray);
						m_pMemDC->SelectObject(pOldFont);
						//ClipStrArray(m_pMemDC,rect,0,pStrArray,&m_strArray);
					}
				}
			}
			break;
	}
	return CWndBase::OnChildNotify(message,nID,pLResult);
}
*/

CWndHelpTip::CWndHelpTip() 
{ 
} 
CWndHelpTip::~CWndHelpTip() 
{ 
} 
void CWndHelpTip::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndHelpTip::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );
	pWndText->AddWndStyle( WBS_NOFRAME );
	pWndText->AddWndStyle( WBS_NODRAWFRAME );
	CScript s;
	if( s.Load( MakePath( DIR_CLIENT, _T("tip.inc" ) ) ) == FALSE )
		return;
	s.GetToken();
	while( s.tok != FINISHED )
	{
		CString string = "    " + s.Token;
		m_aString.Add( string );
		s.GetToken();
	}
	m_nPosString = 0;
	pWndText->SetString( m_aString.GetAt( 0 ), 0xffffffff );//ADEILSON 0xff000000
} 
BOOL CWndHelpTip::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	InitDialog( g_Neuz.GetSafeHwnd(), APP_HELPER_TIP );
	MoveParentCenter();
	return TRUE;
} 
BOOL CWndHelpTip::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	switch( nID )
	{
	case WIDC_NEXT:
		{
			m_nPosString++;
			if( m_nPosString == m_aString.GetSize() )
				m_nPosString = 0;
			CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );
			pWndText->SetString( m_aString.GetAt( m_nPosString ), 0xffffffff );//ADEILSON 0xff000000
		}
		break;
	case WIDC_CLOSE:
		Destroy();
	}
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndHelpTip::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndHelpTip::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndHelpTip::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndHelpTip::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case WIDC_CHECK1:
		g_Option.m_bTip = !g_Option.m_bTip;
		Destroy();
		break;
	//case WIDC_CLOSE:
	//	Destroy();
	//	return TRUE;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
/****************************************************
  WndId : APP_HELP_INSTANT - Instant Help
  CtrlId : WIDC_TEXT1 - 
  CtrlId : WIDC_BUTTON1 - Button
****************************************************/

CWndHelpInstant::CWndHelpInstant() 
{ 
} 
CWndHelpInstant::~CWndHelpInstant() 
{ 
} 
void CWndHelpInstant::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndHelpInstant::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요


	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	pWndText->SetString( prj.GetHelp( m_strHelpKey ) );
	
	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndHelpInstant::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_HELP_INSTANT, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndHelpInstant::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndHelpInstant::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndHelpInstant::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndHelpInstant::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndHelpInstant::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndHelpInstant::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_CLOSE )
	{
#ifdef __HELP_BUG_FIX
		Destroy();
#else //__HELP_BUG_FIX
		// 헬프 이중생성 방지
		vector<CString>::iterator where = find(g_vecHelpInsKey.begin(), g_vecHelpInsKey.end(), m_strHelpKey );

		if(where != g_vecHelpInsKey.end())
			g_vecHelpInsKey.erase(where);

		Destroy( TRUE );		
#endif //__HELP_BUG_FIX
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

