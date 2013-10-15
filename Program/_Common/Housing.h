#pragma once

#if __VER >= 13 // __HOUSING

#include "Object3D.h"

/////////////////////////////////////////////////////////////////////
//// CHousing ///////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
struct HOUSINGINFO
{
	DWORD		dwItemId;		// 아이템 ID
	time_t		tKeepTime;		// 기간(지속시간)
	BOOL		bSetup;			// 설치여부
	D3DXVECTOR3	vPos;			// 설치위치
	float		fAngle;			// 설치각도
#ifdef __WORLDSERVER
	OBJID		objId;			// 설치된 가구의 컨트롤ID(제거시 필요)
#endif // __WORLDSERVER
	HOUSINGINFO() : dwItemId(NULL_ID), tKeepTime(0), vPos(0.0f,0.0f,0.0f), bSetup(FALSE), fAngle(0.0f)
#ifdef __WORLDSERVER
	, objId( NULL_ID )
#endif // __WORLDSERVER
	{} // 초기화
#ifdef __DBSERVER
	HOUSINGINFO( DWORD dwII ) : dwItemId(dwII), tKeepTime(0), vPos(0.0f,0.0f,0.0f), bSetup(FALSE), fAngle(0.0f)
	{
		ItemProp* pItemProp = prj.GetItemProp( dwItemId );   
		if( pItemProp )
		{
			CTime time	= CTime::GetCurrentTime() + CTimeSpan( 0, 0, pItemProp->dwAbilityMin, 0 );
			tKeepTime	= (time_t)( time.GetTime() );
		}
	}
#endif // __DBSERVER
	void Serialize( CAr & ar )
	{
		if( ar.IsStoring() )
			ar << dwItemId << static_cast<time_t>(tKeepTime - time_null()) << bSetup << vPos << fAngle;
		else
		{
			ar >> dwItemId >> tKeepTime;
			tKeepTime += time_null();
			ar >> bSetup >> vPos >> fAngle;
		}
	}
};

class CHousing
{
public:
	CHousing( DWORD dwPlayerId );
	~CHousing(void);

#ifdef __CLIENT
	vector<DWORD>	m_vecVisitable;
	static CHousing* GetInstance();
	void	GetHousingList( vector<HOUSINGINFO> & vHousingList );	// 가구 목록 정보를 복사해서 넘김.
	void	GetVisitAllow( vector<DWORD> & vVisitAllow );		// 방문허용자 목록을 넘김
#endif // __CLIENT
		
	void Serialize( CAr & ar );
	void SetFurnitureList( HOUSINGINFO& housingInfo, BOOL bAdd );
	void SetupFurniture( HOUSINGINFO housingInfo );
	void SetVisitAllow( DWORD dwTargetId, BOOL bAllow );

#ifdef __WORLDSERVER
	BOOL IsListUpAble( CUser* pUser, DWORD dwItemId );	// 가구 목록 추가가 가능한가?
	BOOL IsSetupAble( CUser* pUser, HOUSINGINFO housingInfo );		// 가구 설치가 가능한가?
	BOOL IsAllowVisit( DWORD dwPlayerId );	// 내가 허용한 방문자 인가?
	BOOL AddFurnitureControl( int nIndex );		// 가구 컨트롤을 생성한다.
	void RemoveFurnitureControl( int nIndex );	// 가구 컨트롤을 제거한다.
	void AddAllFurnitureControl();				// 설치된 모든 가구 컨트롤을 생성한다.
	void SetBuff( DWORD dwItemId, BOOL bSet );	// 가구 설치나 제거시 버프설정..
	vector<DWORD> GetAllPaperingInfo();			// 벽지 및 장판 정보..
	
	void Setting( BOOL bSetting ) { m_bSetting = bSetting; }
	BOOL m_bSetting;			// 지금 뭔가 세팅중인가?(TransServer와 타이밍 문제로..)
#endif // __WORLDSERVER

#ifdef __DBSERVER
	void ProcessExpiredFurniture( time_t time, BOOL bGMRevmoe = FALSE );
#endif // __DBSERVER

private:
	BOOL	RemoveFurnitureList( DWORD dwItemId );
	int		GetIndexFromList( DWORD dwItemId );

	DWORD	m_dwMasterId;
	vector<HOUSINGINFO> m_vecHousingInfo;		// 가구 목록
	vector<DWORD>		m_vecIdVisitAllow;		// 내가 방문 허용한 목록
};

#ifndef __CLIENT	// __WORLDSERVER, __DBSERVER
/////////////////////////////////////////////////////////////////////
//// CHousingMng ////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
typedef map<DWORD, CHousing*> MAP_HP;
typedef map< DWORD, vector<DWORD> > MAP_VSTABLE;
class CHousingMng
{
public:
	CHousingMng(void);
	~CHousingMng(void);
	void	Clear();
	static CHousingMng* GetInstance();
#ifdef __WORLDSERVER
	void ReqLoadHousingInfo( DWORD dwPlayerId );
	BOOL ReqSetFurnitureList( CUser* pUser, DWORD dwItemId );	// Trans에 가구 목록 추가 요청
	BOOL ReqSetupFurniture( CUser* pUser, HOUSINGINFO housingInfo );	// Trans에 가구 설치 및 제거 요청
	BOOL ReqSetAllowVisit( CUser* pUser, DWORD dwPlayerId, BOOL bAllow ); // Trans에 허용 목록 추가 요청
	void ReqGMFunrnitureListAll( CUser* pUser );	// GM 가구 목록 전체 삭제..

	void CreateRoomLayer( DWORD dwPlayerId );	// 접속시 방 레이어를 생성하고 설치된 가구 컨트롤을 생성한다.
	BOOL DestroyHousing( DWORD dwPlayerId );	// 로그 아웃시 하우징 정보 제거

private:
	MAP_VSTABLE	m_mapVisitable;		// 방문 가능 목록
public:
	void SetAddVisitable( DWORD dwPlayerId, DWORD dwTargetId );		// 방문 가능 목록 추가
	void SetRemoveVisitable( DWORD dwPlayerId, DWORD dwTargetId );	// 방문 가능 목록 제거
	void OnReqVisitableList( CUser* pUser );	// 클라이언트가 방문 가능 목록을 요청했다.
	void SetVisitRoom( CUser* pUser, DWORD dwPlayerId ); // 방문...
	void GoOut( CUser* pUser );		// 방에서 나감.
#endif // __WORLDSERVER
	
	CHousing*	CreateHousing( DWORD dwPlayerId );	// 접속시 하우징 정보 생성(Trans는 최초 접속시)
	CHousing*	GetHousing( DWORD dwPlayerId );	// 해당 ID의 하우징 정보 Get
	
	void		SetFurnitureList( DWORD dwPlayerId, HOUSINGINFO& housingInfo, BOOL bAdd );	// 가구 목록 추가 및 제거
	void		SetupFurniture( DWORD dwPlayerId, HOUSINGINFO housingInfo );	// 가구 설치 및 제거
	void		SetVisitAllow( DWORD dwPlayerId, DWORD dwTargetId, BOOL bAllow );	// 허용 목록 추가

#ifdef __DBSERVER
	void		ProcessRemoveExpiredFurniture();	// 만료시간 검사
#endif // __DBSERVER

private:
	MAP_HP		m_mapHousing;
};

#endif // n__CLIENT

#ifdef __CLIENT

#define RED_MODE		1
#define NORMAL_MODE		2

class CDeployManager
{

private:
	HOUSINGINFO			m_ItemInfo;

	LPDIRECT3DTEXTURE9	m_pOriginWall;				// 벽 기본텍스쳐
	LPDIRECT3DTEXTURE9	m_pOriginTile;				// 바닥 기본텍스쳐
	vector<LPDIRECT3DTEXTURE9>	m_vecOriginal;		// 타겟오브젝트 텍스쳐셋
	LPDIRECT3DTEXTURE9	m_pRed;
	BOOL				m_bIsColl;
	BOOL				m_bIsMyRoom;				// 가는방이 내방인가?
	int					m_nBlendFactor;
	int					m_nNumTex;
	
public:
	CObj*				m_pTargetObj;
	CObj*				m_pWallObj;
	float				m_fAngle;

	CDeployManager(void);
	~CDeployManager(void);

	static	CDeployManager* GetInstance();
	BOOL	LoadToDeploy(int nItemId, HOUSINGINFO housingInfo);
	BOOL	ChangeObjMode(int nType);
	HOUSINGINFO*	EndDeploy();
	BOOL	IsReady();
	BOOL	IsCollide();
	void	Init();
	void	Process();
	BOOL	CheckCollision();
	BOOL	ChangeWallTex(TCHAR* pTexName = NULL);
	BOOL	ChangeTileTex(TCHAR* pTexName = NULL);
	BOOL	IsMyRoom();
	BOOL	SetMyRoom(BOOL bFlag);
};

#endif // __CLIENT
#endif // __VER >= 13

#if __VER >= 15 // __GUILD_HOUSE

#ifdef __CLIENT

struct HOUSING_ITEM;

class CGuildDeployManager 
{
	// 길드 하우스 가구 편집기 
public:
	CGuildDeployManager( );
	virtual ~CGuildDeployManager( );

	static CGuildDeployManager* GetInstance( );

	BOOL	LoadToDeploy(int nItemId, const HOUSING_ITEM& housingInfo);
	BOOL	ChangeObjMode(int nType);
	HOUSING_ITEM*	EndDeploy();

	BOOL	IsReady();
	void	Init();
	void	Process();
	BOOL	CheckCollision();
	BOOL	ChangeWallTex(TCHAR* pTexName = NULL); 
	BOOL	ChangeTileTex(TCHAR* pTexName = NULL);
	BOOL	IsManagementPower( )						{ return m_bIsManagement; }	// 가구 제어 권한이 있는가?
	void	SetManagementPower( BOOL bMng )				{ m_bIsManagement = bMng; }
	BOOL	IsCollide()									{ return m_bIsColl; }

	BOOL	MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

	std::string GetNameHouseObj( );
	std::string GetNameHouseWallTex( );
	std::string GetNameHouseTileTex( );
	
protected:
	HOUSING_ITEM* m_pItem;

	LPDIRECT3DTEXTURE9	m_pOriginWall;				// 벽 기본텍스쳐
	LPDIRECT3DTEXTURE9	m_pOriginTile;				// 바닥 기본텍스쳐
	vector<LPDIRECT3DTEXTURE9>	m_vecOriginal;		// 타겟오브젝트 텍스쳐셋
	LPDIRECT3DTEXTURE9	m_pRed;
	BOOL				m_bIsColl;
	BOOL				m_bIsManagement;				// 가구설정 권한이 있는가?
	int					m_nBlendFactor;
	int					m_nNumTex;

	CObj*				m_pTargetObj;
	CObj*				m_pWallObj;
	float				m_fAngle;

	CModelObject*		m_pClonedModel;

};

inline CGuildDeployManager* GuildDeploy( ) { return CGuildDeployManager::GetInstance( ); }

#endif //__CLIENT

#endif	//__GUILD_HOUSE

