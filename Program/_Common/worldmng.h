#ifndef __WORLDMNG_H__
#define __WORLDMNG_H__

#pragma once

typedef	REGIONELEM	RegionElem, *PRegionElem;

typedef struct tagWORLD
{
	TCHAR	m_szFileName[128];
	TCHAR	m_szWorldName[128];
	DWORD	m_dwId;
	BOOL	IsValid()	{	return( m_szFileName[0] != '\0' );	}
}
WORLD,* LPWORLD;

#define MAX_WORLD	256

class CWorld;

#ifdef __WORLDSERVER
class CWorldMng
#else	// __WORLDSERVER
#ifdef __STL_0402
class CWorldMng : public map<string, CWorld*>
#else	// __STL_0402
class CWorldMng : public CMyMap2<CWorld*>
#endif	// __STL_0402
#endif	// __WORLDSERVER
{

#ifdef __WORLDSERVER	//	server	--------------------------------------------
private:
	CWorld*		m_pFirstActive;
public:
	LONG		m_lCount;		// ¿ùµåÀÇ °¹¼ö 

	void		Free();
	CWorld*		GetWorld( DWORD dwWorldID );
	CWorld*		GetFirstActive()	{	return m_pFirstActive;	}
	void		Add( CJurisdiction* pJurisdiction );
	void		ReadObject();
	void		Process();
#if __VER >= 15 // __GUILD_HOUSE
	BOOL		HasNobody_Replace( DWORD dwWorldId, int nLayer );
#endif // __GUILD_HOUSE
#ifdef __LAYER_1015
	BOOL		AddObj( CObj* pObj, DWORD dwWorldID, BOOL bAddItToGlobalId, int nLayer );
#else	// __LAYER_1015
	BOOL		AddObj( CObj* pObj, DWORD dwWorldID, BOOL bAddItToGlobalId = FALSE );
#endif	// __LAYER_1015
	BOOL		PreremoveObj( OBJID objid );
	CObj*		PregetObj( OBJID objid );
	u_long		Respawn();
	DWORD		GetObjCount();

	CRegionElemArray	m_aRevivalPos;
	CRegionElemArray	m_aRevivalRgn;	
#else	// __WORLDSERVER	//	not server	------------------------------------

private:
	CWorld*		m_pWorld;
public:
//	Constructions
//	Operations
	void	Free();
	CWorld*		operator () () {	return	m_pWorld; }	
	CWorld*		Get()	{	return	m_pWorld;	}
	CWorld*		Open( LPDIRECT3DDEVICE9 pd3dDevice, LPCSTR lpszWorld );
	CWorld*		Open( LPDIRECT3DDEVICE9 pd3dDevice, OBJID idWorld );
	void DestroyCurrentWorld();
#endif	// __WOLDSERVER	--------------------------------------------------------

//	common	--------------------------------------------------------------------
private:
	int		m_nSize;
	void	LoadRevivalPos( DWORD dwWorldId, LPCTSTR lpszWorld );		// as loading script, it is called

#ifdef __WORLDSERVER
public:
	PRegionElem		GetRevivalPosChao( DWORD dwWorldId, LPCTSTR sKey );
	PRegionElem		GetNearRevivalPosChao( DWORD dwWorldId, D3DXVECTOR3 & vPos );
	PRegionElem		GetRevivalPos( DWORD dwWorldId, LPCTSTR sKey );
	PRegionElem		GetNearRevivalPos( DWORD dwWorldId, D3DXVECTOR3 & vPos );
#endif	// __WORLDSERVER
public:

	CFixedArray< WORLD > m_aWorld;
//	Constructions
	CWorldMng();
	virtual	~CWorldMng();
//	Operations
	LPWORLD	GetWorldStruct( OBJID idWorld )	{ return (LPWORLD)m_aWorld.GetAt( idWorld ); }
	BOOL	LoadScript( LPCTSTR lpszFileName );
	void LoadAllMoverDialog();

#ifdef __MAP_SECURITY
	void	AddMapCheckInfo( const char* szFileName );
private:
	BOOL	SetMapCheck( const char* szFileName );
#ifdef __CLIENT
	vector<string> m_vecstrFileName;
public:
	void	CheckMapKey();
#endif // __CLIENT

#ifdef __WORLDSERVER
	map<string, string>	m_mapMapKey;
public:
	void	CheckMapKey( CUser* pUser, const char* szFileName, const char* szMapKey );
#endif // __WORLDSERVER

#endif // __MAP_SECURITY
/*--------------------------------------------------------------------------------*/
};

#endif	// __WORLDMNG_H__