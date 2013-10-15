#include "stdafx.h"
#include "definetext.h"
#include "AppDefine.h"
#include "wndvendor.h"
#include "WndVendorBuy.h"
#include "dpclient.h"
extern	CDPClient	g_DPlay;


/*
  WndId : APP_VENDOR_BUY - 구입 확인
  CtrlId : WIDC_EDIT1 - 개수 입력 창
  CtrlId : WIDC_STATIC4 - 사는 가격
  CtrlId : WIDC_STATIC6 - 단가
  CtrlId : WIDC_OK - Button
  CtrlId : WIDC_CANCEL - Button
*/

CWndVendorBuy::CWndVendorBuy( CItemBase* pItemBase, int iIndex )
{
	m_pItemBase		= pItemBase;
	m_iIndex	= iIndex;
}

CWndVendorBuy::~CWndVendorBuy()
{

} 

void CWndVendorBuy::OnDraw( C2DRender* p2DRender )
{

}

void CWndVendorBuy::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_bIsFirst      = TRUE;
	m_Calc.m_nValue = 0;
	m_Calc.m_ch     = 0;
	SetFocus();

	char str[64]	= { 0, };
	sprintf( str, "%d", m_pItemBase->GetExtra() );
	CWndStatic* pWndStatic;
	pWndStatic	= (CWndStatic*)GetDlgItem( WIDC_BUYNUM );
	pWndStatic->SetTitle( CString( str ) );
	pWndStatic->SetFocus();
	pWndStatic->AddWndStyle(WSS_MONEY);

	pWndStatic	= (CWndStatic*)GetDlgItem( WIDC_STATIC4 );
	sprintf( str, "%d", m_pItemBase->m_nCost );
	pWndStatic->SetTitle( CString( str ) );
	pWndStatic->AddWndStyle(WSS_MONEY);

	pWndStatic	= (CWndStatic*)GetDlgItem( WIDC_STATIC5 );
	sprintf( str, "%d", m_pItemBase->m_nCost * m_pItemBase->GetExtra() );
	pWndStatic->SetTitle( CString( str ) );
	pWndStatic->AddWndStyle(WSS_MONEY);
	
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
}

BOOL CWndVendorBuy::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ )
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_VENDOREX_BUY, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndVendorBuy::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

void CWndVendorBuy::OnSize( UINT nType, int cx, int cy )
{
	CWndNeuz::OnSize( nType, cx, cy );
}

void CWndVendorBuy::OnLButtonUp( UINT nFlags, CPoint point )
{

}

void CWndVendorBuy::OnLButtonDown( UINT nFlags, CPoint point )
{

}

BOOL CWndVendorBuy::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	CString str;

	SetFocus();
	
	CWndStatic* pWndStatic;
	pWndStatic = (CWndStatic*)GetDlgItem( WIDC_BUYNUM );

	str = pWndStatic->GetTitle();
	int nMaxBuyNum;
	if( m_pItemBase )
		nMaxBuyNum = m_pItemBase->GetExtra();

	switch( nID )
	{
	case WIDC_NUM0:
		OnChar('0');
		break;
	case WIDC_NUM1:
		OnChar('1');
		break;
	case WIDC_NUM2:
		OnChar('2');
		break;
	case WIDC_NUM3:
		OnChar('3');
		break;
	case WIDC_NUM4:
		OnChar('4');
		break;
	case WIDC_NUM5:
		OnChar('5');
		break;
	case WIDC_NUM6:
		OnChar('6');
		break;
	case WIDC_NUM7:
		OnChar('7');
		break;
	case WIDC_NUM8:
		OnChar('8');
		break;
	case WIDC_NUM9:
		OnChar('9');
		break;
	case WIDC_NUM00:
		{
			if(str == "0")
				break;

			str+="00";
			pWndStatic->SetTitle(str);
			break;
		}
	case WIDC_NUM000:
		{
			if(str == "0")
				break;

			str+="000";
			pWndStatic->SetTitle(str);
			break;
		}
	case WIDC_VENMIN:
		{
			pWndStatic->SetTitle( "1" );
			break;
		}
	case WIDC_VENMAX:
		{
			if(m_pItemBase)
			{
				CString str;
				str.Format( "%d", m_pItemBase->GetExtra() );
				
				pWndStatic->SetTitle(str);
			}
			break;
		}
	}

	char strstr[64]	= { 0, };
	int nNum	= atoi( pWndStatic->GetTitle() );
	if(m_pItemBase && m_pItemBase->GetExtra() < nNum )
	{
		CString str;
		str.Format( "%d", m_pItemBase->GetExtra() );
		pWndStatic->SetTitle(str);
		nNum = m_pItemBase->GetExtra();
	}
	pWndStatic	= (CWndStatic*)GetDlgItem( WIDC_STATIC5 );
	sprintf( strstr, "%d", m_pItemBase->m_nCost * nNum );
	pWndStatic->SetTitle( CString( strstr ) );
	
	if( (message == WNM_CLICKED && nID == WIDC_OK) || message == EN_RETURN )
	{
		pWndStatic	= (CWndStatic*)GetDlgItem( WIDC_BUYNUM );
		CWndVendor* pWndVendor	= (CWndVendor*)GetParentWnd();
		int nNum	= atoi( pWndStatic->GetTitle() );
		if( nNum < 1 )	return TRUE;	// 개수가 너무 작습니다.
#if __VER >= 8 // __S8_VENDOR_REVISION
		int nBufNum = nNum;
		if( nNum > m_pItemBase->GetExtra() )
			nBufNum = m_pItemBase->GetExtra();
		if( g_pPlayer->GetGold() < ((CItemElem*)m_pItemBase)->m_nCost * nBufNum )
		{
			g_WndMng.OpenMessageBox( _T( prj.GetText(TID_GAME_LACKMONEY)), MB_OK, this );
			return TRUE;			
		}
#endif // __VER >= 8 // __S8_VENDOR_REVISION
		g_DPlay.SendBuyPVendorItem( pWndVendor->m_pVendor->GetId(), m_iIndex, m_pItemBase->m_dwItemId, nNum );
		Destroy();
	}
	else if( nID == WIDC_CANCEL )
	{
		Destroy();
	}
	

	
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

int CWndVendorBuy::ProcessCalc(CALC_DATA calc, int num)
{
	int i;

	switch( calc.m_ch )
	{
	case '*':
		i = m_Calc.m_nValue * num;
		return i;
	case '/':
		i = m_Calc.m_nValue / num;
		return i;
	case '+':
		i = m_Calc.m_nValue + num;
		return i;
	case '-':
		i = m_Calc.m_nValue - num;
		return i;
	}
	
	return 0;
}

void CWndVendorBuy::OnChar(UINT nChar)
{
	int nLen;
	CString str;
	BOOL    bKeychar = FALSE;
	CWndStatic* pWndStatic;
	pWndStatic = (CWndStatic*)GetDlgItem( WIDC_BUYNUM );
	
	str = pWndStatic->GetTitle();
	nLen = str.GetLength();
	char buffer[20] = { 0 };

	SetFocus();
	
	switch( nChar )
	{
		case 27:
			{
				m_bIsFirst = FALSE;
				m_Calc.m_nValue = 0;
				pWndStatic->SetTitle("0");
				break;
			}

		case 8:
			if( nLen > 1 )
				str.Delete( nLen-1 );
			else
				str = "0";

			pWndStatic->SetTitle(str);
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			bKeychar = TRUE;
			break;
	}	

	if( bKeychar )
	{
		if( m_bIsFirst == FALSE )
		{
			str = "0";
			m_bIsFirst = TRUE;
		}

		(str != "0") ? str.Format("%s%c", str, nChar) : str.Format("%c", nChar);
		pWndStatic->SetTitle(str);
	}
	
	char strstr[64]	= { 0, };
	int nNum	= atoi( pWndStatic->GetTitle() );
	if(m_pItemBase && m_pItemBase->GetExtra() < nNum )
	{
		CString str;
		str.Format( "%d", m_pItemBase->GetExtra() );
		pWndStatic->SetTitle(str);
		nNum = m_pItemBase->GetExtra();
	}
	
	pWndStatic	= (CWndStatic*)GetDlgItem( WIDC_STATIC5 );
	sprintf( strstr, "%d", m_pItemBase->m_nCost * nNum );
	pWndStatic->SetTitle( CString( strstr ) );
}

