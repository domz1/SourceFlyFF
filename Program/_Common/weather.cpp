#include "stdafx.h"
     
#define absC(x) (((x)<0) ? -(x):(x))
#define signC(x) ((x)>0 ? 1: ((x)==0?0:(-1)))
#define maxC(x,y) (((x)>(y))?(x):(y))
#define SUM_RAIN(nCloud) ((15 + ((nCloud - 70) / 2)) * 10)

int CSnow::m_nSnowNum = 0;

CSnow::CSnow(CWeather* pWeather,CObj* pObj) //: CObj(0,OI_SFX,SI_SNOW,0)
{
	m_nSnowNum++;
	m_pWeather = pWeather;
	m_pObjCenter = pObj;
	m_fFallSpeed = 0.05f;
	m_fDirSpeed  = 0.01f;
}
CSnow::~CSnow()
{
}
void CSnow::Process()
{
	D3DXVECTOR3 vPos = GetPos();
	D3DXVECTOR3 vPosCenter = m_pObjCenter->GetPos();
	//vPos.x = vPos2.x;
	//vPos.z = vPos2.z;
	if( vPos.y > vPosCenter.y - 1.0f )
	{
		vPos.x += m_fDirSpeed;
		vPos.y -= m_fFallSpeed;
	}
	if( vPos.y <= vPosCenter.y - 1.0f )
	{
//		CCtrl* pRain;
		/*
		WORD nTile = GetWorld()->GetTile(ptz);
		if(nTile != 0xffff)
		{
			int nTileId = prj.GetTileId(nTile);
			if(nTileId == TILE_RIVER || nTileId == TILE_RIVERSIDE || nTileId == TILE_SEA)
			{
				pRain = new CCtrl(CPtZ(ptz.x,ptz.y,0),OI_SFX,SI_RAIN2,1);
				m_pField->AddObj(pRain);
			}
		}
		*/
		// 빗방울 갯수가 총 빗방울 갯수보다 크면 파괴한다.
		int nSnowNum = 500;
		if( m_pWeather->m_nCloud >= 70 )
			nSnowNum = 10;//SUM_RAIN(m_pWeather->m_nCloud);
#ifndef __WORLDSERVER
		if( m_nSnowNum > nSnowNum || GetWorld()->m_bViewWeather == FALSE ) //|| m_pWeather->m_bFireRain )
#else
		if( m_nSnowNum > nSnowNum )
#endif
		{
			CSnow::m_nSnowNum--;
			GetWorld()->RemoveObj( this );
		}
		else
		{
//			if(m_pWeather->m_nCloud >= 70)
//				m_nFallSpeed = ((m_pWeather->m_nCloud - 50) / 2)+(rand() % 5);
//			m_nDirSpeed = m_pWeather->m_nWindSpeed / 10;
			//if(m_pWeather->m_nWind < 4)
//				m_nDirSpeed *= -1;
			do
			{
				vPos = m_pObjCenter->GetPos();
				vPos.x += (rand() % (8)) - 4;
				vPos.z += (rand() % (8)) - 4;
				vPos.y = m_pObjCenter->GetPos().y + (rand() % 10);
				break;
			}
			while( 1 );
		}
	}
	//if(vPos.x >= MAP_SIZE * 3)	ptz.x = MAP_SIZE * 3 - 1;
	SetPos( vPos );

	/*
	CPtZ ptz = GetPtz();

	if(ptz.z > 0)
	{
		ptz.x += m_nDirSpeed;
		ptz.x += (2 - xRandom(5));			// xRandom()은 xSFX.cpp에 새로 정의된 랜덤입니다. -_-;;;
		ptz.z -= (m_nFallSpeed / 4);		// 컴파일과정에서 자동으로 쉬프트로 최적화 된다(XuZhu).
	}
	if(ptz.z <= 0)
	{
		WORD nTile = GetField()->GetTile(ptz);
		if(nTile != 0xffff)
		{
			int nTileId = prj.GetTileId(nTile);
			// 떨어진 곳이 물이 아닐 경우에만 눈이 녹는 표현을 한다.
			if(nTileId != TILE_RIVER && nTileId != TILE_RIVERSIDE)
			{
				CCtrl* pSnow = new CCtrl(CPtZ(ptz.x,ptz.y,0),OI_SFX,SI_SNOW,0);
				pSnow->SetSectionPos(1+GetIdx3());
				m_pField->AddObj(pSnow);
			}
		}
		// 빗방울 갯수가 총 빗방울 갯수보다 크면 파괴한다.
		int nSnowNum = 0;
		if(m_pWeather->m_nCloud >= 70)
			nSnowNum = SUM_RAIN(m_pWeather->m_nCloud);
		if(m_nSnowNum > nSnowNum || m_pWeather->m_bFireRain)
			SetDestroy();
		else
		{
			int nSnowMax = 1 + (m_pWeather->m_nCloud - 71) / 10;
			SetIdx3(rand() % nSnowMax);
			m_nFallSpeed = ((m_pWeather->m_nCloud - 50) / 2)+(rand() % 5);
			m_nDirSpeed = m_pWeather->m_nWindSpeed / 10;
			do
			{
				ptz = m_pObjCenter->GetPtz();
				ptz.x += (rand() % (800)) - 400;
				ptz.y += (rand() % (800)) - 400;
				ptz.z = 200 + (rand() % 50);
			}
			while(!m_pField->PtInField(ptz));
		}
	}
	if(ptz.x >= GetField()->m_size.cx)	ptz.x = GetField()->m_size.cx-1;

	SetPtz(ptz);
	*/
//Obj::Process( fElapsedTime );
}
void CSnow::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( !IsVisible() || ( IsCull() && GetType() != 1 ))
		return;

	if( IsUpdateMatrix() )
		UpdateMatrix();

	if(GetPos().y >= 0)
	{
			//m_matWorld = m_matTrans;

		pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

		m_pModel->Render(pd3dDevice);

		pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	}
}
///////////////////////////////////////////////////////////////////

int CRain::m_nRainNum = 0;

CRain::CRain(CWeather* pWeather,CObj* pObj) //: CCtrl(0,OI_SFX,SI_RAIN,0)
{
	m_nRainNum++;
	m_pWeather = pWeather;
	m_pObjCenter = pObj;
	m_fFallSpeed = 0.5f;
	m_fDirSpeed  = 0.01f;
//	SetZ(0);
}
CRain::~CRain()
{
}

void CRain::Process()
{
	D3DXVECTOR3 vPos = GetPos();
	D3DXVECTOR3 vPosCenter = m_pObjCenter->GetPos();
	//vPos.x = vPos2.x;
	//vPos.z = vPos2.z;
	if( vPos.y > vPosCenter.y )
	{
		vPos.x += m_fDirSpeed;
		vPos.y -= m_fFallSpeed;
	}
	if( vPos.y <= vPosCenter.y )
	{
//		CCtrl* pRain;
		/*
		WORD nTile = GetWorld()->GetTile(ptz);
		if(nTile != 0xffff)
		{
			int nTileId = prj.GetTileId(nTile);
			if(nTileId == TILE_RIVER || nTileId == TILE_RIVERSIDE || nTileId == TILE_SEA)
			{
				pRain = new CCtrl(CPtZ(ptz.x,ptz.y,0),OI_SFX,SI_RAIN2,1);
				m_pField->AddObj(pRain);
			}
		}
		*/
		// 빗방울 갯수가 총 빗방울 갯수보다 크면 파괴한다.
		int nRainNum = 100;
		if( m_pWeather->m_nCloud >= 70 )
			nRainNum = 10;//SUM_RAIN(m_pWeather->m_nCloud);
#ifndef __WORLDSERVER
		if( m_nRainNum > nRainNum || GetWorld()->m_bViewWeather == FALSE )//|| m_pWeather->m_bFireRain )
#else
		if( m_nRainNum > nRainNum )
#endif
		{
			CRain::m_nRainNum--;
			GetWorld()->RemoveObj( this );
		}
		else
		{
//			if(m_pWeather->m_nCloud >= 70)
//				m_nFallSpeed = ((m_pWeather->m_nCloud - 50) / 2)+(rand() % 5);
//			m_nDirSpeed = m_pWeather->m_nWindSpeed / 10;
			//if(m_pWeather->m_nWind < 4)
//				m_nDirSpeed *= -1;
			do
			{
				vPos = m_pObjCenter->GetPos();
				vPos.x += (rand() % (8)) - 4;
				vPos.z += (rand() % (8)) - 4;
				vPos.y = m_pObjCenter->GetPos().y + (rand() % 10);
				break;
			}
			while( 1 );
		}
	}
	//if(vPos.x >= MAP_SIZE * 3)	ptz.x = MAP_SIZE * 3 - 1;
	SetPos( vPos );
}
void CRain::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( !IsVisible() || ( IsCull() && GetType() != 1 ))
		return;

	if( IsUpdateMatrix() )
		UpdateMatrix();

	if(GetPos().y >= 0)
	{

		pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_DESTALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCALPHA );

		m_pModel->Render(pd3dDevice);

		pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	}
}
///////////////////////////////////////////////////////////////////
int CFireRain::m_nFireRainNum = 0;

CFireRain::CFireRain(CObj *pObj)// : CCtrl(0,OI_SFX,SI_FIRERAIN_SMALL,0)
{
	m_nFireRainNum++;
	m_pObjCenter = pObj;
	m_nDirSpeed = 15;
	m_nFallSpeed = 25;
	//SetZ(0);
}

CFireRain::~CFireRain()
{
	if(m_nFireRainNum > 0)
		m_nFireRainNum--;
}

void CFireRain::Process()
{
	/*
	CPtZ ptz = GetPtz();
	if(ptz.z > 0)
	{
		ptz.x += m_nDirSpeed;
		ptz.z -= m_nFallSpeed;
	}
	if(ptz.z <= 0) {
		ptz.z = 0;
		WORD nTile = GetField()->GetTile(ptz);
		if(nTile != 0xffff) {
			int nTileId = prj.GetTileId(nTile);
			if(nTileId != TILE_WOOD && nTileId != TILE_LIBRARY && (rand()%100) > 75)
			{
				CCtrl *pObj = new CCtrl(CPtZ(ptz.x,ptz.y,0),OI_SFX,SI_FIRERAIN_AFTER,0);
				m_pField->AddObj(pObj);
			}
		}
		if(m_nFireRainNum==0)
			SetDestroy();
		else {
			do {
				ptz = m_pObjCenter->GetPtz();
				ptz.x += (rand() % (800)) - 400;
				ptz.y += (rand() % (800)) - 400;
				ptz.z = 200 + (rand() % 50);
			} while(!m_pField->PtInField(ptz));
		}
	}
	if(ptz.x >= GetField()->m_size.cx)	ptz.x = GetField()->m_size.cx-1;

	SetPtz(ptz);
	*/
	CObj::Process();
}
void CFireRain::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	/*
	if(GetPtz().z >= 0)
	{
		WORD nTile = GetField()->GetTile(GetPtz());
		if(nTile != 0xffff) {
			int nTileId = prj.GetTileId(nTile);
			if(nTileId != TILE_WOOD && nTileId != TILE_LIBRARY)
				CCtrl::Paint(pMemDC);
		}
	}
	*/
}
///////////////////////////////////////////////////////////////////

CWeather::CWeather(CDate* pDate)
{
	m_pDate           = pDate;
	m_nSeason         = 0;
	m_nWind           = 0;
	m_nCloud          = 30;
	m_nWindSpeed      = 20;
	m_nTemperature    = 0;
	m_nHumidity       = 0;    
	m_nThunderLight   = 0;
	m_bFixCloud       = FALSE;
	m_bFixWind        = FALSE;
	m_bFixWindSpeed   = FALSE;
	m_bFixTemperature = FALSE;
	m_bFixHumidity    = FALSE;    
//	m_bFireRain       = FALSE;
	m_nWeatherCnt     = 0;
}
CWeather::CWeather()
{
//	m_pDate = &prj.m_date;
	m_nSeason         = 0;
	m_nWind           = 0;
	m_nCloud          = 30;
	m_nMaxCloud       = 30;
	m_nWindSpeed      = 20;
	m_nMaxWindSpeed   = 20;
	m_nTemperature    = 0;
	m_nHumidity       = 0;    
	m_nThunderLight   = 0;
	m_bFixCloud       = FALSE;
	m_bFixWind        = FALSE;
	m_bFixWindSpeed   = FALSE;
	m_bFixTemperature = FALSE;
	m_bFixHumidity    = FALSE;    
//	m_bFireRain       = FALSE;
	m_nWeatherCnt     = 0;
}
CWeather::~CWeather()
{
}
void CWeather::ControlWeather()//
{
#ifndef _BEAST
	// 계절과 기본 기온, 기본 습도를 정한다.
	if(m_pDate->m_nMonth <= 2 || m_pDate->m_nMonth >= 11)
	{
		m_nSeason = WEATHER_WINTER;
		if(m_bFixTemperature == FALSE) m_nTemperature = 5;
		if(m_bFixHumidity == FALSE)	m_nHumidity = 10;
	}
	else
	if(m_pDate->m_nMonth >= 3 && m_pDate->m_nMonth <= 5)
	{
		m_nSeason = WEATHER_SPRING;
		if(m_bFixTemperature == FALSE) m_nTemperature = 10;
		if(m_bFixHumidity == FALSE)	m_nHumidity = 30;
	}
	else
	if(m_pDate->m_nMonth >= 6 && m_pDate->m_nMonth <= 8)
	{
		m_nSeason = WEATHER_SUMMER;
		if(m_bFixTemperature == FALSE) m_nTemperature = 30;
		if(m_bFixHumidity == FALSE)	m_nHumidity = 50;
	}
	else
	if(m_pDate->m_nMonth >= 9 && m_pDate->m_nMonth <= 10)
	{
		m_nSeason = WEATHER_FALL;
		if(m_bFixTemperature == FALSE) m_nTemperature = 10;
		if(m_bFixHumidity == FALSE)	m_nHumidity = 30;
	}
#ifdef _CLIENT
	if(g_Trans.m_bOnline)
		return;
#endif
	m_nWeatherCnt++;
//	if(m_nWeatherCnt > (PROCESSTIMER * 2))
	{
		m_nWeatherCnt = 0;

		// 시간에 따라서 기온과 습도를 조절한다.
		if((m_pDate->m_nHour >= 19 && m_pDate->m_nHour <= 24) || m_pDate->m_nHour <= 6)
		{
			if(m_bFixTemperature == FALSE) m_nTemperature -= 10;
			if(m_bFixHumidity == FALSE)	m_nHumidity += 10;
		}
		else
		if(m_pDate->m_nHour >= 7 && m_pDate->m_nHour <= 11)
		{
			if(m_bFixTemperature == FALSE) m_nTemperature -= 5;
			if(m_bFixHumidity == FALSE) m_nHumidity += 5;
		}
		/*
		// 구름의 증가/감소, 구름은 바람이 불고 습도가 높아야 증가한다.
		if(m_nWindSpeed && m_bFixWindSpeed == FALSE)
		{
			if(!(rand() % (101 - m_nWindSpeed)))
			{
				if(m_nCloud < m_nMaxCloud)
					m_nCloud++;
				else
				if(m_nCloud > m_nMaxCloud)
					m_nCloud--;
			}
		}
		*/
		// 바람을 불게 하거나 멈추게 하거나 바람 방향을 바꾼다. 
		if(!(rand() % 300))// && m_nWindSpeed == 0)
		{
			if(m_bFixWindSpeed == FALSE)
				m_nWindSpeed = rand() % 100;
			if(m_bFixWind == FALSE)
				m_nWind = rand() % 8;
		}
		/*
		if(m_nWindSpeed < m_nMaxWindSpeed)
		{
			if(m_bFixWindSpeed == FALSE)
				m_nWindSpeed++;
		}
		else
		if(m_nWindSpeed > m_nMaxWindSpeed)
		{
			if(m_bFixWindSpeed == FALSE)
				m_nWindSpeed--;
		}
		*/
		if(m_nWindSpeed == 0)
		{
			if(m_bFixWind == FALSE)
				m_nWind = -1;
		}
		if(m_bFixCloud == FALSE)
		{
			// 구름을 발생시킨다.
			if(m_nCloud < 70 && !(rand() % 1000))
				m_nCloud = 70 + rand() % 30;
			else
			if(m_nCloud > 70 && !(rand() % 300))
				m_nCloud = rand() % 50;
		}
		// 구름의 양, 비나 눈이 오는지 여부로 습도와 기온을 조절한다.
		if(m_nCloud > 70)
		{
			if(m_bFixTemperature == FALSE) m_nHumidity += m_nCloud - 70; // 최대 30 정도로 습도를 높인다.
			if(m_bFixHumidity == FALSE) m_nTemperature -= (m_nCloud - 70) / 5; // 최대 6 정도 기온을 떨어뜨린다.
		}
	}

	if(m_nThunderLight)
	{
		m_nThunderLight -= 3;
		if(m_nThunderLight < 0)
			m_nThunderLight = 0;
	}
	else
	if(m_nTemperature > 5 && m_nCloud > 90 && !(rand() % 100))
	{
		m_nThunderLight = 31;
//		PlayEffect( SND_THUNDER );
	}
#endif
}

static	int		nHourFlag = 0, nHourFlag2 = 0;

void CWeather::Process( CWorld* pWorld )
{
#if !defined( __WORLDSERVER )
	//if(m_nCloud >= 70)
	{
		
		// snow
		if( 1 ) //m_nTemperature <= 5 && m_nSeason == WEATHER_WINTER )
		{
			int nNum = 500;//SUM_RAIN( m_nCloud );
			if( CSnow::m_nSnowNum < nNum )
			{
				for(int i = 0; i < nNum - CSnow::m_nSnowNum ; i++)
				{
					CSnow* pSnow = new CSnow( this, g_pPlayer );
					pSnow->m_dwType = 1;
					pSnow->SetScale( D3DXVECTOR3(1.5f,1.5f,1.5f) );
					pSnow->SetPos( D3DXVECTOR3( 0.0f,-1.0f,0.0f) );
					pSnow->SetIndex( g_Neuz.m_pd3dDevice, 19 );
					pWorld->AddObj( pSnow );
				}
			}
		}
		else
		{
			int nNum = 100;//SUM_RAIN( m_nCloud );
			if( CRain::m_nRainNum < nNum )
			{
				for(int i = 0; i < nNum - CRain::m_nRainNum ; i++)
				{
					CRain* pRain = new CRain( this, g_pPlayer);
					pRain->m_dwType = 1;
					pRain->SetScale( D3DXVECTOR3(1.5f,1.5f,1.5f) );
					pRain->SetPos( D3DXVECTOR3( 0.0f,-1.0f,0.0f) );
					pRain->SetIndex( g_Neuz.m_pd3dDevice, 18 );
					pWorld->AddObj( pRain );
				}
			}
		}
	}
#endif // !defined( __WORLDSERVER )
}
void CWeather::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
//	if(m_nThunderLight)
	//	pMemDC->PaintAlphaRect(pMemDC->m_clipRect,MKRGB15(31,31,31),m_nThunderLight);
}
BOOL CWeather::IsThunder()
{
	if(m_nThunderLight) return TRUE;
	return FALSE;
}