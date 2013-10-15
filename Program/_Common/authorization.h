#ifndef __AUTHORIZATION_H
#define __AUTHORIZATION_H

// AUTHORAZATION 정의들. ADMINISTRATOR에 가까울 수룩 수퍼 계정이다.

#define AUTH_OBSERVER      'D' // 관찰자 계정 ; 방송용, 모니터링, 감시용
#define AUTH_GENERAL       'F' // 일반 사용자 유저
#define AUTH_LOGCHATTING   'G' // 채팅로그를 남길 일반유저 
#define AUTH_JOURNALIST    'H' // 게임을 취재하기 위한 기자 권한
#define AUTH_HELPER        'J' // 준 운영자 권한 ; 사용자를 돕기 위한 게임 마스터
#define AUTH_GAMEMASTER    'L' // 게임 마스터 ; 게임 전체의 진행이 가능한 계정 GM_LEVEL_1
#define AUTH_GAMEMASTER2   'M' // 게임 마스터 ; 게임 전체의 진행이 가능한 계정 GM_LEVEL_2
#define AUTH_GAMEMASTER3   'N' // 게임 마스터 ; 게임 전체의 진행이 가능한 계정 GM_LEVEL_3
#define AUTH_OPERATOR      'O' // 더미 계정 ; 현재는 AUTH_ADMINISTRATOR과 같은 기능으로 세팅 GM_LEVEL_4
#define AUTH_ADMINISTRATOR 'P' // 관리자 계정 ; 모든 기능을 사용할 수 있음. GM_LEVEL_4


// 모드(무적, 투명화 등등)CMover::m_dwMode를 사용함. 디파인에 말머리좀 있으면 좋겠네. -xuzhu-
// 1, 2, 4, 8, 10, 20, 40...
#define MATCHLESS_MODE			(DWORD)0x00000001	// 무적 상태
#define TRANSPARENT_MODE		(DWORD)0x00000002	// 투명 상태
#define ONEKILL_MODE			(DWORD)0x00000004	// 초필 상태
#define DONMOVE_MODE			(DWORD)0x00000008	// 움직이지 못하는 상태
#define SAYTALK_MODE			(DWORD)0x00000010	// 귓속말 못하는 상태
#define MATCHLESS2_MODE			(DWORD)0x00000020	// 무적 상태2 (데미지는 입지만 죽지는 않는다.)
#define NO_ATTACK_MODE			(DWORD)0x00000040	// 공격 못하는 상태
#define ITEM_MODE				(DWORD)0x00000080	// 아이템 못버리고 못집는 모드
#define COMMUNITY_MODE			(DWORD)0x00000100	// 길드, 파티, 친구, 개인간 거래, 개인상점 할수 없음
#define TALK_MODE				(DWORD)0x00000200	// 일반 말 못하는 상태
#define SHOUTTALK_MODE			(DWORD)0x00000400	// 외치기 못하는 상태
#define RECOVERCHAO_MODE		(DWORD)0x00000800	// 카오 극복 모드
#define	FREEPK_MODE				(DWORD)0x00001000	// CTRL키 누름 없이 pk가능
#define	PVPCONFIRM_MODE			(DWORD)0x00002000	// PVP거절 상태
#define	QUERYSETPLAYERNAME_MODE	(DWORD)0x00004000	// 캐릭터 명 변경 가능 모드
#define	MODE_MAILBOX			(DWORD)0x00008000	// 읽지 않은 편지

#define	EVENT_OLDBOY_MODE		(DWORD)0x00010000
#define	EQUIP_DENIAL_MODE		(DWORD)0x00020000

#define MODE_EXPUP_STOP			(DWORD)0x00040000		// 경험치 상승 금지모드.

#define GCWAR_NOT_CLICK_MODE	(DWORD)0x00080000		// 타인이 자신을 클릭 못한다...(피킹 패스)
#define GCWAR_RENDER_SKIP_MODE	(DWORD)0x00100000		// 랜더링 스킵

#define	MODE_OPTION_DONT_RENDER_MASK	(DWORD)0x01000000	// 마스크 랜더 여부


#define FRESH_MODE			(DWORD)0x10000000	// 처음 접속하는 케릭터냐
#define NOTFRESH_MODE		(DWORD)0x20000000	// 얼굴 바꾸기에 필요
#define NOTFRESH_MODE2		(DWORD)0x40000000	// 얼굴 바꾸기에 필요2

#define	MODE_OUTOF_PARTYQUESTRGN	(DWORD)0x80000000	// 파티 퀘스트 밖으로 

#define DONTALK_MODE		(DWORD)0x00000610	// 말 못하는 상태 TALK_MODE, SAYTALK_MODE, SHOUTTALK_MODE 합한것
#define OBSERVE_MODE		(DWORD)0x000000c0	// COMMUNITY_MODE, ITEM_MODE, ATACK_MODE, SHOUTTALK_MODE, SAYTALK_MODE 합한것
#define	ALL_MODE			(DWORD)0x000005d0

#define	QUIZ_RENDER_SKIP_MODE		(DWORD)0x00200000	// 퀴즈이벤트 지역에서의 랜더링 스킵


// PK, PVP, 시전CMover::m_dwStateMode사용.
#define STATE_PK_MODE			(DWORD)0x00000001	// PK 상태
#define STATE_PVP_MODE			(DWORD)0x00000002	// PVP 상태
#define STATE_BASEMOTION_MODE	(DWORD)0x00000004	// 시전중인 상태

#define STATE_BASEMOTION		(DWORD)0x0000000c	// STATE_BASEMOTION_MODE + STATE_BASEMOTION_END_MODE

#define STATEMODE_BASEMOTION_ON			(BYTE)0x00
#define STATEMODE_BASEMOTION_OFF		(BYTE)0x01
#define STATEMODE_BASEMOTION_CANCEL		(BYTE)0x02


#endif