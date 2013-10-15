// WorldMap.cpp: implementation of the CWorldMap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WorldMap.h"
#include "resData.h"
#include "party.h"
#include "defineText.h"

#if __VER >= 15 // __TELEPORTER
#include "defineSound.h"
#include "DPClient.h"
extern CDPClient g_DPlay;
#endif

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	//__SYS_PLAYER_DATA

#if __VER >= 13 // __RAINBOW_RACE
#include "RainbowRace.h"
#endif //__RAINBOW_RACE

#if __VER >= 14 // __NEW_CONTINENT
#include "Continent.h"
#endif // __NEW_CONTINENT

extern	CParty g_Party;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWorldMap::CWorldMap()
{
#if __VER < 14 // __NEW_CONTINENT
	m_mapView.clear();
#endif // __NEW_CONTINENT
	m_bRender = FALSE;
	m_pTexWorldMap = NULL;

	m_cPos.x = 0;
	m_cPos.y = 0;
#if __VER >= 13 // __CSC_VER13_1
	m_nSelMon = -1;
	m_bShowMonsterInfo = FALSE;
#endif //__CSC_VER13_1

#if __VER >= 15 // __TELEPORTER
	m_bTeleportMode = FALSE;
	m_pTelPosTexture = NULL;
	m_idTeleporter = NULL_ID;
#endif
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	m_rectDestination = CRect( -1, -1, -1, -1 );
	m_bDestinationMode = FALSE;
	m_pDestinationPositionTexture = NULL;
	m_nDestinationTextureAlpha = MINIMUM_ALPHA;
	m_tmOld = g_tmCurrent;
	m_bAlphaSwitch = FALSE;
#endif // __IMPROVE_QUEST_INTERFACE

#ifdef __IMPROVE15_WORLDMAP
	m_nSelected = 0;
#endif
}

CWorldMap::~CWorldMap()
{
#if __VER >= 13 // __CSC_VER13_1
	m_MonsterInfo.m_vecMonsterInfo.clear();
#endif //__CSC_VER13_1
#if __VER >= 13 // __RAINBOW_RACE
	m_RainbowNPC.m_vecRainbowNPC.clear();
#endif //__RAINBOW_RACE
	DeleteDeviceObjects();
}

CWorldMap* CWorldMap::GetInstance( void )
{
	static CWorldMap sWorldMap;
	return &sWorldMap;
}

FLOAT _fMap_X1 = 7194.0f;
FLOAT _fMap_Y1 = 2000.0f;
FLOAT _fMap_X2 = 10100.0f;
FLOAT _fMap_Y2 = 4250.0f;

void CWorldMap::Init()
{
	m_texArrow[0].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME,"ImgMapArrow.bmp"), 0xffff00ff );
	m_texArrow[1].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME,"ImgMapArrowParty.bmp"), 0xffff00ff );
	ZeroMemory( &m_billboard, sizeof( m_billboard )  );
	m_billboard[0].rect.SetRect( 0, 0, m_texArrow[0].m_size.cx, m_texArrow[0].m_size.cy );
	m_billboard[0].ptCenter = CPoint( m_texArrow[0].m_size.cx / 2, m_texArrow[0].m_size.cy / 2 );
	m_billArrow[0].InitDeviceObjects( g_Neuz.m_pd3dDevice, &m_billboard[0], &m_texArrow[0] );
	m_billArrow[0].RestoreDeviceObjects();

	m_billboard[1].rect.SetRect( 0, 0, m_texArrow[1].m_size.cx, m_texArrow[1].m_size.cy );
	m_billboard[1].ptCenter = CPoint( m_texArrow[1].m_size.cx / 2, m_texArrow[1].m_size.cy / 2 );
	m_billArrow[1].InitDeviceObjects( g_Neuz.m_pd3dDevice, &m_billboard[1], &m_texArrow[1] );
	m_billArrow[1].RestoreDeviceObjects();
	
#if __VER < 14 // __NEW_CONTINENT
	m_mapView.clear();
	vector<CPoint> vecMap;
	
	// 플라리스
	vecMap.clear();
	vecMap.push_back( CPoint(6159, 3407) );
	vecMap.push_back( CPoint(6350, 3825) );
	vecMap.push_back( CPoint(6736, 4649) );
	vecMap.push_back( CPoint(7246, 4570) );
	vecMap.push_back( CPoint(8258, 5022) );
	vecMap.push_back( CPoint(8213, 4367) );
	vecMap.push_back( CPoint(8021, 4037) );
	vecMap.push_back( CPoint(8017, 3496) );
	vecMap.push_back( CPoint(7245, 2685) );
	vecMap.push_back( CPoint(6783, 2631) );
	vecMap.push_back( CPoint(6366, 2922) );
	vecMap.push_back( CPoint(6159, 3407) );
	
	m_mapView.insert( multimap< CString, vector<CPoint> >::value_type( "Flaris", vecMap ) );
	
	// 세인트모닝
	vecMap.clear();
	vecMap.push_back( CPoint(7253, 2684) );
	vecMap.push_back( CPoint(8023, 3496) );
	vecMap.push_back( CPoint(8029, 4039) );
	vecMap.push_back( CPoint(8217, 4365) );
	vecMap.push_back( CPoint(8609, 4187) );
	vecMap.push_back( CPoint(9489, 3047) );
	vecMap.push_back( CPoint(9289, 2130) );
	vecMap.push_back( CPoint(8765, 1615) );
	vecMap.push_back( CPoint(7855, 1554) );
	vecMap.push_back( CPoint(7506, 1870) );
	vecMap.push_back( CPoint(7253, 2684) );
	
	m_mapView.insert( multimap< CString, vector<CPoint> >::value_type( "Saint", vecMap ) );
	
	// 리시스정원
	vecMap.clear();
	vecMap.push_back( CPoint(9123, 3339) );
	vecMap.push_back( CPoint(8922, 3740) );
	vecMap.push_back( CPoint(8607, 4127) );
	vecMap.push_back( CPoint(8285, 4397) );
	vecMap.push_back( CPoint(8316, 4748) );
	vecMap.push_back( CPoint(8833, 4762) );
	vecMap.push_back( CPoint(9612, 4992) );
	vecMap.push_back( CPoint(10436, 4396) );
	vecMap.push_back( CPoint(9441, 3171) );
	vecMap.push_back( CPoint(9123, 3339) );
	
	m_mapView.insert( multimap< CString, vector<CPoint> >::value_type( "Ricis", vecMap ) );
	
	// 다콘1
	vecMap.clear();
	vecMap.push_back( CPoint(4858, 2781) );
	vecMap.push_back( CPoint(4561, 3613) );
	vecMap.push_back( CPoint(4051, 3835) );
	vecMap.push_back( CPoint(3796, 3790) );
	vecMap.push_back( CPoint(3574, 4200) );
	vecMap.push_back( CPoint(3455, 4479) );
	vecMap.push_back( CPoint(3072, 4563) );
	vecMap.push_back( CPoint(2833, 5237) );
	vecMap.push_back( CPoint(3572, 5185) );
	vecMap.push_back( CPoint(4863, 4778) );
	vecMap.push_back( CPoint(6756, 4875) );
	vecMap.push_back( CPoint(6558, 4367) );
	vecMap.push_back( CPoint(6387, 3826) );
	vecMap.push_back( CPoint(6102, 3298) );
	vecMap.push_back( CPoint(5795, 2727) );
	vecMap.push_back( CPoint(4858, 2781) );
	
	m_mapView.insert( multimap< CString, vector<CPoint> >::value_type( "Darkon12", vecMap ) );
	
	// 다콘2,3
	vecMap.clear();
	vecMap.push_back( CPoint(2834, 5241) );
	vecMap.push_back( CPoint(3069, 4562) );
	vecMap.push_back( CPoint(3451, 4476) );
	vecMap.push_back( CPoint(3567, 4196) );
	vecMap.push_back( CPoint(3792, 3791) );
	vecMap.push_back( CPoint(4049, 3736) );
	vecMap.push_back( CPoint(4555, 3615) );
	vecMap.push_back( CPoint(4854, 2775) );
	vecMap.push_back( CPoint(3395, 1291) );
	vecMap.push_back( CPoint(1291, 2425) );
	vecMap.push_back( CPoint(1867, 3867) );
	vecMap.push_back( CPoint(1946, 4694) );
	vecMap.push_back( CPoint(2834, 5241) );
	
	m_mapView.insert( multimap< CString, vector<CPoint> >::value_type( "Darkon3", vecMap ) );
#endif // __NEW_CONTINENT

	m_strMapStringList[0] = "WORLD_Flyff.dds";
	m_strMapStringList[1] = "WORLD_Darkon12.dds";
	m_strMapStringList[2] = "WORLD_Darkon3.dds";
	m_strMapStringList[3] = "WORLD_Ricis.dds";
	m_strMapStringList[4] = "WORLD_Saint.dds";
	m_strMapStringList[5] = "WORLD_Flaris.dds";
#if __VER >= 14 // __NEW_CONTINENT
	m_strMapStringList[6] = "WORLD_harmonin.dds";
#endif // __NEW_CONTINENT

#if __VER >= 15 // __NEW_CONTINENT15
	m_strMapStringList[7] = "WORLD_Estia.dds";
#endif // __NEW_CONTINENT15

	m_texMapButton.LoadScript( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, _T( "texMapButton.inc" ) ) );	

#if __VER >= 13 // __CSC_VER13_1
	m_strMonScript[0] = "texMapMonster_Darkon12.inc";
	m_strMonScript[1] = "texMapMonster_Darkon3.inc";
	m_strMonScript[2] = "texMapMonster_Ricis.inc";
	m_strMonScript[3] = "texMapMonster_Saint.inc";
	m_strMonScript[4] = "texMapMonster_Flaris.inc";
#if __VER >= 14 // __NEW_CONTINENT
	m_strMonScript[5] = "texMapMonster_Harmonin.inc";
#endif //__NEW_CONTINENT

#if __VER >= 15 // __NEW_CONTINENT15
	m_strMonScript[6] = "texMapMonster_Estia.inc";
#endif //__NEW_CONTINENT15

#endif //__CSC_VER13_1

	m_fRate = 0.0f;
	switch( g_Option.m_nResWidth )
	{
	case 800:
		m_fRate = (FLOAT)800 / (FLOAT)1280;
		m_cpScreen.x = 800;
		m_cpScreen.y = 600;
		m_cOffset.x = (g_Option.m_nResWidth-m_cpScreen.x) / 2;
		m_cOffset.y = (g_Option.m_nResHeight-m_cpScreen.y) / 2;
		break;
	case 1024:
		m_fRate = (FLOAT)1024 / (FLOAT)1280;
		m_cpScreen.x = 1024;
		m_cpScreen.y = 768;
		m_cOffset.x = (g_Option.m_nResWidth-m_cpScreen.x) / 2;
		m_cOffset.y = (g_Option.m_nResHeight-m_cpScreen.y) / 2;
		break;
	case 1280:
#if __VER >= 9 // __CSC_VER9_RESOLUTION
		if(g_Option.m_nResHeight == 720) //Wide
		{
			m_fRate = (FLOAT)1280 / (FLOAT)1280;
			m_cpScreen.x = 960;
			m_cpScreen.y = 720;
			m_cOffset.x = (g_Option.m_nResWidth-m_cpScreen.x) / 2;
			m_cOffset.y = (g_Option.m_nResHeight-m_cpScreen.y) / 2;			
		}
		else if(g_Option.m_nResHeight == 768) //Wide
		{
			m_fRate = (FLOAT)1280 / (FLOAT)1280;
			m_cpScreen.x = 960;
			m_cpScreen.y = 768;
			m_cOffset.x = (g_Option.m_nResWidth-m_cpScreen.x) / 2;
			m_cOffset.y = (g_Option.m_nResHeight-m_cpScreen.y) / 2;			
		}
		else if(g_Option.m_nResHeight == 800) //Wide
		{
			m_fRate = (FLOAT)1280 / (FLOAT)1280;
			m_cpScreen.x = 960;
			m_cpScreen.y = 800;
			m_cOffset.x = (g_Option.m_nResWidth-m_cpScreen.x) / 2;
			m_cOffset.y = (g_Option.m_nResHeight-m_cpScreen.y) / 2;			
		}
		else
		{
			m_fRate = (FLOAT)1280 / (FLOAT)1280;
			m_cpScreen.x = 1280;
			m_cpScreen.y = 1024;
			m_cOffset.x = (g_Option.m_nResWidth-m_cpScreen.x) / 2;
			m_cOffset.y = (g_Option.m_nResHeight-m_cpScreen.y) / 2;			
		}
#else //__CSC_VER9_RESOLUTION
		m_fRate = (FLOAT)1280 / (FLOAT)1280;
		m_cpScreen.x = 1280;
		m_cpScreen.y = 960;
		m_cOffset.x = (g_Option.m_nResWidth-m_cpScreen.x) / 2;
		m_cOffset.y = (g_Option.m_nResHeight-m_cpScreen.y) / 2;
#endif //__CSC_VER9_RESOLUTION
		break;
#if __VER >= 9 // __CSC_VER9_RESOLUTION
	case 1360:
		m_fRate = (FLOAT)1360 / (FLOAT)1280;
		m_cpScreen.x = 1024;
		m_cpScreen.y = 768;
		m_cOffset.x = (g_Option.m_nResWidth-m_cpScreen.x) / 2;
		m_cOffset.y = (g_Option.m_nResHeight-m_cpScreen.y) / 2;
		break;
	case 1400:
		m_fRate = (FLOAT)1400 / (FLOAT)1280;
		m_cpScreen.x = 1400;
		m_cpScreen.y = 1050;
		m_cOffset.x = (g_Option.m_nResWidth-m_cpScreen.x) / 2;
		m_cOffset.y = (g_Option.m_nResHeight-m_cpScreen.y) / 2;
		break;
	case 1440:
		m_fRate = (FLOAT)1440 / (FLOAT)1280;
		m_cpScreen.x = 1200;
		m_cpScreen.y = 900;
		m_cOffset.x = (g_Option.m_nResWidth-m_cpScreen.x) / 2;
		m_cOffset.y = (g_Option.m_nResHeight-m_cpScreen.y) / 2;
		break;
	case 1600:
		m_fRate = (FLOAT)1600 / (FLOAT)1280;
		m_cpScreen.x = 1600;
		m_cpScreen.y = 1200;
		m_cOffset.x = (g_Option.m_nResWidth-m_cpScreen.x) / 2;
		m_cOffset.y = (g_Option.m_nResHeight-m_cpScreen.y) / 2;
		break;
	case 1680:
		m_fRate = (FLOAT)1680 / (FLOAT)1280;
		m_cpScreen.x = 1400;
		m_cpScreen.y = 1050;
		m_cOffset.x = (g_Option.m_nResWidth-m_cpScreen.x) / 2;
		m_cOffset.y = (g_Option.m_nResHeight-m_cpScreen.y) / 2;
		break;
#endif //__CSC_VER9_RESOLUTION	
	}

#if __VER >= 14 // __NEW_CONTINENT
	m_fRate = (FLOAT)g_Option.m_nResHeight / (FLOAT)1024;
#endif //__NEW_CONTINENT

	m_cPos.y = (LONG)( 80.0f * m_fRate );
	
	FLOAT fGap = 0;
#if __VER >= 9 // __CSC_VER9_RESOLUTION
	int wideOffsetX = 0;
	if((g_Option.m_nResWidth == 1360 || g_Option.m_nResWidth == 1440 || g_Option.m_nResWidth == 1680) || 
		(g_Option.m_nResWidth == 1280 && (g_Option.m_nResHeight == 720 || g_Option.m_nResHeight == 768 || g_Option.m_nResHeight == 800))) //Wide Offset
		wideOffsetX = m_cOffset.x;

	CTexture *pTexture = m_texMapButton.GetAt(0);
	CPoint cp = CPoint( m_cPos.x, m_cPos.y );
	m_cRect[0].SetRect( cp.x + wideOffsetX, cp.y, (int)( cp.x + ((FLOAT)pTexture->m_size.cx * m_fRate) + wideOffsetX ), (int)( cp.y+((FLOAT)pTexture->m_size.cy * m_fRate) ) );
	
	pTexture	= m_texMapButton.GetAt(3);
	cp.y		= (LONG)( cp.y + ((FLOAT)pTexture->m_size.cy * m_fRate) + fGap );
	m_cRect[1].SetRect( cp.x + wideOffsetX, cp.y, (int)( cp.x + ((FLOAT)pTexture->m_size.cx * m_fRate) + wideOffsetX ), (int)( cp.y+((FLOAT)pTexture->m_size.cy * m_fRate) ) );
	
	pTexture	= m_texMapButton.GetAt(6);
	cp.y		= (LONG)( cp.y + ((FLOAT)pTexture->m_size.cy * m_fRate) + fGap );
	m_cRect[2].SetRect( cp.x + wideOffsetX, cp.y, (int)( cp.x + ((FLOAT)pTexture->m_size.cx * m_fRate) + wideOffsetX ), (int)( cp.y+((FLOAT)pTexture->m_size.cy * m_fRate) ) );
	
	cp.y	= (LONG)( cp.y + ((FLOAT)pTexture->m_size.cy * m_fRate) + fGap );
	pTexture = m_texMapButton.GetAt(9);
	m_cRect[3].SetRect( cp.x + wideOffsetX, cp.y, (int)( cp.x + ((FLOAT)pTexture->m_size.cx * m_fRate) + wideOffsetX ), (int)( cp.y+((FLOAT)pTexture->m_size.cy * m_fRate) ) );
	
	cp.y	= (LONG)( cp.y + ((FLOAT)pTexture->m_size.cy * m_fRate) + fGap );
	pTexture = m_texMapButton.GetAt(12);
	m_cRect[4].SetRect( cp.x + wideOffsetX, cp.y, (int)( cp.x + ((FLOAT)pTexture->m_size.cx * m_fRate) + wideOffsetX ), (int)( cp.y+((FLOAT)pTexture->m_size.cy * m_fRate) ) );
	
	cp.y	= (LONG)( cp.y + ((FLOAT)pTexture->m_size.cy * m_fRate) + fGap );
	pTexture = m_texMapButton.GetAt(15);
	m_cRect[5].SetRect( cp.x + wideOffsetX, cp.y, (int)( cp.x + ((FLOAT)pTexture->m_size.cx * m_fRate) + wideOffsetX ), (int)( cp.y+((FLOAT)pTexture->m_size.cy * m_fRate) ) );

#if __VER >= 13 // __CSC_VER13_1
	cp.y	= (LONG)( cp.y + ((FLOAT)pTexture->m_size.cy * m_fRate) + fGap );
	pTexture = m_texMapButton.GetAt(18);
	m_cRect[6].SetRect( cp.x + wideOffsetX, cp.y, (int)( cp.x + ((FLOAT)pTexture->m_size.cx * m_fRate) + wideOffsetX ), (int)( cp.y+((FLOAT)pTexture->m_size.cy * m_fRate) ) );
#if __VER >= 14 // __NEW_CONTINENT
	cp.y	= (LONG)( cp.y + ((FLOAT)pTexture->m_size.cy * m_fRate) + fGap );
	pTexture = m_texMapButton.GetAt(21);
	m_cRect[7].SetRect( cp.x + wideOffsetX, cp.y, (int)( cp.x + ((FLOAT)pTexture->m_size.cx * m_fRate) + wideOffsetX ), (int)( cp.y+((FLOAT)pTexture->m_size.cy * m_fRate) ) );
#endif //__NEW_CONTINENT

#if __VER >= 15 // __NEW_CONTINENT15
	cp.y	= (LONG)( cp.y + ((FLOAT)pTexture->m_size.cy * m_fRate) + fGap );
	pTexture = m_texMapButton.GetAt(24);
	m_cRect[8].SetRect( cp.x + wideOffsetX, cp.y, (int)( cp.x + ((FLOAT)pTexture->m_size.cx * m_fRate) + wideOffsetX ), (int)( cp.y+((FLOAT)pTexture->m_size.cy * m_fRate) ) );
#endif // __NEW_CONTINENT15

#endif //__CSC_VER13_1
#else //__CSC_VER9_RESOLUTION
	CTexture *pTexture = m_texMapButton.GetAt(0);
	CPoint cp = CPoint( m_cPos.x, m_cPos.y );
	m_cRect[0].SetRect( cp.x, cp.y, cp.x + ((FLOAT)pTexture->m_size.cx * m_fRate), cp.y+((FLOAT)pTexture->m_size.cy * m_fRate) );
	
	pTexture = m_texMapButton.GetAt(3);
	cp.y += ((FLOAT)pTexture->m_size.cy * m_fRate) + fGap;
	m_cRect[1].SetRect( cp.x, cp.y, cp.x + ((FLOAT)pTexture->m_size.cx * m_fRate), cp.y+((FLOAT)pTexture->m_size.cy * m_fRate) );
	
	pTexture = m_texMapButton.GetAt(6);
	cp.y += ((FLOAT)pTexture->m_size.cy * m_fRate) + fGap;
	m_cRect[2].SetRect( cp.x, cp.y, cp.x + ((FLOAT)pTexture->m_size.cx * m_fRate), cp.y+((FLOAT)pTexture->m_size.cy * m_fRate) );	

	cp.y += ((FLOAT)pTexture->m_size.cy * m_fRate) + fGap;
	pTexture = m_texMapButton.GetAt(9);
	m_cRect[3].SetRect( cp.x, cp.y, cp.x + ((FLOAT)pTexture->m_size.cx * m_fRate), cp.y+((FLOAT)pTexture->m_size.cy * m_fRate) );
	
	cp.y += ((FLOAT)pTexture->m_size.cy * m_fRate) + fGap;
	pTexture = m_texMapButton.GetAt(12);
	m_cRect[4].SetRect( cp.x, cp.y, cp.x + ((FLOAT)pTexture->m_size.cx * m_fRate), cp.y+((FLOAT)pTexture->m_size.cy * m_fRate) );
	
	cp.y += ((FLOAT)pTexture->m_size.cy * m_fRate) + fGap;
	pTexture = m_texMapButton.GetAt(15);
	m_cRect[5].SetRect( cp.x, cp.y, cp.x + ((FLOAT)pTexture->m_size.cx * m_fRate), cp.y+((FLOAT)pTexture->m_size.cy * m_fRate) );
#endif //__CSC_VER9_RESOLUTION
	m_nDrawMenu[0] = 0;
	m_nDrawMenu[1] = 3;
	m_nDrawMenu[2] = 6;
	m_nDrawMenu[3] = 9;
	m_nDrawMenu[4] = 12;
	m_nDrawMenu[5] = 15;
#if __VER >= 13 // __CSC_VER13_1
	m_nDrawMenu[6] = 18;
#if __VER >= 14 // __NEW_CONTINENT
	m_nDrawMenu[7] = 21;
#endif //__NEW_CONTINENT

#if __VER >= 15 // __NEW_CONTINENT15
	m_nDrawMenu[8] = 24;
#endif

#endif //__CSC_VER13_1
	m_bRender = FALSE;
}

void CWorldMap::Process()
{
	m_nDrawMenu[0] = 0;
	m_nDrawMenu[1] = 3;
	m_nDrawMenu[2] = 6;
	m_nDrawMenu[3] = 9;
	m_nDrawMenu[4] = 12;
	m_nDrawMenu[5] = 15;
#if __VER >= 13 // __CSC_VER13_1
	m_nDrawMenu[6] = 18;
#if __VER >= 14 // __NEW_CONTINENT
	m_nDrawMenu[7] = 21;
#endif //__NEW_CONTINENT

#if __VER >= 15 // __NEW_CONTINENT15
	m_nDrawMenu[8] = 24;
#endif //__NEW_CONTINENT15

#endif //__CSC_VER13_1
	
	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetApplet( APP_WORLD );
	if( pWndWorld )
	{
		CPoint point = pWndWorld->GetMousePoint();

#ifndef __IMPROVE15_WORLDMAP

#if __VER >= 13 // __CSC_VER13_1
		for( int i = 0 ; i < MAX_BUTTON ; ++i )
#else //__CSC_VER13_1
		for( int i = 0 ; i < 6 ; ++i )
#endif //__CSC_VER13_1
		{
			if( m_nMap != i && m_cRect[i].PtInRect( point ) )
			{
				m_nDrawMenu[i] = m_nDrawMenu[i] + 1;
			}
		}

#endif // __IMPROVE15_WORLDMAP

#if __VER >= 13 // __CSC_VER13_1
		if(m_bShowMonsterInfo && this->IsRender())
		{
			BOOL bSel = FALSE;
			for(int i=0; i<(int)( m_MonsterInfo.GetNumber() ); i++)
			{
				__MONSTER_INFO stMonsterInfo = m_MonsterInfo.m_vecMonsterInfo[i];
				if(stMonsterInfo.m_rectPos.PtInRect(point))
				{
					CEditString strEdit;
					CString strTemp;

					for(int j=0; j<stMonsterInfo.m_nMonCnt; j++)
					{
						MoverProp* pMoverProp = prj.GetMoverProp(stMonsterInfo.m_dwMonsterId[j]);
					
						if(pMoverProp)
						{
							strTemp.Format( prj.GetText( TID_GAME_MONSTER_INFORMATION_LEVEL ) );
							strEdit.AddString(strTemp, D3DCOLOR_XRGB(0, 0, 255));
							strTemp.Format("%d ", pMoverProp->dwLevel);
							strEdit.AddString(strTemp, D3DCOLOR_XRGB(130, 130, 200));
							strTemp.Format("%s", pMoverProp->szName);

							if(j+1 == stMonsterInfo.m_nMonCnt) //거대만 빨간색으로 표시
								strEdit.AddString(strTemp, D3DCOLOR_XRGB(255, 0, 0));
							else
								strEdit.AddString(strTemp);

							strEdit.AddString("\n");

							for(int k=0; k<(int)( pMoverProp->m_DropItemGenerator.GetSize() ); k++)
							{
								DROPITEM* pDropItem = pMoverProp->m_DropItemGenerator.GetAt(k, FALSE);
							}
						}
					}
					strEdit.AddString("\n");
					ItemProp* pItemProp = prj.GetItemProp( stMonsterInfo.m_dwDropItemId );

					strTemp.Format("%s : ", prj.GetText(TID_GAME_DROP_ITEM));
					strEdit.AddString(strTemp);

					if(pItemProp)
					{
						strTemp.Format("%s", pItemProp->szName);
						strEdit.AddString(strTemp, D3DCOLOR_XRGB(46, 112, 169));
					}
					else
						strEdit.AddString(prj.GetText(TID_GAME_DROP_NONE));
					
					g_toolTip.PutToolTip(10000, strEdit, stMonsterInfo.m_rectPos, point, 0);
#ifndef __IMPROVE_MAP_SYSTEM
					g_toolTip.SetWorldMapMonsterInfo(stMonsterInfo.m_nMonCnt, stMonsterInfo.m_dwMonsterId);
#endif // __IMPROVE_MAP_SYSTEM

					m_nSelMon = i;
					bSel = TRUE;
				}
			}

			if(!bSel)
				m_nSelMon = -1;
		}
#endif //__CSC_VER13_1
	}
}
void CWorldMap::RenderPlayer( C2DRender *p2DRender, BOOL bMyPlayer, D3DXVECTOR3 vPos, const TCHAR* szName )
{
#if __VER >= 14 // __NEW_CONTINENT
	int nMap = CContinent::GetInstance()->GetMapNo( vPos );
#else // __NEW_CONTINENT
	int nMap = GetMapNo( vPos );
#endif // __NEW_CONTINENT
					
	if( m_nMap !=0 && m_nMap != nMap )
		return;
	// 주인공 화살표 출력
	// 화면 비율 때문에 임의로 정사각형 뷰포트를 지정해 놓는다. 안그러면 화살표 모양이 찌그러짐.
#if __VER >= 9 // __CSC_VER9_RESOLUTION
	D3DVIEWPORT9 viewport;
	if((g_Option.m_nResWidth == 1360 || g_Option.m_nResWidth == 1440 || g_Option.m_nResWidth == 1680) || 
		(g_Option.m_nResWidth == 1280 && (g_Option.m_nResHeight == 720 || g_Option.m_nResHeight == 768 || g_Option.m_nResHeight == 800))) //Wide Offset
	{
		viewport.X      = m_cOffset.x;
		viewport.Y      = 0;
		viewport.Width  = m_cpScreen.x;
		viewport.Height = m_cpScreen.y;
		viewport.MinZ   = 0.0f;
		viewport.MaxZ   = 1.0f;
	}
	else
	{
		viewport.X      = 0;
		viewport.Y      = 0;
		viewport.Width  = g_Option.m_nResWidth;
		viewport.Height = g_Option.m_nResHeight;
		viewport.MinZ   = 0.0f;
		viewport.MaxZ   = 1.0f;
	}
#else //__CSC_VER9_RESOLUTION
	D3DVIEWPORT9 viewport;
	viewport.X      = 0;
	viewport.Y      = 0;
	viewport.Width  = g_Option.m_nResWidth;
	viewport.Height = g_Option.m_nResHeight;
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;
#endif //__CSC_VER9_RESOLUTION
	D3DDEVICE->SetViewport( &viewport );

	// 프로젝션 
	D3DXMATRIX matProj;
	D3DXMatrixIdentity( &matProj );
	D3DDEVICE->SetTransform( D3DTS_PROJECTION, &matProj );
	// 카메라 
	D3DXMATRIX  matView;
	D3DXMatrixIdentity( &matView );
	D3DDEVICE->SetTransform( D3DTS_VIEW, &matView );

	D3DXMATRIX mat, matWorld;
	D3DXMatrixIdentity( &matWorld );

	D3DXMatrixScaling( &mat, 0.07f, 0.07f, 0.07f );
	D3DXMatrixMultiply( &matWorld, &matWorld, &mat );

	D3DXVECTOR3 vDir      = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
	D3DXVECTOR3 vDestNor;
	if( bMyPlayer )
		vDestNor  = vPos - g_Neuz.m_camera.m_vPos;
	else
		vDestNor  = vPos;
	D3DXVECTOR3 vAxis;
	D3DXQUATERNION   qRot;
	
	FLOAT       fTheta;
	D3DXVec3Normalize( &vDestNor, &vDestNor );
	D3DXVec3Cross( &vAxis, &vDir, &vDestNor );
	fTheta = D3DXVec3Dot( &vDir, &vDestNor );
	D3DXQuaternionRotationAxis( &qRot, &vAxis, acosf( fTheta ) );
	
	D3DXVECTOR3 vYPW;
	
	QuaternionRotationToYPW( qRot, vYPW );
	D3DXMatrixRotationZ( &mat, -(vYPW.y) );
			
	D3DXMatrixMultiply( &matWorld, &matWorld, &mat );

	FLOAT fMap_X1 = _fMap_X1;
	FLOAT fMap_Y1 = _fMap_Y1;
	FLOAT fMap_X2 = _fMap_X2;
	FLOAT fMap_Y2 = _fMap_Y2;

#if __VER >= 14 // __NEW_CONTINENT
	if( m_nMap == 0 )
#else __NEW_CONTINENT
	if( m_strViewMapString == m_strMapStringList[0] )
#endif // __NEW_CONTINENT
	{
		fMap_X1 = 0.0f;
		fMap_Y1 = 1298.0f;//0.0f;
		fMap_X2 = 11800.0f;
		fMap_Y2 = 9757.0f; //9000.0f;		// DDS 의 전체크기 1280,960을 실제 크기에 대한 사이길이 ( 상단 123, 하단 30 )를 구한후 실제크기에 해당하는 
											// 월드 실제좌표를 구해서( 8662, 1770 ) 사이길이를 실제크기로 환산한후 각 좌표에 더하고( 상단 ) 빼준다( 하단 )
	}
	else
#if __VER >= 14 // __NEW_CONTINENT
	if( m_nMap == 1 )
#else __NEW_CONTINENT
	if( m_strViewMapString == m_strMapStringList[1] ) // 다콘 1, 2
#endif // __NEW_CONTINENT
	{
		fMap_X1 = 2355.0f;
		fMap_Y1 = 2019.0f;
		fMap_X2 = 7187.0f;
		fMap_Y2 = 5814.0f;
	}
	else
#if __VER >= 14 // __NEW_CONTINENT
	if( m_nMap == 2 )
#else __NEW_CONTINENT
	if( m_strViewMapString == m_strMapStringList[2] ) // 다콘 3
#endif // __NEW_CONTINENT
	{
		fMap_X1 = 985.0f;
		fMap_Y1 = 1621.0f;
		fMap_X2 = 5754.0f;
		fMap_Y2 = 5149.0f;
	}		
	else
#if __VER >= 14 // __NEW_CONTINENT
	if( m_nMap == 3 )
#else __NEW_CONTINENT
	if( m_strViewMapString == m_strMapStringList[3] ) // 리시스
#endif // __NEW_CONTINENT
	{
		fMap_X1 = 8080.0f;
		fMap_Y1 = 3200.0f;
		fMap_X2 = 10280.0f;
		fMap_Y2 = 5000.0f;
	}
	else
#if __VER >= 14 // __NEW_CONTINENT
	if( m_nMap == 4 )
#else __NEW_CONTINENT
	if( m_strViewMapString == m_strMapStringList[4] ) // 세인트모닝
#endif // __NEW_CONTINENT
	{
		fMap_X1 = 6854.0f;
		fMap_Y1 = 1580.0f;
		fMap_X2 = 10600.0f;
		fMap_Y2 = 4430.0f;
	}
	else
#if __VER >= 14 // __NEW_CONTINENT
	if( m_nMap == 5 )
#else __NEW_CONTINENT
	if( m_strViewMapString == m_strMapStringList[5] ) // 플라리스
#endif // __NEW_CONTINENT
	{
		fMap_X1 = 5500.0f;
		fMap_Y1 = 2540.0f;
		fMap_X2 = 9200.0f;
		fMap_Y2 = 5130.0f;
	}
#if __VER >= 14 // __NEW_CONTINENT
	else
	if( m_nMap == 6 ) // 하르모닌
	{
		fMap_X1 = 4795.0f;		// ui맵의 테두리 대 대륙사이즈 비율을 이용해 실제 월드대륙좌표에 적용시킨 ui의 테투리에 해당하는 월드 좌표
		fMap_Y1 = 4587.0f;
		fMap_X2 = 8391.0f;
		fMap_Y2 = 7236.0f;
	}
#endif //__NEW_CONTINENT
#if __VER >= 15 // __NEW_CONTINENT15
	else 
	if( m_nMap == 7 ) // 이스티아 캐니언
	{
		fMap_X1 = 5150.0f;
		fMap_Y1 = 5573.0f;
		fMap_X2 = 9890.0f;
		fMap_Y2 = 9158.0f;
	}
#endif
	
	FLOAT fX = fMap_X2 - fMap_X1;
	FLOAT fY = fMap_Y1 - fMap_Y2;

	D3DXVECTOR3 v1 = D3DXVECTOR3( -1, 1, -1 );
	D3DXVECTOR3 v2 = D3DXVECTOR3( 1, -1, 1 );
	D3DXVECTOR3 v3 = D3DXVECTOR3( 1, 1, 1 ); 
	
	FLOAT fRate1 = (fMap_X2 - vPos.x) / fX ;
	FLOAT fRate2 = (fMap_Y1 - vPos.z) / fY ;

	D3DXVec3Lerp( &v3, &v1, &v2, fRate1 );
	
	matWorld._41 = -v3.x;

	D3DXVec3Lerp( &v3, &v1, &v2, fRate2 );
	
	matWorld._42 = -v3.y;	

	D3DDEVICE->SetTransform( D3DTS_WORLD, &matWorld );

	D3DDEVICE->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );		
	D3DDEVICE->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );		

	// 화살표 출력 
	if( bMyPlayer )
		m_billArrow[0].Render( D3DDEVICE );
	else
		m_billArrow[1].Render( D3DDEVICE );	

	{
		int nX;
		int nY;
#if __VER >= 9 // __CSC_VER9_RESOLUTION
	if((g_Option.m_nResWidth == 1360 || g_Option.m_nResWidth == 1440 || g_Option.m_nResWidth == 1680) || 
		(g_Option.m_nResWidth == 1280 && (g_Option.m_nResHeight == 720 || g_Option.m_nResHeight == 768 || g_Option.m_nResHeight == 800))) //Wide Offset
	{
		v1 = D3DXVECTOR3( (FLOAT)( m_cOffset.x ), 0.0f, 0.0f );
		v2 = D3DXVECTOR3( (FLOAT)( m_cpScreen.x + m_cOffset.x ), (FLOAT)( m_cpScreen.y ), 0.0f );
	}
	else
	{
		v1 = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		v2 = D3DXVECTOR3( (FLOAT)( g_Option.m_nResWidth ), (FLOAT)( g_Option.m_nResHeight ), 0.0f );
	}
#else //__CSC_VER9_RESOLUTION
		v1 = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		v2 = D3DXVECTOR3( (FLOAT)( g_Option.m_nResWidth ), (FLOAT)( g_Option.m_nResHeight ), 0.0f );
#endif //__CSC_VER9_RESOLUTION
		D3DXVec3Lerp( &v3, &v2, &v1, fRate1 );
		nX = (int)( v3.x );
		D3DXVec3Lerp( &v3, &v2, &v1, fRate2 );
		nY = (int)( v3.y );

		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont( CWndBase::m_Theme.m_pFontWorld );

		CSize cPos = p2DRender->GetFont()->GetTextExtent( szName );

		nX -= cPos.cx / 2;
		nY -= (cPos.cy + (cPos.cy / 2));

// sun!!
//		nX *= 0.5f;
//		nY *= 0.5f;

		if( bMyPlayer )			
			p2DRender->TextOut( nX, nY, szName, 0xff7fff7f );	
		else
			p2DRender->TextOut( nX, nY, szName, 0xffffff7f );	
		
		p2DRender->SetFont( pOldFont );
	}	
}
void CWorldMap::RenderWorldMap( C2DRender *p2DRender )
{
	if( m_bRender )
	{	
#if __VER >= 9 // __CSC_VER9_RESOLUTION
		if((g_Option.m_nResWidth == 1360 || g_Option.m_nResWidth == 1440 || g_Option.m_nResWidth == 1680) || 
			(g_Option.m_nResWidth == 1280 && (g_Option.m_nResHeight == 720 || g_Option.m_nResHeight == 768 || g_Option.m_nResHeight == 800))) //Wide Background Black
			p2DRender->RenderFillRect( CRect( 0, 0, g_Option.m_nResWidth, g_Option.m_nResHeight ), 0xff000000 );
		else
			p2DRender->RenderFillRect( CRect( 0, 0, g_Option.m_nResWidth, g_Option.m_nResHeight ), 0xff3B3D2F );
#else //__CSC_VER9_RESOLUTION
		p2DRender->RenderFillRect( CRect( 0, 0, g_Option.m_nResWidth, g_Option.m_nResHeight ), 0xff3B3D2F );
#endif //__CSC_VER9_RESOLUTION
				
 		p2DRender->RenderTextureEx( CPoint(m_cOffset.x, m_cOffset.y), m_cpScreen, m_pTexWorldMap, 255, 1.0f, 1.0f );

#if __VER >= 15 // __TELEPORTER
		if( m_bTeleportMode )
			RenderTelPos( p2DRender );
#endif
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
		if( m_bDestinationMode == TRUE )
			RenderDestinationPosition( p2DRender );
#endif // __IMPROVE_QUEST_INTERFACE
		RenderPlayer( p2DRender, TRUE, g_pPlayer->GetPos( ) , g_pPlayer->GetName() );
		
		for( int i = 0 ; i < g_Party.GetSizeofMember() ; ++i )
		{
			if( g_pPlayer->m_idPlayer != g_Party.GetPlayerId( i ) )
			{
				if( !g_Party.m_aMember[ i ].m_bRemove )
#if __VER >= 11 // __SYS_PLAYER_DATA
					RenderPlayer( p2DRender, FALSE, g_Party.GetPos( i ), CPlayerDataCenter::GetInstance()->GetPlayerString( g_Party.m_aMember[ i ].m_uPlayerId ) );	
#else	// __SYS_PLAYER_DATA
					RenderPlayer( p2DRender, FALSE, g_Party.GetPos( i ), g_Party.m_aMember[ i ].m_szName );	
#endif	// __SYS_PLAYER_DATA
			}
		}

		FLOAT fGap = 0;
		CPoint cp;
		
		m_nDrawMenu[m_nMap] = m_nDrawMenu[m_nMap] + 2;

		CTexture *pTexture;

#ifndef __IMPROVE15_WORLDMAP		// 대륙 이미지 버튼 제거 

#if __VER >= 13 // __CSC_VER13_1
		for( i = 0; i < MAX_BUTTON; ++i )
#else //__CSC_VER13_1
		for( i = 0; i < 6; ++i )
#endif //__CSC_VER13_1
		{
			pTexture = m_texMapButton.GetAt(m_nDrawMenu[i]);

			if( pTexture )
			{
				cp.x = m_cRect[i].left;
				cp.y = m_cRect[i].top;
				pTexture->RenderScal( p2DRender, cp, 255, m_fRate, m_fRate );
			}
		}
#endif // __IMPROVE15_WORLDMAP

#if __VER >= 13 // __CSC_VER13_1
		if(m_bShowMonsterInfo && m_nMap > 0)
		{
			for(i=0; i<(int)( m_MonsterInfo.GetNumber() ); i++)
			{
				pTexture = m_MonsterInfo.GetAt(i);

				if( pTexture )
				{
					__MONSTER_INFO stMonsterInfo = m_MonsterInfo.m_vecMonsterInfo[i];
					cp.x = stMonsterInfo.m_rectPos.left;
					cp.y = stMonsterInfo.m_rectPos.top;

					if(m_nSelMon == i)
						pTexture->RenderScal( p2DRender, cp, 255, m_fRate, m_fRate );
					else
						pTexture->RenderScal( p2DRender, cp, 150, m_fRate, m_fRate );
				}
			}

			if(m_nSelMon > -1)
				g_toolTip.Paint(p2DRender);
		}
#endif //__CSC_VER13_1
#if __VER >= 13 // __RAINBOW_RACE
		DWORD dwRainbowRaceTime = CRainbowRace::GetInstance()->m_dwRemainTime;
		if(dwRainbowRaceTime > 0)
		{
			for(i=0; i<(int)( m_RainbowNPC.GetNumber() ); i++)
			{
				__RAINBOW_NPC stRainbowNPC = m_RainbowNPC.m_vecRainbowNPC[i];
				if(m_nMap == 0)
				{
					pTexture = m_RainbowNPC.GetAt(i);
					if(pTexture)
					{
						cp.x = stRainbowNPC.m_rectTotalMapPos.left;
						cp.y = stRainbowNPC.m_rectTotalMapPos.top;

						pTexture->RenderScal( p2DRender, cp, 255, m_fRate, m_fRate );
					}
				}
				else
				{
					if(stRainbowNPC.m_nMap == m_nMap)
					{
						pTexture = m_RainbowNPC.GetAt(i);
						if(pTexture)
						{
							cp.x = stRainbowNPC.m_rectPos.left;
							cp.y = stRainbowNPC.m_rectPos.top;

							pTexture->RenderScal( p2DRender, cp, 255, m_fRate, m_fRate );
						}
					}
				}
			}
		}
#endif //__RAINBOW_RACE

#ifdef __IMPROVE15_WORLDMAP
		CPoint oldPoint = p2DRender->m_ptOrigin;
		CRectClip  oldClipRect = p2DRender->m_clipRect;
		m_pWndList->Paint( p2DRender );

		p2DRender->m_ptOrigin = oldPoint;
		p2DRender->m_clipRect = oldClipRect;
#endif
	}
}

BOOL CWorldMap::LoadWorldMap()
{
	// 전체 맵중에 내가 현재 어디에 있는지를 검사한다.
#if __VER >= 14 // __NEW_CONTINENT
	int nMap = CContinent::GetInstance()->GetMapNo( g_pPlayer );
#else // __NEW_CONTINENT
	int nMap = GetMapNo( g_pPlayer->GetPos() );
#endif // __NEW_CONTINENT

#ifdef __IMPROVE15_WORLDMAP
	m_pWndList = new CWndListBox;
	CRect rect( 50, 200, 200, 600 );
	((CWndStatic*)m_pWndList)->m_strTexture = CString("WndEditTile00.tga");
	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
	m_pWndList->Create( WBS_CHILD, rect, pWndWorld, 0 );

//	for( int i = 0; i < MAX_BUTTON; ++i )
//		m_pWndList->AddString( CContinent::GetInstance()->GetContinentName( i ).GetBuffer( 0 ) );

	//대륙 인덱스 작업이 안돼있어서 걍 일단 테스트로..
	m_pWndList->AddString( "WORLD" );
	m_pWndList->AddString( "Darkon1,2" );
	m_pWndList->AddString( "Darkon3" );
	m_pWndList->AddString( "Ricis" );
	m_pWndList->AddString( "Saint Morning" );
	m_pWndList->AddString( "Flaris" );
	m_pWndList->AddString( "Harmonin" );
	m_pWndList->AddString( "Estia" );
	m_pWndList->AddString( "MONSTER" );

#endif //__IMPROVE15_WORLDMAP

#if __VER >= 15 // __TELEPORTER
	if( m_bTeleportMode )
		nMap = CONT_NODATA;
#endif
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	if( m_bDestinationMode == TRUE )
		nMap = CONT_NODATA;
#endif // __IMPROVE_QUEST_INTERFACE

	if( nMap != 100 )
	{
		m_strViewMapString = m_strMapStringList[nMap];
		m_pTexWorldMap = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( "Theme\\", ::GetLanguage(), m_strViewMapString ), 0xff000000 );

#if __VER >= 13 // __CSC_VER13_1
#if __VER >= 14 // __NEW_CONTINENT
		if(nMap > 0 && nMap < MAX_BUTTON - 1 && m_MonsterInfo.m_nMap != nMap)
#else // __NEW_CONTINENT
		if(nMap > 0 && nMap < 6 && m_MonsterInfo.m_nMap != nMap)
#endif // __NEW_CONTINENT
		{
			m_MonsterInfo.DeleteDeviceObjects();
			m_MonsterInfo.LoadScript( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, _T( m_strMonScript[nMap-1] ) ), nMap );	
		}
#endif //__CSC_VER13_1

#if __VER >= 13 // __RAINBOW_RACE
		m_RainbowNPC.LoadScript( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, _T( "texMapRainbow_NPC.inc" ) ) );
#endif //__RAINBOW_RACE

		if( m_pTexWorldMap )
		{
			m_nMap = nMap;
			m_bRender = TRUE;
			return TRUE;
		}
	}
	else
	{
		m_strViewMapString.Empty();
#if __VER >= 9  // __INSERT_MAP
		g_WndMng.PutString( prj.GetText(TID_GAME_MAPMSG), NULL, prj.GetTextColor(TID_GAME_MAPMSG) );
#endif
	}
	return FALSE;
}

#if __VER < 14 // __NEW_CONTINENT
int CWorldMap::GetMapNo( D3DXVECTOR3 vPos )
{
	BOOL bResult = 100;
	CString str;
	CPoint  cpoint;
	vector<CPoint>	vecPoint;
	
	cpoint.x    = vPos.x;
	cpoint.y    = vPos.z;
	
	for( multimap< CString, vector<CPoint> >::iterator iter = m_mapView.begin(); iter != m_mapView.end(); ++iter )
	{
		str		  = iter->first;
		vecPoint  = iter->second;	
		if( Point_In_Poly( vecPoint, cpoint ) )
		{
			if( str == CString( "Darkon12" ) )
				bResult = 1;
			else 
			if( str == CString( "Darkon3" ) )
				bResult = 2;
			else 
			if( str == CString( "Ricis" ) )
				bResult = 3;
			else
			if( str == CString( "Saint" ) )
				bResult = 4;
			else
			if( str == CString( "Flaris" ) )
				bResult = 5;
#if __VER >= 14 // __NEW_CONTINENT
			else
			if( str == CString( "Harmonin" ) )
				bResult = 6;
#endif //__NEW_CONTINENT

#if __VER >= 15 // __NEW_CONTINENT15
			else
			if( str == CString( "Estia" ) )
				bResult = 7;
#endif //__NEW_CONTINENT15

		}
	}

	return bResult;
}
#endif // __NEW_CONTINENT

void CWorldMap::DeleteWorldMap()
{
//	m_pTexWorldMap->DeleteDeviceObjects();
	m_bRender = FALSE;
#if __VER >= 15 // __TELEPORTER
	m_bTeleportMode = FALSE;
	m_idTeleporter = NULL_ID;
#endif
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	m_bDestinationMode = FALSE;
#endif // __IMPROVE_QUEST_INTERFACE

#ifdef __IMPROVE15_WORLDMAP
	SAFE_DELETE( m_pWndList );
#endif
}

void CWorldMap::DeleteDeviceObjects()
{
//	m_pTexWorldMap->DeleteDeviceObjects();
	m_texArrow[0].DeleteDeviceObjects();
	m_texArrow[1].DeleteDeviceObjects();
	m_billArrow[0].DeleteDeviceObjects();
	m_billArrow[1].DeleteDeviceObjects();
	m_texMapButton.DeleteDeviceObjects();
#if __VER >= 13 // __CSC_VER13_1
	m_MonsterInfo.DeleteDeviceObjects();
#endif //__CSC_VER13_1
#if __VER >= 13 // __RAINBOW_RACE
	m_RainbowNPC.DeleteDeviceObjects();
#endif //__RAINBOW_RACE
}

void CWorldMap::RestoreDeviceObjects()
{
//	m_pTexWorldMap->DeleteDeviceObjects();
	m_texArrow[0].DeleteDeviceObjects();
	m_billArrow[0].DeleteDeviceObjects();
	m_texArrow[1].DeleteDeviceObjects();
	m_billArrow[1].DeleteDeviceObjects();
	m_texMapButton.RestoreDeviceObjects(g_Neuz.m_pd3dDevice);
#if __VER >= 13 // __CSC_VER13_1
	m_MonsterInfo.RestoreDeviceObjects(g_Neuz.m_pd3dDevice);
#endif //__CSC_VER13_1
#if __VER >= 13 // __RAINBOW_RACE
	m_RainbowNPC.RestoreDeviceObjects(g_Neuz.m_pd3dDevice);
#endif //__RAINBOW_RACE

}

void CWorldMap::InvalidateDeviceObjects()
{
	m_billArrow[0].InvalidateDeviceObjects();
	m_billArrow[1].InvalidateDeviceObjects();
	m_texMapButton.InvalidateDeviceObjects();
#if __VER >= 13 // __CSC_VER13_1
	m_MonsterInfo.InvalidateDeviceObjects();
#endif //__CSC_VER13_1
#if __VER >= 13 // __RAINBOW_RACE
	m_RainbowNPC.InvalidateDeviceObjects();
#endif //__RAINBOW_RACE
}

void CWorldMap::OnLButtonDown( )
{
	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetApplet( APP_WORLD );
	if( !pWndWorld )		
		return;

#if __VER >= 15 // __TELEPORTER
	if( m_bTeleportMode )
	{
		CPoint pt = pWndWorld->GetMousePoint( );
		int max_size = m_cTelPosRects.size();
		for( int i = 0; i < max_size; ++i )
		{
			WMapIcon& kIcon = m_cTelPosRects[ i ];
			CRect& rect = kIcon._rect;
			if( rect.PtInRect( pt ) )
			{
				// 해당 포지션 버튼을 눌렀다.
				CMover* pMover = prj.GetMover( m_idTeleporter );
				if( !pMover )
					return;

				PLAYSND( SND_INF_CLICK );
				g_DPlay.SendTeleporterReq( pMover->m_szCharacterKey, kIcon._index );
				DeleteWorldMap( );
				return;
			}
		}
	}
#endif	//__TELEPORTER

#ifdef __IMPROVE15_WORLDMAP
	m_pWndList->OnLButtonDown( 1, m_pWndList->GetMousePoint() );//WindowProc( message, wParam, lParam ); 
	if( m_pWndList->GetCurSel() < MAX_BUTTON  )
	{
		//몬스터 정보는 선택안되고 찍으면 몬스터 정보만 보여주기 
		int nowMapNum = 0;
		nowMapNum = m_pWndList->GetCurSel();

//		if( nowMapNum == MAX_BUTTON -1 )
//			m_pWndList->SetCurSel( m_nMap );
//		else 
			
		if( nowMapNum > -1 && nowMapNum < MAX_BUTTON - 1 )
		{
			m_nMap = nowMapNum;

#if __VER >= 15 // __TELEPORTER
			UpdateTeleportWorld( );
#endif 
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
			UpdateDestinationPosition( pWndWorld->m_vDestinationArrow );
#endif // __IMPROVE_QUEST_INTERFACE
			m_strViewMapString = m_strMapStringList[nowMapNum];
			m_pTexWorldMap = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( "Theme\\", ::GetLanguage(), m_strViewMapString ), 0xff000000 );

			if(nowMapNum > 0 && m_MonsterInfo.m_nMap != nowMapNum)
			{
				m_MonsterInfo.DeleteDeviceObjects();
				m_MonsterInfo.LoadScript( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, _T( m_strMonScript[nowMapNum-1] ) ), nowMapNum );
			}
		}
		else if( nowMapNum == MAX_BUTTON - 1 )
		{
			m_bShowMonsterInfo = !m_bShowMonsterInfo;
		}
	}
#else

	if( pWndWorld )		
	{
		CPoint point = pWndWorld->GetMousePoint();
		
#if __VER >= 13 // __CSC_VER13_1
		for( int i = 0 ; i < MAX_BUTTON ; ++i )
#else //__CSC_VER13_1
		for( int i = 0 ; i < 6 ; ++i )
#endif //__CSC_VER13_1
		{
			if( m_cRect[i].PtInRect( point ) )
			{
#if __VER >= 13 // __CSC_VER13_1
#if __VER >= 14 // __NEW_CONTINENT
				if(i < MAX_BUTTON - 1)
#else //__NEW_CONTINENT
				if(i < 6)
#endif //__NEW_CONTINENT
				{
					m_nMap = i;

#if __VER >= 15 // __TELEPORTER
					UpdateTeleportWorld( );
#endif 
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
					UpdateDestinationPosition( pWndWorld->m_vDestinationArrow );
#endif // __IMPROVE_QUEST_INTERFACE
					m_strViewMapString = m_strMapStringList[m_nMap];
					m_pTexWorldMap = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( "Theme\\", ::GetLanguage(), m_strViewMapString ), 0xff000000 );

					if(m_nMap > 0 && m_MonsterInfo.m_nMap != m_nMap)
					{
						m_MonsterInfo.DeleteDeviceObjects();
						m_MonsterInfo.LoadScript( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, _T( m_strMonScript[m_nMap-1] ) ), m_nMap );
					}
				}
#if __VER >= 14 // __NEW_CONTINENT
				else if(i == MAX_BUTTON - 1)
#else //__NEW_CONTINENT
				else if(i == 6)
#endif //__NEW_CONTINENT
					m_bShowMonsterInfo = !m_bShowMonsterInfo;
#else //__CSC_VER13_1
				m_nMap = i;
				m_strViewMapString = m_strMapStringList[m_nMap];
				m_pTexWorldMap = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( "Theme\\", ::GetLanguage(), m_strViewMapString ), 0xff000000 );
#endif //__CSC_VER13_1

				break;
			}
		}
	}

#endif //__IMPROVE15_WORLDMAP
}

#if __VER >= 15 // __TELEPORTER
void CWorldMap::UpdateTeleportWorld( )
{
	//해당 텔레포터로부터 얻어온 월드포인트 자료를 대상으로 현재 보고있는 맵월드 아이디에 따라 
	//좌표를 업데이트 시킨다.

	m_cTelPosRects.clear( ); //일단 날려버려 

	CPoint pt;
	CSize size( 16, 16 );
	int max_size = m_cWorldPos.size();
	for( int i = 0; i < max_size; ++i )
	{
		BOOL bOK = WorldPosToMapPos( m_cWorldPos[i], pt );
		if( bOK )
		{
			WMapIcon icon;
			icon._index = i;
			pt.x	= (LONG)( pt.x - ( size.cx * 0.5F ) );
			pt.y	= (LONG)( pt.y - ( size.cy * 0.5F ) );
			icon._rect = CRect( pt, size );
			m_cTelPosRects.push_back( icon );
		}
	}
}

void CWorldMap::ProcessingTeleporter( CMover* pFocusMover )
{
	CHARACTER* pCharacter  = pFocusMover->GetCharacter( );
	if( !pCharacter )
		return;

	if( pCharacter->m_vecTeleportPos.empty( ) )
		return;

	if( m_idTeleporter != pFocusMover->GetId( ) )
	{
		//최초로 불렸거나 다른 텔레포터일경우 
		m_idTeleporter = pFocusMover->GetId( );
		m_cWorldPos = pCharacter->m_vecTeleportPos;

		UpdateTeleportWorld( );
	}
}

void CWorldMap::RenderTelPos( C2DRender *p2DRender )
{
	m_pTelPosTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "ButtTeleport.bmp"), 0xffff00ff );
	if( !m_pTelPosTexture )
		return;

	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetApplet( APP_WORLD );
	if( !pWndWorld )
		return;

	CPoint mousePt = pWndWorld->GetMousePoint();

	CPoint pt;
	for( vector< WMapIcon >::iterator iter = m_cTelPosRects.begin(); iter != m_cTelPosRects.end(); ++iter )
	{
		WMapIcon& kIcon = *iter;
		CRect& rect = kIcon._rect;
		pt.x = rect.left;
		pt.y = rect.top;
		if( rect.PtInRect( mousePt ) )
		{
			// 해당 구역에 마우스가 위치한 경우 선택됐다는 뭐 그런?
			pt.x -= 2;
			pt.y -= 2;
			m_pTelPosTexture->Render( p2DRender, pt, CPoint( 20, 20 ) );
		}else
		{
			m_pTelPosTexture->Render( p2DRender, pt, CPoint( 16, 16) );
		}
	}

}

void CWorldMap::SetTelMode( BOOL bMode )
{
	m_bTeleportMode = bMode;
	m_bRender = m_bTeleportMode;
}

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
void CWorldMap::UpdateDestinationPosition( const D3DXVECTOR3& rPosition )
{
	CPoint pt( 0, 0 );
	CSize size( 16, 16 );
	BOOL bOK = WorldPosToMapPos( rPosition, pt );
	if( bOK )
	{
		pt.x	= (LONG)( pt.x - ( size.cx * 0.5F ) );
		pt.y	= (LONG)( pt.y - ( size.cy * 0.5F ) );
		m_rectDestination = CRect( pt, size );
	}
	else
		m_rectDestination = CRect( -1, -1, -1, -1 );
}

void CWorldMap::RenderDestinationPosition( C2DRender* p2DRender )
{
	if( m_rectDestination == CRect( -1, -1, -1, -1 ) )
		return;
	
	m_pDestinationPositionTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "ButtDestination.bmp"), 0xffff00ff );
	if( m_pDestinationPositionTexture == NULL )
		return;

	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetApplet( APP_WORLD );
	if( !pWndWorld )
		return;

	float fDelta = ( static_cast< float >( g_tmCurrent - m_tmOld ) / 1000.0f );
	m_tmOld = g_tmCurrent;

	static const float ANIMATION_SPEED = 800.0f;
	if( m_bAlphaSwitch == FALSE )
	{
		m_nDestinationTextureAlpha += static_cast< int >( ANIMATION_SPEED * fDelta );
		if( m_nDestinationTextureAlpha > MAXIMUM_ALPHA )
		{
			m_nDestinationTextureAlpha = MAXIMUM_ALPHA;
			m_bAlphaSwitch = TRUE;
		}
	}
	else
	{
		m_nDestinationTextureAlpha -= static_cast< int >( ANIMATION_SPEED * fDelta );
		if( m_nDestinationTextureAlpha < MINIMUM_ALPHA )
		{
			m_nDestinationTextureAlpha = MINIMUM_ALPHA;
			m_bAlphaSwitch = FALSE;
		}
	}
	m_pDestinationPositionTexture->RenderScal( p2DRender, CPoint( m_rectDestination.left, m_rectDestination.top ), m_nDestinationTextureAlpha, 0.6F, 0.6F );
}

void CWorldMap::SetDestinationMode( BOOL bDestinationMode )
{
	m_bDestinationMode = bDestinationMode;
}
#endif // __IMPROVE_QUEST_INTERFACE

BOOL CWorldMap::WorldPosToMapPos( const D3DXVECTOR3& vPos, OUT CPoint& cPos )
{
	if( 0 != m_nMap )		// 전체맵이 아닐경우 해당 지역에 관한 자표만 표시할꺼임
	{
		
		int nMap = CContinent::GetInstance()->GetMapNo( vPos );
		if( m_nMap != nMap )
			return FALSE;		
	}


	FLOAT fMap_X1 = _fMap_X1;
	FLOAT fMap_Y1 = _fMap_Y1;
	FLOAT fMap_X2 = _fMap_X2;
	FLOAT fMap_Y2 = _fMap_Y2;

	if( m_nMap == 0 )
	{
		fMap_X1 = 0.0f;
		fMap_Y1 = 1298.0f; //0.0f;
		fMap_X2 = 11800.0f;
		fMap_Y2 = 9757.0f; //9000.0f;
	}
	else
	if( m_nMap == 1 )
	{
		fMap_X1 = 2355.0f;
		fMap_Y1 = 2019.0f;
		fMap_X2 = 7187.0f;
		fMap_Y2 = 5814.0f;
	}
	else
	if( m_nMap == 2 )
	{
		fMap_X1 = 985.0f;
		fMap_Y1 = 1621.0f;
		fMap_X2 = 5754.0f;
		fMap_Y2 = 5149.0f;
	}		
	else
	if( m_nMap == 3 )
	{
		fMap_X1 = 8080.0f;
		fMap_Y1 = 3200.0f;
		fMap_X2 = 10280.0f;
		fMap_Y2 = 5000.0f;
	}
	else
	if( m_nMap == 4 )
	{
		fMap_X1 = 6854.0f;
		fMap_Y1 = 1580.0f;
		fMap_X2 = 10600.0f;
		fMap_Y2 = 4430.0f;
	}
	else
	if( m_nMap == 5 )
	{
		fMap_X1 = 5500.0f;
		fMap_Y1 = 2540.0f;
		fMap_X2 = 9200.0f;
		fMap_Y2 = 5130.0f;
	}
	else
	if( m_nMap == 6 ) // 하르모닌
	{
		fMap_X1 = 4795.0f;		// ui맵의 테두리 대 대륙사이즈 비율을 이용해 실제 월드대륙좌표에 적용시킨 ui의 테투리에 해당하는 월드 좌표
		fMap_Y1 = 4587.0f;
		fMap_X2 = 8391.0f;
		fMap_Y2 = 7236.0f;
	}
	else
	if( m_nMap == 7 ) // 이스티아 캐니언
	{
		fMap_X1 = 5150.0f;
		fMap_Y1 = 5573.0f;
		fMap_X2 = 9890.0f;
		fMap_Y2 = 9158.0f;
	}
	
	FLOAT fX = fMap_X2 - fMap_X1;
	FLOAT fY = fMap_Y1 - fMap_Y2;

	D3DXVECTOR3 v1 = D3DXVECTOR3( -1, 1, -1 );
	D3DXVECTOR3 v2 = D3DXVECTOR3( 1, -1, 1 );
	D3DXVECTOR3 v3 = D3DXVECTOR3( 1, 1, 1 ); 
	
	FLOAT fRate1 = (fMap_X2 - vPos.x) / fX ;
	FLOAT fRate2 = (fMap_Y1 - vPos.z) / fY ;

	int nX;
	int nY;
	if((g_Option.m_nResWidth == 1360 || g_Option.m_nResWidth == 1440 || g_Option.m_nResWidth == 1680) || 
		(g_Option.m_nResWidth == 1280 && (g_Option.m_nResHeight == 720 || g_Option.m_nResHeight == 768 || g_Option.m_nResHeight == 800))) //Wide Offset
	{
		v1 = D3DXVECTOR3( (FLOAT)( m_cOffset.x ), 0.0f, 0.0f );
		v2 = D3DXVECTOR3( (FLOAT)( m_cpScreen.x + m_cOffset.x ), (FLOAT)( m_cpScreen.y ), 0.0f );
	}
	else
	{
		v1 = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		v2 = D3DXVECTOR3( (FLOAT)( g_Option.m_nResWidth ), (FLOAT)( g_Option.m_nResHeight ), 0.0f );
	}

	D3DXVec3Lerp( &v3, &v2, &v1, fRate1 );
	nX = (int)( v3.x );
	D3DXVec3Lerp( &v3, &v2, &v1, fRate2 );
	nY = (int)( v3.y );

	cPos.x = nX;
	cPos.y = nY;

	return TRUE;

}

#endif	//__TELEPORTER

//=====================================================================================================================================================

#if __VER >= 13 // __CSC_VER13_1
CMonsterInfoPack::CMonsterInfoPack()
{
	m_nMap = 0;
}

CMonsterInfoPack::~CMonsterInfoPack()
{
}

BOOL CMonsterInfoPack::LoadScript( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR pszFileName, int nMap )
{
	CScript scanner;
	if( scanner.Load( pszFileName ) == FALSE )
	{
		Error( "%s not found", pszFileName );
		return FALSE;
	}

	m_nMap = nMap;

	DWORD dwCount = 0;
	CTexture* pTexture;
	BOOL bMultiLang = FALSE;
	m_vecMonsterInfo.clear();

	do {
		scanner.GetToken();
		if(scanner.Token == "number")
		{	
			m_dwNumber = scanner.GetNumber();
			m_ap2DTexture = new CTexture[ m_dwNumber ];
		}
		else if( scanner.Token == "MULTI_LANGUAGE" )
		{
			bMultiLang = TRUE;
		}
		else if(scanner.Token == "texture")
		{	
			scanner.GetToken();	

			TCHAR strFileName[MAX_PATH]; 
			strcpy(strFileName,scanner.token);	
			D3DCOLOR d3dKeyColor = scanner.GetHex();
			// 여기서 텍스춰 생성 (Create the texture using D3DX)
			D3DXIMAGE_INFO imageInfo;

			if( bMultiLang )
			{
				LoadTextureFromRes( pd3dDevice, MakePath( "Theme\\", ::GetLanguage(), strFileName ), 
										D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, //D3DFMT_A4R4G4B4, 
										D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
										D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, d3dKeyColor, &imageInfo, NULL, &m_pTexture );
			}
			else
			{
				LoadTextureFromRes( pd3dDevice, MakePath( DIR_THEME, strFileName ), 
					D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, //D3DFMT_A4R4G4B4, 
					D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
					D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, d3dKeyColor, &imageInfo, NULL, &m_pTexture );
			}

			m_size.cx = imageInfo.Width;
			m_size.cy = imageInfo.Height;
		}
		else if(scanner.Token == "serialize")
		{
			if( dwCount >= m_dwNumber )
			{
				Error( "%s 에러, 할당 :%d, 실제갯수 : %d", pszFileName, m_dwNumber, dwCount );
				return FALSE;
			}
			int nCnt = 0;
			int nFrame = scanner.GetNumber();
			SIZE size;
			size.cx = scanner.GetNumber();
			size.cy = scanner.GetNumber();
			POINT start;
			start.x = scanner.GetNumber();
			start.y = scanner.GetNumber();
			POINT center;
			center.x = scanner.GetNumber();
			center.y = scanner.GetNumber();
			
			for( int i = start.y; i < m_size.cy; i += size.cy )
			{
				for( int j = start.x; j < m_size.cx; j += size.cx, nCnt++ )
				{
					if( nCnt < nFrame )
					{
						if( dwCount >= m_dwNumber )
						{
							Error( "%s 에러, 할당 :%d, 실제갯수 : %d", pszFileName, m_dwNumber, dwCount );
							return FALSE;
						}
						
						pTexture = &m_ap2DTexture[ dwCount ];
						dwCount++;

						pTexture->m_size = size;	
						pTexture->m_ptCenter = center;
						int x = j;
						int y = i;
						pTexture->m_fuLT = (FLOAT) x / m_size.cx;
						pTexture->m_fvLT = (FLOAT) y / m_size.cy;
						pTexture->m_fuRT = (FLOAT) ( (FLOAT)x + pTexture->m_size.cx - 0 ) / (FLOAT)m_size.cx;
						pTexture->m_fvRT = (FLOAT) y / m_size.cy;
						pTexture->m_fuLB = (FLOAT) x / m_size.cx;
						pTexture->m_fvLB = (FLOAT) ( (FLOAT)y + pTexture->m_size.cy - 0 ) / (FLOAT)m_size.cy;
						pTexture->m_fuRB = (FLOAT) ( (FLOAT)x + pTexture->m_size.cx - 0 ) / (FLOAT)m_size.cx;
						pTexture->m_fvRB = (FLOAT) ( (FLOAT)y + pTexture->m_size.cy - 0 ) / (FLOAT)m_size.cy;
						pTexture->m_pTexture = m_pTexture;
					}
				}
			}

			for(i=0; i<nFrame; i++)
			{
				__MONSTER_INFO stMonInfo;
				stMonInfo.m_nMonCnt = scanner.GetNumber();
				for(int j=0; j<stMonInfo.m_nMonCnt; j++)
					stMonInfo.m_dwMonsterId[j] = scanner.GetNumber();

				if(g_Option.m_nResWidth == 1280 && g_Option.m_nResHeight == 960)
				{
					stMonInfo.m_rectPos.left = scanner.GetNumber();
					stMonInfo.m_rectPos.top = scanner.GetNumber();
				}
				else
				{
					CWorldMap* pWorldMap = CWorldMap::GetInstance();
					if(pWorldMap)
					{
						stMonInfo.m_rectPos.left = ((scanner.GetNumber() * pWorldMap->GetCpScreen().x) / 1280) + pWorldMap->GetCpOffset().x;
						stMonInfo.m_rectPos.top = ((scanner.GetNumber() * pWorldMap->GetCpScreen().y) / 960) + pWorldMap->GetCpOffset().y;
					}
				}
				stMonInfo.m_rectPos.right = stMonInfo.m_rectPos.left + size.cx;
				stMonInfo.m_rectPos.bottom = stMonInfo.m_rectPos.top + size.cy;

				stMonInfo.m_dwDropItemId = scanner.GetNumber();
				
				m_vecMonsterInfo.push_back(stMonInfo);
			}
		}
	} while(scanner.tok!=FINISHED);

	return TRUE;
}
#endif //__CSC_VER13_1


#if __VER >= 13 // __RAINBOW_RACE
BOOL CRainbowNPCPack::LoadScript( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR pszFileName )
{
	CScript scanner;
	if( scanner.Load( pszFileName ) == FALSE )
	{
		Error( "%s not found", pszFileName );
		return FALSE;
	}

	DWORD dwCount = 0;
	CTexture* pTexture;
	BOOL bMultiLang = FALSE;
	m_vecRainbowNPC.clear();

	do {
		scanner.GetToken();
		if(scanner.Token == "number")
		{	
			m_dwNumber = scanner.GetNumber();
			m_ap2DTexture = new CTexture[ m_dwNumber ];
		}
		else if( scanner.Token == "MULTI_LANGUAGE" )
		{
			bMultiLang = TRUE;
		}
		else if(scanner.Token == "texture")
		{	
			scanner.GetToken();	

			TCHAR strFileName[MAX_PATH]; 
			strcpy(strFileName,scanner.token);	
			D3DCOLOR d3dKeyColor = scanner.GetHex();
			// 여기서 텍스춰 생성 (Create the texture using D3DX)
			D3DXIMAGE_INFO imageInfo;

			if( bMultiLang )
			{
				LoadTextureFromRes( pd3dDevice, MakePath( "Theme\\", ::GetLanguage(), strFileName ), 
										D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, //D3DFMT_A4R4G4B4, 
										D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
										D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, d3dKeyColor, &imageInfo, NULL, &m_pTexture );
			}
			else
			{
				LoadTextureFromRes( pd3dDevice, MakePath( DIR_THEME, strFileName ), 
					D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, //D3DFMT_A4R4G4B4, 
					D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
					D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, d3dKeyColor, &imageInfo, NULL, &m_pTexture );
			}

			m_size.cx = imageInfo.Width;
			m_size.cy = imageInfo.Height;
		}
		else if(scanner.Token == "serialize")
		{
			if( dwCount >= m_dwNumber )
			{
				Error( "%s 에러, 할당 :%d, 실제갯수 : %d", pszFileName, m_dwNumber, dwCount );
				return FALSE;
			}
			int nCnt = 0;
			int nFrame = scanner.GetNumber();
			SIZE size;
			size.cx = scanner.GetNumber();
			size.cy = scanner.GetNumber();
			POINT start;
			start.x = scanner.GetNumber();
			start.y = scanner.GetNumber();
			POINT center;
			center.x = scanner.GetNumber();
			center.y = scanner.GetNumber();
			
			for( int i = start.y; i < m_size.cy; i += size.cy )
			{
				for( int j = start.x; j < m_size.cx; j += size.cx, nCnt++ )
				{
					if( nCnt < nFrame )
					{
						if( dwCount >= m_dwNumber )
						{
							Error( "%s 에러, 할당 :%d, 실제갯수 : %d", pszFileName, m_dwNumber, dwCount );
							return FALSE;
						}
						
						pTexture = &m_ap2DTexture[ dwCount ];
						dwCount++;

						pTexture->m_size = size;	
						pTexture->m_ptCenter = center;
						int x = j;
						int y = i;
						pTexture->m_fuLT = (FLOAT) x / m_size.cx;
						pTexture->m_fvLT = (FLOAT) y / m_size.cy;
						pTexture->m_fuRT = (FLOAT) ( (FLOAT)x + pTexture->m_size.cx - 0 ) / (FLOAT)m_size.cx;
						pTexture->m_fvRT = (FLOAT) y / m_size.cy;
						pTexture->m_fuLB = (FLOAT) x / m_size.cx;
						pTexture->m_fvLB = (FLOAT) ( (FLOAT)y + pTexture->m_size.cy - 0 ) / (FLOAT)m_size.cy;
						pTexture->m_fuRB = (FLOAT) ( (FLOAT)x + pTexture->m_size.cx - 0 ) / (FLOAT)m_size.cx;
						pTexture->m_fvRB = (FLOAT) ( (FLOAT)y + pTexture->m_size.cy - 0 ) / (FLOAT)m_size.cy;
						pTexture->m_pTexture = m_pTexture;
					}
				}
			}

			for(i=0; i<nFrame; i++)
			{
				__RAINBOW_NPC stRainbowNPC;
				stRainbowNPC.m_nMap = scanner.GetNumber();

				if(g_Option.m_nResWidth == 1280 && g_Option.m_nResHeight == 960)
				{
					stRainbowNPC.m_rectTotalMapPos.left = scanner.GetNumber();
					stRainbowNPC.m_rectTotalMapPos.top = scanner.GetNumber();
				}
				else
				{
					CWorldMap* pWorldMap = CWorldMap::GetInstance();
					if(pWorldMap)
					{
						stRainbowNPC.m_rectTotalMapPos.left = ((scanner.GetNumber() * pWorldMap->GetCpScreen().x) / 1280) + pWorldMap->GetCpOffset().x;
						stRainbowNPC.m_rectTotalMapPos.top = ((scanner.GetNumber() * pWorldMap->GetCpScreen().y) / 960) + pWorldMap->GetCpOffset().y;
					}
				}
				stRainbowNPC.m_rectTotalMapPos.right = stRainbowNPC.m_rectTotalMapPos.left + size.cx;
				stRainbowNPC.m_rectTotalMapPos.bottom = stRainbowNPC.m_rectTotalMapPos.top + size.cy;
				
				if(g_Option.m_nResWidth == 1280 && g_Option.m_nResHeight == 960)
				{
					stRainbowNPC.m_rectPos.left = scanner.GetNumber();
					stRainbowNPC.m_rectPos.top = scanner.GetNumber();
				}
				else
				{
					CWorldMap* pWorldMap = CWorldMap::GetInstance();
					if(pWorldMap)
					{
						stRainbowNPC.m_rectPos.left = ((scanner.GetNumber() * pWorldMap->GetCpScreen().x) / 1280) + pWorldMap->GetCpOffset().x;
						stRainbowNPC.m_rectPos.top = ((scanner.GetNumber() * pWorldMap->GetCpScreen().y) / 960) + pWorldMap->GetCpOffset().y;
					}
				}
				stRainbowNPC.m_rectPos.right = stRainbowNPC.m_rectPos.left + size.cx;
				stRainbowNPC.m_rectPos.bottom = stRainbowNPC.m_rectPos.top + size.cy;
				
				m_vecRainbowNPC.push_back(stRainbowNPC);
			}
		}
	} while(scanner.tok!=FINISHED);

	return TRUE;
}

#endif //__RAINBOW_RACE
