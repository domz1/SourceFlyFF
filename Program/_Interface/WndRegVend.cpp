#include "stdafx.h"
#include "definetext.h"
#include "AppDefine.h"
#include "WndVendor.h"
#include "WndRegVend.h"
#include "dpclient.h"
extern	CDPClient	g_DPlay;



/*
	WndId : APP_REGVEND
	CtrlId : WIDC_EDIT1 - 개수
	CtrlId : WIDC_EDIT2 - 단가
	CtrlId : WIDC_OK
	CtrlId : WIDC_BUTTON2
*/
CWndRegVend::CWndRegVend()
{
	m_pItemBase	= NULL;
	m_iIndex	= -1;

	m_dwFocus   = -1;
	m_pt[0] = m_pt[1] = CPoint(0,0);
}

CWndRegVend::~CWndRegVend()
{

}

void CWndRegVend::SetItem( int iIndex, CItemBase* pItemBase )
{
	CWndStatic* pStatic = (CWndStatic*)GetDlgItem(WIDC_SELLNUM);

	char str[32]	= { 0, };
	sprintf( str, "%d", ( (CItemElem*)pItemBase )->m_nItemNum );
	pStatic->SetTitle( str );
	m_dwFocus = 0;
	
	ItemProp* pItemProp	= pItemBase->GetProp();
	if( pItemProp )
	{
		pStatic	= (CWndStatic*)GetDlgItem(WIDC_SELLPRI);
#if __VER >= 8 // __S8_VENDOR_REVISION
		sprintf( str, "%d", 0 );
#else // __VER >= 8 // __S8_VENDOR_REVISION
		sprintf( str, "%d", pItemProp->dwCost );
#endif // __VER >= 8 // __S8_VENDOR_REVISION
		pStatic->SetTitle( str );

		if(( (CItemElem*)pItemBase )->m_nItemNum <= 1 )
			m_dwFocus = 1;
	}
		
	m_iIndex	= iIndex;
	m_pItemBase		= pItemBase;
}

void CWndRegVend::OnDraw( C2DRender* p2DRender ) 
{
	if( m_dwFocus != -1 )
	{
		m_pTex->Render( p2DRender, m_pt[m_dwFocus] );
	}
}

void CWndRegVend::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	
	CWndStatic* pStatic = (CWndStatic*)GetDlgItem(WIDC_SELLNUM);
	CRect rect = pStatic->GetWindowRect(TRUE);
	m_pt[0] = pStatic->GetWindowRect(TRUE).TopLeft();
	m_pt[0].y -= 4;
	m_pt[0].x -= 4;
	pStatic->AddWndStyle(WSS_MONEY);

	pStatic = (CWndStatic*)GetDlgItem(WIDC_SELLPRI);
	rect = pStatic->GetWindowRect(TRUE);
	m_pt[1] = pStatic->GetWindowRect(TRUE).TopLeft();
	m_pt[1].y -= 4;
	m_pt[1].x -= 4;
	pStatic->AddWndStyle(WSS_MONEY);
	
	m_bIsFirst      = TRUE;
	m_Calc.m_nValue = 0;
	m_Calc.m_ch     = 0;
	SetFocus();

	m_pTex = m_textureMng.AddTexture( m_pApp->m_pd3dDevice,  MakePath( DIR_THEME, "WndVenderArrowEx.tga" ), 0xffff00ff );

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 

BOOL CWndRegVend::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ )
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_VENDOREX_SELL, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndRegVend::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

void CWndRegVend::OnSize( UINT nType, int cx, int cy )
{
	CWndNeuz::OnSize( nType, cx, cy );
}

void CWndRegVend::OnLButtonUp( UINT nFlags, CPoint point )
{
	{
		CPoint point2 = GetMousePoint();
		LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_SELLNUM );
		CRect rectCtrl = lpWndCtrl->rect;
		
		if( rectCtrl.PtInRect( point2 ) )
		{
			m_dwFocus = 0;
			return;
		}

		lpWndCtrl = GetWndCtrl(WIDC_SELLPRI);
		rectCtrl = lpWndCtrl->rect;
		if( rectCtrl.PtInRect( point2 ) )
		{
			m_dwFocus = 1;
			return;
		}
	}
}

void CWndRegVend::OnLButtonDown( UINT nFlags, CPoint point )
{
}

BOOL CWndRegVend::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	CString str;

	SetFocus();
	
	CWndStatic* pWndStatic;
	pWndStatic = (m_dwFocus == 0) ? (CWndStatic*)GetDlgItem( WIDC_SELLNUM ): (CWndStatic*)GetDlgItem( WIDC_SELLPRI );

	str = pWndStatic->GetTitle();

	switch( nID )
	{
	case WIDC_VENEQUA:
		{
			if( m_bIsFirst )
			{
				char buffer[20] = { 0 };
				int i = ProcessCalc( m_Calc, atoi(str) );

				if(i < 0)
					_itoa( 2100000000, buffer, 10 );
				else
					_itoa( i, buffer, 10 );

				str = buffer;
				pWndStatic->SetTitle(str);
				m_bIsFirst = FALSE;
				m_Calc.m_nValue = 0;
			}
		}
		break;
	case WIDC_VENPLUS:
		OnChar('+');
		break;
	case WIDC_VENMINUS:
		OnChar('-');
		break;
	case WIDC_VENMULTI:
		OnChar('*');
		break;
	case WIDC_VENDIVID:
		OnChar('/');
		break;
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

			CString strSum;
			strSum = str + "00";
			if(strlen(strSum) <= 9)
			{
				str+="00";
				pWndStatic->SetTitle(str);
			}
			break;
		}
	case WIDC_NUM000:
		{
			if(str == "0")
				break;

			CString strSum;
			strSum = str + "000";
			if(strlen(strSum) <= 9)
			{
				str+="000";
				pWndStatic->SetTitle(str);
			}
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
				ItemProp *pItemProp = m_pItemBase->GetProp();

				int nValue = 0;
#if __VER >= 8 // __S8_VENDOR_REVISION
				nValue = ( m_dwFocus == 0 ) ? ( (CItemElem*)m_pItemBase )->m_nItemNum: 2100000000;
#else // __VER >= 8 // __S8_VENDOR_REVISION
				nValue = ( m_dwFocus == 0 ) ? ( (CItemElem*)m_pItemBase )->m_nItemNum: pItemProp->dwCost;
#endif // __VER >= 8 // __S8_VENDOR_REVISION
				
				CString str;
				str.Format( "%d", nValue );
				
				pWndStatic->SetTitle(str);
				
			}
			break;
		}
	}
	
	if( nID == WIDC_OK || message ==  EN_RETURN )
	{
		pWndStatic	= (CWndStatic*)GetDlgItem( WIDC_SELLNUM );
		LPCTSTR str	= pWndStatic->GetTitle();
		int nNum	= 1;
		nNum	= atoi( str );
		CItemElem* pItemElem	= (CItemElem*)m_pItemBase;
		if( nNum < 1 )	return TRUE;	// 개수가 너무 작습니다.
		if( nNum > pItemElem->m_nItemNum )	nNum	= pItemElem->m_nItemNum;

		int nCost	= 0;
		pWndStatic	= (CWndStatic*)GetDlgItem( WIDC_SELLPRI );
		str		= pWndStatic->GetTitle();
#if __VER < 8 // __S8_VENDOR_REVISION
		if( strlen( str ) > 9 )	return TRUE;	// 숫자가 너무 큽니다.
#endif // __VER < 8 // __S8_VENDOR_REVISION
		//nCost	= atoi( str );

		__int64  n64Cost = 0;
		n64Cost = _atoi64(str);
		if( n64Cost > INT_MAX )
			n64Cost = INT_MAX;
		nCost = static_cast<int>( n64Cost );

		if( nCost < 1 )	return TRUE;	// 비정상적인 수치가 입력되었습니다.
		
		ItemProp* pItemProp	= m_pItemBase->GetProp();

		
#if __VER >= 8 // __S8_VENDOR_REVISION
//		if( 999999999 < ((EXPINTEGER)nCost*nNum) )
//		{
//			g_WndMng.OpenMessageBox( _T(prj.GetText(TID_GAME_VENDOR_MAX_ONE_GOLD)), MB_OK, this );
//			return TRUE;
//		}
		
		int nGold = g_pPlayer->GetGold();
		for( int iv = 0 ; iv < MAX_VENDITEM ; ++iv )
		{
			CItemBase *pItemBase = g_pPlayer->m_vtInfo.GetItem( iv );
			if( pItemBase == NULL )
				continue;

			nGold += ((CItemElem*)pItemBase)->m_nCost * pItemBase->GetExtra();
		}					
		int nOldGold = nGold;
		//nGold += (nCost * nNum);

		INT64 n64Sum(0);
		INT64 n64Gold(nGold);
		INT64 n64itemCost(nCost);
		INT64 n64Num(nNum);
		n64Sum = n64Gold + (n64itemCost * n64Num);

		if( n64Sum <= 0 || 2100000000 <= n64Sum || nOldGold >= n64Sum )
		{
			g_WndMng.OpenMessageBox( _T(prj.GetText(TID_GAME_VENDOR_MAX_ALL_GOLD)), MB_OK, this );
			return TRUE;
		}

//		if( nGold <= 0 || nOldGold >= nGold || 2100000000 <= nGold )
//		{
//			g_WndMng.OpenMessageBox( _T(prj.GetText(TID_GAME_VENDOR_MAX_ALL_GOLD)), MB_OK, this );
//			return TRUE;
//		}
#endif // __VER >= 8 // __S8_VENDOR_REVISION
		
#if __VER < 8     //8차게임내아이템판매가격제한풀기
		if( nCost > pItemProp->dwCost * 1000 )
		{
			g_WndMng.OpenMessageBox( _T( prj.GetText(TID_GAME_LIMITSELL)), MB_OK, this );
			return TRUE;
		}
#endif	//	   __VER < 8  

		g_DPlay.SendRegisterPVendorItem( m_iIndex, 0, (BYTE)( m_pItemBase->m_dwObjId ), nNum, nCost );
		Destroy( FALSE );
	}
	else if( nID == WIDC_CANCEL )	// WIDC_CANCEL
	{
		Destroy( FALSE );
	}
	

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}


int CWndRegVend::ProcessCalc(CALC_DATA calc, int num)
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

void CWndRegVend::OnChar(UINT nChar)
{
	int i;
	int nLen;
	CString str;
	BOOL    bKeychar = FALSE;
	CWndStatic* pWndStatic;
	pWndStatic = (m_dwFocus == 0) ? (CWndStatic*)GetDlgItem( WIDC_SELLNUM ): (CWndStatic*)GetDlgItem( WIDC_SELLPRI );
	
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
		case '*':
			if( m_bIsFirst && m_Calc.m_nValue != 0 )
			{
				i = ProcessCalc( m_Calc, atoi(str) );
				_itoa( i, buffer, 10 );
				str = buffer;
				pWndStatic->SetTitle(str);
				m_Calc.m_nValue = 0;
			}				
			m_bIsFirst = FALSE;
			m_Calc.m_nValue = atoi(str);
			m_Calc.m_ch     = nChar;
			break;
		case '/':
			if( m_bIsFirst && m_Calc.m_nValue != 0 )
			{
				i = ProcessCalc( m_Calc, atoi(str) );
				_itoa( i, buffer, 10 );
				str = buffer;
				pWndStatic->SetTitle(str);
				m_Calc.m_nValue = 0;
			}				
			m_bIsFirst = FALSE;
			m_Calc.m_nValue = atoi(str);
			m_Calc.m_ch     = nChar;
			break;
		case '+':
			if( m_bIsFirst && m_Calc.m_nValue != 0 )
			{
				i = ProcessCalc( m_Calc, atoi(str) );
				_itoa( i, buffer, 10 );
				str = buffer;
				pWndStatic->SetTitle(str);
				m_Calc.m_nValue = 0;
			}				
			m_bIsFirst = FALSE;
			m_Calc.m_nValue = atoi(str);
			m_Calc.m_ch     = nChar;
			break;
		case '-':
			if( m_bIsFirst && m_Calc.m_nValue != 0 )
			{
				i = ProcessCalc( m_Calc, atoi(str) );
				_itoa( i, buffer, 10 );
				str = buffer;
				pWndStatic->SetTitle(str);
				m_Calc.m_nValue = 0;
			}
			m_bIsFirst = FALSE;
			m_Calc.m_nValue = atoi(str);
			m_Calc.m_ch     = nChar;
			break;
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
		
		if(str != "0") 
		{
			if(strlen(str) <= 9)
				str.Format("%s%c", str, nChar);
		}
		else
			str.Format("%c", nChar);

		pWndStatic->SetTitle(str);
	}
	
}

