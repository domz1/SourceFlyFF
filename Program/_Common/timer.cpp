#include "stdafx.h"
#include "timer.h"
#include <mmsystem.h>



///////////////////////////////////////////////////////////////////////////////////////
CTimer::CTimer()
{
	Set( 0.0f );
}

CTimer::CTimer( float fInterval )
{
	Set( fInterval );
}

CTimer::CTimer( float fInterval, BOOL bFirstTimeOut)
{
	Set( fInterval, bFirstTimeOut );
}

double CTimer::GetTime()
{
	//LARGE_INTEGER largeTime = { 0, 0 };
	//QueryPerformanceCounter(&largeTime);
#ifdef __GLOBAL_COUNT_0705
	return (double)g_tmCurrent; //((double) largeTime.LowPart + (double) largeTime.HighPart * (double) 0xffffffffffffffff);
#else // __GLOBAL_COUNT_0705 
	return (double)timeGetTime(); //((double) largeTime.LowPart + (double) largeTime.HighPart * (double) 0xffffffffffffffff);
#endif // __GLOBAL_COUNT_0705
}

void CTimer::Set( float fInterval, BOOL bFirstTimeOut )
{
	m_fInterval		= fInterval;
	m_curTime	= GetTime();
	m_endTime	= bFirstTimeOut ? m_curTime : m_curTime + fInterval;
}

void CTimer::Reset( void )
{
	Set( m_fInterval );
}

BOOL CTimer::Over( void )
{
	double dwCurr	= GetTime();

	if( dwCurr >= m_endTime )
		return TRUE;

	if( m_endTime > 3758096381 && dwCurr < 536870911 )
		return TRUE;

	return FALSE;
}

double CTimer::GetLeftTime( void )
{
	return ( GetTime() - m_curTime );
}

float CTimer::GetInterval( void )
{
	return m_fInterval;
}

void CTimer::operator()( void )
{
	while( !Over() );
}

void CTimer::operator()( double fInterval )
{
	Set( (float)fInterval );	while( !Over() );
}

#define HOUR_SUNRISE     6 
#define HOUR_SUNSET      18
#define HOUR_MOONRISE    18
#define HOUR_MOONSET     6 
#define SETRISE_VARIABLE 2

// 해가 뜨는 상황을 퍼센트로. 100에 가까우면 만땅, 0에 가까우면 나타나는 중임

int CGameTimer::GetSunPercent() 
{
	int nMin, nHour, nResult = 0;
	if( m_nHour >= HOUR_SUNRISE && m_nHour < HOUR_SUNSET )
	{
		nHour = m_nHour - HOUR_SUNRISE;
		nMin = nHour * 60 + m_nMin;	// 60 * 2 : nMin = 100 : x
		nResult = nMin * 100 / ( 60 * SETRISE_VARIABLE );
	}
	else
	if( m_nHour >= HOUR_SUNSET )
	{
		nHour = m_nHour - HOUR_SUNSET; 
		nMin =  nHour * 60 + m_nMin;	// 60 * 2 : nMin = 100 : x
		nResult = 100 - ( nMin * 100 / ( 60 * SETRISE_VARIABLE ) );
	}
	if( nResult < 0 ) nResult = 0;
	if( nResult > 100 ) nResult = 100;
//	if( nResult > 0 && nResult < 100 )
//	;//TRACE( "sun %d \n", nResult );
	return nResult;
}

// 달 혹은 별이 뜨는 상황을 퍼센트로. 100에 가까우면 만땅, 0에 가까우면 나타나는 중임

int CGameTimer::GetMoonPercent() 
{
	int nResult = 100 - GetSunPercent();
	/*
	int nMin, nHour, nResult = 100;
	if( m_nHour >= HOUR_MOONSET && m_nHour < HOUR_MOONRISE - SETRISE_VARIABLE ) // / 18 < 20
	{
		nHour = m_nHour - HOUR_MOONSET;
		nMin = nHour * 60 + m_nMin; // 60 * 2 : nMin = 100 : x
		nResult = 100 - ( nMin * 100 / ( 60 * SETRISE_VARIABLE ) );
	}
	else
	if( m_nHour >= HOUR_MOONRISE  )
	{
		nHour = m_nHour - HOUR_MOONRISE; 
		nMin = nHour * 60 + m_nMin; // 60 * 2 : nMin = 100 : x
		nResult = ( nMin * 100 / ( 60 * SETRISE_VARIABLE ) );
	}
	if( nResult < 0 ) nResult = 0;
	if( nResult > 100 ) nResult = 100;
	*/
//	if( nResult > 0 && nResult < 100 )
//	;//	TRACE( "Moon %d \n", nResult );
	return nResult;
}
//1 = 1000배
//10 = 100배
//100 = 10배
//1000 = 1배
#define TIMESPEED 10
CGameTimer::CGameTimer()
{
	m_nSec = 0;
	m_nMin = 0;
	m_nHour = 0;
	m_nDay = 0;
	m_dBeginTime = 0;
	m_dCurrentTime = 0;
	SetCurrentTime( 3, 15 );
	m_bFixed = FALSE;
}
CGameTimer::CGameTimer( double dCurrentime )
{
	m_nSec = 0;
	m_nMin = 0;
	m_nHour = 0;
	m_nDay = 0;
	m_dBeginTime = 0;
	m_dCurrentTime = 0;
	SetCurrentTime( dCurrentime );
	m_bFixed = FALSE;
}
CGameTimer::~CGameTimer()
{
}
void CGameTimer::SetCurrentTime( double dCurrentime ) 
{ 
	m_dCurrentTime = dCurrentime; 
    m_dBeginTime = (double)timeGetTime();
}
double CGameTimer::GetCurrentTime() 
{ 
	double dTime = (double)timeGetTime();
	return dTime - m_dBeginTime + m_dCurrentTime;
}
void CGameTimer::SetCurrentTime( int nDay, int nHour )
{
	int nSec = ( ( nDay * 24 ) + nHour ) * 60 * 60;
	SetCurrentTime( nSec * TIMESPEED );
}
void CGameTimer::Compute()
{
	if( m_bFixed )
		return;
	long lCurTime = (long)( GetCurrentTime() );
	int nSec, nMin, nHour, nDay;
	lCurTime /= TIMESPEED;

	nSec    = lCurTime / 60;
	m_nSec  = lCurTime % 60;
	nMin    = nSec  / 60; // 60초로 나눈다.
	m_nMin  = nSec  % 60; // 60초로 나눈다.
	nHour   = nMin  / 24; // 24시간으로 나눈다.
	m_nHour = nMin  % 24; // 24시간으로 나눈다.
	nDay    = nHour / 30; // 30일로 나눈다.
	m_nDay  = nHour % 30; // 30일로나눈다.
	
	m_nHour++; // 1 based
	m_nDay++; // 1 based
	//m_nHour = 19;
	
#ifdef __XUZHU
//#ifdef _DEBUG
//	m_nHour = 7;		// 언제나 낮이 되도록
//#endif
#endif	
}
void CGameTimer::SetTime( int nDay, int nHour, int nMin, int nSec )
{
	m_nDay = nDay;
	m_nHour = nHour;
	m_nMin = nMin;
	m_nSec = nSec;
}
///////////////////////////////////
void CSkipTimer::Set( float fInterval )
{
	m_curTime	= GetTime();
	m_fInterval		= fInterval;
	m_bMaxSkip	= 0;
}
BOOL CSkipTimer::Over( void )
{
	if( m_fInterval == 0.0f )
		return FALSE;

	DWORD dwCurr = (DWORD)GetTime();

	if( dwCurr > ( m_curTime + m_fInterval ) )	
	{
		m_curTime	+= m_fInterval;
		return TRUE;
	}

	if( dwCurr < 536870911 && ( m_curTime + m_fInterval ) > 3758096381 )
	{
		m_curTime += m_fInterval;
		return TRUE;
	}

	return FALSE;
}

BOOL CSkipTimer::IsNeedSkip( void )
{
	if (GetTime() > m_curTime+m_fInterval)
		return TRUE;
	return FALSE;
}

void CSkipTimer::Skip( void )
{
	m_curTime	+= m_fInterval;
}

BOOL CSkipTimer::IsMaxSkip( void )
{
	double time = GetTime();

	time	= ( time - m_curTime ) / m_fInterval;
	
	if( m_bMaxSkip == 0 )
	{
		m_bMaxSkip = (int)( time / SKIPPING_PARAM );
	}
	else
	{
		m_bMaxSkip --;
	}

	if( m_bMaxSkip == 0 )
		return TRUE;

	return FALSE;
}

////////////////////////////////////////////////////////////////////

//IMPLEMENT_SERIAL(CDate,CObject,VERSIONABLE_SCHEMA|0)
/*
void CDate::Serialize(CArchive& ar)
{
	if(ar.IsStoring())
		ar << m_nYear << m_nMonth << m_nDay << m_nHour << m_nMinute << m_nSecond << m_nDelay;
	else
		ar >> m_nYear >> m_nMonth >> m_nDay >> m_nHour >> m_nMinute >> m_nSecond >> m_nDelay;
}
*/
CDate::CDate()
{
	m_nYear   = 1;
	m_nMonth  = 1;
	m_nDay    = 1;
	m_nHour   = 1;
	m_nMinute = 1;
	m_nSecond = 1;
	m_nDelay  = 1;
}
CDate::CDate(int nYear,int nMonth,int nDay,int nHour,int nMinute,int nSecond,int nDelay)
{
	m_nYear   = nYear  ;
	m_nMonth  = nMonth ;
	m_nDay    = nDay   ;
	m_nHour   = nHour  ;
	m_nMinute = nMinute;
	m_nSecond = nSecond;
	m_nDelay  = nDelay ;
	m_delay.Set((float)m_nDelay);
}
CDate::~CDate() 
{
}
BOOL CDate::IsNight()
{
	if(m_nHour < 6 || m_nHour >= 19)
		return TRUE;
	return FALSE;
}

// 현재 날짜를 1일로 환산하여 리턴
int		CDate :: GetDay( void )
{
	int		nDay = (m_nYear * 365) + (m_nMonth * 30) + m_nDay;

	return	nDay;
}
int CDate::ComputeShade(int nMax)
{
//	int nVal;
	int nResult = 0;
	if(m_nHour >= 21 || m_nHour <= 4)
		return nMax-1;
	if(m_nHour >= 19) // 19, 20
	{
		nResult = (((m_nHour - 19) * 60) + m_nMinute) / 16;
	}
	if(m_nHour <= 6) // 5, 6
	{
		nResult = (((6 - m_nHour) * 60) + (60-m_nMinute)) / 16;
	}
	if(nResult == nMax) 
		nResult--;
	return nResult;
}
void CDate::GetTimeColor(LPTIMECOLOR pTimeColor)
{
	int nTime;
	// 밤이 되고 있음 
	if(m_nHour >= 18)
	{
		nTime = (m_nHour - 18) * 60 + m_nMinute; // 분단위를 구함 (맥시섬 360 + 59 = 419)
		// 419 : 0x0f = nTime = x
		pTimeColor->m_nDarkAlpha = nTime * 0x0f / 359;
		if(pTimeColor->m_nDarkAlpha > 0x0f) 
			pTimeColor->m_nDarkAlpha = 0x0f;
		pTimeColor->m_nR = -3;
		pTimeColor->m_nG = -3;
		pTimeColor->m_nB = 0;
	}
	else
	// 아침이 되고 있음 
	if(m_nHour <= 5)
	{
		nTime = (8 - m_nHour) * 60 - m_nMinute; // 420 - 1 = 419) (밤이 끝나는 시각과 일치)
		// 419 : 0x0f = nTime = x
		pTimeColor->m_nDarkAlpha = nTime * 0x0f / 359;
		if(pTimeColor->m_nDarkAlpha > 0x0f) 
			pTimeColor->m_nDarkAlpha = 0x0f;
		pTimeColor->m_nR = 0;
		pTimeColor->m_nG = 0;
		pTimeColor->m_nB = 0;
	}
	else
	{
		pTimeColor->m_nDarkAlpha = 0;
		pTimeColor->m_nR = 0;
		pTimeColor->m_nG = 0;
		pTimeColor->m_nB = 0;
	}
}
void CDate::Process()
{
	while(m_delay.Over())
	{
		//m_delay.Set(m_nDelay);//m_nDelay);
		m_nSecond++;
		if(m_nSecond >= 60)
		{
			m_nSecond = 1;
			m_nMinute++;
		}
		if(m_nMinute >= 60)
		{
			m_nMinute = 1;
			m_nHour++;
		}
		if(m_nHour > 24)
		{
			m_nHour = 1;
			m_nDay++;
		}
		if(m_nDay > 30)
		{
			m_nDay = 1;
			m_nMonth++;
		}
		if(m_nMonth > 12)
		{ 
			m_nMonth = 1;
			m_nYear++;
		}
	}
}
#if 0 
//////////////////////////////////
static	HANDLE	g_hTimerThread;
int		g_bHacking = 0;
DWORD WINAPI	TimerThread( void *pvoid );

void	InitTimerThread( void )
{
	DWORD	ID;

	g_hTimerThread = CreateThread(NULL, 0, TimerThread, NULL, 0, &ID);
	g_bHacking = 0;
}

void	DestroyTimerThread( void )
{
	TerminateThread( g_hTimerThread, 0 );
	while ( !CloseHandle( g_hTimerThread ) );
}

DWORD WINAPI	TimerThread( void *pvoid )
{
	DWORD	timeStart, timeEnd;
	int		nCheck = 0;
	int		nCheckSum = 0;

	while (1)	
	{
		timeStart = timeGetTime();
		Sleep(5000);
		timeEnd = timeGetTime();
		if( (timeEnd - timeStart) > 5625 )
			g_bHacking = timeEnd - timeStart;
		nCheck = 0;
		nCheckSum = 0;
		Sleep(15000);			// 5초에 한번씩 검사
	}

	ExitThread(0);
	return 0;
}
#endif // if 0


///////////////////////////////////////////////
DWORD	m_lTime, m_dwOldTime;

#ifdef __BS_EFFECT_LUA
DWORD g_timeMTE = 0;
#endif;


static BOOL	s_bFrameSkip = TRUE;

void InitFST( void )
{
	m_lTime = 0;
	m_dwOldTime = timeGetTime();
	s_bFrameSkip = TRUE;
}

// 프레임 스키핑을 켜고/끈다
void	SetFrameSkip( BOOL bFlag )
{
	s_bFrameSkip = bFlag;
	if( bFlag == TRUE )		InitFST();
}

// 현재 플래그 돌려줌
BOOL	GetFrameSkip( void )
{
	return s_bFrameSkip;
}

static void UpdateTime( void )
{
	DWORD	dwTime = timeGetTime();

	m_lTime += (dwTime - m_dwOldTime);	// 경과된 시간을 더함
	if( m_lTime > 1000 )	m_lTime = 1000;

#ifdef __BS_EFFECT_LUA
	g_timeMTE += ( dwTime - m_dwOldTime );
	if( g_timeMTE > 20000 )	//20초마다 갱신
		g_timeMTE = 0;
#endif //__BS_EFFECT_LUA
	m_dwOldTime = dwTime;
}

BOOL	IsDrawTiming( void )
{
	if( s_bFrameSkip == FALSE )		return TRUE;		// 프레임 스킵이 꺼져있으면 무조건 드로우.
	UpdateTime();
	if( m_lTime >= 1000 )	
	{
		m_lTime = 0;
		return TRUE;
	}
	return (m_lTime < (1000 / FRAME_PER_SEC)) ? TRUE : FALSE;
}

void	SetFST( void )
{
	if( s_bFrameSkip == FALSE )		return;
	UpdateTime();
	while (m_lTime < (1000 / FRAME_PER_SEC))
		UpdateTime();

	m_lTime -= (1000 / FRAME_PER_SEC);
}










