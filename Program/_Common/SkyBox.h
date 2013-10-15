#ifndef __SKYBOX_H_2002_5_14
#define __SKYBOX_H_2002_5_14

#include <D3D9.h>
#include <basetsd.h> 

class CWorld;

#define D3DFVF_D3DSKYBOXVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)
#define D3DFVF_D3DSUNVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

struct D3DSKYBOXVERTEX
{
	D3DXVECTOR3 p;
	DWORD       c;
	FLOAT       tu1, tv1;
};

struct D3DSUNVERTEX
{
	D3DXVECTOR3 p;
	DWORD       c;
	FLOAT       tu1, tv1;
};

//#define WEATHER_NONE 0 
//#define WEATHER_SLOW 1 
//#define WEATHER_RAIN 2 
class CSkyBox
{
	CTimer m_timerWeather;
	int m_nWeather;
	void	DrawRain(LPDIRECT3DDEVICE9 pd3dDevice);
	void	DrawSnow(LPDIRECT3DDEVICE9 pd3dDevice);
	void	DrawFall(LPDIRECT3DDEVICE9 pd3dDevice);
	
public:
	enum SKY_TYPE
	{
		SKY_SIDE,
		SKY_CLOUD,
		SKY_MOON,
		SKY_SUN,
	};

	// 각종 텍스쳐
	LPDIRECT3DTEXTURE9 m_pMoonTexture;
	LPDIRECT3DTEXTURE9 m_pSkyBoxTexture;
	LPDIRECT3DTEXTURE9 m_pSkyBoxTexture2;
	LPDIRECT3DTEXTURE9 m_pSkyBoxTexture3;
	LPDIRECT3DTEXTURE9 m_pCloudTexture;
	LPDIRECT3DTEXTURE9 m_pCloudTexture2;
	LPDIRECT3DTEXTURE9 m_pCloudTexture3;
	LPDIRECT3DTEXTURE9 m_pSunTexture;
	LPDIRECT3DTEXTURE9 m_pSunTexture2;
	LPDIRECT3DTEXTURE9 m_pSnowTexture;
	LPDIRECT3DTEXTURE9 m_pLensTexture[8];
	LPDIRECT3DVERTEXBUFFER9 m_pTopSkyBoxVB;
	LPDIRECT3DVERTEXBUFFER9 m_pSideSkyBoxVB;
	LPDIRECT3DVERTEXBUFFER9 m_pSunVB;
	LPDIRECT3DVERTEXBUFFER9 m_pLensFlareVB[8];
	LPDIRECT3DVERTEXBUFFER9 m_pRainVB;
	LPDIRECT3DVERTEXBUFFER9 m_pSnowVB;
	LPDIRECT3DVERTEXBUFFER9 m_pFallVB;
	LPDIRECT3DVERTEXBUFFER9 m_pStarVB;
	CWorld*        m_pWorld       ; // 자신이 속한 월드의 포인터를 갖는다. 

	D3DXVECTOR3 m_vVelocity[2000]; // 눈빗방울용 속도 
	D3DXVECTOR3 m_vFall[2000]; // 눈빗방울용 위치 배열
	//D3DXVECTOR3 m_vSnow[2000]; // 눈덩이용 위치 배열
//	D3DXVECTOR3 m_vStar[1000];

	D3DXMATRIX m_matStar[2000]; // 별용 위치 배열
	DWORD m_nFall; // 현재 빗방울 갯수
//	DWORD m_nSnow; // 현재 눈덩이 갯수
//	DWORD m_nStar; // 현재 별 갯수

	BOOL m_bOldSkyBox;

	float m_fCloudx,m_fCloudy; // 흘러가는 구름용 좌표
	float m_fSunAngle; // 현재 태양의 각도

#if __VER >= 15 // __BS_CHANGING_ENVIR
	BOOL m_bLockWeather; //gmpbigsun :  날씨 고정flag (대륙단위로 설정한 날씨는 어떠한 이유로든 변하지 않는다)
#endif

	CSkyBox();
	~CSkyBox();

#ifdef __ENVIRONMENT_EFFECT
	static	CSkyBox*	GetInstance();
#endif // __ENVIRONMENT_EFFECT

	void SetWeather( int nWeather, BOOL bOnOff );		

	void InitFall();

	void Free();
	void SetVertices();
	void SetStarVertices();

	// 처리 및 출력 
	void    Process();
	void    Render(CWorld* pWorld, LPDIRECT3DDEVICE9 pd3dDevice);
	void	DrawLensFlare(LPDIRECT3DDEVICE9 pd3dDevice);
	void    RenderFall( LPDIRECT3DDEVICE9 pd3dDevice );
		
	BOOL CheckSun(LPDIRECT3DDEVICE9 pd3dDevice);

	int GetWeather( )	{ return m_nWeather; }

	IDirect3DTexture9* GetCurrSkyTexture( SKY_TYPE eType );

	// Direct3D 관련 오브젝트 초기화및 제거, 재설정 관련 
	HRESULT InitDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice);
	HRESULT RestoreDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice);
	HRESULT DeleteDeviceObjects();
	HRESULT InvalidateDeviceObjects();
};

#endif