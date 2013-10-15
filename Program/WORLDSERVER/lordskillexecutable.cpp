#include "stdafx.h"

#if __VER >= 12 // __LORD

#include "lordskillexecutable.h"
#include "lord.h"
#include "definelordskill.h"
#include "definetext.h"
#include "user.h"
#include "randomoption.h"
#include "slord.h"
extern	CUserMng	g_UserMng;

void CLordSkillComponentItemBuf::Execute( u_long idPlayer, u_long idTarget, VOID* pParam )
{
	Use();
	CUser* pUser	= g_UserMng.GetUserByPlayerID( idPlayer );
	if( IsValidObj( pUser ) )
	{
		CUser* pTarget	= pUser;
		if( GetTargetType() != LTT_SELF )
			pTarget		= g_UserMng.GetUserByPlayerID( idTarget );
		if( IsValidObj( pTarget ) && pTarget->GetType() == OT_MOVER && pTarget->IsPlayer() )
		{
			ItemProp* pItemProp		= prj.GetItemProp( GetItem() );
			if( pItemProp )
			{
				// #motion
				pUser->DoApplySkill( pTarget, pItemProp, NULL );
				// #print
				if( GetId() == LI_RAGE )
				{
					pUser->AddDefinedText( TID_GAME_LORD_SKILL_RAGE_S001, "%s", pTarget->GetName() );
					pTarget->AddDefinedText( TID_GAME_LORD_SKILL_RAGE_S002 );
				}
				PutSkillLog( pTarget );
			}
		}
	}
}

void CLordSkillComponentSummon::Execute( u_long idPlayer, u_long idTarget, VOID* pParam )
{
	Use();
	CUser* pUser	= g_UserMng.GetUserByPlayerID( idPlayer );
	CUser* pTarget	= g_UserMng.GetUserByPlayerID( idTarget );
	if( IsValidObj( pTarget ) && IsValidObj( pUser ) && pUser->GetWorld() )
	{
		pTarget->AddDefinedText( TID_GAME_LORD_SKILL_SUMMON_S001 );
		pTarget->REPLACE( g_uIdofMulti, pUser->GetWorld()->GetID(), pUser->GetPos(), REPLACE_NORMAL, pUser->GetLayer() );
		PutSkillLog( pTarget );
	}
}

int CLordSkillComponentSummon::IsExecutable( CUser* pUser, const char* szTarget, u_long &idTarget )
{
	int nText	= CLordSkillComponentExecutable::IsExecutable( pUser, szTarget, idTarget );
	return( nText > 0? nText: IsTeleportable( pUser, g_UserMng.GetUserByPlayerID( idTarget ) ) );
}

void CLordSkillComponentTeleport::Execute( u_long idPlayer, u_long idTarget, VOID* pParam )
{
	Use();
	CUser* pUser	= g_UserMng.GetUserByPlayerID( idPlayer );
	CUser* pTarget	= g_UserMng.GetUserByPlayerID( idTarget );
	if( IsValidObj( pUser ) && IsValidObj( pTarget ) && pTarget->GetWorld() )
	{
		pUser->REPLACE( g_uIdofMulti, pTarget->GetWorld()->GetID(), pTarget->GetPos(), REPLACE_NORMAL, pTarget->GetLayer() );
		PutSkillLog( pTarget );
	}
}

int CLordSkillComponentTeleport::IsExecutable( CUser* pUser, const char* szTarget, u_long &idTarget )
{
	int nText	= CLordSkillComponentExecutable::IsExecutable( pUser, szTarget, idTarget );
	return( nText > 0? nText: IsTeleportable( pUser, g_UserMng.GetUserByPlayerID( idTarget ) ) );	// 12-2 ˬ
}

namespace	lordskill
{
	int UseRequirements( CUser* pUser, const char* szTarget, int nSkill, u_long & idTarget )
	{
		CLordSkill* pSkills		= CSLord::Instance()->GetSkills();
		CLordSkillComponentExecutable* pComponent	= pSkills->GetSkill( nSkill );
		if( !pComponent )
			return TID_GAME_LORD_SKILL_USE_E002;
		return pComponent->IsExecutable( pUser, szTarget, idTarget );
	}
};

#endif	// __LORD