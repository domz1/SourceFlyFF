#include "stdafx.h"
#include "BillingMgrJP2.h"
#include "MsgHdr.h"
#include "account.h"
#include "DBManager.h"
#include "dpsrvr.h"
#include "dpdbsrvr.h"

extern	CAccountMng	g_AccountMng;
extern	CDbManager	g_DbManager;
extern	CDPSrvr		g_dpSrvr;
extern	CDPDBSrvr	g_dpDbSrvr;
///////////////////////////////////////////////////////////////////////
// CBillingMgrJP2
///////////////////////////////////////////////////////////////////////
CBillingMgrJP2::CBillingMgrJP2()
{
	m_iBillingFreePass	= 0;
}

CBillingMgrJP2::~CBillingMgrJP2()
{
}

BOOL CBillingMgrJP2::SetConfig( BILLING_ENUM id, DWORD data )
{
	switch( id )
	{
	case BID_FREEPASS:
		m_iBillingFreePass = (int)data;
		break;
	}
	return TRUE;
}

// 초기화: 빌링 서버와의 연결 객체를 할당해 둔다.
bool CBillingMgrJP2::Init( HWND hWnd )
{
	::SetTimer( hWnd, IDT_KICKOUT, 1000 * 60, NULL );
	return true;
}

// 할당된 자원을 반납한다.
void CBillingMgrJP2::Release()
{
	safe_delete( this );
}

// 빌링정보를 검사한다.
BYTE CBillingMgrJP2::CheckAccount( int nType, DWORD dwKey, const char* szAccount, const char* szAddr )
{
	if( nType != m_iBillingFreePass )  // 무료유저? 
	{ 
		// 유료유저는 디비에 쿼리 
		g_DbManager.PostBillingQuery( szAccount, dwKey, 1 );
		return ACCOUNT_BILLING_WAIT_ACK;
	}
	else														
	{
		// 무료유저는 TRACE
		char szTrace[1024];
		sprintf( szTrace, "FreePass - Account:%s fCheck:%d", szAccount, nType );
		OutputDebugString( szTrace );

		return ACCOUNT_CHECK_OK;
	}
}


// 빌링에 관련된 윈도우 메세지가 처리되게 한다.
BOOL CBillingMgrJP2::PreTranslateMessage( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) 
{
	switch( message ) 
	{
		case WM_TIMER:
			{
				switch( wParam )
				{
				case IDT_KICKOUT:
					g_AccountMng.KickOutCheck();	
					return TRUE;
				}
			}
			break;
	}

	return FALSE;
}

// 
void CBillingMgrJP2::OnTimer( CAccount* pAccount )
{
	if( pAccount->m_fCheck == m_iBillingFreePass )	// 무료사용자 skip
		return;

	CTime tm = 0;
	if( pAccount->m_TimeOverDays == tm )			// login 미처리자 or timeover skip
		return;		

	CTime cur = CTime::GetCurrentTime();
	if( pAccount->m_TimeOverDays <= cur )			// timerover 처리 
	{	
		pAccount->m_TimeOverDays = 0;
		g_dpSrvr.CloseExistingConnection( pAccount->m_lpszAccount, ERROR_BILLING_TIME_OVER );
	}
	else
	{
		// 남은 시간 통지
		CTimeSpan ts = pAccount->m_TimeOverDays - cur;
		if ( ts.GetTotalMinutes() >= 59 && ts.GetTotalMinutes() <= 61 && 
			 pAccount->m_nStatus != ACCOUNT_STATUS_NOTIFIED &&
			 pAccount->m_fRoute == TRUE )
		{
			pAccount->m_nStatus = ACCOUNT_STATUS_NOTIFIED;				
			g_dpDbSrvr.SendOneHourNotify( pAccount );	// TRANS에 보낸다.
		}
	}			
}

// sp 리턴값 설명:
// fError(결과),fText(결과를 문자열로),fTime(endday까지의시간을 초로)
// 예: 0,OK,540530
void CBillingMgrJP2::OnDBQuery( CQuery& query, tagDB_OVERLAPPED_PLUS* pOV )
{
	long lResult = SUCCESS;
	CTime endDay = CTime::GetCurrentTime();

	sprintf( pOV->szQuery, "BILLING_STR '%s'", pOV->szAccount );	

#if defined(_DEBUG)
	CTimeSpan timeSpan( 2, 0, 0, 0 );
	endDay += timeSpan;	
#else
	if( query.Exec( pOV->szQuery ) )
	{
		if( query.Fetch() )
		{
			int fError = query.GetInt( "fError" );
			switch( fError )
			{
			case 0:
				{
				int nSecs = query.GetInt( "fTime" );		
				CTimeSpan timeSpan( 0, 0, 0, nSecs );
				endDay += timeSpan;

				lResult = SUCCESS;
				}
				break;
			case 1:
				lResult = TIME_OVER;
				break;
			case 2:
				lResult = BILLING_INFO_FAILED;
				break;
			default:
				lResult = OTHER_ERROR;
				break;
			}
		}
		else
		{
			lResult = BILLING_INFO_FAILED;
		}
	}
	else
	{
		lResult = DATABASE_ERROR;
		WriteLog( "%s, %d\r\n\t%s", __FILE__, __LINE__, pOV->szQuery );
	}                
#endif

	BILLING_INFO result;
	result.szAccount = pOV->szAccount;
	result.dwKey = pOV->dwKey;
	result.lResult = lResult;
	result.pTimeOverDays = &endDay;	// endDay는 lResult가 SUCCESS인 경우에만 설정되면 된다.
	
	g_AccountMng.SendBillingResult( &result );
}
