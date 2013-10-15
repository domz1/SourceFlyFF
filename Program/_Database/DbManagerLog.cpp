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

#include "eveschool.h"

#if __VER >= 15 // __GUILD_HOUSE
#include "GuildHouse.h"
#endif // __GUILD_HOUSE

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
		if( FALSE == VerifyString( lpString, __FILE__, __LINE__, lpszPlayer, lpDbOverlappedPlus ) )		return;

#define	VERIFY_GUILD_STRING( lpString, lpszGuild )	\
if( FALSE == VerifyString( lpString, __FILE__, __LINE__, lpszGuild ) )		return;

void CDbManager::LogItem(CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	// 이곳이 Database server의 핸들러 부분이라면, 
	//	ACTION + 캐릭터1 + 캐릭터2 + 장소 + 시드1 + 시드2 + 잔액1 + 잔액2 +
	//	아이템개수1 + 아이템개수2 + ( 아이템고유번호 + 아이템이름 + 개수 )1 + .... + ( 아이템고유번호 + 아이템이름 + 개수 )2 + .....
	char chAction[2];
	char lpszPlayer1[36], lpszPlayer2[36], lpszItemText[36];
	arRead.ReadString( chAction, 2 );
	char chswitch = chAction[0];
	arRead.ReadString( lpszPlayer1, 36 );
	arRead.ReadString( lpszPlayer2, 36 );

	CString strPlayer1	= lpszPlayer1;
	CString strPlayer2	= lpszPlayer2;
	strPlayer1.Replace( '\'', ' ' );
	lstrcpy( lpszPlayer1, (LPCSTR)strPlayer1 );
	strPlayer2.Replace( '\'', ' ' );
	lstrcpy( lpszPlayer2, (LPCSTR)strPlayer2 );

	DWORD dwWorldID;
	arRead >> dwWorldID;
	
	int iSeed1, iSeed2;
	int iReMainder1, iReMainder2;
	arRead >> iSeed1 >> iSeed2;

	char szQuery[QUERY_SIZE]	= { 0,};

	switch( chswitch )
	{
	case 'T':
		{
			int nTradeID	= GetTradeNo();
			// 0. tblTradeLog. 트레이드 단위에 대한 저장입니다. (TradeID, 시간, 장소가 저장됩니다)
			call_uspLoggingTrade( qry, 0, nTradeID, dwWorldID );
			arRead >> iReMainder1 >> iReMainder2;
			u_long idPlayer1, idPlayer2;
			int nLevel1, nLevel2, nJob1, nJob2;
			arRead >> idPlayer1 >> nLevel1 >> nJob1;
			arRead >> idPlayer2 >> nLevel2 >> nJob2;
			char lpAddr1[100]	= { 0, };
			char lpAddr2[100]	= { 0, };
			arRead.ReadString( lpAddr1, 100 );
			arRead.ReadString( lpAddr2, 100 );
			// 1. tblTradeDetailLog. 트레이드의 사람별 정보입니다. (idPlayer, IP 등)
			call_uspLoggingTrade( qry, 1, nTradeID, 0, idPlayer1, iSeed1, lpAddr1, nLevel1, nJob1 );
			call_uspLoggingTrade( qry, 1, nTradeID, 0, idPlayer2, iSeed2, lpAddr2, nLevel2, nJob2 );
			u_long uSize1, uSize2;
			arRead >> uSize1 >> uSize2;
			for( DWORD i = 0; i < uSize1; i++ )
			{
				memset( lpszItemText, 0, sizeof( lpszItemText ) );
				SERIALNUMBER iSerialNumber;
				short nItemNum = 0;
				int	  nAbilityOption = 0;
				int	  nItemResist = 0;
				int	  nResistAbilityOption = 0;
				int   nNegudo = 0;
				int	  nMaxNegudo = 0;
				DWORD dwItemId	= 0;
				arRead >> dwItemId;
				arRead >> iSerialNumber;
				arRead.ReadString( lpszItemText, 36 );
				CString strItem	= lpszItemText;
				strItem.Replace( '\'', ' ' );
				lstrcpy( lpszItemText, (LPCSTR)strItem );
				arRead >> nItemNum;
				arRead >> nAbilityOption;
				arRead >> nItemResist;
				arRead >> nResistAbilityOption;
				arRead >> nNegudo;
				arRead >> nMaxNegudo;

				LogItemInfo aLogItem;
				::memset( &(aLogItem), 0, sizeof( &(aLogItem) ) );
				aLogItem.SendName = lpszPlayer1;
				aLogItem.RecvName = lpszPlayer2;
				aLogItem.WorldId = dwWorldID;
				aLogItem.Gold = iReMainder1 + iSeed1;
				aLogItem.Gold2 = iReMainder1 + iSeed1;
				aLogItem.ItemNo = iSerialNumber;
				//aLogItem.ItemName = lpszItemText;
				_tcsncpy( aLogItem.szItemName, lpszItemText, 30 );

				aLogItem.itemNumber = nItemNum;
				aLogItem.nAbilityOption = nAbilityOption;
				aLogItem.nItemResist = nItemResist;
				aLogItem.nResistAbilityOption = nResistAbilityOption;
				aLogItem.Negudo = nNegudo;
				aLogItem.MaxNegudo = nMaxNegudo;
				aLogItem.Gold_1 = iReMainder2;
				aLogItem.Action = chAction;

				arRead >> aLogItem.m_bCharged;
				arRead >> aLogItem.m_dwKeepTime;
#if __VER >= 12 // __EXT_PIERCING
				arRead >> aLogItem.nPiercedSize;
				
				int	k = 0;
				
				for( k=0; k<aLogItem.nPiercedSize; k++ )
					arRead >> aLogItem.adwItemId[k];
				arRead >> aLogItem.nUMPiercedSize;
				for( k=0; k<aLogItem.nUMPiercedSize; k++ )
					arRead >> aLogItem.adwUMItemId[k];
#else // __EXT_PIERCING
				arRead >> aLogItem.nPiercedSize;
				arRead >> aLogItem.adwItemId0;
				arRead >> aLogItem.adwItemId1;
				arRead >> aLogItem.adwItemId2;
				arRead >> aLogItem.adwItemId3;
#if __VER >= 9 // __ULTIMATE
				arRead >> aLogItem.adwItemId4;
#endif // __ULTIMATE
#endif // __EXT_PIERCING
#if __VER >= 11 // __SYS_IDENTIFY
				arRead >> aLogItem.m_iRandomOptItemId;
#else	// __SYS_IDENTIFY
				arRead >> aLogItem.m_nRandomOptItemId;
#endif	// __SYS_IDENTIFY
				
#if __VER >= 9 // __PET_0410
				arRead >> aLogItem.nPetKind;
				arRead >> aLogItem.nPetLevel;
				arRead >> aLogItem.dwPetExp;
				arRead >> aLogItem.wPetEnergy;
				arRead >> aLogItem.wPetLife;
				arRead >> aLogItem.nPetAL_D;
				arRead >> aLogItem.nPetAL_C;
				arRead >> aLogItem.nPetAL_B;
				arRead >> aLogItem.nPetAL_A;
				arRead >> aLogItem.nPetAL_S;
#endif // __PET_0410

				DBQryNewItemLog( szQuery, aLogItem );

				if ( FALSE == qry->Exec( szQuery ) )
				{
					WriteLog( "%s, %d\t%s, %s", __FILE__, __LINE__, lpszPlayer1, lpszPlayer2 );
					FreeRequest( lpDbOverlappedPlus );
					return;
				}
				// 2. tblTradeItemLog. 트레이드 대상 아이템 정보입니다. (ItemIndex, SerialNo 외)
#if __VER >= 11 // __SYS_IDENTIFY
				call_uspLoggingTrade( qry, 2, nTradeID, 0, idPlayer2, 0, "", 0, 0, dwItemId, iSerialNumber, nItemNum, nAbilityOption, nItemResist, nResistAbilityOption, aLogItem.m_iRandomOptItemId );
#else	// __SYS_IDENTIFY
				call_uspLoggingTrade( qry, 2, nTradeID, 0, idPlayer2, 0, "", 0, 0, dwItemId, iSerialNumber, nItemNum, nAbilityOption, nItemResist, nResistAbilityOption, aLogItem.m_nRandomOptItemId );
#endif	// __SYS_IDENTIFY
			}
			for( i = 0; i < uSize2; i++ )
			{
				memset( lpszItemText, 0, sizeof( lpszItemText ) );
				SERIALNUMBER iSerialNumber;
				short nItemNum = 0;
				int	  nAbilityOption = 0;
				int	  nItemResist = 0;
				int	  nResistAbilityOption = 0;
				int   nNegudo = 0;
				int		nMaxNegudo	= 0;
				DWORD dwItemId	= 0;
				arRead >> dwItemId;
				arRead >> iSerialNumber;
				arRead.ReadString( lpszItemText, 36 );
				CString strItem	= lpszItemText;
				strItem.Replace( '\'', ' ' );
				lstrcpy( lpszItemText, (LPCSTR)strItem );
				arRead >> nItemNum;
				arRead >> nAbilityOption;
				arRead >> nItemResist;
				arRead >> nResistAbilityOption;
				arRead >> nNegudo;
				arRead >> nMaxNegudo;

				LogItemInfo aLogItem;
				::memset( &(aLogItem), 0, sizeof( &(aLogItem) ) );
				aLogItem.SendName = lpszPlayer2;
				aLogItem.RecvName = lpszPlayer1;
				aLogItem.WorldId = dwWorldID;
				aLogItem.Gold = iReMainder2 + iSeed2;
				aLogItem.Gold2 = iReMainder2 + iSeed2;
				aLogItem.ItemNo = iSerialNumber;
				//aLogItem.ItemName = lpszItemText;
				_tcsncpy( aLogItem.szItemName, lpszItemText, 30 );

				aLogItem.itemNumber = nItemNum;
				aLogItem.nAbilityOption = nAbilityOption;
				aLogItem.nItemResist = nItemResist;
				aLogItem.nResistAbilityOption = nResistAbilityOption;
				aLogItem.Negudo = nNegudo;
				aLogItem.MaxNegudo	= nMaxNegudo;
				aLogItem.Gold_1 = iReMainder1;
				aLogItem.Action = chAction;
				
				arRead >> aLogItem.m_bCharged;
				arRead >> aLogItem.m_dwKeepTime;
#if __VER >= 12 // __EXT_PIERCING
				arRead >> aLogItem.nPiercedSize;
				for( int i=0; i<aLogItem.nPiercedSize; i++ )
					arRead >> aLogItem.adwItemId[i];
				arRead >> aLogItem.nUMPiercedSize;
				for( i=0; i<aLogItem.nUMPiercedSize; i++ )
					arRead >> aLogItem.adwUMItemId[i];
#else // __EXT_PIERCING
				arRead >> aLogItem.nPiercedSize;
				arRead >> aLogItem.adwItemId0;
				arRead >> aLogItem.adwItemId1;
				arRead >> aLogItem.adwItemId2;
				arRead >> aLogItem.adwItemId3;
#if __VER >= 9 // __ULTIMATE
				arRead >> aLogItem.adwItemId4;
#endif // __ULTIMATE
#endif // __EXT_PIERCING

#if __VER >= 11 // __SYS_IDENTIFY
				arRead >> aLogItem.m_iRandomOptItemId;
#else	// __SYS_IDENTIFY
				arRead >> aLogItem.m_nRandomOptItemId;
#endif	// __SYS_IDENTIFY

#if __VER >= 9 // __PET_0410
				arRead >> aLogItem.nPetKind;
				arRead >> aLogItem.nPetLevel;
				arRead >> aLogItem.dwPetExp;
				arRead >> aLogItem.wPetEnergy;
				arRead >> aLogItem.wPetLife;
				arRead >> aLogItem.nPetAL_D;
				arRead >> aLogItem.nPetAL_C;
				arRead >> aLogItem.nPetAL_B;
				arRead >> aLogItem.nPetAL_A;
				arRead >> aLogItem.nPetAL_S;
#endif // __PET_0410

				DBQryNewItemLog( szQuery, aLogItem );

				if ( FALSE == qry->Exec( szQuery ) )
				{
					WriteLog( "%s, %d\t%s, %s", __FILE__, __LINE__, lpszPlayer1, lpszPlayer2 );
					FreeRequest( lpDbOverlappedPlus );
					return;
				}
				// 2. tblTradeItemLog. 트레이드 대상 아이템 정보입니다. (ItemIndex, SerialNo 외)
#if __VER >= 11 // __SYS_IDENTIFY
				call_uspLoggingTrade( qry, 2, nTradeID, 0, idPlayer1, 0, "", 0, 0, dwItemId, iSerialNumber, nItemNum, nAbilityOption, nItemResist, nResistAbilityOption, aLogItem.m_iRandomOptItemId );
#else	// __SYS_IDENTIFY
				call_uspLoggingTrade( qry, 2, nTradeID, 0, idPlayer1, 0, "", 0, 0, dwItemId, iSerialNumber, nItemNum, nAbilityOption, nItemResist, nResistAbilityOption, aLogItem.m_nRandomOptItemId );
#endif	// __SYS_IDENTIFY
			}
			if( iSeed1 > 0 )
			{
				LogItemInfo aLogItem;
				::memset( &(aLogItem), 0, sizeof( &(aLogItem) ) );
				aLogItem.SendName = lpszPlayer1;
				aLogItem.RecvName = lpszPlayer2;
				aLogItem.WorldId = dwWorldID;
				aLogItem.Gold = iReMainder1 + iSeed1;
				aLogItem.Gold2 = iReMainder1;
				//aLogItem.ItemName = "SEED";
				_stprintf( aLogItem.szItemName, "%d", II_GOLD_SEED1 );
				aLogItem.itemNumber = iSeed1;
				aLogItem.Gold_1 = iReMainder2;
				aLogItem.Action = chAction;
				DBQryNewItemLog( szQuery, aLogItem );

				if ( FALSE == qry->Exec( szQuery ) )
				{
					WriteLog( "%s, %d\t%s, %s", __FILE__, __LINE__, lpszPlayer1, lpszPlayer2 );
					FreeRequest( lpDbOverlappedPlus );
					return;
				}
			}
			if( iSeed2 > 0 )
			{
				LogItemInfo aLogItem;
				::memset( &(aLogItem), 0, sizeof( &(aLogItem) ) );
				aLogItem.SendName = lpszPlayer2;
				aLogItem.RecvName = lpszPlayer1;
				aLogItem.WorldId = dwWorldID;
				aLogItem.Gold = iReMainder2 + iSeed2;
				aLogItem.Gold2 = iReMainder2;
				//aLogItem.ItemName = "SEED";
				_stprintf( aLogItem.szItemName, "%d", II_GOLD_SEED1 );
				aLogItem.itemNumber = iSeed2;
				aLogItem.Gold_1 = iReMainder1;
				aLogItem.Action = chAction;
				DBQryNewItemLog( szQuery, aLogItem );

				if ( FALSE == qry->Exec( szQuery ) )
				{
					WriteLog( "%s, %d\t%s, %s", __FILE__, __LINE__, lpszPlayer1, lpszPlayer2 );
					FreeRequest( lpDbOverlappedPlus );
					return;
				}	
			}
			break;
		}
	default:
		{
			memset( lpszItemText, 0, sizeof( lpszItemText ) );
			SERIALNUMBER	ItemNo;
			DWORD nItemNum = 0;
			int nNegudo = 0; // 내구도
			int nMaxNegudo	= 0; 
			int	  nAbilityOption = 0;
			DWORD dwGold_1 = 0;
			int nSlot = 0;
			int nSlot1 = 0;
			int nItemResist = 0;
			int nResistAbilityOption = 0;
			
			arRead >> ItemNo;
			arRead >> nNegudo;
			arRead >> nMaxNegudo;
			arRead.ReadString( lpszItemText, 36 );
			CString strItem	= lpszItemText;
			strItem.Replace( '\'', ' ' );
			lstrcpy( lpszItemText, (LPCSTR)strItem );
			arRead >> nItemNum;
			arRead >> nAbilityOption;
			arRead >> dwGold_1;
			arRead >> nSlot;
			arRead >> nSlot1;
			arRead >> nItemResist;
			arRead >> nResistAbilityOption;

			LogItemInfo aLogItem;
			::memset( &(aLogItem), 0, sizeof( &(aLogItem) ) );
			aLogItem.SendName = lpszPlayer1;
			aLogItem.RecvName = lpszPlayer2;
			aLogItem.WorldId = dwWorldID;
			aLogItem.Gold = iSeed1;
			aLogItem.Gold2 = iSeed2;
			aLogItem.ItemNo		= ItemNo;
			//aLogItem.ItemName = lpszItemText;
			_tcsncpy( aLogItem.szItemName, lpszItemText, 30 );

			aLogItem.itemNumber = nItemNum;
			aLogItem.nAbilityOption = nAbilityOption;
			aLogItem.nItemResist = nItemResist;
			aLogItem.nResistAbilityOption = nResistAbilityOption;
			aLogItem.Negudo = nNegudo;
			aLogItem.MaxNegudo	= nMaxNegudo;
			aLogItem.Gold_1 = dwGold_1;
			aLogItem.nSlot = nSlot;
			aLogItem.nSlot1 = nSlot1;
			aLogItem.Action = chAction;

			arRead >> aLogItem.m_bCharged;
			arRead >> aLogItem.m_dwKeepTime;
#if __VER >= 12 // __EXT_PIERCING
			arRead >> aLogItem.nPiercedSize;
			for( int i=0; i<aLogItem.nPiercedSize; i++ )
				arRead >> aLogItem.adwItemId[i];
			arRead >> aLogItem.nUMPiercedSize;
			for( i=0; i<aLogItem.nUMPiercedSize; i++ )
				arRead >> aLogItem.adwUMItemId[i];
#else // __EXT_PIERCING
			arRead >> aLogItem.nPiercedSize;
			arRead >> aLogItem.adwItemId0;
			arRead >> aLogItem.adwItemId1;
			arRead >> aLogItem.adwItemId2;
			arRead >> aLogItem.adwItemId3;
#if __VER >= 9 // __ULTIMATE
			arRead >> aLogItem.adwItemId4;
#endif // __ULTIMATE
#endif // __EXT_PIERCING

#if __VER >= 11 // __SYS_IDENTIFY
			arRead >> aLogItem.m_iRandomOptItemId;
#else	// __SYS_IDENTIFY
			arRead >> aLogItem.m_nRandomOptItemId;
#endif	// __SYS_IDENTIFY

#if __VER >= 9 // __PET_0410
			arRead >> aLogItem.nPetKind;
			arRead >> aLogItem.nPetLevel;
			arRead >> aLogItem.dwPetExp;
			arRead >> aLogItem.wPetEnergy;
			arRead >> aLogItem.wPetLife;
			arRead >> aLogItem.nPetAL_D;
			arRead >> aLogItem.nPetAL_C;
			arRead >> aLogItem.nPetAL_B;
			arRead >> aLogItem.nPetAL_A;
			arRead >> aLogItem.nPetAL_S;
#endif // __PET_0410
			DBQryNewItemLog( szQuery, aLogItem );

			if ( FALSE == qry->Exec( szQuery ) )
			{
				WriteLog( "LogItem %s, %d\t%s, %s", __FILE__, __LINE__, lpszPlayer1, lpszPlayer2 );
				if( strlen( szQuery ) < 4096 )
					WriteLog( " LogItem Qyery => %s", szQuery );
				FreeRequest( lpDbOverlappedPlus );
				return;
			}
			break;
		}
	}
	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::LogPlayDeath(CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	char		cPlay[MAX_PLAYER] = { 0, };					// 캐릭터 아이디
	int			iServer_No;							// 서버 번호
	DWORD		dWorld_No;							// 월드 번호
	D3DXVECTOR3	Pos;								// 좌표
	int			iLevel;								// 레벨
	int			iAttackDmg;							// 최대공격력
	int			iMaxHP;								// 최대 HP
	char		cMurder[64] = { 0, };				// 살인자 (몬스터나 캐릭, 트릭)
	u_long		uidPlayer;


	arRead.ReadString( cPlay, MAX_PLAYER );
	iServer_No	= g_appInfo.dwSys;
	arRead >> dWorld_No;
	arRead >> Pos;
	arRead >> iLevel;
	arRead >> iAttackDmg;
	arRead >> iMaxHP;
	arRead.ReadString( cMurder, 64 );
	arRead >> uidPlayer;

	
	char szQuery[QUERY_SIZE]	= { 0,};

	//	mulcom	BEGIN100407	LOG_STR L2 cMurder quotation mark 제거
	//DBQryLog( szQuery, "L2", uidPlayer, iServer_No, 0, iLevel, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'A', dWorld_No, cMurder, Pos.x, Pos.y, Pos.z, iAttackDmg, iMaxHP );
	CString	strMurderName	= cMurder;
	strMurderName.Replace( "'", " " );

	DBQryLog( szQuery, "L2", uidPlayer, iServer_No, 0, iLevel, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 'A', dWorld_No, strMurderName.GetBuffer(), Pos.x, Pos.y, Pos.z, iAttackDmg, iMaxHP );
	//	mulcom	BEGIN100407	LOG_STR L2 cMurder quotation mark 제거

	/// 레벨을 추가해야함
	if ( FALSE == qry->Exec( szQuery ) )
	{
		WriteLog( "%s, %d\t%d", __FILE__, __LINE__, uidPlayer );
		FreeRequest( lpDbOverlappedPlus );
		return;
	}

//	qry->Clear();
	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::LogLevelUp(CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	char		cPlay[17] = {0,};					// 캐릭터 아이디
	int			iServer_No;							// 서버 번호
	int			iLevel;								// 캐릭터 레벨
	int			iPlayJob;							// 직업_No
	int			iJobLevel;							// 직업 레벨
	int			iStr;								// 힘
	int			iDex;								// 민첩
	int			iInt;								// 지능
	int			iSta;								// 체력
	int			iGP;								// GP(성장포인트)
	int			iLP;								// LP(교육포인트)
	EXPINTEGER	iExp1;								// Exp(경험치)
	int			iFlightLevel;						// 비행레벨
	char		cAction;							// 행동( 몬스터를 죽여서 렙업을 하여였는지, 퀘스트를 이용하여 렙업을 하였는지를 나타냄
	u_long		uidPlayer;							
	DWORD		dwGold;
	DWORD		dwPlayerTime;	

	iServer_No	= g_appInfo.dwSys;

	arRead >> iExp1;
	arRead >> iFlightLevel;
	arRead >> iLevel;
	arRead >> iPlayJob;
	arRead >> iJobLevel;
	arRead >> iStr;
	arRead >> iDex;
	arRead >> iInt;
	arRead >> iSta;
	arRead >> iGP;
	arRead >> iLP;
	arRead >> dwGold;
	arRead >> dwPlayerTime;
	
	int iAction;
	arRead >> iAction;
	arRead >> uidPlayer;
	switch( iAction )
	{
	case 1:					// 레벨 업
		cAction = 'L';
		break;
	case 2:					// 스탯 업
		cAction = 'S';
		break;
	case 3:					// 잡(직업)레벨 업
		cAction = 'J';
		break;
	case 4:					// 전직
		cAction = 'C';
		break;
	case 5:					// 경험치 렙 5이상 20%단위
		cAction = 'E';
		break;
	case 6:
		cAction = 'D';		// 경험치 다운시 렙따
		break;
	case 7:
		cAction = 'M';		// 경험치 다운
		break;
	case 8:
		cAction = '1';		// 로드라이트
		break;
	case 9:
		cAction = '2';		// 로드스타
		break;
	case 10:
		cAction = '3';		// 누군가가 부활을 해주었따
		break;		
	default:
		{
			WriteLog( "%s, %d\t%c", __FILE__, __LINE__, iAction );
			FreeRequest( lpDbOverlappedPlus );
			return;
		}
	}

	char szQuery[QUERY_SIZE]	= { 0,};
	DBQryLog( szQuery, "L1", uidPlayer, iServer_No, iExp1, iLevel, iPlayJob, iJobLevel, iFlightLevel, iStr, iDex, iInt, iSta, iGP, iLP, cAction, 0, '\0', 0, 0, 0, dwGold, dwPlayerTime );

	if ( FALSE == qry->Exec( szQuery ) )
	{
		WriteLog( "%s, %d\t%d, %s", __FILE__, __LINE__, uidPlayer, szQuery );
		FreeRequest( lpDbOverlappedPlus );
		return;
	}

	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::LogServerDeath(CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	int			iServer_No;							// 서버 번호
	SYSTEMTIME	siStart;
		
	iServer_No	= g_appInfo.dwSys;
	arRead.Read( &siStart, sizeof(SYSTEMTIME) );

	CTime tStart( siStart );
	CString strStart = tStart.Format( "%Y%m%d%H%M%S" );
	char szQuery[QUERY_SIZE]	= { 0,};
	DBQryLog( szQuery, "L8", 0, iServer_No, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 'A',
		0, '\0', 0.0f, 0.0f, 0.0f, 0, 0,
		0, 0, 0, 0, '\0', 0, 0,
		0, strStart.GetBuffer( strStart.GetLength() ) );

	if ( FALSE == qry->Exec( szQuery ) )
	{
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );
		FreeRequest( lpDbOverlappedPlus );
		return;
	}

	FreeRequest( lpDbOverlappedPlus );
}
#if __VER >= 13 // __HONORABLE_TITLE			// 달인
void	CDbManager::LogGetHonorTime(CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	int			iServer_No;							// 서버 번호
	u_long		uidPlayer;
	int			nGetHonor;
	
	iServer_No	= g_appInfo.dwSys;
	arRead >> nGetHonor;
	arRead >> uidPlayer;

	char szQuery[QUERY_SIZE]	= { 0,};
	sprintf( szQuery,
		"uspHonorLog @serverindex='%02d',@idPlayer='%07d',@nHonor='%d'",  g_appInfo.dwSys,uidPlayer, nGetHonor );
//	qry->Clear();
	if ( FALSE == qry->Exec( szQuery ) )
	{
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );
//		qry->Clear();
		FreeRequest( lpDbOverlappedPlus );
		return;
	}

//	qry->Clear();
	FreeRequest( lpDbOverlappedPlus );;
}
#endif	// __HONORABLE_TITLE			// 달인
void CDbManager::LogUniqueItem(CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	int			iServer_No;							// 서버 번호
	char		cCrateMan[17] = {0,};				// 생성자
	int			iWorld;								// 월드번호
	D3DXVECTOR3	Pos;								// 좌표
	char		cUniqueName[51] = {0,};				// 아이템 명
	int			cItemAddLevel;						// 아이템 추가레벨
	u_long		uidPlayer;
	
	iServer_No	= g_appInfo.dwSys;
	arRead.ReadString( cCrateMan, 17 );
	arRead >> iWorld;
	arRead >> Pos;
	arRead.ReadString( cUniqueName, 51 );
	arRead >> cItemAddLevel;
	arRead >> uidPlayer;

	char szQuery[QUERY_SIZE]	= { 0,};
	DBQryLog( szQuery, "L4", uidPlayer, iServer_No, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 'A',
		iWorld, '\0', Pos.x, Pos.y, Pos.z, 0, 0, '\0', 0, 0, 0, cUniqueName, 0, 0, cItemAddLevel );

	qry->Exec( szQuery );

	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::LogQuest(CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	int Action;
	char cName[17] = {0,};
	int iServer_No;
	int nQuest;
	CTime		tQuest	= CTime::GetCurrentTime();	// 날짜
	u_long	uidPlayer;
	
	iServer_No	= g_appInfo.dwSys;
	CString strQuestTime	= tQuest.Format( "%Y%m%d%H%M%S" );

	arRead >> Action;
	arRead.ReadString( cName, 17 );
	arRead >> nQuest;
	arRead >> uidPlayer;
	
	char szQuery[QUERY_SIZE]	= { 0,};

//	qry->Clear();
	if( Action == 1 ) // 퀘스트 시작
	{
		DBQryLog( szQuery, "L6", uidPlayer, iServer_No, 0, 0, 0, 0, 0, 
			0, 0, 0, 0, 0, 0, 'C', 0, '\0', 0.0f, 0.0f, 0.0f, 0, 0, 0,
			0, 0, 0, '\0', 0, 0, 0, strQuestTime.GetBuffer( strQuestTime.GetLength() ), 0, '\0', nQuest );

		if ( FALSE == qry->Exec( szQuery ) )
		{
			WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );
			FreeRequest( lpDbOverlappedPlus );
			return;
		}

//		qry->Clear();
	}
	else	// 퀘스트 완료
	{
		DBQryLog( szQuery, "L7", uidPlayer, iServer_No, 0, 0, 0, 0, 0, 
			0, 0, 0, 0, 0, 0, 'A', 0, '\0', 0.0f, 0.0f, 0.0f, 0, 0, 0,
			0, 0, 0, '\0', 0, 0, 0, strQuestTime.GetBuffer( strQuestTime.GetLength() ), 0, '\0', nQuest );

		if ( FALSE == qry->Exec( szQuery ) )
		{
			WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );
			FreeRequest( lpDbOverlappedPlus );
			return;
		}

//		qry->Clear();
	}
	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::DBQryLog( char* qryLog, char* Gu, u_long idPlayer, int nserverindex, EXPINTEGER nExp1, int nLevel,
						  int nJob, int JobLv, int FlightLv, int nStr, int nDex,
						  int nInt, int nSta, int nRemainGP, int nRemainLP, char szState,
						  int WorldID, char* szkilled_szName, float vPos_x, float vPos_y, float vPos_z,
						  int nAttackPower, int nMax_HP, char *uGetidPlayer, DWORD dwGold, int nRemainGold,
						  int nItem_UniqueNo, char *szItem_Name, int Item_durability, int Item_count, int ItemAddLv,
						  char* szStart_Time, int TotalPlayerTime, char *szRemoteIP, int nQuest_Index, u_long uKill_idPlayer )
{
	CString strItem	= szItem_Name;
	strItem.Replace( '\'', ' ' );
	sprintf( qryLog, "LOG_STR '%s',@im_idPlayer='%07d',@iserverindex='%02d',@im_nExp1=%I64d,@im_nLevel=%d,"
		"@im_nJob=%d,@im_nJobLv=%d,@im_nFlightLv=%d,@im_nStr=%d,@im_nDex=%d,"
		"@im_nInt=%d,@im_nSta=%d,@im_nRemainGP=%d,@im_nRemainLP=%d,@iState='%c',"
		"@idwWorldID=%d,@ikilled_m_szName='%s',@im_vPos_x=%f,@im_vPos_y=%f,@im_vPos_z=%f,"
		"@iattackPower=%d,@imax_HP=%d,@im_GetszName='%s',@im_dwGold=%d,@im_nRemainGold=%d,"
		"@iItem_UniqueNo=%d,@iItem_Name='%s',@iItem_durability=%d,@iItem_count=%d,@iItem_AddLv=%d,"
		"@iStart_Time='%s',@iTotalPlayTime=%d,@iremoteIP='%s',@iQuest_Index=%d,@ikilled_m_idPlayer='%07d'",
		Gu, idPlayer, nserverindex, nExp1, nLevel,
		nJob, JobLv, FlightLv, nStr, nDex,
		nInt, nSta, nRemainGP, nRemainLP, szState,
		WorldID, szkilled_szName, vPos_x, vPos_y, vPos_z,
		nAttackPower, nMax_HP, uGetidPlayer, dwGold, nRemainGold,
//		nItem_UniqueNo, szItem_Name, Item_durability, Item_count, ItemAddLv,
		nItem_UniqueNo, (LPCSTR)strItem, Item_durability, Item_count, ItemAddLv,
		szStart_Time, TotalPlayerTime, szRemoteIP, nQuest_Index, uKill_idPlayer );	
	if( strlen(qryLog) > 40960 )
		ASSERT(0);
	
}

void CDbManager::Gamema_Chat( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	char szGamema[1024];
	u_long uidPlayer;

	arRead >> uidPlayer;
	arRead.ReadString( szGamema, 1024 );
	
	CString strBuf = szGamema;
	strBuf.Replace( "--", "" );
	strBuf.Replace( "'", "" );
	strBuf.Replace( "/*", "" );
	strBuf.Replace( "*/", "" );

	// LOG_GAMEMASTER_STR '%s','%s','%s'
	char szQuery[QUERY_SIZE]	= { 0,};
	sprintf( szQuery,
		"LOG_GAMEMASTER_STR @im_idPlayer='%07d',@iserverindex='%02d',@iszWords='%s'", uidPlayer, g_appInfo.dwSys, strBuf );

//	qry->Clear();
	if ( FALSE == qry->Exec( szQuery ) )
	{
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );
//		qry->Clear();
		FreeRequest( lpDbOverlappedPlus );
		return;
	}

//	qry->Clear();
	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::LogConcurrentUserNumber( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	
	//	qry->Clear();
	
	int cbSize;
	int anCount[64];
#ifdef __LOG_PLAYERCOUNT_CHANNEL
	int nChannel;
	ar >> nChannel;
#endif // __LOG_PLAYERCOUNT_CHANNEL
	ar >> cbSize;
	//	memset( (void*)anCount, 0, sizeof(int) * 64 );
	ar.Read( (void*)anCount, sizeof(int) * cbSize );
	//	LOG_USER_CNT_STR '00', 0, 0, 0, 0
	char szQuery[QUERY_SIZE]	= { 0,};
#ifdef __LOG_PLAYERCOUNT_CHANNEL
	sprintf( szQuery, "LOG_USER_CNT_STR '%02d', %d", g_appInfo.dwSys, nChannel );
#else // __LOG_PLAYERCOUNT_CHANNEL
	sprintf( szQuery, "LOG_USER_CNT_STR '%02d'", g_appInfo.dwSys );
#endif // __LOG_PLAYERCOUNT_CHANNEL
	
	char szCount[10];
	for( int i = 0; i < cbSize; i++ )
	{
		sprintf( szCount, ", %d", anCount[i] );
		strcat( szQuery, szCount );
	}
	
	qry->Exec( szQuery );
	
	//	qry->Clear();
	
	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::LogPkPvp( CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	int nSendBuf = 0;
	u_long uidPlayer, uLoseidPlayer;
	int iServer_No, iLoseServer_No;
	int nLevel, nLoseLevel;
	int nPoint;
#if __VER >= 8 // __S8_PK
	int nPKValue, nLosePKValue;
	DWORD dwPKPropensity, dwLosePKPropensity;
#else // __VER >= 8 // __S8_PK
	int nSlaughter, nLoseSlaughter;
	int nKillNum, nLoseKillNum;
#endif // __VER >= 8 // __S8_PK
	int nFame, nLoseFame;
	char chState;
	D3DXVECTOR3 vPos, vLosePos;

	arRead >> nSendBuf;
	arRead >> nPoint;
	arRead >> chState;
	
	if( nSendBuf == 1 || nSendBuf == 3 )
	{
		arRead >> uLoseidPlayer;
		arRead >> iLoseServer_No;
		arRead >> nLoseLevel;
		arRead >> vLosePos;
		arRead >> nLoseFame;
#if __VER >= 8 // __S8_PK
		arRead >> nLosePKValue;
		arRead >> dwLosePKPropensity;
#else // __VER >= 8 // __S8_PK
		arRead >> nLoseKillNum;
		arRead >> nLoseSlaughter;
#endif // __VER >= 8 // __S8_PK
	}
	
	if( nSendBuf == 2 || nSendBuf == 3 )
	{
		arRead >> uidPlayer;
		arRead >> iServer_No;
		arRead >> nLevel;
		arRead >> vPos;
		arRead >> nFame;
#if __VER >= 8 // __S8_PK
		arRead >> nPKValue;
		arRead >> dwPKPropensity;
#else // __VER >= 8 // __S8_PK
		arRead >> nKillNum;
		arRead >> nSlaughter;
#endif // __VER >= 8 // __S8_PK
	}

	char szQuery[QUERY_SIZE]	= { 0,};
#if __VER >= 8 // __S8_PK
	if( nSendBuf == 3 )
	{
		DBQryLog( szQuery, "LA", uidPlayer, g_appInfo.dwSys, nLoseLevel, nLevel, nPoint, dwPKPropensity, nPKValue,
			nFame, dwLosePKPropensity, nLoseFame, 0, 0, 0, chState, 0, '\0', vPos.x, 0.0f, vPos.z, 0, 0,
			'\0', 0, 0, 0, '\0', 0, 0, 0, 
			'\0', 0, '\0', 0, uLoseidPlayer );
	}
	else if( nSendBuf == 1 )
	{
		DBQryLog( szQuery, "LA", 0, g_appInfo.dwSys, nLoseLevel, 0, nPoint, 0, nLosePKValue,
			0, dwLosePKPropensity, nLoseFame, 0, 0, 0, chState, 0, '\0', vLosePos.x, 0.0f, vLosePos.z, 0, 0,
			'\0', 0, 0, 0, '\0', 0, 0, 0, 
			'\0', 0, '\0', 0, uLoseidPlayer );
	}
	else
	{
		DBQryLog( szQuery, "LA", uidPlayer, g_appInfo.dwSys, 0, nLevel, nPoint, dwPKPropensity, nPKValue,
			nFame, 0, 0, 0, 0, 0, chState, 0, '\0', vPos.x, 0.0f, vPos.z, 0, 0,
			'\0', 0, 0, 0, '\0', 0, 0, 0, 
			'\0', 0, '\0', 0, 0 );
	}
#else // __VER >= 8 // __S8_PK
	if( nSendBuf == 3 )
	{
		DBQryLog( szQuery, "LA", uidPlayer, g_appInfo.dwSys, nLoseLevel, nLevel, nPoint, nSlaughter, nKillNum,
			nFame, nLoseSlaughter, nLoseFame, 0, 0, 0, chState, 0, '\0', vPos.x, 0.0f, vPos.z, 0, 0,
			'\0', 0, 0, 0, '\0', 0, 0, 0, 
			'\0', 0, '\0', 0, uLoseidPlayer );
	}
	else if( nSendBuf == 1 )
	{
		DBQryLog( szQuery, "LA", 0, g_appInfo.dwSys, nLoseLevel, 0, nPoint, 0, nLoseKillNum,
			0, nLoseSlaughter, nLoseFame, 0, 0, 0, chState, 0, '\0', vLosePos.x, 0.0f, vLosePos.z, 0, 0,
			'\0', 0, 0, 0, '\0', 0, 0, 0, 
			'\0', 0, '\0', 0, uLoseidPlayer );
	}
	else
	{
		DBQryLog( szQuery, "LA", uidPlayer, g_appInfo.dwSys, 0, nLevel, nPoint, nSlaughter, nKillNum,
			nFame, 0, 0, 0, 0, 0, chState, 0, '\0', vPos.x, 0.0f, vPos.z, 0, 0,
			'\0', 0, 0, 0, '\0', 0, 0, 0, 
			'\0', 0, '\0', 0, 0 );
	}
#endif // __VER >= 8 // __S8_PK

	if ( FALSE == qry->Exec( szQuery ) )
	{
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, qry );
		FreeRequest( lpDbOverlappedPlus );
		return;
	}

	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::LogSchool( CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	
	u_long uidPlayer;
	char szName[128];
		
	arRead >> uidPlayer;
	arRead.ReadString( szName, 128 );
	
	//LOG_EVENT_STR @im_idPlayer = '000001',@iserverindex = '00'
	char szQuery[QUERY_SIZE]	= { 0,};
	sprintf( szQuery, "LOG_EVENT_STR @im_idPlayer = '%07d',@iserverindex = '%02d', @im_szName = '%s'", uidPlayer, g_appInfo.dwSys, szName );

	if ( FALSE == qry->Exec( szQuery ) )
	{
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, qry );
		FreeRequest( lpDbOverlappedPlus );
		return;
	}
			
//	qry->Clear();
		
	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::LogSkillPoint( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	
	u_long			uidPlayer;						// 플레이어 아이디
	int				nAction;						// 행동
	EXPINTEGER		nSkillExp = 0;						// 스킬경험치
	int				nSkillPoint;					// SP
	int				nSkillLevel = 0;					// 지금까지 올린 스킬레벨
	DWORD			dwSkillID;						// 스킬 아이디
	DWORD			dwLevel;						// 위스킬에 대한 레벨
	int				nPoint;							// 사용한 스킬 포인트
	
	arRead >> nAction;
	arRead >> uidPlayer;
	arRead >> nSkillLevel;
	arRead >> nSkillPoint;
	arRead >> dwSkillID;
	arRead >> dwLevel;
	arRead >> nPoint;

	char szQuery[QUERY_SIZE]	= { 0,};

	if( nAction == LOG_SKILLPOINT_GET_QUEST )
	{
		sprintf( szQuery, "uspLoggingAcquireSkillPoint '%02d','%07d',%d,%d,%d,%d,%d,%I64d,10", 
			g_appInfo.dwSys, uidPlayer, dwSkillID, dwLevel, nPoint, nSkillPoint, nSkillLevel, nSkillExp );
	}
	else if( nAction == LOG_SKILLPOINT_GET_HUNT )
	{
		sprintf( szQuery, "uspLoggingAcquireSkillPoint '%02d','%07d',%d,%d,%d,%d,%d,%I64d,11", 
			g_appInfo.dwSys, uidPlayer, dwSkillID, dwLevel, nPoint, nSkillPoint, nSkillLevel, nSkillExp );
	}
	else if( nAction == LOG_SKILLPOINT_USE )
	{
		sprintf( szQuery, "uspLoggingUsingSkillPoint '%02d','%07d',%d,%d,%d,%d,%d,%I64d,20", 
			g_appInfo.dwSys, uidPlayer, dwSkillID, dwLevel, nPoint, nSkillPoint, nSkillLevel, nSkillExp );
	}

	if ( FALSE == pQuery->Exec( szQuery ) )
	{
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, pQuery );
		FreeRequest( lpDbOverlappedPlus );
		return;
	}
	FreeRequest( lpDbOverlappedPlus );
}

void CDbManager::DBQryNewItemLog( char* qryLog, const LogItemInfo& info )
{
	sprintf( qryLog, "LOG_ITEM_STR "
		"@im_szName='%s',@iserverindex='%02d',@im_GetszName='%s',@idwWorldID=%d,@im_dwGold=%d,@im_nRemainGold=%d,"
		"@iItem_UniqueNo=%d,@iItem_Name='%s',@iItem_durability=%d,@im_nAbilityOption=%d,@Im_GetdwGold=%d,"
		"@iItem_count=%d,@iState='%s',@im_nSlot0=%d,@im_nSlot1=%d,@im_bItemResist=%d,@im_nResistAbilityOption=%d"
		",@im_bCharged=%d,@im_dwKeepTime=%d,"
#if __VER >= 11 // __SYS_IDENTIFY
		"@im_nRandomOptItemId=%I64d"
#else	// __SYS_IDENTIFY
		"@im_nRandomOptItemId=%d"
#endif	// __SYS_IDENTIFY
		",@inPiercedSize=%d,@iadwItemId0=%d,@iadwItemId1=%d,@iadwItemId2=%d,@iadwItemId3=%d"
#if __VER >= 9 // __ULTIMATE
		",@iadwItemId4=%d"
#endif // __ULTIMATE

		",@iMaxDurability=%d"

#if __VER >= 9 // __PET_0410
		",@inPetKind=%d,@inPetLevel=%d,@idwPetExp=%d,@iwPetEnergy=%d,@iwPetLife=%d"
		",@inPetAL_D=%d,@inPetAL_C=%d,@inPetAL_B=%d,@inPetAL_A=%d,@inPetAL_S=%d"
#endif // __PET_0410
#if __VER >= 12 // __EXT_PIERCING
		",@iadwItemId5=%d,@iadwItemId6=%d,@iadwItemId7=%d,@iadwItemId8=%d,@iadwItemId9=%d"
		",@inUMPiercedSize=%d,@iadwUMItemId0=%d,@iadwUMItemId1=%d,@iadwUMItemId2=%d,@iadwUMItemId3=%d,@iadwUMItemId4=%d"
#endif // __EXT_PIERCING
		,
		info.SendName, g_appInfo.dwSys, info.RecvName, info.WorldId, info.Gold, info.Gold2, 
		info.ItemNo, info.szItemName, info.Negudo, info.nAbilityOption, info.Gold_1,
		info.itemNumber, info.Action, info.nSlot, info.nSlot1, info.nItemResist, info.nResistAbilityOption
		, info.m_bCharged, info.m_dwKeepTime,
#if __VER >= 11 // __SYS_IDENTIFY
		info.m_iRandomOptItemId
#else	// __SYS_IDENTIFY
		info.m_nRandomOptItemId
#endif	// __SYS_IDENTIFY
#if __VER >= 12 // __EXT_PIERCING
		, info.nPiercedSize, info.adwItemId[0], info.adwItemId[1], info.adwItemId[2], info.adwItemId[3]
		, info.adwItemId[4]
#else // __EXT_PIERCING
		, info.nPiercedSize, info.adwItemId0, info.adwItemId1, info.adwItemId2, info.adwItemId3
#if __VER >= 9 // __ULTIMATE
		, info.adwItemId4
#endif // __ULTIMATE
#endif // __EXT_PIERCING
		, info.MaxNegudo
#if __VER >= 9 // __PET_0410
		, info.nPetKind, info.nPetLevel, info.dwPetExp, info.wPetEnergy, info.wPetLife
		, info.nPetAL_D, info.nPetAL_C, info.nPetAL_B, info.nPetAL_A, info.nPetAL_S
#endif // __PET_0410
#if __VER >= 12 // __EXT_PIERCING
		, info.adwItemId[5], info.adwItemId[6], info.adwItemId[7], info.adwItemId[8], info.adwItemId[9]
		, info.nUMPiercedSize, info.adwUMItemId[0], info.adwUMItemId[1], info.adwUMItemId[2], info.adwUMItemId[3], info.adwUMItemId[4]
#endif // __EXT_PIERCING
		);	
	if( strlen(qryLog) > 40960 )
		ASSERT(0);
///	TRACE("%s\n", qryLog);
}

#if __VER >= 11 // __SYS_IDENTIFY
BOOL CDbManager::call_uspLoggingTrade( CQuery* pQuery, int nFlag, int nTradeID, DWORD dwWorldID, u_long idPlayer, DWORD dwTradeGold, const char* lpAddr, int nLevel, int nJob, int nItemIndex, SERIALNUMBER iItemSerialNum, int nItemCnt, int nAbilityOpt, int nItemResist, int nResistAbilityOpt, __int64 iRandomOpt )
#else	// __SYS_IDENTIFY
BOOL CDbManager::call_uspLoggingTrade( CQuery* pQuery, int nFlag, int nTradeID, DWORD dwWorldID, u_long idPlayer, DWORD dwTradeGold, const char* lpAddr, int nLevel, int nJob, int nItemIndex, SERIALNUMBER iItemSerialNum, int nItemCnt, int nAbilityOpt, int nItemResist, int nResistAbilityOpt, int nRandomOpt )
#endif	// __SYS_IDENTIFY
{
	char pszQuery[1024]	= { 0, };
	switch( nFlag )
	{
		case 0:
			{
				sprintf( pszQuery, "uspLoggingTrade @pFlag=0, @pTradeID=%d, @pserverindex='%02d', @pWorldID=%d",
					nTradeID, g_appInfo.dwSys, dwWorldID );
				break;
			}
		case 1:
			{
				sprintf( pszQuery, "uspLoggingTrade @pFlag=1, @pTradeID=%d, @pserverindex='%02d', @pWorldID=%d, @pidPlayer='%07d', @pTradeGold=%d, @pPlayerIP='%s', @pLevel=%d, @pJob=%d",
					nTradeID, g_appInfo.dwSys, dwWorldID, idPlayer, dwTradeGold, lpAddr, nLevel, nJob );
				break;
			}
		case 2:
			{
				sprintf( pszQuery, "uspLoggingTrade @pFlag=2, @pTradeID=%d, @pserverindex='%02d', @pidPlayer='%07d', @pItemIndex='%d', @pItemSerialNum=%d, @pItemCnt=%d, @pAbilityOpt=%d, @pItemResist=%d, @pResistAbilityOpt=%d,"
#if __VER >= 11 // __SYS_IDENTIFY
						"@pRandomOpt=%I64d",
#else	// __SYS_IDENTIFY
						"@pRandomOpt=%d",
#endif	// __SYS_IDENTIFY
#if __VER >= 11 // __SYS_IDENTIFY
					nTradeID, g_appInfo.dwSys, idPlayer, nItemIndex, iItemSerialNum, nItemCnt, nAbilityOpt, nItemResist, nResistAbilityOpt, iRandomOpt );
#else	// __SYS_IDENTIFY
					nTradeID, g_appInfo.dwSys, idPlayer, nItemIndex, iItemSerialNum, nItemCnt, nAbilityOpt, nItemResist, nResistAbilityOpt, nRandomOpt );
#endif	// __SYS_IDENTIFY
				break;
			}
	}
	if ( FALSE == pQuery->Exec( pszQuery ) )
	{
		WriteLog( "%s, %d:\t%s", __FILE__, __LINE__, pszQuery );
		return FALSE;
	}
//	OutputDebugString( pszQuery );
	return TRUE;
}

void CDbManager::LogExpBox( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov )
{
	u_long idPlayer;
	OBJID objid;
	EXPINTEGER iExp;
	BOOL bGet;

	CAr ar( pov->lpBuf, pov->uBufSize );
	ar >> idPlayer >> objid >> iExp >> bGet;
	char pszQuery[1024]	= { 0, };

	sprintf( pszQuery, "uspLogExpBox @pserverindex='%02d', @pidPlayer='%07d', @pobjid=%d, @piExp=%I64d, @pState='%c'",
		g_appInfo.dwSys, idPlayer, objid, iExp, ( bGet? 'G': 'D' ) );

	if ( FALSE == pQuery->Exec( pszQuery ) )
		WriteLog( "%s, %d:\t%s", __FILE__, __LINE__, pszQuery );
	FreeRequest( pov );
//	OutputDebugString( pszQuery );
}

#if __VER >= 14 // __INSTANCE_DUNGEON
void CDbManager::LogInstanceDungeon( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	DWORD dwDungeonId, dwWorldId;
	u_long uChannel;
	int	nDungeonType;
	char chState;

	arRead >> dwDungeonId >> dwWorldId >> uChannel >> nDungeonType >> chState;
	
	pQuery->Execute( "usp_Log_Ins_Dungeon_Insert @serverindex = '%02d', @m_DungeonID = '%07d', @m_WorldID = %d, @m_channel = %d, @m_Type = %d, @m_State = '%c'", 
	g_appInfo.dwSys, dwDungeonId, dwWorldId, uChannel, nDungeonType, chState );
	
	FreeRequest( lpDbOverlappedPlus );
}
#endif // __INSTANCE_DUNGEON

#ifdef __ERROR_LOG_TO_DB
void CDbManager::LogError( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	u_long idPlayer, idChannel;
	char szAccount[MAX_ACCOUNT] = {0, };
	char chType;
	char szError[1024] = {0, };

	arRead >> idPlayer;
	arRead.ReadString( szAccount, MAX_ACCOUNT );
	arRead >> idChannel >> chType;
	arRead.ReadString( szError, 1024 );
	
	pQuery->Execute( "usp_SystemError_Insert @m_idPlayer = '%07d', @serverindex = '%02d', @account = %s, @nChannel = %d, @chType = '%c', @szError = '%s'", 
	idPlayer, g_appInfo.dwSys, szAccount, idChannel, chType, szError );
	
	FreeRequest( lpDbOverlappedPlus );
}
#endif // __ERROR_LOG_TO_DB

#if __VER >= 15 // __GUILD_HOUSE
void CDbManager::LogGuildFurniture( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	DWORD dwGuildId;
	GH_Fntr_Info gfi;
	char chState;

	arRead >> dwGuildId;
	gfi.Serialize( arRead );
	arRead >> chState;

	pQuery->Execute( "usp_GuildFurniture_Log @serverindex = '%02d', @m_idGuild = '%06d', @SEQ = %d, @ItemIndex = %d, @tKeepTime = %d, @bSetup = %d, @nState = '%c'", 
		g_appInfo.dwSys, dwGuildId, gfi.nSeqNum, gfi.dwItemId, gfi.tKeepTime, static_cast<int>( gfi.bSetup ), chState );

	FreeRequest( lpDbOverlappedPlus );
}
#endif // __GUILD_HOUSE