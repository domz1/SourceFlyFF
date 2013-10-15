#pragma once

#if __VER >= 13 // __RAINBOW_RACE
#include "MiniGameBase.h"

enum {	RMG_GAWIBAWIBO = 0, RMG_DICEPLAY, RMG_ARITHMATIC, RMG_STOPWATCH,
		RMG_TYPING, RMG_PAIRGAME, RMG_LADDER,
		RMG_MAX };

//////////////////////////////////////////////////////////////////
// CRainbowRace //////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
class CRainbowRace
{
public:
	CRainbowRace();
	~CRainbowRace();
	void	DestroyGame();

	void	Serialize( CAr & ar );
	BOOL	IsCompletedGame( int nGameNum );	// nGameNum(RMG_...) 게임을 클리어 했는가?
	BOOL	IsAllCompleted() { return ( m_wGameState == (0x7fff >> (15 - RMG_MAX)) ); } // 모든 미니게임을 클리어 했는가?
	
	WORD	m_wGameState;		// 해당 미니게임의 완료여부(각 비트)
	WORD	m_wNowGame;			// 현재 진행중인 미니게임(해당 비트)
#ifdef __CLIENT
	static CRainbowRace* GetInstance();
	int		GetGameKey();
	WORD	GetGameKeyToNowGame( int nGameKey ) {	return (0x0001 << nGameKey); }
	void	SendMinigamePacket( int nGameKey = RMG_MAX, int nState = MP_NONE, int nParam1 = 0, int nParam2 = 0 );
	void	SendMinigameExtPacket( vector<string> &vecszData, int nGameKey = RMG_MAX, int nState = MP_NONE, int nParam1 = 0, int nParam2 = 0 );

	DWORD	m_dwRemainTime;
	BOOL	m_bRRFinishMsg;
	BOOL	m_bCheckEnd;
#endif // __CLIENT
#ifdef __WORLDSERVER
	void	SetNowGameComplete( CUser* pUser );	// 이번 게임을 완료상태로 만들고 다음 게임을 셋팅한다.
	void	SetNextMiniGame();	// 다음 게임 셋팅...
	void	OnMiniGamePacket( CUser* pUser, __MINIGAME_PACKET* pMiniGamePacket );

	void	SetFinish()	{ m_bFinished = TRUE; m_wGameState = 0x0000; }		// 완주 설정
	BOOL	IsFinished() { return m_bFinished; }	// 이미 완주한 놈인가..
private:
	int		GetCompletedGameNum();		// 현재까지 완료한 미니게임 개수
	void	SetMiniGame( int nGameNum );
	CMiniGameBase*	m_pMiniGame;		// 현재 진행중인 미니게임 포인터

	BOOL	m_bFinished;		// 레인보우 레이스 완주했는가..
#endif // __WORLDSERVER
};


//////////////////////////////////////////////////////////////////
// CRainbowRaceMng ///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
#ifdef __WORLDSERVER
typedef map<DWORD, CRainbowRace*> MAPRR;

class CRainbowRaceMng
{
public:
	enum {	RR_CLOSED = 0, RR_APPLICATION, RR_OPEN, RR_WAIT, RR_READY, RR_PROGRESS };

	CRainbowRaceMng(void);
	~CRainbowRaceMng(void);

	BOOL Init();
	void Clear();
	
	static CRainbowRaceMng* GetInstance();
	
	BOOL	LoadScript();
	int		GetMiniGameSize()			{ return m_vecMiniGame.size(); }	// 미니게임 갯수
	void	SetState( int nState )	{ m_nState = nState; }				// State 변경
	int		GetState()				{ return m_nState; }				// 현재 State

	void	Process();
	void	SetNextTime( DWORD dwTick )		{ m_dwNextTime = dwTick; }			// 다음 단계 시간 설정
	CMiniGameBase* GetMiniGame( int nGameNum ) { return m_vecMiniGame[nGameNum]; }

	CRainbowRace*	GetRainbowRacerPtr( DWORD dwPlayerId );	// 해당 플레이어의 레이스 정보를 갖는 포인터를 얻는다.
	BOOL			IsEntry( DWORD dwPlayerId );	// 해당 플레이어가 진행중인 선수인가?(블링크윙, 스킬, 아이템 막기위해...)
	BOOL			IsApplicant( DWORD dwPlayerId ) { return GetRainbowRacerPtr( dwPlayerId ) ? TRUE : FALSE; }		// 신청한 유저인가?
	int				GetApplicantNum() { return m_mapRainbowRace.size(); }	// 참가자 인원 수
	void			SetApplicationUser( CUser* pUser );		// 플레이어가 신청한 경우..
	void			SetApplication( DWORD dwPlayerId );		// 신청 등록
	BOOL			SetDropOut( DWORD dwPlayerId );			// 탈락
	DWORD			GetNextTime()				{ return m_dwNextTime; }			// 다음 단계로 넘어가는 시간
	void			OnMiniGamePacket( CUser* pUser, __MINIGAME_PACKET* pMiniGamePacket );	// 미니게임 패킷을 받은뒤 처리
	void			SetRanking( CUser* pUser );	// 레이스를 종료한 캐릭터를 순위에 등록
	void			SetPrevRanking( vector<DWORD> & vecPrevRanking );	// 지난 순위 등록
	vector<DWORD>	GetPrevRanking();
	
private:
	struct MINIGMAME_PRIZE_LIST
	{
		DWORD dwPlayerId;	int nCompletedNum;	DWORD dwCompletedTick;
		MINIGMAME_PRIZE_LIST( DWORD dwPI, int nCN, DWORD dwCT )
		: dwPlayerId( dwPI ), nCompletedNum( nCN ), dwCompletedTick( dwCT )	{}
	};
	vector<MINIGMAME_PRIZE_LIST>	m_vecMiniGamePrizeList;
	void	SetMiniGamePrize( DWORD dwTick );	// 미니게임완료 상품지급예약된 상품을 지급
public:
	void	SetMiniGamePrizeList( DWORD dwPlayerId, int nCompletedNum, DWORD dwCompletedTick )// 미니게임완료 상품지급 예약
	{ m_vecMiniGamePrizeList.push_back( MINIGMAME_PRIZE_LIST( dwPlayerId, nCompletedNum, dwCompletedTick ) ); }
	

private:
	BOOL	IsApplicationTime();		// 레인보우 레이스 신청 시간인가?
	BOOL	IsOpenTime();				// 레인보우 레이스 오픈 시간인가?
	void	BeginRainbowRace();			// 레인보우 레이스 시작을 클라에 알림(비접속자는 탈락)
	void	SetNPC();					// 진행 NPC를 세운다.
	void	RemoveNPC();				// 레이스가 완료되면 진행 NPC를 제거한다.
	void	SetPrize();					// 상품을 우편으로 전달한다.
	
	MAPRR					m_mapRainbowRace;	// 각 플레이어에 대한 상태 목록
	vector<CMiniGameBase*>	m_vecMiniGame;		// 미니게임 목록
	CLuaBase				m_Lua;

	int			m_nState;					// 현재 상태
	DWORD		m_dwNextTime;				// 다음 상태로 넘어가는 시간
	vector<OBJID> m_vecNPCId;				// 진행 NPC의 Id
	vector<DWORD> m_vecdwRankingId;			// 순위별 PlayerId
	vector<DWORD> m_vecPrevRanking;			// 지난 5위까지 순위
};
#endif __WORLDSERVER

#endif // __RAINBOW_RACE