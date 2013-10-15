#ifndef	__MOVERMSG_H__
#define	__MOVERMSG_H__


#define		OBJSTA_NONE					0x00000000	// 아무것도 아닌상태.

// stop/move
#define		OBJSTA_STAND				0x00000001	// 대기상태
#define		OBJSTA_STAND2				0x00000002	// 대기상태EX
#define		OBJSTA_SIT					0x00000003	// 앉기
#define		OBJSTA_FMOVE				0x00000004	// 앞으로 걷기
#define		OBJSTA_BMOVE				0x00000005	// 뒤로 걷기
#define		OBJSTA_LEFT					0x00000006	// 옆으로 걷기(수평이동)
#define		OBJSTA_RIGHT				0x00000007	// 
#define		OBJSTA_PICKUP				0x00000008	// (물건등을)줍다.

#define		OBJSTA_LMOVE				0x00000009	// 수평왼쪽으로 걷기
#define		OBJSTA_RMOVE				0x0000000a	// 스평오른쪽으로 걷기
#define		OBJSTA_STOP_RUN				0x0000000b	// 제자리 뛰기

#define		OBJSTA_MOVE_ALL				0x000000FF
#define		OBJSTA_NOT_STAND			(OBJSTA_MOVE_ALL & (~OBJSTA_STAND))		// STAND이외가 있는가.

// turn
#define		OBJSTA_LTURN				0x00000100	// 좌 턴
#define		OBJSTA_RTURN				0x00000200	// 우 턴
#define		OBJSTA_TURN_ALL				0x00000300	//
// look up/down
#define		OBJSTA_LOOKUP				0x00000400	//
#define		OBJSTA_LOOKDOWN				0x00000800	//
#define		OBJSTA_LOOK_ALL				0x00000C00

// jump
#define		OBJSTA_FJUMP_READY			0x00001000	// 점프전 도약 - 앞점프
#define		OBJSTA_FJUMP				0x00002000	// 점프중
#define		OBJSTA_FFALL				0x00003000	// 떨어지는중
#define		OBJSTA_FLAND				0x00004000	// 착지
#define		OBJSTA_SJUMP1				0x00005000	// 점프전 도약 - 제자리점프
#define		OBJSTA_SJUMP2				0x00006000	// 점프중
#define		OBJSTA_SJUMP3				0x00007000	// 떨어지는중
#define		OBJSTA_SJUMP4				0x00008000	// 착지
#define		OBJSTA_BJUMP_READY			0x00009000	// 점프전 도약 - 백점프
#define		OBJSTA_BJUMP				0x0000a000	// 점프중
#define		OBJSTA_BFALL				0x0000b000	// 떨어지는중
#define		OBJSTA_BLAND				0x0000c000	// 착지
#define		OBJSTA_JUMP_ALL				0x0000F000
// attack
#define		OBJSTA_ATK1					0x00010000	// 1번연타 공격동작중
#define		OBJSTA_ATK2					0x00020000	// 2번연타 공격동작중
#define		OBJSTA_ATK3					0x00030000	// ...
#define		OBJSTA_ATK4					0x00040000	// ...				// 나중에 "공격중"이라는 개념으로 통합될지도 모르겠다....
// wand attack
#define		OBJSTA_ATK_MAGIC1    		0x00050000	// 완드공격동작중
#define		OBJSTA_ATK_RANGE1    		0x00060000	// 원거리공격동작중
#define		OBJSTA_RANGE3				0x00070000	// 
#define		OBJSTA_RANGE4				0x00080000	// 
// melee skill
#define		OBJSTA_ATK_MELEESKILL		0x00090000	// 근접 전투스킬동작 중
#define		OBJSTA_ATK_RANGESKILL		0x000a0000	// 장거리 전투스킬동작 중
#define		OBJSTA_ATK_CASTING1			0x000b0000	// 마법 캐스팅 1번 (시작)
#define		OBJSTA_ATK_CASTING2			0x000c0000	// 마법 캐스팅 2번 (반복.)
#define		OBJSTA_ATK_MAGICSKILL		0x000d0000	// 마법 발사 동작.
#define		OBJSTA_SP_ATK1				0x00100000	// 특수공격 : 몬스터가 사용.
#define		OBJSTA_SP_ATK2				0x00200000	// 특수공격2 : 몬스터가 사용.
#define		OBJSTA_ATK_ALL				0x00FF0000
//#define		OBJSTA_RANGE_ALL			0x0f000000
// damage
#define		OBJSTA_DMG					0x01000000	// 피격
#define		OBJSTA_DMG_RESERVED1		0x04000000	
#define		OBJSTA_DMG_RESERVED2		0x05000000	

#define		OBJSTA_DMG_FLY				0x02000000	// 왕 피격 - 날아가기
#define		OBJSTA_DMG_DIE				0x03000000	// 왕 피격후 땅에 털썩!
#define		OBJSTA_DMG_LIVE				0x06000000	// 쓰러진 상태에서 다시 일어나기.
#define		OBJSTA_DMG_FLY_ALL			0x02000000	// 날아가는 데미지 모두. 다른곳에서 2번째 비트를 쓰지 않도록주의.

#define		OBJSTA_DEAD					0x08000000	// 죽어 널부러진 상태(죽은상태)
#define		OBJSTA_DISAPPEAR			0x09000000	// 죽어 없어지는 상태
#define		OBJSTA_RESURRECTION			0x0C000000	// 부활 0xd = 1101
#define		OBJSTA_DEAD_RESERVED1		0x0D000000	
#define		OBJSTA_DIE_ALL				0x08000000	// 죽음 - 0x00800000 비트가 온이면 죽은 상태.
#define		OBJSTA_DMG_ALL				0x0F000000	// 피격 & 죽음 모두

// 일반 action - 다른 어떤 명령도 이 액션중에는 영향을 못미친다.
#define		OBJSTA_COLLECT				0x10000000	// 채집.
#define		OBJSTA_APPEAR				0x20000000	// 등장씬.
#define		OBJSTA_APPEAR2				0x30000000  // 등장씬 2
#define		OBJSTA_STUN					0x40000000	// 스턴 상태.
#define		OBJSTA_ACTION_ALL			0xF0000000	// 

#define		OBJSTA_ALL	(OBJSTA_MOVE_ALL | OBJSTA_TURN_ALL | OBJSTA_LOOK_ALL | OBJSTA_JUMP_ALL | OBJSTA_ATK_ALL | OBJSTA_DMG_ALL | OBJSTA_ACTION_ALL )


// State Flag
#define		OBJSTAF_COMBAT				0x00000001	// 전투 모드
#define		OBJSTAF_WALK				0x00000002	// 걷는 모드
#define		OBJSTAF_SIT					0x00000004	// 앉기 상태
#define		OBJSTAF_FLY					0x00000008	// 비행 모드
#define		OBJSTAF_ACC					0x00000010	// 가속 상태
#define		OBJSTAF_ETC					0x00000020	// 커스텀 모션
#define		OBJSTAF_ACCTURN				0x00000040	// 급 선회.
#define		OBJSTAF_TURBO				0x00000080	// 터보 모드.

// Act Message
enum OBJMSG
{
	OBJMSG_NONE,
	OBJMSG_FORWARD,		// 전진 해라
	OBJMSG_BACKWARD,	// 후진 해라
	OBJMSG_STAND,		// 제자리에 서있어라
	OBJMSG_SITDOWN,		// 앉아라
	OBJMSG_STANDUP,		// 서라.(앉아있을때)
	OBJMSG_STOP,		// 멈춰라
	OBJMSG_ASTOP,		// 멈춰라
	OBJMSG_LTURN,		// 왼쪽으로 돌아라
	OBJMSG_RTURN,		// 오른쪽으로 돌아라
	OBJMSG_STOP_TURN,		// 도는걸 멈춰라
	OBJMSG_PICKUP,		// 줍다.
	OBJMSG_RESURRECTION,	// 부활.
	OBJMSG_COLLECT,		// 채집.
	OBJMSG_APPEAR,		// 등장
	OBJMSG_APPEAR2,		// 등장2
	OBJMSG_STUN,		// 스턴상태
	OBJMSG_JUMP,		// 점프
	OBJMSG_MODE_COMBAT,		// 전투모드가 되어라
	OBJMSG_MODE_PEACE,		// 평화모드가 되어라,
	OBJMSG_MODE_WALK,		// 걷기모드가 되어라
	OBJMSG_MODE_RUN,		// 뛰기모드가 되어라
	OBJMSG_MODE_FLY,		// 비행모드가 되어라
	OBJMSG_MODE_GROUND,		// 지상모드가 되어라
	OBJMSG_MODE_TURBO_ON,	// 순간 가속 모드.
	OBJMSG_MODE_TURBO_OFF,	// 가속모드 해제.
	OBJMSG_MODE_TURN,
	OBJMSG_ACC_START,	// 가속 시작
	OBJMSG_ACC_STOP,	// 가속 중지
	OBJMSG_ATK1,		// 연타공격 1시작
	OBJMSG_ATK2,		// 연타공격 2시작..
	OBJMSG_ATK3,
	OBJMSG_ATK4,
	OBJMSG_SP_ATK1,		// 특수공격 : 몬스터가 사용.
	OBJMSG_SP_ATK2,
	OBJMSG_ATK_RANGE1,	// 원거리공격 1시작 - 활같은...
	OBJMSG_ATK_MAGIC1,	// 완드공격
	OBJMSG_MELEESKILL,
	OBJMSG_MAGICSKILL,
	OBJMSG_MAGICCASTING,
	OBJMSG_DIE,		// 죽어라!
	OBJMSG_DAMAGE,			// 데미지먹음.
	OBJMSG_DAMAGE_FORCE,	// 강데미지 - 맞으면 날아감.
	OBJMSG_LOOKUP,
	OBJMSG_LOOKDOWN,
	OBJMSG_STOP_LOOK,
	OBJMSG_MOTION,
	OBJMSG_TURNMOVE,
	OBJMSG_TURNMOVE2,
	OBJMSG_BASE_RADY,		// 기본 시전 동작
	OBJMSG_TEMP,
	OBJMSG_TEMP2,	// 디버깅용 공격메시지
	OBJMSG_TEMP3,
	OBJMSG_DESTPOS,
	OBJMSG_DESTOBJ,
	OBJMSG_FALL,
	OBJMSG_LFORWARD,
	OBJMSG_RFORWARD,
	OBJMSG_STOP_RUN,
};

// 오브젝트 행동
enum OBJACT
{
	OBJACT_NONE,
//	OBJACT_MOVETO_OBJ,	
//	OBJACT_MOVETO_POS,
	OBJACT_MELEE_ATTACK,
	OBJACT_MAGIC_ATTACK,
	OBJACT_RANGE_ATTACK,
	OBJACT_SP_ATTACK,
	OBJACT_USESKILL,
	OBJACT_USEITEM,
	OBJACT_COLLECT,

};

#define		MVRF_NONE			0x00000000
#define		MVRF_SKILL			0x00000001		// 연속 스킬 사용중.
#define		MVRF_CRITICAL		0x00000002		// 크리티컬 모드. 1회용.
#define		MVRF_TRACKING		0x00000004		// 비행중 자동추적모드중.
#define		MVRF_WARMARK		0x00000008		// 전투마크가 머리위에 떴는지. 0이면 CreateSfx를 해주고 1로 만들어 준다.
#define		MVRF_COLLECT		0x00000010		// 채집 이펙트.
#define		MVRF_HITCONT		0x00000020		// 일반공격 타점이후부터 애니끝날때까지 계속 ON
#define		MVRF_LASERCHARGE	0x00000040		// 레이저 충전중.
#define		MVRF_EFFECT1		0x00000080		// 무버가 하나씩 달고다닐수 있게 하는 이펙트(라이젬같은넘..)
#define		MVRF_NOMOVE			0x00000100		// 이동금지. 제자리에서 이동못함.
#define		MVRF_NOATTACK		0x00000200		// 공격금지. 근접/완드/스킬등 공격 및 스킬을 사용 못함
#define		MVRF_NOUSERITEM		0x00000400		// 아이템사용금지. 
#define		MVRF_NOANI			0x00000800		// 애니메이션 금지.
#define		MVRF_NOACTION		(MVRF_NOMOVE | MVRF_NOATTACK | MVRF_NOUSERITEM)		// 액션금지( 이동/공격/스킬/아이템 모두 합한거 )
#define		MVRF_FREEZE			(MVRF_NOANI | MVRF_NOACTION)	// 애니메이션도 멈추고 동작도 다 멈춘다.
#define		MVRF_MEDITATION_SIT	0x00001000		// 메디테이션사용후 앉기 플래그 - 최초 한번만 앉아야 하기때문.
#define		MVRF_SETITEMSFX		0x00002000		// 세트 아이템 효과 이펙트


#define		ENEMY_PVP_PARTY		0		// 파티 듀얼
#define		ENEMY_WAR			1		// 길드전
#define		ENEMY_PK			2		// 피케이
#define		ENEMY_PVP			3		// 듀얼

#endif	// __MOVERMSG_H__
