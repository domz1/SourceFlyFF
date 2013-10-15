#ifndef __WORLD_2002_1_22
#define __WORLD_2002_1_22

#ifdef __LAYER_1015
#define	ADDOBJ( pObj, bAddItToGlobalId, nLayer )	AddObj( (pObj), (bAddItToGlobalId), (nLayer) )
#else	// __LAYER_1015
#define	ADDOBJ( pObj, bAddItToGlobalId, nLayer )	AddObj( (pObj), (bAddItToGlobalId) )
#endif	// __LAYER_1015

#ifdef __WORLDSERVER
	#include "Respawn.h"
	#include "ServerDesc.h"
	#ifdef __LAYER_1015
		#include "..\WorldServer\layeredlinkmap.h"
	#else	// __LAYER_1015
		#include "..\WorldServer\LinkMap.h"
	#endif	// __LAYER_1015
#endif

////////////////////////////////////////////////////////////////////////////////////////////
// define 
///////////////////////////////////////////////////////////////////////////////////////////

#define	MAX_ADDOBJS				20480
#define	MAX_DYNAMICOBJ			81920
//#define	MAX_BKGND				20480
#define	MAX_MODIFYLINK			4096
#define	MAX_REPLACEOBJ			1024
#define	CLOSEWORKER				(DWORD)-1
#define D3DFVF_BOUNDBOXVERTEX	(D3DFVF_XYZ|D3DFVF_DIFFUSE) 
#define MAX_DISPLAYOBJ			5000
#define MAX_DISPLAYSFX			500
//#define MINIMAP_SIZE			256
#define MINIMAP_SIZE			200
#define WLD_EXTRA_WIDTH			10.0f
#define	MAX_DELETEOBJS			4096
#define MAX_MOVERSELECT			5
#define	MAX_ON_DIE				1024

////////////////////////////////////////////////////////////////////////////////////////////
// extern 
///////////////////////////////////////////////////////////////////////////////////////////
extern LPDIRECT3DINDEXBUFFER9	g_pIB;
extern LPDIRECT3DINDEXBUFFER9	g_pExIB;
extern LPDIRECT3DINDEXBUFFER9	g_pGridIB;
extern WORD						g_anPrimitive[3];
extern WORD						g_anPrimitiveA[3];
extern WORD						g_anStartIndex[4];

////////////////////////////////////////////////////////////////////////////////////////////
// struct 선언 
///////////////////////////////////////////////////////////////////////////////////////////

struct BOUNDBOXVERTEX 
{ 
	D3DXVECTOR3 p; 
	DWORD		color; 
};

typedef struct tagLANDATTR
{
	DWORD dwColor;
     
	DWORD dwGenRegionAttr;
	DWORD dwPayRegionAttr;
	DWORD dwMusic;
	DWORD dwRepeat;
	DWORD dwCityName;
	DWORD dwLandName;
	DWORD dwPayID;
	CHAR  dwKey[ 64 ];
} LANDATTR,* LPLANDATTR; 

typedef struct tagLIGHTCOLOR 
{
	FLOAT r1,g1,b1,r2,g2,b2; 
} LIGHTCOLOR,* LPLIGHTCOLOR;

#ifdef __WORLDSERVER
typedef	struct	tagON_DIE
{
	CUser*	pDie;
	CUser*	pAttacker;
}	ON_DIE ;
#endif	// __WORLDSERVER

typedef struct tagREPLACEOBJ
{
	CMover*		pObj;
	DWORD		dwWorldID;
	D3DXVECTOR3	vPos;
//	DWORD		dpid;
	u_long		uIdofMulti;
#ifdef __LAYER_1015
	int		nLayer;
#endif	// __LAYER_1015
} REPLACEOBJ, *LPREPLACEOBJ; 


#if __VER >= 15 // __BS_CHANGING_ENVIR
typedef vector< D3DXVECTOR3 >		Vec3D_Container;
typedef Vec3D_Container::iterator	Vec3D_Iter;

struct TextureSet
{
	TextureSet( ) { _bUse = FALSE; _pTexture = NULL; }
	~TextureSet( ) { _pTexture = NULL; }

	BOOL _bUse;
	string _strFileName;
	IDirect3DTexture9* _pTexture;
};

struct ENVIR_INFO
{
	// 대륙 환경 정보, 월드당 여러개의 대륙이 존재할수있음.
	ENVIR_INFO( )
	{
		_iWeather = 0;
		_bUseEnvir = FALSE;
		_bUseRealData = FALSE;
#ifdef _DEBUG
		_VBforLine = NULL;
#endif 
	}

	int		_id;
	string _strName;
	BOOL	_bUseEnvir;
	BOOL	_bUseRealData;
	float _fAmbient[ 3 ];
	float _fDiffuse[ 3 ];
	float _fFogStart;
	float _fFogEnd;
	int	  _iWeather;

	TextureSet _kSky;
	TextureSet _kCloud;
	TextureSet _kSun;
	TextureSet _kMoon;

	Vec3D_Container _cVertices;

		
#ifdef _DEBUG
	IDirect3DVertexBuffer9* _VBforLine;
	~ENVIR_INFO( ) { SAFE_RELEASE( _VBforLine ); }
#endif

};

typedef vector< ENVIR_INFO >				ENVIR_INFO_Container;
typedef ENVIR_INFO_Container::iterator		ENVIR_INFO_Iter;

struct WORLD_ENVIR_INFO
{
	TextureSet _kSky[ 3 ];		// 0 : night, 1 : day, 2 : evening
	TextureSet _kCloud[ 3 ];
	TextureSet _kSun;
	TextureSet _kMoon;
};

#endif


////////////////////////////////////////////////////////////////////////////////////////////
// class foward선언 
////////////////////////////////////////////////////////////////////////////////////////////
class CObj;
class CAnim;
class CItem;
class CSfx;
class CCtrl;
class CMover;
class CLandscape;


class CWorld
{
	friend CLandscape;
	friend CObj;
	friend CMover;

public:
	CWorld();
	~CWorld();

public:
	DWORD			m_dwWorldID;
	int				m_nLandWidth;
	int				m_nLandHeight;
	int				WORLD_WIDTH;	 //		( MAP_SIZE * m_nLandWidth )
	int				WORLD_HEIGHT;	 //		( MAP_SIZE * m_nLandHeight )

	int				m_iMPU;			//gmpbigsun : MPU of the world

///////////////////////////////////////////////////////////////////////////////
#ifdef __WORLDSERVER
///////////////////////////////////////////////////////////////////////////////
private:
	int		m_cbRunnableObject;
public:
	int		GetRunnableObjectCount()	{	return m_cbRunnableObject;	}
	float*			m_apHeightMap;	
	LPWATERHEIGHT	m_apWaterHeight;		

#ifdef __LAYER_1015
	CLayeredLinkMap	m_linkMap;
#else	// __LAYER_1015
	CLinkMap		m_linkMap;
#endif	// __LAYER_1015

	u_long			m_dwObjNum;
	CCtrl*			m_apObject[MAX_DYNAMICOBJ];	// dynamic 객체를 담는다.		// 312k
	vector< CObj* > m_vecBackground;			// static 객체를 담는다.

//	CRIT_SEC		m_csModifyLink;
	int				m_cbModifyLink;
	CObj*			m_apModifyLink[MAX_MODIFYLINK];		// 16k

//	CRIT_SEC		m_csReplaceObj;
	int				m_cbReplaceObj;
	REPLACEOBJ		m_aReplaceObj[MAX_REPLACEOBJ];		// 4k

//	CRIT_SEC		m_AddRemoveLock;
	int				m_cbAddObjs;
	CObj*			m_apAddObjs[MAX_ADDOBJS];				// 80k
	BOOL			m_bAddItToGlobalId[MAX_ADDOBJS];		// 80k

	TCHAR			m_lpszWorld[64];
	CDWordStack		m_ObjStack;
	u_long			m_cbUser;
	CWorld*			nextptr;	// next world ptr
#ifdef __LAYER_1021
	CLayerdRespawner	m_respawner;
#else	// __LAYER_1021
	CRespawner		m_respawner;		// 5.84m
#endif	// __LAYER_1021
	BOOL			m_bLoadScriptFlag;
	int				m_cbOnDie;
	ON_DIE			m_aOnDie[MAX_ON_DIE];	// 8k

public:
	void			Process();
	void			ModifyView( CCtrl* pCtrl );
	BOOL			ReadWorld( const CRect & rcLandscapce ); 
	BOOL			PreremoveObj( OBJID objid );
	CObj*			PregetObj( OBJID objid );
	u_long			Respawn()	{	return m_respawner.Spawn( this );	}
	DWORD			GetObjCount() { return m_dwObjNum; }
	void			OnDie( CMover* pDie, CMover* pAttacker );
	void			_OnDie( void );
	CMover*			FindMover( LPCTSTR szName );
private:
	int				IsOverlapped( int c, int p, int r, int w );
	void			AddItToView( CCtrl* pCtrl );
	void			_add();
	void			_process();
	void			_modifylink();
	void			_replace();
	void			_delete();

///////////////////////////////////////////////////////////////////////////////
#else // end __WORLDSERVER
///////////////////////////////////////////////////////////////////////////////

public:
	static D3DLIGHT9	m_light;
	static D3DLIGHT9	m_lightFogSky;
	static CObj*		m_aobjCull[ MAX_DISPLAYOBJ ];
	static CObj*		m_asfxCull[ MAX_DISPLAYSFX ];
	static int			m_nObjCullSize;
	static int			m_nSfxCullSize;
	static CWeather		m_weather;
	static CCamera*		m_pCamera;
	static D3DXMATRIX	m_matProj;
	static D3DCOLOR		m_dwBgColor;
	static FLOAT		m_fFarPlane;
	static FLOAT		m_fNearPlane;
	static BOOL			m_bZoomView;
	static int			m_nZoomLevel;
	static CSkyBox		m_skyBox;
	static CMover*		m_amvrSelect[ MAX_MOVERSELECT ];
	CLandscape**		m_apLand;
	FLOAT				m_fElapsedTime;
	CObj*				m_pObjFocus;
	OBJID				m_idObjFocusOld;
	DWORD				m_dwAmbient;
	CMapStringToPtr		m_mapLight;
	DWORD				m_dwLightIndex;
	int					m_nVisibilityLand;
	FLOAT				m_fFogStartValue;		//
	FLOAT				m_fFogEndValue;			//
	FLOAT				m_fFogDensity;			//
	int					m_nCharLandPosX;
	int					m_nCharLandPosZ;

	int                     m_nBoundBoxVertexNum;
	LPDIRECT3DVERTEXBUFFER9 m_pBoundBoxVertexBuffer;
    LPDIRECT3DDEVICE9       m_pd3dDevice;        
	D3DMATERIAL9			m_baseMaterial;

	// 디버그 정보 관련 
	static BOOL			m_bViewLODTerrain;
	static BOOL			m_bMiniMapRender;
	BOOL				m_bViewGrid;
	BOOL				m_bViewGridPatch;
	BOOL				m_bViewGridLandscape;
	BOOL				m_bViewWireframe;
	BOOL				m_bViewSkybox;
	BOOL				m_bViewTerrain;
	BOOL				m_bViewAllObjects;
	BOOL				m_bViewFog;
	BOOL				m_bViewWater;
	BOOL				m_bViewName;
	BOOL				m_bViewHP;
	BOOL				m_bViewLight;
	BOOL				m_bViewWeather;
	BOOL				m_bViewBoundBox;
	BOOL				m_bViewLODObj;
	BOOL				m_bCullObj;
	BOOL				m_bProcess;
	BOOL				m_bViewObj;
	BOOL				m_bViewMover;
	BOOL				m_bViewItem;
	BOOL				m_bViewRegion;
	BOOL				m_bViewSpawn;
	BOOL				m_bViewHeightAttribute;
	BOOL				m_bViewIdState;
	// 디버그 정보 관련 끝 


	void			Process();
	static DWORD	GetDiffuseColor() { return D3DCOLOR_ARGB( 255,(int)(m_lightFogSky.Diffuse.r * 255),(int)(m_lightFogSky.Diffuse.g * 255),(int)(m_lightFogSky.Diffuse.b * 255) ); }
	static DWORD	GetAmbientColor() { return D3DCOLOR_ARGB( 255,(int)(m_lightFogSky.Ambient.r * 255),(int)(m_lightFogSky.Ambient.g * 255),(int)(m_lightFogSky.Ambient.b * 255) ); }
	void			WorldPosToLand( D3DXVECTOR3 vPos, int& x, int& z ); 
	void			WorldPosToLandPos( D3DXVECTOR3 vPos, int& x, int& z ); 
	BOOL			LandInWorld( int x, int z );
	BOOL			IsVecInVisibleLand( D3DXVECTOR3 vPos, D3DXVECTOR3 vCenterPos, int nVisibilityLand );
	BOOL			IsVecInRange( D3DXVECTOR3 vPos, D3DXVECTOR3 vCenterPos, FLOAT fRadius );
	void			SetObjFocus( CObj* pObj, BOOL bSend = TRUE );
	CObj*			GetObjFocus() { return m_pObjFocus; }
#if __VER >= 13 // __HOUSING
	CObj*			GetObjByName(TCHAR* ObjName);
	void			ForceTexture(LPDIRECT3DTEXTURE9 pNewTex);
	BOOL			ProcessObjCollision(D3DXVECTOR3 vPos, CObj* pTargetObj,CObj* pWallObj);
	BOOL			TestOBBIntersect(BBOX* BoxA, BBOX* BoxB);
	BOOL			TestTriIntersect(CObj* pWallObj, CObj* pTargetObj);
#endif	// __HOUSING
	CLandscape*		GetLandscape( CObj* pObj );
	CLandscape*		GetLandscape( D3DXVECTOR3 vPos );
	CLandscape*		GetLandscape( int x, int z );
	CPatch*			GetPatch( D3DXVECTOR3 vPos );
	void			ProcessAllSfx( void );

///////////////////////////////////////////////////////////////////////////////
#endif	//end !__WORLDSERVER
///////////////////////////////////////////////////////////////////////////////

public:
	CMapWordToPtr   m_mapCreateChar;
	FLOAT			m_fMaxHeight;
	FLOAT			m_fMinHeight;
	int				m_nDeleteObjs;
	CObj*			m_apDeleteObjs[MAX_DELETEOBJS];		//
	TCHAR			m_szFilePath[ MAX_PATH ];
	TCHAR			m_szFileName[ 64  ];
	TCHAR			m_szWorldName[ 128 ];
	TCHAR			m_szKeyRevival[ 32 ];
	DWORD			m_dwIdWorldRevival; 
	BOOL			m_bFly;
	BOOL			m_bIsIndoor;
	DWORD			m_dwDiffuse;
	D3DXVECTOR3		m_v3LightDir;
	DWORD           m_dwIdMusic;
	int				m_nPKMode;
	BOOL			m_bSrc;

//	CRegionElemArray	m_aBeginPos;
	CRegionElemArray	m_aRegion;
	CRegionElemArray	m_aStructure;

	LIGHTCOLOR m_k24Light[24];				//월드당 24시간 Light정보를 외부Data에서 가져옴 

#if __VER >= 15 // __BS_CHANGING_ENVIR
	string m_strCurContName;
	BOOL m_bProcessingEnvir;			
	DWORD m_dwOldTime;
	DWORD m_dwAddedTime;

	float m_fOldFogStart[2];
	float m_fOldFogEnd[2];
	int m_iOldWorldWeather;
	BOOL m_bUsing24Light;

	ENVIR_INFO m_kOldContinent;
	ENVIR_INFO m_kCurContinent;
	ENVIR_INFO_Container m_cContinents;

	//for the world
	WORLD_ENVIR_INFO m_kWorldEnvir;
#endif

#ifdef __BS_CHANGEABLE_WORLD_SEACLOUD
	TextureSet m_kSeaCloud;
#endif;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// World.cpp
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	DWORD			GetID()	{ return m_dwWorldID; }

#ifdef __JEFF_11_4
	BOOL	IsArena( void )	
		{	return( m_dwWorldID == WI_WORLD_ARENA );	}
#endif	// __JEFF_11_4

	void			CalcBound();
	BOOL			VecInWorld( const D3DXVECTOR3& vPos );
	BOOL			VecInWorld( FLOAT x, FLOAT z );
	void			ClipX( FLOAT& x );
	void			ClipZ( FLOAT& z );

	void			Free();
	void			LoadAllMoverDialog();

#ifdef __LAYER_1015
	BOOL			AddObj( CObj* pObj, BOOL bAddItToGlobalId, int nLayer );
	void			DeleteLayerControls( int nLayer );
#else	// __LAYER_1015
	BOOL			AddObj( CObj* pObj, BOOL bAddItToGlobalId = FALSE );
#endif	// __LAYER_1015
	void			RemoveObj( CObj* pObj );
	void			DeleteObj( CObj* pObj );
	void			DestroyObj( CObj* pObj );
	BOOL			DoNotAdd( CObj* pObj );
	BOOL			AddObjArray( CObj* pObj );
	void			RemoveObjArray( CObj* pObj );
	BOOL			InsertObjLink( CObj* pObj );
	BOOL			RemoveObjLink( CObj* pObj );
	BOOL			RemoveObjLink2( CObj* pObj );
	void			AddMoveLandObj( CObj* pObj );
#ifdef __LAYER_1015
	CObj*			GetObjInLinkMap( const D3DXVECTOR3 & vPos, DWORD dwLinkType, int nLinkLevel, int nLayer );
	BOOL			SetObjInLinkMap( const D3DXVECTOR3 & vPos, DWORD dwLinkType, int nLinkLevel, CObj* pObj, int nLayer );
#else	// __LAYER_1015
	CObj*			GetObjInLinkMap( const D3DXVECTOR3 & vPos, DWORD dwLinkType, int nLinkLevel );
	BOOL			SetObjInLinkMap( const D3DXVECTOR3 & vPos, DWORD dwLinkType, int nLinkLevel, CObj* pObj );
#endif	// __LAYER_1015

	FLOAT			GetFullHeight( const D3DXVECTOR3& vPos );
	// 2008/09/24 康: GetFullHeight가 정확한 오브젝트 위 y좌표를
	// 반환하지 않아 임시로 ProcessUnderCollision을 참고하여 만들었다.
	FLOAT			GetItemHeight( const D3DXVECTOR3 & vPos );
	FLOAT			GetUnderHeight( const D3DXVECTOR3 &vPos );
#if __VER >= 15 // __BOUND_BOX_COLLISION
	FLOAT			GetOverHeightForPlayer( D3DXVECTOR3 &vPos, CObj* pExceptionObj = NULL );
#endif // __BOUND_BOX_COLLISION
	FLOAT			GetOverHeight( D3DXVECTOR3 &vPos, CObj* pExceptionObj = NULL );
	FLOAT			GetLandHeight( float x, float z );
	FLOAT			GetLandHeight( const D3DXVECTOR3& vPos );
	FLOAT			GetLandHeight_Fast( float x, float z );
	void			GetLandTri( float x, float z, D3DXVECTOR3* pTri );
	BOOL			GetLandTri2( float x, float z, D3DXVECTOR3* pTri2 );
	int				GetLandTris( float x, float z, D3DXVECTOR3* pTris );
	LPWATERHEIGHT	GetWaterHeight( const D3DXVECTOR3& vPos )	{ return GetWaterHeight( (int)vPos.x, (int)vPos.z ); }
	LPWATERHEIGHT	GetWaterHeight( int x, int z );
	int				GetHeightAttribute( float x, float z );
	
#ifdef __LAYER_1015
	BOOL			ProcessCollision( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir, int nSlideCnt, int nLayer );
#else	// __LAYER_1015
	BOOL			ProcessCollision( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir, int nSlideCnt );
#endif	// __LAYER_1015
	BOOL			ProcessCollisionReflection( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir, int nRecusCnt );
	FLOAT			ProcessUnderCollision( D3DXVECTOR3 *pOut, CObj **pObjColl, D3DXVECTOR3 &vPos );	
	BOOL			IntersectObjLine( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vEnd, BOOL bSkipTrans = FALSE, BOOL bWithTerrain = FALSE, BOOL bWithObject = TRUE );	
	BOOL			IntersectObjLine2( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vEnd, BOOL bSkipTrans = FALSE, BOOL bWithTerrain = FALSE, BOOL bWithObject = TRUE );	
	FLOAT			IntersectRayTerrain2( const D3DXVECTOR3 &vPickRayOrig, const D3DXVECTOR3 &vPickRayDir );
	void			SendDamageAround( const D3DXVECTOR3 *pvPos, int nDmgType, CMover *pAttacker, int nApplyType, int nAttackID, float fRange );
		
#ifdef __WORLDSERVER
	BOOL	IsUsableDYO( CObj* pObj );
#ifdef __CHIPI_DYO
	BOOL	IsUsableDYO2( LPCHARACTER pCharacter );
#endif // __CHIPI_DYO
#endif	// __WORLDSERVER

#ifdef __CLIENT
#if __VER >= 15 // __BS_CHANGING_ENVIR
public:
	int				GetDiffuseAvg( );
	BOOL			IsInContinent( );	//존재하는 어느대륙에라도 속해있는가?

	ENVIR_INFO*		GetContinentInfo( const string& name );

	//Hook the rendering
	BOOL			HookRenderSky( CSkyBox::SKY_TYPE eType, int numRender, float& fAlpha );	

protected:
	//for the world
	void			InitWorldEnvir( const char* filename );
	BOOL			ReadFile24Light( const char* filename );

	//for the continent
	void			ReadFileContinent( const char* filename );
	void			InitContinent( const char* filename );		//월드 진입시 초기화 ( 파일정보 추출 )
	void			InitAfterCreatedPlayer( );				//월드가 생성되고 주인공이 생성된 후 2차초기화 
	void			StartEV( const ENVIR_INFO& kOld );		//환경변화 시작 !				
	BOOL			CheckInOutContinent( );					//주인공에 대한 대륙진입 체크 ( 월드 <-> 대륙 )
	void			MoveInContinent( const ENVIR_INFO& kInfo );
	void			MoveOutContinent( const string& oldname );
	void			HookUpdateLight( CLight* pLight );		//환경이 변해야 할때 Hooking the light 
	ENVIR_INFO*		GetInContinent( const D3DXVECTOR3& test_point );
	
	BOOL			HookRenderSky_Side( int numRender, ENVIR_INFO* pInfo, float& fAlpha );
	BOOL			HookRenderSky_Cloud( int numRender, ENVIR_INFO* pInfo,float& fAlpha );
	
	//debugging
	void			UpdateContinentLines( );
	void			RenderContinentLines( );
	
#endif
#if __VER >= 15 // __15TH_INSTANCE_DUNGEON
public:
	BOOL			IsWorldInstanceDungeon() { return ( WI_INSTANCE_OMINOUS <= GetID() && WI_INSTANCE_LAST_ID >= GetID() ); }
#endif // __15TH_INSTANCE_DUNGEON
#endif // __CLIENT

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// World3D.cpp
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if __VER >= 15 // __GUILD_HOUSE
public:
	BOOL			IsWorldGuildHouse() { return ( WI_GUILDHOUSE_SMALL <= GetID() && WI_GUILDHOUSE_LARGE >= GetID() ); }
#endif // __GUILD_HOUSE

#ifndef __WORLDSERVER
public:
	BOOL			InitWorldEnvir( );		// 24시간 Light 설정 및 기타 환경 초기화 

#if __VER >= 15 // __GUILD_HOUSE
	void			InProcessing( );		// gmpbigsun : 현재 월드로 진입시 한번 호출됨
	void			OutProcessing( );		// gmpibgsun : 현재 월드에서 퇴장시 한번 호출됨 
#endif

	// Render
	void			Projection( LPDIRECT3DDEVICE9 pd3dDevice, int nWidth, int nHeight );
	void			Render( LPDIRECT3DDEVICE9 pd3dDevice, CD3DFont* pFont = NULL );
	void			RenderBase( LPDIRECT3DDEVICE9 pd3dDevice, CD3DFont* pFont );
		
	// Light, Camera and etc...
	void			AddLight( CLight* pLight );
	CLight*			GetLight( LPCTSTR lpszKey );
	void			SetCamera( CCamera* pCamera ) { m_pCamera = pCamera; }
	CCamera*		GetCamera() { return m_pCamera; }
	void			SetLight( BOOL bLight );
	void			SetFogEnable( LPDIRECT3DDEVICE9 pd3dDevice, BOOL bEnable );

	// Culling
	void			UpdateCullInfo( D3DXMATRIX* pMatView, D3DXMATRIX* pMatProj );
	void			CalculateBound();

	// Pick and Intersect
	BOOL			ClientPointToVector( D3DXVECTOR3 *pOut, RECT rect, POINT point, D3DXMATRIX* pmatProj, D3DXMATRIX* pmatView, D3DXVECTOR3* pVector, BOOL bObject = FALSE );
	BOOL			IsPickTerrain(RECT rect, POINT point, D3DXMATRIX* pmatProj, D3DXMATRIX* pmatView );
	FLOAT			IntersectRayTerrain( const D3DXVECTOR3 &vPickRayOrig, const D3DXVECTOR3 &vPickRayDir );
	CObj*			PickObject( RECT rectClient, POINT ptClient, D3DXMATRIX* pmatProj, D3DXMATRIX* pmatView, DWORD dwObjectFilter = 0xffffffff, CObj* pExceptionObj = NULL, D3DXVECTOR3* pVector = NULL, BOOL bOnlyTopPick = FALSE, BOOL bOnlyNPC = FALSE  );
	CObj*			PickObject_Fast( RECT rectClient, POINT ptClient, D3DXMATRIX* pmatProj, D3DXMATRIX* pmatView, DWORD dwObjectFilter = 0xffffffff, CObj* pExceptionObj = NULL, BOOL bBoundBox = FALSE, BOOL bOnlyNPC = FALSE );
	BOOL			CheckBound( D3DXVECTOR3* vPos, D3DXVECTOR3* vDest, D3DXVECTOR3* vOut, FLOAT* fLength );
		
	void			RenderGrid();
	void			RenderGrids( CRect rect, WORD dx, DWORD color );
	void			RenderWorldGrids(int wx,int wy,CPoint ptLT,CPoint ptRB,WORD dx,DWORD color);
	
	// Direct3D 관련 오브젝트 초기화및 제거, 재설정 관련 
	HRESULT			InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT			RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT			DeleteDeviceObjects();
	HRESULT			InvalidateDeviceObjects();
	void			RenderTerrain();
	static HRESULT	StaticInitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	static HRESULT	StaticRestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	static HRESULT	StaticDeleteDeviceObjects();
	static HRESULT	StaticInvalidateDeviceObjects();

	BOOL			ReadWorld( D3DXVECTOR3 vPos, BOOL bEraseOldLand = TRUE );

private:
	void			RenderWater();
	void			RenderObject( CD3DFont* pFont = NULL );
	void			RenderBoundBoxVertex( CObj* pObj );
	void			SetBoundBoxVertex( CObj* pObj );

	void			RenderObj(CObj* pObj);

#endif // !__WORLDSERVER

	
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WorldFile.cpp
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	BOOL			OpenWorld( LPCTSTR lpszFileName, BOOL bDir = FALSE );
	BOOL			OpenWorld( OBJID idWorld, BOOL bDir = FALSE );
#ifdef __LAYER_1021
private:
	BOOL	HasNobody_Process( int nLayer );
	BOOL	HasNoObj_Add( int nLayer );
	BOOL	HasNobody_Replace( int nLayer );
	BOOL	IsLayerPlayer( CObj* pObj, int nLayer );

public:
#if __VER >= 15 // __GUILD_HOUSE
	void	Invalidate( int nLayer, BOOL bInvalid = TRUE )	{	m_linkMap.Invalidate( nLayer, bInvalid );	}
#else // __GUILD_HOUSE
	void	Invalidate( int nLayer )	{	m_linkMap.Invalidate( nLayer );	}
#endif // __GUILD_HOUSE
	BOOL	HasNobody( int nLayer );
	void	DriveOut( int nLayer );
	BOOL	LoadObject( int nLayer );
	BOOL	CreateLayer( int nLayer );
	BOOL	ReleaseLayer( int nLayer );		// 임의 호출 금지
	void	DestroyObj( int nLayer );
#else	// __LAYER_1021
	BOOL	LoadObject();
#endif	// __LAYER_1021
#ifdef __LAYER_1021
	void	Expand();
#endif	// __LAYER_1021
	BOOL			ReadRegion( CScript& s );
	BOOL			ReadRespawn( CScript& s );
	BOOL			LoadRegion();
	BOOL			LoadPatrol();
};


#if !defined(__WORLDSERVER)

inline BOOL CWorld::LandInWorld( int x, int z )
{
	if( x < 0 || z < 0 || x >= m_nLandWidth || z >= m_nLandHeight )
		return FALSE;
	return TRUE;
}

inline BOOL CWorld::IsVecInVisibleLand( D3DXVECTOR3 vPos, D3DXVECTOR3 vCenterPos, int nVisibilityLand )
{
	if( (int)vPos.x < 0 || (int)vPos.z < 0 || (int)vPos.x >= WORLD_WIDTH * m_iMPU || (int)vPos.z >= WORLD_HEIGHT * m_iMPU )
		return FALSE;
	int x1 = int( vCenterPos.x ) / ( MAP_SIZE * m_iMPU );
	int z1 = int( vCenterPos.z ) / ( MAP_SIZE * m_iMPU );
	int x2 = int( vPos.x ) / ( MAP_SIZE * m_iMPU );
	int z2 = int( vPos.z ) / ( MAP_SIZE * m_iMPU );
	return x2 >= (int)x1 - nVisibilityLand && x2 <= (int)x1 + nVisibilityLand &&
		   z2 >= (int)z1 - nVisibilityLand && z2 <= (int)z1 + nVisibilityLand ;
}

inline BOOL CWorld::IsVecInRange( D3DXVECTOR3 vPos, D3DXVECTOR3 vCenterPos, FLOAT fRadius  )
{
	if( (int)vPos.x < 0 || (int)vPos.z < 0 || (int)vPos.x >= WORLD_WIDTH * m_iMPU || (int)vPos.z >= WORLD_HEIGHT * m_iMPU )
		return FALSE;
	if( vPos.x >= ( vCenterPos.x - fRadius ) && vPos.z >= ( vCenterPos.z - fRadius ) 
		&& vPos.x < ( vCenterPos.x + fRadius ) && vPos.z < ( vCenterPos.z + fRadius ) )
		return TRUE;
	return FALSE;
}

inline void CWorld::WorldPosToLand( D3DXVECTOR3 vPos, int& x, int& z ) 
{
	x = int( vPos.x ) / ( MAP_SIZE * m_iMPU );
	z = int( vPos.z ) / ( MAP_SIZE * m_iMPU );
}

inline void CWorld::WorldPosToLandPos( D3DXVECTOR3 vPos, int& x, int& z ) 
{
	x = int( vPos.x ) % ( MAP_SIZE * m_iMPU );
	z = int( vPos.z ) % ( MAP_SIZE * m_iMPU );
}

inline CLandscape* CWorld::GetLandscape( CObj* pObj )
{
	FLOAT rX = pObj->GetPos().x;
	FLOAT rZ = pObj->GetPos().z;
	int   mX = int( rX / ( MAP_SIZE * m_iMPU ) );
	int   mZ = int( rZ / ( MAP_SIZE * m_iMPU ) );
	int   nOffset = mX + mZ * m_nLandWidth;
	if( nOffset < 0 || nOffset >= m_nLandWidth * m_nLandHeight )
		return NULL;
	return m_apLand[ nOffset ];
}

inline CLandscape* CWorld::GetLandscape( D3DXVECTOR3 vPos )
{
	FLOAT rX = vPos.x;
	FLOAT rZ = vPos.z ;
	int   mX = int( rX / ( MAP_SIZE * m_iMPU ) );
	int   mZ = int( rZ / ( MAP_SIZE * m_iMPU ) );
	int   nOffset = mX + mZ * m_nLandWidth;
	if( nOffset < 0 || nOffset >= m_nLandWidth * m_nLandHeight )
		return NULL;
	return m_apLand[ nOffset ];
}

inline CLandscape* CWorld::GetLandscape( int x, int z )
{
	return m_apLand[ x + z * m_nLandWidth ];
}

inline CPatch* CWorld::GetPatch( D3DXVECTOR3 vPos )
{
	if( (int)vPos.x < 0 || (int)vPos.z < 0 || (int)vPos.x >= m_nLandWidth * MAP_SIZE * m_iMPU|| (int)vPos.z >= m_nLandHeight * MAP_SIZE * m_iMPU ) return NULL;
	FLOAT rX = vPos.x/m_iMPU;
	FLOAT rZ = vPos.z/m_iMPU ;
	int   mX = int( rX / ( MAP_SIZE ) );
	int   mZ = int( rZ / ( MAP_SIZE ) );
	FLOAT rPX = rX - mX*MAP_SIZE ;
	FLOAT rPZ = rZ - mZ*MAP_SIZE ;
	int   mPX = int( rPX / ( PATCH_SIZE ) );
	int   mPZ = int( rPZ / ( PATCH_SIZE ) );
	return &(m_apLand[ mX + mZ * m_nLandWidth ]->m_aPatches[mPZ][mPX]);
}
#endif	// !__WORLDSERVER


inline BOOL CWorld::VecInWorld( FLOAT x, FLOAT z )
{
//	if( x < 0 || z < 0 || (int)x >= m_nLandWidth * ( MAP_SIZE * MPU ) - 4 || 
//	(int)z >= m_nLandHeight * ( MAP_SIZE * MPU ) - 4 )
//		return FALSE;

	float inv_mpu = 1.0f / (float)m_iMPU;

	if( _isnan( x ) )
		return FALSE;
	if( x < 0.0f )
		return FALSE;
	if( (x * inv_mpu) >= ( WORLD_WIDTH-1 ) )	
	{
		return FALSE;
	}

	if( _isnan( z ) )
		return FALSE;
	if( z < 0.0f )
		return FALSE;
	if( (z * inv_mpu) >= ( WORLD_HEIGHT-1 ) )		
		return FALSE;

	return TRUE;
}

inline BOOL CWorld::VecInWorld( const D3DXVECTOR3 & vPos )
{
	if( _isnan( (double)vPos.y ) )
		return FALSE;

	return VecInWorld( vPos.x, vPos.z );
}

inline void CWorld::ClipX( FLOAT& x )
{
	if( x < WLD_EXTRA_WIDTH ) 
		x = WLD_EXTRA_WIDTH;
	else
	{
		float fMax = WORLD_WIDTH * m_iMPU - WLD_EXTRA_WIDTH;
		if( x > fMax )	
			x = fMax;
	}
}

inline void CWorld::ClipZ( FLOAT& z )
{
	if( z < WLD_EXTRA_WIDTH ) 
		z = WLD_EXTRA_WIDTH;
	else
	{
		float fMax = WORLD_HEIGHT * m_iMPU - WLD_EXTRA_WIDTH;
		if( z > fMax )	
			z = fMax;
	}
}


extern CObj *GetLastPickObj( void );

//gmpbigsun : MPU -> _pWorld->m_iMPU로 수정함.

#ifdef __WORLDSERVER	// _nRadiusLink
#ifdef __LAYER_1015
	#define FOR_LINKMAP( _pWorld, _vPos, _pObj, _nRange, _dwLinkType, _nLayer ) { \
			int _nLinkX = (int)( _vPos.x / _pWorld->m_iMPU );	\
			int _nLinkZ = (int)( _vPos.z / _pWorld->m_iMPU );	\
			int _nWidthLink, _nMaxWidth;	\
			int _nMaxHeight;	\
			int _nUnit;	\
			int _nLinkXMin, _nLinkZMin, _nLinkXMax, _nLinkZMax;	\
			int _nX, _nZ;	\
			int _d;	\
			int _nPos;	\
			for( int _i = 0; _i < (_pWorld)->m_linkMap.GetMaxLinkLevel( _dwLinkType, _nLayer ); _i++ ) {	\
				_nWidthLink		= (_pWorld)->m_linkMap.GetLinkWidth( _dwLinkType, _i, _nLayer );	\
				CObj** _pObjs	= (_pWorld)->m_linkMap.GetObj( _dwLinkType, _i, _nLayer );	\
				ASSERT( _pObjs );	\
				_nMaxWidth	= _nWidthLink * (_pWorld)->m_nLandWidth ;	\
				_nMaxHeight	= _nWidthLink * (_pWorld)->m_nLandHeight;	\
				_nUnit	=  ( MAP_SIZE * (_pWorld)->m_nLandWidth ) / _nMaxWidth;	\
				_nX		= ( _nLinkX / _nUnit ) * _nUnit * _pWorld->m_iMPU;	\
				_nZ		= ( _nLinkZ / _nUnit ) * _nUnit * _pWorld->m_iMPU;	\
				_d		= _nUnit * _pWorld->m_iMPU / 2;	\
				_nX		= ( (int)( _vPos.x ) - _nX > _d )? 1: 0;	\
				_nZ		= ( (int)( _vPos.z ) - _nZ > _d )? 1: 0;	\
				_nLinkXMin = ( ( _nLinkX - _nRange ) / _nUnit ) + ( _nX - 1 );	if( _nLinkXMin < 0 ) _nLinkXMin = 0;	\
				_nLinkZMin = ( ( _nLinkZ - _nRange ) / _nUnit ) + ( _nZ - 1 );	if( _nLinkZMin < 0 ) _nLinkZMin = 0;	\
				_nLinkXMax = ( ( _nLinkX + _nRange ) / _nUnit ) + _nX;	if( _nLinkXMax >= _nMaxWidth  ) _nLinkXMax = _nMaxWidth - 1;	\
				_nLinkZMax = ( ( _nLinkZ + _nRange ) / _nUnit ) + _nZ;	if( _nLinkZMax >= _nMaxHeight ) _nLinkZMax = _nMaxHeight - 1;	\
				for( int _j = _nLinkZMin; _j <= _nLinkZMax; _j++ )	\
				{	\
					for( int _k = _nLinkXMin; _k <= _nLinkXMax; _k++ ) \
					{	\
						_nPos = _j * _nMaxWidth + _k;	\
						_pObj = _pObjs[ _nPos ];	\
						int __limit = 1000;  \
						while( _pObj && __limit-- ) { 
#else	// __LAYER_1015
	#define FOR_LINKMAP( _pWorld, _vPos, _pObj, _nRange, _dwLinkType, _nLayer ) { \
			int _nLinkX = (int)( _vPos.x / _pWorld->m_iMPU );	\
			int _nLinkZ = (int)( _vPos.z / _pWorld->m_iMPU );	\
			int _nWidthLink, _nMaxWidth;	\
			int _nMaxHeight;	\
			int _nUnit;	\
			int _nLinkXMin, _nLinkZMin, _nLinkXMax, _nLinkZMax;	\
			int _nX, _nZ;	\
			int _d;	\
			int _nPos;	\
			for( int _i = 0; _i < (_pWorld)->m_linkMap.GetMaxLinkLevel( _dwLinkType ); _i++ ) {	\
				_nWidthLink		= (_pWorld)->m_linkMap.GetLinkWidth( _dwLinkType, _i);	\
				CObj** _pObjs	= (_pWorld)->m_linkMap.GetObj( _dwLinkType, _i );	\
				ASSERT( _pObjs );	\
				_nMaxWidth	= _nWidthLink * (_pWorld)->m_nLandWidth ;	\
				_nMaxHeight	= _nWidthLink * (_pWorld)->m_nLandHeight;	\
				_nUnit	=  ( MAP_SIZE * (_pWorld)->m_nLandWidth ) / _nMaxWidth;	\
				_nX		= ( _nLinkX / _nUnit ) * _nUnit * _pWorld->m_iMPU;	\
				_nZ		= ( _nLinkZ / _nUnit ) * _nUnit * _pWorld->m_iMPU;	\
				_d		= _nUnit * _pWorld->m_iMPU / 2;	\
				_nX		= ( (int)( _vPos.x ) - _nX > _d )? 1: 0;	\
				_nZ		= ( (int)( _vPos.z ) - _nZ > _d )? 1: 0;	\
				_nLinkXMin = ( ( _nLinkX - _nRange ) / _nUnit ) + ( _nX - 1 );	if( _nLinkXMin < 0 ) _nLinkXMin = 0;	\
				_nLinkZMin = ( ( _nLinkZ - _nRange ) / _nUnit ) + ( _nZ - 1 );	if( _nLinkZMin < 0 ) _nLinkZMin = 0;	\
				_nLinkXMax = ( ( _nLinkX + _nRange ) / _nUnit ) + _nX;	if( _nLinkXMax >= _nMaxWidth  ) _nLinkXMax = _nMaxWidth - 1;	\
				_nLinkZMax = ( ( _nLinkZ + _nRange ) / _nUnit ) + _nZ;	if( _nLinkZMax >= _nMaxHeight ) _nLinkZMax = _nMaxHeight - 1;	\
				for( int _j = _nLinkZMin; _j <= _nLinkZMax; _j++ )	\
				{	\
					for( int _k = _nLinkXMin; _k <= _nLinkXMax; _k++ ) \
					{	\
						_nPos = _j * _nMaxWidth + _k;	\
						_pObj = _pObjs[ _nPos ];	\
						int __limit = 1000;  \
						while( _pObj && __limit-- ) { 
#endif	// __LAYER_1015
	#define END_LINKMAP	\
		pObj = pObj->GetNextNode(); } } } } }
#else	// __WORLDSERVER
	#define FOR_LINKMAP( _pWorld, _vPos, _pObj, _nRange, _dwLinkType, _nLayer ) { \
		int _nLinkX = (int)( _vPos.x / _pWorld->m_iMPU );	\
		int _nLinkZ = (int)( _vPos.z / _pWorld->m_iMPU );	\
		int _nWidthLink;	\
		int _nMaxWidth, _nMaxHeight;	\
		int _nUnit;		\
		int _nLinkXMin, _nLinkZMin, _nLinkXMax, _nLinkZMax;	\
		int _nX, _nZ;	\
		int _d;		\
		int _nPos;	\
		for( int _i = 0; _i < MAX_LINKLEVEL; _i++ ) {	\
			_nWidthLink	= CLandscape::m_nWidthLinkMap[ _i ];	\
			_nMaxWidth	= _nWidthLink * (_pWorld)->m_nLandWidth;	\
			_nMaxHeight		= _nWidthLink * (_pWorld)->m_nLandHeight;	\
			_nUnit	= ( MAP_SIZE * (_pWorld)->m_nLandWidth ) / _nMaxWidth;	\
			_nX		= ( _nLinkX / _nUnit ) * _nUnit * _pWorld->m_iMPU;	\
			_nZ		= ( _nLinkZ / _nUnit ) * _nUnit * _pWorld->m_iMPU;	\
			_d		= _nUnit * _pWorld->m_iMPU / 2;		\
			_nX		= ( (int)( _vPos.x ) - _nX > _d )? 1: 0;		\
			_nZ		= ( (int)( _vPos.z ) - _nZ > _d )? 1: 0;		\
			_nLinkXMin = ( ( _nLinkX - _nRange ) / _nUnit ) + ( _nX - 1 );	if( _nLinkXMin < 0 ) _nLinkXMin = 0;	\
			_nLinkZMin = ( ( _nLinkZ - _nRange ) / _nUnit ) + ( _nZ - 1 );	if( _nLinkZMin < 0 ) _nLinkZMin = 0;	\
			_nLinkXMax = ( ( _nLinkX + _nRange ) / _nUnit ) + _nX;	if( _nLinkXMax >= _nMaxWidth  ) _nLinkXMax = _nMaxWidth - 1;	\
			_nLinkZMax = ( ( _nLinkZ + _nRange ) / _nUnit ) + _nZ;	if( _nLinkZMax >= _nMaxHeight ) _nLinkZMax = _nMaxHeight - 1;	\
			for( int _j = _nLinkZMin; _j <= _nLinkZMax; _j++ ) \
			{ \
				for( int _k = _nLinkXMin; _k <= _nLinkXMax; _k++ ) \
				{ \
					CLandscape* _pLand \
						= (_pWorld)->m_apLand[ ( _j / _nWidthLink ) * (_pWorld)->m_nLandWidth + ( _k / _nWidthLink ) ]; \
					if( _pLand ) \
					{ \
						_nPos = ( _j % _nWidthLink ) * _nWidthLink + ( _k % _nWidthLink ); \
	 					CObj** _pObjs = _pLand->GetObjLink( _dwLinkType, _i );	\
						ASSERT( _pObjs ); \
						_pObj = _pObjs[ _nPos ]; \
						while( _pObj ) { \
							if( IsValidObj( pObj ) )  

	#define END_LINKMAP pObj = pObj->GetNextNode(); } } } } } }

	#define FOR_LAND( _pWorld, _pLand, _nVisibilityLand, _bVisuble ) { \
		if( (_pWorld)->m_pCamera ) \
		{ \
			int _i, _j, _x, _y; \
			(_pWorld)->WorldPosToLand( (_pWorld)->m_pCamera->m_vPos, _x, _y ); \
			int _nXMin = _x - _nVisibilityLand; if( _nXMin < 0 ) _nXMin = 0; \
			int _nYMin = _y - _nVisibilityLand; if( _nYMin < 0 ) _nYMin = 0; \
			int _nXMax = _x + _nVisibilityLand; if( _nXMax >= (_pWorld)->m_nLandWidth  ) _nXMax = (_pWorld)->m_nLandWidth - 1; \
			int _nYMax = _y + _nVisibilityLand; if( _nYMax >= (_pWorld)->m_nLandHeight ) _nYMax = (_pWorld)->m_nLandHeight - 1; \
			for( _i = _nYMin; _i <= _nYMax; _i++ ) \
			{ \
				for( _j = _nXMin; _j <= _nXMax; _j++ ) \
				{ \
					if( (_pWorld)->m_apLand[ _i * (_pWorld)->m_nLandWidth + _j ] ) \
					{ \
						_pLand = (_pWorld)->m_apLand[ _i * (_pWorld)->m_nLandWidth + _j ]; \
						if( _bVisuble == FALSE || ( _bVisuble == TRUE && _pLand->isVisibile() ) ) 

	#define END_LAND } } } } }

	#define FOR_OBJARRAY( _pLand, _pObj ) { \
		for( int _k = 0; _k < MAX_OBJARRAY; _k++ ) \
		{ \
			CObj** _apObjs = _pLand->m_apObject[ _k ]; \
			for( int _l = 0; _l < int( _pLand->m_adwObjNum[ _k ] ); _l++ ) \
			{ \
				_pObj = _apObjs[ _l ]; \
				if( IsValidObj( _pObj ) )  

	#define END_OBJARRAY } } }

	#define FOR_OBJ( _pLand, _pObj, _nType ) { \
			CObj** _apObjs = _pLand->m_apObject[ _nType ]; \
			for( int _l = 0; _l < int( _pLand->m_adwObjNum[ _nType ] ); _l++ ) \
			{ \
				_pObj = _apObjs[ _l ]; \
				if( IsValidObj( _pObj ) )  

	#define END_OBJ } }  

#endif	// __WORLDSERVER
#endif	// __WORLD_2002_1_22