#include "stdafx.h"
#include "SfxHitArray.h"

CSfxHitArray::CSfxHitArray()
{
	m_id	= 0;
	memset( m_aSfxHit, 0, sizeof(SfxHit) * nMaxSizeOfSfxHit );
}

CSfxHitArray::~CSfxHitArray()
{

}

int CSfxHitArray::Add( int id, OBJID objid, DWORD dwAtkFlags, DWORD dwSkill, int nMaxDmgCnt )
{
	if( id == 0 )	
		id	= ++m_id;

	for( int i = 0; i < nMaxSizeOfSfxHit; i++ )
	{
		if( m_aSfxHit[i].id == 0 )
		{
			m_aSfxHit[i].id		= id;
			m_aSfxHit[i].objid		= objid;
			m_aSfxHit[i].dwAtkFlags		= dwAtkFlags;
			m_aSfxHit[i].dwSkill = dwSkill;
			m_aSfxHit[i].nMaxDmgCnt = nMaxDmgCnt;
			return( id );
		}
	}

	memset( m_aSfxHit, 0, sizeof(m_aSfxHit) );	// 꽉차서 더이상 사용 못하므로 일단 클리어 해줌.
	return( -1 );
}

PSfxHit CSfxHitArray::GetAt( int nIndex )
{
	if( nIndex < 0 || nIndex >= nMaxSizeOfSfxHit )
		return( NULL );
	if( m_aSfxHit[nIndex].id != 0 )
		return( &m_aSfxHit[nIndex] );
	return( NULL );
}

BOOL CSfxHitArray::RemoveAt( int nIndex, BOOL bForce )
{
	if( nIndex < 0 || nIndex >= nMaxSizeOfSfxHit )
		return( FALSE ); 
	if( m_aSfxHit[nIndex].id != 0 )
	{
		if( --m_aSfxHit[nIndex].nMaxDmgCnt == 0 || bForce )		// 카운트를 깎고 0이 됐을때만 완전히 삭제.
			m_aSfxHit[nIndex].id	= 0;
		return( TRUE );
	}
	return( FALSE );
}

int CSfxHitArray::FindSfxHit( int id )
{
	for( int i = 0; i < nMaxSizeOfSfxHit; i++ ) {
		if( m_aSfxHit[i].id == id )
			return( i );
	}
	return( -1 );
}

PSfxHit CSfxHitArray::GetSfxHit( int id )
{
	int nIndex	= FindSfxHit( id );
	if( nIndex < 0 )
		return( NULL );
	return GetAt( nIndex );
}

BOOL CSfxHitArray::RemoveSfxHit( int id, BOOL bForce )
{
	int nIndex	= FindSfxHit( id );
	if( nIndex < 0 )
		return( FALSE );
	return RemoveAt( nIndex, bForce );
}
