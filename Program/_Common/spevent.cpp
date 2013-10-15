#include "stdafx.h"
#include "spevent.h"
#include "script.h"
#ifdef __WORLDSERVER
#include "user.h"
extern	CUserMng	g_UserMng;
#ifdef __EVENT_0117
#include "respawn.h"
#include "worldmng.h"
extern	CWorldMng	g_WorldMng;
#endif	// __EVENT_0117
#endif	// __WORLDSERVER

CXMasEvent::CXMasEvent()
: CSPEvent()
{
	m_dwTimeout	= GetTickCount();
	memset( (void*)m_adwInterval, 0, sizeof(DWORD) * 24 );
	m_lSkip		= 0;
}

CXMasEvent::~CXMasEvent()
{

}

BOOL CXMasEvent::IsTimeout( int nHour )
{
	DWORD dwTickCount	= GetTickCount();
	if( dwTickCount >= m_dwTimeout )
	{
		m_dwTimeout	= dwTickCount + m_adwInterval[nHour];

		if( m_lSkip > 0 )
		{
			m_lSkip--;
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CXMasEvent::LoadScript( LPCTSTR lpFilename )
{
	CScript s;
	if( s.Load( lpFilename ) == FALSE )
		return FALSE;
	
	int an[24]	= { 0, };
	int nTotal	= 0;
	
	int nMax	= s.GetNumber();	// drop num a day

	for( int i = 0; i < 24; i++ )
	{
		an[i]	= s.GetNumber();
		nTotal	+= an[i];
	}
	for( i = 0; i < 24; i++ )
		m_adwInterval[i]	= (DWORD)( (float)MIN( 60 ) / ( nMax * (float)an[i] / (float)nTotal ) );

	return TRUE;
}

void CXMasEvent::Skip( LONG lSkip )
{
	m_lSkip		= lSkip;
}

CXMasEvent*	CXMasEvent::GetInstance( void )
{
	static	CXMasEvent	sXMasEvent;
	return	&sXMasEvent;
}

CEventItem::CEventItem()
{
	m_dwItemId	= 0;
	m_nMax	= 0;
	m_nNum	= 0;
	m_dwTimeout	= 0;
	m_lSkip	= 0;
	memset( (void*)m_adwInterval, 0, sizeof(m_adwInterval) );
}

CEventItem::CEventItem( DWORD dwItemId, int nMax, int nNum )
{
	m_dwItemId	= dwItemId;
	m_nMax	= nMax;
	m_nNum	= nNum;
	m_dwTimeout	= GetTickCount();
	m_lSkip	= 0;

	static	int	s_anHour[24]	=
		{	505, 409, 324, 280, 220, 203, 202, 212,
			227, 261, 302, 349, 571, 701, 764, 803,
			790, 789, 754, 849, 936, 940, 919, 720	};
	int nTotal	= 0;

	for( int i = 0; i < 24; i++ )
		nTotal	+= s_anHour[i];
	for( i = 0; i < 24; i++ )
		m_adwInterval[i]	= (DWORD)( (float)MIN( 60 ) / ( nMax * (float)s_anHour[i] / (float)nTotal ) );
}

BOOL CEventItem::IsTimeout( int nHour )
{
	DWORD dwTickCount	= GetTickCount();
	if( dwTickCount >= m_dwTimeout )
	{
		m_dwTimeout	= dwTickCount + m_adwInterval[nHour];

		if( m_lSkip > 0 )
		{
			m_lSkip--;
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

void CEventItem::Skip( LONG lSkip )
{
	m_lSkip		= lSkip;
}

void CEventItem::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_dwItemId;
		ar << m_nMax;
		ar << m_nNum;
		ar.Write( (const void*)m_adwInterval, sizeof(m_adwInterval) );
	}
	else
	{
		ar >> m_dwItemId;
		ar >> m_nMax;
		ar >> m_nNum;
		ar.Read( (void*)m_adwInterval, sizeof(m_adwInterval) );
		m_dwTimeout		= GetTickCount();
	}
}

CEventGeneric::CEventGeneric()
{
	m_dwFlag	= 0;
}

CEventGeneric::~CEventGeneric()
{
	Clear();	// clear all
}

void CEventGeneric::Clear( BOOL bDestructor )
{
	for( list<EVENT_GENERIC*>::iterator i = m_lspEvent.begin(); i != m_lspEvent.end(); ++i )
		safe_delete( *i );
	m_lspEvent.clear();

	for( map<int, list<CEventItem*>*>::iterator i2 = m_mapEventItemList.begin(); i2 != m_mapEventItemList.end(); ++i2 )
	{
		list<CEventItem*>* pList	= i2->second;
		for( list<CEventItem*>::iterator i3 = pList->begin(); i3 != pList->end(); ++i3 )
			safe_delete( *i3 );
		pList->clear();
		safe_delete( pList );
	}
	m_mapEventItemList.clear();
#ifdef __EVENT_0117
	if( bDestructor )
	{
		for( map<int, CSpawn*>::iterator i3	= m_mapSpawn.begin(); i3 != m_mapSpawn.end(); ++i3 )
			safe_delete( i3->second );
		m_mapSpawn.clear();
	}
#endif	// __EVENT_0117
}

BOOL CEventGeneric::LoadScript( LPCSTR lpFilename )
{
	CScript s;
	if( s.Load( lpFilename ) == FALSE )
		return FALSE;
	s.GetToken();	// subject or FINISHED
	
//	int nEvent	= 500;

	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "Event" ) )
		{
			PEVENT_GENERIC pEvent	= new EVENT_GENERIC;
			pEvent->nId		= s.GetNumber();
//			pEvent->nId	= nEvent++;
			pEvent->nFlag	= 0x00000001 << ( pEvent->nId - 500 );
			m_lspEvent.push_back( pEvent );

			s.GetToken();	// {
			s.GetToken();
			while( *s.token != '}' )
			{
				if( s.Token == _T( "Title" ) )
				{
					s.GetToken();	// {
					int nLang	= s.GetNumber();
					while( *s.token != '}' )
					{
						s.GetLastFull();
						CString token	= s.token;
						token.Replace( "\"", "" );
						if( nLang == ::GetLanguage() )
						{
							CString str		= token;
//							if( str.IsEmpty() )
//								str	= "Empty";
							lstrcpy( pEvent->pszTitle, (LPCSTR)str );
						}
						nLang	= s.GetNumber();
					}
				}

				else if( s.Token == _T( "Time" ) )
				{
#ifdef __DBSERVER
					s.GetToken();	// {
					int nYear	= s.GetNumber();
					int nMonth	= s.GetNumber();
					int nDay	= s.GetNumber();
					int nHour	= s.GetNumber();
					CTime t	= CTime( nYear, nMonth, nDay, nHour, 0, 0 );
					pEvent->tStart	= (time_t)( t.GetTime() );
					nYear	= s.GetNumber();
					nMonth	= s.GetNumber();
					nDay	= s.GetNumber();
					nHour	= s.GetNumber();
					t	= CTime( nYear, nMonth, nDay, nHour, 0, 0 );
					pEvent->tEnd	= (time_t)( t.GetTime() );
					s.GetToken();	// }
#else	// __DBSERVER
					s.GetToken();
					s.GetToken();
					s.GetToken();
					s.GetToken();
					s.GetToken();
					s.GetToken();
					s.GetToken();
					s.GetToken();
					s.GetToken();
					s.GetToken();
#endif	// __DBSERVER
				}
				else if( s.Token == _T( "Item" ) )
				{
#ifdef __DBSERVER
					s.GetToken();	// {
					DWORD dwItemId	= s.GetNumber();
					while( *s.token != '}' )
					{
						if( dwItemId == 0 )
						{
							//
						}
						int nMax	= s.GetNumber();
						int nNum	= s.GetNumber();
						if( FALSE == AddItem( pEvent->nId, dwItemId, nMax, nNum ) )
						{
							//
						}
						dwItemId	= s.GetNumber();
					}
#else	// __DBSERVER
					s.GetToken();
					s.GetToken();
					while( *s.token != '}' )
					{
						s.GetToken();
						s.GetToken();
						s.GetToken();
					}
#endif	// __DBSERVER
				}
				else if( s.Token == _T( "fExpFactor" ) )
				{
#ifdef __DBSERVER
					pEvent->fExpFactor	= s.GetFloat();
#else	// __DBSERVER
					s.GetToken();
#endif	// __DBSERVER
				}
#ifdef __ITEMDROPRATE
				else if( s.Token == _T( "fItemDropRate" ) )
				{
#ifdef __DBSERVER
					pEvent->fItemDropRate = s.GetFloat();
#else	// __DBSERVER
					s.GetToken();
#endif	// __DBSERVER
				}
#endif // __ITEMDROPRATE
				else if( s.Token == _T( "Spawn" ) )
				{
#if defined(__WORLDSERVER ) && defined(__EVENT_0117)
					s.GetToken();	// {
					DWORD dwType	= s.GetNumber();
					DWORD dwIndex	= s.GetNumber();
					int nMax	= s.GetNumber();
					float fRatio	= s.GetFloat();
					DWORD dwInterval	= 86400000 / nMax;	// 86400000	= 1일
					AddSpawn( pEvent->nId, dwType, dwIndex, nMax, fRatio, dwInterval );
					s.GetToken();	// }
#else	// __WORLDSERVER
					s.GetToken();
					s.GetToken();
					s.GetToken();
					s.GetToken();
					s.GetToken();
					s.GetToken();
#endif	// __WORLDSERVER
				}
				s.GetToken();
			}
		}
		s.GetToken();
	}
	return TRUE;
}

BOOL CEventGeneric::AddItem( int nEvent, DWORD dwItemId, int nMax, int nNum )
{
	list<CEventItem*>*	pList	= NULL;
	map<int, list<CEventItem*>*>::iterator i	= m_mapEventItemList.find( nEvent );
	if( i == m_mapEventItemList.end() )
	{
		pList	= new list<CEventItem*>;
		bool bResult	= m_mapEventItemList.insert( map<int, list<CEventItem*>*>::value_type( nEvent, pList ) ).second;
		if( !bResult )	// ?
			return FALSE;
	}
	else
	{
		pList	= i->second;
	}
	CEventItem* pEventItem	= new CEventItem( dwItemId, nMax, nNum );
	pList->push_back( pEventItem );
	return TRUE;
}

CEventGeneric*	CEventGeneric::GetInstance( void )
{
	static CEventGeneric	sEventGeneric;
	return &sEventGeneric;
}

#ifdef __WORLDSERVER
CEventItem* CEventGeneric::GetItem( int* pnNum )
{
	int nHour	= CTime::GetCurrentTime().GetHour();
	for( map<int, list<CEventItem*>*>::iterator i = m_mapEventItemList.begin(); i != m_mapEventItemList.end(); ++i )
	{
		if( g_eLocal.GetState( i->first ) )
		{
			list<CEventItem*>* pList	= i->second;
			for( list<CEventItem*>::iterator i2 = pList->begin(); i2 != pList->end(); ++i2 )
			{
				CEventItem* pEventItem	= *i2;
				if( pEventItem->IsTimeout( nHour ) )
				{
					int nNum	= xRandom( 1, pEventItem->m_nNum + 1 );
					pEventItem->Skip( (LONG)( nNum - 1 ) );
					*pnNum	= nNum;
					return pEventItem;
				}
			}
		}
	}
	return NULL;
}

FLOAT CEventGeneric::GetExpFactor( void )
{
	FLOAT	fExpFactor	= 1.0f;
	for( list<EVENT_GENERIC*>::iterator i = m_lspEvent.begin(); i != m_lspEvent.end(); ++i )
	{
		PEVENT_GENERIC pEvent	= *i;
		if( g_eLocal.GetState( pEvent->nId ) )
			fExpFactor	*= pEvent->fExpFactor;
	}
	return fExpFactor;
}

#ifdef __ITEMDROPRATE
FLOAT CEventGeneric::GetItemDropRateFactor( void )
{
	FLOAT	fFactor	= 1.0f;
	for( list<EVENT_GENERIC*>::iterator i = m_lspEvent.begin(); i != m_lspEvent.end(); ++i )
	{
		PEVENT_GENERIC pEvent	= *i;
		if( g_eLocal.GetState( pEvent->nId ) )
			fFactor	*= pEvent->fItemDropRate;
	}
	return fFactor;
}
#endif // __ITEMDROPRATE
#endif	// __WORLDSERVER
void CEventGeneric::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << (int)m_lspEvent.size();
		for( list<EVENT_GENERIC*>::iterator i = m_lspEvent.begin(); i != m_lspEvent.end(); ++i )
		{
			PEVENT_GENERIC pEvent	= *i;
			ar.Write( (const void*)pEvent, sizeof(EVENT_GENERIC) );
		}
		ar << (int)m_mapEventItemList.size();
		for( map<int, list<CEventItem*>*>::iterator i2 = m_mapEventItemList.begin(); i2 != m_mapEventItemList.end(); ++i2 )
		{
			ar << (int)i2->first;
			list<CEventItem*>* pList	= i2->second;
			ar << (int)pList->size();
			for( list<CEventItem*>::iterator i3 = pList->begin(); i3 != pList->end(); ++i3 )
			{
				CEventItem* pEventItem	= *i3;
				pEventItem->Serialize( ar );
			}
		}
	}
	else
	{
		Clear( FALSE );
		int nEventSize;
		ar >> nEventSize;
		for( int i = 0; i < nEventSize; i++ )
		{
			PEVENT_GENERIC pEvent	= new EVENT_GENERIC;
			ar.Read( (void*)pEvent, sizeof(EVENT_GENERIC) );
			m_lspEvent.push_back( pEvent );
		}
		int nEventItemListSize;
		ar >> nEventItemListSize;
		for( i = 0; i < nEventItemListSize; i++ )
		{
			int nEvent;
			ar >> nEvent;
			list<CEventItem*>* pList	= new list<CEventItem*>;
			bool bResult	= m_mapEventItemList.insert( map<int, list<CEventItem*>*>::value_type( nEvent, pList ) ).second;
			if( !bResult )
			{
				//
			}
			int nEventItemSize;
			ar >> nEventItemSize;
			for( int j = 0; j < nEventItemSize; j++ )
			{
				CEventItem* pEventItem	= new CEventItem;
				pEventItem->Serialize( ar );
				pList->push_back( pEventItem );
			}
		}
	}
}

BOOL CEventGeneric::Run( void )
{
	BOOL f	= FALSE;
	time_t t	= time_null();
	for( list<EVENT_GENERIC*>::iterator i = m_lspEvent.begin(); i != m_lspEvent.end(); ++i )
	{
		PEVENT_GENERIC pEvent	= *i;
		BOOL bEvent	= ( t >= pEvent->tStart && t < pEvent->tEnd );
		// 康
		char lpOutputString[512]	= { 0, };
		sprintf( lpOutputString, "m_dwFlag=0x%08x, nId=%d, tStart=%d, tEnd=%d, t=%d, nFlag=%d", m_dwFlag, pEvent->nId, pEvent->tStart, pEvent->tEnd, time_null(), pEvent->nFlag );
		OutputDebugString( lpOutputString );

		if( bEvent && !( m_dwFlag & pEvent->nFlag ) )
		{
			m_dwFlag	|= pEvent->nFlag;
			f	= TRUE;
		}
		else if( !bEvent && ( m_dwFlag & pEvent->nFlag ) )
		{
			m_dwFlag	&= ~pEvent->nFlag;
			f	= TRUE;
		}
	}
	return f;
}

PEVENT_GENERIC CEventGeneric::GetEvent( int nEvent )
{
	for( list<EVENT_GENERIC*>::iterator i = m_lspEvent.begin(); i != m_lspEvent.end(); ++i )
	{
		PEVENT_GENERIC pEvent	= *i;
		if( pEvent->nId == nEvent )
			return pEvent;
	}
	return NULL;
}

#ifdef __WORLDSERVER
#ifdef __EVENT_1101
LONG CEventGeneric::GetEventElapsed( void )
{
	PEVENT_GENERIC pEvent	= GetEvent( 531 );
	if( !pEvent )
		return 0;
	CTime tStart	= CTime( pEvent->tStart );
	CTime tCur	= CTime::GetCurrentTime();
	if( tCur < tStart )
		return 0;
	CTimeSpan ts	= tCur - tStart;
	return	( (LONG)( ts.GetDays() ) );
}

void CEventGeneric::CallTheRoll( void )
{
	PEVENT_GENERIC pEvent	= GetEvent( 531 );	// 이벤트가 등록되어 있지 않으면
	if( !pEvent )
		return;

	if( g_eLocal.GetState( 531 ) == 0 )	// 이벤트 기간이 아니면
		return;

	int nElapsed	= (int)GetEventElapsed();
	g_UserMng.CallTheRoll( nElapsed );
}
#endif	// __EVENT_1101
#endif	// __WORLDSERVER

#ifdef __EVENT_0117
void CEventGeneric::AddSpawn( int nEvent, DWORD dwType, DWORD dwIndex, int nMax, float fRatio, DWORD dwInterval )
{
	CSpawn* pSpawn	= new CSpawn( dwType, dwIndex, nMax, fRatio, dwInterval );
	bool bResult	= m_mapSpawn.insert( map<int, CSpawn*>::value_type( nEvent, pSpawn ) ).second;
}

void CEventGeneric::AddRegionGeneric( int nLevel, DWORD dwWorldId, CRespawnInfo* pi )
{
	REGION_GENERIC	regionGeneric;
	regionGeneric.nLevel	= nLevel;
	regionGeneric.dwWorldId		= dwWorldId;
	regionGeneric.pi	= pi;

	m_aRegionGeneric.push_back( regionGeneric );
}

bool prRegionGeneric( REGION_GENERIC r1, REGION_GENERIC r2 )
{
	return r2.nLevel > r1.nLevel;
}

void CEventGeneric::SortRegionGeneric( void )
{
	std::sort( m_aRegionGeneric.begin(), m_aRegionGeneric.end(),  prRegionGeneric );
}

void CEventGeneric::Spawn( void )
{
	if( m_aRegionGeneric.size() == 0 )
		return;
	for( map<int, CSpawn*>::iterator i	= m_mapSpawn.begin(); i != m_mapSpawn.end(); ++i )
	{
		if( g_eLocal.GetState( i->first ) )
		{
			CSpawn* pSpawn	= i->second;
			if( pSpawn->IsTimeout() )
			{
				int nIndex	= pSpawn->GetSpawnIndex( m_aRegionGeneric.size() );
				CWorld* pWorld	= g_WorldMng.GetWorld( m_aRegionGeneric[nIndex].dwWorldId );
				if( pWorld )
				{
					D3DXVECTOR3 v;
					m_aRegionGeneric[nIndex].pi->GetPos( v );
					v.y		=	pWorld->GetLandHeight( v.x, v.z ) + 1.0f;
					CObj* pObj	= CreateObj( D3DDEVICE, pSpawn->m_dwType, pSpawn->m_dwIndex );
					if( pSpawn->m_dwType == OT_ITEM )
					{
						CItemElem* pItemElem	= new CItemElem;
						pItemElem->m_dwItemId	= pSpawn->m_dwIndex;
						pItemElem->m_nItemNum	= 1;
						ItemProp* pItemProp	= pItemElem->GetProp();
						pItemElem->m_nHitPoint	= pItemProp->dwEndurance;
						pItemElem->SetSerialNumber();
						( (CItem*)pObj )->m_dwDropTime	= g_tmCurrent + MIN( 1440 );	// 24시간 유지
						( (CItem*)pObj )->m_pItemBase	= pItemElem;
						pObj->SetPos( v );
//#ifdef _DEBUG
						char lpOutputString[100]	= { 0, };
						sprintf( lpOutputString, "spawn: type = %d, index = %d, world = %d, x = %f, z = %f\n", OT_ITEM, pItemElem->m_dwItemId, pWorld->GetID(), pObj->GetPos().x, pObj->GetPos().z );
						//g_UserMng.AddWorldMsg( WI_WORLD_MADRIGAL, lpOutputString );
						OutputDebugString( lpOutputString );
//#endif	// _DEBUG
						pWorld->ADDOBJ( pObj, TRUE, nTempLayer );
					}
					else
					{
						// 몬스터와 컨트롤 스폰은 이 후 추가 예정
						SAFE_DELETE( pObj );
					}
				}	// pWorld
			}	// IsTimeout
		}
	}
}

CSpawn::CSpawn()
{
	m_nMax	= 0;
	m_fRatio	= 0.0F;
	m_dwType	= 0;
	m_dwIndex	= 0;
	m_dwInterval	= 0;
	m_dwTimeout		= 0;
}

CSpawn::CSpawn( DWORD dwType, DWORD dwIndex, int nMax, float fRatio, DWORD dwInterval )
{
	m_dwType	= dwType;
	m_dwIndex	= dwIndex;
	m_nMax	= nMax;
	m_fRatio	= fRatio;
	m_dwInterval	= dwInterval;
	m_dwTimeout		= 0;
}

BOOL CSpawn::IsTimeout( void )
{
	if( g_tmCurrent >= m_dwTimeout )
	{
		m_dwTimeout		= g_tmCurrent + m_dwInterval;
		return TRUE;
	}
	return FALSE;
}
#endif	// __EVENT_0117