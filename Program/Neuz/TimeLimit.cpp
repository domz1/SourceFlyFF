#include "stdafx.h"
#include "TimeLimit.h"

static int g_nLimit = INT_MAX;		// 제한 횟수 
static DWORD g_dwPeriod  = 5000;		// 제한 기간 

CTimeLimit::CTimeLimit()
{
	m_dwTick    = 0;
	m_nRequest	= 0;
}

CTimeLimit::CTimeLimit( int nLimit, DWORD dwPeriod )
{
	m_dwTick	= 0;
	m_nRequest	= 0;
	SetLimitation( nLimit, dwPeriod );
}

// 시도가 시간 제한을 넘는지 검사  ( TRUE - 제한을 넘었다 )
BOOL CTimeLimit::Check()
{
	m_nRequest++;
	if( GetTickCount() > m_dwTick )
	{
		m_dwTick = GetTickCount() + g_dwPeriod;
		m_nRequest = 1;
	}

	return ( m_nRequest > g_nLimit );
}

void CTimeLimit::SetLimitation( int nLimit, DWORD dwPeriod )
{
	g_nLimit   = nLimit;
	g_dwPeriod = dwPeriod;
}
