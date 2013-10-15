#ifndef __GUILD_H__
#define	__GUILD_H__

#include "mempooler.h"
#include <map>
#include <string>
#include "ar.h"
#include "script.h"
#if defined(__DBSERVER) || defined(__CORESERVER)
#include "..\DatabaseServer\project.h"
#include "..\_Network\Objects\obj.h"
#endif

#include "guildquest.h"

////////////////////////////////////////////////////////////////////////////////
// 길드관련 상수 
////////////////////////////////////////////////////////////////////////////////

const int MAX_GM_ALIAS			= 48;
const int MAX_GM_LEVEL			= 5;
const int MAX_GM_SIZE			= 128;
const int MAX_SQL_G_NAME		= 16;
const int MAX_G_NAME			= 48;
const int MAX_BYTE_NOTICE		= 128;		// 길드공지사항 버퍼 최대길이 
const int MAX_GUILD_LEVEL		= 60;		// 길드 최대레벨 	
const int MAX_BYTE_VOTETITLE	= 26;		// 투표 제목 버퍼 최대 길이 
const int MAX_BYTE_VOTEQUESTION	= 171;		// 투표 내용 버퍼 최대 길이 
const int MAX_BYTE_VOTESELECT   = 21;		// 투표 선택사항 버퍼 최대 길이 
const int MAX_VOTE_ENTRY		= 20;		// 길드가 투표를 최대 몇개 가지고 있는가

#define GM_MASTER 1
#define GM_KINGPIN 5
#define GM_CAPTAIN 10
#define GM_SUPPORTER 20
#define GM_ROOKIE 80

const DWORD PF_MEMBERLEVEL		= 0x00000001;
const DWORD PF_LEVEL			= 0x00000002;
const DWORD PF_INVITATION		= 0x00000004;
const DWORD PF_PENYA			= 0x00000008;
const DWORD PF_ITEM				= 0x00000010;
#if __VER >= 15 // __GUILD_HOUSE
const DWORD PF_GUILDHOUSE_FURNITURE		= 0x00000020;
const DWORD PF_GUILDHOUSE_UPKEEP		= 0x00000040;
#endif // __GUILD_HOUSE

typedef struct _SGuildMsgHeader
{
	enum
	{
		GUILD_BANK		= 0x0001,
		PENYA			= 0x0001,
		ITEM			= 0x0002,
	};

	union
	{
		DWORD		HeadA;			//
		struct 
		{
			WORD	HeadASub;		//	길드 아이디
			WORD	HeadAMain;		//	업데이트될 총 타입 갯수
		};
	};

	union
	{
		DWORD		HeadB;			//	업데이트 타입
		struct 
		{
			WORD	HeadBSub;		//	용도에 맞게 쪼개 쓴다.
			WORD	HeadBMain;
		};
	};

}GUILD_MSG_HEADER, *LPGUILD_MSG_HEADER;

////////////////////////////////////////////////////////////////////////////////
//투표 관련 
////////////////////////////////////////////////////////////////////////////////

struct VOTE_INSERTED_INFO
{
	u_long  idGuild;									// 길드ID
	u_long	idVote;										// 투표ID
	char    szTitle[MAX_BYTE_VOTETITLE];				// 제목 
	char	szQuestion[MAX_BYTE_VOTEQUESTION];			// 투표질문내용 
	char	szSelections[4][MAX_BYTE_VOTESELECT];		// 선택문장 
};

extern CAr&  operator<<(CAr& ar, VOTE_INSERTED_INFO& info);
extern CAr&  operator>>(CAr& ar, VOTE_INSERTED_INFO& info);


// 길드 투표의 선택  
struct GUILD_VOTE_SELECT
{
	char szString[MAX_BYTE_VOTESELECT];		// 선택문장 
	BYTE cbCount;							// 선택count
};

// 길드 투표 
class CGuildVote
{
public:
	CGuildVote();
	virtual ~CGuildVote();
	void	Serialize( CAr & ar );
	bool	IsCompleted() const { return m_bCompleted; }
	void	SetComplete() { m_bCompleted = true; }
	void    Init( const VOTE_INSERTED_INFO& info, bool bCompleted, BYTE* cbCounts );
	void	Cast( BYTE cbSelect );

	u_long  GetID() const { return m_idVote; }
	LPCTSTR	GetTitle() const { return m_szTitle; }
	LPCTSTR	GetQuestion() const { return m_szQuestion; }
	GUILD_VOTE_SELECT& GetVoteSelect(int i) { return m_selects[i]; }

private:
	u_long	m_idVote;								// ID
	bool	m_bCompleted;							// 완료상태 : true, 진행중: false
	char    m_szTitle[MAX_BYTE_VOTETITLE];			// 제목 
	char	m_szQuestion[MAX_BYTE_VOTEQUESTION];	// 투표질문내용 
	GUILD_VOTE_SELECT	m_selects[4];				// 선택 
};

////////////////////////////////////////////////////////////////////////////////
//CONTRIBUTION_CHANGED_INFO 관련 
////////////////////////////////////////////////////////////////////////////////
struct CONTRIBUTION_CHANGED_INFO
{
	u_long idGuild;					// 공헌정보가 변경된 변경된 길드id
	u_long idPlayer;				// 공헌한 플레이어 
	DWORD dwPxpCount, dwPenya;		// 플레이어가 공헌한 pxp횟수, 페냐 
	DWORD dwGuildPxpCount, dwGuildPenya;	// 공헌에 의해 변경된 길드 pxp횟수, 페냐 
	WORD nGuildLevel;						// 길드레벨 
};

extern CAr&  operator<<(CAr& ar, CONTRIBUTION_CHANGED_INFO& info);
extern CAr&  operator>>(CAr& ar, CONTRIBUTION_CHANGED_INFO& info);

////////////////////////////////////////////////////////////////////////////////
//CGuildTable 관련 
////////////////////////////////////////////////////////////////////////////////

struct GUILD_TABLE_ENTRY
{
	DWORD	dwPxpCount;		// 필요공헌도 
	DWORD	dwPenya;		// 필요페냐 
	WORD	nMaxMember;		// 최대인원 
};

class CGuildTable
{
public:
	virtual ~CGuildTable();

	BOOL	Load(LPCTSTR szFileName);
	DWORD	GetPxpCount(WORD nLevel) const;
	DWORD	GetPenya(WORD nLevel) const;
	WORD	GetMaxMemeber(WORD nLevel) const;
	void	Serialize( CAr & ar );
	BOOL	ReadBlock( CScript & script );
	int		GetMaxLevel() const { return m_nCount; } 

	static CGuildTable& GetInstance();
	
private:
	CGuildTable();
	GUILD_TABLE_ENTRY	m_table[MAX_GUILD_LEVEL];
	int					m_nCount;					// m_table에 실제 데이타수 
};

////////////////////////////////////////////////////////////////////////////////
//CGuildMember 관련 
////////////////////////////////////////////////////////////////////////////////

class CGuildMember
{
public:
	u_long	m_idPlayer;						// Player ID
	int		m_nPay;							// 월급
	int		m_nGiveGold;					// 길드에 기부한 페냐
	DWORD	m_dwGivePxpCount;				// 길드에 기부한 PXP횟수( 스킬 경험치 )
	short	m_nWin;							// 무엇을 이겼지?
	short	m_nLose;						// 무엇을 졌을까나?
	char	m_szAlias[MAX_GM_ALIAS];	
	BYTE	m_nMemberLv;					// 나의 지위
#if __VER < 11 // __SYS_PLAYER_DATA
	DWORD	m_dwSex;						// 성별
	LONG	m_nJob;							// 직업
	LONG	m_nLevel;						// 레벨
	BYTE	m_nLogin;						// 로그인 상태( 1 ) / 로그 아웃( 0 )
	BYTE	m_nMultiNo;						// 멀티 번호
#endif	// __SYS_PLAYER_DATA
	u_long  m_idSelectedVote;				// 선택한 투표ID
	int		m_nSurrender;	// 항복 회수
	int		m_nClass;						// 등급 A, B, C

public:
//	Constructions
	CGuildMember();
	virtual ~CGuildMember();
//	Operations
	void	Serialize( CAr & ar );

	CGuildMember &	operator = ( CGuildMember & source );
	
#ifdef __CLIENT
	CMover *GetMover( void ) { return prj.GetUserByID( m_idPlayer ); }
#endif // client
public:
#ifndef __VM_0820
#ifndef __MEM_TRACE
	static	MemPooler<CGuildMember>*	sm_pPool;
	void*	operator new( size_t nSize )	{	return CGuildMember::sm_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CGuildMember::sm_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CGuildMember::sm_pPool->Free( (CGuildMember*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CGuildMember::sm_pPool->Free( (CGuildMember*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820
};

////////////////////////////////////////////////////////////////////////////////
//CGuild 관련 
////////////////////////////////////////////////////////////////////////////////

// 공헌판단결과 
enum CONTRIBUTION_RESULT						
{
	CONTRIBUTION_OK,							// 공헌가능 
	CONTRIBUTION_FAIL_MAXLEVEL,					// 공헌불가 - 최고 레벨 
	CONTRIBUTION_FAIL_GUILD_OVERFLOW_PXP,		// 공헌불가 - 길드 PXP overflow
	CONTRIBUTION_FAIL_GUILD_OVERFLOW_PENYA,		// 공헌불가 - 길드 Penya overflow
	CONTRIBUTION_FAIL_INVALID_CONDITION,		// 공헌불가 - 
	CONTRIBUTION_FAIL_OVERFLOW_PXP,				// 공헌불가 - 공헌한 PXP overflow
	CONTRIBUTION_FAIL_OVERFLOW_PENYA			// 공헌불가 - 공헌한 Penya overflow
};

#define	GF_WARTIME		(DWORD)0x00000001

typedef	struct	_GUILD_MEMBER_INFO
{
	u_long	idPlayer;
#if __VER < 11 // __SYS_PLAYER_DATA
	LONG	nLevel;
	LONG	nJob;
	DWORD	dwSex;
#endif	// __SYS_PLAYER_DATA
	BYTE	nMultiNo;
	_GUILD_MEMBER_INFO() 
	{	idPlayer	= 0;
#if __VER < 11 // __SYS_PLAYER_DATA
		nLevel	= nJob	= dwSex	= 0;
#endif	// __SYS_PLAYER_DATA
		nMultiNo	= 100;
	}
}
GUILD_MEMBER_INFO, *PGUILD_MEMBER_INFO;

class	CGuildWar;

class CGuild
{
public:
	u_long	m_idGuild;							// Guild ID
	char	m_szGuild[MAX_G_NAME];				// Guild Name
	map<u_long, CGuildMember*>	m_mapPMember;	// 회원들
	u_long	m_idMaster;							// 길드장 ID
	int		m_nLevel;							// Guild Level
	DWORD	m_adwPower[MAX_GM_LEVEL];			// 권한 설정값
	DWORD	m_adwPenya[MAX_GM_LEVEL];			// 페냐 설정값
	BOOL	m_bActive;							// 활동중? 중지중? 검사하는건가?
	DWORD   m_dwLogo;							// 로고 이미지 번호
	DWORD   m_dwContributionPxp;				// 공헌된 PXP
	char	m_szNotice[MAX_BYTE_NOTICE];		// 공지사항
	DWORD	m_dwFlag;							// 길드 비고
	BOOL	m_bSendPay;							// 길드 월급을 줬는지 확인
	
#if !defined( __CORESERVER)
	CItemContainer<CItemElem>	m_GuildBank;	// 길드 창고
#endif
	DWORD						m_nGoldGuild;	// 길드 창고 페냐 / 공헌된 페냐

	list <CGuildVote*>			m_votes;		// 투표데이타들 
	u_long	m_idEnemyGuild;						// 전쟁중인 적대길드
	int		m_nWinPoint;
	int		m_nWin;
	int		m_nLose;
	int		m_nSurrender;
	int		m_nDead;

#ifdef __CORESERVER
	BOOL	m_bLog;
#endif	// __CORESERVER

public:
//	Constructions
	CGuild();
	virtual ~CGuild();
	void	Clear( void );
//	Operations
//	If an error occurs, return nonzero.
	u_long	GetGuildId( void )		{ return m_idGuild; };
	BOOL	AddMember( CGuildMember* pMember );
	BOOL	RemoveMember( u_long idPlayer );
	BOOL	IsMember( u_long idPlayer )		{	return( GetMember( idPlayer ) != NULL );	}
	BOOL	IsMaster( u_long idPlayer )	{	return( m_idMaster == idPlayer );	}
	void	Serialize( CAr & ar, BOOL bDesc );
	BOOL	SetLogo( DWORD dwLogo );
	void	SetNotice( const char* szNotice );

	void	MeritResultMsg( CONTRIBUTION_RESULT cbResult );
	void	SetContribution( CONTRIBUTION_CHANGED_INFO & info );
	BOOL	AddContribution( DWORD dwPxp, DWORD dwPenya, u_long idPlayer );
	CONTRIBUTION_RESULT CanContribute( DWORD dwPxp, DWORD dwPenya, u_long idPlayer );
	void	DecrementMemberContribution( u_long idPlayer, DWORD dwPenya, DWORD dwPxpCount );

	void	AddVote( const VOTE_INSERTED_INFO& info, bool bCompleted = false, BYTE* cbCounts = NULL );
	CGuildVote* FindVote( u_long idVote );
	bool	ModifyVote( u_long idVote, BYTE cbOperation, BYTE cbExtra );
		
	//	길드 창고에서 페냐를 가져올수 있는지 확인
	BOOL	IsGetPenya( u_long idPlayer )	{	return m_adwPower[GetMember(idPlayer)->m_nMemberLv] & PF_PENYA; }
	//	길드 창고에서 아이템을 가져올수 있는지 확인
	BOOL	IsGetItem( u_long idPlayer )	{	return m_adwPower[GetMember(idPlayer)->m_nMemberLv] & PF_ITEM; }
	
	// 권한 검사
	BOOL	IsCmdCap( int nMemberLv, DWORD dwPower )	{	return( ( m_adwPower[nMemberLv] & dwPower )? TRUE: FALSE );	}

#if __VER >= 15 // __GUILD_HOUSE
	BOOL	IsAuthority( DWORD dwPlayerId, int nAuthority )	{ return GetMember(dwPlayerId) ? ( m_adwPower[GetMember(dwPlayerId)->m_nMemberLv] & nAuthority ) : FALSE; }
#endif // __GUILD_HOUSE
	//	Attributes
	int		GetSize( void )	{	return m_mapPMember.size();	}
	CGuildMember*	GetMember( u_long idPlayer );
	u_long	m_idWar;
	CGuildWar* GetWar( void );
	int		GetMaxMemberSize( void );

	int		GetMemberLvSize( int nMemberLv );
	int		GetMaxMemberLvSize( int nMemberLv );

	CGuild &	operator = ( CGuild & source );

	GUILDQUEST	m_aQuest[MAX_GUILD_QUEST];
	BYTE	m_nQuestSize;
	PGUILDQUEST		m_pQuest;
	PGUILDQUEST		FindQuest( int nQuestId );
	PGUILDQUEST		GetQuest( int nQuestId )	{	return FindQuest( nQuestId );	}
	BOOL	RemoveQuest( int nQuestId );
	void	SetQuest( int nQuestId, int nState );
#ifdef __WORLDSERVER
	void	ReplaceLodestar( const CRect & rect );
	void	Replace( DWORD dwWorldId, D3DXVECTOR3 & vPos, BOOL bMasterAround = FALSE );
#endif	// __WORLDSERVER

#ifndef __CORESERVER
	CItemElem*	GetItem( DWORD dwItemId, SERIALNUMBER iSerialNumber );
	short	RemoveItem( DWORD dwId, short nItemNum );
#endif	// __CORESERVER

	static	int	sm_anMaxMemberSize[MAX_GUILD_LEVEL];
	static	int	sm_anMaxMemberLvSize[MAX_GM_LEVEL];
public:
#ifndef __VM_0820
#ifndef __MEM_TRACE
	static	MemPooler<CGuild>*	sm_pPool;
	void*	operator new( size_t nSize )	{	return CGuild::sm_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CGuild::sm_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CGuild::sm_pPool->Free( (CGuild*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CGuild::sm_pPool->Free( (CGuild*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820
};

////////////////////////////////////////////////////////////////////////////////
//CGuildMng 관련 
////////////////////////////////////////////////////////////////////////////////

class CPlayer;

class CGuildMng
{
public:
	u_long	m_id;								// load
	map<u_long, CGuild*>	m_mapPGuild;		// Guild ID를 이용하여 길드 얻음
	map<string, CGuild*>	m_mapPGuild2;		// Guild Name를 이용하여 길드 얻음
#if !defined(__WORLDSERVER) && !defined(__CLIENT)
	CRIT_SEC	m_AddRemoveLock;
#endif

public:
//	Constructions
	CGuildMng();
	virtual ~CGuildMng();
	void	Clear( void );
	u_long	AddGuild( CGuild* pGuild );
	BOOL	RemoveGuild( u_long idGuild );
	CGuild*		GetGuild( u_long idGuild );
	CGuild*		GetGuild( const char* szGuild );
	void	Serialize( CAr & ar, BOOL bDesc );
	BOOL	SetName( CGuild* pGuild, const char* szName );
public:
#ifdef __WORLDSERVER
	BOOL	m_bSendPay;
	void	Process( void );
#endif // __WORLDSERVER
#ifdef __CORESERVER	
	void	AddConnection( CPlayer* pPlayer );
	void	RemoveConnection( CPlayer* pPlayer );
#endif	// __CORESERVER
//	Attributes
	int		GetSize( void )	{	return m_mapPGuild.size();	}
};



enum {	GUILD_PENYA_MINE_UPDATE, GUILD_ITEM_MINE_UPDATE, GUILD_PENYA_ALL_UPDATE, GUILD_ITEM_ALL_UPDATE };
enum { GUILD_PUT_ITEM, GUILD_GET_ITEM, GUILD_PUT_PENYA, GUILD_GET_PENYA, GUILD_CLOAK, GUILD_QUERY_REMOVE_GUILD_BANK };


#define MAX_RANK_LIST	20

#ifdef __DBSERVER
extern	APP_INFO	g_appInfo;
#endif

////////////////////////////////////////////////////////////////////////////////
//	CGuildRank 
////////////////////////////////////////////////////////////////////////////////
class CGuildRank
{
public:
	/*
	 *	R1 : 최강길드
	 *	R2 : 최다승
	 *	R3 : 최다패
	 *	R4 : 최다항복패
	 *	R5 : 최고결속력
	 *	R6 : 최고자금
	 *	R7 : 평균고렙
	 *	R8 : 최대플레이
	 */
	enum RANKING
	{
		R1, R2, R3, R4, R5, R6, R7, R8, RANK_END
	};

	// 랭크 정보 구조체
	typedef struct _SGuildRanking
	{
		int			m_dwLogo;
		char		m_szGuild[17];
		char		m_szName[33];
		int			m_nWin;
		int			m_nLose;
		int			m_nSurrender;
		float		m_AvgLevel;
		int			m_nWinPoint;
	}GUILD_RANKING, *LPGUILD_RANKING;
	
	GUILD_RANKING	m_Ranking[RANK_END][MAX_RANK_LIST];
	int				m_Total[RANK_END];
	DWORD			m_Version;

#if !defined(__WORLDSERVER) && !defined(__CLIENT)
	CRIT_SEC		m_Lock;
#endif
	CTime			m_UpdateTime;


	GUILD_RANKING*	operator[](int i)		{	return m_Ranking[i]; }

public:
	CGuildRank()									{	m_Version = 0; m_UpdateTime = CTime::GetCurrentTime();}
	~CGuildRank()									{	;}

	static CGuildRank* Instance();
#ifdef __DBSERVER
	/*	
	 *	TRANS 서버일때에만 Rank함수를 Call 할수 있다.
	 */
	BOOL GetRanking(CQuery* pQuery, LPCTSTR p_strQuery)
	{
		m_Lock.Enter( theLineFile );

		m_UpdateTime = CTime::GetCurrentTime();

		m_Version++;
		ZeroMemory(m_Ranking, sizeof(GUILD_RANKING)*MAX_RANK_LIST*RANK_END);
		ZeroMemory(m_Total, sizeof(int)*RANK_END);

		pQuery->Clear();

		for ( int i=R1; i<RANK_END; ++i )
		{
			sprintf(const_cast<char*>(p_strQuery), "RANKING.RANKING_DBF.dbo.RANKING_STR 'R%d','%02d'", i+1, g_appInfo.dwSys );
			if( FALSE == pQuery->Exec( p_strQuery ) )
			{
				Error( "CDbManager::UpdateGuildRanking에서 (%s) 실패", p_strQuery );
				m_Lock.Leave( theLineFile );
				return FALSE;
			}

			int& j = m_Total[i];
			for ( ; pQuery->Fetch() == TRUE ; ++j )
			{
				m_Ranking[i][j].m_dwLogo		= pQuery->GetInt("m_dwLogo");
				
				pQuery->GetStr( "m_szGuild"		, m_Ranking[i][j].m_szGuild );
				pQuery->GetStr( "m_szName"		, m_Ranking[i][j].m_szName );

				m_Ranking[i][j].m_nWin			= pQuery->GetInt("m_nWin");
				m_Ranking[i][j].m_nLose			= pQuery->GetInt("m_nLose");
				m_Ranking[i][j].m_nSurrender	= pQuery->GetInt("m_nSurrender");			
				m_Ranking[i][j].m_AvgLevel		= pQuery->GetFloat("m_AvgLevel");				
				m_Ranking[i][j].m_nWinPoint		= pQuery->GetInt("m_nWinPoint");
			}
			
			pQuery->Clear();
		}

		m_Lock.Leave( theLineFile );

		return TRUE;
	}

	
	BOOL RankingDBUpdate(CQuery* pQuery, LPCTSTR p_strQuery)
	{
		m_Lock.Enter( theLineFile );

		pQuery->Clear();

		sprintf(const_cast<char*>(p_strQuery), "MAKE_RANKING_STR '%d'", g_appInfo.dwSys );
		if( FALSE == pQuery->Exec( p_strQuery ) )
		{
			Error( "CDbManager::RankingDBUpdate (%s) 실패", p_strQuery );
			m_Lock.Leave( theLineFile );
			return FALSE;
		}

		m_Lock.Leave( theLineFile );

		return TRUE;
	}
#endif//__DBSERVER

	void Serialize( CAr & ar )
	{
#if !defined(__WORLDSERVER) && !defined(__CLIENT)
		m_Lock.Enter( theLineFile );
#endif

		if( ar.IsStoring() )
		{
			ar << m_Version;
			ar.Write( m_Total, sizeof(int)*RANK_END );
			for ( int i=R1; i<RANK_END; i++ )
			{
				if ( m_Total[i] )
				{
					ar.Write( m_Ranking[i], sizeof(GUILD_RANKING)*m_Total[i] );
				}
			}
		}
		else
		{
			ar >> m_Version;
			ar.Read( m_Total, sizeof(int)*RANK_END );
			for ( int i=R1; i<RANK_END; i++ )
			{
				if ( m_Total[i] )
				{
					ar.Read( m_Ranking[i], sizeof(GUILD_RANKING)*m_Total[i] );
				}
			}
		}

#if !defined(__WORLDSERVER) && !defined(__CLIENT)
		m_Lock.Leave( theLineFile );
#endif	// __WORLDSERVER
	}
};

#endif	// __GUILD_H__