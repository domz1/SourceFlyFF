#ifndef __ACCOUNT_H__
#define	__ACCOUNT_H__

#include "mempooler.h"
#include <map>
#include <string>
using	namespace	std;

#define	MAX_CERTIFIER			32
const int LEFTIME_NOTIFIED		= 1;

//빌링의 결과값 
enum BILLING_RETURN
{
	SUCCESS				= 0,
	BILLING_INFO_FAILED = 1,
	DATABASE_ERROR		= 2,
	OTHER_ERROR			= 3,
	TIME_OVER			= 11
};

struct BILLING_INFO
{
	const char*		szAccount;
	DWORD			dwKey;
	long			lResult;				// 빌링의 결과값 
	const CTime*	pTimeOverDays; 
	char			cbLastOneLogon;			// 대만에서 사용하는 플래그 'Y'일 경우만 30분통지를 한다. 

	BILLING_INFO() : 
		szAccount( NULL ), 
		dwKey( 0 ),
		lResult( OTHER_ERROR ),
		cbLastOneLogon( 'N' )		
	{
	}
};

enum ACCOUNT_STATUS
{
	ACCOUNT_STATUS_INITIAL,			// 초기상태 
	ACCOUNT_STATUS_NOTIFIED,		// 남은 시간을 알려준 상태 
	ACCOUNT_STATUS_SECONDQUERY,		// 2번째 쿼리결과를 기다리는 상태   
};

class CAccount
{
public:
//	Constructions
	CAccount();
	CAccount( LPCTSTR lpszAccount, DWORD dpid1, DWORD dpid2, BYTE b18, int fCheck );
	virtual	~CAccount();

	BOOL	IsAdult();
	void	SetTimeOverDays(  const CTime* pOverDays );
	void	SendBillingResult( BILLING_INFO* pResult );

//	Attributes
public:
	DWORD			m_dpid1;
	DWORD			m_dpid2;
	DWORD			m_dwIdofServer;
	BYTE			m_cbAccountFlag;
	DWORD			m_dwBillingClass;
	CTime			m_TimeOverDays;
	TCHAR			m_lpszAccount[MAX_ACCOUNT];
	BOOL			m_fRoute;
	DWORD			m_dwPing;
	DWORD			m_dwAuthKey;
	int				m_cbRef;
	int				m_fCheck;
	ACCOUNT_STATUS	m_nStatus;				// ACCOUNT_STATUS_INITIAL, ...
	char			m_cbLastOneLogon;		// 대만에서 사용하는 플래그 'Y'일 경우만 30분통지를 한다. 
	u_long			m_uIdofMulti;

public:
#ifndef __VM_0820
#ifndef __MEM_TRACE
	static MemPooler<CAccount>*	m_pPool;
	void*	operator new( size_t nSize )	{	return CAccount::m_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CAccount::m_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CAccount::m_pPool->Free( (CAccount*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CAccount::m_pPool->Free( (CAccount*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820
};

class CAccountMng
{
private:
	map<string, CAccount*>	m_stringToPAccount;
	map<DWORD, int>			m_dpidToIndex;
	map<DWORD, CAccount*>	m_adpidToPAccount[MAX_CERTIFIER];
	int		m_nSizeof;
	int		m_nOldHour;
#ifndef __EUROPE_0514
	DWORD	m_dwSerial;
#endif	// __EUROPE_0514
public:
	int						m_nCount;
	map<DWORD, DWORD>		m_2IdofServer;

	int		m_nYear;
	int		m_nMonth;
	int		m_nDay;
	int		m_nHour;
	int		m_nMinute;
public:
//	Constructions
	CAccountMng();
	virtual	~CAccountMng();
public:
//	Attributes
	int			GetIndex( DWORD dpid1 );
	int			GetIdofServer( DWORD dpid );
	CAccount*	GetAccount( LPCTSTR lpszAccount );
	CAccount*	GetAccount( DWORD dpid1, DWORD dpid2 );
	CAccount*	GetAccount( LPCTSTR lpszAccount, DWORD dwSerial );
#ifdef __LOG_PLAYERCOUNT_CHANNEL
	map<string, CAccount*> GetMapAccount();
#endif // __LOG_PLAYERCOUNT_CHANNEL

//	Operations
	void	Clear( void );
	void	AddConnection( DWORD dpid );
	void	RemoveConnection( DWORD dpid );
	void	AddIdofServer( DWORD dpid, DWORD dwIdofServer );
#ifdef __SERVERLIST0911
	DWORD	RemoveIdofServer( DWORD dpid, BOOL bRemoveConnection = TRUE );
#else	// __SERVERLIST0911
	void	RemoveIdofServer( DWORD dpid, BOOL bRemoveConnection = TRUE );
#endif	// __SERVERLIST0911

	BYTE	AddAccount( LPCTSTR lpszAccount, DWORD dpid1, DWORD dpid2, DWORD* pdwAuthKey, BYTE b18, int fCheck );
	void	RemoveAccount( LPCTSTR lpszAccount );	
	void	RemoveAccount( DWORD dpid1, DWORD dpid2 );
	void	PreventExcess();
	void	KickOutCheck();
	BOOL	IsTimeCheckAddr();
	void	SendBillingResult( BILLING_INFO* pResult );

public:
	CMclCritSec		m_AddRemoveLock;

#if __VER >= 14 // __PCBANG
	void	PushPCBangPlayer( DWORD dwAuthKey, DWORD dwClass )	{ m_mapPCBang.insert( map<DWORD, DWORD>::value_type( dwAuthKey, dwClass ) ); }
	DWORD	PopPCBangPlayer( DWORD dwAuthKey );

private:
	map<DWORD, DWORD>	m_mapPCBang;
#endif // __PCBANG
};

#endif	// __ACCOUNT_H__