#include "stdafx.h"
#include "cpdh.h"

CPdh::CPdh()
{
	m_nSizeOfPdhCounter		= 0;
	memset( m_appdhCounter, 0, sizeof(m_appdhCounter) );
}

CPdh::~CPdh()
{

}

BOOL CPdh::OpenQuery( void )
{
	if( PdhOpenQuery( NULL, 1, &m_hQuery ) != ERROR_SUCCESS )
		return FALSE;
	return TRUE;
}

void CPdh::CloseQuery( void )
{
	PdhCloseQuery( &m_hQuery );

	for( int i = 0; i < m_nSizeOfPdhCounter; i++ ) {
		SAFE_DELETE( m_appdhCounter[i] );
	}
}

int CPdh::AddCounter( const char* szFullCounterPath )
{
	PPDH_COUNTER ppdhCounter	= new PDH_COUNTER;
//	add to current query	
	if( PdhAddCounter( m_hQuery, szFullCounterPath, (DWORD)ppdhCounter, &( ppdhCounter->hCounter ) ) != ERROR_SUCCESS ) {
		SAFE_DELETE( ppdhCounter );
		return( -1 );
	}

	m_appdhCounter[m_nSizeOfPdhCounter++]	= ppdhCounter;
	ppdhCounter->lValue		= 0;
	ppdhCounter->nNextIndex		= 0;
	ppdhCounter->nOldestIndex	= 0;
	ppdhCounter->nRawCount	= 0;
	return( m_nSizeOfPdhCounter - 1 );
}

BOOL CPdh::RemoveCounter( int npdhCounter )
{
	PPDH_COUNTER ppdhCounter	= GetPdhCounter( npdhCounter );
	if( !ppdhCounter )
		return FALSE;
	if( PdhRemoveCounter( ppdhCounter->hCounter ) != ERROR_SUCCESS )
		return FALSE;
	return TRUE;
}

BOOL CPdh::CollectQueryData( void )
{
	if( PdhCollectQueryData( m_hQuery ) != ERROR_SUCCESS )
		return FALSE;
	return TRUE;
}

BOOL CPdh::UpdateValue( PPDH_COUNTER ppdhCounter )
{
	PDH_FMT_COUNTERVALUE pdhFormattedValue;
//	get the value from the PDH
	if( PdhGetFormattedCounterValue( ppdhCounter->hCounter, PDH_FMT_LONG, NULL, &pdhFormattedValue ) != ERROR_SUCCESS )
		return FALSE;
//	test the value for validity
	if( pdhFormattedValue.CStatus != ERROR_SUCCESS )
		return FALSE;
//	set value
	ppdhCounter->lValue		= pdhFormattedValue.longValue;
	return TRUE;
}

BOOL CPdh::UpdateRawValue( PPDH_COUNTER ppdhCounter )
{
	PPDH_RAW_COUNTER ppdhRawCounter;
//	assign the next value into the array
	ppdhRawCounter	= &( ppdhCounter->apdhRawCounter[ppdhCounter->nNextIndex] );
	if( PdhGetRawCounterValue( ppdhCounter->hCounter, NULL, ppdhRawCounter ) !=  ERROR_SUCCESS )
		return FALSE;
//	update raw counter
	ppdhCounter->nRawCount	= min( ppdhCounter->nRawCount + 1, MAX_RAW_VALUES );
//	update next index
	ppdhCounter->nNextIndex		= ( ppdhCounter->nNextIndex + 1 ) % MAX_RAW_VALUES;
	// The oldest index remains zero until the array is filled.
	// It will now be the same as the next index since it was previously assigned.
	if( ppdhCounter->nRawCount >= MAX_RAW_VALUES )
		ppdhCounter->nOldestIndex	= ppdhCounter->nNextIndex;
	return TRUE;
}

long CPdh::GetCounterValue( int npdhCounter )
{
	PPDH_COUNTER	ppdhCounter		= GetPdhCounter( npdhCounter );
	if( !ppdhCounter )
		return( -1L );
//	udpate the value(s)
	if( !UpdateValue( ppdhCounter ) )	return( -1 );
	if( !UpdateRawValue( ppdhCounter ) )	return( -1 );

	return( ppdhCounter->lValue );
}

PPDH_COUNTER CPdh::GetPdhCounter( int npdhCounter )
{
	if( npdhCounter >= 0 && npdhCounter < m_nSizeOfPdhCounter )
		return( m_appdhCounter[npdhCounter] );
	return NULL;
}

CPdh1::CPdh1()
{
	OpenQuery();
	m_nProcessor	= AddCounter( CP_PROCESSOR_PROCESSOR_TIME );
	m_nMemory	= AddCounter( CP_MEMORY_COMMITTED_BYTES_IN_USE );
}

CPdh1::~CPdh1()
{
	CloseQuery();
}

long CPdh1::GetProcessorTime( void )	// %
{
	if( !CollectQueryData() )
		return( -1 );
	return GetCounterValue( m_nProcessor );
}

long CPdh1::GetCommittedBytesInUse( void )	// %
{
	if( !CollectQueryData() )
		return( -1 );
	return GetCounterValue( m_nMemory );
}

CPdh1	g_pdh;