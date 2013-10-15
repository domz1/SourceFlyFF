#ifndef LANDSCAPE_H
#define LANDSCAPE_H

#ifndef __MAP_SIZE

#define MAP_SIZE (128)
#define NUM_PATCHES_PER_SIDE (16)							// 128(MAP_SIZE)에 16(NUM_PATCHES_PER_SIDE)개의 패치가 있음 
#define PATCH_SIZE (MAP_SIZE/NUM_PATCHES_PER_SIDE)			// 패치의 한 변의 크기
#define LIGHTMAP_SIZE ((PATCH_SIZE-1)*NUM_PATCHES_PER_SIDE) // 랜드스케이프 하나에 사용될 라이트맵의 한 변의 크기

#ifdef __CLIENT
#define LANDREALSCALE ( MAP_SIZE * MPU )
#define MPU g_MPU
#define MPU_INV ( 1.0f / float( MPU ) )
#else
//#define MPU 4												// METER PER UNIT
#define MPU OLD_MPU
#endif

//const float MPU_INV = 1.0f / float(MPU);

#define INIT_HEIGHT		0
#define MAX_LAYER		20

#define D3DFVF_D3DLANDSCAPEVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX2)
#define D3DFVF_WATERVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1) 
#define D3DFVF_HGTATTRVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE) 

#define HGT_NOWALK 1000.0f // 걷기, 점프 금지 
#define HGT_NOFLY  2000.0f // 비행 금지 
#define HGT_NOMOVE 3000.0f // 걷기, 비행 금지 
#define HGT_DIE    4000.0f // 죽음 

#define HATTR_NONE   0 
#define HATTR_NOWALK 1 // 걷기, 점프 금지 
#define HATTR_NOFLY  2 // 비행 금지 
#define HATTR_NOMOVE 3 // 걷기, 비행 금지 
#define HATTR_DIE    4 // 죽음 

#define WTYPE_NONE  0x00
#define WTYPE_CLOUD 0x01
#define WTYPE_WATER 0x02

struct WATERVERTEX 
{ 
	D3DXVECTOR3 p; 
	D3DXVECTOR3 n; 
	DWORD color; 
	FLOAT u, v; 
};

struct HGTATTRVERTEX 
{ 
	D3DXVECTOR3 p; 
	D3DXVECTOR3 n; 
	DWORD color; 
};
struct D3DLANDSCAPEVERTEX
{
	D3DXVECTOR3 p; 
	D3DXVECTOR3 n;
	FLOAT       tu1, tv1;
	FLOAT       tu2, tv2;
};

class CObj;
class CWorld;


// 랜드스케이프에서 사용하는 레이어 클래스
// 하나당 한 종류의 텍스쳐가 어떻게 깔려있는지의 정보를 가진다
struct CLandLayer
{
public:
	BOOL m_bVisible;
	WORD m_nTex; // 이 레이어가 담당하는 텍스쳐 ID
	BOOL m_aPatchEnable[NUM_PATCHES_PER_SIDE*NUM_PATCHES_PER_SIDE]; // 패치의 표시 상태를 저장하는 배열
	LPDIRECT3DTEXTURE9 m_pLightMap; // 라이트맵 포인터

	CLandLayer(LPDIRECT3DDEVICE9 pd3dDevice,WORD nTex);
	~CLandLayer();
	BOOL GetPatchEnable(int x, int z) { return m_aPatchEnable[x+z*NUM_PATCHES_PER_SIDE]; } // 이 부분의 패치의 표시 상태를 리턴
};



typedef struct
{
	BYTE byWaterHeight;
	BYTE byWaterTexture;

} WATERHEIGHT,* LPWATERHEIGHT;

typedef struct
{
	DWORD					WaterVertexNum;
	LPDIRECT3DVERTEXBUFFER9	pVB;
} WATERVERTEXBUFFER, * LPWATERVB;

#define MASK_WATERFRAME	0xfc

// 랜드스케이프 클래스
// 패치의 집합으로서 월드의 일부를 구성
class CLandscape
{
	friend CWorld;
public:
	CLandscape();
	~CLandscape();

	LPDIRECT3DVERTEXBUFFER9 m_pVB;
protected:
	static FLOAT	m_fCloud_u1, m_fCloud_v1 ;
	static FLOAT	m_fCloud_u2, m_fCloud_v2 ;
#if __VER < 14 // __WATER_EXT
	static FLOAT	m_fWaterFrame;
#endif //__WATER_EXT

	LPDIRECT3DDEVICE9		m_pd3dDevice;	// d3d 디바이스
	CWorld*					m_pWorld;		// 월드의 포인터
	FLOAT*					m_pHeightMap;											// 높이 맵 (실제 할당 주소)
	WATERHEIGHT				m_aWaterHeight[ NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE ];
	BYTE					m_aLandAttr[ NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE ];
	BOOL					m_bDirty;										// 버텍스 버퍼를 수정할 필요가 있을 경우 TRUE로 세팅
	BOOL					m_bVisible;				 						// 컬링된 결과
	D3DXVECTOR3				m_avBounds[8];									// 컬링에 사용할 바운드 박스 벡터
	D3DXPLANE				m_aplaneBounds[6];								// 컬링에 사용할 바운드 박스 평면
	int						m_nWorldX,m_nWorldY;							// 이 랜드스케이프의 좌하단 월드좌표
	//LPDIRECT3DVERTEXBUFFER9 m_pVB;											// 버텍스 버퍼
	WATERVERTEXBUFFER		*m_pWaterVB;

	DWORD					m_nCloudVertexNum;
	LPDIRECT3DVERTEXBUFFER9 m_pCloudVertexBuffer;
	DWORD					m_nWaterVertexNum;
	LPDIRECT3DVERTEXBUFFER9 m_pWaterVertexBuffer;


	FLOAT				GetHeightMap( int nOffset );  
	void				OptimizeLayer();
	CLandLayer*			NewLayer( WORD nTex );								// 지정한 레이어의 포인터를 리턴
	int					GetHeightAttribute( int x, int z );
	LPWATERHEIGHT		GetWaterHeight(int x, int z );
	HRESULT				MakeWaterVertexBuffer();
	HRESULT				MakeLandAttrVertexBuffer();
	HRESULT				MakeHgtAttrVertexBuffer();

public:
	static	int			m_nWidthLinkMap[ MAX_LINKLEVEL ];

	DWORD				m_dwVersion;
	CPtrArray			m_aLayer; // 이 랜드스케이프에 사용될 레이어들의 배열
	BOOL				m_abPatchRendered[NUM_PATCHES_PER_SIDE*NUM_PATCHES_PER_SIDE];
	CObj**				m_apObject [ MAX_OBJARRAY ];
	DWORD				m_adwObjNum[ MAX_OBJARRAY ];
	CDWordStack			m_aObjStack[ MAX_OBJARRAY ];
	CTexture			m_texMiniMap;
	CTexture            m_ALPHACIRCLE;
	CPatch				m_aPatches[NUM_PATCHES_PER_SIDE][NUM_PATCHES_PER_SIDE];	// 패치 배열
	CObj**				m_apObjLink[MAX_LINKTYPE][MAX_LINKLEVEL];
	
	CObj***				GetObjLink( DWORD dwLinkType )	{	return( m_apObjLink[dwLinkType] );	}
	CObj**				GetObjLink( DWORD dwLinkType, DWORD dwLinkLevel )	{	return( m_apObjLink[dwLinkType][dwLinkLevel] );		}
	HRESULT				InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice, CWorld* pWorld );
	HRESULT				RestoreDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice);
	HRESULT				InvalidateDeviceObjects();
	HRESULT				DeleteDeviceObjects();
	int					isVisibile( ) { return m_bVisible; } // 컬링된 결과를 리턴
	void				FreeTerrain(); // 지형의 메모리 할당을 해제한다.
	void				ResetTerrain( DWORD dwInitHeight ,BYTE* pHeight=NULL); // 지형을 초기화한다.
	void				NewLandscape( DWORD dwTextureId ); // 지형을 새로 만든다.
	void				SetVertices(); // 버텍스 버퍼 재구성
	void				RenderPatches(); // 모든 패치를 모든 레이어별로 그린다.
	HRESULT				Render( LPDIRECT3DDEVICE9 pd3dDevice, BOOL bLod = TRUE );
	HRESULT				RenderWater( LPDIRECT3DDEVICE9 pd3dDevice );
#if __VER >= 13 // __HOUSING
	BOOL				ForceTexture(LPDIRECT3DTEXTURE9 pNewTex);
#endif	// __HOUSING
	void				CalculateBound(); // 컬링용 바운드 박스 재계산
	void				UpdateCull(void); // 각 패치별 컬링과 LOD 적용
	void				Cull(); // 컬링
	FLOAT				GetHeight_Fast( float x, float z );
	FLOAT				GetHeight( FLOAT x, FLOAT z );
	FLOAT				GetHeight( DWORD x, DWORD z ); // 지정 위치의 높이를 돌려준다.
	FLOAT				GetHeight( POINT pos );
	void				ReadLandscape(FILE* fp); // 로드
	BOOL				LoadLandscape( LPCTSTR lpszFileName, int x = 0, int y = 0 );
	void				SetUsedAllObjects();
	void				AddObjArray( CObj* pObj );
	void				RemoveObjArray( CObj* pObj );
	void				RemoveObjArray( int nIndex );
	BOOL				InsertObjLink( CObj* pObj );
	BOOL				RemoveObjLink( CObj* pObj );
	BOOL				SetObjInLinkMap( D3DXVECTOR3 vPos, DWORD dwLinkType, int nLinkLevel, CObj* pObj );
	CObj*				GetObjInLinkMap( D3DXVECTOR3 vPos, DWORD dwLinkType, int nLinkLevel );
	int					GetObjArraySize();
};

void SetLODDetail( int nDetail );

#else	// not __MAP_SIZE

 #define MAP_SIZE (128)
 #define NUM_PATCHES_PER_SIDE (16)
 #define MPU 4 // METER PER UNIT

#endif	// __MAP_SIZE
#endif	// LANDSCAPE_H