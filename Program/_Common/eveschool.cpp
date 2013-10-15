#include "stdafx.h"

#include "eveschool.h"
#ifdef __WORLDSERVER
#include "DPSrvr.h"
#include "DPCoreClient.h"
#include "guild.h"
#include "dpdatabaseclient.h"
#include "user.h"
extern	CDPSrvr		g_DPSrvr;
extern	CDPCoreClient	g_DPCoreClient;
extern	CGuildMng	g_GuildMng;

extern	CDPDatabaseClient	g_dpDBClient;

extern	CUserMng	g_UserMng;

#include "WorldMng.h"
extern CWorldMng g_WorldMng;

#include "definetext.h"
#include "defineobj.h"
#include "definesound.h"

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

CEveSchool::CEveSchool()
{
	m_pSchoolMng	= &g_GuildMng;
	m_dwsbstart2	= GetTickCount();
}

BOOL CEveSchool::Ready( void )
{
// "/sbready
	if( g_eLocal.GetState( EVE_SCHOOL_BATTLE ) != SBS_END )
		return FALSE;

	g_UserMng.ModifyMode( OBSERVE_MODE );	// Observe mode	// all

	for( map<u_long, CGuild*>::iterator i = m_pSchoolMng->m_mapPGuild.begin(); i != m_pSchoolMng->m_mapPGuild.end(); ++i )
	{
		CGuild* pSchool	= i->second;
		pSchool->m_nDead	= 0;
		for( map<u_long, CGuildMember*>::iterator i2 = pSchool->m_mapPMember.begin(); i2 != pSchool->m_mapPMember.end(); ++i2 )
		{
			CGuildMember* pMember	= i2->second;
			CUser* pUser	= g_UserMng.GetUserByPlayerID( pMember->m_idPlayer );
			if( IsValidObj( (CObj*)pUser ) )
			{
				pUser->REPLACE( g_uIdofMulti, WI_WORLD_EVENT01, GetPos( pSchool->m_idGuild ), REPLACE_NORMAL, nDefaultLayer );
				pUser->SetNotMode( OBSERVE_MODE );	// Not observe mode
				g_UserMng.AddModifyMode( pUser );
			}
		}
	}

	if( g_eLocal.SetState( EVE_SCHOOL_BATTLE, SBS_READY ) )
		g_UserMng.AddSetLocalEvent( EVE_SCHOOL_BATTLE, SBS_READY );

	return TRUE;
}

BOOL CEveSchool::Start( void )
{
// "/sbstart"
	if( g_eLocal.GetState( EVE_SCHOOL_BATTLE ) != SBS_READY )
		return FALSE;

	if( g_eLocal.SetState( EVE_SCHOOL_BATTLE, SBS_START ) )
		g_UserMng.AddSetLocalEvent( EVE_SCHOOL_BATTLE, SBS_START );
	return TRUE;
}

BOOL CEveSchool::Start2( void )
{
// "/sbstart2"
	if( g_eLocal.GetState( EVE_SCHOOL_BATTLE ) != SBS_START )
		return FALSE;

	if( g_eLocal.SetState( EVE_SCHOOL_BATTLE, SBS_START2 ) )
		g_UserMng.AddSetLocalEvent( EVE_SCHOOL_BATTLE, SBS_START2 );

	m_dwsbstart2	= GetTickCount();
	return TRUE;
}

BOOL CEveSchool::Report( void )
{
//	"/sbreport"
	if( g_eLocal.GetState( EVE_SCHOOL_BATTLE ) == SBS_END )
		return FALSE;

	SCHOOL_ENTRY	school[MAX_SCHOOL];

	int iIndex	= 0;
	for( map<u_long, CGuild*>::iterator i = m_pSchoolMng->m_mapPGuild.begin();
		i != m_pSchoolMng->m_mapPGuild.end(); ++i )
	{
		CGuild* pSchool	= i->second;
		
		school[iIndex].id	= pSchool->m_idGuild;
		lstrcpy( school[iIndex].lpName, pSchool->m_szGuild );
		school[iIndex].nDead	= pSchool->m_nDead;

		for( map<u_long, CGuildMember*>::iterator i2 = pSchool->m_mapPMember.begin();
		i2 != pSchool->m_mapPMember.end(); ++i2 )
		{
			CGuildMember* pMember	= i2->second;
			CUser* pUser	= g_UserMng.GetUserByPlayerID( pMember->m_idPlayer );
			if( IsValidObj( (CObj*)pUser ) )
			{
				school[iIndex].nSize++;
				if( pUser->IsRegionAttr( RA_FIGHT ) )
				{
					if( !pUser->IsMode( OBSERVE_MODE ) )
					{
						school[iIndex].nSurvivor++;
						school[iIndex].nLevel	+= pUser->GetLevel();
					}
				}
			}
		}
		iIndex++;
	}
	// sort
	for( int j = 0; j < MAX_SCHOOL-1; j++ )
	{
		for( int k = j + 1; k < MAX_SCHOOL; k++ )
		{
			int nAvg1, nAvg2;
			if( school[j].nSurvivor == 0 )
				nAvg1	= 0;
			else
				nAvg1	= school[j].nLevel / school[j].nSurvivor;
			if( school[k].nSurvivor == 0 )
				nAvg2	= 0;
			else
				nAvg2	= school[k].nLevel / school[k].nSurvivor;

			if( ( school[j].nSurvivor < school[k].nSurvivor ) ||
				( school[j].nSurvivor == school[k].nSurvivor && nAvg1 < nAvg2 ) )
			{	// swap
				SCHOOL_ENTRY tmp;
				memcpy( &tmp, &school[j], sizeof(SCHOOL_ENTRY) );
				memcpy( &school[j], &school[k], sizeof(SCHOOL_ENTRY) );
				memcpy( &school[k], &tmp, sizeof( SCHOOL_ENTRY) );
			}
		}
	}

	short nElapse	= (short)( (int)( GetTickCount() - m_dwsbstart2 ) / (int)MIN( 1 ) );
	g_UserMng.AddSchoolReport( school, nElapse );	// neuz

	return TRUE;
}

BOOL CEveSchool::End( void )
{
// "/sbend"
	if( g_eLocal.GetState( EVE_SCHOOL_BATTLE ) != SBS_START2 )	// '/sbstart2'
		return FALSE;

	SCHOOL_ENTRY	school[MAX_SCHOOL];

	int iIndex	= 0;
	for( map<u_long, CGuild*>::iterator i = m_pSchoolMng->m_mapPGuild.begin();
		i != m_pSchoolMng->m_mapPGuild.end(); ++i )
	{
		CGuild* pSchool	= i->second;
		
		school[iIndex].id	= pSchool->m_idGuild;
		lstrcpy( school[iIndex].lpName, pSchool->m_szGuild );
		school[iIndex].nDead	= pSchool->m_nDead;

		for( map<u_long, CGuildMember*>::iterator i2 = pSchool->m_mapPMember.begin();
		i2 != pSchool->m_mapPMember.end(); ++i2 )
		{
			CGuildMember* pMember	= i2->second;
			CUser* pUser	= g_UserMng.GetUserByPlayerID( pMember->m_idPlayer );
			if( IsValidObj( (CObj*)pUser ) )
			{
				if( pUser->IsRegionAttr( RA_FIGHT ) )
				{
					if( !pUser->IsMode( OBSERVE_MODE ) )
					{
						school[iIndex].nSurvivor++;
						school[iIndex].nLevel	+= pUser->GetLevel();
					}
				}
				else
				{
					pUser->SetMode( OBSERVE_MODE );		// MAKE observe mode
					g_UserMng.AddModifyMode( pUser );
				}
			}
		}
		iIndex++;
	}

	// sort
	for( int j = 0; j < MAX_SCHOOL-1; j++ )
	{
		for( int k = j + 1; k < MAX_SCHOOL; k++ )
		{
			int nAvg1, nAvg2;
			if( school[j].nSurvivor == 0 )
				nAvg1	= 0;
			else
				nAvg1	= school[j].nLevel / school[j].nSurvivor;
			if( school[k].nSurvivor == 0 )
				nAvg2	= 0;
			else
				nAvg2	= school[k].nLevel / school[k].nSurvivor;

			if( ( school[j].nSurvivor < school[k].nSurvivor ) ||
				( school[j].nSurvivor == school[k].nSurvivor && nAvg1 < nAvg2 ) )
			{	// swap
				SCHOOL_ENTRY tmp;
				memcpy( &tmp, &school[j], sizeof(SCHOOL_ENTRY) );
				memcpy( &school[j], &school[k], sizeof(SCHOOL_ENTRY) );
				memcpy( &school[k], &tmp, sizeof( SCHOOL_ENTRY) );
			}
		}
	}

	if( g_eLocal.SetState( EVE_SCHOOL_BATTLE, SBS_END ) )
		g_UserMng.AddSetLocalEvent( EVE_SCHOOL_BATTLE, SBS_END );

	g_dpDBClient.SendSchoolReport( school );	// log
	g_UserMng.AddSchoolReport( school );	// neuz	

	return TRUE;
}

BOOL CEveSchool::LoadPos( LPCSTR lpszFileName )
{
	CScanner s;

	if( !s.Load( lpszFileName ) )
		return FALSE;
	
	u_long id;
	D3DXVECTOR3 vPos;

	id	= s.GetNumber();
	while( s.tok != FINISHED )
	{
		vPos.x	= s.GetFloat();
		vPos.y	= s.GetFloat();
		vPos.z	= s.GetFloat();

		if( id > MAX_SCHOOL || id < 1 )
			m_vDefault	= vPos;
		else
			m_avPos[id-1]	= vPos;
		id	= s.GetNumber();
	}
	return TRUE;
}

D3DXVECTOR3 CEveSchool::GetPos( u_long id )
{
	if( id > MAX_SCHOOL || id < 1 )
//		return D3DXVECTOR3( 0, 0, 0 );
		return m_vDefault;
	return m_avPos[id-1];
}

CEveSchool* CEveSchool::GetInstance()
{
	static	CEveSchool	sEveSchool;
	return & sEveSchool;
}
#endif // __WORLDSERVER

CGuildCombat::CGuildCombat()
{
	m_nState = CLOSE_STATE;
	m_nGCState = WAR_CLOSE_STATE;
	m_nGuildCombatIndex = 0;
	m_uWinGuildId = 0;
	m_nWinGuildCount = 0;
	m_uBestPlayer = 0;
	m_vecGCGetPoint.clear();
	m_vecGCPlayerPoint.clear();
#ifdef __WORLDSERVER
#if __VER >= 8 // __GUILDCOMBAT_85
	vecGCSendItem.clear();
#endif // __VER >= 8
#ifdef __S_BUG_GC
	m_vecGuildCombatMem.clear();
#else // __S_BUG_GC
	m_GuildCombatMem.clear();
#endif // __S_BUG_GC
	vecRequestRanking.clear();
	m_dwTime = 0;
	m_nStopWar = 0;
	m_nJoinPanya = 0;
	m_nGuildLevel = 0;
	m_nMaxJoinMember = 0;
	m_nMaxPlayerLife = 0;
	m_nMaxWarPlayer = 0;
	m_nMaxPlayerPercent = 0;
	m_nMaxGuildPercent = 0;
	m_nRequestCanclePercent = 0;
	m_nNotRequestPercent = 0;
	m_nItemPenya = 0;
	m_bMutex     = FALSE;
	m_bMutexMsg  = FALSE;

	memset( __AutoOpen, 0, sizeof(__AUTO_OPEN) * 7 );	

	m_nDay	= 0;
#endif // __WORLDSERVER
#ifdef __CLIENT
	m_bRequest = FALSE;
#endif // __CLIENT
}

CGuildCombat::~CGuildCombat()
{
	GuildCombatClear( 99 );
}

void CGuildCombat::GuildCombatClear( int nState )
{
	m_nState = CLOSE_STATE;
	m_nGCState = WAR_CLOSE_STATE;

	if( nState == 99 || nState == 1 )
	{
		m_nGuildCombatIndex = 0;
		m_uWinGuildId = 0;
		m_nWinGuildCount = 0;
		m_uBestPlayer = 0;
	}	

	m_vecGCGetPoint.clear();
#ifdef __WORLDSERVER

#ifdef __S_BUG_GC
	for( int gcmi = 0 ; gcmi < (int)( m_vecGuildCombatMem.size() ) ; ++gcmi )
		safe_delete( m_vecGuildCombatMem[gcmi] );
	m_vecGuildCombatMem.clear();
#else // __S_BUG_GC
	for( map<u_long, __GuildCombatMember*>::iterator i = m_GuildCombatMem.begin(); i != m_GuildCombatMem.end(); ++i )
		safe_delete( i->second );
	m_GuildCombatMem.clear();
#endif // __S_BUG_GC
	vecRequestRanking.clear();
	m_dwTime = 0;
	m_nStopWar = 0;
	if( g_eLocal.GetState( EVE_GUILDCOMBAT ) )
		g_DPCoreClient.SendGuildCombatState( CLOSE_STATE );		

	if( nState == 99 || nState == 3 )
	{
		m_nJoinPanya = 0;
		m_nGuildLevel = 0;
		m_nMaxJoinMember = 0;
		m_nMaxPlayerLife = 0;
		m_nMaxWarPlayer = 0;
		m_nMaxPlayerPercent = 0;
		m_nMaxGuildPercent = 0;
		m_nRequestCanclePercent = 0;
		m_nNotRequestPercent = 0;
		m_nItemPenya = 0;
	}
#endif // __WORLDSERVER
#ifdef __CLIENT
	m_bRequest = FALSE;
#endif // __CLIENT
}

void CGuildCombat::GuildCombatGameClear()
{
	GuildCombatClear( 2 );
}

void CGuildCombat::AddvecGCGetPoint( u_long uidGuildAttack, u_long uidGuildDefence, u_long uidPlayerAttack, u_long uidPlayerDefence, int nPoint,
									BOOL bKillDiffernceGuild, BOOL bMaster, BOOL bDefender, BOOL bLastLife )
{
	__GCGETPOINT GCGetPoint;
	GCGetPoint.uidGuildAttack = uidGuildAttack;
	GCGetPoint.uidGuildDefence = uidGuildDefence;
	GCGetPoint.uidPlayerAttack = uidPlayerAttack;
	GCGetPoint.uidPlayerDefence = uidPlayerDefence;
	GCGetPoint.nPoint = nPoint;
	GCGetPoint.bKillDiffernceGuild = bKillDiffernceGuild;
	GCGetPoint.bMaster = bMaster;
	GCGetPoint.bDefender = bDefender;
	GCGetPoint.bLastLife = bLastLife;
	m_vecGCGetPoint.push_back( GCGetPoint );
#ifdef __WORLDSERVER
	g_UserMng.AddGCLogRealTimeWorld( GCGetPoint );
#endif // __WORLDSERVER
}

void CGuildCombat::AddvecGCPlayerPoint( u_long uidPlayer, int nJob, int nPoint )
{
	__GCPLAYERPOINT GCPlayerPoint;
	GCPlayerPoint.uidPlayer = uidPlayer;
	GCPlayerPoint.nJob = nJob;
	GCPlayerPoint.nPoint = nPoint;
	m_vecGCPlayerPoint.push_back( GCPlayerPoint );
}

#ifdef __WORLDSERVER
void CGuildCombat::SelectPlayerClear( u_long uidGuild )
{
#ifdef __S_BUG_GC
	__GuildCombatMember* pGCMember = FindGuildCombatMember( uidGuild );
	if( pGCMember != NULL )
		pGCMember->SelectMemberClear();
#else // __S_BUG_GC
	map<u_long, __GuildCombatMember*>::iterator it = m_GuildCombatMem.find( uidGuild );
	if( it != m_GuildCombatMem.end() )
	{
		__GuildCombatMember* pGCMember = it->second;
		pGCMember->SelectMemberClear();
	}	
#endif // __S_BUG_GC
}

// 참가 신청
void CGuildCombat::JoinGuildCombat( u_long idGuild, DWORD dwPenya, BOOL bRequest )
{
#ifdef __S_BUG_GC
	__GuildCombatMember* pGCMember = FindGuildCombatMember( idGuild );
	if( pGCMember != NULL )
	{
		pGCMember->dwPenya = dwPenya;		
		pGCMember->bRequest = bRequest;
	}
	else
	{
		pGCMember	= new __GuildCombatMember;
		pGCMember->Clear();
		pGCMember->uGuildId = idGuild;
		pGCMember->dwPenya = dwPenya;
		pGCMember->bRequest = bRequest;
		m_vecGuildCombatMem.push_back( pGCMember );
	}
#else // __S_BUG_GC
	map<u_long, __GuildCombatMember*>::iterator it = m_GuildCombatMem.find( idGuild );
	if( it != m_GuildCombatMem.end() )
	{
		__GuildCombatMember* pGCMember = it->second;
		pGCMember->dwPenya = dwPenya;		
		pGCMember->bRequest = bRequest;
	}
	else
	{
		__GuildCombatMember* pGCMember	= new __GuildCombatMember;
		pGCMember->Clear();
		pGCMember->dwPenya = dwPenya;
		pGCMember->bRequest = bRequest;
		m_GuildCombatMem.insert( map<u_long, __GuildCombatMember*>::value_type( idGuild, pGCMember ) );
	}
#endif // __S_BUG_GC

	SetRequestRanking();
}
// 참가 탈퇴
void CGuildCombat::OutGuildCombat( u_long idGuild )
{
	if( !g_eLocal.GetState( EVE_GUILDCOMBAT ) )
		return; 
	
#ifdef __S_BUG_GC
	__GuildCombatMember* pGCMember = FindGuildCombatMember( idGuild );
	if( pGCMember != NULL ) // 취소시 바로 우편함으로 지급
	{
		CGuild* pGuild = g_GuildMng.GetGuild( idGuild );
		if( pGuild && pGCMember->bRequest )
		{
			CItemElem itemElem;
			char szMsg[1000];
			sprintf( szMsg, "%s", prj.GetText( TID_GAME_GC_CANCELREQUEST ) );
			char szMsg1[1000];
			sprintf( szMsg1, "%s", prj.GetText( TID_GAME_GC_CANCELREQUEST1 ) );
			g_dpDBClient.SendQueryPostMail( pGuild->m_idMaster, 0, itemElem, MulDiv( pGCMember->dwPenya, m_nRequestCanclePercent, 100 ), szMsg, szMsg1 );
		}		
		pGCMember->bRequest = FALSE;
		pGCMember->dwPenya = 0;
	}
#else // __S_BUG_GC
	map<u_long, __GuildCombatMember*>::iterator it = m_GuildCombatMem.find( idGuild );
	if( it != m_GuildCombatMem.end() )
	{
		__GuildCombatMember* pGCMember = it->second;

		// 취소시 바로 우편함으로 지급
		CGuild* pGuild = g_GuildMng.GetGuild( idGuild );
		if( pGuild && pGCMember->bRequest )
		{
			CItemElem itemElem;
			char szMsg[1000];
			sprintf( szMsg, "%s", prj.GetText( TID_GAME_GC_CANCELREQUEST ) );
			char szMsg1[1000];
			sprintf( szMsg1, "%s", prj.GetText( TID_GAME_GC_CANCELREQUEST1 ) );
			g_dpDBClient.SendQueryPostMail( pGuild->m_idMaster, 0, itemElem, MulDiv( pGCMember->dwPenya, m_nRequestCanclePercent, 100 ), szMsg, szMsg1 );
		}		
		pGCMember->bRequest = FALSE;
		pGCMember->dwPenya = 0;
	}
#endif // __S_BUG_GC
	SetRequestRanking();
}

// 참가한 길드가 전투에 참가할 유저를 선택
void CGuildCombat::AddSelectPlayer( u_long idGuild, u_long uidPlayer )
{
#ifdef __S_BUG_GC
	__GuildCombatMember* pGCMember = FindGuildCombatMember( idGuild );
	if( pGCMember != NULL )
	{
		__JOINPLAYER* pJoinPlayer = new __JOINPLAYER;
		pJoinPlayer->nlife = m_nMaxPlayerLife;	
		pJoinPlayer->uidPlayer = uidPlayer;
		pJoinPlayer->nMap = 99;
		pJoinPlayer->nPoint = 0;
		pJoinPlayer->uKillidGuild = 0;
		pJoinPlayer->dwTelTime = 0;
		pGCMember->vecGCSelectMember.push_back( pJoinPlayer );
	}
#else // __S_BUG_GC
	map<u_long, __GuildCombatMember*>::iterator it = m_GuildCombatMem.find( idGuild );
	if( it != m_GuildCombatMem.end() )
	{
		__GuildCombatMember* pGCMember = it->second;
		__JOINPLAYER* pJoinPlayer = new __JOINPLAYER;
		pJoinPlayer->nlife = m_nMaxPlayerLife;	
		pJoinPlayer->uidPlayer = uidPlayer;
//		pJoinPlayer->bEntry = FALSE;
		pJoinPlayer->nMap = 99;
		pJoinPlayer->nPoint = 0;
		pJoinPlayer->uKillidGuild = 0;
		pJoinPlayer->dwTelTime = 0;
		pGCMember->vecGCSelectMember.push_back( pJoinPlayer );
	}
#endif // __S_BUG_GC
}
void CGuildCombat::GetSelectPlayer( u_long idGuild, vector<__JOINPLAYER> &vecSelectPlayer )
{
#ifdef __S_BUG_GC
	__GuildCombatMember* pGCMember = FindGuildCombatMember( idGuild );
	if( pGCMember != NULL )
	{
		for( int veci = 0 ; veci < (int)( pGCMember->vecGCSelectMember.size() ) ; ++veci )
		{
			__JOINPLAYER* pJoinPlayer =  pGCMember->vecGCSelectMember[veci];
			vecSelectPlayer.push_back( *pJoinPlayer );
		}
	}	
#else // __S_BUG_GC
	map<u_long, __GuildCombatMember*>::iterator it = m_GuildCombatMem.find( idGuild );
	if( it != m_GuildCombatMem.end() )
	{
		__GuildCombatMember* pGCMember = it->second;
		for( int veci = 0 ; veci < pGCMember->vecGCSelectMember.size() ; ++veci )
		{
			__JOINPLAYER* pJoinPlayer =  pGCMember->vecGCSelectMember[veci];
			vecSelectPlayer.push_back( *pJoinPlayer );
		}
	}	
#endif // __S_BUG_GC
}

void CGuildCombat::JoinWar( CUser* pUser, int nMap , BOOL bWar)
{
	D3DXVECTOR3 v3Pos[4];
	v3Pos[0] = D3DXVECTOR3(1263, 83, 1303);
	v3Pos[1] = D3DXVECTOR3(1310, 83, 1303);
	v3Pos[2] = D3DXVECTOR3(1265, 83, 1254);
	v3Pos[3] = D3DXVECTOR3(1314, 83, 1253);

	int nPos = xRandom(4);
	if( nMap == 99 )
		nMap = nPos;

	if( m_nState == WAR_STATE )
	{
		if( m_nGCState == WAR_WAR_STATE )
			pUser->m_nGuildCombatState = 1;
		else if( m_nGCState == MAINTENANCE_STATE )
			pUser->m_nGuildCombatState = 2;

		if( m_nGCState == WAR_WAR_STATE || m_nGCState == MAINTENANCE_STATE )
			g_UserMng.AddGuildCombatUserState( pUser );
	}
	
	((CMover*)pUser)->REPLACE( g_uIdofMulti, WI_WORLD_GUILDWAR, v3Pos[nMap], REPLACE_NORMAL, nDefaultLayer );
	pUser->m_vtInfo.SetOther( NULL );

	if( bWar )
	{
	}
	else
	{
		pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_WELCOME) );
		pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_MASTER_MAP) );
	}
}

void CGuildCombat::OutWar( CUser* pUser, CUser* pLeader, BOOL bLogOut )
{
#ifdef __S_BUG_GC
	__GuildCombatMember* pGCMember = FindGuildCombatMember( pUser->m_idGuild );
	if( pGCMember != NULL )
	{
		// 먼저 다른 유저를 찾음
		SetPlayerChange( pUser, pLeader );
		// 디펜더가 로그아웃중이면 다른 출전자가 자동으로 디펜더가 됨

		if( bLogOut )
		{
			if( pUser->m_idPlayer == pGCMember->m_uidDefender )	// 디펜더 검사
			{
				CGuild* pGuild = g_GuildMng.GetGuild( pUser->m_idGuild  );
				if( pGuild )
				{
					vector<int> vecDefender;
					vecDefender.clear();
					for( int vecii0 = 0 ; vecii0 < (int)( pGCMember->vecGCSelectMember.size() ) ; ++vecii0 )
					{
						__JOINPLAYER* pJoinPlayer3 = pGCMember->vecGCSelectMember[vecii0];
						CMover* pMover = prj.GetUserByID( pJoinPlayer3->uidPlayer );
						if( IsValidObj( pMover ) )
						{						
							if( pGuild->IsMaster( pMover->m_idPlayer ) == FALSE && pGCMember->m_uidDefender != pMover->m_idPlayer && 0 < pJoinPlayer3->nlife )
							{
								vecDefender.push_back( vecii0 );
							}
						}
					}
					if( 0 < vecDefender.size() )
					{
						int nPlayerPos = xRandom( vecDefender.size() );
						int nDefender = vecDefender[nPlayerPos];
						__JOINPLAYER* pJoinPlayer4 = pGCMember->vecGCSelectMember[nDefender];
						pGCMember->m_uidDefender = pJoinPlayer4->uidPlayer;
					}
				}
			}	// if( pUser->m_idPlayer == pGCMember->m_uidDefender )	// 디펜더 검사
		}	// if( bLogOut )
	}
#else __S_BUG_GC
	map<u_long, __GuildCombatMember*>::iterator it = m_GuildCombatMem.find( pUser->m_idGuild );
	if( it != m_GuildCombatMem.end() )
	{
		// 먼저 다른 유저를 찾음
		SetPlayerChange( pUser, pLeader );
		// 디펜더가 로그아웃중이면 다른 출전자가 자동으로 디펜더가 됨

		if( bLogOut )
		{
			__GuildCombatMember* pGCMember	= it->second;
			if( pUser->m_idPlayer == pGCMember->m_uidDefender )	// 디펜더 검사
			{
				CGuild* pGuild = g_GuildMng.GetGuild( pUser->m_idGuild  );
				if( pGuild )
				{
					vector<int> vecDefender;
					vecDefender.clear();
					for( int vecii0 = 0 ; vecii0 < pGCMember->vecGCSelectMember.size() ; ++vecii0 )
					{
						__JOINPLAYER* pJoinPlayer3 = pGCMember->vecGCSelectMember[vecii0];
						CMover* pMover = prj.GetUserByID( pJoinPlayer3->uidPlayer );
						if( IsValidObj( pMover ) )
						{						
							if( pGuild->IsMaster( pMover->m_idPlayer ) == FALSE && pGCMember->m_uidDefender != pMover->m_idPlayer && 0 < pJoinPlayer3->nlife )
							{
								vecDefender.push_back( vecii0 );
							}
						}
					}
					if( 0 < vecDefender.size() )
					{
						int nPlayerPos = xRandom( vecDefender.size() );
						int nDefender = vecDefender[nPlayerPos];
						__JOINPLAYER* pJoinPlayer4 = pGCMember->vecGCSelectMember[nDefender];
						pGCMember->m_uidDefender = pJoinPlayer4->uidPlayer;
					}
				}
			}	// if( pUser->m_idPlayer == pGCMember->m_uidDefender )	// 디펜더 검사
		}	// if( bLogOut )
	}
#endif // __S_BUG_GC
	g_UserMng.AddGCGuildStatus( pUser->m_idGuild );
	g_UserMng.AddGCWarPlayerlist();

	// 중간에 게임 중단해도 되는지 검사
	UserOutGuildCombatResult( pUser );
}
void CGuildCombat::JoinObserver( CUser* pUser )
{
	if( pUser->IsBaseJob() )
	{
		pUser->AddDiagText( prj.GetText(TID_GAME_GUILDCOMBAT_WAR_ENTER) );
		return;
	}
	
	// 버프 없애기
	pUser->RemoveCommonBuffs();
	g_UserMng.AddRemoveAllSkillInfluence( pUser );
	((CMover*)pUser)->REPLACE( g_uIdofMulti, WI_WORLD_GUILDWAR, D3DXVECTOR3( 1361.6f, 0.0f, 1273.3f ), REPLACE_NORMAL, nDefaultLayer );
	pUser->m_vtInfo.SetOther( NULL );
	// 길드대전에 오신걸 환영합니다.
	pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_WELCOME) );
	// 관전자 모드로 입장하셨습니다.
	pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_ZOOM_USE) );

	if( m_nGCState <= ENTER_STATE )
		pUser->AddGCWarPlayerlist();

	if( IsSelectPlayer( pUser ) )
	{
		pUser->m_nGuildCombatState		= 2;
		g_UserMng.AddGuildCombatUserState( (CMover*)pUser );
#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
		if( ENTER_STATE <= m_nGCState && m_nGCState <= WAR_CLOSE_STATE )
			g_DPCoreClient.SendGCRemoveParty( pUser->GetPartyId(), pUser->m_idPlayer );
#else // __IMPROVE_SYSTEM_VER15
		// 참가자 들은 파티 해제
		if( m_nGCState == ENTER_STATE )
		{
			g_DPCoreClient.SendGCRemoveParty( pUser->GetPartyId(), pUser->m_idPlayer );
		}
		else if( MAINTENANCE_STATE <= m_nGCState && m_nGCState <= WAR_CLOSE_STATE )
		{
#ifdef __S_BUG_GC
			__GuildCombatMember* pGCMember = FindGuildCombatMember( pUser->m_idGuild );
			if( pGCMember != NULL )
			{
				CMover* pLeader = NULL;
				for( int veci = 0 ; veci < pGCMember->vecGCSelectMember.size() ; ++veci )
				{
						__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];
					if( pJoinPlayer->uidPlayer != pUser->m_idPlayer )
					{
						CMover* pMover = prj.GetUserByID( pJoinPlayer->uidPlayer );
						if( IsValidObj( pMover ) && pMover->GetWorld() && pMover->GetWorld()->GetID() == WI_WORLD_GUILDWAR )
						{
							if( 0 < pMover->GetPartyId() )
							{
								pLeader = pMover;
								break;
							}
						}
					}
				}
				
				if( pLeader != NULL )
				{
					if( 0 < pUser->GetPartyId() )
					{
						if( pUser->GetPartyId() != pLeader->GetPartyId() )
							g_DPCoreClient.SendGCRemoveParty( pUser->GetPartyId(), pUser->m_idPlayer );
					}
					
					if( pUser->GetPartyId() != pLeader->GetPartyId() )
						g_DPCoreClient.SendGCAddParty( pLeader->m_idPlayer, pLeader->GetLevel(), pLeader->GetJob(), pLeader->GetSex(), 
						pUser->m_idPlayer, pUser->GetLevel(), pUser->GetJob(), pUser->GetSex() );
				}
			}
#else // __S_BUG_GC
			map<u_long, __GuildCombatMember*>::iterator ita = m_GuildCombatMem.find( pUser->m_idGuild );
			if( ita != m_GuildCombatMem.end() )
			{
				__GuildCombatMember* pGuildCombatMem = ita->second;
				
				CMover* pLeader = NULL;
				for( int veci = 0 ; veci < pGuildCombatMem->vecGCSelectMember.size() ; ++veci )
				{
						__JOINPLAYER* pJoinPlayer = pGuildCombatMem->vecGCSelectMember[veci];
					if( pJoinPlayer->uidPlayer != pUser->m_idPlayer )
					{
						CMover* pMover = prj.GetUserByID( pJoinPlayer->uidPlayer );
						if( IsValidObj( pMover ) && pMover->GetWorld() && pMover->GetWorld()->GetID() == WI_WORLD_GUILDWAR )
						{
							if( 0 < pMover->GetPartyId() )
							{
								pLeader = pMover;
								break;
							}
						}
					}
				}
				
				if( pLeader != NULL )
				{
					if( 0 < pUser->GetPartyId() )
					{
						if( pUser->GetPartyId() != pLeader->GetPartyId() )
							g_DPCoreClient.SendGCRemoveParty( pUser->GetPartyId(), pUser->m_idPlayer );
					}
					
					if( pUser->GetPartyId() != pLeader->GetPartyId() )
						g_DPCoreClient.SendGCAddParty( pLeader->m_idPlayer, pLeader->GetLevel(), pLeader->GetJob(), pLeader->GetSex(), 
						pUser->m_idPlayer, pUser->GetLevel(), pUser->GetJob(), pUser->GetSex() );
				}
			}
#endif // __S_BUG_GC
		}
#endif // __IMPROVE_SYSTEM_VER15
	}
	g_UserMng.AddGCGuildStatus( pUser->m_idGuild, pUser );
	g_UserMng.AddGCGuildPrecedence( pUser );
	g_UserMng.AddGCPlayerPrecedence( pUser );	
}

void CGuildCombat::GuildCombatRequest( CUser* pUser, DWORD dwPenya )
{
	CGuild* pGuild	= g_GuildMng.GetGuild( pUser->m_idGuild );
	if( pGuild && pGuild->IsMaster( pUser->m_idPlayer ) )
	{
		// 시간 검사 ( 월 ~ 금 )
		//길드대전중에는 신청할 수 없습니다.
		if( m_nState != CGuildCombat::CLOSE_STATE )
		{
			pUser->AddDiagText( prj.GetText(TID_GAME_GUILDCOMBAT_REQUESTEND) );
			return;
		}

		// 길드레벨 m_nGuildLevel의 값 이상인지 검사
		if( pGuild->m_nLevel < m_nGuildLevel )
		{
			CString strMsg;
			strMsg.Format( prj.GetText( TID_GAME_GUILDCOMBAT_REQUEST_LEVEL ), m_nGuildLevel );
			pUser->AddDiagText( strMsg );
			return;
		}

		// 기존에 신청 한 페냐
		DWORD dwExistingPenya = 0;
#ifdef __S_BUG_GC
		__GuildCombatMember* pGCMember = FindGuildCombatMember( pUser->m_idGuild );
		if( pGCMember != NULL )
			dwExistingPenya = pGCMember->dwPenya;
#else // __S_BUG_GC
		map<u_long, __GuildCombatMember*>::iterator it	= m_GuildCombatMem.find( pUser->m_idGuild );
		if( it != m_GuildCombatMem.end() )
		{
			__GuildCombatMember *pGuildCombatMember	= it->second;
			dwExistingPenya = pGuildCombatMember->dwPenya;
		}
#endif // __S_BUG_GC

		// INT_MAX값에 넘지 않아야 한다
		if( dwPenya > INT_MAX )
			return;

		// 기본 페냐(m_nJoinPenya) 보다 더 많이 가지고 있는지 확인
		if( (int)( dwPenya ) < m_nJoinPanya )
		{
			CString strMsg;
			strMsg.Format( prj.GetText( TID_GAME_GUILDCOMBAT_REQUEST_BASEPENYA ), m_nJoinPanya );
			pUser->AddDiagText( strMsg );
			pUser->AddText( prj.GetText(TID_GAME_LACKMONEY) );	
			return;
		}
		
		// 전에 신청한 페냐 보다 더 많아야 한다.
		if( dwPenya <= dwExistingPenya )
		{
			CString strMsg;
			strMsg.Format( prj.GetText( TID_GAME_GUILDCOMBAT_REQUEST_PENYA ), dwPenya );
			pUser->AddDiagText( strMsg );
			return;
		}


		// 신청한 페냐가 가지고 있는지 확인
		if( pUser->GetGold() < (int)( (dwPenya-dwExistingPenya) ) )
		{
			pUser->AddDiagText( prj.GetText( TID_GAME_GUILDCOMBAT_REQUEST_NOTPENYA ) );
			return;
		}

		LogItemInfo aLogItem;
		aLogItem.Action = "9";
		aLogItem.SendName = pUser->GetName();
		aLogItem.RecvName = "GUILDCOMBAT";
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		aLogItem.Gold = pUser->GetGold();
		aLogItem.Gold2 = pUser->GetGold() -(dwPenya-dwExistingPenya);
		//aLogItem.ItemName = "SEED";
		_stprintf( aLogItem.szItemName, "%d", II_GOLD_SEED1 );
		aLogItem.itemNumber = dwPenya-dwExistingPenya;
		g_DPSrvr.OnLogItem( aLogItem );

		pUser->AddGold( (int)( (-1) * (int)( dwPenya-dwExistingPenya ) ) );
		g_dpDBClient.SendInGuildCombat( pUser->m_idGuild, dwPenya, dwExistingPenya );
	}
	else
	{
		//길드가 없거나 길드장이 아닙니다.
		pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_NOT_GUILD_LEADER) );
	}
}

void CGuildCombat::GuildCombatCancel( CUser* pUser )
{
	CGuild* pGuild	= g_GuildMng.GetGuild( pUser->m_idGuild );
	if( pGuild && pGuild->IsMaster( pUser->m_idPlayer ) )
	{
		// 시간 검사 ( 월 ~ 금 )
#ifdef __S_BUG_GC
		__GuildCombatMember* pGCMember = FindGuildCombatMember( pUser->m_idGuild );
		if( pGCMember != NULL )
		{
			if( pGCMember->bRequest )
			{
				if( m_nState != CGuildCombat::CLOSE_STATE )
				{
					pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_NOT_USE));	// 지금은 길드대전중에는 길드대전 탈퇴를 할수 없습니다
					return;
				}
				g_dpDBClient.SendOutGuildCombat( pUser->m_idGuild );
			}
			else
			{
				pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_NOTAPP) );
			}
		}
#else // __S_BUG_GC
		map<u_long, __GuildCombatMember*>::iterator it	= m_GuildCombatMem.find( pUser->m_idGuild );
		if( it != m_GuildCombatMem.end() )
		{
			__GuildCombatMember* pGCMember = it->second;

			if( pGCMember->bRequest )
			{
				if( m_nState != CGuildCombat::CLOSE_STATE )
				{
					pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_NOT_USE));	// 지금은 길드대전중에는 길드대전 탈퇴를 할수 없습니다
					return;
				}
				g_dpDBClient.SendOutGuildCombat( pUser->m_idGuild );
			}
			else
			{
				pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_NOTAPP) );
			}
		}
#endif // __S_BUG_GC
		else
		{
			//길드대전 신청을 하지 않았습니다.
			pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_NOTAPP) );
		}
	}
	else
	{
		//길드가 없거나 길드장이 아닙니다.
		pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_NOT_GUILD_LEADER) );
	}
}
// 중간에 캐릭터가 죽거나 로그아웃시에 승리길드가 나오면 대전 종료
void CGuildCombat::UserOutGuildCombatResult( CUser* pUser )
{
	int nCount = 0;
#ifdef __S_BUG_GC
	__GuildCombatMember* pGCMember = FindGuildCombatMember( pUser->m_idGuild );
	if( pGCMember != NULL )
	{
		for( int veci = 0 ; veci < (int)( pGCMember->vecGCSelectMember.size() ) ; ++veci )
		{
			__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];
			// 생명이 있는 캐릭이 있으면 검사 안해도 됨.
			if( 0 < pJoinPlayer->nlife )
				return;
		}

		// 다른길드의 상태 파악( 길드의 전투작 1개이상인지 검사 1개의 길드만 나오면 게임 종료 )
		for( int nVeci = 0 ; nVeci < (int)( vecRequestRanking.size() ) ; ++nVeci )
		{
			if( nVeci >= m_nMaxGuild )
				break;
			
			__REQUESTGUILD RequestGuild = vecRequestRanking[nVeci];
			__GuildCombatMember *pGCMember2 = FindGuildCombatMember( RequestGuild.uidGuild );
			if( pGCMember2 != NULL )
			{
				if( pUser->m_idGuild != pGCMember2->uGuildId )
				{
					BOOL bLive = FALSE;
					for( int veci2 = 0 ; veci2 < (int)( pGCMember2->vecGCSelectMember.size() ) ; ++veci2 )
					{
						__JOINPLAYER* pJoinPlayer = pGCMember2->vecGCSelectMember[veci2];
						if( 0 < pJoinPlayer->nlife )
							bLive = TRUE;
					}
					if( bLive )
					{
						++nCount;
						if( nCount == 2 )
						{
							return; // 2개 이상이므로 계속 진행
						}
					}
				}
			}
		}
		// 길드가 하나밖에 없으므로 게임 종료
		if( nCount == 1 )
			m_nStopWar = 1;
	}
#else // __S_BUG_GC
	map<u_long, __GuildCombatMember*>::iterator it = m_GuildCombatMem.find( pUser->m_idGuild );
	if( it != m_GuildCombatMem.end() )
	{
		__GuildCombatMember *pGCMember	= it->second;
		for( int veci = 0 ; veci < pGCMember->vecGCSelectMember.size() ; ++veci )
		{
			__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];
			// 생명이 있는 캐릭이 있으면 검사 안해도 됨.
			if( 0 < pJoinPlayer->nlife )
				return;
		}

		// 다른길드의 상태 파악( 길드의 전투작 1개이상인지 검사 1개의 길드만 나오면 게임 종료 )
		for( int nVeci = 0 ; nVeci < vecRequestRanking.size() ; ++nVeci )
		{
			if( nVeci >= m_nMaxGuild )
				break;
			
			__REQUESTGUILD RequestGuild = vecRequestRanking[nVeci];
			map<u_long, __GuildCombatMember*>::iterator it2 = m_GuildCombatMem.find( RequestGuild.uidGuild );
			if( it2 != m_GuildCombatMem.end() )
			{
				__GuildCombatMember *pGCMember2 = it2->second;
				if( pUser->m_idGuild != it2->first )
				{
					BOOL bLive = FALSE;
					for( int veci2 = 0 ; veci2 < pGCMember2->vecGCSelectMember.size() ; ++veci2 )
					{
						__JOINPLAYER* pJoinPlayer = pGCMember2->vecGCSelectMember[veci2];
						if( 0 < pJoinPlayer->nlife )
							bLive = TRUE;
					}
					if( bLive )
					{
						++nCount;
						if( nCount == 2 )
						{
							return; // 2개 이상이므로 계속 진행
						}
					}
				}
			}
		}
		// 길드가 하나밖에 없으므로 게임 종료
		if( nCount == 1 )
			m_nStopWar = 1;
	}
#endif // __S_BUG_GC
}

void CGuildCombat::GuildCombatResult( BOOL nResult, u_long idGuildWin )
{
	vector<u_long> vecSameidGuildWin; 
	vecSameidGuildWin.clear();
	int nMaxPoint = -1;

	// 최고 점수를 가지고 옴( 최고 점수가 공동으로 가지고 있으면 vecSameidGuildWin에 넣음 )
	for( int nVeci = 0 ; nVeci < (int)( vecRequestRanking.size() ) ; ++nVeci )
	{
		if( nVeci >= m_nMaxGuild )
			break;
		
		__REQUESTGUILD RequestGuild = vecRequestRanking[nVeci];
#ifdef __S_BUG_GC
		__GuildCombatMember* pGCMember = FindGuildCombatMember( RequestGuild.uidGuild );
		if( pGCMember != NULL )
		{
			int nPoint = 0;
			for( int veci = 0 ; veci < (int)( pGCMember->vecGCSelectMember.size() ) ; ++veci )
			{
				__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];
				nPoint += pJoinPlayer->nPoint + pJoinPlayer->nlife;
			}
			pGCMember->nGuildPoint = nPoint;
			// 순위 바뀜
			if( nMaxPoint < nPoint )
			{
				nMaxPoint = nPoint;
				vecSameidGuildWin.clear();
				vecSameidGuildWin.push_back( pGCMember->uGuildId );
			}
			else if( nMaxPoint == nPoint )
			{
				vecSameidGuildWin.push_back( pGCMember->uGuildId );
			}
		}
#else // __S_BUG_GC
		map<u_long, __GuildCombatMember*>::iterator it = m_GuildCombatMem.find( RequestGuild.uidGuild );
		if( it != m_GuildCombatMem.end() )
		{
			int nPoint = 0;
			__GuildCombatMember* pGCMember = it->second;
			for( int veci = 0 ; veci < pGCMember->vecGCSelectMember.size() ; ++veci )
			{
				__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];
				nPoint += pJoinPlayer->nPoint + pJoinPlayer->nlife;
			}
			pGCMember->nGuildPoint = nPoint;
			// 순위 바뀜
			if( nMaxPoint < nPoint )
			{
				nMaxPoint = nPoint;
				vecSameidGuildWin.clear();
				vecSameidGuildWin.push_back( it->first );
			}
			else if( nMaxPoint == nPoint )
			{
				vecSameidGuildWin.push_back( it->first );
			}
		}
#endif // __S_BUG_GC
	}

	// 공동우승이 있으면 부활기회의 합의 값이 가장 큰 길드 승리
	int nMaxLife = 0;
	if( 1 < vecSameidGuildWin.size() )
	{
		vector<u_long> vecSameidGuildWinTmp;
		vecSameidGuildWinTmp.clear();
		for( int vecSame = 0 ; vecSame < (int)( vecSameidGuildWin.size() ) ; ++vecSame )
		{
			int nLife = 0;
#ifdef __S_BUG_GC
			__GuildCombatMember* pGCMember = FindGuildCombatMember( vecSameidGuildWin[vecSame] );
			if( pGCMember != NULL )
			{
				for( int veci2 = 0 ; veci2 < (int)( pGCMember->vecGCSelectMember.size() ) ; ++veci2 )
				{
					__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci2];
					nLife += pJoinPlayer->nlife;
				}
				
				// 순위 바뀜
				if( nMaxLife < nLife )
				{
					nMaxLife = nLife;
					vecSameidGuildWinTmp.clear();
					vecSameidGuildWinTmp.push_back( pGCMember->uGuildId );
				}
				else if( nMaxLife == nLife )
				{
					vecSameidGuildWinTmp.push_back( pGCMember->uGuildId );
				}
			}
#else // __S_BUG_GC
			map<u_long, __GuildCombatMember*>::iterator it = m_GuildCombatMem.find( vecSameidGuildWin[vecSame] );
			if( it != m_GuildCombatMem.end() )
			{
				__GuildCombatMember* pGCMember = it->second;
				for( int veci2 = 0 ; veci2 < pGCMember->vecGCSelectMember.size() ; ++veci2 )
				{
					__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci2];
					nLife += pJoinPlayer->nlife;
				}
				
				// 순위 바뀜
				if( nMaxLife < nLife )
				{
					nMaxLife = nLife;
					vecSameidGuildWinTmp.clear();
					vecSameidGuildWinTmp.push_back( it->first );
				}
				else if( nMaxLife == nLife )
				{
					vecSameidGuildWinTmp.push_back( it->first );
				}
			}
#endif // __S_BUG_GC
		}
		vecSameidGuildWin = vecSameidGuildWinTmp;
	}

	// 공동우승이 있으면 생명이 있는 유저의 평균레벨이 큰 길드 승리
	float fMaxAvgLv = 0.0f;
	if( 1 < vecSameidGuildWin.size() )
	{
		vector<u_long> vecSameidGuildWinTmp;
		vecSameidGuildWinTmp.clear();
		for( int vecSame = 0 ; vecSame < (int)( vecSameidGuildWin.size() ) ; ++vecSame )
		{
			float fAvgLv = 0.0f;
			int nSubLevel = 0;
			int nValidObjCount = 0;
#ifdef __S_BUG_GC
			__GuildCombatMember* pGCMember = FindGuildCombatMember( vecSameidGuildWin[vecSame] );
			if( pGCMember != NULL )
			{
				for( int veci2 = 0 ; veci2 < (int)( pGCMember->vecGCSelectMember.size() ) ; ++veci2 )
				{
					__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci2];
					if( 0 < pJoinPlayer->nlife )
					{
						CUser* pUsertmp = (CUser*)prj.GetUserByID( pJoinPlayer->uidPlayer );
						if( IsValidObj( pUsertmp ) )
						{
							nSubLevel += pUsertmp->GetLevel();
							++nValidObjCount;
						}
					}
				}
				
				if( nValidObjCount )
				{
					fAvgLv = (float)( nSubLevel / nValidObjCount );
				}
				
				// 순위 바뀜
				if( fMaxAvgLv < fAvgLv )
				{
					fMaxAvgLv = fAvgLv;
					vecSameidGuildWinTmp.clear();
					vecSameidGuildWinTmp.push_back( pGCMember->uGuildId );
				}
				else if( fMaxAvgLv == fAvgLv )
				{
					vecSameidGuildWinTmp.push_back( pGCMember->uGuildId );
				}
			}
#else // __S_BUG_GC
			map<u_long, __GuildCombatMember*>::iterator it = m_GuildCombatMem.find( vecSameidGuildWin[vecSame] );
			if( it != m_GuildCombatMem.end() )
			{
				__GuildCombatMember* pGCMember = it->second;
				for( int veci2 = 0 ; veci2 < pGCMember->vecGCSelectMember.size() ; ++veci2 )
				{
					__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci2];
					if( 0 < pJoinPlayer->nlife )
					{
						CUser* pUsertmp = (CUser*)prj.GetUserByID( pJoinPlayer->uidPlayer );
						if( IsValidObj( pUsertmp ) )
						{
							nSubLevel += pUsertmp->GetLevel();
							++nValidObjCount;
						}
					}
				}
				
				if( nValidObjCount )
				{
					fAvgLv = nSubLevel / nValidObjCount;
				}
				
				// 순위 바뀜
				if( fMaxAvgLv < fAvgLv )
				{
					fMaxAvgLv = fAvgLv;
					vecSameidGuildWinTmp.clear();
					vecSameidGuildWinTmp.push_back( it->first );
				}
				else if( fMaxAvgLv == fAvgLv )
				{
					vecSameidGuildWinTmp.push_back( it->first );
				}
			}
#endif // __S_BUG_GC
		}
		vecSameidGuildWin = vecSameidGuildWinTmp;
	}

	char str[512] = {0,};
	CString strPrizeMsg;

	// 걍 맨처음 길드가 우승( 공동우승이어도.. )
	if( 0 < vecSameidGuildWin.size() )
	{
		CGuild* pGuild	= g_GuildMng.GetGuild( vecSameidGuildWin[0] );
		if( pGuild )
		{
			if( g_eLocal.GetState( EVE_GUILDCOMBAT ) )
			{
				g_UserMng.AddWorldCreateSfxObj( XI_NAT_ROCKET02, 1291.0f, 85.0f, 1279.0f, FALSE, WI_WORLD_GUILDWAR );
				g_DPCoreClient.SendPlayMusic( WI_WORLD_GUILDWAR, BGM_IN_FITUP );
			}
			
			// 연승 계산
			if( m_uWinGuildId == vecSameidGuildWin[0] )
				++m_nWinGuildCount;
			else
				m_nWinGuildCount = 1;
			
			m_uWinGuildId = vecSameidGuildWin[0];
			sprintf( str, prj.GetText(TID_GAME_GUILDCOMBAT_END) );
			g_DPCoreClient.SendSystem( str );
			g_DPCoreClient.SendCaption( str );
			sprintf( str, prj.GetText(TID_GAME_GUILDCOMBAT_WINNER), pGuild->m_szGuild );
#if __VER >= 8 // __GUILDCOMBAT_85
			int nBufWinGuildCount = m_nWinGuildCount;
			if( m_nMaxGCSendItem < m_nWinGuildCount )
				nBufWinGuildCount = m_nMaxGCSendItem;
			// 연승 아이템 주기
			CString strGuildMsg;
#if __VER < 11 // __GUILDCOMBATCHIP
			strGuildMsg.Format( prj.GetText(TID_GAME_GUILDCOMBAT_PRIZEMSG), pGuild->m_szGuild, m_nWinGuildCount );
			m_vecstrGuildMsg.push_back( strGuildMsg );
			strGuildMsg.Format( prj.GetText(TID_GAME_GUILDCOMBAT_PRIZEMSG1 ) );
			m_vecstrGuildMsg.push_back( strGuildMsg );
#endif // __GUILDCOMBATCHIP
			for( int si = 0 ; si < (int)( vecGCSendItem.size() ) ; ++si )
			{
				if( vecGCSendItem[si].nWinCount != nBufWinGuildCount )
					continue;

				// 길드창고에 넣기
				CItemElem itemElem;
				itemElem.m_dwItemId = vecGCSendItem[si].dwItemId;
				itemElem.m_nItemNum	= vecGCSendItem[si].nItemNum;
				itemElem.m_nHitPoint = itemElem.GetProp()->dwEndurance;
				itemElem.SetSerialNumber();
				if ( pGuild->m_GuildBank.Add( &itemElem ) )
				{
					LogItemInfo aLogItem;
					aLogItem.Action = "W";
					aLogItem.SendName = "GUILDCOMBAT";
					aLogItem.RecvName = "GUILDBANK";
					g_DPSrvr.OnLogItem( aLogItem, &itemElem, vecGCSendItem[si].nItemNum );
					
					g_DPSrvr.UpdateGuildBank( pGuild, GUILD_PUT_ITEM, 0, 0, &itemElem, 0, vecGCSendItem[si].nItemNum );
					g_UserMng.AddPutItemElem( vecSameidGuildWin[0], &itemElem );

					CString strItemMsg;
					strItemMsg.Format( prj.GetText(TID_UPGRADE_SUPPORTM), itemElem.m_nItemNum );					
					strGuildMsg.Format("  - %s %s", itemElem.GetProp()->szName, strItemMsg );
					m_vecstrGuildMsg.push_back( strGuildMsg );
				}
				else
				{
					LogItemInfo aLogItem;
					aLogItem.Action = "W";
					aLogItem.SendName = "GUILDCOMBAT_NOT";
					aLogItem.RecvName = "GUILDBANK";
					g_DPSrvr.OnLogItem( aLogItem, &itemElem, vecGCSendItem[si].nItemNum );
				}
			}
#if __VER < 11 // __GUILDCOMBATCHIP
			strGuildMsg.Format(prj.GetText(TID_GAME_GUILDCOMBAT_PRIZEMSG2) );
			m_vecstrGuildMsg.push_back( strGuildMsg );
#endif // __GUILDCOMBATCHIP
#else // __VER >= 8
			if( 0 )//g_eLocal.GetState( 500 ) == 1 )
			{
				CItemElem itemElem1;
				itemElem1.m_dwItemId	= II_SYS_SYS_SCR_BXSCRACH;	// 스크래치
				itemElem1.m_nItemNum	= 1;

				CItemElem aItemElem[3];
				int nItemSize	= 0;
				CTime t	= CTime::GetCurrentTime() + CTimeSpan( 14, 0, 0, 0 );	// 2주 사용 가능
				DWORD dwKeepTime	= t.GetTime();
				if( m_nWinGuildCount == 1 )
				{
					aItemElem[0].m_dwItemId	= II_RID_RID_BOR_RIDINGCLOUD;
					aItemElem[0].m_nItemNum	= 1;
					aItemElem[0].m_dwKeepTime	= dwKeepTime;
					nItemSize	= 1;
				}
				else if( m_nWinGuildCount == 2 )
				{
					aItemElem[0].m_dwItemId	= II_GEN_JEW_NEC_HPNECKLACE06;
					aItemElem[0].m_nItemNum	= 1;
					aItemElem[0].m_dwKeepTime	= dwKeepTime;
					nItemSize	= 1;
				}
				else if( m_nWinGuildCount == 3 )
				{
					aItemElem[0].m_dwItemId	= II_GEN_JEW_EAR_DEFEARRING06;
					aItemElem[0].m_nItemNum	= 1;
					aItemElem[0].m_dwKeepTime	= dwKeepTime;
					aItemElem[1].m_dwItemId	= II_GEN_JEW_EAR_DEFEARRING06;
					aItemElem[1].m_nItemNum	= 1;
					aItemElem[1].m_dwKeepTime	= dwKeepTime;
					nItemSize	= 2;
				}
				else	// > 4
				{
					aItemElem[0].m_dwItemId	= II_GEN_JEW_NEC_HPNECKLACE06;
					aItemElem[0].m_nItemNum	= 1;
					aItemElem[0].m_dwKeepTime	= dwKeepTime;
					aItemElem[1].m_dwItemId	= II_GEN_JEW_EAR_DEFEARRING06;
					aItemElem[1].m_nItemNum	= 1;
					aItemElem[1].m_dwKeepTime	= dwKeepTime;
					aItemElem[2].m_dwItemId	= II_GEN_JEW_EAR_DEFEARRING06;
					aItemElem[2].m_nItemNum	= 1;
					aItemElem[2].m_dwKeepTime	= dwKeepTime;
					nItemSize	= 3;
				}

				for( map<u_long, __GuildCombatMember*>::iterator i1	= m_GuildCombatMem.begin(); i1 != m_GuildCombatMem.end(); ++i1 )
				{
					u_long idGuild	= i1->first;
					__GuildCombatMember* pGCMember	= i1->second;
					for( int i = 0; i < pGCMember->vecGCSelectMember.size(); ++i )
					{
						__JOINPLAYER* pJoinPlayer	= pGCMember->vecGCSelectMember[i];
						itemElem1.SetSerialNumber();
						g_dpDBClient.SendQueryPostMail( pJoinPlayer->uidPlayer, 0, itemElem1, 0, itemElem1.GetProp()->szName, (char*)GETTEXT( TID_GAME_GUILDCOMBAT_EVENT_MAIL ) );
						if( idGuild == m_uWinGuildId )
						{
							for( int i = 0; i < nItemSize; i++ )
							{
								aItemElem[i].SetSerialNumber();
								g_dpDBClient.SendQueryPostMail( pJoinPlayer->uidPlayer, 0, aItemElem[i], 0, aItemElem[i].GetProp()->szName, "" );
							}
						}
					}
				}
			}	// 500
#endif // __VER >= 8
		}
		u_long uBestPlayerGuild;
		int nGetPoint;
		m_uBestPlayer = GetBestPlayer( &uBestPlayerGuild, &nGetPoint );

#if __VER < 11 // __GUILD_COMBAT_1TO1
		g_DPCoreClient.SendSystem( str );
		g_DPCoreClient.SendCaption( str, 0, TRUE );
#if __VER >= 11 // __SYS_PLAYER_DATA
		sprintf( str, prj.GetText(TID_GAME_GUILDCOMBAT_WINNER1), CPlayerDataCenter::GetInstance()->GetPlayerString( m_uBestPlayer ) );
#else	// __SYS_PLAYER_DATA
		sprintf( str, prj.GetText(TID_GAME_GUILDCOMBAT_WINNER1), prj.GetPlayerString( m_uBestPlayer ) );
#endif	// __SYS_PLAYER_DATA
		g_DPCoreClient.SendSystem( str );
		g_DPCoreClient.SendCaption( str, 0, TRUE );
		sprintf( str, prj.GetText(TID_GAME_GUILDCOMBAT_WINNER2) );
		g_DPCoreClient.SendSystem( str );
		g_DPCoreClient.SendCaption( str, 0, TRUE );
#endif // __GUILD_COMBAT_1TO1
		
		++m_nGuildCombatIndex;
		g_UserMng.AddGCWinGuild();
		g_UserMng.AddGCBestPlayer();

		g_UserMng.AddGCLogWorld();
	}
}

#if __VER >= 11 // __GUILDCOMBATCHIP
void CGuildCombat::GuildCombatResultRanking()
{
	vector<__REQUESTGUILD> vecGCRanking;
	vecGCRanking = vecRequestRanking;
	
	for( int i=0; i<(int)( vecGCRanking.size()-1 ); i++ )
	{
		if( i >= m_nMaxGuild )
			break;
		// 순위별로 정렬
		for( int j=0; j<(int)( vecGCRanking.size()-1-i ); j++ )
		{
			__REQUESTGUILD RequestGuild1 = vecGCRanking[j];
			__GuildCombatMember* pGCMember1 = FindGuildCombatMember( RequestGuild1.uidGuild );
			__REQUESTGUILD RequestGuild2 = vecGCRanking[j+1];
			__GuildCombatMember* pGCMember2 = FindGuildCombatMember( RequestGuild2.uidGuild );
			
			if( !pGCMember1 || !pGCMember2 )
			{
				Error( "GuildCombatResultRanking() - pGCMember1 or pGCMember2 is NULL" );
				return;
			}

			// 점수가 많은 길드가 높은 순위
			if( pGCMember1->nGuildPoint < pGCMember2->nGuildPoint )
			{
				__REQUESTGUILD temp = vecGCRanking[j];
				vecGCRanking[j] = vecGCRanking[j+1];
				vecGCRanking[j+1] = temp;
			}
			// 점수가 같을 때 생명이 많은쪽이 높은 순위
			else if( pGCMember1->nGuildPoint == pGCMember2->nGuildPoint )
			{
				int nLife1=0, nLife2=0;
				for( int k=0 ; k<(int)( pGCMember1->vecGCSelectMember.size() ); k++ )
				{
					__JOINPLAYER* pJoinPlayer = pGCMember1->vecGCSelectMember[k];
					if( pJoinPlayer )
						nLife1 += pJoinPlayer->nlife;
				}
				for( int k=0 ; k<(int)( pGCMember2->vecGCSelectMember.size() ); k++ )
				{
					__JOINPLAYER* pJoinPlayer = pGCMember2->vecGCSelectMember[k];
					if( pJoinPlayer )
						nLife2 += pJoinPlayer->nlife;
				}
	
				if( nLife1 < nLife2 )
				{
					__REQUESTGUILD temp = vecGCRanking[j];
					vecGCRanking[j] = vecGCRanking[j+1];
					vecGCRanking[j+1] = temp;
				}
				// 생명도 같을 때 생명이 남은 길드원들의 평균레벨이 높은 길드
				else if( nLife1 == nLife2 )
				{
					int nLevel=0, nCount = 0;
					float fAvg1=0, fAvg2=0;
					for( int k=0 ; k<(int)( pGCMember1->vecGCSelectMember.size() ); k++ )
					{
						__JOINPLAYER* pJoinPlayer = pGCMember1->vecGCSelectMember[k];
						if( pJoinPlayer && 0 < pJoinPlayer->nlife )
						{
							CUser* pUsertmp = (CUser*)prj.GetUserByID( pJoinPlayer->uidPlayer );
							if( IsValidObj( pUsertmp ) )
							{
								nLevel += pUsertmp->GetLevel();
								nCount++;
							}
						}
					}
					if( nCount )
						fAvg1 = (float)( nLevel/nCount );
					else
						fAvg1 = 1.0f;
					nLevel = nCount = 0;
					for( int k=0 ; k<(int)( pGCMember2->vecGCSelectMember.size() ); k++ )
					{
						__JOINPLAYER* pJoinPlayer = pGCMember2->vecGCSelectMember[k];
						if( pJoinPlayer && 0 < pJoinPlayer->nlife )
						{
							CUser* pUsertmp = (CUser*)prj.GetUserByID( pJoinPlayer->uidPlayer );
							if( IsValidObj( pUsertmp ) )
							{
								nLevel += pUsertmp->GetLevel();
								nCount++;
							}
						}
					}
					if( nCount )
						fAvg2 = (float)( nLevel/nCount );
					else
						fAvg2 = 1.0f;

					if( fAvg1 < fAvg2 )
					{
						__REQUESTGUILD temp = vecGCRanking[j];
						vecGCRanking[j] = vecGCRanking[j+1];
						vecGCRanking[j+1] = temp;
					}
				} //if 생명
			} //if 점수
		} //for j
	} //for i

	// 아이템(레드칩) 지급
	for( int i=0; i<(int)( vecGCRanking.size() ); i++ )
	{
		if( i >= 3 ) break;

		__REQUESTGUILD RequestGuild = vecGCRanking[i];
		__GuildCombatMember* pGCMember = FindGuildCombatMember( RequestGuild.uidGuild );
		if( !pGCMember )
		{
			Error( "GuildCombatResultRanking() - pGCMember is NULL" );
			continue;
		}

		CItemElem itemElem;
		itemElem.m_dwItemId = II_CHP_RED;
		float fChipNum = m_nJoinPanya * vecGCRanking.size() * 0.9f * 0.00001f * 0.1f;
		switch( i )
		{
			case 0 :	// 1위 
				fChipNum *= 0.7f;
				break;
			case 1 :	// 2위
				fChipNum *= 0.2f;
				break;
			case 2 :	// 3위
				fChipNum *= 0.1f;
				break;
		}
		itemElem.m_nItemNum = (int)fChipNum;
		if( itemElem.m_nItemNum < 1 )
			itemElem.m_nItemNum		= 1;
		itemElem.m_bCharged = itemElem.GetProp()->bCharged;

		LogItemInfo aLogItem;
		aLogItem.Action = "W";
		aLogItem.Gold_1 = i;
					
		for( int j=0 ; j<(int)( pGCMember->vecGCSelectMember.size() ); j++ )
		{
			__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[j];
			if( !pJoinPlayer )
			{
				Error( "GuildCombatResultRanking() - pJoinPlayer is NULL" );
				continue;
			}

			CUser* pUsertmp = (CUser*)prj.GetUserByID( pJoinPlayer->uidPlayer );
			if( IsValidObj( pUsertmp ) )
			{
				aLogItem.SendName = pUsertmp->GetName();
				// 아이템 지급
				itemElem.SetSerialNumber();
				if( pUsertmp->CreateItem( &itemElem ) )
				{
					aLogItem.RecvName = "GUILDCOMBAT_CHIP";
					pUsertmp->AddDefinedText( TID_GAME_GUILDCOMBAT1TO1_WARGUILDWINCHIP, "%d", itemElem.m_nItemNum );
				}
				else
				{
					g_dpDBClient.SendQueryPostMail( pUsertmp->m_idPlayer, 0, itemElem, 0, itemElem.GetProp()->szName, itemElem.GetProp()->szName );
					//pUsertmp->AddDefinedText( TID_GAME_MINIGAME_ITEM_POST, "" );
					aLogItem.RecvName = "GUILDCOMBAT_CHIP_POST";
				}
				int cbChip	= pUsertmp->GetItemNum( II_CHP_RED );
				aLogItem.Gold	= cbChip;
				aLogItem.Gold2	= cbChip + itemElem.m_nItemNum;
				aLogItem.Gold_1 = itemElem.m_nItemNum;
				g_DPSrvr.OnLogItem( aLogItem, &itemElem, itemElem.m_nItemNum );
				TRACE( "GC REDCHIP - %s, %d위, %s : %d개\n", pUsertmp->GetName(), i+1, itemElem.GetProp()->szName, itemElem.m_nItemNum );
			}
		}
	}
}
#endif // __GUILDCOMBATCHIP

void CGuildCombat::GuildCombatCloseTeleport()
{
	if( m_nStopWar != 200 )
		g_dpDBClient.SendGuildCombatResult();
	
	PRegionElem pRgnElem = g_WorldMng.GetRevivalPos( WI_WORLD_MADRIGAL, "flaris" );
	if( NULL == pRgnElem )
		return;

#ifdef __LAYER_1015
	g_UserMng.ReplaceWorld( WI_WORLD_GUILDWAR, WI_WORLD_MADRIGAL, 6968.0f, 3328.8f, nDefaultLayer );
#else	// __LAYER_1015
	g_UserMng.ReplaceWorld( WI_WORLD_GUILDWAR, WI_WORLD_MADRIGAL, 6968.0f, 3328.8f );
#endif	// __LAYER_1015

#if __VER >= 8 // __GUILDCOMBAT_85
	for( int i = 0 ; i < (int)( m_vecstrGuildMsg.size() ) ; ++i )
		g_UserMng.AddGuildMsg( m_uWinGuildId, m_vecstrGuildMsg[i] );
#endif // __VER >= 8

	m_nState = CLOSE_STATE;
	m_nGCState = WAR_CLOSE_STATE;
	m_dwTime = 0;
	m_nStopWar = 0;
	if( g_eLocal.GetState( EVE_GUILDCOMBAT ) )
		g_DPCoreClient.SendGuildCombatState( CLOSE_STATE );	

	m_bMutex = FALSE;	

	SetNpc();
}

void CGuildCombat::SetNpc( void )
{
	CMover* pMover	= (CMover*)CreateObj( D3DDEVICE, OT_MOVER, MI_MADA_ACHABEN );
	lstrcpy( pMover->m_szCharacterKey, "Mada_Guildcombatshop" );
	pMover->InitNPCProperty();
	pMover->InitCharacter( pMover->GetCharacter() );
	pMover->SetPos( D3DXVECTOR3(1343, 94, 1215) );
	pMover->InitMotion( MTI_STAND );
	pMover->UpdateLocalMatrix();
	CWorld* pWorld	= g_WorldMng.GetWorld( WI_WORLD_GUILDWAR );
	if( pWorld )
		pWorld->ADDOBJ( pMover, TRUE, nDefaultLayer );
	
	CMover* pMover1	= (CMover*)CreateObj( D3DDEVICE, OT_MOVER, MI_MADA_ACHABEN );
	lstrcpy( pMover1->m_szCharacterKey, "Mada_Guildcombatshop" );
	pMover1->InitNPCProperty();
	pMover1->InitCharacter( pMover1->GetCharacter() );
	pMover1->SetPos( D3DXVECTOR3(1288, 98, 1356) );
	pMover1->InitMotion( MTI_STAND );
	pMover1->UpdateLocalMatrix();
	if( pWorld )
		pWorld->ADDOBJ( pMover1, TRUE, nDefaultLayer );
}

void CGuildCombat::GuildCombatOpen( void )
{
	// 길드전쟁 서버에 있는 모든 캐릭터들을 모두 나가게함..
	if( m_nState != CLOSE_STATE )
		return;

#if __VER >= 8 // __GUILDCOMBAT_85
	m_vecstrGuildMsg.clear();
#endif // __VER >= 8
	if( g_eLocal.GetState( EVE_GUILDCOMBAT ) )
		g_DPCoreClient.SendGuildCombatState( OPEN_STATE );	

	m_vecGCGetPoint.clear();
	m_GCResultValueGuild.clear();
	m_GCResultValuePlayer.clear();
	g_dpDBClient.SendGuildCombatStart();
	m_nGCState = NOTENTER_STATE;
	m_nProcessGo	= 0;
	CString strOK;
	CString strCancle;
	strOK.Format( "%s", prj.GetText(TID_GAME_GUILDCOMBAT_JOIN_OK) );
	strCancle.Format( "%s", prj.GetText(TID_GAME_GUILDCOMBAT_JOIN_CANCLE) );

	for( int veci = 0 ; veci < (int)( vecRequestRanking.size() ) ; ++veci )
	{
		__REQUESTGUILD RequestGuild = vecRequestRanking[veci];

		CGuild* pGuild = g_GuildMng.GetGuild( RequestGuild.uidGuild );
		if( pGuild )
		{
			for( map<u_long, CGuildMember*>::iterator Guildi = pGuild->m_mapPMember.begin();	// neuz
			Guildi != pGuild->m_mapPMember.end(); ++Guildi )
			{
				CGuildMember* pGuildMember = Guildi->second;
				if( pGuildMember->m_nMemberLv == GUD_MASTER || pGuildMember->m_nMemberLv == GUD_KINGPIN )
				{
					CUser* pUser	= (CUser*)prj.GetUserByID( pGuildMember->m_idPlayer );
					if( IsValidObj( pUser ) )
					{
						if( veci < m_nMaxGuild )
							pUser->AddGCDiagMessage( strOK );
						else
							pUser->AddGCDiagMessage( strCancle );
					}
				}
			}
		}
	}
}
void CGuildCombat::SetRequestRanking( void )
{
	vecRequestRanking.clear();
#ifdef __S_BUG_GC
	for( int gcmi = 0 ; gcmi < (int)( m_vecGuildCombatMem.size() ) ; ++gcmi )
	{
		__GuildCombatMember* pGCMember = m_vecGuildCombatMem[gcmi];

		if( pGCMember->bRequest == FALSE )
			continue;
		
		__REQUESTGUILD RequstGuild;
		RequstGuild.dwPenya = pGCMember->dwPenya;
		RequstGuild.uidGuild = pGCMember->uGuildId;
		if( 0 == vecRequestRanking.size() )
		{
			vecRequestRanking.push_back( RequstGuild );
		}
		else
		{
			BOOL bInsert = FALSE;
			for( vector<__REQUESTGUILD>::iterator  itv = vecRequestRanking.begin() ; itv != vecRequestRanking.end() ; ++itv )
			{
				if( ((__REQUESTGUILD)*itv).dwPenya  == pGCMember->dwPenya )
				{
					BOOL bSamPenya = FALSE;
					while( ((__REQUESTGUILD)*itv).dwPenya == pGCMember->dwPenya )
					{
						++itv;
						if( itv == vecRequestRanking.end() )
						{
							bSamPenya = TRUE; 
							--itv;
							break;
						}							
					}
					
					if( bSamPenya )
					{
						vecRequestRanking.push_back( RequstGuild );
					}
					else
					{
						vecRequestRanking.insert( itv, RequstGuild );
					}
					bInsert = TRUE;
					break;
				}
				else
				if( ((__REQUESTGUILD)*itv).dwPenya  < pGCMember->dwPenya )
				{
					vecRequestRanking.insert( itv, RequstGuild );
					bInsert = TRUE;
					break;
				}				
			}
			if( bInsert == FALSE )
			{
				vecRequestRanking.push_back( RequstGuild );
			}
		}
	}
#else // __S_BUG_GC
	for( map<u_long, __GuildCombatMember*>::iterator ita = m_GuildCombatMem.begin() ; ita != m_GuildCombatMem.end() ; ++ita )
	{
		__GuildCombatMember* pGCMember = ita->second;

		if( pGCMember->bRequest == FALSE )
			continue;
		
		__REQUESTGUILD RequstGuild;
		RequstGuild.dwPenya = pGCMember->dwPenya;
		RequstGuild.uidGuild = ita->first;
		if( 0 == vecRequestRanking.size() )
		{
			vecRequestRanking.push_back( RequstGuild );
		}
		else
		{
			BOOL bInsert = FALSE;
			for( vector<__REQUESTGUILD>::iterator  itv = vecRequestRanking.begin() ; itv != vecRequestRanking.end() ; ++itv )
			{
				if( ((__REQUESTGUILD)*itv).dwPenya  == pGCMember->dwPenya )
				{
					BOOL bSamPenya = FALSE;
					while( ((__REQUESTGUILD)*itv).dwPenya == pGCMember->dwPenya )
					{
						++itv;
						if( itv == vecRequestRanking.end() )
						{
							bSamPenya = TRUE; 
							--itv;
							break;
						}							
					}
					
					if( bSamPenya )
					{
						vecRequestRanking.push_back( RequstGuild );
					}
					else
					{
						vecRequestRanking.insert( itv, RequstGuild );
					}
					bInsert = TRUE;
					break;
				}
				else
				if( ((__REQUESTGUILD)*itv).dwPenya  < pGCMember->dwPenya )
				{
					vecRequestRanking.insert( itv, RequstGuild );
					bInsert = TRUE;
					break;
				}				
			}
			if( bInsert == FALSE )
			{
				vecRequestRanking.push_back( RequstGuild );
			}
		}
	}
#endif // __S_BUG_GC
}
void CGuildCombat::SetDefender( u_long uidGuild, u_long uidDefender )
{
#ifdef __S_BUG_GC
	__GuildCombatMember* pGCMember = FindGuildCombatMember( uidGuild );
	if( pGCMember != NULL )
		pGCMember->m_uidDefender = uidDefender;
#else // __S_BUG_GC
	map<u_long, __GuildCombatMember*>::iterator it	= m_GuildCombatMem.find( uidGuild );
	if( it != m_GuildCombatMem.end() )
	{	
		__GuildCombatMember* pGCMember = it->second;
		pGCMember->m_uidDefender = uidDefender;
	}	
#endif // __S_BUG_GC
}
u_long CGuildCombat::GetDefender( u_long uidGuild )
{
	u_long uidDefender = 0;
#ifdef __S_BUG_GC
	__GuildCombatMember* pGCMember = FindGuildCombatMember( uidGuild );
	if( pGCMember != NULL )
		uidDefender = pGCMember->m_uidDefender;
#else // __S_BUG_GC
	map<u_long, __GuildCombatMember*>::iterator it	= m_GuildCombatMem.find( uidGuild );
	if( it != m_GuildCombatMem.end() )
	{	
		__GuildCombatMember* pGCMember = it->second;
		uidDefender = pGCMember->m_uidDefender;
	}
#endif // __S_BUG_GC
	return uidDefender;
}
u_long CGuildCombat::GetBestPlayer( u_long* dwGetGuildId, int* nGetPoint )
{
	u_long uBestPlayer = 0;
	map<u_long, int> mapSame;	// 동점자 발생시 처리하기 위한 playerId
	// 포인트 검사
	BOOL bResult = FALSE;
	int nMaxPoint = -1;
	for( int nVeci = 0 ; nVeci < (int)( vecRequestRanking.size() ) ; ++nVeci )
	{
		if( nVeci >= m_nMaxGuild )
			break;
		
		__REQUESTGUILD RequestGuild = vecRequestRanking[nVeci];
#ifdef __S_BUG_GC
		__GuildCombatMember* pGCMember = FindGuildCombatMember( RequestGuild.uidGuild );
		if( pGCMember != NULL )
		{
			for( int veci = 0 ; veci < (int)( pGCMember->vecGCSelectMember.size() ) ; ++veci )
			{
				__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];
				if( nMaxPoint < pJoinPlayer->nPoint )
				{
					nMaxPoint = pJoinPlayer->nPoint;
					uBestPlayer = pJoinPlayer->uidPlayer;
					*nGetPoint = pJoinPlayer->nPoint;
					*dwGetGuildId = pGCMember->uGuildId;
					bResult = TRUE;
					mapSame.clear();
					mapSame.insert( make_pair( pJoinPlayer->uidPlayer, nVeci ) );
				}
				else if( nMaxPoint == pJoinPlayer->nPoint )
				{
					bResult = FALSE;
					mapSame.insert( make_pair( pJoinPlayer->uidPlayer, nVeci ) );
				}
			}
		}
#else // __S_BUG_GC
		map<u_long, __GuildCombatMember*>::iterator ita = m_GuildCombatMem.find( RequestGuild.uidGuild );
		if( ita != m_GuildCombatMem.end() )
		{
			__GuildCombatMember* pGCMember = ita->second;
			for( int veci = 0 ; veci < pGCMember->vecGCSelectMember.size() ; ++veci )
			{
				__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];
				if( nMaxPoint < pJoinPlayer->nPoint )
				{
					nMaxPoint = pJoinPlayer->nPoint;
					uBestPlayer = pJoinPlayer->uidPlayer;
					*nGetPoint = pJoinPlayer->nPoint;
					*dwGetGuildId = ita->first;
					bResult = TRUE;
				}
				else if( nMaxPoint == pJoinPlayer->nPoint )
				{
					bResult = FALSE;
				}
			}
		}
#endif // __S_BUG_GC
	}

	// 레벨 & 경험치 검사
#ifdef __S_BUG_GC	// 동점자 처리 버그 수정 chipi 080218
	int nMinLevel = 0x7fffffff;
	EXPINTEGER nMinExp = (EXPINTEGER)0x7fffffffffffffff;
#else // __S_BUG_GC
	int nMaxLevel = -1;
	int nMaxExp = -1;
#endif // __S_BUG_GC

	if( bResult == FALSE )
	{
		for( int nVeci = 0 ; nVeci < (int)( vecRequestRanking.size() ) ; ++nVeci )
		{
			if( nVeci >= m_nMaxGuild )
				break;

			__REQUESTGUILD RequestGuild = vecRequestRanking[nVeci];
#ifdef __S_BUG_GC
			__GuildCombatMember* pGCMember = FindGuildCombatMember( RequestGuild.uidGuild );
			if( pGCMember != NULL )
			{
				for( int veci = 0 ; veci < (int)( pGCMember->vecGCSelectMember.size() ) ; ++veci )
				{
					__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];
					// 최고 점수의 동점자가 아닌 경우 continue
					if( !pJoinPlayer || mapSame.find( pJoinPlayer->uidPlayer ) == mapSame.end() )
						continue;
					
					CMover* pMover = prj.GetUserByID( pJoinPlayer->uidPlayer );
					if( IsValidObj( pMover ) )
					{
						//if( nMaxLevel < pMover->GetLevel() )
						if( nMinLevel > pMover->GetLevel() )
						{
							//nMaxLevel = pMover->GetLevel();
							//nMaxExp = pMover->GetExp1();
							nMinLevel = pMover->GetLevel();
							nMinExp = pMover->GetExp1();
							uBestPlayer = pJoinPlayer->uidPlayer;
							*nGetPoint = pJoinPlayer->nPoint;
							*dwGetGuildId = pGCMember->uGuildId;
						}
						//else if( nMaxLevel == pMover->GetLevel() )
						else if( nMinLevel == pMover->GetLevel() )
						{
							if( nMinExp > pMover->GetExp1() )
							{
								//nMaxLevel = pMover->GetLevel();
								//nMaxExp = pMover->GetExp1();
								nMinLevel = pMover->GetLevel();
								nMinExp = pMover->GetExp1();
								uBestPlayer = pJoinPlayer->uidPlayer;
								*nGetPoint = pJoinPlayer->nPoint;
								*dwGetGuildId = pGCMember->uGuildId;
							}
						}
					}
				}
			}
#else // __S_BUG_GC
			map<u_long, __GuildCombatMember*>::iterator ita = m_GuildCombatMem.find( RequestGuild.uidGuild );
			if( ita != m_GuildCombatMem.end() )
			{
				__GuildCombatMember* pGCMember = ita->second;
				for( int veci = 0 ; veci < pGCMember->vecGCSelectMember.size() ; ++veci )
				{
					__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];
					CMover* pMover = prj.GetUserByID( pJoinPlayer->uidPlayer );
					if( IsValidObj( pMover ) )
					{
						if( nMaxLevel < pMover->GetLevel() )
						{
							nMaxLevel = pMover->GetLevel();
							nMaxExp = pMover->GetExp1();
							uBestPlayer = pJoinPlayer->uidPlayer;
							*nGetPoint = pJoinPlayer->nPoint;
							*dwGetGuildId = ita->first;
						}
						else if( nMaxLevel == pMover->GetLevel() )
						{
							if( nMaxExp < pMover->GetExp1() )
							{
								nMaxLevel = pMover->GetLevel();
								nMaxExp = pMover->GetExp1();
								uBestPlayer = pJoinPlayer->uidPlayer;
								*nGetPoint = pJoinPlayer->nPoint;
								*dwGetGuildId = ita->first;
							}
						}
					}
				}
			}
#endif // __S_BUG_GC
		}
	}
	return uBestPlayer;
}

// 지금까지의 총 상금
__int64 CGuildCombat::GetPrizePenya( int nFlag )
{
	// 상금에 포함할 길드들..
	__int64 nPrizePenya = 0;
	for( int veci = 0 ; veci < (int)( vecRequestRanking.size() ) ; ++veci )
	{
		if( veci >= m_nMaxGuild )
			break;
		
		nPrizePenya += vecRequestRanking[veci].dwPenya;
	}
	__int64 nResult;

	switch( nFlag )
	{
	case 0:
		nResult = MulDiv( (int)( nPrizePenya ), m_nMaxGuildPercent, 100 );
		break;
	case 1:
		nResult = MulDiv( (int)( nPrizePenya ), m_nMaxPlayerPercent, 100 );
		break;
	case 2:
		nResult = nPrizePenya;
	}

	return nResult;
}
// 길드의 신청한 Penya
DWORD CGuildCombat::GetRequstPenya( u_long uidGuild )
{
	DWORD dwRequestPenya = 0;
#ifdef __S_BUG_GC
	__GuildCombatMember* pGCMember = FindGuildCombatMember( uidGuild );
	if( pGCMember != NULL )
		dwRequestPenya = pGCMember->dwPenya;
#else // __S_BUG_GC
	map<u_long, __GuildCombatMember*>::iterator it	= m_GuildCombatMem.find( uidGuild );
	if( it != m_GuildCombatMem.end() )
	{	
		__GuildCombatMember* pGCMember = it->second;
		dwRequestPenya = pGCMember->dwPenya;
	}
#endif // __S_BUG_GC
	return dwRequestPenya;
}
// 지금부터 다음 대전이 시작될 남은시간
CTime CGuildCombat::GetNextGuildCobmatTime()
{
	CTime tCurrentTime = CTime::GetCurrentTime();
	CTime tNextCombat;
	int nDayofWeek = m_nDay - tCurrentTime.GetDayOfWeek();
	if( 0 < nDayofWeek )
		tNextCombat = tCurrentTime + CTimeSpan( nDayofWeek, 0, 0, 0 ); 
	else if( 0 == nDayofWeek )
	{
		CTimeSpan tCTime = CTimeSpan( 0, tCurrentTime.GetHour(), tCurrentTime.GetMinute(), tCurrentTime.GetSecond() );
		CTimeSpan tNTime = CTimeSpan( 0, __AutoOpen[m_nDay-1].nHour, __AutoOpen[m_nDay-1].nMinute, 0 );
		if( tCTime <= tNTime )
			tNextCombat = tCurrentTime;
		else
			tNextCombat = tCurrentTime + CTimeSpan( m_nDay, 0, 0, 0 );
	}
	else if( 0 > nDayofWeek )
	{
		nDayofWeek = 7 + nDayofWeek;
		tNextCombat = tCurrentTime + CTimeSpan( nDayofWeek, 0, 0, 0 ); 
	}
	tNextCombat = CTime( tNextCombat.GetYear(), tNextCombat.GetMonth(), tNextCombat.GetDay(), __AutoOpen[m_nDay-1].nHour, __AutoOpen[m_nDay-1].nMinute, 0 );
	return tNextCombat;
}

#ifdef __S_BUG_GC
CGuildCombat::__GuildCombatMember* CGuildCombat::FindGuildCombatMember( u_long GuildId )
{
	__GuildCombatMember* pGCMem = NULL;
	for( int i = 0 ; i < (int)( m_vecGuildCombatMem.size() ) ; ++i )
	{
		if( GuildId == m_vecGuildCombatMem[i]->uGuildId )
		{
			pGCMem = m_vecGuildCombatMem[i];
			break;
		}
	}
	return pGCMem;
}
#endif // __S_BUG_GC
void CGuildCombat::SetSelectMap( CUser* pUser, int nMap )
{
#ifdef __S_BUG_GC
	__GuildCombatMember* pGCMember = FindGuildCombatMember( pUser->m_idGuild );
	if( pGCMember != NULL )
	{	
		for( int veci = 0 ; veci < (int)( pGCMember->vecGCSelectMember.size() ) ; ++veci )
		{
			__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];
			if( pJoinPlayer->uidPlayer == pUser->m_idPlayer )
			{
				pJoinPlayer->nMap = nMap;
				break;
			}
		}
	}
#else // __S_BUG_GC
	map<u_long, __GuildCombatMember*>::iterator it	= m_GuildCombatMem.find( pUser->m_idGuild );
	if( it != m_GuildCombatMem.end() )
	{	
		__GuildCombatMember* pGCMember = it->second;
		for( int veci = 0 ; veci < pGCMember->vecGCSelectMember.size() ; ++veci )
		{
			__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];
			if( pJoinPlayer->uidPlayer == pUser->m_idPlayer )
			{
				pJoinPlayer->nMap = nMap;
				break;
			}
		}
	}
#endif // __S_BUG_GC
}
void CGuildCombat::GuildCombatEnter( CUser* pUser )
{
	BOOL bJoin = TRUE;
	if( m_nState == OPEN_STATE || m_nState == CLOSE_STATE || m_nState == WAR_STATE )
	{
		BOOL bJoin = TRUE;
		if( m_nState == OPEN_STATE && m_nGCState != ENTER_STATE )
		{
			bJoin = FALSE;
			pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_NOT_JOIN) );
		}

		if( bJoin )
		{
			JoinObserver( pUser );
		}
	}
	else if( m_nState == COMPLET_CLOSE_STATE )
	{
		// 아무도 못들어옴.. 지금은 닫혀있는 상태
		pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_NOT_OPEN) );
		bJoin = FALSE;
	}
	else
	{
		bJoin = FALSE;
	}

	if( bJoin )
	{
		if( 0 < m_dwTime )
		{		
			pUser->AddGuildCombatNextTime( m_dwTime - GetTickCount(), GuildCombatProcess[m_nProcessGo - 1].dwParam );
		}
	}
}
// 전쟁 준비 시간
void CGuildCombat::SetMaintenance()
{
	CString strMsgMaster = prj.GetText( TID_GAME_GUILDCOMBAT_JOIN_MSG_MASTER );
	CString strMsgDefender = prj.GetText( TID_GAME_GUILDCOMBAT_JOIN_MSG_DEFENDER );
	CString strMsgGeneral = prj.GetText( TID_GAME_GUILDCOMBAT_JOIN_MSG_GENERAL );
	
	BOOL bWinGuild_Continue = FALSE;		// 요번게임이 취소 되었다면 연승을 연장 여부
	int nCount = 0;
	for( int nVeci = 0 ; nVeci < (int)( vecRequestRanking.size() ) ; ++nVeci )
	{
		// 최대로 들어갈수 있는 길드만 들만 참여가능
		if( nVeci >= m_nMaxGuild )
			break;

		__REQUESTGUILD RequestGuild = vecRequestRanking[nVeci];

		if( RequestGuild.uidGuild == m_uWinGuildId )
			bWinGuild_Continue = TRUE;

#ifdef __S_BUG_GC
		__GuildCombatMember* pGCMember = FindGuildCombatMember( RequestGuild.uidGuild );
		if( pGCMember != NULL )
		{
			CGuild* pGuild = g_GuildMng.GetGuild( pGCMember->uGuildId );
			if( pGuild )
			{
				pGCMember->nJoinCount = 0;
				pGCMember->nWarCount = 0;
				BOOL bMaxWarCount = FALSE;

				for( int i = 0; i < (int)( pGCMember->vecGCSelectMember.size() ) ; ++i )
				{
					__JOINPLAYER* pJoinPlayer	= pGCMember->vecGCSelectMember[i];
					if( 0 < pJoinPlayer->nlife )
						pGCMember->lspFifo.push_back( pJoinPlayer );
				}

				while( pGCMember->lspFifo.size() > 0 )
				{
					__JOINPLAYER* pJoinPlayer	= pGCMember->lspFifo.front();
					CMover* pMover	= prj.GetUserByID( pJoinPlayer->uidPlayer );
					if( IsValidObj( pMover ) && pMover->GetWorld()->GetID() == WI_WORLD_GUILDWAR )
					{
						pGCMember->nWarCount++;
//						pJoinPlayer->bEntry	= TRUE;
						pJoinPlayer->dwTelTime	= timeGetTime();
						((CUser*)pMover)->AddGCJoinWarWindow( pJoinPlayer->nMap, m_nMaxMapTime );
						pGCMember->lspFifo.pop_front();
					}
					else
					{
						pGCMember->lspFifo.pop_front();
						pJoinPlayer->nlife--;
						if( pJoinPlayer->nlife > 0 )
							pGCMember->lspFifo.push_back( pJoinPlayer );
						if( pJoinPlayer->nlife < 0 )
							pJoinPlayer->nlife	= 0;
					}
					if( pGCMember->nWarCount == m_nMaxWarPlayer )
						break;
				}

				g_UserMng.AddGCGuildStatus( pGCMember->uGuildId );

#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
				for( int veci = 0; veci < (int)( pGCMember->vecGCSelectMember.size() ) ; ++veci )
				{
					__JOINPLAYER* pJoinPlayer	= pGCMember->vecGCSelectMember[veci];
					CMover* pMover	= prj.GetUserByID( pJoinPlayer->uidPlayer );
					if( IsValidObj( pMover ) && pMover->GetWorld()->GetID() == WI_WORLD_GUILDWAR )
					{
						// Message
						if( pGuild->IsMaster( pMover->m_idPlayer ) )	// Master
							((CUser*)pMover)->AddDiagText( strMsgMaster );
						else if( pGCMember->m_uidDefender == pMover->m_idPlayer )	// Defender
							((CUser*)pMover)->AddDiagText( strMsgDefender );
						else
							((CUser*)pMover)->AddDiagText( strMsgGeneral );
					}
				}
#else // __IMPROVE_SYSTEM_VER15
				// 메세지 처리
				CMover* pLeader = NULL;
				for( int veci = 0; veci < pGCMember->vecGCSelectMember.size() ; ++veci )
				{
					__JOINPLAYER* pJoinPlayer	= pGCMember->vecGCSelectMember[veci];
					CMover* pMover	= prj.GetUserByID( pJoinPlayer->uidPlayer );
					if( IsValidObj( pMover ) && pMover->GetWorld()->GetID() == WI_WORLD_GUILDWAR )
					{
						// Message
						if( pGuild->IsMaster( pMover->m_idPlayer ) )	// Master
							((CUser*)pMover)->AddDiagText( strMsgMaster );
						else if( pGCMember->m_uidDefender == pMover->m_idPlayer )	// Defender
							((CUser*)pMover)->AddDiagText( strMsgDefender );
						else
							((CUser*)pMover)->AddDiagText( strMsgGeneral );
						if( pLeader == NULL )
							pLeader = pMover;
						else					
							g_DPCoreClient.SendGCAddParty( pLeader->m_idPlayer, pLeader->GetLevel(), pLeader->GetJob(), pLeader->GetSex(), 
							pMover->m_idPlayer, pMover->GetLevel(), pMover->GetJob(), pMover->GetSex() );
					}
				}
#endif // __IMPROVE_SYSTEM_VER15

				if( 0 < pGCMember->vecGCSelectMember.size() )
					++nCount;
			}
		}
#else // __S_BUG_GC
		map<u_long, __GuildCombatMember*>::iterator ita = m_GuildCombatMem.find( RequestGuild.uidGuild );
		if( ita != m_GuildCombatMem.end() )
		{
			__GuildCombatMember* pGCMember = ita->second;
			CGuild* pGuild = g_GuildMng.GetGuild( RequestGuild.uidGuild );
			if( pGuild )
			{
				pGCMember->nJoinCount = 0;
				pGCMember->nWarCount = 0;
				BOOL bMaxWarCount = FALSE;

				for( int i = 0; i < pGCMember->vecGCSelectMember.size() ; ++i )
				{
					__JOINPLAYER* pJoinPlayer	= pGCMember->vecGCSelectMember[i];
					if( 0 < pJoinPlayer->nlife )
						pGCMember->lspFifo.push_back( pJoinPlayer );
				}

				while( pGCMember->lspFifo.size() > 0 )
				{
					__JOINPLAYER* pJoinPlayer	= pGCMember->lspFifo.front();
					CMover* pMover	= prj.GetUserByID( pJoinPlayer->uidPlayer );
					if( IsValidObj( pMover ) && pMover->GetWorld()->GetID() == WI_WORLD_GUILDWAR )
					{
						pGCMember->nWarCount++;
//						pJoinPlayer->bEntry	= TRUE;
						pJoinPlayer->dwTelTime	= timeGetTime();
						((CUser*)pMover)->AddGCJoinWarWindow( pJoinPlayer->nMap, m_nMaxMapTime );
						pGCMember->lspFifo.pop_front();
					}
					else
					{
						pGCMember->lspFifo.pop_front();
						pJoinPlayer->nlife--;
						if( pJoinPlayer->nlife > 0 )
							pGCMember->lspFifo.push_back( pJoinPlayer );
						if( pJoinPlayer->nlife < 0 )
							pJoinPlayer->nlife	= 0;
					}
					if( pGCMember->nWarCount == m_nMaxWarPlayer )
						break;
				}

				g_UserMng.AddGCGuildStatus( ita->first );

				// 메세지 처리
				CMover* pLeader = NULL;
				for( int veci = 0; veci < pGCMember->vecGCSelectMember.size() ; ++veci )
				{
					__JOINPLAYER* pJoinPlayer	= pGCMember->vecGCSelectMember[veci];
					CMover* pMover	= prj.GetUserByID( pJoinPlayer->uidPlayer );
					if( IsValidObj( pMover ) && pMover->GetWorld()->GetID() == WI_WORLD_GUILDWAR )
					{
						// Message
						if( pGuild->IsMaster( pMover->m_idPlayer ) )	// Master
							((CUser*)pMover)->AddDiagText( strMsgMaster );
						else if( pGCMember->m_uidDefender == pMover->m_idPlayer )	// Defender
							((CUser*)pMover)->AddDiagText( strMsgDefender );
						else
							((CUser*)pMover)->AddDiagText( strMsgGeneral );
						if( pLeader == NULL )
							pLeader = pMover;
						else					
							g_DPCoreClient.SendGCAddParty( pLeader->m_idPlayer, pLeader->GetLevel(), pLeader->GetJob(), pLeader->GetSex(), 
							pMover->m_idPlayer, pMover->GetLevel(), pMover->GetJob(), pMover->GetSex() );
					}
				}

				if( 0 < pGCMember->vecGCSelectMember.size() )
					++nCount;
			}
		}
#endif // __S_BUG_GC
	}

	// 참가한 길드가 2개 이상일때만 길드대전이 시작하게함.
#if __VER >= 8 // __GUILDCOMBAT_85
	if( nCount < m_nMinGuild )
#else // __VER >= 8
	if( nCount < 2 )
#endif // __VER >= 8
	{
		// 시작이 안되었을때
		// 연승한 길드가 참가하고 있으면 승수 이어감
		// 참가하지 않았으면 승수 없어짐( 승리한 길드가 없음 )
		if( bWinGuild_Continue == FALSE )
		{
			g_dpDBClient.SendGuildcombatContinue( m_nGuildCombatIndex-1, m_uWinGuildId, 0 );
			m_uWinGuildId = 0;
			m_nWinGuildCount = 0;

			g_UserMng.AddGCWinGuild();
		}
		SetGuildCombatCloseWait( TRUE );
	}
}

void CGuildCombat::SetEnter()
{
	char str[512] = {0,};
	sprintf( str, prj.GetText(TID_GAME_GUILDCOMBAT_ENTER) );
	g_DPCoreClient.SendSystem( str );
	g_DPCoreClient.SendCaption( str, 0, TRUE );
#if __VER < 11 // __GUILD_COMBAT_1TO1
	sprintf( str, prj.GetText(TID_GAME_GUILDCOMBAT_ENTER1) );
	g_DPCoreClient.SendSystem( str );
	g_DPCoreClient.SendCaption( str, 0, TRUE );
#endif // __GUILD_COMBAT_1TO1

	for( int nVeci = 0 ; nVeci < (int)( vecRequestRanking.size() ) ; ++nVeci )
	{
		// 최대로 들어갈수 있는 길드만 들만 참여가능
		if( nVeci >= m_nMaxGuild )
			break;
		
		__REQUESTGUILD RequestGuild = vecRequestRanking[nVeci];
#ifdef __S_BUG_GC
		__GuildCombatMember* pGCMember = FindGuildCombatMember( RequestGuild.uidGuild );
		if( pGCMember != NULL )
		{
			for( int veci = 0 ; veci < (int)( pGCMember->vecGCSelectMember.size() ) ; ++veci )
			{
				__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];
				CMover* pMover = prj.GetUserByID( pJoinPlayer->uidPlayer );
				if( IsValidObj( pMover ) )
				{
					sprintf( str, "%s", prj.GetText(TID_GAME_GUILDCOMBAT_TELE) );
					((CUser*)pMover)->AddGCTele( str );
				}
			}
		}
#else // __S_BUG_GC
		map<u_long, __GuildCombatMember*>::iterator ita = m_GuildCombatMem.find( RequestGuild.uidGuild );
		if( ita != m_GuildCombatMem.end() )
		{
			__GuildCombatMember* pGCMember = ita->second;
			for( int veci = 0 ; veci < pGCMember->vecGCSelectMember.size() ; ++veci )
			{
				__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];
				CMover* pMover = prj.GetUserByID( pJoinPlayer->uidPlayer );
				if( IsValidObj( pMover ) )
				{
					sprintf( str, "%s", prj.GetText(TID_GAME_GUILDCOMBAT_TELE) );
					((CUser*)pMover)->AddGCTele( str );
				}
			}
		}
#endif // __S_BUG_GC
	}
}
// 대전 시작
void CGuildCombat::SetGuildCombatStart()
{
	__GuildCombatMember* pGuildCombatMem;
	__JOINPLAYER* pJoinPlayer;
	__REQUESTGUILD RequestGuild;

	for( int nVeci = 0 ; nVeci < (int)( vecRequestRanking.size() ) ; ++nVeci )
	{
		if( nVeci >= m_nMaxGuild )
			break;
		
		RequestGuild = vecRequestRanking[nVeci];
#ifdef __S_BUG_GC
		pGuildCombatMem = FindGuildCombatMember( RequestGuild.uidGuild );
		if( pGuildCombatMem != NULL )
		{
			for( int veci = 0 ; veci < (int)( pGuildCombatMem->vecGCSelectMember.size() ) ; ++veci )
			{
				pJoinPlayer = pGuildCombatMem->vecGCSelectMember[veci];
				BOOL bFind	= FALSE;
				for( list<__JOINPLAYER*>::iterator i1 = pGuildCombatMem->lspFifo.begin(); i1 != pGuildCombatMem->lspFifo.end(); ++i1 )
				{
					if( pJoinPlayer == *i1 )
					{
						bFind	= TRUE;
						break;
					}
				}				
				if( pJoinPlayer->nlife > 0 && !bFind )
				{
					CMover* pMover = prj.GetUserByID( pJoinPlayer->uidPlayer );
					if( IsValidObj( pMover ) )
					{
						pMover->m_nGuildCombatState		= 1;
						g_UserMng.AddGuildCombatUserState( pMover );
					}
				}
			}
		}
#else // __S_BUG_GC
		map<u_long, __GuildCombatMember*>::iterator ita = m_GuildCombatMem.find( RequestGuild.uidGuild );
		if( ita != m_GuildCombatMem.end() )
		{
			pGuildCombatMem = ita->second;

			for( int veci = 0 ; veci < pGuildCombatMem->vecGCSelectMember.size() ; ++veci )
			{
				pJoinPlayer = pGuildCombatMem->vecGCSelectMember[veci];
//				if( pJoinPlayer->bEntry == TRUE && 0 < pJoinPlayer->nlife )
				BOOL bFind	= FALSE;
				for( list<__JOINPLAYER*>::iterator i1 = pGuildCombatMem->lspFifo.begin(); i1 != pGuildCombatMem->lspFifo.end(); ++i1 )
				{
					if( pJoinPlayer == *i1 )
					{
						bFind	= TRUE;
						break;
					}
				}				
				if( pJoinPlayer->nlife > 0 && !bFind )
				{
					CMover* pMover = prj.GetUserByID( pJoinPlayer->uidPlayer );
					if( IsValidObj( pMover ) )
					{
						pMover->m_nGuildCombatState		= 1;
						g_UserMng.AddGuildCombatUserState( pMover );
					}
				}
			}
		}
#endif // __S_BUG_GC
	}
}
// 전쟁 종료
void CGuildCombat::SetGuildCombatClose( BOOL bGM )
{
	if( m_nState == CLOSE_STATE )
		return;

	if( bGM )
	{
		m_nStopWar = 1;	
		return;
	}

	for( int nVeci = 0 ; nVeci < (int)( vecRequestRanking.size() ) ; ++nVeci )
	{
		if( nVeci >= m_nMaxGuild )
			break;
		
		__REQUESTGUILD RequestGuild = vecRequestRanking[nVeci];
#ifdef __S_BUG_GC
		__GuildCombatMember* pGCMember = FindGuildCombatMember( RequestGuild.uidGuild );
		if( pGCMember != NULL )
		{
			for( int veci = 0 ; veci < (int)( pGCMember->vecGCSelectMember.size() ) ; ++veci )
			{
				__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];
				CMover* pMover = prj.GetUserByID( pJoinPlayer->uidPlayer );
				if( IsValidObj( pMover ) )
				{
					pMover->m_nGuildCombatState = 0;
					g_UserMng.AddGuildCombatUserState( pMover );
				}
			}
		}
#else // __S_BUG_GC
		map<u_long, __GuildCombatMember*>::iterator ita = m_GuildCombatMem.find( RequestGuild.uidGuild );
		if( ita != m_GuildCombatMem.end() )
		{
			__GuildCombatMember* pGuildCombat = ita->second;
			for( int veci = 0 ; veci < pGuildCombat->vecGCSelectMember.size() ; ++veci )
			{
				__JOINPLAYER* pJoinPlayer = pGuildCombat->vecGCSelectMember[veci];
				CMover* pMover = prj.GetUserByID( pJoinPlayer->uidPlayer );
				if( IsValidObj( pMover ) )
				{
					pMover->m_nGuildCombatState = 0;
					g_UserMng.AddGuildCombatUserState( pMover );
				}
			}
		}
#endif // __S_BUG_GC
	}

	// 길드 부활 포인트 얻은 획득 주기
	for( int nVeci = 0 ; nVeci < (int)( vecRequestRanking.size() ) ; ++nVeci )
	{
		if( nVeci >= m_nMaxGuild )
			break;
		
		__REQUESTGUILD RequestGuild = vecRequestRanking[nVeci];
#ifdef __S_BUG_GC
		__GuildCombatMember* pGCMember = FindGuildCombatMember( RequestGuild.uidGuild );
		if( pGCMember != NULL )
		{
			int nRevivalPoint = 0;
			for( int veci = 0 ; veci < (int)( pGCMember->vecGCSelectMember.size() ) ; ++veci )
			{
				__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];
				nRevivalPoint += pJoinPlayer->nlife;
			}
			pGCMember->nGuildPoint += nRevivalPoint;

			CGuild* pGuildMsg;
			CGuildMember*	pMember;
			CUser*			pUsertmp;
			pGuildMsg = g_GuildMng.GetGuild( RequestGuild.uidGuild );
			if( pGuildMsg )
			{
				for( map<u_long, CGuildMember*>::iterator iGuild = pGuildMsg->m_mapPMember.begin();
				iGuild != pGuildMsg->m_mapPMember.end(); ++iGuild )
				{
					pMember		= iGuild->second;
					pUsertmp	= (CUser*)prj.GetUserByID( pMember->m_idPlayer );
					if( IsValidObj( pUsertmp ) && pUsertmp->GetWorld() && pUsertmp->GetWorld()->GetID() == WI_WORLD_GUILDWAR )
						pUsertmp->AddDefinedCaption( TRUE, TID_GAME_GUILDCOMBAT_POINT_REVIVAL, "%d", nRevivalPoint );
				}
			}
		}
#else // __S_BUG_GC
		map<u_long, __GuildCombatMember*>::iterator ita = m_GuildCombatMem.find( RequestGuild.uidGuild );
		if( ita != m_GuildCombatMem.end() )
		{
			int nRevivalPoint = 0;
			__GuildCombatMember* pGuildCombat = ita->second;
			for( int veci = 0 ; veci < pGuildCombat->vecGCSelectMember.size() ; ++veci )
			{
				__JOINPLAYER* pJoinPlayer = pGuildCombat->vecGCSelectMember[veci];
				nRevivalPoint += pJoinPlayer->nlife;
			}
			pGuildCombat->nGuildPoint += nRevivalPoint;

			CGuild* pGuildMsg;
			CGuildMember*	pMember;
			CUser*			pUsertmp;
			pGuildMsg = g_GuildMng.GetGuild( RequestGuild.uidGuild );
			if( pGuildMsg )
			{
				for( map<u_long, CGuildMember*>::iterator iGuild = pGuildMsg->m_mapPMember.begin();
				iGuild != pGuildMsg->m_mapPMember.end(); ++iGuild )
				{
					pMember		= iGuild->second;
					pUsertmp	= (CUser*)prj.GetUserByID( pMember->m_idPlayer );
					if( IsValidObj( pUsertmp ) && pUsertmp->GetWorld() && pUsertmp->GetWorld()->GetID() == WI_WORLD_GUILDWAR )
						pUsertmp->AddDefinedCaption( TRUE, TID_GAME_GUILDCOMBAT_POINT_REVIVAL, "%d", nRevivalPoint );
				}
			}
		}
#endif // __S_BUG_GC
	}
	g_UserMng.AddGCGuildPrecedence();
}
// 전쟁 종료후 결과값
void CGuildCombat::SetGuildCombatCloseWait( BOOL bGM )
{
	if( m_nState == CLOSE_STATE )
		return;

	if( bGM )
	{
		m_nStopWar = 2;
		return;
	}
	else
	{
		GuildCombatResult();
#if __VER >= 11 // __GUILDCOMBATCHIP
		GuildCombatResultRanking();
#endif // __GUILDCOMBATCHIP
	}	
}
// 신청한 길드중에 출전할수 있는 길드인지?
BOOL CGuildCombat::IsRequestWarGuild( u_long uidGuild, BOOL bAll )
{
	for( int veci = 0 ; veci < (int)( vecRequestRanking.size() ) ; ++veci )
	{
		if( bAll == FALSE )
		{
			if( m_nMaxGuild <= veci )
				break;
		}

		__REQUESTGUILD RequestGuild = vecRequestRanking[veci];
		if( uidGuild == RequestGuild.uidGuild )
			return TRUE;
	}
	return FALSE;
}

// 참가한 선수인지?
BOOL CGuildCombat::IsSelectPlayer( CUser* pUser )
{
	__GuildCombatMember* pGuildCombatMem;
	__JOINPLAYER* pJoinPlayer;
	__REQUESTGUILD RequestGuild;

	for( int nVeci = 0 ; nVeci < (int)( vecRequestRanking.size() ) ; ++nVeci )
	{
		if( nVeci >= m_nMaxGuild )
			break;

		RequestGuild = vecRequestRanking[nVeci];
		if( RequestGuild.uidGuild == pUser->m_idGuild )
		{
#ifdef __S_BUG_GC
			pGuildCombatMem = FindGuildCombatMember( RequestGuild.uidGuild );
			if( pGuildCombatMem != NULL )
			{
				for( int veci = 0 ; veci < (int)( pGuildCombatMem->vecGCSelectMember.size() ) ; ++veci )
				{
					pJoinPlayer = pGuildCombatMem->vecGCSelectMember[veci];
					if( pJoinPlayer->uidPlayer == pUser->m_idPlayer )
					{
						if( pJoinPlayer->nlife > 0 )
							return TRUE;
						break;
					}
				}
			}
#else // __S_BUG_GC
			map<u_long, __GuildCombatMember*>::iterator ita = m_GuildCombatMem.find( RequestGuild.uidGuild );
			if( ita != m_GuildCombatMem.end() )
			{
				pGuildCombatMem = ita->second;
				
				for( int veci = 0 ; veci < pGuildCombatMem->vecGCSelectMember.size() ; ++veci )
				{
					pJoinPlayer = pGuildCombatMem->vecGCSelectMember[veci];
					if( pJoinPlayer->uidPlayer == pUser->m_idPlayer )
					{
						if( pJoinPlayer->nlife > 0 )
							return TRUE;
						break;
					}
				}
			}
#endif // __S_BUG_GC
			break;
		}
	}
	return FALSE;
}

void CGuildCombat::Process()
{
#ifdef __WORLDSERVER
#ifdef _DEBUG	
//	TRACE( "GuildWarFlyff::Process <%d>, <%d>\n", m_nState, m_nGuildCombatState);
#endif //_DEBUG
	if( m_nState != CLOSE_STATE )
	{
		if( m_nProcessGo < m_nProcessCount[0] )
			ProcessCommand();
		ProcessJoinWar();	//
	}
	else
	{
		CTime ctime = CTime::GetCurrentTime();

		if( __AutoOpen[ctime.GetDayOfWeek()-1].bUseing )
		{
			// 스킵 될 수 있으므로 수정을 필요로 함
			if( __AutoOpen[ctime.GetDayOfWeek()-1].nHour == ctime.GetHour() &&
				__AutoOpen[ctime.GetDayOfWeek()-1].nMinute == ctime.GetMinute() && m_bMutex == FALSE )
			{
#ifdef __S_BUG_GC
				if( (int)( m_vecGuildCombatMem.size() ) >= m_nMinGuild )
#else // __S_BUG_GC
				if( m_GuildCombatMem.size() > 1 )
#endif // __S_BUG_GC
				{				
					GuildCombatOpen();
				}
				else
				{
					m_ctrMutexOut.Set( SEC(60) );
					m_bMutexMsg = TRUE;
					g_DPCoreClient.SendSystem( prj.GetText( TID_GAME_GUILDCOMBAT_NEXT_COMBAT ) );	
					g_DPCoreClient.SendSystem( prj.GetText( TID_GAME_GUILDCOMBAT_ENJOY ) );	
				}

				m_bMutex = TRUE;				
			}

			if( m_bMutexMsg && m_ctrMutexOut.IsTimeOut() )
			{
				m_bMutexMsg = FALSE;
				m_bMutex    = FALSE;
			}
		}
	}
#endif //__WORLDSERVER
}

void CGuildCombat::ProcessJoinWar()
{
	for( int nVeci = 0 ; nVeci < (int)( vecRequestRanking.size() ) ; ++nVeci )
	{
		if( nVeci >= m_nMaxGuild )
			break;
		
		__REQUESTGUILD RequestGuild = vecRequestRanking[nVeci];
#ifdef __S_BUG_GC
		__GuildCombatMember* pGCMember = FindGuildCombatMember( RequestGuild.uidGuild );
		if( pGCMember != NULL )
		{
			for( int veci = 0 ; veci < (int)( pGCMember->vecGCSelectMember.size() ) ; ++veci )
			{
				__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];
				if( pJoinPlayer->dwTelTime != 0 )
				{
					if( pJoinPlayer->dwTelTime + m_nMaxMapTime * 1000 <= timeGetTime()  )
					{
						pJoinPlayer->dwTelTime = 0;
						CMover* pMover = prj.GetUserByID( pJoinPlayer->uidPlayer );					
						if( IsValidObj( pMover ) )
						{
							JoinWar( (CUser*)pMover, pJoinPlayer->nMap );
						}
					}
				}
			}
		}
#else // __S_BUG_GC
		map<u_long, __GuildCombatMember*>::iterator ita = m_GuildCombatMem.find( RequestGuild.uidGuild );
		if( ita != m_GuildCombatMem.end() )
		{
			__GuildCombatMember* pGCMember = ita->second;
			for( int veci = 0 ; veci < pGCMember->vecGCSelectMember.size() ; ++veci )
			{
				__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];
				if( pJoinPlayer->dwTelTime != 0 )
				{
					if( pJoinPlayer->dwTelTime + m_nMaxMapTime * 1000 <= timeGetTime()  )
					{
						pJoinPlayer->dwTelTime = 0;
						CMover* pMover = prj.GetUserByID( pJoinPlayer->uidPlayer );					
						if( IsValidObj( pMover ) )
						{
							JoinWar( (CUser*)pMover, pJoinPlayer->nMap );
						}
					}
				}
			}
		}
#endif // __S_BUG_GC
	}
}
void CGuildCombat::ProcessCommand()
{
	if( m_nStopWar == 1 )
	{
		m_nProcessGo = m_nProcessCount[COMPLET_CLOSE_STATE];
		m_dwTime = 0;
		m_nStopWar = 0;
	}
	else if( m_nStopWar == 2 )
	{
		m_nProcessGo = m_nProcessCount[GM_COLSE_STATE];
		m_dwTime = 0;
		m_nStopWar = 200;
	}
	if( m_dwTime <= GetTickCount() )
	{
		if( GuildCombatProcess[m_nProcessGo].nState == WARSTATE )
		{
			if( GuildCombatProcess[m_nProcessGo].dwCommand == NOTENTER_COUNT_STATE )
#ifdef __LAYER_1015
				g_UserMng.ReplaceWorld( WI_WORLD_GUILDWAR, WI_WORLD_MADRIGAL, 6968.0f, 3328.8f, nDefaultLayer );
#else	// __LAYER_1015
				g_UserMng.ReplaceWorld( WI_WORLD_GUILDWAR, WI_WORLD_MADRIGAL, 6968.0f, 3328.8f );
#endif	// __LAYER_1015
			else if( GuildCombatProcess[m_nProcessGo].dwCommand == MAINTENANCE_STATE )
			{
				// 전쟁 준비 시작
				SetMaintenance();
			}
			else if( GuildCombatProcess[m_nProcessGo].dwCommand == ENTER_STATE )
			{
				// 들어올수 있는 상태
				SetEnter();
			}
			else if( GuildCombatProcess[m_nProcessGo].dwCommand == WAR_WAR_STATE )
			{
				// 전쟁 시작 
				SetGuildCombatStart();
			}
			else if( GuildCombatProcess[m_nProcessGo].dwCommand == WAR_CLOSE_STATE )
			{
				// 전쟁 종료
				SetGuildCombatClose();
			}
			else if( GuildCombatProcess[m_nProcessGo].dwCommand == WAR_CLOSE_WAIT_STATE )
			{
				// 전쟁 종료후 결과값 나오기 대기
				SetGuildCombatCloseWait();
			}
			else if( GuildCombatProcess[m_nProcessGo].dwCommand == WAR_TELEPORT_STATE )
			{
				// 전쟁 종료후 텔레포트
				GuildCombatCloseTeleport();
			}
		}

		switch( GuildCombatProcess[m_nProcessGo].nState )
		{
		case ALLMSG:
			{
#if __VER >= 11 // __GUILD_COMBAT_1TO1
				if( GuildCombatProcess[m_nProcessGo].dwCommand != TID_GAME_GUILDCOMBAT_OPEN_MSG )
#endif // __GUILD_COMBAT_1TO1
				{
					CString str;
					str.Format( prj.GetText( GuildCombatProcess[m_nProcessGo].dwCommand ), GuildCombatProcess[m_nProcessGo].dwTime / 1000 );
					g_DPCoreClient.SendSystem( str );
				}

				if( GuildCombatProcess[m_nProcessGo].dwCommand == TID_GAME_GUILDCOMBAT_OPEN_MSG )
					SendGuildCombatEnterTime( );
			}
			break;
		case GUILDMSG:
			{
			}
			break;
		case JOINMSG:
			{
				CString str;
				str.Format( prj.GetText( GuildCombatProcess[m_nProcessGo].dwCommand ), GuildCombatProcess[m_nProcessGo].dwTime / 1000 );
//				SendJoinMsg( str );
			}
			break;
		case WORLDMSG:
			{
				CString str;
				str.Format( prj.GetText( GuildCombatProcess[m_nProcessGo].dwCommand ), GuildCombatProcess[m_nProcessGo].dwTime / 1000 );
				g_UserMng.AddWorldMsg( WI_WORLD_GUILDWAR, str );
			}
			break;
		case STATE:
			{
				if( g_eLocal.GetState( EVE_GUILDCOMBAT ) )
					g_DPCoreClient.SendGuildCombatState( GuildCombatProcess[m_nProcessGo].dwCommand );
//				m_nState = GuildCombatProcess[m_nProcessGo].dwCommand;
			}
			break;
		case WARSTATE:
			{
				m_nGCState = GuildCombatProcess[m_nProcessGo].dwCommand;
				g_UserMng.AddGuildCombatState();
			}
			break;
		}
		m_dwTime = GetTickCount() + GuildCombatProcess[m_nProcessGo].dwTime;
		if( GuildCombatProcess[m_nProcessGo].dwParam != 0 )
			g_UserMng.AddGuildCombatNextTimeWorld( GuildCombatProcess[m_nProcessGo].dwTime, GuildCombatProcess[m_nProcessGo].dwParam );
		++m_nProcessGo;
	}
}


BOOL CGuildCombat::LoadScript( LPCSTR lpszFileName )
{
	CScanner s;
	
	if( !s.Load( lpszFileName ) )
		return FALSE;

	ZeroMemory( m_nProcessCount, sizeof( int ) * 25 );
	ZeroMemory( GuildCombatProcess, sizeof( __GuildCombatProcess ) * 250 );
	int		nCount;
	int		nAutoOpenCnt = 0;

	while( s.tok != FINISHED )
	{
		nCount = -1;

#if defined(__MAINSERVER)
		if( s.Token == _T( "AUTO_OPEN_IDC" ) )
		{
			s.GetToken();	// {
			s.GetToken();	// {
			
			int nDay, nTime1, nTime2;
			while( *s.token != '}' )
			{
				if( nAutoOpenCnt >= 7 )
				{
					Error( "CGuildCombat::LoadScript() 자동 시작 설정 갯수가 넘음!!" );
					return FALSE;
				}

				nDay = atoi( s.Token );
				m_nDay = nDay;
				if( nDay <= 0 || nDay > 7 )
				{
					Error( "CGuildCombat::LoadScript() Day Error = %d", nDay );
					return FALSE;
				}
				__AutoOpen[nDay-1].bUseing = TRUE;

				nTime1 = s.GetNumber();
				if( nTime1 < 0 || nTime1 > 24 )
				{
					Error( "CGuildCombat::LoadScript() Time Error = %d", nTime1 );
					return FALSE;
				}
				__AutoOpen[nDay-1].nHour = nTime1;
				
				nTime2 = s.GetNumber();
				if( nTime2 < 0 || nTime2 > 59 )
				{
					Error( "CGuildCombat::LoadScript() Time Error = %d", nTime2 );
					return FALSE;
				}
				__AutoOpen[nDay-1].nMinute = nTime2;

				nAutoOpenCnt++;

				s.GetToken();
			}		
		}
#else
		if( s.Token == _T( "AUTO_OPEN_TEST" ) )
		{
			s.GetToken();	// {
			s.GetToken();	// {
			
			int nDay, nTime1, nTime2;
			while( *s.token != '}' )
			{
				if( nAutoOpenCnt >= 7 )
				{
					Error( "CGuildCombat::LoadScript() 자동 시작 설정 갯수가 넘음!!" );
					return FALSE;
				}
				
				nDay = atoi( s.Token );
				m_nDay = nDay;
				if( nDay <= 0 || nDay > 7 )
				{
					Error( "CGuildCombat::LoadScript() Day Error = %d", nDay );
					return FALSE;
				}
				__AutoOpen[nDay-1].bUseing = TRUE;
				
				nTime1 = s.GetNumber();
				if( nTime1 < 0 || nTime1 > 24 )
				{
					Error( "CGuildCombat::LoadScript() Time Error = %d", nTime1 );
					return FALSE;
				}
				__AutoOpen[nDay-1].nHour = nTime1;
				
				nTime2 = s.GetNumber();
				if( nTime2 < 0 || nTime2 > 59 )
				{
					Error( "CGuildCombat::LoadScript() Time Error = %d", nTime2 );
					return FALSE;
				}
				__AutoOpen[nDay-1].nMinute = nTime2;
				
				nAutoOpenCnt++;
				
				s.GetToken();
			}		
		}
#endif
		else if( s.Token == _T( "JOINPENYA" ) )
		{
			m_nJoinPanya = s.GetNumber();
		}
		else if( s.Token == _T( "GUILDLEVEL" ) )
		{
			m_nGuildLevel = s.GetNumber();
		}
#if __VER >= 8 // __GUILDCOMBAT_85
		else if( s.Token == _T( "MINJOINGUILDSIZE" ) )
		{
			m_nMinGuild = s.GetNumber();
		}
#endif // __VER >= 8
		else if( s.Token == _T( "MAXJOINGUILDSIZE" ) )
		{
			m_nMaxGuild = s.GetNumber();
		}
		else if( s.Token == _T( "MAXPLAYERLIFE" ) )
		{
			m_nMaxPlayerLife = s.GetNumber();
		}
		else if( s.Token == _T( "MAXWARPLAYER" ) )
		{
			m_nMaxWarPlayer = s.GetNumber();
		}
		else if( s.Token == _T( "MAXMAPTIME" ) )
		{
			m_nMaxMapTime = s.GetNumber();
		}
		else if( s.Token == _T( "MAXJOINMEMBERSIZE" ) )
		{
			m_nMaxJoinMember = s.GetNumber();
		}
		else if( s.Token == _T( "MAXGUILDPERCENT" ) )
		{
			m_nMaxGuildPercent = s.GetNumber();
		}
		else if( s.Token == _T( "MAXPLAYERPERCENT" ) )
		{
			m_nMaxPlayerPercent = s.GetNumber();
		}
		else if( s.Token == _T( "REQUESTCANCLEPERCENT" ) )
		{
			m_nRequestCanclePercent = s.GetNumber();
		}
		else if( s.Token == _T( "NOTREQUESTPERCENT" ) )
		{
			m_nNotRequestPercent = s.GetNumber();
		}
		else if( s.Token == _T( "ITEMPENYA" ) )
		{
			m_nItemPenya = s.GetNumber();
		}
#if __VER >= 8 // __GUILDCOMBAT_85
		else if( s.Token == _T( "MAX_GCSIENDITEM" ) )
		{
			m_nMaxGCSendItem = s.GetNumber();
		}
		else if( s.Token == _T( "GCSENDITEM" ) )
		{
			int nWinCount = s.GetNumber();
			s.GetToken();
			DWORD dwItemId = CScript::GetDefineNum( s.Token );
			if( dwItemId == -1 )
			{
				Error( "CGuildCombat::LoadScript() 아이템이 없는것을 설정했음" );
				return FALSE;
			}
			int nItemNum = s.GetNumber();
			__GCSENDITEM GCSendItem;
			GCSendItem.nWinCount = nWinCount;
			GCSendItem.dwItemId = dwItemId;
			GCSendItem.nItemNum = nItemNum;
			vecGCSendItem.push_back( GCSendItem );
		}
#endif // __VER >= 8
		else if( s.Token == _T( "OPEN" ) )
		{
			nCount = 0;
		}
		else if( s.Token == _T( "MAINTENANCE" ) )
		{
			nCount = 1;
		}
		else if( s.Token == _T( "WAR" ) )
		{
			nCount = 2;
		}

		if( nCount != -1 )
		{
			m_nProcessCount[nCount] = s.GetNumber();
			s.GetToken();	// {
			s.GetToken();	// {
			int nSoCount = 0;
			while( *s.token != '}' )
			{
				if( s.Token == _T( "ALLMSG" ) )
				{
					GuildCombatProcess[25*nCount+nSoCount].nState	= ALLMSG;
				}
				else if( s.Token == _T( "GUILDMSG" ) )
				{
					GuildCombatProcess[25*nCount+nSoCount].nState	= GUILDMSG;
				}
				else if( s.Token == _T( "JOINMSG" ) )
				{
					GuildCombatProcess[25*nCount+nSoCount].nState	= JOINMSG;
				}
				else if( s.Token == _T( "WORLDMSG" ) )
				{
					GuildCombatProcess[25*nCount+nSoCount].nState	= WORLDMSG;
				}
				else if( s.Token == _T( "STATE" ) )
				{
					GuildCombatProcess[25*nCount+nSoCount].nState	= STATE;
				}
				else if( s.Token == _T( "WARSTATE" ) )
				{
					GuildCombatProcess[25*nCount+nSoCount].nState	= WARSTATE;
				}
				else if( s.Token == _T( "WAIT" ) )
				{
					GuildCombatProcess[25*nCount+nSoCount].nState	= WAIT;
				}
				GuildCombatProcess[25*nCount+nSoCount].dwTime	= s.GetNumber();
				GuildCombatProcess[25*nCount+nSoCount].dwCommand	= s.GetNumber();
				GuildCombatProcess[25*nCount+nSoCount].dwParam		= s.GetNumber();
				if( GuildCombatProcess[25*nCount+nSoCount].nState == STATE )
					m_nProcessCount[GuildCombatProcess[25*nCount+nSoCount].dwCommand] = 25*nCount+nSoCount;
				
				++nSoCount;
				s.GetToken();
			}
		}

		s.GetToken();
	}	
	return TRUE;
}

void CGuildCombat::SendGCLog( void )
{
	for( int nVeci = 0 ; nVeci < (int)( vecRequestRanking.size() ) ; ++nVeci )
	{
		if( nVeci >= m_nMaxGuild )
			break;
		
		__REQUESTGUILD RequestGuild = vecRequestRanking[nVeci];
#ifdef __S_BUG_GC
		__GuildCombatMember* pGCMember = FindGuildCombatMember( RequestGuild.uidGuild );
		if( pGCMember != NULL )
		{
			int nPoint = 0;
			for( int veci = 0 ; veci < (int)( pGCMember->vecGCSelectMember.size() ) ; ++veci )
			{
				__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];
				CUser* pUser = (CUser*)prj.GetUserByID( pJoinPlayer->uidPlayer );
				if( IsValidObj( pUser ) )
					pUser->AddGCLog();
			}
		}
#else // __S_BUG_GC
		map<u_long, __GuildCombatMember*>::iterator it = m_GuildCombatMem.find( RequestGuild.uidGuild );
		if( it != m_GuildCombatMem.end() )
		{
			int nPoint = 0;
			__GuildCombatMember* pGCMember = it->second;
			for( int veci = 0 ; veci < pGCMember->vecGCSelectMember.size() ; ++veci )
			{
				__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];
				CUser* pUser = (CUser*)prj.GetUserByID( pJoinPlayer->uidPlayer );
				if( IsValidObj( pUser ) )
				{
					pUser->AddGCLog();
				}
			}
		}
#endif // __S_BUG_GC
	}
}

void CGuildCombat::SendJoinMsg( LPCTSTR lpszString  )
{

}

void CGuildCombat::SendGuildCombatEnterTime( void )
{
	g_UserMng.AddGuildCombatEnterTime( GuildCombatProcess[m_nProcessGo].dwTime );
/*	for( map<u_long, __GuildCombatMember*>::iterator ita = m_GuildCombatMem.begin() ; ita != m_GuildCombatMem.end() ; ++ita )
	{
		CGuild* pGuild = g_GuildMng.GetGuild( ita->first );
		if( pGuild )
		{
			CGuildMember*	pMember;
			CUser*			pUsertmp;
			for( map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin();
			i != pGuild->m_mapPMember.end(); ++i )
			{
				pMember		= i->second;
				pUsertmp	= (CUser*)prj.GetUserByID( pMember->m_idPlayer );
				if( IsValidObj( pUsertmp ) )
				{
					pUsertmp->AddGuildCombatEnterTime( GuildCombatProcess[m_nProcessGo].dwTime );
				}
			}
		}
	}
*/
}

void CGuildCombat::SetPlayerChange( CUser* pUser, CUser* pLeader )
{
#ifdef __S_BUG_GC
	__GuildCombatMember* pGCMember = FindGuildCombatMember( pUser->m_idGuild );
	if( pGCMember != NULL )
	{
		for( int i = 0; i < (int)( pGCMember->vecGCSelectMember.size() ) ; ++i )
		{
			__JOINPLAYER* pJoinPlayer	= pGCMember->vecGCSelectMember[i];
			if( pJoinPlayer->uidPlayer == pUser->m_idPlayer )
			{
				// 대기열에 존재 하는  pUser는 전투와 무관하므로 무시
				for( list<__JOINPLAYER*>::iterator i1 = pGCMember->lspFifo.begin(); i1 != pGCMember->lspFifo.end(); ++i1 )
				{
					if( pJoinPlayer == *i1 )
						return;
				}
//				pJoinPlayer->bEntry	= FALSE;
				pJoinPlayer->dwTelTime	= 0;
				pUser->m_nGuildCombatState	= 0;
				g_UserMng.AddGuildCombatUserState( pUser );
				--pGCMember->nWarCount;
				pJoinPlayer->nlife--;
				if( pJoinPlayer->nlife > 0 )
					pGCMember->lspFifo.push_back( pJoinPlayer );
				if( pJoinPlayer->nlife < 0 )
					pJoinPlayer->nlife	= 0;

				while( pGCMember->lspFifo.size() > 0 )
				{
					__JOINPLAYER* pJoinPlayer	= pGCMember->lspFifo.front();
					CMover* pMover	= prj.GetUserByID( pJoinPlayer->uidPlayer );
					if( IsValidObj( pMover ) && pMover->GetWorld()->GetID() == WI_WORLD_GUILDWAR )
					{
						pGCMember->nWarCount++;
//						pJoinPlayer->bEntry	= TRUE;
						pJoinPlayer->dwTelTime	= timeGetTime();
						((CUser*)pMover)->AddGCJoinWarWindow( pJoinPlayer->nMap, m_nMaxMapTime );
						pGCMember->lspFifo.pop_front();
					}
					else
					{
						pGCMember->lspFifo.pop_front();
						pJoinPlayer->nlife--;
						if( pJoinPlayer->nlife > 0 )
							pGCMember->lspFifo.push_back( pJoinPlayer );
						if( pJoinPlayer->nlife < 0 )
							pJoinPlayer->nlife	= 0;
					}
					if( pGCMember->nWarCount == m_nMaxWarPlayer )
						break;
				}	// while
				break;
			}	// if
		}	// for
	}	// if
#else // __S_BUG_GC
	map<u_long, __GuildCombatMember*>::iterator it = m_GuildCombatMem.find( pUser->m_idGuild );
	if( it != m_GuildCombatMem.end() )
	{
		__GuildCombatMember* pGCMember	= it->second;
		for( int i = 0; i < pGCMember->vecGCSelectMember.size() ; ++i )
		{
			__JOINPLAYER* pJoinPlayer	= pGCMember->vecGCSelectMember[i];
			if( pJoinPlayer->uidPlayer == pUser->m_idPlayer )
			{
				// 대기열에 존재 하는  pUser는 전투와 무관하므로 무시
				for( list<__JOINPLAYER*>::iterator i1 = pGCMember->lspFifo.begin(); i1 != pGCMember->lspFifo.end(); ++i1 )
				{
					if( pJoinPlayer == *i1 )
						return;
				}
//				pJoinPlayer->bEntry	= FALSE;
				pJoinPlayer->dwTelTime	= 0;
				pUser->m_nGuildCombatState	= 0;
				g_UserMng.AddGuildCombatUserState( pUser );
				--pGCMember->nWarCount;
				pJoinPlayer->nlife--;
				if( pJoinPlayer->nlife > 0 )
					pGCMember->lspFifo.push_back( pJoinPlayer );
				if( pJoinPlayer->nlife < 0 )
					pJoinPlayer->nlife	= 0;

				while( pGCMember->lspFifo.size() > 0 )
				{
					__JOINPLAYER* pJoinPlayer	= pGCMember->lspFifo.front();
					CMover* pMover	= prj.GetUserByID( pJoinPlayer->uidPlayer );
					if( IsValidObj( pMover ) && pMover->GetWorld()->GetID() == WI_WORLD_GUILDWAR )
					{
						pGCMember->nWarCount++;
//						pJoinPlayer->bEntry	= TRUE;
						pJoinPlayer->dwTelTime	= timeGetTime();
						((CUser*)pMover)->AddGCJoinWarWindow( pJoinPlayer->nMap, m_nMaxMapTime );
						pGCMember->lspFifo.pop_front();
					}
					else
					{
						pGCMember->lspFifo.pop_front();
						pJoinPlayer->nlife--;
						if( pJoinPlayer->nlife > 0 )
							pGCMember->lspFifo.push_back( pJoinPlayer );
						if( pJoinPlayer->nlife < 0 )
							pJoinPlayer->nlife	= 0;
					}
					if( pGCMember->nWarCount == m_nMaxWarPlayer )
						break;
				}	// while
				break;
			}	// if
		}	// for
	}	// if
#endif // __S_BUG_GC
}
void CGuildCombat::GetPoint( CUser* pAttacker, CUser* pDefender )
{
	// 선택한 넘들 가지고 오기
	BOOL bMaster = FALSE;
	BOOL bDefender = FALSE;
	BOOL bLastLife = FALSE;
	BOOL bKillDiffernceGuild = FALSE;
	int nGetPoint = 2;
	__JOINPLAYER* pJoinPlayerAttacker = NULL;
	__JOINPLAYER* pJoinPlayerDefender = NULL;
	__GuildCombatMember* pGCMemberAttacker = NULL;
	CGuild* pGuild	= g_GuildMng.GetGuild( pAttacker->m_idGuild );
	if( pGuild && pGuild->IsMaster( pAttacker->m_idPlayer ) )
		bMaster = TRUE;

#ifdef __S_BUG_GC
	pGCMemberAttacker = FindGuildCombatMember( pAttacker->m_idGuild );
	if( pGCMemberAttacker != NULL )
	{
		for( int veci = 0 ; veci < (int)( pGCMemberAttacker->vecGCSelectMember.size() ) ; ++veci )
		{
			__JOINPLAYER* pJoinPlayer = pGCMemberAttacker->vecGCSelectMember[veci];
			if( pJoinPlayer->uidPlayer == pAttacker->m_idPlayer )
			{
				pJoinPlayerAttacker = pJoinPlayer;
				break;
			}
		}
		if( pJoinPlayerAttacker == NULL )
			return;

		if( pJoinPlayerAttacker->uKillidGuild != 0 && pJoinPlayerAttacker->uKillidGuild != pDefender->m_idGuild )
			bKillDiffernceGuild = TRUE;

		if( pJoinPlayerAttacker->nlife == 1 )
			bLastLife = TRUE;
	}
#else // __S_BUG_GC
	map<u_long, __GuildCombatMember*>::iterator it = m_GuildCombatMem.find( pAttacker->m_idGuild );
	if( it != m_GuildCombatMem.end() )
	{
		pGCMemberAttacker	= it->second;
		for( int veci = 0 ; veci < (int)( pGCMemberAttacker->vecGCSelectMember.size() ) ; ++veci )
		{
			__JOINPLAYER* pJoinPlayer = pGCMemberAttacker->vecGCSelectMember[veci];
			if( pJoinPlayer->uidPlayer == pAttacker->m_idPlayer )
			{
				pJoinPlayerAttacker = pJoinPlayer;
				break;
			}
		}
		if( pJoinPlayerAttacker == NULL )
			return;

		if( pJoinPlayerAttacker->uKillidGuild != 0 && pJoinPlayerAttacker->uKillidGuild != pDefender->m_idGuild )
			bKillDiffernceGuild = TRUE;

		if( pJoinPlayerAttacker->nlife == 1 )
			bLastLife = TRUE;
	}	
#endif // __S_BUG_GC

	if( pGCMemberAttacker == NULL )
		return;

#ifdef __S_BUG_GC
	__GuildCombatMember* pGCMember = FindGuildCombatMember( pDefender->m_idGuild );
	if( pGCMember != NULL )
	{
		for( int veci = 0 ; veci < (int)( pGCMember->vecGCSelectMember.size() ) ; ++veci )
		{
			__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];
			if( pJoinPlayer->uidPlayer == pDefender->m_idPlayer )
			{
				pJoinPlayerDefender = pJoinPlayer;
				break;
			}
		}
		if( pJoinPlayerDefender == NULL )
			return;

		if( pDefender->m_idPlayer == pGCMember->m_uidDefender )
			bDefender = TRUE;
	}	
#else // __S_BUG_GC
	it = m_GuildCombatMem.find( pDefender->m_idGuild );
	if( it != m_GuildCombatMem.end() )
	{
		__GuildCombatMember* pGCMember	= it->second;
		for( int veci = 0 ; veci < pGCMember->vecGCSelectMember.size() ; ++veci )
		{
			__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];
			if( pJoinPlayer->uidPlayer == pDefender->m_idPlayer )
			{
				pJoinPlayerDefender = pJoinPlayer;
				break;
			}
		}
		if( pJoinPlayerDefender == NULL )
			return;

		if( pDefender->m_idPlayer == pGCMember->m_uidDefender )
			bDefender = TRUE;
	}	
#endif // __S_BUG_GC

	// 이전과 다른 길드원을 Kill 했을 경우
	if( bKillDiffernceGuild )
		++nGetPoint;
	// 길드마스터가 적 길드원을 Kill 했을 경우
	if( bMaster )
		++nGetPoint;
	// 디펜더를 Kill 했을 경우
	if( bDefender )
		++nGetPoint;
	// 부활포인트 0일 때 Kill 했을 경우
	if( bLastLife )
		++nGetPoint;

	pJoinPlayerAttacker->nPoint += nGetPoint;
	pGCMemberAttacker->nGuildPoint += nGetPoint;
	
	CGuild* pGuildMsg;
	CGuildMember*	pMember;
	CUser*			pUsertmp;

	pGuildMsg = pAttacker->GetGuild();
	if( pGuildMsg )
	{
		for( map<u_long, CGuildMember*>::iterator i = pGuildMsg->m_mapPMember.begin();
		i != pGuildMsg->m_mapPMember.end(); ++i )
		{
			pMember		= i->second;
			pUsertmp	= (CUser*)prj.GetUserByID( pMember->m_idPlayer );
			if( IsValidObj( pUsertmp ) && pUsertmp->GetWorld() && pUsertmp->GetWorld()->GetID() == WI_WORLD_GUILDWAR )
			{
				if( bMaster )
					pUsertmp->AddDefinedCaption( TRUE, TID_GAME_GUILDCOMBAT_POINT_MASTER, "%d", nGetPoint );
				else 
					pUsertmp->AddDefinedCaption( TRUE, TID_GAME_GUILDCOMBAT_POINT_GENERAL, "%d", nGetPoint );
			}
		}
	}

	if( bDefender )
	{
		pGuildMsg = pDefender->GetGuild();
		if( pGuildMsg )
		{
			for( map<u_long, CGuildMember*>::iterator i = pGuildMsg->m_mapPMember.begin();
			i != pGuildMsg->m_mapPMember.end(); ++i )
			{
				pMember		= i->second;
				pUsertmp	= (CUser*)prj.GetUserByID( pMember->m_idPlayer );
				if( IsValidObj( pUsertmp ) && pUsertmp->GetWorld() && pUsertmp->GetWorld()->GetID() == WI_WORLD_GUILDWAR )
				{
					if( pGuild )
						pUsertmp->AddDefinedCaption( TRUE, TID_GAME_GUILDCOMBAT_POINT_DEFENDER, "\"%s\"", pGuild->m_szGuild );	
				}
			}
		}
	}
	AddvecGCGetPoint( pAttacker->m_idGuild, pDefender->m_idGuild, pAttacker->m_idPlayer, pDefender->m_idPlayer, nGetPoint, bKillDiffernceGuild, bMaster, bDefender, bLastLife );
	g_UserMng.AddGCGuildPrecedence();
	g_UserMng.AddGCPlayerPrecedence();
}

void CGuildCombat::SerializeGCWarPlayerList( CAr & ar )
{
	if( m_nMaxGuild <= (int)( vecRequestRanking.size() ) )
		ar << m_nMaxGuild;
	else
		ar << (int)vecRequestRanking.size();

	for( int nVeci = 0 ; nVeci < (int)( vecRequestRanking.size() ) ; ++nVeci )
	{
		if( nVeci >= m_nMaxGuild )
			break;
		
		__REQUESTGUILD RequestGuild = vecRequestRanking[nVeci];
#ifdef __S_BUG_GC
		__GuildCombatMember* pGCMember = FindGuildCombatMember( RequestGuild.uidGuild );
		if( pGCMember != NULL )
		{
			__JOINPLAYER* pJoinPlayer1;

			ar << pGCMember->m_uidDefender;
			ar << (int)pGCMember->vecGCSelectMember.size();
			for( int vecSi = 0 ; vecSi < (int)( pGCMember->vecGCSelectMember.size() ) ; ++vecSi )
			{
				pJoinPlayer1 = pGCMember->vecGCSelectMember[vecSi];

				ar << pJoinPlayer1->uidPlayer;
				if( 0 < pJoinPlayer1->nlife )
				{
					BOOL bFind	= FALSE;
					for( list<__JOINPLAYER*>::iterator i1 = pGCMember->lspFifo.begin(); i1 != pGCMember->lspFifo.end(); ++i1 )
					{
						if( pJoinPlayer1 == *i1 )
						{
							bFind	= TRUE;
							break;
						}
					}
				
					if( bFind )	// 대기중
					{
						ar << (int)0;
					}
					else // 전투자
					{
						ar << (int)1;
					}
				}
				else // 대기중
				{
					ar << (int)0;
				}
			}
		}
#else // __S_BUG_GC
		map<u_long, __GuildCombatMember*>::iterator it = m_GuildCombatMem.find( RequestGuild.uidGuild );
		if( it != m_GuildCombatMem.end() )
		{
			__GuildCombatMember* pGCMember = it->second;
			__JOINPLAYER* pJoinPlayer1;

			ar << pGCMember->m_uidDefender;
			ar << (int)pGCMember->vecGCSelectMember.size();
			for( int vecSi = 0 ; vecSi < pGCMember->vecGCSelectMember.size() ; ++vecSi )
			{
				pJoinPlayer1 = pGCMember->vecGCSelectMember[vecSi];

				ar << pJoinPlayer1->uidPlayer;
				if( 0 < pJoinPlayer1->nlife )
				{
					BOOL bFind	= FALSE;
					for( list<__JOINPLAYER*>::iterator i1 = pGCMember->lspFifo.begin(); i1 != pGCMember->lspFifo.end(); ++i1 )
					{
						if( pJoinPlayer1 == *i1 )
						{
							bFind	= TRUE;
							break;
						}
					}
				
					if( bFind )	// 대기중
					{
						ar << (int)0;
					}
					else // 전투자
					{
						ar << (int)1;
					}
				}
				else // 대기중
				{
					ar << (int)0;
				}
			}
		}
#endif // __S_BUG_GC
		else
		{
			ar << (u_long)0;
			ar << (int)0;
		}
	}
}
#endif	// __WORLDSERVER

CGuildCombat	g_GuildCombatMng;
