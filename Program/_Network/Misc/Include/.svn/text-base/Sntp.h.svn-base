/*
by PJ Naughter
www.naughter.com
*/
#ifndef __SNTP_H__
#define	__SNTP_H__

// Representation of an NTP timestamp
struct	CNtpTimePacket
{
	DWORD	dwInteger;
	DWORD	dwFractional;
};

// Helper class to encapsulate NTP timestamps
class CNtpTime
{
public:
//	Construtions
	CNtpTime();
	CNtpTime( const CNtpTime & time );
	CNtpTime( CNtpTimePacket & packet );
	CNtpTime( const SYSTEMTIME & st );
//	Operations
	CNtpTime &	operator=( const CNtpTime & time );
	double	operator-( const CNtpTime & time )	const;
	CNtpTime	operator+( const double & timespan )	const;
	operator SYSTEMTIME()	const;
	operator CNtpTimePacket()	const;
	operator unsigned __int64()	const	{	return m_Time;	}
	DWORD	Seconds()	const;
	DWORD	Fraction()	const;
	
static	CNtpTime	GetCurrentTime();
static	DWORD	MsToNtpFraction( WORD wMilliSeconds );
static	WORD	NtpFractionToMs( DWORD dwFraction );
static	double	NtpFractionToSecond( DWORD dwFraction );
protected:
// Internal static functions and data
static	long	GetJulianDay( WORD Year, WORD Month, WORD Day );
static	void	GetGregorianDate( long JD, WORD & Year, WORD & Month, WORD & Day );
static	DWORD	m_MsToNTP[1000];
// The actual data
	unsigned __int64	m_Time;
};

struct	NtpServerResponse
{
	int	nLeapIndicator;	// 0: no warning
										// 1: last minute in day has 61 seconds
										// 2: last minute has 59 seconds
										// 3: clock not synchronized
	int	nStratum;	// 0: unspecified or unavailable
							// 1: primary reference
							// 2-15: secondary reference( via NTP or SNTP )
							// 16-255: reserved
	CNtpTime	OriginateTime;	// Time when the request was sent from the client to the SNTP server
	CNtpTime	ReceiveTime;	// Time when the request was received by the server
	CNtpTime	TransmitTime;	// Time when the server sent the request back to the client
	CNtpTime	DestinationTime;	// Time when the reply was received by the client
	double	RoundTripDelay;	// Round trip time in seconds
	double	LocalClockOffset;	// Local clock offset relative to the server
};

#define	MAX_NTP_SERVER	9
// The actual SNTP class
class CSNTPClient
{
public:
//	Constructions
	CSNTPClient();
//	Operations
	BOOL	GetServerTime( LPCTSTR pszHostName, NtpServerResponse & response, int nPort = 123 );
	BOOL	GetServerTime( LPSYSTEMTIME lpSystemTime );
	DWORD	GetTimeout( void )	const	{ return	m_dwTimeout;	}
	void	SetTimeout( DWORD dwTimeout )	{	m_dwTimeout	= dwTimeout;	}
	BOOL	SetClientTime( const CNtpTime & newTime );
protected:
//	Ar 22-07-2000
#ifndef _WIN32_WCE
	BOOL	EnableSetTimePrivilege( void );
	void	RevertSetTimePrivilege( void );
#endif	// _WIN32_WCE
	DWORD	m_dwTimeout;
#ifndef _WIN32_WCE
	HANDLE	m_hToken;
	TOKEN_PRIVILEGES	m_TokenPriv;
	BOOL	m_bTakenPrivilege;
#endif	// _WIN32_WCE
public:
static char	m_pszHostNames[MAX_NTP_SERVER][MAX_PATH];
};

#endif	// __SNTP_H__