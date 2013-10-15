#include "stdafx.h"
#include "mover.h"

BOOL CMover::HasBuff( WORD wType, WORD wId )
{
#ifdef __BUFF_1107
	return m_buffs.HasBuff( wType, wId );
#else	// __BUFF_1107
	return m_SkillState.HasSkill( wType, wId );
#endif	// __BUFF_1107
}

void CMover::RemoveBuff( WORD wType, WORD wId )
{
#ifdef __BUFF_1107
	m_buffs.RemoveBuff( wType, wId );
#else	// __BUFF_1107
	m_SkillState.RemoveSkillInfluence( wType, wId );
#endif	// __BUFF_1107
}

BOOL CMover::HasBuffByIk3( DWORD dwIk3 )
{
#ifdef __BUFF_1107
	return m_buffs.HasBuffByIk3( dwIk3 );
#else	// __BUFF_1107
	return m_SkillState.HasLikeItemBuf( dwIk3 );
#endif	// __BUFF_1107
}

#ifdef __WORLDSERVER
BOOL CMover::HasPet()
{
#ifdef __BUFF_1107
	return m_buffs.HasPet();
#else	// __BUFF_1107
	return m_SkillState.HasPet();
#endif	// __BUFF_1107
}

void CMover::RemovePet()
{
#ifdef __BUFF_1107
	m_buffs.RemoveBuffPet();
#else	// __BUFF_1107
	m_SkillState.RemovePet();
#endif	// __BUFF_1107
}
#endif	// __WORLDSERVER

DWORD CMover::GetDisguise()
{
#ifdef __BUFF_1107
	return m_buffs.GetDisguise();
#else	// __BUFF_1107
	#ifdef __CLIENT
	return m_SkillState.GetDisguise();					
	#endif // __CLIENT

	return 0;
#endif	// __BUFF_1107
}

BOOL CMover::AddBuff( WORD wType, WORD wId, DWORD dwLevel, DWORD tmTime, OBJID oiAttacker )
{
#ifdef __BUFF_1107
	return m_buffs.AddBuff( wType, wId, dwLevel, tmTime, oiAttacker );
#else	// __BUFF_1107
	#ifdef __PVPDEBUFSKILL
	return m_SkillState.Set( wType, wId, dwLevel, tmTime, oiAttacker );
	#else // __PVPDEBUFSKILL
	return m_SkillState.Set( wType, wId, dwLevel, tmTime );
	#endif // __PVPDEBUFSKILL
#endif	// __BUFF_1107
}

void CMover::RemoveCommonBuffs()
{
#ifdef __BUFF_1107
	m_buffs.RemoveBuffs( RBF_COMMON );
#else	// __BUFF_1107
	m_SkillState.RemoveAllSkillInfluence();
#endif	// __BUFF_1107
}

#ifdef __CLIENT
void CMover::ClearBuffInst()
{
#ifdef __BUFF_1107
	m_buffs.ClearInst();
#else	// __BUFF_1107
	m_SkillState.Reset();
#endif	// __BUFF_1107
}
#endif	// __CLIENT

void CMover::RemoveChrStateBuffs( DWORD dwChrState )
{
#ifdef __BUFF_1107
	m_buffs.RemoveBuffs( RBF_CHRSTATE, dwChrState );
#else	// __BUFF_1107
	m_SkillState.RemoveSkillInfluenceState( dwChrState );
#endif	// __BUFF_1107
}

void CMover::RemoveIk3Buffs( DWORD dwIk3 )
{
#ifdef __BUFF_1107
	m_buffs.RemoveBuffs( RBF_IK3, dwIk3 );
#else	// __BUFF_1107
	m_SkillState.RemoveLikeItemBuf( dwIk3 );
#endif	// __BUFF_1107
}

void CMover::RemoveAllBuff()
{
#ifdef __BUFF_1107
	m_buffs.RemoveBuffs( RBF_UNCONDITIONAL );
#else	// __BUFF_1107
	m_SkillState.RemoveAllBuff();
#endif	// __BUFF_1107
}

void CMover::ProcessBuff()
{
#ifdef __BUFF_1107
	m_buffs.Process();
#else	// __BUFF_1107
	m_SkillState.Process();
#endif	// __BUFF_1107
}

void CMover::RemoveAttackerBuffs( OBJID oiAttacker )
{
#ifdef __BUFF_1107
	m_buffs.RemoveBuffs( RBF_ATTACKER, oiAttacker );
#else	// __BUFF_1107
	#ifdef __PVPDEBUFSKILL
	m_SkillState.RemoveSkillInfluenceFromID( oiAttacker );
	#endif // __PVPDEBUFSKILL
#endif	// __BUFF_1107
}

void CMover::RemoveDstParamBuffs( DWORD dwDstParam )
{
#ifdef __BUFF_1107
	m_buffs.RemoveBuffs( RBF_DSTPARAM, dwDstParam );
#else	// __BUFF_1107
	m_SkillState.RemoveSkillInfluenceDestParam( dwDstParam );
#endif	// __BUFF_1107
}

void CMover::RemoveDebufBuffs()
{
#ifdef __BUFF_1107
	m_buffs.RemoveBuffs( RBF_DEBUF );
#else	// __BUFF_1107
	m_SkillState.RemoveAllSkillDebuff();
#endif	// __BUFF_1107
}