#include "StdAfx.h"
#ifdef __IMPROVE_MAP_SYSTEM
#ifdef __CLIENT
#include "TeleportationIconInfo.h"
//-----------------------------------------------------------------------------
TeleportationIconInfo::TeleportationIconInfo( void ) : 
m_nIndex( -1 ), 
m_pointIcon( -1, -1 )
{
}
//-----------------------------------------------------------------------------
TeleportationIconInfo::~TeleportationIconInfo( void )
{
}
//-----------------------------------------------------------------------------
void TeleportationIconInfo::SetIndex( int nIndex )
{
	m_nIndex = nIndex;
}
//-----------------------------------------------------------------------------
int TeleportationIconInfo::GetIndex( void ) const
{
	return m_nIndex;
}
//-----------------------------------------------------------------------------
void TeleportationIconInfo::SetIconPoint( const CPoint& pointIcon )
{
	m_pointIcon = pointIcon;
}
//-----------------------------------------------------------------------------
const CPoint& TeleportationIconInfo::GetIconPoint( void ) const
{
	return m_pointIcon;
}
//-----------------------------------------------------------------------------
#endif // __CLIENT
#endif // __IMPROVE_MAP_SYSTEM