#include "stdafx.h"
#include "Environment.h"

#ifdef __CORESERVER
#include "dpcoresrvr.h"
extern	CDPCoreSrvr g_dpCoreSrvr;
#endif // __CORESERVER

/*--------------------------------------------------------------------------------*/

CEnvironment::CEnvironment()
{
#ifdef __ENVIRONMENT_EFFECT

#ifdef __WORLDSERVER
	::memset( m_aEnvironmentEffect, 0, sizeof( m_aEnvironmentEffect ) );
#endif // __WORLDSERVER

#ifdef __CLIENT
	m_nSeason = SEASON_NONE;
#endif // __CLIENT

	m_bEffect = FALSE;
	m_tEffectTime.Set( MIN( 60 ), TRUE );

#else // __ENVIRONMENT_EFFECT

	m_bRain = m_bSnow = FALSE;
#ifdef __EVENTLUA_RAIN
	m_tRainTime.Set( MIN( 60 ) );
#else // __EVENTLUA_RAIN
#ifdef __RAIN_EVENT
	m_tRainTime.Set( MIN( 60 ) );
#else // __RAIN_EVENT
	m_tRainTime.Set( MIN( 10 ) );
#endif // __RAIN_EVENT
#endif // __EVENTLUA_RAIN
#ifdef __EVENTLUA_SNOW
	m_tSnowTime.Set( MIN( 60 ) );
#else // __EVENTLUA_SNOW
	m_tSnowTime.Set( MIN( 10 ) );
#endif // __EVENTLUA_SNOW

#endif // __ENVIRONMENT_EFFECT

#ifdef __CORESERVER
	m_Authority = FALSE;
	m_hWorker	= m_hCloseWorker	= NULL;
#endif // __CORESERVER
}

CEnvironment::~CEnvironment()
{
#ifdef __CORESERVER
	CloseWorkers();
#endif // __CORESERVER
}



#ifdef __ENVIRONMENT_EFFECT

CEnvironment* CEnvironment::GetInstance()
{
	static CEnvironment sEnvironment;
	return & sEnvironment;
}

#ifdef __WORLDSERVER
BOOL CEnvironment::LoadScript()
{
	CScript scanner;

	if( scanner.Load( "EnvironmentEffect.txt", FALSE ) == FALSE )
	{
		Error( "CEnvironment::LoadScript - Script Load Failed" );
		return FALSE;
	}

	::memset( m_aEnvironmentEffect, 0, sizeof( m_aEnvironmentEffect ) );

	int nLang = scanner.GetNumber();
	while( scanner.tok != FINISHED )
	{
		if( nLang < 0 || nLang >= LANG_MAX )
		{
			Error( "CEnvironment::LoadScript - Invalid Lang : %[d]", nLang );
			return FALSE;
		}

		for( int i = 0; i < 12; ++i )
		{
			int nSeason = scanner.GetNumber();
			if( nSeason < 0 || nSeason >= SEASON_MAX )
			{
				Error( "CEnvironment::LoadScript - Invalid Season : %[d]", nSeason );
				return FALSE;
			}
			m_aEnvironmentEffect[nLang][i] = nSeason;
		}
		nLang = scanner.GetNumber();
	}
	return TRUE;
}

int CEnvironment::GetSeason()
{
	int nLang = ::GetLanguage();
	//int nSubLang = ::GetSubLanguage();
	if( nLang < 0 || nLang >= LANG_MAX )
	{
		Error( "CEnvironment::GetSeason - Invalid argument. Lang : [%d]", nLang );
		return SEASON_NONE;
	}

	CTime timeCurr = CTime::GetCurrentTime();
	int nMonth = timeCurr.GetMonth();
	
	return m_aEnvironmentEffect[nLang][nMonth - 1];
}
#endif // __WORLDSERVER

#ifdef __CLIENT
void CEnvironment::SetSeason( int nSeason )
{
	if( nSeason < SEASON_NONE || nSeason >= SEASON_MAX )
	{
		m_nSeason = SEASON_NONE;
	}
	else
	{
		m_nSeason = nSeason;
	}
}
#endif // __CLIENT

#endif // __ENVIRONMENT_EFFECT

void CEnvironment::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
#ifdef __ENVIRONMENT_EFFECT
		ar << m_bEffect;
#else // __ENVIRONMENT_EFFECT
		ar << m_bRain;
		ar << m_bSnow;
#endif // __ENVIRONMENT_EFFECT
	}
	else
	{
#ifdef __ENVIRONMENT_EFFECT
		ar >> m_bEffect;
#else // __ENVIRONMENT_EFFECT
		ar >> m_bRain;
		ar >> m_bSnow;
#endif // __ENVIRONMENT_EFFECT
	}
}

#ifdef __CORESERVER
BOOL CEnvironment::CreateWorkers( void )
{
	DWORD dwThreadId;
	m_hCloseWorker	= CreateEvent( NULL, FALSE, FALSE, NULL );
	m_hWorker	= chBEGINTHREADEX( NULL, 0, _Worker, this, 0, &dwThreadId );
	if( m_hWorker == NULL )
		return FALSE;
	return TRUE;
}

void CEnvironment::CloseWorkers( void )
{
	CLOSE_THREAD( m_hWorker, m_hCloseWorker );
}

UINT CEnvironment::_Worker( LPVOID pParam )
{
	CEnvironment* pEnvironment	= (CEnvironment*)pParam;
	pEnvironment->Worker();
	return 0;
}

void CEnvironment::Worker( void )
{
	HANDLE hHandle	= m_hCloseWorker;
	while( WaitForSingleObject( hHandle, 60000 ) == WAIT_TIMEOUT )	// 10√ 	= 1000 * 10 //1000 * 60 * 5 = 60000
	{
		if( m_Authority )
		{
#ifdef __ENVIRONMENT_EFFECT
			if( m_bEffect == TRUE )
			{
				m_Authority = FALSE;
				m_tEffectTime.Reset();
				g_dpCoreSrvr.SendEnvironmentEffect();
			}
#else // __ENVIRONMENT_EFFECT
			if( m_bRain )
			{
#ifdef __EVENTLUA_RAIN
				m_Authority = FALSE;
				m_tRainTime.Reset();
#else // __EVENTLUA_RAIN
#ifdef __RAIN_EVENT
				m_Authority = FALSE;
				m_tRainTime.Reset();
#endif // __RAIN_EVENT
#endif // __EVENTLUA_RAIN
				g_dpCoreSrvr.SendEnvironmentRain( m_bRain );
			}
			if( m_bSnow )
			{
#ifdef __EVENTLUA_SNOW
				m_Authority = FALSE;
				m_tSnowTime.Reset();
#endif // __EVENTLUA_SNOW
				g_dpCoreSrvr.SendEnvironmentSnow( m_bSnow );
			}
#endif // __ENVIRONMENT_EFFECT
		}
		else
		{
#ifdef __ENVIRONMENT_EFFECT

			if( m_tEffectTime.IsTimeOut() == TRUE )
			{
				BOOL bOldEffect = m_bEffect;

				if( random( 10 ) < 5 )
				{
					m_bEffect = TRUE;
					m_tEffectTime.Reset();
				}
				else
				{
					m_bEffect = FALSE;
				}

				if( bOldEffect != m_bEffect )
				{
					g_dpCoreSrvr.SendEnvironmentEffect();
				}
			}

#else // __ENVIRONMENT_EFFECT

#ifdef __JAPAN_SAKURA
			CTime timeCurr	= CTime::GetCurrentTime();
			int nMonth = timeCurr.GetMonth();
			if( nMonth >= 6 && nMonth <= 8 ) // 6 ~ 8 - ∫Ò
			{
				if( m_tRainTime.IsTimeOut() )
				{
					BOOL bRainBuf = m_bRain;

					if( random(1440) < 5 )
					{
						m_tRainTime.Reset();
						m_bRain = TRUE;
					}
					else
					{
						m_bRain = FALSE;
					}

					if( bRainBuf != m_bRain )
					{
						g_dpCoreSrvr.SendEnvironmentRain( m_bRain );
					}
				}
			}
			else // 3 ~ 5 - ∫¢≤…, 9 ~ 11 - ¥‹«≥, 12,1,2 - ¥´
			{
				if( m_tSnowTime.IsTimeOut() )
				{
					BOOL bSnowBuf = m_bSnow;
					if( random(1440) < 5 )
					{
						m_tSnowTime.Reset();
						m_bSnow = TRUE;
					}
					else
					{
						m_bSnow = FALSE;
					}

					if( bSnowBuf != m_bSnow )
					{
						g_dpCoreSrvr.SendEnvironmentSnow( m_bSnow );
					}
				}
			}
#else __JAPAN_SAKURA
			CTime timeCurr	= CTime::GetCurrentTime();
//			CString strMessage;
			
			int nMonth = timeCurr.GetMonth();
			if( 1 == nMonth || 2 == nMonth || 12 == nMonth ) // ¥´≥ª∏Æ¥¬ ¥ﬁ 12ø˘ 1ø˘ 2ø˘  ∆ﬁ∆ﬁ~~ ¥´¿Ãø…¥œ¥Ÿ. «œ¥√ø°º≠ ¥´¿Ãø…¥œ¥Ÿ.
			{
				if( m_tSnowTime.IsTimeOut() )
				{
					BOOL bSnowBuf = m_bSnow;
					if( random(1440) < 5 ) // 1∫–ø° «—π¯æø ∞ÀªÁ 1440 = 60∫– * 24Ω√∞£
					{
						m_tSnowTime.Reset();
						m_bSnow = TRUE;
//						strMessage.Format( "%02dø˘%02d¿œ %02d:%02d:%02d\t∫Ò∞° ¡÷∏£∏§ ¡÷∏£∏§~~", 
//							timeCurr.GetMonth(), timeCurr.GetDay(), timeCurr.GetHour(), timeCurr.GetMinute(), timeCurr.GetSecond() );
					}
					else
					{
						m_bSnow = FALSE;
//						strMessage.Format( "%02dø˘%02d¿œ %02d:%02d:%02d\t∫Ò∞° ∏ÿ√„ªÛ≈¬", 
//							timeCurr.GetMonth(), timeCurr.GetDay(), timeCurr.GetHour(), timeCurr.GetMinute(), timeCurr.GetSecond() );
					}
					
					if( bSnowBuf != m_bSnow )
					{
						g_dpCoreSrvr.SendEnvironmentSnow( m_bSnow );
					}

/*					m_tSnowTime.Reset();
					m_bSnow = !m_bSnow;
					g_dpCoreSrvr.SendEnvironmentSnow( m_bSnow );
*/				}
			}
			else	// ∫Ò∞° ≥ª∏Æ≥◊~~ ∫ÒµŒ≥ª∏Æ¥¬µ• ∏∑∞…∏Æ «—¿‹~~ ^^;
			{
				if( m_tRainTime.IsTimeOut() )
				{
					BOOL bRainBuf = m_bRain;

					if( random(1440) < 5 ) // 1∫–ø° «—π¯æø ∞ÀªÁ 1440 = 60∫– * 24Ω√∞£
					{
						m_tRainTime.Reset();
						m_bRain = TRUE;
//						strMessage.Format( "%02dø˘%02d¿œ %02d:%02d:%02d\t¥´¿Ã ∆ﬁ∆ﬁ~~", 
//							timeCurr.GetMonth(), timeCurr.GetDay(), timeCurr.GetHour(), timeCurr.GetMinute(), timeCurr.GetSecond() );
					}
					else
					{
						m_bRain = FALSE;
//						strMessage.Format( "%02dø˘%02d¿œ %02d:%02d:%02d\t¥´¿Ã ∏ÿ√„ªÛ≈¬", 
//							timeCurr.GetMonth(), timeCurr.GetDay(), timeCurr.GetHour(), timeCurr.GetMinute(), timeCurr.GetSecond() );
					}
					
					if( bRainBuf != m_bRain )
					{
						g_dpCoreSrvr.SendEnvironmentRain( m_bRain );
					}
				}
			}
//			if( strMessage.GetLength() > 10 )
//				WriteLog( strMessage );
#endif // __JAPAN_SAKURA

#endif // __ENVIRONMENT_EFFECT
		}

		TRACE( "CEnvironment Worker\n" );
	}
}
#endif // __CORESERVER

#ifndef __ENVIRONMENT_EFFECT
CEnvironment	g_Environment;
#endif // __ENVIRONMENT_EFFECT

