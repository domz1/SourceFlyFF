// PCBang.cpp: implementation of the CPCBang class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PCBang.h"

#if __VER >= 14 // __PCBANG

#ifdef __WORLDSERVER
CPCBangInfo::CPCBangInfo( DWORD dwPCBangClass, DWORD dwConnectTime )
:m_dwPCBangClass( dwPCBangClass ),
m_dwConnectTime( dwConnectTime ),
m_dwDisconnectTime( 0 ),
m_dwUnitTime( 0 ),
m_fExpFactor( 1.0f ),
m_fPieceItemDropFactor( 1.0f )
{
	UpdateInfo();
}

CPCBangInfo::~CPCBangInfo()
{
}

BOOL CPCBangInfo::ProcessRemove( DWORD dwPlayerId )
{
	if( m_dwDisconnectTime != 0 && m_dwDisconnectTime + MIN(10) < g_tmCurrent )
		return TRUE;
	
	DWORD dwUnitTime = GetUnitTime();
	if( m_dwUnitTime < dwUnitTime )
	{
		m_dwUnitTime = dwUnitTime;
		UpdateInfo();
		CUser* pUser = static_cast<CUser*>( prj.GetUserByID( dwPlayerId ) );
		if( IsValidObj( pUser ) )
			pUser->AddPCBangInfo( this );
	}

	return FALSE;
}

void CPCBangInfo::UpdateInfo()
{
	m_fExpFactor = CPCBang::GetInstance()->GetExpInfo( m_dwUnitTime );
	m_fPieceItemDropFactor = CPCBang::GetInstance()->GetPieceItemDropInfo( m_dwUnitTime );
}
#endif // __WORLDSERVER

void CPCBangInfo::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_dwPCBangClass << g_tmCurrent - m_dwConnectTime << m_fExpFactor << m_fPieceItemDropFactor;
	}
	else
	{
		ar >> m_dwPCBangClass >> m_dwConnectTime >> m_fExpFactor >> m_fPieceItemDropFactor;
		m_dwConnectTime = g_tmCurrent - m_dwConnectTime;
	}
}

#ifdef __CLIENT
CPCBangInfo::CPCBangInfo()
:m_dwPCBangClass( 0 ),
m_dwConnectTime( 0 ),
m_fExpFactor( 1.0f ),
m_fPieceItemDropFactor( 1.0f )
{
}

CPCBangInfo::~CPCBangInfo()
{
}

CPCBangInfo* CPCBangInfo::GetInstance()
{
	static CPCBangInfo sPCBangInfo;
	return &sPCBangInfo;
}
#endif // __CLIENT


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#ifdef __WORLDSERVER
CPCBang::CPCBang()
:m_bApply( TRUE )
{
	LoadScript();
}

CPCBang::~CPCBang()
{
	m_vecfExp.clear();
	m_vecfDropRate.clear();
}

CPCBang* CPCBang::GetInstance()
{
	static CPCBang sPCBang;
	return & sPCBang;
}

BOOL CPCBang::LoadScript()
{
	CLuaBase lua;
	if( lua.RunScript( "PCBang.lua" ) != 0 )
	{
		::AfxMessageBox( "PCBang.lua Load Failed!!" );
		return FALSE;
	}
	
	lua.GetGloabal( "tExp" );
	lua.PushNil();
	while( lua.TableLoop( -2 ) )
	{
		m_vecfExp.push_back( static_cast<float>( lua.ToNumber( -1 ) ) );
		lua.Pop( 1 );
	}
	lua.Pop( 0 );

	lua.GetGloabal( "tDropRate" );
	lua.PushNil();
	while( lua.TableLoop( -2 ) )
	{
		m_vecfDropRate.push_back( static_cast<float>( lua.ToNumber( -1 ) ) );
		lua.Pop( 1 );
	}
	lua.Pop( 0 );

	return TRUE;
}


void CPCBang::SetPCBangPlayer( CUser* pUser, DWORD dwPCBangClass )
{
	if( dwPCBangClass == 0 )
	{
		m_mapPCBang.erase( pUser->m_idPlayer );
		return;
	}

	CPCBangInfo* pPI = GetPCBangInfo( pUser->m_idPlayer );
	if( pPI )	// 접속해제 한지 10분이 안지난 놈이면...
	{
		pPI->SetDisconnect( 0 );
		pUser->AddPCBangInfo( pPI );
	}
	else
	{
		CPCBangInfo pi( dwPCBangClass, g_tmCurrent );
		m_mapPCBang.insert( MAPPBI::value_type( pUser->m_idPlayer, pi ) );
		pUser->AddPCBangInfo( &pi );
	}
		
	if( IsApply() )
		pUser->AddBuff( BUFF_EQUIP, II_PCBANG_BUFF01, 1, 999999999 );
}

void CPCBang::DestroyPCBangPlayer( DWORD dwPlayerId )
{
	CPCBangInfo* pPI = GetPCBangInfo( dwPlayerId );
	if( pPI )
		pPI->SetDisconnect( g_tmCurrent );
}

DWORD CPCBang::GetPCBangClass( DWORD dwPlayerId )
{
	CPCBangInfo* pPI = GetPCBangInfo( dwPlayerId );
	if( pPI )
		return pPI->GetPCBangClass();

	return 0;
}

void CPCBang::ProcessPCBang()
{
	for( MAPPBI::iterator it=m_mapPCBang.begin(); it!=m_mapPCBang.end(); )
	{
		if( it->second.ProcessRemove( it->first ) )
			m_mapPCBang.erase( it++ );
		else
			it++;
	}
}

void CPCBang::SetApply( BOOL bApply )
{
	m_bApply = bApply;

	if( IsApply() )
	{
		for( MAPPBI::iterator it=m_mapPCBang.begin(); it!=m_mapPCBang.end(); it++ )
		{
			CUser* pUser = static_cast<CUser*>( prj.GetUserByID( it->first ) );
			if( IsValidObj( pUser ) )
				pUser->AddBuff( BUFF_EQUIP, II_PCBANG_BUFF01, 1, 999999999 );
		}
	}
	else
	{
		for( MAPPBI::iterator it=m_mapPCBang.begin(); it!=m_mapPCBang.end(); it++ )
		{
			CUser* pUser = static_cast<CUser*>( prj.GetUserByID( it->first ) );
			if( IsValidObj( pUser ) )
				pUser->RemoveBuff( BUFF_EQUIP, II_PCBANG_BUFF01 );
		}
	}
}

CPCBangInfo* CPCBang::GetPCBangInfo( DWORD dwPlayerId )
{
	MAPPBI::iterator it = m_mapPCBang.find( dwPlayerId );
	if( it == m_mapPCBang.end() )
		return NULL;
	
	return &it->second;
}

float CPCBang::GetExpInfo( DWORD dwHour )
{
	int nMaxIndex = m_vecfExp.size() - 1;
	if( nMaxIndex >= 0 )
	{
		if( (int)( dwHour ) > nMaxIndex )
			return m_vecfExp[nMaxIndex];
		else
			return m_vecfExp[dwHour];
	}

	return 1.0f;
}

float CPCBang::GetExpFactor( CUser* pUser )
{
	if( !IsApply() || !IsValidObj( pUser ) || !pUser->m_buffs.HasBuff( BUFF_EQUIP, II_PCBANG_BUFF01 ) )
		return 1.0f;

	CPCBangInfo* pPI = GetPCBangInfo( pUser->m_idPlayer );
	if( pPI )
		return pPI->GetExpFactor();

	return 1.0f;
}

float CPCBang::GetPartyExpFactor( CUser* apUser[], int nMemberSize )
{
	float fExpFactor = 0.0f;
	for( int i=0; i<nMemberSize; i++ )
		fExpFactor += GetExpFactor( apUser[i] ) - 1.0f;		
	fExpFactor /= nMemberSize;
	fExpFactor += 1.0f;

	return fExpFactor;
}

float CPCBang::GetPieceItemDropInfo( DWORD dwHour )
{
	int nMaxIndex = m_vecfDropRate.size() - 1;
	if( nMaxIndex >= 0 )
	{
		if( (int)( dwHour ) > nMaxIndex )
			return m_vecfDropRate[nMaxIndex];
		else
			return m_vecfDropRate[dwHour];
	}

	return 1.0f;
}

float CPCBang::GetPieceItemDropFactor( CUser* pUser )
{	
	if( !IsApply() || !IsValidObj( pUser ) || !pUser->m_buffs.HasBuff( BUFF_EQUIP, II_PCBANG_BUFF01 ) )
		return 1.0f;

	CPCBangInfo* pPI = GetPCBangInfo( pUser->m_idPlayer );
	if( pPI )
		return pPI->GetPieceItemDropFactor();
	
	return 1.0f;
}
#endif // __WORLDSERVER

#endif // __PCBANG