#ifndef __DBCONTROLLER_H__
#define	__DBCONTROLLER_H__


////////////////////////////////////////////////////////////////////////////////
class CDbControllerTimer;
class CDbController
{
public:
	enum	{	eTimer	= -1,	};
public:
	CDbController();
	virtual	~CDbController();

	BOOL	CreateDbHandler( DWORD dwMilliseconds = INFINITE );
	void	CloseDbHandler( void );
	BOOL	PostRequest( int nQuery, BYTE* lpBuf, int nBufSize, DWORD dwCompletionKey );
	void	SetQueryObject( CQuery* pQuery )	{	m_pQuery	= pQuery;	}
	CQuery*	CreateQuery( void );
	CQuery*	GetQueryObject( void )	{	return m_pQuery;	}

	static	u_long	_DbHandler( LPVOID* pParam );
	void	DbHandler( void );

	virtual	void	Handler( LPDB_OVERLAPPED_PLUS pov, DWORD dwCompletionKey )		= 0;
	virtual void	OnTimer()	{}
	DWORD	GetTimer()	{	return m_dwTimer;	}

private:
	void	SetTimer( DWORD dwMilliseconds );

private:
	HANDLE	m_hIocp;
	HANDLE	m_hDbHndlr;
	CQuery*		m_pQuery;
	DWORD	m_dwTimer;
};


////////////////////////////////////////////////////////////////////////////////
typedef pair<CDbController*, DWORD>	PDD;
typedef vector<PDD>	VPDD;

class CDbControllerTimer
{
public:
	virtual	~CDbControllerTimer();
	void	Register( CDbController* dbCtrl );
	void	Create( void );
	void	Destroy( void );
	static CDbControllerTimer*	GetInstance( void );
private:
	CDbControllerTimer();
	static	u_int	_Tick( LPVOID pParam );
	void	Tick( void );

private:
	VPDD	m_vDbController;
	CMclCritSec	m_csLock;
	HANDLE	m_hTickThread;
	HANDLE	m_hCloseTick;
};


////////////////////////////////////////////////////////////////////////////////
namespace	post	{
	void	Post( u_long idReceiver, CMail* pMail, CQuery* pQuery );
};


#endif	// __DBCONTROLLER_H__