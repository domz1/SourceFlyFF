// WndArcane.cpp: implementation of the CWndNeuz class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AppDefine.h"
//#include "WndSample.h"
#include "WndDebugInfo.h"
#include "DPClient.h"
extern	CDPClient	g_DPlay;

/////////////////////////////////////////////////////////////////////////////////////
// 캡춰 윈도 
/////////////////////////////////////////////////////////////////////////////////////

CWndDebugInfo::CWndDebugInfo()
{
	SetPutRegInfo( FALSE );
}

CWndDebugInfo::~CWndDebugInfo()
{

}
void CWndDebugInfo::OnDraw(C2DRender* p2DRender)
{
    // Show frame rate
	p2DRender->TextOut( 2,  0 ,g_Neuz.m_strFrameStats , D3DCOLOR_ARGB( 255, 255, 255, 255 )/*adeilson*/ );
	p2DRender->TextOut( 2, 20 ,g_Neuz.m_strDeviceStats, D3DCOLOR_ARGB( 255, 255, 255, 255 )/*adeilson*/ );

	TCHAR strDebug[90];

	if( g_pPlayer && g_pPlayer->GetWorld() )
	{
		D3DXVECTOR3 vPos = g_pPlayer->GetPos();
		_stprintf( strDebug, _T("x:%6.3f y:%6.3f z:%6.3f %6.3f"), vPos.x, vPos.y, vPos.z,(float)g_pPlayer->GetAngle());
		p2DRender->TextOut( 2, 40, strDebug, D3DCOLOR_ARGB( 255, 255, 255, 255 )/*adeilson*/ );
		_stprintf( strDebug, _T("Objects:%d  Faces:%d   LandFace:%d %d" ), g_pPlayer->GetWorld()->m_nObjCullSize, g_nMaxTri,0, g_WndMng.m_pWndWorld->GetMouseMode() );
		p2DRender->TextOut( 2, 55, strDebug, D3DCOLOR_ARGB( 255, 255, 255, 255 )/*adeilson*/ );
	}
	else
	{
		_stprintf( strDebug, _T( "x:0.0 y:0.0 z:%0.0 0.0" ) );
		p2DRender->TextOut( 2, 40, strDebug, D3DCOLOR_ARGB( 255, 255, 255, 255 )/*adeilson*/ );
		_stprintf( strDebug, _T("Objects:0  Faces:0" ) );
		p2DRender->TextOut( 2, 55, strDebug, D3DCOLOR_ARGB( 255, 255, 255, 255 )/*adeilson*/ );
	}

	// chipi_090814 - 핑 출력을 디버그 창으로 이동
	if( g_pPlayer && g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
	{
		CString string;
		string.Format( "Ping(%d ms)", g_Neuz.m_dwPingTime );
		p2DRender->TextOut( 200, 260, string, D3DCOLOR_ARGB( 255, 255, 255, 255 )/*adeilson*/ );
	}
}
void CWndDebugInfo::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	CWndButton* pWndButton;

	pWndButton = (CWndButton*)GetDlgItem( WIDC_GRID       ); pWndButton->SetCheck( g_WorldMng()->m_bViewGrid      );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_TERRAIN    ); pWndButton->SetCheck( g_WorldMng()->m_bViewTerrain   );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_LOD_TERRAIN); pWndButton->SetCheck( g_WorldMng()->m_bViewLODTerrain);
	pWndButton = (CWndButton*)GetDlgItem( WIDC_LOD_OBJ    ); pWndButton->SetCheck( g_WorldMng()->m_bViewLODObj    );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_ANIMATION  ); pWndButton->SetCheck( CObj::IsAnimate()	 	      );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_LIGHT      ); pWndButton->SetCheck( g_WorldMng()->m_bViewLight     );
	//pWndButton = (CWndButton*)GetDlgItem( WIDC_CAPTURE    ); pWndButton->SetCheck( 	 	);
	pWndButton = (CWndButton*)GetDlgItem( WIDC_HIDE       ); pWndButton->SetCheck( !g_pPlayer->IsVisible() );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_WIREFRAME  ); pWndButton->SetCheck( g_WorldMng()->m_bViewWireframe  );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_OBJECT     ); pWndButton->SetCheck( g_WorldMng()->m_bViewAllObjects );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_FRAME_SKIP ); pWndButton->SetCheck( GetFrameSkip()	               );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_WEATHER    ); pWndButton->SetCheck( g_WorldMng()->m_bViewWeather	   );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_BOUNDBOX   ); pWndButton->SetCheck( g_WorldMng()->m_bViewBoundBox   );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_ATTRIBUTE  ); pWndButton->SetCheck( g_WorldMng()->m_bViewHeightAttribute	 	);
	pWndButton = (CWndButton*)GetDlgItem( WIDC_SKYBOX     ); pWndButton->SetCheck( g_WorldMng()->m_bViewSkybox 	  );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_NAME       ); pWndButton->SetCheck( g_WorldMng()->m_bViewName 	  );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_CULLOBJ    ); pWndButton->SetCheck( g_WorldMng()->m_bCullObj	      );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_COLLISION  ); pWndButton->SetCheck( CObj::m_bCollision	 	      );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_FOG        ); pWndButton->SetCheck( g_WorldMng()->m_bViewFog 	  );	 
	pWndButton = (CWndButton*)GetDlgItem( WIDC_IDSTATE    ); pWndButton->SetCheck( g_WorldMng()->m_bViewIdState   );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_WATER      ); pWndButton->SetCheck( g_WorldMng()->m_bViewWater 	  );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_HOUR       ); pWndButton->SetCheck( g_GameTimer.IsFixed()  );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_CHECK1     ); pWndButton->SetCheck( g_WorldMng()->m_bViewHP 	  );
	
	CString string;
	string.Format( "%d", g_GameTimer.m_nHour );
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT_HOUR ); pWndEdit->SetString( string );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_HOUR ); pWndButton->SetCheck(  g_GameTimer.IsFixed() );
	//g_World.m_bViewWater
	//////////////////////////////////////////////////////
	
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
/*
void CWndDebugInfo::OnInitialUpdate()
{
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
				m_wndDebug[ i3 ].Create( aszCommand[ i3], WBS_CHECK, CRect( rectClient.left + 2, rectClient.top, rectClient.left + 100, rectClient.top + 20 ), this, 100 + i3 ); 
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
	m_wndDebug[ 4  ].SetCheck( g_WorldMng()->m_bViewAllObjects   );
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
	
	CWndNeuz::OnInitialUpdate();
}
*/
BOOL CWndDebugInfo::Initialize(CWndBase* pWndParent,DWORD dwWndId)
{
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_DEBUGINFO, 0, CPoint( 0, 0 ), pWndParent );
/*	
//	CRect rect(0,0,280,190);
	CRect rect(0,0,280,270);
	SetTitle("디버그 정보");
	return CWndNeuz::Create( 0 | WBS_MOVE | WBS_SOUND | WBS_THICKFRAME | WBS_CAPTION, rect, &g_WndMng, dwWndId);
	*/
}
BOOL CWndDebugInfo::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	CWndButton* pWndButton = (CWndButton*)*pLResult;
	//if( message == WNM_CLICKED )
	{
		switch( nID )
		{
		case WIDC_BUTTON1: // exp up
		{
			CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
			int nExp = _ttoi( pWndEdit->GetString() );

			if( nExp >= 0 && nExp < 0x7fffffff )
			{
				g_DPlay.SendExp( nExp );
			}
			break;
		}
			/*
		case 1003: // skinset
			g_pPlayer->SetSkinSet( _ttoi( m_wndEditExp.GetString() ) );
			break;
		case 1004: // hair
			g_pPlayer->SetHair( _ttoi( m_wndEditExp.GetString() ) );
			break;
		case 1005: // haircolor
			g_pPlayer->SetHairColor( _ttoi( m_wndEditExp.GetString() ) );
			break;
		case 1006: // face
			g_pPlayer->SetHead( _ttoi( m_wndEditExp.GetString() ) );
			break;
			
		case 1000:
			{
				CRect rect = GetWindowRect();
				m_wndExtend.SetCheck( !m_wndExtend.GetCheck() );
				if( m_wndExtend.GetCheck() )
					SetWndSize( rect.Width(), rect.Height() + 55 );
				else
					SetWndSize( rect.Width(), rect.Height() - 55 );
			}
			break;
			*/
//		case 100: g_World.m_bViewGrid      = !g_World.m_bViewGrid      ; 
		case WIDC_GRID: g_WorldMng()->m_bViewGrid      = !g_WorldMng()->m_bViewGrid      ; 
/*
			m_wndListBox.Create( WBS_MENU | WBS_NOFRAME, CRect(10,10,100,100), this, 10);
			m_wndListBox.AddString("test1");
			m_wndListBox.AddString("test1");
			m_wndListBox.AddString("test1");
			m_wndListBox.AddString("test1");
			m_wndListBox.AddString("test1");
			m_wndListBox.AddString("test1");
*/		//	m_wndListBox.AddString("test1");m_wndListBox.AddString("test1");m_wndListBox.AddString("test1");
				
			break;
//		case 101: g_World.m_bViewWireframe = !g_World.m_bViewWireframe ; break;
//		case 102: g_World.m_bViewSkybox    = !g_World.m_bViewSkybox    ; break;
//		case 103: g_World.m_bViewTerrain   = !g_World.m_bViewTerrain   ; break;
//		case 104: g_World.m_bViewObject    = !g_World.m_bViewObject    ; break;
//		case 105: 
//			g_World.m_bViewFog = !g_World.m_bViewFog; 			    
//			g_World.SetFogEnable( g_Neuz.m_pd3dDevice, g_World.m_bViewFog ); 
//			break;
//		case 106: g_World.m_bViewLOD       = !g_World.m_bViewLOD       ; break;
//		case 107: g_World.m_bViewWater     = !g_World.m_bViewWater     ; break;
//		case 108: g_World.m_bViewName      = !g_World.m_bViewName      ; break;
		case WIDC_WIREFRAME: g_WorldMng()->m_bViewWireframe = !g_WorldMng()->m_bViewWireframe ; break;
		case WIDC_SKYBOX: g_WorldMng()->m_bViewSkybox    = !g_WorldMng()->m_bViewSkybox    ; break;
		case WIDC_TERRAIN: g_WorldMng()->m_bViewTerrain   = !g_WorldMng()->m_bViewTerrain   ; break;
		case WIDC_OBJECT: g_WorldMng()->m_bViewAllObjects   = !g_WorldMng()->m_bViewAllObjects   ; break;
		case 105: 
			g_WorldMng()->m_bViewFog = !g_WorldMng()->m_bViewFog; 			    
			g_WorldMng()->SetFogEnable( g_Neuz.m_pd3dDevice, g_WorldMng()->m_bViewFog ); 
			break;
		case WIDC_LOD_TERRAIN: g_WorldMng()->m_bViewLODTerrain       = !g_WorldMng()->m_bViewLODTerrain       ; break;
		case WIDC_LOD_OBJ: g_WorldMng()->m_bViewLODObj      = !g_WorldMng()->m_bViewLODObj       ; break;
		case WIDC_WATER: g_WorldMng()->m_bViewWater     = !g_WorldMng()->m_bViewWater     ; break;
		case WIDC_NAME: g_WorldMng()->m_bViewName      = !g_WorldMng()->m_bViewName      ; break;
		case WIDC_ANIMATION: CObj::AnimateEnable( !CObj::IsAnimate() ); break;
		case WIDC_FRAME_SKIP: SetFrameSkip( !GetFrameSkip() ); break;
		case WIDC_FOG: g_WorldMng()->m_bViewFog = !g_WorldMng()->m_bViewFog; break;
		case WIDC_CHECK1: g_WorldMng()->m_bViewHP      = !g_WorldMng()->m_bViewHP      ; break;
			
//		case 111: g_World.m_bCullObj = !g_World.m_bCullObj; break;
//		case 112: g_World.m_bViewLight = !g_World.m_bViewLight; break;
//		case 113: g_World.m_bViewWeather = !g_World.m_bViewWeather; break;
		case WIDC_CULLOBJ: g_WorldMng()->m_bCullObj = !g_WorldMng()->m_bCullObj; break;
		case WIDC_LIGHT: g_WorldMng()->m_bViewLight = !g_WorldMng()->m_bViewLight; break;
		case WIDC_WEATHER: g_WorldMng()->m_bViewWeather = !g_WorldMng()->m_bViewWeather; break;
		case WIDC_ATTRIBUTE: g_WorldMng()->m_bViewHeightAttribute = !g_WorldMng()->m_bViewHeightAttribute; break;
		case WIDC_HIDE: g_pPlayer->SetVisible( !g_pPlayer->IsVisible() ); break;
		//case WIDC_114: m_pTheme->m_bNudeSkin = !m_pTheme->m_bNudeSkin; break;
			/*
		case 115: 
			if( g_Neuz.m_bCapture == FALSE )
			{
				m_wndDebug[ 15 ].SetCheck( !m_wndDebug[ 15 ].GetCheck() );
				m_pWndCapture = new CWndCapture;
				m_pWndCapture->Initialize( this, 100000 );
			} 
			else
			{
				g_Neuz.m_bCapture = FALSE;
				m_wndDebug[ 15 ].SetCheck( g_Neuz.m_bCapture );
			}
			break;
			*/
		case WIDC_BOUNDBOX: 
//			g_World.m_bViewBoundBox = !g_World.m_bViewBoundBox;
			g_WorldMng()->m_bViewBoundBox = !g_WorldMng()->m_bViewBoundBox;
			break;
		case WIDC_COLLISION:
			CObj::m_bCollision = !CObj::m_bCollision;
			break;
//			{
//				CWndSample* pWndSample = new CWndSample;
//				pWndSample->Initialize();
//			}
		case WIDC_IDSTATE:
			g_WorldMng()->m_bViewIdState = !g_WorldMng()->m_bViewIdState;
			break;
		case WIDC_HOUR:
			 g_GameTimer.SetFixed( !g_GameTimer.IsFixed() );
			 pWndButton = (CWndButton*)GetDlgItem( WIDC_HOUR );
			 if( pWndButton->GetCheck() )
			 {
				 CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT_HOUR );
				 int nHour = atoi( pWndEdit->GetString() );
				 if( nHour > 23 ) nHour = 23;
				 if( nHour < 0 ) nHour = 0;					 
				 g_GameTimer.SetTime( 0, nHour, 0, 0 );//Fixed( 	g_GameTimer.IsFixed() );//g_WorldMng()->m_nFixedHour = nHour;
			 }
			 
			break;
		case WIDC_EDIT_HOUR:
			{
				pWndButton = (CWndButton*)GetDlgItem( WIDC_HOUR );
				if( pWndButton->GetCheck() )
				{
					CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT_HOUR );
					int nHour = atoi( pWndEdit->GetString() );
					if( nHour > 23 ) nHour = 23;
					if( nHour < 0 ) nHour = 0;					 
					g_GameTimer.SetTime( 0, nHour, 0, 0 );//Fixed( 	g_GameTimer.IsFixed() );//g_WorldMng()->m_nFixedHour = nHour;
				}
			}
			break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
BOOL CWndDebugInfo::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndDebugInfo::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType,cx,cy);
}
void CWndDebugInfo::OnLButtonUp(UINT nFlags, CPoint point)
{
	CWndNeuz::OnLButtonUp(nFlags,point);
}
void CWndDebugInfo::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWndNeuz::OnLButtonDown(nFlags,point);
}
void CWndDebugInfo::OnDestroyChildWnd(CWndBase* pWndChild)
{
	if( m_pWndCapture == pWndChild )
		SAFE_DELETE( m_pWndCapture );
}	

