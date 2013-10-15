#ifndef __MESSENGER_H__
#define	__MESSENGER_H__

#include "Ar.h"

#define FRS_ONLINE		0	// 기본 : 게임중 
#define FRS_OFFLINE		1	// 자동 : 로그오프
#define FRS_BLOCK	2	// 설정 : 차단 
#define FRS_ABSENT	3	// 설정 : 자리비움
#define FRS_HARDPLAY	4	// 설정 : 열랩중 
#define FRS_EAT		5	// 설정 : 식사중 
#define FRS_REST	6	// 설정 : 휴식중
#define FRS_MOVE	7	// 설정 : 이동중 
#define FRS_DIE		8	// 자동 : 사망 
#define FRS_DANGER		9	// 자동 : 위험
#define FRS_OFFLINEBLOCK	10
#define	FRS_AUTOABSENT		11	// 자동 : 자동 자리비움	
#define MAX_FRIENDSTAT		12	// 맥스값

#define MAX_FRIEND		200 // 맥스값 친구 등록 횟수

typedef struct tagFRIEND
{
	BOOL bGroup;	// TRUE이면 그룹, 아니면 유저 
	DWORD dwState;	// 상태 0 이면 정상, 1이면 disconnect, 2이면 자리 비움.....
	DWORD dwUserId;
#if __VER < 11 // __SYS_PLAYER_DATA
	BYTE  nSex;
	LONG  nJob;
	TCHAR szName[ 64 ];
#endif	// __SYS_PLAYER_DATA
	BOOL  bSave;
#ifdef __CLIENT
	u_long m_uIdofMulti;
#endif // __CLIENT

	tagFRIEND()
	{
		bGroup = FALSE;
		dwState = FRS_OFFLINE;
		dwUserId = 0;
#if __VER < 11 // __SYS_PLAYER_DATA
		nSex = 0;
		nJob = 0;
		szName[0] = '\0';
#endif	// __SYS_PLAYER_DATA
		bSave = FALSE;
#ifdef __CLIENT
		m_uIdofMulti = 100;
#endif // __CLIENT
	}
} FRIEND,* LPFRIEND;

typedef	map< u_long, FRIEND*>	C2FriendPtr;

class CMessenger
{
public:
	C2FriendPtr m_aFriend; // 내가 등록한 유저들( 친구 등록 )
	C2FriendPtr m_adifferntFriend; // 나를 등록한 유저들
	DWORD		m_dwMyState;

public:
#if __VER >= 11 // __SYS_PLAYER_DATA
	BOOL		AddFriend( u_long uidPlayer );
	BOOL		AddFriend( u_long uidPlayer, DWORD dwSate, BOOL bSave = FALSE );
#else	// __SYS_PLAYER_DATA
	BOOL		AddFriend( u_long uidPlayer, LONG nJob, BYTE nSex );
	BOOL		AddFriend( u_long uidPlayer, LONG nJob, BYTE nSex, LPSTR lpName );
	BOOL		AddFriend( u_long uidPlayer, LONG nJob, BYTE nSex, DWORD dwSate, BOOL bSave = FALSE );
#endif	// __SYS_PLAYER_DATA
	void		SetSave( void );
	BOOL		RemoveFriend( u_long uidPlayer );
	BOOL		RemoveDifferntFriend( u_long uidPlayer );
	LPFRIEND	GetFriend( u_long uidPlayer );
	LPFRIEND	GetDefferntFriend( u_long uidPlayer );
	BOOL		IsFriend( u_long uidPlayer );
	BOOL		IsDefferntFriend( u_long uidPlayer );
	BOOL		AddDefferntFriend( u_long uidPlayer );
	void		Clear( void );
	void		Serialize( CAr & ar );
	DWORD		GetState() { return m_dwMyState; };
	LPFRIEND    GetAt( int nIndex );
	int			GetSize();
	virtual	CMessenger&	CMessenger::operator=( CMessenger & rMessenger );

	CMessenger();
	~CMessenger();
};

#endif	//	_MESSENGER_H