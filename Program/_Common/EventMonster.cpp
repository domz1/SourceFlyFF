// EventMonster.cpp: implementation of the CEventMonster class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EventMonster.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#ifdef __EVENT_MONSTER

CEventMonster::CEventMonster()
{

}

CEventMonster::~CEventMonster()
{

}

CEventMonster* CEventMonster::GetInstance( void )
{	
	static CEventMonster sEventMonster;
	return &sEventMonster;
}

void CEventMonster::LoadScript()
{
	CLuaBase lua;
	if( lua.RunScript( "EventMonster.lua" ) != 0 )
	{
		Error( "LUA - EventMonster.lua Failed!!!" );
		ASSERT(0);
	}

	lua.GetGloabal( "tEventMonster" );
	lua.PushNil();
	while( lua.TableLoop( -2 ) )
	{
		__EVENTMONSTER EventMonster;

		DWORD	dwId = CScript::GetDefineNum( lua.GetFieldToString( -1, "strMonsterId" ) );
		EventMonster.nLevel = static_cast<int>( lua.GetFieldToNumber( -1, "nLevel" ) );
		EventMonster.dwLootTime = static_cast<DWORD>( lua.GetFieldToNumber( -1, "nLootTime" ) );
		EventMonster.fItemDropRange = static_cast<float>( lua.GetFieldToNumber( -1, "fItemDropRange" ) );
		EventMonster.bPet = lua.GetFieldToBool( -1, "bPet" );
		EventMonster.bGiftBox = lua.GetFieldToBool( -1, "bGiftBox" );
		
		m_mapEventMonster.insert( make_pair( dwId, EventMonster ) );
		lua.Pop( 1 );
	}

	lua.Pop(0);
}

BOOL CEventMonster::IsEventMonster( DWORD dwId )
{
	map< DWORD, __EVENTMONSTER >::iterator it = m_mapEventMonster.find( dwId );
	if( it != m_mapEventMonster.end() )
		return TRUE;
	
	return FALSE;
}

BOOL CEventMonster::SetEventMonster( DWORD dwId )
{
	m_it = m_mapEventMonster.find( dwId );
	if( m_it != m_mapEventMonster.end() )
		return TRUE;

	return FALSE;	
}

BOOL CEventMonster::IsPetAble()
{
	if( m_it != m_mapEventMonster.end() )
		return m_it->second.bPet;
	
	return FALSE;	
}

BOOL CEventMonster::IsGiftBoxAble()
{
	if( m_it != m_mapEventMonster.end() )
		return m_it->second.bGiftBox;

	return FALSE;	
}


DWORD CEventMonster::GetLootTime()
{
	if( m_it != m_mapEventMonster.end() )
		return m_it->second.dwLootTime;
	
	return 0;
}

int CEventMonster::GetLevelGap()
{
	if( m_it != m_mapEventMonster.end() )
		return m_it->second.nLevel;
	
	return MAX_GENERAL_LEVEL;
}

float CEventMonster::GetItemDropRange()
{
	if( m_it != m_mapEventMonster.end() )
		return m_it->second.fItemDropRange;

	return 2.0f;
}
#endif // __EVENT_MONSTER