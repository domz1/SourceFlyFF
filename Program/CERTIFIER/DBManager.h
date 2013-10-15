#ifndef __DBMANAGER_H__
#define	__DBMANAGER_H__

#include "query.h"
#include "ar.h"
#include "mempool.h"
#include <dplay.h>
#include <map>
#include <string>
#include <set>
using	namespace	std;

const DWORD			DEFAULT_DB_WORKER_THREAD_NUM = 8;

enum QUERYMODE
{	
	NEW_ACCOUNT, 
	CERTIFY, 
	CLOSE_EXISTING_CONNECTION,	
};

typedef	struct	tagACCOUNT_INFO
{
	char	szAccount[MAX_ACCOUNT];
	char	szPassword[MAX_PASSWORD];
#ifdef __TWN_LOGIN0816
	char	szSessionPwd[MAX_SESSION_PWD];
#endif	// __TWN_LOGIN0816
#ifdef __EUROPE_0514
	char	szBak[MAX_ACCOUNT];
#endif	// __EUROPE_0514
#if __VER >= 14 // __PCBANG
	DWORD	dwPCBangClass;
#endif // __PCBANG
}	ACCOUNT_INFO, *LPACCOUNT_INFO;

typedef	struct tagDB_OVERLAPPED_PLUS
{
	int				nQueryMode;
	ACCOUNT_INFO	AccountInfo;
	DWORD			dpId;
	DWORD			dwIP;		//connected ip
}	DB_OVERLAPPED_PLUS, *LPDB_OVERLAPPED_PLUS;

class CAccountMgr;

#ifdef __GPAUTH
typedef	struct	_GPAUTH_RESULT
{
	int		nResult;
	char	szResult[256];
	char	szGPotatoID[20];
	int		nGPotatoNo;
	char	szNickName[20];
	BOOL	bGM;
#ifdef __GPAUTH_02
	char	szCheck[255];
#endif	// __GPAUTH_02
	_GPAUTH_RESULT()
	{
		nResult	= -1;
		*szResult	= '\0';
		*szGPotatoID	= '\0';
		nGPotatoNo	= 0;
		*szNickName		= '\0';
		bGM	= 0;
		#ifdef __GPAUTH_02
		*szCheck	= '\0';
		#endif	// __GPAUTH_02
	}
}	GPAUTH_RESULT;
#endif	// __GPAUTH

#ifdef __JAPAN_AUTH
#define JAPAN_AUTH_RESULT_FAIL		0
#define JAPAN_AUTH_RESULT_AGREE		1
#define JAPAN_AUTH_RESULT_REJECT	2
typedef	struct	_JAPAN_AUTH_RESULT
{
	int		nResult;
	char	szResultMsg[256];
	_JAPAN_AUTH_RESULT()
	{
		nResult = JAPAN_AUTH_RESULT_FAIL;
		memset( szResultMsg, 0, sizeof( szResultMsg ) );
	}
}	JAPAN_AUTH_RESULT;
#endif // __JAPAN_AUTH

class CDbManager
{
protected:
	HANDLE	m_hDbWorkerThreadTerminate[DEFAULT_DB_WORKER_THREAD_NUM];
	HANDLE	m_hIOCP[DEFAULT_DB_WORKER_THREAD_NUM];

public:
	CMemPool<DB_OVERLAPPED_PLUS>*	m_pDbIOData;
	set<string>						m_eveSchoolAccount;
	char							m_szLoginPWD[256];

public:
	CDbManager();
	~CDbManager();

	BOOL	CreateDbWorkers();
	void	GetStrTime( CTime *time, char *strbuf );
	void	Certify( CQuery & query, LPDB_OVERLAPPED_PLUS pData, CAccountMgr& accountMgr );
	void	CloseExistingConnection( CQuery & qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
#ifdef __GPAUTH
	void	Certify2( CQuery & query, LPDB_OVERLAPPED_PLUS pov, CAccountMgr & mgr );
	void	CloseExistingConnection2( CQuery & query, LPDB_OVERLAPPED_PLUS pov );

	#ifdef __GPAUTH_03
	void	SQLAddAccount( CQuery & query, char* szAccount, char* szPassword, BOOL bGM );
	#else	// __GPAUTH_03
	void	SQLAddAccount( CQuery & query, char* szAccount, char* szPassword );
	#endif	// __GPAUTH_03

#endif	// __GPAUTH
#ifdef __JAPAN_AUTH
	void	Certify_Japan( CQuery & query, LPDB_OVERLAPPED_PLUS pov, CAccountMgr & mgr );
	void	CloseExistingConnection_Japan( CQuery & query, LPDB_OVERLAPPED_PLUS pov );
#endif // __JAPAN_AUTH
#if __VER >= 14 // __PCBANG
	void	DBQryAccount( char* qryAccount, LPDB_OVERLAPPED_PLUS pData );
#else // __PCBANG
#ifdef __TWN_LOGIN0816
	void	DBQryAccount( char* qryAccount, char* szAccount, char* szPass, char* szSessionPwd = "" );
#else	// __TWN_LOGIN0816
	void	DBQryAccount( char* qryAccount, char* szAccount, char* szPass );
#endif	// __TWN_LOGIN0816
#endif // __PCBANG
	BOOL	LoadEveSchoolAccount( void );
	BOOL	IsEveSchoolAccount( const char* pszAccount );
	BYTE	GetAccountFlag( int f18, LPCTSTR szAccount );
	void	OnCertifyQueryOK( CQuery & qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus, const char* szCheck = "" );
	HANDLE	GetIOCPHandle( int n );
	void	PostQ( LPDB_OVERLAPPED_PLUS pData );
};

u_int	__stdcall	DbWorkerThread( LPVOID lpDbManager );	// DbWorkerThread
#ifdef __GPAUTH
void	GetGPAuthResult( const char* szUrl, int nMode, int nGameMode, const char* sAccount, const char* sPassword, const char* sAddr, GPAUTH_RESULT & result );
u_int	__stdcall	GPotatoAuthWorker( LPVOID pParam );
#endif	// __GPAUTH
#ifdef __JAPAN_AUTH
u_int __stdcall		JapanAuthWorker( LPVOID pParam );
JAPAN_AUTH_RESULT	GetJapanAuthResult( LPDB_OVERLAPPED_PLUS pov );
#endif // __JAPAN_AUTH

#endif	// __DBMANAGER_H__