#include "stdafx.h"
#include "defineText.h"
#include "User.h"
#include "DPDatabaseClient.h"
#include "DPSrvr.h"
#include "WorldMng.h"

extern	CUserMng	g_UserMng;

#ifndef __MEM_TRACE
#ifdef _DEBUG
#define new new( __FILE__, __LINE__ )
#endif
#endif	// __MEM_TRACE

#include "UserMacro.h"

extern	CDPSrvr		g_DPSrvr;
extern	CWorldMng	g_WorldMng;


/*  
//
// 자원 채집 시작
//
BOOL CUser::DoCollect( CMover *pTarget )
{
	BOOL bAble = TRUE;

	if( pTarget->m_idCollecter != NULL_ID )	// pTarget이 주인이 있는 몹이고 
	{
		CMover *pCollecter = prj.GetMover( pTarget->m_idCollecter );
		if( IsValidObj(pCollecter) )
		{
			if( m_idparty )		// 채집하려는 사람이 파티가 있는가?
			{
				if( m_idparty != pCollecter->m_idparty )		// 몹 주인과 같은 파티가 아니다.
					if( pTarget->m_nCollectOwnCnt > 0 )			// 아직 시간이 덜풀렸는가?
						bAble = FALSE;		// 못먹음.
			} else
			// 파티가 없을때
			{
				if( pTarget->m_idCollecter != GetId() )			// 몹 주인이 내가 아니냐?
					if( pTarget->m_nCollectOwnCnt > 0 )			// 아직 시간이 덜풀렸는가?
						bAble = FALSE;		// 못먹음.
			}
		}
//		else
			// 주인이 사라졌으면 걍 먹을 수 있음.
//
//		if( pTarget->m_idCollecter != GetId() )	// 그 주인이 내가 아니고
//		{
//			CMover *pCollecter = prj.GetMover( pTarget->m_idCollecter );
//			if( IsValidObj( pCollecter ) )
//			{
//				if( m_idparty && m_idparty != pCollecter->m_idparty )	// 같은 파티원도 아니고
//				{
//					if( pTarget->m_nCollectOwnCnt > 0 ) // 아직 시간이 덜 풀렸으면
//					{
//						AddDefinedText( TID_COLLECT_ERROROTHER );
//						return FALSE;				// pTarget은 this가 못 먹는다.
//					}
//				}
//			}
//		}

	}

	// 채집이 허용되지 않는 상황이면
	if( bAble == FALSE )
	{
		AddDefinedText( TID_COLLECT_ERROROTHER );		// 에러메시지 출력하고
		return FALSE;				// pTarget은 this가 못 먹는다.
	}
	

	ItemProp *pHandProp = GetActiveHandItemProp();
	if( pHandProp->dwItemKind3 != IK3_COLLECTER )
		return FALSE;
				
	if( CMover::DoCollect( pTarget ) == TRUE )
	{
		m_idTargetCollect = pTarget->GetId();	// pTarget을 채집중이라는것을 표시함.
//		pTarget->m_idCollecter = GetId();		// 타겟에다 내꺼라는 표시함.
		m_nCollect = 0;
		m_tmCollect = g_tmCurrent;
	} else
		return FALSE;
	
	return TRUE;
}
*/

#ifdef __XUZHU
int g_nCC = 0;
#endif

/*
//
//
// this가 pTarget을 채집한다.
int		CUser::OnActCollecting( void )
{
	CMover *pTarget = prj.GetMover( m_idTargetCollect );		// 채집대상의 포인터 꺼냄.
	if( IsInvalidObj( pTarget ) )	// 채집중 시간이 다되서 오브젝트가 사라지거나 거시기한 오브젝트면 걍 리턴.
		return TRUE;		// 채집 동작 해제하라.

	// 채집 모션이 진행되는동안 계속 들어온다.
	if( g_tmCurrent > m_tmCollect + 1000.0f )
	{
		m_tmCollect = g_tmCurrent;
		ItemProp *pHandProp = GetActiveHandItemProp();
		if( pHandProp == NULL )
		{
			Error( "CUser::OnActCollecting : pHandProp==NULL" );
			return TRUE;
		}
		if( pHandProp->dwItemKind3 != IK3_COLLECTER )		// 채집중 도구를 없애면 채집 취소.
			return TRUE;
		
		m_nCollect += (int)pHandProp->dwLoadingTime;				// 아이템의 채집능력에 따라 채집량을 누적.
		pTarget->m_nResource -= (int)pHandProp->dwLoadingTime;		// 채집한 만큼 타겟의 리소스에서 뺌.
		if( pTarget->m_nResource < 0 )
			pTarget->m_nResource = 0;
		if( m_nCollect >= pHandProp->nShellQuantity )	// 채집양이 맥스치를 넘어가면.
		{
			//보조석 1개 생성;
			BYTE bID;
			CItemElem itemElem;
			itemElem.m_dwItemId	= pTarget->GetProp()->dwSourceMaterial;		// 자원 생성.
			itemElem.m_nItemNum		= 1;
			if( itemElem.m_dwItemId != NULL_ID )
			{
				BOOL bRet = CreateItem( &itemElem, &bID );
				if( bRet == TRUE )
				{
					LogItemInfo aLogItem;
					aLogItem.Action = "C";
					aLogItem.SendName = pTarget->GetName();
					aLogItem.RecvName = GetName();
					aLogItem.WorldId = GetWorld()->GetID();
					aLogItem.Gold = GetGold();
					aLogItem.Gold2 = GetGold();
					aLogItem.ItemNo = itemElem.GetSerialNumber();
					aLogItem.ItemName = itemElem.GetProp()->szName;
					aLogItem.itemNumber = 1;
					g_DPSrvr.OnLogItem( aLogItem );
				}
			} else
				Error( "CUser::OnActCollecting : %s가 dwSourceMaterial이 -1", pTarget->GetName() );

			FLOAT fResRatio = (float)pTarget->m_nResource / pTarget->GetProp()->dwMaterialAmount;		// 몇퍼센트 남았는지 ...
			FLOAT fScale = pTarget->m_pModel->m_pModelElem->m_fScale;		// mdlDyna에 있던 scale값이 100%
			if( fResRatio > 1.0f )
			{
				Error( "OnActCollecting : %f %d %d", fResRatio, pTarget->m_nResource, (int)pTarget->GetProp()->dwMaterialAmount );
				fResRatio = 1.0f;
			}
			fScale *= fResRatio;
			D3DXVECTOR3 vScale = D3DXVECTOR3( fScale, fScale, fScale );
			pTarget->SetScale( vScale );
			if( fScale > 10.0f )
				Error( "OnActCollecting %s %f %f %d %d", GetName(), fScale, 
														pTarget->m_pModel->m_pModelElem->m_fScale, pTarget->m_nResource, pTarget->GetProp()->dwMaterialAmount );
			
			g_UserMng.AddSetScale( pTarget, fResRatio );
			
#ifdef __XUZHU
			g_nCC ++;
			TRACE( "item Create: %d\n", g_nCC );
#endif

			m_nCollect -= pHandProp->nShellQuantity;	// 오버된것 재조정.
		}
		if( pTarget->m_nResource <= 0 )							// 타겟의 리소스가 다떨어졌으면.
		{
			// 채집끝
#ifdef __XUZHU
			g_nCC = 0;
#endif
			pTarget->m_nResource = 0;							// 0으로만 해두고 타겟이 없어지는건 그쪽에서 알아서 한다.
			AddDefinedText( TID_COLLECT_EMPTY );	// 채집이 끝났습니다
			return TRUE;	// 채집동작 해제하라.
		}
#ifdef __XUZHU
		TRACE( "%d %d %d\n", m_tmCollect, pTarget->m_nResource, m_nCollect );
#endif
			
	}
	return 0;
}
*/

// tmMaxEscape : 탈출의 쿨타임
void CUser::AddEscape( DWORD tmMaxEscape )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_DO_ESCAPE;
	m_Snapshot.ar << tmMaxEscape;
}

// 액션포인트 갱신.
void CUser::AddSetActionPoint( int nAP )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_SETACTIONPOINT;
	m_Snapshot.ar << nAP;
}

void CUser::AddSetTarget( OBJID idTarget )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_SETTARGET;
	m_Snapshot.ar << idTarget;
}

// ----------------------------------------------------------------------------------
//
//   CUserMng Lux
//
// ----------------------------------------------------------------------------------
/*
void	CUserMng::AddDoCollect( CUser* pUser, OBJID idTarget )
{
	CAr ar;
	ar << GETID( pUser ) << SNAPSHOTTYPE_DO_COLLECT;
	ar << idTarget;
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pUser )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pUser )
}
*/

void	CUserMng::AddCreateSkillEffect( CMover *pAttacker, OBJID idTarget, DWORD dwSkill, DWORD dwLevel )
{
	CAr ar;
	ar << GETID( pAttacker ) << SNAPSHOTTYPE_ACTIVESKILL;
	ar << idTarget << dwSkill << dwLevel;
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pAttacker )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pAttacker )
}

// pMover가 스턴 된것을 주위 유저클라에 알림.
void	CUserMng::AddSetStun( CMover *pMover, int nMaxTime )
{
	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_SETSTUN;
	ar << nMaxTime;
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pMover )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void	CUserMng::AddDoApplySkill( CCtrl *pCtrl, OBJID idTarget, DWORD dwSkill, DWORD dwLevel )
{
	CAr ar;
	ar << GETID( pCtrl ) << SNAPSHOTTYPE_DOAPPLYUSESKILL;
	ar << idTarget << dwSkill << dwLevel;
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pCtrl )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pCtrl )
}

// SendActMsg를 주위 유저들에게 보냄 .
void	CUserMng::AddSendActMsg( CMover *pMover, OBJMSG dwMsg, int nParam1, int nParam2, int nParam3 )
{
	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_SENDACTMSG;
	ar << (int)dwMsg;
	ar << pMover->IsFly();
	ar << nParam1 << nParam2 << nParam3;
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pMover )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

// Mover를 어느방향으로 밀리게 하다.
// pMover : 밀리는 무버
// vPos : 밀리는 시점에서의 vPos - 정확한 동기화때문에 좌표와 각도까지 필요하다.
// fAngle : 밀리는 시점에서의 Angle - 
// fPushAngle : 미는 각도.
// fPower : 미는 힘.
void	CUserMng::AddPushPower( CMover *pMover, D3DXVECTOR3 vPos, FLOAT fAngle, FLOAT fPushAngle, FLOAT fPower )
{
	if( (pMover->m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL) || pMover->m_pActMover->GetState() & OBJSTA_STUN ) // 데미지 플라이중이면 리턴
		return;

	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_PUSHPOWER;
	ar << vPos;
	ar << fAngle;
	ar << fPushAngle << fPower;
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pMover )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void	CUserMng::AddRemoveSkillInfluence( CMover *pMover, WORD wType, WORD wID )
{
	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_REMOVESKILLINFULENCE;
	ar << wType;
	ar << wID;
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pMover )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void CUserMng::AddSetPosAngle( CCtrl* pCtrl, const D3DXVECTOR3 &vPos, FLOAT fAngle )
{
	CAr ar;
	
	ar << GETID( pCtrl ) << SNAPSHOTTYPE_SETPOSANGLE;
	ar << vPos << fAngle;
	
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pCtrl )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pCtrl )
}

/*
void CUser::AddRemoveSkillInfluence( WORD wType, WORD wID )
{
	if( IsDelete() )	return;
	
	m_Snapshot.Lock( theLineFile );
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_REMOVESKILLINFULENCE;
	m_Snapshot.ar << wType;
	m_Snapshot.ar << wID;
	
	m_Snapshot.Unlock( theLineFile );
}
*/