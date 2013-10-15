#ifndef __THREADMNG_H__
#define __THREADMNG_H__

class	CTimeout
{
protected:
	DWORD	m_curTime;
	DWORD	m_endTime;
	DWORD	m_dwInterval;
public:
	CTimeout( DWORD dwInterval, DWORD dwFirstTimeout );
	virtual	~CTimeout()	{}
	DWORD Over( DWORD dwCurr );		// if > 0, over
	DWORD Over( void );		// if > 0, over
	void	Reset( DWORD dwCurr, DWORD dwTimeout );
	BOOL TimeoutReset( void );
	BOOL TimeoutReset( DWORD dwCurr );
};

class CRunObject
{
private:
	HANDLE	m_hRunObject;
	HANDLE	m_hClose;
public:
//
	CRunObject();
	virtual	~CRunObject();
//	Op
	BOOL	Init( void );
	void	Close( void );
//
	static	u_int	_Run( LPVOID pParam );
	static	CRunObject*	GetInstance( void );
	void	Run( void );
	void	ToggleProfiling();
};

#endif	// __THREADMNG_H__