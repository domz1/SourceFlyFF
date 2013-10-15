// stdafx.cpp : source file that includes just the standard includes
//	Neuz.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"  

#include "Ship.h"


CProject         prj;
CWorldMng  	     g_WorldMng;
CToolTip         g_toolTip;
#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
CToolTip         g_toolTipSub1;
CToolTip         g_toolTipSub2;
#endif // __IMPROVE_SYSTEM_VER15
CULLINFO         g_cullinfo;
CMover*          g_pPlayer;
CShip*			 g_pShip;				// 플레이어가 조종 하는 비공정.
BYTE             g_bKeyTable[256];		// 키 입력 테이블
BOOL             g_bSlotSwitchAboutEquipItem[ MAX_SLOT_ITEM ];		// 장착 아이템에 대한 키 눌림 스위치
#ifdef __XKEYEDGE
BYTE             g_bKeyTable2[256];		// 키 입력 테이블( 키 엣지 검사 )
#endif //__XKEYEDGE
CModelObject*    g_pBipedMesh;
CD3DApplication* g_pD3dApp; 
CGameTimer       g_GameTimer;
CTickCount       g_TickCount;
CFlyffEvent      g_eLocal;
ADDSMMODE        g_AddSMMode;
DWORD	         g_tmCurrent = 0;		// 전역으로 사용할 현재 시간.
time_t		g_tCurrent;		//	time( NULL )
int		         g_nRenderCnt = 0;		// 렌더화면때마다 하나씩 카운트 되는 변수
int		         g_nProcessCnt = 0;		// 프로세스마다 하나씩 카운트 되는 변수.

#ifdef __BS_CONSOLE
DlgConsole g_Console;
#endif

#ifdef __CERTIFIER_COLLECTING_SYSTEM
vector< CString > g_vecEncryptedValidCertifierIP;
#endif // __CERTIFIER_COLLECTING_SYSTEM

CNeuzApp         g_Neuz;                // 맨 마지막에

#if __VER >= 11 // __GUILD_COMBAT_1TO1
CGuildCombat1to1Mng g_GuildCombat1to1Mng;
#endif // __GUILD_COMBAT_1TO1

