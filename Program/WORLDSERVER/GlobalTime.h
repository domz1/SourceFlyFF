#ifndef __GLOBALTIME_H__
#define __GLOBALTIME_H__

class CStopwatch
{
private:
	DWORD			m_dwTime;
	LARGE_INTEGER	m_liPerformanceCount;

public:
	// Operations
	void	Play(  )
	{
		m_dwTime = timeGetTime();
	}

	void	PlayEx(  )
	{
		QueryPerformanceCounter( &m_liPerformanceCount );
	}

	DWORD	Stop(  )
	{
		return( timeGetTime() - m_dwTime );
	}

	float	StopEx(  )
	{
		LARGE_INTEGER li;
		QueryPerformanceCounter( &li );
		return	( (float)( li.QuadPart - m_liPerformanceCount.QuadPart ) );
	}
};



#endif	// __GLOBALTIME_H__
