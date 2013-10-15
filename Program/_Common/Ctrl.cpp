// Ctrl.cpp: implementation of the CCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "defineSkill.h"
#include "defineObj.h"
#include "defineText.h"
#include "ResData.h"
#ifdef __WORLDSERVER
#include "CtrlObjs.h"
#endif
#include "Ship.h"

#ifdef __CLIENT
#include "DPClient.h"
extern	CDPClient	g_DPlay;
#endif  

#ifdef __WORLDSERVER
#include "user.h"
#include "dpcoreclient.h"
#include "dpsrvr.h"

#ifdef __WORLDSERVER
 extern	CUserMng	g_UserMng;
 extern	CDPCoreClient	g_DPCoreClient;
 extern	CDPSrvr		g_DPSrvr;
 #include "Party.h"
 extern	CPartyMng	g_PartyMng;
 #include "dpDatabaseClient.h"
 extern	CDPDatabaseClient	g_dpDBClient;
#endif // Worldserver
#endif

#include "commonctrl.h"

extern map< string, DWORD > g_MapStrToObjId;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCtrl::CCtrl()
{
	m_dwType = OT_CTRL;
	m_objid	 = NULL_ID;

#ifdef __WORLDSERVER
	m_lRespawn	= -1;
	m_nRespawnType = 0;

	memset( m_nOldCenter, 0, sizeof(int) * MAX_LINKLEVEL );
#endif	// __WORLDSERVER

	m_pIAObjLink = NULL;
	D3DXMatrixIdentity( &m_mInvTM );
}

CCtrl::~CCtrl()
{
#ifdef __WORLDSERVER
	if( IsVirtual() )
		return;

	RemoveItFromView();
#endif	// __WORLDSERVER

	RemoveItFromGlobalId();
}

void CCtrl::Process()
{
	CObj::Process();
#ifdef __WORLDSERVER
	ProcessDeleteRespawn();
#endif
}

void CCtrl::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	CObj::Render( pd3dDevice );
}

BOOL CCtrl::Read( CFileIO* pFile )
{
	return CObj::Read( pFile );
}
// this가 pIA에 링크되었다.
// pIA가 NULL이면 링크해제.
void CCtrl::SetIAObjLink( CShip *pIA )
{
	if( pIA != NULL )	// IA에 링크시켜야 할때...
	{
		if( m_pIAObjLink != pIA )				// 기존에 링크랑 다른링크가 될때만...
			if( pIA->FindCtrl( GetId() ) == NULL )			// pIA에 이미 this가 등록되었는지 보고...
				pIA->AddCtrl( GetId() );			// IA오브젝트에 this를 등록시킴.
	} else
	{
		if( m_pIAObjLink )
			m_pIAObjLink->RemoveCtrl( GetId() );
	}

	m_pIAObjLink = pIA;
}

void CCtrl::AddItToGlobalId()
{
	prj.m_objmap.Add( this );

	if( OT_MOVER == GetType() && ( (CMover*)this )->IsPlayer() )
	{
		bool bResult = prj.m_idPlayerToUserPtr.insert( make_pair( ( (CMover*)this )->m_idPlayer, (CMover*)this ) ).second;
		if( bResult == false )
			Error("AddItToGlobalId id:%d duplicated.", ((CMover*)this)->m_idPlayer );
	}

#ifdef __WORLDSERVER	
	if( GetType() == OT_CTRL )
	{
		CCommonCtrl* pCommonCtrl = (CCommonCtrl*)this;
		if( pCommonCtrl->m_CtrlElem.m_strCtrlKey[0] != '\0' )
			g_MapStrToObjId.insert( map< string, DWORD >::value_type(pCommonCtrl->m_CtrlElem.m_strCtrlKey, pCommonCtrl->GetId() ) );
	}
#endif // __WORLDSERVER
}

void CCtrl::RemoveItFromGlobalId()
{
	if( m_objid == NULL_ID )	
		return;

	prj.m_objmap.RemoveKey( m_objid );

	if( OT_MOVER == GetType() && ( (CMover*)this )->IsPlayer() && ( (CMover*)this )->m_idPlayer != (u_long)0xffffffff )
		prj.m_idPlayerToUserPtr.erase( ( (CMover*)this )->m_idPlayer );

#ifdef __WORLDSERVER	
	if( GetType() == OT_CTRL )
	{
		CCommonCtrl* pCommonCtrl = (CCommonCtrl*)this;
		if( pCommonCtrl->m_CtrlElem.m_strCtrlKey[0] != '\0' )
			g_MapStrToObjId.erase( pCommonCtrl->m_CtrlElem.m_strCtrlKey );
	}
#endif // __WORLDSERVER
}

#ifdef __WORLDSERVER

BOOL CCtrl::ProcessDeleteRespawn()
{
	// 리스폰이 Remove이면 삭제 명령 
#ifdef __LAYER_1021
	CRespawnInfo* pRespawnInfo = GetWorld()->m_respawner.GetRespawnInfo( GetRespawn(), m_nRespawnType, GetLayer() );
#else	// __LAYER_1021
	CRespawnInfo* pRespawnInfo = GetWorld()->m_respawner.GetRespawnInfo( GetRespawn(), m_nRespawnType );
#endif	// __LAYER_1021
	if( pRespawnInfo && pRespawnInfo->m_bRemove )
	{
		Delete();
		return TRUE;
	}
	return FALSE;
}

void CCtrl::RemoveItFromView( BOOL bRemoveall )
{
	if( !GetWorld() )	
		return;

	CUser* pUser;
	if( GetType() == OT_MOVER && ( (CMover*)this )->IsPlayer() )
	{
		map<DWORD, CUser* >::iterator it = m_2pc.begin();
		while( it != m_2pc.end() )
		{
			pUser = it->second;
			if( pUser != this && pUser->PCRemoveKey( GetId() ) )
				pUser->AddRemoveObj( GetId() );
			++it;
		}

		if( bRemoveall )
			m_2pc.clear();
	}
	else
	{
		map<DWORD, CUser* >::iterator it = m_2pc.begin();
		while( it != m_2pc.end() )
		{
			pUser = it->second;
			if( pUser->NPCRemoveKey( GetId() ) )
				pUser->AddRemoveObj( GetId() );
			++it;
		}

		if( bRemoveall )
			m_2pc.clear();
	}

}

BOOL CCtrl::IsNearPC( CUser* pUser )
{
	return IsNearPC( pUser->GetId() );
}

BOOL CCtrl::IsNearPC( OBJID objid )
{
	map<DWORD, CUser* >::iterator it = m_2pc.find( objid );
	return ( it != m_2pc.end() ) ;
}

#endif	// __WORLDSERVER

BOOL	CCtrl::GetSkillProp( ItemProp **ppSkillProp, AddSkillProp **ppAddSkillProp, int nSkill, DWORD dwLevel, LPCTSTR szErr )
{
	ItemProp* pSkillProp = prj.GetSkillProp( nSkill );	// UseSkill에서 사용한 스킬의 프로퍼티 꺼냄
	if( pSkillProp == NULL )
	{
		Error( "%s : %d. 스킬(%d)의 프로퍼티가 없다.", szErr, GetIndex(), nSkill );
		return FALSE;
	}
	
	
	AddSkillProp *pAddSkillProp	= prj.GetAddSkillProp( pSkillProp->dwSubDefine, dwLevel );
	if( pAddSkillProp == NULL )
	{
		Error( "%s : %d. Add스킬(%d) Lv%d의 프로퍼티가 없다.", szErr, GetIndex(), nSkill, dwLevel );
		return FALSE;
	}

	if( pAddSkillProp->dwName != pSkillProp->dwID )
		Error( "%s : %d. Add스킬(%d) AddSkill->dwName과 Skill->dwID가 다르다.", szErr, GetIndex(), nSkill );
	
	*ppSkillProp = pSkillProp;
	*ppAddSkillProp = pAddSkillProp;
	
	return TRUE;
}


// vPos를 중심으로 주변에 효과를 적용한다.
// ------------------------------------------
// 트랩이 쓸수도 있고. 몬스터가쓸수도있다(뱅), 주인공이 스킬로 사용할수도 있다.
// 효과는 반드시 SkillProp을 만들어서 그 아이디를 넘겨야 한다.
//
// 경우에따라 pAddSkillProp은 NULL일수도 있다.  아이템에서 사용할수도 있기때문. 아이템은 애드스킬프롭이 없다.
// pCenter : 광역 지속스킬의 타겟. NULL일수도 있다
#if __VER >= 8 // __S8_PK
void	CCtrl::ApplySkillRegion( const D3DXVECTOR3 &vPos, int nApplyType, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProb, BOOL bOnlyDmg, CCtrl* pCenter, BOOL bControl )
#else // __VER >= 8 // __S8_PK
void	CCtrl::ApplySkillRegion( const D3DXVECTOR3 &vPos, int nApplyType, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProb, BOOL bOnlyDmg, CCtrl* pCenter )
#endif // __VER >= 8 // __S8_PK
{
#ifdef __WORLDSERVER
	int nRange	= 4;	// 4m
	CObj* pObj;
	CCtrl *pSrc = this;
	D3DXVECTOR3 vDist;

	if( pSkillProp == NULL )
		Error( "CCtrl::ApplySkillRegion : pSkillProp == NULL A:%d-%d", pSrc->GetType(), pSrc->GetIndex() );

	FLOAT fRange = 4.0f;
	if( pAddSkillProp )										// 애드스킬이 있으면 거기서 꺼냄
	{
		fRange = (float)pAddSkillProp->dwSkillRange;
		if( pAddSkillProp->dwSkillRange == NULL_ID )
			Error( "경고 : 스킬 %d의 dwSkillRange가 지정되지 않음", pAddSkillProp->dwID );
	}
	else
	{
		fRange = (float)pSkillProp->_dwSkillRange;			// 없으면 스킬프로퍼티에서 꺼냄.
		if( pSkillProp->_dwSkillRange == NULL_ID )
			Error( "경고 : 스킬 %d의 dwSkillRange가 지정되지 않음", pSkillProp->dwID );
	}

	if( fRange <= 4.0f )		nRange = 4;
	else if( fRange <= 8.0f )	nRange = 8;
	else if( fRange <= 16.0f )	nRange = 16;
	else						nRange = 32;

	if( fRange <= 0 )	// 범위가 0이거나 음수일수는 없다.
		Error( "CCtrl::ApplySkillRegion : D:%d A:%d-%d %d %f", GetIndex(), pSrc->GetType(), pSrc->GetIndex(), pSkillProp->dwID, fRange );

	BOOL	bApply = FALSE;
	BOOL	bTarget = TRUE;

	//------------ 적용대상이 플레이어인가 
	if( nApplyType & OBJTYPE_PLAYER )	
	{
		FOR_LINKMAP( GetWorld(), vPos, pObj, nRange, CObj::linkPlayer, GetLayer() )
		{
			bApply = FALSE;	// 일단 FALSE로 초기화

			if( pObj->GetType() != OT_MOVER )
				Error( "ApplySkillRegion : pObj가 MOVER가 아니다 %d", pObj->GetType() );

			if( pSrc->GetType() == OT_MOVER )		// 공격자가 무버냐.
			{
				CMover *pAttacker = (CMover *)pSrc;
				if( pAttacker->IsPlayer() )			// 공격자가 플레이어냐
				{
					bApply = TRUE;
#if __VER >= 8 // __S8_PK
					CMover *pDefender = (CMover *)pObj;
					if( pDefender->IsPlayer() )
					{
						if( pCenter )
							bTarget = (pCenter == pDefender);
						else
							bTarget = FALSE;

						if( bControl == FALSE && pAttacker->GetHitType( pDefender, bTarget, FALSE ) == HITTYPE_PK )
							bApply = FALSE;
//	#ifdef	__JHMA_VER_8_5_2			 // 8.5차 듀얼중인 두구룹이 연관되지않게 수정  World
						else if( pAttacker->m_nDuel == 1 && pDefender->m_nDuel == 1 && pAttacker->GetHitType( pDefender, bTarget, FALSE ) != HITTYPE_PVP )
							bApply = FALSE;
//	#endif // __JHMA_VER_8_5_1	
					}
#endif // __VER >= 8 // __S8_PK
				} else
				{	// 공격자가 몬스터면
					bApply = TRUE;		// 
				}
			} else
			{	// 공격자가 무버가 아니다.
				bApply = TRUE;
			}

			if( bApply )
			{
				if( pObj != pSrc )		// 어태커는 검색대상에서 제외.
				{
					if( pObj->IsRangeObj( vPos, fRange ) )	// vCenter + fRange와 pObj의 스피어가 충돌했느냐
					{
						CMover *pTarget = (CMover *)pObj;		// <- 여긴 무조건 pObj는 플레이어므로 해도됨.
					
						if( pCenter )
							bTarget = (pCenter == pTarget);
						else
							bTarget = FALSE;

						if( IsValidObj( pTarget ) && pTarget->IsLive() )
						{
							pTarget->ApplySkill( pSrc, pSkillProp, pAddSkillProp, bIgnoreProb, 0, bOnlyDmg, bTarget );		// 대상에게 효과를 적용함.
						}
					}
				}
				
				bApply = FALSE;	// 다음 루프를 위해서 초기화.
			} // bApply
		}
		END_LINKMAP
	} // OBJTYPE_PLAYER

	// 적용대상이 몬스터/컨트롤 인가.
	if( nApplyType & (OBJTYPE_MONSTER | OBJTYPE_CTRL) )
	{
		FOR_LINKMAP( GetWorld(), vPos, pObj, nRange, CObj::linkDynamic, GetLayer() )	// linkDynamic을 쓴다
		{
			bApply = FALSE;
			if( pObj->GetType() == OT_MOVER )				// 대상이 무버면.
			{
				CMover *pTarget = (CMover *)pObj;
				if( pTarget->IsPeaceful() == FALSE )		// NPC가 아닌경우만 적용
					bApply = TRUE;
#if __VER >= 8 //	#ifdef	__JHMA_VER_8_5_1			 // 8.5차 경비병 범위스킬 공격효과 불가로 수정 World
				CMover *pAttacker = (CMover *)pSrc;
				if( pAttacker->IsPlayer() && pAttacker->IsChaotic() == FALSE && pTarget->GetProp()->dwClass == RANK_GUARD )
					bApply = FALSE;
#endif //	#endif // __JHMA_VER_8_5_1			 // 8.5차 경비병 범위스킬 공격효과 불가로 수정 World
			} else
			// 대상이 컨트롤이면.
			if( pObj->GetType() == OT_CTRL )
			{
				if( pSrc->GetType() == OT_MOVER )			// 공격자가 무버일때만 적용. 컨트롤이 컨트롤에게 영향을 줄순 없다.
					bApply = TRUE;							
			}

			if( bApply )
			{
				if( pObj != pSrc )		// 공격자는 검사대상에서 제외.
				{
					if( pObj->IsRangeObj( vPos, fRange ) )	// vCenter + fRange와 pObj의 스피어가 충돌했느냐
					{
						if( pObj->GetType() == OT_MOVER )	// 타겟이 무버냐?
						{
							if( IsValidObj( pObj ) && ((CMover *)pObj)->IsLive() )		// 타겟이 무버면 살아있는지 검사.
								((CCtrl*)pObj)->ApplySkill( pSrc, pSkillProp, pAddSkillProp, bIgnoreProb, 0, bOnlyDmg );		// 대상에게 효과를 적용함.
						} else
						{
							// 타겟이 무버가 아니냐?
							((CCtrl*)pObj)->ApplySkill( pSrc, pSkillProp, pAddSkillProp, bIgnoreProb, 0, bOnlyDmg );		// 대상에게 효과를 적용함.
						}
					}
				}
			} // bApply
		}
		END_LINKMAP
	}
		
#endif // WORLDSERVER		
} // ApplySkillRegion()


// this를 중심으로 주변에 효과를 적용한다.
// ------------------------------------------
// 트랩이 쓸수도 있고. 몬스터가쓸수도있다(뱅), 주인공이 스킬로 사용할수도 있다.
// 효과는 반드시 SkillProp을 만들어서 그 아이디를 넘겨야 한다.
//
// 경우에따라 pAddSkillProp은 NULL일수도 있다.  아이템에서 사용할수도 있기때문. 아이템은 애드스킬프롭이 없다.
// fRangeCustom에 값이 있을경우에는 프로퍼티 nRange값을 무시하고 fRangeCustom으로 바뀐다.
#if __VER >= 8 // __S8_PK
void	CCtrl::ApplySkillAround( CCtrl *pSrc, int nApplyType, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProb, BOOL bOnlyDmg, FLOAT fRangeCustom, BOOL bControl )
#else // __VER >= 8 // __S8_PK
void	CCtrl::ApplySkillAround( CCtrl *pSrc, int nApplyType, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProb, BOOL bOnlyDmg, FLOAT fRangeCustom )
#endif // __VER >= 8 // __S8_PK
{
#ifdef __WORLDSERVER
	int nRange	= 4;	// 4m
	CObj* pObj;
	D3DXVECTOR3 vPos = GetPos();
	D3DXVECTOR3 vDist;

	if( pSkillProp == NULL )
		Error( "CCtrl::ApplySkillAround : pSkillProp == NULL D:%d A:%d-%d", GetIndex(), pSrc->GetType(), pSrc->GetIndex() );

	FLOAT fRange = 4.0f;
	if( pAddSkillProp )										// 애드스킬이 있으면 거기서 꺼냄
		fRange = (float)pAddSkillProp->dwSkillRange;
	else
		fRange = (float)pSkillProp->_dwSkillRange;			// 없으면 스킬프로퍼티에서 꺼냄.

	if( fRange <= 4.0f )		nRange = 4;
	else if( fRange <= 8.0f )	nRange = 8;
	else if( fRange <= 16.0f )	nRange = 16;
	else						nRange = 32;

	if( fRangeCustom != 0.0f )
	{
		fRange = fRangeCustom;
	}

	FLOAT fTargetRatio = 0.0f;
	if( pSkillProp->dwSpellRegion == SRO_REGION )	// REGION속성일땐 타겟도 데미지를 먹음.
		fTargetRatio = 1.0f;

	if( fRange <= 0 )	// 범위가 0이거나 음수일수는 없다.
		Error( "CCtrl::ApplySkillAround : D:%d A:%d-%d %d %f", GetIndex(), pSrc->GetType(), pSrc->GetIndex(), pSkillProp->dwID, fRange );

	BOOL	bApply = FALSE;
	BOOL	bTarget = FALSE;

	//------------ 적용대상이 플레이어인가 
	if( nApplyType & OBJTYPE_PLAYER )	
	{
		FOR_LINKMAP( GetWorld(), vPos, pObj, nRange, CObj::linkPlayer, GetLayer() )
		{
			bApply = FALSE;	// 일단 FALSE로 초기화

			if( pObj->GetType() != OT_MOVER )
				Error( "ApplySkillAround : pObj가 MOVER가 아니다 %d", pObj->GetType() );

			if( pSrc->GetType() == OT_MOVER )		// 공격자가 무버냐.
			{
				CMover *pAttacker = (CMover *)pSrc;
				if( pAttacker->IsPlayer() )			// 공격자가 플레이어냐
				{
					bApply	= TRUE;
#if __VER >= 8 // __S8_PK
					CMover *pDefender = (CMover *)pObj;
					if( pDefender->IsPlayer() )
					{
						bTarget = (this == pDefender);
						
						if( bControl == FALSE && pAttacker->GetHitType( pDefender, bTarget, FALSE ) == HITTYPE_PK )
							bApply = FALSE;
//	#ifdef	__JHMA_VER_8_5_2			 // 8.5차 듀얼중인 두구룹이 연관되지않게 수정  World
						else if( pAttacker->m_nDuel == 1 && pDefender->m_nDuel == 1 && pAttacker->GetHitType( pDefender, bTarget, FALSE ) != HITTYPE_PVP )
							bApply = FALSE;
//	#endif // __JHMA_VER_8_5_1	

					}
#endif // __VER >= 8 // __S8_PK
				} else
				{	// 공격자가 몬스터면
					bApply = TRUE;		// 
				}
			} else
			{	// 공격자가 무버가 아니다.
				bApply = TRUE;
			}

			if( bApply )
			{
				if( pObj->IsRangeObj( this, fRange ) )	// this(Center)의 스피어와 pObj의 스피어가 충돌했느냐
				{
					if( pObj == this && fTargetRatio == 0.0f )	// 타겟데미지 비율이 0이면 자기자신(타겟)은 데미지를 주지않음.
					{
					} else
					{
						if( pObj != pSrc )		// 어태커는 검색대상에서 제외.
						{
							CMover *pTarget = (CMover *)pObj;		// <- 여긴 무조건 pObj는 플레이어므로 해도됨.
							bTarget = (this == pTarget);
							if( IsValidObj( pTarget ) && pTarget->IsLive() )
								pTarget->ApplySkill( pSrc, pSkillProp, pAddSkillProp, bIgnoreProb, 0, bOnlyDmg, bTarget );		// 대상에게 효과를 적용함.
						}
					}
				}
				
				bApply = FALSE;	// 다음 루프를 위해서 초기화.
			} // bApply
		}
		END_LINKMAP
	} // OBJTYPE_PLAYER

	// 적용대상이 몬스터/컨트롤 인가.
	if( nApplyType & (OBJTYPE_MONSTER | OBJTYPE_CTRL) )
	{
		FOR_LINKMAP( GetWorld(), vPos, pObj, nRange, CObj::linkDynamic, GetLayer() )	// linkDynamic을 쓴다
		{
			bApply = FALSE;
			if( pObj->GetType() == OT_MOVER )				// 대상이 무버면.
			{
				CMover *pTarget = (CMover *)pObj;
				if( pTarget->IsPeaceful() == FALSE )		// NPC가 아닌경우만 적용
					bApply = TRUE;
#if __VER >= 8 //	#ifdef	__JHMA_VER_8_5_1			 // 8.5차 경비병 범위스킬 공격효과 불가로 수정 World
				CMover *pAttacker = (CMover *)pSrc;
				if( pAttacker->IsPlayer() && pAttacker->IsChaotic() == FALSE && pTarget->GetProp()->dwClass == RANK_GUARD )
					bApply = FALSE;
#endif //	#endif // __JHMA_VER_8_5_1			 // 8.5차 경비병 범위스킬 공격효과 불가로 수정 World
			} else
			// 대상이 컨트롤이면.
			if( pObj->GetType() == OT_CTRL )
			{
				if( pSrc->GetType() == OT_MOVER )			// 공격자가 무버일때만 적용. 컨트롤이 컨트롤에게 영향을 줄순 없다.
					bApply = TRUE;							
			}

			if( bApply )
			{
				if( pObj->IsRangeObj( this, fRange ) )	// this(Center)의 스피어와 pObj의 스피어가 충돌했느냐
				{
					if( pObj == this && fTargetRatio == 0.0f )	// 타겟데미지 비율이 0이면 자기자신(타겟)은 데미지를 주지않음.
					{
					} else
					{
						if( pObj != pSrc )		// 공격자는 검사대상에서 제외.
						{
							if( pObj->GetType() == OT_MOVER )	// 타겟이 무버냐?
							{
								if( IsValidObj( pObj ) && ((CMover *)pObj)->IsLive() )		// 타겟이 무버면 살아있는지 검사.
									((CCtrl*)pObj)->ApplySkill( pSrc, pSkillProp, pAddSkillProp, bIgnoreProb, 0, bOnlyDmg );		// 대상에게 효과를 적용함.
							} else
							{
								// 타겟이 무버가 아니냐?
								((CCtrl*)pObj)->ApplySkill( pSrc, pSkillProp, pAddSkillProp, bIgnoreProb, 0, bOnlyDmg );		// 대상에게 효과를 적용함.
							}
						}
					}
				}
			} // bApply
		}
		END_LINKMAP
	}
		
#endif // WORLDSERVER		
} // ApplySkillAround()

// 
// 폭, 높이, 길이를 설정하여 육면체 형태의 데미지 영역을 생성.
// nDmgType : AF_???
// nApplyType : OBJTYPE_PLAYER , MONSTER
// nAttackID : 스킬이나 아이템 ID
// fWidth, fHeight, fDepth : 폭, 높이, 길이.
// this == Attacker
#if __VER >= 8 // __S8_PK
void	CCtrl::ApplySkillLine( int nApplyType, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, float fWidth, float fHeight, float fDepth, bool bIgnoreProb, BOOL bControl )
#else // __VER >= 8 // __S8_PK
void	CCtrl::ApplySkillLine( int nApplyType, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, float fWidth, float fHeight, float fDepth, bool bIgnoreProb )
#endif // __VER >= 8 // __S8_PK
{
#ifdef __WORLDSERVER
	int nRange	= 4;	// 4m
	CObj* pObj;
	D3DXVECTOR3 vPos = GetPos();
	D3DXVECTOR3 vDist;

	if( pSkillProp == NULL )
		Error( "CCtrl::ApplySkillLine : pSkillProp == NULL %d", GetIndex() );
	
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
	if( fDepth <= 4.0f )		nRange = 4;
	else if( fDepth <= 8.0f )	nRange = 8;
	else if( fDepth <= 16.0f )	nRange = 16;
	else						nRange = 32;
	
	if( fDepth <= 0 )	// 범위가 0이거나 음수일수는 없다.
		Error( "CCtrl::ApplySkillLine : %d %d", GetIndex(), fDepth );

	
	BOOL	bApply = FALSE;
	BOOL	bTarget = FALSE;

	if( nApplyType & OBJTYPE_PLAYER )	// 적용대상이 플레이어인가 
	{
		FOR_LINKMAP( GetWorld(), vPos, pObj, nRange, CObj::linkPlayer, GetLayer() )
		{
			bApply = FALSE;	// 시작은 FALSE

			if( pObj->GetType() != OT_MOVER )
				Error( "ApplySkillLine : pObj가 MOVER가 아니다 %d", pObj->GetType() );

			if( GetType() == OT_MOVER )		// 공격자가 무버냐.
			{
				CMover *pAttacker = (CMover *)this;
				if( pAttacker->IsPlayer() )			// 공격자가 플레이어냐
				{
					bApply = TRUE;
#if __VER >= 8 // __S8_PK
					CMover *pDefender = (CMover *)pObj;
					if( pDefender->IsPlayer() )
					{
						if( bControl == FALSE && pAttacker->GetHitType( pDefender, bTarget, FALSE ) == HITTYPE_PK )
							bApply = FALSE;
//	#ifdef	__JHMA_VER_8_5_2			 // 8.5차 듀얼중인 두구룹이 연관되지않게 수정  World
						else if( pAttacker->m_nDuel == 1 && pDefender->m_nDuel == 1 && pAttacker->GetHitType( pDefender, bTarget, FALSE ) != HITTYPE_PVP )
							bApply = FALSE;
//	#endif // __JHMA_VER_8_5_1	
					}
#endif // __VER >= 8 // __S8_PK
				} else
				{
					// 공격자가 몬스터면
					bApply = TRUE;		// 
				}
			} else
			{
				// 공격자가 무버가 아니다.
				bApply = TRUE;
			}
			
			if( bApply && pObj != this )		// this는 공격대상에서 제외.
			{
				CMover *pTarget = (CMover *)pObj;
				if( IsValidObj( pTarget ) && pTarget->IsLive() )
				{
					// 타겟측 좌표를 역변환.
					D3DXVec3TransformCoord( &vDestLocal, &pTarget->GetPos(), &mInv );
					// 타겟측 AABB
					D3DXVECTOR3 vMin2 = vDestLocal + pTarget->m_pModel->m_vMin;
					D3DXVECTOR3 vMax2 = vDestLocal + pTarget->m_pModel->m_vMax;
					if( ::IsTouchAABB( vMin1, vMax1, vMin2, vMax2 ) )		// AABB충돌검사.
					{
						pTarget->ApplySkill( this, pSkillProp, pAddSkillProp, bIgnoreProb, 0, FALSE, bTarget );		// bTarget 타겟이 없으므로 FALSE로 넣음. PK시 선빵을 칠수없음
					}
				}
			} // bApply
		}
		END_LINKMAP
	}

	// 적용대상이 몬스터인가.
	if( nApplyType & (OBJTYPE_MONSTER | OBJTYPE_CTRL) )
	{
		FOR_LINKMAP( GetWorld(), vPos, pObj, nRange, CObj::linkDynamic, GetLayer() )
		{
			bApply = FALSE;
			if( pObj->GetType() == OT_MOVER )				// 대상이 무버면.
			{
				CMover *pTarget = (CMover *)pObj;
				if( pTarget->IsPeaceful() == FALSE )		// NPC가 아닌경우만 적용
					bApply = TRUE;
#if __VER >= 8 //	#ifdef	__JHMA_VER_8_5_1			 // 8.5차 경비병 범위스킬 공격효과 불가로 수정 World
				CMover *pAttacker = (CMover *)this;
				if( pAttacker->IsPlayer() && pAttacker->IsChaotic() == FALSE && pTarget->GetProp()->dwClass == RANK_GUARD )
					bApply = FALSE;
#endif //	#endif // __JHMA_VER_8_5_1			 // 8.5차 경비병 범위스킬 공격효과 불가로 수정 World
			} else
			// 대상이 컨트롤이면
			if( pObj->GetType() == OT_CTRL )
			{
				if( GetType() == OT_MOVER )			// 공격자가 무버일때만 적용. 컨트롤이 컨트롤에게 영향을 줄순 없다.
					bApply = TRUE;							
			}
			
			
			if( bApply )
			{
				if( pObj->GetType() == OT_MOVER )
				{
					CMover *pTarget = (CMover *)pObj;

					if( IsValidObj( pTarget ) && pTarget->IsLive() )
					{
						// 타겟측 좌표를 역변환.
						D3DXVec3TransformCoord( &vDestLocal, &pTarget->GetPos(), &mInv );
						// 타겟측 AABB
						D3DXVECTOR3 vMin2 = vDestLocal + pTarget->m_pModel->m_vMin;
						D3DXVECTOR3 vMax2 = vDestLocal + pTarget->m_pModel->m_vMax;
						if( ::IsTouchAABB( vMin1, vMax1, vMin2, vMax2 ) )		// AABB충돌검사.
						{
							((CCtrl*)pObj)->ApplySkill( this, pSkillProp, pAddSkillProp, bIgnoreProb );		// 대상에게 효과를 적용함.
						}
					}
				} else
					((CCtrl*)pObj)->ApplySkill( this, pSkillProp, pAddSkillProp, bIgnoreProb );		// 대상에게 효과를 적용함.
			}
		}
		END_LINKMAP
	}
		
#endif // WORLDSERVER		
} // ApplySkillLine()

//
// idParty 극단대상 범위 효과.
//
int		CCtrl::ApplySkillAroundTroupe( u_long idParty, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProb )
{
	BOOL bRet = FALSE;
#ifdef __WORLDSERVER
	if( GetType() != OT_MOVER )		return bRet;	// 무버가 아닌게 극단스킬을 썼을리 없다.
	if( ((CMover *)this)->IsPlayer() == FALSE )		return bRet;	// 플레이어가 아닌게 극단스킬을 썼을리 없다.

	CParty *pParty	= g_PartyMng.GetParty( idParty );
	if( pParty )
	{
		int i;
		CUser *pMember;
		D3DXVECTOR3	vDist;
		float	fDist;
		float	fMaxDistSq = (float)pAddSkillProp->dwSkillRange;
		fMaxDistSq *= fMaxDistSq;
		FLOAT fTargetRatio = 0.0f;
		if( pSkillProp->dwSpellRegion == SRO_REGION )
			fTargetRatio = 1.0f;
		for( i = 0; i < pParty->m_nSizeofMember; i++ )
		{
			pMember	= g_UserMng.GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
			if( IsValidObj( pMember ) && !pMember->IsDie()
				&& GetWorld() && pMember->GetWorld() && GetWorld()->GetID() == pMember->GetWorld()->GetID()
				&& GetLayer() == pMember->GetLayer() )	// mirchang_100217 범위 검사 오류 수정
			{
				if( pMember == this && fTargetRatio == 0.0f )	// 타겟데미지 비율이 0이면 자기자신(타겟)은 데미지를 주지않음.
				{
				} else
				{
					vDist = pMember->GetPos() - GetPos();
					fDist = D3DXVec3LengthSq( &vDist );			// 파티 멤버와 시전자와의 거리.
					if( fDist < fMaxDistSq )					// xx미터보다 작으면
					{
						pMember->ApplySkill( this, pSkillProp, pAddSkillProp, bIgnoreProb );	// 멤버 각각에게 효과 적용.
						bRet = TRUE;
					}
				}
			}
		}
	}
#endif // WorldServer
	return bRet;
} // ApplySkillAroundTroupe

//
// ApplySkill 내부에서 스킬의 하드코딩 처리부.
// this : Target
// pSrc : 시전자.
//
BOOL CCtrl::ApplySkillHardCoding( CCtrl *pSrc, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp )
{
#ifdef __WORLDSERVER
	switch( pSkillProp->dwID )
	{
	case SI_ASS_HEAL_RESURRECTION:		// 부활
		if( GetType() == OT_MOVER )
		{
			CMover *pTarget = (CMover *)this;
			if( pTarget->IsPlayer() && pTarget->IsDie() )	// 플레이어이고 타겟이 죽어있을때
			{
				if( pTarget->m_pActMover->GetDmgState() == OBJSTA_RESURRECTION )
				{
					return FALSE;		// 이미 부활중인 사람에게 쓴거라면 취소.
				} else
				{
					g_dpDBClient.SendLogLevelUp( pTarget, 10 );	// 부활 스킬 로그

					if( ((CUser *)this)->m_Resurrection_Data.bUseing != TRUE )
					{
						((CUser *)this)->AddResurrectionMessage();
					
						((CUser *)this)->m_Resurrection_Data.bUseing        = TRUE;
						((CUser *)this)->m_Resurrection_Data.dwPlayerID     = ((CMover*)pSrc)->m_idPlayer;
						((CUser *)this)->m_Resurrection_Data.pSkillProp     = pSkillProp;
						((CUser *)this)->m_Resurrection_Data.pAddSkillProp  = pAddSkillProp;
					}

					break;
				}

				// 부활때 DST_RECOVERY_EXP가 있으면 겸치가 조금 깎임.
				if( pAddSkillProp->dwDestParam2 == DST_RECOVERY_EXP )
					pTarget->SubDieDecExp(TRUE, pAddSkillProp->nAdjParamVal2 );	// 부활이 되면서 겸치가 조금 깎임.
			}
		}
		break;
	} // switch
#endif // WorldServer
	return TRUE;
}

#ifdef __CLIENT
void CCtrl::CreateYoyoSkill( CSfx* pSfx, CCtrl *pTarget, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp )
{
	ItemProp* pItemProp = ((CMover *)this)->GetActiveHandItemProp();
	
	if(pItemProp)
	{
		D3DXVECTOR3 vPos;
		D3DXVECTOR3 vLocal;
		D3DXVECTOR3 vPosSrc   = GetPos() + D3DXVECTOR3( 0.0f, 1.0f, 0.0f ); // 발사 지점을 임의로 올려준다. 땜빵 
		D3DXVECTOR3 vPosDest  = pTarget->GetPos() + D3DXVECTOR3( 0.0f, 1.0f, 0.0f ); // 목표 지점을 임의로 올려준다. 땜빵 
		
		CModelObject *pModel = (CModelObject *)m_pModel;
		
		int nSkill = pSkillProp->dwID;
		
		switch( nSkill )
		{
		case SI_ACR_SUP_SLOWSTEP:
		case SI_JST_YOYO_HITOFPENYA:
		case SI_JST_YOYO_VATALSTAB:
			{
				pModel->GetHandPos( &vPos, PARTS_RWEAPON, GetMatrixWorld() );
				pSfx = CreateSfxYoYo( D3DDEVICE, pItemProp->dwSfxObj2, vPos, GetId(), vPosDest );	
				((CSfxItemYoyoAtk*)pSfx)->MakePath(PARTS_RWEAPON);
			}
			break;
		case SI_ACR_YOYO_DEADLYSWING:
		case SI_ACR_YOYO_CROSSLINE:
			{
				pModel->GetHandPos( &vPos, PARTS_RWEAPON, GetMatrixWorld() );
				pSfx = CreateSfxYoYo( D3DDEVICE, pItemProp->dwSfxObj2, vPos, GetId(), vPosDest );	
				((CSfxItemYoyoAtk*)pSfx)->MakePath(PARTS_RWEAPON);

				pModel->GetHandPos( &vPos, PARTS_LWEAPON, GetMatrixWorld() );
				pSfx = CreateSfxYoYo( D3DDEVICE, pItemProp->dwSfxObj2, vPos, GetId(), vPosDest );	
				((CSfxItemYoyoAtk*)pSfx)->MakePath(PARTS_LWEAPON);
			}
			break;							
		}
	}

}
#endif //__CLIENT

//
// 스킬을 썼을때 클라에서 생성해야할 sfx 처리
//
void CCtrl::CreateSkillSfx( CCtrl *pTarget, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp )
{
#ifdef __CLIENT
		
	{
		// 발사체 형태가 아니다.
		CSfx *pSfx = NULL;
		if( (int)pAddSkillProp->dwSkillTime > 0 )	// 지속시간이 있는 스킬은
		{
			if( pSkillProp->dwSfxObj4 == NULL_ID )		// 이펙이 유지되는 스킬이 아닐경우에만.
			{
				if( pSkillProp->dwExeTarget == EXT_MELEEATK )
				{
					D3DXVECTOR3 vLocal;
					
					if( pSkillProp->dwLinkKind == IK3_YOYO )
					{
						vLocal = pTarget->GetPos();
						vLocal.y += 1.0f;
					}
					else
					{
						AngleToVectorXZ( &vLocal, GetAngle(), 1.3f );	// 때리는방향 1미터앞.
						vLocal += GetPos();
						vLocal.y += 1.0f;
					}

					pSfx = CreateSfx( D3DDEVICE, pSkillProp->dwSfxObj3, GetPos(), GetId(), vLocal, pTarget->GetId(), 0 );
					
					if( pSkillProp->dwSfxObj5 != NULL_ID )		// 시전자에게 발생
						pSfx = CreateSfx( D3DDEVICE, pSkillProp->dwSfxObj5, GetPos(), GetId(), this->GetPos(), this->GetId(), 0 );  // 5번 이펙트를 출력함.
				} else
				{
					if( pSkillProp->dwSfxObj3 != NULL_ID )		// 타겟에게 발생
						pSfx = CreateSfx( D3DDEVICE, pSkillProp->dwSfxObj3, GetPos(), GetId(), pTarget->GetPos(), pTarget->GetId(), 0 );  // 3번 이펙트를 출력함.
					if( pSkillProp->dwSfxObj5 != NULL_ID )		// 시전자에게 발생
						pSfx = CreateSfx( D3DDEVICE, pSkillProp->dwSfxObj5, GetPos(), GetId(), this->GetPos(), this->GetId(), 0 );  // 5번 이펙트를 출력함.
				}

				if( pSkillProp->dwLinkKind == IK3_YOYO )  //요요는 아이템프로퍼티참조하여 이펙트 생성(예외처리)
				{
				#ifdef __CLIENT
					CreateYoyoSkill( pSfx, pTarget, pSkillProp, pAddSkillProp );
				#endif
				}
			} else
			{
				if( GetType() == OT_MOVER )
				{
					if( pSkillProp->dwSkillType == NULL_ID )	// NULL_ID면 아이템들
					{
						if( ((CMover *)this)->HasBuff( BUFF_ITEM, (WORD)( pSkillProp->dwID ) ) == FALSE )	// 이미 실행되고 있지 않을때만 이펙 출력
							CreateSfx( D3DDEVICE, pSkillProp->dwSfxObj4, this->GetPos(), this->GetId(), pTarget->GetPos(), pTarget->GetId(), (int)(pAddSkillProp->dwSkillTime / 1000.0f) );
					} else
					{
						// 이미 실행되고 있지 않을때만 이펙 출력
						if( ((CMover *)this)->HasBuff( BUFF_SKILL, (WORD)( pSkillProp->dwID ) ) == FALSE
							&& pSkillProp->dwID != SI_MER_SHIELD_PROTECTION && pSkillProp->dwID != SI_MER_SHIELD_PANBARRIER )	// 091022 mirchang - 여기서 먼저 생성 후 IBuff.Process에서 sfx를 다시 생성하는 문제 발생. 땜빵.
							CreateSfx( D3DDEVICE, pSkillProp->dwSfxObj4, this->GetPos(), this->GetId(), pTarget->GetPos(), pTarget->GetId(), (int)(pAddSkillProp->dwSkillTime / 1000.0f) );
					}
				}
			}
		} else
		{	// 지속시간 없는 스킬
			if( pSkillProp->dwExeTarget == EXT_MELEEATK )	// 근접공격 스킬의 타격 이펙트.
			{
				if( pSkillProp->dwSfxObj3 != NULL_ID )	
				{
					D3DXVECTOR3 vLocal;

					if( pSkillProp->dwLinkKind == IK3_YOYO )
					{
						vLocal = pTarget->GetPos();
						vLocal.y += 1.0f;
					}
					else
					{
						AngleToVectorXZ( &vLocal, GetAngle(), 1.3f );	// 때리는방향 1미터앞.
						vLocal += GetPos();		// GetPos()를 this에서 pTarget으로 바꿨다. 2006/6/20 xuzhu.
						vLocal.y += 1.0f;
					}

					pSfx = CreateSfx( D3DDEVICE, pSkillProp->dwSfxObj3, vLocal, GetId(), vLocal, pTarget->GetId(), 0 );	// 2006/6/20 xuzhu 가 바꿈.
				}
			} else
			{
				if( pSkillProp->dwSfxObj3 != NULL_ID )
				{
					D3DXVECTOR3 vPos = pTarget->GetPos();
					vPos.y += 1.0f;
					pSfx = CreateSfx( D3DDEVICE, pSkillProp->dwSfxObj3, GetPos(), GetId(), vPos, pTarget->GetId(), 0 );
				}
			}

			if( pSkillProp->dwLinkKind == IK3_YOYO )  //요요는 아이템프로퍼티참조하여 이펙트 생성(예외처리)
			{
			#ifdef __CLIENT
				CreateYoyoSkill( pSfx, pTarget, pSkillProp, pAddSkillProp );
			#endif
			}
			else
			if( pSkillProp->dwSfxObj5 != NULL_ID )		// 시전자에게 발생
				pSfx = CreateSfx( D3DDEVICE, pSkillProp->dwSfxObj5, GetPos(), GetId(), this->GetPos(), this->GetId(), 0 );  // 5번 이펙트는 시전자 자신에게 출력
		}
		if( pSfx )
		{
			D3DXVECTOR3 vPos = GetPos();
			PLAYSND( pSkillProp->dwSndAttack1, &vPos );		// 발사 효과음.
		}
			
		//dwSkillRange
		if( pSkillProp->dwID == SI_GEN_FLASH && pSfx )
		{
			FLOAT fRange = 1.5f;
			pSfx->SetScale(  D3DXVECTOR3(fRange, fRange, fRange) );
		}
	}
	
#endif // Client
} // CreateSkillSfx


// ApplySkill 내부에서 데미지 주는부분만 빠짐.
int		CCtrl::ApplyDamage( CCtrl *pSrc, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, int nParam, BOOL bTarget ) 
{
	int nDamage = 0;
	// 데미지류 스킬의 데미지 전달.
	switch( pSkillProp->dwExeTarget )
	{
	case EXT_MELEEATK:		// 근접스킬데미지공격
	case EXT_MAGICATK:			// 즉시마법데미지공격
	case EXT_MAGICATKSHOT:
		{
			if( pSkillProp->dwID == SI_MAG_FIRE_HOTAIR )	// 핫에어 땜빵처리.
			{
				nDamage = SendDamage( AF_MAGICSKILL, pSrc->GetId(), nParam, bTarget );
			} else
			{
				int nAtkFlag = 0;
				if( pSkillProp->dwSkillType == KT_MAGIC )
					nAtkFlag = AF_MAGICSKILL;
				else
					nAtkFlag = AF_MELEESKILL;
				
				if( pSkillProp->dwComboStyle == CT_FINISH )		nDamage = SendDamageForce( nAtkFlag, pSrc->GetId(), (pSkillProp->dwID << 16) | pAddSkillProp->dwSkillLvl, bTarget );
				else											nDamage = SendDamage( nAtkFlag, pSrc->GetId(), (pSkillProp->dwID << 16) | pAddSkillProp->dwSkillLvl, bTarget );
			}
		}
		break;
	}

	return nDamage;
}

// 적용가능한 스킬이냐?
BOOL CCtrl::IsPossibleApplySkill( CCtrl *pSrc, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp )
{
	return TRUE;
}

// pSkillProp을 this에 적용한다.  데미지가 될수도 있고. 상태변화가 될수도있다.
// pSrc는 시전자(혹은 어태커)
// pAddSkillProp은 NULL일수도 있음을 주의.
void	CCtrl::ApplySkill( CCtrl *pSrc, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProb, int nParam, BOOL bOnlyDmg, BOOL bTarget )
{
#ifdef __PK_PVP_SKILL_REGION
	if( pSrc->GetType() == OT_MOVER && GetType() == OT_MOVER )
	{
		if( ((CMover *)pSrc)->IsPlayer() && ((CMover *)this)->IsPlayer() && pSkillProp->dwSkillType != NULL_ID )
		{
			DWORD dwPKRegionAttr = ((CMover *)this)->GetPKPVPRegionAttr();
			int nAttackerPK = ((CMover *)pSrc)->IsPKPVPInspectionBase( dwPKRegionAttr, FALSE );
			int nDefenDerPK = ((CMover *)this)->IsPKPVPInspectionBase( dwPKRegionAttr, FALSE );
			if( nAttackerPK == 1 || nDefenDerPK == 1 )
			{
				if( pSkillProp->nEvildoing < 0 )
				{
					CMover* pAttacker = (CMover*)pSrc;
					CMover* pDefender = (CMover*)this;
					if( !(pAttacker->IsPVPTarget( pDefender ) || 
						pAttacker->IsWarTarget( pDefender ) ||
						pAttacker->IsGuildCombatTarget( pDefender )
#ifdef __JEFF_11_4
						|| pAttacker->IsArenaTarget( pDefender )
#endif	// __JEFF_11_4
						) )
							return;
					
					//if( !((CMover *)pSrc)->IsPVPTarget((CMover *)this) )
					//	return;
				}
			}
		}
	}
#endif // __PK_PVP_SKILL_REGION
	// 적용가능한 스킬이냐?
	if( IsPossibleApplySkill( pSrc, pSkillProp, pAddSkillProp ) == FALSE )
		return;

	// 데미지속성이 있을경우 그것을 처리.
	int nDamage = ApplyDamage( pSrc, pSkillProp, pAddSkillProp, nParam, bTarget );
	if( bOnlyDmg == TRUE )		
		return;

#if __VER >= 10	// __AI_0711
	// 준보스 몬스터는 모든 디버프 면역
	if( GetType() == OT_MOVER )
	{
		if( ( (CMover*)this )->IsRank( RANK_MIDBOSS ) )
		{
			// 면역 이펙트
			return;
		}
	}
#endif	// __AI_0711

#if defined(__WORLDSERVER)
	if( this != pSrc && pSrc->GetType() == OT_MOVER && GetType() == OT_MOVER )		// 광역 디버프 중 타겟이아니면 skip
	{
		if( ((CMover *)this)->IsDie() == FALSE )
		{
#if __VER >= 8 // __S8_PK
			if( pSkillProp->nEvildoing < 0 )													// 나쁜마법은
				if( ((CMover *)pSrc)->GetHitType2(((CMover *)this), bTarget, FALSE ) == HITTYPE_FAIL )  	// 평화적인상대에게
					return;																			// 적용안됨
#ifdef __JEFF_11_4
			CWorld* pWorld	= pSrc->GetWorld();
			if(  pWorld && pWorld->IsArena() == FALSE )		// 아레나가 아니면,
#endif	// __JEFF_11_4
			{
				if( pSkillProp->nEvildoing > 0 )													// 좋은 마법은
					if( ((CMover *)pSrc)->GetHitType2(((CMover *)this), bTarget, TRUE ) != HITTYPE_FAIL )  	// 적대적인상대에게
						return;																			// 적용안됨
			}
#else // __VER >= 8 // __S8_PK
			if( pSkillProp->nEvildoing < 0 )													// 나쁜마법은
				if( ((CMover *)pSrc)->GetHitType2(((CMover *)this), bTarget ) == HITTYPE_FAIL )  	// 평화적인상대에게
					return;																			// 적용안됨
			if( pSkillProp->nEvildoing > 0 )													// 좋은 마법은
				if( ((CMover *)pSrc)->GetHitType2(((CMover *)this), bTarget ) != HITTYPE_FAIL )  	// 적대적인상대에게
					return;																			// 적용안됨
#endif // __VER >= 8 // __S8_PK
		}
		else
		{
			if( pSkillProp->dwExeTarget != EXT_MAGICATKSHOT )
				if( pSkillProp->dwSfxObj2 != NULL_ID )
					g_UserMng.AddCreateSfxObj( this, pSkillProp->dwSfxObj2 );
			
			if( pSkillProp->nEvildoing < 0 )
				return;
		}
	}
/*#ifdef __INSTANCE_AGGRO_SYSTEM
	CreateAggro( pSrc, pSkillProp, pAddSkillProp );
#endif // __INSTANCE_AGGRO_SYSTEM*/
#endif //defined(__WORLDSERVER)

	int nProb	= pSkillProp->nProbability;
	if( pAddSkillProp )
	{
		nProb	= pAddSkillProp->nProbability;
#if __VER >= 9	// __SKILL_0706
		BOOL bPVP	= ( pSrc->GetType() == OT_MOVER && ( (CMover *)pSrc )->IsPlayer() && GetType() == OT_MOVER && ( (CMover *)this )->IsPlayer() );
		if( bPVP )
			nProb	= pAddSkillProp->nProbabilityPVP;
#endif	// __SKILL_0706
	}

	if( bIgnoreProb )	// 확률무시옵이 있으면 무조건 100%
		nProb = NULL_ID;
	
	if( nProb == NULL_ID || (int)( xRandom(100) ) < nProb  )		// 적용 확률, =로 되어 있으면 100% 적용
	{
		if( GetType() == OT_MOVER )		// 타겟이 무버일때만 지속시간 처리
		{
			int nSkillTime = 0;
			if( pAddSkillProp )		
				nSkillTime = (int)pAddSkillProp->dwSkillTime;
			else					
				nSkillTime = (int)pSkillProp->dwSkillTime;

			BOOL bNew = TRUE;	// 스킬이 첨 적용됐는가.
#if __VER >= 8 //__Y_FLAG_SKILL_BUFF
			if( nSkillTime >= 0 )	// 지속시간이 있는 스킬(프로텍션 등)
#else //__Y_FLAG_SKILL_BUFF
			if( nSkillTime > 0 )	// 지속시간이 있는 스킬(프로텍션 등)					
#endif //__Y_FLAG_SKILL_BUFF
			{
				DWORD dwTime1 = 0, dwTime2 = 0;
				if( pSkillProp->dwReferTarget1 == RT_TIME || pSkillProp->dwReferTarget2 == RT_TIME )
				{
					if( pSrc->GetType() == OT_MOVER )		// 시전자가 무버일때만.
						((CMover *)pSrc)->SubReferTime( &dwTime1, &dwTime2, pSkillProp, pAddSkillProp );
				}

				WORD wType;
				if( pSkillProp->dwSkillType == NULL_ID )	
					wType = BUFF_ITEM;
				else										
					wType = BUFF_SKILL;

				// pTarget은 this가 될수도있고 타인이 될수도 있다.
			#if defined(__WORLDSERVER)
				int nLevel = 0;
				if( pAddSkillProp )		
					nLevel = pAddSkillProp->dwSkillLvl;
				else					
					nLevel = 0;

				// dwSkillTime이 스킬자체의 지속시간으로도 쓰이고 부가효과의 지속시간으로도 쓰이기때문에 이런구조는 맞지 않다
				// 이대로라면 버닝필드도 버프처럼 걸려버린다.
				// bNew = ((CMover *)this)->m_SkillState.Set( wType, pSkillProp->dwID, nLevel, nSkillTime + (int)dwTime1 + (int)dwTime2 );	
				if( pSkillProp->dwID == SI_ELE_FIRE_BURINGFIELD )
					bNew = FALSE;	//  버닝필드는 예외처리 
				else
#ifdef __PVPDEBUFSKILL
					bNew	= static_cast<CMover*>( this )->AddBuff( wType, (WORD)( pSkillProp->dwID ), nLevel, nSkillTime + (int)dwTime1 + (int)dwTime2, pSrc->GetId() );
#else // __PVPDEBUFSKILL
					bNew = ((CMover *)this)->m_SkillState.Set( wType, pSkillProp->dwID, nLevel, nSkillTime + (int)dwTime1 + (int)dwTime2 );	
#endif // __PVPDEBUFSKILL

				
			#elif defined(__CLIENT)
				bNew = ((CMover *)this)->HasBuff( wType, (WORD)( pSkillProp->dwID ) ) ? FALSE : TRUE;
			#endif // Client
			}
			
			// 최초 적용된 스킬에만
			if( bNew )	// 이미 걸려있는 상태에서 또 쓰면 bNew는 FALSE가 된다.
			{
			#ifdef __WORLDSERVER
				// 스킬중 예외처리 해야할것을 처리함.
				if( ApplySkillHardCoding( pSrc, pSkillProp, pAddSkillProp ) == FALSE )
					return;

				// 부활스킬셋팅이 되어있다면 아랫것들 무시
				if( ((CMover *)this)->m_Resurrection_Data.bUseing )
					return;

				// 데미지도 주면서 파라메터도 변하는 스킬이 있기땜에 데미지랑 따로 처리.
				switch( pSkillProp->dwExeTarget )
				{
				case EXT_SELFCHGPARAMET:		// 시전자 자신.
					if( pSrc->GetType() == OT_MOVER )	// 타겟(여기선 타겟이 시전자)이 무버일때만...
						((CMover *)pSrc)->ApplyParam( (CMover *)pSrc, pSkillProp, pAddSkillProp, TRUE, nDamage );
					break;
				case EXT_OBJCHGPARAMET:
				case EXT_MAGIC:
				case EXT_ANOTHER:
				case EXT_ANOTHERWITH:
				default:
					if( this->GetType() == OT_MOVER )	// 타겟이 무버일때만.
						((CMover *)this)->ApplyParam( (CMover *)pSrc, pSkillProp, pAddSkillProp, TRUE , nDamage );
					break;
				}

				if( this->GetType() == OT_MOVER )
				{
					// 밀리는 양이 있는가?
					CMover *pTarget = (CMover *)this;
					DWORD dwDmgShift;
					if( pAddSkillProp )
						dwDmgShift = pAddSkillProp->dwDmgShift;
					else
						dwDmgShift = pSkillProp->dwDmgShift;

					if( dwDmgShift != NULL_ID && pTarget->GetProp()->dwClass != RANK_SUPER && pTarget->GetProp()->dwClass != RANK_MIDBOSS )
					{
						FLOAT fAngle = GetDegree( pTarget->GetPos(), pSrc->GetPos() );	// 시전자와 타겟의 각도를 구함
						AngleToVectorXZ( &pTarget->m_pActMover->m_vDeltaE, fAngle, 0.85f );
						g_UserMng.AddPushPower( pTarget, pTarget->GetPos(), pTarget->GetAngle(), fAngle, 0.85f );
					}
					
					// 상대방을 끌어당긴다
					if( pSkillProp->dwID == SI_ACR_YOYO_PULLING
#ifdef __3RD_LEGEND16	
						|| pSkillProp->dwID == SI_LOD_SUP_PULLING
#endif // __3RD_LEGEND16
						)
					{
						if( pAddSkillProp && pAddSkillProp->nDestData1[2] != NULL_ID )
						{
							// 당겨질 거리
							FLOAT fPullingLen = (FLOAT)pAddSkillProp->nDestData1[2] * 0.001f; 
							
							// 타겟과의 거리
							D3DXVECTOR3 v3Len = pTarget->GetPos() - pSrc->GetPos();
							FLOAT fTotalLen = D3DXVec3LengthSq( &v3Len ) * 0.1f;
							{
								if( fTotalLen > ((CMover*)this)->GetAttackRange( AR_HRANGE ) )
									fTotalLen = ((CMover*)this)->GetAttackRange( AR_HRANGE );

								// 거리에 따라 댕겨지는 힘이 다르게 적용됨
								FLOAT fDest = fTotalLen / ((CMover*)this)->GetAttackRange( AR_HRANGE );
								{
									// 실제땡겨질 거리 계산
									FLOAT fAngle = GetDegree( pTarget->GetPos(), pSrc->GetPos() );	// 시전자와 타겟의 각도를 구함
									AngleToVectorXZ( &pTarget->m_pActMover->m_vDeltaE, fAngle, fPullingLen * -fDest);
									g_UserMng.AddPushPower( pTarget, pTarget->GetPos(), pTarget->GetAngle(), fAngle, fPullingLen * -fDest );
								}
							}
						}
					}
				}
			#endif // __WORLDSERVER
			} // if( bNew )

		#ifdef __WORLDSERVER
			if( pSkillProp->dwID == SI_GEN_FLASH && GetType() == OT_MOVER && ((CMover *)this)->IsPlayer() )
			{
				((CUser *)this)->AddCommonSkill( pSkillProp->dwID, pAddSkillProp->dwSkillLvl );
			}
		#endif // __WORLDSERVER
		
		}	// this == OT_MOVER
	}	// if( nProb == NULL_ID || xRandom(100) < nProb  )
#ifdef __WORLDSERVER
	#if __VER >= 11 // __MA_VER11_06				// 확율스킬 효과수정 world,neuz
	else
	{
		if(pSkillProp->dwID == SI_PSY_HERO_STONE )
			g_UserMng.AddRemoveSfxObj( this, pSkillProp->dwSfxObj4 );
	}
	#endif // __MA_VER11_06				// 확율스킬 효과수정 world,neuz
#endif	//__WORLDSERVER
	#ifdef __WORLDSERVER
		switch( pSkillProp->dwSpellRegion )
		{
#ifdef __YDEBUG
		case SRO_DIRECT:
#endif //__YDEBUG
		case SRO_AROUND:
		case SRO_REGION:	// 범위공격일땐 여기서 타겟들 각각에 sfxobj3를 뿌려줌.
			if( pSkillProp->dwExeTarget != EXT_MAGICATKSHOT )
				if( pSkillProp->dwSfxObj2 != NULL_ID )
					g_UserMng.AddCreateSfxObj( this, pSkillProp->dwSfxObj2 );
			break;
		}

		// 예외처리: 강탈스킬 - 페냐를 훔치면서 데미지를 준다
		if( pSkillProp->dwID == SI_ACR_YOYO_SNITCH )
		{
			// pSrc - 공격자 플레이어, this - 피공격자 몬스터 
			// 피공격자 잔액이 0이 아니면 강탈 한다.
			CUser* pUser = (CUser*)pSrc;
			CUser* pDest = (CUser*)this;
			if( pDest->GetType() == OT_MOVER && pDest->GetGold() > 0 )
			{
				int nPlus = (pDest->GetLevel()*20) - (pUser->GetLevel()*10);
				
				if( pUser->GetLevel() <= 20 )
				{
				}
				else
				if( pUser->GetLevel() <= 30 )
				{
					nPlus	= (int)( nPlus * 0.7f );
				}
				else
				if( pUser->GetLevel() <= 40 )
				{
					nPlus	= (int)( nPlus * 0.5f );
				}
				else
				{
					nPlus	= (int)( nPlus * 0.3f );
				}
		
				if( nPlus > 0 )
				{				
					nPlus = min( nPlus, pDest->GetGold() );		// 피공격자 돈 보다는 많지 않게 
					pUser->AddGold( nPlus );
					pDest->AddGold( -nPlus );

					pUser->AddGoldText( nPlus );
				}
			}		
		}
	#endif	//__WORLDSERVER
} // ApplySkill
/*#ifdef __INSTANCE_AGGRO_SYSTEM
#ifdef __WORLDSERVER
void	CCtrl::CreateAggro( CCtrl *pSrc, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp )
{
	int nRange	= 48;	// scan range
	float fDistSq;

	CMover *pMover = (CMover *)pSrc; // Player
	CMover *pTarget = (CMover *)this; // target
	CMover *pCastTarget; // Monster
	MoverProp *pTargetProp = pTarget->GetProp();

	CObj* pObj;
	D3DXVECTOR3 vPos = pMover->GetPos();
	D3DXVECTOR3 vDist;

	if( pMover->IsPlayer() && pTarget != pMover ) // only player can generate aggro
	{
		if( pSkillProp->IsAlwaysAggro() )
			pTarget->PostAIMsg( AIMSG_DAMAGE, pMover->GetId(), 0, 1 );
		else
		if( pSkillProp->IsDebuff() && !pTarget->IsPlayer() && pTarget->GetProp()->dwAI >= AII_AGGRO_NORMAL )
			pTarget->PostAIMsg( AIMSG_DAMAGE, pMover->GetId(), pSkillProp->dwID, 2 );
        else
		if( pSkillProp->IsHeal() && pTarget->IsPlayer() )
		{
			FOR_LINKMAP( GetWorld(), vPos, pObj, nRange, CObj::linkDynamic, GetLayer() ) // get everyone in range
			{ 
				if( pObj->GetType() == OT_MOVER && ((CMover *)pObj)->IsPeaceful() == FALSE && !((CMover *)pObj)->IsPlayer() ) // is mover, not npc and not player
				{
					vDist = pTarget->GetPos() - vPos;
					fDistSq = D3DXVec3LengthSq( &vDist );
					if( fDistSq < 48.0f * 48.0f )
					{
						pCastTarget = (CMover *)pObj;
						MoverProp *pCastTargetProp = pCastTarget->GetProp();
						if ( pCastTargetProp->dwAI >= AII_AGGRO_NORMAL ) // is instance monster
						{
							if( IsValidObj( (CObj*)pCastTarget ) && pCastTarget->IsLive() )
							{
								pCastTarget->PostAIMsg( AIMSG_SKILLSUPPORT, pMover->GetId(), pSkillProp->dwID, pTarget->GetId() );
							}
						}
					}
				}
			}
			END_LINKMAP
		}else
		if( pSkillProp->IsBuff() && pTarget->IsPlayer() )
		{
			FOR_LINKMAP( GetWorld(), vPos, pObj, nRange, CObj::linkDynamic, GetLayer() ) // get everyone in range
			{ 
				if( pObj->GetType() == OT_MOVER && ((CMover *)pObj)->IsPeaceful() == FALSE && !((CMover *)pObj)->IsPlayer() ) // is mover, not npc and not player
				{
					vDist = pTarget->GetPos() - vPos;
					fDistSq = D3DXVec3LengthSq( &vDist );
					if( fDistSq < 48.0f * 48.0f )
					{
						pCastTarget = (CMover *)pObj;
						MoverProp *pCastTargetProp = pCastTarget->GetProp();
						if ( pCastTargetProp->dwAI >= AII_AGGRO_NORMAL ) // is instance monster
						{
							if( IsValidObj( (CObj*)pCastTarget ) && pCastTarget->IsLive() )
							{
								pCastTarget->PostAIMsg( AIMSG_SKILLSUPPORT, pMover->GetId(), pSkillProp->dwID, pTarget->GetId() );
							}
						}
					}
				}
			}
			END_LINKMAP
		}
	}
}

#endif // __WORLDSERVER
#endif // __INSTANCE_AGGRO_SYSTEM*/

// 발사형태의 스킬의 발사체를 생성하는 부분
// 이 발사체가 타겟에게 맞았을때 DoApplySkill()이 발동된다.
int		CCtrl::ShootSkill( CCtrl *pTarget, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp )
{
#ifdef __CLIENT
	// 발사형태가 아니면 리턴
	if( pSkillProp->dwExeTarget != EXT_MAGICATKSHOT 
		&& pSkillProp->dwExeTarget != EXT_MAGICSHOT 
		)	// 발사체 sfx 생성.
		return 0;

	DWORD dwShootObj;
	{
		dwShootObj = pSkillProp->dwSfxObj2;
	}

	if( dwShootObj != NULL_ID )
	{
		D3DXVECTOR3 vPos = GetPos();
		
		if( ((CMover*)this)->IsPlayer() )		
		{
			// 보우에서 나가는 화살은 몇가지 셋팅을 해주자
			if( pSkillProp->dwLinkKind == IK3_BOW )
			{
				// 화살은 왼손에 연결...
				CModelObject *pModel = (CModelObject *)m_pModel;
				pModel->GetHandPos( &vPos, PARTS_LWEAPON, GetMatrixWorld() );		// 주먹 월드좌표 구함		
				vPos.y -=	1.0f;
			}
		}
		
		CSfxShoot *pShootSfx = CreateShootSfx( D3DDEVICE, dwShootObj, vPos, GetId(), D3DXVECTOR3(0,0,0), pTarget->GetId() );
		if( pShootSfx )
		{		
			D3DXVECTOR3 vPos = GetPos();
			PLAYSND( pSkillProp->dwSndAttack1, &vPos );		// 발사 효과음.
			
			if( GetType() == OT_MOVER )
			{
				if( ((CMover *)this)->IsActiveMover() )		// g_pPlayer가 쏠때만 생성해서 전송.
				{
					int idSfxHit = ++(((CMover *)this)->m_idSfxHit);	// 클라에서 발사체가 생성된 즉시 ID를 만들고 그것을 서버로 보냄.
					int		nMaxDmgCnt = 1;
					if( pSkillProp->tmContinuousPain != NULL_ID )		// 지속스킬이 있는 데미지 형태면
						nMaxDmgCnt = (pAddSkillProp->dwSkillTime / pAddSkillProp->dwPainTime) + 1;		// 몇번 데미지를 먹냐.
					
					if( pSkillProp->dwSkillType == KT_SKILL )
						g_DPlay.SendSfxID( pTarget->GetId(), idSfxHit, AF_MELEESKILL, NULL_ID, nMaxDmgCnt );
					else
						g_DPlay.SendSfxID( pTarget->GetId(), idSfxHit, AF_MAGICSKILL, NULL_ID, nMaxDmgCnt );
					pShootSfx->m_idSfxHit	= idSfxHit;
				}
			}
			pShootSfx->SetSkill( pSkillProp->dwID );
			pShootSfx->m_nMagicPower	= pAddSkillProp->dwSkillLvl;	// 스킬레벨은 MagicPower변수로 같이써도 될거 같다.
			pShootSfx->m_dwSfxHit = pSkillProp->dwSfxObj3;	// 폭발 이펙트 등록.

		}
		// 지속시간동안 이펙트도 유지되어야 하는것은 CSkillInfluence::Process()에서 처리한다.
	} else
		Error( "ShootSkill MAGICATKSHOT : %s dwSfxObj2가 지정되지 않음", pSkillProp->szName );
#endif // CLIENT
	return 1;
}


// 범위적용시 타겟의 종류에 따라 주변 타겟의 종류를 선택함.
int		CCtrl::TargetSelecter( CCtrl *pTarget )
{
	int nApplyType = 0;
	CCtrl *pSrc = this;

	if( pTarget->GetType() == OT_MOVER )	// 타겟이 무버일때
	{
		nApplyType = OBJTYPE_PLAYER | OBJTYPE_MONSTER;	// 주위 플레이어들/몬스터에게 적용
	}
	else
	{
		// 타겟이 컨트롤일땐
		if( pSrc->GetType() == OT_MOVER )	// 공격자가 무버냐
		{
			if( ((CMover *)pSrc)->IsPlayer() )		// 공격자가 플레이어면
				nApplyType = OBJTYPE_PLAYER | OBJTYPE_CTRL | OBJTYPE_MONSTER;	// 주위 몬스터와 컨트롤에게 적용.
		}
		else
		{	// 공격자가 컨트롤이냐
			nApplyType = OBJTYPE_PLAYER;	// 플레이어에게 적용
		}
	}

	return nApplyType;
}

#if __VER >= 8 // __S8_PK
int		CCtrl::DoApplySkill( CCtrl *pTarget, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProb, int nParam, BOOL bOnlyDmg, BOOL bControl )
#else // __VER >= 8 // __S8_PK
int		CCtrl::DoApplySkill( CCtrl *pTarget, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProb, int nParam, BOOL bOnlyDmg )
#endif // __VER >= 8 // __S8_PK
{
#if __VER >= 8 // __S8_PK
	int nResult = DoApplySkillEx( pTarget, pSkillProp, pAddSkillProp, bIgnoreProb, nParam, bOnlyDmg, bControl );
#else // __VER >= 8 // __S8_PK
	int nResult = DoApplySkillEx( pTarget, pSkillProp, pAddSkillProp, bIgnoreProb, nParam, bOnlyDmg );
#endif // __VER >= 8 // __S8_PK
	if( nResult == 1 )
	{
	#if defined(__WORLDSERVER)	
		if( this != pTarget && GetType() == OT_MOVER && pTarget->GetType() == OT_MOVER )
		{
			if( pSkillProp->nEvildoing < 0 )	// 나쁜마법을 쓰면 공격과 같다고 간주한다. 
				((CMover*)pTarget)->OnAttacked( (CMover*)this, 0, TRUE, 0 );	// TRUE는 타겟이다.
		}
	#endif 
	}

	return nResult;
}

// 
// 지정된 스킬의 효과를 pTarget에게 적용한다.
// 지정범위에 따라서 pTarget을 중심으로 다른 target에도 적용된다.
// 이것을 사용하는 주체는 사람이 아닐수도 있다
// 여기서는 SpellRegion에 따라 적용대상을 분류만 하고 실제 효과적용은
// ApplySkill에서 처리한다.
// bOnlyDmg : 오직 데미지적용시키는것만 한다.  광역 지속데미지개체에서 사용함.
#if __VER >= 8 // __S8_PK
int		CCtrl::DoApplySkillEx( CCtrl *pTarget, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProb, int nParam, BOOL bOnlyDmg, BOOL bControl )
#else // __VER >= 8 // __S8_PK
int		CCtrl::DoApplySkillEx( CCtrl *pTarget, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProb, int nParam, BOOL bOnlyDmg )
#endif // __VER >= 8 // __S8_PK
{
	int		nRet = 1;
	CCtrl	*pSrc = this;

#ifndef __CLIENT
#ifdef __AI_AGGRO
	if( (pSkillProp->dwItemKind3 == DIS_HEAL || pSkillProp->dwItemKind3 == DIS_SUPPORT || pSkillProp->dwItemKind3 == DIS_CHEER ) && ((CMover*)pTarget)->IsAttackMode() )
		((CMover*)pSrc)->SetAggro( ((CMover*)pSrc)->GetAggro() + 1 );
#endif
#endif

	switch( pSkillProp->dwSpellRegion )
	{
	case SRO_AROUND:	// 대상 0% 주위 100%
	case SRO_REGION:	// 대상 100% 주위 100%
		{
#if __VER >= 10 //  __LEGEND
			if( pSkillProp->dwExeTarget != EXT_TROUPE && pSkillProp->dwExeTarget != EXT_TROUPEWITH )	// 극단대상이 아닐때.
#else // __VER >= 10 //  __LEGEND
			if( pSkillProp->dwExeTarget != EXT_TROUPE )	// 극단대상이 아닐때.
#endif // __VER >= 10 // __LEGEND
			{
				int nApplyType = 0;
				nApplyType = pSrc->TargetSelecter( pTarget );
#if __VER >= 8 // __S8_PK
				pTarget->ApplySkillAround( this, nApplyType, pSkillProp, pAddSkillProp, bIgnoreProb, bOnlyDmg, 0.0f, bControl );
#else // __VER >= 8 // __S8_PK
				pTarget->ApplySkillAround( this, nApplyType, pSkillProp, pAddSkillProp, bIgnoreProb, bOnlyDmg );
#endif // __VER >= 8 // __S8_PK
			}
#if __VER >= 10 //  __LEGEND
			else if( pSkillProp->dwExeTarget == EXT_TROUPEWITH )	// 극단대상이 아닐때
			{
				// 극단 대상 범위 효과.
				if( GetType() == OT_MOVER )
				{
					if( ((CMover *)this)->IsPlayer() )
					{
						if( pAddSkillProp )
						{
#ifndef __3RD_LEGEND16
							int ntmpProb	= pAddSkillProp->nProbability;
							if( (int)( xRandom(100) ) < ntmpProb )
							{
#endif // __3RD_LEGEND16
								if( ApplySkillAroundTroupe( ((CMover *)this)->m_idparty, pSkillProp, pAddSkillProp, TRUE ) == FALSE )// 멤버 각각에게 효과 적용.
								{
									pSrc->ApplySkill( this, pSkillProp, pAddSkillProp, TRUE );	
								}
#ifndef __3RD_LEGEND16
							}
#endif // __3RD_LEGEND16
						}
					}
				}
			}			
#endif // __VER >= 10 // __LEGEND
			else
			{
				// 극단 대상 범위 효과.
				if( GetType() == OT_MOVER )
				{
					if( ((CMover *)this)->IsPlayer() )
					{
						if( ApplySkillAroundTroupe( ((CMover *)this)->m_idparty, pSkillProp, pAddSkillProp, bIgnoreProb ) == FALSE )
							return 0;  // 사용실패.
					}
				}
			}
#ifdef __WORLDSERVER
			// chipi - 범위 ctrl 생성 스킬로 인해 듀얼 종료 후 카오되는 현상 수정
			if( pSkillProp->tmContinuousPain != NULL_ID && !(((CMover*)pTarget)->IsPlayer() && ((CMover*)pTarget)->m_nDead) )	// 지속데미지 형태냐?
			//if( pSkillProp->tmContinuousPain != NULL_ID )
			{
				CContDamageCtrl *pDmgCtrl = new CContDamageCtrl;	// 지속데미지를 주는 가상오브젝트 생성 - 메모리풀링 처리 할것
				if( pDmgCtrl )
				{
					pDmgCtrl->m_pSkillProp = pSkillProp;
					pDmgCtrl->m_pAddSkillProp = pAddSkillProp;
					pDmgCtrl->m_idSrc = pSrc->GetId();
					pDmgCtrl->m_idTarget = pTarget->GetId();
					pDmgCtrl->SetPos( pTarget->GetPos() );		// 중심이 되는 타겟의 좌표로...
#if __VER >= 8 // __S8_PK
					pDmgCtrl->m_bControl = bControl;
#endif // __VER >= 8 // __S8_PK
					// chipi - 범위 ctrl 생성 스킬로 인해 듀얼 종료 후 카오되는 현상 수정
					if( ((CMover*)pSrc)->IsPVPTarget( ((CMover*)pTarget) ) )
						pDmgCtrl->m_bDuelTarget = TRUE;

					GetWorld()->ADDOBJ( pDmgCtrl, FALSE, GetLayer() );
				}
			}
#endif
		}
		break;
	case SRO_LINE:		// this를 중심으로 m_fAngle라인.
		{
			int nApplyType = 0;
			nApplyType = pSrc->TargetSelecter( pTarget );
#if __VER >= 8 // __S8_PK
			ApplySkillLine( nApplyType, pSkillProp, pAddSkillProp, 2.5f, 2.5f, (float)pAddSkillProp->dwSkillRange, bIgnoreProb, bControl );
#else // __VER >= 8 // __S8_PK
			ApplySkillLine( nApplyType, pSkillProp, pAddSkillProp, 2.5f, 2.5f, (float)pAddSkillProp->dwSkillRange, bIgnoreProb );
#endif // __VER >= 8 // __S8_PK
		}
		break;
	case SRO_DIRECT:	// 대상 하나에만 적용
	default:
		if( pSkillProp->dwID != SI_PSY_PSY_PSYCHICWALL )		// 사이킥월은 처리 안함.
			pTarget->ApplySkill( this, pSkillProp, pAddSkillProp, bIgnoreProb, nParam );
		break;
	} 

#ifdef __WORLDSERVER		
	// 싸이킥월의 벽을 만들어냄.
	if( pSkillProp->dwID == SI_PSY_PSY_PSYCHICWALL )
	{
		BOOL bCreate = FALSE;
		int i = 0;
		if( GetType() == OT_MOVER )
		{
			if( ((CMover *)this)->IsPlayer() )
			{
				CUser *pUser = (CUser *)this;
				for( i = 0; i < 2; i ++ )		// 한번에 2개이상 생성되지 않게함.
				{
					if( pUser->m_pWall[i] == NULL )
					{
						CObj *pObj = CreateObj( D3DDEVICE, OT_CTRL, CI_PSYCHICWALL );		// 벽 컨트롤 생성.
						if( pObj )
						{
							pObj->SetPos( this->GetPos() );
							pObj->SetAngle( -GetAngle() );
							((CCommonCtrl *)pObj)->m_pSkillProp = pSkillProp;
							((CCommonCtrl *)pObj)->m_pAddSkillProp = pAddSkillProp;
							((CCommonCtrl *)pObj)->m_idAttacker = GetId();
							((CCommonCtrl *)pObj)->m_bControl = bControl;

							GetWorld()->ADDOBJ( pObj, TRUE, GetLayer() );
							pUser->m_pWall[i] = pObj;
							bCreate = TRUE;
							break;
						}
					}
				}
			}
		}
		if( bCreate == FALSE )	// 생성못했으면
			nRet = 0;			// 실패 리턴

	}


	if( pSrc->GetType() == OT_MOVER )
	{
		// 모든 스킬사용시 다크일루젼상태이면 해제한다!!~
		if( pSkillProp->dwID != SI_ACR_SUP_DARKILLUSION )
		{
			if( ((CMover *)pSrc)->HasBuff( BUFF_SKILL, SI_ACR_SUP_DARKILLUSION ) )
			{
				((CMover *)pSrc)->SetDarkCover( FALSE );
				((CMover *)pSrc)->RemoveBuff( BUFF_SKILL, SI_ACR_SUP_DARKILLUSION );		// 해제.
			}
		}

		// 제스터 버프는 특정 버프가 있으면 해제하고 걸어야함
		if( pSkillProp->dwID == SI_JST_SUP_POISON )
		{
			((CMover *)pSrc)->RemoveBuff( BUFF_SKILL, SI_JST_SUP_BLEEDING );
			((CMover *)pSrc)->RemoveBuff( BUFF_SKILL, SI_JST_SUP_ABSORB );
		}
		else
		if( pSkillProp->dwID == SI_JST_SUP_BLEEDING )
		{
			((CMover *)pSrc)->RemoveBuff( BUFF_SKILL, SI_JST_SUP_POISON );
			((CMover *)pSrc)->RemoveBuff( BUFF_SKILL, SI_JST_SUP_ABSORB );
		}
		else		
		if( pSkillProp->dwID == SI_JST_SUP_ABSORB )
		{
			((CMover *)pSrc)->RemoveBuff( BUFF_SKILL, SI_JST_SUP_BLEEDING );
			((CMover *)pSrc)->RemoveBuff( BUFF_SKILL, SI_JST_SUP_POISON );
		}

	}	
#endif // WorldServer
	
	// 스킬에 필요한 sfx를 생성.
	CreateSkillSfx( pTarget, pSkillProp, pAddSkillProp );
	
	// 2차 스킬 발동. ActiveSkill
	if( pSkillProp->dwActiveSkill != NULL_ID )
	{
		if( pSkillProp->dwActiveSkillRate == NULL_ID || xRandom(100) <= pSkillProp->dwActiveSkillRate )	// 발동확률에 걸렸는가.
#if __VER >= 8 // __S8_PK
			pSrc->DoActiveSkill( pSkillProp->dwActiveSkill, 1, pTarget, false, bControl );		// dwActiveSkill Lv1이 발동됨.
#else // __VER >= 8 // __S8_PK
			pSrc->DoActiveSkill( pSkillProp->dwActiveSkill, 1, pTarget );		// dwActiveSkill Lv1이 발동됨.
#endif // __VER >= 8 // __S8_PK
			
	}

	return nRet;
}

// nLevel의 dwSkill이 pTarget을 향하여 발동됨.  어태커는 this
// bIgnoreProb이 TRUE이면 내부에서 확률계산을 무시하고 100% 실행되게 한다.
#if __VER >= 8 // __S8_PK
void CCtrl::DoActiveSkill( DWORD dwSkill, int nLevel, CCtrl *pTarget, bool bIgnoreProb, BOOL bControl )
#else // __VER >= 8 // __S8_PK
void CCtrl::DoActiveSkill( DWORD dwSkill, int nLevel, CCtrl *pTarget, bool bIgnoreProb )
#endif // __VER >= 8 // __S8_PK
{
	ItemProp *pSkillProp;
	AddSkillProp *pAddSkillProp;
	GetSkillProp( &pSkillProp, &pAddSkillProp, dwSkill, nLevel, "DoActiveSkill" );
	
	int nRet;
	if( pSkillProp->dwExeTarget == EXT_MAGICATKSHOT || 
		pSkillProp->dwExeTarget == EXT_MAGICSHOT )		// 쏘는 형태면 sfx만 발생
		nRet = ShootSkill( pTarget, pSkillProp, pAddSkillProp );
	else
	{
#if __VER >= 8 // __S8_PK
		nRet = DoApplySkill( pTarget, pSkillProp, pAddSkillProp, bIgnoreProb, 0, FALSE, bControl );
#else // __VER >= 8 // __S8_PK
		nRet = DoApplySkill( pTarget, pSkillProp, pAddSkillProp, bIgnoreProb );
#endif // __VER >= 8 // __S8_PK
 #ifdef __WORLDSERVER		
		g_UserMng.AddDoApplySkill( this, pTarget->GetId(), dwSkill, nLevel );
 #endif
	}
}

