// LuaBase.h: interface for the CLuaBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LUABASE_H__85CF0FDE_0B13_4D74_AC9E_407878030516__INCLUDED_)
#define AFX_LUABASE_H__85CF0FDE_0B13_4D74_AC9E_407878030516__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma comment( lib, "..\\Lua\\lua.lib" )

extern "C" {
#include "..\Lua\lua.h"
#include "..\Lua\lualib.h"
#include "..\Lua\lauxlib.h"
}

class CLuaBase  
{
public:
	CLuaBase();
	virtual ~CLuaBase();

	static int	_TRACE( lua_State* pLuaState );
	static int	_ERROR( lua_State* pLuaState );
	
	lua_State*	GetLuaState()								{  return m_pLuaState;  }
	void		RegisterFunction( char* szFromLuaFunc, lua_CFunction ToCFunc );
	int			RunScript( const char* szFileName );
	
	BOOL		IsNil( int nStackPos )						{  return lua_isnil( m_pLuaState, nStackPos );  }
	BOOL		IsTable( int nStackPos )					{  return lua_istable( m_pLuaState, nStackPos );  }
	
	void		PushNumber( double nValue )					{  lua_pushnumber( m_pLuaState, nValue );  }
	void		PushString( const char* szValue )			{  lua_pushstring( m_pLuaState, szValue );  }
	void		PushPointer( void* p )						{  lua_pushlightuserdata( m_pLuaState, p );  }
	void		PushBool( int bValue )						{  lua_pushboolean( m_pLuaState, bValue );  }
	void		PushNil()									{  lua_pushnil( m_pLuaState );  }
	void		Pop( int nPop );

	double		ToNumber( int nStackPos )					{  return lua_tonumber( m_pLuaState, nStackPos );  }
	const char*	ToString( int nStackPos )					{  return lua_tostring( m_pLuaState, nStackPos );  }
	BOOL		ToBool( int nStackPos )						{  return static_cast<BOOL>( lua_toboolean( m_pLuaState, nStackPos ) );  }
	int			TableLoop( int nStackPos )					{  return lua_next( m_pLuaState, nStackPos );  }	
	void		GetGloabal( char* szName )					{  lua_getglobal( m_pLuaState, szName );  }
	double		GetGlobalNumber( char* szName );
	const char* GetGlobalString( char* szName );
	BOOL		GetGlobalBool( char* szName );
	void		GetField( int nStackPos, char* szName )		{  lua_getfield( m_pLuaState, nStackPos, szName );  }
	double		GetFieldToNumber( int nStackPos, char* szName );
	const char* GetFieldToString( int nStackPos, char* szName );
	BOOL		GetFieldToBool( int nStackPos, char* szName );
	
	BOOL		GetLuaFunction( char* szFunc );
	void		CallLuaFunction( int nElem, int nReturn );
	
private:
	lua_State*	m_pLuaState;
};

#endif // !defined(AFX_LUABASE_H__85CF0FDE_0B13_4D74_AC9E_407878030516__INCLUDED_)
