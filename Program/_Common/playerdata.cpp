#include "stdafx.h"
#include "playerdata.h"
#ifdef __CLIENT
#include "dpclient.h"
extern	CDPClient	g_DPlay;
#endif	// __CLIENT

#if __VER >= 11 // __SYS_PLAYER_DATA
CPlayerDataCenter::CPlayerDataCenter()
{
}
CPlayerDataCenter::~CPlayerDataCenter()
{
	Clear();
}

CPlayerDataCenter*	CPlayerDataCenter::GetInstance( void )
{
	static CPlayerDataCenter sPlayerDataMng;
	return &sPlayerDataMng;
}

void CPlayerDataCenter::Clear()
{
	CMclAutoLock	Lock( m_Access );
	for( map<u_long, PlayerData*>::iterator i	= m_mapPlayerData.begin(); i != m_mapPlayerData.end(); ++i )
		safe_delete( i->second );
	m_mapPlayerData.clear();
}

#ifdef __CLIENT
PlayerData*	CPlayerDataCenter::GetPlayerData( u_long idPlayer, BOOL bQuery )
#else	// __CLIENT
PlayerData*	CPlayerDataCenter::GetPlayerData( u_long idPlayer )
#endif	// __CLIENT
{
	PlayerData*	pPlayerData	= NULL;
	map<u_long, PlayerData*>::iterator i	= m_mapPlayerData.find( idPlayer );
	if( i == m_mapPlayerData.end() )
	{
#ifdef __CLIENT
		pPlayerData	= new PlayerData;
		pPlayerData->data.nLevel		= 1;
		bool b	= m_mapPlayerData.insert( map<u_long, PlayerData*>::value_type( idPlayer, pPlayerData ) ).second;
		ASSERT( b );
		if( bQuery )
			g_DPlay.SendQueryPlayerData( idPlayer );
#endif	// __CLIENT
	}
	else
	{
		pPlayerData	= i->second;
#ifdef __CLIENT
		if( pPlayerData->data.nLevel == 0 )
		{
			pPlayerData->data.nLevel		= 1;
			if( bQuery )
				g_DPlay.SendQueryPlayerData( idPlayer );
		}
#endif	// __CLIENT
	}
	return pPlayerData;
}

PlayerData*	CPlayerDataCenter::AddPlayerData( u_long idPlayer, PlayerData & pd )
{
	CMclAutoLock	Lock( m_Access );
	PlayerData* ptr	= GetPlayerData( idPlayer );

// __CHIPI_080109 - 캐릭터 삭제 후 동일 이름으로 캐릭터 생성시 문제
	m_mapPlayerStringToId.erase( pd.szPlayer );
// __CHIPI_080109
	bool b	= m_mapPlayerStringToId.insert( map<string, u_long>::value_type( pd.szPlayer, idPlayer ) ).second;
	ASSERT( b );

	if( ptr == NULL )
	{
		PlayerData* pPlayerData		= new PlayerData;
		memcpy( pPlayerData, &pd, sizeof(PlayerData) );
		b	= m_mapPlayerData.insert( map<u_long, PlayerData*>::value_type( idPlayer, pPlayerData ) ).second;	
		ASSERT( b );
		return pPlayerData;
	}
	if( pd.data.nVer == 0 )
	{
		pd.data.nVer	= ptr->data.nVer;
		pd.data.uLogin	= ptr->data.uLogin;
	}
	memcpy( ptr, &pd, sizeof(PlayerData) );
//	TRACE( "AddPlayerData, %s, v = %d, l = %d\n", ptr->szPlayer, ptr->data.nVer, ptr->data.uLogin );
	return ptr;
}

#ifdef __CLIENT
PlayerData*	CPlayerDataCenter::AddPlayerData( u_long idPlayer, const char* pszPlayer )
{
	CMclAutoLock	Lock( m_Access );
	PlayerData* pPlayerData	= GetPlayerData( idPlayer );
	if( pPlayerData )
		return pPlayerData;
	bool b	= m_mapPlayerStringToId.insert( map<string, u_long>::value_type( pszPlayer, idPlayer ) ).second;
	ASSERT( b );
	pPlayerData		= new PlayerData;
	lstrcpy( pPlayerData->szPlayer, pszPlayer );
	b	= m_mapPlayerData.insert( map<u_long, PlayerData*>::value_type( idPlayer, pPlayerData ) ).second;
	ASSERT( b );	
	return pPlayerData;
}
#endif	// __CLIENT

BOOL	CPlayerDataCenter::DeletePlayerData( u_long idPlayer )
{
	CMclAutoLock	Lock( m_Access );
	map<u_long, PlayerData*>::iterator i	= m_mapPlayerData.find( idPlayer );
	if( i != m_mapPlayerData.end() )
	{
		m_mapPlayerStringToId.erase( i->second->szPlayer );
		safe_delete( i->second );
		m_mapPlayerData.erase( i );
		return TRUE;
	}
	return FALSE;
}

int CPlayerDataCenter::Serialize( CAr & ar, int nFirst, int nCount )
{
	int nRet	= 0;
	CMclAutoLock	Lock( m_Access );
	if( ar.IsLoading() )
	{
		u_long uSize;
		ar >> uSize;
		int nRet	= uSize;
		for( u_long i = 0; i < uSize; i++ )
		{
			u_long idPlayer;
			ar >> idPlayer;
			PlayerData pd;
			ar.ReadString( pd.szPlayer, MAX_PLAYER );
			ar.Read( &pd.data, sizeof(sPlayerData) );
			AddPlayerData( idPlayer, pd );
		}
	}
	else
	{
		if( !nCount )
		{
			ar << (u_long)m_mapPlayerData.size();
			nRet	= m_mapPlayerData.size();
			for( map<u_long, PlayerData*>::iterator i = m_mapPlayerData.begin(); i != m_mapPlayerData.end(); ++i )
			{
				ar << i->first;
				PlayerData* pPlayerData	= i->second; 
				ar.WriteString( pPlayerData->szPlayer );
				ar.Write( &pPlayerData->data, sizeof(sPlayerData) );
			}
		}
		else
		{
			int nRBount	= nFirst + nCount;
			if( nRBount > (int)( m_mapPlayerData.size() ) )
				nRBount	= m_mapPlayerData.size();
			ar << static_cast<u_long>( nRBount - nFirst );
			int nth		= 0;
			for( map<u_long, PlayerData*>::iterator i = m_mapPlayerData.begin(); i != m_mapPlayerData.end(); ++i )
			{
				if( nth >= nFirst && nth < nRBount )
				{
					ar << i->first;
					PlayerData* pPlayerData	= i->second; 
					ar.WriteString( pPlayerData->szPlayer );
					ar.Write( &pPlayerData->data, sizeof(sPlayerData) );
					nRet++;
				}	// if
				nth++;
			}	// for
		}	// if
	}
	return nRet;
}

/*
void CPlayerDataCenter::GetPlayerString( u_long idPlayer, char* pszPlayer )
{
	CMclAutoLock	Lock( m_Access );
	PlayerData* pPlayerData	= GetPlayerData( idPlayer );
	if( pPlayerData )
		lstrcpy( pszPlayer, pPlayerData->szPlayer );
}
*/
const char*	CPlayerDataCenter::GetPlayerString( u_long idPlayer )
{
//	CMclAutoLock	Lock( m_Access );
	PlayerData* pPlayerData		= GetPlayerData( idPlayer );
	if( pPlayerData )
		return pPlayerData->szPlayer;
	return NULL;
}

u_long	CPlayerDataCenter::GetPlayerId( char* pszPlayer )
{
	u_long idPlayer	 = 0;
	map<string, u_long>::iterator i		= m_mapPlayerStringToId.find( pszPlayer );
	if( i != m_mapPlayerStringToId.end() )
		idPlayer	= i->second;
	return idPlayer;
}

void CPlayerDataCenter::ReplacePlayerString( u_long idPlayer, char* pszPlayer, char* pszOld )
{
	CMclAutoLock	Lock( m_Access );
	PlayerData* pPlayerData	= GetPlayerData( idPlayer );
	if( pPlayerData )
	{
		lstrcpy( pszOld, pPlayerData->szPlayer );
		lstrcpy( pPlayerData->szPlayer, pszPlayer );
	}
}
#endif	// __SYS_PLAYER_DATA