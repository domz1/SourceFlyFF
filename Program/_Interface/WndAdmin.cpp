// WndArcane.cpp: implementation of the CWndNeuz class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "defineText.h"

#include "WndAdmin.h"        // 운영자 매뉴 

/////////////////////////////////////////////////////////////////////////////////////
// 캡춰 윈도 
/////////////////////////////////////////////////////////////////////////////////////

CWndAdmin::CWndAdmin()
{
}

CWndAdmin::~CWndAdmin()
{

}
void CWndAdmin::OnDraw(C2DRender* p2DRender)
{
}
void CWndAdmin::OnInitialUpdate()
{

	CRect rect = GetClientRect();
	//ct.DeflateRect( 2, 2 );
	m_wndTabCtrl.Create( 0, rect, this, 12 );
	m_wndStateBase.Create( WBS_CHILD | WBS_NOFRAME, rect, this, 100 );
	m_wndStateDetail.Create( WBS_CHILD | WBS_NOFRAME, rect, this, 101 );
	m_wndJob.Create( WBS_CHILD | WBS_NOFRAME, rect, this, 102 );
	WTCITEM tabTabItem;

	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_APP_CHARACTER_BASE);//"기본";
	tabTabItem.pWndBase = &m_wndStateBase;
	m_wndTabCtrl.InsertItem( 0, &tabTabItem );

	tabTabItem.pszText = prj.GetText(TID_APP_CHARACTER_DETAIL);//"자세히";
	tabTabItem.pWndBase = &m_wndStateDetail;
	m_wndTabCtrl.InsertItem( 1, &tabTabItem );

	tabTabItem.pszText = prj.GetText(TID_APP_CHARACTER_JOB);//"직업";
	tabTabItem.pWndBase = &m_wndJob;
	m_wndTabCtrl.InsertItem( 2, &tabTabItem );


	/*
	CRect rectClient = GetClientRect();
	rectClient.top = 80;

#define MAX_COMMAND 18

	static TCHAR aszCommand[MAX_COMMAND][30] = 
	{ 
		_T( "Grid" ), _T( "Wireframe" ), _T( "Skybox" ), _T( "Terrain" ), _T( "Object" ), _T( "Fog" ), _T( "LOD" ), 
		_T( "Water" ), _T( "Name" ), _T( "Animation" ), _T( "FrameSkip" ), _T( "CullObj" ), _T( "Light" ), 
		_T( "Weather" ), _T( "NudeSkin" ), _T( "Capture" ), _T( "BoundBox" ), _T( "Collision" )
	};

	for( int i = 0, i3 = 0; i < 6; i++ )
	{
		for( int i2 = 0; i2 < 3; i2++, i3++)
		{
			if( i3 < MAX_COMMAND )
			{
				m_wndDebug[ i3 ].Create( aszCommand[ i3], BBS_CHECK, CRect( rectClient.left + 2, rectClient.top, rectClient.left + 100, rectClient.top + 20 ), this, 100 + i3 ); 
				//m_wndDebug[ i3 ].SetPushColor( D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
				//m_wndDebug[ i3 ].SetFontColor( D3DCOLOR_ARGB( 255, 000, 000, 255 ) );
			}
			rectClient.left += 100;
		}
		rectClient.left = 0;
		rectClient.top += 20;
	}
	m_wndEditExp.Create   ( g_Neuz.GetSafeHwnd(), 0, CRect( 5, 200, 100, 220 ), this, 1001 );
	m_wndButtonExp.Create ( _T( "Exp Up!" ), 0, CRect( 105, 200, 160, 220 ), this, 1002 ); 
	m_wndSetSkinSet.Create( _T( "SkinSet" ), 0, CRect(   5, 230,  60, 250 ), this, 1003 ); 
	m_wndSetHair.Create   ( _T( "Hair"    ), 0, CRect(  75, 230, 130, 250 ), this, 1004 ); 
	m_wndSetColor.Create  ( _T( "Color"   ), 0, CRect( 135, 230, 190, 250 ), this, 1005 ); 
	m_wndSetFace.Create   ( _T( "Face"    ), 0, CRect( 195, 230, 250, 250 ), this, 1006 ); 


//	m_wndDebug[ 0  ].SetCheck( g_World.m_bViewGrid      );
//	m_wndDebug[ 1  ].SetCheck( g_World.m_bViewWireframe );
//	m_wndDebug[ 2  ].SetCheck( g_World.m_bViewSkybox    );
//	m_wndDebug[ 3  ].SetCheck( g_World.m_bViewTerrain   );
//	m_wndDebug[ 4  ].SetCheck( g_World.m_bViewObject    );
//	m_wndDebug[ 5  ].SetCheck( g_World.m_bViewFog       );
//	m_wndDebug[ 6  ].SetCheck( g_World.m_bViewLOD       );
//	m_wndDebug[ 7  ].SetCheck( g_World.m_bViewWater     );
//	m_wndDebug[ 8  ].SetCheck( g_World.m_bViewName      );
	m_wndDebug[ 0  ].SetCheck( g_WorldMng()->m_bViewGrid      );
	m_wndDebug[ 1  ].SetCheck( g_WorldMng()->m_bViewWireframe );
	m_wndDebug[ 2  ].SetCheck( g_WorldMng()->m_bViewSkybox    );
	m_wndDebug[ 3  ].SetCheck( g_WorldMng()->m_bViewTerrain   );
	m_wndDebug[ 4  ].SetCheck( g_WorldMng()->m_bViewObject    );
	m_wndDebug[ 5  ].SetCheck( g_WorldMng()->m_bViewFog       );
	m_wndDebug[ 6  ].SetCheck( g_WorldMng()->m_bViewLOD       );
	m_wndDebug[ 7  ].SetCheck( g_WorldMng()->m_bViewWater     );
	m_wndDebug[ 8  ].SetCheck( g_WorldMng()->m_bViewName      );

	m_wndDebug[ 9  ].SetCheck( CObj::IsAnimate()  );
	m_wndDebug[ 10 ].SetCheck( GetFrameSkip() );
//	m_wndDebug[ 11 ].SetCheck( g_World.m_bCullObj );
//	m_wndDebug[ 12 ].SetCheck( g_World.m_bViewLight );
//	m_wndDebug[ 13 ].SetCheck( g_World.m_bViewWeather );
	m_wndDebug[ 11 ].SetCheck( g_WorldMng()->m_bCullObj );
	m_wndDebug[ 12 ].SetCheck( g_WorldMng()->m_bViewLight );
	m_wndDebug[ 13 ].SetCheck( g_WorldMng()->m_bViewWeather );
	m_wndDebug[ 14 ].SetCheck( m_pTheme->m_bNudeSkin );
	m_wndDebug[ 15 ].SetCheck( g_Neuz.m_bCapture );
//	m_wndDebug[ 16 ].SetCheck( g_World.m_bViewBoundBox );
	m_wndDebug[ 16 ].SetCheck( g_WorldMng()->m_bViewBoundBox );
	m_wndDebug[ 17 ].SetCheck( CObj::m_bCollision );
	
*/

/*
	rectClient = GetClientRect();
	rectClient.left = rectClient.right - 15;
	rectClient.right -= 2;
	rectClient.top = 40;
	rectClient.bottom = 52;
	m_wndExtend.Create( "V", 0, rectClient, this, 1000 ); 
	m_wndExtend.SetCheck( TRUE );
*/
	CWndNeuz::OnInitialUpdate();
}
BOOL CWndAdmin::Initialize(CWndBase* pWndParent,DWORD dwWndId)
{
//	CRect rect(0,0,280,190);
	CRect rect(0,0,280,270);
	SetTitle("디버그 정보");
	return CWndNeuz::Create( WBS_MOVE|WBS_SOUND|WBS_CAPTION|WBS_EXTENSION, rect, &g_WndMng, dwWndId);
}
BOOL CWndAdmin::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
BOOL CWndAdmin::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndAdmin::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType,cx,cy);
}
void CWndAdmin::OnLButtonUp(UINT nFlags, CPoint point)
{
	CWndNeuz::OnLButtonUp(nFlags,point);
}
void CWndAdmin::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWndNeuz::OnLButtonDown(nFlags,point);
}
void CWndAdmin::OnDestroyChildWnd(CWndBase* pWndChild)
{
	if( m_pWndCapture == pWndChild )
		SAFE_DELETE( m_pWndCapture );
}	

