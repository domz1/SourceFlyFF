#include "StdAfx.h"
#ifdef __IMPROVE_MAP_SYSTEM
#ifdef __CLIENT
#include "MapMonsterInformation.h"

//-----------------------------------------------------------------------------
CMapMonsterInformation::CMapMonsterInformation( void ) : 
m_nMonsterIDNumber( 0 ), 
m_dwDropItemID( 0 ), 
m_rectIconPosition( 0, 0, 0, 0 )
{
}
//-----------------------------------------------------------------------------
CMapMonsterInformation::~CMapMonsterInformation( void )
{
	m_vecMonsterID.clear();
}
//-----------------------------------------------------------------------------
void CMapMonsterInformation::SetMonsterIDNumber( int nMonsterIDNumber )
{
	m_nMonsterIDNumber = nMonsterIDNumber;
}
//-----------------------------------------------------------------------------
int CMapMonsterInformation::GetMonsterIDNumber( void ) const
{
	return m_nMonsterIDNumber;
}
//-----------------------------------------------------------------------------
void CMapMonsterInformation::InsertMonsterID( DWORD dwMonsterID )
{
	m_vecMonsterID.push_back( dwMonsterID );
}
//-----------------------------------------------------------------------------
DWORD CMapMonsterInformation::GetMonsterID( int nIndex ) const
{
	return m_vecMonsterID[ nIndex ];
}
//-----------------------------------------------------------------------------
vector< DWORD >& CMapMonsterInformation::GetMonsterIDVector( void )
{
	return m_vecMonsterID;
}
//-----------------------------------------------------------------------------
void CMapMonsterInformation::SetDropItemID( DWORD dwDropItemID )
{
	m_dwDropItemID = dwDropItemID;
}
//-----------------------------------------------------------------------------
DWORD CMapMonsterInformation::GetDropItemID( void ) const
{
	return m_dwDropItemID;
}
//-----------------------------------------------------------------------------
void CMapMonsterInformation::SetIconPositionRect( const CRect& rectIconPosition )
{
	m_rectIconPosition = rectIconPosition;
}
//-----------------------------------------------------------------------------
const CRect& CMapMonsterInformation::GetIconPositionRect( void ) const
{
	return m_rectIconPosition;
}
//-----------------------------------------------------------------------------
#endif // __CLIENT
#endif // __IMPROVE_MAP_SYSTEM