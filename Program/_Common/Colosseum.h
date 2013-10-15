#ifndef TESTCLASS_H
#define TESTCLASS_H

#ifdef __COLOSSEUM

struct __COLOSSEUM_MONSTER
{
	int		dwTrueId;
	int		dwFakeId;
	int		nProb;
	int		dwId;
	int		nTime;
	int		nStage;
	int		nDifficult;
};

class CColosseumMatch
{
public:
			CColosseumMatch();
	virtual ~CColosseumMatch();
	void	Process();
	void	CreateMover();
	void	DestroyMover();
	void	CreateRoomLayer();
	void	EnterRoom( CUser* pUser );
	void	EndMatch( BOOL bWin );
	CUser*	GetUser( u_long uPlayerID );
	void	EndWaitTime();
	void	KickUser( u_long m_uPlayerID );
	BOOL	IsTeleported( u_long m_uPlayerID );

	u_long	m_uPartyID;
	BYTE	m_nStage;
	int		m_nStartTime;
	int		m_nNextStage;
	BOOL	m_bWaitTime;
	BYTE	m_byDifficult;
	BYTE	m_byMenu;
	vector<u_long>	m_vecPlayerID;
	DWORD	m_nMoverID;
	u_long	m_uGuildID;
	BYTE	m_byBossStage;
};

typedef map<u_long, CColosseumMatch*> COLOSSEUMMATCH;

class CColosseumMng
{
public:
	CColosseumMng();
	virtual	~CColosseumMng();

	static	CColosseumMng*	GetInstance( void );
			BOOL	CreateRoomLayer( int nPartyID );
			void	DeleteRoomLayer( int nPartyID );
			BOOL	CreateNewMatch( CUser* pUser, BYTE byDifficult, BYTE byMenu );
			void	Process();
			BOOL	LoadScript();
			void	DestroyMatch( u_long m_uPartyID );
			CColosseumMatch*	GetMatchByParty( CParty* pParty );
			CColosseumMatch*	GetMatchByMoverID( DWORD dwMoverID );

			CLuaBase m_Lua;
			int		nStartWaitTime;
			int		nWaitAfterStage;
			BYTE	byMaxLowStage;
			BYTE	byMaxHardStage;
			COLOSSEUMMATCH m_mapMatch;

			vector<__COLOSSEUM_MONSTER> m_vecMonster;
};

#endif //__COLOSSEUM
#endif //__COLOSSEUM_H__