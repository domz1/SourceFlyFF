// stdafx.cpp : source file that includes just the standard includes
//	LoginServer.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

char	g_lpCoreAddr[16];
char	g_lpDBAddr[16];
char    g_szMSG_VER[256] = "20040706";
BOOL	g_bNProtectAuth = FALSE;

CTickCount	g_TickCount;
