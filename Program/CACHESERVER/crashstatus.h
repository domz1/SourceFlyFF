#ifndef __CRASH_STATUS
#define	__CRASH_STATUS

#ifdef __CRASH_0404

class CDPMng;
class CPlayerMng;

class CCrashStatus
{
public:
	CCrashStatus();
	virtual	~CCrashStatus();

	void	Clear( void );

	static	CCrashStatus*	GetInstance( void );
	void	RegisterDPMng( LPCSTR szClass, CDPMng* pdpMng );
	DWORD	GetMemoryLoad( void );	// GlobalMemoryStatus
	int		GetChildAllocated( void )	{	return m_nChildAllocated;	}	// CDPCacheSrvr's
	int		GetPlayerCount( CPlayerMng* pPlayerMng );
	DWORD	GetLastPacket( CDPMng* pdpMng );
	int		GetMainThreadTask( void )	{	return m_nTask;	}

	void	SetMainThreadTask( int nTask )	{	m_nTask		= nTask;	}
	void	SetLastPacket( CDPMng* pdpMng, DWORD dwHdr );
	void	AllocChild( void )	{	m_nChildAllocated++;	}
	void	FreeChild( void )	{	m_nChildAllocated--;	}

	void	PrintStatus( void );
private:
	int	m_nTask;
	int	m_nChildAllocated;
	map<CDPMng*, DWORD>	m_mapLastPacket;
	map<string, CDPMng*>	m_mapDPClass;
	CMclCritSec		m_AccessLock;
};

#endif	// __CRASH_0404

#endif	// __CRASH_STATUS