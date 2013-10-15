#include "StdAfx.h"
#ifdef __IMPROVE_MAP_SYSTEM
#ifdef __CLIENT
#include "RainbowNPCInformation.h"

//-----------------------------------------------------------------------------
CRainbowNPCInformation::CRainbowNPCInformation( void ) : 
m_nMapID( 0 ), 
m_rectWorldPosition( -1, -1, -1, -1 ), 
m_rectRegionPosition( -1, -1, -1, -1 )
{
}
//-----------------------------------------------------------------------------
CRainbowNPCInformation::~CRainbowNPCInformation( void )
{
}
//-----------------------------------------------------------------------------
void CRainbowNPCInformation::SetMapID( int nMapID )
{
	m_nMapID = nMapID;
}
//-----------------------------------------------------------------------------
int CRainbowNPCInformation::GetMapID( void ) const
{
	return m_nMapID;
}
//-----------------------------------------------------------------------------
void CRainbowNPCInformation::SetWorldPositionRect( const CRect& rectWorldPosition )
{
	m_rectWorldPosition = rectWorldPosition;
}
//-----------------------------------------------------------------------------
const CRect& CRainbowNPCInformation::GetWorldPositionRect( void ) const
{
	return m_rectWorldPosition;
}
//-----------------------------------------------------------------------------
void CRainbowNPCInformation::SetRegionPositionRect( const CRect& rectRegionPosition )
{
	m_rectRegionPosition = rectRegionPosition;
}
//-----------------------------------------------------------------------------
const CRect& CRainbowNPCInformation::GetRegionPositionRect( void ) const
{
	return m_rectRegionPosition;
}
//-----------------------------------------------------------------------------
#endif // __CLIENT
#endif // __IMPROVE_MAP_SYSTEM