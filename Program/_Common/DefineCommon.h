#ifndef __COMMON_DEFINE__
#define __COMMON_DEFINE__

#include <windows.h>

//////////////////////////////////////////////////////////////////////////////
// 클라이언트 와 서버에서 공용으로 사용하는 값(constant), 형을 정의하는 파일
//////////////////////////////////////////////////////////////////////////////
const int MAX_SHORTCUT_STRING = 128;
const int MAX_GOLD_COIN = 32767;
const int MAX_GOLD_REWARD = INT_MAX;
const int MAX_INPUT_REWARD = 200000000;
const int MIN_INPUT_REWARD = 1000;
const int REQ_WANTED_GOLD = 1000;			// 현상범 위치정보 보기 돈 

const unsigned char	REMOVE_VOTE	= 0;		// 투표제거 
const unsigned char	CLOSE_VOTE	= 1;		// 투표종료 
const unsigned char	CAST_VOTE	= 2;		// 투표하기 
const int			TIMEWAIT_CLOSE = 10;	// 비 평화지역 종료대기 시간  
const	int		TIMEWAIT_RELEASE	= 3600;
const	int		TIMEWAIT_INSTANT_BANKER		= 180000;


const int MAX_CHARGE_LEVEL = 4;

extern const int OLD_MPU;
extern int g_MPU;

enum ATK_TYPE 
{
	ATK_FORCE,					// 미리계산된 데미지를 강제로 줄 경우 
	ATK_MELEESKILL,				// 밀리스킬 
	ATK_MAGICSKILL,				// 매직 스킬 
	ATK_MAGIC,					// 완드 공격 
	ATK_GENERIC,				// 일반 공격 
};

class CMover;

struct ATTACK_INFO 
{
	CMover*		pAttacker;			// 공격자 
	CMover*		pDefender;			// 방어자 
	DWORD		dwAtkFlags;			// 공격 플래그
	int			nParam;				// 공격 추가 플래그 
	int			nParts;				// 오른손인가 왼손인가 
	int			nDEFFactor;			// 방어력 팩터 

	ATK_TYPE	GetAtkType() const;
	BOOL		CanIgnoreDEF() const;
	int			GetChargeLevel() const;
	int			GetAttackCount() const;
	int			GetSkill() const;
	BOOL		IsRangeAttack() const;
	int			GetSkillLevel() const; 
};

#ifdef __CLIENT
	inline int ATTACK_INFO::GetSkill() const
	{
		return 0;
	}
#endif

#endif	// __COMMON_DEFINE__
