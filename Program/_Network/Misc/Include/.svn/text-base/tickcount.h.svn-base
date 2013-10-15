#ifndef __TICKCOUNT_H__
#define	__TICKCOUNT_H__

class CTickCount
{
private:
	__int64		m_nStart;
	DWORD	m_dwBase;
	DWORD	m_dwOffset;
public:
//	Constructions
	CTickCount();
	~CTickCount()	{}
//	Operations
	void	Start( const SYSTEMTIME & st );
	void	Start( const __int64 & nStart );
	void	Start( void );
	__int64		GetTickCount( void );
	int		GetOffset( const __int64 & nStart );
};

inline CTickCount::CTickCount()
{
	m_nStart	= 0;
	m_dwBase	= m_dwOffset	= 0;
}

inline void CTickCount::Start( const SYSTEMTIME & st )
{
	FILETIME ft;
	SystemTimeToFileTime( &st, &ft );
	memcpy( &m_nStart, &ft, sizeof(__int64) );
	m_dwBase	= timeGetTime();
}

inline void CTickCount::Start( const __int64 & nStart )
{
	m_nStart	= nStart;
	m_dwBase	= m_dwOffset	= timeGetTime();
}

inline void CTickCount::Start( void )
{
	FILETIME systemTimeAsFileTime;
	GetSystemTimeAsFileTime( &systemTimeAsFileTime );
	memcpy( &m_nStart, &systemTimeAsFileTime, sizeof(__int64) );
	m_dwBase	= timeGetTime();
}

inline __int64 CTickCount::GetTickCount( void )
{
	m_dwOffset	= timeGetTime();
	return( m_nStart + ( m_dwOffset - m_dwBase ) * 10000 );
}

inline int CTickCount::GetOffset( const __int64 & nStart )
{
	LARGE_INTEGER liDiff;
	liDiff.QuadPart		= GetTickCount() - nStart;
	return (int)liDiff.LowPart / 10000;
}

#endif	// __TICKCOUNT_H__