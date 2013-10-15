// InstanceDungeonBase.h: interface for the CInstanceDungeonBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INSTANCEDUNGEONBASE_H__5F92E1D4_2A9F_4602_8D26_35E328B9C160__INCLUDED_)
#define AFX_INSTANCEDUNGEONBASE_H__5F92E1D4_2A9F_4602_8D26_35E328B9C160__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if __VER >= 14 // __INSTANCE_DUNGEON
#include "ar.h"
#ifdef __WORLDSERVER
#include "defineText.h"
#endif // __WORLDSERVER

enum { IDTYPE_SOLO=0, IDTYPE_PARTY, IDTYPE_GUILD, IDTYPE_MAX };
enum { ID_NORMAL=0, ID_MIDBOSS, ID_BOSS };
#ifdef __3RD_LEGEND16
const WORD CLASS_NORMAL=0x0001, CLASS_MASTER=0x0002, CLASS_HERO=0x0004, CLASS_LEGEND_HERO=0x0008;
#else // __3RD_LEGEND16
const WORD CLASS_NORMAL=0x0001, CLASS_MASTER=0x0002, CLASS_HERO=0x0004;
#endif // __3RD_LEGEND16
const int COOLTIMECOUNT = 10;

typedef struct INSTACNEDUNGEON_INFO
{
	DWORD	dwWorldId;
	ULONG	uMultiKey;
#ifdef __WORLDSERVER
	int		nState;
	int		nPlayerCount;
	int		nKillCount;
#endif // __WORLDSERVER
	INSTACNEDUNGEON_INFO( DWORD dwWI, ULONG uMK ) : dwWorldId( dwWI ), uMultiKey( uMK )
#ifdef __WORLDSERVER
	,nState( ID_NORMAL ), nPlayerCount( 0 ), nKillCount( 0 )
#endif // __WORLDSERVER
	{}
	void	Serialize( CAr & ar )
	{
		if( ar.IsStoring() )
			ar << dwWorldId << uMultiKey;
		else
			ar >> dwWorldId >> uMultiKey;
	}
} ID_INFO;	

typedef vector<ID_INFO> VEC_IDINFO;
typedef map<DWORD, VEC_IDINFO> MAP_IDBASE;

struct COOLTIME_INFO
{
	DWORD dwWorldId;
	DWORD dwDungeonId;
	DWORD dwCoolTime;
	COOLTIME_INFO() : dwWorldId( NULL_ID), dwDungeonId( NULL_ID ), dwCoolTime( 0 ) {}
	COOLTIME_INFO( DWORD dwWI, DWORD dwDI, DWORD dwCT ) : dwWorldId( dwWI ), dwDungeonId( dwDI ), dwCoolTime( dwCT ) {}
	void	Serialize( CAr & ar )
	{
		if( ar.IsStoring() )
			ar << dwWorldId << dwDungeonId << ( dwCoolTime - GetTickCount() );
		else
		{
			ar >> dwWorldId >> dwDungeonId >> dwCoolTime;
			dwCoolTime += GetTickCount();
		}
	}
};
typedef vector<COOLTIME_INFO> VEC_CTINFO;
typedef map<DWORD, VEC_CTINFO> MAP_CTINFO;

//////////////////////////////////////////////////////////////////////
// CInstanceDungeonBase
//////////////////////////////////////////////////////////////////////
class CInstanceDungeonBase
{
public:
	CInstanceDungeonBase( int nIDType );
	virtual ~CInstanceDungeonBase();
	
	int		GetType()	{ return m_nIDType; }
	void	SerializeAllInfo( CAr & ar );
	
	BOOL	CreateDungeon( ID_INFO ID_Info, DWORD dwDungeonId );
	BOOL	DestroyDungeon( ID_INFO ID_Info, DWORD dwDungeonId );
	void	DestroyAllDungeonByDungeonID( DWORD dwDungeonId );
	void	DestroyAllDungeonByMultiKey( ULONG uMultiKey );		//현재까지는 CoreServer만 사용.

	void	SetDungeonCoolTimeInfo( COOLTIME_INFO CT_Info, DWORD dwPlayerId );
	void	ResetDungeonCoolTimeInfo( DWORD dwWorldId, DWORD dwDungeonId );

private:
	BOOL	CreateDungeonLayer( ID_INFO & ID_Info, DWORD dwLayer );
	BOOL	DestroyDungeonLayer( ID_INFO & ID_Info, DWORD dwLayer );

	VEC_IDINFO* GetDungeonVector( DWORD dwDungeonId );
	ID_INFO*	GetDungeonInfo( DWORD dwDungeonId, DWORD dwWorldId );

	VEC_CTINFO*		GetCoolTimeVector( DWORD dwPlayerId );
	COOLTIME_INFO*	GetCoolTimeInfo( DWORD dwPlayerId, DWORD dwWorldId );
	
	int				m_nIDType;
	MAP_IDBASE		m_mapID;
	MAP_CTINFO		m_mapCTInfo;

#ifdef __CORESERVER
public:
	void	DungeonCoolTimeCountProcess();
	void	UpdateDungeonCoolTimeInfo();

private:
	int		m_nCoolTimeCount;
#endif // __CORESERVER

#ifdef __WORLDSERVER
private:
	struct DUNGEON_DATA
	{
		struct MONSTER
		{
			int nState;
			DWORD dwMonsterId;
			BOOL bRed;
			D3DXVECTOR3 vPos;
		};

		DWORD	dwClass;
		int		nMinLevel;
		int		nMaxLevel;
		DWORD	dwCoolTime;
		map<int, D3DXVECTOR3> mapTeleportPos;
		vector<MONSTER> vecMonster;
		map<int, int>	mapObjCount;
	};
	typedef map<DWORD, DUNGEON_DATA> MAP_IDDATA;
	MAP_IDDATA m_mapDungeonData;

	int			GetState( DWORD dwWorldId, DWORD dwDungeonId );
	void		SetNextState( ID_INFO* pInfo, DWORD dwDungeonId );
	D3DXVECTOR3 GetTeleportPos( DWORD dwWorldId, DWORD dwDungeonId );
	BOOL		CheckClassLevel( CUser* pUser, DWORD dwWorldId );
	void		CreateMonster( DWORD dwDungeonId, DWORD dwWorldId );
	BOOL		IsNowStateMonster( DWORD dwWorldId, DWORD dwDungeonId, DWORD dwMonsterId );
	int			GetObjCount( DWORD dwWorldId, DWORD dwDungeonId );
	DWORD		GetCoolTime( DWORD dwWorldId );
	BOOL		IncreasePlayerCount( DWORD dwDungeonId, DWORD dwWorldId );
	BOOL		DecreasePlayerCount( CUser* pUser, DWORD dwDungeonId, DWORD dwWorldId );

	BYTE		m_nMaxInstanceDungeon;

protected:
	BOOL	TeleportToDungeon( CUser* pUser, DWORD dwWorldId, DWORD dwDungeonId );
	BOOL	LeaveToOutside( CUser* pUser, DWORD dwWorldId, DWORD dwDungeonId );
	void	SetLeaveMarkingPos( CUser* pUser, DWORD dwWorldId, D3DXVECTOR3 vPos );

public:
	void	LoadScript( const char* szFileName );
	virtual BOOL EnteranceDungeon( CUser* pUser, DWORD dwWorldId ) = FALSE;
	virtual BOOL LeaveDungeon( CUser* pUser, DWORD dwWorldId ) = FALSE;

	void	SetInstanceDungeonKill( DWORD dwWorldId, DWORD dwDungeonId, DWORD dwMonsterId );
	void	DeleteDungeonCoolTimeInfo( DWORD dwPlayerId );
#endif // __WORLDSERVER
};


//////////////////////////////////////////////////////////////////////
// CInstanceDungeonHelper
//////////////////////////////////////////////////////////////////////
class CInstanceDungeonHelper
{
public:
	CInstanceDungeonHelper();
	~CInstanceDungeonHelper();
	static CInstanceDungeonHelper* GetInstance();

	void OnCreateDungeon( int nType, ID_INFO ID_Info, DWORD dwDungeonId );
	void OnDestroyDungeon( int nType, ID_INFO ID_Info, DWORD dwDungeonId );
	void OnSetDungeonCoolTimeInfo( ULONG uKey, int nType, COOLTIME_INFO CT_Info, DWORD dwPlayerId );
	void OnResetDungeonCoolTimeInfo( int nType, DWORD dwWorldId, DWORD dwDungeonId );

	void SendInstanceDungeonCreate( int nType, DWORD dwDungeonId, ID_INFO & ID_Info );
	void SendInstanceDungeonDestroy( int nType, DWORD dwDungeonId, ID_INFO & ID_Info );
	void SendInstanceDungeonSetCoolTimeInfo( ULONG uKey, int nType, DWORD dwPlayerId, COOLTIME_INFO & CT_Info );

	void DestroyAllDungeonByMultiKey( ULONG uMultiKey );

private:
	CInstanceDungeonBase* GetDungeonPtr( int nType );

	typedef map<DWORD, int> MAP_WORLD2TYPE;
	MAP_WORLD2TYPE	m_mapDungeonType;

#ifdef __CORESERVER
public:
	void SendInstanceDungeonAllInfo( DWORD dpId );
	void SendInstanceDungeonDeleteCoolTimeInfo( int nType, DWORD dwPlayerId );
#endif // __CORESERVER
#ifdef __WORLDSERVER
public:
	void	OnInstanceDungeonAllInfo( CAr & ar );
	BOOL	IsDungeonType( DWORD dwWorldId, int nType )	{ return GetType( dwWorldId ) == nType; }
	BOOL	IsInstanceDungeon( DWORD dwWorldId )	{ return GetType( dwWorldId ) != NULL_ID; }
	void	SetDungeonType( DWORD dwWorldId, int nType );
	BOOL	EnteranceDungeon( CUser* pUser, DWORD dwWorldId );
	BOOL	LeaveDungeon( CUser* pUser, DWORD dwWorldId );
	void	GoOut( CUser* pUser );
	void	SetInstanceDungeonKill( DWORD dwWorldId, DWORD dwDungeonId, DWORD dwMonsterId );
	void	OnDeleteDungeonCoolTimeInfo( int nType, DWORD dwPlayerId );

	BYTE	m_nIDLayerNum;

private:
	void	SetCertifyKey( ULONG uMultiKey = NULL_ID );
	BOOL	IsCertifying()	{ return m_uCertifyKey != NULL_ID; } 
	int		GetType( DWORD dwWorldId );

	ULONG	m_uCertifyKey;
#endif // __WORLDSERVER
};

#endif // __INSTANCE_DUNGEON

#endif // !defined(AFX_INSTANCEDUNGEONBASE_H__5F92E1D4_2A9F_4602_8D26_35E328B9C160__INCLUDED_)
