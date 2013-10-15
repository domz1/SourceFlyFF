#include "StdAfx.h"
#ifdef __IMPROVE_MAP_SYSTEM
#ifdef __CLIENT
#include "UserMarkPositionInfo.h"
//-----------------------------------------------------------------------------
CUserMarkPositionInfo::CUserMarkPositionInfo( void ) : 
m_strName( _T( "" ) ), 
m_fPositionX( 0.0F ), 
m_fPositionY( 0.0F ), 
m_dwID( 0 )
{
}
//-----------------------------------------------------------------------------
CUserMarkPositionInfo::~CUserMarkPositionInfo( void )
{
}
//-----------------------------------------------------------------------------
void CUserMarkPositionInfo::SetID( DWORD dwID )
{
	m_dwID = dwID;
}
//-----------------------------------------------------------------------------
DWORD CUserMarkPositionInfo::GetID( void ) const
{
	return m_dwID;
}
//-----------------------------------------------------------------------------
void CUserMarkPositionInfo::SetName( const CString& strName )
{
	m_strName = strName;
}
//-----------------------------------------------------------------------------
const CString& CUserMarkPositionInfo::GetName( void ) const
{
	return m_strName;
}
//-----------------------------------------------------------------------------
void CUserMarkPositionInfo::SetPositionX( FLOAT fPositionX )
{
	m_fPositionX = fPositionX;
}
//-----------------------------------------------------------------------------
FLOAT CUserMarkPositionInfo::GetPositionX( void ) const
{
	return m_fPositionX;
}
//-----------------------------------------------------------------------------
void CUserMarkPositionInfo::SetPositionY( FLOAT fPositionY )
{
	m_fPositionY = fPositionY;
}
//-----------------------------------------------------------------------------
FLOAT CUserMarkPositionInfo::GetPositionY( void ) const
{
	return m_fPositionY;
}
//-----------------------------------------------------------------------------
#endif // __CLIENT
#endif // __IMPROVE_MAP_SYSTEM