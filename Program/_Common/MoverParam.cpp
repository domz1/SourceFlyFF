#include "stdafx.h"
#include "defineSound.h"
#include "defineText.h"
#include "defineQuest.h"
#include "resdata.h"
#include "defineObj.h"
#ifdef __WORLDSERVER
#include "DialogMsg.h"
#include "user.h"
#include "dpcoreclient.h"
#include "spevent.h"

#if __VER >= 12 // __LORD
#ifdef __WORLDSERVER
#include "slord.h"
#endif	// __WORLDSERVER 
#ifdef __CLIENT
#include "clord.h"
#endif	// __CLIENT
#endif	// __LORD

#if __VER >= 9 // __CSC_VER9_1
#ifdef __CLIENT
#include "wndfield.h"
#endif	// __CLIENT
#endif	// __CSC_VER9_1

extern	CUserMng	g_UserMng;
extern	CDPCoreClient	g_DPCoreClient;

#include "dpdatabaseclient.h"
extern	CDPDatabaseClient	g_dpDBClient;
extern	CDPCoreClient	g_DPCoreClient;
#endif	// __WORLDSERVER

#ifdef __CLIENT			
#include "Dialogmsg.h"
#endif

#include "Party.h"
extern	CPartyMng	g_PartyMng;

#ifdef __WORLDSERVER
#include "DPSrvr.h"
extern	CDPSrvr		g_DPSrvr;
#endif // __WORLDSERVER

#include "Project.h"
#include "ProjectCmn.h"


#ifdef __WORLDSERVER

#ifdef __ENVIRONMENT_EFFECT

#include "Environment.h"

#else // __ENVIRONMENT_EFFECT

#if defined(__EVENTLUA_RAIN) || defined(__EVENTLUA_SNOW)
#include "Environment.h"
extern CEnvironment g_Environment;
#else // __EVENTLUA_RAIN || __EVENTLUA_SNOW
#ifdef __RAIN_EVENT
#include "Environment.h"
extern CEnvironment g_Environment;
#endif // __RAIN_EVENT
#endif// __EVENTLUA_RAIN || __EVENTLUA_SNOW

#endif // __ENVIRONMENT_EFFECT

#if __VER >= 14 // __PCBANG
#include "PCBang.h"
#endif // __PCBANG

#if __VER >= 15 // __GUILD_HOUSE
#include "GuildHouse.h"
#endif // __GUILD_HOUSE

#if __VER >= 15 // __CAMPUS
#include "CampusHelper.h"
#endif // __CAMPUS
#endif // __WORLDSERVER

LPSKILL CMover::GetSkill( int nType, int nIdx )
{
	// nType == 0이면 직업 스킬
	// nType == 1이면 라이선스 스킬 
	if( nIdx >= MAX_SKILL_JOB )
	{
		Error( "CMover::GetSkill : %s nId = %d 범위를 벗어남", m_szName, nIdx );
		nIdx = 0;
	}
	return &m_aJobSkill[ nIdx ];
}
LPSKILL CMover::GetSkill( DWORD dwSkill )
{
	for( int i = 0; i < MAX_SKILL_JOB; i++ )	
	{
		if( m_aJobSkill[i].dwSkill == dwSkill )
			return &m_aJobSkill[i];
	}
	return NULL;
}

int		CMover::GetSkillIdx( DWORD dwSkill )
{
	for( int i = 0; i < MAX_SKILL_JOB; i++ )	
	{
		if( m_aJobSkill[i].dwSkill == dwSkill )
			return i;
	}
	return -1;
}


BOOL CMover::CheckSkill( DWORD dwSkill )
{
	ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
	
	if( pSkillProp == NULL || pSkillProp->nLog == 1 )
		return FALSE;
#if __VER >= 10 // __LEGEND	//	9차 전승시스템	Neuz, World, Trans
#ifdef __3RD_LEGEND16
	if( !IsMaster() && !IsHero() && !IsLegendHero() && GetLevel() < (int)( pSkillProp->dwReqDisLV ) )
#else // __3RD_LEGEND16
	if( !IsMaster() && !IsHero() && GetLevel() < (int)( pSkillProp->dwReqDisLV ) )
#endif // __3RD_LEGEND16
		return FALSE;
#else //__LEGEND	//	9차 전승시스템	Neuz, World, Trans
	if( GetLevel() < pSkillProp->dwReqDisLV )
		return FALSE;
#endif	//__LEGEND	//	9차 전승시스템	Neuz, World, Trans
				
	if( pSkillProp->dwReSkill1 != 0xffffffff )
	{
		LPSKILL pSkillBuf = GetSkill( pSkillProp->dwReSkill1 );
		if( pSkillBuf )
		{
			if( pSkillBuf->dwLevel < pSkillProp->dwReSkillLevel1 )
			{
				return FALSE;
			}
		}
	}
	
	if( pSkillProp->dwReSkill2 != 0xffffffff )
	{
		LPSKILL pSkillBuf = GetSkill( pSkillProp->dwReSkill2 );
		if( pSkillBuf )
		{
			if( pSkillBuf->dwLevel < pSkillProp->dwReSkillLevel2 )
			{
				return FALSE;
			}				
		}
	}	
	
	return TRUE;
}


// 케릭터가 레벨업시 스킬중에 케릭터레벨땜시 활성화 안된것들 활성여부 체크하여 정보출력
void CMover::PutLvUpSkillName_1( DWORD dwLevel )
{
#ifdef __CLIENT
	for( int i = 0; i < MAX_SKILL_JOB; i++ )	
	{
		if( m_aJobSkill[i].dwSkill == NULL_ID ) 
			continue;
		
		ItemProp* pSkillProp = prj.GetSkillProp( m_aJobSkill[i].dwSkill );
		
		if( pSkillProp && dwLevel == pSkillProp->dwReqDisLV )
		{
			if( pSkillProp->dwReSkill1 != -1 )
			{
				LPSKILL pSkill = GetSkill(pSkillProp->dwReSkill1);

				if( pSkillProp->dwReSkillLevel1 > pSkill->dwLevel )
					continue;
			}
			if( pSkillProp->dwReSkill2 != -1 )
			{
				LPSKILL pSkill = GetSkill(pSkillProp->dwReSkill2);
				
				if( pSkillProp->dwReSkillLevel2 > pSkill->dwLevel )
					continue;
			}

			CString str;
			str.Format( prj.GetText( TID_GAME_REAPSKILL ), pSkillProp->szName );
			g_WndMng.PutString( (LPCTSTR)str, NULL, prj.GetTextColor( TID_GAME_REAPSKILL ) );
		}
	}
#endif //__CLIENT
}		

BOOL IsSkillParent( CMover* pMover, DWORD dwSkill, DWORD dwChildSkill )
{
	LPSKILL pSkill = pMover->GetSkill( dwSkill );
	if( pSkill == NULL )
		return FALSE;

	ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
	if( pSkillProp == NULL )
		return FALSE;

	if( pSkillProp->dwReSkill1 == dwChildSkill || pSkillProp->dwReSkill2 == dwChildSkill )
		return TRUE;

	return FALSE;
}

BOOL IsActive( CMover* pMover, DWORD dwSkill )
{
	LPSKILL pSkill = pMover->GetSkill( dwSkill );
	if( pSkill == NULL )
		return FALSE;

	ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
	if( pSkillProp == NULL )
		return FALSE;

	if( pSkillProp->dwReSkill1 != NULL_ID )
	{
		LPSKILL pSkill1;
		ItemProp* pSkillProp1;

		pSkill1		= pMover->GetSkill( pSkillProp->dwReSkill1 );
		if( pSkill1 == NULL )
			return FALSE;

		pSkillProp1 = prj.GetSkillProp( pSkill1->dwSkill );
		if( pSkillProp1 == NULL )
			return FALSE;

#if __VER >= 10 // __LEGEND	//	9차 전승시스템	Neuz, World, Trans
#ifdef __3RD_LEGEND16
		if( (int)( pSkillProp1->dwReqDisLV ) > pMover->GetLevel() && !pMover->IsMaster() && !pMover->IsHero() && !pMover->IsLegendHero() )
#else // __3RD_LEGEND16
		if( (int)( pSkillProp1->dwReqDisLV ) > pMover->GetLevel() && !pMover->IsMaster() && !pMover->IsHero() )
#endif // __3RD_LEGEND16
			return FALSE;
#else //__LEGEND	//	9차 전승시스템	Neuz, World, Trans
		if( pSkillProp1->dwReqDisLV > pMover->GetLevel() )
			return FALSE;
#endif	//__LEGEND	//	9차 전승시스템	Neuz, World, Trans
		
		if( pSkillProp->dwReSkillLevel1 != pSkill1->dwLevel )
			return FALSE;
	}

	if( pSkillProp->dwReSkill2 != NULL_ID )
	{
		LPSKILL pSkill1;
		ItemProp* pSkillProp1;
		
		pSkill1		= pMover->GetSkill( pSkillProp->dwReSkill2 );
		if( pSkill1 == NULL )
			return FALSE;
	
		pSkillProp1 = prj.GetSkillProp( pSkill1->dwSkill );
		if( pSkillProp1 == NULL )
			return FALSE;
#if __VER >= 10 // __LEGEND	//	9차 전승시스템	Neuz, World, Trans
#ifdef __3RD_LEGEND16
		if( (int)( pSkillProp1->dwReqDisLV ) > pMover->GetLevel() && !pMover->IsMaster() && !pMover->IsHero() && !pMover->IsLegendHero() )
#else // __3RD_LEGEND16
		if( (int)( pSkillProp1->dwReqDisLV ) > pMover->GetLevel() && !pMover->IsMaster() && !pMover->IsHero() )
#endif // __3RD_LEGEND16
			return FALSE;
#else //__LEGEND	//	9차 전승시스템	Neuz, World, Trans
		if( pSkillProp1->dwReqDisLV > pMover->GetLevel() )
			return FALSE;
#endif	//__LEGEND	//	9차 전승시스템	Neuz, World, Trans
		
		if( pSkillProp->dwReSkillLevel2 != pSkill1->dwLevel )
			return FALSE;		
	}
#if __VER >= 10 // __LEGEND	//	9차 전승시스템	Neuz, World, Trans
#ifdef __3RD_LEGEND16
		if( (int)( pSkillProp->dwReqDisLV ) > pMover->GetLevel() && !pMover->IsMaster() && !pMover->IsHero() && !pMover->IsLegendHero() )
#else // __3RD_LEGEND16
		if( (int)( pSkillProp->dwReqDisLV ) > pMover->GetLevel() && !pMover->IsMaster() && !pMover->IsHero() )
#endif // __3RD_LEGEND16
			return FALSE;
#else //__LEGEND	//	9차 전승시스템	Neuz, World, Trans
	if( pSkillProp->dwReqDisLV > pMover->GetLevel() )
		return FALSE;
#endif	//__LEGEND	//	9차 전승시스템	Neuz, World, Trans

	return TRUE;
}


// 스킬 레벨업시 케릭터 레벨땜시 안나왔던 스킬들 검색하여 정보 출력
void CMover::PutLvUpSkillName_2( DWORD dwSkill )
{
#ifdef __CLIENT
	LPSKILL pSkill = GetSkill( dwSkill );
	ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
	
	//현재 활성화 되어있는냐?
	//나를 갖고있는 스킬을 검사하여 조건에 맞으면 그스킬을 활성화 한다.
	for( int i = 0; i < MAX_SKILL_JOB; i++ )	
	{
		if( m_aJobSkill[i].dwSkill == NULL_ID ) 
			continue;

		if( IsSkillParent( this, m_aJobSkill[i].dwSkill, dwSkill ) )
		{
			if( IsActive( this, m_aJobSkill[i].dwSkill ) )
			{
				ItemProp* pSkillProp1;
				pSkillProp1 = prj.GetSkillProp( m_aJobSkill[i].dwSkill );

				CString str;
				str.Format( prj.GetText( TID_GAME_REAPSKILL ), pSkillProp1->szName );
				g_WndMng.PutString( (LPCTSTR)str, NULL, prj.GetTextColor( TID_GAME_REAPSKILL ) );	
			}
		}
	}
#endif // CLIENT
}

#ifdef __WORLDSERVER
/// 거래, 죽음, 개인상점중, 전투중 FALSE
int CMover::GetSummonState()
{
	// 거래
	if( m_vtInfo.GetOther() != NULL )
		return 1;
	  // 창고
	if( m_bBank )
		return 1;
	  // 길드창고
	if( m_bGuildBank )
		return 1;

	// 죽음
	if( IsDie() )
		return 2;

	// 개인상점중
	if( m_vtInfo.VendorIsVendor() || m_vtInfo.IsVendorOpen() )
		return 3;

	// 전투중
	if( IsAttackMode() )
		return 4;
	
	// 비행중
	if( ( IsFly() ) )
		return 5;


#if __VER >= 8 // 8차 듀얼 061226 ma
	if( m_nDuel )
		return 6;
#endif // __VER >= 8 // 8차 듀얼 061226 ma


	return 0;
}
#endif //


#ifdef __CLIENT
LPCTSTR CMover::GetJobString()
{
	if( m_nJob != -1 )
		return prj.m_aJob[ m_nJob ].szName;
	return _T( "" );
}
#endif	// __CLIENT

BOOL  CMover::IsJobType( DWORD dwJobType ) 
{ 
	if( IsExpert() )
	{
		if( dwJobType == JTYPE_EXPERT ) 
			return TRUE;
		return FALSE;
	}
	return JTYPE_BASE == dwJobType;
}
BOOL CMover::IsBaseJob()
{
	return prj.m_aJob[ m_nJob ].dwJobType == JTYPE_BASE;
}
BOOL CMover::IsExpert()
{
	return prj.m_aJob[ m_nJob ].dwJobType == JTYPE_EXPERT;
}
BOOL CMover::IsPro()
{
	return prj.m_aJob[ m_nJob ].dwJobType == JTYPE_PRO;
}

#if __VER >= 10 // __LEGEND	//	9차 전승시스템	Neuz, World, Trans
BYTE	CMover::GetLegendChar()
{
	if(IsMaster())
		return LEGEND_CLASS_MASTER;
	else if(IsHero())
		return	LEGEND_CLASS_HERO;
#ifdef __3RD_LEGEND16
	else if(IsLegendHero())
		return LEGEND_CLASS_LEGENDHERO;
#endif //  __3RD_LEGEND16
	else
		return LEGEND_CLASS_NORMAL;
}

BOOL	CMover::IsMaster()
{
	return prj.m_aJob[ m_nJob ].dwJobType == 5;//temp JTYPE_MASTER;
}

BOOL	CMover::IsHero()
{
	return prj.m_aJob[ m_nJob ].dwJobType == 6;//temp JTYPE_HERO;
}
#endif	//__LEGEND	//	9차 전승시스템	Neuz, World, Trans

#ifdef __3RD_LEGEND16
BOOL	CMover::IsLegendHero()
{
	return prj.m_aJob[ m_nJob ].dwJobType == JTYPE_LEGEND_HERO;
}
#endif // __3RD_LEGEND16

BOOL CMover::IsInteriorityJob( int nJob )
{
	if( nJob == JOB_VAGRANT || nJob == m_nJob )
		return TRUE;

	if( IsPro() && JOB_VAGRANT < nJob && nJob < MAX_EXPERT )
	{
		if( nJob * 2 + 4 == m_nJob || nJob * 2 + 5 == m_nJob )
		{
			return TRUE;
		}
	}
	if( IsMaster()  )	// 레벨이 안되면 못참.				
	{
		if( nJob < MAX_EXPERT )
		{
			if( nJob * 2 + 14 == m_nJob || nJob * 2 + 15 == m_nJob )
				return TRUE;
		}
		else if( nJob < MAX_PROFESSIONAL )
		{
			if( nJob + 10 == m_nJob )
				return TRUE;
		}
	}
	if( IsHero()  )	// 레벨이 안되면 못참.				
	{
		if( nJob < MAX_EXPERT )
		{
			if( nJob * 2 + 22 == m_nJob || nJob * 2 + 23 == m_nJob )
				return TRUE;
		}
		else if( nJob < MAX_PROFESSIONAL )
		{
			if( nJob + 18 == m_nJob )
				return TRUE;
		}
#if __VER >= 14 // __NEW_ITEM_LIMIT_LEVEL
		else if( nJob < MAX_MASTER )	// Hero는 마스터 무기를 장착할 수 있다.
		{
			if( nJob + 8 == m_nJob )
				return TRUE;
		}
#endif // __NEW_ITEM_LIMIT_LEVEL
	}
#ifdef __3RD_LEGEND16
	if( IsLegendHero() )
	{
		if( nJob < MAX_EXPERT )
		{
			if( nJob * 2 + 30 == m_nJob || nJob * 2 + 31 == m_nJob )
				return TRUE;
		}
		else if( nJob < MAX_PROFESSIONAL )
		{
			if( nJob + 26 == m_nJob )
				return TRUE;
		}
		else if( nJob < MAX_MASTER )	// Hero는 마스터 무기를 장착할 수 있다.
		{
			if( nJob + 16 == m_nJob )
				return TRUE;
		}
		else if( nJob < MAX_HERO )
		{
			if( nJob + 8 == m_nJob )
				return TRUE;
		}
	}
#endif // __3RD_LEGEND16
	return FALSE;
}

// 스킬수 3(기본)14(익스퍼트)7(프로페셔널)
// 직업 10, 익스퍼트 40. 프로 30
BOOL  CMover::SetExpert( int nJob )
{
	if( m_nJob == nJob )
		return FALSE;
	m_nJob = nJob;

	int nNum = 3; // 3은 기본 스킬의 갯수다. 3 이상부터 익스퍼트 스킬이 추가된다는 의미 
	LPSKILL lpSkill;
	ItemProp** apSkillProp = prj.m_aJobSkill[ m_nJob ];
	int nJobNum = prj.m_aJobSkillNum[ m_nJob ];
	for( int i = 0; i < nJobNum; i++ )
	{
		ItemProp* pSkillProp = apSkillProp[ i ];
		lpSkill = &m_aJobSkill[ nNum++ ];
		lpSkill->dwSkill = pSkillProp->dwID;
	}
	return TRUE;
}

int   CMover::GetJob()
{
	return m_nJob; 
}

int   CMover::GetExpPercent()
{
	int nExpPercent = 0;

	nExpPercent = (int)( GetExp1() * 10000 / GetMaxExp1() );

	return nExpPercent; 
}


int   CMover::GetRemainGP()
{
	return m_nRemainGP;
}

// 명중률 하락 상태는 이큅이벤트와 상관없이 올수 있기때문에 GetParm()을 여기서 최종 계산해야한다.
int	CMover::GetAdjHitRate( void )
{
	return GetParam( DST_ADJ_HITRATE, m_nAdjHitRate );
}

// 회피율 
int CMover::GetParrying()
{
	if( IsPlayer() )
		return	( (int)( (GetDex() * 0.5f) + GetParam( DST_PARRY, m_nAdjParry ) ) );
	else
		return GetProp()->dwER;
}


void CMover::UpdateParam()
{
	if( IsPlayer() ) 
	{
		SumEquipDefenseAbility( &m_nDefenseMin, &m_nDefenseMax ); 
		SumEquipAdjValue( &m_nAdjHitRate, &m_nAdjParry );
	}
	else
	{
		m_nDefenseMin = m_nDefenseMax = GetNaturalArmor();
		m_nAdjHitRate = 0;
		m_nAdjParry = 0;
	}
}
// 아이템의 성능을 얻는다. 
int CMover::GetItemAbility( int nItem )
{
	ItemProp* pItemProp = prj.GetItemProp( nItem );
	int nAbilityMin = GetParam( DST_ABILITY_MIN, pItemProp->dwAbilityMin );
	int nAbilityMax = GetParam( DST_ABILITY_MAX, pItemProp->dwAbilityMax );
	if( nAbilityMax == nAbilityMin )
		return nAbilityMin;
	return nAbilityMin + ( xRand() % (nAbilityMax - nAbilityMin ) );
}
// 아이템의 최소 성능을 얻는다.
int CMover::GetItemAbilityMin( int nItem )
{
	ItemProp* pItemProp = prj.GetItemProp( nItem );
	int nAbilityMin = GetParam( DST_ABILITY_MIN, pItemProp->dwAbilityMin );
	return nAbilityMin;
}
// 아이템의 최대 성능을 얻는다.
int CMover::GetItemAbilityMax( int nItem )
{
	ItemProp* pItemProp = prj.GetItemProp( nItem );
	int nAbilityMax = GetParam( DST_ABILITY_MAX, pItemProp->dwAbilityMax );
	return nAbilityMax;
}

// 히트포인트 증감
void CMover::IncHitPoint(int nVal) 
{ 
	SetPointParam( DST_HP, m_nHitPoint + nVal );
}
// 마나포인트 증감
void CMover::IncManaPoint(int nVal) 
{ 
#ifdef __WORLDSERVER 			
	if( IsSMMode( SM_MAINTAIN_MP ) && nVal < 0 )
		return;
#ifdef __EVENT1206
	if( IsPlayer() && g_eLocal.GetState( EVE_EVENT1206 ) ) //  raiders_fix051217
		return;
#endif	// __EVENT1206
#endif // __WORLDSERVER
	SetPointParam( DST_MP, m_nManaPoint + nVal );
}

void CMover::IncFatiguePoint(int nVal) 
{ 
#ifdef __WORLDSERVER
	if( IsSMMode( SM_MAINTAIN_FP ) && nVal < 0 )
		return;
#ifdef __EVENT1206
	if( IsPlayer() && g_eLocal.GetState( EVE_EVENT1206 ) )	// raiders_fix051217
		return;
#endif	// __EVENT1206
#endif // __WORLDSERVER
	SetPointParam( DST_FP, m_nFatiguePoint + nVal );
}

#if __VER < 8 // __S8_PK
SLAUGHTER_GRADE CMover::GetSlaughterGrade()
{
	return prj.GetSlaughterGrade( m_nSlaughter );		// project.h
}

PK_TYPE GetPKCase( CMover *pAttacker, CMover *pDefender )
{
	SLAUGHTER_GRADE atkGrade = pAttacker->GetSlaughterGrade();			
	SLAUGHTER_GRADE defGrade = pDefender->GetSlaughterGrade();			

	if( pAttacker->IsMode( RECOVERCHAO_MODE ) )		// 카르마 회복 기간에 있는 상태에서 다시 다른 유저를 가격할 경우, 
		atkGrade = SLAUGHTER_SEMI_CHAOTIC;			// 준 카오 유저와 같은 룰이 적용이 된다.
	
	static PK_TYPE table[3][3] = {
		{ NORMAL_AND_NORMAL, NORMAL_AND_SEMI, NORMAL_AND_CHAO },
		{ SEMI_AND_NORMAL,   SEMI_AND_SEMI,   SEMI_AND_CHAO },
		{ CHAO_AND_NORMAL,   CHAO_AND_SEMI,   CHAO_AND_CHAO }
	};

	return table[ atkGrade ][ defGrade ];
}
#endif // __VER < 8 // __S8_PK
// 듀얼후의 결과 처리.
// this가 승자가 된다.
// pLose는 패배자.

#ifdef __WORLDSERVER

#if __VER < 8 // __S8_PK
// PK시도에 의한 m_nSlaughter(=카르마 수치) 변경  
// nDead : 죽은자, return : 증감량.
int CMover::IncSlaughterPoint2( CMover *pDead )
{
	ASSERT( pDead->IsPlayer() && IsPlayer() );

	int nOld = m_nSlaughter;
	PK_TYPE type = GetPKCase( this, pDead );

	if( IsRegionAttr( RA_PENALTY_PK ) == FALSE )
		return 0;

	switch( type )
	{
	case NORMAL_AND_NORMAL:		// 일반 유저 와 일반 유저 
		m_nSlaughter -= 50;
		break;
	case NORMAL_AND_SEMI:
		break;
	case NORMAL_AND_CHAO:
		break;

	case SEMI_AND_NORMAL:		// 준카오와 일반 유저 
		m_nSlaughter -= 30;
		break;
	case SEMI_AND_SEMI:
		break;
	case SEMI_AND_CHAO:
		break;

	case CHAO_AND_NORMAL:		// 카오와 일반 유저 
		break;
	case CHAO_AND_SEMI:
		break;
	case CHAO_AND_CHAO:
		break;
	}
	return ( m_nSlaughter - nOld );

}

// PK에 의한 경우, slaughter값을 변경시킨다.
// pDead : 죽은자, return : 증감량.
int CMover::IncSlaughterPoint( CMover *pDead )
{
	ASSERT( pDead->IsPlayer() && IsPlayer() );

	int nOld = m_nSlaughter;

	PK_TYPE type = GetPKCase( this, pDead );

	switch( type )
	{
	case NORMAL_AND_NORMAL:		// 일반 유저 와 일반 유저 
		if( IsRegionAttr( RA_PENALTY_PK ) )
			m_nSlaughter = -2000;
		else if( IsRegionAttr( RA_PK ) )
			m_nSlaughter -= 50;
		break;
	case NORMAL_AND_SEMI:
		break;
	case NORMAL_AND_CHAO:
		{
			if( pDead->IsAfterDeath() == FALSE )
			{
				// 절대값의 차가 큰 경우만 카르마 포인트를 얻는다.
				int nGradeA = prj.GetKarmaProp( m_nSlaughter )->nGrade;
				int nGradeB = prj.GetKarmaProp( pDead->m_nSlaughter )->nGrade;
				int nDelta = abs(nGradeB) - abs(nGradeA);
				if( nDelta > 0 )
					m_nSlaughter += nDelta;
			}
		}	
		break;

	case SEMI_AND_NORMAL:		// 준카오와 일반 유저 
		if( IsRegionAttr( RA_PENALTY_PK ) )
			m_nSlaughter -= 2000;
		else if( IsRegionAttr( RA_PK ) )
			m_nSlaughter -= 50;
		break;
	case SEMI_AND_SEMI:
		break;

	case SEMI_AND_CHAO:
		break;

	case CHAO_AND_NORMAL:		// 카오와 일반 유저 
		if( IsRegionAttr( RA_PENALTY_PK ) )
		{
			int nGrade = prj.GetKarmaProp( m_nSlaughter )->nGrade;
			switch( nGrade )
			{
			case -4: m_nSlaughter -= 5; break;
			case -5: m_nSlaughter -= 3; break;
			case -6: m_nSlaughter -= 1; break;
			}
		}
		break;
	case CHAO_AND_SEMI:
		break;
	case CHAO_AND_CHAO:
		break;
	}

	return ( m_nSlaughter - nOld );
}
#endif // __VER < 8 // __S8_PK

void CMover::ChangeFame( CMover *pLose )
{
	int nVal = 0;
	float fVal = 0;
	int nGap = 0;

	nGap = pLose->GetLevel() - GetLevel();		// 패자와 승자의 레벨차이

	if( nGap >= 80 )	    { nVal = 100; fVal = 15.0f; }	// 패자가 레벨이 훨 높았다.
	else if( nGap >= 30 )	{ nVal = 50;  fVal = 10.0f; }
	else if( nGap >= 20 )	{ nVal = 30;  fVal = 8.0f;  }
	else if( nGap >= 10 )	{ nVal = 10;  fVal = 6.0f;  }
	else if( nGap <= -80 )	{ nVal = 1;   fVal = 1.0f;  }	// 패자가 레벨이 훨 낮았다.
	else if( nGap <= -30 )	{ nVal = 2;   fVal = 2.0f;  }
	else if( nGap <= -20 )	{ nVal = 3;   fVal = 3.0f;  }
	else if( nGap <= -10 )	{ nVal = 4;   fVal = 4.0f;  }
	else                    { nVal = 5;   fVal = 5.0f;  }	// 9 ~ -9


	if( nVal )		// 승자에게 명성치 줌
	{	
		if( this->IsFly() && pLose->IsFly() )	// 공중 듀얼의 경우에는 패배시 상실하는 명성 포인트는 동일하지만, 승리 시 획득하는 명성포인트는 지상 전투에 2배로 한다
			nVal *= 2;
			
		m_nFame += nVal;	
		g_UserMng.AddSetFame( this, m_nFame );
		((CUser*)this)->AddDefinedText( TID_GAME_GETFAMEPOINT, "%d", nVal );	// xxx 명성 포인트를 얻었다
#if __VER >= 13 // __HONORABLE_TITLE			// 달인
		((CUser*)this)->CheckHonorStat();
		((CUser*)this)->AddHonorListAck();
		g_UserMng.AddHonorTitleChange( this, m_nHonor);
#endif	// __HONORABLE_TITLE			// 달인
	}
	
	if( fVal )	// 패자의 명성치(퍼센트)를 깎음.
	{
		int		nDecVal;
		fVal = fVal / 100.0f;	// 비율로 환산.
		nDecVal = (int)(pLose->m_nFame * fVal);
		if( nDecVal <= 0 )	nDecVal = 1;
		pLose->m_nFame -= nDecVal;
		if( pLose->m_nFame < 0 )
			pLose->m_nFame = 0;

		g_UserMng.AddSetFame( pLose, pLose->m_nFame );
		((CUser*)pLose)->AddDefinedText( TID_GAME_DECFAMEPOINT, "%d", nDecVal );	// xxx 명성 포인트가 감소.
		g_dpDBClient.SendLogPkPvp( this, pLose, nVal, 'D' );
#if __VER >= 13 // __HONORABLE_TITLE			// 달인
		((CUser*)pLose)->CheckHonorStat();
		((CUser*)pLose)->AddHonorListAck();
		g_UserMng.AddHonorTitleChange( pLose, pLose->m_nHonor);
#endif	// __HONORABLE_TITLE			// 달인
	}
}
#endif // __WORLDSERVER

int	CMover::DuelResult( CMover *pLose )
{
#ifdef __WORLDSERVER
	if( pLose->m_nDuel == 1 )
	{
		if( pLose->GetId() != m_idDuelOther )
			return 0;
	}

	// 승자는 레벨차에따라 경험치 상승
	// 패자는 레벨차에따라 경험치% 하락
	if( pLose->m_nDuel == 1 )	// 개인 PvP
	{
		ChangeFame( pLose );
		
#ifdef __PVPDEBUFSKILL
		//pLose->m_SkillState.SetMover( pLose );
		pLose->m_nDead = PROCESS_COUNT * 2;		// 종료 후 2초간은 무적
		pLose->RemoveAttackerBuffs( pLose->m_idDuelOther );
		//m_SkillState.SetMover( this );
		m_nDead = PROCESS_COUNT * 2;		// 종료 후 2초간은 무적
		RemoveAttackerBuffs( m_idDuelOther );
#endif // __PVPDEBUFSKILL
	}
	else
	// 극단 듀얼.
	if( pLose->m_nDuel == 2 )		// 극단 듀얼중일땐 극단장끼리의 레벨을 비교.
	{
		int nVal = 0;
		float fVal = 0;
		int nGap = 0;

		CParty *pLoseParty = g_PartyMng.GetParty( pLose->m_idparty );
		if( pLoseParty == NULL || pLoseParty->IsLeader( pLose->m_idPlayer ) == FALSE )		// 파티가 없거나 죽은넘이 대장이 아니면 걍 리턴.
			return 0;
		
		CParty *pParty = g_PartyMng.GetParty( m_idparty );		// 이긴측 파티.
		if( pParty == NULL )
			return 0;

		CMover* pLeader	= pParty->GetLeader();
		if( IsValidObj( pLeader ) )
			nGap	= pLose->GetLevel() - pLeader->GetLevel();
		else
			nGap	= pLose->GetLevel() - GetLevel();
		
		if( nGap >= 80 )	{ nVal = 100; fVal = 15.0f; }		// 패자가 레벨이 훨 높았다.
		else if( nGap >= 50 )	{ nVal = 50; fVal = 10.0f; }
		else if( nGap >= 20 )	{ nVal = 30; fVal = 8.0f; }
		else if( nGap >= 10 )	{ nVal = 10; fVal = 6.0f; }
		else if( nGap <= -80 )	{ nVal = 1; fVal = 1.0f; }		// 패자가 레벨이 훨 낮았다.
		else if( nGap <= -50 )	{ nVal = 2; fVal = 2.0f; }
		else if( nGap <= -20 )	{ nVal = 3; fVal = 3.0f; }
		else if( nGap <= -10 )	{ nVal = 4; fVal = 4.0f; }
		else { nVal = 5; fVal = 5.0f; }	// 9 ~ -9

		g_DPCoreClient.SendSetPartyDuel( pParty->m_uPartyId, pLoseParty->m_uPartyId, FALSE );

		pLose->m_bLastDuelParty = TRUE;
		pLoseParty->DoDuelResult( pParty, 0, nVal, fVal );		// 파티원들에게 패배사실을 알리고 듀얼 중지시킴.
		pParty->DoDuelResult( pLoseParty, 1, nVal, fVal );		// 파티원들에게 승리를 알림.
	}
#endif	// __WORLDSERVER

	return 1;
}

// 히트포인트를 셋하기
void CMover::SetHitPoint(int nVal) 
{
	m_nHitPoint = nVal;
//	m_nHitPoint += nVal;
	if( m_nHitPoint < 0 )
		m_nHitPoint = 0;
	if( m_nHitPoint > GetMaxHitPoint() )
		m_nHitPoint = GetMaxHitPoint(); 
}
// 마나 포인트를 셋하기 
void CMover::SetManaPoint(int nVal) 
{
	m_nManaPoint = nVal;
	if( m_nManaPoint < 0 )
		m_nManaPoint = 0;
	if( m_nManaPoint > GetMaxManaPoint() )
		m_nManaPoint = GetMaxManaPoint(); 
}
// 피지칼 포인트를 셋하기 
void CMover::SetFatiguePoint(int nVal) 
{
	m_nFatiguePoint = nVal;
	if( m_nFatiguePoint < 0 )
		m_nFatiguePoint = 0;
	if( m_nFatiguePoint > GetMaxFatiguePoint() )
		m_nFatiguePoint = GetMaxFatiguePoint(); 
}

int CMover::GetPointParam(int nDestParameter )
{
	switch( nDestParameter )
	{
		case DST_HP:	return( GetHitPoint() );
		case DST_MP:	return( GetManaPoint() );
		case DST_FP:	return( GetFatiguePoint() );
		case DST_GOLD:	return( GetGold() );
	}
	return( 0 );
}

void CMover::SetPointParam( int nDstParameter, int nValue, BOOL bTrans )	// bTrans가 TRUE이면 강제전송
{
#ifdef __WORLDSERVER
	int nMax = nValue;
#endif // __WORLDSERVER
	if( nValue < 0 )	nValue	= 0;
	BOOL	bTransfer	= bTrans;

	switch( nDstParameter )
	{
		case DST_HP:
#ifdef __WORLDSERVER
			if( nValue > GetMaxHitPoint() || nMax == 999999999 )	nValue	= GetMaxHitPoint();
#else // __WORLDSERVER
			if( nValue > GetMaxHitPoint() )	nValue	= GetMaxHitPoint();
#endif // __WORLDSERVER
			if( m_nHitPoint != nValue )
			{
				bTransfer	= TRUE;
				m_nHitPoint		= nValue;
			}
			break;
		case DST_MP:
#ifdef __WORLDSERVER
			if( nValue > GetMaxManaPoint() || nMax == 999999999 )	nValue	= GetMaxManaPoint();
#else // __WORLDSERVER
			if( nValue > GetMaxManaPoint() )	nValue	= GetMaxManaPoint();
#endif // __WORLDSERVER
			if( m_nManaPoint != nValue )
			{
				bTransfer	= TRUE;
				m_nManaPoint	= nValue;
			}
			break;
		case DST_FP:
#ifdef __WORLDSERVER
			if( nValue > GetMaxFatiguePoint() || nMax == 999999999 )	nValue	= GetMaxFatiguePoint();
#else // __WORLDSERVER
			if( nValue > GetMaxFatiguePoint() )		nValue	= GetMaxFatiguePoint();
#endif // __WORLDSERVER
			if( m_nFatiguePoint != nValue )
			{
				bTransfer	= TRUE;
				m_nFatiguePoint		= nValue;
			}
			break;
		case DST_GOLD:
			//m_dwGold	= (DWORD)nValue;
#ifdef __PERIN_BUY_BUG	// chipi_091219 추가
#ifdef __WORLDSERVER
			Error( "SetPointParam( DST_GOLD ) - [User:%s(%07d)], [Value:%d]", static_cast<CUser*>(this)->GetName(), static_cast<CUser*>(this)->m_idPlayer, nValue );
			return;
#endif // __WORLDSERVE
#endif // __PERIN_BUY_BUG

	#ifdef __CLIENT
			if( IsActiveMover() )
				PLAYSND(SND_INF_TRADE);
	#endif //__CLIENT
			bTransfer	= TRUE;
			SetGold( nValue );
			break;
	}
#ifdef __WORLDSERVER
	if( bTransfer )
		g_UserMng.AddSetPointParam( this, nDstParameter, nValue );
#endif	// __WORLDSERVER
}
BOOL CMover::IncStrLevel()
{
#ifdef __S_ADD_RESTATE
	if( m_nRemainGP > 0 )
	{
		m_nStr += 1;
		m_nRemainGP -= 1;
		return TRUE;
	}
#else // __S_ADD_RESTATE
	LPJOBABILITY pJobAbillity;
	if( IsExpert() )
		pJobAbillity = &prj.m_aJobAbillity[ GetJob() ];
	else
		pJobAbillity = &prj.m_aJobAbillity[ JOB_VAGRANT ];
	if( (int)m_nRemainGP >= (int)pJobAbillity->dwStr )		// 이부분 (int)로 캐스트 해주지 않으면 컴파일러 버그 난다.
	{
		m_nStr += pJobAbillity->dwStrAdd;
		m_nRemainGP -= pJobAbillity->dwStr;
		return TRUE;
	}
#endif // __S_ADD_RESTATE
	return FALSE;
}
BOOL CMover::IncStaLevel()
{
#ifdef __S_ADD_RESTATE
	if( m_nRemainGP > 0 )
	{
		m_nSta += 1;
		m_nRemainGP -= 1;
		return TRUE;
	}
#else // __S_ADD_RESTATE
	LPJOBABILITY pJobAbillity;
	if( IsExpert() )
		pJobAbillity = &prj.m_aJobAbillity[ GetJob() ];
	else
		pJobAbillity = &prj.m_aJobAbillity[ JOB_VAGRANT ];
	if( (int)m_nRemainGP >= (int)pJobAbillity->dwSta )
	{
		m_nSta += pJobAbillity->dwStaAdd;
		m_nRemainGP -= pJobAbillity->dwSta;
		return TRUE;
	}
#endif // __S_ADD_RESTATE
	return FALSE;
}
BOOL CMover::IncDexLevel()
{
#ifdef __S_ADD_RESTATE
	if( m_nRemainGP > 0 )
	{
		m_nDex += 1;
		m_nRemainGP -= 1;
		return TRUE;
	}
#else // __S_ADD_RESTATE
	LPJOBABILITY pJobAbillity;
	if( IsExpert() )
		pJobAbillity = &prj.m_aJobAbillity[ GetJob() ];
	else
		pJobAbillity = &prj.m_aJobAbillity[ JOB_VAGRANT ];
	if( (int)m_nRemainGP >= (int)pJobAbillity->dwDex )
	{
		m_nDex += pJobAbillity->dwDexAdd;
		m_nRemainGP -= pJobAbillity->dwDex;
		return TRUE;
	}
#endif // __S_ADD_RESTATE
	return FALSE;
}
BOOL CMover::IncIntLevel()
{
#ifdef __S_ADD_RESTATE
	if( m_nRemainGP > 0 )
	{
		m_nInt += 1;
		m_nRemainGP -= 1;
		return TRUE;
	}
#else // __S_ADD_RESTATE
	LPJOBABILITY pJobAbillity;
	if( IsExpert() )
		pJobAbillity = &prj.m_aJobAbillity[ GetJob() ];
	else
		pJobAbillity = &prj.m_aJobAbillity[ JOB_VAGRANT ];
	if( (int)m_nRemainGP >= (int)pJobAbillity->dwInt )
	{
		m_nInt += pJobAbillity->dwIntAdd;
		m_nRemainGP -= pJobAbillity->dwInt;
		return TRUE;
	}
#endif // __S_ADD_RESTATE
	return FALSE;
}

BOOL CMover::AddExperience( EXPINTEGER nExp, BOOL bFirstCall, BOOL bMultiPly, BOOL bMonster )
{
#ifdef __VTN_TIMELIMIT
	//	mulcom	BEGIN100315	베트남 시간 제한
	if( ::GetLanguage() == LANG_VTN )
	{
		if( IsPlayer() && m_nAccountPlayTime != -1 )
		{
			if( m_nAccountPlayTime < 0 || m_nAccountPlayTime > MIN( 300 ) )
			{
				nExp	= 0;
			}
			else if( m_nAccountPlayTime >= MIN( 180 ) && m_nAccountPlayTime <= MIN( 300 ) )
			{
				nExp	= (EXPINTEGER)( nExp * 0.5f );
			}
		}
	}
	//	mulcom	END100315	베트남 시간 제한
#endif // __VTN_TIMELIMIT

	if( nExp <= 0 )		// nExp가 마이너스이거나 0이면 처리할필요 없음.
		return FALSE;

	if( m_nHitPoint <= 0 )
		return FALSE;

#if __VER < 15 // __GUILD_HOUSE
	if( bFirstCall && bMultiPly )
	{
		nExp	*= GetExpFactor();
	}
#endif // __GUILD_HOUSE
#ifdef __INTERNALSERVER
	TRACE( "EXP = %I64d\n", nExp );
#endif	// __INTERNALSERVER

#if __VER >= 14 // __ANGEL_EXPERIENCE
#ifdef __3RD_LEGEND16
	if( IsMaster() || IsHero() || IsLegendHero())
#else // __3RD_LEGEND16
	if( IsMaster() || IsHero() )
#endif // __3RD_LEGEND16
		nExp /= 2;
#if __VER >= 15 // __GUILD_HOUSE
	if( bFirstCall && bMultiPly )
	{
#ifdef __WORLDSERVER
		EXPINTEGER nAddExp = static_cast<CUser*>( this )->GetAddExpAfterApplyRestPoint( nExp );
		nExp	= (EXPINTEGER)( nExp * GetExpFactor() );
		nExp += nAddExp;
#endif // __WORLDSERVER
	}
#endif // __GUILD_HOUSE

#if __VER >= 8 //__CSC_VER8_5
	if( bFirstCall && HasBuffByIk3( IK3_ANGEL_BUFF ) )
	{
		int nAngel = 100;
#ifdef __BUFF_1107
		IBuff* pBuff	= m_buffs.GetBuffByIk3( IK3_ANGEL_BUFF );
		WORD wId	= ( pBuff? pBuff->GetId(): 0 );
#else	// __BUFF_1107
		LPSKILLINFLUENCE lpSkillIn = m_SkillState.GetItemBuf( IK3_ANGEL_BUFF );
		WORD wId	= ( lpSkillIn? lpSkillIn->wID: 0 );
#endif	// __BUFF_1107
		if( wId > 0 )
		{
			ItemProp* pItemProp = prj.GetItemProp( wId );
			if( pItemProp )
				nAngel = (int)( (float)pItemProp->nAdjParamVal1 );
		}
		if( nAngel <= 0 || 100 < nAngel  )
			nAngel = 100;
		// 康: 전직 전 엔젤 버프 활성화 후 전직하는 경우, 필요 수치가 너무 크다.
		EXPINTEGER nMaxAngelExp = prj.m_aExpCharacter[m_nAngelLevel].nExp1 / 100 * nAngel;
		if( m_nAngelExp < nMaxAngelExp )
		{
			nExp /= 2;
			m_nAngelExp += nExp;
#ifdef __WORLDSERVER
#ifdef __ANGEL_LOG
#ifdef __EXP_ANGELEXP_LOG
			int nAngelExpPercent = (int)( m_nAngelExp * 100 / nMaxAngelExp );
			int nNextAngelExpLog = (int)(((CUser*)this)->m_nAngelExpLog / 20 + 1) * 20;
			
			if( nAngelExpPercent >= nNextAngelExpLog )
			{
				((CUser*)this)->m_nAngelExpLog = nAngelExpPercent;
				ItemProp* pItemProp = prj.GetItemProp( wId );
				
				if( pItemProp )
				{
					LogItemInfo aLogItem;
					aLogItem.Action		= "&";
					aLogItem.SendName	= ((CUser*)this)->GetName();
					aLogItem.RecvName	= "ANGEL_EXP_LOG";
					aLogItem.WorldId	= ((CUser*)this)->GetWorld()->GetID();
					aLogItem.Gold		= aLogItem.Gold2 = ((CUser*)this)->GetGold();
					//aLogItem.ItemName	= pItemProp->szName;
					_stprintf( aLogItem.szItemName, "%d", pItemProp->dwID );
					aLogItem.Gold_1		= (DWORD)( m_nAngelExp );
					g_DPSrvr.OnLogItem( aLogItem );
				}
			}
#endif //  __EXP_ANGELEXP_LOG
#endif // __ANGEL_LOG
			
			BOOL bAngelComplete = FALSE;
			if( m_nAngelExp > nMaxAngelExp )
			{
				m_nAngelExp = nMaxAngelExp;
				bAngelComplete = TRUE;
			}
			((CUser*)this)->AddAngelInfo( bAngelComplete );
#endif // __WORLDSERVER
		}
	}
#endif // __CSC_VER8_5
#endif // __ANGEL_EXPERIENCE

	if( IsBaseJob() )
	{
		if( m_nLevel >= MAX_JOB_LEVEL )
		{
			m_nExp1		= 0;
			return TRUE;
		}
	}
	else if( IsExpert() )
	{
		if( m_nLevel >= MAX_JOB_LEVEL + MAX_EXP_LEVEL )
		{
			m_nExp1		= 0;
			return TRUE;
		}
	}
	else if( IsPro() )
	{
		if( m_nLevel > MAX_GENERAL_LEVEL )
		{
			m_nLevel = MAX_GENERAL_LEVEL;
			return TRUE;
		}		
	}
#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
	else if(IsMaster())
	{
		if( m_nLevel > MAX_GENERAL_LEVEL )
		{
			m_nLevel = MAX_GENERAL_LEVEL;
			return TRUE;
		}
#if __VER < 14 // __ANGEL_EXPERIENCE
		else
		{
			nExp /= 2;
		}
#endif // __ANGEL_EXPERIENCE
	}
	else if(IsHero())
	{
#ifdef __3RD_LEGEND16
		if( m_nLevel >= MAX_LEGEND_LEVEL  )
#else // __3RD_LEGEND16
		if( m_nLevel >  MAX_LEGEND_LEVEL  )
#endif // __3RD_LEGEND16
		{
			m_nLevel = MAX_LEGEND_LEVEL;
			return TRUE;
		}
#if __VER < 14 // __ANGEL_EXPERIENCE
		else
		{
			nExp /= 2;
		}
#endif // __ANGEL_EXPERIENCE
	}
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans
#ifdef __3RD_LEGEND16
	else if( IsLegendHero() )
	{
		if( m_nLevel > MAX_3RD_LEGEND_LEVEL )
		{
			m_nLevel = MAX_3RD_LEGEND_LEVEL;
			return TRUE;
		}
	}
#endif // __3RD_LEGEND16

	int nLevelbuf = m_nLevel;
	int nNextLevel = m_nLevel + 1;

#if __VER < 14 // __ANGEL_EXPERIENCE
#if __VER >= 8 //__CSC_VER8_5
	if( bFirstCall && HasBuffByIk3( IK3_ANGEL_BUFF ) )
	{
		int nAngel = 100;
#ifdef __BUFF_1107
		IBuff* pBuff	= m_buffs.GetBuffByIk3( IK3_ANGEL_BUFF );
		WORD wId	= ( pBuff? pBuff->GetId(): 0 );
#else	// __BUFF_1107
		LPSKILLINFLUENCE lpSkillIn = m_SkillState.GetItemBuf( IK3_ANGEL_BUFF );
		WORD wId	= ( lpSkillIn? lpSkillIn->wID: 0 );
#endif	// __BUFF_1107
		if( wId > 0 )
		{
			ItemProp* pItemProp = prj.GetItemProp( wId );
			if( pItemProp )
				nAngel = (float)pItemProp->nAdjParamVal1;
		}
		if( nAngel <= 0 || 100 < nAngel  )
			nAngel = 100;
		// 康: 전직 전 엔젤 버프 활성화 후 전직하는 경우, 필요 수치가 너무 크다.
		EXPINTEGER nMaxAngelExp = prj.m_aExpCharacter[m_nAngelLevel].nExp1 / 100 * nAngel;
		if( m_nAngelExp < nMaxAngelExp )
		{
			nExp /= 2;
			m_nAngelExp += nExp;
#ifdef __WORLDSERVER
#ifdef __ANGEL_LOG
#ifdef __EXP_ANGELEXP_LOG
			int nAngelExpPercent = m_nAngelExp * 100 / nMaxAngelExp;
			int nNextAngelExpLog = (int)(((CUser*)this)->m_nAngelExpLog / 20 + 1) * 20;
	
			if( nAngelExpPercent >= nNextAngelExpLog )
			{
				((CUser*)this)->m_nAngelExpLog = nAngelExpPercent;
				ItemProp* pItemProp = prj.GetItemProp( wId );

				if( pItemProp )
				{
					LogItemInfo aLogItem;
					aLogItem.Action = "&";
					aLogItem.SendName = ((CUser*)this)->GetName();
					aLogItem.RecvName = "ANGEL_EXP_LOG";
					aLogItem.WorldId = ((CUser*)this)->GetWorld()->GetID();
					aLogItem.Gold = aLogItem.Gold2 = ((CUser*)this)->GetGold();
					//aLogItem.ItemName = pItemProp->szName;
					_stprintf( aLogItem.szItemName, "%d", pItemProp->dwID );
					aLogItem.Gold_1 = m_nAngelExp;
					g_DPSrvr.OnLogItem( aLogItem );
				}
			}
#endif //  __EXP_ANGELEXP_LOG
#endif // __ANGEL_LOG

			BOOL bAngelComplete = FALSE;
			if( m_nAngelExp > nMaxAngelExp )
			{
				m_nAngelExp = nMaxAngelExp;
				bAngelComplete = TRUE;
			}
			((CUser*)this)->AddAngelInfo( bAngelComplete );
#endif // __WORLDSERVER
		}
	}
#endif // __CSC_VER8_5
#endif // __ANGEL_EXPERIENCE

	m_nExp1 += nExp;									// pxp와 관계없이 exp가 증가. 만쉐이!
	
	if( bFirstCall )
	{
#if __VER >= 8 // __S8_PK
#ifdef __WORLDSERVER
		if( bMonster && IsChaotic() )
		{
			m_dwPKExp	= (DWORD)( m_dwPKExp + nExp );
			DWORD dwPropensity = GetPKPropensity(); // undorflow
			int nLevelPKExp = prj.GetLevelExp( GetLevel() ); // Level별 감소 경험치
			if( nLevelPKExp != 0 )
			{
				int nSubExp = m_dwPKExp / nLevelPKExp;
				if( nSubExp )
				{
					SetPKPropensity( GetPKPropensity() - ( m_dwPKExp / nLevelPKExp ) );
					if( dwPropensity <= GetPKPropensity() )
						SetPKPropensity( 0 );
					m_dwPKExp %= nLevelPKExp;
					
					g_UserMng.AddPKPropensity( this );
					g_dpDBClient.SendLogPkPvp( this, NULL, 0, 'P' );
				}
			}
		}
#endif // __WORLDSERVER
#endif // __VER >= 8 // __S8_PK
	}

	if( m_nExp1 >= prj.m_aExpCharacter[nNextLevel].nExp1 )	// 레벨업
	{
#ifdef __3RD_LEGEND16
		if( IsLegendHero() && ( nNextLevel > MAX_3RD_LEGEND_LEVEL ) )
		{
			m_nLevel = MAX_3RD_LEGEND_LEVEL;

			m_nExp1  = (prj.m_aExpCharacter[nNextLevel].nExp1 - 1);
			return FALSE;
		}
		else if( !IsHero() && !IsLegendHero() && nNextLevel > MAX_GENERAL_LEVEL )
		{
			m_nLevel = MAX_GENERAL_LEVEL;

			m_nExp1  = (prj.m_aExpCharacter[nNextLevel].nExp1 - 1);
			return FALSE;
		}
#else // __3RD_LEGEND16
#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
		if( IsHero() && ( nNextLevel > MAX_LEGEND_LEVEL ) )
		{
			m_nLevel = MAX_LEGEND_LEVEL;

			m_nExp1  = (prj.m_aExpCharacter[nNextLevel].nExp1 - 1);
			return FALSE;
		}
		else if( !IsHero() && nNextLevel > MAX_GENERAL_LEVEL )
		{
			m_nLevel = MAX_GENERAL_LEVEL;

			m_nExp1  = (prj.m_aExpCharacter[nNextLevel].nExp1 - 1);
			return FALSE;
		}
#else //__LEGEND	//	10차 전승시스템	Neuz, World, Trans
		if( nNextLevel > MAX_GENERAL_LEVEL )
		{
			m_nLevel = MAX_GENERAL_LEVEL;

			m_nExp1  = (prj.m_aExpCharacter[nNextLevel].nExp1 - 1);
			return FALSE;
		}
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans
#endif // __3RD_LEGEND16
		EXPINTEGER nExptmp;
//		BOOL f	= FALSE;

		{
			m_nRemainGP += prj.m_aExpCharacter[ nNextLevel ].dwLPPoint;
#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
#ifdef __3RD_LEGEND16
			if( IsMaster() || IsHero() || IsLegendHero() )
#else // __3RD_LEGEND16
			if( IsMaster() || IsHero() )
#endif // __3RD_LEGEND16
				m_nRemainGP++;
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans

			nExptmp		= m_nExp1 - prj.m_aExpCharacter[nNextLevel].nExp1;
#if __VER >= 8  
			m_nExp1		= 0;
#else //  __VER >= 8  
			m_nExp1		= m_nDeathExp		= 0;
#endif //  __VER >= 8  
			m_nLevel	= nNextLevel;

//			if( m_nLevel > m_nDeathLevel )
//				f	= TRUE;

			BOOL bLevelUp = TRUE;
			if( IsBaseJob() && m_nLevel > MAX_JOB_LEVEL ) 
			{
				m_nLevel = MAX_JOB_LEVEL;
				bLevelUp = FALSE;
			}
			else if( IsExpert() && m_nLevel > MAX_JOB_LEVEL + MAX_EXP_LEVEL )
			{
				m_nLevel = MAX_JOB_LEVEL + MAX_EXP_LEVEL;
				bLevelUp = FALSE;
			}
			else if( IsPro() && m_nLevel > MAX_GENERAL_LEVEL )
			{
				m_nLevel = MAX_GENERAL_LEVEL;
				bLevelUp = FALSE;

				m_nExp1  = (prj.m_aExpCharacter[nNextLevel].nExp1 - 1);
				nExptmp  = 0;
				return FALSE;
			}
#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
			else if( IsMaster() && m_nLevel > MAX_GENERAL_LEVEL )
			{
				m_nLevel = MAX_GENERAL_LEVEL;
				bLevelUp = FALSE;
				m_nExp1  = (prj.m_aExpCharacter[nNextLevel].nExp1 - 1);
				nExptmp  = 0;
				return FALSE;
			}
#ifdef __3RD_LEGEND16
			else if( IsHero() && (m_nLevel > MAX_LEGEND_LEVEL ) )
			{
				m_nLevel = MAX_LEGEND_LEVEL;
				bLevelUp = FALSE;
			}
			else if( IsLegendHero() && m_nLevel > MAX_3RD_LEGEND_LEVEL )
			{
				m_nLevel = MAX_GENERAL_LEVEL;
				bLevelUp = FALSE;
				m_nExp1  = (prj.m_aExpCharacter[nNextLevel].nExp1 - 1);
				nExptmp  = 0;
				return FALSE;
			}			
#else // __3RD_LEGEND16
			else if( IsHero() && (m_nLevel > MAX_LEGEND_LEVEL ) )
			{
				m_nLevel = MAX_LEGEND_LEVEL;
				bLevelUp = FALSE;
				m_nExp1  = (prj.m_aExpCharacter[nNextLevel].nExp1 - 1);
				nExptmp  = 0;
				return FALSE;
			}
#endif // __3RD_LEGEND16
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans

			if( bLevelUp )
			{
				m_nHitPoint = GetMaxHitPoint();
				m_nManaPoint = GetMaxManaPoint();
				m_nFatiguePoint = GetMaxFatiguePoint();
				SetJobLevel( m_nLevel, m_nJob );
#if __VER >= 8  
				if( m_nDeathLevel >= m_nLevel )
				{
					m_nRemainGP -= prj.m_aExpCharacter[ nNextLevel ].dwLPPoint;
#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
#ifdef __3RD_LEGEND16
					if( IsMaster() || IsHero() || IsLegendHero() )
#else // __3RD_LEGEND16
					if( IsMaster() || IsHero() )
#endif // __3RD_LEGEND16
						m_nRemainGP--;
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans
				}
#endif //  __VER >= 8  

#ifdef __WORLDSERVER
				if( m_nDeathLevel < m_nLevel )
				{
					int nGetPoint = ((GetLevel() - 1) / 20) + 2;
#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
					if( IsMaster() || IsHero() )
						SetMasterSkillPointUp();
					else
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans
					{
						AddSkillPoint( nGetPoint );
						g_dpDBClient.SendLogSkillPoint( LOG_SKILLPOINT_GET_HUNT, nGetPoint, this, NULL );
#ifdef __S_NEW_SKILL_2
						g_dpDBClient.SaveSkill( (CUser*)this );
#endif // __S_NEW_SKILL_2
					}
#ifdef __S_RECOMMEND_EVE
				    if( g_eLocal.GetState( EVE_RECOMMEND ) && IsPlayer() )
					{
						g_dpDBClient.SendRecommend( (CUser*)this );
					}					
#endif // __S_RECOMMEND_EVE
#ifdef __EXP_ANGELEXP_LOG
					((CUser*)this)->m_nExpLog = 0;
#endif // __EXP_ANGELEXP_LOG
#ifdef __EVENTLUA_GIFT
					prj.m_EventLua.SetLevelUpGift( (CUser*)this, m_nLevel );
#endif // __EVENTLUA_GIFT
#if __VER >= 15 // __CAMPUS
					CCampusHelper::GetInstance()->SetLevelUpReward( (CUser*)this );
#endif // __CAMPUS
				}
#endif // __WORLDSERVER
			}
			else
			{
				m_nRemainGP -= prj.m_aExpCharacter[ nNextLevel ].dwLPPoint;
#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
#ifdef __3RD_LEGEND16
				if( IsMaster() || IsHero() || IsLegendHero() )
#else // __3RD_LEGEND16
				if( IsMaster() || IsHero() )
#endif // __3RD_LEGEND16
					m_nRemainGP--;
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans
				nExptmp	= m_nExp1	= 0;
			}

			// 레벨 20이 되는순간 비행레벨은 1이된다.
			if( m_nLevel == 20 )
				SetFlightLv( 1 );
			
#ifdef __CLIENT
			if( m_pActMover && ( m_pActMover->IsState( OBJSTA_STAND ) || m_pActMover->IsState( OBJSTA_STAND2 )) )
				SetMotion( MTI_LEVELUP, ANILOOP_1PLAY, MOP_FIXED );
			CreateSfx(g_Neuz.m_pd3dDevice,XI_GEN_LEVEL_UP01,GetPos(),GetId());
			PlayMusic( BGM_IN_LEVELUP );
#endif	// __CLIENT
		}

		if( nExptmp > 0 )
			AddExperience( nExptmp, FALSE, bMultiPly );
		return TRUE;
	}

	return FALSE;
}

// 경험치를 nExp만큼 깎는다.  렙다운도 된다.
// bExp2Clear : pxp를 0으로 할건지 말건지.
// bLvDown : 레벨다운을 할건지 말건지.
BOOL CMover::DecExperience( EXPINTEGER nExp, BOOL bExp2Clear, BOOL bLvDown )
{
	if( nExp < 0 )	// 값은 항상 양수로만 줘야 한다.
		return FALSE;
	
	int nPrevLevel	= m_nLevel - 1;
	if( nPrevLevel < 1 )	// 레벨 1이하로는 더이상 내려가지 않음.
		return FALSE;

	m_nExp1		-= nExp;	// nExp만큼 경험치를 깎음. Exp2는 0%로.

#if __VER < 8     // 8차 스킬경험치다운변경
	m_nSkillExp	-= nExp;
#endif	// __VER >= 8  

	if( m_nExp1 < 0 )
	{
		if( bLvDown )	// 렙다운이 되는 상황.
		{
#if __VER >= 8  
			--m_nLevel;
			m_nExp1 = (EXPINTEGER)(prj.m_aExpCharacter[m_nLevel+1].nExp1 + m_nExp1 );
#endif //  __VER >= 8  
			return TRUE;	// 렙다운 됐으면 TRUE리턴
		}
		else
		{
			m_nExp1		= 0;
#ifdef __WORLDSERVER
			g_dpDBClient.SendLogLevelUp( this, 7 );
#endif // __WORLDSERVER
		}
	}
	return FALSE;
}

// 현재 레벨의 최대경험치의 %로 경험치 하락
// ex) fPercent = 0.2f;		// 20%
BOOL CMover::DecExperiencePercent( float fPercent, BOOL bExp2Clear, BOOL bLvDown )
{
	EXPINTEGER	nExpOld		= m_nExp1;		// 깎이기전 exp
	EXPINTEGER	nMaxExp		= prj.m_aExpCharacter[m_nLevel+1].nExp1;	// 깎이기전 경험치치 맥스량.
	int	nOldLv	= m_nLevel;		// 깎이기전 레벨.

	if( fPercent == 0.0f )	
		return FALSE;	// 0%를 깎는 상황이면 계산할 필요 없다.

	EXPINTEGER	nDecExp = (EXPINTEGER)(prj.m_aExpCharacter[m_nLevel+1].nExp1 * fPercent );	// 현재레벨의 최대경험치 * 퍼센트
	bLvDown		= DecExperience( nDecExp, bExp2Clear, bLvDown );

	if( bLvDown )	// 실제로 레벨이 다운되었을때.
	{
		if( m_nLevel - nOldLv < -1 )	// 레벨이 두단계 이상 다운되는 일은 없어야 한다.
			Error( "CMover::DecExperiencePercent : %s 레벨다운 이상. %d -> %d  %I64d(%f)"
			, m_szName, nOldLv, m_nLevel, nDecExp, fPercent );

		EXPINTEGER	nDecExp2	= ( prj.m_aExpCharacter[m_nLevel+1].nExp1 - m_nExp1 ) + nExpOld;	// 실제 깎인 포인트.
		if( nDecExp2 > nDecExp )	// 실제 깎인양이 원래 깎으려고 했던 양보다 많으면 경고.
			Error( "1 CMover::DecExperiencePercent : %s(Lv%d) 경험치가 이상하게 깎임. %I64d %I64d(%f) %I64d %I64d"
			, m_szName, GetLevel(), nMaxExp, nDecExp, fPercent, nExpOld, m_nExp1 );
	}
	else
	{
		EXPFLOAT fOldPercent = static_cast<EXPFLOAT>( nExpOld ) / static_cast<EXPFLOAT>( nMaxExp );		// 깎이기전 퍼센트량.
		EXPFLOAT fCurPercent = static_cast<EXPFLOAT>( m_nExp1 ) / static_cast<EXPFLOAT>( nMaxExp );		// 깍은후 퍼센트량.

		if( fOldPercent - fCurPercent > 0.060f )
			Error( "0 CMover::DecExperiencePercent : %s(Lv%d) 경험치가 이상하게 깎임. %I64d %I64d(%f) %I64d(%f) %I64d(%f)"
			, m_szName, GetLevel(), nMaxExp, nDecExp, fPercent, nExpOld, fOldPercent, m_nExp1, fCurPercent );
	}

	return bLvDown;
}

// 비행 경험치 획득
BOOL CMover::AddFxp( int nFxp )
{
	if( nFxp <= 0 )		// nExp가 마이너스이거나 0이면 처리할필요 없음.
		return FALSE;

	if( m_nHitPoint <= 0 )		// 죽고나선 경험치 획득 못함.
		return FALSE;

	if( GetFlightLv() == 0 )
		return FALSE;

	int nNextLevel = GetFlightLv() + 1;

	m_nFxp += nFxp;		// 일단 경험치를 더하고.
	if( (DWORD)( m_nFxp ) >= prj.m_aFxpCharacter[nNextLevel].dwFxp ) // 레벨업이 되었는지 확인.
	{
		int		FxpBuf;
		FxpBuf = m_nFxp - prj.m_aFxpCharacter[ nNextLevel ].dwFxp;		// 레벨업하고도 얼마나 더 오버됐는지?
		m_nFxp = 0;
		SetFlightLv( nNextLevel );	// 레벨 증가.
		
#ifdef __CLIENT
		// 레벨업시 이펙트
		CreateSfx(g_Neuz.m_pd3dDevice,XI_GEN_LEVEL_UP01,GetPos(),GetId());
		PlayMusic( BGM_IN_LEVELUP );
#endif	// __CLIENT
	
		// 오바된 경험치는 한번더 돌려서 더해줌.
		if( FxpBuf > 0)
		{
			AddFxp( FxpBuf );
		}

		return TRUE;
	}
	return FALSE;
}

BOOL CMover::AddChangeJob( int nJob )
{
	BOOL	bResult = FALSE;
	LPSKILL lpSkill;
	if( MAX_JOBBASE <= nJob && nJob < MAX_EXPERT ) // 1차 전직 까지~~ ^^;;;;
	{
		m_nJob = nJob;
		
		ItemProp** apSkillProp = prj.m_aJobSkill[ m_nJob ];
		int nJobNum = prj.m_aJobSkillNum[ m_nJob ];
		for( int i = 0; i < nJobNum; i++ )
		{
			ItemProp* pSkillProp = apSkillProp[ i ];
			lpSkill = &m_aJobSkill[ i + MAX_JOB_SKILL ];
			lpSkill->dwSkill = pSkillProp->dwID;
		}
		bResult = TRUE;
	}
	if( MAX_EXPERT <= nJob && nJob < MAX_PROFESSIONAL ) // 2차 전직 까지~~ ^^;;;;
	{
		m_nJob = nJob;
		
		ItemProp** apSkillProp = prj.m_aJobSkill[ m_nJob ];
		int nJobNum = prj.m_aJobSkillNum[ m_nJob ];
		for( int i = 0; i < nJobNum; i++ )
		{
			ItemProp* pSkillProp = apSkillProp[ i ];
			lpSkill = &m_aJobSkill[ i + MAX_JOB_SKILL + MAX_EXPERT_SKILL ];
			lpSkill->dwSkill = pSkillProp->dwID;
		}
		bResult = TRUE;
	}
#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
	if( MAX_PROFESSIONAL <= nJob && nJob < MAX_MASTER ) // 전승 까지~~ ^^;;;;
	{
		m_nJob = nJob;
		
		ItemProp** apSkillProp = prj.m_aJobSkill[ m_nJob ];
		int nJobNum = prj.m_aJobSkillNum[ m_nJob ];
		for( int i = 0; i < nJobNum; i++ )
		{
			ItemProp* pSkillProp = apSkillProp[ i ];
			lpSkill = &m_aJobSkill[ i + MAX_JOB_SKILL + MAX_EXPERT_SKILL + MAX_PRO_SKILL];
			lpSkill->dwSkill = pSkillProp->dwID;
			lpSkill->dwLevel = 1;	//master스킬은 1부터 준다.
		}
		bResult = TRUE;
	}
	
	if( MAX_MASTER <= nJob && nJob < MAX_HERO ) // 영웅 까지~~ ^^;;;;
	{
		m_nJob = nJob;
		
		ItemProp** apSkillProp = prj.m_aJobSkill[ m_nJob ];
		int nJobNum = prj.m_aJobSkillNum[ m_nJob ];
		for( int i = 0; i < nJobNum; i++ )
		{
			ItemProp* pSkillProp = apSkillProp[ i ];
			lpSkill = &m_aJobSkill[ i + MAX_JOB_SKILL + MAX_EXPERT_SKILL + MAX_PRO_SKILL + MAX_MASTER_SKILL ];
			lpSkill->dwSkill = pSkillProp->dwID;
		}
		bResult = TRUE;
	}
#ifdef __3RD_LEGEND16
	if( MAX_HERO <= nJob && nJob < MAX_LEGEND_HERO )
	{
		m_nJob = nJob;
		
		ItemProp** apSkillProp = prj.m_aJobSkill[ m_nJob ];
		int nJobNum = prj.m_aJobSkillNum[ m_nJob ];
		for( int i = 0; i < nJobNum; i++ )
		{
			ItemProp* pSkillProp = apSkillProp[ i ];
			lpSkill = &m_aJobSkill[ i + MAX_JOB_SKILL + MAX_EXPERT_SKILL + MAX_PRO_SKILL + MAX_MASTER_SKILL + MAX_HERO_SKILL ];
			lpSkill->dwSkill = pSkillProp->dwID;
		}
		bResult = TRUE;
	}
#endif // __3RD_LEGEND16
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans


#ifdef __WORLDSERVER
#ifdef __S_NEW_SKILL_2
	g_dpDBClient.SaveSkill( (CUser*)this );
#endif // __S_NEW_SKILL_2
#if __VER >= 13 // __HONORABLE_TITLE			// 달인
	((CUser*)this)->CheckHonorStat();
	((CUser*)this)->AddHonorListAck();
	g_UserMng.AddHonorTitleChange( this, m_nHonor);
#endif	// __HONORABLE_TITLE			// 달인
#endif // __WORLDSERVER
	return bResult;
}

BOOL CMover::SetFxp( int nFxp, int nFlightLv )
{
	m_nFxp = nFxp;

	// 레벨이 올랐으면.
	if( nFlightLv > GetFlightLv() )
	{
		SetFlightLv( nFlightLv );
#ifdef __CLIENT
		CreateSfx(g_Neuz.m_pd3dDevice,XI_GEN_LEVEL_UP01,GetPos(),GetId());
		PlayMusic( BGM_IN_LEVELUP );
		g_WndMng.PutString( prj.GetText( TID_GAME_FLYLVLUP ), NULL, prj.GetTextColor( TID_GAME_FLYLVLUP ) );
#endif // CLIENT
		return TRUE;
	}

	return FALSE;
}

BOOL CMover::SetExperience( EXPINTEGER nExp1, int nLevel )
{
	m_nExp1		= nExp1;

	if( IsInvalidObj(this) )
		return 0;

#ifdef __FASTJOBCHANGE
#ifdef __CLIENT
	if( (GetLevel() == MAX_GENERAL_LEVEL /*|| GetLevel() == MAX_LEGEND_LEVEL*/) && GetExpPercent() == 9999 )
	{
		SAFE_DELETE( g_WndMng.m_pJobChangeEx );
		g_WndMng.m_pJobChangeEx = new CWndJobChangeEx;
		g_WndMng.m_pJobChangeEx->Initialize();
	}
#endif
#endif//__FASTJOBCHANGE

	if( nLevel > m_nLevel )
	{
#ifdef __CLIENT
 		// 15렙 되면 더이상 초보자가 아니므로 자동으로 초보자도움말 끄자.
		if( nLevel == 15 )		// 1차전직레벨 15에 대한 define 있으면 그걸로 바꿔주. -xuzhu-
			g_Option.m_nInstantHelp = 0;
 		
		PutLvUpSkillName_1(nLevel);

		if( m_pActMover && ( m_pActMover->IsState( OBJSTA_STAND ) || m_pActMover->IsState( OBJSTA_STAND2 ) ) )
			SetMotion( MTI_LEVELUP, ANILOOP_1PLAY, MOP_FIXED );
		CreateSfx(g_Neuz.m_pd3dDevice,XI_GEN_LEVEL_UP01,GetPos(),GetId());
		PlayMusic( BGM_IN_LEVELUP );
		if( m_nDeathLevel < nLevel )
			g_WndMng.PutString( prj.GetText( TID_GAME_LEVELUP ), NULL, prj.GetTextColor( TID_GAME_LEVELUP ) );
		
		if( g_WndMng.m_pWndWorld )
		{
			if( ::GetLanguage() != LANG_JAP )
				g_Caption1.AddCaption(  prj.GetText( TID_GAME_LEVELUP_CAPTION ), g_WndMng.m_pWndWorld->m_pFontAPICaption );// CWndBase::m_Theme.m_pFontCaption );
			else
				g_Caption1.AddCaption(  prj.GetText( TID_GAME_LEVELUP_CAPTION ), NULL );// CWndBase::m_Theme.m_pFontCaption );
		}
		
		// 2, 8, 12
		static	int nPatLv[4]	= { 2, 8, 12, 0	};
		for( int i = 0; i < 3; i++ ) {
			if( m_nLevel < nPatLv[i] && nLevel >= nPatLv[i] ) {
				g_WndMng.PutString( prj.GetText( TID_GAME_PATTERNUP ), NULL, prj.GetTextColor( TID_GAME_PATTERNUP ) );
				break;
			}
			else if( m_nLevel >= nPatLv[2] )
				break;
		}

		if( TRUE == IsJobType( JTYPE_BASE ) )
		{
			if( nLevel == 15 )
				g_WndMng.PutString( prj.GetText( TID_EVE_LEVEL15 ), NULL, prj.GetTextColor( TID_EVE_LEVEL15 ) );

			static	int nNum	= prj.m_aJobSkillNum[JOB_VAGRANT];
		}
		if( m_nLevel < 20 && nLevel >= 20 )
		{
			g_WndMng.PutString( prj.GetText( TID_GAME_FLYLVLUP ), NULL, prj.GetTextColor( TID_GAME_FLYLVLUP ) );
			SetFlightLv( 1 );
		}
		else if( nLevel < 20 )
			SetFlightLv( 0 );
#endif	// __CLIENT
		m_nLevel	= nLevel;

		SetJobLevel( m_nLevel, m_nJob );

		m_nHitPoint = GetMaxHitPoint();
		m_nManaPoint = GetMaxManaPoint();
		m_nFatiguePoint = GetMaxFatiguePoint();

#ifdef __CLIENT
		if( IsPlayer() )
		{
			CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
			
			if( pWndWorld ) 
			{
#if __VER >= 13 // __RENEW_CHARINFO
				pWndWorld->GetAdvMgr()->AddAdvButton(APP_CHARACTER3);
#elif __VER >= 9 // __CSC_VER9_2
				pWndWorld->GetAdvMgr()->AddAdvButton(APP_CHARACTER2);
#else //__CSC_VER9_2
				pWndWorld->GetAdvMgr()->AddAdvButton(APP_CHARACTER);
#endif //__CSC_VER9_2
			#if __VER < 12 // __MOD_TUTORIAL				
				if( GetLevel() != 1 )
					pWndWorld->m_pWndGuideSystem->GuideStart(FALSE);
			#endif

#ifdef __FASTJOBCHANGE
if( GetLevel() == MAX_JOB_LEVEL || GetLevel() == (MAX_JOB_LEVEL+MAX_EXP_LEVEL) || GetLevel() == MAX_LEGEND_LEVEL )
{
	SAFE_DELETE( g_WndMng.m_pJobChangeEx );
	g_WndMng.m_pJobChangeEx = new CWndJobChangeEx;
	g_WndMng.m_pJobChangeEx->Initialize();
}
#endif//__FASTJOBCHANGE

				switch(GetLevel())
				{
			#if __VER >= 12 // __MOD_TUTORIAL
				case 15:
					pWndWorld->m_pWndGuideSystem->GuideStart(GUIDE_CHANGEJOB);
					break;						
				case 20:
					pWndWorld->m_pWndGuideSystem->GuideStart(GUIDE_FLIGHT_METHOD);
					break;
				case 40:
					pWndWorld->m_pWndGuideSystem->GuideStart(GUIDE_APP_GUILD);
					break;
			#else
				case 2:
#if __VER >= 13 // __RENEW_CHARINFO
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(APP_CHARACTER3);
#else if __VER >= 9 // __CSC_VER9_2
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(APP_CHARACTER2);
#else //__CSC_VER9_2
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(APP_CHARACTER);
#endif //__CSC_VER9_2
					break;
				case 3:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(GUIDE_EVENT_BERSERKERMODE);
					break;
				case 4:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(APP_SKILL_BEFOREJOB);
					break;
				case 8:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(GUIDE_ESSENSE);
					break;
				case 16:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(GUIDE_FLIGHT);
					break;
				case 15:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(GUIDE_CHANGEJOB);
					break;						
				case 20:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(GUIDE_FLIGHT_METHOD);
					break;
				case 49:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(GUIDE_APP_GUILD);
					break;
			#endif
				}
			}
		}
#endif //__CLIENT		

		return TRUE;
	}
	else if( nLevel < m_nLevel )	// 레벨 다운이 되었을때.
	{
		m_nLevel	= nLevel;
		if( nLevel < 20 )
			SetFlightLv( 0 );

#ifdef __CLIENT
		if( IsPlayer() )
		{
			CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
			
			if( pWndWorld ) 
			{
			#if __VER < 12 // __MOD_TUTORIAL
				if( GetLevel() != 1 )
					pWndWorld->m_pWndGuideSystem->GuideStart(FALSE);
			#endif
				switch(GetLevel())
				{
				case 2:
#if __VER >= 13 // __RENEW_CHARINFO
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(APP_CHARACTER3);
#elif __VER >= 9 // __CSC_VER9_2
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(APP_CHARACTER2);
#else //__CSC_VER9_2
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(APP_CHARACTER);
#endif //__CSC_VER9_2
					break;
				case 3:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(GUIDE_EVENT_BERSERKERMODE);
					break;
				case 4:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(APP_SKILL_BEFOREJOB);
					break;
				case 8:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(GUIDE_ESSENSE);
					break;
				case 15:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(GUIDE_CHANGEJOB);
					break;					
				case 16:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(GUIDE_FLIGHT);
					break;
				case 20:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(GUIDE_FLIGHT_METHOD);
					break;
				case 40:
					pWndWorld->m_pWndGuideSystem->SendGuideMessage(GUIDE_APP_GUILD);
					break;
				}
			}
		}
#endif	// __CLIENT
	}


	return FALSE;
}


void CMover::SetEquipDstParam()
{
	if( IsPlayer() ) 
	{
		CItemElem* pItemElem;
		for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
		{
			pItemElem = GetEquipItem( i ); //m_Inventory.GetAtId( m_adwEquipment[ i ] );
			if( pItemElem )
				SetDestParam( pItemElem->m_dwItemId, FALSE );	//don't send
		}
	}
}


/////////////////////////////////////////////////////////////////////////////// aaaaaaaaa
int CMover::SumEquipDefenseAbility( LONG* pnMin, LONG* pnMax )
{
	if( IsPlayer() )
	{
		int	nMin = 0, nMax = 0;
		CItemElem* pItemElem;
		ItemProp *pItemProp;

		for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
		{
			pItemElem = GetEquipItem( i ); //m_Inventory.GetAtId( m_adwEquipment[ i ] );
			//if(pItem && !pItem->IsBreakableItem() && (pItem->m_nHitPoint == 1 || pItem->m_nHitPoint == 0))
			//	continue; IK2_ARMOR
			// IK_ARMOR를 이제 체크하지 않으므로 머리나 팔찌등에 디펜스값이 들어가지 않도록 주의한다.
			// 무기의 어빌리티는 디펜스에 포함되지 않는다
			//if( pItemElem && pItemElem->GetProp()->dwItemKind2 < IK2_WEAPON_DIRECT && pItemElem->GetProp()->dwItemKind2 > IK2_WEAPON_GUN )  //&& ( pItemElem->m_nHitPoint > 1 || pItemElem->m_nHitPoint == -1) )
			if( pItemElem )
			{
				pItemProp = pItemElem->GetProp();

				if( pItemProp == NULL )
				{
					Error( "Parts Not Property = %d", i );
				}

				if( pItemProp )
				{
					if( pItemProp->dwItemKind2 == IK2_ARMOR || pItemProp->dwItemKind2 == IK2_ARMORETC )
					{
						int nOption = pItemElem->GetAbilityOption();
						int nOptionVal = 0;
						if( nOption > 0 ) 
							nOptionVal = (int)( pow( (float)( nOption ), 1.5f ) );
							
				#if __VER < 11 // __REMOVE_ENDURANCE
						if( pItemElem->m_nRepairNumber >= 100 )
							nOptionVal	+= prj.GetExpUpItem( pItemProp->dwItemKind3, 1 );
				#endif // __REMOVE_ENDURANCE						
						float f		= GetItemMultiplier( pItemElem );
						nMin	+= (int)( pItemProp->dwAbilityMin * f ) + nOptionVal;
						nMax	+= (int)( pItemProp->dwAbilityMax * f ) + nOptionVal;
					}
				}
			}
		}
		// 모든 합산이 끝났다면 각 요소에 수정치를 적용한다.
		nMin = GetParam( DST_ABILITY_MIN, nMin );
		nMax = GetParam( DST_ABILITY_MAX, nMax );
		if( pnMin ) *pnMin = nMin;
		if( pnMax ) *pnMax = nMax;
		return ( ( nMin + nMax ) / 2 ); // 리턴 값은 평균 값이다. 판정시에는 사용하지 않는다. 출력이나 기타 용도로 사용한다.
	}
	else
	{
	}
	
	return 0;
}

void CMover::SumEquipAdjValue( int* pnAdjHitRate, int* pnAdjParray )
{
	*pnAdjHitRate = 0;
	*pnAdjParray = 0;

	CItemElem* pItemElem;
	for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
	{
		pItemElem = GetEquipItem( i );
		if( pItemElem )  
		{
			if( pItemElem->GetProp() )
			{
				if( (int)pItemElem->GetProp()->nAdjHitRate > 0 )
					*pnAdjHitRate += pItemElem->GetProp()->nAdjHitRate;
				if( (int)pItemElem->GetProp()->dwParry > 0 )
					*pnAdjParray += pItemElem->GetProp()->dwParry;
			}
		}
	}
}

// 안보이는 상태
BOOL CMover::SetDarkCover( BOOL bApply, DWORD tmMaxTime )
{
	if( bApply )
	{
		if( GetAdjParam( DST_IMMUNITY ) & CHS_INVISIBILITY )	// 투명화에 면역이라....그런것도 있나?
			return FALSE;	
		SetDestParam( DST_CHRSTATE, CHS_INVISIBILITY, NULL_CHGPARAM );
		m_wDarkCover =	(short)( (tmMaxTime / 1000.0f) * PROCESS_COUNT );
	} else
	{
		if( GetAdjParam( DST_CHRSTATE) & CHS_INVISIBILITY )
		{
			ResetDestParam( DST_CHRSTATE, CHS_INVISIBILITY, TRUE );		// 암흑상태해제
		}
		m_wDarkCover = 0;
	}

	return TRUE;
}

// 스턴상태로 만들기
BOOL CMover::SetStun( BOOL bApply, DWORD tmMaxTime )
{
	if( bApply )
	{
		MoverProp *pMoverProp = GetProp();
		if( pMoverProp->dwClass == RANK_SUPER || pMoverProp->dwClass == RANK_MIDBOSS )
			return FALSE;			// 슈퍼몹은 스턴에 안걸린다. 이렇게 하지말고 스턴에 이뮨이 되게 설정하는게 좋다.

		if( GetAdjParam( DST_IMMUNITY ) & CHS_STUN ) 
			return FALSE;		// 독에 면역상태다.

		SetDestParam( DST_CHRSTATE, CHS_STUN, NULL_CHGPARAM, FALSE );
		SendActMsg( OBJMSG_STUN );		// 스턴 상태 시작.
		m_wStunCnt = (short)( (tmMaxTime / 1000.0f) * PROCESS_COUNT );		// 얼마동안이나 스턴상태가 되느냐.
	}
	else
	{
		if( GetAdjParam( DST_CHRSTATE ) & CHS_STUN )	// 이거 검사안하면 매번 클라로 ResetDestParam을 날린다.
			ResetDestParam( DST_CHRSTATE, CHS_STUN, FALSE);	// 스턴 상태 해제.
		m_wStunCnt = 0;
		m_dwFlag &= (~MVRF_NOACTION);				// 액션 금지 상태로 전환.
	}

#ifdef __WORLDSERVER
	g_UserMng.AddSetStun( this, tmMaxTime );
#endif // Worldserver

	return TRUE;
}


// this를 독에 걸리게 한다.
// idAttacker : 공격자(컨트롤일수도 있슴)
// tmMaxTime : 1/1000초 단위.
// tmUnit : 몇초 마다 독데미지가 깎일꺼냐.
// nDamage : 데미지량
BOOL CMover::SetPoison( BOOL bApply, OBJID idAttacker, DWORD tmMaxTime, DWORD tmUnit, short wDamage )
{
	if( bApply )
	{
		if( GetAdjParam( DST_IMMUNITY ) & CHS_POISON ) 
			return FALSE;		// 독에 면역상태다.
		SetDestParam( DST_CHRSTATE, CHS_POISON, NULL_CHGPARAM );
		if( tmMaxTime == -1 )
			Error( "SetPoison : %s 독효과의 지속시간이 지정되지 않음", GetName() );
		if( tmUnit == -1 )
			Error( "SetPoison : %s 독효과의 tick 시간이 지정되지 않음", GetName() );
		if( wDamage == -1 )
			Error( "SetPoison : %s 독효과의 틱당 데미지가 지정되지 않음", GetName() );
		
		m_wPoisonCnt = (short)( (tmMaxTime / 1000.0f) * PROCESS_COUNT );			// 1/1000
		m_wPoisonDamage = wDamage;
		m_tmPoisonUnit = tmUnit;
		m_idPoisonAttacker = idAttacker;
	} else
	{
		if( GetAdjParam( DST_CHRSTATE) & CHS_POISON )
			ResetDestParam( DST_CHRSTATE, CHS_POISON, TRUE );	// 독상태 해제
		m_wPoisonCnt = 0;
		m_wPoisonDamage = 0;
		m_tmPoisonUnit = 0;
	}

	return TRUE;
}

// this를 암흑상태에 빠지게 한다
// tmMaxTime : 암흑 지속시간(1/1000)
// nAdjHitRate : 지속시간동안 수정될 명중률
// bApply : 적용/해제
BOOL CMover::SetDark( BOOL bApply, DWORD tmMaxTime, int nAdjHitRate )
{
	if( bApply )
	{
		if( GetAdjParam( DST_IMMUNITY ) & CHS_DARK ) 
			return FALSE;		// 암흑에 면역상태다.
		SetDestParam( DST_CHRSTATE, CHS_DARK, NULL_CHGPARAM );
		SetDestParam( DST_ADJ_HITRATE, nAdjHitRate, NULL_CHGPARAM );
		m_wDarkCnt = (short)( (tmMaxTime / 1000.0f) * PROCESS_COUNT );
		if( nAdjHitRate == -1 )
			Error( "SetDark : %s 어둠효과의 명중률 수정치가 지정되지 않음", GetName() );
		m_wDarkVal = nAdjHitRate;
	} else
	{
		if( GetAdjParam( DST_CHRSTATE) & CHS_DARK )
		{
			ResetDestParam( DST_CHRSTATE, CHS_DARK, TRUE );		// 암흑상태해제
			ResetDestParam( DST_ADJ_HITRATE, m_wDarkVal, TRUE );	// 암흑동안 수정됐던 명중률 해제
//			if( m_wDarkCnt >= 0 )
//				Error( "SetDark : %d", m_wDarkVal );
		}
		m_wDarkCnt = 0;
		m_wDarkVal = 0;
	}
	return TRUE;
}

// this를 출혈을 일으킨다. 독과 비슷.
// tmMaxTime : 1/1000초 단위.
// tmUnit : 몇초 마다 데미지가 깎일꺼냐.
// nDamage : 데미지량
BOOL	CMover::SetBleeding( BOOL bApply, OBJID idAttacker, DWORD tmMaxTime, DWORD tmUnit, short wDamage )
{
	if( bApply )
	{
		if( GetAdjParam( DST_IMMUNITY ) & CHS_BLEEDING ) return FALSE;		// 출혈에 면역상태다.
		
		SetDestParam( DST_CHRSTATE, CHS_BLEEDING, NULL_CHGPARAM );
		if( tmMaxTime == -1 )
			Error( "SetBleeding : %s 출혈효과의 지속시간이 지정되지 않음", GetName() );
		if( tmUnit == -1 )
			Error( "SetBleeding : %s 출혈효과의 tick 시간이 지정되지 않음", GetName() );
		if( wDamage == -1 )
			Error( "SetBleeding : %s 출혈효과의 틱당 데미지가 지정되지 않음", GetName() );
		
		m_wBleedingCnt = (short)( (tmMaxTime / 1000.0f) * PROCESS_COUNT );			// 1/1000
		m_wBleedingDamage = wDamage;
		m_tmBleedingUnit = tmUnit;
		m_idBleedingAttacker = idAttacker;
	} else
	{
		if( GetAdjParam( DST_CHRSTATE) & CHS_BLEEDING )
			ResetDestParam( DST_CHRSTATE, CHS_BLEEDING, TRUE );	// 독상태 해제
		m_wBleedingCnt = 0;
		m_wBleedingDamage = 0;
		m_tmBleedingUnit = 0;
	}
	return TRUE;
}

// dwState를 해제함
// CHS_ALL을 하면 모든 디버프.
void CMover::RemoveDebuff( DWORD dwState )
{
	// 스킬로 걸린건 이걸로 해제해야하고
	RemoveChrStateBuffs( dwState );
	// 스킬이 아닌 아이템같은걸로 상태만 걸린건 이걸로 풀어야 한다.
	if( dwState & CHS_DARK )
		SetDark( FALSE );
	if( dwState & CHS_POISON )
		SetPoison( FALSE );
	if( dwState & CHS_STUN )
		SetStun( FALSE );
#if __VER >= 10	// 
	if( dwState & CHS_BLEEDING )
		SetBleeding( FALSE );
	
	#if __VER >= 11 // __MA_VER11_06				// 확율스킬 효과수정 world,neuz
	if( dwState & DST_CLEARBUFF )
	{
		RemoveDstParamBuffs( DST_SPEED );
		RemoveDstParamBuffs( DST_CHR_CHANCEPOISON );
		RemoveDstParamBuffs( DST_CHR_CHANCESTUN );
		RemoveDstParamBuffs( DST_CHR_CHANCEBLEEDING );
	}
	#endif // __MA_VER11_06				// 확율스킬 효과수정 world,neuz

#endif	//

	// 이거 안에서 Reset을 호출한다.
	SetDestParam( DST_CURECHR, dwState, NULL_CHGPARAM );
}

// this에게 걸린 버프를 1개 없앤다.
void CMover::RemoveBuffOne( DWORD dwSkill )
{
#ifdef __BUFF_1107
	m_buffs.RemoveBuffs( RBF_ONCE | RBF_GOODSKILL, 0 );
#else	// __BUFF_1107
	m_SkillState.RemoveOneSkillBuff();		// 이로운버프스킬 하나를 제거 한다.
#endif	// __BUFF_1107
}	

void CMover::RemoveBuffAll( void )
{
#ifdef __BUFF_1107
	m_buffs.RemoveBuffs( RBF_GOODSKILL, 0 );
#else	// __BUFF_1107
	m_SkillState.RemoveAllSkillBuff();		// 이로운버프스킬을 모두 찾아 삭제 한다.
#endif	// __BUFF_1107
}




///////////////////////////////////////////////////////////////////////////////
// 
// 독, 암흑등 추가능력치의 세팅은 이걸로 하자.
void CMover::SetDestParam( int nIdx, ItemProp *pProp, BOOL bSend )
{
	SetDestParam( pProp->dwDestParam[nIdx], pProp->nAdjParamVal[nIdx], pProp->dwChgParamVal[nIdx], bSend );
}

void CMover::ResetDestParam( int nIdx, ItemProp *pProp, BOOL bSend )
{
	ResetDestParam( pProp->dwDestParam[nIdx], pProp->nAdjParamVal[nIdx], bSend );
}

// 목표 파라미터를 셋한다.
void CMover::SetDestParam( int nItemIdx, BOOL bSend )
{
	ItemProp* pItemProp = prj.GetItemProp( nItemIdx );
	if( pItemProp->dwDestParam1 != -1 )
		SetDestParam( pItemProp->dwDestParam1, pItemProp->nAdjParamVal1, pItemProp->dwChgParamVal1, bSend );
	if( pItemProp->dwDestParam2 != -1 )
		SetDestParam( pItemProp->dwDestParam2, pItemProp->nAdjParamVal2, pItemProp->dwChgParamVal2, bSend );
#ifdef __PROP_0827
	if( pItemProp->dwDestParam3 != -1 )
		SetDestParam( pItemProp->dwDestParam3, pItemProp->nAdjParamVal3, pItemProp->dwChgParamVal3, bSend );
#endif	// __PROP_0827
#if __VER >= 19
	if( pItemProp->dwDestParam4 != -1 )
		SetDestParam( pItemProp->dwDestParam4, pItemProp->nAdjParamVal4, pItemProp->dwChgParamVal4, bSend );
	if( pItemProp->dwDestParam5 != -1 )
		SetDestParam( pItemProp->dwDestParam5, pItemProp->nAdjParamVal5, pItemProp->dwChgParamVal5, bSend );
	if( pItemProp->dwDestParam6 != -1 )
		SetDestParam( pItemProp->dwDestParam6, pItemProp->nAdjParamVal6, pItemProp->dwChgParamVal6, bSend );
#endif//__VER >= 19
}

//--------------------------------------------------------------------------
//
// 목표 파라메타를 셋한다.
void CMover::SetDestParam( int nDstParameter, int nAdjParameterValue, int nChgParameterValue, BOOL bSend )
{
	BOOL fSuccess	= FALSE;

#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
	if( (nDstParameter < 0 ) && ( nDstParameter != DST_ADJDEF_RATE && nDstParameter != DST_ADJDEF ) )
		return;
#else
	if( nDstParameter < 0 )
		return;
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans
	
	// HP, MP등은 SetPointParam으로 처리한다. 근디 SetDestParam 하나로 통일시켰으면 낫지 않을까?
	switch( nDstParameter )
	{
#ifdef __WORLDSERVER
	case DST_HP:
		{
			if( nAdjParameterValue == -1 )
				SetPointParam( nDstParameter, GetMaxHitPoint() );
			else
				SetPointParam( nDstParameter, GetHitPoint() + nAdjParameterValue );
		}
		return;
	case DST_MP:
		{
			if( nAdjParameterValue == -1 )
				SetPointParam( nDstParameter, GetMaxManaPoint() );
			else
				SetPointParam( nDstParameter, GetManaPoint() + nAdjParameterValue );
			
		}
		return;
	case DST_FP:
		{
			if( nAdjParameterValue == -1 )
				SetPointParam( nDstParameter, GetMaxFatiguePoint() );
			else
				SetPointParam( nDstParameter, GetFatiguePoint() + nAdjParameterValue );
		}
		return;
#else // __WORLDSERVER
	case DST_HP:	SetPointParam( nDstParameter, GetHitPoint() + nAdjParameterValue );	return;
	case DST_MP:	SetPointParam( nDstParameter, GetManaPoint() + nAdjParameterValue );	return;
	case DST_FP:	SetPointParam( nDstParameter, GetFatiguePoint() + nAdjParameterValue );	return;
#endif // __WORLDSERVER
	case DST_GOLD:	
#ifdef __PERIN_BUY_BUG	// chipi_091219 추가
		Error( "SetDestParam( DST_GOLD ) - [User:%s(%07d)], [Value:%d]", static_cast<CUser*>(this)->GetName(), static_cast<CUser*>(this)->m_idPlayer, nAdjParameterValue );
		return;
#endif // __PERIN_BUY_BUG

		if( AddGold( nAdjParameterValue ) == FALSE )	
			SetGold( 0 );		// underflow이면 0로 만든다. 이전코드와 호환되게 한다. 뭔가 찜찜 
		return;

	case DST_RESIST_ALL:
		SetDestParam( DST_RESIST_ELECTRICITY,	nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_RESIST_FIRE,			nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_RESIST_WIND,			nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_RESIST_WATER,			nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_RESIST_EARTH,			nAdjParameterValue, nChgParameterValue, bSend );
		return;
	case DST_STAT_ALLUP:
		SetDestParam( DST_STR,	nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_DEX,	nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_INT,	nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_STA,	nAdjParameterValue, nChgParameterValue, bSend );
		return;
	case DST_HPDMG_UP:	// DST_HP_MAX + DST_CHR_DMG 
		SetDestParam( DST_HP_MAX, nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_CHR_DMG, nAdjParameterValue, nChgParameterValue, bSend );
		return;
	case DST_DEFHITRATE_DOWN: // DST_ADJDEF + DST_ADJ_HITRATE
		SetDestParam( DST_ADJDEF, nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_ADJ_HITRATE, nAdjParameterValue, nChgParameterValue, bSend );
		return;
	case DST_LOCOMOTION:
		SetDestParam( DST_SPEED,	nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_JUMPING,	( nAdjParameterValue * 3 ), nChgParameterValue, bSend );
		return;
#ifdef __NEWWPN1024
	case DST_MASTRY_ALL:
		SetDestParam( DST_MASTRY_EARTH,	nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_MASTRY_FIRE,	nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_MASTRY_WATER,	nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_MASTRY_ELECTRICITY,	nAdjParameterValue, nChgParameterValue, bSend );
		SetDestParam( DST_MASTRY_WIND,	nAdjParameterValue, nChgParameterValue, bSend );
		return;
#endif	// __NEWWPN1024

	case DST_HP_RECOVERY_RATE:
		{
			int nMax	= GetMaxOriginHitPoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_HP_RECOVERY, nRecv, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_MP_RECOVERY_RATE:
		{
			int nMax	= GetMaxOriginManaPoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_MP_RECOVERY, nRecv, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_FP_RECOVERY_RATE:
		{
			int nMax	= GetMaxOriginFatiguePoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_FP_RECOVERY, nRecv, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_ALL_RECOVERY:
		{
			// HP
			SetDestParam( DST_HP_RECOVERY, nAdjParameterValue, NULL_CHGPARAM, bSend );
			// MP
			SetDestParam( DST_MP_RECOVERY, nAdjParameterValue, NULL_CHGPARAM, bSend );
			// FP
			SetDestParam( DST_FP_RECOVERY, nAdjParameterValue, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_ALL_RECOVERY_RATE:
		{
			// HP
			int nMax	= GetMaxOriginHitPoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_HP_RECOVERY, nRecv, NULL_CHGPARAM, bSend );

			// MP
			nMax	= GetMaxOriginManaPoint();		// 버프의 영향을 받지 않은 오리지날 양
			nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_MP_RECOVERY, nRecv, NULL_CHGPARAM, bSend );

			// FP
			nMax	= GetMaxOriginFatiguePoint();		// 버프의 영향을 받지 않은 오리지날 양
			nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_FP_RECOVERY, nRecv, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_KILL_ALL:	
		{
			// HP
			SetDestParam( DST_KILL_HP, nAdjParameterValue, NULL_CHGPARAM, bSend );
			// MP
			SetDestParam( DST_KILL_MP, nAdjParameterValue, NULL_CHGPARAM, bSend );
			// FP
			SetDestParam( DST_KILL_FP, nAdjParameterValue, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_KILL_HP_RATE:
		{
			int nMax	= GetMaxOriginHitPoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_KILL_HP, nRecv, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_KILL_MP_RATE:
		{
			int nMax	= GetMaxOriginManaPoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_KILL_MP, nRecv, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_KILL_FP_RATE:
		{
			int nMax	= GetMaxOriginFatiguePoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_KILL_FP, nRecv, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_KILL_ALL_RATE:
		{
			// HP
			int nMax	= GetMaxOriginHitPoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_KILL_HP, nRecv, NULL_CHGPARAM, bSend );
			
			// MP
			nMax	= GetMaxOriginManaPoint();		// 버프의 영향을 받지 않은 오리지날 양
			nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_KILL_MP, nRecv, NULL_CHGPARAM, bSend );
			
			// FP
			nMax	= GetMaxOriginFatiguePoint();		// 버프의 영향을 받지 않은 오리지날 양
			nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			SetDestParam( DST_KILL_FP, nRecv, NULL_CHGPARAM, bSend );
		}
		return;
	case DST_ALL_DEC_RATE:
		{
			SetDestParam( DST_MP_DEC_RATE, nAdjParameterValue, NULL_CHGPARAM, bSend );
			SetDestParam( DST_FP_DEC_RATE, nAdjParameterValue, NULL_CHGPARAM, bSend );
		}
		return;
	} // switch

	// 이 검사를 왜 위에다 안넣고 여기다 넣었냐면
	// DST_GOLD, PXP, RESIST_ALL같은경우는 MAX_ADJPARAMARY보다 더 큰값을 사용한다. 
	// 그래서 그런것들은 위에서 처리하고 바로 리턴하고
	// 일반적인것들만 이쪽으로 온다.
	if( nDstParameter >= MAX_ADJPARAMARY)
		return;
	
	if( nAdjParameterValue != 0 )	// 0이 들어온것도 잘못된거니까 경고를 내야 하지 않을까 싶다.
	{
		fSuccess	= TRUE;

		switch( nDstParameter )
		{
		case DST_CHRSTATE:		// 상태변경의 경우는
		case DST_IMMUNITY:
			if( nAdjParameterValue != NULL_ID )
			{
				if( nAdjParameterValue == 0xffffffff )
				{
					Error( "SetDestParam : Adj == -1, %s", GetName() );
					return;
				}
				m_adjParamAry[nDstParameter] |= nAdjParameterValue;		// 비트로 온오프 된다.
			} else
				fSuccess = FALSE;
			break;
		case DST_CURECHR:		// 상태해제
			ResetDestParam( DST_CHRSTATE, nAdjParameterValue, bSend );
			return;
		case DST_REFLECT_DAMAGE:
			m_adjParamAry[nDstParameter] += nAdjParameterValue;		// 데미지의 몇%를 돌려주는가?
			m_chgParamAry[nDstParameter] = nChgParameterValue;		// 몇%의 확률로 발동되는가?
			break;
		case DST_CHR_CHANCEBLEEDING:
		case DST_CHR_CHANCESTEALHP:
		case DST_CHR_CHANCEPOISON:
		case DST_CHR_CHANCEDARK:
		case DST_CHR_CHANCESTUN:
		case DST_AUTOHP:
			m_adjParamAry[nDstParameter] += nAdjParameterValue;		// 찬스 확률.	// %미만으로 회복되게 할꺼냐.
			m_chgParamAry[nDstParameter] = nChgParameterValue;		// 찬스가 걸렸을때 지속시간		// %HP가 회복될꺼냐.
			break;
#if __VER >= 9	// __PET_0410
		case DST_HEAL:
			m_nHealCnt	= (short)( PROCESS_COUNT * 6.0f );		// 쿨타임 6초
			m_adjParamAry[nDstParameter]	+= nAdjParameterValue;	
			break;
#endif	// __PET_0410
		default:
			m_adjParamAry[nDstParameter] += nAdjParameterValue;		// 누적
			break;
		}
	}

	else if( nChgParameterValue != 0x7FFFFFFF ) 
	{
		fSuccess	= TRUE;

		// 尹
		if( nDstParameter == DST_SPEED )
		{
			if( m_chgParamAry[nDstParameter] == 0x7FFFFFFF )
				m_chgParamAry[nDstParameter]	= 0;
			m_chgParamAry[nDstParameter]++;
		}
		else
		{
			m_chgParamAry[nDstParameter] = nChgParameterValue;	// 고정된 값.
		}
	}
#ifdef __WORLDSERVER
	if( TRUE == ( fSuccess & bSend ) ) {
		g_UserMng.AddSetDestParam( this, nDstParameter, nAdjParameterValue, nChgParameterValue );
	}
#endif	// __WORLDSERVER
}

//
// 목표 파라메타를 리셋한다.
//
void CMover::ResetDestParam( int nDstParameter, int nAdjParameterValue, BOOL fSend )
{
	switch( nDstParameter )
	{
	case DST_RESIST_ALL:
		ResetDestParam( DST_RESIST_ELECTRICITY,	nAdjParameterValue, TRUE );
		ResetDestParam( DST_RESIST_FIRE,		nAdjParameterValue, TRUE );
		ResetDestParam( DST_RESIST_WIND,		nAdjParameterValue, TRUE );
		ResetDestParam( DST_RESIST_WATER,		nAdjParameterValue, TRUE );
		ResetDestParam( DST_RESIST_EARTH,		nAdjParameterValue, TRUE );
		return;
	case DST_STAT_ALLUP:
		ResetDestParam( DST_STR,	nAdjParameterValue, TRUE );
		ResetDestParam( DST_DEX,	nAdjParameterValue, TRUE );
		ResetDestParam( DST_INT,	nAdjParameterValue, TRUE );
		ResetDestParam( DST_STA,	nAdjParameterValue, TRUE );
		return;
	case DST_HPDMG_UP:	// DST_HP_MAX + DST_CHR_DMG 
		ResetDestParam( DST_HP_MAX, nAdjParameterValue, TRUE );
		ResetDestParam( DST_CHR_DMG, nAdjParameterValue, TRUE );
		return;
	case DST_DEFHITRATE_DOWN: // DST_ADJDEF + DST_ADJ_HITRATE
		ResetDestParam( DST_ADJDEF, nAdjParameterValue, TRUE );
		ResetDestParam( DST_ADJ_HITRATE, nAdjParameterValue, TRUE );
		return;
	case DST_LOCOMOTION:
		ResetDestParam( DST_SPEED,	nAdjParameterValue, TRUE );
		ResetDestParam( DST_JUMPING,	( nAdjParameterValue * 3 ), TRUE );
		return;
#ifdef __NEWWPN1024
	case DST_MASTRY_ALL:
		ResetDestParam( DST_MASTRY_EARTH,	nAdjParameterValue, TRUE );
		ResetDestParam( DST_MASTRY_FIRE,	nAdjParameterValue, TRUE );
		ResetDestParam( DST_MASTRY_WATER,	nAdjParameterValue, TRUE );
		ResetDestParam( DST_MASTRY_ELECTRICITY,	nAdjParameterValue, TRUE );
		ResetDestParam( DST_MASTRY_WIND,	nAdjParameterValue, TRUE );
		return;
#endif	// __NEWWPN1024

	case DST_HP_RECOVERY_RATE:
		{
			int nMax	= GetMaxOriginHitPoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_HP_RECOVERY, nRecv, TRUE );
		}
		return;
	case DST_MP_RECOVERY_RATE:
		{
			int nMax	= GetMaxOriginManaPoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_MP_RECOVERY, nRecv, TRUE );
		}
		return;
	case DST_FP_RECOVERY_RATE:
		{
			int nMax	= GetMaxOriginFatiguePoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_FP_RECOVERY, nRecv, TRUE );
		}
		return;
	case DST_ALL_RECOVERY:
		{
			// HP
			ResetDestParam( DST_HP_RECOVERY, nAdjParameterValue, TRUE );
			// MP
			ResetDestParam( DST_MP_RECOVERY, nAdjParameterValue, TRUE );
			// FP
			ResetDestParam( DST_FP_RECOVERY, nAdjParameterValue, TRUE );
		}
		return;
	case DST_ALL_RECOVERY_RATE:
		{
			// HP
			int nMax	= GetMaxOriginHitPoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_HP_RECOVERY, nRecv, TRUE );
			
			// MP
			nMax	= GetMaxOriginManaPoint();		// 버프의 영향을 받지 않은 오리지날 양
			nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_MP_RECOVERY, nRecv, TRUE );
			
			// FP
			nMax	= GetMaxOriginFatiguePoint();		// 버프의 영향을 받지 않은 오리지날 양
			nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_FP_RECOVERY, nRecv, TRUE );
		}
		return;
	case DST_KILL_ALL:	
		{
			// HP
			ResetDestParam( DST_KILL_HP, nAdjParameterValue, TRUE );
			// MP
			SetDestParam( DST_KILL_MP, nAdjParameterValue, TRUE );
			// FP
			SetDestParam( DST_KILL_FP, nAdjParameterValue, TRUE );
		}
		return;
	case DST_KILL_HP_RATE:
		{
			int nMax	= GetMaxOriginHitPoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_KILL_HP, nRecv, TRUE );
		}
		return;
	case DST_KILL_MP_RATE:
		{
			int nMax	= GetMaxOriginManaPoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_KILL_MP, nRecv, TRUE );
		}
		return;
	case DST_KILL_FP_RATE:
		{
			int nMax	= GetMaxOriginFatiguePoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_KILL_FP, nRecv, TRUE );
		}
		return;
	case DST_KILL_ALL_RATE:
		{
			// HP
			int nMax	= GetMaxOriginHitPoint();		// 버프의 영향을 받지 않은 오리지날 양
			int nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_KILL_HP, nRecv, TRUE );
			// MP
			nMax	= GetMaxOriginManaPoint();		// 버프의 영향을 받지 않은 오리지날 양
			nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_KILL_MP, nRecv, TRUE );
			// FP
			nMax	= GetMaxOriginFatiguePoint();		// 버프의 영향을 받지 않은 오리지날 양
			nRecv	= (int)( (nMax * (nAdjParameterValue / 100.0f)) );
			ResetDestParam( DST_KILL_FP, nRecv, TRUE );
		}
		return;
	case DST_ALL_DEC_RATE:
		{
			SetDestParam( DST_MP_DEC_RATE, nAdjParameterValue, TRUE );
			SetDestParam( DST_FP_DEC_RATE, nAdjParameterValue, TRUE );
		}
	}


	// 이 검사를 왜 위에다 안넣고 여기다 넣었냐면
	// DST_GOLD, PXP, RESIST_ALL같은경우는 MAX_ADJPARAMARY보다 더 큰값을 사용한다. 
	// 그래서 그런것들은 위에서 처리하고 바로 리턴하고
	// 일반적인것들만 이쪽으로 온다.
	
	if( nDstParameter >= MAX_ADJPARAMARY || nDstParameter < 0 )
	{
//		Error( "ResetDestParam %s : 잘못된 값 nDstParam : %d", m_szName, nDstParameter );
		return;
	}
	if( nAdjParameterValue != 0 ) 
	{
		if( nDstParameter == DST_CHRSTATE )
			m_adjParamAry[nDstParameter] &= (~nAdjParameterValue);		// 해당비트 오프.
		else
			m_adjParamAry[nDstParameter] += (-nAdjParameterValue);
	}

	// 尹
	if( nAdjParameterValue == 0 )	//
	{
		if( nDstParameter == DST_SPEED && m_chgParamAry[nDstParameter] != 0x7FFFFFFF && m_chgParamAry[nDstParameter] > 0 )
		{
			if( --m_chgParamAry[nDstParameter] == 0 )
				m_chgParamAry[nDstParameter]		= 0x7FFFFFFF;
		}
		else
		{
			m_chgParamAry[nDstParameter]		= 0x7FFFFFFF;
		}
	}
#ifdef __WORLDSERVER
	if( fSend == TRUE ) 
	{
	#ifdef	__SPEED_SYNC_0108 // ResetDestParam speed 수정
		if( nDstParameter == DST_SPEED )
			g_UserMng.AddResetDestParamSync( this, nDstParameter, nAdjParameterValue, (int)m_adjParamAry[nDstParameter] );
		else
			g_UserMng.AddResetDestParam( this, nDstParameter, nAdjParameterValue );
	#else	// 	__SPEED_SYNC_0108 ResetDestParam speed 수정
		g_UserMng.AddResetDestParam( this, nDstParameter, nAdjParameterValue );
	#endif	// 	__SPEED_SYNC_0108 ResetDestParam speed 수정
	}
#endif	// __WORLDSERVER
}

#ifdef __SPEED_SYNC_0108		// ResetDestParam speed 수정
void CMover::ResetDestParamSync( int nDstParameter,int nAdjParameterValue, int nParameterValue, BOOL fSend )
{
#ifdef __CLIENT
	if( nDstParameter >= MAX_ADJPARAMARY || nDstParameter < 0 )
	{
//		Error( "ResetDestParam %s : 잘못된 값 nDstParam : %d", m_szName, nDstParameter );
		return;
	}

	switch( nDstParameter )
	{
		case DST_SPEED:
			{
				m_adjParamAry[nDstParameter] = nParameterValue;
				if( nAdjParameterValue == 0 && m_chgParamAry[nDstParameter] != 0x7FFFFFFF && m_chgParamAry[nDstParameter] > 0 )
				{
					if( --m_chgParamAry[nDstParameter] == 0 )
						m_chgParamAry[nDstParameter]		= 0x7FFFFFFF;
				}
			}
			break;
	}
#endif	// __CLIENT
}
#endif // __SPEED_SYNC_0108		// ResetDestParam speed 수정

// 수정 파라미터를 얻는다.
int CMover::GetAdjParam( int nDestParameter )
{
	if( nDestParameter < MAX_ADJPARAMARY )
		return m_adjParamAry[ nDestParameter ];
	return 0;
}
// 교체 파라미터를 얻는다.
int CMover::GetChgParam( int nDestParameter )
{
	if( nDestParameter < MAX_ADJPARAMARY )
		return m_chgParamAry[ nDestParameter ];
	return 0x7FFFFFFF;
}

// 파라미터를 얻는다.
int CMover::GetParam( int nDest, int nParam )
{
	int nChgParam = GetChgParam( nDest );
	if( nChgParam != 0x7FFFFFFF ) 
		return nChgParam;

	int nAdjParam = GetAdjParam( nDest );
	if( nAdjParam )	
		return nParam + nAdjParam;
	else
		return nParam;
}

/////////////////////////////////////////////////////////////////////////////
// 여기부터 파라메타 얻기 함수들 
// GetParam을 사용하여 수정된 값을 내준다.
//
// 히트 포인트 얻기 
int CMover::GetHitPoint()
{
	return GetParam( DST_HP, m_nHitPoint );
}
// 마나 포인트 얻기 
int CMover::GetManaPoint()
{
#ifdef __EVENT1206
	if( IsPlayer() && g_eLocal.GetState( EVE_EVENT1206 ) )  // raiders_fix051217
		return GetMaxManaPoint();
#endif	// __EVENT1206
	return GetParam( DST_MP, m_nManaPoint );
}
// 피지칼 포인트 얻기
int CMover::GetFatiguePoint()
{
#ifdef __EVENT1206
	if(  IsPlayer() && g_eLocal.GetState( EVE_EVENT1206 ) )	// raiders_fix051217
		return GetMaxFatiguePoint();
#endif	// __EVENT1206
	return GetParam( DST_FP, m_nFatiguePoint );
}

// 최대 히트 포인트 얻기 
int CMover::GetMaxHitPoint()
{
	if( IsInvalidObj(this) )	
		return 0;

	float factor = 1.0f;
	int nResult = GetParam( DST_HP_MAX, GetMaxOriginHitPoint( FALSE ) );
	int nFactor = GetParam( DST_HP_MAX_RATE, 0 );

	factor += (float)nFactor/(float)100;
	nResult	= (int)( nResult * factor );

#ifdef __JEFF_11
	if( nResult < 1 )
		nResult		= 1;
#endif	// __JEFF_11
	return nResult; 
}

// 최대 마나 포인트 얻기 
int CMover::GetMaxManaPoint()
{
	float factor = 1.0f;
	int nResult = GetParam( DST_MP_MAX, GetMaxOriginManaPoint( FALSE ) );
	int nFactor = GetParam( DST_MP_MAX_RATE, 0 );
	
	factor += (float)nFactor/(float)100;
	nResult	= (int)( nResult * factor );

#ifdef __JEFF_11
	if( nResult < 1 )
		nResult		= 1;
#endif	// __JEFF_11
	return nResult; 
}

// 최대 피로 포인트 얻기
int CMover::GetMaxFatiguePoint()
{
	float factor = 1.0f;
	int nResult = GetParam( DST_FP_MAX, GetMaxOriginFatiguePoint( FALSE ) );
	int nFactor = GetParam( DST_FP_MAX_RATE, 0 );
	
	factor += (float)nFactor/(float)100;
	nResult	= (int)( nResult * factor );

#ifdef __JEFF_11
	if( nResult < 1 )
		nResult		= 1;
#endif	// __JEFF_11
	return nResult; 
}

int CMover::GetMaxPoint(int nDest)
{
	switch( nDest )
	{
		case DST_HP:
			return GetMaxHitPoint();
		case DST_MP:
			return GetMaxManaPoint();
		case DST_FP:
			return GetMaxFatiguePoint();
	}
	
	return 0;
}

int CMover::GetMaxOriginHitPoint( BOOL bOriginal )
{
	if( IsInvalidObj(this) )	
		return 0;

	if( IsPlayer() )
	{
		int nSta;
		if( bOriginal )
			nSta = m_nSta;
		else
			nSta = GetSta();

		JobProp* pProperty = prj.GetJobProp( GetJob() ); 

		float a = (pProperty->fFactorMaxHP*m_nLevel)/2.0f;
		float b = a * ((m_nLevel+1.0f)/4.0f) * (1.0f + nSta/50.0f) + (nSta*10.0f) ;
		float maxHP = b + 80.f; 
		return (int)maxHP;
	}
	else
	{
		MoverProp *pMoverProp = GetProp();
#ifdef __S1108_BACK_END_SYSTEM
		return int( pMoverProp->dwAddHp * prj.m_fMonsterHitpointRate * pMoverProp->m_fHitPoint_Rate ); 
#else // __S1108_BACK_END_SYSTEM
		return pMoverProp->dwAddHp; 
#endif // __S1108_BACK_END_SYSTEM
	}

	return 0;
}

int CMover::GetMaxOriginManaPoint( BOOL bOriginal )
{
	int nInt = 0;
	if( bOriginal )
		nInt = m_nInt;
	else
		nInt = GetInt();

	if( IsPlayer() )
	{
		// INT((((BaseLv*2) + (INT*8))*Job계수) + 22)+(INT*Job계수)) +  ① 
		// ① : 아이템에의한 추가 상승 (%단위) TDDO
		JobProp* pProperty = prj.GetJobProp( GetJob() ); 
		float factor = pProperty->fFactorMaxMP;
		
		int nMaxMP = (int)( ((((m_nLevel*2.0f) + ( nInt*8.0f))*factor) + 22.0f)+( nInt*factor) );
		return nMaxMP;
	}
	return 	( ( m_nLevel * 2 ) + ( nInt * 8 ) + 22 );
}

int CMover::GetMaxOriginFatiguePoint( BOOL bOriginal )
{
	int nSta, nStr, nDex;
	if( bOriginal )
	{
		nSta = m_nSta;
		nStr = m_nStr;
		nDex = m_nDex;
	}
	else
	{
		nSta = GetSta();
		nStr = GetStr();
		nDex = GetDex();
	}

	if( IsPlayer() )
	{
		// (((BaseLv*2) + (STA*6))*Job계수) +(STA*Job계수) + ① + ②
		JobProp* pProperty = prj.GetJobProp( GetJob() ); 
		float factor = pProperty->fFactorMaxFP;

		int nMaxFP = (int)( (((m_nLevel*2.0f) + (nSta*6.0f))*factor) +(nSta*factor) );
		return nMaxFP;
	}
	
	return ( ( m_nLevel * 2 ) + ( nStr * 7 ) + ( nSta *2 ) + ( nDex * 4 ) );
}

int CMover::GetHitPointPercent( int nPercent )
{
	int nMax = GetMaxHitPoint();
	if( nMax == 0 ) return 0;
//	return GetHitPoint() * nPercent / nMax;
	return MulDiv( GetHitPoint(), nPercent, nMax );
}
int CMover::GetManaPointPercent( int nPercent )
{
	int nMax = GetMaxManaPoint();
	if( nMax == 0 ) return 0;
//	return GetManaPoint() * nPercent / GetMaxManaPoint();
	return MulDiv( GetManaPoint(), nPercent, nMax );
}
int CMover::GetFatiguePointPercent( int nPercent )
{
	int nMax = GetMaxFatiguePoint();
	if( nMax == 0 ) return 0;
//	return GetFatiguePoint() * nPercent / GetMaxFatiguePoint();
	return MulDiv( GetFatiguePoint(),  nPercent, nMax );
}

// HP 회복 값 얻기
/*
	HP Recovery : 회복량	PC/NPC : int(((현재레벨 + STA)/3.4)+2) + AddHpRecovery
	MP Recovery : 회복량	PC/NPC : int(((현재레벨 + INT)/4)+2) + Addecovery
	FP Recovery : 회복량	PC/NPC : int(((현재레벨 + STA + DEX)/4.5)+2) + AddHpRecovery

	HP Recovery : 회복속도	PC/NPC : 휴식(정지)시 5초에 한번씩 회복하며 휴식하지 않는 상황에서는 12초에 한번씩 회복함 - AddHpRecoveryTime
	MP Recovery : 회복속도	PC/NPC : 휴식(정지)시 5초에 한번씩 회복하며 휴식하지 않는 상황에서는 12초에 한번씩 회복함 - AddMpRecoveryTime
	FP Recovery : 회복속도	PC/NPC : 휴식(정지)시 5초에 한번씩 회복하며 휴식하지 않는 상황에서는 12초에 한번씩 회복함 - AddFpRecoveryTime

  */
int CMover::GetHPRecovery()
{
	float fFactor = 1.0f;
	if( IsPlayer() )
	{
		JobProp* pProperty = prj.GetJobProp( GetJob() ); 
		fFactor = pProperty->fFactorHPRecovery;
	}

	//(레벨/3)+ (MaxHP/ (500* 레벨))+(STA*Job계수)
	int nValue = (int)( (GetLevel() / 3.0f) + (GetMaxHitPoint() / (500.f * GetLevel())) + (GetSta() * fFactor) );
#if __VER >= 9 // __RECOVERY10
	nValue	= (int)( nValue - ( nValue * 0.1f ) ); // 회복량 10% 하향
#endif //__RECOVERY10
	return GetParam( DST_HP_RECOVERY, nValue );
}
// MP 회복 값 얻기
int CMover::GetMPRecovery()
{
	float fFactor = 1.0f;
	if( IsPlayer() )
	{
		JobProp* pProperty = prj.GetJobProp( GetJob() ); 
		fFactor = pProperty->fFactorMPRecovery;
	}

	// ((레벨*1.5)+(MaxMP/(500*레벨))+(INT*Job계수))*0.2
	int nValue = (int)( ((GetLevel() * 1.5f) + (GetMaxManaPoint() / (500.f * GetLevel())) + (GetInt() * fFactor)) * 0.2f );
#if __VER >= 9 // __RECOVERY10
	nValue	= (int)( nValue - ( nValue * 0.1f ) ); // 회복량 10% 하향
#endif //__RECOVERY10
	return GetParam( DST_MP_RECOVERY, nValue );
}
// FP 회복 값 얻기 
int CMover::GetFPRecovery()
{
	float fFactor = 1.0f;
	if( IsPlayer() )
	{
		JobProp* pProperty = prj.GetJobProp( GetJob() ); 
		fFactor = pProperty->fFactorFPRecovery;
	}
	// ((레벨*2)+(MaxFP/(500*레벨))+(STA*Job계수))*0.2
	int nValue = (int)( ((GetLevel() * 2.0f) + (GetMaxFatiguePoint() / (500.f * GetLevel())) + (GetSta() * fFactor)) * 0.2f );
#if __VER >= 9 // __RECOVERY10
	nValue	= (int)( nValue - ( nValue * 0.1f ) ); // 회복량 10% 하향
#endif //__RECOVERY10
	return GetParam( DST_FP_RECOVERY, nValue );
}

#ifdef __CLIENT
// 명성 이름을 스트링으로 돌려준다.
LPCTSTR CMover::GetFameName( void )
{
	switch( GetJob() )
	{
	// 아크로뱃
	case JOB_ACROBAT:
	case JOB_JESTER:
	case JOB_RANGER:
	case JOB_JESTER_MASTER:
	case JOB_RANGER_MASTER:
	case JOB_JESTER_HERO:
	case JOB_RANGER_HERO:
		if( m_nFame >= 100000000 )	return prj.GetText( TID_GAME_ACR_FAME10 );
		else if( m_nFame >= 5000000 )	return prj.GetText( TID_GAME_ACR_FAME09 );
		else if( m_nFame >= 1000000 )	return prj.GetText( TID_GAME_ACR_FAME08 );
		else if( m_nFame >= 50000 )	return prj.GetText( TID_GAME_ACR_FAME07 );
		else if( m_nFame >= 20000 )	return prj.GetText( TID_GAME_ACR_FAME06 );
		else if( m_nFame >= 10000 )	return prj.GetText( TID_GAME_ACR_FAME05 );
		else if( m_nFame >= 4000 )	return prj.GetText( TID_GAME_ACR_FAME04 );
		else if( m_nFame >= 1000 )	return prj.GetText( TID_GAME_ACR_FAME03 );
		else if( m_nFame >= 100 )	return prj.GetText( TID_GAME_ACR_FAME02 );
		else if( m_nFame >= 10 )	return prj.GetText( TID_GAME_ACR_FAME01 );
		break;

	// 머서너리일때
	case JOB_MERCENARY:	
	case JOB_KNIGHT:	
	case JOB_BLADE:
	case JOB_KNIGHT_MASTER:
	case JOB_BLADE_MASTER:
	case JOB_KNIGHT_HERO:	
	case JOB_BLADE_HERO:
		if( m_nFame >= 100000000 )	return prj.GetText( TID_GAME_MER_FAME10 );
		else if( m_nFame >= 5000000 )	return prj.GetText( TID_GAME_MER_FAME09 );
		else if( m_nFame >= 1000000 )	return prj.GetText( TID_GAME_MER_FAME08 );
		else if( m_nFame >= 50000 )	return prj.GetText( TID_GAME_MER_FAME07 );
		else if( m_nFame >= 20000 )	return prj.GetText( TID_GAME_MER_FAME06 );
		else if( m_nFame >= 10000 )	return prj.GetText( TID_GAME_MER_FAME05 );
		else if( m_nFame >= 4000 )	return prj.GetText( TID_GAME_MER_FAME04 );
		else if( m_nFame >= 1000 )	return prj.GetText( TID_GAME_MER_FAME03 );
		else if( m_nFame >= 100 )	return prj.GetText( TID_GAME_MER_FAME02 );
		else if( m_nFame >= 10 )	return prj.GetText( TID_GAME_MER_FAME01 );
		break;
	// 매지션
	case JOB_MAGICIAN:
	case JOB_PSYCHIKEEPER:	
	case JOB_ELEMENTOR:
	case JOB_PSYCHIKEEPER_MASTER:	
	case JOB_ELEMENTOR_MASTER:
	case JOB_PSYCHIKEEPER_HERO:	
	case JOB_ELEMENTOR_HERO:

		if( m_nFame >= 100000000 )	return prj.GetText( TID_GAME_MAG_FAME10 );
		else if( m_nFame >= 5000000 )	return prj.GetText( TID_GAME_MAG_FAME09 );
		else if( m_nFame >= 1000000 )	return prj.GetText( TID_GAME_MAG_FAME08 );
		else if( m_nFame >= 50000 )	return prj.GetText( TID_GAME_MAG_FAME07 );
		else if( m_nFame >= 20000 )	return prj.GetText( TID_GAME_MAG_FAME06 );
		else if( m_nFame >= 10000 )	return prj.GetText( TID_GAME_MAG_FAME05 );
		else if( m_nFame >= 4000 )	return prj.GetText( TID_GAME_MAG_FAME04 );
		else if( m_nFame >= 1000 )	return prj.GetText( TID_GAME_MAG_FAME03 );
		else if( m_nFame >= 100 )	return prj.GetText( TID_GAME_MAG_FAME02 );
		else if( m_nFame >= 10 )	return prj.GetText( TID_GAME_MAG_FAME01 );
		break;

	// 어시스트
	case JOB_ASSIST:
	case JOB_BILLPOSTER:	
	case JOB_RINGMASTER:
	case JOB_BILLPOSTER_MASTER:	
	case JOB_RINGMASTER_MASTER:
	case JOB_BILLPOSTER_HERO:	
	case JOB_RINGMASTER_HERO:
		if( m_nFame >= 100000000 )	return prj.GetText( TID_GAME_ASS_FAME10 );
		else if( m_nFame >= 5000000 )	return prj.GetText( TID_GAME_ASS_FAME09 );
		else if( m_nFame >= 1000000 )	return prj.GetText( TID_GAME_ASS_FAME08 );
		else if( m_nFame >= 50000 )	return prj.GetText( TID_GAME_ASS_FAME07 );
		else if( m_nFame >= 20000 )	return prj.GetText( TID_GAME_ASS_FAME06 );
		else if( m_nFame >= 10000 )	return prj.GetText( TID_GAME_ASS_FAME05 );
		else if( m_nFame >= 4000 )	return prj.GetText( TID_GAME_ASS_FAME04 );
		else if( m_nFame >= 1000 )	return prj.GetText( TID_GAME_ASS_FAME03 );
		else if( m_nFame >= 100 )	return prj.GetText( TID_GAME_ASS_FAME02 );
		else if( m_nFame >= 10 )	return prj.GetText( TID_GAME_ASS_FAME01 );		

		break;
	}
	return "";
}

#if __VER < 8 // __S8_PK
// 슬로터 이름을 스트링으로 돌려준다.
LPCTSTR CMover::GetSlaughterName( void )
{
	KarmaProp* pProp = prj.GetKarmaProp( m_nSlaughter );

	if( pProp )
	{
		if(strlen(pProp->szName) > 0 )
			return pProp->szName;
	}	

	return "";
}
#endif // __VER < 8 // __S8_PK

#endif


// 마법 저향력 얻기 
int CMover::GetResistMagic()
{
	return GetParam( DST_RESIST_MAGIC, 0 );
}
int CMover::GetResistSpell( int nDestParam )
{
	MoverProp *pProp = GetProp();
	if( pProp == NULL )
		Error( "CMover::GetReistSpell : %d 프로퍼티 읽기 실패", GetName() );

	int		nResist = 0;

	switch( nDestParam )
	{
	case DST_RESIST_ELECTRICITY:	nResist = pProp->nResistElecricity;		break;
	case DST_RESIST_FIRE:			nResist = pProp->nResistFire;		break;
	case DST_RESIST_WATER:			nResist = pProp->nResistWater;		break;
	case DST_RESIST_EARTH:			nResist = pProp->nResistEarth;		break;
	case DST_RESIST_WIND:			nResist = pProp->nResistWind;		break;
	default:
		Error( "CMover::GetResistSpell : %s 파라메터 잘못됨 %d", GetName(), nDestParam );
		break;
	}
	return GetParam( nDestParam, nResist );
}

int CMover::GetNaturalArmor()
{
	return GetProp()->dwNaturalArmor;//GetParam(DST_NATURAL_ARMOR,GetProp()->m_dwNaturalArmor);
}

int CMover::GetStr()
{
	int nResult = m_nStr + GetParam( DST_STR, 0 );

#if __VER < 8 // __S8_PK
	// 시련모드이고 시련제한상태이면 페널티 적용
	if( IsMode( RECOVERCHAO_MODE ) && IsSMMode(SM_STR_DOWN) )
	{
		KarmaProp* pKarmaProp = prj.GetKarmaProp(m_nSlaughter);
		if( pKarmaProp )
			nResult -= (int)(nResult * (pKarmaProp->nStatLimitRate / 100.0f));
	}
#endif // __VER < 8 // __S8_PK

#ifdef __JEFF_11
	if( nResult < 1 )
		nResult	= 1;
#endif	// __JEFF_11

	return nResult;
}
int CMover::GetDex()
{
	int nResult = m_nDex + GetParam( DST_DEX, 0 );

#if __VER < 8 // __S8_PK
	// 시련모드이고 시련제한상태이면 페널티 적용
	if( IsMode( RECOVERCHAO_MODE ) && IsSMMode(SM_DEX_DOWN) )
	{
		KarmaProp* pKarmaProp = prj.GetKarmaProp(m_nSlaughter);
		if( pKarmaProp )
			nResult -= (int)(nResult * (pKarmaProp->nStatLimitRate / 100.0f));
	}
#endif // __VER < 8 // __S8_PK

#ifdef __JEFF_11
	if( nResult < 1 )
		nResult	= 1;
#endif	// __JEFF_11

	return nResult;
}
int CMover::GetInt()
{
	int nResult = m_nInt + GetParam( DST_INT, 0 );

#if __VER < 8 // __S8_PK
	// 시련모드이고 시련제한상태이면 페널티 적용
	if( IsMode( RECOVERCHAO_MODE ) && IsSMMode(SM_INT_DOWN) )
	{
		KarmaProp* pKarmaProp = prj.GetKarmaProp(m_nSlaughter);
		if( pKarmaProp )
			nResult -= (int)(nResult * (pKarmaProp->nStatLimitRate / 100.0f));
	}
#endif // __VER < 8 // __S8_PK

#ifdef __JEFF_11
	if( nResult < 1 )
		nResult	= 1;
#endif	// __JEFF_11

	return nResult;
}
int CMover::GetSta()
{
	int nResult = m_nSta + GetParam( DST_STA, 0 );

#if __VER < 8 // __S8_PK
	// 시련모드이고 시련제한상태이면 페널티 적용
	if( IsMode( RECOVERCHAO_MODE ) && IsSMMode(SM_STA_DOWN) )
	{
		KarmaProp* pKarmaProp = prj.GetKarmaProp(m_nSlaughter);
		if( pKarmaProp )
			nResult -= (int)(nResult * (pKarmaProp->nStatLimitRate / 100.0f));
	}
#endif // __VER < 8 // __S8_PK

#ifdef __JEFF_11
	if( nResult < 1 )
		nResult	= 1;
#endif	// __JEFF_11

	return nResult;
}

FLOAT CMover::GetSpeed(FLOAT fSrcSpeed)
{
#if __VER >= 9	// __PET_0410
#ifdef __CLIENT
	if( m_dwAIInterface == AII_EGG && m_pAIInterface )
	{
		CAIEgg* pAI	= (CAIEgg*)m_pAIInterface;
		CMover* pOwner	= prj.GetMover( pAI->GetOwnerId() );
		if( IsValidObj( pOwner ) )
			return pOwner->GetSpeed( pOwner->m_pActMover->m_fSpeed );
	}
#endif	// __CLIENT
#endif	// __PET_0410

#if __VER >= 9	//__AI_0509
	fSrcSpeed	*= GetSpeedFactor();
#endif	// __AI_0509

	if( m_fCrrSpd != 0.0f )
		return m_fCrrSpd;
	int nChgValue	= GetChgParam( DST_SPEED );
//	if( nChgValue != 0x7FFFFFFF )
//		return fSrcSpeed * (nChgValue / 100.0f);		// ex) fSrcSpeed * 0.5f

	// 尹
	if( nChgValue != 0x7FFFFFFF )
		return 0;

	int nAdjValue	= GetAdjParam( DST_SPEED );
	if( nAdjValue != 0 )
	{
#ifdef __JEFF_11
		fSrcSpeed = fSrcSpeed + ( fSrcSpeed * (nAdjValue / 100.0f) );
		if( fSrcSpeed < 0.0F )
			fSrcSpeed	= 0.0F;
#endif	// __JEFF_11
		return fSrcSpeed;
	}
#ifdef __JEFF_11
	if( fSrcSpeed < 0.0F )
		fSrcSpeed	= 0.0F;
#endif	// __JEFF_11

	return fSrcSpeed;
}




//
// 현재 동작상태를 강제로 세팅
// 동기화 이외는 사용하지 말것.
//

// 행위 명령은 강제 동기화 하고 이동 명령은 무시한다.
void CMover::BehaviorActionForceSet( void )
{
	if( m_CorrAction.fValid && m_CorrAction.fBehavior == TRUE ) {
		if( m_CorrAction.dwStateFlag & OBJSTAF_FLY ) {
			ActionForceSet2( m_CorrAction.v, m_CorrAction.vd, m_CorrAction.f,
								m_CorrAction.fAngleX, m_CorrAction.fAccPower, m_CorrAction.fTurnAngle,
								m_CorrAction.dwState, m_CorrAction.dwStateFlag,
								m_CorrAction.dwMotion, m_CorrAction.nMotionEx, m_CorrAction.nLoop, m_CorrAction.dwMotionOption );
		}
		else {
			ActionForceSet( m_CorrAction.v, m_CorrAction.vd, m_CorrAction.f,
								m_CorrAction.dwState, m_CorrAction.dwStateFlag,
								m_CorrAction.dwMotion, m_CorrAction.nMotionEx, m_CorrAction.nLoop, m_CorrAction.dwMotionOption );
		}
	}
	memset( &m_CorrAction, 0, sizeof(CORR_ACTION) );
}

void CMover::ActionForceSet( D3DXVECTOR3 &vPos, D3DXVECTOR3 &vDelta, FLOAT fAngle, 
							 DWORD dwState, DWORD dwStateFlag, int nMotion, int nMotionEx, int nLoop, DWORD dwMotionOption )
{
	m_uRemnantCorrFrm	= 0;
	if( m_pActMover == NULL )	return;
	CModelObject* pModel = (CModelObject*)m_pModel;
#ifdef __SYNC_1217
	ClearDestPos();
#else	// __SYNC_1217
	ClearDest();
#endif	// __SYNC_1217
	ClearDestAngle();
	SetPos( vPos );
	SetAngle( fAngle );
	m_pActMover->ForcedSet( vDelta, dwState, dwStateFlag, nMotionEx );
	m_dwMotion = -1;
	SetMotion( nMotion, nLoop, dwMotionOption );
	pModel->SetMotionBlending( FALSE );
}

void CMover::ActionForceSet2( D3DXVECTOR3 &vPos, D3DXVECTOR3 &vDelta, FLOAT fAngle, 
							 FLOAT fAngleX, FLOAT fAccPower, FLOAT fTurnAngle,
							 DWORD dwState, DWORD dwStateFlag, int nMotion, int nMotionEx, int nLoop, DWORD dwMotionOption )
{
	m_uRemnantCorrFrm	= 0;
	if( m_pActMover == NULL )	return;
	CModelObject* pModel = (CModelObject*)m_pModel;
	ClearDest();
	ClearDestAngle();
	SetPos( vPos );
	SetAngle( fAngle );
	SetAngleX( fAngleX );
	m_pActMover->ForcedSet2( vDelta, fAccPower, fTurnAngle, dwState, dwStateFlag, nMotionEx );
	m_dwMotion = -1;
#ifdef __WORLDSERVER	
	if( nMotion == 31 && (dwMotionOption & MOP_FIXED) == 0 )
		dwMotionOption |= MOP_FIXED;	
#endif

	SetMotion( nMotion, nLoop, dwMotionOption );
	pModel->SetMotionBlending( FALSE );
}

void CMover::HalfForceSet( D3DXVECTOR3 & vPos, D3DXVECTOR3 & vd, float fAngle, float fAngleX, float fAccPower, float fTurnAngle )
{
	m_uRemnantCorrFrm	= 0;
	if( m_pActMover == NULL )	return;
	ClearDest();
	ClearDestAngle();
	SetPos( vPos );
	SetAngle( fAngle );
	SetAngleX( fAngleX );
	m_pActMover->HalfForcedSet( vd, fAccPower, fTurnAngle );
}

BOOL CMover::IsCompleteQuest( int nQuestId )
{
	for( int i = 0; i < m_nCompleteQuestSize; i++ )
	{
		if( m_aCompleteQuest[ i ] == nQuestId )
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CMover::MakeCompleteQuest( int nQuestId, LPQUEST lpQuest )
{
	for( int i = 0; i < m_nCompleteQuestSize; i++ )
	{
		if( m_aCompleteQuest[ i ] == nQuestId )
		{
			ZeroMemory( lpQuest, sizeof( QUEST ) );
			lpQuest->m_wId = nQuestId;
			lpQuest->m_nState = QS_END;
			return TRUE;
		}
	}
	return FALSE;
}
void CMover::RemoveAllQuest()
{
	m_nQuestSize = 0;
	m_nCompleteQuestSize = 0;
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	m_nCheckedQuestSize = 0;
#endif // __IMPROVE_QUEST_INTERFACE
}
void CMover::RemoveCompleteQuest()
{
	m_nCompleteQuestSize = 0;
}


LPQUEST CMover::FindQuest( int nQuestId )
{
	for( int i = 0; i < m_nQuestSize; i++ )
	{
		if( m_aQuest[ i ].m_wId == nQuestId )
		{
			return &m_aQuest[ i ];
		}
	}

	return NULL;
}
BOOL CMover::RemoveQuest( int nQuestId )
{
	QuestProp* pProp = prj.m_aPropQuest.GetAt( nQuestId );

	for( int i = 0; i < m_nQuestSize; i++ )
	{
		if( m_aQuest[ i ].m_wId == nQuestId )
		{
			for( ; i < m_nQuestSize - 1; i++ )
			{
				m_aQuest[ i ] = m_aQuest[ i + 1 ];
			}
			m_nQuestSize--;
			break;
		}
	}
	for( i = 0; i < m_nCompleteQuestSize; i++ )
	{
		if( m_aCompleteQuest[ i ] == nQuestId )
		{
			for( ; i < m_nCompleteQuestSize - 1; i++ )
			{
				m_aCompleteQuest[ i ] = m_aCompleteQuest[ i + 1 ];
			}
			m_nCompleteQuestSize--;
			break;
		}
	}
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	for( i = 0; i < m_nCheckedQuestSize; ++i )
	{
		if( m_aCheckedQuest[ i ] == nQuestId )
		{
			for( ; i < m_nCheckedQuestSize -1; ++i )
				m_aCheckedQuest[ i ] = m_aCheckedQuest[ i + 1 ];
			m_aCheckedQuest[ --m_nCheckedQuestSize ] = 0;
			break;
		}
	}
#endif // __IMPROVE_QUEST_INTERFACE

#ifdef __CLIENT
	PlayMusic( BGM_EV_END );
#endif
	return TRUE;
}

BOOL CMover::SetQuest( int nQuestId, int nState, LPQUEST lpReturnQuest )
{
	QUEST newQuest;
	ZeroMemory( &newQuest, sizeof( QUEST ) );
	newQuest.m_wId = nQuestId;
	newQuest.m_nState = nState;

	LPQUEST lpQuest = FindQuest( nQuestId );
	// 이미 완료된 케스트라면 세팅 실패 
	if( lpQuest == NULL && IsCompleteQuest( newQuest.m_wId ) )
		return FALSE;
	if( lpReturnQuest )
		memcpy( lpReturnQuest, &newQuest, sizeof( QUEST ) );
	return __SetQuest( lpQuest, &newQuest );
}
BOOL CMover::SetQuest( LPQUEST lpNewQuest )
{
	LPQUEST lpQuest = FindQuest( lpNewQuest->m_wId );
	// 이미 완료된 케스트라면 세팅 실패 
	if( lpQuest == NULL && IsCompleteQuest( lpNewQuest->m_wId ) )
		return FALSE;
	return __SetQuest( lpQuest, lpNewQuest );
}
BOOL CMover::__SetQuest( LPQUEST lpQuest, LPQUEST lpNewQuest )
{
	// 퀘스트를 발견 못했으면 새로 추가한다.
	if( lpQuest == NULL )
	{
		if( m_nQuestSize >= MAX_QUEST )
		{
			Error("SetQuestCnt : 퀘스트 초과");
			return FALSE;
		}
#ifdef __CLIENT
		PlayMusic( BGM_EV_START );
#endif
		if( m_nCompleteQuestSize < MAX_COMPLETE_QUEST ) 
			lpQuest = &m_aQuest[ m_nQuestSize++ ];
		else
			return FALSE;

		QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( lpNewQuest->m_wId );
		if( pQuestProp && pQuestProp->m_nEndCondLimitTime  )
			lpNewQuest->m_wTime = pQuestProp->m_nEndCondLimitTime;
#ifdef __WORLDSERVER
		// 타이머 초기화 
		m_timerQuestLimitTime.Reset();
#endif
	}
	else
	// 퀘스트 완료다. 그렇다면 캐쉬에서 뒤져서 제거하자 
	if( lpNewQuest->m_nState == QS_END )
	{
		// 대신, 완료 궤스트 배열에 넣어 보관한다.
		if( m_nCompleteQuestSize < MAX_COMPLETE_QUEST ) 
		{
			for( int i = 0; i < m_nQuestSize; i++ )
			{
				if( m_aQuest[ i ].m_wId == lpNewQuest->m_wId )
				{
					for( ; i < m_nQuestSize - 1; i++ )
					{
						m_aQuest[ i ] = m_aQuest[ i + 1 ];
					}
					m_nQuestSize--;
					break;
				}
			}
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
			for( i = 0; i < m_nCheckedQuestSize; ++i )
			{
				if( m_aCheckedQuest[ i ] == lpNewQuest->m_wId )
				{
					for( ; i < m_nCheckedQuestSize - 1; ++i )
						m_aCheckedQuest[ i ] = m_aCheckedQuest[ i + 1 ];
					m_aCheckedQuest[--m_nCheckedQuestSize] = 0;
					break;
				}
			}
#endif // __IMPROVE_QUEST_INTERFACE
			QuestProp* pProp = prj.m_aPropQuest.GetAt( lpNewQuest->m_wId );
			// 퀘스트 타입이 의뢰소라면 완료 배열에 들어가지 않는다. 
			//if( pProp->m_nQuestType != QT_REQUEST && pProp->m_bRepeat == FALSE )
			if( pProp && pProp->m_bRepeat == FALSE )
				m_aCompleteQuest[ m_nCompleteQuestSize++ ] = lpNewQuest->m_wId; 
			lpQuest = NULL;
#if __VER >= 15 /* __IMPROVE_QUEST_INTERFACE */  && defined( __CLIENT )
			g_QuestTreeInfoManager.DeleteTreeInformation( lpNewQuest->m_wId );
			D3DXVECTOR3& rDestinationArrow = g_WndMng.m_pWndWorld->m_vDestinationArrow;
			rDestinationArrow = D3DXVECTOR3( -1.0F, 0.0F, -1.0F );
#endif // defined( __IMPROVE_QUEST_INTERFACE ) && defined( __CLIENT )
		}
		else
			return FALSE;
	}
	if( lpQuest )
		memcpy( lpQuest, lpNewQuest, sizeof( QUEST ) );

#ifdef __CLIENT
	if( lpNewQuest->m_nState == QS_END )
		PlayMusic( BGM_EV_END );
#endif
	return TRUE;
}

LPQUEST CMover::GetQuest( int nQuestId )
{
	return FindQuest( nQuestId );
}

int CMover::GetItemNum( DWORD dwItemId )
{
	int nNum	= 0;
	int nSize	= m_Inventory.GetMax();
	CItemElem* pItemElem;
	for( int i = 0 ; i < nSize; i++ )
	{
		pItemElem	= m_Inventory.GetAtId( i );
		if( pItemElem )
		{
			//raiders.06.05.02 (prevent abuse quest ending condition : trade & quest )
			if( IsUsableItem( pItemElem ) == FALSE )
				return 0;
			if( pItemElem->m_dwItemId == dwItemId )
				nNum	+= pItemElem->m_nItemNum;
		}
	}
	return nNum;
}

#ifdef __CLIENT
int CMover::GetItemNumForClient( DWORD dwItemId )
{
	int nNum	= 0;
	int nSize	= m_Inventory.GetMax();
	CItemElem* pItemElem;
	for( int i = 0 ; i < nSize; i++ )
	{
		pItemElem	= m_Inventory.GetAtId( i );
		if( pItemElem )
		{
			if( pItemElem->m_dwItemId == dwItemId )
				nNum	+= pItemElem->m_nItemNum;
		}
	}
	return nNum;
}
#endif //__CLIENT
#ifdef __WORLDSERVER
// 스크립트에서 사용하는 함수
// 퀘스트에서 아이템을 제거할 때 사용한다. 퀘스트 아이템들은 거래나 개인상점에 올릴 수 없기 때문에
// IsUsableItem검사를 하지 않는다.
int CMover::RemoveItemA( DWORD dwItemId, short nNum )
{
	if( nNum == -1 )
	{
		return RemoveAllItem( dwItemId );
	}

	int	nSize	= m_Inventory.GetMax();
	CItemElem* pItemElem;
	short nRemnant	= nNum;

	for( int i = 0 ; i < nSize && nRemnant > 0; i++ )
	{
		pItemElem	= m_Inventory.GetAtId( i );
		if( pItemElem && pItemElem->m_dwItemId == dwItemId )
		{
			if( nRemnant > pItemElem->m_nItemNum )
			{
				UpdateItem( i, UI_NUM, 0 );	// remove
				nRemnant	-= pItemElem->m_nItemNum;
			}
			else
			{
				UpdateItem( i, UI_NUM, pItemElem->m_nItemNum - nRemnant );
				nRemnant	= 0;
			}
		}
	}
	return ( nNum - nRemnant );
}
#endif	// __WORLDSERVER

// 스크립트에서 사용하는 함수 
int CMover::RemoveAllItem( DWORD dwItemId )
{
	int nNum = 0;
	int	nSize	= m_Inventory.GetMax();

	for( int i = 0 ; i < nSize; ++i )
	{
		CItemElem* pItemElem = m_Inventory.GetAtId( i );
		if( pItemElem && pItemElem->m_dwItemId == dwItemId )
		{
			nNum += pItemElem->m_nItemNum;
			UpdateItem( i, UI_NUM, 0 );		// remove
		}
	}
	return nNum;
}

void CMover::OnTradeRemoveUser()
{
	if( IsPlayer() )
	{
		CMover* pOther = m_vtInfo.GetOther();
		if( IsValidObj( pOther ) && pOther->IsPlayer() )
		{	
			if( pOther->m_vtInfo.GetOther() == this )
			{
				pOther->m_vtInfo.TradeClear();
			}
		}
		m_vtInfo.TradeClear();
	}
}

#ifdef __WORLDSERVER

void CMover::Abrade( CMover* pAttacker, int nParts )
{
#if __VER < 11 // __REMOVE_ENDURANCE
	if( NULL == pAttacker )
		return;

	if( pAttacker->IsPlayer() )
	{
		CItemElem* pWeapon	= pAttacker->GetWeaponItem( nParts );
		if( pWeapon == NULL )
			return;

		ItemProp* pWeaponProp	= pWeapon->GetProp();
		if( pWeaponProp == NULL )
			return;
		
		int nWpn = pWeapon->m_nHitPoint;
		pWeapon->m_nHitPoint	-= ( pWeaponProp->nAbrasion  / 5 ) * GetItemEnduranceWeight(0);			// 뒤에서 추가로 깎지 않고 여기서 2번 곱했다.
		if( pWeapon->m_nHitPoint < 0 )		pWeapon->m_nHitPoint = 0;
		
		if( nWpn != pWeapon->m_nHitPoint )
			pAttacker->UpdateItem( pWeapon->m_dwObjId, UI_HP, pWeapon->m_nHitPoint );
		
		if( !IsPlayer() )	// PVP
			return;
		
		DWORD dwParts	= GetRandomPartsAbraded();
		CItemElem* pArmorElem	= m_Inventory.GetEquip( dwParts );
		if( pArmorElem )
		{
			ItemProp* pArmorProp		= pArmorElem->GetProp();
			if( pArmorProp && ( pArmorProp->dwItemKind2 == IK2_ARMOR || pArmorProp->dwItemKind2 == IK2_ARMORETC ))
			{
				int nArm = pArmorElem->m_nHitPoint;
				pArmorElem->m_nHitPoint		-= pArmorProp->nAbrasion * GetItemEnduranceWeight(0);
				if( pArmorElem->m_nHitPoint < 0 )	pArmorElem->m_nHitPoint	= 0;
				
				if( nArm != pArmorElem->m_nHitPoint )
					UpdateItem( pArmorElem->m_dwObjId, UI_HP, pArmorElem->m_nHitPoint );
			}
		}
	}
	else
	{
		if( !IsPlayer() )	// MVP
			return;

		DWORD dwParts	= GetRandomPartsAbraded();
		CItemElem* pArmorElem	= m_Inventory.GetEquip( dwParts );
		if( pArmorElem == NULL )
			return;
		
		ItemProp* pArmorProp		= pArmorElem->GetProp();
		if( pArmorProp && ( pArmorProp->dwItemKind2 == IK2_ARMOR || pArmorProp->dwItemKind2 == IK2_ARMORETC ) )
		{
			ItemProp* pWeaponProp	= pAttacker->GetActiveHandItemProp( nParts );
			if( pWeaponProp )
			{
				int nArm = pArmorElem->m_nHitPoint;
				pArmorElem->m_nHitPoint		-= pArmorProp->nAbrasion * GetItemEnduranceWeight(0);
				if( pArmorElem->m_nHitPoint < 0 )
					pArmorElem->m_nHitPoint	= 0;
				if( nArm != pArmorElem->m_nHitPoint )
					UpdateItem( pArmorElem->m_dwObjId, UI_HP, pArmorElem->m_nHitPoint );
			}
		}
	}
#endif // __REMOVE_ENDURANCE
}

DWORD CMover::GetRandomPartsAbraded( void )
{
//	실드 50%
	int nRand	= xRand();
	if( nRand % 2 )
		return PARTS_SHIELD;
	nRand	= nRand % 100;
	if( nRand < 20 )	// 20%
		return PARTS_CAP;
	else if( nRand < 60 )	// 40%
		return PARTS_UPPER_BODY;
	else if( nRand < 85 )	// 25%
		return PARTS_HAND;
	else	// 15%
		return PARTS_FOOT;
}
#endif	// __WORLDSERVER



float CMover::GetItemEnduranceInfluence( int nEndurance )
{
	if( nEndurance == 0 )
		return 0.0f;
/*	else if( nEndurance <= 50 )
		return 0.6f;
	else if( nEndurance <= 75 )
		return 0.8f;
*/	return 1.0f;
}

int CMover::GetItemEnduranceWeight( int nEndurance )
{
	int nMinusMultyPly = 5;
/*	if( nEndurance <= 50 )
		return (int)3 * nMinusMultyPly;
	else if( nEndurance <= 75 )
		return (int)2 * nMinusMultyPly;
*/	return 1 * nMinusMultyPly;
}

#if __VER >= 11 // __SYS_IDENTIFY
void CMover::UpdateItemEx( unsigned char id, char cParam, __int64 iValue )
{
	CItemElem* pItemElem	= (CItemElem*)GetItemId( id );

	if( pItemElem )
	{
		switch( cParam )
		{
			case UI_RANDOMOPTITEMID:
				{
					pItemElem->SetRandomOptItemId( iValue );
					break;
				}
			default:
				break;
		}
#ifdef __CLIENT
		CWndInventory* pWnd		= (CWndInventory*)g_WndMng.GetWndBase( APP_INVENTORY );
		if( pWnd )
			pWnd->UpdateTooltip();
#endif	// __CLIENT
	}
#ifdef __WORLDSERVER
	if( IsPlayer() )
		( (CUser*)this )->AddUpdateItemEx( id, cParam, iValue );
#endif
}
#endif	// __SYS_IDENTIFY

#if __VER >= 15 // __PETVIS
void CMover::UpdateItem( BYTE nId, CHAR cParam, DWORD dwValue, DWORD dwTime )
#else // __PETVIS
void CMover::UpdateItem( BYTE nId, CHAR cParam, DWORD dwValue )
#endif // __PETVIS
{
	CItemBase* pItemBase = GetItemId( nId );

	if( pItemBase )
	{
		switch( cParam )
		{
			case UI_COOLTIME:
			case UI_NUM:
				{
					if( (int)dwValue <= 0 ) 
					{
						RemoveItemId( nId );
					}
					else 
					{
						( (CItemElem*)pItemBase )->m_nItemNum	= (short)( dwValue );
					}
					break;
				}
			case UI_HP:
				{
					( (CItemElem*)pItemBase )->m_nHitPoint	= dwValue;
					UpdateParam();
					break;
				}
			case UI_REPAIR_NUM:
				{
					( (CItemElem*)pItemBase )->m_nRepair	= dwValue;
					break;
				}
			case UI_RN:
				{
					if( ( (CItemElem*)pItemBase )->m_nRepairNumber >= 100 )
						return;
					if( dwValue > 100 )
						dwValue		= 100;
					( (CItemElem*)pItemBase )->m_nRepairNumber	= (BYTE)( dwValue );
					UpdateParam();
				}
				break;
				
			case UI_AO: // 아이템 + 올리기...
				{
#if __VER >= 13 // __EXT_ENCHANT
					DWORD dwMax = 20;
#else // __EXT_ENCHANT
					DWORD dwMax	= 10;
#endif // __EXT_ENCHANT
#if __VER >= 11 // __SYS_COLLECTING
					if( ( (CItemElem*)pItemBase )->IsAccessory() )
						dwMax	= 20;
#endif	// __SYS_COLLECTING
#if __VER >= 11 // __SYS_IDENTIFY
					if( ( (CItemElem*)pItemBase )->IsCollector( TRUE ) )
						dwMax	= 5;
#endif	// __SYS_IDENTIFY
					if( ( (CItemElem*)pItemBase )->GetAbilityOption() > (int)( dwMax ) )
						return;
					if( dwValue > dwMax )
						dwValue		= dwMax;

					( (CItemElem*)pItemBase )->SetAbilityOption( dwValue );
					UpdateParam();
				}
				break;
			case UI_RAO: // 아이템 속성 + 올리기...
				{
#if __VER >= 13 // __EXT_ENCHANT
					if( ( (CItemElem*)pItemBase )->m_nResistAbilityOption > 20 )
#else // __EXT_ENCHANT
					if( ( (CItemElem*)pItemBase )->m_nResistAbilityOption > 10 ) 
#endif // __EXT_ENCHANT
						return;
#if __VER >= 13 // __EXT_ENCHANT
					if( dwValue > 20 )
						dwValue		= 20;
#else // __EXT_ENCHANT
					if( dwValue > 10 )
						dwValue		= 10;
#endif // __EXT_ENCHANT
					( (CItemElem*)pItemBase )->m_nResistAbilityOption	= dwValue;
					UpdateParam();
				}
				break;
			case UI_IR:  // 아이템에 무슨속성인지 넣기
				{
					( (CItemElem*)pItemBase )->m_bItemResist	= (BYTE)( dwValue );
					UpdateParam();
				}
				break;

			case UI_PIERCING_SIZE:
				( (CItemElem*)pItemBase )->SetPiercingSize( dwValue );
				break;
			case UI_PIERCING:
				{
					WORD wIndex	= LOWORD( dwValue ), wItemId	= HIWORD( dwValue );
					( (CItemElem*)pItemBase )->SetPiercingItem( wIndex, wItemId );
				}
				break;

#if __VER >= 12 // __EXT_PIERCING
			case UI_ULTIMATE_PIERCING_SIZE:
				( (CItemElem*)pItemBase )->SetUltimatePiercingSize( dwValue );
				break;
			case UI_ULTIMATE_PIERCING:
				{
					WORD wIndex	= LOWORD( dwValue ), wItemId	= HIWORD( dwValue );
					( (CItemElem*)pItemBase )->SetUltimatePiercingItem( wIndex, wItemId );
				}
				break;
#endif // __EXT_PIERCING

#if __VER >= 15 // __PETVIS
			case UI_PETVIS_SIZE:	
				{
					static_cast<CItemElem*>(pItemBase)->SetPiercingSize( dwValue );
					static_cast<CItemElem*>(pItemBase)->SetVisKeepTimeSize( dwValue );

#ifdef __CLIENT	//sun : 실체 비스 장착이( 서버로부터 패킷을 받아서 처리될때 )이루어질때 효과음 출력 
					PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
#endif // __CLIENT
				}
				break;
			case UI_PETVIS_ITEM:
				{
					WORD wIndex	= LOWORD( dwValue ), wItemId	= HIWORD( dwValue );
					static_cast<CItemElem*>(pItemBase)->SetPiercingItem( wIndex, wItemId );
					static_cast<CItemElem*>(pItemBase)->SetVisKeepTime( wIndex, ( dwTime * 60 ) + time_null() ); // 분단위 시간을 초단위로 변경

#ifdef __CLIENT
					PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
#endif // __CLIENT
				}
				break;

			case UI_PETVIS_ITEMSWAP:
				{
					WORD wPos1	= LOWORD( dwValue ), wPos2	= HIWORD( dwValue );
					static_cast<CItemElem*>(pItemBase)->SetSwapVisItem( wPos1, wPos2 );

#ifdef __CLIENT
					PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
#endif // __CLIENT
				}
				break;

			case UI_TRANSFORM_VISPET:
				{
					static_cast<CItemElem*>(pItemBase)->m_bTranformVisPet = static_cast<BOOL>( dwValue );
					static_cast<CItemElem*>(pItemBase)->m_bCharged = TRUE;
				}
				break;
#endif // __PETVIS

			case UI_RANDOMOPTITEMID:
				{
					( (CItemElem*)pItemBase )->SetRandomOpt( dwValue );
					break;
				}
			case UI_KEEPTIME:	// offset
				{
					CTime tm	= CTime::GetCurrentTime() + CTimeSpan( 0, 0, dwValue, 0 );
					( (CItemElem*)pItemBase )->m_dwKeepTime	= (DWORD)( tm.GetTime() );
					break;
				}

			case UI_FLAG:
				{
#ifdef __CLIENT
					DWORD dwObjIndex	= (DWORD)(short)LOWORD( dwValue );
					BYTE byFlag		= (BYTE)HIWORD( dwValue );
					int nParts	= dwObjIndex - m_Inventory.GetSize();
					if( nParts >= 0 && nParts < MAX_HUMAN_PARTS )
						m_aEquipInfo[nParts].byFlag	= byFlag;
					( (CItemElem*)pItemBase )->m_byFlag		= byFlag;
					UpdateParts( FALSE );
#endif	// __CLIENT
					UpdateParam();
					break;
				}

			default:
				break;
		}
#if __VER >= 9 // __CSC_VER9_1
#ifdef __CLIENT
		CWndInventory* pWnd	= (CWndInventory*)g_WndMng.GetWndBase( APP_INVENTORY );
		if( pWnd )
			pWnd->UpdateTooltip();

// 		CWndInventory* pWnd	= (CWndInventory*)g_WndMng.GetWndBase( );		
// 		if( pWnd )
// 			pWnd->UpdateTooltip();
#endif	// __CLIENT
#endif	// __CSC_VER9_1
	}
	else
	{
		switch( cParam )
		{
			case UI_FLAG:
				{
#ifdef __CLIENT
					DWORD dwObjIndex	= (DWORD)(short)LOWORD( dwValue );
					BYTE byFlag		= (BYTE)HIWORD( dwValue );
					int nParts	= dwObjIndex - m_Inventory.GetSize();
					if( nParts >= 0 && nParts < MAX_HUMAN_PARTS )
					{
						m_aEquipInfo[nParts].byFlag	= byFlag;
						UpdateParts( TRUE );
					}
#endif	// __CLIENT
					break;
				}
			default:
				break;
		}
	}
#ifdef __WORLDSERVER
	if( IsPlayer() )
#if __VER >= 15 // __PETVIS
	{
		static_cast<CUser*>( this )->AddUpdateItem( 0, nId, cParam, dwValue, dwTime );
		if( pItemBase && cParam == UI_TRANSFORM_VISPET && HasActivatedEatPet() )
		{
			InactivateEatPet();
			ActivateEatPet( static_cast<CItemElem*>( pItemBase ) );
		}
	}
#else // __PETVIS
		( (CUser*)this )->AddUpdateItem( 0, nId, cParam, dwValue );
#endif // __PETVIS
#endif // __WORLDSERVER
}

float CMover::GetItemDropRateFactor( CMover* pAttacker )
{
	float fFactor	= 1.0F;
#ifdef __WORLDSERVER
	fFactor		*= prj.m_fItemDropRate;
#ifdef __S1108_BACK_END_SYSTEM
	fFactor		*= GetProp()->m_fItemDrop_Rate;
#endif // __S1108_BACK_END_SYSTEM
#ifdef __ITEMDROPRATE
	fFactor		*= CEventGeneric::GetInstance()->GetItemDropRateFactor();
#endif // __ITEMDROPRATE
#if __VER >= 9 // __EVENTLUA
	fFactor		*= prj.m_EventLua.GetItemDropRate();
#endif // __EVENTLUA
#endif	// __WORLDSERVER
	return fFactor;
}

float CMover::GetPieceItemDropRateFactor( CMover* pAttacker )
{
	float fFactor	= 1.0F;
#ifdef __WORLDSERVER
#if __VER >= 9 // __EVENTLUA
	fFactor		*= prj.m_EventLua.GetPieceItemDropRate();
#endif // __EVENTLUA
#if __VER >= 12 // __LORD
	ILordEvent* pEvent		= CSLord::Instance()->GetEvent();
	fFactor		*= pEvent->GetIFactor();
	if( pAttacker->HasBuff( BUFF_ITEM, II_SYS_SYS_LS_CHEERING ) )
		fFactor		*=	1.1F;
	if( pAttacker->HasBuff( BUFF_ITEM, II_SYS_SYS_VOTE_THANKS ) )
		fFactor		*= 1.05F;
#endif	// __LORD
#if __VER >= 13 // __COUPLE_1202
	if( pAttacker->HasBuff( BUFF_ITEM, II_COUPLE_BUFF_MIRACLE_01 ) )
		fFactor	*= 1.05F;
#endif	// __COUPLE_1202
#if __VER >= 14 // __PCBANG
	if( pAttacker->IsPlayer() )
		fFactor *= CPCBang::GetInstance()->GetPieceItemDropFactor( static_cast<CUser*>( pAttacker ) );
#endif // __PCBANG
#endif // __WORLDSERVER
	return fFactor;
}

float CMover::GetExpFactor( void )
{
	float fFactor	= 1.0f;
	if( IsMode( EVENT_OLDBOY_MODE ) )
		fFactor	*= 1.1f;

	BOOL bGlvCheer	= FALSE;
	BOOL bSpecial = FALSE;
#ifndef __NEW_STACKABLE_AMPS
	int nCountBuf = 0;
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESA ) )
		++nCountBuf;
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESA1 ) )
		++nCountBuf;
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESA2 ) )
		++nCountBuf;
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESB ) )
		++nCountBuf;
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESB1 ) )
		++nCountBuf;
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESB2 ) )
		++nCountBuf;
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESC ) )
		++nCountBuf;
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESC1 ) )
		++nCountBuf;
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESC2 ) )
		++nCountBuf;
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESD ) )
		++nCountBuf;
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESD1 ) )
		++nCountBuf;
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESD2 ) )
		++nCountBuf;
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESE ) )
		++nCountBuf;
#ifdef __S_ADD_EXP
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESS ) )
	{
		++nCountBuf;	bSpecial = TRUE;
	}
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESS1 ) )
	{
		++nCountBuf;	bSpecial = TRUE;
	}
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESS2 ) )
	{
		++nCountBuf;	bSpecial = TRUE;
	}
#endif // __S_ADD_EXP

	int nLevel = GetLevel();
	if( nCountBuf == 1 )
	{
		fFactor *= 1.5f;
	}
	else if( nCountBuf == 2 )
	{
		if( bSpecial || nLevel < 61 )
			fFactor *= 2.0f;
		else
			fFactor *= 1.5f;
	}
	else if( 3 <= nCountBuf )
	{
		if( bSpecial || nLevel < 41 )
			fFactor *= 2.5f;
		else if( nLevel < 61 )
			fFactor *= 2.0f;
		else
			fFactor *= 1.5f;
	}

	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPES ) )
		fFactor	*= 1.5f;
	
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPEM ) )
		fFactor	*= 2.0f;
#else // __NEW_STACKABLE_AMPS

	int nLevel = GetLevel();
	int nExtraEXP = 0;

	for( MAPBUFF::iterator i = m_buffs.m_mapBuffs.begin(); i != m_buffs.m_mapBuffs.end(); ++i )
	{
		ItemProp* pItemProp = prj.GetItemProp( i->second->GetId());

		if( pItemProp )
		{
			if( pItemProp->dwItemKind3 == IK3_EXP_RATE )
			{
				if( nLevel >= pItemProp->nMinLimitLevel && nLevel <= pItemProp->nMaxLimitLevel )
				{
					nExtraEXP += pItemProp->nEffectValue;
				}
			}
		}
	}
	fFactor *= 1.0f + ((float)nExtraEXP / 100 );

#endif // __NEW_STACKABLE_AMPS
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_GLVCHEER ) )
	{
		bGlvCheer	= TRUE;
		fFactor	*= 1.10f;
	}

	if( !bGlvCheer && HasBuff( BUFF_ITEM, II_CHEERUP ) )
	{
#ifdef __EVENTLUA_CHEEREXP
		if( prj.m_EventLua.GetCheerExpFactor() > 1.05f )
			fFactor *= prj.m_EventLua.GetCheerExpFactor();
		else
#endif // __EVENTLUA_CHEEREXP
			fFactor	*= 1.05f;
	}

#ifdef __WORLDSERVER

	fFactor	*= CEventGeneric::GetInstance()->GetExpFactor();
#if __VER >= 9 // __EVENTLUA
	fFactor	*= prj.m_EventLua.GetExpFactor();
#endif // __EVENTLUA
	fFactor		*= prj.m_fMonsterExpRate;
#if __VER >= 9 // __ULTIMATE
	int nDstExp = GetParam( DST_EXPERIENCE, 0 );
	if( 0 < nDstExp )
		fFactor *= ( 1.0f + ( nDstExp / 100.0f ) );
#endif // __ULTIMATE

#if __VER >= 12 // __LORD
	ILordEvent* pEvent		= CSLord::Instance()->GetEvent();
	fFactor		*= pEvent->GetEFactor();
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_LS_CHEERING ) )
		fFactor		*=	1.1F;
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_VOTE_THANKS ) )
		fFactor		*=	1.05F;
#endif	// __LORD

#ifdef __ENVIRONMENT_EFFECT

	if( CEnvironment::GetInstance()->GetEnvironmentEffect() == TRUE )
	{
		fFactor *= prj.m_EventLua.GetWeatherEventExpFactor();
	}

#else // __ENVIRONMENT_EFFECT

#ifdef __EVENTLUA_RAIN
	if( g_Environment.m_bRain )
		fFactor *= prj.m_EventLua.GetRainEventExpFactor();
#else // __EVENTLUA_RAIN
#ifdef __RAIN_EVENT
	if( g_Environment.m_bRain )
		fFactor *= 2.0f;
#endif // __RAIN_EVENT
#endif // __EVENTLUA_RAIN
#ifdef __EVENTLUA_SNOW
	if( g_Environment.m_bSnow )
		fFactor *= prj.m_EventLua.GetSnowEventExpFactor();
#endif // __EVENTLUA_SNOW

#endif // __ENVIRONMENT_EFFECT

#endif	// __WORLDSERVER

	return fFactor;
}

#ifdef __SKILL0517
int	CMover::GetSkillLevel( SKILL* pSkill )
{
	if( !pSkill )
		return 0;
	ItemProp* pSkillProp	= prj.GetSkillProp( pSkill->dwSkill );
	if( pSkillProp )
	{
		DWORD dwSkillLevel	= (DWORD)GetParam( DST_SKILL_LEVEL, pSkill->dwLevel );
		return(  dwSkillLevel > pSkillProp->dwExpertMax? pSkillProp->dwExpertMax: dwSkillLevel );
	}
	return 0;
}
#endif	// __SKILL0517