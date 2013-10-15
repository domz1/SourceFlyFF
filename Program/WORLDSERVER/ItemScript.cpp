#include "stdafx.h"
#include "ItemScript.h"
#include "..\lua\lua.hpp"
#include "WScript.h"

static int Trace( lua_State *L )
{
	TRACE("%s",lua_tostring(L,1));
	return 0;
}

static int _SetDestParam( lua_State *L )
{
	CMover *pMover = (CMover *)lua_touserdata(L, 1);
	int nDST = (int)lua_tonumber(L, 2);
	int nAdd = (int)lua_tonumber(L, 3);

	pMover->SetDestParam( nDST, nAdd, NULL_CHGPARAM );
	return 0;
}

static int _ResetDestParam( lua_State *L )
{
	CMover *pMover = (CMover *)lua_touserdata(L, 1);
	int nDST = (int)lua_tonumber(L, 2);
	int nAdj = (int)lua_tonumber(L, 3);

	pMover->ResetDestParam( nDST, nAdj, TRUE );
	return 0;
}


BOOL CompileItemScript( LPCTSTR szFile )
{
#if !defined(__LUASCRIPT060908)
	return TRUE;
#endif

	lua_State* pVM = GetVM();

	lua_register( pVM, "Trace", Trace );
	lua_register( pVM, "SetDestParam", _SetDestParam );
	lua_register( pVM, "ResetDestParam", _ResetDestParam );

	if( luaL_dofile( pVM, szFile ) !=0 ) 
	{
		OutputDebugString( lua_tostring( pVM, -1) );
		return FALSE;
	}
	return TRUE;
}

BOOL RunItemScript( CMover* pMover, DWORD dwItemID, ITEM_OP operation, int* pResult )
{
#if !defined(__LUASCRIPT060908)
	return TRUE;
#endif

	char szFunction[64];

	if( operation == ITEM_OP_USE )
		sprintf( szFunction, "F%d_use", dwItemID );
	else if( operation == ITEM_OP_EQUIP )
		sprintf( szFunction, "F%d_equip", dwItemID );
	else if( operation == ITEM_OP_UNEQUIP )
		sprintf( szFunction, "F%d_unequip", dwItemID );

	lua_State* pVM = GetVM();

	lua_pushstring( pVM, szFunction );
	lua_gettable( pVM, LUA_GLOBALSINDEX );
	if ( lua_isfunction( pVM, -1 ) )
	{
		lua_pushlightuserdata( pVM, pMover );				// 1st arg
		lua_call( pVM, 1, 1 );  
		if( pResult )
			*pResult = (int)lua_tonumber( pVM, -1 );		// return value
		lua_pop( pVM, 1);
		return TRUE;
	}
	lua_pop( pVM, 1);
	return FALSE;
}

