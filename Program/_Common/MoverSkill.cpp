#include "stdafx.h"
#include "defineItem.h"

#if __VER >= 19
#include "defineItemGrade.h"
#include "defineItemType.h"
#endif

#include "defineSkill.h"
#include "defineSound.h"
#include "defineText.h"
#include "ModelObject.h"
#include "CreateObj.h"
#include "defineobj.h"
#include "eveschool.h"
#include "party.h"

#include "definequest.h"

#ifdef __CLIENT
	#include "DPClient.h"
#else	// __CLIENT
	#include "..\_aiinterface\AIPet.h"
	#include "User.h"
	#include "WorldMng.h"
	#include "DPSrvr.h"
	#include "DPCoreClient.h"
	#include "dpdatabaseclient.h"
#ifdef __SYS_TICKET
	#include "ticket.h"
#endif	// __SYS_TICKET
#if __VER >= 15 // __PETVIS
	#include "ItemUpgrade.h"
#endif  // __PETVIS
#endif	// __CLIENT

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

#if __VER >= 13 // __HONORABLE_TITLE			// 달인
	#include "honor.h"
#endif	// __HONORABLE_TITLE			// 달인

#ifdef __FUNNY_COIN
#include "FunnyCoin.h"
#endif // __FUNNY_COIN

#ifdef __CLIENT
	extern	CDPClient			g_DPlay;
#else
	extern	CUserMng			g_UserMng;
	extern	CWorldMng			g_WorldMng;
	extern	CDPSrvr				g_DPSrvr;
	extern	CDPCoreClient		g_DPCoreClient;
	extern	CDPDatabaseClient	g_dpDBClient;
#if __VER >= 15 // __GUILD_HOUSE
#include "GuildHouse.h"
#endif // __GUILD_HOUSE
#endif

extern	CPartyMng				g_PartyMng;
extern	CGuildCombat			g_GuildCombatMng;

extern void __SetQuest( DWORD dwIdMover, int nQuest );

#ifdef __WORLDSERVER

#if __VER >= 10 // __LEGEND	//	9차 전승시스템	Neuz, World, Trans
void	CMover::SetMasterSkillPointUp()
{
	if( m_nLevel != 72 && m_nLevel != 84 && m_nLevel != 96 && m_nLevel != 108 )
		return;
	if( IsHero() )
		return;
#ifdef __3RD_LEGEND16
	if( IsLegendHero() )
		return;
#endif // __3RD_LEGEND16
	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
	{				
		LPSKILL lpSkill = &(m_aJobSkill[i]);
		if( lpSkill && lpSkill->dwSkill != NULL_ID )
		{
			ItemProp* pSkillProp    = prj.GetSkillProp( lpSkill->dwSkill );			
			if( pSkillProp == NULL )
				continue;
			if( pSkillProp->dwItemKind1 != JTYPE_MASTER)
				continue;
			lpSkill->dwLevel++;
			g_dpDBClient.SendLogSkillPoint( LOG_SKILLPOINT_USE, 1, this, &(m_aJobSkill[i]) );
		}
	}	
	g_UserMng.AddCreateSfxObj(this, XI_SYS_EXCHAN01, GetPos().x, GetPos().y, GetPos().z);

	CUser *pUser = (CUser *)this;
	pUser->AddDoUseSkillPoint( &m_aJobSkill[0], m_nSkillPoint );

#ifdef __S_NEW_SKILL_2
	g_dpDBClient.SaveSkill( this );
#endif // __S_NEW_SKILL_2
}
#endif	//__LEGEND	//	9차 전승시스템	Neuz, World, Trans

// 스킬큐에 들어 있는 마법들의 총 캐스팅 시간을 계산.
int		CMover::GetQueueCastingTime()
{
	if( IsNPC() )	
		return 0;

	CUser *pUser = (CUser *)this;
	int		i;
	SHORTCUT *pSlotQueue = pUser->m_playTaskBar.m_aSlotQueue;
	int		nTotalTime = 0;

	for( i = 0; i < MAX_SLOT_QUEUE; i ++ )
	{
		if( pSlotQueue[i].m_dwShortcut )
		{
			LPSKILL pSkill = GetSkill( 0, pSlotQueue[i].m_dwId );
			
			ItemProp *pSkillProp;
			if( !( pSkillProp = pSkill->GetProp() ) )					// JobSkill 리스트에서 꺼낸 스킬의 프로퍼티를 꺼냄.
			{
				Error( "CMover::GetQueueCastingTime : %s. 스킬(%d)의 프로퍼티가 없다.", m_szName, pSkill->dwSkill );
				return FALSE;	// property not found
			}
			
			AddSkillProp* pAddSkillProp;
			
		#ifdef __SKILL0517
			if( !( pAddSkillProp	= prj.GetAddSkillProp( pSkillProp->dwSubDefine, GetSkillLevel( pSkill ) ) ) )					// JobSkill 리스트에서 꺼낸 스킬의 프로퍼티를 꺼냄.
		#else	// __SKILL0517
			if( !( pAddSkillProp	= prj.GetAddSkillProp( pSkillProp->dwSubDefine, pSkill->dwLevel ) ) )					// JobSkill 리스트에서 꺼낸 스킬의 프로퍼티를 꺼냄.
		#endif	// __SKILL0517
			{
				Error( "CMover::GetQueueCastingTime : %s. 애드스킬(%d)의 프로퍼티가 없다.", m_szName, pSkill->dwSkill );
				return FALSE;	// property not found
			}

		#ifdef _DEBUG
		#if __VER >= 9	// __SKILL_0706
			if( (int)pAddSkillProp->dwCastingTime <= 0 )
				Error( "GetQueueCastingTime : %s의 addProp dwCastingTime이 %d", pSkillProp->szName, pAddSkillProp->dwCastingTime );
		#else	// __SKILL_0706
			if( (int)pAddSkillProp->dwSkillReady <= 0 )
				Error( "GetQueueCastingTime : %s의 addProp dwSkillReady가 %d", pSkillProp->szName, pAddSkillProp->dwSkillReady );
		#endif	// __SKILL_0706
		#endif
#if __VER >= 9	// __SKILL_0706
			nTotalTime += (int)pAddSkillProp->dwCastingTime;
#else	// __SKILL_0706
			nTotalTime += (int)pAddSkillProp->dwSkillReady;
#endif	// __SKILL_0706
		}
	}

	nTotalTime = (int)(nTotalTime * 0.7f);		// 70프로 적용한후.
	nTotalTime = (int)( (nTotalTime / 1000.0f) * SEC1 );	// 캐스팅단위 시간으로 변환.
	nTotalTime	= GetCastingTime( nTotalTime );

	return nTotalTime;
}

#endif // worldserver


// 최초의 스킬 사용 명령이 내리짐
// Mover가 사용가능한 스킬배열(m_aJobSkill[3+14])에서 nIdx에 해당하는 스킬을 꺼내 그것을 실행한다.
// sutType : 스킬사용시 단축키/스킬큐의 여부를 클라로부터 받아서 처리.  캐스팅타임 계산이 다르다.
//
#ifdef __CLIENT
#if __VER >= 8 // __S8_PK
	BOOL	CMover::DoUseSkill( int nType, int nIdx, OBJID idFocusObj, SKILLUSETYPE sutType, BOOL bControl, const int nCastingTime, DWORD dwSkill, DWORD dwLevel )
#else // __VER >= 8 // __S8_PK
	BOOL	CMover::DoUseSkill( int nType, int nIdx, OBJID idFocusObj, SKILLUSETYPE sutType, const int nCastingTime, DWORD dwSkill, DWORD dwLevel )
#endif // __VER >= 8 // __S8_PK
#else // __CLIENT
	#if __VER >= 8 // __S8_PK
		BOOL	CMover::DoUseSkill( int nType, int nIdx, OBJID idFocusObj, SKILLUSETYPE sutType, BOOL bControl )
	#else // __VER >= 8 // __S8_PK
		BOOL	CMover::DoUseSkill( int nType, int nIdx, OBJID idFocusObj, SKILLUSETYPE sutType )
	#endif // __VER >= 8 // __S8_PK
#endif // __CLIENT
{
	if( IsNPC() )	
		return FALSE;		// 엔피씨는 이쪽을 타면 안된다.

#ifdef __WORLDSERVER
	DWORD dwSkill = 0, dwLevel;
#endif	// __WORLDSERVER
	
	if( nIdx >= 0 )
	{
		LPSKILL pSkill = GetSkill( nType, nIdx );
		if( pSkill == NULL )
		{
			Error( "CMover::DoUseSkill : %s는 nIdx에 스킬을 가지고 있지 않다. %d", m_szName, nIdx );
			return FALSE;
		}
		dwSkill	= pSkill->dwSkill;
	#ifdef __SKILL0517
		dwLevel	= GetSkillLevel( pSkill );
	#else	// __SKILL0517
		dwLevel	= pSkill->dwLevel;
	#endif	// __SKILL0517

		if( dwSkill == DWORD(-1) )
			return FALSE;

		if( dwLevel <= 0 )
			return FALSE;
/*
#ifdef __GUILD_COMBAT_1TO1 // chipi_071227 임시로 막음
		if( dwSkill == SI_KNT_HERO_DRAWING )
		{
		#ifdef __CLIENT
			if( g_GuildCombat1to1Mng.IsPossibleMover( this ) )
				return FALSE;
		#endif // __CLIENT
		#ifdef __WORLDSERVER
			if( g_GuildCombat1to1Mng.IsPossibleUser( (CUser*)this ) )
			{
				((CUser*)this)->AddDefinedText( TID_GAME_NEVERKILLSTOP );
				return FALSE;
			}
		#endif // __WORLDSERVER
		}
#endif // __GUILD_COMBAT_1TO1
*/
	}

#ifdef __WORLDSERVER
	int nCastingTime = 0;
#endif

	ItemProp *pSkillProp = NULL;
	AddSkillProp *pAddSkillProp = NULL;
	GetSkillProp( &pSkillProp, &pAddSkillProp, dwSkill, dwLevel, "DoUseSkill_P" );

	if(pSkillProp == NULL)
		return FALSE;

	OBJMSG dwMsg = OBJMSG_NONE;
	int nMotion = (int)pSkillProp->dwUseMotion;
	
	if( pSkillProp->dwSkillType == KT_MAGIC )
	{
		if( sutType == SUT_QUEUESTART )		// 스킬큐 시작 스킬
		{
		#ifdef __WORLDSERVER
			nCastingTime = GetQueueCastingTime();	// 큐내의 모든 스킬의 캐스팅을 몰아서 한다.
		#endif
			dwMsg = OBJMSG_MAGICCASTING;
		} 
		else if( sutType == SUT_QUEUEING )	// 스킬큐 진행중 스킬
		{
		#ifdef __WORLDSERVER
			nCastingTime = 0;						// 캐스팅 동작없이 시전.
		#endif
			nMotion += 2;
			dwMsg = OBJMSG_MAGICSKILL;
		} 
		else
		{									// 일반적인 마법 스킬 사용
		#ifdef __WORLDSERVER
		
		#if __VER >= 9	// __SKILL_0706
			nCastingTime	= (int)( ( pAddSkillProp->dwCastingTime / 1000.0f ) * SEC1 );	// 캐스팅 동작 있음.
		#else	// __SKILL_0706
			nCastingTime = (int)((pAddSkillProp->dwSkillReady / 1000.0f) * SEC1);	// 캐스팅 동작 있음.
		#endif	// __SKILL_0706
		
			nCastingTime = GetCastingTime( nCastingTime );
		#endif	// __WORLDSERVER
			dwMsg = OBJMSG_MAGICCASTING;
		}
	}
	else if( pSkillProp->dwSkillType == KT_SKILL )
	{
	#ifdef __WORLDSERVER
		nCastingTime = 1;
	#endif
	}

#if __VER >= 8 // __S8_PK
	BOOL bSuccess = DoUseSkill( dwSkill, dwLevel, idFocusObj, sutType, bControl, nCastingTime );
#else // __VER >= 8 // __S8_PK
	BOOL bSuccess = DoUseSkill( dwSkill, dwLevel, idFocusObj, sutType, nCastingTime );
#endif // __VER >= 8 // __S8_PK
	return  bSuccess;
}

// dwSkill,nLevel만 있으면 어디서든지 사용가능한 버전	
// 몬스터까지 사용가능
#if __VER >= 8 // __S8_PK
BOOL CMover::DoUseSkill( DWORD dwSkill, int nLevel, OBJID idFocusObj, SKILLUSETYPE sutType, BOOL bControl, const int nCastingTime )
#else // __VER >= 8 // __S8_PK
BOOL CMover::DoUseSkill( DWORD dwSkill, int nLevel, OBJID idFocusObj, SKILLUSETYPE sutType, const int nCastingTime )
#endif // __VER >= 8 // __S8_PK
{
	if( IsDie() )	return FALSE;
	if( m_pActMover->IsFly() )	return FALSE;
	if( m_dwFlag & MVRF_NOATTACK )	
		return FALSE;		// 공격금지 상태에선 스킬 못씀.

#ifdef __QUIZ
	if( GetWorld() && GetWorld()->GetID() == WI_WORLD_QUIZ )
		return FALSE;
#endif // __QUIZ
	
#if defined(__WORLDSERVER)	
	// 길드대전맵에서 선수만 사용
	CWorld* pWorld	= GetWorld();
	if( pWorld && pWorld->GetID() == WI_WORLD_GUILDWAR )
	{		
		if( m_nGuildCombatState == 0 )
		{
			if( IsPlayer() )
				((CUser*) this)->AddText( prj.GetText( TID_GAME_GUILDCOMBAT_STANDS_NOTUSESKILL ) );	//"수정해야함 : 관중석에서는 스킬을 사용할수 없습니다" );
			return FALSE;
		}
	}
#endif //defined(__WORLDSERVER)

	if( IsMode( NO_ATTACK_MODE ) ) return FALSE;
	
	ItemProp *pSkillProp = NULL;
	AddSkillProp *pAddSkillProp = NULL;
	GetSkillProp( &pSkillProp, &pAddSkillProp, dwSkill, nLevel, "DoUseSkill" );

#ifdef __CLIENT
	// 일대일 길드대전 시 블링크풀 사용 금지
	DWORD dwWorldID(GetWorld()->GetID());
	if(dwWorldID >= WI_WORLD_GUILDWAR1TO1_0 && dwWorldID <= WI_WORLD_GUILDWAR1TO1_L)
	{
		if(pSkillProp->dwID == SI_MAG_MAG_BLINKPOOL)
		{
			g_WndMng.PutString(prj.GetText(TID_GAME_NEVERKILLSTOP), NULL, prj.GetTextColor(TID_GAME_NEVERKILLSTOP));
			return FALSE;
		}
	}
#endif // __CLIENT

	CMover* pTarget = prj.GetMover( idFocusObj );	// 타겟의 포인터를 얻어냄.
	if( IsInvalidObj( pTarget ) )	
		return FALSE;

	if( pSkillProp->dwID == SI_ASS_HEAL_HEALING )	// 힐링 상대가 죽었으면 취소
	{
		if( pTarget->IsDie() )
			return FALSE;
	}

#ifdef __WORLDSERVER
#ifdef __PK_PVP_SKILL_REGION
	if( pSkillProp->nEvildoing < 0 )	//PK 금지구역에서 악행 종류의 스킬은 PVP 상대에게만 사용하도록..
	{
		int nAttackerPK, nDefenderPK;
		nAttackerPK = nDefenderPK = 0;
		if( IsPlayer() && pTarget->IsPlayer() && (this != pTarget) ) //(this!=pTarget) - 타겟을 안찍는 스킬은 타겟이 시전자이다.
		{
			DWORD dwRegionAttr	= GetPKPVPRegionAttr();
			nAttackerPK = IsPKPVPInspectionBase( dwRegionAttr, FALSE );
			nDefenderPK = pTarget->IsPKPVPInspectionBase( dwRegionAttr, FALSE );
			if( (nAttackerPK == 1 || nDefenderPK == 1) &&
				!( 
						IsPVPTarget( pTarget )
					|| IsWarTarget( pTarget )
					|| IsGuildCombatTarget( pTarget ) 
#ifdef __JEFF_11_4
					|| IsArenaTarget( pTarget )
#endif	// __JEFF_11_4
				) )
			{
				((CUser*)this)->AddDefinedText( TID_GMAE_PK_NOT_AREA, "" );
				return FALSE;
			}
		}
	}
#endif // __PK_PVP_SKILL_REGION
	if( HasBuff( BUFF_SKILL, SI_BLD_SUP_BERSERK ) )		// 버서크가 걸려있는 상태에선 스킬사용 금지.
		return FALSE;

#ifdef __NPC_BUFF
	if( ( pSkillProp->dwID == SI_ASS_CHEER_QUICKSTEP && pTarget->HasBuff(BUFF_SKILL, SI_GEN_EVE_QUICKSTEP) )
		|| ( pSkillProp->dwID == SI_ASS_CHEER_HASTE && pTarget->HasBuff(BUFF_SKILL, SI_GEN_EVE_HASTE) )
		|| ( pSkillProp->dwID == SI_ASS_CHEER_HEAPUP && pTarget->HasBuff(BUFF_SKILL, SI_GEN_EVE_HEAPUP) )
		|| ( pSkillProp->dwID == SI_ASS_CHEER_ACCURACY && pTarget->HasBuff(BUFF_SKILL, SI_GEN_EVE_ACCURACY) ) )
	{
		if( pTarget->IsPlayer() )
			((CUser*)pTarget)->AddDefinedText( TID_GAME_NPCBUFF_FAILED, "\"%s\"", pSkillProp->szName );
		return FALSE;
	}
#endif // __NPC_BUFF

	if( pSkillProp->dwID == SI_ACR_YOYO_SNITCH || pSkillProp->dwID == SI_JST_YOYO_VATALSTAB )	// 강탈, 혼신의 일격스킬이면 다이일루젼상태인지 검사한다.
	{
		if( !HasBuff( BUFF_SKILL, SI_ACR_SUP_DARKILLUSION ) )		// 다크일루젼 상태가 아니면 사용못함
		{
			if( IsPlayer() )
				((CUser*)this)->AddDefinedText( TID_GAME_REQ_DARK, "" );		// 다크일루젼상태에서 가능합니다.
			return FALSE;
		}
	}

#if __VER >= 13 // __RAINBOW_RACE
	if( pSkillProp->dwID == SI_RIG_HERO_RETURN
		&& IsPlayer() && CRainbowRaceMng::GetInstance()->IsEntry( ((CUser*)this)->m_idPlayer ) )
	{
		((CUser*)this)->AddDefinedText( TID_GAME_RAINBOWRACE_NOTELEPORT );
		return FALSE;
	}
#endif // __RAINBOW_RACE
#endif //defined(__WORLDSERVER)

	MoverProp *pMoverProp = pTarget->GetProp();
	if( pMoverProp == NULL )
		Error( "DoUseSkill : %s , 타겟 %s 의 프로퍼티가 없다", m_szName, pTarget->m_szName );
	
	if( pTarget != this )	// 타인에게 쓰는 종류일때만 아래검사를 함.
	{
	#ifdef __WORLDSERVER
		if( pSkillProp->dwUseChance == WUI_NOW )	// 康 - 리소스 해킹
			return FALSE;
		if( pSkillProp->dwExeTarget == EXT_SELFCHGPARAMET )
			return FALSE;

		if( pTarget->IsNPC() && pTarget->IsPeaceful() )	
			return FALSE;	// NPC지만 친한?NPC에겐 스킬 사용 금지
		
		if( !g_eLocal.GetState( EVE_STEAL ) )
		{
			if( pSkillProp->nEvildoing < 0 )	// 나쁜마법을 쓰려할때
				if( IsSteal( pTarget ) )	// pTarget를 스틸 하려하면 
					return FALSE;
		}
#ifdef __JEFF_11_4
		if(  pWorld && pWorld->IsArena() == FALSE )		// 아레나가 아니면,
#endif	// __JEFF_11_4
		{
			if( pSkillProp->nEvildoing > 0 )	// 좋은 마법을
			{
				HITTYPE hy;
#if __VER >= 8 // __S8_PK
				if( (hy = GetHitType2( pTarget, TRUE, TRUE)) != HITTYPE_FAIL )  	// 적대적인 대상에게 쓰려할때
#else // __VER >= 8 // __S8_PK
				if( (hy = GetHitType2( pTarget, TRUE)) != HITTYPE_FAIL )  	// 적대적인 대상에게 쓰려할때
#endif // __VER >= 8 // __S8_PK
				{
					if( IsPlayer() )
						((CUser*)this)->AddDefinedText( TID_GAME_NEVERKILLSTOP, "" );		// 스킬을 사용할 수 없습니다.
					return FALSE;		// 스킬 취소
				}
			}
		}
		
	#endif
		SetAngle( GetDegree(pTarget->GetPos(), GetPos()) );		// 목표쪽으로 몸을 돌림.
	}
	
	// 길드컴뱃중에는 부활스킬 금지
	if( GetWorld()->GetID() == WI_WORLD_GUILDWAR )
	{
#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
		if( pSkillProp->dwID == SI_MAG_MAG_BLINKPOOL || pSkillProp->dwID == SI_RIG_HERO_RETURN )
#else //__LEGEND	//	10차 전승시스템	Neuz, World, Trans
		if( pSkillProp->dwID == SI_MAG_MAG_BLINKPOOL )
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans
			return FALSE;

		if( g_GuildCombatMng.m_nState != CGuildCombat::WAR_STATE )
		{
			if( pSkillProp->dwID == SI_ASS_HEAL_RESURRECTION )
				return FALSE;
		}
	}

#ifdef __WORLDSERVER

/*
---- 2008.03.25 드로잉스킬 버그 삭제
	#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
	if( pSkillProp->dwID == SI_KNT_HERO_DRAWING )
	{
		CParty* pParty	= g_PartyMng.GetParty( ((CUser*)this)->m_idparty );
		if( pParty )
			pParty->DoUsePartyReCall( ((CUser*)this)->m_idparty, ((CUser*)this)->m_idPlayer, 0);
	}
	#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans
---- 2008.03.25 드로잉스킬 버그 삭제
*/
	if( g_eLocal.GetState( EVE_SCHOOL ) )
	{
		if( pSkillProp->dwID == SI_ASS_HEAL_RESURRECTION )
			return FALSE;
	}

	// 스킬을 사용할때 적합한 무기를 들고 있는가 검사
	ItemProp* pItemProp = NULL;
	DWORD	dwItemKind3;

	if( pSkillProp->dwLinkKind == IK3_SHIELD || pSkillProp->dwLinkKind == IK3_MAGICBARUNA || pSkillProp->dwLinkKind == IK3_ZEMBARUNA )	// 필요아아이템이 방패일때
	{
		CItemElem *pItemElem = GetEquipItem( PARTS_SHIELD );
		if( pItemElem )
		{
			pItemProp = pItemElem->GetProp();
			dwItemKind3 = pItemProp->dwItemKind3;
		} else
			dwItemKind3 = NULL_ID;
	}
	else	// 필요아이템이 방패가 아닐때는 액티브 아이템을...
	{
		pItemProp = GetActiveHandItemProp();
		dwItemKind3 = pItemProp->dwItemKind3;
	}
	
	switch( pSkillProp->dwLinkKind )	// 스킬이 필요로하는 무기.
	{
	case IK3_ENCHANTWEAPON:
		{
			if( IsPlayer() )
			{
				BOOL bFail = FALSE;

				CItemElem *pItemElemR = GetWeaponItem();
				CItemElem *pItemElemL = GetLWeaponItem();

				if( pItemElemR && pItemElemL )	// 양손쥐고 있을때.
				{	// 양손다 속성이 없을땐
					if( (pItemElemR && pItemElemR->m_bItemResist == SAI79::NO_PROP) &&		
						(pItemElemL && pItemElemL->m_bItemResist == SAI79::NO_PROP) )
						bFail = TRUE;		// 사용금지
				} 
				else if( pItemElemR && pItemElemL == NULL )	// 오른손만 쥐고 있을때.
				{
					if( pItemElemR && pItemElemR->m_bItemResist == SAI79::NO_PROP )		// 오른손에 속성이 없을땐
						bFail = TRUE;		// 사용금지
				} 
				else if( pItemElemR == NULL && pItemElemL == NULL )	// 양손다 없을때
				{
					bFail = TRUE;		// 이때는 무조건 사용금지
				}

				if( bFail )
				{
					( (CUser*)this )->AddDefinedText( TID_SKILL_NOTELEWEAPON, "" );	// 속성무기가 없어 사용못함.
					return FALSE;
				}
			}
		}
		break;
	case IK3_MAGICBOTH:
		if( dwItemKind3 != IK3_WAND  && dwItemKind3 != IK3_STAFF )	// 손에든 무기가 완드도 스태프도 아니면
		{
			if( IsPlayer() )
				( (CUser*)this )->AddDefinedText( TID_GAME_WRONGITEM, "" );
			return FALSE;
		}
		break;
	case IK3_YOBO:
#ifdef __CROSSBOW
		if( dwItemKind3 != IK3_YOYO && dwItemKind3 != IK3_BOW && dwItemKind3 != IK3_CROSSBOW )	
#else // __CROSSBOW
		if( dwItemKind3 != IK3_YOYO && dwItemKind3 != IK3_BOW )	
#endif // __CROSSBOW
		{
			if( IsPlayer() )
				( (CUser*)this )->AddDefinedText( TID_GAME_WRONGITEM, "" );
			return FALSE;
		}
		break;
#ifdef __CROSSBOW
	case IK3_BOW:
		if( dwItemKind3 != IK3_BOW && dwItemKind3 != IK3_CROSSBOW )
		{
			if( IsPlayer() )
				( (CUser*)this )->AddDefinedText( TID_GAME_WRONGITEM, "" );
			return FALSE;
		}
		break;
#endif // __CROSSBOW
	default:
		if( pSkillProp->dwLinkKind != NULL_ID && pSkillProp->dwLinkKind != dwItemKind3 )	// 그외는 IK3를 비교해서 틀리면 에러.
		{
			if( IsPlayer() )
				( (CUser*)this )->AddDefinedText( TID_GAME_WRONGITEM, "" );
			return FALSE;
		}
		break;
	}

#if __VER >= 8 //__CSC_VER8_3 //타겟의 버프중 이미 걸린 버프가 레벨이 높을 경우 걸지 못하게 함.
	if(pSkillProp->dwSkillType == BUFF_SKILL)
	{
		CMover* searchMover;
		if(pTarget == this)
			searchMover = this;
		else
			searchMover = pTarget;

#ifdef __BUFF_1107
		IBuff* pBuff	= searchMover->m_buffs.GetBuff( BUFF_SKILL, (WORD)( pSkillProp->dwID ) );
		if( pBuff )
		{
			if( pBuff->GetLevel() > (DWORD)( nLevel ) )
#else	// __BUFF_1107
		SKILLINFLUENCE* hasSkill = searchMover->m_SkillState.Find( BUFF_SKILL, pSkillProp->dwID );
		if( hasSkill )
		{
			if( hasSkill->dwLevel > nLevel )
#endif	// __BUFF_1107
			{
				if( IsPlayer() ) // 윤상이	// this가 CUser가 아닐 수 있음.
					( (CUser*)this )->AddDefinedText( TID_GAME_DONOTUSEBUFF, "" );
				return FALSE;
			}
		}
	}
#endif //__CSC_VER8_3

	// 투핸드용이냐 원핸드용이냐의 조건검사.
	if( pSkillProp->dwHanded != NULL_ID )
	{
		if( pItemProp )
		{
			if( IsPlayer() )
			{
				if( pSkillProp->dwHanded == HD_DUAL )		// 듀얼용 스킬인데
				{
					if( IsDualWeapon() == FALSE )			// 듀얼로 안차고 있다.
					{
						((CUser*)this)->AddDefinedText( TID_GAME_WRONGITEM, "" );
						return FALSE;
					}
				} 
				else if( pItemProp->dwHanded != pSkillProp->dwHanded )	// 스킬과 쥐고 있는 아이템의 Handed 종류가 다르면 에러
				{
					((CUser*)this)->AddDefinedText( TID_GAME_WRONGITEM, "" );
					return FALSE;
				}
			}
		}
	}

	// 필요총알종류가 지정되어 있다면
	if( IsBullet( pSkillProp ) == FALSE )
		return FALSE;

	if( IsPlayer() )
	{
#if __VER >= 10 // __LEGEND	//	9차 전승시스템	Neuz, World, Trans
#ifdef __3RD_LEGEND16
		if( pSkillProp->dwReqDisLV != NULL_ID && GetLevel() < (int)( pSkillProp->dwReqDisLV )  && !IsMaster() && !IsHero() && !IsLegendHero() )
#else // __3RD_LEGEND16
		if( pSkillProp->dwReqDisLV != NULL_ID && GetLevel() < (int)( pSkillProp->dwReqDisLV )  && !IsMaster() && !IsHero() )
#endif // __3RD_LEGEND16
			return FALSE;
#else //__LEGEND	//	9차 전승시스템	Neuz, World, Trans
		if( pSkillProp->dwReqDisLV != NULL_ID && GetLevel() < pSkillProp->dwReqDisLV )
			return FALSE;
#endif	//__LEGEND	//	9차 전승시스템	Neuz, World, Trans
		
		if( pSkillProp->dwReSkill1 != 0xffffffff )
		{
			LPSKILL pSkillBuf = GetSkill( pSkillProp->dwReSkill1 );
			if( pSkillBuf )
			{
				if( pSkillBuf->dwLevel < pSkillProp->dwReSkillLevel1 )
				{
					( (CUser*)this )->AddText( "SkillLevel1 " );
					return FALSE;
				}
			}
			else
			{
				int nIdx = GetSkillIdx( dwSkill );
				Error( "DoUseSkill : %s NULL GetSkill %d = dwReSkill(%d, %d)", m_szName, nIdx, pSkillProp->dwReSkill1, pSkillProp->dwReSkill2 );
			}
		}

		if( pSkillProp->dwReSkill2 != 0xffffffff )
		{
			LPSKILL pSkillBuf = GetSkill( pSkillProp->dwReSkill2 );
			if( pSkillBuf )
			{
				if( pSkillBuf->dwLevel < pSkillProp->dwReSkillLevel2 )
				{
					( (CUser*)this )->AddText( "SkillLevel2 " );
					return FALSE;
				}				
			}
			else
			{
				int nIdx = GetSkillIdx( dwSkill );
				Error( "DoUseSkill : %s NULL GetSkill %d = dwReSkill(%d, %d)", m_szName, nIdx, pSkillProp->dwReSkill1, pSkillProp->dwReSkill2 );
			}
		}
	}

	if( pSkillProp->dwID == SI_BIL_PST_ASALRAALAIKUM )	// 앗살라의 경우 예외처리.
	{
		//if( GetFatiguePoint() <= 0  )		// FP가 하나도 없으면 사용안됨. // chipi_090917 제거
		if( IsPlayer() && GetFatiguePoint() < GetReqFp( pAddSkillProp->nReqFp )  )		// chipi_090917 필요 FP 검사
		{
			((CUser*)this)->AddDefinedText( TID_GAME_REQFP ); // chipi_00917
			return FALSE;
		}
	}

	// 쿨타임 검사.
	{
		int nSkillIdx = GetSkillIdx( dwSkill );		// 스킬리스트 인덱스를 찾음.
		if( nSkillIdx >= 0 )
		{
			if( GetReuseDelay( nSkillIdx ) )		// 쓰려고 하는 스킬 쿨타임이 남아있으면 실행안됨.
			{
				if( IsPlayer() )
					((CUser*)this)->AddDefinedText( TID_GAME_SKILLWAITTIME, "" );	// 아직 사용할 수 없습니다.
				return FALSE;
			}
		}
	}
#endif // __WORLDSERVER
		

	SendActMsg( OBJMSG_STOP );
	ClearDest();

	OBJMSG dwMsg = OBJMSG_NONE;
	int nMotion = (int)pSkillProp->dwUseMotion;
	
	//--- 마나가 충분한지 검사...
	if( pSkillProp->dwSkillType == KT_SKILL )
	{
	#ifdef __WORLDSERVER

		#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
			if(GetReqFp( pAddSkillProp->nReqFp ) == 9999999)
			{
				if( GetManaPoint() != GetMaxManaPoint())
				{
					if( TRUE == IsPlayer() )
						( (CUser*)this )->AddDefinedText( TID_GAME_REQMP, "" );
					return FALSE;
				}
				if( GetFatiguePoint() != GetMaxFatiguePoint())
				{
					if( TRUE == IsPlayer() )
						( (CUser*)this )->AddDefinedText( TID_GAME_REQFP, "" );
					return FALSE;
				}
			}
			else
			{
				if( GetFatiguePoint() - GetReqFp( pAddSkillProp->nReqFp ) < 0 )	// FP가 모자르므로 취소
				{
					if( TRUE == IsPlayer() )
						( (CUser*)this )->AddDefinedText( TID_GAME_REQFP, "" );
					return FALSE;
				}

				// KT_SKILL중에 MP가 소모되는 값이있으면 FP와 같이 소모시켜준다.
				if( (int)pAddSkillProp->nReqMp > 0 )
				{
					if( GetManaPoint() - GetReqMp( pAddSkillProp->nReqMp ) < 0 )		// 마나가 모자르므로 취소
					{
						if( TRUE == IsPlayer() )
							( (CUser*)this )->AddDefinedText( TID_GAME_REQMP, "" );
						return FALSE;
					}
				}
			}
		#else //__LEGEND	//	10차 전승시스템	Neuz, World, Trans
			if( GetFatiguePoint() - GetReqFp( pAddSkillProp->nReqFp ) < 0 )	// FP가 모자르므로 취소
			{
				if( TRUE == IsPlayer() )
					( (CUser*)this )->AddDefinedText( TID_GAME_REQFP, "" );
				return FALSE;
			}

			// KT_SKILL중에 MP가 소모되는 값이있으면 FP와 같이 소모시켜준다.
			if( (int)pAddSkillProp->nReqMp > 0 )
			{
				if( GetManaPoint() - GetReqMp( pAddSkillProp->nReqMp ) < 0 )		// 마나가 모자르므로 취소
				{
					if( TRUE == IsPlayer() )
						( (CUser*)this )->AddDefinedText( TID_GAME_REQMP, "" );
					return FALSE;
				}
			}
		#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans

	#endif
		dwMsg = OBJMSG_MELEESKILL;	// 클라에서도 실행해야함
	} 
	else if( pSkillProp->dwSkillType == KT_MAGIC )
	{
	#ifdef __WORLDSERVER
		if( GetAdjParam( DST_CHRSTATE ) & CHS_SILENT )		// 침묵상태에선 매직스킬 쓸 수 없음.
			return FALSE;

#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
		if(GetReqMp( pAddSkillProp->nReqMp ) == 9999999)
		{
			if( GetManaPoint() != GetMaxManaPoint())
			{
				if( TRUE == IsPlayer() )
					( (CUser*)this )->AddDefinedText( TID_GAME_REQMP, "" );
				return FALSE;
			}
			if( GetFatiguePoint() != GetMaxFatiguePoint())
			{
				if( TRUE == IsPlayer() )
					( (CUser*)this )->AddDefinedText( TID_GAME_REQFP, "" );
				return FALSE;
			}
		}
		else
		{
			if( GetManaPoint() - GetReqMp( pAddSkillProp->nReqMp ) < 0 )		// 마나가 모자르므로 취소
			{
				if( TRUE == IsPlayer() )
					( (CUser*)this )->AddDefinedText( TID_GAME_REQMP, "" );
				return FALSE;
			}
		}
#else //__LEGEND	//	10차 전승시스템	Neuz, World, Trans
		if( GetManaPoint() - GetReqMp( pAddSkillProp->nReqMp ) < 0 )		// 마나가 모자르므로 취소
		{
			if( TRUE == IsPlayer() )
				( (CUser*)this )->AddDefinedText( TID_GAME_REQMP, "" );
			return FALSE;
		}
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans

	#endif // worldserver
		if( sutType == SUT_QUEUESTART )		// 스킬큐 시작 스킬. - 큐내의 모든 스킬의 캐스팅을 몰아서 한다.
		{
			if( IsPlayer() )
				dwMsg = OBJMSG_MAGICCASTING;
		} 
		else if( sutType == SUT_QUEUEING )	// 스킬큐 진행중 스킬. - 캐스팅 동작없이 시전.
		{
			nMotion += 2;
			dwMsg = OBJMSG_MAGICSKILL;
		} 
		else
		{									// 일반적인 마법 스킬 사용 - 캐스팅 동작 있음.
			dwMsg = OBJMSG_MAGICCASTING;
		}
	}
	else
		Error( "DoUseSkill : %s dwSkillType = %d", m_szName, pSkillProp->dwSkillType );	// 프로퍼티값이 이상함.

	if( sutType != SUT_QUEUEING && nCastingTime == 0 && pSkillProp->dwID != SI_GEN_ATK_COUNTER )
		nMotion += 2;
	
	//--- 동작 시작 -----------------------------------------------------------------
#if __VER >= 10 // __LEGEND	//	9차 전승시스템	Neuz, World, Trans
	int nRet = m_pActMover->SendActMsg( dwMsg, nMotion, idFocusObj, nCastingTime, dwSkill );
#else //__LEGEND	//	9차 전승시스템	Neuz, World, Trans
	int nRet = m_pActMover->SendActMsg( dwMsg, nMotion, idFocusObj, nCastingTime );
#endif	//__LEGEND	//	9차 전승시스템	Neuz, World, Trans

	if( nRet == 1 )	// 스킬 동작 시작.
	{
		// 명령수행중 필요한 파라메터세팅.
		SetActParam( OBJACT_USESKILL, dwSkill, idFocusObj );	
		m_nAParam[3]	= nLevel;
#if __VER >= 8 // __S8_PK
		m_nAParam[4]	= bControl;
#endif // __VER >= 8 // __S8_PK

	#ifdef __WORLDSERVER
		if( pSkillProp->dwSkillType == KT_SKILL )
		{
			// 근접스킬은 사용하는 즉시 fp감소.
			if( (int)pAddSkillProp->nReqFp > 0 )
				IncFatiguePoint( -GetReqFp( pAddSkillProp->nReqFp ) );			// FP감소

			int nReqMp	= GetReqMp( pAddSkillProp->nReqMp );
			if( nReqMp > 0 )
			{
				IncManaPoint( -nReqMp );
			}
		}
	#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
		if( pSkillProp->dwSkillType == KT_MAGIC )
		{
			if( GetReqMp( pAddSkillProp->nReqMp ) == 9999999 )
			{
				IncFatiguePoint( -GetFatiguePoint() );			// FP감소
				IncManaPoint( -GetManaPoint() );
			}
		}
	#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans
		
		if( pSkillProp->dwLinkKindBullet != NULL_ID )	// 총알(?)을 필요로 하는거면
		{
			CItemElem *pItemElem = GetEquipItem( PARTS_BULLET );
			if( pItemElem )
				ArrowDown( 1 );
		}

		if( pSkillProp->nEvildoing < 0 )		// 나쁜마법으로만 몬스터나 플레이어를 찜할수있다.
		{
			pTarget->SetJJim( this );			// pTarget를 찜했다는걸 표시.
			m_nAtkCnt = 1;						// 빗자루 딜레이를 위한 공격 카운터 시작.
		}

		// DST param이 적용되는 스킬은 여기서 적용함.
		g_UserMng.AddUseSkill( this, dwSkill, nLevel, idFocusObj, sutType, nCastingTime );
	#endif	// __WORLDSERVER
	} 
	else if( nRet == -2 )	// 0:큐에 쌓는다 -1:모름  -2:완전실패하여 취소시킴   -3:모름
	{
		return FALSE;
	}
	
	m_dwFlag |= MVRF_SKILL;		// 스킬사용 명령이 시작됐다.
#ifdef __CLIENT
	if( IsActiveMover() )		// g_pPlayer일경우 
	{
		CWndTaskBar* pTaskBar = g_WndMng.m_pWndTaskBar;
		if( pTaskBar->m_nExecute == 1 )		// 스킬바 사용 대기중상태였으면
			pTaskBar->m_nExecute = 2;		// 실제 스킬바 사용중으로 전환.
	}
#endif

#ifdef __CLIENT
	{
		DWORD dwLinkSfxDouble = NULL_ID;
		switch( dwSkill )
		{	// 여기다 XI_ 추가하면 CreateSfx의 CSfxPartsLink()에도 추가시켜야 함/
		case SI_BLD_DOUBLEAX_SPRINGATTACK:	dwLinkSfxDouble = XI_SKILL_BLD_DOUBLEAX_SPRINGATTACK01;	break;
		case SI_BLD_DOUBLE_SONICBLADE:		dwLinkSfxDouble = XI_SKILL_BLD_DOUBLE_SONICBLADE01;	break;
		case SI_KNT_TWOSW_CHARGE:			dwLinkSfxDouble = XI_SKILL_KNT_TWOSW_CHARGE01;	break;
		case SI_BLD_DOUBLESW_BLADEDANCE:	dwLinkSfxDouble = XI_SKILL_BLD_DOUBLESW_BLADEDANCE01;      break;

		// 제스터 양손 이펙트 : sfx프레임이 끝이면 소멸함
		case SI_JST_YOYO_VATALSTAB:	dwLinkSfxDouble = XI_SKILL_JST_YOYO_VATALSTAB01;	break;
		case SI_JST_YOYO_HITOFPENYA:	dwLinkSfxDouble = XI_SKILL_JST_YOYO_HITOFPENYA01;	break;

		case SI_JST_YOYO_CRITICALSWING:	dwLinkSfxDouble = XI_SKILL_JST_SUP_CRITICALSWING01;	break;
		case SI_ACR_SUP_SLOWSTEP:	dwLinkSfxDouble = XI_SKILL_ACR_YOYO_SLOWSTEP01;	break;
		case SI_JST_SUP_POISON:		dwLinkSfxDouble = XI_SKILL_JST_SUP_POISON01;	break;
		case SI_JST_SUP_BLEEDING:	dwLinkSfxDouble = XI_SKILL_JST_SUP_BLEEDING01;	break;
		case SI_JST_YOYO_BACKSTAB:	dwLinkSfxDouble = XI_SKILL_JST_YOYO_BACKSTAB01;	break;
		case SI_JST_SUP_ABSORB:	dwLinkSfxDouble = XI_SKILL_JST_SUP_ABSORB01;	break;
			break;
		}
		if( dwLinkSfxDouble != NULL_ID )
		{
			CSfx *pSfx = CreateSfx( g_Neuz.m_pd3dDevice, dwLinkSfxDouble, GetPos(), GetId() );  // 시전동작이기때문에 무조건 자기에게 나타난다.
			if( pSfx )
			{
				pSfx->SetPartLink( 0 );		// 오른손에 링크.
				pSfx = CreateSfx( g_Neuz.m_pd3dDevice, dwLinkSfxDouble, GetPos(), GetId() );  // 링크이펙 하나더 생성.
				if( pSfx )
					pSfx->SetPartLink( 1 );		// 손에 링크.
			}
		}
	}

	// 왼손 링크 이펙
	DWORD dwLinkSfxLeft = NULL_ID;
	switch( dwSkill )
	{	// 아크로벳 스킬이면 활 시전 이펙트 왼손에 링크한다
	case SI_RAG_BOW_ICEARROW:	dwLinkSfxLeft = XI_SKILL_RAG_BOW_ICEARROW01;	break;
	case SI_ACR_BOW_JUNKBOW:	dwLinkSfxLeft = XI_SKILL_ACR_BOW_JUNKBOW01;	break;
	case SI_ACR_BOW_AIMEDSHOT:	dwLinkSfxLeft = XI_SKILL_ACR_BOW_AIMEDSHOT01;	break;
	case SI_ACR_BOW_SILENTSHOT:	dwLinkSfxLeft = XI_SKILL_ACR_BOW_SILENTSHOT01;	break;
	case SI_ACR_BOW_ARROWRAIN:	dwLinkSfxLeft = XI_SKILL_ACR_BOW_ARROWRAIN01;      break;
	case SI_ACR_BOW_AUTOSHOT:	dwLinkSfxLeft = XI_SKILL_ACR_BOW_AUTOSHOT01;      break;
		
	case SI_RAG_SUP_FASTATTACK:	  dwLinkSfxLeft = XI_SKILL_RAG_SUP_FASTATTACK01;break;
	case SI_RAG_BOW_FLAMEARROW:	  dwLinkSfxLeft = XI_SKILL_RAG_BOW_FLAMEARROW01;break;
	case SI_RAG_BOW_PIRCINGARROW: dwLinkSfxLeft = XI_SKILL_RAG_BOW_PIRCINGARROW01;break;
	case SI_RAG_BOW_POISONARROW:  dwLinkSfxLeft = XI_SKILL_RAG_BOW_POISONARROW01;break;
	case SI_RAG_BOW_SILENTARROW:  dwLinkSfxLeft = XI_SKILL_RAG_BOW_SILENTARROW01;break;
	case SI_RAG_BOW_TRIPLESHOT:   dwLinkSfxLeft = XI_SKILL_RAG_BOW_TRIPLESHOT01;break;
		
		break;
	}
	
	if( dwLinkSfxLeft != NULL_ID )
	{
		CSfx *pSfx = CreateSfx( g_Neuz.m_pd3dDevice, dwLinkSfxLeft, GetPos(), GetId() );  // 시전동작이기때문에 무조건 자기에게 나타난다.
		if( pSfx )
			pSfx->SetPartLink( 1 );		// 왼손에 링크.
	}

	// 오른손 링크 이펙
	DWORD dwLinkSfxRight = NULL_ID;
	switch( dwSkill )
	{
	case SI_MER_ONE_SPLMASH: dwLinkSfxRight = XI_SKILL_MER_ONE_SUPPORT04; break;
	case SI_MER_ONE_GUILOTINE: dwLinkSfxRight = XI_SKILL_MER_ONE_SUPPORT02; break;
	case SI_MER_ONE_REFLEXHIT: dwLinkSfxRight = XI_SKILL_MER_ONE_SUPPORT01; break;
	case SI_MER_ONE_SPECIALHIT: dwLinkSfxRight = XI_SKILL_MER_ONE_SUPPORT03; break;
	case SI_ASS_KNU_POWERFIST:	dwLinkSfxRight = XI_SKILL_ASS_KNU_SUPPORT03;	break;
	case SI_ASS_KNU_TAMPINGHOLE:	dwLinkSfxRight = XI_SKILL_ASS_KNU_SUPPORT02;	break;
	case SI_ASS_KNU_BURSTCRACK:	dwLinkSfxRight = XI_SKILL_ASS_KNU_SUPPORT01;	break;
	case SI_GEN_ATK_COUNTER:	dwLinkSfxRight = XI_SKILL_JST_YOYO_BACKSTAB01;	break;
		break;
	}
	
	if( dwLinkSfxRight != NULL_ID )
	{
		CSfx *pSfx = CreateSfx( g_Neuz.m_pd3dDevice, dwLinkSfxRight, GetPos(), GetId() );  // 시전동작이기때문에 무조건 자기에게 나타난다.
		if( pSfx )
			pSfx->SetPartLink( 0 );		// 오른손에 링크.
	}

	// 마법 혹은 전투스킬의 발동시작 이펙트발생
	// 자기id GetId()
	// 타겟id idFocusObj
	if( pSkillProp->dwSfxObj != NULL_ID )
	{
		CreateSfx( g_Neuz.m_pd3dDevice, pSkillProp->dwSfxObj, GetPos(), GetId() );  // 시전동작이기때문에 무조건 자기에게 나타난다.
	}	
	// 공격스킬과 타겟이 몬스터 이면 화면에 출력을 안함
	if( pSkillProp->dwExeTarget != EXT_MELEEATK && 
		pSkillProp->dwExeTarget != EXT_MAGICATK && 
		pSkillProp->dwExeTarget != EXT_AROUNDATK &&  
		pSkillProp->dwExeTarget != EXT_MAGICATKSHOT && 
		pTarget->IsPlayer() )
	{
		CString str;
		if( this == g_pPlayer ) // 내가 쓴것
		{
			str.Format( prj.GetText( TID_GAME_SKILL_SELF ), pSkillProp->szName );
			g_WndMng.PutString( (LPCTSTR)str, NULL, prj.GetTextColor( TID_GAME_SKILL_SELF ) );	
		}
		else
		if( pTarget == g_pPlayer && ( this != pTarget || pSkillProp->dwID == SI_ASS_CHEER_CIRCLEHEALING ) ) // 내가 쓴것이 아니라면
		{
			str.Format( prj.GetText( TID_GAME_SKILL_MATE ), GetName(), pSkillProp->szName );
			g_WndMng.PutString( (LPCTSTR)str, NULL, prj.GetTextColor( TID_GAME_SKILL_MATE ) );	
		}
	}
#endif	// __CLIENT

#ifdef __CLIENT	
	if( ::GetLanguage() == LANG_JAP )
	{
		if( g_pPlayer )
		{
			if( g_Option.m_bVoice )
			{
				if( IsActiveMover() )
				{
					int nindex = 0;
					
					if( pSkillProp->dwID != -1 )
					{
						if( GetSex() == 0 )
							nindex = pSkillProp->dwID+3000;
						else
							nindex = pSkillProp->dwID+3500;

						g_SoundMng.m_nOldSoundVolume = g_SoundMng.m_nSoundVolume;
						g_SoundMng.m_nSoundVolume = 0;
						PLAYSND( nindex );

						g_SoundMng.m_nSoundVolume = g_SoundMng.m_nOldSoundVolume;
					}
				}
			}
		}
	}
#endif	// __CLIENT

#if __VER >= 8 // __S8_PK
#ifdef __WORLDSERVER
	if( g_eLocal.GetState( EVE_PK ) )
	{
		if( !IsChaotic() && ( pTarget->IsPKPink() || ( pTarget->IsChaotic() && pSkillProp->nEvildoing > 0 ) ) )
		{
			SetPKPink( GetTickCount() + SEC(prj.m_PKSetting.nReadyAttack) );
			g_UserMng.AddPKPink( this, 1 );	
		}
	}
#endif // __WORLDSERVER
#endif // __VER >= 8 // __S8_PK
	
	return TRUE;
}

//
//	지속성을 갖는 스킬이 시간이 다되 끝났을때 호출됨.
//
void	CMover::OnEndSkillState( DWORD dwSkill, DWORD dwLevel )
{
	if( dwSkill < 0 || (int)( dwSkill ) > prj.m_aPropSkill.GetSize() )
	{
		Error( "OnEndSkillState : 스킬의 범위를 넘어섬. %d", dwSkill );
		return;
	}
	ItemProp* pSkillProp = NULL;
	AddSkillProp *pAddSkillProp = NULL;
	GetSkillProp( &pSkillProp, &pAddSkillProp, dwSkill, dwLevel, "OnEndSkillState" );

	if( dwSkill == SI_ACR_YOYO_COUNTER )	// 카운터 어택스킬의 경우
	{
		m_pActMover->ResetState( OBJSTA_ATK_ALL );		// 공격동작 중단시킴.  (이런 패턴의 스킬은 앞으로 일반화 시키자)
	}
#if defined( __CLIENT ) 
/*	// 091022 mirchang - 프로텍션, 펜바리어 스킬 버프 해제 시 sfx 삭제
	#if __VER >= 11 // __MA_VER11_06				// 확율스킬 효과수정 world,neuz
	if(	dwSkill == SI_MAG_FIRE_HOTAIR)
	{
		SetRemoveSfxObj( pSkillProp->dwSfxObj2 );
//		CSfx *pSfx = prj.GetSfx((this)->m_dwObjID,
//		if(pSfx)
//			pSfx->m_nSec = 0;	
	}
	if(	dwSkill == SI_PSY_HERO_STONE)
	{
		SetRemoveSfxObj( pSkillProp->dwSfxObj4 );
	}
	#endif // __MA_VER11_06				// 확율스킬 효과수정 world,neuz
*/	// 091022 mirchang - 프로텍션, 펜바리어 스킬 버프 해제 시 sfx 삭제
	switch( dwSkill )	// 091022 mirchang - 프로텍션, 펜바리어 스킬 버프 해제 시 sfx 삭제
	{
		case SI_MAG_FIRE_HOTAIR :
			SetRemoveSfxObj( pSkillProp->dwSfxObj2 );
			break;
		case SI_PSY_HERO_STONE :
		case SI_MER_SHIELD_PANBARRIER:
		case SI_MER_SHIELD_PROTECTION:
			SetRemoveSfxObj( pSkillProp->dwSfxObj4 );
			break;
	}
  #endif	// 

	BOOL	bSend = FALSE;
#ifdef __WORLDSERVER
	bSend = TRUE;			// 서버일경우 유저들에게 보내줌.

#ifdef __3RD_LEGEND16
	if( dwSkill == SI_FLO_SUP_ABSOLUTE )
		ResetDestParam( DST_IGNORE_DMG_PVP, GetParam( DST_IGNORE_DMG_PVP, 0), bSend );
	else
#endif // __3RD_LEGEND16
	{
		ResetDestParam( pAddSkillProp->dwDestParam1, pAddSkillProp->nAdjParamVal1, bSend );
		ResetDestParam( pAddSkillProp->dwDestParam2, pAddSkillProp->nAdjParamVal2, bSend );
	}
#endif


}

// 일반 공격 동작이 끝났을때 호출.
void	CMover::OnEndMeleeAttack( DWORD dwState )
{
#ifdef __WORLDSERVER
	PostAIMsg( AIMSG_END_MELEEATTACK );
#endif // WorldServer
}

#if __VER >= 11 // __MA_VER11_06				// 확율스킬 효과수정 world,neuz
DWORD	CMover::GetRemoveSfxObj(DWORD	dwRemoveSfxObjID)
{
	DWORD	dwResult = 0;
	for(int i=0;i<MAX_SKILLBUFF_COUNT;i++)
	{
		if(dwRemoveSfxObj[i] ==  dwRemoveSfxObjID)
		{
			dwResult = dwRemoveSfxObj[i];
			dwRemoveSfxObj[i] = 0;
			return	dwResult;
		}
	}	
	return	dwResult;	
}

void	CMover::SetRemoveSfxObj(DWORD	dwRemoveSfxObjID)
{
	for(int i=0;i<MAX_SKILLBUFF_COUNT;i++)
	{
		if(dwRemoveSfxObj[i] ==  dwRemoveSfxObjID)
			return;
	}
	for( i=0;i<MAX_SKILLBUFF_COUNT;i++)
	{
		if(dwRemoveSfxObj[i] ==  0)
		{
			dwRemoveSfxObj[i] = dwRemoveSfxObjID;
			return;
		}
	}
}
#endif // __MA_VER11_06				// 확율스킬 효과수정 world,neuz
/*--------------------------------------------------------------------------------------------------------


											Do Use Item


----------------------------------------------------------------------------------------------------------*/

// 아이템 ID로 아이템 생성(인벤토리와 상관없이 사용)
BOOL CMover::DoUseItemVirtual( DWORD dwItemId, BOOL bEffectSkip )
{
#ifdef __WORLDSERVER
	//인벤없는 아템 사용시는 죽음과 관계없이 실행가능하게한다.

	ItemProp* pItemProp = prj.GetItemProp( dwItemId );

	if( pItemProp )
	{
		if( IsItemRedyTime( pItemProp, pItemProp->dwID, FALSE ) == FALSE )
			return FALSE;

		D3DXVECTOR3 sPos = D3DXVECTOR3(0.0f,0.0f,0.0f);

		if( dwItemId == II_SYS_SYS_SCR_RETURN )
		{
			// 돌아가기 
			if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_RETURN ) )
				RemoveBuff( BUFF_ITEM, II_SYS_SYS_SCR_RETURN );

			DWORD dwWorldID;
			D3DXVECTOR3 vPos = ((CUser*)this)->GetReturnPos( &dwWorldID );
			((CUser*)this)->REPLACE( g_uIdofMulti, dwWorldID, vPos, REPLACE_NORMAL, nTempLayer );
			return TRUE;
		}


		if( dwItemId == II_CHR_SYS_SCR_ESCAPEBLINKWING )
		{
			if( pItemProp->dwCircleTime != (DWORD)-1 )
			{
				if( !IsSMMode( SM_ESCAPE ) )
				{
					SetSMMode( SM_ESCAPE, pItemProp->dwCircleTime );
				}
				else
				{
					return FALSE;
				}
			}
			return DoUseItemBlinkWing( pItemProp, NULL, FALSE );
		}

		DWORD	dwSfxID = pItemProp->dwSfxObj3;
		if( dwSfxID != NULL_ID && !bEffectSkip )		// 아이템 사용시에 생성될 이펙트
		{
			g_UserMng.AddCreateSfxObj( this, dwSfxID, sPos.x, sPos.y, sPos.z );	// 절대좌표로 하자.
		}	
	}
#endif //__WORLDSERVER

	return TRUE;
}

// 아이템을 사용하는 시점에 호출된다. 
void CMover::OnAfterUseItem( const ItemProp* pItemProp )
{
#ifdef __WORLDSERVER
	if( pItemProp == NULL )
		return;

	D3DXVECTOR3 sPos = D3DXVECTOR3(0.0f,0.0f,0.0f);
	DWORD dwSfxID = pItemProp->dwSfxObj3;		// 아이템 사용시에 생성될 이펙트

	if(	pItemProp->dwItemKind3 == IK3_MAGICTRICK ) // 이펙트 아이템류
	{
		float	fTheta = D3DXToRadian( GetAngle() );
		switch( pItemProp->dwID )
		{
		case II_GEN_MAG_TRI_FIRESHOWER:
		case II_GEN_MAG_TRI_HWFIREWORKS:
		case II_CHR_MAG_TRI_ROCKETBOMB:
		case II_CHR_MAG_TRI_HEARTBOMB:
		case II_CHR_MAG_TRI_TWISTERBOMB:
			sPos.x = GetPos().x + sinf(fTheta) * 3.0f;			// 바라보는 방향 1미터 앞에다 발생시킴.
			sPos.z = GetPos().z + -cosf(fTheta) * 3.0f;
			sPos.y = GetPos().y + 1.5f;
			sPos.y = GetWorld()->GetUnderHeight( D3DXVECTOR3(sPos.x,sPos.y,sPos.z) );
			break;
		case II_GEN_MAG_TRI_NEWYEARBOMB:
		case II_GEN_MAG_TRI_SULNALBOMB:
		case II_GEN_MAG_TRI_GOODBYEBOMB:
			sPos.x = GetPos().x;
			sPos.z = GetPos().z;
			sPos.y = GetPos().y + 3.0f;
			break;
		}
	}

	if( dwSfxID != NULL_ID )		
		g_UserMng.AddCreateSfxObj( this, dwSfxID, sPos.x, sPos.y, sPos.z );	// 절대좌표로 하자.
#if __VER >= 13 // __HONORABLE_TITLE			// 달인
	if( IsPlayer() )
		((CUser*)this)->SetHonorAdd(pItemProp->dwID,HI_USE_ITEM);
#ifndef __MAINSERVER
	else
		FILEOUT( "..\\HonorError.txt", "OnAfterUseItem()\n" );
#endif	// __MAINSERVER
#endif	// __HONORABLE_TITLE			// 달인

#endif // __WORLDSERVER
}

BOOL CMover::DoUseItem( DWORD dwData, DWORD dwFocusId, int nPart  )
{
#ifdef __WORLDSERVER
	if( IsDie() )
		return FALSE;
#ifdef __S_SERVER_UNIFY
	if( ((CUser*)this)->m_bAllAction == FALSE )
		return FALSE;
#endif // __S_SERVER_UNIFY

	WORD dwType = LOWORD( dwData );
	WORD dwId   = HIWORD( dwData );

	CItemBase* pItemBase = GetItemId( dwId ); 
	if( IsUsableItem( pItemBase ) == FALSE )
		return FALSE;

	CItemElem* pItemElem = (CItemElem*)pItemBase;
	ItemProp* pItemProp  = pItemBase->GetProp();

	if( pItemProp->dwItemKind3 == IK3_LINK )
	{
		pItemProp	= prj.GetItemProp( pItemProp->dwActiveSkill );
		if( pItemProp == NULL )
			return FALSE;
	}

	DWORD dwItemKind1    = pItemProp->dwItemKind1;
	DWORD dwItemKind2    = pItemProp->dwItemKind2;
	DWORD dwItemKind3    = pItemProp->dwItemKind3;

	// 길드대전맵에서 선수만 사용
	CWorld* pWorld	= GetWorld();
	if( pWorld && pWorld->GetID() == WI_WORLD_GUILDWAR )
	{			
		if( m_nGuildCombatState == 0 )
		{
			if( IsPlayer() )
				((CUser*) this)->AddText( prj.GetText( TID_GAME_GUILDCOMBAT_STANDS_NOTUSEITEM ) );	//"수정해야함 : 관중석에서는 아이템을 사용할수 없습니다" );
			return FALSE;
		}
	}
#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans 석화디버프에 걸리면 아이템 사용금지
	if( GetAdjParam( DST_CHRSTATE ) & CHS_SETSTONE)
		return FALSE;
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans
	if( pItemProp->dwParts != NULL_ID )
	{
		// armor, weapon
		if( m_pActMover->IsActAttack() == FALSE )	// 공격중엔 장비 못바꿈.
		{
			DoUseEquipmentItem( pItemElem, dwId, nPart );
		}
		// 장착 아이템은 탈착 가능해야하므로 기간 만료에 의한 사용 제한 없음.
	}
	else	// 일반적인 아이템 사용 
	{
		if( IsItemRedyTime( pItemProp, pItemElem->m_dwObjId, TRUE ) == FALSE )
			return FALSE;

		DWORD dwGroup = 0;

		if( IsPlayer() )
		{
			CUser* pUser = (CUser*)this;

			dwGroup = pUser->m_cooltimeMgr.GetGroup( pItemProp );
			if( dwGroup )
			{
				if( pUser->m_cooltimeMgr.CanUse( dwGroup ) == FALSE )
				{
					return FALSE;
				}
			}
		}

		// 비 상용화 아이템
		switch( pItemProp->dwItemKind2 )
		{
#if __VER >= 15 // __GUILD_HOUSE
			case IK2_GUILDHOUSE_FURNITURE:
			case IK2_GUILDHOUSE_NPC:
			case IK2_GUILDHOUSE_PAPERING:
				{
					if( GuildHouseMng->SendWorldToDatabase( static_cast<CUser*>( this ), GUILDHOUSE_PCKTTYPE_LISTUP, GH_Fntr_Info( pItemProp->dwID ) ) )
						g_DPSrvr.PutItemLog( static_cast<CUser*>( this ), "f", "GUILDHOUSE_LISTUP", pItemElem, 1 );
					else
						return FALSE;
					break;
				}
			case IK2_GUILDHOUES_COMEBACK:
				{
					if( !GuildHouseMng->EnteranceGuildHouse( static_cast<CUser*>( this ), pItemProp->dwID ) )
						return FALSE;
					break;
				}			
#endif // __GUILD_HOUSE
#if __VER >= 13 // __HOUSING
			case IK2_FURNITURE:
			case IK2_PAPERING:
				{
					if(	CHousingMng::GetInstance()->ReqSetFurnitureList( (CUser*)this, pItemElem->m_dwItemId ) )
					{
						// 하우징 리스트 추가 로그
						LogItemInfo aLogItem;
						aLogItem.SendName	= GetName();
						aLogItem.RecvName = "HOUSING_USE";
						aLogItem.WorldId	= GetWorld()->GetID();
						aLogItem.Gold	= GetGold();
						aLogItem.Gold2	= GetGold();
						aLogItem.Action	= "f";
						g_DPSrvr.OnLogItem( aLogItem, pItemElem );
					}
					else
						return FALSE;
					break;
				}
#endif // __HOUSING
#ifdef __FUNNY_COIN
			case IK2_TOCASH:
				{
					if( !CFunnyCoin::GetInstance()->DoUseFunnyCoin( (CUser*)this, pItemElem ) )
						return FALSE;
				}
				break;
#endif // __FUNNY_COIN
			case IK2_WARP:
				return DoUseItemWarp( pItemProp, pItemElem );
#if __VER >= 11 // __SYS_COLLECTING
			case IK2_BUFF2:
				{
					if( IsDoUseBuff( pItemProp ) != 0 )
						return FALSE;
					CTime tm	= CTime::GetCurrentTime() + CTimeSpan( 0, 0, pItemProp->dwAbilityMin, 0 );
					time_t t	= (time_t)( tm.GetTime() );
					// wID: dwItemId
					// dwLevel
					AddBuff( BUFF_ITEM2, (WORD)( pItemElem->m_dwItemId ), t, 0 ); 
					break;
				}
#endif	// __SYS_COLLECTING
#ifdef __BUFF_TOGIFT
			case IK2_BUFF_TOGIFT:
#endif // __BUFF_TOGIFT
			case IK2_BUFF:
				{
					if( IsDoUseBuff( pItemProp ) != 0 )
						return FALSE;
					DoApplySkill( this, pItemProp, NULL );		// 대상에게 아이템효과를 적용함. 
#if __VER >= 8 //__CSC_VER8_5
					if( pItemProp->dwItemKind3 == IK3_ANGEL_BUFF )
					{
		#ifdef __EXP_ANGELEXP_LOG
						((CUser*)this)->m_nAngelExpLog = 0;
		#endif // __EXP_ANGELEXP_LOG
		#ifdef __ANGEL_LOG
						LogItemInfo aLogItem;
						aLogItem.Action = "&";
						aLogItem.SendName = ((CUser*)this)->GetName();
						aLogItem.RecvName = "ANGEL_SUMMON";
						aLogItem.WorldId = ((CUser*)this)->GetWorld()->GetID();
						aLogItem.Gold = aLogItem.Gold2 = ((CUser*)this)->GetGold();
						//aLogItem.ItemName = pItemProp->szName;
						_stprintf( aLogItem.szItemName, "%d", pItemProp->dwID );
						g_DPSrvr.OnLogItem( aLogItem );
		#endif // __ANGEL_LOG
						m_nAngelExp = 0;
						m_nAngelLevel = GetLevel() + 1;
						((CUser*)this)->AddAngelInfo();
					}
#endif // __CSC_VER8_5
				}
				break;
			case IK2_TEXT: //텍스트 문서 처리 
				// 퀘스트가 있으면 퀘스트 시작 
				if( pItemProp->dwQuestId && pItemElem->m_bItemResist == FALSE )
				{
					::__SetQuest( GetId(), pItemProp->dwQuestId );
					pItemElem->m_bItemResist = TRUE;
					UpdateItem( (BYTE)( pItemElem->m_dwObjId ), UI_IR, pItemElem->m_bItemResist );	
				}
				break;
			case IK2_SYSTEM:
				{
#ifdef __JEFF_9_20
#ifdef __WORLDSERVER
					if( pItemProp->dwLimitLevel1 != 0xFFFFFFFF )
					{
						if( GetLevel() < (int)( pItemProp->dwLimitLevel1 ) )
						{
							( (CUser*)this )->AddDefinedText( TID_GAME_ITEM_LEVEL, "\"%d\"", pItemProp->dwLimitLevel1 );
							return FALSE;
						}
					}
#endif	// __WORLDSERVER
#endif	// __JEFF_9_20
					int nResult = 0;
					nResult = DoUseItemSystem( pItemProp, pItemElem, nPart );
					{
						if( 0 < nResult )
						{
							if( nResult == 2 )
							{
								((CUser*)this)->AddDefinedText( TID_GAME_LIMITED_USE, "" ); // 사용중이어서 사용할수 없음
							}
							else if( nResult == 3 )
							{
								((CUser*)this)->AddDefinedText( TID_GAME_NOTOVERLAP_USE, "" ); // 이 아이템과는 중복하여 사용할 수 없습니다
							}
							return FALSE;
						}
						else
						{
							if( pItemElem->m_dwItemId == II_SYS_SYS_SCR_RECCURENCE_LINK )
							{
								g_dpDBClient.SendLogSMItemUse( "1", (CUser*)this, pItemElem, pItemProp );
							}

							//////////////////////////////////////////////////////////////////////////
							//	mulcom	BEGIN100125	이벤트용 리스킬 및 이벤트용 리스테트 사용 내역에 대한 로그 추가
							//						( e-mail : [유럽] 아이템 로그 추가 ( 2010-01-25 17:33 ) 참고 )
							else if( pItemElem->m_dwItemId == II_SYS_SYS_SCR_RECCURENCE && pItemElem->m_bCharged != TRUE )
							{
								g_DPSrvr.PutItemLog( (CUser*)( this ), "w", "USE_RECCURENCE_ITEM", pItemElem, 1 );
							}
							else if(  pItemElem->m_dwItemId == II_CHR_SYS_SCR_RESTATE && pItemElem->m_bCharged != TRUE )
							{
								g_DPSrvr.PutItemLog( (CUser*)( this ), "w", "USE_RESTATE_ITEM", pItemElem, 1 );
							}
							//	mulcom	END100125	이벤트용 리스킬 및 이벤트용 리스테트 사용 내역에 대한 로그 추가
							//////////////////////////////////////////////////////////////////////////
						}
					}
				}
				break;
#ifdef __NEW_PROP_PARAMETER
			case IK2_ONCE:
				{
					int nResult = 0;
					nResult = DoUseItemOnce( pItemProp, pItemElem, nPart );
					{
						if( 0 < nResult )
						{
							if( nResult == 2 )
							{
								((CUser*)this)->AddDefinedText( TID_GAME_LIMITED_USE, "" ); // 사용중이어서 사용할수 없음
							}
							else if( nResult == 3 )
							{
								((CUser*)this)->AddDefinedText( TID_GAME_NOTOVERLAP_USE, "" ); // 이 아이템과는 중복하여 사용할 수 없습니다
							}
							return FALSE;
						}
					}
				}
				break;
			case IK2_KEEP:
				{
#ifndef __NEW_STACKABLE_AMPS
					int nResult = 0;
					nResult = DoUseItemSystem( pItemProp, pItemElem, nPart );
					{
						if( 0 < nResult )
						{
							if( nResult == 2 )
							{
								((CUser*)this)->AddDefinedText( TID_GAME_LIMITED_USE, "" ); // 사용중이어서 사용할수 없음
							}
							else if( nResult == 3 )
							{
								((CUser*)this)->AddDefinedText( TID_GAME_NOTOVERLAP_USE, "" ); // 이 아이템과는 중복하여 사용할 수 없습니다
							}
							return FALSE;
						}
					}
#else //  __NEW_STACKABLE_AMPS
					int nResult = 0;
					nResult = DoUseItemKeep( pItemProp, pItemElem, nPart );
					{
						if( 0 < nResult )
						{

							if( nResult == 1 )
							{
								CString str;
								str.Format( "%d - %d", pItemProp->nMinLimitLevel, pItemProp->nMaxLimitLevel );
								((CUser*)this)->AddDefinedText( TID_GAME_EXP_NOTUSELEVEL, "\"%s\"", str ); // 
								return FALSE;
							}
							else if( nResult == 2 )
							{
								((CUser*)this)->AddDefinedText( TID_GAME_LIMITED_USE, "" ); // 사용중이어서 사용할수 없음
								return FALSE;
							}
							else if( nResult == 3 )
							{
								CString str;
								str.Format( "%d", GetLevel());
								CString szMaxBuf;
								szMaxBuf.Format( "%d", pItemProp->nMaxDuplication );
								((CUser*)this)->AddDefinedText( TID_GAME_EXP_NOTUSE, "\"%s\" \"%s\"", str, szMaxBuf ); // 
								//((CUser*)this)->AddDefinedText( TID_GAME_NOTOVERLAP_USE, "" ); // 이 아이템과는 중복하여 사용할 수 없습니다
								return FALSE;
							}
							else if( nResult == 4 )
							{
								int nExtraEXP = 0;
								for( MAPBUFF::iterator i = m_buffs.m_mapBuffs.begin(); i != m_buffs.m_mapBuffs.end(); ++i )
								{
									ItemProp* pAmpProp = prj.GetItemProp( i->second->GetId());

									if( pAmpProp )
									{
										if( pAmpProp->dwItemKind3 == IK3_EXP_RATE )
										{
											nExtraEXP += pAmpProp->nEffectValue;
										}
									}
								}
								CString strTemp;
								strTemp.Format( "%s %d%% %s", prj.GetText( TID_GAME_EXPITEM_TOOLTIP ), nExtraEXP, prj.GetText( TID_GAME_EXPITEM_TOOLTIP1 ) );
								((CUser*)this)->AddText( strTemp ); // 
							}
						}
					}
#endif // __NEW_STACKABLE_AMPS
				}
				break;
#endif // __NEW_PROP_PARAMETER
			case IK2_BLINKWING:
				return DoUseItemBlinkWing( pItemProp, pItemElem );

			case IK2_REFRESHER:
				{
					if( pItemProp->dwCircleTime != (DWORD)-1 )
					{
						if( IsSMMode( SM_MAINTAIN_MP ) == FALSE && SetSMMode( SM_MAINTAIN_MP, pItemProp->dwCircleTime ) )
						{
							SetPointParam( DST_MP, GetMaxManaPoint() );
						}
						else
						{
							((CUser*)this)->AddDefinedText( TID_GAME_LIMITED_USE, "" ); //   
							return FALSE;
						}
						g_dpDBClient.SendLogSMItemUse( "1", (CUser*)this, pItemElem, pItemProp );
					}
					else
					{
						if( DoUseItemFood( pItemProp, pItemElem ) == FALSE )
							return FALSE;
					}
					break;
				}
			case IK2_POTION:
				if( pItemProp->dwCircleTime != (DWORD)-1 )	// 상용아이템은 dwCircleTime을 썼더라. 나중에 dwSkillTime으로 통합예정.
				{
					if( IsSMMode( SM_MAINTAIN_FP ) == FALSE && SetSMMode( SM_MAINTAIN_FP, pItemProp->dwCircleTime ) )
					{
						SetPointParam( DST_FP, GetMaxFatiguePoint() );
					}
					else
					{
						((CUser*)this)->AddDefinedText( TID_GAME_LIMITED_USE, "" ); //   
						return FALSE;
					}
					g_dpDBClient.SendLogSMItemUse( "1", (CUser*)this, pItemElem, pItemProp );
				}
				else
				{
					if( DoUseItemFood( pItemProp, pItemElem ) == FALSE )
						return FALSE;
				}
				break;
			case IK2_FOOD:
				if( DoUseItemFood( pItemProp, pItemElem ) == FALSE )
					return FALSE;
				break;
			case IK2_AIRFUEL:	// 비행연료류
				{
					ItemProp *pRideProp = prj.GetItemProp( m_dwRideItemIdx );	// 빗자루 프로퍼티
					if( pRideProp && TRUE == IsPlayer() && m_pActMover->IsFly() )
					{
						if( pItemProp->dwItemKind3 == IK3_ACCEL )	// 가속 연료
						{
							m_tmAccFuel = pItemProp->dwAFuelReMax * 1000;;	// 몇초간 가속할수 있냐.
						}
						else
						{	// 비행연료
							if( pRideProp->dwItemKind3 == IK3_BOARD && pItemProp->dwItemKind3 == IK3_CFLIGHT )	// 타고있는건 보드 / 사용하려는건 빗자루용
							{
								((CUser*)this)->AddDefinedText( TID_GAME_COMBFUEL, "" );	// 이건 빗자루용이여~
								return FALSE;
							}
							else
							if( pRideProp->dwItemKind3 == IK3_STICK && pItemProp->dwItemKind3 == IK3_BFLIGHT )	// 타고있는건 빗자루 / 사용하려는건 보드용
							{
								((CUser*)this)->AddDefinedText( TID_GAME_BOARDFUEL, "" );	// 이건 보드용이여~
								return FALSE;
							}
								
							m_nFuel += pItemProp->dwFuelRe;		// 충전량.
							if( (DWORD)( m_nFuel ) >= pRideProp->dwFFuelReMax )	// 빗자루 연료통보다 크면 끝.
								m_nFuel = pRideProp->dwFFuelReMax;
						}
						((CUser *)this)->AddSetFuel( m_nFuel, m_tmAccFuel );		// 변화된 량을 전송. - SetPointParam에다 합쳐도 괜찮을듯?
					} else
						return FALSE;
				}
				break;

			case IK2_GMTEXT:
				{
					BOOL bGMTime = TRUE;
					if( pItemProp->dwSkillTime != NULL_ID )		// 지속시간이 있는 아이템
					{
						if( pItemProp->dwItemKind3 == IK3_TEXT_DISGUISE )	// 변신 아이템은 비행중에는 사용할수 없음.
						{
							if( IsFly() )
							{
								((CUser*)this)->AddDefinedText( TID_PK_FLIGHT_NOUSE, "" ); // 비행 중에는 사용할 수 없습니다.
								bGMTime = FALSE;
							}
#if __VER >= 14	// __INSTANCE_DUNGEON	// 인던 내 또는 입장시 변신구 사용 금지
							else if( CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( GetWorld()->GetID() ) )
							{
								((CUser*)this)->AddDefinedText( TID_GAME_INSTANCE_DISGUISE01 );
								bGMTime = FALSE;
							}
#endif // __INSTANCE_DUNGEON	// 인던 내 또는 입장시 변신구 사용 금지
						}
						if( bGMTime )
						{
							if( !HasBuffByIk3( pItemProp->dwItemKind3 ) )
							{
								DoApplySkill( this, pItemProp, NULL );		// 대상에게 아이템효과를 적용함. 
							}
							else
							{
								((CUser*)this)->AddDefinedText( TID_GAME_LIMITED_USE, "" ); // 사용중이어서 사용할수 없음
								bGMTime = FALSE;
							}
						}								
					}

					if(  pItemProp->dwID == II_SYS_SYS_GM_NODISGUISE )
					{
						if( HasBuffByIk3( IK3_TEXT_DISGUISE ) )
							RemoveIk3Buffs( IK3_TEXT_DISGUISE );
						else
							bGMTime = FALSE;
					}

					if( bGMTime )
					{
						if( pItemProp->dwID != II_SYS_SYS_GM_NODISGUISE )
						{
							char szGMCommand[64] = {0,};
							CString szGMText = pItemProp->szTextFileName;
							szGMText.Replace( '(', '\"' );
							szGMText.Replace( ')', '\"' ); 
							int nGMCount = szGMText.Find( "/", 1 );
							if( nGMCount != -1 )
								strncpy( szGMCommand, szGMText, nGMCount );
							else
								strcpy( szGMCommand, szGMText );
							ParsingCommand( szGMCommand, (CMover*)this, TRUE );
						}
					}
					else
						return FALSE;
				}
				break;
			case IK2_SKILL:
				{
					BOOL bUseItem = FALSE;
					DWORD dwActiveSkill = pItemProp->dwActiveSkill;
					if( dwActiveSkill != NULL_ID )		// 추가 발동 스킬이 있다.
					{
						if( pItemProp->dwActiveSkillRate == NULL_ID ||
							xRandom(100) < pItemProp->dwActiveSkillRate )	// 발동확률에 걸렸는가.
						{
							ItemProp* pSkillProp;
							pSkillProp = (ItemProp *)prj.GetSkillProp( dwActiveSkill );
							if( pSkillProp )
							{
								if( pSkillProp->dwUseChance == WUI_TARGETINGOBJ )	// 타겟에게 사용하는 방식.
								{
									if( IsPlayer() )
									{
										DWORD idTarget = ((CUser*)this)->m_idSetTarget;
										CMover *pTarget = prj.GetMover( idTarget );
										if( IsValidObj( pTarget ) )
										{
											DoActiveSkill( dwActiveSkill, pItemProp->dwActiveSkillLv, pTarget );	// dwActiveSkill 발동.
										}
									}
								} else
								if( pSkillProp->dwUseChance == WUI_NOW )		// 자신에게만 사용하는 방식.
								{
									DoActiveSkill( dwActiveSkill, pItemProp->dwActiveSkillLv, this );	// 발동
								}
							} else
								Error( "DoUseItem, IK2_SKILL, item %s -> %d skill not prop", pItemProp->szName, dwActiveSkill );
						}
					}
				}
				break;
#ifdef __TREASUREBOX
			case IK2_TREASURE:
				return FALSE;
				break;
#endif//__TREASUREBOX
			default:
				{
					if( pItemProp->dwSkillTime != NULL_ID )		// 지속시간이 있는 아이템
					{
						DoApplySkill( this, pItemProp, NULL );		// 대상에게 아이템효과를 적용함. 
					}
				}
		} // switch ik2

		switch( pItemProp->dwItemKind3 )
		{
			case IK3_EGG:
				DoUseSystemPet( pItemElem );
				break;
			case IK3_PET:
				DoUseEatPet( pItemElem );
				break;
#ifdef __NEW_ITEM_VARUNA
			case IK3_SUMMON_NPC:
				DoUseBarunaPet(pItemElem);
				break;
#endif // __NEW_ITEM_VARUNA

		}

		OnAfterUseItem( pItemProp );	// raiders 06.04.20
		pItemElem->UseItem();			// --m_nItemNum;

		if( pItemElem->m_bCharged )		// 상용화 아이템 로그
			g_dpDBClient.SendLogSMItemUse( "1", (CUser*)this, pItemElem, pItemProp );		

		CHAR cUIParam = UI_NUM;
		if( dwGroup )	// 쿨타임 아이템이면 사용시각을 기록한다.
		{
			CUser* pUser = (CUser*)this;
			pUser->m_cooltimeMgr.SetTime( dwGroup, pItemProp->GetCoolTime() );	
			cUIParam = UI_COOLTIME;
		}

		UpdateItem( (BYTE)( dwId ), cUIParam, pItemElem->m_nItemNum );	// 갯수가 0이면  아이템 삭제 , 전송 
	}
	
#endif // WORLDSERVER

	return TRUE;
}

// IK2_BLINKWING의 처리.
BOOL CMover::DoUseItemBlinkWing( ItemProp *pItemProp, CItemElem* pItemElem, BOOL bUse )
{
#ifdef __WORLDSERVER
	if( m_pActMover->IsSit() ) // 2005.12.08 raiders 앉아 있으면 안되게 
		return FALSE;

	if( IsStateMode( STATE_BASEMOTION_MODE ) ) 
		return FALSE;

	if( bUse )
	{
		if( GetWorld()->m_dwWorldID == WI_WORLD_KEBARAS )
			return FALSE;
	}

	if( GetWorld()->m_dwWorldID == WI_WORLD_GUILDWAR )
		return FALSE;

#if __VER >= 11 // __GUILD_COMBAT_1TO1
	if( g_GuildCombat1to1Mng.IsPossibleUser( (CUser*)this ) )
	{
		if( IsPlayer() )
			((CUser*)this)->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_NOBLINKWING );
		return FALSE;
	}
#endif // __GUILD_COMBAT_1TO1

#if __VER >= 12 // __SECRET_ROOM
	if( CSecretRoomMng::GetInstance()->IsInTheSecretRoom( this ) )
	{
		if( IsPlayer() )
			((CUser*)this)->AddDefinedText( TID_GAME_SECRETROOM_USEBRINKITEM );
		return FALSE;
	}
#endif // __SECRET_ROOM

#if __VER >= 13 // __RAINBOW_RACE
	if( CRainbowRaceMng::GetInstance()->IsEntry( ((CUser*)this)->m_idPlayer ) )
	{
		if( IsPlayer() )
			((CUser*)this)->AddDefinedText( TID_GAME_RAINBOWRACE_NOTELEPORT );
		return FALSE;
	}
#endif // __RAINBOW_RACE

#if __VER >= 13 // __HOUSING
	if( GetWorld()->GetID() == WI_WORLD_MINIROOM )	// 하우징에서는 블링크윙 금지.
		return FALSE;
#endif // __HOUSING

	// 제한레벨이 걸려있을때.
 	if( pItemProp->dwLimitLevel1 != NULL_ID && (int)( pItemProp->dwLimitLevel1 ) > GetLevel() )
	{
		if( TRUE == IsPlayer() )
			( (CUser*)this )->AddDefinedText( TID_GAME_USINGNOTLEVEL, "\"%s\"", pItemProp->szName );
		return FALSE;
	}

	if( pItemProp->dwItemKind3 == IK3_TOWNBLINKWING )	// IK3_TOWNBLINKWING
	{
		REPLACE_TYPE type = REPLACE_NORMAL;
		if( pItemProp->dwID == II_CHR_SYS_SCR_ESCAPEBLINKWING )
			type = REPLACE_FORCE; 

		// lodestar
		if( bUse )
		{
			if( IsUsableItem( pItemElem ) == FALSE )
				return FALSE;

			pItemElem->UseItem();
			UpdateItem( (BYTE)( pItemElem->m_dwObjId ), UI_NUM, pItemElem->m_nItemNum );
		}
		
		PRegionElem pRgnElem	= NULL;
		CWorld* pWorld	= GetWorld();
		if( !pWorld )
			return FALSE;

		if( IsChaotic() )
		{
#if __VER >= 8 // __S8_PK
			if( pWorld->GetID() != pWorld->m_dwIdWorldRevival && pWorld->m_dwIdWorldRevival != 0 )
				pRgnElem	= g_WorldMng.GetRevivalPosChao( pWorld->m_dwIdWorldRevival, pWorld->m_szKeyRevival );
			if( NULL == pRgnElem )	// Find near revival pos
				pRgnElem	= g_WorldMng.GetNearRevivalPosChao( pWorld->GetID(), GetPos() );
#else // __VER >= 8 // __S8_PK
			pRgnElem	= g_WorldMng.GetNearRevivalPosChao( pWorld->GetID(), GetPos() );
#endif // __VER >= 8 // __S8_PK
		}
		else

		{
			if( pWorld->GetID() != pWorld->m_dwIdWorldRevival && pWorld->m_dwIdWorldRevival != 0 )
				pRgnElem	= g_WorldMng.GetRevivalPos( pWorld->m_dwIdWorldRevival, pWorld->m_szKeyRevival );
			if( NULL == pRgnElem )	// Find near revival pos
				pRgnElem	= g_WorldMng.GetNearRevivalPos( pWorld->GetID(), GetPos() );
		}
		if( NULL != pRgnElem )
			REPLACE( g_uIdofMulti, pRgnElem->m_dwWorldId, pRgnElem->m_vPos, type, nRevivalLayer );
		else
			REPLACE( g_uIdofMulti, pWorld->GetID(), GetPos(), type, nDefaultLayer );
	}
	else
	{
		//		x: dwItemAtkOrder1
		//		y: dwItemAtkOrder2
		//		z: dwItemAtkOrder3
		//		angle: dwItemAtkOrder4
		//		dwWeaponType: worldid
		if( pItemProp->dwWeaponType == WI_WORLD_NONE || pItemProp->dwWeaponType == 0xffffffff )
			return FALSE;
		D3DXVECTOR3 vPos( (float)( pItemProp->dwItemAtkOrder1 ), (float)( pItemProp->dwItemAtkOrder2 ), (float)( pItemProp->dwItemAtkOrder3 ) );
		SetAngle( (float)pItemProp->dwItemAtkOrder4 );
		// remove & transfer
		if( bUse )
		{
			if( IsUsableItem( pItemElem ) == FALSE )
				return FALSE;

			pItemElem->UseItem();
			UpdateItem( (BYTE)( pItemElem->m_dwObjId ), UI_NUM, pItemElem->m_nItemNum );
		}
#if __VER >= 8 // __S8_PK
		if( IsChaotic() )
		{
			PRegionElem pRgnElem	= NULL;
			CWorld* pWorld = g_WorldMng.GetWorld( pItemProp->dwWeaponType );
			if( pWorld )
				pRgnElem	= g_WorldMng.GetRevivalPosChao( pItemProp->dwWeaponType, pItemProp->szTextFileName );
			if( NULL == pRgnElem )	// Find near revival pos
				pRgnElem	= g_WorldMng.GetNearRevivalPosChao( pWorld->GetID(), GetPos() );

			if( pRgnElem )
				REPLACE( g_uIdofMulti, pRgnElem->m_dwWorldId, pRgnElem->m_vPos, REPLACE_NORMAL, nRevivalLayer );
		}
		else
#endif // __VER >= 8 // __S8_PK
		{
			REPLACE( g_uIdofMulti, pItemProp->dwWeaponType, vPos, REPLACE_NORMAL, nTempLayer );
		}
	}
#endif // worldserver
	return TRUE;
}

int	CMover::DoUseItemSystem( ItemProp *pItemProp, CItemElem* pItemElem, int nPart )
{
	int nResult = 0;
#ifdef __WORLDSERVER
#ifdef __AZRIA_1023
	if( IsPlayer() )
	{
		CUser* pThis	= static_cast<CUser*>( this );
		if( pThis->HasInput() )
			return pThis->DoUseItemInput( pItemProp, pItemElem );
	}
#endif	// __AZRIA_1023
	switch( pItemProp->dwID )
	{
	case II_CHR_SYS_SCR_RESTATE:
		{
			if( m_nInt == 15 && m_nSta == 15 && m_nStr == 15 && m_nDex == 15 )
			{
				((CUser*)this)->AddDefinedText( TID_GAME_DONOTUSE_RESTATE );
				nResult = 1;
			}
			else
				ReState();
		}
		break;
#ifdef __S_ADD_RESTATE
	case II_CHR_SYS_SCR_RESTATE_STR:
		{
			if( m_nStr == 15 )
			{
				((CUser*)this)->AddDefinedText( TID_GAME_DONOTUSE_RESTATE );			
				nResult = 1;
			}
			else
				ReStateOne( 0 );
		}
		break;
	case II_CHR_SYS_SCR_RESTATE_STA:
		{
			if( m_nSta == 15 )
			{
				((CUser*)this)->AddDefinedText( TID_GAME_DONOTUSE_RESTATE );			
				nResult = 1;
			}
			else
				ReStateOne( 1 );
		}
		break;
	case II_CHR_SYS_SCR_RESTATE_DEX:
		{
			if( m_nDex == 15 )
			{
				((CUser*)this)->AddDefinedText( TID_GAME_DONOTUSE_RESTATE );			
				nResult = 1;
			}
			else
				ReStateOne( 2 );
		}
		break;
	case II_CHR_SYS_SCR_RESTATE_INT:
		{
			if( m_nInt == 15 )
			{
				((CUser*)this)->AddDefinedText( TID_GAME_DONOTUSE_RESTATE );			
				nResult = 1;
			}
			else
				ReStateOne( 3 );
		}
		break;
#endif // __S_ADD_RESTATE
	case II_CHR_SYS_SCR_TRANSY:
		{
			if( DoUseItemSexChange( nPart ) == FALSE )
			{
				nResult = 1;
			}
			else
			{
				if( HasBuffByIk3( IK3_TEXT_DISGUISE ) )
					RemoveIk3Buffs( IK3_TEXT_DISGUISE );
			}
		}
		break;
#if __VER >= 9	// __PET_0410
	case II_SYS_SYS_SCR_PET_LIFE:
	case II_SYS_SYS_SCR_PET_LIFE02:
		{
			CPet* pPet	= GetPet();
			if( pPet == NULL )
			{
				( (CUser*)this )->AddDefinedText( TID_GAME_PET_NOT_FOUND );
				nResult	= 1;
				break;
			}

			if( pPet->GetLevel() == PL_EGG )
			{
				( (CUser*)this )->AddDefinedText( TID_GAME_PET_CANT_USE_TO_EGG );
				nResult	= 1;
				break;
			}
			
			if( pPet->GetLife() >= MAX_PET_LIFE )
			{
				nResult	= 1;
				break;
			}

			WORD wLife	= pPet->GetLife();
			wLife	+= ( pItemProp->dwID == II_SYS_SYS_SCR_PET_LIFE? CPetProperty::GetInstance()->GetAddLife(): 1 );

			pPet->SetLife( wLife > MAX_PET_LIFE? MAX_PET_LIFE: wLife );
				
			CItemElem* pItemElem	= GetPetItem();
			( (CUser*)this )->AddPetState( pItemElem->m_dwObjId, pPet->GetLife(), pPet->GetEnergy(), pPet->GetExp() );
			g_dpDBClient.CalluspPetLog( m_idPlayer, pItemElem->GetSerialNumber(), wLife, PETLOGTYPE_LIFE, pPet );
			break;
		}
	case II_SYS_SYS_SCR_PET_HATCH:
		{
			CPet* pPet	= GetPet();
			if( pPet == NULL )
			{
				( (CUser*)this )->AddDefinedText( TID_GAME_PET_NOT_FOUND );
				nResult	= 1;
				break;
			}
			if( pPet->GetLevel() != PL_EGG )
			{
				( (CUser*)this )->AddDefinedText( TID_GAME_PET_4EGG );
				nResult	= 1;
				break;
			}
			PetLevelup();
			break;
		}
#endif	// __PET_0410
#if __VER >= 11 // __SYS_COLLECTING
	case II_GEN_TOO_COL_NORMALBATTERY:
		{
			CUser* pThis	= (CUser*)this;
			if( !pThis->DoUseItemBattery() )
			{
				pThis->AddDefinedText( TID_GAME_CANT_USE_BATTERY );
				nResult	= 1;
			}
			break;
		}
#endif	// __SYS_COLLECTING
	case II_CHR_SYS_SCR_SHOUTFULL15:
		{
			if( !IsShoutFull() )
				SetSMMode( SM_SHOUT15, pItemProp->dwCircleTime );
			else
				nResult = 2;
		}
		break;
	case II_CHR_SYS_SCR_SHOUTFULL30:
		{
			if( !IsShoutFull() )
				SetSMMode( SM_SHOUT30, pItemProp->dwCircleTime );
			else
				nResult = 2;
		}
		break;
	case II_CHR_SYS_SCR_SHOUTFULL001:
		{
			if( !IsShoutFull() )
				SetSMMode( SM_SHOUT001, pItemProp->dwCircleTime );
			else
				nResult = 2;
		}
		break;
	case II_CHR_SYS_SCR_PSKILLFULL1:
		{
			if( !( IsSMMode( SM_PARTYSKILL1 ) || IsSMMode( SM_PARTYSKILL15 ) || IsSMMode( SM_PARTYSKILL30 ) ) )
#if __VER >= 12 // __PARSKILL1001	//12차 파스킬 아이템 수정  world,core,neuz
			{
				SetSMMode( SM_PARTYSKILL1, pItemProp->dwCircleTime );
				g_DPCoreClient.SendUserPartySkill( m_idPlayer, PARTY_PARSKILL_MODE, 1000, 0, 1 );
			}
#else	//__PARSKILL1001	//12차 파스킬 아이템 수정  world,core,neuz
				SetSMMode( SM_PARTYSKILL1, pItemProp->dwCircleTime );
#endif //__PARSKILL1001	//12차 파스킬 아이템 수정  world,core,neuz
			else
				nResult = 2;
		}
		break;
	case II_CHR_SYS_SCR_PSKILLFULL15:
		{
			if( !( IsSMMode( SM_PARTYSKILL1 ) || IsSMMode( SM_PARTYSKILL15 ) || IsSMMode( SM_PARTYSKILL30 ) ) )
#if __VER >= 12 // __PARSKILL1001	//12차 파스킬 아이템 수정  world,core,neuz
			{
				SetSMMode( SM_PARTYSKILL15, pItemProp->dwCircleTime );
				g_DPCoreClient.SendUserPartySkill( m_idPlayer, PARTY_PARSKILL_MODE, 1000, 0, 1 );
			}
#else	//__PARSKILL1001	//12차 파스킬 아이템 수정  world,core,neuz
				SetSMMode( SM_PARTYSKILL15, pItemProp->dwCircleTime );
#endif //__PARSKILL1001	//12차 파스킬 아이템 수정  world,core,neuz
			else
				nResult = 2;
		}
		break;
	case II_CHR_SYS_SCR_PSKILLFULL30:
		{
			if( !( IsSMMode( SM_PARTYSKILL1 ) || IsSMMode( SM_PARTYSKILL15 ) || IsSMMode( SM_PARTYSKILL30 ) ) )
#if __VER >= 12 // __PARSKILL1001	//12차 파스킬 아이템 수정  world,core,neuz
			{
				SetSMMode( SM_PARTYSKILL30, pItemProp->dwCircleTime );
				g_DPCoreClient.SendUserPartySkill( m_idPlayer, PARTY_PARSKILL_MODE, 1000, 0, 1 );
			}
#else	//__PARSKILL1001	//12차 파스킬 아이템 수정  world,core,neuz
				SetSMMode( SM_PARTYSKILL30, pItemProp->dwCircleTime );
#endif //__PARSKILL1001	//12차 파스킬 아이템 수정  world,core,neuz
			else
				nResult = 2;
		}
		break;
	case II_CHR_SYS_SCR_COMMBANK15:
		{
			if( !IsCommBank() )
				SetSMMode( SM_BANK15, pItemProp->dwCircleTime );
			else
				nResult = 2;
		}
		break;
	case II_CHR_SYS_SCR_COMMBANK30:
		{
			if( !IsCommBank() )
				SetSMMode( SM_BANK30, pItemProp->dwCircleTime );
			else
				nResult = 2;
		}
		break;
	case II_CHR_SYS_SCR_COMMBANK001:
		{
			if( !IsCommBank() )
				SetSMMode( SM_BANK001, pItemProp->dwCircleTime );
			else
				nResult = 2;
		}
		break;
	case II_CHR_SYS_SCR_ACTIVITION:
		{
			if( !( IsSMMode( SM_ACTPOINT ) ) )
				SetSMMode( SM_ACTPOINT, pItemProp->dwCircleTime );
			else
				nResult = 2;
		}
		break;
	case II_CHR_SYS_SCR_UPCUTSTONE01:
		{
			if( !( IsSMMode( SM_ATTACK_UP1 ) || IsSMMode( SM_ATTACK_UP ) ) )
				SetSMMode( SM_ATTACK_UP1, pItemProp->dwCircleTime );
			else
				nResult = 2;
		}
		break;
	case II_CHR_SYS_SCR_UPCUTSTONE:
		{
			if( !( IsSMMode( SM_ATTACK_UP1 ) || IsSMMode( SM_ATTACK_UP ) ) )
			{
				SetSMMode( SM_ATTACK_UP, pItemProp->dwCircleTime );
			}
			else
				nResult = 2;
		}
		break;
#if __VER < 8 // __S8_PK
	case II_SYS_SYS_SCR_FORGIVE:
		if( IsPlayer() )
		{
			if( IsChaotic() )
			{
				UpgradeKarma();
				((CUser*)this)->AddDefinedText( TID_GAME_GETKARMA );	// 카르마가 1단계 회복되었습니다						
			}
			else
			{
				// 카오틱 플레이어의 경우만 사용 할 수 있습니다.
				((CUser*)this)->AddDefinedText( TID_GAME_NOTCHOAS );
				nResult = 1;		// 1 - 아이템이 사용안되게 한다.
			}
		}
		break;
#endif // __VER < 8 // __S8_PK
	case II_SYS_SYS_SCR_BLESSING:
		{
			if( !( IsSMMode( SM_REVIVAL ) ) )
				SetSMMode( SM_REVIVAL, pItemProp->dwCircleTime );
			else
				nResult = 2;					
		}
		break;
	case II_SYS_SYS_SCR_SELPAR:
		{
			CParty* pParty;
			pParty = g_PartyMng.GetParty( GetPartyId() );
			if( pParty )
			{
				if( pParty->m_nLevel < MAX_PARTYLEVEL )	// 단막극단일때만 사용가능
					pParty->SetPartyLevel( ((CUser*)this), 10, 180, 0 );
				else
					nResult = 1;
			}
			else
			{
				nResult = 1;
			}
			if( nResult == 1 )
				((CUser*)this)->AddDefinedText( TID_GAME_NTROUPEO10, "" );
		}
		break;
	case II_SYS_SYS_SCR_HOLY:
		RemoveDebufBuffs();
		break;
	case II_SYS_SYS_SCR_VELOCIJUMP:
		{
			if( !( IsSMMode( SM_VELOCIJUMP ) ) )
			{
				if( pItemProp->dwDestParam1 != -1 )
					SetDestParam( pItemProp->dwDestParam1, pItemProp->nAdjParamVal1, pItemProp->nAdjParamVal1 );
				if( pItemProp->dwDestParam2 != -1 )
					SetDestParam( pItemProp->dwDestParam2, pItemProp->nAdjParamVal2, pItemProp->nAdjParamVal2 );
				SetSMMode( SM_VELOCIJUMP, pItemProp->dwCircleTime );
			}
			else
			{
				nResult = 2;	
			}
		}
		break;
	case II_SYS_SYS_SCR_RECCURENCE:
		{
			if( InitSkillExp() )
				((CUser*)this)->AddInitSkill();
			else
				nResult = 1;
		}
		break;
#ifndef __NEW_STACKABLE_AMPS
	case II_SYS_SYS_SCR_AMPES:	// "ES증폭의 두루마리"		
		{
			// ES, EM은 동일안 아이템.. I는 같이 사용할수 업습니다.
			if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPES ) || HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPEM ) ||
				HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESA ) || HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESA1) || HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESA2) ||
				HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESB ) || HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESB1) || HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESB2) ||
				HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESC ) || HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESC1) || HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESC2) || 
				HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESD ) || HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESD1) || HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESD2) ||
#ifdef __S_ADD_EXP
				HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESS ) || HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESS1) || HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESS2) ||
#endif // __S_ADD_EXP
				HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESE )
				)
				nResult = 2;
			else if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPI ) )
				nResult = 3;
			else
				DoApplySkill( (CCtrl*)this, pItemProp, NULL );
		}
		break;
	case II_SYS_SYS_SCR_AMPESA:
	case II_SYS_SYS_SCR_AMPESB:
	case II_SYS_SYS_SCR_AMPESC:
	case II_SYS_SYS_SCR_AMPESD:
	case II_SYS_SYS_SCR_AMPESE:
#ifdef __S_ADD_EXP
	case II_SYS_SYS_SCR_AMPESS:
#endif // __S_ADD_EXP
		{
			if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPES ) || HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPEM ) )
				nResult = 2;
			else if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPI ) )
				nResult = 3;

			// 레벨 검사
			int nLevel = GetLevel();

			CString str;
			if( pItemProp->dwID == II_SYS_SYS_SCR_AMPESA )
			{
				if( 20 < nLevel )
				{
					str.Format( "%d - %d", 1, 20 );
					((CUser*)this)->AddDefinedText( TID_GAME_EXP_NOTUSELEVEL, "\"%s\"", str ); // 
					nResult = 99;
				}
			}
			else if( pItemProp->dwID == II_SYS_SYS_SCR_AMPESB )
			{
				if( nLevel < 21 || 40 < nLevel )
				{
					str.Format( "%d - %d", 21, 40 );
					((CUser*)this)->AddDefinedText( TID_GAME_EXP_NOTUSELEVEL, "\"%s\"", str ); // 
					nResult = 99;
				}
			}
			else if( pItemProp->dwID == II_SYS_SYS_SCR_AMPESC )
			{
				if( nLevel < 41 || 60 < nLevel )
				{
					str.Format( "%d - %d", 41, 60 );
					((CUser*)this)->AddDefinedText( TID_GAME_EXP_NOTUSELEVEL, "\"%s\"", str ); // 
					nResult = 99;
				}
			}
			else if( pItemProp->dwID == II_SYS_SYS_SCR_AMPESD )
			{
				if( nLevel < 61 || 80 < nLevel )
				{
					str.Format( "%d - %d", 61, 80 );
					((CUser*)this)->AddDefinedText( TID_GAME_EXP_NOTUSELEVEL, "\"%s\"", str ); // 
					nResult = 99;
				}
			}
			else if( pItemProp->dwID == II_SYS_SYS_SCR_AMPESE )
			{
				if( nLevel < 81 || 120 < nLevel )
				{
					str.Format( "%d - %d", 81, 120 );
					((CUser*)this)->AddDefinedText( TID_GAME_EXP_NOTUSELEVEL, "\"%s\"", str ); // 
					nResult = 99;
				}
			}

			if( !(nResult == 2 || nResult == 3 || nResult == 99 ) )
			{
				BOOL bBuf[3];
				int nBufCount = 0;
				BOOL bSpecial = FALSE;
				int nSpecialCount = 0;

				ZeroMemory( bBuf, sizeof( bBuf ) );
				if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESA ) )
				{
					bBuf[0] = TRUE;		++nBufCount;
				}
				if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESA1 ) )
				{
					bBuf[1] = TRUE;		++nBufCount;
				}
				if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESA2 ) )
				{
					bBuf[2] = TRUE;		++nBufCount;
				}

				if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESB ) )
				{
					bBuf[0] = TRUE;		++nBufCount;
				}
				if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESB1 ) )
				{
					bBuf[1] = TRUE;		++nBufCount;
				}
				if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESB2 ) )
				{
					bBuf[2] = TRUE;		++nBufCount;
				}

				if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESC ) )
				{
					bBuf[0] = TRUE;		++nBufCount;
				}
				if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESC1 ) )
				{
					bBuf[1] = TRUE;		++nBufCount;
				}
				if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESC2 ) )
				{
					bBuf[2] = TRUE;		++nBufCount;
				}
				if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESD ) )
				{
					bBuf[0] = TRUE;		++nBufCount;
				}
				if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESD1 ) )
				{
					bBuf[1] = TRUE;		++nBufCount;
				}
				if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESD2 ) )
				{
					bBuf[2] = TRUE;		++nBufCount;
				}
				if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESE ) )
				{
					bBuf[0] = TRUE;		++nBufCount;
				}
#ifdef __S_ADD_EXP
				if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESS ) )
				{
					bBuf[0] = TRUE;		++nBufCount;	nSpecialCount++;//bSpecial = TRUE;
				}
				if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESS1 ) )
				{
					bBuf[1] = TRUE;		++nBufCount;	nSpecialCount++;//bSpecial = TRUE;
				}
				if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESS2 ) )
				{
					bBuf[2] = TRUE;		++nBufCount;	nSpecialCount++;//bSpecial = TRUE;
				}
#endif // __S_ADD_EXP

				int nMaxBuf = 3;
				//if( bSpecial == FALSE )
				{				
					if( nLevel < 41 )
						nMaxBuf = 3;				
					else if( nLevel < 61 && nSpecialCount < 2 )
						nMaxBuf = 2;
					else if( nLevel <= 120 && nSpecialCount < 1 )
						nMaxBuf = 1;
					else
						nMaxBuf = 3;
				}

#ifdef __S_ADD_EXP
				//if( bSpecial && pItemProp->dwID != II_SYS_SYS_SCR_AMPESS )
				if( nSpecialCount && pItemProp->dwID != II_SYS_SYS_SCR_AMPESS )
				{
					nResult = 2;					
				}
				else
#endif // __S_ADD_EXP
				if( nBufCount < nMaxBuf )
				{
					for( int i = 0 ; i < nMaxBuf ; ++i )
					{
						if( bBuf[i] == FALSE )
						{
							if( i == 0 )
								pItemProp = prj.GetItemProp( pItemProp->dwID );
							else if( i == 1 )
								pItemProp = prj.GetItemProp( pItemProp->dwID + 1 );
							else 
								pItemProp = prj.GetItemProp( pItemProp->dwID + 2 );	
							break;
						}							
					}

					DoApplySkill( (CCtrl*)this, pItemProp, NULL );
					float fFactor = 1;
					switch( nBufCount + 1)
					{
					case 1:
						{
							fFactor *= 1.5f;
						}
						break;
					case 2:
						{
							//if( bSpecial || nLevel < 61 )
							if( nSpecialCount || nLevel < 61 )
								fFactor *= 2.0f;
							else
								fFactor *= 1.5f;
						}
						break;
					case 3:
						{
							//if( bSpecial || nLevel < 41 )
							if( nSpecialCount || nLevel < 41 )
								fFactor *= 2.5f;
							else if( nLevel < 61 )
								fFactor *= 2.0f;
							else
								fFactor *= 1.5f;
						}
						break;
					}
					CString strTemp;
#if __VER >= 11 // __EXPITEM_TOOLTIP_CHANGE
					int nPercent = (int)( (fFactor - 1.0f) * 100.0f );
					strTemp.Format( "%s %d%% %s", prj.GetText( TID_GAME_EXPITEM_TOOLTIP ), nPercent, prj.GetText( TID_GAME_EXPITEM_TOOLTIP1 ) );
#else //__EXPITEM_TOOLTIP_CHANGE
					strTemp.Format( "%s%.1f%s", prj.GetText( TID_GAME_EXPTOOLTIP ), fFactor, prj.GetText( TID_GAME_EXPTOOLTIP1 ) );
#endif //__EXPITEM_TOOLTIP_CHANGE
					((CUser*)this)->AddText( strTemp ); // 
				}
				else
				{
#ifdef __S_ADD_EXP
					//if( bSpecial )
					//{
					//	((CUser*)this)->AddDefinedText( TID_GAME_EXP_3 ); //
					//}
//					else
#endif // __S_ADD_EXP
					{
						if( nLevel < 21 )
							str.Format( "%d - %d", 1, 20 );
						else if( nLevel < 41 )
							str.Format( "%d - %d", 21, 40 );
						else if( nLevel < 61 )
							str.Format( "%d - %d", 41, 60 );
						else if( nLevel < 81 )
							str.Format( "%d - %d", 61, 80 );
						else if( nLevel <= 120 )
							str.Format( "%d - %d", 81, 120 );

						CString szMaxBuf;
						szMaxBuf.Format( "%d", nMaxBuf );
						((CUser*)this)->AddDefinedText( TID_GAME_EXP_NOTUSE, "\"%s\" \"%s\"", str, szMaxBuf ); // 
					}
					nResult = 99;					
				}				
			}
		}
		break;
	case II_SYS_SYS_SCR_AMPEM:	// "EM증폭의 두루마리"		
		{
			// ES, EM은 동일안 아이템.. I는 같이 사용할수 업습니다.
			if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPES ) || HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPEM ) ||
				HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESA ) || HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESA1) || HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESA2) ||
				HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESB ) || HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESB1) || HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESB2) ||
				HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESC ) || HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESC1) || HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESC2) || 
				HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESD ) || HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESD1) || HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESD2) ||
#ifdef __S_ADD_EXP
				HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESS ) || HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESS1) || HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESS2) ||
#endif // __S_ADD_EXP
				HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPESE ) 
				)
				nResult = 2;
			else if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_AMPI ) )
				nResult = 3;
			else
				DoApplySkill( (CCtrl*)this, pItemProp, NULL );
		}
		break;
#endif // __NEW_STACKABLE_AMPS
#ifdef __Y_BEAUTY_SHOP_CHARGE
	case II_SYS_SYS_SCR_BEAUTYSHOP: // "Beauty Shop Charge"		
		{
		}
		break;
#endif // __Y_BEAUTY_SHOP_CHARGE
	case II_SYS_SYS_SCR_GLVCHEER:	// 응원의 흰 장갑
	case II_SYS_SYS_SCR_SMELPROT:	// "보호의 두루마리"		
#ifdef __SM_ITEM_2ND_EX
	case II_SYS_SYS_SCR_SMELPROT2:
#endif	// __SM_ITEM_2ND_EX
#if __VER >= 9 // __ULTIMATE
	case II_SYS_SYS_SCR_SMELPROT3: // " 보호의 두루마리(최상급)
#endif // __ULTIMATE
#if __VER >= 11 // __SYS_COLLECTING
	case II_SYS_SYS_SCR_SMELPROT4:	// 악세사리 보호의 두루마리
#endif	// __SYS_COLLECTING
	case II_SYS_SYS_SCR_SMELTING:	// "제련의 두루마리"	
	case II_SYS_SYS_SCR_SUPERLEADERPARTY:	// "단장의 축복"		
	case II_SYS_SYS_SCR_SUPERSMELTING:	// "제련 두루마리(진)"
	case II_SYS_SYS_SCR_FONTEDIT:	// "Font Edit"		
#ifdef __Y_BEAUTY_SHOP_CHARGE
	case II_SYS_SYS_SCR_VENDOR:	// "Vendor Charge"		
#endif //__Y_BEAUTY_SHOP_CHARGE
	case II_SYS_SYS_SCR_SKLINSTRUCT:	// "기술의 전수자"
#if __VER >= 14 // __EXT_ATTRIBUTE
	case II_SYS_SYS_SCR_SMELTING2:	// 속성 제련의 두루마리
#endif // __EXT_ATTRIBUTE

		{
			if( HasBuff( BUFF_ITEM, (WORD)( pItemProp->dwID ) ) )
				nResult = 2;
			else
				DoApplySkill( (CCtrl*)this, pItemProp, NULL );
		}
		break;
		//	mulcom	BEGIN100405	각성 보호의 두루마리
#ifdef	__PROTECT_AWAKE
	case	II_SYS_SYS_SCR_AWAKESAFE:
		{
			if( HasBuff( BUFF_ITEM, (WORD)( pItemProp->dwID ) ) )
			{
				nResult = 2;
			}
			else
			{
				DoApplySkill( (CCtrl*)this, pItemProp, NULL );
			}
		}
		break;
#endif	//__PROTECT_AWAKE
		//	mulcom	END100405	각성 보호의 두루마리


#if __VER >= 9	// __PET_0410
	case II_SYS_SYS_SCR_PET_TONIC_A:
	case II_SYS_SYS_SCR_PET_TONIC_B:
#if __VER >= 12 // __PET_0519
		nResult		= DoUseItemPetTonic( pItemElem );	// 펫 영양제 사용
		break;
#else	// __PET_0519
		if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_PET_TONIC_A ) || HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_PET_TONIC_B ) )
			nResult = 2;
		else
			DoApplySkill( (CCtrl*)this, pItemProp, NULL );
		break;
#endif	// __PET_0519
	case II_SYS_SYS_SCR_PET_FEED_POCKET:
		{
			nResult = DoUseItemFeedPocket( pItemElem );
			break;
		}
#endif	// __PET_0410
#ifdef __EVE_BALLOON
#if __VER < 14 // __BALLOON_CODE_IMPROVEMENT
	case II_SYS_SYS_EVE_BALLOON:
	case II_SYS_SYS_EVE_BALLOON01:
	case II_SYS_SYS_EVE_BALLOON02:
	case II_SYS_SYS_EVE_BALLOON_01:
	case II_SYS_SYS_EVE_BALLOON01_01:
	case II_SYS_SYS_EVE_BALLOON02_01:
		{
			if( HasBuff( BUFF_ITEM, II_SYS_SYS_EVE_BALLOON )
				|| HasBuff( BUFF_ITEM, II_SYS_SYS_EVE_BALLOON01 )
				|| HasBuff( BUFF_ITEM, II_SYS_SYS_EVE_BALLOON02 )
				|| HasBuff( BUFF_ITEM, II_SYS_SYS_EVE_BALLOON_01 )
				|| HasBuff( BUFF_ITEM, II_SYS_SYS_EVE_BALLOON01_01 )
				|| HasBuff( BUFF_ITEM, II_SYS_SYS_EVE_BALLOON02_01 )
			)
				nResult = 2;
			else
				DoApplySkill( (CCtrl*)this, pItemProp, NULL );
		}
		break;
#endif // __BALLOON_CODE_IMPROVEMENT
#endif //__EVE_BALLOON
#if __VER >= 9 // __II_SYS_SYS_SCR_GET
	case II_SYS_SYS_SCR_GET01:
	case II_SYS_SYS_SCR_GET02:
		{
			if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_GET01 ) ||
				HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_GET02 ) )
				nResult = 2;
			else
				DoApplySkill( (CCtrl*)this, pItemProp, NULL );
		}
		break;
#endif // __II_SYS_SYS_SCR_GET
	case II_SYS_SYS_SCR_RETURN:		// "귀환의 두루마리"		
		{
			if( IsPlayer() )
			{
				if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_RETURN ) )
					nResult = 2;
				else
				{
					CWorld* pWorld	= GetWorld();
					if( !pWorld )
						return 0;

					D3DXVECTOR3 vPos = GetPos();
					//클락워크 돔/ 케바라스에서는 이 아이템을 사용할 수 없으며 만약 사용하려 시도 시 '이 지역은 제한 지역이므로 아이템을 사용할 수 없습니다.'라는 메시지를 출력한다
					if( pWorld->GetID() == WI_WORLD_GUILDWAR || pWorld->GetID() == WI_WORLD_KEBARAS || prj.IsGuildQuestRegion( vPos ) )
					{			
						((CUser*) this)->AddDefinedText( TID_GAME_LIMITZONE_USE );
						return 0;
					}

#if __VER >= 11 // __GUILD_COMBAT_1TO1
					if( g_GuildCombat1to1Mng.IsPossibleUser( (CUser*)this ) )
					{
						((CUser*) this)->AddDefinedText( TID_GAME_LIMITZONE_USE );
						return 0;
					}
#endif // __GUILD_COMBAT_1TO1

					((CUser*) this)->SetReturnPos( vPos );

					//1.귀환모드로 설정 
					DoApplySkill( (CCtrl*)this, pItemProp, NULL );
					//2. 선택된 마을로 이동 
					LPCTSTR lpszKey = ((CUser*)this)->m_lpszVillage;
					if( lpszKey )
					{
						PRegionElem pRgnElem = g_WorldMng.GetRevivalPos( GetWorld()->GetID(), lpszKey );
						if( pRgnElem )
							REPLACE( g_uIdofMulti, pRgnElem->m_dwWorldId, pRgnElem->m_vPos, REPLACE_NORMAL, nRevivalLayer );
					}
					else
					{
						Error( "II_SYS_SYS_SCR_RETURN selected village is NULL\n" );
					}
				}
			}
		}
		break;
	case II_SYS_SYS_SCR_CUSTODY2:
		{
			CMover* pMover	= (CMover*)CreateObj( D3DDEVICE, OT_MOVER, MI_INFO_PENG );
			lstrcpy( pMover->m_szCharacterKey, "MaFl_InstantBank" );
			pMover->InitNPCProperty();
			pMover->InitCharacter( pMover->GetCharacter() );
			pMover->SetPos( GetPos() );
			pMover->InitMotion( MTI_STAND );
			pMover->UpdateLocalMatrix();
			pMover->m_dwTickCreated		= GetTickCount();
			GetWorld()->ADDOBJ( pMover, TRUE, GetLayer() );
			break;
		}
	
	case II_SYS_SYS_SCR_FRIENDSUMMON_A:
	case II_SYS_SYS_SCR_FRIENDSUMMON_B:
		{
			int nState = GetSummonState();
			if( nState != 0 )
			{
				DWORD dwMsgId = 0;
				if( nState == 1 )	// 거래중
					dwMsgId = TID_GAME_TRADE_NOTUSE;
				else if( nState == 2 ) // 죽음
					dwMsgId = TID_GAME_DIE_NOTUSE;
				else if( nState == 3 ) // 개인상점 중
					dwMsgId = TID_GAME_VENDOR_NOTUSE;
				else if( nState == 4 ) // 전투중
					dwMsgId = TID_GAME_ATTACK_NOTUSE;
				else if( nState == 5 ) // 비행중
					dwMsgId = TID_GAME_FLY_NOTUSE;
				
				((CUser*) this)->AddDefinedText( TID_GAME_STATE_NOTUSE , "\"%s\"", prj.GetText( dwMsgId ) );
			}
#if __VER >= 11 // __GUILD_COMBAT_1TO1
			else if( g_GuildCombat1to1Mng.IsPossibleUser( (CUser*)this ) )
			{
				((CUser*) this)->AddDefinedText( TID_GAME_STATE_NOTUSE , "\"%s\"", prj.GetText( TID_GAME_ATTACK_NOTUSE ) );
			}
#endif // __GUILD_COMBAT_1TO1
			else
			{
				((CUser*) this)->AddSummonFriendUse( pItemElem );	
			}
			nResult = 99;
		}
		break;

#if __VER >= 11 // __MA_VER11_02	// 수표 개념 화페 '페린' 추가
	case II_SYS_SYS_SCR_PERIN:
		{
			int nState = GetSummonState();
			if( nState != 0 )
			{
				DWORD dwMsgId = 0;
				if( nState == 1 )	// 거래중
					dwMsgId = TID_GAME_TRADE_NOTUSE;
				else if( nState == 2 ) // 죽음
					dwMsgId = TID_GAME_DIE_NOTUSE;
				else if( nState == 3 ) // 개인상점 중
					dwMsgId = TID_GAME_VENDOR_NOTUSE;
				else if( nState == 4 ) // 전투중
					dwMsgId = TID_GAME_ATTACK_NOTUSE;
				else if( nState == 5 ) // 비행중
					dwMsgId = TID_GAME_FLY_NOTUSE;
				
				((CUser*) this)->AddDefinedText( TID_GAME_STATE_NOTUSE , "\"%s\"", prj.GetText( dwMsgId ) );
				nResult = 99;
			}
			else
			{
				float	fTmpGold = (float)( ((CUser*) this)->GetGold() );
				if( fTmpGold >= 2000000000 || fTmpGold < 0  )
				{
					((CUser*) this)->AddDiagText( prj.GetText(TID_GAME_TOOMANYMONEY_USE_PERIN) );
					return 99;
				}
				else
				{
					((CUser*) this)->AddGold(100000000,true);

					// chipi080711 - 페린사용 로그
					LogItemInfo aLogItem;
					aLogItem.Action = "C";
					aLogItem.SendName = ((CUser*) this)->GetName();
					aLogItem.RecvName = "USE_PERIN";
					aLogItem.WorldId = ((CUser*) this)->GetWorld()->GetID();
					aLogItem.Gold = (DWORD)( fTmpGold );
					aLogItem.Gold2 = ((CUser*) this)->GetGold();
					aLogItem.Gold_1  = ((CUser*) this)->GetGold() - static_cast<int>(fTmpGold);
					//aLogItem.ItemName = pItemElem->GetName();
					_stprintf( aLogItem.szItemName, "%d", pItemProp->dwID );
					aLogItem.itemNumber = 1;
					aLogItem.ItemNo = pItemElem->GetSerialNumber();
					g_DPSrvr.OnLogItem( aLogItem );
				}
			}
		}
		break;
#endif	// __MA_VER11_02
#ifdef __ADD_RESTATE_LOW
	case II_CHR_SYS_SCR_RESTATE_STR_LOW:
		{
			if( m_nStr == 15 )
			{
				((CUser*)this)->AddDefinedText( TID_GAME_DONOTUSE_RESTATE );			
				nResult = 1;
			}
			else
				ReStateOneLow( 0 );
		}
		break;
	case II_CHR_SYS_SCR_RESTATE_STA_LOW:
		{
			if( m_nSta == 15 )
			{
				((CUser*)this)->AddDefinedText( TID_GAME_DONOTUSE_RESTATE );			
				nResult = 1;
			}
			else
				ReStateOneLow( 1 );
		}
		break;
	case II_CHR_SYS_SCR_RESTATE_DEX_LOW:
		{
			if( m_nDex == 15 )
			{
				((CUser*)this)->AddDefinedText( TID_GAME_DONOTUSE_RESTATE );			
				nResult = 1;
			}
			else
				ReStateOneLow( 2 );
		}
		break;
	case II_CHR_SYS_SCR_RESTATE_INT_LOW:
		{
			if( m_nInt == 15 )
			{
				((CUser*)this)->AddDefinedText( TID_GAME_DONOTUSE_RESTATE );			
				nResult = 1;
			}
			else
				ReStateOneLow( 3 );
		}
		break;
#endif // __ADD_RESTATE_LOW

	default:
			nResult = 1;
		break;
	}

#ifndef __AZRIA_1023
#ifdef __SYS_TICKET
	if( pItemProp->dwItemKind3 == IK3_TICKET )
	{
		( (CUser*)this )->DoUseItemTicket( pItemElem );
		nResult		= 0;
	}
#endif	// __SYS_TICKET
#endif	// __AZRIA_1023

#if __VER >= 14 // __BALLOON_CODE_IMPROVEMENT
	if( pItemProp->dwItemKind3 == IK3_BALLOON )
	{
		if( HasBuffByIk3( IK3_BALLOON ) != FALSE )
			nResult = 2;
		else
		{
			DoApplySkill( (CCtrl*)this, pItemProp, NULL );
			nResult = 0;
		}
	}
#endif // __BALLOON_CODE_IMPROVEMENT

#endif // __WORLDSERVER

	return nResult;
}

#ifdef __NEW_PROP_PARAMETER
int CMover::DoUseItemOnce( ItemProp* pItemProp, CItemElem* pItemElem, int nPart)
{
	int nResult = 0;
#ifdef __WORLDSERVER
	if( HasBuffByIk3( pItemProp->dwItemKind3 ))
		nResult = 2;
	else
		DoApplySkill( (CCtrl*)this, pItemProp, NULL );
#endif // __WORLDSERVER
	return nResult;
}
int CMover::DoUseItemKeep( ItemProp* pItemProp, CItemElem* pItemElem, int nPart)
{
#ifdef __WORLDSERVER
#ifdef __NEW_STACKABLE_AMPS
// First Check how many Amps are currently active.
	int nAmount = 0;

	for( MAPBUFF::iterator i = m_buffs.m_mapBuffs.begin(); i != m_buffs.m_mapBuffs.end(); ++i )
	{
		ItemProp* pItemProp = prj.GetItemProp( i->second->GetId());

		if( pItemProp )
		{
			if( pItemProp->dwItemKind3 == IK3_EXP_RATE )
			{
				nAmount++;
			}
		}
	}
// Cant hold more Amps
	if( nAmount >= MAX_AMP_STACK_COUNT )
	{
		return 2;
	}
// Check Level
	if( GetLevel() < pItemProp->nMinLimitLevel || GetLevel() > pItemProp->nMaxLimitLevel )
	{
		return 1;
	}
// Can hold more Amps. Try activating scroll
	if( HasBuff( BUFF_ITEM, (WORD) ( pItemProp->dwID )))
	{
		MAPAMPSTORE::iterator i	= prj.m_mapAmpStore.find( pItemProp->dwID );
		if( i != prj.m_mapAmpStore.end() )
		{
			AMP_STORE_INFO* info = i->second;
			if( HasBuff( BUFF_ITEM, (WORD)(info->dwIDSecond)))
			{
				if( HasBuff( BUFF_ITEM, (WORD)(info->dwIDThird)))
				{
					if( HasBuff( BUFF_ITEM, (WORD) (info->dwIDFourth)))
					{
						if( HasBuff( BUFF_ITEM, (WORD) (info->dwIDFifth)))
						{
							return 2;
						}
						else
						{
							if( pItemProp->nMaxDuplication >= 5 )
							{
								ItemProp* pAmpProp = prj.GetItemProp( info->dwIDFifth );
								DoApplySkill( (CCtrl*)this, pAmpProp, NULL );
								return 4;
							}
						}
					}
					else
					{
						if( pItemProp->nMaxDuplication >= 4 )
						{
							ItemProp* pAmpProp = prj.GetItemProp( info->dwIDFourth );
							DoApplySkill( (CCtrl*)this, pAmpProp, NULL );
							return 4;
						}
					}
				}
				else
				{
					if( pItemProp->nMaxDuplication >= 3 )
					{
						ItemProp* pAmpProp = prj.GetItemProp( info->dwIDThird );
						DoApplySkill( (CCtrl*)this, pAmpProp, NULL );
						return 4;
					}
				}
			}
			else
			{
				if( pItemProp->nMaxDuplication >= 2 )
				{
					ItemProp* pAmpProp = prj.GetItemProp( info->dwIDSecond );
					DoApplySkill( (CCtrl*)this, pAmpProp, NULL );
					return 4;
				}
			}
		}
	}
	else
	{
//		DoApplySkill( (CCtrl*)this, pItemProp, NULL );
		Error( "%d", (DWORD)pItemProp->dwSkillTime );
		AddBuff( BUFF_ITEM, (WORD)(pItemProp->dwID), 0, pItemProp->dwSkillTime, GetId() );
		return 4;
	}
#endif // __NEW_STACKABLE_AMPS
#endif // __WORLDSERVER
	return 3;
}
#endif // __NEW_PROP_PARAMETER
#ifdef __WORLDSERVER
BOOL CMover::IsDoUseBuff( ItemProp* pItemProp )
{
	int nResult = 0;
	if( pItemProp->dwItemKind3 == IK3_POTION_BUFF_STR
		|| pItemProp->dwItemKind3 == IK3_POTION_BUFF_DEX
		|| pItemProp->dwItemKind3 == IK3_POTION_BUFF_INT
		|| pItemProp->dwItemKind3 == IK3_POTION_BUFF_STA
		|| pItemProp->dwItemKind3 == IK3_POTION_BUFF_DEFENSE		
#if __VER >= 8 //__CSC_VER8_5
		|| pItemProp->dwItemKind3 == IK3_ANGEL_BUFF		
#endif // __CSC_VER8_5
#ifdef __BUFF_TOGIFT
		|| pItemProp->dwItemKind2 == IK2_BUFF_TOGIFT
#endif // __BUFF_TOGIFT
		)
	{
		if( HasBuffByIk3( pItemProp->dwItemKind3 ) )
			nResult = 1;

#if __VER >= 8 //__CSC_VER8_5
#ifdef __PKSERVER_USE_ANGEL
		if( pItemProp->dwItemKind3 == IK3_ANGEL_BUFF )
		{
			if(g_eLocal.GetState( EVE_PK ))	
			{
				if( 0 >= GetPKValue() )
					nResult = 2;
				if( GetQuest( QUEST_PKMINUS ) == NULL )
					nResult = 3;
				if( m_Inventory.GetAtItemId( II_SYS_SYS_QUE_ANGEL_RED100 ) != NULL 
					|| m_Inventory.GetAtItemId( II_SYS_SYS_QUE_ANGEL_BLUE100 ) != NULL 
					|| m_Inventory.GetAtItemId( II_SYS_SYS_QUE_ANGEL_GREEN100 ) != NULL 
					|| m_Inventory.GetAtItemId( II_SYS_SYS_QUE_ANGEL_WHITE100 ) != NULL 
					)
					nResult = 4;
			}
			else
				nResult = 5;
		}
#else //__PKSERVER_USE_ANGEL
		if( pItemProp->dwItemKind3 == IK3_ANGEL_BUFF )
		{
			if( 0 >= GetPKValue() )
				nResult = 2;
			if( GetQuest( QUEST_PKMINUS ) == NULL )
				nResult = 3;
			if( m_Inventory.GetAtItemId( II_SYS_SYS_QUE_ANGEL_RED100 ) != NULL 
				|| m_Inventory.GetAtItemId( II_SYS_SYS_QUE_ANGEL_BLUE100 ) != NULL 
				|| m_Inventory.GetAtItemId( II_SYS_SYS_QUE_ANGEL_GREEN100 ) != NULL 
				|| m_Inventory.GetAtItemId( II_SYS_SYS_QUE_ANGEL_WHITE100 ) != NULL 
				)
				nResult = 4;
		}
#endif //__PKSERVER_USE_ANGEL
#endif // __CSC_VER8_5
	}
#if __VER >= 8
	else if( pItemProp->dwID == II_SYS_SYS_SCR_DEFENSE20 || pItemProp->dwID == II_SYS_SYS_SCR_DEFENSE50 )
	{
		if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_DEFENSE20 ) || HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_DEFENSE50 ) )
			nResult = 1;
	}
#endif // __VER >= 8
#ifdef __JEFF_11_1
	else if( pItemProp->dwID == II_SYS_SYS_SCR_PET_FEED_POCKET02 )
	{
		int nMax	= m_Inventory.GetMax();
		CItemElem* pItem;
		for( int i = 0 ; i < nMax; i++ )
		{
			pItem	= m_Inventory.GetAtId( i );
			if( IsUsableItem( pItem )
				&& pItem->m_dwItemId == II_SYS_SYS_SCR_PET_FEED_POCKET
				&& pItem->m_dwKeepTime > 0
				&& !pItem->IsFlag( CItemElem::expired )
				)
			{
				nResult		= 1;
				break;
			}
		}
		if( nResult != 1 )
		{
		#ifdef __BUFF_1107
			IBuff* pBuff	= m_buffs.GetBuff( BUFF_ITEM, (WORD)( pItemProp->dwID ) );
			if( pBuff )
			{
				DWORD dwCurr	= ::timeGetTime();
				if( static_cast<int>( pItemProp->dwSkillTime ) > static_cast<int>( pItemProp->dwSkillTime + ( pBuff->GetTotal() - ( dwCurr - pBuff->GetInst() ) ) ) )
					nResult		= 1;
			}
		#else	// __BUFF_1107
			LPSKILLINFLUENCE ptr	= m_SkillState.FindPtr( BUFF_ITEM, pItemProp->dwID );
			if( ptr )
			{
				DWORD dwCurr	= ::timeGetTime();
				if( static_cast<int>( pItemProp->dwSkillTime ) > static_cast<int>( pItemProp->dwSkillTime + ( ptr->tmCount - ( dwCurr - ptr->tmTime ) ) ) )
					nResult		= 1;
			}
		#endif	// __BUFF_1107
		}
	}
#endif	// __JEFF_11_1
	else if( HasBuff( BUFF_ITEM, (WORD)( pItemProp->dwID ) ) )
	{
		nResult = 1;
	}
#if __VER >= 11 // __SYS_COLLECTING
	else if( pItemProp->dwID == II_GEN_TOO_COL_SILVERBATTERY
		|| pItemProp->dwID == II_GEN_TOO_COL_GOLDBATTERY
		|| pItemProp->dwID == II_GEN_TOO_COL_BATTERY001
		)
	{
		if( HasBuff( BUFF_ITEM2, II_GEN_TOO_COL_SILVERBATTERY )
			|| HasBuff( BUFF_ITEM2, II_GEN_TOO_COL_GOLDBATTERY )
			|| HasBuff( BUFF_ITEM2, II_GEN_TOO_COL_BATTERY001 )
			)
			nResult = 1;
	}
#endif // __SYS_COLLECTING
#if __VER >= 12 // __JHMA_VER12_1	//12차 극단유료아이템
	#define II_SYS_SYS_SCR_PARTYEXPUP01_01 20296
#define II_SYS_SYS_SCR_PARTYSKILLUP01_01 20297
	else if( pItemProp->dwID == II_SYS_SYS_SCR_PARTYEXPUP01
		|| pItemProp->dwID == II_SYS_SYS_SCR_PARTYEXPUP02
		|| pItemProp->dwID == II_SYS_SYS_SCR_PARTYEXPUP01_01
		)
	{
		if( HasBuff( BUFF_ITEM2, II_SYS_SYS_SCR_PARTYEXPUP01 )
			|| HasBuff( BUFF_ITEM2, II_SYS_SYS_SCR_PARTYEXPUP02 )
			|| HasBuff( BUFF_ITEM2, II_SYS_SYS_SCR_PARTYEXPUP01_01 )
			)
			nResult = 1;
	}
	else if( pItemProp->dwID == II_SYS_SYS_SCR_SPETGOOD
		)
	{
		if( HasBuff( BUFF_ITEM2, II_SYS_SYS_SCR_SPETGOOD )
			)
			nResult = 1;
	}
	else if( pItemProp->dwID == II_SYS_SYS_SCR_PARTYSKILLUP01
		|| pItemProp->dwID == II_SYS_SYS_SCR_PARTYSKILLUP02
		|| pItemProp->dwID == II_SYS_SYS_SCR_PARTYSKILLUP01_01
		)
	{
		if( HasBuff( BUFF_ITEM2, II_SYS_SYS_SCR_PARTYSKILLUP01 )
			|| HasBuff( BUFF_ITEM2, II_SYS_SYS_SCR_PARTYSKILLUP02 )
			|| HasBuff( BUFF_ITEM2, II_SYS_SYS_SCR_PARTYSKILLUP01_01 )
			)
			nResult = 1;
	}
#endif // //12차 극단유료아이템
	
	switch( nResult )
	{
	case 1:
		((CUser*)this)->AddDefinedText( TID_GAME_LIMITED_USE, "" ); // 사용중이어서 사용할수 없음
		break;
#if __VER >= 8 //__CSC_VER8_5
	case 2:
		((CUser*)this)->AddDefinedText( TID_GAME_PKVALUE_NOT_ANGEL );
		break;
	case 3:
		((CUser*)this)->AddDefinedText( TID_GAME_NOT_QUEST_ANGEL );
		break;	
	case 4:
		((CUser*)this)->AddDefinedText( TID_GAME_NOT_COMPLETE_ANGEL );
		break;	
#endif // __CSC_VER8_5
	}
	return nResult;
}
#endif // __WORLDSERVER

// 소스보기 헷갈리니까 유료아이템만 따로 뺏다.
BOOL	CMover::DoUseItemFood_SM( ItemProp *pItemProp, CItemElem* pItemElem )
{
#ifdef __WORLDSERVER
	if( !( IsSMMode( SM_MAX_HP ) || IsSMMode( SM_MAX_HP50 ) ) )		// MAXHP 버프가 없을때만 사용됨.
	{
		DWORD nAdjParamVal;
		int nType;
		if( pItemProp->dwID == II_CHR_FOO_COO_BULLHAMS )
		{
			nAdjParamVal = pItemProp->nAdjParamVal2;
			nType = SM_MAX_HP;
		}
		else
		if( pItemProp->dwID == II_CHR_FOO_COO_GRILLEDEEL )
		{
			m_nPlusMaxHitPoint = nAdjParamVal = int( GetMaxOriginHitPoint() / ( 100 / pItemProp->nAdjParamVal1 ) );
			nType = SM_MAX_HP50;
		}
		else
			return FALSE;
			
		SetDestParam( pItemProp->dwDestParam1, nAdjParamVal, nAdjParamVal );
		SetSMMode( nType, pItemProp->dwCircleTime );
	}
	else
	{
		((CUser*)this)->AddDefinedText( TID_GAME_LIMITED_USE, "" );	// 사용중인 상용 아이템
		return FALSE;
	}
#endif
	return TRUE;
}

// IK2_FOOD / IK2_REFRESHER
BOOL	CMover::DoUseItemFood( ItemProp *pItemProp, CItemElem* pItemElem )
{
#ifdef __WORLDSERVER
	if( pItemProp->dwID == II_GEN_FOO_INS_SUPERHOTDOG )
	{
		SetPointParam( DST_HP, GetMaxHitPoint() );
		SetPointParam( DST_MP, GetMaxManaPoint() );
		SetPointParam( DST_FP, GetMaxFatiguePoint() );
		return TRUE;
	}

	int nPoint;
	int nMax, nHP, nHPMax;
#ifdef __PROP_0827
	for( int i = 0; i < 3; i ++ )
#else	// __PROP_0827
	for( int i = 0; i < 2; i ++ )
#endif	// __PROP_0827
	{
		if( pItemProp->dwDestParam[i] != (DWORD)-1 )
		{
			nHP	   = GetPointParam( pItemProp->dwDestParam[i] );
			nPoint = pItemProp->nAdjParamVal[i];
			nHPMax = GetMaxPoint( pItemProp->dwDestParam[i]);
			nMax   = pItemProp->dwAbilityMin;
			
			if( pItemProp->dwCircleTime != -1)	// 유료아이템만 dwCircleTime으로 다 썼더라....
			{
				BOOL bRet = DoUseItemFood_SM( pItemProp, pItemElem );
				if( bRet == FALSE )	return FALSE;
			}
			else
			{
				if( pItemProp->dwDestParam[i] == DST_HP || pItemProp->dwDestParam[i] == DST_MP || pItemProp->dwDestParam[i] == DST_FP )
				{
					if( nPoint != -1 )
					{
						if( nHP >= nMax )
						{
							if( nHP + (nPoint * 0.3f) >= nHPMax )
								nHP = nHPMax;
							else
							{
								if( TRUE == IsPlayer() )
								{
									DWORD dwParam = 0;
									switch( pItemProp->dwDestParam[i] )
									{
									case DST_HP:	dwParam = TID_GAME_LIMITHP;		break;
									case DST_MP:	dwParam = TID_GAME_LIMITMP;		break;
									case DST_FP:	dwParam = TID_GAME_LIMITFP;		break;
									}
									
									((CUser*)this)->AddDefinedText( dwParam, "" );
								}
								nPoint = (int)( nPoint * 0.3f );
								nHP = nHP + nPoint;
							}
						}
						else
						{
							if( nHP + nPoint >= nHPMax )	// 포인트 더했을때 오바되는걸 처리.
								nHP = nHPMax;
							else
								nHP = nHP + nPoint;
						}
					}
					
					SetPointParam( pItemProp->dwDestParam[i], nHP );
				} // DST_HP, MP, FP
				else
				{
					DoApplySkill( this, pItemProp, NULL );
				}
			}
		}
	} // for
#endif // worldserver
	
	return TRUE;
}


#ifdef __WORLDSERVER
BOOL CMover::DoUseItemSexChange( int nFace )
{
	// 1.
	for( DWORD dwParts = 0; dwParts < MAX_HUMAN_PARTS; dwParts++ )
	{
		if( dwParts == PARTS_HEAD || dwParts == PARTS_HAIR || dwParts == PARTS_LWEAPON
			|| dwParts == PARTS_RWEAPON || dwParts == PARTS_SHIELD || dwParts == PARTS_RIDE
			|| ( dwParts >= PARTS_NECKLACE1 && dwParts <= PARTS_BULLET ) )
			continue;
		
		CItemElem* pArmor	= m_Inventory.GetEquip( dwParts );
		if( pArmor )
		{
			((CUser*)this)->AddDefinedText( TID_GAME_CHECK_EQUIP, "" );
				return FALSE;
		}
	}

	// 2.
	DWORD dwIndex;
	if( GetSex() == SEX_MALE )
	{
		dwIndex		= MI_FEMALE;
		SetSex( SEX_FEMALE );
	}
	else
	{
		dwIndex	= MI_MALE;
		SetSex( SEX_MALE );
	}

	SetTypeIndex( D3DDEVICE, OT_MOVER, dwIndex );
	ResetScale();
	SetMotion( MTI_WALK );
	//RedoEquip( FALSE );		// chipi_091125 제거
	RedoEquip( FALSE, FALSE );	// chipi_091125 추가 - 악세사리, 무기 등의 아이템은 착용 상태로 트랜지가 가능한데 능력치를 다시(중복) 적용시키는 문제가 있었다.
	UpdateLocalMatrix();

	// 3.
	g_UserMng.AddSexChange( this );

	// 4.
	m_dwHeadMesh	= (DWORD)nFace;
	g_UserMng.AddChangeFace( m_idPlayer, (DWORD)nFace );

#if __VER >= 11 // __SYS_PLAYER_DATA
	g_dpDBClient.SendUpdatePlayerData( static_cast<CUser*>(this) );
#endif // __SYS_PLAYER_DATA

	return TRUE;
}
#endif	// __WORLDSERVER

/*--------------------------------------------------------------------------------------------------------


											Do Wand Attack


----------------------------------------------------------------------------------------------------------*/
//
// 완드어택을 하라!
//
int	 CMover::DoAttackMagic( CObj *pTargetObj, int nMagicPower, int idSfxHit )
{
	if( IsInvalidObj(pTargetObj) )		// 타겟이 거시기 하면 리턴
		return( -1 );

	ItemProp* pItemProp;
#ifndef __CLIENT
	CItemElem* pItemElem = GetWeaponItem();	// 손에든 무기가 없으면 리턴
	if( pItemElem == NULL )
		return( - 1 ) ;
	pItemProp	= pItemElem->GetProp();
#else	// __CLIENT
	if( IsActiveMover() ) {
		CItemElem* pItemElem = GetWeaponItem();
		if( pItemElem == NULL )
			return( - 1 ) ;
		pItemProp	= pItemElem->GetProp();
	}
	else {
		DWORD dwItemId	= m_aEquipInfo[PARTS_RWEAPON].dwId;
		if( dwItemId == NULL_ID )
			return( -1 );
		pItemProp	= prj.GetItemProp( dwItemId );
	}
#endif	// __CLIENT

	if( pItemProp->dwItemKind3 != IK3_WAND )		// 들고있는 무기가 완드가 아니면 에러.
		return( -1 );

#ifdef __CLIENT
	// 현재 지점과 목표지점까지 막히는게 있으면 메시지 출력하고 리턴
	CWorld *pWorld = GetWorld();
	D3DXVECTOR3 vStart = GetPos();				vStart.y += 0.5f;
	D3DXVECTOR3 vEnd   = pTargetObj->GetPos();	vEnd.y += 0.5f;

	if( pWorld->IntersectObjLine( NULL, vStart, vEnd, FALSE, FALSE ) )
	{
		g_WndMng.PutString( prj.GetText( TID_GAME_BLOCKTARGETING ), NULL, prj.GetTextColor( TID_GAME_BLOCKTARGETING ) );
		return -1;
	}
#endif  // CLIENT

	OBJID	idTarget = ((CCtrl*)pTargetObj)->GetId();

	int nError;
	if( ( nError = m_pActMover->SendActMsg( OBJMSG_ATK_MAGIC1, idTarget ) ) == 1 )	// 완드공격 동작 시작.
	{
		if( m_pActMover->IsFly() == FALSE )		
			SetAngle( GetDegree(pTargetObj->GetPos(), GetPos()) );		// 목표쪽으로 몸을 돌림.
		SetActParam( OBJACT_MAGIC_ATTACK, idTarget, nMagicPower );		// 실행중 사용할 파라메터 등록
		
#ifdef __CLIENT	
		if( nMagicPower >= 1 )	// 레벨 2이상을 쓰면 액션포인트 하나 올라감.
		{
			if( g_WndMng.m_pWndTaskBar->m_nActionPoint < 100 )
				g_WndMng.m_pWndTaskBar->m_nActionPoint ++;
		}
#endif
		
#ifdef __WORLDSERVER
		if( IsPlayer() )
		{
			if( nMagicPower >= 1 && ((CUser*)this)->m_playTaskBar.m_nActionPoint < 100 )	// 2단계충전이상 사용하면 액션포인트 올라감.
				((CUser*)this)->m_playTaskBar.m_nActionPoint ++;
		}
		if( nMagicPower == 3 )		// 4단계짜리를 쓰면 마나 5포인트 감소
			IncManaPoint( -GetReqMp( pItemProp->dwReqMp ) );	
		g_UserMng.AddMagicAttack( this, OBJMSG_ATK_MAGIC1, idTarget, 0, 0, nMagicPower, idSfxHit );
#else	// __WORLDSERVER
#ifdef __CLIENT
		if( IsActiveMover() ) {
			SetActParam( OBJACT_MAGIC_ATTACK, idTarget, nMagicPower );	// 효과 식별자 등록
			g_DPlay.SendMagicAttack( OBJMSG_ATK_MAGIC1, ( (CCtrl*)pTargetObj )->GetId(), 0, 0, nMagicPower, m_idSfxHit );
		}
#endif	// __CLIENT
#endif	// __WORLDSERVER
	}
	return( nError );
}

/*--------------------------------------------------------------------------------------------------------


											Do Range Attack


----------------------------------------------------------------------------------------------------------*/
//
// 레인지어택을 하라!
// dwItemID : 무기아이템 아이디
//
int	 CMover::DoAttackRange( CObj *pTargetObj, DWORD dwItemID, int idSfxHit )
{
	ItemProp* pItemProp = NULL;
	int nPower = 0;

	if( IsInvalidObj(pTargetObj) )
		return -1;
	

#ifdef __CLIENT
	if( IsActiveMover() )
	{
		// 현재 지점과 목표지점까지 막히는게 있으면 메시지 출력하고 리턴
		CWorld *pWorld = GetWorld();
		D3DXVECTOR3 vStart = GetPos();				vStart.y += 0.5f;
		D3DXVECTOR3 vEnd   = pTargetObj->GetPos();	vEnd.y += 0.5f;

		if( pWorld->IntersectObjLine( NULL, vStart, vEnd, FALSE, FALSE ) )
		{
			g_WndMng.PutString( prj.GetText( TID_GAME_BLOCKTARGETING ), NULL, prj.GetTextColor( TID_GAME_BLOCKTARGETING ) );
			
			return -1;
		}
	}
#endif  // CLIENT
	

	if( IsPlayer() )
	{
		pItemProp = GetActiveHandItemProp();			
		nPower = dwItemID;

	#ifdef __WORLDSERVER
		CItemElem* pItemElem = m_Inventory.GetEquip( PARTS_BULLET );	// 화살이 있는지 체크
		if( pItemElem == NULL || pItemElem->GetProp()->dwItemKind3 != IK3_ARROW )
			return -1;
	#endif // __WORLDSERVER
	}
	else
	{
		if( dwItemID == NULL_ID || dwItemID == 0 )	
			pItemProp = GetActiveHandItemProp();		// 이런경우는 걍 손에 든 무기(혹은 기본아이템)
		else
			pItemProp = prj.GetItemProp( dwItemID );
	}
	
	if( pItemProp == NULL )
	{
		Error( "DoAttackRange : %s ActiveHandItemProp = NULL", m_szName );
		return -1;
	}

	if( pItemProp->dwWeaponType != WT_RANGE && pItemProp->dwWeaponType != WT_RANGE_BOW )		// 들고있는 무기가 RANGE가 아니면 리턴
	{
		return -1;
	}
	
	DWORD dwUseMotion = pItemProp->dwUseMotion;	
	if( dwUseMotion == NULL_ID )
	{
		if( IsPlayer() )
		{
			dwUseMotion = MTI_ATK_13 - MTI_STAND_13;//23;
		}
		else
		{
			Error( "DoAttackRange : %s [%d]'s dwUseMotion = NULL_ID", m_szName, pItemProp->dwID );
			dwUseMotion = MTI_ATK1;		// 프로퍼티에 장거리 공격 모션이 지정되지 않았으면 기본 모션으로 한다.
		}
	}

	OBJID	idTarget = ((CCtrl*)pTargetObj)->GetId();

	int nError;
	if( ( nError = m_pActMover->SendActMsg( OBJMSG_ATK_RANGE1, idTarget, (int)dwUseMotion ) ) == 1 )	// 장거리공격.
	{
		SetAngle( GetDegree(pTargetObj->GetPos(), GetPos()) );			// 목표쪽으로 몸을 돌림.
		SetActParam( OBJACT_RANGE_ATTACK, idTarget, dwItemID, idSfxHit );	// 효과 식별자 등록
		
	#ifdef __WORLDSERVER
		g_UserMng.AddRangeAttack( this, OBJMSG_ATK_RANGE1, idTarget, dwItemID, 0, idSfxHit );
		ArrowDown( 1 );		// 화살 1개 없앰
	#else	// __WORLDSERVER
		if( IsActiveMover() )	// 클라에서 플레이어가 사용한거라면 서버로 정보 보냄.
			g_DPlay.SendRangeAttack( OBJMSG_ATK_RANGE1, ( (CCtrl*)pTargetObj )->GetId(), dwItemID, m_idSfxHit ); 
	#endif	// __WORLDSERVER
	}
	return nError;
}

// 몬스터 특수공격
int		CMover::DoAttackSP( CObj *pTargetObj, DWORD dwItemID )
{
	if( IsInvalidObj(pTargetObj) )
		return( -1 );
	
	ItemProp* pItemProp;
	if( dwItemID == NULL_ID || dwItemID == 0 )	
		pItemProp = GetActiveHandItemProp();	// 이런경우는 걍 손에 든 무기(혹은 기본아이템)
	else
		pItemProp = prj.GetItemProp( dwItemID );	// 직접 아이템을 지정한 경우는 그걸로 공격함(몬스터가 주로 사용함)
	if( pItemProp == NULL )
	{
		Error( "DoAttackSP : %s 의 ItemProp이 NULL %d", m_szName, (int)dwItemID );
		return FALSE;
	}

	DWORD dwUseMotion = pItemProp->dwUseMotion;
	if( dwUseMotion == NULL_ID )
	{
		Error( "DoAttackSP : %s의 공격%d의 dwUseMotion이 지정되지 않았다", m_szName, pItemProp->dwID );
		dwUseMotion = MTI_ATK1;		// 프로퍼티에 장거리 공격 모션이 지정되지 않았으면 기본 모션으로 한다.
	}
	
	OBJID	idTarget = ((CCtrl*)pTargetObj)->GetId();
	
	int nError;
	if( ( nError = m_pActMover->SendActMsg( OBJMSG_SP_ATK1, idTarget, (int)dwItemID, (int)dwUseMotion ) ) == 1 )	// 장거리공격.
	{
		SetAngle( GetDegree(pTargetObj->GetPos(), GetPos()) );		// 목표쪽으로 몸을 돌림.
		SetActParam( OBJACT_SP_ATTACK, idTarget, (int)dwItemID );	// 효과 식별자 등록
		
#ifdef __WORLDSERVER
		g_UserMng.AddAttackSP( this, OBJMSG_SP_ATK1, idTarget, dwItemID, 0 );
#else	// __WORLDSERVER
	
#if 0 //2009_12_18 기획에서 제거 요청 : 발동타임에 타겟에 대해 이펙트 필요없음 
		DWORD dwSfxObj = pItemProp->dwSfxObj3;		// 특수공격에 발동이펙트가 있다면 3번사용.
		if( dwSfxObj != NULL_ID )
			CreateSfx( D3DDEVICE, dwSfxObj, GetPos(), GetId(), D3DXVECTOR3(0,0,0), idTarget );
#endif 

	//gmpbigsun: 발동시 공격자에 대한 발동 effect
	DWORD dwSfxObj = pItemProp->dwSfxObj;
	if( NULL_ID != dwSfxObj )
		CreateSfx( D3DDEVICE, dwSfxObj, GetPos() );

		
#endif	// __WORLDSERVER
	}

	return( nError );
}

int		CMover::DoAttackMelee( CMover *pTarget, OBJMSG dwMsg, DWORD dwItemID )
{
	SendActMsg( OBJMSG_STOP );		// 제자리 멈춤.

	if( IsInvalidObj(pTarget) )
		return 0;
	
	ItemProp* pItemProp;
	if( dwItemID == NULL_ID || dwItemID == 0 )	
		pItemProp = GetActiveHandItemProp();	// 이런경우는 걍 손에 든 무기(혹은 기본아이템)
	else
		pItemProp = prj.GetItemProp( dwItemID );	// 직접 아이템을 지정한 경우는 그걸로 공격함(몬스터가 주로 사용함)
	if( pItemProp == NULL )
	{
		Error( "DoAttackMelee : %s 의 ItemProp이 NULL %d", GetName(), (int)dwItemID );
		return FALSE;
	}

	// 모션 시작하고.
	int nError	= SendActMsg( dwMsg, pTarget->GetId(), MAKELONG( 0, 0 ), MAKELONG( 0, 0 ) );
	if( nError > 0 )
	{
		SetActParam( OBJACT_MELEE_ATTACK, pTarget->GetId(), (int)dwItemID );	// OnAttackMelee에서 쓸수 있도록 파라메터 등록.

#ifdef __CLIENT
#ifdef __HACK_1023
		ItemProp* pHandItemProp		= GetActiveHandItemProp();
		FLOAT fVal	= pHandItemProp? pHandItemProp->fAttackSpeed: 0.0F;
		g_DPlay.SendMeleeAttack( dwMsg, pTarget->GetId(), MAKELONG( 0, 0 ), MAKELONG( 0, (WORD)nError ), fVal );
#else	// __HACK_1023
		g_DPlay.SendMeleeAttack( dwMsg, pTarget->GetId(), MAKELONG( 0, 0 ), MAKELONG( 0, (WORD)nError ) );
#endif	// __HACK_1023
		if( IsActiveMover() )
		{
			PlayCombatMusic();
#if __VER < 11 // __REMOVE_ENDURANCE
			CItemElem* pWeapon	= GetWeaponItem();
			if( pWeapon )
			{
				ItemProp* pItemProp		= pWeapon->GetProp();
				if( pItemProp )
					if( ( ( pWeapon->m_nHitPoint * 100 ) / pItemProp->dwEndurance ) == 0 )
						g_WndMng.PutString( prj.GetText( TID_GAME_WASTEENDURANCE ), NULL, prj.GetTextColor( TID_GAME_WASTEENDURANCE ) );
			}
#endif // __REMOVE_ENDURANCE
		}
#else	// __CLIENT
		g_UserMng.AddMeleeAttack( this, dwMsg, pTarget->GetId(), MAKELONG( 0, 0 ), MAKELONG( 0, (WORD)nError ) );
#endif	// __CLIENT
	}

	return TRUE;	
}	



// 주변에 있는 아이템중 하나를 집는다.
void	CMover::DoPickupItemAround()
{
#ifdef __CLIENT
	if( m_pActMover->IsActAttack() )	return;		// 공격중 사용안됨.
	if( IsDie() )		return;						// 죽었을땐 사용안됨.
	
	int nRange	= 4;	// 4m
	float fDistMin = 99999.0f, fDistSq;
	CObj* pObj, *pMinObj = NULL;
	D3DXVECTOR3 vPos = GetPos();
	D3DXVECTOR3 vDist;
	FOR_LINKMAP( GetWorld(), vPos, pObj, nRange, CObj::linkDynamic, GetLayer() )
	{
		if( pObj->GetType() == OT_ITEM )
		{
			vDist = pObj->GetPos() - vPos;		// this -> 아이템까지의 벡터
			fDistSq = D3DXVec3LengthSq( &vDist );
			if( fDistSq < 4.0f * 4.0f )		// 아이템과의 거리가 x미터 이내인것을 대상으로.
			{
				if( fDistSq < fDistMin )	// 그중 젤 가까운놈으로 선택
				{
					fDistMin = fDistSq;
					pMinObj = pObj;
				}
			}
		}
	}
	END_LINKMAP

	// 아이템 집을게 있다.
	if( pMinObj )
	{
		CMD_SetUseItem( (CCtrl*)pMinObj );
	}
#endif // __CLIENT
}


// this를 타겟으로 주변에 데미지를 준다.
// ------------------------------------------
// nDmgType : 데미지 타입 ex) AF_MAGICSKILL
// pAttacker : 공격자(시전자) - 앞으로 CCtrl*로 바꿔야 한다
// nApplyType : 적용대상. ex) OBJTYPE_PLAYER | OBJTYPE_MONSTER
// nSkill : 사용한 스킬종류
// fRange : 타겟을 중심으로한 데미지 범위(미터)
// fTargetRatio : 타겟에게 적용되는 데미지 비율
// fAroundRatio : 주변에 적용되는 데미지 비율
// (AF_MAGICSKILL, this, nSkill, 5.0f, 0.0, 1.0f )
void	CMover::SendDamageAround( int nDmgType, CMover *pAttacker, int nApplyType, int nAttackID, float fRange, float fTargetRatio, float fAroundRatio )
{
#ifdef __WORLDSERVER
	int nRange	= 4;	// 4m
	float fDistSq;
	CObj* pObj;
	CMover *pTarget;
	D3DXVECTOR3 vPos = GetPos();
	D3DXVECTOR3 vDist;

	if( fRange <= 4.0f )
		nRange = 4;
	else if( fRange <= 8.0f )
		nRange = 8;
	else if( fRange <= 16.0f )
		nRange = 16;
	else
		nRange = 32;
	
	if( fRange <= 0 )	// 범위가 0이거나 음수일수는 없다.
		Error( "CMover::SendDamageAround : D:%s A:%s %d %f", GetName(), pAttacker->GetName(), nAttackID, fRange );

	ItemProp* pProp;
	if( nDmgType == AF_MAGICSKILL )
	{
		pProp = prj.GetSkillProp( nAttackID );		// UseSkill에서 사용한 스킬의 프로퍼티 꺼냄
		if( pProp == NULL )
		{
			Error( "CMover::SendDamageAround : %s. 스킬(%d)의 프로퍼티가 업다.", m_szName, nAttackID );
			return;	// property not found
		}
	} else
	{
		pProp = prj.GetItemProp( nAttackID );		// 아이템 프로퍼티꺼냄
		if( pProp == NULL )
		{
			Error( "CMover::SendDamageAround : %s. 아이템(%d)의 프로퍼티가 업다.", m_szName, nAttackID );
			return;	// property not found
		}
	}
	
	BOOL	bDamage = FALSE;
	BOOL	bTarget = FALSE;

	if( nApplyType & OBJTYPE_PLAYER )	// 적용대상이 플레이어인가 
	{
		FOR_LINKMAP( GetWorld(), vPos, pObj, nRange, CObj::linkPlayer, GetLayer() )
		{
			if( pObj->GetType() == OT_MOVER )
			{
				bDamage = TRUE;

				if( bDamage )
				{
					vDist = pObj->GetPos() - vPos;		// this -> 타겟까지의 벡터
					fDistSq = D3DXVec3LengthSq( &vDist );
					if( fDistSq < fRange * fRange )		// 타겟과의 거리가 fRange미터 이내인것을 대상으로.
					{
						if( pObj == this && fTargetRatio == 0.0f )	// 타겟데미지 비율이 0이면 자기자신(타겟)은 데미지를 주지않음.
						{
						} else
						{
							if( pObj != pAttacker )		// 어태커는 검색대상에서 제외.
							{
								pTarget = (CMover *)pObj;
								bTarget = ( pTarget == this );
								if( IsValidObj( (CObj*)pTarget ) && pTarget->IsLive() )
								{
									if( pProp->dwComboStyle == CT_FINISH )
										pTarget->m_pActMover->SendDamageForce( nDmgType, pAttacker->GetId(), (nAttackID << 16), bTarget );
									else
										pTarget->m_pActMover->SendDamage( nDmgType, pAttacker->GetId(), (nAttackID << 16), bTarget );
								}
							}
						}
					}
					
					bDamage = FALSE;	// 다음 루프를 위해서 초기화.
				} // bDamage
			}
		}
		END_LINKMAP
	}

	// 적용대상이 몬스터인가.
	if( nApplyType & OBJTYPE_MONSTER )
	{
		FOR_LINKMAP( GetWorld(), vPos, pObj, nRange, CObj::linkDynamic, GetLayer() )
		{
			if( pObj->GetType() == OT_MOVER && ((CMover *)pObj)->IsPeaceful() == FALSE )
			{
				vDist = pObj->GetPos() - vPos;		// this -> 타겟까지의 벡터
				fDistSq = D3DXVec3LengthSq( &vDist );
				if( fDistSq < fRange * fRange )		// 타겟과의 거리가 fRange미터 이내인것을 대상으로.
				{
					if( pObj == this && fTargetRatio == 0.0f )	// 타겟데미지 비율이 0이면 자기자신(타겟)은 데미지를 주지않음.
					{
					} else
					{
						if( pObj != pAttacker )		// 공격자는 검사대상에서 제외.
						{
							pTarget = (CMover *)pObj;
							if( IsValidObj( (CObj*)pTarget ) && pTarget->IsLive() )
							{
								if( pProp->dwComboStyle == CT_FINISH )
									pTarget->m_pActMover->SendDamageForce( nDmgType, pAttacker->GetId(), (nAttackID << 16) );
								else
									pTarget->m_pActMover->SendDamage( nDmgType, pAttacker->GetId(), (nAttackID << 16) );
							}
						}
					}
				}
			}
		}
		END_LINKMAP
	}
		
#endif // WORLDSERVER		
} // SendDamageAround()
/*
BOOL	CMover::DoCollect( CMover *pTarget )
{
	SendActMsg( OBJMSG_STOP );
	BOOL bSuccess = SendActMsg( OBJMSG_COLLECT );
	ClearDest();
		
	return bSuccess;
}
*/


// 
// 폭, 높이, 길이를 설정하여 육면체 형태의 데미지 영역을 생성.
// nDmgType : AF_???
// nApplyType : OBJTYPE_PLAYER , MONSTER
// nAttackID : 스킬이나 아이템 ID
// fWidth, fHeight, fDepth : 폭, 높이, 길이.
// this == Attacker
void	CMover::SendDamageLine( int nDmgType, int nApplyType, int nAttackID, 
								float fWidth, float fHeight, float fDepth )
{
#ifdef __WORLDSERVER
	int nRange	= 4;	// 4m

	CObj* pObj;
	CMover *pTarget;
	D3DXVECTOR3 vPos = GetPos();
	D3DXVECTOR3 vDist;

	// 데미지 영역 Local AABB.
	D3DXVECTOR3 vMin1 = D3DXVECTOR3( -fWidth / 2.0f, -fHeight / 2.0f, -fDepth );
	D3DXVECTOR3 vMax1 = D3DXVECTOR3(  fWidth / 2.0f,  fHeight / 2.0f, 0 );

	D3DXMATRIX mInv;		// 공격자측 월드역행렬
	D3DXMATRIX m1, m2, m3;
	D3DXMatrixRotationY( &m1, D3DXToRadian( -GetAngle() ) );
	D3DXMatrixTranslation( &m2, GetPos().x, GetPos().y, GetPos().z );
	D3DXMatrixMultiply( &m3, &m1, &m2 );	// world = rot * trans
	
	D3DXMatrixInverse( &mInv, NULL, &m3 );		// 공격자의 역행렬 만듬.

	// 타겟의 좌표를 공격자측 기준으로 역변환 한것.
	D3DXVECTOR3 vDestLocal;

	// 일반적으로 fDepth가 가장 길기때문에 검사 영역은 fDepth로 했다. 
	if( fDepth <= 4.0f )
		nRange = 4;
	else if( fDepth <= 8.0f )
		nRange = 8;
	else if( fDepth <= 16.0f )
		nRange = 16;
	else
		nRange = 32;
	
	if( fDepth <= 0 )	// 범위가 0이거나 음수일수는 없다.
		Error( "CMover::SendDamageLine : D:%s A:%s %d %f", GetName(), nAttackID, fDepth );

	ItemProp* pProp;
	if( nDmgType == AF_MAGICSKILL )
	{
		pProp = prj.GetSkillProp( nAttackID );		// UseSkill에서 사용한 스킬의 프로퍼티 꺼냄
		if( pProp == NULL )
		{
			Error( "CMover::SendDamageLine : %s. 스킬(%d)의 프로퍼티가 업ㅂ다.", m_szName, nAttackID );
			return;	// property not found
		}
	} else
	{
		pProp = prj.GetItemProp( nAttackID );		// 아이템 프로퍼티꺼냄
		if( pProp == NULL )
		{
			Error( "CMover::SendDamageLine : %s. 아이템(%d)의 프로퍼티가 업ㅂ다.", m_szName, nAttackID );
			return;	// property not found
		}
	}
	
	BOOL	bDamage = FALSE;

	if( nApplyType & OBJTYPE_PLAYER )	// 적용대상이 플레이어인가 
	{
		FOR_LINKMAP( GetWorld(), vPos, pObj, nRange, CObj::linkPlayer, GetLayer() )
		{
			if( pObj->GetType() == OT_MOVER )
			{
				bDamage = TRUE;	

				if( bDamage && pObj != this )		// this는 공격대상에서 제외.
				{
					pTarget = (CMover *)pObj;
					if( IsValidObj( pTarget ) && pTarget->IsLive() )
					{
						// 타겟측 좌표를 역변환.
						D3DXVec3TransformCoord( &vDestLocal, &pTarget->GetPos(), &mInv );
						// 타겟측 AABB
						D3DXVECTOR3 vMin2 = vDestLocal + pTarget->m_pModel->m_vMin;
						D3DXVECTOR3 vMax2 = vDestLocal + pTarget->m_pModel->m_vMax;
						if( ::IsTouchAABB( vMin1, vMax1, vMin2, vMax2 ) )		// AABB충돌검사.
						{
						//	if( pProp->dwComboStyle == CT_FINISH )
						//		pTarget->m_pActMover->SendDamageForce( nDmgType, pAttacker->GetId(), (nAttackID << 16) );
						//	else
						//		pTarget->m_pActMover->SendDamage( nDmgType, pAttacker->GetId(), (nAttackID << 16) ); 					
						}
					}
				} // bDamage
			}
		}
		END_LINKMAP
	}

	// 적용대상이 몬스터인가.
	if( nApplyType & OBJTYPE_MONSTER )
	{
		FOR_LINKMAP( GetWorld(), vPos, pObj, nRange, CObj::linkDynamic, GetLayer() )
		{
			if( pObj->GetType() == OT_MOVER && ((CMover *)pObj)->IsPeaceful() == FALSE )
			{
				pTarget = (CMover *)pObj;
				if( IsValidObj( pTarget ) && pTarget->IsLive() )
				{
					// 타겟측 좌표를 역변환.
					D3DXVec3TransformCoord( &vDestLocal, &pTarget->GetPos(), &mInv );
					// 타겟측 AABB
					D3DXVECTOR3 vMin2 = vDestLocal + pTarget->m_pModel->m_vMin;
					D3DXVECTOR3 vMax2 = vDestLocal + pTarget->m_pModel->m_vMax;
					if( ::IsTouchAABB( vMin1, vMax1, vMin2, vMax2 ) )		// AABB충돌검사.
					{
						if( pProp->dwComboStyle == CT_FINISH )
							pTarget->m_pActMover->SendDamageForce( nDmgType, GetId(), (nAttackID << 16) );
						else
							pTarget->m_pActMover->SendDamage( nDmgType, GetId(), (nAttackID << 16) ); 
					}
				}
			}
		}
		END_LINKMAP
	}
		
#endif // WORLDSERVER		
} // SendDamageLine()


#ifdef __WORLDSERVER
BOOL CMover::DoUseItemWarp( ItemProp* pItemProp, CItemElem* pItemElem )
{
	switch( pItemProp->dwID )
	{
		case II_GEN_WARP_COUPLERING:
		case II_GEN_WARP_WEDDING_BAND:
#if __VER >= 8 // __S_COUPLERING01
		case II_GEN_WARP_COUPLERING01:
#endif // __VER >= 8
			{
				if( IsFly() )
				{
					return FALSE;
				}
//				if( pItemElem->IsFlag( CItemElem::expired ) )
//				{
//					( (CUser*)this )->AddDefinedText( TID_GAME_ITEM_EXPIRED );
//					return FALSE;
//				}
				u_long idCouple	= (u_long)( pItemElem->GetRandomOptItemId() );
				if( idCouple == m_idPlayer )
				{
					( (CUser*)this )->AddDefinedText( TID_GAME_COUPLERING_ERR01, "%s", GetName() );
					return FALSE;
				}

				if( idCouple > 0 )
				{
					// teleport
#if __VER >= 11 // __SYS_PLAYER_DATA
					const char* lpszPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( idCouple );
#else	// __SYS_PLAYER_DATA
					LPCSTR lpszPlayer	= prj.GetPlayerString( idCouple );
#endif	// __SYS_PLAYER_DATA
					if( lpszPlayer )
					{
						CUser* pUser	= (CUser*)prj.GetUserByID( idCouple );
						if( IsValidObj( (CObj*)pUser ) )
						{
							if( pUser->IsFly() )
							{
								( (CUser*)this )->AddDefinedText( TID_GAME_COUPLERING_ERR03 );
								return FALSE;
							}
							if( pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_GUILDWAR )
								return FALSE;
							if( GetWorld() && GetWorld()->GetID() == WI_WORLD_GUILDWAR )
								return FALSE;
#if __VER >= 11	// __GUILD_COMBAT_1TO1
							if( GetWorld() && GetWorld()->GetID() >= WI_WORLD_GUILDWAR1TO1_0
								          && GetWorld()->GetID() <= WI_WORLD_GUILDWAR1TO1_L )
								return FALSE;
							if( pUser->GetWorld() && pUser->GetWorld()->GetID() >= WI_WORLD_GUILDWAR1TO1_0
												&& pUser->GetWorld()->GetID() <= WI_WORLD_GUILDWAR1TO1_L )
								return FALSE;
#endif // __GUILD_COMBAT_1TO1
#if __VER >= 13 // __RAINBOW_RACE
							if( CRainbowRaceMng::GetInstance()->IsEntry( ((CUser*)this)->m_idPlayer )
								|| CRainbowRaceMng::GetInstance()->IsEntry( idCouple ) )
							{
								((CUser*)this)->AddDefinedText( TID_GAME_RAINBOWRACE_NOTELEPORT );
								return FALSE;
							}
#endif // __RAINBOW_RACE

							if( GetWorld() == pUser->GetWorld() && !prj.IsGuildQuestRegion( pUser->GetPos() )
#if __VER >= 14 // __INSTANCE_DUNGEON
								&& GetLayer() == pUser->GetLayer()
#endif // __INSTANCE_DUNGEON
#ifdef __QUIZ
								&& GetWorld()->GetID() != WI_WORLD_QUIZ
								&& pUser->GetWorld()->GetID() != WI_WORLD_QUIZ
#endif // __QUIZ
								)
							{
								 g_DPCoreClient.SendTeleportPlayer( m_idPlayer, idCouple );
							}
							else
							{
								( (CUser*)this )->AddDefinedText( TID_GAME_COUPLERING_ERR02, "%s", lpszPlayer );
							}
						}
						else
							( (CUser*)this )->AddDefinedText( TID_DIAG_0061, "%s", lpszPlayer );
					}
				}
				else
				{
					// carve
#if __VER >= 11 // __SYS_IDENTIFY
					UpdateItemEx( (BYTE)( pItemElem->m_dwObjId ), UI_RANDOMOPTITEMID, (__int64)m_idPlayer );
#else	// __SYS_IDENTIFY
					UpdateItem( (BYTE)( pItemElem->m_dwObjId ), UI_RANDOMOPTITEMID, m_idPlayer );
#endif	// __SYS_IDENTIFY
					if( II_GEN_WARP_COUPLERING == pItemProp->dwID )
						UpdateItem( (BYTE)( pItemElem->m_dwObjId ), UI_KEEPTIME, 21600 );	// 15 days
					( (CUser*)this )->AddDefinedText( TID_GAME_COUPLERING_CARVE, "%s", GetName() );
				}
				return TRUE;
			}
		default:
			break;
	}
	return FALSE;
}
#endif	// __WORLDSERVER

// nIndex skill의 재사용 딜레이를 리턴한다. 
// nIndex - skill의 index, ( m_tmReUseDelay, m_aJobSkill의 인덱스는 같은 의미 )
DWORD CMover::GetReuseDelay( int nIndex )
{
	ASSERT( nIndex >= 0 );	

	DWORD dwCur = ::timeGetTime();
	if( dwCur > m_tmReUseDelay[nIndex] )	// 종료시각 보다 현재 시각이 크면 
		return 0;								// 딜레이 없다.
	else									// 아니면 
		return m_tmReUseDelay[nIndex] - dwCur;	// 종료시각 - 현재시각 
}

#if __VER >= 9	// __PET_0410
#ifndef __CLIENT
int CMover::DoUseItemFeedPocket( CItemElem* pPocket )
{
	int nResult = 0;

#ifdef __JEFF_11_1
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_PET_FEED_POCKET02 ) )
	{
		( (CUser*)this )->AddDefinedText( TID_GAME_DONOTUSE_PETPOCKET );
		return	4;
	}
#endif	// __JEFF_11_1
	// 활성화 검사
	if( HasBuff( BUFF_ITEM, (WORD)( pPocket->m_dwItemId ) ) )
	{
		if( pPocket->m_dwKeepTime != 0 )
		{
			RemoveBuff( BUFF_ITEM, (WORD)( pPocket->m_dwItemId ) );
			nResult = 4;
		}
		else
			nResult = 2;
	}
	else
	{
		ItemProp* pProp	= pPocket->GetProp();
		if( pPocket->m_dwKeepTime == 0 )	// 만료 시간이 설정되어 있지 않으므로 최초 사용이다.
		{
			int nMax	= m_Inventory.GetMax();
			CItemElem* pItemElem;
			for( int i = 0 ; i < nMax; i++ )
			{
				pItemElem	= m_Inventory.GetAtId( i );
				if( IsUsableItem( pItemElem )
					&& pItemElem->m_dwItemId == pPocket->m_dwItemId
					&& pItemElem->m_dwKeepTime > 0
					&& !pItemElem->IsFlag( CItemElem::expired )
					)
				{
					// 기간이 만료되지 않은 동일 아이템이 존재하여 사용할 수 없다.
					( (CUser*)this )->AddDefinedText( TID_GAME_DONOTUSE_PETPOCKET, "" );
					nResult = 4;
					return nResult;
				}
			}
			ASSERT( pProp->dwAbilityMin < 259200 );
			UpdateItem( (BYTE)( pPocket->m_dwObjId ), UI_KEEPTIME, pProp->dwAbilityMin );	// 1440	// 1 days
		}
		DoApplySkill( (CCtrl*)this, pProp, NULL );
	}

	return nResult;
}
#endif	// __CLIENT
#endif	// __PET_0410

#ifdef __WORLDSERVER
void CMover::ActivateSystemPet( CItemElem* pItemElem )
{
#ifdef __QUIZ
	if( GetWorld() && GetWorld()->GetID() == WI_WORLD_QUIZ )
		return;
#endif // __QUIZ
	if( IsFly() )
	{
		static_cast<CUser*>( this )->AddDefinedText( TID_GAME_CANNOT_CALL_PET_ON_FLYING );
		return;	
	}
	BOOL bFirst		= FALSE;
	if( pItemElem->m_pPet == NULL )
	{
		pItemElem->m_pPet	= new CPet;
		pItemElem->m_pPet->InitEgg();
		bFirst	= TRUE;
	}
	SetPetId( pItemElem->m_dwObjId );

//#if __VER >= 12 // __PET_0519	// CMover::ProcessPetAvail()로 이동
//	SetDestParamRandomOptExtension( pItemElem );	// 시스템 펫 활성화 시 각성 효과 적용
//#endif	// __PET_0519
	g_dpDBClient.CalluspPetLog( m_idPlayer, pItemElem->GetSerialNumber(), 0, PETLOGTYPE_CALL, pItemElem->m_pPet );
	// pet, log
#ifdef __PET_1024
	g_UserMng.AddPetCall( this, pItemElem->m_dwObjId, pItemElem->m_pPet->GetIndex(), (BYTE)pItemElem->m_pPet->GetLevel(), pItemElem->m_pPet->GetName() );
#else	// __PET_1024
	g_UserMng.AddPetCall( this, pItemElem->m_dwObjId, pItemElem->m_pPet->GetIndex(), (BYTE)pItemElem->m_pPet->GetLevel() );
#endif	// __PET_1024
	
	if( bFirst )
		static_cast<CUser*>(this )->AddPet( pItemElem->m_pPet, PF_PET );
}

void CMover::InactivateSystemPet( CItemElem* pItemElem )
{
	if( GetPetId() == pItemElem->m_dwObjId )
		PetRelease();
	else
		static_cast<CUser*>(this )->AddDefinedText( TID_GAME_PET_NOWUSE );
}

void CMover::DoUseSystemPet( CItemElem* pItemElem )
{
	if( HasActivatedSystemPet() )
		InactivateSystemPet( pItemElem );
	else
		ActivateSystemPet( pItemElem );
}

void CMover::DoUseEatPet( CItemElem* pItemElem )
{
#ifdef __NEW_ITEM_VARUNA
	if( HasActivatedEatPet() )
	{
		CMover* pPet = prj.GetMover( GetEatPetId() );
		if( pPet && pPet->GetProp()->dwAI == AII_NONE )
			InactivateBarunaPet();
		else
			InactivateEatPet();
	}
#else // __NEW_ITEM_VARUNA
	if( HasActivatedEatPet() )
		InactivateEatPet();
#endif // __NEW_ITEM_VARUNA
	else
		ActivateEatPet( pItemElem );
}		

void CMover::ActivateEatPet( CItemElem* pItemElem )
{
	ItemProp* pProp	= pItemElem->GetProp();
	if( !pProp )
		return;

#ifdef __QUIZ
	if( GetWorld() && GetWorld()->GetID() == WI_WORLD_QUIZ )
		return;
#endif // __QUIZ

	if( IsFly() )
	{
		static_cast<CUser*>( this )->AddDefinedText( TID_GAME_CANNOT_CALL_PET_ON_FLYING );
		return;
	}
//		ASSERT( pItemProp->dwLinkKind != NULL_ID )
#ifdef __LAYER_1015
	CMover *pEatPet	= CreateMover( GetWorld(), pProp->dwLinkKind, GetPos(), GetLayer() );
#else	// __LAYER_1015
	CMover *pEatPet	= CreateMover( GetWorld(), pProp->dwLinkKind, GetPos() );
#endif	// __LAYER_1015
	if( pEatPet )
	{
		CAIPet *pAIPet	= static_cast<CAIPet*>( pEatPet->m_pAIInterface );
		if( pAIPet )
		{
			pAIPet->SetOwner( GetId() );
			SetEatPetId( pEatPet->GetId() );
#if __VER >= 12 // __PET_0519
			pAIPet->SetItem( this, pItemElem );		// 먹펫 활성화 시 각성 효과 적용
#endif	// __PET_0519
#if __VER >= 15 // __PETVIS
			if( pItemElem->IsVisPet() )
			{
				if( pItemElem->GetPiercingSize() <= 0 )	// 첫 소환시 비스펫이면 비스 슬롯 2개(기본) 뚫어줌
					UpdateItem( (BYTE)( pItemElem->m_dwObjId ), UI_PETVIS_SIZE, PETVIS_DEFAULT_VIS_SLOT_SZIE );

				SetVisPetItem( pItemElem->m_dwObjId );	// 해당 먹펫이 비스펫이면 비스의 능력치를 적용하고 이펙트를 지정한다.
				SetPetVisDST( pItemElem );
				pEatPet->m_dwMoverSfxId = pItemElem->GetVisPetSfxId();
				static_cast<CUser*>(this)->AddActivateVisPet( pItemElem->m_dwObjId, GetEatPetId() );
			}
#endif // __PETVIS
		}
	}
}

void CMover::InactivateEatPet( void )
{
	CMover* pEatPet		= prj.GetMover( GetEatPetId() );
	if( IsValidObj( pEatPet ) )
	{
#if __VER >= 12 // __PET_0519
		// 먹펫 소환 종료 시 각성 효과 제거
		CAIPet *pAIPet	= static_cast<CAIPet*>( pEatPet->m_pAIInterface );
		if( pAIPet )
		{
#if __VER >= 15 // __PETVIS
			CItemElem* pItemElem = GetVisPetItem();
			if( pItemElem )
			{
				ResetPetVisDST( pItemElem );
				SetVisPetItem( NULL_ID );
				static_cast<CUser*>(this)->AddActivateVisPet( NULL_ID, NULL_ID );
			}
#endif // __PETVIS
			pAIPet->ResetItem();
		}
#endif	// __PET_0519

		pEatPet->Delete();
	}
	SetEatPetId( NULL_ID );
}

#if __VER >= 15 // __PETVIS
void CMover::SetPetVisDST( CItemElem* pItemElem )
{
	if( pItemElem && pItemElem->IsVisPet() )
	{
		AddBuff( BUFF_EQUIP, (WORD)( pItemElem->m_dwItemId ), 1, 999999999 );
		
		vector<BYTE> vecValidTable = GetValidVisTable( pItemElem );
		for( int i=0; i<pItemElem->GetPiercingSize(); i++ )
		{
			ItemProp* pItemProp = prj.GetItemProp( pItemElem->GetPiercingItem( i ) );
			if( vecValidTable[i] == SUCCSESS_NEEDVIS && pItemProp )
			{
				SetDestParam( 0, pItemProp );
				SetDestParam( 1, pItemProp );
				SetDestParam( 2, pItemProp );
			}
			else
			{
				if( IsPlayer() && pItemProp )
					static_cast<CUser*>( this )->AddDefinedText( TID_GAME_BUFFPET_NOTREQVIS, "\"%s\"", pItemProp->szName );
			}
		}
	}
}

void CMover::ResetPetVisDST( CItemElem* pItemElem )
{
	if( pItemElem && pItemElem->IsVisPet() )
	{
		m_buffs.RemoveBuff( BUFF_EQUIP, (WORD)( pItemElem->m_dwItemId ), FALSE );

		vector<BYTE> vecValidTable = GetValidVisTable( pItemElem );
		for( int i=0; i<pItemElem->GetPiercingSize(); i++ )
		{
			ItemProp* pItemProp = prj.GetItemProp( pItemElem->GetPiercingItem( i ) );
			if( vecValidTable[i] == SUCCSESS_NEEDVIS && pItemProp )
			{
				ResetDestParam( 0, pItemProp );
				ResetDestParam( 1, pItemProp );
				ResetDestParam( 2, pItemProp );
			}
		}
	}
}

void CMover::ProcessVisPet()
{
	if( HasActivatedVisPet() )
	{
		CItemElem* pItemElem = GetVisPetItem();
		if( pItemElem )
		{
			for( int i=0; i<pItemElem->GetPiercingSize(); i++ )
			{
				if( pItemElem->GetPiercingItem( i ) != 0 && time_null() >= pItemElem->GetVisKeepTime( i ) )
				{
					ItemProp* pItemProp = prj.GetItemProp( pItemElem->GetPiercingItem( i ) );
					if( pItemProp )
						static_cast<CUser*>( this )->AddDefinedText( TID_GAME_BUFFPET_TIMEOUTVIS, "\"%s\"", pItemProp->szName );
					CItemUpgrade::GetInstance()->RemovePetVisItem( static_cast<CUser*>( this ), i, TRUE );
				}
			}
		}		
	}
}
#endif // __PETVIS
#ifdef __NEW_ITEM_VARUNA
void CMover::DoUseBarunaPet( CItemElem* pItemElem )
{
	if( HasActivatedEatPet() )
	{
		CMover* pPet = prj.GetMover( GetEatPetId() );
		if( pPet && pPet->GetProp()->dwAI == AII_NONE )
		{
			InactivateBarunaPet();
			return;
		}
		else
		{
			InactivateEatPet();
			return;
		}
	}

	ItemProp* pProp	= pItemElem->GetProp();
	if( !pProp )
		return;

#ifdef __QUIZ
	if( GetWorld() && GetWorld()->GetID() == WI_WORLD_QUIZ )
		return;
#endif // __QUIZ

	if( IsFly() )
	{
		static_cast<CUser*>( this )->AddDefinedText( TID_GAME_CANNOT_CALL_PET_ON_FLYING );
		return;
	}

	CMover *pBarunaPet	= CreateMover( GetWorld(), pProp->dwLinkKind, GetPos(), GetLayer() );

	if( pBarunaPet )
	{
		CString szCharacterKey;
		switch( pProp->dwLinkKind )
		{
		case MI_PET_SMELTPIYO:
			szCharacterKey = "Pet_Seltpiyo";
			break;
		case MI_PET_SMELTUDI:
			szCharacterKey = "Pet_Seltpudi";
			break;
		case MI_PET_SMELTCOBI:
			szCharacterKey = "Pet_Seltcobi";
			break;
		case MI_PET_SMELTPIERCE:
			szCharacterKey = "Pet_Seltpierce";
			break;
		default:
			szCharacterKey = "Pet_Seltpiyo";
			break;
		}

		lstrcpy( pBarunaPet->m_szCharacterKey, szCharacterKey );

		pBarunaPet->InitNPCProperty();
		pBarunaPet->InitCharacter( pBarunaPet->GetCharacter() );
		pBarunaPet->m_idBarunaPetOwner = GetId();
		dwBarunaPetItemId = pItemElem->m_dwObjId;
		SetEatPetId( pBarunaPet->GetId() );
		static_cast<CUser*>(this)->AddActivateBarunaPet( pItemElem->m_dwObjId, pBarunaPet->GetId(), TRUE );
	}
}
void CMover::InactivateBarunaPet( void )
{
	CMover* pBarunaPet		= prj.GetMover( GetEatPetId() );
	if( IsValidObj( pBarunaPet ) )
	{
		pBarunaPet->Delete();
	}
	SetEatPetId( NULL_ID );
	static_cast<CUser*>(this)->AddActivateBarunaPet( NULL_ID, NULL_ID, FALSE );
}

#endif // __NEW_ITEM_VARUNA
#endif	// __WORLDSERVER

#if __VER >= 15 // __PETVIS
vector<BYTE> CMover::GetValidVisTable( CItemElem* pItemElem )
{
	vector<BYTE> vecTemp;
	vecTemp.resize( pItemElem->GetPiercingSize(), UNDEFINED_NEEDVIS );

	for( DWORD i=0; i<vecTemp.size(); i++ )
		if( pItemElem->GetPiercingItem( i ) == 0 )
			vecTemp[i] = FAILED_BOTH_NEEDVIS;
	
	for( DWORD i=0; i<vecTemp.size(); i++ )
		if( vecTemp[i] == UNDEFINED_NEEDVIS )
			SetValidNeedVis( pItemElem, i, vecTemp );

	return vecTemp;
}

BOOL CMover::SetValidNeedVis( CItemElem* pItemElem, int nPos, vector<BYTE> & vecValid )
{
	if( vecValid[nPos] != UNDEFINED_NEEDVIS )
		return ( vecValid[nPos] == SUCCSESS_NEEDVIS );
	
	ItemProp* pPropVis = prj.GetItemProp( pItemElem->GetPiercingItem( nPos ) );
	if( !pPropVis )
	{
		vecValid[nPos] = FAILED_BOTH_NEEDVIS;
		return FALSE;
	}

	if( pPropVis->dwReferTarget1 == NULL_ID && pPropVis->dwReferTarget2 == NULL_ID )
	{
		vecValid[nPos] = SUCCSESS_NEEDVIS;
		return TRUE;
	}

	BOOL bResult1 = FALSE, bResult2 = FALSE;
	if( pPropVis->dwReferTarget1 == NULL_ID ) bResult1 = TRUE;
	if( pPropVis->dwReferTarget2 == NULL_ID ) bResult2 = TRUE;

	for( int i=0; i<pItemElem->GetPiercingSize(); i++ )
	{
		if( pPropVis->dwReferTarget1 == pItemElem->GetPiercingItem( i ) )
			bResult1 = SetValidNeedVis( pItemElem, i, vecValid );

		if( pPropVis->dwReferTarget2 == pItemElem->GetPiercingItem( i ) )
			bResult2 = SetValidNeedVis( pItemElem, i, vecValid );

		if( bResult1 && bResult2 )
		{
			vecValid[nPos] = SUCCSESS_NEEDVIS;
			return TRUE;
		}
	}

	if( !bResult1 && bResult2 )
		vecValid[nPos] = FAILED_1ST_NEEDVIS;
	else if( bResult1 && !bResult2 )
		vecValid[nPos] = FAILED_2ND_NEEDVIS;
	else
		vecValid[nPos] = FAILED_BOTH_NEEDVIS;
	return FALSE;
}

BYTE CMover::IsSatisfyNeedVis( CItemElem* pItemElemVisPet, ItemProp* pItemPropVis )
{
	if( pItemPropVis )
	{
		DWORD	dwNeeds[2] = { pItemPropVis->dwReferTarget1, pItemPropVis->dwReferTarget2 };
		if( dwNeeds[0] == NULL_ID && dwNeeds[1] == NULL_ID )
			return SUCCSESS_NEEDVIS;
		
		vector<BYTE> vecValidTable = GetValidVisTable( pItemElemVisPet );
		for( int i=0; i<pItemElemVisPet->GetPiercingSize(); i++ )	// 필요 비스가 충족되지 않은 경우는 능력치가 적용되지 않았으므로 능력치를 빼면 안된다.
		{
			DWORD dwVis = pItemElemVisPet->GetPiercingItem( i );
			if( dwNeeds[0] == dwVis && vecValidTable[i] == SUCCSESS_NEEDVIS && time_null() < pItemElemVisPet->GetVisKeepTime( i ) )	dwNeeds[0] = NULL_ID;
			if( dwNeeds[1] == dwVis && vecValidTable[i] == SUCCSESS_NEEDVIS && time_null() < pItemElemVisPet->GetVisKeepTime( i ) )	dwNeeds[1] = NULL_ID;
			if( dwNeeds[0] == NULL_ID && dwNeeds[1] == NULL_ID )
				return SUCCSESS_NEEDVIS;
		}
			
		if( dwNeeds[0] != NULL_ID && dwNeeds[1] == NULL_ID )
			return FAILED_1ST_NEEDVIS;
		else if( dwNeeds[0] == NULL_ID && dwNeeds[1] != NULL_ID )
			return FAILED_2ND_NEEDVIS;
	}

	return FAILED_BOTH_NEEDVIS;
}
#endif // __PETVIS

#if __VER >= 12 // __PET_0519
// 펫 영양제 사용
int CMover::DoUseItemPetTonic( CItemElem* pItemElem )
{
	// 펫 영양제 두 종류를 섞어서 사용할 수 없다
	if( ( pItemElem->m_dwItemId == II_SYS_SYS_SCR_PET_TONIC_A && HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_PET_TONIC_B ) )
		|| ( pItemElem->m_dwItemId == II_SYS_SYS_SCR_PET_TONIC_B && HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_PET_TONIC_A ) )
	)
		return 2;

	// 시간 오버플로우 막기
	DWORD dwCurr	= ::timeGetTime();
#ifdef __BUFF_1107
	IBuff* pBuff	= m_buffs.GetBuff( BUFF_ITEM, (WORD)( pItemElem->m_dwItemId ) );
	if( pBuff && static_cast<int>( pItemElem->GetProp()->dwSkillTime ) > static_cast<int>( pItemElem->GetProp()->dwSkillTime + ( pBuff->GetTotal() - ( dwCurr - pBuff->GetInst() ) ) ) )
		return 1;
#else	// __BUFF_1107
	LPSKILLINFLUENCE ptr	= m_SkillState.FindPtr( BUFF_ITEM, pItemElem->m_dwItemId );
	if( ptr && static_cast<int>( pItemElem->GetProp()->dwSkillTime ) > static_cast<int>( pItemElem->GetProp()->dwSkillTime + ( ptr->tmCount - ( dwCurr - ptr->tmTime ) ) ) )
		return 1;
#endif	// __BUFF_1107

	// 효과 적용
	DoApplySkill( this, pItemElem->GetProp(), NULL );
	return 0;
}
#endif	// __PET_0519