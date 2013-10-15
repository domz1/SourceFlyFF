#include "stdafx.h"
#include "defineText.h"
#include "User.h"
#include "DPDatabaseClient.h"
#include "dpcoreclient.h"
#include "DPSrvr.h"
#include "WorldMng.h"
#include "..\_Common\Ship.h"
#include "defineobj.h"
#ifdef __WORLDSERVER
#include "..\_aiinterface\AIPet.h"
#ifdef __SYS_TICKET
#include "ticket.h"
#endif	// __SYS_TICKET
#endif

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

#if __VER >= 11 // __SYS_COLLECTING
#include "collecting.h"
#include "definesound.h"
#include "defineitem.h"
#endif	// __SYS_COLLECTING

#if __VER >= 19
#include "defineItemGrade.h"
#include "defineItemType.h"
#endif//__VER >= 19

#if __VER >= 12 // __SECRET_ROOM
#include "SecretRoom.h"
#endif // __SECRET_ROOM

#if __VER >= 12 // __TAX
#include "Tax.h"
#endif // __TAX

#if __VER >= 12 // __LORD
#include "slord.h"
#include "lordskillexecutable.h"
#endif	// __LORD

#if __VER >= 15 // __PETVIS
#include "ItemUpgrade.h"
#endif // __PETVIS

extern	CDPDatabaseClient	g_dpDBClient;
extern	CDPCoreClient	g_DPCoreClient;

#include "Environment.h"

#ifndef __ENVIRONMENT_EFFECT
extern	CEnvironment	g_Environment;
#endif // __ENVIRONMENT_EFFECT

#include "guild.h"
extern	CGuildMng	g_GuildMng;
#include "guildwar.h"
extern	CGuildWarMng	g_GuildWarMng;

#include "Chatting.h"
extern	CChattingMng	g_ChattingMng;

#include "Party.h"
extern	CPartyMng			g_PartyMng;
extern CGuildCombat g_GuildCombatMng;

#ifndef __MEM_TRACE
#ifdef _DEBUG
#define new new( __FILE__, __LINE__ )
#endif
#endif	// __MEM_TRACE

#include "UserMacro.h"
#include "post.h"

#if __VER >= 13 // __COUPLE_1117
#include "couplehelper.h"
#endif	// __COUPLE_1117

/*----------------------------------------------------------------------------------------------------------------------------------------------------------------*/
extern	CDPSrvr		g_DPSrvr;
extern	CWorldMng	g_WorldMng;
extern	CUserMng	g_UserMng;
/*----------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#ifndef __VM_0820
#ifndef __MEM_TRACE
#ifdef __INTERNALSERVER
	#ifdef __VM_0819
	CUserPool*	CUser::m_pPool	= new CUserPool( 5, "CUser" );
	#else	// __VM_0819
	CUserPool*	CUser::m_pPool	= new CUserPool( 5 );
	#endif	// __VM_0819
#else	// __INTERNALSERVER
	#ifdef __VM_0819
	CUserPool*	CUser::m_pPool	= new CUserPool( 512, "CUser" );
	#else	// __VM_0819
	CUserPool*	CUser::m_pPool	= new CUserPool( 512 );
	#endif	// __VM_0819
#endif	// __INTERNALSERVER
#endif	// __MEM_TRACE
#endif	// __VM_0820

CUserMng	g_UserMng;

CUser::CUser()
{
	Init();
}

CUser::CUser( DPID dpidCache, DPID dpidUser )
{
	Init( dpidCache, dpidUser );
}

CUser::~CUser()
{
	RemoveItFromView2();	// npc
}

BOOL CUser::IsValid()
{
	if( IsDelete() )
		return FALSE;
	else
		return m_bValid;
}

void CUser::Init( DPID dpidCache, DPID dpidUser )
{
	CMover::m_bPlayer	= TRUE;

	
	//////////////////////////////////////////////////////////////////////////
	m_bCheckTransMailBox = FALSE;
	m_nCountFromClient = 0;
	m_dwTickFromClient = 0;
	//////////////////////////////////////////////////////////////////////////


	m_dwSerial = 0;	
	m_bValid = FALSE;
	m_Snapshot.dpidCache = dpidCache;
	m_Snapshot.dpidUser  = dpidUser;

	m_dwAuthKey		= 0;
	m_idSetTarget	= NULL_ID;
	m_nOverHeal		= 0;
	*m_szPartyName	= '\0';

	memset( (void*)m_szBankPass, 0, sizeof(char) * 5 );
	memset( &m_playAccount, 0, sizeof(PLAY_ACCOUNT) );


//	m_idTargetCollect = NULL_ID;	
//	m_nCollect = 0;					
//	m_tmCollect = 0;
	m_idChatting = 0;
	memset( m_pWall, 0, sizeof(m_pWall) );
	m_dwLeavePenatyTime = 0;
	m_dwDestroyTime = 0;
	m_tmEscape = 0;
	m_idSnoop		= 0;

	m_dwReturnWorldID = 0;
	m_vReturnPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_lpszVillage = NULL;

	m_bPosting	= FALSE;
	m_bInstantBank	= FALSE;

	DWORD dwTick = ::timeGetTime();
	m_dwTickSFS = dwTick + 1000;
	m_dwTickNotify = dwTick + 200;
	m_dwTimeout4Save = SEC_SAVEPLAYER;

#ifdef __EVENT_1101
#ifdef __EVENT_1101_2
	m_nEventFlag = 0;
#else // __EVENT_1101_2
	m_dwEventFlag	= 0;
#endif // __EVENT_1101_2
	m_dwEventTime	= 0;
	m_dwEventElapsed	= 0;
#endif	// __EVENT_1101
#ifdef __HACK_1130
	m_tmDuelRequest		= 0;
#endif	// __HACK_1130

#ifdef __S_SERVER_UNIFY
	m_bAllAction = TRUE;
#endif // __S_SERVER_UNIFY
#ifdef __QUEST_1208
	m_tickScript	= GetTickCount();
#endif	// __QUEST_1208
	
#ifdef __EVE_MINIGAME
	m_nKawibawiboWin	= 0;
	m_nKawibawiboState  = 3;
	for ( int i=0; i<6; ++i )
		m_nBetFiveSystem[i] = 0;
#endif // __EVE_MINIGAME

#ifdef __EXP_ANGELEXP_LOG
	m_nExpLog = 0;
	m_nAngelExpLog = 0;
#endif // __EXP_ANGELEXP_LOG

#if __VER >= 11 // __SYS_COLLECTING
	m_nCollecting	= 0;
#endif	// __SYS_COLLECTING

#ifdef __EVENTLUA_COUPON
	m_nCoupon = 0;
	m_dwTickCoupon = GetTickCount();
#endif // __EVENTLUA_COUPON

#if __VER >= 12 // __LORD
	m_idElection	= NULL_ID;
	m_bQuerying		= FALSE;
#endif	// __LORD

#ifdef __AZRIA_1023
	m_szInput[0]	= '\0';
#endif	// __AZRIA_1023

#if __VER >= 13 // __COUPLE_1117
	m_idProposer	= 0;
#if __VER >= 13 // __COUPLE_1202
	m_cbProcessCouple	= 0;
#endif	// __COUPLE_1202
#endif	// __COUPLE_1117

#ifdef __EVENTLUA_KEEPCONNECT
	m_dwTickKeepConnect = GetTickCount();
#endif // __EVENTLUA_KEEPCONNECT
#if __VER >= 15 // __GUILD_HOUSE
	m_nRestPoint = 0;
	m_nPrevRestPoint = 0;
	m_tRestPointTick = time_null();
#endif // __GUILD_HOUSE

#ifdef __PERIN_BUY_BUG
	m_dwLastTryBuyItem = NULL_ID;
	m_dwLastBuyItemTick = GetTickCount();
#endif // __PERIN_BUY_BUG
}

void CUser::LevelUpSetting( void )
{
	g_UserMng.AddSetLevel( this, (short)GetLevel() );
	AddSetGrowthLearningPoint( m_nRemainGP );
	g_dpDBClient.SendLogLevelUp( this, 1 );	// 레벨업 로그
#if __VER >= 11 // __SYS_PLAYER_DATA
	g_dpDBClient.SendUpdatePlayerData( this );
#else	// __SYS_PLAYER_DATA
	if( 0 < GetPartyId() )
		g_DPCoreClient.SendPartyMemberLevel( this );
	if( m_idGuild != 0 )
		g_DPCoreClient.SendGuildChangeJobLevel( this );
#endif	// __SYS_PLAYER_DATA
}

void CUser::ExpUpSetting( void )
{
	// 레벨 5이상 로그_레벨업 테이블에 로그를 남긴다
	// 20% 단위로 로그를 남김
	if( GetLevel() > 5 ) // 레벨 5이상
	{
#ifdef __EXP_ANGELEXP_LOG
		int nNextExpLog = (int)(m_nExpLog/20 + 1) * 20;	
		int nExpPercent = (int)( GetExp1() * 100 / GetMaxExp1() );
		if( nExpPercent >= nNextExpLog )
		{
			m_nExpLog = nExpPercent;
			g_dpDBClient.SendLogLevelUp( this, 5 );
		}
#else // __EXP_ANGELEXP_LOG
		int iLogExp = GetExp1() * 100 / GetMaxExp1();
		iLogExp /= 20;
		
		if( ( 20 * ( iLogExp + 1 ) ) <= ( GetExp1() * 100 / GetMaxExp1() ) )
			g_dpDBClient.SendLogLevelUp( this, 5 );
#endif // __EXP_ANGELEXP_LOG
	}
}

void CUser::RemoveItFromView2( BOOL bRemoveall )
{
	CCtrl* pCtrl;

	map<DWORD, CCtrl*>::iterator it = m_2npc.begin();
	for( ; it != m_2npc.end(); ++it )
	{
		pCtrl = it->second;
		pCtrl->PCRemoveKey( GetId() );
	}
	if( bRemoveall )
		m_2npc.clear();
}

#ifdef __MAP_SECURITY
void CUser::Open( DWORD dwWorldId )
#else // __MAP_SECURITY
void CUser::Open( void )
#endif // __MAP_SECURITY
{
	AddItToGlobalId();
	PCSetAt( GetId(), (CCtrl*)this );
	m_Snapshot.SetSnapshot( GetId(),  PACKETTYPE_JOIN );

#ifdef __ENVIRONMENT_EFFECT

	AddEnvironmentSetting();

#endif // __ENVIRONMENT_EFFECT

#ifdef __MAP_SECURITY
	AddWorldReadInfo( dwWorldId, GetPos() );
#endif // __MAP_SECURITY
	AddAddObj( (CCtrl*)this );
	AddGameTimer( g_GameTimer.GetCurrentTime() );
	AddTaskBar();
	AddEnvironment();
#if __VER >= 11 // __SYS_PLAYER_DATA
	AddPlayerData();
#endif	// __SYS_PLAYER_DATA
	AddFriendGameJoin();
	AddPartyName();
	ADDGameJoin();

	AddAllGuilds();
	AddMyGuild();
	AddMyGuildWar();

	AddFlyffEvent();

#if __VER >= 12 // __LORD
	AddLord();
#endif	// __LORD
#if __VER >= 13 // __COUPLE_1117
	AddCouple();
#endif	// __COUPLE_1117
		
/*	// chipi_090617 - 불필요하다.
	CItemElem *pShield = GetEquipItem( PARTS_SHIELD );
	if( pShield )	// 방패를 차고 있냐
	{
		ItemProp *pHandItemProp = GetActiveHandItemProp();
		if( pHandItemProp->dwHanded == HD_TWO )	// 손에든 무기가 양손무기냐.
		{
			DoEquip( pShield, 0 );	// 방패를 벗겨냄;
			AddDoEquip( pShield->m_dwObjId, pShield->m_dwItemId, 0 );
		}
	}
*/
}

// 유저의 주기적 처리 
// 200ms   Notify
// 10000ms SubSMMode
// 3Min    CheckFiniteItem, SavePlayer
void CUser::Process()
{
	if( IsValid() == FALSE )
		return;

	DWORD dwTick	= g_tmCurrent;

	if( IsMode( MODE_OUTOF_PARTYQUESTRGN ) )
	{
		SetNotMode( MODE_OUTOF_PARTYQUESTRGN );
		D3DXVECTOR3 vPos	= D3DXVECTOR3( 6968.0f, 0, 3328.8f );
		REPLACE( g_uIdofMulti, WI_WORLD_MADRIGAL, vPos, REPLACE_NORMAL, nDefaultLayer );
		return;
	}

	if( m_dwDestroyTime && dwTick > m_dwDestroyTime )
	{
		g_UserMng.DestroyPlayer( this );
		return;
	}

	if( IsStateMode( STATE_BASEMOTION_MODE ) )
	{
		if( (int)( dwTick ) >= m_nReadyTime )
		{	
			m_nReadyTime	= 0;
			if( m_bItemFind )
			{
				CItemElem* pItemElem = m_Inventory.GetAtId( m_dwUseItemId );
				if( pItemElem )
				{
					DoUseItem( MAKELONG( ITYPE_ITEM, pItemElem->m_dwObjId ), pItemElem->m_dwObjId );
				}
				else
				{
					AddDefinedText(TID_PK_BLINK_LIMIT, "" );	// 시전중에는 사용할수 없습니다
					SetStateNotMode( STATE_BASEMOTION_MODE, STATEMODE_BASEMOTION_CANCEL );	// 셑팅 빼기
					m_dwUseItemId = 0;
				}
			}
			else
			{
				ItemProp* pItemProp = prj.GetItemProp( m_dwUseItemId );
				if( pItemProp )
				{
					DoUseItemVirtual( m_dwUseItemId, FALSE );
				}
			}
		}
	}

#ifdef __SYS_TICKET
	if( ( m_nCount & 15 ) == 0 )
	{
		if( GetWorld() && CTicketProperty::GetInstance()->IsTarget( GetWorld()->GetID() ) )
		{
			CItemElem* pTicket	= GetTicket();
			if( !pTicket )
			{
				RemoveIk3Buffs( IK3_TICKET );
				REPLACE( g_uIdofMulti, WI_WORLD_MADRIGAL, D3DXVECTOR3( 6971.984F, 100.0F, 3336.884F ), REPLACE_FORCE, nDefaultLayer );
			}
		}
		else
		{
			RemoveIk3Buffs( IK3_TICKET );
		}
#if __VER >= 13 // __COUPLE_1202
		ProcessCouple();
#endif // __COUPLE_1202
#if __VER >= 15 // __CAMPUS
		ProcessCampus();
		CCampusHelper::GetInstance()->RecoveryCampusPoint( this, dwTick );
#endif // __CAMPUS
	}
#endif	// __SYS_TICKET

	CheckTickCheer();

	CMover::Process();

	if( IsLive() )
	{
		if( m_dwFlag & MVRF_MEDITATION_SIT )
		{
			if( m_pActMover->IsStateFlag( OBJSTAF_SIT ) ) 
			{
				m_dwFlag &= (~MVRF_MEDITATION_SIT);
			} 
			else
			{
				if( SendActMsg( OBJMSG_SITDOWN ) == 1 )
				{
					ClearDest();
					ClearDestAngle();
					memset( &m_CorrAction, 0, sizeof(CORR_ACTION) );
					
					g_UserMng.AddMotion( this, OBJMSG_SITDOWN );
					m_dwFlag &= (~MVRF_MEDITATION_SIT);
				}
			}
		}


		// 비행연료 자동 소모
		if( m_nFuel > 0 )
		{
			if( (m_nCount & 15) == 0 )
			{
				if( m_pActMover->IsFly() && m_pActMover->IsStateFlag( OBJSTAF_ACC ) )
				{
					ItemProp *pItemProp = prj.GetItemProp( m_dwRideItemIdx );
					if( pItemProp )
						AddSetFuel( m_nFuel, m_tmAccFuel );	// 클라에 변화된 양을 전송
				}
			}
		}

		if( m_nOverHeal > 0)
			--m_nOverHeal;
	}

	if( dwTick > m_dwTickNotify )				// 200ms마다 
	{
		m_dwTickNotify = dwTick + 200;
		Notify();
	}

	if( dwTick > m_dwTickSFS )					// 1초마다 
	{
//		m_dwTickSFS = dwTick + 1000;
		m_dwTickSFS	+= 1000;	// 초당 오차가 평균 33 ms	- 康
#if __VER >= 11 // __CHIPI_071210
		CheckFiniteItem();
#endif // __CHIPI_071210
		#ifdef __EVENTLUA_COUPON
		prj.m_EventLua.SetCoupon( this, dwTick );
		#endif // __EVENTLUA_COUPON
#ifdef __EVENTLUA_KEEPCONNECT
		prj.m_EventLua.SetKeepConnectEvent( this, dwTick );
#endif // __EVENTLUA_KEEPCONNECT

		CWorld* pWorld = GetWorld();
		if( pWorld )
		{
			SubSMMode();
			if( --m_dwTimeout4Save == 0 )		// 180초마다 
			{
#if __VER < 11 // __CHIPI_071210
				CheckFiniteItem();
#endif // __CHIPI_071210
#if __VER >= 13 // __HONORABLE_TITLE			// 달인
				CheckHonorTime();
#endif	// __HONORABLE_TITLE			// 달인
#ifdef __LAYER_1015
				g_dpDBClient.SavePlayer( this, pWorld->GetID(), GetPos(), GetLayer() );
#else	// __LAYER_1015
				g_dpDBClient.SavePlayer( this, pWorld->GetID(), GetPos() );
#endif	// __LAYER_1015
				AddEventLuaDesc();
			}
#if __VER >= 15 // __GUILD_HOUSE
			if( GuildHouseMng->IsGuildHouse( pWorld->GetID() ) && GetLayer() == m_idGuild )
			{
				if( m_tRestPointTick <= time_null() )	
				{
					SetIncRestPoint( REST_POINT_INC );
					SetRestPointTick();
				}
			}
			
			if( m_nRestPoint != m_nPrevRestPoint )
			{
				AddRestPoint();
				m_nPrevRestPoint = m_nRestPoint;
			}
#endif // __GUILD_HOUSE
		}
#ifdef __VTN_TIMELIMIT
		if( m_nAccountPlayTime > -1 )
			m_nAccountPlayTime += SEC( 1 );
#endif // __VTN_TIMELIMIT
	}


#if __VER >= 8 // __S8_PK
	if( IsPKPink() && GetTickCount() > GetPKPink() )
	{
		SetPKPink( 0 );
		g_UserMng.AddPKPink( this, 0 );
	}
#endif // __VER >= 8 // __S8_PK
#if __VER >= 15 // __REACTIVATE_EATPET
	if( HasActivatedEatPet() )
	{
		CMover* pEatPet = prj.GetMover( GetEatPetId() );
		if( IsValidObj( pEatPet ) )
		{
			if( !IsValidArea( pEatPet, 32 ) )
			{
				CAIPet* pAIPet	= static_cast<CAIPet*>( pEatPet->m_pAIInterface );
				if( pAIPet )
				{
					CItemElem* pItemElem = (CItemElem*)GetItemId( pAIPet->GetPetItemId() );
					InactivateEatPet();
					if( IsUsableItem( pItemElem ) )
						DoUseItem( MAKELONG( ITYPE_ITEM, pItemElem->m_dwObjId ), pItemElem->m_dwObjId );
				}
				else
					InactivateEatPet();
			}
		}
	}
#endif // __REACTIVATE_EATPET
}

//void CUser::Notify( void )
int	CUser::Notify( void )
{
	if( IsValid() == FALSE )
		return	0;

	if( m_dwDestroyTime )		// 종료예약 유저는 Notify하지 않는다.
		return 0;

	static u_int uOffset	= sizeof(DPID)+sizeof(DWORD)+sizeof(OBJID)+sizeof(short);
	LPBYTE lpBuf;
	int nBufSize;

	if( m_Snapshot.cb > 0 )
	{
		lpBuf	= m_Snapshot.ar.GetBuffer( &nBufSize );
		*(UNALIGNED WORD*)( lpBuf + sizeof(DPID) + sizeof(DWORD) + sizeof(OBJID) )	= m_Snapshot.cb;
		g_DPSrvr.Send( (LPVOID)lpBuf, nBufSize, m_Snapshot.dpidCache );
		*(UNALIGNED DWORD*)( lpBuf + sizeof(DPID) )		= PACKETTYPE_SNAPSHOT;
		m_Snapshot.cb	= 0;
		m_Snapshot.ar.ReelIn( uOffset );
		return nBufSize;
	}
	return 0;
}

#ifdef __S_SERVER_UNIFY
void CUser::SetAllAction()
{
	// 캐릭 이름이 첫글자가 숫자로 사용하면 못움직이게 하고 이름을 바꾸게 하자
	CString strName = GetName();
	LPCTSTR lpszString = strName;
	char c = strName[ 0 ];
	BOOL bCharacter = FALSE;
	m_bAllAction = TRUE;
	if( ( c >= '0' && c <= '9' ) && !IsMultiByte( lpszString ) ) // "명칭에 첫글자를 숫자로 사용할 수 없습니다."
	{
		m_bAllAction = FALSE;
		bCharacter = TRUE;
	}
	// 길드 이름이 첫글자가 숫자로 사용하면 못움직이게 하고 이름을 바꾸게 하자
	else if( m_idGuild != 0 )
	{
		CGuild* pGuild	= g_GuildMng.GetGuild( m_idGuild );
		if( pGuild && pGuild->IsMaster( m_idPlayer ) )
		{
			CString strGuildName = pGuild->m_szGuild;
			LPCTSTR lpszGuildString = strGuildName;
#ifdef _DEBUG
			if( !strGuildName.IsEmpty() )
#endif // _DEBUG
			{
				c = strGuildName[ 0 ];
				if( ( c >= '0' && c <= '9' ) && !IsMultiByte( lpszGuildString ) ) // "명칭에 첫글자를 숫자로 사용할 수 없습니다."
					m_bAllAction = FALSE;
			}
		}
	}
	AddAllAction( bCharacter );
}
#endif // __S_SERVER_UNIFY

void CUser::AddBlock( LPBYTE lpBlock, u_long uBlockSize )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar.Write( lpBlock, uBlockSize );
}

void CUser::AddHdr( OBJID objid, WORD wHdr )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << objid;
	m_Snapshot.ar << wHdr;
}

//this: i, pCtrl: other
void CUser::AddAddObj( CCtrl * pCtrl )
{
	if( IsDelete() )	return;

	//	mulcom	BEGIN100430
	if( pCtrl == NULL )
	{
		return;
	}
	//	mulcom	END100430

	m_Snapshot.cb++;
	m_Snapshot.ar << pCtrl->GetId() << SNAPSHOTTYPE_ADD_OBJ << (BYTE)pCtrl->GetType() << pCtrl->GetIndex();
	CObj::SetMethod( pCtrl == this ? METHOD_NONE : METHOD_EXCLUDE_ITEM );
	pCtrl->Serialize( m_Snapshot.ar );
}

void CUser::AddText( LPCSTR lpsz )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_TEXT;
#ifdef __S_SERVER_UNIFY
	m_Snapshot.ar << TEXT_GENERAL;
#endif // __S_SERVER_UNIFY
	m_Snapshot.ar.WriteString( lpsz );
}


void CUser::AddDiagText( LPCSTR lpsz )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
#ifdef __S_SERVER_UNIFY
	m_Snapshot.ar << SNAPSHOTTYPE_TEXT;
	m_Snapshot.ar << TEXT_DIAG;
#else // __S_SERVER_UNIFY
	m_Snapshot.ar << SNAPSHOTTYPE_DIAG_TEXT;
#endif // __S_SERVER_UNIFY
	m_Snapshot.ar.WriteString( lpsz );
}

#ifdef __S_SERVER_UNIFY
void CUser::AddAllAction( BOOL bCharacter )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ALLACTION;
	m_Snapshot.ar << m_bAllAction;
	m_Snapshot.ar << bCharacter;
}
#endif // __S_SERVER_UNIFY


void CUser::AddReplace( DWORD dwWorldID, D3DXVECTOR3 & vPos )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_REPLACE;
	m_Snapshot.ar << dwWorldID;
	m_Snapshot.ar << vPos;
}

void CUser::AddCreateItem( CItemBase* pItemBase, BYTE* pnId, short* pnNum, BYTE nCount )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CREATEITEM;
	m_Snapshot.ar << (BYTE)0;
	pItemBase->Serialize( m_Snapshot.ar );
	m_Snapshot.ar << nCount;
	m_Snapshot.ar.Write( (void*)pnId, sizeof(BYTE) * nCount );
	m_Snapshot.ar.Write( (void*)pnNum, sizeof(short) * nCount );
}

void CUser::AddMoveItem( BYTE nItemType, BYTE nSrcIndex, BYTE nDestIndex )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_MOVEITEM;
	m_Snapshot.ar << nItemType << nSrcIndex << nDestIndex;
}

void CUser::AddTrade( CUser* pTrader, u_long uidPlayer )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GETID( pTrader );
	m_Snapshot.ar << SNAPSHOTTYPE_TRADE;
	m_Snapshot.ar << uidPlayer;

	pTrader->m_Inventory.Serialize( m_Snapshot.ar );
}

void CUser::AddComfirmTrade( OBJID objid )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << objid;
	m_Snapshot.ar << SNAPSHOTTYPE_CONFIRMTRADE;
}

void CUser::AddComfirmTradeCancel( OBJID objid )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << objid;
	m_Snapshot.ar << SNAPSHOTTYPE_CONFIRMTRADECANCEL;
}

void CUser::AddExpBoxCoolTime( OBJID objid, DWORD dwTime )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_EXPBOXCOLLTIME;
	m_Snapshot.ar << objid;
	m_Snapshot.ar << dwTime;
}


void CUser::AddCommonSkill( DWORD dwSkill, DWORD dwLevel )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_COMMONSKILL;
	m_Snapshot.ar << dwSkill << dwLevel;
}

void CUser::AddTradePut( OBJID objid, BYTE i , BYTE nItemType, BYTE nId, short nItemNum )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << objid;
	m_Snapshot.ar << SNAPSHOTTYPE_TRADEPUT;
	m_Snapshot.ar << i << nItemType << nId << nItemNum;
}

void CUser::AddTradePutError( void )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_TRADEPUTERROR;
}

void CUser::AddTradePull( OBJID objid, BYTE i )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << objid;
	m_Snapshot.ar << SNAPSHOTTYPE_TRADEPULL;
	m_Snapshot.ar << i;
}

void CUser::AddTradePutGold( OBJID objid, DWORD dwGold )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << objid;
	m_Snapshot.ar << SNAPSHOTTYPE_TRADEPUTGOLD;
	m_Snapshot.ar << dwGold;
}
//raiders.2006.11.28
/*
void CUser::AddTradeClearGold( OBJID objid )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << objid;
	m_Snapshot.ar << SNAPSHOTTYPE_TRADECLEARGOLD;
}
*/
void CUser::AddTradeCancel( OBJID objid, u_long uidPlayer, int nMode )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << objid;
	m_Snapshot.ar << SNAPSHOTTYPE_TRADECANCEL;
	m_Snapshot.ar << uidPlayer;
	m_Snapshot.ar << nMode;
}



void CUser::AddOpenShopWnd( CMover* pVendor )
{
	int i;
	m_Snapshot.cb++;
	m_Snapshot.ar << pVendor->GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_OPENSHOPWND;
	for( i = 0; i < MAX_VENDOR_INVENTORY_TAB; i++ )
		pVendor->m_ShopInventory[i]->Serialize( m_Snapshot.ar );
}

void CUser::AddUnregisterPVendorItem( BYTE iIndex )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_UNREGISTER_PVENDOR_ITEM;
	m_Snapshot.ar << iIndex;
}

void CUser::AddRegisterPVendorItem( BYTE iIndex, BYTE nType, BYTE nId, short nNum, int nCost )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_REGISTER_PVENDOR_ITEM;
	m_Snapshot.ar << iIndex << nType << nId << nNum << nCost;
}

void CUser::AddPVendorClose( OBJID objidVendor )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << objidVendor;
	m_Snapshot.ar << SNAPSHOTTYPE_PVENDOR_CLOSE;
	m_Snapshot.ar << (BYTE)0;
}

void CUser::AddPVendorItem( CUser* pUser, BOOL bState )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << pUser->GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PVENDOR_ITEM;

	u_long uOffset	= m_Snapshot.ar.GetOffset();
	BYTE nVendorItem	= 0;

	m_Snapshot.ar << (BYTE)0;	// nVendorItem
	for( int i = 0; i < MAX_VENDITEM; i++ )
	{
		CItemElem* pItemElem = (CItemElem *)pUser->m_vtInfo.GetItem( i );
		if( pItemElem  == NULL )
			continue;

		m_Snapshot.ar << (BYTE)i;
		pItemElem->Serialize( m_Snapshot.ar );
		m_Snapshot.ar << pItemElem->GetExtra();
		m_Snapshot.ar << pItemElem->m_nCost;
		nVendorItem++;
	}
	m_Snapshot.ar << bState;

	GETBLOCK( m_Snapshot.ar, lpBlock, nBlockSize );
	*(UNALIGNED BYTE*)( lpBlock + uOffset )	= nVendorItem;
}

void CUser::AddPutItemGuildBank( CItemElem* pItemElem )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PUTITEMGUILDBANK;
	m_Snapshot.ar << (BYTE)1; // 아이템을 받는 헤더
	pItemElem->Serialize( m_Snapshot.ar );
}

void CUser::AddGetItemGuildBank( CItemElem* pItemElem )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GETITEMGUILDBANK;
	m_Snapshot.ar << (BYTE)1; // 아이템을 보내는 헤더
	pItemElem->Serialize( m_Snapshot.ar );
}

void CUser::AddGetGoldGuildBank( DWORD p_Gold, BYTE p_Mode, u_long playerID, BYTE cbCloak )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GETITEMGUILDBANK;
	m_Snapshot.ar << (BYTE)p_Mode; // Gold를 보내는 헤더(0, 2가 들어온다.)
	m_Snapshot.ar << p_Gold;
	m_Snapshot.ar << playerID;	   // 길드돈을 빼내간 길원
	m_Snapshot.ar << cbCloak;	   // 망토를 산경우 (= 1)
}


void CUser::AddPutItemBank( BYTE nSlot, CItemElem* pItemElem )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PUTITEMBANK;
	m_Snapshot.ar << nSlot;
	pItemElem->Serialize( m_Snapshot.ar );
}

void CUser::AddGetItemBank( CItemElem* pItemElem )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GETITEMBANK;
	pItemElem->Serialize( m_Snapshot.ar );
}

void CUser::AddPutGoldBank( BYTE nSlot, DWORD dwGold, DWORD dwGoldBank )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PUTGOLDBANK;
	m_Snapshot.ar << nSlot << dwGold << dwGoldBank;
}

void CUser::AddMoveBankItem( BYTE nSrcIndex, BYTE nDestIndex )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_MOVEBANKITEM;
	m_Snapshot.ar << nSrcIndex << nDestIndex;
}

void CUser::AddBankIsFull( void )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_BANKISFULL;
}

void CUser::AddRemoveSkillInfluence( WORD wType, WORD wID )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_REMOVESKILLINFULENCE;
	m_Snapshot.ar << wType;
	m_Snapshot.ar << wID;
	
}

void CUser::AddBankWindow( int nMode, DWORD dwId, DWORD dwItemId )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_BANKWINDOW;
	m_Snapshot.ar << nMode;
	m_Snapshot.ar << dwId << dwItemId;

}

void CUser::AddGuildBankWindow( int nMode )
{
	if( IsDelete() )	return;
	

	CGuild* pGuild = GetGuild();
	if (pGuild)
	{
		m_Snapshot.cb++;
		m_Snapshot.ar << GetId();
		m_Snapshot.ar << SNAPSHOTTYPE_GUILD_BANK_WND;
		m_Snapshot.ar << nMode;
		
		m_Snapshot.ar << pGuild->m_nGoldGuild;
		pGuild->m_GuildBank.Serialize(m_Snapshot.ar);
	}
}

void CUser::AddChangeBankPass( int nMode, DWORD dwId, DWORD dwItemId )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CHANGEBANKPASS;
	m_Snapshot.ar << nMode;
	m_Snapshot.ar << dwId << dwItemId;
}

void CUser::AddconfirmBankPass( int nMode, DWORD dwId, DWORD dwItemId )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CONFIRMBANKPASS;
	m_Snapshot.ar << nMode;
	m_Snapshot.ar << dwId << dwItemId;
}

#if __VER >= 11 // __SYS_IDENTIFY
void CUser::AddUpdateItemEx( unsigned char id, char cParam, __int64 iValue )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_UPDATE_ITEM_EX;
	m_Snapshot.ar << id << cParam << iValue;
}
#endif	// __SYS_IDENTIFY

#if __VER >= 15 // __PETVIS
void CUser::AddUpdateItem( CHAR cType, BYTE nId, CHAR cParam, DWORD dwValue, DWORD dwTime )
#else // __PETVIS
void CUser::AddUpdateItem( CHAR cType, BYTE nId, CHAR cParam, DWORD dwValue )
#endif // __PETVIS
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_UPDATE_ITEM;
	m_Snapshot.ar << cType << nId << cParam << dwValue;
#if __VER >= 15 // __PETVIS
	m_Snapshot.ar << dwTime;
#endif // __PETVIS
}

void CUser::AddUpdateBankItem( BYTE nSlot, BYTE nId, CHAR cParam, DWORD dwValue )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_UPDATE_BANKITEM;
	m_Snapshot.ar << nSlot << nId << cParam << dwValue;
}

void CUser::AddSetExperience( EXPINTEGER nExp1, WORD wLevel, int nSkillPoint, int nSkillLevel, EXPINTEGER nDeathExp, WORD wDeathLevel )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SETEXPERIENCE;
	m_Snapshot.ar << nExp1 << wLevel;
	m_Snapshot.ar  << nSkillLevel << nSkillPoint;
	m_Snapshot.ar << nDeathExp << wDeathLevel;
}

void CUser::AddSetFxp( int nFxp, int nFlightLv )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SETFXP;
	m_Snapshot.ar << (WORD)nFxp << (WORD)nFlightLv;
}

void CUser::AddSetGrowthLearningPoint( long nRemainGP )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SET_GROWTH_LEARNING_POINT;
	m_Snapshot.ar << nRemainGP << (long)0;
}

void CUser::AddSetStatLevel( CHAR chID, long nValue, long nRemainGP )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SET_STAT_LEVEL;
	m_Snapshot.ar << chID << nValue << nRemainGP;
}

void CUser::AddSetChangeJob( int nJob )
{
	DWORD dwJobLv[MAX_JOB] = {0, };

	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SET_JOB_SKILL;
	m_Snapshot.ar << nJob;
#ifdef __3RD_LEGEND16
	m_Snapshot.ar.Write( (void*)&m_aJobSkill[0], sizeof(SKILL) *  ( MAX_JOB_SKILL + MAX_EXPERT_SKILL + MAX_PRO_SKILL + MAX_MASTER_SKILL + MAX_HERO_SKILL + MAX_LEGEND_HERO_SKILL ) );
#else // __3RD_LEGEND16
#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
	m_Snapshot.ar.Write( (void*)&m_aJobSkill[0], sizeof(SKILL) *  ( MAX_JOB_SKILL + MAX_EXPERT_SKILL + MAX_PRO_SKILL + MAX_MASTER_SKILL + MAX_HERO_SKILL ) );
#else //__LEGEND	//	10차 전승시스템	Neuz, World, Trans
	m_Snapshot.ar.Write( (void*)&m_aJobSkill[0], sizeof(SKILL) *  ( MAX_JOB_SKILL + MAX_EXPERT_SKILL + MAX_PRO_SKILL ) );
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans
#endif // __3RD_LEGEND16
	m_Snapshot.ar.Write( (void*)dwJobLv, sizeof(DWORD) * MAX_JOB );
}

void CUser::AddReturnSay( int ReturnFlag, const CHAR* lpszPlayer )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_RETURNSAY;
	m_Snapshot.ar << ReturnFlag;
	m_Snapshot.ar.WriteString( lpszPlayer );
	
}

void CUser::AddGameTimer( double dCurrentTime )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GAMETIMER;
	m_Snapshot.ar << dCurrentTime;
	
}

void CUser::AddDoEquip( BYTE nId, DWORD dwItemId, BYTE fEquip )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_DOEQUIP;
	m_Snapshot.ar << nId << dwItemId << fEquip;
	
}

void CUser::AddTaskBar()
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_TASKBAR;
	m_playTaskBar.Serialize( m_Snapshot.ar );
	
}

void CUser::AddSendErrorParty( DWORD dw, DWORD dwSkill )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ERRORPARTY;
	m_Snapshot.ar << dw;
	if( dw == ERROR_NOTTARGET )
	{
		m_Snapshot.ar << dwSkill;
	}
	
}

void CUser::AddSetPartyMemberParam( u_long idPlayer, BYTE nParam, int nVal )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SET_PARTY_MEMBER_PARAM;
	m_Snapshot.ar << idPlayer << nParam << nVal;
	
}

#if __VER >= 11 // __SYS_PLAYER_DATA
void CUser::AddPartyMember( CParty *pParty, u_long idPlayer, const char* pszLeader, const char* pszMember )
#else	// __SYS_PLAYER_DATA
void CUser::AddPartyMember( CParty *pParty, u_long idPlayer )
#endif	// __SYS_PLAYER_DATA
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYMEMBER;
	m_Snapshot.ar << idPlayer;
#if __VER >= 11 // __SYS_PLAYER_DATA
	m_Snapshot.ar.WriteString( pszLeader );
	m_Snapshot.ar.WriteString( pszMember );
#endif	// __SYS_PLAYER_DATA
	if( pParty )
	{
		m_Snapshot.ar << pParty->m_nSizeofMember;
		pParty->Serialize( m_Snapshot.ar );
	}
	else
	{
		m_Snapshot.ar << (int)0;
	}
	
}

void CUser::AddPartyExpLevel( int Exp, int Level, int nPoint )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYEXP;
	m_Snapshot.ar << Exp;
	m_Snapshot.ar << Level;
	m_Snapshot.ar << nPoint;
	
}
#if __VER >= 12 // __JHMA_VER12_1	//12차 극단유료아이템
void CUser::AddSetPartyMode( int nMode, BOOL bOnOff, LONG nPoint ,DWORD dwSkillTime )			
#else // //12차 극단유료아이템
void CUser::AddSetPartyMode( int nMode, BOOL bOnOff, LONG nPoint )			
#endif // //12차 극단유료아이템
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SETPARTYMODE;
	m_Snapshot.ar << nMode;
#if __VER >= 12 // __JHMA_VER12_1	//12차 극단유료아이템
	m_Snapshot.ar << dwSkillTime;
#endif // //12차 극단유료아이템
	m_Snapshot.ar << bOnOff;
	if( bOnOff == TRUE )
	{
		m_Snapshot.ar << nPoint;
	}
	
}

void CUser::AddPartyChangeItemMode( int nItemMode )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYCHANGEITEMMODE;
	m_Snapshot.ar << nItemMode;
	
}

void CUser::AddPartyChangeExpMode( int nExpMode )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYCHANGEEXPMODE;
	m_Snapshot.ar << nExpMode;
	
}


void CUser::AddPartyChangeName( const char * szPartyName )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYCHANGENAME;
	m_Snapshot.ar.WriteString( szPartyName );
	
}

void CUser::AddPartyChangeTroup( const char * szPartyName )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYCHANGETROUP;
	m_Snapshot.ar.WriteString( szPartyName );
	
}

void CUser::AddPartyRequest( CUser * pLeader, CUser * pMember, BOOL bTroup )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYREQEST;
	m_Snapshot.ar << pLeader->m_idPlayer << pLeader->m_nLevel << pLeader->m_nJob << pLeader->GetSex();
	m_Snapshot.ar << pMember->m_idPlayer << pMember->m_nLevel << pMember->m_nJob << pMember->GetSex();
	m_Snapshot.ar.WriteString( pLeader->m_szName );
	m_Snapshot.ar << bTroup;
	
}

void CUser::AddPartyRequestCancel( u_long uLeaderid, u_long uMemberid, int nMode )
{
	// nMode
	// 0 : 실제로 캔슬
	// 1 : 이미 극단에 포함되어 있음
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYREQESTCANCEL;
	m_Snapshot.ar << uLeaderid << uMemberid;
	m_Snapshot.ar << nMode;
	
}

void CUser::AddPartyName()
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ADDPARTYNAME;
	m_Snapshot.ar.WriteString( m_szPartyName );
	
}

// nSkill : 파티스킬 ID
// vLeader : 리더의 좌표.
void CUser::AddPartySkillCall( const D3DXVECTOR3 &vLeader )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYSKILL_CALL;
	m_Snapshot.ar << vLeader;
	
}

void	CUser::AddPartySkillBlitz( OBJID idTarget )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYSKILL_BLITZ;
	m_Snapshot.ar << idTarget;
	
}

void CUser::AddFriendGameJoin()
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ADDFRIENDGAMEJOIN;
#ifdef __RT_1025
	m_RTMessenger.Serialize( m_Snapshot.ar );
#else	// __RT_1025
	m_Messenger.Serialize( m_Snapshot.ar );
#endif	// __RT_1025
}

void CUser::AddPartyChangeLeader( u_long uidChangeLeader )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ADDPARTYCHANGELEADER;
	m_Snapshot.ar << uidChangeLeader;
	
}

void CUser::AddCancelQuest( DWORD dwQuestCancelID )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_REMOVEQUEST;
	m_Snapshot.ar << (int)-1 << dwQuestCancelID;
	
}

void CUser::AddRemoveQuest( DWORD dwQuestCancelID )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_REMOVEQUEST;
	m_Snapshot.ar << (int)0 << dwQuestCancelID;
	
}

void CUser::AddRemoveAllQuest()
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_REMOVEQUEST;
	m_Snapshot.ar << (int)1 << (DWORD)0;
	
}
void CUser::AddRemoveCompleteQuest()
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_REMOVEQUEST;
	m_Snapshot.ar << (int)2 << (DWORD)0;
	
}

void CUser::ADDGameJoin( void )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ADDGAMEJOIN;
	m_Snapshot.ar << m_dwSavePlayerTime;
	
}

void CUser::AddFriendReqest( u_long uLeader, LONG nJob, BYTE nSex, const char * szName )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ADDFRIENDREQEST;
	m_Snapshot.ar << uLeader;
	m_Snapshot.ar << nSex;
	m_Snapshot.ar << nJob;
	m_Snapshot.ar.WriteString( szName );
	
}

void CUser::AddFriendCancel()
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ADDFRIENDCANCEL;
	
}

void CUser::AddFriendError( BYTE nError, const char * szName )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ADDFRIENDERROR;
	m_Snapshot.ar << nError;
	m_Snapshot.ar.WriteString( szName );
	
}

#if __VER < 11 // __SYS_PLAYER_DATA
void CUser::AddGetFriendName( int nCount, FRIEND* aFriend )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ADDGETFRIENDNAME;
	m_Snapshot.ar << nCount;

	for( int i = 0 ; i < nCount ; i++ )
	{
		m_Snapshot.ar << aFriend[i].dwUserId;
		m_Snapshot.ar.WriteString( aFriend[i].szName );
	}
}
#endif	// __SYS_PLAYER_DATA

#ifdef __ENVIRONMENT_EFFECT

void CUser::AddEnvironmentSetting()
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ENVIRONMENTALL;
	m_Snapshot.ar << CEnvironment::GetInstance()->GetSeason();
}

#endif // __ENVIRONMENT_EFFECT

void CUser::AddEnvironment( void )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ENVIRONMENT;

#ifdef __ENVIRONMENT_EFFECT

	CEnvironment::GetInstance()->Serialize( m_Snapshot.ar );
	
	if( CEnvironment::GetInstance()->GetEnvironmentEffect() == TRUE )
	{
		m_Snapshot.ar.WriteString( prj.m_EventLua.GetWeatherEventTitle().c_str() );
	}

#else // __ENVIRONMENT_EFFECT

	g_Environment.Serialize( m_Snapshot.ar );
#ifdef __EVENTLUA_RAIN
	if( g_Environment.m_bRain )
		m_Snapshot.ar.WriteString( prj.m_EventLua.GetRainEventTitle().c_str() );
#endif // __EVENTLUA_RAIN
#ifdef __EVENTLUA_SNOW
	if( g_Environment.m_bSnow )
		m_Snapshot.ar.WriteString( prj.m_EventLua.GetSnowEventTitle().c_str() );
#endif // __EVENTLUA_SNOW

#endif // __ENVIRONMENT_EFFECT
}
void CUser::AddPartyChat( const CHAR* lpName, const CHAR* lpString, OBJID objid )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYCHAT;
	m_Snapshot.ar << objid;

	m_Snapshot.ar.WriteString( lpName );
	m_Snapshot.ar.WriteString( lpString );
	
}

#ifdef __RT_1025
void CUser::AddAddFriend( u_long idPlayer, const char* lpszPlayer )
#else	// __RT_1025
void CUser::AddAddFriend( u_long idPlayer, LONG nJob, DWORD dwSex, const char* lpszPlayer )
#endif	// __RT_1025
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ADDFRIEND;
	m_Snapshot.ar << idPlayer;
#ifndef __RT_1025
	m_Snapshot.ar << nJob;
	m_Snapshot.ar << (BYTE)dwSex;
#endif	// __RT_1025
	m_Snapshot.ar.WriteString( lpszPlayer );
	
}

void CUser::AddRemoveFriend( u_long uidSender )
{
	if( IsDelete() ) return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_REMOVEFRIEND;
	m_Snapshot.ar << uidSender;
	
}

void CUser::AddDuelRequest( u_long uidSrc, u_long uidDst )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_DUELREQUEST;
	m_Snapshot.ar << uidSrc << uidDst;
	
}

void CUser::AddDuelStart( u_long uidTarget, int bStart )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_DUELSTART;
	m_Snapshot.ar << uidTarget;
	m_Snapshot.ar << bStart;
	
}

void CUser::AddDuelCount( int nCount )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_DUELCOUNT;
	m_Snapshot.ar << nCount;
	
}

void CUser::AddDuelNo( OBJID idTarget )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_DUELNO;
	m_Snapshot.ar << idTarget;
	
}

void CUser::AddDuelCancel( OBJID idTarget )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_DUELCANCEL;
	m_Snapshot.ar << idTarget;
	
}
// 파티듀얼--------------------------------------------------------------------
void CUser::AddDuelPartyRequest( u_long uidSrc, u_long uidDst )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_DUELPARTYREQUEST;
	m_Snapshot.ar << uidSrc << uidDst;
	
}

void CUser::AddDuelPartyStart( LPCTSTR szPartyName, int nMax, OBJID *pMembers, u_long idParty, int bFlag )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_DUELPARTYSTART;
	m_Snapshot.ar << nMax;
	m_Snapshot.ar << idParty;
	m_Snapshot.ar.WriteString( szPartyName );
	for( int  i = 0; i < nMax; i ++ )
		m_Snapshot.ar << pMembers[i];
	m_Snapshot.ar << bFlag;
	
}

void CUser::AddDuelPartyNo( OBJID idTarget )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_DUELPARTYNO;
	m_Snapshot.ar << idTarget;
	
}

void CUser::AddDuelPartyCancel( CParty *pDuelOther )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_DUELPARTYCANCEL;

	if( pDuelOther )
	{
		int		i, nSize = pDuelOther->GetSizeofMember();	// 상대 파티멤버수
		m_Snapshot.ar << nSize;
		for( i = 0; i < nSize; i ++ )	
		{
			m_Snapshot.ar << pDuelOther->m_aMember[i].m_uPlayerId;		// 상대 파티원 유저아이디.
		}
	}
	else
	{
		m_Snapshot.ar << 0;
	}
	
}

void CUser::AddDuelPartyResult( CParty *pDuelOther, BOOL bWin )
{
	if( IsDelete() )	return;

	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_DUELPARTYRESULT;
	m_Snapshot.ar << bWin;

	int		i, nSize = pDuelOther->GetSizeofMember();	// 상대 파티멤버수
	m_Snapshot.ar << nSize;
	for( i = 0; i < nSize; i ++ )	
	{
		m_Snapshot.ar << pDuelOther->m_aMember[i].m_uPlayerId;		// 상대 파티원 유저아이디.
	}
	
	
}

#if __VER >= 11 // __SYS_PLAYER_DATA
void CUser::AddQueryPlayerData( u_long idPlayer, PlayerData* pPlayerData )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_QUERY_PLAYER_DATA;
	m_Snapshot.ar << idPlayer;
	m_Snapshot.ar.WriteString( pPlayerData->szPlayer );
	m_Snapshot.ar.Write( &pPlayerData->data, sizeof(sPlayerData) );
}

void CUser::AddPlayerData( void )
{
	vector<u_long>	aPlayer;
#ifdef __RT_1025
	for( map<u_long, Friend>::iterator i1 = m_RTMessenger.begin(); i1 != m_RTMessenger.end(); ++i1 )
		aPlayer.push_back( i1->first );
#else	// __RT_1025
	for( map<u_long, FRIEND*>::iterator i1 = m_Messenger.m_aFriend.begin(); i1 != m_Messenger.m_aFriend.end(); ++i1 )
	{
		if( i1->second )
			aPlayer.push_back( i1->second->dwUserId );
	}
#endif	//__RT_1025
	CGuild* pGuild	= GetGuild();
	if( pGuild )
	{
		for( map<u_long, CGuildMember*>::iterator i2 = pGuild->m_mapPMember.begin(); i2 != pGuild->m_mapPMember.end(); ++i2 )
			aPlayer.push_back( i2->second->m_idPlayer );
	}
	for( int i = 0; i < (int)( aPlayer.size() ); i++ )
	{
		PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->GetPlayerData( aPlayer[i] );
		if( pPlayerData )
			AddQueryPlayerData( aPlayer[i], pPlayerData );
	}
}
#else	// __SYS_PLAYER_DATA
void CUser::AddQueryPlayerString( u_long idPlayer, LPCSTR lpszPlayer, BYTE nQuery )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_QUERYPLAYERSTRING;
	m_Snapshot.ar << idPlayer;
	m_Snapshot.ar.WriteString( lpszPlayer );
	m_Snapshot.ar << nQuery;
	
}

void CUser::AddQueryPlayerListString( BYTE nQuery )
{
	if( IsDelete() )	return;
	
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_QUERYPLAYERLISTSTRING;
	m_Snapshot.ar << nQuery;
}

void CUser::AddFriendChangeJob( u_long uidPlayer, int nJob )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ADDFRIENDCHANGEJOB;
	m_Snapshot.ar << uidPlayer;
	m_Snapshot.ar << nJob;
}

void CUser::AddPartyMemberJob( int nMemberIndex, int nJob)
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYMEMBERJOB;
	m_Snapshot.ar << nMemberIndex;
	m_Snapshot.ar << nJob;
}

void CUser::AddPartyMemberLevel( int nMemberIndex, int nLevel)
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYMEMBERLEVEL;
	m_Snapshot.ar << nMemberIndex;
	m_Snapshot.ar << nLevel;
}

void CUser::AddChangeGuildJobLevel( u_long uidPlayer, LONG nJob, LONG nLevel )
{
	if( IsDelete() )	return;
	
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_GUILD_CHANGEJOBLEVEL;
	m_Snapshot.ar << uidPlayer;
	m_Snapshot.ar << nJob;	
	m_Snapshot.ar << nLevel;	
}
#endif	// __SYS_PLAYER_DATA

void CUser::AddGuildInvite( u_long idGuild, u_long idMaster )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_GUILD_INVITE;
	m_Snapshot.ar << idGuild;
	m_Snapshot.ar << idMaster;
	
}

void CUser::AddAllGuilds( void )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_ALL_GUILDS;
	g_GuildMng.Serialize( m_Snapshot.ar, TRUE );
	
}

void CUser::AddMyGuild( void )
{
	if( IsDelete() )	return;

	CGuild* pGuild	= GetGuild();
	if( !pGuild )
		return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_GUILD;
	m_Snapshot.ar << pGuild->m_idGuild;
	pGuild->Serialize( m_Snapshot.ar, FALSE );
}

void CUser::AddMyGuildWar( void )
{
	if( IsDelete() )	return;

	CGuildWar* pWar	= GetWar();
	if( !pWar )
		return;

	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_WAR;
	m_Snapshot.ar << pWar->m_idWar;
	pWar->Serialize( m_Snapshot.ar );
	
}

void CUser::AddContribution( CONTRIBUTION_CHANGED_INFO& info )
{
	if( IsDelete() )	return;

	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_GUILD_CONTRIBUTION;
	m_Snapshot.ar << info;
	
}

void CUser::AddInsertedVote( VOTE_INSERTED_INFO& info )
{
	if( IsDelete() )	return;

	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_GUILD_ADDVOTE;
	m_Snapshot.ar << info;
	
}

void CUser::AddModifyVote( u_long idVote, BYTE cbOperation, BYTE cbExtra )
{
	if( IsDelete() )	return;

	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_GUILD_MODIFYVOTE;
	m_Snapshot.ar << idVote;
	m_Snapshot.ar << cbOperation;
	m_Snapshot.ar << cbExtra;
	
}

void CUser::AddSetNotice( u_long idGuild, const char* szNotice )
{
	if( IsDelete() )	return;

	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_GUILD_NOTICE;
	m_Snapshot.ar << idGuild;
	m_Snapshot.ar.WriteString( szNotice );
	
}

void CUser::AddSetGuildAuthority( DWORD dwAuthority[] )
{
	if( IsDelete() )	return;
	
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_GUILD_AUTHORITY;
	m_Snapshot.ar.Write( dwAuthority, sizeof(DWORD) * MAX_GM_LEVEL );
	
}

void CUser::AddSetGuildPenya( DWORD dwType, DWORD dwPenya )
{
	if( IsDelete() )	return;
	
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_GUILD_PENYA;
	m_Snapshot.ar << dwType << dwPenya;	
	
}

void CUser::AddGuildRealPenya( int nGoldGuild, int nType )
{
	if( IsDelete() )	return;
	
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_GUILD_REAL_PENYA;
	m_Snapshot.ar << nGoldGuild;	
	m_Snapshot.ar << nType;	
	
}

void CUser::SendGuildRank()
{
	if( IsDelete() )	return;
	
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_REQUEST_GUILDRANK;
	CGuildRank::Instance()->Serialize( m_Snapshot.ar );
	
}

void CUser::AddSMMode( int nType, DWORD dwTime )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_SM_MODE;

	DWORD BufSMTime = 0;
	if( dwTime != 0 )
	{
		if( g_AddSMMode.bSMModetime[nType] )
		{
			CTime tSMtime( m_dwSMTime[nType] );
			CTimeSpan ct = tSMtime - CTime::GetCurrentTime();
			BufSMTime = (DWORD)( ct.GetTotalSeconds() );
		}
		else
		{
			BufSMTime = m_dwSMTime[nType];
		}
	}
	else
	{
		BufSMTime = 0;
	}

	m_Snapshot.ar << nType;
	m_Snapshot.ar << BufSMTime;
	
	if( nType < 0 || SM_MAX <= nType )
	{
		WriteLog( "SMMODE OVERFLOW nType : %d", nType );
	}
	
}

void CUser::AddSMModeAll()
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_SM_MODE_ALL;

	u_long uOffset	= m_Snapshot.ar.GetOffset();
	m_Snapshot.ar << (short)0;
	short nReal = 0;

	for( int i = 0 ; i < SM_MAX ; ++i )
	{
		DWORD BufSMTime = 0;
		if( 0 < m_dwSMTime[i] )
		{
			if( g_AddSMMode.bSMModetime[i] )
			{
				CTime tSMtime( m_dwSMTime[i] );
				CTimeSpan ct = tSMtime - CTime::GetCurrentTime();
				BufSMTime = (DWORD)( ct.GetTotalSeconds() );
			}
			else
			{
				BufSMTime = m_dwSMTime[i];
			}
			m_Snapshot.ar << i;
			m_Snapshot.ar << BufSMTime;
			++nReal;
		}
	}
	
	GETBLOCK( m_Snapshot.ar, lpBlock, nBlockSize );
	*(UNALIGNED short*)( lpBlock + uOffset ) = nReal;

	
}

void CUser::AddResistSMMode( BYTE nAttackResistLeft, BYTE nAttackResistRight, BYTE nDefenceResist )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_RESISTSMMODE;
	m_Snapshot.ar << nAttackResistLeft;
	m_Snapshot.ar << nAttackResistRight;
	m_Snapshot.ar << nDefenceResist;
	
}

void CUser::AddCommercialElem( DWORD dwItemId, int nResistSMItemId )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_COMMERCIALELEM;
	m_Snapshot.ar << dwItemId;
	m_Snapshot.ar << nResistSMItemId;
	
}

void CUser::AddFlyffEvent( void )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_FLYFF_EVENT;
	g_eLocal.Serialize( m_Snapshot.ar );
	
}

#if __VER >= 9 // __EVENTLUA
void CUser::AddEventLuaDesc( int nState, string strDesc )
{
	if( strDesc.length() == 0 ) return;
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_EVENTLUA_DESC;
	m_Snapshot.ar << nState;
	m_Snapshot.ar.WriteString( strDesc.c_str() );	
}
#endif // __EVENTLUA

void CUser::AddGoldText( int nPlus )
{
	char szPlus[64];
	char szGold[64];

	sprintf( szPlus, "%d", nPlus );
	sprintf( szGold, "%d", GetGold() );

	CString strPlus = GetNumberFormatEx( szPlus );
	CString strGold = GetNumberFormatEx( szGold );

	AddDefinedText( TID_GAME_REAPMONEY, "%s %s", strPlus, strGold );
}

void CUser::AddRemoveGuildBankItem( u_long idGuild, DWORD dwId, DWORD dwItemNum )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_REMOVE_GUILD_BANK_ITEM;
	m_Snapshot.ar << idGuild << dwId << dwItemNum;
	
}

void CUser::AddDefinedText( int dwText, LPCSTR lpszFormat, ... )
{
	if( IsDelete() )	return;

	TCHAR szBuffer[1024];

	va_list args;
	va_start( args, lpszFormat );
	int nBuf = _vsntprintf( szBuffer, sizeof(szBuffer)-1, lpszFormat, args );
	va_end( args );

	if( nBuf < 0 )
		return;

	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_DEFINEDTEXT;
	m_Snapshot.ar << dwText;
	m_Snapshot.ar.WriteString( szBuffer );
	
}

void CUser::AddChatText( int dwText, LPCSTR lpszFormat, ... )
{
	if( IsDelete() )	return;
	
	TCHAR szBuffer[1024];
	
	va_list args;
	va_start( args, lpszFormat );
	int nBuf = _vsntprintf( szBuffer, sizeof(szBuffer)-1, lpszFormat, args );
	va_end( args );
	
	if( nBuf < 0 )
		return;
	
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CHATTEXT;
	m_Snapshot.ar << dwText;
	m_Snapshot.ar.WriteString( szBuffer );
	
}

void CUser::AddDefinedText( int dwText )
{
	if( IsDelete() )	return;
	
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_DEFINEDTEXT1;
	m_Snapshot.ar << dwText;
	
}

void CUser::AddExpBoxInfo( OBJID objid, DWORD dwSet, DWORD dwTime, u_long idPlayer )
{
	if( IsDelete() )	return;
	
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_EXPBOXINFO;
	m_Snapshot.ar << objid;
	m_Snapshot.ar << dwSet;
	m_Snapshot.ar << dwTime;
	m_Snapshot.ar << idPlayer;
	
}

void CUser::AddDefinedCaption( BOOL bSmall, int dwText, LPCSTR lpszFormat, ... )
{
	if( IsDelete() )	return;
	
	TCHAR szBuffer[1024];
	
	va_list args;
	va_start( args, lpszFormat );
	int nBuf = _vsntprintf( szBuffer, sizeof(szBuffer)-1, lpszFormat, args );
	va_end( args );
	
	if( nBuf < 0 )
		return;
	
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_DEFINEDCAPTION;
	m_Snapshot.ar << bSmall;
	m_Snapshot.ar << dwText;
	m_Snapshot.ar.WriteString( szBuffer );
	
}

void CUser::AddSetGuildQuest( int nQuestId, int nState )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SETGUILDQUEST;
	m_Snapshot.ar << nQuestId << nState;
	
}

void CUser::AddRemoveGuildQuest( int nQuestId )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_REMOVEGUILDQUEST;
	m_Snapshot.ar << nQuestId;
	
}

void CUser::AddSetQuest( LPQUEST lpQuest )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SETQUEST;
	m_Snapshot.ar.Write( lpQuest, sizeof( QUEST ) );//ar << nQuestIdx << nState;
	
}

void CUser::AddQueryGetDestObj( OBJID idFrom )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_QUERYGETDESTOBJ;
	m_Snapshot.ar << idFrom;
	
}

void CUser::AddGetDestObj( OBJID objid, OBJID objidDest, FLOAT fRange )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << objid;
	m_Snapshot.ar << SNAPSHOTTYPE_GETDESTOBJ;
	m_Snapshot.ar << objidDest;
	m_Snapshot.ar << fRange;

	
}

void CUser::AddQueryGetPos( OBJID idFrom )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_QUERYGETPOS;
	m_Snapshot.ar << idFrom;
	
}

void CUser::AddGetPos( OBJID objid, const D3DXVECTOR3 & vPos, float fAngle )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << objid;
	m_Snapshot.ar << SNAPSHOTTYPE_GETPOS;
	m_Snapshot.ar << vPos << fAngle;
	
}

void CUser::AddResurrectionMessage()
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_RESURRECTION_MESSAGE;
	
}

// 디버깅용으로 쓰인다.  선택된 무버의 좌표등의 값들을 클라로 보낸다.
void CUser::AddCorrReq( CMover *pMover )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << pMover->GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CORRREQ;
	m_Snapshot.ar << pMover->GetPos();
	m_Snapshot.ar << pMover->m_idAttacker;
	
}

void CUser::AddSetFuel( int nFuel, DWORD tmAccFuel )
{
	if( IsDelete() )	return;
	
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SETFUEL;
	m_Snapshot.ar << nFuel << tmAccFuel;
	
}

void CUser::AddSetState( LONG nStr, LONG nSta, LONG nDex, LONG nInt, LONG nGP )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SETSTATE;
	m_Snapshot.ar << nStr << nSta << nDex << nInt;
	m_Snapshot.ar << (LONG)0 << nGP;
	
}

void CUser::AddSetSkill( DWORD dwSkill, DWORD dwSkillLevel )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CMDSETSKILLLEVEL;
	m_Snapshot.ar << dwSkill << dwSkillLevel;
	
}

void CUser::AddMotionError( void )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_MOTION;
	m_Snapshot.ar << (DWORD)OBJMSG_NONE;
	
}




// 클라이언트에게 쪽지를 보낸다.
void CUser::AddTag( short nTagCount, const TAG_ENTRY* tags )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_TAG;

	u_long uOffset	= m_Snapshot.ar.GetOffset();
	m_Snapshot.ar << (short)0;
	
	short nReal = 0;
	for( short i=0; i<nTagCount; ++i )
	{
#if __VER >= 11 // __SYS_PLAYER_DATA
		LPCTSTR lpszName = CPlayerDataCenter::GetInstance()->GetPlayerString( tags[i].idFrom );
#else	// __SYS_PLAYER_DATA
		LPCTSTR lpszName = prj.GetPlayerString( tags[i].idFrom );
#endif	// __SYS_PLAYER_DATA
		if( lpszName )
		{
			TRACE("TAG:%s %d %s\n", lpszName, tags[i].dwDate, tags[i].szString);

			m_Snapshot.ar.WriteString( lpszName );				// 누구에게서 
			m_Snapshot.ar << tags[i].dwDate;					// 언제 
			m_Snapshot.ar.WriteString( tags[i].szString );		// 쪽지 내용 

			nReal++;
		}
	}
	GETBLOCK( m_Snapshot.ar, lpBlock, nBlockSize );
	*(UNALIGNED short*)( lpBlock + uOffset ) = nReal;

	
}


// 버프스킬 적용상태를 클라에 전송
void CUser::AddSetSkillState( CMover *pMover, DWORD dwSkill, DWORD dwLevel, DWORD dwTime )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << pMover->GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SETSKILLSTATE;
	m_Snapshot.ar << dwSkill;
	m_Snapshot.ar << dwLevel;
	m_Snapshot.ar << dwTime;
}

void CUser::AddPlayMusic( u_long idMusic )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_PLAYMUSIC;
	m_Snapshot.ar << idMusic;
	
}

void CUser::AddPlaySound( u_long idSound )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_PLAYSOUND;
	m_Snapshot.ar << (BYTE)0;
	m_Snapshot.ar << idSound;
	
}

void CUser::AddPlaySound2( const char* szSound )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_PLAYSOUND;
	m_Snapshot.ar << (BYTE)1;
	m_Snapshot.ar.WriteString( szSound );
	
}

void CUser::AddMoverFocus( CMover *pMover )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_MOVERFOCUS;
	m_Snapshot.ar << pMover->m_idPlayer;
	m_Snapshot.ar << pMover->GetGold();
	m_Snapshot.ar << pMover->GetExp1();
	
}

void CUser::AddPartyMapInfo( int nIndex, D3DXVECTOR3 vPos )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYMAPINFO;
	m_Snapshot.ar << nIndex;
	m_Snapshot.ar << vPos;
	
}

void CUser::AddSetNaviPoint( const NaviPoint & nv, OBJID objid, const char* Name )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << objid;
	m_Snapshot.ar << SNAPSHOTTYPE_SETNAVIPOINT;
	m_Snapshot.ar << nv.Pos;// << nv.On;
	m_Snapshot.ar.WriteString( Name );
	
}

void CUser::AddSetDuel( CMover* pMover )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SETDUEL;
	m_Snapshot.ar << pMover->GetId() << pMover->m_nDuel << pMover->m_nDuelState << pMover->m_idDuelOther << pMover->m_idDuelParty;
	
}

#if __VER >= 8 // __S8_PK
void CUser::AddPKValue()
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PK_RELATION;
	m_Snapshot.ar << PK_PKVALUE;
	m_Snapshot.ar << GetPKValue();
}
#else // __VER >= 8 // __S8_PK
void CUser::UpdatePlayerEnemy( BYTE byType, OBJID objid )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_UPDATE_PLAYER_ENEMY;
	m_Snapshot.ar << byType;
	m_Snapshot.ar << objid;
}
#endif // __VER >= 8 // __S8_PK


void CUser::ScheduleDestory( DWORD dwDestoryTime  )
{
	m_dwDestroyTime = dwDestoryTime;
}

void CUser::AddSnoop( const char* lpString )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_SNOOP;
	m_Snapshot.ar.WriteString( lpString );
	
}

void CUser::AddSetCheerParam( int nCheerPoint, DWORD dwRest, BOOL bAdd )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GETID( this );
	m_Snapshot.ar << SNAPSHOTTYPE_SETCHEERPARAM;
	m_Snapshot.ar << nCheerPoint << dwRest;
	m_Snapshot.ar << bAdd; // chipi_080412
	
}

void CUser::AddQueryEquip( CUser* pUser )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GETID( pUser );
	m_Snapshot.ar << SNAPSHOTTYPE_QUERYEQUIP;

	u_long uOffset	= m_Snapshot.ar.GetOffset();
	int cbEquip		= 0;
	
	m_Snapshot.ar << cbEquip;
	
	for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
	{
		CItemElem* pItemElem	= pUser->GetEquipItem( i );
		if( pItemElem )
		{
			m_Snapshot.ar << i;
			m_Snapshot.ar << pItemElem->GetRandomOptItemId();
//			m_Snapshot.ar.Write( &pItemElem->m_piercingInfo, sizeof(PIERCINGINFO) );
			pItemElem->SerializePiercing( m_Snapshot.ar );
			m_Snapshot.ar << pItemElem->m_bItemResist;
			m_Snapshot.ar << pItemElem->m_nResistAbilityOption;
			cbEquip++;
		}
	}

	GETBLOCK( m_Snapshot.ar, lpBlock, nBlockSize );
	*(UNALIGNED int*)( lpBlock + uOffset )	= cbEquip;

	
}

void CUser::AddSummonFriendUse( CItemElem* pItemElem )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GETID( this );
	m_Snapshot.ar << SNAPSHOTTYPE_SUMMON;
	m_Snapshot.ar << SOMMON_FRIEND;
	m_Snapshot.ar << MAKELONG( ITYPE_ITEM, pItemElem->m_dwObjId );
	m_Snapshot.ar << pItemElem->m_dwObjId;
	
}
void CUser::AddSummonFriendConfirm( OBJID objid, DWORD dwData, const char* szName, const char* szWorldName )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GETID( this );
	m_Snapshot.ar << SNAPSHOTTYPE_SUMMON;
	m_Snapshot.ar << SOMMON_FRIEND_CONFIRM;
	m_Snapshot.ar << objid << dwData;
	m_Snapshot.ar.WriteString( szName );
	m_Snapshot.ar.WriteString( szWorldName );
	
}
void CUser::AddSummonPartyConfirm( OBJID objid, DWORD dwData, const char * szWorldName )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GETID( this );
	m_Snapshot.ar << SNAPSHOTTYPE_SUMMON;
	m_Snapshot.ar << SOMMON_PARTY_CONFIRM;
	m_Snapshot.ar << objid << dwData;
	m_Snapshot.ar.WriteString( szWorldName );
	
}

void CUser::SetReturnPos( const D3DXVECTOR3& vPos )
{
	ASSERT( GetWorld() );

	m_dwReturnWorldID = GetWorld()->GetID();
	m_vReturnPos = vPos;
	TRACE("SetReturnPos: %f %f %f\n", m_vReturnPos.x, m_vReturnPos.y, m_vReturnPos.z );
}

D3DXVECTOR3& CUser::GetReturnPos( DWORD* pdwWorldID )
{
	*pdwWorldID = m_dwReturnWorldID;
	return m_vReturnPos;
}

// pItemElem을 사용하는데 과련 정보는 PACKITEMELEM을 참조한다.
// pItemElem은 사용한 아이템 
void CUser::DoUsePackItem( CItemElem* pItemElem, PPACKITEMELEM pPackItemElem )
{
	time_t t	= 0;
	if( pPackItemElem->nSpan )	// minutes
	{
		CTime time	= CTime::GetCurrentTime() + CTimeSpan( 0, 0, pPackItemElem->nSpan, 0 );
		t	= (time_t)( time.GetTime() );
	}

	if( m_Inventory.GetEmptyCount() >= pPackItemElem->nSize )
	{
		for( int i = 0; i < pPackItemElem->nSize; i++ )
		{
			CItemElem itemElem;
			itemElem.m_dwItemId	= pPackItemElem->adwItem[i];
			itemElem.SetAbilityOption( pPackItemElem->anAbilityOption[i] );
			itemElem.m_nItemNum	= pPackItemElem->anNum[i];
			itemElem.m_bCharged		= itemElem.GetProp()->bCharged;
			itemElem.m_dwKeepTime	= (DWORD)t;

//			if( pItemElem->IsFlag( CItemElem::binds ) )
			if( pItemElem->IsBinds() )
				itemElem.SetFlag( CItemElem::binds );

			if( CreateItem( &itemElem ) )
			{
				AddDefinedText( TID_GAME_REAPITEM, "\"%s\"", itemElem.GetProp()->szName );
				g_DPSrvr.PutCreateItemLog( this, &itemElem , "E", "PACK" );
//				ItemProp* pItemProp		= itemElem.GetProp();
//				if( pItemProp->dwSfxObj3 != -1 )
//					g_UserMng.AddCreateSfxObj( this, pItemProp->dwSfxObj3, GetPos().x, GetPos().y, GetPos().z );
			}
			else
			{
				// critical err
			}
		}
		OnAfterUseItem( pItemElem->GetProp() );
		UpdateItem( (BYTE)( pItemElem->m_dwObjId ), UI_NUM, pItemElem->m_nItemNum - 1 );
	}
	else
	{
		AddDefinedText( TID_GAME_LACKSPACE );			
	}
}

BOOL CUser::DoUseGiftbox( CItemElem* pItemElem, DWORD dwItemId )
{
//	DWORD dwItem;
//	int nNum;
//	BYTE nFlag;
	GIFTBOXRESULT	result;
	/*
#ifdef __GIFTBOX0213
	if( CGiftboxMan::GetInstance()->Open( dwItemId, &dwItem, &nNum, m_idPlayer, pItemElem ) )
#else	// __GIFTBOX0213
	*/
	if( CGiftboxMan::GetInstance()->Open( dwItemId, &result ) )
//#endif	// __GIFTBOX0213
	{
		if( m_Inventory.GetEmptyCount() < 1 )
		{
			AddDefinedText( TID_GAME_LACKSPACE );
			return TRUE;
		}
		if( pItemElem )
		{
			OnAfterUseItem( pItemElem->GetProp() );
			UpdateItem( (BYTE)( pItemElem->m_dwObjId ), UI_NUM, pItemElem->m_nItemNum - 1 );
		}

		CItemElem itemElem;
		itemElem.m_dwItemId	= result.dwItem;
		itemElem.m_nItemNum	= result.nNum;
		if( result.nFlag != 4 )	// ignore property
		{
			itemElem.m_byFlag	= result.nFlag;
			itemElem.m_bCharged		= itemElem.GetProp()->bCharged;
		}

		time_t t	= 0;
		if( result.nSpan )	// minutes
		{
			CTime time	= CTime::GetCurrentTime() + CTimeSpan( 0, 0, result.nSpan, 0 );
			t	= (time_t)( time.GetTime() );
		}
		itemElem.m_dwKeepTime	= t;
		itemElem.SetAbilityOption( result.nAbilityOption );

		if( CreateItem( &itemElem ) )
		{
			AddDefinedText( TID_GAME_REAPITEM, "\"%s\"", itemElem.GetProp()->szName );						
			LogItemInfo aLogItem;
			aLogItem.Action = "E";
			aLogItem.SendName = GetName();
			aLogItem.RecvName = "GIFTBOX";
			aLogItem.WorldId = GetWorld()->GetID();
			aLogItem.Gold = aLogItem.Gold2 = GetGold();
			g_DPSrvr.OnLogItem( aLogItem, &itemElem, result.nNum );
		}
		
		return TRUE;
	}

	return FALSE;
}


void CUser::DoUseEveFBOX( CItemElem* pItemElem ) 
{
	DWORD dwIdx;
	if( pItemElem )
	{
		OnAfterUseItem( pItemElem->GetProp() );
		UpdateItem( (BYTE)( pItemElem->m_dwObjId ), UI_NUM, pItemElem->m_nItemNum - 1 );
	}

	int nRand	= xRandom( 1, 1000 );
	if( nRand > 470 )   // 53%
		dwIdx    = II_GEN_FOO_INS_CHOCOLATE;
	else if( nRand > 130 )         // 34%
		dwIdx    = II_CHR_MAG_TRI_HEARTBOMB;
	else if( nRand > 108 )         // 2.2%
		dwIdx    = II_ARM_S_CLO_SHO_WARMSLEEPER;
	else if( nRand > 86 )         // 2.2%
		dwIdx    = II_ARM_S_CLO_HAT_MUSHROOM;
	else if( nRand > 71 )         // 1.5%
		dwIdx    = II_ARM_S_CLO_HAT_STATHAT;
	else if( nRand > 66 )   // 0.5%
		dwIdx    = II_WEA_SWO_HARTBONG;
	else if( nRand > 56 )         // 1%
		dwIdx    = II_ARM_M_CLO_KOREAN01SUIT;
	else if( nRand > 41 )          // 1.5%
		dwIdx    = II_ARM_M_CLO_KOREAN01SHOES;
	else if( nRand > 31 )          // 1%
		dwIdx    = II_ARM_F_CLO_KOREAN01SUIT;
	else if( nRand > 16 )          // 1.5%
		dwIdx    = II_ARM_F_CLO_KOREAN01SHOES;
	else if( nRand > 6 )          // 1%
		dwIdx    = II_SYS_SYS_EVE_WINGS;
	else if( nRand > 5 )          // 0.1%
		dwIdx    = II_WEA_KNU_ISHOFIST;
	else	// 0.5%
		dwIdx    = II_ARM_F_CLO_KOREAN01HAT;

	CItemElem itemElem;
	itemElem.m_dwItemId	= dwIdx;
	itemElem.m_nItemNum	= 1;
	if( CreateItem( &itemElem ) )
	{
		LogItemInfo aLogItem;
		aLogItem.Action = "E";
		aLogItem.SendName = GetName();
		aLogItem.RecvName = "GIFTBOX";
		aLogItem.WorldId = GetWorld()->GetID();
		aLogItem.Gold = aLogItem.Gold2 = GetGold();
		g_DPSrvr.OnLogItem( aLogItem, &itemElem, 1 );
	}
	else
	{
		WriteError( "GIFTBOX//0//%s", GetName() );
	}
}

// 아이템을 사용할 수 있는 상태입니까?
BOOL CUser::IsUsableState( DWORD dwId )
{
	if( m_Inventory.m_apItem == NULL )
	{
		WriteError( "ITEMCONTAINER//1" );
		return FALSE;
	}

	if( m_Inventory.IsEquip( dwId ) == FALSE )
	{
		// 장비가 장착되어 있지 않은 상태에서는 거래상태를 체크한다.
		// 장비가 장착되어 있는 상태에서는 거래중에도 벗을 수 있어야 하기 때문에 검사하지 않는다. 
		if( m_vtInfo.GetOtherID() != NULL_ID )
		{
			AddDefinedText( TID_GAME_TRADELIMITUSING, "" );
			return FALSE;
		}
		if( m_vtInfo.VendorIsVendor() )
		{
			return FALSE;
		}
	}

#ifdef __S_SERVER_UNIFY
	if( m_bAllAction == FALSE )
		return FALSE;
#endif // __S_SERVER_UNIFY

	return TRUE;
}

void CUser::OnDoUseItem( DWORD dwData, OBJID objid, int nPart )
{
	DWORD dwItemId;
	DWORD dwId = HIWORD( dwData );

	if( IsUsableState( dwId ) == FALSE )
		return;

#ifdef __QUIZ
	if( GetWorld() && GetWorld()->GetID() == WI_WORLD_QUIZ )
		return;
#endif // __QUIZ

	CItemElem* pItemElem = m_Inventory.GetAtId( dwId );
	if( IsUsableItem( pItemElem ) )
	{
		// 기간 만료
		ItemProp* pItemProp	= pItemElem->GetProp();
		if( pItemProp && pItemProp->dwParts == NULL_ID && pItemElem->IsFlag( CItemElem::expired ) )
		{
#if __VER >= 9	// __PET_0410
			if(pItemProp->dwItemKind3 == IK3_EGG)
				AddDefinedText( TID_GAME_PET_DEAD );
			else
				AddDefinedText( TID_GAME_ITEM_EXPIRED );
#else
			AddDefinedText( TID_GAME_ITEM_EXPIRED );
#endif //__PET_0410
			return;
		}
#if __VER >= 15 // __PETVIS
		if( pItemProp )
		{
			if( pItemProp->dwItemKind3 == IK3_VIS )
			{
				CItemUpgrade::GetInstance()->SetPetVisItem( this, pItemElem->m_dwObjId );
				return;
			}
			else if( pItemProp->dwID == II_SYS_SYS_VIS_KEY01 )
			{
				CItemUpgrade::GetInstance()->PetVisSize( this, pItemElem->m_dwObjId );
				return;
			}
			else if( pItemProp->dwID == II_SYS_SYS_SCR_PET_MAGIC )
			{
				CItemUpgrade::GetInstance()->TransFormVisPet( this, pItemElem->m_dwObjId );
				return;
			}
		}			
#endif // __PETVIS

		dwItemId = pItemElem->m_dwItemId;
	
		PPACKITEMELEM pPackItemElem	= CPackItem::GetInstance()->Open( dwItemId );
		if( pPackItemElem )
		{
			DoUsePackItem( pItemElem, pPackItemElem );
			return;
		}
	}
	else
	{
		pItemElem = NULL;
		dwItemId = 0;
	}

	DoUseItem( dwData, objid, nPart );

	if( DoUseGiftbox( pItemElem, dwItemId ) )
		return;

	switch( dwItemId )
	{
	case II_SYS_SYS_EVE_FBOX:
		DoUseEveFBOX( pItemElem );
		break;
	}
}

#if __VER >= 9	// __PET_0410
void CUser::AddPetState( DWORD dwPetId, WORD wLife, WORD wEnergy, DWORD dwExp )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GETID( this );
	m_Snapshot.ar << SNAPSHOTTYPE_PET_STATE;
	m_Snapshot.ar << dwPetId << wLife << wEnergy << dwExp;
}

void	CUser::AddPetSetExp( DWORD dwExp )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GETID( this );
	m_Snapshot.ar << SNAPSHOTTYPE_PET_SET_EXP;
	m_Snapshot.ar << dwExp;
}

void CUser::AddPet( CPet* pPet, BYTE nPetLevelup )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GETID( this );
	m_Snapshot.ar << SNAPSHOTTYPE_PET;
	pPet->Serialize( m_Snapshot.ar );
	m_Snapshot.ar << nPetLevelup;
}

void CUser::AddPetFoodMill(int nResult, int nCount)
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GETID( this );
	m_Snapshot.ar << SNAPSHOTTYPE_PET_MILL;
	m_Snapshot.ar << nResult;
	m_Snapshot.ar << nCount;
}
#endif	// __PET_0410

#if __VER >= 11 // __SYS_COLLECTING
void	CUser::ProcessCollecting( void )
{
	CItemElem* pCol		= GetCollector();
	if( pCol == NULL )	
	{
		// 1. 통상 상태
		if( m_pActMover->GetActionState() == OBJSTA_COLLECT )
			StopCollecting();
	}
	else if( IsDisguise() )	
	{
		if( m_pActMover->GetActionState() == OBJSTA_COLLECT )
			StopCollecting();
	}
	else if( m_pActMover->GetActionState() == OBJSTA_COLLECT )
	{
		// 3. 채집 상태
		// 3.1. 배터리 소모
		if( !HasBuff( BUFF_ITEM2, II_GEN_TOO_COL_SILVERBATTERY )
			&& !HasBuff( BUFF_ITEM2, II_GEN_TOO_COL_GOLDBATTERY ) 
			&& !HasBuff( BUFF_ITEM2, II_GEN_TOO_COL_BATTERY001 ) )
		{
			if( --pCol->m_nHitPoint < 0 )
				pCol->m_nHitPoint	= 0;

			UpdateItem( (BYTE)( pCol->m_dwObjId ), UI_HP, pCol->m_nHitPoint );

			if( pCol->m_nHitPoint == 0 )
			{
				StopCollecting();
				return;
			}
		}

		// 3.2. 채집
		CCollectingProperty* pProperty	= CCollectingProperty::GetInstance();
		if( ++m_nCollecting >= pProperty->GetCool( pCol->GetAbilityOption() ) )
		{
			m_nCollecting		= 0;

			DWORD dwItemId	= pProperty->GetItem();
			if( dwItemId == 0 )	
				return;
			CItemElem itemElem;
			itemElem.m_dwItemId	= dwItemId;
			itemElem.m_nItemNum		= 1;
			itemElem.SetSerialNumber();
			if( CreateItem( &itemElem ) == TRUE )
			{
//				ItemProp* pItemProp		= itemElem.GetProp();
//				if( pItemProp )
//					AddDefinedText( TID_GAME_REAPITEM, "\"%s\"", pItemProp->szName );
				AddRestartCollecting( dwItemId );
				// log
				LogItemInfo	log;
				log.Action	= "c";
				log.SendName	= GetName();
				log.RecvName	= "CreateItem";
				log.WorldId		= GetWorld()->GetID();
				log.Gold	= GetGold();
				log.Gold2	= GetGold();
				g_DPSrvr.OnLogItem( log, &itemElem, itemElem.m_nItemNum );
			}
		}
		//
	}
	else
	{
		// 2. 채집 대기 상태
	}
}

void CUser::AddRestartCollecting( DWORD dwItemId )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GETID( this );
	m_Snapshot.ar << SNAPSHOTTYPE_RESTART_COLLECTING;
	m_Snapshot.ar << dwItemId;
}

void CUser::StartCollecting( void )
{
//#ifdef _DEBUG
//	if( !IsRegionAttr( RA_SAFETY ) )
//#else	// _DEBUG
	if( !IsRegionAttr( RA_COLLECTING ) )
//#endif	// _DEBUG
	{
		AddDefinedText( TID_GAME_COLLECTING_REGION );		
		return;
	}
	// state
	if( GetCollector() == NULL )
		return;
	CMover::StartCollecting();
	g_UserMng.AddStartCollecting( this );
	// log
	LogItemInfo	log;
	log.Action	= "c";
	log.SendName	= GetName();
	log.RecvName	= "StartCollecting";
	log.WorldId		= GetWorld()->GetID();
	log.Gold	= GetGold();
	log.Gold2	= GetGold();
	log.Gold_1	= GetCollector()->m_nHitPoint;
	g_DPSrvr.OnLogItem( log );
#if __VER >= 13 // __HONORABLE_TITLE			// 달인
	m_dwHonorCheckTime = GetTickCount();
#endif	// __HONORABLE_TITLE			// 달인
}

void CUser::StopCollecting( void )
{
	// state
	CMover::StopCollecting();
	m_nCollecting	= 0;
	g_UserMng.AddStopCollecting( this );
	// log
	LogItemInfo	log;
	log.Action	= "c";
	log.SendName	= GetName();
	log.RecvName	= "StopCollecting";
	log.WorldId		= GetWorld()->GetID();
	g_DPSrvr.OnLogItem( log );
}

BOOL CUser::DoUseItemBattery( void )
{
	CItemElem* pCol		= GetCollector();
	if( pCol == NULL || m_pActMover->GetActionState() == OBJSTA_COLLECT )
		return FALSE;
	UpdateItem( (BYTE)( pCol->m_dwObjId ), UI_HP, CCollectingProperty::GetInstance()->GetMaxBattery() );

	return TRUE;
}

#if __VER < 14 // __SMELT_SAFETY
// called by CDPSrvr::OnEnchant()
BOOL CUser::PreRefine( DWORD dwTarget, DWORD dwMaterial )
{
	CItemElem* pMaterial	= m_Inventory.GetAtId( dwMaterial );
	// 재료가 유효한가?
	if( !IsUsableItem( pMaterial ) )
		return TRUE;
	CItemElem* pTarget	= m_Inventory.GetAtId( dwTarget );
	// 대상이 유효한가?
	if( !IsUsableItem( pTarget ) )
		return TRUE;
	// 대상이 장착중인가?
	if( m_Inventory.IsEquip( dwTarget ) )
	{
		AddDefinedText( TID_GAME_EQUIPPUT , "" );
		return TRUE;
	}

	// 대상이 채집기인가?
	if( pTarget->IsCollector( TRUE ) )
	{
		RefineCollector( pTarget, pMaterial );
		return TRUE;
	}
	if( pTarget->IsAccessory() )
	{
		RefineAccessory( pTarget, pMaterial );
		return TRUE;
	}

	return FALSE;
}

void	CUser::RefineCollector( CItemElem* pTarget, CItemElem* pMaterial )
{
	// 재료가 문스톤인가?
	if( pMaterial->m_dwItemId != II_GEN_MAT_MOONSTONE && pMaterial->m_dwItemId != II_GEN_MAT_MOONSTONE_1 )
	{
		AddDefinedText( TID_GAME_NOTEQUALITEM , "" );
		return;
	}
	CCollectingProperty* pProperty	= CCollectingProperty::GetInstance();
	if( pTarget->GetAbilityOption() >= pProperty->GetMaxCollectorLevel() )
	{
		AddDefinedText( TID_GAME_MAX_COLLECTOR_LEVEL, "" );
		return;
	}
	int nProb	= pProperty->GetEnchantProbability( pTarget->GetAbilityOption() );
	if( nProb == 0 )
		return;

	// log
	LogItemInfo lii;
	lii.SendName	= GetName();
	lii.RecvName	= "UPGRADEITEM";
	lii.WorldId	= GetWorld()->GetID();
	lii.Gold	= GetGold();
	lii.Gold2	= GetGold();

	DWORD dwRand	= xRandom( 1000 );	// 0 - 999
	if( dwRand < nProb )
	{
		AddDefinedText( TID_UPGRADE_SUCCEEFUL );
		AddPlaySound( SND_INF_UPGRADESUCCESS );
		if( IsMode( TRANSPARENT_MODE ) == 0 )
			g_UserMng.AddCreateSfxObj( this, XI_INT_SUCCESS, GetPos().x, GetPos().y, GetPos().z );
		UpdateItem( pTarget->m_dwObjId, UI_AO,  pTarget->GetAbilityOption()+1 );
		lii.Action		= "H";
		g_DPSrvr.OnLogItem( lii, pTarget, pTarget->m_nItemNum );
	}
	else
	{
		AddDefinedText( TID_UPGRADE_FAIL );
		AddPlaySound( SND_INF_UPGRADEFAIL );
		if( IsMode( TRANSPARENT_MODE ) == 0 )
			g_UserMng.AddCreateSfxObj( this, XI_INT_FAIL, GetPos().x, GetPos().y, GetPos().z );
		// log
	}
	lii.Action	= "N";
	g_DPSrvr.OnLogItem( lii, pMaterial, pMaterial->m_nItemNum );
	UpdateItem( pMaterial->m_dwObjId, UI_NUM, pMaterial->m_nItemNum - 1 );	
}

void CUser::RefineAccessory( CItemElem* pTarget, CItemElem* pMaterial )
{
	// 재료가 문스톤인가?
	if( pMaterial->m_dwItemId != II_GEN_MAT_MOONSTONE && pMaterial->m_dwItemId != II_GEN_MAT_MOONSTONE_1 )
	{
		AddDefinedText( TID_GAME_NOTEQUALITEM );
		return;
	}
	if( pTarget->GetAbilityOption() >= MAX_AAO )	// 20
	{
		AddDefinedText( TID_GAME_ACCESSORY_MAX_AAO );
		return;
	}

	// log
	LogItemInfo lii;
	lii.SendName	= GetName();
	lii.RecvName	= "UPGRADEITEM";
	lii.WorldId	= GetWorld()->GetID();
	lii.Gold	= GetGold();
	lii.Gold2	= GetGold();

	CAccessoryProperty* pProperty	= CAccessoryProperty::GetInstance();
	DWORD dwProbability		= pProperty->GetProbability( pTarget->GetAbilityOption() );
	DWORD dwRandom	= xRandom( 10000 );	// 0 - 9999

	// 액세서리 보호의 두루마리
	BOOL bSmelprot	= FALSE;
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT4 ) )
	{
		bSmelprot	= TRUE;
		RemoveBuff( BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT4 );
		ItemProp* pItemProp = prj.GetItemProp( II_SYS_SYS_SCR_SMELPROT4 );
		if( pItemProp )
			g_dpDBClient.SendLogSMItemUse( "2", this, NULL, pItemProp );
	}

	if( dwRandom < dwProbability )	// 성공
	{
		AddDefinedText( TID_UPGRADE_SUCCEEFUL );
		AddPlaySound( SND_INF_UPGRADESUCCESS );
			
		if( IsMode( TRANSPARENT_MODE ) == 0)
			g_UserMng.AddCreateSfxObj( this, XI_INT_SUCCESS, GetPos().x, GetPos().y, GetPos().z);

		int nAbilityOption	= pTarget->GetAbilityOption() + 1;
		UpdateItem( pTarget->m_dwObjId, UI_AO,  nAbilityOption );

		lii.Action		= "H";
		g_DPSrvr.OnLogItem( lii, pTarget, pTarget->m_nItemNum );
	}
	else	// 실패
	{
		AddDefinedText( TID_UPGRADE_FAIL );
		AddPlaySound( SND_INF_UPGRADEFAIL );
		if( IsMode( TRANSPARENT_MODE ) == 0 )
			g_UserMng.AddCreateSfxObj( this, XI_INT_FAIL, GetPos().x, GetPos().y, GetPos().z );
		if( !bSmelprot )
		{
			if( pTarget->GetAbilityOption() >= 3 )		// 삭제
			{
				lii.Action	= "L";
				g_DPSrvr.OnLogItem( lii, pTarget, pTarget->m_nItemNum );
				UpdateItem( pTarget->m_dwObjId, UI_NUM, 0 );
			}
		}
	}
	lii.Action	= "N";
	g_DPSrvr.OnLogItem( lii, pMaterial, pMaterial->m_nItemNum );
	UpdateItem( pMaterial->m_dwObjId, UI_NUM, pMaterial->m_nItemNum - 1 );	
}
#endif // __SMELT_SAFETY
#endif	// __SYS_COLLECTING

#ifdef __SYS_TICKET
CItemElem* CUser::FindActiveTicket( DWORD dwItemId )
{
	for( int i = 0; i < m_Inventory.GetMax(); i++ )
	{
		CItemElem* pItem	= m_Inventory.GetAtId( i );
		if( pItem && pItem->IsActiveTicket( dwItemId ) )
			return pItem;
	}
	for( i = 0; i < MAX_POCKET; i++ )
	{
		if( !m_Pocket.IsAvailable( i, FALSE ) )
			continue;
		CPocket* pPocket	= m_Pocket.GetPocket( i );
		for( int j = 0; j < pPocket->GetMax(); j++ )
		{
			CItemElem* pItem	= pPocket->GetAtId( j );
			if( pItem && pItem->IsActiveTicket( dwItemId ) )
				return pItem;
		}
	}
	return NULL;
}

void CUser::DoUseItemTicket( CItemElem* pItemElem )
{
	if( g_GuildCombat1to1Mng.IsPossibleUser( this ) )
		return;

	ItemProp* pItemProp		= pItemElem->GetProp();
	if( HasBuffByIk3( pItemProp->dwItemKind3 ) )
	{
		if( !HasBuff( BUFF_ITEM, (WORD)( pItemElem->m_dwItemId ) ) )
		{
			AddDefinedText( TID_GAME_MUST_STOP_OTHER_TICKET );
			return;
		}
		if( pItemElem->m_dwKeepTime == 0 )
		{
			AddDefinedText( TID_GAME_LIMITED_USE );
			return;
		}
		RemoveBuff( BUFF_ITEM, (WORD)( pItemElem->m_dwItemId ) );
		REPLACE( g_uIdofMulti, WI_WORLD_MADRIGAL, D3DXVECTOR3( 6971.984F, 100.0F, 3336.884F ), REPLACE_FORCE, nDefaultLayer );
	}
	else
	{
#if __VER >= 13 // __RAINBOW_RACE
		if( CRainbowRaceMng::GetInstance()->IsEntry( m_idPlayer ) )
		{
			AddDefinedText( TID_GAME_RAINBOWRACE_NOTELEPORT );
			return;
		}
#endif // __RAINBOW_RACE
		if( pItemElem->m_dwKeepTime == 0 )
		{
			CItemElem* pTicket	= FindActiveTicket( pItemElem->m_dwItemId );
			if( pTicket )
			{
				AddDefinedText( TID_GAME_LIMITED_USE );
				return;
			}
			else
			{
				UpdateItem( (BYTE)( pItemElem->m_dwObjId ), UI_KEEPTIME, pItemElem->GetProp()->dwAbilityMin );
				// log
				LogItemInfo	log;
				log.Action	= "e";
				log.SendName	= GetName();
				log.RecvName	= "DoUseItemTicket";
				log.WorldId		= GetWorld()->GetID();
				log.Gold	= GetGold();
				log.Gold2	= GetGold();
				g_DPSrvr.OnLogItem( log, pItemElem, 1 );
			}
		}
		CTicketProperty* pProperty	= CTicketProperty::GetInstance();
		TicketProp* pTicketProp		= pProperty->GetTicketProp( pItemElem->m_dwItemId );
		if( pTicketProp )
		{
#ifdef __AZRIA_1023
			int nLayer	= ::atoi( GetInput() );
			int nExpand	= CTicketProperty::GetInstance()->GetExpanedLayer( pTicketProp->dwWorldId );
			if( nLayer <= 0 && nLayer >= -nExpand )
			{
				DoApplySkill( (CCtrl*)this, pItemProp, NULL );
				REPLACE( g_uIdofMulti, pTicketProp->dwWorldId, pTicketProp->vPos, REPLACE_NORMAL, nLayer );
			}
#else	// __AZRIA_1023
			DoApplySkill( (CCtrl*)this, pItemProp, NULL );
			REPLACE( g_uIdofMulti, pTicketProp->dwWorldId, pTicketProp->vPos, REPLACE_NORMAL, nTempLayer );
#endif	// __AZRIA_1023
		}
	}
}
#endif	// __SYS_TICKET

#if __VER >= 11 // __GUILD_COMBAT_1TO1
void CUser::AddGC1to1TenderOpenWnd( int nPenya )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_GC1TO1_TENDEROPENWND;
	m_Snapshot.ar << nPenya;
	m_Snapshot.ar << g_GuildCombat1to1Mng.m_nJoinPenya;
}

void CUser::AddGC1to1TenderGuildView( int nPenya, int nRanking, time_t t, vector<CGuildCombat1to1Mng::__GC1TO1TENDER>& vecTenderGuild )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_GC1TO1_TENDERVIEW;
	m_Snapshot.ar << nPenya;
	m_Snapshot.ar << nRanking;
	m_Snapshot.ar << t;
	m_Snapshot.ar << vecTenderGuild.size();
	for( int i=0; i<(int)( vecTenderGuild.size() ); i++ )
		m_Snapshot.ar << vecTenderGuild[i].ulGuildId;
}

void CUser::AddGC1to1NowState( int nState, int nTime, int nProgCount )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_GC1TO1_NOWSTATE;
	m_Snapshot.ar << nState << nTime;

	if( nState == g_GuildCombat1to1Mng.GC1TO1_ENTRANCE )
	{
		// 구성된 멤버 ID목록
		int nIndex = g_GuildCombat1to1Mng.GetTenderGuildIndexByUser( this );
		int nSize = g_GuildCombat1to1Mng.m_vecTenderGuild[nIndex].vecMemberId.size();
		m_Snapshot.ar << nSize;
		for( int i=0; i<nSize; i++ )
			m_Snapshot.ar << g_GuildCombat1to1Mng.m_vecTenderGuild[nIndex].vecMemberId[i];
		// 상대편 길드 ID
		nIndex = g_GuildCombat1to1Mng.GetTargetTenderGuildIndexByUser( this );
		m_Snapshot.ar << g_GuildCombat1to1Mng.m_vecTenderGuild[nIndex].ulGuildId;
	}

	if( nState == CGuildCombat1to1::GC1TO1WAR_WAIT )
		m_Snapshot.ar << nProgCount;
}

void CUser::AddGC1to1MemberLineUpOpenWnd( vector<u_long>& vecMemberId )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_GC1TO1_MEMBERLINEUPOPENWND;
	m_Snapshot.ar << g_GuildCombat1to1Mng.m_nMinJoinPlayerLevel;
	m_Snapshot.ar << g_GuildCombat1to1Mng.m_nMinJoinPlayer;
	m_Snapshot.ar << g_GuildCombat1to1Mng.m_nMaxJoinPlayer;
	m_Snapshot.ar << vecMemberId.size();
	for( int i=0; i<(int)( vecMemberId.size() ); i++ )
		m_Snapshot.ar << vecMemberId[i];
}

void CUser::AddGC1to1WarResult( int m_nState, u_long uIdPlayer, int nWinCount0, int nWinCount1 )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_GC1TO1_WARRESULT;
	m_Snapshot.ar << m_nState << uIdPlayer << nWinCount0 << nWinCount1;
}
#endif // __GUILD_COMBAT_1TO1

#if __VER >= 8 // __CSC_VER8_5
void CUser::RemoveAngel( void )
{
#ifdef __ANGEL_LOG
	if( HasBuffByIk3( IK3_ANGEL_BUFF ) )
	{
#ifdef __BUFF_1107
		IBuff* pBuff	= m_buffs.GetBuffByIk3( IK3_ANGEL_BUFF );
		ItemProp* pItemProp	= NULL;
		if( pBuff )
			pItemProp	= pBuff->GetProp();
#else	// __BUFF_1107
		LPSKILLINFLUENCE lpSkillIn	= m_SkillState.GetItemBuf( IK3_ANGEL_BUFF );
		ItemProp* pItemProp	= NULL;
		if( lpSkillIn )
			pItemProp	= prj.GetItemProp( lpSkillIn->wID );
#endif	// __BUFF_1107
		if( pItemProp )
		{
			LogItemInfo aLogItem;
			aLogItem.Action = "&";
			aLogItem.SendName	= GetName();
			aLogItem.RecvName	= "ANGEL_DIE";
			aLogItem.WorldId	= GetWorld()->GetID();
			aLogItem.Gold		= aLogItem.Gold2	= GetGold();
			aLogItem.Gold_1		= (DWORD)( m_nAngelExp );
			//aLogItem.ItemName	= pItemProp->szName;
			_stprintf( aLogItem.szItemName, "%d", pItemProp->dwID );
			g_DPSrvr.OnLogItem( aLogItem );
		}
		RemoveIk3Buffs( IK3_ANGEL_BUFF );
		m_nAngelLevel	= 0;
		m_nAngelExp		= 0;
	}
#else // __ANGEL_LOG
	m_SkillState.RemoveLikeItemBuf( IK3_ANGEL_BUFF );
	m_nAngelLevel	= m_nAngelExp	= 0;
#endif // __ANGEL_LOG
	AddAngelInfo();
}
#endif	// __CSC_VER8_5

#if __VER >= 11 // __SYS_POCKET
void	CUser::AddPocketAttribute( int nAttribute, int nPocket, int nData )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_POCKET_ATTRIBUTE;
	m_Snapshot.ar << nAttribute << nPocket << nData;
}

void	CUser::AddPocketView( void )
{
	for( int i = 0; i < MAX_POCKET; i++ )
	{
		char sPocket[100]	= { 0,};
		sprintf( sPocket, "p# : %d, b : %d, t : %d", i, m_Pocket.IsAvailable( i ), m_Pocket.GetAvailable( i ) );
		AddText( sPocket );
		CPocket* pPocket	= m_Pocket.GetPocket( i );
		if( pPocket )
		{
			for( int j = 0; j < pPocket->GetMax(); j++ )
			{
				CItemElem* pItem	= pPocket->GetAtId( j );
				if( pItem )
				{
					char sItem[100]	= { 0,};
					sprintf( sItem, "nItem : %d, szName: %s, nNum : %d", j, pItem->GetProp()->szName, pItem->m_nItemNum );
					AddText( sItem );
				}
			}
		}
	}
}

void	CUser::AddPocketAddItem( int nPocket, CItemElem* pItem )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_POCKET_ADD_ITEM;
	m_Snapshot.ar << nPocket;
	pItem->Serialize( m_Snapshot.ar );
}

void	CUser::AddPocketRemoveItem( int nPocket, int nItem, short nNum )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_POCKET_REMOVE_ITEM;
	m_Snapshot.ar << nPocket << nItem << nNum;
}
#endif	// __SYS_POCKET

#ifdef __JEFF_11
void	CUser::AddQuePetResurrectionResult( BOOL bResult )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_QUE_PETRESURRECTION_RESULT;
	m_Snapshot.ar << bResult;
}
#endif	// __JEFF_11

void CUser::ValidateItem( void )
{
	int nSize	= m_Inventory.GetMax();
	ItemProp* pItemProp;
	for( int i = 0; i < nSize; i++ )
	{
		CItemElem* pItemElem	= m_Inventory.GetAtId( i );
		if( pItemElem )
		{
			pItemElem->SetExtra( 0 );
			if( pItemElem->GetSerialNumber() == 0 )
				pItemElem->SetSerialNumber();

			pItemProp	= pItemElem->GetProp();
			if( !pItemProp || pItemProp->dwItemKind3 == IK3_VIRTUAL )
			{
				WriteError( "ITEMPROP//0//%s//%d", GetName(), pItemElem->m_dwItemId );
				m_Inventory.RemoveAtId( i );
				continue;
			}
			if( pItemElem->m_nItemNum <= 0 )
			{
				WriteError( "ITEMNUM//0//%s//%d", GetName(), pItemElem->m_nItemNum );
				m_Inventory.RemoveAtId( i );
			}
		}
	}
}

void CUser::AdjustGuildQuest( DWORD dwWorldId )
{
	CGuildQuestProcessor* pProcessor	= CGuildQuestProcessor::GetInstance();
	int nId = -1;
	if( dwWorldId == WI_WORLD_MADRIGAL )
		nId = pProcessor->PtInQuestRect( GetPos() );

	if( nId > -1 )
	{
		PGUILDQUESTELEM pElem	= pProcessor->GetGuildQuest( nId );
		if( !pElem || pElem->idGuild != m_idGuild )
		{
			CWorld* pWorld	= g_WorldMng.GetWorld( dwWorldId );
			if( pWorld )
			{
				PRegionElem pRgnElem	= NULL;
				if( IsChaotic() )
				{
					if( pWorld->GetID() != pWorld->m_dwIdWorldRevival && pWorld->m_dwIdWorldRevival != 0 )
						pRgnElem	= g_WorldMng.GetRevivalPosChao( pWorld->m_dwIdWorldRevival, pWorld->m_szKeyRevival );
					if( !pRgnElem )	// Find near revival pos
						pRgnElem	= g_WorldMng.GetNearRevivalPosChao( pWorld->GetID(), GetPos() );
				}
				else
				{
					if( pWorld->GetID() != pWorld->m_dwIdWorldRevival && pWorld->m_dwIdWorldRevival != 0 )
						pRgnElem	= g_WorldMng.GetRevivalPos( pWorld->m_dwIdWorldRevival, pWorld->m_szKeyRevival );
					if( !pRgnElem )	// Find near revival pos
						pRgnElem	= g_WorldMng.GetNearRevivalPos( pWorld->GetID(), GetPos() );
				}
				if( pRgnElem )
					SetPos( pRgnElem->m_vPos );
			}
		}
	}
}

void CUser::AdjustMailboxState( void )
{
	CMailBox* pMailBox	= CPost::GetInstance()->GetMailBox( m_idPlayer );
	if( pMailBox && pMailBox->IsStampedMailExists() )
		SetMode( MODE_MAILBOX );
	else
		SetNotMode( MODE_MAILBOX );
}

void CUser::AddEventLuaDesc( void )
{
	vector<BYTE> vecList	= prj.m_EventLua.GetEventList();
	for( int it = 0; it < (int)( vecList.size() ); it++ )
		AddEventLuaDesc( 2, prj.m_EventLua.GetDesc( vecList[it] ) );
}

void CUser::AdjustPartyQuest( DWORD dwWorldId )
{
	CPartyQuestProcessor* pProc		= CPartyQuestProcessor::GetInstance();
	int nId		= pProc->PtInQuestRect( dwWorldId, GetPos() );
	if( nId > -1 )
	{
		PPARTYQUESTELEM pElem	= pProc->GetPartyQuest( nId );
		if( pElem && pElem->idParty == m_idparty )
		{
			DWORD dwTime	= pElem->dwEndTime - GetTickCount();
			AddQuestTextTime( TRUE, pElem->nProcess , dwTime );
		}
		else if( !pElem || pElem->idParty != m_idparty )
		{
			SetMode( MODE_OUTOF_PARTYQUESTRGN );
		}
	}
}

#if __VER >= 13 // __COUPLE_1117
void CUser::AddCouple()
{
	if( IsDelete() )	return;
	CCouple* pCouple	= CCoupleHelper::Instance()->GetCouple( m_idPlayer );
	if( pCouple )
	{
		m_Snapshot.cb++;
		m_Snapshot.ar << GetId();
		m_Snapshot.ar << SNAPSHOTTYPE_COUPLE;
		pCouple->Serialize( m_Snapshot.ar );
	}
}

void CUser::AddProposeResult( u_long idProposer, const char* pszProposer )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PROPOSE_RESULT;
	m_Snapshot.ar << idProposer;
	m_Snapshot.ar.WriteString( pszProposer );
}

void CUser::AddCoupleResult( u_long idPartner, const char* pszPartner )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_COUPLE_RESULT;
	m_Snapshot.ar << idPartner;
	m_Snapshot.ar.WriteString( pszPartner );
}

void CUser::AddDecoupleResult()
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_DECOUPLE_RESULT;
}

#if __VER >= 13 // __COUPLE_1202
// 1회/1초 
void CUser::ProcessCouple()
{
	BOOL bTick	= FALSE;
	if( ++m_cbProcessCouple > 60 )
	{
		bTick	= TRUE;
		m_cbProcessCouple	= 0;
	}
	CCouple* pCouple	= CCoupleHelper::Instance()->GetCouple( m_idPlayer );
	if( pCouple )
	{
		CUser* pPartner	= static_cast<CUser*>( prj.GetUserByID( pCouple->GetPartner( m_idPlayer ) ) );
		if( IsValidObj( pPartner ) )
		{
			if( bTick && m_idPlayer > pPartner->m_idPlayer && pCouple->GetLevel() < CCouple::eMaxLevel )	// 1 커플 1회 요청
				g_dpDBClient.SendQueryAddCoupleExperience( m_idPlayer, 1 );

			if( !HasBuffByIk3( IK3_COUPLE_BUFF ) )
				ActiveCoupleBuff( pCouple->GetLevel() );
		}
		else
		{
			RemoveIk3Buffs( IK3_COUPLE_BUFF );
		}
	}
	else
	{
		RemoveIk3Buffs( IK3_COUPLE_BUFF );
	}
}

void CUser::ActiveCoupleBuff( int nLevel )
{
	VS& vSkills	= CCoupleProperty::Instance()->GetSkill( nLevel );
	for( int i = 0; i < (int)( vSkills.size() ); i++ )
	{
		ItemProp* pProp	= prj.GetItemProp( vSkills[i] );
		if( pProp )
			DoApplySkill( this, pProp, NULL );
	}
}

void CUser::AddAddCoupleExperience( int nExperience )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ADD_COUPLE_EXPERIENCE;
	m_Snapshot.ar << nExperience;
}
#endif	// __COUPLE_1202

#endif	// __COUPLE_1117
/*----------------------------------------------------------------------------------------------------------------------------------------------------------------*/
CUserMng::CUserMng()
{
	m_lCount = 0;
}

CUserMng::~CUserMng()
{
}


// 캐쉬서버와 연결이 끊길 경우, 모든 유저를 삭제한다.
// 월드서버에서 소켓과 접속이 끊길 경우 호출된다.
void CUserMng::RemoveAllUsers()
{
	map<DWORD, CUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		DestroyPlayer( it->second );
	}

	m_users.clear();
	m_lCount = 0;
}


CUser* CUserMng::AddUser( DPID dpidCache, DPID dpidUser, DPID dpidSocket )
{
	CUser *pUser = GetUser( dpidCache, dpidUser );
	if( pUser == NULL )
	{
		++m_lCount;
		
		pUser = new CUser( dpidCache, dpidSocket );
		pUser->m_dwSerial = dpidUser;
		m_users.insert( make_pair( dpidUser, pUser ) );
	}
	else
	{
		ASSERT( FALSE );
		pUser = NULL;
	}

	return pUser;
}

extern CCommonCtrl* CreateExpBox( CUser* pUser );

void CUserMng::RemoveUser( DWORD dwSerial )
{
	map<DWORD, CUser*>::iterator it = m_users.find( dwSerial );
	if( it == m_users.end() )
		return;

	CUser* pUser = it->second;

	if( IsValidObj( pUser ) ) 
	{
		pUser->OnTradeRemoveUser();

		m_users.erase( it );
		--m_lCount;
		RemoveUserFromCacheMsg( pUser );
	}
}


// 캐쉬서버로 부터 PACKETTYPE_LEAVE를 받은 경우에만  
void CUserMng::RemoveUserFromCacheMsg( CUser *pUser )
{
	// 안전지역이면 즉시 종료 
	if( pUser->m_pWorld == NULL || pUser->IsRegionAttr(RA_SAFETY) || pUser->IsAuthHigher( AUTH_GAMEMASTER ) )   // 안전지역?		
	{
		DestroyPlayer( pUser );
		return;
	}

	if( pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_GUILDWAR )
	{
		DestroyPlayer( pUser );
		return;
	}

	ASSERT( pUser->m_dwDestroyTime == 0 );

	// 반대의 경우는 종료예약 
	if( pUser->m_dwLeavePenatyTime == 0 )						// 페널티 시간이 설정 안되었으면 
	{
		pUser->ScheduleDestory( ::timeGetTime() + TIMEWAIT_CLOSE * 1000 );	//  종료예약 
		g_dpDBClient.SendPreventLogin( pUser->m_playAccount.lpszAccount, TIMEWAIT_CLOSE + 5 );
	}
	else if( ::timeGetTime() > pUser->m_dwLeavePenatyTime )		// 페널티 시간을 견디었으면 
	{
		DestroyPlayer( pUser );												// 종료 
	}
	else														// 페널티 시간을 견디지 못했으면
	{
		pUser->ScheduleDestory( pUser->m_dwLeavePenatyTime );				//  종료예약 
		g_dpDBClient.SendPreventLogin( pUser->m_playAccount.lpszAccount, TIMEWAIT_CLOSE );
	}
}

CUser* CUserMng::GetUser( DPID dpidCache, DPID dpidUser )
{
	UNUSED_ALWAYS(dpidCache);

	map<DWORD, CUser*>::iterator it = m_users.find( dpidUser );
	if( it != m_users.end() )
		return it->second;
	else
		return NULL;
}

CUser* CUserMng::GetUserByPlayerID( u_long idPlayer )
{
	return (CUser*)prj.GetUserByID( idPlayer );
}

#ifdef __LAYER_1015
BOOL CUserMng::AddPlayer( CUser *pUser, DWORD dwWorldID, int nLayer )
#else	// __LAYER_1015
BOOL CUserMng::AddPlayer( CUser *pUser, DWORD dwWorldID )
#endif	// __LAYER_1015
{
#ifdef __MAP_SECURITY
	pUser->Open( dwWorldID );
#else // __MAP_SECURITY
	pUser->Open();	// Including set id
#endif // __MAP_SECURITY
#ifdef __LAYER_1015
	BOOL bResult = g_WorldMng.AddObj( pUser, dwWorldID, FALSE, nLayer ); 
#else	//__LAYER_1015
	BOOL bResult = g_WorldMng.AddObj( pUser, dwWorldID, FALSE ); 
#endif	// __LAYER_1015
	pUser->SetValid( bResult );
	return bResult;
}

void CUserMng::DestroyPlayer( CUser* pUser )
{
	CWorld* pWorld = pUser->GetWorld();
	if( pWorld == NULL ) 	
	{
		safe_delete( pUser );
		return;
	}

#if __VER >= 11 // __SYS_COLLECTING
	if( pUser->IsCollecting() )
		pUser->StopCollecting();
#endif	// __SYS_COLLECTING

#ifdef __EVE_MINIGAME
	prj.m_MiniGame.DestroyWnd_FiveSystem( pUser );
#endif // __EVE_MINIGAME

#if __VER >= 8 //__Y_FLAG_SKILL_BUFF
	if( pUser->HasActivatedEatPet() )
		pUser->InactivateEatPet();
#endif //__Y_FLAG_SKILL_BUFF		

	D3DXVECTOR3 vPos;
	DWORD		dwWorldId;
#ifdef __LAYER_1015
	int nLayer	= nDefaultLayer;
#endif	// __LAYER_1015

	// 로그 아웃시 길드대전 맵에 있으면 모두 flaris로 이동
	if( pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_GUILDWAR )
	{
		PRegionElem pRgnElem = g_WorldMng.GetRevivalPos( WI_WORLD_MADRIGAL, "flaris" );
		if( pRgnElem )
		{
			dwWorldId	= pRgnElem->m_dwWorldId;
			vPos	= pRgnElem->m_vPos;
#ifdef __LAYER_1015
			nLayer	= nRevivalLayer;
#endif	// __LAYER_1015
		}
		else
		{
			dwWorldId	= pUser->GetWorld()->GetID();
			vPos	= pUser->GetPos();
#ifdef __LAYER_1015
			nLayer	= pUser->GetLayer();
#endif	// __LAYER_1015
		}
		if( !pUser->IsDie() )
			pUser->RemoveAllEnemies();
	}
	else
	{
		if( pUser->IsDie() )
		{
			CWorld* pWorld = pUser->GetWorld();
			
			CCommonCtrl* pCtrl	= CreateExpBox( pUser );
			if( pCtrl && pWorld )
			{
				pCtrl->AddItToGlobalId();
				pWorld->ADDOBJ( pCtrl, FALSE, pUser->GetLayer() );
				g_dpDBClient.SendLogExpBox( pUser->m_idPlayer, pCtrl->GetId(), pCtrl->m_nExpBox );
			}
			float fRate		= pUser->SubDieDecExp( FALSE );
			pUser->SetHitPoint( (int)(pUser->GetMaxHitPoint() * fRate) );	// hp 회복
			int nVal	= (int)(pUser->GetMaxManaPoint() * fRate);			// mp 회복
			if( pUser->GetManaPoint() < nVal )
				pUser->SetManaPoint( nVal );
			nVal	= (int)(pUser->GetMaxFatiguePoint() * fRate);			// fp 회복
			if( pUser->GetFatiguePoint() < nVal )
				pUser->SetFatiguePoint( nVal );
			

			PRegionElem pRgnElem	= NULL;
			if( pUser->IsChaotic() )
			{
#if __VER >= 8 // __S8_PK
				if( pWorld->GetID() != pWorld->m_dwIdWorldRevival && pWorld->m_dwIdWorldRevival != 0 )
					pRgnElem	= g_WorldMng.GetRevivalPosChao( pWorld->m_dwIdWorldRevival, pWorld->m_szKeyRevival );
				
				if( NULL == pRgnElem )
					pRgnElem	= g_WorldMng.GetNearRevivalPosChao( pWorld->GetID(), pUser->GetPos() );
#else // __VER >= 8 // __S8_PK
				pRgnElem	= g_WorldMng.GetNearRevivalPosChao( pWorld->GetID(), pUser->GetPos() );
#endif // __VER >= 8 // __S8_PK
			}
			else
			{
				if( pWorld->GetID() != pWorld->m_dwIdWorldRevival && pWorld->m_dwIdWorldRevival != 0 )
					pRgnElem	= g_WorldMng.GetRevivalPos( pWorld->m_dwIdWorldRevival, pWorld->m_szKeyRevival );
				
				if( NULL == pRgnElem )
					pRgnElem	= g_WorldMng.GetNearRevivalPos( pWorld->GetID(), pUser->GetPos() );
			}

			if( pRgnElem )
			{
				dwWorldId	= pRgnElem->m_dwWorldId;
				vPos	= pRgnElem->m_vPos;
#ifdef __LAYER_1015
				nLayer	= nRevivalLayer;
#endif	// __LAYER_1015
			}
			else
			{
				dwWorldId	= pWorld->GetID();
				vPos	= pUser->GetPos();
#ifdef __LAYER_1015
				nLayer	= pUser->GetLayer();
#endif	// __LAYER_1015
			}
		}
		else 
		{
			dwWorldId	= pWorld->GetID();
			vPos	= pUser->GetPos();
#ifdef __LAYER_1015
			nLayer	= pUser->GetLayer();
#endif	// __LAYER_1015
			if( pUser->IsFly() )	// 비행중이었으면 스탑시킴.
				pUser->SendActMsg( OBJMSG_STOP );

			pUser->RemoveAllEnemies();
		}
	}

	CChatting* pChatting = g_ChattingMng.GetChttingRoom( pUser->m_idChatting );
	if( pChatting )
	{
		CUser * pUserBuf;
		for( int i = 0 ; i < pChatting->GetChattingMember() ; ++i )
		{
			pUserBuf = (CUser*)prj.GetUserByID( pChatting->m_idMember[i] );
			if( ::IsValidObj( pUserBuf ) )
			{
				// 채팅에서 나감
				pUserBuf->AddRemoveChatting( pUser->m_idPlayer );
			}
		}
		pChatting->RemoveChattingMember( pUser->m_idPlayer );
		pUser->m_idChatting = 0;
	}	

#if __VER >= 12 // __SECRET_ROOM
	// 비밀의 방 - 길마이면 패배 처리하고 아니면 밖으로 쫓아냄
	if( CSecretRoomMng::GetInstance()->IsInTheSecretRoom( pUser ) )
	{
		//if( CSecretRoomMng::GetInstance()->IsGuildMaster( pUser ) )
		//	CSecretRoomMng::GetInstance()->SetFailGuild( pUser );
		
		dwWorldId = WI_WORLD_MADRIGAL;
		vPos = CSecretRoomMng::GetInstance()->GetRevivalPos( pUser );
#ifdef __LAYER_1015
		nLayer	= nRevivalLayer;
#endif	// __LAYER_1015
	}
	
	// 사냥터용 비밀의 방안에 있으면...
	if( pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_DUNGEON_SECRET_0 )
	{
#ifdef __LAYER_1015
		nLayer	= nDefaultLayer;
#endif	// __LAYER_1015
		if( pUser->m_idMarkingWorld == WI_WORLD_MADRIGAL )
		{
			dwWorldId = pUser->m_idMarkingWorld;
			vPos = pUser->m_vMarkingPos;
		}
		else
		{
			dwWorldId = WI_WORLD_MADRIGAL;
			vPos = D3DXVECTOR3( 6968, 100, 3328 );
		}
	}
#endif // __SECRET_ROOM

#if __VER >= 13 // __HOUSING
	if( pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_MINIROOM )
	{
		dwWorldId = WI_WORLD_MADRIGAL;
		vPos = D3DXVECTOR3( 6968, 100, 3328 );
#ifdef __LAYER_1015
		nLayer	= nDefaultLayer;
#endif	// __LAYER_1015
	}
	CHousingMng::GetInstance()->DestroyHousing( pUser->m_idPlayer );
#endif // __HOUSING

#if __VER >= 13 // __RAINBOW_RACE
	// 접속종료시 탈락 처리로 변경 chipi_090317
	CRainbowRaceMng::GetInstance()->SetDropOut( pUser->m_idPlayer );
#endif // __RAINBOW_RACE

#if __VER >= 14 // __INSTANCE_DUNGEON
	if( CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( pUser->GetWorld()->GetID() ) )
	{
		CInstanceDungeonHelper::GetInstance()->GoOut( pUser );
		if( pUser->m_idMarkingWorld == WI_WORLD_MADRIGAL )
		{
			dwWorldId = pUser->m_idMarkingWorld;
			vPos = pUser->m_vMarkingPos;
		}
		else
		{
			dwWorldId = WI_WORLD_MADRIGAL;
			vPos = D3DXVECTOR3( 6968, 100, 3328 );
		}
		nLayer = nDefaultLayer;
	}
#endif // __INSTANCE_DUNGEON

#ifdef __LAYER_1015
	g_dpDBClient.SavePlayer( pUser, dwWorldId, vPos, nLayer, TRUE );
#else	// __LAYER_1015
	g_dpDBClient.SavePlayer( pUser, dwWorldId, vPos, TRUE );
#endif	// __LAYER_1015

#ifdef __S_NEW_SKILL_2
	g_dpDBClient.SaveSkill( pUser );	// 게임 종료시 스킬 저장
#endif // __S_NEW_SKILL_2
	g_dpDBClient.SendLogConnect( pUser );
#if __VER >= 14 // __PCBANG
	CPCBang::GetInstance()->DestroyPCBangPlayer( pUser->m_idPlayer );
#endif // __PCBANG
#if __VER >= 15 // __GUILD_HOUSE
	GuildHouseMng->GoOutGuildHouse( pUser );
#endif // __GUILD_HOUSE
#ifdef __INVALID_LOGIN_0320
	g_dpDBClient.CalluspXXXMultiServer( 0, pUser );
#else	// __INVALID_LOGIN_0320
	g_dpDBClient.CalluspXXXMultiServer( 0, pUser->m_idPlayer );
#endif	// __INVALID_LOGIN_0320

	
	//////////////////////////////////////////////////////////////////////////
	pUser->ResetCheckClientReq();
	//////////////////////////////////////////////////////////////////////////


	if( pWorld->DoNotAdd( pUser ) )		// pUser가 m_apAddObjs중에 있으면 LINKMAP에는 없다. 
	{
		SAFE_DELETE( pUser );			// 메모리에서 삭제 
	}
	else
	{
		pUser->Delete();				// 링크맵에서도 삭제하게 한다.
	}
	if( pUser )
	{
		if( g_eLocal.GetState( EVE_GUILDCOMBAT ) )
		{
			if( g_GuildCombatMng.m_nGCState == CGuildCombat::MAINTENANCE_STATE || g_GuildCombatMng.m_nGCState == CGuildCombat::WAR_WAR_STATE )
				g_GuildCombatMng.OutWar( pUser, NULL, TRUE );
		}
#if __VER >= 11 // __GUILD_COMBAT_1TO1
		if( g_eLocal.GetState( EVE_GUILDCOMBAT1TO1 ) )
		{
			if( g_GuildCombat1to1Mng.IsPossibleUser( pUser ) && pUser->m_nGuildCombatState == 1 )
			{
				int nIndex = g_GuildCombat1to1Mng.GetTenderGuildIndexByUser( pUser );
				if( nIndex != NULL_ID )
				{
					int nStageId = g_GuildCombat1to1Mng.m_vecTenderGuild[nIndex].nStageId;
					if( nStageId != NULL_ID )
						g_GuildCombat1to1Mng.m_vecGuilCombat1to1[nStageId].GuildCombat1to1WarResultCheck( TRUE, pUser );
				}
			}
		}
#endif // __GUILD_COMBAT_1TO1
	}
}
/*
void CUserMng::Notify( void )
{
	map<DWORD, CUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		CUser* pUser = it->second;
		pUser->Notify();
	}
}
*/

void CUserMng::ModifyMode( DWORD dwMode )
{
	map<DWORD, CUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		CUser* pUser = it->second;
		if( pUser->IsValid() == FALSE )
			continue;

		if( pUser->m_dwAuthorization < AUTH_GAMEMASTER )
		{
			pUser->SetMode( dwMode );
			AddModifyMode( pUser );
		}
	}
}

void CUserMng::AddEventMessage( CCtrl* pCtrl, const TCHAR* szChat, int nFlag, DWORD dwItemId )
{
	CAr ar;
	
	ar << GETID( pCtrl ) << SNAPSHOTTYPE_EVENTMESSAGE;
	ar.WriteString( szChat );
	ar << nFlag;
	ar << dwItemId;
	
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pCtrl )
	USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pCtrl )
}

void	CUserMng::AddDlgEmoticon( CCtrl* pCtrl, int nIdx )
{
	CHAR szString[32] = "!";
	CHAR szNum[32];
	itoa( nIdx, szNum, 10 );
	strcat( szString, szNum );
	AddChat( pCtrl, szString );
}

#if __VER >= 11 // __SYS_COLLECTING
void CUserMng::AddStartCollecting( CUser* pUser )
{
	CAr ar;

	ar << GETID( pUser ) << SNAPSHOTTYPE_START_COLLECTING;
	
	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pUser )
	{
		USERPTR->AddBlock( lpBuf, nBufSize );
	}
	NEXT_VISIBILITYRANGE( pUser )
}

void CUserMng::AddStopCollecting( CUser* pUser )
{
	CAr ar;

	ar << GETID( pUser ) << SNAPSHOTTYPE_STOP_COLLECTING;
	
	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pUser )
	{
		USERPTR->AddBlock( lpBuf, nBufSize );
	}
	NEXT_VISIBILITYRANGE( pUser )
}
#endif	// __SYS_COLLECTING

void CUserMng::AddChat( CCtrl* pCtrl, const TCHAR* szChat )
{
	CAr ar;

	ar << GETID( pCtrl ) << SNAPSHOTTYPE_CHAT;
	ar.WriteString( szChat );
	
	GETBLOCK( ar, lpBuf, nBufSize );

#ifdef __QUIZ
	BOOL bCheck = FALSE;
	D3DXVECTOR3 vPos = D3DXVECTOR3( 0, 0, 0 );
	if( pCtrl->GetType() == OT_MOVER && ( (CMover*)pCtrl )->IsPlayer() 
		&& ( (CUser*)pCtrl )->GetWorld() && ( (CUser*)pCtrl )->GetWorld()->GetID() == WI_WORLD_QUIZ )
	{
		bCheck = TRUE;
		vPos = ( (CUser*)pCtrl )->GetPos();
	}
#endif // __QUIZ

	FOR_VISIBILITYRANGE( pCtrl )
	{
#ifdef __QUIZ
		if( bCheck )
		{
			D3DXVECTOR3 vDist = vPos - USERPTR->GetPos();
			float fDistSq = D3DXVec3LengthSq( &vDist );
			if( fDistSq <= 10.0f )
#endif // __QUIZ
				USERPTR->AddBlock( lpBuf, nBufSize );
#ifdef __QUIZ
		}
		else
			USERPTR->AddBlock( lpBuf, nBufSize );
#endif // __QUIZ
		
		if( USERPTR->m_idSnoop > 0 && pCtrl->GetType() == OT_MOVER && ( (CMover*)pCtrl )->IsPlayer() )
			g_DPCoreClient.SendChat( ( (CMover*)pCtrl )->m_idPlayer, USERPTR->m_idPlayer, szChat );
	}
	NEXT_VISIBILITYRANGE( pCtrl )
}

void CUserMng::AddDefinedText( CMover* pMover, int dwText, LPCSTR lpszFormat, ... )
{
	TCHAR szBuffer[1024];
	
	va_list args;
	va_start( args, lpszFormat );
	int nBuf = _vsntprintf( szBuffer, sizeof(szBuffer)-1, lpszFormat, args );
	va_end( args );
	
	if( nBuf <= 0 )
		return;

	CAr ar;
	
	ar << GETID( pMover ) << SNAPSHOTTYPE_DEFINEDTEXT;
	ar << dwText;
	ar.WriteString( szBuffer );
	
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pMover )
	{
		USERPTR->AddBlock( lpBuf, nBufSize );
	}
	NEXT_VISIBILITYRANGE( pMover )		
}
void CUserMng::AddShipActMsg( CMover* pMover, CShip *pShip, DWORD dwMsg, int nParam1, int nParam2 )
{
	CAr ar;
	
	ar << GETID( pMover ) << SNAPSHOTTYPE_SHIP_ACTMSG;
	ar << dwMsg << nParam1 << nParam2 << pShip->GetId();
	
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pMover )
		if( USERPTR != pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void CUserMng::AddMotion( CMover* pMover, DWORD dwMsg )
{
	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_MOTION;
	ar << dwMsg;

	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pMover )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void CUserMng::AddRemoveAllSkillInfluence( CMover* pMover )
{
	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_REMOVEALLSKILLINFULENCE;
	
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pMover )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void CUserMng::AddWorldShout( const TCHAR* szName, const TCHAR* szShout, D3DXVECTOR3 vPos, CWorld* pWorld )
{
	if( pWorld )
	{
		CAr arBlock;
		arBlock << NULL_ID << SNAPSHOTTYPE_SHOUT;
		arBlock << NULL_ID;
		arBlock.WriteString( szName );
		arBlock.WriteString( szShout );
#if __VER >= 12 // __LORD
		arBlock << (DWORD)0xffff99cc;
#endif	// __LORD
		GETBLOCK( arBlock, lpBlock, uBlockSize );
		
		AddBlock( pWorld, vPos, 0xff, lpBlock, uBlockSize );
	}		
}

void CUserMng::AddDamage( CMover* pMover, OBJID objidAttacker, DWORD dwHit, DWORD dwAtkFlags )
{
	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_DAMAGE;
	ar << objidAttacker << dwHit;
	ar << dwAtkFlags;

	if( dwAtkFlags & AF_FLYING )
	{
		ar << pMover->GetPos();
		ar << pMover->GetAngle();
	}

	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pMover )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void CUserMng::AddDisguise( CMover* pMover, DWORD dwMoverIdx )
{
	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_DISGUISE << dwMoverIdx;

	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pMover )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}
void CUserMng::AddNoDisguise( CMover* pMover )
{
	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_NODISGUISE;

	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pMover )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}
void CUserMng::AddSetStateLevel( CMover* pMover, CHAR chID, long nValue )
{
	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_SET_STAT_LEVEL;
	ar << chID << nValue << pMover->m_nRemainGP;
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pMover )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}
void CUserMng::AddMoverDeath( CMover* pMover, CMover* pAttacker, DWORD dwMsg )
{
	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_MOVERDEATH;
	ar << GETID( pAttacker ) << dwMsg;

	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pMover )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void CUserMng::AddUseSkill( CMover* pMover, DWORD dwSkill, DWORD dwLevel, OBJID objid, int nUseType, int nCastingTime )
{
	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_USESKILL;
	ar << dwSkill << dwLevel;
	ar << objid << nUseType << nCastingTime;

	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pMover )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void CUserMng::AddDoEquip( CMover* pMover, int nPart, BYTE nId, const EQUIP_INFO & rEquipInfo, BYTE fEquip )
{
	CAr ar;
	DWORD	idGuild = 0;

	ar << GETID( pMover ) << SNAPSHOTTYPE_DOEQUIP;

	ar  << nId << idGuild << fEquip;
	ar.Write( (void*)&rEquipInfo, sizeof(EQUIP_INFO) );

	ar << nPart;

	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pMover )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void CUserMng::AddDoEquip( CMover* pMover, int nPart, CItemElem *pItemElem, BYTE fEquip )
{
	EQUIP_INFO equipInfo;
	equipInfo.dwId	= pItemElem->m_dwItemId;
	equipInfo.nOption	= pItemElem->GetAttrOption();
	equipInfo.byFlag	= pItemElem->m_byFlag;
	AddDoEquip( pMover, nPart, (BYTE)pItemElem->m_dwObjId, equipInfo, fEquip );
}


void CUserMng::AddVendor( CMover* pVendor )
{
	CAr ar;
	ar << GETID( pVendor ) << SNAPSHOTTYPE_VENDOR;

	for( int i = 0; i < MAX_VENDOR_INVENTORY_TAB; i++ )
		pVendor->m_ShopInventory[i]->Serialize( ar );

	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pVendor )
		if( USERPTR->m_vtInfo.GetOther() == pVendor )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pVendor )
}

void CUserMng::AddUpdateVendor( CMover* pVendor, CHAR cTab, BYTE nId, short nNum )
{
	CAr ar;
	ar << GETID( pVendor ) << SNAPSHOTTYPE_UPDATE_VENDOR;

	ar << cTab << nId << nNum;

	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pVendor )
		if( USERPTR->m_vtInfo.GetOther() == pVendor )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pVendor )
}

void CUserMng::AddSetDestParam( CMover* pMover, int nDstParameter, int nAdjParameterValue, int nChgParameterValue )
{
	CAr ar;

	ar << GETID( pMover );
	ar << SNAPSHOTTYPE_SETDESTPARAM;

	ar << nDstParameter << nAdjParameterValue << nChgParameterValue;

	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pMover )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void CUserMng::AddResetDestParam( CMover* pMover, int nDstParameter, int nAdjParameterValue )
{
	CAr ar;

	ar << GETID( pMover );
	ar << SNAPSHOTTYPE_RESETDESTPARAM;

	ar << nDstParameter << nAdjParameterValue;

	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pMover )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}
#ifdef __SPEED_SYNC_0108		// ResetDestParam speed 수정
void CUserMng::AddResetDestParamSync( CMover* pMover, int nDstParameter,int nAdjParameterValue, int nParameterValue )
{
	CAr ar;

	ar << GETID( pMover );
	ar << SNAPSHOTTYPE_RESETDESTPARAM_SYNC;

	ar << nDstParameter << nAdjParameterValue << nParameterValue;

	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pMover )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}
#endif // __SPEED_SYNC_0108		// ResetDestParam speed 수정

void CUserMng::AddSetPointParam( CMover* pMover, int nDstParameter, int nValue )
{
	CAr ar;

	ar << GETID( pMover );
	ar << SNAPSHOTTYPE_SETPOINTPARAM;

	ar << nDstParameter << nValue;

	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pMover )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void CUserMng::AddSetPos( CCtrl* pCtrl, D3DXVECTOR3 & vPos )
{
	CAr ar;

	ar << GETID( pCtrl ) << SNAPSHOTTYPE_SETPOS;
	ar << vPos;

	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pCtrl )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pCtrl )
}


void CUserMng::AddSetLevel( CMover* pMover, WORD wLevel )
{
	CAr ar;

	ar << GETID( pMover ) << SNAPSHOTTYPE_SETLEVEL;
	ar << wLevel;

	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pMover )
		if( USERPTR != pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void CUserMng::AddSetFlightLevel( CMover* pMover, int nFlightLv )
{
	CAr ar;
	
	ar << GETID( pMover ) << SNAPSHOTTYPE_SETFLIGHTLEVEL;
	ar << (WORD)nFlightLv;
	
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pMover )
		if( USERPTR != pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}


void CUserMng::AddSetSkillLevel( CMover* pMover, DWORD dwSkill, DWORD dwLevel )
{
	CAr ar;

	ar << GETID( pMover ) << SNAPSHOTTYPE_SETSKILLLEVEL;
	ar << dwSkill << dwLevel;

	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pMover )
		if( USERPTR != pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}


#ifdef __IAOBJ0622
void CUserMng::AddSetDestPos( CMover* pMover, CONST D3DXVECTOR3 & vPos, BYTE fForward, OBJID objidIAObj )
#else	// __IAOBJ0622
void CUserMng::AddSetDestPos( CMover* pMover, CONST D3DXVECTOR3 & vPos, BYTE fForward )
#endif	// __IAOBJ0622
{
	CAr ar;

	ar << GETID( pMover ) << SNAPSHOTTYPE_DESTPOS;
	ar << vPos << fForward;

#ifdef __IAOBJ0622
	ar << objidIAObj;
#endif	// __IAOBJ0622

	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pMover )
		if( USERPTR != pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void CUserMng::AddSetMovePattern( CMover* pMover, int nPattern, const D3DXVECTOR3 &vPos, FLOAT fAngle, FLOAT fAngleX )
{
	CAr ar;
	
	ar << GETID( pMover ) << SNAPSHOTTYPE_SETMOVEPATTERN;
	ar << nPattern << vPos << fAngle << fAngleX;
	
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pMover )
		if( USERPTR != pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void CUserMng::AddSetDestAngle( CMover* pMover, float fAngle, BYTE fLeft )
{
	CAr ar;

	ar << GETID( pMover ) << SNAPSHOTTYPE_DESTANGLE;
	ar << fAngle << fLeft;

	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pMover )
		if( USERPTR != pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}
void CUserMng::AddMoverSetDestObj( CMover* pMover, OBJID objid, float fRange, BOOL fTransferToMe )
{
	CAr ar;

	ar << GETID( pMover ) << SNAPSHOTTYPE_MOVERSETDESTOBJ;
	ar << objid << fRange;

	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pMover )
		if( ( fTransferToMe || USERPTR != pMover ) )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void CUserMng::AddMeleeAttack( CMover* pMover, DWORD dwAtkMsg, OBJID objid, int nParam2, int nParam3 )
{
	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_MELEE_ATTACK;
	ar << dwAtkMsg << objid << nParam2 << nParam3;
	
	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pMover )
		if( USERPTR != pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void CUserMng::AddMeleeAttack2( CMover* pMover, DWORD dwAtkMsg, OBJID objid, int nParam2, int nParam3 )
{
	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_MELEE_ATTACK2;
	ar << dwAtkMsg << objid << nParam2 << nParam3;
	
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pMover )
		if( USERPTR != pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
		NEXT_VISIBILITYRANGE( pMover )
}

void CUserMng::AddMagicAttack( CMover* pMover, DWORD dwAtkMsg, OBJID objid, int nParam2, int nParam3, int nMagicPower, int idSfxHit )
{
	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_MAGIC_ATTACK;
	ar << dwAtkMsg << objid << nParam2 << nParam3 << nMagicPower << idSfxHit;

	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pMover )
		if( USERPTR != pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void CUserMng::AddRangeAttack( CMover* pMover, DWORD dwAtkMsg, OBJID objid, int nParam2, int nParam3, int idSfxHit )
{
	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_RANGE_ATTACK;
	ar << dwAtkMsg << objid << nParam2 << nParam3 << idSfxHit;
	
	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pMover )
		if( USERPTR != pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void CUserMng::AddAttackSP( CMover* pMover, DWORD dwAtkMsg, OBJID objid, int nParam2, int nParam3 )
{
	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_SP_ATTACK;
	ar << dwAtkMsg << objid << nParam2 << nParam3 ;
	
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pMover )
		if( USERPTR != pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
		NEXT_VISIBILITYRANGE( pMover )
}

void CUserMng::AddMoverMoved( CMover* pMover, D3DXVECTOR3 & v, D3DXVECTOR3 &vd, float f,
		DWORD dwState, DWORD dwStateFlag, DWORD dwMotion, int nMotionEx,
		int nLoop, DWORD dwMotionOption, __int64 nTickCount )
{
	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_MOVERMOVED;
	ar << v << vd << f;
	ar << dwState << dwStateFlag << dwMotion << nMotionEx;
	ar << nLoop << dwMotionOption << nTickCount;
	
	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pMover )
		if( USERPTR != pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void CUserMng::AddMoverBehavior( CMover* pMover, D3DXVECTOR3 & v, D3DXVECTOR3 &vd, float f,
		DWORD dwState, DWORD dwStateFlag, DWORD dwMotion, int nMotionEx,
		int nLoop, DWORD dwMotionOption, __int64 nTickCount, BOOL fTransferToMe )
{
	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_MOVERBEHAVIOR;
	ar << v << vd << f;
	ar << dwState << dwStateFlag << dwMotion << nMotionEx;
	ar << nLoop << dwMotionOption << nTickCount;
	
	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pMover )
		if( fTransferToMe || USERPTR != pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void CUserMng::AddMoverMoved2( CMover* pMover, D3DXVECTOR3 & v, D3DXVECTOR3 &vd, float f,
		float fAngleX, float fAccPower, float fTurnAngle,
		DWORD dwState, DWORD dwStateFlag, DWORD dwMotion, int nMotionEx,
		int nLoop, DWORD dwMotionOption, __int64 nTickCount, BYTE nFrame )
{
	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_MOVERMOVED2;
	ar << v << vd << f << fAngleX << fAccPower << fTurnAngle;
	ar << dwState << dwStateFlag << dwMotion << nMotionEx;
	ar << nLoop << dwMotionOption << nTickCount;
	ar << nFrame;
	
	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pMover )
		if( USERPTR != pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void CUserMng::AddMoverBehavior2( CMover* pMover, D3DXVECTOR3 & v, D3DXVECTOR3 &vd, float f,
		float fAngleX, float fAccPower, float fTurnAngle,
		DWORD dwState, DWORD dwStateFlag, DWORD dwMotion, int nMotionEx,
		int nLoop, DWORD dwMotionOption, __int64 nTickCount, BOOL fTransferToMe )
{
	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_MOVERBEHAVIOR2;
	ar << v << vd << f << fAngleX << fAccPower << fTurnAngle;
	ar << dwState << dwStateFlag << dwMotion << nMotionEx;
	ar << nLoop << dwMotionOption << nTickCount;
	
	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pMover )
		if( fTransferToMe || USERPTR != pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void CUserMng::AddMoverAngle( CMover* pMover, D3DXVECTOR3 & v, D3DXVECTOR3 & vd,
							float f, float fAngleX, float fAccPower, float fTurnAngle, __int64 nTickCount )
{
	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_MOVERANGLE;
	ar << v << vd << f << fAngleX << fAccPower << fTurnAngle << nTickCount;
	
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pMover )
		if( USERPTR != pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}




void CUserMng::AddMoverCorr( CMover* pMover, D3DXVECTOR3 & v, D3DXVECTOR3 &vd, float f,
		DWORD dwState, DWORD dwStateFlag, DWORD dwMotion, int nMotionEx,
		int nLoop, DWORD dwMotionOption, __int64 nTickCount )
{
	if( pMover )
	{
		if( (pMover->m_pActMover->GetState() & OBJSTA_DMG_FLY) )
		{
			return;
		}
	}
	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_MOVERCORR;
	ar << v << vd << f;
	ar << dwState << dwStateFlag << dwMotion << nMotionEx;
	ar << nLoop << dwMotionOption << nTickCount;
	
	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pMover )
		if( USERPTR != pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void CUserMng::AddMoverCorr2( CMover* pMover, D3DXVECTOR3 & v, D3DXVECTOR3 &vd, float f,
		float fAngleX, float fAccPower, float fTurnAngle,
		DWORD dwState, DWORD dwStateFlag, DWORD dwMotion, int nMotionEx,
		int nLoop, DWORD dwMotionOption, __int64 nTickCount )
{
	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_MOVERCORR2;
	ar << v << vd << f << fAngleX << fAccPower << fTurnAngle;
	ar << dwState << dwStateFlag << dwMotion << nMotionEx;
	ar << nLoop << dwMotionOption << nTickCount;
	
	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pMover )
		if( USERPTR != pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

// x, y, z ; 절대좌표에 오브젝트 생성.
void CUserMng::AddCreateSfxObj( CCtrl* pCtrl, DWORD dwSfxObj, float x, float y, float z, BOOL bFlag )
{
	CAr ar;
	ar << GETID( pCtrl ) << SNAPSHOTTYPE_CREATESFXOBJ;

	ar << dwSfxObj << x << y << z;
	ar << bFlag;

	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pCtrl )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pCtrl )
}

#if __VER >= 11 // __MA_VER11_06				// 확율스킬 효과수정 world,neuz
void CUserMng::AddRemoveSfxObj( CCtrl* pCtrl, DWORD dwSfxObj, float x, float y, float z, BOOL bFlag )
{
	CAr ar;
	ar << GETID( pCtrl ) << SNAPSHOTTYPE_REMOVESFXOBJ;

	ar << dwSfxObj << x << y << z;
	ar << bFlag;

	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pCtrl )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pCtrl )
}
#endif // __MA_VER11_06				// 확율스킬 효과수정 world,neuz


void CUserMng::AddMotionArrive( CMover* pMover, OBJMSG objmsg )
{
	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_MOTION_ARRIVE;
	
	ar << objmsg;
	
	GETBLOCK( ar, lpBuf, nBufSize );
	FOR_VISIBILITYRANGE( pMover )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

#if __VER >= 8 // __S8_PK
void CUserMng::AddPKPink( CMover* pMover, BYTE byPink )
{
	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_PK_RELATION;
	ar << PK_PINK;
	ar << byPink;
	
	GETBLOCK( ar, lpBuf, nBufSize );
	FOR_VISIBILITYRANGE( pMover )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}
void CUserMng::AddPKPropensity( CMover* pMover )
{
	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_PK_RELATION;
	ar << PK_PROPENSITY;
	ar << pMover->GetPKPropensity();

	GETBLOCK( ar, lpBuf, nBufSize );
	FOR_VISIBILITYRANGE( pMover )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}
#endif // __VER >= 8 // __S8_PK

void CUserMng::AddWorldCreateSfxObj( DWORD dwSfxObj, float x, float y, float z, BOOL bFlag, DWORD dwWorldId )
{
	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_CREATESFXOBJ << dwSfxObj << x << y << z << bFlag ;
	GETBLOCK( arBlock, lpBlock, uBlockSize );
	CWorld* pWorld	= g_WorldMng.GetWorld( dwWorldId );
	AddBlockNoLock( lpBlock, uBlockSize, pWorld );
}

void	CUserMng::AddCreateSfxAllow( CMover *pMover, DWORD dwSfxObjArrow, DWORD dwSfxObjHit, D3DXVECTOR3 vPosDest, int idTarget )
{
	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_CREATESFXALLOW;
	ar << dwSfxObjArrow << dwSfxObjHit << vPosDest.x << vPosDest.y << vPosDest.z << idTarget;
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pMover )
		if( USERPTR != pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void CUserMng::AddNearSetChangeJob( CMover* pMover, int nJob, LPSKILL lpSkill )
{
	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_SET_NEAR_JOB_SKILL;
	ar << nJob;
#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
	if( pMover->IsHero() )
		ar.Write( (void*)&pMover->m_aJobSkill[MAX_EXPERT_SKILL ], sizeof(SKILL) *  ( MAX_PRO_SKILL + MAX_MASTER_SKILL ) );
	else if( pMover->IsMaster() )
		ar.Write( (void*)&pMover->m_aJobSkill[MAX_EXPERT_SKILL ], sizeof(SKILL) *  ( MAX_PRO_SKILL + MAX_MASTER_SKILL ) );
	else
		ar.Write( (void*)&pMover->m_aJobSkill[MAX_JOB_SKILL], sizeof(SKILL) *  ( MAX_EXPERT_SKILL ) );
#else //__LEGEND	//	10차 전승시스템	Neuz, World, Trans
	ar.Write( (void*)&pMover->m_aJobSkill[MAX_JOB_SKILL], sizeof(SKILL) *  ( MAX_EXPERT_SKILL ) );
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans

	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pMover )
		if( USERPTR != pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void CUserMng::AddModifyMode( CUser* pUser )
{
	CAr ar;
	ar << GETID( pUser ) << SNAPSHOTTYPE_MODIFYMODE;
	ar << pUser->m_dwMode;
	
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pUser )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pUser )
}

void CUserMng::AddStateMode( CUser* pUser, BYTE nFlag )
{
	CAr ar;
	ar << GETID( pUser ) << SNAPSHOTTYPE_STATEMODE;
	ar << pUser->m_dwStateMode;
	ar << nFlag;
	if( nFlag == STATEMODE_BASEMOTION_ON )
	{
		ItemProp* pItemProp = NULL;
		if( pUser->m_bItemFind )
		{
			CItemElem* pItemElem = ((CMover*)pUser)->m_Inventory.GetAtId( pUser->m_dwUseItemId );
			if( pItemElem )
			{
				pItemProp = pItemElem->GetProp();
			}
		}
		else
		{
			pItemProp = prj.GetItemProp( pUser->m_dwUseItemId );
		}

		if( pItemProp )
		{
			ar << pItemProp->dwID;
		}
		else
		{
			Error( "나오면 안됨.. 시전" );
			return;
		}
	}
	
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pUser )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pUser )
}

#if __VER < 8 // __S8_PK
void CUserMng::AddSetSlaughterPoint( CMover* pMover, int nVal, int nNumKill )
{
	CAr ar;
	
	ar << GETID( pMover ) << SNAPSHOTTYPE_SET_SLAUGHTER_POINT;
	ar << nVal << nNumKill;
	
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pMover )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}
#endif // __VER < 8 // __S8_PK

void CUserMng::AddSetFame( CMover* pMover, int nFame )
{
	CAr ar;
	
	ar << GETID( pMover ) << SNAPSHOTTYPE_SETFAME;
	ar << nFame;
	
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pMover )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void CUserMng::AddSetLocalEvent( short id, BYTE nState )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_SET_LOCAL_EVENT;
	ar << id << nState;
	GETBLOCK( ar, lpBuf, uBufSize );

	AddBlock( lpBuf, uBufSize );	// all
}

void CUserMng::AddGameRate( FLOAT fRate, BYTE nFlag )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_GAMERATE;
	ar << fRate;
	ar << nFlag;
	GETBLOCK( ar, lpBuf, uBufSize );
	
	AddBlock( lpBuf, uBufSize );	// all
}

void CUserMng::AddChangeFace( u_long uidPlayer, DWORD dwFace )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_CHANGEFACE;
	ar << uidPlayer << dwFace;
	GETBLOCK( ar, lpBuf, uBufSize );

	AddBlock( lpBuf, uBufSize );	// all
}

void CUserMng::AddSchoolReport( PSCHOOL_ENTRY pSchool, short nElapse )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_SCHOOL_REPORT;
	ar.Write( pSchool, sizeof(SCHOOL_ENTRY) * MAX_SCHOOL );
	ar << nElapse;

	GETBLOCK( ar, lpBuf, uBufSize );
	AddBlock( lpBuf, uBufSize );	// all
}

void CUserMng::AddSexChange( CMover* pMover )
{
	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_SEX_CHANGE;
	ar << (u_short)pMover->GetIndex();
	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pMover )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void CUserMng::AddCreateGuild( u_long idPlayer, const char* lpszPlayer, u_long idGuild, const char* szGuild )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_CREATE_GUILD;
	ar << idPlayer << idGuild;
	ar.WriteString( lpszPlayer? lpszPlayer: "" );
	ar.WriteString( szGuild );
	GETBLOCK( ar, lpBuf, uBufSize );

	AddBlock( lpBuf, uBufSize );	// all
}

void CUserMng::AddDestroyGuild( const char* lpszPlayer, u_long idGuild )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_DESTROY_GUILD;
	ar.WriteString( lpszPlayer? lpszPlayer: "" );
	ar << idGuild;
	GETBLOCK( ar, lpBuf, uBufSize );

	AddBlock( lpBuf, uBufSize );	// all
}

void CUserMng::AddSetGuild( CUser* pUser, u_long idGuild )
{
	CAr ar;
	ar << GETID( pUser ) << SNAPSHOTTYPE_SET_GUILD;
	ar << idGuild;
	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pUser )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pUser )
}

void CUserMng::AddSetWar( CUser* pUser, u_long idWar )
{
	CAr ar;
	ar << GETID( pUser ) << SNAPSHOTTYPE_SET_WAR;
	ar << idWar;
	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pUser )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pUser )
}

//모든 유저에게 알리는 버젼 
void CUserMng::AddSetLogo( u_long idGuild, DWORD dwLogo )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_GUILD_LOGO;
	ar << idGuild << dwLogo;

	GETBLOCK( ar, lpBuf, uBufSize );
	AddBlock( lpBuf, uBufSize );	// all
}

void CUserMng::AddGetItemElem( CUser* pUser, CItemElem* pItemElem )
{
	CAr ar;

	ar << GETID( pUser ) << SNAPSHOTTYPE_GETITEMGUILDBANK;
	ar << (BYTE)3;
	pItemElem->Serialize( ar );
	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pUser )
	if ( USERPTR != pUser && USERPTR->m_bGuildBank == TRUE && USERPTR->m_idGuild == pUser->m_idGuild)
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pUser )
}

void CUserMng::AddPutItemElem( CUser* pUser, CItemElem* pItemElem )
{
	CAr ar;
	
	ar << GETID( pUser ) << SNAPSHOTTYPE_PUTITEMGUILDBANK;
	ar << (BYTE)3;
	pItemElem->Serialize( ar );
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pUser )
		if (  USERPTR != pUser && USERPTR->m_bGuildBank == TRUE && USERPTR->m_idGuild == pUser->m_idGuild)
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pUser )
}

#if __VER >= 8 // __GUILDCOMBAT_85
void CUserMng::AddPutItemElem( u_long uidGuild, CItemElem* pItemElem )
{
	CAr ar;
	
	ar << NULL_ID << SNAPSHOTTYPE_PUTITEMGUILDBANK;
	ar << (BYTE)3;
	pItemElem->Serialize( ar );
	GETBLOCK( ar, lpBuf, nBufSize );
	
	map<DWORD, CUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		CUser* pUsertmp = it->second;
		if( pUsertmp->IsValid() == FALSE )
			continue;
		
		if( pUsertmp->m_idGuild == uidGuild )
			pUsertmp->AddBlock( lpBuf, nBufSize );
	}
}
#endif // __VER >= 8

void CUserMng::AddPVendorOpen( CUser* pUser )
{
	CAr ar;
	ar << GETID( pUser ) << SNAPSHOTTYPE_PVENDOR_OPEN;
	ar.WriteString( pUser->m_vtInfo.GetTitle() );
	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pUser )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pUser )
}

void CUserMng::AddPVendorClose( CUser* pUser )
{
	CAr ar;
	ar << GETID( pUser ) << SNAPSHOTTYPE_PVENDOR_CLOSE;
	ar << (BYTE)1;
	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pUser )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pUser )
}

#if __VER >= 11 // __MOD_VENDOR
void CUserMng::AddPVendorItemNum( CUser* pUser, BYTE nItem, short nVend, const char* sBuyer )
#else	// __MOD_VENDOR
void CUserMng::AddPVendorItemNum( CUser* pUser, BYTE nItem, short nVend )
#endif	// __MOD_VENDOR
{
	CAr ar;
	ar << GETID( pUser ) << SNAPSHOTTYPE_PVENDOR_ITEM_NUM;
	ar << nItem << nVend;
#if __VER >= 11 // __MOD_VENDOR
	ar.WriteString( sBuyer );
#endif	// __MOD_VENDOR
	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pUser )
		if( USERPTR == pUser || USERPTR->m_vtInfo.GetOther() == pUser )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pUser )
}

void CUserMng::AddSetHair( CUser* pUser, BYTE nHair, BYTE r, BYTE g, BYTE b )
{
	CAr ar;
	ar << GETID( pUser ) << SNAPSHOTTYPE_SET_HAIR;
	ar << nHair << r << g << b;
	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pUser )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pUser )
}

void CUserMng::AddHdr( CCtrl* pCtrl, WORD wHdr )
{
	CAr ar;
	ar << GETID( pCtrl ) << wHdr;

	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pCtrl )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pCtrl )
}

void CUserMng::AddBlock( LPBYTE lpBlock, u_long uBlockSize )
{
	map<DWORD, CUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		CUser* pUser = it->second;
		if( pUser->IsValid() == FALSE )
			continue;

		if( pUser->GetWorld() )
			pUser->AddBlock( lpBlock, uBlockSize );
	}

}

void CUserMng::AddBlock( LPBYTE lpBlock, u_long uBlockSize, CWorld* pWorld )
{
	if( !pWorld )
		return;

	map<DWORD, CUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		CUser* pUser = it->second;
		if( pUser->IsValid() == FALSE )
			continue;

		if( pUser->GetWorld() == pWorld )
			pUser->AddBlock( lpBlock, uBlockSize );
	}
}

void CUserMng::AddBlockNoLock( LPBYTE lpBlock, u_long uBlockSize )
{
	map<DWORD, CUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		CUser* pUser = it->second;
		if( pUser->IsValid() == FALSE )
			continue;

		if( pUser->GetWorld() )
			pUser->AddBlock( lpBlock, uBlockSize );
	}
}

void CUserMng::AddBlockNoLock( LPBYTE lpBlock, u_long uBlockSize, CWorld* pWorld )
{
	if( !pWorld )
		return;

	map<DWORD, CUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		CUser* pUser = it->second;
		if( pUser->IsValid() == FALSE )
			continue;

		if( pUser->GetWorld() == pWorld )
			pUser->AddBlock( lpBlock, uBlockSize );
	}
}

void CUserMng::AddBlock( CWorld* pWorld, const D3DXVECTOR3 & vPos, int nRange, LPBYTE lpBlock, u_long uBlockSize )
{
	if( !pWorld )
		return;

	float d	= (float)( nRange * nRange );
	D3DXVECTOR3 vtmp;

	map<DWORD, CUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		CUser* pUser = it->second;
		if( pUser->IsValid() == FALSE )
			continue;

		vtmp = vPos - pUser->GetPos();
		if( pUser->GetWorld() == pWorld && D3DXVec3LengthSq( &vtmp ) < d )
			pUser->AddBlock( lpBlock, uBlockSize );
	}
}

void CUserMng::AddGameSetting( void )
{
	map<DWORD, CUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		CUser* pUser = it->second;
		if( pUser->IsValid() == FALSE )
			continue;
		
		pUser->AddGameSetting();
	}
}
#if __VER >= 13
void CUserMng::AddShout( CUser* pUserSrc, int nRange, LPBYTE lpBlock, u_long uBlockSize )
#else // __VER >= 13
void CUserMng::AddShout( const D3DXVECTOR3 & vPos, int nRange, LPBYTE lpBlock, u_long uBlockSize )
#endif // __VER >= 13
{
	float fRange = (float)( nRange * nRange );
	D3DXVECTOR3 v;
	
	map<DWORD, CUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		CUser* pUser = it->second;
		if( pUser->IsValid() == FALSE )
			continue;
		if( pUser->GetWorld() == NULL )
			continue;
#if __VER >= 13
		if( nRange > 0  )
		{
			v = pUserSrc->GetPos() - pUser->GetPos();
			if(	pUser->GetWorld() != pUserSrc->GetWorld() || pUser->GetLayer() != pUserSrc->GetLayer()
				|| D3DXVec3LengthSq( &v ) > fRange
				)
				continue;
		}

		pUser->AddBlock( lpBlock, uBlockSize );
#else // __VER >= 13
		v = vPos - pUser->GetPos();
		if( nRange == 0 || D3DXVec3LengthSq( &v ) < fRange )
		{
			pUser->AddBlock( lpBlock, uBlockSize );
		}
#endif // __VER >= 13
	}
}


void CUserMng::AddWorldMsg( const CRect* pRect, LPCTSTR lpszString )
{
	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_WORLDMSG;
	arBlock.WriteString( lpszString );
	GETBLOCK( arBlock, lpBlock, uBlockSize );

	D3DXVECTOR3 vtmp;
	
	map<DWORD, CUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		CUser* pUser = it->second;
		if( pUser->IsValid() == FALSE )
			continue;

		vtmp = pUser->GetPos();
		if( pUser->GetWorld() )
		{
			if( pRect == NULL )
				pUser->AddBlock( lpBlock, uBlockSize );
			else if( pRect->PtInRect( CPoint( (int) vtmp.x, (int) vtmp.y ) ) )
				pUser->AddBlock( lpBlock, uBlockSize );
		}
	}
}


// pMover를 fScalePercent로 줄인다.
// ex)
// pMover가 클라에서 2.0f 2.0f 2.0f 스케일 상태였는데 fScalePercent가 0.5가 넘어갔다면
// 2.0 * 0.5 = 1.0f     1.0, 1.0, 1.0으로 스케일 조정된다
void CUserMng::AddSetScale( CMover* pMover, float fScalePercent )
{
	CAr ar;
	
	ar << GETID( pMover );
	ar << SNAPSHOTTYPE_SETSCALE;	
	ar << fScalePercent;
	
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pMover )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

// pCenter : 이놈을 중심으로 한 유저들에게 보낸다
// pTarget : SkillState를 적용할 실제 대상무버.
void CUserMng::AddSetSkillState( CMover* pCenter, CMover *pTarget, WORD wType, WORD wID, DWORD dwLevel, DWORD dwTime  )
{
	CAr ar;
	
	ar << GETID( pCenter );
	ar << SNAPSHOTTYPE_SETSKILLSTATE;	
	ar << pTarget->GetId() << wType << wID;
#if __VER >= 11 // __SYS_COLLECTING
	if( wType == BUFF_ITEM2 )
	{
		time_t t	= (time_t)dwLevel - time_null();
		ar << t;
	}
	else
		ar << dwLevel;
#else	// __SYS_COLLECTING
	ar << dwLevel;
#endif	// __SYS_COLLECTING
	ar << dwTime;
	
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pCenter )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pCenter )
}

#ifdef __S1108_BACK_END_SYSTEM
void CUserMng::AddMonsterProp()
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_MONSTERPROP;

	ar << prj.m_nAddMonsterPropSize;
	for( int i = 0 ; i < prj.m_nAddMonsterPropSize ; ++i )
	{
		ar.WriteString( prj.m_aAddProp[ i ].szMonsterName );
		ar << prj.m_aAddProp[ i ].nHitPoint;
		ar << prj.m_aAddProp[ i ].nAttackPower;
		ar << prj.m_aAddProp[ i ].nDefence;
		ar << prj.m_aAddProp[ i ].nExp;
		ar << prj.m_aAddProp[ i ].nItemDrop;
		ar << prj.m_aAddProp[ i ].nPenya;
	}
	
	ar << prj.m_nRemoveMonsterPropSize;
	for( i = 0 ; i < prj.m_nRemoveMonsterPropSize ; ++i )
	{
		ar.WriteString( prj.m_aRemoveProp[i] );
	}

	GETBLOCK( ar, lpBuf, uBufSize );
	AddBlock( lpBuf, uBufSize );	// all
}

void CUserMng::AddGMChat( int nSize )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_GMCHAT;
	ar << nSize;
	for( int i = 0 ; i < nSize ; ++i )
	{
		ar.WriteString( prj.m_chGMChat[i] );
	}

	GETBLOCK( ar, lpBuf, uBufSize );
	AddBlock( lpBuf, uBufSize );	// all
}
#endif // __S1108_BACK_END_SYSTEM

void CUserMng::AddGuildCombatUserState( CMover* pMover )
{
	CAr ar;
	ar << GETID( pMover );
	ar << SNAPSHOTTYPE_GUILDCOMBAT;
	ar << GC_USERSTATE;
	ar << pMover->GetId() << pMover->m_nGuildCombatState;
	
	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pMover )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void CUserMng::AddGCWinGuild( void )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_GUILDCOMBAT << GC_WINGUILD;
	ar << g_GuildCombatMng.m_nGuildCombatIndex << g_GuildCombatMng.m_uWinGuildId << g_GuildCombatMng.m_nWinGuildCount;
	
	GETBLOCK( ar, lpBuf, uBufSize );
	AddBlockNoLock( lpBuf, uBufSize );	// all
}
void CUserMng::AddGCBestPlayer( void )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_GUILDCOMBAT << GC_BESTPLAYER;
	ar << g_GuildCombatMng.m_uBestPlayer;
	
	GETBLOCK( ar, lpBuf, uBufSize );
	AddBlockNoLock( lpBuf, uBufSize );	// all
}

void CUserMng::AddGuildCombatState( void )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_GUILDCOMBAT;
	ar << GC_GCSTATE;
	ar << g_GuildCombatMng.m_nState << g_GuildCombatMng.m_nGCState;
	
	GETBLOCK( ar, lpBuf, uBufSize );
	AddBlockNoLock( lpBuf, uBufSize );	// all
}

void CUserMng::AddGuildCombatNextTimeWorld( DWORD dwTime, DWORD dwState )
{
	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_GUILDCOMBAT;
	arBlock << GC_NEXTTIMESTATE;
	arBlock << dwTime << dwState;
	GETBLOCK( arBlock, lpBlock, uBlockSize );
	
	map<DWORD, CUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		CUser* pUser = it->second;
		if( pUser->IsValid() == FALSE )
			continue;
		
		pUser->AddBlock( lpBlock, uBlockSize );
	}
}
void CUserMng::AddGuildCombatEnterTime( DWORD dwTime )
{
	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_GUILDCOMBAT;
	arBlock << GC_ENTERTIME;
	arBlock << dwTime ;
	GETBLOCK( arBlock, lpBlock, uBlockSize );
	
	map<DWORD, CUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		CUser* pUser = it->second;
		if( pUser->IsValid() == FALSE )
			continue;

		pUser->AddBlock( lpBlock, uBlockSize );
	}
}

void CUserMng::AddGCWarPlayerlist( u_long uidGuild, CUser* pSendUser )
{
	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_GUILDCOMBAT;
	arBlock << GC_WARPLAYERLIST;
	g_GuildCombatMng.SerializeGCWarPlayerList( arBlock );
	GETBLOCK( arBlock, lpBlock, uBlockSize );
	
	if( pSendUser == NULL )
	{
		map<DWORD, CUser*>::iterator it;
		for( it = m_users.begin(); it != m_users.end(); ++it )
		{
			CUser* pUser = it->second;
			if( pUser->IsValid() == FALSE )
				continue;
			
			if( uidGuild )
			{
				if( uidGuild == pUser->m_idGuild )
				{
					pUser->AddBlock( lpBlock, uBlockSize );
				}			
			}
			else if( pUser->GetWorld()->GetID() == WI_WORLD_GUILDWAR )
			{
				pUser->AddBlock( lpBlock, uBlockSize );
			}
		}
	}
	else
	{
		pSendUser->AddBlock( lpBlock, uBlockSize );
	}
}
void CUserMng::AddGCLogWorld( void )
{
	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_GUILDCOMBAT;
	arBlock << GC_LOG;
	arBlock << (u_long)g_GuildCombatMng.m_vecGCGetPoint.size();
	for( int veci = 0 ; veci < (int)( g_GuildCombatMng.m_vecGCGetPoint.size() ) ; ++veci )
	{
		CGuildCombat::__GCGETPOINT GCGetPoint = g_GuildCombatMng.m_vecGCGetPoint[ veci ];
		arBlock << GCGetPoint.uidGuildAttack;
		arBlock << GCGetPoint.uidGuildDefence;
		arBlock << GCGetPoint.uidPlayerAttack;
		arBlock << GCGetPoint.uidPlayerDefence;
		arBlock << GCGetPoint.nPoint;
		arBlock << GCGetPoint.bKillDiffernceGuild;
		arBlock << GCGetPoint.bMaster;
		arBlock << GCGetPoint.bDefender;
		arBlock << GCGetPoint.bLastLife;
	}
	GETBLOCK( arBlock, lpBlock, uBlockSize );

	map<DWORD, CUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		CUser* pUser = it->second;
		if( pUser->IsValid() == FALSE )
			continue;
		
		if( pUser->GetWorld()->GetID() == WI_WORLD_GUILDWAR )
			pUser->AddBlock( lpBlock, uBlockSize );
	}
}
void CUserMng::AddGCLogRealTimeWorld( CGuildCombat::__GCGETPOINT GCGetPoint )
{
	LPCSTR szAttacker, szDefender;
#if __VER >= 11 // __SYS_PLAYER_DATA
	szAttacker	= CPlayerDataCenter::GetInstance()->GetPlayerString( GCGetPoint.uidPlayerAttack );
	szDefender	= CPlayerDataCenter::GetInstance()->GetPlayerString( GCGetPoint.uidPlayerDefence );
#else	// __SYS_PLAYER_DATA
	szAttacker = prj.GetPlayerString( GCGetPoint.uidPlayerAttack );
	szDefender = prj.GetPlayerString( GCGetPoint.uidPlayerDefence );
#endif	// __SYS_PLAYER_DATA
	if( szAttacker == NULL || szDefender == NULL )
		return;

	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_GUILDCOMBAT;
	arBlock << GC_LOG_REALTIME;
	arBlock << GCGetPoint.uidGuildAttack;
	arBlock << GCGetPoint.uidGuildDefence;
	arBlock.WriteString( szAttacker );
	arBlock.WriteString( szDefender );
	arBlock << GCGetPoint.uidPlayerAttack;
	arBlock << GCGetPoint.uidPlayerDefence;
	arBlock << GCGetPoint.nPoint;
	arBlock << GCGetPoint.bKillDiffernceGuild;
	arBlock << GCGetPoint.bMaster;
	arBlock << GCGetPoint.bDefender;
	arBlock << GCGetPoint.bLastLife;
	GETBLOCK( arBlock, lpBlock, uBlockSize );
	
	map<DWORD, CUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		CUser* pUser = it->second;
		if( pUser->IsValid() == FALSE )
			continue;
		
		if( pUser->GetWorld()->GetID() == WI_WORLD_GUILDWAR )
			pUser->AddBlock( lpBlock, uBlockSize );
	}
}

#ifdef __LAYER_1015
void CUserMng::ReplaceWorld( DWORD dwWorldId, DWORD dwReplaceWorldId, float fReplaceX, float fReplaceZ, int nLayer )
#else	// __LAYER_1015
void CUserMng::ReplaceWorld( DWORD dwWorldId, DWORD dwReplaceWorldId, float fReplaceX, float fReplaceZ )
#endif	// __LAYER_1015
{
	CWorld* pWorld	= g_WorldMng.GetWorld( dwWorldId );
	if( pWorld )
	{
		map<DWORD, CUser*>::iterator it;
		for( it = m_users.begin(); it != m_users.end(); ++it )
		{
			CUser* pUser = it->second;
			if( pUser->IsValid() == FALSE )
				continue;
			
			if( pUser->GetWorld() == pWorld )
			{
				D3DXVECTOR3 v3Pos = pUser->GetPos();
				AddCreateSfxObj( (CMover*)pUser, XI_GEN_WEARF, v3Pos.x, v3Pos.y, v3Pos.z );
				( (CMover*)pUser)->REPLACE( g_uIdofMulti, dwReplaceWorldId, D3DXVECTOR3( fReplaceX, 0.0f, fReplaceZ ), REPLACE_NORMAL, nLayer );
				pUser->m_vtInfo.SetOther( NULL );
				AddCreateSfxObj( (CMover*)pUser, XI_GEN_WEARF, fReplaceX, v3Pos.y, fReplaceZ );				
			}
		}
	}
}

#ifdef __LAYER_1015
void CUserMng::ReplaceWorldArea( u_long idParty, DWORD dwWorldId, DWORD dwReplaceWorldId, float fReplaceX, float fReplaceZ, float fArea, BOOL bDieFlag, int nLayer )
#else	// __LAYER_1015
void CUserMng::ReplaceWorldArea( u_long idParty, DWORD dwWorldId, DWORD dwReplaceWorldId, float fReplaceX, float fReplaceZ, float fArea, BOOL bDieFlag )
#endif	// __LAYER_1015
{
	CWorld* pWorld	= g_WorldMng.GetWorld( dwWorldId );
	if( pWorld )
	{
		for( int i = 0; i < (int)( pWorld->m_dwObjNum ); i++ )
		{
			CObj* pObj	= pWorld->m_apObject[i];
			if( IsValidObj( pObj ) && pObj->GetType() == OT_MOVER && ( (CMover*)pObj )->IsPlayer() && ( (CMover*)pObj )->m_idparty == idParty )
			{
				CUser* pUser	= (CUser*)pObj;
				if( bDieFlag )
				{
					if( pUser->IsDie() )
						continue;
				}

				D3DXVECTOR3 vPos	= pUser->GetPos();
				AddCreateSfxObj( pUser, XI_GEN_WEARF, vPos.x, vPos.y, vPos.z );
				float fNewArea	= fArea * 2.0f;
				fReplaceX += (-fArea) + xRandomF( fNewArea );
				fReplaceZ += (-fArea) + xRandomF( fNewArea );
				pUser->REPLACE( g_uIdofMulti, dwReplaceWorldId, D3DXVECTOR3( fReplaceX, 0.0f, fReplaceZ ), REPLACE_NORMAL, nLayer );
				AddCreateSfxObj( (CMover*)pUser, XI_GEN_WEARF, fReplaceX, vPos.y, fReplaceZ );
			}
		}
	}
}

void CUserMng::AddWorldMsg( DWORD dwWorldId, LPCTSTR lpszString )
{
	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_WORLDMSG;
	arBlock.WriteString( lpszString );
	GETBLOCK( arBlock, lpBlock, uBlockSize );
	
	map<DWORD, CUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		CUser* pUser = it->second;
		if( pUser->IsValid() == FALSE )
			continue;
		if( pUser->GetWorld()->GetID() == dwWorldId )
			pUser->AddBlock( lpBlock, uBlockSize );
	}
}

void CUserMng::AddGuildMsg( u_long idGuild, LPCSTR lpsz )
{
	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
	if( pGuild )
		AddGuildMsg( pGuild, lpsz );
}

void CUserMng::AddGuildMsg( CGuild* pGuild, LPCSTR lpsz )
{
	CGuildMember*	pMember;
	CUser*			pUsertmp;
	
	for( map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin();
	i != pGuild->m_mapPMember.end(); ++i )
	{
		pMember		= i->second;
		pUsertmp	= (CUser*)prj.GetUserByID( pMember->m_idPlayer );
		if( IsValidObj( pUsertmp ) )
			pUsertmp->AddText( lpsz );
	}
}

void CUserMng::AddGCIsRequest( u_long uidGuild, BOOL bRequest )
{
	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_GUILDCOMBAT;
	arBlock << GC_ISREQUEST;
	arBlock << bRequest;

	GETBLOCK( arBlock, lpBlock, uBlockSize );
	map<DWORD, CUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		CUser* pUsertmp = it->second;
		if( pUsertmp->IsValid() == FALSE )
			continue;
		
		if( pUsertmp->m_idGuild == uidGuild )
			pUsertmp->AddBlock( lpBlock, uBlockSize );
	}
}
void CUserMng::AddGCGuildStatus( u_long uidGuild, CUser* pSendUser )
{
	if( g_GuildCombatMng.m_nState == CGuildCombat::CLOSE_STATE )
		return;

	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_GUILDCOMBAT;
	arBlock << GC_GUILDSTATUS;
	
#ifdef __S_BUG_GC
	CGuildCombat::__GuildCombatMember* pGCMember = g_GuildCombatMng.FindGuildCombatMember( uidGuild );
	if( pGCMember != NULL )
	{
		// 순서에 맞게 vecPlayerList에 넣기 : 선수 -> 대기자(들어갈수 있음) -> 대기자(못들어감)
		list<CGuildCombat::__JOINPLAYER*> lspPlyaerList;
		lspPlyaerList.clear();

		CGuildCombat::__JOINPLAYER* pJoinPlayer;
		// 대기자(들어갈수 잇음) 넣음
		for( list<CGuildCombat::__JOINPLAYER*>::iterator i1 = pGCMember->lspFifo.begin(); i1 != pGCMember->lspFifo.end(); ++i1 )
		{
			pJoinPlayer = *i1;
			lspPlyaerList.push_back( pJoinPlayer );
		}

		// 선수 및 대기자(못들어감) 넣음
		for( int veci = pGCMember->vecGCSelectMember.size()-1 ; veci >= 0 ; --veci )
		{
			pJoinPlayer = pGCMember->vecGCSelectMember[veci];
			if( 0 < pJoinPlayer->nlife )	// 선수
			{
				BOOL bFind = FALSE;
				for( i1 = pGCMember->lspFifo.begin(); i1 != pGCMember->lspFifo.end(); ++i1 )
				{
					if( pJoinPlayer == *i1 )
					{
						bFind	= TRUE;
						break;
					}
				}				
				if( bFind == FALSE )
				{
					lspPlyaerList.push_front( pJoinPlayer );
				}
			}
			else // 대기자(못들어감)
			{
				lspPlyaerList.push_back( pJoinPlayer );
			}
		}

		// 보내기
		if( 0 < pGCMember->lspFifo.size() )
			arBlock << g_GuildCombatMng.m_nMaxWarPlayer;
		else
			arBlock << (int)0;
		
		arBlock << (int)lspPlyaerList.size();
		for( i1 = lspPlyaerList.begin(); i1 != lspPlyaerList.end(); ++i1 )
		{
			pJoinPlayer = *i1;
			arBlock << pJoinPlayer->uidPlayer;
			arBlock << pJoinPlayer->nlife;
		}	
	}
#else // __S_BUG_GC
	map<u_long, CGuildCombat::__GuildCombatMember*>::iterator itGuild = g_GuildCombatMng.m_GuildCombatMem.find( uidGuild );
	if( itGuild != g_GuildCombatMng.m_GuildCombatMem.end() )
	{
		CGuildCombat::__GuildCombatMember* pGCMember	= itGuild->second;
		
		// 순서에 맞게 vecPlayerList에 넣기 : 선수 -> 대기자(들어갈수 있음) -> 대기자(못들어감)
		list<CGuildCombat::__JOINPLAYER*> lspPlyaerList;
		lspPlyaerList.clear();

		CGuildCombat::__JOINPLAYER* pJoinPlayer;

		// 대기자(들어갈수 잇음) 넣음
		for( list<CGuildCombat::__JOINPLAYER*>::iterator i1 = pGCMember->lspFifo.begin(); i1 != pGCMember->lspFifo.end(); ++i1 )
		{
			pJoinPlayer = *i1;
			lspPlyaerList.push_back( pJoinPlayer );
		}

		// 선수 및 대기자(못들어감) 넣음
		for( int veci = pGCMember->vecGCSelectMember.size()-1 ; veci >= 0 ; --veci )
		{
			pJoinPlayer = pGCMember->vecGCSelectMember[veci];
			if( 0 < pJoinPlayer->nlife )	// 선수
			{
				BOOL bFind = FALSE;
				for( i1 = pGCMember->lspFifo.begin(); i1 != pGCMember->lspFifo.end(); ++i1 )
				{
					if( pJoinPlayer == *i1 )
					{
						bFind	= TRUE;
						break;
					}
				}				
				if( bFind == FALSE )
				{
					lspPlyaerList.push_front( pJoinPlayer );
				}
			}
			else // 대기자(못들어감)
			{
				lspPlyaerList.push_back( pJoinPlayer );
			}
		}

		// 보내기
		if( 0 < pGCMember->lspFifo.size() )
			arBlock << g_GuildCombatMng.m_nMaxWarPlayer;
		else
			arBlock << (int)0;
		
		arBlock << (int)lspPlyaerList.size();
		for( i1 = lspPlyaerList.begin(); i1 != lspPlyaerList.end(); ++i1 )
		{
			pJoinPlayer = *i1;
			arBlock << pJoinPlayer->uidPlayer;
			arBlock << pJoinPlayer->nlife;
		}	
	}
#endif // __S_BUG_GC
	else
	{
		arBlock << (int)0;
		arBlock << (int)0;
	}

	GETBLOCK( arBlock, lpBlock, uBlockSize );
	if( pSendUser == NULL )
	{
		map<DWORD, CUser*>::iterator it;
		for( it = m_users.begin(); it != m_users.end(); ++it )
		{
			CUser* pUser = it->second;
			if( pUser->IsValid() == FALSE )
				continue;
			
			if( pUser->m_idGuild == uidGuild )
			{
				pUser->AddBlock( lpBlock, uBlockSize );
			}
		}
	}
	else
	{
		pSendUser->AddBlock( lpBlock, uBlockSize );
	}
}
void CUserMng::AddGCGuildPrecedence( CUser* pSendUser )
{
	if( g_GuildCombatMng.m_nState == CGuildCombat::CLOSE_STATE )
		return;

	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_GUILDCOMBAT;
	arBlock << GC_GUILDPRECEDENCE;

#ifdef __S_BUG_GC
	arBlock << (int)g_GuildCombatMng.m_vecGuildCombatMem.size();
	for( int gcmi = 0 ; gcmi < (int)( g_GuildCombatMng.m_vecGuildCombatMem.size() ) ; ++gcmi )
	{
		CGuildCombat::__GuildCombatMember* pGCMember = g_GuildCombatMng.m_vecGuildCombatMem[gcmi];
		if( 0 < pGCMember->vecGCSelectMember.size() )
		{			
			arBlock << (BOOL)TRUE; // bSend;
			CGuild* pGuild = g_GuildMng.GetGuild( pGCMember->uGuildId );
			if( pGuild )
				arBlock.WriteString( pGuild->m_szGuild );
			else
				arBlock.WriteString( "Not Guild" );
			arBlock << pGCMember->nGuildPoint;
		}
		else
		{
			arBlock << (BOOL)FALSE; // bSend;
		}
	}
#else // __S_BUG_GC
	arBlock << (int)g_GuildCombatMng.m_GuildCombatMem.size();
	for( map<u_long, CGuildCombat::__GuildCombatMember*>::iterator itGuild = g_GuildCombatMng.m_GuildCombatMem.begin();
		itGuild != g_GuildCombatMng.m_GuildCombatMem.end(); ++itGuild )
	{
		CGuildCombat::__GuildCombatMember* pGCMember = itGuild->second;
		if( 0 < pGCMember->vecGCSelectMember.size() )
		{			
			arBlock << (BOOL)TRUE; // bSend;
			CGuild* pGuild = g_GuildMng.GetGuild( itGuild->first );
			if( pGuild )
				arBlock.WriteString( pGuild->m_szGuild );
			else
				arBlock.WriteString( "Not Guild" );
			arBlock << pGCMember->nGuildPoint;
		}
		else
		{
			arBlock << (BOOL)FALSE; // bSend;
		}		
	}
#endif // __S_BUG_GC

	GETBLOCK( arBlock, lpBlock, uBlockSize );
	if( pSendUser == NULL )
	{
		map<DWORD, CUser*>::iterator it;
		for( it = m_users.begin(); it != m_users.end(); ++it )
		{
			CUser* pUser = it->second;
			if( pUser->IsValid() == FALSE )
				continue;
			if( pUser->GetWorld()->GetID() == WI_WORLD_GUILDWAR )
				pUser->AddBlock( lpBlock, uBlockSize );
		}
	}
	else
	{
		pSendUser->AddBlock( lpBlock, uBlockSize );
	}
}
void CUserMng::AddGCPlayerPrecedence( CUser* pSendUser )
{
	if( g_GuildCombatMng.m_nState == CGuildCombat::CLOSE_STATE )
		return;
	
	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_GUILDCOMBAT;
	arBlock << GC_PLAYERPRECEDENCE;

#ifdef __S_BUG_GC
	arBlock << (int)g_GuildCombatMng.m_vecGuildCombatMem.size();
	for( int gcmi = 0 ; gcmi < (int)( g_GuildCombatMng.m_vecGuildCombatMem.size() ) ; ++gcmi )
	{
		CGuildCombat::__GuildCombatMember* pGCMember = g_GuildCombatMng.m_vecGuildCombatMem[gcmi];
		arBlock << (int)pGCMember->vecGCSelectMember.size();
		for( int veci = 0 ; veci < (int)( pGCMember->vecGCSelectMember.size() ) ; ++veci )
		{
			CGuildCombat::__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];
			arBlock << pJoinPlayer->uidPlayer;
			arBlock << pJoinPlayer->nPoint;
		}
	}
#else // __S_BUG_GC
	arBlock << (int)g_GuildCombatMng.m_GuildCombatMem.size();
	for( map<u_long, CGuildCombat::__GuildCombatMember*>::iterator itGuild = g_GuildCombatMng.m_GuildCombatMem.begin();
	itGuild != g_GuildCombatMng.m_GuildCombatMem.end(); ++itGuild )
	{
		CGuildCombat::__GuildCombatMember* pGCMember = itGuild->second;
		arBlock << (int)pGCMember->vecGCSelectMember.size();
		for( int veci = 0 ; veci < pGCMember->vecGCSelectMember.size() ; ++veci )
		{
			CGuildCombat::__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];
			arBlock << pJoinPlayer->uidPlayer;
			arBlock << pJoinPlayer->nPoint;
		}
	}
#endif // __S_BUG_GC

	GETBLOCK( arBlock, lpBlock, uBlockSize );
	
	if( pSendUser == NULL )
	{
		map<DWORD, CUser*>::iterator it;
		for( it = m_users.begin(); it != m_users.end(); ++it )
		{
			CUser* pUser = it->second;
			if( pUser->IsValid() == FALSE )
				continue;
			
			if( pUser->GetWorld()->GetID() == WI_WORLD_GUILDWAR )
			{
				pUser->AddBlock( lpBlock, uBlockSize );
			}
		}
	}
	else
	{
		pSendUser->AddBlock( lpBlock, uBlockSize );
	}
}

void CUser::AddRunScriptFunc( const RunScriptFunc & runScriptFunc )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_RUNSCRIPTFUNC;
	m_Snapshot.ar << runScriptFunc.wFuncType;
	switch( runScriptFunc.wFuncType )
	{
		case FUNCTYPE_ADDKEY:
		case FUNCTYPE_ADDANSWER:
			{
				m_Snapshot.ar.WriteString( runScriptFunc.lpszVal1 );
				m_Snapshot.ar.WriteString( runScriptFunc.lpszVal2 );
				m_Snapshot.ar << runScriptFunc.dwVal1;
				m_Snapshot.ar << runScriptFunc.dwVal2;
				break;
			}
		case FUNCTYPE_REMOVEKEY:
			{
				m_Snapshot.ar.WriteString( runScriptFunc.lpszVal1 );
				break;
			}
		case FUNCTYPE_SAY:
		case FUNCTYPE_SAYQUEST:
			{
				m_Snapshot.ar.WriteString( runScriptFunc.lpszVal1 );
				m_Snapshot.ar << runScriptFunc.dwVal2;
				break;
			}
		case FUNCTYPE_INITSTAT:
		case FUNCTYPE_INITSTR:
		case FUNCTYPE_INITSTA:			
		case FUNCTYPE_INITDEX:
		case FUNCTYPE_INITINT:
			{
				m_Snapshot.ar << runScriptFunc.dwVal1;
				break;
			}
		case FUNCTYPE_SETNAVIGATOR:
			{
				m_Snapshot.ar << runScriptFunc.dwVal1;
				m_Snapshot.ar << runScriptFunc.vPos;
				break;
			}
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
		case FUNCTYPE_NEWQUEST:
		case FUNCTYPE_CURRQUEST:
			{
				m_Snapshot.ar.WriteString( runScriptFunc.lpszVal1 );
				m_Snapshot.ar.WriteString( runScriptFunc.lpszVal2 );
				m_Snapshot.ar << runScriptFunc.dwVal1;
				m_Snapshot.ar << runScriptFunc.dwVal2;
				break;
			}
#endif // __IMPROVE_QUEST_INTERFACE
		default:
			break;
	}
	
}

void CUser::AddEnterChatting( CUser* pUser )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CHATTING;
	m_Snapshot.ar << CHATTING_ENTERMEMBER;
	m_Snapshot.ar << pUser->m_idPlayer;
	m_Snapshot.ar.WriteString( pUser->GetName() );
	
}

void CUser::AddNewChatting( CChatting* pChatting )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CHATTING;
	m_Snapshot.ar << CHATTING_NEWCHATTING;
	m_Snapshot.ar << pChatting->GetChattingMember();
	for( int i = 0 ; i < pChatting->GetChattingMember() ; ++i )
	{
		m_Snapshot.ar << pChatting->m_idMember[i];
#if __VER >= 11 // __SYS_PLAYER_DATA
		if( NULL == CPlayerDataCenter::GetInstance()->GetPlayerString( pChatting->m_idMember[i] ) )
#else	// __SYS_PLAYER_DATA
		if( NULL == prj.GetPlayerString( pChatting->m_idMember[i] ) )
#endif	// __SYS_PLAYER_DATA
		{
			Error( "AddNewChatting : MAX_ChattingMember = %d, GetMember = %d", pChatting->GetChattingMember(), pChatting->m_idMember[i] );
			m_Snapshot.ar.WriteString( "" );
		}
		else
		{
#if __VER >= 11 // __SYS_PLAYER_DATA
			m_Snapshot.ar.WriteString( CPlayerDataCenter::GetInstance()->GetPlayerString( pChatting->m_idMember[i] ) );
#else	//__SYS_PLAYER_DATA
			m_Snapshot.ar.WriteString( prj.GetPlayerString( pChatting->m_idMember[i] ) );
#endif	// __SYS_PLAYER_DATA
		}
	}
}

#if __VER >= 8 //__CSC_VER8_5
/*
void CUser::AddCreateAngel(BOOL isSuccess, char* createAngel)
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ANGEL;
	m_Snapshot.ar << ANGEL_WNDCREATE;
	m_Snapshot.ar << isSuccess;
	m_Snapshot.ar.WriteString( createAngel );
}
*/

void CUser::AddAngelInfo( BOOL bComplete )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ANGEL;
	m_Snapshot.ar << ANGEL_INFO;
	m_Snapshot.ar << m_nAngelExp;
	m_Snapshot.ar << m_nAngelLevel;
	m_Snapshot.ar << bComplete;
}
#endif //__CSC_VER8_5

void CUser::AddRemoveChatting( u_long uidPlayer )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CHATTING;
	m_Snapshot.ar << CHATTING_REMOVEMEMBER;
	m_Snapshot.ar << uidPlayer;
	
}

void CUser::AddDeleteChatting()
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CHATTING;
	m_Snapshot.ar << CHATTING_DELETECHATTING;
	
}

void CUser::AddChatting( u_long uidPlayer, char* pszChat )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CHATTING;
	m_Snapshot.ar << CHATTING_CHATTING;
	m_Snapshot.ar << uidPlayer;
	m_Snapshot.ar.WriteString( pszChat );
	
}

void CUser::AddChttingRoomState( BOOL bState )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CHATTING;
	m_Snapshot.ar << CHATTING_CHATTINGROOMSTATE;
	m_Snapshot.ar << bState;
	
}

void CUser::AddGameRate( FLOAT fRate, BYTE nFlag )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GAMERATE;
	m_Snapshot.ar << fRate;
	m_Snapshot.ar << nFlag;
	
}


void CUser::AddGameSetting()
{
	if( IsDelete() )	return;

	AddGameRate( prj.m_fShopCost, GAME_RATE_SHOPCOST );
	AddGameRate( (float)( prj.m_dwVagSP ), GAME_SKILL_VAGSP );
	AddGameRate( (float)( prj.m_dwExpertSP ), GAME_SKILL_EXPERTSP );
	AddGameRate( (float)( prj.m_dwProSP ), GAME_SKILL_PROSP );
#ifdef __SHOP_COST_RATE
	AddGameRate( prj.m_EventLua.GetShopBuyFactor(), GAME_RATE_SHOP_BUY );
	AddGameRate( prj.m_EventLua.GetShopSellFactor(), GAME_RATE_SHOP_SELL );
#endif // __SHOP_COST_RATE
	if( ((CMover*)this)->IsAuthHigher( AUTH_GAMEMASTER ) )
	{
		AddGameRate( prj.m_fItemDropRate, GAME_RATE_ITEMDROP );
		AddGameRate( prj.m_fGoldDropRate, GAME_RATE_GOLDDROP );
		AddGameRate( prj.m_fMonsterExpRate, GAME_RATE_MONSTEREXP );
		AddGameRate( prj.m_fMonsterHitRate, GAME_RATE_MONSTERHIT );
		
#ifdef __S1108_BACK_END_SYSTEM
		AddGameRate( prj.m_fMonsterRebirthRate, GAME_RATE_REBIRTH );
		AddGameRate( prj.m_fMonsterHitpointRate, GAME_RATE_HITPOINT );
		AddGameRate( prj.m_fMonsterAggressiveRate, GAME_RATE_AGGRESSIVE );
#endif // __S1108_BACK_END_SYSTEM
	}	
}

#ifdef __S1108_BACK_END_SYSTEM
void CUser::AddMonsterProp()
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_MONSTERPROP;

	m_Snapshot.ar << prj.m_nMonsterPropSize;
	for( int i = 0 ; i < prj.m_nMonsterPropSize ; ++i )
	{
		m_Snapshot.ar.WriteString( prj.m_aMonsterProp[ i ].szMonsterName );
		m_Snapshot.ar << prj.m_aMonsterProp[ i ].nHitPoint;
		m_Snapshot.ar << prj.m_aMonsterProp[ i ].nAttackPower;
		m_Snapshot.ar << prj.m_aMonsterProp[ i ].nDefence;
		m_Snapshot.ar << prj.m_aMonsterProp[ i ].nExp;
		m_Snapshot.ar << prj.m_aMonsterProp[ i ].nItemDrop;
		m_Snapshot.ar << prj.m_aMonsterProp[ i ].nPenya;
	}
	
	m_Snapshot.ar << prj.m_nRemoveMonsterPropSize;
	for( i = 0 ; i < prj.m_nRemoveMonsterPropSize ; ++i )
	{
		m_Snapshot.ar.WriteString( prj.m_aRemoveProp[i] );
	}

	
}
#endif // __S1108_BACK_END_SYSTEM

void CUser::AddInitSkill()
{

	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_INITSKILLPOINT;
	m_Snapshot.ar << m_nSkillPoint;
}


void CUser::AddDoUseSkillPoint( SKILL aJobSkill[], int nSkillPoint )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_DOUSESKILLPOINT;

	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
	{
		LPSKILL lpSkill;
		lpSkill = &aJobSkill[ i ];
		
		m_Snapshot.ar << lpSkill->dwSkill << lpSkill->dwLevel;
	}
	m_Snapshot.ar << nSkillPoint;
}

void CUser::AddWantedInfo( const D3DXVECTOR3& vPos, BYTE byOnline, DWORD dwWorldID, LPCTSTR lpszWorld )
{
	if( IsDelete() )	return;

	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_WANTED_INFO;
	m_Snapshot.ar << vPos;
	m_Snapshot.ar << byOnline;
	m_Snapshot.ar << dwWorldID;		
	m_Snapshot.ar.WriteString( lpszWorld );
	
}

void CUserMng::AddCommonPlace( CCtrl* pCtrl, BYTE nType )
{
	CAr ar;
	
	ar << GETID( pCtrl ) << SNAPSHOTTYPE_COMMONPlACE;
	ar << nType;
	
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pCtrl )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pCtrl )
}

void CUser::AddReturnScroll()
{
	if( IsDelete() )	return;

	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_RETURNSCORLL;
	
}

void CUserMng::OutputStatistics( void )
{
#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
	int	acbUser[MAX_LEGEND_LEVEL];
	
	int cb	= 0;
	int nTotal	= 0;

	memset( acbUser, 0, sizeof(int) * MAX_LEGEND_LEVEL );

	map<DWORD, CUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		CUser* pUser = it->second;
		if( pUser->IsValid() == FALSE )
			continue;

		if( pUser->GetLevel() >= 1 && pUser->GetLevel() <= MAX_LEGEND_LEVEL )
		{
			acbUser[pUser->GetLevel() - 1]++;
			cb++;
			nTotal	+= pUser->GetLevel();
		}
	}

	static char lpOutputString[4096];
	*lpOutputString	= '\0';
	for( int i = 0; i < MAX_LEGEND_LEVEL; i++ )
	{
		if( acbUser[i] == 0 )
			continue;
		char lpString[32]	= { 0, };
		sprintf( lpString, "%d\t%d\n", i+1, acbUser[i] );
		lstrcat( lpOutputString, lpString );
	}

	if( cb > 0 )
	{
		char lpOverview[32]	= { 0, };
		sprintf( lpOverview, "U=%d, A=%d", cb, nTotal / cb );
		lstrcat( lpOutputString, lpOverview );
	}

	CTime time	= CTime::GetCurrentTime();
	FILEOUT( time.Format( "../statistics%Y%m%d%H%M%S.txt" ), lpOutputString );
#else //__LEGEND	//	10차 전승시스템	Neuz, World, Trans

	int	acbUser[MAX_GENERAL_LEVEL];
	int cb	= 0;
	int nTotal	= 0;

	memset( acbUser, 0, sizeof(int) * MAX_GENERAL_LEVEL );

	map<DWORD, CUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		CUser* pUser = it->second;
		if( pUser->IsValid() == FALSE )
			continue;

		if( pUser->GetLevel() >= 1 && pUser->GetLevel() <= MAX_GENERAL_LEVEL )
		{
			acbUser[pUser->GetLevel() - 1]++;
			cb++;
			nTotal	+= pUser->GetLevel();
		}
	}

	static char lpOutputString[4096];
	*lpOutputString	= '\0';
	for( int i = 0; i < MAX_GENERAL_LEVEL; i++ )
	{
		if( acbUser[i] == 0 )
			continue;
		char lpString[32]	= { 0, };
		sprintf( lpString, "%d\t%d\n", i+1, acbUser[i] );
		lstrcat( lpOutputString, lpString );
	}

	if( cb > 0 )
	{
		char lpOverview[32]	= { 0, };
		sprintf( lpOverview, "U=%d, A=%d", cb, nTotal / cb );
		lstrcat( lpOutputString, lpOverview );
	}

	CTime time	= CTime::GetCurrentTime();
	FILEOUT( time.Format( "../statistics%Y%m%d%H%M%S.txt" ), lpOutputString );
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans

}

void CUser::AddPostMail( CMail* pMail )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_POSTMAIL;
	pMail->Serialize( m_Snapshot.ar );
	
}

void CUser::AddRemoveMail( u_long nMail, int nType )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_REMOVEMAIL;
	m_Snapshot.ar << nMail << nType;
	
}

void CUser::AddMailBox( CMailBox* pMailBox )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_QUERYMAILBOX;
	pMailBox->Serialize( m_Snapshot.ar );
	
}

void CUser::SendCheckMailBoxReq( BOOL bCheckTransMailBox )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_QUERYMAILBOX_REQ;
	m_Snapshot.ar << bCheckTransMailBox;
}


void CUser::CheckTransMailBox( BOOL bCheckTransMailBox )
{
	m_bCheckTransMailBox = bCheckTransMailBox;
}

BOOL CUser::GetCheckTransMailBox()
{
	return m_bCheckTransMailBox;
}

bool CUser::CheckClientReq()
{
	DWORD dwTick = GetTickCount();
	if( dwTick >= m_dwTickFromClient + CHECK_TICK_FROM_CLIENT )
	{
		m_dwTickFromClient = dwTick;
		++m_nCountFromClient;
		return true;
	}
	return false;
}

void CUser::ResetCheckClientReq()
{
	m_dwTickFromClient = 0;
	m_nCountFromClient = 0;
}

int CUser::GetCountClientReq()
{
	return m_nCountFromClient;
}

// 신청 윈도우 띄움
void CUser::AddGCWindow( __int64 nPrizePenya, DWORD dwRequstPenya, DWORD dwMinRequestPenya )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_IN_WINDOW;
	m_Snapshot.ar << nPrizePenya << dwRequstPenya << dwMinRequestPenya;
	
}

void CUser::AddGCRequestStatus( __int64 nPrizePenya, vector<CGuildCombat::__REQUESTGUILD> vecRequestStatus )
{
	DWORD dwRequstPenya = 0;
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_REQUEST_STATUS;
	CTime tNextTime = g_GuildCombatMng.GetNextGuildCobmatTime();
	time_t d = (time_t)( tNextTime.GetTime() - time_null() );
	m_Snapshot.ar << d;
	m_Snapshot.ar << nPrizePenya;
	int nSize = vecRequestStatus.size();
	m_Snapshot.ar << nSize;
	for( int i = 0 ; i < nSize ; ++i )
	{
		CGuildCombat::__REQUESTGUILD RequestGuild = vecRequestStatus[i];
		CGuild* pGuild = g_GuildMng.GetGuild( RequestGuild.uidGuild );
		if( pGuild )
			m_Snapshot.ar.WriteString( pGuild->m_szGuild );
		else
			m_Snapshot.ar.WriteString( "NotGuild" );

		if( m_idGuild == RequestGuild.uidGuild )
			dwRequstPenya = RequestGuild.dwPenya;
	}	
	m_Snapshot.ar << dwRequstPenya;
	
}
// 선택 캐릭터 윈도우 띄움
void CUser::AddGCSelectPlayerWindow( vector<CGuildCombat::__JOINPLAYER> &vecSelectPlayer, u_long uidDefender, BOOL bWindow, BOOL bRequestWar )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_SELECTPLAYER;
	m_Snapshot.ar << g_GuildCombatMng.m_nMaxJoinMember;
	m_Snapshot.ar << g_GuildCombatMng.m_nMaxWarPlayer;

	m_Snapshot.ar << bRequestWar;
	if( bRequestWar )
	{
		m_Snapshot.ar << bWindow;
		if( bWindow )
		{
			m_Snapshot.ar << uidDefender;
			int nSize = vecSelectPlayer.size();
			m_Snapshot.ar << nSize;
			for( int i = 0 ; i < nSize ; ++i )
			{
				CGuildCombat::__JOINPLAYER JoinPlayer = vecSelectPlayer[i];
				m_Snapshot.ar << JoinPlayer.uidPlayer;
			}	
		}
	}
	
}
// 전투위치 윈도우 띄움
void CUser::AddGCJoinWarWindow( int nMap, int nTelTime )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_SELECTWARPOS;
	m_Snapshot.ar << nMap;
	m_Snapshot.ar << nTelTime;
	
}
void CUser::AddGCWinGuild( void )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_WINGUILD;
	m_Snapshot.ar << g_GuildCombatMng.m_nGuildCombatIndex << g_GuildCombatMng.m_uWinGuildId << g_GuildCombatMng.m_nWinGuildCount;
	
}
void CUser::AddGCBestPlayer( void )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_BESTPLAYER;
	m_Snapshot.ar << g_GuildCombatMng.m_uBestPlayer;
	
}
void CUser::AddGCIsRequest( BOOL bRequest )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_ISREQUEST;
	m_Snapshot.ar << bRequest;
	
}
void CUser::AddGCGetPenyaGuild( int nGetResult, __int64 nGetPenya )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_GETPENYAGUILD;
	m_Snapshot.ar << nGetResult;
	m_Snapshot.ar << nGetPenya;
	
}
void CUser::AddGCGetPenyaPlayer( int nGetResult, __int64 nGetPenya )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_GETPENYAPLAYER;
	m_Snapshot.ar << nGetResult;
	m_Snapshot.ar << nGetPenya;
	
}
void CUser::AddGCDiagMessage( const char* str )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_DIAGMESSAGE;
	m_Snapshot.ar.WriteString( str );
	
}
void CUser::AddGCTele( const char* str )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_TELE;
	m_Snapshot.ar.WriteString( str );
	
}
void CUser::AddGuildCombatNextTime( DWORD dwTime, DWORD dwState )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_NEXTTIMESTATE;
	m_Snapshot.ar << dwTime << dwState;
	
}
void CUser::AddGuildCombatEnterTime( DWORD dwTime )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_ENTERTIME;
	m_Snapshot.ar << dwTime ;
	
}

void CUser::AddGCWarPlayerlist()
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_WARPLAYERLIST;
	g_GuildCombatMng.SerializeGCWarPlayerList( m_Snapshot.ar );
	
}

void CUser::AddCtrlCoolTimeCancel()
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_EXPBOXCOLLTIMECANCEL;
	
}

void CUser::AddGuildCombatState( void )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_GCSTATE;
	m_Snapshot.ar << g_GuildCombatMng.m_nState << g_GuildCombatMng.m_nGCState;
	
}
void CUser::AddGCLog( void )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_LOG;
	m_Snapshot.ar << (u_long)g_GuildCombatMng.m_vecGCGetPoint.size();
	for( int veci = 0 ; veci < (int)( g_GuildCombatMng.m_vecGCGetPoint.size() ) ; ++veci )
	{
		CGuildCombat::__GCGETPOINT GCGetPoint = g_GuildCombatMng.m_vecGCGetPoint[ veci ];
		m_Snapshot.ar << GCGetPoint.uidGuildAttack;
		m_Snapshot.ar << GCGetPoint.uidGuildDefence;
		m_Snapshot.ar << GCGetPoint.uidPlayerAttack;
		m_Snapshot.ar << GCGetPoint.uidPlayerDefence;
		m_Snapshot.ar << GCGetPoint.nPoint;
		m_Snapshot.ar << GCGetPoint.bKillDiffernceGuild;
		m_Snapshot.ar << GCGetPoint.bMaster;
		m_Snapshot.ar << GCGetPoint.bDefender;
		m_Snapshot.ar << GCGetPoint.bLastLife;
	}
	
}
void CUser::AddGCPlayerPoint( void )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_PLAYERPOINT;
	m_Snapshot.ar << (u_long)g_GuildCombatMng.m_vecGCPlayerPoint.size();
	for( int veci = 0 ; veci < (int)( g_GuildCombatMng.m_vecGCPlayerPoint.size() ) ; ++veci )
	{
		CGuildCombat::__GCPLAYERPOINT GCPlayerPoint = g_GuildCombatMng.m_vecGCPlayerPoint[ veci ];
		m_Snapshot.ar << GCPlayerPoint.uidPlayer;
		m_Snapshot.ar << GCPlayerPoint.nJob;
		m_Snapshot.ar << GCPlayerPoint.nPoint;
	}	
}
void CUser::AddQuestTextTime( BOOL bFlag, int nState, DWORD dwTime )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_QUEST_TEXT_TIME;
	m_Snapshot.ar << bFlag;
	m_Snapshot.ar << nState;
	m_Snapshot.ar << dwTime;
	
}

void CUser::AddFocusObj(OBJID objid)
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_FOCUSOBJ;
	m_Snapshot.ar << objid;
	
}

void CUser::OnMsgArrival( DWORD dwParam )
{
	CCtrl* pObj = prj.GetCtrl( dwParam );
	if( NULL == pObj )
		return;

	if( pObj->GetType() == OT_ITEM )
	{
		if( IsFly() )
			return;

		if( IsMode( ITEM_MODE ) )
			return;

		CItem* pItem = (CItem*)pObj;

		//pItem이 집을수 있는 아이템인가 검사.
		if( IsLoot( pItem ) == FALSE )	// 집을 수 없는 경우
		{
			AddDefinedText( TID_GAME_PRIORITYITEMPER, "\"%s\"", pItem->GetProp()->szName );	// %는 다른사람 아이템임다.
			return;
		}
		//pItem을 줏음.
		if( DoLoot( pItem ) == FALSE )
		{
			return;
		}
		g_UserMng.AddMotion( this, OBJMSG_PICKUP );		// 줍는 모션
	}
	else
	if( pObj->GetType() == OT_MOVER )
	{
	}
	else if( pObj->GetType() == OT_CTRL )
	{
		if( IsValidObj( (CObj*)pObj ) )
		{
			CCommonCtrl*  pCommonCtrl = (CCommonCtrl*)pObj;
			if( !pCommonCtrl )
				return;

			CItemElem* pKey = NULL;

			// 오브젝트 동작 조건 검사
			if( pCommonCtrl->m_CtrlElem.m_dwSet & UA_PLAYER_ID )
			{
				if( m_idPlayer != pCommonCtrl->m_idExpPlayer )
				{
					AddDefinedText( TID_GAME_NOT_EXPBOX );
					AddClearTarget(); //Focus된 Ctrl을 해제하도록 전송.
					return;
				}

				if( pCommonCtrl->m_bAction == TRUE )
				{
					AddClearTarget(); //Focus된 Ctrl을 해제하도록 전송.
					return;
				}		
				
				if( pCommonCtrl->m_dwCtrlReadyTime != 0xffffffff )
				{
					AddClearTarget(); //Focus된 Ctrl을 해제하도록 전송.
					return;
				}
				
				pCommonCtrl->m_dwCtrlReadyTime = timeGetTime() + OPEN_COOLTIME;
				m_dwCtrlReadyTime        = timeGetTime() + OPEN_COOLTIME;
				m_dwCtrlReadyId  = pCommonCtrl->GetId();						
				
				AddExpBoxCoolTime( pCommonCtrl->GetId(), pCommonCtrl->m_dwCtrlReadyTime - timeGetTime() );
			}

			if( pCommonCtrl->m_CtrlElem.m_dwSet & UA_ITEM )
			{
				// 아이템과 아이템 개수와 내구력이 둘중 있어야 열수 있음
				BOOL bResult = FALSE;
				pKey = m_Inventory.GetAtItemId( pCommonCtrl->m_CtrlElem.m_dwSetItem );
				if( IsUsableItem( pKey ) )
				{
					int nItemCount = m_Inventory.GetAtItemNum( pCommonCtrl->m_CtrlElem.m_dwSetItem );	// 아이템 갯수 꺼내기						
					if( nItemCount < (int)( pCommonCtrl->m_CtrlElem.m_dwSetItemCount ) && pCommonCtrl->m_CtrlElem.m_dwSetEndu == 0 )	
						bResult = TRUE;
				}
				else
				{
					if( pCommonCtrl->m_CtrlElem.m_dwSetEndu == 0 )
						bResult = TRUE;
				}
				
				if( bResult )
				{
					AddDefinedText( TID_ITEMBOX_MESSAGE1, "\"%s\"", prj.GetItemProp(pCommonCtrl->m_CtrlElem.m_dwSetItem)->szName );
					AddClearTarget(); //Focus된 Ctrl을 해제하도록 전송.
					return;
				}							
			}
			if( pCommonCtrl->m_CtrlElem.m_dwSet & UA_LEVEL )
			{
				if( GetLevel() < (int)( pCommonCtrl->m_CtrlElem.m_dwSetLevel ) )
				{
					AddDefinedText( TID_ITEMBOX_MESSAGE2 );
					AddClearTarget(); //Focus된 Ctrl을 해제하도록 전송.
					return;
				}
			}
			if( pCommonCtrl->m_CtrlElem.m_dwSet & UA_QUEST )
			{
				LPQUEST pQuest = GetQuest( pCommonCtrl->m_CtrlElem.m_dwSetQuestNum );

				if( pQuest )
				{
					if( pQuest->m_nState != pCommonCtrl->m_CtrlElem.m_dwSetFlagNum )
					{
						QuestProp * pQuestPorp = prj.m_aPropQuest.GetAt( pQuest->m_wId );
						if( pQuestPorp )
							AddDefinedText( TID_ITEMBOX_MESSAGE4, "\"%s\"", pQuestPorp->m_szTitle );

						AddClearTarget(); //Focus된 Ctrl을 해제하도록 전송.
						return;
					}
				}
				else
				{
						QuestProp * pQuestPorp = prj.m_aPropQuest.GetAt( pCommonCtrl->m_CtrlElem.m_dwSetQuestNum );
						if( pQuestPorp )
							AddDefinedText( TID_ITEMBOX_MESSAGE4, "\"%s\"", pQuestPorp->m_szTitle );

						AddClearTarget(); //Focus된 Ctrl을 해제하도록 전송.
						return;
				}
			}
			if( pCommonCtrl->m_CtrlElem.m_dwSet & UA_CLASS )
			{
				if( GetJob() != pCommonCtrl->m_CtrlElem.m_bSetJob[GetJob()] )
				{
					AddDefinedText( TID_ITEMBOX_MESSAGE5 );
					AddClearTarget(); //Focus된 Ctrl을 해제하도록 전송.
					return;
				}
			}
			if( pCommonCtrl->m_CtrlElem.m_dwSet & UA_GENDER )
			{
				if( GetSex() != pCommonCtrl->m_CtrlElem.m_dwSetGender )
				{
					AddDefinedText( TID_ITEMBOX_MESSAGE3 );
					AddClearTarget(); //Focus된 Ctrl을 해제하도록 전송.
					return;
				}
			}

			if( this )	// ??
				pCommonCtrl->m_nMoverID = GetId();
		
			if( pCommonCtrl->m_CtrlElem.m_dwSet & UA_TELEPORT )
			{
				if( IsFly() == FALSE )
				{
					D3DXVECTOR3 vPos( (float)( pCommonCtrl->m_CtrlElem.m_dwTeleX ), (float)( pCommonCtrl->m_CtrlElem.m_dwTeleY ), (float)( pCommonCtrl->m_CtrlElem.m_dwTeleZ ) );
					REPLACE( g_uIdofMulti, pCommonCtrl->m_CtrlElem.m_dwTeleWorldId, vPos, REPLACE_NORMAL, nTempLayer );
				}
				else
				{
					AddDefinedText( TID_PK_NO_FLIGHTAREA );
				}
			}
			// 퀘스트
			if( pCommonCtrl->m_CtrlElem.m_dwSet & UA_QUEST_END )
			{
				if( pCommonCtrl->m_CtrlElem.m_dwSetQuestNum1 != 0 )
				{
					LPQUEST pQuest = GetQuest( pCommonCtrl->m_CtrlElem.m_dwSetQuestNum1 );
					if( pQuest )
					{
						QUEST quest;
						if( SetQuest( pQuest->m_wId, pCommonCtrl->m_CtrlElem.m_dwSetFlagNum1, &quest ) )
						{
							AddSetQuest( &quest );
						}
					}
				}
				if( pCommonCtrl->m_CtrlElem.m_dwSetQuestNum2 != 0 )
				{
					LPQUEST pQuest = GetQuest( pCommonCtrl->m_CtrlElem.m_dwSetQuestNum2 );
					if( pQuest )
					{
						QUEST quest;
						if( SetQuest( pQuest->m_wId, pCommonCtrl->m_CtrlElem.m_dwSetFlagNum2, &quest ) )
						{
							AddSetQuest( &quest );
						}
					}
				}
			}

			// 아이템 삭제
			if( pCommonCtrl->m_CtrlElem.m_dwSet & UA_ITEM )
			{
				int nItemCount = pCommonCtrl->m_CtrlElem.m_dwSetItemCount;
				for( int i = 0; i < 10 && nItemCount > 0; ++i )
				{
					pKey = m_Inventory.GetAtItemId( pCommonCtrl->m_CtrlElem.m_dwSetItem );
					if( IsUsableItem( pKey ) )
					{
						if( nItemCount <= pKey->m_nItemNum )
						{
							RemoveItem( (BYTE)pKey->m_dwObjId, (short)nItemCount );
							nItemCount = 0;
						}
						else
						{
							RemoveItem( (BYTE)pKey->m_dwObjId, (short)pKey->m_nItemNum );
							nItemCount -= pKey->m_nItemNum;
						}							
					}
				}
			}
			pCommonCtrl->SetActionPlay();
			AddClearTarget();
		}
	}
}


void CUser::CheckFiniteItem()
{
	for( int i = 0; i < m_Inventory.GetMax(); i++ )
	{
		CItemElem* pItemElem	= m_Inventory.GetAtId( i );
		if( pItemElem && pItemElem->IsExpiring() && !pItemElem->IsFlag( CItemElem::expired ) )
		{
			if( pItemElem->IsEatPet() )
			{
				if( IsUsingEatPet( pItemElem ) )
					InactivateEatPet();
				UpdateItem( (BYTE)( pItemElem->m_dwObjId ), UI_NUM, 0 );
			}
			else
			{
				ItemProp* pItemProp		= pItemElem->GetProp();
#ifdef 	__BLADELWEAPON0608	//	9th 블레이드 양손에 무기를 착용 시 추가 옵션은 오른손에 들고 있는 무기의 것만 적용이 되도록 수정
				if( pItemElem->m_dwObjIndex >= m_Inventory.GetSize() &&
					(pItemElem->m_dwObjIndex - m_Inventory.GetSize() ) != PARTS_LWEAPON  )	// equiped item
					ResetDestParamEquip( pItemProp, pItemElem );
#else //__BLADELWEAPON0608	//	9th 블레이드 양손에 무기를 착용 시 추가 옵션은 오른손에 들고 있는 무기의 것만 적용이 되도록 수정
				if( (int)( pItemElem->m_dwObjIndex ) >= (int)( m_Inventory.GetSize() ) )	// equiped item
					ResetDestParamEquip( pItemProp, pItemElem );
#endif	//__BLADELWEAPON0608	//	9th 블레이드 양손에 무기를 착용 시 추가 옵션은 오른손에 들고 있는 무기의 것만 적용이 되도록 수정

				pItemElem->SetFlag( CItemElem::expired );
				UpdateItem( (BYTE)( pItemElem->m_dwObjId ), UI_FLAG, MAKELONG( pItemElem->m_dwObjIndex, pItemElem->m_byFlag ) );
#if __VER >= 9	// __PET_0410
				// 먹이 주머니 기간 만료이고, 활성화 되어있는 경우에는 비 활성화 처리한다.
				if( pItemElem->m_dwItemId == II_SYS_SYS_SCR_PET_FEED_POCKET && pItemElem->m_dwKeepTime > 0 && HasBuff( BUFF_ITEM, (WORD)( pItemElem->m_dwItemId ) ) )
					RemoveBuff( BUFF_ITEM, (WORD)( pItemElem->m_dwItemId ) );
#endif	// __PET_0410
			}
		}
	}
}


void CUser::DoSMItemEquip( CItemElem* pItemElem, DWORD dwParts )
{
	BYTE nResistAttackLeft = (BYTE)0xff;
	BYTE nResistAttackRight = (BYTE)0xff;
	BYTE nResistDefense = (BYTE)0xff;
	BYTE nResist = 0;
	switch( pItemElem->m_nResistSMItemId )
	{
	case II_CHR_SYS_SCR_FIREASTONE:
		nResist = SAI79::FIRE;
		break;
	case II_CHR_SYS_SCR_WATEILSTONE:
		nResist = SAI79::WATER;
		break;
	case II_CHR_SYS_SCR_WINDYOSTONE:
		nResist = SAI79::WIND;
		break;
	case II_CHR_SYS_SCR_LIGHTINESTONE:
		nResist = SAI79::ELECTRICITY;
		break;
	case II_CHR_SYS_SCR_EARTHYSTONE:
		nResist = SAI79::EARTH;
		break;
	case II_CHR_SYS_SCR_DEFIREASTONE:
		nResist = SAI79::FIRE;
		break;
	case II_CHR_SYS_SCR_DEWATEILSTONE:
		nResist = SAI79::WATER;
		break;
	case II_CHR_SYS_SCR_DEWINDYOSTONE:
		nResist = SAI79::WIND;
		break;
	case II_CHR_SYS_SCR_DELIGHTINESTONE:
		nResist = SAI79::ELECTRICITY;
		break;
	case II_CHR_SYS_SCR_DEEARTHYSTONE:
		nResist = SAI79::EARTH;
		break;
	}

	if( dwParts == PARTS_LWEAPON )
	{
		nResistAttackLeft = nResist;
	}
	else if( dwParts == PARTS_RWEAPON )
	{
		nResistAttackRight = nResist;
	}
	else
	{
		nResistDefense = nResist;
	}

	ItemProp* pItemProp = NULL;
	pItemProp = prj.GetItemProp( pItemElem->m_nResistSMItemId );
	if( pItemProp )
	{
		if( nResistAttackLeft != (BYTE)0xff )
		{
			m_nAttackResistLeft = nResistAttackLeft;
			((CUser*)this)->AddResistSMMode( m_nAttackResistLeft, m_nAttackResistRight, m_nDefenseResist );
			SetSMMode( SM_RESIST_ATTACK_LEFT, pItemProp->dwCircleTime );
			g_dpDBClient.SendLogSMItemUse( "1", (CUser*)this, pItemElem, pItemElem->GetProp() );
			g_dpDBClient.SendLogSMItemUse( "1", (CUser*)this, NULL, pItemProp );
		}
		else
		if( nResistAttackRight != (BYTE)0xff )
		{
			m_nAttackResistRight = nResistAttackRight;
			((CUser*)this)->AddResistSMMode( m_nAttackResistLeft, m_nAttackResistRight, m_nDefenseResist );
			SetSMMode( SM_RESIST_ATTACK_RIGHT, pItemProp->dwCircleTime );
			g_dpDBClient.SendLogSMItemUse( "1", (CUser*)this, pItemElem, pItemElem->GetProp() );
			g_dpDBClient.SendLogSMItemUse( "1", (CUser*)this, NULL, pItemProp );
		}
		else
		if( nResistDefense != (BYTE)0xff )
		{
			m_nDefenseResist = nResistDefense;
			((CUser*)this)->AddResistSMMode( m_nAttackResistLeft, m_nAttackResistRight, m_nDefenseResist );
			SetSMMode( SM_RESIST_DEFENSE, pItemProp->dwCircleTime );
			g_dpDBClient.SendLogSMItemUse( "1", (CUser*)this, pItemElem, pItemElem->GetProp() );
			g_dpDBClient.SendLogSMItemUse( "1", (CUser*)this, NULL, pItemProp );
		}
	}
}

void CUser::DoSMItemUnEquip( CItemElem* pItemElem, DWORD dwParts )
{
	if( pItemElem->m_nResistSMItemId == II_CHR_SYS_SCR_FIREASTONE || pItemElem->m_nResistSMItemId == II_CHR_SYS_SCR_WATEILSTONE ||
		pItemElem->m_nResistSMItemId == II_CHR_SYS_SCR_WINDYOSTONE || pItemElem->m_nResistSMItemId == II_CHR_SYS_SCR_LIGHTINESTONE ||
		pItemElem->m_nResistSMItemId == II_CHR_SYS_SCR_EARTHYSTONE
		)
	{
		if( dwParts == PARTS_LWEAPON )
		{
			SetSMMode( SM_RESIST_ATTACK_LEFT, 1 );
			g_dpDBClient.SendLogSMItemUse( "2", (CUser*)this, pItemElem, pItemElem->GetProp() );
		}
		else
		{
			SetSMMode( SM_RESIST_ATTACK_RIGHT, 1 );
		}
		pItemElem->m_nResistSMItemId = 0;
		((CUser*)this)->AddCommercialElem( pItemElem->m_dwObjId, 0 );
		g_dpDBClient.SendLogSMItemUse( "2", (CUser*)this, pItemElem, pItemElem->GetProp() );
	}
	else
	if( pItemElem->m_nResistSMItemId == II_CHR_SYS_SCR_DEFIREASTONE || pItemElem->m_nResistSMItemId == II_CHR_SYS_SCR_DEWATEILSTONE ||
		pItemElem->m_nResistSMItemId == II_CHR_SYS_SCR_DEWINDYOSTONE || pItemElem->m_nResistSMItemId == II_CHR_SYS_SCR_DELIGHTINESTONE ||
		pItemElem->m_nResistSMItemId == II_CHR_SYS_SCR_DEEARTHYSTONE
		)
	{
		SetSMMode( SM_RESIST_DEFENSE, 1 );
		pItemElem->m_nResistSMItemId = 0;
		((CUser*)this)->AddCommercialElem( pItemElem->m_dwObjId, 0 );
		g_dpDBClient.SendLogSMItemUse( "2", (CUser*)this, pItemElem, pItemElem->GetProp() );
	}
}

void CUserMng::AddAddRegion( DWORD dwWorldId, REGIONELEM & re )
{
	CAr ar;
	
	ar << NULL_ID << SNAPSHOTTYPE_ADDREGION;
	ar << dwWorldId;
	ar.Write( &re, sizeof(re) );
	
	map<DWORD, CUser*>::iterator it;
	list<CUser*>	lspUser;
	list<CUser*>::iterator i;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		CUser* pUser = it->second;
		if( pUser->IsValid() == FALSE )
			continue;
		CWorld* pWorld	= pUser->GetWorld();
		POINT point	= {	(LONG)( pUser->GetPos().x ), (LONG)( pUser->GetPos().z ) };
		if( pWorld && pWorld->GetID() == dwWorldId && re.m_rect.PtInRect( point ) )
		{
			pUser->SetPosChanged( TRUE );
			lspUser.push_back( pUser );
		}
	}

	ar << (int)lspUser.size();
	for( i	= lspUser.begin(); i != lspUser.end(); ++i )
		ar << (*i)->GetId();

	GETBLOCK( ar, lpBuf, nBufSize );

	// transfer
	for( i	= lspUser.begin(); i != lspUser.end(); ++i )
		(*i)->AddBlock( lpBuf, nBufSize );

	lspUser.clear();
}

#ifdef __EVENT_1101
#ifdef __EVENT_1101_2
__int64 CUser::GetEventFlagBit( int nBit )
#else // __EVENT_1101_2
int CUser::GetEventFlagBit( int nBit )
#endif // __EVENT_1101_2
{
#ifdef __EVENT_1101_2
	return m_nEventFlag & ( (__int64)0x0000000000000001 << nBit );
#else // __EVENT_1101_2
	return m_dwEventFlag & ( 0x00000001 << nBit );
#endif // __EVENT_1101_2
}

int CUser::SetEventFlagBit( int nBit )
{
	int nCount	= 0;
#ifdef __EVENT_1101_2
	m_nEventFlag	|= ( (__int64)0x0000000000000001 << nBit );
	for( int i = 0; i < 63; i++ )
#else // __EVENT_1101_2
	m_dwEventFlag	|= 0x00000001 << nBit;
	for( int i = 0; i < 32; i++ )
#endif // __EVENT_1101_2

	{
		if( GetEventFlagBit( i ) )
			nCount++;
	}
	return nCount;
}

void CUser::AddCallTheRoll( void )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CALLTHEROLL;
#ifdef __EVENT_1101_2
	m_Snapshot.ar << m_nEventFlag;
#else // __EVENT_1101_2
	m_Snapshot.ar << m_dwEventFlag;
#endif // __EVENT_1101_2
}

typedef struct __ITEMINFO
{
	DWORD dwItemId;
	int nItemNum;
	int nBins;
	__ITEMINFO( DWORD dwII, int nIN, int nB )
	:dwItemId( dwII ), nItemNum( nIN ), nBins( nB ) {}
} ITEMINFO;

void CUserMng::CallTheRoll( int nBit )
{
	map<DWORD, CUser*>::iterator i;
	for( i = m_users.begin(); i != m_users.end(); ++i )
	{
		CUser* pUser = i->second;
		if( pUser->IsValid() == FALSE )
			continue;
		if( pUser->GetEventFlagBit( nBit ) )	// 0 or 1
			continue;
		
		if( pUser->GetLevel() < 16 )
		{
			continue;
		}

		if( (int)( pUser->m_dwEventElapsed ) < nBit )
		{
			pUser->m_dwEventElapsed	= nBit;
			pUser->m_dwEventTime	= 1;	// restart
		}
		else
		{
#ifdef __MAINSERVER
			if( ++pUser->m_dwEventTime >= 60 )
#else	// __MAINSERVER
			if( ++pUser->m_dwEventTime >= 1 )
#endif	// __MAINSERVER
			{
				int nCount	= pUser->SetEventFlagBit( nBit );
				pUser->AddCallTheRoll();	// print
				vector<ITEMINFO> vecItemInfo;
#ifdef __EVENT_1101_2
				// 매일 지급
				vecItemInfo.push_back( ITEMINFO( II_SYS_SYS_EVE_FUN01, 2, 0 ) );
				
				switch( nCount )
				{
					case 5:
						vecItemInfo.push_back( ITEMINFO( II_CHR_FOO_COO_REMANTIS, 3, 0 ) );
						break;

					case 10:
						vecItemInfo.push_back( ITEMINFO( II_CHR_FOO_COO_REMANTIS, 3, 0 ) );
						vecItemInfo.push_back( ITEMINFO( II_SYS_SYS_SCR_BX_PET_LEYENA07, 1, 0 ) );
						break;

					case 19:
						vecItemInfo.push_back( ITEMINFO( II_CHR_FOO_COO_REMANTIS, 3, 0 ) );
						vecItemInfo.push_back( ITEMINFO( II_SYS_TICKET_KEBARAS001, 1, 0 ) );
						break;

					case 24:
						vecItemInfo.push_back( ITEMINFO( II_CHR_FOO_COO_REMANTIS, 3, 0 ) );
						vecItemInfo.push_back( ITEMINFO( II_SYS_SYS_SCR_BX_PET_LEYENA07, 1, 0 ) );
						vecItemInfo.push_back( ITEMINFO( II_GEN_TOO_COL_SILVERBATTERY, 1, 0 ) );
						break;

					case 29:
						vecItemInfo.push_back( ITEMINFO( II_CHR_FOO_COO_REMANTIS, 3, 0 ) );
						vecItemInfo.push_back( ITEMINFO( II_SYS_SYS_SCR_AWAKECANCEL, 1, 0 ) );
						vecItemInfo.push_back( ITEMINFO( II_SYS_SYS_SCR_BAGBAG15, 1, 0 ) );
						break;

					case 34:
						vecItemInfo.push_back( ITEMINFO( II_CHR_FOO_COO_REMANTIS, 3, 0 ) );
#ifdef __PROTECT_AWAKE
						vecItemInfo.push_back( ITEMINFO( II_SYS_SYS_SCR_AWAKESAFE, 1, 0 ) );
#endif // __PROTECT_AWAKE
#if __VER >= 15 // __PETVIS
						vecItemInfo.push_back( ITEMINFO( II_SYS_SYS_SCR_BXVISMAGIC01, 2, 0 ) );
						vecItemInfo.push_back( ITEMINFO( II_SYS_SYS_SCR_BXVISMELEE01, 2, 0 ) );
#endif // __PETVIS
						break;

					case 35:
						vecItemInfo.push_back( ITEMINFO( II_CHR_FOO_COO_REMANTIS, 5, 0 ) );
#if __VER >= 15 // __PETVIS
						vecItemInfo.push_back( ITEMINFO( II_SYS_SYS_SCR_PET_MAGIC, 1, 0 ) );
#endif // __PETVIS
						break;

					default:
						break;
				}
#endif // #ifdef __EVENT_1101_2
				char lpText[512];
				for( size_t size=0; size<vecItemInfo.size(); size++ )
				{
					if( vecItemInfo[size].dwItemId > 0 )
					{
						CItemElem itemElem;
						itemElem.m_dwItemId	= vecItemInfo[size].dwItemId;
						itemElem.m_nItemNum	= vecItemInfo[size].nItemNum;
						itemElem.SetFlag( vecItemInfo[size].nBins );
						itemElem.SetSerialNumber();

						memset( lpText, 0, sizeof( lpText ) );
						sprintf( lpText, (char*)GETTEXT( TID_GAME_EVENT_MAIL_CALLTHEROLL ), nCount, itemElem.GetProp()->szName );
						g_dpDBClient.SendQueryPostMail( pUser->m_idPlayer, 0, itemElem, 0, (char*)GETTEXT( TID_GAME_EVENT_MAIL_CALLTHEROLL_TITLE ), lpText );
					}
				}
			}
		}
	}
}
#endif	// __EVENT_1101

#ifdef __EVE_MINIGAME
void CUser::AddKawibawiboResult( int nResult, int nWinCount, DWORD dwItemId, int nItemCount, DWORD dwNextItemId, int nNextItemCount )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_MINIGAME;
	m_Snapshot.ar << MINIGAME_KAWIBAWIBO_RESUTLT;

	m_Snapshot.ar << nResult;
	m_Snapshot.ar << nWinCount;
	if( nResult == CMiniGame::KAWIBAWIBO_WIN )
	{
		m_Snapshot.ar << dwItemId << dwNextItemId;
		m_Snapshot.ar << nItemCount << nNextItemCount;
		m_Snapshot.ar << prj.m_MiniGame.nKawiBawiBo_MaxWin;
	}
	else if( nResult == CMiniGame::KAWIBAWIBO_GETITEM )
	{
		m_Snapshot.ar << dwItemId;
		m_Snapshot.ar << nItemCount;
	}
}

void CUser::AddReassembleOpenWnd( vector<DWORD> vecItemId )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_MINIGAME;
	m_Snapshot.ar << MINIGAME_REASSEMBLE_OPENWND;
	m_Snapshot.ar << vecItemId.size();
	for( int i=0; i<(int)( vecItemId.size() ); ++i )
		m_Snapshot.ar << vecItemId[i];
}
	

void CUser::AddReassembleResult( DWORD dwItemId, int nItemCount )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_MINIGAME;
	m_Snapshot.ar << MINIGAME_REASSEMBLE_RESULT;
	
	m_Snapshot.ar << dwItemId;
	m_Snapshot.ar << nItemCount;
}

void CUser::AddAlphabetOpenWnd( int nQuestionID, int nPublicAlphabet, char chPublicChar, CString strQuestion )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_MINIGAME;
	m_Snapshot.ar << MINIGAME_ALPHABET_OPENWND;

	m_Snapshot.ar << nQuestionID;
	m_Snapshot.ar << nPublicAlphabet;
	m_Snapshot.ar << chPublicChar;
	m_Snapshot.ar.WriteString( strQuestion );
}

void CUser::AddAlphabetResult( int nResult, DWORD dwItemId, int nItemCount )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_MINIGAME;
	m_Snapshot.ar << MINIGAME_ALPHABET_RESULT;
	
	m_Snapshot.ar << nResult;
	if( nResult != CMiniGame::ALPHABET_FAILED && nResult != CMiniGame::ALPHABET_NOTENOUGH_MONEY )
	{
		m_Snapshot.ar << dwItemId;
		m_Snapshot.ar << nItemCount;
	}
}

void CUser::AddFiveSystemOpenWnd( int nBetMinPenya, int nBetMaxPenya, int nMultiple )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_MINIGAME;
	m_Snapshot.ar << MINIGAME_FIVESYSTEM_OPENWND;
	
	m_Snapshot.ar << nBetMinPenya;
	m_Snapshot.ar << nBetMaxPenya;
	m_Snapshot.ar << nMultiple;
}

void CUser::AddFiveSystemResult( int nResult, int nGetPenya, int nBetNum )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_MINIGAME;
	m_Snapshot.ar << MINIGAME_FIVESYSTEM_RESULT;
	
	m_Snapshot.ar << nResult;
	if( ( nResult != CMiniGame::FIVESYSTEM_NOTENOUGH ) &&
		( nResult != CMiniGame::FIVESYSTEM_OVERMAX ) &&
		( nResult != CMiniGame::FIVESYSTEM_FAILED ) )
		m_Snapshot.ar << nGetPenya;
	
	if( nResult == CMiniGame::FIVESYSTEM_BETOK )
		m_Snapshot.ar << nBetNum;
	
}
#endif // __EVE_MINIGAME

#if __VER >= 9 // __ULTIMATE
void CUser::AddUltimateMakeItem( int nResult )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ULTIMATE;
	m_Snapshot.ar << ULTIMATE_MAKEITEM;
	m_Snapshot.ar << nResult;
}

void CUser::AddUltimateMakeGem( int nResult, int nNum)
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ULTIMATE;
	m_Snapshot.ar << ULTIMATE_MAKEGEM;
	m_Snapshot.ar << nResult;
	m_Snapshot.ar << nNum;
}

void CUser::AddUltimateWeapon( BYTE state, int nResult )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ULTIMATE;
	m_Snapshot.ar << state;
	m_Snapshot.ar << nResult;
}
#endif // __ULTIMATE

#ifdef __TRADESYS
void CUser::AddExchangeResult( BYTE state, int nResult )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_EXCHANGE;
	m_Snapshot.ar << state;
	m_Snapshot.ar << nResult;
}
#endif // __TRADESYS

#if __VER >= 10 // __REMOVE_ATTRIBUTE
void CUser::AddRemoveAttribute( BOOL bSuccess )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_REMOVE_ATTRIBUTE;
	m_Snapshot.ar << bSuccess;
}
#endif // __REMOVE_ATTRIBUTE

#if __VER >= 9	// __PET_0410
#ifdef __PET_1024
void CUserMng::AddSetPetName( CUser* pUser, const char* szPetName )
{
	CAr ar;
	
	ar << GETID( pUser ) << SNAPSHOTTYPE_SET_PET_NAME;
	ar.WriteString( szPetName );

	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pUser )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pUser )
}
#endif	// __PET_1024

#ifdef __PET_1024
void CUserMng::AddPetCall( CMover* pMover, DWORD dwPetId, DWORD dwIndex, BYTE nPetLevel, const char* szPetName )
#else	// __PET_1024
void CUserMng::AddPetCall( CMover* pMover, DWORD dwPetId, DWORD dwIndex, BYTE nPetLevel )
#endif	// __PET_1024
{
	CAr ar;
	
	ar << GETID( pMover ) << SNAPSHOTTYPE_PET_CALL;
	ar << dwPetId << dwIndex << nPetLevel;
#ifdef __PET_1024
	ar.WriteString( szPetName );
#endif	// __PET_1024
	
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pMover )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void CUserMng::AddPetRelease( CMover* pMover )
{
	CAr ar;
	
	ar << GETID( pMover ) << SNAPSHOTTYPE_PET_RELEASE;
	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pMover )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void CUserMng::AddPetLevelup( CMover* pMover, DWORD dwPetId )
{
	CAr ar;
	
	ar << GETID( pMover ) << SNAPSHOTTYPE_PET_LEVELUP;
	ar << dwPetId;

	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pMover )
		if( USERPTR != pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void CUserMng::AddPetFeed( CMover* pMover, WORD wEnergy )
{
	// 兩
	CAr ar;
	
	ar << GETID( pMover ) << SNAPSHOTTYPE_PET_FEED;
	ar << wEnergy;

	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}
#endif	// __PET_0410

#if __VER >= 13 // __HONORABLE_TITLE			// 달인
void CUserMng::AddHonorTitleChange( CMover* pMover, int nChange )
{
	// 兩
	CAr ar;
	
	ar << GETID( pMover ) << SNAPSHOTTYPE_HONOR_CHANGE_ACK;
	ar << nChange;

	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

#endif	// __HONORABLE_TITLE			// 달인

#if __VER >= 9	//__AI_0509
void CUserMng::AddSetSpeedFactor( CMover* pMover, FLOAT fSpeedFactor )
{
	CAr ar;
	
	ar << GETID( pMover ) << SNAPSHOTTYPE_SET_SPEED_FACTOR;
	ar << fSpeedFactor;

	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pMover )
			USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}
#endif	// __AI_0509

#if __VER >= 10 // __LEGEND	//	9차 전승시스템	Neuz, World, Trans
void CUser::AddLegendSkillResult( int nResult )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_LEGENDSKILLUP_RESULT;
//	m_Snapshot.ar << LEGENDSKILL_RESULT;	
	m_Snapshot.ar << nResult;	
}
#endif	//__LEGEND	//	9차 전승시스템	Neuz, World, Trans

#if __VER >= 9 // __EVENTLUA
void CUserMng::AddEventLua( BYTE nId, BOOL bState )
{
	string strDesc = prj.m_EventLua.GetDesc( nId );
	
	map<DWORD, CUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		CUser* pUser = it->second;
		if( pUser->IsValid() == FALSE )
			continue;
		pUser->AddEventLuaDesc( (int)bState, strDesc );
	}
}
#endif // __EVENTLUA


#if __VER >= 11 // __MA_VER11_04	// 길드 창고 로그 기능 world,database,neuz
void CUser::AddGuildBankLogView( BYTE byListType, short nLogCount, const __GUILDBANKLOG_ENTRY* logs )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDLOG_VIEW;
	m_Snapshot.ar << byListType;
	m_Snapshot.ar << nLogCount;
	
	for( short i=0; i<nLogCount; ++i )
	{
		TRACE("GuildBankLogView:%s %d %d %d %d\n", logs[i].szPlayer, logs[i].nDate,logs[i].nItemID,logs[i].nItemAbilityOption,logs[i].nItemCount);
		m_Snapshot.ar.WriteString( logs[i].szPlayer );				// 누구 
		m_Snapshot.ar << logs[i].nDate;					// 언제 
		m_Snapshot.ar << logs[i].nItemID;					//  nItemAbilityOption
		if( byListType == GI_LOG_VIEW_ITEM_ADD || byListType == GI_LOG_VIEW_ITEM_REMOVE )
		{
			m_Snapshot.ar << logs[i].nItemAbilityOption;
			m_Snapshot.ar << logs[i].nItemCount;					//  
		}
	}
}
#endif //__MA_VER11_04	// 길드 창고 로그 기능 world,database,neuz

#if __VER >= 13 // __HONORABLE_TITLE			// 달인
void CUser::AddHonorListAck()
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_HONOR_LIST_ACK;
	
	for( int i=0; i<MAX_HONOR_TITLE; ++i )
	{
		m_Snapshot.ar << m_aHonorTitle[i];
	}
}
#endif	// __HONORABLE_TITLE			// 달인

#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
void CUser::AddSealChar( short nLogCount, const __SEALCHAR_ENTRY* seals )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SEALCHAR_REQ;
	m_Snapshot.ar << nLogCount;
	
	for( short i=0; i<nLogCount; ++i )
	{
		TRACE("AddSealChar:%s %d %d \n", seals[i].szPlayer, seals[i].nPlayerSlot,seals[i].idPlayer);
		m_Snapshot.ar << seals[i].nPlayerSlot;					// 
		m_Snapshot.ar << seals[i].idPlayer;					//  
		m_Snapshot.ar.WriteString( seals[i].szPlayer );				// 누구 
	}
}
void CUser::AddSealCharSet()
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SEALCHARGET_REQ;
}
#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz

#ifdef __EVENTLUA_COUPON
void CUser::AddCoupon( int nCoupon )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_EVENT_COUPON;
	m_Snapshot.ar << nCoupon;
}
#endif // __EVENTLUA_COUPON

#if __VER >= 12 // __SECRET_ROOM
void CUser::AddSecretRoomMngState( int nState, DWORD dwRemainTime )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SECRETROOM_MNG_STATE;
	m_Snapshot.ar << nState;
	m_Snapshot.ar << dwRemainTime;
}

void CUser::AddSecretRoomContInfo( BYTE nContinent, BYTE nType, vector<__SECRETROOM_TENDER> & vecSecreetRoomTender, int nIndex )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SECRETROOM_INFO;
	m_Snapshot.ar << nContinent;
	m_Snapshot.ar << nType;

	switch( nType )
	{
		case SECRETROOM_GUILDLIST :
			{
				m_Snapshot.ar << vecSecreetRoomTender.size();
	
				for( int i=0; i<(int)( vecSecreetRoomTender.size() ); i++ )
				{
					m_Snapshot.ar << vecSecreetRoomTender[i].dwGuildId;
					m_Snapshot.ar << vecSecreetRoomTender[i].nWarState;
					m_Snapshot.ar << vecSecreetRoomTender[i].nKillCount;

					if( m_idGuild == vecSecreetRoomTender[i].dwGuildId )
					{
						int nSize = vecSecreetRoomTender[i].vecLineUpMember.size();
						m_Snapshot.ar << nSize;

						for(int j=0; j<nSize; j++)
						{
							m_Snapshot.ar << vecSecreetRoomTender[i].vecLineUpMember[j];
						}
					}
				}
				
				map<int, int> mapMonsterNum = CSecretRoomMng::GetInstance()->m_mapMonsterNum;
				map<int, int>::iterator it = mapMonsterNum.begin();
				m_Snapshot.ar << mapMonsterNum.size();
				for( ; it!=mapMonsterNum.end(); it++ )
				{
					m_Snapshot.ar << it->first;
					m_Snapshot.ar << it->second;
				}
			}
			break;

		case SECRETROOM_KILLCOUNT :
			{
				m_Snapshot.ar << nIndex;
				m_Snapshot.ar << vecSecreetRoomTender[nIndex].nKillCount;
			}
			break;

		case SECRETROOM_WARSTATE :
			{
				m_Snapshot.ar << nIndex;
				m_Snapshot.ar << vecSecreetRoomTender[nIndex].nWarState;
			}
			break;
	}
}

void CUser::AddSecretRoomTenderOpenWnd( int nTenderPenya )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SECRETROOM_TENDEROPENWND;
	m_Snapshot.ar << CSecretRoomMng::GetInstance()->m_nMinPenya;
	m_Snapshot.ar << nTenderPenya;
}

void CUser::AddSecretRoomLineUpOpenWnd( vector<DWORD>& vecLineUpMember )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SECRETROOM_LINEUPOPENWND;
	m_Snapshot.ar << CSecretRoomMng::GetInstance()->m_nMinGuildMemberNum;
	m_Snapshot.ar << CSecretRoomMng::GetInstance()->m_nMaxGuildMemberNum;
	m_Snapshot.ar << vecLineUpMember.size();

	for( int i=0; i<(int)( vecLineUpMember.size() ); i++ )
		m_Snapshot.ar << vecLineUpMember[i];
}

void CUser::AddSecretRoomTenderView( int nTenderPenya, int nRanking, time_t t, vector<__SECRETROOM_TENDER>& vecSRTender )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SECRETROOM_TENDERVIEW;
	m_Snapshot.ar << nTenderPenya;
	m_Snapshot.ar << nRanking;
	m_Snapshot.ar << t;
	m_Snapshot.ar << vecSRTender.size();
	for( int i=0; i<(int)( vecSRTender.size() ); i++ )
		m_Snapshot.ar << vecSRTender[i].dwGuildId;
}
#endif // __SECRET_ROOM

#if __VER >= 12 // __LORD
void CUser::AddLord( void )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_LORD;
	CSLord::Instance()->Serialize( m_Snapshot.ar );
}

int CUser::GetMuteText( void )
{
	if( m_dwMute > 0 )
		return TID_GAME_STATE_MUTE;
	if( HasBuff( BUFF_ITEM, II_SYS_SYS_LS_RAGE ) )
		return TID_GAME_LORD_SKILL_RAGE_E001;
	return 0;
}

void CUser::AddLordSkillTick( CLordSkill* pSkills )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_LORD_SKILL_TICK;
	pSkills->SerializeTick( m_Snapshot.ar );	
}

// 
BOOL CUser::IsTeleportable( void )
{
	DWORD dwWorld	= GetWorld()->GetID();
	if( dwWorld == WI_WORLD_GUILDWAR
		|| ( dwWorld >= WI_WORLD_GUILDWAR1TO1_0 &&  dwWorld <= WI_WORLD_GUILDWAR1TO1_L )
		|| dwWorld == WI_DUNGEON_MUSCLE
		|| dwWorld == WI_DUNGEON_KRRR
		|| dwWorld == WI_DUNGEON_BEAR
		|| dwWorld == WI_WORLD_KEBARAS
		|| ( dwWorld == WI_WORLD_MADRIGAL && prj.IsGuildQuestRegion( GetPos() ) )
		|| dwWorld == WI_DUNGEON_SECRET_0
		|| ( dwWorld >= WI_DUNGEON_SECRET_1 && dwWorld <= WI_DUNGEON_SECRET_L )
		|| dwWorld == WI_WORLD_CISLAND	// 2008.01.21 코랄 아일랜드 검사 추가
//		|| ( dwWorld >= WI_WORLD_HEAVEN01 && dwWorld <= WI_WORLD_HEAVEN05 )
#if __VER >= 14 // __INSTANCE_DUNGEON
		|| CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( dwWorld )
#endif // __INSTANCE_DUNGEON
#ifdef __QUIZ
		|| dwWorld == WI_WORLD_QUIZ
#endif // __QUIZ
	)
		return FALSE;
	return ( GetSummonState() == 0 );
}
#endif	// __LORD

#if __VER >= 12 // __MOD_TUTORIAL
void CUser::AddSetTutorialState( void )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_SET_TUTORIAL_STATE;
	m_Snapshot.ar << GetTutorialState();
}
#endif	// __MOD_TUTORIAL

#if __VER >= 12 // __LORD
void CUserMng::AddElectionBeginCandidacy( void )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_ELECTION_BEGIN_CANDIDACY;
	GETBLOCK( ar, lpBuf, uBufSize );
	AddBlock( lpBuf, uBufSize );
}

void CUserMng::AddElectionBeginVote( int nRequirement )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_ELECTION_BEGIN_VOTE;
	ar << nRequirement;
	GETBLOCK( ar, lpBuf, uBufSize );
	AddBlock( lpBuf, uBufSize );
}

void CUserMng::AddElectionEndVote( u_long idPlayer )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_ELECTION_END_VOTE;
	ar << idPlayer;
	GETBLOCK( ar, lpBuf, uBufSize );
	AddBlock( lpBuf, uBufSize );
}

void CUserMng::AddElectionAddDeposit( u_long idPlayer, __int64 iDeposit, time_t tCreate )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_ELECTION_ADD_DEPOSIT;
	ar << idPlayer << iDeposit << tCreate;
	GETBLOCK( ar, lpBuf, uBufSize );
	AddBlock( lpBuf, uBufSize );
}

void CUserMng::AddElectionSetPledge( u_long idPlayer, const char* szPledge )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_ELECTION_SET_PLEDGE;
	ar << idPlayer;
	ar.WriteString( szPledge );
	GETBLOCK( ar, lpBuf, uBufSize );
	AddBlock( lpBuf, uBufSize );
}

void CUserMng::AddElectionIncVote( u_long idPlayer, u_long idElector )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_ELECTION_INC_VOTE;
	ar << idPlayer << idElector;
	GETBLOCK( ar, lpBuf, uBufSize );
	AddBlock( lpBuf, uBufSize );
}

void CUserMng::AddLEventCreate( CLEComponent* pComponent )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_L_EVENT_CREATE;
	pComponent->Serialize( ar );
	GETBLOCK( ar, lpBuf, uBufSize );
	AddBlock( lpBuf, uBufSize );
}

void CUserMng::AddLEventInitialize( void )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_L_EVENT_INITIALIZE;
	GETBLOCK( ar, lpBuf, uBufSize );
	AddBlock( lpBuf, uBufSize );
}

void CUserMng::AddLEventTick( ILordEvent* pEvent )
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_L_EVENT_TICK;
	pEvent->SerializeTick( ar );
	GETBLOCK( ar, lpBuf, uBufSize );
	AddBlock( lpBuf, uBufSize );
}

void CUserMng::AddLordSkillUse( CUser* pUser, u_long idTarget, int nSkill )
{
	CAr ar;
	ar << GETID( pUser ) << SNAPSHOTTYPE_LORD_SKILL_USE << idTarget << nSkill;
	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pUser )
	{
		USERPTR->AddBlock( lpBuf, nBufSize );
	}
	NEXT_VISIBILITYRANGE( pUser )
}

void CUserMng::AddQueryPlayerData( u_long idPlayer )
{
	if( idPlayer == NULL_ID )
		return;
	PlayerData* pData	= CPlayerDataCenter::GetInstance()->GetPlayerData( idPlayer );
	if( !pData )
		return;
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_QUERY_PLAYER_DATA;
	ar << idPlayer;
	ar.WriteString( pData->szPlayer );
	ar.Write( &pData->data, sizeof(sPlayerData) );
	GETBLOCK( ar, lpBuf, uBufSize );
	AddBlock( lpBuf, uBufSize );
}
#endif	// __LORD

#if __VER >= 12 // __TAX
void CUser::AddTaxInfo( void )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_TAX_ALLINFO;
	CTax::GetInstance()->Serialize( m_Snapshot.ar );
}

void CUser::AddTaxSetTaxRateOpenWnd( BYTE nCont )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_TAX_SETTAXRATE_OPENWND;
	m_Snapshot.ar << nCont;
	m_Snapshot.ar << CTax::GetInstance()->m_nMinTaxRate << CTax::GetInstance()->m_nMaxTaxRate;
}

void CUserMng::AddTaxInfo( void )
{
	map<DWORD, CUser*>::iterator it;
	for( it = m_users.begin(); it != m_users.end(); ++it )
	{
		CUser* pUser = it->second;
		if( pUser->IsValid() == FALSE )
			continue;
		pUser->AddTaxInfo();
	}
}
#endif // __TAX

#if __VER >= 13 // __RAINBOW_RACE
void CUser::AddRainbowRacePrevRankingOpenWnd()
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_RAINBOWRACE_PREVRANKING_OPENWND;
	vector<DWORD> vecdwPrevRanking;
	vecdwPrevRanking = CRainbowRaceMng::GetInstance()->GetPrevRanking();
	m_Snapshot.ar << vecdwPrevRanking.size();
	for( int i=0; i<(int)( vecdwPrevRanking.size() ); i++ )
		m_Snapshot.ar << vecdwPrevRanking[i];
}

void CUser::AddRainbowRaceApplicationOpenWnd()
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_RAINBOWRACE_APPLICATION_OPENWND;
	m_Snapshot.ar << CRainbowRaceMng::GetInstance()->GetApplicantNum();
}

void CUser::AddRainbowRaceState( CRainbowRace* pRainbowRace )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_RAINBOWRACE_NOWSTATE;
	pRainbowRace->Serialize( m_Snapshot.ar );
}

void CUser::AddMiniGameState( __MINIGAME_PACKET MiniGamePacket )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_RAINBOWRACE_MINIGAMESTATE;
	MiniGamePacket.Serialize( m_Snapshot.ar );
}

void CUser::AddMiniGameExtState( __MINIGAME_EXT_PACKET MiniGameExtPacket )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_RAINBOWRACE_MINIGAMEEXTSTATE;
	MiniGameExtPacket.Serialize( m_Snapshot.ar );
}
#endif // __RAINBOW_RACE

#if __VER >= 13 // __HOUSING
void CUser::AddHousingAllInfo( CHousing* pHousing )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_HOUSING_ALLINFO;
	pHousing->Serialize( m_Snapshot.ar );
}

void CUser::AddHousingSetFurnitureList( HOUSINGINFO& housingInfo, BOOL bAdd )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_HOUSING_FURNITURELIST;
	housingInfo.Serialize( m_Snapshot.ar );
	m_Snapshot.ar << bAdd;
}

void CUser::AddHousingSetupFurniture( HOUSINGINFO* pHousingInfo )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_HOUSING_SETUPFURNITURE;
	pHousingInfo->Serialize( m_Snapshot.ar );
}

void CUser::AddHousingPaperingInfo( DWORD dwItemId, BOOL bSetup )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_HOUSING_PAPERINGINFO;
	m_Snapshot.ar << dwItemId;
	m_Snapshot.ar << bSetup;
}

void CUserMng::AddHousingPaperingInfo( DWORD dwItemId, BOOL bSetup, DWORD dwMasterId )
{
	for( map<DWORD, CUser*>::iterator it=m_users.begin(); it!=m_users.end(); ++it )
	{
		CUser* pUser = it->second;
		if( pUser->IsValid() && pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_MINIROOM  )
		{
			if( pUser->GetLayer() == static_cast<int>( dwMasterId ) )
				pUser->AddHousingPaperingInfo( dwItemId, bSetup );
		}
	}
}

void CUser::AddHousingSetVisitAllow( DWORD dwTargetId, BOOL bAllow )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_HOUSING_SETVISITALLOW;
	m_Snapshot.ar << dwTargetId << bAllow;
}

void CUser::AddHousingVisitableList( vector<DWORD> & vecVisitable )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_HOUSING_VISITABLELIST;
	m_Snapshot.ar << vecVisitable.size();
	for( int i=0; i<(int)( vecVisitable.size() ); i++ )
		m_Snapshot.ar << vecVisitable[i];
}
#endif // __HOUSING

#if __VER >= 13 // __QUEST_HELPER
void CUser::AddNPCPos( const D3DXVECTOR3 & vPos )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_QUESTHELPER_NPCPOS;
	m_Snapshot.ar << vPos;
}
#endif // __QUEST_HELPER

void CUser::AddClearTarget()
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CLEAR_TARGET;
}

BOOL CUser::IsGuildMaster()
{
	if( GetGuild() && GetGuild()->IsMaster( m_idPlayer ) )
		return TRUE;

	return FALSE;
}

#ifdef __AZRIA_1023
int CUser::DoUseItemInput( ItemProp* pProp, CItemElem* pItem )
{
	switch( pProp->dwItemKind3 )
	{
		case IK3_TICKET:
			DoUseItemTicket( pItem );
			return 0;
		default:	break;
	}
	switch( pProp->dwID )
	{
#ifdef __PET_1024
		case II_SYS_SYS_SCR_PET_NAMING:
			return DoUseItemPetNaming();
#endif	// __PET_1024
		default:
			return 1;
	}
	return 1;
}
#endif	// __AZRIA_1023

#ifdef __PET_1024
int CUser::DoUseItemPetNaming()
{
	CPet* pPet	= GetPet();
	if( !pPet )
	{
		AddDefinedText( TID_GAME_NAME_PET_E00 );
		return 1;
	}
	if( prj.IsInvalidName( GetInput() ) || !prj.IsAllowedLetter( GetInput() ) )
	{
		AddDefinedText( TID_GAME_NAME_PET_E01 );
		return 1;
	}
	pPet->SetName( GetInput() );
	g_UserMng.AddSetPetName( this, pPet->GetName() );
	return 0;
}
#endif	// __PET_1024

#if __VER >= 14 // __PCBANG
void CUser::AddPCBangInfo( CPCBangInfo* pPI )
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PCBANG_INFO;
	pPI->Serialize( m_Snapshot.ar );
}
#endif // __PCBANG

#ifdef __VTN_TIMELIMIT
void CUser::AddAccountPlayTime()
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ACCOUNT_PLAYTIME;
	m_Snapshot.ar << m_nAccountPlayTime;
}

void CUserMng::ResetAccountPlayTime()
{
	for( map<DWORD, CUser*>::iterator it=m_users.begin(); it!=m_users.end(); ++it )
	{
		CUser* pUser = it->second;
		if( pUser->IsValid() && pUser->m_nAccountPlayTime > -1 )
		{
			pUser->m_nAccountPlayTime = 0;
			pUser->AddAccountPlayTime();
		}
	}
}
#endif // __VTN_TIMELIMIT

#if __VER >= 14 // __SMELT_SAFETY
void CUser::AddSmeltSafety( BYTE nResult )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SMELT_SAFETY;
	m_Snapshot.ar << nResult;
}
#endif // __SMELT_SAFETY

#ifdef __MAP_SECURITY
void CUser::AddWorldReadInfo( DWORD dwWorldId, D3DXVECTOR3 vPos )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_WORLD_READINFO;
	m_Snapshot.ar << dwWorldId << vPos;
}
#endif // __MAP_SECURITY

#ifdef __QUIZ
void CUser::AddQuizEventState( int nType, int nState )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_QUIZ_STATE;
	m_Snapshot.ar << nType << nState;
}

void CUser::AddQuizEventMessage( const char* lpszMessage, int nState )
{
	if( strlen( lpszMessage ) == 0 ) return;
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_QUIZ_MESSAGE;
	m_Snapshot.ar.WriteString( lpszMessage );
	m_Snapshot.ar << nState;
}

void CUser::AddQuizQuestion( int nType, const char* lpszQuestion, int nCount )
{
	if( strlen( lpszQuestion ) == 0 ) return;
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_QUIZ_QUESTION;
	m_Snapshot.ar << nType;
	m_Snapshot.ar.WriteString( lpszQuestion );
	m_Snapshot.ar << nCount;
}
#endif // __QUIZ

#if __VER >= 15 // __PETVIS
void CUser::AddActivateVisPet( OBJID objIdVisPetItem, OBJID objIdVisPetId )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_VISPET_ACTIVATE;
	m_Snapshot.ar << objIdVisPetItem << objIdVisPetId;
}
	
void CUserMng::AddChangeMoverSfxId( CMover* pMover )
{
	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_MOVER_CHANGESFX;
	ar << pMover->m_dwMoverSfxId;

	GETBLOCK( ar, lpBuf, nBufSize );
	
	FOR_VISIBILITYRANGE( pMover )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}
#endif // __PETVIS

#if __VER >= 15 // __GUILD_HOUSE
void CUser::AddGuildHousePakcet( int nPacketType, GH_Fntr_Info & gfi, int nIndex )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDHOUSE_PACKET;
	m_Snapshot.ar << nPacketType << nIndex;
	gfi.Serialize( m_Snapshot.ar );
}

void CUser::AddGuildHouseAllInfo( CGuildHouseBase* pGuildHouse )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDHOUSE_ALLINFO;
	m_Snapshot.ar << static_cast<BOOL>( g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) );
	if( pGuildHouse )
	{
		m_Snapshot.ar << TRUE;
		pGuildHouse->SerializeAllInfo( m_Snapshot.ar );
	}
	else
		m_Snapshot.ar << FALSE;
}

void CUser::AddGuildHouseRemvoe()
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDHOUSE_REMOVE;
}

EXPINTEGER CUser::GetAddExpAfterApplyRestPoint( EXPINTEGER nExp )
{
	if( m_nRestPoint > 0 )
	{
		EXPINTEGER nAddExp = (EXPINTEGER)( nExp * GuildHouseMng->GetRestExpFactor( m_nLevel ) );	// 추가 경험치를 구한다.
		EXPFLOAT fTemp = static_cast<EXPFLOAT>( nExp * min( m_nLevel / 5, 1 ) ) / static_cast<EXPFLOAT>( prj.m_aExpCharacter[m_nLevel + 1].nExp1 );
		int nSubRestPoint = (int)( fTemp * static_cast<EXPFLOAT>( MAX_REST_POINT * GuildHouseMng->GetRestPointFactor(  this ) ) );	// 차감될 휴식포인트
		if( nSubRestPoint > 0 )	// 차감되는 포인트가 있어야 추가 경험치를 적용할 수 있다.
		{
			int nRemainPoint = SetDecRestPoint( nSubRestPoint );	// 휴식포인트를 차감하고 남은 포인트를 반환한다.
			if( nRemainPoint < 0 )	// 남은 포인트가 부족하면 남은 포인트만큼의 추가 경험치를 반환한다.
				return nAddExp * ( nSubRestPoint + nRemainPoint ) / nSubRestPoint;
			else	// 포인트가 부족하지 않으면 추가 경험치를 반환한다.
				return nAddExp;
		}
	}

	return 0;
}

int CUser::SetAddRestPoint( int nRestPoint )
{
	if( nRestPoint == 0 ) return m_nRestPoint;
	
	m_nRestPoint	= (int)( m_nRestPoint + ( nRestPoint * GuildHouseMng->GetRestPointFactor( this ) ) );
	int nRemainPoint = m_nRestPoint;
	if( m_nRestPoint < 0 )	m_nRestPoint = 0;
	if( m_nRestPoint > MAX_REST_POINT ) m_nRestPoint = MAX_REST_POINT;

	return nRemainPoint;
}

void CUser::AddRestPoint()
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDHOUSE_RESTPOINT;
	m_Snapshot.ar << m_nRestPoint;
}

BOOL CUserMng::HasUserSameWorldnLayer( CUser* pUserSrc )
{
	if( !IsValidObj( pUserSrc ) || !pUserSrc->GetWorld() )
		return FALSE;

	for( map<DWORD, CUser*>::iterator it=m_users.begin(); it!=m_users.end(); it++ )
	{
		if( ( pUserSrc != it->second ) && ( pUserSrc->GetWorld() && it->second->GetWorld() )
			&& ( pUserSrc->GetWorld()->GetID() == it->second->GetWorld()->GetID() ) && ( pUserSrc->GetLayer() && it->second->GetLayer() ) )
			return TRUE;
	}

	return FALSE;
}
#endif // __GUILD_HOUSE

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
void CUser::AddCheckedQuest()
{
	if( IsDelete() )	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_QUEST_CHECKED;
	m_Snapshot.ar << this->m_nCheckedQuestSize;
	if( this->m_nCheckedQuestSize )
		m_Snapshot.ar.Write( this->m_aCheckedQuest, sizeof(WORD) * this->m_nCheckedQuestSize ); 
}
#endif // __IMPROVE_QUEST_INTERFACE

#if __VER >= 15 // __CAMPUS
void CUser::AddInviteCampusMember( CUser* pRequest )
{
	if( IsDelete() )	return;
	if( !IsValidObj( pRequest ) )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CAMPUS_INVITE;
	m_Snapshot.ar << pRequest->m_idPlayer;
	m_Snapshot.ar.WriteString( pRequest->GetName() );
}

void CUser::AddUpdateCampus( CCampus* pCampus )
{
	if( IsDelete() )	return;
	if( !pCampus )		return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CAMPUS_UPDATE;
	pCampus->Serialize( m_Snapshot.ar );
}

void CUser::AddRemoveCampus( u_long idCampus )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CAMPUS_REMOVE;
	m_Snapshot.ar << idCampus;
}

void CUser::AddUpdateCampusPoint( int nCampusPoint )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CAMPUS_UPDATE_POINT;
	m_Snapshot.ar << nCampusPoint;
}

void CUser::ProcessCampus()
{
	CCampus* pCampus = CCampusHelper::GetInstance()->GetCampus( GetCampusId() );
	if( pCampus && pCampus->IsMember( m_idPlayer ) )
	{
		int nBuffLevel = pCampus->GetBuffLevel( m_idPlayer );
		if( nBuffLevel )
		{
			if( pCampus->IsMaster( m_idPlayer ) )
			{
				if( pCampus->IsChangeBuffLevel( m_idPlayer ) )
					RemoveIk3Buffs( IK3_TS_BUFF );
			}
			if( !HasBuffByIk3( IK3_TS_BUFF ) )
				ActiveCampusBuff( nBuffLevel );
		}
		else
			RemoveIk3Buffs( IK3_TS_BUFF );
	}
	else
		RemoveIk3Buffs( IK3_TS_BUFF );
}

void CUser::ActiveCampusBuff( int nLevel )
{
	ItemProp* pProp	= prj.GetItemProp( CCampusHelper::GetInstance()->GetCampusBuff( nLevel ) );
	if( pProp )
		DoApplySkill( this, pProp, NULL );
}

int CUser::GetPointByType( int nType )
{
	int nPoint = 0;

	switch( nType )
	{
	case POINTTYPE_CAMPUS:
		nPoint = GetCampusPoint();
		break;

	default:
		break;
	}
	
	return nPoint;
}
#endif // __CAMPUS




//	mulcom	BEGIN100405	각성 보호의 두루마리
void	CUser::SelectAwakeningValue( DWORD dwItemObjID, DWORD dwSerialNumber, BYTE bySelectFlag )
{
	CItemElem *	pItem = NULL;
	pItem	= (CItemElem*)( GetItemId( dwItemObjID ) );

	if( pItem == NULL )
	{
		WriteLog( "pItem is NULL in SelectAwakeningValue function. ItemObjID : [%d]", dwItemObjID );

		return;
	}

	DWORD	dwItemSerialNumber = 0;
	dwItemSerialNumber	= (DWORD)( pItem->GetSerialNumber() );

	if( dwItemSerialNumber != dwSerialNumber )
	{
		WriteLog( "Item SerialNumber is different. ItemSerialNumber : [%d] [%d]", dwItemSerialNumber, dwSerialNumber );

		return;
	}

	if( pItem->GetNewRandomOption() == 0 )
	{
		WriteLog( "This item have not new random option." );

		return;
	}

	bool	bRetValue	= false;

	bRetValue	= pItem->SelectRandomOption( bySelectFlag );

	if( bRetValue == true )
	{
		UpdateItemEx( (BYTE)( pItem->m_dwObjId ), UI_RANDOMOPTITEMID, pItem->GetRandomOptItemId() );

		//	mulcom	BEGIN100426	각성 보호의 두루마리 로그.
		//WriteLog( "UserID [%d] : ItemSerialNumber[%d] SelectAwakeningValue bySelectFlag[%d]", (int)( GetId() ), (int)( pItem->GetSerialNumber() ), (int)( bySelectFlag ) );

 		TCHAR	szSelectValue[128];
 		::memset( szSelectValue, 0, sizeof(szSelectValue) );

		if( bySelectFlag == 1 )			//	_AWAKE_OLD_VALUE
		{
 			::_stprintf( szSelectValue, "SELECT _AWAKE_OLD_VALUE" );
		}
		else if( bySelectFlag == 2 )	//	_AWAKE_NEW_VALUE
		{
			::_stprintf( szSelectValue, "SELECT _AWAKE_NEW_VALUE" );
		}
		else
		{
			::_stprintf( szSelectValue, "SELECT ERROR" );
		}

 		g_DPSrvr.PutItemLog( this, "z", szSelectValue, pItem, 1 );
		//	mulcom	END100426	각성 보호의 두루마리 로그.

	}

	return;
}

void	CUser::SendNewRandomOption( unsigned char id, DWORD dwSerialNumber, __int64 n64NewRandomOption )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ITEM_SELECT_AWAKENING_VALUE;
	m_Snapshot.ar << id << dwSerialNumber << n64NewRandomOption;
}
//	mulcom	END100405	각성 보호의 두루마리


#ifdef __GUILD_HOUSE_MIDDLE
void CUser::AddGuildHouseTenderMainWnd( map<OBJID, char*>& mapGHList )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDHOUSE_TENDER_MAINWND;
	m_Snapshot.ar << mapGHList.size();
	for( map<OBJID, char*>::iterator it = mapGHList.begin(); it != mapGHList.end(); ++it )
	{
		m_Snapshot.ar << it->first;
		m_Snapshot.ar.WriteString( it->second );
	}
}

void CUser::AddGuildHouseTenderInfoWnd( OBJID objGHId, int nTenderMinPenya, __int64 nTenderPenya, vector<DWORD>& vecGuildId )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDHOUSE_TENDER_INFOWND;
	m_Snapshot.ar << objGHId << nTenderMinPenya << nTenderPenya << vecGuildId.size();
	for( DWORD i = 0; i < vecGuildId.size(); ++i )
	{
		m_Snapshot.ar << vecGuildId[i];
	}
}

void CUser::AddGuildHouseTenderResult( OBJID objGHId, BOOL bResult )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDHOUSE_TENDER_RESULT;
	m_Snapshot.ar << objGHId << bResult;
}
#endif // __GUILD_HOUSE_MIDDLE

#ifdef	__COLOSSEUM
void CUser::SendColosseumEnter()
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_COLOSSEUM_ENTER;
}
void CUser::SendColosseumStart( int nTime )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_COLOSSEUM_START;
	m_Snapshot.ar << nTime;
}
void CUser::SendColosseumReady1( int nTime, BYTE nStage )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_COLOSSEUM_READY1;
	m_Snapshot.ar << nTime << nStage;
}
void CUser::SendColosseumReady2( int nTime )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_COLOSSEUM_READY2;
	m_Snapshot.ar << nTime;
}
void CUser::SendColosseumEndMatch( BOOL bWin, int nTime )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_COLOSSEUM_ENDMATCH;
	m_Snapshot.ar << bWin << nTime;
}
#endif //__COLOSSEUM

#ifdef __NEW_ITEM_VARUNA
void CUser::AddActivateBarunaPet( DWORD dwItemId, DWORD dwBarunaPetID, BOOL bActivate )
{
	if( IsDelete() )	return;
	
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ADDBARUNAPET;
	m_Snapshot.ar << dwItemId << dwBarunaPetID <<bActivate;
}
#endif// __NEW_ITEM_VARUNA

#ifdef __TREASUREBOX
void CUser::SendTreasure( TreasureItem dwIDs[3], int nCount[3] )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SENDTREASURE;
	for( int i=0;i<3;i++ )
	{
		m_Snapshot.ar << dwIDs[i].dwItemID;
		m_Snapshot.ar << nCount[i];
	}
}
#endif//__TREASUREBOX