// AccountTimeLimitDBCtrl.cpp: implementation of the CAccountTimeLimitDBCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AccountTimeLimitDBCtrl.h"

#ifdef __VTN_TIMELIMIT
#include "dptrans.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAccountTimeLimitDBCtrl::CAccountTimeLimitDBCtrl()
{

}

CAccountTimeLimitDBCtrl::~CAccountTimeLimitDBCtrl()
{

}


void CAccountTimeLimitDBCtrl::Handler( LPDB_OVERLAPPED_PLUS pov, DWORD dwCompletionKey )
{
	CAr ar( pov->lpBuf, pov->uBufSize );

	switch( pov->nQueryMode )
	{
		case QUERY_TIMELIMIT_LOAD:
		{
			DWORD	dwPlayerId; 
			int		nPlayTime = -1;
			char	szAccount[MAX_ACCOUNT];

			ar >> dwPlayerId;
			ar.ReadString( szAccount, MAX_ACCOUNT );
			
			CQuery* pQuery = GetQueryObject();
			if( pQuery->Execute( "USP_AccountPlay_Select '%s'", szAccount ) )
			{
				if( pQuery->Fetch() )
				{
					nPlayTime = pQuery->GetInt( "PlayTime" ) * SEC( 1 );	// 초단위를 ms단위로 변환
					DWORD dwLastPlayDate	= static_cast<DWORD>( pQuery->GetInt( "PlayDate" ) );
					int nYear = dwLastPlayDate/10000;	dwLastPlayDate%=10000;
					int nMonth = dwLastPlayDate/100;	dwLastPlayDate%=100;
					int nDay = dwLastPlayDate;
					CTime time( nYear, nMonth, nDay+1, 0, 0, 0 );	// 하루를 더한 값이 더 작으면 초기화..
					if( time < CTime::GetCurrentTime() )
						nPlayTime = 0;
					
					if( !prj.m_EventLua.IsTimeLimit() )		// 루아이벤트 스크립트에 베트남 제한 여부 설정
						nPlayTime = 0;
				}
			}

			if( nPlayTime < 0 )
				Error( "PlayTime is wrong - PlayerId : %d, PlayTime : %d", dwPlayerId, nPlayTime );
			CDPTrans::GetInstance()->SendTimeLimitAck( dwPlayerId, nPlayTime, dwCompletionKey );
		}
		break;

		case QUERY_TIMELIMIT_UPDATE:
		{
			char	szAccount[MAX_ACCOUNT];
			int		nPlayTime;

			ar.ReadString( szAccount, MAX_ACCOUNT );
			ar >> nPlayTime;
			nPlayTime /= SEC( 1 );	// ms 단위를 초단위로 변환

			CTime time = CTime::GetCurrentTime();
			DWORD dwLastDate = (time.GetYear()*10000) + (time.GetMonth()*100) + time.GetDay();
			
			GetQueryObject()->Execute( "USP_AccountPlay_Update '%s', %d, %d", szAccount, dwLastDate, nPlayTime );
		}
		break;
	}
}

void CAccountTimeLimitDBCtrl::OnTimer()
{
	CTime time = CTime::GetCurrentTime();
	if( time.GetHour() == 0 && time.GetMinute() == 0 )
		CDPTrans::GetInstance()->SendTimeLimitReset();
}


////////////////////////////////////////////////////////////////////////////////////////////////
CAccountTimeLimitDBMng::CAccountTimeLimitDBMng()
{
	if( !m_AccTimeLimitDBDtrl.CreateDbHandler( MIN(1) ) )
		Error( "CHousingDBMng - m_HousingDBCtrl.CreateDbHandler()" );
}

CAccountTimeLimitDBMng::~CAccountTimeLimitDBMng()
{
	m_AccTimeLimitDBDtrl.CloseDbHandler();
}

CAccountTimeLimitDBMng* CAccountTimeLimitDBMng::GetInstance()
{
	static CAccountTimeLimitDBMng sAccTmLmtDBMng;
	return & sAccTmLmtDBMng;
}
#endif // __VTN_TIMELIMIT