#ifndef __MAP_INFORMATION_MANAGER_H__
#define __MAP_INFORMATION_MANAGER_H__

#ifdef __IMPROVE_MAP_SYSTEM
#ifdef __CLIENT
#include "MapComboBoxData.h"
#include "MapMonsterInformationPack.h"
#include "RainbowNPCInformationPack.h"
#include "UserMarkPositionInfo.h"

class CMapInformationManager
{
public:
	CMapInformationManager( void );
	~CMapInformationManager( void );

public:
	BOOL LoadMapInformationData( void );
	void DeleteAllMapInformation( void );

public:
	CTexture* const GetPCArrowTexture( void );
	CTexture* const GetPartyPCArrowTexture( void );
	CTexture* const GetTeleportationPositionTexture( void );
	CTexture* const GetDestinationPositionTexture( void );
	CTexture* const GetNPCPositionTexture( void );
	CTexture* const GetUserMarkPositionTexture( void );
	MapComboBoxDataVector* GetMapCategoryVector( void );
	MapComboBoxDataVector* GetMapNameVector( void );
	MapComboBoxDataVector* GetNPCNameVector( void );
	CMapMonsterInformationPack* FindMapMonsterInformationPack( DWORD dwFirstKey );
	CRect& FindRealPosition( BYTE byFirstKey );
	CRainbowNPCInformationPack* GetRainbowNPCInformationPack( void );
	BOOL InsertUserMarkPositionInfo( const CString& strName, FLOAT fPositionX, FLOAT fPositionY );
	void DeleteUserMarkPositionInfo( DWORD dwID );
	CUserMarkPositionInfo* const FindUserMarkPositionInfo( DWORD dwID );
	void DeleteAllUserMarkPositionInfo( void );
	UserMarkPositionInfoList& GetUserMarkPositionInfoList( void );

private:
	BOOL LoadMapIconTexture( void );
	BOOL LoadPropMapComboBoxData( void );
	BOOL LoadMapMonsterInformationPack( void );
	BOOL LoadRainbowNPCInformationPack( void );

private:
	enum { USER_MARK_MAX_SIZE = 20 };

private:
	CTexture* m_pPCArrowTexture;
	CTexture* m_pPartyPCArrowTexture;
	CTexture* m_pTeleportationPositionTexture;
	CTexture* m_pDestinationPositionTexture;
	CTexture* m_pNPCPositionTexture;
	CTexture* m_pUserMarkPositionTexture;
	MapComboBoxDataVector m_MapCategoryVector;
	MapComboBoxDataVector m_MapNameVector;
	MapComboBoxDataVector m_NPCNameVector;
	MapMonsterInformationPackMap m_MapMonsterInformationPackMap;
	map< BYTE, CRect* > m_RealPositionRectMap;
	CRainbowNPCInformationPack* m_pRainbowNPCInformationPack;
	UserMarkPositionInfoList m_UserMarkPositionInfoList;
};
#endif // __CLIENT
#endif // __IMPROVE_MAP_SYSTEM

#endif // __MAP_INFORMATION_MANAGER_H__
