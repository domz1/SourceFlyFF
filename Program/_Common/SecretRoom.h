// SecretRoom.h: interface for the CSecretRoom class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SECRETROOM_H__E848C037_E4D5_44ED_BC88_2CDBE643B64F__INCLUDED_)
#define AFX_SECRETROOM_H__E848C037_E4D5_44ED_BC88_2CDBE643B64F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if __VER >= 12 // __SECRET_ROOM
#include "Continent.h"
////////////////////////////////////////////////////////////////

enum { SRMNG_CLOSE = 1, SRMNG_OPEN, SRMNG_LINEUP, SRMNG_ENTRANCE, SRMNG_WARWAIT, SRMNG_WAR };
enum { SRCONT_CLOSE = 101, SRCONT_OPEN, SRCONT_WAR, SRCONT_CLOSEWAIT };

#define MONSTER_FAILED	0
#define MONSTER_NORMAL	1
#define MONSTER_MIDBOSS	2
#define MONSTER_BOSS	3
#define MONSTER_WIN		4



struct __SECRETROOM_MONSTER
{
	DWORD	dwId;
	int		nNum;
	BOOL	bRed;
	int		x1, z1, x2, z2, y;
	__SECRETROOM_MONSTER()
	{
		dwId = NULL_ID;
		bRed = FALSE;
		nNum = x1 = z1 = x2 = z2 = y = 0;
	}
};


struct __SECRETROOM_TENDER
{
	DWORD	dwGuildId;
	int		nPenya;
	int		nKillCount;
	DWORD	dwWorldId;
	int		nWarState;
	vector<DWORD>	vecLineUpMember;
	vector<__SECRETROOM_MONSTER> vecMonster;
	__SECRETROOM_TENDER()
	{
		dwGuildId = NULL_ID;
		nPenya = 0;
		nKillCount = 0;
		dwWorldId = NULL_ID;
		nWarState = MONSTER_NORMAL;
	}

	bool operator ==( DWORD dwGuildId )
	{
		return this->dwGuildId == dwGuildId;
	}
};

class CSecretRoomContinent;
class CSecretRoom;

class CSecretRoomMng
{
public:	
	CSecretRoomMng();
	virtual ~CSecretRoomMng();

	static CSecretRoomMng*	GetInstance( void );
	
	BYTE GetContinent( CMover* pMover );
	BOOL IsInTheSecretRoom( CMover* pMover );

	map<BYTE, CSecretRoomContinent*>	m_mapSecretRoomContinent;
	
	int	m_nMinGuildLevel;			// 참여 가능 최소 길드 레벨
	int m_nMinGuildMemberLevel;		// 참여 길드원의 최소 레벨
	int m_nMinPenya;				// 최소 입찰 페냐
	int m_nMinGuildNum;				// 참여 가능 길드 수
	int m_nMaxGuildNum;				// 참여 가능 길드 수
	int m_nMinGuildMemberNum;		// 최소 참여 길드원 수
	int m_nMaxGuildMemberNum;		// 최대 참여 길드원 수
	int m_nCancelReturnRate;		// 입찰 취소 페냐 반환률
	int m_nDropoutReturnRate;		// 입찰 실패 페냐 반환률

	int m_nMonsterGenNum;			// 중간 보스 출현 조건 몬스터 수

	int		m_nState;				// 현재 state
	DWORD	m_dwRemainTime;			// 현재 state의 남은 시간

	map<int, int>	m_mapMonsterNum;	// 각 몬스터의 종 개체수(지정된 개체수씩 생성하기 때문에 필요하다.)
#ifdef __CLIENT
	CSecretRoomContinent* m_pSRCont;
#endif // __CLIENT
#ifdef __WORLDSERVER	
	BOOL LoadScript();
	void Process();			//  진행 순서가 정의되어 있으니까 코드 수정할때 어려우면 필요하면 여기부터 확인하자!!!
	BOOL CheckOpenTime();	// 비밀의 방 오픈시간인지 검사
	void SendNowState( CUser* pUser, int nState, DWORD dwRemainTime );
	void SendNowStateAllMember( BYTE nCont = CONT_NODATA, int nState = 0, DWORD dwRemainTime = 0 );
	void GetAllInfo( CUser* pUser );				// 접속시 현재 진행중인 구성원이면 비밀의 방 현재 정보를 전송
	BOOL IsPossibleGuild( CUser* pUser );			// 입찰 가능한 길드인가?
	BOOL IsGuildMaster( CUser* pUser );				// 길드 마스터 인가?
	BOOL IsOtherTenderGuild( CUser* pUser );		// 다른 대륙에 입찰한 길드인가?
	void SetTenderOpenWnd( CUser* pUser );			// 비밀의 방 신청창을 열때 필요한 정보 전송
	void SetTender( CUser* pUser, int nPenya );		// 입찰
	void SetTenderCancelReturn( CUser* pUser );		// 입찰 취소시 수수료를 제외한 신청금을 반환한다.
	void SetLineUpOpenWnd( CUser* pUser );			// 참가자 구성 창을 열때 필요한 정보 전송
	void SetLineUp( CUser* pUser, vector<DWORD> vecLineUpMember );	// 참가자 구성
	void GetTenderView( CUser* pUser );				// 입찰현황 창을 열때 
	void SecretRoomOpen();							// 금주 비밀의 방을 오픈한다.
	void SetTeleportSecretRoom( CUser* pUser );		// 비밀의 방으로 텔레포트...
	void SetSecretRoomKill( DWORD dwWorldId, DWORD dwMonsterId );	// 비밀의 방에서 몬스터를 kill했을때 처리
	void SetFailGuild( CUser* pUser );				// 길마가 죽거나 로그 아웃시 패배 처리
	D3DXVECTOR3 GetRevivalPos( CUser* pUser );		// 캐릭터 사망시, 종료시, 로그 아웃시 각 캐릭터 부활위치

	CLuaBase m_Lua;

	vector<__SECRETROOM_MONSTER> m_vecNormalMonster;	// 일반 몬스터 종류, 위치, 개체수, 선공 속성
	vector<__SECRETROOM_MONSTER> m_vecMidBossMonster;	// 중간보스 몬스터 종류, 위치, 개체수, 선공 속성
	vector<__SECRETROOM_MONSTER> m_vecBossMonster;		// 최종보스 몬스터 종류, 위치, 개체수, 선공 속성
#endif // __WORLDSERVER
};


class CSecretRoomContinent  
{
public:
	CSecretRoomContinent( BYTE nCont );
	virtual ~CSecretRoomContinent();

	// CSecretRoomMng를 통해 들어오는 함수가 거의 대부분이다.

	int		m_nState;
	DWORD	m_dwRemainTime;
	BYTE	m_nContinent;	// 현재 진행되는 비밀의 방이 해당하는 대륙(동부, 서부)
	vector<__SECRETROOM_TENDER> m_vecSecretRoomTender;		// 신청 길드 목록

	int		GetTenderGuild( DWORD dwGuildId );			// 신청한 길드이면 m_vecSecretRoomTender의 index를 반환한다.
#ifdef __WORLDSERVER
	void	Process( DWORD dwTick );	
	BOOL	IsLineUpMember( CUser* pUser );				// 구성된 참가자 인가?
	void	SetContTender( CUser* pUser, int nPenya );	// 실제 해당 대륙에 입찰
	void	SortTenderGuild();							// 입찰금 순으로 정렬
	void	SetContTenderCancelReturn( CUser* pUser );	// 입찰 취소시 반환
	void	SetContLineUp( CUser* pUser, vector<DWORD> vecLineUpMember );	// 참가자 구성
	void	SecretRoomContOpen();							// 실제 대륙별 비밀의 방 오픈
	void	SetSecretRoomWorld( int nContNum );				// 각 길드를 비밀의 방 16개중에 지정해 준다.
	void	SetTeleportSecretRoomCont( CUser* pUser );		// 해당 비밀의 방으로 텔레포트...
	void	SetTeleportNormalRoom( CUser* pUser, int nIndex );		// 일반 몬스터 위치로 텔레포트...
	void	SetTeleportBossRoom( CUser* pUser, int nIndex );		// 최종보스인 경우 텔레포트 위치가 다르다.
	void	CreateMonster();			// 몬스터를 지정한 갯수(스크립트)만큼씩 단계적으로 생성한다.
	void	RemoveAllSecretRoomObj( DWORD dwWorldId );		// 비밀의 방에서 패배하거나 승리길드가 나오면 모든 몬스터 제거
	void	SetSecretRoomContKill( DWORD dwWorldId, DWORD dwMonsterId, BOOL bKill = TRUE );	// 몬스터 kill시 처리(승리 길드, 보스단계 도 처리한다.)
	BOOL	IsSecretRoomMonster( DWORD dwMonsterId, int nWarState );	// 몬스터 kill시 해당 몬스터가 비밀의 방 몬스터인가?
	void	UpdateInfoToMember( CUser* pUser, BYTE nType, int nIndex = NULL_ID );	// kill count를 client에 알려준다.
	void	UpdateInfoToAllMember( BYTE nType, int nIndex = NULL_ID );		// 위 정보를 구성된 모든 client에 전송(해당 대륙만)
	void	ProcessInGuildMaster();	// 길마가 해당 비밀의 방에 존재하는 검사
	void	SetContFailGuild( DWORD dwGuildId );			// 길마가 죽거나 로그 아웃시 패배 처리
	void	SetContCloseWait();								// 비밀의 방이 종료되고 대기시간...
	void	SetContClose();									// 비밀의 방 최종 종료

	int		m_nFailedGuild;	 //패배한 길드 수(신청길드 수와 같으면 승리 길드 없이 종료)
#endif // __WORLDSERVER

};

/////////////////////////////////////////////////////////////////////
#endif // __SECRET_ROOM

#endif // !defined(AFX_SECRETROOM_H__E848C037_E4D5_44ED_BC88_2CDBE643B64F__INCLUDED_)


