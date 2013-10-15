#ifndef __PLAYER_DATA_H__
#define	__PLAYER_DATA_H__

#include "ar.h"

typedef struct _sPlayerData
{
	BYTE	nJob;
	BYTE	nLevel;
//	DWORD	dwState;
	BYTE	nSex;
	int		nVer;
	BYTE	uLogin;
}	sPlayerData;

typedef struct	_PlayerData
{
	sPlayerData
		data;
	char	szPlayer[MAX_PLAYER];
	_PlayerData()
	{
		memset( this, 0, sizeof(_PlayerData) );
	}
}	PlayerData;

typedef struct	_PDVer
{
	u_long	idPlayer;
	int		nVer;
	_PDVer()
	{	idPlayer	= nVer	= 0;	}
	_PDVer( u_long u, int n )
	{
		idPlayer	= u;
		nVer	= n;
	}
}	PDVer;

class CPlayerDataCenter
{
public:
	CPlayerDataCenter();
	virtual	~CPlayerDataCenter();
	static	CPlayerDataCenter*	GetInstance( void );
	void		Clear();
#ifdef __CLIENT
	PlayerData*	GetPlayerData( u_long idPlayer, BOOL bQuery = TRUE );
#else	// __CLIENT
	PlayerData*	GetPlayerData( u_long idPlayer );
#endif	// __CLIENT
	PlayerData*	AddPlayerData( u_long idPlayer, PlayerData & pd );
#ifdef __CLIENT
	PlayerData*	AddPlayerData( u_long idPlayer, const char* pszPlayer );
#endif	// __CLIENT
	BOOL	DeletePlayerData( u_long idPlayer );
	int		Serialize( CAr & ar, int nFirst = 0, int nCount = 0 );
//	void	GetPlayerString( u_long idPlayer, char* pszPlayer );
	const char*	GetPlayerString( u_long idPlayer );
	void	ReplacePlayerString( u_long idPlayer, char* pszPlayer, char* pszOld );
	u_long	GetPlayerId( char* pszPlayer );
	int		size( void )	{	return m_mapPlayerData.size();	}

private:
	map<u_long, PlayerData*>	m_mapPlayerData;
	map<string, u_long>	m_mapPlayerStringToId;
public:
	CMclCritSec		m_Access;
};
#endif	// __PLAYER_DATA_H__