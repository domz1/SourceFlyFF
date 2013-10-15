#include "stdafx.h"
#include ".\rainbowracedbctrl.h"

#if __VER >= 13 // __RAINBOW_RACE
#include "dptrans.h"
extern AppInfo g_appInfo;

/////////////////////////////////////////////////////////////////////////
/// CRainbowRaceDBCtrl //////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
CRainbowRaceDBCtrl::CRainbowRaceDBCtrl(void)
: m_nTimes( 0 )
{
}
CRainbowRaceDBCtrl::~CRainbowRaceDBCtrl(void)
{
}

void CRainbowRaceDBCtrl::Handler( LPDB_OVERLAPPED_PLUS pov, DWORD dwCompletionKey )
{
	switch( pov->nQueryMode )
	{
		case QUERY_RR_LOAD :
			{
				LoadRainbowRace( dwCompletionKey );
				break;
			}

		case QUERY_RR_APPLICATION : 
			{
				CAr ar( pov->lpBuf, pov->uBufSize );
				DWORD dwPlayerId;
				ar >> dwPlayerId;
				Application( dwPlayerId );
				break;
			}

		case QUERY_RR_FAILEDUSER : 
			{
				CAr ar( pov->lpBuf, pov->uBufSize );
				DWORD dwPlayerId;
				ar >> dwPlayerId;
				FailedUser( dwPlayerId );
				break;
			}

		case QUERY_RR_RANKING :
			{
				CAr ar( pov->lpBuf, pov->uBufSize );
				DWORD dwPlayerId;
				int nRanking;
				ar >> dwPlayerId >> nRanking;
				if( dwPlayerId != NULL_ID )
					UpdateRanking( dwPlayerId, nRanking );
				else
				{
					UpdateWinnersLosers();
					m_nTimes++;
				}
				break;
			}
	}
}

void CRainbowRaceDBCtrl::LoadRainbowRace( DPID dpId )
{
	CQuery* pQuery = GetQueryObject();
	char szQuery[QUERY_SIZE] = {0,};

	// m_nTimes 정보 가져오기.. 없으면 0
	sprintf( szQuery, "RAINBOWRACE_STR 'S1', '%02d', 0, '0000000', 0", g_appInfo.dwSys );
	if( pQuery->Exec( szQuery ) == FALSE )
	{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return; }

	if( pQuery->Fetch() )
		m_nTimes = pQuery->GetInt( "nTimes" );
	if( m_nTimes == 0 )
	{
		m_nTimes = 1;
		return;
	}

	// 신청자 목록
	vector<DWORD> vec_dwNowPlayerId;
	sprintf( szQuery, "RAINBOWRACE_STR 'S2', '%02d', %d, '0000000', 0", g_appInfo.dwSys, m_nTimes );
	if( pQuery->Exec( szQuery ) == FALSE )
	{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return; }

	while( pQuery->Fetch() )
		vec_dwNowPlayerId.push_back( static_cast<DWORD>( pQuery->GetInt( "m_idPlayer" ) ) );
	
	if( vec_dwNowPlayerId.size() == 0 )		// 현재 신청자가 없으면 다음 회 진행...
		m_nTimes++;

	// 이전 대회 랭킹
	vector<DWORD> vec_dwPrevRanking;
	if( m_nTimes > 1 )	// 현재가 2회 이상일때만..( 1회 이상 진행했을 경우 )
	{
		sprintf( szQuery, "RAINBOWRACE_STR 'S3', '%02d', %d, '0000000', 0", g_appInfo.dwSys, m_nTimes );
		if( pQuery->Exec( szQuery ) == FALSE )
		{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return; }

		while( pQuery->Fetch() )
			vec_dwPrevRanking.push_back( static_cast<DWORD>( pQuery->GetInt( "m_idPlayer" ) ) );
	}

	CDPTrans::GetInstance()->SendRainbowRaceInfo( vec_dwNowPlayerId, vec_dwPrevRanking, dpId );
}

void CRainbowRaceDBCtrl::Application( DWORD dwPlayerId )
{
	CQuery* pQuery = GetQueryObject();
	char szQuery[QUERY_SIZE] = {0,};

	// 등록한 신청자를 DB에 저장한다.
	sprintf( szQuery, "RAINBOWRACE_STR 'I1', '%02d', %d, '%07d', 0", g_appInfo.dwSys, m_nTimes, dwPlayerId );
	if( pQuery->Exec( szQuery ) == FALSE )
	{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return; }
}

void CRainbowRaceDBCtrl::FailedUser( DWORD dwPlayerId )
{
	CQuery* pQuery = GetQueryObject();
	char szQuery[QUERY_SIZE] = {0,};

	// 레인보우레이스 시작시 접속안한 유저 실패처리
	sprintf( szQuery, "RAINBOWRACE_STR 'U3', '%02d', %d, '%07d', 0", g_appInfo.dwSys, m_nTimes, dwPlayerId );
	if( pQuery->Exec( szQuery ) == FALSE )
	{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return; }
}


void CRainbowRaceDBCtrl::UpdateRanking( DWORD dwPlayerId, int nRanking )
{
	CQuery* pQuery = GetQueryObject();
	char szQuery[QUERY_SIZE] = {0,};

	// 완주한 유저를 랭킹 예약한다.
	sprintf( szQuery, "RAINBOWRACE_STR 'U1', '%02d', %d, '%07d', %d", g_appInfo.dwSys, m_nTimes, dwPlayerId, nRanking );
	if( pQuery->Exec( szQuery ) == FALSE )
	{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return; }
}

void CRainbowRaceDBCtrl::UpdateWinnersLosers()
{
	CQuery* pQuery = GetQueryObject();
	char szQuery[QUERY_SIZE] = {0,};

	// 랭킹예약자와 완주실패자를 저장한다.
	sprintf( szQuery, "RAINBOWRACE_STR 'U2', '%02d', %d, '0000000', 0", g_appInfo.dwSys, m_nTimes );
	if( pQuery->Exec( szQuery ) == FALSE )
	{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return; }
}


/////////////////////////////////////////////////////////////////////////
/// CRainbowRaceDBMng ///////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
CRainbowRaceDBMng::CRainbowRaceDBMng(void)
{
	if( !m_RRDbCtrl.CreateDbHandler() )
		Error( "CRainbowRaceDBMng - m_RRDbCtrl.CreateDbHandler()" );
}

CRainbowRaceDBMng::~CRainbowRaceDBMng(void)
{
	m_RRDbCtrl.CloseDbHandler();
}

CRainbowRaceDBMng* CRainbowRaceDBMng::GetInstance()
{
	static CRainbowRaceDBMng sRainbowRaceDBMng;
	return & sRainbowRaceDBMng;
}

#endif // __RAINBOW_RACE