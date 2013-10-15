#ifndef __CREATEMONSTER_H__
#define	__CREATEMONSTER_H__

#if __VER >= 12 // __NEW_ITEMCREATEMON_SERVER

struct CREATE_MONSTER_PROP
{
	DWORD dwKeepTime;
	map<DWORD, int> mapMonster;
	
	CREATE_MONSTER_PROP() :	dwKeepTime(0) {}
		
	DWORD GetRandomMonsterId()
	{
		int nRandom = xRandom( 100 );
		for( map<DWORD, int>::iterator it=mapMonster.begin(); it!=mapMonster.end(); it++ )
		{
			if( nRandom < it->second )
				break;
			nRandom -= it->second;
		}

		return it!=mapMonster.end() ? it->first : NULL_ID;
	}
};

struct CREATE_MONSTER_INFO
{
	DWORD dwOwnerId;
	DWORD dwEndTick;
	char  chState;
	
	CREATE_MONSTER_INFO(): dwOwnerId(NULL_ID), dwEndTick(0), chState( 'N' ) {}
};

typedef map<DWORD, CREATE_MONSTER_PROP*> MAPPROP;
typedef map<DWORD, CREATE_MONSTER_INFO> MAPINFO;

class CCreateMonster
{
public:
	//	Constructions
	CCreateMonster();
	~CCreateMonster();

	static CCreateMonster* GetInstance( void );
	void LoadScript();
	void SetState( OBJID objId, const char chState );
	void ProcessRemoveMonster();
	BOOL IsAttackAble( CUser* pUser, CMover* pTarget, BOOL bTextOut = FALSE );
	void CreateMonster( CUser* pUser, DWORD dwObjId, D3DXVECTOR3 vPos );
	void RemoveInfo( CMover* pMover );
	

private:
	CREATE_MONSTER_PROP* GetCreateMonsterProp( CItemElem* pItemElem );

	int m_nMaxCreateNum;
	MAPPROP m_mapCreateMonsterProp;
	MAPINFO m_mapCreateMonsterInfo;

};
#endif // __NEW_ITEMCREATEMON_SERVER



///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
#if __VER < 12 // __NEW_ITEMCREATEMON_SERVER
class CCreateMonster
{
	struct __MONSTERPERSENT {
		DWORD dwMonsterId;
		short nPersent;
	};
public:
	//	Constructions
	CCreateMonster();
	~CCreateMonster();
	DWORD GetCreateMonster( DWORD dwItemId );
	map< DWORD, vector< __MONSTERPERSENT > > pMonsterItem;
#ifdef __WORLDSERVER
	BOOL LoadScript( LPCSTR lpszFileName );
#endif // __WORLDSERVER
};

#endif // __NEW_ITEMCREATEMON_SERVER

#endif	//	__CREATEMONSTER_H__
