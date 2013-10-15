#include "StdAfx.h"
#include "defineObj.h"
#include "dbmanager.h"
#include "dploginsrvr.h"
#include "dpcoresrvr.h"
#include "dptrans.h"
#include "..\_Network\Objects\Obj.h"
#include "misc.h"
#include "mytrace.h"

#include "party.h"
#include "guild.h"
extern	CGuildMng	g_GuildMng;
#include "guildwar.h"
extern	CGuildWarMng	g_GuildWarMng;

#include "dpaccountclient.h"
extern	CDPAccountClient	g_dpAccountClient;

extern const int MAX_TASKBAR; 
extern const int MAX_APPLETTASKBAR; 
extern const int MAX_ITEMTASKBAR;   
extern const char NullStr[2];

#if defined( __VERIFY_PLAYER ) || defined( __PROVIDE ) || defined( __S0707_ITEM_CONV ) || defined(__RECOVER0816)
#define	MAX_QUERY_SIZE	1024 * 64
#include "resource.h"
BOOL	CALLBACK	VerifyPlayerDlgProc( HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam );
#endif	// __VERIFY_PLAYER OR __PROVIDE

extern  CDPCoreSrvr			g_dpCoreSrvr;
extern	CDPLoginSrvr		g_dpLoginSrvr;
extern	CProject			prj;
extern	APP_INFO			g_appInfo;
extern	CMyTrace			g_MyTrace;

#define	VERIFYSTRING( lpString, lpszPlayer )	\
		if( FALSE == VerifyString( lpString, __FILE__, __LINE__, lpszPlayer, lpDBOP ) )		return;

#define	VERIFY_GUILD_STRING( lpString, lpszGuild )	\
if( FALSE == VerifyString( lpString, __FILE__, __LINE__, lpszGuild ) )		return;

void CDbManager::SendJoin( CMover* pMover, LPDB_OVERLAPPED_PLUS lpDBOP, DWORD dwAuthKey, 
						  DWORD dwBillingClass, LPCTSTR szPartyName, LPCTSTR szBankPass, 
						  DWORD dwPlayTime, DWORD dwFlyTime, int nMaximumLevel, int nTags, TAG_ENTRY* tags,
						  LPCTSTR szTimeGuild )
{
	PLAY_ACCOUNT playAccount;
	_tcscpy( playAccount.lpszAccount, lpDBOP->AccountInfo.szAccount );
	GetLocalTime( &playAccount.m_stLogin );
#ifdef __BILLING0712
	playAccount.dwBillingClass	= dwBillingClass;
#endif	// __BILLING0712

	BEFORESENDDUAL( ar, PACKETTYPE_JOIN, lpDBOP->dpidCache, lpDBOP->dpidUser );
	ar << dwAuthKey;
	ar << playAccount;
	ar << pMover->m_dwWorldID;
#ifdef __LAYER_1015
	ar << pMover->GetLayer();
#endif	// __LAYER_1015
	ar << pMover->m_dwIndex;
	pMover->Serialize( ar );			
	pMover->m_UserTaskBar.Serialize( ar );
#ifdef __RT_1025
	pMover->m_RTMessenger.Serialize( ar );
#else	// __RT_1025
	pMover->m_Messenger.Serialize( ar );
#endif	// __RT_1025
	ar.WriteString( szPartyName );
	ar.WriteString( szBankPass );
	ar << dwPlayTime;
#ifndef __JEFF_9_20
	ar << dwFlyTime;
#endif	// __JEFF_9_20
	ar << nMaximumLevel;
	WriteTag(ar, nTags, tags);	// 쪽지 쓰기 
	ar.WriteString( szTimeGuild );
	ar << pMover->m_dwReturnWorldID;
	ar << pMover->m_vReturnPos;

#ifdef __EVENT_1101
#ifdef __EVENT_1101_2
	ar << pMover->m_nEventFlag;
#else // __EVENT_1101_2
	ar << pMover->m_dwEventFlag;
#endif // __EVENT_1101_2
	ar << pMover->m_dwEventTime;
	ar << pMover->m_dwEventElapsed;
#endif	// __EVENT_1101

#ifdef __EXP_ANGELEXP_LOG
	ar << pMover->m_nExpLog;
	ar << pMover->m_nAngelExpLog;
#endif // __EXP_ANGELEXP_LOG

#ifdef __EVENTLUA_COUPON
	ar << pMover->m_nCoupon;
#endif // __EVENTLUA_COUPON
#if __VER >= 14 // __PCBANG
	ar << pMover->m_dwPCBangClass;
#endif // __PCBANG
#if __VER >= 15 // __GUILD_HOUSE
	ar << pMover->m_nRestPoint << static_cast<time_t>( pMover->m_tLogOut ? time_null() - pMover->m_tLogOut : 0 );
#endif // __GUILD_HOUSE
	SEND( ar, CDPTrans::GetInstance(), lpDBOP->dpid );
	
	char lpOutputString[128]	= { 0, };
	sprintf( lpOutputString, "DATABASESERVER.EXE\t// JOIN_1\t// %d\n", lpDBOP->dpid );
	OutputDebugString( lpOutputString );
}

/*
캐릭터 복구시 - 뱅크가 사라지는 문제 

캐릭터 생성
해당 캐릭의 뱅크슬롯에 아이템 넣음
캐릭터 삭제
캐릭터 복구
로그인 
S8
result set의 뱅크 정보를 읽지 않음 ( 캐쉬 hit 이기 때문에 )
캐릭터를 새로 읽어도 뱅크는 새로 읽지 않음( 캐쉬에 있기 때문에 )
*/

void CDbManager::Join( CQuery* qry, CQuery* qry1, CQuery* qrylog, LPDB_OVERLAPPED_PLUS lpDBOP )
{
	BYTE	_nSlot;
	u_long	idPlayer;
	DWORD	dwAuthKey, dwBillingClass		= 0;
	char	szBankPass[5]	= { 0, };
	char	szPartyName[36]	= { 0, };
	char	szTimeGuild[15]	= { 0, };
	TAG_ENTRY tags[MAX_TAGS];

	CAr arRead( lpDBOP->lpBuf, lpDBOP->uBufSize );
	arRead >> dwAuthKey;	// o
	arRead.ReadString( lpDBOP->AccountInfo.szAccount, MAX_ACCOUNT );
	arRead.ReadString( lpDBOP->AccountInfo.szPassword, MAX_PASSWORD );
	arRead >> _nSlot >> idPlayer;
	lpDBOP->AccountInfo.nPlayerSlot		= _nSlot;

	arRead >> lpDBOP->dpid >> lpDBOP->dpidCache >> lpDBOP->dpidUser;
#ifdef __BILLING0712
	arRead >> dwBillingClass;
#endif	// __BILLING0712
#if __VER >= 14 // __PCBANG
	DWORD dwPCBangClass;
	arRead >> dwPCBangClass;
#endif // __PCBANG
	char lpOutputString[128]	= { 0, };
	sprintf( lpOutputString, "DATABASESERVER.EXE\t// JOIN_0\t// %d\n", lpDBOP->dpid );
	OutputDebugString( lpOutputString );

	// CHARACTER_STR 'S8',@im_idPlayer,@iserverindex,@account
	// CHARACTER_STR 'S8','000001','01','홍길동'
	char szQuery[QUERY_SIZE]	= { 0,};
	DBQryCharacter( szQuery, "S8", idPlayer, g_appInfo.dwSys, lpDBOP->AccountInfo.szAccount );

	if( !qry->Exec( szQuery ) )
	{
		FreeRequest( lpDBOP );
		return;
	}
	if( !qry->Fetch() )
	{
		FreeRequest( lpDBOP );
		return;
	}
	qry->GetStr( "m_szName", lpDBOP->AccountInfo.szPlayer );
	m_joinLock.Enter();
	strcpy( m_aszJoin[m_nJoin], lpDBOP->AccountInfo.szPlayer );
	m_nJoin	= ( m_nJoin + 1 ) % MAX_JOIN_SIZE;
	m_joinLock.Leave();

	
	BYTE nSlot = (BYTE)qry->GetInt( "playerslot" );

	// 임시 변수에 쿼리 결과를 저장한다
	CMover	mover;
	mover.m_nSlot	= nSlot;
#if __VER >= 14 // __PCBANG
	mover.m_dwPCBangClass = dwPCBangClass;
#endif // __PCBANG
	mover.m_dwIndex	= (DWORD)qry->GetInt( "m_dwIndex" );
	mover.InitProp();
	mover.m_idPlayer	= (u_long)qry->GetInt( "m_idPlayer" );

	BOOL bRefresh	= FALSE;
	if( qry->GetInt( "m_idCompany" ) > 0 )
		bRefresh	= TRUE;

	/*GetInventory( &mover, qry, lpDBOP );
	GetEquipment( &mover, qry, lpDBOP );
	GetTaskBar( &mover, qry, lpDBOP );
	GetCardCube( &mover, qry, lpDBOP );
	GetBaseCharacter( &mover, qry, lpDBOP );
	GetJobLv( &mover, qry, lpDBOP );
	GetQuest( &mover, qry, lpDBOP );
	GetSMMode( &mover, qry, lpDBOP );
	GetSKillInfluence( &mover, qry, lpDBOP );*/
	// mirchang_100416 VERIFYSTRING use return value
	GetBaseCharacter( &mover, qry, lpDBOP );
	if( GetInventory( &mover, qry, lpDBOP ) == FALSE
		|| GetEquipment( &mover, qry, lpDBOP ) == FALSE
		|| GetTaskBar( &mover, qry, lpDBOP ) == FALSE
		|| GetQuest( &mover, qry, lpDBOP ) == FALSE
		|| GetSMMode( &mover, qry, lpDBOP ) == FALSE
		|| GetSKillInfluence( &mover, qry, lpDBOP ) == FALSE )
	{
		return;
	}
	GetCardCube( &mover, qry, lpDBOP );
	GetJobLv( &mover, qry, lpDBOP );
	// mirchang_100416

#ifdef __RT_1025
	mover.m_RTMessenger.SetState( (DWORD)qry->GetInt( "m_nMessengerState" ) );
#else	// __RT_1025
	mover.m_Messenger.m_dwMyState	= (DWORD)qry->GetInt( "m_nMessengerState" );
#endif	// __RT_1025
	mover.m_dwReturnWorldID		= qry->GetInt( "m_dwReturnWorldID" );
	mover.m_vReturnPos.x	= qry->GetFloat( "m_vReturnPos_x" );
	mover.m_vReturnPos.y	= qry->GetFloat( "m_vReturnPos_y" );
	mover.m_vReturnPos.z	= qry->GetFloat( "m_vReturnPos_z" );
	
	DWORD dwPlayTime	= qry->GetInt( "TotalPlayTime" );
#ifdef __JEFF_9_20
	mover.m_dwMute		= qry->GetInt( "m_dwFlyTime" );
	DWORD dwFlyTime		= 0;	//
#else	// __JEFF_9_20
	DWORD dwFlyTime		= qry->GetInt( "m_dwFlyTime" );
#endif	// __JEFF_9_20
	int nMaximumLevel	= qry->GetInt( "m_nMaximumLevel" );
	qry->GetStr( "m_tGuildMember", szTimeGuild );
	
	char cAuth = qry->GetChar( "m_chLoginAuthority" );
	if( cAuth == 'F' )
		cAuth = qry->GetChar( "m_chAuthority" );	
	mover.m_dwAuthorization = (DWORD)cAuth;
	mover.m_dwMode	&= ~( EVENT_OLDBOY_MODE );
	if( qry->GetInt( "last_connect" ) == 0 )
		mover.m_dwMode	|= EVENT_OLDBOY_MODE;
	mover.m_idGuild		= (u_long)qry->GetInt( "m_idGuild" );
	mover.m_idWar	= (u_long)qry->GetInt( "m_idWar" );

#ifdef __EVENT_1101
#ifdef __EVENT_1101_2
	mover.m_nEventFlag	= qry->GetInt64( "dwEventFlag" );
#else // __EVENT_1101_2
	mover.m_dwEventFlag	= (DWORD)qry->GetInt( "dwEventFlag" );
#endif // __EVENT_1101_2
	mover.m_dwEventTime	= (DWORD)qry->GetInt( "dwEventTime" );
	mover.m_dwEventElapsed	= (DWORD)qry->GetInt( "dwEventElapsed" );
#endif	// __EVENT1101

	DWORD dwPetId	= (DWORD)qry->GetInt( "m_dwPetId" );
	mover.SetPetId( dwPetId );

#ifdef __EXP_ANGELEXP_LOG
	mover.m_nExpLog			= qry->GetInt( "m_nExpLog" );
	mover.m_nAngelExpLog	= qry->GetInt( "m_nAngelExpLog" );
#endif // __EXP_ANGELEXP_LOG

#ifdef __EVENTLUA_COUPON
	mover.m_nCoupon = qry->GetInt( "m_nCoupon" );
#endif // __EVENTLUA_COUPON

#ifdef __LAYER_1015
	mover.SetLayer( qry->GetInt( "m_nLayer" ) );
#endif	// __LAYER_1015

#if __VER >= 15 // __CAMPUS
	mover.SetCampusPoint( qry->GetInt( "m_nCampusPoint" ) );
	mover.SetCampusId( qry->GetInt( "idCampus" ) );
#endif // __CAMPUS

#if __VER >= 15 // __GUILD_HOUSE
	mover.m_nRestPoint = qry->GetInt( "m_nRestPoint" );
	mover.m_tLogOut = time_null() - qry->GetInt( "m_LogOutTime" );
#endif // __GUILD_HOUSE

	if( !qry->MoreResults() )
	{
		FreeRequest( lpDBOP );
		return;
	}
	while( qry->Fetch() )
	{
		int nPlayerSlot = qry->GetInt( "playerslot" );
		if( nSlot == nPlayerSlot )
			qry->GetStr( "m_BankPw", szBankPass );
		//GetBank( &mover, qry, lpDBOP, nPlayerSlot );
		// mirchang_100416 VERIFYSTRING use return value
		if( GetBank( &mover, qry, lpDBOP, nPlayerSlot ) == FALSE )
		{
			return;
		}
		// mirchang_100416
	}

#if __VER >= 11 // __SYS_POCKET
	if( qry->MoreResults() )
	{
		while( qry->Fetch() )
		{
			//GetPocket( &mover, qry, lpDBOP );
			// mirchang_100416 VERIFYSTRING use return value
			if( GetPocket( &mover, qry, lpDBOP ) == FALSE )
			{
				return;
			}
			// mirchang_100416
		}
	}
#endif	// __SYS_POCKET

	GetMyPartyName( qry, lpDBOP, mover.m_idPlayer, szPartyName );
	int nTags = SelectTag( qry, lpDBOP, mover.m_idPlayer, tags );
#ifdef __RT_1025
	LoadMessenger( &mover, qry );
#else	// __RT_1025
	GetMessengerFriend( &mover, qry, lpDBOP );
#endif	// __RT_1025

#if __VER >= 13 // __HONORABLE_TITLE			// 달인
	GetHonor( &mover, qry, lpDBOP );
#endif	// __HONORABLE_TITLE			// 달인

	if( !GetSkill( &mover, qry, lpDBOP ) )
	{
		FreeRequest( lpDBOP );
		return;
	}

	m_AddRemoveLock.Enter();
	BOOL bCacheHit	= FALSE;
	BOOL bExists	= FALSE;
	ACCOUNT_CACHE* pCache	= m_AccountCacheMgr.GetAccount( lpDBOP->AccountInfo.szAccount, &bCacheHit ); 
	if( bCacheHit  )
	{
		if( CanJoin( lpDBOP->AccountInfo.szAccount, pCache ) == FALSE )
		{
			m_AddRemoveLock.Leave();
			FreeRequest( lpDBOP );
			return;
		}
		if( pCache->pMover[nSlot] == NULL )
			pCache->pMover[nSlot]	= new CMover;
		else
			bExists		= TRUE;		// 캐시에 남아있는 캐릭터면 캐시 정보를 사용한다
	}
	else
		pCache->pMover[nSlot] = new CMover;
	
	CMover* pMover	= pCache->pMover[nSlot];

#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
	if( memcmp( pCache->m_idPlayerBank, mover.m_idPlayerBank, sizeof(u_long) * 3 ) )
	{
		bRefresh = TRUE;
	}
	if( dwPlayTime == 0 || pMover->m_idPlayer != mover.m_idPlayer )	
		bRefresh = TRUE;
#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz


	// 처음 접속하는 계정이거나 새로고침이 설정되어 있는
	// 캐릭터의 경우 어카운트 캐시에 은행을 복사한다
	if( !bCacheHit || bRefresh )
	{
		for( int j = 0 ; j < 3 ; j++ )
			pCache->m_Bank[j].Copy( mover.m_Bank[j] );
		memcpy( pCache->m_idPlayerBank, mover.m_idPlayerBank, sizeof(u_long) * 3 );
		memcpy( pCache->m_dwGoldBank, mover.m_dwGoldBank, sizeof(DWORD) * 3 );
	}

	pMover->Lock();
	// 캐릭터가 캐시에 없거나 새로고침이 설정되어 있으면
	// 모든 정보를 복사한다
	pMover->Copy( &mover, !bExists || bRefresh );
	// 캐릭터에 은행을 복사한다
	for( int j = 0 ; j < 3 ; j++ )
		pMover->m_Bank[j].Copy( pCache->m_Bank[j] );
	memcpy( pMover->m_idPlayerBank, pCache->m_idPlayerBank, sizeof(u_long) * 3 );
	memcpy( pMover->m_dwGoldBank, pCache->m_dwGoldBank, sizeof(DWORD) * 3 );

	pMover->PeriodTick();
	// 임시 변수에 모든 정보를 복사한다.
	// 이것은 락이 없는 상태에서 쿼리를 실행하기 위해서이다
	mover.Copy( pMover, TRUE );
	pMover->Unlock();

	m_AccountCacheMgr.AddAccount( pMover->m_idPlayer, lpDBOP->AccountInfo.szAccount, bCacheHit, pCache );
	m_AddRemoveLock.Leave();

	// 지급, 삭제
	BOOL bResult	= FALSE;
#ifndef __NOLOG
	bResult
		= SendItemtoCharacter( nSlot, &mover, qry, qry1, qrylog, lpDBOP );
	bResult
		= RemoveItemtoCharacter( &mover, qry, qry1, qrylog, lpDBOP )
		|| bResult;
#endif	// __NOLOG
	bResult	= mover.RemoveItemIK3()
		|| bResult;
	/*
		- 문제 -
			임시 변수에서 지급 삭제가 이루어졌기 때문에,
			해당 사용자의 월드 서버로의 진입이 실패할 경우 자료가
			유실 될 수 있다. 따라서 이에 대한 처리가 필요하다.
		- 해결 -
			1. 지급, 삭제로 인하여 변경이 있을 경우 캐시에 재 복사한다.
			2. 지급과 삭제에 대한 쿼리부분에 대한 결과를 임의의 자료에
				저장하고 이를 메모리에서 처리한다.
			- 현재 1, 추후 2로 수정 예정
	*/
	if( bResult )
	{
		m_AddRemoveLock.Enter();
		pCache	= m_AccountCacheMgr.Find( lpDBOP->AccountInfo.szAccount );
		if( pCache )
		{
			pMover	= pCache->pMover[nSlot];
			if( pMover )
			{
				pMover->Lock();
				pMover->Copy( &mover, TRUE );
				pMover->Unlock();
				OutputDebugString( "bResult: 1" );
			}
		}
		m_AddRemoveLock.Leave();
	}
	
	PutExpLog( qrylog, &mover, 'I' );
	SendJoin( &mover, lpDBOP, dwAuthKey, dwBillingClass, szPartyName, szBankPass, dwPlayTime, dwFlyTime, nMaximumLevel, nTags, tags, szTimeGuild );
	g_dpCoreSrvr.SendMemberTime( mover.m_idPlayer, szTimeGuild );
	FreeRequest( lpDBOP );
}