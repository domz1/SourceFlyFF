#include "stdafx.h"
#include "defineText.h"
#include "defineSkill.h"
#include "party.h"

#ifdef __CORESERVER
#include "dpcoresrvr.h"
extern	CDPCoreSrvr g_dpCoreSrvr;
#include "player.h"
extern	CPlayerMng	g_PlayerMng;
#endif // __CORESERVER

#ifdef __WORLDSERVER
#include "worldmng.h"
#include "User.h"
extern	CUserMng	g_UserMng;
extern CWorldMng   g_WorldMng;
#include "dpcoreclient.h"
extern	CDPCoreClient	g_DPCoreClient;
#include "dpdatabaseclient.h"
extern	CDPDatabaseClient	g_dpDBClient;
#if __VER >= 12 // __LORD
#include "slord.h"
#endif	// __LORD
#endif	// __WORLDSERVER

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

#ifdef __COLOSSEUM
#include "Colosseum.h"
#endif // __COLOSSEUM

#ifndef __VM_0820
#ifndef __MEM_TRACE
	#ifdef __VM_0819
	MemPooler<CParty>*	CParty::m_pPool		= new MemPooler<CParty>( 512, "CParty" );
	#else	// __VM_0819
	MemPooler<CParty>*	CParty::m_pPool		= new MemPooler<CParty>( 512 );
	#endif	// __VM_0819
#endif	// __MEM_TRACE
#endif	// __VM_0820

#if __VER >= 14 // __INSTANCE_DUNGEON
#include "InstanceDungeonParty.h"
#endif // __INSTANCE_DUNGEON
/*--------------------------------------------------------------------------------*/

CParty::CParty()
{
	m_uPartyId	= 0;
	m_nSizeofMember	= 0;
	memset( m_sParty, 0, sizeof(m_sParty) );
	m_nLevel = 1;
	m_nExp = m_nPoint = 0;
	m_nTroupsShareExp = m_nTroupeShareItem = 0;
	m_nKindTroup = 0;							// 단막극단
	m_nReferens = 0;
	m_nGetItemPlayerId = 0;
	for( int i = 0 ; i < MAX_PARTYMODE ; i++ )
	{
		m_nModeTime[i] = 0;
	}
	m_idDuelParty = 0;
#ifdef __WORLDSERVER
	m_dwWorldId = 0;
#endif // __WORLDSERVER
}

CParty::~CParty()
{
}

void CParty::InitParty()
{
	m_uPartyId	= 0;
	m_nSizeofMember	= 0;
	memset( m_sParty, 0, sizeof(m_sParty) );
	m_nLevel = 1;
	m_nExp = m_nPoint = 0;
	m_nTroupsShareExp = m_nTroupeShareItem = 0;
	m_nKindTroup = 0;							// 단막극단
	m_nReferens = 0;	
	for( int i = 0 ; i < MAX_PTMEMBER_SIZE ; i++ )
	{
		m_aMember[i].m_uPlayerId	= 0;
		m_aMember[i].m_tTime = CTime::GetCurrentTime();
		m_aMember[i].m_bRemove = FALSE;
#if __VER < 11 // __SYS_PLAYER_DATA
		m_aMember[i].m_nLevel = m_aMember[i].m_nJob = 0;
		m_aMember[i].m_szName[0] = '\0';
#endif	// __SYS_PLAYER_DATA
	}
}

#ifndef __CORESERVER
CMover* CParty::GetLeader( void ) 
{ 
#ifdef __WORLDSERVER
	CMover *pLeader = (CMover *)g_UserMng.GetUserByPlayerID( m_aMember[0].m_uPlayerId );	// 리더의 포인터
	return pLeader;
#else
	CMover *pLeader = (CMover *)prj.GetUserByID( m_aMember[0].m_uPlayerId );	// 리더의 포인터
	return pLeader;
#endif
}
#endif // __CORESERVER

int CParty::FindMember( u_long uPlayerId )
{
	for( int i = 0; i < m_nSizeofMember; i++ )
		if( m_aMember[i].m_uPlayerId == uPlayerId )
			return i;
	return -1;
}

#if __VER >= 11 // __SYS_PLAYER_DATA
BOOL CParty::NewMember( u_long uPlayerId )
#else	// __SYS_PLAYER_DATA
BOOL CParty::NewMember( u_long uPlayerId, LONG nLevel, LONG nJob, BYTE nSex, LPSTR szName )
#endif	// __SYS_PLAYER_DATA
{
#if __VER < 11 // __SYS_PLAYER_DATA
	if( szName == NULL )
		return FALSE;
#endif	// __SYS_PLAYER_DATA
	
	if( IsMember( uPlayerId ) == FALSE && m_nSizeofMember < MAX_PTMEMBER_SIZE )
	{
		m_aMember[m_nSizeofMember].m_uPlayerId = uPlayerId;
#if __VER < 11 // __SYS_PLAYER_DATA
		m_aMember[m_nSizeofMember].m_nLevel = nLevel;
		m_aMember[m_nSizeofMember].m_nJob = nJob;
		m_aMember[m_nSizeofMember].m_nSex = nSex;
#endif	// __SYS_PLAYER_DATA
		m_aMember[m_nSizeofMember].m_bRemove	= FALSE;
#if __VER < 11 // __SYS_PLAYER_DATA
		strcpy( m_aMember[m_nSizeofMember].m_szName, szName );
#endif	// __SYS_PLAYER_DATA
		m_nSizeofMember++;
		return TRUE;
	}
	return FALSE;
}

BOOL CParty::DeleteMember( u_long uPlayerId )
{
	int Findid = FindMember( uPlayerId );
	if( Findid >= 0)
	{
		for( int i = Findid ; i < m_nSizeofMember-1 ; i++ )
		{
			memcpy( &m_aMember[i], &m_aMember[i+1], sizeof(PartyMember) );
		}
		m_nSizeofMember--;
#ifdef __WORLDSERVER
#if __VER >= 14 // __INSTANCE_DUNGEON
		CInstanceDungeonParty::GetInstance()->SetPartyLeaveTime( uPlayerId );
#endif // __INSTANCE_DUNGEON
#endif // __WORLDSERVER
		return TRUE;
	}
	return FALSE;
}

void CParty::ChangeLeader( u_long uLeaderId )
{
	int changeIndex = FindMember( uLeaderId );
	SwapPartyMember( 0, changeIndex );
}

void CParty::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_uPartyId << m_nKindTroup << m_nSizeofMember;
		ar << m_nLevel << m_nExp << m_nPoint;
		ar << m_nTroupsShareExp << m_nTroupeShareItem;
		ar << m_idDuelParty;
		for( int i = 0 ; i < MAX_PARTYMODE  ; i++ )
		{
			ar << m_nModeTime[i];
		}
		if( m_nKindTroup )
			ar.WriteString( m_sParty );
		for( i = 0 ; i < m_nSizeofMember ; i++ )
		{
			ar << m_aMember[i].m_uPlayerId;
#if __VER < 11 // __SYS_PLAYER_DATA
			ar << m_aMember[i].m_nLevel;
			ar << m_aMember[i].m_nJob;
#endif	// __SYS_PLAYER_DATA
			ar << m_aMember[i].m_bRemove;
#if __VER < 11 // __SYS_PLAYER_DATA
			ar << m_aMember[i].m_nSex;
			ar.WriteString( m_aMember[i].m_szName );
#endif	// __SYS_PLAYER_DATA
		}
	}
	else
	{
		ar >> m_uPartyId >> m_nKindTroup >> m_nSizeofMember;
		ar >> m_nLevel >> m_nExp >> m_nPoint;
		ar >> m_nTroupsShareExp >> m_nTroupeShareItem;
		ar >> m_idDuelParty;
		for( int i = 0 ; i < MAX_PARTYMODE  ; i++ )
		{
			ar >> m_nModeTime[i];
		}
		if( m_nKindTroup )
			ar.ReadString( m_sParty, 33 );
		for( i = 0 ; i < m_nSizeofMember ; i++ )
		{
			ar >> m_aMember[i].m_uPlayerId;
#if __VER < 11 // __SYS_PLAYER_DATA
			ar >> m_aMember[i].m_nLevel;
			ar >> m_aMember[i].m_nJob;
#endif	// __SYS_PLAYER_DATA
			ar >> m_aMember[i].m_bRemove;
#if __VER < 11 // __SYS_PLAYER_DATA
			ar >> m_aMember[i].m_nSex;
			ar.ReadString( m_aMember[i].m_szName, 20 );
#endif	// __SYS_PLAYER_DATA
		}
	}
}

void CParty::SwapPartyMember( int first, int Second )
{
	PartyMember PartyMemberBuf;

	memcpy( &PartyMemberBuf, &m_aMember[first], sizeof(PartyMember) );
	memcpy( &m_aMember[first], &m_aMember[Second], sizeof(PartyMember) );
	memcpy( &m_aMember[Second], &PartyMemberBuf, sizeof(PartyMember) );
}

#if __VER >= 12 // __JHMA_VER12_1	//12차 극단유료아이템
int CParty::GetPartyModeTime( int nMode )
{
	return m_nModeTime[nMode];	
}
void CParty::SetPartyMode( int nMode, DWORD dwSkillTime ,int nCachMode )
{
	if( nCachMode == 1)
        m_nModeTime[nMode] += (int)dwSkillTime;	
	else
		m_nModeTime[nMode] = (int)dwSkillTime;	
}
#else	//12차 극단유료아이템
void CParty::SetPartyMode( int nMode, DWORD dwSkillTime )
{
	m_nModeTime[nMode] = (int)dwSkillTime;	
}
#endif // //12차 극단유료아이템

#ifdef __WORLDSERVER
void CParty::SetPartyLevel( CUser* pUser, DWORD dwLevel, DWORD dwPoint, DWORD dwExp )
{
	g_DPCoreClient.SendPartyLevel( pUser, dwLevel, dwPoint, dwExp );
}
#endif // __WORLDSERVER

void CParty::GetPoint( int nTotalLevel, int nMemberSize, int nDeadLeavel )
{
#ifdef __WORLDSERVER
#ifndef __PARTYDEBUG
	if( (nTotalLevel / nMemberSize) - nDeadLeavel < 5 )
#endif // __PARTYDEBUG
	{
		BOOL bExpResult = TRUE;
		BOOL bSuperLeader = FALSE;
#if __VER >= 12 // __JHMA_VER12_1	//12차 극단유료아이템
		BOOL bLeaderSMExpUp = FALSE;
#endif // //12차 극단유료아이템
		if( m_nKindTroup == 0 && m_nLevel >= MAX_PARTYLEVEL )
			bExpResult = FALSE;

		CMover* pMover = GetLeader();
		if( pMover && pMover->HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_SUPERLEADERPARTY ) )
		{
			bSuperLeader = TRUE;
		}
#if __VER >= 12 // __JHMA_VER12_1	//12차 극단유료아이템
		#define II_SYS_SYS_SCR_PARTYEXPUP01_01 20296
		#define II_SYS_SYS_SCR_PARTYSKILLUP01_01 20297
		if( pMover && ( pMover->HasBuff( BUFF_ITEM2, II_SYS_SYS_SCR_PARTYEXPUP01 ) || pMover->HasBuff( BUFF_ITEM2, II_SYS_SYS_SCR_PARTYEXPUP02 )
			 || pMover->HasBuff( BUFF_ITEM2, II_SYS_SYS_SCR_PARTYEXPUP01_01 ) ) )
		{
			bLeaderSMExpUp = TRUE;
		}
#endif // //12차 극단유료아이템

		if( bExpResult )
#if __VER >= 12 // __JHMA_VER12_1	//12차 극단유료아이템
			g_DPCoreClient.SendAddPartyExp( m_uPartyId, nDeadLeavel, bSuperLeader , bLeaderSMExpUp );
#else // //12차 극단유료아이템
			g_DPCoreClient.SendAddPartyExp( m_uPartyId, nDeadLeavel, bSuperLeader );
#endif // //12차 극단유료아이템
	}
#endif // __WORLDSERVER
}

void CParty::DoUsePartySkill( u_long uPartyId, u_long uLeaderid, int nSkill )
{
#ifdef __WORLDSERVER
	CUser* pMember	= NULL;
	int		i;
	
	if( IsLeader( uLeaderid ) && m_nKindTroup == 1 )
	{
		ItemProp* pItemProp =  prj.GetPartySkill( nSkill );
		if( pItemProp )
		{
#ifndef __PARTYDEBUG
			if( int( GetLevel() - pItemProp->dwReqDisLV ) >= 0 )
#endif // __PARTYDEBUG
			{
#ifndef __PARTYDEBUG
	#if __VER >= 12 // __JHMA_VER12_1	//12차 극단유료아이템
				CUser *pLeadertmp = g_UserMng.GetUserByPlayerID( m_aMember[0].m_uPlayerId );	// 리더의 포인터
				if( IsValidObj( pLeadertmp ) == FALSE )
					return;
				int nHasCashSkill = 0,nFPoint = 0;
				if( pLeadertmp->HasBuff( BUFF_ITEM2, II_SYS_SYS_SCR_PARTYSKILLUP01 ) 
					|| pLeadertmp->HasBuff( BUFF_ITEM2, II_SYS_SYS_SCR_PARTYSKILLUP02 )
					|| pLeadertmp->HasBuff( BUFF_ITEM2, II_SYS_SYS_SCR_PARTYSKILLUP01_01 ) )
				{
					if( nSkill == ST_LINKATTACK 
						|| nSkill == ST_FORTUNECIRCLE 
						|| nSkill == ST_STRETCHING
						|| nSkill == ST_GIFTBOX )
						nHasCashSkill = 1;
				}

				DWORD dwSkillTime	= pItemProp->dwSkillTime;
				int nRemovePoint	= pItemProp->dwExp;

				nFPoint	= int( GetPoint() - pItemProp->dwExp);
#if __VER >= 12 // __LORD
				// 군주의 극단
				// 군주가 극단장 으로써 극단스킬 사용 시,
				// 지속시간 4배 증가(소모 포인트는 동일)
				if( CSLord::Instance()->IsLord( uLeaderid ) )
					dwSkillTime		*= 4;
#endif	// __LORD

				if( nFPoint >= 0 )
	#else
				if( int( GetPoint() - pItemProp->dwExp) >= 0 )
	#endif // //12차 극단유료아이템
#endif // __PARTYDEBUG
				{
					switch( nSkill )
					{
					case ST_CALL:
						{
							g_DPCoreClient.SendRemovePartyPoint( uPartyId, pItemProp->dwExp );
							CUser *pLeader = g_UserMng.GetUserByPlayerID( m_aMember[0].m_uPlayerId );	// 리더의 포인터
							if( IsValidObj( pLeader ) == FALSE )
								break;

							// 단장 중심으로 모여달라는 표시
							for( i = 0; i < m_nSizeofMember; i ++ )		// 단장(0)에게는 보낼필요 없다.
							{
								pMember		= g_UserMng.GetUserByPlayerID( m_aMember[i].m_uPlayerId );
								// 단장 어라운드 지역에 있는 사람에게만 보낸다
								if( IsValidObj( (CObj*)pMember ) )
									pMember->AddPartySkillCall( pLeader->GetPos() );		// 각 멤버들에게 단장이 좌표를 전송함.
							}
						}
						break;
					case ST_BLITZ:
						{
							CUser *pLeader = g_UserMng.GetUserByPlayerID( m_aMember[0].m_uPlayerId );	// 리더의 포인터
							if( IsValidObj( pLeader ) == FALSE )
								break;

							if( pLeader->m_idSetTarget != NULL_ID )
							{
								CMover *pT = prj.GetMover( pLeader->m_idSetTarget );
								if( pT && !pT->IsPlayer() )
								{
									g_DPCoreClient.SendRemovePartyPoint( uPartyId, pItemProp->dwExp );
									// 단장이 타겟으로 집중공격 표시
									for( i = 0; i < m_nSizeofMember; i ++ )
									{
										pMember		= g_UserMng.GetUserByPlayerID( m_aMember[i].m_uPlayerId );
										if( IsValidObj( (CObj*)pMember ) )
										{
											if( m_nModeTime[PARTY_GIFTBOX_MODE] || m_nModeTime[PARTY_FORTUNECIRCLE_MODE] )
											{
												pMember->AddPartySkillBlitz( pLeader->m_idSetTarget );		// 각 멤버들에게 단장타겟으로 잡은 무버의 아이디를 보냄.
											}
											else
											{
												if( pLeader->IsNearPC( pMember ) )
													pMember->AddPartySkillBlitz( pLeader->m_idSetTarget );		// 각 멤버들에게 단장타겟으로 잡은 무버의 아이디를 보냄.
											}
										}
									}
								}
								else
								{
									pLeader->AddSendErrorParty( ERROR_NOTTARGET, ST_BLITZ );
								}
							}
							else
							{
								pLeader->AddSendErrorParty( ERROR_NOTTARGET, ST_BLITZ );
								// 타겟을 안잡았다.
							}
						}
						break;
					case ST_RETREAT:
						{
							g_DPCoreClient.SendRemovePartyPoint( uPartyId, pItemProp->dwExp );
							// 후퇴 표시
							CUser *pLeader = g_UserMng.GetUserByPlayerID( m_aMember[0].m_uPlayerId );	// 리더의 포인터
							if( IsValidObj( pLeader ) == FALSE )
								break;

							for( i = 0; i < m_nSizeofMember; i ++ )
							{
								pMember		= g_UserMng.GetUserByPlayerID( m_aMember[i].m_uPlayerId );
								if( IsValidObj( (CObj*)pMember ) )
								{
									if( m_nModeTime[PARTY_GIFTBOX_MODE] || m_nModeTime[PARTY_FORTUNECIRCLE_MODE] )
									{
										pMember->AddHdr( pMember->GetId(), SNAPSHOTTYPE_PARTYSKILL_RETREAT );
									}
									else
									{
										if( pLeader->IsNearPC( pMember ) )
											pMember->AddHdr( pMember->GetId(), SNAPSHOTTYPE_PARTYSKILL_RETREAT );
									}
								}
							}
						}
						break;
					case ST_SPHERECIRCLE:
						{
							// 크리티컬 확률
							CUser *pLeader = g_UserMng.GetUserByPlayerID( m_aMember[0].m_uPlayerId );	// 리더의 포인터
							if( IsValidObj( pLeader ) == FALSE )
								break;
							
							if( pLeader->m_idSetTarget != NULL_ID )
							{
								CMover * pT = prj.GetMover( pLeader->m_idSetTarget );
								if( pT && !pT->IsPlayer() )
								{
									g_DPCoreClient.SendRemovePartyPoint( uPartyId, pItemProp->dwExp );
									for( i = 0; i < m_nSizeofMember; i ++ )
									{
										pMember		= g_UserMng.GetUserByPlayerID( m_aMember[i].m_uPlayerId );
										if( IsValidObj( (CObj*)pMember ) )
										{
											if( m_nModeTime[PARTY_GIFTBOX_MODE] || m_nModeTime[PARTY_FORTUNECIRCLE_MODE] )
											{
												pMember->AddHdr( pLeader->m_idSetTarget, SNAPSHOTTYPE_PARTYSKILL_SPHERECIRCLE );
												pMember->m_dwFlag |= MVRF_CRITICAL;
											}
											else
											{
												if( pLeader->IsNearPC( pMember ) )
												{
													pMember->AddHdr( pLeader->m_idSetTarget, SNAPSHOTTYPE_PARTYSKILL_SPHERECIRCLE );
													pMember->m_dwFlag |= MVRF_CRITICAL;
												}
											}
										}
									}
								}
								else
								{
									pLeader->AddSendErrorParty( ERROR_NOTTARGET, ST_SPHERECIRCLE );
									// 타겟이 몬스터가 아니다
								}
							}
							else
							{
								pLeader->AddSendErrorParty( ERROR_NOTTARGET, ST_SPHERECIRCLE );
								// 타겟을 안잡았다.
							}
//							g_DPCoreClient.SendSetPartyExp( uLeaderid, m_nPoint );
						}
						break;

#if __VER >= 12 // __JHMA_VER12_1	//12차 극단유료아이템
					case ST_LINKATTACK:
						{
							// 데미지 증가
							g_DPCoreClient.SendUserPartySkill( uLeaderid, PARTY_LINKATTACK_MODE, dwSkillTime, nRemovePoint, nHasCashSkill );
						}
						break;
					case ST_FORTUNECIRCLE:
						{
							// 유니크 아이템 발생확률 증가
								g_DPCoreClient.SendUserPartySkill( uLeaderid, PARTY_FORTUNECIRCLE_MODE, dwSkillTime, nRemovePoint, nHasCashSkill );
						}
						break;
					case ST_STRETCHING:
						{
							// 쉬는경우 회복속도 높여줌
							g_DPCoreClient.SendUserPartySkill( uLeaderid, PARTY_STRETCHING_MODE, dwSkillTime, nRemovePoint, nHasCashSkill );
						}
						break;
					case ST_GIFTBOX:
						{
							// 아이템 양이 두배
							g_DPCoreClient.SendUserPartySkill( uLeaderid, PARTY_GIFTBOX_MODE, dwSkillTime, nRemovePoint, nHasCashSkill );
						}
						break;
#else	//12차 극단유료아이템
					case ST_LINKATTACK:
						{
							// 데미지 증가
							g_DPCoreClient.SendUserPartySkill( uLeaderid, PARTY_LINKATTACK_MODE, dwSkillTime, nRemovePoint );
						}
						break;
					case ST_FORTUNECIRCLE:
						{
							// 유니크 아이템 발생확률 증가
								g_DPCoreClient.SendUserPartySkill( uLeaderid, PARTY_FORTUNECIRCLE_MODE, dwSkillTime, nRemovePoint );
						}
						break;
					case ST_STRETCHING:
						{
							// 쉬는경우 회복속도 높여줌
							g_DPCoreClient.SendUserPartySkill( uLeaderid, PARTY_STRETCHING_MODE, dwSkillTime, nRemovePoint );
						}
						break;
					case ST_GIFTBOX:
						{
							// 아이템 양이 두배
							g_DPCoreClient.SendUserPartySkill( uLeaderid, PARTY_GIFTBOX_MODE, dwSkillTime, nRemovePoint );
						}
						break;
#endif // //12차 극단유료아이템
					default:
						break;
					}
				}
#ifndef __PARTYDEBUG
				else
				{
					CUser *pLeader = g_UserMng.GetUserByPlayerID( m_aMember[0].m_uPlayerId );	// 리더의 포인터
					if( IsValidObj( pLeader ) )
					{
						pLeader->AddSendErrorParty( ERROR_NOTPARTYPOINT );
					}
					//포인트가 모자라 스킬을사용할수 없습니다.
					
				}
#endif // __PARTYDEBUG
			}
#ifndef __PARTYDEBUG
			else
			{
				CUser *pLeader = g_UserMng.GetUserByPlayerID( m_aMember[0].m_uPlayerId );	// 리더의 포인터
				if( IsValidObj( pLeader ) )
				{
					pLeader->AddSendErrorParty( ERROR_NOTPARTYSKILL );
				}
				//이 스킬은 배우지를 못했습니다.
			}
#endif // __PARTYDEBUG
		}
	}
	else
	{
		// 리더가 아니거나 순회극단이 아닌경우
	}
#endif	// __WORLDSERVER
}

//
//
// idDstParty가 this파티가 결투를 시작했다는걸 모든 멤버들에게 알림.
#ifdef __WORLDSERVER
void CParty::DoDuelPartyStart( CParty *pDst )
{
	int		i, j;
	OBJID	pDstMember[ MAX_PTMEMBER_SIZE ];
	memset( pDstMember, 0xff, sizeof(pDstMember) );
	
	m_idDuelParty = pDst->m_uPartyId;		// this파티의 상대
	
	CMover *pMember, *pMember2;

	LPCSTR pszLeader;
	if( pDst->m_nKindTroup )	// 순회극단 극단 이름 보냄
	{
		pszLeader = pDst->m_sParty;
	}
	else	// 단막극단 일때는 리더이름을 보냄
	{
#if __VER >= 11 // __SYS_PLAYER_DATA
		pszLeader	= CPlayerDataCenter::GetInstance()->GetPlayerString( pDst->GetPlayerId( 0 ) );
#else	// __SYS_PLAYER_DATA
		pszLeader	= prj.GetPlayerString( pDst->GetPlayerId( 0 ) );
#endif	//__SYS_PLAYER_DATA
	}
	
	if( !pszLeader )
	{
		Error( "Error Party PVP 1" );
		return;
	}

	for( i = 0; i < m_nSizeofMember; i ++ )		// 극단원들 루프.
	{
		pMember	= (CMover *)g_UserMng.GetUserByPlayerID( m_aMember[i].m_uPlayerId );
		if( IsValidObj( pMember ) )
		{
			pMember->m_nDuel = 2;		// 2는 파티듀얼중.
			pMember->m_idDuelParty = m_idDuelParty;
			pMember->m_nDuelState = 104;
			// 상대방 파티의 멤버아이디를 다 꺼냄.
			for( j = 0; j < pDst->m_nSizeofMember; j ++ )
			{
				pMember2 = (CMover *)g_UserMng.GetUserByPlayerID( pDst->m_aMember[j].m_uPlayerId );
				if( IsValidObj(pMember2) )
					pDstMember[j] = pMember2->GetId();
			}
			// 상대 파티에 대한 정보를 우리멤버들에게 보냄
			((CUser *)pMember)->AddDuelPartyStart( pszLeader , pDst->m_nSizeofMember, pDstMember, pDst->m_uPartyId );		// 상대방 멤버의 ID를 다보낸다.
		}
	}
}	

//
// 파티원들에게 bWin에 따라 승/패 사실을 알리고 듀얼을 종료시킴.
//
void CParty::DoDuelResult( CParty *pDuelOther, BOOL bWin, int nAddFame, float fSubFameRatio )
{
	CUser *pMember;
	int		i;
	
	for( i = 0; i < m_nSizeofMember; i ++ )
	{
		pMember = g_UserMng.GetUserByPlayerID( m_aMember[i].m_uPlayerId );
		if( IsValidObj( pMember ) )
		{
			pMember->AddDuelPartyResult( pDuelOther, bWin );		// 각 멤버들에게 승/패 사실을 알림. / 상대파티원 리스트도 보냄.
			pMember->ClearDuelParty();

			if( bWin )
			{
				pMember->m_nFame += nAddFame;	// 이긴측이면 명성 증가
				pMember->AddDefinedText( TID_GAME_GETFAMEPOINT, "%d", nAddFame );	// xxx 명성 포인트가 증가
#ifdef __WORLDSERVER
				g_dpDBClient.SendLogPkPvp( pMember, NULL, nAddFame, 'E' );
#endif // __WORLDSERVER
			} 
			else
			{
				int nDecVal;
				fSubFameRatio = fSubFameRatio / 100.0f;	// 비율로 환산.
				nDecVal = (int)(pMember->m_nFame * fSubFameRatio);
				if( nDecVal == 0 )	nDecVal = 1;
				pMember->m_nFame -= nDecVal;
				if( pMember->m_nFame < 0 )	pMember->m_nFame = 0;
				pMember->AddDefinedText( TID_GAME_DECFAMEPOINT, "%d", nDecVal );	// xxx 명성 포인트가 감소.
#ifdef __WORLDSERVER
				g_dpDBClient.SendLogPkPvp( NULL, pMember, nAddFame, 'E' );
#endif // __WORLDSERVER
			}
			
			// 바뀐 명성치를 pMember의 주위에 날려줌.
			g_UserMng.AddSetFame( pMember, pMember->m_nFame );
		}
	}
	m_idDuelParty = 0;
}


void CParty::DoUsePartyReCall( u_long uPartyId, u_long uLeaderid, int nSkill )
{
	CUser* pMember	= NULL;
	int		i;

	CUser* pUser    = NULL;
	pUser = g_UserMng.GetUserByPlayerID( uLeaderid );
	if( !IsValidObj( (CObj*)pUser ) )
		return;
	
	for( i = 0; i < m_nSizeofMember; i ++ )		// 단장(0)에게는 보낼필요 없다.
	{
		pMember		= g_UserMng.GetUserByPlayerID( m_aMember[i].m_uPlayerId );
		if( IsValidObj( (CObj*)pMember ) )
		{
			if(pMember->m_idPlayer != pUser->m_idPlayer)
#ifdef __LAYER_1015
				g_DPCoreClient.SendSummonPlayer( pUser->m_idPlayer, pUser->GetWorld()->GetID(), pUser->GetPos(), pMember->m_idPlayer, pUser->GetLayer() );
#else	// __LAYER_1015
				g_DPCoreClient.SendSummonPlayer( pUser->m_idPlayer, pUser->GetWorld()->GetID(), pUser->GetPos(), pMember->m_idPlayer );
#endif	// __LAYER_1015
		}
	}
}

#endif // worldserver

//
// 극단 듀얼 해제
// 
void CParty::DoDuelPartyCancel( CParty* pDuelParty )
{
#ifndef __CORESERVER
	int		i;
	
	CMover *pMember;
	for( i = 0; i < m_nSizeofMember; i ++ )		// 극단원 모두에게 듀얼 해제를 세팅하고 클라에도 알림.
	{
#ifdef __WORLDSERVER
		pMember	= (CMover *)g_UserMng.GetUserByPlayerID( m_aMember[i].m_uPlayerId );
#else
#ifdef __CLIENT
		pMember	= prj.GetUserByID( m_aMember[i].m_uPlayerId );
#endif
#endif // worldserver
		if( IsValidObj( pMember ) )
		{
#ifdef __WORLDSERVER
			if( pDuelParty )
				((CUser *)pMember)->AddDuelPartyCancel( pDuelParty );		// 각 멤버들에게 듀얼이 취소되었다고 알림.
			else
				((CUser *)pMember)->AddDuelPartyCancel( NULL );		// 각 멤버들에게 듀얼이 취소되었다고 알림.
			if( pMember->m_idDuelParty != m_idDuelParty )
				Error( "CParty::DoDuelPartyCancel : 파티멤버 %s의 정보이상. %d %d", pMember->GetName(), pMember->m_idDuelParty, m_idDuelParty );
			pMember->ClearDuelParty();
#endif // worldserver
		}
	}
	m_idDuelParty = 0;		// 파티 해제
#endif	// __CORESERVER
}	

#ifdef __WORLDSERVER
void CParty::ReplaceLodestar( const CRect &rect )
{
	// locked
	CUser* pUser;
	for( int i = 0; i < m_nSizeofMember; i ++ )		// 극단원 모두에게 듀얼 해제를 세팅하고 클라에도 알림.
	{
		pUser	= g_UserMng.GetUserByPlayerID( m_aMember[i].m_uPlayerId );

		if( IsValidObj( pUser ) )
		{
			CWorld* pWorld	= pUser->GetWorld();
			if( pWorld )
			{
				POINT point	= { (int)pUser->GetPos().x, (int)pUser->GetPos().z	};
				if( rect.PtInRect( point ) )
				{
					PRegionElem pRgnElem	= NULL;
					if( pWorld->GetID() != pWorld->m_dwIdWorldRevival && pWorld->m_dwIdWorldRevival != 0 )
						pRgnElem	= g_WorldMng.GetRevivalPos( pWorld->m_dwIdWorldRevival, pWorld->m_szKeyRevival );
					if( !pRgnElem )	// Find near revival pos
						pRgnElem	= g_WorldMng.GetNearRevivalPos( pWorld->GetID(), pUser->GetPos() );
					if( pRgnElem )
						pUser->REPLACE( g_uIdofMulti, pRgnElem->m_dwWorldId, pRgnElem->m_vPos, REPLACE_NORMAL, nRevivalLayer );
				}
			}
		}
	}
}

void CParty::Replace( DWORD dwWorldId, D3DXVECTOR3 & vPos, BOOL bMasterAround )
{
	CUser *pMember;
	for( int i = 0; i < m_nSizeofMember; i ++ )		// 극단원 모두에게 듀얼 해제를 세팅하고 클라에도 알림.
	{
		pMember	= g_UserMng.GetUserByPlayerID( m_aMember[i].m_uPlayerId );

		if( IsValidObj( pMember ) )
		{
			/*
			if( GetQuest( QUEST_BOSS_LV1 ) != NULL )
				pMember->UnequipRide();
			
			if( GetQuest( QUEST_BOSS_LV2 ) != NULL )
				pMember->UnequipRide();
			
			if( GetQuest( QUEST_BOSS_LV3 ) != NULL )
				pMember->UnequipRide();
			*/			
			pMember->REPLACE( g_uIdofMulti, dwWorldId, vPos, REPLACE_NORMAL, nTempLayer );
#ifdef __BUFF_1107
			pMember->m_buffs.RemoveBuffs( RBF_COMMON, 0 );
#else	// __BUFF_1107
			pMember->m_SkillState.RemoveAllSkillInfluence();
#endif	// __BUFF_1107
		}
	}
}

void CParty::Replace( DWORD dwWorldId, LPCTSTR sKey )
{
	CUser *pMember;
	for( int i = 0; i < m_nSizeofMember; i ++ )		// 극단원 모두에게 듀얼 해제를 세팅하고 클라에도 알림.
	{
		pMember	= g_UserMng.GetUserByPlayerID( m_aMember[i].m_uPlayerId );
		
		if( IsValidObj( pMember ) )
		{
			PRegionElem pRgnElem = g_WorldMng.GetRevivalPos( dwWorldId, sKey );
			if( NULL != pRgnElem )
				pMember->REPLACE( g_uIdofMulti, pRgnElem->m_dwWorldId, pRgnElem->m_vPos, REPLACE_NORMAL, nRevivalLayer );
		}
	}
}

BOOL CParty::ReplaceChkLv( int Lv )
{
	CUser *pMember;
	for( int i = 0; i < m_nSizeofMember; i ++ )		// 극단원 모두에게 듀얼 해제를 세팅하고 클라에도 알림.
	{
		pMember	= g_UserMng.GetUserByPlayerID( m_aMember[i].m_uPlayerId );
		
		if( IsValidObj( pMember ) )
		{
			if( pMember->GetLevel() > Lv )
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}
#endif //__WORLDSERVER



CParty g_Party;
/*--------------------------------------------------------------------------------*/
#ifndef __CLIENT

CPartyMng::CPartyMng()
{
	m_id	= 0;
#ifdef __WORLDSERVER
	m_nSecCount = 0;
#endif // __WORLDSERVER
//	m_2Party.SetSize( 1024, 4096, 1024 );
#ifdef __CORESERVER
	m_hWorker	= m_hCloseWorker	= NULL;
#endif // __CORESERVER
}

CPartyMng::~CPartyMng()
{
//	CPartyBucket* pBucket	= m_2Party.GetFirstActive();
//	while( pBucket )
//	{
//		delete pBucket->m_value;
//		pBucket		= pBucket->pNext;
//	}
//	m_2Party.RemoveAll();
	Clear();
}

void CPartyMng::Clear( void )
{
#ifndef __WORLDSERVER
	m_AddRemoveLock.Enter( theLineFile );	// lock1
#endif	// __WORLDSERVER

	for( C2PartyPtr::iterator i	= m_2PartyPtr.begin(); i != m_2PartyPtr.end(); ++i )
		safe_delete( i->second );

	m_2PartyPtr.clear();
	
#ifdef __CORESERVER
	CloseWorkers();
#endif // __CORESERVER

#ifndef __WORLDSERVER
	m_AddRemoveLock.Leave( theLineFile );	// unlock1
#endif	// __WORLDSERVER
}

// 극단 생성
// uLeaderPlayerId : 단장, uPartyId : 1번째 단원
// 극단를 생설할때는 2명으로 생성함( 혼자서는 극단를 생성할수 없음 )
u_long	CPartyMng::NewParty( u_long uLeaderId, LONG nLeaderLevel, LONG nLeaderJob, BYTE nLeaderSex, LPSTR szLeaderName, u_long uMemberId, LONG nMemberLevel, LONG nMemberJob, BYTE nMemberSex, LPSTR szMembername, u_long uPartyId )
{
//	locked
	if( 0 == uPartyId )
	{
		m_id++;
	}
	else
	{
		m_id = uPartyId;
	}

	
	if( NULL == GetParty( m_id ) ) // NULL 이면 극단이 없으므로 만들어야 한다
	{
		CParty* pParty	= new CParty;
		
//		pParty->Lock();
		pParty->SetPartyId( m_id );
#if __VER >= 11 // __SYS_PLAYER_DATA
		if( TRUE == pParty->NewMember( uLeaderId ) && TRUE == pParty->NewMember( uMemberId ) )
#else	// __SYS_PLAYER_DATA
		if( TRUE == pParty->NewMember( uLeaderId, nLeaderLevel, nLeaderJob, nLeaderSex, szLeaderName ) && TRUE == pParty->NewMember( uMemberId, nMemberLevel, nMemberJob, nMemberSex, szMembername ) )
#endif	// __SYS_PLAYER_DATA
		{
//			m_2Party.SetAt( m_id, pParty );
			m_2PartyPtr.insert( C2PartyPtr::value_type( m_id, pParty ) );
			pParty->m_nGetItemPlayerId = pParty->m_aMember[0].m_uPlayerId;
//			pParty->Unlock();
			return m_id;
		}
		else // 극단에 소속되어 있다
		{
//			pParty->Unlock();
			safe_delete( pParty );
		}
	}
	return 0;
}

// 극단 해체
// 해체는 사람이 부족하여 자동으로 해체
// 단독적으로 해체할수 없음
BOOL CPartyMng::DeleteParty( u_long uPartyId )
{
	CParty* pParty = GetParty( uPartyId );
	if( pParty )
	{
		if( pParty->m_idDuelParty )		// 극단듀얼중일때 극단이 해체되었으면
		{
			CParty *pDstParty = GetParty( pParty->m_idDuelParty );		// 상대 파티원들에게도 this파티가 해체되어 듀얼이해제됐다는걸 알림.
			if( pDstParty )
			{
				pDstParty->DoDuelPartyCancel( pParty );
			} 
			else
			{
				Error( "CPartyMng::DeleteParty : 상대파티를 찾을 수 없음 %d", pParty->m_idDuelParty );
			}

	#ifdef __WORLDSERVER
			g_DPCoreClient.SendSetPartyDuel( pParty->m_uPartyId, pParty->m_idDuelParty, FALSE );
	#endif // __WORLDSERVER
			pParty->DoDuelPartyCancel( pDstParty );	// this파티원들에게도 듀얼해제됐다는걸 알림.
		}
#ifdef __WORLDSERVER
#if __VER >= 14 // __INSTANCE_DUNGEON
		CInstanceDungeonParty::GetInstance()->DestroyAllDungeonByDungeonID( uPartyId );
#endif // __INSTANCE_DUNGEON

#ifdef	__COLOSSEUM
		CColosseumMng::GetInstance()->DestroyMatch( uPartyId );
#endif // __COLOSSEUM
#endif // __WORLDSERVER

		safe_delete( pParty );
		m_2PartyPtr.erase( uPartyId );
		return TRUE;
	}
	return FALSE;
}

CParty* CPartyMng::GetParty( u_long uPartyId )
{
//	CParty* pParty;
//	if( m_2Party.Lookup( uPartyId, pParty ) )
//	{
//		return pParty;
//	}
	C2PartyPtr::iterator i		= m_2PartyPtr.find( uPartyId );
	if( i != m_2PartyPtr.end() )
		return i->second;
	return NULL;
}

void CPartyMng::Serialize( CAr & ar )
{
#ifndef __WORLDSERVER
	m_AddRemoveLock.Enter( theLineFile );	// lock1
#endif	// __WORLDSERVER

	if( ar.IsStoring() )
	{
		ar << m_id;
		int nCount	= 0;
		u_long uOffset	= ar.GetOffset();
		ar << nCount;
		for( C2PartyPtr::iterator i	= m_2PartyPtr.begin(); i != m_2PartyPtr.end(); ++i )
		{
			CParty* pParty = (CParty*)i->second;
			pParty->Serialize( ar );
			nCount++;
		}
		int nBufSize1;
		LPBYTE lpBuf1	= ar.GetBuffer( &nBufSize1 );
		*(UNALIGNED int*)( lpBuf1 + uOffset )	= nCount;
	}
	else
	{
		int nCount;
		ar >> m_id;
		ar >> nCount;		
		for( int i = 0 ; i < nCount ; i++ )
		{
			CParty* pParty	= new CParty;
//			pParty->Lock();
			pParty->Serialize( ar );
			m_2PartyPtr.insert( C2PartyPtr::value_type( pParty->m_uPartyId, pParty ) );
//			pParty->Unlock();
		}
	}
#ifndef __WORLDSERVER
	m_AddRemoveLock.Leave( theLineFile );	// unlock1
#endif	// __WORLDSERVER
}

#ifdef __CORESERVER
BOOL CPartyMng::IsPartyNameId( u_long uidPlayer )
{
	if( m_2PartyNameLongPtr.end() != m_2PartyNameLongPtr.find( uidPlayer ) )
	{
		return TRUE;
	}
	return FALSE;
}
BOOL CPartyMng::IsPartyName( const char* szPartyName )
{
	if( m_2PartyNameStringPtr.end() != m_2PartyNameStringPtr.find( szPartyName ) )
	{
		return TRUE;
	}
	return FALSE;
}

LPCSTR CPartyMng::GetPartyString( u_long uidPlayer )
{
	ULONG2STRING::iterator i	= m_2PartyNameLongPtr.find( uidPlayer );
	if( i != m_2PartyNameLongPtr.end() )
		return( i->second.data() );
	return NULL;
}

u_long CPartyMng::GetPartyID( const char* szPartyName )
{
	STRING2ULONG::iterator i	= m_2PartyNameStringPtr.find( szPartyName );
	if( i != m_2PartyNameStringPtr.end() )
		return( i->second );
	return 0;
}



void CPartyMng::AddPartyName( u_long uidPlayer, const char* szPartyName )
{
	m_2PartyNameLongPtr.insert(  map<u_long, string>::value_type( uidPlayer, szPartyName ) );
	m_2PartyNameStringPtr.insert(  map<string, u_long>::value_type(szPartyName, uidPlayer ) );
}

void CPartyMng::RemovePartyName( u_long uidPlayer, const char* szPartyName )
{
	ULONG2STRING::iterator iter = m_2PartyNameLongPtr.find( uidPlayer );
	STRING2ULONG::iterator iter1 = m_2PartyNameStringPtr.find( szPartyName );

	if( iter != m_2PartyNameLongPtr.end() )
		m_2PartyNameLongPtr.erase( iter );

	if( iter1 != m_2PartyNameStringPtr.end() )
		m_2PartyNameStringPtr.erase( iter1 );
//	m_2PartyNameLongPtr.insert(  map<u_long, string>::value_type( uidPlayer, szPartyName ) );
//	m_2PartyNameStringPtr.insert(  map<string, u_long>::value_type(szPartyName, uidPlayer ) );
}

BOOL CPartyMng::CreateWorkers( void )
{
	DWORD dwThreadId;
	m_hCloseWorker	= CreateEvent( NULL, FALSE, FALSE, NULL );
	m_hWorker	= chBEGINTHREADEX( NULL, 0, _Worker, this, 0, &dwThreadId );
	if( m_hWorker == NULL )
		return FALSE;
	return TRUE;
}

void CPartyMng::CloseWorkers( void )
{
	CLOSE_THREAD( m_hWorker, m_hCloseWorker );
}

UINT CPartyMng::_Worker( LPVOID pParam )
{
	CPartyMng* pPartyMng	= (CPartyMng*)pParam;
	pPartyMng->Worker();
	return 0;
}

void CPartyMng::Worker( void )
{
	CPlayer* pMember;

	HANDLE hHandle	= m_hCloseWorker;
	while( WaitForSingleObject( hHandle, 1000 ) == WAIT_TIMEOUT )
	{
		CTime timeCurr	= CTime::GetCurrentTime();

		CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
		CMclAutoLock	Lock2( m_AddRemoveLock );

		for( C2PartyPtr::iterator i	= m_2PartyPtr.begin(); i != m_2PartyPtr.end(); )
		{
			CParty* pParty = (CParty*)i->second;
			++i;
			if( pParty->m_nReferens > 0 )
			{
				for( int j = 1; j < pParty->m_nSizeofMember; j++ )
				{
					if( pParty->m_aMember[j].m_uPlayerId != 0 && pParty->m_aMember[j].m_bRemove )
					{
						if( timeCurr.GetTime() - pParty->m_aMember[j].m_tTime.GetTime() > 60 * 10 )	// 10분 : 60 * 10 // 지금은 1분
						{
							u_long idMember		= pParty->GetPlayerId( j );
							if( pParty->DeleteMember( idMember ) )
							{
								pParty->m_nReferens--;
								
								g_dpCoreSrvr.SendRemoveParty( pParty->m_uPartyId, pParty->m_aMember[0].m_uPlayerId, idMember );
								
								if( pParty->GetSizeofMember() < 2 )
								{
									pMember		= g_PlayerMng.GetPlayer( pParty->GetPlayerId( 0 ) );
									if( pMember )
										pMember->m_uPartyId	= 0;
									DeleteParty( pParty->m_uPartyId );
									break;
								}

								if( pParty->m_nReferens == 0 )
									break;
							}
						}
					}
				}	// for
			}	// if

			// 파티모드를 체크
			for( int j = 0 ; j < MAX_PARTYMODE ; j++ )
			{
				if( pParty->m_nModeTime[j] )
				{
#if __VER >= 12 // __PARSKILL1001	//12차 파스킬 아이템 수정  world,core,neuz
					if( j == PARTY_PARSKILL_MODE)
						continue;
#endif //__PARSKILL1001	//12차 파스킬 아이템 수정  world,core,neuz
					pParty->m_nModeTime[j] -= 1000;
					if( pParty->m_nModeTime[j] <= 0 )
					{
						pParty->m_nModeTime[j] = 0;
						g_dpCoreSrvr.SendSetPartyMode( pParty->m_uPartyId, j, FALSE );
					}
				}
			}
		}	// for
//		TRACE( "Worker Party\n" );
	}
}

void CPartyMng::AddConnection( CPlayer* pPlayer )
{
	CParty* pParty;
	CMclAutoLock	Lock( m_AddRemoveLock );
	
	pParty	= GetParty( pPlayer->m_uPartyId );
	if( pParty )
	{
		int i	= pParty->FindMember( pPlayer->uKey );
		if( i < 0 )
		{
			pPlayer->m_uPartyId		= 0;
			return;
		}

		pParty->m_aMember[i].m_bRemove	= FALSE;
		pParty->m_nReferens--;

		BEFORESENDDUAL( ar, PACKETTYPE_ADDPLAYERPARTY, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << pPlayer->m_uPartyId << pPlayer->uKey;
		SEND( ar, &g_dpCoreSrvr, DPID_ALLPLAYERS );
	}
	else
		pPlayer->m_uPartyId		= 0;
}

void CPartyMng::RemoveConnection( CPlayer* pPlayer )
{
	if( pPlayer->m_uPartyId == 0 )
		return;

	CParty* pParty;
	CMclAutoLock	Lock( m_AddRemoveLock );
	
	pParty	= GetParty( pPlayer->m_uPartyId );
	if( pParty )
	{
		int i	= pParty->FindMember( pPlayer->uKey );
		if( i < 0 )
			return;
		pParty->m_aMember[i].m_tTime	= CTime::GetCurrentTime();
		pParty->m_aMember[i].m_bRemove	= TRUE;
		pParty->m_nReferens++;

		BEFORESENDDUAL( ar, PACKETTYPE_REMOVEPLAYERPARTY, DPID_UNKNOWN, DPID_UNKNOWN );
		ar << pPlayer->m_uPartyId << pPlayer->uKey;
		SEND( ar, &g_dpCoreSrvr, DPID_ALLPLAYERS );

#if __VER >= 12 // __PARSKILL1001 090917 mirchang - 파스킬 아이템 수정
		if( pParty->m_nModeTime[PARTY_PARSKILL_MODE] )
			g_dpCoreSrvr.SendSetPartyMode( pParty->m_uPartyId, PARTY_PARSKILL_MODE, FALSE );
#endif // __PARSKILL1001 090917 mirchang - 파스킬 아이템 수정

		if( i == 0 )
		{
			bool fRemoveParty	= true;
			for( int j = 1; j < pParty->m_nSizeofMember; j++ )
			{
				if( pParty->m_aMember[j].m_bRemove == FALSE )
				{
					fRemoveParty	= false;
					pParty->SwapPartyMember( 0, j );
					break;
				}
			}
#if __VER >= 12 // __PARSKILL1001	//12차 파스킬 아이템 수정  world,core,neuz
			for( int k = 0 ; k < MAX_PARTYMODE ; k++ )
			{
				if( pParty->m_nModeTime[k] )
				{
					if( k == PARTY_PARSKILL_MODE)
						continue;
					pParty->m_nModeTime[k] = 0;
				}
			}
#endif //__PARSKILL1001	//12차 파스킬 아이템 수정  world,core,neuz

			if( fRemoveParty )
			{
				CPlayer* pPlayer;
				for( j = 0; j < pParty->m_nSizeofMember; j++ )
				{
					pPlayer		= g_PlayerMng.GetPlayer( pParty->GetPlayerId( j ) );
					if( pPlayer )
						pPlayer->m_uPartyId		= 0;
				}
				DeleteParty( pParty->m_uPartyId );
			}
		}
	}
}
#endif // __CORESERVER

#ifdef __WORLDSERVER

void CPartyMng::PartyMapInfo( )
{
	const float PARTY_MAP_AROUND = 32.0f * 32.0f;	// m_nVisibilityRange에 영향을 받는다.

	if( ++m_nSecCount < PARTY_MAP_SEC )
		return;

	m_nSecCount = 0;

	D3DXVECTOR3 vPosBuf;
	float fDist;
	for( C2PartyPtr::iterator i	= m_2PartyPtr.begin(); i != m_2PartyPtr.end(); ++i )
	{
		CParty* pParty = (CParty*)i->second;
		for( int j = 0 ; j < pParty->GetSizeofMember() ; ++j )
		{
			CMover* pMover = prj.GetUserByID( pParty->GetPlayerId( j ) );
			if( !IsValidObj( pMover ) )
				continue;
			
			vPosBuf = pMover->GetPos() - pParty->GetPos( j );
			fDist = D3DXVec3LengthSq( &vPosBuf );
			if( 0.0f < fDist )
			{
				pParty->SetPos( j, pMover->GetPos() );
				
				CMover* pSendMover;
				for( int k = 0 ; k < pParty->GetSizeofMember() ; ++k )
				{
					if( k == j )
						continue;
					pSendMover = prj.GetUserByID( pParty->GetPlayerId( k ) );
					if( !IsValidObj( pSendMover ) )
						continue;

					vPosBuf = pSendMover->GetPos() - pMover->GetPos();
					fDist = D3DXVec3LengthSq( &vPosBuf );
					if( fDist > PARTY_MAP_AROUND )
						((CUser*)pSendMover)->AddPartyMapInfo( j, pMover->GetPos() );		
				}
			}
		}
	}
}

#endif // __WORLDSERVER

CPartyMng	g_PartyMng;
#endif // not client