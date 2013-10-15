#include "stdafx.h"
#include "CtrlObjs.h"
#include "defineSkill.h"
#include "user.h"
#include "dpsrvr.h"
#include "Party.h"
#include "dpDatabaseClient.h"

 
extern	CUserMng	g_UserMng;
extern	CDPSrvr		g_DPSrvr;
extern	CPartyMng	g_PartyMng;
extern	CDPDatabaseClient	g_dpDBClient;


void CContDamageCtrl::Init( void )
{
	m_tmStart = m_tmUpdate = timeGetTime();
	m_pSkillProp = NULL;
	m_pAddSkillProp = NULL;
	m_idTarget = m_idSrc = NULL_ID;
#if __VER >= 8 // __S8_PK
	m_bControl = FALSE;
#endif // __VER >= 8 // __S8_PK
	// chipi - 범위 ctrl 생성 스킬로 인해 듀얼 종료 후 카오되는 현상 수정
	m_bDuelTarget = FALSE;
}
 
void CContDamageCtrl::Destroy( void )
{
	Init();
}
 
void CContDamageCtrl::ApplyDamage( void )
{
	ItemProp *pSkillProp = m_pSkillProp;
	AddSkillProp *pAddSkillProp = m_pAddSkillProp;
	
	CCtrl *pSrc = prj.GetCtrl( m_idSrc );			// 시전자.  중간에 없어질수도 있슴.
	if( IsInvalidObj(pSrc) )
	{
		// 이때 처리를 해야한다.
		Delete();
		return;
	}

	// chipi - 범위 ctrl 생성 스킬로 인해 듀얼 종료 후 카오되는 현상 수정
	if( pSrc->GetType() == OT_MOVER && !((CMover*)pSrc)->m_nDuel && m_bDuelTarget )
		return;
	
	CCtrl *pCenter = prj.GetCtrl( m_idTarget );		// 중심이 되는 오브젝트.  중간에 없어질수도 있슴.
	D3DXVECTOR3 vPos = GetPos();					// 중심좌표
	int nApplyType;
	nApplyType = OBJTYPE_PLAYER | OBJTYPE_MONSTER;
#if __VER >= 8 // __S8_PK
	pSrc->ApplySkillRegion( vPos, nApplyType, pSkillProp, pAddSkillProp, false, TRUE, pCenter, m_bControl );
#else // __VER >= 8 // __S8_PK
	pSrc->ApplySkillRegion( vPos, nApplyType, pSkillProp, pAddSkillProp, false, TRUE, pCenter );
#endif // __VER >= 8 // __S8_PK
}

 // 지속데미지를 줌.
void CContDamageCtrl::Process()
{
	ItemProp *pSkillProp = m_pSkillProp;
	AddSkillProp *pAddSkillProp = m_pAddSkillProp;
	
	if( pSkillProp->tmContinuousPain == NULL_ID )
		Error( "%s의 SkillProp tmContinuousPain의 값이 -1", pSkillProp->dwID );

	if( (int)(g_tmCurrent - m_tmUpdate) > (int)pSkillProp->tmContinuousPain )		// 단위시간이 지나면
	{
		m_tmUpdate = g_tmCurrent;		// 시간 갱신하고
		// 지속데미지 처리를 이곳에 넣음.
		ApplyDamage();
	}

	if( (int)(g_tmCurrent - m_tmStart) > (int)m_pAddSkillProp->dwSkillTime )		// 총시간이 지나면 끝.
		Delete();
}
 
	

