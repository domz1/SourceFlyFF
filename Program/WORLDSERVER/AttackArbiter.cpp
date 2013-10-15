#include "stdafx.h"
#include "DefineObj.h"
#include "defineSkill.h"
#include "ActionMover.h"
#include "User.h"
#include "defineitem.h"

#if __VER >= 19
#include "defineItemGrade.h"
#include "defineItemType.h"
#endif//__VER >= 19

#include "AttackArbiter.h"
#include "guild.h"
extern	CGuildMng			g_GuildMng;

extern	CUserMng			g_UserMng;
#include "party.h"
extern	CPartyMng	g_PartyMng;

#include "dpdatabaseclient.h"
extern	CDPDatabaseClient	g_dpDBClient;

#ifdef __EVENT_MONSTER
#include "EventMonster.h"
#endif // __EVENT_MONSTER

#if __VER >= 12 // __NEW_ITEMCREATEMON_SERVER
#include "CreateMonster.h"
#endif // __NEW_ITEMCREATEMON_SERVER

//////////////////////////////////////////////////////////////////////
// ATTACK_INFO
//////////////////////////////////////////////////////////////////////

ATK_TYPE ATTACK_INFO::GetAtkType() const
{
	ATK_TYPE type = ATK_GENERIC;

	if( dwAtkFlags & AF_MELEESKILL )
	{
		type = ATK_MELEESKILL;
	}
	else if( dwAtkFlags & AF_MAGICSKILL )
	{
		type = ATK_MAGICSKILL;
	}	
	else if( dwAtkFlags & AF_MAGIC )		// 일반 완드 공격
	{
		type = ATK_MAGIC;
	}
	else if( dwAtkFlags & AF_FORCE )		// 반사 데미지같은 경우 
	{
		type = ATK_FORCE;
	}

	return type;
}

// 방어력 무시의 경우인가?
BOOL ATTACK_INFO::CanIgnoreDEF() const		
{
	int nSkill = GetSkill();
	switch ( nSkill )
	{
	case SI_BIL_PST_ASALRAALAIKUM :
		return TRUE;
	case SI_JST_YOYO_HITOFPENYA :
		return TRUE;
	}

	if( dwAtkFlags & AF_FORCE )		// 미리계산 된 데미지의 경우, 방어력 무시 (예: 반사, 사이킥 월)  
		return TRUE;
	return FALSE;
}

int ATTACK_INFO::GetChargeLevel() const 
{
	if( IsSkillAttack( dwAtkFlags ) )
		return 0;
	
	return ( nParam & 0xFF);			// 완드충전량
}

int ATTACK_INFO::GetSkillLevel() const 
{
	ASSERT( IsSkillAttack( dwAtkFlags ) );
	return ( nParam & 0xFF);	
}

int ATTACK_INFO::GetAttackCount() const 
{
	return ( nParam >> 8) & 0xFF;		// 일반 0, 지속데미지 1이상 
}

int ATTACK_INFO::GetSkill() const
{
	if( IsSkillAttack( dwAtkFlags ) )
		return ( nParam >> 16) & 0xFFFF;	// 스킬이었다면 스킬ID
	else
		return 0;
}

BOOL ATTACK_INFO::IsRangeAttack() const
{
	return ( dwAtkFlags & AF_RANGE );
}

//////////////////////////////////////////////////////////////////////
// CAttackArbiter
//////////////////////////////////////////////////////////////////////

CAttackArbiter::CAttackArbiter( DWORD dwMsg, CMover* pAttacker, CMover* pDefender, DWORD dwAtkFlags, int nParam, BOOL bTarget, int nReflect )
		: m_dwMsg( dwMsg ), 
		  m_pAttacker( pAttacker ), 
		  m_pDefender( pDefender ), 
		  m_dwAtkFlags( dwAtkFlags ), 
		  m_nParam( nParam ),
		  m_bTarget( bTarget ),
		  m_nReflect( nReflect )		  
{
#if __VER >= 11 // __REFLECTDMG_AFTER
	m_nReflectDmg = 0;
#endif // __REFLECTDMG_AFTER
}

CAttackArbiter::~CAttackArbiter()
{
}

int CAttackArbiter::OnDamageMsgW()
{
	if( !CheckValidDamageMsg() )
	{
/*	불필요한 코드
#if __VER >= 8 // __S8_PK
		if( m_pAttacker->IsPlayer() && m_pDefender->IsPlayer() )
		{
			HITTYPE type = m_pAttacker->GetHitType( m_pDefender, m_bTarget, m_nReflect );
			if( type == HITTYPE_PK )
			{
				if( m_nReflect )
					return 0;
				
				if( m_pAttacker->IsChaotic() || m_pDefender->IsChaotic() )
					return 0;
#ifdef __JEFF_11_4
				CWorld* pWorld	= m_pDefender->GetWorld();
				if( pWorld && pWorld->IsArena() )
					return 0;
#endif	// __JEFF_11_4

				if( m_pDefender->IsPKPink() )
					m_pAttacker->SetPKPink( GetTickCount() + SEC(prj.m_PKSetting.nReadyAttack) );
				else
					m_pAttacker->SetPKPink( GetTickCount() + SEC(prj.m_PKSetting.nGeneralAttack) );
				g_UserMng.AddPKPink( m_pAttacker, 1 );
			}
		}
#endif // __VER >= 8 __S8_PK
*/
		return 0;
	}
	ATTACK_INFO info;
	info.dwAtkFlags = m_dwAtkFlags;
	info.pAttacker  = m_pAttacker;
	info.pDefender  = m_pDefender;
	info.nParam     = m_nParam;
	info.nParts     = PARTS_RWEAPON;

	int nDamage = 0;
	BYTE cbHandFlag	= GetHandFlag();
	for( BYTE cbFlag = 0x01; cbFlag <= 0x02; ++cbFlag )	
	{
		if( cbHandFlag & cbFlag )
		{
			info.nParts = (cbFlag & 0x01) ? PARTS_RWEAPON : PARTS_LWEAPON;
#ifdef __EVENTLUA_SPAWN
			if( m_pDefender->IsNPC() && prj.m_EventLua.IsEventSpawnMonster( m_pDefender->GetIndex() ) && !m_pAttacker->IsMode( ONEKILL_MODE ) )
			{
				nDamage = 1;
				continue;
			}
#endif // __EVENTLUA_SPAWN
#if __VER < 11 // __REMOVE_ENDURANCE
			m_pDefender->Abrade( m_pAttacker, info.nParts );
#endif // __REMOVE_ENDURANCE
			int n = CalcDamage( &info );					// 데미지를 구한다. (m_dwAtkFlags도 변경될 수 있다) 
			if( n > 0 )
			{
				ProcessAbnormal( n, &info );				// 스턴, 암흑, 독, 반사 상태처리 
#if __VER >= 9 // __ULTIMATE
				StealHP( n, info.GetAtkType() );								// 흡혈 
#else // __ULTIMATE
				StealHP( n );								// 흡혈 
#endif // __ULTIMATE
				nDamage += n;
			}
		}
	}

#if __VER >= 8 // __GUILDCOMBAT_85
	if( m_pAttacker->IsPlayer() && m_pDefender->IsPlayer() )
	{
		nDamage -= (nDamage * m_pDefender->GetParam( DST_PVP_DMG_RATE, 0) / 100);
#if __VER >= 9 // __CHAO_DMGDEC
		if( !m_pAttacker->IsGuildCombatTarget( m_pDefender ) )
		{
			int nDamageDecRate = m_pAttacker->GetPKValue() * 5;
			if( nDamageDecRate > 99 )
				nDamageDecRate = 99;
			nDamage -= (int)( nDamage * ( (float)nDamageDecRate/(float)100) );
		}
#endif // __CHAO_DMGDEC
	}
#endif // __VER >= 8

	nDamage = max( nDamage, 1 );
	int nHP = MinusHP( &nDamage );							// HP 감소 시킴 

#if __VER >= 14 // __INSTANCE_DUNGEON
	if( CMonsterSkill::GetInstance()->MonsterTransform( m_pDefender, nHP ) )
		return 0;
#endif // __INSTANCE_DUNGEON

	if( m_pDefender->m_pActMover->IsSit() )					// 앉기해제 & 이동을 멈춤 
		m_pDefender->m_pActMover->ResetState( OBJSTA_MOVE_ALL );
	m_pDefender->m_pActMover->SendActMsg( OBJMSG_STAND );	
	
	m_pDefender->OnAttacked( m_pAttacker, nDamage, m_bTarget, m_nReflect );		// 공격자 피격자 저장, 경험치 지급율 변경	
	if( nHP > 0 )
	{
		OnDamaged( nDamage );								// 데미지 모션 취하기, 데미지 전송, 범죄자설정 
		// MinusHP()에서 가져옴 - 데미지 이후 회복시켜야 한다.
		int nActionHPRate = m_pDefender->GetAdjParam( DST_AUTOHP );							// 발동 HP율
		if( nActionHPRate > 0 )
		{
			float fHPPercent = (float)nHP / m_pDefender->GetMaxHitPoint();					// 현재 HP의 퍼센트 율
			if( fHPPercent <= (nActionHPRate / 100.0f) )									// 현재 HP가 발동HP이하가 되면
			{
				float fAddHP = m_pDefender->GetChgParam( DST_AUTOHP ) / 100.0f;				// 회복시킬 HP율
				int nAddHP = (int)( m_pDefender->GetMaxHitPoint() * fAddHP );				// MaxHP의 일정비율을 취함.
				nHP += nAddHP;
				m_pDefender->SetPointParam( DST_HP, nHP, TRUE );									// HP증가.
				g_UserMng.AddCreateSfxObj( m_pDefender, XI_GEN_CURE01 );					// 회복 효과 발생.
				m_pDefender->RemoveBuff( BUFF_SKILL, SI_ASS_HEAL_PREVENTION );	// 해제.
			}
		}
#if __VER >= 11 // __REFLECTDMG_AFTER
		if( m_nReflectDmg )
		{
			m_pAttacker->m_pActMover->SendDamage( AF_FORCE, m_pDefender->GetId(), m_nReflectDmg, TRUE, 2 );
			m_nReflectDmg = 0;
		}
#endif// __REFLECTDMG_AFTER
	}
	else	
		OnDied();											// 경험치, 아이템 드랍 
	

#if __VER >= 8   // 8차 듀얼존에 관계없이 PVP가능하게함   Neuz, World
	m_pDefender->EndPVP(nHP);
#else	//__VER >= 8
	m_pDefender->m_nHitPoint = nHP;
#endif	//__VER >= 8

	return nDamage;
}

// 데미지를 입기 위한 조건을 충족하는가? TRUE - 충족 
BOOL CAttackArbiter::CheckValidDamageMsg()
{
#ifdef __EVENT_MONSTER
	// 이벤트 몬스터이고 레벨이 15넘게 차이나면 MISS
	if( m_pAttacker->IsPlayer() && !m_pDefender->IsPlayer() )
	{
		MoverProp* lpMoverProp = m_pDefender->GetProp();
		if( lpMoverProp && CEventMonster::GetInstance()->SetEventMonster( lpMoverProp->dwID ) )
			if( m_pAttacker->GetLevel() - m_pDefender->GetLevel() >= CEventMonster::GetInstance()->GetLevelGap() )
				m_dwAtkFlags = AF_MISS;
	}
#endif // __EVENT_MONSTER

#if __VER >= 12 // __NEW_ITEMCREATEMON_SERVER
	if( m_pAttacker->IsPlayer() && !CCreateMonster::GetInstance()->IsAttackAble( static_cast<CUser*>(m_pAttacker), m_pDefender, TRUE ) )
	{
		m_dwAtkFlags = AF_MISS;
		m_pDefender->m_idAttacker = NULL_ID;
		m_pDefender->m_idTargeter = NULL_ID;
	}
#endif // __NEW_ITEMCREATEMON_SERVER

	if( m_dwAtkFlags & AF_MISS ) 
	{
		g_UserMng.AddDamage( m_pDefender, GETID( m_pAttacker ), 0, m_dwAtkFlags );
		return FALSE;
	}

	// 맞는시점에도 스틸체크를 해야한다.
	// 동시에 스킬을 썼을때 발동하는시점에 스킬을 사용하기땜에 둘다 스틸상태가 아닌채로 들어오기 때문이다.
	// 발동하는 시점에도 역시 스틸체크를 해야하는것은 내몹을 딴놈이 공격하는 모션이 아예 나오지 않기 위해서 미리 체크 해야하기때문.
	if( !g_eLocal.GetState( EVE_STEAL ) )
	{
		if( m_pAttacker->IsSteal( m_pDefender ) )	// m_pAttacker가 스틸하려 하는가?
			return FALSE;					
	}
	return TRUE;
}

int CAttackArbiter::CalcATK( ATTACK_INFO* pInfo )
{
	int nATK = 0;
	int nCount = pInfo->GetAttackCount();		// 일반 0, 지속데미지 1이상 

	switch( pInfo->GetAtkType() )
	{
	case ATK_FORCE:								// 미리계산된 데미지 (반사, 사이킥 월)
		nATK = m_nParam;						
		nCount = 0;		
		break;
	case ATK_MELEESKILL:						// 밀리스킬 
		nATK = m_pAttacker->GetMeleeSkillPower( pInfo );
		break;
	case ATK_MAGICSKILL:						// 매직 스킬 
		nATK = m_pAttacker->GetMagicSkillPower( pInfo );
		break;
	case ATK_MAGIC:								// 완드 공격 
		nATK = m_pAttacker->GetMagicHitPower( pInfo->GetChargeLevel() );
		break;
	case ATK_GENERIC:							// 일반 공격 
		nATK = m_pAttacker->GetHitPower( pInfo );
		break;
	}

	nATK	= (int)( nATK * m_pAttacker->GetATKMultiplier( m_pDefender, pInfo->dwAtkFlags ) );	// 순수공격력을 증폭한다. 	
	if( nCount > 0 )															// 지속데미지는 계산된 ATK 10%
		nATK	= (int)( nATK * 0.1f );

//#ifdef __PET_0410
	nATK	+= m_pAttacker->GetParam( DST_ATKPOWER, 0 );
//#endif	// __PET_0410

#if __VER >= 9 // __EVENTLUA_ATKDEF
	if( m_pAttacker->IsPlayer() )
		nATK += prj.m_EventLua.GetAttackPower();
#endif // __EVENTLUA_ATKDEF

#ifdef __JEFF_11
	if( nATK < 0 )
		nATK	= 0;
#endif	// __JEFF_11

	return nATK;
}


// 데미지를 구한다. (m_dwAtkFlags도 변경될 수 있다)
int CAttackArbiter::CalcDamage( ATTACK_INFO* pInfo )
{
	if( m_pAttacker->m_dwMode & ONEKILL_MODE )	// 원킬 모드
		return m_pDefender->GetHitPoint();	

	int nDamage = PostCalcDamage( CalcATK( pInfo ), pInfo );
	m_dwAtkFlags = pInfo->dwAtkFlags;

	return nDamage;
}

int CAttackArbiter::OnAfterDamage( ATTACK_INFO* pInfo, int nDamage )
{
	CMover* pAttacker = pInfo->pAttacker;
	CMover* pDefender = pInfo->pDefender;

	if( pAttacker->IsPlayer() && nDamage > 0 )
	{
		if( pDefender->GetAdjParam( DST_CHRSTATE ) & CHS_DOUBLE )	// 더블데미지 상태 걸렸다
			pDefender->RemoveChrStateBuffs( CHS_DOUBLE );
	}

	if( pDefender->GetAdjParam( DST_CHRSTATE) & CHS_ATK_COUNTERATTACK  ) // 공격당했을때 : 데미지 없음
	{
		if( pDefender->IsRangeObj( pAttacker->GetPos(), 3.0f ) )
		{
			pDefender->RemoveChrStateBuffs( CHS_ATK_COUNTERATTACK );

			int nLevel = 1;
			if( pDefender->IsPlayer() )
			{
				LPSKILL pSkill = pDefender->GetSkill(SI_ACR_YOYO_COUNTER);
				if( pSkill )
				{
#ifdef __SKILL0517
					nLevel	= pDefender->GetSkillLevel( pSkill );
#else	// __SKILL0517
					nLevel = pSkill->dwLevel;
#endif	// __SKILL0517
				}
			}
#if __VER >= 8 // __S8_PK
			pDefender->DoUseSkill( SI_GEN_ATK_COUNTER, nLevel, pAttacker->GetId(), SUT_NORMAL, FALSE, 0 );
#else // __VER >= 8 __S8_PK
			pDefender->DoUseSkill( SI_GEN_ATK_COUNTER, nLevel, pAttacker->GetId(), SUT_NORMAL, 0 );
#endif // __VER >= 8 __S8_PK

			nDamage = 0;
			pInfo->dwAtkFlags = AF_MISS;
		}
	}
	else
	if( pDefender->GetAdjParam( DST_CHRSTATE) & CHS_DMG_COUNTERATTACK  ) // 공격당했을때 : 데미지 있음
	{
		if( pDefender->IsRangeObj( pAttacker->GetPos(), 3.0f ) )
		{
			pDefender->RemoveChrStateBuffs( CHS_DMG_COUNTERATTACK );
#if __VER >= 8 // __S8_PK
			pDefender->DoUseSkill( SI_GEN_ATK_COUNTER, pInfo->GetSkillLevel(), pAttacker->GetId(), SUT_NORMAL, FALSE, 0 );	
#else // __VER >= 8 __S8_PK
			pDefender->DoUseSkill( SI_GEN_ATK_COUNTER, pInfo->GetSkillLevel(), pAttacker->GetId(), SUT_NORMAL, 0 );	
#endif // __VER >= 8 __S8_PK
		}
	}

	if( pInfo->GetSkill() == SI_JST_YOYO_HITOFPENYA && nDamage > 0 )
	{
		nDamage = min( pAttacker->GetGold(), nDamage );
		pAttacker->AddGold( -nDamage );
	}

#if __VER >= 9	//__AI_0509
	if( pDefender->IsReturnToBegin() )
	{
		nDamage = 0;
		pInfo->dwAtkFlags = AF_MISS;
	}
#endif	// __AI_0509

	return nDamage;
}

enum POSTCALC_TYPE
{
	POSTCALC_DPC,				// Defense, Parry, Critical 적용
	POSTCALC_MAGICSKILL,		// 속성공격 포함 매직 스킬  
	POSTCALC_GENERIC,			// 근접공격  
};

// 공격계산 타입
POSTCALC_TYPE GetPostCalcType( DWORD dwAtkFlags )
{
	if ( dwAtkFlags & AF_MAGICSKILL )		// 매직 스킬
		return POSTCALC_MAGICSKILL;

	if ( dwAtkFlags & AF_GENERIC )			// 일반 공격
		return POSTCALC_GENERIC;
	
	return POSTCALC_DPC;
}

int CAttackArbiter::PostCalcDamage( int nATK, ATTACK_INFO* pInfo )
{
	if ( pInfo->dwAtkFlags & AF_FORCE )		// 
		return nATK;

	CMover* pAttacker = pInfo->pAttacker;
	CMover* pDefender = pInfo->pDefender;

	// 예외처리: 매직스킬이 아니면, 몬스터가 플레이어를 공격할 때 레벨차에 따른 공격력UP
	if( ((pInfo->dwAtkFlags & AF_MAGICSKILL) == 0) && pAttacker->IsNPC() && pDefender->IsPlayer() )	
	{
		int nDelta = pAttacker->GetLevel() - pDefender->GetLevel();
		if( nDelta > 0 ) 
		{
			float fpower = 1.0f + ( 0.05f * nDelta );
			nATK	= (int)( nATK * fpower );
		}
	}

	int nDamage = nATK;
	switch( GetPostCalcType( pInfo->dwAtkFlags ) )
	{
	case POSTCALC_DPC:
		nDamage = pDefender->ApplyDPC( nATK, pInfo );	// Defense, Parry, Critical 적용
#if __VER < 13 // __EXT_ENCHANT
		if( nATK > 0 )
			nDamage += pAttacker->CalcPropDamage( pDefender, pInfo->dwAtkFlags );		// 속성데미지를 추가
#endif // __EXT_ENCHANT
		break;
	case POSTCALC_MAGICSKILL:
		nDamage = pAttacker->PostCalcMagicSkill( nATK, pInfo );
		break;
	case POSTCALC_GENERIC:
		nDamage = pAttacker->PostCalcGeneric( nATK, pInfo );
		break;
	}

	if( nDamage <= 0 )
		return 0;

	nDamage += pAttacker->CalcLinkAttackDamage( nDamage );						// 링크어택 스킬을 적용한다.
	if( pInfo->GetSkill() == SI_BIL_PST_ASALRAALAIKUM )
		nDamage += PostAsalraalaikum();

	nDamage	= (int)( nDamage * pAttacker->GetDamageMultiplier( pInfo ) );		// 데미지증폭 
#ifdef __3RD_LEGEND16
  if( pDefender->IsPlayer() && pAttacker->IsPlayer() )
  {
    int nImmunity = pDefender->GetParam( DST_IGNORE_DMG_PVP, 0 );
    if( nImmunity > 0 )
    {
      if( ( nImmunity - nDamage ) <= 0 )
      {
        nDamage -= nImmunity;
        pDefender->RemoveBuff( BUFF_SKILL, SI_FLO_SUP_ABSOLUTE );
      }
      else
      {
        pDefender->ResetDestParam( DST_IGNORE_DMG_PVP, nDamage, TRUE );
        nDamage = 0;
      }
    }
  }
#endif // __3RD_LEGEND16
	nDamage = OnAfterDamage( pInfo, nDamage );
	return nDamage;
}	

int CAttackArbiter::PostAsalraalaikum()
{
	int nAddDmg = 0;

	LPSKILL pSkill = m_pAttacker->GetSkill( SI_BIL_PST_ASALRAALAIKUM );
	DWORD dwSkillLevel = 
#ifdef __SKILL0517
		m_pAttacker->GetSkillLevel( pSkill );
#else	// __SKILL0517
		pSkill ? pSkill->dwLevel : 0;
#endif	// __SKILL0517

#if __VER >= 12 // __MONSTER_SKILL
	if( m_pAttacker->IsNPC() )
		dwSkillLevel = CMonsterSkill::GetInstance()->GetMonsterSkillLevel( m_pAttacker, SI_BIL_PST_ASALRAALAIKUM );
#endif // __MONSTERSKILL

	switch( dwSkillLevel )
	{
		case 1:		nAddDmg = 20;	break;
		case 2:		nAddDmg = 30;	break;
		case 3:		nAddDmg = 40;	break;
		case 4:		nAddDmg = 50;	break;
		case 5:		nAddDmg = 60;	break;
		case 6:		nAddDmg = 70;	break;
		case 7:		nAddDmg = 80;	break;
		case 8:		nAddDmg = 90;	break;
		case 9:		nAddDmg = 100;	break;
		case 10:	nAddDmg = 150;	break;
	}

	int nMP = m_pAttacker->GetManaPoint();						// 일단 받아놓고
	if( m_pAttacker->IsPlayer() && m_pAttacker->IsSMMode( SM_MAINTAIN_MP ) == FALSE )		// MP 유료아이템 먹었을때는 MP닳면 안된다.
		m_pAttacker->SetPointParam( DST_MP, 0 );				// 0으로 만듬.
	return ( ( ( m_pAttacker->GetStr() / 10 ) * dwSkillLevel ) * ( 5 + nMP / 10 ) + nAddDmg );
}

//
//
void CAttackArbiter::ChanceSkill( int nDestParam, CCtrl *pAttackCtrl, CCtrl *pDefendCtrl )
{
#ifdef _DEBUG
	if( nDestParam == DST_CHR_CHANCESTEALHP )
	{
		int a = 0;
	}
#endif
	if( m_pAttacker->GetType() != OT_MOVER )	return;
	if( m_pDefender->GetType() != OT_MOVER )	return;
	
	CMover *pAttacker = (CMover *)pAttackCtrl;
	CMover *pDefender = (CMover *)pDefendCtrl;

	int nRate = pAttacker->GetAdjParam( nDestParam );
	if( nRate > 0 )	// 공격자가 스킬발동상태
	{
		if( (int)( xRandom(100) ) <= nRate )
		{
			DWORD dwAddSkill = pAttacker->GetChgParam( nDestParam );		// 타격시 발동될 애드스킬
			
			AddSkillProp *pAddSkillProp;
			pAddSkillProp = prj.GetAddSkillProp( dwAddSkill );
			if( pAddSkillProp )
				pAttacker->DoActiveSkill( pAddSkillProp->dwName, pAddSkillProp->dwSkillLvl, pDefender );		// dwActiveSkill Lv1이 발동됨.
			//				m_pDefender->SetPoison( TRUE, m_pAttacker->GetId(), m_pAttacker->GetChgParam( DST_CHR_CHANCEPOISON ), m_pAttacker->m_tmPoisonUnit, m_pAttacker->m_wPoisonDamage );
		}
		
	}
}


// 상태이상(반사, 스턴, 암흑, 독)처리
void CAttackArbiter::ProcessAbnormal( int nDamage, ATTACK_INFO* pInfo )
{
	int	nSkill = pInfo->GetSkill();

	//방어자가 반사상태이면?  공격자도 데미지를 입는다.
	int nReflectDmgRate = m_pDefender->GetAdjParam( DST_REFLECT_DAMAGE );
	if( nReflectDmgRate > 0 )
	{
		int nReflectRate = m_pDefender->GetChgParam( DST_REFLECT_DAMAGE );	

		BOOL bAble = TRUE;
		if( nReflectRate > 0 )			// 반사시킬 확률이 있을땐 확률검사 함
			if( (int)( xRandom(100) ) > nReflectRate )	// 확률 실패냐?
				bAble = FALSE;

		ATK_TYPE atkType = pInfo->GetAtkType();
		switch( atkType )
		{
		case ATK_MELEESKILL:
		case ATK_MAGICSKILL:
			if( m_pDefender->HasBuff( BUFF_SKILL, SI_PSY_NLG_CRUCIOSPELL ) )		// 방어자가 크루시오 스펠일 경우
				bAble = FALSE;
#if __VER >= 11 // __REFLECTDMG_AFTER
			break;
#endif // __REFLECTDMG_AFTER
		case ATK_FORCE:															// 미리계산된 데미지
			bAble = FALSE;
			break;
		}

		if( bAble )
		{
			int nDmg = (int)( nDamage * (float)(nReflectDmgRate / 100.0f) );
#if __VER >= 8 // __S8_PVP_DST_REFLECT_DAMAGE
			if( m_pAttacker->IsPlayer() && m_pDefender->IsPlayer() )
				nDmg = (int)( nDmg * 0.1f );
#endif // __VER >= 8 // __S8_PVP_DST_REFLECT_DAMAGE
#if __VER >= 11 // __REFLECTDMG_AFTER
			m_nReflectDmg = nDmg;
#else // __REFLECTDMG_AFTER
			m_pAttacker->m_pActMover->SendDamage( AF_FORCE, 
			                                  m_pDefender->GetId(), 
			                                  nDmg, TRUE, 2 );
#endif // __REFLECTDMG_AFTER
		}
	}

	// chipi_080325 - AF_MAGICSKILL 인경우 특정 스킬에 한해 상태이상 처리
	if( (m_dwAtkFlags & AF_MAGICSKILL) && m_pAttacker->IsPlayer() )
	{
		switch( nSkill )
		{
			case SI_BIL_PST_BGVURTIALBOLD :
			case SI_BIL_PST_ASALRAALAIKUM :
				ChanceSkill( DST_CHR_CHANCESTUN,	m_pAttacker, m_pDefender );
				break;
		}
	}
		
	if( m_dwAtkFlags & (AF_GENERIC | AF_MELEESKILL) )		// 근접평타/근접스킬에 맞았다.
	{
		if( m_pAttacker->IsPlayer() )
		{
			ChanceSkill( DST_CHR_CHANCESTUN,	m_pAttacker, m_pDefender );
			ChanceSkill( DST_CHR_CHANCEPOISON,	m_pAttacker, m_pDefender );
			ChanceSkill( DST_CHR_CHANCEDARK,	m_pAttacker, m_pDefender );
			ChanceSkill( DST_CHR_CHANCEBLEEDING, m_pAttacker, m_pDefender );
			ChanceSkill( DST_CHR_CHANCESTEALHP, m_pAttacker, m_pDefender );
		}
		
		// Active Skill(마법 발동)
		{
			ItemProp *pAttackerHandItemProp = m_pAttacker->GetActiveHandItemProp();
			if( pAttackerHandItemProp )
			{
				DWORD dwActiveSkill	= pAttackerHandItemProp->dwActiveSkill;
				DWORD dwActiveSkillLevel = pAttackerHandItemProp->dwActiveSkillLv;
				if( dwActiveSkillLevel == NULL_ID )
					dwActiveSkillLevel = 1;
				if(  dwActiveSkill != NULL_ID )	// 공격자의 무기에 발동마법이 있다.
				{
					if( pAttackerHandItemProp->dwActiveSkillRate == NULL_ID ||		// = 면 100%
						(DWORD)( random(100) ) < pAttackerHandItemProp->dwActiveSkillRate )	// 발동확률에 걸렸는가.
					{
						m_pAttacker->DoActiveSkill( dwActiveSkill, dwActiveSkillLevel, m_pDefender, true );		// dwActiveSkill Lv1이 발동됨. true를 준건. 여기서 확률계산 햇으므로 내부에서는 하지마라는뜻.
						TRACE( "%s의 ActiveSkill 발동, ", m_pAttacker->GetName() );
					}
				}
			}
		}
	} // AF_GENERIC

	if( m_dwAtkFlags & ( AF_MELEESKILL | AF_MAGICSKILL ) )	// 스킬공격이 시도될때
	{
		ItemProp* pSkillProp;
		AddSkillProp *pAddSkillProp;
		m_pAttacker->GetSkillProp( &pSkillProp, &pAddSkillProp, nSkill, pInfo->GetSkillLevel(), "ProcessAbnormal" );
		DWORD dwActiveSkill = pAddSkillProp->dwActiveSkill;
		if( dwActiveSkill != NULL_ID )		// 추가 발동 스킬이 있다.
		{
			DWORD dwActiveSkillRate	= pAddSkillProp->dwActiveSkillRate;
#if __VER >= 9	// __SKILL_0706
			if( m_pAttacker->IsPlayer() && m_pDefender->IsPlayer() )
				dwActiveSkillRate	= pAddSkillProp->dwActiveSkillRatePVP;
#endif	// __SKILL_0706
			// 발동확률에 걸렸는가.
			if( dwActiveSkillRate == NULL_ID || xRandom(100) < dwActiveSkillRate )
				m_pAttacker->DoActiveSkill( dwActiveSkill, pAddSkillProp->dwSkillLvl, m_pDefender );	// 발동
		}
	}
#ifdef __BARUNA_PIERCING
	if( m_dwAtkFlags &( AF_GENERIC | AF_MELEESKILL | AF_MAGICSKILL ))
	{
		if (m_pDefender->IsPlayer() || !m_pAttacker->IsPlayer())
			return;

		CItemElem *pItemElem = m_pAttacker->GetWeaponItem();
		if( !pItemElem )
			return;
		ItemProp *pProp = pItemElem->GetProp();
		if ( !pProp )
			return;

		if( !pProp->IsBaruna() )
			return;

		for( int i = 0; i < pItemElem->GetPiercingSize(); i++ )
		{
			if( pItemElem->GetPiercingItem( i ) != 0 )
			{
				DWORD dwRuneId = pItemElem->GetPiercingItem(i);				 
				ItemProp *pRuneProp = prj.GetItemProp( dwRuneId );
				if( !pRuneProp )
					return;
				if( (int)( xRandom( 10000 ) ) < pRuneProp->dwHitActiveSkillProb )
				{
					if( pRuneProp->dwHitActiveSkillTarget == IST_ANOTHER )
						m_pAttacker->DoActiveSkill( pRuneProp->dwHitActiveSkillId, pRuneProp->dwHitActiveSkillLv, m_pDefender );
					else
					if( pRuneProp->dwHitActiveSkillTarget == IST_SELF )
						m_pAttacker->DoActiveSkill( pRuneProp->dwHitActiveSkillId, pRuneProp->dwHitActiveSkillLv, m_pAttacker );
				}
			}
		}
	}
#endif // __BARUNA_PIERCING
}

// HP 감소 시킴 
int CAttackArbiter::MinusHP( int *pnDamage )
{
	int nHP = m_pDefender->GetHitPoint() - *pnDamage;	// HP를 깎는다.
	if( nHP <= 0 )
	{
		if( m_pDefender->m_dwMode & MATCHLESS2_MODE )	// HP가 0미만으로 떨어졌는데 반무적 상태냐
			nHP = 1;									// hp를 1로 복구해줌
		else
			nHP = 0;									// 반무적 아니면 걍 죽는게 맞다.
		*pnDamage = m_pDefender->GetHitPoint() - nHP;
	}
	/*
	else	
	{
		int nActionHPRate = m_pDefender->GetAdjParam( DST_AUTOHP );							// 발동 HP율
		if( nActionHPRate > 0 )
		{
			float fHPPercent = (float)nHP / m_pDefender->GetMaxHitPoint();					// 현재 HP의 퍼센트 율
			if( fHPPercent <= (nActionHPRate / 100.0f) )									// 현재 HP가 발동HP이하가 되면
			{
				float fAddHP = m_pDefender->GetChgParam( DST_AUTOHP ) / 100.0f;				// 회복시킬 HP율
				int nAddHP = m_pDefender->GetMaxHitPoint() * fAddHP;						// MaxHP의 일정비율을 취함.
				nHP += nAddHP;
				m_pDefender->SetPointParam( DST_HP, nHP );									// HP증가.
				g_UserMng.AddCreateSfxObj( m_pDefender, XI_GEN_CURE01 );					// 회복 효과 발생.
				m_pDefender->RemoveBuff( BUFF_SKILL, SI_ASS_HEAL_PREVENTION );	// 해제.				
			}
		}
	}
	*/

	return nHP;
}

// 흡혈 
#if __VER >= 9 // __ULTIMATE
void CAttackArbiter::StealHP( int nDamage, ATK_TYPE type )
#else // __ULTIMATE
void CAttackArbiter::StealHP( int nDamage )
#endif // __ULTIMATE
{
	ItemProp *pHandItem = m_pAttacker->GetActiveHandItemProp();
	if( pHandItem )
	{
		if( pHandItem->dwDestParam1 == DST_CHR_STEALHP )					// 들고 있던 무기에 흡혈 옵션이 있으면
		{
			int nStealHP = (int)( nDamage * (pHandItem->nAdjParamVal1 / 100.0f) );	// 실제가격한 데미지의 %를 취함.
			m_pAttacker->SetDestParam( DST_HP, nStealHP, NULL_CHGPARAM );				// 공격자에게 흡혈HP를 줌.
		}
	}

#if __VER >= 9 // __ULTIMATE
	if( type == ATK_GENERIC && m_pAttacker->IsPlayer() )
	{
		int nDstHP = m_pAttacker->GetParam( DST_MELEE_STEALHP, 0 );
		int nStealHP = (int)( nDamage * (nDstHP / 100.0f) );
		if( 0 < nDstHP )
			m_pAttacker->SetDestParam( DST_HP, nStealHP, NULL_CHGPARAM );				// 공격자에게 흡혈HP를 줌.
	}
#endif // __ULTIMATE
}




// 공격당하고 데미지를 입을 경우 처리 
void CAttackArbiter::OnDamaged( int nDamage )
{
#if __VER >= 9 // __RECOVERY10
	m_pDefender->m_nAtkCnt = 1;
#endif // __RECOVERY10
	m_pDefender->SetDamagedMotion( m_pAttacker, m_dwAtkFlags );

	g_UserMng.AddDamage( m_pDefender, GETID( m_pAttacker ), nDamage, m_dwAtkFlags );
}

// 공격당하고 죽는 경우 처리 
void CAttackArbiter::OnDied()
{
#ifndef __CLIENT
#ifdef __AI_AGGRO
	if( m_pDefender->IsPlayer() )
		m_pDefender->SetAggro( 0 );
#endif
#endif

#if __VER >= 8   // 8차 듀얼존에 관계없이 PVP가능하게함   Neuz, World
	if( m_pDefender && m_pAttacker && m_pDefender->m_nDuel != 0 && m_pAttacker->m_nDuel != 0 && 
		m_pDefender->m_idDuelOther == m_pAttacker->GetId() )
	{
		OnDiedPVP();
		return;
	}
#endif	//__VER >= 8  
	m_pDefender->SubPVP( m_pAttacker, m_nReflect );					// m_pDefender가 죽고난 후 슬로터관련 처리.

	m_pAttacker->AddKillRecovery();

	OnDiedSchoolEvent();

	if( m_pDefender->IsNPC() && m_pDefender->IsDie() )	 
	{
		m_pDefender->Delete();							// 일단 땜빵... 이런경우는 걍 없애자.
		return;
	}

	if( m_pDefender->m_dwIndex == MI_CLOCKWORK1 || m_pDefender->m_dwIndex == MI_BIGMUSCLE || m_pDefender->m_dwIndex == MI_KRRR )		
		m_pDefender->SubAroundExp( m_pAttacker, 50.0f );		// 반경50미터 이내에는 다 경험치 나눠가짐.
	else
#ifdef __AI_AGGRO
		//CMover* pAttacker = m_pAttacker;
		if( m_pAttacker->HasOwner() && IsValidObjID( m_pAttacker->GetOwner() ) )
		{
			m_pAttacker = prj.GetMover( m_pAttacker->GetOwner() );
		}
		m_pAttacker->SubExperience( m_pDefender );
#else
		m_pAttacker->SubExperience( m_pDefender );		// m_pDefender를 죽이고 난후의 m_pAttacker 경험치 처리.
#endif//__AI_AGGRO
	m_pDefender->DropItemByDied( m_pAttacker );

#if __VER >= 8 // __S8_PK
#ifdef __JEFF_11_4
	BOOL bArena		= m_pDefender->GetWorld() && m_pDefender->GetWorld()->IsArena();
#endif	// __JEFF_11_4

	if( m_pDefender->IsPlayer() && m_pDefender->IsChaotic()
#ifdef __JEFF_11_4
		&& !bArena
#endif	// __JEFF_11_4
		)
	{
		DWORD dwPropensity = m_pDefender->GetPKPropensity(); // undorflow
		m_pDefender->SetPKPropensity( m_pDefender->GetPKPropensity() - xRandom( prj.m_PKSetting.nDiePropensityMin, prj.m_PKSetting.nDiePropensityMax + 1 ));
		if( dwPropensity <= m_pDefender->GetPKPropensity() )
			m_pDefender->SetPKPropensity( 0 );
		g_UserMng.AddPKPropensity( m_pDefender );
		g_dpDBClient.SendLogPkPvp( m_pDefender, m_pAttacker, 0, 'P' );
	}	
#endif // __VER >= 8 // __S8_PK

	m_pAttacker->m_nAtkCnt = 0;							// 타겟을 죽였으면 공격자의 어택카운트 클리어
	m_pDefender->DoDie( m_pAttacker, m_dwMsg );			// m_pDefender야 죽어라. 

	m_pAttacker->ClearDestObj();						// 목표를 죽였으면 이동목표도 클리어.
}

#if __VER >= 8     // 8차 듀얼존에 관계없이 PVP가능하게함   Neuz, World
// PVP공격당하고 죽는 경우 처리 
void CAttackArbiter::OnDiedPVP()
{
	if( m_pDefender->IsNPC() && m_pDefender->IsDie() )	 
	{
		m_pDefender->Delete();							// 일단 땜빵... 이런경우는 걍 없애자.
		return;
	}

	m_pDefender->SubDuel( m_pAttacker );//듀얼결과 보내기
	
	m_pDefender->SetPosChanged( TRUE );
	m_pAttacker->SetPosChanged( TRUE );

	m_pAttacker->ClearDuel();
	( (CUser*)m_pAttacker )->AddSetDuel( m_pAttacker );
	m_pDefender->ClearDuel();
	( (CUser*)m_pDefender )->AddSetDuel( m_pDefender );

	m_pDefender->AddKillRecovery();
	m_pAttacker->AddKillRecovery();

	m_pAttacker->m_nAtkCnt = 0;							//  공격자의 어택카운트 클리어
	m_pDefender->m_nAtkCnt = 0;							//  방어자의 어택카운트 클리어

	m_pDefender->DoPVPEnd( m_pAttacker, false ,m_dwMsg );			//

	( (CUser*)m_pAttacker )->AddDuelStart( m_pDefender->m_idPlayer , 3 );	// 서로 상대방에 대한 아이디만 보내주면 된다. 3:이김, 4 : 짐
	( (CUser*)m_pDefender )->AddDuelStart( m_pAttacker->m_idPlayer , 4 );


	m_pAttacker->ClearDestObj();						// 이동목표도 클리어.
	m_pDefender->ClearDestObj();						// 이동목표도 클리어.
}
#endif	// __VER >= 8  


// 스쿨이벤트 처리 
void CAttackArbiter::OnDiedSchoolEvent()
{
	if( g_eLocal.GetState( EVE_SCHOOL_BATTLE ) == SBS_START2 )	// '/sbstart2'에서만 피격
	{
		if( m_pDefender->m_dwAuthorization < AUTH_GAMEMASTER )
		{
			if( !m_pDefender->IsMode( OBSERVE_MODE ) )
			{
				CGuild* pGuild	= m_pDefender->GetGuild();
				if( pGuild )
					pGuild->m_nDead++;	
			}
			m_pDefender->SetMode( OBSERVE_MODE );
			g_UserMng.AddModifyMode( (CUser*)m_pDefender );
		}

		static EVENTITEM eventitem[3] =
		{
			II_GEN_FOO_INS_HOTDOG,		12,
			II_GEN_REF_REF_TENTH,		7,
			II_GEN_POT_DRI_VITAL900,	7,
		};

		for( int i = 0; i < 3; i++ )
		{
			CItemElem* pItemElem		= new CItemElem;
			pItemElem->m_dwItemId		= eventitem[i].dwIndex;
			pItemElem->m_nItemNum		= (short)( eventitem[i].dwPrabability );
			pItemElem->m_nHitPoint		= pItemElem->GetProp()->dwEndurance;
			pItemElem->SetSerialNumber();

			CItem* pItem		= new CItem;
			pItem->m_pItemBase	= pItemElem;
			pItem->SetIndex( D3DDEVICE, pItemElem->m_dwItemId );
			pItem->SetPos( m_pDefender->GetPos() );
			pItem->SetAngle( (float)( xRandom( 360 ) ) );
			m_pDefender->GetWorld()->ADDOBJ( pItem, TRUE, m_pDefender->GetLayer() );
		}
	}
}

BYTE CAttackArbiter::GetHandFlag( void )
{
	BYTE cbHandFlag = 0x01;				// 디폴트는 오른손이다 .

	if( m_dwAtkFlags & AF_RANGE )		// 활의 경우 무조건 오른손 
		return cbHandFlag;

	if( m_dwAtkFlags & AF_GENERIC )		// 평타일땐
	{
		cbHandFlag = m_nParam & 0x03;	// 하위 2비트를 빼오면 오른손으로 친건지 왼손으로친건지 나온다.
		if( cbHandFlag == 0 )	
			cbHandFlag = 0x01;			// 값이 없으면 디폴트는 오른손
	}

	return cbHandFlag;
}

