#include "stdafx.h"
#include "..\_network\objects\Obj.h"
#include "dbmanager.h"
#include "AccountCacheMgr.h"
#include <algorithm>

#ifdef __INTERNALSERVER
	const int MAX_CACHE_ALLOC = 100;
#else
	const int MAX_CACHE_ALLOC = 5000;
#endif


// TODO: list와 map을 두개 두고 하지 말고, 하나로 하는 것이 좋을 것 같다.
//       그리고, m_2AccountPtr도 제거하자. ( 찾은 것을 앞으로 두면 검색도 빠를 수 있을 것 같다. )
CAccountCacheMgr::CAccountCacheMgr()
{
	m_nAlloc = 0;
}

ACCOUNT_CACHE* CAccountCacheMgr::Find( LPCTSTR szAccount )
{
	map<string, ACCOUNT_CACHE*>::iterator it = m_2Account.find( szAccount );
	if( it != m_2Account.end() )
		return it->second;
	else
		return NULL;
}

void CAccountCacheMgr::Clear()
{
	ACCOUNT_CACHE* AccountCache;
	for( map<string, ACCOUNT_CACHE*>::iterator i = m_2Account.begin(); i != m_2Account.end(); ++i )
	{
		AccountCache = i->second;
		AccountCache->Clear();
		safe_delete( AccountCache );
	}
	m_2Account.clear();

	m_list.clear();
	m_nAlloc = 0;
}

void CAccountCacheMgr::AddMover( u_long idPlayer, LPCTSTR szAccount, int nSlot )
{
//	m_2AccountPtr.insert( make_pair( idPlayer, szAccount ) );
	ACCOUNT_CACHE* pCache = Find( szAccount );
	if( pCache )
		pCache->ClearMover( nSlot, idPlayer );
}


void CAccountCacheMgr::RemoveMover( u_long idPlayer, LPCTSTR szAccount )
{
//	m_2AccountPtr.erase( idPlayer );
	ACCOUNT_CACHE* AccountCache = Find( szAccount );
	if( AccountCache )
	{
		for( int i = 0; i < 3; ++i )
		{
			if( AccountCache->m_idPlayerBank[i] == idPlayer )
			{
				AccountCache->ClearMover( i, 0 );
				break;
			}
		}
	}
}

void CAccountCacheMgr::AddAccount( u_long idPlayer, LPCTSTR szAccount, BOOL bCache, ACCOUNT_CACHE *pCache )
{
//	ULONG2STRING::iterator it = m_2AccountPtr.find( idPlayer );
//	if( it == m_2AccountPtr.end() )
//		m_2AccountPtr.insert( map<u_long, string>::value_type( idPlayer, szAccount ) );
	if( !bCache )
		m_2Account.insert( map<string, ACCOUNT_CACHE*>::value_type( szAccount, pCache ) );
}

/*
ACCOUNT_CACHE* CAccountCacheMgr::Find( u_long idPlayer )
{
	ULONG2STRING::iterator it = m_2AccountPtr.find( idPlayer );
	if( it == m_2AccountPtr.end() )
	{
		Error( "Error :: Get Playerid->Account Find : %d", idPlayer );
		return NULL;
	}

	string strAccount = it->second;
	ACCOUNT_CACHE* pCache = Find( strAccount.c_str() );
	if( !pCache )
		Error( "Error :: Get Account->AccountCache Find : %s", strAccount.c_str() );

	return pCache;
}
*/

ACCOUNT_CACHE* CAccountCacheMgr::GetAccount( LPCTSTR szAccount, BOOL *pbCacheHit )
{
	ACCOUNT_CACHE* pCache = Find( szAccount );
	if( pCache )
	{
		*pbCacheHit = TRUE;

		list< ACCOUNT_CACHE* >::iterator where = std::find( m_list.begin(), m_list.end(), pCache );
		assert( where != m_list.end() );
		m_list.erase( where );
		m_list.push_front( pCache );					// 가장 최신에 사용한 것으로 설정 
		pCache->m_tmLastAccess	= ::time( NULL );
	}
	else
	{
		if( m_nAlloc++ < MAX_CACHE_ALLOC ) 
			pCache = new ACCOUNT_CACHE;
		else
		{	
			int cbList	= m_list.size();
			while( cbList > 0 )
			{
				pCache	= m_list.back();
				m_list.pop_back();
				if( pCache->IsReleaseable() )
				{
					m_2Account.erase( pCache->m_strAccount.c_str() );	
					pCache->Clear();
					break;
				}
				m_list.push_front( pCache );
				cbList--;
			}
			if( cbList == 0 )
				pCache	= new ACCOUNT_CACHE;
		}
		m_list.push_front( pCache );					// 가장 최신에 사용한 것으로 설정 

		pCache->m_strAccount = szAccount; 
		pCache->Init();									// 뱅크 메모리 해제
	}

	return pCache;
}

#ifdef __INVALID_LOGIN_0320
void CAccountCacheMgr::ChangeMultiServer( LPCTSTR szAccount, u_long uMultiServer )
{
	g_DbManager.m_AddRemoveLock.Enter();
	ACCOUNT_CACHE* pCache	= Find( szAccount );
	if( pCache )
		pCache->ChangeMultiServer( uMultiServer );
	g_DbManager.m_AddRemoveLock.Leave();
}

int CAccountCacheMgr::InitMultiServer( u_long uMultiServer )
{
	int cbPlayer	= 0;
	g_DbManager.m_AddRemoveLock.Enter();
	ACCOUNT_CACHE* pCache;
	for( map<string, ACCOUNT_CACHE*>::iterator i = m_2Account.begin(); i != m_2Account.end(); ++i )
	{
		pCache	= i->second;
		if( pCache->GetMultiServer() == uMultiServer )
		{
			pCache->ChangeMultiServer( 0 );
			cbPlayer++;
		}
	}
	g_DbManager.m_AddRemoveLock.Leave();
	return cbPlayer;
}
#endif	// __INVALID_LOGIN_0320