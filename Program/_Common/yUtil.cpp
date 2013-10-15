#include "stdafx.h"
#include "yUtil.h"

#define MAX_ALPHA		230


CFLASH g_Flash;

CFLASH::CFLASH()  
{ 
	Init(); 
}
CFLASH::~CFLASH()
{
}
	
void CFLASH::Init()
{
	m_bAction		= FALSE;
	m_bFade			= FALSE;
	m_nAlphaCount	= 0;
	m_dwStopTime	= 0;
	m_dwTime		= 0;
	m_dwEndTime		= 0;
	m_bStopTime     = FALSE;
	m_bRunTime		= FALSE;
	m_nAlphaAcc     = 10;
	
}
void CFLASH::RenderFlash( C2DRender* p2DRender )
{
	_PROFILE("CFLASH::RenderFlash()");

	if( m_bAction )
	{
		// 흰색으로 만든다.
		if( m_bFade == FALSE )
		{
			m_nAlphaCount+=m_nAlphaAcc;
			m_nAlphaAcc+=20;
		}
		// 페이드 아웃함( 정지시간, 지속시간 )
		else
		{
			// 처음에 한번만 정지 유지시간 입력
			if( !m_bStopTime )
			{
				m_dwStopTime = timeGetTime()+m_dwStopTime;
				m_bStopTime = TRUE;
			}

			if( timeGetTime() > m_dwStopTime )
			{
				//처음에 한번만 지속유지시간 입력
				DWORD dwSaveTime = timeGetTime();
				
				BOOL bBackup = m_bRunTime;

				if(	!m_bRunTime )
				{
					m_dwEndTime = dwSaveTime+m_dwTime;
					m_bRunTime = TRUE;
				}

				FLOAT fLerp = (FLOAT)(m_dwEndTime - dwSaveTime) / (FLOAT)m_dwTime;

				D3DXCOLOR dwColor;
				D3DXCOLOR dwStartColor = D3DCOLOR_RGBA( 255, 255, 255, 0 );
				D3DXCOLOR dwEndColor   = D3DCOLOR_RGBA( 255, 255, 255, 255  );
				
				// 지나온시간과 완료시간사이의 보간
				D3DXColorLerp( &dwColor, &dwStartColor, &dwEndColor, fLerp );
				
				m_nAlphaCount = (int)( dwColor.a * 255 );
				
				if( m_nAlphaCount <= 0 || m_dwEndTime < dwSaveTime )
					Init();
			}
			else
			{
			}
		}
		
		// 가장 밝은 색이 되면 페이드 아웃을 실시함
		if( m_nAlphaCount > MAX_ALPHA )
		{
			m_bFade = TRUE;
			m_nAlphaCount = MAX_ALPHA;
		}
		
		p2DRender->RenderFillRect( CRect( 0, 0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT-48 ), D3DCOLOR_ARGB( m_nAlphaCount, 255, 255, 255 ) );
	}
}

void CFLASH::SetTime( DWORD dwStopTime, DWORD dwTime, BOOL bAction )
{
	Init();
	SetAction( bAction );

	m_dwStopTime = dwStopTime;
	m_dwTime	 = dwTime;
}

void CFLASH::SetAction( BOOL bAction )
{
	m_bAction = bAction;
}

BOOL IsForceAttack()
{
	return ( GetAsyncKeyState(VK_CONTROL) & 0x8000 );
}

#define min_ata(x,y) (x < y ? x : y)
#define max_ata(x,y) (x > y ? x : y)
#define INSIDE 0
#define OUTSIDE 1


int Point_In_Poly( vector<CPoint> vecMap, CPoint test_point )
{
	int counter = 0;
	int i;
	double xinters;
	CPoint p1,p2;
	
	p1 = vecMap[0];
	for (i=1;i<=(int)( vecMap.size() );i++) {
		p2 = vecMap[i % vecMap.size()];
		if (test_point.y > min_ata(p1.y,p2.y)) {
			if (test_point.y <= max_ata(p1.y,p2.y)) {
				if (test_point.x <= max_ata(p1.x,p2.x)) {
					if (p1.y != p2.y) {
						xinters = (test_point.y-p1.y)*(p2.x-p1.x)/(p2.y-p1.y)+p1.x;
						if (p1.x == p2.x || test_point.x <= xinters)
							counter++;
					}
				}
			}
		}
		p1 = p2;
	}
	
	if (counter % 2 == 0)
		return 0;
	else
		return 1;
}
/*
{
	int x, y;
	int nx, ny, loop;
	
	//	Clockwise order.
	
	for ( loop = 0; loop < vecMap.size(); loop++ )
	{
		//	generate a 2d normal ( no need to normalise ).
		nx = vecMap[ ( loop + 1 ) % vecMap.size() ].y - vecMap[ loop ].y;
		ny = vecMap[ loop ].x - vecMap[ ( loop + 1 ) % vecMap.size() ].x;
		
		x = test_point.x - vecMap[ loop ].x;
		y = test_point.y - vecMap[ loop ].y;
		
		//	Dot with edge normal to find side.
		if ( ( x * nx ) + ( y * ny ) > 0 )
			return 0;
	}
	
	return 1;
}
*/