#ifndef __MAP_MONSTER_ICON_INFORMATION_PACK_H__
#define __MAP_MONSTER_ICON_INFORMATION_PACK_H__

#include "MapMonsterInformation.h"

#ifdef __IMPROVE_MAP_SYSTEM
#ifdef __CLIENT
class CMapMonsterInformationPack : public CTexturePack
{
public:
	CMapMonsterInformationPack( void );
	~CMapMonsterInformationPack( void );

public:
	BOOL LoadScript( const CString& strFileName );

public:
	const CMapMonsterInformation* GetMapMonsterInformation( int nIndex ) const;

private:
	void DeleteAllMapMonsterInformation( void );

private:
	MapMonsterInformationVector m_MapMonsterInformationVector;
};
typedef map< DWORD, CMapMonsterInformationPack* > MapMonsterInformationPackMap;
#endif // __CLIENT
#endif // __IMPROVE_MAP_SYSTEM

#endif // __MAP_MONSTER_ICON_INFORMATION_H__