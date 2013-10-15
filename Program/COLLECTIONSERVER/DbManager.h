// DbManager.h: interface for the CDbManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DBMANAGER_H__A67A82DF_C65A_4654_89D0_61B9A29CE7D2__INCLUDED_)
#define AFX_DBMANAGER_H__A67A82DF_C65A_4654_89D0_61B9A29CE7D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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
	LOG_CERTIFY_INFO
};

typedef	struct tagDB_OVERLAPPED_PLUS
{
	char	szCertifyIP[16];
	char	szClientVer[30];
	int		nQueryMode;
	int		nFlyffVer;
	int		nLang;
	char	szClientIP[16];
}	DB_OVERLAPPED_PLUS, *LPDB_OVERLAPPED_PLUS;

#define	DbManager	CDbManager::GetInstance()
class CDbManager
{
public:
	CDbManager();
	virtual ~CDbManager();

	static CDbManager* GetInstance();
	
public:
	CMemPool<DB_OVERLAPPED_PLUS>*	m_pDbIOData;
	char	m_szDSN_NAME_COLLECTION[260];
	char	m_szDB_ADMIN_ID_COLLECTION[260];
	char	m_szDB_ADMIN_PASS_COLLECTION[256];

	BOOL	CreateDbWorkers();
	HANDLE	GetIOCPHandle( int n );
	void	PostQ( LPDB_OVERLAPPED_PLUS pData );

	void	DBQryLogCertifyInfo( char* qryAccount, LPDB_OVERLAPPED_PLUS pData );
	void	LogCertifyInfo( CQuery & query, LPDB_OVERLAPPED_PLUS pData );

private:
	HANDLE	m_hDbWorkerThreadTerminate[DEFAULT_DB_WORKER_THREAD_NUM];
	HANDLE	m_hIOCP[DEFAULT_DB_WORKER_THREAD_NUM];
};

u_int	__stdcall	DbWorkerThread( LPVOID lpDbManager );	// DbWorkerThread

#endif // !defined(AFX_DBMANAGER_H__A67A82DF_C65A_4654_89D0_61B9A29CE7D2__INCLUDED_)
