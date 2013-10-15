#include "stdafx.h"

#	if defined(__WORLDSERVER) || defined(__CLIENT)
#include "mover.h"
#include "defineItem.h"

#if __VER >= 19
#include "defineItemGrade.h"
#include "defineItemType.h"
#endif//__VER >= 19

#	else	// __WORLDSERVER, __CLIENT
#include "..\_network\objects\obj.h"
#	endif	// __WORLDSERVER, __CLIENT

#ifndef GETBLOCK
#define	GETBLOCK( ar, lpBuf, nBufSize )	\
										int nBufSize;	\
										LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );
#endif	// __GETBLOCK
#if __VER >= 13 // __HONORABLE_TITLE
#include "honor.h"
#endif	// __HONORABLE_TITLE
void CObj::Serialize( CAr & ar )	// 21
{
	if( ar.IsStoring() )
	{
		ar << (u_char)m_dwType;

		// mirchang_100513 type cast fix
		//ar << (u_short)m_dwIndex;
		ar << m_dwIndex;
		// mirchang_100513 type cast fix

		ar << (u_short)( m_vScale.x * 100.0f );
		ar << m_vPos;
		ar << (short)( m_fAngle * 10.0f );
	}
	else
	{
		u_short u2;
		short i2;
		m_dwType = m_dwIndex = 0; 
		
		ar >> (u_char&)m_dwType;				// m_dwType

		// mirchang_100513 type cast fix
		//ar >> u2;								// m_dwIndex
		//m_dwIndex	= (DWORD)u2;
		ar >> m_dwIndex;
		// mirchang_100513 type cast fix

		ar >> u2;								// m_vScale.x * 100.0f
		m_vScale.x	=	m_vScale.y	=	m_vScale.z	= (float)u2 / 100.0f;
		ar >> m_vPos;							// m_vPos
		ar >> i2;								// m_fAngle * 10.0f
		m_fAngle	= (float)i2 / 10.0f;
	#if defined(__WORLDSERVER) || defined(__CLIENT)
		SetAngle( m_fAngle );
	#endif

	#ifdef __WORLDSERVER
		m_vLink		= m_vPos;
	#endif	// __WORLDSERVER
	}
}

#if defined(__WORLDSERVER) || defined(__CLIENT)
// 장어구이, 신속의 두루마리 등의 예외처리
void CMover::OnApplySM()
{
	ItemProp* aItemprop;
	int nAdjParamVal;

	if( m_dwSMTime[SM_MAX_HP] > 0 )
	{
		aItemprop = prj.GetItemProp( II_CHR_FOO_COO_BULLHAMS );
		nAdjParamVal = aItemprop->nAdjParamVal2;
		SetDestParam( aItemprop->dwDestParam1, nAdjParamVal, NULL_CHGPARAM, 1 );	// fixed by jeff
	}

	if( m_dwSMTime[SM_MAX_HP50] > 0 )
	{
		aItemprop = prj.GetItemProp( II_CHR_FOO_COO_GRILLEDEEL );
		nAdjParamVal = m_nPlusMaxHitPoint;
		SetDestParam( aItemprop->dwDestParam1, nAdjParamVal, NULL_CHGPARAM, 1 );	// fixed by jeff
	}

	if( m_dwSMTime[SM_VELOCIJUMP] > 0 )
	{
		aItemprop = prj.GetItemProp( g_AddSMMode.dwSMItemID[SM_VELOCIJUMP] );
		if( aItemprop )
		{
			if( aItemprop->dwDestParam1 != -1 )
				SetDestParam( aItemprop->dwDestParam1, aItemprop->nAdjParamVal1, NULL_CHGPARAM );
			if( aItemprop->dwDestParam2 != -1 )
				SetDestParam( aItemprop->dwDestParam2, aItemprop->nAdjParamVal2, NULL_CHGPARAM );
		}
	}
}
#endif // __WORLDSERVER __CLIENT

void CMover::Serialize( CAr & ar )
{
	CCtrl::Serialize( ar );	//	25
	DWORD dwGold;

	if( ar.IsStoring() )	// STORING
	{
		ar << (u_short)m_dwMotion;
		ar << (u_char)m_bPlayer;
		ar << m_nHitPoint;
		ar << m_pActMover->GetState();
		ar << m_pActMover->GetStateFlag();
		ar << (u_char)m_dwBelligerence;
#if __VER >= 15 // __PETVIS
		ar << m_dwMoverSfxId;
#endif // __PETVIS
		if( m_bPlayer )	// PLAYER
		{
			ar.WriteString( m_szName );
			ar << GetSex();
			ar << (u_char)m_dwSkinSet;
			ar << (u_char)m_dwHairMesh;
			ar << m_dwHairColor;
			ar << (u_char)m_dwHeadMesh;
			ar << m_idPlayer;
			ar << (u_char)m_nJob;
			ar << (u_short)m_nStr;
			ar << (u_short)m_nSta;
			ar << (u_short)m_nDex;
			ar << (u_short)m_nInt;
			ar << (u_short)m_nLevel;
			ar << m_nFuel;
			ar << m_tmAccFuel;
			if( m_idGuild > 0 )
			{
				ar << (u_char)1;
				ar << m_idGuild;
				ar << m_idWar;
			}
			else
			{
				ar << (u_char)0;
			}
			ar << m_idGuildCloak;

			if( m_idparty > 0 )
			{
				ar << (u_char)1;
				ar << m_idparty;
				ar << m_idDuelParty;
			}
			else
			{
				ar << (u_char)0;
			}

			ar << (char)m_dwAuthorization;
			ar << m_dwMode;
			ar << m_dwStateMode;
 #ifdef __WORLDSERVER
			CItemElem* pItemElem = m_Inventory.GetAtId( m_dwUseItemId );
			if( pItemElem )
				ar << pItemElem->GetProp()->dwID;
			else
				ar << (OBJID)0;
 #else // __WORLDSERVER
			ar << m_dwUseItemId;
 #endif // __WORLDSERVER
#if __VER >= 8 // __S8_PK
			if( m_dwPKTime > 0 )
	#ifdef __WORLDSERVER
				ar << ( m_dwPKTime - GetTickCount() );
	#else // __WORLDSERVER
				ar << m_dwPKTime;
	#endif // __WORLDSERVER
			else
				ar << m_dwPKTime;
			ar << m_nPKValue;
			ar << m_dwPKPropensity;
			ar << m_dwPKExp;
#else // __VER >= 8 // __S8_PK
			ar << (u_short)m_nNumKill;
			ar << m_nSlaughter;
#endif // __VER >= 8 // __S8_PK
			ar << m_nFame;
			ar << (u_char)m_nDuel;
#if __VER >= 13 // __HONORABLE_TITLE			// 달인
			ar << m_nHonor;					// 달인선택 
#endif	// __HONORABLE_TITLE			// 달인
			{
				int i;
				for( i = 0; i < MAX_HUMAN_PARTS; i ++ )
				{
					ar << m_aEquipInfo[i].nOption;
				}
			}
			ar << m_nGuildCombatState;
			
			for( int j = 0 ; j < SM_MAX ; ++j )
				ar << m_dwSMTime[j];

			if( CObj::GetMethod() == METHOD_NONE )
			{
				ar << (u_short)m_nManaPoint;
				ar << (u_short)m_nFatiguePoint;
#if __VER >= 12 // __MOD_TUTORIAL
				ar << m_nTutorialState;
#else	// __MOD_TUTORIAL
				ar << (u_short)m_nFlightLv;
#endif	// __MOD_TUTORIAL
				ar << m_nFxp;
				
				dwGold = GetGold();
				ar << dwGold;
				ar << m_nExp1;
				ar << m_nSkillLevel;
				ar << m_nSkillPoint;
				ar << m_nDeathExp;
				ar << m_nDeathLevel;
				DWORD dwJobLv[MAX_JOB] = {0, };
				ar.Write( (void*)dwJobLv, sizeof(DWORD) * MAX_JOB );
				ar << m_idMarkingWorld;
				ar << m_vMarkingPos;
				ar << m_nQuestSize;
				ar.Write( m_aQuest, sizeof(QUEST) * m_nQuestSize );
				ar << m_nCompleteQuestSize;
				ar.Write( m_aCompleteQuest, sizeof(WORD) * m_nCompleteQuestSize ); 

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
				ar << m_nCheckedQuestSize;
				ar.Write( m_aCheckedQuest, sizeof(WORD) * m_nCheckedQuestSize ); 
#endif // __IMPROVE_QUEST_INTERFACE

				ar << m_idMurderer;
				ar << (short)m_nRemainGP;
				ar << (short)0;
				{
					for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
						ar << m_aEquipInfo[i].dwId;
				}
				ar.Write( (void*)m_aJobSkill, sizeof(SKILL) *  ( MAX_SKILL_JOB ) );
				
				ar << (BYTE)m_nCheerPoint << m_dwTickCheer - GetTickCount();

				ar << m_nSlot;
				for( int k = 0 ; k < 3 ; ++k )
					ar << m_dwGoldBank[k];
				for( k = 0 ; k < 3 ; ++k )
					ar << m_idPlayerBank[k];
				ar << m_nPlusMaxHitPoint;
				ar << m_nAttackResistLeft;
				ar << m_nAttackResistRight;
				ar << m_nDefenseResist;
#if __VER >= 8 // __CSC_VER8_5
				ar << m_nAngelExp;
				ar << m_nAngelLevel;
#endif // __CSC_VER8_5
				m_Inventory.Serialize( ar );
				for( k = 0 ; k < 3 ; ++k )
					m_Bank[k].Serialize( ar );
#if __VER >= 9	// __PET_0410
				ar << GetPetId();	// 소환 중인 펫 인벤토리 위치
#endif	// __PET_0410
#if __VER >= 11 // __SYS_POCKET
				m_Pocket.Serialize( ar );
#endif	// __SYS_POCKET
#ifdef __JEFF_9_20
				ar << m_dwMute;
#endif	// __JEFF_9_20
#if __VER >= 13 // __HONORABLE_TITLE			// 달인
				for( int i = 0 ; i < MAX_HONOR_TITLE ; ++i )
				{
					ar << m_aHonorTitle[i];

				}
#endif	// __HONORABLE_TITLE			// 달인
#if __VER >= 15 // __CAMPUS
				ar << m_idCampus;
				ar << m_nCampusPoint;
#endif // __CAMPUS
			}
			else if( CObj::GetMethod() == METHOD_EXCLUDE_ITEM )
			{
#	if defined (__WORLDSERVER) || defined(__CLIENT)
				ar.WriteString( m_vtInfo.GetTitle() );
#	endif
				u_char uSize	= 0;
				u_long uOffset	= ar.GetOffset();
				ar << uSize;
				for( u_char uParts = 0; uParts < MAX_HUMAN_PARTS; uParts++ )
				{
					CItemElem* pItemElem	= m_Inventory.GetEquip( uParts );
					if( pItemElem )
					{
						uSize++;
						ar << uParts;
						ar << (u_short)pItemElem->m_dwItemId;
						ar << pItemElem->m_byFlag;
					}
				}
				GETBLOCK( ar, pBlock, nBlockSize );
				*(UNALIGNED u_char*)( pBlock + uOffset )	= uSize;
#if __VER >= 9	// __PET_0410
				DWORD dwPetId	= NULL_ID;
#ifdef __PET_1024
				char* pszPetName	= "";
#endif	// __PET_1024
				CItemElem* pItemElem	= GetPetItem();
				if( pItemElem )
				{
					dwPetId		= MAKELONG( (WORD)pItemElem->m_pPet->GetIndex(), (WORD)pItemElem->m_pPet->GetLevel() );
#ifdef __PET_1024
					pszPetName	= const_cast<char*>( pItemElem->m_pPet->GetName() );
#endif	// __PET_1024
				}
				ar << dwPetId;
#ifdef __PET_1024
				ar.WriteString( pszPetName );
#endif	// __PET_1024
#endif	// __PET_0410
			}
		}
		else	// NPC
		{
			ar << (u_char)m_dwHairMesh << m_dwHairColor << (u_char)m_dwHeadMesh;
			ar.WriteString( m_szCharacterKey );

			u_char uSize	= 0;
			u_long uOffset	= ar.GetOffset();
			ar << uSize;
			if( IsEquipableNPC() )
			{
				CItemElem* pItemElem;
				for( u_char uParts = 0; uParts < MAX_HUMAN_PARTS; uParts++ )
				{
					pItemElem	= m_Inventory.GetEquip( uParts );
					if( pItemElem )
					{
						uSize++;
						ar << uParts;
						ar << (u_short)pItemElem->m_dwItemId;
					}
				}
			}
			GETBLOCK( ar, pBlock, nBlockSize );
			*(UNALIGNED u_char*)( pBlock + uOffset )	= uSize;


			ar << (u_char)m_bActiveAttack;
			ar << (u_char)m_nMovePattern;
			ar << (u_char)m_nMoveEvent;
			ar << m_nMoveEventCnt;
#if __VER >= 9	//__AI_0509
			ar << m_fSpeedFactor;
#endif	// __AI_0509
		}
#ifdef __BUFF_1107
		m_buffs.Serialize( ar );
#else	// __BUFF_1107
		m_SkillState.Serialize( ar );
#endif	// __BUFF_1107
	}
	else	// LOADING
	{
		m_dwMotion = 0;

		u_short nMotion;
		ar >> nMotion;							// m_dwMotion

		#if defined(__WORLDSERVER) || defined(__CLIENT)
				m_dwMotion = (DWORD)nMotion;	// 성별도 정해지지 않는 상태에서는 SetMotion호출하지 않는다.
		#else	// defined(__WORLDSERVER) || defined(__CLIENT)
			m_dwMotion = (DWORD)nMotion;
		#endif

		m_bPlayer	=
		m_dwBelligerence	=
		m_idGuild	=
		m_idWar	=
		m_idparty	=
		m_idDuelParty	=
		m_nDuel		= 
		m_dwRideItemIdx		= 0;

		ClearEquipInfo();
		
		ar >> (u_char&)m_bPlayer;

#ifdef __SKILL_0205
		SKILL	aJobSkill[MAX_SKILL_JOB];
		memcpy( aJobSkill, m_aJobSkill, sizeof(aJobSkill) );
#endif	// __SKILL_0205

		InitProp();

		m_nHitPoint	= 0;

		ar >> m_nHitPoint;
		DWORD dw1, dw2;
		ar >> dw1;
		ar >> dw2;
		m_pActMover->ClearStateFlag();
		m_pActMover->AddStateFlag( dw2 );
		m_pActMover->__ForceSetState( dw1 );
		ar >> (u_char&)m_dwBelligerence;
#if __VER >= 15 // __PETVIS
		ar >> m_dwMoverSfxId;
#endif // __PETVIS
		if( m_bPlayer )	// PLAYER
		{
			m_dwSkinSet		=
			m_dwHairMesh	=
			m_dwHeadMesh	=
			m_idPlayer	=
			m_nJob	=
			m_nStr	=
			m_nSta	=
			m_nDex	=
			m_nInt	=
			m_nLevel	=
			m_dwAuthorization	=
#if __VER < 8 // __S8_PK
			m_nNumKill	=
#endif // __VER < 8 // __S8_PK
									0;

			BYTE bySex;
			ar.ReadString( m_szName, 32 );
			ar >> bySex;
			SetSex( bySex );
			ar >> (u_char&)m_dwSkinSet;
			ar >> (u_char&)m_dwHairMesh;
			ar >> m_dwHairColor;
			ar >> (u_char&)m_dwHeadMesh;
#	if defined (__WORLDSERVER) || defined(__CLIENT)
			SetHairColor( m_dwHairColor );
#	endif	// __WORLDSERVER	//__CLIENT
			ar >> m_idPlayer;
			ar >> (u_char&)m_nJob;
			ar >> (u_short&)m_nStr;
			ar >> (u_short&)m_nSta;
			ar >> (u_short&)m_nDex;
			ar >> (u_short&)m_nInt;
			ar >> (u_short&)m_nLevel;
			ar >> m_nFuel;
			ar >> m_tmAccFuel;

			u_char u1;
			ar >> u1;
			if( u1 == 1 )
			{
				ar >> m_idGuild;
				ar >> m_idWar;
			}
			ar >> m_idGuildCloak;

			ar >> u1;
			if( u1 == 1 )
			{
				ar >> m_idparty;
				ar >> m_idDuelParty;
			}

			ar >> (char&)m_dwAuthorization;
			ar >> m_dwMode;
			ar >> m_dwStateMode;
#ifdef __WORLDSERVER
			OBJID dwItemTem;
			ar >> dwItemTem;
#else // __WORLDSERVER
			ar >> m_dwUseItemId;
#endif // __WORLDSERVER
#if __VER >= 8 // __S8_PK
			ar >> m_dwPKTime;
	#ifdef __WORLDSERVER
			if( m_dwPKTime > 0 )
				m_dwPKTime = GetTickCount() + m_dwPKTime;
	#endif // __WORLDSERVER
			ar >> m_nPKValue;
			ar >> m_dwPKPropensity;
			ar >> m_dwPKExp;
#else // __VER >= 8 // __S8_PK
			ar >> (u_short&)m_nNumKill;
			ar >> m_nSlaughter;
#endif // __VER >= 8 // __S8_PK
			ar >> m_nFame;
			ar >> (u_char&)m_nDuel;
#if __VER >= 13 // __HONORABLE_TITLE			// 달인
			int nTemp = -1;
			ar >> nTemp;
#ifdef __CLIENT
			if(m_nHonor != nTemp)// 달인선택 
			{
				m_nHonor = nTemp;
				SetTitle(CTitleManager::Instance()->GetTitle(m_nHonor));
			}
#else	// __CLIENT
			m_nHonor = nTemp;
#endif	// __CLIENT
#endif	// __HONORABLE_TITLE			// 달인
			{
				for( int i = 0; i < MAX_HUMAN_PARTS; i ++ )
				{
					ar >> m_aEquipInfo[i].nOption;
				}
			}
			ar >> m_nGuildCombatState;
			
			for( int j = 0 ; j < SM_MAX ; ++j )
				ar >> m_dwSMTime[j];
			// 장어구이, 신속의 두루마리 등의 예외처리는 m_nPlusMaxHitPoint등이 세트되고 OnApplySM() 실행한다.

			if( CObj::GetMethod() == METHOD_NONE )
			{
				m_nManaPoint	= 0;
				m_nFatiguePoint	= 0;
#if __VER < 12 // __MOD_TUTORIAL
				m_nFlightLv		= 0;
#endif	// __MOD_TUTORIAL
				m_nRemainGP		= 0;

				ar >> (u_short&)m_nManaPoint;
				ar >> (u_short&)m_nFatiguePoint;
#if __VER >= 12 // __MOD_TUTORIAL
				ar >> m_nTutorialState;
#ifdef __CLIENT
				g_Option.m_nTutorialLv	= m_nTutorialState;
#endif	// __CLIENT
#else	// __MOD_TUTORIAL
				ar >> (u_short&)m_nFlightLv;
#endif	// __MOD_TUTORIAL
				ar >> m_nFxp;

				ar >> dwGold;
				SetGold( dwGold );

				ar >> m_nExp1;
				ar >> m_nSkillLevel;
				ar >> m_nSkillPoint;
				ar >> m_nDeathExp;
				ar >> m_nDeathLevel; 
				DWORD dwJobLv[MAX_JOB];		// 사용하지 않음 
				ar.Read( (void*)dwJobLv, sizeof(DWORD) * MAX_JOB );
				ar >> m_idMarkingWorld;
				ar >> m_vMarkingPos;
				ar >> m_nQuestSize;
				ar.Read( m_aQuest, sizeof(QUEST) * m_nQuestSize ); 
				ar >> m_nCompleteQuestSize;
				ar.Read( m_aCompleteQuest, sizeof(WORD) * m_nCompleteQuestSize ); 

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
				ar >> m_nCheckedQuestSize;
				ar.Read( m_aCheckedQuest, sizeof(WORD) * m_nCheckedQuestSize );
#endif // __IMPROVE_QUEST_INTERFACE

				ar >> m_idMurderer;
				short n1, n2;		// n2는 사용하지 않는다.
				ar >> n1 >> n2;		
				m_nRemainGP	= n1;
				{
					for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
						ar >> m_aEquipInfo[i].dwId;
				}
#ifdef __SKILL_0205
				ar.Read( (void*)m_aJobSkill, sizeof(SKILL) *  ( MAX_SKILL_JOB ) );
				for( int i = 0 ; i < MAX_SKILL_JOB; i++)
					m_abUpdateSkill[i]	= (BOOL)( memcmp( &m_aJobSkill[i], &aJobSkill[i], sizeof(SKILL) ) != 0 );
#else	// __SKILL_0205
				ar.Read( (void*)m_aJobSkill, sizeof(SKILL) *  ( MAX_SKILL_JOB ) );
#endif	// __SKILL_0205

				m_nCheerPoint	= 0;
				ar >> (BYTE&)m_nCheerPoint >> m_dwTickCheer;
				m_dwTickCheer	+= GetTickCount();
				ar >> m_nSlot;
				for( int k = 0 ; k < 3 ; ++k )
					ar >> m_dwGoldBank[k];
				for( k = 0 ; k < 3 ; ++k )
					ar >> m_idPlayerBank[k];

				ar >> m_nPlusMaxHitPoint;
				ar >> m_nAttackResistLeft;
				ar >> m_nAttackResistRight;
				ar >> m_nDefenseResist;
#if __VER >= 8 //__CSC_VER8_5
				ar >> m_nAngelExp;
				ar >> m_nAngelLevel;
#endif // __CSC_VER8_5
				m_Inventory.Serialize( ar );
				for( k = 0 ; k < 3 ; ++k )
					m_Bank[k].Serialize( ar );

				CItemElem* pItemElem	= m_Inventory.GetEquip( PARTS_RIDE );
				if( pItemElem )
					m_dwRideItemIdx		= pItemElem->m_dwItemId;

#if __VER >= 9	// __PET_0410
				DWORD dwPetId;
				ar >> dwPetId;
				SetPetId( dwPetId );
	#ifdef __PET_1024
	#ifdef __CLIENT
				CPet* pPet	= GetPet();
				if( pPet )
					m_pet.SetName( const_cast<char*>( pPet->GetName() ) );
	#endif	// __CLIENT
	#endif	// __PET_1024
#endif	// __PET_0410
#if __VER >= 11 // __SYS_POCKET
				m_Pocket.Serialize( ar );
#endif	// __SYS_POCKET

#ifdef __JEFF_9_20
				ar >> m_dwMute;
#endif	// __JEFF_9_20
#if __VER >= 13 // __HONORABLE_TITLE	// 달인
#ifdef __CLIENT
				CTitleManager::Instance()->InitEarned();
#endif	// __CLIENT
				for( int l = 0 ; l < MAX_HONOR_TITLE ; ++l )
				{
					ar >> m_aHonorTitle[l];
#ifdef __CLIENT
					int nNeed = CTitleManager::Instance()->GetNeedCount(l, -1);
					if(m_aHonorTitle[l] >= nNeed && nNeed > 0)
					{
						CTitleManager::Instance()->AddEarned(l);	
					}
					else
					{
						// 획득된 타이틀이지만 요구사항을 충족못하게 될때
						if(CTitleManager::Instance()->IsEarned(l))
						{
							CTitleManager::Instance()->RemoveEarned(l);
						}
					}
#endif	// __CLIENT
				}
#endif	// __HONORABLE_TITLE			// 달인
#if __VER >= 15 // __CAMPUS
				ar >> m_idCampus;
				ar >> m_nCampusPoint;
#endif // __CAMPUS
			}
			else if( CObj::GetMethod() == METHOD_EXCLUDE_ITEM )
			{
			#if defined (__WORLDSERVER) || defined(__CLIENT)
				char szPVendor[MAX_VENDORNAME] = {0, };
				ar.ReadString( szPVendor, MAX_VENDORNAME );
				m_vtInfo.SetTitle( szPVendor );
			#endif
				for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
					m_aEquipInfo[i].dwId	= NULL_ID;

				u_char uSize;
				ar >> uSize;
				u_char uParts;
				u_short	u2;
				for( u_char uCount = 0; uCount < uSize; uCount++ )
				{
					ar >> uParts;
					ar >> u2;
					m_aEquipInfo[uParts].dwId	= (DWORD)u2;
					ar >> m_aEquipInfo[uParts].byFlag;
				}
				if( m_aEquipInfo[PARTS_RIDE].dwId != NULL_ID )
					m_dwRideItemIdx		= m_aEquipInfo[PARTS_RIDE].dwId;
#if __VER >= 9	// __PET_0410
				DWORD dwPetId;
				ar >> dwPetId;
				SetPetId( dwPetId );
	#ifdef __PET_1024
				char szPetName[MAX_PET_NAME]	= { 0,};
				ar.ReadString( szPetName, MAX_PET_NAME );
	#ifdef __CLIENT
				m_pet.SetName( szPetName );
	#endif	// __CLIENT
	#endif	// __PET_1024
#endif	// __PET_0410
			}

		#if defined(__WORLDSERVER) || defined(__CLIENT)
			if( m_dwRideItemIdx )
			{
				ItemProp *pItemProp	= prj.GetItemProp( m_dwRideItemIdx );
				if( pItemProp && m_nFuel == -1 )
					m_nFuel		= (int)( pItemProp->dwFFuelReMax * 0.2f );
			}
			OnApplySM();
		#endif	// __WORLDSERVER	// __CLIENT

		}
		else	// NPC
		{
			ar >> (u_char&)m_dwHairMesh >> m_dwHairColor >> (u_char&)m_dwHeadMesh;
			ar.ReadString( m_szCharacterKey, 32 );
		#if defined (__WORLDSERVER) || defined(__CLIENT)
			SetHairColor( m_dwHairColor );
		#endif	// __WORLDSERVER	//__CLIENT
			{
				for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
					m_aEquipInfo[i].dwId	= NULL_ID;
			}
			u_char uSize;
			ar >> uSize;
			if( IsEquipableNPC() )
			{
				u_char uParts;
				u_short	u2;
				for( u_char uCount = 0; uCount < uSize; uCount++ )
				{
					ar >> uParts;
					ar >> u2;
					m_aEquipInfo[uParts].dwId	= (DWORD)u2;
					/*
#ifdef __PERF_0229
					if( GetIndex() == 11 || GetIndex() == 12 )
					{
						int nAttr	= 0; //xRandom( 0,  5 );
						int nAttrLevel	= 0;
						//if( nAttr > 0 )
						//	nAttrLevel= xRandom( 8, 10 );
						int nLevel	= 0;
						//if( nAttr < 3 )
								nLevel	= xRandom( 9, 10 );
						int nOption	= nLevel;
						nOption	|= ( nAttrLevel << 24 );
						nOption	|= (nAttr << 16 );
						m_aEquipInfo[uParts].nOption	= nOption;
					}
#endif	// __PERF_0229
					*/
				}
			}

			m_bActiveAttack	=
			m_nMovePattern	=
			m_nMoveEvent	=
									0;

			ar >> (u_char&)m_bActiveAttack;
			ar >> (u_char&)m_nMovePattern;
			ar >> (u_char&)m_nMoveEvent;
			ar >> m_nMoveEventCnt;
		#ifdef __CLIENT
			if( *m_szCharacterKey )
			{
				LPCHARACTER pCharacter	= prj.GetCharacter( m_szCharacterKey );
				if( pCharacter )
				{
					_tcscpy( m_szName, pCharacter->m_strName );
					AllocShopInventory( pCharacter );
				}
			}
		#endif	// __CLIENT
#if __VER >= 9	//__AI_0509
			ar >> m_fSpeedFactor;
#endif	// __AI_0509
		}

#ifdef __BUFF_1107
		m_buffs.Serialize( ar );
#else	// __BUFF_1107
		m_SkillState.Serialize( ar );
#endif	// __BUFF_1107

	#ifdef __CLIENT
		InitInterpolation();
	#endif	// __CLIENT

	} // LOADING

#ifdef __CLIENT	
	m_fDestScale = GetScale().x;
	if( m_fDestScale > 10.0f )
	{
		LPCTSTR szErr = Error( "SerializeOpt:%f %s", m_fDestScale, GetName() );
		ADDERRORMSG( szErr );
	}
#endif
}
