#include "stdafx.h"
#include "WScript.h"
#include "..\lua\lua.hpp"

static lua_State* g_VM = NULL;

lua_State* GetVM()
{
	return g_VM;
}

BOOL InitializeScriptLib()
{
#if !defined(__LUASCRIPT060908)
	return TRUE;
#else
	g_VM = lua_open();
	luaL_openlibs(g_VM);

	return TRUE;
#endif
}

void UninitializeScriptLib()
{
#if !defined(__LUASCRIPT060908)
	return;
#else
	if( g_VM )
		lua_close(g_VM);
#endif
}
