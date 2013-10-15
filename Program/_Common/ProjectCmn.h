#ifndef __PROJECTCMN_H__
#define	__PROJECTCMN_H__

#define	MAX_OBJARRAY			8
#define	MAX_QUICKSLOT			21
#define	MAX_EQUIPMENT			18
#define	MAX_INVENTORY			42			// 60 //42
#define MAX_GUILDBANK			42
#define	MAX_BANK				42
#define	MAX_REPAIR				16
#define	MAX_TRADE				25
#define	MAX_VENDITEM			30
#define	MAX_REPAIRINGITEM		25
#define	MAX_VENDORNAME			48
#define	MAX_STUFF				( MAX_INVENTORY + MAX_QUICKSLOT )
#define	MAX_EXPLPPOINT			41
#define	MAX_EXPSKILL			21
#define	MAX_EXPCHARACTER		200
#define	MAX_FXPCHARACTER		200
#define	MAX_CHARACTER_LIST		3 
#define	MAX_VENDOR_INVENTORY	100
#define	MAX_VENDOR_INVENTORY_TAB	4
#define	MAX_QUEST				100
#define	MAX_COMPLETE_QUEST		300 
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
#define MAX_CHECKED_QUEST		5
#endif // __IMPROVE_QUEST_INTERFACE
#define MAX_PARTYLEVEL			10
#define MAX_ADDEXPPARTY			16
#define MAX_RENEWEVENT			8000
#define MAX_REALITEM			8			// 이벤트 상품 아이템
#define MAX_EVENTREALITEM		4096
#define MAX_EXPJOBLEVEL			90
#define	ITEM_KIND_MAX			100

const int	MAX_PROPMOVER =		2000;		// MoverProp배열의 최대갯수 

#define	MAX_HONOR_TITLE		150	// 달인타이틀 제한

#if __VER >= 8 // __S8_VENDOR_REVISION
#define MAX_VENDOR_REVISION		20
#endif //__S8_VENDOR_REVISION

#ifdef __NEW_STACKABLE_AMPS
#define MAX_AMP_STACK_COUNT	5
#endif // __NEW_STACKABLE_AMPS

#ifdef __16_SHOP
#define MAX_CART 12
#endif//__16_SHOP

#if __VER >= 19
#define MAX_QUEST_COND_KILL     3
#endif //__VER >= 19

#define dwDestParam1	dwDestParam[0]
#define dwDestParam2	dwDestParam[1]
#define nAdjParamVal1	nAdjParamVal[0]
#define nAdjParamVal2	nAdjParamVal[1]
#define dwChgParamVal1	dwChgParamVal[0]
#define dwChgParamVal2	dwChgParamVal[1]

#ifdef __PROP_0827
#define dwDestParam3	dwDestParam[2]
#define nAdjParamVal3	nAdjParamVal[2]
#define dwChgParamVal3	dwChgParamVal[2]
#endif	// __PROP_0827

#if __VER >= 19
#define dwDestParam4	dwDestParam[3]
#define nAdjParamVal4	nAdjParamVal[3]
#define dwChgParamVal4	dwChgParamVal[3]

#define dwDestParam5	dwDestParam[4]
#define nAdjParamVal5	nAdjParamVal[4]
#define dwChgParamVal5	dwChgParamVal[4]

#define dwDestParam6	dwDestParam[5]
#define nAdjParamVal6	nAdjParamVal[5]
#define dwChgParamVal6	dwChgParamVal[5]
#endif//__VER >= 19

#if __VER >= 9
#include "defineitemkind.h"
#endif//__VER >= 9

#if __VER >= 19
#include "defineItemGrade.h"
#include "defineItemType.h"
#endif//__VER >= 19

struct tagColorText
{
	DWORD dwColor;
	 
	TCHAR * lpszData;
}; 

struct ObjProp
{
	DWORD	dwID;		// 변수명	
	TCHAR	szName[64];	// 한글명칭 
	DWORD	dwType; 
	DWORD	dwAI;		// AIInterface
	DWORD	dwHP;
	ObjProp()
	{
		*szName	= '\0';
		dwID	= dwType	= dwAI	= dwHP	= 0;
	}
};

struct CtrlProp : ObjProp
{
	DWORD   dwCtrlKind1;
	DWORD   dwCtrlKind2;
	DWORD   dwCtrlKind3;
	DWORD   dwSfxCtrl;
	DWORD   dwSndDamage;
	CtrlProp() : ObjProp()
	{
		dwCtrlKind1	= dwCtrlKind2	= dwCtrlKind3	= dwSfxCtrl	= dwSndDamage	= 0;
	}

#if __VER >= 15 // __GUILD_HOUSE
	BOOL IsGuildHousingObj( )	{ return CK1_GUILD_HOUSE == dwCtrlKind1; } //길드 하우징 전용 오브젝트 인가?
	BOOL IsHousingObj( )		{ return CK1_HOUSING == dwCtrlKind1; }		//개인 하우징 전용 오브젝트 
#endif // __GUILD_HOUSE
};

struct AddSkillProp
{
	DWORD	dwID;				// 변수명	
	DWORD	dwName;				// 명칭
	DWORD	dwSkillLvl;			// 스킬레벨
	DWORD	dwAbilityMin;		// 최소능력
	DWORD	dwAbilityMax;		// 최대능력
	DWORD	dwAttackSpeed;		// 공격속도
	DWORD	dwDmgShift;			// 타격시 반동
	DWORD	nProbability;		// 발동 확률
	DWORD	dwTaunt;			// 타운트
	DWORD	dwDestParam[2];		// 적용대상1
	DWORD	nAdjParamVal[2];	// 적용값1;
	DWORD	dwChgParamVal[2];	//적용변화값1
	int		nDestData1[3];		// 적용데이타3개, DestParam[0]에 해당.
	DWORD	dwActiveSkill;		// 발동 마법
	DWORD	dwActiveSkillRate;	// 발동 마법 확률.
#if __VER >= 9	// __SKILL_0706
	DWORD	dwActiveSkillRatePVP;	// 대인 발동 마법 확률.
#endif	// __SKILL_0706
	int		nReqMp;				//필요MP
	int		nReqFp;				//필요FP
#if __VER >= 9	// __SKILL_0706
	DWORD	dwCooldown;		// 쿨다운
	DWORD	dwCastingTime;		//기술 준비시간
#else	// __SKILL_0706
	DWORD	dwSkillReady;		//기술 준비시간
#endif	// __SKILL_0706
	DWORD	dwSkillRange;		// 기술 시전거리	
	DWORD	dwCircleTime;		//유지시간
	DWORD   dwPainTime;         // 지속 피해시간
	DWORD	dwSkillTime;		//지속시간
	int		nSkillCount;		// 발사체 발사 개수. 개당공격력 = 총공격력 / nSkillCount;
	DWORD   dwSkillExp;         //스킬 경험치
	DWORD	dwExp;				//현재경험치
	DWORD	dwComboSkillTime;	//콤보스킬타임
#if __VER >= 9	// __SKILL_0706
	DWORD	dwAbilityMinPVP;	// 최소능력(대인)
	DWORD	dwAbilityMaxPVP;	// 최대능력(대인)
	DWORD	nProbabilityPVP;	// 발동 확률(대인)
#endif	// __SKILL_0706

	AddSkillProp()
	{
		dwID = dwName = 0;
		nProbability = 0;
		dwSkillLvl = dwAbilityMin = dwAbilityMax = dwAttackSpeed = dwDmgShift = dwTaunt = 0;
		dwDestParam[0] = dwDestParam[1] = nAdjParamVal[0] = nAdjParamVal[1] = dwChgParamVal[0] = dwChgParamVal[1] = 0;
		nReqMp = nReqFp = 0;
		dwActiveSkillRate	= 0;
#if __VER >= 9	// __SKILL_0706
		dwCastingTime	= 0;
		dwAbilityMinPVP		= 0;
		dwAbilityMaxPVP		= 0;
		dwCooldown	= 0;
		nProbabilityPVP	= 0;
		dwActiveSkillRatePVP	= 0;
#else	// __SKILL_0706
		dwSkillReady = 0;
#endif	// __SKILL_0706
		dwCircleTime = dwSkillTime = dwExp = dwComboSkillTime = 0;
	}
};

struct PartySkillProp
{
	DWORD	dwID;				// 변수명	
	DWORD	dwName;				// 명칭
	DWORD	dwSkillLvl;			// 스킬레벨
	DWORD	dwAbilityMin;		// 최소능력
	DWORD	dwAtkAbilityMax;	// 최대능력
	DWORD	dwAttackSpeed;		// 공격속도
	DWORD	dwDmgShift;			// 타격시 반동
	DWORD	dwGroggy;			// 그로키
	DWORD	dwTaunt;			// 타운트
	DWORD	dwDestParam[2];		// 적용대상1
	DWORD	nAdjParamVal[2];	// 적용값1;
	DWORD	dwChgParamVal[2];	//적용변화값1
	DWORD	dwReqMp;			//필요MP
	DWORD	dwReqFp;			//필요FP
	DWORD	dwSkillReady;		//기술 준비시간
	DWORD	dwCircleTime;		//유지시간
	DWORD	dwSkillTime;		//지속시간
	DWORD	dwExp;				//현재경험치
	DWORD	dwComboSkillTime;	//콤보스킬타임
	PartySkillProp()
	{
		dwID = dwName = 0;
		dwSkillLvl = dwAbilityMin = dwAtkAbilityMax = dwAttackSpeed = dwDmgShift = dwGroggy = dwTaunt = 0;
		dwDestParam[0] = dwDestParam[1] = nAdjParamVal[0] = nAdjParamVal[1] = dwChgParamVal[0] = dwChgParamVal[1] = 0;
		dwReqMp = dwReqFp = dwSkillReady = 0;
		dwCircleTime = dwSkillTime = dwExp = dwComboSkillTime = 0;
	}
};

enum 
{	
	FILE_FILTER	= 0, 
	FILE_INVALID = 1, 
	FILE_NOTICE = 2,	
	FILE_GUILDCOMBAT_TEXT_1 = 3, 
	FILE_GUILDCOMBAT_TEXT_2 = 4, 
	FILE_GUILDCOMBAT_TEXT_3 = 5, 
	FILE_GUILDCOMBAT_TEXT_4 = 6, 
	FILE_GUILDCOMBAT_TEXT_5 = 7,
	FILE_GUILDCOMBAT_TEXT_6 = 8
#ifdef __RULE_0615
	,FILE_ALLOWED_LETTER	= 9
#endif	// __RULE_0615
#if __VER >= 11 // __GUILD_COMBAT_1TO1
	,
	FILE_GUILDCOMBAT_1TO1_TEXT_1 = 10,
	FILE_GUILDCOMBAT_1TO1_TEXT_2 = 11,
	FILE_GUILDCOMBAT_1TO1_TEXT_3 = 12,
	FILE_GUILDCOMBAT_1TO1_TEXT_4 = 13,
	FILE_GUILDCOMBAT_1TO1_TEXT_5 = 14
#endif //__GUILD_COMBAT_1TO1
#ifdef __VENDOR_1106
	,FILE_ALLOWED_LETTER2	= 15
#endif	// __VENDOR_1106
};

// Item Property Type
enum IP_TYPE 
{
	IP_FLAG_NONE	= 0x00,				// 플래그 없음 
	IP_FLAG_BINDS	= 0x01,				// 1 - 귀속 아이템 
	IP_FLAG_UNDESTRUCTABLE	= 0x02	//	2 - 삭제 불가 아이템
#if __VER >= 14 // __EQUIP_BIND
	,IP_FLAG_EQUIP_BIND	= 0x04
#endif // __EQUIP_BIND
//	0x04
};

struct ItemProp : CtrlProp
{
	DWORD	dwMotion;			// 동작 
	DWORD	dwNum;				// 기본생성개수	
	DWORD	dwPackMax;			// 최대곂침개수	
	DWORD	dwItemKind1;		// 1차아이템종류	
	DWORD	dwItemKind2;		// 2차아이템종류	
	DWORD	dwItemKind3;		// 3차아이템종류	
	DWORD	dwItemJob;			// 아이템직업소속 
	BOOL	bPermanence;		// 소비불가	
	DWORD	dwUseable;			// 사용시점	
	DWORD	dwItemSex;			// 사용성별	
	DWORD	dwCost;				// 가격	 
	DWORD	dwEndurance;		// 내구력	
	int		nLog;				// 로그
	int		nAbrasion;			// 마모율
	int		nMaxRepair;			// 수리횟수
	DWORD	dwHanded;			// 잡는형식	
	DWORD	dwFlag;				// 다목적 플래그	
	DWORD	dwParts;			// 장착위치	
	DWORD	dwPartsub;			// 장착위치	
	DWORD	bPartsFile;			// 파츠파일 사용여부 
	DWORD	dwExclusive;		// 제거될위치	
	DWORD	dwBasePartsIgnore;
	DWORD	dwItemLV;			// 아이템레벨	
	DWORD	dwItemRare;			// 희귀성	
	DWORD   dwShopAble;
	int		nShellQuantity;		// 최대장전개수	- IK가 채집도구일때는 최대 채집량이 된다.(삭제 예정)
	DWORD	dwActiveSkillLv;	// 발동 마법 레벨
	DWORD   dwFuelRe;			// 팩당연료충전량
	DWORD	dwAFuelReMax;		// 가속연료충전량
	DWORD	dwSpellType;		// 마법속성	
	DWORD	dwLinkKindBullet;	// 필요 소모장비 아이템	
	DWORD	dwLinkKind;			// 필요 아이템종류
	DWORD	dwAbilityMin;		// 최소능력치 - 공격력, 방어력, 성능 기타등등 
	DWORD	dwAbilityMax;		// 최대능력치 - 공격력, 방어력, 성능 기타등등 
	BOOL	bCharged;		
	SAI79::ePropType	eItemType;
	short	wItemEatk;			// 속성 데미지( 위 속성 타입으로 공격력을 설정한다. )
	DWORD   dwParry;			// 회피율 
	DWORD   dwblockRating;		// 블럭 수치 
	int		nAddSkillMin;		// 최소 추가 스킬
	int		nAddSkillMax;		// 최대 추가 스킬.
	DWORD	dwAtkStyle;			// 공격 스타일 
	DWORD	dwWeaponType;		// 무기종류 
	DWORD	dwItemAtkOrder1;	// 아이템1차공격순서
	DWORD	dwItemAtkOrder2;	// 아이템2차공격순서
	DWORD	dwItemAtkOrder3;	// 아이템3차공격순서
	DWORD	dwItemAtkOrder4;	// 아이템4차공격순서
    DWORD	tmContinuousPain;	// 지속 피해 
	DWORD	dwRecoil;			// 반동	
	DWORD	dwLoadingTime;		// 장전시간	- IK가 채집도구일때는 채집속도(능력)이 된다.
	LONG	nAdjHitRate;		// 추가공격성공률	
	FLOAT	fAttackSpeed;		// 공격속도	
	DWORD	dwDmgShift;			// 타격시 반동	
	DWORD	dwAttackRange;		// 공격범위	
	int		nProbability;		// 적용확률

#if __VER >= 19
	DWORD	dwDestParam[6];
	DWORD	nAdjParamVal[6];
	DWORD	dwChgParamVal[6];
    int		nDestData1[6];
#else

#ifndef __PROP_0827
	DWORD	dwDestParam[2];		// 적용대상1	
	LONG	nAdjParamVal[2];	// 적용값1	
	DWORD	dwChgParamVal[2];	// 적용변화값1	
#else	// __PROP_0827
	DWORD	dwDestParam[3];		// 적용대상1	
	LONG	nAdjParamVal[3];	// 적용값1	
	DWORD	dwChgParamVal[3];	// 적용변화�
#endif	// __PROP_0827
	int		nDestData1[3];		// 적용데이타값 3개, destParam1에만 해당됨.
#endif//__VER >= 19

	DWORD	dwActiveSkill;		// 발동 마법
	DWORD	dwActiveSkillRate;	// 발동 마법 확률.
	DWORD	dwReqMp;			// 필요MP	
	DWORD	dwReqFp;			// 필요FP	
	DWORD	dwReqDisLV;
	DWORD   dwReSkill1;
	DWORD   dwReSkillLevel1;
	DWORD   dwReSkill2;
	DWORD   dwReSkillLevel2;
	DWORD	dwSkillReadyType;
	DWORD	dwSkillReady;		// 기술 준비시간	
	DWORD	_dwSkillRange;		// 기술 시전거리	
	DWORD	dwSfxElemental;
	DWORD	dwSfxObj;			// 생성할 효과 오브젝트 
	DWORD	dwSfxObj2;			// 생성할 효과 오브젝트 
	DWORD	dwSfxObj3;			// 발동효과, 타겟
	DWORD	dwSfxObj4;			// 지속되는 효과 이펙트.
	DWORD	dwSfxObj5;			// 발동효과, 시전자 
	DWORD	dwUseMotion;		// 사용 모션
	DWORD	dwCircleTime;		// 유지시간 
	DWORD	dwSkillTime;		// 지속시간	
	DWORD	dwExeTarget;
	DWORD	dwUseChance;
	DWORD	dwSpellRegion;		// 마법 범위 
	DWORD   dwReferStat1;
	DWORD   dwReferStat2;
	DWORD   dwReferTarget1;
	DWORD   dwReferTarget2;
	DWORD   dwReferValue1;
	DWORD   dwReferValue2;
	DWORD	dwSkillType;		// 기술속성	
	int		nItemResistElecricity;
	int		nItemResistDark;
	int		nItemResistFire;
	int		nItemResistWind;
	int		nItemResistWater;
	int		nItemResistEarth;
	LONG	nEvildoing;			// 악행	
	DWORD	dwExpertLV;	
	DWORD	dwExpertMax;		// 최대숙련레벨 
	DWORD	dwSubDefine;
	DWORD	dwExp;				// 현재경험치	
	DWORD	dwComboStyle;
	FLOAT	fFlightSpeed;		// 비행추진력(속도)
	FLOAT	fFlightLRAngle;		// 좌우 턴 각도.
	FLOAT	fFlightTBAngle;		// 상하 턴 각도.
	DWORD	dwFlightLimit;		// 비행제한레벨
	DWORD	dwFFuelReMax;		// 비행연료충전량
	DWORD	dwLimitLevel1;		// 제한레벨1
	int		nReflect;			// 리플렉션 옵션.
	DWORD	dwSndAttack1;		// 효과음 : 공격 1
	DWORD	dwSndAttack2;		// 효과음 : 공격 2
	DWORD	dwQuestId;
	TCHAR	szTextFileName[64];	// item에 GM command에 넣는 것에 사용

#ifdef __CLIENT
    TCHAR	szIcon[64];			// dds파일 이름 
	TCHAR	szCommand[256];		// 설명문 
#endif
	int		nVer;

#ifdef __VERIFY_0201
	TCHAR	szIcon[64];
	TCHAR	szCommand[256];		// 설명문 
#endif	// __VERIFY_0201

#ifdef __NEW_PROP_PARAMETER
	DWORD	dwBuffTickType;
	int		nMinLimitLevel;
	int		nMaxLimitLevel;
	int		nItemGroup;
	int		nUseLimitGroup;
	int		nMaxDuplication;
	int		nEffectValue;
	int		nTargetMinEnchant;
	int		nTargetMaxEnchant;
	BOOL	bResetBind;
	int		nBindCondition;
	int		nResetBindCondition;
	DWORD	dwHitActiveSkillId;
	DWORD	dwHitActiveSkillLv;
	DWORD	dwHitActiveSkillProb;
	DWORD	dwHitActiveSkillTarget;
	DWORD	dwDamageActiveSkillId;
	DWORD	dwDamageActiveSkillLv;
	DWORD	dwDamageActiveSkillProb;
	DWORD	dwDamageActiveSkillTarget;
	DWORD	dwEquipActiveSkillId;
	DWORD	dwEquipActiveSkillLv;
	DWORD	dwSmelting;
	DWORD	dwAttsmelting;
	DWORD	dwGemsmelting;
	DWORD	dwPierce;
	DWORD	dwUprouse;
	BOOL	bAbsoluteTime;
#endif // __NEW_PROP_PARAMETER

#if __VER >= 18
	DWORD   dwItemGrade;
	BOOL	bCanTrade;
	DWORD   dwMainCategory;
	DWORD   dwSubCategory;
	BOOL    bCanHaveServerTransform;
	BOOL	bCanSavePotion;
	BOOL	bCanLooksChange;
	BOOL	blsLooksChangeMaterial;
	BOOL	bCanUseActionSlot;
	DWORD	dwEquipItemKeepSkill;
	DWORD	dwMonsterGrade;
#endif//__VER >= 18

	DWORD	GetCoolTime()  { return dwSkillReady; }
#if __VER >= 9	// __ULTIMATE
	BOOL	IsUltimate( void )
		{
			return( 
					(
						dwItemKind2 == IK2_WEAPON_DIRECT
						|| dwItemKind2 == IK2_WEAPON_MAGIC
					)
				&& dwReferStat1 == WEAPON_ULTIMATE
			);
		}
#endif	//
#ifdef __NEW_ITEM_VARUNA
	BOOL IsBaruna( void )
	{
		return( dwReferStat1 >= BARUNA_D && dwReferStat1 <= BARUNA_S );
	}
#endif // __NEW_ITEM_VARUNA
#ifdef __NEW_STACKABLE_AMPS
	BOOL IsExperienceScroll( void )
	{
		return( dwItemKind3 == IK3_EXP_RATE );
	}
#endif // __NEW_STACKABLE_AMPS

#ifdef __TABBED_INV
		int	GetInvenIndex( )
		{
			int nIndex = 0;
			switch( dwItemKind3 )
			{
				case IK3_GEM:
					nIndex = 3;
					break;
				case IK3_SUMMON_NPC:
				case IK3_PET:
					nIndex = 1;
					break;
				case IK3_GLOVE:
				case IK3_CLOTH:
				case IK3_CLOAK:
				case IK3_SHOES:
				case IK3_MASK:
				case IK3_HAT:
					nIndex = 2;
					break;
				default:
					nIndex = 0;
					break;
			}
			return nIndex;
		};
#endif//tabb_inv

#if __VER >= 15 // __PETVIS
	BOOL	IsVisPet()	{ return (dwItemKind3 == IK3_PET) && (dwReferStat1 == PET_VIS); }
	BOOL	IsVis()		{ return (dwItemKind3 == IK3_VIS ); }
#ifdef __CLIENT
	BOOL	IsVisKey()	{ return (II_SYS_SYS_VIS_KEY01 == dwID ); }
	BOOL	IsPickupToBuff( ) { return ( II_SYS_SYS_SCR_PET_MAGIC == dwID ); }
#endif //__CLIENT
#endif // __PETVIS
/*#ifdef __INSTANCE_AGGRO_SYSTEM
	BOOL IsDebuff()
	{
		if (   nEvildoing != 1
			&& dwReferTarget1 != RT_ATTACK
			&& dwReferTarget2 != RT_ATTACK )
			return TRUE;
		else
			return FALSE;
	}
	BOOL IsBuff()
	{
		return( nEvildoing == 1 );
	}
	BOOL IsHeal()
	{
		return( dwReferTarget1 == RT_HEAL );
	}
	BOOL IsAlwaysAggro()
	{
		return ( dwID == 400 );
	}
#endif // __INSTANCE_AGGRO_SYSTEM*/
};


// 직업에 따른 factor ENUM
enum JOB_PROP_TYPE
{
	JOB_PROP_SWD,
	JOB_PROP_AXE,	
	JOB_PROP_STAFF,
	JOB_PROP_STICK,
	JOB_PROP_KNUCKLE,
	JOB_PROP_WAND,
	JOB_PROP_BLOCKING,
	JOB_PROP_YOYO,
	JOB_PROP_CRITICAL,
};

// 직업에 따른 프로퍼티 ( propJob.inc에서 읽어들임 )
struct JobProp
{
	float	fAttackSpeed;			//공속 
	float	fFactorMaxHP;			//최대 HP 계산에 사용되는 factor
	float	fFactorMaxMP;			//최대 MP 계산에 사용되는 factor
	float	fFactorMaxFP;			//최대 FP 계산에 사용되는 factor
	float   fFactorDef;				//물리 방어력 계산에 사용되는 factor
	float	fFactorHPRecovery;		//HP회복 factor
	float	fFactorMPRecovery;		//MP회복 factor
	float	fFactorFPRecovery;		//FP회복 factor
	float	fMeleeSWD;				//WT_MELEE_SWD의 ATK factor
	float	fMeleeAXE;				//WT_MELEE_AXE의 ATK factor
	float   fMeleeSTAFF; 			//WT_MELEE_STAFF의 ATK factor
	float   fMeleeSTICK;			//WT_MELEE_STICK의 ATK factor
	float   fMeleeKNUCKLE;			//WT_MELEE_KNUCKLE의 ATK factor
	float   fMagicWAND;				//WT_MAGIC_WAND의 ATK factor 
	float   fBlocking;				//블록킹 factor
	float	fMeleeYOYO;				//요요의 ATK factor 
	float   fCritical;				//크리티컬 처리
};

typedef	struct	tagRANDOM_ITEM
{
	BYTE	m_lpQuestFlagArray[ITEM_KIND_MAX / 8 + 1];

	void	SetItemKind( int nItemKind,BOOL bBool );
	BOOL	IsItemKind( int nItemKind );

	BYTE	m_nTotalNum;	// 발생 아이템 갯수 
	BYTE	m_nUniqueMax;	// 유니크 아이템 최대 
	BYTE	m_nUniqueMin;	// 유니크 아이템 최소  
	WORD	m_nGoldMax;		// 골드 맥스 
	WORD	m_nAmmoMax;		// 총알 맥스 
	WORD	m_nDartMax;		// 표창 맥스 
	WORD	m_nGoldMin;		// 골드 맥스 
	WORD	m_nAmmoMin;	// 총알 맥스 
	WORD	m_nDartMin;	// 표창 맥스 
	tagRANDOM_ITEM()
	{
		m_nTotalNum		= 0;
		m_nUniqueMax	= 0;
		m_nUniqueMin	= 0;
		m_nGoldMax		= 0;
		m_nAmmoMax		= 0;
		m_nDartMax		= 0;
		m_nGoldMin		= 0;
		m_nAmmoMin		= 0;
		m_nDartMin		= 0;
	}
}
RANDOM_ITEM,* LPRANDOM_ITEM;

/*----------------------------------------------------------------------------------------------------*/
enum DROPTYPE
{
	DROPTYPE_NORMAL,
	DROPTYPE_SEED,
};

typedef struct	tagDROPITEM
{
	DROPTYPE	dtType;
	DWORD	dwIndex;
	DWORD	dwProbability;
	DWORD	dwLevel;
	DWORD	dwNumber;
	DWORD	dwNumber2;	// Min, Max중 Max로 씀.
}
DROPITEM,	*LPDROPITEM;

typedef	struct	tagDROPKIND
{
	DWORD	dwIK3;
	short	nMinUniq;
	short	nMaxUniq;
}
DROPKIND,	*LPDROPKIND;

typedef	struct	tagQUESTITEM
{
	DWORD	dwQuest;
	DWORD   dwState;
	DWORD	dwIndex;
	DWORD	dwProbability;
	DWORD	dwNumber; 
}
QUESTITEM,	*PQUESTITEM;

typedef struct tagEVENTITEM
{
	DWORD	dwIndex;
	DWORD	dwPrabability;
}
EVENTITEM,	*PEVENTITEM;

#define MAX_QUESTITEM	16
#define	MAX_DROPKIND	80

class CDropItemGenerator
{
private:
	vector<DROPITEM>	m_dropItems;

public:
	DWORD				m_dwMax;

public:
//	Contructions
	CDropItemGenerator() { m_dwMax = 0; }
	virtual	~CDropItemGenerator()	{	Clear();	}

//	Operations
	void		AddTail( CONST DROPITEM & rDropItem, const char* s );
	void	Clear( void )	{	m_dropItems.clear();	}
	DWORD		GetSize( void )	{	return m_dropItems.size();	}
	DROPITEM*	GetAt( int nIndex, BOOL bUniqueMode, float fProbability = 0.0f );
};

class CDropKindGenerator
{
private:
	int		m_nSize;
	DROPKIND	m_aDropKind[MAX_DROPKIND];
public:
//	Contructions
	CDropKindGenerator()
		{	m_nSize	= 0;	}
	virtual	~CDropKindGenerator()	{}
//	Operations
	void	AddTail( const DROPKIND & rDropKind );	// memcpy
	int		GetSize( void )		{	return m_nSize;	}
	LPDROPKIND	GetAt( int nIndex );
};

class CQuestItemGenerator
{
private:
	u_long	m_uSize;
	QUESTITEM	m_pQuestItem[MAX_QUESTITEM];
public:
	CQuestItemGenerator()
		{	m_uSize	= 0;	}
	~CQuestItemGenerator()	{}
	void	AddTail( const QUESTITEM & rQuestItem );
	u_long	GetSize( void )	{	return m_uSize;	}
	QUESTITEM*	GetAt( int nIndex );
};

#if __VER >= 14 // __INSTANCE_DUNGEON
struct MonsterTransform
{
	float fHPRate;
	DWORD dwMonsterId;
	MonsterTransform() : fHPRate( 0.0f ), dwMonsterId( NULL_ID ) {}
};
#endif // __INSTANCE_DUNGEON
/*----------------------------------------------------------------------------------------------------*/

struct MoverProp : CtrlProp
{
	DWORD	dwStr;	// 힘,                  
	DWORD	dwSta;	// 체력,
	DWORD	dwDex;	// 민첩,
	DWORD	dwInt;	// 지능,
	DWORD	dwHR;
	DWORD   dwER;
	DWORD	dwRace;		// 종족,
	DWORD	dwBelligerence;		// 호전성,
	DWORD	dwGender;	// 성별,
	DWORD	dwLevel;	// 레벨,
	DWORD	dwFlightLevel;	// 비행레벨
	DWORD	dwSize;		// 크기,
	DWORD   dwClass;
	BOOL	bIfParts;	// 파츠냐?
	int		nChaotic;	// 나쁜놈 마이너스/ 좋은넘 플러스
#ifdef __S1108_BACK_END_SYSTEM
	DWORD	dwUseable;	// 방어 캐릭수,
#else // __S1108_BACK_END_SYSTEM
	DWORD	dwDefExtent;	// 방어 캐릭수,
#endif // __S1108_BACK_END_SYSTEM
	DWORD	dwActionRadius;		// 전투행동번경,
	DWORD	dwAtkMin;	// 최소타격치,
	DWORD	dwAtkMax;	// 최대타격치,
	DWORD	dwAtk1;
	DWORD	dwAtk2;
	DWORD	dwAtk3;
	DWORD	dwAtk4;		// dwHorizontalRate가 이걸로 바뀜.
#if __VER >= 9	//__AI_0509
	FLOAT	fFrame;	// 이동 시 프레임 가중치
	DWORD	dwOrthograde;
#else	// __AI_0509
	DWORD	dwVerticalRate;		// 세로비율,
	DWORD	dwDiagonalRate;		// 사선비율,
#endif	// __AI_0509
	DWORD	dwThrustRate;	// 찌르기비율,

	DWORD	dwChestRate;
	DWORD	dwHeadRate;  
	DWORD	dwArmRate;
	DWORD	dwLegRate;

	DWORD	dwAttackSpeed;	// 공격속도,
	DWORD	dwReAttackDelay;
	DWORD	dwAddHp;		// ,
	DWORD	dwAddMp;		// ,
	DWORD	dwNaturalArmor;	// 자연방어력 
	int		nAbrasion;	// 마모도
	int		nHardness;	// 경도
	DWORD	dwAdjAtkDelay;	// 추가공격속도저하,

	SAI79::ePropType	eElementType;
	short				wElementAtk;		// 속성 데미지( 위 속성 타입으로 공격력을 설정한다. )

	DWORD	dwHideLevel;	// 레벨 안보이는넘이냐..
	FLOAT	fSpeed;	// 이동속도,
	DWORD	dwShelter;	// 거주지역,
	DWORD	dwFlying;	// 비행유무,
	DWORD	dwJumpIng;	// 높이뛰기 
	DWORD	dwAirJump;	// 멀리뛰기
	DWORD	bTaming;	// 조련유무 
	DWORD	dwResisMgic;	//마법저항 

	int		nResistElecricity;
	int		nResistDark;
	int		nResistFire;
	int		nResistWind;
	int		nResistWater;
	int		nResistEarth;
	
	DWORD	dwCash;		// 제공금액
	DWORD	dwSourceMaterial;	// 제공재료
	DWORD	dwMaterialAmount;	// 재료양
	DWORD	dwCohesion;	// 재료응집도
	DWORD	dwHoldingTime;	// 시체유지시간
	DWORD	dwCorrectionValue;	// 아이템생성보정값
	EXPINTEGER	nExpValue;
	int		nFxpValue;		// 비행경험치.
	DWORD	nBodyState;		// 몸상태,
	DWORD	dwAddAbility;	// 추가능력,
	DWORD	bKillable;	// 죽음유무,

	DWORD	dwVirtItem[3];
	DWORD	bVirtType[3]; 

	DWORD   dwSndAtk1  ;
	DWORD   dwSndAtk2  ;
	DWORD   dwSndDie1  ;
	DWORD   dwSndDie2  ;
	DWORD   dwSndDmg1  ;
	DWORD   dwSndDmg2  ;
	DWORD   dwSndDmg3  ;
	DWORD   dwSndIdle1 ;
	DWORD   dwSndIdle2 ;

	short   m_nEvasionHP;
	short	m_nEvasionSec;
	short   m_nRunawayHP          ; // HP가 10 이하면 도주 
	short   m_nCallHelperMax       ; // 총 갯수 
	short   m_nCallHP              ; // 도움요청하기 위한 HP
	short   m_nCallHelperIdx  [ 5 ]; // 도움요청하기 Id
	short   m_nCallHelperNum  [ 5 ]; // 도움요청하기 Id
	short   m_bCallHelperParty[ 5 ]; // 도움요청하기 Id

	short   m_dwAttackMoveDelay;
	short   m_dwRunawayDelay;
	short   m_bPartyAttack;
	short   m_nHelperNum;

#if !defined(__CORESERVER) 
	int		m_nScanJob;		// 타겟을 검색할때 특정 직업으로 검색하는가. 0 이면 ALL
	// 공격조건들은 비스트에서 선공으로 지정되어 있을때 얘기다.
	int		m_nHPCond;		// 타겟이 hp% 이하일때 공격을 함.
	int		m_nLvCond;		// 타겟과 레벨비교하여 낮으면 공격을 함.
	int		m_nRecvCondMe;	// 회복 조건.  내hp가 몇%이하로 떨어졌을때?
	int		m_nRecvCondHow;	// 회복할때 회복할 양 %단위
	int		m_nRecvCondMP;	// 회복할때 MP소모량 %단위
	BYTE	m_bMeleeAttack;	// 근접 공격 AI가 있는가?
	BYTE	m_bRecvCondWho;	// 누구를 치료할꺼냐. 0:지정안됨 1:다른놈 2:나 3:모두.
	BYTE	m_bRecvCond;	// 치료하냐? 0:치료안함 1:전투중에만 치료함 2:전투/비전투 모두 치료
	BYTE	m_bHelpWho;		// 도움요청시 - 0:부르지않음 1:아무나  2:같은종족만.
	BYTE	m_bRangeAttack[ MAX_JOB ];		// 각 직업별 원거리 공격 거리.
	int		m_nSummProb;	// 소환 확률 : 0이면 소환능력 없음.
	int		m_nSummNum;		// 한번에 몇마리나 소환하냐.
	int		m_nSummID;		// 어떤 몬스터?
	int		m_nHelpRangeMul;	// 도움요청 거리. 시야의 배수
	DWORD	m_tmUnitHelp;			// 헬프 타이머.
	int		m_nBerserkHP;		// 버서커가 되기 위한 HP%
	float	m_fBerserkDmgMul;	// 버서커가 되었을때 데미지 배수.
	int		m_nLoot;			// 루팅몹인가.
	int		m_nLootProb;		// 루팅 확률
	DWORD   m_dwScanQuestId; 
	DWORD   m_dwScanItemIdx; 
	int		m_nScanChao;		// 카오, 비카오 검색
#endif // !__CORESERVER
	
#ifdef __S1108_BACK_END_SYSTEM
	float	m_fHitPoint_Rate;		// 몬스터 최대 HP률 // dwAddHp * m_nHitPoint_Rate
	float	m_fAttackPower_Rate;	// 몬스터 최대 공격률 // dwAtkMin * m_nAttackPower_Rate
	float	m_fDefence_Rate;		// 몬스터 최대 방어률 // dwAddHp * m_nDefence_Rate
	float	m_fExp_Rate;			// 몬스터 최대 경험치률 // dwAddHp * m_nExp_Rate
	float	m_fItemDrop_Rate;		// 몬스터 최대 아이템 드롭률 // dwAddHp * m_nItemDrop_Rate
	float	m_fPenya_Rate;			// 몬스터 최대 페냐률 // dwAddHp * m_nPenya_Rate
	BOOL	m_bRate;
#endif // __S1108_BACK_END_SYSTEM

	
	short	m_nAttackItemNear;
	short	m_nAttackItemFar;
	short	m_nAttackItem1;
	short	m_nAttackItem2;
	short	m_nAttackItem3;
	short	m_nAttackItem4;
	short	m_nAttackItemSec;
	short	m_nMagicReflection;
	short	m_nImmortality;
	BOOL 	m_bBlow;
	short	m_nChangeTargetRand;

	short   m_nAttackFirstRange;
	RANDOM_ITEM		m_randomItem  ;
	CDropItemGenerator	m_DropItemGenerator;
	CQuestItemGenerator		m_QuestItemGenerator;
	CDropKindGenerator	m_DropKindGenerator;
#if __VER >= 14 // __INSTANCE_DUNGEON
	MonsterTransform	m_MonsterTransform;
#endif // __INSTANCE_DUNGEON
	
#if __VER >= 19
	DWORD	dwAreaColor;
	char	szNpcMark[64];
	DWORD	dwMadrigalGiftPoint;
#endif

	MoverProp()
	{
#if __VER >= 19
		dwAreaColor = AREA_NORMAL;
#endif
		dwStr	= dwSta	= dwDex	= dwInt	= dwHR	= dwER	= dwRace	= dwBelligerence	=dwGender
		= dwLevel	= dwFlightLevel	= dwSize	= dwClass	= bIfParts	= nChaotic
		#ifdef __S1108_BACK_END_SYSTEM
		= dwUseable
		#else // __S1108_BACK_END_SYSTEM
		= dwDefExtent
		#endif // __S1108_BACK_END_SYSTEM
		= dwActionRadius	= dwAtkMin	= dwAtkMax	= dwAtk1	= dwAtk2	= dwAtk3	= dwAtk4	= 0;
		#if __VER >= 9	//__AI_0509
			fFrame	= 1.0F;
			dwOrthograde	= 0;
		#else	// __AI_0509
			dwVerticalRate	= dwDiagonalRate	= 0;
		#endif	// __AI_0509
		dwThrustRate		= 0;
		dwChestRate			= 0;
		dwHeadRate			= 0;
		dwArmRate			= 0;
		dwLegRate			= 0;
		dwAttackSpeed		= 0;
		dwReAttackDelay		= 0;
		dwAddHp				= 0;
		dwAddMp				= 0;
		dwNaturalArmor		= 0;
		nAbrasion			= 0;
		nHardness			= 0;
		dwAdjAtkDelay		= 0;
		dwHideLevel			= 0;
		eElementType		= SAI79::NO_PROP;
		wElementAtk			= 0;
		fSpeed				= 0.0F;
		dwShelter			= 0;
		dwFlying			= 0;
		dwJumpIng			= 0;
		dwAirJump			= 0;
		bTaming				= 0;
		dwResisMgic			= 0;
		nResistElecricity	= 0;
		nResistDark			= 0;
		nResistFire			= 0;
		nResistWind			= 0;
		nResistWater		= 0;
		nResistEarth		= 0;
		dwCash				= 0;
		dwSourceMaterial	= 0;
		dwMaterialAmount	= 0;
		dwCohesion			= 0;
		dwHoldingTime		= 0;
		dwCorrectionValue	= 0;
		nExpValue			= 0;
		nFxpValue			= 0;
		nBodyState			= 0;
		dwAddAbility		= 0;
		bKillable			= 0;
		
		memset( dwVirtItem, 0, sizeof(dwVirtItem) );
		memset( bVirtType, 0, sizeof(bVirtType) );

		dwSndAtk1			= 0;
		dwSndAtk2			= 0;
		dwSndDie1			= 0;
		dwSndDie2			= 0;
		dwSndDmg1			= 0;
		dwSndDmg2			= 0;
		dwSndDmg3			= 0;
		dwSndIdle1			= 0;
		dwSndIdle2			= 0;
		m_nEvasionHP		= 0;
		m_nEvasionSec		= 0;
		m_nRunawayHP		= 0;
		m_nCallHelperMax	= 0;
		m_nCallHP			= 0;
		
		memset( m_nCallHelperIdx, 0, sizeof(m_nCallHelperIdx) );
		memset( m_nCallHelperNum, 0, sizeof(m_nCallHelperNum) );
		memset( m_bCallHelperParty, 0, sizeof(m_bCallHelperParty) );
		
		m_dwAttackMoveDelay	= 0;
		m_dwRunawayDelay	= 0;
		m_bPartyAttack		= 0;
		m_nHelperNum		= 0;

#if !defined(__CORESERVER) 
		m_nScanJob			= 0;
		m_nHPCond			= 0;
		m_nLvCond			= 0;
		m_nRecvCondMe		= 0;
		m_nRecvCondHow		= 0;
		m_nRecvCondMP		= 0;
		m_bMeleeAttack		= 0;
		m_bRecvCondWho		= 0;
		m_bRecvCond			= 0;
		m_bHelpWho			= 0;
		memset( m_bRangeAttack, 0, sizeof(m_bRangeAttack) );
		m_nSummProb			= 0;
		m_nSummNum			= 0;
		m_nSummID			= 0;
		m_nHelpRangeMul		= 0;
		m_tmUnitHelp		= 0;
		m_nBerserkHP		= 0;
		m_fBerserkDmgMul	= 0;
		m_nLoot				= 0;
		m_nLootProb			= 0;
		m_dwScanQuestId		= 0;
		m_dwScanItemIdx		= 0;
		m_nScanChao			= 0;
#endif // !__CORESERVER

#ifdef __S1108_BACK_END_SYSTEM
		m_fHitPoint_Rate	=
		m_fAttackPower_Rate	= 
		m_fDefence_Rate	=
		m_fExp_Rate	= 
		m_fItemDrop_Rate	= 
		m_fPenya_Rate	= 0.0F;
		m_bRate	=
#endif // __S1108_BACK_END_SYSTEM
		m_nAttackItemNear	=
		m_nAttackItemFar	=
		m_nAttackItem1	=
		m_nAttackItem2	=
		m_nAttackItem3	=
		m_nAttackItem4	=
		m_nAttackItemSec	=
		m_nMagicReflection	=
		m_nImmortality	=
		m_bBlow	=
		m_nChangeTargetRand	=
		m_nAttackFirstRange	= 0;
	}
};

typedef	struct	tagBeginItem
{
	DWORD	dwItemType;
	DWORD	dwItemId;
	DWORD	dwNum;
}
BEGINITEM, *LPBEGINITEM; 

#define MAX_BEGINEQUIP  10
#define MAX_BEGINITEM   42 //20 
#define MAX_JOBITEM     10 

typedef struct tagJOBITEM
{
	DWORD adwMale[ MAX_BEGINEQUIP ][ 2 ];
	BEGINITEM beginItem[ MAX_BEGINITEM ];

} JOBITEM,* LPJOBITEM;

#define	MAX_BEGINPOS	16

typedef	struct tagBEGINPOSARR
{
	u_long	uSize;
	D3DXVECTOR3 avPos[MAX_BEGINPOS];
}
BEGINPOSARR, *LPBEGINPOSARR;

#define TASKBAR_TOP    0
#define TASKBAR_BOTTOM 1
#define TASKBAR_LEFT   2
#define TASKBAR_RIGHT  3

// short cut
#define MAX_SLOT_APPLET			18

#ifdef __NEW_TASKBAR_V19
#define MAX_SLOT_ITEM	10
#define MAX_SLOT_QUEUE			6
#else
#define MAX_SLOT_ITEM	9
#define MAX_SLOT_QUEUE			5
#endif//__NEW_TASKBAR_V19


#if __VER >= 11 // __CSC_VER11_5
#define	MAX_SLOT_ITEM_COUNT		8
#else
#define	MAX_SLOT_ITEM_COUNT		4
#endif //__CSC_VER11_5


#define	SHORTCUT_NONE    0
#define	SHORTCUT_ETC     1
#define	SHORTCUT_APPLET  2
#define	SHORTCUT_MOTION  3
#define	SHORTCUT_SCRIPT  4
#define	SHORTCUT_ITEM    5
//#define	SHORTCUT_CARD    5
//#define	SHORTCUT_JACKBOX 6
#define	SHORTCUT_SKILL   6
#define SHORTCUT_OBJECT  7
#define SHORTCUT_CHAT    8
#define SHORTCUT_SKILLFUN 9
#define SHORTCUT_EMOTICON 10
#define SHORTCUT_LORDSKILL	 11


#if defined ( __CLIENT )
class CWndBase;
#endif	// __CLIENT

typedef struct tagSHORTCUT
{
#if defined ( __CLIENT )
	CWndBase* m_pFromWnd   ;
	CTexture* m_pTexture   ;
#if __VER >= 8	// __JEFF_VER_8
	DWORD	m_dwItemId;
#endif	// __JEFF_VER_8
#endif	// __CLIENT
	DWORD     m_dwShortcut ; 
	DWORD     m_dwId       ; 
	DWORD     m_dwType     ; 
	DWORD     m_dwIndex    ; 
	DWORD     m_dwUserId   ; 
	DWORD     m_dwData     ; 
	TCHAR     m_szString[MAX_SHORTCUT_STRING]; // SHORTCUT_CHAT일 경우 저장.
	BOOL IsEmpty() { return m_dwShortcut == SHORTCUT_NONE; }
	void Empty() { m_dwShortcut = SHORTCUT_NONE; }
} SHORTCUT,* LPSHORTCUT;

typedef struct tagEXPPARTY
{
	DWORD	Exp;
	DWORD	Point;
}
EXPPARTY, *LPEXPPARTY;

typedef struct tagRENEWEVENT
{
	int		nLevel;
	DWORD	dwItemId;
	TCHAR	strItemName[64];
	float	fPercent;
}
RENEWEVENT, *LPRENEWEVENT;

typedef struct tagRENEWEVENTITEM
{
#ifdef __S0517
	CTime	EndTime;
#else // __S0517
	TCHAR strTime[32];
#endif // __S0517
	int nLevel;
	int	nRealitem[ MAX_REALITEM ];
	int nRealitemCount[ MAX_REALITEM ];
	int nSendRealitemCount[ MAX_REALITEM ];
	int nPercent[ MAX_REALITEM ];
	int nNextIndex;
}
RENEWEVENTITEM, *LPRENEWEVENTITEM;


typedef struct tagExpCharacter
{
	EXPINTEGER	nExp1;
	EXPINTEGER	nExp2;
	DWORD dwLPPoint;
	EXPINTEGER	nLimitExp;
}
EXPCHARACTER, *LPEXPCHARACTER;

#ifdef __TREASUREBOX
#ifdef __WORLDSERVER
struct TreasureItem
{
	DWORD dwKeyID;
	DWORD dwItemID;
	int nChance;
	int nMaxamount;
	int nRepeat;
	int nLimitamount;
	BOOL bNotice;
};
#endif//__WORLDSERVER
struct TreasureBox
{
	DWORD dwBoxID;
	DWORD dwKeyID[3];
};
#endif//__TREASUREBOX

#endif	// __PROJECTCMN_H__