#include "stdafx.h"
#include "defineSound.h"
#include "DefineObj.h"
#include "CommonCtrl.h"
#include "Ship.h"
#ifdef __CLIENT
	#include "Sfx.h"
#endif




#ifdef __CLIENT

/*
	XI_로 시작하는건 하나의 빌보드 텍스춰일 수도 있고, 특수효과 툴에 의해 생성된 데이타일 수도 있다.
	
	CreateGenSfx - 일반 효과 인덱스에 해당하는 효과 인덱스로 생성 
			dwSfxIdx( EI_EXPLOSION ) -> Create( XI_EXPLOSION )

	CreateItemSfx - 아이템 인덱스에 해당하는 효과 인덱스로 생성 
			pItemElem( II_FOOD ) -> Create( XI_FOOD )
			
	CreateSkillSfx - 스킬 인덱스에 해당하는 효과 인덱스로 생성 
			pItemElem( SI_FIREBALL ) -> Create( XI_FIREBALL )
		
*/



// 일반 효과 생성 
// SFX 새로 만들고도 여기다가 추가 안해주면 생성 안된다.
CSfx* CreateSfx( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwSfxObj, 
				 D3DXVECTOR3& vPosSrc, OBJID idSrc, D3DXVECTOR3& vPosDest, OBJID idDest, int nSec )
{
	CSfx* pObj = NULL;
#ifdef _DEBUG
	if( dwSfxObj == 265 )	// 디버깅 하려면 이걸 바꿔 쓰세요.
	{
		int a = 0;
	}
#endif

	CMover *pSrc = prj.GetMover( idSrc );
	CMover *pDst = prj.GetMover( idDest );
	
#ifndef __WORLDSERVER
	switch( dwSfxObj )
	{
	case XI_SETIEM_EFFECTHAND:
	case XI_GEN_ITEM_SETITEM03:
	case XI_GEN_ITEM_SETITEM04:
	case XI_GEN_ITEM_SETITEM05:
	case XI_GEN_ITEM_SETITEM06:
	case XI_GEN_ITEM_SETITEM07:
	case XI_GEN_ITEM_SETITEM08:
	case XI_GEN_ITEM_SETITEM09:
	case XI_GEN_ITEM_SETITEM10:
			pObj = new CSfxPartsLinkShoulder(); break;

	case XI_HIT_SWORD01 : pObj = new CSfxGenNormalDmg01(); break;
	case XI_HIT_SWORD02 : pObj = new CSfxGenNormalDmg01(); break;
	case XI_HIT_STICK01 : pObj = new CSfxGenNormalDmg01(); break;
	case XI_HIT_STICK02 : pObj = new CSfxGenNormalDmg01(); break;

	case XI_FIR_WAND01  : pObj = new CSfxItemWandAtk1(); break;

	case XI_FIR_WAND02  : pObj = new CSfxItemWandAtkAir(); break;

	case XI_FIR_RANGE01 : pObj = new CSfx(); break;
	case XI_FIR_RANGE02 : pObj = new CSfx(); break;
		
	case XI_GEN_RESTORATION01   : pObj = new CSfxGenRestoration01();break;
	case XI_GEN_LOGIN01         : pObj = new CSfxGenLogin()        ;break;
	case XI_GEN_WARP01          : pObj = new CSfxGenWarp()         ;break;
	case XI_GEN_PC_DIE01        : pObj = new CSfxGenPcDie()        ;break;
	case XI_GEN_MOVEMARK01		: pObj = new CSfxGenMoveMark()     ;break;
		
	case XI_GEN_RAINCIRCLE01	: pObj = new CSfxGenRainCircle()  ;break;
		
	case XI_GEN_WATERCIRCLE01	: pObj = new CSfxGenWaterCircle()  ;break;
	case XI_GEN_WATERCROWN01	: pObj = new CSfxGenWaterCrown()   ; PLAYSND( SND_GEN_WATER, &vPosSrc ); break;
		
	case XI_ITEM_WAND_ATK1		: pObj = new CSfxItemWandAtk1()   ;break;
	case XI_ITEM_WAND_ATK2		: pObj = new CSfxItemWandAtk2()   ;break;
	case XI_ITEM_WAND_ATK3		: pObj = new CSfxItemWandAtk3()   ;break;
	case XI_ITEM_WAND_ATK4		: pObj = new CSfxItemWandAtk4()   ;break;
		
	case XI_ITEM_RANGE_ATK1		: pObj = new CSfxItemRangeAtk1()   ;break;
	case XI_ITEM_RANGE_ATK2		: pObj = new CSfxItemRangeAtk1()   ;break;
	case XI_ITEM_RANGE_ATK3		: pObj = new CSfxItemRangeAtk1()   ;break;
	case XI_ITEM_RANGE_ATK4		: pObj = new CSfxItemRangeAtk1()   ;break;
		
	case XI_SKILL_VAG_ONE_OVERCUTTER01 : 
	case XI_SKILL_VAG_ONE_CLEANHIT01   : 
	case XI_SKILL_VAG_ONE_BRANDISH01   : 
	case XI_SKILL_MER_ONE_SPECIALHIT02:		//스페셜 히트 터지는 sfx
	case XI_SKILL_MER_ONE_REFLEXHIT02:
	case XI_SKILL_BIL_KNU_BELIALSMESHING02:
	case XI_SKILL_BLD_DOUBLE_CROSSSTRIKE02:
	case XI_SKILL_BLD_DOUBLE_ARMORPENETRATE02:
//	case XI_SKILL_KNT_TWOAX_POWERSTUMP03:
	case XI_SKILL_BIL_KNU_BLOODFIST02:
	case XI_SKILL_BLD_DOUBLE_SONICBLADE03:
	case XI_SKILL_BIL_KNU_PIERCINGSERPENT02:
	case XI_SKILL_RIN_SUP_MERKABAHANZELRUSHA03:
	case XI_SKILL_ELE_FIRE_BURINGFIELD02:
	case XI_SKILL_KNT_TWOSW_EARTHDIVIDER02:
#ifdef __3RD_LEGEND16
	case XI_SKILL_STORM_STORMBLASTE04:
	case XI_SKILL_WINDL_COUNTERBACK01:
#endif // __3RD_LEGEND16
		pObj = new CSfxFixed();		// fixed는 이펙트 
		if( pObj && IsValidObj(pSrc) )
			pObj->SetAngle( -pSrc->GetAngle() ); // 시전자의 방향과 일치시킨다.
//			pObj->SetAngle( 180.0f - pSrc->GetAngle() ); // 시전자의 방향과 일치시킨다.

		break;
	case XI_SKILL_BIL_KNU_SONICHAND02:		// sfx 방향바껴서 각도 조절 따로 하는거
	case XI_SKILL_BIL_KNU_ASALRAALAIKUM02:
		pObj = new CSfxFixed();		// fixed는 이펙트 
		if( pObj && IsValidObj(pSrc) )
		{
			float fAngle = -pSrc->GetAngle() + 180;
			pObj->SetAngle( fAngle ); // 시전자의 방향과 일치시킨다.
		}
		
		break;
		// 폭발씬 류는 지상에서 좀 띄워서 출력
//	case XI_SKILL_RAG_SUP_FASTATTACK02:
//	case XI_SKILL_ACR_YOYO_SLOWSTEP02:
	case XI_HIT_YOY01:
	case XI_HIT_BOW01:
	case XI_SKILL_JST_YOYO_HITOFPENYA02:
	case XI_SKILL_JST_YOYO_VATALSTAB02:
	case XI_SKILL_ACR_YOYO_SLOWSTEP02:
	case XI_SKILL_RAG_BOW_FLAMEARROW03:
	case XI_SKILL_RAG_BOW_PIRCINGARROW03:
	case XI_SKILL_RAG_BOW_POISONARROW03:
	case XI_SKILL_RAG_BOW_SILENTARROW03:
	case XI_SKILL_RAG_SUP_NATURE02:
	case XI_SKILL_RAG_BOW_TRIPLESHOT03:
		
	case XI_SKILL_ACR_YOYO_SNITCH02:
	case XI_SKILL_ACR_BOW_AIMEDSHOT03:
	case XI_SKILL_ACR_BOW_JUNKBOW03:
	case XI_SKILL_ACR_BOW_SILENTSHOT03:
	case XI_SKILL_ACR_BOW_AUTOSHOT03:
	case XI_SKILL_BLD_DOUBLEAX_HAWKATTACK03:
	case XI_SKILL_BLD_DOUBLESW_BLADEDANCE02:
	case XI_SKILL_BLD_DOUBLESW_BLADEDANCE03:
	case XI_SKILL_KNT_TWO_POWERSWING02:
//		vPosDest.y += 1.0f;
		pObj = new CSfxFixed();		// fixed는 이펙트 
		if( pObj && IsValidObj(pSrc) )
			pObj->SetAngle( 180.0f - pSrc->GetAngle() ); // 시전자의 방향과 일치시킨다.
		break;
		
	case XI_SKILL_MER_ONE_KEENWHEEL01     : pObj = new CSfxSkillMerKeenWheel()    ;break;
	case XI_SKILL_MER_ONE_SPLMASH01       : pObj = new CSfxSkillMerSplmash()      ;break;
	case XI_SKILL_MER_ONE_BLINDSIDE01     : pObj = new CSfxSkillMerBlindSide()    ;break;
	case XI_SKILL_MAG_WIND_SWORDWIND01  : pObj = new CSfxSkillMagSwordWind()     ; break;
	case XI_SKILL_MAG_WIND_STRONGWIND01 : pObj = new CSfxSkillMagStrongWind()    ; break;
	case XI_SKILL_MAG_FIRE_BOOMERANG01  : pObj = new CSfxSkillMagFireBoomerang() ; break;
	case XI_SKILL_MAG_FIRE_HOTAIR01     : pObj = new CSfxSkillMagHotAir()        ; break;

	case XI_SKILL_MAG_EARTH_CASTING01:
	case XI_SKILL_MAG_ELECTRICITY_CASTING01:
	case XI_SKILL_MAG_WATER_CASTING01:		// 일단 캐스팅 동작은 모두 FireCasting으로 쓴다.
	case XI_SKILL_MAG_MAG_CASTING01:
	case XI_SKILL_ELE_FIRE_CASTING01:
	case XI_SKILL_ASS_CHEER_HASCASTING01:
	case XI_SKILL_ELE_MULTI_CASTING01:
	case XI_SKILL_PSY_PSY_CASTING01:
	case XI_SKILL_MAG_FIRE_CASTING01: 
	case XI_SKILL_ASS_HEAL_CASTING01:
	case XI_SKILL_ASS_RES_CASTING01:
		pObj = new CSfxSkillMagFireCasting();
		break;
#ifdef __3RD_LEGEND16
	case XI_SKILL_MENT_DARKNESSSCREAM01:
	case XI_SKILL_LORDK_SHILDSTRIKE01:
	case XI_SKILL_ELE_FINALSPEAR01:
	case XI_SKILL_LORDK_SCOPESTRIKE02:
		pObj = new CSfxSkillCastingFrontDynamic();
		break;
	case XI_SKILL_MENT_DARKNESSRAKE02:
		pObj = new CSfxSkillCastingLSideDynamic();
		break;
	case XI_SKILL_MENT_ATTACKDECREASE01:
	case XI_SKILL_MENT_DEFENDERDECREASE01:
	case XI_SKILL_MENT_SPEEDDECREASE01:
	case XI_SKILL_CRACK_POWERINCREASE01:
	case XI_SKILL_CRACK_CONTROL01:
	case XI_SKILL_WINDL_CONTROLINCREASE01:
	case XI_SKILL_ELE_SLEEPING01:
		pObj = new CSfxSkillAboveTargetDynamic();		
		break;
#endif // __3RD_LEGEND16
	case XI_SKILL_MAG_WIND_CASTING01    : pObj = new CSfxSkillMagWindCasting();break;

	case XI_SKILL_ASS_CHEER_CASTING01    : pObj = new CSfxSkillMagWindCasting();break;
		
	case XI_GEN_CURE01: PLAYSND( SND_GEN_LEVELUP, &vPosSrc ); pObj = new CSfx(); break;
	case XI_GEN_REF01: PLAYSND( SND_GEN_LEVELUP, &vPosSrc ); pObj = new CSfx(); break;

	case XI_HIT_CRITICAL01: pObj = new CSfx(); break;	
	case XI_HIT_MISS01: pObj = new CSfx(); break;	
		
	case XI_NPC_RAN_SPITTL    : pObj = new CSfxItemWandAtk1()   ;break;    
	case XI_NPC_RAN_GAS       : pObj = new CSfxItemWandAtk1()   ;break;    
	case XI_NPC_RAN_MAGICBALL : pObj = new CSfxItemWandAtk1()   ;break;    
	case XI_NPC_RAN_MAGICBLUE : pObj = new CSfxItemWandAtk1()   ;break;    
	case XI_NPC_RAN_CARD      : pObj = new CSfxItemWandAtk1()   ;break;    
	case XI_NPC_RAN_CANNON    : pObj = new CSfxItemWandAtk1()   ;break;    
	case XI_SKILL_MUSHMOOT_02 : pObj = new CSfxMagicMiAtk1()   ;break;    
	case XI_SKILL_TRO_STRETCHING01 : pObj = new CSfxTroStretching01()   ;break;    
	case XI_SKILL_TRO_STRETCHING02 : pObj = new CSfxTroStretching02()   ;break;    
	case XI_SKILL_TRO_BLITZ : pObj = new CSfxTroBlitz()   ;break;    
	case XI_SKILL_ASS_KNU_BURSTCRACK01 : pObj = new CSfxSkillAssBurstcrack()   ;break;    
	case XI_SKILL_ASS_KNU_TAMPINGHOLE01 : pObj = new CSfxSkillAssTampinghole()   ;break;    
	case XI_NPC_DIR_STEAM : pObj = new CSfxNpcDirSteam()   ;break;    
	case XI_GEN_PVP_FLAG01: pObj = new CSfxDuelParty();		break;	// 이거 CSfxDuelParty()로 안쓸거면 모든소스검색해서 이거쓴거 바꿔줘야함
	case XI_ITEM_COLLECT: pObj = new CSfxCollect();		break;
	case XI_NPCSP1DIRAMP:		pObj = new CSfxClockWorksCharge();	break;
	case XI_NPCSP1DIRCANNON:	pObj = new CSfxClockWorksCannon();	break;
	case XI_NPCSP1RANBALL:		pObj = new CSfxAtkStraight();		break;
	case XI_SKILL_MUSHMOOT_CHARGE:	pObj = new CSfxMushmootCharge();	break;
	case XI_SKILL_MAG_WATER_ICEMISSILE01   : pObj = new CSfxSkillMagIceMissile()    ;break;
	case XI_SKILL_MER_ONE_BLOODYSTRIKE02:	
		pObj = new CSfx();
		if( pObj )
		{
			CMover *pSrc = prj.GetMover( idSrc );
			CMover *pDst = prj.GetMover( idDest );
			if( IsValidObj(pSrc) && IsValidObj(pDst) )
			{
				FLOAT fAngle = GetDegree( pSrc->GetPos(), pDst->GetPos() );
				pObj->SetAngle( -fAngle );
			}
		}
		break;
	// 일반적인 쏘고 날아가서 맞는 이펙트는 이걸로 통일하자.
	case XI_SKILL_MAG_ELECTRICITY_LIGHTINGBALL01: 	pObj = new CSfxSkillMagLightningBall();		break;
	case XI_SKILL_MAG_FIRE_FIRESTRIKE02:		pObj = new CSfxShoot();		break;
	case XI_SKILL_MAG_EARTH_SPIKESTONE01:	pObj = new CSfxSkillMagSpikeStone();		break;
	case XI_NAT_WINGANGEL01:	// 천사의 날개
		pObj = new CSfxRotate();
		break;
	case XI_NAT_WASTART01:	// 천사의 날개 시전 효과.
		pObj = new CSfx();
		CreateSfx( pd3dDevice, XI_NAT_WINGANGEL01, vPosSrc, idSrc, vPosDest, idDest, nSec );		// 날개 2분동안 출력.
		break;
	case XI_NAT_CUPITSTART01:	// 큐피트날개 시전 효과.
		pObj = new CSfx();
		CreateSfx( pd3dDevice, XI_NAT_WINGANGEL01, vPosSrc, idSrc, vPosDest, idDest, nSec );		// 날개 5분동안 출력.
		break;
	case XI_SKILL_KNT_TWO_POWERSWING01:		// 나이트-가드스킬
		idDest = NULL_ID;
		pObj = new CSfx();
		break;
	case XI_SKILL_MER_ONE_GUILOTIN02: 	// 필참 - 파워웨이브.
		pObj = new CSfxShootWave();
		break;
	case XI_SKILL_DROP_DUST_RAIN:
		pObj = new CSfxItemRangeAtk1_StoneRain();
		break;
	case XI_SKILL_DROP_DUST: 
		pObj = new CSfxItemRangeAtk1_Stone();
		break;
	case XI_SKILL_ACR_BOW_ARROWRAIN03:
		pObj = new CSfxItemRangeAtk1_AllowRain();		
		break;
	case XI_SKILL_RAG_BOW_ARROWRAIN:
		pObj = new CSfxItemRangeAtk1_Allow();
		break;
		
	// 활용 시전이펙트는 팔에 링크되게 한다.
	case XI_SKILL_RAG_BOW_ICEARROW01:
	case XI_SKILL_RAG_SUP_FASTATTACK01:
	case XI_SKILL_RAG_BOW_FLAMEARROW01:
	case XI_SKILL_RAG_BOW_PIRCINGARROW01:
	case XI_SKILL_RAG_BOW_POISONARROW01:
	case XI_SKILL_RAG_BOW_SILENTARROW01:
	case XI_SKILL_RAG_BOW_TRIPLESHOT01:
					
	case XI_SKILL_ACR_BOW_JUNKBOW01:
	case XI_SKILL_ACR_BOW_AIMEDSHOT01:
	case XI_SKILL_ACR_BOW_SILENTSHOT01:
	case XI_SKILL_ACR_BOW_ARROWRAIN01:
	case XI_SKILL_ACR_BOW_AUTOSHOT01:
	case XI_SKILL_MER_ONE_SUPPORT01:
	case XI_SKILL_MER_ONE_SUPPORT02:
	case XI_SKILL_MER_ONE_SUPPORT03:
	case XI_SKILL_MER_ONE_SUPPORT04:
	case XI_SKILL_ASS_KNU_SUPPORT01:
	case XI_SKILL_ASS_KNU_SUPPORT02:
	case XI_SKILL_ASS_KNU_SUPPORT03:
#ifdef __3RD_LEGEND16
	case XI_SKILL_LORDK_GRANDRAGE01:
	case XI_SKILL_LORDK_TEMPLARPULLING01:
	case XI_SKILL_CRACK_RANGESTRIKE01:
#endif // __3RD_LEGEND16
		pObj = new CSfxAllowPartsLink();
		break;

	case XI_SKILL_JST_YOYO_VATALSTAB01:
	case XI_SKILL_JST_YOYO_HITOFPENYA01:
	case XI_SKILL_ACR_YOYO_SLOWSTEP01:
	case XI_SKILL_JST_SUP_POISON01:  // 손에 링크되는 이펙
	case XI_SKILL_JST_SUP_BLEEDING01:
	case XI_SKILL_JST_YOYO_BACKSTAB01:		
	case XI_SKILL_JST_SUP_ABSORB01:		
	case XI_SKILL_JST_SUP_CRITICALSWING01:	
		pObj = new CSfxPartsLinkJst();

	case XI_SKILL_BLD_DOUBLEAX_SPRINGATTACK01:		// 손에 링크되는 이펙
	case XI_SKILL_BLD_DOUBLE_SONICBLADE01:
	case XI_SKILL_KNT_TWOSW_CHARGE01:
	case XI_SKILL_BLD_DOUBLESW_BLADEDANCE01:
		pObj = new CSfxPartsLinkBlade();		
		break;
	case XI_EVE_EVENT_FAIRYLY:
		pObj = new CSfxFixed();
		break;
		
#if __VER >= 12 // __ITEMCREATEMON_S0602
	case XI_CHR_CURSOR1:
		pObj = new CSfxCursor();
		break;
#endif // __ITEMCREATEMON_S0602

	case XI_BUFFPET_GRADE1:
	case XI_BUFFPET_GRADE2:
	case XI_BUFFPET_GRADE3:
		pObj = new CSfxLinkMover();
		break;
		
	case 0: break;
	default: pObj = new CSfx(); break;
	}
	if( pObj )
	{
		int bRet = pObj->SetSfx( pd3dDevice, dwSfxObj, vPosSrc, idSrc, vPosDest, idDest, nSec ); 
		if( bRet )
		{
			if( pObj->GetModel() == NULL )
			{
				LPCTSTR szErr = Error( "CreateSfx : %d sfx의 m_pModel이 없음", dwSfxObj );
				ADDERRORMSG( szErr );
			}
			if( pObj->GetModel()->m_pModelElem == NULL )
			{
				LPCTSTR szErr = Error( "CreateSfx : %d sfx의 m_pModelElem이 없음", dwSfxObj );
				ADDERRORMSG( szErr );
			}
			if( pObj->GetModel()->m_pModelElem->m_szPart[0] )
				PLAYSND( pObj->GetModel()->m_pModelElem->m_szPart, &vPosSrc );

			// 오브젝트 생성후 후처리.
			switch( dwSfxObj )
			{
/*			case XI_SKILL_MAG_ELECTRICITY_LIGHTINGBALL01:
				{
					CSfxShoot *pSfxShoot = (CSfxShoot *)pObj;
					pSfxShoot->SetHitSfx( "sfx_sklmagelelgtbal01", idSrc, idDest, 2.0f );
					pSfxShoot->SetDir( TRUE );
					pSfxShoot->SetSndHit( SND_PC_SKILLM_FIREBOOMERANG2 );
				}
				break;*/
			case XI_SKILL_MER_ONE_GUILOTIN02: 	// 필참 - 파워웨이브.
				{
					CSfxShootWave *pSfxShootWave = (CSfxShootWave *)pObj;
					pSfxShootWave->SetHitSfx( XI_SKILL_MER_ONE_GUILOTIN02, idSrc, idDest, 2.0f );
					pSfxShootWave->SetDir( TRUE );
//					pSfxShoot->SetSndHit( SND_PC_SKILLM_FIREBOOMERANG2 );
				}
				break;
			case XI_SKILL_MAG_FIRE_FIRESTRIKE02:
				{
					CSfxShoot *pSfxShoot = (CSfxShoot *)pObj;
					pSfxShoot->SetDir( TRUE );
				}
				break;
			case XI_SKILL_MER_ONE_SNEAKER02:	// 스네어
				{
					CMover *pDst = prj.GetMover( idDest );
					if( IsValidObj( pDst ) )
					{
						FLOAT fRadius = pDst->GetRadius();
						pObj->SetScale( D3DXVECTOR3( fRadius, fRadius, fRadius ) );
					}
				}
				break;
			}

			// 월드에 오브젝트 추가.
			g_WorldMng.Get()->AddObj(pObj);
		} else
		{
			SAFE_DELETE( pObj );
		}
	}
#endif	// __WORLDSERVER
	return pObj;
}

// 요요무기는 무조건 여기만 호출
CSfx* CreateSfxYoYo( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwSfxObj, 
				 D3DXVECTOR3& vPosSrc, OBJID idSrc, D3DXVECTOR3& vPosDest, OBJID idDest, int nSec )
{
	CSfx* pObj = NULL;

#ifdef _DEBUG
	if( dwSfxObj == 116 )	// 디버깅 하려면 이걸 바꿔 쓰세요.
	{
		int a = 0;
	}
#endif
	CMover *pSrc = prj.GetMover( idSrc );
	CMover *pDst = prj.GetMover( idDest );
	
#ifndef __WORLDSERVER
	/*
	switch( dwSfxObj )
	{
	}
	*/
	// 요요
	pObj = new CSfxItemYoyoAtk();

	if( pObj )
	{
		int bRet = pObj->SetSfx( pd3dDevice, dwSfxObj, vPosSrc, idSrc, vPosDest, idDest, nSec ); 
		if( bRet )
		{
			if( pObj->GetModel() == NULL )
			{
				LPCTSTR szErr = Error( "CreateSfxYoYo : %d sfx의 m_pModel이 없음", dwSfxObj );
				ADDERRORMSG( szErr );
			}
			if( pObj->GetModel()->m_pModelElem == NULL )
			{
				LPCTSTR szErr = Error( "CreateSfxYoYo : %d sfx의 m_pModelElem이 없음", dwSfxObj );
				ADDERRORMSG( szErr );
			}
			if( pObj->GetModel()->m_pModelElem->m_szPart[0] )
				PLAYSND( pObj->GetModel()->m_pModelElem->m_szPart, &vPosSrc );

			// 월드에 오브젝트 추가.
			g_WorldMng.Get()->AddObj(pObj);
		} else
		{
			SAFE_DELETE( pObj );
		}
	}
#endif	// __WORLDSERVER
	return pObj;
}

CSfxShoot* CreateShootSfx( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwSfxObj, 
						D3DXVECTOR3& vPosSrc, OBJID idSrc, D3DXVECTOR3& vPosDest, OBJID idDest, int nSec )
{
	CSfxShoot* pObj = NULL;
#ifdef _DEBUG
	if( dwSfxObj == XI_SKILL_MAG_FIRE_FIRESTRIKE01 )	// 디버깅 하려면 이걸 바꿔 쓰세요.
	{
		int a = 0;
	}
#endif
	CMover *pSrc = prj.GetMover( idSrc );
	CMover *pDst = prj.GetMover( idDest );
	
#ifndef __WORLDSERVER
	switch( dwSfxObj )
	{
	case XI_FIR_WAND01  : pObj = new CSfxItemWandAtk1(); break;
	case XI_FIR_WAND02  : pObj = new CSfxItemWandAtkAir(); break;

	case XI_ITEM_WAND_ATK1		: pObj = new CSfxItemWandAtk1()   ;break;
	case XI_ITEM_WAND_ATK2		: pObj = new CSfxItemWandAtk2()   ;break;
	case XI_ITEM_WAND_ATK3		: pObj = new CSfxItemWandAtk3()   ;break;
	case XI_ITEM_WAND_ATK4		: pObj = new CSfxItemWandAtk4()   ;break;

	case XI_SKILL_RAG_BOW_TRIPLESHOT02:
	case XI_SKILL_ACR_BOW_JUNKBOW02:
		pObj = new CSfxItemRangeAtk_JunkBow()   ;break;
		
	case XI_SKILL_RAG_BOW_ICEARROW02:
	case XI_SKILL_RAG_BOW_FLAMEARROW02:
	case XI_SKILL_RAG_BOW_PIRCINGARROW02:
	case XI_SKILL_RAG_BOW_POISONARROW02:
	case XI_SKILL_RAG_BOW_SILENTARROW02:
			
	case XI_SKILL_ACR_BOW_AIMEDSHOT02:
	case XI_SKILL_ACR_BOW_SILENTSHOT02:
	case XI_SKILL_ACR_BOW_ARROWRAIN02:
	case XI_SKILL_ACR_BOW_AUTOSHOT02:

	case XI_ITEM_RANGE_ATK1		: pObj = new CSfxItemRangeAtk1()   ;break;
	case XI_ITEM_RANGE_ATK2		: pObj = new CSfxItemRangeAtk1()   ;break;
	case XI_ITEM_RANGE_ATK3		: pObj = new CSfxItemRangeAtk1()   ;break;
	case XI_ITEM_RANGE_ATK4		: pObj = new CSfxItemRangeAtk1()   ;break;
		
	case XI_SKILL_MAG_WIND_SWORDWIND01  : pObj = new CSfxSkillMagSwordWind()     ; break;
	case XI_SKILL_MAG_WIND_STRONGWIND01 : pObj = new CSfxSkillMagStrongWind()    ; break;
	case XI_SKILL_MAG_FIRE_BOOMERANG01  : pObj = new CSfxSkillMagFireBoomerang() ; break;
	case XI_SKILL_MAG_FIRE_HOTAIR01     : pObj = new CSfxSkillMagHotAir()        ; break;

	case XI_NPC_RAN_SPITTL    : 
	case XI_NPC_RAN_GAS       : 
	case XI_NPC_RAN_MAGICBALL : 
	case XI_NPC_RAN_MAGICBLUE : 
	case XI_NPC_RAN_CARD      : 
	case XI_NPC_RAN_CANNON    : 
		pObj = new CSfxItemWandAtk1()   ;break;    
		
	case XI_NPCSP1RANBALL:		pObj = new CSfxAtkStraight();		break;

	case XI_SKILL_MAG_WATER_ICEMISSILE01   : pObj = new CSfxSkillMagIceMissile()    ;break;

	
	case XI_SKILL_MAG_ELECTRICITY_LIGHTINGBALL01: 	pObj = new CSfxSkillMagLightningBall();		break;
	case XI_SKILL_MAG_EARTH_SPIKESTONE01:	pObj = new CSfxSkillMagSpikeStone();		break;
	case XI_SKILL_MER_ONE_GUILOTIN02: 	// 필참 - 파워웨이브.
		pObj = new CSfxShootWave();
		break;
		
	case 0:		break;
	default:	// 그외 모든 디폴트.
		pObj = new CSfxShoot();		
		break;		// 일반적인 쏘고 날아가서 맞는 이펙트는 이걸로 통일하자.
	}

	if( pObj )
	{
		D3DXVECTOR3 vPosShoot = vPosSrc;	vPosShoot.y += 1.0f;
		int bRet = pObj->SetSfx( pd3dDevice, dwSfxObj, vPosShoot, idSrc, vPosDest, idDest, nSec ); 
		if( bRet )
		{
			if( pObj->GetModel()->m_pModelElem == NULL )
			{
				LPCTSTR szErr = Error( "CreateShootSfx : %d sfx의 m_pModelElem이 없음", dwSfxObj );
				ADDERRORMSG( szErr );
			}
			if( pObj->GetModel()->m_pModelElem->m_szPart[0] )
				PLAYSND( pObj->GetModel()->m_pModelElem->m_szPart, &vPosSrc );

			pObj->SetDir( TRUE );		// 방향성을 갖게 하자.

			// 오브젝트 생성후 후처리.
			switch( dwSfxObj )
			{
			case XI_SKILL_MER_ONE_GUILOTIN02: 	// 필참 - 파워웨이브.
				{
					CSfxShootWave *pSfxShootWave = (CSfxShootWave *)pObj;
					pSfxShootWave->SetHitSfx( XI_SKILL_MER_ONE_GUILOTIN02, idSrc, idDest, 2.0f );
					pSfxShootWave->SetDir( TRUE );
				}
				break;
			}

			// 월드에 오브젝트 추가.
			g_WorldMng.Get()->AddObj(pObj);
		} else
		{
			SAFE_DELETE( pObj );
		}
	}
#endif	// __WORLDSERVER
	return pObj;
}

CSfx* CreateItemReadySfx( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwSfxObj, 
				 D3DXVECTOR3& vPosSrc, OBJID idSrc, D3DXVECTOR3& vPosDest, OBJID idDest, int nSec )
{
	CSfx* pObj = NULL;
	CMover *pSrc = prj.GetMover( idSrc );
	CMover *pDst = prj.GetMover( idDest );
#ifndef __WORLDSERVER
	pObj = new CSfxReady();
	if( pObj )
	{
		int bRet = pObj->SetSfx( pd3dDevice, dwSfxObj, vPosSrc, idSrc, vPosDest, idDest, nSec ); 
		if( bRet )
		{
			if( pObj->GetModel() == NULL )
			{
				LPCTSTR szErr = Error( "CreateItemReadySfx : %d sfx의 m_pModel이 없음", dwSfxObj );
				ADDERRORMSG( szErr );
			}
			if( pObj->GetModel()->m_pModelElem == NULL )
			{
				LPCTSTR szErr = Error( "CreateItemReadySfx : %d sfx의 m_pModelElem이 없음", dwSfxObj );
				ADDERRORMSG( szErr );
			}
			if( pObj->GetModel()->m_pModelElem->m_szPart[0] )
				PLAYSND( pObj->GetModel()->m_pModelElem->m_szPart, &vPosSrc );	

			g_WorldMng.Get()->AddObj(pObj);

		} else
			SAFE_DELETE( pObj );
	}
#endif // not World

	return pObj;
}

#endif //__CLIENT

#ifdef __LAYER_1015
CMover *CreateMover( CWorld *pWorld, DWORD dwID, const D3DXVECTOR3 &vPos, int nLayer )
#else	// __LAYER_1015
CMover *CreateMover( CWorld *pWorld, DWORD dwID, D3DXVECTOR3 vPos )
#endif	// __LAYER_1015
{
	CMover *pMover;

	pMover = (CMover *)CreateObj( D3DDEVICE, OT_MOVER, dwID );
	if( pMover == NULL )	
		return NULL;
	pMover->SetPos( vPos );
	pMover->InitMotion( MTI_STAND );
	pMover->UpdateLocalMatrix();
	
	pMover->AddItToGlobalId();
	pWorld->ADDOBJ( pMover, FALSE, nLayer );

	return pMover;
}

CObj* CreateObj( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwObjType, DWORD dwObjIndex, BOOL bInitProp )
{
	CObj* pObj = NULL;
	switch( dwObjType )
	{
		case OT_OBJ  : pObj = new CObj;			break;
		case OT_CTRL : pObj = new CCommonCtrl;	break;
		case OT_SFX  : 
			#ifdef __CLIENT
				pObj = new CSfx; 
			#else
				return NULL;
			#endif
			break;  
		case OT_ITEM : pObj = new CItem;		break;
		case OT_MOVER: pObj = new CMover;		break;
		case OT_SHIP:	
			switch( dwObjIndex )
			{
			case 4:		pObj = new CShipLoop;	break;
			default:	pObj = new CShip;		break;
			}
			break;
	}
	pObj->m_dwType = dwObjType;

	if( pObj && pObj->GetType() == OT_SFX )
	{
		#ifdef __CLIENT
			((CSfx*)pObj)->SetSfx( pd3dDevice, dwObjIndex, D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), NULL_ID, D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), NULL_ID ); 
		#endif
		return pObj;
	}
	else if( pObj->SetIndex( pd3dDevice, dwObjIndex, bInitProp ) == TRUE )
	{
		pObj->SetMotion( MTI_STAND );
		// 스피드를 설정하려면 현재로선 이수밖에....좆치안타.
		if( dwObjType == OT_MOVER )
		{
			CMover *pMover = (CMover *)pObj;
			pMover->m_pActMover->m_fSpeed = pMover->GetProp()->fSpeed;
#if __VER >= 9	//__AI_0509
			MoverProp* pProp	= pMover->GetProp();
			if( pProp->dwAI == AII_MONSTER
#ifdef __AI_AGGRO
				|| pProp->dwAI == AII_AGGRO_NORMAL || pProp->dwAI == AII_PARTY_AGGRO_LEADER 
				|| pProp->dwAI == AII_PARTY_AGGRO_SUB || pProp->dwAI == AII_ARENA_REAPER )
#endif//__AI_AGGRO
			{
				pMover->m_pActMover->m_fSpeed	/= 2.0F;
//				if( pProp->dwClass == RANK_SUPER )
//					pMover->m_pActMover->m_fSpeed	*= 1.5F;
			}
#endif	// __AI_0509
		}
		return pObj;
	}

	Error( "CreateObj : type=%d, index=%d, bInitProp=%d", dwObjType, dwObjIndex, bInitProp );
	SAFE_DELETE( pObj );
	return NULL;
}


CObj* ReadObj( CFileIO* pFile )
{
	DWORD dwObjType;
	pFile->Read( &dwObjType, sizeof( DWORD ), 1 );

	CObj* pObj = NULL;
	switch( dwObjType )
	{
		case OT_OBJ  : pObj = new CObj; break;
		case OT_CTRL : pObj = new CCommonCtrl; break;
		case OT_SFX  : 
			#ifdef __CLIENT
				pObj = new CSfx; 
			#endif
			break;  
		case OT_ITEM : pObj = new CItem; break;
		case OT_MOVER:				// 파일에서 읽어들이는 것은 모두 NPC
			pObj = new CMover; 
			((CMover *)pObj)->InitNPCProperty();
			break;
		case OT_SHIP:	pObj = new CShip;	break;
	}

	if( pObj ) 
	{
		pObj->Read( pFile );
		return pObj;
	}
	return NULL;
}
BOOL CheckVersion( FILE* fp, LPCTSTR lpszName, LPCTSTR lpszVersion )
{
	CHAR szName[32];
	CHAR szVersion[32];

	fread( szName, sizeof( szName ), 1, fp );
	fread( szVersion, sizeof( szVersion ), 1, fp );

	if( !strcmp( szName, lpszName ) && !strcmp( szVersion, lpszVersion) )
		return TRUE;

	return FALSE;
}
BOOL WriteVersion( FILE* fp, LPCTSTR lpszName, LPCTSTR lpszVersion )
{
	if( strlen( lpszName ) >= 32 ||  strlen( lpszVersion ) >= 32 )
		return FALSE;
	fwrite( lpszName, sizeof( 32 ), 1, fp );
	fwrite( lpszVersion, sizeof( 32 ), 1, fp );
	return TRUE;
}
BOOL CheckVersion( FILE* fp, DWORD dwVersion )
{
	DWORD dwVer;
	fread( &dwVer, sizeof( DWORD ), 1, fp );

	if( dwVer == dwVersion )
		return TRUE;

	return FALSE;
}
BOOL WriteVersion( FILE* fp, DWORD dwVersion  )
{
	fwrite( &dwVersion, sizeof( DWORD ), 1, fp );
	return TRUE;
}




















