#include "stdafx.h"

#if __VER >= 11 // __SYS_POCKET

#include "pocket.h"

#ifdef __WORLDSERVER
#include "user.h"
#endif	// __WORLDSERVER

CPocket::CPocket()
{
	m_bExpired	= TRUE;
	m_tExpirationDate	= 0;
}

CPocket::~CPocket()
{
}

void CPocket::Copy( CPocket & rPocket )
{
	CItemContainer<CItemElem>::Copy( rPocket );
	m_bExpired	= rPocket.IsExpired();
	m_tExpirationDate	= rPocket.GetExpirationDate();
}

void CPocket::Serialize( CAr & ar )
{
	CItemContainer<CItemElem>::Serialize( ar );
	if( ar.IsStoring() )
	{
		ar << m_bExpired << m_tExpirationDate;
		if( m_tExpirationDate )
		{
			time_t	t	= m_tExpirationDate - time_null();
			ar << t;
		}
	}
	else
	{
		ar >> m_bExpired >> m_tExpirationDate;
		if( m_tExpirationDate )
		{
			time_t	t;
			ar >> t;
#ifdef __CLIENT
			m_tExpirationDate	= time_null() + t;
#endif	// __CLIENT
		}
	}
}

CPocketController::CPocketController()
{
#ifdef __WORLDSERVER
	m_pMover	= NULL;
#endif	// __WORLDSERVER
	memset( m_apPocket, 0, sizeof(m_apPocket) );
#ifndef __OPT_MEM_0811
	Avail( 0 );
#endif	// __OPT_MEM_0811
}

CPocketController::~CPocketController()
{
	Clear();
}

void CPocketController::Clear( void )
{
	for( int i = 0; i < MAX_POCKET; i++ )
		SAFE_DELETE( m_apPocket[i] );
}

void CPocketController::Avail( int nPocket, time_t t )
{
	time_t tCurrent		= time_null();
	if( m_apPocket[nPocket] )
	{
		if( m_apPocket[nPocket]->IsExpired() )
			m_apPocket[nPocket]->SetExpirationDate( tCurrent + t );
		else
			m_apPocket[nPocket]->SetExpirationDate( m_apPocket[nPocket]->GetExpirationDate() + t );
	}
	else
	{
		m_apPocket[nPocket]		= new CPocket;
		m_apPocket[nPocket]->SetItemContainer( 0, nPocket == 0? POCKET_SLOT_SIZE_0: POCKET_SLOT_SIZE_1_2 );
		m_apPocket[nPocket]->SetExpirationDate( tCurrent + t );
	}
	m_apPocket[nPocket]->SetExpired( FALSE );

	if( t == 0 )
		m_apPocket[nPocket]->SetExpirationDate( 0 );
}

#if defined(__WORLDSERVER) || defined(__CLIENT)
CItemElem*	CPocketController::GetAtId( int nPocket, int nItem, BOOL bExpiration )
{
	if( !IsAvailable( nPocket, bExpiration ) )
		return NULL;
	return m_apPocket[nPocket]->GetAtId( nItem );
}

#ifdef __CLIENT
CItemElem*	CPocketController::GetAt( int nPocket, int nIndex )
{
	if( !IsAvailable( nPocket, FALSE ) )
		return NULL;
	return m_apPocket[nPocket]->GetAt( nIndex );
}
#endif	// __CLIENT
/*
BOOL	CPocketController::Add( CItemElem* pItemElem, vector<PocketController_Result>* apResult )
{
	BYTE anId[POCKET_SLOT_SIZE_1_2];
	short	anNum[POCKET_SLOT_SIZE_1_2];
	BYTE nCount	= 0;
	
	for( int i = 0; i < MAX_POCKET; i++ )
	{
		if( IsAvailable( i ) && m_apPocket[i]->Add( pItemElem, anId, anNum, &nCount ) )
		{
			for( int j = 0; j < nCount; j++ )
			{
				PocketController_Result	result( i, anId[j], anNum[j] ); 
				apResult->push_back( result );
			}
			return TRUE;
		}
	}
	return FALSE;
}
*/

BOOL	CPocketController::Add( int nPocket, CItemElem*pItemElem, vector<PocketController_Result>* apResult )
{
	BYTE anId[POCKET_SLOT_SIZE_1_2];
	short	anNum[POCKET_SLOT_SIZE_1_2];
	BYTE nCount	= 0;

#ifdef __SECURITY_FIXES
	if (nPocket < 0)
		return FALSE;
#endif // __SECURITY_FIXES

	if( IsAvailable( nPocket ) && m_apPocket[nPocket]->Add( pItemElem, anId, anNum, &nCount ) )
	{
		if( apResult )
		{
			for( int i = 0; i < nCount; i++ )
			{
				PocketController_Result	result( nPocket, anId[i], anNum[i] ); 
				apResult->push_back( result );
			}
		}
#ifdef __WORLDSERVER
//		if( m_pMover->IsPlayer() )
			( (CUser*)m_pMover )->AddPocketAddItem( nPocket, pItemElem );
#endif	// __WORLDSERVER
		return TRUE;
	}
	return FALSE;
}

BOOL	CPocketController::RemoveAtId( int nPocket, int nItem, int nNum )	// if nNum < 0, remove all.
{
	if( !IsAvailable( nPocket ) )
		return FALSE;
	CItemElem* pItemElem	= m_apPocket[nPocket]->GetAtId( nItem );
	if( !pItemElem )
		return FALSE;

	if( pItemElem->m_nItemNum > nNum )
		pItemElem->m_nItemNum	-= nNum;
	else
		m_apPocket[nPocket]->RemoveAtId( nItem );

#ifdef __WORLDSERVER
//	if( m_pMover->IsPlayer() )
		( (CUser*)m_pMover )->AddPocketRemoveItem( nPocket, nItem, nNum );
#endif	// __WORLDSERVER

	return TRUE;
}

int	CPocketController::RemoveItem( DWORD dwItemId, int nNum )
{
	short nRemnant	= nNum;
	CItemElem* pItemElem	= NULL;

	for( int i = 0; i < MAX_POCKET; i++ )
	{
		if( IsAvailable( i ) )
		{
			int nMax	= m_apPocket[i]->GetMax();
			for( int j = 0; j < nMax && nRemnant > 0; j++ )
			{
				pItemElem	= m_apPocket[i]->GetAtId( j ) ;
				if( pItemElem && pItemElem->m_dwItemId == dwItemId )
				{
					if( nRemnant > pItemElem->m_nItemNum )
					{
						nRemnant	-= pItemElem->m_nItemNum;
						m_apPocket[i]->RemoveAtId( j );
					}
					else
					{
						pItemElem->m_nItemNum	-= nRemnant;
						nRemnant	= 0;
						return nNum;
					}
				}
			}
		}
	}
	return( nNum - nRemnant );
}

int		CPocketController::GetEmptyCount( void )
{
	int nCount	= 0;
	for( int i = 0; i < MAX_POCKET; i++ )
	{
		if( IsAvailable( i ) )
			nCount	+= m_apPocket[i]->GetEmptyCount();
	}
	return nCount;
}

//	포켓이 완전히 비어있는지 검사
BOOL	CPocketController::IsAllClean( void )
{
	int nCount	= 0,nCountMax = 0;
	for( int i = 0; i < MAX_POCKET; i++ )
	{
		if( m_apPocket[i] != NULL )
		{
			nCount	+= m_apPocket[i]->GetEmptyCount();
			nCountMax	+= m_apPocket[i]->GetMax();
		}
	}
	if( nCount == nCountMax)
		return TRUE;
	else
		return FALSE;
}

int		CPocketController::GetCount( DWORD dwItemId )
{
	int nCount	= 0;
	for( int i = 0; i < MAX_POCKET; i++ )
	{
		if( IsAvailable( i ) )
			nCount	+= m_apPocket[i]->GetCount( dwItemId );
	}
	return nCount;
}

time_t	CPocketController::GetAvailable( int nPocket )
{
	if( IsAvailable( nPocket ) )
	{
		if( m_apPocket[nPocket]->GetExpirationDate() )
			return max( m_apPocket[nPocket]->GetExpirationDate() - time_null(), 0 );
	}
	return 0;
}

void	CPocketController::SetAttribute( int nAttribute, int nPocket, int nData )
{
	switch( nAttribute )
	{
		case expired:
			{
				if( IsAvailable( nPocket, FALSE ) )
					m_apPocket[nPocket]->SetExpired( (BOOL)nData );
				break;
			}
		case avail:
			{
				Avail( nPocket, (time_t)nData );
				break;
			}
		default:
			break;
	}
#ifdef __WORLDSERVER
	( (CUser*)m_pMover )->AddPocketAttribute( nAttribute, nPocket, nData );
#endif	// __WORLDSERVER
}
#endif	// __WORLDSERVER || __CLIENT

void	CPocketController::Copy( CPocketController & rPocketController )
{
	Clear();
	for( int i = 0; i < MAX_POCKET; i++ )
	{
		if( rPocketController.IsAvailable( i, FALSE ) )
		{
			Avail( i );
			m_apPocket[i]->Copy( *rPocketController.m_apPocket[i] );
		}
	}
}

void	CPocketController::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		for( int i = 0; i < MAX_POCKET; i++ )
		{
			if( IsAvailable( i, FALSE ) )
			{
				ar << (BYTE)1;
				m_apPocket[i]->Serialize( ar );
			}
			else
				ar << (BYTE)0;
		}
	}
	else
	{
		Clear();
		for( int i = 0; i < MAX_POCKET; i++ )
		{
			BYTE bExists;
			ar >> bExists;
			if( !bExists )
				continue;
			Avail( i );
			m_apPocket[i]->Serialize( ar );
		}
	}
}

CPocket*	CPocketController::GetPocket( int nPocket )
{
	if( IsAvailable( nPocket, FALSE ) )
		return m_apPocket[nPocket];
	return NULL;
}

#ifdef __WORLDSERVER
void	CPocketController::ProcessExpiring( void )
{
	if( m_pMover->IsNPC() )
		return;
	CUser* pUser	= (CUser*)m_pMover;

	for( int i = 1; i < MAX_POCKET; i++ )
	{
		if( IsAvailable( i ) )
		{
			if( time_null() > m_apPocket[i]->GetExpirationDate() )
			{
				SetAttribute( expired, i, TRUE );
#ifdef __INTERNALSERVER
				pUser->AddPocketView();
#endif	// __INTERNAL_SERVER
			}
		}
	}
}
#endif	// __WORLDSERVER
#endif	// __SYS_POCKET