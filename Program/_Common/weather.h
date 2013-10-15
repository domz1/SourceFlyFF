#ifndef __WEATHER_H
#define __WEATHER_H

///////////////////////////////////

#define WEATHER_NONE       0
#define WEATHER_THUNDER    1
#define WEATHER_RAIN       2
#define WEATHER_SNOW       3

#define WEATHER_SPRING     0
#define WEATHER_SUMMER     1
#define WEATHER_FALL       2
#define WEATHER_WINTER     3

////////////////////////////////// 

class CWeather;

class CSnow : public CObj 
{
public:
static int m_nSnowNum;
	CWeather* m_pWeather;
	CObj* m_pObjCenter;
	FLOAT m_fFallSpeed;
	FLOAT m_fDirSpeed ;
	CSnow(CWeather* pWeather,CObj* pObj);
	~CSnow();
	virtual void Process();
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
};
//////////////////////////////////

class CRain : public CObj
{
public:
static int m_nRainNum;
	CWeather* m_pWeather;
	CObj* m_pObjCenter;
	FLOAT m_fFallSpeed;
	FLOAT m_fDirSpeed ;
	CRain(CWeather* pWeather,CObj* pObj);
	~CRain();
	virtual void Process();
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
};
//////////////////////////////////

class CFireRain : public CObj
{
public:
static int m_nFireRainNum;
	CWeather* m_pWeather;
	CObj* m_pObjCenter;
	int m_nFallSpeed;
	int m_nDirSpeed;
	CFireRain(CObj *pObj);
	~CFireRain();
	virtual void Process();
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
};
class CWeather  
{
	void ControlWeather();
public:
	CDate* m_pDate         ;
	int  m_nMaxWindSpeed   ;
	int  m_nMaxCloud       ;
	int  m_nEffect         ;
	int  m_nSeason         ;
	int  m_nCloud          ;
	int  m_nTemperature    ;
	int  m_nHumidity       ;
	int  m_nWind           ;
	int  m_nWindSpeed      ;
	int  m_nThunderLight   ;
	int  m_nWeatherCnt     ;
	BOOL m_bFixCloud       ;
	BOOL m_bFixTemperature ;
	BOOL m_bFixHumidity    ;
	BOOL m_bFixWind        ;
	BOOL m_bFixWindSpeed   ; 
	DWORD m_dwWeather      ; // rain, snow,
	/*
	BOOL m_bFireRain;
	BOOL m_bRain;
	BOOL m_bSnow;
	//BOOL m_bIndoor         ;
*/
	CWeather(CDate* pDate);
	CWeather();
	~CWeather();
	void SetWeather(CTime* pTime,int nSeason,int nCloud,int nTemperature,int nHumidity,int nWind);
	void SetWeather(int nSeason,int nCloud,int nTemperature,int nHumidity,int nWind);
	BOOL IsThunder();
	virtual void Process( CWorld* pWorld );
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
};
#endif