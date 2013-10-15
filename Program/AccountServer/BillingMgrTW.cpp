#include "stdafx.h"
#include "BillingMgrTW.h"
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
// BillingMgrTW
///////////////////////////////////////////////////////////////////////
CBillingMgrTW::CBillingMgrTW()
{
	m_iBillingFreePass	= 0;
}

CBillingMgrTW::~CBillingMgrTW()
{
}

BOOL CBillingMgrTW::SetConfig( BILLING_ENUM id, DWORD data )
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
bool CBillingMgrTW::Init( HWND hWnd )
{
	::SetTimer( hWnd, IDT_KICKOUT, 1000 * 60, NULL );
	return true;
}

// 할당된 자원을 반납한다.
void CBillingMgrTW::Release()
{
	safe_delete( this );
}

// 빌링정보를 검사한다.
BYTE CBillingMgrTW::CheckAccount( int nType, DWORD dwKey, const char* szAccount, const char* szAddr )
{
	g_DbManager.PostBillingQuery( szAccount, dwKey, 1 );
	return ACCOUNT_BILLING_WAIT_ACK;

/*
	if( nType != GetFreePass() )  // 무료유저? 
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
*/
}


// 빌링에 관련된 윈도우 메세지가 처리되게 한다.
BOOL CBillingMgrTW::PreTranslateMessage( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) 
{
	switch( message ) 
	{
		case WM_TIMER:
			{
				switch( wParam )
				{
				case IDT_KICKOUT:
					g_AccountMng.KickOutCheck();	// 최종적으로 CBillingMgrTW::OnTimer가 호출된다.	
					return TRUE;
				}
			}
			break;
	}

	return FALSE;
}


// 
void CBillingMgrTW::OnTimer( CAccount* pAccount )
{
	CTime tm = 0;
	if( pAccount->m_TimeOverDays == tm )		// login 미처리자 or timeover skip
		return;		

	CTime cur = CTime::GetCurrentTime();
	if( pAccount->m_TimeOverDays <= cur )		// timerover 처리 
	{	
		pAccount->m_nStatus = ACCOUNT_STATUS_SECONDQUERY;	
		g_DbManager.PostBillingQuery( pAccount->m_lpszAccount, pAccount->m_dwAuthKey, 2 );
	}
	else
	{
		if( pAccount->m_cbLastOneLogon == 'Y' )	// Billing-Game 협의서_20041104.doc 4page 참조 
		{
			// 30분정도 남았을 때 - 남은 시간 통지
			CTimeSpan ts = pAccount->m_TimeOverDays - cur;
			if ( ts.GetTotalMinutes() >= 29 && ts.GetTotalMinutes() <= 31 && 
				 pAccount->m_nStatus != ACCOUNT_STATUS_NOTIFIED &&
				 pAccount->m_fRoute == TRUE )
			{
				pAccount->m_nStatus = ACCOUNT_STATUS_NOTIFIED;				
				g_dpDbSrvr.SendOneHourNotify( pAccount );	// TRANS에 보낸다.
			}
		}
	}			
}

/*
    bill           int 
		0: 게임정지 당한유저,로그인을 할수없음
		151: 로그인을 할수있는유저
		156: 유저의 자료가부족함으로,로그인이안됨
    End_Time       datetime
    LastOneLogon   char(1)
		'Y': 마지막으로 게임에 로그인이됨

    select @account      as account,
           @bill         as bill,
           @End_Time     as End_Time,
           @LastOneLogon as LastOneLogon

*/
void CBillingMgrTW::OnDBQuery( CQuery& query, tagDB_OVERLAPPED_PLUS* pOV )
{
	long lResult = OTHER_ERROR;
	CTime endDay;

	sprintf( pOV->szQuery, "OGB_SP_LOGON_BILLING_CHECK '%s', %d", pOV->szAccount, pOV->nExtra );	

	TIMESTAMP_STRUCT ts;
	char szAccount[64] = "";
	char szLastOneLogon[8] = "";
	int  nBill = -1;

	QUERY_BINDINFO infos[4] = {
		{SQL_C_CHAR,      szAccount,      63,				NULL},
		{SQL_C_LONG,      &nBill,         sizeof(nBill),	NULL},
		{SQL_C_TIMESTAMP, &ts,            sizeof(ts),		NULL},
		{SQL_C_CHAR,      szLastOneLogon, 7,				NULL}
	};

	if( query.Exec( pOV->szQuery, 4, infos ) )
	{
		if( query.Fetch() )
		{
			switch( nBill )
			{
			case 0:
				lResult = OTHER_ERROR;
				break;
			case 151:
				{
				endDay = CTime(ts.year, ts.month, ts.day, ts.hour, ts.minute, ts.second);
#ifdef _DEBUG
				CTimeSpan timeSpan( 0, 0, 0, 5 * 60 );
				endDay += timeSpan;	
#endif
				if( endDay > CTime::GetCurrentTime() )
					lResult = SUCCESS;
				else
					lResult = TIME_OVER;
				}
				break;
			case 156:
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

	char szTrace[256];
	if( lResult == SUCCESS || lResult == TIME_OVER )
	{
		sprintf( szTrace, "%s param:%d bill:%d (%04d/%02d/%02d-%02d:%02d:%02d) LastOneLogon:%c\n",
			  szAccount, pOV->nExtra, nBill,
			  endDay.GetYear(), endDay.GetMonth(), endDay.GetDay(), 
			  endDay.GetHour(), endDay.GetMinute(), endDay.GetSecond(),
			  szLastOneLogon[0] );			  
	}
	else
	{
		sprintf( szTrace, "%s param:%d bill:%d \n", szAccount, pOV->nExtra, nBill ); 
	}
	OutputDebugString( szTrace );

	BILLING_INFO info;
	info.szAccount = pOV->szAccount;
	info.dwKey = pOV->dwKey;
	info.lResult = lResult;
	info.pTimeOverDays = &endDay;	// endDay는 lResult가 SUCCESS인 경우에만 설정되면 된다.
	info.cbLastOneLogon = szLastOneLogon[0];

	g_AccountMng.SendBillingResult( &info );
}