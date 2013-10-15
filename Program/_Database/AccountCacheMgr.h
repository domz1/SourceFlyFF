#ifndef _ACCOUNTCACHEMANAGER_H_
#define _ACCOUNTCACHEMANAGER_H_

struct ACCOUNT_CACHE;
class CAccountCacheMgr
{
private:
	map<string, ACCOUNT_CACHE*>	m_2Account;
//	map<u_long, string>			m_2AccountPtr;
	MemPooler<ACCOUNT_CACHE>*	m_pAccountMemPooler;
	list< ACCOUNT_CACHE* >		m_list;
	int							m_nAlloc;
public:
	CAccountCacheMgr();
	ACCOUNT_CACHE*	Find( LPCTSTR szAccount );
	void			Clear();
	void			AddMover( u_long idPlayer, LPCTSTR szAccount, int nSlot );
	void			RemoveMover( u_long idPlayer, LPCTSTR szAccount );
	void			AddAccount( u_long idPlayer, LPCTSTR szAccount, BOOL bCache, ACCOUNT_CACHE *pCache );
//	ACCOUNT_CACHE*	_Find( u_long idPlayer );
	ACCOUNT_CACHE*	GetAccount( LPCTSTR szAccount, BOOL *pbCacheHit );
#ifdef __INVALID_LOGIN_0320
	void	ChangeMultiServer( LPCTSTR szAccount, u_long uMultiServer );
	int		InitMultiServer( u_long uMultiServer );
#endif	// __INVALID_LOGIN_0320
};


#endif // _ACCOUNTCACHEMANAGER_H_