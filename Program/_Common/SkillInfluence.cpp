#include "stdafx.h"

#ifndef __BUFF_1107

#include "skillinfluence.h"
#include "defineSkill.h"
#include "..\_UnhandledException\ExceptionHandler.h"

#ifdef __WORLDSERVER
#include "User.h"
extern	CUserMng	g_UserMng;
#if __VER >= 9
#include "defineobj.h"
#endif	//
#include "DPDatabaseClient.h"
extern	CDPDatabaseClient	g_dpDBClient;
#endif // __WORLDSERVER


CSkillInfluence::CSkillInfluence()
{
	Init();
}

//
CSkillInfluence::~CSkillInfluence()
{
	Destroy();
}

//
void CSkillInfluence::Init( void )
{
	m_pMover		= NULL;
	memset( m_aSkillInfluence, 0, sizeof(m_aSkillInfluence) );
#if __VER < 8 //__CSC_VER8_3
	m_pEmptyNode = NULL;
#endif //__CSC_VER8_3
}

#if __VER >= 8 //__CSC_VER8_3
#if !( defined( __CORESERVER ) || defined( __DBSERVER ) ) 
SKILLINFLUENCE* CSkillInfluence::SortSkillArray()
{
	SKILLINFLUENCE *pList = m_aSkillInfluence, *pNode, *tempNode;
	int	i = 0;
	BOOL swapflag = FALSE;
	
	while(i < MAX_SKILLINFLUENCE)
	{
		pNode = pList++;
		if((int)pNode->wID == 0 && !swapflag)
		{
			tempNode = pNode;
			swapflag = TRUE;
		}
		if(swapflag && (int)pNode->wID > 0)
		{
			*tempNode = *pNode;
			Remove(pNode);
			tempNode++;
		}
		i++;
	}
	return tempNode;
}

#ifdef __PVPDEBUFSKILL
BOOL CSkillInfluence::InsertBuff(SKILLINFLUENCE *pNode, WORD wType, WORD wID, DWORD dwLevel, DWORD dwTime, DWORD dwAttackerID)
#else // __PVPDEBUFSKILL
BOOL CSkillInfluence::InsertBuff(SKILLINFLUENCE *pNode, WORD wType, WORD wID, DWORD dwLevel, DWORD dwTime)
#endif // __PVPDEBUFSKILL
{
	int	i = 0;
	int count = 0;
	SKILLINFLUENCE *pList = m_aSkillInfluence, *pNext, *pFirstSBuff;
	BOOL FirstFlag = FALSE;
	ItemProp* pItem = NULL;
	BOOL bFlag = FALSE;

	while(i < MAX_SKILLINFLUENCE)
	{
		pNext = pList++;
		if( pNext->wType == BUFF_SKILL )
		{
			if(!FirstFlag)
			{
				pFirstSBuff = pNext;
				FirstFlag = TRUE;
			}
			count++;
		}
		i++;
	}
#if __VER >= 9	// __PET_0410
	if( count == MAX_SKILLBUFF_COUNT && wType == BUFF_SKILL )
#else	// __PET_0410
	if( count == MAX_SKILLBUFF_COUNT && wType != BUFF_ITEM )
#endif	// __PET_0410
	{
		pItem = prj.GetSkillProp(pFirstSBuff->wID);
		if(pItem != NULL && pItem->nEvildoing >= 0) //맨 처음 버프가 디버프일 경우 밀 수 없다.
		{
			RemoveSkillInfluence(pFirstSBuff->wType, pFirstSBuff->wID);
#ifdef __PVPDEBUFSKILL
			bFlag = Set(pNode, wType, wID, dwLevel, dwTime, dwAttackerID);
#else // __PVPDEBUFSKILL
			bFlag = Set(pNode, wType, wID, dwLevel, dwTime);
#endif // __PVPDEBUFSKILL
		}
	}
	else
#ifdef __PVPDEBUFSKILL
		bFlag = Set(pNode, wType, wID, dwLevel, dwTime, dwAttackerID);
#else // __PVPDEBUFSKILL
		bFlag = Set(pNode, wType, wID, dwLevel, dwTime );
#endif // __PVPDEBUFSKILL

	return bFlag;
}
#endif // #if !( defined( __CORESERVER ) || defined( __DBSERVER ) ) i
#endif //__CSC_VER8_3

LPSKILLINFLUENCE CSkillInfluence::GetItemBuf( DWORD dwItemKind3 )
{
	char szGMCommand[64] = {0,};
	for( int i = 0; i < MAX_SKILLINFLUENCE; i ++ )	
	{
		if( m_aSkillInfluence[i].wType != BUFF_ITEM )
			continue;
		if( (int)m_aSkillInfluence[i].wID <= 0 )
			continue;
		ItemProp *pItemProp = prj.GetItemProp( m_aSkillInfluence[i].wID );
		if( pItemProp == NULL || pItemProp->dwItemKind3 != dwItemKind3 )
			continue;
		//찾음
		return &m_aSkillInfluence[i];
	}
	return NULL;
}

//
// 새로운 스킬 상태를 추가한다.
//
#ifdef __PVPDEBUFSKILL
BOOL CSkillInfluence::Set( WORD wType, WORD wID, DWORD dwLevel, DWORD dwTime, OBJID dwAttackerID )
#else // __PVPDEBUFSKILL
BOOL CSkillInfluence::Set( WORD wType, WORD wID, DWORD dwLevel, DWORD dwTime )
#endif // __PVPDEBUFSKILL
{
	SKILLINFLUENCE *pList = m_aSkillInfluence, *pNode;
	BOOL	bAdd = FALSE;

#if __VER >= 8 //__CSC_VER8_3
#if !( defined( __CORESERVER ) || defined( __DBSERVER ) )
	//Array Sort
	SKILLINFLUENCE *pNextNode;
	pNextNode = SortSkillArray();
#endif // !( defined( __CORESERVER ) || defined( __DBSERVER ) )
#endif //__CSC_VER8_3

#if __VER >= 8 //__Y_FLAG_SKILL_BUFF
	if( (int)dwTime < 0 )
	{
		LPCTSTR szErr = Error( "CSkillInfluence::Set : dwTime=%d, %d %d %d", (int)dwTime, wType, wID, dwLevel );
		return FALSE;
	}	
#else //__Y_FLAG_SKILL_BUFF
	if( (int)dwTime <= 0 )
	{
		LPCTSTR szErr = Error( "CSkillInfluence::Set : dwTime=%d, %d %d %d", (int)dwTime, wType, wID, dwLevel );
		return FALSE;
	}
#endif //__Y_FLAG_SKILL_BUFF
	
	pNode = Find( wType, wID );		// 이미 같은 스킬상태가 있었는지 찾음.

#if __VER >= 8 //__CSC_VER8_3
	if( pNode && pNode->dwLevel == dwLevel ) // 8차 Level Check Add.
#else
	if( pNode )		// 있었으면 시간만 재 세팅함.
#endif //__CSC_VER8_3
	{
		DWORD dwTimeRemain = pNode->tmCount - ( ::timeGetTime() - pNode->tmTime ); // 현재 스킬 남은 시간
#ifdef __PVPDEBUFSKILL
#ifdef __JEFF_11_1
		if( pNode ->wType == BUFF_ITEM && 
			( pNode->wID == II_SYS_SYS_SCR_PET_FEED_POCKET02 
#if __VER >= 12 // __PET_0519
				// 펫 영양제가 중복 사용될 수 있도록 수정
				// 이 후 더 추가된다면 프로퍼티를 확인하는 방법으로 수정하자
				|| pNode->wID == II_SYS_SYS_SCR_PET_TONIC_A
				|| pNode->wID == II_SYS_SYS_SCR_PET_TONIC_B 
#endif	// __PET_0519
			)
		)
			dwTime	+= dwTimeRemain;
#endif	// __JEFF_11_1

		if(	dwTime > dwTimeRemain )	// 남은 시간이 더 적은 경우만 시간 재세팅
			Set( pNode, pNode->wType, pNode->wID, pNode->dwLevel, dwTime, dwAttackerID );		// 시작 시간을 재 세팅.
#else // __PVPDEBUFSKILL
		Set( pNode, pNode->wType, pNode->wID, pNode->dwLevel, dwTime );		// 시작 시간을 재 세팅.
#endif // __PVPDEBUFSKILL
		return FALSE;
	}
	else	// 없었으면 추가함.
	{
		int		i = MAX_SKILLINFLUENCE;
#if __VER < 8 //__CSC_VER8_3		
		if( m_pEmptyNode )
		{
#ifdef __PVPDEBUFSKILL
			if( Set( m_pEmptyNode, wType, wID, dwLevel, dwTime, dwAttackerID ) )
#else // __PVPDEBUFSKILL
			if( Set( m_pEmptyNode, wType, wID, dwLevel, dwTime ) )
#endif // __PVPDEBUFSKILL
			{
				bAdd = TRUE;
				// 윤상이 확인 - Set에 성공했을 경우에만 m_pEmptyNode를 NULL로 초기화 해야 하지 않을까?
				m_pEmptyNode = NULL;	// 빈곳에 넣었으니 지워줘야 한다.
			}
		} 
		else
#endif //__CSC_VER8_3		
		{
#if __VER >= 8 && !( defined( __CORESERVER ) || defined( __DBSERVER ) ) //__CSC_VER8_3
			if( !(pNode && pNode->dwLevel >= dwLevel) )
			{
				if(pNode)
					RemoveSkillInfluence(pNode->wType, pNode->wID);
				// 윤상이 확인 - 버프가 꽉 차 있는 경우 InsertBuff에 실패헌다. 그럴 경우 bAdd가 TRUE면 버프는 들어가지 않으나, DST PARAM은 적용되어 해제되지 않는다.
#ifdef __PVPDEBUFSKILL
				if( InsertBuff(pNextNode, wType, wID, dwLevel, dwTime, dwAttackerID) )
#else // __PVPDEBUFSKILL
				if( InsertBuff(pNextNode, wType, wID, dwLevel, dwTime) )
#endif // __PVPDEBUFSKILL
					bAdd = TRUE;
				else
					bAdd = FALSE;
			}
			else
				bAdd = FALSE;
#else
			while( i-- )
			{
				pNode = pList++;
				if( (int)pNode->wID > 0 )	continue;	// 비어있는 공간을 찾음.

#ifdef __PVPDEBUFSKILL
				Set( pNode, wType, wID, dwLevel, dwTime, dwAttackerID );
#else // __PVPDEBUFSKILL
				Set( pNode, wType, wID, dwLevel, dwTime );
#endif // __PVPDEBUFSKILL
				bAdd = TRUE;
				break;
			}
#endif //__CSC_VER8_3
		}
		if( bAdd == FALSE )
		{
		}
	}
	return( bAdd );
}


//
#ifdef __PVPDEBUFSKILL
BOOL CSkillInfluence::Set( SKILLINFLUENCE *pNode, WORD wType, WORD wID, DWORD dwLevel, DWORD dwTime, OBJID dwAttackerID )
#else // __PVPDEBUFSKILL
BOOL CSkillInfluence::Set( SKILLINFLUENCE *pNode, WORD wType, WORD wID, DWORD dwLevel, DWORD dwTime )
#endif // __PVPDEBUFSKILL
{
//	locked
	if( pNode == NULL )
	{
		Error( "CSkillInfluence::Set : pNode가 NULL" );
		return FALSE;
	}
	
#ifdef __WORLDSERVER
	if( m_pMover->IsPlayer() && ((CUser*)m_pMover)->m_nDead )
	{
		ItemProp		*pSkillProp;
		
		if( wType == BUFF_SKILL )
		{
			pSkillProp = prj.GetSkillProp( wID );
			if( pSkillProp && pSkillProp->nEvildoing < 0 )
				return FALSE;
		}
	}
#endif // __WORLDSERVER

	pNode->wType	= wType;
	pNode->wID		= wID;
	pNode->dwLevel	= dwLevel;
	pNode->tmTime	= timeGetTime();
	pNode->tmCount = dwTime;
#ifdef __PVPDEBUFSKILL
	pNode->dwAttackerID = dwAttackerID;
#endif // __PVPDEBUFSKILL
#ifdef __WORLDSERVER
	BOOL bCharged = FALSE;
	if( pNode->wType == BUFF_ITEM
#if __VER >= 11 // __SYS_COLLECTING
		|| pNode->wType == BUFF_ITEM2
#endif	// __SYS_COLLECTING
		)
	{
		ItemProp *pItemProp = prj.GetItemProp( pNode->wID );
		if( pItemProp && pItemProp->bCharged ) // 상용화 아이템은 종료가 안되게 수정
			bCharged = TRUE;
	}
#if __VER >= 9	// __PET_0410
	else if( pNode->wType == BUFF_PET )
		bCharged	= TRUE;
#endif	// __PET_0410
#ifdef __DST_GIFTBOX
	else if( pNode->wType == BUFF_EQUIP )
		bCharged = TRUE;
#endif // __DST_GIFTBOX

	if( bCharged == FALSE && dwTime >= MIN(61) )
		if( m_pMover )
			Error( "SkillInflu::Set : Name=%s Time=%d dwID=%d Str=%d sta=%d int=%d dwLevel=%d GM=%d", m_pMover->GetName(), dwTime, pNode->wID, m_pMover->GetStr(), m_pMover->GetSta(), m_pMover->GetInt(), pNode->dwLevel, m_pMover->m_dwAuthorization );

	g_UserMng.AddSetSkillState( m_pMover, m_pMover, wType, wID, dwLevel, dwTime );
#endif //__WORLDSERVER
	return	TRUE;
}

//
void CSkillInfluence::Serialize( CAr & ar )
{
	int j, nMax = 0;
	DWORD	dwOdd;

	if( ar.IsStoring() )	// Saving
	{
		for( j = 0; j < MAX_SKILLINFLUENCE; j++ )		// 데이타 있는것만 개수 셈.
			if( m_aSkillInfluence[j].wID > 0 )
				nMax ++;

		ar << nMax;
		for( j = 0; j < MAX_SKILLINFLUENCE ; ++j )
		{
			if( m_aSkillInfluence[j].wID > 0 )
			{
				ar << m_aSkillInfluence[j].wType;
				ar << m_aSkillInfluence[j].wID;

#if __VER >= 11 // __SYS_COLLECTING
				if( m_aSkillInfluence[j].wType == BUFF_ITEM2 )
				{
					time_t t	= (time_t)m_aSkillInfluence[j].dwLevel - time_null();
					ar << t;
				}
				else
					ar << m_aSkillInfluence[j].dwLevel;
#else	// __SYS_COLLECTING
				ar << m_aSkillInfluence[j].dwLevel;
#endif	// __SYS_COLLECTING

#ifdef __WORLDSERVER			
				if( m_aSkillInfluence[j].tmCount )
				{
					if( m_aSkillInfluence[j].tmTime )
						dwOdd = m_aSkillInfluence[j].tmCount - (timeGetTime() - m_aSkillInfluence[j].tmTime);	// 남은시간 계산.
					else
						dwOdd = m_aSkillInfluence[j].tmCount;
				}
				else
					dwOdd = 0;
				if( (int)dwOdd < 0 )		dwOdd = 0;
				ar << dwOdd;		// 남은 시간을 저장.
#else // World
				ar << m_aSkillInfluence[j].tmCount;
#endif // not World
			}
		}
	} else
	// Loading
	{

		memset( m_aSkillInfluence, 0, sizeof(m_aSkillInfluence) );
#if __VER < 8 //__CSC_VER8_3
		m_pEmptyNode = NULL;
#endif //__CSC_VER8_3
		ar >> nMax;
		for( j = 0 ; j < nMax; ++j )		// 있는만큼만 부른다.
		{
			ar >> m_aSkillInfluence[j].wType;
			ar >> m_aSkillInfluence[j].wID;
#if __VER >= 11 // __SYS_COLLECTING
			if( m_aSkillInfluence[j].wType == BUFF_ITEM2 )
			{
				time_t t;
				ar >> t;
				m_aSkillInfluence[j].dwLevel	= time_null() + t;
			}
			else
				ar >> m_aSkillInfluence[j].dwLevel;
#else	// __SYS_COLLECTING
			ar >> m_aSkillInfluence[j].dwLevel;
#endif	// __SYS_COLLECTING
			ar >> dwOdd;
#ifdef __WORLDSERVER				
			BOOL bCharged = FALSE;
			if( m_aSkillInfluence[j].wType == BUFF_ITEM
#if __VER >= 11 // __SYS_COLLECTING
				|| m_aSkillInfluence[j].wType == BUFF_ITEM2
#endif	// __SYS_COLLECTING
				)
			{
				ItemProp *pItemProp = prj.GetItemProp( m_aSkillInfluence[j].wID );
				if( pItemProp )
				{
					if( pItemProp->bCharged ) // 상용화 아이템은 종료가 안되게 수정
						bCharged = TRUE;
				}
			}
#if __VER >= 9	// __PET_0410
			else if( m_aSkillInfluence[j].wType == BUFF_PET )
				bCharged	= TRUE;
#endif	// __PET_0410
#ifdef __DST_GIFTBOX
			else if( m_aSkillInfluence[j].wType == BUFF_EQUIP )
				bCharged	= TRUE;
#endif // __DST_GIFTBOX

			if( bCharged == FALSE && m_pMover && dwOdd >= MIN(61) )
			{
				Error( "SkillInflu::>Load : %s %d", m_pMover->GetName(), dwOdd );
				dwOdd = MIN(60);
			}
#endif //__WORLDSERVER
			m_aSkillInfluence[j].tmCount = dwOdd;			// 로딩했을때 남은시간을 로딩해서 세기 시작.
		} // for

	}
}

//
//


void CSkillInfluence::Reset( void )
{
	int i;
	for( i = 0; i < MAX_SKILLINFLUENCE; i ++ )		// 시작타이머를 클리어시켜 버프스킬을 다시 세팅함.
		m_aSkillInfluence[i].tmTime = 0;
}

#ifdef __CLIENT
DWORD CSkillInfluence::GetDisguise( void )
{
	char szGMCommand[64] = {0,};
	for( int i = 0; i < MAX_SKILLINFLUENCE; i ++ )	
	{
		if( m_aSkillInfluence[i].wType != BUFF_ITEM )
			continue;
		if( (int)m_aSkillInfluence[i].wID <= 0 )
			continue;
		ItemProp *pItemProp = prj.GetItemProp( m_aSkillInfluence[i].wID );
		if( pItemProp == NULL || pItemProp->dwItemKind3 != IK3_TEXT_DISGUISE )
			continue;
		//변신중인지 찾는다.
		char* p1 = strstr( pItemProp->szTextFileName, "/dis ");		//pItemProp->szTextFileName - /dis 20 /nodis
		if( p1 )
		{
			// 변신 번호를 찾는다.
			int n = atoi( p1 + 5 );	// +5 - "/dis " skip
			if( n > 0 )
			{
				MoverProp* pMoverProp	= prj.GetMoverProp( n );
				if( pMoverProp )
					return pMoverProp->dwID;
			}
		}
	}
	return NULL_ID;
}
#endif // __CLIENT

#if !( defined( __CORESERVER ) || defined( __DBSERVER ) ) 

BOOL CSkillInfluence::HasLikeItemBuf( DWORD dwItemKind3 )
{
	return LikeItemBuf( dwItemKind3 );
}

BOOL CSkillInfluence::LikeItemBuf( DWORD dwItemKind3 )
{
	for( int i = 0; i < MAX_SKILLINFLUENCE; i ++ )	
	{
		if( m_aSkillInfluence[i].wType == BUFF_ITEM 
#if __VER >= 11 // __SYS_COLLECTING
			|| m_aSkillInfluence[i].wType == BUFF_ITEM2
#endif	// __SYS_COLLECTING
			)
		{
			ItemProp *pItemProp = prj.GetItemProp( m_aSkillInfluence[i].wID );
			if( pItemProp && dwItemKind3 == pItemProp->dwItemKind3 )
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

void CSkillInfluence::RemoveLikeItemBuf( DWORD dwItemKind3 )
{
	for( int i = 0; i < MAX_SKILLINFLUENCE; i ++ )	
	{
		if( m_aSkillInfluence[i].wType == BUFF_ITEM
#if __VER >= 11 // __SYS_COLLECTING
			|| m_aSkillInfluence[i].wType == BUFF_ITEM2
#endif	// __SYS_COLLECTING
			)
		{
			ItemProp *pItemProp = prj.GetItemProp( m_aSkillInfluence[i].wID );
			if( pItemProp && dwItemKind3 == pItemProp->dwItemKind3 )
			{
				RemoveSkillInfluence( m_aSkillInfluence[i].wType, pItemProp->dwID );
			}
		}
	}
}



BOOL CSkillInfluence::RemoveAllSkillInfluence()
{
	int		i = MAX_SKILLINFLUENCE;
	SKILLINFLUENCE *pList = m_aSkillInfluence, *pNode;
	
	while( i-- )
	{
		pNode = pList++;

		if( (int)pNode->wID > 0 )
		{
			if( pNode->wType == BUFF_ITEM
#if __VER >= 11 // __SYS_COLLECTING
				|| pNode->wType == BUFF_ITEM2
#endif	// __SYS_COLLECTING
			)
			{
				ItemProp *pItemProp = prj.GetItemProp( pNode->wID );
				if( pItemProp && pItemProp->bCharged ) // 상용화 아이템은 종료가 안되게 수정
					continue;
			}
#ifdef __VER >= 12
			if( pNode->wType == BUFF_EQUIP )
				continue;
#endif // __VER >= 12
#if __VER >= 9	// __PET_0410
			if( pNode->wType == BUFF_PET )
				continue;
#endif	// __PET_0410

			RemoveSkillInfluence( pNode );
		}
	}

	return TRUE;
}		
/*
BOOL CSkillInfluence::IsEmpty()
{
	int     nCount = 0;
	int		i = MAX_SKILLINFLUENCE;
	SKILLINFLUENCE *pList = m_aSkillInfluence, *pNode;
	
	while( i-- )
	{
		pNode = pList++;
		
		if( (int)pNode->wID > 0 )
		{
			nCount ++;
			return FALSE;
		}
	}
	return TRUE;
}
*/
	
//
// 등록된 스킬상태들을 매 프로세스 검사하여 시간이 지난것들은 삭제한다.
//
void CSkillInfluence::Process( void )
{
#if defined(__WORLDSERVER)
	DWORD dwTimeCurrent	= g_tmCurrent;		// 현재 시간 읽음.
#else	//defined(__WORLDSERVER) //극단스킬종료시간판단문제로 클라이언트만 수정
	DWORD dwTimeCurrent	= timeGetTime();		// 현재 시간 읽음.
#endif  //defined(__WORLDSERVER)

	CMover *pMover = m_pMover;
	ItemProp *pProp = NULL;
#if __VER >= 11 // __SYS_COLLECTING
	time_t	t	= time_null();
#endif	// __SYS_COLLECTING

	int		i = MAX_SKILLINFLUENCE;
	SKILLINFLUENCE *pList = m_aSkillInfluence, *pNode;

	while( i-- )
	{
		pNode = pList++;
		if( (int)pNode->wID <= 0 )	continue;	// 비어있는 공간은 스킵.

		// 현재시각에서 시작시간을 빼면 흐른시간이 나온다. 흐른시간이 스킬지속시간을 넘어서면 삭제.
	#if defined(__WORLDSERVER)
		// 시간이 다되기전에 어떤특정 조건에 의해서 중간해제가 되는경우의 처리.
		if( pMover )
		{
			if( pNode->wType == BUFF_SKILL )
			{
				switch( pNode->wID )
				{
				case SI_JST_SUP_POISON:
				case SI_JST_SUP_BLEEDING:
				case SI_JST_SUP_ABSORB:
				case SI_ACR_SUP_YOYOMASTER:
				case SI_JST_YOYO_CRITICALSWING:
					if( pMover->GetActiveHandItemProp()->dwWeaponType != WT_MELEE_YOYO )		// 손에들고 있는 무기가 요요가 아닐때.
					{
						RemoveSkillInfluence( pNode );		// 해제.
						continue;
					}
					break;
				case SI_RAG_SUP_FASTATTACK:
				case SI_ACR_SUP_BOWMASTER:
					if( pMover->GetActiveHandItemProp()->dwWeaponType != WT_RANGE_BOW )		// 손에들고 있는 무기가 너클이 아닐때.
					{
						RemoveSkillInfluence( pNode );		// 해제.
						continue;
					}
					break;
#if __VER >= 10 // __LEGEND	//	9차 전승시스템	Neuz, World, Trans
				case SI_BLD_MASTER_ONEHANDMASTER:
					if( !pMover->IsDualWeapon() )		// 손에들고 있는 무기가 양손무기가 아닐때.
					{
						RemoveSkillInfluence( pNode );		// 해제.
						continue;
					}
					break;
				case SI_KNT_MASTER_TWOHANDMASTER:
					if( pMover->GetActiveHandItemProp()->dwHanded != HD_TWO )		// 손에들고 있는 무기가 양손무기가 아닐때.
					{
						RemoveSkillInfluence( pNode );		// 해제.
						continue;
					}
					break;
#endif	//__LEGEND	//	9차 전승시스템	Neuz, World, Trans
				case SI_MER_SUP_IMPOWERWEAPON:		// 
					{
						BOOL bRelease = FALSE;

						CItemElem *pItemElemR = pMover->GetWeaponItem();
						CItemElem *pItemElemL = pMover->GetLWeaponItem();

						if( pItemElemR && pItemElemL )	// 양손쥐고 있을때.
						{
							if( (pItemElemR && pItemElemR->m_bItemResist == SAI79::NO_PROP) &&		// 양손다 속성이 없을땐
								(pItemElemL && pItemElemL->m_bItemResist == SAI79::NO_PROP) )
								bRelease = TRUE;		// 해제
						} else
						if( pItemElemR && pItemElemL == NULL )	// 오른손만 쥐고 있을때.
						{
							if( pItemElemR && pItemElemR->m_bItemResist == SAI79::NO_PROP )		// 오른손에 속성이 없을땐
								bRelease = TRUE;		// 해제
						} else
						if( pItemElemR == NULL && pItemElemL == NULL )	// 양손다 없을때
						{
							bRelease = TRUE;		// 이때는 무조건 해제
						}

						if( bRelease )
						{
							RemoveSkillInfluence( pNode );		// 해제.
							continue;
						}
					}
					break;
				case SI_MER_SUP_BLAZINGSWORD:		// 
				case SI_MER_SUP_SWORDMASTER:		// 
					{
						CItemElem *pItemElem = pMover->GetLWeaponItem();
#ifndef __RIGHTHAND_SKILL
						if( pItemElem && pItemElem->GetProp()->dwWeaponType != WT_MELEE_SWD )		// 왼손에 무기를 들고 있고 들고 있는 무기가 칼이 아닐때.
						{
							RemoveSkillInfluence( pNode );		// 해제.
							continue;
						}
#endif // __RIGHTHAND_SKILL
					}
					if( pMover->GetActiveHandItemProp()->dwWeaponType != WT_MELEE_SWD )		// 오른손에들고 있는 무기가 칼이 아닐때.
					{
						RemoveSkillInfluence( pNode );		// 해제.
						continue;
					}
					break;
				case SI_MER_SUP_SMITEAXE:		// 
				case SI_MER_SUP_AXEMASTER:		// 
					{
						CItemElem *pItemElem = pMover->GetLWeaponItem();
#ifndef __RIGHTHAND_SKILL						
						if( pItemElem && pItemElem->GetProp()->dwWeaponType != WT_MELEE_AXE )		// 왼손에 무기를 들고 있고 들고 있는 무기가 도끼가 아닐때.
						{
							RemoveSkillInfluence( pNode );		// 해제.
							continue;
						}
#endif // __RIGHTHAND_SKILL
					}
					if( pMover->GetActiveHandItemProp()->dwWeaponType != WT_MELEE_AXE )		// 손에들고 있는 무기가 도끼가 아닐때.
					{
						RemoveSkillInfluence( pNode );		// 해제.
						continue;
					}
					break;
				case SI_BIL_PST_ASMODEUS:
					if( pMover->GetActiveHandItemProp()->dwWeaponType != WT_MELEE_KNUCKLE )		// 손에들고 있는 무기가 너클이 아닐때.
					{
						RemoveSkillInfluence( pNode );		// 해제.
						continue;
					}
					break;
				} // switch
			} // BuffSkill
		} else	// pMover
		{
			Error( "CSkillInfluence::Process : pMover가 널 %d", pNode->wID );
		}
	#endif // __WORLDSERVER

	#ifdef __CLIENT
		if( pMover )	// 이게 NULL일때도 있다 CWndWorld::m_SkillState 에서는 무버가 없다.
		{
			if( pNode->wType == BUFF_ITEM
#if __VER >= 11 // __SYS_COLLECTING
				|| pNode->wType == BUFF_ITEM2
#endif	// __SYS_COLLECTING
				)
				pProp = prj.GetItemProp( pNode->wID );
			else if( pNode->wType == BUFF_SKILL )
				pProp = prj.GetSkillProp( pNode->wID );
			// if BUFF_PET, pProp = NULL

			if( pProp )
			{
				if( pNode->bEffect == FALSE )	// 이펙트 생성안된거면.
				{
					// 계속 유지되는 이펙트는 dwSfxObj4를 사용한다.
					float fSkillTime = (float)pNode->tmCount / 1000.0f;		// nCount를 초단위로 변환.
					pNode->bEffect = TRUE;
					
					if( pProp->dwSfxObj4 != NULL_ID )
					{
					#if __VER >= 8 //__Y_FLAG_SKILL_BUFF
						if( (int)pNode->tmCount == 0 )
							CreateSfx( g_Neuz.m_pd3dDevice, pProp->dwSfxObj4, pMover->GetPos(), pMover->GetId(), D3DXVECTOR3( 0, 0, 0), pMover->GetId(), 0 );					
						else
					#endif //__Y_FLAG_SKILL_BUFF
							CreateSfx( g_Neuz.m_pd3dDevice, pProp->dwSfxObj4, pMover->GetPos(), pMover->GetId(), D3DXVECTOR3( 0, 0, 0), pMover->GetId(), fSkillTime );					
					}
				}
			}
		}
	#endif // CLIENT
		
		if( m_pMover && pNode->tmTime == 0 )		// Loading되고 난후 SetDestParam이 아직 세팅 안됐다.
		{
			if( pNode->wType == BUFF_SKILL )
			{
				ItemProp *pSkillProp = NULL;
				AddSkillProp *pAddSkillProp = NULL;
				BOOL bRet = m_pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, 
					pNode->wID, pNode->dwLevel, "CSkillInfluence::Process" );
				if( bRet )
				{
					m_pMover->ApplyParam( m_pMover, pSkillProp, pAddSkillProp, FALSE, 0 );	// SetDestParam적용. 이때는 클라로 따로 보내지 않음.
				}
			}
			else if( pNode->wType == BUFF_ITEM )
			{
				ItemProp *pItemProp = prj.GetItemProp( pNode->wID );
				if( pItemProp )
				{
					m_pMover->ApplyParam( m_pMover, pItemProp, NULL, FALSE, 0 );	// SetDestParam적용. 이때는 클라로 따로 보내지 않음.
				}
			}
#if __VER >= 9	// __PET_0410
			else if( pNode->wType == BUFF_PET )
			{
				int nDstParam	= HIWORD( pNode->dwLevel );
				int nAdjParam	= LOWORD( pNode->dwLevel );
				m_pMover->SetDestParam( nDstParam, nAdjParam, NULL_CHGPARAM, FALSE );
			}
#endif	// __PET_0410

			pNode->tmTime = dwTimeCurrent;
		}

		BOOL bRemove = TRUE;
		if( pNode->wType == BUFF_ITEM )
		{
			ItemProp *pItemProp = prj.GetItemProp( pNode->wID );
			if( pItemProp && pItemProp->dwSkillTime == 999999999 )
				bRemove = FALSE;
		}
#ifdef __DST_GIFTBOX
		else if( pNode->wType == BUFF_EQUIP )
			bRemove = FALSE;
#endif // __DST_GIFTBOX
		
		if( m_pMover != NULL && pNode->wType == BUFF_SKILL )
		{
			ItemProp *pSkillProp = NULL;
			AddSkillProp *pAddSkillProp = NULL;
			BOOL bRet = m_pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, 
				pNode->wID, pNode->dwLevel, "CSkillInfluence::Process" );

			if( pAddSkillProp )
			{
				if( pAddSkillProp->dwSkillTime > 0 )
					bRemove = TRUE;
				else
					bRemove = FALSE;
			}
		}
#if __VER >= 9	// __PET_0410
		if( pNode->wType == BUFF_PET )
			bRemove	= FALSE;
#endif	// __PET_0410

#if __VER >= 11 // __SYS_COLLECTING
		if( pNode->wType == BUFF_ITEM2 )
		{
			if( t >= pNode->dwLevel )
			{
#ifdef __WORLDSERVER
				ItemProp *pItemProp		= prj.GetItemProp( pNode->wID );
				if( pItemProp )
					g_dpDBClient.SendLogSMItemUse( "2", (CUser*)m_pMover, NULL, pItemProp );
#endif	// __WORLDSERVER
				RemoveSkillInfluence( pNode );
			}
		}
		else
#endif	// __SYS_COLLECTING
		if( bRemove && dwTimeCurrent - pNode->tmTime >= pNode->tmCount )	// tmCount시간만큼 경과했으면 종료.
		{
#if __VER >= 10
#ifdef __WORLDSERVER
			if( pNode->wType == BUFF_SKILL )
			{
				if( pNode->wID == SI_GEN_BURN )		// 화상
				{
					// 5000 데미지 주자.
					ItemProp* pSkillProp	= prj.GetSkillProp( SI_GEN_BURN );
					int nMin	= pSkillProp->dwAbilityMin;
					int nMax	= pSkillProp->dwAbilityMax;
					int nDamage		= xRandom( nMin, nMax );
					m_pMover->SendDamage( AF_FORCE, pNode->dwAttackerID, nDamage );
					g_UserMng.AddCreateSfxObj( m_pMover, XI_SKILL_ELE_FIRE_BURINGFIELD02 );
				}
			}
			else if( pNode->wType == BUFF_ITEM )
			{
				if( pNode->wID == II_SYS_SYS_SCR_PET_FEED_POCKET02 )
				{
					ItemProp *pItemProp		= prj.GetItemProp( pNode->wID );
					if( pItemProp )
						g_dpDBClient.SendLogSMItemUse( "2", (CUser*)m_pMover, NULL, pItemProp );
				}
			}
#endif	// __WORLDSERVER
#endif	//
			RemoveSkillInfluence( pNode );
		}
	}
}

#endif // #if !( defined( __CORESERVER ) || defined( __DBSERVER ) ) 


#if !( defined( __CORESERVER ) || defined( __DBSERVER ) ) 
void CSkillInfluence::RemoveSkillInfluence( SKILLINFLUENCE *pSkillInfluence )
{
	if( m_pMover )
	{
		if( pSkillInfluence->wType == BUFF_ITEM		// 아이템의 지속효과가 다되어 끝냄.
#if __VER >= 11 // __SYS_COLLECTING
			|| pSkillInfluence->wType == BUFF_ITEM2
#endif	// __SYS_COLLECTING
			)
		{
#ifdef __WORLDSERVER
			ItemProp *pItemProp = prj.GetItemProp( pSkillInfluence->wID );
			if( pItemProp )
			{
				m_pMover->ResetDestParam( pItemProp->dwDestParam1, pItemProp->nAdjParamVal1, TRUE );
				m_pMover->ResetDestParam( pItemProp->dwDestParam2, pItemProp->nAdjParamVal2, TRUE );
				if( 0 < strlen( pItemProp->szTextFileName ) && pItemProp->dwItemKind3 != IK3_ANGEL_BUFF )
				{
				#if __VER >= 8 //__Y_FLAG_SKILL_BUFF
					// 해당 변신구의 발동 스킬을 검사하여 삭제시킨다..
					if( pItemProp->dwActiveSkill != NULL_ID )
					{
						RemoveSkillInfluence( BUFF_SKILL, pItemProp->dwActiveSkill );
					}
				#endif //__Y_FLAG_SKILL_BUFF
					char szGMCommand[64] = {0,};
					CString szGMText = pItemProp->szTextFileName;
					szGMText.Replace( '(', '\"' );
					szGMText.Replace( ')', '\"' ); 
					int nGMCount = szGMText.Find( "/", 1 );
					if( nGMCount != -1 )
						strncpy( szGMCommand, &pItemProp->szTextFileName[nGMCount], szGMText.GetLength() );
					else
						Error( "GMText 해제하는 것이 없음 : %d, %s", pItemProp->dwID, pItemProp->szTextFileName );
					ParsingCommand( szGMCommand, (CMover*)m_pMover, TRUE );
				}
			}
#endif
		} 
		else if( pSkillInfluence->wType == BUFF_SKILL )
		{
			m_pMover->OnEndSkillState( pSkillInfluence->wID, pSkillInfluence->dwLevel );		// 스킬상태가 하나 끝나면 핸들러 호출.
		}
#if __VER >= 9	// __PET_0410
#ifdef __WORLDSERVER
		else if( pSkillInfluence->wType == BUFF_PET )
		{
			int nDstParam	= HIWORD( pSkillInfluence->dwLevel );
			int nAdjParam	= LOWORD( pSkillInfluence->dwLevel );
			m_pMover->ResetDestParam( nDstParam, nAdjParam, TRUE );
		}
#endif	// __WORLDSERVER
#endif	// __PET_0410
	}
#ifdef __WORLDSERVER
	g_UserMng.AddRemoveSkillInfluence( m_pMover, pSkillInfluence->wType, pSkillInfluence->wID );
#endif // __WORLDSERVER
	Remove( pSkillInfluence );	// 이걸 젤 마지막에 불러줘야 한다. 위에서 pSkillInfl->wType 이렇게 참조하니깐.
	
}

// dwSkill을 찾아서 없앰.
#ifdef __PVPDEBUFSKILL
BOOL	CSkillInfluence::RemoveSkillInfluenceFromID( OBJID dwAttackerID )
{
	int		i = MAX_SKILLINFLUENCE;
	SKILLINFLUENCE *pList = m_aSkillInfluence, *pNode;
	BOOL	bRet = FALSE;
	
	if( m_pMover == NULL )	return FALSE;
	
	while( i-- )
	{
		pNode = pList++;
		if( pNode->wType == BUFF_SKILL )
		{
			if( ((int)pNode->wID > 0) && (pNode->dwAttackerID == dwAttackerID) )	// 스킬 있는걸 찾음.
			{
				ItemProp		*pSkillProp;
				AddSkillProp	*pAddSkillProp;
				
				// 스킬 프로퍼티를 꺼내서.
				m_pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, pNode->wID, pNode->dwLevel, "RemoveAllSkillDebuff" );
				if( pSkillProp && pSkillProp->nEvildoing < 0 )
					RemoveSkillInfluence( pNode );
			}
		}
	} // loop
	
	return bRet;
}
#endif // __PVPDEBUFSKILL

BOOL	CSkillInfluence::RemoveSkillInfluence( WORD wType, WORD wID )
{
	SKILLINFLUENCE* pSkillInfluence;
	
	pSkillInfluence = Find( wType, wID );
	if( pSkillInfluence )
	{
		RemoveSkillInfluence( pSkillInfluence );
		return TRUE;
	} else
	{
		return FALSE;
	}
}

// dwChrState의 상태를 가진 스킬을 찾아 모두 해제함
BOOL	CSkillInfluence::RemoveSkillInfluenceState( DWORD dwChrState )
{
	int		i = MAX_SKILLINFLUENCE;
	SKILLINFLUENCE *pList = m_aSkillInfluence, *pNode;
	BOOL	bRet = FALSE;
	
	if( m_pMover == NULL )	return FALSE;

	int	j;
				
	while( i-- )
	{
		pNode = pList++;
		if( pNode->wType == BUFF_SKILL )
		{
			if( (int)pNode->wID > 0 )	// 스킬 있는걸 찾음.
			{
				ItemProp		*pSkillProp;
				AddSkillProp	*pAddSkillProp;
				
				// 스킬 프로퍼티를 꺼내서.
				m_pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, pNode->wID, pNode->dwLevel, "RemoveSkillInfluence" );
				for( j = 0; j < 2; j ++ )
				{
					if( pAddSkillProp->dwDestParam[j] == DST_CHRSTATE )		// DestParam을 검사.
					{
						if( pAddSkillProp->nAdjParamVal[j] & dwChrState )	// dwChrState 상태를 가지고 있는 스킬이면
						{
							RemoveSkillInfluence( pNode );		// 이상태 해제
							bRet = TRUE;
							continue;
						}
					} 

				}
			}
		} else
		if( pNode->wType == BUFF_ITEM
#if __VER >= 11 // __SYS_COLLECTING
			|| pNode->wType == BUFF_ITEM2
#endif	// __SYS_COLLECTING
			)
		{
			if( pNode->wID > 0 )
			{
				ItemProp *pItemProp = prj.GetItemProp( pNode->wID );
				if( pItemProp )
				{
//#ifdef __PROP_0827	// ApplyParam에서 인자 2개만 허용했으므로, 3이 아니다.
//					for( j = 0; j < 3; j ++ )
//#else	// __PROP_0827
					for( j = 0; j < 2; j ++ )
//#endif	// __PROP_0827
					{
						if( pItemProp->dwDestParam[j] == DST_CHRSTATE )		// DestParam을 검사.
						{
							if( pItemProp->nAdjParamVal[j] & dwChrState )	// dwChrState 상태를 가지고 있는 스킬이면
							{
								RemoveSkillInfluence( pNode );		// 이상태 해제
								bRet = TRUE;
								continue;
							}
						} 
					}
				}
			}
		}
	} // loop
	
	return bRet;
}

#if __VER >= 11 // __MA_VER11_06				// 확율스킬 효과수정 world,neuz
// dwDestParam 상태를 가진 스킬을 찾아 모두 해제함
BOOL	CSkillInfluence::RemoveSkillInfluenceDestParam( DWORD dwDestParam )
{
	int		i = MAX_SKILLINFLUENCE;
	SKILLINFLUENCE *pList = m_aSkillInfluence, *pNode;
	BOOL	bRet = FALSE;
	
	if( m_pMover == NULL )	return FALSE;

	int	j;
				
	while( i-- )
	{
		pNode = pList++;
		if( pNode->wType == BUFF_SKILL )
		{
			if( (int)pNode->wID > 0 )	// 스킬 있는걸 찾음.
			{
				ItemProp		*pSkillProp;
				AddSkillProp	*pAddSkillProp;
				
				// 스킬 프로퍼티를 꺼내서.
				m_pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, pNode->wID, pNode->dwLevel, "RemoveSkillInfluence" );
				for( j = 0; j < 2; j ++ )
				{
					// 현재는 나쁜마법 해제에만 사용한다.
					// 추후 범용 사용의 경우, 파라미터 nEvildoing을 포함한다.
					if( pSkillProp->nEvildoing < 0 && pAddSkillProp->dwDestParam[j] == dwDestParam )		// DestParam을 검사.
					{
						RemoveSkillInfluence( pNode );		// 이상태 해제
						bRet = TRUE;
						continue;
					} 

				}
			}
		}
	} // loop
	return bRet;
}
#endif // __MA_VER11_06				// 확율스킬 효과수정 world,neuz

// dwChrState의 상태를 가진 스킬을 찾아 모두 해제함
BOOL	CSkillInfluence::RemoveAllSkillDebuff( void )
{
	int		i = MAX_SKILLINFLUENCE;
	SKILLINFLUENCE *pList = m_aSkillInfluence, *pNode;
	BOOL	bRet = FALSE;
	
	if( m_pMover == NULL )	return FALSE;
	
	while( i-- )
	{
		pNode = pList++;
		if( pNode->wType == BUFF_SKILL )
		{
			if( (int)pNode->wID > 0 )	// 스킬 있는걸 찾음.
			{
				ItemProp		*pSkillProp;
				AddSkillProp	*pAddSkillProp;
				
				// 스킬 프로퍼티를 꺼내서.
				m_pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, pNode->wID, pNode->dwLevel, "RemoveAllSkillDebuff" );
				if( pSkillProp && pSkillProp->nEvildoing < 0 )
					RemoveSkillInfluence( pNode );
			}
		} else
		if( pNode->wType == BUFF_ITEM
#if __VER >= 11 // __SYS_COLLECTING
			|| pNode->wType == BUFF_ITEM2
#endif	// __SYS_COLLECTING
			)
		{
			if( pNode->wID > 0 )
			{
				ItemProp *pItemProp = prj.GetItemProp( pNode->wID );
				if( pItemProp && pItemProp->nEvildoing < 0 )
					RemoveSkillInfluence( pNode );
			}
		}
	} // loop
	
	return bRet;
}

// 버프스킬들을 찾아 모두 제거.
BOOL	CSkillInfluence::RemoveAllSkillBuff( void )
{
	int		i = MAX_SKILLINFLUENCE;
	SKILLINFLUENCE *pList = m_aSkillInfluence, *pNode;
	BOOL	bRet = FALSE;
	
	if( m_pMover == NULL )	return FALSE;
	
	while( i-- )
	{
		pNode = pList++;
		if( pNode->wType == BUFF_SKILL )
		{
			if( (int)pNode->wID > 0 )	// 스킬 있는걸 찾음.
			{
				ItemProp		*pSkillProp;
				AddSkillProp	*pAddSkillProp;
				
				// 스킬 프로퍼티를 꺼내서.
				m_pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, pNode->wID, pNode->dwLevel, "RemoveAllSkillBuff" );
				if( pSkillProp && pSkillProp->nEvildoing >= 0 )
				{
					RemoveSkillInfluence( pNode );
				}
			}
		}
/*		else
		if( pNode->wType == BUFF_ITEM )
		{
			if( pNode->wID > 0 )
			{
				ItemProp *pItemProp = prj.GetItemProp( pNode->wID );
				if( pItemProp && pItemProp->nEvildoing < 0 )
					RemoveSkillInfluence( pNode );
			}
		} */
	} // loop
	
	return bRet;
	
}

#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
// 모든버프들을 찾아 모두 제거.
BOOL	CSkillInfluence::RemoveAllBuff( void )
{
	int		i = MAX_SKILLINFLUENCE;
	SKILLINFLUENCE *pList = m_aSkillInfluence, *pNode;
	BOOL	bRet = FALSE;
	
	if( m_pMover == NULL )	return FALSE;
	
	while( i-- )
	{
		pNode = pList++;

		if( pNode->wID > 0 )
			RemoveSkillInfluence( pNode );
	} // loop
	
	return bRet;
}
#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz


// 버프스킬(Evildoing >= 0)하나만 제거.
BOOL CSkillInfluence::RemoveOneSkillBuff( void )
{
	int		i = MAX_SKILLINFLUENCE;
	SKILLINFLUENCE *pList = m_aSkillInfluence, *pNode;
	
	if( m_pMover == NULL )	return FALSE;
	
	while( i-- )
	{
		pNode = pList++;
		if( pNode->wType == BUFF_SKILL )
		{
			if( (int)pNode->wID > 0 )	// 스킬 있는걸 찾음.
			{
				ItemProp		*pSkillProp;
				AddSkillProp	*pAddSkillProp;
				
				// 스킬 프로퍼티를 꺼내서.
				m_pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, pNode->wID, pNode->dwLevel, "RemoveAllSkillBuff" );
				if( pSkillProp && pSkillProp->nEvildoing >= 0 )
				{
					RemoveSkillInfluence( pNode );
					return TRUE;
				}
			}
		}
/*		else
		if( pNode->wType == BUFF_ITEM )
		{
			if( pNode->wID > 0 )
			{
				ItemProp *pItemProp = prj.GetItemProp( pNode->wID );
				if( pItemProp && pItemProp->nEvildoing < 0 )
					RemoveSkillInfluence( pNode );
			}
		} */
	} // loop
	
	return FALSE;
}

#if __VER >= 9	// __PET_0410
BOOL CSkillInfluence::RemovePet( void )
{
	SKILLINFLUENCE*	pSkillInfluence	= FindPet();
	if( pSkillInfluence )
	{
		RemoveSkillInfluence( pSkillInfluence );
#if __VER >= 12 // __PET_0519
		CItemElem* pItem	= m_pMover->GetPetItem();
		if( pItem )
			m_pMover->ResetDestParamRandomOptExtension( pItem );
#endif	// __PET_0519
		return TRUE;
	}
	return FALSE;
}
#endif	// __PET_0410

#endif // #if !( defined( __CORESERVER ) || defined( __DBSERVER ) )

#endif	// __BUFF_1107