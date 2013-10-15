#include "StdAfx.h"
#ifdef __IMPROVE_MAP_SYSTEM
#ifdef __CLIENT
#include "MapInformationManager.h"
#include "defineMapComboBoxData.h"
#include "Project.h"
#include "ContinentDef.h"

//-----------------------------------------------------------------------------
CMapInformationManager::CMapInformationManager( void ) : 
m_pPCArrowTexture( NULL ), 
m_pPartyPCArrowTexture( NULL ), 
m_pTeleportationPositionTexture( NULL ), 
m_pDestinationPositionTexture( NULL ), 
m_pNPCPositionTexture( NULL ), 
m_pUserMarkPositionTexture( NULL ), 
m_pRainbowNPCInformationPack( NULL )
{
}
//-----------------------------------------------------------------------------
CMapInformationManager::~CMapInformationManager( void )
{
	DeleteAllMapInformation();
}
//-----------------------------------------------------------------------------
BOOL CMapInformationManager::LoadMapInformationData( void )
{
	LoadMapIconTexture();
	LoadPropMapComboBoxData();
	LoadMapMonsterInformationPack();
	LoadRainbowNPCInformationPack();

	return TRUE;
}
//-----------------------------------------------------------------------------
void CMapInformationManager::DeleteAllMapInformation( void )
{
	DeleteAllUserMarkPositionInfo();

	SAFE_DELETE( m_pRainbowNPCInformationPack );

	for( map< BYTE, CRect* >::iterator Iterator = m_RealPositionRectMap.begin(); Iterator != m_RealPositionRectMap.end(); ++Iterator )
	{
		SAFE_DELETE( Iterator->second );
	}
	m_RealPositionRectMap.clear();

	for( MapMonsterInformationPackMap::iterator Iterator = m_MapMonsterInformationPackMap.begin(); Iterator != m_MapMonsterInformationPackMap.end(); ++Iterator )
	{
		SAFE_DELETE( Iterator->second );
	}
	m_MapMonsterInformationPackMap.clear();

	for( MapComboBoxDataVector::iterator Iterator = m_MapCategoryVector.begin(); Iterator != m_MapCategoryVector.end(); ++Iterator )
	{
		SAFE_DELETE( *Iterator );
	}
	m_MapCategoryVector.clear();

	for( MapComboBoxDataVector::iterator Iterator = m_MapNameVector.begin(); Iterator != m_MapNameVector.end(); ++Iterator )
	{
		SAFE_DELETE( *Iterator );
	}
	m_MapNameVector.clear();

	for( MapComboBoxDataVector::iterator Iterator = m_NPCNameVector.begin(); Iterator != m_NPCNameVector.end(); ++Iterator )
	{
		SAFE_DELETE( *Iterator );
	}
	m_NPCNameVector.clear();
}
//-----------------------------------------------------------------------------
CTexture* const CMapInformationManager::GetPCArrowTexture( void )
{
	return m_pPCArrowTexture;
}
//-----------------------------------------------------------------------------
CTexture* const CMapInformationManager::GetPartyPCArrowTexture( void )
{
	return m_pPartyPCArrowTexture;
}
//-----------------------------------------------------------------------------
CTexture* const CMapInformationManager::GetTeleportationPositionTexture( void )
{
	return m_pTeleportationPositionTexture;
}
//-----------------------------------------------------------------------------
CTexture* const CMapInformationManager::GetDestinationPositionTexture( void )
{
	return m_pDestinationPositionTexture;
}
//-----------------------------------------------------------------------------
CTexture* const CMapInformationManager::GetNPCPositionTexture( void )
{
	return m_pNPCPositionTexture;
}
//-----------------------------------------------------------------------------
CTexture* const CMapInformationManager::GetUserMarkPositionTexture( void )
{
	return m_pUserMarkPositionTexture;
}
//-----------------------------------------------------------------------------
MapComboBoxDataVector* CMapInformationManager::GetMapCategoryVector( void )
{
	return &m_MapCategoryVector;
}
//-----------------------------------------------------------------------------
MapComboBoxDataVector* CMapInformationManager::GetMapNameVector( void )
{
	return &m_MapNameVector;
}
//-----------------------------------------------------------------------------
MapComboBoxDataVector* CMapInformationManager::GetNPCNameVector( void )
{
	return &m_NPCNameVector;
}
//-----------------------------------------------------------------------------
CMapMonsterInformationPack* CMapInformationManager::FindMapMonsterInformationPack( DWORD dwFirstKey )
{
	MapMonsterInformationPackMap::const_iterator Iterator = m_MapMonsterInformationPackMap.find( dwFirstKey );
	if( Iterator != m_MapMonsterInformationPackMap.end() )
	{
		return Iterator->second;
	}
	return NULL;
}
//-----------------------------------------------------------------------------
CRect& CMapInformationManager::FindRealPosition( BYTE byFirstKey )
{
	static CRect rectNull( -1, -1, -1, -1 );
	map< BYTE, CRect* >::const_iterator Iterator = m_RealPositionRectMap.find( byFirstKey );
	if( Iterator != m_RealPositionRectMap.end() )
	{
		return *( Iterator->second );
	}
	return rectNull;
}
//-----------------------------------------------------------------------------
CRainbowNPCInformationPack* CMapInformationManager::GetRainbowNPCInformationPack( void )
{
	return m_pRainbowNPCInformationPack;
}
//-----------------------------------------------------------------------------
BOOL CMapInformationManager::InsertUserMarkPositionInfo( const CString& strName, FLOAT fPositionX, FLOAT fPositionY )
{
	if( static_cast< int >( m_UserMarkPositionInfoList.size() ) >= USER_MARK_MAX_SIZE )
	{
		g_WndMng.PutString( 
			_T( "더 이상 위치 등록을 할 수 없습니다. 기존에 등록된 위치를 정리하신 후 다시 등록하세요 " ), 
			NULL, 
			0xffff0000 );

		return FALSE;
	}

	static DWORD dwID = 1;
	CUserMarkPositionInfo* pUserMarkPositionInfo = new CUserMarkPositionInfo;
	pUserMarkPositionInfo->SetID( dwID );
	pUserMarkPositionInfo->SetName( strName );
	pUserMarkPositionInfo->SetPositionX( fPositionX );
	pUserMarkPositionInfo->SetPositionY( fPositionY );
	m_UserMarkPositionInfoList.push_back( pUserMarkPositionInfo );
	++dwID;

	return TRUE;
}
//-----------------------------------------------------------------------------
void CMapInformationManager::DeleteUserMarkPositionInfo( DWORD dwID )
{
	for( UserMarkPositionInfoList::iterator Iterator = m_UserMarkPositionInfoList.begin(); Iterator != m_UserMarkPositionInfoList.end(); ++Iterator )
	{
		CUserMarkPositionInfo* pUserMarkPositionInfo = ( CUserMarkPositionInfo* )( *Iterator );
		if( pUserMarkPositionInfo == NULL )
		{
			continue;
		}

		if( dwID == pUserMarkPositionInfo->GetID() )
		{
			SAFE_DELETE( pUserMarkPositionInfo );
			m_UserMarkPositionInfoList.erase( Iterator );

			break;
		}
	}
}
//-----------------------------------------------------------------------------
CUserMarkPositionInfo* const CMapInformationManager::FindUserMarkPositionInfo( DWORD dwID )
{
	for( UserMarkPositionInfoList::iterator Iterator = m_UserMarkPositionInfoList.begin(); Iterator != m_UserMarkPositionInfoList.end(); ++Iterator )
	{
		CUserMarkPositionInfo* const pUserMarkPositionInfo = ( CUserMarkPositionInfo* )( *Iterator );
		if( pUserMarkPositionInfo == NULL )
		{
			continue;
		}

		if( dwID == pUserMarkPositionInfo->GetID() )
		{
			return pUserMarkPositionInfo;
		}
	}
	return NULL;
}
//-----------------------------------------------------------------------------
void CMapInformationManager::DeleteAllUserMarkPositionInfo( void )
{
	for( UserMarkPositionInfoList::iterator Iterator = m_UserMarkPositionInfoList.begin(); Iterator != m_UserMarkPositionInfoList.end(); ++Iterator )
	{
		SAFE_DELETE( *Iterator );
	}
	m_UserMarkPositionInfoList.clear();
}
//-----------------------------------------------------------------------------
UserMarkPositionInfoList& CMapInformationManager::GetUserMarkPositionInfoList( void )
{
	return m_UserMarkPositionInfoList;
}
//-----------------------------------------------------------------------------
BOOL CMapInformationManager::LoadMapIconTexture( void )
{
	CTexture* pPCArrowTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, _T( "ImgMapArrow.bmp" ) ), 0xffff00ff );
	if( pPCArrowTexture )
	{
		m_pPCArrowTexture = pPCArrowTexture;
	}

	CTexture* pPartyPCArrowTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, _T( "ImgMapArrowParty.bmp" ) ), 0xffff00ff );
	if( pPartyPCArrowTexture )
	{
		m_pPartyPCArrowTexture = pPartyPCArrowTexture;
	}

	CTexture* pTeleportationPositionTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "ButtTeleport.bmp"), 0xffff00ff );
	if( pTeleportationPositionTexture )
	{
		m_pTeleportationPositionTexture = pTeleportationPositionTexture;
	}

	CTexture* pDestinationPositionTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "ButtDestination.bmp"), 0xffff00ff );
	if( pDestinationPositionTexture )
	{
		m_pDestinationPositionTexture = pDestinationPositionTexture;
	}

	CTexture* pNPCPositionTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "MapNPCPositionMark.bmp"), 0xffff00ff );
	if( pNPCPositionTexture )
	{
		m_pNPCPositionTexture = pNPCPositionTexture;
	}

	CTexture* pUserMarkPositionTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "WndRainbowRaceLadderFail.bmp"), 0xffff00ff );
	if( pUserMarkPositionTexture )
	{
		m_pUserMarkPositionTexture = pUserMarkPositionTexture;
	}

	return TRUE;
}
//-----------------------------------------------------------------------------
BOOL CMapInformationManager::LoadPropMapComboBoxData( void )
{
	CScript script;
	if( script.Load( _T( "propMapComboBoxData.inc" ) ) == FALSE )
	{
		return FALSE;
	}

	DWORD dwMapComboBoxDataID = script.GetNumber(); // ID;
	while( script.tok != FINISHED )
	{
		script.GetToken(); // {
		CMapComboBoxData::Category eMapComboBoxCategory = static_cast< CMapComboBoxData::Category >( MCC_MAP_CATEGORY );
		CString strMapComboBoxTitle = _T( "" );
		CString strMapPictureFileName = _T( "" );
		CRect rectRealPosition( 0, 0, 0, 0 );
		CPoint pointNPCPosition( -1, -1 );
		CString strMapMonsterInformationFileName = _T( "" );
		BYTE byLocationID = CONT_NODATA;
		DWORD dwMapComboBoxDataParentID = MCD_NONE;
		int nBlock = 1;
		while( nBlock > 0 && script.tok != FINISHED )
		{
			script.GetToken();
			if( script.Token == _T( "{" ) )
			{
				++nBlock;
			}
			else if( script.Token == _T( "}" ) )
			{
				--nBlock;
			}
			else if( script.Token == _T( "SetCategory" ) )
			{
				script.GetToken(); // (
				eMapComboBoxCategory = static_cast< CMapComboBoxData::Category >( script.GetNumber() );
				script.GetToken(); // )
				script.GetToken(); // ;
			}
			else if( script.Token == _T( "SetTitle" ) )
			{
				script.GetToken(); // (
				strMapComboBoxTitle = prj.GetLangScript( script );
			}
			else if( script.Token == _T( "SetPictureFile" ) )
			{
				script.GetToken(); // (
				strMapPictureFileName = prj.GetLangScript( script );
			}
			else if( script.Token == _T( "SetRealPositionRect" ) )
			{
				script.GetToken(); // (
				rectRealPosition.left = script.GetNumber();
				script.GetToken(); // ,
				rectRealPosition.top = script.GetNumber();
				script.GetToken(); // ,
				rectRealPosition.right = script.GetNumber();
				script.GetToken(); // ,
				rectRealPosition.bottom = script.GetNumber();
				script.GetToken(); // )
				script.GetToken(); // ;
			}
			else if( script.Token == _T( "SetMonsterInformationFile" ) )
			{
				script.GetToken(); // (
				strMapMonsterInformationFileName = prj.GetLangScript( script );
			}
			else if( script.Token == _T( "SetLocationID" ) )
			{
				script.GetToken(); // (
				byLocationID = static_cast< BYTE >( script.GetNumber() );
				script.GetToken(); // )
				script.GetToken(); // ;
			}
			else if( script.Token == _T( "SetNPCPosition" ) )
			{
				script.GetToken(); // (
				pointNPCPosition.x = script.GetNumber();
				script.GetToken(); // ,
				pointNPCPosition.y = script.GetNumber();
				script.GetToken(); // )
				script.GetToken(); // ;
			}
			else if( script.Token == _T( "SetParentID" ) )
			{
				script.GetToken(); // (
				dwMapComboBoxDataParentID = script.GetNumber();
				script.GetToken(); // )
				script.GetToken(); // ;
			}
		}

		CMapComboBoxData* pMapComboBoxData = new CMapComboBoxData( dwMapComboBoxDataID, eMapComboBoxCategory, strMapComboBoxTitle );
		pMapComboBoxData->SetPictureFileName( strMapPictureFileName );
		/*if( strMapPictureFileName != _T( "" ) )
		{
			CTexture* pMapTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( _T( "Theme\\" ), ::GetLanguage(), strMapPictureFileName ), 0xffffffff );
			pMapComboBoxData->SetMapTexture( pMapTexture );
		}*/
		if( eMapComboBoxCategory == MCC_MAP_NAME )
		{
			CRect* pRealPositionRect = new CRect( rectRealPosition );
			m_RealPositionRectMap[ byLocationID ] = pRealPositionRect;
		}
		pMapComboBoxData->SetMapMonsterInformationFileName( strMapMonsterInformationFileName );
		pMapComboBoxData->SetLocationID( byLocationID );
		pMapComboBoxData->SetNPCPosition( pointNPCPosition );
		pMapComboBoxData->SetParentID( dwMapComboBoxDataParentID );

		switch( eMapComboBoxCategory )
		{
		case MCC_MAP_CATEGORY:
			{
				m_MapCategoryVector.push_back( pMapComboBoxData );
				break;
			}
		case MCC_MAP_NAME:
			{
				m_MapNameVector.push_back( pMapComboBoxData );
				break;
			}
		case MCC_NPC_NAME:
			{
				m_NPCNameVector.push_back( pMapComboBoxData );
				break;
			}
		}
		dwMapComboBoxDataID = script.GetNumber(); // ID
	}
	return TRUE;
}
//-----------------------------------------------------------------------------
BOOL CMapInformationManager::LoadMapMonsterInformationPack( void )
{
	for( MapComboBoxDataVector::iterator Iterator = m_MapNameVector.begin(); Iterator != m_MapNameVector.end(); ++Iterator )
	{
		CMapComboBoxData* pMapComboBoxData = ( CMapComboBoxData* )( *Iterator );
		if( pMapComboBoxData == NULL )
		{
			continue;
		}

		if( pMapComboBoxData->GetMapMonsterInformationFileName() == _T( "" ) )
		{
			continue;
		}

		CMapMonsterInformationPack* pMapMonsterInformationPack = new CMapMonsterInformationPack;
		if( pMapMonsterInformationPack->LoadScript( MakePath( DIR_THEME, pMapComboBoxData->GetMapMonsterInformationFileName() ) ) == FALSE )
		{
			continue;
		}

		m_MapMonsterInformationPackMap[ pMapComboBoxData->GetID() ] = pMapMonsterInformationPack;
	}

	return TRUE;
}
//-----------------------------------------------------------------------------
BOOL CMapInformationManager::LoadRainbowNPCInformationPack( void )
{
	m_pRainbowNPCInformationPack = new CRainbowNPCInformationPack;
	m_pRainbowNPCInformationPack->LoadScript( MakePath( DIR_THEME, _T( "texMapRainbow_NPC.inc" ) ) );

	return TRUE;
}
//-----------------------------------------------------------------------------
#endif // __CLIENT
#endif // __IMPROVE_MAP_SYSTEM