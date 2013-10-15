#include "StdAfx.h"
#ifdef __IMPROVE_MAP_SYSTEM
#ifdef __CLIENT
#include "MapComboBoxData.h"
#include "ContinentDef.h"

//-----------------------------------------------------------------------------
CMapComboBoxData::CMapComboBoxData( DWORD dwID, Category eCategory, const CString& strTitle ) : 
m_dwID( dwID ), 
m_eCategory( eCategory ), 
m_strTitle( strTitle ), 
m_strPictureFileName( _T( "" ) ), 
m_pMapTexture( NULL ), 
m_strMapMonsterInformationFileName( _T( "" ) ), 
m_byLocationID( CONT_NODATA ), 
m_pointNPCPosition( -1, -1 ), 
m_dwParentID( 0 )
{
}
//-----------------------------------------------------------------------------
CMapComboBoxData::~CMapComboBoxData( void )
{
}
//-----------------------------------------------------------------------------
DWORD CMapComboBoxData::GetID( void ) const
{
	return m_dwID;
}
//-----------------------------------------------------------------------------
CMapComboBoxData::Category CMapComboBoxData::GetCategory( void ) const
{
	return m_eCategory;
}
//-----------------------------------------------------------------------------
const CString& CMapComboBoxData::GetTitle( void ) const
{
	return m_strTitle;
}
//-----------------------------------------------------------------------------
void CMapComboBoxData::SetPictureFileName( const CString& strPictureFileName )
{
	m_strPictureFileName = strPictureFileName;
}
//-----------------------------------------------------------------------------
const CString& CMapComboBoxData::GetPictureFileName( void ) const
{
	return m_strPictureFileName;
}
//-----------------------------------------------------------------------------
void CMapComboBoxData::SetMapTexture( CTexture* const pMapTexture )
{
	m_pMapTexture = pMapTexture;
}
//-----------------------------------------------------------------------------
CTexture* const CMapComboBoxData::GetMapTexture( void )
{
	return m_pMapTexture;
}
//-----------------------------------------------------------------------------
void CMapComboBoxData::SetMapMonsterInformationFileName( const CString& strMapMonsterInformationFileName )
{
	m_strMapMonsterInformationFileName = strMapMonsterInformationFileName;
}
//-----------------------------------------------------------------------------
const CString& CMapComboBoxData::GetMapMonsterInformationFileName( void ) const
{
	return m_strMapMonsterInformationFileName;
}
//-----------------------------------------------------------------------------
void CMapComboBoxData::SetLocationID( BYTE byLocationID )
{
	m_byLocationID = byLocationID;
}
//-----------------------------------------------------------------------------
BYTE CMapComboBoxData::GetLocationID( void ) const
{
	return m_byLocationID;
}
//-----------------------------------------------------------------------------
void CMapComboBoxData::SetNPCPosition( const CPoint& pointNPCPosition )
{
	m_pointNPCPosition = pointNPCPosition;
}
//-----------------------------------------------------------------------------
const CPoint& CMapComboBoxData::GetNPCPosition( void ) const
{
	return m_pointNPCPosition;
}
//-----------------------------------------------------------------------------
void CMapComboBoxData::SetParentID( DWORD dwParentID )
{
	m_dwParentID = dwParentID;
}
//-----------------------------------------------------------------------------
DWORD CMapComboBoxData::GetParentID( void ) const
{
	return m_dwParentID;
}
//-----------------------------------------------------------------------------
#endif // __CLIENT
#endif // __IMPROVE_MAP_SYSTEM