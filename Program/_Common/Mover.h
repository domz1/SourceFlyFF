#if !defined(AFX_MOVER_H__4B7B21D7_A2D3_4115_946C_68DC9045A845__INCLUDED_)
#define AFX_MOVER_H__4B7B21D7_A2D3_4115_946C_68DC9045A845__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if __VER >= 11 // __SYS_POCKET
#include "pocket.h"
#endif	// __SYS_POCKET

#ifdef __BUFF_1107
#include "buff.h"
#endif	// __BUFF_1107

#include "Ctrl.h"
#include "..\_AIInterface\ActionMover.h"
#include "SkillInfluence.h"
#include "CooltimeMgr.h"
#include "mempooler.h"

#ifdef __WORLDSERVER
	#include "NpcProperty.h"
	#include "SfxHitArray.h"
	#include "..\_AIInterface\AIInterface.h"
#else
	#include "CooltimeMgr.h"
#endif

typedef	MemPooler<CMover>	CMoverPool;

#define	MAX_NAME				64

#define COLOR_GUILD_FARBE_10        0xFFFFD700 
#define COLOR_GUILD_FARBE_20        0xFFFF00FF 
#define COLOR_GUILD_FARBE_30        0xFF32CD32 
#define COLOR_GUILD_FARBE_40        0xFF00BFFF 
#define COLOR_GUILD_FARBE_50        0xFFFF0000
#define COLOR_GUILD_FARBE_60	    0xFF000000  

#define	COLOR_PLAYER			0xffffffff
#define COLOR_GAMEMASTER        0xff2D7CAE
#define COLOR_ADMINISTRATOR     0xffAE2D2D
#define	COLOR_NPC				0xffa0a0ff  
#define	COLOR_MONSTER			0xffffffa0
#define	COLOR_PARTY				0xff70f070
#define COLOR_GUILDCOMBAT		0xff00ff00

/// SetMotion() option
#define	MOP_NONE				0x00000000
#define	MOP_SWDFORCE			0x00000001		/// 검광 생성하게
#define	MOP_FIXED				0x00000002		/// dwMotion값을 그대로 로딩하게 ( +100 안하게.. )
#define	MOP_NO_TRANS			0x00000004		/// 모션 트랜지션 하지않음.
#define	MOP_HITSLOW				0x00000008		/// 타점이후부터 슬로우.

#define	REQ_USESKILL			0x00000001		/// 응답요청 플래그 - 스킬사용 응답
#define	REQ_MOTION				0x00000002		/// 응답요청 플래그 - 모션 응답

#define	OBJTYPE_PLAYER			0x00000001		
#define	OBJTYPE_MONSTER			0x00000002		
#define OBJTYPE_CTRL			0x00000004		
#define	OBJTYPE_FLYING			0x00010000		/// m_dwTypeFlag에서 사용.

#define	CRITICAL_PROBABILITY	3
#define	CRITICAL_BERSERK_HP		30
#define	CRITICAL_BERSERK_PROB	22
#define	CRITICAL_AFTER_DEATH	22

// UPDATEITEM
#define	UI_NUM					0
#define	UI_HP					1
#define	UI_RN					2
#define	UI_AO					3
#define	UI_RAO					4
#define	UI_IR					5
#define	UI_PIERCING_SIZE		6
#define	UI_PIERCING				7
#define UI_COOLTIME				8
#define	UI_FLAG					9
#define	UI_RANDOMOPTITEMID		10
#define	UI_KEEPTIME				11
#define UI_REPAIR_NUM			12

#if __VER >= 12 // __EXT_PIERCING
#define UI_ULTIMATE_PIERCING_SIZE	13
#define UI_ULTIMATE_PIERCING		14
#endif // __EXT_PIERCING

#if __VER >= 15 // __PETVIS
#define UI_PETVIS_SIZE			15
#define UI_PETVIS_ITEM			16
#define UI_PETVIS_ITEMSWAP		17
#define UI_TRANSFORM_VISPET		18

const int PETVIS_DEFAULT_VIS_SLOT_SZIE	=	2;
#endif // __PETVIS

#ifdef __ADD_RESTATE_LOW
#define RESTATE_LOW				50
#endif // __ADD_RESTATE_LOW

#ifdef __3RD_LEGEND16
#define MAX_SKILL_JOB	( MAX_JOB_SKILL + MAX_EXPERT_SKILL + MAX_PRO_SKILL + MAX_MASTER_SKILL + MAX_HERO_SKILL + MAX_LEGEND_HERO_SKILL )
#else // __3RD_LEGEND16
#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
#define MAX_SKILL_JOB	( MAX_JOB_SKILL + MAX_EXPERT_SKILL + MAX_PRO_SKILL + MAX_MASTER_SKILL + MAX_HERO_SKILL )
#else //__LEGEND	//	10차 전승시스템	Neuz, World, Trans
#define MAX_SKILL_JOB	( MAX_JOB_SKILL + MAX_EXPERT_SKILL + MAX_PRO_SKILL )
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans
#endif // __3RD_LEGEND16

#ifdef __INTERNALSERVER
#define	TICK_CHEERPOINT	MIN( 1 )
#else	// __INTERNALSERVER
#define	TICK_CHEERPOINT	MIN( 30 )
#endif	// __INTERNALSERVER

#define	MAX_CHEERPOINT	10
#define	NULL_CHGPARAM	0x7FFFFFFF

const DWORD NEXT_TICK_DUEL          = SEC(1);
const DWORD NEXT_TICK_ENDDUEL       = MIN(3);
const DWORD NEXT_TICK_RECOVERY      = SEC(2);
const DWORD NEXT_TICK_RECOVERYSTAND = SEC(3);


#if __VER >= 10 // __LEGEND	//	9차 전승시스템	Neuz, World, Trans

#define	LEGEND_CLASS_NORMAL			0
#define	LEGEND_CLASS_MASTER			1
#define	LEGEND_CLASS_HERO			2
#ifdef __3RD_LEGEND16
#define LEGEND_CLASS_LEGENDHERO	3
#endif // __3RD_LEGEND16
#endif	//__LEGEND	//	9차 전승시스템	Neuz, World, Trans

#ifdef __BARUNA_PIERCING
#define MAX_BARUNA_DOT 2
#endif // __BARUNA_PIERCING

#define MAX_HUMAN_PARTS 31

/// Replace함수에서 사용하는 타입 	
enum REPLACE_TYPE
{
	REPLACE_NORMAL,				/// 비행체 타고 인벤이 차면 막음 
	REPLACE_FORCE,				/// 무조건 이동 
};

/// 스킬 사용 타입
enum SKILLUSETYPE
{
	SUT_NORMAL,			/// 단축키에 넣고 사용하는 일반적인 방식.
	SUT_QUEUESTART,		/// 스킬큐로 처음 시작하는 스킬이다.(캐스팅을 몰아서함)
	SUT_QUEUEING,		/// 스킬큐의 연속발사중 스킬이다.(캐스팅동작없이 바로 발사)
};

////////////////////////////////////////////////////////////////////////////////
// PVP 관련 
////////////////////////////////////////////////////////////////////////////////

/// Player vs Player의 모드 
enum PVP_MODE
{
	PVP_MODE_NONE,				/// PVP불가 	
	PVP_MODE_GUILDWAR,			/// 길드워 
	PVP_MODE_PK,				/// PK
	PVP_MODE_DUEL				/// 듀얼 
};

#if __VER < 8 // __S8_PK
/// PK 타입 
enum PK_TYPE
{
	NORMAL_AND_NORMAL,			/// 일반 유저 와 일반 유저 
	NORMAL_AND_SEMI,
	NORMAL_AND_CHAO,
	SEMI_AND_NORMAL,			/// 준카오와 일반 유저 
	SEMI_AND_SEMI,
	SEMI_AND_CHAO,
	CHAO_AND_NORMAL,			/// 카오와 일반 유저 
	CHAO_AND_SEMI,
	CHAO_AND_CHAO
};

/// 카르마 변경원인 
enum CHANGE_SLAUGHTER_TYPE
{
	CHANGE_SLAUGHTER_ATTACK,	/// 'PK시도'로 인한 카르마(=슬로터값) 변경 
	CHANGE_SLAUGHTER_KILL,		/// PK로 인한 카르마(=슬로터값) 변경 
	CHANGE_SLAUGHTER_RECOVERY,	/// 자연회복 
	CHANGE_SLAUGHTER_SET,		/// 단순한 대입 
};
#endif // __VER < 8 // __S8_PK

/// 거래 확인 타입 
enum TRADE_CONFIRM_TYPE
{
	TRADE_CONFIRM_ERROR,		/// 인벤에 여유가 없거나, GOLD가 OVERFLOW나는 경우 
	TRADE_CONFIRM_OK,			/// 상대도 ok(최종 거래)
};

/// 거래상태 
enum TRADE_STATE
{
	TRADE_STEP_ITEM,			/// 아이템 단계 - 아이템을 올리고 내리고  
	TRADE_STEP_OK,			    /// OK누른 단계 - ok버튼을 눌렸다.        
	TRADE_STEP_CONFIRM,			/// CONFIRM단계 - 최종확인단계            
};

////////////////////////////////////////////////////////////////////////////////
// struct
////////////////////////////////////////////////////////////////////////////////

/// 장착 정보 구조체 
typedef	struct	_EQUIP_INFO
{
	DWORD	dwId;
	int		nOption;
	BYTE	byFlag;
}	EQUIP_INFO,	*PEQUIP_INFO;

/// 장착 추가 정보 구조체 
typedef	struct	_EQUIP_INFO_ADD
{
#if __VER >= 11 // __SYS_IDENTIFY
	__int64		iRandomOptItemId;
#else	// __SYS_IDENTIFY
	int				nRandomOptItemId;
#endif	// __SYS_IDENTIFY
//	PIERCINGINFO	piercingInfo;
	CPiercing	piercing;

	BYTE			bItemResist;
	int				nResistAbilityOption;
#ifdef __CLIENT
	CTexture*		pTexture;
#endif	// __CLIENT
	_EQUIP_INFO_ADD()
	{
		iRandomOptItemId	= 0;
		piercing.Clear();
		bItemResist	= FALSE;
		nResistAbilityOption	= 0;
#ifdef __CLIENT
		pTexture	= NULL;
#endif	// __CLIENT
	}
	_EQUIP_INFO_ADD& operator=( const _EQUIP_INFO_ADD& e )
	{
		iRandomOptItemId	= e.iRandomOptItemId;
		piercing	= e.piercing;
		bItemResist		= e.bItemResist;
		nResistAbilityOption	= e.nResistAbilityOption;
#ifdef __CLIENT
		pTexture	= e.pTexture;
#endif	// __CLIENT
		return *this;
	}
}	EQUIP_INFO_ADD, *PEQUIP_INFO_ADD;

/// 플레이어 외양 구조체 
typedef struct tagPLAYER
{
	EQUIP_INFO	m_aEquipInfo[MAX_HUMAN_PARTS];
	char		m_byCostume;
	char		m_bySkinSet;
	char		m_byFace;
	char		m_byHairMesh;
	char		m_byHairColor;
	char		m_byHeadMesh;
	char		m_bySex;
	char		m_byJob;
	u_short		m_uSlot;
} PLAYER,* LPPLAYER;

/// 퀘스트 구조체 
typedef struct tagQuest
{
	BYTE		m_nState;
	WORD		m_wTime;
	WORD		m_wId;
//	BYTE		m_nKillNPCNum[ 2 ];	// chipi_091015 - NPC Kill Quest 갯수 확장( BYTE -> WORD )
	WORD		m_nKillNPCNum[ MAX_QUEST_COND_KILL ];	// chipi_091015 - NPC Kill Quest 갯수 확장( BYTE -> WORD )
	BYTE		m_bPatrol   : 1;
	BYTE		m_bDialog   : 1;
	BYTE		m_bReserve3 : 1;
	BYTE		m_bReserve4 : 1;
	BYTE		m_bReserve5 : 1;
	BYTE		m_bReserve6 : 1;
	BYTE		m_bReserve7 : 1;
	BYTE		m_bReserve8 : 1;

	QuestProp* GetProp() { return prj.m_aPropQuest.GetAt( m_wId ); }

} QUEST,* LPQUEST;

/// 동기화에서 사용하는 구조체 
typedef	struct tagCORR_ACTION
{
	float		f;
	float		fSpeed;
	DWORD		dwState;
	DWORD		dwStateFlag;
	DWORD		dwMotion;
	int			nMotionEx;
	int			nLoop;
	DWORD		dwMotionOption;
	D3DXVECTOR3	v;
	D3DXVECTOR3	vd;
	float		fAngleX;
	float		fAccPower;
	float		fTurnAngle;
	BOOL		fValid;
	BOOL		fBehavior;
	BOOL		fHalf;
} CORR_ACTION, *PCORR_ACTION;

/// 피격 정보 구조체 
struct HIT_INFO
{
	int		nHit;		/// 입은 데미지 
	DWORD	dwTick;		/// 공격당한 시각 
};

typedef map<OBJID, HIT_INFO> SET_OBJID;		

/// 공격의 타입 
enum HITTYPE
{
	HITTYPE_FAIL	= 0,
	HITTYPE_GENERIC,			/// Player vs Monster
	HITTYPE_PVP,				/// Player vs Player
	HITTYPE_WAR,				/// 길드전 
	HITTYPE_SCHOOL,				/// 학교 대항 이벤트 타입 
	HITTYPE_PK,					/// Player Killing
	HITTYPE_GUILDCOMBAT,		/// 길드대전 
#ifdef __JEFF_11_4
	HITTYPE_ARENA,		// 아래나
#endif	// __JEFF_11_4
};

/// 리저렉션 데이타 구조체 
typedef struct RESURRECTION_DATA
{
	u_long		  dwPlayerID;
	BOOL		  bUseing;
	ItemProp	 *pSkillProp;
	AddSkillProp *pAddSkillProp;
	int			  nDamage;
} RESURRECTION_DATA;

/// 거래후 결과 구조체 
struct VENDOR_SELL_RESULT
{
	CItemElem	item;			/// 팔린 아이템 
	int			nRemain;		/// 팔리고 남은 갯수 
	int			nErrorCode;		/// 에러코드 
};

/// 개인상점(Vendor)과 거래(Trade) 클래스 
class CVTInfo
{
private:
	DWORD					m_dwTradeGold;					/// 거래중인 돈 
	CItemBase*				m_apItem_VT[MAX_VENDITEM];		/// vendor and trader share pointer array
	OBJID					m_objId;						/// 상대방 id
	CMover*					m_pOwner;						/// 클래스 소유자 
	TRADE_STATE				m_state;						/// 거래상태 
	string					m_strTitle;						/// 개인상점의 타이틀 

public:
	OBJID					GetOtherID() { return m_objId; }
	CMover*					GetOther();
	void					SetOther( CMover* pMover );
	void					Init( CMover* pMover );
	CItemBase*				GetItem( BYTE i );
	void					SetItem( BYTE i, CItemBase* pItemBase );
	LPCTSTR					GetTitle();
	void					SetTitle( LPCTSTR szTitle );
	BOOL					IsVendorOpen();

	void					TradeClear();
	void					TradeSetItem( BYTE nId, BYTE i, short nItemNum );
	BOOL					TradeClearItem( BYTE i );
	void					TradeSetGold( DWORD dwGold );
	int						TradeGetGold();
	BOOL					TradeConsent();
	DWORD					TradeSetItem2( BYTE nId, BYTE i, short & nItemNum );	// Return: dwText 
	TRADE_CONFIRM_TYPE		TradeLastConfirm( CAr & ar );
	TRADE_STATE				TradeGetState();
	void					TradeSetState( TRADE_STATE state );

	void					VendorClose( BOOL bClearTitle = TRUE );	// 개인 상점 닫기
	void					VendorCopyItems( CItemBase** ppItemVd );
	void					VendorItemNum( BYTE i, short nNum );
	void					VendorSetItem( BYTE nId, BYTE i, short nNum, int nCost );
	BOOL					VendorClearItem( BYTE i );
	BOOL					VendorSellItem( CMover* pBuyer, BYTE nItem, DWORD dwItemId, short nNum, VENDOR_SELL_RESULT& result );
	BOOL					VendorIsVendor(); 
	BOOL				IsTrading( CItemElem* pItemElem );
};

/// 서버에서 SFX해킹 때문에 생긴 자료형 
struct SFXHIT_INFO
{
	OBJID	idTarget;
	int		nMagicPower;
	DWORD	dwSkill;
	int		nDmgCnt;
	float	fDmgAngle;
	float	fDmgPower;
	DWORD	dwAtkFlags;
#if __VER >= 8 // __S8_PK
	BOOL	bControl;
#endif // __VER >= 8 // __S8_PK
	DWORD	dwTickCount;
};

struct	SFXHIT_COUNT
{
	DWORD	dwSkill;
	DWORD	dwTickCount;
};

class	CGuild;
class	CGuildWar;
class	CNpcProperty;
struct	ATTACK_INFO;
struct	REGIONELEM;
class	CParty;
class	CActionMover;

#ifdef __LAYER_1015
#define	REPLACE( uMulti, dwWorld, vPos, type, nLayer )	Replace( (uMulti), (dwWorld), (vPos), (type), (nLayer) )
#else	// __LAYER_1015
#define	REPLACE( uMulti, dwWorld, vPos, type, nLayer )	Replace( (uMulti), (dwWorld), (vPos), (type) )
#endif	// __LAYER_1015

#ifdef __CLIENT
class CClientPet
{
public:
	CClientPet();
	virtual	~CClientPet();
public:
	void	SetObj( CMover* pObj )	{	m_pObj	= pObj;	}
	CMover*	GetObj()	{	return m_pObj;		}
	void	SetLevelup( BYTE nLevelup )		{	m_nLevelup	= nLevelup;		}
	BYTE	GetLevelup()	{	return m_nLevelup;	}
#ifdef __PET_1024
	void	SetName( char* szName );
	char*	GetName()	{	return m_szName;	}
	BOOL	HasName()	{	return strlen( m_szName ) > 0;	}
#endif	// __PET_1024
private:
	CMover*	m_pObj;
	BYTE	m_nLevelup;		// PLU_LEVEL_UP 이면, 펫 생성 시 펫 레벨업 효과 생성하고 0으로 초기화
	
#ifdef __PET_1024
	char	m_szName[MAX_PET_NAME];
#endif	// __PET_1024
};
#endif	// __CLIENT


/// 플레이어와 NPC
class CMover : public CCtrl
{	
public:	
	CMover();
	virtual ~CMover();

	friend			CActionMover;
	enum			{ PLAYER, NPC };			/// CMover의 타입 

public:
	BOOL			m_bPlayer;					/// Player인가, 아니라면 NPC
	u_long			m_idPlayer;					/// player의 db번호, NPC는 0xffffffff
	CActionMover*	m_pActMover;				/// 무버를 움직이는 객체. 여기서 무버의 움직임을 컨트롤한다.
	DWORD			m_dwTypeFlag;				/// OBJTYPE_FLYING 만 사용 flag
	DWORD			m_dwMode;					/// 무적, 투명, 초필 등등 flag
	DWORD			m_dwFlag;					/// MVRF_ 관련 flag 시리즈
	DWORD			m_dwStateMode;				/// PK, PVP, 비행 시전 ... flag
	FLOAT			m_fCrrSpd;					/// 이동 속도  
	float			m_fAniSpeed;				/// 애니메이션 속도 배수. 1.0이 기본. 2.0이면 두배빠르기. 2.0이 맥스다.
	float			m_fArrivalRange;			/// 목표에 얼마만큼 근접해야하는가. 미터단위. 디폴트 0
	OBJID			m_idDest;					/// 목표 객체 아이디
	D3DXVECTOR3		m_vDestPos;					/// 목표 지점 
	float			m_fDestAngle;				/// 목표 각도 		
	CShip*          m_pIADestPos;				/// 여기에 포인터가 있다면 m_vDestPos의 좌표는 m_pIADestPos오브젝에 대한 상대좌표가 된다.
	BOOL			m_fWaitQueryGetPos;			/// 동기화 관련 
	int				m_nCorr;					/// 동기화 관련 - 남아있는 동기 프레임 수(실체)
	u_long			m_uRemnantCorrFrm;			/// 동기화 관련 - 남아있는 동기 프레임 수(고스트)
	CORR_ACTION		m_CorrAction;				/// 동기화 정보 구조체 
	bool			m_bForward;					/// 전진중?
	bool			m_bPositiveX;				/// GetPos().x - m_vDestPos.x > 0.0f
	bool			m_bPositiveZ;				/// GetPos().z - m_vDestPos.z > 0.0f
	bool			m_bLeft;					/// left회전중? 클라만??
	DWORD			m_dwRegionAttr;				/// 현재 좌표의 region 속성 
	DWORD			m_dwOldRegionAttr;			/// 이전 좌표의 region 속성 
	DWORD			m_dwMotion;					/// 모션 번호 
	DWORD			m_dwMotionOption;			/// 모션 추가 옵션
	OBJMSG			m_dwMotionArrive;			/// 이동 완료 후 모션

	DWORD			m_dwTickRecovery;			/// 앉은 상태의 hp, mp 복구 timer
	DWORD			m_dwTickRecoveryStand;		/// stand 상태의 hp, mp 복구 timer

#ifdef __VTN_TIMELIMIT
	int				m_nAccountPlayTime;		//ms단위 계정 플레이 시간
#endif // __VTN_TIMELIMIT

	BOOL	HasBuff( WORD wType, WORD wId );
	void	RemoveBuff( WORD wType, WORD wId );
	BOOL	HasBuffByIk3( DWORD dwIk3 );
	BOOL	HasPet();
#ifdef __WORLDSERVER
	void	RemovePet();
#endif	// __WORLDSERVER
	DWORD	GetDisguise();
	BOOL	AddBuff( WORD wType, WORD wId, DWORD dwLevel, DWORD tmTime, OBJID oiAttacker = NULL_ID );
	void	RemoveCommonBuffs();
	void	RemoveChrStateBuffs( DWORD dwChrState );
	void	RemoveIk3Buffs( DWORD dwIk3 );
	void	RemoveAllBuff();
	void	RemoveAttackerBuffs( OBJID oiAttacker );
	void	RemoveDstParamBuffs( DWORD dwDstParam );
	void	RemoveDebufBuffs();
	void	ProcessBuff();
#ifdef __CLIENT
	void	ClearBuffInst();
	int		GetCurrentMaxSkillPoint();
#endif	// __CLIENT

#ifdef __BUFF_1107
	CBuffMgr	m_buffs;
#else	// __BUFF_1107
	CSkillInfluence m_SkillState;				/// 지속성을 갖는 스킬의 상태 처리(예:프로텍션 ??초동안 방어력 얼마 증가)
#endif	// __BUFF_1107
	DWORD			m_dwSMTime[SM_MAX];			/// 유료 아이템 시간 값을 가지고 있음
	SKILL			m_aJobSkill[ MAX_SKILL_JOB ];		/// 스킬 배열 
	DWORD			m_tmReUseDelay[ MAX_SKILL_JOB ];	/// 스킬 재사용시각
	
	LONG			m_nStr, m_nSta, m_nDex, m_nInt;		/// 스텟 
	LONG			m_nLevel;					/// 레벨 
	LONG			m_nDeathLevel;				/// 죽었을 때 레벨
	EXPINTEGER		m_nExp1, m_nDeathExp;		/// m_nExp1 - 경험치, DeathExp - 죽었을 때 경험치

private:
#if __VER >= 12 // __MOD_TUTORIAL
	int		m_nTutorialState;
#else	__MOD_TUTORIAL
	LONG	m_nFlightLv;
#endif	// __MOD_TUTORIAL
public:
#if __VER >= 12 // __MOD_TUTORIAL
	LONG	GetFlightLv( void )	{	return ( GetLevel() >= 20? 1: 0 );		}
	void	SetFlightLv( LONG nFlightLv )	{}
	int		GetTutorialState( void )	{	return m_nTutorialState;	}
	void	SetTutorialState( int nTutorialState )	{	m_nTutorialState	= nTutorialState;	}
#else	// __MOD_TUTORIAL
	LONG	GetFlightLv( void )	{	return m_nFlightLv;		}
	void	SetFlightLv( LONG nFlightLv )	{	m_nFlightLv	= nFlightLv;	}
#endif	// __MOD_TUTORIAL

	LONG			m_nFxp;		/// 비행레벨, 
	LONG			m_nHitPoint;				/// HP
	LONG			m_nManaPoint;				/// MP
	LONG			m_nFatiguePoint;			/// FP( 지구력 )
	LONG			m_nRemainGP;				/// 스탯 포인트 
	LONG			m_nDefenseMin, m_nDefenseMax;	/// 방어력 min, 방어력 max
	int				m_nAdjHitRate, m_nAdjParry;		/// 수정치 
	BYTE			m_bySex;						/// 성별 
	DWORD			m_dwSkinSet, m_dwFace, m_dwHairMesh, m_dwHairColor, m_dwHeadMesh;	/// 외양 
	FLOAT			m_fHairColorR, m_fHairColorG, m_fHairColorB;						/// 머리색 
	LONG			m_nJob;						/// 직업 
	DWORD			m_dwAuthorization;			/// 유저 권한 (일반에서 최고 관리자까지 )

	TCHAR			m_szCharacterKey[32];		/// NPC key문자열 
	DWORD			m_dwBelligerence;			/// 호전성 여부 
	BOOL			m_bActiveAttack;			/// 선공몹인가?
	DWORD			m_dwVirtItem;				/// npc가 사용(공격)할 아이템. 또는 플레이어가 무기 비장착시 사용할 기본 아이템. 
	DWORD			m_dwVirtType;				/// m_dwVirtItem가 아이템인지 스킬인지 결정 VT_ITEM, VT_SKILL
private:
	OBJID	m_oiEatPet;
public:
	int				m_nFuel;					/// 비행체 현재 연료.
	int				m_tmAccFuel;				/// 가속연료(시간단위)
	BOOL			m_bItemFind;				/// 캐스팅 후에 아이템이 있는지 검사할 것인가? 
	int				m_nReadyTime;				/// 비행체 타기 캐스팅 시간
	OBJID			m_dwUseItemId;				/// 비행체 타기 아이템 아이디 

	DWORD			m_dwCtrlReadyTime;			/// box open 캐스팅 타입  
	OBJID			m_dwCtrlReadyId;			/// 캐스팅 대상 box 아이디  

	OBJID			m_idAttacker;				/// this를 실제 공격한넘
	OBJID			m_idTargeter;				/// this를 타겟잡고 점유하고 있는넘.
	OBJID			m_idTracking;				/// 비행중 자동추적하고 있는 타겟.
	OBJID			m_idLastHitMover;			/// this가 마지막으로 쳤던 무버아이디
	float			m_fExpRatio;				/// exp 지급률. this를 죽였을때 this의 exp의 몇퍼센트를 줄것인가. 디폴트는 1.0이다. 이것은 파티때는 적용되지 않는다.
	int				m_nAtkCnt;					/// 공격하면 카운터가 0이되면서 계속 증가한다.
	int				m_nReflexDmg;				/// 리플렉스 데미지 누적 (서버만)
	LONG			m_nPlusMaxHitPoint;			/// 추가 MAX HP
	DWORD			m_tmPoisonUnit;				/// 독 적용 시간
	DWORD			m_tmBleedingUnit;			/// 몇초마다 한번씩인가?
	OBJID			m_idPoisonAttacker;			/// 독 공격자
	OBJID			m_idBleedingAttacker;		/// 출혈 공격자
	short			m_wStunCnt;					/// 스턴상태 시간 
	short			m_wPoisonCnt;				/// 독상태 시간 
	short			m_wPoisonDamage;			/// 독으로 깎일 데미지
	short			m_wDarkCnt;					/// 암흑 지속시간
	short			m_wDarkVal;					/// 암흑동안 깎일 명중률 수정치
	short			m_wBleedingCnt;				/// 출혈 상태 시간 
	short			m_wBleedingDamage;			/// 출혈로 깎일 데미지
	short			m_wDarkCover;				/// 안보이게 숨기
#if __VER >= 9	// __PET_0410
	short			m_nHealCnt;		//	0일 경우만 자동 치유
#endif	// __PET_0410
#ifdef __BARUNA_PIERCING
	short			m_dwForceDamageDamage[4];
	short			m_dwForceDamageCnt[4];
	DWORD			m_dwForceDamageUnit[4];
	OBJID			m_dwForceAttackerID[4];
#endif // __BARUNA_PIERCING
#if __VER >= 9	//__AI_0509
	FLOAT	m_fSpeedFactor;
	void	SetSpeedFactor( FLOAT fSpeedFactor );
	FLOAT	GetSpeedFactor( void )	{	return m_fSpeedFactor;	}
#ifdef __WORLDSERVER
	BOOL	IsReturnToBegin( void );
#endif	// __WORLDSERVER
#endif	// __AI_0509

#if __VER >= 10	// __AI_0711
	BOOL	IsRank( DWORD dwClass )	
		{
			MoverProp* pProp	= GetProp();
			return( pProp && pProp->dwClass == dwClass );
		}
#endif	// __AI_0711

	BYTE			m_nAttackResistLeft;		/// 코드 존재하지만 게임내 사용하지 않음 ( 왼손 속성 공격력 추가 ) 
	BYTE			m_nAttackResistRight;		/// 코드 존재하지만 게임내 사용하지 않음 ( 오른손 속성 공격력 추가 ) 
	BYTE			m_nDefenseResist;			/// 코드 존재하지만 게임내 사용하지 않음 ( 속성 방어력 ) 
	
	u_long			m_idparty;					/// 파티 번호 (클라에선 특별한 경우가 아니면 사용해선 안됨  g_Party.m_uPartyId를 사용 )
	u_long			m_idGuild;					/// 길드 번호 
	BOOL			m_bGuildBank;				/// 길드 뱅크를 이용중인지 확인
	DWORD			m_idGuildCloak;				/// 망토의 길드번호를 가지고 있음
	u_long			m_idWar;					/// 길드전 고유 번호 

	DWORD			m_tmActionPoint;			/// 액션포인트 자동상승 타이머(3초) 
	DWORD			m_dwTickCheer;				/// 응원 타이머 
	int				m_nCheerPoint;				/// 타인에게 응원 받은 값  

	DWORD			m_dwTickDuel;				/// 듀얼 상태 타이머 
	DWORD			m_dwTickEndDuel;			/// 듀얼 종료 타이머 
	int				m_nDuelState;				/// 듀얼상태 
	int				m_nDuel;					/// 듀얼중 상태 ( 0:없음 1:개인듀얼중 2:파티듀얼중 )
	OBJID			m_idDuelOther;				/// 듀얼 상대의 아이디
	u_long			m_idDuelParty;				/// 파티듀얼중이면 상대방 파티의 아이디 
	
	int				m_nFame;					/// 명성치 
	u_long			m_idMurderer;				/// 날 마지막으로 죽였던넘 플레이어 아이디
#if __VER >= 8 // __S8_PK
	DWORD			m_dwPKTime;					/// 핑크 상태 시간
	int				m_nPKValue;					/// PK 수치
	DWORD			m_dwPKPropensity;			/// PK 성향
	DWORD			m_dwPKExp;					/// PK 성향 습득 경험치		
#else // __VER >= 8 // __S8_PK
	int				m_nNumKill;					/// 킬수
	DWORD			m_dwKarmaTick;				/// 카르마 포인트 자연 회복용 타이머 
	int				m_nSlaughter;				/// 카르마포인트(카오 수치)
#endif // __VER >= 8 // __S8_PK
#if __VER >= 8 //__CSC_VER8_5
	EXPINTEGER		m_nAngelExp;				/// 엔젤 경험치
	LONG			m_nAngelLevel;				/// 엔젤 Level
#endif // __CSC_VER8_5

#if __VER >= 10 // __LEGEND	//	9차 전승시스템	Neuz, World, Trans
	BYTE			m_nLegend;
#endif	//__LEGEND	//	9차 전승시스템	Neuz, World, Trans
#if __VER >= 11 // __MA_VER11_06				// 확율스킬 효과수정 world,neuz
	DWORD			dwRemoveSfxObj[MAX_SKILLBUFF_COUNT];			// 삭제할 효과 오브젝트 
	DWORD			GetRemoveSfxObj(DWORD	dwRemoveSfxObjID);
	void			SetRemoveSfxObj(DWORD	dwRemoveSfxObjID);
#endif // __MA_VER11_06				// 확율스킬 효과수정 world,neuz

#if __VER >= 13 // __HONORABLE_TITLE			// 달인
	int				m_nHonor;					// 달인선택 
	DWORD			m_dwHonorCheckTime;			//달인 시간체크
	int				m_aHonorTitle[MAX_HONOR_TITLE];			// 달인수치
	int				GetHonorTitle(int nIdx)	{	return m_aHonorTitle[nIdx];	}
	void			SetHonorCount(int nIdx , int nCount );
	void			CheckHonorStat();
	void			CheckHonorTime();
	void			SetHonorAdd(int nSmallIdx,int nLargeGroup,int nDefault = 0);
	CString			m_strTitle;
	LPCTSTR			GetTitle();
	void			SetTitle(LPCTSTR	pTitle);

#endif	// __HONORABLE_TITLE			// 달인

#ifdef __CLIENT
	CSfx*			m_pSfxWing;
	DWORD			m_dwWingTime;

#if __VER >= 15 // __PETVIS
	CSfx*			m_pSfxBuffPet;
	void			SetSfxBuffPet( const DWORD idEffect );
#endif	//__PETVIS
#endif //__CLIENT

#ifdef __JEFF_9_20
	DWORD		m_dwMute;
#if __VER < 12 // __LORD
	BOOL	IsMute( void )	{	return ( m_dwMute > 0 );	}
#endif	// __LORD
#endif	// __JEFF_9_20

	RESURRECTION_DATA				m_Resurrection_Data;		/// 리저렉션 스킬 정보 
	CItemContainer< CItemElem  >*	m_ShopInventory[ MAX_VENDOR_INVENTORY_TAB ];	/// 상인NPC의 물품 정보 
	CItemContainer< CItemElem  >	m_Inventory;				/// 인벤토리

	CItemContainer< CItemElem  >	m_InventoryTAB[4];

#if __VER >= 11 // __SYS_POCKET
	CPocketController	m_Pocket;
#endif	// __SYS_POCKET
	EQUIP_INFO		m_aEquipInfo[MAX_HUMAN_PARTS];				/// 장착정보 	
	CVTInfo			m_vtInfo;					/// 거래와 개인상점 번호 

	BYTE			m_nSlot;					/// 로그인 화면의 3개 캐릭터 순서 번호 
	BOOL			m_bBank;					/// 은행 이용 중?
	u_long			m_idPlayerBank[3];			/// 3개 캐릭터 idPlayer
	DWORD			m_dwGoldBank[3];			/// 3개 캐릭터 길드돈 
	CItemContainer< CItemElem >	m_Bank[3];		/// 3개 캐릭터 은행 아이템 

	BYTE			m_nQuestKeeping;			/// 클락워크 길드 퀘스트 번호  
	BYTE			m_nPartyQuestKeeping;		/// 거대 몬스터 퀘스트 번호 
	BYTE			m_nQuestSize;				/// 퀘스트 갯수 
	LPQUEST			m_aQuest;					/// 현재 진행중인 퀘스트 배열 
	BYTE			m_nCompleteQuestSize;		/// 완료된 퀘스트 갯수 
	LPWORD   		m_aCompleteQuest;			/// 완료된 퀘스트 배열 	
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	BYTE			m_nCheckedQuestSize;
	LPWORD			m_aCheckedQuest;
#endif // __IMPROVE_QUEST_INTERFACE

	int				m_nDead;					/// 죽은 후 5초간은 무적
	int				m_nGuildCombatState;		/// 길드 대전 상태 
	D3DXVECTOR3		m_vMarkingPos;				/// 로드라이트에서 마킹한 좌표 
	OBJID			m_idMarkingWorld;			/// 로드라이트에서 마킹한 월드 아이디 

#ifdef __CLIENT
	static BOOL		m_bQuestEmoticonAdd;		/// 퀘스트 아이콘 확축 프로세스 
	static FLOAT	m_fQuestEmoticonScale;
	int				m_nQuestEmoticonIndex;
	bool			m_bShowQuestEmoticon;

	NaviPoint		m_nvPoint;
	V_NaviPoint		m_vOtherPoint;
	int				m_idSfxHit;
	BYTE			m_nWaterCircleCount;
	DWORD			m_dwReqFlag;				/// 응답요청 플래그.
	char			m_szPartyName[32];
	float			m_fDestScale;				/// 쪼그라들때 부드럽게 하기위한..
	float			m_fDestScaleSlerp;			/// 1.0f ~ 0.0으로 변화함.
	CCooltimeMgr	m_cooltimeMgr;	
	LPDIRECT3DTEXTURE9		m_pCloakTexture;	/// 망토텍스쳐.

#if __VER >= 8 //__Y_EYE_FLASH_8
	static LPDIRECT3DTEXTURE9		m_pTextureEye[2][MAX_HEAD];
	static LPDIRECT3DTEXTURE9		m_pTextureEyeFlash[2][MAX_HEAD];
#endif //__Y_EYE_FLASH_8

#endif // __CLIENT

#ifdef __WORLDSERVER
	CNpcProperty*	m_pNpcProperty;				/// NPC 고유 정보( 대사, 퀘스트, 파일이름 ) 현재는 마을 NPC 대화처리자
	CTimer          m_timerQuestLimitTime;		/// 퀘스트 제한 시간 타이머 
	DWORD	        m_dwPKTargetLimit;			/// PK선공제한시각
	DWORD			m_dwTickCreated;			/// 생성 시각 
	BOOL			m_bLastPK;					/// 마지막으로 죽었을때 플레이어에게(듀얼포함) 죽은건가 그외의 경우로 죽은건가.
	BOOL			m_bLastDuelParty;			/// 파티듀얼로 사망?
	CTime			m_tGuildMember;				/// 길드 탈퇴 시각 
	BOOL			m_bGuildCombat;				/// 길드 대전 중?
//	OBJID			m_idCollecter;				/// this의 채집소유자.
//	int				m_nResource;				/// 자원 보유량. - 몬스터측에서 사용.
//	int				m_nCollectOwnCnt;			/// > 0 인상황은 this는 m_idCollecter것이다.
	CSfxHitArray	m_sfxHitArray;				/// sfx hit 정보 배열 
	map< OBJID, queue< SFXHIT_INFO> >	m_mapSFXInfo;		/// sfx 해킹 때문에 ....	
//	map< OBJID, int>					m_mapSFXCount;		/// sfx가 발사되면 ++ 
	map< OBJID, queue< SFXHIT_COUNT > >	m_mapSFXCount;
#endif	// __WORLDSERVER

#ifdef __AI_AGGRO
#ifndef __CLIENT
private:
	DWORD	m_dwAggro;
public:
	DWORD	GetAggro(){ return m_dwAggro; };
	void	SetAggro( long dwAggro ){ if( dwAggro >= 0 ) m_dwAggro = (DWORD)dwAggro; else m_dwAggro = 0; };
#endif
#endif

#ifdef __WORLDSERVER
private:
	void	DoUseEatPet( CItemElem* PitemElem );
	void	DoUseSystemPet( CItemElem* pItemElem );
public:
	void	ActivateEatPet( CItemElem* pItemElem );
	void	ActivateSystemPet( CItemElem* pItemElem );
	void	InactivateEatPet( void );
	void	InactivateSystemPet( CItemElem* pItemElem );

#if __VER >= 15 // __PETVIS
	void	SetPetVisDST( CItemElem* pItemElem );
	void	ResetPetVisDST( CItemElem* pItemElem );
	void	ProcessVisPet();
#endif // __PETVIS
#endif	// __WORLDSERVER
#if __VER >= 12 // __PET_0519
	BOOL	IsUsingEatPet( CItemElem* pItemElem );	// 사용중인 먹펫인가?
#endif	// __PET_0519

	BOOL	HasActivatedEatPet( void )		{	return m_oiEatPet != NULL_ID;	}
	BOOL	HasActivatedSystemPet( void )	{	return m_dwPetId != NULL_ID;	}
	OBJID	GetEatPetId( void )		{	return m_oiEatPet;		}
	void	SetEatPetId( OBJID oiEatPet )		{	m_oiEatPet	= oiEatPet;	}
	DWORD	GetPetId( void )	{	return m_dwPetId;	}
	void	SetPetId( DWORD dwPetId )		{	m_dwPetId	= dwPetId;	}
private:
	DWORD	m_dwPetId;	// 소환중이 펫의 인벤토리 위치(自), 소환중인 펫 인덱스(他)
#if __VER >= 12 // __PET_0519
	int		DoUseItemPetTonic( CItemElem* pItemElem );	// 펫 영양제 사용
#endif	// __PET_0519
#if __VER >= 15 // __PETVIS
private:
	BOOL	SetValidNeedVis( CItemElem* pItemElem, int nPos, vector<BYTE> & vecValid );
public:
	vector<BYTE>	GetValidVisTable( CItemElem* pItemElem );
	BYTE		IsSatisfyNeedVis( CItemElem* pItemElemVisPet, ItemProp* pItemPropVis );			
	CItemElem*	GetVisPetItem( void )	{ return m_Inventory.GetAtId( m_objIdVisPet ); }
	void		SetVisPetItem( OBJID objId )	{ m_objIdVisPet = objId; }
	BOOL		HasActivatedVisPet()	{ return m_objIdVisPet != NULL_ID; }
	DWORD		m_dwMoverSfxId;	// 무버에 붙는 이펙트
private:
	OBJID		m_objIdVisPet;	// 소환중인 비스펫의 인벤토리 위치
#endif // __PETVIS

public:
#if __VER >= 9	// __PET_0410
	CPet*	GetPet( void );
	CItemElem*	GetPetItem( void );
	void	PetLevelup( void );
	void	PetRelease( void );

#ifdef __CLIENT
	CClientPet	m_pet;
	CModelObject*	GetPetModel( void )		{	return (CModelObject*)( m_pet.GetObj()? m_pet.GetObj()->m_pModel: NULL );	}
	void	CreatePetSfx( void );
#if __VER >= 12 // __UPDATE_OPT
	BOOL	IsOptionRenderMask()	{	return !IsMode( MODE_OPTION_DONT_RENDER_MASK );	}
#endif	// __UPDATE_OPT
#else	// __CLIENT	// __WORLDSERVER
	void	ProcessPetAvail( void );
	void	ProcessPetEnergy( void );
	void	ProcessPetExp( void );
	int		DoUseItemFeedPocket( CItemElem* pItemElem );
#endif	// __CLIENT
#endif	// __PET_0410

#ifdef __SYS_TICKET
	CItemElem*	GetTicket( void );
#endif	// __SYS_TICKET

#if __VER >= 11 // __SYS_POCKET
	CItemElem*	GetItemId2( int nPocket, int nItem, BOOL bExpiration = TRUE );
	BOOL	CreateItem2( CItemElem* pItem, int nPocket );
	void	RemoveItem2( int nItem, short nNum, int nPocket );
#endif	// __SYS_POCKET

#if __VER >= 11 // __SYS_COLLECTING
	int						m_nMaxCltTime;
	int						m_nCltTime;
	BOOL	IsCollecting( void )	{	return	m_pActMover->GetActionState() == OBJSTA_COLLECT;	}
	virtual	void	ProcessCollecting( void );
	virtual	void	StartCollecting( void );
	virtual	void	StopCollecting( void );
	CItemElem*	GetCollector( void );
#endif	//	__SYS_COLLECTING

	int				m_nSkillLevel;				/// 지금까지 올린 스킬레벨
	int				m_nSkillPoint;				/// 스킬 찍을 포인트 수치 

	CTimer			m_SkillTimer;
	BOOL			m_SkillTimerStop;
	
#if __VER >= 8 //__CSC_VER8_5
	BOOL			m_pAngelFlag;				/// Angel Buff Exist?
	CModelObject*	m_pAngel;					/// Angel Model Object.
	D3DXVECTOR3		m_AngelPos;					/// Angel Position.
	D3DXMATRIX		m_AngelWorldM;				/// Angel Matrix.
	DWORD			m_dwAngelKind;				/// Angel Kind.
#endif //__CSC_VER8_5
	
#ifdef __EVE_BALLOON
	BOOL			m_pBalloonFlag;				/// Balloon Buff Exist?
	CModelObject*	m_pBalloon;					/// Balloon Model Object.
	D3DXVECTOR3		m_BalloonPos;				/// Balloon Position.
	D3DXMATRIX		m_BalloonWorldM;			/// Balloon Matrix.
	DWORD			m_dwBalloonKind;			/// Balloon Kind.
	float			m_fBalloonYPos;				/// Balloon Y Position
	BOOL			m_bBalloon;
#endif //__EVE_BALLOON

private:
	SET_OBJID		m_idEnemies;				/// 적에게 공격받은 정보 

	OBJACT			m_oaCmd;					/// 오브젝트 명령  
	int				m_nCParam[3];				/// 범용 명령 파라메터
	OBJACT			m_oaAct;					/// 오브젝트 행동상태
#if __VER >= 8 // __S8_PK
	int				m_nAParam[5];				/// 범용 행동 파라메터
#else // __VER >= 8 // __S8_PK
	int				m_nAParam[4];				/// 범용 행동 파라메터
#endif // __VER >= 8 // __S8_PK

	LONG			m_adjParamAry[ MAX_ADJPARAMARY ];		/// 수정 파라미터( 예: hp + 수정hp )
	LONG			m_chgParamAry[ MAX_ADJPARAMARY ];		/// 교체 파라미터( 예: 교체 hp )

	BOOL			m_bRegenItem;				/// NPC 상인이 팔 아이템 생성해 두었는가 flag  
	int				m_nMovePattern;				/// NPC 패턴 이동 타입
	int				m_nMoveEvent;				/// NPC 패턴 이동때 쓰이는 상태변화 99면 상태끝
	int				m_nMoveEventCnt;			/// NPC 패턴이동때 쓰이는 상태변화카운터

#ifdef	__CLIENT
	DWORD			m_dwLadolfFlag;
	CModelObject*   m_pLadolf;					/// 라돌프모델
	int				m_nDmgCnt;					/// 데미지 카운트
#if __VER >= 14 // __WING_ITEM
	enum WingStateFlag
	{
		FLOATING, FLYING, TURNING
	};

	WingStateFlag	m_eWingStateFlag;		/// 애니메이션 플래그 스위치
	float			m_fOldLengthSq;			/// 날개 애니메이션 속도 변화량의 시작점을 저장하기 위한 변수
#endif // __WING_ITEM
#if __VER >= 8 //__Y_EYE_FLASH_8
	CTimer			m_tmEye[2];
#endif //__Y_EYE_FLASH_8
#endif // __CLIENT

#if __VER >= 15 // __CAMPUS
private:
	u_long	m_idCampus;			// 사제 아이디
	int		m_nCampusPoint;		// 사제 포인트
public:
	u_long	GetCampusId()		{	return m_idCampus;	}
	void	SetCampusId( u_long idCampus )		{	m_idCampus = idCampus;	}
	int		GetCampusPoint()	{	return m_nCampusPoint;	}
	void	SetCampusPoint( int nMPPoint )		{	m_nCampusPoint = nMPPoint;	}
#endif // __CAMPUS

#ifdef __NEW_ITEM_VARUNA
	DWORD	dwBarunaPetItemId;
	u_long	m_idBarunaPetOwner;
	u_long	GetBarunaPetOwner() { return m_idBarunaPetOwner; }
	void	DoUseBarunaPet(CItemElem *pItemElem);
	void	InactivateBarunaPet();
	void	ProcBarunaPet();
#endif // __NEW_ITEM_VARUNA

protected:
	int				m_nCount;					/// 무버가 범용으로 쓰는 순차적 카운터. 생성자 외엔 0으로 초기화 하지 말것.
	DWORD			m_dwGold;					/// 페냐 
	DWORD			m_dwRideItemIdx;			/// 비행체의 아이템 인덱스
	CModelObject*	m_pRide;					/// 비행체 객체 포인터 
	TCHAR			m_szName[MAX_NAME];			/// 이름 
	
public:
	static CMover*  GetActiveMover() { return (CMover*)m_pObjActive; }	// 주인공 객체 얻기 	
	static	int		GetHairCost( CMover* pMover, BYTE nR, BYTE nG, BYTE nB, BYTE nHair );
	static void		UpdateParts( int nSex, int nSkinSet, int nFace, int nHairMesh, int nHeadMesh, PEQUIP_INFO pEquipInfo, CModelObject* pModel, CItemContainer< CItemElem  >* pInventory, BOOL bIfParts = TRUE, CMover* pMover = NULL );
	static BOOL		DoEquip( int nSex, int nSkinSet, CItemElem* pItemElem, int nPart, const EQUIP_INFO & rEquipInfo, CItemContainer< CItemElem  >* pInventory, PEQUIP_INFO pEquipeInfo, CModelObject* pModel, BOOL bEquip, CMover *pMover );
	static	float	GetItemEnduranceInfluence( int nEndurance );	
	static	int		GetItemEnduranceWeight( int nEndurance );	

	virtual	BOOL	SetIndex( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwIndex, BOOL bInitProp = FALSE, BOOL bDestParam = TRUE );
	virtual BOOL	Read( CFileIO* pFile );
	virtual void	Process();
	virtual	void	Serialize( CAr & ar ); // 시리얼라이즈 ; 네트웍 상태에서 서버와 클라이언트, 클라이언트 서버가 주고받을 패킷 내용 
	virtual	CModel* LoadModel( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwType, DWORD dwIndex );
	virtual void	InitProp( BOOL bInitAI = TRUE );		// 객체를 프로퍼티 내용으로 초기화 	
//	virtual int		OnActCollecting();				// User만 사용되는 것이므로 CUser가서 찾을것.
	virtual int		SendDamage( DWORD dwAtkFlag, OBJID idAttacker, int nParam = 0, BOOL bTarget = TRUE ) { return m_pActMover->SendDamage( dwAtkFlag, idAttacker, nParam, bTarget );  }
	virtual int		SendDamageForce( DWORD dwAtkFlag, OBJID idAttacker, int nParam = 0, BOOL bTarget = TRUE ) { return m_pActMover->SendDamageForce( dwAtkFlag, idAttacker, nParam, bTarget ); }	// 강공격

	void			Init();										// 객체 초기화 
	void			InitLevel( int nJob, LONG nLevel, BOOL bGamma = TRUE );			// 객체를 nLevel로 바꿔줌

#ifdef __FASTJOBCHANGE
	void			InitFastJobChange( int nJob, LONG nLevel, BOOL bGamma = TRUE ) ;
#endif//__FASTJOBCHANGE

	void			ProcessAniSpeed();
	void			AllocShopInventory( LPCHARACTER pCharacter );
	BOOL			IsVendorNPC();
	void			CheckTickCheer();
	void			SetCheerParam( int nCheerPoint, DWORD dwTickCount, DWORD dwRest );
	void			ClearEquipInfo();
	int				GetMaxPoint(int nDest);
	DWORD			GetReuseDelay( int nIndex );
	BOOL			InitSkillExp();
	void			InitCharacter( LPCHARACTER lpCharacter );	// 이름 초기화 LoadDialog호출 
	LPCHARACTER		GetCharacter();								// 
	void			InitNPCProperty();							// NPC관련 스크립트 초기화 	
	BOOL			LoadDialog();								// dialog 스크립트 로드 
	void			ProcessRecovery();							// HP, MP, FP회복을 처리한다.
	BOOL			IsActiveMover() { return m_pObjActive == this; }	// 내가 주인공 객체인가?
	int				IsSteal( CMover *pTarget );		// pTarget을 스틸하려 하는가.
	int				IsSteal( OBJID idTaget );		// id로 검사하는 버전.
	u_long			GetPartyId() { return m_idparty; }
	BOOL			IsMode( DWORD dwMode ); 
	void			SetMode( DWORD dwMode )		{ m_dwMode |= dwMode; }	// 유저상태 셑팅
	void			SetNotMode( DWORD dwMode )	{ m_dwMode &= (~dwMode); } // 유저상태 리셑
	BOOL			SetDarkCover( BOOL bApply, DWORD tmMaxTime = 0 );
	BOOL			SetStun( BOOL bApply, DWORD tmMaxTime = 0 );
	BOOL			SetPoison( BOOL bApply, OBJID idAttacker = NULL_ID, DWORD tmMaxTime = 0, DWORD tmUnit = 0, short wDamage = 0 );	// this를 독에 걸리게 한다.
	BOOL			SetDark( BOOL bApply, DWORD tmMaxTime = 0, int nAdjHitRate = 0 );	// this를 암흑상태에 빠지게 한다 .
	BOOL			SetBleeding( BOOL bApply, OBJID idAttacker = NULL_ID, DWORD tmMaxTime = 0, DWORD tmUnit = 0, short wDamage = 0 );	// this를 출혈시킨다.
	void			RemoveDebuff( DWORD dwState );		// 디버프 해제
	void			RemoveBuffOne( DWORD dwSkill = 0 );		// 버프 하나 해제
	void			RemoveBuffAll();				// 버프 모두 해제.
	BOOL			IsStateMode( DWORD dwMode ); 
	void			SetStateMode( DWORD dwMode, BYTE nFlag );		// 유저상태 셑팅
	void			SetStateNotMode( DWORD dwMode, BYTE nFlag );	// 유저상태 리셑
	BOOL			IsUseItemReadyTime( ItemProp* pItemProp, OBJID dwObjItemId );
	BOOL			IsNPC()				{ return !m_bPlayer; }
	BOOL			IsPlayer()			{ return m_bPlayer; }
	BOOL			IsEquipableNPC()	{	return( GetCharacter() && GetCharacter()->m_nEquipNum > 0 );	}
	BOOL			IsFlyingNPC()		{ return (m_dwTypeFlag & OBJTYPE_FLYING) ? TRUE : FALSE; }		// 비행형 몹인가.
	BOOL			IsFly();
	MoverProp*		GetProp() { return prj.GetMoverProp( m_dwIndex ); }	// 객체의 프로퍼티 얻기 	
	ItemProp*		GetActiveHandItemProp( int nParts = PARTS_RWEAPON );							// 손에 들려진 아이템의 프로퍼티를 얻기 
	ItemProp*		GetTransyItem( ItemProp* pItemProp, BOOL bCheck = FALSE, LPCTSTR lpszFileName = NULL );
	OBJID			GetDestId()			{ return m_idDest; }
	CCtrl*			GetDestObj()		{ return prj.GetCtrl( m_idDest ); }
	void			SetDestObj( CCtrl* pObj, float fRange = 0.0f )	{	SetDestObj( pObj->GetId(), fRange );	}
	D3DXVECTOR3		GetDestPos()		{ return m_vDestPos; }
	void			SetDestPos( CONST D3DXVECTOR3& vDestPos, bool bForward = true, BOOL fTransfer = TRUE );
	void			SetDestPos( CShip *pIAObj, const D3DXVECTOR3 &vDestPos );
	
	void			ClearDestObj()     	{ m_idDest = NULL_ID; m_fArrivalRange = 0.0f; }
	void			ClearDestPos()		{ m_vDestPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ); }		
	void			ClearDest()			{ ClearDestObj(); ClearDestPos(); }
	void			SetStop()			{ SendActMsg( OBJMSG_STAND ); ClearDestObj(); } // 멈춤 

	BOOL			IsEmptyDestObj()	{ return m_idDest == NULL_ID; }
	BOOL			IsEmptyDestPos()	{ return m_vDestPos.x == 0.0f && m_vDestPos.z == 0.0f; }
	BOOL			IsEmptyDest()		{ return IsEmptyDestPos() && IsEmptyDestObj(); }
	void			SetDestAngle( float fDestAngle, bool bLeft )	{	m_fDestAngle	= fDestAngle;	m_bLeft	= bLeft;	}
	void			ClearDestAngle()	{ m_fDestAngle	= -1.0f; }
	BOOL			IsRegionAttr( DWORD dwAttribite ) { return ( m_dwRegionAttr & dwAttribite ) == dwAttribite ? TRUE : FALSE; }
	REGIONELEM*		UpdateRegionAttr();
	DWORD			GetRideItemIdx()	{ return m_dwRideItemIdx; }
	void			SetRide( CModel *pModel, int nRideItemIdx = 0 ) { m_dwRideItemIdx = nRideItemIdx; m_pRide = (CModelObject*)pModel; 	};
	void			ClearDuel();
	void			ClearDuelParty();		
	int				SendActMsg( OBJMSG dwMsg, int nParam1 = 0, int nParam2 = 0, int nParam3 = 0, int nParam4 = 0 ); 
#ifdef __INSTANCE_AGGRO_SYSTEM
	void			SendAIMsg( DWORD dwMsg, DWORD dwParam1 = 0, DWORD dwParam2 = 0, DWORD dwParam3 = 0 );
	void			PostAIMsg( DWORD dwMsg, DWORD dwParam1 = 0, DWORD dwParam2 = 0, DWORD dwParam3 = 0 );
#else // __INSTANCE_AGGRO_SYSTEM*/
	void			SendAIMsg( DWORD dwMsg, DWORD dwParam1 = 0, DWORD dwParam2 = 0 );
	void			PostAIMsg( DWORD dwMsg, DWORD dwParam1 = 0, DWORD dwParam2 = 0 );
#endif // __INSTANCE_AGGRO_SYSTEM
	BOOL			SetMotion(DWORD dwMotion, int nLoop = ANILOOP_LOOP, DWORD dwOption = 0 );	// MOP_SWDFORCE, MOP_FIXED, MOP_NO_TRANS, MOP_HITSLOW
	BOOL			InitMotion(DWORD dwMotion);
	void			SetDamagedMotion( CMover* pAttacker, DWORD dwAtkFlags );
	CItemElem*		GetWeaponItem( int nParts = PARTS_RWEAPON );					// 장착한 무기 얻기 
	CItemElem*		GetLWeaponItem();					// 왼손에 장착한 무기 얻기.
	CItemElem*		GetEquipItem( int nParts );			// 장착한 아이템 얻기 
	ItemProp*		GetEquipItemProp( CItemContainer<CItemElem>* pInventory, PEQUIP_INFO pEquipInfo, int nParts );
	BOOL			IsDualWeapon();
	void			RedoEquip( BOOL fFakeParts, BOOL bDestParam = TRUE );	
	void			UpdateParts( BOOL bFakeParts  = FALSE ); // normal or fake
	int				InvalidEquipOff( BOOL fFakeparts );		// 거시기한 장비를 벗김.
	BOOL			DoEquip( CItemElem* pItemElem, BOOL bEquip, int nPart = -1 ); // for normal
#if __VER >= 11 // __SYS_IDENTIFY
	BOOL			IsEquipAble( CItemElem* pItem,BOOL bIgnoreLevel = FALSE );		// 장착가능한가?
#else	// __SYS_IDENTIFY
	BOOL			IsEquipAble( ItemProp *pItemProp, OBJID dwObjid ,BOOL bIgnoreLevel = FALSE);		// 장착가능한가?
#endif	// __SYS_IDENTIFY
	BOOL			IsUnEquipAble( ItemProp *pItemProp );					// 벗는게 가능한가?
	void			SetEquipDstParam();
	void			SumEquipAdjValue( int* pnAdjHitRate, int* pnAdjParray );
	int				SumEquipDefenseAbility( LONG* pnMin, LONG* pnMax );
	void			SetDestParamEquip( ItemProp* pItemProp, CItemElem* pItemElem, BOOL bIgnoreSetItem = FALSE );
	void			ResetDestParamEquip( ItemProp* pItemProp, CItemElem* pItemElem );
	LPQUEST			FindQuest( int nQuestId );
	LPQUEST			GetQuest( int nQuestId );
	BOOL			RemoveQuest( int nQuestId );
	BOOL			IsCompleteQuest( int nQuestId );
	BOOL			MakeCompleteQuest( int nQuestId, LPQUEST lpQuest );
	BOOL			SetQuest( int nQuestId, int nState, LPQUEST lpReturnQuest );
	BOOL			SetQuest( LPQUEST lpQuest );
	void			RemoveAllQuest();
	void			RemoveCompleteQuest();
	BOOL            IsDisguise();
	BOOL			NoDisguise( LPDIRECT3DDEVICE9 pd3dDevice = NULL );
	BOOL			Disguise( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwMoverIndex );
	DWORD			IsAuthorization( DWORD dwAuthorization ) { return dwAuthorization == m_dwAuthorization; }
	DWORD			IsAuthHigher( DWORD dwAuthorization ) { return dwAuthorization <= m_dwAuthorization; }
	void			UpdateParam();		
	int				GetParam( int nDestParameter, int nParam );
	void			SetDestParam( int nDstParameter, int nAdjParameterValue, int nChgParameterValue, BOOL fSend = TRUE );
	void			SetDestParam( int nItemIdx2, BOOL fSend = TRUE );
	void			SetDestParam( int nIdx, ItemProp *pProp, BOOL bSend = TRUE );
	void			ResetDestParam( int nDstParameter, int nAdjParameterValue, BOOL fSend = TRUE );
#ifdef __SPEED_SYNC_0108		// ResetDestParam speed 수정
	void			ResetDestParamSync( int nDstParameter,int nAdjParameterValue, int nParameterValue, BOOL fSend = TRUE );
#endif // __SPEED_SYNC_0108		// ResetDestParam speed 수정
	void			ResetDestParam( int nIdx, ItemProp *pProp, BOOL bSend = TRUE );
	int				GetAdjParam( int nDestParameter );
	int				GetChgParam( int nDestParameter );
	int				GetPointParam( int nDstParameter );
	void			SetPointParam( int nDstParameter, int nValue, BOOL bTrans = FALSE ); // bTrans가 TRUE이면 강제전송
	void			OnApplySM();
	LPSKILL			GetSkill( int nType, int nIdx );
	LPSKILL			GetSkill( DWORD dwSkill );
	int				GetSkillIdx( DWORD dwSkill );
	void			OnEndSkillState( DWORD dwSkill, DWORD dwLevel );
	BOOL			CheckSkill( DWORD dwSkill );
	void			SetHair( int nHair );
	void			SetHairColor( DWORD dwHairColor );
	void			SetHairColor( FLOAT r, FLOAT g, FLOAT b );
	void			SetHead( int nHead );
	void			SetSkinSet( int nSkinSet );
	void			SetName( const char* lpszName )		{	lstrcpy( m_szName, lpszName );	}
	LPCTSTR			GetName( BOOL bNickname = FALSE );	// 객체의 이름 얻기
	BYTE			GetSex() { return m_bySex; }
	void			SetSex( BYTE bySex ) { m_bySex = bySex; }
	BOOL			IsPeaceful() { return m_dwBelligerence == BELLI_PEACEFUL; }
	BOOL			IsBaseJob();
	BOOL			IsExpert();
	BOOL			IsPro();
	BOOL			IsInteriorityJob( int nJob );
	BOOL			SetExpert( int nExpert );
	BOOL			AddChangeJob( int nJob );
	int				GetJob();
	int				GetExpPercent();
	int				SetLevel( int nSetLevel );
	int				AddGPPoint( int nAddGPPoint );
	void			SetJobLevel( int nLevel, int nJob );
	BOOL			IsJobType( DWORD dwJobType ); 
	int				GetLevel() { return m_nLevel; }
	int				GetFxp() { return m_nFxp; }
	int				GetTxp() { return m_nFxp; }
	EXPINTEGER		GetExp1()	{	return m_nExp1;	}
	EXPINTEGER		GetMaxExp1()	{	return prj.m_aExpCharacter[m_nLevel+1].nExp1;	}
	EXPINTEGER		GetMaxExp2()	{	return prj.m_aExpCharacter[m_nLevel].nExp2;	}
	int				GetRemainGP(); // growth
	void			IncHitPoint( int nVal) ;
	void			IncManaPoint(int nVal) ;
	void			IncFatiguePoint(int nVal) ;
	void			SetHitPoint( int nVal) ;
	void			SetManaPoint( int nVal ) ;
	void			SetFatiguePoint( int nVal ) ;
	float			GetExpFactor();
	float			GetItemDropRateFactor( CMover* pAttacker );
	float			GetPieceItemDropRateFactor( CMover* pAttacker );
	BOOL			AddExperience( EXPINTEGER nExp, BOOL bFirstCall = TRUE, BOOL bMultiply = TRUE, BOOL bMonster = FALSE );	// bMultiply : 상용화 아이템 적용? // bMonster : 몬스터를 죽여서 온 경험치 인가?
	BOOL			DecExperience( EXPINTEGER nExp, BOOL bExp2Clear, BOOL bLvDown );	// 경험치를 깎는다.
	BOOL			DecExperiencePercent( float fPercent, BOOL bExp2Clear, BOOL bLvDown  );	// 경험치를 퍼센트로 깎는다.
	BOOL			AddFxp( int nFxp );
	BOOL			SetFxp( int nFxp, int nFlightLv );
#if __VER < 8 // __S8_PK
	int				GetKarma() { return m_nSlaughter; }
#endif // __VER < 8 // __S8_PK
	BOOL			IsPVPTarget( CMover* pMover );
	BOOL			IsWarTarget( CMover* pMover );
	BOOL			IsSchoolTarget( CMover* pMover );
	HITTYPE			GetHitType( CMover* pMover, BOOL bTarget, int nReflect );
#ifdef __JEFF_11_4
	BOOL	IsArenaTarget( CMover* pMover );
#endif	// __JEFF_11_4
#if __VER >= 8 // __S8_PK
	HITTYPE			GetHitType2( CMover* pMover, BOOL bTarget, BOOL bGood );
#else // __VER >= 8 // __S8_PK
	HITTYPE			GetHitType2( CMover* pMover, BOOL bTarget );
#endif // __VER >= 8 // __S8_PK
	void			ReSetDuelTime( CMover* pAttacker , CMover* pDefender);
	BOOL			IncStrLevel();
	BOOL			IncStaLevel();
	BOOL			IncDexLevel();
	BOOL			IncIntLevel();
	int				GetHR();
	void			SetStr( int nStr ) { m_nStr = nStr; }
	void			SetSta( int nSta ) { m_nSta = nSta; }
	void			SetDex( int nDex ) { m_nDex = nDex; }
	void			SetInt( int nInt ) { m_nInt = nInt; }	
	int				GetStr();
	int				GetDex();
	int				GetInt();
	int				GetSta();
	FLOAT			GetSpeed(FLOAT fSrcSpeed);    
	int				GetGold();
#if __VER >= 12 // __LORD
	int		GetPerinNum( void );
	__int64		GetTotalGold( void );
#ifdef __WORLDSERVER
	int		RemoveTotalGold( __int64 iGold );	// 제거된 페린 개수
	int		RemovePerin( int nPerin );
#endif	// __WORLDSERVER
#endif	// __LORD
	void			SetGold( int nGold );
	BOOL			AddGold( int nGold, BOOL bSend = TRUE );
	int				GetHitPointPercent( int nPercent = 100 );
	int				GetManaPointPercent( int nPercent = 100 );
	int				GetFatiguePointPercent( int nPercent = 100 );
	int				GetHitPoint();
	int				GetManaPoint();
	int				GetFatiguePoint();
	int				GetMaxHitPoint();
	int				GetMaxManaPoint();
	int				GetMaxFatiguePoint();
	int				GetMaxOriginHitPoint( BOOL bOrinal = TRUE );
	int				GetMaxOriginManaPoint( BOOL bOrinal = TRUE );
	int				GetMaxOriginFatiguePoint( BOOL bOrinal = TRUE );
	int				GetHPRecovery();
	int				GetMPRecovery();
	int				GetFPRecovery();
	int				GetNaturalArmor();
 	float			GetAttackSpeed();
	float			GetCastingAniSpeed();
	int				GetCastingTime( int nCastingTime );
	float			GetDamageMultiplier( ATTACK_INFO* pInfo );
	int				PostCalcMagicSkill( int nATK, ATTACK_INFO* pInfo );
	int				PostCalcGeneric( int nATK, ATTACK_INFO* pInfo );
	float			GetMagicSkillFactor( CMover* pDefender, SAI79::ePropType skillType );
	float			GetATKMultiplier( CMover* pDefender, DWORD dwAtkFlags );
	float			GetDEFMultiplier( ATTACK_INFO* pInfo );
	float			GetBlockFactor( CMover* pAttacker, ATTACK_INFO* pInfo );
	int				GetWeaponATK( DWORD dwWeaponType );
	int				GetPlusWeaponATK( DWORD dwWeaponType );
	int				GetWeaponPlusDamage( int nDamage, BOOL bRandom = TRUE );
	void			GetDamagePropertyFactor( CMover* pDefender, int* pnATKFactor, int* pnDEFFactor, int nParts );
	int				GetPropATKPlus( int nParts );
	int				GetPropDEFPlus();
	void			GetDamageRange( int& nMin, int& nMax );
	BOOL			IsBlocking( CMover* pAttacker );
	BOOL			CanFlyByAttack();
	BOOL			IsCriticalAttack( CMover* pDefense, DWORD dwAtkFlags );
	int				GetCriticalProb();
	int				GetReqMp( int nReqMp );
	int				GetReqFp( int nReqFp );
	BOOL			GetAttackResult( CMover* pHitObj, DWORD dwOption );
	int				GetAdjHitRate();
	int				GetHitPower( ATTACK_INFO* pInfo );
	int				GetRangeHitPower( ATTACK_INFO* pInfo );
	int				GetParrying();
	int				GetDefenseByItem( BOOL bRandom = TRUE );
	int				GetShowDefense( BOOL bRandom );
	int				GetResistMagic();
	int				GetResistSpell( int nDestParam );
	int				GetMeleeSkillPower( ATTACK_INFO* pInfo );	// 근접공격 스킬데미지
	int				GetMagicHitPower( int nMagicPower );			// 완드공격 데미지
	int				GetItemAbility( int nItem );
	int				GetItemAbilityMin( int nItem );
	int				GetItemAbilityMax( int nItem );
	void			GetHitMinMax( int* pnMin, int* pnMax, ATTACK_INFO *pInfo = NULL );
	BOOL			IsAfterDeath();
	BOOL			IsDie() { return m_pActMover->IsDie() || m_nHitPoint == 0; }
	BOOL			IsLive() { return m_pActMover->IsDie() == FALSE || m_nHitPoint > 0; }		// && 를  ||로 바꿨음.  !=를 >로 바꿈
	int				GetCount() { return m_nCount; }
#if __VER >= 8 // __S8_PK
	void			SetPKPink( DWORD dwTime ) { if( dwTime == 0 || m_dwPKTime < dwTime ) m_dwPKTime = dwTime; }
	DWORD			GetPKPink( void ) { return m_dwPKTime; }
	BOOL			IsPKPink( void )	{ return m_dwPKTime > 0; }			/// PK 핑크 모드인지
	BOOL			IsChaotic( void ) { return m_dwPKPropensity > 0; }	/// 카오인지
	void			SetPKValue( int nValue );
	int				GetPKValue( void ) { return m_nPKValue; }
	void			SetPKPropensity( DWORD dwValue );
	DWORD			GetPKPropensity( void ) { return m_dwPKPropensity; }
	DWORD			NextPKPropensity( int nPKValue );
#else // __VER >= 8 // __S8_PK
	BOOL			IsGuardReactionChao();
	BOOL			IsGuardReactionNormal(); 
	BOOL			IsChaotic() { return GetSlaughterGrade() != SLAUGHTER_NORMAL; }	
	SLAUGHTER_GRADE	GetSlaughterGrade();						
#endif // __VER >= 8 // __S8_PK
	float			GetResist(SAI79::ePropType p_PropType);
	int				GetSetItemParts(DWORD dwParts);
	int				GetSetItemClient();
	int				GetSetItem( CItemElem* pItemElem = NULL );
	BOOL			IsSetItemPart( DWORD dwParts );
	void			SetSetItemAvail( int nAbilityOption );
	void			ResetSetItemAvail( int nAbilityOption );
	void			DestParamPiercingAvail( CItemElem* pItemElem, BOOL bSET = TRUE );
	BOOL			Pierce( CItemElem *pSuit, DWORD dwItemId );
	void			SetDestParamSetItem( CItemElem* pItemElem );
	void			ResetDestParamSetItem( CItemElem* pItemElem );
	int				GetEquipedSetItemNumber( CSetItem* pSetItem );
	void			GetEquipedSetItem( int nSetItemId, BOOL* pbEquiped, int* pnEquip );
	void			SetDestParamRandomOpt( CItemElem* pItemElem );
	void			ResetDestParamRandomOpt( CItemElem* pItemElem );
	
	void	SetDestParamRandomOptOrigin( CItemElem* pItemElem );
	void	SetDestParamRandomOptExtension( CItemElem* pItemElem );
	void	ResetDestParamRandomOptOrigin( CItemElem* pItemElem );
	void	ResetDestParamRandomOptExtension( CItemElem* pItemElem );

	void			GetItemATKPower( int *pnMin, int *pnMax, ItemProp* pItemProp, CItemElem *pWeapon ); 
	float			GetItemMultiplier( CItemElem* pItemElem );
	BOOL			SubLootDropNotMob( CItem *pItem );
	BOOL			SubLootDropMobSingle( CItem *pItem );
	BOOL			SubLootDropMobParty( CItem *pItem, CParty *pParty );
	BOOL			SubLootDropMob( CItem *pItem );
	BOOL			DoLoot( CItem *pItem );		// 바닥에 떨어진 pItem을 줍는다.
	void			PickupGoldCore( int nGold );
	void			PickupGold( int nGold, BOOL bDropMob );
	BOOL			IsDropable( CItemElem* pItemElem, BOOL bPK );
 	BOOL			IsDropableState( BOOL bPK );
	CItem*			DropGold( DWORD dwGold, const D3DXVECTOR3& vPos, BOOL bPK = FALSE );
#if __VER >= 15 // __PETVIS
	void			UpdateItem( BYTE nId, CHAR cParam, DWORD dwValue, DWORD dwTime = 0 );
#else // __PETVIS
	void			UpdateItem( BYTE nId, CHAR cParam, DWORD dwValue );
#endif // __PETVIS
#if __VER >= 11 // __SYS_IDENTIFY
	void		UpdateItemEx( unsigned char id, char cParam, __int64 iValue );
#endif	// __SYS_IDENTIFY
	CItem*			_DropItemNPC( DWORD dwItemType, DWORD dwID, short nDropNum, const D3DXVECTOR3 &vPos );
	CItem*			DropItem( DWORD dwID, short nDropNum, const D3DXVECTOR3 &vPos, BOOL bPlayer = FALSE );
	int				DoDropItemRandom( BOOL bExcludeEquip, CMover* pAttacker, BOOL bOnlyEquip = FALSE );
	int				GetItemNum( DWORD dwItemId );
#ifdef __CLIENT
	int				GetItemNumForClient( DWORD dwItemId ); // Client에서만 사용하는 아이템 갯수 구하기(Null check 이외의 Usable check안함)
#endif //__CLIENT
	int				RemoveAllItem( DWORD dwItemId );
	BOOL			AddItem( CItemBase* pItemBase );
	CItemBase*		GetItemId( DWORD dwId );
	void			RemoveItemId( DWORD dwId  );
	void			SetKeeptimeInven( DWORD dwItemId, DWORD dwTime );
	void			SetKeeptimeBank( DWORD dwItemId, DWORD dwTime );
	void			SetKeeptimeItem( CItemElem* pItemElem, DWORD dwTime );
	void			OnTradeRemoveUser();
	BOOL			AddItemBank( int nSlot, CItemElem* pItemElem );
	void			UpdateItemBank( int nSlot, BYTE nId, CHAR cParam, DWORD dwValue );
	CItemBase*		GetItemBankId( int nSlot, DWORD dwId );
	void			RemoveItemBankId( int nSlot, DWORD dwId );
	void			GenerateVendorItem( ItemProp** apItemProp, int* pcbSize, int nMax, LPVENDOR_ITEM pVendor );
	BOOL			DropItemByDied( CMover* pAttacker );		// 죽어서 떨어트리는 드랍.
#if __VER >= 8 // __S8_PK
	BOOL			DoUseSkill( DWORD dwSkill, int nLevel, OBJID idFocusObj, SKILLUSETYPE sutType = SUT_NORMAL, BOOL bControl = FALSE, const int nCastingTime = 0 );		// dwSkill/nLevel만 가지고도 사용할 수 있는 버전.
#else // __VER >= 8 // __S8_PK
	BOOL			DoUseSkill( DWORD dwSkill, int nLevel, OBJID idFocusObj, SKILLUSETYPE sutType = SUT_NORMAL, const int nCastingTime = 0 );		// dwSkill/nLevel만 가지고도 사용할 수 있는 버전.
#endif // __VER >= 8 // __S8_PK
	void			SetPKTargetLimit( int nSec );
	void			ClearCmd(); 
	OBJACT			GetCmd() { return m_oaCmd; }
	OBJACT			GetAct() { return m_oaAct; }
	int				GetCmdParam( int nIdx ) { return m_nCParam[nIdx]; }	
	void			ClearActParam(); 
	int				GetEnemyCount();
	OBJID			GetMaxEnemyHitID();
	DWORD			AddEnemy( OBJID objid, int nHit );
	int				GetEnemyHit( OBJID objid, DWORD* pdwTick = NULL );
	void			RemoveEnemy( OBJID objid  );
	void			RemoveAllEnemies();
	BOOL			IsAttackAble( CObj *pObj );
	FLOAT			GetAttackRange( DWORD dwAttackRange );
	BOOL			IsAttackAbleNPC( CMover* pNPC );
	BOOL			IsPKInspection( CMover* pMover );
	int				IsPKPVPInspectionBase( DWORD dwRegionAttr, BOOL bPVP = TRUE );
#if __VER >= 8     // 8차 듀얼존에 관계없이 PVP가능하게함   Neuz, World
	int				IsPVPInspectionBase( DWORD dwRegionAttr,DWORD dwWorldID, BOOL bPVP = TRUE );//8차 듀얼존에 관계없이 PVP가능하게함   Neuz, World
#endif	// __VER >= 8  
	BOOL			SubPKPVPInspectionBase( CMover* pMover, CMover* pMover1, DWORD dwPKAttr, int nFlag );
	void			PrintString( CMover* pMover, DWORD dwId );
	int				CMD_SetUseSkill( OBJID idTarget, int nSkillIdx, SKILLUSETYPE sutType = SUT_NORMAL );
	void			CMD_SetMeleeAttack( OBJID idTarget, FLOAT fRange = 0.0f );
	void			CMD_SetMagicAttack( OBJID idTarget, int nMagicPower );
	void			CMD_SetRangeAttack( OBJID idTarget, int nPower );
	void			CMD_SetMoveToPos( const D3DXVECTOR3 &vPos );
	void			CMD_SetMoveToObj( OBJID idObj );
	void			CMD_SetUseItem( CCtrl *pCtrl );
//	void			CMD_SetCollect( CObj *pObj );
	void			OnAfterUseItem( const ItemProp* pItemProp );
	void			UnequipRide();		
	void			EquipItem( CItemElem *pItemElem, BOOL bEquip, int nPart );
	BOOL			DoUseEquipmentItem( CItemElem* pItemElem, DWORD dwId, int nPart );
	BOOL			DoUseItemVirtual( DWORD dwItemId, BOOL bEffectSkip );
	BOOL			DoUseItem( DWORD dwItemId, DWORD dwFocusId, int nPart = -1 );
	BOOL			DoUseItemBlinkWing( ItemProp *pItemProp, CItemElem* pItemElem, BOOL bUes = TRUE );
	BOOL			DoUseItemFood_SM( ItemProp *pItemProp, CItemElem* pItemElem );
	BOOL			DoUseItemFood( ItemProp *pItemProp, CItemElem* pItemElem );
	BOOL			DoUseItemWarp( ItemProp* pItemProp, CItemElem* pItemElem );
	int				DoUseItemSystem( ItemProp *pItemProp, CItemElem* pItemElem, int nPart = -1 );
#ifdef __NEW_PROP_PARAMETER
	int				DoUseItemOnce( ItemProp* pItemProp, CItemElem* pItemElem, int nPart = -1);
	int				DoUseItemKeep( ItemProp* pItemProp, CItemElem* pItemElem, int nPart = -1);
#endif // __NEW_PROP_PARAMETER
	void			DoPickupItemAround();
	int				DoAttackMagic( CObj *pTargetObj, int nMagicPower,int idSfxHit = 0 );
	int				DoAttackRange( CObj *pTargetObj, DWORD dwItemID, int idSfxHit );
	int				DoAttackSP( CObj *pTargetObj, DWORD dwItemID );
	int				DoAttackMelee( CMover *pTarget, OBJMSG dwMsg = OBJMSG_ATK1, DWORD dwItemID = 0 );
	int				DoAttackMelee( OBJID idTarget, OBJMSG dwMsg = OBJMSG_ATK1, DWORD dwItemID = 0 );
	int				DoDie( CCtrl *pAttackCtrl, DWORD dwMsg = 0 );	// 뒈져라.	
	void			ChangeFame( CMover *pLose );
	int				DuelResult( CMover *pLose );
	PVP_MODE		GetPVPCase( CMover *pAttacker );
	void			SubPVP( CMover *pAttacker, int nReflect );	// 사람과 사람의 싸움에서 죽었을때.
	int				SubDuel( CMover *pAttacker );	// 듀얼중에 죽었을때 
	int				SubPK( CMover *pAttacker, int nReflect );		// PK에의해 죽었을때.
	int				SubWar( CMover *pAttacker );	// 전쟁때 죽었음
	int				SubExperience( CMover *pDead );
	void			AddKillRecovery();
	BOOL			IsValidArea( CMover* pMover, float fLength );
	void			SetJJim( CMover *pJJimer );		// this를 pJJimer가 찜했다는걸 표시.
	void			SubReferTime( DWORD *pTime1, DWORD *pTime2, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp );
	BOOL			ApplyParam( CCtrl *pSrc, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, BOOL bSend = TRUE, int nDamage = 0 );	// this에게 파라메터를 적용. pSrc는 시전자.
	BOOL			ApplyMagicSkill( CMover *pMover, DWORD dwSkill, DWORD dwLevel, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp );	// pMover에게 스킬효과를 적용함.
	BOOL			GetSkillProp( ItemProp **ppSkillProp, AddSkillProp **ppAddSkillProp, int nSkill, DWORD dwLevel, LPCTSTR szErr );
	BOOL			CreateSkillEffect( OBJID idTarget, int nSkill, DWORD dwLevel );		// 실제 스킬 발동.
	void			RemoveInvisible();		// 투명화 상태걸려있으면 해제함.
	void			SendDamageAround( int nDmgType, CMover *pAttacker, int nApplyType, int nAttackID, float fRange, float fTargetRatio, float fAroundRatio );	// this를 중심으로 주변에 데미지를 입힘.
	void			SendDamageLine( int nDmgType, int nApplyType, int nAttackID, float fWidth, float fHeight, float fDepth );
	void			ChangeExpRatio( CMover* pAttacker, CMover* pDefender );
	void			OnAttacked( CMover* pAttacker, int nDamage, BOOL bTarget, int nReflect );
	BOOL			OnDamage( int nItemID, DWORD dwState, CMover *pHitObj, const D3DXVECTOR3 *pvDamagePos = NULL, DWORD dwAtkFlag = AF_GENERIC );
#if __VER >= 9	// __SKILL_0706
	void			SetCoolTime( AddSkillProp* pAddSkillProp, LPCTSTR szCall );
#else	// __SKILL_0706
	void			SetCoolTime( ItemProp *pSkillProp, LPCTSTR szCall );
#endif	// __SKILL_0706
	BOOL			OnMeleeSkill( int nType, int nCount = 1 );		// OBJMSG_ATK_MELEESKILL실행후 타점이 되었을때 발생하는 이벤트.
	BOOL			OnMagicSkill( int nType, int nCount = 1 );		// OBJMSG_ATK_MAGICSKILL실행후 타점이 되었을때 발생하는 이벤트.
	BOOL			OnAttackRange();		// 장거리무기가 발사되는 시점에 발생하는 이벤트
	BOOL			OnAttackMelee( DWORD dwState, CMover *pHitObj );		// 근거리공격시 타점이 되었을때 발생하는 이벤트.
	BOOL			OnAttackMeleeContinue( DWORD dwState );		// 첫번째 타점이후부터 애니끝날때까지 계속.
	void			OnAttackMelee_ClockWorks( DWORD dwState, CMover *pHitObj );
	void			OnAttackMelee_BigMuscle( DWORD dwState, CMover *pHitObj );	
	void			OnAttackMelee_Krrr( DWORD dwState, CMover *pHitObj );	
	void			OnAttackMelee_Bear( DWORD dwState, CMover *pHitObj );	
#if __VER >= 10 // __Y_DRAGON_FIRE
	void			OnAttackMelee_Meteonyker( DWORD dwState, CMover *pHitObj );
#endif//__Y_DRAGON_FIRE
	BOOL			OnAttackMagic();		// 완드공격이 발사되는 시점에 발생하는 이벤트.
	BOOL			OnAttackSP();			// 특수공격.
	void			OnActDrop();			// 캐릭터가 높은곳에서 떨어지기 시작하는 시점에 발생.
	void			OnActCollision();		//
	void			OnActEndMeleeSkill();	// 근접스킬 동작이 끝나는 시점에 발생.
	void			OnActEndMagicSkill();	// 마법스킬 동작이 끝나는 시점에 발생.
	void			OnEndMeleeAttack( DWORD dwState );		// 일반동작 공격 끝날때 발생(몹만).
	void			OnActEndJump4();		// 착지 플래그가 사라지는 순간.
	void			OnActLanding();		// 착지 하는 순간.
	void			OnActFMove();
	void			OnActIALanding( CObj *pIAObj, const D3DXVECTOR3 &vPos );		// Interact오브젝트에 착지했을때.
	void			OnAttackStart( CMover *pTarget, OBJMSG dwMsg );					// 공격을 막 시작했을때(일반/스킬/완드 등)
	BOOL			IsBullet( ItemProp* pItemProp );
	void			ProcessMovePattern();
	int				GetMovePattern() { return m_nMovePattern; }
	int				GetMoveEvent() { return m_nMoveEvent; }
	void			SetMovePattern( int nPattern );		// 이동패턴 설정.
//	BOOL			DoCollect( CMover *pTarget );
	void			BehaviorActionForceSet();		// 명령이 행동 명령이면 강제 동기하고, 무빙 명령이면 무시한다.
	void			ActionForceSet( D3DXVECTOR3 &vPos, D3DXVECTOR3 &vDelta, FLOAT fAngle, 
									DWORD dwState, DWORD dwStateFlag, int nMotion, int nMotionEx, 
									int nLoop, DWORD dwMotionOption );
	void			ActionForceSet2( D3DXVECTOR3 &vPos, D3DXVECTOR3 &vDelta, FLOAT fAngle,
									 FLOAT fAngleX, FLOAT fAccPower, FLOAT fTurnAngle,
									 DWORD dwState, DWORD dwStateFlag, int nMotion, int nMotionEx, 
									 int nLoop, DWORD dwMotionOption );
	void			HalfForceSet( D3DXVECTOR3 & vPos, D3DXVECTOR3 & vd,	float fAngle, float fAnlgeX, 
		                          float fAccPower, float fTurnAngle );
	CGuild*			GetGuild();
	CGuildWar*		GetWar();
	BOOL			IsSMMode( int nType ) { return ( m_dwSMTime[nType] > 0 ) ? TRUE : FALSE; }
	void			ReState();
#ifdef __S_ADD_RESTATE
	void			ReStateOne( int nKind );
#endif // __S_ADD_RESTATE
#ifdef __ADD_RESTATE_LOW
	void			ReStateOneLow( int nKind );
#endif // __ADD_RESTATE_LOW
	float			GetJobPropFactor( JOB_PROP_TYPE type );
	int				CalcDefense( ATTACK_INFO* pInfo, BOOL bRandom = TRUE );
	int				CalcDefenseCore( CMover* pAttacker, DWORD dwAtkFlags, BOOL bRandom = TRUE );	
	int				CalcDefensePlayer( CMover* pAttacker, DWORD dwAtkFlags );
	int				CalcDefenseNPC( CMover* pAttacker, DWORD dwAtkFlags );
	void			PutLvUpSkillName_1( DWORD dwLevel );
	void			PutLvUpSkillName_2( DWORD dwSkill );
	BOOL			IsRegionMove( DWORD dwOlgRegionAttr, DWORD dwRegionAttr );
	DWORD			GetPKPVPRegionAttr();
	CItemBase*		GetVendorItem();		
	BOOL			IsAttackMode();
	void			AddSkillPoint( int nPoint);
	
#if __VER >= 8     // 8차 듀얼존에 관계없이 PVP가능하게함   Neuz, World
	void			DoPVPEnd( CCtrl *pAttackCtrl, bool bWinner , DWORD dwMsg = 0 );	// 듀얼끝난뒤 처리
	void			EndPVP(int	nPVPHP);	// 듀얼끝난뒤 처리
	BOOL			m_bPVPEnd;			//듀얼이 끝났는가
#endif	// __VER >= 8  

#if __VER >= 10 // __LEGEND	//	9차 전승시스템	Neuz, World, Trans
	BYTE			GetLegendChar()	;
	void			SetLegendChar(	BYTE	nLegend	)	{	m_nLegend	=	nLegend;	}
	BOOL			IsMaster();
	BOOL			IsHero();
	void			SetMasterSkillPointUp();
#ifdef __3RD_LEGEND16
	BOOL			IsLegendHero();
#endif
#endif	//__LEGEND	//	9차 전승시스템	Neuz, World, Trans

#if __VER >= 14 // __NEW_ITEM_LIMIT_LEVEL
	DWORD			GetJobType( int nJob = NULL_ID )	{	if( nJob == NULL_ID ) nJob = m_nJob;	return prj.m_aJob[ nJob ].dwJobType;	}	// 직업 타입
#endif // __NEW_ITEM_LIMIT_LEVEL


#if __VER >= 8 //__CSC_VER8_5
	void			AngelMoveProcess();
#endif //__CSC_VER8_5
#ifdef __EVE_BALLOON
	void			BalloonMoveProcess();
#endif //__EVE_BALLOON

#if __VER >= 8 // __S8_PK
	void			OnAttackSFX( OBJID	idTarget, int nMagicPower, DWORD dwSkill, int nDmgCnt, float	fDmgAngle, float fDmgPower,  DWORD dwAtkFlags, BOOL bControl );
#else // __VER >= 8 // __S8_PK
	void			OnAttackSFX( OBJID	idTarget, int nMagicPower, DWORD dwSkill, int nDmgCnt, float	fDmgAngle, float fDmgPower,  DWORD dwAtkFlags );
#endif // __VER >= 8 // __S8_PK
	BOOL			IsGuildCombatTarget( CMover* pMover );
#ifdef __WORLDSERVER
	void	ProcessSFXDamage( void );
	void	ProcessSFXExpire( void );
#endif	// __WORLDSERVER

#if __VER >= 9	// __PET_0410
	void	ProcessPet( void );
#endif	// __PET_0410

#ifdef __WORLDSERVER
	void			SetDestObj( OBJID idObj, float fRange = 0.0f, BOOL bSend = FALSE );
#else
	void			SetDestObj( OBJID idObj, float fRange = 0.0f, BOOL bSend = TRUE );
#endif

#if defined(__WORLDSERVER)
#if __VER < 8 // __S8_PK
	void			UpgradeKarma();
#endif // __VER < 8 // __S8_PK
	BOOL			IsPVPInspection( CMover* pMover, int nFlag = 0 );
	void			AddExperienceKillMember( CMover *pDead, EXPFLOAT fExpValue, MoverProp* pMoverProp, float fFxpValue );
	void			AddExperienceSolo( EXPFLOAT fExpValue, MoverProp* pMoverProp, float fFxpValue, BOOL bParty );
	void			AddExperienceParty( CMover *pDead, EXPFLOAT fExpValue, MoverProp* pMoverProp, float fFxpValue, CParty* pParty, CUser* apMember[], int* nTotalLevel, int* nMaxLevel10, int* nMaxLevel, int* nMemberSize );
	void			AddExperiencePartyContribution( CMover *pDead, CUser* apMember[], CParty* pParty, EXPFLOAT fExpValue, float fFxpValue, int nMemberSize, int nMaxLevel10 );
	void			AddExperiencePartyLevel( CUser* apMember[], CParty* pParty, EXPFLOAT fExpValue, float fFxpValue, int nMemberSize, int nMaxLevel10 );
	BOOL			GetPartyMemberFind( CParty* pParty, CUser* apMember[], int* nTotalLevel, int* nMaxLevel10, int* nMaxLevel, int* nMemberSize );
	float			GetExperienceReduceFactor( int nLevel, int nMaxLevel );
//	BOOL			IsResourceMonster() { return m_nResource != -1; }		// 자원몬스터냐? -1이면 자원몬스터가 아니다. 0 ~ 자원몬스터라는 뜻.
	void			ArrowDown( int nCount );
	int				GetQueueCastingTime();
#if __VER >= 8 // __S8_PK
	BOOL			DoUseSkill( int nType, int nIdx, OBJID idFocusObj, SKILLUSETYPE sutType, BOOL bControl );
	float			SubDieDecExp( BOOL bTransfer = TRUE, DWORD dwDestParam = 0, BOOL bResurrection = FALSE  );	// 죽었을때 겸치 깎는 부분.
#else // __VER >= 8 // __S8_PK
	BOOL			DoUseSkill( int nType, int nIdx, OBJID idFocusObj, SKILLUSETYPE sutType );
	float			SubDieDecExp( BOOL bTransfer = TRUE, DWORD dwDestParam = 0 );	// 죽었을때 겸치 깎는 부분.
#endif // __VER >= 8 // __S8_PK

	void			SubAroundExp( CMover *pAttacker, float fRange );		// this를 중심으로 fRange범위안에 있는 유저에게 경험치를 배분한다.
	void			AddPartyMemberExperience( CUser * pUser, EXPINTEGER nExp, int nFxp );
	void			GetDieDecExp( int nLevel, FLOAT& fRate, FLOAT& fDecExp, BOOL& bPxpClear, BOOL& bLvDown );
#if __VER >= 8 // __S8_PK
	void			GetDieDecExpRate( FLOAT& fDecExp, DWORD dwDestParam, BOOL bResurrection );
#else // __VER >= 8 // __S8_PK
	void			GetDieDecExpRate( FLOAT& fDecExp, DWORD dwDestParam, int nSlaughter );
#endif // __VER >= 8 // __S8_PK
	BOOL			CreateItem( CItemBase* pItemBase, BYTE* pnId = NULL, short* pnNum = NULL, BYTE nCount = 0 );
	void			RemoveItem( BYTE nId, short nNum );
	int				RemoveItemA( DWORD dwItemId, short nNum );
	void			RemoveVendorItem( CHAR chTab, BYTE nId, short nNum );
	void			RemoveItemIK3( DWORD dwItemKind3 );
	BOOL			DoUseItemSexChange( int nFace );
	BOOL			ReplaceInspection( REGIONELEM* pPortkey );
#ifdef __LAYER_1015
	BOOL			Replace( u_long uIdofMulti, DWORD dwWorldID, D3DXVECTOR3 & vPos, REPLACE_TYPE type, int nLayer );
#else	// __LAYER_1015
	BOOL			Replace( u_long uIdofMulti, DWORD dwWorldID, D3DXVECTOR3 & vPos, REPLACE_TYPE type );
#endif	// __LAYER_1015
#if __VER < 8 // __S8_PK
	int				ChangeSlaughter( CHANGE_SLAUGHTER_TYPE type, CMover* pDefender, int nSetCarmaPoint = 0 );
	int				IncSlaughterPoint2( CMover *pDead );
	int				IncSlaughterPoint( CMover *pDead );
#endif // __VER < 8 // __S8_PK
	BOOL			IsLoot( CItem *pItem, BOOL bPet = FALSE );
	void			ProcInstantBanker();
	void			ProcInstantGC();
	BOOL			IsItemRedyTime( ItemProp* pItemProp, OBJID dwObjid, BOOL bItemFind );
	int				ApplyDPC( int ATK, ATTACK_INFO* pInfo );
	int				CalcLinkAttackDamage( int nDamage ); // 파티스킬:링크어택의 추가 데이미계산 	
	int				CalcPropDamage( CMover* pDefender, DWORD dwAtkFlags );
	int				CalcGenericDamage( CMover* pDefender, DWORD& dwAtkFlags );
	int				GetMagicSkillPower( ATTACK_INFO* pInfo );
	void			SubSMMode();
	void			ClearAllSMMode();
	BOOL			SetSMMode( int nType, DWORD dwTime );
	void			Abrade( CMover* pAttacker, int nParts = PARTS_RWEAPON );
	DWORD			GetRandomPartsAbraded();
	void			SetMarkingPos();
	void			RemoveSFX( OBJID idTarget, int id, BOOL bForce, DWORD dwSkill );
	int				GetSFXCount( OBJID idTarget );
	void			IncSFXCount( OBJID idTarget, DWORD dwSkill );
	void			AddSFXInfo( OBJID idTarget, SFXHIT_INFO& info );
	void			ClearSFX( OBJID idTarget );
	int				GetSummonState();
	BOOL			IsDoUseBuff( ItemProp* pItemProp );
	
#endif // __WORLDSERVER

#ifdef __CLIENT
	virtual void	Render( LPDIRECT3DDEVICE9 pd3dDevice );

	void			InitInterpolation();
	void			Interpolate();
	BOOL			IsStateDbuff();
	BOOL			EndMotion();
	BOOL			IsPKAttackAble( CMover* pMover );
	BOOL			IsPVPAttackAble( CMover* pMover );
	CSfx*			CreateSfxArrow( DWORD dwSfxObjArrow, DWORD dwSfxObjHit, D3DXVECTOR3 vPosDest, int idTarget );
	void			SetDmgCnt( int nDmgCnt ) { m_nDmgCnt = nDmgCnt; }
	void			CreateAbilityOption_SetItemSFX( int nAbilityOption );
	BOOL			GetEquipFlag( int nParts, BYTE* pbyFlag );
	void			OverCoatItemRenderCheck(CModelObject* pModel);
	void			PlayCombatMusic();
	BOOL			IsLoot( CItem *pItem ) { return TRUE; }
	LPCTSTR			GetFameName();						// 명성 이름 얻기
#if __VER < 8 // __S8_PK
	LPCTSTR			GetSlaughterName();					// 슬러터이름 얻기
#endif // __VER < 8 // __S8_PK
	LPCTSTR			GetJobString();						// 직업 이름 얻기 
	void			DialogOut( LPCTSTR lpszText );		// 말풍선에 의한 대사 출력
	BOOL			DoFakeEquip( const EQUIP_INFO & rEquipInfo, BOOL bEquip, int nPart, CModelObject* pModel = NULL ); // for Fake
	void			RenderGauge( LPDIRECT3DDEVICE9 pd3dDevice, int nValue );
	void			RenderTurboGauge( LPDIRECT3DDEVICE9 pd3dDevice, DWORD nColor, int nValue, int nMaxValue );
	void			SetRenderPartsEffect( int nParts );
	void			RenderPartsEffect( LPDIRECT3DDEVICE9 pd3dDevice );
	void			RenderName( LPDIRECT3DDEVICE9 pd3dDevice, CD3DFont* pFont, DWORD dwColor = 0xffffffff );//adeilson
	void			RenderHP(LPDIRECT3DDEVICE9 pd3dDevice);
#if __VER >= 11 // __SYS_COLLECTING
	void			RenderCltGauge(LPDIRECT3DDEVICE9 pd3dDevice);
#endif
	void			RenderChrState(LPDIRECT3DDEVICE9 pd3dDevice);
	void			RenderFlag( int nType );
	void			RenderCasting( LPDIRECT3DDEVICE9 pd3dDevice );
	void			RenderCtrlCasting( LPDIRECT3DDEVICE9 pd3dDevice );
#if __VER >= 10 // __LEGEND	//	9차 전승시스템	Neuz, World, Trans
	void			RenderSkillCasting( LPDIRECT3DDEVICE9 pd3dDevice );
#endif	//__LEGEND	//	9차 전승시스템	Neuz, World, Trans
	void			RenderPVPCount( LPDIRECT3DDEVICE9 pd3dDevice );
	void			RenderQuestEmoticon( LPDIRECT3DDEVICE9 pd3dDevice );
	void			RenderGuildNameLogo( LPDIRECT3DDEVICE9 pd3dDevice, CD3DFont* pFont, DWORD dwColor );
#if __VER >= 8 // __S8_PK
	BOOL			DoUseSkill( int nType, int nIdx, OBJID idFocusObj, SKILLUSETYPE sutType, BOOL bControl, const int nCastingTime, DWORD dwSkill = 0, DWORD dwLevel = 0 );
#else // __VER >= 8 // __S8_PK
	BOOL			DoUseSkill( int nType, int nIdx, OBJID idFocusObj, SKILLUSETYPE sutType, const int nCastingTime, DWORD dwSkill = 0, DWORD dwLevel = 0 );
#endif // __VER >= 8 // __S8_PK
#if __VER >= 8 //__CSC_VER8_5
	void			RenderAngelStatus(LPDIRECT3DDEVICE9 pd3dDevice);
#endif //__CSC_VER8_5
#endif	// __CLIENT

	int				GetSkillLevel( SKILL* pSkill );
	BOOL			SetExperience( EXPINTEGER nExp1, int nLevel );

	BOOL	IsUsing( CItemElem* pItemElem );

	BOOL	IsShoutFull( void );
	BOOL	IsCommBank( void );

private:
	void			ProcessMove();
	void			ProcessMoveArrival( CCtrl *pObj );
	void			ProcessRegenItem();
	void			ProcessRegion();
	void			ProcessQuest();
	void			ProcessIAObjLink( D3DXVECTOR3& vPos);
	void			ProcessActionPoint();
#ifdef __CLIENT
	void			ProcessScaleSlerp();
	void			ProcessWaterCircle( const D3DXVECTOR3& vPosTemp, const D3DXVECTOR3& vPos );
	void			ProcessWalkSound();
	void			ProcessDustSFX();
	void			ProcessETC();
#else
	void			ProcessScript();
	void			ProcessAbnormalState();
	void			ProcessTarget();
#endif // __CLIENT

	BOOL			DropItem( CMover* pAttacker );		// 죽어서 떨어트리는 드랍.
	void			EnforcedGhostCorr();
	void			ApproachGhostAngle();
	void			OnArriveAtPos();
	void			OnArrive( DWORD dwParam1, DWORD dwParam2 );
	void			SetActParam( OBJACT act, int nParam1 = 0, int nParam2 = 0, int nParam3 = 0 );
	int				GetActParam( int nIdx ) { return m_nAParam[nIdx]; }
	void			SetCmd( OBJACT cmd, int nParam1 = 0, int nParam2 = 0, int nParam3 = 0 );
	void			SetCmdParam( int nIdx, int nValue ) { m_nCParam[nIdx] = nValue; }
	BOOL			__SetQuest( LPQUEST lpQuest, LPQUEST lpNewQuest );

#ifdef __INSTANCE_AGGRO_SYSTEM
private:
	DWORD m_dwIdOwner;
public:
	BOOL			HasOwner(){ return ( m_dwIdOwner != NULL && m_dwIdOwner != NULL_ID ); };
	DWORD			GetOwner(){ return m_dwIdOwner; };
	void			SetOwner( DWORD dwId ){ m_dwIdOwner = dwId; };
#endif//__INSTANCE_AGGRO_SYSTEM

public:
#ifdef __CLIENT
#if __VER >= 8 //__CSC_VER8_5
	void			CreateAngelParticle(D3DXVECTOR3 vPos);
#endif //__CSC_VER8_5

#if __VER >= 9	// __PET_0410
	void			CreatePetParticle(D3DXVECTOR3 vPos);
#endif //__PET_0410
	void			ProcessEyeFlash();
#if __VER >= 14 // __WING_ITEM
	void			WingMotionSetting( const CModelObject* pModel );
	float			GetRideFrameSpeed( void );
#endif // __WING_ITEM

#ifdef __BS_EFFECT_LUA
	const char*		GetNameO3D( );
	BOOL			SetDataMTE( const char* fname1, const char* fname2 );
#endif

#endif //__CLIENT
#ifndef __VM_0820
#ifndef __MEM_TRACE
	static			CMoverPool*		m_pPool;
	void*			operator new( size_t nSize )									{ return CMover::m_pPool->Alloc();	}
	void*			operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{ return CMover::m_pPool->Alloc();	}
	void			operator delete( void* lpMem )									{ CMover::m_pPool->Free( (CMover*)lpMem );	}
	void			operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{ CMover::m_pPool->Free( (CMover*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820
};


// 유저상태 알아봄
inline	BOOL CMover::IsMode( DWORD dwMode ) 
{ 
#ifdef __HACK_0516
	switch( dwMode )
	{
		case MATCHLESS_MODE:		// 무적 상태
		case ONEKILL_MODE:		// 초필 상태
		case MATCHLESS2_MODE:	// 무적 상태2
			if( m_dwAuthorization == AUTH_GENERAL )
				return FALSE;
			break;
	}
#endif	// __HACK_0516
	if( (dwMode & TRANSPARENT_MODE) )	// 투명화를 검사하러 들어왔을때
		if( GetAdjParam(DST_CHRSTATE) & CHS_INVISIBILITY )	// 마법투명화가 걸렸는가?
			return TRUE;
	return ( ( m_dwMode & dwMode ) == dwMode ) ? TRUE : FALSE; 
}	

inline	BOOL CMover::IsStateMode( DWORD dwMode ) 
{ 
	return ( ( m_dwStateMode & dwMode ) == dwMode ) ? TRUE : FALSE; 
}	

inline BOOL CMover::IsFly() 
{ 
#ifdef _DEBUG
	if( IsNPC() )
	{
		Error( _T( "CMover::IsFly : NPC는 IsFly()를 사용하면 안된다. IsFlyingNPC()를 사용하셔! %s" ), m_szName );
		return FALSE;
	}
#endif
	return m_pActMover->IsFly();
}

inline int	CMover::DoAttackMelee( OBJID idTarget, OBJMSG dwMsg, DWORD dwItemID )
{
	CMover *pTarget = (CMover *)prj.GetMover( idTarget );
	if( IsInvalidObj(pTarget) )
		return 0;

	return DoAttackMelee( pTarget, dwMsg, dwItemID );
}

#if !defined(__WORLDSERVER)
inline int CMover::IsSteal( OBJID idTaget )		// id로 검사하는 버전.
{
	CMover *pTarget = prj.GetMover( idTaget );
	if( IsInvalidObj(pTarget) )		
		return 0;
	else
		return 	IsSteal( pTarget );
}
#endif	// __WORLDSERVER

// 액션 매시지를 보냄 ; 객체의 움직임을 매시지로 제어함 
inline int CMover::SendActMsg( OBJMSG dwMsg, int nParam1, int nParam2, int nParam3, int nParam4 ) 	
{	
	if( m_pActMover )	
		return m_pActMover->SendActMsg( dwMsg, nParam1, nParam2, nParam3, nParam4 );	
	else
		return 0;
}

//raiders.2006.11.28	 trade돈을 계산에 포함하던 것을 제거
inline int CMover::GetGold()
{
	int nGold = m_dwGold;
//	nGold -= m_vtInfo.TradeGetGold();
	ASSERT( nGold >= 0 );
	return nGold;
}

inline void CMover::SetGold( int nGold )
{
	ASSERT( nGold >= 0 );
	m_dwGold = (DWORD)nGold;
}

inline void CMover::ClearCmd() 
{ 
//	TRACE( "ClearCmd %d->0 ", m_oaCmd );
	m_oaCmd = OBJACT_NONE; 
	memset( m_nCParam, 0xcd, sizeof(m_nCParam) ); 
}

inline void CMover::ClearActParam() 
{ 
	m_oaAct = OBJACT_NONE; 
	memset( m_nAParam, 0xcd, sizeof(m_nAParam) );
}


extern int GetWeaponPlusDamage( int nDamage, BOOL bRandom, ItemProp* pItemProp , int nOption );
extern int __IsEndQuestCondition( CMover* pMover, int nQuestId );
extern int __IsBeginQuestCondition( CMover* pMover, int nQuestId );
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
extern int __IsNextLevelQuest( CMover* pMover, int nQuestId );
#endif // __IMPROVE_QUEST_INTERFACE
extern	BOOL AttackBySFX( CMover* pAttacker, SFXHIT_INFO & info );

#endif // !defined(AFX_MOVER_H__4B7B21D7_A2D3_4115_946C_68DC9045A845__INCLUDED_)