#ifndef __DPADBILL_H__
#define	__DPADBILL_H__

#include "dpmng.h"

#include "dbmanager.h"
extern	CDbManager	g_DbManager;


class CDPAdbill : public CDPMng<CBuffer3>
{
private:
	DWORD	m_dwKey;
public:
//	Constructions
	CDPAdbill();
	virtual	~CDPAdbill();
//	Overrides
	virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );

	void	OnAddConnection( DPID dpid );
	void	OnRemoveConnection( DPID dpid );

	static	CDPAdbill*	GetInstance();
};

//________________________________________________________________________________

class CBuyingInfoMng
{
private:
	map<DWORD, PBUYING_INFO3>	m_mapPbi3;

public:
	CMclCritSec		m_AddRemoveLock;

public:
//	Constructions
	CBuyingInfoMng()	{}
	~CBuyingInfoMng()	{	Clear();	}
	void	Clear( void )
		{
			m_AddRemoveLock.Enter();
			for( map<DWORD, PBUYING_INFO3>::iterator i = m_mapPbi3.begin(); i != m_mapPbi3.end(); ++i )
				safe_delete( i->second );
			m_mapPbi3.clear();
			m_AddRemoveLock.Leave();
		}
//	Operations
	void	Add( PBUYING_INFO3 pbi3 )
		{
			m_AddRemoveLock.Enter();
			m_mapPbi3.insert( map<DWORD, PBUYING_INFO3>::value_type( pbi3->dwKey, pbi3 ) );
			m_AddRemoveLock.Leave();
		}
	void	Remove( DWORD dwKey )
		{
			m_AddRemoveLock.Enter();
			m_mapPbi3.erase( dwKey );
			m_AddRemoveLock.Leave();
		}

	PBUYING_INFO3	Get( DWORD dwKey )
		{
			map<DWORD, PBUYING_INFO3>::iterator i	= m_mapPbi3.find( dwKey );
			if( i != m_mapPbi3.end() )
				return i->second;
			return NULL;
		}
	
	void	Process( void )
		{
			DWORD dwTickCount	= GetTickCount();
//			static DWORD adwKey[1024];
			vector<DWORD>	adwKey;
			
			int nSize	= 0;
			
			m_AddRemoveLock.Enter();
			for( map<DWORD, PBUYING_INFO3>::iterator i = m_mapPbi3.begin(); i != m_mapPbi3.end(); ++i )
			{
				PBUYING_INFO3 pbi3	= i->second;
				if( dwTickCount - pbi3->dwTickCount > SEC( 3 ) )
				{
					CDPAdbill::GetInstance()->Send( pbi3, sizeof(BUYING_INFO), pbi3->dpid );
//					adwKey[nSize++]	= pbi3->dwKey;
					adwKey.push_back( pbi3->dwKey );
				}
			}
			
//			Remove
//			for( int j = 0; j < nSize; j++ )
			for( DWORD j = 0; j < adwKey.size(); j++ )
			{
//				PBUYING_INFO3 ptr	= CBuyingInfoMng::GetInstance()->Get( adwKey[j] );
				PBUYING_INFO3 ptr	= CBuyingInfoMng::GetInstance()->Get( adwKey[j] );
				if( ptr )
				{
					static	char lpOutputString[1024]	= { 0, };
					sprintf( lpOutputString, "ACCOUNTSERVER.EXE\t// timeout\t// dwPlayerId = %07d\t// dwItemId = %d", ptr->dwPlayerId, ptr->dwItemId );
					OutputDebugString( lpOutputString );
					/*
					ptr->szBxaid		// 구매 번호
					ptr->dwItemId	// 아이템 인덱스
					ptr->dwItemNum	// 아이템 개수
					ptr->dwServerIndex	// 서버 인덱스
					ptr->dwPlayerId		// 플레이어 식별자
					ptr->dwRetVal	// 지급 성공 여부, 성공 : 1, 실패 : 0
					*/
					BUYING_INFO2	bi2;
					DWORD sn = 0;
					strcpy( bi2.szBxaid, ptr->szBxaid );
					bi2.dwItemId = ptr->dwItemId;
					bi2.dwItemNum = ptr->dwItemNum;
					bi2.dwServerIndex = ptr->dwServerIndex;
					bi2.dwRetVal = ptr->dwRetVal;
					bi2.dwPlayerId = ptr->dwPlayerId;
					CAr ar;
					ar.Write( (void*)&bi2, sizeof(BUYING_INFO2) );
					ar << sn;

					int nBufSize;
					LPBYTE lpData	= ar.GetBuffer( &nBufSize );
					LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.m_pDbIOData->Alloc();
					memcpy( lpDbOverlappedPlus->lpBuf, lpData, nBufSize );
					lpDbOverlappedPlus->uBufSize	= nBufSize;
					lpDbOverlappedPlus->nQueryMode	= LOG_SM_ITEM;
					PostQueuedCompletionStatus( g_DbManager.m_hDbCompletionPort, 1, NULL, &lpDbOverlappedPlus->Overlapped );
				}
				Remove( adwKey[j] );
				SAFE_DELETE( ptr );
			}

			m_AddRemoveLock.Leave();

			adwKey.clear();
		}

	static	CBuyingInfoMng*	GetInstance();
};
#endif	// __DPADBILL_H__