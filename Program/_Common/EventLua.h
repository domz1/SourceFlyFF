// EventLua.h: interface for the CEventLua class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EVENTLUA_H__CB7F2A4F_B0F3_47A4_BC55_55047E95D5D2__INCLUDED_)
#define AFX_EVENTLUA_H__CB7F2A4F_B0F3_47A4_BC55_55047E95D5D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if __VER >= 9 // __EVENTLUA

#include "LuaBase.h"

#ifdef __WORLDSERVER
#if __VER >= 12 // __EVENTLUA_0826
class CEventLua;
class CEventLuaProxy
{
public:
	CEventLuaProxy();
	virtual	~CEventLuaProxy();
public:
	vector<BYTE>	GetEventList()	{	return m_vEventLists;	}
	void	SetEventList( const vector<BYTE> & vEventLists )	{	m_vEventLists.assign( vEventLists.begin(), vEventLists.end() );	}
	float	GetExpFactor()	{	return m_fExpFactor;	}
	void	SetExpFactor( float fExpFactor )	{	m_fExpFactor	= fExpFactor;	}
	float	GetItemDropRate()	{	return m_fItemDropRate;	}
	void	SetItemDropRate( float fItemDropRate )	{	m_fItemDropRate	= fItemDropRate;	}
	float	GetPieceItemDropRate()	{	return m_fPieceItemDropRate;	}
	void	SetPieceItemDropRate( float fPieceItemDropRate )	{	m_fPieceItemDropRate	= fPieceItemDropRate;	}
	float	GetGoldDropFactor()	{	return m_fGoldDropFactor;	}
	void	SetGoldDropFactor( float GoldDropFactor )	{	m_fGoldDropFactor	= GoldDropFactor;	}
	int		GetAttackPower()	{	return m_nAttackPower;	}
	void	SetAttackPower( int nAttackPower )	{	m_nAttackPower	= nAttackPower;		}
	int		GetDefensePower()	{	return m_fDefensePower;		}
	void	SetDefensePower( int fDefensePower )	{	m_fDefensePower	= fDefensePower;	}
	DWORD	GetCouponEvent()	{	return m_dwCouponEvent;	}
	void	SetCouponEvent( DWORD dwCouponEvent )	{	m_dwCouponEvent	= dwCouponEvent;	}
#ifdef __EVENTLUA_CHEEREXP
	void	SetCheerExpFactor( float fCheerExpFactor ) { m_fCheerExpFactor = fCheerExpFactor; }
	float	GetCheerExpFactor() { return m_fCheerExpFactor; }
#endif // __EVENTLUA_CHEEREXP
#ifdef __EVENTLUA_KEEPCONNECT
	DWORD	GetKeepConnectTime()	{	return m_dwKeepConnect;	}
	void	SetKeepConnectTime( DWORD dwKeepConnect )	{	m_dwKeepConnect	= dwKeepConnect;	}
#endif // __EVENTLUA_KEEPCONNECT

#ifdef __ENVIRONMENT_EFFECT

	void	SetWeatherEventExpFactor( float fWeatherEventExpFactor )	{ m_fWeatherEventExpFactor = fWeatherEventExpFactor; }
	float	GetWeatherEventExpFactor()	{ return m_fWeatherEventExpFactor; }

#else // __ENVIRONMENT_EFFECT

#ifdef __EVENTLUA_RAIN
	void	SetRainEventExpFactor( float fRainEventExpFactor )	{ m_fRainEventExpFactor = fRainEventExpFactor; }
	float	GetRainEventExpFactor()	{ return m_fRainEventExpFactor; }
#endif // __EVENTLUA_RAIN
#ifdef __EVENTLUA_SNOW
	void	SetSnowEventExpFactor( float fSnowEventExpFactor )	{ m_fSnowEventExpFactor = fSnowEventExpFactor; }
	float	GetSnowEventExpFactor()	{ return m_fSnowEventExpFactor; }
#endif // __EVENTLUA_SNOW

#endif // __ENVIRONMENT_EFFECT

#ifdef __SHOP_COST_RATE
	void	SetShopBuyFactor( float fShopBuyFactor )	{	m_fShopBuyFactor = fShopBuyFactor;	}
	float	GetShopBuyFactor()	{	return m_fShopBuyFactor;	}
	void	SetShopSellFactor( float fShopSellFactor )	{	m_fShopSellFactor = fShopSellFactor;	}
	float	GetShopSellFactor()	{	return m_fShopSellFactor;	}
#endif // __SHOP_COST_RATE
	void	Initialize( CEventLua* pEventLua );
private:
	vector<BYTE>	m_vEventLists;
	float	m_fExpFactor;
	float	m_fItemDropRate;
	float	m_fPieceItemDropRate;
	float	m_fGoldDropFactor;
	int		m_nAttackPower;
	int		m_fDefensePower;
	DWORD	m_dwCouponEvent;
#ifdef __EVENTLUA_CHEEREXP
	float	m_fCheerExpFactor;
#endif // __EVENTLUA_CHEEREXP
#ifdef __EVENTLUA_KEEPCONNECT
	DWORD	m_dwKeepConnect;
#endif // __EVENTLUA_KEEPCONNECT

#ifdef __ENVIRONMENT_EFFECT

	float	m_fWeatherEventExpFactor;

#else // __ENVIRONMENT_EFFECT

#ifdef __EVENTLUA_RAIN
	float	m_fRainEventExpFactor;
#endif // __EVENTLUA_RAIN
#ifdef __EVENTLUA_SNOW
	float	m_fSnowEventExpFactor;
#endif // __EVENTLUA_SNOW

#endif // __ENVIRONMENT_EFFECT

#ifdef __SHOP_COST_RATE
	float	m_fShopBuyFactor;
	float	m_fShopSellFactor;
#endif // __SHOP_COST_RATE
};
#endif	// __EVENTLUA_0826
#endif // __WORLDSERVER

class CEventLua  
{
public:
	CEventLua();
	virtual ~CEventLua();

#ifdef __DBSERVER
#ifdef __AUTO_NOTICE
	BOOL			IsNoticeTime();
	vector<string>	GetNoticeMessage();
#endif // __AUTO_NOTICE
	BOOL			CheckEventState();
	map<BYTE, BOOL>	m_mapState;
	CMclCritSec		m_Access;
#ifdef __VTN_TIMELIMIT
private:
	BOOL			m_bTimeLimit;
public:
	BOOL			IsTimeLimit()	{ return m_bTimeLimit; }
#endif // __VTN_TIMELIMIT
#endif // __DBSERVER

#if defined(__WORLDSERVER) || defined(__DBSERVER)
	vector<BYTE>	GetEventList( BOOL bProxy = TRUE );
	void			LoadScript();
	BOOL			IsPossible() { return m_bRun; }
	BOOL			m_bRun;
	CLuaBase m_Lua;
#endif // __WORLDSERVER, __DBSERVER

#ifdef __WORLDSERVER
	void			SetState( BYTE nId, BOOL bState );
	float			GetExpFactor( BOOL bProxy = TRUE );
	float			GetItemDropRate( BOOL bProxy = TRUE );
	float			GetPieceItemDropRate( BOOL bProxy = TRUE );
	float			GetGoldDropFactor( BOOL bProxy = TRUE );
	int				GetAttackPower( BOOL bProxy = TRUE );
	int				GetDefensePower( BOOL bProxy = TRUE );
#ifdef __EVENTLUA_COUPON
	void			SetCoupon( CUser* pUser, DWORD dwTick );
#endif // __EVENTLUA_COUPON
	string			GetDesc( BYTE nId );	// x
#ifdef __EVENTLUA_GIFT
	void			SetLevelUpGift( CUser* pUser, int nLevel );		// x
#endif // __EVENTLUA_GIFT
	map<DWORD, int>	GetItem( DWORD dwLevel );	// x
	void GetAllEventList( CUser* pUser );	// x
	void GetEventInfo( CUser* pUser, int nId );	// x
#ifdef __EVENTLUA_CHEEREXP
	float GetCheerExpFactor( BOOL bProxy = TRUE );
#endif // __EVENTLUA_CHEEREXP

#ifdef __ENVIRONMENT_EFFECT

	float			GetWeatherEventExpFactor( BOOL bProxy = TRUE );
	string			GetWeatherEventTitle();

#else // __ENVIRONMENT_EFFECT

#ifdef __EVENTLUA_RAIN
	float			GetRainEventExpFactor( BOOL bProxy = TRUE );
	string			GetRainEventTitle();
#endif // __EVENTLUA_RAIN
#ifdef __EVENTLUA_SNOW
	float			GetSnowEventExpFactor( BOOL bProxy = TRUE );
	string			GetSnowEventTitle();
#endif // __EVENTLUA_SNOW

#endif // __ENVIRONMENT_EFFECT

#ifdef __SHOP_COST_RATE
	float			GetShopBuyFactor( BOOL bProxy = TRUE );
	float			GetShopSellFactor( BOOL bProxy = TRUE );
#endif // __SHOP_COST_RATE
#endif // __WORLDSERVER
	
	DWORD	GetCouponEvent( BOOL bProxy = TRUE );

#ifdef __WORLDSERVER
#if __VER >= 12 // __EVENTLUA_0826
	void	PrepareProxy()	{	m_proxy.Initialize( this );	}
private:
	CEventLuaProxy	m_proxy;
#endif	// __EVENTLUA_0826

#ifdef __EVENTLUA_SPAWN
private:
	struct __SPAWNINFO
	{
		int		nType;
		DWORD	dwId, dwInterval, dwTimeOut;
		__SPAWNINFO( int nT, DWORD dwI, DWORD dwIT ) : nType( nT ), dwId( dwI ), dwInterval( dwIT ), dwTimeOut( 0 ) {}
		BOOL	IsTimeOut()
		{
			DWORD dwCurrentTime = timeGetTime();
			if( dwTimeOut > dwCurrentTime )
				return FALSE;
			dwTimeOut = dwCurrentTime + dwInterval;
			return TRUE;
		}
	};
	typedef vector<__SPAWNINFO> VECSPAWNINFO;

	struct __SPAWNREGION
	{
		DWORD	dwWorldId;		// 월드 번호
		CRect	rect;	// 참조 영역
		__SPAWNREGION( DWORD dwWI, CRect & r ) : dwWorldId( dwWI ), rect( r ) {}
		D3DXVECTOR3 GetPos() { return D3DXVECTOR3( (float)( xRandom( rect.left, rect.right ) ), 0.0f, (float)( xRandom( rect.top, rect.bottom ) ) ); }
	};

	vector<__SPAWNREGION>	m_vecSpawnRegion;		// spawn 이벤트 진행시 아이템 및 몬스터의 생성위치(월드 및 좌표) 정보

	map<BYTE, VECSPAWNINFO>	m_mapSpawnList;			// spawn 이벤트 진행시 생성될 아이템 및 몬스터 정보
	map<DWORD, BYTE>		m_mapMonsterId;			// spawn 이벤트 진행시 생성된 몬스터가 이벤트용 인지 검사하기 위해 필요
	map<OBJID, BYTE>		m_mapSpawnedMonster;	// spawn 이벤트 진행시 실제로 생성된 몬스터들의 목록 저장(이벤트 종료시 삭제하기 위해 필요)
	vector<BYTE>			m_vecEndSpawnEvent;			// 종료된 이벤트 ID 목록( 남아있는 spawn 몬스터가 있으면 다 삭제한 후 ID를 제거한다

	void	SetSpawnEvent( BYTE nId, BOOL bState );
	__SPAWNREGION GetSpawnRegion() { return m_vecSpawnRegion[xRandom( m_vecSpawnRegion.size() )]; }
public:
	void	SetAddSpawnRegion( DWORD dwWorldId, CRect & rect ) { m_vecSpawnRegion.push_back( __SPAWNREGION( dwWorldId, rect ) ); }
	BOOL	IsEventSpawnMonster( DWORD dwMonsterId ) { return m_mapMonsterId.find( dwMonsterId ) != m_mapMonsterId.end(); }
	void	EventSpawnProcess();
	void	DeleteEndEventProcess();
	void	RemoveSpawnedMonster( OBJID objId ) { m_mapSpawnedMonster.erase( objId ); }
#endif // __EVENTLUA_SPAWN

#ifdef __EVENTLUA_KEEPCONNECT
private:
	vector<u_long>	m_vecKeepConnectUser;
	map<DWORD, int>	m_mapItemList;
public:
	void			SetKeepConnectEvent( CUser* pUser, DWORD dwTick );
	DWORD			GetKeepConnectTime( BOOL bProxy = TRUE );
	void			GetKeepConnectItem();
	void			KeepConnectEventProcess();
#endif // __EVENTLUA_KEEPCONNECT

#endif // __WORLDSERVER
};
#endif // __EVENTLUA

#endif // !defined(AFX_EVENTLUA_H__CB7F2A4F_B0F3_47A4_BC55_55047E95D5D2__INCLUDED_)
