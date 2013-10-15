#include "StdAfx.h"
#include "defineObj.h"
#include "dbmanager.h"
#include "dploginsrvr.h"
#include "dpcoresrvr.h"
#include "dptrans.h"
#include "..\_Network\Objects\Obj.h"
#include "misc.h"
#include "mytrace.h"
#include "definetext.h"

#include "party.h"
#include "guild.h"
extern	CGuildMng	g_GuildMng;
#include "guildwar.h"
extern	CGuildWarMng	g_GuildWarMng;

#include "dpaccountclient.h"
extern	CDPAccountClient	g_dpAccountClient;

#include "eveschool.h"

#include "serialnumber.h"

#include "post.h"

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

#if __VER >= 15 // __GUILD_HOUSE
#include "GuildHouseDBCtrl.h"
#endif // __GUILD_HOUSE

#if __VER >= 15 // __CAMPUS
#include "CampusDBCtrl.h"
#endif // __CAMPUS

#if defined( __VERIFY_PLAYER ) || defined( __PROVIDE ) || defined( __S0707_ITEM_CONV ) || defined(__RECOVER0816) || defined(__ITEM_REMOVE_LIST)
#define	MAX_QUERY_SIZE	1024 * 64
#ifdef IDC_STATIC
#undef IDC_STATIC
#endif
#include "resource.h"
BOOL	CALLBACK	VerifyPlayerDlgProc( HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam );
#endif	// __VERIFY_PLAYER OR __PROVIDE

extern  CDPCoreSrvr			g_dpCoreSrvr;
extern	CDPLoginSrvr		g_dpLoginSrvr;
extern	CProject			prj;
extern	APP_INFO			g_appInfo;
extern	CMyTrace			g_MyTrace;

#define	VERIFYSTRING( lpString, lpszPlayer )	\
		if( FALSE == VerifyString( lpString, __FILE__, __LINE__, lpszPlayer, lpDbOverlappedPlus ) )		return;

#define	VERIFY_GUILD_STRING( lpString, lpszGuild )	\
if( FALSE == VerifyString( lpString, __FILE__, __LINE__, lpszGuild ) )		return;

#if __VER < 11 // __SYS_PLAYER_DATA
CMclCritSec	s_cs;
#endif	// __SYS_PLAYER_DATA
CDbManager & g_DbManager = CDbManager::GetInstance();							// CDbManager 클래스 생성

// MAX_TASKBAR 300일경우 MAX_APPLETTASKBAR = 5400, MAX_ITEMTASKBAR = 10800
// MAX_TASKBAR 172일경우 MAX_APPLETTASKBAR = 3096, MAX_ITEMTASKBAR = 6192
extern const int MAX_TASKBAR; 
extern const int MAX_APPLETTASKBAR; 
extern const int MAX_ITEMTASKBAR;   
extern const char NullStr[2];

#ifdef __TOOMANY_PENDINGLOG1115
	LONG	CDbManager::m_lPending	= 0;
#endif	// __TOOMANY_PENDINGLOG1115

CDbManager & CDbManager::GetInstance()
{
	static CDbManager DbManager;
	return DbManager;
}

CDbManager::CDbManager()
{
	memset( (void*)m_aszJoin, 0, sizeof(char) * MAX_JOIN_SIZE * MAX_PLAYER );
	m_nJoin	= 0;

	m_idPlayer	= 0;
#ifndef __TRANS_0413
	//	mulcom	BEGIN100415	DB_OVERLAPPED_PLUS 메모리 풀 사용.
	m_pDbIOData		= new MemPooler<DB_OVERLAPPED_PLUS>( 1024 );
	//	mulcom	END100415	DB_OVERLAPPED_PLUS 메모리 풀 사용.
#endif	// __TRANS_0413


#ifdef __S1108_BACK_END_SYSTEM
	m_hWorker	= m_hCloseWorker	= NULL;
#endif // __S1108_BACK_END_SYSTEM

	m_hItemUpdateWorker = m_hItemUpdateCloseWorker = NULL;
	m_nItemUpdate = 0;
#ifdef __ITEM_REMOVE_LIST
	m_RemoveItem_List.clear();
#endif // __ITEM_REMOVE_LIST
	memset( m_apQuery, 0, sizeof(m_apQuery) );

	DB_ADMIN_PASS_LOG[0] = '\0';
	DB_ADMIN_PASS_CHARACTER01[0] = '\0';
	DB_ADMIN_PASS_BACKSYSTEM[0] = '\0';
	DB_ADMIN_PASS_ITEMUPDATE[0] = '\0';
	m_cbTrade	= 0;
}

CDbManager::~CDbManager()
{
	Clear();	
}



void CDbManager::Clear( void )
{
	CloseDbWorkers();

	CMclAutoLock Lock( m_AddRemoveLock );

	m_AccountCacheMgr.Clear();

#ifndef __TRANS_0413
	SAFE_DELETE( m_pDbIOData );
#endif	// __TRANS_0413
}

void CDbManager::CreatePlayer( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	
	arRead.ReadString( lpDbOverlappedPlus->AccountInfo.szAccount, MAX_ACCOUNT );
	arRead.ReadString( lpDbOverlappedPlus->AccountInfo.szPassword, MAX_PASSWORD );

	BYTE nSlot, nFace, nCostume, nSkinSet, nHairMesh;
	DWORD dwHairColor;
	BYTE nSex, nJob, nHeadMesh;
#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
	int nBankPW = 0;
#endif // __2ND_PASSWORD_SYSTEM

	//	 CHARACTER_STR 'I1',@im_idPlayer,@iserverindex,@iaccount,@im_szName,@iplayerslot,@idwWorldID,@im_dwIndex,@im_vPos_x,@im_vPos_y,@im_vPos_z,
	//	 @im_szCharacterKey,@im_dwSkinSet,@im_dwHairMesh,@im_dwHairColor,@im_dwHeadMesh,@im_dwSex
	// 	 CHARACTER_STR 'I1','','01','beat','샛별공주',0,0,0,0,0,0,'',0,0,0,0,0
	arRead >> nSlot;
	arRead.ReadString( lpDbOverlappedPlus->AccountInfo.szPlayer, MAX_PLAYER );

#ifdef __RULE_0615
	// 해킹이므로 무시
	if( prj.IsInvalidName( lpDbOverlappedPlus->AccountInfo.szPlayer ) || prj.IsAllowedLetter( lpDbOverlappedPlus->AccountInfo.szPlayer ) == FALSE )
	{
		FreeRequest( lpDbOverlappedPlus );
		return;
	}
	prj.Formalize( lpDbOverlappedPlus->AccountInfo.szPlayer );
#endif	// __RULE_0615

	arRead >> nFace >> nCostume >> nSkinSet >> nHairMesh;
	arRead >> dwHairColor;
	arRead >> nSex >> nJob >> nHeadMesh;
#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
	arRead >> nBankPW;
#endif // __2ND_PASSWORD_SYSTEM

	DWORD dwAuthKey;
	arRead >> dwAuthKey;
	DWORD dwIndex	= ( nSex == SEX_FEMALE ? MI_FEMALE : MI_MALE );
	D3DXVECTOR3	vPos;
	DWORD dwWorldID	= WI_WORLD_MADRIGAL;
	if( !g_appInfo.dwSys )
		dwWorldID	= WI_WORLD_EVENT01;

	if( prj.GetRandomBeginPos( dwWorldID, &vPos ) == FALSE ) {
		ASSERT( 0 );
	}

	if( nSex != SEX_FEMALE && nSex != SEX_MALE )
	{
		FreeRequest( lpDbOverlappedPlus );
		return;
	}

#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
	if( nBankPW < 0 || nBankPW > 9999 )
	{
		Error( "nBankPW is Invalid! szPlayer : %s, nBankPW : %d", lpDbOverlappedPlus->AccountInfo.szPlayer, nBankPW );
		FreeRequest( lpDbOverlappedPlus );
		return;
	}
#endif // __2ND_PASSWORD_SYSTEM

	char szQuery[QUERY_SIZE]	= { 0,};
	DBQryCharacter( szQuery, "I1", 0, g_appInfo.dwSys, lpDbOverlappedPlus->AccountInfo.szAccount, lpDbOverlappedPlus->AccountInfo.szPlayer, nSlot, dwWorldID,
		dwIndex, vPos.x, vPos.y, vPos.z, '\0', nSkinSet, nHairMesh,	dwHairColor, nHeadMesh, nSex );

	if( FALSE == qry->Exec( szQuery ) )
	{
		WriteLog( "%s, %d\r\n\t%s", __FILE__, __LINE__, szQuery );
		FreeRequest( lpDbOverlappedPlus );
		return;
	}

#ifdef __Chogiha
	// 초기화 작업할때 쓰인다~~~~ 평상시 안씀
	WriteToFile(nSkinSet, nHairMesh, nSex, nJob, nHeadMesh, dwHairColor, lpDbOverlappedPlus->AccountInfo.szPlayer );
#endif

	if( qry->Fetch() )
	{
		int nError;
		u_long nidPlayer;
		nError = qry->GetInt( "fError" );
		if( nError == 1 )
		{
			nidPlayer = qry->GetInt( "m_idPlayer" );	
#if __VER >= 11 // __SYS_PLAYER_DATA
			PlayerData pd;
//			pd.nJob	= 0;
//			pd.dwState	= 0;
			pd.data.nLevel	= 1;
			pd.data.nSex	= nSex;
			pd.data.nVer	= 1;
			lstrcpy( pd.szPlayer, lpDbOverlappedPlus->AccountInfo.szPlayer );
			PlayerData* pPlayerData		= CPlayerDataCenter::GetInstance()->AddPlayerData( nidPlayer, pd );
#else	// __SYS_PLAYER_DATA
			s_cs.Enter();
			m_ulong2str.insert( map<u_long, string>::value_type( nidPlayer, lpDbOverlappedPlus->AccountInfo.szPlayer ) );
			s_cs.Leave();
#endif	// __SYS_PLAYER_DATA
			m_AddRemoveLock.Enter();
			m_AccountCacheMgr.AddMover( nidPlayer, lpDbOverlappedPlus->AccountInfo.szAccount, nSlot );
			m_AddRemoveLock.Leave();

#if __VER >= 11 // __SYS_PLAYER_DATA
			CDPTrans::GetInstance()->SendAddPlayerData( nidPlayer, pPlayerData );
#else	// __SYS_PLAYER_DATA
			CDPTrans::GetInstance()->SendPlayerID( nidPlayer, lpDbOverlappedPlus->AccountInfo.szPlayer );
#endif	// __SYS_PLAYER_DATA

			g_dpLoginSrvr.SendPlayerList( lpDbOverlappedPlus->AccountInfo.szAccount, lpDbOverlappedPlus->AccountInfo.szPassword, lpDbOverlappedPlus->dpid, dwAuthKey );
#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
			char szQuery[QUERY_SIZE]	= { 0,};
			sprintf( szQuery, "BANK_STR 'U', '%07d', '%02d', '%04d'", nidPlayer, g_appInfo.dwSys, nBankPW );
			
			if( FALSE == qry->Exec( szQuery ) )
			{
				WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );
				FreeRequest( lpDbOverlappedPlus );
				return;
			}
#endif // __2ND_PASSWORD_SYSTEM
		}
		else
		{
			// 캐릭터 중복
			g_dpLoginSrvr.SendError( ERROR_USER_EXISTS, lpDbOverlappedPlus->dpid );
		}
	}
	FreeRequest( lpDbOverlappedPlus );
}

// 캐릭터 삭제 
void CDbManager::RemovePlayer( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	
	u_long idPlayer;
	char szNo[255] = {0,};
	arRead.ReadString( lpDbOverlappedPlus->AccountInfo.szAccount, MAX_ACCOUNT );
	arRead.ReadString( lpDbOverlappedPlus->AccountInfo.szPassword, MAX_PASSWORD );
	arRead.ReadString( szNo, 255 );

	arRead >> idPlayer;
	DWORD dwAuthKey;
	arRead >> dwAuthKey;

#ifdef __RT_1025
	size_t nSize;	u_long uMessengerId;
	vector<u_long> vecMessenger;
	arRead >> nSize;
	for( size_t i=0; i<nSize; i++ )
	{
		arRead >> uMessengerId;
		vecMessenger.push_back( uMessengerId );
	}
#endif // __RT_1025

#ifdef __REMOVE_PLAYER_0221
	arRead >> lpDbOverlappedPlus->dpid;
#endif	// __REMOVE_PLAYER_0221

	// CHARACTER_STR 'D1',@im_idPlayer,@iserverrindex,@iaccount (isblock)
	// CHARACTER_STR 'D1','000001','01','D'
	char szQuery[QUERY_SIZE]	= { 0,};
	DBQryCharacter( szQuery, "D1", idPlayer, g_appInfo.dwSys, lpDbOverlappedPlus->AccountInfo.szAccount, szNo);

	if( FALSE == qry->Exec( szQuery ) )
	{
		WriteLog( "%s, %d\t%s\r\n\t%s, %s", __FILE__, __LINE__, szQuery, lpDbOverlappedPlus->AccountInfo.szAccount, lpDbOverlappedPlus->AccountInfo.szPassword );
		FreeRequest( lpDbOverlappedPlus );
		return;
	}

	if( qry->Fetch() )
	{
		int nError = 0;
		nError = qry->GetInt( "fError" );
		switch( nError )
		{
			case 1:	// 주민번호틀림
				g_dpLoginSrvr.SendError( ERROR_NO_SUCH_GROUP, lpDbOverlappedPlus->dpid );
				g_DbManager.FreeRequest( lpDbOverlappedPlus );
				return;
			case 2:	// 계정없음
				g_DbManager.FreeRequest( lpDbOverlappedPlus );
				return;
			case 3:	// 길드전 참가
				{
					g_dpLoginSrvr.SendError( ERROR_WARTIME, lpDbOverlappedPlus->dpid );
					FreeRequest( lpDbOverlappedPlus );
					return;
				}
			case 4:	// 길드 소속
				{					
					u_long idGuild	= qry->GetInt( "fText" );
					g_dpCoreSrvr.SendDelPlayer( idPlayer, idGuild );
					break;
				}
		}
	}
	else
	{
		FreeRequest( lpDbOverlappedPlus );
		return;
	}

#if __VER >= 15 // __CAMPUS
	CCampus* pCampus = CCampusHelper::GetInstance()->GetCampus( CCampusHelper::GetInstance()->GetCampusIdByPlayerId( idPlayer ) );
	if( pCampus && pCampus->IsMember( idPlayer ) )
	{
		CAr ar;
		ar << pCampus->GetCampusId() << idPlayer;
		int nBufSize;
		LPBYTE lpBuf = ar.GetBuffer( &nBufSize );
		CCampusHelper::GetInstance()->PostRequest( CAMPUS_REMOVE_MEMBER, lpBuf, nBufSize );
	}
#endif // __CAMPUS

#if __VER >= 11 // __SYS_PLAYER_DATA
	CPlayerDataCenter::GetInstance()->DeletePlayerData( idPlayer );
#else	// __SYS_PLAYER_DATA
	s_cs.Enter();
	m_ulong2str.erase( idPlayer );
	s_cs.Leave();
#endif	// __SYS_PLAYER_DATA
	m_AddRemoveLock.Enter();
	m_AccountCacheMgr.RemoveMover( idPlayer, lpDbOverlappedPlus->AccountInfo.szAccount );
	m_AddRemoveLock.Leave();

#if __VER >= 11 // __SYS_PLAYER_DATA
	CDPTrans::GetInstance()->SendDeletePlayerData( idPlayer );
#else	// __SYS_PLAYER_ID
	CDPTrans::GetInstance()->SendRemovePlayerID( idPlayer );
#endif	// __SYS_PLAYER_ID
#ifdef __RT_1025
	for( vector<u_long>::iterator it=vecMessenger.begin(); it!=vecMessenger.end(); it++ )
	{
		qry->Execute( "uspDeleteMessenger '%02d', '%07d', '%07d'", g_appInfo.dwSys, idPlayer, (*it) );
		g_dpCoreSrvr.SendRemovePlayerFriend( idPlayer, (*it) );
	}
#endif // __RT_1025
	g_dpLoginSrvr.SendPlayerList( lpDbOverlappedPlus->AccountInfo.szAccount, lpDbOverlappedPlus->AccountInfo.szPassword, lpDbOverlappedPlus->dpid, dwAuthKey );
	FreeRequest( lpDbOverlappedPlus );
}


void CDbManager::ChangeBankPass( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	
	char szName[33];
	char szNewPass[5];
	u_long uidPlayer;
	arRead.ReadString( szName, 33 );
	arRead.ReadString( szNewPass, 5 );
	arRead >> uidPlayer;

	//BANK_STR 'U','000001','01','1234'
	char szQuery[QUERY_SIZE]	= { 0,};
	sprintf( szQuery,
		"BANK_STR 'U', '%07d', '%02d', '%s'", uidPlayer, g_appInfo.dwSys, szNewPass );

	if( FALSE == qry->Exec( szQuery ) )
	{
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );
		FreeRequest( lpDbOverlappedPlus );
		return;
	}
	FreeRequest( lpDbOverlappedPlus );
}

// 플레이어 리스트를 보냄(한계정에 있는 모든 플레이어)
void CDbManager::SendPlayerList( CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	BEFORESENDSOLE( ar, PACKETTYPE_PLAYER_LIST, lpDbOverlappedPlus->dpid );
	ar << lpDbOverlappedPlus->dwAuthKey;	
	char szQuery[QUERY_SIZE]	= { 0,};
	DBQryCharacter( szQuery, "S2", 0, g_appInfo.dwSys, lpDbOverlappedPlus->AccountInfo.szAccount, lpDbOverlappedPlus->AccountInfo.szPassword );

	if( FALSE == qry->Exec( szQuery ) )
	{
		WriteLog( "%s, %d\t%s\r\n\t%s, %s", __FILE__, __LINE__, szQuery, lpDbOverlappedPlus->AccountInfo.szAccount, lpDbOverlappedPlus->AccountInfo.szPassword );
		FreeRequest( lpDbOverlappedPlus );
		return;
	}

	u_long ulOffSet = ar.GetOffset();
	ar << (INT)0;
	
	int countPlayer = 0;
	u_long uidPlayer[5];
	BYTE nslot[5];
	BYTE MessengerSlot[5];
	DWORD	dwMessengerState[5];
	int countMessenger = 0;

	DWORD	adwWorldId[3];
	D3DXVECTOR3	avPos[3];
	memset( adwWorldId, 0, sizeof(adwWorldId) );
	ACCOUNT_CACHE* pAccount		= NULL;
	g_DbManager.m_AddRemoveLock.Enter();
	{
		pAccount = m_AccountCacheMgr.Find( lpDbOverlappedPlus->AccountInfo.szAccount );
		if( pAccount )
		{
			for( int i = 0; i < 3; i++ )
			{
				if( pAccount->pMover[i] )
				{
					adwWorldId[i] = pAccount->pMover[i]->m_dwWorldID;
					avPos[i]      = pAccount->pMover[i]->m_vPos;
				}
			}
		}
	}
	g_DbManager.m_AddRemoveLock.Leave();
	
	//	mulcom	BEGIN100218	S2 Patch
	int	nWhileCount	= 0;
	//	mulcom	END100218	S2 Patch

	while( qry->Fetch() )
	{

		//	mulcom	BEGIN100218	S2 Patch
		nWhileCount++;

		if( nWhileCount > 3 )
		{
			break;
		}

		if( countPlayer > 2 )
		{
			WriteLog( "%s, %d\t%s : countPlayer > 2", __FILE__, __LINE__, lpDbOverlappedPlus->AccountInfo.szAccount );

			break;
		}
		//	mulcom	END100218	S2 Patch


		int nError = 0;
		nError = qry->GetInt( "fError" );
		if( nError == 1 || nError == 2 )	// 1 : 암호틀림 2 : 계정없음
		{
			WriteLog( "%s, %d\t%s, %s, %d", __FILE__, __LINE__, lpDbOverlappedPlus->AccountInfo.szAccount, lpDbOverlappedPlus->AccountInfo.szPassword, nError );
//			qry->Clear();
			FreeRequest( lpDbOverlappedPlus );
			return;
		}

		int islot = qry->GetInt("playerslot");

		//	mulcom	BEGIN100218	S2 Patch
		if( islot < 0 || islot > 2 )
		{
			WriteLog( "%s, %d\t%s SlotError islot [%d]", __FILE__, __LINE__, lpDbOverlappedPlus->AccountInfo.szAccount, islot );
			
			continue;
		}

		for( int nSlotCount = 0; nSlotCount<countPlayer; nSlotCount++ )
		{
			if( nslot[nSlotCount] == islot )
			{
				WriteLog( "%s, %d\t%s nslot[nSlotCount] == islot", __FILE__, __LINE__, lpDbOverlappedPlus->AccountInfo.szAccount );
				
				continue;
			}
		}
		//	mulcom	END100218	S2 Patch

		nslot[countPlayer] = (BYTE)islot;


		CMover mover;
		mover.InitProp();

		DWORD dwWorldID;
		int nBlock = 0;

		BOOL bRefresh	= qry->GetInt( "m_idCompany" ) > 0;

		if( adwWorldId[islot] > 0 
			&& bRefresh == FALSE
			)
		{
			dwWorldID	= adwWorldId[islot];
			mover.m_vPos	= avPos[islot];
		}
		else
		{
			dwWorldID = qry->GetInt( "dwWorldID" );
			mover.m_vPos.x = qry->GetFloat( "m_vPos_x");
			mover.m_vPos.y = qry->GetFloat( "m_vPos_y");
			mover.m_vPos.z = qry->GetFloat( "m_vPos_z");
		}

		mover.m_dwMode	= 0;
		if( qry->GetInt( "last_connect" ) == 3 )
			mover.m_dwMode	= EVENT_OLDBOY_MODE;

		countPlayer++;

		qry->GetStr( "m_szName", mover.m_szName);

		char strEnd_Time[13] = {0,};
		qry->GetStr("BlockTime", strEnd_Time );
		CTime tEnd_Time;

		GetStrTime(&tEnd_Time, strEnd_Time);

		CTime NowTime = CTime::GetCurrentTime();
		if( NowTime > tEnd_Time )
		{
			// 접속이 가능한 상태
			nBlock = 1;
		}

		{
			mover.m_dwIndex = (DWORD)qry->GetInt("m_dwIndex");
			mover.m_idPlayer	= (DWORD)qry->GetInt( "m_idPlayer" );
			mover.m_idparty = (DWORD)qry->GetInt("m_idparty");
			mover.m_idGuild		= (DWORD)qry->GetInt( "m_idGuild" );
			mover.m_idWar	= (DWORD)qry->GetInt( "m_idWar" );

			mover.m_dwSkinSet = (DWORD)qry->GetInt("m_dwSkinSet");
			mover.m_dwHairMesh = (DWORD)qry->GetInt("m_dwHairMesh");
			mover.m_dwHeadMesh = (DWORD)qry->GetInt("m_dwHeadMesh");
			mover.m_dwHairColor = (DWORD)qry->GetInt("m_dwHairColor");
			mover.SetSex( (BYTE) qry->GetInt("m_dwSex") );
			mover.m_nJob = (LONG)qry->GetInt("m_nJob");
			mover.m_nLevel = (LONG)qry->GetInt("m_nLevel");
			mover.m_nStr = qry->GetInt( "m_nStr");
			mover.m_nSta = qry->GetInt( "m_nSta");
			mover.m_nDex = qry->GetInt( "m_nDex");
			mover.m_nInt = qry->GetInt( "m_nInt");
#if __VER < 8 // __S8_PK
			mover.m_nSlaughter = qry->GetInt( "m_nSlaughter");
#endif // __VER < 8 // __S8_PK

			int CountStr	= 0;
			int IndexItem		= 0;
			char Inven[8024]	= { 0, };
			qry->GetStr( "m_Inventory", Inven );
			VERIFYSTRING( Inven, mover.m_szName );
			while( '$' != Inven[CountStr] )
			{
				CItemElem BufItemElem;
				IndexItem = GetOneItem( &BufItemElem, Inven, &CountStr );
				if( IndexItem == -1 )
				{
					Error( "Inventory : << 프로퍼티 없음. %s, %d", mover.m_szName, BufItemElem.m_dwItemId );
				}
				else
				{
					if( IndexItem >= MAX_INVENTORY + MAX_HUMAN_PARTS )
					{
						Error( "SendPlayerList::Inventory : << IndexItem %s, %d", mover.m_szName, IndexItem );
						Error( "Inventory = %s", Inven );
						FreeRequest( lpDbOverlappedPlus );
						return;
					}
					mover.m_Inventory.m_apItem[IndexItem] = BufItemElem;
				}

			}
			CountStr	= 0;
			int iIndex	= 0;
			char sIndex[512]	= { 0, };
			qry->GetStr( "m_apIndex", sIndex );
			VERIFYSTRING( sIndex, mover.m_szName );
			while( '$' != sIndex[CountStr] )
			{
				mover.m_Inventory.m_apIndex[iIndex]	= (DWORD)GetIntFromStr( sIndex, &CountStr );
				iIndex++;
			}

			CountStr	= 0;
			int IndexEquipment	= 0;
			char Equipment[500]	= {0,};
			qry->GetStr( "m_adwEquipment", Equipment );
			VERIFYSTRING( Equipment, mover.m_szName );
			while( '$' != Equipment[CountStr] )
			{
				if(IndexEquipment < MAX_HUMAN_PARTS)
					mover.m_aEquipInfo[IndexEquipment].dwId		= (DWORD)GetIntFromStr(Equipment, &CountStr);
				IndexEquipment++;
			}

			CAr arbuf;

			arbuf << islot;
			arbuf << nBlock;
			arbuf << dwWorldID;
			arbuf << mover.m_dwIndex;
			arbuf.WriteString( mover.m_szName );
			arbuf << mover.m_vPos;
			arbuf << mover.m_idPlayer;
			arbuf << mover.m_idparty;
			arbuf << mover.m_idGuild;
			arbuf << mover.m_idWar;
			arbuf << mover.m_dwSkinSet;
			arbuf << mover.m_dwHairMesh;
			arbuf << mover.m_dwHairColor;
			arbuf << mover.m_dwHeadMesh;
			arbuf << mover.GetSex();
			arbuf << mover.m_nJob;
			arbuf << mover.m_nLevel;
			arbuf << (DWORD)0;
			arbuf << mover.m_nStr;
			arbuf << mover.m_nSta;
			arbuf << mover.m_nDex;
			arbuf << mover.m_nInt;
#if __VER < 8 // __S8_PK
			arbuf << mover.m_nSlaughter;
#endif // __VER < 8 // __S8_PK
			arbuf << mover.m_dwMode;

			MessengerSlot[countMessenger] = islot;
			uidPlayer[countMessenger] = mover.m_idPlayer;
			dwMessengerState[countMessenger] = qry->GetInt( "m_nMessengerState" );
			countMessenger++;

			u_long uOffset	= arbuf.GetOffset();
			arbuf << (int)0;	// reserve
			
			int countbuf = 0;
			CItemElem* pItemElem;
			for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
			{
				pItemElem	= mover.m_Inventory.GetEquip( i );
				if( pItemElem && !pItemElem->IsFlag( CItemElem::expired ) )
				{
					ItemProp* pItemProp	= prj.GetItemProp( pItemElem->m_dwItemId );
					if( pItemProp )
					{
						if( pItemProp->dwParts == i || pItemProp->dwPartsub == i ||
							( pItemProp->dwParts == PARTS_RWEAPON && i == PARTS_LWEAPON ) )
						{
							arbuf << pItemElem->m_dwItemId;
							countbuf++;
						}
					}
				}
			}
			int nBufsize;
			BYTE* pBuf = arbuf.GetBuffer( &nBufsize );
			*( pBuf + uOffset ) = countbuf;
			ar.Write( pBuf, nBufsize );
		} 
	} // while	
	int nBufsize;
	BYTE* pBuf = ar.GetBuffer( &nBufsize );
	*( pBuf + ulOffSet ) = countPlayer;


	ar << countMessenger;
	for( int i = 0 ; i < countMessenger ; i++ )
	{
		CMover cMoverBuf;
		cMoverBuf.InitProp();
		cMoverBuf.m_idPlayer = uidPlayer[i];
#ifdef __RT_1025
		LoadMessenger( &cMoverBuf, qry );
		cMoverBuf.m_RTMessenger.SetState( dwMessengerState[i] );
#else	// __RT_1025
		GetMessengerFriend( &cMoverBuf, qry, lpDbOverlappedPlus );
		cMoverBuf.m_Messenger.m_dwMyState = dwMessengerState[i];
#endif	// __RT_1025
		ar << MessengerSlot[i];
#ifdef __RT_1025
		cMoverBuf.m_RTMessenger.Serialize( ar );
#else	// __RT_1025
		cMoverBuf.m_Messenger.Serialize( ar );
#endif	// __RT_1025
	}

	SEND( ar, &g_dpLoginSrvr, DPID_ALLPLAYERS );
	FreeRequest( lpDbOverlappedPlus );
}

BOOL CDbManager::OpenGuildCombat( void )
{
	m_nGuildCombatIndex	= 0;
	m_uWinGuildId = 0;
	m_nWinGuildCount = 0;
	m_uBestPlayer = 0;
#ifdef __S_BUG_GC
	m_vecGuildCombat.clear();
#else // __S_BUG_GC
	m_GuildCombat.clear();
#endif // __S_BUG_GC
	
	CQuery qry;
	
	if( qry.Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) == FALSE )
	{
		WriteLog( "%s, %d", __FILE__, __LINE__ );
		AfxMessageBox( "Error : Not Connect Character DB" );
		return FALSE;
	}
	
	char szQuery[40960];
	// 인덱스 얻어오기
	sprintf( szQuery, "uspLoadMaxCombatID '%02d'", g_appInfo.dwSys );
	if( FALSE == qry.Exec( szQuery ) )
	{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return FALSE; }
	
	if( qry.Fetch() )
	{
		m_nGuildCombatIndex = qry.GetInt( "MaxNum" );
		
		// 가지고온 컴벳 인덱스가 종료한 인덱스 인지 확인
		sprintf( szQuery, "uspLoadCombatInfo %d, '%02d'", m_nGuildCombatIndex, g_appInfo.dwSys );
		if( FALSE == qry.Exec( szQuery ) )
		{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return FALSE; }
		
		if( qry.Fetch() )
		{
			int nClose = qry.GetInt( "Status" );
			
			if( nClose == 30 || nClose == -100 ) // 길드대전 종료
			{
				++m_nGuildCombatIndex;
				sprintf( szQuery, "uspAddNewCombat %d,'%02d'", m_nGuildCombatIndex, g_appInfo.dwSys );
				if( FALSE == qry.Exec( szQuery ) )
				{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return FALSE; }
			}
		}
		else
		{
			sprintf( szQuery, "uspAddNewCombat %d,'%02d'", m_nGuildCombatIndex, g_appInfo.dwSys );
			if( FALSE == qry.Exec( szQuery ) )
			{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return FALSE; }
		}
		
		// 최종 승리길드 및 연승 얻어오기
		sprintf( szQuery, "uspLoadWinnerGuildInfo %d,'%02d'", m_nGuildCombatIndex-1, g_appInfo.dwSys );
		if( FALSE == qry.Exec( szQuery ) )
		{	WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );	return FALSE; }
		if( qry.Fetch() )
		{
			m_uWinGuildId = qry.GetInt( "GuildID" );
			m_nWinGuildCount = qry.GetInt( "StraightWin" );
		}

		// 최종 베스트 플레이어 얻어오기
		sprintf( szQuery, "uspLoadCombatBestPlayer %d,'%02d'", m_nGuildCombatIndex-1, g_appInfo.dwSys );
		if( FALSE == qry.Exec( szQuery ) )
		{	WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );	return FALSE; }
		if( qry.Fetch() )
		{
			m_uBestPlayer = qry.GetInt( "PlayerID" );
		}
		
		// 참가길드 얻어오기
		sprintf( szQuery, "uspLoadCombatGuildList %d, '%02d'", m_nGuildCombatIndex, g_appInfo.dwSys );
		if( FALSE == qry.Exec( szQuery ) )
		{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return FALSE; }
		
		while( qry.Fetch() )
		{
			u_long GuildId	= (u_long)( qry.GetInt( "GuildID" ) );
			DWORD dwPenya	= (DWORD)( qry.GetInt64( "CombatFee" ) );
			int nStatus		= qry.GetInt( "Status" );
			__GUILDCOMBATJOIN GCJoin;
			GCJoin.dwPenya = dwPenya;
			if( nStatus == 10 || nStatus == 12 || nStatus == 20 )
				GCJoin.bRequest = TRUE;
			else
				GCJoin.bRequest = FALSE;
#ifdef __S_BUG_GC
			GCJoin.uGuildId = GuildId;
			m_vecGuildCombat.push_back( GCJoin);
#else // __S_BUG_GC
			m_GuildCombat.insert( map<u_long, __GUILDCOMBATJOIN>::value_type(GuildId, GCJoin) );
#endif // __S_BUG_GC
		}
		
		// 지금 까지 보상 및 참여금을 안받은 길드 및 베스트 플레이어 얻어오기
		m_GCResultValueGuild.clear();
		m_GCResultValuePlayer.clear();
		sprintf( szQuery, "uspLoadCombatUnpaidList '%02d'", g_appInfo.dwSys );
		if( FALSE == qry.Exec( szQuery ) )
		{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return FALSE; }
		while( qry.Fetch() )
		{
			char cFlag;
			cFlag = qry.GetChar( "Flag" );
			if( cFlag == 'G' )	// 길드
			{
				__GCRESULTVALUEGUILD ResultValueGuild;
				ResultValueGuild.nCombatID = qry.GetInt( "CombatID" );
				ResultValueGuild.uidGuild = qry.GetInt( "GuildID" );
				ResultValueGuild.nReturnCombatFee = qry.GetInt64( "ReturnCombatFee" );
				ResultValueGuild.nReward = qry.GetInt64( "Reward" );
				m_GCResultValueGuild.push_back( ResultValueGuild );
			}
			else				// 플레이어
			{
				__GCRESULTVALUEPLAYER ResultValuePlayer;
				ResultValuePlayer.nCombatID = qry.GetInt( "CombatID" );
				ResultValuePlayer.uidGuild = qry.GetInt( "GuildID" );
				ResultValuePlayer.uidPlayer = qry.GetInt( "PlayerID" );
				ResultValuePlayer.nReward = qry.GetInt64( "Reward" );
				m_GCResultValuePlayer.push_back( ResultValuePlayer );
			}
		}

		m_vecGCPlayerPoint.clear();
		sprintf( szQuery, "uspRankGuildCombatPlayer '%02d'", g_appInfo.dwSys );
		if( FALSE == qry.Exec( szQuery ) )
		{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return FALSE; }
		while( qry.Fetch() )
		{
			__GCPLAYERPOINT GCPlayerPoint;
			GCPlayerPoint.uidPlayer = qry.GetInt( "PlayerID" );
			GCPlayerPoint.nJob = qry.GetInt( "Job" );
			GCPlayerPoint.nPoint = qry.GetInt( "PointSummary" );
			m_vecGCPlayerPoint.push_back( GCPlayerPoint );
		}
	}
	else
	{
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );
	}
	return TRUE;
}

#if __VER >= 11 // __SYS_PLAYER_DATA
BOOL	CDbManager::LoadPlayerData( void )
{
	CQuery	query;
	if( query.Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) == FALSE )
	{
		Error( "LoadPlayerData: can't connect character db" );
		AfxMessageBox( "can't connect character db" );
	}
	char szQuery[100]	= { 0,};

	sprintf( szQuery, "uspLoadPlayerData '%02d'", g_appInfo.dwSys );
	if( !query.Exec( szQuery ) )
	{
		Error( "LoadPlayerData - query: %s", szQuery );
		return FALSE;
	}

	PlayerData	pd;
	u_long idPlayer		= 0;
	while( query.Fetch() )
	{
		idPlayer	= query.GetInt( "m_idPlayer" );
		query.GetStr( "m_szName", pd.szPlayer );
		pd.data.nSex	= (int)query.GetInt( "m_dwSex" );
		pd.data.nJob	= (int)query.GetInt( "m_nJob" );
		pd.data.nLevel	= (int)query.GetInt( "m_nLevel" );
//		pd.data.dwState	= (DWORD)query.GetInt( "m_nMessengerState" );
		pd.data.nVer		= 1;	//
		pd.data.uLogin	= 0;
		PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->AddPlayerData( idPlayer, pd );
		ASSERT( pPlayerData );
	}
	m_idPlayer	= idPlayer;
	return TRUE;
}
#else	// 
BOOL CDbManager::OpenPlayerID( void )
{
//{{AFX
	// CHARACTER_STR 'S3','',@iserverindex
	CQuery qry;

	if( qry.Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) == FALSE )
	{
		WriteLog( "%s, %d", __FILE__, __LINE__ );
		AfxMessageBox( "Error : Not Connect Character DB" );
		return FALSE;
	}
	char szQuery[40960];

	char szPlayer[36];
	u_long id	=0;
	
	DBQryCharacter( szQuery, "S3", 0, g_appInfo.dwSys );
	
	if( FALSE == qry.Exec( szQuery ) )
	{
		
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );
		return FALSE;
	}


	while( qry.Fetch() )
	{
		qry.GetStr( "m_szName", szPlayer );
		id	= qry.GetInt( "m_idPlayer" );
		m_ulong2str.insert( map<u_long, string>::value_type( id, szPlayer ) );
	}
	m_idPlayer	= id;
	return TRUE;
//}}AFX
}
#endif	// __SYS_PLAYER_DATA

BOOL CDbManager::GetPartyName( void )
{
	CQuery qry;
	
	if( qry.Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) == FALSE )
	{
		WriteLog( "%s, %d", __FILE__, __LINE__ );
		return FALSE;
	}
	
	char szQuery[256], szPartyName[36];
	u_long id	=0;

	
	sprintf( szQuery,
		"PARTY_STR 'S2','','%02d'",
		g_appInfo.dwSys );

	if( FALSE == qry.Exec( szQuery ) )
	{
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );
		return FALSE;
	}
	
	while( qry.Fetch() )
	{
		qry.GetStr( "partyname", szPartyName );
		id	= qry.GetInt( "m_idPlayer" );
		m_2PartyNamePtr.insert( map<u_long, string>::value_type( id, szPartyName ) );
	}
	m_idPlayer	= id;
	return TRUE;	
}

#ifndef __RT_1025
BOOL CDbManager::SetMessenger( void )
{
	CQuery qry;
	
	if( qry.Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) == FALSE )
	{
		WriteLog( "%s, %d", __FILE__, __LINE__ );
		return FALSE;
	}
	
	char szQuery[256];
	
	sprintf( szQuery,
		"MESSENGER_STR 'D1','','%02d'",
		g_appInfo.dwSys );
	
	if( FALSE == qry.Exec( szQuery ) )
	{
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );
		return FALSE;
	}
	return TRUE;	
}
#endif	// __RT_1025

BOOL CDbManager::GetMyPartyName( CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus, u_long uidPlayer, char * szPartyName )
{
	char szQuery[QUERY_SIZE]	= { 0,};
	sprintf( szQuery,
		"PARTY_STR 'S1','%07d','%02d'",
		uidPlayer, g_appInfo.dwSys );
	
	if( FALSE == qry->Exec( szQuery ) )
	{
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );
		return FALSE;
	}
	
	if( qry->Fetch() )
	{
		qry->GetStr( "partyname", szPartyName );
	}
	return TRUE;
}

//
//SP : uspLoggingLogin
//
//Parameter :
//          @pPlayerID      char(6),
//          @pserverindex   char(2),
//          @pAccount       varchar(32),
//          @pWorldID       int,
//          @pGold          bigint,
//          @pLevel         int,  -- 캐릭터 레벨
//          @pJob           int, -- 캐릭터 직업
//          @pStartDt       char(14),
//          @pTotalPlayTime int,
//          @pRemoteIP      varchar(16)
//Return : N/N


void CDbManager::LogPlayConnect( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	int		iServer_No;				// 서버 번호
	DWORD	iMap_No;				// 맵 번호
	char	cIP[16] = { 0, };		// 접속한 넘의 아이피
	DWORD	dConnectTime;
	DWORD	dSeed;					// 돈
	SYSTEMTIME	siIn;
	u_long	uidPlayer;
	char szAccount[MAX_ACCOUNT];
	int nLevel;
	int nJob;

	iServer_No	= g_appInfo.dwSys;
	arRead >> iMap_No;
	arRead.Read( &siIn, sizeof(SYSTEMTIME) );
	arRead.ReadString( cIP, 16 );
	arRead >> dSeed;
	arRead >> uidPlayer;
	arRead.ReadString( szAccount, MAX_ACCOUNT );
	arRead >> nLevel;
	arRead >> nJob;
#if __VER >= 14 // __PCBANG
	DWORD dwPCBangClass;
	arRead >> dwPCBangClass;
#endif // __PCBANG

	CTime tIn( siIn ), tOut	= CTime::GetCurrentTime();
	CTimeSpan tConnect	= tOut - tIn;
	
	CString strOut = tOut.Format( "%Y%m%d%H%M%S" );

	CString strIn	= tIn.Format( "%Y%m%d%H%M%S" );
	dConnectTime	= (DWORD)tConnect.GetTotalSeconds();
	char timebuf[15] = {0,};
	strncpy( timebuf, (LPCTSTR)strIn, 14 );
	
	char szQuery[QUERY_SIZE]	= { 0,};
	sprintf( szQuery, 
			"uspLoggingLogin '%07d', '%02d', '%s', %d, %d, %d, %d, '%s', %d, '%s'"
#if __VER >= 14 // __PCBANG
			", %d"
#endif // __PCBANG
			, uidPlayer, g_appInfo.dwSys, szAccount, iMap_No, dSeed, nLevel, nJob, timebuf, dConnectTime, cIP
#if __VER >= 14 // __PCBANG
			,dwPCBangClass
#endif // __PCBANG
			 );

	if ( FALSE == qry->Exec( szQuery ) )
	{
		WriteLog( "%s, %d\t%d, %s", __FILE__, __LINE__, uidPlayer, szQuery );
		FreeRequest( lpDbOverlappedPlus );
		return;
	}
	FreeRequest( lpDbOverlappedPlus );
}


void CDbManager::AddPartyName( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	u_long uidPlayer;
	TCHAR	sParty[33];		
	arRead >> uidPlayer;
	arRead.ReadString( sParty, 33 );

	char szQuery[QUERY_SIZE]	= { 0,};
	sprintf( szQuery,
		"PARTY_STR 'A1','%07d','%02d','%s'",
		uidPlayer, g_appInfo.dwSys, sParty );

	if( FALSE == pQuery->Exec( szQuery ) )
	{
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );
		FreeRequest( lpDbOverlappedPlus );
		return;
	}
	
	ULONG2STRING::iterator iter = m_2PartyNamePtr.find( uidPlayer );
	if( iter != m_2PartyNamePtr.end() )
		m_2PartyNamePtr.erase( iter );

	m_2PartyNamePtr.insert( map<u_long, string>::value_type( uidPlayer, sParty ) );
	FreeRequest( lpDbOverlappedPlus );
}

#ifndef __RT_1025
void CDbManager::RemoveFriend( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	
	u_long uidPlayer, uidFriend;		
	arRead >> uidPlayer >> uidFriend;

	// 친구 삭제
	/*  MESSENGER_STR 'D1',@im_idPlayer,@iserverindex,@if_idPlayer
		MESSENGER_STR 'D1','000001','01','000002' */
	char szQuery[QUERY_SIZE]	= { 0,};
	sprintf( szQuery,
		"MESSENGER_STR 'D1','%07d','%02d','%07d'",
		uidPlayer, g_appInfo.dwSys, uidFriend);

	if( FALSE == pQuery->Exec( szQuery ) )
	{
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );
		FreeRequest( lpDbOverlappedPlus );
		return;
	}
	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::GetMessengerFriend( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	pMover->m_Messenger.Clear();
	// 내가 등록한 아이디 가지고 오기
	char szQuery[QUERY_SIZE]	= { 0,};

	sprintf( szQuery,
		"MESSENGER_STR 'S1','%07d','%02d'",
		pMover->m_idPlayer, g_appInfo.dwSys );
	if( FALSE == qry->Exec( szQuery ) )
	{
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );
		return;
	}

	while( qry->Fetch() )
	{
		u_long uFriendId;
		BYTE nSex;
		LONG nJob;
		DWORD dwState;
		
		uFriendId = qry->GetInt("f_idPlayer");
		nSex = qry->GetInt( "m_dwSex" );
		nJob = qry->GetInt( "m_nJob" );
		dwState = qry->GetInt( "m_dwState" );
		pMover->m_Messenger.AddFriend( uFriendId, nJob, nSex, dwState );
	}
	// 나를 동록한 아이디 가지고 오기
	sprintf( szQuery,
		"MESSENGER_STR 'S2','%07d','%02d'",
		pMover->m_idPlayer, g_appInfo.dwSys );
	if( FALSE == qry->Exec( szQuery ) )
	{
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );
		return;
	}
	
	while( qry->Fetch() )
	{
		u_long uFriendId;
		uFriendId = qry->GetInt("f_idPlayer");
		pMover->m_Messenger.AddDefferntFriend( uFriendId );
	}
}

void CDbManager::SaveMessengerFriend( CQuery *qry, CMover* pMover, char* szQuery )
{
	for( C2FriendPtr::iterator i	= pMover->m_Messenger.m_aFriend.begin(); i != pMover->m_Messenger.m_aFriend.end(); ++i )
	{
		LPFRIEND pFriend = (LPFRIEND)i->second;
		pFriend->dwUserId;

		if( pFriend->bSave )
		{
			// 메신저 추가시 성별하고 직업은 DB에서 자체적으로 Join을 해서 성별과 직업을 가지고 옴
			sprintf( szQuery,
				"MESSENGER_STR @iGu='A1',@im_idPlayer='%07d',@iserverindex='%02d',@if_idPlayer='%07d',@im_dwState=%d", 
				pMover->m_idPlayer, g_appInfo.dwSys, pFriend->dwUserId, pFriend->dwState ); 

			if( FALSE == qry->Exec( szQuery ) )
			{
				WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );
				return;
			}
		}
	}
}
#endif	// __RT_1025

BOOL CDbManager::SendItemtoCharacter( int nSlot, CMover* pMover, CQuery *qry, CQuery *qry1, CQuery *qrylog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
		// CHARACTER_STR 'S4',@im_idPlayer,@iserverindex
		char szQuery[QUERY_SIZE]	= { 0,};
		DBQryCharacter( szQuery, "S4", pMover->m_idPlayer, g_appInfo.dwSys );

		if( FALSE == qry->Exec( szQuery ) )
		{
			WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );
			return FALSE;
		}

		BOOL bResult	= FALSE;
		while( qry->Fetch() )
		{
			__SendItemContents SendItemContents;
#if __VER >= 12 // __EXT_PIERCING
			CItemElem* pItemElem = &SendItemContents.itemElem;
			pItemElem->m_bCharged = TRUE;
			pItemElem->SetRandomOptItemId( 0 );
#else // __EXT_PIERCING
			SendItemContents.nCharged = 1;
#if __VER >= 11 // __SYS_IDENTIFY
			SendItemContents.iRandomOptItemId	= 0;
#else	// __SYS_IDENTIFY
			SendItemContents.nRandomOptItemId	= 0;
#endif	// __SYS_IDENTIFY
#endif // __EXT_PIERCING

			if( GetSendItem( qry, &SendItemContents ) == FALSE )
			{
#if __VER >= 12 // __EXT_PIERCING
				WriteLog( "SendItem::GetSendItem PlayerName = %s, Item_Count = %d Qry = %s", pMover->m_szName, SendItemContents.Item_Count, szQuery );
#endif // __EXT_PIERCING
				continue;
			}

			if( 0 == strcmp( SendItemContents.OneItem, "penya" ) )				// 페냐인지 확인
			{
				// Penya 지급
				if( SendPenya( qry1, szQuery, pMover, SendItemContents.Item_Count, SendItemContents.nNo ) == FALSE )
				{
					WriteLog( "SendItem::SendPenya PlayerName = %s, Item_Count = %d Qry = %s", pMover->m_szName, SendItemContents.Item_Count, szQuery );
					continue;
				}

				bResult		= TRUE;
				
#ifndef __NOLOG
				LogItemInfo aLogItem;
				aLogItem.SendName = pMover->m_szName;
				aLogItem.RecvName = "DB";
				aLogItem.Gold = pMover->GetGold() - SendItemContents.Item_Count;
				aLogItem.Gold2 = pMover->GetGold();
				//aLogItem.ItemName = "SEED";
				_stprintf( aLogItem.szItemName, "%d", II_GOLD_SEED1 );
				aLogItem.itemNumber = SendItemContents.Item_Count;
				aLogItem.Action = "U";
				if( ItemLogQuery( qrylog, "LC", aLogItem, pMover->m_idPlayer, SendItemContents.nNo, SendItemContents.OneItem ) == FALSE )
				{
//					return;
					continue;
				}
#endif // __NOLOG
				
			}
			else	// 아이템 지급
			{
				SERIALNUMBER iSerialNumber = SendItem( qry1, szQuery, pMover, SendItemContents );
				if( iSerialNumber != 0 )
				{
					bResult		= TRUE;
#ifndef __NOLOG
					LogItemInfo aLogItem;
					aLogItem.SendName = pMover->m_szName;
					aLogItem.RecvName = "DB";
					aLogItem.Gold = pMover->GetGold();
					aLogItem.Gold2 = pMover->GetGold();
					//aLogItem.ItemName = SendItemContents.OneItem;
					_stprintf( aLogItem.szItemName, "%d", SendItemContents.itemElem.m_dwItemId );
					aLogItem.itemNumber = SendItemContents.Item_Count;
					aLogItem.ItemNo = iSerialNumber;
#if __VER >= 12 // __EXT_PIERCING
					aLogItem.nAbilityOption = pItemElem->GetOption();
					aLogItem.nItemResist = pItemElem->m_bItemResist;
					aLogItem.nResistAbilityOption = pItemElem->m_nResistAbilityOption;
#else // __EXT_PIERCING
					aLogItem.nAbilityOption = SendItemContents.m_nAbilityOption;
					aLogItem.nItemResist = SendItemContents.nItemResist;
					aLogItem.nResistAbilityOption = SendItemContents.nResistAbilityOption;
#endif // __EXT_PIERCING
					aLogItem.Action = "U";
					if( ItemLogQuery( qrylog, "LC", aLogItem, pMover->m_idPlayer, SendItemContents.nNo, SendItemContents.OneItem ) == FALSE )
					{
//						return;
						continue;
					}
#endif // __NOLOG
				}
				else
				{
//					return;
					continue;
				}
			}


		}
//		else
//		{
//			return;
//		}
//	}
	return bResult;
}

BOOL CDbManager::GetSendItem( CQuery *pQry, __SendItemContents * pSendItemContents )
{
	pSendItemContents->nNo = pQry->GetInt( "m_nNo" );										// 유니크 넘버
	pQry->GetStr( "Item_Name", pSendItemContents->OneItem );								// 이름
	
	pSendItemContents->Item_Count				= pQry->GetInt( "Item_count" );				// 갯수
#if __VER >= 12 // __EXT_PIERCING
	int nIndex = atoi( pSendItemContents->OneItem );
	ItemProp* pItemProp = NULL;
	if( nIndex > 0 )
		pItemProp = prj.GetItemProp( nIndex );	
	else
		pItemProp = prj.GetItemProp( pSendItemContents->OneItem );
	if( !pItemProp )
	{
		if( strcmp( pSendItemContents->OneItem, "penya" ) == 0 )
			return TRUE;
		return FALSE;
	}

	CItemElem* pItemElem = &pSendItemContents->itemElem;
	pItemElem->m_dwItemId				= pItemProp->dwID;
	pItemElem->m_nItemNum				= pSendItemContents->Item_Count > 0 ? pSendItemContents->Item_Count : 1;
	pItemElem->SetOption( pQry->GetInt( "m_nAbilityOption" ) <= CQuery::CQUERYNULL ? 0 : pQry->GetInt( "m_nAbilityOption" ) );
	
	// mirchang_100514 TransformVisPet_Log
#if __VER >= 15 // __PETVIS
	DWORD dwItemResist					= pQry->GetInt( "m_bItemResist" ) <= CQuery::CQUERYNULL ? 0 : pQry->GetInt( "m_bItemResist" );
	ItemProp* pVisPetProp = prj.GetItemProp( (int)dwItemResist );
	if( pVisPetProp != NULL && pVisPetProp->dwItemKind3 == IK3_PET )
	{
		if( dwItemResist == pItemElem->m_dwItemId )
		{
			pItemElem->m_bTranformVisPet	= TRUE;
			pItemElem->m_bItemResist		= SAI79::NO_PROP;
		}
		else
		{
			Error( "CDbManager::GetSendItem - TransformVisPet ItemId is Different. ItemId : [%d], ItemResist : [%d]", pItemElem->m_dwItemId, dwItemResist );
			pItemElem->m_bItemResist		= SAI79::NO_PROP;
		}
	}
	else
	{
		pItemElem->m_bItemResist		= (BYTE)dwItemResist;
	}
#else // __PETVIS
	pItemElem->m_bItemResist			= pQry->GetInt( "m_bItemResist" ) <= CQuery::CQUERYNULL ? 0 : pQry->GetInt( "m_bItemResist" );
#endif // __PETVIS
	// mirchang_100514 TransformVisPet_Log

	pItemElem->m_nResistAbilityOption	= pQry->GetInt( "m_nResistAbilityOption" ) <= CQuery::CQUERYNULL ? 0 : pQry->GetInt( "m_nResistAbilityOption" );
	pItemElem->m_bCharged				= pQry->GetInt( "m_bCharged" ) <= 0 ? TRUE : FALSE;
	//pItemElem->m_dwKeepTime			= pQry->GetInt64( "m_dwKeepTime" ) <= CQuery::CQUERYNULL ? 0 : pQry->GetInt64( "m_dwKeepTime" );
	pSendItemContents->m_dwKeepTime		= pQry->GetInt64( "m_dwKeepTime" ) <= CQuery::CQUERYNULL ? 0 : pQry->GetInt64( "m_dwKeepTime" );
	
	//pItemElem->SetRandomOptItemId( pQry->GetInt64( "nRandomOptItemId" ) <= CQuery::CQUERYNULL ? 0 : pQry->GetInt64( "nRandomOptItemId" ) );
	pItemElem->SetRandomOptItemId( pQry->GetInt64( "nRandomOptItemId" ) == CQuery::CQUERYNULL ? 0 : pQry->GetInt64( "nRandomOptItemId" ) );

	pItemElem->SetPiercingSize( pQry->GetInt( "nPiercedSize" ) <= CQuery::CQUERYNULL ? 0 : pQry->GetInt( "nPiercedSize" ) );
	for( int i=0; i<pItemElem->GetPiercingSize(); i++ )
	{
		char szItem[32] = {0,};
		sprintf( szItem, "adwItemId%d", i );
		pItemElem->SetPiercingItem( i, pQry->GetInt( szItem ) <= CQuery::CQUERYNULL ? 0 : pQry->GetInt( szItem ) );
	}
	pItemElem->SetUltimatePiercingSize( pQry->GetInt( "nUMPiercedSize" ) <= CQuery::CQUERYNULL ? 0 : pQry->GetInt( "nUMPiercedSize" ) );
	for( i=0; i<pItemElem->GetUltimatePiercingSize(); i++ )
	{
		char szItem[32] = {0,};
		sprintf( szItem, "adwUMItemId%d", i );
		pItemElem->SetUltimatePiercingItem( i, pQry->GetInt( szItem ) <= CQuery::CQUERYNULL ? 0 : pQry->GetInt( szItem ) );
	}
#else // __EXT_PIERCING
	pSendItemContents->m_nAbilityOption			= pQry->GetInt( "m_nAbilityOption" );		// AbilityOption ( + ) Option
	pSendItemContents->nItemResist				= pQry->GetInt( "m_bItemResist" );			// 속성값	
	pSendItemContents->nResistAbilityOption	= pQry->GetInt( "m_nResistAbilityOption" );	// 속성에 대한 Option값
	pSendItemContents->nCharged				= pQry->GetInt( "m_bCharged" );				// 상용화 아이템인지 확인 0 : 상용화, 1 : 상용화 아님
	pSendItemContents->nPiercedSize			= pQry->GetInt( "nPiercedSize" );
	pSendItemContents->adwItemId0			= pQry->GetInt( "adwItemId0" );
	pSendItemContents->adwItemId1			= pQry->GetInt( "adwItemId1" );
	pSendItemContents->adwItemId2			= pQry->GetInt( "adwItemId2" );
	pSendItemContents->adwItemId3			= pQry->GetInt( "adwItemId3" );
#ifdef __SEND_ITEM_ULTIMATE
	pSendItemContents->adwItemId4			= pQry->GetInt( "adwItemId4" );
#endif // __SEND_ITEM_ULTIMATE
	pSendItemContents->m_dwKeepTime			= pQry->GetInt64( "m_dwKeepTime" );
#if __VER >= 11 // __SYS_IDENTIFY
	pSendItemContents->iRandomOptItemId		= pQry->GetInt64( "nRandomOptItemId" );
#else	// __SYS_IDENTIFY
	pSendItemContents->nRandomOptItemId		= pQry->GetInt64( "nRandomOptItemId" );
#endif	// __SYS_IDENTIFY
	
	if( 0 == strcmp( pSendItemContents->OneItem, "NULL") )
	{
		WriteLog( "Send NULL Item_Name : %s", pSendItemContents->OneItem );
		return FALSE;
	}
	
	if( pSendItemContents->Item_Count == -100 || pSendItemContents->Item_Count == 0 )
		pSendItemContents->Item_Count = 1;
	
	if( pSendItemContents->m_nAbilityOption == -100 )
		pSendItemContents->m_nAbilityOption = 0;
	
	if( pSendItemContents->nItemResist == -100 )
		pSendItemContents->nItemResist = 0;
	
	if( pSendItemContents->nResistAbilityOption == -100 )
		pSendItemContents->nResistAbilityOption = 0;

	if( 0 < pSendItemContents->nCharged )
		pSendItemContents->nCharged = 0;
	else
		pSendItemContents->nCharged = 1;
	
	if( pSendItemContents->nPiercedSize < 0 || MAX_PIERCING_ULTIMATE < pSendItemContents->nPiercedSize )
		pSendItemContents->nPiercedSize = 0;

	if( pSendItemContents->adwItemId0 == -100 )
		pSendItemContents->adwItemId0 = 0;

	if( pSendItemContents->adwItemId1 == -100 )
		pSendItemContents->adwItemId1 = 0;
	
	if( pSendItemContents->adwItemId2 == -100 )
		pSendItemContents->adwItemId2 = 0;

	if( pSendItemContents->adwItemId3 == -100 )
		pSendItemContents->adwItemId3 = 0;

#ifdef __SEND_ITEM_ULTIMATE
	if( pSendItemContents->adwItemId4 == -100 )
		pSendItemContents->adwItemId4 = 0;
#endif // __SEND_ITEM_ULTIMATE


	if( pSendItemContents->nPiercedSize == 0 )
	{
		pSendItemContents->adwItemId0 = 0;
		pSendItemContents->adwItemId1 = 0;
		pSendItemContents->adwItemId2 = 0;
		pSendItemContents->adwItemId3 = 0;
#ifdef __SEND_ITEM_ULTIMATE
		pSendItemContents->adwItemId4 = 0;
#endif // __SEND_ITEM_ULTIMATE
	}
	else if( pSendItemContents->nPiercedSize == 1 )
	{
		pSendItemContents->adwItemId1 = 0;
		pSendItemContents->adwItemId2 = 0;
		pSendItemContents->adwItemId3 = 0;
#ifdef __SEND_ITEM_ULTIMATE
		pSendItemContents->adwItemId4 = 0;
#endif // __SEND_ITEM_ULTIMATE
	}
	else if( pSendItemContents->nPiercedSize == 2 )
	{
		pSendItemContents->adwItemId2 = 0;
		pSendItemContents->adwItemId3 = 0;
#ifdef __SEND_ITEM_ULTIMATE
		pSendItemContents->adwItemId4 = 0;
#endif // __SEND_ITEM_ULTIMATE
	}
	else if( pSendItemContents->nPiercedSize == 3 )
	{
		pSendItemContents->adwItemId3 = 0;
#ifdef __SEND_ITEM_ULTIMATE
		pSendItemContents->adwItemId4 = 0;
#endif // __SEND_ITEM_ULTIMATE
	}
#ifdef __SEND_ITEM_ULTIMATE
	else if( pSendItemContents->nPiercedSize == 4 )
	{
		pSendItemContents->adwItemId4 = 0;
	}
#endif // __SEND_ITEM_ULTIMATE
	

	if( pSendItemContents->m_dwKeepTime == -100 )
		pSendItemContents->m_dwKeepTime = 0;
#if __VER >= 11 // __SYS_IDENTIFY
	if( pSendItemContents->iRandomOptItemId == -100 )
		pSendItemContents->iRandomOptItemId	= 0;
#else	// __SYS_IDENTIFY
	if( pSendItemContents->nRandomOptItemId == -100 )
		pSendItemContents->nRandomOptItemId	= 0;
#endif	// __SYS_IDENTIFY
#endif // __EXT_PIERCING
	return TRUE;
}


BOOL CDbManager::SendPenya( CQuery *pQry, char* szSql, CMover* pMover, int nPenya, int nNo )
{
	if( SendItemDeleteQuery( pQry, szSql, nNo ) == FALSE )
		return FALSE;
 
//	ULONGLONG ullSumGold = pMover->m_dwGold + nPenya;
//	DWORD dwError = 0xffffffff;
//	if( dwError < ullSumGold )
//	{
//		pMover->m_dwGold = dwError;
//	}
//	else
//	{
//		pMover->m_dwGold = ullSumGold;
//	}
	pMover->AddGold( nPenya, FALSE );
	return TRUE;
}

#if __VER >= 12 // __EXT_PIERCING
SERIALNUMBER CDbManager::SendItem( CQuery *pQry, char* szSql, CMover* pMover, __SendItemContents& SendItemContents )
{
	SERIALNUMBER iSerialNumber = 0;
	CItemElem* pItemElem = &SendItemContents.itemElem;

	iSerialNumber	= CSerialNumber::GetInstance()->Get();

	// 유지시간 검사
	DWORD dwKeepTime = 0;
	// if( pItemElem->m_dwKeepTime != 0 )
	if( SendItemContents.m_dwKeepTime != 0 )
	{
		DWORD dwTemp;
		int nYear, nMonth, nDay, nHour, nMin;
//		nYear = pItemElem->m_dwKeepTime / 100000000;	dwTemp = pItemElem->m_dwKeepTime % 100000000;
		nYear	= (int)( SendItemContents.m_dwKeepTime / 100000000 );	dwTemp	= (DWORD)( SendItemContents.m_dwKeepTime % 100000000 );
		nMonth = dwTemp / 1000000;		dwTemp = dwTemp % 1000000;
		nDay = dwTemp / 10000;			dwTemp = dwTemp % 10000;
		nHour = dwTemp / 100;			dwTemp = dwTemp % 100;
		nMin = dwTemp;

		CTime tKeepTime( nYear, nMonth, nDay, nHour, nMin, 0 );
		dwKeepTime = (DWORD)( tKeepTime.GetTime() );
		if( (time_t)dwKeepTime <= time( NULL ) )
		{
			FILEOUT( "..\\SendItem.log", "Error dwKeepTime Over IdPlayer = %d, PlayerName = %s, ItemName : %s, KeepTime = %d", pMover->m_idPlayer, pMover->m_szName,  SendItemContents.OneItem, pItemElem->m_dwKeepTime );
			SendItemDeleteQuery( pQry, szSql, SendItemContents.nNo );
			return 0;
		}
		pItemElem->m_dwKeepTime = dwKeepTime;
	}

	// 피어싱 검사
	if( pItemElem->IsPiercedItem() )
	{
		for( int i=0; i<pItemElem->GetPiercingSize(); i++ )		// 일반 아이템
		{
			if( pItemElem->GetPiercingItem( i ) != 0 )
			{
				ItemProp* pItemProp = prj.GetItemProp( pItemElem->GetPiercingItem( i ) );
				if( !pItemProp || !pItemElem->IsPierceAble( pItemProp->dwItemKind3 ) )
				{
					FILEOUT( "..\\SendItem.log", "Error Not IK3_SOCKETCARD or IK3_SOCKETCARD2 Piercing Over IdPlayer = %d, PlayerName = %s, ItemName : %s, PiercingItemId = %d", pMover->m_idPlayer, pMover->m_szName, SendItemContents.OneItem, pItemElem->m_dwItemId );
					SendItemDeleteQuery( pQry, szSql, SendItemContents.nNo );
					return 0;
				}
			}
		}

		for( i=0; i<pItemElem->GetUltimatePiercingSize(); i++ )		// 얼터멋 웨폰
		{
			if( pItemElem->GetUltimatePiercingItem( i ) != 0 )
			{
				ItemProp* pItemProp = prj.GetItemProp( pItemElem->GetUltimatePiercingItem( i ) );
				if( !pItemProp || pItemProp->dwItemKind3 != IK3_ULTIMATE )
				{
					FILEOUT( "..\\SendItem.log", "Error Not IK3_ULTIMATE Piercing Over IdPlayer = %d, PlayerName = %s, ItemName : %s, PiercingItemId = %d", pMover->m_idPlayer, pMover->m_szName, SendItemContents.OneItem, pItemElem->m_dwItemId );
					SendItemDeleteQuery( pQry, szSql, SendItemContents.nNo );
					return 0;
				}
			}
		}
	}	// 피어싱 검사

	//m_Inventory.Add( dwId, dwNum, nOption, iSerialNumber, nItemResist, nResistAbilityOption, NULL, NULL, NULL, nCharged, nPiercedSize, adwItemId0, adwItemId1, adwItemId2, adwItemId3, adwItemId4, m_dwKeepTime, iRandomOptItemId )
	if( !pMover->m_Inventory.Add( pItemElem, NULL, NULL, NULL ) )
	{
		FILEOUT( "..\\SendItem.log", "Error Full IdPlayer = %d, PlayerName = %s, ItemName : %s", pMover->m_idPlayer, pMover->m_szName,  SendItemContents.OneItem );
		return 0;
	}

	if( SendItemDeleteQuery( pQry, szSql, SendItemContents.nNo ) == FALSE )
	{
		//			FILEOUT( "..\\SendItem.log", "Success IdPlayer = %d, PlayerName = %s, ItemName : %s", pMover->m_idPlayer, pMover->m_szName,  szItemName );
		return 0;
	}

	pItemElem->SetSerialNumber( iSerialNumber );
	return pItemElem->GetSerialNumber();
}
#else // __EXT_PIERCING
SERIALNUMBER CDbManager::SendItem( CQuery *pQry, char* szSql, CMover* pMover, __SendItemContents SendItemContents )
{
	SERIALNUMBER iSerialNumber = 0;

	int nIndex = atoi( SendItemContents.OneItem );
	ItemProp* pItemProp = NULL;
	if( nIndex > 0 )
		pItemProp = prj.GetItemProp( nIndex );	
	else
		pItemProp = prj.GetItemProp( SendItemContents.OneItem );	
	
	// 만약 강화 아이템을 넣어야 한다면, Item_Send 테이블에 강화 정도를 나타내는 컬럼을 추가하여 읽은 후, 이 함수 마지막 인자(현재 0)를  그 값으로 대체한다.
	if( pItemProp )
	{
		iSerialNumber	= CSerialNumber::GetInstance()->Get();

		// 유지시간 검사
		DWORD dwKeepTime = 0;
		if( SendItemContents.m_dwKeepTime != 0 )
		{
			DWORD dwTemp;
			int nYear, nMonth, nDay, nHour, nMin;
			nYear = SendItemContents.m_dwKeepTime / 100000000;	dwTemp = SendItemContents.m_dwKeepTime % 100000000;
			nMonth = dwTemp / 1000000;		dwTemp = dwTemp % 1000000;
			nDay = dwTemp / 10000;			dwTemp = dwTemp % 10000;
			nHour = dwTemp / 100;			dwTemp = dwTemp % 100;
			nMin = dwTemp;
			
			CTime tKeepTime( nYear, nMonth, nDay, nHour, nMin, 0 );
			dwKeepTime = tKeepTime.GetTime();
			if( (time_t)dwKeepTime <= time( NULL ) )
			{
				FILEOUT( "..\\SendItem.log", "Error dwKeepTime Over IdPlayer = %d, PlayerName = %s, ItemName : %s, KeepTime = %d", pMover->m_idPlayer, pMover->m_szName,  SendItemContents.OneItem, SendItemContents.m_dwKeepTime );
				SendItemDeleteQuery( pQry, szSql, SendItemContents.nNo );
				return 0;
			}
		}

		// 피어싱 검사
		if( SendItemContents.nPiercedSize != 0 )
		{
			if( pItemProp->dwItemKind3 != IK3_SUIT
#ifdef __SEND_ITEM_ULTIMATE
				&& pItemProp->dwReferStat1 != WEAPON_ULTIMATE
#endif // __SEND_ITEM_ULTIMATE
				)
			{
				FILEOUT( "..\\SendItem.log", "Error Not IK3_SUIT or ULTIMATE PiercedSize Over IdPlayer = %d, PlayerName = %s, ItemName : %s, nPiercedSize = %d", pMover->m_idPlayer, pMover->m_szName, SendItemContents.OneItem, SendItemContents.nPiercedSize );
				SendItemDeleteQuery( pQry, szSql, SendItemContents.nNo );
				return 0;
			}

			for( int nitem = 0 ; nitem < SendItemContents.nPiercedSize ; ++nitem )
			{
				ItemProp* pItemPropPiercing;
				DWORD dwItemid;
				if( nitem == 0 )
				{
					pItemPropPiercing = prj.GetItemProp( SendItemContents.adwItemId0 );
					dwItemid = SendItemContents.adwItemId0;
				}
				else if( nitem == 1 )
				{
					pItemPropPiercing = prj.GetItemProp( SendItemContents.adwItemId1 );
					dwItemid = SendItemContents.adwItemId1;
				}
				else if( nitem == 2 )
				{
					pItemPropPiercing = prj.GetItemProp( SendItemContents.adwItemId2 );
					dwItemid = SendItemContents.adwItemId2;
				}
				else if( nitem == 3 )
				{
					pItemPropPiercing = prj.GetItemProp( SendItemContents.adwItemId3 );
					dwItemid = SendItemContents.adwItemId3;
				}
#ifdef __SEND_ITEM_ULTIMATE
				else if( nitem == 4 )
				{
					pItemPropPiercing = prj.GetItemProp( SendItemContents.adwItemId4 );
					dwItemid = SendItemContents.adwItemId4;
				}
#endif // __SEND_ITEM_ULTIMATE


				if( dwItemid != 0 )
				{
					if( pItemPropPiercing == NULL || (pItemPropPiercing->dwItemKind3 != IK3_SOCKETCARD)
#ifdef __SEND_ITEM_ULTIMATE
						&& (pItemPropPiercing->dwItemKind3 != IK3_ULTIMATE)
#endif // __SEND_ITEM_ULTIMATE
						)
					{
						FILEOUT( "..\\SendItem.log", "Error Not IK3_SOCKETCARD or IK3_ULTIMATE Piercing Over IdPlayer = %d, PlayerName = %s, ItemName : %s, PiercingItemId = %d", pMover->m_idPlayer, pMover->m_szName, SendItemContents.OneItem, dwItemid );
						SendItemDeleteQuery( pQry, szSql, SendItemContents.nNo );
						return 0;			
					}
				}
			}
		}
		
#if __VER >= 11 // __SYS_IDENTIFY
#ifdef __SEND_ITEM_ULTIMATE
		if( pMover->AddItem( ITYPE_ITEM, pItemProp->dwID, SendItemContents.Item_Count, SendItemContents.m_nAbilityOption, iSerialNumber, SendItemContents.nItemResist, SendItemContents.nResistAbilityOption, SendItemContents.nCharged, SendItemContents.nPiercedSize, SendItemContents.adwItemId0, SendItemContents.adwItemId1, SendItemContents.adwItemId2, SendItemContents.adwItemId3, SendItemContents.adwItemId4, dwKeepTime, SendItemContents.iRandomOptItemId ) == FALSE ) // 실재로 아이템을 넣음
#else // __SEND_ITEM_ULTIMATE
		if( pMover->AddItem( ITYPE_ITEM, pItemProp->dwID, SendItemContents.Item_Count, SendItemContents.m_nAbilityOption, iSerialNumber, SendItemContents.nItemResist, SendItemContents.nResistAbilityOption, SendItemContents.nCharged, SendItemContents.nPiercedSize, SendItemContents.adwItemId0, SendItemContents.adwItemId1, SendItemContents.adwItemId2, SendItemContents.adwItemId3, dwKeepTime, SendItemContents.iRandomOptItemId ) == FALSE ) // 실재로 아이템을 넣음
#endif // __SEND_ITEM_ULTIMATE
#else	// __SYS_IDENTIFY
#ifdef __SEND_ITEM_ULTIMATE
		if( pMover->AddItem( ITYPE_ITEM, pItemProp->dwID, SendItemContents.Item_Count, SendItemContents.m_nAbilityOption, iSerialNumber, SendItemContents.nItemResist, SendItemContents.nResistAbilityOption, SendItemContents.nCharged, SendItemContents.nPiercedSize, SendItemContents.adwItemId0, SendItemContents.adwItemId1, SendItemContents.adwItemId2, SendItemContents.adwItemId3, SendItemContents.adwItemId4, dwKeepTime, SendItemContents.nRandomOptItemId ) == FALSE ) // 실재로 아이템을 넣음		
#else // __SEND_ITEM_ULTIMATE
		if( pMover->AddItem( ITYPE_ITEM, pItemProp->dwID, SendItemContents.Item_Count, SendItemContents.m_nAbilityOption, iSerialNumber, SendItemContents.nItemResist, SendItemContents.nResistAbilityOption, SendItemContents.nCharged, SendItemContents.nPiercedSize, SendItemContents.adwItemId0, SendItemContents.adwItemId1, SendItemContents.adwItemId2, SendItemContents.adwItemId3, dwKeepTime, SendItemContents.nRandomOptItemId ) == FALSE ) // 실재로 아이템을 넣음
#endif // __SEND_ITEM_ULTIMATE
#endif	// __SYS_IDENTIFY
		{
			FILEOUT( "..\\SendItem.log", "Error Full IdPlayer = %d, PlayerName = %s, ItemName : %s", pMover->m_idPlayer, pMover->m_szName,  SendItemContents.OneItem );
			return 0;
		}
		
		if( SendItemDeleteQuery( pQry, szSql, SendItemContents.nNo ) == FALSE )
		{
			//			FILEOUT( "..\\SendItem.log", "Success IdPlayer = %d, PlayerName = %s, ItemName : %s", pMover->m_idPlayer, pMover->m_szName,  szItemName );
			return 0;
		}
	}
	else
	{
		if( SendItemDeleteQuery( pQry, szSql, SendItemContents.nNo ) == FALSE )
		{
			FILEOUT( "..\\SendItem.log", "Error Not PropElem IdPlayer = %d, PlayerName = %s, ItemName : %s", pMover->m_idPlayer, pMover->m_szName,  SendItemContents.OneItem );
			return 0;
		}
	}
	return iSerialNumber;
}
#endif // __EXT_PIERCING


BOOL CDbManager::SendItemDeleteQuery( CQuery *pQry, char* szSql, int nNo )
{
	DBQryCharacter( szSql, "S5", 0, 0, "", "", nNo );
	
	if( FALSE == pQry->Exec( szSql ) )
	{
		WriteLog( "SendItemDeleteQuery :: nNo = %d", nNo );
		return FALSE;
	}
	
	if( pQry->Fetch() )
	{
		int nError	= pQry->GetInt( "fError" );
		if( nError == 0 )
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CDbManager::ItemLogQuery( CQuery *pQryLog, char *szQueryState, const LogItemInfo& info, u_long uIdPlayer, int nNo, char *szItemName )
{
	char szQuerylog[4096] = { 0, };

	DBQryLog( szQuerylog, szQueryState, uIdPlayer , g_appInfo.dwSys, nNo, 0, 0, 0, 0, info.nItemResist, info.nResistAbilityOption, 0, 0, 0, 0, 'A', 0, '\0',
		0.0f, 0.0f, 0.0f, 0, 0, "\0", 0, 0, 0, szItemName, 0, info.itemNumber );
			
	if( FALSE == pQryLog->Exec( szQuerylog ) )
	{
		WriteLog( "%s, %d\tSendItemLogQuery LC = %s", __FILE__, __LINE__, szQuerylog );
		return FALSE;
	}
	
	DBQryNewItemLog( szQuerylog, info );
	
	if( FALSE == pQryLog->Exec( szQuerylog ) )
	{
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuerylog );
		return FALSE;
	}
	return TRUE;
}

BOOL CDbManager::RemoveItemtoCharacter( CMover* pMover, CQuery *qry, CQuery *qry1, CQuery *qrylog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	BOOL	bResult		= FALSE;

	//  CHARACTER_STR 'S6',@im_idPlayer,@iserverindex
	char szQuery[QUERY_SIZE]	= { 0,};
	DBQryCharacter( szQuery, "S6", pMover->m_idPlayer, g_appInfo.dwSys );
	
	if( FALSE == qry->Exec( szQuery ) )
	{
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );
		return FALSE;
	}
	
	while( qry->Fetch() )
	{
		char chState;
		char OneItem[MAX_PATH];
		int nNo, Item_Count, m_nAbilityOption, nItemResist, nResistAbilityOption;
		SERIALNUMBER iSerialNumber;
		BOOL bIsPenya = FALSE;
		
		if( GetRemoveItem( qry, nNo, OneItem, Item_Count, m_nAbilityOption, nItemResist, nResistAbilityOption, chState ) == FALSE )
			continue;

		if( 0 == strcmp( OneItem, "penya" ) )				// 페냐인지 확인
		{
			if( RemovePenya( qry1, szQuery, pMover, Item_Count, nNo, chState ) == FALSE )
			{
				WriteLog( "RemoveItem::RemovePenya PlayerName = %s, Item_Count = %d, State = %c, Qry1 = %s", pMover->m_szName, Item_Count, chState, szQuery );
				continue;
			}
			bIsPenya = TRUE;
		}
		else												// 아이템 삭제
		{
			iSerialNumber	= RemoveItem( qry1, szQuery, nNo, pMover, OneItem, Item_Count, m_nAbilityOption, nItemResist, nResistAbilityOption, chState );
		}

		if( bIsPenya )				// 페냐인지 확인
		{
			//sprintf( OneItem, "SEED" );
			_stprintf( OneItem, "%d", II_GOLD_SEED1 );
			iSerialNumber = 0;
		}
		else
		{
			if( iSerialNumber == 0 )
				continue;
		}

		bResult		= TRUE;

		LogItemInfo aLogItem;

		if( chState == 'I' )
			aLogItem.Action = "I";
		else
			aLogItem.Action = "K";

		aLogItem.SendName = pMover->m_szName;
		aLogItem.RecvName = "DB";
		//aLogItem.ItemName = OneItem;
		// mirchang_100409 item name -> item Id
		char szItemId[32] = {0, };
		if( lstrlen( OneItem ) > 0 )
		{
			ItemProp* pItemProp = NULL;
			int nIndex = atoi( OneItem );
			if( nIndex > 0 )
			{
				pItemProp = prj.GetItemProp( nIndex );
			}
			else
			{
				pItemProp = prj.GetItemProp( OneItem );
			}
			if( pItemProp != NULL )
			{
				_ultoa( pItemProp->dwID, szItemId, 10 );
			}
			else
			{
				_stprintf( szItemId, "%d", -1 );
			}
		}
		_tcscpy( aLogItem.szItemName, szItemId );
		// mirchang_100317 item name -> item Id
		aLogItem.Gold2 = pMover->GetGold();
		aLogItem.itemNumber = Item_Count;
		aLogItem.nItemResist = nItemResist;
		aLogItem.nResistAbilityOption = nResistAbilityOption;
		
		if( bIsPenya )
		{
			aLogItem.Gold = pMover->GetGold() + Item_Count;
		}
		else
		{
			aLogItem.Gold = pMover->GetGold();
			aLogItem.ItemNo = iSerialNumber;
			aLogItem.nAbilityOption = m_nAbilityOption;
		}
		if( ItemLogQuery( qrylog, "LD", aLogItem, pMover->m_idPlayer, nNo, OneItem ) == FALSE )
			continue;
	}
	return bResult;
}

BOOL CDbManager::RemoveItemDeleteQuery( CQuery *pQry, char* szSql, int nNo )
{
	DBQryCharacter( szSql, "S7", 0, 0, "", "", nNo );

	if( FALSE == pQry->Exec( szSql ) )
	{
		WriteLog( "RemoveItemDeleteQuery :: nNo = %d", nNo );
		return FALSE;
	}

	if( pQry->Fetch() )
	{
		int nError	= pQry->GetInt( "fError" );
		if( nError == 0 )
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

HANDLE s_hHandle	= (HANDLE)NULL;

BOOL CDbManager::CreateDbWorkers( void )
{
	s_hHandle	= CreateEvent( NULL, FALSE, FALSE, NULL );

	DWORD dwThreadId;

	m_hIOCPGet	= CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );	// select
	ASSERT( m_hIOCPGet );
	for( int i = 0; i < MAX_GETTHREAD_SIZE; i++ )
	{
		m_hThreadGet[i]		= chBEGINTHREADEX( NULL, 0, _GetThread, (LPVOID)this, 0, &dwThreadId ); 
		ASSERT( m_hThreadGet[i] );
		SetThreadPriority( m_hThreadGet[i], THREAD_PRIORITY_BELOW_NORMAL );
//		Sleep( 1000 );
		if( WaitForSingleObject( s_hHandle, SEC( 3 ) ) == WAIT_TIMEOUT )
			OutputDebugString( "DATABASESERVER.EXE\t// TIMEOUT\t// ODBC\n" );
	}
	m_hIOCPPut	= CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );	// insert log
	ASSERT( m_hIOCPPut );
	for( i = 0; i < MAX_PUTTHREAD_SIZE; i++ )
	{
		m_hThreadPut[i]		= chBEGINTHREADEX( NULL, 0, _PutThread, (LPVOID)this, 0, &dwThreadId ); 
		ASSERT( m_hThreadPut[i] );
		SetThreadPriority( m_hThreadPut[i], THREAD_PRIORITY_BELOW_NORMAL );
//		Sleep( 1000 );
		if( WaitForSingleObject( s_hHandle, SEC( 3 ) ) == WAIT_TIMEOUT )
			OutputDebugString( "DATABASESERVER.EXE\t// TIMEOUT\t// ODBC\n" );
	}

	m_hIOCPGuild = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );	// 길드용
	m_hThreadGuild = chBEGINTHREADEX( NULL, 0, _GuildThread, (LPVOID)this, 0, &dwThreadId ); 

	if( WaitForSingleObject( s_hHandle, SEC( 3 ) ) == WAIT_TIMEOUT )
		OutputDebugString( "DATABASESERVER.EXE\t// TIMEOUT\t// ODBC\n" );

	m_hCloseSPThread	= CreateEvent( NULL, FALSE, FALSE, NULL );
	m_hSPThread	= chBEGINTHREADEX( NULL, 0, _SPThread, (LPVOID)this, 0, &dwThreadId );	// update
	ASSERT( m_hSPThread );

#ifdef __S1108_BACK_END_SYSTEM
	m_hCloseWorker	= CreateEvent( NULL, FALSE, FALSE, NULL );
	m_hWorker	= chBEGINTHREADEX( NULL, 0, _BackSystem, this, 0, &dwThreadId );
	ASSERT( m_hWorker );
#endif // __S1108_BACK_END_SYSTEM
	
	m_hItemUpdateCloseWorker	= CreateEvent( NULL, FALSE, FALSE, NULL );
	m_hItemUpdateWorker	= chBEGINTHREADEX( NULL, 0, _ItemUpdateThread, this, 0, &dwThreadId );
	ASSERT( m_hWorker );

	for( i = 0; i < MAX_QUERY_RESERVED; i++ )
	{
		m_apQuery[i]	= new CQuery;
		if( FALSE == m_apQuery[i]->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
		{
			for( i = 0; i < MAX_QUERY_RESERVED; i++ )
				SAFE_DELETE( m_apQuery[i] );
			OutputDebugString( "MAX_QUERY_RESERVED" );
			return FALSE;
		}
	}
	return TRUE;
}

void CDbManager::CloseDbWorkers( void )
{
	if( NULL != m_hIOCPGet )
	{
		for( int i = 0; i < MAX_GETTHREAD_SIZE; i++ )
			PostQueuedCompletionStatus( m_hIOCPGet, 0, NULL, NULL );
		WaitForMultipleObjects( MAX_GETTHREAD_SIZE, m_hThreadGet, TRUE, INFINITE );
		CLOSE_HANDLE( m_hIOCPGet );
		for( i = 0; i < MAX_GETTHREAD_SIZE; i++ )
			CLOSE_HANDLE( m_hThreadGet[i] );
	}
	if( NULL != m_hIOCPPut )
	{
		for( int i = 0; i < MAX_PUTTHREAD_SIZE; i++ )
			PostQueuedCompletionStatus( m_hIOCPPut, 0, NULL, NULL );
		WaitForMultipleObjects( MAX_PUTTHREAD_SIZE, m_hThreadPut, TRUE, INFINITE );
		CLOSE_HANDLE( m_hIOCPPut );
		for( i = 0; i < MAX_PUTTHREAD_SIZE; i++ )
			CLOSE_HANDLE( m_hThreadPut[i] );
	}

	if( m_hIOCPGuild )
	{
		PostQueuedCompletionStatus( m_hIOCPGuild, 0, NULL, NULL );
		WaitForSingleObject( m_hThreadGuild, INFINITE );
		CLOSE_HANDLE( m_hIOCPGuild );
		CLOSE_HANDLE( m_hThreadGuild );
	}

#ifdef __S1108_BACK_END_SYSTEM
	CLOSE_THREAD( m_hWorker, m_hCloseWorker );
#endif // __S1108_BACK_END_SYSTEM

	CLOSE_THREAD( m_hItemUpdateWorker, m_hItemUpdateCloseWorker );

	CLOSE_THREAD( m_hSPThread, m_hCloseSPThread );

	for( int i = 0; i < MAX_QUERY_RESERVED; i++ )
		SAFE_DELETE( m_apQuery[i] );

	CLOSE_HANDLE( s_hHandle );
}

UINT CDbManager::_GetThread( LPVOID pParam )
{
	CDbManager* pDbManager	= (CDbManager*)pParam;
	pDbManager->GetThread();
	return 0;
}

UINT CDbManager::_PutThread( LPVOID pParam )
{
	CDbManager* pDbManager	= (CDbManager*)pParam;
	pDbManager->PutThread();
	return 0;
}

UINT CDbManager::_SPThread( LPVOID pParam )
{
	CDbManager* pDbManager	= (CDbManager*)pParam;
	pDbManager->SPThread();
	return 0;
}

UINT CDbManager::_UpdateThread( LPVOID pParam )
{
	CDbManager* pDbManager	= (CDbManager*)pParam;
	pDbManager->UpdateThread();
	return 0;
}

UINT CDbManager::_GuildThread( LPVOID pParam )
{
	CDbManager* pDbManager	= (CDbManager*)pParam;
	pDbManager->GuildThread();
	return 0;
}

#ifdef __S1108_BACK_END_SYSTEM
UINT CDbManager::_BackSystem( LPVOID pParam )
{
	CDbManager* pDbManager	= (CDbManager*)pParam;
	pDbManager->BackSystem();
	return 0;
}
#endif // __S1108_BACK_END_SYSTEM

UINT CDbManager::_ItemUpdateThread( LPVOID pParam )
{
	CDbManager* pDbManager	= (CDbManager*)pParam;
	pDbManager->ItemUpdateThread();
	return 0;
}

void CDbManager::GetThread( void )
{
	CQuery* pQuery	= new CQuery;
	if( FALSE == pQuery->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
	{
		WriteLog( "%s, %d CharacterDB Connect", __FILE__, __LINE__ );
		SAFE_DELETE( pQuery );
		return;
	}
	
	CQuery* pQuery1	= new CQuery;
	if( FALSE == pQuery1->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
	{
		WriteLog( "%s, %d CharacterDB Connect", __FILE__, __LINE__ );
		SAFE_DELETE( pQuery );
		SAFE_DELETE( pQuery1 );
		return;
	}

#ifdef __NOLOG
	CQuery* pQueryLog	= NULL;
#else	// __NOLOG
	CQuery* pQueryLog = new CQuery;
	if( FALSE == pQueryLog->Connect( 3, DSN_NAME_LOG, DB_ADMIN_ID_LOG, DB_ADMIN_PASS_LOG ) )
	{
		WriteLog( "%s, %d LogDB Connect", __FILE__, __LINE__ );
		SAFE_DELETE( pQuery );
		SAFE_DELETE( pQuery1 );
		SAFE_DELETE( pQueryLog );
		return;
	}
#endif	// __NOLOG

	SetEvent( s_hHandle );

	BOOL bReturnValue	= FALSE;
	DWORD dwBytesTransferred	= 0;
	DWORD dwCompletionKey	= 0;
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus	= NULL;
	
	while( true )
	{
		bReturnValue	= GetQueuedCompletionStatus( m_hIOCPGet, &dwBytesTransferred, &dwCompletionKey, (LPOVERLAPPED*)&lpDbOverlappedPlus, INFINITE );
		if( FALSE == bReturnValue )
		{
			ASSERT( 0 );
		}
		if( dwBytesTransferred == 0 )
		{
			SAFE_DELETE( pQuery );
			SAFE_DELETE( pQuery1 );
			SAFE_DELETE( pQueryLog );
			return;
		}

		switch( lpDbOverlappedPlus->nQueryMode )
		{
			case SENDPLAYERLIST:
				SendPlayerList( pQuery, lpDbOverlappedPlus );
				break;
			case JOIN:
				Join( pQuery, pQuery1, pQueryLog, lpDbOverlappedPlus );
				break;
			case CREATEPLAYER:
				CreatePlayer( pQuery, lpDbOverlappedPlus );
				break;
			case REMOVEPLAYER:
				RemovePlayer( pQuery, lpDbOverlappedPlus );
				break;
#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
			case QM_LOGIN_PROTECT:
				LoginProtectCert( pQuery, lpDbOverlappedPlus );
				break;
#endif // __2ND_PASSWORD_SYSTEM
			default:
				{
					WriteLog( "%s, %d\t%d", __FILE__, __LINE__, lpDbOverlappedPlus->nQueryMode );
					FreeRequest( lpDbOverlappedPlus );
					return;
				}
		}
	}
}

void CDbManager::PutThread( void )	// log
{
	CQuery* pQueryChar	= new CQuery;
#ifndef __NOLOG
	CQuery* pQueryLog	= new CQuery;
#else	// __NOLOG
	CQuery* pQueryLog	= NULL;
#endif	// __NOLOG
	
	if( FALSE == pQueryChar->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
	{
		WriteLog( "%s, %d", __FILE__, __LINE__ );
		SAFE_DELETE( pQueryChar );
		SAFE_DELETE( pQueryLog );
		return;
	}
#ifndef __NOLOG
	if( FALSE == pQueryLog->Connect( 3, DSN_NAME_LOG, DB_ADMIN_ID_LOG, DB_ADMIN_PASS_LOG ) )
	{
		WriteLog( "%s, %d", __FILE__, __LINE__ );
		SAFE_DELETE( pQueryChar );
		SAFE_DELETE( pQueryLog );
		return;
	}
#endif	// __NOLOG

	SetEvent( s_hHandle );

	BOOL bReturnValue	= FALSE;
	DWORD dwBytesTransferred	= 0;
	DWORD dwCompletionKey		= 0;
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus	= NULL;
	
	while( true )
	{
		bReturnValue	= GetQueuedCompletionStatus( m_hIOCPPut, &dwBytesTransferred, &dwCompletionKey, (LPOVERLAPPED*)&lpDbOverlappedPlus, INFINITE );
		if( FALSE == bReturnValue )
		{
			ASSERT( 0 );
		}
		if( dwBytesTransferred == 0 )
		{
			SAFE_DELETE( pQueryChar );
#ifndef __NOLOG
			SAFE_DELETE( pQueryLog );
#endif	// __NOLOG
			return;
		}

		switch( lpDbOverlappedPlus->nQueryMode )
		{
#ifndef __NOLOG
			case SAVECONCURRENTUSERNUMBER:
				LogConcurrentUserNumber( pQueryLog, lpDbOverlappedPlus );
#ifdef __TOOMANY_PENDINGLOG1115
				CDbManager::ReleasePending();
#endif	// __TOOMANY_PENDINGLOG1115
				break;
			case LOG_PLAY_CONNECT:
				LogPlayConnect( pQueryLog, lpDbOverlappedPlus );
#ifdef __TOOMANY_PENDINGLOG1115
				CDbManager::ReleasePending();
#endif	// __TOOMANY_PENDINGLOG1115
				break;
			case LOG_ITEM:
				LogItem( pQueryLog, lpDbOverlappedPlus );
#ifdef __TOOMANY_PENDINGLOG1115
				CDbManager::ReleasePending();
#endif	// __TOOMANY_PENDINGLOG1115
				break;
			case LOG_PLAY_DEATH:
				LogPlayDeath( pQueryLog, lpDbOverlappedPlus );
#ifdef __TOOMANY_PENDINGLOG1115
				CDbManager::ReleasePending();
#endif	// __TOOMANY_PENDINGLOG1115
				break;
			case LOG_LEVELUP:
				LogLevelUp( pQueryLog, lpDbOverlappedPlus );
#ifdef __TOOMANY_PENDINGLOG1115
				CDbManager::ReleasePending();
#endif	// __TOOMANY_PENDINGLOG1115
				break;
			case LOG_SERVER_DEATH:
				LogServerDeath( pQueryLog, lpDbOverlappedPlus );
#ifdef __TOOMANY_PENDINGLOG1115
				CDbManager::ReleasePending();
#endif	// __TOOMANY_PENDINGLOG1115
				break;
			case LOG_UNIQUEITEM:
				LogUniqueItem( pQueryLog, lpDbOverlappedPlus );
#ifdef __TOOMANY_PENDINGLOG1115
				CDbManager::ReleasePending();
#endif	// __TOOMANY_PENDINGLOG1115
				break;
			case LOG_QUEST:
				LogQuest( pQueryLog, lpDbOverlappedPlus );
#ifdef __TOOMANY_PENDINGLOG1115
				CDbManager::ReleasePending();
#endif	// __TOOMANY_PENDINGLOG1115
				break;
			case GAMEMA_CHAT:
				Gamema_Chat( pQueryLog, lpDbOverlappedPlus );
#ifdef __TOOMANY_PENDINGLOG1115
				CDbManager::ReleasePending();
#endif	// __TOOMANY_PENDINGLOG1115
				break;
			case LOG_PK_PVP:
				LogPkPvp( pQueryLog, lpDbOverlappedPlus );
#ifdef __TOOMANY_PENDINGLOG1115
				CDbManager::ReleasePending();
#endif	// __TOOMANY_PENDINGLOG1115
				break;
			case LOG_SCHOOL:
				LogSchool( pQueryLog, lpDbOverlappedPlus );
#ifdef __TOOMANY_PENDINGLOG1115
				CDbManager::ReleasePending();
#endif	// __TOOMANY_PENDINGLOG1115
				break;
			case LOG_SKILLPOINT:
				LogSkillPoint( pQueryLog, lpDbOverlappedPlus );
				break;

#endif	// __NOLOG

			case QM_SET_PLAYER_NAME:
				SetPlayerName( pQueryChar, pQueryLog, lpDbOverlappedPlus );
				break;

			case QM_SNOOPGUILD:
				SnoopGuild( pQueryLog, lpDbOverlappedPlus );
				break;

			case SAVE_PLAY_TIME:
				SavePlayTime( pQueryChar, lpDbOverlappedPlus );
				break;
#ifdef __S_NEW_SKILL_2
			case SAVE_SKILL: 
				AllSaveSkill( pQueryChar, lpDbOverlappedPlus );
				break;
#endif // __S_NEW_SKILL_2

#if __VER >= 13 // __HONORABLE_TITLE			// 달인
			case LOG_GETHONORTIME: 
				LogGetHonorTime( pQueryLog, lpDbOverlappedPlus );
				break;
#endif	// __HONORABLE_TITLE			// 달인
#ifdef __RT_1025
			case QM_ADD_MESSENGER:
				AddMessenger( pQueryChar, lpDbOverlappedPlus );
				break;
			case QM_DELETE_MESSENGER:
				DeleteMessenger( pQueryChar, lpDbOverlappedPlus );
				break;
			case QM_UPDATE_MESSENGER:
				UpdateMessenger( pQueryChar, lpDbOverlappedPlus );
				break;
#else	// __RT_1025
			case REMOVE_FRIEND:
				RemoveFriend( pQueryChar, lpDbOverlappedPlus );
				break;
#endif	// __RT_1025
			case ADD_PARTYNAME:
				AddPartyName( pQueryChar, lpDbOverlappedPlus );
				break;
			case CHANGE_BANKPASS:
				ChangeBankPass( pQueryChar, lpDbOverlappedPlus );
				break;

			case INSERT_TAG:
				InsertTag( pQueryChar, lpDbOverlappedPlus );
				break;
#ifndef __NOLOG
			case SCHOOL_REPORT:
				SchoolReport( pQueryLog, lpDbOverlappedPlus );
#ifdef __TOOMANY_PENDINGLOG1115
				CDbManager::ReleasePending();
#endif	// __TOOMANY_PENDINGLOG1115
				break;
#endif	// __NOLOG
			case QM_DELETE_REMOVE_GUILD_BANK_TBL:
				DeleteRemoveGuildBankTbl( pQueryChar, lpDbOverlappedPlus );
				break;
			case QM_CALL_USPLOGGINGQUEST:
				call_uspLoggingQuest( pQueryLog, lpDbOverlappedPlus );
				break;
			case QM_CALL_XXX_MULTI_SERVER:
				call_uspXXXMultiServer( pQueryChar, lpDbOverlappedPlus );
				break;
			case QM_LOG_EXPBOX:
				LogExpBox( pQueryLog, lpDbOverlappedPlus );
				break;
			case QM_CALL_USP_PET_LOG:
				CalluspPetLog( pQueryLog, lpDbOverlappedPlus );
				break;
#if __VER >= 14 // __INSTANCE_DUNGEON
			case LOG_INSTANCEDUNGEON:
				LogInstanceDungeon( pQueryLog, lpDbOverlappedPlus );
				break;
#endif // __INSTANCE_DUNGEON
#ifdef __ERROR_LOG_TO_DB
			case LOG_ERROR:
				LogError( pQueryLog, lpDbOverlappedPlus );
				break;
#endif // __ERROR_LOG_TO_DB
#if __VER >= 15 // __GUILD_HOUSE
			case LOG_GUILDFURNITURE:
				LogGuildFurniture( pQueryLog, lpDbOverlappedPlus );
				break;
#endif // __GUILD_HOUSE
			default:
				WriteLog( "%s, %d\t%d", __FILE__, __LINE__, lpDbOverlappedPlus->nQueryMode );
				FreeRequest( lpDbOverlappedPlus );
				break;
		} // end of switch 
	} // end of while
}

void CDbManager::UpdateThread( void )
{
	CQuery* pQuery	= new CQuery;
	if( FALSE == pQuery->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
	{
		WriteLog( "%s, %d", __FILE__, __LINE__ );
		SAFE_DELETE( pQuery );
		return;
	}

	CQuery* pQueryLog	= new CQuery;
	if( FALSE == pQueryLog->Connect( 3, DSN_NAME_LOG, DB_ADMIN_ID_LOG, DB_ADMIN_PASS_LOG ) )
	{
		char s[128];
		sprintf( s, "%s, %d", __FILE__, __LINE__ );
		AfxMessageBox( s );
		SAFE_DELETE( pQuery );
		SAFE_DELETE( pQueryLog );
		return;
	}

	SetEvent( s_hHandle );

	BOOL bReturnValue	= FALSE;
	DWORD dwBytesTransferred	= 0;
	DWORD dwCompletionKey		= 0;
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus	= NULL;
	char* szQuery	= new char[40960];
	CMover* pMover	= NULL;
	
	while( true )
	{
		bReturnValue	= GetQueuedCompletionStatus( m_hIOCPUpdate, &dwBytesTransferred, &dwCompletionKey, (LPOVERLAPPED*)&lpDbOverlappedPlus, INFINITE );
		if( FALSE == bReturnValue )
		{
			ASSERT( 0 );
		}

		if( dwBytesTransferred == 0 )
		{
			SAFE_DELETE( pQuery );
			SAFE_DELETE( pQueryLog );
			SAFE_DELETE_ARRAY( szQuery );
			return;
		}

//		SetEvent( m_hWait );

		pMover	= (CMover*)dwBytesTransferred;
		SavePlayer( pQuery, pQueryLog, pMover, szQuery );
#ifdef __INVALID_LOGIN_0612
		if( pMover->m_bLogout )
			g_DbManager.m_AccountCacheMgr.ChangeMultiServer( pMover->m_szAccount, 0 );
#endif	// __INVALID_LOGIN_0612
		SAFE_DELETE( pMover );
	}
}

void CDbManager::SPThread( void )
{
	DWORD dwThreadId;
	m_hIOCPUpdate	= CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );	// insert log
	ASSERT( m_hIOCPUpdate );
	for( int i = 0; i < MAX_UPDATETHREAD_SIZE; i++ )
	{
		m_hThreadUpdate[i]		= chBEGINTHREADEX( NULL, 0, _UpdateThread, (LPVOID)this, 0, &dwThreadId ); 
		ASSERT( m_hThreadUpdate[i] );
		SetThreadPriority( m_hThreadUpdate[i], THREAD_PRIORITY_BELOW_NORMAL );
		if( WaitForSingleObject( s_hHandle, SEC( 3 ) ) == WAIT_TIMEOUT )
			OutputDebugString( "DATABASESERVER.EXE\t// TIMEOUT\t// ODBC\n" );
	}
	
	WaitForSingleObject( m_hCloseSPThread, INFINITE );

	for( i = 0; i < MAX_UPDATETHREAD_SIZE; i++ )
		PostQueuedCompletionStatus( m_hIOCPUpdate, 0, NULL, NULL );
	WaitForMultipleObjects( MAX_UPDATETHREAD_SIZE, m_hThreadUpdate, TRUE, INFINITE );
	CLOSE_HANDLE( m_hIOCPUpdate );
	for( i = 0; i < MAX_UPDATETHREAD_SIZE; i++ )
		CLOSE_HANDLE( m_hThreadUpdate[i] );
}

void CDbManager::CreateGuild( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
//	u_long idPlayer, idGuild;
	int nSize;
	GUILD_MEMBER_INFO info[MAX_PTMEMBER_SIZE];
	u_long idGuild;
	char szGuild[MAX_G_NAME];
//	ar >> idPlayer >> idGuild;
	ar >> nSize;
	ar.Read( info, sizeof(GUILD_MEMBER_INFO)*nSize );
	ar >> idGuild;
	ar.ReadString( szGuild, MAX_G_NAME );

	GUILD_QUERYINFO qi1( "A1" );
	qi1.idGuild	= idGuild;
	qi1.pszGuild	= szGuild;

	char szQuery[QUERY_SIZE]	= { 0,};
	DBQryGuild( szQuery, qi1 );

	if( FALSE == pQuery->Exec( szQuery ) )
	{
		FreeRequest( lpDbOverlappedPlus );
		return;
	}
	if( pQuery->Fetch() )
	{
		int nError	= pQuery->GetInt( "nError" );
		if( nError != 1 )
		{
			FreeRequest( lpDbOverlappedPlus );
			return;
		}
	}
	for( int i = 0; i < nSize; i++ )
	{
		if( info[i].idPlayer != 0 )
		{
			GUILD_QUERYINFO qi2( "A2" );
//			qi2.idPlayer	= idPlayer;
			qi2.idPlayer	= info[i].idPlayer;
			qi2.idGuild	= idGuild;
			qi2.nLevel		= ( i == 0? GUD_MASTER: GUD_ROOKIE );
			DBQryGuild( szQuery, qi2 );
			if( FALSE == pQuery->Exec( szQuery ) )
			{
//				WriteLog( "CreateGuild(): A2" );
			}
		}
	}
#ifndef __NOLOG
	GUILDLOG_QUERYINFO qi( "L3" );
	qi.idGuild = idGuild;
	qi.idPlayer = info[0].idPlayer;
	qi.pszState = "S";
	qi.pszGuildBank = szGuild;
	
	DBQryGuildLog( szQuery, qi );
	if( FALSE == pQueryLog->Exec( szQuery ) )
	{
		FreeRequest( lpDbOverlappedPlus );
		return;
	}
	GUILDLOG_QUERYINFO LogQi( "L1" );
	LogQi.idGuild = idGuild;
	LogQi.idDoPlayer = info[0].idPlayer;
	LogQi.pszState = "G";

	for( i = 0; i < nSize; i++ )
	{
		if( info[i].idPlayer != 0 )
		{
			LogQi.idPlayer = info[i].idPlayer;
			DBQryGuildLog( szQuery, LogQi );
			if( FALSE == pQueryLog->Exec( szQuery ) )
			{
				FreeRequest( lpDbOverlappedPlus );
				return;
			}
		}
	}
	
#endif // __NOLOG
	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::DestroyGuild( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long idGuild, idMaster;
	ar >> idGuild;
	ar >> idMaster;

	GUILD_QUERYINFO info( "D1" );
	info.idGuild	= idGuild;

	char szQuery[QUERY_SIZE]	= { 0,};
	DBQryGuild( szQuery, info );

	if( FALSE == pQuery->Exec( szQuery ) )
	{
		FreeRequest( lpDbOverlappedPlus );
		return;
	}
#if __VER >= 15 // __GUILD_HOUSE
	GuildHouseDBMng->PostRequest( GUILDHOUSE_REMOVE, NULL, 0, idGuild );
#endif // __GUILD_HOUSE
#ifndef __NOLOG
	GUILDLOG_QUERYINFO qi( "L3" );
	qi.idGuild = idGuild;
	qi.idPlayer = idMaster;
	qi.pszState = "E";
	
	DBQryGuildLog( szQuery, qi );
	if( FALSE == pQueryLog->Exec( szQuery ) )
	{
		FreeRequest( lpDbOverlappedPlus );
		return;
	}
#endif // __NOLOG
	FreeRequest( lpDbOverlappedPlus );
}
	
void CDbManager::AddGuildMember( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long idPlayer, idGuild, DoidPlayer;
	ar >> idPlayer >> idGuild >> DoidPlayer;
	GUILD_QUERYINFO qi( "A2" );
	qi.idPlayer	= idPlayer;
	qi.idGuild	= idGuild;
	qi.nLevel		= GUD_ROOKIE;

	char szQuery[QUERY_SIZE]	= { 0,};
	DBQryGuild( szQuery, qi );

	if( FALSE == pQuery->Exec( szQuery ) )
	{
		FreeRequest( lpDbOverlappedPlus );
		return;
	}
#ifndef __NOLOG
	GUILDLOG_QUERYINFO LogQi( "L1" );
	LogQi.idGuild = idGuild;
	LogQi.idPlayer = idPlayer;
	LogQi.idDoPlayer = DoidPlayer;
	LogQi.pszState = "J";
	
	DBQryGuildLog( szQuery, LogQi );
	if( FALSE == pQueryLog->Exec( szQuery ) )
	{
		FreeRequest( lpDbOverlappedPlus );
		return;
	}
#endif // __NOLOG

	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::RemoveGuildMember( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long idPlayer, idGuild, DoidPlayer;
	ar >> idPlayer >> idGuild >> DoidPlayer;

	GUILD_QUERYINFO qi( "D2" );
	qi.idPlayer	= idPlayer;

	char szQuery[QUERY_SIZE]	= { 0,};
	DBQryGuild( szQuery, qi );

	if( FALSE == pQuery->Exec( szQuery ) )
	{
		FreeRequest( lpDbOverlappedPlus );
		return;
	}
#ifndef __NOLOG
	GUILDLOG_QUERYINFO LogQi( "L1" );
	LogQi.idGuild = idGuild;
	LogQi.idPlayer = idPlayer;
	LogQi.idDoPlayer = DoidPlayer;
	if( idPlayer == DoidPlayer )
		LogQi.pszState = "L";
	else
		LogQi.pszState = "R";
	
	DBQryGuildLog( szQuery, LogQi );
	if( FALSE == pQueryLog->Exec( szQuery ) )
	{
		FreeRequest( lpDbOverlappedPlus );
		return;
	}
#endif // __NOLOG
	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::UpdateGuildMemberLv( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long idPlayer;
	int nMemberLv;
	ar >> idPlayer >> nMemberLv;

	GUILD_QUERYINFO qi( "U2" );
	qi.idPlayer	= idPlayer;
	qi.nLevel	= nMemberLv;

	char szQuery[QUERY_SIZE]	= { 0,};
	DBQryGuild( szQuery, qi );

	if( FALSE == pQuery->Exec( szQuery ) )
	{
		FreeRequest( lpDbOverlappedPlus );
		return;
	}
	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::UpdateGuildClass( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long idPlayer;
	int nClass;
	ar >> idPlayer >> nClass;
	
	GUILD_QUERYINFO qi( "U9" );
	qi.idPlayer	= idPlayer;
	qi.nClass	= nClass;
	char szQuery[QUERY_SIZE]	= { 0,};
	DBQryGuild( szQuery, qi );
	
	if( FALSE == pQuery->Exec( szQuery ) )
	{
		FreeRequest( lpDbOverlappedPlus );
		return;
	}
	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::UpdateGuildNickName( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long idPlayer;
	char strNickName[MAX_GM_ALIAS] = { 0, };
	ar >> idPlayer;
	ar.ReadString( strNickName, MAX_GM_ALIAS );
	
	GUILD_QUERYINFO qi( "UA" );
	qi.idPlayer	= idPlayer;
	qi.pszGuild = strNickName;

	char szQuery[QUERY_SIZE]	= { 0,};
	DBQryGuild( szQuery, qi );
	
	if( FALSE == pQuery->Exec( szQuery ) )
	{
		FreeRequest( lpDbOverlappedPlus );
		return;
	}
	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::UpdateGuildMaster( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long idPlayer, idPlayer2;
	ar >> idPlayer >> idPlayer2;

	GUILD_QUERYINFO qi( "U2" );
	qi.idPlayer	= idPlayer;
	qi.nLevel	= GUD_ROOKIE;
	qi.nClass	= 0;

	char szQuery[QUERY_SIZE]	= { 0,};
	DBQryGuild( szQuery, qi );

	if( FALSE == pQuery->Exec( szQuery ) )
	{
		FreeRequest( lpDbOverlappedPlus );
		return;
	}

	qi.idPlayer	= idPlayer2;
	qi.nLevel	= GUD_MASTER;
	DBQryGuild( szQuery, qi );
	
	if( FALSE == pQuery->Exec( szQuery ) )
	{
		FreeRequest( lpDbOverlappedPlus );
		return;
	}

	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::OpenGuild( void )
{
//	0
	g_GuildMng.Clear();
	g_GuildWarMng.Clear();

//	1
	CQuery* pQuery	= new CQuery;
	if( pQuery->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) == FALSE )
	{
		char s[128];
		sprintf( s, "DB open failed: %s", DSN_NAME_CHARACTER01 );
		AfxMessageBox( s );
		SAFE_DELETE( pQuery );
		return;
	}

	char szSql[1024];		
	//  길드 가져오기 모두 다~~~
	DBQryGuild( szSql, "S1" );	// ORDER BY
	if( FALSE == pQuery->Exec( szSql ) )
	{
		Error( "sql error: %s", szSql );
		SAFE_DELETE( pQuery );
		return;
	}

	while( pQuery->Fetch() )
	{
		CGuild* pGuild	= new CGuild;
		
		pGuild->m_idGuild						= (u_long)pQuery->GetInt( "m_idGuild" );
		pGuild->m_nLevel						= pQuery->GetInt( "m_nLevel" );
		pGuild->m_dwContributionPxp				= (DWORD)pQuery->GetInt( "m_nGuildPxp" );
		pGuild->m_dwLogo						= (DWORD)pQuery->GetInt( "m_dwLogo" );
		//  권한
		pGuild->m_adwPower[GUD_MASTER]			= 0x000000FF;
		pGuild->m_adwPower[GUD_KINGPIN]			= (DWORD)pQuery->GetInt( "Lv_1" );
		pGuild->m_adwPower[GUD_CAPTAIN]			= (DWORD)pQuery->GetInt( "Lv_2" );
		pGuild->m_adwPower[GUD_SUPPORTER]		= (DWORD)pQuery->GetInt( "Lv_3" );
		pGuild->m_adwPower[GUD_ROOKIE]			= (DWORD)pQuery->GetInt( "Lv_4" );
		
		//  월급량
		pGuild->m_adwPenya[GUD_MASTER]			= (DWORD)pQuery->GetInt( "Pay_0" );
		pGuild->m_adwPenya[GUD_KINGPIN]			= (DWORD)pQuery->GetInt( "Pay_1" );
		pGuild->m_adwPenya[GUD_CAPTAIN]			= (DWORD)pQuery->GetInt( "Pay_2" );
		pGuild->m_adwPenya[GUD_SUPPORTER]		= (DWORD)pQuery->GetInt( "Pay_3" );
		pGuild->m_adwPenya[GUD_ROOKIE]			= (DWORD)pQuery->GetInt( "Pay_4" );
				
		pGuild->m_bActive						= ( pQuery->GetChar( "isuse" ) == 'F' );

		pQuery->GetStr( "m_szNotice", pGuild->m_szNotice );
		pQuery->GetStr( "m_szGuild", pGuild->m_szGuild );
		StringTrimRight( pGuild->m_szGuild );
		
		pGuild->m_nGoldGuild					= (DWORD)pQuery->GetInt( "m_nGuildGold" );
		pGuild->m_nWin	= pQuery->GetInt( "m_nWin" );
		pGuild->m_nSurrender	= pQuery->GetInt( "m_nSurrender" );
		pGuild->m_nLose	= pQuery->GetInt( "m_nLose" );
		g_GuildMng.AddGuild( pGuild );
	}
	// 길드 멤버 가지고 오기 ~~ 길드에 따른 멤버들의 정보
	DBQryGuild( szSql, "S2" );
	if( FALSE == pQuery->Exec( szSql ) )
	{
		Error( "sql error: %s", szSql );
		SAFE_DELETE( pQuery );
		return;
	}

	u_long idGuild;
	while( pQuery->Fetch() )
	{
		CGuildMember* pMember	= new CGuildMember;
		pMember->m_idPlayer			= (u_long)pQuery->GetInt( "m_idPlayer" );
		pMember->m_nMemberLv		= pQuery->GetInt( "m_nMemberLv" );
		pMember->m_nGiveGold		= pQuery->GetInt( "m_nGiveGold" );
		pMember->m_dwGivePxpCount	= (DWORD)pQuery->GetInt( "m_nGivePxp" );
		pMember->m_nPay				= pQuery->GetInt( "m_nPay" );
		pMember->m_nWin				= pQuery->GetInt( "m_nWin" );
		pMember->m_nLose			= pQuery->GetInt( "m_nLose" );
#if __VER < 11 // __SYS_PLAYER_DATA
		pMember->m_nJob				= pQuery->GetInt( "m_nJob" );
		pMember->m_dwSex			= (DWORD)pQuery->GetInt( "m_dwSex" );
		pMember->m_nLevel			= pQuery->GetInt( "m_nLevel" );
#endif	// __SYS_PLAYER_DATA

#ifdef __GUILDVOTE
		pMember->m_idSelectedVote	= pQuery->GetInt( "m_idVote" );
#endif
		pMember->m_nClass			= pQuery->GetInt( "m_nClass" );
		
		pQuery->GetStr( "m_szAlias", pMember->m_szAlias );		
		idGuild						= (u_long)pQuery->GetInt( "m_idGuild" );
		CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
		if( pGuild )
		{
			pGuild->AddMember( pMember );
			if( pMember->m_nMemberLv == GUD_MASTER )
				pGuild->m_idMaster	= pMember->m_idPlayer;
		}
		else
		{
			WriteLog( "OpenGuild(): Player's guild not found - %d, %d", pMember->m_idPlayer, idGuild );
			SAFE_DELETE( pMember );
		}
	}
#ifdef __GUILDVOTE
	//	 GUILD 투표 전부 가져오기  GUILD_VOTE_STR 'S1',	@iserverindex
	VOTE_QUERYINFO info("S1");
	DBQryVote( szSql, info );	
	if( pQuery->Exec( szSql ) )
	{
		VOTE_INSERTED_INFO info;

		BYTE cbStatus;
		BYTE cbCounts[4];

		while( pQuery->Fetch() )
		{
			info.idGuild	= (u_long)pQuery->GetInt( "m_idGuild" );			
			info.idVote		= (u_long)pQuery->GetInt( "m_idVote" );			

			CGuild* pGuild	= g_GuildMng.GetGuild( info.idGuild );
			if( pGuild == NULL )
			{
				WriteLog( "OpenGuild(): vote[%d] guild[%d] not found", info.idVote, info.idGuild );
				continue;
			}

			cbStatus =  pQuery->GetInt( "m_cbStatus" ); // cbStatus - 투표중:1, 투표완료:2  
			pQuery->GetStr( "m_szTitle", info.szTitle );	
			pQuery->GetStr( "m_szQuestion", info.szQuestion );	
			pQuery->GetStr( "m_szString1", info.szSelections[0] );	
			pQuery->GetStr( "m_szString2", info.szSelections[1] );	
			pQuery->GetStr( "m_szString3", info.szSelections[2] );	
			pQuery->GetStr( "m_szString4", info.szSelections[3] );	

			cbCounts[0]	= (BYTE)pQuery->GetInt( "m_cbCount1" );			
			cbCounts[1]	= (BYTE)pQuery->GetInt( "m_cbCount2" );			
			cbCounts[2]	= (BYTE)pQuery->GetInt( "m_cbCount3" );			
			cbCounts[3]	= (BYTE)pQuery->GetInt( "m_cbCount4" );			
			
			pGuild->AddVote( info, cbStatus == 2, cbCounts );
		}
	} 
	else
	{
		WriteLog( "OpenGuild(): VOTE QUERY ERROR");
		TRACE("ERROR: OpenGuild(): VOTE QUERY Exec()\n");
	}
#endif // __GUILDVOTE

	// 길드 랭킹정보 받아오기
	CGuildRank* pGuildRank = CGuildRank::Instance();
	pGuildRank->GetRanking( pQuery, szSql );

	WAR_QUERYINFO wqi( "S1" );
	DBQryWar( szSql, wqi );
	
	if( FALSE == pQuery->Exec( szSql ) )
	{
		Error( "sql error: %s", szSql );
		SAFE_DELETE( pQuery );
		return;
	}

	char sTime[13]	= { 0, };
	while( pQuery->Fetch() )
	{
		CGuildWar* pWar	= new CGuildWar;
		pWar->m_idWar	= (u_long)pQuery->GetInt( "m_idWar" );
		pWar->m_Decl.idGuild	= (u_long)pQuery->GetInt( "m_idGuild" );
		pWar->m_Decl.nSize	= (u_long)pQuery->GetInt( "m_nCount" );
		pWar->m_Decl.nSurrender	= (u_long)pQuery->GetInt( "m_nSurrender" );
		pWar->m_Decl.nDead	= (u_long)pQuery->GetInt( "m_nDeath" );
		pWar->m_Decl.nAbsent	= (u_long)pQuery->GetInt( "m_nAbsent" );
		pWar->m_Acpt.idGuild	= (u_long)pQuery->GetInt( "f_idGuild" );
		pWar->m_Acpt.nSize	= (u_long)pQuery->GetInt( "f_nCount" );
		pWar->m_Acpt.nSurrender	= (u_long)pQuery->GetInt( "f_nSurrender" );
		pWar->m_Acpt.nDead	= (u_long)pQuery->GetInt( "f_nDeath" );
		pWar->m_Acpt.nAbsent	= (u_long)pQuery->GetInt( "f_nAbsent" );
		pWar->m_nFlag	= pQuery->GetChar( "State" );
		pQuery->GetStr( "StartTime", sTime );
		GetStrTime( &pWar->m_time, sTime );
		g_GuildWarMng.AddWar( pWar );
		CGuild* pDecl	= g_GuildMng.GetGuild( pWar->m_Decl.idGuild );
		if( pDecl )
			pDecl->m_idEnemyGuild	= pWar->m_Acpt.idGuild;
		CGuild* pAcpt	= g_GuildMng.GetGuild( pWar->m_Acpt.idGuild );
		if( pAcpt )
			pAcpt->m_idEnemyGuild	= pWar->m_Decl.idGuild;
	}

	WAR_QUERYINFO wqi2( "S2" );
	DBQryWar( szSql, wqi2 );
	if( FALSE == pQuery->Exec( szSql ) )
	{
		Error( "sql error: %s", szSql );
		return;
	}
	if( pQuery->Fetch() )
		g_GuildWarMng.m_id	= pQuery->GetInt( "Max_m_idWar" );

	SAFE_DELETE( pQuery );
}


void CDbManager::UpdateGuildLogo( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long idGuild;
	DWORD dwLogo;
	ar >> idGuild >> dwLogo;

	GUILD_QUERYINFO info("U3");
	info.idGuild = idGuild;
	info.dwLogo = dwLogo;

	char szQuery[QUERY_SIZE]	= { 0,};
	DBQryGuild( szQuery, info);
	if( FALSE == pQuery->Exec( szQuery ) )
	{
		FreeRequest( lpDbOverlappedPlus );
		return;
	}
	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::UpdateGuildContribution( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	BYTE nLevelUp;  // 레벨업 1 : 0( 했으면 1 )
	LONG nMemberLv;
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	CONTRIBUTION_CHANGED_INFO cci;
	ar >> cci;
	ar >> nLevelUp;
	ar >> nMemberLv;

	GUILD_QUERYINFO info("U4");
	info.idGuild	= cci.idGuild;          // 길드 업뎃 
	info.nGuildPxp	= cci.dwGuildPxpCount;	// 길드 Pxp
	info.nGuildGold = cci.dwGuildPenya;		// 길드 Penya
	info.nLevel		= cci.nGuildLevel;		// 길드 레벨
	info.idPlayer	= cci.idPlayer;			// 길드멤버 업뎃 
	info.dwLv1		= cci.dwPenya;			// 공헌 Penya
	info.dwLv2		= cci.dwPxpCount;		// 공헌 Pxp

	char szQuery[QUERY_SIZE]	= { 0,};
	DBQryGuild( szQuery, info);
	if( FALSE == pQuery->Exec( szQuery ) )
	{
		FreeRequest( lpDbOverlappedPlus );
		return;
	}
#ifndef __NOLOG
	if( info.dwLv1 != 0 )	
	{
		// 돈을 넣었을경우
		GUILDLOG_QUERYINFO qi( "L2" );	// 길드 창고 로그
		qi.idGuild = info.idGuild;
		qi.idPlayer = info.idPlayer;
		qi.nGuildGold = info.nGuildGold;
		qi.nItem	= info.dwLv1;
		if( nLevelUp )
		{
			qi.pszState = "L";
		}
		else
		{
			qi.pszState = "I";
		}
		DBQryGuildLog( szQuery, qi );
		if( FALSE == pQueryLog->Exec( szQuery ) )
		{
			FreeRequest( lpDbOverlappedPlus );
			return;
		}
		GUILDLOG_QUERYINFO qi1( "L4" );	// 길드 공헌 로그
		qi1.idGuild = info.idGuild;
		qi1.idPlayer = info.idPlayer;
		qi1.nGuildGold = info.nGuildGold;
		qi1.nLevel = nMemberLv;
		qi1.nGuildLv = info.nLevel;
		qi1.nGuildPxp = info.nGuildPxp;
		if( nLevelUp )
		{
			qi1.pszState = "L";
		}
		else
		{
			qi1.pszState = "G";
		}
		DBQryGuildLog( szQuery, qi1 );
		if( FALSE == pQueryLog->Exec( szQuery ) )
		{
			FreeRequest( lpDbOverlappedPlus );
			return;
		}
	}
	else
	{
		// Pxp를 넣었을 경우
		GUILDLOG_QUERYINFO qi2( "L4" );
		qi2.idGuild = info.idGuild;
		qi2.idPlayer = info.idPlayer;
		qi2.nGuildGold = info.nGuildGold;
		qi2.nLevel = nMemberLv;
		qi2.nGuildLv = info.nLevel;
		qi2.nGuildPxp = info.nGuildPxp;
		if( nLevelUp )
		{
			qi2.pszState = "L";
		}
		else
		{
			qi2.pszState = "P";
		}
		DBQryGuildLog( szQuery, qi2 );
		if( FALSE == pQueryLog->Exec( szQuery ) )
		{
			FreeRequest( lpDbOverlappedPlus );
			return;
		}
	}
#endif // __NOLOG
	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::UpdateGuildNotice( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long idGuild;
	char szNotice[MAX_BYTE_NOTICE];
	ar >> idGuild;
	ar.ReadString( szNotice, MAX_BYTE_NOTICE );

	GUILD_QUERYINFO info("U5");
	info.idGuild = idGuild;
	info.pszNotice = szNotice;

	char szQuery[QUERY_SIZE]	= { 0,};
	DBQryGuild( szQuery, info);

	SQLINTEGER cbLen = SQL_NTS;
	if( pQuery->BindParameter( 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 127, 0, szNotice, 0, &cbLen ) == FALSE )
	{
		FreeRequest( lpDbOverlappedPlus );
		return;
	}
	
	if( FALSE == pQuery->Exec( szQuery ) )
	{
		FreeRequest( lpDbOverlappedPlus );
		return;
	}
	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::UpdateGuildAuthority( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long idGuild;
	DWORD adwAuthority[MAX_GM_LEVEL];

	ar >> idGuild;
	ar.Read( adwAuthority, sizeof(DWORD) * MAX_GM_LEVEL );
	
	GUILD_QUERYINFO info("U1");
	info.idGuild = idGuild;
	info.dwLv1 = adwAuthority[GUD_KINGPIN];
	info.dwLv2 = adwAuthority[GUD_CAPTAIN];
	info.dwLv3 = adwAuthority[GUD_SUPPORTER];
	info.dwLv4 = adwAuthority[GUD_ROOKIE];

	char szQuery[QUERY_SIZE]	= { 0,};
	DBQryGuild( szQuery, info);
	if( FALSE == pQuery->Exec( szQuery ) )
	{
		FreeRequest( lpDbOverlappedPlus );
		return;
	}
	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::UpdateGuildPenya( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long idGuild;
	DWORD adwPenya[MAX_GM_LEVEL];
	ar >> idGuild;
	ar.Read( adwPenya, sizeof(DWORD) * MAX_GM_LEVEL );
	
	GUILD_QUERYINFO info("U6");
	info.idGuild = idGuild;
	info.dwLogo = adwPenya[GUD_MASTER];
	info.dwLv1 = adwPenya[GUD_KINGPIN];
	info.dwLv2 = adwPenya[GUD_CAPTAIN];
	info.dwLv3 = adwPenya[GUD_SUPPORTER];
	info.dwLv4 = adwPenya[GUD_ROOKIE];

	char szQuery[QUERY_SIZE]	= { 0,};
	DBQryGuild( szQuery, info);
	if( FALSE == pQuery->Exec( szQuery ) )
	{
		FreeRequest( lpDbOverlappedPlus );
		return;
	}
	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::UpdateGuildRealPay( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long idGuild;
	DWORD dwGold;
	DWORD dwPay;
	ar >> idGuild >> dwGold >> dwPay;
		
	GUILD_QUERYINFO info("U7");
	info.idGuild = idGuild;
	info.dwLv1 = dwGold;

	char szQuery[QUERY_SIZE]	= { 0,};
	DBQryGuild( szQuery, info );
	if( FALSE == pQuery->Exec( szQuery ) )
	{
		FreeRequest( lpDbOverlappedPlus );
		return;
	}
#ifndef __NOLOG
	GUILDLOG_QUERYINFO qi( "L2" );
	qi.idGuild = idGuild;
	qi.nGuildGold = dwGold;
	qi.nItem	= dwPay;
	qi.pszState = "P";
	
	DBQryGuildLog( szQuery, qi );
	if( FALSE == pQueryLog->Exec( szQuery ) )
	{
		FreeRequest( lpDbOverlappedPlus );
		return;
	}
#endif // __NOLOG
	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::UpdateGuildSetName( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long idGuild;
	char szName[ MAX_G_NAME ];
	ar >> idGuild;
	ar.ReadString( szName, MAX_G_NAME );
	
	GUILD_QUERYINFO info("U8");
	info.idGuild = idGuild;
	info.pszGuild = szName;

	char szQuery[QUERY_SIZE]	= { 0,};
	DBQryGuild( szQuery, info );
	if( FALSE == pQuery->Exec( szQuery ) )
	{
		FreeRequest( lpDbOverlappedPlus );
		return;
	}
	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::OpenQueryGuildBank( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr	ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	// 2. 쿼리문을 생성후 실행한다.
	char szSql[128];
	sprintf(szSql, "GUILD_BANK_STR 'S1','0','%02d'", g_appInfo.dwSys);
	if( FALSE == pQuery->Exec( szSql ) )
	{
		FreeRequest( lpDbOverlappedPlus );
		return;
	}

	while( 1 )
	{
	// 3. 월드 서버에 전송할 패킷을 생성한다.
	BEFORESENDDUAL( ar2, PACKETTYPE_GUILD_BANK, DPID_UNKNOWN, DPID_UNKNOWN );

	// 4. 로딩할 길드창고 데이터를 스택에 생성한다.
//	CItemContainer<CItemElem>	GuildBank;	// 길드 창고
	int							nGoldGuild; // 길드 Credit
	int							nGuildId	= 0;
	int							nBufsize	= 0;
	int							nCount		= 0;
	u_long						ulOffSet	= ar2.GetOffset();

	// 5. 쿼리한 결과를 저장한다.
	ar2 << static_cast<int>(0);
	BOOL bFetch	= FALSE;
	while( bFetch = pQuery->Fetch() )
	{
		nCount++;
		CItemContainer<CItemElem>	GuildBank;	// 길드 창고
		GuildBank.SetItemContainer( ITYPE_ITEM, MAX_GUILDBANK );
//		GuildBank.Clear();
		
		nGuildId		= pQuery->GetInt( "m_idGuild" );
		nGoldGuild		= pQuery->GetInt( "m_nGuildGold" );
		
		int		CountStr		= 0;
		int		IndexItem		= 0;
		int		itemCount		= 0;
		
		char m_apIndex[512]		= { 0, };
		pQuery->GetStr( "m_apIndex", m_apIndex );
		VERIFY_GUILD_STRING( m_apIndex, g_GuildMng.GetGuild(nGuildId)->m_szGuild );
		while( '$' != m_apIndex[CountStr] )
		{
			GuildBank.m_apIndex[itemCount]	= (DWORD)GetIntFromStr( m_apIndex, &CountStr );
			itemCount++;
		}
		
		CountStr	= 0;
		int IndexObjIndex	= 0;
		char ObjIndex[512]	= {0,};
		pQuery->GetStr( "m_dwObjIndex", ObjIndex );
		VERIFY_GUILD_STRING( ObjIndex, g_GuildMng.GetGuild(nGuildId)->m_szGuild );
		while( '$' != ObjIndex[CountStr] )
		{
			GuildBank.m_apItem[IndexObjIndex].m_dwObjIndex	= (DWORD)GetIntFromStr( ObjIndex, &CountStr );
			IndexObjIndex++;
		}
		
		
		CountStr		= 0;
		IndexItem		= 0;
		char	Bank[7500]		= { 0, };
		pQuery->GetStr( "m_GuildBank", Bank );
		VERIFY_GUILD_STRING( Bank, g_GuildMng.GetGuild(nGuildId)->m_szGuild );
		while( '$' != Bank[CountStr] )
		{
			CItemElem BufItemElem;
			IndexItem = GetOneItem( &BufItemElem, Bank, &CountStr );
			if( IndexItem == -1 )
			{
				Error( "OpenQuery::GuildBank : << 프로퍼티 없음. %d, %d", nGuildId, BufItemElem.m_dwItemId );
			}
			else
			{
				if( IndexItem >= MAX_GUILDBANK )
				{
					Error( "OpenQueryGuildBank::GuildBank : << IndexItem %d, %d", nGuildId, IndexItem );
					Error( "GuildBank = %s", Bank );
					return;
				}
				GuildBank.m_apItem[IndexItem] = BufItemElem;
			}

		}

		CountStr	= 0;
		int nExtBank = 0;
		char ExtBank[2000] = {0,};
		pQuery->GetStr( "m_extGuildBank", ExtBank );
		VERIFYSTRING( ExtBank, g_GuildMng.GetGuild(nGuildId)->m_szGuild );
		while( '$' != ExtBank[CountStr] )
		{
			GuildBank.m_apItem[nExtBank].m_bCharged					= (BOOL)GetIntPaFromStr( ExtBank, &CountStr );
			if( GuildBank.m_apItem[nExtBank].m_bCharged != 1 )
				GuildBank.m_apItem[nExtBank].m_bCharged	= 0;
			GuildBank.m_apItem[nExtBank].m_dwKeepTime				= (DWORD)GetIntPaFromStr( ExtBank, &CountStr );
#if __VER >= 11 // __SYS_IDENTIFY
			GuildBank.m_apItem[nExtBank].SetRandomOptItemId( GetInt64PaFromStr( ExtBank, &CountStr ) );
#else	// __SYS_IDENTIFY
			GuildBank.m_apItem[nExtBank].SetRandomOptItemId( (DWORD)GetIntPaFromStr( ExtBank, &CountStr ) );
#endif	// __SYS_IDENTIFY
#if __VER >= 15 // __PETVIS
			GuildBank.m_apItem[nExtBank].m_bTranformVisPet = static_cast<BOOL>( GetIntPaFromStr( ExtBank, &CountStr ) );
#endif // __PETVIS

			++CountStr;
			++nExtBank;
		}
		
		CountStr	= 0;
		int nPirecingBank = 0;
		char PirecingBank[4000] = {0,};
		pQuery->GetStr( "m_GuildBankPiercing", PirecingBank );
		VERIFYSTRING( PirecingBank, g_GuildMng.GetGuild(nGuildId)->m_szGuild );
		while( '$' != PirecingBank[CountStr] )
		{
#if __VER >= 12 // __EXT_PIERCING
			LoadPiercingInfo( GuildBank.m_apItem[nPirecingBank], PirecingBank, &CountStr );
#else // __EXT_PIERCING
			int nItemIdCount = 0;
			GuildBank.m_apItem[nPirecingBank].SetPiercingSize( GetIntFromStr( PirecingBank, &CountStr ) );
			for( int i = 0 ; i < GuildBank.m_apItem[nPirecingBank].GetPiercingSize() ; ++i )
			{
				GuildBank.m_apItem[nPirecingBank].SetPiercingItem( nItemIdCount, (DWORD)GetIntFromStr( PirecingBank, &CountStr ) );
				++nItemIdCount;
			}
#endif // __EXT_PIERCING
			++nPirecingBank;
		}

		CountStr	= 0;
		int nGuildBankPet = 0;
#ifdef __PET_1024
		char szGuildBankPet[4200] = {0,};
#else	// __PET_1024
		char szGuildBankPet[2688] = {0,};
#endif	// __PET_1024
		pQuery->GetStr( "szGuildBankPet", szGuildBankPet );
		VERIFYSTRING( szGuildBankPet, g_GuildMng.GetGuild(nGuildId)->m_szGuild );
		while( '$' != szGuildBankPet[CountStr] )
		{
//			SAFE_DELETE( GuildBank.m_apItem[nGuildBankPet].m_pPet );
			BOOL bPet	= (BOOL)GetIntFromStr( szGuildBankPet, &CountStr );
			if( bPet )
			{
				CPet* pPet	= GuildBank.m_apItem[nGuildBankPet].m_pPet	= new CPet;
				BYTE nKind	= (BYTE)GetIntFromStr( szGuildBankPet, &CountStr );
				pPet->SetKind( nKind );
				BYTE nLevel	= (BYTE)GetIntFromStr( szGuildBankPet, &CountStr );
				pPet->SetLevel( nLevel );
				DWORD dwExp	= (DWORD)GetIntFromStr( szGuildBankPet, &CountStr );
				pPet->SetExp( dwExp );
				WORD wEnergy	= (WORD)GetIntFromStr( szGuildBankPet, &CountStr );
				pPet->SetEnergy( wEnergy );
#ifdef __PET_1024
				WORD wLife	= (WORD)GetIntPaFromStr( szGuildBankPet, &CountStr );
				pPet->SetLife( wLife );
				for( int i = PL_D; i <= pPet->GetLevel(); i++ )
				{
					BYTE nAvailLevel	= (BYTE)GetIntPaFromStr( szGuildBankPet, &CountStr );
					pPet->SetAvailLevel( i, nAvailLevel );
				}
				char szFmt[MAX_PET_NAME_FMT]	= { 0,};
				GetStrFromStr( szGuildBankPet, szFmt, &CountStr );
				char szName[MAX_PET_NAME]	= { 0,};
				GetDBFormatStr( szName, MAX_PET_NAME, szFmt );
				pPet->SetName( szName );
#else	// __PET_1024
				WORD wLife	= (WORD)GetIntFromStr( szGuildBankPet, &CountStr );
				pPet->SetLife( wLife );
				for( int i = PL_D; i <= pPet->GetLevel(); i++ )
				{
					BYTE nAvailLevel	= (BYTE)GetIntFromStr( szGuildBankPet, &CountStr );
					pPet->SetAvailLevel( i, nAvailLevel );
				}
#endif	// __PET_1024
			}
//			++CountStr;
			++nGuildBankPet;
		}

		ar2 << nGuildId;
		ar2 << nGoldGuild;
		GuildBank.Serialize(ar2);

		if( nCount >= 1000 )
			break;
	}
	// 패킷 헤더를 설정한다.
	BYTE* pBuf = ar2.GetBuffer( &nBufsize );

	*(UNALIGNED int*)( pBuf + ulOffSet )	= nCount;
	// 패킷을 월드서버에 전송한다.
	SEND( ar2, CDPTrans::GetInstance(), lpDbOverlappedPlus->dpid );

	if( bFetch == FALSE )
		break;
	}

	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::UpdateGuildBankUpdate( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr							ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	CItemContainer<CItemElem>	GuildBank;	// 길드 창고
	int							nGoldGuild; // 길드 Credit
	int							nGuildId	= 0;
	BYTE						cbUpdateContribution;
	u_long						idPlayer, idLogPlayer;
	int							nMode;		// GUILD_PUT_ITEM, GUILD_GET_ITEM, GUILD_PUT_PENYA, GUILD_GET_PENYA, GUILD_CLOAK
	DWORD						dwItemId;	// 아이템 아이디
	short						nItemCount; // 아이템 갯수
	int							nAbilityOption;	// 옵션
	SERIALNUMBER	iSerialNumber;	// 유니크 넘버
	DWORD						dwPenya;	// 거래 페냐량
	GuildBank.SetItemContainer( ITYPE_ITEM, MAX_GUILDBANK );

	ar >> nGuildId;
	ar >> nGoldGuild;
	GuildBank.Serialize(ar);
	ar >> cbUpdateContribution;	// 길드멤버 공헌페냐를 변경해야 하는가?
	ar >> idPlayer;				// 뱅크를 업뎃한 플레이어
	ar >> nMode;
	ar >> dwItemId;
	ar >> nAbilityOption;
	ar >> iSerialNumber;
	ar >> nItemCount;
	ar >> dwPenya;

	idLogPlayer = idPlayer;		// 로그에 남길 주체자
	if( cbUpdateContribution != 1 )	// 망토를 산경우에는 공헌도를 감소하지 않는다.
		idPlayer = 0;

	char NullStr[2]				= "$";
	ItemContainerStruct icsGuildBank;
	SaveGuildBank( &GuildBank, &icsGuildBank );

	char szQuery[QUERY_SIZE]	= { 0,};
	sprintf( szQuery, 
		"GUILD_BANK_STR 'U1','%06d','%02d','%d','%s','%s','%s','%07d','%s','%s','%s'",
		nGuildId, g_appInfo.dwSys, nGoldGuild, icsGuildBank.szIndex, icsGuildBank.szObjIndex, icsGuildBank.szItem, idPlayer,icsGuildBank.szExt, icsGuildBank.szPiercing, icsGuildBank.szPet );

	if( FALSE == pQuery->Exec( szQuery ) )
	{
		Error( "update guild bank:%s ", szQuery );
		return;
	}
#ifndef __NOLOG
	GUILDLOG_QUERYINFO qi( "L2" );
	qi.idGuild = nGuildId;
	qi.idPlayer = idLogPlayer;
	qi.nGuildGold = nGoldGuild;

	switch( nMode )
	{
	case GUILD_PUT_ITEM:
		qi.pszState = "A";		// 아이템 넣기
		qi.nItem	= dwItemId;
		qi.nAbilityOption = nAbilityOption;
		qi.iUniqueNo = iSerialNumber;
		qi.nItem_count = nItemCount;
		break;
	case GUILD_GET_ITEM:
		qi.pszState = "D";		// 아이템 빼기
		qi.nItem	= dwItemId;
		qi.nAbilityOption = nAbilityOption;
		qi.iUniqueNo = iSerialNumber;
		qi.nItem_count = nItemCount;
		break;
	case GUILD_CLOAK:
		qi.pszState = "C";		// 망토 생성
		qi.nItem	= dwItemId;
		qi.nAbilityOption = nAbilityOption;
		qi.iUniqueNo = iSerialNumber;
		qi.nItem_count = nItemCount;
		break;
	case GUILD_GET_PENYA:
		qi.pszState = "O";		// 돈 빼기
		qi.nItem	= dwPenya;
		break;
	case GUILD_QUERY_REMOVE_GUILD_BANK:
		qi.pszState	= "R";
		break;
	}

	qi.pszGuildBank		= icsGuildBank.szItem;

	DBQryGuildLog( szQuery, qi );
	if( FALSE == pQueryLog->Exec( szQuery ) )
	{
		FreeRequest( lpDbOverlappedPlus );
		return;
	}
#endif // __NOLOG
	FreeRequest( lpDbOverlappedPlus );
}

// TODO:try-catch
void CDbManager::AddGuildVote( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	int i;
	u_long idGuild;
	char szTitle[MAX_BYTE_VOTETITLE];
	char szQuestion[MAX_BYTE_VOTEQUESTION];
	char szSelections[4][MAX_BYTE_VOTESELECT];

	ar >> idGuild;
	ar.ReadString( szTitle, MAX_BYTE_VOTETITLE );
	ar.ReadString( szQuestion, MAX_BYTE_VOTEQUESTION );
	for( i=0; i<4; ++i )
		ar.ReadString( szSelections[i], MAX_BYTE_VOTESELECT );


	char szQuery[QUERY_SIZE]	= { 0,};
    sprintf( szQuery, 
		     "{call GUILD_VOTE_STR('A1', '%02d', '%06d', %d, ?, ?, ?, ?, ?, ?, %d)}",
			 g_appInfo.dwSys, idGuild, 0, 0);

	BOOL bOK[6];
	SQLINTEGER cbLen = SQL_NTS;
	bOK[0] = pQuery->BindParameter( 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, MAX_BYTE_VOTETITLE, 0, szTitle, 0, &cbLen );
	bOK[1] = pQuery->BindParameter( 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, MAX_BYTE_VOTEQUESTION, 0, szQuestion, 0, &cbLen );
	bOK[2] = pQuery->BindParameter( 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, MAX_BYTE_VOTESELECT, 0, szSelections[0], 0, &cbLen );
	bOK[3] = pQuery->BindParameter( 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, MAX_BYTE_VOTESELECT, 0, szSelections[1], 0, &cbLen );
	bOK[4] = pQuery->BindParameter( 5, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, MAX_BYTE_VOTESELECT, 0, szSelections[2], 0, &cbLen );
	bOK[5] = pQuery->BindParameter( 6, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, MAX_BYTE_VOTESELECT, 0, szSelections[3], 0, &cbLen );
	
	u_long idVote = 0;
	if( bOK[0] && bOK[1] && bOK[2] && bOK[3] && bOK[4] && bOK[5] )
	{
		if( pQuery->Exec( szQuery ) )
		{
			if( pQuery->Fetch() )
			{
				idVote = (u_long)pQuery->GetInt( "m_idVote" );
			}
		}
	}
	// 성공이던지, 실패이던지 패킷을 보낸다.
	VOTE_INSERTED_INFO	info;
	info.idGuild = idGuild;
	info.idVote = idVote;
	memcpy(info.szTitle, szTitle, MAX_BYTE_VOTETITLE);
	memcpy(info.szQuestion, szQuestion, MAX_BYTE_VOTEQUESTION);
	for( i=0; i<4; ++i )
		memcpy( info.szSelections[i], szSelections[i], MAX_BYTE_VOTESELECT );
	g_dpCoreSrvr.SendAddVoteResult( info );

	FreeRequest( lpDbOverlappedPlus );
}

// 투표제거 
void CDbManager::RemoveGuildVote( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long idVote;
	ar >> idVote;

	VOTE_QUERYINFO info("D1");
	info.idVote = idVote;

	char szQuery[QUERY_SIZE]	= { 0,};
	DBQryVote( szQuery, info );  
	pQuery->Exec( szQuery );
	FreeRequest( lpDbOverlappedPlus );
}

// 투표 종료 
void CDbManager::CloseGuildVote( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long idVote;
	ar >> idVote;

	VOTE_QUERYINFO info("U2");
	info.idVote = idVote;

	char szQuery[QUERY_SIZE]	= { 0,};
	DBQryVote( szQuery, info );  
	pQuery->Exec( szQuery );
	FreeRequest( lpDbOverlappedPlus );
}

//투표하기 
void CDbManager::CastGuildVote( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long idVote;
	BYTE cbSelection;
	ar >> idVote;
	ar >> cbSelection;	
	
	VOTE_QUERYINFO info("U1");
	info.idVote = idVote;
	info.cbSelection = cbSelection;

	char szQuery[QUERY_SIZE]	= { 0,};
	DBQryVote( szQuery, info );  
	pQuery->Exec( szQuery );
	FreeRequest( lpDbOverlappedPlus );
}



void CDbManager::AcptWar( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	TRACE( "ACPT_GUILD_WAR\n" );
	u_long idWar, idDecl, idAcpt;
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	ar >> idWar >> idDecl >> idAcpt;
	
//	GUILD_WAR_STR 'A1',@im_idGuild,@iserverindex,@im_idWar,@iState,@if_idGuild
	WAR_QUERYINFO wqi( "A1" );
	wqi.idGuild	= idDecl;
	wqi.idWar	= idWar;
	wqi.iState	= WF_WARTIME;
	wqi.f_idGuild	= idAcpt;
	char szQuery[QUERY_SIZE]	= { 0,};
	DBQryWar( szQuery, wqi );
	pQuery->Exec( szQuery );
	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::WarEnd( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	TRACE( "WAR_END\n" );
	u_long idWar, idDecl, idAcpt;
	int nWptDecl, nWptAcpt, nType;
	char GuildWarStartTime[15] = {0,};
	int nWinPointDecl, nWinPointAcpt, nGetPointDecl, nGetPointAcpt;
	
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	ar >> idWar >> idDecl >> idAcpt >> nWptDecl >> nWptAcpt >> nType;
	ar.ReadString( GuildWarStartTime, 15 );
	ar >> nWinPointDecl >> nWinPointAcpt >> nGetPointDecl >> nGetPointAcpt;
	
	//	GUILD_WAR_STR 'U3',@im_idGuild,@iserverindex,@im_idWar,@iState,@if_idGuild 
	WAR_QUERYINFO wqi( "U3" );
	wqi.idWar	= idWar;
	wqi.idGuild	= idDecl;
	wqi.nWinPoint	= nWptDecl;
	wqi.f_idGuild	= idAcpt;
	wqi.f_nWinPoint	= nWptAcpt;
	switch( nType )
	{
	case WR_DECL_GN:
		wqi.iState	= '1';
		break;
	case WR_DECL_SR:
		wqi.iState	= '2';
		break;
	case WR_ACPT_GN:
		wqi.iState	= '3';
		break;
	case WR_ACPT_SR:
		wqi.iState	= '4';
		break;
	default:
		wqi.iState	= '9';
		break;
	}
	char szQuery[QUERY_SIZE]	= { 0,};
	DBQryWar( szQuery, wqi );
	pQuery->Exec( szQuery );

#ifndef __NOLOG
//	@im_idGuild,@im_idPlayer(f_idPlayer),@iserverindex,@im_nGuildGold(m_nCurExp),@im_nLevel(m_nGetExp), 
//	@im_GuildLv(m_nCurPenya),@iGuildPoint(m_nGetPenya),'',@iState,@im_Item(m_idWar),'',0,0,0,@is_date
//	LOG_GUILD_STR 'L5','000001', '000001','01',100,10,100,10,'','1',9,'',0,0,0,'20040717123012' @iState : 1 승 2
	LPCTSTR szState[2];

	switch( nType )
	{
	case WR_DECL_GN:
		szState[0]	= "1";
		szState[1]	= "5";
		break;
	case WR_DECL_SR:
		szState[0]	= "2";
		szState[1]	= "6";
		break;
	case WR_ACPT_GN:
		szState[0]	= "5";
		szState[1]	= "1";
		break;
	case WR_ACPT_SR:
		szState[0]	= "6";
		szState[1]	= "2";
		break;
	default:
		szState[0]	= "9";
		szState[1]	= "9";
		break;
	}

	GUILDLOG_QUERYINFO qi( "L5" );
	qi.idGuild = idDecl;
	qi.idPlayer = idAcpt;
	qi.nGuildGold = nWinPointDecl;
	qi.nLevel = nGetPointDecl;
	qi.nGuildLv = 0;
	qi.nGuildPxp = 0;
	qi.nItem = idWar;
	qi.pszTime = GuildWarStartTime;
	qi.pszState = szState[0];

	DBQryGuildLog( szQuery, qi );
	pQueryLog->Exec( szQuery );
	qi.idGuild = idAcpt;
	qi.idPlayer = idDecl;
	qi.nGuildGold = nWinPointAcpt;
	qi.nLevel = nGetPointAcpt;
	qi.nGuildLv = 0;
	qi.nGuildPxp = 0;
	qi.pszState = (char*)wqi.iState;
	qi.nItem = idWar;
	qi.pszTime = GuildWarStartTime;
	qi.pszState = szState[1];
	
	DBQryGuildLog( szQuery, qi );
	pQueryLog->Exec( szQuery );
#endif // __NOLOG
	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::Surrender( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	TRACE( "SURRENDER\n" );
	u_long idWar, idPlayer, idGuild;
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	ar >> idWar >> idPlayer >> idGuild;

// GUILD_WAR_STR 'U1',@im_idGuild,@iserverindex,@im_idWar,@iState,@if_idGuild,@im_idPlayer
	WAR_QUERYINFO wqi( "U1" );
	wqi.idGuild	= idGuild;
	wqi.idWar	= idWar;
	wqi.idPlayer	= idPlayer;
	char szQuery[QUERY_SIZE]	= { 0,};
	DBQryWar( szQuery, wqi );
	pQuery->Exec( szQuery );
	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::WarDead( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	u_long idWar, idGuild;
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	ar >> idWar >> idGuild;

//	GUILD_WAR_STR 'U2',@im_idGuild,@iserverindex,@im_idWar,@iState,@if_idGuild (m_idPlayer)
	WAR_QUERYINFO wqi( "U2" );
	wqi.idWar	= idWar;
	wqi.idGuild	= idGuild;

	char szQuery[QUERY_SIZE]	= { 0,};
	DBQryWar( szQuery, wqi );
	pQuery->Exec( szQuery );
	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::WarMasterAbsent( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	u_long idWar, idGuild;
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	ar >> idWar >> idGuild;

//	GUILD_WAR_STR 'U4',@im_idGuild,@iserverindex,@im_idWar,@iState,@if_idGuild (m_idPlayer)
	WAR_QUERYINFO wqi( "U4" );
	wqi.idWar	= idWar;
	wqi.idGuild	= idGuild;
	char szQuery[QUERY_SIZE]	= { 0,};
	DBQryWar( szQuery, wqi );
	pQuery->Exec( szQuery );
	FreeRequest( lpDbOverlappedPlus );
}


void CDbManager::UpdateGuildRanking( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr	ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	// 길드 랭크 정보를 받아온다.
	CGuildRank* pGuildRank = CGuildRank::Instance();

	char szQuery[QUERY_SIZE]	= { 0,};
	pGuildRank->GetRanking( pQuery, szQuery );
	
	// 길드 랭크 정보가 Refresh 되었을때..
	// Core서버에게 정보를 보내준다.
	CDPTrans::GetInstance()->SendUpdateGuildRankFinish();

	FreeRequest( lpDbOverlappedPlus );
}


void CDbManager::UpdateGuildRankingDB( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr	ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	
	// 길드 랭크 정보를 받아온다.
	CGuildRank* pGuildRank = CGuildRank::Instance();

	char szQuery[QUERY_SIZE]	= { 0,};

	pGuildRank->RankingDBUpdate( pQuery, szQuery );
	pGuildRank->GetRanking( pQuery, szQuery );
	
	// 길드 랭크 정보가 Refresh 되었을때..
	// Core서버에게 정보를 보내준다.
	CDPTrans::GetInstance()->SendUpdateGuildRankFinish();
	
	FreeRequest( lpDbOverlappedPlus );
}


// 쪽지를 ar에 기록한다.
void CDbManager::WriteTag(CAr &ar, int count, const TAG_ENTRY* tags)
{
	short nTagCount = count;
	ar << nTagCount;

	const TAG_ENTRY * pEntry;
	for( short i=0; i<nTagCount; ++i )
	{
		pEntry = &tags[i];

		ar << pEntry->idFrom;					// 보낸사람 
		ar << pEntry->dwDate;					// 보낸시각 
		ar.WriteString( pEntry->szString );		// 쪽지 내용 
	}
}

int CDbManager::SelectTag( CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus, u_long uidPlayer, TAG_ENTRY* tags)
{
	// TAG_STR 'S1','000001','01' 
	char szQuery[QUERY_SIZE]	= { 0,};

	sprintf( szQuery,
		"TAG_STR 'S1','%07d','%02d'",
		uidPlayer, g_appInfo.dwSys );
	
	if( FALSE == qry->Exec( szQuery ) )
	{
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );
		return 0;
	}

	int count = 0;			
	while( qry->Fetch() )
	{
		TAG_ENTRY *pEntry = &tags[count];

		pEntry->idFrom = qry->GetInt( "f_idPlayer" );	// 보낸사람 
		pEntry->dwDate = qry->GetInt( "CreateTime" );	// 보낸시각 
		qry->GetStr( "m_Message", pEntry->szString );	// 메세지 

		if( ++count >= MAX_TAGS )
			break;
	}
	return count;
}

void CDbManager::InsertTag( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	u_long idFrom;
	u_long idTo;
	char szString[256];
	arRead >> idFrom >> idTo;
	arRead.ReadString( szString, 256 );

	TRACE( "CDbManager::InsertTag %d %d %s\n", idFrom, idTo, szString );
	//TAG_STR 'A1','000001','01','000002','안녕하세요?'
	
	char szQuery[QUERY_SIZE]	= { 0,};
	sprintf( szQuery,
		"{call TAG_STR('A1', '%07d', '%02d', '%07d', ?)}", idTo, g_appInfo.dwSys, idFrom);

	SQLINTEGER cbLen = SQL_NTS;
	if( qry->BindParameter( 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 256, 0, szString, 0, &cbLen ) == FALSE )
	{
		FreeRequest( lpDbOverlappedPlus );
		return;
	}

	if( FALSE == qry->Exec( szQuery ) )
	{
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );
		TRACE("CDbManager::InsertTag -> qry->Exec %s\n", szQuery);
		FreeRequest( lpDbOverlappedPlus );
		return;
	}

	if( qry->Fetch() )
	{
		int nError = qry->GetInt( "fError" );
		if (nError == 1)	
			g_dpCoreSrvr.SendTagResult(idFrom, 0);	// 쪽지가 20개를 초과했어요
		else
			g_dpCoreSrvr.SendTagResult(idFrom, 1);	// 성공 
	}
	FreeRequest( lpDbOverlappedPlus );
}

#ifdef __S_RECOMMEND_EVE
void CDbManager::RecommendEve( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	char szAccount[MAX_ACCOUNT];
	u_long idPlayer;
	LONG nLevel;
	BYTE nSex;
	int nValue;
	
	arRead.ReadString( szAccount, MAX_ACCOUNT );
	arRead >> idPlayer >> nLevel >> nSex >> nValue;

#ifndef __GETMAILREALTIME
	return;
#endif // __GETMAILREALTIME

	char szQuery[QUERY_SIZE]	= { 0,};
	sprintf( szQuery, "MAIL_STR_REALTIME 'I1', '%02d', %d, %d, '%07d', %d, '%s', %d", g_appInfo.dwSys, nSex, nValue, idPlayer, nLevel, szAccount, time(NULL) );

	if( FALSE == qry->Exec( szQuery ) )
	{
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );
		TRACE("CDbManager::RecommendEve -> qry->Exec %s\n", szQuery);
		FreeRequest( lpDbOverlappedPlus );
		return;
	}

	if( qry->Fetch() )
	{
	}
	FreeRequest( lpDbOverlappedPlus );
}
#endif // __S_RECOMMEND_EVE

void CDbManager::SchoolReport( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	TRACE( "SCHOOL_REPORT\n" );
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	SCHOOL_ENTRY school[MAX_SCHOOL];
	ar.Read( &school, sizeof(SCHOOL_ENTRY) * MAX_SCHOOL );
/*
	[200407_1_SCHOOL_TBL]  저장하기
	 ex ) 
	 [200407_1_SCHOOL_STR] @iranking,@im_szGuild,@isurvivor
	 [200407_1_SCHOOL_STR] 1,'대원외국어고등학교',12
*/
	for( int i = 0; i < MAX_SCHOOL; i++ )
	{
		if( lstrlen( school[i].lpName ) > 0 )
		{
			char szQuery[QUERY_SIZE]	= { 0,};
			sprintf( szQuery,
				"EVENT_FINAL_STR %d, '%s', %d",
				( i + 1 ), school[i].lpName, school[i].nSurvivor );
			if( FALSE == pQuery->Exec( szQuery ) )
				WriteLog( "%s", szQuery );
		}
	}
	FreeRequest( lpDbOverlappedPlus );
}

// szDst에 comma를 붙이고 16진수로 변환된 문자열을 붙인다.
void CDbManager::SetStrDBFormat( char* szDst, const char* szSrc )
{
	char* pCur = szDst + strlen( szDst );
	*pCur++ = ',';

	while( *szSrc )
	{
		sprintf( pCur, "%2X",  (BYTE)*szSrc );
		pCur += 2;
		szSrc++;
	}
	*pCur = '\0';
}

inline void CDbManager::DBQryGuild( char* szSql, const char* szType, u_long idPlayer, u_long idGuild, const char* szGuild )
{
	sprintf( szSql, "GUILD_STR '%s', '%07d', '%02d', '%06d', '%s'", szType, idPlayer, g_appInfo.dwSys, idGuild, szGuild );
}

void CDbManager::DBQryGuild( char* szSql, const GUILD_QUERYINFO& info )
{
	if( info.pszType[0] == 'U' && info.pszType[1] == '5' )
	{
		sprintf( szSql, "{ call GUILD_STR('%s',"
			"'%07d','%02d','%06d','%s',%d,%d,%d,%d,"
			"%d,%d,%d,%d,%d,%d,"
			"%d,?,%d) }", 
			info.pszType,
			info.idPlayer, g_appInfo.dwSys, info.idGuild, info.pszGuild,info.dwLv1, info.dwLv2, info.dwLv3, info.dwLv4,
			info.nLevel, info.nGuildGold, info.nGuildPxp, info.nWin, info.nLose, info.nSurrender,
			info.dwLogo, info.nClass );
		
	}
	else
	{
		sprintf( szSql, "GUILD_STR '%s',"
			"@im_idPlayer='%07d',@iserverindex='%02d',@im_idGuild='%06d',@im_szGuild='%s',@iLv_1=%d,@iLv_2=%d,@iLv_3=%d,@iLv_4=%d,"
			"@im_nLevel=%d,@im_nGuildGold=%d,@im_nGuildPxp=%d,@im_nWin=%d,@im_nLose=%d,@im_nSurrender=%d,"
			"@im_dwLogo=%d,@im_szNotice='%s',@im_nClass=%d", 
			info.pszType,
			info.idPlayer, g_appInfo.dwSys, info.idGuild, info.pszGuild,info.dwLv1, info.dwLv2, info.dwLv3, info.dwLv4,
			info.nLevel, info.nGuildGold, info.nGuildPxp, info.nWin, info.nLose, info.nSurrender,
			info.dwLogo, info.pszNotice, info.nClass
			);
		
	}


	TRACE("%s\n", szSql);
}

void CDbManager::DBQryVote( char* szSql, const VOTE_QUERYINFO& info )
{
	sprintf( szSql, "GUILD_VOTE_STR '%s',@iserverindex='%02d',@im_idGuild='%06d',@im_idVote=%d,"
		            "@im_szTitle='%s',@im_szQuestion='%s',@im_szString1='%s',@im_szString2='%s',@im_szString3='%s',@im_szString4='%s',@im_cbCount=%d",
			info.pszType, g_appInfo.dwSys, info.idGuild, info.idVote, 
			info.pszTitle, info.pszQuestion, 
			info.pszSelections[0], info.pszSelections[1], info.pszSelections[2], info.pszSelections[3],
			info.cbSelection );
	TRACE("%s\n", szSql);
}

void CDbManager::DbQryMail( char* szSql, const MAIL_QUERYINFO& info )
{
	sprintf( szSql, "MAIL_STR '%s', @nMail=%d, @serverindex='%02d', @idReceiver='%07d', @idSender='%07d', @nGold=%d, @tmCreate=%d, @byRead=%d, @szTitle='%s', @szText='%s',"
		"@dwItemId=%d, @nItemNum=%d, @nRepairNumber=%d, @nHitPoint=%d, @nMaxHitPoint=%d, @nMaterial=%d, @byFlag=%d, @dwSerialNumber=%d,"
		"@nOption=%d, @bItemResist=%d, @nResistAbilityOption=%d, @idGuild=%d, @nResistSMItemId=%d, @bCharged=%d, @dwKeepTime=%d,"
		"@nRandomOptItemId=%I64d,"
		"@nPiercedSize=%d, @dwItemId1=%d, @dwItemId2=%d, @dwItemId3=%d, @dwItemId4=%d"
		", @bPet=%d, @nKind=%d, @nLevel=%d, @dwExp=%d, @wEnergy=%d, @wLife=%d,"
		"@anAvailLevel_D=%d, @anAvailLevel_C=%d, @anAvailLevel_B=%d, @anAvailLevel_A=%d, @anAvailLevel_S=%d"
		", @dwItemId5=%d"
		,info.pszType, info.nMail, g_appInfo.dwSys, info.idReceiver, info.idSender, info.nGold, info.tmCreate, info.byRead, info.szTitle, info.szText, 
		info.dwItemId, info.nItemNum, info.nRepairNumber, info.nHitPoint, info.nMaxHitPoint, info.nMaterial, info.byFlag, info.iSerialNumber,
		info.nOption, info.bItemResist, info.nResistAbilityOption, info.idGuild, info.nResistSMItemId, info.bCharged, info.dwKeepTime,
		info.iRandomOptItemId,
		info.nPiercedSize, info.dwItemId1, info.dwItemId2, info.dwItemId3, info.dwItemId4
		, info.bPet, info.nKind, info.nLevel, info.dwExp, info.wEnergy, info.wLife,
		info.anAvailLevel[PL_D], info.anAvailLevel[PL_C], info.anAvailLevel[PL_B], info.anAvailLevel[PL_A], info.anAvailLevel[PL_S]
		,info.dwItemId5
		);
}

void CDbManager::MakeQueryAddMail( char* szSql, CMail* pMail, u_long idReceiver )
{
	CItemElem item;
	CPet pet;
	BOOL bPet	= FALSE;
	if( pMail->m_pItemElem )
	{
		item	= *pMail->m_pItemElem;
		if( pMail->m_pItemElem->m_pPet )
		{
			pet		= *pMail->m_pItemElem->m_pPet;
			bPet	= TRUE;
		}
	}
	else
		item.m_nItemNum = 0;

#if __VER >= 12 // __EXT_PIERCING
	if( item.GetProp() && item.GetProp()->IsUltimate() )
	{
		sprintf( szSql, "{call MAIL_STR('A1', %d, '%02d', '%07d', '%07d', %d, %d, %d, ?, ?,"
			"%d, %d, %d, %d, %d, %d, %d, %d,"
			"%d, %d, %d, %d, %d, %d, %d,"
			"%I64d,"
			"%d, %d, %d, %d, %d"
			",%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d"
			",%d"
			",%d, %d, %d, %d, %d"	// 추가 피어싱 아이템
			",%d, %d, %d, %d, %d"	// 추가 피어싱 아이템
			",%d"	// 추가 피어싱 사이즈
			")}",

			pMail->m_nMail, g_appInfo.dwSys, idReceiver, pMail->m_idSender, pMail->m_nGold, pMail->m_tmCreate, pMail->m_byRead,
			item.m_dwItemId, item.m_nItemNum, item.m_nRepairNumber, item.m_nHitPoint, item.m_nRepair, 0, item.m_byFlag, item.GetSerialNumber(),
			item.GetOption(), item.m_bItemResist, item.m_nResistAbilityOption, item.m_idGuild, item.m_nResistSMItemId, item.m_bCharged, item.m_dwKeepTime,
			item.GetRandomOptItemId(),
			item.GetUltimatePiercingSize(), item.GetUltimatePiercingItem( 0 ), item.GetUltimatePiercingItem( 1 ), item.GetUltimatePiercingItem( 2 ), item.GetUltimatePiercingItem( 3 ),
			bPet, pet.GetKind(), pet.GetLevel(), pet.GetExp(), pet.GetEnergy(), pet.GetLife(),
			pet.GetAvailLevel( PL_D ), pet.GetAvailLevel( PL_C ), pet.GetAvailLevel( PL_B ), pet.GetAvailLevel( PL_A ), pet.GetAvailLevel( PL_S )
			, item.GetUltimatePiercingItem( 4 )
			, item.GetPiercingItem( 0 ), item.GetPiercingItem( 1 ), item.GetPiercingItem( 2 ), item.GetPiercingItem( 3 ),  item.GetPiercingItem( 4 )
			, item.GetPiercingItem( 5 ), item.GetPiercingItem( 6 ), item.GetPiercingItem( 7 ), item.GetPiercingItem( 8 ),  item.GetPiercingItem( 9 )
			, item.GetPiercingSize()
			);
	}
	else
	{
		sprintf( szSql, "{call MAIL_STR('A1', %d, '%02d', '%07d', '%07d', %d, %d, %d, ?, ?,"
			"%d, %d, %d, %d, %d, %d, %d, %d,"
			"%d, %d, %d, %d, %d, %d, %d,"
			"%I64d,"
			"%d, %d, %d, %d, %d"
			",%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d"
			",%d"
			",%d, %d, %d, %d, %d"	// 추가 피어싱 아이템
			",%d, %d, %d, %d, %d"	// 추가 피어싱 아이템
			",%d"	// 추가 피어싱 사이즈
			")}",

			pMail->m_nMail, g_appInfo.dwSys, idReceiver, pMail->m_idSender, pMail->m_nGold, pMail->m_tmCreate, pMail->m_byRead,
			item.m_dwItemId, item.m_nItemNum, item.m_nRepairNumber, item.m_nHitPoint, item.m_nRepair, 0, item.m_byFlag, item.GetSerialNumber(),
			item.GetOption(), item.m_bItemResist, item.m_nResistAbilityOption, item.m_idGuild, item.m_nResistSMItemId, item.m_bCharged, item.m_dwKeepTime,
			item.GetRandomOptItemId(),
			item.GetPiercingSize(), item.GetPiercingItem( 0 ), item.GetPiercingItem( 1 ), item.GetPiercingItem( 2 ), item.GetPiercingItem( 3 ),
			bPet, pet.GetKind(), pet.GetLevel(), pet.GetExp(), pet.GetEnergy(), pet.GetLife(),
			pet.GetAvailLevel( PL_D ), pet.GetAvailLevel( PL_C ), pet.GetAvailLevel( PL_B ), pet.GetAvailLevel( PL_A ), pet.GetAvailLevel( PL_S )
			, item.GetPiercingItem( 4 )
			, item.GetPiercingItem( 5 ), item.GetPiercingItem( 6 ), item.GetPiercingItem( 7 ), item.GetPiercingItem( 8 ), item.GetPiercingItem( 9 )
			, 0, 0, 0, 0, 0
			, 0
			);
	}
#else // __EXT_PIERCING
	sprintf( szSql, "{call MAIL_STR('A1', %d, '%02d', '%07d', '%07d', %d, %d, %d, ?, ?,"
		"%d, %d, %d, %d, %d, %d, %d, %d,"
		"%d, %d, %d, %d, %d, %d, %d,"
		"%I64d,"
		"%d, %d, %d, %d, %d"
		",%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d"
		",%d"
		")}",

		pMail->m_nMail, g_appInfo.dwSys, idReceiver, pMail->m_idSender, pMail->m_nGold, pMail->m_tmCreate, pMail->m_byRead,
		item.m_dwItemId, item.m_nItemNum, item.m_nRepairNumber, item.m_nHitPoint, item.m_nRepair, 0, item.m_byFlag, item.GetSerialNumber(),
		item.GetOption(), item.m_bItemResist, item.m_nResistAbilityOption, item.m_idGuild, item.m_nResistSMItemId, item.m_bCharged, item.m_dwKeepTime,
		item.GetRandomOptItemId(),
		item.GetPiercingSize(), item.GetPiercingItem( 0 ), item.GetPiercingItem( 1 ), item.GetPiercingItem( 2 ), item.GetPiercingItem( 3 ),
		bPet, pet.GetKind(), pet.GetLevel(), pet.GetExp(), pet.GetEnergy(), pet.GetLife(),
		pet.GetAvailLevel( PL_D ), pet.GetAvailLevel( PL_C ), pet.GetAvailLevel( PL_B ), pet.GetAvailLevel( PL_A ), pet.GetAvailLevel( PL_S )
		,item.GetPiercingItem( 4 )
		);
#endif // __EXT_PIERCING
}

void CDbManager::DBQryGuildLog( char* szSql, const GUILDLOG_QUERYINFO& info )
{
	sprintf( szSql, "LOG_GUILD_STR '%s', @im_idGuild='%06d', @im_idPlayer='%07d', @iserverindex='%02d',"
		"@im_nGuildGold=%d, @im_nLevel=%d, @im_GuildLv=%d, @iGuildPoint=%d,"
		"@iDo_m_idPlayer='%07d', @iState='%s', @im_Item=%d, @im_GuildBank='%s',"
		"@im_nAbilityOption=%d, @iItem_count=%d, @iItem_UniqueNo=%d, @is_date='%s'",
		info.pszType, info.idGuild, info.idPlayer, g_appInfo.dwSys,
		info.nGuildGold, info.nLevel, info.nGuildLv, info.nGuildPxp,
		info.idDoPlayer, info.pszState, info.nItem, info.pszGuildBank,
		info.nAbilityOption, info.nItem_count, info.iUniqueNo, info.pszTime );

	if( strlen(szSql) > 8012 )
		TRACE("%s\n", szSql);
}


void CDbManager::DBQryWar( char* szSql, const WAR_QUERYINFO & info )
{
	sprintf( szSql, "GUILD_WAR_STR '%s', @im_idGuild='%06d', @iserverindex='%02d', @im_idWar='%06d', @iState='%c', @if_idGuild='%06d', @im_idPlayer='%07d', @im_nWinPoint = %d, @if_nWinPoint = %d",
		info.pszType, info.idGuild, g_appInfo.dwSys, info.idWar, info.iState, info.f_idGuild, info.idPlayer, info.nWinPoint, info.f_nWinPoint );
}


void CDbManager::QueryGuildQuest( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	TRACE( "PACKETTYPE_QUERYGUILDQUEST\n" );
//	GUILD_QUEST_STR 'S1', @im_idGuild,@iserverindex
	char	pszSql[128];
	sprintf( pszSql, "GUILD_QUEST_STR 'S1', @im_idGuild = '', @iserverindex = '%02d'", g_appInfo.dwSys );
	if( !pQuery->Exec( pszSql ) )
	{
		FreeRequest( lpDbOverlappedPlus );
		return;
	}

	int nCount	= 0;
	u_long idGuild	= 0, idCurr;
	GUILDQUEST pQuest[MAX_GUILD_QUEST];
	BYTE nQuestSize;

	BEFORESENDDUAL( ar, PACKETTYPE_QUERYGUILDQUEST, DPID_UNKNOWN, DPID_UNKNOWN );
	u_long uOffset	= ar.GetOffset();
	ar << static_cast<int>( 0 );	// nCount

	while( pQuery->Fetch() )
	{
		idCurr	= (u_long)pQuery->GetInt( "m_idGuild" );
		if( idCurr != idGuild )
		{
			if( idGuild != 0 )
			{
				ar << idGuild << nQuestSize;
				ar.Write( (void*)pQuest, sizeof(GUILDQUEST) * nQuestSize );
				nCount++;
			}
			nQuestSize	= 0;
			idGuild		= idCurr;
		}
		pQuest[nQuestSize].nId	= pQuery->GetInt( "n_Id" );
		pQuest[nQuestSize].nState	= pQuery->GetInt( "nState" );
		nQuestSize++;
	}

	if( idGuild != 0 )
	{
		ar << idGuild << nQuestSize;
		ar.Write( (void*)pQuest, sizeof(GUILDQUEST) * nQuestSize );
		nCount++;
	}

	int nBufferSize;
	BYTE* pBuffer	= ar.GetBuffer( &nBufferSize );
	*(UNALIGNED int*)( pBuffer + uOffset )	= nCount;
	SEND( ar, CDPTrans::GetInstance(), lpDbOverlappedPlus->dpid );

	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::InsertGuildQuest( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	TRACE( "PACKETTYPE_SETGUILDQUEST\n" );
//	GUILD_QUEST_STR 'A1', @im_idGuild,@iserverindex,@in_Id
//	GUILD_QUEST_STR 'A1', '000001','01',0
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	u_long idGuild;
	int nId;
	ar >> idGuild >> nId;

	char	pszSql[128];
	sprintf( pszSql, "GUILD_QUEST_STR 'A1', @im_idGuild = '%06d', @iserverindex = '%02d', @in_Id = %d",
		idGuild, g_appInfo.dwSys, nId );

	if( !pQuery->Exec( pszSql ) )
	{
		FreeRequest( lpDbOverlappedPlus );
		return;
	}
	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::UpdateGuildQuest( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	TRACE( "PACKETTYPE_UPDATEGUILDQUEST\n" );
//	GUILD_QUEST_STR 'U1', @im_idGuild,@iserverindex,@in_Id,@in_State
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	u_long idGuild;
	int nId, nState;
	ar >> idGuild >> nId >> nState;
	
	char	pszSql[128];
	sprintf( pszSql, "GUILD_QUEST_STR 'U1', @im_idGuild = '%06d', @iserverindex = '%02d', @in_Id = %d, @in_State = %d",
		idGuild, g_appInfo.dwSys, nId, nState );

	if( !pQuery->Exec( pszSql ) )
	{
		FreeRequest( lpDbOverlappedPlus );
		return;
	}
	FreeRequest( lpDbOverlappedPlus );
}


void CDbManager::GuildThread( void )
{
	CQuery* pQuery	= new CQuery;
	if( FALSE == pQuery->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
	{
		WriteLog( "%s, %d GuildDB Connect", __FILE__, __LINE__ );
		SAFE_DELETE( pQuery );
		return;
	}

#ifndef __NOLOG
	CQuery* pQueryLog	= new CQuery;
#else	// __NOLOG
	CQuery* pQueryLog	= NULL;
#endif	// __NOLOG

#ifndef __NOLOG
	if( FALSE == pQueryLog->Connect( 3, DSN_NAME_LOG, DB_ADMIN_ID_LOG, DB_ADMIN_PASS_LOG ) )
	{
		WriteLog( "%s, %d", __FILE__, __LINE__ );
		SAFE_DELETE( pQuery );
		SAFE_DELETE( pQueryLog );
		return;
	}
#endif	// __NOLOG

	SetEvent( s_hHandle );

	BOOL bReturnValue	= FALSE;
	DWORD dwBytesTransferred	= 0;
	DWORD dwCompletionKey		= 0;
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus	= NULL;
	
	while( true )
	{
		bReturnValue	= GetQueuedCompletionStatus( m_hIOCPGuild, &dwBytesTransferred, &dwCompletionKey, (LPOVERLAPPED*)&lpDbOverlappedPlus, INFINITE );
		if( FALSE == bReturnValue )
			continue;

		if( dwBytesTransferred == 0 )
		{
			SAFE_DELETE( pQuery );
			SAFE_DELETE( pQueryLog );
			return;
		}

		if( dwCompletionKey != 0 && lpDbOverlappedPlus == NULL )
		{
			CDPTrans::GetInstance()->SendAllGuildCombat( (DPID)dwCompletionKey );
			continue;
		}
		switch( lpDbOverlappedPlus->nQueryMode )
		{
			case CREATE_GUILD:
				CreateGuild( pQuery, pQueryLog, lpDbOverlappedPlus );
				break;
			case DESTROY_GUILD:
				DestroyGuild( pQuery, pQueryLog, lpDbOverlappedPlus );
				break;
			case ADD_GUILD_MEMBER:
				AddGuildMember( pQuery, pQueryLog, lpDbOverlappedPlus );
				break;
			case REMOVE_GUILD_MEMBER:
				RemoveGuildMember( pQuery, pQueryLog, lpDbOverlappedPlus );
				break;
			case UPDATE_GUILD_MEMBER_LEVEL:
				UpdateGuildMemberLv( pQuery, lpDbOverlappedPlus );
				break;
			case UPDATE_GUILD_CLASS:
				UpdateGuildClass( pQuery, lpDbOverlappedPlus );
				break;
			case UPDATE_GUILD_NICKNAME:
				UpdateGuildNickName( pQuery, lpDbOverlappedPlus );
				break;
			case QM_UPDATEGUILDMASTER:
				UpdateGuildMaster( pQuery, lpDbOverlappedPlus );
				break;
			case UPDATE_GUILD_LOGO:
				UpdateGuildLogo( pQuery, lpDbOverlappedPlus );
				break;
			case UPDATE_GUILD_CONTRIBUTION:
				UpdateGuildContribution( pQuery, pQueryLog, lpDbOverlappedPlus ); 
				break;
			case UPDATE_GUILD_NOTICE:
				UpdateGuildNotice( pQuery, lpDbOverlappedPlus );
				break;
			case UPDATE_GUILD_AUTHORITY:
				UpdateGuildAuthority( pQuery, lpDbOverlappedPlus );
				break;
			case UPDATE_GUILD_PENYA:
				UpdateGuildPenya( pQuery, lpDbOverlappedPlus );
				break;
			case UPDATE_GUILD_REALPAY:
				UpdateGuildRealPay( pQuery, pQueryLog, lpDbOverlappedPlus );
				break;
			case GUILD_SETNAME:
				UpdateGuildSetName( pQuery, lpDbOverlappedPlus );
				break;
			case GUILD_BANK_UPDAATE:
				UpdateGuildBankUpdate( pQuery, pQueryLog, lpDbOverlappedPlus );
				break;
			case GUILD_BANK_QUERY:
				OpenQueryGuildBank( pQuery, lpDbOverlappedPlus );
				break;

			case ADD_GUILD_VOTE:
				AddGuildVote( pQuery, lpDbOverlappedPlus );
				break;
			case REMOVE_GUILD_VOTE:
				RemoveGuildVote( pQuery, lpDbOverlappedPlus );
				break;
			case CLOSE_GUILD_VOTE:
				CloseGuildVote( pQuery, lpDbOverlappedPlus );
				break;
			case CAST_GUILD_VOTE:
				CastGuildVote( pQuery, lpDbOverlappedPlus );
				break;

			case ACPT_GUILD_WAR:
				AcptWar( pQuery, lpDbOverlappedPlus );
				break;
			case WAR_END:
				WarEnd( pQuery, pQueryLog, lpDbOverlappedPlus );
				break;
			case SURRENDER:
				Surrender( pQuery, lpDbOverlappedPlus );
				break;
			case WAR_MASTER_ABSENT:
				WarMasterAbsent( pQuery, lpDbOverlappedPlus );
				break;
			case WAR_DEAD:
				WarDead( pQuery, lpDbOverlappedPlus );
				break;

			case GUILD_RANKING_UPDATE:
				UpdateGuildRanking( pQuery, lpDbOverlappedPlus );
				break;
			case GUILD_RANKING_UPDATE_DB:
				UpdateGuildRankingDB( pQuery, lpDbOverlappedPlus );
				break;

			case QM_QUERYGUILDQUEST:
				QueryGuildQuest( pQuery, lpDbOverlappedPlus );
				break;
			case QM_INSERTGUILDQUEST:
				InsertGuildQuest( pQuery, lpDbOverlappedPlus );
				break;
			case QM_UPDATEGUILDQUEST:
				UpdateGuildQuest( pQuery, lpDbOverlappedPlus );
				break;

			case WANTED_OP:
				WantedOperation( pQuery, lpDbOverlappedPlus );
				break;
			case START_GUILDCOMBAT:
				StartGuildCombat( pQuery, lpDbOverlappedPlus );
				break;
			case IN_GUILDCOMBAT:
				InGuildCombat( pQuery, lpDbOverlappedPlus );
				break;
			case OUT_GUILDCOMBAT:
				OutGuildCombat( pQuery, lpDbOverlappedPlus );
				break;
			case RESULT_GUILDCOMBAT:
				ResultGuildCombat( pQuery, lpDbOverlappedPlus );
				break;
			case GETPENYAGUILD_GUILDCOMBAT:
				GetPenyaGuildGC( pQuery, lpDbOverlappedPlus );
				break;
			case GETPENYAPLAYER_GUILDCOMBAT:
				GetPenyaPlayerGC( pQuery, lpDbOverlappedPlus );
				break;
			case CONTINUE_GUILDCOMBAT:
				ContinueGC( pQuery, lpDbOverlappedPlus );
				break;
			case ADD_MAIL:
				AddMail( pQuery, lpDbOverlappedPlus );
				break;
			case REMOVE_MAIL:
				RemoveMail( pQuery, lpDbOverlappedPlus );
				break;
			case REMOVE_MAIL_ITEM:
				RemoveMailItem( pQuery, lpDbOverlappedPlus );
				break;
			case REMOVE_MAIL_GOLD:
				RemoveMailGold( pQuery, lpDbOverlappedPlus );
				break;
			case READ_MAIL:
				ReadMail( pQuery, lpDbOverlappedPlus );
				break;
#ifdef __S_RECOMMEND_EVE
			case RECOMMEND_EVE:
				RecommendEve( pQuery, lpDbOverlappedPlus );
				break;
#endif // __S_RECOMMEND_EVE
#ifdef __GETMAILREALTIME
			case QM_GETMAIL_REALTIME:
				QueryGetMailRealTime( pQuery );
				FreeRequest( lpDbOverlappedPlus );
				break;
#endif // __GETMAILREALTIME
#if __VER >= 11 // __GUILD_COMBAT_1TO1
			case GC1TO1_LOAD:
				LoadGC1to1TenderGuild( pQuery, lpDbOverlappedPlus, (DPID)dwCompletionKey );
				break;
			case GC1TO1_STATE:
				GC1to1State( pQuery, lpDbOverlappedPlus );
				break;
			case GC1TO1_TENDER:
				GC1to1Tender( pQuery, lpDbOverlappedPlus );
				break;
			case GC1TO1_LINEUP:
				GC1to1LineUp( pQuery, lpDbOverlappedPlus );
				break;
			case GC1TO1_WARPERSON:
				GC1to1WarPerson( pQuery, lpDbOverlappedPlus );
				break;
			case GC1TO1_WARGUILD:
				GC1to1WarGuild( pQuery, lpDbOverlappedPlus );
				break;
#endif // __GUILD_COMBAT_1TO1
#if __VER >= 11 // __MA_VER11_04	// 길드 창고 로그 기능 world,database,neuz
			case QM_GUILDBANK_LOG_VIEW:
				GuildBankLogView( pQuery, lpDbOverlappedPlus );
				break;
#endif //__MA_VER11_04	// 길드 창고 로그 기능 world,database,neuz
#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
			case QM_SEALCHAR:
				SealChar( pQuery, lpDbOverlappedPlus );
				break;
			case QM_SEALCHARCONM:
				SealCharConm( pQuery, lpDbOverlappedPlus );
				break;
			case QM_SEALCHARGET:
				SealCharGet( pQuery, lpDbOverlappedPlus );
				break;
			case QM_SEALCHARSET:
				SealCharSet( pQuery, lpDbOverlappedPlus );
				break;
#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
#ifdef __JEFF_FIX_0
			case QM_ALL_PLAYER_DATA:
				{
					CDPTrans::GetInstance()->SendAllPlayerData( lpDbOverlappedPlus->dpid );
					CDPTrans::GetInstance()->SendAllMail( lpDbOverlappedPlus->dpid );
					FreeRequest( lpDbOverlappedPlus );
					break;
				}
			case QM_QUERY_MAIL_BOX:
				{
// 					//	BEGINTEST
// 					Error( "QM_QUERY_MAIL_BOX");

					CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
					u_long idReceiver;
					ar >> idReceiver;
					CPost *pPost    = CPost::GetInstance();
					CMclAutoLock	Lock( pPost->m_csPost );

					CMailBox* pMailBox	= NULL;
					pMailBox = pPost->GetMailBox( idReceiver );
					
					if( pMailBox != NULL )
					{
						CDPTrans::GetInstance()->SendMailBox( pMailBox, lpDbOverlappedPlus->dpid );
					}
					else
					{
						CDPTrans::GetInstance()->SendMailBoxReq( idReceiver, lpDbOverlappedPlus->dpid, FALSE, pMailBox );
					}

					FreeRequest( lpDbOverlappedPlus );
					break;
				}
#endif	// __JEFF_FIX_0
		}
	}
}

#ifdef __TOOMANY_PENDINGLOG1115
BOOL CDbManager::IsTooManyPending( void )
{
	if( CDbManager::m_lPending > MAX_PENDING )
	{
		WriteError( "PENDING//0" );
		return TRUE;
	}
	AddPending();
	return FALSE;
}
#endif	// __TOOMANY_PENDINGLOG1115

void CDbManager::SerializeGuildCombat( CAr & ar )
{
	ar << m_nGuildCombatIndex;
	ar << m_uWinGuildId;
	ar << m_nWinGuildCount;
	ar << m_uBestPlayer;
#ifdef __S_BUG_GC
	ar << (u_long)m_vecGuildCombat.size();
	for( DWORD gci = 0 ; gci < m_vecGuildCombat.size() ; ++gci )
	{
		__GUILDCOMBATJOIN GCJoin = m_vecGuildCombat[gci];
		ar << GCJoin.uGuildId;
		ar << GCJoin.dwPenya;
		ar << GCJoin.bRequest;
	}
#else // __S_BUG_GC
	ar << (u_long)m_GuildCombat.size();
	for( map<u_long, __GUILDCOMBATJOIN>::iterator it = m_GuildCombat.begin() ; it != m_GuildCombat.end() ; ++ it )
	{
		__GUILDCOMBATJOIN GCJoin = it->second;
		ar << (u_long)it->first;
		ar << GCJoin.dwPenya;
		ar << GCJoin.bRequest;
	}
#endif // __S_BUG_GC
	SerializeResultValue( ar );
	SerializePlayerPoint( ar );
}
void CDbManager::SerializeResultValue( CAr & ar )
{
	// 길드 보상 관련 정보 주기
	ar << (u_long)m_GCResultValueGuild.size();
	for( DWORD veci = 0 ; veci < m_GCResultValueGuild.size() ; ++veci )
	{
		__GCRESULTVALUEGUILD ResultValueGuild = m_GCResultValueGuild[veci];
		ar << ResultValueGuild.nCombatID;
		ar << ResultValueGuild.uidGuild;
		ar << ResultValueGuild.nReturnCombatFee;
		ar << ResultValueGuild.nReward;
	}
	// 베스트 플레이어 보상 관련 정보 주기
	ar << (u_long)m_GCResultValuePlayer.size();
	for( DWORD veci = 0 ; veci < m_GCResultValuePlayer.size() ; ++veci )
	{
		__GCRESULTVALUEPLAYER ResultValuePlayer = m_GCResultValuePlayer[veci];
		ar << ResultValuePlayer.nCombatID;
		ar << ResultValuePlayer.uidGuild;
		ar << ResultValuePlayer.uidPlayer;
		ar << ResultValuePlayer.nReward;
	}
}
void CDbManager::SerializePlayerPoint( CAr & ar )
{
	// 길드대전 개인 포인트
	ar << (u_long)m_vecGCPlayerPoint.size();
	for( DWORD veci = 0 ; veci < m_vecGCPlayerPoint.size() ; ++veci )
	{
		__GCPLAYERPOINT GCPlayerPoint = m_vecGCPlayerPoint[veci];
		ar << GCPlayerPoint.uidPlayer;
		ar << GCPlayerPoint.nJob;
		ar << GCPlayerPoint.nPoint;
	}
}

#if __VER < 11 // __SYS_PLAYER_DATA
//{{AFX
void CDbManager::SerializePlayerID( CAr & ar )
{
	ASSERT( ar.IsStoring() );
	
	CMclAutoLock	Lock( s_cs );
	
	ar << (u_long)m_ulong2str.size();
	for( ULONG2STRING::iterator i = m_ulong2str.begin(); i != m_ulong2str.end(); ++i )
	{
		ar << (*i).first;
		ar.WriteString( (*i).second.data() );
	}
}
//}}AFX
#endif	// __SYS_PLAYER_DATA

/*
u_long CDbManager::GetPlayerID( LPCSTR lpszPlayer )
{
	CMclAutoLock	Lock( s_cs );
	m_idPlayer++;
	m_ulong2str.insert( map<u_long, string>::value_type( m_idPlayer, lpszPlayer ) );
	return( m_idPlayer );
}
*/

void CDbManager::SerializeWanted( CQuery* pQuery, CAr& out )
{
	int		nCount = 0;
	char	szPlayer[64];
	__int64 penya;
	CTime	date;

	u_long	idPlayer;
	char	szMsg[WANTED_MSG_MAX + 1] = "";

	u_long uOffset	= out.GetOffset();
	out << nCount;	

	char szBuffer[32];
	char szDate[13] = {0,};
	while( pQuery->Fetch() )
	{
		idPlayer = pQuery->GetInt( "m_idPlayer" );		// 
		pQuery->GetStr( "m_szName", szPlayer );			// 현상범 이름 
		penya = pQuery->GetInt64( "penya" );			// 현상금액 
		pQuery->GetStr( "s_date", szDate );				// 만기일 
		pQuery->GetStr( "szMsg", szBuffer );			// 십자평 

		GetStrTime( &date, szDate );
		memcpy( szMsg, szBuffer, WANTED_MSG_MAX );		// 잘못된 데이타를 막기위한 코드 
		szMsg[WANTED_MSG_MAX] = '\0';

		out << idPlayer;
		out.WriteString( szPlayer );
		out << penya;
		out << (long)date.GetTime(); 
		out.WriteString( szMsg );

		nCount++;
	}

	int nBufferSize;
	BYTE* pBuffer	= out.GetBuffer( &nBufferSize );
	*(UNALIGNED int*)( pBuffer + uOffset )	= nCount;
}

BOOL CDbManager::OnWantedQuery( CQuery* pQuery, WANTED_QUERYINFO& info )
{
	char szQeury[1024];
	sprintf( szQeury,
			"{call WANTED_STR('%s', '%07d', '%02d', '%d', ?)}", 
             info.pszType, info.idPlayer, g_appInfo.dwSys, info.nGold );

	SQLINTEGER cbLen = SQL_NTS;
	if( !pQuery->BindParameter( 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, WANTED_MSG_MAX, 0, info.szMsg, 0, &cbLen ) )
		return FALSE;

	if( !pQuery->Exec( szQeury ) )
		return FALSE ;

	return TRUE;
}

BOOL CDbManager::OpenWanted( CAr& ar )
{
	CQuery qry;
	if( qry.Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) == FALSE )
	{
		AfxMessageBox( "Error : Not Connect Character DB" );
		return FALSE;
	}

	WANTED_QUERYINFO info( NULL );
	info.pszType = "S1";
	if( OnWantedQuery( &qry, info ) )
	{
		SerializeWanted( &qry, ar );
		return TRUE;
	}
	else
		return FALSE;
}

//	WANTED_STR @iGu,@m_idPlayer,@serverindex,@penya,@szMsg
//	@iGu = 'S1' 현상범 리스트 얻기 (@m_idPlayer,@serverindex ) 
//	@iGu = 'A1' 현상금 걸기 (@m_idPlayer,@serverindex,@penya,@szMsg ) 
//	@iGu = 'D1' 현상범 제거 (@m_idPlayer,@serverindex) 
//	현상금 시스템 디비 처리를 한다.
void CDbManager::WantedOperation( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	BYTE	byReqType;
	u_long	idPlayer;
	int		nGold;
	long	nEnd;
	char	szMsg[WANTED_MSG_MAX + 1] = "";

	ar >> byReqType;
	ar >> idPlayer;
	ar >> nGold;
	ar >> nEnd;					// 사용하지 않는다.
	ar.ReadString( szMsg, WANTED_MSG_MAX + 1 );

	FreeRequest( lpDbOverlappedPlus );

	WANTED_QUERYINFO info( NULL );
	switch( byReqType )
	{
	case CT_WANTED_REQ_LIST:
		info.pszType = "S1";
		break;
	case CT_WANTED_SET:
		if( nGold == 0 )
		{
			info.pszType	= "D1";
			info.idPlayer	= idPlayer;
		}
		else
		{
			info.pszType	= "A1";
			info.idPlayer	= idPlayer;
			info.nGold		= nGold;
			strcpy( info.szMsg, szMsg );
		}
		break;
	default:
		ASSERT( FALSE );
		break;
	}

#ifdef __INTERNALSERVER
	OnWantedQuery( pQuery, info );
#else
	if( OnWantedQuery( pQuery, info ) )
	{
		if( byReqType == CT_WANTED_REQ_LIST )
		{
			BEFORESEND( out, PACKETTYPE_TC_LIST );
			SerializeWanted( pQuery, out );
			SEND( out, &g_dpCoreSrvr, DPID_ALLPLAYERS );
		}
	}
#endif
}

void CDbManager::InGuildCombat( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	u_long idGuild;
	DWORD dwPenya;
	DWORD dwExistingPenya;
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	ar >> idGuild;
	ar >> dwPenya;	
	ar >> dwExistingPenya;

	char szQuery[QUERY_SIZE]	= { 0,};
	sprintf( szQuery, "uspJoinGuildToCombat %d,'%02d','%06d',%d,%d", 
		m_nGuildCombatIndex, g_appInfo.dwSys, idGuild, dwPenya,0 );

	if( pQuery->Exec( szQuery ) )
	{
		__GUILDCOMBATJOIN GCJoin;
		GCJoin.dwPenya = dwPenya;
		GCJoin.bRequest = TRUE;
#ifdef __S_BUG_GC
		GCJoin.uGuildId = idGuild;
		m_vecGuildCombat.push_back( GCJoin );
#else // __S_BUG_GC
		m_GuildCombat.insert( map<u_long, __GUILDCOMBATJOIN>::value_type( idGuild, GCJoin) );
#endif // __S_BUG_GC
		CDPTrans::GetInstance()->SendInGuildCombat( idGuild, dwPenya, dwExistingPenya );
	}
	else
	{	WriteLog( "InGuildCombat()"); TRACE("ERROR: InGuildCombat()\n"); }
	
	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::OutGuildCombat( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	u_long idGuild;
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	ar >> idGuild;

	char szQuery[QUERY_SIZE]	= { 0,};
	sprintf( szQuery, "uspCancelGuildToCombat %d,'%02d','%06d'", m_nGuildCombatIndex, g_appInfo.dwSys, idGuild );

	if( pQuery->Exec( szQuery ) )
	{
#ifdef __S_BUG_GC
		BOOL bFind = FALSE;
		__GUILDCOMBATJOIN GCJoin;
		for( DWORD gci = 0 ; gci < m_vecGuildCombat.size() ; ++gci )
		{
			if( idGuild == m_vecGuildCombat[gci].uGuildId )
			{
				bFind = TRUE;
				GCJoin = m_vecGuildCombat[gci];
				break;
			}
		}
		if( bFind )
			GCJoin.bRequest = FALSE;
#else // __S_BUG_GC
		map<u_long, __GUILDCOMBATJOIN>::iterator it = m_GuildCombat.find( idGuild );
		if( it != m_GuildCombat.end() )
		{
			__GUILDCOMBATJOIN GCJoin = it->second;
			GCJoin.bRequest = FALSE;
		}
#endif // __S_BUG_GC
		CDPTrans::GetInstance()->SendOutGuildCombat( idGuild );
	}
	else
	{	WriteLog( "OutGuildCombat()");	TRACE("ERROR: OutGuildCombat()\n");	}

	FreeRequest( lpDbOverlappedPlus );
}
void CDbManager::StartGuildCombat( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	// 길드 보상 및 출전 페냐 소멸
	for( DWORD veci = 0 ; veci < m_GCResultValueGuild.size() ; ++veci )
	{
		__GCRESULTVALUEGUILD ResultValueGuild = m_GCResultValueGuild[veci];

		char szQuery[QUERY_SIZE]	= { 0,};
		sprintf( szQuery, "uspExpireCombatReward %d,'%02d','%06d','%07d'", 
			ResultValueGuild.nCombatID, g_appInfo.dwSys, ResultValueGuild.uidGuild, 0 );
		if( FALSE == pQuery->Exec( szQuery ) )
		{	WriteLog( "StartGuildCombat()");	TRACE("ERROR: StartGuildCombat()\n");	}
	}
	m_GCResultValueGuild.clear();

	// 베스트 플레이어 보상
	for( DWORD veci = 0 ; veci < m_GCResultValuePlayer.size() ; ++veci )
	{
		__GCRESULTVALUEPLAYER ResultValuePlayer = m_GCResultValuePlayer[veci];
		char szQuery[QUERY_SIZE]	= { 0,};
		sprintf( szQuery, "uspExpireCombatReward %d,'%02d','%06d','%07d'", 
			ResultValuePlayer.nCombatID, g_appInfo.dwSys, ResultValuePlayer.uidGuild, ResultValuePlayer.uidPlayer );
		if( FALSE == pQuery->Exec( szQuery ) )
		{	WriteLog( "StartGuildCombat()");	TRACE("ERROR: StartGuildCombat()\n");	}
	}
	m_GCResultValueGuild.clear();
	
	// 대전 전쟁 시작
	char szQuery[QUERY_SIZE]	= { 0,};
	sprintf( szQuery, "uspStartCombat %d,'%02d'", m_nGuildCombatIndex, g_appInfo.dwSys );
	if( FALSE == pQuery->Exec( szQuery ) )
	{	WriteLog( "StartGuildCombat()");	TRACE("ERROR: StartGuildCombat()\n");	}
	FreeRequest( lpDbOverlappedPlus );
}
void CDbManager::ResultGuildCombat( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	// 대전 종료
	char szQuery[QUERY_SIZE]	= { 0,};
	sprintf( szQuery, "uspEndCombat %d,'%02d'", m_nGuildCombatIndex, g_appInfo.dwSys );
	if( FALSE == pQuery->Exec( szQuery ) )
	{	WriteLog( "ResultGuildCombat()");	TRACE("ERROR: ResultGuildCombat()\n");	}
	
/*	// 베스트 플레이어 저장
	u_long uBestPlayer;
	u_long uBestPlayerGuild;
	DWORD dwBestPlayerPrize;
	int nGetPoint;
	ar >> uBestPlayer;
	ar >> uBestPlayerGuild;
	ar >> nGetPoint;
	ar >> dwBestPlayerPrize;

	sprintf( lpDbOverlappedPlus->szQuery, "uspGradeCombatPlayer %d,'%02d','%06d','%06d',%d, %d", 
		m_nGuildCombatIndex, g_appInfo.dwSys, uBestPlayerGuild, uBestPlayer, nGetPoint, dwBestPlayerPrize );
	if( FALSE == pQuery->Exec( lpDbOverlappedPlus->szQuery ) )
	{	WriteLog( "ResultGuildCombat()");	TRACE("ERROR: ResultGuildCombat()\n");	}
	
*/	
	ar >> m_uWinGuildId;
	ar >> m_nWinGuildCount;
	ar >> m_uBestPlayer;	

	int nGuildPoint, nWinCount;
	u_long uidGuild, uSizeGuild;
	__int64 nPrize;
	DWORD dwReturnPenya;
	// 길드 저장
	ar >> uSizeGuild;
	for( DWORD i = 0 ; i < uSizeGuild ; ++i )
	{
		ar >> uidGuild;
		ar >> nGuildPoint;
		ar >> nPrize;
		ar >> dwReturnPenya;
		ar >> nWinCount;
		char szQuery[QUERY_SIZE]	= { 0,};
		sprintf( szQuery, "uspGradeCombatGuild %d,'%02d','%06d',%d,%d,%I64d,%d", 
			m_nGuildCombatIndex, g_appInfo.dwSys, uidGuild, nGuildPoint, dwReturnPenya, nPrize, nWinCount );
		if( FALSE == pQuery->Exec( szQuery ) )
		{	WriteLog( "ResultGuildCombat()");	TRACE("ERROR: ResultGuildCombat()\n");	}

		// 플레이어 저장
		u_long uSizePlayer;
		u_long uidPlayer;
		int nPlayerPoint;
		__int64 nPrizePlyaer;
		ar >> uSizePlayer;
		for( DWORD j = 0 ; j < uSizePlayer ; ++j )
		{
			ar >> uidPlayer;
			ar >> nPlayerPoint;
			ar >> nPrizePlyaer;
			if( m_uBestPlayer == uidPlayer )
				nPrizePlyaer = 1;
//			if( 0 < nPrizePlyaer )
//				m_uBestPlayer = uidPlayer;

			char szQuery[QUERY_SIZE]	= { 0,};
			sprintf( szQuery, "uspGradeCombatPlayer %d,'%02d','%06d','%07d',%d,%I64d", 
				m_nGuildCombatIndex, g_appInfo.dwSys, uidGuild, uidPlayer, nPlayerPoint, nPrizePlyaer );
			if( FALSE == pQuery->Exec( szQuery ) )
			{	WriteLog( "ResultGuildCombat()");	TRACE("ERROR: ResultGuildCombat()\n");	}
		}
	}

#ifdef __S_BUG_GC
	m_vecGuildCombat.clear();
#else // __S_BUG_GC
	m_GuildCombat.clear();
#endif // __S_BUG_GC
	++m_nGuildCombatIndex;

	// 다음대전 시작
	sprintf( szQuery, "uspAddNewCombat %d,'%02d'", m_nGuildCombatIndex, g_appInfo.dwSys );
	if( FALSE == pQuery->Exec( szQuery ) )
	{	WriteLog( "ResultGuildCombat()");	TRACE("ERROR: ResultGuildCombat()\n");	}

	// 보상 및 참여금을 안받은 길드 및 베스트 플레이어 얻어오기
	m_GCResultValueGuild.clear();
	m_GCResultValuePlayer.clear();
	sprintf( szQuery, "uspLoadCombatUnpaidList '%02d'", g_appInfo.dwSys );
	if( FALSE == pQuery->Exec( szQuery ) )
	{	WriteLog( "ResultGuildCombat()");	TRACE("ERROR: ResultGuildCombat()\n");	}
	while( pQuery->Fetch() )
	{
		char cFlag;
		cFlag = pQuery->GetChar( "Flag" );
		if( cFlag == 'G' )	// 길드
		{
			__GCRESULTVALUEGUILD ResultValueGuild;
			ResultValueGuild.nCombatID = pQuery->GetInt( "CombatID" );
			ResultValueGuild.uidGuild = pQuery->GetInt( "GuildID" );
			ResultValueGuild.nReturnCombatFee = pQuery->GetInt64( "ReturnCombatFee" );
			ResultValueGuild.nReward = pQuery->GetInt64( "Reward" );
			m_GCResultValueGuild.push_back( ResultValueGuild );
		}
		else				// 플레이어
		{
			__GCRESULTVALUEPLAYER ResultValuePlayer;
			ResultValuePlayer.nCombatID = pQuery->GetInt( "CombatID" );
			ResultValuePlayer.uidGuild = pQuery->GetInt( "GuildID" );
			ResultValuePlayer.uidPlayer = pQuery->GetInt( "PlayerID" );
			ResultValuePlayer.nReward = pQuery->GetInt64( "Reward" );
			m_GCResultValuePlayer.push_back( ResultValuePlayer );
		}
	}	
	
	// 개인 포인트 가지고 오기
	m_vecGCPlayerPoint.clear();
	sprintf( szQuery, "uspRankGuildCombatPlayer '%02d'", g_appInfo.dwSys );
	if( FALSE == pQuery->Exec( szQuery ) )
	{	WriteLog( "ResultGuildCombat()");	TRACE("ERROR: ResultGuildCombat()\n");	}
	while( pQuery->Fetch() )
	{
		__GCPLAYERPOINT GCPlayerPoint;
		GCPlayerPoint.uidPlayer = pQuery->GetInt( "PlayerID" );
		GCPlayerPoint.nJob = pQuery->GetInt( "Job" );
		GCPlayerPoint.nPoint = pQuery->GetInt( "PointSummary" );
		m_vecGCPlayerPoint.push_back( GCPlayerPoint );
	}
	
	CDPTrans::GetInstance()->SendResultGuildCombat();
	CDPTrans::GetInstance()->SendPlayerPointGuildCombat();

	FreeRequest( lpDbOverlappedPlus );
}
void CDbManager::GetPenyaGuildGC( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long uidPlayer, uidGuild;
	int nGuildCombatID;
	ar >> uidPlayer;
	ar >> nGuildCombatID;
	ar >> uidGuild;

	BOOL bFind = FALSE;
	int nFindVeci = 0;
	__GCRESULTVALUEGUILD ResultValuetmp;
	for( DWORD veci = 0 ; veci < m_GCResultValueGuild.size() ; ++veci )
	{
		ResultValuetmp = m_GCResultValueGuild[veci];
		if( nGuildCombatID == ResultValuetmp.nCombatID && uidGuild == ResultValuetmp.uidGuild )
		{
			bFind = TRUE;
			nFindVeci = veci;
			break;
		}
	}
	if( bFind )
	{
		char szQuery[QUERY_SIZE]	= { 0,};
		sprintf( szQuery, "uspRewardCombatGuild %d,'%02d','%06d'", ResultValuetmp.nCombatID, g_appInfo.dwSys, ResultValuetmp.uidGuild );
		if( FALSE == pQuery->Exec( szQuery ) )
		{	WriteLog( "GetPenyaGuildGuildCombat()");	TRACE("ERROR: GetPenyaGuildGuildCombat()\n");	}

		CDPTrans::GetInstance()->SendGetPenyaGuildGC( uidPlayer, nGuildCombatID, uidGuild );
		vector<__GCRESULTVALUEGUILD>::iterator ita = &(m_GCResultValueGuild[nFindVeci]);
		m_GCResultValueGuild.erase( ita );		
	}
	FreeRequest( lpDbOverlappedPlus );
}
void CDbManager::GetPenyaPlayerGC( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long uidPlayer, uidGuild;
	int nGuildCombatID;
	ar >> uidPlayer;
	ar >> nGuildCombatID;
	ar >> uidGuild;

	BOOL bFind = FALSE;
	int nFindVeci = 0;
	__GCRESULTVALUEPLAYER ResultValuetmp;
	for( DWORD veci = 0 ; veci < m_GCResultValuePlayer.size() ; ++veci )
	{
		ResultValuetmp = m_GCResultValuePlayer[veci];
		if( nGuildCombatID == ResultValuetmp.nCombatID && uidPlayer == ResultValuetmp.uidPlayer && uidGuild == ResultValuetmp.uidGuild )
		{
			bFind = TRUE;
			nFindVeci = veci;
			break;
		}
	}
	if( bFind )
	{
		char szQuery[QUERY_SIZE]	= { 0,};
		sprintf( szQuery, "uspRewardCombatPlayer %d,'%02d','%06d','%07d'"
			, ResultValuetmp.nCombatID, g_appInfo.dwSys, uidGuild, ResultValuetmp.uidPlayer );
		if( FALSE == pQuery->Exec( szQuery ) )
		{	WriteLog( "GetPenyaGuildGuildCombat()");	TRACE("ERROR: GetPenyaGuildGuildCombat()\n");	}

		CDPTrans::GetInstance()->SendGetPenyaPlayerGC( uidPlayer, nGuildCombatID, uidGuild );
		vector<__GCRESULTVALUEPLAYER>::iterator ita = &(m_GCResultValuePlayer[nFindVeci]);
		m_GCResultValuePlayer.erase( ita );		
	}
	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::ContinueGC( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long uidGuild;
	int nGuildCombatID;
	int nWinCount;
	ar >> nGuildCombatID;
	ar >> uidGuild;
	ar >> nWinCount;

	m_uWinGuildId = 0;
	m_nWinGuildCount = 0;
	
	char szQuery[QUERY_SIZE]	= { 0,};
		sprintf( szQuery, "uspCombatContinue %d,'%02d','%06d',%d"
		, nGuildCombatID, g_appInfo.dwSys, uidGuild, nWinCount );
	if( FALSE == pQuery->Exec( szQuery ) )
	{	WriteLog( "GetPenyaGuildGuildCombat()");	TRACE("ERROR: GetPenyaGuildGuildCombat()\n");	}

	CDPTrans::GetInstance()->SendContinueGC();

	FreeRequest( lpDbOverlappedPlus );
}

BOOL CDbManager::CanJoin( LPCTSTR szAccount, ACCOUNT_CACHE* pAccountCache )
{ 
	time_t t	= ::time( NULL );
#ifdef __INVALID_LOGIN_0320
	if( pAccountCache->IsPlaying() )
	{
		// 멀티 서버 번호가 초기화되지 않는 경우가 있다.
		// 종료 후 시간이 3분 이상 경과 되었다면 허용한다.
		if( t > pAccountCache->m_tmPrevent + 180 )
		{
			pAccountCache->ChangeMultiServer( 0 );
			Error( "CanJoin - time over" );
		}
		else
		{
			Error( "CanJoin - playing" );
			return FALSE;
		}
	}
#endif	// __INVALID_LOGIN_0320

	if( t > pAccountCache->m_tmPrevent )	// 일반 
		return TRUE;
	else
	{
		char szBuffer[256];
		sprintf( szBuffer, "CanJoin - cur:%d prevent:%d\n", t, pAccountCache->m_tmPrevent );
		OutputDebugString( szBuffer );
		return FALSE;
	}
}

void CDbManager::PutExpLog( CQuery* pQueryLog, CMover* pMover, char ch )
{
	BOOL bPut	= ( ch != 'P' );
	DWORD dwTick	= GetTickCount();
	if( !bPut )
	{
		if( pMover->m_dwPeriodicTick < dwTick )
			bPut	= TRUE;
	}
	if( bPut )
	{
		pMover->m_dwPeriodicTick	= dwTick + MIN( 120 );

		char szSql[1024]	= { 0, };
		DBQryLog( szSql, "L1", pMover->m_idPlayer, g_appInfo.dwSys,
			pMover->m_nExp1, pMover->m_nLevel,
			pMover->m_nJob, 0, pMover->GetFlightLv(),
			pMover->m_nStr, pMover->m_nDex, pMover->m_nInt, pMover->m_nSta,
			pMover->m_nRemainGP, 0,
			ch, 0, '\0', 0, 0, 0, pMover->GetGold(), 0 );

		if( FALSE == pQueryLog->Exec( szSql ) )
			return;
	}
}

void CDbManager::SetPlayerName( CQuery* pQueryChar, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	u_long idPlayer;
	DWORD dwData;
	char lpszPlayer[MAX_PLAYER];
	ar >> idPlayer;
	ar.ReadString( lpszPlayer, MAX_PLAYER );
	ar >> dwData;
	
	BOOL f	= FALSE;
	char szQuery[QUERY_SIZE]	= { 0,};
	DBQryCharacter( szQuery, "U4", idPlayer, g_appInfo.dwSys, NULL, lpszPlayer );
	if( pQueryChar->Exec( szQuery ) )
	{
		if( pQueryChar->Fetch() )
			f	= (BOOL)pQueryChar->GetInt( "fError" );
		char lpOutputString[128]	= { 0, };
		sprintf( lpOutputString, "DATABASESERVER.EXE\t// QM_SET_PLAYER_NAME\t//1(%d)\n", f );
		OutputDebugString( lpOutputString );
	}
	else
	{
		OutputDebugString( "DATABASESERVER.EXE\t// QM_SET_PLAYER_NAME\t//0(0)\n" );
	}

	if( f )
	{
#if __VER >= 11 // __SYS_PLAYER_DATA
		char szOldPlayer[MAX_PLAYER]		= { 0, };
		CPlayerDataCenter::GetInstance()->ReplacePlayerString( idPlayer, lpszPlayer, szOldPlayer );
#else	// _SYS_PLAYER_DATA
		s_cs.Enter();
		#ifndef __NOLOG
			char szOldPlayer[MAX_PLAYER]	= { 0, };
			map<u_long, string>::iterator i	= m_ulong2str.find( idPlayer );
			if( i != m_ulong2str.end() )
				lstrcpy( szOldPlayer, i->second.data() );
		#endif	// __NOLOG
		m_ulong2str.erase( idPlayer );
		m_ulong2str.insert( map<u_long, string>::value_type( idPlayer, lpszPlayer ) );
		s_cs.Leave();
#endif	// _SYS_PLAYER_DATA
/*
		uspChangeNameLog
                  @pserverindex char(2) :
                  @pidPlayer char(6) :
                  @pOldName varchar(32) :
                  @pNewName varchar(32) :
*/
		#ifndef __NOLOG
		char szQuery[QUERY_SIZE]	= { 0,};
		sprintf( szQuery, "uspChangeNameLog @pserverindex = '%02d', @pidPlayer = '%07d', @pOldName='%s', @pNewName='%s'",
			g_appInfo.dwSys, idPlayer, szOldPlayer, lpszPlayer );
#ifdef _DEBUG
		OutputDebugString( szQuery );
#endif	// _DEBUG
		if( pQueryLog->Exec( szQuery ) == FALSE )
			Error( "query failed: %s", szQuery );
		#endif	// __NOLOG
	}
	g_dpCoreSrvr.SendSetPlayerName( idPlayer, lpszPlayer, dwData, f );

	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::SnoopGuild( CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long idGuild, idPlayer;
	char lpszChat[1024]		= { 0, };
	ar >> idGuild >> idPlayer;
	ar.ReadString( lpszChat, 1024 );

	CString strChat	= lpszChat;
	strChat.Replace( "--", "" );
	strChat.Replace( "'", "" );
	strChat.Replace( "/*", "" );
	strChat.Replace( "*/", "" );

	OutputDebugString( "DATABASESERVER.EXE\t// QM_SNOOPGUILD\n" );
	// LOG_GAMEMASTER_STR '%s','%s','%s'
	char szQuery[QUERY_SIZE]	= { 0,};
	sprintf( szQuery, "LOG_GAMEMASTER_STR  '@im_idPlayer=%07d',@iserverindex='%02d',@iszWords='%s',@im_idGuild='%06d'", idPlayer, g_appInfo.dwSys, strChat, idGuild );
	if ( FALSE == pQueryLog->Exec( szQuery ) )
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );

	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::DBQryCharacter( char* qryCharacter, char* Gu, u_long idPlaeyr, int nserverindex, char* szAccount, char* szName,
								int nPlayerslot, DWORD dwWorldID, DWORD dwIndex, float vPos_x, float vPos_y,
								float vPos_z, char* szCharacterKey, DWORD dwSkinSet, DWORD dwHairMesh, DWORD dwHairColor,
								DWORD dwHeadMesh, DWORD dwSex, float vScale_x, DWORD dwMotion, float fAngle,
								int nHitPoint, int nManaPoint, int nFatiguePoint, DWORD dwRideItemIdx, DWORD dwGold,
								int nJob, char* szActMover, int nStr, int nSta, int nDex,
								int nInt, int nLevel, EXPINTEGER nExp1, EXPINTEGER nExp2, char* szJobSkill,
								char* szLicenseSkill, char* aJobLv, DWORD dwExpertLv, int nidMarkingWorld, float vMarkingPos_x,
								float vMarkingPos_y, float vMarkingPos_z, int nRemainGP, int nRemainLP, int nFlightLv,
								int nFxp, int nTxp, char* szQuestCntArray, char szAuthority, DWORD dwMode,
#if __VER >= 8 // __S8_PK
								int nidparty, int nidMuerderer, int nFame,
#else // __VER >= 8 // __S8_PK
								int nidparty, int nNumKill, int nidMuerderer, int nSlaughter, int nFame,
#endif // __VER >= 8 // __S8_PK
								EXPINTEGER nDeathExp, int nDeathLevel, DWORD dwFlyTime, DWORD dwMessengerState,
								int nTotalPlayTime, char* szCard, char* szIndex_Card, char* szObjIndexCard, char* szCube,
								char* szIndex_Cube, char* szObjIndex_Cube, char* szInventory, char* dzapIndex, char* szadwEquipment,
								char* szdwObjIndex, char* szSlotApplet, char* szSlotItem, char* szSlotQueue, int nActionPoint,
								char* szBank, char* m_apIndex_Bank, char* sadwObjIndex_Bank, DWORD dwGoldBank,
								int nFuel, int tmAccFuel, char* szSMTime, char* szSkillInfluence, DWORD m_dwSkillPoint
								, char* m_aCompleteQuest
								, char* ExtInven, char* PirecingInven, char* ExtBank, char* PirecingBank
								, DWORD dwReturnWorldID, float fReturnPosX, float fReturnPosY, float fReturnPosZ
#if __VER >= 8 // __S8_PK
								, int nPKValue, DWORD dwPKPropensity, DWORD dwPKExp	
#endif // __VER >= 8 // __S8_PK
#if __VER >= 8 //__CSC_VER8_5
								, EXPINTEGER nAngelExp, LONG nAngelLevel
#endif // __CSC_VER8_5
								, char* szInventoryPet,	char* szBankPet, DWORD dwPetId
#ifdef __EXP_ANGELEXP_LOG
								, int nExpLog, int nAngelExpLog
#endif // __EXP_ANGELEXP_LOG
#ifdef __EVENTLUA_COUPON
								, int nCoupon
#endif // __EVENTLUA_COUPON
#if __VER >= 13 // __HONORABLE_TITLE			// 달인
								, int nHonor					// 달인선택 
#endif	// __HONORABLE_TITLE			// 달인
#ifdef __LAYER_1015
								, int nLayer
#endif	// __LAYER_1015
#if __VER >= 15 // __CAMPUS
								, int nCampusPoint, u_long idCampus
#endif // __CAMPUS
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
								, char* m_aCheckedQuest
#endif // __IMPROVE_QUEST_INTERFACE
								)
{
	sprintf( qryCharacter, "CHARACTER_STR '%s'", Gu );
	char strCharacter[40960];

	sprintf( strCharacter, ",@im_idPlayer='%07d',@iserverindex='%02d',@iaccount='%s'",
		idPlaeyr, nserverindex, szAccount );
	strncat( qryCharacter, strCharacter, sizeof(strCharacter) );

	if( Gu[0] == 'S' && Gu[1] == '8' )
		return;
	
	sprintf( strCharacter, ",@im_szName='%s',@iplayerslot=%d,@idwWorldID=%d,@im_dwIndex=%d,@im_vPos_x=%f,@im_vPos_y=%f",
		szName, nPlayerslot, dwWorldID, dwIndex, vPos_x, vPos_y );
	strncat( qryCharacter, strCharacter, sizeof(strCharacter) );

	sprintf( strCharacter, ",@im_vPos_z=%f,@im_szCharacterKey='%s',@im_dwSkinSet=%d,@im_dwHairMesh=%d,@im_dwHairColor=%d,@im_dwHeadMesh=%d,@im_dwSex=%d,@im_vScale_x=%f,@im_dwMotion=%d,@im_fAngle=%f",
		vPos_z,szCharacterKey, dwSkinSet, dwHairMesh, dwHairColor, dwHeadMesh, dwSex, vScale_x, dwMotion, fAngle );
	strncat( qryCharacter, strCharacter, sizeof(strCharacter) );

	sprintf( strCharacter, ",@im_nHitPoint=%d,@im_nManaPoint=%d,@im_nFatiguePoint=%d,@im_dwRideItemIdx=%d,@im_dwGold=%d,@im_nJob=%d,@im_pActMover='%s',@im_nStr=%d,@im_nSta=%d,@im_nDex=%d",
		nHitPoint, nManaPoint, nFatiguePoint, dwRideItemIdx, dwGold, nJob, szActMover, nStr, nSta, nDex );
	strncat( qryCharacter, strCharacter, sizeof(strCharacter) );

	sprintf( strCharacter, ",@im_nInt=%d,@im_nLevel=%d,@im_nExp1=%I64d,@im_nExp2=%I64d,@im_aJobSkill='%s',@im_aLicenseSkill='%s',@im_aJobLv='%s',@im_dwExpertLv=%d,@im_idMarkingWorld=%d,@im_vMarkingPos_x=%f",
		nInt, nLevel, nExp1, (EXPINTEGER)0, szJobSkill, szLicenseSkill, aJobLv, dwExpertLv, nidMarkingWorld, vMarkingPos_x );
	strncat( qryCharacter, strCharacter, sizeof(strCharacter) );
	
	sprintf( strCharacter, ",@im_vMarkingPos_y=%f,@im_vMarkingPos_z=%f,@im_nRemainGP=%d,@im_nRemainLP=%d,@im_nFlightLv=%d,@im_nFxp=%d,@im_nTxp=%d,@im_lpQuestCntArray='%s',@im_chAuthority='%c',@im_dwMode=%d",
		vMarkingPos_y, vMarkingPos_z, nRemainGP, nRemainLP,	nFlightLv, nFxp, nTxp, szQuestCntArray, szAuthority, dwMode );
	strncat( qryCharacter, strCharacter, sizeof(strCharacter) );

#if __VER >= 8 // __S8_PK
	sprintf( strCharacter, ",@im_idparty=%d,@im_idMuerderer=%d,@im_nFame=%d,@im_nDeathExp=%I64d,@im_nDeathLevel=%d,@im_dwFlyTime=%d,@im_nMessengerState=%d,@iTotalPlayTime=%d",
		nidparty, nidMuerderer, nFame, nDeathExp, nDeathLevel, dwFlyTime, dwMessengerState, nTotalPlayTime );
#else // __VER >= 8 // __S8_PK
	sprintf( strCharacter, ",@im_idparty=%d,@im_nNumKill=%d,@im_idMuerderer=%d,@im_nSlaughter=%d,@im_nFame=%d,@im_nDeathExp=%I64d,@im_nDeathLevel=%d,@im_dwFlyTime=%d,@im_nMessengerState=%d,@iTotalPlayTime=%d",
		nidparty, nNumKill, nidMuerderer, nSlaughter, nFame, nDeathExp, nDeathLevel, dwFlyTime, dwMessengerState, nTotalPlayTime );
#endif // __VER >= 8 // __S8_PK
	strncat( qryCharacter, strCharacter, sizeof(strCharacter) );

	sprintf( strCharacter, ",@im_Card='%s',@im_Index_Card='%s',@im_ObjIndex_Card='%s',@im_Cube='%s',@im_Index_Cube='%s',@im_ObjIndex_Cube='%s',@im_Inventory='%s',@im_apIndex='%s',@im_adwEquipment='%s',@im_dwObjIndex='%s'",
		szCard, szIndex_Card, szObjIndexCard, szCube, szIndex_Cube, szObjIndex_Cube, szInventory, dzapIndex, szadwEquipment, szdwObjIndex );
	strncat( qryCharacter, strCharacter, sizeof(strCharacter) );

	sprintf( strCharacter, ",@im_aSlotApplet='%s',@im_aSlotItem='%s',@im_aSlotQueue='%s',@im_SkillBar=%d,@im_Bank='%s',@im_apIndex_Bank='%s',@im_dwObjIndex_Bank='%s',@im_dwGoldBank=%d,@im_nFuel=%d",
		szSlotApplet, szSlotItem, szSlotQueue, nActionPoint, szBank, m_apIndex_Bank, sadwObjIndex_Bank, dwGoldBank, nFuel );
	strncat( qryCharacter, strCharacter, sizeof(strCharacter) );

	sprintf( strCharacter, ",@im_tmAccFuel=%d", tmAccFuel );
	strncat( qryCharacter, strCharacter, sizeof(strCharacter) );	

	sprintf( strCharacter, ",@im_dwSMTime='%s'", szSMTime );
	strncat( qryCharacter, strCharacter, sizeof(strCharacter) );	

	sprintf( strCharacter, ",@iSkillInfluence='%s'", szSkillInfluence );
	strncat( qryCharacter, strCharacter, sizeof(strCharacter) );	

	sprintf( strCharacter, ",@im_aCompleteQuest='%s'", m_aCompleteQuest );
	strncat( qryCharacter, strCharacter, sizeof(strCharacter) );	

	sprintf( strCharacter, ",@im_extInventory='%s',@im_InventoryPiercing='%s',@im_extBank='%s',@im_BankPiercing ='%s'", ExtInven, PirecingInven, ExtBank, PirecingBank );
	strncat( qryCharacter, strCharacter, sizeof(strCharacter) );

	sprintf( strCharacter, ",@im_dwReturnWorldID=%d,@im_vReturnPos_x=%f,@im_vReturnPos_y=%f,@im_vReturnPos_z=%f",
		                     dwReturnWorldID, fReturnPosX, fReturnPosY, fReturnPosZ );
	strncat( qryCharacter, strCharacter, sizeof(strCharacter) );
	
#if __VER >= 8 // __S8_PK
	sprintf( strCharacter, ",@im_nPKValue=%d,@im_dwPKPropensity=%d,@im_dwPKExp=%d",
							nPKValue, dwPKPropensity, dwPKExp );
	strncat( qryCharacter, strCharacter, sizeof(strCharacter) );
#endif // __VER >= 8 // __S8_PK
#if __VER >= 8 //__CSC_VER8_5
	sprintf( strCharacter, ",@im_nAngelExp=%I64d,@im_nAngelLevel=%d",
							nAngelExp, nAngelLevel );
	strncat( qryCharacter, strCharacter, sizeof(strCharacter) );
#endif // __CSC_VER8_5

	sprintf( strCharacter, ",@iszInventoryPet='%s',@iszBankPet='%s', @im_dwPetId=%d",
							szInventoryPet, szBankPet, dwPetId );
	strncat( qryCharacter, strCharacter, sizeof(strCharacter) );
#ifdef __EXP_ANGELEXP_LOG
	sprintf( strCharacter, ",@im_nExpLog=%d, @im_nAngelExpLog=%d",
							nExpLog, nAngelExpLog );
	strncat( qryCharacter, strCharacter, sizeof(strCharacter) );
#endif // __EXP_ANGELEXP_LOG
#ifdef __EVENTLUA_COUPON
	sprintf( strCharacter, ",@im_nCoupon=%d",
							nCoupon );
	strncat( qryCharacter, strCharacter, sizeof(strCharacter) );
#endif // __EVENTLUA_COUPON
#if __VER >= 13 // __HONORABLE_TITLE			// 달인
	sprintf( strCharacter, ",@im_nHonor=%d",
							nHonor );
	strncat( qryCharacter, strCharacter, sizeof(strCharacter) );
#endif	// __HONORABLE_TITLE			// 달인
#ifdef __LAYER_1015
	sprintf( strCharacter, ",@im_nLayer=%d",
							nLayer );
	strncat( qryCharacter, strCharacter, sizeof(strCharacter) );
#endif	// __LAYER_1015
	
#if __VER >= 15 // __CAMPUS
	sprintf( strCharacter, ",@im_nCampusPoint=%d", nCampusPoint );
	strncat( qryCharacter, strCharacter, sizeof(strCharacter) );

	sprintf( strCharacter, ",@im_idCampus=%d", idCampus );
	strncat( qryCharacter, strCharacter, sizeof(strCharacter) );
#endif // __CAMPUS

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	sprintf( strCharacter, ",@im_aCheckedQuest='%s'", m_aCheckedQuest );
	strncat( qryCharacter, strCharacter, sizeof(strCharacter) );
#endif // __IMPROVE_QUEST_INTERFACE

	if( strlen(qryCharacter) > 40960 )
	{
		ASSERT(0);
	}
}

BOOL CDbManager::BankToItemSendTbl( LPCSTR lpFileName )
{
	CQuery* pQueryLoad	= new CQuery;
	CQuery* pQuerySave	= new CQuery;

	if( FALSE == pQueryLoad->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
	{
		SAFE_DELETE( pQueryLoad );
		SAFE_DELETE( pQuerySave );
		AfxMessageBox( "CDbManager::BankToItemSendTblList", MB_OK );
		return FALSE;
	}
	if( FALSE == pQuerySave->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
	{
		SAFE_DELETE( pQueryLoad );
		SAFE_DELETE( pQuerySave );
		AfxMessageBox( "CDbManager::BankToItemSendTblList", MB_OK );
		return FALSE;
	}

	char* szSql		= new char[40960];

	CScanner s;
	if( s.Load( lpFileName ) )
	{
		s.GetToken();
		while( s.tok != FINISHED )
		{
			sprintf( szSql, "select a.m_idPlayer, a.serverindex, a.m_Bank, a.m_apIndex_Bank, a.m_dwObjIndex_Bank, b.m_extBank, b.m_BankPiercing  from BANK_TBL a, BANK_EXT_TBL b where a.m_idPlayer = b.m_idPlayer and a.m_idPlayer = '%07d' and a.serverindex = b.serverindex and b.serverindex = '%02d'"
				, atoi( (const char*)s.Token ), g_appInfo.dwSys );

			if( FALSE == pQueryLoad->Exec( szSql ) )
			{
				SAFE_DELETE( pQueryLoad );
				SAFE_DELETE( pQuerySave );
				SAFE_DELETE_ARRAY( szSql );
				AfxMessageBox( "Exec", MB_OK );
				return FALSE;
			}

			if( FALSE == pQueryLoad->Fetch() )
			{
				SAFE_DELETE( pQueryLoad );
				SAFE_DELETE( pQuerySave );
				SAFE_DELETE_ARRAY( szSql );
				AfxMessageBox( "Fetch", MB_OK );
				return FALSE;
			}
			CMover* pMover	= new CMover;
			pMover->m_idPlayer	= pQueryLoad->GetInt( "m_idPlayer" );
			GetBankMover( pMover, pQueryLoad, 0 );

			int cbBank	= pMover->m_Bank[0].m_dwItemMax;
			for( int i = 0; i < cbBank; i++ )
			{
				CItemElem* pItemElem	= pMover->m_Bank[0].GetAtId( i );
				if( pItemElem )
				{
					FILEOUT( "..\\BankToItemSendTbl.log", "idPlayer:%07d\tItem:%s(%d)\tnItemNum:%d\tnPiercedSize:%d\tdwTime:%d", pMover->m_idPlayer, prj.GetItemProp( pItemElem->m_dwItemId )->szName, pItemElem->m_dwItemId, pItemElem->m_nItemNum, pItemElem->GetPiercingSize(), pItemElem->m_dwKeepTime );
					if( pItemElem->GetPiercingSize() > 0 )
					{
						for( int j = 0; j < pItemElem->GetPiercingSize(); j++ )
						{
							if( pItemElem->GetPiercingItem( j ) > 0 )
							{
								FILEOUT( "..\\BankToItemSendTbl.log", "adwItemId[%d]:%d", j, pItemElem->GetPiercingItem( j ) );
								sprintf( szSql, "insert ITEM_SEND_TBL (m_idPlayer, serverindex, Item_Name, Item_count, m_nAbilityOption,  End_Time, m_bItemResist, m_nResistAbilityOption, m_bCharged) VALUES ('%07d','%02d','%d',%d,%d,NULL,%d,%d,1)",
									pMover->m_idPlayer, g_appInfo.dwSys, pItemElem->GetPiercingItem( j ), 1, 0, 0, 0 );
								if( FALSE == pQuerySave->Exec( szSql ) )
								{
									SAFE_DELETE( pQueryLoad );
									SAFE_DELETE( pQuerySave );
									SAFE_DELETE_ARRAY( szSql );
									AfxMessageBox( "insert ITEM_SEND_TBL", MB_OK );
									return FALSE;
								}
							}
						}
					}
					sprintf( szSql, "insert ITEM_SEND_TBL (m_idPlayer, serverindex, Item_Name, Item_count, m_nAbilityOption,  End_Time, m_bItemResist, m_nResistAbilityOption, m_bCharged) VALUES ('%07d','%02d','%d',%d,%d,NULL,%d,%d,1)",
						pMover->m_idPlayer, g_appInfo.dwSys, pItemElem->m_dwItemId, pItemElem->m_nItemNum, pItemElem->GetOption(), pItemElem->m_bItemResist, pItemElem->m_nResistAbilityOption );
					ItemProp* pItemProp		= prj.GetItemProp( pItemElem->m_dwItemId );
					TRACE( "%s\n", szSql );
					if( FALSE == pQuerySave->Exec( szSql ) )
					{
						SAFE_DELETE( pQueryLoad );
						SAFE_DELETE( pQuerySave );
						SAFE_DELETE_ARRAY( szSql );
						AfxMessageBox( "insert ITEM_SEND_TBL", MB_OK );
						return FALSE;
					}
					pMover->m_Bank[0].RemoveAtId( i );
				}
			}
			ItemContainerStruct icsBank;
			SaveBank( pMover, &pMover->m_Bank[0], &icsBank );

			sprintf( szSql, "UPDATE BANK_TBL"
				" SET m_Bank = '%s', m_apIndex_Bank = '%s', m_dwObjIndex_Bank = '%s'"
				" where m_idPlayer = '%07d' and serverindex = '%02d'",
				icsBank.szItem, icsBank.szIndex, icsBank.szObjIndex, pMover->m_idPlayer, g_appInfo.dwSys );
			if( FALSE == pQuerySave->Exec( szSql ) )
			{
				SAFE_DELETE( pQueryLoad );
				SAFE_DELETE( pQuerySave );
				SAFE_DELETE_ARRAY( szSql );
				AfxMessageBox( "UPDATE BANK_TBL", MB_OK );
				return FALSE;
			}		
			TRACE( "%s\n", szSql );
			sprintf( szSql, "UPDATE BANK_EXT_TBL"
				" SET m_extBank = '%s', m_BankPiercing = '%s'"
				" where m_idPlayer = '%07d' and serverindex = '%02d'",
				icsBank.szExt, icsBank.szPiercing, pMover->m_idPlayer, g_appInfo.dwSys );
			if( FALSE == pQuerySave->Exec( szSql ) )
			{
				SAFE_DELETE( pQueryLoad );
				SAFE_DELETE( pQuerySave );
				SAFE_DELETE_ARRAY( szSql );
				AfxMessageBox( "UPDATE BANK_EXT_TBL", MB_OK );
				return FALSE;
			}
			TRACE( "%s\n", szSql );
			s.GetToken();
		}
		SAFE_DELETE_ARRAY( szSql );
		return TRUE;
	}

	SAFE_DELETE_ARRAY( szSql );
	return FALSE;
}

BOOL CDbManager::InventoryToItemSendTbl( LPCSTR lpFileName )
{
	CQuery* pQueryLoad	= new CQuery;
	CQuery* pQuerySave	= new CQuery;

	if( FALSE == pQueryLoad->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
	{
		SAFE_DELETE( pQueryLoad );
		SAFE_DELETE( pQuerySave );
		AfxMessageBox( "CDbManager::InventoryToItemSendTblList", MB_OK );
		return FALSE;
	}
	if( FALSE == pQuerySave->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
	{
		SAFE_DELETE( pQueryLoad );
		SAFE_DELETE( pQuerySave );
		AfxMessageBox( "CDbManager::InventoryToItemSendTblList", MB_OK );
		return FALSE;
	}

	char* szSql		= new char[40960];

	CScanner s;
	if( s.Load( lpFileName ) )
	{
		s.GetToken();
		while( s.tok != FINISHED )
		{
			sprintf( szSql, "select a.m_idPlayer, a.serverindex, c.m_dwGold, a.m_Inventory, a.m_apIndex, a.m_dwObjIndex, a.m_adwEquipment, b.m_extInventory, b.m_InventoryPiercing  from INVENTORY_TBL a, INVENTORY_EXT_TBL b, CHARACTER_TBL c where a.m_idPlayer = b.m_idPlayer and b.m_idPlayer = c.m_idPlayer and a.serverindex = b.serverindex and b.serverindex = c.serverindex and a.m_idPlayer = '%07d' and a.serverindex = '%02d'"
				, atoi( (const char*)s.Token ), g_appInfo.dwSys );

			if( FALSE == pQueryLoad->Exec( szSql ) )
			{
				SAFE_DELETE( pQueryLoad );
				SAFE_DELETE( pQuerySave );
				SAFE_DELETE_ARRAY( szSql );
				AfxMessageBox( "Exec", MB_OK );
				return FALSE;
			}

			if( FALSE == pQueryLoad->Fetch() )
			{
				SAFE_DELETE( pQueryLoad );
				SAFE_DELETE( pQuerySave );
				SAFE_DELETE_ARRAY( szSql );
				AfxMessageBox( "Fetch", MB_OK );
				return FALSE;
			}
			CMover* pMover	= new CMover;
			pMover->m_idPlayer	= pQueryLoad->GetInt( "m_idPlayer" );
			DWORD dwGold	= pQueryLoad->GetInt( "m_dwGold" );
			pMover->SetGold( dwGold );
//			GetBankMover( pMover, pQueryLoad, 0 );

			sprintf( szSql, "insert ITEM_SEND_TBL (m_idPlayer, serverindex, Item_Name, Item_count ) VALUES ('%07d','%02d','penya', %d)",
				pMover->m_idPlayer, g_appInfo.dwSys, pMover->GetGold() );
			if( FALSE == pQuerySave->Exec( szSql ) )
			{
				SAFE_DELETE( pQueryLoad );
				SAFE_DELETE( pQuerySave );
				SAFE_DELETE_ARRAY( szSql );
				AfxMessageBox( "insert ITEM_SEND_TBL", MB_OK );
				return FALSE;
			}

			sprintf( szSql, "UPDATE CHARACTER_TBL SET m_dwGold = 0 where m_idPlayer = '%07d' and serverindex = '%02d'",
				pMover->m_idPlayer, g_appInfo.dwSys );
			if( FALSE == pQuerySave->Exec( szSql ) )
			{
				SAFE_DELETE( pQueryLoad );
				SAFE_DELETE( pQuerySave );
				SAFE_DELETE_ARRAY( szSql );
				AfxMessageBox( "insert ITEM_SEND_TBL", MB_OK );
				return FALSE;
			}


			LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus	= NULL;
			//GetInventory( pMover, pQueryLoad, lpDbOverlappedPlus );
			// mirchang_100416 VERIFYSTRING use return value
			if( GetInventory( pMover, pQueryLoad, lpDbOverlappedPlus ) == FALSE )
			{
				SAFE_DELETE( pQueryLoad );
				SAFE_DELETE( pQuerySave );
				SAFE_DELETE_ARRAY( szSql );
				AfxMessageBox( "insert ITEM_SEND_TBL", MB_OK );
				return FALSE;
			}
			// mirchang_100416

			int cbInventory	= pMover->m_Inventory.m_dwItemMax;
			for( int i = 0; i < cbInventory; i++ )
			{
				CItemElem* pItemElem	= pMover->m_Inventory.GetAtId( i );
				if( pItemElem )
				{
					FILEOUT( "..\\InventoryToItemSendTbl.log", "idPlayer:%07d\tItem:%s(%d)\tnItemNum:%d\tnPiercedSize:%d\tdwTime:%d", pMover->m_idPlayer, prj.GetItemProp( pItemElem->m_dwItemId )->szName, pItemElem->m_dwItemId, pItemElem->m_nItemNum, pItemElem->GetPiercingSize(), pItemElem->m_dwKeepTime );
					if( pItemElem->GetPiercingSize() > 0 )
					{
						for( int j = 0; j < pItemElem->GetPiercingSize(); j++ )
						{
							if( pItemElem->GetPiercingItem( j ) > 0 )
							{
								FILEOUT( "..\\InventoryToItemSendTbl.log", "adwItemId[%d]:%d", j, pItemElem->GetPiercingItem( j ) );
								sprintf( szSql, "insert ITEM_SEND_TBL (m_idPlayer, serverindex, Item_Name, Item_count, m_nAbilityOption,  End_Time, m_bItemResist, m_nResistAbilityOption, m_bCharged) VALUES ('%07d','%02d','%d',%d,%d,NULL,%d,%d,1)",
									pMover->m_idPlayer, g_appInfo.dwSys, pItemElem->GetPiercingItem( j ), 1, 0, 0, 0 );
								if( FALSE == pQuerySave->Exec( szSql ) )
								{
									SAFE_DELETE( pQueryLoad );
									SAFE_DELETE( pQuerySave );
									SAFE_DELETE_ARRAY( szSql );
									AfxMessageBox( "insert ITEM_SEND_TBL", MB_OK );
									return FALSE;
								}
							}
						}
					}
					sprintf( szSql, "insert ITEM_SEND_TBL (m_idPlayer, serverindex, Item_Name, Item_count, m_nAbilityOption,  End_Time, m_bItemResist, m_nResistAbilityOption, m_bCharged) VALUES ('%07d','%02d','%d',%d,%d,NULL,%d,%d,1)",
						pMover->m_idPlayer, g_appInfo.dwSys, pItemElem->m_dwItemId, pItemElem->m_nItemNum, pItemElem->GetOption(), pItemElem->m_bItemResist, pItemElem->m_nResistAbilityOption );
					ItemProp* pItemProp		= prj.GetItemProp( pItemElem->m_dwItemId );
//					TRACE( "%s\n", szSql );
					if( FALSE == pQuerySave->Exec( szSql ) )
					{
						SAFE_DELETE( pQueryLoad );
						SAFE_DELETE( pQuerySave );
						SAFE_DELETE_ARRAY( szSql );
						AfxMessageBox( "insert ITEM_SEND_TBL", MB_OK );
						return FALSE;
					}
					pMover->m_Inventory.RemoveAtId( i );
				}
			}
			ItemContainerStruct icsInventory;
			SaveInventory( pMover, &icsInventory );

			sprintf( szSql, "UPDATE INVENTORY_TBL"
				" SET m_Inventory = '%s', m_apIndex = '%s', m_dwObjIndex = '%s'"
				" where m_idPlayer = '%07d' and serverindex = '%02d'",
				icsInventory.szItem, icsInventory.szIndex, icsInventory.szObjIndex, pMover->m_idPlayer, g_appInfo.dwSys );
			if( FALSE == pQuerySave->Exec( szSql ) )
			{
				SAFE_DELETE( pQueryLoad );
				SAFE_DELETE( pQuerySave );
				SAFE_DELETE_ARRAY( szSql );
				AfxMessageBox( "UPDATE INVENTORY_TBL", MB_OK );
				return FALSE;
			}		

			sprintf( szSql, "UPDATE INVENTORY_EXT_TBL"
				" SET m_extInventory = '%s', m_InventoryPiercing = '%s'"
				" where m_idPlayer = '%07d' and serverindex = '%02d'",
				icsInventory.szExt, icsInventory.szPiercing, pMover->m_idPlayer, g_appInfo.dwSys );
			if( FALSE == pQuerySave->Exec( szSql ) )
			{
				SAFE_DELETE( pQueryLoad );
				SAFE_DELETE( pQuerySave );
				SAFE_DELETE_ARRAY( szSql );
				AfxMessageBox( "UPDATE INVENTORY_EXT_TBL", MB_OK );
				return FALSE;
			}
//			TRACE( "%s\n", szSql );
			s.GetToken();
		}
		SAFE_DELETE_ARRAY( szSql );
		return TRUE;
	}

	SAFE_DELETE_ARRAY( szSql );
	return FALSE;
}

BOOL CheckValidItem( DWORD dwItemId, short nNum )
{
		ItemProp* pItemProp	= prj.GetItemProp( dwItemId );
		if( NULL == pItemProp )
		{
			return FALSE;
		}
		else
		{
			if( nNum <= 0 || nNum > (short)pItemProp->dwPackMax )
				return FALSE;
		}
		return TRUE;
}

BOOL CDbManager::LoadPost( void )
{
	CPost *pPost = CPost::GetInstance();

	CMclAutoLock	Lock( pPost->m_csPost );
	if( m_qryPostProc.Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) == FALSE )
	{
		WriteLog( "%s, %d", __FILE__, __LINE__ );
		AfxMessageBox( "Error : Can't connect CHAR_DB" );
		return FALSE;
	}

#ifdef __POST_DUP_1209
	set<int>	setnMail;
	int	nTotal	= 0;
#endif	// __POST_DUP_1209

	MAIL_QUERYINFO info( "S1" );
	char szQuery[QUERY_SIZE]	= { 0, };
	DbQryMail( szQuery, info );
	if( FALSE == m_qryPostProc.Exec( szQuery ) )
	{
		AfxMessageBox( "QUERY: MAIL_STR 'S1'" );
		return FALSE;
	}

	while( m_qryPostProc.Fetch() )
	{
		u_long idReceiver	= m_qryPostProc.GetInt( "idReceiver" );
		CMail* pMail	= new CMail;
		pMail->m_nMail	= m_qryPostProc.GetInt( "nMail" );
		pMail->m_idSender	= m_qryPostProc.GetInt( "idSender" );

#ifdef __JEFF_11_2
		if( !CPlayerDataCenter::GetInstance()->GetPlayerData( idReceiver ) )
		{
			idReceiver	= pMail->m_idSender;
			pMail->m_idSender	= 0;
		}
#endif	// __JEFF_11_2

		int nGoldFlag	= m_qryPostProc.GetInt( "GoldFlag" );
		pMail->m_nGold	= 0;
		if( nGoldFlag == 0 )
			pMail->m_nGold	= m_qryPostProc.GetInt( "nGold" );
		pMail->m_tmCreate	= (time_t)m_qryPostProc.GetInt( "tmCreate" );
		pMail->m_byRead	= (BYTE)m_qryPostProc.GetInt( "byRead" );

		//	mulcom	BEGIN100420	
		//m_qryPostProc.GetStr( "szTitle", pMail->m_szTitle );
		char	szTempTitle[128];
		::memset( szTempTitle, 0, sizeof(szTempTitle) );
		m_qryPostProc.GetStr( "szTitle", szTempTitle );
		
		::memcpy( pMail->m_szTitle, szTempTitle, sizeof(pMail->m_szTitle) );
		pMail->m_szTitle[MAX_MAILTITLE-1] = '\0';
		//	mulcom	END100420	


		m_qryPostProc.GetStr( "szText", pMail->m_szText );

		int nItemFlag	= m_qryPostProc.GetInt( "ItemFlag" );
		DWORD dwItemId	= m_qryPostProc.GetInt( "dwItemId" );

		if( dwItemId && nItemFlag == 0 )
		{
			pMail->m_pItemElem	= new CItemElem;
			pMail->m_pItemElem->m_dwItemId	= dwItemId;
			GetItemFromMail( &m_qryPostProc, pMail->m_pItemElem );

			if( CheckValidItem( dwItemId, pMail->m_pItemElem->m_nItemNum ) == FALSE )
			{
#ifdef __POST_DUP_1209
				nTotal++;
#endif // __POST_DUP_1209
				SAFE_DELETE( pMail );
				continue;
			}
		}
#ifdef __POST_DUP_1209
		bool bResult	= setnMail.insert( pMail->m_nMail ).second;
		if( !bResult )
		{
			AfxMessageBox( "CDbManager.LoadPost: duplicated" );
			return FALSE;
		}
#endif	// __POST_DUP_1209

		if( FALSE == CPost::GetInstance()->AddMail( idReceiver, pMail ) )
		{
			AfxMessageBox( "ERROR: LoadPost: AddMail" );
			return FALSE;
		}
#ifdef __POST_DUP_1209
		nTotal++;
#endif	// __POST_DUP_1209
	}

	sprintf( szQuery, "uspLoadMaxMailID @pserverindex='%02d'", g_appInfo.dwSys );
	if( FALSE == m_qryPostProc.Exec( szQuery ) )
	{
		AfxMessageBox( "QUERY: uspLoadMaxMailID" );
		return FALSE;
	}
	if( m_qryPostProc.Fetch() )
	{
		int nMaxMailID	= m_qryPostProc.GetInt( "MaxMailID" );
#ifdef __POST_DUP_1209
		int nCount	= m_qryPostProc.GetInt( "nCount" );
		if( nTotal != nCount )
		{
			char szTemp[255] = {0,};
			sprintf( szTemp, "CDbManager.LoadPost: count -> count : %d, nTotal : %d", nCount, nTotal );
			AfxMessageBox( szTemp );
			return FALSE;
		}
#endif	// __POST_DUP_1209
		if( (int)( CMail::s_nMail ) > nMaxMailID )
		{
			AfxMessageBox( "MaxMailID is not valid" );
			return FALSE;
		}
		CMail::s_nMail	= nMaxMailID;
	}
	return TRUE;
}

void CDbManager::GetItemFromMail( CQuery* pQuery, CItemElem* pItemElem )
{
	pItemElem->m_bCharged	= pQuery->GetInt( "bCharged" );
	if( pItemElem->m_bCharged != 1 )
		pItemElem->m_bCharged	= 0;
	pItemElem->m_bItemResist	= pQuery->GetInt( "bItemResist" );
	pItemElem->m_byFlag	= pQuery->GetInt( "byFlag" );
	pItemElem->m_dwKeepTime	= pQuery->GetInt( "dwKeepTime" );
	pItemElem->SetSerialNumber( pQuery->GetSerialNumber( "dwSerialNumber" ) );
	pItemElem->m_idGuild	= pQuery->GetInt( "idGuild" );
	pItemElem->SetOption( pQuery->GetInt( "nOption" ) );
	pItemElem->m_nHitPoint	= pQuery->GetInt( "nHitPoint" );
	pItemElem->m_nRepair = pQuery->GetInt( "nMaxHitPoint" );
	pItemElem->m_nItemNum	= pQuery->GetInt( "nItemNum" );
#if __VER >= 11 // __SYS_IDENTIFY
	__int64 iRandomOptItemId	= pQuery->GetInt64( "nRandomOptItemId" );
	if( iRandomOptItemId == -102 )
		iRandomOptItemId	= 0;
	pItemElem->SetRandomOptItemId( iRandomOptItemId );
#else	// __SYS_IDENTIFY
	pItemElem->SetRandomOptItemId( pQuery->GetInt( "nRandomOptItemId" ) );
#endif	// __SYS_IDENTIFY
	pItemElem->m_nRepairNumber	= pQuery->GetInt( "nRepairNumber" );
	pItemElem->m_nResistAbilityOption	= pQuery->GetInt( "nResistAbilityOption" );
	pItemElem->m_nResistSMItemId	= pQuery->GetInt( "nResistSMItemId" );
#if __VER >= 12 // __EXT_PIERCING
	GetPiercingInfoFromMail( pQuery, pItemElem );
#else // __EXT_PIERCING
	pItemElem->SetPiercingSize( pQuery->GetInt( "nPiercedSize" ) );
	pItemElem->SetPiercingItem( 0, pQuery->GetInt( "dwItemId1" ) );
	pItemElem->SetPiercingItem( 1, pQuery->GetInt( "dwItemId2" ) );
	pItemElem->SetPiercingItem( 2, pQuery->GetInt( "dwItemId3" ) );
	pItemElem->SetPiercingItem( 3, pQuery->GetInt( "dwItemId4" ) );
#if __VER >= 9 // __ULTIMATE
	pItemElem->SetPiercingItem( 4, pQuery->GetInt( "dwItemId5" ) );
#endif // __ULTIMATE
#endif // __EXT_PIERCING
#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
	if( pItemElem->m_dwItemId == II_SYS_SYS_SCR_SEALCHARACTER )
	{
#if __VER >= 11 // __SYS_PLAYER_DATA
		CPlayerDataCenter::GetInstance()->m_Access.Enter();
		const char*	lpszPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( pItemElem->m_nHitPoint );
#else	// __SYS_PLAYER_DATA
		LPCSTR lpszPlayer	= prj.GetPlayerString( pItemElem->m_nHitPoint );
#endif	// __SYS_PLAYER_DATA
		if(lpszPlayer != NULL)
			memcpy(pItemElem->m_szItemText,lpszPlayer,sizeof(pItemElem->m_szItemText));
#if __VER >= 11 // __SYS_PLAYER_DATA
		CPlayerDataCenter::GetInstance()->m_Access.Leave();
#endif	// __SYS_PLAYER_DATA
	}
#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz

	BOOL bPet	= pQuery->GetInt( "bPet" );
	if( bPet )
	{
		SAFE_DELETE( pItemElem->m_pPet );
		CPet* pPet	= pItemElem->m_pPet	= new CPet;
		BYTE nKind	= (BYTE)pQuery->GetInt( "nKind" );
		BYTE nLevel		= (BYTE)pQuery->GetInt( "nLevel" );
		DWORD dwExp	= (DWORD)pQuery->GetInt( "dwExp" );
		WORD wEnergy	= (WORD)pQuery->GetInt( "wEnergy" );
		WORD wLife	= (WORD)pQuery->GetInt( "wLife" );
		BYTE anAvailLevel[PL_MAX];
		anAvailLevel[PL_D]	= (BYTE)pQuery->GetInt( "anAvailLevel_D" );
		anAvailLevel[PL_C]	= (BYTE)pQuery->GetInt( "anAvailLevel_C" );
		anAvailLevel[PL_B]	= (BYTE)pQuery->GetInt( "anAvailLevel_B" );
		anAvailLevel[PL_A]	= (BYTE)pQuery->GetInt( "anAvailLevel_A" );
		anAvailLevel[PL_S]	= (BYTE)pQuery->GetInt( "anAvailLevel_S" );
#ifdef __PET_1024
		char szPetName[MAX_PET_NAME]	= { 0,};
		pQuery->GetStr( "szPetName", szPetName );
		if( strcmp( szPetName, "NULL" ) == 0 )
			szPetName[0]	= '\0';
#endif	// __PET_1024
		pPet->SetKind( nKind );
		pPet->SetLevel( nLevel );
		pPet->SetExp( dwExp );
		pPet->SetEnergy( wEnergy );
		pPet->SetLife( wLife );
		pPet->SetAvailLevel( PL_D, anAvailLevel[PL_D] );
		pPet->SetAvailLevel( PL_C, anAvailLevel[PL_C] );
		pPet->SetAvailLevel( PL_B, anAvailLevel[PL_B] );
		pPet->SetAvailLevel( PL_A, anAvailLevel[PL_A] );
		pPet->SetAvailLevel( PL_S, anAvailLevel[PL_S] );
#ifdef __PET_1024
		pPet->SetName( szPetName );
#endif	// __PET_1024
	}
}

void CDbManager::AddMail( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov )
{
// 	//	BEGINTEST
// 	Error( "CDbManager::AddMail" );

	CAr ar( pov->lpBuf, pov->uBufSize );
	u_long idReceiver;
	CMail* pMail	= new CMail;
	pMail->m_tmCreate	= time( NULL );
	ar >> idReceiver >> pMail->m_idSender;
	BYTE byItem;
	ar >> byItem;
	if( byItem )
	{
		pMail->m_pItemElem	= new CItemElem;
		pMail->m_pItemElem->Serialize( ar );
	}
	ar >> pMail->m_nGold;
	ar.ReadString( pMail->m_szTitle, MAX_MAILTITLE );
	ar.ReadString( pMail->m_szText, MAX_MAILTEXT );

	CPost *pPost    = CPost::GetInstance();

//	CMclAutoLock	Lock( pPost->m_csPost );
	pPost->m_csPost.Enter();
	BOOL bResult	= CPost::GetInstance()->AddMail( idReceiver, pMail ) > 0;
	pPost->m_csPost.Leave();

	if( bResult )
	{
		char szQuery[QUERY_SIZE]	= { 0,};
		CDbManager::MakeQueryAddMail( szQuery, pMail, idReceiver );

		SQLINTEGER cbLen	= SQL_NTS;

		if( pQuery->BindParameter( 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 128, 0, (void*)pMail->m_szTitle, 0, &cbLen ) == FALSE
			|| pQuery->BindParameter( 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 1024, 0, (void*)pMail->m_szText, 0, &cbLen ) == FALSE )
		{
			Error( "QUERY: PACKETTYPE_QUERYPOSTMAIL" );
			CDPTrans::GetInstance()->SendPostMail( FALSE, idReceiver, pMail );
			pPost->m_csPost.Enter();
			CMailBox* pMailBox	= pPost->GetMailBox( idReceiver );
			pMailBox->RemoveMail( pMail->m_nMail );
			pPost->m_csPost.Leave();
			FreeRequest( pov );
			return;
		}

		if( FALSE == pQuery->Exec( szQuery ) )
		{
			Error( "QUERY: PACKETTYPE_QUERYPOSTMAIL" );
			CDPTrans::GetInstance()->SendPostMail( FALSE, idReceiver, pMail );
			pPost->m_csPost.Enter();
			CMailBox* pMailBox	= pPost->GetMailBox( idReceiver );
			pMailBox->RemoveMail( pMail->m_nMail );
			pPost->m_csPost.Leave();
			FreeRequest( pov );
			return;
		}
	}
	else
	{
		Error( "idSender : %d, pMail->m_nMail : %d", pMail->m_idSender, pMail->m_nMail );
	}
	
	CDPTrans::GetInstance()->SendPostMail( TRUE, idReceiver, pMail );
	FreeRequest( pov );
}

void CDbManager::RemoveMail( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov )
{
	CAr ar( pov->lpBuf, pov->uBufSize );

	u_long idReceiver, nMail;
	ar >> idReceiver >> nMail;

	CPost *pPost    = CPost::GetInstance();
//	CMclAutoLock	Lock( pPost->m_csPost );
	pPost->m_csPost.Enter();	// l

	CMailBox* pMailBox	= pPost->GetMailBox( idReceiver );
	if( pMailBox )
	{
		pMailBox->RemoveMail( nMail );
		pPost->m_csPost.Leave();	// u
		MAIL_QUERYINFO info( "D1" );
		info.nMail	= nMail;
		char szQuery[QUERY_SIZE]	= { 0,};
		DbQryMail( szQuery, info );
		if( FALSE == pQuery->Exec( szQuery ) )
			Error( "QUERY: PACKETTYPE_QUERYREMOVEMAIL" );
		else
			CDPTrans::GetInstance()->SendRemoveMail( idReceiver, nMail );
	}
	else
		pPost->m_csPost.Leave();	// u

	FreeRequest( pov );
}

#ifdef __POST_1204
void CDbManager::RemoveMail( list<CMail*> & lspMail, time_t t )
{
	char szQuery[QUERY_SIZE]	= { 0,};
	MAIL_QUERYINFO info( "D2" );
	info.tmCreate	= t;
	DbQryMail( szQuery, info );
	if( m_qryPostProc.Exec( szQuery ) )
	{
		for( list<CMail*>::iterator i = lspMail.begin(); i != lspMail.end(); ++i )
		{
			CMail* pMail	= *i;
			CMailBox* pMailBox	= pMail->GetMailBox();
			CDPTrans::GetInstance()->SendRemoveMail( pMailBox->m_idReceiver, pMail->m_nMail );
			pMailBox->RemoveMail( pMail->m_nMail );
		}
	}
}
#else	// __POST_1204
//{{AFX
void CDbManager::RemoveMail( list<CMail*> & lspMail )
{
	char szQuery[QUERY_SIZE]	= { 0,};
	for( list<CMail*>::iterator i = lspMail.begin(); i != lspMail.end(); ++i )
	{
		CMail* pMail	= *i;
		MAIL_QUERYINFO info( "D1" );
		info.nMail	= pMail->m_nMail;
		DbQryMail( szQuery, info );
		if( m_qryPostProc.Exec( szQuery ) )
		{
			CMailBox* pMailBox	= pMail->GetMailBox();
			pMailBox->RemoveMail( info.nMail );
			CDPTrans::GetInstance()->SendRemoveMail( pMailBox->m_idReceiver, info.nMail );
		}
	}
}
//}}AFX
#endif	// __POST_1204

void CDbManager::RemoveMailItem( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov )
{
	CAr ar( pov->lpBuf, pov->uBufSize );

	u_long idReceiver, nMail, uQuery;
	ar >> idReceiver >> nMail >> uQuery;

	CPost* pPost = CPost::GetInstance();
//	CMclAutoLock	Lock( pPost->m_csPost );
	pPost->m_csPost.Enter();	// l

	CMailBox* pMailBox	= pPost->GetMailBox( idReceiver );
	if( pMailBox )
	{
		pMailBox->RemoveMailItem( nMail );
		pPost->m_csPost.Leave();	// u
		MAIL_QUERYINFO info( "U1" );
		info.nMail	= nMail;
		char szQuery[QUERY_SIZE]	= { 0,};
		DbQryMail( szQuery, info );
		if( FALSE == pQuery->Exec( szQuery ) )
			Error( "QUERY: PACKETTYPE_QUERYGETMAILITEM" );
		else
			CDPTrans::GetInstance()->SendGetMailItem( idReceiver, nMail, uQuery );
	}
	else
		pPost->m_csPost.Leave();	// u
	FreeRequest( pov );
}

void CDbManager::RemoveMailGold( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov )
{
	CAr ar( pov->lpBuf, pov->uBufSize );

	u_long idReceiver, nMail, uQuery;
	ar >> idReceiver >> nMail >> uQuery;

	CPost* pPost = CPost::GetInstance();
//	CMclAutoLock	Lock( pPost->m_csPost );
	pPost->m_csPost.Enter();	// l

	CMailBox* pMailBox	= pPost->GetMailBox( idReceiver );
	if( pMailBox )
	{
		pMailBox->RemoveMailGold( nMail );
		pPost->m_csPost.Leave();	// u
		MAIL_QUERYINFO info( "U2" );
		info.nMail	= nMail;
		char szQuery[QUERY_SIZE]	= { 0,};
		DbQryMail( szQuery, info );
		if( FALSE == pQuery->Exec( szQuery ) )
			Error( "QUERY: PACKETTYPE_QUERYGETMAILITEM" );
		else
			CDPTrans::GetInstance()->SendGetMailGold( idReceiver, nMail, uQuery );
	}
	else
		pPost->m_csPost.Leave();	// u
	FreeRequest( pov );
}

void CDbManager::ReadMail( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov )
{
	CAr ar( pov->lpBuf, pov->uBufSize );

	u_long idReceiver, nMail;
	ar >> idReceiver >> nMail;

	CPost* pPost = CPost::GetInstance();
//	CMclAutoLock	Lock( pPost->m_csPost );
	pPost->m_csPost.Enter();	// l

	CMailBox* pMailBox	= pPost->GetMailBox( idReceiver );
	if( pMailBox )
	{
		pMailBox->ReadMail( nMail );
		pPost->m_csPost.Leave();	// u
		MAIL_QUERYINFO info( "U3" );
		info.nMail	= nMail;
		char szQuery[QUERY_SIZE]	= { 0,};
		DbQryMail( szQuery, info );
		if( FALSE == pQuery->Exec( szQuery ) )
			Error( "QUERY: PACKETTYPE_READMAIL" );
		else
			CDPTrans::GetInstance()->SendReadMail( idReceiver, nMail );
	}
	else
	{
		pPost->m_csPost.Leave();	// u
		Error( "CDbManager::ReadMail - pMailBox is NULL. idReceiver : %07d", idReceiver );
	}
	FreeRequest( pov );
}

int	CDbManager::GetTradeNo( void )
{
	time_t	t	= time( NULL );
	t	&= 0xffffffe0;
	LONG lCount	= InterlockedIncrement( &m_cbTrade );
	lCount	&= 0x0000001f;
	return (int)( t + lCount );
}

BOOL CDbManager::RestorePet( LPCSTR lpFileName )
{
	map<DWORD, int>	mRestore;
	CScript s;
	if( FALSE == s.Load( lpFileName ) )
		return FALSE;

	SERIALNUMBER dwSerial	= s.GetNumber();
	while( s.tok != FINISHED )
	{
		int nLife	= s.GetNumber();
		BOOL bResult	= mRestore.insert( map<DWORD, int>::value_type( dwSerial, nLife ) ).second;
		dwSerial	= s.GetNumber();
	}

	RestorePetInventory( mRestore );
	RestorePetBank( mRestore );
	RestorePetGuildBank( mRestore );

	for( map<DWORD, int>::iterator i = mRestore.begin(); i != mRestore.end(); ++i )
		FILEOUT( "../result.txt", "%07d\t%d", i->first, i->second );

	return TRUE;
}

BOOL CDbManager::Conv( LPCSTR lpFileName )
{
#if __VER >= 11 // __SYS_COLLECTING
	map<DWORD, CONV_RESULT_ITEM>	mConv;
#else	// __SYS_COLLECTING
	map<DWORD, DWORD>	mConv;
#endif	// __SYS_COLLECTING
	CScript s;
	if( FALSE == s.Load( lpFileName ) )
	{
		return FALSE;
	}

	DWORD dwItemId1	= s.GetNumber();
	while( s.tok != FINISHED )
	{
#if __VER >= 11 // __SYS_COLLECTING
		CONV_RESULT_ITEM	result;
		result.dwItemId		= s.GetNumber();
		result.nAbilityOption	= s.GetNumber();
		BOOL bResult	= mConv.insert( map<DWORD, CONV_RESULT_ITEM>::value_type( dwItemId1, result ) ).second;
#else	// __SYS_COLLECTING
		DWORD dwItemId2	= s.GetNumber();
		BOOL bResult	= mConv.insert( map<DWORD, DWORD>::value_type( dwItemId1, dwItemId2 ) ).second;
#endif	// __SYS_COLLECTING
#ifdef __VERIFY_0201
		/*
		ItemProp* pItemProp1	= prj.GetItemProp( dwItemId1 );
		ItemProp* pItemProp2	= prj.GetItemProp( dwItemId2 );
		if( pItemProp1 && pItemProp2 )
		{
			if( lstrcmp( pItemProp1->szIcon, pItemProp2->szIcon ) == 0 )
			{
				if( pItemProp1->dwLimitLevel1 > pItemProp2->dwLimitLevel1 )
				{
					TRACE( "level, %d, %d\n", dwItemId1, dwItemId2 );
				}
			}
			else
			{
				TRACE( "icon is different, %d, %d\n", dwItemId1, dwItemId2 );
			}
		}
		else
		{
			TRACE( "no property, %d or %d\n", dwItemId1, dwItemId2 );
		}
		*/
#endif	// __VERIFY_0201
		dwItemId1	= s.GetNumber();
	}
#ifdef __VERIFY_0201
	CScript s1;
	if( FALSE == s1.Load( "item.txt", FALSE ) )
	{
		return FALSE;
	}
	CStringArray	strArray;
	s1.GetToken_NoDef();
	while( s1.tok != FINISHED )
	{
		CString str	= s1.Token;
		strArray.Add( str );
		s1.GetToken_NoDef();
	}

	CScript s2;
	if( FALSE == s2.Load( "file.txt", FALSE ) )
	{
		return FALSE;
	}
	CStringArray	strFileArray;
	s2.GetToken();
	while( s2.tok != FINISHED )
	{
		CString str	= s2.Token;
		strFileArray.Add( str );
		s2.GetToken();
	}
	for( int i = 0; i < strFileArray.GetSize(); i++ )
	{
		CScript s3;
		if( FALSE == s3.Load( strFileArray.GetAt( i ), i <= 13 ) )
		{
			return FALSE;
		}
		CString strFileData	= s3.m_pProg;
		for( int j = 0; j < strArray.GetSize(); j++ )
		{
			int n	= strFileData.Find( strArray.GetAt( j ), 0 );
			while( n >= 0 )
			{
				int nCheck	= n + strArray.GetAt( j ).GetLength();
				char c	= strFileData.GetAt( nCheck );
				if( c > 'z' || c < '0' )
					TRACE( "%s occurs in %s\n", strArray.GetAt( j ), strFileArray.GetAt( i ) );
				n	= strFileData.Find( strArray.GetAt( j ), n+1 );
			}
		}
	}
#else	__VERIFY_0201
	ConvInventory( mConv );
	ConvBank( mConv );
	ConvGuildBank( mConv );
#if __VER >= 12
	ConvertPocket( mConv );
#endif	// 12
#endif	// __VERIFY_0201

	return TRUE;
}

BOOL CDbManager::RemoveInvalidItem( void )
{
	// 쿼리 객체 생성
	CQuery* pQueryLoad	= new CQuery;
	if( FALSE == pQueryLoad->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
	{
		safe_delete( pQueryLoad );
		return FALSE;
	}
	// 인벤토리 검사 후 제거
	if( !pQueryLoad->Execute( "SELECT * FROM INVENTORY_TBL a, INVENTORY_EXT_TBL b WHERE b.serverindex = a.serverindex and a.m_idPlayer = b.m_idPlayer" ) )
	{
		safe_delete( pQueryLoad );
		return FALSE;
	}
	FILEOUT( "../INVENTORY_TBL.txt", "m_idPlayer\tserverindex\tm_Inventory\tm_apIndex\tm_dwObjIndex" );
	FILEOUT( "../INVENTORY_EXT_TBL.txt", "m_idPlayer\tserverindex\tm_extInventory\tm_InventoryPiercing\tszInventoryPet" );

	while( pQueryLoad->Fetch() )
	{
		int nServer	= pQueryLoad->GetInt( "serverindex" );
		BOOL bUpdate	= FALSE;
		CMover* pMover	= new CMover;
		pMover->m_idPlayer	= pQueryLoad->GetInt( "m_idPlayer" );
		//GetInventory( pMover, pQueryLoad, NULL );
		// mirchang_100416 VERIFYSTRING use return value
		if( GetInventory( pMover, pQueryLoad, NULL ) == FALSE )
		{
			safe_delete( pMover );
			safe_delete( pQueryLoad );
			return FALSE;
		}
		// mirchang_100416

		for( DWORD i = 0; i < pMover->m_Inventory.m_dwItemMax; i++ )
		{
			CItemElem* pItem	= pMover->m_Inventory.GetAtId( i );
			if( pItem && pItem->IsInvalid() )
			{
				pMover->m_Inventory.RemoveAtId( i );
				bUpdate		= TRUE;
			}
		}
		if( bUpdate )
		{
			ItemContainerStruct is;
			SaveInventory( pMover, &is );
			FILEOUT( "../INVENTORY_TBL.txt", "%07d\t%02d\t%s\t%s\t%s",
				pMover->m_idPlayer,
				nServer,
				is.szItem,
				is.szIndex,
				is.szObjIndex );
			FILEOUT( "../INVENTORY_EXT_TBL.txt", "%07d\t%02d\t%s\t%s\t%s",
				pMover->m_idPlayer,
				nServer,
				is.szExt,
				is.szPiercing,
				is.szPet );
		}
		safe_delete( pMover );
	}

	// 은행 검사 후 제거
	if( !pQueryLoad->Execute( "SELECT * FROM BANK_TBL a, BANK_EXT_TBL b WHERE b.serverindex = a.serverindex and a.m_idPlayer = b.m_idPlayer" ) )
	{
		safe_delete( pQueryLoad );
		return FALSE;
	}
	FILEOUT( "../BANK_TBL.txt", "m_idPlayer\tserverindex\tm_Bank\tm_apIndex_Bank\tm_dwObjIndex_Bank" );
	FILEOUT( "../BANK_EXT_TBL.txt", "m_idPlayer\tserverindex\tm_extBank\tm_BankPiercing\tszBankPet" );
	while( pQueryLoad->Fetch() )
	{
		int nServer	= pQueryLoad->GetInt( "serverindex" );
		BOOL bUpdate	= FALSE;
		CMover* pMover	= new CMover;
		pMover->m_idPlayer	= pQueryLoad->GetInt( "m_idPlayer" );
		GetBankMover( pMover, pQueryLoad, 0 );

		for( DWORD i = 0; i < pMover->m_Bank[0].m_dwItemMax; i++ )
		{
			CItemElem* pItem	= pMover->m_Bank[0].GetAtId( i );
			if( pItem && pItem->IsInvalid() )
			{
				pMover->m_Bank[0].RemoveAtId( i );
				bUpdate	= TRUE;
			}
		}
		if( bUpdate )
		{
			ItemContainerStruct is;
			SaveBank( pMover, &pMover->m_Bank[0], &is );
			FILEOUT( "../BANK_TBL.txt", "%07d\t%02d\t%s\t%s\t%s",
				pMover->m_idPlayer,
				nServer,
				is.szItem,
				is.szIndex,
				is.szObjIndex );
			FILEOUT( "../BANK_EXT_TBL.txt", "%07d\t%02d\t%s\t%s\t%s",
				pMover->m_idPlayer,
				nServer,
				is.szExt, 
				is.szPiercing,
				is.szPet );
		}
		safe_delete( pMover );
	}

	// 길드 창고 검사 후 제거
	if( !pQueryLoad->Execute( "SELECT * FROM GUILD_BANK_TBL a, GUILD_BANK_EXT_TBL b WHERE a.serverindex = b.serverindex and a.m_idGuild = b.m_idGuild" ) )
	{
		safe_delete( pQueryLoad );
		return FALSE;
	}
	FILEOUT( "../GUILD_BANK_TBL.txt", "m_idGuild\tserverindex\tm_apIndex\tm_dwObjIndex\tm_GuildBank" );
	FILEOUT( "../GUILD_BANK_EXT_TBL.txt", "m_idGuild\tserverindex\tm_extGuildBank\tm_GuildBankPiercing\tszGuildBankPet" );

	while( pQueryLoad->Fetch() )
	{
		BOOL bUpdate	= FALSE;
		CItemContainer<CItemElem>	GuildBank;
		int	nGuildId	= 0;
		GuildBank.SetItemContainer( ITYPE_ITEM, MAX_GUILDBANK );

		int nServer	= pQueryLoad->GetInt( "serverindex" );
		nGuildId	= pQueryLoad->GetInt( "m_idGuild" );
		GetGuildBank( &GuildBank, pQueryLoad );
		for( DWORD i = 0; i < GuildBank.m_dwItemMax; i++ )
		{
			CItemElem* pItem	= GuildBank.GetAtId( i );
			if( pItem && pItem->IsInvalid() )
			{
				GuildBank.RemoveAtId( i );
				bUpdate	= TRUE;
			}
		}
		if( bUpdate )
		{
			ItemContainerStruct is;
			SaveGuildBank( &GuildBank, &is );
			FILEOUT( "../GUILD_BANK_TBL.txt", "%06d\t%02d\t%s\t%s\t%s",
				nGuildId,
				nServer,
				is.szIndex,
				is.szObjIndex,
				is.szItem );
			FILEOUT( "../GUILD_BANK_EXT_TBL.txt", "%06d\t%02d\t%s\t%s\t%s",
				nGuildId,
				nServer,
				is.szExt,
				is.szPiercing,
				is.szPet );
		}
	}

	// 휴대용 가방 검사 후 제거
	if( !pQueryLoad->Execute( "SELECT * FROM tblPocket A, tblPocketExt B WHERE A.serverindex = B.serverindex AND A.idPlayer = B.idPlayer AND A.nPocket = B.nPocket ORDER BY A.idPlayer, A.nPocket" ) )
	{
		safe_delete( pQueryLoad );
		return FALSE;
	}
	FILEOUT( "../tblPocket.txt", "serverindex\tidPlayer\tnPocket\tszItem\tszIndex\tszObjIndex" );
	FILEOUT( "../tblPocketExt.txt", "serverindex\tidPlayer\tnPocket\tszExt\tszPiercing\tszPet" );
	while( pQueryLoad->Fetch() )
	{
		int nServer		= pQueryLoad->GetInt( "serverindex" );
		BOOL bUpdate	= FALSE;
		CMover* pMover	= new CMover;
		pMover->m_idPlayer	= pQueryLoad->GetInt( "idPlayer" );
		int nPocket		= pQueryLoad->GetInt( "nPocket" );
		//GetPocket( pMover, pQueryLoad, NULL );
		// mirchang_100416 VERIFYSTRING use return value
		if( GetPocket( pMover, pQueryLoad, NULL ) == FALSE )
		{
			safe_delete( pMover );
			safe_delete( pQueryLoad );
			return FALSE;
		}
		// mirchang_100416
		CPocket* pPocket	= pMover->m_Pocket.GetPocket( nPocket );
		if( !pPocket )
		{
			safe_delete( pMover );
			continue;
		}
		for( DWORD i = 0; i < pPocket->m_dwItemMax; i++ )
		{
			CItemElem* pItem	= pPocket->GetAtId( i );
			if( pItem && pItem->IsInvalid() )
			{
				pPocket->RemoveAtId( i );
				bUpdate	= TRUE;
			}
		}
		if( bUpdate )
		{
			PocketStruct aPocketStruct[3];
			SavePocket( pMover, aPocketStruct );
			FILEOUT( "../tblPocket.txt", "%02d\t%07d\t%d\t%s\t%s\t%s",
				nServer,
				pMover->m_idPlayer,
				nPocket,
				aPocketStruct[nPocket].szItem,
				aPocketStruct[nPocket].szIndex,
				aPocketStruct[nPocket].szObjIndex );
			FILEOUT( "../tblPocketExt.txt", "%02d\t%07d\t%d\t%s\t%s\t%s",
				nServer,
				pMover->m_idPlayer,
				nPocket,
				aPocketStruct[nPocket].szExt,
				aPocketStruct[nPocket].szPiercing,
				aPocketStruct[nPocket].szPet );
		}
		safe_delete( pMover );
	}

	safe_delete( pQueryLoad );
	return TRUE;
}

#if __VER >= 12
CQuery* CDbManager::CreateCharQuery( void )
{
	CQuery* pQuery	= new CQuery;
	if( pQuery->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
		return pQuery;
	safe_delete( pQuery );
	return NULL;
}

BOOL CDbManager::ConvertPocket( MDC & mConv )
{
	CQuery* pQuery	= CreateCharQuery();
	if( !pQuery )
	{
		Error( "CDbManager.ConvertPocket: CreateCharQuery:" );
		return FALSE;
	}

	if( !pQuery->Execute( "SELECT * FROM tblPocket A, tblPocketExt B WHERE A.serverindex = B.serverindex AND A.idPlayer = B.idPlayer AND A.nPocket = B.nPocket ORDER BY A.idPlayer, A.nPocket" ) )
	{
		safe_delete( pQuery );
		Error( "CDbManager.ConvertPocket: CQuery.Execute:" );
		return FALSE;
	}
	FILEOUT( "../tblPocket.txt", "serverindex\tidPlayer\tnPocket\tszItem" );
	while( pQuery->Fetch() )
	{
		int nServer		= pQuery->GetInt( "serverindex" );
		BOOL bUpdate	= FALSE;
		CMover* pMover	= new CMover;
		pMover->m_idPlayer	= pQuery->GetInt( "idPlayer" );
		int nPocket		= pQuery->GetInt( "nPocket" );
		//GetPocket( pMover, pQuery, NULL );
		// mirchang_100416 VERIFYSTRING use return value
		if( GetPocket( pMover, pQuery, NULL ) == FALSE )
		{
			safe_delete( pMover );
			safe_delete( pQuery );
			return FALSE;
		}
		// mirchang_100416
		CPocket* pPocket	= pMover->m_Pocket.GetPocket( nPocket );
		if( !pPocket )
		{
			safe_delete( pMover );
			continue;
		}
		for( DWORD i = 0; i < pPocket->m_dwItemMax; i++ )
		{
			CItemElem* pItem	= pPocket->GetAtId( i );
			if( pItem )
			{
				MDC::iterator iter	= mConv.find( pItem->m_dwItemId );
				if( iter != mConv.end() )
				{
					bUpdate	= TRUE;
					pItem->m_dwItemId	= iter->second.dwItemId;
					if( iter->second.nAbilityOption >= 0 )
						pItem->SetOption( iter->second.nAbilityOption );
				}
			}
		}
		if( bUpdate )
		{
			PocketStruct aPocketStruct[3];
			SavePocket( pMover, aPocketStruct );
			FILEOUT( "../tblPocket.txt", "%02d\t%07d\t%d\t%s",
				nServer, pMover->m_idPlayer, nPocket, aPocketStruct[nPocket].szItem );
		}
		safe_delete( pMover );
	}

	safe_delete( pQuery );
	return TRUE;
}
#endif	// 12

#if __VER >= 11 // __SYS_COLLECTING
BOOL CDbManager::ConvInventory( map<DWORD, CONV_RESULT_ITEM> & mConv )
#else	// __SYS_COLLECTING
BOOL CDbManager::ConvInventory( map<DWORD, DWORD> & mConv )
#endif	// __SYS_COLLECTING
{
	CQuery* pQueryLoad	= new CQuery;

	if( FALSE == pQueryLoad->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
	{
		SAFE_DELETE( pQueryLoad );
		AfxMessageBox( "CDbManager::ConvInventory", MB_OK );
		return FALSE;
	}

	char szQuery[200]	= { 0,};
//	sprintf( szQuery, "SELECT * FROM INVENTORY_TBL a, INVENTORY_EXT_TBL b WHERE a.serverindex = '%02d' and b.serverindex = a.serverindex and a.m_idPlayer = b.m_idPlayer", g_appInfo.dwSys );
	sprintf( szQuery, "SELECT * FROM INVENTORY_TBL a, INVENTORY_EXT_TBL b WHERE b.serverindex = a.serverindex and a.m_idPlayer = b.m_idPlayer" );

	if( FALSE == pQueryLoad->Exec( szQuery ) )
	{
		SAFE_DELETE( pQueryLoad );
		AfxMessageBox( "Exec", MB_OK );
		return FALSE;
	}

#ifdef __CONV_0906
	FILEOUT( "../INVENTORY_TBL.txt", "serverindex\tm_idPlayer\tm_Inventory" );
#endif	// __CONV_0906

	while( pQueryLoad->Fetch() )
	{
		int nServer	= pQueryLoad->GetInt( "serverindex" );
		BOOL bUpdate	= FALSE;
		CMover* pMover	= new CMover;
		pMover->m_idPlayer	= pQueryLoad->GetInt( "m_idPlayer" );

		//GetInventory( pMover, pQueryLoad, NULL );
		// mirchang_100416 VERIFYSTRING use return value
		if( GetInventory( pMover, pQueryLoad, NULL ) == FALSE )
		{
			SAFE_DELETE( pMover );
			SAFE_DELETE( pQueryLoad );
			return FALSE;
		}
		// mirchang_100416

		for( DWORD i = 0; i < pMover->m_Inventory.m_dwItemMax; i++ )
		{
			CItemElem* pItemElem	= pMover->m_Inventory.GetAtId( i );
			if( pItemElem )
			{
#if __VER >= 11 // __SYS_COLLECTING
				map<DWORD, CONV_RESULT_ITEM>::iterator it	= mConv.find( pItemElem->m_dwItemId );
#else	// __SYS_COLLECTING
				map<DWORD, DWORD>::iterator it	= mConv.find( pItemElem->m_dwItemId );
#endif	// __SYS_COLLECTING
				if( it != mConv.end() )
				{
					bUpdate		= TRUE;
#if __VER >= 11 // __SYS_COLLECTING
					pItemElem->m_dwItemId	= it->second.dwItemId;
					pItemElem->SetOption( it->second.nAbilityOption );
#else	// __SYS_COLLECTING
					pItemElem->m_dwItemId	= it->second;
#endif	// __SYS_COLLECTING
				}
			}
		}

		if( bUpdate )
		{
			ItemContainerStruct icsInventory;
			SaveInventory( pMover, &icsInventory );

//			FILEOUT( "../queryItemid.txt", "UPDATE INVENTORY_TBL SET m_Inventory = '%s' where m_idPlayer = '%06d' and serverindex = '%02d'",
//				szInventory, pMover->m_idPlayer, g_appInfo.dwSys );
#ifdef __CONV_0906
			FILEOUT( "../INVENTORY_TBL.txt", "%02d\t%07d\t%s",
				nServer, pMover->m_idPlayer, icsInventory.szItem );
#else	// __CONV_0906
			FILEOUT( "../queryItemid.txt", "UPDATE INVENTORY_TBL SET m_Inventory = '%s' where m_idPlayer = '%07d' and serverindex = '%02d'",
					icsInventory.szItem, pMover->m_idPlayer, nServer );
#endif	// __CONV_0906
		}
		SAFE_DELETE( pMover );
	}

	SAFE_DELETE( pQueryLoad );
	return TRUE;
}

#if __VER >= 11 // __SYS_COLLECTING
BOOL CDbManager::ConvBank( map<DWORD, CONV_RESULT_ITEM> & mConv )
#else	// __SYS_COLLECTING
BOOL CDbManager::ConvBank( map<DWORD, DWORD> & mConv )
#endif	// __SYS_COLLECTING
{
	CQuery* pQueryLoad	= new CQuery;

	if( FALSE == pQueryLoad->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
	{
		SAFE_DELETE( pQueryLoad );
		AfxMessageBox( "CDbManager::ConvBank", MB_OK );
		return FALSE;
	}

	char szQuery[200]	= { 0,};
//		sprintf( szQuery, "SELECT * FROM BANK_TBL a, BANK_EXT_TBL b WHERE a.serverindex = '%02d' and a.serverindex = b.serverindex and a.m_idPlayer = b.m_idPlayer", g_appInfo.dwSys );
	sprintf( szQuery, "SELECT * FROM BANK_TBL a, BANK_EXT_TBL b WHERE a.serverindex = b.serverindex and a.m_idPlayer = b.m_idPlayer" );

	if( FALSE == pQueryLoad->Exec( szQuery ) )
	{
		SAFE_DELETE( pQueryLoad );
		AfxMessageBox( "Exec", MB_OK );
		return FALSE;
	}

#ifdef __CONV_0906
	FILEOUT( "../BANK_TBL.txt", "serverindex\tm_idPlayer\tm_Bank" );
#endif	// __CONV_0906

	while( pQueryLoad->Fetch() )
	{
		BOOL bUpdate	= FALSE;
		CMover* pMover	= new CMover;
		int nServer	= pQueryLoad->GetInt( "serverindex" );
		pMover->m_idPlayer	= pQueryLoad->GetInt( "m_idPlayer" );
		GetBankMover( pMover, pQueryLoad, 0 );

		for( DWORD i = 0; i < pMover->m_Bank[0].m_dwItemMax; i++ )
		{
			CItemElem* pItemElem	= pMover->m_Bank[0].GetAtId( i );
			if( pItemElem )
			{
#if __VER >= 11 // __SYS_COLLECTING
				map<DWORD, CONV_RESULT_ITEM>::iterator it	= mConv.find( pItemElem->m_dwItemId );
#else	// __SYS_COLLECTING
				map<DWORD, DWORD>::iterator it	= mConv.find( pItemElem->m_dwItemId );
#endif	// __SYS_COLLECTING
				if( it != mConv.end() )
				{
					bUpdate	= TRUE;
#if __VER >= 11 // __SYS_COLLECTING
					pItemElem->m_dwItemId	= it->second.dwItemId;
					pItemElem->SetOption( it->second.nAbilityOption );
#else	// __SYS_COLLECTING
					pItemElem->m_dwItemId	= it->second;
#endif	// __SYS_COLLECTING
				}
			}
		}
		if( bUpdate )
		{
			ItemContainerStruct icsBank;
			SaveBank( pMover, &pMover->m_Bank[0], &icsBank );

//			FILEOUT( "../queryItemId.txt", "UPDATE BANK_TBL SET m_Bank = '%s' where m_idPlayer = '%06d' and serverindex = '%02d'",
//				szBank, pMover->m_idPlayer, g_appInfo.dwSys );
#ifdef __CONV_0906
			FILEOUT( "../BANK_TBL.txt", "%02d\t%07d\t%s",
				nServer, pMover->m_idPlayer, icsBank.szItem );
#else	// __CONV_0906
			FILEOUT( "../queryItemId.txt", "UPDATE BANK_TBL SET m_Bank = '%s' where m_idPlayer = '%07d' and serverindex = '%02d'",
					icsBank.szItem, pMover->m_idPlayer, nServer );
#endif	// __CONV_0906
		}
		SAFE_DELETE( pMover );
	}

	SAFE_DELETE( pQueryLoad );
	return TRUE;
}

#if __VER >= 11 // __SYS_COLLECTING
BOOL CDbManager::ConvGuildBank( map<DWORD, CONV_RESULT_ITEM> & mConv )
#else	// __SYS_COLLECTING
BOOL CDbManager::ConvGuildBank( map<DWORD, DWORD> & mConv )
#endif	// __SYS_COLLECTING
{
	CQuery* pQueryChar	= new CQuery;

	if( FALSE == pQueryChar->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
	{
		SAFE_DELETE( pQueryChar );
		AfxMessageBox( "CDbManager::ConvGuildBank", MB_OK );
		return FALSE;
	}

	char szQuery[200]	= { 0,};

//	sprintf( szQuery, "SELECT * FROM GUILD_BANK_TBL a, GUILD_BANK_EXT_TBL b WHERE a.serverindex = '%02d' and a.serverindex = b.serverindex and a.m_idGuild = b.m_idGuild", g_appInfo.dwSys );
	sprintf( szQuery, "SELECT * FROM GUILD_BANK_TBL a, GUILD_BANK_EXT_TBL b WHERE a.serverindex = b.serverindex and a.m_idGuild = b.m_idGuild" );
	if( FALSE == pQueryChar->Exec( szQuery ) )
	{
		SAFE_DELETE( pQueryChar );
		AfxMessageBox( szQuery, MB_OK );
		return FALSE;
	}

#ifdef __CONV_0906
	FILEOUT( "../GUILD_BANK_TBL.txt", "serverindex\tm_idGuild\tm_GuildBank" );
#endif	// __CONV_0906

	while( pQueryChar->Fetch() )
	{
		BOOL	bUpdate	= FALSE;
		CItemContainer<CItemElem>	GuildBank;
		int	nGuildId	= 0;
		
		GuildBank.SetItemContainer( ITYPE_ITEM, MAX_GUILDBANK );

		int nServer	= pQueryChar->GetInt( "serverindex" );
		nGuildId	= pQueryChar->GetInt( "m_idGuild" );
		GetGuildBank( &GuildBank, pQueryChar );

		for( DWORD i = 0; i < GuildBank.m_dwItemMax; i++ )
		{
			CItemElem* pItemElem	= GuildBank.GetAtId( i );
			if( pItemElem )
			{
#if __VER >= 11 // __SYS_COLLECTING
				map<DWORD, CONV_RESULT_ITEM>::iterator it	= mConv.find( pItemElem->m_dwItemId );
#else	// __SYS_COLLECTING
				map<DWORD, DWORD>::iterator it	= mConv.find( pItemElem->m_dwItemId );
#endif	// __SYS_COLLECTING
				if( it != mConv.end() )
				{
					bUpdate	= TRUE;
#if __VER >= 11 // __SYS_COLLECTING
					pItemElem->m_dwItemId	= it->second.dwItemId;
					pItemElem->SetOption( it->second.nAbilityOption );
#else	// __SYS_COLLECTING
					pItemElem->m_dwItemId	= it->second;
#endif	// __SYS_COLLECTING
				}
			}
		}

		if( bUpdate )
		{
			ItemContainerStruct icsGuildBank;
			SaveGuildBank( &GuildBank, &icsGuildBank );
			
//			FILEOUT( "../queryItemid.txt", "UPDATE GUILD_BANK_TBL SET m_GuildBank = '%s' where m_idGuild = '%06d' and serverindex = '%02d'",
//				szBank, nGuildId, g_appInfo.dwSys );
#ifdef __CONV_0906
			FILEOUT( "../GUILD_BANK_TBL.txt", "%02d\t%06d\t%s",
				nServer, nGuildId, icsGuildBank.szItem );
#else	// __CONV_0906
			FILEOUT( "../queryItemid.txt", "UPDATE GUILD_BANK_TBL SET m_GuildBank = '%s' where m_idGuild = '%06d' and serverindex = '%02d'",
					icsGuildBank.szItem, nGuildId, nServer );
#endif	// __CONV_0906
		}
	}
	
	SAFE_DELETE( pQueryChar );
	return TRUE;
}

BOOL CDbManager::ItemRemove0203( LPCSTR lpFileName )
{
	CScanner s;
	if( FALSE == s.Load( lpFileName ) )
	{
		AfxMessageBox( lpFileName );
		return FALSE;
	}
	
	map<SERIALNUMBER, DWORD>	mapItemRemove;

	DWORD dwID;
	SERIALNUMBER iSerialNumber;

	dwID	= s.GetNumber();
	while( s.tok != FINISHED )
	{
		iSerialNumber	= s.GetSerialNumber();
		bool bResult	= mapItemRemove.insert( map<SERIALNUMBER, DWORD>::value_type( iSerialNumber, dwID ) ).second;
		if( !bResult )
		{
			AfxMessageBox( "mapItemRemove.insert" );
			return FALSE;
		}
		dwID	= s.GetNumber();
	}

	CQuery* pQuery	= new CQuery;

	if( FALSE == pQuery->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
	{
		SAFE_DELETE( pQuery );
		AfxMessageBox( "CDbManager::ItemRemove", MB_OK );
		return FALSE;
	}
	
	char* szSql		= new char[40960];

	sprintf( szSql, "select a.m_idPlayer, a.serverindex, a.m_Inventory, a.m_apIndex, a.m_dwObjIndex, a.m_adwEquipment, b.m_extInventory, b.m_InventoryPiercing  from INVENTORY_TBL a, INVENTORY_EXT_TBL b, CHARACTER_TBL c where a.m_idPlayer = b.m_idPlayer and b.m_idPlayer = c.m_idPlayer and a.serverindex = b.serverindex and b.serverindex = c.serverindex and a.serverindex = '%02d'" , g_appInfo.dwSys );

	if( FALSE == pQuery->Exec( szSql ) )
	{
		AfxMessageBox( szSql, MB_OK );
		SAFE_DELETE( pQuery );
		SAFE_DELETE_ARRAY( szSql );
		return FALSE;
	}

	while( pQuery->Fetch() )
	{
		bool f	= FALSE;
		CMover* pMover	= new CMover;
		pMover->m_idPlayer	= pQuery->GetInt( "m_idPlayer" );
			
		LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus	= NULL;
		//GetInventory( pMover, pQuery, lpDbOverlappedPlus );
		// mirchang_100416 VERIFYSTRING use return value
		if( GetInventory( pMover, pQuery, lpDbOverlappedPlus ) == FALSE )
		{
			SAFE_DELETE( pMover );
			SAFE_DELETE( pQuery );
			SAFE_DELETE_ARRAY( szSql );
			return FALSE;
		}
		// mirchang_100416

		int cbInventory	= pMover->m_Inventory.m_dwItemMax;
		for( int i = 0; i < cbInventory; i++ )
		{
			CItemElem* pItemElem	= pMover->m_Inventory.GetAtId( i );
			if( pItemElem )
			{
				map<SERIALNUMBER, DWORD>::iterator i	= mapItemRemove.find( pItemElem->GetSerialNumber() );
				if( i != mapItemRemove.end() )
				{
					if( pItemElem->m_dwItemId == i->second )
					{
						FILEOUT( "ir.txt", "inventory: idPlayer = %d, dwID = %d, liSerialNumber = %d", pMover->m_idPlayer, pItemElem->m_dwItemId, pItemElem->GetSerialNumber() );
						pMover->m_Inventory.RemoveAtId( pItemElem->m_dwObjId );
						f	= true;
					}
				}
			}
		}
		if( f )
		{
			ItemContainerStruct icsInventory;
			SaveInventory( pMover, &icsInventory );

			sprintf( szSql, "UPDATE INVENTORY_TBL"
				" SET m_Inventory = '%s', m_apIndex = '%s', m_dwObjIndex = '%s'"
				" where m_idPlayer = '%07d' and serverindex = '%02d'",
				icsInventory.szItem, icsInventory.szIndex, icsInventory.szObjIndex, pMover->m_idPlayer, g_appInfo.dwSys );
		//			TRACE( "%s\n", szSql );
			
			FILE* file;
			if( ( file = fopen( MakeFileNameDate( "../query.txt" ),"a" ) ) )
				fprintf( file, "\n%s\n", szSql );

			sprintf( szSql, "UPDATE INVENTORY_EXT_TBL"
				" SET m_extInventory = '%s', m_InventoryPiercing = '%s'"
				" where m_idPlayer = '%07d' and serverindex = '%02d'",
				icsInventory.szExt, icsInventory.szPiercing, pMover->m_idPlayer, g_appInfo.dwSys );
		//			TRACE( "%s\n", szSql );
			if( file )
				fprintf( file, "\n%s\n", szSql );
			fclose( file );
		}
		SAFE_DELETE( pMover );
	}

	pQuery->Clear();

	sprintf( szSql, "select a.m_idPlayer, a.serverindex, a.m_Bank, a.m_apIndex_Bank, a.m_dwObjIndex_Bank, b.m_extBank, b.m_BankPiercing  from BANK_TBL a, BANK_EXT_TBL b where a.m_idPlayer = b.m_idPlayer and a.serverindex = b.serverindex and b.serverindex = '%02d'"
		, g_appInfo.dwSys );

	if( FALSE == pQuery->Exec( szSql ) )
	{
		AfxMessageBox( szSql, MB_OK );
		SAFE_DELETE( pQuery );
		SAFE_DELETE_ARRAY( szSql );
		return FALSE;
	}

	while( pQuery->Fetch() )
	{
		bool f	= false;
		CMover* pMover	= new CMover;
		pMover->m_idPlayer	= pQuery->GetInt( "m_idPlayer" );
		GetBankMover( pMover, pQuery, 0 );

		int cbBank	= pMover->m_Bank[0].m_dwItemMax;
		for( int i = 0; i < cbBank; i++ )
		{
			CItemElem* pItemElem	= pMover->m_Bank[0].GetAtId( i );
			if( pItemElem )
			{
				map<SERIALNUMBER, DWORD>::iterator i	= mapItemRemove.find( pItemElem->GetSerialNumber() );
				if( i != mapItemRemove.end() )
				{
					if( pItemElem->m_dwItemId == i->second )
					{
						FILEOUT( "ir.txt", "bank: idPlayer = %d, dwID = %d, liSerialNumber = %d", pMover->m_idPlayer, pItemElem->m_dwItemId, pItemElem->GetSerialNumber() );
						pMover->m_Bank[0].RemoveAtId( pItemElem->m_dwObjId );
						f	= true;
					}
				}
			}
		}
		if( f )
		{
			ItemContainerStruct icsBank;
			SaveBank( pMover, &pMover->m_Bank[0], &icsBank );

			sprintf( szSql, "UPDATE BANK_TBL"
				" SET m_Bank = '%s', m_apIndex_Bank = '%s', m_dwObjIndex_Bank = '%s'"
				" where m_idPlayer = '%07d' and serverindex = '%02d'",
				icsBank.szItem, icsBank.szIndex, icsBank.szObjIndex, pMover->m_idPlayer, g_appInfo.dwSys );
	//			TRACE( "%s\n", szSql );
			FILE* file;
			if( ( file = fopen( MakeFileNameDate( "../query.txt" ),"a" ) ) )
				fprintf( file, "\n%s\n", szSql );

			sprintf( szSql, "UPDATE BANK_EXT_TBL"
				" SET m_extBank = '%s', m_BankPiercing = '%s'"
				" where m_idPlayer = '%07d' and serverindex = '%02d'",
				icsBank.szExt, icsBank.szPiercing, pMover->m_idPlayer, g_appInfo.dwSys );
	//			TRACE( "%s\n", szSql );
			if( file )
				fprintf( file, "\n%s\n", szSql );
		}
		safe_delete( pMover );
	}

	pQuery->Clear();

//
	sprintf( szSql, "select a.m_idGuild, a.m_GuildBank,"
		" b.m_extGuildBank , b.m_GuildBankPiercing"
		" from "
		" GUILD_BANK_TBL a, GUILD_BANK_EXT_TBL b"
		" where a.serverindex = b.serverindex"
		" and b.serverindex = '%02d'"
		" and a.m_idGuild = b.m_idGuild"
		, g_appInfo.dwSys );

	if( FALSE == pQuery->Exec( szSql ) )
	{
		AfxMessageBox( szSql, MB_OK );
		SAFE_DELETE( pQuery );
		SAFE_DELETE_ARRAY( szSql );
		return FALSE;
	}

	while( pQuery->Fetch() )
	{
		bool f	= false;
		CItemContainer<CItemElem>	GuildBank;
		int	nGuildId	= 0;
		
		GuildBank.SetItemContainer( ITYPE_ITEM, MAX_GUILDBANK );

		nGuildId	= pQuery->GetInt( "m_idGuild" );
		GetGuildBank( &GuildBank, pQuery );

		int cbGuildBank	= GuildBank.m_dwItemMax;
		for( int i = 0; i < cbGuildBank; i++ )
		{
			CItemElem* pItemElem	= GuildBank.GetAtId( i );
			if( pItemElem )
			{
				map<SERIALNUMBER, DWORD>::iterator i	= mapItemRemove.find( pItemElem->GetSerialNumber() );
				if( i != mapItemRemove.end() )
				{
					if( pItemElem->m_dwItemId == i->second )
					{
						FILEOUT( "ir.txt", "guildbank: idGuild = %d, dwID = %d, liSerialNumber = %d", nGuildId, pItemElem->m_dwItemId, pItemElem->GetSerialNumber() );
						GuildBank.RemoveAtId( pItemElem->m_dwObjId );
						f	= true;
					}
				}
			}
		}
		if( f )
		{
			ItemContainerStruct icsGuildBank;
			SaveGuildBank( &GuildBank, &icsGuildBank );
					
			sprintf( szSql, "UPDATE GUILD_BANK_TBL"
				" SET m_GuildBank = '%s'"
				" where m_idGuild = '%06d' and serverindex = '%02d'",
				icsGuildBank.szItem, nGuildId, g_appInfo.dwSys );
	//		TRACE( "%s\n", szSql );
//			FILEOUT( "query.txt", szSql );
			FILE* file;
			if( ( file = fopen( MakeFileNameDate( "../query.txt" ),"a" ) ) )
				fprintf( file, "\n%s\n", szSql );
			
				
			sprintf( szSql, "UPDATE GUILD_BANK_EXT_TBL"
				" SET m_extGuildBank = '%s', m_GuildBankPiercing = '%s'"
				" where m_idGuild = '%06d' and serverindex = '%02d'",
				icsGuildBank.szExt, icsGuildBank.szPiercing, nGuildId, g_appInfo.dwSys );
	//		TRACE( "%s\n", szSql );
			if( file )
				fprintf( file, "\n%s\n", szSql );
		}
	}

	SAFE_DELETE( pQuery );
	SAFE_DELETE_ARRAY( szSql );
	return TRUE;
}

#ifdef __GETMAILREALTIME
BOOL CDbManager::QueryGetMailRealTime( CQuery* pQuery )
{
	if( NULL == pQuery )
		return FALSE;

	char szQuery[QUERY_SIZE]	= { 0, };
	sprintf( szQuery, "MAIL_STR_REALTIME 'S1', '%02d'", g_appInfo.dwSys );
	if( FALSE == pQuery->Exec( szQuery ) )
	{
		AfxMessageBox( szQuery );
		return FALSE;
	}

	__MAIL_REALTIME OneMail;
	vector< __MAIL_REALTIME > vecMailRT;

	while( pQuery->Fetch() )
	{
		u_long idReceiver	= pQuery->GetInt( "idReceiver" );
		CMail* pMail	= new CMail;
		OneMail.nMail_Before = pQuery->GetInt( "nMail" );
		pMail->m_idSender	= pQuery->GetInt( "idSender" );
		int nGoldFlag	= pQuery->GetInt( "GoldFlag" );
		pMail->m_nGold	= 0;
		if( nGoldFlag == 0 )
			pMail->m_nGold	= pQuery->GetInt( "nGold" );
		pMail->m_tmCreate	= (time_t)pQuery->GetInt( "tmCreate" );
		pMail->m_byRead	= (BYTE)pQuery->GetInt( "byRead" );

		//	mulcom	BEGIN100420	
		//pQuery->GetStr( "szTitle", pMail->m_szTitle );
		char	szTempTitle[128];
		::memset( szTempTitle, 0, sizeof(szTempTitle) );
		pQuery->GetStr( "szTitle", szTempTitle );

		::memcpy( pMail->m_szTitle, szTempTitle, sizeof(pMail->m_szTitle) );
		pMail->m_szTitle[MAX_MAILTITLE-1] = '\0';
		//	mulcom	END100420	

		pQuery->GetStr( "szText", pMail->m_szText );

		int nItemFlag	= pQuery->GetInt( "ItemFlag" );
		DWORD dwItemId	= pQuery->GetInt( "dwItemId" );

		if( dwItemId && nItemFlag == 0 )
		{
			ItemProp* pItemProp	= prj.GetItemProp( dwItemId );
			if( !pItemProp )
			{
				Error( "CDbManager::QueryGetMailRealTime: Not ItemProp = %d", dwItemId );
				continue;
			}
			pMail->m_pItemElem	= new CItemElem;
			pMail->m_pItemElem->m_dwItemId	= dwItemId;
			GetItemFromMail( pQuery, pMail->m_pItemElem );
			
			if( CheckValidItem( dwItemId, pMail->m_pItemElem->m_nItemNum ) == FALSE )
			{
				SAFE_DELETE( pMail );
				continue;
			}

			CPost *pPost    = CPost::GetInstance();

			pPost->m_csPost.Enter();
			OneMail.nMail_After	= CPost::GetInstance()->AddMail( idReceiver, pMail );

			pPost->m_csPost.Leave();
			CDPTrans::GetInstance()->SendPostMail( TRUE, idReceiver, pMail );

			vecMailRT.push_back( OneMail );
		}
	}

	for( DWORD i = 0 ; i < vecMailRT.size() ; ++i )
	{
		sprintf( szQuery, "MAIL_STR_REALTIME 'U1', '%02d', %d, %d, '%07d', %d, '%s', %d, %d, %d", 
							g_appInfo.dwSys, vecMailRT[i].nMail_Before, vecMailRT[i].nMail_After, 0, 0, "", 0, vecMailRT[i].m_liSerialNumber, vecMailRT[i].m_nHitPoint );

		if( FALSE == pQuery->Exec( szQuery ) )
		{
			AfxMessageBox( szQuery );
			return FALSE;
		}
	}
	
	return TRUE;
}
#endif // __GETMAILREALTIME

BOOL CDbManager::QueryRemoveGuildBankTbl( void )
{
	if( NULL == m_apQuery[0] )
		return FALSE;
/*
	uspLoadRemoveItemFromGuildBank
	Parameter :
		@pserverindex char(2)
	Return : 
		idGuild char(6),
		nNum int,
		ItemIndex varchar(32),
		ItemSerialNum int,
		ItemCnt int,
		DeleteCnt int
*/
	char pszQuery[100]	= { 0, };
	sprintf( pszQuery, "uspLoadRemoveItemFromGuildBank @pserverindex = '%02d'", g_appInfo.dwSys );
	if( FALSE == m_apQuery[0]->Exec( pszQuery ) )
	{
		Error( "query: %s", pszQuery );
		return FALSE;
	}
	while( m_apQuery[0]->Fetch() )
	{
		int nNo	= (int)m_apQuery[0]->GetInt( "nNum" );
		u_long idGuild	= (u_long)m_apQuery[0]->GetInt( "idGuild" );

		char pszItemIndex[100]	= { 0, };
		m_apQuery[0]->GetStr( "ItemIndex", pszItemIndex );
		DWORD dwItemId	= 0;
		if( lstrcmp( pszItemIndex, "penya" ) )	// item
			dwItemId	= atoi( pszItemIndex );
		SERIALNUMBER iSerialNumber	= (SERIALNUMBER)m_apQuery[0]->GetSerialNumber( "ItemSerialNum" );
		DWORD dwItemNum	= (DWORD)m_apQuery[0]->GetInt( "DeleteRequestCnt" );
		CDPTrans::GetInstance()->SendQueryRemoveGuildBankTbl( nNo, idGuild, dwItemId, iSerialNumber, dwItemNum );
#ifdef _DEBUG
		char lpOutputString[200];
		sprintf( lpOutputString, "TRANS: QueryRemoveGuildBankTbl: nNo = %d, idGuild=%d, dwItemId=%d, liSerialNumber=%d, nItemNum=%d", nNo, idGuild, dwItemId, iSerialNumber, dwItemNum );
		OutputDebugString( lpOutputString );
#endif	// _DEBUG
	}
	return TRUE;
}

void CDbManager::DeleteRemoveGuildBankTbl( CQuery* pQueryChar, LPDB_OVERLAPPED_PLUS lpov )
{
	int nNo;
	DWORD dwRemoved;
	CAr ar( lpov->lpBuf, lpov->uBufSize );
	ar >> nNo >> dwRemoved;
/*
	uspRemoveItemFromGuildBank:
		Parameter :
			@pserverindex char(2)
			@pNum int
		Return :
			0 : not exist
			1 : success
*/
	char szQuery[QUERY_SIZE]	= { 0,};
	sprintf( szQuery, "uspRemoveItemFromGuildBank @pserverindex = '%02d', @pNum = %d, @pDeleteCnt=%d", g_appInfo.dwSys, nNo, dwRemoved );
#ifdef _DEBUG
		OutputDebugString( szQuery );
#endif	// _DEBUG
	if( FALSE == pQueryChar->Exec( szQuery ) )
		Error( "%s", szQuery );
	FreeRequest( lpov );
}

void CDbManager::call_uspLoggingQuest( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov )
{
	u_long idPlayer;
	int nQuest, nState;
	char pszComment[100]	= { 0, };
	CAr ar( pov->lpBuf, pov->uBufSize );
	ar >> idPlayer >> nQuest >> nState;
	ar.ReadString( pszComment, 100 );
/*
	uspLoggingQuest:
			Parameter :
          @pserverindex
           @pidPlayer
           @pQuestIndex
           @pState
           @Comment
		Return :
*/
	char szQuery[QUERY_SIZE]	= { 0,};
	sprintf( szQuery, "uspLoggingQuest @pserverindex='%02d', @pidPlayer='%07d', @pQuestIndex=%d, @pState=%d, @pComment='%s'", g_appInfo.dwSys, idPlayer, nQuest, nState, pszComment );
#ifdef _DEBUG
	OutputDebugString( szQuery );
#endif	// _DEBUG
	if( FALSE == pQuery->Exec( szQuery ) )
		Error( "%s", szQuery );
	FreeRequest( pov );
}

void CDbManager::call_uspXXXMultiServer( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov )
{
	u_long uKey, idPlayer;
	CAr ar( pov->lpBuf, pov->uBufSize );
	ar >> uKey >> idPlayer;

	char szQuery[QUERY_SIZE]	= { 0,};

	if( uKey > 0 )
	{
		int i	= (int)uKey;// / 100;
		if( idPlayer > 0 )	//			uspChangeMultiServer @pserverindex(char), @pidPlayer (char), @pMultiServer(int)
			sprintf( szQuery, "uspChangeMultiServer @pserverindex='%02d', @pidPlayer='%07d', @pMultiServer=%d", g_appInfo.dwSys, idPlayer, i );
		else	// uspInitMultiServer @pserverindex(char), @pMultiServer(int)
			sprintf( szQuery, "uspInitMultiServer @pserverindex='%02d', @pMultiServer=%d", g_appInfo.dwSys, i );
	}
	else
	{
		if( idPlayer > 0 )	// o
			sprintf( szQuery, "uspChangeMultiServer @pserverindex='%02d', @pidPlayer='%07d', @pMultiServer=%d", g_appInfo.dwSys, idPlayer, 0 );
	}
#ifdef _DEBUG
	OutputDebugString( szQuery );
#endif	// _DEBUG
	if( FALSE == pQuery->Exec( szQuery ) )
		Error( "%s", szQuery );
	FreeRequest( pov );
}

DB_OVERLAPPED_PLUS*		CDbManager::AllocRequest( void )
{
#ifdef __TRANS_0413
	DB_OVERLAPPED_PLUS* ptr	= new DB_OVERLAPPED_PLUS;
	memset( ptr, 0, sizeof(DB_OVERLAPPED_PLUS) );
	return ptr;
#else	// __TRANS_0413
	//	return	m_pDbIOData->Alloc();
	DB_OVERLAPPED_PLUS* ptr	= m_pDbIOData->Alloc();
	memset( ptr, 0, sizeof(DB_OVERLAPPED_PLUS) );
	return ptr;
#endif	// __TRANS_0413
}
void	CDbManager::FreeRequest( DB_OVERLAPPED_PLUS* pOverlappedPlus )
{
#ifdef __TRANS_0413
	SAFE_DELETE_ARRAY( pOverlappedPlus->lpBuf );
	SAFE_DELETE( pOverlappedPlus );
#else	// __TRANS_0413
	SAFE_DELETE_ARRAY( pOverlappedPlus->lpBuf );
	m_pDbIOData->Free( pOverlappedPlus );
#endif	// __TRANS_0413
}

void	CDbManager::MakeRequest( DB_OVERLAPPED_PLUS* pOverlappedPlus,  LPBYTE lpBuf, u_long uBufSize )
{
	//	mulcom	BEGIN100415
	if( uBufSize < 0 || uBufSize >= (1024*1024) )
	{
		Error( _T( "MakeRequest uBufSize Error. [%d]" ), uBufSize );
		
		uBufSize = 0;
	}
	//	mulcom	END100415

#ifdef __TRANS_0413
	pOverlappedPlus->lpBuf	= new BYTE[uBufSize];
#else
	pOverlappedPlus->lpBuf	= new BYTE[uBufSize];
	::memset( pOverlappedPlus->lpBuf, 0, uBufSize );
#endif	// __TRANS_0413


	memcpy( pOverlappedPlus->lpBuf, lpBuf, uBufSize );
	pOverlappedPlus->uBufSize	= uBufSize;

	return;
}

void CDbManager::CalluspPetLog( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov )
{
	CAr ar( pov->lpBuf, pov->uBufSize );
	
	u_long idPlayer;
	SERIALNUMBER iSerial;
	DWORD dwData;
	int nType;
	CPet pet;

	ar >> idPlayer >> iSerial >> dwData >> nType;
	pet.Serialize( ar );

	char szQuery[QUERY_SIZE]	= { 0,};
	sprintf( szQuery, "uspPetLog "
		"@serverindex='%02d', @idPlayer='%07d', @iSerial=%d, @nType=%d, @dwData=%d, @nKind=%d,"
		"@nLevel=%d, @wLife=%d, @wEnergy=%d, @dwExp=%d,"
		"@nAvailParam1=%d, @nAvailParam2=%d, @nAvailParam3=%d, @nAvailParam4=%d, @nAvailParam5=%d",
		g_appInfo.dwSys, idPlayer, iSerial, nType, dwData, pet.GetKind(), pet.GetLevel(), pet.GetLife(), pet.GetEnergy(), pet.GetExp(),
		pet.GetAvailLevel( PL_D ), pet.GetAvailLevel( PL_C ), pet.GetAvailLevel( PL_B ), pet.GetAvailLevel( PL_A ), pet.GetAvailLevel( PL_S )
	 );

#ifdef _DEBUG
	TRACE( "%s\n", szQuery );
#endif	// _DEBUG

	pQuery->Exec( szQuery );

	FreeRequest( pov );
}

BOOL CDbManager::RestorePetInventory( map<DWORD, int> & mRestore )
{
	OutputDebugString( "start - RestorePetInventory" );
	CQuery* pQueryLoad	= new CQuery;

	if( FALSE == pQueryLoad->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
	{
		SAFE_DELETE( pQueryLoad );
		AfxMessageBox( "CDbManager::RestorePetInventory", MB_OK );
		return FALSE;
	}

	char szQuery[200]	= { 0,};
//	sprintf( szQuery, "SELECT * FROM INVENTORY_TBL a, INVENTORY_EXT_TBL b WHERE a.serverindex = '%02d' and b.serverindex = a.serverindex and a.m_idPlayer = b.m_idPlayer", g_appInfo.dwSys );
	sprintf( szQuery, "SELECT * FROM INVENTORY_TBL a, INVENTORY_EXT_TBL b WHERE b.serverindex = a.serverindex and a.m_idPlayer = b.m_idPlayer" );

	if( FALSE == pQueryLoad->Exec( szQuery ) )
	{
		SAFE_DELETE( pQueryLoad );
		AfxMessageBox( "Exec", MB_OK );
		return FALSE;
	}

	while( pQueryLoad->Fetch() )
	{
		int nServer	= pQueryLoad->GetInt( "serverindex" );
		BOOL bUpdate	= FALSE;
		CMover* pMover	= new CMover;
		pMover->m_idPlayer	= pQueryLoad->GetInt( "m_idPlayer" );

		//GetInventory( pMover, pQueryLoad, NULL );
		// mirchang_100416 VERIFYSTRING use return value
		if( GetInventory( pMover, pQueryLoad, NULL ) == FALSE )
		{
			SAFE_DELETE( pMover );
			SAFE_DELETE( pQueryLoad );
			return FALSE;
		}
		// mirchang_100416

		for( DWORD i = 0; i < pMover->m_Inventory.m_dwItemMax; i++ )
		{
			CItemElem* pItemElem	= pMover->m_Inventory.GetAtId( i );
			if( pItemElem )
			{
				map<DWORD, int>::iterator it	= mRestore.find( pItemElem->GetSerialNumber() );
				if( it != mRestore.end() )
				{
					if( pItemElem->m_pPet )
					{
						int nLife	= it->second;
						if( pItemElem->IsFlag( CItemElem::expired ) )
						{
							pItemElem->m_byFlag	&= ~CItemElem::expired;
							nLife--;	// 사망이 리셋되므로 생명 1추가와 같다.
						}
						pItemElem->m_pPet->SetLife( pItemElem->m_pPet->GetLife() + nLife );
						pItemElem->m_pPet->SetEnergy( pItemElem->m_pPet->GetMaxEnergy() );
						bUpdate		= TRUE;
						mRestore.erase( it );
					}
				}
			}
		}

		if( bUpdate )
		{
			ItemContainerStruct icsInventory;
			SaveInventory( pMover, &icsInventory );
			FILEOUT( "../queryRestorePet.txt", "UPDATE INVENTORY_TBL SET m_Inventory = '%s' where m_idPlayer = '%07d' and serverindex = '%02d'",
				icsInventory.szItem, pMover->m_idPlayer, nServer );
			FILEOUT( "../queryRestorePet.txt", "UPDATE INVENTORY_EXT_TBL SET szInventoryPet = '%s' where m_idPlayer = '%07d' and serverindex = '%02d'",
				icsInventory.szPet, pMover->m_idPlayer, nServer );
		}
		SAFE_DELETE( pMover );
	}
	SAFE_DELETE( pQueryLoad );

	OutputDebugString( "end - RestorePetInventory" );
	return TRUE;
}

BOOL CDbManager::RestorePetBank( map<DWORD, int> & mRestore )
{
	OutputDebugString( "start - RestorePetBank" );

	CQuery* pQueryLoad	= new CQuery;

	if( FALSE == pQueryLoad->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
	{
		SAFE_DELETE( pQueryLoad );
		AfxMessageBox( "CDbManager::RestorePetBank", MB_OK );
		return FALSE;
	}

	char szQuery[200]	= { 0,};
//		sprintf( szQuery, "SELECT * FROM BANK_TBL a, BANK_EXT_TBL b WHERE a.serverindex = '%02d' and a.serverindex = b.serverindex and a.m_idPlayer = b.m_idPlayer", g_appInfo.dwSys );
	sprintf( szQuery, "SELECT * FROM BANK_TBL a, BANK_EXT_TBL b WHERE a.serverindex = b.serverindex and a.m_idPlayer = b.m_idPlayer" );

	if( FALSE == pQueryLoad->Exec( szQuery ) )
	{
		SAFE_DELETE( pQueryLoad );
		AfxMessageBox( "Exec", MB_OK );
		return FALSE;
	}

	while( pQueryLoad->Fetch() )
	{
		BOOL bUpdate	= FALSE;
		CMover* pMover	= new CMover;
		int nServer	= pQueryLoad->GetInt( "serverindex" );
		pMover->m_idPlayer	= pQueryLoad->GetInt( "m_idPlayer" );
		GetBankMover( pMover, pQueryLoad, 0 );

		for( DWORD i = 0; i < pMover->m_Bank[0].m_dwItemMax; i++ )
		{
			CItemElem* pItemElem	= pMover->m_Bank[0].GetAtId( i );
			if( pItemElem )
			{
				map<DWORD, int>::iterator it	= mRestore.find( pItemElem->GetSerialNumber() );
				if( it != mRestore.end() )
				{
					if( pItemElem->m_pPet )
					{
						int nLife	= it->second;
						if( pItemElem->IsFlag( CItemElem::expired ) )
						{
							pItemElem->m_byFlag	&= ~CItemElem::expired;
							nLife--;	// 사망이 리셋되므로 생명 1추가와 같다.
						}
						pItemElem->m_pPet->SetLife( pItemElem->m_pPet->GetLife() + nLife );
						pItemElem->m_pPet->SetEnergy( pItemElem->m_pPet->GetMaxEnergy() );
						bUpdate		= TRUE;
						mRestore.erase( it );
					}
				}
			}
		}
		if( bUpdate )
		{
			ItemContainerStruct icsBank;
			SaveBank( pMover, &pMover->m_Bank[0], &icsBank );
			FILEOUT( "../queryRestorePet.txt", "UPDATE BANK_TBL SET m_Bank = '%s' where m_idPlayer = '%07d' and serverindex = '%02d'",
				icsBank.szItem, pMover->m_idPlayer, nServer );
			FILEOUT( "../queryRestorePet.txt", "UPDATE BANK_EXT_TBL SET szBankPet = '%s' where m_idPlayer = '%07d' and serverindex = '%02d'",
				icsBank.szPet, pMover->m_idPlayer, nServer );
		}
		SAFE_DELETE( pMover );
	}

	SAFE_DELETE( pQueryLoad );

	OutputDebugString( "end - RestorePetBank" );
	return TRUE;
}

BOOL CDbManager::RestorePetGuildBank( map<DWORD, int> & mRestore )
{
//	return TRUE;
	OutputDebugString( "start - RestorePetGuildBank" );

	CQuery* pQueryChar	= new CQuery;

	if( FALSE == pQueryChar->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
	{
		SAFE_DELETE( pQueryChar );
		AfxMessageBox( "CDbManager::RestorePetGuildBank", MB_OK );
		return FALSE;
	}

	char szQuery[1024]	= { 0,};

//	sprintf( szQuery, "SELECT * FROM GUILD_BANK_TBL a, GUILD_BANK_EXT_TBL b WHERE a.serverindex = '%02d' and a.serverindex = b.serverindex and a.m_idGuild = b.m_idGuild", g_appInfo.dwSys );
	sprintf( szQuery, "SELECT * FROM GUILD_BANK_TBL a, GUILD_BANK_EXT_TBL b WHERE a.serverindex = b.serverindex and a.m_idGuild = b.m_idGuild" );
	if( FALSE == pQueryChar->Exec( szQuery ) )
	{
		SAFE_DELETE( pQueryChar );
		AfxMessageBox( szQuery, MB_OK );
		return FALSE;
	}

	while( pQueryChar->Fetch() )
	{
		BOOL	bUpdate	= FALSE;
		CItemContainer<CItemElem>	GuildBank;
		int	nGuildId	= 0;
		
		GuildBank.SetItemContainer( ITYPE_ITEM, MAX_GUILDBANK );

		int nServer	= pQueryChar->GetInt( "serverindex" );
		nGuildId	= pQueryChar->GetInt( "m_idGuild" );
		GetGuildBank( &GuildBank, pQueryChar );

		for( DWORD i = 0; i < GuildBank.m_dwItemMax; i++ )
		{
			CItemElem* pItemElem	= GuildBank.GetAtId( i );
			if( pItemElem )
			{
				map<DWORD, int>::iterator it	= mRestore.find( pItemElem->GetSerialNumber() );
				if( it != mRestore.end() )
				{
					if( pItemElem->m_pPet )
					{
						int nLife	= it->second;
						if( pItemElem->IsFlag( CItemElem::expired ) )
						{
							pItemElem->m_byFlag	&= ~CItemElem::expired;
							nLife--;	// 사망이 리셋되므로 생명 1추가와 같다.
						}
						pItemElem->m_pPet->SetLife( pItemElem->m_pPet->GetLife() + nLife );
						pItemElem->m_pPet->SetEnergy( pItemElem->m_pPet->GetMaxEnergy() );
						bUpdate		= TRUE;
						mRestore.erase( it );
					}
				}
			}
		}
		if( bUpdate )
		{
			ItemContainerStruct icsGuildBank;
			SaveGuildBank( &GuildBank, &icsGuildBank );
			FILEOUT( "../queryRestorePet.txt", "UPDATE GUILD_BANK_TBL SET m_GuildBank = '%s' where m_idGuild = '%06d' and serverindex = '%02d'",
				icsGuildBank.szItem, nGuildId, nServer );
			FILEOUT( "../queryRestorePet.txt", "UPDATE GUILD_BANK_EXT_TBL SET szGuildBankPet = '%s' where m_idGuild = '%06d' and serverindex = '%02d'",
				icsGuildBank.szPet, nGuildId, nServer );
		}
	}
	
	SAFE_DELETE( pQueryChar );

	OutputDebugString( "end - RestorePetGuildBank" );
	return TRUE;
}

#ifdef __RT_1025
void	CDbManager::LoadMessenger( CMover* pMover, CQuery* pQuery )
{
	pMover->m_RTMessenger.clear();
	char szSQL[100]	= { 0,};
	sprintf( szSQL, "uspLoadMessenger '%02d', '%07d'" , g_appInfo.dwSys, pMover->m_idPlayer );
	if( !pQuery->Exec( szSQL ) )
		return;
	u_long idFriend;
	Friend f;
	int nSize	= 0;
	while( pQuery->Fetch() )
	{
		idFriend	= (u_long)pQuery->GetInt( "idFriend" );
		f.bBlock	= (DWORD)pQuery->GetInt( "bBlock" );
		if( ++nSize > MAX_FRIEND )
		{
			Error( "TOO MANY FRIEND: %07d", pMover->m_idPlayer );
			break;
		}
		pMover->m_RTMessenger.SetFriend( idFriend, &f );
	}
}

void CDbManager::AddMessenger( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov )
{
	u_long idPlayer, idFriend;
	CAr ar( pov->lpBuf, pov->uBufSize );
	ar >> idPlayer >> idFriend;
	// uspAddMessenger '01', '0000001', '0000002'

	char szSQL[100]	= { 0,};
	sprintf( szSQL, "uspAddMessenger '%02d', '%07d', '%07d'", g_appInfo.dwSys, idPlayer, idFriend );
	if( !pQuery->Exec( szSQL ) )
	{
		// error
	}
	FreeRequest( pov );
}

void CDbManager::DeleteMessenger( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov )
{
	u_long idPlayer, idFriend;
	CAr ar( pov->lpBuf, pov->uBufSize );
	ar >> idPlayer >> idFriend;
	// uspDeleteMessenger '01', '0000001', '0000002'

	char szSQL[100]	= { 0,};
	sprintf( szSQL, "uspDeleteMessenger '%02d', '%07d', '%07d'", g_appInfo.dwSys, idPlayer, idFriend );
	if( !pQuery->Exec( szSQL ) )
	{
		// error
	}
	FreeRequest( pov );
}

void CDbManager::UpdateMessenger( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov )
{
	u_long idPlayer, idFriend;
	BOOL bBlock;
	CAr ar( pov->lpBuf, pov->uBufSize );
	ar >> idPlayer >> idFriend >> bBlock;
	// uspUpdateMessenger '01', '0000001', '0000002, %d'

	char szSQL[100]	= { 0,};
	sprintf( szSQL, "uspUpdateMessenger '%02d', '%07d', '%07d', %d", g_appInfo.dwSys, idPlayer, idFriend, bBlock );
	if( !pQuery->Exec( szSQL ) )
	{
		// error
	}
	FreeRequest( pov );
}
#endif	// __RT_1025

#if __VER >= 11 // __GUILD_COMBAT_1TO1
void CDbManager::LoadGC1to1TenderGuild( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus, DPID dpId )
{
	vector< CGuildCombat1to1Mng::__GC1TO1TENDER > vecTenderGuild;
	vector< CGuildCombat1to1Mng::__GC1TO1TENDER > vecTenderFailGuild;
	
	char szQuery[QUERY_SIZE] = {0,};
	// 인덱스 얻어오기
	sprintf( szQuery, "usp_Guild_Combat_1to1_CombatID '%02d'", g_appInfo.dwSys );
	if( FALSE == pQuery->Exec( szQuery ) )
	{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return; }
	
	if( pQuery->Fetch() )
	{
		prj.m_GuildCombat1to1.m_nCombatId = pQuery->GetInt( "CombatID" );	// 가장 최근 combatId
		
		// 입찰 길드
		sprintf( szQuery, "usp_Guild_Combat_1to1_Tender '%02d', %d, '%06d', %d, '%c', '%s'",
				g_appInfo.dwSys, prj.m_GuildCombat1to1.m_nCombatId, 0, 0, ' ', "S1" );
		if( FALSE == pQuery->Exec( szQuery ) )
		{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return; }
		while( pQuery->Fetch() )	// 입찰 길드 목록
		{
			CGuildCombat1to1Mng::__GC1TO1TENDER gc1to1Tender;
			gc1to1Tender.ulGuildId = pQuery->GetInt( "m_idGuild" );
			gc1to1Tender.nPenya = pQuery->GetInt( "m_nPenya" );

			vecTenderGuild.push_back( gc1to1Tender );
		}
		for( DWORD i=0; i<vecTenderGuild.size(); i++ )	// 입찰 길드 선수 목록
		{
			sprintf( szQuery, "usp_Guild_Combat_1to1_Battle_Person '%02d', %d, '%06d', '%07d', %d, '%c', '%s'",
					g_appInfo.dwSys, prj.m_GuildCombat1to1.m_nCombatId, vecTenderGuild[i].ulGuildId, 0, 0, ' ', "S1" );
			if( FALSE == pQuery->Exec( szQuery ) )
			{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return; }
			while( pQuery->Fetch() )
				vecTenderGuild[i].vecMemberId.push_back( pQuery->GetInt( "m_idPlayer" ) );
		}

		// 입찰 실패 길드
		sprintf( szQuery, "usp_Guild_Combat_1to1_Tender '%02d', %d, '%06d', %d, '%c', '%s'",
				g_appInfo.dwSys, prj.m_GuildCombat1to1.m_nCombatId, 0, 0, ' ', "S2" );
		if( FALSE == pQuery->Exec( szQuery ) )
		{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return; }
		while( pQuery->Fetch() )
		{
			CGuildCombat1to1Mng::__GC1TO1TENDER gc1to1TenderFail;
			gc1to1TenderFail.ulGuildId = pQuery->GetInt( "m_idGuild" );
			gc1to1TenderFail.nPenya = pQuery->GetInt( "m_nPenya" );

			vecTenderFailGuild.push_back( gc1to1TenderFail );
		}
		
		if( vecTenderGuild.size() == 0 )
			prj.m_GuildCombat1to1.m_nCombatId++;
		
		CDPTrans::GetInstance()->SendGC1to1TenderGuild( vecTenderGuild, vecTenderFailGuild, dpId );

		FreeRequest( lpDbOverlappedPlus );
	}
}

void CDbManager::GC1to1State( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	ar >> prj.m_GuildCombat1to1.m_nState;
	if( prj.m_GuildCombat1to1.m_nState == CGuildCombat1to1Mng::GC1TO1_CLOSE )
		prj.m_GuildCombat1to1.m_nCombatId++;
	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::GC1to1Tender( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	
	char	cGu;
	u_long	uIdGuild;
	int		nPenya;
	char	cState;
	ar >> cGu >> uIdGuild >> nPenya >> cState;

	char szQuery[QUERY_SIZE] = {0,};
	
	switch( cGu )
	{
		case 'I' :
			sprintf( szQuery, "usp_Guild_Combat_1to1_Tender '%02d', %d, '%06d', %d, '%c', 'I1'",
					g_appInfo.dwSys, prj.m_GuildCombat1to1.m_nCombatId, uIdGuild, nPenya, cState );
			break;

		case 'U' :
			sprintf( szQuery, "usp_Guild_Combat_1to1_Tender '%02d', %d, '%06d', %d, '%c', 'U1'",
					g_appInfo.dwSys, prj.m_GuildCombat1to1.m_nCombatId, uIdGuild, nPenya, cState );
			break;
	}

	if( pQuery->Exec( szQuery ) == FALSE )
	{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return; }
	
	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::GC1to1LineUp( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	
	u_long	uIdGuild, uPlayerId;
	int nVecSize;
	vector<u_long> vecMemberId;
	
	ar >> uIdGuild >> nVecSize;
	for( int i=0; i<nVecSize; i++ )
	{
		ar >> uPlayerId;
		vecMemberId.push_back( uPlayerId );
	}

	char szQuery[QUERY_SIZE] = {0,};
	sprintf( szQuery, "usp_Guild_Combat_1to1_Battle_Person '%02d', %d, '%06d', '%07d', %d, '%c', '%s'",
						g_appInfo.dwSys, prj.m_GuildCombat1to1.m_nCombatId, uIdGuild, 0, 0, ' ', "D1" );
	if( pQuery->Exec( szQuery ) == FALSE )
	{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return; }

	for( DWORD i=0; i<vecMemberId.size(); i++ )
	{
		sprintf( szQuery, "usp_Guild_Combat_1to1_Battle_Person '%02d', %d, '%06d', '%07d', %d, '%c', '%s'",
						g_appInfo.dwSys, prj.m_GuildCombat1to1.m_nCombatId, uIdGuild, vecMemberId[i], i+1, 'N', "I1" );
		if( pQuery->Exec( szQuery ) == FALSE )
		{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return; }
	}

	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::GC1to1WarPerson( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	
	u_long	uIdGuild, uPlayerId;
	char cState;
		
	ar >> uIdGuild >> uPlayerId >> cState;
	
	char szQuery[QUERY_SIZE] = {0,};
	sprintf( szQuery, "usp_Guild_Combat_1to1_Battle_Person '%02d', %d, '%06d', '%07d', %d, '%c', '%s'",
						g_appInfo.dwSys, prj.m_GuildCombat1to1.m_nCombatId, uIdGuild, uPlayerId, 0, cState, "U1" );
	if( pQuery->Exec( szQuery ) == FALSE )
	{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return; }

	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::GC1to1WarGuild( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	
	DWORD dwWorldId;
	u_long uGuildId_0, uGuildId_1;
	char cState;
		
	ar >> dwWorldId >> uGuildId_0 >> uGuildId_1 >> cState;
	
	char szQuery[QUERY_SIZE] = {0,};
	switch( cState )
	{
		case 'W':
			sprintf( szQuery, "usp_Guild_Combat_1to1_Battle '%02d', %d, %d, '%06d', '%06d', '%c', '%s'",
						g_appInfo.dwSys, prj.m_GuildCombat1to1.m_nCombatId, dwWorldId, uGuildId_0, uGuildId_1, cState, "I1" );
			break;

		case 'E':
			sprintf( szQuery, "usp_Guild_Combat_1to1_Battle '%02d', %d, %d, '%06d', '%06d', '%c', '%s'",
						g_appInfo.dwSys, prj.m_GuildCombat1to1.m_nCombatId, dwWorldId, uGuildId_0, uGuildId_1, cState, "U1" );
			break;
	}
	
	if( pQuery->Exec( szQuery ) == FALSE )
	{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return; }

	FreeRequest( lpDbOverlappedPlus );
}
#endif // __GUILD_COMBAT_1TO1

#if __VER >= 11 // __MA_VER11_04	// 길드 창고 로그 기능 world,database,neuz
void CDbManager::GuildBankLogView( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	BYTE	byListType;
	u_long	idReceiver,g_uIdofMulti;
	DWORD	idGuild;

	ar >> idReceiver;
	ar >> idGuild;
	ar >> byListType;
	ar >> g_uIdofMulti;			

	DPID	dpidMulti = lpDbOverlappedPlus->dpid;

	
	char szQuery[QUERY_SIZE] = {0,};
	switch( byListType )
	{
		case GI_LOG_VIEW_ITEM_ADD:
			sprintf( szQuery, "RANKING.RANKING_DBF.dbo.usp_guildbank_log_view '%s','%06d','%02d'",
						 "S1", idGuild ,g_appInfo.dwSys );
			break;
		case GI_LOG_VIEW_ITEM_REMOVE:
			sprintf( szQuery, "RANKING.RANKING_DBF.dbo.usp_guildbank_log_view '%s','%06d','%02d'",
						 "S2", idGuild ,g_appInfo.dwSys );
			break;
		case GI_LOG_VIEW_MONEY_ADD:
			sprintf( szQuery, "RANKING.RANKING_DBF.dbo.usp_guildbank_log_view '%s','%06d','%02d'",
						 "S3", idGuild ,g_appInfo.dwSys );
			break;
		case GI_LOG_VIEW_MONEY_REMOVE:
			sprintf( szQuery, "RANKING.RANKING_DBF.dbo.usp_guildbank_log_view '%s','%06d','%02d'",
						 "S4", idGuild ,g_appInfo.dwSys );
			break;
	}
	if( pQuery->Exec( szQuery ) == FALSE )
	{ 
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); 
	}
	else
	{
//		BEFORESENDSOLE( out, PACKETTYPE_GUILDLOG_VIEW, dpidMulti);
		BEFORESENDDUAL( out, PACKETTYPE_GUILDLOG_VIEW, DPID_UNKNOWN, DPID_UNKNOWN);
		int		nCount = 0,m_Item = 0,Item_count = 0,nAbilityOption = 0;
		CTime	date;

		u_long	idPlayer;

		u_long uOffset	= out.GetOffset();
		out << nCount;	
		out << byListType;	
		out << idReceiver;	

		char szDate[13] = {0,};
		while( pQuery->Fetch() )
		{
			idPlayer = pQuery->GetInt( "m_idPlayer" );		// 
			char szSender[MAX_PLAYER]	= "unknown";
	#if __VER >= 11 // __SYS_PLAYER_DATA
			CPlayerDataCenter::GetInstance()->m_Access.Enter();
			const char* pszPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( idPlayer );
			if( pszPlayer )
				lstrcpy( szSender, pszPlayer );
			CPlayerDataCenter::GetInstance()->m_Access.Leave();
	#else	// __SYS_PLAYER_DATA
			s_cs.Enter();
			map<u_long, string>::iterator i	= m_ulong2str.find( idPlayer );
			if( i != m_ulong2str.end() )
				lstrcpy( szSender, i->second.data() );
			s_cs.Leave();
	#endif	// __SYS_PLAYER_DATA

			pQuery->GetStr( "s_date", szDate );				//  
			m_Item = pQuery->GetInt( "m_Item" );		// 
			nAbilityOption = pQuery->GetInt( "m_nAbilityOption" );		// 
			Item_count = pQuery->GetInt( "Item_count" );		// 

			GetStrTime( &date, szDate );

			out.WriteString( szSender );
			out << (long)date.GetTime(); 
			out << m_Item;
			out << nAbilityOption;
			out << Item_count;

			nCount++;
		}

		int nBufferSize;
		BYTE* pBuffer	= out.GetBuffer( &nBufferSize );
		*(UNALIGNED int*)( pBuffer + uOffset )	= nCount;
		SEND( out, CDPTrans::GetInstance(), dpidMulti );
//		SEND( out, CDPTrans::GetInstance(), g_uIdofMulti );
	}
	FreeRequest( lpDbOverlappedPlus );

}
#endif //__MA_VER11_04	// 길드 창고 로그 기능 world,database,neuz
#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
void CDbManager::SealChar( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	u_long	idReceiver,g_uIdofMulti;
	TCHAR	lpszAccount[MAX_ACCOUNT];

	ar >> idReceiver;
	ar >> g_uIdofMulti;			
	ar.ReadString( lpszAccount, MAX_ACCOUNT );

	DPID	dpidMulti = lpDbOverlappedPlus->dpid;

	
	char szQuery[QUERY_SIZE] = {0,};
	sprintf( szQuery, "uspLoadSealChar '%02d','%s'",
						 g_appInfo.dwSys, lpszAccount);
	if( pQuery->Exec( szQuery ) == FALSE )
	{ 
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); 
	}
	else
	{
//		BEFORESENDSOLE( out, PACKETTYPE_GUILDLOG_VIEW, dpidMulti);
		BEFORESENDDUAL( out, PACKETTYPE_SEALCHAR_REQ, DPID_UNKNOWN, DPID_UNKNOWN);
		int		nCount = 0;
		u_long	idPlayer,playerslot;

		u_long uOffset	= out.GetOffset();
		out << nCount;	
		out << idReceiver;	

		char szDate[13] = {0,};
		while( pQuery->Fetch() )
		{
			playerslot = pQuery->GetInt( "playerslot" );		// 
			idPlayer = pQuery->GetInt( "m_idPlayer" );		// 
			char szName[MAX_PLAYER]	= "unknown";
			pQuery->GetStr( "m_szName", szName );				//  
			out << playerslot;
			out << idPlayer;
			out.WriteString( szName );

			nCount++;
		}

		int nBufferSize;
		BYTE* pBuffer	= out.GetBuffer( &nBufferSize );
		*(UNALIGNED int*)( pBuffer + uOffset )	= nCount;
		SEND( out, CDPTrans::GetInstance(), dpidMulti );
//		SEND( out, CDPTrans::GetInstance(), g_uIdofMulti );
	}
	FreeRequest( lpDbOverlappedPlus );
}
void CDbManager::SealCharConm( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	u_long	idReceiver,g_uIdofMulti;

	ar >> idReceiver;
	ar >> g_uIdofMulti;			

	DPID	dpidMulti = lpDbOverlappedPlus->dpid;

	
	char szQuery[QUERY_SIZE] = {0,};
	sprintf( szQuery, "uspSetSealChar '%02d','%07d'",
						 g_appInfo.dwSys, idReceiver);
	if( pQuery->Exec( szQuery ) == FALSE )
	{ 
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); 
	}
	FreeRequest( lpDbOverlappedPlus );
}
void CDbManager::SealCharGet( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	u_long	idReceiver,g_uIdofMulti;
	TCHAR	lpszAccount[MAX_ACCOUNT];
	DWORD	dwGetID;

	ar >> idReceiver;
	ar >> g_uIdofMulti;			
	ar.ReadString( lpszAccount, MAX_ACCOUNT );
	ar >> dwGetID;

	DPID	dpidMulti = lpDbOverlappedPlus->dpid;

	
	char szQuery[QUERY_SIZE] = {0,};
	sprintf( szQuery, "uspLoadSealChar '%02d','%s'",
						 g_appInfo.dwSys, lpszAccount);
	if( pQuery->Exec( szQuery ) == FALSE )
	{ 
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); 
	}
	else
	{
//		BEFORESENDSOLE( out, PACKETTYPE_GUILDLOG_VIEW, dpidMulti);
		BEFORESENDDUAL( out, PACKETTYPE_SEALCHARGET_REQ, DPID_UNKNOWN, DPID_UNKNOWN);
		int		nCount = 0;
		u_long	idPlayer,playerslot;

		u_long uOffset	= out.GetOffset();
		out << nCount;	
		out << idReceiver;	
		out << dwGetID;	

		char szDate[13] = {0,};
		while( pQuery->Fetch() )
		{
			playerslot = pQuery->GetInt( "playerslot" );		// 
			idPlayer = pQuery->GetInt( "m_idPlayer" );		// 
			char szName[MAX_PLAYER]	= "unknown";
			pQuery->GetStr( "m_szName", szName );				//  
			out << playerslot;
			out << idPlayer;
			out.WriteString( szName );

			nCount++;
		}

		int nBufferSize;
		BYTE* pBuffer	= out.GetBuffer( &nBufferSize );
		*(UNALIGNED int*)( pBuffer + uOffset )	= nCount;
		SEND( out, CDPTrans::GetInstance(), dpidMulti );
//		SEND( out, CDPTrans::GetInstance(), g_uIdofMulti );
	}
	FreeRequest( lpDbOverlappedPlus );
}
void CDbManager::SealCharSet( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	u_long	idReceiver,g_uIdofMulti;
	LONG	lSetPlayerSlot;
	TCHAR	lpszAccount[MAX_ACCOUNT];
	DWORD	dwSealPlayerID;
	ar >> idReceiver;
	ar >> g_uIdofMulti;			
	ar.ReadString( lpszAccount, MAX_ACCOUNT );
	ar >> lSetPlayerSlot;			
	ar >> dwSealPlayerID;			

	DPID	dpidMulti = lpDbOverlappedPlus->dpid;

	
	char szQuery[QUERY_SIZE] = {0,};
	sprintf( szQuery, "uspSetSealCharUpdate '%02d','%07d','%s','%d','%07d'",
						 g_appInfo.dwSys, idReceiver,lpszAccount,lSetPlayerSlot,dwSealPlayerID);
	if( pQuery->Exec( szQuery ) == FALSE )
	{ 
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); 
	}
	FreeRequest( lpDbOverlappedPlus );
}

#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
void CDbManager::LoginProtectCert( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	
	char lpszAccount[MAX_ACCOUNT] = {0, };
	char lpszPlayer[MAX_PLAYER]	= {0, };
	u_long idPlayer;
	int nBankPW;

	arRead.ReadString( lpszAccount, MAX_ACCOUNT );
	arRead.ReadString( lpszPlayer, MAX_PLAYER );
	arRead >> idPlayer >> nBankPW;

	char szQuery[QUERY_SIZE]	= {0, };
	sprintf( szQuery, "usp_BankPW_Check '%02d', '%07d', '%04d'", g_appInfo.dwSys, idPlayer, nBankPW );

	if( FALSE == qry->Exec( szQuery ) )
	{
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );
		FreeRequest( lpDbOverlappedPlus );
		return;
	}
	int nError;
	if( qry->Fetch() )
		nError = qry->GetInt( "f_check" );

	if( nError == 0 )
		g_dpLoginSrvr.SendLoginProtect( lpszAccount, lpszPlayer, idPlayer, TRUE, lpDbOverlappedPlus->dpid );
	else
		g_dpLoginSrvr.SendLoginProtect( lpszAccount, lpszPlayer, idPlayer, FALSE, lpDbOverlappedPlus->dpid );

	FreeRequest( lpDbOverlappedPlus );
}
#endif // __2ND_PASSWORD_SYSTEM

#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
