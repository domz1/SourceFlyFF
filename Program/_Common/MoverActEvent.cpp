#include "stdafx.h"
#include "defineText.h"
#include "defineObj.h"
#include "defineSkill.h"
#include "Ship.h"
#include "ModelObject.h"
#include "CreateObj.h"
//#include "ModelGlobal.h"


#ifdef __CLIENT
	#include "AppDefine.h"
	#include "defineSound.h"
	#include "Sfx.h"
	#include "DPClient.h"
	#include "..\_Common\ParticleMng.h"
#else	// __CLIENT
	#include "User.h"
	#include "WorldMng.h"
	#include "DPSrvr.h"
	#include "DPCoreClient.h"
	#include "dpdatabaseclient.h"
	#include "..\_AIInterface\AIInterface.h"
	#include "Party.h"
#endif	// not __CLIENT

#ifdef __EVENT_MONSTER
	#include "EventMonster.h"
#endif // __EVENT_MONSTER



#ifdef __CLIENT
	extern	CDPClient			g_DPlay;
#else	// __CLIENT
	extern	CUserMng			g_UserMng;
	extern	CWorldMng			g_WorldMng;
	extern	CDPSrvr				g_DPSrvr;
	extern	CDPDatabaseClient	g_dpDBClient;
	extern	CDPCoreClient		g_DPCoreClient;
	extern	CPartyMng			g_PartyMng;
#endif	// not __CLIENT

BOOL IsLootSendText( ItemProp* pItemProp )
{
	if( pItemProp == NULL )
		return FALSE;

	if( pItemProp->dwItemKind1 == IK1_GOLD    ||
		pItemProp->dwItemKind1 == IK1_GENERAL ||
		pItemProp->dwItemKind1 == IK1_SYSTEM  ||
		pItemProp->dwItemKind1 == IK1_RIDE        )
		return FALSE;

	return TRUE;
}

/*--------------------------------------------------------------------------------------------------------


											Action Event Handler
										   ----------------------
						CActionMover내에서 동작에 관련된 이벤트가 발생하면 호출되는 함수군


----------------------------------------------------------------------------------------------------------*/

//
// 스킬 모션이 작동하다 타격 프레임이 되면 이 함수를 호출함 
// 근접전 스킬.    
//
BOOL CMover::OnMeleeSkill( int nType, int nCount )
{
	if( m_oaAct != OBJACT_USESKILL )	
		return FALSE;	// 스킬사용후 타점이 되기전에 다른 명령으로 바뀐경우. 이런상황이 와선 안된다.
	int		nSkill = GetActParam( 0 );
	OBJID	idTarget = GetActParam( 1 );
	DWORD	dwLevel	= GetActParam( 3 );
#if __VER >= 8 // __S8_PK
	BOOL	bControl = GetActParam( 4 );
#endif // __VER >= 8 // __S8_PK

	ItemProp* pSkillProp;
	AddSkillProp* pAddSkillProp;

	// 스킬프로퍼티 가져옴.
	GetSkillProp( &pSkillProp, &pAddSkillProp, nSkill, dwLevel, "OnMeleeSkill" );

	// .타겟에게 스킬효과(데미지같은..)를 준다.
	CMover* pFocusObj = prj.GetMover( idTarget );		// 타겟무버포인터를 얻어냄.
	if( IsInvalidObj( pFocusObj ) )	
		return FALSE;

#if defined(__CLIENT)
	CModelObject* pModel =(CModelObject*)m_pModel;
	if( pModel )
	{
		pModel->m_SparkInfo.m_bUsed  = TRUE;
		pModel->m_SparkInfo.m_v3Color.x = 2.0f;
		pModel->m_SparkInfo.m_v3Color.y = 2.0f;
		pModel->m_SparkInfo.m_v3Color.z = 2.0f;
		pModel->m_SparkInfo.m_nCnt = 0;
		pModel->m_SparkInfo.m_fLerp= 1.0f;
		
		D3DXVECTOR3 vDir;
		if( pFocusObj == this )
			vDir = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
		else
			vDir = GetPos() - pFocusObj->GetPos();

		D3DXVec3Normalize( &pModel->m_SparkInfo.m_v3SparkDir, &vDir );
	}
#endif //defined(__CLIENT) 

	int nDamage = 0;

	// 쏘는 형태만 sfx발생.
	if( pSkillProp->dwExeTarget == EXT_MAGICATKSHOT || pSkillProp->dwExeTarget == EXT_MAGICSHOT	)	
	{
#ifdef __WORLDSERVER
		int nMaxDmgCnt	= 1;
		if( pSkillProp->tmContinuousPain != NULL_ID )
			nMaxDmgCnt	= ( pAddSkillProp->dwSkillTime / pAddSkillProp->dwPainTime ) + 1;
		float fDmgPower = 0;
		if( pSkillProp->dwDmgShift != NULL_ID )
			fDmgPower	= (int)pSkillProp->dwDmgShift * 0.2f;
//		for( int nDmgCnt = 0; nDmgCnt < nMaxDmgCnt; nDmgCnt++ )
#if __VER >= 8 // __S8_PK
			OnAttackSFX( pFocusObj->GetId(), pAddSkillProp->dwSkillLvl, pSkillProp->dwID, 0, GetAngle(), fDmgPower, ( pSkillProp->dwSkillType == KT_SKILL? AF_MELEESKILL: AF_MAGICSKILL ), bControl );
#else // __VER >= 8 // __S8_PK
			OnAttackSFX( pFocusObj->GetId(), pAddSkillProp->dwSkillLvl, pSkillProp->dwID, 0, GetAngle(), fDmgPower, ( pSkillProp->dwSkillType == KT_SKILL? AF_MELEESKILL: AF_MAGICSKILL ) );
#endif // __VER >= 8 // __S8_PK
#endif	// __WORLDSERVER
		nDamage = ShootSkill( pFocusObj, pSkillProp, pAddSkillProp );		// 서버에선 이부분은 실행안된다.
	}
	else
	{
		// 적용범위 처리. 내부에서 ApplySkill()을 호출.
#if __VER >= 8 // __S8_PK
		nDamage = DoApplySkill( pFocusObj, pSkillProp, pAddSkillProp, false, 0, FALSE, bControl );
#else // __VER >= 8 // __S8_PK
		nDamage = DoApplySkill( pFocusObj, pSkillProp, pAddSkillProp );
#endif // __VER >= 8 // __S8_PK
	}
	TRACE( "OnMeleeSkill%s, ", pSkillProp->szName );

	if( nDamage && nCount == 0 )		// 연속타점일경우 첫번째 타점에만 적용
	{
		// 쿨타임 시간 적용 - 클라 서버 동시에 돌아간다
#ifdef __CLIENT
		if( IsActiveMover() )
#endif
		{
#if __VER >= 9	// __SKILL_0706
			if( pAddSkillProp->dwCooldown != 0xFFFFFFFF )	// 쿨타임이 있는 스킬의 경우
				SetCoolTime( pAddSkillProp, "OnMeleeSkill" );
#else	// __SKILL_0706
			if( pSkillProp->dwSkillReady != 0xffffffff )	// 쿨타임이 있는 스킬의 경우
				SetCoolTime( pSkillProp, "OnMeleeSkill" );
#endif	// __SKILL_0706
		}
	}
	return TRUE;
} 


// RT_TIME을 쓰는 스킬들의 스킬 시간을 계산.
void	CMover::SubReferTime( DWORD *pTime1, DWORD *pTime2, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp )
{
	DWORD dwTime1 = 0, dwTime2 = 0;

	if( pSkillProp->dwReferTarget1 == RT_TIME )
	{
		switch( pSkillProp->dwReferStat1 )
		{
		case DST_STA:	dwTime1 = GetSta();		break;
		case DST_DEX:	dwTime1 = GetDex();		break;
		case DST_INT:	dwTime1 = GetInt();		break;
		}
		
		dwTime1 = (DWORD)( ((pSkillProp->dwReferValue1/10.0f)*dwTime1) + (pAddSkillProp->dwSkillLvl * FLOAT(dwTime1/50.0f)) );
	}
	
	if( pSkillProp->dwReferTarget2 == RT_TIME )
	{
		switch( pSkillProp->dwReferStat2 )
		{
		case DST_STA:	dwTime2 = GetSta();		break;
		case DST_DEX:	dwTime2 = GetDex();		break;
		case DST_INT:	dwTime2 = GetInt();		break;
		}
		
		dwTime2 = (DWORD)( ((pSkillProp->dwReferValue2/10.0f)*dwTime2) + (pAddSkillProp->dwSkillLvl * FLOAT(dwTime2/50.0f)) );
	}

	*pTime1 = dwTime1;
	*pTime2 = dwTime2;
}

//
// this에게 파라메터를 적용한다.
// SetDestParam을 일괄적으로 하는 부분.
// m_SkillState.Serialize()후에도 이게 수행된다.
// pSrc는 시전자.
BOOL	CMover::ApplyParam( CCtrl *pSrc, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, BOOL bSend, int nDamage )
{
	int		i;
	CMover *pTarget = (CMover *)this;

	DWORD	dwDestParam, dwSkillTime, dwPainTime, dwAbilityMin;
	int		nAdjParam, nChgParam;
	int		nDestData[3];

#if __VER >= 9	// __SKILL_0706
	BOOL bPVP	= pSrc->GetType() == OT_MOVER && ( (CMover*)pSrc )->IsPlayer() && IsPlayer();
#endif	// __SKILL_0706

	for( i = 0; i < 2; i ++ )
	{
		if( pAddSkillProp )
		{
			dwDestParam = pAddSkillProp->dwDestParam[i];
			dwSkillTime = pAddSkillProp->dwSkillTime;
			dwPainTime = pAddSkillProp->dwPainTime;
#if __VER >= 9	// __SKILL_0706
			dwAbilityMin	= bPVP? pAddSkillProp->dwAbilityMinPVP: pAddSkillProp->dwAbilityMin;
#else	// __SKILL_0706
			dwAbilityMin	= pAddSkillProp->dwAbilityMin;
#endif	// __SKILL_0706
			nAdjParam	= pAddSkillProp->nAdjParamVal[i];
			nChgParam	= (int)pAddSkillProp->dwChgParamVal[i];
			nDestData[0] = pAddSkillProp->nDestData1[0];
			nDestData[1] = pAddSkillProp->nDestData1[1];
			nDestData[2] = pAddSkillProp->nDestData1[2];
		}
		else
		{	
			dwDestParam = pSkillProp->dwDestParam[i];
			dwSkillTime = pSkillProp->dwSkillTime;
			dwPainTime  = pSkillProp->tmContinuousPain;
			dwAbilityMin = pSkillProp->dwAbilityMin;
			nAdjParam	= pSkillProp->nAdjParamVal[i];
			nChgParam	= (int)pSkillProp->dwChgParamVal[i];
			nDestData[0] = pSkillProp->nDestData1[0];
			nDestData[1] = pSkillProp->nDestData1[1];
			nDestData[2] = pSkillProp->nDestData1[2];
		}
		// destParam 1
		switch( dwDestParam )
		{
		case NULL_ID:
			break;
		case DST_CHRSTATE:		// 상태 변경
			if( nAdjParam & CHS_INVISIBILITY )	// 다크일루션상태로...
			{
				if( pTarget->SetDarkCover( TRUE, dwSkillTime ) == FALSE )
					return FALSE;
				pTarget->m_wDarkCover = 0;		
			}
			else
			if( nAdjParam & CHS_STUN )	// 스턴상태로...
			{
				if( pTarget->SetStun( TRUE, dwSkillTime ) == FALSE )
					return FALSE;
				// 일단 땜빵. 
				// 스턴이 걸리는건 두가지 루트가 있는데
				// 1. 스킬에 맞았을때.
				// 2. 스턴아이템에 맞았을때
				// 3. 기타 상황(트랩같은..)
				// 스킬을 사용해서 스턴이 걸린건 SkillInflu에서 처리해서
				// 해제까지 시켜주기땜에 m_wStunCnt에 값이 들어가면 두군데서 해제를 시키게 된다.
				// 아이템에 의해 스턴이 걸린건 SkillInflu에서 처리를 안하므로 m_wStunCnt가 필요하다.
				// 이곳은 스킬에 의해서 들어오는 곳이기 때문.
				// 구조가 잘못설계됐다.  스킬중심이 아니고 상태중심으로 처리가 됐어야 했다.
				pTarget->m_wStunCnt = 0;		
			} else
#ifdef __3RD_LEGEND16
			if( nAdjParam & CHS_SLEEPING )	// 스턴상태로...
			{
				if( pTarget->SetStun( TRUE, dwSkillTime ) == FALSE )
					return FALSE;
				pTarget->m_wStunCnt = 0;		
			} else
#endif // __3RD_LEGEND16
			if( nAdjParam & CHS_POISON )	// 독상태로..
			{
				if( pTarget->SetPoison( TRUE, pSrc->GetId(), dwSkillTime, dwPainTime, nDestData[0] ) == FALSE )	// nDestData[0] 틱당 들어가는 데미지
					return FALSE;
				pTarget->m_wPoisonCnt = 0;		// 스턴이랑 같은 이유
			} else
			if( nAdjParam & CHS_DARK )	// 암흑상태
			{
				if( pTarget->SetDark( TRUE, dwSkillTime, nDestData[1] ) == FALSE )		// nDestData1[1] : 명중률 수정치.
					return FALSE;
				pTarget->m_wDarkCnt = 0;		// 스턴이랑 같은 이유
			} else

			if( nAdjParam & CHS_BLEEDING )	// 출혈상태로..
			{
				if( nDestData[0] < 0 )
					nDestData[0]	= nDamage / 5;
				if( pTarget->SetBleeding( TRUE, pSrc->GetId(), dwSkillTime, dwPainTime, nDestData[0] ) == FALSE )
					return FALSE;
				pTarget->m_wBleedingCnt = 0;		// 스턴이랑 같은 이유
			} else
			{
				// 다른 상태들은 걍 세팅함.
				BOOL bAble = TRUE;
				if( (nAdjParam & CHS_SLOW) && pTarget->GetProp()->dwClass == RANK_MIDBOSS )		// 슬로우상태를 중간보스에게 걸려고 했다면.
					bAble = FALSE;		// 면역.
				if( bAble )
					pTarget->SetDestParam( dwDestParam, nAdjParam, NULL_CHGPARAM, TRUE );
			}
			break;

		case DST_CHR_STEALHP: 			// HP흡수.  흡혈은 DST_로 하기가 좀 안좋다.
			{
				int nStealHP = (int)( nDamage * (nAdjParam / 100.0f) );	// 실제가격한 데미지에서 흡수할양을 정함.
				if( pSrc->GetType() == OT_MOVER )
					((CMover *)pSrc)->SetDestParam( DST_HP, nStealHP, NULL_CHGPARAM );		// 시전자 HP로 흡수.
			}
			break;
		case DST_CHR_STEALHP_IMM: 			// 지정된양의 HP흡수.  
			{
				if( GetType() == OT_MOVER )
				{
					int nHP = GetHitPoint();		// 방어자의 hp
					int nStealHP = nAdjParam;	// 흡수해야할 hp
					if( nHP < nStealHP )	// 피를 뽑아와야하는데 남은피가 더 적으면.
						nStealHP = nHP;		// 남은피만큼 뽑아옴
					//SetDestParam( DST_HP, -nStealHP, 0 );		// 피를 뺏김.
					if( pSrc->GetType() == OT_MOVER )
					{
//   						((CMover*)this)->m_pActMover->SendDamage( AF_FORCE,		// 타겟은 데미지입고
//   													      GetId(), 
//  														  nStealHP );
						// chipi_080303 버그 수정 - 몬스터가 자기 자신을 타겟으로 설정하는 문제
 						((CMover*)this)->m_pActMover->SendDamage( AF_FORCE,		// 타겟은 데미지입고
  														  ((CMover*)pSrc)->GetId(), 
 														  nStealHP );


						((CMover *)pSrc)->SetDestParam( DST_HP, nStealHP, NULL_CHGPARAM, TRUE  );		// 공격자 HP로 흡수.
					}
				}
			}
			break;

		case DST_CURECHR:		// 상태 해제
			pTarget->RemoveDebuff( nAdjParam );
			break;

		case DST_CLEARBUFF:		// 대상의 버프를 파괴.
			if( nAdjParam == NULL_ID )
			{
				pTarget->RemoveBuffAll();
			}
			else
			{
				for( int i = 0; i < nAdjParam; i ++ )
					pTarget->RemoveBuffOne();
			}
			break;
		case DST_CHR_CHANCESTUN:	// adj:확률		data1:스킬
		case DST_CHR_CHANCEPOISON:	// adj:확률		data1:스킬
		case DST_CHR_CHANCEDARK:	// adj:확률		data1:스킬
		case DST_CHR_CHANCEBLEEDING:// adj:확률		data1:스킬
		case DST_CHR_CHANCESTEALHP:	// adj:확률		data1:스킬
			pTarget->SetDestParam( dwDestParam, nAdjParam, nChgParam, TRUE );
			break;
			
#ifdef __WORLDSERVER
		case DST_HP:	// 힐링.
			if( pAddSkillProp )
			{
				if( pSkillProp->dwReferTarget1 == RT_HEAL || pSkillProp->dwReferTarget2 == RT_HEAL )
				{
					DWORD dwHp1 = 0, dwHp2 = 0;
					
					if( pSrc->GetType() == OT_MOVER )	// 시전자가 사람일때만 스탯에 의한 영향
					{
						if( pSkillProp->dwReferTarget1 == RT_HEAL )
						{
							switch( pSkillProp->dwReferStat1 )
							{
							case DST_STA:		dwHp1 = ((CMover *)pSrc)->GetSta();			break;
							case DST_DEX:		dwHp1 = ((CMover *)pSrc)->GetDex();			break;
							case DST_INT:		dwHp1 = ((CMover *)pSrc)->GetInt();			break;
							}
							dwHp1 = (DWORD)( ((pSkillProp->dwReferValue1 / 10.0f) * dwHp1) + (pAddSkillProp->dwSkillLvl * FLOAT(dwHp1 / 50.0f)) );
						}
						
						if( pSkillProp->dwReferTarget2 == RT_HEAL )
						{
							switch( pSkillProp->dwReferStat2 )
							{
							case DST_STA:		dwHp2 = ((CMover *)pSrc)->GetSta();			break;
							case DST_DEX:		dwHp2 = ((CMover *)pSrc)->GetDex();			break;
							case DST_INT:		dwHp2 = ((CMover *)pSrc)->GetInt();			break;
							}
							dwHp2 = (DWORD)( ((pSkillProp->dwReferValue2 / 10.0f) * dwHp2) + (pAddSkillProp->dwSkillLvl * FLOAT(dwHp2 / 50.0f)) );
						}
					}
					
					// 증가될 HP량.
					int nIncHP = pAddSkillProp->nAdjParamVal1 + dwHp1 + dwHp2;
					// 힐링계열 스킬을 썼냐?
					if( (pAddSkillProp->dwDestParam1 == DST_HP || pAddSkillProp->dwDestParam2 == DST_HP) && nIncHP > 0 )
					{
						if( pSrc->GetType() == OT_MOVER )
							if( ((CMover *)pSrc)->IsPlayer() )	// 시전자가 플레이어일때
								if( pTarget->GetMaxHitPoint() > pTarget->GetHitPoint() + nIncHP )		// 타겟이 오버힐 됐냐?
									((CUser *)pSrc)->m_nOverHeal = PROCESS_COUNT * 30;				// 시전자는 30초간 오버힐 상태. 
					}
					
					// 힐시작.
					pTarget->SetDestParam( pAddSkillProp->dwDestParam1, nIncHP, pAddSkillProp->dwChgParamVal1, TRUE );
					
				}
			}
			break;
#endif // WorldServer
#ifdef __BARUNA_PIERCING
#ifdef __WORLDSERVER
		case DST_HEAL_HP_RATE:
			{
				int nIncHp = (((CMover*)pSrc)->GetMaxHitPoint() / 10000 ) * pAddSkillProp->nAdjParamVal1;
				((CMover*)pSrc)->SetDestParam( DST_HP, nIncHp, pAddSkillProp->dwChgParamVal1, TRUE);
			}
			break;
		case DST_HEAL_FP_RATE:
			{
				int nIncFp = (((CMover*)pSrc)->GetMaxFatiguePoint() * 10000 ) / pAddSkillProp->nAdjParamVal1;
				((CMover*)pSrc)->SetDestParam( DST_FP, nIncFp, pAddSkillProp->dwChgParamVal1, TRUE);
			}
			break;
		case DST_HEAL_MP_RATE:
			{
				int nIncMp = (((CMover*)pSrc)->GetMaxManaPoint() * 10000 ) / pAddSkillProp->nAdjParamVal1;
				((CMover*)pSrc)->SetDestParam( DST_MP, nIncMp, pAddSkillProp->dwChgParamVal1, TRUE);
			}
			break;
		case DST_FORCE_DAMAGE_SAFE_HP_RATE:
			{
				//if( pAddSkillProp->nAdjParamVal1 == 0 )
				//{
				//	((CMover*)pSrc)->DoDie( pSrc);	
				//}
				//else
				//{
//					int nDecHp = ((((CMover*)pSrc)->GetMaxHitPoint() * pAddSkillProp->nAdjParamVal1 ) / 10000);
//					if( ((CMover*)pSrc)->GetHitPointPercent() >= nDecHP )
//						((CMover*)pSrc)->SetDestParam( DST_HP, nDecHp, pAddSkillProp->dwChgParamVal1, TRUE);
				//}
				pTarget->SendDamage( AF_FORCE, ((CMover*)pSrc)->GetId(), 
					( pTarget->GetMaxHitPoint() / 10000 ) * pAddSkillProp->nAdjParamVal1 ) ;
			}
			break;
		case DST_FORCE_DAMAGE_VALUE:
			for ( int i = 0; i <= MAX_BARUNA_DOT; i++ )
			{
				if( m_dwForceAttackerID[i] == 0 )
				{
					m_dwForceDamageCnt[i] =  (short)( ( pAddSkillProp->dwSkillTime / 1000.0f) * PROCESS_COUNT );
					m_dwForceDamageDamage[i] = (short)pAddSkillProp->nAdjParamVal1;
					m_dwForceDamageUnit[i] = pAddSkillProp->dwPainTime;
					m_dwForceAttackerID[i] = pSrc->GetId();
					//m_adjParamAry[DST_FORCE_DAMAGE_VALUE]	+= pAddSkillProp->nAdjParamVal1;
					pTarget->SetDestParam( dwDestParam, nAdjParam, nChgParam, bSend );
					break;
				}
			}
#endif // __WORLDSERVER
#endif // __BARUNA_PIERCING
		case DST_SPEED:
			{
				MoverProp *pMoverProp = pTarget->GetProp();
				if( pMoverProp )
				{
					if( pMoverProp->dwClass != RANK_MIDBOSS		// MIDBOSS만 아니면 다 걸림.
#if __VER >= 12 // __SECRET_ROOM
						&& pMoverProp->dwClass != RANK_SUPER	// 12차 부터는 SUPER도 안걸린다.
#endif // __SECRET_ROOM
						)		
						pTarget->SetDestParam( dwDestParam, nAdjParam, nChgParam, bSend );
				}
			}
			break;
		default: // 그외 DST_는 일반적인 루틴을 탄다.
			// 일반 DST_처리
			pTarget->SetDestParam( dwDestParam, nAdjParam, nChgParam, bSend );
			break;
		}
	}

	return TRUE;
}


BOOL	CMover::GetSkillProp( ItemProp **ppSkillProp, AddSkillProp **ppAddSkillProp, int nSkill, DWORD dwLevel, LPCTSTR szErr )
{
	char szStr[256] = {0, };
	sprintf( szStr, "%s-%s", m_szName, szErr );

	return CCtrl::GetSkillProp( ppSkillProp, ppAddSkillProp, nSkill, dwLevel, szStr );
}


// 쿨타임 시작!
#if __VER >= 9	// __SKILL_0706
void	CMover::SetCoolTime( AddSkillProp* pAddSkillProp, LPCTSTR szCall )
{
	int nIdx	= GetSkillIdx( pAddSkillProp->dwName );		// 스킬리스트 인덱스를 찾음.
	
	if( nIdx < 0 || nIdx >= MAX_SKILL_JOB )
		Error( "szCall SetCoolTime : %d %d스킬을 찾을 수 없음 %s", nIdx, pAddSkillProp->dwName, GetName() );
	else
		m_tmReUseDelay[ nIdx ] = pAddSkillProp->dwCooldown + timeGetTime();		// 1/1000단위
}
#else	// __SKILL_0706
void	CMover::SetCoolTime( ItemProp *pSkillProp, LPCTSTR szCall )
{
	int nIdx = GetSkillIdx( pSkillProp->dwID );		// 스킬리스트 인덱스를 찾음.
	
	if( nIdx < 0 || nIdx >= MAX_SKILL_JOB )
		Error( "szCall SetCoolTime : %d %d스킬을 찾을 수 없음 %s", nIdx, pSkillProp->dwID, GetName() );
	else
		m_tmReUseDelay[ nIdx ] = pSkillProp->dwSkillReady + timeGetTime();	// 1/1000단위
}
#endif	// __SKILL_0706
//
//	마법 스킬의 타점때 호출.
//  nCount 워터볼의 경우 여러번 호출이 되기때문에 nCount==0 의 경우만 경험치가 올라간다.
//
BOOL CMover::OnMagicSkill( int nType, int nCount )
{
	if( m_oaAct != OBJACT_USESKILL )	
		return FALSE;	// 스킬사용후 타점이 되기전에 다른 명령으로 바뀐경우. 이런상황이 와선 안된다.

	int			nSkill = GetActParam(0);
	OBJID		idTarget = GetActParam(1);
	DWORD		dwLevel = GetActParam( 3 );
#if __VER >= 8 // __S8_PK
	BOOL	bControl = GetActParam( 4 );
#endif // __VER >= 8 // __S8_PK

	ItemProp*	pSkillProp = NULL;
	AddSkillProp* pAddSkillProp = NULL;
	LPSKILL		pSkill	= NULL;
	
	BOOL bRet = GetSkillProp( &pSkillProp, &pAddSkillProp, nSkill, dwLevel, "CMover::OnMagicSkill" );
	if( bRet == FALSE )
	{
		Error( "OnMagicSkill : 스킬프롭읽기 실패함. 현재진행중인 Motion=%d %d", m_dwMotion, dwLevel );
		return FALSE;
	}
	
	CMover* pFocusObj = prj.GetMover( idTarget );		// 타겟무버포인터를 얻어냄.
	if( IsInvalidObj( pFocusObj ) )		
		return FALSE;

	if( pSkillProp->dwID == SI_ASS_HEAL_HEALING )	// 힐링 상대가 죽었으면 취소
	{
		if( pFocusObj->IsDie() )
			return FALSE;
	}

	BOOL bSuccess = FALSE;
	BOOL bShoot	= pSkillProp->dwExeTarget == EXT_MAGICATKSHOT  || pSkillProp->dwExeTarget == EXT_MAGICSHOT;
	if( bShoot )	// 쏘는 형태만 sfx발생.
	{
#ifdef __WORLDSERVER
		int nMaxDmgCnt	= 1;
		if( pSkillProp->tmContinuousPain != NULL_ID )
			nMaxDmgCnt	= ( pAddSkillProp->dwSkillTime / pAddSkillProp->dwPainTime ) + 1;
		float fDmgPower = 0;
		if( pSkillProp->dwDmgShift != NULL_ID )
			fDmgPower	= (int)pSkillProp->dwDmgShift * 0.2f;
//		for( int nDmgCnt = 0; nDmgCnt < nMaxDmgCnt; nDmgCnt++ )
#if __VER >= 8 // __S8_PK
			OnAttackSFX( pFocusObj->GetId(), pAddSkillProp->dwSkillLvl, pSkillProp->dwID, 0, GetAngle(), fDmgPower, ( pSkillProp->dwSkillType == KT_SKILL? AF_MELEESKILL: AF_MAGICSKILL ), bControl );
#else // __VER >= 8 // __S8_PK
			OnAttackSFX( pFocusObj->GetId(), pAddSkillProp->dwSkillLvl, pSkillProp->dwID, 0, GetAngle(), fDmgPower, ( pSkillProp->dwSkillType == KT_SKILL? AF_MELEESKILL: AF_MAGICSKILL ) );
#endif // __VER >= 8 // __S8_PK
#endif	// __WORLDSERVER
		bSuccess = ShootSkill( pFocusObj, pSkillProp, pAddSkillProp );		// 서버에선 이부분은 실행안된다.
	}
	else
	{
		// 적용범위 처리. 내부에서 ApplySkill()을 호출.
#if __VER >= 8 // __S8_PK
		int nDamage = DoApplySkill( pFocusObj, pSkillProp, pAddSkillProp, false, 0, FALSE, bControl );
#else // __VER >= 8 // __S8_PK
		int nDamage = DoApplySkill( pFocusObj, pSkillProp, pAddSkillProp );
#endif // __VER >= 8 // __S8_PK
		if( nDamage == 0 )
			return FALSE;
		bSuccess = (BOOL)nDamage;

	#ifdef __CLIENT	
		if( pSkillProp->dwID == SI_MAG_MAG_BLINKPOOL )
		{
			CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
			
			if( IsActiveMover() )
			{
				D3DXVECTOR3 vStart;
				D3DXVECTOR3 vEnd;

				vStart = GetPos();		vStart.y += 1.0f;
				vEnd = pWndWorld->m_vTelePos;
				
				CWorld *pWorld = GetWorld();
				
				if( pWorld->IntersectObjLine( NULL, vStart, vEnd, FALSE, FALSE ) )
				{
					g_WndMng.m_pWndWorld->SetNextSkill( NEXTSKILL_NONE );
					g_WndMng.PutString( prj.GetText( TID_GAME_BLOCKTARGETING ), NULL, prj.GetTextColor( TID_GAME_BLOCKTARGETING ) );
					return FALSE;
				}
			
				g_DPlay.SendTeleSkill( GetId(), pWndWorld->m_vTelePos );
			}
		}
	#endif // Client		
	}
	
	if( bSuccess )	// 스킬사용 실패하면 마나안깎임. / 경험치 안오름.
	{
	#ifdef __WORLDSERVER
		// 발사한 순간에 마나등을 깍는다.
		int nReqMp = GetReqMp( pAddSkillProp->nReqMp );

		#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
		if(nReqMp != 9999999)
		#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans
		{
			if( pAddSkillProp->nSkillCount > 1 )
				nReqMp /= pAddSkillProp->nSkillCount;
			IncManaPoint( -nReqMp );
			if( pAddSkillProp->nReqFp != NULL_ID )
				IncFatiguePoint( -GetReqFp( pAddSkillProp->nReqFp ) );
		}
		
		#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
			if( pSkillProp->dwID == SI_RIG_HERO_RETURN )
			{
				REPLACE_TYPE type = REPLACE_NORMAL;

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
			if( pSkillProp->dwID == SI_KNT_HERO_DRAWING )
			{
				D3DXVECTOR3 vSrc	= GetPos();
				FLOAT		fAngle	= GetDegree( pFocusObj->GetPos(), GetPos() );	// 시전자와 타겟의 각도를 구함
				//vSrc.x -= 1.0f;
				//vSrc.z -= 1.0f;

				pFocusObj->SetPos(vSrc);
				g_UserMng.AddSetPos( pFocusObj, vSrc );
				g_UserMng.AddPushPower( pFocusObj, pFocusObj->GetPos(), pFocusObj->GetAngle(), fAngle, 0.2f );
				pFocusObj->ClearDestObj();
//				pFocusObj->SetPosChanged( TRUE );
			}
		#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans


	#endif // WORLDSERVER

		// 쿨타임 시간 적용 - 클라 서버 동시에 돌아간다
	#ifdef __CLIENT
		if( IsActiveMover() && nCount == 0 )		// 연속타점일경우 첫번째 타점에만 적용
	#endif // __CLIENT
#if __VER >= 9	// __SKILL_0706
		if( pAddSkillProp->dwCooldown != 0xFFFFFFFF )
			SetCoolTime( pAddSkillProp, "OnMagicSkill" );
#else	// __SKILL_0706
		if( pSkillProp->dwSkillReady != 0xffffffff )	// 쿨타임이 있는 스킬의 경우
			SetCoolTime( pSkillProp, "OnMagicSkill" );
#endif	// __SKILL_0706
	} // bSuccess

#if defined(__CLIENT)
	CModelObject* pModel = (CModelObject*)m_pModel;
	if( pModel )
	{
		pModel->m_SparkInfo.m_bUsed  = TRUE;
		pModel->m_SparkInfo.m_v3Color.x = 2.0f;
		pModel->m_SparkInfo.m_v3Color.y = 2.0f;
		pModel->m_SparkInfo.m_v3Color.z = 2.0f;
		pModel->m_SparkInfo.m_nCnt = 0;
		pModel->m_SparkInfo.m_fLerp= 1.0f;
		
		AngleToVectorXZ( &pModel->m_SparkInfo.m_v3SparkDir, GetAngle(), -1.0f );
	}
#endif //defined(__CLIENT)

	return TRUE;
} // OnMagicSkill


//
//	장거리무기가 발사되는 시점에 발생하는 이벤트
//
BOOL	CMover::OnAttackRange()
{
	int		idTarget = GetActParam( 0 );
	DWORD	dwItemID = GetActParam( 1 );		// 사용할 아이템 아이디 (player이면 충전레벨)
	int		idSfxHit = GetActParam( 2 );		// 몬스터가 사용한것이면 여기에 서버로부터 받은값이 들어있다.
#if __VER >= 8 // __S8_PK
	BOOL	bControl = FALSE;
#endif // __VER >= 8 // __S8_PK

	CMover* pHit = prj.GetMover( idTarget );	// 타겟의 포인터를 얻어냄.
	if( IsInvalidObj(pHit) )	
		return FALSE;							// 클릭했을당시는 있었으나 발사되기전에 삭제되었다.
	
	ItemProp *pItemProp = NULL;
	if( IsPlayer() )
		pItemProp = GetActiveHandItemProp();		// 이런경우는 걍 손에 든 무기(혹은 기본아이템)
	else 
	{
		if( dwItemID == NULL_ID || dwItemID == 0 )	
			pItemProp = GetActiveHandItemProp();		// 이런경우는 걍 손에 든 무기(혹은 기본아이템)
		else
			pItemProp = prj.GetItemProp( dwItemID );	// 직접 아이템을 지정한 경우는 그걸로 공격함(몬스터가 주로 사용함)
	}

	if( pItemProp == NULL )
		return FALSE;

#ifdef __WORLDSERVER
#if __VER >= 12 // __MONSTER_SKILL
	if( IsNPC() )
		CMonsterSkill::GetInstance()->ApplySkill( this, pHit, ATK_RANGE );
#endif // __MONSTER_SKILL
#endif // __WORLDSERVER

	DWORD dwSfxObj = pItemProp->dwSfxObj2;
	
	D3DXVECTOR3 vPosDest = pHit->GetPos() + D3DXVECTOR3( 0.0f, 1.0f, 0.0f ); // 발사 지점을 임의로 올려준다. 땜빵 
	D3DXVECTOR3 vPosSrc  = GetPos() + D3DXVECTOR3( 0.0f, 1.0f, 0.0f ); // 발사 지점을 임의로 올려준다. 땜빵 

#ifdef __CLIENT		
	CSfx* pSfx = NULL;
#endif
	// 아크로뱃은 이펙트 출력이 항상 왼손에...
	if( IsPlayer() )		
	{
		ItemProp *pItemElem = GetActiveHandItemProp( PARTS_BULLET );  // 화살아이템을 참조로 화살이펙트 생성
		if( pItemElem && pItemElem->dwSfxObj2 != NULL_ID )
		{
		#ifdef __CLIENT
			if( IsActiveMover() )
				pSfx = CreateSfxArrow( pItemElem->dwSfxObj2, pItemProp->dwSfxObj3, vPosDest, idTarget );
		#endif //__CLIENT

		#ifdef __WORLDSERVER
			g_UserMng.AddCreateSfxAllow( this, pItemElem->dwSfxObj2, pItemProp->dwSfxObj3, vPosDest, idTarget );
		#endif //__WORLDSERVER
		}
	}
	else
	{
		if( dwSfxObj == NULL_ID )	
			return FALSE;
	#ifdef __CLIENT		
		pSfx = CreateSfx( D3DDEVICE, dwSfxObj, vPosSrc,GetId(), vPosDest ,idTarget );
	#endif
	}

	DWORD dwAtkFlags = AF_GENERIC;
	if( IsPlayer() )
		dwAtkFlags |= AF_RANGE;

#ifdef __WORLDSERVER
#if __VER >= 8 // __S8_PK
	OnAttackSFX( pHit->GetId(), ( IsPlayer()? dwItemID: 0 ), NULL_ID, 0, 0, 0, dwAtkFlags, bControl );
#else // __VER >= 8 // __S8_PK
	OnAttackSFX( pHit->GetId(), ( IsPlayer()? dwItemID: 0 ), NULL_ID, 0, 0, 0, dwAtkFlags );
#endif // __VER >= 8 // __S8_PK
#endif	// __WORLDSERVER

#ifdef __CLIENT
	if( pSfx )
	{
		if( IsActiveMover() )		
		{
			idSfxHit = ++m_idSfxHit; 	// 클라에서 발사체가 생성된 즉시 ID를 만들고 그것을 서버로 보냄.
			g_DPlay.SendSfxID( idTarget, idSfxHit, dwAtkFlags );
			pSfx->m_idSfxHit	= idSfxHit;
			pSfx->m_nMagicPower = IsPlayer() ? dwItemID : 0;		// 충전레벨도 넣어준다.
		}
		// 쏜놈이 g_pPlayer거나 / 쏜놈은 몹이고 맞는놈이 g_pPlayer면
		if( IsActiveMover() || (IsNPC() && pHit->IsActiveMover()) )
			pSfx->m_idSfxHit	= idSfxHit;		// id를 등록함
	}
#endif //__CLIENT
	return TRUE;
}


//
//	완드(매직)어택이 발사되는 시점에 발생하는 이벤트.
//	
BOOL	CMover::OnAttackMagic()
{
	int		idTarget = GetActParam( 0 );
	int		nMagicPower = GetActParam( 1 );
#if __VER >= 8 // __S8_PK
	BOOL	bControl = GetActParam( 4 );
#endif // __VER >= 8 // __S8_PK

	CMover* pHit = prj.GetMover( idTarget );	// 타겟의 포인터를 얻어냄.

	if( IsInvalidObj(pHit) )	
		return FALSE;		// 클릭했을당시는 있었으나 발사되기전에 삭제되었다.

#ifdef __WORLDSERVER
#if __VER >= 8 // __S8_PK
	OnAttackSFX( pHit->GetId(), nMagicPower, NULL_ID, 0, 0, 0, AF_MAGIC, bControl );
#else // __VER >= 8 // __S8_PK
	OnAttackSFX( pHit->GetId(), nMagicPower, NULL_ID, 0, 0, 0, AF_MAGIC );
#endif // __VER >= 8 // __S8_PK
#else	// __WORLDSERVER
	DWORD dwWeaponId;

	if( IsActiveMover() ) 
	{
		CItemElem* pWeapon = GetWeaponItem();
		if( pWeapon == NULL )	return FALSE;
		dwWeaponId	= pWeapon->m_dwItemId;
	}
	else 
	{
		dwWeaponId	= m_aEquipInfo[PARTS_RWEAPON].dwId;
		if( dwWeaponId == NULL )	return FALSE;
	}

#if __VER < 11 // __REMOVE_ENDURANCE
	if( IsActiveMover() )
	{
		CItemElem* pWeapon = GetWeaponItem();
		ItemProp* pItemProp		= pWeapon->GetProp();
		if( pItemProp )
			if( ( ( pWeapon->m_nHitPoint * 100 ) / pItemProp->dwEndurance ) == 0 )
				g_WndMng.PutString( prj.GetText( TID_GAME_WASTEENDURANCE ), NULL, prj.GetTextColor( TID_GAME_WASTEENDURANCE ) );
	}
#endif // __REMOVE_ENDURANCE

	D3DXVECTOR3 vPosDest = pHit->GetPos(); 
	// 발사 목표지점은 상대의 중앙.
	float fHeight = fabs(pHit->m_pModel->m_vMax.y) / 2.0f;	// 아이바트같은넘은 공중에 떠있기땜에 순수 높이인 Max값만 취했다.
	fHeight *= pHit->GetScale().x;
	vPosDest.y += fHeight;	

	D3DXVECTOR3 vPosSrc = GetPos() + D3DXVECTOR3( 0.0f, 1.0f, 0.0f ); // 발사 지점을 임의로 올려준다. 땜빵 

	ItemProp *pHandItemProp = GetActiveHandItemProp( PARTS_RWEAPON );
	CSfx* pSfx = NULL;
	if( m_pActMover->IsFly() )
	{
		pSfx = CreateSfx( D3DDEVICE, XI_FIR_WAND02, vPosSrc, GetId(), vPosDest, idTarget );
		if( pSfx )
			pSfx->ShootSfx( GetAngle(), -GetAngleX(), 0.75f );		 
	} 
	else
	{
		DWORD dwSfxObj2 = XI_FIR_WAND01;
		if( pHandItemProp )
			dwSfxObj2 = pHandItemProp->dwSfxObj2;

		pSfx = CreateSfx( D3DDEVICE, dwSfxObj2, vPosSrc,GetId(), vPosDest ,idTarget );
		if( pSfx )
		{
			switch(nMagicPower) 
			{
			case 1:		pSfx->SetScale( D3DXVECTOR3( 1.2f, 1.2f, 1.2f ) );	break;
			case 2:		pSfx->SetScale( D3DXVECTOR3( 1.7f, 1.7f, 1.7f ) );	break;
			case 3:
			case 4:		pSfx->SetScale( D3DXVECTOR3( 2.5f, 2.5f, 2.5f ) );	break;
			}
			((CSfxItemWandAtk1 *)pSfx)->m_fCenter = fHeight;		
		}
	}

	if( pSfx )
	{
		if( IsActiveMover() )		// g_pPlayer만 아래짓을 하게 한다.
		{
			int idSfxHit = ++m_idSfxHit;			// 클라에서 발사체가 생성된 즉시 ID를 만들고 그것을 서버로 보냄.
			g_DPlay.SendSfxID( idTarget, idSfxHit, AF_MAGIC );
			pSfx->m_idSfxHit	= idSfxHit;
			pSfx->m_nMagicPower = nMagicPower;		// 충전레벨도 넣어준다.
		}
	}


	CModelObject* pModel = (CModelObject*)m_pModel;
	if( pModel )
	{
		pModel->m_SparkInfo.m_bUsed  = TRUE;
		pModel->m_SparkInfo.m_v3Color.x = 2.0f;
		pModel->m_SparkInfo.m_v3Color.y = 2.0f;
		pModel->m_SparkInfo.m_v3Color.z = 2.0f;
		pModel->m_SparkInfo.m_nCnt = 0;
		pModel->m_SparkInfo.m_fLerp= 1.0f;
		
		AngleToVectorXZ( &pModel->m_SparkInfo.m_v3SparkDir, GetAngle(), -1.0f );
	}
#endif // __WORLDSERVER
	return TRUE;
}

void	CMover::OnAttackMelee_ClockWorks( DWORD dwState, CMover *pHitObj )
{
	int		nItemID = GetActParam( 1 );	// 사용할 아이템 아이디

#ifdef __WORLDSERVER
	UpdateMatrix();		// 몸체의 특정부위에서 나는 데미지를 처리하기 위해선 서버에서도 매트릭스를 갱신해야한다.
	CModelObject *pModel = (CModelObject *)m_pModel;
	pModel->UpdateMatrixBone();		// 서버에서는 본애니메이션을 하지 않으므로 GetEventPos()를 사용할 일이 있을땐 업데이트를 해줘야 한다.
#endif // WorldServer
	
	switch( dwState )
	{
	case OBJSTA_ATK1:		// 칼로 내려치기
		{
			D3DXVECTOR3 vPos;
			CModelObject *pModel = (CModelObject *)m_pModel;
			pModel->GetEventPos( &vPos, 0 );
			D3DXVec3TransformCoord( &vPos, &vPos, GetMatrixWorldPtr() );

			OnDamage( nItemID, dwState, NULL, &vPos );		// 이펙트가 발생하는 위치에 데미지를 주고
#ifdef __CLIENT
			GetWorld()->m_pCamera->SetQuake( 15, 0.5f );
			CSfx *pSfx = CreateSfx( D3DDEVICE, XI_NPCSP1DIRFIRESP, vPos );	// 이펙트를 발생 - 서버에선 생성 안됨.
			if( pSfx )
				pSfx->SetAngle( -GetAngle() );
#endif 
		}
		break;
	case OBJSTA_ATK2:		// 포쏘기
		{
			D3DXVECTOR3 vPos;
			CModelObject *pModel = (CModelObject *)m_pModel;
			pModel->GetEventPos( &vPos, 1 );	// 왼손 집게발 위치.
			D3DXVec3TransformCoord( &vPos, &vPos, GetMatrixWorldPtr() );
#ifdef __CLIENT
			CSfx *pSfx = CreateSfx( D3DDEVICE, XI_NPCSP1DIRCANNON, vPos, GetId() );	// 발사이펙트.
			if( pSfx )
			{
				pSfx->SetAngle( -GetAngle() );
				pSfx->SetScale( D3DXVECTOR3(4.0f, 4.0f, 4.0f) );
			}
#endif
			CMover *pTarget = pHitObj;
			if( IsValidObj(pTarget) )
			{
				OnDamage( nItemID, dwState, NULL, &pTarget->GetPos() );		// 타겟의 좌표를 중심으로 데미지를 줌.
#ifdef __CLIENT
				pSfx = CreateSfx( D3DDEVICE, XI_NPCSP1RANBALL, vPos, GetId(), pTarget->GetPos(), pTarget->GetId(), -1 );		// 포탄
				if( pSfx )
				{
					pSfx->SetScale( D3DXVECTOR3(4.0f, 4.0f, 4.0f) );
					{
						FLOAT fAngXZ, fAngH;
						D3DXVECTOR3 vDist = pTarget->GetPos() - vPos;
						xGetDegree( &fAngXZ, &fAngH, vDist );		// 목표와의 각도구함.
						((CSfxAtkStraight *)pSfx)->ShootSfx( fAngXZ, -fAngH, 2.0f, XI_NPCSP1RANBOOM );		// 날아가는 방향 설정.
					}
				}
#endif
			}
		}
		break;
	case OBJSTA_ATK3:
#ifdef __CLIENT
		GetWorld()->m_pCamera->SetQuake( 30, 0.7f );
#endif
		break;
	case OBJSTA_ATK4:	// 발로 밟기.
		{
			D3DXVECTOR3 vPos;

			vPos = GetPos();
			DWORD dwFlag = AF_GENERIC;
			if( xRandom(100) < 15 )
				dwFlag |= AF_FLYING;
			OnDamage( nItemID, dwState, NULL, &vPos, dwFlag );		// 데미지는 몸중심을 기준으로 한다.

			CModelObject *pModel = (CModelObject *)m_pModel;
			pModel->GetEventPos( &vPos, 3 );
			D3DXVec3TransformCoord( &vPos, &vPos, GetMatrixWorldPtr() );

#ifdef __CLIENT
			GetWorld()->m_pCamera->SetQuake( 60, 1.5f );

			CSfx *pSfx = CreateSfx( D3DDEVICE, XI_NPCSP1DIRCIRCLE, vPos );
			if( pSfx )
				pSfx->SetScale( D3DXVECTOR3(2.0f, 2.0f, 2.0f) );
			pModel->GetEventPos( &vPos, 4 );
			D3DXVec3TransformCoord( &vPos, &vPos, GetMatrixWorldPtr() );

			pSfx = CreateSfx( D3DDEVICE, XI_NPCSP1DIRCIRCLE, vPos );
			if( pSfx )
				pSfx->SetScale( D3DXVECTOR3(2.0f, 2.0f, 2.0f) );
#endif
		}
		break;
	}
}

void	CMover::OnAttackMelee_BigMuscle( DWORD dwState, CMover *pHitObj )
{
	int		nItemID = GetActParam( 1 );	// 사용할 아이템 아이디
	CModelObject *pModel = (CModelObject *)m_pModel;
	
#ifdef __WORLDSERVER
	UpdateMatrix();		// 몸체의 특정부위에서 나는 데미지를 처리하기 위해선 서버에서도 매트릭스를 갱신해야한다.
	pModel->UpdateMatrixBone();		// 서버에서는 본애니메이션을 하지 않으므로 GetEventPos()를 사용할 일이 있을땐 업데이트를 해줘야 한다.
#endif // WorldServer
	
	switch( dwState )
	{
	case OBJSTA_ATK1:	
	case OBJSTA_ATK2:
		{
			if( IsValidObj(pHitObj) )
			{
				OnDamage( nItemID, dwState, pHitObj );		
			}
		}
		break;
	case OBJSTA_ATK3:		// 땅치기 스킬 - 두손
		{
			D3DXMATRIX mMatrix;
			
			D3DXMatrixMultiply( &mMatrix, (pModel->GetMatrixBone(12)), &(GetMatrixWorld()) );
			
			D3DXVECTOR3 v2;
			v2.x = mMatrix._41;
			v2.y = mMatrix._42;
			v2.z = mMatrix._43;
			v2.y -= 5.0f;
			
		#ifdef __CLIENT
			// 땅치고 끝날때 이펙트 왼손
			CSfx *pSfx = CreateSfx( D3DDEVICE, XI_SKILL_CIRCLE_DUST, v2 );			
			if( pSfx )
				pSfx->SetScale( D3DXVECTOR3( 12.0f, 12.0f, 12.0f ) );

			PLAYSND( SND_PC_SKILLD_HITOFPENYA );//, &v2 );
		#endif //__CLIENT 			
			
			D3DXMatrixMultiply( &mMatrix, (pModel->GetMatrixBone(16)), &(GetMatrixWorld()) );
			D3DXVECTOR3 v3;
			v3.x = mMatrix._41;
			v3.y = mMatrix._42;
			v3.z = mMatrix._43;						
			v3.y -= 5.0f;
			
		#ifdef __CLIENT
			// 땅치고 끝날때 이펙트 오른손
			pSfx = CreateSfx( D3DDEVICE, XI_SKILL_CIRCLE_DUST, v3 );			
			if( pSfx )
				pSfx->SetScale( D3DXVECTOR3( 12.0f, 12.0f, 12.0f ) );
		#endif //__CLIENT 			
		
			if( m_pActMover->m_nMotionHitCount == 1 )
			{				
	#ifdef __WORLDSERVER
				GetWorld()->SendDamageAround( &GetPos(), (AF_GENERIC | AF_CRITICAL) , this, OBJTYPE_PLAYER, II_WEA_MOB_MONSTER4_ATK2
					, 16.0f );					

				ItemProp *pSkillProp;
				AddSkillProp *pAddSkillProp;
				BOOL bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SI_GEN_BLEEDING, 10, "OnAttackMelee_BigMuscle" );
				if( bSuccess )
					ApplySkillAround( this, OBJTYPE_PLAYER, pSkillProp, pAddSkillProp , false );							
	#endif //__WORLDSERVER
				
			}
			else
			{
	#ifdef __CLIENT
				PLAYSND( SND_PC_BOSS_STUN );
	#endif //__CLIENT
	#ifdef __WORLDSERVER				
				ItemProp *pSkillProp;
				AddSkillProp *pAddSkillProp;
				BOOL bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SI_BIGMUSCLE_STUNGROUP, 1, "OnAttackMelee_BigMuscle" );
				if( bSuccess )
					ApplySkillAround( this, OBJTYPE_PLAYER, pSkillProp, pAddSkillProp , false );							
	#endif //__WORLDSERVER
			}
		}
		break;
	case OBJSTA_ATK4:   // 땅치기 스킬 - 한손
		{
			static D3DXVECTOR3 v3OldPos;

			if( m_pActMover->m_nMotionHitCount == 0 )
			{
				v3OldPos = pHitObj->GetPos();			
			}
			else
			if( m_pActMover->m_nMotionHitCount == 1 )
			{
				D3DXMATRIX mMatrix;
				
				D3DXMatrixMultiply( &mMatrix, (pModel->GetMatrixBone(12)), &(GetMatrixWorld()) );
				
				D3DXVECTOR3 v2;
				v2.x = mMatrix._41;
				v2.y = mMatrix._42;
				v2.z = mMatrix._43;
				v2.y -= 5.0f;
				
			#ifdef __CLIENT
				// 땅치고 끝날때 이펙트 왼손
				CSfx *pSfx = CreateSfx( D3DDEVICE, XI_SKILL_CIRCLE_DUST, v2 );
				if( pSfx )
					pSfx->SetScale( D3DXVECTOR3( 9.0f, 9.0f, 9.0f ) );
				
				pSfx = CreateSfx( D3DDEVICE, XI_SKILL_MAG_EARTH_ROCKCRASH01, v3OldPos );
				if( pSfx )
					pSfx->SetScale( D3DXVECTOR3( 2.0f, 2.0f, 2.0f ) );
				
				PLAYSND( SND_PC_SKILLD_HITOFPENYA );
			#endif //__CLIENT

			#ifdef __WORLDSERVER
				ItemProp *pSkillProp;
				AddSkillProp *pAddSkillProp;
				BOOL bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SI_GEN_BLEEDING, 10, "OnAttackMelee_BigMuscle" );
				if( bSuccess )
					DoApplySkill( pHitObj, pSkillProp, pAddSkillProp );	
				
				GetWorld()->SendDamageAround( &(v3OldPos), AF_GENERIC | AF_CRITICAL , this, OBJTYPE_PLAYER, II_WEA_MOB_MONSTER4_ATK2
					, 4.0f );	
			#endif //__WORLDSERVER
				
					
			}
		}
		
		break;
	}
}

void	CMover::OnAttackMelee_Krrr( DWORD dwState, CMover *pHitObj )
{
	int		nItemID = GetActParam( 1 );	// 사용할 아이템 아이디

	CModelObject *pModel = (CModelObject *)m_pModel;
	
#ifdef __WORLDSERVER
	UpdateMatrix();		// 몸체의 특정부위에서 나는 데미지를 처리하기 위해선 서버에서도 매트릭스를 갱신해야한다.
	pModel->UpdateMatrixBone();		// 서버에서는 본애니메이션을 하지 않으므로 GetEventPos()를 사용할 일이 있을땐 업데이트를 해줘야 한다.
#endif // WorldServer
	
	switch( dwState )
	{
	case OBJSTA_ATK1:	
		{
	#ifdef __WORLDSERVER			
			OnDamage( nItemID, dwState, pHitObj );		
	#endif //__WORLDSERVER			
		}
		break;
	case OBJSTA_ATK2:	
		{
	#ifdef __WORLDSERVER
			int nRandom = xRandom(100);
			if( nRandom <= 50 )
			{
				ItemProp *pSkillProp;
				AddSkillProp *pAddSkillProp;
				BOOL bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SI_KRRR_POISON, 1, "OnSfxHit" );
				if( bSuccess )
				{
					pHitObj->ApplySkill( this, pSkillProp, pAddSkillProp );
				}
			}
			else
			{
				OnDamage( nItemID, dwState, pHitObj );		
			}
	#endif // WorldServer
		}
		break;
	case OBJSTA_ATK3:		// 두손으로 도끼 내려치기
		{
		#ifdef __CLIENT
			CSfx *pSfx = CreateSfx( D3DDEVICE, XI_NPCSP1DIRFIRESP, pHitObj->GetPos() );	// 이펙트를 발생 - 서버에선 생성 안됨.
			if( pSfx )
				pSfx->SetAngle( -GetAngle() );

			PLAYSND( SND_PC_BOSS_CRASH, &pHitObj->GetPos() );
		#endif //__CLIENT

		#ifdef __WORLDSERVER
			ItemProp *pSkillProp;
			AddSkillProp *pAddSkillProp;
			BOOL bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SI_GEN_KNOCK_BACK, 4, "OnAttackMelee_BigMuscle" );
			if( bSuccess )
				ApplySkillAround( this, OBJTYPE_PLAYER, pSkillProp, pAddSkillProp , false );							

			GetWorld()->SendDamageAround( &(pHitObj->GetPos()), AF_GENERIC, this, OBJTYPE_PLAYER, II_WEA_MOB_MONSTER4_ATK1
				, 7.0f );	
		#endif //__WORLDSERVER
		}
		break;
	case OBJSTA_ATK4:		// 주문을 외우고 플레이어들 한곳에 모인후 광역데미지 줌
		{
			static D3DXVECTOR3	  vLocal;
			if( m_pActMover->m_nMotionHitCount == 0 )
			{
		#ifdef __CLIENT
				PLAYSND( SND_PC_BOSS_SUMMONS );
		#endif //__CLIENT
				
				AngleToVectorXZ( &vLocal, GetAngle(), 5.0f );	// 때리는방향 5미터앞.
				vLocal += GetPos();

				FLOAT fHeight = GetWorld()->GetLandHeight( vLocal.x, vLocal.z );
				vLocal.y = fHeight;
				
		#ifdef __WORLDSERVER
				CPartyQuestProcessor* pProc	= CPartyQuestProcessor::GetInstance();
				CWorld* pWorld	= GetWorld();
				if( pWorld )
				{
					int nId	= pProc->PtInQuestRect( pWorld->GetID(), GetPos() );
					if( nId > -1 )
					{
						PPARTYQUESTELEM pElem	= pProc->GetPartyQuest( nId );
						PPARTYQUESTRECT pRect	= pProc->GetPartyQuestRect( nId );
						if( pElem && pRect )
#ifdef __LAYER_1015
							g_UserMng.ReplaceWorldArea( pElem->idParty, pRect->dwWorldId, pRect->dwWorldId, vLocal.x, vLocal.z, 0.5f, TRUE, GetLayer() );
#else	// __LAYER_1015
							g_UserMng.ReplaceWorldArea( pElem->idParty, pRect->dwWorldId, pRect->dwWorldId, vLocal.x, vLocal.z, 0.5f, TRUE );
#endif	// __LAYER_1015
					}
				}
		#endif //__WORLDSERVER
			}
			else
			if( m_pActMover->m_nMotionHitCount == 1 )		// 약 5초후에 스턴
			{	
		#ifdef __CLIENT
				PLAYSND( SND_PC_BOSS_STUN );
		#endif //__CLIENT
#ifdef __WORLDSERVER
				ItemProp *pSkillProp;
				AddSkillProp *pAddSkillProp;
				BOOL bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SI_KRRR_STUNGROUP, 1, "OnAttackMelee_Krrr" );
				if( bSuccess )
					ApplySkillAround( this, OBJTYPE_PLAYER, pSkillProp, pAddSkillProp , false );	
#endif //__WORLDSERVER
				
			}
			else
			if( m_pActMover->m_nMotionHitCount == 2 )
			{	
#ifdef __CLIENT
				D3DXVECTOR3 v3Pos = vLocal;
				CSfx *pSfx = CreateSfx( D3DDEVICE, XI_SKILL_ELE_MULTI_METEOSHOWER02, v3Pos );	// 이펙트를 발생 - 서버에선 생성 안됨.
				if( pSfx )
					pSfx->SetScale( D3DXVECTOR3( 0.8f, 0.8f, 0.8f ) );
				
				v3Pos.x += ( xRandomF( 12.0f ) + -6.0f );
				pSfx = CreateSfx( D3DDEVICE, XI_SKILL_ELE_MULTI_METEOSHOWER02, v3Pos );	// 이펙트를 발생 - 서버에선 생성 안됨.
				if( pSfx )
					pSfx->SetScale( D3DXVECTOR3( 0.8f, 0.8f, 0.8f ) );
				
				v3Pos = vLocal;
				v3Pos.z += ( xRandomF( 12.0f ) + -6.0f );
				pSfx = CreateSfx( D3DDEVICE, XI_SKILL_ELE_MULTI_METEOSHOWER02, v3Pos );	// 이펙트를 발생 - 서버에선 생성 안됨.
				if( pSfx )
					pSfx->SetScale( D3DXVECTOR3( 0.8f, 0.8f, 0.8f ) );
				
#endif //__CLIENT
				
			}
			else
			if( m_pActMover->m_nMotionHitCount == 3 )
			{	
		#ifdef __WORLDSERVER
				GetWorld()->SendDamageAround( &(pHitObj->GetPos()), AF_GENERIC, this, OBJTYPE_PLAYER, II_WEA_MOB_MONSTER4_ATK2
					, 5.0f );	
		#endif //__WORLDSERVER
				
			}
		}
		break;
	}
}

#if __VER >= 10 // __Y_DRAGON_FIRE
void	CMover::OnAttackMelee_Meteonyker( DWORD dwState, CMover *pHitObj )
{
	int		nItemID = GetActParam( 1 );	// 사용할 아이템 아이디

	CModelObject *pModel = (CModelObject *)m_pModel;
	
#ifdef __WORLDSERVER
	UpdateMatrix();		// 몸체의 특정부위에서 나는 데미지를 처리하기 위해선 서버에서도 매트릭스를 갱신해야한다.
	pModel->UpdateMatrixBone();		// 서버에서는 본애니메이션을 하지 않으므로 GetEventPos()를 사용할 일이 있을땐 업데이트를 해줘야 한다.
#endif // WorldServer
	
	switch( dwState )
	{
	case OBJSTA_ATK1:
		{
			if( IsValidObj(pHitObj) )
			{
				OnDamage( nItemID, dwState, pHitObj );		
			}
		}
		break;
	case OBJSTA_ATK3:		// 파이어볼 발사
		{
			if( m_pActMover->m_nMotionHitCount == 0 )
			{
				CMover *pTarget = pHitObj;

				D3DXVECTOR3 vPos;
				CModelObject *pModel = (CModelObject *)m_pModel;
				pModel->GetEventPos( &vPos, 0 );	// 용의 입 좌표.
				D3DXVec3TransformCoord( &vPos, &vPos, GetMatrixWorldPtr() );

				if( IsValidObj(pTarget) )
				{
					OnDamage( nItemID, dwState, pTarget, &pTarget->GetPos() );		// 타겟의 좌표를 중심으로 데미지를 줌.
					ItemProp *pSkillProp;
					AddSkillProp *pAddSkillProp;
					BOOL bFind	= GetSkillProp( &pSkillProp, &pAddSkillProp, SI_GEN_STUNSINGLE, 5, "SI_GEN_STUNSINGLE" );
					if( bFind )
					{
						if( xRand() % 2 )
							DoApplySkill( pTarget, pSkillProp, pAddSkillProp , false );
					}

	#ifdef __CLIENT
					CSfx *pSfx = CreateSfx( D3DDEVICE, XI_NPCSP1RANBALL, vPos, GetId(), pTarget->GetPos(), pTarget->GetId(), -1 );		// 포탄
					if( pSfx )
					{
						pSfx->SetScale( D3DXVECTOR3(4.0f, 4.0f, 4.0f) );
						{
							FLOAT fAngXZ, fAngH;
							D3DXVECTOR3 vDist = pTarget->GetPos() - vPos;
							xGetDegree( &fAngXZ, &fAngH, vDist );		// 목표와의 각도구함.
							((CSfxAtkStraight *)pSfx)->ShootSfx( fAngXZ, -fAngH, 2.0f, XI_NPCSP1RANBOOM );		// 날아가는 방향 설정.
						}
					}
	#endif	// __CLIENT
				}
			}
		}
		break;
	case OBJSTA_ATK4:   // 날개 접었다가 펼치기
		{
			if( m_pActMover->m_nMotionHitCount == 0 )
			{
				// 광역 효과
				ItemProp *pSkillProp;
				AddSkillProp *pAddSkillProp;
				BOOL bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SI_GEN_BURN, 1, "OnAttackMelee_Meteonyker" );
				if( bSuccess )
				{
					D3DXVECTOR3 vPos	= GetPos();
					ApplySkillAround( this, OBJTYPE_PLAYER, pSkillProp, pAddSkillProp, false, FALSE, 10.0F );
					MoverProp* pProp	= GetProp();
					OnDamage( pProp->dwAtk4, OBJSTA_ATK4, NULL, &vPos );
#ifdef __CLIENT
					CSfx *pSfx = CreateSfx( D3DDEVICE, XI_NPCMETEONYKER, vPos );
#endif	// __CLIENT
				}
			}
		}
		break;
	}
}
#endif //__Y_DRAGON_FIRE

void	CMover::OnAttackMelee_Bear( DWORD dwState, CMover *pHitObj )
{
	int		nItemID = GetActParam( 1 );	// 사용할 아이템 아이디

	CModelObject *pModel = (CModelObject *)m_pModel;
	
#ifdef __WORLDSERVER
	UpdateMatrix();		// 몸체의 특정부위에서 나는 데미지를 처리하기 위해선 서버에서도 매트릭스를 갱신해야한다.
	pModel->UpdateMatrixBone();		// 서버에서는 본애니메이션을 하지 않으므로 GetEventPos()를 사용할 일이 있을땐 업데이트를 해줘야 한다.
#endif // WorldServer
	
	switch( dwState )
	{
	case OBJSTA_ATK1:	
		{
	#ifdef __WORLDSERVER
			ItemProp *pSkillProp;
			AddSkillProp *pAddSkillProp;
			BOOL bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SI_GEN_ATKSPELL_RATEGROUP, 5, "OnAttackMelee_BigMuscle" );
			if( bSuccess )
				ApplySkillAround( this, OBJTYPE_PLAYER, pSkillProp, pAddSkillProp , false );			
			
			OnDamage( nItemID, dwState, pHitObj );					
	#endif // WorldServer
		}
		break;
	case OBJSTA_ATK2:	
		{
	#ifdef __WORLDSERVER
			OnDamage( nItemID, dwState, pHitObj );					
	#endif // WorldServer
		}
		break;
	case OBJSTA_ATK3:		
		{
			if( m_pActMover->m_nMotionHitCount == 0 )
			{				
	#ifdef __CLIENT
				CSfx *pSfx = CreateSfx( D3DDEVICE, XI_SKILL_CIRCLE_DUST, GetPos() );			
				if( pSfx )
					pSfx->SetScale( D3DXVECTOR3( 12.0f, 12.0f, 12.0f ) );
				
				PLAYSND( SND_PC_SKILLD_HITOFPENYA );//, &v2 );
	#endif //__CLIENT 			
				
	#ifdef __WORLDSERVER
				ItemProp *pSkillProp;
				AddSkillProp *pAddSkillProp;
				BOOL bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SI_GEN_KNOCK_BACK, 4, "OnAttackMelee_BigMuscle" );
				if( bSuccess )
					ApplySkillAround( this, OBJTYPE_PLAYER, pSkillProp, pAddSkillProp , false );
	#endif //__WORLDSERVER			
			}
			else
			if( m_pActMover->m_nMotionHitCount == 1 )
			{
	#ifdef __CLIENT
				PLAYSND( SND_PC_BOSS_STUN );
	#endif //__CLIENT
				
	#ifdef __WORLDSERVER
				ItemProp *pSkillProp;
				AddSkillProp *pAddSkillProp;
				BOOL bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SI_GEN_STUNGROUP, 5, "OnAttackMelee_Bear" );
				if( bSuccess )
					ApplySkillAround( this, OBJTYPE_PLAYER, pSkillProp, pAddSkillProp , false );				
	#endif //__WORLDSERVER			
			}
		}
		break;
	case OBJSTA_ATK4:	
		{	
#ifdef __WORLDSERVER
			ItemProp *pSkillProp;
			AddSkillProp *pAddSkillProp;
			BOOL bSuccess;
	
			// 자신 힐링
			bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SI_ASS_HEAL_HEALING, 1, "OnAttackMelee_BigMuscle" );
			if( bSuccess )
				DoApplySkill( this, pSkillProp, pAddSkillProp , false );

			//버서크
			bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SI_MUSHMOOT_BERSERK, 1, "OnAttackMelee_Bear" );
			if( bSuccess )
				DoApplySkill( this, pSkillProp, pAddSkillProp , false );

			// 우든스킨 
			bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SI_MUSHMOOT_WOODENSKIN, 1, "OnAttackMelee_Bear" );
			if( bSuccess )
				DoApplySkill( this, pSkillProp, pAddSkillProp , false );
			
			// 힘업
			bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SI_ASS_CHEER_HEAPUP, 1, "OnAttackMelee_BigMuscle" );
			if( bSuccess )
				DoApplySkill( this, pSkillProp, pAddSkillProp , false );

			//캐논볼
			bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SI_ASS_CHEER_CANNONBALL, 1, "OnAttackMelee_BigMuscle" );
			if( bSuccess )
				DoApplySkill( this, pSkillProp, pAddSkillProp , false );

			//멘탈사인
			bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SI_ASS_CHEER_MENTALSIGN, 1, "OnAttackMelee_BigMuscle" );
			if( bSuccess )
				DoApplySkill( this, pSkillProp, pAddSkillProp , false );

			//비프업
			bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SI_ASS_CHEER_BEEFUP, 1, "OnAttackMelee_BigMuscle" );
			if( bSuccess )
				DoApplySkill( this, pSkillProp, pAddSkillProp , false );
			
			
			//캐츠플렉스
			bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SI_ASS_CHEER_CATSREFLEX, 1, "OnAttackMelee_BigMuscle" );
			if( bSuccess )
				DoApplySkill( this, pSkillProp, pAddSkillProp , false );

			//어큐러시
			bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SI_ASS_CHEER_ACCURACY, 1, "OnAttackMelee_BigMuscle" );
			if( bSuccess )
				DoApplySkill( this, pSkillProp, pAddSkillProp , false );

			//해이스트
			bSuccess = GetSkillProp( &pSkillProp, &pAddSkillProp, SI_ASS_CHEER_HASTE, 1, "OnAttackMelee_BigMuscle" );
			if( bSuccess )
				DoApplySkill( this, pSkillProp, pAddSkillProp , false );
			

			// 보스몬스터가 점점 커진다.
			D3DXVECTOR3 vScale = GetScale();

			vScale *= 1.3f;

			if( vScale.x < 10.0f )
			{
				SetScale( vScale );
				g_UserMng.AddSetScale( this, (vScale.x / m_pModel->m_pModelElem->m_fScale)  );
			}
			
#endif //__WORLDSERVER			
		}
		break;
	}
}
// 데미지를 적용하는 부분. - 공격방식에 따라서 타점한번에 어러번 호출될 수도 있다.
// pHitObj : 데미지를 먹을 대상. NULL이면 vDamagePos의 좌표를 중심으로 데미지를 준다.
BOOL	CMover::OnDamage( int nItemID, DWORD dwState, CMover *pHitObj, const D3DXVECTOR3 *pvDamagePos, DWORD dwAtkFlag )
{
	int nParam = 0x01;		//sun

	if( IsPlayer() && IsDualWeapon() )		// 쌍칼모드냐?
	{
		if( m_pActMover->m_dwAtkFlags & AF_GENERIC )	// 평타면
		{
			switch( dwState )
			{
			case OBJSTA_ATK1:	nParam |= 0x01;		break; // 오른손
			case OBJSTA_ATK2:	nParam |= 0x02;		break; // 왼손
			case OBJSTA_ATK3:	nParam |= 0x01;		break; // 오른손
			case OBJSTA_ATK4:	nParam |= 0x03;		break; // 양손
			}
		}
	}


#if defined(__CLIENT)
	if( IsValidObj(pHitObj) )
	{
		CModelObject* pHitModel = (CModelObject*)pHitObj->m_pModel;
		if( pHitModel )
		{
			D3DXVECTOR3 vDir;
			pHitModel->m_SparkInfo.m_bUsed = TRUE;
			pHitModel->m_SparkInfo.m_v3Color.x = 2.0f;
			pHitModel->m_SparkInfo.m_v3Color.y = 2.0f;
			pHitModel->m_SparkInfo.m_v3Color.z = 2.0f;
			pHitModel->m_SparkInfo.m_nCnt = 0;
			pHitModel->m_SparkInfo.m_fLerp= 1.0f;

			vDir = pHitObj->GetPos() - GetPos();
			D3DXVec3Normalize( &pHitModel->m_SparkInfo.m_v3SparkDir, &vDir );
		}
	}
#endif //defined(__CLIENT) 
	
	int	nDamage = 0;
	if( IsPlayer() )
	{
		// 플레이어
		nDamage = pHitObj->m_pActMover->SendDamage( m_pActMover->m_dwAtkFlags, GetId(), nParam );
	}
	else
	{
		// 몬스터
		ItemProp *pItemProp = prj.GetItemProp( nItemID );
		if( pItemProp )
		{
			if( pItemProp->dwSpellRegion == SRO_AROUND )
			{
				int nType = OBJTYPE_PLAYER;		// 플레이어들만 데미지.
				if( pHitObj )
					pHitObj->SendDamageAround( dwAtkFlag, this, nType, nItemID, (float)pItemProp->_dwSkillRange, 1.0f, 1.0f ); 
				else
					GetWorld()->SendDamageAround( pvDamagePos, dwAtkFlag, this, nType, nItemID, (float)pItemProp->_dwSkillRange ); 
			} else
			{
				if( pHitObj )
					nDamage = pHitObj->m_pActMover->SendDamage( m_pActMover->m_dwAtkFlags, GetId(), 1 );
			}
		} else
		{
			if( pHitObj )
				nDamage = pHitObj->m_pActMover->SendDamage( m_pActMover->m_dwAtkFlags, GetId(), 1 );
		}
	}


 #ifdef __WORLDSERVER
	if( nDamage > 0 )		// THIS가 공격했는데 데미지 먹이는데 성공했다
	{
		if( pHitObj->m_pActMover->GetState() & OBJSTA_ATK_MELEESKILL )	// 맞는놈이 스킬 모션중이었다.
		{
			DWORD dwSkill = pHitObj->GetActParam(0);		// 현재 사용하고 있는 스킬. dwSkill
			if( dwSkill == SI_MER_ONE_REFLEXHIT )			// 리플렉히트 스킬을 쓰고 있는 중에 맞았다.
			{
				pHitObj->m_nReflexDmg += nDamage;			// 리플렉데미지는 쌓임.
			}
		}
	}
 #endif
	return nDamage > 0;
}

//
// 일반 공격의 타점때 호출되는 핸들러.
BOOL	CMover::OnAttackMelee( DWORD dwState, CMover *pHitObj )
{
	CModelObject	*pModel = (CModelObject *)m_pModel;
	int		nItemID = GetActParam( 1 );	// 사용할 아이템 아이디

#ifdef __WORLDSERVER
	PostAIMsg( AIMSG_ATTACK_MELEE );
#endif
	
 	if( m_dwIndex == MI_CLOCKWORK1 )	// 클락워크의 경우 때릴때 이펙트.
	{
		OnAttackMelee_ClockWorks( dwState, pHitObj );
		return TRUE;
	}
	
	if( m_dwIndex == MI_BIGMUSCLE )	// 빅머슬의 경우 때릴때 이펙트.
	{
		OnAttackMelee_BigMuscle( dwState, pHitObj );
		m_pActMover->m_nMotionHitCount++;		
		return TRUE;
	}

	if( m_dwIndex == MI_KRRR )	// 크르릉의 경우 때릴때 이펙트.
	{
		OnAttackMelee_Krrr( dwState, pHitObj );
		m_pActMover->m_nMotionHitCount++;		
		return TRUE;
	}	

	if( m_dwIndex == MI_MUSHMOOT )	// 크르릉의 경우 때릴때 이펙트.
	{
		OnAttackMelee_Bear( dwState, pHitObj );
		m_pActMover->m_nMotionHitCount++;		
		return TRUE;
	}	

#if __VER >= 10 // __Y_DRAGON_FIRE
	if( m_dwIndex == MI_DU_METEONYKER || m_dwIndex == MI_DU_METEONYKER2 || m_dwIndex == MI_DU_METEONYKER3 ||
		m_dwIndex == MI_DU_METEONYKER4 )
	{
		OnAttackMelee_Meteonyker( dwState, pHitObj );
		m_pActMover->m_nMotionHitCount++;		
		return TRUE;
	}
#endif //__Y_DRAGON_FIRE

#ifdef __WORLDSERVER
#if __VER >= 12 // __MONSTER_SKILL
	if( IsNPC() && CMonsterSkill::GetInstance()->ApplySkill( this, pHitObj, ATK_MELEE ) )
	{
		//m_pActMover->m_nMotionHitCount++;
		return TRUE;
	}
#endif // __MONSTER_SKILL
#endif // __WORLDSERVER

	BOOL bSuccess = FALSE;
	BOOL bDamage = TRUE;

#ifdef __WORLDSERVER
	if( IsNPC() && pHitObj->IsPlayer() && pHitObj->IsRangeObj( this, 10.0f ) == FALSE )		// 두번죽는 경우땜에 넣어봤다. 몹->유저 10미터 이상 떨어져있을때는 안맞게.
		bDamage = FALSE;
#endif
	if( bDamage )
	{
		bSuccess = OnDamage( nItemID, dwState, pHitObj );	// 데미지를 준다.
	}

#ifdef __WORLDSERVER
	if( bSuccess )
	{
		if( IsPlayer() )
		{
			if( dwState == OBJSTA_ATK1 && ((CUser*)this)->m_playTaskBar.m_nActionPoint < 100 )	// 2타째 치는건 액션포인트 올라감.
				((CUser*)this)->m_playTaskBar.m_nActionPoint ++;
		}
	}
#endif	// worldserver

#ifdef __CLIENT
	if( bSuccess )		// 타격에 성공했다면 타격음을 플레이.
	{
		if( CMover::GetActiveMover() == this )
		{
			if( dwState == OBJSTA_ATK1 && g_WndMng.m_pWndTaskBar->m_nActionPoint < 100 )	// 2타째 치는건 액션포인트 올라감.
				g_WndMng.m_pWndTaskBar->m_nActionPoint ++;
		}
		//  플레이어는 들고 있는 무기에 따라 타격 음이 다르다.
		ItemProp* pItemProp = GetActiveHandItemProp();
		if( pItemProp )
		{
/*		#ifndef __Y_DRAGON_FIRE
			if( pItemProp->dwSfxObj2 != NULL_ID )
			{
				CSfx *pSfx = CreateSfx( D3DDEVICE, pItemProp->dwSfxObj2,  GetPos(), GetId(), this->GetPos(), this->GetId(), 0 ); 
			}
		#endif //__Y_DRAGON_FIRE
*/
			PLAYSND( pItemProp->dwSndAttack1, &GetPos() );
		}
		
		if( IsPlayer() )
		{
			pModel->m_nPause = 5;	// frame 멈춤
		}
		else
		{
			pModel->m_nPause = 0;	// 몬스터는 멈추지 않음
			pHitObj->m_pModel->m_nPause = 0;
		}

#if defined(__CLIENT)
		{
			if( IsValidObj( pHitObj ) )
			{
				pModel->m_SparkInfo.m_bUsed  = TRUE;
				pModel->m_SparkInfo.m_v3Color.x = 2.0f;
				pModel->m_SparkInfo.m_v3Color.y = 2.0f;
				pModel->m_SparkInfo.m_v3Color.z = 2.0f;
				pModel->m_SparkInfo.m_nCnt = 0;
				pModel->m_SparkInfo.m_fLerp= 1.0f;
				
				D3DXVECTOR3 vDir = GetPos() - pHitObj->GetPos();
				D3DXVec3Normalize( &pModel->m_SparkInfo.m_v3SparkDir, &vDir );
			}
		}
#endif //defined(__CLIENT)
		
		
	}
#endif // client
	
	
	return TRUE;
}

//
//
// 타점 이후부터 끝날때까지 계속 호출.
BOOL	CMover::OnAttackMeleeContinue( DWORD dwState )
{
	int		nItemID = GetActParam( 1 );	// 사용할 아이템 아이디

	if( m_dwIndex == MI_CLOCKWORK1 )	// 클락워크의 경우 때릴때 이펙트.
	{
	#ifdef __WORLDSERVER
		UpdateMatrix();		// 몸체의 특정부위에서 나는 데미지를 처리하기 위해선 서버에서도 매트릭스를 갱신해야한다.
		CModelObject *pModel = (CModelObject *)m_pModel;
		pModel->UpdateMatrixBone();		// 서버에서는 본애니메이션을 하지 않으므로 GetEventPos()를 사용할 일이 있을땐 업데이트를 해줘야 한다.
	#endif // WorldServer

		switch( dwState )
		{
		case OBJSTA_ATK3:
			{
				D3DXVECTOR3 v1, v2;
				CModelObject *pModel = (CModelObject *)m_pModel;
				pModel->GetEventPos( &v1, 2 );		// 레이저 나가는 부분.
				D3DXVec3TransformCoord( &v1, &v1, GetMatrixWorldPtr() );	// 월드공간으로 변환.
				
				v2 = D3DXVECTOR3( 0, 0, 40.0f );
				D3DXVec3TransformCoord( &v2, &v2, pModel->GetMatrixBone(6) );	// 6이 머리.  머리에서 v2지점의 위치를 계산.
				D3DXVec3TransformCoord( &v2, &v2, GetMatrixWorldPtr() );	// 월드 공간으로 변환.
				
		#ifdef __CLIENT
				extern CPartsLaser g_Laser;
				g_Laser.SetPos( v1, v2, 0.5f );
		#endif

		#ifdef __CLIENT
				if( (m_pActMover->m_nCount & 1) == 0 )
		#else				
				if( (m_pActMover->m_nCount & 3) == 0 )
		#endif
				{
					CWorld *pWorld = GetWorld();
					D3DXVECTOR3 vOut;
					if( pWorld->IntersectObjLine( &vOut, v1, v2, FALSE, TRUE, FALSE ) )
					{
						OnDamage( nItemID, dwState, NULL, &vOut );		// 이펙트가 터지는 위치에 데미지를 줌.

					#ifdef __CLIENT
						CSfx *pSfx = CreateSfx( D3DDEVICE,  XI_NPCSP1RANSPARK, vOut );
						if( pSfx )
							pSfx->SetScale( D3DXVECTOR3( 2.0f, 2.0f, 2.0f ) );
					#endif // __CLIENT
					}
						
				}
			}
			break;
		}
	}
#if __VER >= 10 // __Y_DRAGON_FIRE
	else
	if( m_dwIndex == MI_DU_METEONYKER || m_dwIndex == MI_DU_METEONYKER2 || m_dwIndex == MI_DU_METEONYKER3 ||
		m_dwIndex == MI_DU_METEONYKER4 )
	{
	#ifdef __WORLDSERVER
		UpdateMatrix();		// 몸체의 특정부위에서 나는 데미지를 처리하기 위해선 서버에서도 매트릭스를 갱신해야한다.
		CModelObject *pModel = (CModelObject *)m_pModel;
		pModel->UpdateMatrixBone();		// 서버에서는 본애니메이션을 하지 않으므로 GetEventPos()를 사용할 일이 있을땐 업데이트를 해줘야 한다.
	#endif // WorldServer

		switch( dwState )
		{
			case OBJSTA_ATK2:
			{
				if( m_pActMover->m_nMotionHitCount == 1 )
				{
					D3DXVECTOR3 v1, v2, v3;
					CModelObject *pModel = (CModelObject *)m_pModel;
					pModel->GetEventPos( &v1, 0 );		// 레이저 나가는 부분.
					pModel->GetEventPos( &v3, 1 );		// 레이저 나가는 부분.
					D3DXVec3TransformCoord( &v1, &v1, GetMatrixWorldPtr() );	// 월드공간으로 변환.
					D3DXVec3TransformCoord( &v3, &v3, GetMatrixWorldPtr() );	// 월드공간으로 변환.

					v2 = D3DXVECTOR3( 0, 0, 40.0f );
					D3DXVec3TransformCoord( &v2, &v2, pModel->GetMatrixBone(6) );	// 6이 머리.  머리에서 v2지점의 위치를 계산.
					D3DXVec3TransformCoord( &v2, &v2, GetMatrixWorldPtr() );	// 월드 공간으로 변환.
					
			#ifdef __CLIENT
					D3DXVECTOR3 vDir = v3 - v1;
					D3DXVec3Normalize( &vDir, &vDir );
					D3DXVECTOR3 vTemp;
					D3DXVECTOR3 vScal = D3DXVECTOR3( 0.3f, 0.3f, 0.3f );
					D3DXVECTOR3 vModelScal = GetScale();

					vScal.x *= vModelScal.x;
					vScal.y *= vModelScal.y;
					vScal.z *= vModelScal.z;

					for( int i = 0; i < 2; i ++ )
					{
						vTemp = vDir * (xRandomF( 0.2f ) + 0.1f );
						vTemp *= 0.8f;
						vTemp.y -= (xRandomF( 0.05f ) + 0.01f );
						
						extern CPartsFireDragon	g_FireDragon;
						g_FireDragon.Create( D3DDEVICE, v1, XI_NAT_FIRE01_ADV, vScal, vTemp );
					}

			#endif

			#ifdef __CLIENT
					if( (m_pActMover->m_nCount & 1) == 0 )
			#else				
					if( (m_pActMover->m_nCount & 3) == 0 )
			#endif
					{
						CWorld *pWorld = GetWorld();
						D3DXVECTOR3 vOut	= GetPos();
//						if( pWorld->IntersectObjLine( &vOut, v1, v2, FALSE, TRUE, FALSE ) )
						{
							OnDamage( nItemID, dwState, NULL, &vOut );		// 이펙트가 터지는 위치에 데미지를 줌.
	/*
						#ifdef __CLIENT
							CSfx *pSfx = CreateSfx( D3DDEVICE,  XI_NPCSP1RANSPARK, vOut );
							if( pSfx )
								pSfx->SetScale( D3DXVECTOR3( 2.0f, 2.0f, 2.0f ) );
						#endif // __CLIENT
	*/
						}
							
					}
				}
				break;
			}
		}
	}
#endif //__Y_DRAGON_FIRE

	return TRUE;
}

// 몬스터의 특수공격 타격부분에 호출
BOOL	CMover::OnAttackSP()
{
	int		idTarget = GetActParam( 0 );		
	int		nItemID = GetActParam( 1 );	// 사용할 아이템 아이디
	
	CMover* pHit = prj.GetMover( idTarget );	// 타겟의 포인터를 얻어냄.
	if( IsInvalidObj(pHit) )	return FALSE;		// 클릭했을당시는 있었으나 발사되기전에 삭제되었다.

	ItemProp *pItemProp;
	if( nItemID == NULL_ID || nItemID == 0 )	
		pItemProp = GetActiveHandItemProp();	// 이런경우는 걍 손에 든 무기(혹은 기본아이템)
	else
		pItemProp = prj.GetItemProp( nItemID );	// 직접 아이템을 지정한 경우는 그걸로 공격함(몬스터가 주로 사용함)

	if( pItemProp == NULL )
	{
		Error( "CMover::OnAttackSP : %s의 itemProp이 없다. %d", m_szName, nItemID );
		return FALSE;
	}

	if( pItemProp->dwSpellRegion == SRO_AROUND )
	{	
		SendDamageAround( AF_GENERIC, this, OBJTYPE_PLAYER, nItemID, (float)( pItemProp->_dwSkillRange ), 0.0, 1.0f );	// 범위공격스따일
	} else
	{
#ifdef __CLIENT
		if( pHit->IsLive() )
			pHit->m_pActMover->SendDamage( AF_MONSTER_SP_CLIENT, GetId(), (nItemID << 16) );	// 09_12_21 몬스터 스페셜 공격
#else
		if( pHit->IsLive() )
			pHit->m_pActMover->SendDamage( AF_GENERIC, GetId(), (nItemID << 16) );	// 직접 공격 스따일.
#endif
	}

	return TRUE;
}                                                         

//
//	캐릭터가 높은곳에서 떨어진 시점에 호출
//
void	CMover::OnActDrop()
{
#ifdef __WORLDSERVER
	if( m_pActMover->GetDmgState() & OBJSTA_DMG_FLY_ALL )		// 데미지 플라이땐 이거 안하는게 좋다.
		return;

	if( FALSE == IsPlayer() )
	{
		if( m_nCorr < 0
#if __VER >= 9	//__AI_0509
			&& GetSpeedFactor() < 1.9F
#endif	// __AI_0509
#ifdef __JEFF_11
			&& m_dwAIInterface != AII_PET
#endif	//	__JEFF_11
			)
		{
			ClearDestPos();
			SendActMsg( OBJMSG_STAND );
			if( m_pActMover->IsFly() )
				g_UserMng.AddMoverCorr2( this, GetPos(), m_pActMover->m_vDelta, GetAngle(), GetAngleX(),
					m_pActMover->m_fAccPower, m_pActMover->m_fTurnAngle, m_pActMover->GetState(), m_pActMover->GetStateFlag(),
					m_dwMotion, m_pActMover->m_nMotionEx, m_pModel->m_nLoop, m_dwMotionOption, g_TickCount.GetTickCount() );
			else
				g_UserMng.AddMoverCorr( this, GetPos(), m_pActMover->m_vDelta, GetAngle(),
					m_pActMover->GetState(), m_pActMover->GetStateFlag(), m_dwMotion, m_pActMover->m_nMotionEx,
					m_pModel->m_nLoop, m_dwMotionOption, g_TickCount.GetTickCount() );
			m_nCorr		= 0;
		}
	}
	else
	{
		if( FALSE == IsEmptyDest() && FALSE == m_fWaitQueryGetPos )
		{
			( (CUser*)this )->AddQueryGetPos( NULL_ID );
			m_fWaitQueryGetPos	= TRUE;
		}
	}
#else	// __WORLDSERVER

	if( IsActiveMover() )
	{
		if( m_nCorr != 0 )
		{
			if( m_nCorr < 0 ) {
				ClearDest();
				SendActMsg( OBJMSG_STAND );
			}
			if( m_pActMover->IsFly() )
				g_DPlay.SendPlayerCorr2();
			else
				g_DPlay.SendPlayerCorr();
			m_nCorr		= 0;
		}
	}
	else
	{
#if __VER >= 9	//__AI_0509
		if( IsPlayer() || GetSpeedFactor() < 1.9F )
#endif	// __AI_0509
		{
			if( FALSE == IsEmptyDest() && FALSE == m_fWaitQueryGetPos )
			{
				g_DPlay.SendQueryGetPos( this );
				m_fWaitQueryGetPos	= TRUE;
			}
		}
	}
#endif	// not __WORLDSERVER
}

void	CMover::OnActCollision()
{
#ifdef __WORLDSERVER
	if( FALSE == IsPlayer() )
	{
		if( m_nCorr < 0 
#if __VER >= 9	//__AI_0509
			&& GetSpeedFactor() < 1.9F && m_dwAIInterface != AII_PET
#endif	// __AI_0509
			)
		{

			ClearDest();
			SendActMsg( OBJMSG_STAND );

			if( m_pActMover->IsFly() )
				g_UserMng.AddMoverCorr2( this, GetPos(), m_pActMover->m_vDelta, GetAngle(), GetAngleX(),
					m_pActMover->m_fAccPower, m_pActMover->m_fTurnAngle, m_pActMover->GetState(), m_pActMover->GetStateFlag(),
					m_dwMotion, m_pActMover->m_nMotionEx, m_pModel->m_nLoop, m_dwMotionOption, g_TickCount.GetTickCount() );
			else
				g_UserMng.AddMoverCorr( this, GetPos(), m_pActMover->m_vDelta, GetAngle(),
					m_pActMover->GetState(), m_pActMover->GetStateFlag(), m_dwMotion, m_pActMover->m_nMotionEx,
					m_pModel->m_nLoop, m_dwMotionOption, g_TickCount.GetTickCount() );

			m_nCorr		= 0;

			PostAIMsg( AIMSG_COLLISION );
		}
	}
	else
	{
		if( FALSE == IsEmptyDest() && FALSE == m_fWaitQueryGetPos )
		{
			( (CUser*)this )->AddQueryGetPos( NULL_ID );
			m_fWaitQueryGetPos	= TRUE;
		}
	}

#else	// __WORLDSERVER
	if( TRUE == IsActiveMover() )
	{
		if( m_nCorr != 0 )
		{
			if( m_nCorr < 0 )
			{
				ClearDest();
				SendActMsg( OBJMSG_STAND );
				CWndWorld* pWndWorld	= (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
				if( pWndWorld )
					pWndWorld->m_bLButtonDown	= FALSE;
			}
			if( m_pActMover->IsFly() )
				g_DPlay.SendPlayerCorr2();
			else
				g_DPlay.SendPlayerCorr();

			m_nCorr	= 0;
		}
	}
	else if( FALSE == IsActiveMover() )
	{
#if __VER >= 9	//__AI_0509
		if( IsPlayer() || GetSpeedFactor() < 1.9F )
#endif	// __AI_0509
		{
			if( FALSE == IsEmptyDest() && FALSE == m_fWaitQueryGetPos )
			{
				g_DPlay.SendQueryGetPos( this );
				m_fWaitQueryGetPos	= TRUE;
			}
		}
	}

#endif	// __WORLDSERVER
}

// 근접스킬 동작 끝났을때.
void	CMover::OnActEndMeleeSkill()
{
	ClearActParam();			// 행동상태 클리어
	m_dwFlag &= (~MVRF_SKILL);
#ifdef __WORLDSERVER
	if( IsPlayer() )
	{
		if( ((CUser *)this)->m_playTaskBar.m_nUsedSkillQueue != -1 )
		{
			((CUser *)this)->m_playTaskBar.SetNextSkill( (CUser *)this );
		}
	}
#endif 

}

// 마법스킬 동작 끝났을때.
void	CMover::OnActEndMagicSkill()
{
	ClearActParam();			// 행동상태 클리어
	m_dwFlag &= (~MVRF_SKILL);
 #ifdef __WORLDSERVER
	if( IsPlayer() )
	{
		if( ((CUser *)this)->m_playTaskBar.m_nUsedSkillQueue != -1 )
		{
			((CUser *)this)->m_playTaskBar.SetNextSkill( (CUser *)this );
		}
	}
 #endif 
}

// 착지(JUMP4)플래그가 사라지는 순간.
void	CMover::OnActEndJump4()
{
	if( FALSE == IsEmptyDestPos() )
		SetDestPos( GetDestPos(), m_bForward, FALSE );

	if( m_dwIndex == MI_CLOCKWORK1 )
	{
		SendActMsg( OBJMSG_APPEAR );
	}
}

// 착지 하는 순간에 발생.
void	CMover::OnActLanding()
{
#ifdef __CLIENT	
	if( m_dwIndex == MI_CLOCKWORK1 )
	{
		GetWorld()->m_pCamera->SetQuake( 60, 1.0f );		// 보스가 떨어지면 땅이 흔들림.
	}
#endif
}

// 전진 하는 순간에 발생.
void	CMover::OnActFMove()
{
#ifdef __WORLDSERVER
	PostAIMsg( AIMSG_BEGINMOVE );
#endif
}

// Interact 오브젝트에 착지 했을때.
// pIAObj : 어떤 IA오브젝트에 착지했는가.
// vPos : 착지한 this의 좌표.
void	CMover::OnActIALanding( CObj *pIAObj, const D3DXVECTOR3 &vPos )
{
	if( pIAObj->GetType() != OT_SHIP )	
		return;
	CShip *pIA = (CShip *)pIAObj;

	CObj *pOldIA = GetIAObjLink();
	SetIAObjLink( pIA );
	
	if( IsActiveMover() )
	{
		if( pIA != pOldIA )	// 걸어다닐때도 계속 갱신되기땜에 매번 보낼필요는 없다.
		{
#ifdef __CLIENT			
			D3DXVECTOR3 vLocal = GetPos() - pIA->GetPos();		// IA오브젝트로부터의 상대 좌표를 뽑음.
			g_DPlay.SendLocalPosFromIA( vLocal, pIA->GetId() );				// 상대좌표 서버로 전송
			TRACE( "SendLocalPosFromIA\n" );
#endif //__CLIENT
		}
	}

}

/*
int		CMover::OnActCollecting() 
{ 
#ifdef __CLIENT
	if( (m_dwFlag & MVRF_COLLECT) == 0 )
	{
		ItemProp *pHandProp = GetActiveHandItemProp();
		if( pHandProp->dwSfxObj2 != NULL_ID )
		{
			D3DXVECTOR3 vSrc, vLocal = D3DXVECTOR3(0, 0.5f, 0);
			((CModelObject *)m_pModel)->GetForcePos( &vLocal, 0, PARTS_RWEAPON, GetMatrixWorld() );
			vSrc = vLocal;
			CSfx *pSfx = CreateSfx( D3DDEVICE, pHandProp->dwSfxObj2, vSrc, GetId(), D3DXVECTOR3(0,0,0), NULL_ID, -1 );
			if( pSfx )
			{
				pSfx->SetAngle( -GetAngle() + 90.0f );
				m_dwFlag |= MVRF_COLLECT;		// SFX가 생성됐음.
			}
		}
	}
#endif // Client
	return 0; 
}				
*/

// 공격을 막 시작했을때(일반/스킬/완드 등)
void	CMover::OnAttackStart( CMover *pTarget, OBJMSG dwMsg )
{
}

// this가 투명화 상태 걸려있으면 해제시킴.
void	CMover::RemoveInvisible()
{
#ifdef __WORLDSERVER

	if( GetAdjParam( DST_CHRSTATE ) & CHS_INVISIBILITY )		// 투명화 상태다.
	{
		ResetDestParam( DST_CHRSTATE, CHS_INVISIBILITY, TRUE );
	}

	RemoveBuff( BUFF_SKILL, SI_ACR_SUP_DARKILLUSION );		// 해제.				

#endif // __WORLDSERVER
}


// pItem이 루팅가능한가를 판별.
#ifdef __WORLDSERVER
BOOL CMover::IsLoot( CItem *pItem, BOOL bPet )
{
	BOOL bTake	= FALSE;
	
	if( pItem->IsDelete() || ( pItem->m_idHolder != 0 && pItem->m_idHolder != m_idPlayer ) )
	{
		return bTake;
	}

	if( pItem->m_idOwn != NULL_ID )		// 아이템에 주인이 있을때
	{
		// 일반적으로 떨어진 아이템.
		CMover *pOther = prj.GetMover( pItem->m_idOwn );		// pItem을 집을 권한이 있는사람.(가령...막타)
		if( IsValidObj(pOther) )
		{
			if( m_idparty && (pOther->m_idparty == m_idparty) )		// 파티가 있고 나랑 같은 파티원이면 줏을수 있어요~~~ ㅋㅋㅋ
				bTake = TRUE;
		} else
			bTake = TRUE;		// idOwn이 Invalid상태면 아무나 줏을 수 있음.
		
		if( pItem->m_idOwn == GetId() )		// 아이템의 소유권자면 집을 수 있다.
			bTake = TRUE;
		
	} else
		bTake = TRUE;	// 주인이 없는거면 아무나 줏을수 있다.

	DWORD dwTime	= g_tmCurrent;
#ifdef __EVENT_MONSTER
	DWORD dwMonsterId = pItem->m_IdEventMonster;
	if( dwMonsterId != NULL_ID && CEventMonster::GetInstance()->SetEventMonster( dwMonsterId ) )
	{
		DWORD dwLootTime = CEventMonster::GetInstance()->GetLootTime();
		if( (dwTime - pItem->m_dwDropTime) >= (DWORD)( SEC(dwLootTime) ) )
			bTake = TRUE;
		
		if( bPet && !CEventMonster::GetInstance()->IsPetAble() )
			bTake = FALSE;
	}
	else
#endif // __EVENT_MONSTER
#if __VER >= 9 // __S_9_ADD
	if( (dwTime - pItem->m_dwDropTime) >= SEC(7) )	// 7초가 지난건 아무나 줏을 수 있다.
#else // __S_9_ADD
	if( (dwTime - pItem->m_dwDropTime) >= SEC(40) )	// 40초가 지난건 아무나 줏을 수 있다.
#endif // __S_9_ADD
		bTake = TRUE;

	// 펫은 리스폰된 아이템을 집을수 없음.
	if( bPet && bTake )
	{
		if( 0 < ((CCtrl*)pItem)->m_lRespawn )
			bTake = FALSE;
#ifdef __JEFF_11
		// 소지품이 가득 차서 해당 아이템을 획득할 수 없다
		CItemElem* pItemElem	= (CItemElem*)pItem->m_pItemBase;
		ItemProp* pItemProp		= pItemElem->GetProp();
		if( pItemProp->dwItemKind3 != IK3_GOLD && m_Inventory.IsFull( pItemElem, pItemProp, pItemElem->m_nItemNum ) )
			bTake	= FALSE;
#endif	// __JEFF_11
	}

	return bTake;
}
#endif // WorldServer

// DoLoot의 하위 - 몹이 떨어트린 아이템을 솔로잉일때 집으려 할때 처리부
BOOL CMover::SubLootDropMobSingle( CItem *pItem )
{
	BOOL bSuccess = TRUE;
#ifdef __WORLDSERVER	
	CItemBase* pItemBase = pItem->m_pItemBase;
	bSuccess	= CreateItem( pItemBase );
	
	if( IsPlayer() && pItemBase->GetProp() != NULL )
	{
		if( bSuccess )
		{
			((CUser*)this)->AddDefinedText( TID_GAME_REAPITEM, "\"%s\"", pItemBase->GetProp()->szName );

			if( IsLootSendText( pItemBase->GetProp() ) )
				((CUser*)this)->AddChatText( TID_GAME_REAPITEM, "\"%s\"", pItemBase->GetProp()->szName );									
			
		}
		else
			((CUser*)this)->AddDefinedText( TID_GAME_LACKSPACE );
	}
	
	if( ( (CItemElem*)pItemBase )->IsLogable() && bSuccess )
	{
		switch( pItemBase->GetProp()->dwItemKind2 ) 
		{
		case IK2_FOOD:
		case IK2_MAGIC:
		case IK2_POTION:
		case IK2_REFRESHER:
			{
				if( ( (CItemElem*)pItemBase )->m_nItemNum > 9 )
				{
					LogItemInfo aLogItem;
					aLogItem.Action = "R";
					aLogItem.SendName = "GROUND";
					aLogItem.RecvName = GetName();
					aLogItem.WorldId = GetWorld()->GetID();
					aLogItem.Gold = aLogItem.Gold2 = GetGold();
					g_DPSrvr.OnLogItem( aLogItem, ( (CItemElem*)pItemBase ), ( (CItemElem*)pItemBase )->m_nItemNum );
				}
				break;
			}
		default:
			{
				LogItemInfo aLogItem;
				aLogItem.Action = "R";
				aLogItem.SendName = "GROUND";
				aLogItem.RecvName = GetName();
				aLogItem.WorldId = GetWorld()->GetID();
				aLogItem.Gold = aLogItem.Gold2 = GetGold();
				g_DPSrvr.OnLogItem( aLogItem, ( (CItemElem*)pItemBase ), ( (CItemElem*)pItemBase )->m_nItemNum );
				break;
			}
		}
	}

#endif // WorldServer
	return bSuccess;
} // SubLootDropMobSingle

// DoLoot의 하위 - 몹이 떨어트린 아이템을 파티가 있을때 집으려 할때 처리부
BOOL CMover::SubLootDropMobParty( CItem *pItem, CParty *pParty )
{
	BOOL bSuccess = TRUE;;
#ifdef __WORLDSERVER
	CUser *pListMember[ 8 ];
	int	nMaxListMember = 0;
	CUser* pMember	= NULL;
	CUser* pGetUser = NULL;
//	float fDist;
	D3DXVECTOR3	vDist;
	CItemBase* pItemBase = pItem->m_pItemBase;
	
	// 아이템줍는사람 반경안에 드는 멤버들만 추려낸다.
	memset( pListMember, 0, sizeof(pListMember) );
	for( int i = 0; i < pParty->m_nSizeofMember; i++ )
	{
		pMember		= g_UserMng.GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
//		if( IsValidObj( (CObj*)pMember ) )
		// 12-3 康: 아이템 획득은 같은 맵 일정 반경에 속한 사용자로 제한해야한다.
		/*  
		if( IsValidObj( (CObj*)pMember ) && GetWorld() == pMember->GetWorld() )
		{
			vDist = pMember->GetPos() - GetPos();
			fDist = D3DXVec3LengthSq( &vDist );		// 아이템 줍는사람과 멤버간의 거리.
			if( fDist < 32.0f * 32.0f )		// xx미터보다 작으면
			{
				pListMember[ nMaxListMember++ ] = pMember;
			}
		}
		*/ //2009.01.21 // 반경 검사에 레이어 추가
		if( IsValidArea( pMember, 32.0f ) )
			pListMember[ nMaxListMember++ ] = pMember;
	}

	if( nMaxListMember == 0 )
	{
		pGetUser	= (CUser*)this;
	}
	else
	{
		switch( pParty->m_nTroupeShareItem )
		{
		case 1 :	// 순서분배

			{
				for( i = 0 ; i < nMaxListMember ; i++ )
				{
					if( pParty->m_nGetItemPlayerId == pListMember[i]->m_idPlayer )
					{
						if( i + 1 >= nMaxListMember )
						{
							pGetUser = pListMember[0];
							break;												
						}
						else
						{
							pGetUser = pListMember[ i+1 ];
							break;
						}
					}
				}
				
				if( pGetUser == NULL )
				{
					pGetUser = pListMember[0];
				}
			}
			break;
		case 2 :	// 수동분배 : 단장에게 다 들어감
			{
				if( pParty->IsLeader( pListMember[0]->m_idPlayer ) )
				{
					pGetUser = pListMember[0];
				}
				else
				{
					pGetUser = (CUser*)this;
				}
			}
			break;
		case 3 :	// 랜덤분배 : 랜덤하게 들어감
			{
				DWORD dwRand = xRandom( nMaxListMember );
				pGetUser = pListMember[dwRand];
			}
			break;
		default:	// 기본 개임분배

			{
				pGetUser = (CUser*)this; 
			}
			break;
		}	// switch
	}	
	pParty->m_nGetItemPlayerId = pGetUser->m_idPlayer;

	bSuccess	= pGetUser->CreateItem( pItemBase );

	for( i = 0; i < nMaxListMember; i++ )
	{
		pMember = pListMember[i];
		if( TRUE == pMember->IsPlayer() && pItemBase->GetProp() != NULL )
		{
			if( bSuccess )
			{
				if( pMember->m_idPlayer != pGetUser->m_idPlayer )
				{
					if( ::GetLanguage() == LANG_TWN || ::GetLanguage() == LANG_JAP || ::GetLanguage() == LANG_HK )
						( (CUser*)pMember )->AddDefinedText( TID_GAME_TROUPEREAPITEM, "\"%s\" \"%s\"", pGetUser->GetName(), pItemBase->GetProp()->szName );
					else
					{
						if( ::GetLanguage() == LANG_THA )
							( (CUser*)pMember )->AddDefinedText( TID_GAME_REAPITEM_THA, "\"%s\" \"%s\"", pGetUser->GetName(), pItemBase->GetProp()->szName );
						else
							( (CUser*)pMember )->AddDefinedText( TID_GAME_REAPITEM, "\"%s%s %s\"", pGetUser->GetName(), prj.GetText(TID_GAME_FROM2), pItemBase->GetProp()->szName );
					}
				}
				else
				{
					( (CUser*)pGetUser )->AddDefinedText( TID_GAME_REAPITEM, "\"%s\"", pItemBase->GetProp()->szName );
				}
			}
			else
			{
				// CreateItem을 실패함 Full
				if( pGetUser == pMember )
					( (CUser*)pGetUser )->AddDefinedText( TID_GAME_LACKSPACE );
			}
		}									
	}
	
	if( ( (CItemElem*)pItemBase )->IsLogable() && bSuccess )
	{
		switch( pItemBase->GetProp()->dwItemKind2 ) 
		{
		case IK2_FOOD:
		case IK2_MAGIC:
		case IK2_POTION:
		case IK2_REFRESHER:
			{
				if( ( ( (CItemElem*)pItemBase )->m_nItemNum ) > 9 )
				{
					LogItemInfo aLogItem;
					aLogItem.Action = "R";
					aLogItem.SendName = "GROUND";
					aLogItem.RecvName = pGetUser->GetName();
					aLogItem.WorldId = pGetUser->GetWorld()->GetID();
					aLogItem.Gold = aLogItem.Gold2 = pGetUser->GetGold();
					g_DPSrvr.OnLogItem( aLogItem, ( (CItemElem*)pItemBase ), ( (CItemElem*)pItemBase )->m_nItemNum );
				}
				break;
			}
		default:
			{
				LogItemInfo aLogItem;
				aLogItem.Action = "R";
				aLogItem.SendName = "GROUND";
				aLogItem.RecvName = pGetUser->GetName();
				aLogItem.WorldId = pGetUser->GetWorld()->GetID();
				aLogItem.Gold = aLogItem.Gold2 = pGetUser->GetGold();
				g_DPSrvr.OnLogItem( aLogItem, ( (CItemElem*)pItemBase ), ( (CItemElem*)pItemBase )->m_nItemNum );
				break;
			}
		}
	}
#endif // __WORLDSERVER
	return bSuccess;
} // SubLootDropMobParty

//
//
// DoLoot의 하위 - 몹이 떨어트린 아이템 루팅부...
BOOL CMover::SubLootDropMob( CItem *pItem )
{
	BOOL bSuccess = TRUE;
#ifdef __WORLDSERVER
	CParty *pParty	= g_PartyMng.GetParty( m_idparty );
	if( pParty && pParty->IsMember( m_idPlayer )  )	// 파티가 있으므로 경험치 분배대로 해야함
	{
		// 파티가 있을경우 루팅 처리.
		bSuccess = SubLootDropMobParty( pItem, pParty );
	}
	else // 파티가 없을때~
	{
		bSuccess = SubLootDropMobSingle( pItem );
	}
#endif	// __WORLDSERVER

	return bSuccess;
}

// DoLoot의 하위 - 몹이 떨어트린게 아닌! 아이템루팅부...
BOOL CMover::SubLootDropNotMob( CItem *pItem )
{
	BOOL bSuccess = TRUE;
#ifdef __WORLDSERVER
	CItemBase* pItemBase = pItem->m_pItemBase;
	
	bSuccess	= CreateItem( pItemBase );
	if( TRUE == IsPlayer() && pItemBase->GetProp() != NULL )
	{
		if( bSuccess )
		{
			((CUser*)this)->AddDefinedText( TID_GAME_REAPITEM, "\"%s\"", pItemBase->GetProp()->szName );						

			if( IsLootSendText( pItemBase->GetProp() ) )
				((CUser*)this)->AddChatText( TID_GAME_REAPITEM, "\"%s\"", pItemBase->GetProp()->szName );									
		}
		else
			((CUser*)this)->AddDefinedText( TID_GAME_LACKSPACE );
	}
	
	if( ( (CItemElem*)pItemBase )->IsLogable() && bSuccess )
	{
		switch( pItemBase->GetProp()->dwItemKind2 ) 
		{
		case IK2_FOOD:
		case IK2_MAGIC:
		case IK2_POTION:
		case IK2_REFRESHER:
			{
				if( ( (CItemElem*)pItemBase )->m_nItemNum > 9 )
				{
					LogItemInfo aLogItem;
					aLogItem.Action = "R";
					aLogItem.SendName = "GROUND";
					aLogItem.RecvName = GetName();
					aLogItem.WorldId = GetWorld()->GetID();
					aLogItem.Gold = aLogItem.Gold2 = GetGold();
					g_DPSrvr.OnLogItem( aLogItem, ( (CItemElem*)pItemBase ), ( (CItemElem*)pItemBase )->m_nItemNum );
				}
				break;
			}
		default:
			{
				LogItemInfo aLogItem;
				aLogItem.Action = "R";
				aLogItem.SendName = "GROUND";
				aLogItem.RecvName = GetName();
				aLogItem.WorldId = GetWorld()->GetID();
				aLogItem.Gold = aLogItem.Gold2 = GetGold();
				g_DPSrvr.OnLogItem( aLogItem, ( (CItemElem*)pItemBase ), ( (CItemElem*)pItemBase )->m_nItemNum );
				break;
			}
		}
	}
#endif // __WORLDSERVER
	
	return bSuccess;
} // SubLootDropNotMob

//////////////////////////////////////////////////////////////////////////
//
// 바닥에 떨어진 pItem을 갖는다.
//
//////////////////////////////////////////////////////////////////////////
BOOL CMover::DoLoot( CItem *pItem )
{
	if( IsPlayer() == FALSE )	return FALSE;

	CItemBase* pItemBase = pItem->m_pItemBase;
	BOOL bSuccess = TRUE;

#ifdef __CLIENT	
	PLAYSND( SND_INF_GROUNDPICKUP );
#endif
	
	ItemProp *pItemProp = pItem->GetProp();
	if( pItemProp->dwItemKind1 == IK1_GOLD ) 
	{
		PickupGold( ((CItemElem*)pItemBase)->GetGold(), pItem->m_bDropMob );
	}
	else 
	{
		// <아이템 로그 - 획득>
#ifdef __WORLDSERVER
		if( pItem->m_bDropMob == TRUE ) // 몬스터가 떨어뜨린것만
		{
			// 몹이 떨어트린 아이템에 대한 루팅 처리
			bSuccess = SubLootDropMob( pItem );
		}
		else	// 어떤유저가 떨어뜨린것은 줏은넘이 임자~
		{
			// 몹이 떨어트린게 아닌 아이템의 루팅에 대한 처리
			bSuccess = SubLootDropNotMob( pItem );

		}
#endif	// __WORLDSERVER
	}

	if( TRUE == bSuccess )
	{
		pItem->Delete();
#ifdef __WORLDSERVER
//		g_UserMng.AddMotion( this, OBJMSG_PICKUP );
		RemoveInvisible();		// 아이템 줏으면 투명 해제.
#endif	// __WORLDSERVER
	}

	return bSuccess;
}

