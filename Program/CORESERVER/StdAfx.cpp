// stdafx.cpp : source file that includes just the standard includes
//	CoreServer.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file



CTickCount	g_TickCount;
CGameTimer	g_GameTimer;

FLOAT	s_fPartyExpRate		= 1.0f;

#ifdef __EVENT0913
BOOL	s_bEvent0913	= FALSE;
#endif	// __EVENT0913
#ifdef __EVENT1206
BOOL	s_bEvent1206	= FALSE;
#endif	// __EVENT1206
BOOL	s_bTest		= FALSE;

DWORD	s_dwEvent	= 0;
//#ifdef __DBL_PARTYEXP
//BOOL	s_bDblPartyExp	= TRUE;
//#endif	// __DBL_PARTYEXP

//.,..\..\..\..\resource,..\..\..\_Network\Net\Include,..\..\..\_Network\Misc\Include,..\..\..\_Network\Mcl\Include,..\..\..\_network,..\..\..\_DirectX,..\..\..\_Common,..\..\..\_Interface,..\..\..\_unhandledexception
