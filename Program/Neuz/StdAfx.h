// stdafx.h : include file for standard system include files,         
//  or project specific include files that are used frequently, but
//      are changed infrequently
//  

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_
 
#if _MSC_VER > 1000
#pragma once 
#endif // _MSC_VER > 1000

#pragma warning(disable:4786)

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define _WIN32_WINNT 0x0500
   
// Windows or Standard        
#include <afxwin.h>      
#include <AFXTEMPL.H>        // MFC Automation classes  
#include <winuser.h>    
#include <basetsd.h>  
#include <memory.h>  
#include <mmsystem.h>   
#include <string.h>   
#include <stdio.h>  
#include <tchar.h>  
#include <time.h> 
#include <stdlib.h>
#include <process.h>

#include <afxdisp.h>        // MFC Automation classes

// STL 
#include <vector>
#include <list>
#include <map>
#include <string> 
#include <queue>
using namespace std;


#define		__CLIENT
// exception handler관련 
#define		ADDR_ACCUMULATOR	"211.33.142.133"		

#include "NeuzVersion.h"  
#include "VersionCommon.h"
#include "DefineCommon.h"
#include "Resource.h"    
#include "authorization.h" 

// DirectX or 3D Core
#include <D3D9.h>
#include <dxerr9.h>
#include <dsound.h>
#include <dshow.h>

#include "memtrace.h"

#include "dxutil.h"
#include "d3denumeration.h" 
#include "d3dsettings.h"
#include "d3dapp.h" 
#include "vutil.h"
#include "d3dfont.h"
#include "d3dutil.h"
#include "DSUtil.h"

// Network
#include "CmnHdr.h"
#include "File.h"
#include "chassert.h"

// Resource
#include "misc.h"
#include "define.h"
#include "defineAttribute.h"
#include "defineItemKind.h"

#if __VER >= 19
#include "defineItemGrade.h"
#include "defineItemType.h"
#endif//__VER >= 19

#include "defineJob.h"
#include "defineWorld.h"
#include "WndStyle.h"

// MultiMedia
#include <atlbase.h> 

// Common
#include "Data.h"
#include "Timer.h"
#include "Scanner.h"
#include "Script.h"
#include "2DRender.h"
#include "SkyBox.h"
#include "patch.h"
#include "Project.h"
#include "path.h"
#include "landscape.h"
#include "xUtil3D.h"
#include "Graphic3D.h"
#include "Debug.h"
#include "Music.h"
#include "HwOption.h"
#include "Messenger.h"
#include "yUtil.h"

#if __VER >= 15 // __BS_CHANGING_ENVIR
#include "TexturePool.h"	//gmpbigsun
#endif	//__BS_CHANGING_ENVIR

// Model
#include "Model.h"
#include "Billboard.h"
#include "Material.h"
#include "Bone.h"
#include "Object3D.h"
#include "ModelObject.h"
#include "ModelMng.h"
#include "Particle.h"

// object
#include "Obj.h"
#include "Ctrl.h"
#include "Item.h"
#include "sfxbase.h"
#include "Mover.h"   
#include "Region.h"
#include "Weather.h"
#include "Camera.h"
#include "Light.h"
#include "World.h"
#include "WorldMng.h"

#include "CreateObj.h"
#include "authorization.h"

// Interface
#include "EditString.h"      // CString을 확장한 CEditString
#include "ToolTip.h"         // 툴팁 
#include "ITheme.h"          // 인터페이스의 색상, 스킨, 폰트 따위의 정의 클레스 
#include "WndBase.h"         // 윈도의 기본 클래스, 기타 각종 콘트롤 (버튼, 스크롤바 등등) 
#include "WndControl.h"      // 윈도 각종 콘트롤 (버튼, 스크롤바 등등)
#include "wndEditCtrl.h"     // 에디트 콘트롤, IME, 문자 입력
#include "WndItemCtrl.h"     // 윈도 각종 콘트롤 (버튼, 스크롤바 등등)

#ifdef __16_SHOP
#include "WndShopCtrl.h"
#endif

#include "WndTradeCtrl.h"     // 윈도 각종 콘트롤 (버튼, 스크롤바 등등)
#include "WndNeuz.h"         // Neuz 형태의 윈도 출력 기본 클래스 
#include "WndMessageBox.h"   // 매시지 박스, 경고, 종료 등등 
#include "WndGuideSystem.h"
#include "WndTitle.h"        // Title의 윈도 클래스 (Login, Character List 등등)
#include "WndTaskBar.h"      // 태스크 바 클래스 관련, 매뉴 등등 
#include "WndField.h"        // Field의 윈도 클래스 (상태, 인벤토리 등등)
#include "WndCommunication.h"// Field의 윈도 클래스 (상태, 인벤토리 등등)
#include "WndOption.h"       // Option 관련 
#include "WndOptionGame.h"	 // 게임 옵션
#include "WndDialog.h"       // 대화  
#include "WndShop.h"         // 상점 
#include "WndMessengerSearch.h" // 메신저 검색창
#include "WndManager.h"      // 윈도 매니저 
#include "WndWorld.h"        // 월드 윈도 
#include "WndGuild.h"			// 길드 윈도
#include "WndRankGuild.h"		// 길드 랭킹
#include "WndRankInfo.h"
#include "WndRankWar.h"
#include "WndGuildVote.h"         // 길드투표 윈도
#include "WndUpgradeBase.h"         // 재련윈도 

#ifdef __YCLOTH
#include "cloth.h"
#endif


// Application
#include "DialogMsg.h"
#include "Neuz.h"
#include "tickcount.h"
#include "flyffevent.h"

// console
#ifdef __BS_CONSOLE
#include "Console.h"
#endif

#ifdef __BS_EFFECT_LUA
#include "BsCommon.h"
#endif

#define D3DDEVICE   g_Neuz.m_pd3dDevice

extern BYTE             g_bKeyTable[256]; // 키 입력 테이블 
extern BOOL             g_bSlotSwitchAboutEquipItem[ MAX_SLOT_ITEM ];
#ifdef __XKEYEDGE
extern BYTE             g_bKeyTable2[256]; // 키 입력 테이블 (키 에지검사용)
#endif //__XKEYEDGE
extern CNeuzApp         g_Neuz        ; // 어플리케이션 클래스, DX3D 프레임 워크 
extern CToolTip         g_toolTip     ; // 툴팁 출력 
#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
extern CToolTip         g_toolTipSub1 ;
extern CToolTip         g_toolTipSub2 ;
#endif // __IMPROVE_SYSTEM_VER15
extern CMover*          g_pPlayer     ; // 플레이어 객체 
extern CWorldMng	    g_WorldMng    ;
extern CProject         prj;
extern CModelObject*    g_pBipedMesh;
extern CD3DApplication* g_pD3dApp;
extern CGameTimer       g_GameTimer;

struct CULLINFO
{
    D3DXVECTOR3 vecFrustum[8];    // corners of the view frustum
    D3DXPLANE planeFrustum[6];    // planes of the view frustum
};
extern CULLINFO     g_cullinfo;
extern	CTickCount	g_TickCount;
extern CFlyffEvent	g_eLocal; 
extern ADDSMMODE	g_AddSMMode;
extern DWORD		g_tmCurrent;	// 전역으로 사용할 현재 시간.
extern int			g_nRenderCnt;	// 렌더화면때마다 하나씩 카운트 되는 변수
extern int			g_nProcessCnt;	// 프로세스마다 하나씩 카운트 되는 변수.


#if __VER >= 11 // __GUILD_COMBAT_1TO1
#include "GuildCombat1to1.h"
extern CGuildCombat1to1Mng g_GuildCombat1to1Mng;
#endif // __GUILD_COMBAT_1TO1

#ifdef __BS_CONSOLE
extern DlgConsole g_Console;
#endif

#ifdef __CERTIFIER_COLLECTING_SYSTEM
extern vector< CString > g_vecEncryptedValidCertifierIP;
#endif // __CERTIFIER_COLLECTING_SYSTEM




//////////////////////////////////////////////////////////////////////////
//	mulcom	BEGIN100218	패스워드 암호화.
#include "../_Common/Rijndael.h"
//	mulcom	END100218	패스워드 암호화.
//////////////////////////////////////////////////////////////////////////


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
